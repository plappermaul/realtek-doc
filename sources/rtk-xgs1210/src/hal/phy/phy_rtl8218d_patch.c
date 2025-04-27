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
phy_8218d_sram_patch(uint32 unit, rtk_port_t basePort)
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

/* Function Name:
 *      phy_8218d_nc_init
 * Description:
 *      nc init
 * Input:
 *      unit      - unit id
 *      basePort  - base MAC ID of the 8218D
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      nc init skip init 75m 1
 */
void
phy_8218d_nc_init(uint32 unit, rtk_port_t basePort)
{
    uint32  port, phyPort;
    uint32  fix_page = 0x1F, maxPage;
    uint32  phy_state = 0, patch_rdy;
    uint32  wait_cnt;


    maxPage = HAL_MIIM_FIX_PAGE;

    for (phyPort = 0; phyPort < PORT_NUM_IN_8218D; phyPort++)
    {
        port = basePort + phyPort;

        hal_miim_park_write(unit, port, 0, fix_page, 30, 0x0);

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

            //# set patch key = 0x1900 + ROMID, ROMID = 0x0
            hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x8028);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x8901);
            //# set patch lock = 1
            hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0xb82e);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0001);

            hal_miim_park_write(unit, port, maxPage, fix_page, 0x001f, 0x0a43);

            hal_miim_park_write(unit, port, maxPage, fix_page, 0x1b, 0xB820);
            hal_miim_park_write(unit, port, maxPage, fix_page, 0x1c, 0x0090);
            hal_miim_park_write(unit, port, maxPage, fix_page, 0x1b, 0xA012);
            hal_miim_park_write(unit, port, maxPage, fix_page, 0x1c, 0x0000);
            hal_miim_park_write(unit, port, maxPage, fix_page, 0x1b, 0xA014);
            hal_miim_park_write(unit, port, maxPage, fix_page, 0x1c, 0x2c08);
            hal_miim_park_write(unit, port, maxPage, fix_page, 0x1c, 0x2c16);
            hal_miim_park_write(unit, port, maxPage, fix_page, 0x1c, 0x2c19);
            hal_miim_park_write(unit, port, maxPage, fix_page, 0x1c, 0x2c19);
            hal_miim_park_write(unit, port, maxPage, fix_page, 0x1c, 0x2c19);
            hal_miim_park_write(unit, port, maxPage, fix_page, 0x1c, 0x2c19);
            hal_miim_park_write(unit, port, maxPage, fix_page, 0x1c, 0x2c19);
            hal_miim_park_write(unit, port, maxPage, fix_page, 0x1c, 0x2c19);
            hal_miim_park_write(unit, port, maxPage, fix_page, 0x1c, 0xd71e);
            hal_miim_park_write(unit, port, maxPage, fix_page, 0x1c, 0x3231);
            hal_miim_park_write(unit, port, maxPage, fix_page, 0x1c, 0x1c12);
            hal_miim_park_write(unit, port, maxPage, fix_page, 0x1c, 0xc096);
            hal_miim_park_write(unit, port, maxPage, fix_page, 0x1c, 0xd710);
            hal_miim_park_write(unit, port, maxPage, fix_page, 0x1c, 0x6080);
            hal_miim_park_write(unit, port, maxPage, fix_page, 0x1c, 0xd71e);
            hal_miim_park_write(unit, port, maxPage, fix_page, 0x1c, 0x7fac);
            hal_miim_park_write(unit, port, maxPage, fix_page, 0x1c, 0x2aa7);
            hal_miim_park_write(unit, port, maxPage, fix_page, 0x1c, 0xba02);
            hal_miim_park_write(unit, port, maxPage, fix_page, 0x1c, 0xe003);
            hal_miim_park_write(unit, port, maxPage, fix_page, 0x1c, 0x0202);
            hal_miim_park_write(unit, port, maxPage, fix_page, 0x1c, 0xd710);
            hal_miim_park_write(unit, port, maxPage, fix_page, 0x1c, 0x2ad3);
            hal_miim_park_write(unit, port, maxPage, fix_page, 0x1c, 0xc500);
            hal_miim_park_write(unit, port, maxPage, fix_page, 0x1c, 0x9a02);
            hal_miim_park_write(unit, port, maxPage, fix_page, 0x1c, 0x2aa6);
            hal_miim_park_write(unit, port, maxPage, fix_page, 0x1b, 0xA01A);
            hal_miim_park_write(unit, port, maxPage, fix_page, 0x1c, 0x0000);
            hal_miim_park_write(unit, port, maxPage, fix_page, 0x1b, 0xA006);
            hal_miim_park_write(unit, port, maxPage, fix_page, 0x1c, 0xffff);
            hal_miim_park_write(unit, port, maxPage, fix_page, 0x1b, 0xA004);
            hal_miim_park_write(unit, port, maxPage, fix_page, 0x1c, 0xffff);
            hal_miim_park_write(unit, port, maxPage, fix_page, 0x1b, 0xA002);
            hal_miim_park_write(unit, port, maxPage, fix_page, 0x1c, 0x0aa5);
            hal_miim_park_write(unit, port, maxPage, fix_page, 0x1b, 0xA000);
            hal_miim_park_write(unit, port, maxPage, fix_page, 0x1c, 0x3ad1);
            hal_miim_park_write(unit, port, maxPage, fix_page, 0x1b, 0xB820);
            hal_miim_park_write(unit, port, maxPage, fix_page, 0x1c, 0x0010);

            //# clear patch lock
            hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0xb82e);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0000);
            //# clear patch key
            hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x8028);
            hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0000);
            //# clear patch request
            hal_miim_park_write(unit, port, maxPage, fix_page, 31, 0x0b82);
            hal_miim_parkBits_write(unit, port, maxPage, fix_page, 16, 4, 4, 0x0);
        }

        /* change page back to 0 */
        hal_miim_park_write(unit, port, maxPage, fix_page, 31, 0x0);
    }


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


