/*
 * Copyright (C) 2021 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: $
 * $Date: $
 *
 * Purpose : PHY experimental Driver APIs.
 *
 * Feature : PHY experimental Driver APIs
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
#include <hal/phy/phydef.h>
#include <hal/phy/phy_common.h>

/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */

rt_phyInfo_t phy_8211FS_info =
{
    .phy_num    = 1,
    .eth_type   = HWP_GE,
    .isComboPhy = {0},
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

/* Function Name:
 *      phy_experimental_8211FS_init
 * Description:
 *      Initialize experimental PHY.
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
phy_experimental_8211FS_init(uint32 unit, rtk_port_t port)
{
    return RT_ERR_OK;
}

/* Function Name:
 *      phy_experimental_8211FS_mapperInit
 * Description:
 *      Initialize experimental PHY driver.
 * Input:
 *      pPhydrv - pointer of phy driver
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void
phy_experimental_8211FS_mapperInit(rt_phydrv_t *pPhydrv)
{
    pPhydrv->phydrv_index                               = RT_PHYDRV_EXP_RTL8211FS;
    pPhydrv->fPhydrv_init                               = phy_experimental_8211FS_init;
    pPhydrv->fPhydrv_enable_set                         = phy_common_enable_set;
    pPhydrv->fPhydrv_enable_get                         = phy_common_enable_get;

#if !defined(__BOOTLOADER__)
    pPhydrv->fPhydrv_reset_set                          = phy_common_reset_set;
    pPhydrv->fPhydrv_autoNegoEnable_get                 = phy_common_autoNegoEnable_get;
    pPhydrv->fPhydrv_autoNegoEnable_set                 = phy_common_autoNegoEnable_set;
    pPhydrv->fPhydrv_autoNegoAbility_get                = phy_common_autoNegoAbility_get;
    pPhydrv->fPhydrv_autoNegoAbility_set                = phy_common_autoNegoAbility_set;
    pPhydrv->fPhydrv_duplex_get                         = phy_common_duplex_get;
    pPhydrv->fPhydrv_duplex_set                         = phy_common_duplex_set;
    pPhydrv->fPhydrv_speed_get                          = phy_common_speed_get;
    pPhydrv->fPhydrv_speed_set                          = phy_common_speed_set;
    pPhydrv->fPhydrv_masterSlave_get                    = phy_common_masterSlave_get;
    pPhydrv->fPhydrv_masterSlave_set                    = phy_common_masterSlave_set;
    pPhydrv->fPhydrv_linkStatus_get                     = phy_common_linkStatus_get;
    pPhydrv->fPhydrv_peerAutoNegoAbility_get            = phy_common_copperPeerAutoNegoAbility_get;

    pPhydrv->fPhydrv_reg_get                            = phy_common_reg_get;
    pPhydrv->fPhydrv_reg_set                            = phy_common_reg_set;
    pPhydrv->fPhydrv_reg_park_get                       = phy_common_reg_park_get;
    pPhydrv->fPhydrv_reg_park_set                       = phy_common_reg_park_set;
    pPhydrv->fPhydrv_reg_mmd_get                        = phy_common_reg_mmd_get;
    pPhydrv->fPhydrv_reg_mmd_set                        = phy_common_reg_mmd_set;
    pPhydrv->fPhydrv_reg_mmd_portmask_set               = phy_common_reg_mmd_portmask_set;
#endif
}

