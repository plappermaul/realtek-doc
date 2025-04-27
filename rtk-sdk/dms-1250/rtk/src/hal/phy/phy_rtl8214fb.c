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
 * Purpose : PHY 8212B/8214B/8214BF Driver APIs.
 *
 * Feature : PHY 8212B/8214B/8214BF Driver APIs
 *
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/debug/rt_log.h>
#include <ioal/mem32.h>
#include <hal/common/halctrl.h>
#include <hal/phy/phydef.h>
#include <hal/phy/phy_common.h>
#include <hal/phy/phy_rtl8214fb.h>
#include <rtk/default.h>
#include <hal/phy/phy_rtl8214fb_patch.h>
#include <hal/mac/miim_common_drv.h>
#include <osal/memory.h>
#include <osal/time.h>


/*
 * Symbol Definition
 */

typedef struct phy_8214fb_confcode_port_s {
    uint16  page_no;
    uint16  reg_no;
    uint16  reg_val;
} phy_8214fb_confcode_port_t;

typedef struct hal_phy_info_s
{
    uint8   auto_1000f[RTK_MAX_NUM_OF_PORTS]; /* copper, reg[9].bit[9] shadow for patch down-speed mechanism */
    uint8   ldps_state[RTK_MAX_NUM_OF_PORTS]; /* copper, reg[21].bit[12] shadow for link-down power saving state */
    rtk_portmask_t      autoNegoEnablePortMask;       /* auto nego shadow state for loopback */
} hal_phy_info_t;

static hal_phy_info_t   *pPhy_info[RTK_MAX_NUM_OF_UNIT];
static uint32           phyInfo_alloc[RTK_MAX_NUM_OF_UNIT];

/*
 * Data Declaration
 */
rt_phyInfo_t phy_8214b_info =
{
    .phy_num    = PORT_NUM_IN_8214FB,
    .eth_type   = HWP_GE,
    .isComboPhy = {1, 1, 1, 1, 0, 0, 0, 0},
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

rt_phyInfo_t phy_8212b_info =
{
    .phy_num    = PORT_NUM_IN_8212B,
    .eth_type   = HWP_GE,
    .isComboPhy = {1, 1, 0, 0, 0, 0, 0, 0},
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

#define PHY_MEDIA_LINKDOWN          (0)
#define PHY_MEDIA_LINKUP            (1)

/*
 * Function Declaration
 */

 /* Function Name:
 *      phy_8214FBdrv_ge_mapperInit
 * Description:
 *      Initialize PHY 8214FB driver.
 * Input:
 *      pPhydrv - pointer of phy driver
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void phy_8214FBdrv_ge_mapperInit(rt_phydrv_t *pPhydrv)
{
    pPhydrv->phydrv_index = RT_PHYDRV_RTL8214FB;
    pPhydrv->fPhydrv_init = phy_8214fb_init;
    pPhydrv->fPhydrv_enable_set = phy_8214fb_enable_set;
    pPhydrv->fPhydrv_ieeeTestMode_set = phy_8214fb_ieeeTestMode_set;
#if !defined(__BOOTLOADER__)
    pPhydrv->fPhydrv_media_get = phy_8214fb_media_get;
    pPhydrv->fPhydrv_media_set = phy_8214fb_media_set;
    pPhydrv->fPhydrv_autoNegoEnable_get = phy_8214fb_autoNegoEnable_get;
    pPhydrv->fPhydrv_autoNegoEnable_set = phy_8214fb_autoNegoEnable_set;
    pPhydrv->fPhydrv_autoNegoAbility_get = phy_8214fb_autoNegoAbility_get;
    pPhydrv->fPhydrv_autoNegoAbility_set = phy_8214fb_autoNegoAbility_set;
    pPhydrv->fPhydrv_duplex_get = phy_8214fb_duplex_get;
    pPhydrv->fPhydrv_duplex_set = phy_8214fb_duplex_set;
    pPhydrv->fPhydrv_speed_get = phy_8214fb_speed_get;
    pPhydrv->fPhydrv_speed_set = phy_8214fb_speed_set;
    pPhydrv->fPhydrv_greenEnable_get = phy_8214fb_greenEnable_get;
    pPhydrv->fPhydrv_greenEnable_set = phy_8214fb_greenEnable_set;
    pPhydrv->fPhydrv_eeeEnable_get = phy_8214fb_eeeEnable_get;
    pPhydrv->fPhydrv_eeeEnable_set = phy_8214fb_eeeEnable_set;
    pPhydrv->fPhydrv_fiber_media_get = phy_8214fb_fiber_media_get;
    pPhydrv->fPhydrv_fiber_media_set = phy_8214fb_fiber_media_set;
    pPhydrv->fPhydrv_downSpeedEnable_get = phy_8214fb_downSpeedEnable_get;
    pPhydrv->fPhydrv_downSpeedEnable_set = phy_8214fb_downSpeedEnable_set;
    pPhydrv->fPhydrv_fiberOAMLoopBackEnable_set = phy_8214fb_fiberOAMLoopBack_set;
    pPhydrv->fPhydrv_loopback_get = phy_8214fb_loopback_get;
    pPhydrv->fPhydrv_loopback_set = phy_8214fb_loopback_set;
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
    pPhydrv->fPhydrv_masterSlave_get = phy_common_masterSlave_get;
    pPhydrv->fPhydrv_masterSlave_set = phy_common_masterSlave_set;
    pPhydrv->fPhydrv_crossOverMode_get = phy_8214fb_crossOverMode_get;
    pPhydrv->fPhydrv_crossOverMode_set = phy_8214fb_crossOverMode_set;
    pPhydrv->fPhydrv_rtctResult_get = phy_8214fb_rtctResult_get;
    pPhydrv->fPhydrv_rtct_start = phy_8214fb_rtct_start;
#endif
} /* end of phy_8214FBdrv_ge_mapperInit*/

/* Function Name:
 *      phy_8214Bdrv_ge_mapperInit
 * Description:
 *      Initialize PHY 8214B driver.
 * Input:
 *      pPhydrv - pointer of phy driver
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void phy_8214Bdrv_ge_mapperInit(rt_phydrv_t *pPhydrv)
{
    pPhydrv->phydrv_index = RT_PHYDRV_RTL8214B;
    pPhydrv->fPhydrv_init = phy_8214fb_init;
    pPhydrv->fPhydrv_enable_set = phy_8214fb_enable_set;
    pPhydrv->fPhydrv_ieeeTestMode_set = phy_8214fb_ieeeTestMode_set;
#if !defined(__BOOTLOADER__)
    pPhydrv->fPhydrv_media_get = phy_8214fb_media_get;
    pPhydrv->fPhydrv_media_set = phy_8214fb_media_set;
    pPhydrv->fPhydrv_autoNegoEnable_get = phy_8214fb_autoNegoEnable_get;
    pPhydrv->fPhydrv_autoNegoEnable_set = phy_8214fb_autoNegoEnable_set;
    pPhydrv->fPhydrv_autoNegoAbility_get = phy_8214fb_autoNegoAbility_get;
    pPhydrv->fPhydrv_autoNegoAbility_set = phy_8214fb_autoNegoAbility_set;
    pPhydrv->fPhydrv_duplex_get = phy_8214fb_duplex_get;
    pPhydrv->fPhydrv_duplex_set = phy_8214fb_duplex_set;
    pPhydrv->fPhydrv_speed_get = phy_8214fb_speed_get;
    pPhydrv->fPhydrv_speed_set = phy_8214fb_speed_set;
    pPhydrv->fPhydrv_greenEnable_get = phy_8214fb_greenEnable_get;
    pPhydrv->fPhydrv_greenEnable_set = phy_8214fb_greenEnable_set;
    pPhydrv->fPhydrv_eeeEnable_get = phy_8214fb_eeeEnable_get;
    pPhydrv->fPhydrv_eeeEnable_set = phy_8214fb_eeeEnable_set;
    pPhydrv->fPhydrv_fiber_media_get = phy_8214fb_fiber_media_get;
    pPhydrv->fPhydrv_fiber_media_set = phy_8214fb_fiber_media_set;
    pPhydrv->fPhydrv_downSpeedEnable_get = phy_8214fb_downSpeedEnable_get;
    pPhydrv->fPhydrv_downSpeedEnable_set = phy_8214fb_downSpeedEnable_set;
    pPhydrv->fPhydrv_fiberOAMLoopBackEnable_set = phy_8214fb_fiberOAMLoopBack_set;
    pPhydrv->fPhydrv_loopback_get = phy_8214fb_loopback_get;
    pPhydrv->fPhydrv_loopback_set = phy_8214fb_loopback_set;
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
    pPhydrv->fPhydrv_masterSlave_get = phy_common_masterSlave_get;
    pPhydrv->fPhydrv_masterSlave_set = phy_common_masterSlave_set;
    pPhydrv->fPhydrv_rtctResult_get = phy_8214fb_rtctResult_get;
    pPhydrv->fPhydrv_rtct_start = phy_8214fb_rtct_start;
    pPhydrv->fPhydrv_crossOverMode_get = phy_8214fb_crossOverMode_get;
    pPhydrv->fPhydrv_crossOverMode_set = phy_8214fb_crossOverMode_set;
    pPhydrv->fPhydrv_linkDownPowerSavingEnable_get = phy_8214fb_linkDownPowerSavingEnable_get;
    pPhydrv->fPhydrv_linkDownPowerSavingEnable_set = phy_8214fb_linkDownPowerSavingEnable_set;
    pPhydrv->fPhydrv_gigaLiteEnable_get = phy_8214fb_gigaLiteEnable_get;
#endif

} /* end of phy_8214Bdrv_ge_mapperInit*/

/* Function Name:
 *      phy_8212Bdrv_ge_mapperInit
 * Description:
 *      Initialize PHY 8212B driver.
 * Input:
 *      pPhydrv - pointer of phy driver
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void phy_8212Bdrv_ge_mapperInit(rt_phydrv_t *pPhydrv)
{
    pPhydrv->phydrv_index = RT_PHYDRV_RTL8212B;
    pPhydrv->fPhydrv_init = phy_8214fb_init;
    pPhydrv->fPhydrv_enable_set = phy_8214fb_enable_set;
    pPhydrv->fPhydrv_ieeeTestMode_set = phy_8214fb_ieeeTestMode_set;
#if !defined(__BOOTLOADER__)
    pPhydrv->fPhydrv_media_get = phy_8214fb_media_get;
    pPhydrv->fPhydrv_media_set = phy_8214fb_media_set;
    pPhydrv->fPhydrv_autoNegoEnable_get = phy_8214fb_autoNegoEnable_get;
    pPhydrv->fPhydrv_autoNegoEnable_set = phy_8214fb_autoNegoEnable_set;
    pPhydrv->fPhydrv_autoNegoAbility_get = phy_8214fb_autoNegoAbility_get;
    pPhydrv->fPhydrv_autoNegoAbility_set = phy_8214fb_autoNegoAbility_set;
    pPhydrv->fPhydrv_duplex_get = phy_8214fb_duplex_get;
    pPhydrv->fPhydrv_duplex_set = phy_8214fb_duplex_set;
    pPhydrv->fPhydrv_speed_get = phy_8214fb_speed_get;
    pPhydrv->fPhydrv_speed_set = phy_8214fb_speed_set;
    pPhydrv->fPhydrv_greenEnable_get = phy_8214fb_greenEnable_get;
    pPhydrv->fPhydrv_greenEnable_set = phy_8214fb_greenEnable_set;
    pPhydrv->fPhydrv_eeeEnable_get = phy_8214fb_eeeEnable_get;
    pPhydrv->fPhydrv_eeeEnable_set = phy_8214fb_eeeEnable_set;
    pPhydrv->fPhydrv_fiber_media_get = phy_8214fb_fiber_media_get;
    pPhydrv->fPhydrv_fiber_media_set = phy_8214fb_fiber_media_set;
    pPhydrv->fPhydrv_downSpeedEnable_get = phy_8214fb_downSpeedEnable_get;
    pPhydrv->fPhydrv_downSpeedEnable_set = phy_8214fb_downSpeedEnable_set;
    pPhydrv->fPhydrv_fiberOAMLoopBackEnable_set = phy_8214fb_fiberOAMLoopBack_set;
    pPhydrv->fPhydrv_loopback_get = phy_8214fb_loopback_get;
    pPhydrv->fPhydrv_loopback_set = phy_8214fb_loopback_set;
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
    pPhydrv->fPhydrv_masterSlave_get = phy_common_masterSlave_get;
    pPhydrv->fPhydrv_masterSlave_set = phy_common_masterSlave_set;
    pPhydrv->fPhydrv_rtctResult_get = phy_8214fb_rtctResult_get;
    pPhydrv->fPhydrv_rtct_start = phy_8214fb_rtct_start;
    pPhydrv->fPhydrv_crossOverMode_get = phy_8214fb_crossOverMode_get;
    pPhydrv->fPhydrv_crossOverMode_set = phy_8214fb_crossOverMode_set;
    pPhydrv->fPhydrv_linkDownPowerSavingEnable_get = phy_8214fb_linkDownPowerSavingEnable_get;
    pPhydrv->fPhydrv_linkDownPowerSavingEnable_set = phy_8214fb_linkDownPowerSavingEnable_set;
    pPhydrv->fPhydrv_gigaLiteEnable_get = phy_8214fb_gigaLiteEnable_get;
#endif
} /* end of phy_8212Bdrv_ge_mapperInit*/

/* Function Name:
 *      phy_8214fb_init
 * Description:
 *      Initialize PHY 8212B/8214B/8214FB.
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
phy_8214fb_init(uint32 unit, rtk_port_t port)
{
    uint32  val;
    uint32  base_port = 0;
    uint32  def_port_state = RTK_DEFAULT_PORT_ADMIN_ENABLE; /* enable(1), disable(0) */
    int32   ret = RT_ERR_FAILED;
    uint32  serdes_link, retry_serdes_count = 0;
    uint32  version, is_fb, is_12b = 0;

    base_port = port - (port % PORT_NUM_IN_8214FB);

    /* Initialize the PHY software shadow */
    if (phyInfo_alloc[unit] == 0)
    {
        pPhy_info[unit] = (hal_phy_info_t *)osal_alloc(sizeof(hal_phy_info_t));
        if (NULL == pPhy_info[unit])
        {
            RT_ERR(RT_ERR_FAILED, (MOD_HAL), "memory allocate failed");
            return RT_ERR_FAILED;
        }
        osal_memset(pPhy_info[unit], 0, sizeof(hal_phy_info_t));
        phyInfo_alloc[unit] = 1;
    }


    /* Disable the auto senseing and force the media to fiber */
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_8, 30, &val)) != RT_ERR_OK)
        return ret;
    val |= 0x2000;
    val |= 0x1000;
    if ((ret = hal_miim_write(unit, port, PHY_PAGE_8, 30, val)) != RT_ERR_OK)
        return ret;

    /* Patch some 8328 MAC auto config phy register back to chip default
     * PHY X, page 0, reg 4, value 0x01A0 (for fiber-1000)
     * PHY X, page 0, reg 9, value 0x0000 (for fiber-1000)
     */
    if ((ret = hal_miim_write(unit, port, PHY_PAGE_8, 4, 0x01A0)) != RT_ERR_OK)
        return ret;
    if ((ret = hal_miim_write(unit, port, PHY_PAGE_8, 9, 0x0000)) != RT_ERR_OK)
        return ret;

    /* Set the default port state */
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &val)) != RT_ERR_OK)
        return ret;
    if (def_port_state)
        val &= ~(0x1 << 11);
    else
        val |= (0x1 << 11);
    if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, val)) != RT_ERR_OK)
        return ret;

    /* Disable the auto senseing and force the media to copper */
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_8, 30, &val)) != RT_ERR_OK)
        return ret;
    val |= 0x2000;
    val &= ~(1<<12);
    if ((ret = hal_miim_write(unit, port, PHY_PAGE_8, 30, val)) != RT_ERR_OK)
        return ret;

    /* Read the chip copper Auto-1000F ability value and initial the shadow value */
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, 9, &val)) != RT_ERR_OK)
        return ret;
    /* store to shadow */
    (*pPhy_info[unit]).auto_1000f[port] = (val >> _1000Base_TFullDuplex_OFFSET) & 0x1;

    /* Read the chip copper LDPS state value and initial the shadow value */
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, 21, &val)) != RT_ERR_OK)
        return ret;
    /* store ldps state to shadow */
    (*pPhy_info[unit]).ldps_state[port] = (val >> 12) & 0x1;

    /* Set the default port state */
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &val)) != RT_ERR_OK)
        return ret;
    if (def_port_state)
        val &= ~(0x1 << 11);
    else
        val |= (0x1 << 11);
    if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, val)) != RT_ERR_OK)
        return ret;

    /*if chip default media is auto fiber or auto copper, force media to copper to prevent auto sensing problem.*/
    phy_8214fb_media_set(unit, port, PORT_MEDIA_COPPER);

    /* Configure option when 1000-X Nway is failure, try force mode */
    if ((ret = hal_miim_write(unit, port, 8, 31, 0x000F)) != RT_ERR_OK)
        return ret;
    if ((ret = hal_miim_write(unit, port, 15, 30, 0x0018)) != RT_ERR_OK)
        return ret;
    if ((ret = hal_miim_write(unit, port, 15, 25, 0x074D)) != RT_ERR_OK)
        return ret;


#if defined(CONFIG_SDK_RTL8380)
    if (HWP_8380_30_FAMILY(unit))
    {
        uint32 phy_data;

        /* For 8380, enable serdes always linkup */
        if((port % 8) == 0)
        {
            /*14B*/
            phy_data = 8;
            ret = hal_miim_write(unit,  port,  15,  30, phy_data);
            if(ret != RT_ERR_OK)
                return ret;

            phy_data = 0x8403;
            ret = hal_miim_write(unit,  port,  15,  21, phy_data);
            if(ret != RT_ERR_OK)
                return ret;

            phy_data = 8;
            ret = hal_miim_write(unit,  port+1,  15,  30, phy_data);
            if(ret != RT_ERR_OK)
                return ret;

            phy_data = 0x8403;
            ret = hal_miim_write(unit,  port+1,  15,  21, phy_data);
            if(ret != RT_ERR_OK)
                return ret;
        }

        /*Enable PktGen for PHY error counter patch*/
        phy_data = 0x5a06;
        ret = hal_miim_write(unit,  port,  6,  0, phy_data);
        if(ret != RT_ERR_OK)
            return ret;
     }
#endif

    /* check the serdes is linkup or not?
     * If serdes is not linkup, reset the serdes by disable/enable serdes.
     * retry 5 time for this mechanism
     */
    if (port == base_port)
    {
        if ((ret = hal_miim_read(unit, base_port, 10, 18, &version)) != RT_ERR_OK)
            return ret;

        is_fb = version & 0xF;
        if (is_fb == 0)
        {
            if ((ret = hal_miim_write(unit, base_port+3, 8, 18, 0x93f0)) != RT_ERR_OK)
            {
                return ret;
            }

            if ((ret = hal_miim_read(unit, base_port+3, 8, 19, &val)) != RT_ERR_OK)
                return ret;

            if (((val & 0xF) == 0xE) || ((val & 0xF) == 0x8))
                is_12b = 1;
            else
                is_12b = 0;
        }

        if (is_fb == 0 && is_12b == 1)
        {
            if ((ret = hal_miim_write(unit, base_port+3, 8, 28, 0xE000)) != RT_ERR_OK) /* disable SerDes */
                return ret;
            osal_time_mdelay(100);
            if ((ret = hal_miim_write(unit, base_port+3, 8, 28, 0xC000)) != RT_ERR_OK) /* enable SerDes */
                return ret;
        }
        else
        {
            if ((ret = hal_miim_write(unit, base_port+3, 8, 28, 0xE000)) != RT_ERR_OK) /* disable SerDes */
                return ret;
            osal_time_mdelay(100);
            if ((ret = hal_miim_write(unit, base_port+3, 8, 28, 0x8000)) != RT_ERR_OK) /* enable SerDes */
                 return ret;
        }

        do {
            osal_time_mdelay(100);
            if ((ret = hal_miim_read(unit, base_port+2, 8, 18, &serdes_link)) != RT_ERR_OK)
                return ret;
            if (is_fb == 0 && is_12b == 1)
            {
                if (serdes_link != 0x0003)
                {
                    if ((ret = hal_miim_write(unit, base_port+3, 8, 28, 0xE000)) != RT_ERR_OK) /* disable SerDes */
                        return ret;
                    osal_time_mdelay(100);
                    if ((ret = hal_miim_write(unit, base_port+3, 8, 28, 0xC000)) != RT_ERR_OK) /* enable SerDes */
                        return ret;
                }
                else
                {
                    break;
                }
            }
            else
            {
                if (serdes_link != 0x0033)
                {
                    if ((ret = hal_miim_write(unit, base_port+3, 8, 28, 0xE000)) != RT_ERR_OK) /* disable SerDes */
                        return ret;
                    osal_time_mdelay(100);
                    if ((ret = hal_miim_write(unit, base_port+3, 8, 28, 0x8000)) != RT_ERR_OK) /* enable SerDes */
                        return ret;
                }
                else
                {
                    break;
                }
            }
        } while (retry_serdes_count++ < 5);
    }

#if defined(CONFIG_SDK_RTL8390)
    if (HWP_8350_FAMILY_ID(unit))
    {
            uint32  idx = 0;

            static confcode_mac_regval_t rtl835x_mp_mac_serdes_rst[] =
            {
                /* Serdes 0 Reset */
                { 0xa3c0, 0x50043f },
                { 0xa3c0, 0xf0043f },
                { 0xa3c0, 0x43f },
                { 0xa004, 0x714670c0 },
                { 0xa004, 0x710670c0 },
                { 0xa104, 0x714670c0 },
                { 0xa104, 0x710670c0 },
                /* Serdes 2 Reset */
                { 0xa7c0, 0x50043f },
                { 0xa7c0, 0xf0043f },
                { 0xa7c0, 0x43f },
                { 0xa404, 0x714670c0 },
                { 0xa404, 0x710670c0 },
                { 0xa504, 0x714670c0 },
                { 0xa504, 0x710670c0 },
                /* Serdes 4 Reset */
                { 0xabc0, 0x50043f },
                { 0xabc0, 0xf0043f },
                { 0xabc0, 0x43f },
                { 0xa804, 0x714670c0 },
                { 0xa804, 0x710670c0 },
                { 0xa904, 0x714670c0 },
                { 0xa904, 0x710670c0 },
                /* Serdes 6 Reset */
                { 0xafc0, 0x50043f },
                { 0xafc0, 0xf0043f },
                { 0xafc0, 0x43f },
                { 0xac04, 0x714670c0 },
                { 0xac04, 0x710670c0 },
                { 0xad04, 0x714670c0 },
                { 0xad04, 0x710670c0 },
                /* Serdes 8 Reset */
                { 0xb3f8, 0x50000 },
                { 0xb3f8, 0xf0000 },
                { 0xb3f8, 0x0 },
                { 0xb004, 0x714670c0 },
                { 0xb004, 0x710670c0 },
                { 0xb104, 0x714670c0 },
                { 0xb104, 0x710670c0 },
                /* Serdes 10 Reset */
                { 0xb7c0, 0x50043f },
                { 0xb7c0, 0xf0043f },
                { 0xb7c0, 0x43f },
                { 0xb404, 0x714670c0 },
                { 0xb404, 0x710670c0 },
                { 0xb504, 0x714670c0 },
                { 0xb504, 0x710670c0 },
                /* Serdes 12 Reset */
                { 0xbbf8, 0x50000 },
                { 0xbbf8, 0xf0000 },
                { 0xbbf8, 0x0 },
                { 0xb804, 0x714670c0 },
                { 0xb804, 0x710670c0 },
                { 0xb904, 0x714670c0 },
                { 0xb904, 0x710670c0 },
            };

            /* Reset Serdes */
            for (idx=0; idx<(sizeof(rtl835x_mp_mac_serdes_rst)/sizeof(confcode_mac_regval_t)); idx++)
            {
                ioal_mem32_write(unit, rtl835x_mp_mac_serdes_rst[idx].reg, \
                                rtl835x_mp_mac_serdes_rst[idx].val);
            }
    }
#endif
    phy_8214fb_greenEnable_set(unit, port, DISABLED);
    return RT_ERR_OK;
} /* end of phy_8214fb_init */

/* Function Name:
 *      phy_8214fb_greenEnable_get
 * Description:
 *      Get the status of link-up green feature of the specific port in the specific unit
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      pEnable - pointer to status of link-up green feature
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      1. The link-up green feature. (Cable Length Power Saving)
 */
int32
phy_8214fb_greenEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    uint32  phyData;
    int32   ret = RT_ERR_FAILED;

    /* getting page 5, register 1, bit[8] for enable/disable green */
    if ((ret = hal_miim_read(unit, port, 5, 1, &phyData)) != RT_ERR_OK)
        return ret;
    if ((phyData >> 8) & 0x1)
        *pEnable = DISABLED;
    else
        *pEnable = ENABLED;

    return RT_ERR_OK;
} /* end of phy_8214fb_greenEnable_get */

/* Function Name:
 *      phy_8214fb_greenEnable_set
 * Description:
 *      Set the status of link-up green feature of the specific port in the specific unit
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - status of link-up green feature
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      1. The link-up green feature. (Cable Length Power Saving)
 */
int32
phy_8214fb_greenEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    uint32  phyData;
    int32   ret = RT_ERR_FAILED;

    /* For Link-On and Cable Length Power Saving (per-port) */
    /* setting page 5, register 1, bit[8] for enable/disable green */
    if ((ret = hal_miim_read(unit, port, 5, 1, &phyData)) != RT_ERR_OK)
        return ret;
    phyData &= 0xFEFF;
    if (ENABLED == enable)
        phyData = 0x0201;
    else
        phyData = 0x0301;
    if ((ret = hal_miim_write(unit, port, 5, 1, phyData)) != RT_ERR_OK)
        return ret;
    /* setting the GreenEthernet TX/RX Threshold */
    if (ENABLED == enable)
    {
        /* enable RX GreenEthernet threshold */
        if ((ret = hal_miim_write(unit, port, 3, 24, 0xBC3D)) != RT_ERR_OK)
            return ret;
        /* enable TX GreenEthernet threshold */
        if ((ret = hal_miim_write(unit, port, 3, 25, 0x3C3D)) != RT_ERR_OK)
            return ret;
    }
    else
    {
        /* disable RX GreenEthernet threshold */
        if ((ret = hal_miim_write(unit, port, 3, 24, 0xBC3C)) != RT_ERR_OK)
            return ret;
        /* disable TX GreenEthernet threshold */
        if ((ret = hal_miim_write(unit, port, 3, 25, 0x3C3C)) != RT_ERR_OK)
            return ret;
    }

    /* For Link-Down Power Saving (per-port) */
    if ((ret = hal_miim_read(unit, port, 0, 21, &phyData)) != RT_ERR_OK)
        return ret;
    phyData &= ~(0x1 << 12);
    if (ENABLED == enable)
    {
        phyData |= (0x1 << 12);
    }
    if ((ret = hal_miim_write(unit, port, 0, 21, phyData)) != RT_ERR_OK)
        return ret;

    return ret;
} /* end of phy_8214fb_greenEnable_set */

/* Function Name:
 *      phy_8214fb_media_get
 * Description:
 *      Get PHY 8212B/8214FB/8214B media type.
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
 *      1. media type is PORT_MEDIA_COPPER or PORT_MEDIA_FIBER
 */
int32
phy_8214fb_media_get(uint32 unit, rtk_port_t port, rtk_port_media_t *pMedia)
{
    uint32  val;
    int32   ret = RT_ERR_FAILED;

    RT_PARAM_CHK((NULL == pMedia), RT_ERR_NULL_POINTER);

    if ((ret = hal_miim_read(unit, port, PHY_PAGE_8, 30, &val)) != RT_ERR_OK)
        return ret;

    if (0x2000 == (val & 0x2000))
    {
        if (0x1000 == (val & 0x1000))
            *pMedia = PORT_MEDIA_FIBER;
        else
            *pMedia = PORT_MEDIA_COPPER;
    }
    else
    {
        return RT_ERR_TYPE;
    }

    return RT_ERR_OK;
} /* end of phy_8214fb_media_get */

/* Function Name:
 *      phy_8214fb_media_set
 * Description:
 *      Get PHY 8212B/8214FB/8214B media type.
 * Input:
 *      unit  - unit id
 *      port  - port id
 *      media - phy media type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      1. media type is PORT_MEDIA_COPPER or PORT_MEDIA_FIBER
 *      2. there are 3 major steps for change the media from original to new
 *         - turn off original media
 *         - turn on new media
 *         - set new media to first priority
 */
int32
phy_8214fb_media_set(uint32 unit, rtk_port_t port, rtk_port_media_t media)
{
    uint32  val, phyReg0, is_phyDown;
    int32   ret = RT_ERR_FAILED;

    switch (media)
    {
        case PORT_MEDIA_COPPER:
            if ((ret = hal_miim_read(unit, port, PHY_PAGE_8, 30, &val)) != RT_ERR_OK)
                return ret;
            if ((((val >> 13) & 0x1) == 1) && (((val >> 12) & 0x1) == 0))
            {
                /* no change media */
                break;
            }
            /* If the original media is PORT_MEDIA_FIBER, need to apply
             * reg0.bit11 power down state from fiber to copper
             */
            if ((((val >> 13) & 0x1) == 1) && (((val >> 12) & 0x1) == 1))
            {
                if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, 0, &phyReg0)) != RT_ERR_OK)
                    return ret;
                is_phyDown = (phyReg0 >> 11) & 0x1;

                /* turn-on copper */
                val &= ~(1<<12); /* select UTP */
                val |= (0x2000); /* disable auto-sensing */
                if ((ret = hal_miim_write(unit, port, PHY_PAGE_8, 30, val)) != RT_ERR_OK)
                    return ret;
                if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, 0, &phyReg0)) != RT_ERR_OK)
                    return ret;
                phyReg0 &= 0xF7FF;
                phyReg0 |= (is_phyDown << 11);
                if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, 0, phyReg0)) != RT_ERR_OK)
                    return ret;
            }
            else
            {
                /* turn-on copper */
                val &= ~(1<<12); /* select UTP */
                val |= (0x2000); /* disable auto-sensing */
                if ((ret = hal_miim_write(unit, port, PHY_PAGE_8, 30, val)) != RT_ERR_OK)
                    return ret;
            }
            /* Force TX 2.5G */
            if ((ret = hal_miim_write(unit, port, PHY_PAGE_8, 31, 0x000F)) != RT_ERR_OK)
                return ret;
            if ((ret = hal_miim_write(unit, port, 15, 30, 0x0018)) != RT_ERR_OK)
                return ret;
            if ((ret = hal_miim_write(unit, port, 15, 25, 0x3749)) != RT_ERR_OK)
                return ret;
            if ((ret = hal_miim_write(unit, port, 15, 31, 0x0008)) != RT_ERR_OK)
                return ret;
            /* force power */
            if ((ret = hal_miim_write(unit, port, PHY_PAGE_8, 31, 0x000F)) != RT_ERR_OK)
                return ret;
            if ((ret = hal_miim_write(unit, port, 15, 30, 0x001E)) != RT_ERR_OK)
                return ret;
            if ((ret = hal_miim_write(unit, port, 15, 21, 0xC129)) != RT_ERR_OK)
                return ret;
            if ((ret = hal_miim_write(unit, port, 15, 31, 0x0008)) != RT_ERR_OK)
                return ret;

            break; /* end of switch-case PORT_MEDIA_COPPER */

        case PORT_MEDIA_FIBER:
            if ((ret = hal_miim_read(unit, port, PHY_PAGE_8, 30, &val)) != RT_ERR_OK)
                return ret;
            if ((((val >> 13) & 0x1) == 1) && (((val >> 12) & 0x1) == 1))
            {
                /* no change media */
                 break;
            }
            /* If the original media is PORT_MEDIA_COPPER, need to apply
             * reg0.bit11 power down state from fiber to copper
             */
            if ((((val >> 13) & 0x1) == 1) && (((val >> 12) & 0x1) == 0))
            {
                if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, 0, &phyReg0)) != RT_ERR_OK)
                    return ret;
                is_phyDown = (phyReg0 >> 11) & 0x1;

                /* turn-on fiber */
                val |= (0x3000); /* disable auto-sensing + select FIBER */
                if ((ret = hal_miim_write(unit, port, PHY_PAGE_8, 30, val)) != RT_ERR_OK)
                    return ret;
                if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, 0, &phyReg0)) != RT_ERR_OK)
                    return ret;
                phyReg0 &= 0xF7FF;
                phyReg0 |= (is_phyDown << 11);
                if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, 0, phyReg0)) != RT_ERR_OK)
                    return ret;
                if (is_phyDown)
                {
                    /* Force TX 2.5G */
                    if ((ret = hal_miim_write(unit, port, PHY_PAGE_8, 31, 0x000F)) != RT_ERR_OK)
                        return ret;
                    if ((ret = hal_miim_write(unit, port, 15, 30, 0x0018)) != RT_ERR_OK)
                        return ret;
                    if ((ret = hal_miim_write(unit, port, 15, 25, 0x374D)) != RT_ERR_OK)
                        return ret;
                    if ((ret = hal_miim_write(unit, port, 15, 31, 0x0008)) != RT_ERR_OK)
                        return ret;
                    /* force power */
                    if ((ret = hal_miim_write(unit, port, PHY_PAGE_8, 31, 0x000F)) != RT_ERR_OK)
                        return ret;
                    if ((ret = hal_miim_write(unit, port, 15, 30, 0x001E)) != RT_ERR_OK)
                        return ret;
                    if ((ret = hal_miim_write(unit, port, 15, 21, 0xC129)) != RT_ERR_OK)
                        return ret;
                    if ((ret = hal_miim_write(unit, port, 15, 31, 0x0008)) != RT_ERR_OK)
                        return ret;
                }
                else
                {
                    /* Default setting */
                    if ((ret = hal_miim_write(unit, port, PHY_PAGE_8, 31, 0x000F)) != RT_ERR_OK)
                        return ret;
                    if ((ret = hal_miim_write(unit, port, 15, 30, 0x0018)) != RT_ERR_OK)
                        return ret;
                    if ((ret = hal_miim_write(unit, port, 15, 25, 0x074D)) != RT_ERR_OK)
                        return ret;
                    if ((ret = hal_miim_write(unit, port, 15, 31, 0x0008)) != RT_ERR_OK)
                        return ret;
                    /* Default setting */
                    if ((ret = hal_miim_write(unit, port, PHY_PAGE_8, 31, 0x000F)) != RT_ERR_OK)
                        return ret;
                    if ((ret = hal_miim_write(unit, port, 15, 30, 0x001E)) != RT_ERR_OK)
                        return ret;
                    if ((ret = hal_miim_write(unit, port, 15, 21, 0xC178)) != RT_ERR_OK)
                        return ret;
                    if ((ret = hal_miim_write(unit, port, 15, 31, 0x0008)) != RT_ERR_OK)
                        return ret;
                }
            }
            else
            {
                /* turn-on fiber */
                val |= (0x3000); /* disable auto-sensing + select FIBER */
                if ((ret = hal_miim_write(unit, port, PHY_PAGE_8, 30, val)) != RT_ERR_OK)
                    return ret;
                /* Default setting */
                if ((ret = hal_miim_write(unit, port, PHY_PAGE_8, 31, 0x000F)) != RT_ERR_OK)
                    return ret;
                if ((ret = hal_miim_write(unit, port, 15, 30, 0x0018)) != RT_ERR_OK)
                    return ret;
                if ((ret = hal_miim_write(unit, port, 15, 25, 0x074D)) != RT_ERR_OK)
                    return ret;
                if ((ret = hal_miim_write(unit, port, 15, 31, 0x0008)) != RT_ERR_OK)
                    return ret;
                /* Default setting */
                if ((ret = hal_miim_write(unit, port, PHY_PAGE_8, 31, 0x000F)) != RT_ERR_OK)
                    return ret;
                if ((ret = hal_miim_write(unit, port, 15, 30, 0x001E)) != RT_ERR_OK)
                    return ret;
                if ((ret = hal_miim_write(unit, port, 15, 21, 0xC178)) != RT_ERR_OK)
                    return ret;
                if ((ret = hal_miim_write(unit, port, 15, 31, 0x0008)) != RT_ERR_OK)
                    return ret;
            }
            break; /* end of switch-case PORT_MEDIA_FIBER */
        default:
            break;
    }

    return RT_ERR_OK;
} /* end of phy_8214fb_media_set */

/* Function Name:
 *      phy_8214fb_auto_1000f_get
 * Description:
 *      Get PHY 8212B/8214FB/8214B copper 1000f ability from shadow.
 * Input:
 *      unit     - unit id
 *      port     - port id
 * Output:
 *      pAbility - pointer to copper 1000f ability
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - invalid parameter
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      1. copper 1000f ability value is as following:
 *      - 0: not the ability
 *      - 1: have the ability
 */
int32
phy_8214fb_auto_1000f_get(uint32 unit, rtk_port_t port, uint32 *pAbility)
{
    RT_PARAM_CHK((NULL == pAbility), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pPhy_info[unit]), RT_ERR_NULL_POINTER);

    *pAbility = (*pPhy_info[unit]).auto_1000f[port];
    return RT_ERR_OK;
} /* end of phy_8214fb_auto_1000f_get */

/* Function Name:
 *      phy_8214fb_auto_1000f_set
 * Description:
 *      Set PHY 8212B/8214FB/8214B copper 1000f ability to shadow.
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      ability  - copper 1000f ability
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - invalid parameter
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      1. copper 1000f ability value is as following:
 *      - 0: not the ability
 *      - 1: have the ability
 */
int32
phy_8214fb_auto_1000f_set(uint32 unit, rtk_port_t port, uint32 ability)
{
    RT_PARAM_CHK((ability != 0) && (ability != 1), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pPhy_info[unit]), RT_ERR_NULL_POINTER);

    (*pPhy_info[unit]).auto_1000f[port] = ability;
    return RT_ERR_OK;
} /* end of phy_8214fb_auto_1000f_set */

/* Function Name:
 *      phy_8214fb_enable_set
 * Description:
 *      Set PHY interface status of the specific port
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - admin configuration of PHY interface
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8214fb_enable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    uint32  phyData, phyData_fiber, phyData_copper;
    uint32  val, restore_val;
    int32   ret;
    hal_control_t   *pHalCtrl;

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_read(unit, port, PHY_PAGE_8, 30, &restore_val)) != RT_ERR_OK)
        return ret;

    if ((restore_val & 0x2000) == 0x0000)
    {
        val = (restore_val | 0x2000);
        if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_write(unit, port, PHY_PAGE_8, 30, val)) != RT_ERR_OK)
            return ret;

        if ((val & 0x0020) == 0x0000)
        {   /* Original media: Auto-Fiber */
            val &= ~(0x1000);
            if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_write(unit, port, PHY_PAGE_8, 30, val)) != RT_ERR_OK)
                return ret;
            if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &phyData_copper)) != RT_ERR_OK)
                return ret;
            phyData_copper &= ~(PowerDown_MASK);
            if (DISABLED == enable)
                phyData_copper |= (1 << PowerDown_OFFSET);
            if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, phyData_copper)) != RT_ERR_OK)
                return ret;

            val |= (0x1000);
            if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_write(unit, port, PHY_PAGE_8, 30, val)) != RT_ERR_OK)
                return ret;
            if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &phyData_fiber)) != RT_ERR_OK)
                return ret;
            phyData_fiber &= ~(PowerDown_MASK);
            if (DISABLED == enable)
                phyData_fiber |= (1 << PowerDown_OFFSET);
            if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, phyData_fiber)) != RT_ERR_OK)
                return ret;
        }
        else
        {   /* Original media: Auto-Copper */
            val |= (0x1000);
            if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_write(unit, port, PHY_PAGE_8, 30, val)) != RT_ERR_OK)
                return ret;
            if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &phyData_fiber)) != RT_ERR_OK)
                return ret;
            phyData_fiber &= ~(PowerDown_MASK);
            if (DISABLED == enable)
                phyData_fiber |= (1 << PowerDown_OFFSET);
            if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, phyData_fiber)) != RT_ERR_OK)
                return ret;

            val &= ~(0x1000);
            if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_write(unit, port, PHY_PAGE_8, 30, val)) != RT_ERR_OK)
                return ret;
            if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &phyData_copper)) != RT_ERR_OK)
                return ret;
            phyData_copper &= ~(PowerDown_MASK);
            if (DISABLED == enable)
                phyData_copper |= (1 << PowerDown_OFFSET);
            if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, phyData_copper)) != RT_ERR_OK)
                return ret;
        }

        if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_write(unit, port, PHY_PAGE_8, 30, restore_val)) != RT_ERR_OK)
            return ret;
    }
    else
    {
        if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &phyData)) != RT_ERR_OK)
            return ret;

        phyData &= ~(PowerDown_MASK);
        if (DISABLED == enable)
            phyData |= (1 << PowerDown_OFFSET);

        if ((restore_val & 0x1000) == 0x1000)
        {   /* force-fiber media */
            if (DISABLED == enable)
            {
                /* Force TX 2.5G */
                if ((ret = hal_miim_write(unit, port, PHY_PAGE_8, 31, 0x000F)) != RT_ERR_OK)
                    return ret;
                if ((ret = hal_miim_write(unit, port, 15, 30, 0x0018)) != RT_ERR_OK)
                    return ret;
                if ((ret = hal_miim_write(unit, port, 15, 25, 0x374D)) != RT_ERR_OK)
                    return ret;
                if ((ret = hal_miim_write(unit, port, 15, 31, 0x0008)) != RT_ERR_OK)
                    return ret;
                /* force power */
                if ((ret = hal_miim_write(unit, port, PHY_PAGE_8, 31, 0x000F)) != RT_ERR_OK)
                    return ret;
                if ((ret = hal_miim_write(unit, port, 15, 30, 0x001E)) != RT_ERR_OK)
                    return ret;
                if ((ret = hal_miim_write(unit, port, 15, 21, 0xC129)) != RT_ERR_OK)
                    return ret;
                if ((ret = hal_miim_write(unit, port, 15, 31, 0x0008)) != RT_ERR_OK)
                    return ret;
            }
            else
            {
                /* Default setting */
                if ((ret = hal_miim_write(unit, port, PHY_PAGE_8, 31, 0x000F)) != RT_ERR_OK)
                    return ret;
                if ((ret = hal_miim_write(unit, port, 15, 30, 0x0018)) != RT_ERR_OK)
                    return ret;
                if ((ret = hal_miim_write(unit, port, 15, 25, 0x074D)) != RT_ERR_OK)
                    return ret;
                if ((ret = hal_miim_write(unit, port, 15, 31, 0x0008)) != RT_ERR_OK)
                    return ret;
                /* Default setting */
                if ((ret = hal_miim_write(unit, port, PHY_PAGE_8, 31, 0x000F)) != RT_ERR_OK)
                    return ret;
                if ((ret = hal_miim_write(unit, port, 15, 30, 0x001E)) != RT_ERR_OK)
                    return ret;
                if ((ret = hal_miim_write(unit, port, 15, 21, 0xC178)) != RT_ERR_OK)
                    return ret;
                if ((ret = hal_miim_write(unit, port, 15, 31, 0x0008)) != RT_ERR_OK)
                    return ret;
            }
        }

        if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, phyData)) != RT_ERR_OK)
            return ret;
    }
    osal_time_udelay(500000);

    return RT_ERR_OK;
} /* end of phy_8214fb_enable_set */


#if !defined(__BOOTLOADER__)
/* Function Name:
 *      _phy_8214fb_intMedia_get
 * Description:
 *      Get 8214fb internal media
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      media         - internal media
 *      link_status  - media link status
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
_phy_8214fb_intMedia_get(uint32 unit, rtk_port_t port, rtk_port_media_t *media, uint32 *link_status)
{
    uint32              phyData1;
    int32               ret;
    uint8               is_fiber_linkup = 0, is_copper_linkup = 0;

    /* get media */
    if ((ret = phy_8214fb_media_get(unit, port, media)) != RT_ERR_OK)
        return ret;

    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_STATUS_REG, &phyData1)) != RT_ERR_OK)
        goto ERR;
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_STATUS_REG, &phyData1)) != RT_ERR_OK)
        goto ERR;

    if (phyData1 & LinkStatus_MASK)
    {
        *link_status = PHY_MEDIA_LINKUP;
        if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_EXTENDED_STATUS_REG, &phyData1)) != RT_ERR_OK)
            goto ERR;
        if((phyData1 & _1000BaseX_FullDuplex_R15_MASK) || (phyData1 & _1000BaseX_HalfDuplex_R15_MASK))
        {
            is_fiber_linkup = PHY_MEDIA_LINKUP; /*Link up media is Fiber*/
        }
        if((phyData1 & _1000Base_TFullDuplex_R15_MASK) || (phyData1 & _1000Base_THalfDuplex_R15_MASK))
        {
            is_copper_linkup = PHY_MEDIA_LINKUP; /*Link up media is Copper*/
        }

    }else{
        is_fiber_linkup = PHY_MEDIA_LINKDOWN;
        is_copper_linkup = PHY_MEDIA_LINKDOWN;
        *link_status = PHY_MEDIA_LINKDOWN;
    }

    if (PORT_MEDIA_FIBER == *media || PORT_MEDIA_COPPER == *media)
        return RT_ERR_OK;

    /* nego media */
    if (is_copper_linkup == is_fiber_linkup)
    {
        *media = PORT_MEDIA_COPPER;
    }
    else if (is_fiber_linkup)
        *media = PORT_MEDIA_FIBER;
    else
        *media = PORT_MEDIA_COPPER;

    return RT_ERR_OK;
ERR:
    return ret;
}   /* end of _phy_8214fb_intMedia_get */

/* Function Name:
 *      phy_8214fb_utpDownSpeedEnable_get
 * Description:
 *      Get UTP down speed 1000M --> 100M status of the specific port
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to status of UTP down speed
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 * Note:
 *      None
 */
int32
phy_8214fb_utpDownSpeedEnable_get(uint32 unit, rtk_port_t port,
    rtk_enable_t *pEnable)
{
    uint32  phyData;
    int32   ret;

    if ((ret = hal_miim_write(unit, port, 0, 31, 7)) != RT_ERR_OK)
        return ret;

    if ((ret = hal_miim_write(unit, port, 7, 30, 0x2d)) != RT_ERR_OK)
        return ret;

    if ((ret = hal_miim_read(unit, port, 7, 24, &phyData)) != RT_ERR_OK)
        goto ERR;

    if (phyData & (1 << 4))
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;

    if ((ret = hal_miim_write(unit, port, 0, 31, 0)) != RT_ERR_OK)
        return ret;

    ERR:
        hal_miim_write(unit, port, 0, 31, 0);

    return ret;
}

/* Function Name:
 *      phy_8214fb_utpDownSpeedEnable_set
 * Description:
 *      Set UTP down speed 1000M --> 100M status of the specific port
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      enable  - status of UTP down speed
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 * Note:
 *      None
 */
int32
phy_8214fb_utpDownSpeedEnable_set(uint32 unit, rtk_port_t port,
    rtk_enable_t enable)
{
    uint32  phyData;
    int32   ret;

    if ((ret = hal_miim_write(unit, port, 0, 31, 7)) != RT_ERR_OK)
        return ret;

    if ((ret = hal_miim_write(unit, port, 7, 30, 0x2d)) != RT_ERR_OK)
        return ret;

    if ((ret = hal_miim_read(unit, port, 7, 24, &phyData)) != RT_ERR_OK)
        goto ERR;

    /* DownSpeed to 100M*/
    phyData &= ~(1 << 4);

    if (ENABLED == enable)
        phyData |= (1 << 4);
    else
        phyData &= ~(1 << 4);

    if ((ret = hal_miim_write(unit, port, 7, 24, phyData)) != RT_ERR_OK)
        goto ERR;

    if ((ret = hal_miim_write(unit, port, 0, 31, 0)) != RT_ERR_OK)
        return ret;

    return ret;
ERR:
    hal_miim_write(unit, port, 0, 31, 0);
    return ret;
}

/* Function Name:
 *      phy_8214fb_downSpeedEnable_get
 * Description:
 *      Get UTP down speed 1000M --> 100M status of the specific port
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to status of down speed
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 * Note:
 *      None
 */
int32
phy_8214fb_downSpeedEnable_get(uint32 unit, rtk_port_t port,
    rtk_enable_t *pEnable)
{
    int32   ret;
    rtk_port_media_t    media;
    uint32              link_sts;

    if ((ret = _phy_8214fb_intMedia_get(unit, port, &media, &link_sts)) != RT_ERR_OK)
        return ret;

    if(PORT_MEDIA_FIBER == media)
    {
        ret = RT_ERR_PHY_MEDIA;
    }else{
        ret = phy_8214fb_utpDownSpeedEnable_get(unit, port, pEnable);
    }
    return ret;
}

/* Function Name:
 *      phy_8214fb_downSpeedEnable_set
 * Description:
 *      Set UTP down speed 1000M --> 100M status of the specific port
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      enable  - status of down speed
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 * Note:
 *      None
 */
int32
phy_8214fb_downSpeedEnable_set(uint32 unit, rtk_port_t port,
    rtk_enable_t enable)
{
    int32   ret;
    rtk_port_media_t    media;
    uint32              link_sts;

    if ((ret = _phy_8214fb_intMedia_get(unit, port, &media, &link_sts)) != RT_ERR_OK)
        return ret;

    if(PORT_MEDIA_FIBER == media)
    {
        ret = RT_ERR_PHY_MEDIA;
    }else{
        ret = phy_8214fb_utpDownSpeedEnable_set(unit, port, enable);
    }
    return ret;
}

/* Function Name:
 *      phy_8214fb_autoNegoAbility_get
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
phy_8214fb_autoNegoAbility_get(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility)
{
    int32   ret;
    uint32  phyData4;
    uint32  phyData9;
    uint32  phyData;
    uint32  restore_val, val;
    rtk_port_media_t media;
    hal_control_t   *pHalCtrl;
    uint32  phyExtStatus15, is_fiber_linkup = 0, is_copper_linkup = 0;

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_STATUS_REG, &val);
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_STATUS_REG, &val)) != RT_ERR_OK)
        return ret;
    if (val & LinkStatus_MASK)
    {
        if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, 15, &phyExtStatus15)) != RT_ERR_OK)
            return ret;
        is_fiber_linkup = (phyExtStatus15 >> 15) & 0x1;
        is_copper_linkup = (phyExtStatus15 >> 13) & 0x1;

        /* get value from internal status register */
        if ((ret = hal_miim_read(unit, port, PHY_PAGE_8, 23, &phyData)) != RT_ERR_OK)
            return ret;

        /* get value from CHIP*/
        if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_AN_ADVERTISEMENT_REG, &phyData4)) != RT_ERR_OK)
            return ret;

        if (is_fiber_linkup)
        {
            if ((phyData & 0x0188) == 0x0088)
            {   /* 100Base-FX Linkup */
                pAbility->FC = (phyData4 & Pause_R4_MASK) >> Pause_R4_OFFSET;
                pAbility->AsyFC = (phyData4 & AsymmetricPause_R4_MASK) >> AsymmetricPause_R4_OFFSET;
                pAbility->Half_10 = 0;
                pAbility->Full_10 = 0;
                pAbility->Half_100 = 0;
                pAbility->Full_100 = 0;
                pAbility->Half_1000 = 0;
                pAbility->Full_1000 = 0;
            }
            else
            {   /* 1000Base-X Linkup */
                pAbility->FC = (phyData4 & _1000BaseX_Pause_R4_MASK) >> _1000BaseX_Pause_R4_OFFSET;
                pAbility->AsyFC = (phyData4 & _1000BaseX_AsymmetricPause_R4_MASK) >> _1000BaseX_AsymmetricPause_R4_OFFSET;
                pAbility->Half_10 = 0;
                pAbility->Full_10 = 0;
                pAbility->Half_100 = 0;
                pAbility->Full_100 = 0;
                pAbility->Half_1000 = (phyData4 & _1000BaseX_HalfDuplex_R4_MASK) >> _1000BaseX_HalfDuplex_R4_OFFSET;
                pAbility->Full_1000 = (phyData4 & _1000BaseX_FullDuplex_R4_MASK) >> _1000BaseX_FullDuplex_R4_OFFSET;
            }
        }
        else if (is_copper_linkup)
        {
            pAbility->FC = (phyData4 & Pause_R4_MASK) >> Pause_R4_OFFSET;
            pAbility->AsyFC = (phyData4 & AsymmetricPause_R4_MASK) >> AsymmetricPause_R4_OFFSET;

            if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_1000_BASET_CONTROL_REG, &phyData9)) != RT_ERR_OK)
                return ret;

            pAbility->Full_100= (phyData4 & _100Base_TX_FD_R4_MASK) >> _100Base_TX_FD_R4_OFFSET;
            pAbility->Half_100= (phyData4 & _100Base_TX_R4_MASK) >> _100Base_TX_R4_OFFSET;
            pAbility->Full_10= (phyData4 & _10Base_T_FD_R4_MASK) >> _10Base_T_FD_R4_OFFSET;
            pAbility->Half_10= (phyData4 & _10Base_T_R4_MASK) >> _10Base_T_R4_OFFSET;
            pAbility->Half_1000 = (phyData9 & _1000Base_THalfDuplex_MASK) >> _1000Base_THalfDuplex_OFFSET;
            pAbility->Full_1000 = (phyData9 & _1000Base_TFullDuplex_MASK) >> _1000Base_TFullDuplex_OFFSET;
        }
        return RT_ERR_OK;
    }

    /* Link-down */
    if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_read(unit, port, PHY_PAGE_8, 30, &restore_val)) != RT_ERR_OK)
        return ret;

    if ((restore_val & 0x2000) == 0x0000)
    {
        val = (restore_val | 0x2000);
        if (restore_val & 0x0020)
            val &= ~(0x1000);
        else
            val |= (0x1000);

        if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_write(unit, port, PHY_PAGE_8, 30, val)) != RT_ERR_OK)
            return ret;
        if (restore_val & 0x0020)
            media = PORT_MEDIA_COPPER;
        else
            media = PORT_MEDIA_FIBER;
    }
    else
    {
        if (restore_val & 0x1000)
            media = PORT_MEDIA_FIBER;
        else
            media = PORT_MEDIA_COPPER;
    }

    /* get value from CHIP*/
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_AN_ADVERTISEMENT_REG, &phyData4)) != RT_ERR_OK)
        return ret;

    if (PORT_MEDIA_FIBER == media)
    {
        if ((restore_val & 0x0c00) == 0x0c00)
        {   /* 100-fx */
            pAbility->FC = (phyData4 & Pause_R4_MASK) >> Pause_R4_OFFSET;
            pAbility->AsyFC = (phyData4 & AsymmetricPause_R4_MASK) >> AsymmetricPause_R4_OFFSET;
            pAbility->Half_10 = 0;
            pAbility->Full_10 = 0;
            pAbility->Half_100 = 0;
            pAbility->Full_100 = 0;
            pAbility->Half_1000 = 0;
            pAbility->Full_1000 = 0;
        }
        else
        {   /* 1000-x */
            pAbility->FC = (phyData4 & _1000BaseX_Pause_R4_MASK) >> _1000BaseX_Pause_R4_OFFSET;
            pAbility->AsyFC = (phyData4 & _1000BaseX_AsymmetricPause_R4_MASK) >> _1000BaseX_AsymmetricPause_R4_OFFSET;
            pAbility->Half_10 = 0;
            pAbility->Full_10 = 0;
            pAbility->Half_100 = 0;
            pAbility->Full_100 = 0;
            pAbility->Half_1000 = (phyData4 & _1000BaseX_HalfDuplex_R4_MASK) >> _1000BaseX_HalfDuplex_R4_OFFSET;
            pAbility->Full_1000 = (phyData4 & _1000BaseX_FullDuplex_R4_MASK) >> _1000BaseX_FullDuplex_R4_OFFSET;
        }
    }
    else if (PORT_MEDIA_COPPER == media)
    {
        pAbility->FC = (phyData4 & Pause_R4_MASK) >> Pause_R4_OFFSET;
        pAbility->AsyFC = (phyData4 & AsymmetricPause_R4_MASK) >> AsymmetricPause_R4_OFFSET;

        if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_1000_BASET_CONTROL_REG, &phyData9)) != RT_ERR_OK)
            return ret;

        pAbility->Full_100= (phyData4 & _100Base_TX_FD_R4_MASK) >> _100Base_TX_FD_R4_OFFSET;
        pAbility->Half_100= (phyData4 & _100Base_TX_R4_MASK) >> _100Base_TX_R4_OFFSET;
        pAbility->Full_10= (phyData4 & _10Base_T_FD_R4_MASK) >> _10Base_T_FD_R4_OFFSET;
        pAbility->Half_10= (phyData4 & _10Base_T_R4_MASK) >> _10Base_T_R4_OFFSET;
        pAbility->Half_1000 = (phyData9 & _1000Base_THalfDuplex_MASK) >> _1000Base_THalfDuplex_OFFSET;
        pAbility->Full_1000 = (phyData9 & _1000Base_TFullDuplex_MASK) >> _1000Base_TFullDuplex_OFFSET;
    }

    if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_write(unit, port, PHY_PAGE_8, 30, restore_val)) != RT_ERR_OK)
        return ret;

    return ret;
} /* end of phy_8214fb_autoNegoAbility_get */

/* Function Name:
 *      phy_8214fb_autoNegoAbility_set
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
phy_8214fb_autoNegoAbility_set(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility)
{
    int32   ret;
    uint32  phyData0;
    uint32  phyData4;
    uint32  phyData9;
    uint32  phyData;
    uint32  restore_val, val;
    rtk_enable_t     enable;
    rtk_port_media_t media;
    hal_control_t   *pHalCtrl;
    uint32  phyExtStatus15, is_fiber_linkup = 0, is_copper_linkup = 0;

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_STATUS_REG, &val);
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_STATUS_REG, &val)) != RT_ERR_OK)
        return ret;
    if (val & LinkStatus_MASK)
    {
        if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, 15, &phyExtStatus15)) != RT_ERR_OK)
            return ret;
        is_fiber_linkup = (phyExtStatus15 >> 15) & 0x1;
        is_copper_linkup = (phyExtStatus15 >> 13) & 0x1;

        phy_8214fb_autoNegoEnable_get(unit, port, &enable);

        /* get value from internal status register */
        if ((ret = hal_miim_read(unit, port, PHY_PAGE_8, 23, &phyData)) != RT_ERR_OK)
            return ret;

        /* get value to CHIP*/
        if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_AN_ADVERTISEMENT_REG, &phyData4)) != RT_ERR_OK)
            return ret;

        if (is_fiber_linkup)
        {
            if ((phyData & 0x0188) == 0x0088)
            {   /* 100Base-FX Linkup */
                phyData4 = phyData4 & ~(Pause_R4_MASK | AsymmetricPause_R4_MASK);
                phyData4 = phyData4
                        | (pAbility->FC << Pause_R4_OFFSET)
                        | (pAbility->AsyFC << AsymmetricPause_R4_OFFSET);
            }
            else
            {   /* 1000Base-X Linkup */
                phyData4 = phyData4 & ~(_1000BaseX_Pause_R4_MASK | _1000BaseX_AsymmetricPause_R4_MASK);
                phyData4 = phyData4
                        | (pAbility->FC << _1000BaseX_Pause_R4_OFFSET)
                        | (pAbility->AsyFC << _1000BaseX_AsymmetricPause_R4_OFFSET);
                phyData4 = phyData4 & ~(_1000BaseX_HalfDuplex_R4_MASK | _1000BaseX_FullDuplex_R4_MASK);
                phyData4 = phyData4 | (pAbility->Half_1000 << _1000BaseX_HalfDuplex_R4_OFFSET)
                        | (pAbility->Full_1000 << _1000BaseX_FullDuplex_R4_OFFSET);
            }
            if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_AN_ADVERTISEMENT_REG, phyData4)) != RT_ERR_OK)
                return ret;
        }
        else if (is_copper_linkup)
        {
            phyData4 = phyData4 & ~(Pause_R4_MASK | AsymmetricPause_R4_MASK);
            phyData4 = phyData4
                    | (pAbility->FC << Pause_R4_OFFSET)
                    | (pAbility->AsyFC << AsymmetricPause_R4_OFFSET);

            if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_1000_BASET_CONTROL_REG, &phyData9)) != RT_ERR_OK)
                return ret;

            phyData4 = phyData4 &
                    ~(_100Base_TX_FD_R4_MASK | _100Base_TX_R4_MASK | _10Base_T_FD_R4_MASK | _10Base_T_R4_MASK);
            phyData4 = phyData4
                    | (pAbility->Full_100 << _100Base_TX_FD_R4_OFFSET)
                    | (pAbility->Half_100 << _100Base_TX_R4_OFFSET)
                    | (pAbility->Full_10 << _10Base_T_FD_R4_OFFSET)
                    | (pAbility->Half_10 << _10Base_T_R4_OFFSET);

            phyData9 = phyData9 & ~(_1000Base_TFullDuplex_MASK | _1000Base_THalfDuplex_MASK);
            phyData9 = phyData9 | (pAbility->Full_1000 << _1000Base_TFullDuplex_OFFSET)
                       | (pAbility->Half_1000 << _1000Base_THalfDuplex_OFFSET);

            if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_AN_ADVERTISEMENT_REG, phyData4)) != RT_ERR_OK)
                return ret;

            if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_1000_BASET_CONTROL_REG, phyData9)) != RT_ERR_OK)
                return ret;
        }

        /* Force re-autonegotiation if AN is on */
        if (ENABLED == enable)
        {
            if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &phyData0)) != RT_ERR_OK)
                return ret;

            phyData0 = phyData0 & ~(RestartAutoNegotiation_MASK);
            phyData0 = phyData0 | (enable << RestartAutoNegotiation_OFFSET);

            if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, phyData0)) != RT_ERR_OK)
                return ret;
        }
        return RT_ERR_OK;
    }

    /* Link-down */
    if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_read(unit, port, PHY_PAGE_8, 30, &restore_val)) != RT_ERR_OK)
        return ret;

    if ((restore_val & 0x2000) == 0x0000)
    {
        val = (restore_val | 0x2000);
        if (restore_val & 0x0020)
            val &= ~(0x1000);
        else
            val |= (0x1000);

        if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_write(unit, port, PHY_PAGE_8, 30, val)) != RT_ERR_OK)
            return ret;
        if (restore_val & 0x0020)
            media = PORT_MEDIA_COPPER;
        else
            media = PORT_MEDIA_FIBER;
    }
    else
    {
        if (restore_val & 0x1000)
            media = PORT_MEDIA_FIBER;
        else
            media = PORT_MEDIA_COPPER;
    }

    phy_8214fb_autoNegoEnable_get(unit, port, &enable);

    /* get value from CHIP*/
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_AN_ADVERTISEMENT_REG, &phyData4)) != RT_ERR_OK)
        return ret;

    if (PORT_MEDIA_FIBER == media)
    {
        if ((restore_val & 0x0c00) == 0x0c00)
        {   /* 100-fx */
            phyData4 = phyData4 & ~(Pause_R4_MASK | AsymmetricPause_R4_MASK);
            phyData4 = phyData4
                    | (pAbility->FC << Pause_R4_OFFSET)
                    | (pAbility->AsyFC << AsymmetricPause_R4_OFFSET);
        }
        else
        {   /* 1000-x */
            phyData4 = phyData4 & ~(_1000BaseX_Pause_R4_MASK | _1000BaseX_AsymmetricPause_R4_MASK);
            phyData4 = phyData4
                    | (pAbility->FC << _1000BaseX_Pause_R4_OFFSET)
                    | (pAbility->AsyFC << _1000BaseX_AsymmetricPause_R4_OFFSET);
            phyData4 = phyData4 & ~(_1000BaseX_HalfDuplex_R4_MASK | _1000BaseX_FullDuplex_R4_MASK);
            phyData4 = phyData4 | (pAbility->Half_1000 << _1000BaseX_HalfDuplex_R4_OFFSET)
                    | (pAbility->Full_1000 << _1000BaseX_FullDuplex_R4_OFFSET);
        }
        if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_AN_ADVERTISEMENT_REG, phyData4)) != RT_ERR_OK)
            return ret;
    }
    else if (PORT_MEDIA_COPPER == media)
    {
        phyData4 = phyData4 & ~(Pause_R4_MASK | AsymmetricPause_R4_MASK);
        phyData4 = phyData4
                | (pAbility->FC << Pause_R4_OFFSET)
                | (pAbility->AsyFC << AsymmetricPause_R4_OFFSET);

        if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_1000_BASET_CONTROL_REG, &phyData9)) != RT_ERR_OK)
            return ret;

        phyData4 = phyData4 &
                ~(_100Base_TX_FD_R4_MASK | _100Base_TX_R4_MASK | _10Base_T_FD_R4_MASK | _10Base_T_R4_MASK);
        phyData4 = phyData4
                | (pAbility->Full_100 << _100Base_TX_FD_R4_OFFSET)
                | (pAbility->Half_100 << _100Base_TX_R4_OFFSET)
                | (pAbility->Full_10 << _10Base_T_FD_R4_OFFSET)
                | (pAbility->Half_10 << _10Base_T_R4_OFFSET);

        phyData9 = phyData9 & ~(_1000Base_THalfDuplex_MASK | _1000Base_TFullDuplex_MASK);
        phyData9 = phyData9 | (pAbility->Full_1000 << _1000Base_TFullDuplex_OFFSET)
                   | (pAbility->Half_1000 << _1000Base_THalfDuplex_OFFSET);

        if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_AN_ADVERTISEMENT_REG, phyData4)) != RT_ERR_OK)
            return ret;

        if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_1000_BASET_CONTROL_REG, phyData9)) != RT_ERR_OK)
            return ret;
    }

    /* Force re-autonegotiation if AN is on*/
    if (ENABLED == enable)
    {
        if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &phyData0)) != RT_ERR_OK)
            return ret;

        phyData0 = phyData0 & ~(RestartAutoNegotiation_MASK);
        phyData0 = phyData0 | (enable << RestartAutoNegotiation_OFFSET);

        if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, phyData0)) != RT_ERR_OK)
            return ret;
    }

    if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_write(unit, port, PHY_PAGE_8, 30, restore_val)) != RT_ERR_OK)
        return ret;
    return RT_ERR_OK;
} /* end of phy_8214fb_autoNegoAbility_set */


/* Function Name:
 *      phy_8214fb_speed_get
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
phy_8214fb_speed_get(uint32 unit, rtk_port_t port, rtk_port_speed_t *pSpeed)
{
    int32   ret;
    uint32  phyData0;
    uint32  restore_val, val;
    hal_control_t   *pHalCtrl;

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_STATUS_REG, &val);
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_STATUS_REG, &val)) != RT_ERR_OK)
        return ret;
    if (val & LinkStatus_MASK)
    {
        /* get value from CHIP*/
        if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &phyData0)) != RT_ERR_OK)
            return ret;

        *pSpeed = ((phyData0 & SpeedSelection1_MASK) >> (SpeedSelection1_OFFSET -1))
                  | ((phyData0 & SpeedSelection0_MASK) >> SpeedSelection0_OFFSET);
        return RT_ERR_OK;
    }

    /* Link-down */
    if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_read(unit, port, PHY_PAGE_8, 30, &restore_val)) != RT_ERR_OK)
        return ret;

    if ((restore_val & 0x2000) == 0x0000)
    {
        val = (restore_val | 0x2000);
        if (restore_val & 0x0020)
            val &= ~(0x1000);
        else
            val |= (0x1000);

        if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_write(unit, port, PHY_PAGE_8, 30, val)) != RT_ERR_OK)
            return ret;
    }

    /* get value from CHIP*/
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &phyData0)) != RT_ERR_OK)
        return ret;

    *pSpeed = ((phyData0 & SpeedSelection1_MASK) >> (SpeedSelection1_OFFSET -1))
              | ((phyData0 & SpeedSelection0_MASK) >> SpeedSelection0_OFFSET);

    if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_write(unit, port, PHY_PAGE_8, 30, restore_val)) != RT_ERR_OK)
        return ret;
    return ret;
} /* end of phy_8214fb_speed_get */


/* Function Name:
 *      phy_8214fb_speed_set
 * Description:
 *      Set speed mode status of the specific port
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      speed         - link speed status 10/100/1000
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8214fb_speed_set(uint32 unit, rtk_port_t port, rtk_port_speed_t speed)
{
    int32   ret;
    uint32  phyData0;
    rtk_port_media_t media;
    uint32  restore_val, val;
    hal_control_t   *pHalCtrl;
    uint32  phyExtStatus15, is_fiber_linkup = 0;

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_STATUS_REG, &val);
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_STATUS_REG, &val)) != RT_ERR_OK)
        return ret;
    if (val & LinkStatus_MASK)
    {
        if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, 15, &phyExtStatus15)) != RT_ERR_OK)
            return ret;
        is_fiber_linkup = (phyExtStatus15 >> 15) & 0x1;

        if (is_fiber_linkup)
        {
            if ((ret = hal_miim_read(unit, port, PHY_PAGE_8, 30, &restore_val)) != RT_ERR_OK)
                return ret;
            val = restore_val & (~(3<<10));
            if (speed == PORT_SPEED_1000M)
            {
                val |= (2<<10);
            }
            else if (speed == PORT_SPEED_100M)
            {
                val |= (3<<10);
            }
            if ((ret = hal_miim_write(unit, port, PHY_PAGE_8, 30, val)) != RT_ERR_OK)
                return ret;
        }

        /* set value to CHIP*/
        if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &phyData0)) != RT_ERR_OK)
            return ret;

        phyData0 = phyData0 & ~(SpeedSelection1_MASK | SpeedSelection0_MASK);
        phyData0 = phyData0 | (((speed & 2) << (SpeedSelection1_OFFSET - 1)) | ((speed & 1) << SpeedSelection0_OFFSET));

        if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, phyData0)) != RT_ERR_OK)
            return ret;
        return RT_ERR_OK;
    }

    /* Link-down */
    if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_read(unit, port, PHY_PAGE_8, 30, &restore_val)) != RT_ERR_OK)
        return ret;

    if ((restore_val & 0x2000) == 0x0000)
    {
        val = (restore_val | 0x2000);
        if (restore_val & 0x0020)
            val &= ~(0x1000);
        else
            val |= (0x1000);

        if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_write(unit, port, PHY_PAGE_8, 30, val)) != RT_ERR_OK)
            return ret;
        if (restore_val & 0x0020)
            media = PORT_MEDIA_COPPER;
        else
            media = PORT_MEDIA_FIBER;
    }
    else
    {
        if (restore_val & 0x1000)
            media = PORT_MEDIA_FIBER;
        else
            media = PORT_MEDIA_COPPER;
    }

    if (PORT_MEDIA_FIBER == media)
    {
        if ((ret = hal_miim_read(unit, port, PHY_PAGE_8, 30, &val)) != RT_ERR_OK)
            return ret;
        val &= ~(3<<10);
        restore_val &= ~(3<<10);
        if (speed == PORT_SPEED_1000M)
        {
            val |= (2<<10);
            restore_val |= (2<<10);
        }
        else if (speed == PORT_SPEED_100M)
        {
            val |= (3<<10);
            restore_val |= (3<<10);
        }
        if ((ret = hal_miim_write(unit, port, PHY_PAGE_8, 30, val)) != RT_ERR_OK)
            return ret;
    }

    /* get value from CHIP*/
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &phyData0)) != RT_ERR_OK)
        return ret;

    phyData0 = phyData0 & ~(SpeedSelection1_MASK | SpeedSelection0_MASK);
    phyData0 = phyData0 | (((speed & 2) << (SpeedSelection1_OFFSET - 1)) | ((speed & 1) << SpeedSelection0_OFFSET));

    if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, phyData0)) != RT_ERR_OK)
        return ret;

    if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_write(unit, port, PHY_PAGE_8, 30, restore_val)) != RT_ERR_OK)
        return ret;
    return ret;
} /* end of phy_8214fb_speed_set */

/* Function Name:
 *      phy_8214fb_rtctResult_get
 * Description:
 *      Get test result of RTCT.
 * Input:
 *      unit        - unit id
 *      port        - the port for retriving RTCT test result
 * Output:
 *      pRtctResult - RTCT result
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_PHY_RTCT_NOT_FINISH   - RTCT not finish. Need to wait a while.
 *      RT_ERR_TIMEOUT      - RTCT test timeout in this port.
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
phy_8214fb_rtctResult_get(uint32 unit, rtk_port_t port, rtk_rtctResult_t *pRtctResult)
{
    int32   ret = RT_ERR_FAILED;
    uint32  phyData;
    rtk_port_media_t  media;
    uint32  speed;

    if ((ret = phy_8214fb_media_get(unit, port, &media)) != RT_ERR_OK)
        return ret;

    if (media == PORT_MEDIA_COPPER)
    {
        /* Check the port is link up or not? */
        ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_STATUS_REG, &phyData);
        if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_STATUS_REG, &phyData)) != RT_ERR_OK)
            return ret;

        phy_common_speed_get(unit, port, &speed);

        if ((phyData & LinkStatus_MASK) && speed != PORT_SPEED_10M)
        {
            /* If the port is link up,
             * return cable length from green function
             */
            if ((ret = hal_miim_read(unit, port, 1, 4, &phyData)) != RT_ERR_OK)
                return ret;
            if ((phyData & 0x1) == 0x0000)
                return RT_ERR_PHY_RTCT_NOT_FINISH;
            if ((ret = hal_miim_read(unit, port, 1, 30, &phyData)) != RT_ERR_OK)
                return ret;

            osal_memset(pRtctResult, 0, sizeof(rtk_rtctResult_t));
            if (speed == PORT_SPEED_1000M) /* 1000M */
            {
                pRtctResult->linkType = PORT_SPEED_1000M;
                pRtctResult->un.ge_result.channelALen = (phyData & 0x00FF)*100;
                pRtctResult->un.ge_result.channelBLen = (phyData & 0x00FF)*100;
                pRtctResult->un.ge_result.channelCLen = (phyData & 0x00FF)*100;
                pRtctResult->un.ge_result.channelDLen = (phyData & 0x00FF)*100;
            }
            else /* 100M */
            {
                pRtctResult->linkType = PORT_SPEED_100M;
                pRtctResult->un.fe_result.rxLen = (phyData & 0x00FF)*100;
                pRtctResult->un.fe_result.txLen = (phyData & 0x00FF)*100;
            }
        }
        else
        {
            if ((ret = hal_miim_read(unit, port, 11, 27, &phyData)) != RT_ERR_OK)
                return ret;

            if (0 == (phyData & 0x4000))
                return RT_ERR_PHY_RTCT_NOT_FINISH;

            pRtctResult->linkType = PORT_SPEED_1000M;
            /* Length = (Index/64)*8ns*(0.2m/ns) = Index/40 (m) = (2.5) * Index (cm) */
            pRtctResult->un.ge_result.channelALen = (phyData & 0x1FFF)*5/2;

            if ((ret = hal_miim_read(unit, port, 11, 26, &phyData)) != RT_ERR_OK)
                return ret;
            pRtctResult->un.ge_result.channelAShort = phyData & 0x1000;
            pRtctResult->un.ge_result.channelBShort = phyData & 0x2000;
            pRtctResult->un.ge_result.channelCShort = phyData & 0x4000;
            pRtctResult->un.ge_result.channelDShort = phyData & 0x8000;
            pRtctResult->un.ge_result.channelAOpen  = phyData & 0x0100;
            pRtctResult->un.ge_result.channelBOpen  = phyData & 0x0200;
            pRtctResult->un.ge_result.channelCOpen  = phyData & 0x0400;
            pRtctResult->un.ge_result.channelDOpen  = phyData & 0x0800;
            pRtctResult->un.ge_result.channelAMismatch  = phyData & 0x0010;
            pRtctResult->un.ge_result.channelBMismatch  = phyData & 0x0020;
            pRtctResult->un.ge_result.channelCMismatch  = phyData & 0x0040;
            pRtctResult->un.ge_result.channelDMismatch  = phyData & 0x0080;
            pRtctResult->un.ge_result.channelALinedriver  = phyData & 0x0001;
            pRtctResult->un.ge_result.channelBLinedriver  = phyData & 0x0002;
            pRtctResult->un.ge_result.channelCLinedriver  = phyData & 0x0004;
            pRtctResult->un.ge_result.channelDLinedriver  = phyData & 0x0008;

            if ((ret = hal_miim_read(unit, port, 11, 28, &phyData)) != RT_ERR_OK)
                return ret;
            pRtctResult->un.ge_result.channelBLen = (phyData & 0x1FFF)*5/2;

            if ((ret = hal_miim_read(unit, port, 11, 29, &phyData)) != RT_ERR_OK)
                return ret;
            pRtctResult->un.ge_result.channelCLen = (phyData & 0x1FFF)*5/2;

            if ((ret = hal_miim_read(unit, port, 11, 30, &phyData)) != RT_ERR_OK)
                return ret;
            pRtctResult->un.ge_result.channelDLen = (phyData & 0x1FFF)*5/2;
        }
    }
    else
    {
        /* RTCT function is not supoprted in fiber media */
        return RT_ERR_PHY_MEDIA;
    }

    return ret;
} /* end of phy_8214fb_rtctResult_get */

/* Function Name:
 *      phy_8214fb_rtct_start
 * Description:
 *      Start PHY interface RTCT test of the specific port
 * Input:
 *      unit          - unit id
 *      port          - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 *      RT_ERR_PHY_MEDIA - media error
 * Note:
 *      None
 */
int32
phy_8214fb_rtct_start(uint32 unit, rtk_port_t port)
{
    int32   ret = RT_ERR_FAILED;
    uint32  phyData, phyData21, i;
    rtk_port_media_t  media;
    uint32  speed;

    if ((ret = phy_8214fb_media_get(unit, port, &media)) != RT_ERR_OK)
        return ret;

    if (media == PORT_MEDIA_COPPER)
    {
        /* Check the port is link up or not? */
        ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_STATUS_REG, &phyData);
        if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_STATUS_REG, &phyData)) != RT_ERR_OK)
            return ret;

        phy_common_speed_get(unit, port, &speed);

        if ((phyData & LinkStatus_MASK) && speed != PORT_SPEED_10M)
        {
            /* If the port is link up,
             * enable the green function to know the cable length
             */
            if ((ret = hal_miim_read(unit, port, 1, 4, &phyData)) != RT_ERR_OK)
            {
                RT_LOG(LOG_EVENT, MOD_HAL, "hal_miim_read(unit %d, port %d) page 1 reg 4 value 0x%x!!", unit, port, phyData);
                return ret;
            }
            phyData |= 0x1;
            if ((ret = hal_miim_write(unit, port, 1, 4, phyData)) != RT_ERR_OK)
            {
                RT_LOG(LOG_EVENT, MOD_HAL, "hal_miim_write(unit %d, port %d) page 1 reg 4 value 0x%x!!", unit, port, phyData);
                return ret;
            }
        }
        else
        {
            /* Disable Power Saving Mode First */
            if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, 21, &phyData21)) != RT_ERR_OK)
                return ret;
            phyData = phyData21 & ~(0x1000);
            if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, 21, phyData)) != RT_ERR_OK)
                return ret;
            osal_time_udelay(100 * 1000);
            /* Change to ext. page 47 for force giga power */
            if ((ret = hal_miim_write(unit, port, 7, 30, 0x002F)) != RT_ERR_OK)
                return ret;
            if ((ret = hal_miim_write(unit, port, 7, 23, 0xD820)) != RT_ERR_OK)
                return ret;
            /* Page 11, PHY 0-3, Register 25 (Per-port RTCT Configuration Register)
             * bit[15:15]: Enable port 0-3 cable test. When enable port x RTCT,
             *             RTL8214 will scan port x cable status from channel A
             *             to channel D sequentially.
             *             1: Enable
             *             0: Disable
             */
            if ((ret = hal_miim_read(unit, port, 11, 25, &phyData)) != RT_ERR_OK)
                return ret;

            phyData &= ~(0x8000);
            if ((ret = hal_miim_write(unit, port, 11, 25, phyData)) != RT_ERR_OK)
                return ret;

            osal_time_udelay(100 * 1000);
            phyData |= (0x8000);
            if ((ret = hal_miim_write(unit, port, 11, 25, phyData)) != RT_ERR_OK)
                return ret;

            for (i = 0; i < RTCT_CHECKBUSY_TIMES; i++)
            {
                osal_time_udelay(500 * 1000);
                if ((ret = hal_miim_read(unit, port, 11, 27, &phyData)) != RT_ERR_OK)
                    return ret;
                if (phyData & 0x4000)
                    break;
            }

            /* Change to ext. page 47 for normal power */
            if ((ret = hal_miim_write(unit, port, 7, 30, 0x002F)) != RT_ERR_OK)
                return ret;
            if ((ret = hal_miim_write(unit, port, 7, 23, 0xD88F)) != RT_ERR_OK)
                return ret;
            /* Restore Power Saving Mode */
            if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, 21, phyData21)) != RT_ERR_OK)
                return ret;

            if (RTCT_CHECKBUSY_TIMES == i)
                return RT_ERR_PHY_RTCT_TIMEOUT;
        }
    }
    else
    {
        /* RTCT function is not supoprted in fiber media */
        return RT_ERR_PHY_MEDIA;
    }

    return ret;
} /* end of phy_8214fb_rtct_start */

/* Function Name:
 *      phy_8214fb_eeeEnable_get
 * Description:
 *      Get enable status of EEE function in the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to enable status of EEE
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
phy_8214fb_eeeEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  phyData;

    /* get value from CHIP*/
    if ((ret = hal_miim_write(unit, port, 7, 0x1e, 0x0020)) != RT_ERR_OK)
        return ret;

    if ((ret = hal_miim_read(unit, port, 7, 0x15, &phyData)) != RT_ERR_OK)
        return ret;

    if ((phyData >> 8) & 0x1)
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;

    return ret;
} /* end of phy_8214fb_eeeEnable_get */

/* Function Name:
 *      phy_8214fb_eeeEnable_set
 * Description:
 *      Set enable status of EEE function in the specified port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - enable status of EEE
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
phy_8214fb_eeeEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    uint32  phyData0;
    int32   ret;
    rtk_enable_t    an_enable;
    uint32  phyData, restore_phyData;
    uint32  base_port, val, phyExtStatus15;
    uint32  is_copper_linkup, is_phyDown;

    base_port = port - (port % PORT_NUM_IN_8214FB);

    phy_8214fb_autoNegoEnable_get(unit, port, &an_enable);

    ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_STATUS_REG, &val);
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_STATUS_REG, &val)) != RT_ERR_OK)
        return ret;

    if (val & LinkStatus_MASK)
    {
        if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, 15, &phyExtStatus15)) != RT_ERR_OK)
            return ret;
        is_copper_linkup = (phyExtStatus15 >> 13) & 0x1;

        if (is_copper_linkup)
        {

        }
        else
        {
            /* eee register is in copper page and can't access when fiber is linkup
             * If access via page 8 register 16 with force access copper PHY, it will cause
             * the fiber link down.
             */
            return RT_ERR_PHY_FIBER_LINKUP;
        }
    }

    /* linkdown or is_copper_linkup */
    if ((ret = hal_miim_read(unit, base_port+1, 8, 16, &restore_phyData)) != RT_ERR_OK)
        return ret;

    phyData = restore_phyData & ~(1<<(12+port-base_port));
    phyData |= (1<<(8+port-base_port));

    if ((ret = hal_miim_write(unit, base_port+1, 8, 16, phyData)) != RT_ERR_OK)
        return ret;

    /* Power Down PHY */
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &phyData0)) != RT_ERR_OK)
        return ret;
    is_phyDown = (phyData0 >> 11) & 0x1;
    phyData0 |= (1 << 11);
    if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, phyData0)) != RT_ERR_OK)
        return ret;

    if (ENABLED == enable)
    {
        /* Micro-C Enable or Disable Auto Turn off EEE */
        if ((ret = hal_miim_write(unit, port, 5, 0x05, 0x8B85)) != RT_ERR_OK)
            return ret;
        if ((ret = hal_miim_write(unit, port, 5, 0x06, 0xE286)) != RT_ERR_OK)
            return ret;
        if ((ret = hal_miim_write(unit, port, 8, 0x1f, 0x0008)) != RT_ERR_OK)
            return ret;

        /* Micro-C Control 10M EEE */
        if ((ret = hal_miim_write(unit, port, 5, 0x05, 0x8B86)) != RT_ERR_OK)
            return ret;
        if ((ret = hal_miim_write(unit, port, 5, 0x06, 0x8600)) != RT_ERR_OK)
            return ret;
        if ((ret = hal_miim_write(unit, port, 8, 0x1f, 0x0008)) != RT_ERR_OK)
            return ret;

        /* Enable or Disable EEE */
        if ((ret = hal_miim_write(unit, port, 7, 0x1e, 0x0020)) != RT_ERR_OK)
            return ret;
        if ((ret = hal_miim_write(unit, port, 7, 0x15, 0x0100)) != RT_ERR_OK)
            return ret;
        if ((ret = hal_miim_write(unit, port, 7, 0x1b, 0xA03A)) != RT_ERR_OK)
            return ret;
        if ((ret = hal_miim_write(unit, port, 8, 0x1f, 0x0008)) != RT_ERR_OK)
            return ret;

        /* 100/1000M EEE Capability */
        if ((ret = hal_miim_write(unit, port, 0, 0x0d, 0x0007)) != RT_ERR_OK)
            return ret;
        if ((ret = hal_miim_write(unit, port, 0, 0x0e, 0x003C)) != RT_ERR_OK)
            return ret;
        if ((ret = hal_miim_write(unit, port, 0, 0x0d, 0x4007)) != RT_ERR_OK)
            return ret;
        if ((ret = hal_miim_write(unit, port, 0, 0x0e, 0x0006)) != RT_ERR_OK)
            return ret;
        if ((ret = hal_miim_write(unit, port, 0, 0x0d, 0x0000)) != RT_ERR_OK)
            return ret;
        if ((ret = hal_miim_write(unit, port, 8, 0x1f, 0x0008)) != RT_ERR_OK)
            return ret;

        /* 10M EEE Amplitude */
        if ((ret = hal_miim_write(unit, port, 2, 0x0b, 0x17A7)) != RT_ERR_OK)
            return ret;
        if ((ret = hal_miim_write(unit, port, 8, 0x1f, 0x0008)) != RT_ERR_OK)
            return ret;
    }
    else
    {
        /* Micro-C Enable or Disable Auto Turn off EEE */
        if ((ret = hal_miim_write(unit, port, 5, 0x05, 0x8B85)) != RT_ERR_OK)
            return ret;
        if ((ret = hal_miim_write(unit, port, 5, 0x06, 0xC286)) != RT_ERR_OK)
            return ret;
        if ((ret = hal_miim_write(unit, port, 8, 0x1f, 0x0008)) != RT_ERR_OK)
            return ret;

        /* Micro-C Control 10M EEE */
        if ((ret = hal_miim_write(unit, port, 5, 0x05, 0x8B86)) != RT_ERR_OK)
            return ret;
        if ((ret = hal_miim_write(unit, port, 5, 0x06, 0x8600)) != RT_ERR_OK)
            return ret;
        if ((ret = hal_miim_write(unit, port, 8, 0x1f, 0x0008)) != RT_ERR_OK)
            return ret;

        /* Enable or Disable EEE */
        if ((ret = hal_miim_write(unit, port, 7, 0x1e, 0x0020)) != RT_ERR_OK)
            return ret;
        if ((ret = hal_miim_write(unit, port, 7, 0x15, 0x0000)) != RT_ERR_OK)
            return ret;
        if ((ret = hal_miim_write(unit, port, 7, 0x1b, 0xA03A)) != RT_ERR_OK)
            return ret;
        if ((ret = hal_miim_write(unit, port, 8, 0x1f, 0x0008)) != RT_ERR_OK)
            return ret;

        /* 100/1000M EEE Capability */
        if ((ret = hal_miim_write(unit, port, 0, 0x0d, 0x0007)) != RT_ERR_OK)
            return ret;
        if ((ret = hal_miim_write(unit, port, 0, 0x0e, 0x003C)) != RT_ERR_OK)
            return ret;
        if ((ret = hal_miim_write(unit, port, 0, 0x0d, 0x4007)) != RT_ERR_OK)
            return ret;
        if ((ret = hal_miim_write(unit, port, 0, 0x0e, 0x0000)) != RT_ERR_OK)
            return ret;
        if ((ret = hal_miim_write(unit, port, 0, 0x0d, 0x0000)) != RT_ERR_OK)
            return ret;
        if ((ret = hal_miim_write(unit, port, 8, 0x1f, 0x0008)) != RT_ERR_OK)
            return ret;

        /* 10M EEE Amplitude */
        if ((ret = hal_miim_write(unit, port, 2, 0x0b, 0x17A7)) != RT_ERR_OK)
            return ret;
        if ((ret = hal_miim_write(unit, port, 8, 0x1f, 0x0008)) != RT_ERR_OK)
            return ret;
    }

    /* Force re-autonegotiation if AN is on */
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &phyData0)) != RT_ERR_OK)
        return ret;
    if (ENABLED == an_enable)
    {
        phyData0 = phyData0 & ~(RestartAutoNegotiation_MASK);
        phyData0 = phyData0 | (an_enable << RestartAutoNegotiation_OFFSET);
    }

    if (!is_phyDown)
        phyData0 &= ~(1<<11);
    if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, phyData0)) != RT_ERR_OK)
        return ret;

    /* restore PHY 1, page 8, register 16 */
    if ((ret = hal_miim_write(unit, base_port+1, 8, 16, restore_phyData)) != RT_ERR_OK)
        return ret;

    osal_time_usleep(1000 * 1000); /* delay 1000mS */

    return ret;
} /* end of phy_8214fb_eeeEnable_set */

/* Function Name:
 *      phy_8214fb_autoNegoEnable_get
 * Description:
 *      Get autonegotiation enable status of the specific port
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
phy_8214fb_autoNegoEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  phyData0;
    uint32  restore_val, val;
    hal_control_t   *pHalCtrl;

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_STATUS_REG, &val);
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_STATUS_REG, &val)) != RT_ERR_OK)
        return ret;
    if (val & LinkStatus_MASK)
    {
        /* get value from CHIP*/
        if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &phyData0)) != RT_ERR_OK)
            return ret;

        if (phyData0 & AutoNegotiationEnable_MASK)
            *pEnable = ENABLED;
        else
            *pEnable = DISABLED;
        return RT_ERR_OK;
    }

    /* Link-down */
    if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_read(unit, port, PHY_PAGE_8, 30, &restore_val)) != RT_ERR_OK)
        return ret;

    if ((restore_val & 0x2000) == 0x0000)
    {
        val = (restore_val | 0x2000);
        if (restore_val & 0x0020)
            val &= ~(0x1000);
        else
            val |= (0x1000);

        if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_write(unit, port, PHY_PAGE_8, 30, val)) != RT_ERR_OK)
            return ret;
    }

    /* get value from CHIP*/
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &phyData0)) != RT_ERR_OK)
        return ret;

    if (phyData0 & AutoNegotiationEnable_MASK)
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;

    if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_write(unit, port, PHY_PAGE_8, 30, restore_val)) != RT_ERR_OK)
        return ret;
    return RT_ERR_OK;
} /* end of phy_8214fb_autoNegoEnable_get */

/* Function Name:
 *      phy_8214fb_autoNegoEnable_set
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
phy_8214fb_autoNegoEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
    uint32  phyData0;
    uint32  restore_val, val;
    uint32  phyExtStatus15;
    rtk_port_media_t media;
    hal_control_t   *pHalCtrl;

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_STATUS_REG, &val);
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_STATUS_REG, &val)) != RT_ERR_OK)
        return ret;

    if (val & LinkStatus_MASK)
    {   /* Link-up */
        if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, 15, &phyExtStatus15)) != RT_ERR_OK)
            return ret;

        /* configure the Nway and Restart-Nway bit to PHY chip */
        if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &phyData0)) != RT_ERR_OK)
            return ret;

        phyData0 = phyData0 & ~(AutoNegotiationEnable_MASK | RestartAutoNegotiation_MASK);
        phyData0 = phyData0 | ((enable << AutoNegotiationEnable_OFFSET) | (1 << RestartAutoNegotiation_OFFSET));

        if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, phyData0)) != RT_ERR_OK)
            return ret;

        /*Remove : No need to set the fiber media mode to FIBER-AUTO*/
//        if (is_fiber_linkup && (ENABLED == enable))
//        {
//            if ((ret = phy_8214fb_fiber_media_set(unit, port, PORT_FIBER_MEDIA_AUTO)) != RT_ERR_OK)
//                return ret;
//        }
        return RT_ERR_OK;
    }

    /* Link-down */
    if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_read(unit, port, PHY_PAGE_8, 30, &restore_val)) != RT_ERR_OK)
        return ret;

    if ((restore_val & 0x2000) == 0x0000)
    {
        val = (restore_val | 0x2000);
        if (restore_val & 0x0020)
            val &= ~(0x1000);
        else
            val |= (0x1000);

        if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_write(unit, port, PHY_PAGE_8, 30, val)) != RT_ERR_OK)
            return ret;
        if (restore_val & 0x0020)
            media = PORT_MEDIA_COPPER;
        else
            media = PORT_MEDIA_FIBER;
    }
    else
    {
        if (restore_val & 0x1000)
            media = PORT_MEDIA_FIBER;
        else
            media = PORT_MEDIA_COPPER;
    }


    /* configure the Nway and Restart-Nway bit to PHY chip */
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &phyData0)) != RT_ERR_OK)
        return ret;
    phyData0 = phyData0 & ~(AutoNegotiationEnable_MASK | RestartAutoNegotiation_MASK);
    phyData0 = phyData0 | ((enable << AutoNegotiationEnable_OFFSET) | (1 << RestartAutoNegotiation_OFFSET));
    if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, phyData0)) != RT_ERR_OK)
        return ret;

    if ((PORT_MEDIA_FIBER == media) && (ENABLED == enable))
    {
        if ((ret = phy_8214fb_fiber_media_set(unit, port, PORT_FIBER_MEDIA_AUTO)) != RT_ERR_OK)
            return ret;
       /*Remove : No need to set the fiber media mode to FIBER-AUTO*/
//        restore_val = restore_val & 0xf3ff;
    }

    if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_write(unit, port, PHY_PAGE_8, 30, restore_val)) != RT_ERR_OK)
        return ret;
    return RT_ERR_OK;
} /* end of phy_8214fb_autoNegoEnable_set */

/* Function Name:
 *      phy_8214fb_duplex_get
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
phy_8214fb_duplex_get(uint32 unit, rtk_port_t port, uint32 *pDuplex)
{
    int32   ret;
    uint32  phyData0;
    uint32  restore_val, val;
    hal_control_t   *pHalCtrl;

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_STATUS_REG, &val);
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_STATUS_REG, &val)) != RT_ERR_OK)
        return ret;
    if (val & LinkStatus_MASK)
    {
        /* get value from CHIP*/
        if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &phyData0)) != RT_ERR_OK)
            return ret;

        *pDuplex = (phyData0 & DuplexMode_MASK) >> DuplexMode_OFFSET;
        return RT_ERR_OK;
    }

    /* Link-down */
    if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_read(unit, port, PHY_PAGE_8, 30, &restore_val)) != RT_ERR_OK)
        return ret;

    if ((restore_val & 0x2000) == 0x0000)
    {
        val = (restore_val | 0x2000);
        if (restore_val & 0x0020)
            val &= ~(0x1000);
        else
            val |= (0x1000);

        if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_write(unit, port, PHY_PAGE_8, 30, val)) != RT_ERR_OK)
            return ret;
    }

    /* get value from CHIP*/
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &phyData0)) != RT_ERR_OK)
        return ret;

    *pDuplex = (phyData0 & DuplexMode_MASK) >> DuplexMode_OFFSET;

    if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_write(unit, port, PHY_PAGE_8, 30, restore_val)) != RT_ERR_OK)
        return ret;
    return ret;
} /* end of phy_8214fb_duplex_get */

/* Function Name:
 *      phy_8214fb_duplex_set
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
phy_8214fb_duplex_set(uint32 unit, rtk_port_t port, uint32 duplex)
{
    int32   ret;
    uint32  phyData0;
    uint32  restore_val, val;
    hal_control_t   *pHalCtrl;

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_STATUS_REG, &val);
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_STATUS_REG, &val)) != RT_ERR_OK)
        return ret;
    if (val & LinkStatus_MASK)
    {
        /* set value to CHIP*/
        if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &phyData0)) != RT_ERR_OK)
            return ret;

        phyData0 = phyData0 & ~(DuplexMode_MASK);
        phyData0 = phyData0 | (duplex << DuplexMode_OFFSET);

        if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, phyData0)) != RT_ERR_OK)
            return ret;
        return RT_ERR_OK;
    }

    /* Link-down */
    if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_read(unit, port, PHY_PAGE_8, 30, &restore_val)) != RT_ERR_OK)
        return ret;

    if ((restore_val & 0x2000) == 0x0000)
    {
        val = (restore_val | 0x2000);
        if (restore_val & 0x0020)
            val &= ~(0x1000);
        else
            val |= (0x1000);

        if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_write(unit, port, PHY_PAGE_8, 30, val)) != RT_ERR_OK)
            return ret;
    }

    /* get value from CHIP*/
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &phyData0)) != RT_ERR_OK)
        return ret;

    phyData0 = phyData0 & ~(DuplexMode_MASK);
    phyData0 = phyData0 | (duplex << DuplexMode_OFFSET);

    if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, phyData0)) != RT_ERR_OK)
        return ret;

    if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_write(unit, port, PHY_PAGE_8, 30, restore_val)) != RT_ERR_OK)
        return ret;
    return ret;
} /* end of phy_8214fb_duplex_set */

/* Function Name:
 *      phy_8214fb_crossOverMode_get
 * Description:
 *      Get cross over mode in the specified port.
 * Input:
 *      unit  - unit id
 *      port  - port id
 * Output:
 *      pMode - pointer to cross over mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Following value is valid
 *      - PORT_CROSSOVER_MODE_AUTO
 *      - PORT_CROSSOVER_MODE_MDI
 *      - PORT_CROSSOVER_MODE_MDIX
 */
int32
phy_8214fb_crossOverMode_get(uint32 unit, rtk_port_t port, rtk_port_crossOver_mode_t *pMode)
{
    int32   ret;
    uint32  phyData, force_mode, mdi;
    uint32  restore_phyData, base_port, val, phyExtStatus15, cross_config_sts;
    uint32  is_copper_linkup;

    base_port = port - (port % PORT_NUM_IN_8214FB);
    ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_STATUS_REG, &val);
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_STATUS_REG, &val)) != RT_ERR_OK)
        return ret;
    if (val & LinkStatus_MASK)
    {
        if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, 15, &phyExtStatus15)) != RT_ERR_OK)
            return ret;
        is_copper_linkup = (phyExtStatus15 >> 13) & 0x1;

        if (is_copper_linkup)
        {
            /* get value from CHIP*/
            if ((ret = hal_miim_write(unit, port, 7, 0x1e, 0x002d)) != RT_ERR_OK)
                return ret;

            if ((ret = hal_miim_read(unit, port, 7, 0x18, &phyData)) != RT_ERR_OK)
                return ret;
            force_mode = (phyData >> 5) & 0x1;

            if ((ret = hal_miim_read(unit, port, 0, 0x10, &phyData)) != RT_ERR_OK)
                return ret;
            mdi = (phyData >> 5) & 0x1;

        if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, 31, 0x7)) != RT_ERR_OK)
        return ret;
        if ((ret = hal_miim_write(unit, port, 0x7, 30, 0x24)) != RT_ERR_OK)
        return ret;
        if ((ret = hal_miim_read(unit, port, 0x7, 26, &cross_config_sts)) != RT_ERR_OK)
        return ret;
        if((cross_config_sts & 0x1) == 1) /*Check uc_use_force_nxc bit*/
        force_mode = 0;

        if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, 31, 0x0)) != RT_ERR_OK)
        return ret;

            if (force_mode)
            {
                if (mdi)
                    *pMode = PORT_CROSSOVER_MODE_MDI;
                else
                    *pMode = PORT_CROSSOVER_MODE_MDIX;
            }
            else
                *pMode = PORT_CROSSOVER_MODE_AUTO;
            return ret;
        }
        else
        {
            /* cross over register is in copper page and can't access when fiber is linkup
             * If access via page 8 register 16 with force access copper PHY, it will cause
             * the fiber link down.
             */
            return RT_ERR_PHY_FIBER_LINKUP;
        }
    }

    if ((ret = hal_miim_read(unit, base_port+1, 8, 16, &restore_phyData)) != RT_ERR_OK)
        return ret;

    phyData = restore_phyData & ~(1<<(12+port-base_port));
    phyData |= (1<<(8+port-base_port));

    if ((ret = hal_miim_write(unit, base_port+1, 8, 16, phyData)) != RT_ERR_OK)
        return ret;

    /* get value from CHIP*/
    if ((ret = hal_miim_write(unit, port, 7, 0x1e, 0x002d)) != RT_ERR_OK)
        return ret;

    if ((ret = hal_miim_read(unit, port, 7, 0x18, &phyData)) != RT_ERR_OK)
        return ret;
    force_mode = (phyData >> 5) & 0x1;

    if ((ret = hal_miim_read(unit, port, 0, 0x10, &phyData)) != RT_ERR_OK)
        return ret;
    mdi = (phyData >> 5) & 0x1;

    if ((ret = hal_miim_write(unit, base_port+1, 8, 16, restore_phyData)) != RT_ERR_OK)
        return ret;

    if (force_mode)
    {
        if (mdi)
            *pMode = PORT_CROSSOVER_MODE_MDI;
        else
            *pMode = PORT_CROSSOVER_MODE_MDIX;
    }
    else
        *pMode = PORT_CROSSOVER_MODE_AUTO;

    return ret;
} /* end of phy_8214fb_crossOverMode_get */

/* Function Name:
 *      phy_8214fb_crossOverMode_set
 * Description:
 *      Set cross over mode in the specified port.
 * Input:
 *      unit - unit id
 *      port - port id
 *      mode - cross over mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 *      RT_ERR_PORT_ID - invalid port id
 *      RT_ERR_INPUT   - invalid input parameter
 * Note:
 *      Following value is valid
 *      - PORT_CROSSOVER_MODE_AUTO
 *      - PORT_CROSSOVER_MODE_MDI
 *      - PORT_CROSSOVER_MODE_MDIX
 */
int32
phy_8214fb_crossOverMode_set(uint32 unit, rtk_port_t port, rtk_port_crossOver_mode_t mode)
{
    int32   ret;
    uint32  phyData, force_mode, mdi;
    uint32  restore_phyData, base_port, val, phyExtStatus15;
    uint32  is_copper_linkup;
    rtk_enable_t portEnable;

    base_port = port - (port % PORT_NUM_IN_8214FB);

    switch (mode)
    {
        case PORT_CROSSOVER_MODE_AUTO:
            force_mode = 0;
            mdi = 1;
            break;
        case PORT_CROSSOVER_MODE_MDI:
            force_mode = 1;
            mdi = 1;
            break;
        case PORT_CROSSOVER_MODE_MDIX:
            force_mode = 1;
            mdi = 0;
            break;
        default:
            return RT_ERR_INPUT;
    }

    ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_STATUS_REG, &val);
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_STATUS_REG, &val)) != RT_ERR_OK)
        return ret;

    if (val & LinkStatus_MASK)
    {
        if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, 15, &phyExtStatus15)) != RT_ERR_OK)
            return ret;
        is_copper_linkup = (phyExtStatus15 >> 13) & 0x1;

        if (is_copper_linkup)
        {
            /*Power Down*/
            portEnable = DISABLED;
            phy_8214fb_enable_set(unit, port, portEnable);


            osal_time_mdelay(1000);

            if ((ret = hal_miim_read(unit, base_port+1, 8, 16, &restore_phyData)) != RT_ERR_OK)
                goto set_crossOver_failed;

            phyData = restore_phyData & ~(1<<(12+port-base_port));
            phyData |= (1<<(8+port-base_port));

            if ((ret = hal_miim_write(unit, base_port+1, 8, 16, phyData)) != RT_ERR_OK)
                goto set_crossOver_failed;

            /* get value from CHIP*/
            if ((ret = hal_miim_write(unit, port, 7, 0x1e, 0x002d)) != RT_ERR_OK)
                goto set_crossOver_failed;
            if ((ret = hal_miim_read(unit, port, 7, 0x18, &phyData)) != RT_ERR_OK)
                goto set_crossOver_failed;
            phyData &= ~(1 << 5);
            phyData |= (force_mode << 5);
            if ((ret = hal_miim_write(unit, port, 7, 0x18, phyData)) != RT_ERR_OK)
                goto set_crossOver_failed;

            if ((ret = hal_miim_read(unit, port, 0, 0x10, &phyData)) != RT_ERR_OK)
                goto set_crossOver_failed;
            phyData &= ~(1 << 5);
            phyData |= (mdi << 5);
            if ((ret = hal_miim_write(unit, port, 0, 0x10, phyData)) != RT_ERR_OK)
                goto set_crossOver_failed;
set_crossOver_failed:
            if ((ret = hal_miim_write(unit, base_port+1, 8, 16, restore_phyData)) != RT_ERR_OK)
                return ret;
            /*Power Up*/
            portEnable = ENABLED;
            phy_8214fb_enable_set(unit, port, portEnable);

            return ret;
        }
        else
        {
            /* cross over register is in copper page and can't access when fiber is linkup
             * If access via page 8 register 16 with force access copper PHY, it will cause
             * the fiber link down.
             */
            return RT_ERR_PHY_FIBER_LINKUP;
        }
    }

    if ((ret = hal_miim_read(unit, base_port+1, 8, 16, &restore_phyData)) != RT_ERR_OK)
        return ret;

    phyData = restore_phyData & ~(1<<(12+port-base_port));
    phyData |= (1<<(8+port-base_port));

    if ((ret = hal_miim_write(unit, base_port+1, 8, 16, phyData)) != RT_ERR_OK)
        return ret;

    /* get value from CHIP*/
    if ((ret = hal_miim_write(unit, port, 7, 0x1e, 0x002d)) != RT_ERR_OK)
        return ret;
    if ((ret = hal_miim_read(unit, port, 7, 0x18, &phyData)) != RT_ERR_OK)
        return ret;
    phyData &= ~(1 << 5);
    phyData |= (force_mode << 5);
    if ((ret = hal_miim_write(unit, port, 7, 0x18, phyData)) != RT_ERR_OK)
        return ret;

    if ((ret = hal_miim_read(unit, port, 0, 0x10, &phyData)) != RT_ERR_OK)
        return ret;
    phyData &= ~(1 << 5);
    phyData |= (mdi << 5);
    if ((ret = hal_miim_write(unit, port, 0, 0x10, phyData)) != RT_ERR_OK)
        return ret;

    if ((ret = hal_miim_write(unit, base_port+1, 8, 16, restore_phyData)) != RT_ERR_OK)
        return ret;

    return ret;
} /* end of phy_8214fb_crossOverMode_set */

/* Function Name:
 *      phy_8214fb_fiber_media_get
 * Description:
 *      Get PHY 8212B/8214FB/8214B fiber media type.
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      pMedia - pointer buffer of phy fiber media type
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - invalid parameter
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      1. fiber media type value is as following:
 *      - PORT_FIBER_MEDIA_1000
 *      - PORT_FIBER_MEDIA_100
 *      - PORT_FIBER_MEDIA_AUTO
 */
int32
phy_8214fb_fiber_media_get(uint32 unit, rtk_port_t port, rtk_port_fiber_media_t *pMedia)
{
    uint32  val;
    int32   ret = RT_ERR_FAILED;

    if ((ret = hal_miim_read(unit, port, PHY_PAGE_8, 30, &val)) != RT_ERR_OK)
        return ret;
    if (((val >> 11) & 0x1) == 0)
        *pMedia = PORT_FIBER_MEDIA_AUTO;
    else if (((val >> 10) & 0x1) == 0)
        *pMedia = PORT_FIBER_MEDIA_1000;
    else
        *pMedia = PORT_FIBER_MEDIA_100;

    return RT_ERR_OK;
} /* end of phy_8214fb_fiber_media_get */

/* Function Name:
 *      phy_8214fb_fiber_media_set
 * Description:
 *      Get PHY 8212B/8214FB/8214B fiber media type.
 * Input:
 *      unit  - unit id
 *      port  - port id
 *      media - phy media fiber type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      1. fiber media type value is as following:
 *      - PORT_FIBER_MEDIA_1000
 *      - PORT_FIBER_MEDIA_100
 *      - PORT_FIBER_MEDIA_AUTO
 */
int32
phy_8214fb_fiber_media_set(uint32 unit, rtk_port_t port, rtk_port_fiber_media_t media)
{
    uint32  val, phyReg28, phyReg16;
    uint32  base_port = 0;
    int32   ret = RT_ERR_FAILED;

    base_port = port - (port % PORT_NUM_IN_8214FB);

    switch (media)
    {
        case PORT_FIBER_MEDIA_1000:
            if ((ret = hal_miim_read(unit, port, PHY_PAGE_8, 30, &val)) != RT_ERR_OK)
                return ret;
            if ((((val >> 11) & 0x1) == 1) && (((val >> 10) & 0x1) == 0))
            {
                /* no change fiber media */
                break;
            }
            /* Power Down the Fiber Port */
            if ((ret = hal_miim_read(unit, base_port+3, PHY_PAGE_8, 28, &phyReg28)) != RT_ERR_OK)
                return ret;
            val = (phyReg28 | (1<<12) | (1<<(port-base_port+8)));
            if ((ret = hal_miim_write(unit, base_port+3, PHY_PAGE_8, 28, val)) != RT_ERR_OK)
                return ret;
            /* Force Select Fiber Standard Register */
            if ((ret = hal_miim_read(unit, base_port+1, PHY_PAGE_8, 16, &phyReg16)) != RT_ERR_OK)
                return ret;
            val = (phyReg16 | (0x11<<(port-base_port+8)));
            if ((ret = hal_miim_write(unit, base_port+1, PHY_PAGE_8, 16, val)) != RT_ERR_OK)
                return ret;
            /* Enable or Disable 100Base-FX/1000Base-X Auto Sensing */
            /* change to ext. page 19 */
            if ((ret = hal_miim_write(unit, port, 15, 30, 0x0013)) != RT_ERR_OK)
                return ret;
            /* enable or disable 100Base-FX/1000Base-X auto sensing (enable: 0xE46A, disable: 0x646A) */
            if ((ret = hal_miim_write(unit, port, 15, 26, 0x646A)) != RT_ERR_OK)
                return ret;
            /* Fiber Interface Setting */
            if ((ret = hal_miim_read(unit, port, PHY_PAGE_8, 30, &val)) != RT_ERR_OK)
                return ret;
            val &= ~(3<<10);
            val |= (2<<10);
            if ((ret = hal_miim_write(unit, port, PHY_PAGE_8, 30, val)) != RT_ERR_OK)
                return ret;

            /* Fiber Capability Setting */
            if ((ret = hal_miim_read(unit, port, PHY_PAGE_8, 0, &val)) != RT_ERR_OK)
                return ret;
            val &= ~0x3140;
            val |= 0x1140;  /* update bit: 8, 6, 12, 13 */
            if ((ret = hal_miim_write(unit, port, PHY_PAGE_8, 0, val)) != RT_ERR_OK)
                return ret;

            /* Auto Mode Standard Register */
            if ((ret = hal_miim_read(unit, base_port+1, PHY_PAGE_8, 16, &phyReg16)) != RT_ERR_OK)
                return ret;
            val = (phyReg16 & (~(0x11<<(port-base_port+8))));
            if ((ret = hal_miim_write(unit, base_port+1, PHY_PAGE_8, 16, val)) != RT_ERR_OK)
                return ret;
            /* Power On Fiber */
            if ((ret = hal_miim_read(unit, base_port+3, PHY_PAGE_8, 28, &phyReg28)) != RT_ERR_OK)
                return ret;
            val = (phyReg28 & (~(1<<12)) & (~(1<<(port-base_port+8))));
            if ((ret = hal_miim_write(unit, base_port+3, PHY_PAGE_8, 28, val)) != RT_ERR_OK)
                return ret;
            break; /* end of switch-case PORT_FIBER_MEDIA_1000 */

        case PORT_FIBER_MEDIA_100:
            if ((ret = hal_miim_read(unit, port, PHY_PAGE_8, 30, &val)) != RT_ERR_OK)
                return ret;

            if ((((val >> 11) & 0x1) == 1) && (((val >> 10) & 0x1) == 1))
            {
                /* no change fiber media */
                break;
            }
            /* Power Down the Fiber Port */
            if ((ret = hal_miim_read(unit, base_port+3, PHY_PAGE_8, 28, &phyReg28)) != RT_ERR_OK)
                return ret;
            val = (phyReg28 | (1<<12) | (1<<(port-base_port+8)));
            if ((ret = hal_miim_write(unit, base_port+3, PHY_PAGE_8, 28, val)) != RT_ERR_OK)
                return ret;
            /* Force Select Fiber Standard Register */
            if ((ret = hal_miim_read(unit, base_port+1, PHY_PAGE_8, 16, &phyReg16)) != RT_ERR_OK)
                return ret;
            val = (phyReg16 | (0x11<<(port-base_port+8)));
            if ((ret = hal_miim_write(unit, base_port+1, PHY_PAGE_8, 16, val)) != RT_ERR_OK)
                return ret;
            /* Enable or Disable 100Base-FX/1000Base-X Auto Sensing */
            /* change to ext. page 19 */
            if ((ret = hal_miim_write(unit, port, 15, 30, 0x0013)) != RT_ERR_OK)
                return ret;
            /* enable or disable 100Base-FX/1000Base-X auto sensing (enable: 0xE46A, disable: 0x646A) */
            if ((ret = hal_miim_write(unit, port, 15, 26, 0x646A)) != RT_ERR_OK)
                return ret;
            /* Fiber Interface Setting */
            if ((ret = hal_miim_read(unit, port, PHY_PAGE_8, 30, &val)) != RT_ERR_OK)
                return ret;
            val &= ~(3<<10);
            val |= (3<<10);
            if ((ret = hal_miim_write(unit, port, PHY_PAGE_8, 30, val)) != RT_ERR_OK)
                return ret;

            /* Fiber Capability Setting */
            if ((ret = hal_miim_read(unit, port, PHY_PAGE_8, 0, &val)) != RT_ERR_OK)
                return ret;
            val &= ~0x3140;
            val |= 0x2100;  /* update bit: 8, 6, 12, 13 */
            if ((ret = hal_miim_write(unit, port, PHY_PAGE_8, 0, val)) != RT_ERR_OK)
                return ret;

            /* Auto Mode Standard Register */
            if ((ret = hal_miim_read(unit, base_port+1, PHY_PAGE_8, 16, &phyReg16)) != RT_ERR_OK)
                return ret;
            val = (phyReg16 & (~(0x11<<(port-base_port+8))));
            if ((ret = hal_miim_write(unit, base_port+1, PHY_PAGE_8, 16, val)) != RT_ERR_OK)
                return ret;
            /* Power On Fiber */
            if ((ret = hal_miim_read(unit, base_port+3, PHY_PAGE_8, 28, &phyReg28)) != RT_ERR_OK)
                return ret;
            val = (phyReg28 & (~(1<<12)) & (~(1<<(port-base_port+8))));
            if ((ret = hal_miim_write(unit, base_port+3, PHY_PAGE_8, 28, val)) != RT_ERR_OK)
                return ret;
            break; /* end of switch-case PORT_FIBER_MEDIA_100 */

        case PORT_FIBER_MEDIA_AUTO:
            if ((ret = hal_miim_read(unit, port, PHY_PAGE_8, 30, &val)) != RT_ERR_OK)
                return ret;

            if (((val >> 11) & 0x1) == 0)
            {
                /* no change fiber media */
                break;
            }
            /* Power Down the Fiber Port */
            if ((ret = hal_miim_read(unit, base_port+3, PHY_PAGE_8, 28, &phyReg28)) != RT_ERR_OK)
                return ret;
            val = (phyReg28 | (1<<12) | (1<<(port-base_port+8)));
            if ((ret = hal_miim_write(unit, base_port+3, PHY_PAGE_8, 28, val)) != RT_ERR_OK)
                return ret;
            /* Force Select Fiber Standard Register */
            if ((ret = hal_miim_read(unit, base_port+1, PHY_PAGE_8, 16, &phyReg16)) != RT_ERR_OK)
                return ret;
            val = (phyReg16 | (0x11<<(port-base_port+8)));
            if ((ret = hal_miim_write(unit, base_port+1, PHY_PAGE_8, 16, val)) != RT_ERR_OK)
                return ret;
            /* Enable or Disable 100Base-FX/1000Base-X Auto Sensing */
            /* change to ext. page 19 */
            if ((ret = hal_miim_write(unit, port, 15, 30, 0x0013)) != RT_ERR_OK)
                return ret;
            /* enable or disable 100Base-FX/1000Base-X auto sensing (enable: 0xE46A, disable: 0x646A) */
            if ((ret = hal_miim_write(unit, port, 15, 26, 0xE46A)) != RT_ERR_OK)
                return ret;
            /* Fiber Interface Setting */
            if ((ret = hal_miim_read(unit, port, PHY_PAGE_8, 30, &val)) != RT_ERR_OK)
                return ret;
            val &= ~(3<<10);
            if ((ret = hal_miim_write(unit, port, PHY_PAGE_8, 30, val)) != RT_ERR_OK)
                return ret;

            /* Fiber Capability Setting */
            if ((ret = hal_miim_read(unit, port, PHY_PAGE_8, 0, &val)) != RT_ERR_OK)
                return ret;
            val &= ~0x3140;
            val |= 0x1140;  /* update bit: 8, 6, 12, 13 */
            if ((ret = hal_miim_write(unit, port, PHY_PAGE_8, 0, val)) != RT_ERR_OK)
                return ret;

            /* Auto Mode Standard Register */
            if ((ret = hal_miim_read(unit, base_port+1, PHY_PAGE_8, 16, &phyReg16)) != RT_ERR_OK)
                return ret;
            val = (phyReg16 & (~(0x11<<(port-base_port+8))));
            if ((ret = hal_miim_write(unit, base_port+1, PHY_PAGE_8, 16, val)) != RT_ERR_OK)
                return ret;
            /* Power On Fiber */
            if ((ret = hal_miim_read(unit, base_port+3, PHY_PAGE_8, 28, &phyReg28)) != RT_ERR_OK)
                return ret;
            val = (phyReg28 & (~(1<<12)) & (~(1<<(port-base_port+8))));
            if ((ret = hal_miim_write(unit, base_port+3, PHY_PAGE_8, 28, val)) != RT_ERR_OK)
                return ret;
            break; /* end of switch-case PORT_FIBER_MEDIA_100 */

        default:
            break;
    }

    return RT_ERR_OK;
} /* end of phy_8214fb_fiber_media_set */

/* Function Name:
 *      phy_8214fb_linkDownPowerSavingEnable_get
 * Description:
 *      Get the status of link-down power saving of the specific port in the specific unit
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to status of link-down power saving
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 * Note:
 *      1. The RTL8214FB is supported the per-port link-down power saving.
 *      2. The function should be called when media is copper.
 *      3. Error code will be return when fiber media is link up.
 */
int32
phy_8214fb_linkDownPowerSavingEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    uint32  phyData;
    uint32  val, phyExtStatus15;
    uint32  is_copper_linkup;
    uint32  restore_val;
    int32   ret = RT_ERR_FAILED;
    hal_control_t   *pHalCtrl;

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_STATUS_REG, &val);
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_STATUS_REG, &val)) != RT_ERR_OK)
        return ret;

    if (val & LinkStatus_MASK)
    {
        if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, 15, &phyExtStatus15)) != RT_ERR_OK)
            return ret;
        is_copper_linkup = (phyExtStatus15 >> 13) & 0x1;

        if (is_copper_linkup)
        {
            /* getting page 0, register 21, bit[12] for link-down power saving state */
            if ((ret = hal_miim_read(unit, port, 0, 21, &phyData)) != RT_ERR_OK)
                return ret;
            if (((phyData >> 12) & 0x1) == 0x1)
                *pEnable = ENABLED;
            else
                *pEnable = DISABLED;
            return RT_ERR_OK;
        }
        else
        {
            /* return the value from shadow when fiber is linkup. */
            *pEnable = (*pPhy_info[unit]).ldps_state[port];
            return RT_ERR_OK;
        }
    }

    if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_read(unit, port, PHY_PAGE_8, 30, &restore_val)) != RT_ERR_OK)
        return ret;

    /* Link-down */
    if ((restore_val & 0x2000) == 0x0000)
    {
        val = (restore_val | 0x2000);
        val &= ~(0x1000);

        if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_write(unit, port, PHY_PAGE_8, 30, val)) != RT_ERR_OK)
            return ret;
    }

    /* getting page 0, register 21, bit[12] for link-down power saving state */
    if ((ret = hal_miim_read(unit, port, 0, 21, &phyData)) != RT_ERR_OK)
        return ret;
    if (((phyData >> 12) & 0x1) == 0x1)
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;

    if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_write(unit, port, PHY_PAGE_8, 30, restore_val)) != RT_ERR_OK)
        return ret;

    return RT_ERR_OK;
} /* end of phy_8214fb_linkDownPowerSavingEnable_get */

/* Function Name:
 *      phy_8214fb_linkDownPowerSavingEnable_set
 * Description:
 *      Set the status of link-down power saving of the specific port in the specific unit
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - status of link-down power saving
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 * Note:
 *      1. The RTL8214FB is supported the per-port link-down power saving.
 *      2. The function should be called when media is copper.
 *      3. Error code will be return when fiber media is link up.
 */
int32
phy_8214fb_linkDownPowerSavingEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    uint32  phyData;
    uint32  val, phyExtStatus15;
    uint32  is_copper_linkup;
    uint32  restore_val;
    int32   ret = RT_ERR_FAILED;
    hal_control_t   *pHalCtrl;

    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;

    ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_STATUS_REG, &val);
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_STATUS_REG, &val)) != RT_ERR_OK)
        return ret;

    if (val & LinkStatus_MASK)
    {
        if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, 15, &phyExtStatus15)) != RT_ERR_OK)
            return ret;
        is_copper_linkup = (phyExtStatus15 >> 13) & 0x1;

        if (is_copper_linkup)
        {
            /* get value from CHIP*/
            if ((ret = hal_miim_read(unit, port, 0, 21, &phyData)) != RT_ERR_OK)
                return ret;
            if (ENABLED == enable)
            {
                phyData |= (0x1 << 12);
            }
            else
            {
                phyData &= ~(0x1 << 12);
            }
            if ((ret = hal_miim_write(unit, port, 0, 21, phyData)) != RT_ERR_OK)
                return ret;

            /* Update shadow value */
            if (ENABLED == enable)
            {
                (*pPhy_info[unit]).ldps_state[port] = ENABLED;
            }
            else
            {
                (*pPhy_info[unit]).ldps_state[port] = DISABLED;
            }

            return RT_ERR_OK;
        }
        else
        {
            /* register is in copper page and can't access when fiber is linkup. */
            return RT_ERR_PHY_FIBER_LINKUP;
        }
    }

    if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_read(unit, port, PHY_PAGE_8, 30, &restore_val)) != RT_ERR_OK)
        return ret;

    /* Link-down */
    if ((restore_val & 0x2000) == 0x0000)
    {
        val = (restore_val | 0x2000);
        val &= ~(0x1000);

        if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_write(unit, port, PHY_PAGE_8, 30, val)) != RT_ERR_OK)
            return ret;
    }

    /* For Link-Down Power Saving (per-port) */
    if ((ret = hal_miim_read(unit, port, 0, 21, &phyData)) != RT_ERR_OK)
        return ret;
    phyData &= ~(0x1 << 12);
    if (ENABLED == enable)
    {
        phyData |= (0x1 << 12);
    }
    if ((ret = hal_miim_write(unit, port, 0, 21, phyData)) != RT_ERR_OK)
        return ret;

    /* Update shadow value */
    if (ENABLED == enable)
    {
        (*pPhy_info[unit]).ldps_state[port] = ENABLED;
    }
    else
    {
        (*pPhy_info[unit]).ldps_state[port] = DISABLED;
    }

    if ((ret = MACDRV(pHalCtrl)->fMdrv_miim_write(unit, port, PHY_PAGE_8, 30, restore_val)) != RT_ERR_OK)
        return ret;

    return ret;
} /* end of phy_8214fb_linkDownPowerSavingEnable_set */

/* Function Name:
 *      phy_8214fb_gigaLiteEnable_get
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
 *      1. The RTL8214fb is not supported the per-port Giga Lite feature.
 */
int32
phy_8214fb_gigaLiteEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    *pEnable = DISABLED;
    return RT_ERR_OK;
} /* end of phy_8214fb_gigaLiteEnable_get */

/* Function Name:
 *      phy_8214fb_fiberOAMLoopBack_set
 * Description:
 *      Set Fiber-Port OAM Loopback feature,
 * Input:
 *      unit  - unit id
 *      port  - port id
 *      enable - Fiber-Port OAM Loopback feature
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8214fb_fiberOAMLoopBack_set(uint32 unit, rtk_port_t port,
    rtk_enable_t enable)
{
    uint32  phy_data;
    uint32  reg_val;
    int32    ret;

    /*Backup Media*/
    if ((ret = hal_miim_read(unit, port, 8, 30, &phy_data)) != RT_ERR_OK)
        return ret;

    /*Force Fiber*/
    reg_val = phy_data | (0x3<<12);
    if ((ret = hal_miim_write(unit, port, 8, 30, reg_val)) != RT_ERR_OK)
        return ret;

    if ((ret = hal_miim_read(unit, port, 0, 0, &reg_val)) != RT_ERR_OK)
        return ret;

    if(enable == ENABLED)
        reg_val |= (1UL<<14);
    else
        reg_val &= ~(1UL<<14);

    if ((ret = hal_miim_write(unit, port, 0, 0, reg_val)) != RT_ERR_OK)
        return ret;

    /*Restore  Media*/
    if ((ret = hal_miim_write(unit, port, 8, 30, phy_data)) != RT_ERR_OK)
        return ret;


    if(enable == ENABLED)
    {
        /*Delay until PHY Linkup*/
        osal_time_usleep(100 * 1000); /* delay 10mS */
    }

    return ret;
}   /* end of phy_8214fb_fiberOAMLoopBack_set */



/* Function Name:
 *      phy_8214fb_loopback_get
 * Description:
 *      Get PHY Loopback mode of the specific port
 * Input:
 *      unit                - unit id
 *      port                - port id
 * Output:
 *      pEnable             - loopback mode status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8214fb_loopback_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    rtk_port_media_t    media;
    int32               ret;

    if ((ret = phy_8214fb_media_get(unit, port, &media)) != RT_ERR_OK)
    {
        return ret;
    }
    if ((media != PORT_MEDIA_FIBER) && (media != PORT_MEDIA_COPPER))
    {
        return RT_ERR_PHY_MEDIA;
    }

    /* get the standard reg of the foced fiber/copper mode */
    ret = phy_common_loopback_get(unit, port, pEnable);

    return ret;
}


/* Function Name:
 *      phy_8214fb_loopback_set
 * Description:
 *      Set PHY Loopback mode of the specific port
 * Input:
 *      unit                - unit id
 *      port                - port id
 *      enable              - ENABLED: Enable loopback;
 *                            DISABLED: Disable loopback. PHY back to normal operation.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *
 */
int32
phy_8214fb_loopback_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    rtk_port_media_t    media;
    int32               ret;
    rtk_enable_t        prevEnable;
    uint32              data;

    if ((ret = phy_8214fb_media_get(unit, port, &media)) != RT_ERR_OK)
    {
        return ret;
    }

    if ((media != PORT_MEDIA_FIBER) && (media != PORT_MEDIA_COPPER))
    {
        return RT_ERR_PHY_MEDIA;
    }

    hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &data);
    prevEnable = (data & Loopback_MASK) ? ENABLED : DISABLED;
    if (prevEnable != enable)
    {
        if (enable == ENABLED) /* Disable to Enable */
        {
            if (data & AutoNegotiationEnable_MASK)
            {
                /* disable auto-nego */
                data &= ~AutoNegotiationEnable_MASK;
                RTK_PORTMASK_PORT_SET((*pPhy_info[unit]).autoNegoEnablePortMask, port);
            }
            else
            {
                RTK_PORTMASK_PORT_CLEAR((*pPhy_info[unit]).autoNegoEnablePortMask, port);
            }
        }
        else /* Enable to Disable */
        {
            if (RTK_PORTMASK_IS_PORT_SET((*pPhy_info[unit]).autoNegoEnablePortMask, port))
            {
                /* recover auto-nego setting */
                data |= AutoNegotiationEnable_MASK;
            }
        }
    }

    if (enable == ENABLED)
        data |= Loopback_MASK;
    else
        data &= ~Loopback_MASK;

    if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, data)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;

}

#endif /* !defined(__BOOTLOADER__) */

/* Function Name:
 *      phy_8214fb_ieeeTestMode_set
 * Description:
 *      Set test mode for Giga PHY transmitter test
 * Input:
 *      unit  - unit id
 *      port  - port id
 *      pTestMode->mode  - test mode 1 ~ 4 which is defined in IEEE 40.6.1.1.2
 *      pTestMode->channel  - Channel A, B, C, D, or none
 *      pTestMode->flags -
 *          RTK_PORT_PHYTESTMODE_FLAG_ALL_PHY_PORTS -
 *              apply the test on all ports of the PHY.
 *              To use this feature, the "port" parameter shall set to the first port of the PHY.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_NOT_ALLOWED - The operation is not allowed
 *      RT_ERR_PORT_NOT_SUPPORTED - test mode is not supported
 * Note:
 *      RTK_PORT_PHYTESTMODE_FLAG_ALL_PHY_PORTS is not supported
 */
int32
phy_8214fb_ieeeTestMode_set(uint32 unit, rtk_port_t port, rtk_port_phyTestMode_t *pTestMode)
{

    osal_printf("RTL8214FB/RTL8214B/RTL8212B Test Mode (PHYID: %u)\n", port);

    if (pTestMode->flags & RTK_PORT_PHYTESTMODE_FLAG_ALL_PHY_PORTS)
    {
        osal_printf("Not support all-phy-port operation.\n");
        return RT_ERR_NOT_ALLOWED;
    }

    switch (pTestMode->mode)
    {
      case 1:
        {
            /* Disable LDPS */
            hal_miim_write(unit, port,0,21,0x0006); // disable LDPS
            /* Test Mode 1 */
            hal_miim_write(unit, port,0,0,0x1140);  // power on PHY
            hal_miim_write(unit, port,0,9,0x2E00);  // test mode 1
            osal_time_mdelay(100);	 /* delay 100mS and wait for Mirco-P completed */
            /* Adjust Amplitude */
            hal_miim_write(unit, port,2,21,0xAA00); // adjust amplitude
            return RT_ERR_OK;
        }
        break;

      case 4:
        {
            /* Disable LDPS */
            hal_miim_write(unit, port,0,21,0x0006); // disable LDPS
            /* Test Mode 4 */
            hal_miim_write(unit, port,0,0,0x1140);  // power on PHY
            hal_miim_write(unit, port,0,9,0x8E00);  // Enable Test mode 4
            osal_time_mdelay(100);	 /* delay 100mS and wait for Mirco-P completed */
            /* Adjust LDVbias */
            hal_miim_write(unit, port,2,5,0xCE68);  // adjust LDVbias
            return RT_ERR_OK;
        }
        break;

      default:
        osal_printf("The mode (%u) is not suppoted.\n", pTestMode->mode);
        return RT_ERR_PORT_NOT_SUPPORTED;
    }
}

#ifndef __BOOTLOADER__
/* Function Name:
 *      phy_8214fb_patch_set
 * Description:
 *      Set patch to PHY.
 * Input:
 *      unit   - unit id
 *      port   - port id
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
phy_8214fb_patch_set(uint32 unit, rtk_port_t port)
{
    int32 ret;
    ret = sub_phy_8214fb_patch_set(unit, port);
    return ret;
} /* end of phy_8214fb_patch_set */

#endif

