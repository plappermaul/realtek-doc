/*
 * Copyright (C) 2018 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 75479 $
 * $Date: 2017-01-20 15:17:16 +0800 (Fri, 20 Jan 2017) $
 *
 * Purpose : PHY 8284 Driver APIs.
 *
 * Feature : PHY 8284 Driver APIs
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
#include <hal/phy/phy_rtl8226.h>
#include <hal/mac/miim_common_drv.h>
#include <hal/phy/nic_rtl8226/nic_rtl8226.h>
#include <hal/phy/nic_rtl8226/nic_rtl8226b.h>

/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */
rt_phyInfo_t phy_8226_info =
{
    .phy_num    = PORT_NUM_IN_8226,
    .eth_type   = HWP_2_5GE,
    .isComboPhy = {0, 0, 0, 0, 0, 0, 0, 0},

    .flags      = (RTK_PHYINFO_FLAG_1G_MMD_CFG | RTK_PHYINFO_FLAG_NO_RES_REG),

    .xGePhyLocalDuplexAbilityDev            = 31,
    .xGePhyLocalDuplexAbilityAddr           = 0xA400,
    .xGePhyLocalDuplexAbilityBit            = 8,

    .xGePhyLocal1000MSpeedAbilityDev        = 31,
    .xGePhyLocal1000MSpeedAbilityAddr       = 0xA412,
    .xGePhyLocal1000MSpeedAbilityBit        = 9,

    .xGePhyLinkPartner1000MSpeedAbilityDev  = 31,
    .xGePhyLinkPartner1000MSpeedAbilityAddr = 0xA414,
    .xGePhyLinkPartner1000MSpeedAbilityBit  = 11,
};

rt_phyInfo_t phy_8226b_info =
{
    .phy_num    = PORT_NUM_IN_8226,
    .eth_type   = HWP_2_5GE,
    .isComboPhy = {0, 0, 0, 0, 0, 0, 0, 0},

    .flags      = (RTK_PHYINFO_FLAG_1G_MMD_CFG | RTK_PHYINFO_FLAG_NO_RES_REG),

    .xGePhyLocalDuplexAbilityDev            = 31,
    .xGePhyLocalDuplexAbilityAddr           = 0xA400,
    .xGePhyLocalDuplexAbilityBit            = 8,

    .xGePhyLocal1000MSpeedAbilityDev        = 31,
    .xGePhyLocal1000MSpeedAbilityAddr       = 0xA412,
    .xGePhyLocal1000MSpeedAbilityBit        = 9,

    .xGePhyLinkPartner1000MSpeedAbilityDev  = 31,
    .xGePhyLinkPartner1000MSpeedAbilityAddr = 0xA414,
    .xGePhyLinkPartner1000MSpeedAbilityBit  = 11,
};


/*
 * Macro Declaration
 */
/* error code convert */
#define RTL8226_EC_CONV(fn, ret)    \
            do { \
                if ((fn) == SUCCESS) \
                    ret = RT_ERR_OK; \
                else \
                    ret = RT_ERR_FAILED; \
            } while (0)


/*
 * Function Declaration
 */



/* Function Name:
 *      phy_8226_init
 * Description:
 *      Initialize PHY 8226.
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
phy_8226_init(uint32 unit, rtk_port_t port)
{
    uint32  base_port;

    RT_LOG(LOG_TRACE, MOD_PHY, "U=%u P=%u", unit, port);
    if ((base_port = HWP_PHY_BASE_MACID(unit, port)) == HWP_NONE)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "unit=%u,port=%u get base port ID fail", unit, port);
        return RT_ERR_PORT_ID;
    }

    RT_LOG(LOG_TRACE, MOD_PHY, "base_port=%u", base_port);
    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8226_media_get
 * Description:
 *      Get PHY media type.
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
 *      None
 */
int32
phy_8226_media_get(uint32 unit, rtk_port_t port, rtk_port_media_t *pMedia)
{
    *pMedia = PORT_MEDIA_COPPER;
    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8226_enable_set
 * Description:
 *      Set PHY interface status of the specific port
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
phy_8226_enable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    HANDLE  hDevice;
    BOOL    drv_enable;
    int32   ret;

    RT_LOG(LOG_TRACE, MOD_PHY, "unit=%u port=%u enable=%u\n", unit, port, enable);

    hDevice.unit = unit;
    hDevice.port = port;

    if (enable == ENABLED)
        drv_enable = TRUE;
    else
        drv_enable = FALSE;

    RTL8226_EC_CONV(Rtl8226_enable_set(hDevice, drv_enable), ret);

    if (ret != RT_ERR_OK)
    {
        RT_LOG(LOG_TRACE, MOD_PHY, "fail 0x%X", ret);
    }

    return ret;
}



int32
phy_8226_autoNegoEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    HANDLE  hDevice;
    BOOL    drv_enable;
    int32   ret;

    hDevice.unit = unit;
    hDevice.port = port;

    RTL8226_EC_CONV(Rtl8226_autoNegoEnable_get(hDevice, &drv_enable), ret);

    *pEnable = (drv_enable == TRUE) ? ENABLED : DISABLED;

    if (ret != RT_ERR_OK)
    {
        RT_LOG(LOG_TRACE, MOD_PHY, "fail 0x%X", ret);
    }

    return ret;
}

int32
phy_8226_autoNegoEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    HANDLE  hDevice;
    BOOL    drv_enable;
    int32   ret;

    RT_LOG(LOG_TRACE, MOD_PHY, "U=%u P=%u enable=%u", unit, port, enable);

    hDevice.unit = unit;
    hDevice.port = port;

    if (enable == ENABLED)
        drv_enable = TRUE;
    else
        drv_enable = FALSE;

    RTL8226_EC_CONV(Rtl8226_autoNegoEnable_set(hDevice, drv_enable), ret);

    if (ret != RT_ERR_OK)
    {
        RT_LOG(LOG_TRACE, MOD_PHY, "fail 0x%X", ret);
    }

    return ret;
}



/* Function Name:
 *      phy_8226_autoNegoAbility_get
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
phy_8226_autoNegoAbility_get(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility)
{
    HANDLE  hDevice;
    PHY_LINK_ABILITY    drv_phyAbility;
    int32   ret;

    hDevice.unit = unit;
    hDevice.port = port;

    osal_memset(&drv_phyAbility, 0, sizeof(PHY_LINK_ABILITY));

    RTL8226_EC_CONV(Rtl8226_autoNegoAbility_get(hDevice, &drv_phyAbility), ret);
    if (ret != RT_ERR_OK)
    {
        RT_LOG(LOG_TRACE, MOD_PHY, "fail");
        return ret;
    }

    pAbility->Half_10   = drv_phyAbility.Half_10;
    pAbility->Full_10   = drv_phyAbility.Full_10;
    pAbility->Half_100  = drv_phyAbility.Half_100;
    pAbility->Full_100  = drv_phyAbility.Full_100;
    pAbility->Full_1000 = drv_phyAbility.Full_1000;
    pAbility->adv_2_5G  = drv_phyAbility.adv_2_5G;
    pAbility->FC        = drv_phyAbility.FC;
    pAbility->AsyFC     = drv_phyAbility.AsyFC;

    return RT_ERR_OK;
}


/* Function Name:
 *      phy_8226_autoNegoAbility_set
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
phy_8226_autoNegoAbility_set(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility)
{
    HANDLE  hDevice;
    PHY_LINK_ABILITY    drv_phyAbility;
    int32   ret;

    RT_LOG(LOG_TRACE, MOD_PHY, "U=%u P=%u 10h=%u 10f=%u 100h=%u 100f=%u 1000f=%u 2.5g=%u FC=%u AsyFC=%u", unit, port,
        pAbility->Half_10, pAbility->Full_10, pAbility->Half_100, pAbility->Full_100, pAbility->Full_1000, pAbility->adv_2_5G, pAbility->FC, pAbility->AsyFC);

    hDevice.unit = unit;
    hDevice.port = port;

    osal_memset(&drv_phyAbility, 0, sizeof(PHY_LINK_ABILITY));

    drv_phyAbility.Half_10   = pAbility->Half_10;
    drv_phyAbility.Full_10   = pAbility->Full_10;
    drv_phyAbility.Half_100  = pAbility->Half_100;
    drv_phyAbility.Full_100  = pAbility->Full_100;
    drv_phyAbility.Full_1000 = pAbility->Full_1000;
    drv_phyAbility.adv_2_5G  = pAbility->adv_2_5G;
    drv_phyAbility.FC        = pAbility->FC;
    drv_phyAbility.AsyFC     = pAbility->AsyFC;


    RTL8226_EC_CONV(Rtl8226_autoNegoAbility_set(hDevice, &drv_phyAbility), ret);
    if (ret != RT_ERR_OK)
    {
        RT_LOG(LOG_TRACE, MOD_PHY, "U=%u P=%u fail", unit, port);
        return ret;
    }

    return RT_ERR_OK;

}


/* Function Name:
 *      phy_8226_duplex_get
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
phy_8226_duplex_get(uint32 unit, rtk_port_t port, uint32 *pDuplex)
{
    HANDLE  hDevice;
    BOOL    drv_enable;
    int32   ret;

    hDevice.unit = unit;
    hDevice.port = port;

    RTL8226_EC_CONV(Rtl8226_duplex_get(hDevice, &drv_enable), ret);
    if (ret != RT_ERR_OK)
    {
    RT_LOG(LOG_TRACE, MOD_PHY, "U=%u P=%u fail", unit, port);
        return ret;
    }

    *pDuplex = drv_enable;

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8226_duplex_set
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
phy_8226_duplex_set(uint32 unit, rtk_port_t port, uint32 duplex)
{
    HANDLE  hDevice;
    BOOL    drv_enable;
    int32   ret;

    RT_LOG(LOG_TRACE, MOD_PHY, "U=%u P=%u duplex=%u", unit, port, duplex);

    hDevice.unit = unit;
    hDevice.port = port;

    drv_enable = (BOOL)duplex;

    RTL8226_EC_CONV(Rtl8226_duplex_set(hDevice, drv_enable), ret);
    if (ret != RT_ERR_OK)
    {
        RT_LOG(LOG_TRACE, MOD_PHY, "U=%u P=%u fail", unit, port);
        return ret;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      phy_8226_speed_get
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
phy_8226_speed_get(uint32 unit, rtk_port_t port, rtk_port_speed_t *pSpeed)
{
    HANDLE  hDevice;
    UINT16  drv_speed;
    int32   ret;

    hDevice.unit = unit;
    hDevice.port = port;

    RTL8226_EC_CONV(Rtl8226_speed_get(hDevice, &drv_speed), ret);
    if (ret != RT_ERR_OK)
    {
        RT_LOG(LOG_TRACE, MOD_PHY, "U=%u P=%u fail", unit, port);
        return ret;
    }

    switch(drv_speed)
    {
      case LINK_SPEED_10M:
        *pSpeed = PORT_SPEED_10M;
        break;
      case LINK_SPEED_100M:
        *pSpeed = PORT_SPEED_100M;
        break;
      case LINK_SPEED_1G:
        *pSpeed = PORT_SPEED_1000M;
        break;
      case LINK_SPEED_500M:
        *pSpeed = PORT_SPEED_500M;
        break;
	  case LINK_SPEED_2P5G:
        *pSpeed = PORT_SPEED_2_5G;
        break;
      case NO_LINK:
        *pSpeed = PORT_SPEED_10M;
        break;
      default:
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8226_speed_set
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
phy_8226_speed_set(uint32 unit, rtk_port_t port, rtk_port_speed_t speed)
{
    HANDLE  hDevice;
    UINT16  drv_speed;
    int32   ret;

    hDevice.unit = unit;
    hDevice.port = port;

    switch(speed)
    {
      case PORT_SPEED_10M:
        drv_speed = 10;
        break;
      case PORT_SPEED_100M:
        drv_speed = 100;
        break;
      case PORT_SPEED_1000M:
        drv_speed = 1000;
        break;
      case PORT_SPEED_2_5G:
        drv_speed = 2500;
        break;
      default:
        return RT_ERR_INPUT;
    }

    RTL8226_EC_CONV(Rtl8226_force_speed_set(hDevice, drv_speed), ret);
    if (ret != RT_ERR_OK)
    {
        RT_LOG(LOG_TRACE, MOD_PHY, "U=%u P=%u fail", unit, port);
        return ret;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      phy_8226_crossOverMode_get
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
 * Note:
 *      Following value is valid
 *      - PORT_CROSSOVER_MODE_AUTO
 *      - PORT_CROSSOVER_MODE_MDI
 *      - PORT_CROSSOVER_MODE_MDIX
 */
int32
phy_8226_crossOverMode_get(uint32 unit, rtk_port_t port, rtk_port_crossOver_mode_t *pMode)
{
    HANDLE  hDevice;
    PHY_CROSSPVER_MODE  drv_crossOverMode;
    int32   ret;

    hDevice.unit = unit;
    hDevice.port = port;

    RTL8226_EC_CONV(Rtl8226_crossOverMode_get(hDevice, &drv_crossOverMode), ret);
    if (ret != RT_ERR_OK)
    {
        return ret;
    }

    switch (drv_crossOverMode)
    {
      case PHY_CROSSPVER_MODE_MDI:
        *pMode = PORT_CROSSOVER_MODE_MDI;
        break;
      case PHY_CROSSPVER_MODE_MDIX:
        *pMode = PORT_CROSSOVER_MODE_MDIX;
        break;
      case PHY_CROSSPVER_MODE_AUTO:
        *pMode = PORT_CROSSOVER_MODE_AUTO;
        break;
      default:
        RT_LOG(LOG_TRACE, MOD_PHY, "unit=%u port=%u mode=%u", unit, port, drv_crossOverMode);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      phy_8226_crossOverMode_set
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
 * Note:
 *      Following value is valid
 *      - PORT_CROSSOVER_MODE_AUTO
 *      - PORT_CROSSOVER_MODE_MDI
 *      - PORT_CROSSOVER_MODE_MDIX
 */
int32
phy_8226_crossOverMode_set(uint32 unit, rtk_port_t port, rtk_port_crossOver_mode_t mode)
{
    HANDLE  hDevice;
    PHY_CROSSPVER_MODE  drv_crossOverMode;
    int32   ret;

    RT_LOG(LOG_TRACE, MOD_PHY, "unit=%u port=%u mode=%u", unit, port, mode);
    hDevice.unit = unit;
    hDevice.port = port;


    switch (mode)
    {
      case PORT_CROSSOVER_MODE_MDI:
        drv_crossOverMode = PHY_CROSSPVER_MODE_MDI;
        break;
      case PORT_CROSSOVER_MODE_MDIX:
        drv_crossOverMode = PHY_CROSSPVER_MODE_MDIX;
        break;
      case PORT_CROSSOVER_MODE_AUTO:
        drv_crossOverMode = PHY_CROSSPVER_MODE_AUTO;
        break;
      default:
        RT_LOG(LOG_TRACE, MOD_PHY, "unit=%u port=%u mode=%u", unit, port, mode);
        return RT_ERR_INPUT;
    }


    RTL8226_EC_CONV(Rtl8226_crossOverMode_set(hDevice, drv_crossOverMode), ret);
    if (ret != RT_ERR_OK)
    {
        RT_LOG(LOG_TRACE, MOD_PHY, "unit=%u port=%u mode=%u fail", unit, port, drv_crossOverMode);
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8226_crossOverStatus_get
 * Description:
 *      Get cross over status in the specified port.
 * Input:
 *      unit  - unit id
 *      port  - port id
 * Output:
 *      pMode - pointer to cross over mode status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Following value is valid
 *      - PORT_CROSSOVER_STATUS_MDI
 *      - PORT_CROSSOVER_STATUS_MDIX
 */
int32
phy_8226_crossOverStatus_get(uint32 unit, rtk_port_t port, rtk_port_crossOver_status_t *pStatus)
{
    HANDLE  hDevice;
    PHY_CROSSPVER_STATUS  drv_crossOverStatus;
    int32   ret;

    hDevice.unit = unit;
    hDevice.port = port;

    RTL8226_EC_CONV(Rtl8226_crossOverStatus_get(hDevice, &drv_crossOverStatus), ret);
    if (ret != RT_ERR_OK)
    {
        RT_LOG(LOG_TRACE, MOD_PHY, "unit=%u port=%u fail", unit, port);
        return ret;
    }

    switch (drv_crossOverStatus)
    {
      case PHY_CROSSPVER_STATUS_MDI:
        *pStatus = PORT_CROSSOVER_STATUS_MDI;
        break;
      case PHY_CROSSPVER_STATUS_MDIX:
        *pStatus = PORT_CROSSOVER_STATUS_MDIX;
        break;
      default:
        RT_LOG(LOG_TRACE, MOD_PHY, "unit=%u port=%u status=%u", unit, port, drv_crossOverStatus);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      phy_8226_masterSlave_get
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
 *      RT_ERR_PORT_ID       - invalid port id
 *      RT_ERR_NULL_POINTER  - input parameter may be null pointer
 * Note:
 *      This function only works on giga/ 10g port to get its master/slave mode configuration.
 */
int32
phy_8226_masterSlave_get(
    uint32              unit,
    rtk_port_t          port,
    rtk_port_masterSlave_t   *pMasterSlaveCfg,
    rtk_port_masterSlave_t   *pMasterSlaveActual)
{
    HANDLE  hDevice;
    PHY_MASTERSLAVE_MODE  drv_masterSlaveMode;
    int32   ret;
    uint32  phydata;

    hDevice.unit = unit;
    hDevice.port = port;

    RTL8226_EC_CONV(Rtl8226_masterSlave_get(hDevice, &drv_masterSlaveMode), ret);
    if (ret != RT_ERR_OK)
    {
        RT_LOG(LOG_TRACE, MOD_PHY, "unit=%u port=%u fail", unit, port);
        return ret;
    }

    switch (drv_masterSlaveMode)
    {
      case PHY_AUTO_MODE:
        *pMasterSlaveCfg = PORT_AUTO_MODE;
        break;
      case PHY_MASTER_MODE:
        *pMasterSlaveCfg = PORT_MASTER_MODE;
        break;
      case PHY_SLAVE_MODE:
        *pMasterSlaveCfg = PORT_SLAVE_MODE;
        break;
      default:
        RT_LOG(LOG_TRACE, MOD_PHY, "unit=%u port=%u fail", unit, port);
        return RT_ERR_FAILED;
    }

    if ((ret = hal_miim_mmd_read(unit, port, MMD_AN, 33, &phydata)) != RT_ERR_OK)
    {
        RT_LOG(LOG_TRACE, MOD_PHY, "unit=%u port=%u fail 0x%X", unit, port, ret);
        return ret;
    }

    if (phydata & BIT_14)
    {
        *pMasterSlaveActual = PORT_MASTER_MODE;
    }
    else
    {
        *pMasterSlaveActual = PORT_SLAVE_MODE;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8226_masterSlave_set
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
 *      RT_ERR_PORT_ID       - invalid port id
 *      RT_ERR_INPUT         - invalid input parameter
 * Note:
 *      None
 */
int32
phy_8226_masterSlave_set(
    uint32              unit,
    rtk_port_t          port,
    rtk_port_masterSlave_t   masterSlave)
{
    HANDLE  hDevice;
    PHY_MASTERSLAVE_MODE  drv_masterSlaveMode;
    int32   ret;


    RT_LOG(LOG_TRACE, MOD_PHY, "unit=%u port=%u mode=%u", unit, port, masterSlave);
    hDevice.unit = unit;
    hDevice.port = port;

    switch (masterSlave)
    {
      case PORT_AUTO_MODE:
        drv_masterSlaveMode = PHY_AUTO_MODE;
        break;
      case PORT_MASTER_MODE:
        drv_masterSlaveMode = PHY_MASTER_MODE;
        break;
      case PORT_SLAVE_MODE:
        drv_masterSlaveMode = PHY_SLAVE_MODE;
        break;
      default:
        RT_LOG(LOG_TRACE, MOD_PHY, "unit=%u port=%u fail", unit, port);
        return RT_ERR_FAILED;
    }

    RTL8226_EC_CONV(Rtl8226_masterSlave_set(hDevice, drv_masterSlaveMode), ret);
    if (ret != RT_ERR_OK)
    {
        RT_LOG(LOG_TRACE, MOD_PHY, "unit=%u port=%u fail", unit, port);
        return ret;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      phy_8226_loopback_get
 * Description:
 *      Get PHY Loopback mode of the specific port
 * Input:
 *      unit                - unit id
 *      port                - port id
 *      pEnable           -loopback mode status;
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8226_loopback_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    HANDLE  hDevice;
    BOOL    drv_enable;
    int32   ret;

    hDevice.unit = unit;
    hDevice.port = port;

    RTL8226_EC_CONV(Rtl8226_loopback_get(hDevice, &drv_enable), ret);

    *pEnable = (drv_enable == TRUE) ? ENABLED : DISABLED;

    if (ret != RT_ERR_OK)
    {
        RT_LOG(LOG_TRACE, MOD_PHY, "fail 0x%X", ret);
    }

    return ret;
}


/* Function Name:
 *      phy_8226_loopback_set
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
phy_8226_loopback_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    HANDLE  hDevice;
    BOOL    drv_enable;
    int32   ret;

    RT_LOG(LOG_TRACE, MOD_PHY, "U=%u P=%u enable=%u", unit, port, enable);

    hDevice.unit = unit;
    hDevice.port = port;

    if (enable == ENABLED)
        drv_enable = TRUE;
    else
        drv_enable = FALSE;

    RTL8226_EC_CONV(Rtl8226_loopback_set(hDevice, drv_enable), ret);

    if (ret != RT_ERR_OK)
    {
        RT_LOG(LOG_TRACE, MOD_PHY, "fail 0x%X", ret);
    }

    return ret;

}

/* Function Name:
 *      phy_8226_gigaLiteEnable_get
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
 *      None
 */
int32
phy_8226_gigaLiteEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    HANDLE  hDevice;
    BOOL    drv_enable;
    int32   ret;

    hDevice.unit = unit;
    hDevice.port = port;

    RTL8226_EC_CONV(Rtl8226_gigaLiteEnable_get(hDevice, &drv_enable), ret);

    *pEnable = (drv_enable == TRUE) ? ENABLED : DISABLED;

    if (ret != RT_ERR_OK)
    {
        RT_LOG(LOG_TRACE, MOD_PHY, "fail 0x%X", ret);
    }

    return ret;
}



/* Function Name:
 *      phy_8226_gigaLiteEnable_set
 * Description:
 *      Set the status of Giga Lite of the specific port in the specific unit
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - status of Giga Lite
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8226_gigaLiteEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    HANDLE  hDevice;
    BOOL    drv_enable;
    int32   ret;

    RT_LOG(LOG_TRACE, MOD_PHY, "U=%u P=%u enable=%u", unit, port, enable);

    hDevice.unit = unit;
    hDevice.port = port;

    if (enable == ENABLED)
        drv_enable = TRUE;
    else
        drv_enable = FALSE;

    RTL8226_EC_CONV(Rtl8226_gigaLiteEnable_set(hDevice, drv_enable), ret);

    if (ret != RT_ERR_OK)
    {
        RT_LOG(LOG_TRACE, MOD_PHY, "fail 0x%X", ret);
    }

    return ret;
}

/* Function Name:
 *      phy_8226_2point5gLiteEnable_get
 * Description:
 *      Get the status of 2.5G Lite of the specific port in the specific unit
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to status of Giga Lite
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8226_2pt5gLiteEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    HANDLE  hDevice;
    BOOL    drv_enable;
    int32   ret;

    hDevice.unit = unit;
    hDevice.port = port;

    RTL8226_EC_CONV(Rtl8226_2p5gLiteEnable_get(hDevice, &drv_enable), ret);

    *pEnable = (drv_enable == TRUE) ? ENABLED : DISABLED;

    return ret;
}



/* Function Name:
 *      phy_8226_2point5gLiteEnable_set
 * Description:
 *      Set the status of 2.5G Lite of the specific port in the specific unit
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - status of Giga Lite
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8226_2pt5gLiteEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    HANDLE  hDevice;
    BOOL    drv_enable;
    int32   ret;

    hDevice.unit = unit;
    hDevice.port = port;

    if (enable == ENABLED)
        drv_enable = TRUE;
    else
        drv_enable = FALSE;

    RTL8226_EC_CONV(Rtl8226_2p5gLiteEnable_set(hDevice, drv_enable), ret);

    return ret;
}


/* Function Name:
 *      phy_8226_greenEnable_get
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
 * Note:
 *      None
 */
int32
phy_8226_greenEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    HANDLE  hDevice;
    BOOL    drv_enable;
    int32   ret;

    hDevice.unit = unit;
    hDevice.port = port;

    RTL8226_EC_CONV(Rtl8226_greenEnable_get(hDevice, &drv_enable), ret);

    *pEnable = (drv_enable == TRUE) ? ENABLED : DISABLED;

    return ret;
}

/* Function Name:
 *      phy_8226_greenEnable_set
 * Description:
 *      Set the status of link-up green feature of the specific port in the specific unit
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - status of link-up  green feature
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8226_greenEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    HANDLE  hDevice;
    BOOL    drv_enable;
    int32   ret;

    hDevice.unit = unit;
    hDevice.port = port;

    if (enable == ENABLED)
        drv_enable = TRUE;
    else
        drv_enable = FALSE;

    RTL8226_EC_CONV(Rtl8226_greenEnable_set(hDevice, drv_enable), ret);

    return ret;
}


/* Function Name:
 *      phy_8226_eeeEnable_get
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
phy_8226_eeeEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    HANDLE  hDevice;
    PHY_EEE_ENABLE  drv_eee_enable;
    int32   ret;

    hDevice.unit = unit;
    hDevice.port = port;

    RTL8226_EC_CONV(Rtl8226_eeeEnable_get(hDevice, &drv_eee_enable), ret);

    *pEnable = (drv_eee_enable.EEE_2_5G == TRUE) ? ENABLED : DISABLED;

    return ret;
}

/* Function Name:
 *      phy_8226_eeeEnable_set
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
phy_8226_eeeEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    HANDLE  hDevice;
    PHY_EEE_ENABLE  drv_eee_enable;
    int32   ret;

    hDevice.unit = unit;
    hDevice.port = port;

    if (enable == ENABLED)
    {
        drv_eee_enable.EEE_100 = 1;
        drv_eee_enable.EEE_1000 = 1;
        drv_eee_enable.EEE_2_5G = 1;
    }
    else
    {
        drv_eee_enable.EEE_100 = 0;
        drv_eee_enable.EEE_1000 = 0;
        drv_eee_enable.EEE_2_5G = 0;
    }

    RTL8226_EC_CONV(Rtl8226_eeeEnable_set(hDevice, &drv_eee_enable), ret);

    return ret;
}

/* Function Name:
 *      phy_8226_downSpeedEnable_get
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
phy_8226_downSpeedEnable_get(uint32 unit, rtk_port_t port,
    rtk_enable_t *pEnable)
{
    HANDLE  hDevice;
    BOOL    drv_enable;
    int32   ret;

    hDevice.unit = unit;
    hDevice.port = port;

    RTL8226_EC_CONV(Rtl8226_downSpeedEnable_get(hDevice, &drv_enable), ret);

    *pEnable = (drv_enable == TRUE) ? ENABLED : DISABLED;

    return ret;
}

/* Function Name:
 *      phy_8226_downSpeedEnable_set
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
phy_8226_downSpeedEnable_set(uint32 unit, rtk_port_t port,
    rtk_enable_t enable)
{
    HANDLE  hDevice;
    BOOL    drv_enable;
    int32   ret;

    hDevice.unit = unit;
    hDevice.port = port;

    if (enable == ENABLED)
        drv_enable = TRUE;
    else
        drv_enable = FALSE;

    RTL8226_EC_CONV(Rtl8226_downSpeedEnable_set(hDevice, drv_enable), ret);

    return ret;
}


/* Function Name:
 *      phy_8226_rtctResult_get
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
 *      The result length unit is cm.
 */
int32
phy_8226_rtctResult_get(uint32 unit, rtk_port_t port, rtk_rtctResult_t *pRtctResult)
{
    HANDLE  hDevice;
    PHY_RTCT_RESULT drv_rtctResult;
    int32   ret;

    hDevice.unit = unit;
    hDevice.port = port;
    osal_memset(&drv_rtctResult, 0, sizeof(PHY_RTCT_RESULT));
    RTL8226_EC_CONV(Rtl8226_rtctResult_get(hDevice, &drv_rtctResult), ret);
    if (ret != RT_ERR_OK)
    {
        return ret;
    }

    pRtctResult->linkType = PORT_SPEED_1000M;
    pRtctResult->un.ge_result.channelALen = drv_rtctResult.channelALen;
    pRtctResult->un.ge_result.channelBLen = drv_rtctResult.channelBLen;
    pRtctResult->un.ge_result.channelCLen = drv_rtctResult.channelCLen;
    pRtctResult->un.ge_result.channelDLen = drv_rtctResult.channelDLen;

    if (drv_rtctResult.channelAStatus.Mismatch == MIS_MATCH_OPEN)
    {
        pRtctResult->un.ge_result.channelAOpen = 1;
    }
    else if (drv_rtctResult.channelAStatus.Mismatch == MIS_MATCH_SHORT)
    {
        pRtctResult->un.ge_result.channelAShort = 1;
    }
    else
    {
        RT_LOG(LOG_TRACE, MOD_PHY, "8226 unknown channelA status %u\n", drv_rtctResult.channelAStatus.Mismatch);
    }

    if (drv_rtctResult.channelBStatus.Mismatch == MIS_MATCH_OPEN)
    {
        pRtctResult->un.ge_result.channelBOpen = 1;
    }
    else if (drv_rtctResult.channelBStatus.Mismatch == MIS_MATCH_SHORT)
    {
        pRtctResult->un.ge_result.channelBShort = 1;
    }
    else
    {
        RT_LOG(LOG_TRACE, MOD_PHY, "8226 unknown channelB status %u\n", drv_rtctResult.channelBStatus.Mismatch);
    }

    if (drv_rtctResult.channelCStatus.Mismatch == MIS_MATCH_OPEN)
    {
        pRtctResult->un.ge_result.channelCOpen = 1;
    }
    else if (drv_rtctResult.channelCStatus.Mismatch == MIS_MATCH_SHORT)
    {
        pRtctResult->un.ge_result.channelCShort = 1;
    }
    else
    {
        RT_LOG(LOG_TRACE, MOD_PHY, "8226 unknown channelC status %u\n", drv_rtctResult.channelCStatus.Mismatch);
    }

    if (drv_rtctResult.channelDStatus.Mismatch == MIS_MATCH_OPEN)
    {
        pRtctResult->un.ge_result.channelDOpen = 1;
    }
    else if (drv_rtctResult.channelDStatus.Mismatch == MIS_MATCH_SHORT)
    {
        pRtctResult->un.ge_result.channelDShort = 1;
    }
    else
    {
        RT_LOG(LOG_TRACE, MOD_PHY, "8226 unknown channelD status %u\n", drv_rtctResult.channelDStatus.Mismatch);
    }

    return RT_ERR_OK;

}

/* Function Name:
 *      phy_8226_rtct_start
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
 *      RT_ERR_CHIP_NOT_SUPPORTED - chip not supported
 * Note:
 *      RTCT is not supported when port link at 10M.
 */
int32
phy_8226_rtct_start(uint32 unit, rtk_port_t port)
{
    HANDLE  hDevice;
    int32   ret;

    hDevice.unit = unit;
    hDevice.port = port;
    RTL8226_EC_CONV(Rtl8226_rtct_start(hDevice), ret);
    return ret;
}

/* Function Name:
 *      phy_8226_linkDownPowerSavingEnable_get
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
 * Note:
 *      None
 */
int32
phy_8226_linkDownPowerSavingEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    HANDLE  hDevice;
    BOOL    drv_enable;
    int32   ret;

    hDevice.unit = unit;
    hDevice.port = port;

    RTL8226_EC_CONV(Rtl8226_linkDownPowerSavingEnable_get(hDevice, &drv_enable), ret);

    *pEnable = (drv_enable == TRUE) ? ENABLED : DISABLED;

    return ret;
}

/* Function Name:
 *      phy_8226_linkDownPowerSavingEnable_set
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
 * Note:
 *      None
 */
int32
phy_8226_linkDownPowerSavingEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    HANDLE  hDevice;
    BOOL    drv_enable;
    int32   ret;

    hDevice.unit = unit;
    hDevice.port = port;

    if (enable == ENABLED)
        drv_enable = TRUE;
    else
        drv_enable = FALSE;

    RTL8226_EC_CONV(Rtl8226_linkDownPowerSavingEnable_set(hDevice, drv_enable), ret);

    return ret;
}


/* Function Name:
 *      phy_8226_ieeeTestMode_set
 * Description:
 *      Set test mode for PHY transmitter test
 * Input:
 *      unit  - unit id
 *      port  - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_NOT_ALLOWED - The operation is not allowed
 *      RT_ERR_PORT_NOT_SUPPORTED - test mode is not supported
 * Note:
 *      None
 */
int32
phy_8226_ieeeTestMode_set(uint32 unit, rtk_port_t port, rtk_port_phyTestMode_t *pTestMode)
{
    HANDLE  hDevice;
    UINT16  drv_speed = 0;
    PHY_IEEE_TEST_MODE drv_ieeetestmode;
    int32   ret;

    RT_LOG(LOG_TRACE, MOD_PHY, "unit=%u port=%u mode=0x%x channel=%u", unit, port, pTestMode->mode, pTestMode->channel);

    hDevice.unit = unit;
    hDevice.port = port;

    osal_memset(&drv_ieeetestmode, 0, sizeof(PHY_IEEE_TEST_MODE));
    switch (pTestMode->mode)
    {
      case RTK_PORT_PHY_1G_TEST_MODE1:
        drv_ieeetestmode.TM1 = 1;
        drv_speed = LINK_SPEED_1G;
        break;
      case RTK_PORT_PHY_1G_TEST_MODE2:
        drv_ieeetestmode.TM2 = 1;
        drv_speed = LINK_SPEED_1G;
        break;
      case RTK_PORT_PHY_1G_TEST_MODE3:
        drv_ieeetestmode.TM3 = 1;
        drv_speed = LINK_SPEED_1G;
        break;
      case RTK_PORT_PHY_1G_TEST_MODE4:
        drv_ieeetestmode.TM4 = 1;
        drv_speed = LINK_SPEED_1G;
        break;

      case RTK_PORT_PHY_10G_TEST_MODE_NONE:
        drv_ieeetestmode.TMFINISH = 1;
        drv_speed = LINK_SPEED_2P5G;
        break;
      case RTK_PORT_PHY_10G_TEST_MODE1:
        drv_ieeetestmode.TM1 = 1;
        drv_speed = LINK_SPEED_2P5G;
        break;
      case RTK_PORT_PHY_10G_TEST_MODE2:
        drv_ieeetestmode.TM2 = 1;
        drv_speed = LINK_SPEED_2P5G;
        break;
      case RTK_PORT_PHY_10G_TEST_MODE3:
        drv_ieeetestmode.TM3 = 1;
        drv_speed = LINK_SPEED_2P5G;
        break;
      case RTK_PORT_PHY_10G_TEST_MODE4_DUALTONE1:
        drv_ieeetestmode.TM4 = 1;
        drv_ieeetestmode.TONE1 = 1;
        drv_speed = LINK_SPEED_2P5G;
        break;
      case RTK_PORT_PHY_10G_TEST_MODE4_DUALTONE2:
        drv_ieeetestmode.TM4 = 1;
        drv_ieeetestmode.TONE2 = 1;
        drv_speed = LINK_SPEED_2P5G;
        break;
      case RTK_PORT_PHY_10G_TEST_MODE4_DUALTONE3:
        drv_ieeetestmode.TM4 = 1;
        drv_ieeetestmode.TONE3 = 1;
        drv_speed = LINK_SPEED_2P5G;
        break;
      case RTK_PORT_PHY_10G_TEST_MODE4_DUALTONE4:
        drv_ieeetestmode.TM4 = 1;
        drv_ieeetestmode.TONE4 = 1;
        drv_speed = LINK_SPEED_2P5G;
        break;
      case RTK_PORT_PHY_10G_TEST_MODE4_DUALTONE5:
        drv_ieeetestmode.TM4 = 1;
        drv_ieeetestmode.TONE5 = 1;
        drv_speed = LINK_SPEED_2P5G;
        break;
      case RTK_PORT_PHY_10G_TEST_MODE5:
        drv_ieeetestmode.TM5 = 1;
        drv_speed = LINK_SPEED_2P5G;
        break;
      case RTK_PORT_PHY_10G_TEST_MODE6:
        drv_ieeetestmode.TM6 = 1;
        drv_speed = LINK_SPEED_2P5G;
        break;
      default:
        return RT_ERR_INPUT;
    }

    switch (pTestMode->channel)
    {
      case PHY_TESTMODE_CHANNEL_NONE:
        drv_ieeetestmode.channel = TESTMODE_CHANNEL_NONE;
        break;
      case PHY_TESTMODE_CHANNEL_A:
        drv_ieeetestmode.channel = TESTMODE_CHANNEL_A;
        break;
      case PHY_TESTMODE_CHANNEL_B:
        drv_ieeetestmode.channel = TESTMODE_CHANNEL_B;
        break;
      case PHY_TESTMODE_CHANNEL_C:
        drv_ieeetestmode.channel = TESTMODE_CHANNEL_C;
        break;
      case PHY_TESTMODE_CHANNEL_D:
        drv_ieeetestmode.channel = TESTMODE_CHANNEL_D;
        break;
      default:
        drv_ieeetestmode.channel = TESTMODE_CHANNEL_NONE;
        break;
    }

    RTL8226_EC_CONV(Rtl8226_ieeeTestMode_set(hDevice, drv_speed, &drv_ieeetestmode), ret);

    if (ret != RT_ERR_OK)
    {
        RT_LOG(LOG_TRACE, MOD_PHY, "unit=%u port=%u set fail %X", unit, port, ret);
    }

    return ret;
}




/* Function Name:
 *      phy_8226_macIntfSerdesMode_get
 * Description:
 *      Get PHY's MAC side interface serdes mode
 * Input:
 *      unit    - unit ID
 *      port    - Base mac ID of the PHY
 * Output:
 *      pSerdesMode  - PHY serdes mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
int32
phy_8226_macIntfSerdesMode_get(uint32 unit, rtk_port_t port, rt_serdesMode_t *pSerdesMode)
{
    HANDLE  hDevice;
    BOOL    drv_sdsLink = 0;
    PHY_SERDES_MODE     drv_sdsMode = PHY_SERDES_MODE_NO_SDS;
    int32   ret;

    hDevice.unit = unit;
    hDevice.port = port;

    RTL8226_EC_CONV(Rtl8226_serdes_link_get(hDevice, &drv_sdsLink, &drv_sdsMode), ret);

    if (ret != RT_ERR_OK)
        return ret;

    switch (drv_sdsMode)
    {
      case PHY_SERDES_MODE_SGMII:
        *pSerdesMode = RTK_MII_SGMII;
        break;
      case PHY_SERDES_MODE_HiSGMII:
        *pSerdesMode = RTK_MII_HISGMII;
        break;
      case PHY_SERDES_MODE_2500BASEX:
        *pSerdesMode = RTK_MII_2500Base_X;
        break;
      case PHY_SERDES_MODE_USXGMII:
        *pSerdesMode = RTK_MII_USXGMII_2_5GSXGMII;
        break;
      case PHY_SERDES_MODE_NO_SDS:
        *pSerdesMode = RTK_MII_DISABLE;
        break;
      default:
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}


/* Function Name:
 *      phy_8226_macIntfSerdesLinkStatus_get
 * Description:
 *      Get PHY's MAC side interface serdes link status
 * Input:
 *      unit    - unit ID
 *      port    - Base port ID of the PHY
 * Output:
 *      pStatus - link status of the SerDes
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8226_macIntfSerdesLinkStatus_get(uint32 unit, rtk_port_t port, rtk_phy_macIntfSdsLinkStatus_t *pStatus)
{
    HANDLE  hDevice;
    BOOL    drv_sdsLink = 0;
    PHY_SERDES_MODE     drv_sdsMode = PHY_SERDES_MODE_NO_SDS;
    int32   ret;

    hDevice.unit = unit;
    hDevice.port = port;

    RTL8226_EC_CONV(Rtl8226_serdes_link_get(hDevice, &drv_sdsLink, &drv_sdsMode), ret);

    if (ret != RT_ERR_OK)
        return ret;

    pStatus->sds_num = 1;
    pStatus->link_status[0] = (drv_sdsLink) ? PORT_LINKUP : PORT_LINKDOWN;
    return RT_ERR_OK;
}


/* Function Name:
 *      phy_8226_chipRevId_get
 * Description:
 *      Get chip revision ID
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      pChip_rev_id - chip revision ID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8226_chipRevId_get(uint32 unit, rtk_port_t port, uint16 *chip_rev_id)
{
    *chip_rev_id = 0;

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8226b_init
 * Description:
 *      Initialize PHY 8226.
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
phy_8226b_init(uint32 unit, rtk_port_t port)
{
    uint32  base_port;

    RT_LOG(LOG_TRACE, MOD_PHY, "U=%u P=%u", unit, port);
    if ((base_port = HWP_PHY_BASE_MACID(unit, port)) == HWP_NONE)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_HAL), "unit=%u,port=%u get base port ID fail", unit, port);
        return RT_ERR_PORT_ID;
    }

    RT_LOG(LOG_TRACE, MOD_PHY, "base_port=%u", base_port);
    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8226b_media_get
 * Description:
 *      Get PHY media type.
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
 *      None
 */
int32
phy_8226b_media_get(uint32 unit, rtk_port_t port, rtk_port_media_t *pMedia)
{
    *pMedia = PORT_MEDIA_COPPER;
    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8226b_enable_set
 * Description:
 *      Set PHY interface status of the specific port
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
phy_8226b_enable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    HANDLE  hDevice;
    BOOL    drv_enable;
    int32   ret;

    RT_LOG(LOG_TRACE, MOD_PHY, "unit=%u port=%u enable=%u\n", unit, port, enable);

    hDevice.unit = unit;
    hDevice.port = port;

    if (enable == ENABLED)
        drv_enable = TRUE;
    else
        drv_enable = FALSE;

    RTL8226_EC_CONV(Rtl8226b_enable_set(hDevice, drv_enable), ret);

    if (ret != RT_ERR_OK)
    {
        RT_LOG(LOG_TRACE, MOD_PHY, "fail 0x%X", ret);
    }

    return ret;
}



int32
phy_8226b_autoNegoEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    HANDLE  hDevice;
    BOOL    drv_enable;
    int32   ret;

    hDevice.unit = unit;
    hDevice.port = port;

    RTL8226_EC_CONV(Rtl8226b_autoNegoEnable_get(hDevice, &drv_enable), ret);

    *pEnable = (drv_enable == TRUE) ? ENABLED : DISABLED;

    if (ret != RT_ERR_OK)
    {
        RT_LOG(LOG_TRACE, MOD_PHY, "fail 0x%X", ret);
    }

    return ret;
}

int32
phy_8226b_autoNegoEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    HANDLE  hDevice;
    BOOL    drv_enable;
    int32   ret;

    RT_LOG(LOG_TRACE, MOD_PHY, "U=%u P=%u enable=%u", unit, port, enable);

    hDevice.unit = unit;
    hDevice.port = port;

    if (enable == ENABLED)
        drv_enable = TRUE;
    else
        drv_enable = FALSE;

    RTL8226_EC_CONV(Rtl8226b_autoNegoEnable_set(hDevice, drv_enable), ret);

    if (ret != RT_ERR_OK)
    {
        RT_LOG(LOG_TRACE, MOD_PHY, "fail 0x%X", ret);
    }

    return ret;
}



/* Function Name:
 *      phy_8226b_autoNegoAbility_get
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
phy_8226b_autoNegoAbility_get(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility)
{
    HANDLE  hDevice;
    PHY_LINK_ABILITY    drv_phyAbility;
    int32   ret;

    hDevice.unit = unit;
    hDevice.port = port;

    osal_memset(&drv_phyAbility, 0, sizeof(PHY_LINK_ABILITY));

    RTL8226_EC_CONV(Rtl8226b_autoNegoAbility_get(hDevice, &drv_phyAbility), ret);
    if (ret != RT_ERR_OK)
    {
        RT_LOG(LOG_TRACE, MOD_PHY, "fail");
        return ret;
    }

    pAbility->Half_10   = drv_phyAbility.Half_10;
    pAbility->Full_10   = drv_phyAbility.Full_10;
    pAbility->Half_100  = drv_phyAbility.Half_100;
    pAbility->Full_100  = drv_phyAbility.Full_100;
    pAbility->Full_1000 = drv_phyAbility.Full_1000;
    pAbility->adv_2_5G  = drv_phyAbility.adv_2_5G;
    pAbility->FC        = drv_phyAbility.FC;
    pAbility->AsyFC     = drv_phyAbility.AsyFC;

    return RT_ERR_OK;
}


/* Function Name:
 *      phy_8226b_autoNegoAbility_set
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
phy_8226b_autoNegoAbility_set(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility)
{
    HANDLE  hDevice;
    PHY_LINK_ABILITY    drv_phyAbility;
    int32   ret;

    RT_LOG(LOG_TRACE, MOD_PHY, "U=%u P=%u 10h=%u 10f=%u 100h=%u 100f=%u 1000f=%u 2.5g=%u FC=%u AsyFC=%u", unit, port,
        pAbility->Half_10, pAbility->Full_10, pAbility->Half_100, pAbility->Full_100, pAbility->Full_1000, pAbility->adv_2_5G, pAbility->FC, pAbility->AsyFC);

    //osal_printf("%s:%u\n", __FUNCTION__, __LINE__);
    hDevice.unit = unit;
    hDevice.port = port;

    osal_memset(&drv_phyAbility, 0, sizeof(PHY_LINK_ABILITY));

    drv_phyAbility.Half_10   = pAbility->Half_10;
    drv_phyAbility.Full_10   = pAbility->Full_10;
    drv_phyAbility.Half_100  = pAbility->Half_100;
    drv_phyAbility.Full_100  = pAbility->Full_100;
    drv_phyAbility.Full_1000 = pAbility->Full_1000;
    drv_phyAbility.adv_2_5G  = pAbility->adv_2_5G;
    drv_phyAbility.FC        = pAbility->FC;
    drv_phyAbility.AsyFC     = pAbility->AsyFC;


    RTL8226_EC_CONV(Rtl8226b_autoNegoAbility_set(hDevice, &drv_phyAbility), ret);
    if (ret != RT_ERR_OK)
    {
        RT_LOG(LOG_TRACE, MOD_PHY, "U=%u P=%u fail", unit, port);
        return ret;
    }

    return RT_ERR_OK;

}


/* Function Name:
 *      phy_8226b_duplex_get
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
phy_8226b_duplex_get(uint32 unit, rtk_port_t port, uint32 *pDuplex)
{
    HANDLE  hDevice;
    BOOL    drv_enable;
    int32   ret;

    hDevice.unit = unit;
    hDevice.port = port;

    RTL8226_EC_CONV(Rtl8226b_duplex_get(hDevice, &drv_enable), ret);
    if (ret != RT_ERR_OK)
    {
    RT_LOG(LOG_TRACE, MOD_PHY, "U=%u P=%u fail", unit, port);
        return ret;
    }

    *pDuplex = drv_enable;

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8226b_duplex_set
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
phy_8226b_duplex_set(uint32 unit, rtk_port_t port, uint32 duplex)
{
    HANDLE  hDevice;
    BOOL    drv_enable;
    int32   ret;

    RT_LOG(LOG_TRACE, MOD_PHY, "U=%u P=%u duplex=%u", unit, port, duplex);

    hDevice.unit = unit;
    hDevice.port = port;

    drv_enable = (BOOL)duplex;

    RTL8226_EC_CONV(Rtl8226b_duplex_set(hDevice, drv_enable), ret);
    if (ret != RT_ERR_OK)
    {
        RT_LOG(LOG_TRACE, MOD_PHY, "U=%u P=%u fail", unit, port);
        return ret;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      phy_8226b_speed_get
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
phy_8226b_speed_get(uint32 unit, rtk_port_t port, rtk_port_speed_t *pSpeed)
{
    HANDLE  hDevice;
    UINT16  drv_speed;
    int32   ret;

    hDevice.unit = unit;
    hDevice.port = port;

    RTL8226_EC_CONV(Rtl8226b_speed_get(hDevice, &drv_speed), ret);
    if (ret != RT_ERR_OK)
    {
        RT_LOG(LOG_TRACE, MOD_PHY, "U=%u P=%u fail", unit, port);
        return ret;
    }

    if (drv_speed == NO_LINK)
    {
        RTL8226_EC_CONV(Rtl8226b_force_speed_get(hDevice, &drv_speed), ret);
        if (ret != RT_ERR_OK)
        {
            RT_LOG(LOG_TRACE, MOD_PHY, "U=%u P=%u fail", unit, port);
            return ret;
        }
    }

    switch(drv_speed)
    {
      case LINK_SPEED_10M:
        *pSpeed = PORT_SPEED_10M;
        break;
      case LINK_SPEED_100M:
        *pSpeed = PORT_SPEED_100M;
        break;
      case LINK_SPEED_1G:
        *pSpeed = PORT_SPEED_1000M;
        break;
      case LINK_SPEED_500M:
        *pSpeed = PORT_SPEED_500M;
        break;
	  case LINK_SPEED_2P5G:
        *pSpeed = PORT_SPEED_2_5G;
        break;
      case NO_LINK:
        *pSpeed = PORT_SPEED_10M;
        break;
      default:
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8226b_speed_set
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
phy_8226b_speed_set(uint32 unit, rtk_port_t port, rtk_port_speed_t speed)
{
    HANDLE  hDevice;
    UINT16  drv_speed;
    int32   ret;

    hDevice.unit = unit;
    hDevice.port = port;

    switch(speed)
    {
      case PORT_SPEED_10M:
        drv_speed = 10;
        break;
      case PORT_SPEED_100M:
        drv_speed = 100;
        break;
      case PORT_SPEED_1000M:
        drv_speed = 1000;
        break;
      case PORT_SPEED_2_5G:
        drv_speed = 2500;
        break;
      default:
        return RT_ERR_INPUT;
    }

    RTL8226_EC_CONV(Rtl8226b_force_speed_set(hDevice, drv_speed), ret);
    if (ret != RT_ERR_OK)
    {
        RT_LOG(LOG_TRACE, MOD_PHY, "U=%u P=%u fail", unit, port);
        return ret;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      phy_8226b_crossOverMode_get
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
 * Note:
 *      Following value is valid
 *      - PORT_CROSSOVER_MODE_AUTO
 *      - PORT_CROSSOVER_MODE_MDI
 *      - PORT_CROSSOVER_MODE_MDIX
 */
int32
phy_8226b_crossOverMode_get(uint32 unit, rtk_port_t port, rtk_port_crossOver_mode_t *pMode)
{
    HANDLE  hDevice;
    PHY_CROSSPVER_MODE  drv_crossOverMode;
    int32   ret;

    hDevice.unit = unit;
    hDevice.port = port;

    RTL8226_EC_CONV(Rtl8226b_crossOverMode_get(hDevice, &drv_crossOverMode), ret);
    if (ret != RT_ERR_OK)
    {
        return ret;
    }

    switch (drv_crossOverMode)
    {
      case PHY_CROSSPVER_MODE_MDI:
        *pMode = PORT_CROSSOVER_MODE_MDI;
        break;
      case PHY_CROSSPVER_MODE_MDIX:
        *pMode = PORT_CROSSOVER_MODE_MDIX;
        break;
      case PHY_CROSSPVER_MODE_AUTO:
        *pMode = PORT_CROSSOVER_MODE_AUTO;
        break;
      default:
        RT_LOG(LOG_TRACE, MOD_PHY, "unit=%u port=%u mode=%u", unit, port, drv_crossOverMode);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      phy_8226b_crossOverMode_set
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
 * Note:
 *      Following value is valid
 *      - PORT_CROSSOVER_MODE_AUTO
 *      - PORT_CROSSOVER_MODE_MDI
 *      - PORT_CROSSOVER_MODE_MDIX
 */
int32
phy_8226b_crossOverMode_set(uint32 unit, rtk_port_t port, rtk_port_crossOver_mode_t mode)
{
    HANDLE  hDevice;
    PHY_CROSSPVER_MODE  drv_crossOverMode;
    int32   ret;

    RT_LOG(LOG_TRACE, MOD_PHY, "unit=%u port=%u mode=%u", unit, port, mode);
    hDevice.unit = unit;
    hDevice.port = port;

    //osal_printf("%s:%u\n", __FUNCTION__, __LINE__);
    switch (mode)
    {
      case PORT_CROSSOVER_MODE_MDI:
        drv_crossOverMode = PHY_CROSSPVER_MODE_MDI;
        break;
      case PORT_CROSSOVER_MODE_MDIX:
        drv_crossOverMode = PHY_CROSSPVER_MODE_MDIX;
        break;
      case PORT_CROSSOVER_MODE_AUTO:
        drv_crossOverMode = PHY_CROSSPVER_MODE_AUTO;
        break;
      default:
        RT_LOG(LOG_TRACE, MOD_PHY, "unit=%u port=%u mode=%u", unit, port, mode);
        return RT_ERR_INPUT;
    }


    RTL8226_EC_CONV(Rtl8226b_crossOverMode_set(hDevice, drv_crossOverMode), ret);
    if (ret != RT_ERR_OK)
    {
        RT_LOG(LOG_TRACE, MOD_PHY, "unit=%u port=%u mode=%u fail", unit, port, drv_crossOverMode);
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8226b_crossOverStatus_get
 * Description:
 *      Get cross over status in the specified port.
 * Input:
 *      unit  - unit id
 *      port  - port id
 * Output:
 *      pMode - pointer to cross over mode status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Following value is valid
 *      - PORT_CROSSOVER_STATUS_MDI
 *      - PORT_CROSSOVER_STATUS_MDIX
 */
int32
phy_8226b_crossOverStatus_get(uint32 unit, rtk_port_t port, rtk_port_crossOver_status_t *pStatus)
{
    HANDLE  hDevice;
    PHY_CROSSPVER_STATUS  drv_crossOverStatus;
    int32   ret;

    hDevice.unit = unit;
    hDevice.port = port;

    RTL8226_EC_CONV(Rtl8226b_crossOverStatus_get(hDevice, &drv_crossOverStatus), ret);
    if (ret != RT_ERR_OK)
    {
        RT_LOG(LOG_TRACE, MOD_PHY, "unit=%u port=%u fail", unit, port);
        return ret;
    }

    switch (drv_crossOverStatus)
    {
      case PHY_CROSSPVER_STATUS_MDI:
        *pStatus = PORT_CROSSOVER_STATUS_MDI;
        break;
      case PHY_CROSSPVER_STATUS_MDIX:
        *pStatus = PORT_CROSSOVER_STATUS_MDIX;
        break;
      default:
        RT_LOG(LOG_TRACE, MOD_PHY, "unit=%u port=%u status=%u", unit, port, drv_crossOverStatus);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      phy_8226b_masterSlave_get
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
 *      RT_ERR_PORT_ID       - invalid port id
 *      RT_ERR_NULL_POINTER  - input parameter may be null pointer
 * Note:
 *      This function only works on giga/ 10g port to get its master/slave mode configuration.
 */
int32
phy_8226b_masterSlave_get(
    uint32              unit,
    rtk_port_t          port,
    rtk_port_masterSlave_t   *pMasterSlaveCfg,
    rtk_port_masterSlave_t   *pMasterSlaveActual)
{
    HANDLE  hDevice;
    PHY_MASTERSLAVE_MODE  drv_masterSlaveMode;
    int32   ret;
    uint32  phydata;

    hDevice.unit = unit;
    hDevice.port = port;

    RTL8226_EC_CONV(Rtl8226b_masterSlave_get(hDevice, &drv_masterSlaveMode), ret);
    if (ret != RT_ERR_OK)
    {
        RT_LOG(LOG_TRACE, MOD_PHY, "unit=%u port=%u fail", unit, port);
        return ret;
    }

    switch (drv_masterSlaveMode)
    {
      case PHY_AUTO_MODE:
        *pMasterSlaveCfg = PORT_AUTO_MODE;
        break;
      case PHY_MASTER_MODE:
        *pMasterSlaveCfg = PORT_MASTER_MODE;
        break;
      case PHY_SLAVE_MODE:
        *pMasterSlaveCfg = PORT_SLAVE_MODE;
        break;
      default:
        RT_LOG(LOG_TRACE, MOD_PHY, "unit=%u port=%u fail", unit, port);
        return RT_ERR_FAILED;
    }

    if ((ret = hal_miim_mmd_read(unit, port, MMD_AN, 33, &phydata)) != RT_ERR_OK)
    {
        RT_LOG(LOG_TRACE, MOD_PHY, "unit=%u port=%u fail 0x%X", unit, port, ret);
        return ret;
    }

    if (phydata & BIT_14)
    {
        *pMasterSlaveActual = PORT_MASTER_MODE;
    }
    else
    {
        *pMasterSlaveActual = PORT_SLAVE_MODE;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_8226b_masterSlave_set
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
 *      RT_ERR_PORT_ID       - invalid port id
 *      RT_ERR_INPUT         - invalid input parameter
 * Note:
 *      None
 */
int32
phy_8226b_masterSlave_set(
    uint32              unit,
    rtk_port_t          port,
    rtk_port_masterSlave_t   masterSlave)
{
    HANDLE  hDevice;
    PHY_MASTERSLAVE_MODE  drv_masterSlaveMode;
    int32   ret;


    RT_LOG(LOG_TRACE, MOD_PHY, "unit=%u port=%u mode=%u", unit, port, masterSlave);
    hDevice.unit = unit;
    hDevice.port = port;

    //osal_printf("%s:%u\n", __FUNCTION__, __LINE__);
    switch (masterSlave)
    {
      case PORT_AUTO_MODE:
        drv_masterSlaveMode = PHY_AUTO_MODE;
        break;
      case PORT_MASTER_MODE:
        drv_masterSlaveMode = PHY_MASTER_MODE;
        break;
      case PORT_SLAVE_MODE:
        drv_masterSlaveMode = PHY_SLAVE_MODE;
        break;
      default:
        RT_LOG(LOG_TRACE, MOD_PHY, "unit=%u port=%u fail", unit, port);
        return RT_ERR_FAILED;
    }

    RTL8226_EC_CONV(Rtl8226b_masterSlave_set(hDevice, drv_masterSlaveMode), ret);
    if (ret != RT_ERR_OK)
    {
        RT_LOG(LOG_TRACE, MOD_PHY, "unit=%u port=%u fail", unit, port);
        return ret;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      phy_8226b_loopback_get
 * Description:
 *      Get PHY Loopback mode of the specific port
 * Input:
 *      unit                - unit id
 *      port                - port id
 *      pEnable           -loopback mode status;
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8226b_loopback_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    HANDLE  hDevice;
    BOOL    drv_enable;
    int32   ret;

    hDevice.unit = unit;
    hDevice.port = port;

    RTL8226_EC_CONV(Rtl8226b_loopback_get(hDevice, &drv_enable), ret);

    *pEnable = (drv_enable == TRUE) ? ENABLED : DISABLED;

    if (ret != RT_ERR_OK)
    {
        RT_LOG(LOG_TRACE, MOD_PHY, "fail 0x%X", ret);
    }

    return ret;
}


/* Function Name:
 *      phy_8226b_loopback_set
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
phy_8226b_loopback_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    HANDLE  hDevice;
    BOOL    drv_enable;
    int32   ret;

    RT_LOG(LOG_TRACE, MOD_PHY, "U=%u P=%u enable=%u", unit, port, enable);

    hDevice.unit = unit;
    hDevice.port = port;

    //osal_printf("%s:%u\n", __FUNCTION__, __LINE__);
    if (enable == ENABLED)
        drv_enable = TRUE;
    else
        drv_enable = FALSE;

    RTL8226_EC_CONV(Rtl8226b_loopback_set(hDevice, drv_enable), ret);

    if (ret != RT_ERR_OK)
    {
        RT_LOG(LOG_TRACE, MOD_PHY, "fail 0x%X", ret);
    }

    return ret;

}

/* Function Name:
 *      phy_8226b_gigaLiteEnable_get
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
 *      None
 */
int32
phy_8226b_gigaLiteEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    HANDLE  hDevice;
    BOOL    drv_enable;
    int32   ret;

    hDevice.unit = unit;
    hDevice.port = port;

    RTL8226_EC_CONV(Rtl8226b_gigaLiteEnable_get(hDevice, &drv_enable), ret);

    *pEnable = (drv_enable == TRUE) ? ENABLED : DISABLED;

    if (ret != RT_ERR_OK)
    {
        RT_LOG(LOG_TRACE, MOD_PHY, "fail 0x%X", ret);
    }

    return ret;
}



/* Function Name:
 *      phy_8226b_gigaLiteEnable_set
 * Description:
 *      Set the status of Giga Lite of the specific port in the specific unit
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - status of Giga Lite
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8226b_gigaLiteEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    HANDLE  hDevice;
    BOOL    drv_enable;
    int32   ret;

    RT_LOG(LOG_TRACE, MOD_PHY, "U=%u P=%u enable=%u", unit, port, enable);

    hDevice.unit = unit;
    hDevice.port = port;

    //osal_printf("%s:%u\n", __FUNCTION__, __LINE__);
    if (enable == ENABLED)
        drv_enable = TRUE;
    else
        drv_enable = FALSE;

    RTL8226_EC_CONV(Rtl8226b_gigaLiteEnable_set(hDevice, drv_enable), ret);

    if (ret != RT_ERR_OK)
    {
        RT_LOG(LOG_TRACE, MOD_PHY, "fail 0x%X", ret);
    }

    return ret;
}

/* Function Name:
 *      phy_8226b_2point5gLiteEnable_get
 * Description:
 *      Get the status of 2.5G Lite of the specific port in the specific unit
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to status of Giga Lite
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8226b_2pt5gLiteEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    HANDLE  hDevice;
    BOOL    drv_enable;
    int32   ret;

    hDevice.unit = unit;
    hDevice.port = port;

    RTL8226_EC_CONV(Rtl8226b_2p5gLiteEnable_get(hDevice, &drv_enable), ret);

    *pEnable = (drv_enable == TRUE) ? ENABLED : DISABLED;

    return ret;
}



/* Function Name:
 *      phy_8226b_2point5gLiteEnable_set
 * Description:
 *      Set the status of 2.5G Lite of the specific port in the specific unit
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - status of Giga Lite
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8226b_2pt5gLiteEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    HANDLE  hDevice;
    BOOL    drv_enable;
    int32   ret;

    hDevice.unit = unit;
    hDevice.port = port;

    //osal_printf("%s:%u\n", __FUNCTION__, __LINE__);
    if (enable == ENABLED)
        drv_enable = TRUE;
    else
        drv_enable = FALSE;

    RTL8226_EC_CONV(Rtl8226b_2p5gLiteEnable_set(hDevice, drv_enable), ret);

    return ret;
}


/* Function Name:
 *      phy_8226b_greenEnable_get
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
 * Note:
 *      None
 */
int32
phy_8226b_greenEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    HANDLE  hDevice;
    BOOL    drv_enable;
    int32   ret;

    hDevice.unit = unit;
    hDevice.port = port;

    RTL8226_EC_CONV(Rtl8226b_greenEnable_get(hDevice, &drv_enable), ret);

    *pEnable = (drv_enable == TRUE) ? ENABLED : DISABLED;

    return ret;
}

/* Function Name:
 *      phy_8226b_greenEnable_set
 * Description:
 *      Set the status of link-up green feature of the specific port in the specific unit
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - status of link-up  green feature
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8226b_greenEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    HANDLE  hDevice;
    BOOL    drv_enable;
    int32   ret;

    hDevice.unit = unit;
    hDevice.port = port;

    //osal_printf("%s:%u\n", __FUNCTION__, __LINE__);
    if (enable == ENABLED)
        drv_enable = TRUE;
    else
        drv_enable = FALSE;

    RTL8226_EC_CONV(Rtl8226b_greenEnable_set(hDevice, drv_enable), ret);

    return ret;
}


/* Function Name:
 *      phy_8226b_eeeEnable_get
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
phy_8226b_eeeEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    HANDLE  hDevice;
    PHY_EEE_ENABLE  drv_eee_enable;
    int32   ret;

    hDevice.unit = unit;
    hDevice.port = port;

    RTL8226_EC_CONV(Rtl8226b_eeeEnable_get(hDevice, &drv_eee_enable), ret);

    *pEnable = (drv_eee_enable.EEE_2_5G == TRUE) ? ENABLED : DISABLED;

    return ret;
}

/* Function Name:
 *      phy_8226b_eeeEnable_set
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
phy_8226b_eeeEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    HANDLE  hDevice;
    PHY_EEE_ENABLE  drv_eee_enable;
    int32   ret;

    hDevice.unit = unit;
    hDevice.port = port;

    //osal_printf("%s:%u\n", __FUNCTION__, __LINE__);
    if (enable == ENABLED)
    {
        drv_eee_enable.EEE_100 = 1;
        drv_eee_enable.EEE_1000 = 1;
        drv_eee_enable.EEE_2_5G = 1;
    }
    else
    {
        drv_eee_enable.EEE_100 = 0;
        drv_eee_enable.EEE_1000 = 0;
        drv_eee_enable.EEE_2_5G = 0;
    }

    RTL8226_EC_CONV(Rtl8226b_eeeEnable_set(hDevice, &drv_eee_enable), ret);

    return ret;
}

/* Function Name:
 *      phy_8226b_downSpeedEnable_get
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
phy_8226b_downSpeedEnable_get(uint32 unit, rtk_port_t port,
    rtk_enable_t *pEnable)
{
    HANDLE  hDevice;
    BOOL    drv_enable;
    int32   ret;

    hDevice.unit = unit;
    hDevice.port = port;

    RTL8226_EC_CONV(Rtl8226b_downSpeedEnable_get(hDevice, &drv_enable), ret);

    *pEnable = (drv_enable == TRUE) ? ENABLED : DISABLED;

    return ret;
}

/* Function Name:
 *      phy_8226b_downSpeedEnable_set
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
phy_8226b_downSpeedEnable_set(uint32 unit, rtk_port_t port,
    rtk_enable_t enable)
{
    HANDLE  hDevice;
    BOOL    drv_enable;
    int32   ret;

    hDevice.unit = unit;
    hDevice.port = port;

    //osal_printf("%s:%u\n", __FUNCTION__, __LINE__);
    if (enable == ENABLED)
        drv_enable = TRUE;
    else
        drv_enable = FALSE;

    RTL8226_EC_CONV(Rtl8226b_downSpeedEnable_set(hDevice, drv_enable), ret);

    return ret;
}


/* Function Name:
 *      phy_8226b_rtctResult_get
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
 *      The result length unit is cm.
 */
int32
phy_8226b_rtctResult_get(uint32 unit, rtk_port_t port, rtk_rtctResult_t *pRtctResult)
{
    HANDLE  hDevice;
    PHY_RTCT_RESULT drv_rtctResult;
    int32   ret, ret2;
    BOOL    rtct_done;

    hDevice.unit = unit;
    hDevice.port = port;

    osal_memset(&drv_rtctResult, 0, sizeof(PHY_RTCT_RESULT));
    RTL8226_EC_CONV(Rtl8226b_rtctResult_get(hDevice, &drv_rtctResult), ret);
    if (ret != RT_ERR_OK)
    {
        RTL8226_EC_CONV(Rtl8226b_rtctdone_get(hDevice, &rtct_done), ret2);
        if (ret2 != RT_ERR_OK)
        {
            return ret;
        }

        if (rtct_done == FALSE)
        {
            return RT_ERR_PHY_RTCT_NOT_FINISH;
        }
        else
        {
            return ret;
        }
    }

    if (drv_rtctResult.linkType == LINK_SPEED_100M)
    {
        pRtctResult->linkType = PORT_SPEED_100M;
        pRtctResult->un.fe_result.rxLen = drv_rtctResult.rxLen;
        pRtctResult->un.fe_result.txLen = drv_rtctResult.txLen;
    }
    else if ((drv_rtctResult.linkType == LINK_SPEED_1G) || (drv_rtctResult.linkType == LINK_SPEED_2P5G))
    {
        pRtctResult->linkType = PORT_SPEED_1000M;
        pRtctResult->un.ge_result.channelALen = drv_rtctResult.channelALen;
        pRtctResult->un.ge_result.channelBLen = drv_rtctResult.channelBLen;
        pRtctResult->un.ge_result.channelCLen = drv_rtctResult.channelCLen;
        pRtctResult->un.ge_result.channelDLen = drv_rtctResult.channelDLen;
    }
    else
    {
        pRtctResult->linkType = PORT_SPEED_1000M;
        pRtctResult->un.ge_result.channelALen = drv_rtctResult.channelALen;
        pRtctResult->un.ge_result.channelBLen = drv_rtctResult.channelBLen;
        pRtctResult->un.ge_result.channelCLen = drv_rtctResult.channelCLen;
        pRtctResult->un.ge_result.channelDLen = drv_rtctResult.channelDLen;

        pRtctResult->un.ge_result.channelAOpen = drv_rtctResult.channelAStatus.Open;
        pRtctResult->un.ge_result.channelAShort = drv_rtctResult.channelAStatus.Short;
        pRtctResult->un.ge_result.channelAMismatch = drv_rtctResult.channelAStatus.Mismatch;
        pRtctResult->un.ge_result.channelBOpen = drv_rtctResult.channelBStatus.Open;
        pRtctResult->un.ge_result.channelBShort = drv_rtctResult.channelBStatus.Short;
        pRtctResult->un.ge_result.channelBMismatch = drv_rtctResult.channelBStatus.Mismatch;
        pRtctResult->un.ge_result.channelCOpen = drv_rtctResult.channelCStatus.Open;
        pRtctResult->un.ge_result.channelCShort = drv_rtctResult.channelCStatus.Short;
        pRtctResult->un.ge_result.channelCMismatch = drv_rtctResult.channelCStatus.Mismatch;
        pRtctResult->un.ge_result.channelDOpen = drv_rtctResult.channelDStatus.Open;
        pRtctResult->un.ge_result.channelDShort = drv_rtctResult.channelDStatus.Short;
        pRtctResult->un.ge_result.channelDMismatch = drv_rtctResult.channelDStatus.Mismatch;
    }



    return RT_ERR_OK;

}

/* Function Name:
 *      phy_8226b_rtct_start
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
 *      RT_ERR_CHIP_NOT_SUPPORTED - chip not supported
 * Note:
 *      RTCT is not supported when port link at 10M.
 */
int32
phy_8226b_rtct_start(uint32 unit, rtk_port_t port)
{
    HANDLE  hDevice;
    int32   ret;

    hDevice.unit = unit;
    hDevice.port = port;
    //osal_printf("%s:%u\n", __FUNCTION__, __LINE__);
    RTL8226_EC_CONV(Rtl8226b_rtct_start(hDevice), ret);
    return ret;
}

/* Function Name:
 *      phy_8226b_linkDownPowerSavingEnable_get
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
 * Note:
 *      None
 */
int32
phy_8226b_linkDownPowerSavingEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    HANDLE  hDevice;
    BOOL    drv_enable;
    int32   ret;

    hDevice.unit = unit;
    hDevice.port = port;

    RTL8226_EC_CONV(Rtl8226b_linkDownPowerSavingEnable_get(hDevice, &drv_enable), ret);

    *pEnable = (drv_enable == TRUE) ? ENABLED : DISABLED;

    return ret;
}

/* Function Name:
 *      phy_8226b_linkDownPowerSavingEnable_set
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
 * Note:
 *      None
 */
int32
phy_8226b_linkDownPowerSavingEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    HANDLE  hDevice;
    BOOL    drv_enable;
    int32   ret;

    hDevice.unit = unit;
    hDevice.port = port;

    //osal_printf("%s:%u\n", __FUNCTION__, __LINE__);
    if (enable == ENABLED)
        drv_enable = TRUE;
    else
        drv_enable = FALSE;

    RTL8226_EC_CONV(Rtl8226b_linkDownPowerSavingEnable_set(hDevice, drv_enable), ret);

    return ret;
}


/* Function Name:
 *      phy_8226b_ieeeTestMode_set
 * Description:
 *      Set test mode for PHY transmitter test
 * Input:
 *      unit  - unit id
 *      port  - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_NOT_ALLOWED - The operation is not allowed
 *      RT_ERR_PORT_NOT_SUPPORTED - test mode is not supported
 * Note:
 *      None
 */
int32
phy_8226b_ieeeTestMode_set(uint32 unit, rtk_port_t port, rtk_port_phyTestMode_t *pTestMode)
{
    HANDLE  hDevice;
    UINT16  drv_speed = 0;
    PHY_IEEE_TEST_MODE drv_ieeetestmode;
    int32   ret;

    RT_LOG(LOG_TRACE, MOD_PHY, "unit=%u port=%u mode=0x%x channel=%u", unit, port, pTestMode->mode, pTestMode->channel);

    hDevice.unit = unit;
    hDevice.port = port;

    osal_memset(&drv_ieeetestmode, 0, sizeof(PHY_IEEE_TEST_MODE));
    switch (pTestMode->mode)
    {
      case RTK_PORT_PHY_1G_TEST_MODE1:
        drv_ieeetestmode.TM1 = 1;
        drv_speed = LINK_SPEED_1G;
        break;
      case RTK_PORT_PHY_1G_TEST_MODE2:
        drv_ieeetestmode.TM2 = 1;
        drv_speed = LINK_SPEED_1G;
        break;
      case RTK_PORT_PHY_1G_TEST_MODE3:
        drv_ieeetestmode.TM3 = 1;
        drv_speed = LINK_SPEED_1G;
        break;
      case RTK_PORT_PHY_1G_TEST_MODE4:
        drv_ieeetestmode.TM4 = 1;
        drv_speed = LINK_SPEED_1G;
        break;

      case RTK_PORT_PHY_10G_TEST_MODE_NONE:
        drv_ieeetestmode.TMFINISH = 1;
        drv_speed = LINK_SPEED_2P5G;
        break;
      case RTK_PORT_PHY_10G_TEST_MODE1:
        drv_ieeetestmode.TM1 = 1;
        drv_speed = LINK_SPEED_2P5G;
        break;
      case RTK_PORT_PHY_10G_TEST_MODE2:
        drv_ieeetestmode.TM2 = 1;
        drv_speed = LINK_SPEED_2P5G;
        break;
      case RTK_PORT_PHY_10G_TEST_MODE3:
        drv_ieeetestmode.TM3 = 1;
        drv_speed = LINK_SPEED_2P5G;
        break;
      case RTK_PORT_PHY_10G_TEST_MODE4_DUALTONE1:
        drv_ieeetestmode.TM4 = 1;
        drv_ieeetestmode.TONE1 = 1;
        drv_speed = LINK_SPEED_2P5G;
        break;
      case RTK_PORT_PHY_10G_TEST_MODE4_DUALTONE2:
        drv_ieeetestmode.TM4 = 1;
        drv_ieeetestmode.TONE2 = 1;
        drv_speed = LINK_SPEED_2P5G;
        break;
      case RTK_PORT_PHY_10G_TEST_MODE4_DUALTONE3:
        drv_ieeetestmode.TM4 = 1;
        drv_ieeetestmode.TONE3 = 1;
        drv_speed = LINK_SPEED_2P5G;
        break;
      case RTK_PORT_PHY_10G_TEST_MODE4_DUALTONE4:
        drv_ieeetestmode.TM4 = 1;
        drv_ieeetestmode.TONE4 = 1;
        drv_speed = LINK_SPEED_2P5G;
        break;
      case RTK_PORT_PHY_10G_TEST_MODE4_DUALTONE5:
        drv_ieeetestmode.TM4 = 1;
        drv_ieeetestmode.TONE5 = 1;
        drv_speed = LINK_SPEED_2P5G;
        break;
      case RTK_PORT_PHY_10G_TEST_MODE5:
        drv_ieeetestmode.TM5 = 1;
        drv_speed = LINK_SPEED_2P5G;
        break;
      case RTK_PORT_PHY_10G_TEST_MODE6:
        drv_ieeetestmode.TM6 = 1;
        drv_speed = LINK_SPEED_2P5G;
        break;
      default:
        return RT_ERR_INPUT;
    }

    switch (pTestMode->channel)
    {
      case PHY_TESTMODE_CHANNEL_NONE:
        drv_ieeetestmode.channel = TESTMODE_CHANNEL_NONE;
        break;
      case PHY_TESTMODE_CHANNEL_A:
        drv_ieeetestmode.channel = TESTMODE_CHANNEL_A;
        break;
      case PHY_TESTMODE_CHANNEL_B:
        drv_ieeetestmode.channel = TESTMODE_CHANNEL_B;
        break;
      case PHY_TESTMODE_CHANNEL_C:
        drv_ieeetestmode.channel = TESTMODE_CHANNEL_C;
        break;
      case PHY_TESTMODE_CHANNEL_D:
        drv_ieeetestmode.channel = TESTMODE_CHANNEL_D;
        break;
      default:
        drv_ieeetestmode.channel = TESTMODE_CHANNEL_NONE;
        break;
    }

    RTL8226_EC_CONV(Rtl8226b_ieeeTestMode_set(hDevice, drv_speed, &drv_ieeetestmode), ret);

    if (ret != RT_ERR_OK)
    {
        RT_LOG(LOG_TRACE, MOD_PHY, "unit=%u port=%u set fail %X", unit, port, ret);
    }

    return ret;
}




/* Function Name:
 *      phy_8226b_macIntfSerdesMode_get
 * Description:
 *      Get PHY's MAC side interface serdes mode
 * Input:
 *      unit    - unit ID
 *      port    - Base mac ID of the PHY
 * Output:
 *      pSerdesMode  - PHY serdes mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_NOT_SUPPORTED   - This function is not supported by the PHY of this port
 * Note:
 *      None
 */
int32
phy_8226b_macIntfSerdesMode_get(uint32 unit, rtk_port_t port, rt_serdesMode_t *pSerdesMode)
{
    HANDLE  hDevice;
    BOOL    drv_sdsLink = 0;
    PHY_SERDES_MODE     drv_sdsMode = PHY_SERDES_MODE_NO_SDS;
    int32   ret;

    hDevice.unit = unit;
    hDevice.port = port;

    RTL8226_EC_CONV(Rtl8226b_serdes_link_get(hDevice, &drv_sdsLink, &drv_sdsMode), ret);

    if (ret != RT_ERR_OK)
        return ret;

    switch (drv_sdsMode)
    {
      case PHY_SERDES_MODE_SGMII:
        *pSerdesMode = RTK_MII_SGMII;
        break;
      case PHY_SERDES_MODE_HiSGMII:
        *pSerdesMode = RTK_MII_HISGMII;
        break;
      case PHY_SERDES_MODE_2500BASEX:
        *pSerdesMode = RTK_MII_2500Base_X;
        break;
      case PHY_SERDES_MODE_USXGMII:
        *pSerdesMode = RTK_MII_USXGMII_2_5GSXGMII;
        break;
      case PHY_SERDES_MODE_NO_SDS:
        *pSerdesMode = RTK_MII_DISABLE;
        break;
      default:
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}


/* Function Name:
 *      phy_8226b_macIntfSerdesLinkStatus_get
 * Description:
 *      Get PHY's MAC side interface serdes link status
 * Input:
 *      unit    - unit ID
 *      port    - Base port ID of the PHY
 * Output:
 *      pStatus - link status of the SerDes
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8226b_macIntfSerdesLinkStatus_get(uint32 unit, rtk_port_t port, rtk_phy_macIntfSdsLinkStatus_t *pStatus)
{
    HANDLE  hDevice;
    BOOL    drv_sdsLink = 0;
    PHY_SERDES_MODE     drv_sdsMode = PHY_SERDES_MODE_NO_SDS;
    int32   ret;

    hDevice.unit = unit;
    hDevice.port = port;

    RTL8226_EC_CONV(Rtl8226b_serdes_link_get(hDevice, &drv_sdsLink, &drv_sdsMode), ret);

    if (ret != RT_ERR_OK)
        return ret;

    pStatus->sds_num = 1;
    pStatus->link_status[0] = (drv_sdsLink) ? PORT_LINKUP : PORT_LINKDOWN;
    return RT_ERR_OK;
}


/* Function Name:
 *      phy_8226b_chipRevId_get
 * Description:
 *      Get chip revision ID
 * Input:
 *      unit   - unit id
 *      port   - port id
 * Output:
 *      pChip_rev_id - chip revision ID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
phy_8226b_chipRevId_get(uint32 unit, rtk_port_t port, uint16 *chip_rev_id)
{
    *chip_rev_id = 0;

    return RT_ERR_OK;
}



/* Function Name:
 *      phy_8226drv_mapperInit
 * Description:
 *      Initialize PHY 8226 driver.
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
phy_8226drv_mapperInit(rt_phydrv_t *pPhydrv)
{
    pPhydrv->phydrv_index = RT_PHYDRV_RTL8226;
    pPhydrv->fPhydrv_init = phy_8226_init;
    pPhydrv->fPhydrv_media_get = phy_8226_media_get;
    pPhydrv->fPhydrv_autoNegoEnable_get = phy_8226_autoNegoEnable_get;
    pPhydrv->fPhydrv_autoNegoEnable_set = phy_8226_autoNegoEnable_set;
    pPhydrv->fPhydrv_autoNegoAbility_get = phy_8226_autoNegoAbility_get;
    pPhydrv->fPhydrv_autoNegoAbility_set = phy_8226_autoNegoAbility_set;
    pPhydrv->fPhydrv_duplex_get = phy_8226_duplex_get;
    pPhydrv->fPhydrv_duplex_set = phy_8226_duplex_set;
    pPhydrv->fPhydrv_speed_get = phy_8226_speed_get;
    pPhydrv->fPhydrv_speed_set = phy_8226_speed_set;
    pPhydrv->fPhydrv_enable_set = phy_8226_enable_set;
    pPhydrv->fPhydrv_greenEnable_get = phy_8226_greenEnable_get;
    pPhydrv->fPhydrv_greenEnable_set = phy_8226_greenEnable_set;
    pPhydrv->fPhydrv_eeeEnable_get = phy_8226_eeeEnable_get;
    pPhydrv->fPhydrv_eeeEnable_set = phy_8226_eeeEnable_set;
    pPhydrv->fPhydrv_crossOverMode_get = phy_8226_crossOverMode_get;
    pPhydrv->fPhydrv_crossOverMode_set = phy_8226_crossOverMode_set;
    pPhydrv->fPhydrv_crossOverStatus_get = phy_8226_crossOverStatus_get;
    pPhydrv->fPhydrv_masterSlave_get = phy_8226_masterSlave_get;
    pPhydrv->fPhydrv_masterSlave_set = phy_8226_masterSlave_set;
    pPhydrv->fPhydrv_loopback_get = phy_8226_loopback_get;
    pPhydrv->fPhydrv_loopback_set = phy_8226_loopback_set;
    pPhydrv->fPhydrv_downSpeedEnable_get = phy_8226_downSpeedEnable_get;
    pPhydrv->fPhydrv_downSpeedEnable_set = phy_8226_downSpeedEnable_set;
    pPhydrv->fPhydrv_linkDownPowerSavingEnable_get = phy_8226_linkDownPowerSavingEnable_get;
    pPhydrv->fPhydrv_linkDownPowerSavingEnable_set = phy_8226_linkDownPowerSavingEnable_set;
    pPhydrv->fPhydrv_rtctResult_get = phy_8226_rtctResult_get;
    pPhydrv->fPhydrv_rtct_start = phy_8226_rtct_start;
    pPhydrv->fPhydrv_reg_mmd_get = phy_common_reg_mmd_get;
    pPhydrv->fPhydrv_reg_mmd_set = phy_common_reg_mmd_set;
    pPhydrv->fPhydrv_ieeeTestMode_set = phy_8226_ieeeTestMode_set;
    pPhydrv->fPhydrv_macIntfSerdesLinkStatus_get = phy_8226_macIntfSerdesLinkStatus_get;
    pPhydrv->fPhydrv_chipRevId_get = phy_8226_chipRevId_get;
    pPhydrv->fPhydrv_gigaLiteEnable_get = phy_8226_gigaLiteEnable_get;
    pPhydrv->fPhydrv_gigaLiteEnable_set = phy_8226_gigaLiteEnable_set;
    pPhydrv->fPhydrv_2pt5gLiteEnable_get = phy_8226_2pt5gLiteEnable_get;
    pPhydrv->fPhydrv_2pt5gLiteEnable_set = phy_8226_2pt5gLiteEnable_set;
    pPhydrv->fPhydrv_macIntfSerdesMode_get = phy_8226_macIntfSerdesMode_get;

} /* end of phy_8226drv_ge_mapperInit*/


/* Function Name:
 *      phy_8226drv_mapperInit
 * Description:
 *      Initialize PHY 8226B driver.
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
phy_8226bdrv_mapperInit(rt_phydrv_t *pPhydrv)
{
    pPhydrv->phydrv_index = RT_PHYDRV_RTL8226B;
    pPhydrv->fPhydrv_init = phy_8226b_init;
    pPhydrv->fPhydrv_media_get = phy_8226b_media_get;
    pPhydrv->fPhydrv_autoNegoEnable_get = phy_8226b_autoNegoEnable_get;
    pPhydrv->fPhydrv_autoNegoEnable_set = phy_8226b_autoNegoEnable_set;
    pPhydrv->fPhydrv_autoNegoAbility_get = phy_8226b_autoNegoAbility_get;
    pPhydrv->fPhydrv_autoNegoAbility_set = phy_8226b_autoNegoAbility_set;
    pPhydrv->fPhydrv_duplex_get = phy_8226b_duplex_get;
    pPhydrv->fPhydrv_duplex_set = phy_8226b_duplex_set;
    pPhydrv->fPhydrv_speed_get = phy_8226b_speed_get;
    pPhydrv->fPhydrv_speed_set = phy_8226b_speed_set;
    pPhydrv->fPhydrv_enable_set = phy_8226b_enable_set;
    pPhydrv->fPhydrv_greenEnable_get = phy_8226b_greenEnable_get;
    pPhydrv->fPhydrv_greenEnable_set = phy_8226b_greenEnable_set;
    pPhydrv->fPhydrv_eeeEnable_get = phy_8226b_eeeEnable_get;
    pPhydrv->fPhydrv_eeeEnable_set = phy_8226b_eeeEnable_set;
    pPhydrv->fPhydrv_crossOverMode_get = phy_8226b_crossOverMode_get;
    pPhydrv->fPhydrv_crossOverMode_set = phy_8226b_crossOverMode_set;
    pPhydrv->fPhydrv_crossOverStatus_get = phy_8226b_crossOverStatus_get;
    pPhydrv->fPhydrv_masterSlave_get = phy_8226b_masterSlave_get;
    pPhydrv->fPhydrv_masterSlave_set = phy_8226b_masterSlave_set;
    pPhydrv->fPhydrv_loopback_get = phy_8226b_loopback_get;
    pPhydrv->fPhydrv_loopback_set = phy_8226b_loopback_set;
    pPhydrv->fPhydrv_downSpeedEnable_get = phy_8226b_downSpeedEnable_get;
    pPhydrv->fPhydrv_downSpeedEnable_set = phy_8226b_downSpeedEnable_set;
    pPhydrv->fPhydrv_linkDownPowerSavingEnable_get = phy_8226b_linkDownPowerSavingEnable_get;
    pPhydrv->fPhydrv_linkDownPowerSavingEnable_set = phy_8226b_linkDownPowerSavingEnable_set;
    pPhydrv->fPhydrv_rtctResult_get = phy_8226b_rtctResult_get;
    pPhydrv->fPhydrv_rtct_start = phy_8226b_rtct_start;
    pPhydrv->fPhydrv_reg_mmd_get = phy_common_reg_mmd_get;
    pPhydrv->fPhydrv_reg_mmd_set = phy_common_reg_mmd_set;
    pPhydrv->fPhydrv_ieeeTestMode_set = phy_8226b_ieeeTestMode_set;
    pPhydrv->fPhydrv_macIntfSerdesLinkStatus_get = phy_8226b_macIntfSerdesLinkStatus_get;
    pPhydrv->fPhydrv_chipRevId_get = phy_8226b_chipRevId_get;
    pPhydrv->fPhydrv_gigaLiteEnable_get = phy_8226b_gigaLiteEnable_get;
    pPhydrv->fPhydrv_gigaLiteEnable_set = phy_8226b_gigaLiteEnable_set;
    pPhydrv->fPhydrv_2pt5gLiteEnable_get = phy_8226b_2pt5gLiteEnable_get;
    pPhydrv->fPhydrv_2pt5gLiteEnable_set = phy_8226b_2pt5gLiteEnable_set;
    pPhydrv->fPhydrv_macIntfSerdesMode_get = phy_8226b_macIntfSerdesMode_get;

} /* end of phy_8226bdrv_ge_mapperInit*/

