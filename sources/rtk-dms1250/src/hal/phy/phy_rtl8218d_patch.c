/*
 * Copyright (C) 2009 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 74379 $
 * $Date: 2016-12-19 11:39:00 +0800 (Mon, 19 Dec 2016) $
 *
 * Purpose : PHY 8295 Driver APIs.
 *
 * Feature : PHY 8295 Driver APIs
 *
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/debug/rt_log.h>
#include <soc/type.h>
#include <hal/common/halctrl.h>
#include <hal/mac/miim_common_drv.h>
#include <hal/phy/phy_rtl8218d.h>
#include <hal/phy/phy_construct.h>
#include <osal/time.h>
#include <ioal/ioal_param.h>

/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */
uint32 rtl8218d_0819_efuseOcpAddr[] =
    {
        0xBCDC,
        0xBCDE,
        0xBCE0,
        0xBCE2,
        0xBCAC,
        0xBCFC,
    };

/*
 * Macro Declaration
 */
#define PHY_8218D_EFAD2PORT_LE49(_ef)       ((_ef - 2) / (sizeof(rtl8218d_0819_efuseOcpAddr)/sizeof(uint32)))
#define PHY_8218D_EFAD2OcpIdx_LE49(_ef)     ((_ef - 2) % (sizeof(rtl8218d_0819_efuseOcpAddr)/sizeof(uint32)))
#define PHY_8218D_OCP2PAGE_LE49(_d)         REG32_FIELD_GET(_d, 4, 0xFFF0)
#define PHY_8218D_OCP2REG_LE49(_d)          ((REG32_FIELD_GET(_d, 0, 0x000F) / 2) + 16)
#define PHY_8218D_OCP2PAGE_GE50(_d)         REG32_FIELD_GET(_d, 3, 0xFFF8)
#define PHY_8218D_OCP2REG_GE50(_d)          (REG32_FIELD_GET(_d, 0, 0x0007) + 16)


/*
 * Function Declaration
 */


int32
_phy_8218d_efuseSw_load_phy(uint32 unit, rtk_port_t  port, uint32 page, uint32 phy_reg, uint32 data)
{
    int32       ret;

    ret = hal_miim_park_write(unit, port, page, 0x1F, phy_reg, data);

    RT_LOG(LOG_DEBUG, (MOD_HAL|MOD_SWITCH), "U%u P%u 0x%x reg 0x%x data 0x%x\n", unit, port, page, phy_reg, data);
    return ret;
}


/* Function Name:
 *      _phy_8218d_efuse_read
 * Description:
 *      Read from eFUSE
 * Input:
 *      unit      - unit id
 *      basePort  - base MAC ID of the 8218D
 *      efAddr    - address to read
 * Output:
 *      pData     - pointer buffer of read data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
_phy_8218d_efuse_read(uint32 unit, rtk_port_t basePort, uint32 efAddr, uint32 *pData)
{
    int32           ret;
    uint32          data = 0, dataReg30;
    uint32          rCnt = 0;
    uint32          maxPage;

    maxPage = HAL_MIIM_FIX_PAGE;

    /* keep reg.30 data */
    if ((ret = hal_miim_park_read(unit, basePort, 0, 0x1F, 30, &dataReg30)) != RT_ERR_OK)
        return ret;

    if ((ret = hal_miim_park_write(unit, basePort, maxPage, 0x1F, 30, 0x8)) != RT_ERR_OK)
        return ret;

    if ((ret = hal_miim_park_write(unit, basePort, maxPage, 0x1F, 31, 0x26E)) != RT_ERR_OK)
        return ret;

    if ((ret = hal_miim_park_write(unit, basePort, maxPage, 0x1F, 17, efAddr)) != RT_ERR_OK)
        return ret;

    if ((ret = hal_miim_park_write(unit, basePort, maxPage, 0x1F, 16, 0)) != RT_ERR_OK)
        return ret;


    while (1)
    {
        rCnt++;
        ret = hal_miim_park_read(unit, basePort, maxPage, 0x1F, 16, &data);
        if (ret != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_HAL), "unit %u port %u Read16", unit, basePort);
            return ret;
        }

        if (!(data & 0x0004))
        {
            break;
        }

        if (rCnt >= 10000)
        {
            ret = RT_ERR_TIMEOUT;
            RT_ERR(ret, (MOD_HAL), "unit %u port %u ReadTimeOut", unit, basePort);
            *pData = 0;
            goto ERR_RET;
        }
    }

    ret = hal_miim_park_read(unit, basePort, maxPage, 0x1F, 19, pData);
    if (ret != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HAL), "unit %u port %u", unit, basePort);
    }

  ERR_RET:
    /* recover reg 30, park page */
    hal_miim_park_write(unit, basePort, maxPage, 0, 30, dataReg30);

    RT_LOG(LOG_DEBUG, (MOD_HAL|MOD_SWITCH), "U%u P%u efE%u data 0x%04x\n", unit, basePort, efAddr, *pData);

    return ret;
}


/* Function Name:
 *      _phy_8218d_efuseSw_load_le49
 * Description:
 *      software load eFUSE patch to PHY
 * Input:
 *      unit      - unit id
 *      basePort  - base MAC ID of the 8218D
 *      entStart  - start address
 *      entEnd    - end address
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void
_phy_8218d_efuseSw_load_le49(uint32 unit, rtk_port_t basePort, uint32 entStart, uint32 entEnd)
{
    uint32          ent;
    uint32          pmsk, data, phyPort;
    uint32          phyPage, phyReg;
    rtk_port_t      port;

    if (entStart == 0)
    {
        _phy_8218d_efuse_read(unit, basePort, 0, &data);
        pmsk = data & 0xFF00; /* bit15: port0, bit14: port1, ... bit8: port7,  */
    }
    else
    {
        pmsk = 0xFF00; /* set all ports */
    }

    RT_LOG(LOG_DEBUG, (MOD_HAL|MOD_SWITCH), "U%u P%u pmsk:%x\n", unit, basePort, pmsk);

    /* sel GPHY block */
    for (phyPort = 0; phyPort < HWP_PHY_BASE_PHY_MAX(unit, basePort); phyPort++)
    {
        hal_miim_park_write(unit, (basePort + phyPort), 0, 0x1F, 30, 0x1);
    }


    for (ent = ((entStart < 2) ? 2 : entStart);
         ent <= ((entEnd > 49) ? 49 : entEnd);
         ent++)
    {
        phyPort = PHY_8218D_EFAD2PORT_LE49(ent);
        if (!((0x8000 >> phyPort) & pmsk))
            continue;

        data = rtl8218d_0819_efuseOcpAddr[PHY_8218D_EFAD2OcpIdx_LE49(ent)];
        phyPage = PHY_8218D_OCP2PAGE_LE49(data);
        phyReg =  PHY_8218D_OCP2REG_LE49(data);
        port = basePort + phyPort;

        _phy_8218d_efuse_read(unit, basePort, ent, &data);

        _phy_8218d_efuseSw_load_phy(unit, port, phyPage, phyReg, data);
    }/* end for */

    return;
}


/* Function Name:
 *      _phy_8218d_efuseSw_load_a
 * Description:
 *      software load eFUSE patch to PHY
 * Input:
 *      unit      - unit id
 *      basePort  - base MAC ID of the 8218D
 *      *pEnt     - current address
 * Output:
 *      *pEnt     - address to tne next
 * Return:
 *      None
 * Note:
 *      None
 */
void
_phy_8218d_efuseSw_load_a(uint32 unit, rtk_port_t basePort, rtk_port_t port, uint32 *pEnt, uint32 pageReg)
{
    uint32      data;
    uint32      phyPage, phyReg;

    phyPage = PHY_8218D_OCP2PAGE_GE50(pageReg);
    phyReg =  PHY_8218D_OCP2REG_GE50(pageReg);

    *pEnt = *pEnt + 1;
    _phy_8218d_efuse_read(unit, basePort, *pEnt, &data);

    _phy_8218d_efuseSw_load_phy(unit, port, phyPage, phyReg, data);

}


/* Function Name:
 *      _phy_8218d_efuseSw_load_reg30
 * Description:
 *      software load eFUSE patch to PHY
 * Input:
 *      unit      - unit id
 *      basePort  - base MAC ID of the 8218D
 *      *pEnt     - current address
 * Output:
 *      *pEnt     - address to tne next
 * Return:
 *      None
 * Note:
 *      None
 */
void
_phy_8218d_efuseSw_load_reg30(uint32 unit, rtk_port_t basePort, uint32 *pEnt, rtk_port_t *pPort)
{
    uint32      data;
    uint32      phyPort;

    *pEnt = *pEnt + 1;
    _phy_8218d_efuse_read(unit, basePort, *pEnt, &data);

    if ((data & 0x000F) == 0x8) /* [3:0] */
    {
        *pPort = basePort;
        data = 0x8;
    }
    else
    {
        phyPort = REG32_FIELD_GET(data, 4, 0x00F0); /* [7:4] */
        *pPort = basePort + phyPort;
        data = REG32_FIELD_GET(data, 0, 0x000F); /* [3:0] */
    }

    /* sel block */
    _phy_8218d_efuseSw_load_phy(unit, *pPort, 0, 30, data);
}


/* Function Name:
 *      _phy_8218d_efuseSw_load_ge50
 * Description:
 *      software load eFUSE patch to PHY
 * Input:
 *      unit      - unit id
 *      basePort  - base MAC ID of the 8218D
 *      entStart  - start address
 *      entEnd    - end address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
void
_phy_8218d_efuseSw_load_ge50(uint32 unit, rtk_port_t basePort, uint32 entStart, uint32 entEnd)
{
    uint32      ent, data;
    uint32      port = basePort;

    for (ent = ((entStart <= 50) ? 50 : entStart);
         ent <= entEnd;
         ent++)
    {
        _phy_8218d_efuse_read(unit, basePort, ent, &data);

        if (data == 0xFFFF)
        {
            continue;
        }
        else if (data == 0x0000)
        {
            break;
        }
        else if (data == 0xFE00)
        {
            _phy_8218d_efuseSw_load_reg30(unit, basePort, &ent, &port);
        }
        else
        {
            _phy_8218d_efuseSw_load_a(unit, basePort, port, &ent, data);
        }
    }/* end for */

    return;
}

/* Function Name:
 *      _phy_8218d_efuseSw_load
 * Description:
 *      software load eFUSE patch to PHY
 * Input:
 *      unit      - unit id
 *      basePort  - base MAC ID of the 8218D
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
void
_phy_8218d_efuseSw_load(uint32 unit, rtk_port_t basePort)
{
    uint32      data, phyPort;
    uint32      entStart, entEnd;
    int32       ret;

    if ((ret = _phy_8218d_efuse_read(unit, basePort, 253, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HAL), "unit %u port %u", unit, basePort);
        return;
    }
    entStart = REG32_FIELD_GET(data, 8, 0xFF00);    /* 253[15:8] */
    entEnd = REG32_FIELD_GET(data, 0, 0x00FF);      /* 253[7:0] */

    RT_LOG(LOG_DEBUG, (MOD_HAL|MOD_SWITCH), "U%u P%u LoadPhy start:%u end:%u\n", unit, basePort, entStart, entEnd);
    if (entStart <= 49)
    {
        _phy_8218d_efuseSw_load_le49(unit, basePort, entStart, entEnd);
    }

    if (entEnd >= 50)
    {
        _phy_8218d_efuseSw_load_ge50(unit, basePort, entStart, entEnd);
    }

    /* Back to GPHY block, page 0 */
    for (phyPort = 0; phyPort < HWP_PHY_BASE_PHY_MAX(unit, basePort); phyPort++)
    {
        hal_miim_park_write(unit, (basePort + phyPort), 0, 0, 30, 0x1);
    }
}


/* Function Name:
 *      phy_8218d_efuseSwLoadEnable_get
 * Description:
 *      Read from eFUSE
 * Input:
 *      unit      - unit id
 *      basePort  - base MAC ID of the 8218D
 * Output:
 *      enable    - ENABLED: swfotware load should be performed. DISABLED: no need for swfotware load
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8218d_efuseSwLoadEnable_get(uint32 unit, rtk_port_t basePort, rtk_enable_t *enable)
{
    uint32      data, bit12, bit13, bit14, bit15;
    int32       ret;

    *enable = DISABLED;
    if ((ret = _phy_8218d_efuse_read(unit, basePort, 252, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HAL), "unit %u port %u", unit, basePort);
        return ret;
    }

    bit12 = REG32_FIELD_GET(data, 12, (0x1 << 12));
    bit13 = REG32_FIELD_GET(data, 13, (0x1 << 13));
    bit14 = REG32_FIELD_GET(data, 14, (0x1 << 14));
    bit15 = REG32_FIELD_GET(data, 15, (0x1 << 15));

    *enable = bit15 ^ bit14 ^ bit13 ^ bit12;

    return RT_ERR_OK;
}


/* Function Name:
 *      phy_8218d_efuseSw_load
 * Description:
 *      software load eFUSE patch to PHY
 * Input:
 *      unit      - unit id
 *      basePort  - base MAC ID of the 8218D
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8218d_efuseSw_load(uint32 unit, rtk_port_t basePort)
{

    _phy_8218d_efuseSw_load(unit, basePort);

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8218d_sram_patch
 * Description:
 *      software patch SRAM
 * Input:
 *      unit      - unit id
 *      basePort  - base MAC ID of the 8218D
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
void
phy_8218d_sram_patch_cs(uint32 unit, rtk_port_t basePort)
{
    uint32  data, blk, port, phyPort;
    uint32  len_cbl_thr;
    uint32  len_l[PORT_NUM_IN_8218D];
    uint32  rlen_l[PORT_NUM_IN_8218D];
    uint32  len_s[PORT_NUM_IN_8218D];
    uint32  rlen_s[PORT_NUM_IN_8218D];
    int32   ret;


    if ((ret = _phy_8218d_efuse_read(unit, basePort, 252, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HAL), "unit %u port %u read fail", unit, basePort);
        return;
    }

    if (!(data & 0x0800)) /* check SW_LD_LEN */
    {
        RT_LOG(LOG_DEBUG, (MOD_HAL|MOD_PHY), "U%u P%u SW_LD_LEN is FALSE", unit, basePort);
        return;
    }

    RT_LOG(LOG_DEBUG, (MOD_HAL|MOD_PHY), "U%u P%u SW_LD_LEN is TRUE", unit, basePort);

    _phy_8218d_efuse_read(unit, basePort, 235, &len_cbl_thr);
    _phy_8218d_efuse_read(unit, basePort, 236, &len_l[0]);
    _phy_8218d_efuse_read(unit, basePort, 237, &rlen_l[0]);
    _phy_8218d_efuse_read(unit, basePort, 238, &len_l[1]);
    _phy_8218d_efuse_read(unit, basePort, 239, &rlen_l[1]);
    _phy_8218d_efuse_read(unit, basePort, 240, &len_l[2]);
    _phy_8218d_efuse_read(unit, basePort, 241, &rlen_l[2]);
    _phy_8218d_efuse_read(unit, basePort, 242, &len_l[3]);
    _phy_8218d_efuse_read(unit, basePort, 243, &rlen_l[3]);
    _phy_8218d_efuse_read(unit, basePort, 244, &len_l[4]);
    _phy_8218d_efuse_read(unit, basePort, 245, &rlen_l[4]);
    _phy_8218d_efuse_read(unit, basePort, 246, &len_l[5]);
    _phy_8218d_efuse_read(unit, basePort, 247, &rlen_l[5]);
    _phy_8218d_efuse_read(unit, basePort, 248, &len_l[6]);
    _phy_8218d_efuse_read(unit, basePort, 249, &rlen_l[6]);
    _phy_8218d_efuse_read(unit, basePort, 250, &len_l[7]);
    _phy_8218d_efuse_read(unit, basePort, 251, &rlen_l[7]);

    _phy_8218d_efuse_read(unit, basePort, 2, &len_s[0]);
    _phy_8218d_efuse_read(unit, basePort, 3, &rlen_s[0]);
    _phy_8218d_efuse_read(unit, basePort, 8, &len_s[1]);
    _phy_8218d_efuse_read(unit, basePort, 9, &rlen_s[1]);
    _phy_8218d_efuse_read(unit, basePort, 14, &len_s[2]);
    _phy_8218d_efuse_read(unit, basePort, 15, &rlen_s[2]);
    _phy_8218d_efuse_read(unit, basePort, 20, &len_s[3]);
    _phy_8218d_efuse_read(unit, basePort, 21, &rlen_s[3]);
    _phy_8218d_efuse_read(unit, basePort, 26, &len_s[4]);
    _phy_8218d_efuse_read(unit, basePort, 27, &rlen_s[4]);
    _phy_8218d_efuse_read(unit, basePort, 32, &len_s[5]);
    _phy_8218d_efuse_read(unit, basePort, 33, &rlen_s[5]);
    _phy_8218d_efuse_read(unit, basePort, 38, &len_s[6]);
    _phy_8218d_efuse_read(unit, basePort, 39, &rlen_s[6]);
    _phy_8218d_efuse_read(unit, basePort, 44, &len_s[7]);
    _phy_8218d_efuse_read(unit, basePort, 45, &rlen_s[7]);

    hal_miim_park_read(unit, basePort, 0, 0x1F, 30, &blk);
    hal_miim_park_write(unit, basePort, 0, 0x1F, 30, 0);
    for (phyPort = 0; phyPort < PORT_NUM_IN_8218D; phyPort++)
    {
        port = basePort + phyPort;

        RT_LOG(LOG_DEBUG, (MOD_HAL|MOD_PHY), "P%u sram_w LEN_CBL_THR:%X LEN_L:%X RLEN_L:%X LEN_S:%X RLEN_S:%X ", port, len_cbl_thr, len_l[phyPort], rlen_l[phyPort], len_s[phyPort], rlen_s[phyPort]);
        hal_miim_park_write(unit, port, 0, 0x1F, 27, 0x83A0);
        hal_miim_park_write(unit, port, 0, 0x1F, 28, len_cbl_thr);
        hal_miim_park_write(unit, port, 0, 0x1F, 27, 0x83A1);
        hal_miim_park_write(unit, port, 0, 0x1F, 28, len_l[phyPort]);
        hal_miim_park_write(unit, port, 0, 0x1F, 27, 0x83A3);
        hal_miim_park_write(unit, port, 0, 0x1F, 28, rlen_l[phyPort]);
        hal_miim_park_write(unit, port, 0, 0x1F, 27, 0x83A5);
        hal_miim_park_write(unit, port, 0, 0x1F, 28, len_s[phyPort]);
        hal_miim_park_write(unit, port, 0, 0x1F, 27, 0x83A7);
        hal_miim_park_write(unit, port, 0, 0x1F, 28, rlen_s[phyPort]);
    }
    hal_miim_park_write(unit, basePort, 0, 0x1F, 30, blk);
}

void
phy_8218d_b_nc_init_cs(uint32 unit, rtk_port_t basePort)
{
    uint32  port, phyPort;
    uint32  fix_page = 0x1F, maxPage;
    uint32  phy_state = 0, patch_rdy, wait_cnt;


    maxPage = HAL_MIIM_FIX_PAGE;

    for (phyPort = 0; phyPort < PORT_NUM_IN_8218D; phyPort++)
    {
        port = basePort + phyPort;

        hal_miim_park_write(unit, port, 0, fix_page, 30, 0x0);
        hal_miim_park_write(unit, port, maxPage, fix_page, 0, 0x1940);
        hal_miim_park_write(unit, port, maxPage, fix_page, 31, 0x0a46);
        hal_miim_parkBits_read(unit, port, maxPage, fix_page, 21, 10, 8, &phy_state);
        if ((phy_state == 3) || (phy_state == 5))
        {
            //# set patch request = 1
            hal_miim_park_write(unit, port, maxPage, fix_page, 31, 0x0b82);
            hal_miim_parkBits_write(unit, port, maxPage, fix_page, 16, 4, 4, 0x1);
            //# check patch ready
            wait_cnt = 500;
            patch_rdy = 0;
            while (wait_cnt > 0)
            {
                hal_miim_park_write(unit, port, maxPage, fix_page, 31, 0xb80);
                hal_miim_parkBits_read(unit, port, maxPage, fix_page, 16, 6, 6, &patch_rdy);
                if (patch_rdy == 1)
                    break;
                wait_cnt--;
            }
            if (patch_rdy == 0)
            {
                /* change page back to 0 */
                hal_miim_park_write(unit, port, maxPage, fix_page, 31, 0x0);
                RT_LOG(LOG_DEBUG, (MOD_HAL|MOD_PHY), "U%u P%u 8218D nc_init: req_patch_ready is 0\n", unit, port);
                continue;
            }

            hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x8028);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x8901);
            hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0xb82e);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0001);

            hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0xb820);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0090);
            hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0xa012);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0000);
            hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0xa014);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x2c08);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x2c16);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x2c19);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x2c1c);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x2c1f);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x2c28);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x2c2b);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x2c2b);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xd71e);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x3231);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x1c12);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xc096);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xd710);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x6080);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xd71e);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x7fac);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x2aa7);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xba02);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xe003);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0202);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xd710);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x2ad3);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xc500);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x9a02);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x2aa6);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xa120);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xac03);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x297a);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xa120);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xac03);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x2894);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xe018);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0208);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xd705);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x40b7);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xc002);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xd710);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x4000);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x8208);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x2b31);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x4049);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x2b55);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x2b58);
            hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0xa01a);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0000);
            hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0xa026);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xffff);
            hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0xa024);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xffff);
            hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0xa022);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0b53);
            hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0xa020);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x3b2f);
            hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0xa006);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0893);
            hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0xa004);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0979);
            hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0xa002);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0aa5);
            hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0xa000);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xfad1);
            hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0xb820);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0010);

            hal_miim_park_write(unit, port, maxPage, fix_page, 31, 0x0a40);
            hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0xb82e);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0000);
            hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x8028);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0000);
            hal_miim_park_write(unit, port, maxPage, fix_page, 31, 0x0b82);
            hal_miim_parkBits_write(unit, port, maxPage, fix_page, 16, 4, 4, 0x0);
        } /* if (phy_state == 5) */
        else
        {
            RT_LOG(LOG_DEBUG, (MOD_HAL|MOD_PHY), "unit=%u port=%u phy_state=%u", unit, port, phy_state);
        }
    } /* end for */
}


void
phy_8218d_b_uc_init_cs(uint32 unit, rtk_port_t basePort)
{
    uint32  port, phyPort;
    uint32  fix_page = 0x1F, maxPage;
    uint32  phy_state = 0, patch_rdy, wait_cnt;


    maxPage = HAL_MIIM_FIX_PAGE;


    for (phyPort = 0; phyPort < PORT_NUM_IN_8218D; phyPort++)
    {
        port = basePort + phyPort;

        hal_miim_park_write(unit, port, 0, fix_page, 30, 0x0);
        hal_miim_park_write(unit, port, maxPage, fix_page, 0, 0x1940);
        hal_miim_park_write(unit, port, maxPage, fix_page, 31, 0x0a46);
        hal_miim_parkBits_read(unit, port, maxPage, fix_page, 21, 10, 8, &phy_state);
        if ((phy_state == 3) || (phy_state == 5))
        {
            //# set patch request = 1
            hal_miim_park_write(unit, port, maxPage, fix_page, 31, 0x0b82);
            hal_miim_parkBits_write(unit, port, maxPage, fix_page, 16, 4, 4, 0x1);
            //# check patch ready
            wait_cnt = 500;
            patch_rdy = 0;
            while (wait_cnt > 0)
            {
                hal_miim_park_write(unit, port, maxPage, fix_page, 31, 0xb80);
                hal_miim_parkBits_read(unit, port, maxPage, fix_page, 16, 6, 6, &patch_rdy);
                if (patch_rdy == 1)
                    break;
                wait_cnt--;
            }
            if (patch_rdy == 0)
            {
                /* change page back to 0 */
                hal_miim_park_write(unit, port, maxPage, fix_page, 31, 0x0);
                RT_LOG(LOG_DEBUG, (MOD_HAL|MOD_PHY), "U%u P%u 8218D nc_init: req_patch_ready is 0\n", unit, port);
                continue;
            }
            hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x8028);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x8901);

            hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x8419);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xaf84);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x25af);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x842e);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xaf84);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x4eaf);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x84b0);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0284);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xb0bf);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x5a66);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xaf0e);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xfe01);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x05d2);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x00af);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0ca4);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xf8e0);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x87da);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xe187);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xdbbf);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x59be);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0257);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xd9ee);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x87d9);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x00bf);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x85f2);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x025c);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xb1fc);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x04e5);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x87d8);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0d11);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xf62f);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xef31);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xe083);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xa00d);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x01f6);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x271b);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x03aa);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x4bee);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x87d9);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x01bf);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x85f2);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x025c);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xbabf);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x592e);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0258);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x1dad);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x2835);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xbf59);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x3402);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x581d);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xad28);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x17e0);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x87b7);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xe187);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xb8bf);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x5a93);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0257);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xd9e1);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x87bb);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xbf5a);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x8d02);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x57d9);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xae15);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xe087);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xb9e1);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x87ba);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xbf5a);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x9302);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x57d9);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xe187);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xbcbf);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x5a8d);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0257);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xd9af);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x5d69);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xaf5d);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x83f8);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xf9fa);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xfbef);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x69cf);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xe080);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x16ad);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x2602);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xae03);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xaf85);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xd1d3);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x00be);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0000);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xbf5b);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x4d02);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x5cba);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xd409);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x30ef);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x230c);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x211e);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x02bf);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x5b50);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0257);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xd9bf);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x5b53);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0258);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x1d24);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x13a3);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x04e0);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xcce4);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x87e1);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xd200);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xe187);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xd9ac);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x2805);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xbf87);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xe5ae);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x03bf);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x87bf);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x1a92);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xd91b);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x10ad);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x2f04);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x12a2);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0ce6);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xe687);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xe2e1);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x87d9);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xac28);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x05bf);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x87f1);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xae03);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xbf87);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xcb1a);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x92d9);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xe587);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xe4d4);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x4000);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xbf5a);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x6f02);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x57d9);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xbf5a);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x7502);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x57d9);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xbf5a);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x7202);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x57d9);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xd300);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xbe00);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x01d4);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0000);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xbf3d);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xa202);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x57d9);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xbfa8);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xe8d4);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0100);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x1c43);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x1a94);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xd819);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xd90d);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x4559);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x1fe2);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x87e4);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x1b21);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xe687);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xe0ac);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x371f);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xa200);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x02ae);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x3211);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xbf3d);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x9f02);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x57d9);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x08bf);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x3da2);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0257);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xd908);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0285);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xd902);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x52b3);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x829f);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xe8ae);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x1881);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xbf3d);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x9f02);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x57d9);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x08bf);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x3da2);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0257);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xd908);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0285);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xd902);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x52b3);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x129f);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xe80c);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x8113);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xa304);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x9ed4);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0000);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xbf3d);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xa202);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x57d9);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xd410);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x00bf);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x5a6f);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0257);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xd9bf);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x5a75);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0257);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xd9bf);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x5a72);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0257);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xd9bf);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x5aed);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0258);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x1de3);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x87bd);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xd200);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x1b45);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xad27);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x09e1);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x87be);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xbf5a);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xa202);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x57d9);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xc7ef);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x96ff);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xfefd);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xfc04);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xf8fa);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xfb02);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x5c25);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xef67);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xd000);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xe187);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xe3ef);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x7402);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x5c40);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xad50);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xf8ff);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xfefc);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0477);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xa46c);
            hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x87B7);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0157);
            hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x87B9);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0159);
            hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x87BB);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xffff);
            hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x87BD);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xfe78);
            hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0xb818);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0efb);
            hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0xb81a);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0ca2);
            hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0xb81c);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x5d58);
            hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0xb81e);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xffff);
            hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0xb832);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0007);
            hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x87E3);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0a00);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xCFD8);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xE0E8);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xF0F9);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0008);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x1018);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x2129);

            hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x87F1);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0B0A);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0A0A);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0909);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0808);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0706);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0504);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0300);

            hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x87FE);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0000);
            hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x80bd);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x7070);

            hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x87BF);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xBEC6);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xCED7);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xDFE6);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xEEF6);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xFD05);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x090D);

            hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x87CB);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0B0A);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0A0A);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0909);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0808);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0706);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0504);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0300);

            hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0xb82e);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0000);
            hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x8028);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0000);
            hal_miim_park_write(unit, port, maxPage, fix_page, 31, 0x0b82);
            hal_miim_parkBits_write(unit, port, maxPage, fix_page, 16, 4, 4, 0x0);
        } /* end if (phy_state) */
        else
        {
            RT_LOG(LOG_DEBUG, (MOD_HAL|MOD_PHY), "unit=%u port=%u phy_state=%u", unit, port, phy_state);
        }
    } /* end for */

    for (phyPort = 0; phyPort < PORT_NUM_IN_8218D; phyPort++)
    {
        port = basePort + phyPort;
        hal_miim_park_write(unit, port, maxPage, fix_page, 30, 0x0);
        hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x8016);
        hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 14, 14, 0x1);
    }

}


void
_phy_8218d_bConf_init_cs(uint32 unit, rtk_port_t basePort)
{
    int32   ret;
    uint32  port, phyPort;
    uint32  fix_page = 0x1F, maxPage;
    uint32  Val_83a1, Val_83a7, Val_83a0;
    uint32  Val_83a1_b30, Val_83a1_b74, Val_83a1_b1108, Val_83a1_b1512, Val_83a5_new;
    rtk_phy_xfmrRj45Impd_t  impd_ohm = RTK_PHY_XFMRRJ45IMPD_100_OHM;


    if ((ret = ioal_param_phyXfmrRj45Impd_get(unit, &impd_ohm)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_HAL|MOD_PHY), "unit=%u port=%u unable to get XfmrRj45Imed", unit, basePort);
    }

    maxPage = HAL_MIIM_FIX_PAGE;

    for (phyPort = 0; phyPort < PORT_NUM_IN_8218D; phyPort++)
    {
        port = basePort + phyPort;
        hal_miim_park_write(unit, port, maxPage, fix_page, 30, 0);
        hal_miim_park_write(unit, port, maxPage, fix_page, 0, 0x1940);
    }


    hal_miim_park_write(unit, basePort, maxPage, fix_page, 30, 8);
    hal_miim_park_write(unit, basePort, maxPage, fix_page, 31, 0x278);
    hal_miim_park_write(unit, basePort, maxPage, fix_page, 0x12, 0x0455);
    hal_miim_park_write(unit, (basePort + 1), maxPage, fix_page, 31, 0x0bc6);
    hal_miim_park_write(unit, (basePort + 1), maxPage, fix_page, 0x12, 0xc000);
    hal_miim_park_write(unit, (basePort + 1), maxPage, fix_page, 0x12, 0x4000);
    hal_miim_park_write(unit, (basePort + 1), maxPage, fix_page, 0x10, 0x0063);
    hal_miim_park_write(unit, (basePort + 5), maxPage, fix_page, 31, 0x0bc7);
    hal_miim_park_write(unit, (basePort + 5), maxPage, fix_page, 0x12, 0xc000);
    hal_miim_park_write(unit, (basePort + 5), maxPage, fix_page, 0x12, 0x4000);
    hal_miim_park_write(unit, (basePort + 5), maxPage, fix_page, 0x10, 0x0063);
    hal_miim_park_write(unit, basePort, maxPage, fix_page, 0x12, 0x0000);
    hal_miim_park_write(unit, basePort, maxPage, fix_page, 30, 0);


    //pg.0x361 reg.0x10 = sub_version, Reg.11 = main_version
    hal_miim_park_write(unit, basePort, maxPage, fix_page, 30, 8);
    hal_miim_park_write(unit, basePort, maxPage, fix_page, 31, 0x361);
    hal_miim_park_write(unit, basePort, maxPage, fix_page, 0x10, 0x0007);
    hal_miim_park_write(unit, basePort, maxPage, fix_page, 0x11, 0x0001);
    hal_miim_park_write(unit, basePort, maxPage, fix_page, 30, 0);

    phy_8218d_sram_patch_cs(unit, basePort);

    for (phyPort = 0; phyPort < PORT_NUM_IN_8218D; phyPort++)
    {
        port = basePort + phyPort;
        hal_miim_park_write(unit, port, maxPage, fix_page, 30, 0);
        hal_miim_park_write(unit, port, maxPage, fix_page, 0, 0x1940);
        hal_miim_park_write(unit, port, maxPage, fix_page, 31, 0xbc1);
        hal_miim_park_write(unit, port, maxPage, fix_page, 18, 0x0004);
        hal_miim_park_write(unit, port, maxPage, fix_page, 19, 0x0077);
        if (impd_ohm == RTK_PHY_XFMRRJ45IMPD_75_OHM)
        {
            hal_miim_park_write(unit, port, maxPage, fix_page, 20, 0x8200);
        }
        else if (impd_ohm == RTK_PHY_XFMRRJ45IMPD_95_OHM)
        {
            hal_miim_park_write(unit, port, maxPage, fix_page, 20, 0x9C00);
        }
        else //RTK_PHY_XFMRRJ45IMPD_100_OHM
        {
            hal_miim_park_write(unit, port, maxPage, fix_page, 20, 0x9A00);
        }
        hal_miim_park_write(unit, port, maxPage, fix_page, 31, 0xa42);
        hal_miim_park_write(unit, port, maxPage, fix_page, 22, 0x0F10);
        hal_miim_park_write(unit, port, maxPage, fix_page, 31, 0xbcd);
        hal_miim_park_write(unit, port, maxPage, fix_page, 21, 0x0000);
        hal_miim_park_write(unit, port, maxPage, fix_page, 31, 0x0a80);
        hal_miim_parkBits_write(unit, port, maxPage, fix_page, 22, 13, 12, 2);
        hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x8010);
        hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xfff7);
        hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x830f);
        hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0000);
        hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x8311);
        hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0000);
        hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x815b);
        hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0800);
        hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x8314);
        hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, 0x01);
        hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x809a);
        hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x009c);
        hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x80ac);
        hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0384);
        hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x8069);
        hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0824);
        hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x8071);
        hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0824);
        hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x80bd);
        hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x7070);
        hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x8095);
        hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x700E);
    }

    phy_8218d_b_uc_init_cs(unit, basePort);

    phy_8218d_b_nc_init_cs(unit, basePort);

    for (phyPort = 0; phyPort < PORT_NUM_IN_8218D; phyPort++)
    {
        port = basePort + phyPort;
        Val_83a0 = 0x1e00;
        hal_miim_park_write(unit, port, maxPage, fix_page, 30, 0);
        hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x83A1);
        hal_miim_park_read(unit, port, maxPage, fix_page, 28, &Val_83a1);

        Val_83a1_b30 = Val_83a1 & 0x000f;
        if (Val_83a1_b30 >= 0x7)  {
            Val_83a1_b30 = 0xf;
        } else {
            Val_83a1_b30 = Val_83a1_b30 + 8;
        }

        Val_83a1_b74 = Val_83a1 & 0x00f0;
        if (Val_83a1_b74 >= 0x70) {
            Val_83a1_b74 = 0xf0;
        } else {
            Val_83a1_b74 = Val_83a1_b74 + 0x80;
        }

        Val_83a1_b1108 = Val_83a1 & 0x0f00;
        if (Val_83a1_b1108 >= 0x700) {
            Val_83a1_b1108 = 0xf00;
        } else {
            Val_83a1_b1108 = Val_83a1_b1108 + 0x800;
        }

        Val_83a1_b1512 = Val_83a1 & 0xf000;
        if (Val_83a1_b1512 >= 0x7000) {
            Val_83a1_b1512 = 0xf000;
        } else {
            Val_83a1_b1512 = Val_83a1_b1512 + 0x8000;
        }

        Val_83a5_new = Val_83a1_b1512 + Val_83a1_b1108 + Val_83a1_b74 + Val_83a1_b30;

        hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x83A7);
        hal_miim_park_read(unit, port, maxPage, fix_page, 28, &Val_83a7);
        hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x83A0);
        hal_miim_park_write(unit, port, maxPage, fix_page, 28, Val_83a0);
        hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x83A1);
        hal_miim_park_write(unit, port, maxPage, fix_page, 28, Val_83a1);
        hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x83A5);
        hal_miim_park_write(unit, port, maxPage, fix_page, 28, Val_83a5_new);
        hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x87DA);
        hal_miim_park_write(unit, port, maxPage, fix_page, 28, Val_83a1);
        hal_miim_park_write(unit, port, maxPage, fix_page, 31, 0xbcd);
        hal_miim_park_write(unit, port, maxPage, fix_page, 22, Val_83a1);
        hal_miim_park_write(unit, port, maxPage, fix_page, 23, Val_83a7);

        hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x8016);
        hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x4100);
    } /* end for */



    hal_miim_park_write(unit, basePort, maxPage, fix_page, 30, 8);
    hal_miim_park_write(unit, basePort, maxPage, fix_page, 31, 0x346);
    hal_miim_park_write(unit, basePort, maxPage, fix_page, 0x13, 0x5500);
    hal_miim_park_write(unit, basePort, maxPage, fix_page, 0x14, 0x0000);
    hal_miim_park_write(unit, basePort, maxPage, fix_page, 0x15, 0x0000);
    hal_miim_park_write(unit, basePort, maxPage, fix_page, 0x16, 0x0001);
    hal_miim_park_write(unit, basePort, maxPage, fix_page, 0x17, 0x5500);
    hal_miim_park_write(unit, basePort, maxPage, fix_page, 31, 0x347);
    hal_miim_park_write(unit, basePort, maxPage, fix_page, 0x10, 0x0000);
    hal_miim_park_write(unit, basePort, maxPage, fix_page, 0x11, 0x0000);
    hal_miim_park_write(unit, basePort, maxPage, fix_page, 0x12, 0x0001);
    hal_miim_park_write(unit, basePort, maxPage, fix_page, 0x13, 0x5500);
    hal_miim_park_write(unit, basePort, maxPage, fix_page, 0x14, 0x0000);
    hal_miim_park_write(unit, basePort, maxPage, fix_page, 0x15, 0x0000);
    hal_miim_park_write(unit, basePort, maxPage, fix_page, 0x16, 0x0001);
    hal_miim_park_write(unit, basePort, maxPage, fix_page, 0x17, 0x5500);
    hal_miim_park_write(unit, basePort, maxPage, fix_page, 31, 0x348);
    hal_miim_park_write(unit, basePort, maxPage, fix_page, 0x10, 0x0000);
    hal_miim_park_write(unit, basePort, maxPage, fix_page, 0x11, 0x0000);
    hal_miim_park_write(unit, basePort, maxPage, fix_page, 0x12, 0x0001);
    hal_miim_park_write(unit, basePort, maxPage, fix_page, 0x13, 0x5500);
    hal_miim_park_write(unit, basePort, maxPage, fix_page, 0x14, 0x0000);
    hal_miim_park_write(unit, basePort, maxPage, fix_page, 0x15, 0x0000);
    hal_miim_park_write(unit, basePort, maxPage, fix_page, 0x16, 0x0001);
    hal_miim_park_write(unit, basePort, maxPage, fix_page, 0x17, 0x5500);
    hal_miim_park_write(unit, basePort, maxPage, fix_page, 31, 0x349);
    hal_miim_park_write(unit, basePort, maxPage, fix_page, 0x10, 0x0000);
    hal_miim_park_write(unit, basePort, maxPage, fix_page, 0x11, 0x0000);
    hal_miim_park_write(unit, basePort, maxPage, fix_page, 0x12, 0x0001);
    hal_miim_park_write(unit, basePort, maxPage, fix_page, 0x13, 0x5500);
    hal_miim_park_write(unit, basePort, maxPage, fix_page, 0x14, 0x0000);
    hal_miim_park_write(unit, basePort, maxPage, fix_page, 0x15, 0x0000);
    hal_miim_park_write(unit, basePort, maxPage, fix_page, 0x16, 0x0001);
    hal_miim_park_write(unit, basePort, maxPage, fix_page, 0x17, 0x5500);
    hal_miim_park_write(unit, basePort, maxPage, fix_page, 31, 0x34A);
    hal_miim_park_write(unit, basePort, maxPage, fix_page, 0x10, 0x0000);
    hal_miim_park_write(unit, basePort, maxPage, fix_page, 0x11, 0x0000);
    hal_miim_park_write(unit, basePort, maxPage, fix_page, 0x12, 0x0001);

    hal_miim_park_write(unit, basePort, maxPage, fix_page, 30, 0);

}

void
phy_8218d_bConf_init(uint32 unit, rtk_port_t basePort)
{

    _phy_8218d_bConf_init_cs(unit, basePort);

}

/* Function Name:
 *      sub_phy_8218d_patch_set
 * Description:
 *      Patch the PHY:8218B.
 * Input:
 *      unit   - unit id
 *      port   - PHY base port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
sub_phy_8218d_patch_set(uint32 unit, rtk_port_t port)
{
    hal_control_t   *pHalCtrl;
    int32   ret = RT_ERR_FAILED;
    int32   i;

    RT_PARAM_CHK((port != HWP_PHY_BASE_MACID(unit, port)), RT_ERR_PORT_ID);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    /*Re-Patch 8218d*/
    rtl8218d_config(unit, port);

    for (i = 0; i < PORT_NUM_IN_8218D; ++i)
        pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_init(unit, port + i);

    ret = RT_ERR_OK;
    return ret;
} /* end of sub_phy_8218d_patch_set */


