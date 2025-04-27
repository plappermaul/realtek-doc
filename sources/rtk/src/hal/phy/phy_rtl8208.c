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
 * Purpose : PHY 8208 Driver APIs.
 *
 * Feature : PHY 8208 Driver APIs
 *
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/debug/rt_log.h>
#include <hal/common/halctrl.h>
#include <hal/phy/phydef.h>
#include <hal/phy/phy_common.h>
#include <hal/phy/phy_rtl8208.h>
#include <hal/phy/phy_rtl8208_patch.h>
#include <hal/mac/miim_common_drv.h>
#include <osal/memory.h>
#include <osal/time.h>

/*
 * Symbol Definition
 */
typedef struct fe_phy_rtct_info_s {
    uint8   isRxShort[RTK_MAX_NUM_OF_PORTS];
    uint8   isTxShort[RTK_MAX_NUM_OF_PORTS];
    uint8   isRxOpen[RTK_MAX_NUM_OF_PORTS];
    uint8   isTxOpen[RTK_MAX_NUM_OF_PORTS];
    uint32  rxLen[RTK_MAX_NUM_OF_PORTS];
    uint32  txLen[RTK_MAX_NUM_OF_PORTS];
    uint8   flag_completed[RTK_MAX_NUM_OF_PORTS];
} fe_phy_rtct_info_t;


/*
 * Macro Declaration
 */

/* check if the port of 8208D is in fiber-mode.
 *   1: if it is 8208D fiber-mode port; 0: if the PHY is not 8208D or the port is not in fiber-mode.
 */
#define PHY_8208D_FIBER_PORT(unit, port)       (fiber_pmask[unit] && RTK_PORTMASK_IS_PORT_SET(*fiber_pmask[unit], port))?1:0


/*
 * Data Declaration
 */
rt_phyInfo_t phy_8208_info =
{
    .phy_num    = PORT_NUM_IN_8208,
    .eth_type   = HWP_FE,
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

static fe_phy_rtct_info_t  *pPhy_rtctInfo[RTK_MAX_NUM_OF_UNIT] = {NULL};
static rtk_portmask_t       *fiber_pmask[RTK_MAX_NUM_OF_UNIT] = {NULL};
static rtk_portmask_t      *autoNegoEnablePortMask[RTK_MAX_NUM_OF_UNIT] = {NULL};       /* auto nego shadow state for loopback */

/*
 * Function Declaration
 */
/*
 * Function Declaration
 */

/* Function Name:
 *      phy_8208Gdrv_fe_mapperInit
 * Description:
 *      Initialize PHY 8208G driver.
 * Input:
 *      pPhydrv - pointer of phy driver
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void phy_8208Gdrv_fe_mapperInit(rt_phydrv_t *pPhydrv)
{
    pPhydrv->phydrv_index = RT_PHYDRV_RTL8208G;
    pPhydrv->fPhydrv_init = phy_8208_init;
    pPhydrv->fPhydrv_greenEnable_get = phy_8208_greenEnable_get;
    pPhydrv->fPhydrv_greenEnable_set = phy_8208_greenEnable_set;
    pPhydrv->fPhydrv_enable_set = phy_common_enable_set;
#if !defined(__BOOTLOADER__)
    pPhydrv->fPhydrv_media_get = phy_8208_media_get;
    pPhydrv->fPhydrv_autoNegoEnable_get = phy_common_autoNegoEnable_get;
    pPhydrv->fPhydrv_autoNegoEnable_set = phy_common_autoNegoEnable_set;
    pPhydrv->fPhydrv_autoNegoAbility_get = phy_8208_autoNegoAbility_get;
    pPhydrv->fPhydrv_autoNegoAbility_set = phy_8208_autoNegoAbility_set;
    pPhydrv->fPhydrv_duplex_get = phy_common_duplex_get;
    pPhydrv->fPhydrv_duplex_set = phy_common_duplex_set;
    pPhydrv->fPhydrv_speed_get = phy_8208_speed_get;
    pPhydrv->fPhydrv_speed_set = phy_8208_speed_set;
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
    pPhydrv->fPhydrv_loopback_get = phy_common_loopback_get;
    pPhydrv->fPhydrv_loopback_set = phy_8208_loopback_set;
    pPhydrv->fPhydrv_rtctResult_get = phy_8208_rtctResult_get;
    pPhydrv->fPhydrv_rtct_start = phy_8208_rtct_start;
    pPhydrv->fPhydrv_crossOverMode_get = phy_8208_crossOverMode_get;
    pPhydrv->fPhydrv_crossOverMode_set = phy_8208_crossOverMode_set;
    pPhydrv->fPhydrv_linkDownPowerSavingEnable_get = phy_8208_linkDownPowerSavingEnable_get;
    pPhydrv->fPhydrv_linkDownPowerSavingEnable_set = phy_8208_linkDownPowerSavingEnable_set;
#endif
} /* end of phy_8208Gdrv_fe_mapperInit*/


/* Function Name:
 *      phy_8208Ddrv_fe_mapperInit
 * Description:
 *      Initialize PHY 8208D driver.
 * Input:
 *      pPhydrv - pointer of phy driver
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void phy_8208Ddrv_fe_mapperInit(rt_phydrv_t *pPhydrv)
{
    pPhydrv->phydrv_index = RT_PHYDRV_RTL8208D;
    pPhydrv->fPhydrv_init = phy_8208d_init;
    pPhydrv->fPhydrv_enable_set = phy_common_enable_set;
#if !defined(__BOOTLOADER__)
    pPhydrv->fPhydrv_media_get = phy_8208d_media_get;
    pPhydrv->fPhydrv_media_set = phy_8208d_media_set;
    pPhydrv->fPhydrv_autoNegoEnable_get = phy_common_autoNegoEnable_get;
    pPhydrv->fPhydrv_autoNegoEnable_set = phy_8208d_autoNegoEnable_set;
    pPhydrv->fPhydrv_autoNegoAbility_get = phy_8208_autoNegoAbility_get;
    pPhydrv->fPhydrv_autoNegoAbility_set = phy_8208_autoNegoAbility_set;
    pPhydrv->fPhydrv_duplex_get = phy_common_duplex_get;
    pPhydrv->fPhydrv_duplex_set = phy_8208d_duplex_set;
    pPhydrv->fPhydrv_speed_get = phy_8208_speed_get;
    pPhydrv->fPhydrv_speed_set = phy_8208_speed_set;
    pPhydrv->fPhydrv_eeeEnable_get = phy_8208d_eeeEnable_get;
    pPhydrv->fPhydrv_eeeEnable_set = phy_8208d_eeeEnable_set;
    pPhydrv->fPhydrv_fiberOAMLoopBackEnable_set = phy_8208d_fiberOAMLoopBackEnable_set;
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
    pPhydrv->fPhydrv_loopback_get = phy_common_loopback_get;
    pPhydrv->fPhydrv_loopback_set = phy_8208_loopback_set;
    pPhydrv->fPhydrv_rtctResult_get = phy_8208d_rtctResult_get;
    pPhydrv->fPhydrv_rtct_start = phy_8208d_rtct_start;
    pPhydrv->fPhydrv_crossOverMode_get = phy_8208d_crossOverMode_get;
    pPhydrv->fPhydrv_crossOverMode_set = phy_8208d_crossOverMode_set;
    pPhydrv->fPhydrv_linkDownPowerSavingEnable_get = phy_8208d_linkDownPowerSavingEnable_get;
    pPhydrv->fPhydrv_linkDownPowerSavingEnable_set = phy_8208d_linkDownPowerSavingEnable_set;
    pPhydrv->fPhydrv_gigaLiteEnable_get = phy_8208d_gigaLiteEnable_get;
#endif
} /* end of phy_8208Ddrv_fe_mapperInit*/

/* Function Name:
 *      phy_8208d_fiberPortBitmap_get
 * Description:
 *      Get fiber port bit map of 8208D
 * Input:
 *      unit - unit id
 *      base_port - base port (mac id) of the PHY
 * Output:
 *      p_fpbm - fiber port bit map.
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - failed.
 * Note:
 *      None
 */
int32
phy_8208d_fiberPortBitmap_get(uint32 unit, rtk_port_t base_port, uint32 *p_fpbm)
{
    int32       ret;
    uint32      data;
    uint32      sel_txfx, pp_lpbkmode;
    uint32      fpbm=0;

    if ((ret = hal_miim_read(unit, base_port, 67, 30, &data)) != RT_ERR_OK)
    {
        return ret;
    }

    sel_txfx = (data >> 3) & 0x03;
    pp_lpbkmode = (data >> 5) & 0x01;
    switch (sel_txfx)
    {
      case 0x0:
        fpbm = 0x0;
        break;
      case 0x1:
        if (pp_lpbkmode == 0)
            fpbm = 0x80; /* port 7 */
        else
            fpbm = 0xA0; /* port 7, 5 */
        break;
      case 0x2:
        if (pp_lpbkmode == 0)
            fpbm = 0xC0;/* port 7, 6 */
        else
            fpbm = 0xAA;/* port 7, 5, 3, 1 */
        break;
      case 0x3:
        fpbm = 0xFF;
        break;
    }

    *p_fpbm = fpbm;
    return ret;
}


/* Function Name:
 *      phy_8208_init
 * Description:
 *      Initialize PHY 8208.
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
phy_8208_init(uint32 unit, rtk_port_t port)
{
    int32   ret = RT_ERR_OK;
#if defined(CONFIG_SDK_WA_EEE_COMPATIBLE)
#if defined(CONFIG_SDK_RTL8208D)
    uint32  phyData;
#endif
#endif
#if (defined(CONFIG_SDK_WA_EEE_COMPATIBLE) && defined(CONFIG_SDK_RTL8208D)) || defined(CONFIG_SDK_RTL8208L)
    hal_control_t   *pHalCtrl;
#endif


    /* Allocate the shadow database for record RTCT result in the unit */
    if (NULL == pPhy_rtctInfo[unit])
    {
        pPhy_rtctInfo[unit] = (fe_phy_rtct_info_t *)osal_alloc(sizeof(fe_phy_rtct_info_t));
        if (NULL == pPhy_rtctInfo[unit])
        {
            RT_ERR(RT_ERR_FAILED, (MOD_HAL), "memory allocate failed");
            return RT_ERR_FAILED;
        }
        osal_memset(pPhy_rtctInfo[unit], 0, sizeof(fe_phy_rtct_info_t));
    }


    if (NULL == autoNegoEnablePortMask[unit])
    {
        autoNegoEnablePortMask[unit] = (rtk_portmask_t *)osal_alloc(sizeof(rtk_portmask_t));
        if (NULL == autoNegoEnablePortMask[unit])
        {
            RT_ERR(RT_ERR_FAILED, (MOD_HAL), "memory allocate failed");
            return RT_ERR_FAILED;
        }
        osal_memset(autoNegoEnablePortMask[unit], 0, sizeof(rtk_portmask_t));
    }

#if (defined(CONFIG_SDK_WA_EEE_COMPATIBLE) && defined(CONFIG_SDK_RTL8208D)) || defined(CONFIG_SDK_RTL8208L)
    if ((pHalCtrl = hal_ctrlInfo_get(unit)) == NULL)
        return RT_ERR_FAILED;
#endif

#if defined(CONFIG_SDK_WA_EEE_COMPATIBLE)
#if defined(CONFIG_SDK_RTL8208D)
    if (RT_PHYDRV_RTL8208D == pHalCtrl->pPhy_ctrl[port]->pPhydrv->phydrv_index)
    {
        if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, 22, &phyData)) != RT_ERR_OK)
            return ret;
        if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, 22, (phyData & (~(0x1<<14))))) != RT_ERR_OK)
            return ret;
    }
#endif
#endif

#if defined(CONFIG_SDK_RTL8208L)
    if (RT_PHYDRV_RTL8208D == pHalCtrl->pPhy_ctrl[port]->pPhydrv->phydrv_index)
    {
        if ((ret = hal_miim_write(unit, port, 5, 16, 0xe00)) != RT_ERR_OK)
            return ret;
    }
#endif

#if defined(CONFIG_SDK_RTL8380)
    /* For 8380 serdes linkdown watchdog, enable serdes always linkup */
    if (HWP_8330_FAMILY_ID(unit))
    {
        uint32 serdes_data;

        if((port % 8) == 0)
        {
            /*08L*/
            serdes_data = 0x8403;
            hal_miim_write(unit,  port,  64,  16 , serdes_data);
        }
    }

    /*Enable pktgen en for Phy error counter patch code*/
    if (HWP_8330_FAMILY_ID(unit))
    {
        uint32 phy_data;

        /*08L*/
        phy_data = 0x1;
        hal_miim_write(unit,  port,  65,  19 , phy_data);
        phy_data = 0xff;
        hal_miim_write(unit,  port,  65,  21 , phy_data);
        phy_data = 0xff;
        hal_miim_write(unit,  port,  65,  20 , phy_data);
    }
#endif
    phy_8208_greenEnable_set(unit, port, DISABLED);

    return ret;
} /* end of phy_8208_init */


/* Function Name:
 *      phy_8208d_init
 * Description:
 *      Initialize PHY 8208.
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
phy_8208d_init(uint32 unit, rtk_port_t port)
{
    uint32      base_port = 0, fpbm = 0;
    int32       i;

    if (fiber_pmask[unit] == NULL)
    {
        fiber_pmask[unit] = osal_alloc(sizeof(rtk_portmask_t));
        if (fiber_pmask[unit] != NULL)
            osal_memset(fiber_pmask[unit], 0, sizeof(rtk_portmask_t));
        else
            RT_ERR(RT_ERR_FAILED, (MOD_HAL), "portmask memory allocate failed of uint %u", unit);
    }

    base_port = HWP_PHY_BASE_MACID(unit, port);
    if (port == base_port)
    {
        if (fiber_pmask[unit])
        {
            phy_8208d_fiberPortBitmap_get(unit, port, &fpbm);
            for (i = 0; i < PORT_NUM_IN_8208; i++)
            {
                if (!(fpbm & (0x1 << i)))
                    continue;
                RTK_PORTMASK_PORT_SET(*fiber_pmask[unit], (base_port + i));
            }
        }
    }/* endif */

    return phy_8208_init(unit, port);
}

/* Function Name:
 *      phy_8208_greenEnable_get
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
 *      1. The RTL8208 is supported the per-chip link-up green feature.
 *      2. The link-up green feature. (Cable Length Power Saving)
 */
int32
phy_8208_greenEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret = RT_ERR_FAILED;
    uint32  base_port = 0;
    uint32  phyData;

    base_port = port - (port % PORT_NUM_IN_8208);

    /* For Link-On and Cable Length Power Saving (per-chip)
     * setting PHY 0, page 0, register 31, bit 4 (0: enabled, 1: disabled)
     */
    if ((ret = hal_miim_read(unit, base_port, 0, 31, &phyData)) != RT_ERR_OK)
        return ret;
    if (((phyData >> 4) & 0x1) == 0x0)
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;

    return RT_ERR_OK;
} /* end of phy_8208_greenEnable_get */

/* Function Name:
 *      phy_8208_greenEnable_set
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
 *      1. The RTL8208 is supported the per-chip link-up green feature.
 *      2. The link-up green feature. (Cable Length Power Saving)
 */
int32
phy_8208_greenEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret = RT_ERR_FAILED;
    uint32  base_port = 0;
    uint32  phyData;

    base_port = port - (port % PORT_NUM_IN_8208);

    /* For Link-On and Cable Length Power Saving (per-chip)
     * setting PHY 0, page 0, register 31, bit 4 (0: enabled, 1: disabled)
     */
    if ((ret = hal_miim_read(unit, base_port, 0, 31, &phyData)) != RT_ERR_OK)
        return ret;
    phyData &= ~(0x1 << 4);
    if (DISABLED == enable)
    {
        phyData |= (0x1 << 4);
    }
    if ((ret = hal_miim_write(unit, base_port, 0, 31, phyData)) != RT_ERR_OK)
        return ret;

    return ret;
} /* end of phy_8208_greenEnable_set */


#if !defined(__BOOTLOADER__)

/* Function Name:
 *      phy_8208_autoNegoAbility_get
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
phy_8208_autoNegoAbility_get(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility)
{
    int32   ret;
    uint32  phyData4;
    rtk_enable_t     enable;

    phy_common_autoNegoEnable_get(unit, port, &enable);

    /* get value from CHIP*/
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_AN_ADVERTISEMENT_REG, &phyData4)) != RT_ERR_OK)
        return ret;

    pAbility->FC = (phyData4 & Pause_R4_MASK) >> Pause_R4_OFFSET;
    pAbility->AsyFC = (phyData4 & AsymmetricPause_R4_MASK) >> AsymmetricPause_R4_OFFSET;


    pAbility->Full_100= (phyData4 & _100Base_TX_FD_R4_MASK) >> _100Base_TX_FD_R4_OFFSET;
    pAbility->Half_100= (phyData4 & _100Base_TX_R4_MASK) >> _100Base_TX_R4_OFFSET;
    pAbility->Full_10= (phyData4 & _10Base_T_FD_R4_MASK) >> _10Base_T_FD_R4_OFFSET;
    pAbility->Half_10= (phyData4 & _10Base_T_R4_MASK) >> _10Base_T_R4_OFFSET;

    return ret;
} /* end of phy_8208_autoNegoAbility_get */

/* Function Name:
 *      phy_8208_autoNegoAbility_set
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
phy_8208_autoNegoAbility_set(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility)
{
    int32   ret;
    uint32  phyData0, phyData4;
    rtk_enable_t     enable;

    phy_common_autoNegoEnable_get(unit, port, &enable);

    /* get value from CHIP*/
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_AN_ADVERTISEMENT_REG, &phyData4)) != RT_ERR_OK)
        return ret;

    phyData4 = phyData4 & ~(Pause_R4_MASK | AsymmetricPause_R4_MASK);
    phyData4 = phyData4
                | (pAbility->FC << Pause_R4_OFFSET)
                | (pAbility->AsyFC << AsymmetricPause_R4_OFFSET);


    phyData4 = phyData4 &
            ~(_100Base_TX_FD_R4_MASK | _100Base_TX_R4_MASK | _10Base_T_FD_R4_MASK | _10Base_T_R4_MASK);
    phyData4 = phyData4
            | (pAbility->Full_100 << _100Base_TX_FD_R4_OFFSET)
            | (pAbility->Half_100 << _100Base_TX_R4_OFFSET)
            | (pAbility->Full_10 << _10Base_T_FD_R4_OFFSET)
            | (pAbility->Half_10 << _10Base_T_R4_OFFSET);


    if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_AN_ADVERTISEMENT_REG, phyData4)) != RT_ERR_OK)
        return ret;

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

    phy_common_autoNegoEnable_get(unit, port, &enable);
    if(DISABLED == enable)
    {
        phy_common_enable_get(unit, port, &enable);

        if(ENABLED == enable)
        {
            /* Turn off and then turn on the power of port so the partner could detect the speed/duplex change */
            if ((ret = phy_common_enable_set(unit, port, DISABLED)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_HAL|MOD_PHY), "");
                return ret;
            }

            osal_time_usleep(200000);

            if ((ret = phy_common_enable_set(unit, port, ENABLED)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_HAL|MOD_PHY), "");
                return ret;
            }
        }
    }
    return ret;
} /* end of phy_8208_autoNegoAbility_set */


/* Function Name:
 *      phy_8208_speed_get
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
phy_8208_speed_get(uint32 unit, rtk_port_t port, rtk_port_speed_t *pSpeed)
{
    int32   ret;
    uint32  phyData0;

    /* get value from CHIP*/
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &phyData0)) != RT_ERR_OK)
        return ret;

    *pSpeed = (phyData0 & SpeedSelection0_MASK) >> SpeedSelection0_OFFSET;

    return ret;
} /* end of phy_8208_speed_get */


/* Function Name:
 *      phy_8208_speed_set
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
phy_8208_speed_set(uint32 unit, rtk_port_t port, rtk_port_speed_t speed)
{
    int32   ret;
    uint32  phyData0;

    if (PHY_8208D_FIBER_PORT(unit, port))
    {
        if (speed != PORT_SPEED_100M)
        {
            return RT_ERR_PHY_SPEED;
        }
    }

    /* get value from CHIP*/
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &phyData0)) != RT_ERR_OK)
        return ret;

    phyData0 = phyData0 & ~(SpeedSelection1_MASK | SpeedSelection0_MASK);
    phyData0 = phyData0 | ((speed & 1) << SpeedSelection0_OFFSET);

    if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, phyData0)) != RT_ERR_OK)
        return ret;

    return ret;
} /* end of phy_8208_speed_set */


/* Function Name:
 *      phy_8208_rtctResult_get
 * Description:
 *      Get test result of RTCT.
 * Input:
 *      unit        - unit id
 *      port        - the port for retriving RTCT test result
 * Output:
 *      pRtctResult - RTCT result
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_PHY_RTCT_NOT_FINISH - RTCT not finish. Need to wait a while.
 * Note:
 *      None
 */
int32
phy_8208_rtctResult_get(uint32 unit, rtk_port_t port, rtk_rtctResult_t *pRtctResult)
{
    if (0 == pPhy_rtctInfo[unit]->flag_completed[port])
        return RT_ERR_PHY_RTCT_NOT_FINISH;

    pRtctResult->linkType = PORT_SPEED_100M;
    pRtctResult->un.fe_result.isRxShort = pPhy_rtctInfo[unit]->isRxShort[port];
    pRtctResult->un.fe_result.isTxShort = pPhy_rtctInfo[unit]->isTxShort[port];
    pRtctResult->un.fe_result.isRxOpen = pPhy_rtctInfo[unit]->isRxOpen[port];
    pRtctResult->un.fe_result.isTxOpen = pPhy_rtctInfo[unit]->isTxOpen[port];
    pRtctResult->un.fe_result.isRxMismatch = 0;
    pRtctResult->un.fe_result.isTxMismatch = 0;
    pRtctResult->un.fe_result.isRxLinedriver = 0;
    pRtctResult->un.fe_result.isTxLinedriver = 0;
    /* Length = Index/4 (m) = (25) * Index (cm) */
    pRtctResult->un.fe_result.rxLen = pPhy_rtctInfo[unit]->rxLen[port] * 25;
    pRtctResult->un.fe_result.txLen = pPhy_rtctInfo[unit]->txLen[port] * 25;

    return RT_ERR_OK;
} /* end of phy_8208_rtctResult_get */

/* Function Name:
 *      phy_8208_rtct_start
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
 *      RT_ERR_PHY_RTCT_TIMEOUT - PHY RTCT timeout
 * Note:
 *      None
 */
int32
phy_8208_rtct_start(uint32 unit, rtk_port_t port)
{
    int32   ret = RT_ERR_FAILED;
    uint32  base_port = 0;
    uint32  phyData = 0, i;

    /* Reset the shadow database before start rtct */
    pPhy_rtctInfo[unit]->flag_completed[port] = 0;
    pPhy_rtctInfo[unit]->isRxShort[port] = 0;
    pPhy_rtctInfo[unit]->isTxShort[port] = 0;
    pPhy_rtctInfo[unit]->isRxOpen[port] = 0;
    pPhy_rtctInfo[unit]->isTxOpen[port] = 0;
    pPhy_rtctInfo[unit]->rxLen[port] = 0;
    pPhy_rtctInfo[unit]->txLen[port] = 0;

    base_port = port - (port % 8);
    /* Cable test on Rx pair & record result */
    if ((ret = hal_miim_write(unit, base_port+1, PHY_PAGE_0, 29, 0x17d0)) != RT_ERR_OK)
        return ret;

    phyData = ((port % 8) << 13) | 0x0780;
    if ((ret = hal_miim_write(unit, base_port+1, PHY_PAGE_0, 24, phyData)) != RT_ERR_OK)
        return ret;

    osal_time_udelay(100 * 1000);
    phyData = ((port % 8) << 13) | 0x0f80;
    if ((ret = hal_miim_write(unit, base_port+1, PHY_PAGE_0, 24, phyData)) != RT_ERR_OK)
        return ret;

    for (i = 0; i < RTCT_CHECKBUSY_TIMES; i++)
    {
        osal_time_udelay(500 * 1000);
        /* Need to check READY bit */
        if ((ret = hal_miim_read(unit, base_port+1, PHY_PAGE_0, 30, &phyData)) != RT_ERR_OK)
            return ret;
        if (phyData & 0x8000)
            break;
    }

    if (RTCT_CHECKBUSY_TIMES == i)
        return RT_ERR_PHY_RTCT_TIMEOUT;

    switch ((phyData >> 12) &0x3)
    {
        case 0:
            pPhy_rtctInfo[unit]->isRxOpen[port] = 0;
            pPhy_rtctInfo[unit]->isRxShort[port] = 0;
            break;
        case 1:
            pPhy_rtctInfo[unit]->isRxOpen[port] = 1;
            break;
        case 2:
            pPhy_rtctInfo[unit]->isRxShort[port] = 1;
            break;
    }
    pPhy_rtctInfo[unit]->rxLen[port] = phyData & 0x1FF;

    /* Cable test on Tx pair & record result */
    if ((ret = hal_miim_write(unit, base_port+1, PHY_PAGE_0, 29, 0x57d0)) != RT_ERR_OK)
        return ret;

    phyData = ((port % 8) << 13) | 0x0780;
    if ((ret = hal_miim_write(unit, base_port+1, PHY_PAGE_0, 24, phyData)) != RT_ERR_OK)
        return ret;

    osal_time_udelay(100 * 1000);
    phyData = ((port % 8) << 13) | 0x0f80;
    if ((ret = hal_miim_write(unit, base_port+1, PHY_PAGE_0, 24, phyData)) != RT_ERR_OK)
        return ret;

    for (i = 0; i < RTCT_CHECKBUSY_TIMES; i++)
    {
        osal_time_udelay(500 * 1000);
        /* Need to check READY bit */
        if ((ret = hal_miim_read(unit, base_port+1, PHY_PAGE_0, 30, &phyData)) != RT_ERR_OK)
            return ret;
        if (phyData & 0x8000)
            break;
    }

    if (RTCT_CHECKBUSY_TIMES == i)
        return RT_ERR_PHY_RTCT_TIMEOUT;

    switch ((phyData >> 12) &0x3)
    {
        case 0:
            pPhy_rtctInfo[unit]->isTxOpen[port] = 0;
            pPhy_rtctInfo[unit]->isTxShort[port] = 0;
            break;
        case 1:
            pPhy_rtctInfo[unit]->isTxOpen[port] = 1;
            break;
        case 2:
            pPhy_rtctInfo[unit]->isTxShort[port] = 1;
            break;
    }
    pPhy_rtctInfo[unit]->txLen[port] = phyData & 0x1FF;

    phyData = ((port % 8) << 13) | 0x0780;
    if ((ret = hal_miim_write(unit, base_port+1, PHY_PAGE_0, 24, phyData)) != RT_ERR_OK)
        return ret;

    pPhy_rtctInfo[unit]->flag_completed[port] = 1;
    return ret;
} /* end of phy_8208_rtct_start */

/* Function Name:
 *      phy_8208d_rtctResult_get
 * Description:
 *      Get test result of RTCT.
 * Input:
 *      unit        - unit id
 *      port        - the port for retriving RTCT test result
 * Output:
 *      pRtctResult - RTCT result
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_PHY_RTCT_NOT_FINISH - RTCT not finish. Need to wait a while.
 * Note:
 *      None
 */
int32
phy_8208d_rtctResult_get(uint32 unit, rtk_port_t port, rtk_rtctResult_t *pRtctResult)
{
    uint32  value = 0;
    int32   ret;
    uint32  phyData, minus_cable_len;
    uint32  speed;
    uint32  temp_sum;
    uint32  loop_no;
    uint32  max,min;

    if (PHY_8208D_FIBER_PORT(unit, port))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    /* Check the port is link up or not? */
    ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_STATUS_REG, &phyData);
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_STATUS_REG, &phyData)) != RT_ERR_OK)
        return ret;

    phy_common_speed_get(unit, port, &speed);

    if ((phyData & LinkStatus_MASK) && speed != PORT_SPEED_10M)
    {
        if ((ret = hal_miim_read(unit, port, 0, 17, &value)) != RT_ERR_OK)
            return ret;
        if (!(value & 0x000F))
        {
            return RT_ERR_PHY_RTCT_NOT_FINISH;
        }

        /*Read 10 times, remove Max and Min Value, and calculate the average value*/
        {
            temp_sum = 0 ;
            max = 0;
            min = 0x10000;
            for(loop_no = 0; loop_no<10; loop_no++)
            {
               if ((ret = hal_miim_read(unit, port, 0, 29, &value)) != RT_ERR_OK)
                    return ret;

               if(value > max)
                 max = value;

               if(value < min)
                 min = value;

               temp_sum += value;
             }
             temp_sum = temp_sum - (min+max);
             value = temp_sum/8;

        }

        pRtctResult->linkType = PORT_SPEED_100M;
        pRtctResult->un.fe_result.isRxLinedriver = 0;
        pRtctResult->un.fe_result.isTxLinedriver = 0;
        pRtctResult->un.fe_result.isRxMismatch = 0;
        pRtctResult->un.fe_result.isTxMismatch = 0;
        pRtctResult->un.fe_result.isRxShort = 0;
        pRtctResult->un.fe_result.isTxShort = 0;
        pRtctResult->un.fe_result.isRxOpen = 0;
        pRtctResult->un.fe_result.isTxOpen = 0;

        minus_cable_len = 573*(value & 0xFF)/5;
        if (minus_cable_len > 130*100)
            pRtctResult->un.fe_result.rxLen = 0;
        else
            pRtctResult->un.fe_result.rxLen = 130*100 - minus_cable_len;
        pRtctResult->un.fe_result.txLen = pRtctResult->un.fe_result.rxLen;
    }
    else
    {
        if ((ret = hal_miim_read(unit, port, 3, 19, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        if (!(value & 0x0100))
        {
            return RT_ERR_PHY_RTCT_NOT_FINISH;
        }

        pRtctResult->linkType = PORT_SPEED_100M;
        pRtctResult->un.fe_result.isRxLinedriver = (value & 0x0001);
        pRtctResult->un.fe_result.isTxLinedriver = (value & 0x0010);
        pRtctResult->un.fe_result.isRxMismatch = (value & 0x0002);
        pRtctResult->un.fe_result.isTxMismatch = (value & 0x0020);
        pRtctResult->un.fe_result.isRxShort = (value & 0x0004);
        pRtctResult->un.fe_result.isTxShort = (value & 0x0040);
        pRtctResult->un.fe_result.isRxOpen = (value & 0x0008);
        pRtctResult->un.fe_result.isTxOpen = (value & 0x0080);

        if ((ret = hal_miim_read(unit, port, 3, 20, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        pRtctResult->un.fe_result.rxLen = 5*(value & 0xFFF);

        if ((ret = hal_miim_read(unit, port, 3, 21, &value)) != RT_ERR_OK)
        {
            return ret;
        }
        pRtctResult->un.fe_result.txLen = 5*(value & 0xFFF);
    }

    return RT_ERR_OK;
} /* end of phy_8208d_rtctResult_get */

/* Function Name:
 *      phy_8208d_rtct_start
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
 *      RT_ERR_PHY_RTCT_TIMEOUT - PHY RTCT timeout
 * Note:
 *      None
 */
int32
phy_8208d_rtct_start(uint32 unit, rtk_port_t port)
{
    int32   ret;
    uint32  i, value = 0;
    uint32  phyData;
    uint32  speed;

    if (PHY_8208D_FIBER_PORT(unit, port))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    /* Check the port is link up or not? */
    ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_STATUS_REG, &phyData);
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_STATUS_REG, &phyData)) != RT_ERR_OK)
        return ret;

    phy_common_speed_get(unit, port, &speed);

    if ((phyData & LinkStatus_MASK) && speed != PORT_SPEED_10M)
    {
        if ((ret = hal_miim_read(unit, port, 0, 17, &value)) != RT_ERR_OK)
            return ret;
        value &= ~(0xF);
        value |= (0x8);
        if ((ret = hal_miim_write(unit, port, 0, 17, value)) != RT_ERR_OK)
            return ret;

        if ((ret = hal_miim_write(unit, port, 0, 25, 0x0964)) != RT_ERR_OK)
            return ret;
    }
    else
    {
        if ((ret = hal_miim_write(unit, port, 2, 16, 0x2C78)) != RT_ERR_OK)
            return ret;
        if ((ret = hal_miim_write(unit, port, 2, 18, 0x988A)) != RT_ERR_OK)
            return ret;
        if ((ret = hal_miim_write(unit, port, 2, 19, 0x38C8)) != RT_ERR_OK)
            return ret;
        if ((ret = hal_miim_write(unit, port, 2, 20, 0x185E)) != RT_ERR_OK)
            return ret;
        if ((ret = hal_miim_write(unit, port, 2, 21, 0x81B3)) != RT_ERR_OK)
            return ret;
        if ((ret = hal_miim_write(unit, port, 2, 22, 0x60FA)) != RT_ERR_OK)
            return ret;
        if ((ret = hal_miim_write(unit, port, 2, 23, 0xE05A)) != RT_ERR_OK)
            return ret;
        if ((ret = hal_miim_write(unit, port, 2, 24, 0x2200)) != RT_ERR_OK)
            return ret;
        if ((ret = hal_miim_write(unit, port, 2, 25, 0x042B)) != RT_ERR_OK)
            return ret;
        if ((ret = hal_miim_write(unit, port, 2, 26, 0x3427)) != RT_ERR_OK)
            return ret;

        /* Enable RTCT */
        if ((ret = hal_miim_read(unit, port, 3, 19, &value)) != RT_ERR_OK)
            return ret;
        value |= (1<<9);
        if ((ret = hal_miim_write(unit, port, 3, 19, value)) != RT_ERR_OK)
            return ret;

        osal_time_udelay(100 * 1000);

        /* Wait ready RTCT */
        for (i = 0; i < RTCT_CHECKBUSY_TIMES; i++)
        {
            /* Need to check READY bit */
            if ((ret = hal_miim_read(unit, port, 3, 19, &value)) != RT_ERR_OK)
                return ret;
            if (value & 0x0100)
                break;
            osal_time_udelay(500 * 1000);
        }

        if (RTCT_CHECKBUSY_TIMES == i)
            return RT_ERR_PHY_RTCT_TIMEOUT;
    }

    return RT_ERR_OK;
} /* end of phy_8208d_rtct_start */

/* Function Name:
 *      phy_int8208d_rtct_start
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
 *      RT_ERR_PHY_RTCT_TIMEOUT - PHY RTCT timeout
 * Note:
 *      None
 */
int32
phy_int8208d_rtct_start(uint32 unit, rtk_port_t port)
{
    int32   ret = RT_ERR_FAILED;
    uint32  i, value = 0;

    if ((ret = hal_miim_write(unit, port, 2, 16, 0x2C78)) != RT_ERR_OK)
        return ret;
    if ((ret = hal_miim_write(unit, port, 2, 18, 0x9878)) != RT_ERR_OK)
        return ret;
    if ((ret = hal_miim_write(unit, port, 2, 19, 0x30BE)) != RT_ERR_OK)
        return ret;
    if ((ret = hal_miim_write(unit, port, 2, 20, 0x184E)) != RT_ERR_OK)
        return ret;
    if ((ret = hal_miim_write(unit, port, 2, 21, 0x81C2)) != RT_ERR_OK)
        return ret;
    if ((ret = hal_miim_write(unit, port, 2, 22, 0x60E1)) != RT_ERR_OK)
        return ret;
    if ((ret = hal_miim_write(unit, port, 2, 23, 0xE078)) != RT_ERR_OK)
        return ret;
    if ((ret = hal_miim_write(unit, port, 2, 24, 0x223C)) != RT_ERR_OK)
        return ret;
    if ((ret = hal_miim_write(unit, port, 2, 25, 0x042C)) != RT_ERR_OK)
        return ret;
    if ((ret = hal_miim_write(unit, port, 2, 26, 0x3427)) != RT_ERR_OK)
        return ret;

    /* Enable RTCT */
    if ((ret = hal_miim_read(unit, port, 3, 19, &value)) != RT_ERR_OK)
        return ret;
    value |= (1<<9);
    if ((ret = hal_miim_write(unit, port, 3, 19, value)) != RT_ERR_OK)
        return ret;

    osal_time_udelay(100 * 1000);

    /* Wait ready RTCT */
    for (i = 0; i < RTCT_CHECKBUSY_TIMES; i++)
    {
        /* Need to check READY bit */
        if ((ret = hal_miim_read(unit, port, 3, 19, &value)) != RT_ERR_OK)
            return ret;
        if (value & 0x0100)
            break;
        osal_time_udelay(500 * 1000);
    }

    if (RTCT_CHECKBUSY_TIMES == i)
        return RT_ERR_PHY_RTCT_TIMEOUT;

    return RT_ERR_OK;
} /* end of phy_int8208d_rtct_start */

/* Function Name:
 *      phy_8208d_eeeEnable_get
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
phy_8208d_eeeEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  phyData;

    if (PHY_8208D_FIBER_PORT(unit, port))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    /* get value from CHIP*/
    if ((ret = hal_miim_read(unit, port, 4, 16, &phyData)) != RT_ERR_OK)
        return ret;

    if ((phyData >> 12) & 0x3)
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;

    return ret;
} /* end of phy_8208d_eeeEnable_get */

/* Function Name:
 *      phy_8208d_eeeEnable_set
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
phy_8208d_eeeEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    uint32  phyData0, value, eeeVal;
    int32   ret = RT_ERR_FAILED;
    rtk_enable_t     an_enable;

    if (PHY_8208D_FIBER_PORT(unit, port))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    phy_common_autoNegoEnable_get(unit, port, &an_enable);

    if (ENABLED == enable)
    {
        if ((ret = hal_miim_read(unit, port, 4, 16, &value)) != RT_ERR_OK)
            return ret;
        value |= (0x3 << 12); /* bit13: 10M EEE, bit12: 100M EEE n-way ability */
        if ((ret = hal_miim_write(unit, port, 4, 16, value)) != RT_ERR_OK)
            return ret;

        eeeVal = 0x2;
    }
    else
    {
        if ((ret = hal_miim_read(unit, port, 4, 16, &value)) != RT_ERR_OK)
            return ret;
        value &= ~(0x3 << 12); /* bit13: 10M EEE, bit12: 100M EEE n-way ability */
        if ((ret = hal_miim_write(unit, port, 4, 16, value)) != RT_ERR_OK)
            return ret;

        eeeVal = 0x0;
    }

    /* EEE ability */
    if ((ret = hal_miim_write(unit, port, 0x1fff, 31, 0x0)) != RT_ERR_OK)
        return ret;

    if ((ret = hal_miim_write(unit, port, 0x1fff, 13, 0x7)) != RT_ERR_OK)
        return ret;

    if ((ret = hal_miim_write(unit, port, 0x1fff, 14, 0x3c)) != RT_ERR_OK)
        return ret;

    if ((ret = hal_miim_write(unit, port, 0x1fff, 13, 0x4007)) != RT_ERR_OK)
        return ret;

    if ((ret = hal_miim_write(unit, port, 0x1fff, 14, eeeVal)) != RT_ERR_OK)
        return ret;

    if ((ret = hal_miim_write(unit, port, 0x1fff, 13, 0x0)) != RT_ERR_OK)
        return ret;

    /* Force re-autonegotiation if AN is on*/
    if (ENABLED == an_enable)
    {
        if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &phyData0)) != RT_ERR_OK)
            return ret;

        phyData0 = phyData0 & ~(RestartAutoNegotiation_MASK);
        phyData0 = phyData0 | (an_enable << RestartAutoNegotiation_OFFSET);

        if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, phyData0)) != RT_ERR_OK)
            return ret;
    }

    return ret;
} /* end of phy_8208d_eeeEnable_set */

/* Function Name:
 *      phy_8208_crossOverMode_get
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
 *      1) Following value is valid
 *      - PORT_CROSSOVER_MODE_AUTO
 *      - PORT_CROSSOVER_MODE_MDI
 *      - PORT_CROSSOVER_MODE_MDIX
 *      2) The function is per-chip based, not per-port based.
 */
int32
phy_8208_crossOverMode_get(uint32 unit, rtk_port_t port, rtk_port_crossOver_mode_t *pMode)
{
    int32   ret;
    uint32  phyData, auto_mode, mdi;
    uint32  base_port = 0;

    /* get value from CHIP*/
    base_port = port - (port % 8);
    if ((ret = hal_miim_read(unit, base_port, 0, 0x1f, &phyData)) != RT_ERR_OK)
        return ret;
    auto_mode = (phyData >> 7) & 0x1;
    mdi = (phyData >> 6) & 0x1;

    if (auto_mode)
        *pMode = PORT_CROSSOVER_MODE_AUTO;
    else
    {
        if (mdi)
            *pMode = PORT_CROSSOVER_MODE_MDI;
        else
            *pMode = PORT_CROSSOVER_MODE_MDIX;
    }

    return ret;
} /* end of phy_8208_crossOverMode_get */

/* Function Name:
 *      phy_8208_crossOverMode_set
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
 *      1) Following value is valid
 *      - PORT_CROSSOVER_MODE_AUTO
 *      - PORT_CROSSOVER_MODE_MDI
 *      - PORT_CROSSOVER_MODE_MDIX
 *      2) The function is per-chip based, not per-port based.
 *         The operation will apply to all ports of the chip.
 */
int32
phy_8208_crossOverMode_set(uint32 unit, rtk_port_t port, rtk_port_crossOver_mode_t mode)
{
    int32   ret;
    uint32  phyData, auto_mode, mdi;
    uint32  base_port = 0;

    base_port = port - (port % 8);
    switch (mode)
    {
        case PORT_CROSSOVER_MODE_AUTO:
            auto_mode = 1;
            mdi = 1;
            break;
        case PORT_CROSSOVER_MODE_MDI:
            auto_mode = 0;
            mdi = 1;
            break;
        case PORT_CROSSOVER_MODE_MDIX:
            auto_mode = 0;
            mdi = 0;
            break;
        default:
            return RT_ERR_INPUT;
    }

    /* get value from CHIP*/
    if ((ret = hal_miim_read(unit, base_port, 0, 0x1f, &phyData)) != RT_ERR_OK)
        return ret;
    phyData &= ~(3 << 6);
    phyData |= (auto_mode << 7);
    phyData |= (mdi << 6);
    if ((ret = hal_miim_write(unit, base_port, 0, 0x1f, phyData)) != RT_ERR_OK)
        return ret;

    return ret;
} /* end of phy_8208_crossOverMode_set */

/* Function Name:
 *      phy_8208d_crossOverMode_get
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
phy_8208d_crossOverMode_get(uint32 unit, rtk_port_t port, rtk_port_crossOver_mode_t *pMode)
{
    int32   ret;
    uint32  phyData, auto_mode, mdix;

    if (PHY_8208D_FIBER_PORT(unit, port))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    /* get value from CHIP*/
    if ((ret = hal_miim_read(unit, port, 0, 0x1c, &phyData)) != RT_ERR_OK)
        return ret;
    auto_mode = (phyData >> 2) & 0x1;
    mdix = (phyData >> 1) & 0x1;

    if (auto_mode)
        *pMode = PORT_CROSSOVER_MODE_AUTO;
    else
    {
        if (mdix)
            *pMode = PORT_CROSSOVER_MODE_MDIX;
        else
            *pMode = PORT_CROSSOVER_MODE_MDI;
    }

    return ret;
} /* end of phy_8208d_crossOverMode_get */

/* Function Name:
 *      phy_8208d_crossOverMode_set
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
phy_8208d_crossOverMode_set(uint32 unit, rtk_port_t port, rtk_port_crossOver_mode_t mode)
{
    int32   ret;
    uint32  phyData, auto_mode, mdix;

    if (PHY_8208D_FIBER_PORT(unit, port))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    switch (mode)
    {
        case PORT_CROSSOVER_MODE_AUTO:
            auto_mode = 1;
            mdix = 1;
            break;
        case PORT_CROSSOVER_MODE_MDI:
            auto_mode = 0;
            mdix = 0;
            break;
        case PORT_CROSSOVER_MODE_MDIX:
            auto_mode = 0;
            mdix = 1;
            break;
        default:
            return RT_ERR_INPUT;
    }

    /* get value from CHIP*/
    if ((ret = hal_miim_read(unit, port, 0, 0x1c, &phyData)) != RT_ERR_OK)
        return ret;
    phyData &= ~(3 << 1);
    phyData |= (auto_mode << 2);
    phyData |= (mdix << 1);
    if ((ret = hal_miim_write(unit, port, 0, 0x1c, phyData)) != RT_ERR_OK)
        return ret;

    return ret;
} /* end of phy_8208d_crossOverMode_set */

/* Function Name:
 *      phy_8208_linkDownPowerSavingEnable_get
 * Description:
 *      Get the status of link-down power saving of the specific port in the specific unit
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      pEnable - pointer to status of link-down power saving
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 * Note:
 *      1. The RTL8208 is supported the per-chip link-down power saving
 */
int32
phy_8208_linkDownPowerSavingEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret = RT_ERR_FAILED;
    uint32  base_port = 0;
    uint32  phyData;

    base_port = port - (port % PORT_NUM_IN_8208);

    /* read PHY 1, page 0, register 22, bit 3 (0: enabled, 1: disabled) */
    if ((ret = hal_miim_read(unit, base_port + 1, 0, 22, &phyData)) != RT_ERR_OK)
        return ret;
    if (((phyData >> 3) & 0x1) == 0x0)
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;

    return RT_ERR_OK;
} /* end of phy_8208_linkDownPowerSavingEnable_get */

/* Function Name:
 *      phy_8208_linkDownPowerSavingEnable_set
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
 *      1. The RTL8208 is supported the per-chip link-down power saving
 */
int32
phy_8208_linkDownPowerSavingEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret = RT_ERR_FAILED;
    uint32  base_port = 0;
    uint32  phyData;

    base_port = port - (port % PORT_NUM_IN_8208);

    /* For Link-Down Power Saving (per-chip)
     * setting PHY 1, page 0, register 22, bit 3 (0: enabled, 1: disabled)
     */
    if ((ret = hal_miim_read(unit, base_port + 1, 0, 22, &phyData)) != RT_ERR_OK)
        return ret;
    phyData &= ~(0x1 << 3);
    if (DISABLED == enable)
    {
        phyData |= (0x1 << 3);
    }
    if ((ret = hal_miim_write(unit, base_port + 1, 0, 22, phyData)) != RT_ERR_OK)
        return ret;

    return ret;
} /* end of phy_8208_linkDownPowerSavingEnable_set */

/* Function Name:
 *      phy_8208_loopback_set
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
 *      None
 */
int32
phy_8208_loopback_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32               ret;
    rtk_enable_t        prevEnable;
    uint32              data;

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
                RTK_PORTMASK_PORT_SET(*autoNegoEnablePortMask[unit], port);
            }
            else
            {
                RTK_PORTMASK_PORT_CLEAR(*autoNegoEnablePortMask[unit], port);
            }
        }
        else /* Enable to Disable */
        {
            if (RTK_PORTMASK_IS_PORT_SET(*autoNegoEnablePortMask[unit], port))
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

    return ret;
}

/* Function Name:
 *      phy_8208d_linkDownPowerSavingEnable_get
 * Description:
 *      Get the status of link-down power saving of the specific port in the specific unit
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      pEnable - pointer to status of link-down power saving
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 * Note:
 *      1. The RTL8208d is supported the per-port link-down power saving
 */
int32
phy_8208d_linkDownPowerSavingEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret = RT_ERR_FAILED;
    uint32  phyData;

    if (PHY_8208D_FIBER_PORT(unit, port))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    /* read page 0, register 24, bit 15 (1: enabled, 0: disabled) */
    if ((ret = hal_miim_read(unit, port, 0, 24, &phyData)) != RT_ERR_OK)
        return ret;
    if ((phyData >> 15) & 0x1)
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;

    return RT_ERR_OK;
} /* end of phy_8208d_linkDownPowerSavingEnable_get */

/* Function Name:
 *      phy_8208d_linkDownPowerSavingEnable_set
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
 *      1. The RTL8208d is supported the per-port link-down power saving
 */
int32
phy_8208d_linkDownPowerSavingEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret = RT_ERR_FAILED;
    uint32  phyData;

    if (PHY_8208D_FIBER_PORT(unit, port))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    /* For Link-Down Power Saving (per-port)
     * setting page 0, register 24, bit 15 (1: enabled, 0: disabled)
     */
    if ((ret = hal_miim_read(unit, port, 0, 24, &phyData)) != RT_ERR_OK)
        return ret;
    phyData &= ~(0x1 << 15);
    if (ENABLED == enable)
    {
        phyData |= (0x1 << 15);
    }
    if ((ret = hal_miim_write(unit, port, 0, 24, phyData)) != RT_ERR_OK)
        return ret;

    return ret;
} /* end of phy_8208d_linkDownPowerSavingEnable_set */

/* Function Name:
 *      phy_8208d_gigaLiteEnable_get
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
 *      1. The RTL8208d is not supported the per-port Giga Lite feature.
 */
int32
phy_8208d_gigaLiteEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    *pEnable = DISABLED;
    return RT_ERR_OK;
} /* end of phy_8208d_gigaLiteEnable_get */


/* Function Name:
 *      phy_8208_media_get
 * Description:
 *      Get PHY 8208 media type.
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
 *      1. media type is PORT_MEDIA_COPPER
 */
int32
phy_8208_media_get(uint32 unit, rtk_port_t port, rtk_port_media_t *pMedia)
{
    RT_PARAM_CHK((NULL == pMedia), RT_ERR_NULL_POINTER);

    *pMedia = PORT_MEDIA_COPPER;
    return RT_ERR_OK;
} /* end of phy_8208_media_get */

/* Function Name:
 *      phy_8208d_media_get
 * Description:
 *      Get PHY 8208D media type.
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
phy_8208d_media_get(uint32 unit, rtk_port_t port, rtk_port_media_t *pMedia)
{
    uint32  val;
    int32   ret = RT_ERR_FAILED;

    RT_PARAM_CHK((NULL == pMedia), RT_ERR_NULL_POINTER);

    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, 28, &val)) != RT_ERR_OK)
        return ret;

    if ((val >> 5) & 0x1)
        *pMedia = PORT_MEDIA_FIBER;
    else
        *pMedia = PORT_MEDIA_COPPER;

    return RT_ERR_OK;
} /* end of phy_8208d_media_get */

/* Function Name:
 *      phy_8208d_media_set
 * Description:
 *      Get PHY 8208D media type.
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
 *      (1) media type is PORT_MEDIA_COPPER or PORT_MEDIA_FIBER
 */
int32
phy_8208d_media_set(uint32 unit, rtk_port_t port, rtk_port_media_t media)
{
    uint32  val;
    int32   ret = RT_ERR_FAILED;

    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, 28, &val)) != RT_ERR_OK)
        return ret;

    switch (media)
    {
        case PORT_MEDIA_COPPER:
            val &= ~(1<<5);
            break; /* end of switch-case PORT_MEDIA_COPPER */

        case PORT_MEDIA_FIBER:
            val |= (1<<5);
            break; /* end of switch-case PORT_MEDIA_FIBER */

        default:
            return RT_ERR_PORT_NOT_SUPPORTED;
    }

    if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, 28, val)) != RT_ERR_OK)
        return ret;

    if (fiber_pmask[unit] != NULL)
    {
        if (media == PORT_MEDIA_COPPER)
        {
            RTK_PORTMASK_PORT_CLEAR(*fiber_pmask[unit], port);
        }
        else if (media == PORT_MEDIA_FIBER)
        {
            RTK_PORTMASK_PORT_SET(*fiber_pmask[unit], port);
        }
    }


    return RT_ERR_OK;
} /* end of phy_8208d_media_set */


/* Function Name:
 *      phy_8208d_autoNegoEnable_set
 * Description:
 *      Set autonegotiation enable status of the specific port
 * Input:
 *      unit - unit id
 *      port - port id
 *      enable - enable or disable autonegotiation
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
phy_8208d_autoNegoEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{

    if (PHY_8208D_FIBER_PORT(unit, port))
    {
        if (enable != DISABLED)
        {
            return RT_ERR_CHIP_NOT_SUPPORTED;
        }
    }

    return phy_common_autoNegoEnable_set(unit, port, enable);

}

/* Function Name:
 *      phy_8208d_duplex_set
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
phy_8208d_duplex_set(uint32 unit, rtk_port_t port, uint32 duplex)
{

    if (PHY_8208D_FIBER_PORT(unit, port))
    {
        if (duplex != PORT_FULL_DUPLEX)
            return RT_ERR_FAILED;
    }

    return phy_common_duplex_set(unit, port, duplex);
}

/* Function Name:
 *      phy_8208d_fiberOAMLoopBackEnable_set
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
phy_8208d_fiberOAMLoopBackEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    uint32  val;
    int32   ret;


    if (!PHY_8208D_FIBER_PORT(unit, port))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    /* unidirectional enable/disable */
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, &val)) != RT_ERR_OK)
        return ret;

    if (enable == ENABLED)
        val |= (1 << 5);
    else
        val &= ~(1 << 5);

    if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, PHY_CONTROL_REG, val)) != RT_ERR_OK)
        return ret;

    /* force TX enable/disable */
    if ((ret = hal_miim_read(unit, port, PHY_PAGE_0, 22, &val)) != RT_ERR_OK)
        return ret;

    if (enable == ENABLED)
        val |= (1 << 13);
    else
        val &= ~(1 << 13);

    if ((ret = hal_miim_write(unit, port, PHY_PAGE_0, 22, val)) != RT_ERR_OK)
        return ret;

    return ret;
}
#endif /* !defined(__BOOTLOADER__) */

#ifndef __BOOTLOADER__

/* Function Name:
 *      phy_8208_patch_set
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
phy_8208_patch_set(uint32 unit, rtk_port_t port)
{
    int32 ret;
    ret = sub_phy_8208_patch_set(unit, port);
    return ret;
} /* end of phy_8208_patch_set */

#endif

