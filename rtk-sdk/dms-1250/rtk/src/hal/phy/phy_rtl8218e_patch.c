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
 * Purpose : PHY 8218E Driver APIs.
 *
 * Feature : PHY 8218E Driver APIs
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
#include <hal/phy/phy_rtl8218e.h>
#include <hal/phy/phy_rtl8218e_patch.h>
#include <hal/phy/phy_construct.h>
#include <osal/time.h>


/*
 * Symbol Definition
 */
#define PHY_RTL8218E_C_PATCH                1

#define PHY_RTL8218E_PATCH_WAIT_TIMEOUT     10000
#define PHY_RTL8218E_PHY_STATE_PWRDN        (5)
#define PHY_RTL8218E_PHY_STATE_LANON        (3)


/*
 * Data Declaration
 */


/*
 * Macro Declaration
 */
#define PHY_8218E_Efuse_Page                0x026E
#define PHY_8218E_Efuse_CMD_Reg             16
#define PHY_8218E_Efuse_ADR_Reg             17
#define PHY_8218E_Efuse_WDAT_Reg            18
#define PHY_8218E_Efuse_RDAT_Reg            19
#define PHY_8218E_Efuse_ADR_Max             2048
#define PHY_8218E_Efuse_ADR_min             0
#define PHY_8218E_Efuse_Ana_ADR_min         0
#define PHY_8218E_Efuse_Ana_ADR_max         7
#define PHY_8218E_Efuse_CMD_RD              0
#define PHY_8218E_Efuse_CMD_WR              1


/*
 * Function Declaration
 */
int32
_phy_8218e_dummyReg_init(uint32 unit, rtk_port_t port)
{
    uint32  blk = 0;

    hal_miim_read(unit, port, 0, 30, &blk);
    hal_miim_write(unit, port, 0, 30, 8);
    hal_miim_write(unit, port, PHY_8218E_DUMMY_REG_PAGE, PHY_8218E_DUMMY_REG_REG, PHY_8218E_DUMMY_REG_VALUE);
    hal_miim_write(unit, port, 0, 30, blk);

    return RT_ERR_OK;
}


int32
_phy_rtl8218e_patch_phyState_wait(uint32 unit, rtk_port_t port)
{
    int32   ret = RT_ERR_OK;
    uint32  maxPage = HAL_MIIM_FIX_PAGE;
    uint32  fix_page = 0x1F;
    uint32  phy_state = 0;
    uint32  cnt = 0;

    while ((phy_state != PHY_RTL8218E_PHY_STATE_PWRDN) && (phy_state != PHY_RTL8218E_PHY_STATE_LANON))
    {
        if ((ret = hal_miim_park_write(unit, port, maxPage, fix_page, 31, 0x0a46)) != RT_ERR_OK)
            return ret;
        if ((ret = hal_miim_parkBits_read(unit, port, maxPage, fix_page, 21, 10, 8, &phy_state)) != RT_ERR_OK)
            return ret;

        if ((phy_state == PHY_RTL8218E_PHY_STATE_PWRDN) || (phy_state == PHY_RTL8218E_PHY_STATE_LANON))
            break;

        cnt++;
        if (cnt >= PHY_RTL8218E_PATCH_WAIT_TIMEOUT)
        {
            RT_LOG(LOG_INFO, (MOD_HAL|MOD_PHY), "L%d U%u P%u 8218E patch: patch-not-apply phy_state is %u\n",
                __LINE__, unit, port, phy_state);
            return RT_ERR_TIMEOUT;
        }
    }

    return RT_ERR_OK;
}

int32
_phy_rtl8218e_patch_patchReady_wait(uint32 unit, rtk_port_t port)
{
    int32   ret = RT_ERR_OK;
    uint32  maxPage = HAL_MIIM_FIX_PAGE;
    uint32  fix_page = 0x1F;
    uint32  patch_ready = 0;
    uint32  cnt = 0;

    while (patch_ready != 1)
    {
        if ((ret = hal_miim_park_write(unit, port, maxPage, fix_page, 31, 0x0b80)) != RT_ERR_OK)
            return ret;
        if ((ret = hal_miim_parkBits_read(unit, port, maxPage, fix_page, 16, 6, 6, &patch_ready)) != RT_ERR_OK)
            return ret;

        if (patch_ready == 1)
            break;

        cnt++;
        if (cnt >= PHY_RTL8218E_PATCH_WAIT_TIMEOUT)
        {
            RT_LOG(LOG_INFO, (MOD_HAL|MOD_PHY), "L%d U%u P%u 8218E patch: patch-not-apply patch_ready is %u\n",
                __LINE__, unit, port, patch_ready);
            return RT_ERR_TIMEOUT;
        }
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      _phy_8218e_efuse_read
 * Description:
 *      Read from eFUSE
 * Input:
 *      unit        - unit id
 *      basePort    - base port id
 *      efAddr      - address to read
 * Output:
 *      pData       - pointer buffer of read data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
_phy_8218e_efuse_read(uint32 unit, rtk_port_t basePort, uint32 efAddr, uint32 *pData)
{
    int32   ret = RT_ERR_OK;
    uint32  data = 0, dataReg30 = 0, rdy_bit = 0;
    uint32  rCnt = 0;
    uint32  maxPage = 0;

    if ((efAddr < PHY_8218E_Efuse_Ana_ADR_min) || (efAddr > PHY_8218E_Efuse_ADR_Max))
    {
        osal_printf(" Efuse Address 0x%X over range!", efAddr);
        return RT_ERR_FAILED;
    }

    maxPage = HAL_MIIM_FIX_PAGE;

    /* keep reg.30 data */
    if ((ret = hal_miim_park_read(unit, basePort, 0, 0x1F, 30, &dataReg30)) != RT_ERR_OK)
        return ret;

    if ((ret = hal_miim_park_write(unit, basePort, maxPage, 0x1F, 30, 0x8)) != RT_ERR_OK)
        return ret;

    if ((ret = hal_miim_park_write(unit, basePort, maxPage, 0x1F, 31, PHY_8218E_Efuse_Page)) != RT_ERR_OK)
        return ret;

    if ((ret = hal_miim_park_write(unit, basePort, maxPage, 0x1F, PHY_8218E_Efuse_ADR_Reg, efAddr)) != RT_ERR_OK)
        return ret;

    if ((ret = hal_miim_park_write(unit, basePort, maxPage, 0x1F, PHY_8218E_Efuse_CMD_Reg, PHY_8218E_Efuse_CMD_RD)) != RT_ERR_OK)
        return ret;


    while (1)
    {
        rCnt++;
        ret = hal_miim_park_read(unit, basePort, maxPage, 0x1F, PHY_8218E_Efuse_CMD_Reg, &data);
        if (ret != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_HAL), "unit %u port %u Read%u", unit, basePort, PHY_8218E_Efuse_CMD_Reg);
            return ret;
        }
        rdy_bit = data & 0xfe;
        if (rdy_bit == 0)
        {
            break;
        }

        if (rCnt >= PHY_RTL8218E_PATCH_WAIT_TIMEOUT)
        {
            ret = RT_ERR_TIMEOUT;
            RT_ERR(ret, (MOD_HAL), "unit %u port %u Efuse Read no ack", unit, basePort);
            *pData = 0;
            goto ERR_RET;
        }
    }

    ret = hal_miim_park_read(unit, basePort, maxPage, 0x1F, PHY_8218E_Efuse_RDAT_Reg, pData);
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

int32
_phy_8218e_efuse_readbits(uint32 unit, rtk_port_t basePort, uint32 efAddr, uint32 mbit, uint32 lbit, uint32 *pVal)
{
    int32   ret = RT_ERR_OK;
    uint32  data = 0;

    *pVal = 0;
    if ((ret = _phy_8218e_efuse_read(unit, basePort, efAddr, &data)) != RT_ERR_OK)
        return ret;

    *pVal = REG32_FIELD_GET(data, lbit, UINT32_BITS_MASK(mbit, lbit));
    return RT_ERR_OK;
}


#if PHY_RTL8218E_C_PATCH
void
_phy_8218e_c_uc_nctl_perPort_init(uint32 unit, rtk_port_t port)
{
    int32   ret = RT_ERR_OK;
    uint32  fix_page = 0x1F;
    uint32  maxPage = HAL_MIIM_FIX_PAGE;

    hal_miim_park_write(unit, port, maxPage, fix_page, 30, 0);
    hal_miim_park_write(unit, port, maxPage, fix_page, 31, 0xB82);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 16, 4, 4, 0x1);

    if ((ret = _phy_rtl8218e_patch_patchReady_wait(unit, port)) != RT_ERR_OK)
        return;

    hal_miim_park_write(unit, port, maxPage, fix_page, 30, 0);
    hal_miim_park_write(unit, port, maxPage, fix_page, 31, 0xA43);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x854f);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xaf85);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x67af);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x8570);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xaf85);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x73af);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x857d);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xaf85);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x9baf);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x859b);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xaf85);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x9baf);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x859b);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0255);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xc4e0);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x8459);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xaf32);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x7daf);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x3ba5);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0255);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xddee);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x8456);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x80af);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x3266);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x025f);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x51bf);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x5e0e);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0259);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x6abf);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x5e11);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0259);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x6abf);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x5e14);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0259);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x6abf);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x5e17);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0259);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x6aaf);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x361c);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0xb818);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x327a);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0xb81a);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x3ba2);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0xb81c);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x3262);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0xb81e);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x3619);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0xb846);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0000);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0xb848);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0000);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0xb84a);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0000);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0xb84c);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0000);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0xb832);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x000f);

    hal_miim_park_write(unit, port, maxPage, fix_page, 30, 0);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0xB82E);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0000);
    hal_miim_park_write(unit, port, maxPage, fix_page, 31, 0xB82);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 16, 4, 4, 0x0);


    hal_miim_park_write(unit, port, maxPage, fix_page, 30, 0);
    hal_miim_park_write(unit, port, maxPage, fix_page, 31, 0xB82);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 16, 4, 4, 0x1);

    if ((ret = _phy_rtl8218e_patch_patchReady_wait(unit, port)) != RT_ERR_OK)
        return;

    hal_miim_park_write(unit, port, maxPage, fix_page, 30, 0);
    hal_miim_park_write(unit, port, maxPage, fix_page, 31, 0xB82);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 16, 7, 7, 0x1);

    hal_miim_park_write(unit, port, maxPage, fix_page, 30, 0);
    hal_miim_park_write(unit, port, maxPage, fix_page, 31, 0xA43);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0xa016);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0000);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0xa012);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0000);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0xa014);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x1800);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x8010);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x1800);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x8014);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x1800);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x8018);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x1800);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x8018);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x1800);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x8018);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x1800);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x8018);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x1800);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x8018);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x1800);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x8018);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x294c);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0d47);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x1800);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0d40);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x294c);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0db3);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x1800);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0daf);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0xa026);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0000);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0xa024);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0000);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0xa022);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0000);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0xa020);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0000);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0xa006);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0000);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0xa004);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0000);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0xa002);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0dad);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0xa000);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0d43);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0xa008);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0300);

    hal_miim_park_write(unit, port, maxPage, fix_page, 30, 0);
    hal_miim_park_write(unit, port, maxPage, fix_page, 31, 0xB82);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 16, 7, 7, 0x0);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0xB82E);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0000);
    hal_miim_park_write(unit, port, maxPage, fix_page, 31, 0xB82);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 16, 4, 4, 0x0);
}

void
_phy_8218e_c_uc_nctl_init(uint32 unit, rtk_port_t basePort)
{
    uint32  port = 0;
    uint32  phyPort = 0;

    for (phyPort = 0; phyPort < PORT_NUM_IN_8218E; phyPort++)
    {
        port = basePort + phyPort;
        _phy_8218e_c_uc_nctl_perPort_init(unit, port);
    }
}
#endif


void
_phy_8218e_gphy_perPort_init(uint32 unit, rtk_port_t port)
{
    int32   ret = RT_ERR_OK;
    uint32  fix_page = 0x1F;
    uint32  maxPage = HAL_MIIM_FIX_PAGE;
    uint32  basePort = 0;

    basePort = HWP_PHY_BASE_MACID(unit, port);

    if ((ret = _phy_rtl8218e_patch_phyState_wait(unit, port)) != RT_ERR_OK)
        return;

    hal_miim_park_write(unit, port, maxPage, fix_page, 30, 0);
    hal_miim_park_write(unit, port, maxPage, fix_page, 31, 0xA43);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x8016);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 10, 10, 0);

    hal_miim_park_write(unit, port, maxPage, fix_page, 30, 0);
    hal_miim_park_write(unit, port, maxPage, fix_page, 31, 0xA80);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 22, 15, 14, 0x2);
    hal_miim_park_write(unit, port, maxPage, fix_page, 31, 0xBC0);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 21, 8, 6, 0x7);
    hal_miim_park_write(unit, port, maxPage, fix_page, 31, 0xBC1);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 22, 15, 4, 0xB6D);

    hal_miim_park_write(unit, port, maxPage, fix_page, 30, 0);
    hal_miim_park_write(unit, port, maxPage, fix_page, 31, 0xA80);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 21, 15, 14, 0x3);

    hal_miim_park_write(unit, port, maxPage, fix_page, 30, 0);
    hal_miim_park_write(unit, port, maxPage, fix_page, 31, 0xA58);
    hal_miim_park_write(unit, port, maxPage, fix_page, 18, 0x8843);

    hal_miim_park_write(unit, port, maxPage, fix_page, 30, 0);
    hal_miim_park_write(unit, port, maxPage, fix_page, 31, 0xB82);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 16, 4, 4, 0x1);

    hal_miim_park_write(unit, port, maxPage, fix_page, 30, 0);
    hal_miim_park_write(unit, port, maxPage, fix_page, 0, 0x1940);

    if (port == basePort)
    {
        hal_miim_park_write(unit, port, maxPage, fix_page, 30, 0x8);
        hal_miim_park_write(unit, port, maxPage, fix_page, 31, 0x322);
        hal_miim_parkBits_write(unit, port, maxPage, fix_page, 0x10, 15, 12, 0x7);
        hal_miim_parkBits_write(unit, port, maxPage, fix_page, 0x13, 15, 12, 0x7);
        hal_miim_parkBits_write(unit, port, maxPage, fix_page, 0x16, 15, 12, 0x7);
        hal_miim_park_write(unit, port, maxPage, fix_page, 31, 0x323);
        hal_miim_parkBits_write(unit, port, maxPage, fix_page, 0x11, 15, 12, 0x7);
        hal_miim_park_write(unit, port, maxPage, fix_page, 31, 0x324);
        hal_miim_parkBits_write(unit, port, maxPage, fix_page, 0x10, 15, 12, 0x7);
        hal_miim_parkBits_write(unit, port, maxPage, fix_page, 0x13, 15, 12, 0x7);
        hal_miim_parkBits_write(unit, port, maxPage, fix_page, 0x16, 15, 12, 0x7);
        hal_miim_park_write(unit, port, maxPage, fix_page, 31, 0x325);
        hal_miim_parkBits_write(unit, port, maxPage, fix_page, 0x11, 15, 12, 0x7);
        hal_miim_park_write(unit, port, maxPage, fix_page, 31, 0x322);
        hal_miim_parkBits_write(unit, port, maxPage, fix_page, 0x10, 8, 6, 0x3);
        hal_miim_parkBits_write(unit, port, maxPage, fix_page, 0x13, 8, 6, 0x3);
        hal_miim_parkBits_write(unit, port, maxPage, fix_page, 0x16, 8, 6, 0x3);
        hal_miim_park_write(unit, port, maxPage, fix_page, 31, 0x323);
        hal_miim_parkBits_write(unit, port, maxPage, fix_page, 0x11, 8, 6, 0x3);
        hal_miim_park_write(unit, port, maxPage, fix_page, 31, 0x324);
        hal_miim_parkBits_write(unit, port, maxPage, fix_page, 0x10, 8, 6, 0x3);
        hal_miim_parkBits_write(unit, port, maxPage, fix_page, 0x13, 8, 6, 0x3);
        hal_miim_parkBits_write(unit, port, maxPage, fix_page, 0x16, 8, 6, 0x3);
        hal_miim_park_write(unit, port, maxPage, fix_page, 31, 0x325);
        hal_miim_parkBits_write(unit, port, maxPage, fix_page, 0x11, 8, 6, 0x3);
        hal_miim_park_write(unit, port, maxPage, fix_page, 30, 0x0);
    }
}

void
_phy_8218e_uc_nctl_perPort_init(uint32 unit, rtk_port_t port)
{
    int32   ret = RT_ERR_OK;
    uint32  fix_page = 0x1F;
    uint32  maxPage = HAL_MIIM_FIX_PAGE;

    hal_miim_park_write(unit, port, maxPage, fix_page, 30, 0);
    hal_miim_park_write(unit, port, maxPage, fix_page, 31, 0xB82);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 16, 4, 4, 0x1);

    if ((ret = _phy_rtl8218e_patch_patchReady_wait(unit, port)) != RT_ERR_OK)
        return;

    hal_miim_park_write(unit, port, maxPage, fix_page, 30, 0);
    hal_miim_park_write(unit, port, maxPage, fix_page, 31, 0xA43);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x854f);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xaf85);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x67af);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x8570);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xaf85);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x73af);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x857d);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xaf85);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x9baf);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x85ad);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xaf85);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xadaf);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x85ad);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0255);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xd4e0);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x8459);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xaf32);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x8daf);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x3bb5);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0255);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xedee);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x8456);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x80af);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x3276);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x025f);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x61bf);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x5e1e);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0259);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x7abf);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x5e21);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0259);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x7abf);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x5e24);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0259);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x7abf);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x5e27);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0259);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x7aaf);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x362c);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0255);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0802);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x85ad);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0285);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xe602);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x8693);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0256);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x06af);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x539d);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xf8f9);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xfaef);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x69fa);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xe080);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x15ad);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x2626);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xe084);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xd9ac);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x2020);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xbf59);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xd102);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x597a);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xa103);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x17d2);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0002);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x54bf);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xe082);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x04e1);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x8205);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x1b46);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xaa04);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xee84);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xda01);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x12a2);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x02eb);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xfeef);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x96fe);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xfdfc);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x04f8);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xf9ef);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x59e0);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x8015);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xad26);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x1fe0);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x84d9);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xac20);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x1dbf);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x59d1);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0259);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x7aa1);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0310);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xe08f);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xffac);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x2705);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0286);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x18ae);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0902);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x864d);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xae04);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xee8f);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xff00);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xef95);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xfdfc);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x04f8);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xf9fa);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xfbd2);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0002);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x54bf);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xef46);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xe28f);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xfde3);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x8ffe);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x1b65);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x9e0c);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xe48f);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xfde5);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x8ffe);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xee8f);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xff00);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xae0f);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xee8f);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xff80);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x025e);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xccef);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x57e6);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x8ffb);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xe78f);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xfcff);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xfefd);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xfc04);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xf8f9);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xfafb);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xef79);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xfbe2);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x8ffb);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xe38f);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xfcef);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x65e2);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x8211);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xe382);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x12ef);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x7502);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x5ee7);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xac50);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x1dd2);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0002);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x54bf);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xef46);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xe28f);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xfde3);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x8ffe);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x1b65);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x9e10);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xee8f);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xff00);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xe48f);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xfde5);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x8ffe);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xae04);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xee84);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xda01);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xffef);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x97ff);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xfefd);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xfc04);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xf8f9);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xfaef);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x69bf);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x5a8b);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0259);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x7aad);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x2827);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xbf86);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xce02);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x597a);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xef54);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xbf59);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xfe02);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x597a);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xac29);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0dac);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x2805);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xa302);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0cae);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0ea3);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0307);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xae09);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xa304);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x02ae);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x04ee);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x84da);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x01ef);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x96fe);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xfdfc);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0474);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0xa600);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0xb818);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x328a);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0xb81a);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x3bb2);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0xb81c);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x3272);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0xb81e);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x3629);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0xb846);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x5391);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0xb848);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0000);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0xb84a);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0000);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0xb84c);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0000);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0xb832);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x001f);

    hal_miim_park_write(unit, port, maxPage, fix_page, 30, 0);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0xB82E);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0000);
    hal_miim_park_write(unit, port, maxPage, fix_page, 31, 0xB82);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 16, 4, 4, 0x0);


    hal_miim_park_write(unit, port, maxPage, fix_page, 30, 0);
    hal_miim_park_write(unit, port, maxPage, fix_page, 31, 0xB82);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 16, 4, 4, 0x1);

    if ((ret = _phy_rtl8218e_patch_patchReady_wait(unit, port)) != RT_ERR_OK)
        return;

    hal_miim_park_write(unit, port, maxPage, fix_page, 30, 0);
    hal_miim_park_write(unit, port, maxPage, fix_page, 31, 0xB82);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 16, 7, 7, 0x1);

    hal_miim_park_write(unit, port, maxPage, fix_page, 30, 0);
    hal_miim_park_write(unit, port, maxPage, fix_page, 31, 0xA43);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0xa016);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0000);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0xa012);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0000);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0xa014);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x1800);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x8010);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x1800);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x8014);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x1800);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x8018);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x1800);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x8018);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x1800);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x8018);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x1800);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x8018);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x1800);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x8018);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x1800);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x8018);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x294c);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0d47);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x1800);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0d40);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x294c);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0db3);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x1800);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0daf);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0xa026);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0000);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0xa024);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0000);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0xa022);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0000);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0xa020);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0000);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0xa006);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0000);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0xa004);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0000);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0xa002);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0dad);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0xa000);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0d43);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0xa008);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0300);

    hal_miim_park_write(unit, port, maxPage, fix_page, 30, 0);
    hal_miim_park_write(unit, port, maxPage, fix_page, 31, 0xB82);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 16, 7, 7, 0x0);
    hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0xB82E);
    hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0000);
    hal_miim_park_write(unit, port, maxPage, fix_page, 31, 0xB82);
    hal_miim_parkBits_write(unit, port, maxPage, fix_page, 16, 4, 4, 0x0);
}

void
_phy_8218e_sram_perPort_init(uint32 unit, rtk_port_t port)
{
    uint32  basePort = 0;
    uint32  phyPort = 0;
    uint32  maxPage = HAL_MIIM_FIX_PAGE;
    uint32  fix_page = 0x1F;
    uint32  auto_load = 0;
    uint32  SW_LD_LEN = 0;
    uint32  LEN_L = 0;
    uint32  LEN_S = 0;
    uint32  adc_ioffset_dc = 0;
    uint32  adc_ioffset_dc_cha = 0;
    uint32  adc_ioffset_dc_chb = 0;
    uint32  adc_ioffset_dc_chc = 0;
    uint32  adc_ioffset_dc_chd = 0;
    uint32  adc_ioffset_ac = 0;
    uint32  adc_ioffset_ac_cha = 0;
    uint32  adc_ioffset_ac_chb = 0;
    uint32  adc_ioffset_ac_chc = 0;
    uint32  adc_ioffset_ac_chd = 0;
    uint32  tapbin_cal_ab = 0;
    uint32  tapbin_cal_cd = 0;
    uint32  tapbin_cha = 0;
    uint32  tapbin_chb = 0;
    uint32  tapbin_chc = 0;
    uint32  tapbin_chd = 0;
    uint32  ibadj_cal = 0;
    uint32  len_cal = 0;
    uint32  ctle_cal = 0;
    uint32  en_gdac_ibx = 0;

    basePort = HWP_PHY_BASE_MACID(unit, port);
    phyPort = port - basePort;

    _phy_8218e_efuse_read(unit, basePort, 0, &auto_load);
    if (auto_load == 0x00FF)
    {
        _phy_8218e_efuse_read(unit, basePort, (1970 + 4*phyPort), &adc_ioffset_dc);
        _phy_8218e_efuse_read(unit, basePort, (1972 + 4*phyPort), &adc_ioffset_ac);

        hal_miim_park_write(unit, port, maxPage, fix_page, 30, 0);
        hal_miim_park_write(unit, port, maxPage, fix_page, 31, 0xbcf);
        hal_miim_park_write(unit, port, maxPage, fix_page, 22, adc_ioffset_dc);

        adc_ioffset_dc_cha = (adc_ioffset_dc & 0x000F) >> 0;
        adc_ioffset_dc_chb = (adc_ioffset_dc & 0x00F0) >> 4;
        adc_ioffset_dc_chc = (adc_ioffset_dc & 0x0F00) >> 8;
        adc_ioffset_dc_chd = (adc_ioffset_dc & 0xF000) >> 12;

        adc_ioffset_ac_cha = (adc_ioffset_ac & 0x000F) >> 0;
        adc_ioffset_ac_chb = (adc_ioffset_ac & 0x00F0) >> 4;
        adc_ioffset_ac_chc = (adc_ioffset_ac & 0x0F00) >> 8;
        adc_ioffset_ac_chd = (adc_ioffset_ac & 0xF000) >> 12;

        hal_miim_park_write(unit, port, maxPage, fix_page, 31, 0xA43);
        hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x82b1);
        hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, adc_ioffset_dc_cha);
        hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x82b2);
        hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, adc_ioffset_dc_chb);
        hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x82b3);
        hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, adc_ioffset_dc_chc);
        hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x82b4);
        hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, adc_ioffset_dc_chd);
        hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x82b5);
        hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, adc_ioffset_ac_cha);
        hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x82b6);
        hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, adc_ioffset_ac_chb);
        hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x82b7);
        hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, adc_ioffset_ac_chc);
        hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x82b8);
        hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, adc_ioffset_ac_chd);

        _phy_8218e_efuse_read(unit, basePort, (8 + 14*phyPort), &tapbin_cal_ab);
        _phy_8218e_efuse_read(unit, basePort, (10 + 14*phyPort), &tapbin_cal_cd);

        hal_miim_park_write(unit, port, maxPage, fix_page, 30, 0);
        hal_miim_park_write(unit, port, maxPage, fix_page, 31, 0xbcf);
        hal_miim_park_write(unit, port, maxPage, fix_page, 18, tapbin_cal_cd);
        hal_miim_park_write(unit, port, maxPage, fix_page, 19, tapbin_cal_ab);

        tapbin_cha = (tapbin_cal_ab & 0x001F) >> 0;
        tapbin_chb = (tapbin_cal_ab & 0x1F00) >> 8;
        tapbin_chc = (tapbin_cal_cd & 0x001F) >> 0;
        tapbin_chd = (tapbin_cal_cd & 0x1F00) >> 8;

        hal_miim_park_write(unit, port, maxPage, fix_page, 31, 0xA43);
        hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x830E);
        hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, tapbin_cha);
        hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x830F);
        hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, tapbin_chb);
        hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x8310);
        hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, tapbin_chc);
        hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x8311);
        hal_miim_parkBits_write(unit, port, maxPage, fix_page, 28, 15, 8, tapbin_chd);

        _phy_8218e_efuse_read(unit, basePort, (4 + 14*phyPort), &len_cal);
        hal_miim_park_write(unit, port, maxPage, fix_page, 30, 0);
        hal_miim_park_write(unit, port, maxPage, fix_page, 31, 0xBCD);
        hal_miim_park_write(unit, port, maxPage, fix_page, 22, len_cal);
        hal_miim_park_write(unit, port, maxPage, fix_page, 23, len_cal);
        _phy_8218e_efuse_read(unit, basePort, (4 + 14*phyPort), &ctle_cal);
        hal_miim_park_write(unit, port, maxPage, fix_page, 30, 0);
        hal_miim_park_write(unit, port, maxPage, fix_page, 31, 0xbcb);
        hal_miim_park_write(unit, port, maxPage, fix_page, 17, ctle_cal);
        hal_miim_park_write(unit, port, maxPage, fix_page, 19, ctle_cal);

        _phy_8218e_efuse_read(unit, basePort, (16 + 14*phyPort), &ibadj_cal);
        hal_miim_park_write(unit, port, maxPage, fix_page, 30, 0);
        hal_miim_park_write(unit, port, maxPage, fix_page, 31, 0xBCA);
        hal_miim_park_write(unit, port, maxPage, fix_page, 22, ibadj_cal);
    }
    if (auto_load != 0x00FF)
    {
        hal_miim_park_write(unit, port, maxPage, fix_page, 30, 0);
        hal_miim_park_write(unit, port, maxPage, fix_page, 31, 0xA43);
        hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x82b1);
        hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0808);
        hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x82b3);
        hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0808);
        hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x82b5);
        hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0808);
        hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x82b7);
        hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0808);
        hal_miim_park_write(unit, port, maxPage, fix_page, 30, 0);
        hal_miim_park_write(unit, port, maxPage, fix_page, 31, 0xA43);
        hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x830E);
        hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0808);
        hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x8310);
        hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x0808);
        hal_miim_park_write(unit, port, maxPage, fix_page, 30, 0);
        hal_miim_park_write(unit, port, maxPage, fix_page, 31, 0xBCD);
        hal_miim_park_write(unit, port, maxPage, fix_page, 22, 0x8888);
        hal_miim_park_write(unit, port, maxPage, fix_page, 23, 0x8888);
        hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x82FF);
        hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x8888);
        hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x8301);
        hal_miim_park_write(unit, port, maxPage, fix_page, 28, 0x8888);
    }

    _phy_8218e_efuse_read(unit, basePort, 2034, &SW_LD_LEN);
    if (SW_LD_LEN == 0xF7FF)
    {
        _phy_8218e_efuse_read(unit, basePort, (4 + 14*phyPort), &LEN_L);
        _phy_8218e_efuse_read(unit, basePort, (2002 + 4*phyPort), &LEN_S);
        hal_miim_park_write(unit, port, maxPage, fix_page, 30, 0);
        hal_miim_park_write(unit, port, maxPage, fix_page, 31, 0xA43);
        hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x82FF);
        hal_miim_park_write(unit, port, maxPage, fix_page, 28, LEN_L);
        hal_miim_park_write(unit, port, maxPage, fix_page, 27, 0x8301);
        hal_miim_park_write(unit, port, maxPage, fix_page, 28, LEN_S);
    }

    _phy_8218e_efuse_read(unit, basePort, 2, &en_gdac_ibx);
    if (en_gdac_ibx == 0xFF7F)
    {
        hal_miim_park_write(unit, port, maxPage, fix_page, 30, 0);
        hal_miim_park_write(unit, port, maxPage, fix_page, 31, 0xBCE);
        hal_miim_parkBits_write(unit, port, maxPage, fix_page, 23, 15, 15, 1);
    }
}

void
phy_8218e_conf_perPort_init(uint32 unit, rtk_port_t port)
{
    int32   ret = RT_ERR_OK;
    uint32  basePort = 0;
    uint16  chip_rev_id = 0;

    basePort = HWP_PHY_BASE_MACID(unit, port);

    _phy_8218e_dummyReg_init(unit, basePort);

    if ((ret = phy_8218e_chipRevId_get(unit, basePort, &chip_rev_id)) != RT_ERR_OK)
        return;

    _phy_8218e_gphy_perPort_init(unit, port);

    switch (chip_rev_id)
    {
#if PHY_RTL8218E_C_PATCH
        case PHY_RTL8218E_VER_C:
            _phy_8218e_c_uc_nctl_perPort_init(unit, port);
            break;
#endif
        case PHY_RTL8218E_VER_D:
        default:
            _phy_8218e_uc_nctl_perPort_init(unit, port);
            break;
    }

    _phy_8218e_sram_perPort_init(unit, port);
}

/* Function Name:
 *      _phy_8218e_gphy_init
 * Description:
 *      software patch gphy
 * Input:
 *      unit        - unit id
 *      basePort    - base port id
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void
_phy_8218e_gphy_init(uint32 unit, rtk_port_t basePort)
{
    uint32  port = 0;
    uint32  phyPort = 0;

    for (phyPort = 0; phyPort < PORT_NUM_IN_8218E; phyPort++)
    {
        port = basePort + phyPort;
        _phy_8218e_gphy_perPort_init(unit, port);
    }
}

/* Function Name:
 *      _phy_8218e_uc_nctl_init
 * Description:
 *      software patch uc nctl
 * Input:
 *      unit        - unit id
 *      basePort    - base port id
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void
_phy_8218e_uc_nctl_init(uint32 unit, rtk_port_t basePort)
{
    uint32  port = 0;
    uint32  phyPort = 0;

    for (phyPort = 0; phyPort < PORT_NUM_IN_8218E; phyPort++)
    {
        port = basePort + phyPort;
        _phy_8218e_uc_nctl_perPort_init(unit, port);
    }
}

/* Function Name:
 *      _phy_8218e_sram_init
 * Description:
 *      software patch SRAM
 * Input:
 *      unit        - unit id
 *      basePort    - base port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
void
_phy_8218e_sram_init(uint32 unit, rtk_port_t basePort)
{
    uint32  port = 0;
    uint32  phyPort = 0;

    for (phyPort = 0; phyPort < PORT_NUM_IN_8218E; phyPort++)
    {
        port = basePort + phyPort;
        _phy_8218e_sram_perPort_init(unit, port);
    }
}

/* Function Name:
 *      phy_8218e_conf_init
 * Description:
 *      Set patch to PHY.
 * Input:
 *      unit        - unit id
 *      basePort    - base port id
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void
phy_8218e_conf_init(uint32 unit, rtk_port_t basePort)
{
    int32   ret = RT_ERR_OK;
    uint16  chip_rev_id = 0;

    _phy_8218e_dummyReg_init(unit, basePort);

    if ((ret = phy_8218e_chipRevId_get(unit, basePort, &chip_rev_id)) != RT_ERR_OK)
        return;

    _phy_8218e_gphy_init(unit, basePort);

    switch (chip_rev_id)
    {
#if PHY_RTL8218E_C_PATCH
        case PHY_RTL8218E_VER_C:
            _phy_8218e_c_uc_nctl_init(unit, basePort);
            break;
#endif
        case PHY_RTL8218E_VER_D:
        default:
            _phy_8218e_uc_nctl_init(unit, basePort);
            break;
    }

    _phy_8218e_sram_init(unit, basePort);
}


/* Function Name:
 *      sub_phy_8218e_patch_set
 * Description:
 *      Set patch to PHY.
 * Input:
 *      unit    - unit id
 *      port    - port id
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
sub_phy_8218e_patch_set(uint32 unit, rtk_port_t port)
{
    int32           i;
    hal_control_t   *pHalCtrl = NULL;

    RT_PARAM_CHK((port != HWP_PHY_BASE_MACID(unit, port)), RT_ERR_PORT_ID);

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    /* Re-Patch */
    rtl8218e_config(unit, port);

    for (i = 0; i < PORT_NUM_IN_8218E; i++)
        pHalCtrl->pPhy_ctrl[port]->pPhydrv->fPhydrv_init(unit, port + i);

    return RT_ERR_OK;
}

/* Function Name:
 *      sub_phy_8218e_patch_perPort_set
 * Description:
 *      Set patch to PHY.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
sub_phy_8218e_patch_perPort_set(uint32 unit, rtk_port_t port, rtk_portmask_t *pRst_portmask)
{
    uint32          phyPort = 0;
    hal_control_t   *pHalCtrl = NULL;

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    /* Re-Patch */
    rtl8218e_perPort_config(unit, port, pRst_portmask);

    RTK_PORTMASK_SCAN(*pRst_portmask, phyPort)
    {
        pHalCtrl->pPhy_ctrl[phyPort]->pPhydrv->fPhydrv_init(unit, phyPort);
    }

    return RT_ERR_OK;
}

