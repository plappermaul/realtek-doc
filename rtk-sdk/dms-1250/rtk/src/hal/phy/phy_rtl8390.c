/*
 * Copyright (C) 2009-2016 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : PHY 8390 intra serdes Driver APIs.
 *
 * Feature : PHY 8390 intra serdes Driver APIs
 *
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <hal/common/halctrl.h>
#include <hal/phy/phydef.h>
#include <hal/phy/phy_common.h>
#include <hal/phy/phy_rtl8390.h>
#include <hal/mac/reg.h>
#include <hal/mac/serdes.h>
#include <hal/chipdef/cypress/rtk_cypress_reg_struct.h>

#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/debug/rt_log.h>
#include <dal/dal_construct.h>
#include <dal/cypress/dal_cypress_sds.h>

void _phy_8390_serdes_init_10gr_fiber (uint32 unit, int sds);
int32 _phy_8390_10gSds_restart(uint32 unit, rtk_port_t port);

/*
 * Macro Definition
 */


/*
 * Data Declaration
 */

static rtk_port_10gMedia_t  port10gMedia[RTK_MAX_NUM_OF_PORTS];
static rtk_enable_t         port10gState[RTK_MAX_NUM_OF_PORTS];
static rtk_enable_t         portNwayState[RTK_MAX_NUM_OF_PORTS];
static rtk_enable_t         sgmiiEn = DISABLED;

rt_phyInfo_t phy_8390sds_info =
{
    .phy_num    = PORT_NUM_IN_8390SDS,
    .eth_type   = HWP_GE,
    .isComboPhy = {0, 0, 0, 0, 0, 0, 0, 0},
    .flags      = RTK_PHYINFO_FLAG_NONE,

    .xGePhyLocalDuplexAbilityDev            = 0,
    .xGePhyLocalDuplexAbilityAddr           = 0,
    .xGePhyLocalDuplexAbilityBit            = 0,

    .xGePhyLocal1000MSpeedAbilityDev        = 0,
    .xGePhyLocal1000MSpeedAbilityAddr       = 0,
    .xGePhyLocal1000MSpeedAbilityBit        = 0,

    .xGePhyLinkPartner1000MSpeedAbilityDev  = 0,
    .xGePhyLinkPartner1000MSpeedAbilityAddr = 0,
    .xGePhyLinkPartner1000MSpeedAbilityBit  = 0,
};

rt_phyInfo_t phy_8390sds10G_info =
{
    .phy_num    = PORT_NUM_IN_8390SDS10G,
    .eth_type   = HWP_XGE,
    .isComboPhy = {0, 0, 0, 0, 0, 0, 0, 0},
    .flags      = RTK_PHYINFO_FLAG_NONE,

    .xGePhyLocalDuplexAbilityDev            = 0,
    .xGePhyLocalDuplexAbilityAddr           = 0,
    .xGePhyLocalDuplexAbilityBit            = 0,

    .xGePhyLocal1000MSpeedAbilityDev        = 0,
    .xGePhyLocal1000MSpeedAbilityAddr       = 0,
    .xGePhyLocal1000MSpeedAbilityBit        = 0,

    .xGePhyLinkPartner1000MSpeedAbilityDev  = 0,
    .xGePhyLinkPartner1000MSpeedAbilityAddr = 0,
    .xGePhyLinkPartner1000MSpeedAbilityBit  = 0,
};

void
phy_rtl8390_sds_field_write(uint32 unit, uint32 reg, uint32 endBit,
    uint32 startBit, uint32 val)
{
    uint32  configVal, len, mask;
    uint32  i;

    len = endBit - startBit + 1;

    if (32 == len)
        configVal = val;
    else
    {
        mask = 0;
        for (i = startBit; i <= endBit; ++i)
            mask |= (1 << i);

        ioal_mem32_read(unit, reg, &configVal);
        configVal &= ~(mask);
        configVal |= (val << startBit);
    }
    //RT_LOG(LOG_EVENT, (MOD_DAL|MOD_PORT), "reg 0x%x val 0x%x",reg, configVal);
    ioal_mem32_write(unit, reg, configVal);

    return;
}

void _phy_8390_init_fiber_1g_s12(uint32 unit)
{
    PHY_8390_SERDES_SET(0xb818,  3 , 3  , 0x0);
    PHY_8390_SERDES_SET(0xbbfc,  16 , 16  , 0x1);
    PHY_8390_SERDES_SET(0xbbfc,  19 , 18  , 0x2);
    PHY_8390_SERDES_SET(0xbb58,  0 , 0  , 0x1);
    PHY_8390_SERDES_SET(0xbb58,  4 , 4  , 0x1);
    PHY_8390_SERDES_SET(0xbb58,  16 , 16  , 0x1);
    PHY_8390_SERDES_SET(0xbb58,  20 , 20  , 0x1);

    PHY_8390_SERDES_SET(0xbb38,  15 , 0  , 0x0722);
    PHY_8390_SERDES_SET(0xbb40,  15 , 0  , 0x18f5);
    PHY_8390_SERDES_SET(0xbb00,  11 , 11  , 0x0);
    PHY_8390_SERDES_SET(0xb804,  9 , 8  , 0x0);
    PHY_8390_SERDES_SET(0xb880,  13 , 13  , 0x0);
    PHY_8390_SERDES_SET(0xb880,  6 , 6  , 0x1);
    PHY_8390_SERDES_SET(0xb880,  12 , 12  , 0x1);

    PHY_8390_SERDES_SET(0xbb20,  5 , 5  , 0x1);
    PHY_8390_SERDES_SET(0xbb1c,  11 , 7  , 0x15);

    PHY_8390_SERDES_SET(0xbbf8,  21 , 20  , 0x3);
    PHY_8390_SERDES_SET(0xbb20,  3  , 3  , 0x0);
    PHY_8390_SERDES_SET(0xbb40,  15  , 15  , 0x1);
    osal_time_usleep(100 * 1000);
    PHY_8390_SERDES_SET(0xbb40,  15  , 15  , 0x0);
    PHY_8390_SERDES_SET(0xbbf8,  21 , 20  , 0x1);
    PHY_8390_SERDES_SET(0xbbf8,  21 , 20  , 0x0);
}

void _phy_8390_init_fiber_1g_s13(uint32 unit)
{
    PHY_8390_SERDES_SET(0xb918,  3 , 3  , 0x0);
    PHY_8390_SERDES_SET(0xbbfc,  17 , 17  , 0x1);
    PHY_8390_SERDES_SET(0xbbfc,  21 , 20  , 0x2);
    PHY_8390_SERDES_SET(0xbbd8,  0 , 0  , 0x1);
    PHY_8390_SERDES_SET(0xbbd8,  4 , 4  , 0x1);
    PHY_8390_SERDES_SET(0xbbd8,  16 , 16  , 0x1);
    PHY_8390_SERDES_SET(0xbbd8,  20 , 20  , 0x1);

    PHY_8390_SERDES_SET(0xbbb8,  15 , 0  , 0x0722);
    PHY_8390_SERDES_SET(0xbbc0,  15 , 0  , 0x18f5);
    PHY_8390_SERDES_SET(0xbb80,  11 , 11  , 0x0);
    PHY_8390_SERDES_SET(0xb904,  9 , 8  , 0x0);
    PHY_8390_SERDES_SET(0xb980,  13 , 13  , 0x0);
    PHY_8390_SERDES_SET(0xb980,  6 , 6  , 0x1);
    PHY_8390_SERDES_SET(0xb980,  12 , 12  , 0x1);

    PHY_8390_SERDES_SET(0xbba0,  5 , 5  , 0x1);
    PHY_8390_SERDES_SET(0xbb9c,  11 , 7  , 0x15);

    PHY_8390_SERDES_SET(0xbbf8,  23 , 22  , 0x0003);
    PHY_8390_SERDES_SET(0xbba0,  3  , 3  , 0x0);
    PHY_8390_SERDES_SET(0xbbc0,  15  , 15  , 0x1);

    osal_time_usleep(100 * 1000);

    PHY_8390_SERDES_SET(0xbbc0,  15  , 15  , 0x0);
    PHY_8390_SERDES_SET(0xbbf8,  23 , 22  , 0x0001);
    PHY_8390_SERDES_SET(0xbbf8,  23 , 22  , 0x0000);
}

void _phy_8390_init_fiber_100_s12(uint32 unit)
{
    PHY_8390_SERDES_SET(0xbbfc,  16 , 16  , 0x0);
    PHY_8390_SERDES_SET(0xbbfc,  19 , 18  , 0x0);
    PHY_8390_SERDES_SET(0xbb58,  0 , 0  , 0x0);
    PHY_8390_SERDES_SET(0xbb58,  4 , 4  , 0x0);
    PHY_8390_SERDES_SET(0xbb58,  16 , 16  , 0x0);
    PHY_8390_SERDES_SET(0xbb58,  20 , 20  , 0x0);

    PHY_8390_SERDES_SET(0xb880,  12 , 12  , 0x0);
    PHY_8390_SERDES_SET(0xb880,  13 , 13  , 0x1);
    PHY_8390_SERDES_SET(0xb880,  6 , 6  , 0x0);

    PHY_8390_SERDES_SET(0xbb20,  5 , 5  , 0x1);
    PHY_8390_SERDES_SET(0xbb1c,  11 , 7  , 0x15);
}

void _phy_8390_init_fiber_100_s13(uint32 unit)
{
    PHY_8390_SERDES_SET(0xbbfc,  17 , 17  , 0x0);
    PHY_8390_SERDES_SET(0xbbfc,  21 , 20  , 0x0);
    PHY_8390_SERDES_SET(0xbbd8,  0 , 0  , 0x0);
    PHY_8390_SERDES_SET(0xbbd8,  4 , 4  , 0x0);
    PHY_8390_SERDES_SET(0xbbd8,  16 , 16  , 0x0);
    PHY_8390_SERDES_SET(0xbbd8,  20 , 20  , 0x0);

    PHY_8390_SERDES_SET(0xb980,  12 , 12  , 0x0);
    PHY_8390_SERDES_SET(0xb980,  13 , 13  , 0x1);
    PHY_8390_SERDES_SET(0xb980,  6 , 6  , 0x0);

    PHY_8390_SERDES_SET(0xbba0,  5 , 5  , 0x1);
    PHY_8390_SERDES_SET(0xbb9c,  11 , 7  , 0x15);
}

void _phy_8390_sfp_rst_s8(uint32 unit)
{
    PHY_8390_SERDES_SET(0xb3f8,  21 , 20  , 0x0003);
    PHY_8390_SERDES_SET(0xb320,  3  , 3  , 0x0);
    PHY_8390_SERDES_SET(0xb340,  15  , 15  , 0x1);

    osal_time_usleep(100 * 1000);

    PHY_8390_SERDES_SET(0xb340,  15  , 15  , 0x0);
    PHY_8390_SERDES_SET(0xb3f8,  21 , 20  , 0x0001);
    PHY_8390_SERDES_SET(0xb3f8,  21 , 20  , 0x0000);

    PHY_8390_SERDES_SET(0xb004,  22 , 22  , 0x0001);
    osal_time_usleep(100 * 1000);
    PHY_8390_SERDES_SET(0xb004,  22 , 22  , 0x0000);
}

void _phy_8390_sfp_rst_s12(uint32 unit)
{
    PHY_8390_SERDES_SET(0xbbf8,  21 , 20  , 0x0003);
    PHY_8390_SERDES_SET(0xbb20,  3  , 3  , 0x0);
    PHY_8390_SERDES_SET(0xbb40,  15  , 15  , 0x1);

    osal_time_usleep(100 * 1000);

    PHY_8390_SERDES_SET(0xbb40,  15  , 15  , 0x0);
    PHY_8390_SERDES_SET(0xbbf8,  21 , 20  , 0x0001);
    PHY_8390_SERDES_SET(0xbbf8,  21 , 20  , 0x0000);

    PHY_8390_SERDES_SET(0xb804,  22 , 22  , 0x0001);
    osal_time_usleep(100 * 1000);
    PHY_8390_SERDES_SET(0xb804,  22 , 22  , 0x0000);
}

void _phy_8390_sfp_rst_s13(uint32 unit)
{
    PHY_8390_SERDES_SET(0xbbf8,  23 , 22  , 0x0003);
    PHY_8390_SERDES_SET(0xbba0,  3  , 3  , 0x0);
    PHY_8390_SERDES_SET(0xbbc0,  15  , 15  , 0x1);

    osal_time_usleep(100 * 1000);

    PHY_8390_SERDES_SET(0xbbc0,  15  , 15  , 0x0);
    PHY_8390_SERDES_SET(0xbbf8,  23 , 22  , 0x0001);
    PHY_8390_SERDES_SET(0xbbf8,  23 , 22  , 0x0000);

    PHY_8390_SERDES_SET(0xb904,  22 , 22  , 0x0001);
    osal_time_usleep(100 * 1000);
    PHY_8390_SERDES_SET(0xb904,  22 , 22  , 0x0000);
}

/* Function Name:
 *      phy_8390_serdes_ge_mapperInit
 * Description:
 *      Initialize PHY 8390 serdes driver.
 * Input:
 *      pPhydrv - pointer of phy driver
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void phy_8390_serdes_ge_mapperInit(rt_phydrv_t *pPhydrv)
{
    pPhydrv->phydrv_index = RT_PHYDRV_RTL8390_SERDES_GE;
    pPhydrv->fPhydrv_init = phy_8390_init;
    pPhydrv->fPhydrv_enable_set = phy_8390_enable_set;
#if !defined(__BOOTLOADER__)
    pPhydrv->fPhydrv_media_get = phy_8390_media_get;
    pPhydrv->fPhydrv_autoNegoEnable_get = phy_8390_autoNegoEnable_get;
    pPhydrv->fPhydrv_autoNegoEnable_set = phy_8390_autoNegoEnable_set;
    pPhydrv->fPhydrv_autoNegoAbility_get = phy_8390_autoNegoAbility_get;
    pPhydrv->fPhydrv_autoNegoAbility_set = phy_8390_autoNegoAbility_set;
    pPhydrv->fPhydrv_duplex_get = phy_8390_duplex_get;
    pPhydrv->fPhydrv_duplex_set = phy_8390_duplex_set;
    pPhydrv->fPhydrv_speed_get = phy_8390_speed_get;
    pPhydrv->fPhydrv_speed_set = phy_8390_speed_set;
    pPhydrv->fPhydrv_gigaLiteEnable_get = phy_8390_gigaLiteEnable_get;
    pPhydrv->fPhydrv_reg_get = phy_common_reg_get;
    pPhydrv->fPhydrv_reg_set = phy_common_reg_set;
    pPhydrv->fPhydrv_reg_park_get = phy_common_reg_park_get;
    pPhydrv->fPhydrv_reg_park_set = phy_common_reg_park_set;
    pPhydrv->fPhydrv_reg_mmd_get = phy_common_reg_mmd_get;
    pPhydrv->fPhydrv_reg_mmd_set = phy_common_reg_mmd_set;
    pPhydrv->fPhydrv_reg_mmd_portmask_set = phy_common_reg_mmd_portmask_set;
    pPhydrv->fPhydrv_reg_extParkPage_get = phy_common_reg_extParkPage_get;
    pPhydrv->fPhydrv_reg_extParkPage_set = phy_common_reg_extParkPage_set;
    pPhydrv->fPhydrv_reg_extParkPage_portmask_set = phy_common_reg_extParkPage_portmask_set;
    pPhydrv->fPhydrv_masterSlave_get = phy_8390_masterSlave_get;
    pPhydrv->fPhydrv_masterSlave_set = phy_8390_masterSlave_set;
    pPhydrv->fPhydrv_serdesFiberRx_check = phy_8390_serdesFiberRx_check;
    pPhydrv->fPhydrv_10gMedia_get = phy_8390_10gMedia_get;
    pPhydrv->fPhydrv_10gMedia_set = phy_8390_10gMedia_set;
    pPhydrv->fPhydrv_loopback_get = phy_8390_serdes_loopback_get;
    pPhydrv->fPhydrv_loopback_set = phy_8390_serdes_loopback_set;
    pPhydrv->fPhydrv_sds_get = phy_8390_sds_get;
    pPhydrv->fPhydrv_sds_set = phy_8390_sds_set;
#endif
} /* end of phy_8390_serdes_ge_mapperInit*/

int _phy_8390_serdes_10g_eq_dump (uint32 unit, int sds_num)
{
    uint32  val;
    int     ofst, Bin;
    int     binMap[] = {0, 1, 3, 2, 7, 6, 4, 5, 15, 14, \
                        12, 13, 8, 9, 11, 10, 31, 30, 28, 29,   \
                        24, 25, 27, 26, 16, 17, 19, 18, 23, 22, \
                        20, 21};

    if (8 == sds_num)
    {
        ofst = 0;
    }
    else
    {
        ofst = 0x800;
    }

    PHY_8390_SERDES_SET(0xb320 + ofst,  4  , 4  , 1);
    PHY_8390_SERDES_SET(0xb320 + ofst,  11  , 10  , 0x3);
    PHY_8390_SERDES_SET(0xb34c + ofst,  31  , 28  , 0x0);

    PHY_8390_SERDES_SET(0xb340 + ofst,  4  , 2  , 0x4);

    PHY_8390_SERDES_SET(0xb038 + ofst,  9  , 0  , 0x2);
    PHY_8390_SERDES_SET(0xb038 + ofst,  10  , 10  , 0x1);
    PHY_8390_SERDES_SET(0xb038 + ofst,  11  , 11  , 0x1);

    /* [22:18] */
    ioal_mem32_read(unit, 0xb038 + ofst, &val);
    val = ((val >> 18) & 0x1F);
    Bin = binMap[val];

    return Bin;
}   /* end of _phy_8390_serdes_10g_eq_dump */


void _phy_8390_serdes_10g_lcpll (uint32 unit, int sds)
{
    if (8 == sds)
    {
        PHY_8390_SERDES_SET(0xb344,  9  , 0  , 0x200);
        PHY_8390_SERDES_SET(0xb348,  15 , 15 , 1);
        PHY_8390_SERDES_SET(0xb348,  14 , 10 , 0xe);
        PHY_8390_SERDES_SET(0xb348,  9  , 0  , 0x200);
        PHY_8390_SERDES_SET(0xb34c,  11 , 10 , 0x0);
        PHY_8390_SERDES_SET(0xb34c,  9  , 8  , 0x0);
        PHY_8390_SERDES_SET(0xb344,  9  , 0  , 0x200);
    }
    else if (12 == sds)
    {
        PHY_8390_SERDES_SET(0xbb44,  9  , 0  , 0x200);
        PHY_8390_SERDES_SET(0xbb48,  15 , 15 , 1);
        PHY_8390_SERDES_SET(0xbb48,  14 , 10 , 0xe);
        PHY_8390_SERDES_SET(0xbb48,  9  , 0  , 0x200);
        PHY_8390_SERDES_SET(0xbb4c,  11 , 10 , 0x0);
        PHY_8390_SERDES_SET(0xbb4c,  9  , 8  , 0x0);
        PHY_8390_SERDES_SET(0xbb44,  9  , 0  , 0x200);
    }

    hal_mac_serdes_rst(unit, sds);
}   /* end of _phy_8390_serdes_10g_lcpll */

void _phy_8390_serdes_rle0689b_10g_qsgmii_patch (uint32 unit, int sds)
{
    if (8 == sds)
    {
        PHY_8390_SERDES_SET(0xb018,  3  , 3  , 0);
        PHY_8390_SERDES_SET(0xb3fc,  31 , 16 , 0x0);
        PHY_8390_SERDES_SET(0xb300,  15 , 0  , 0xF800);
        PHY_8390_SERDES_SET(0xb300,  31 , 16 , 0xC000);
        PHY_8390_SERDES_SET(0xb304,  15 , 0  , 0x57EF);
        PHY_8390_SERDES_SET(0xb304,  31 , 16 , 0xF000);
        PHY_8390_SERDES_SET(0xb308,  15 , 0  , 0x1000);
        PHY_8390_SERDES_SET(0xb308,  31 , 16 , 0x8000);
        PHY_8390_SERDES_SET(0xb30c,  15 , 0  , 0xBFFF);
        PHY_8390_SERDES_SET(0xb30c,  31 , 16 , 0xFFFF);
        PHY_8390_SERDES_SET(0xb310,  15 , 0  , 0xFFFF);
        PHY_8390_SERDES_SET(0xb310,  31 , 16 , 0x806F);
        PHY_8390_SERDES_SET(0xb314,  15 , 0  , 0x03E4);
        PHY_8390_SERDES_SET(0xb314,  31 , 16 , 0x7FFF);
        PHY_8390_SERDES_SET(0xb318,  15 , 0  , 0x3CC3);
        PHY_8390_SERDES_SET(0xb318,  31 , 16 , 0x23FF);
        PHY_8390_SERDES_SET(0xb31c,  15 , 0  , 0x7A00);
        PHY_8390_SERDES_SET(0xb31c,  31 , 16 , 0x2424);
        PHY_8390_SERDES_SET(0xb320,  15 , 0  , 0x1F16);
        PHY_8390_SERDES_SET(0xb320,  31 , 16 , 0xB84A);
        PHY_8390_SERDES_SET(0xb324,  15 , 0  , 0x96B2);
        PHY_8390_SERDES_SET(0xb324,  31 , 16 , 0xF300);
        PHY_8390_SERDES_SET(0xb328,  15 , 0  , 0x1F53);
        PHY_8390_SERDES_SET(0xb328,  31 , 16 , 0x0000);
        PHY_8390_SERDES_SET(0xb32c,  15 , 0  , 0x0000);
        PHY_8390_SERDES_SET(0xb32c,  31 , 16 , 0xFFFF);
        PHY_8390_SERDES_SET(0xb330,  15 , 0  , 0x7000);
        PHY_8390_SERDES_SET(0xb330,  31 , 16 , 0x0000);
        PHY_8390_SERDES_SET(0xb334,  15 , 0  , 0xFFFF);
        PHY_8390_SERDES_SET(0xb334,  31 , 16 , 0x0800);
        PHY_8390_SERDES_SET(0xb338,  15 , 0  , 0x1018);
        PHY_8390_SERDES_SET(0xb338,  31 , 16 , 0x0200);
        PHY_8390_SERDES_SET(0xb33c,  15 , 0  , 0x0003);
        PHY_8390_SERDES_SET(0xb33c,  31 , 16 , 0xBE10);
        PHY_8390_SERDES_SET(0xb340,  15 , 0  , 0x78F1);
        PHY_8390_SERDES_SET(0xb340,  31 , 16 , 0x9F87);
        PHY_8390_SERDES_SET(0xb344,  15 , 0  , 0x4200);
        PHY_8390_SERDES_SET(0xb344,  31 , 16 , 0xFFCD);
        PHY_8390_SERDES_SET(0xb348,  15 , 0  , 0xBA00);
        PHY_8390_SERDES_SET(0xb348,  31 , 16 , 0x604F);
        PHY_8390_SERDES_SET(0xb34c,  15 , 0  , 0x40AA);
        PHY_8390_SERDES_SET(0xb34c,  31 , 16 , 0x0057);
        PHY_8390_SERDES_SET(0xb350,  15 , 0  , 0xC3E0);
        PHY_8390_SERDES_SET(0xb350,  31 , 16 , 0x033E);
        PHY_8390_SERDES_SET(0xb354,  15 , 0  , 0x619C);
        PHY_8390_SERDES_SET(0xb354,  31 , 16 , 0x2002);
        PHY_8390_SERDES_SET(0xb358,  15 , 0  , 0xFAE2);
        PHY_8390_SERDES_SET(0xb358,  31 , 16 , 0xFEFA);
        PHY_8390_SERDES_SET(0xb35c,  15 , 0  , 0x4E10);
        PHY_8390_SERDES_SET(0xb35c,  31 , 16 , 0x4E10);
        PHY_8390_SERDES_SET(0xb360,  15 , 0  , 0x0000);
        PHY_8390_SERDES_SET(0xb360,  31 , 16 , 0x0000);
    }
    else if (12 == sds)
    {
        PHY_8390_SERDES_SET(0xb818,  3  , 3  , 0);
        PHY_8390_SERDES_SET(0xbbfc,  31 , 16 , 0x0);
        PHY_8390_SERDES_SET(0xbb00,  15 , 0  , 0xF800);
        PHY_8390_SERDES_SET(0xbb00,  31 , 16 , 0xC000);
        PHY_8390_SERDES_SET(0xbb04,  15 , 0  , 0x57EF);
        PHY_8390_SERDES_SET(0xbb04,  31 , 16 , 0xF000);
        PHY_8390_SERDES_SET(0xbb08,  15 , 0  , 0x1000);
        PHY_8390_SERDES_SET(0xbb08,  31 , 16 , 0x8000);
        PHY_8390_SERDES_SET(0xbb0c,  15 , 0  , 0xBFFF);
        PHY_8390_SERDES_SET(0xbb0c,  31 , 16 , 0xFFFF);
        PHY_8390_SERDES_SET(0xbb10,  15 , 0  , 0xFFFF);
        PHY_8390_SERDES_SET(0xbb10,  31 , 16 , 0x806F);
        PHY_8390_SERDES_SET(0xbb14,  15 , 0  , 0x03E4);
        PHY_8390_SERDES_SET(0xbb14,  31 , 16 , 0x7FFF);
        PHY_8390_SERDES_SET(0xbb18,  15 , 0  , 0x3CC3);
        PHY_8390_SERDES_SET(0xbb18,  31 , 16 , 0x23FF);
        PHY_8390_SERDES_SET(0xbb1c,  15 , 0  , 0x7A00);
        PHY_8390_SERDES_SET(0xbb1c,  31 , 16 , 0x2424);
        PHY_8390_SERDES_SET(0xbb20,  15 , 0  , 0x1F16);
        PHY_8390_SERDES_SET(0xbb20,  31 , 16 , 0xB84A);
        PHY_8390_SERDES_SET(0xbb24,  15 , 0  , 0x96B2);
        PHY_8390_SERDES_SET(0xbb24,  31 , 16 , 0xF300);
        PHY_8390_SERDES_SET(0xbb28,  15 , 0  , 0x1F53);
        PHY_8390_SERDES_SET(0xbb28,  31 , 16 , 0x0000);
        PHY_8390_SERDES_SET(0xbb2c,  15 , 0  , 0x0000);
        PHY_8390_SERDES_SET(0xbb2c,  31 , 16 , 0xFFFF);
        PHY_8390_SERDES_SET(0xbb30,  15 , 0  , 0x7000);
        PHY_8390_SERDES_SET(0xbb30,  31 , 16 , 0x0000);
        PHY_8390_SERDES_SET(0xbb34,  15 , 0  , 0xFFFF);
        PHY_8390_SERDES_SET(0xbb34,  31 , 16 , 0x0800);
        PHY_8390_SERDES_SET(0xbb38,  15 , 0  , 0x1018);
        PHY_8390_SERDES_SET(0xbb38,  31 , 16 , 0x0200);
        PHY_8390_SERDES_SET(0xbb3c,  15 , 0  , 0x0003);
        PHY_8390_SERDES_SET(0xbb3c,  31 , 16 , 0xBE10);
        PHY_8390_SERDES_SET(0xbb40,  15 , 0  , 0x78F1);
        PHY_8390_SERDES_SET(0xbb40,  31 , 16 , 0x9F87);
        PHY_8390_SERDES_SET(0xbb44,  15 , 0  , 0x4200);
        PHY_8390_SERDES_SET(0xbb44,  31 , 16 , 0xFFCD);
        PHY_8390_SERDES_SET(0xbb48,  15 , 0  , 0xBA00);
        PHY_8390_SERDES_SET(0xbb48,  31 , 16 , 0x604F);
        PHY_8390_SERDES_SET(0xbb4c,  15 , 0  , 0x40AA);
        PHY_8390_SERDES_SET(0xbb4c,  31 , 16 , 0x0057);
        PHY_8390_SERDES_SET(0xbb50,  15 , 0  , 0xC3E0);
        PHY_8390_SERDES_SET(0xbb50,  31 , 16 , 0x033E);
        PHY_8390_SERDES_SET(0xbb54,  15 , 0  , 0x619C);
        PHY_8390_SERDES_SET(0xbb54,  31 , 16 , 0x2002);
        PHY_8390_SERDES_SET(0xbb58,  15 , 0  , 0xFAE2);
        PHY_8390_SERDES_SET(0xbb58,  31 , 16 , 0xFEFA);
        PHY_8390_SERDES_SET(0xbb5c,  15 , 0  , 0x4E10);
        PHY_8390_SERDES_SET(0xbb5c,  31 , 16 , 0x4E10);
        PHY_8390_SERDES_SET(0xbb60,  15 , 0  , 0x0000);
        PHY_8390_SERDES_SET(0xbb60,  31 , 16 , 0x0000);
    }

    hal_mac_serdes_rst(unit, sds);
}   /* end of _phy_8390_serdes_rle0689b_10g_qsgmii_patch */

void _phy_8390_serdes_init_96m_1g_fiber (uint32 unit, int sds)
{
    uint32  ofst;

    if (8 == sds)
    {
        ofst = 0x0;
    }
    else
    {
        ofst = 0x800;
    }

    PHY_8390_SERDES_SET(0xb014 + ofst,  14  , 14  , 0x0);
    PHY_8390_SERDES_SET(0xb004 + ofst,  30  , 30  , 0x0);
    PHY_8390_SERDES_SET(0xb004 + ofst,  23  , 23  , 0x0);

    PHY_8390_SERDES_SET(0xb080 + ofst,  11 , 11  , 0x1);

    if (HWP_CHIP_REV(unit) < CHIP_REV_ID_C)
    {
        _phy_8390_serdes_init_10gr_fiber(unit, sds);
        _phy_8390_serdes_10g_lcpll(unit, sds);

        if (8 == sds)
        {
            PHY_8390_SERDES_SET(0xb000,  8  , 8  , 1);
            PHY_8390_SERDES_SET(0xb004,  9  , 8  , 0x0);

            PHY_8390_SERDES_SET(0xb34c,  15 , 0  , 0x40aa);
            PHY_8390_SERDES_SET(0xb35c,  15 , 0  , 0x4e10);
            PHY_8390_SERDES_SET(0xb35c,  31 , 16 , 0x4e10);
            PHY_8390_SERDES_SET(0x000c,  3  , 0  , 0x7);
            PHY_8390_SERDES_SET(0xb018,  3  , 3  , 0);
            PHY_8390_SERDES_SET(0xb3fc,  31 , 16 , 0x0);
            PHY_8390_SERDES_SET(0xb358,  0  , 0  , 0);
            PHY_8390_SERDES_SET(0xb358,  4  , 4  , 0);
            PHY_8390_SERDES_SET(0xb358,  16 , 16 , 0);
            PHY_8390_SERDES_SET(0xb358,  20 , 20 , 0);
            PHY_8390_SERDES_SET(0xb338,  15 , 0  , 0x0722);
            PHY_8390_SERDES_SET(0xb340,  15 , 0  , 0x18f5);
            PHY_8390_SERDES_SET(0xb300,  11 , 11 , 0);
            PHY_8390_SERDES_SET(0xb080,  13 , 13 , 0);
            PHY_8390_SERDES_SET(0xb080,  6  , 6  , 1);
            PHY_8390_SERDES_SET(0xb080,  12 , 12 , 1);
            PHY_8390_SERDES_SET(0xb088,  7  , 7  , 1);
            PHY_8390_SERDES_SET(0xb088,  8  , 8  , 1);
            PHY_8390_SERDES_SET(0xb320,  5  , 5  , 1);
            PHY_8390_SERDES_SET(0xb31c,  11 , 7  , 0x16);
            PHY_8390_SERDES_SET(0x031c,  7  , 0  , 0x74);
        }
        else if (12 == sds)
        {
            PHY_8390_SERDES_SET(0xb800,  8  , 8  , 1);
            PHY_8390_SERDES_SET(0xb804,  9  , 8  , 0x0);

            PHY_8390_SERDES_SET(0xbb4c,  15 , 0  , 0x40aa);
            PHY_8390_SERDES_SET(0xbb5c,  15 , 0  , 0x4e10);
            PHY_8390_SERDES_SET(0xbb5c,  31 , 16 , 0x4e10);

            PHY_8390_SERDES_SET(0x000c,  19 , 16 , 0x7);

            PHY_8390_SERDES_SET(0xb818,  3  , 3  , 0);
            PHY_8390_SERDES_SET(0xbbfc,  31 , 16 , 0x0);
            PHY_8390_SERDES_SET(0xbb58,  0  , 0  , 0);
            PHY_8390_SERDES_SET(0xbb58,  4  , 4  , 0);
            PHY_8390_SERDES_SET(0xbb58,  16 , 16 , 0);
            PHY_8390_SERDES_SET(0xbb58,  20 , 20 , 0);
            PHY_8390_SERDES_SET(0xbb38,  15 , 0  , 0x0722);
            PHY_8390_SERDES_SET(0xbb40,  15 , 0  , 0x18f5);
            PHY_8390_SERDES_SET(0xbb00,  11 , 11 , 0);
            PHY_8390_SERDES_SET(0xb804,  9  , 8  , 0x0);
            PHY_8390_SERDES_SET(0xb880,  13 , 13 , 0);
            PHY_8390_SERDES_SET(0xb880,  6  , 6  , 1);
            PHY_8390_SERDES_SET(0xb880,  12 , 12 , 1);
            PHY_8390_SERDES_SET(0xb888,  7  , 7  , 1);
            PHY_8390_SERDES_SET(0xb888,  8  , 8  , 1);
            PHY_8390_SERDES_SET(0xbb20,  5  , 5  , 1);
            PHY_8390_SERDES_SET(0xbb1c,  11 , 7  , 0x16);

            PHY_8390_SERDES_SET(0x034c,  7  , 0  , 0x74);
        }
    }
    else
    {
        if (8 == sds)
        {
            PHY_8390_SERDES_SET(0x000c,  3 , 0  , 0x0);
        }
        else
        {
            PHY_8390_SERDES_SET(0x000c,  19 , 16  , 0x0);
        }

        _phy_8390_serdes_rle0689b_10g_qsgmii_patch(unit, sds);

        if (8 == sds)
        {
            PHY_8390_SERDES_SET(0xb004,  9  , 8  , 0x0);

            PHY_8390_SERDES_SET(0xb080,  13 , 13 , 0);
            PHY_8390_SERDES_SET(0xb080,  6  , 6  , 1);
            PHY_8390_SERDES_SET(0xb080,  12 , 12 , portNwayState[24]);

            PHY_8390_SERDES_SET(0xb088,  7  , 7  , 1);
            PHY_8390_SERDES_SET(0xb088,  8  , 8  , 1);

            PHY_8390_SERDES_SET(0xb320,  5  , 5  , 1);
            PHY_8390_SERDES_SET(0xb31c,  11 , 7  , 0x16);

            PHY_8390_SERDES_SET(0x000c,  3 , 0  , 0x7);
        }
        else
        {
            PHY_8390_SERDES_SET(0xb804,  9  , 8  , 0x0);

            PHY_8390_SERDES_SET(0xb880,  13 , 13 , 0);
            PHY_8390_SERDES_SET(0xb880,  6  , 6  , 1);
            PHY_8390_SERDES_SET(0xb880,  12 , 12 , portNwayState[36]);

            PHY_8390_SERDES_SET(0xb888,  7  , 7  , 1);
            PHY_8390_SERDES_SET(0xb888,  8  , 8  , 1);

            PHY_8390_SERDES_SET(0xbb20,  5  , 5  , 1);
            PHY_8390_SERDES_SET(0xbb1c,  11 , 7  , 0x16);

            PHY_8390_SERDES_SET(0x000c,  19 , 16  , 0x7);
        }
    }

    hal_mac_serdes_rst(unit, sds);

    PHY_8390_SERDES_SET(0xb080 + ofst,  11 , 11  , 0x0);
}   /* end of _phy_8390_serdes_init_96m_1g_fiber */

void _phy_8390_serdes_init_96m_100m_fiber (uint32 unit, int sds)
{
    uint32  ofst;

    if (8 == sds)
    {
        ofst = 0x0;
    }
    else
    {
        ofst = 0x800;
    }

    _phy_8390_serdes_init_96m_1g_fiber(unit, sds);
    if (8 == sds)
    {
        PHY_8390_SERDES_SET(0x000c,  3 , 0  , 0x8);
    }
    else
    {
        PHY_8390_SERDES_SET(0x000c,  19 , 16  , 0x8);
    }

    PHY_8390_SERDES_SET(0xb340 + ofst,  13 , 13  , 0x0);

    PHY_8390_SERDES_SET(0xb080 + ofst,  12 , 12  , 0x0);
    PHY_8390_SERDES_SET(0xb080 + ofst,  13 , 13  , 0x1);
    PHY_8390_SERDES_SET(0xb080 + ofst,  6  , 6   , 0x0);
}   /* end of _phy_8390_serdes_init_96m_100m_fiber */

void _phy_8390_serdes_10g_foreground_offset_range_cali (uint32 unit, int sds_num)
{
    uint32  cnt = 0, record;
    uint32  ofst, ofst_range = 0, fgcal_offset;

    switch (sds_num)
    {
        case 8:
            ofst = 0x0;
            break;
        case 12:
            ofst = 0x800;
            break;
        default:
            return;
    }

    //PHY_8390_SERDES_SET(0xb318 + ofst,  26  , 26  , 0x1);
    PHY_8390_SERDES_SET(0xb320 + ofst,  15  , 14  , 0x0);
    PHY_8390_SERDES_SET(0xb320 + ofst,  4  , 4  , 0x1);
    PHY_8390_SERDES_SET(0xb320 + ofst,  11  , 10  , 0x3);
    PHY_8390_SERDES_SET(0xb34c + ofst,  31  , 28  , 0x0);

    PHY_8390_SERDES_SET(0xb340 + ofst,  4  , 2  , 0x3);
    PHY_8390_SERDES_SET(0xb038 + ofst,  9  , 0  , 0x0);
    PHY_8390_SERDES_SET(0xb038 + ofst,  10  , 10  , 0x1);
    PHY_8390_SERDES_SET(0xb038 + ofst,  11  , 11  , 0x1);

    while (cnt < 4)
    {
        PHY_8390_SERDES_SET(0xb320 + ofst,  15  , 14  , ofst_range);

        for (record = 0; record < 10; ++record)
        {
            PHY_8390_SERDES_SET(0xb340 + ofst,  15  , 15  , 0x1);
            osal_time_usleep(100 * 1000);
            PHY_8390_SERDES_SET(0xb340 + ofst,  15  , 15  , 0x0);

            PHY_8390_SERDES_SET(0xb344 + ofst,  21  , 16  , 0xe);
            /* [19:16] */
            ioal_mem32_read(unit, 0xb038 + ofst, &fgcal_offset);
            fgcal_offset = ((fgcal_offset >> 16) & 0xF);

            ioal_mem32_read(unit, 0xb320 + ofst, &ofst_range);
            ofst_range = ((ofst_range >> 14) & 0x3);

            if ((fgcal_offset != 0x0) && (fgcal_offset != 0xf))
            {
                break;
            }
        }

        if (10 == record)
        {
            ofst_range += 1;
        }
        else
            break;

        ++cnt;
    }
}   /* end of _phy_8390_serdes_10g_foreground_offset_range_cali */

void _phy_8390_serdes_10g_rx_rst (uint32 unit, int sds_num)
{
    uint32  ofst;

    switch (sds_num)
    {
        case 8:
            ofst = 0x0;
            break;
        case 12:
            ofst = 0x800;
            break;
        default:
            return;
    }

    PHY_8390_SERDES_SET(0xb320 + ofst,  3 , 3 , 0x0);
    PHY_8390_SERDES_SET(0xB340 + ofst,  15,  15 , 1);
    osal_time_usleep(100 * 1000);
    PHY_8390_SERDES_SET(0xB340 + ofst,  15,  15 , 0);

    PHY_8390_SERDES_SET(0xB3F8 + ofst, 29, 28, 0x3);
    PHY_8390_SERDES_SET(0xB3F8 + ofst, 17, 16, 0x3);
    PHY_8390_SERDES_SET(0xB3F8 + ofst, 21, 20, 0x1);
    PHY_8390_SERDES_SET(0xB3F8 + ofst, 25, 24, 0x3);

    PHY_8390_SERDES_SET(0xB284 + ofst,  6,  6, 0x1);
    PHY_8390_SERDES_SET(0xB284 + ofst,  5,  5, 0x0);
    PHY_8390_SERDES_SET(0xB284 + ofst,  4,  4, 0x1);
    PHY_8390_SERDES_SET(0xB284 + ofst,  3,  3, 0x1);
    PHY_8390_SERDES_SET(0xB284 + ofst,  2,  2, 0x0);
    PHY_8390_SERDES_SET(0xB284 + ofst,  1,  1, 0x1);
    PHY_8390_SERDES_SET(0xB284 + ofst,  0,  0, 0x1);

    PHY_8390_SERDES_SET(0xB284 + ofst,  0,  0, 0x0);
    PHY_8390_SERDES_SET(0xB3F8 + ofst, 29, 28, 0x0);
    PHY_8390_SERDES_SET(0xB3F8 + ofst, 17, 16, 0x0);
    PHY_8390_SERDES_SET(0xB3F8 + ofst, 21, 20, 0x0);
    PHY_8390_SERDES_SET(0xB3F8 + ofst, 25, 24, 0x0);
}   /* end of _phy_8390_serdes_10g_rx_rst */


void phy_8390_serdes_10g_leq_init (uint32 unit, rtk_port_t port)
{
    uint32  ofst, sds;

    if (HWP_CHIP_REV(unit) <= CHIP_REV_ID_C)
        return;

    if (DISABLED == port10gState[port])
        return;

    if (24 == port)
    {
        ofst = 0;
        sds = 8;
    }
    else
    {
        ofst = 0x800;
        sds = 12;
    }

    PHY_8390_SERDES_SET(0xb300 + ofst,  24 , 24 , 0);
    PHY_8390_SERDES_SET(0xb300 + ofst,  23 , 23 , 1);
    PHY_8390_SERDES_SET(0xb300 + ofst,  22 , 18 , 0xf);

    PHY_8390_SERDES_SET(0xb320 + ofst,  3  , 3  , 0);
    PHY_8390_SERDES_SET(0xb340 + ofst,  15 , 15 , 0x1);
    osal_time_usleep(10 * 1000);
    PHY_8390_SERDES_SET(0xb340 + ofst,  15 , 15 , 0x0);

    _phy_8390_serdes_10g_rx_rst(unit, sds);
}   /* end of phy_8390_serdes_10g_leq_init */

void _phy_8390_serdes_10g_fiber_dfe_adapt (uint32 unit, int sds_num)
{
    uint32  leqCnt, leqVal;
    int32   ofst;
    int32   leq;
    uint32  val, i;
    int32   cnt = 10;
    int32   tap0_sum = 0;
    int32   tap0_avg;

    if (8 == sds_num)
    {
        ofst = 0;
    }
    else
    {
        ofst = 0x800;
    }

    PHY_8390_SERDES_SET(0xb338 + ofst,  14 , 14 , 1);
    PHY_8390_SERDES_SET(0xb360 + ofst,  31 , 31 , 1);
    PHY_8390_SERDES_SET(0xb318 + ofst,  31 , 29 , 0x7);
    PHY_8390_SERDES_SET(0xb31c + ofst,  27 , 25 , 0x7);
    PHY_8390_SERDES_SET(0xb310 + ofst,  30 , 28 , 0x7);
    PHY_8390_SERDES_SET(0xb310 + ofst,  27 , 25 , 0x7);
    PHY_8390_SERDES_SET(0xb304 + ofst,  9  , 5  , 0x3);

    PHY_8390_SERDES_SET(0xb32c + ofst,  21 , 16 , 0x1f);
    PHY_8390_SERDES_SET(0xb32c + ofst,  5  , 0  , 0x0);
    PHY_8390_SERDES_SET(0xb304 + ofst,  27 , 22 , 0x0);
    PHY_8390_SERDES_SET(0xb304 + ofst,  21 , 16 , 0x0);
    PHY_8390_SERDES_SET(0xb330 + ofst,  11 , 6  , 0x0);
    PHY_8390_SERDES_SET(0xb318 + ofst,  10 , 10 , 1);
    PHY_8390_SERDES_SET(0xb318 + ofst,  9  , 9  , 1);
    PHY_8390_SERDES_SET(0xb318 + ofst,  8  , 8  , 1);
    PHY_8390_SERDES_SET(0xb318 + ofst,  2  , 2  , 0);
    PHY_8390_SERDES_SET(0xb318 + ofst,  1  , 1  , 0);
    PHY_8390_SERDES_SET(0xb318 + ofst,  0  , 0  , 0);
    PHY_8390_SERDES_SET(0xb350 + ofst,  26 , 26 , 0);
    PHY_8390_SERDES_SET(0xb350 + ofst,  25 , 25 , 0);

    PHY_8390_SERDES_SET(0xb340 + ofst,  28 , 27 , 0x2);

    PHY_8390_SERDES_SET(0xb300 + ofst,  24 , 24 , 1);
    PHY_8390_SERDES_SET(0xb300 + ofst,  23 , 23 , 0);
    PHY_8390_SERDES_SET(0xb318 + ofst,  26 , 26 , 0);
    PHY_8390_SERDES_SET(0xb320 + ofst,  3  , 3  , 0);

    leqVal = 0;
    leqCnt = 0;
    for (i = 0; i < cnt; ++i)
    {
        do {
            PHY_8390_SERDES_SET(0xb340 + ofst,  15 , 15 , 0x1);
            osal_time_usleep(10 * 1000);
            PHY_8390_SERDES_SET(0xb340 + ofst,  15 , 15 , 0x0);
            osal_time_usleep(500 * 1000);

            leq = _phy_8390_serdes_10g_eq_dump(unit, sds_num);
        } while (leq == 31);

        if (leqVal != leq)
        {
            leqCnt = 1;
            leqVal = leq;
        }
        else
            leqCnt++;

        if ((cnt/2) == leqCnt)
        {
            break;
        }
    }

    leq = _phy_8390_serdes_10g_eq_dump(unit, sds_num);
    PHY_8390_SERDES_SET(0xb300 + ofst,  24 , 24 , 0);
    PHY_8390_SERDES_SET(0xb300 + ofst,  23 , 23 , 1);
    PHY_8390_SERDES_SET(0xb300 + ofst,  22 , 18 , leq);

    PHY_8390_SERDES_SET(0xb320 + ofst,  4  , 4  , 1);
    PHY_8390_SERDES_SET(0xb320 + ofst,  11 , 10 , 0x3);
    PHY_8390_SERDES_SET(0xb340 + ofst,  4  , 2  , 0x3);
    PHY_8390_SERDES_SET(0xb34c + ofst,  31 , 28 , 0x0);
    PHY_8390_SERDES_SET(0xb038 + ofst,  11 , 11 , 1);
    PHY_8390_SERDES_SET(0xb038 + ofst,  10 , 10 , 1);
    PHY_8390_SERDES_SET(0xb038 + ofst,  9  , 0  , 0x0);

    PHY_8390_SERDES_SET(0xb338 + ofst,  8  , 8  , 1);
    PHY_8390_SERDES_SET(0xb314 + ofst,  29 , 29 , 0);
    PHY_8390_SERDES_SET(0xb330 + ofst,  15 , 15 , 0);
    PHY_8390_SERDES_SET(0xb318 + ofst,  8  , 8  , 0);
    PHY_8390_SERDES_SET(0xb318 + ofst,  0  , 0  , 1);
    PHY_8390_SERDES_SET(0xb344 + ofst,  21 , 16 , 0x0);

    for (i = 1; i <= cnt; ++i)
    {
        ioal_mem32_read(unit, 0xb038 + ofst, &val);
        tap0_sum += (val >> 16) & 0x1f;

        osal_time_usleep(1 * 1000);
    }

    tap0_avg = tap0_sum/cnt;
    PHY_8390_SERDES_SET(0xb32c + ofst,  21 , 16 , tap0_avg);
    PHY_8390_SERDES_SET(0xb318 + ofst,  8  , 8  , 1);
    PHY_8390_SERDES_SET(0xb318 + ofst,  0  , 0  , 0);

    PHY_8390_SERDES_SET(0xb320 + ofst,  3  , 3  , 0);
    PHY_8390_SERDES_SET(0xb340 + ofst,  15 , 15 , 1);
    osal_time_usleep(10 * 1000);
    PHY_8390_SERDES_SET(0xb340 + ofst,  15 , 15 , 0);

    PHY_8390_SERDES_SET(0xB3F8 + ofst, 29, 28, 0x3);
    PHY_8390_SERDES_SET(0xB3F8 + ofst, 17, 16, 0x3);
    PHY_8390_SERDES_SET(0xB3F8 + ofst, 21, 20, 0x1);
    PHY_8390_SERDES_SET(0xB3F8 + ofst, 25, 24, 0x3);

    PHY_8390_SERDES_SET(0xB284 + ofst,  6,  6, 0x1);
    PHY_8390_SERDES_SET(0xB284 + ofst,  5,  5, 0x0);
    PHY_8390_SERDES_SET(0xB284 + ofst,  4,  4, 0x1);
    PHY_8390_SERDES_SET(0xB284 + ofst,  3,  3, 0x1);
    PHY_8390_SERDES_SET(0xB284 + ofst,  2,  2, 0x0);
    PHY_8390_SERDES_SET(0xB284 + ofst,  1,  1, 0x1);
    PHY_8390_SERDES_SET(0xB284 + ofst,  0,  0, 0x1);

    PHY_8390_SERDES_SET(0xB284 + ofst,  0,  0, 0x0);
    PHY_8390_SERDES_SET(0xB3F8 + ofst, 29, 28, 0x0);
    PHY_8390_SERDES_SET(0xB3F8 + ofst, 17, 16, 0x0);
    PHY_8390_SERDES_SET(0xB3F8 + ofst, 21, 20, 0x0);
    PHY_8390_SERDES_SET(0xB3F8 + ofst, 25, 24, 0x0);
}   /* end of _phy_8390_serdes_10g_fiber_dfe_adapt */


int32
_phy_8390_10gSds_restart(uint32 unit, rtk_port_t port)
{
    rtk_port_10gMedia_t media;
    int32               ret, sds;

    if (HWP_CHIP_REV(unit) < CHIP_REV_ID_C)
        return RT_ERR_OK;

    if (DISABLED == port10gState[port])
        return RT_ERR_OK;

    /* function body */
    if (24 == port)
    {
        sds = 8;
    }
    else if (36 == port)
    {
        sds = 12;
    }
    else
    {
        ret = RT_ERR_PORT_ID;
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "port %u", port);
        return ret;
    }

    if (HWP_CHIP_REV(unit) == CHIP_REV_ID_C)
        _phy_8390_serdes_10g_rx_rst(unit, sds);
    else
    {
        if ((ret = phy_8390_10gMedia_get(unit, port, &media)) != RT_ERR_OK)
            return ret;

        osal_time_usleep(300 * 1000);

        switch (media)
        {
            case PORT_10GMEDIA_DAC_50CM:
            case PORT_10GMEDIA_DAC_100CM:
            case PORT_10GMEDIA_DAC_300CM:
            //    _phy_8390_serdes_10g_dac_dfe_adapt(unit, sds);
              //  break;
            case PORT_10GMEDIA_FIBER_10G:
                _phy_8390_serdes_10g_fiber_dfe_adapt(unit, sds);
                break;
            default:
                return RT_ERR_CHIP_NOT_SUPPORTED;
        }
    }

    return RT_ERR_OK;
}   /* end of phy_8390_10gSds_restart */

#ifdef PHY_8390_10G_85_OHM
void _phy_8390_serdes_10g_trx_85ohm(uint32 unit, int sds)
{
    uint32  ofst;

    if (8 == sds)
    {
        ofst = 0x0;
    }
    else
    {
        ofst = 0x800;
    }

    PHY_8390_SERDES_SET(0xb324 + ofst,  25 , 25 , 0);
    PHY_8390_SERDES_SET(0xb324 + ofst,  24 , 21 , 0xf);
    PHY_8390_SERDES_SET(0xb324 + ofst,  20 , 20 , 0);
    PHY_8390_SERDES_SET(0xb324 + ofst,  19 , 16 , 0xf);

    return;
}
#endif


void _phy_8390_serdes_init_10gr_fiber (uint32 unit, int sds)
{
    uint32  ofst, port;

    if (8 == sds)
    {
        ofst = 0x0;
        port = 24;
    }
    else
    {
        ofst = 0x800;
        port = 36;
    }

    PHY_8390_SERDES_SET(0xb014 + ofst,  14  , 14  , 0x1);
    PHY_8390_SERDES_SET(0xb004 + ofst,  30  , 30  , 0x1);
    PHY_8390_SERDES_SET(0xb004 + ofst,  23  , 23  , 0x1);

    if (HWP_CHIP_REV(unit) == CHIP_REV_ID_C)
    {
        if (8 == sds)
        {
            PHY_8390_SERDES_SET(0x000c,  3 , 0  , 0x0);
            PHY_8390_SERDES_SET(0xb0b0,  3  , 3  , 1);
            PHY_8390_SERDES_SET(0xb3c0,  24 , 23 , 0x3);
            PHY_8390_SERDES_SET(0xB300,  31 , 0  , 0xFC14F800);
            PHY_8390_SERDES_SET(0xB304,  31 , 0  , 0xF80057EF);
            PHY_8390_SERDES_SET(0xB308,  31 , 0  , 0x80209000);
            PHY_8390_SERDES_SET(0xB30C,  31 , 0  , 0xFFFFBFFF);
            PHY_8390_SERDES_SET(0xB310,  31 , 0  , 0x806FFFFF);
            PHY_8390_SERDES_SET(0xB314,  31 , 0  , 0x482003E4);
            PHY_8390_SERDES_SET(0xB318,  31 , 0  , 0x347F7E80);
            PHY_8390_SERDES_SET(0xB31C,  31 , 0  , 0x242D8A00);
            PHY_8390_SERDES_SET(0xB320,  31 , 0  , 0x984A3F16);
            PHY_8390_SERDES_SET(0xB324,  31 , 0  , 0xF108B632);
            PHY_8390_SERDES_SET(0xB328,  31 , 0  , 0x00001F53);
            PHY_8390_SERDES_SET(0xB32C,  31 , 0  , 0xFFCF0820);
            PHY_8390_SERDES_SET(0xB330,  31 , 0  , 0x00007000);
            PHY_8390_SERDES_SET(0xB334,  31 , 0  , 0x3800FFFF);
            PHY_8390_SERDES_SET(0xB338,  31 , 0  , 0x02000100);
            PHY_8390_SERDES_SET(0xB33C,  31 , 0  , 0xBE100003);
            PHY_8390_SERDES_SET(0xB340,  31 , 0  , 0x97E044F1);
            PHY_8390_SERDES_SET(0xB344,  31 , 0  , 0xFFCC1084);
            PHY_8390_SERDES_SET(0xB348,  31 , 0  , 0x614FFC84);
            PHY_8390_SERDES_SET(0xB34C,  31 , 0  , 0x07C704F7);
            PHY_8390_SERDES_SET(0xB350,  31 , 0  , 0x407EC3E0);
            PHY_8390_SERDES_SET(0xB354,  31 , 0  , 0x6004619C);
            PHY_8390_SERDES_SET(0xB358,  31 , 0  , 0xEEFAEAE2);
            PHY_8390_SERDES_SET(0xB35C,  31 , 0  , 0x0E4B4E4B);
            PHY_8390_SERDES_SET(0xB360,  31 , 0  , 0x00000000);
            PHY_8390_SERDES_SET(0xB364,  31 , 0  , 0x00000000);
            PHY_8390_SERDES_SET(0xB368,  31 , 0  , 0x00000000);
            PHY_8390_SERDES_SET(0xB36C,  31 , 0  , 0x00000000);
            PHY_8390_SERDES_SET(0xB370,  31 , 0  , 0x00000000);
            PHY_8390_SERDES_SET(0xB374,  31 , 0  , 0x00000000);
            PHY_8390_SERDES_SET(0xB378,  31 , 0  , 0x00000000);
            PHY_8390_SERDES_SET(0xB37C,  31 , 0  , 0x00000000);

            PHY_8390_SERDES_SET(0x000c,  3 , 0  , 0x1);
            PHY_8390_SERDES_SET(0xb3f8,  17 , 16 , 0x1);
            osal_time_usleep(500 * 1000);
            PHY_8390_SERDES_SET(0xb3f8,  17 , 16 , 0x3);
            PHY_8390_SERDES_SET(0xb3f8,  17 , 16 , 0x0);
            _phy_8390_10gSds_restart(unit, 24);
        }
        else
        {
            PHY_8390_SERDES_SET(0x000c,  19 , 16  , 0x0);
            PHY_8390_SERDES_SET(0xb8b0,  3  , 3  , 1);
            PHY_8390_SERDES_SET(0xbbc0,  24 , 23 , 0x3);
            PHY_8390_SERDES_SET(0xBB00,  31 , 0  , 0xFC14F800);
            PHY_8390_SERDES_SET(0xBB04,  31 , 0  , 0xF80057EF);
            PHY_8390_SERDES_SET(0xBB08,  31 , 0  , 0x80209000);
            PHY_8390_SERDES_SET(0xBB0C,  31 , 0  , 0xFFFFBFFF);
            PHY_8390_SERDES_SET(0xBB10,  31 , 0  , 0x806FFFFF);
            PHY_8390_SERDES_SET(0xBB14,  31 , 0  , 0x482003E4);
            PHY_8390_SERDES_SET(0xBB18,  31 , 0  , 0x347F7E80);
            PHY_8390_SERDES_SET(0xBB1C,  31 , 0  , 0x242D8A00);
            PHY_8390_SERDES_SET(0xBB20,  31 , 0  , 0x984A3F16);
            PHY_8390_SERDES_SET(0xBB24,  31 , 0  , 0xF108B632);
            PHY_8390_SERDES_SET(0xBB28,  31 , 0  , 0x00001F53);
            PHY_8390_SERDES_SET(0xBB2C,  31 , 0  , 0xFFCF0820);
            PHY_8390_SERDES_SET(0xBB30,  31 , 0  , 0x00007000);
            PHY_8390_SERDES_SET(0xBB34,  31 , 0  , 0x3800FFFF);
            PHY_8390_SERDES_SET(0xBB38,  31 , 0  , 0x02000100);
            PHY_8390_SERDES_SET(0xBB3C,  31 , 0  , 0xBE100003);
            PHY_8390_SERDES_SET(0xBB40,  31 , 0  , 0x97E044F1);
            PHY_8390_SERDES_SET(0xBB44,  31 , 0  , 0xFFCC1084);
            PHY_8390_SERDES_SET(0xBB48,  31 , 0  , 0x614FFC84);
            PHY_8390_SERDES_SET(0xBB4C,  31 , 0  , 0x07C704F7);
            PHY_8390_SERDES_SET(0xBB50,  31 , 0  , 0x407EC3E0);
            PHY_8390_SERDES_SET(0xBB54,  31 , 0  , 0x6004619C);
            PHY_8390_SERDES_SET(0xBB58,  31 , 0  , 0xEEFAEAE2);
            PHY_8390_SERDES_SET(0xBB5C,  31 , 0  , 0x0E4B4E4B);
            PHY_8390_SERDES_SET(0xBB60,  31 , 0  , 0x00000000);
            PHY_8390_SERDES_SET(0xBB64,  31 , 0  , 0x00000000);
            PHY_8390_SERDES_SET(0xBB68,  31 , 0  , 0x00000000);
            PHY_8390_SERDES_SET(0xBB6C,  31 , 0  , 0x00000000);
            PHY_8390_SERDES_SET(0xBB70,  31 , 0  , 0x00000000);
            PHY_8390_SERDES_SET(0xBB74,  31 , 0  , 0x00000000);
            PHY_8390_SERDES_SET(0xBB78,  31 , 0  , 0x00000000);
            PHY_8390_SERDES_SET(0xBB7C,  31 , 0  , 0x00000000);

            PHY_8390_SERDES_SET(0x000c,  19 , 16  , 0x1);
            PHY_8390_SERDES_SET(0xbbf8,  17 , 16 , 0x1);
            osal_time_usleep(500 * 1000);
            PHY_8390_SERDES_SET(0xbbf8,  17 , 16 , 0x3);
            PHY_8390_SERDES_SET(0xbbf8,  17 , 16 , 0x0);
            _phy_8390_10gSds_restart(unit, 36);
        }
    }
    else
    {
        if (8 == sds)
            PHY_8390_SERDES_SET(0x000c,  3 , 0  , 0x0);
        else
            PHY_8390_SERDES_SET(0x000c,  19 , 16 , 0x0);

        PHY_8390_SERDES_SET(0xb0b0 + ofst,  3  , 3  , 0);
        PHY_8390_SERDES_SET(0xb3c0 + ofst,  24 , 23 , 0x3);
        PHY_8390_SERDES_SET(0xb39c + ofst,  29 , 29 , 0);
        PHY_8390_SERDES_SET(0xb388 + ofst,  15 , 15 , 0);
        PHY_8390_SERDES_SET(0xb3a4 + ofst,  23 , 16 , 0);
        PHY_8390_SERDES_SET(0xB300 + ofst,  31 , 0  , 0xFE14F800);
        PHY_8390_SERDES_SET(0xB304 + ofst,  31 , 0  , 0xF80053EF);
        PHY_8390_SERDES_SET(0xB308 + ofst,  31 , 0  , 0x80201000);
        PHY_8390_SERDES_SET(0xB30C + ofst,  31 , 0  , 0xFFFFBFFF);
        PHY_8390_SERDES_SET(0xB310 + ofst,  31 , 0  , 0x806FFFFF);
        PHY_8390_SERDES_SET(0xB314 + ofst,  31 , 0  , 0x482003E4);
        PHY_8390_SERDES_SET(0xB318 + ofst,  31 , 0  , 0x241F7F80);
        PHY_8390_SERDES_SET(0xB31C + ofst,  31 , 0  , 0x042D8980);
        PHY_8390_SERDES_SET(0xB320 + ofst,  31 , 0  , 0x984A3F16);
        PHY_8390_SERDES_SET(0xB324 + ofst,  31 , 0  , 0xF108B632);
        PHY_8390_SERDES_SET(0xB328 + ofst,  31 , 0  , 0x00001F53);
        PHY_8390_SERDES_SET(0xB32C + ofst,  31 , 0  , 0xFFDF0820);
        PHY_8390_SERDES_SET(0xB330 + ofst,  31 , 0  , 0x00007000);
        PHY_8390_SERDES_SET(0xB334 + ofst,  31 , 0  , 0x3800FFFF);
        PHY_8390_SERDES_SET(0xB338 + ofst,  31 , 0  , 0x06000100);
        PHY_8390_SERDES_SET(0xB33C + ofst,  31 , 0  , 0x9E100003);
        PHY_8390_SERDES_SET(0xB340 + ofst,  31 , 0  , 0x97E004F1);
        PHY_8390_SERDES_SET(0xB344 + ofst,  31 , 0  , 0xFFCC1084);
        PHY_8390_SERDES_SET(0xB348 + ofst,  31 , 0  , 0x614FFC84);
        PHY_8390_SERDES_SET(0xB34C + ofst,  31 , 0  , 0x07C704F7);
        if (8 == sds)
        {
            PHY_8390_SERDES_SET(0xB350,  31 , 0  , 0x407EB162);
        }
        else
        {
            PHY_8390_SERDES_SET(0xBB50,  31 , 0  , 0x407EACE1);
        }
        PHY_8390_SERDES_SET(0xB354 + ofst,  31 , 0  , 0x6004609C);
        PHY_8390_SERDES_SET(0xB358 + ofst,  31 , 0  , 0xEFFAEAE2);
        PHY_8390_SERDES_SET(0xB35C + ofst,  31 , 0  , 0x0E4B4E4B);
        PHY_8390_SERDES_SET(0xB360 + ofst,  31 , 0  , 0x00000000);
        PHY_8390_SERDES_SET(0xB364 + ofst,  31 , 0  , 0x00000000);
        PHY_8390_SERDES_SET(0xB368 + ofst,  31 , 0  , 0x00000000);
        PHY_8390_SERDES_SET(0xB36C + ofst,  31 , 0  , 0x00000000);
        PHY_8390_SERDES_SET(0xB370 + ofst,  31 , 0  , 0x00000000);
        PHY_8390_SERDES_SET(0xB374 + ofst,  31 , 0  , 0x00000000);
        PHY_8390_SERDES_SET(0xB378 + ofst,  31 , 0  , 0x00000000);
        PHY_8390_SERDES_SET(0xB37C + ofst,  31 , 0  , 0x00000000);

      #ifdef PHY_8390_10G_85_OHM
        _phy_8390_serdes_10g_trx_85ohm(unit, sds);
      #endif

        PHY_8390_SERDES_SET(0xb3f8 + ofst,  17 , 16 , 0x1);
        osal_time_usleep(100 * 1000);
        PHY_8390_SERDES_SET(0xb3f8 + ofst,  17 , 16 , 0x3);
        PHY_8390_SERDES_SET(0xb3f8 + ofst,  17 , 16 , 0x0);

        if (8 == sds)
            PHY_8390_SERDES_SET(0x000c,  3 , 0  , 0x1);
        else
            PHY_8390_SERDES_SET(0x000c,  19 , 16 , 0x1);

        _phy_8390_serdes_10g_foreground_offset_range_cali(unit, sds);
        _phy_8390_serdes_10g_rx_rst(unit, sds);
        phy_8390_serdes_10g_leq_init(unit, port);
    }
}   /* end of _phy_8390_serdes_init_10gr_fiber */



int32
phy_8390_10gMedia_get(uint32 unit, rtk_port_t port, rtk_port_10gMedia_t *media)
{
    RT_PARAM_CHK(!HWP_10GE_PORT(unit, port), RT_ERR_PORT_ID);

    *media = port10gMedia[port];

    return RT_ERR_OK;
}   /* end of phy_8390_10gMedia_get */

int32
phy_8390_10gMedia_set(uint32 unit, rtk_port_t port, rtk_port_10gMedia_t media)
{
    uint32  sds;
    int     ret = RT_ERR_OK;

    /* parameter check */
    RT_PARAM_CHK(!HWP_10GE_PORT(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((PORT_10GMEDIA_END <= media), RT_ERR_INPUT);

    /* function body */
    if (24 == port)
    {
        sds = 8;
    }
    else if (36 == port)
    {
        sds = 12;
    }
    else
    {
        ret = RT_ERR_PORT_ID;
        return ret;
    }

    if (ENABLED == port10gState[port])
    {
        switch (media)
        {
            case PORT_10GMEDIA_NONE:
                if (8 == sds)
                {
                    PHY_8390_SERDES_SET(0x000c,  3 , 0  , 0x0);
                }
                else
                {
                    PHY_8390_SERDES_SET(0x000c,  19 , 16  , 0x0);
                }
                break;
            case PORT_10GMEDIA_FIBER_10G:
            case PORT_10GMEDIA_DAC_50CM:
            case PORT_10GMEDIA_DAC_100CM:
            case PORT_10GMEDIA_DAC_300CM:
                _phy_8390_serdes_init_10gr_fiber(unit, sds);
                break;
            case PORT_10GMEDIA_FIBER_1G:
                _phy_8390_serdes_init_96m_1g_fiber(unit, sds);
                break;
            default:
                ret = RT_ERR_INPUT;
                return ret;
        }
    }

    port10gMedia[port] = media;

    return ret;
}   /* end of phy_8390_10gMedia_set */

/* Function Name:
 *      phy_8390_init
 * Description:
 *      Initialize 8390 MAC internal serdes PHY.
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8390_init(uint32 unit, rtk_port_t port)
{
    uint32  reg, val;
    int32   ret;

    if (HWP_10GE_PORT(unit, port))
    {
        if ((ret = phy_8390_10gMedia_set(unit, port, PORT_10GMEDIA_NONE)) != RT_ERR_OK)
            return ret;

        PHY_8390_SERDES_SET(0xb3f8,  31 , 16  , 0x0);
        PHY_8390_SERDES_SET(0xbbf8,  31 , 16  , 0x0);

        portNwayState[port] = 1;

        return RT_ERR_OK;
    }
    else
    {
        if (48 == port)
        {
            reg = CYPRESS_SDS12_13_XSG0r;
        }
        else if (49 == port)
        {
            reg = CYPRESS_SDS12_13_XSG1r;
        }
        else
            return RT_ERR_PORT_ID;

        /* auto-nego state can link with link partner is force mode */
        val = 1;
        if ((ret = reg_field_write(unit, reg, CYPRESS_SR4_CFG_EN_LINK_FIB1Gf, &val)) != RT_ERR_OK)
            return ret;

        /* disable EEE */
        val = 0;
        if ((ret = reg_field_write(unit, reg, CYPRESS_FRE16_EEE_RSG_FIB1Gf, &val)) != RT_ERR_OK)
            return ret;

        if ((ret = reg_field_write(unit, reg, CYPRESS_FRE16_EEE_STD_FIB1Gf, &val)) != RT_ERR_OK)
            return ret;

        if ((ret = reg_field_write(unit, reg, CYPRESS_FRE16_C1_PWRSAV_EN_FIB1Gf, &val)) != RT_ERR_OK)
            return ret;

        if ((ret = reg_field_write(unit, reg, CYPRESS_FRE16_C2_PWRSAV_EN_FIB1Gf, &val)) != RT_ERR_OK)
            return ret;

        if ((ret = reg_field_write(unit, reg, CYPRESS_FRE16_EEE_QUIET_FIB1Gf, &val)) != RT_ERR_OK)
            return ret;
    }

    return RT_ERR_OK;
} /* end of phy_8390_init */

/* Function Name:
 *      phy_8390_enable_set
 * Description:
 *      Set interface status of the specific port
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      enable        - admin configuration of PHY interface
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8390_enable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    uint32  reg, val, rxReg = 0, rxField = 0;
    int32   ret;

    if (ENABLED == sgmiiEn)
        return RT_ERR_NOT_ALLOWED;

    if (HWP_10GE_PORT(unit, port))
    {
        rtk_port_10gMedia_t media;
        rtk_enable_t        oldSts;

        if ((ret = phy_8390_10gMedia_get(unit, port, &media)) != RT_ERR_OK)
            return ret;

        oldSts = port10gState[port];
        port10gState[port] = enable;

        if (ENABLED == enable)
        {
            if ((ret = phy_8390_10gMedia_set(unit, port, media)) != RT_ERR_OK)
            {
                port10gState[port] = oldSts;
                return ret;
            }
        }
        else
        {
            if (24 == port)
            {
                PHY_8390_SERDES_SET(0x000c,  3 , 0  , 0x0);
            }
            else
            {
                PHY_8390_SERDES_SET(0x000c,  19 , 16  , 0x0);
            }
        }

        return RT_ERR_OK;
    }
    else
    {
        if (48 == port)
        {
            reg = CYPRESS_SDS12_13_XSG0r;
            rxReg = CYPRESS_SDS12_13_ANA_TGr;
            rxField = CYPRESS_S0_REG_RX_EN_SELF_XSGf;
        }
        else if (49 == port)
        {
            reg = CYPRESS_SDS12_13_XSG1r;
            rxReg = CYPRESS_SDS12_13_ANA_TGr;
            rxField = CYPRESS_S1_REG_RX_EN_SELF_XSGf;
        }
        else
            return RT_ERR_PORT_ID;
    }

    if (DISABLED == enable)
        val = 1;
    else
        val = 0;

    reg_field_write(unit, reg, CYPRESS_FR0_CFG_FIB_PDOWNf, &val);

    if (ENABLED == enable)
    {
        val = 1;
        reg_field_write(unit, rxReg, rxField, &val);
        osal_time_mdelay(200);
        val = 0;
        reg_field_write(unit, rxReg, rxField, &val);
    }

    return RT_ERR_OK;
} /* end of phy_8390_enable_set */

#if !defined(__BOOTLOADER__)

/* Function Name:
 *      phy_8390_media_get
 * Description:
 *      Get 8390 serdes PHY media type.
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      pMedia - pointer buffer of phy media type
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - invalid parameter
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      1. media type is PORT_MEDIA_FIBER
 */
int32
phy_8390_media_get(uint32 unit, rtk_port_t port, rtk_port_media_t *pMedia)
{
    RT_PARAM_CHK((NULL == pMedia), RT_ERR_NULL_POINTER);

    *pMedia = PORT_MEDIA_FIBER;

    return RT_ERR_OK;
} /* end of phy_8390_media_get */

/* Function Name:
 *      phy_8390_gigaLiteEnable_get
 * Description:
 *      Get the status of Giga Lite of the specific port in the specific unit
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to status of Giga Lite
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      1. The RTL8390 is not supported the per-port Giga Lite feature.
 */
int32
phy_8390_gigaLiteEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    *pEnable = DISABLED;
    return RT_ERR_OK;
} /* end of phy_8390_gigaLiteEnable_get */

/* Function Name:
 *      phy_8390_speed_get
 * Description:
 *      Get link speed status of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pSpeed - pointer to PHY link speed
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8390_speed_get(uint32 unit, rtk_port_t port, rtk_port_speed_t *pSpeed)
{
    uint32  serdes_mode, offset, reg;
    uint32  regData0, regData1;
    int32   ret;

    if (ENABLED == sgmiiEn)
        return RT_ERR_NOT_ALLOWED;

    if (HWP_10GE_PORT(unit, port))
    {
        if (24 == port)
        {
            offset = 8;
            reg = CYPRESS_SDS8_9_XSG0r;
        }
        else
        {
            offset = 12;
            reg = CYPRESS_SDS12_13_XSG0r;
        }
    }
    else
    {
        if (48 == port)
        {
            offset = 12;
            reg = CYPRESS_SDS12_13_XSG0r;
        }
        else
        {
            offset = 13;
            reg = CYPRESS_SDS12_13_XSG1r;
        }
    }

    if ((ret = reg_array_field_read(unit, CYPRESS_MAC_SERDES_IF_CTRLr,
            REG_ARRAY_INDEX_NONE, offset, CYPRESS_SERDES_SPD_SELf, &serdes_mode))
            != RT_ERR_OK)
        return ret;

    switch (serdes_mode)
    {
        case 0:
        case 1:
            *pSpeed = PORT_SPEED_10G;
            break;
        case 7:
            reg_field_read(unit, reg, CYPRESS_FR0_CFG_FIB_SPD_RD_00f, &regData0);
            reg_field_read(unit, reg, CYPRESS_FR0_CFG_FIB_SPD_RD_01f, &regData1);
            if (0 == regData0 && 1 == regData1)
                *pSpeed = PORT_SPEED_1000M;
            else
                *pSpeed = PORT_SPEED_100M;
            break;
        case 8:
            *pSpeed = PORT_SPEED_100M;
            break;
        default:
            *pSpeed = PORT_SPEED_100M;
    }

    return ret;
} /* end of phy_8390_speed_get */

/* Function Name:
 *      phy_8390_reset
 * Description:
 *      Reset serdes
 * Input:
 *      unit          - unit id
 *      port          - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
static int32
_phy_8390_reset(uint32 unit, rtk_port_t port)
{
    uint32  speed;
    int     ret;

    if (HWP_10GE_PORT(unit, port))
    {
        if ((ret = phy_8390_speed_get(unit, port, &speed)) != RT_ERR_OK)
            return ret;

        if (PORT_SPEED_10G == speed)
        {
            uint32  sds;

            if (24 == port)
            {
                sds = 8;
            }
            else if (36 == port)
            {
                sds = 12;
            }
            else
                return RT_ERR_PORT_ID;

            _phy_8390_serdes_10g_rx_rst(unit, sds);
        }
        else
        {
            if (24 == port)
            {
                _phy_8390_sfp_rst_s8(unit);
            }
            else if (36 == port)
            {
                _phy_8390_sfp_rst_s12(unit);
            }
            else
                return RT_ERR_PORT_ID;
        }
    }
    else
    {
        if (48 == port)
        {
            _phy_8390_sfp_rst_s12(unit);
        }
        else if (49 == port)
        {
            _phy_8390_sfp_rst_s13(unit);
        }
    }

    return RT_ERR_OK;
}   /* end of _phy_8390_reset */

void _phy_8390_serdes_init_10gr_leq (uint32 unit, int sds)
{
    uint32 pole1 = 0x7;
    uint32 ofst = 0x800;
    uint32 pole3 = 0x1;
    uint32 pole2 = 0x5;
    uint32 addr0 = 0xb340;
    uint32 addr1 = 0xb348;
    uint32 boost1 = 0x0;
    uint32 boost3 = 0x0;
    uint32 boost2 = 0x0;

    if (8 == sds)
    {
        PHY_8390_SERDES_SET(addr0,  27 , 25 , boost1);
        PHY_8390_SERDES_SET(addr0,  24 , 22 , boost2);
        PHY_8390_SERDES_SET(addr0,  21 , 19 , boost3);
        PHY_8390_SERDES_SET(addr0,  18 , 16 , pole1);
        PHY_8390_SERDES_SET(addr0,  30 , 28 , pole2);
        PHY_8390_SERDES_SET(addr1,  25 , 23 , pole3);
    }
    else if (12 == sds)
    {
        PHY_8390_SERDES_SET(addr0+ofst,  27 , 25 , boost1);
        PHY_8390_SERDES_SET(addr0+ofst,  24 , 22 , boost2);
        PHY_8390_SERDES_SET(addr0+ofst,  21 , 19 , boost3);
        PHY_8390_SERDES_SET(addr0+ofst,  18 , 16 , pole1);
        PHY_8390_SERDES_SET(addr0+ofst,  30 , 28 , pole2);
        PHY_8390_SERDES_SET(addr1+ofst,  25 , 23 , pole3);
    }

    _phy_8390_serdes_10g_rx_rst(unit, sds);
}   /* end of _phy_8390_serdes_init_10gr_leq */


void _phy_8390_serdes_init_10gr_rx_current(uint32 unit, int sds)
{
    if (8 == sds)
    {
      PHY_8390_SERDES_SET(0xB324, 31, 31, 1);
      PHY_8390_SERDES_SET(0xB328, 12, 12, 1);
    }
    else if (12 == sds)
    {
      PHY_8390_SERDES_SET(0xBB24, 31, 31, 1);
      PHY_8390_SERDES_SET(0xBB28, 12, 12, 1);
    }
    else
        return;

    _phy_8390_serdes_10g_rx_rst(unit, sds);
}   /* end of _phy_8390_serdes_init_10gr_rx_current */

void _phy_8390_serdes_10g_dac_dfe_adapt (uint32 unit, int sds_num)
{
    int32   ofst;
    int32   leq;
    uint32  val, i;
    int32   cnt = 10, half;
    int32   vthp_sum = 0;
    int32   vthn_sum = 0;
    int32   tap0_sum = 0;
    int32   tap0_final_avg;
    int32   tap1_even = 0, tap1_odd = 0;
    int32   tap1_even_sum = 0, tap1_odd_sum = 0;
    int32   tap1_even_final_avg, tap1_odd_final_avg;
    int32   tap2_even = 0, tap2_odd = 0;
    int32   tap2_even_sum = 0, tap2_odd_sum = 0;
    int32   tap2_even_final_avg, tap2_odd_final_avg;
    int32   vthp_final_avg, vthn_final_avg;
    int32   tap0_avg;
    int32   vthp_avg, vthn_avg;
    int32   tap1_even_avg, tap1_odd_avg;
    int32   tap2_even_avg, tap2_odd_avg;

    half = cnt / 2;

    if (8 == sds_num)
    {
        ofst = 0;
    }
    else
    {
        ofst = 0x800;
    }

    PHY_8390_SERDES_SET(0xb338 + ofst,  14 , 14 , 0);
    PHY_8390_SERDES_SET(0xb360 + ofst,  31 , 31 , 0);
    PHY_8390_SERDES_SET(0xb304 + ofst,  9  , 5  , 0x6);
    PHY_8390_SERDES_SET(0xb308 + ofst,  14 , 12 , 0x0);
    PHY_8390_SERDES_SET(0xb32c + ofst,  14 , 12 , 0x0);
    PHY_8390_SERDES_SET(0xb30c + ofst,  9  , 5  , 0x2);
    PHY_8390_SERDES_SET(0xb30c + ofst,  4  , 0  , 0x2);
    PHY_8390_SERDES_SET(0xb33c + ofst,  29 , 28 , 0x1);
    PHY_8390_SERDES_SET(0xb354 + ofst,  8  , 8  , 0x1);
    PHY_8390_SERDES_SET(0xb354 + ofst,  21 , 16 , 0x3);
    PHY_8390_SERDES_SET(0xb340 + ofst,  28 , 27 , 0x3);

    PHY_8390_SERDES_SET(0xb32c + ofst,  21 , 16 , 0x1f);
    PHY_8390_SERDES_SET(0xb32c + ofst,  5  , 0  , 0x0);
    PHY_8390_SERDES_SET(0xb31c + ofst,  21 , 19 , 0x7);
    PHY_8390_SERDES_SET(0xb31c + ofst,  18 , 16 , 0x7);
    PHY_8390_SERDES_SET(0xb304 + ofst,  27 , 22 , 0x0);
    PHY_8390_SERDES_SET(0xb304 + ofst,  21 , 16 , 0x0);
    PHY_8390_SERDES_SET(0xb330 + ofst,  11 , 6  , 0x0);
    PHY_8390_SERDES_SET(0xb330 + ofst,  5  , 0  , 0x0);
    PHY_8390_SERDES_SET(0xb308 + ofst,  27 , 22 , 0x0);
    PHY_8390_SERDES_SET(0xb308 + ofst,  11 , 6  , 0x0);
    PHY_8390_SERDES_SET(0xb308 + ofst,  5  , 0  , 0x0);
    PHY_8390_SERDES_SET(0xb318 + ofst,  15 , 8  , 0xff);
    PHY_8390_SERDES_SET(0xb318 + ofst,  7  , 0  , 0x0);
    PHY_8390_SERDES_SET(0xb350 + ofst,  30 , 24 , 0x0);

    PHY_8390_SERDES_SET(0xb338 + ofst,  8  , 8  , 1);
    PHY_8390_SERDES_SET(0xb314 + ofst,  29 , 29 , 0);
    PHY_8390_SERDES_SET(0xb330 + ofst,  15 , 15 , 0);
    PHY_8390_SERDES_SET(0xb354 + ofst,  27 , 27 , 0);
    PHY_8390_SERDES_SET(0xb360 + ofst,  25 , 21 , 0x0);
    PHY_8390_SERDES_SET(0xb360 + ofst,  20 , 16 , 0x0);

    PHY_8390_SERDES_SET(0xb300 + ofst,  24 , 24 , 1);
    PHY_8390_SERDES_SET(0xb300 + ofst,  23 , 23 , 0);
    PHY_8390_SERDES_SET(0xb318 + ofst,  26 , 26 , 0);
    PHY_8390_SERDES_SET(0xb320 + ofst,  3  , 3  , 0);
    PHY_8390_SERDES_SET(0xb340 + ofst,  15 , 15 , 1);
    osal_time_usleep(100 * 1000);
    PHY_8390_SERDES_SET(0xb340 + ofst,  15 , 15 , 0);
    osal_time_usleep(500 * 1000);
    leq = _phy_8390_serdes_10g_eq_dump(unit, sds_num);
    //printf( "LEQ cal val = %d\n", leq);
    PHY_8390_SERDES_SET(0xb300 + ofst,  24 , 24 , 0);
    PHY_8390_SERDES_SET(0xb300 + ofst,  23 , 23 , 1);
    PHY_8390_SERDES_SET(0xb300 + ofst,  22 , 18 , leq);

    PHY_8390_SERDES_SET(0xb320 + ofst,  4  , 4  , 1);
    PHY_8390_SERDES_SET(0xb320 + ofst,  11 , 10 , 0x3);
    PHY_8390_SERDES_SET(0xb340 + ofst,  4  , 2  , 0x3);
    PHY_8390_SERDES_SET(0xb34c + ofst,  31 , 28 , 0x0);
    PHY_8390_SERDES_SET(0xb038 + ofst,  11 , 11 , 1);
    PHY_8390_SERDES_SET(0xb038 + ofst,  10 , 10 , 1);
    PHY_8390_SERDES_SET(0xb038 + ofst,  9  , 0  , 0x0);


    PHY_8390_SERDES_SET(0xb318 + ofst,  0  , 0  , 1);
    PHY_8390_SERDES_SET(0xb318 + ofst,  7  , 7  , 1);
    PHY_8390_SERDES_SET(0xb350 + ofst,  24 , 24 , 1);
    PHY_8390_SERDES_SET(0xb318 + ofst,  8  , 8  , 0);
    PHY_8390_SERDES_SET(0xb318 + ofst,  15 , 15 , 0);

    for (i = 1; i <= cnt; ++i)
    {
        PHY_8390_SERDES_SET(0xb344 + ofst,  21 , 16 , 0x0);
        ioal_mem32_read(unit, 0xb038 + ofst, &val);
        tap0_sum += (val >> 16) & 0x1f;

        PHY_8390_SERDES_SET(0xb344+ofst,  21 , 16 , 0xc);
        ioal_mem32_read(unit, 0xb038 + ofst, &val);
        vthp_sum += ((val >> 16) & 0x7);
        vthn_sum += ((val >> 19) & 0x7);

        osal_time_usleep(1 * 1000);
    }
    tap0_avg = tap0_sum/cnt;
    if ((tap0_sum - (tap0_avg * cnt)) >= half)
        tap0_final_avg = tap0_avg + 1;
    else
        tap0_final_avg = tap0_avg;
    //printf( "tap0 avg val = %d\n", tap0_final_avg);
    PHY_8390_SERDES_SET(0xb32c + ofst,  21 , 16 , tap0_final_avg);
    PHY_8390_SERDES_SET(0xb318 + ofst,  8  , 8  , 1);
    PHY_8390_SERDES_SET(0xb318 + ofst,  0  , 0  , 0);
    PHY_8390_SERDES_SET(0xb350 + ofst,  24 , 24 , 0);

    vthp_avg = vthp_sum/cnt;
    if ((vthp_sum - (vthp_avg * cnt)) >= half)
        vthp_final_avg = vthp_avg + 1;
    else
        vthp_final_avg = vthp_avg;

    vthn_avg = vthn_sum/cnt;
    if ((vthn_sum - (vthn_avg * cnt)) >= half)
        vthn_final_avg = vthn_avg + 1;
    else
        vthn_final_avg = vthn_avg;

    //printf( "vthp avg val = %d\n", vthp_final_avg);
    //printf( "vthn avg val = %d\n", vthn_final_avg);
    PHY_8390_SERDES_SET(0xb31c + ofst,  21 , 19 , vthp_final_avg);
    PHY_8390_SERDES_SET(0xb31c + ofst,  18 , 16 , vthn_final_avg);
    PHY_8390_SERDES_SET(0xb318 + ofst,  15 , 15 , 1);
    PHY_8390_SERDES_SET(0xb318 + ofst,  7  , 7  , 0);

    //if (leq > 5)
    {
        PHY_8390_SERDES_SET(0xb338 + ofst,  8  , 8  , 0);
        PHY_8390_SERDES_SET(0xb318 + ofst,  4  , 1  , 0xf);
        PHY_8390_SERDES_SET(0xb350 + ofst,  28 , 25 , 0xf);
        PHY_8390_SERDES_SET(0xb318 + ofst,  12 , 9  , 0x0);

        PHY_8390_SERDES_SET(0xb320 + ofst,  3  , 3  , 0);
        PHY_8390_SERDES_SET(0xb340 + ofst,  15 , 15 , 1);
        osal_time_usleep(100 * 1000);
        PHY_8390_SERDES_SET(0xb340 + ofst,  15 , 15 , 0);

        for (i = 1; i <= cnt; ++i)
        {
            PHY_8390_SERDES_SET(0xb344+ofst,  21 , 16 , 0x1);
            ioal_mem32_read(unit, 0xb038 + ofst, &val);
            tap1_even = (val >> 16) & 0x1f;
            if (((val >> 21) & 0x1) == 1)
                tap1_even_sum -= tap1_even;
            else
                tap1_even_sum += tap1_even;

            PHY_8390_SERDES_SET(0xb344+ofst,  21 , 16 , 0x6);
            ioal_mem32_read(unit, 0xb038 + ofst, &val);
            tap1_odd = (val >> 16) & 0x1f;
            if (((val >> 21) & 0x1) == 1)
                tap1_odd_sum -= tap1_odd;
            else
                tap1_odd_sum += tap1_odd;

            PHY_8390_SERDES_SET(0xb344+ofst,  21 , 16 , 0x2);
            ioal_mem32_read(unit, 0xb038 + ofst, &val);
            tap2_even = (val >> 16) & 0x1f;
            if (((val >> 21) & 0x1) == 1)
                tap2_even_sum -= tap2_even;
            else
                tap2_even_sum += tap2_even;

            PHY_8390_SERDES_SET(0xb344+ofst,  21 , 16 , 0x7);
            ioal_mem32_read(unit, 0xb038 + ofst, &val);
            tap2_odd = (val >> 16) & 0x1f;
            if (((val >> 21) & 0x1) == 1)
                tap2_odd_sum -= tap2_odd;
            else
                tap2_odd_sum += tap2_odd;

            osal_time_usleep(1 * 1000);
        }
        tap1_even_avg = tap1_even_sum/cnt;
        if ((tap1_even_sum - (tap1_even_avg * cnt)) >= half)
            tap1_even_final_avg = tap1_even_avg + 1;
        else
            tap1_even_final_avg = tap1_even_avg;

        tap1_odd_avg = tap1_odd_sum/cnt;
        if ((tap1_odd_sum - (tap1_odd_avg * cnt)) >= half)
            tap1_odd_final_avg = tap1_odd_avg + 1;
        else
            tap1_odd_final_avg = tap1_odd_avg;

        //printf( "tap1_even avg val = %d\n", tap1_even_final_avg);
        //printf( "tap1_odd  avg val = %d\n", tap1_odd_final_avg);
        if (tap1_even_final_avg < 0)
        {
            PHY_8390_SERDES_SET(0xb304+ofst,  27 , 27 , 1);
            val = tap1_even_final_avg * -1;
        }
        else
        {
            PHY_8390_SERDES_SET(0xb304+ofst,  27 , 27 , 0);
            val = tap1_even_final_avg;
        }
        PHY_8390_SERDES_SET(0xb304+ofst,  26 , 22 , val);

        if (tap1_odd_final_avg < 0)
        {
            PHY_8390_SERDES_SET(0xb32c+ofst,  5  , 5  , 1);
            val = tap1_odd_final_avg * -1;
        }
        else
        {
            PHY_8390_SERDES_SET(0xb32c+ofst,  5  , 5  , 0);
            val = tap1_odd_final_avg;
        }
        PHY_8390_SERDES_SET(0xb32c+ofst,  4  , 0  , val);

        if (tap1_even_final_avg < 0)
        {
            PHY_8390_SERDES_SET(0xb308+ofst,  11 , 11 , 1);
            val = tap1_even_final_avg * -1;
        }
        else
        {
            PHY_8390_SERDES_SET(0xb308+ofst,  11 , 11 , 0);
            val = tap1_even_final_avg;
        }
        PHY_8390_SERDES_SET(0xb308+ofst,  10 , 6  , val);

        if (tap1_odd_final_avg < 0)
        {
            PHY_8390_SERDES_SET(0xb330+ofst,  5  , 5  , 1);
            val = tap1_odd_final_avg * -1;
        }
        else
        {
            PHY_8390_SERDES_SET(0xb330+ofst,  5  , 5  , 0);
            val = tap1_odd_final_avg;
        }
        PHY_8390_SERDES_SET(0xb330+ofst,  4  , 0  , val);

        tap2_even_avg = tap2_even_sum/cnt;
        if ((tap2_even_sum - (tap2_even_avg * cnt)) >= half)
            tap2_even_final_avg = tap2_even_avg + 1;
        else
            tap2_even_final_avg = tap2_even_avg;

        tap2_odd_avg = tap2_odd_sum/cnt;
        if ((tap2_odd_sum - (tap2_odd_avg * cnt)) >= half)
            tap2_odd_final_avg = tap2_odd_avg + 1;
        else
            tap2_odd_final_avg = tap2_odd_avg;

        //printf( "tap2_even avg val = %d\n", tap2_even_final_avg);
        //printf( "tap2_odd  avg val = %d\n", tap2_odd_final_avg);
        if (tap2_even_final_avg < 0)
        {
            PHY_8390_SERDES_SET(0xb304+ofst,  21 , 21 , 0);
            val = tap2_even_final_avg * -1;
        }
        else
        {
            PHY_8390_SERDES_SET(0xb304+ofst,  21 , 21 , 0);
            val = tap2_even_final_avg;
        }
        PHY_8390_SERDES_SET(0xb304+ofst,  20 , 16 , val);

        if (tap2_odd_final_avg < 0)
        {
            PHY_8390_SERDES_SET(0xb330+ofst,  11 , 11 , 0);
            val = tap2_odd_final_avg * -1;
        }
        else
        {
            PHY_8390_SERDES_SET(0xb330+ofst,  11 , 11 , 0);
            val = tap2_odd_final_avg;
        }
        PHY_8390_SERDES_SET(0xb330+ofst,  10 , 6  , val);

        if (tap2_even_final_avg < 0)
        {
            PHY_8390_SERDES_SET(0xb308+ofst,  5  , 5  , 0);
            val = tap2_even_final_avg * -1;
        }
        else
        {
            PHY_8390_SERDES_SET(0xb308+ofst,  5  , 5  , 0);
            val = tap2_even_final_avg;
        }
        PHY_8390_SERDES_SET(0xb308+ofst,  4  , 0  , val);

        if (tap2_odd_final_avg < 0)
        {
            PHY_8390_SERDES_SET(0xb308+ofst,  27 , 27 , 0);
            val = tap2_odd_final_avg * -1;
        }
        else
        {
            PHY_8390_SERDES_SET(0xb308+ofst,  27 , 27 , 0);
            val = tap2_odd_final_avg;
        }
        PHY_8390_SERDES_SET(0xb308+ofst,  26 , 22 , val);

        PHY_8390_SERDES_SET(0xb318 + ofst,  12 , 9  , 0xf);
        PHY_8390_SERDES_SET(0xb318 + ofst,  4  , 1  , 0x0);
        PHY_8390_SERDES_SET(0xb350 + ofst,  28 , 25 , 0x0);
        PHY_8390_SERDES_SET(0xb360 + ofst,  19 , 16 , 0xf);
        PHY_8390_SERDES_SET(0xb360 + ofst,  24 , 21 , 0xf);
        PHY_8390_SERDES_SET(0xb354+ofst,  27 , 27 , 1);
    }

    PHY_8390_SERDES_SET(0xB3F8 + ofst, 29, 28, 0x3);
    PHY_8390_SERDES_SET(0xB3F8 + ofst, 17, 16, 0x3);
    PHY_8390_SERDES_SET(0xB3F8 + ofst, 21, 20, 0x1);
    PHY_8390_SERDES_SET(0xB3F8 + ofst, 25, 24, 0x3);

    PHY_8390_SERDES_SET(0xB284 + ofst,  6,  6, 0x1);
    PHY_8390_SERDES_SET(0xB284 + ofst,  5,  5, 0x0);
    PHY_8390_SERDES_SET(0xB284 + ofst,  4,  4, 0x1);
    PHY_8390_SERDES_SET(0xB284 + ofst,  3,  3, 0x1);
    PHY_8390_SERDES_SET(0xB284 + ofst,  2,  2, 0x0);
    PHY_8390_SERDES_SET(0xB284 + ofst,  1,  1, 0x1);
    PHY_8390_SERDES_SET(0xB284 + ofst,  0,  0, 0x1);

    PHY_8390_SERDES_SET(0xB284 + ofst,  0,  0, 0x0);
    PHY_8390_SERDES_SET(0xB3F8 + ofst, 29, 28, 0x0);
    PHY_8390_SERDES_SET(0xB3F8 + ofst, 17, 16, 0x0);
    PHY_8390_SERDES_SET(0xB3F8 + ofst, 21, 20, 0x0);
    PHY_8390_SERDES_SET(0xB3F8 + ofst, 25, 24, 0x0);
}   /* end of _phy_8390_serdes_10g_dac_dfe_adapt */

/* Function Name:
 *      phy_8390_speed_set
 * Description:
 *      Set speed mode status of the specific port
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      speed         - link speed status 10/100/1000
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - invalid parameter
 *      RT_ERR_CHIP_NOT_SUPPORTED - copper media chip is not supported Force-1000
 * Note:
 *      None
 */
int32
phy_8390_speed_set(uint32 unit, rtk_port_t port, rtk_port_speed_t speed)
{
    rtk_enable_t        enable;
    uint32              sdsMode;
    int32               ret;

    if (ENABLED == sgmiiEn)
        return RT_ERR_NOT_ALLOWED;

    if (HWP_10GE_PORT(unit, port))
    {
        switch (speed)
        {
            case PORT_SPEED_10G:
                ret = phy_8390_10gMedia_set(unit, port, PORT_10GMEDIA_FIBER_10G);
                break;
            case PORT_SPEED_1000M:
                ret = phy_8390_10gMedia_set(unit, port, PORT_10GMEDIA_FIBER_1G);
                break;
            default:
                return RT_ERR_CHIP_NOT_SUPPORTED;
        }

        return ret;
    }

    if ((ret = phy_8390_autoNegoEnable_get(unit, port, &enable)) != RT_ERR_OK)
        return ret;

    switch (speed)
    {
        case PORT_SPEED_1000M:
            sdsMode = 7;
            if (48 == port)
            {
                _phy_8390_init_fiber_1g_s12(unit);
                if ((ret = reg_array_field_write(unit, CYPRESS_MAC_SERDES_IF_CTRLr,
                        REG_ARRAY_INDEX_NONE, 12, CYPRESS_SERDES_SPD_SELf,
                        &sdsMode)) != RT_ERR_OK)
                    return ret;
            }
            else if (49 == port)
            {
                _phy_8390_init_fiber_1g_s13(unit);
                if ((ret = reg_array_field_write(unit, CYPRESS_MAC_SERDES_IF_CTRLr,
                        REG_ARRAY_INDEX_NONE, 13, CYPRESS_SERDES_SPD_SELf,
                        &sdsMode)) != RT_ERR_OK)
                    return ret;
            }
            break;
        case PORT_SPEED_100M:
            sdsMode = 8;
            if (48 == port)
            {
                _phy_8390_init_fiber_100_s12(unit);
                if ((ret = reg_array_field_write(unit, CYPRESS_MAC_SERDES_IF_CTRLr,
                        REG_ARRAY_INDEX_NONE, 12, CYPRESS_SERDES_SPD_SELf,
                        &sdsMode)) != RT_ERR_OK)
                    return ret;
            }
            else if (49 == port)
            {
                _phy_8390_init_fiber_100_s13(unit);
                if ((ret = reg_array_field_write(unit, CYPRESS_MAC_SERDES_IF_CTRLr,
                        REG_ARRAY_INDEX_NONE, 13, CYPRESS_SERDES_SPD_SELf,
                        &sdsMode)) != RT_ERR_OK)
                    return ret;
            }
            break;
        default:
            RT_ERR(RT_ERR_FAILED, (MOD_HAL), "port %u speed %u", port, speed);
            return RT_ERR_PHY_SPEED;
    }

    if ((ret = phy_8390_autoNegoEnable_set(unit, port, enable)) != RT_ERR_OK)
        return ret;

    _phy_8390_reset(unit, port);

    return RT_ERR_OK;
} /* end of phy_8390_speed_set */

/* Function Name:
 *      phy_8390_duplex_get
 * Description:
 *      Get duplex mode status of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pDuplex - pointer to PHY duplex mode status
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8390_duplex_get(uint32 unit, rtk_port_t port, rtk_port_duplex_t *pDuplex)
{
    uint32  reg, val, speed;
    int     ret;

    if (ENABLED == sgmiiEn)
        return RT_ERR_NOT_ALLOWED;

    if ((ret = phy_8390_speed_get(unit, port, &speed)) != RT_ERR_OK)
        return ret;

    if (PORT_SPEED_10G == speed)
    {
        *pDuplex = PORT_FULL_DUPLEX;
        return RT_ERR_OK;
    }

    if (HWP_10GE_PORT(unit, port))
    {
        if (24 == port)
            reg = CYPRESS_SDS8_9_XSG0r;
        else if (36 == port)
            reg = CYPRESS_SDS12_13_XSG0r;
        else
            return RT_ERR_PORT_ID;
    }
    else
    {
        /* get value from CHIP*/
        if (48 == port)
            reg = CYPRESS_SDS12_13_XSG0r;
        else if (49 == port)
            reg = CYPRESS_SDS12_13_XSG1r;
        else
            return RT_ERR_PORT_ID;
    }

    reg_field_read(unit, reg, CYPRESS_FR0_CFG_FIB_FULLDUPf, &val);

    if (1 == val)
        *pDuplex = PORT_FULL_DUPLEX;
    else
        *pDuplex = PORT_HALF_DUPLEX;

    return RT_ERR_OK;
} /* end of phy_8390_duplex_get */

/* Function Name:
 *      phy_8390_duplex_set
 * Description:
 *      Set duplex mode status of the specific port
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      duplex        - duplex mode of the port, full or half
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8390_duplex_set(uint32 unit, rtk_port_t port, rtk_port_duplex_t duplex)
{
    uint32  reg, val, speed;
    int32   ret;

    if (ENABLED == sgmiiEn)
        return RT_ERR_NOT_ALLOWED;

    if ((ret = phy_8390_speed_get(unit, port, &speed)) != RT_ERR_OK)
        return ret;

    if (PORT_SPEED_10G == speed)
    {
        if (PORT_FULL_DUPLEX != duplex)
            return RT_ERR_FAILED;

        return RT_ERR_OK;
    }

    if (HWP_10GE_PORT(unit, port))
    {
        if (24 == port)
            reg = CYPRESS_SDS8_9_XSG0r;
        else if (36 == port)
            reg = CYPRESS_SDS12_13_XSG0r;
        else
            return RT_ERR_PORT_ID;
    }
    else
    {
        /* get value from CHIP*/
        if (48 == port)
            reg = CYPRESS_SDS12_13_XSG0r;
        else if (49 == port)
            reg = CYPRESS_SDS12_13_XSG1r;
        else
            return RT_ERR_PORT_ID;
    }

    if (PORT_SPEED_1000M == speed && PORT_FULL_DUPLEX != duplex)
        return RT_ERR_PHY_DUPLEX;

    if (PORT_FULL_DUPLEX == duplex)
        val = 1;
    else
        val = 0;

    reg_field_write(unit, reg, CYPRESS_FR0_CFG_FIB_FULLDUPf, &val);

    _phy_8390_reset(unit, port);

    return RT_ERR_OK;
} /* end of phy_8390_duplex_set */

void _phy_8390_serdes_10g_leq_dc_gain_cali (uint32 unit, int sds_num)
{
    uint32  val;
    int     ofst, Bin;
    int     binMap[] = {0, 1, 3, 2, 7, 6, 4, 5, 15, 14, \
                        12, 13, 8, 9, 11, 10, 31, 30, 28, 29,   \
                        24, 25, 27, 26, 16, 17, 19, 18, 23, 22, \
                        20, 21};

    switch (sds_num)
    {
        case 8:
            ofst = 0x0;
            break;
        case 12:
            ofst = 0x800;
            break;
        default:
            return;
    }

    PHY_8390_SERDES_SET(0xb320 + ofst,  3 , 3 , 0x0);
    PHY_8390_SERDES_SET(0xb340 + ofst,  28  , 27  , 0x0);
    PHY_8390_SERDES_SET(0xb318 + ofst,  26  , 26  , 0x0);

    PHY_8390_SERDES_SET(0xb340 + ofst,  15  , 15  , 0x1);
    osal_time_usleep(100 * 1000);
    PHY_8390_SERDES_SET(0xb340 + ofst,  15  , 15  , 0x0);
    osal_time_usleep(500 * 1000);
    PHY_8390_SERDES_SET(0xb318 + ofst,  26  , 26  , 0x1);

    PHY_8390_SERDES_SET(0xb320 + ofst,  4  , 4  , 1);
    PHY_8390_SERDES_SET(0xb320 + ofst,  11  , 10  , 0x3);
    PHY_8390_SERDES_SET(0xb34c + ofst,  31  , 28  , 0x0);
    PHY_8390_SERDES_SET(0xb340 + ofst,  4  , 2  , 0x4);

    PHY_8390_SERDES_SET(0xb038 + ofst,  9  , 0  , 0x2);
    PHY_8390_SERDES_SET(0xb038 + ofst,  10  , 10  , 0x1);
    PHY_8390_SERDES_SET(0xb038 + ofst,  11  , 11  , 0x1);

    /* [22:18] */
    ioal_mem32_read(unit, 0xb038 + ofst, &val);
    val = ((val >> 18) & 0x1F);
    Bin = binMap[val];

    if (Bin <= 5)
    {
        PHY_8390_SERDES_SET(0xb340 + ofst,  28  , 27  , 0x3);
    }
    else if ((Bin > 5) && (Bin <= 11))
    {
        PHY_8390_SERDES_SET(0xb340 + ofst,  28  , 27  , 0x2);
    }
    else if ((Bin > 11) && (Bin <= 17))
    {
        PHY_8390_SERDES_SET(0xb340 + ofst,  28  , 27  , 0x1);
    }
    else
    {
        PHY_8390_SERDES_SET(0xb340 + ofst,  28  , 27  , 0x0);
    }
}

/* Function Name:
 *      phy_8390_autoNegoEnable_get
 * Description:
 *      Get autonegotiation enable status of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pEnable -   auto negotiation status
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8390_autoNegoEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    uint32  reg, val, speed;
    int32   ret;

    if (ENABLED == sgmiiEn)
        return RT_ERR_NOT_ALLOWED;

    if ((ret = phy_8390_speed_get(unit, port, &speed)) != RT_ERR_OK)
        return ret;

    if (PORT_SPEED_10G == speed)
    {
        *pEnable = DISABLED;
        return RT_ERR_OK;
    }

    if (HWP_10GE_PORT(unit, port))
    {
        if (24 == port)
            reg = CYPRESS_SDS8_9_XSG0r;
        else if (36 == port)
            reg = CYPRESS_SDS12_13_XSG0r;
        else
            return RT_ERR_PORT_ID;
    }
    else
    {
        if (48 == port)
        {
            reg = CYPRESS_SDS12_13_XSG0r;
        }
        else if (49 == port)
        {
            reg = CYPRESS_SDS12_13_XSG1r;
        }
        else
            return RT_ERR_PORT_ID;
    }

    if ((ret = reg_field_read(unit, reg, CYPRESS_FR0_CFG_FIB_ANENf, &val)) != RT_ERR_OK)
        return ret;

    if (val)
    {
        *pEnable = ENABLED;
    }
    else
    {
        *pEnable = DISABLED;
    }

    return ret;
} /* end of phy_8390_autoNegoEnable_get */

/* Function Name:
 *      phy_8390_autoNegoEnable_set
 * Description:
 *      Set autonegotiation enable status of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8390_autoNegoEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    uint32  reg, val, speed;
    int32   ret;

    if (ENABLED == sgmiiEn)
        return RT_ERR_NOT_ALLOWED;

    if (ENABLED == enable)
    {
        val = 1;
    }
    else
    {
        val = 0;
    }

    if ((ret = phy_8390_speed_get(unit, port, &speed)) != RT_ERR_OK)
        return ret;

    if (PORT_SPEED_10G == speed)
    {
        if (ENABLED == enable)
            return RT_ERR_FAILED;
        return RT_ERR_OK;
    }

    if (HWP_10GE_PORT(unit, port))
    {
        if (24 == port)
            reg = CYPRESS_SDS8_9_XSG0r;
        else if (36 == port)
            reg = CYPRESS_SDS12_13_XSG0r;
        else
            return RT_ERR_PORT_ID;
    }
    else
    {
        if (48 == port)
        {
            reg = CYPRESS_SDS12_13_XSG0r;
        }
        else if (49 == port)
        {
            reg = CYPRESS_SDS12_13_XSG1r;
        }
        else
            return RT_ERR_PORT_ID;
    }

    if ((ret = reg_field_write(unit, reg, CYPRESS_FR0_CFG_FIB_ANENf, &val)) != RT_ERR_OK)
        return ret;

    portNwayState[port] = val;

    val = 1;
    if ((ret = reg_field_write(unit, reg, CYPRESS_FR0_CFG_FIB_RESTARTf, &val)) != RT_ERR_OK)
        return ret;

    return ret;
} /* end of phy_8390_autoNegoEnable_set */

/* Function Name:
 *      phy_8390_autoNegoAbility_get
 * Description:
 *      Get ability advertisement for auto negotiation of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pAbility - pointer to PHY auto negotiation ability
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8390_autoNegoAbility_get(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility)
{
    uint32  reg, phyData4, speed;
    int32   ret;

    if (ENABLED == sgmiiEn)
        return RT_ERR_NOT_ALLOWED;

    if ((ret = phy_8390_speed_get(unit, port, &speed)) != RT_ERR_OK)
        return ret;

    if (PORT_SPEED_10G == speed)
    {
        return RT_ERR_OK;
    }

    if (HWP_10GE_PORT(unit, port))
    {
        if (24 == port)
            reg = CYPRESS_SDS8_9_XSG0r;
        else if (36 == port)
            reg = CYPRESS_SDS12_13_XSG0r;
        else
            return RT_ERR_PORT_ID;
    }
    else
    {
        if (48 == port)
        {
            reg = CYPRESS_SDS12_13_XSG0r;
        }
        else if (49 == port)
        {
            reg = CYPRESS_SDS12_13_XSG1r;
        }
        else
            return RT_ERR_PORT_ID;
    }

    /* get value from CHIP*/
    if ((ret = reg_field_read(unit, reg, CYPRESS_FR4_TX_CFG_REGf, &phyData4)) != RT_ERR_OK)
        return ret;

    pAbility->FC = (phyData4 & _1000BaseX_Pause_R4_MASK) >> _1000BaseX_Pause_R4_OFFSET;
    pAbility->AsyFC = (phyData4 & _1000BaseX_AsymmetricPause_R4_MASK) >> _1000BaseX_AsymmetricPause_R4_OFFSET;
    pAbility->Half_10 = 0;
    pAbility->Full_10 = 0;
    pAbility->Half_100 = 0;
    pAbility->Full_100 = 0;
    pAbility->Half_1000 = (phyData4 & _1000BaseX_HalfDuplex_R4_MASK) >> _1000BaseX_HalfDuplex_R4_OFFSET;
    pAbility->Full_1000 = (phyData4 & _1000BaseX_FullDuplex_R4_MASK) >> _1000BaseX_FullDuplex_R4_OFFSET;

    return ret;
} /* end of phy_8390_autoNegoAbility_get */

/* Function Name:
 *      phy_8390_autoNegoAbility_set
 * Description:
 *      Set ability advertisement for auto negotiation of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 *      pAbility  - auto negotiation ability that is going to set to PHY
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8390_autoNegoAbility_set(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility)
{
    rtk_enable_t    enable;
    uint32          reg, phyData4, speed;
    int32           ret;

    if (ENABLED == sgmiiEn)
        return RT_ERR_NOT_ALLOWED;

    if ((ret = phy_8390_speed_get(unit, port, &speed)) != RT_ERR_OK)
        return ret;

    if (PORT_SPEED_10G == speed)
    {
        return RT_ERR_OK;
    }

    if (HWP_10GE_PORT(unit, port))
    {
        if (24 == port)
            reg = CYPRESS_SDS8_9_XSG0r;
        else if (36 == port)
            reg = CYPRESS_SDS12_13_XSG0r;
        else
            return RT_ERR_PORT_ID;
    }
    else
    {
        if (48 == port)
        {
            reg = CYPRESS_SDS12_13_XSG0r;
        }
        else if (49 == port)
        {
            reg = CYPRESS_SDS12_13_XSG1r;
        }
        else
            return RT_ERR_PORT_ID;
    }

    /* get value from CHIP*/
    if ((ret = phy_8390_autoNegoEnable_get(unit, port, &enable)) != RT_ERR_OK)
        return ret;

    if ((ret = reg_field_read(unit, reg, CYPRESS_FR4_TX_CFG_REGf, &phyData4)) != RT_ERR_OK)
        return ret;

    phyData4 = phyData4 & ~(_1000BaseX_Pause_R4_MASK | _1000BaseX_AsymmetricPause_R4_MASK);
            phyData4 = phyData4
                    | (pAbility->FC << _1000BaseX_Pause_R4_OFFSET)
                    | (pAbility->AsyFC << _1000BaseX_AsymmetricPause_R4_OFFSET);
            phyData4 = phyData4 & ~(_1000BaseX_HalfDuplex_R4_MASK | _1000BaseX_FullDuplex_R4_MASK);
            phyData4 = phyData4 | (pAbility->Half_1000 << _1000BaseX_HalfDuplex_R4_OFFSET)
                    | (pAbility->Full_1000 << _1000BaseX_FullDuplex_R4_OFFSET);

    if ((ret = reg_field_write(unit, reg, CYPRESS_FR4_TX_CFG_REGf, &phyData4)) != RT_ERR_OK)
        return ret;

    /* Force re-autonegotiation if AN is on*/
    if (ENABLED == enable)
    {
        if ((ret = reg_field_write(unit, reg, CYPRESS_FR0_CFG_FIB_RESTARTf, &enable)) != RT_ERR_OK)
            return ret;
    }

    return ret;
} /* end of phy_8390_autoNegoAbility_set */

/* Function Name:
 *      _phy_8390_serdesFiber1g_handeler
 * Description:
 *      Handle serdes fiber 1G and reset action
 * Input:
 *      unit - unit id
 *      port - port which is to be handle
 * Output:
 *      pStatus -Fiber RX status
 * Return:
 *      RT_ERR_FAILED - initialize fail
 *      RT_ERR_OK     - initialize success
 * Note:
 *
 */
void
_phy_8390_serdesFiber1g_handeler(uint32 unit, uint32 port, uint32 *pStatus)
{
    uint32  reg, regData;
    uint32  loop, errCnt = 0;

    *pStatus = PHY_FIBER_RX_STATUS_0;

    if (HWP_10GE_PORT(unit, port))
    {
        if (24 == port)
        {
            reg = CYPRESS_SDS8_9_XSG0r;
        }
        else if (36 == port)
        {
            reg = CYPRESS_SDS12_13_XSG0r;
        }
        else
        {
            return;
        }
    }   /* if (HWP_10GE_PORT(unit, port)) */
    else
    {
        if (48 == port)
        {
            reg = CYPRESS_SDS12_13_XSG0r;
        }
        else if (49 == port)
        {
            reg = CYPRESS_SDS12_13_XSG1r;
        }
        else
        {
            return;
        }
    }   /* else of if (HWP_10GE_PORT(unit, port)) */

    reg_field_read(unit, reg, CYPRESS_SRE24_CFG_TMR_ALIf, &regData);
    regData &= ~(0x1);
    reg_field_write(unit, reg, CYPRESS_SRE24_CFG_TMR_ALIf, &regData);

    regData = 0;
    reg_field_write(unit, reg, CYPRESS_SRE24_CFG_SYMBOLERR_CNTf, &regData);

    /* read & clear */
    reg_field_read(unit, reg, CYPRESS_SRE25_MUX_SYMBOLERR_CNTf, &regData);

    for(loop = 0; loop < 3; ++loop)
    {
        reg_field_read(unit, reg, CYPRESS_SRE25_MUX_SYMBOLERR_CNTf, &regData);
        if (0xFF == regData)
        {
            ++errCnt;
        }
        osal_time_usleep(200);
    }

    if (errCnt >= 2)
    {
        *pStatus = PHY_FIBER_RX_STATUS_1;

        if (HWP_10GE_PORT(unit, port))
        {
            if (24 == port)
            {
                regData = 0;
                reg_field_write(unit, CYPRESS_SDS8_9_ANA_TGr,
                        CYPRESS_S0_REG_RX_FORCERUNf, &regData);

                regData = 1;
                reg_field_write(unit, CYPRESS_SDS8_9_ANA_TGr,
                        CYPRESS_S0_REG_RX_EN_SELF_XSGf, &regData);

                osal_time_usleep(200);
                regData = 0;
                reg_field_write(unit, CYPRESS_SDS8_9_ANA_TGr,
                        CYPRESS_S0_REG_RX_EN_SELF_XSGf, &regData);
            }
            else if (36 == port)
            {
                regData = 0;
                reg_field_write(unit, CYPRESS_SDS12_13_ANA_TGr,
                        CYPRESS_S0_REG_RX_FORCERUNf, &regData);

                regData = 1;
                reg_field_write(unit, CYPRESS_SDS12_13_ANA_TGr,
                        CYPRESS_S0_REG_RX_EN_SELF_XSGf, &regData);

                osal_time_usleep(200);
                regData = 0;
                reg_field_write(unit, CYPRESS_SDS12_13_ANA_TGr,
                        CYPRESS_S0_REG_RX_EN_SELF_XSGf, &regData);
            }
        }   /* if (HWP_10GE_PORT(unit, port)) */
        else
        {
            if (48 == port)
            {
                regData = 0;
                reg_field_write(unit, CYPRESS_SDS12_13_ANA_TGr,
                        CYPRESS_S0_REG_RX_FORCERUNf, &regData);

                regData = 1;
                reg_field_write(unit, CYPRESS_SDS12_13_ANA_TGr,
                        CYPRESS_S0_REG_RX_EN_SELF_XSGf, &regData);

                osal_time_usleep(200);
                regData = 0;
                reg_field_write(unit, CYPRESS_SDS12_13_ANA_TGr,
                        CYPRESS_S0_REG_RX_EN_SELF_XSGf, &regData);
            }
            else if (49 == port)
            {
                regData = 0;
                reg_field_write(unit, CYPRESS_SDS12_13_ANA_TGr,
                        CYPRESS_S1_REG_RX_FORCERUNf, &regData);

                regData = 1;
                reg_field_write(unit, CYPRESS_SDS12_13_ANA_TGr,
                        CYPRESS_S1_REG_RX_EN_SELF_XSGf, &regData);

                osal_time_usleep(200);
                regData = 0;
                reg_field_write(unit, CYPRESS_SDS12_13_ANA_TGr,
                        CYPRESS_S1_REG_RX_EN_SELF_XSGf, &regData);
            }
        }   /* else of if (HWP_10GE_PORT(unit, port)) */

        /* read & clear */
        reg_field_read(unit, reg, CYPRESS_SRE25_MUX_SYMBOLERR_CNTf, &regData);

        errCnt = 0;
        for(loop = 0; loop < 3; ++loop)
        {
            reg_field_read(unit, reg, CYPRESS_SRE25_MUX_SYMBOLERR_CNTf, &regData);
            if (0xFF == regData)
            {
                ++errCnt;
            }
            osal_time_usleep(200);
        }

        if (errCnt >= 2)
        {
            regData = 1;
            reg_field_write(unit, reg, CYPRESS_FR0_CFG_FIB_PDOWNf, &regData);

            if (HWP_10GE_PORT(unit, port))
            {
                if (24 == port)
                {
                    regData = 1;
                    reg_field_write(unit, CYPRESS_SDS8_9_XSG0r,
                            CYPRESS_SR3_SOFT_RSTf, &regData);

                    osal_time_usleep(200);
                    regData = 0;
                    reg_field_write(unit, CYPRESS_SDS8_9_XSG0r,
                            CYPRESS_SR3_SOFT_RSTf, &regData);

                }
                else if (36 == port)
                {
                    regData = 1;
                    reg_field_write(unit, CYPRESS_SDS12_13_XSG0r,
                            CYPRESS_SR3_SOFT_RSTf, &regData);

                    osal_time_usleep(200);
                    regData = 0;
                    reg_field_write(unit, CYPRESS_SDS12_13_XSG0r,
                            CYPRESS_SR3_SOFT_RSTf, &regData);
                }
            }   /* if (HWP_10GE_PORT(unit, port)) */
            else
            {
                if (48 == port)
                {
                    regData = 1;
                    reg_field_write(unit, reg,
                            CYPRESS_SR14_SEL_CALIBOKf, &regData);

                    reg_field_write(unit, CYPRESS_SDS12_13_ANA_TGr,
                            CYPRESS_S1_DUMMY_RG3D3C_13_12f, &regData);

                    osal_time_usleep(200);
                    regData = 0;
                    reg_field_write(unit, CYPRESS_SDS12_13_ANA_TGr,
                            CYPRESS_S1_DUMMY_RG3D3C_13_12f, &regData);

                    reg_field_write(unit, reg,
                            CYPRESS_SR14_SEL_CALIBOKf, &regData);
                }
                else if (49 == port)
                {
                    regData = 1;
                    reg_field_write(unit, reg,
                            CYPRESS_SR14_SEL_CALIBOKf, &regData);

                    reg_field_write(unit, CYPRESS_SDS12_13_ANA_TGr,
                            CYPRESS_S1_DUMMY_RG3D3C_15_14f, &regData);

                    osal_time_usleep(200);
                    regData = 0;
                    reg_field_write(unit, CYPRESS_SDS12_13_ANA_TGr,
                            CYPRESS_S1_DUMMY_RG3D3C_15_14f, &regData);

                    reg_field_write(unit, reg,
                            CYPRESS_SR14_SEL_CALIBOKf, &regData);
                }
            }   /* else of if (HWP_10GE_PORT(unit, port)) */

            regData = 0;
            reg_field_write(unit, reg, CYPRESS_FR0_CFG_FIB_PDOWNf, &regData);
        }
    }   /* if (errCnt >= 2) */

    return;
}   /* end of _phy_8390_serdesFiber1g_handeler */

/* Function Name:
 *      phy_8390_serdesFiberRx_check
 * Description:
 *      Handle serdes fiber 1G
 * Input:
 *      unit - unit id
 *      port - port which is to be handle
 * Output:
 *      pStatus -Fiber RX status
 * Return:
 *      RT_ERR_FAILED - initialize fail
 *      RT_ERR_OK     - initialize success
 * Note:
 *
 */
int32
phy_8390_serdesFiberRx_check(uint32 unit, uint32 port, uint32 *pStatus)
{
    uint32 speed;

    /* only work for giga */
    phy_8390_speed_get(unit, port, &speed);
    if (PORT_SPEED_1000M == speed)
    {
        _phy_8390_serdesFiber1g_handeler(unit, port, pStatus);
    }
    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8390_10gSds_restart
 * Description:
 *      Restart 10G port serdes
 * Input:
 *      unit          - unit id
 *      port          - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8390_10gSds_restart(uint32 unit, rtk_port_t port)
{
    uint32  speed;
    int32   ret;
    /* check Init status */

    /* parameter check */
    RT_PARAM_CHK(!HWP_10GE_PORT(unit, port), RT_ERR_PORT_ID);

    if (HWP_CHIP_REV(unit) < CHIP_REV_ID_C)
        return RT_ERR_OK;

    if ((ret = phy_8390_speed_get(unit, port, &speed)) != RT_ERR_OK)
        return ret;

    if (PORT_SPEED_10G != speed)
        return RT_ERR_OK;

    _phy_8390_10gSds_restart(unit, port);

    return RT_ERR_OK;
}   /* end of phy_8390_10gSds_restart */


/* Function Name:
 *      phy_8390_10g_init
 * Description:
 *      Init 10G port
 * Input:
 *      unit          - unit id
 *      port          - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8390_10g_init(uint32 unit, rtk_port_t port)
{
    uint32  speed;
    int32   ret;

    /* parameter check */
    RT_PARAM_CHK(!HWP_10GE_PORT(unit, port), RT_ERR_PORT_ID);

    if (HWP_CHIP_REV(unit) <= CHIP_REV_ID_C)
        return RT_ERR_OK;

    if ((ret = phy_8390_speed_get(unit, port, &speed)) != RT_ERR_OK)
        return ret;

    if (PORT_SPEED_10G != speed)
        return RT_ERR_OK;

    /* function body */

    phy_8390_serdes_10g_leq_init(unit, port);

    return RT_ERR_OK;
}



/* Function Name:
 *      phy_8390_linkChange_process
 * Description:
 *      For 10G SerDes link change process
 * Input:
 *      unit          - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8390_linkChange_process(uint32 unit, rtk_port_t port, rtk_port_linkStatus_t linkSts)
{
    int32               ret;
    rtk_port_10gMedia_t media;

    if (!HWP_10GE_PORT(unit, port))
    {
        return RT_ERR_OK;
    }

    if (( ret = phy_8390_10gMedia_get(unit, port, &media)) != RT_ERR_OK)
    {
        return ret;
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "port %u media:%d link:%s", port, media, ((linkSts==PORT_LINKDOWN)?"DOWN":"UP"));
    switch (media)
    {
        case PORT_10GMEDIA_FIBER_10G:
        case PORT_10GMEDIA_DAC_50CM:
        case PORT_10GMEDIA_DAC_100CM:
        case PORT_10GMEDIA_DAC_300CM:
            if (PORT_LINKDOWN == linkSts)
            {
                ret = phy_8390_10g_init(unit, port);
            }
            else
            {
                ret = phy_8390_10gSds_restart(unit, port);
            }
            break;
        default:
            break;
    }
    return ret;
}

/* Function Name:
 *      phy_8390_masterSlave_get
 * Description:
 *      Get PHY configuration of master/slave mode of the specific port
 * Input:
 *      unit                - unit id
 *      port                - port id
 * Output:
 *      pMasterSlaveCfg     - pointer to the PHY master slave configuration
 *      pMasterSlaveActual  - pointer to the PHY master slave actual link status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      This function only works on giga/ 10g port to get its master/slave mode configuration.
 */
int32
phy_8390_masterSlave_get(uint32 unit, rtk_port_t port,
    rtk_port_masterSlave_t *pMasterSlaveCfg, rtk_port_masterSlave_t *pMasterSlaveActual)
{
    uint32  speed;
    int32   ret;

    if (ENABLED == sgmiiEn)
        return RT_ERR_NOT_ALLOWED;

    if ((ret = phy_8390_speed_get(unit, port, &speed)) != RT_ERR_OK)
        return ret;

    if (PORT_SPEED_10G == speed)
    {
        ret = phy_common_masterSlaveMmd_get(unit, port, pMasterSlaveCfg, pMasterSlaveActual);
    }
    else
    {
        ret = phy_common_masterSlave_get(unit, port, pMasterSlaveCfg, pMasterSlaveActual);
    }

    return ret;
}   /* end of phy_8390_masterSlave_get */

/* Function Name:
 *      phy_8390_masterSlave_set
 * Description:
 *      Set PHY configuration of master/slave mode of the specific port
 * Input:
 *      unit                - unit id
 *      port                - port id
 *      masterSlave         - PHY master slave configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT         - invalid input parameter
 * Note:
 *      None
 */
int32
phy_8390_masterSlave_set(uint32 unit, rtk_port_t port,
    rtk_port_masterSlave_t masterSlave)
{
    uint32  speed;
    int32   ret;

    if (ENABLED == sgmiiEn)
        return RT_ERR_NOT_ALLOWED;

    if ((ret = phy_8390_speed_get(unit, port, &speed)) != RT_ERR_OK)
        return ret;

    if (PORT_SPEED_10G == speed)
    {
        ret = phy_common_masterSlaveMmd_set(unit, port, masterSlave);
    }
    else
    {
        ret = phy_common_masterSlave_set(unit, port, masterSlave);
    }

    return ret;
}   /* end of phy_8390_masterSlave_set */

/* Function Name:
 *      phy_8390_serdes_loopback_get
 * Description:
 *      Get serdes Loopback status of the specific port
 * Input:
 *      unit                - unit id
 *      port                - port id
 * Output:
 *      pEnable           -loopback mode status;
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8390_serdes_loopback_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  reg, field;
    uint32  val = 0;

    if (HWP_10GE_PORT(unit, port))
    {
        if (24 == port)
            reg = CYPRESS_SDS8_9_TGRXr;
        else if (36 == port)
            reg = CYPRESS_SDS12_13_TGRXr;
        else
            return RT_ERR_PORT_ID;

        field = CYPRESS_TGX1_CFG_AFE_LPKf;
    }
    else
    {
        if (48 == port)
            reg = CYPRESS_SDS12_13_XSG0r;
        else if (49 == port)
            reg = CYPRESS_SDS12_13_XSG1r;
        else
            return RT_ERR_PORT_ID;

        field = CYPRESS_SR0_AFE_LPKf;
    }

    if((ret = reg_field_read(unit, reg, field, &val) != RT_ERR_OK))
    {
        RT_ERR(ret, (MOD_DAL|MOD_PHY), "");
        return ret;
    }

    if (1 == val)
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;

    return RT_ERR_OK;
}   /* end of phy_8390_serdes_loopback_get */

/* Function Name:
 *      phy_8390_serdes_loopback_set
 * Description:
 *      Set serdes Loopback status of the specific port
 * Input:
 *      unit                - unit id
 *      port                - port id
 *      enable           -loopback mode status;
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8390_serdes_loopback_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
    uint32  reg, field;
    uint32  val = 0;

    if (HWP_10GE_PORT(unit, port))
    {
        if (24 == port)
            reg = CYPRESS_SDS8_9_TGRXr;
        else if (36 == port)
            reg = CYPRESS_SDS12_13_TGRXr;
        else
            return RT_ERR_PORT_ID;

        field = CYPRESS_TGX1_CFG_AFE_LPKf;
    }
    else
    {
        if (48 == port)
            reg = CYPRESS_SDS12_13_XSG0r;
        else if (49 == port)
            reg = CYPRESS_SDS12_13_XSG1r;
        else
            return RT_ERR_PORT_ID;

        field = CYPRESS_SR0_AFE_LPKf;
    }

    if (ENABLED == enable)
        val = 1;
    else
        val = 0;

    if((ret = reg_field_write(unit, reg, field, &val) != RT_ERR_OK))
    {
        RT_ERR(ret, (MOD_DAL|MOD_PHY), "");
        return ret;
    }

    return RT_ERR_OK;
}   /* end of phy_8390_serdes_loopback_set */

/* Function Name:
 *      phy_8390_sds_get
 * Description:
 *      Get PHY SerDes information
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      sdsCfg  - SerDes configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      None
 */
int32
phy_8390_sds_get(uint32 unit, rtk_port_t port, rtk_sdsCfg_t *sdsCfg)
{
    uint32  sdsMode, ofst, reg, val;
    int32   ret;

    if (HWP_10GE_PORT(unit, port))
    {
        if (24 == port)
        {
            ofst = 8;
            reg = CYPRESS_SDS8_9_XSG0r;
        }
        else
        {
            ofst = 12;
            reg = CYPRESS_SDS12_13_XSG0r;
        }
    }
    else
    {
        if (48 == port)
        {
            ofst = 12;
            reg = CYPRESS_SDS12_13_XSG0r;
        }
        else
        {
            ofst = 13;
            reg = CYPRESS_SDS12_13_XSG1r;
        }
    }

    if ((ret = reg_array_field_read(unit, CYPRESS_MAC_SERDES_IF_CTRLr,
            REG_ARRAY_INDEX_NONE, ofst, CYPRESS_SERDES_SPD_SELf, &sdsMode))
            != RT_ERR_OK)
        return ret;

    switch (sdsMode)
    {
        case 7:
            if ((ret = reg_field_read(unit, reg, CYPRESS_SR4_CFG_FRC_SDS_MODE_ENf, &val)) != RT_ERR_OK)
                return ret;

            if (val == 1)
                sdsCfg->sdsMode = RTK_MII_SGMII;
            else
                sdsCfg->sdsMode = RTK_MII_1000BX_FIBER;
            break;
        default:
            return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}   /* end of phy_8390_sds_get */

int32
_phy_8390_sds_sgmii_set(uint32 unit, rtk_port_t port)
{
    uint32  sdsMode = 7, ofst, reg, val;
    int32   ret;

    if (HWP_10GE_PORT(unit, port))
    {
        if (24 == port)
        {
            ofst = 8;
            reg = CYPRESS_SDS8_9_XSG0r;
        }
        else
        {
            ofst = 12;
            reg = CYPRESS_SDS12_13_XSG0r;
        }
    }
    else
    {
        if (48 == port)
        {
            ofst = 12;
            reg = CYPRESS_SDS12_13_XSG0r;
        }
        else
        {
            ofst = 13;
            reg = CYPRESS_SDS12_13_XSG1r;
        }
    }

    val = 0;
    if ((ret = reg_array_field_write(unit, CYPRESS_MAC_SERDES_IF_CTRLr,
            REG_ARRAY_INDEX_NONE, ofst, CYPRESS_SERDES_SPD_SELf,
            &val)) != RT_ERR_OK)
    {
        return ret;
    }

    osal_time_mdelay(100);

    val = 2;
    if ((ret = reg_field_write(unit, reg, CYPRESS_SR4_CFG_FRC_SDS_MODEf, &val)) != RT_ERR_OK)
        return ret;

    val = 1;
    if ((ret = reg_field_write(unit, reg, CYPRESS_SR4_CFG_FRC_SDS_MODE_ENf, &val)) != RT_ERR_OK)
        return ret;

    if ((ret = reg_array_field_write(unit, CYPRESS_MAC_SERDES_IF_CTRLr,
            REG_ARRAY_INDEX_NONE, ofst, CYPRESS_SERDES_SPD_SELf,
            &sdsMode)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
}   /* end of phy_8390_sds_sgmii_set */

int32
_phy_8390_sds_1000BX_set(uint32 unit, rtk_port_t port)
{
    uint32  reg, val;
    int32   ret;

    if (HWP_10GE_PORT(unit, port))
    {
        if (24 == port)
        {
            reg = CYPRESS_SDS8_9_XSG0r;
        }
        else
        {
            reg = CYPRESS_SDS12_13_XSG0r;
        }
    }
    else
    {
        if (48 == port)
        {
            reg = CYPRESS_SDS12_13_XSG0r;
        }
        else
        {
            reg = CYPRESS_SDS12_13_XSG1r;
        }
    }

    val = 0;
    if ((ret = reg_field_write(unit, reg, CYPRESS_SR4_CFG_FRC_SDS_MODE_ENf, &val)) != RT_ERR_OK)
        return ret;

    ret = phy_8390_speed_set(unit, port, PORT_SPEED_1000M);

    return ret;
}   /* end of _phy_8390_sds_1000BX_set */

/* Function Name:
 *      phy_8390_sds_set
 * Description:
 *      Set PHY SerDes information
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      sdsCfg  - SerDes configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      None
 */
int32
phy_8390_sds_set(uint32 unit, rtk_port_t port, rtk_sdsCfg_t *sdsCfg)
{
    int32   ret;

    switch (sdsCfg->sdsMode)
    {
        case RTK_MII_SGMII:
            ret = _phy_8390_sds_sgmii_set(unit, port);
            sgmiiEn = ENABLED;
            break;
        case RTK_MII_1000BX_FIBER:
            sgmiiEn = DISABLED;
            ret = _phy_8390_sds_1000BX_set(unit, port);
            break;
        default:
            return RT_ERR_FAILED;
    }

    return ret;
}   /* end of phy_8390_sds_set */

#endif /* !defined(__BOOTLOADER__) */

