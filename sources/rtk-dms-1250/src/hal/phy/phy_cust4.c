/*
 * Copyright (C) 2019 Realtek Semiconductor Corp.
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
 * Purpose : Customer PHY Driver sample codes
 *
 * Feature : Customer PHY Driver sample codes
 *      -- In make menuconfig, select CUST4 PHY: Chip Support ---> [*] CUSTOMER PHY 4
 *         Macro "CONFIG_SDK_PHY_CUST4" will be enabled.
 *      -- The SDK defined PHY type is RTK_PHYTYPE_CUST4.
 *      -- If vender PHY C code needed to be build through SDK make, the following makefile can be used:
 *          sdk/build/Makefile.custPhy4
 *      -- Function: phy_identify_cust4() is used for checking if the port is CUST4 PHY in "PHY probe" stage during system init
 *          You need to know the PHY ID of the PHY for implement this function.
 *      -- Function: phy_cust4drv_mapperInit() is the initial for registering driver's callbacks of the PHY
 *          Review PHY's feature and hook or remove drivers in this function.
 *      -- Sample code functions of porting vender's PHY MDIO read/write:
 *          phy_cust4_sampleCodePortingMdio_read()
 *          phy_cust4_sampleCodePortingMdio_write()
 */

/*
 * Include Files
 */
/* RTK headers */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/debug/rt_log.h>
#include <soc/type.h>
#include <hal/common/halctrl.h>
#include <hal/mac/miim_common_drv.h>
#include <hal/phy/phy_cust4.h>

/* !! Customer's PHY driver header include */





/*
 * Symbol Definition
 */

/* !! number of ports in the PHY chip */
#define PHY_CUST4_PORT_NUM                  1

/* !! PHY ID of the CUST4 PHY */
#define PHY_CUST4_PHYID_REG2                0x5555
#define PHY_CUST4_PHYID_REG3                0xEEEE

/*
 * Data Declaration
 */
rt_phyInfo_t phy_cust4_info =
{
    .phy_num    = PHY_CUST4_PORT_NUM,
    .eth_type   = HWP_XGE, /* !!  Ethernet type of the PHY. Refer to rt_port_ethType_t  */
    .isComboPhy = {0, 0, 0, 0, 0, 0, 0, 0}, /* set to 1 if the port is combo port */
    .flags      = (RTK_PHYINFO_FLAG_NO_RES_REG),    /* PHY attribute flags. Set corrisponding flags for the PHY */

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


/*
 * Macro Declaration
 */



/*
 * Function Declaration
 */




/* Function Name:
 *      phy_identify_cust4
 * Description:
 *      Identify the port is CUST4 PHY or not
 *      This function will be called in "phy probe" stage for checking if the port's PHY is CUST4 PHY
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      model_id - model id (not used)
 *      rev_id   - revision id (not used)
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - port is CUST4 PHY, the port will then hook with this PHY driver
 *      RT_ERR_FAILED           - access failure or others
 *      RT_ERR_PHY_NOT_MATCH    - port is not CUST4 PHY
 * Note:
 *      None
 */
int32
phy_identify_cust4(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id)
{
    uint32          reg1d2 = 0, reg1d3 = 0;
    hal_control_t   *pHalCtrl = NULL;
    drv_smi_mdxProtoSel_t   mdxProto;

    RT_LOG(LOG_INFO, (MOD_HAL|MOD_PHY), "unit %u port %u cust4 identify", unit, port);
    RT_PARAM_CHK((NULL == (pHalCtrl = hal_ctrlInfo_get(unit))), RT_ERR_FAILED);

    /*
     * !! If switch's SMI bus supports both Clause 22 and 45 format.
     * Below sample code shows how to probing a PHY that supports Clause 45 format.
     */

    /* get current SMI bus MDIO protocol type */
    if (hal_miim_portSmiMdxProto_get(unit, port, &mdxProto) != RT_ERR_OK)
    {
        return RT_ERR_PHY_NOT_MATCH;
    }
    /* switch to Clause 45 if the bus is in other protocol type. */
    if (mdxProto != DRV_SMI_MDX_PROTO_C45)
    {
        RT_LOG(LOG_INFO, (MOD_HAL|MOD_PHY), "unit %u port %u change SMI to C45", unit, port);
        if (hal_miim_portSmiMdxProto_set(unit, port, DRV_SMI_MDX_PROTO_C45) != RT_ERR_OK)
        {
            RT_LOG(LOG_INFO, (MOD_HAL|MOD_PHY), "unit %u port %u change SMI to C45 fail", unit, port);
            /* PHY can only be accessed through C45 MDC/MDIO format. if SMI is not able to using C45, then return there is no match. */
            return RT_ERR_PHY_NOT_MATCH;
        }
    }

    /* !! Sample code of read PMA/PMD device identifier (Registers 1.2, 1.3) and check if there is a match of CUST4 PHY */
    MACDRV(pHalCtrl)->fMdrv_miim_mmd_read(unit, port, 1, 2, &reg1d2);
    MACDRV(pHalCtrl)->fMdrv_miim_mmd_read(unit, port, 1, 3, &reg1d3);
    RT_LOG(LOG_INFO, (MOD_HAL|MOD_PHY), "unit %u port %u mmd 1.2=0x%X 1.3=0x%X", unit, port, reg1d2, reg1d3);
    if ((reg1d2 == PHY_CUST4_PHYID_REG2) && (reg1d3 == PHY_CUST4_PHYID_REG3))
    {
        goto EXIT_OK;
    }

    /* recover SMI bus setting for this bus is not attached to CUST4 PHY */
    if (mdxProto != DRV_SMI_MDX_PROTO_C45)
    {
        RT_LOG(LOG_INFO, (MOD_HAL|MOD_PHY), "unit %u port %u restore SMI to C22", unit, port);
        hal_miim_portSmiMdxProto_set(unit, port, mdxProto);
    }

    return RT_ERR_PHY_NOT_MATCH;

  EXIT_OK:

    /* return OK when there is a match */
    RT_LOG(LOG_INFO, (MOD_HAL|MOD_PHY), "unit %u port %u cust4 identify match", unit, port);
    return RT_ERR_OK;
}


/* Function Name:
 *      phy_cust4_media_get
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
phy_cust4_media_get(uint32 unit, rtk_port_t port, rtk_port_media_t *pMedia)
{
    /* !! call the vender phy driver to get media type */
    *pMedia = PORT_MEDIA_COPPER; /* dummy code. */
    return RT_ERR_OK;
}


/* Function Name:
 *      phy_cust4_autoNegoEnable_get
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
phy_cust4_autoNegoEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    /* !! call the vender phy driver to get autonegotiation enable status */
    *pEnable = ENABLED; /* dummy code. */

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_cust4_autoNegoEnable_set
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
phy_cust4_autoNegoEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    RT_LOG(LOG_INFO, (MOD_HAL|MOD_PHY), "unit %u port %u cust4 autoNegoEnable_set:%u", unit, port, enable);

    /* !! call the vender phy driver to set autonegotiation enable status */

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_cust4_autoNegoAbility_get
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
phy_cust4_autoNegoAbility_get(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility)
{
    /* !! call the vender phy driver to set autonegotiation enable status */
    osal_memset(pAbility, 0, sizeof(rtk_port_phy_ability_t));
    pAbility->Full_1000 = 1; /* dummy code. */

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_cust4_autoNegoAbility_set
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
phy_cust4_autoNegoAbility_set(uint32 unit, rtk_port_t port, rtk_port_phy_ability_t *pAbility)
{

    RT_LOG(LOG_INFO, (MOD_HAL|MOD_PHY), "unit %u port %u cust4 autoNegoAbility_set", unit, port);

    /* !! call the vender phy driver to set autonegotiation enable status */
    if (pAbility); /* dummy code */


    return RT_ERR_OK;
}

/* Function Name:
 *      phy_cust4_duplex_get
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
phy_cust4_duplex_get(uint32 unit, rtk_port_t port, uint32 *pDuplex)
{
    /* !! call the vender phy driver to get duplex */
    *pDuplex = 1; /* dummy code. */

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_cust4_duplex_set
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
phy_cust4_duplex_set(uint32 unit, rtk_port_t port, uint32 duplex)
{
    RT_LOG(LOG_INFO, (MOD_HAL|MOD_PHY), "unit %u port %u cust4 duplex_set:%u", unit, port, duplex);

    /* !! call the vender phy driver to set autonegotiation enable status */


    return RT_ERR_OK;
}

/* Function Name:
 *      phy_cust4_speed_get
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
phy_cust4_speed_get(uint32 unit, rtk_port_t port, uint32 *pSpeed)
{
    /* !! call the vender phy driver to get speed. enum is rtk_port_speed_t. */
    *pSpeed = PORT_SPEED_1000M; /* dummy code. */

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_cust4_speed_set
 * Description:
 *      Set speed mode status of the specific port
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      speed         - link speed status 100
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - invalid parameter
 * Note:
 *      None
 */
int32
phy_cust4_speed_set(uint32 unit, rtk_port_t port, uint32 speed)
{

    RT_LOG(LOG_INFO, (MOD_HAL|MOD_PHY), "unit %u port %u cust4 speed_set:%u", unit, port, speed);

    /* !! call the vender phy driver to force speed */


    return RT_ERR_OK;

}

/* Function Name:
 *      phy_cust4_enable_set
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
phy_cust4_enable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    RT_LOG(LOG_INFO, (MOD_HAL|MOD_PHY), "unit %u port %u enable:%d", unit, port, enable);

    /* !! call the vender phy driver to enable/disable port */

    return RT_ERR_OK;
}


/* Function Name:
 *      phy_cust4_eeeEnable_get
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
phy_cust4_eeeEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    /* !! call the vender phy driver to get eee enable status */
    *pEnable = ENABLED; /* dummy code. */

    return RT_ERR_OK;
}


/* Function Name:
 *      phy_cust4_eeeEnable_set
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
phy_cust4_eeeEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    RT_LOG(LOG_INFO, (MOD_HAL|MOD_PHY), "unit %u port %u eeeEnable_set:%d", unit, port, enable);

    /* !! call the vender phy driver to set eee enable status */

    return RT_ERR_OK;
}



/* Function Name:
 *      phy_cust4_crossOverMode_get
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
phy_cust4_crossOverMode_get(uint32 unit, rtk_port_t port, rtk_port_crossOver_mode_t *pMode)
{

    /* !! call the vender phy driver to get cross over mode */
    *pMode = PORT_CROSSOVER_MODE_AUTO; /* dummy code */

    return RT_ERR_OK;
}


/* Function Name:
 *      phy_cust4_crossOverMode_set
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
phy_cust4_crossOverMode_set(uint32 unit, rtk_port_t port, rtk_port_crossOver_mode_t mode)
{

    RT_LOG(LOG_INFO, (MOD_HAL|MOD_PHY), "unit %u port %u crossOverMode:%d", unit, port, mode);

    /* !! call the vender phy driver to get cross over mode */

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_cust4_crossOverStatus_get
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
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_PHY_FIBER_LINKUP - This feature is not supported in this mode
 * Note:
 *      Following value is valid
 *      - PORT_CROSSOVER_STATUS_MDI
 *      - PORT_CROSSOVER_STATUS_MDIX
 */
int32
phy_cust4_crossOverStatus_get(uint32 unit, rtk_port_t port, rtk_port_crossOver_status_t *pStatus)
{
    /* !! call the vender phy driver to get cross over status */
    *pStatus = PORT_CROSSOVER_STATUS_MDIX; /* dummy code */

    return RT_ERR_OK;
}



/* Function Name:
 *      phy_cust4_masterSlave_get
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
phy_cust4_masterSlave_get(
    uint32              unit,
    rtk_port_t          port,
    rtk_port_masterSlave_t   *pMasterSlaveCfg,
    rtk_port_masterSlave_t   *pMasterSlaveActual)
{

    /* !! call the vender phy driver to get master-slave config and status */
    *pMasterSlaveCfg = PORT_AUTO_MODE;      /* dummy code */
    *pMasterSlaveActual = PORT_MASTER_MODE; /* dummy code */

    return RT_ERR_OK;

}

/* Function Name:
 *      phy_cust4_masterSlave_set
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
 * Note:
 *      None
 */
int32
phy_cust4_masterSlave_set(
    uint32              unit,
    rtk_port_t          port,
    rtk_port_masterSlave_t   masterSlave)
{
    RT_LOG(LOG_INFO, (MOD_HAL|MOD_PHY), "unit %u port %u masterSlave_set:%d", unit, port, masterSlave);

    /* !! call the vender phy driver to set master-slave */

    return RT_ERR_OK;

}

/* Function Name:
 *      phy_cust4_loopback_get
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
phy_cust4_loopback_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    /* !! call the vender phy driver to set loopback */
    *pEnable = DISABLED; /* dummy code */

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_cust4_loopback_set
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
phy_cust4_loopback_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    RT_LOG(LOG_INFO, (MOD_HAL|MOD_PHY), "unit %u port %u loopback_set:%d", unit, port, enable);

    /* !! call the vender phy driver to set loopback */

    return RT_ERR_OK;
}


/* Function Name:
 *      phy_cust4_rtctResult_get
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
 *      The result unit is cm
 */
int32
phy_cust4_rtctResult_get(uint32 unit, rtk_port_t port, rtk_rtctResult_t *pRtctResult)
{

    osal_memset(pRtctResult, 0, sizeof(rtk_rtctResult_t));

    /* !! call the vender phy driver to get cable diag result */

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_cust4_rtct_start
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
 *      None
 */
int32
phy_cust4_rtct_start(uint32 unit, rtk_port_t port)
{
    RT_LOG(LOG_INFO, (MOD_HAL|MOD_PHY), "unit %u port %u rtct_start:%d", unit, port);

    /* !! call the vender phy driver to cable diag */

    return RT_ERR_OK;
}


/* Function Name:
 *      phy_cust4_ieeeTestMode_set
 * Description:
 *      Set test mode for PHY transmitter test
 * Input:
 *      unit  - unit id
 *      port  - port id
 *      pTestMode->mode  - test mode RTK_PORT_PHY_1G_TEST_MODEx which is defined in IEEE 40.6.1.1.2 for 1G.
 *                         RTK_PORT_PHY_10G_TEST_MODEx is for 10G test.
 *                         RTK_PORT_PHY_100M_TEST_MODE_xx is for 100M test.
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
phy_cust4_ieeeTestMode_set(uint32 unit, rtk_port_t port, rtk_port_phyTestMode_t *pTestMode)
{

    RT_LOG(LOG_INFO, (MOD_HAL|MOD_PHY), "unit %u port %u ieeeTestMode_set: mode=0x%x", unit, port, pTestMode->mode);

    /* !! call the vender phy driver to set test mode */

    return RT_ERR_OK;
}


/* Function Name:
 *      phy_cust4_reg_mmd_set
 * Description:
 *      Get PHY registers.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      mmdAddr - mmd device address
 *      mmdReg  - mmd reg id
 * Output:
 *      pData   - pointer buffer of read data
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      None
 */
int32
phy_cust4_reg_mmd_get(uint32 unit, rtk_port_t port, uint32 mmdAddr, uint32 mmdReg, uint32 *pData)
{
    int32       ret;

    if ((ret = hal_miim_mmd_read(unit, port, mmdAddr, mmdReg, pData)) != RT_ERR_OK)
    {
        RT_LOG(LOG_WARNING, (MOD_HAL|MOD_PHY), "unit %u port %u cust4 mmd 0x%X reg 0x%X get fail %x", unit, port, mmdAddr, mmdReg, ret);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_common_reg_mmd_set
 * Description:
 *      Set PHY registers.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      mmdAddr - mmd device address
 *      mmdReg  - mmd reg id
 *      data    - write data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      None
 */
int32
phy_cust4_reg_mmd_set(uint32 unit, rtk_port_t port, uint32 mmdAddr, uint32 mmdReg, uint32 data)
{
    int32       ret;

    if ((ret = hal_miim_mmd_write(unit, port, mmdAddr, mmdReg, data)) != RT_ERR_OK)
    {
        RT_LOG(LOG_WARNING, (MOD_HAL|MOD_PHY), "unit %u port %u cust4 mmd 0x%X reg 0x%X set fail %x", unit, port, mmdAddr, mmdReg, ret);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      phy_cust4_init
 * Description:
 *      Initialize PHY.
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
phy_cust4_init(uint32 unit, rtk_port_t port)
{

    RT_LOG(LOG_INFO, (MOD_HAL|MOD_PHY), "unit %u port %u cust4 init", unit, port);

    /* !! call vender PHY initial API */

    return RT_ERR_OK;
}


/* Function Name:
 *      phy_cust4drv_mapperInit
 * Description:
 *      Initialize PHY driver.
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
phy_cust4drv_mapperInit(rt_phydrv_t *pPhydrv)
{
    /* !! the basic PHY driver are listed below for your reference.
     *    Remove the code if the vender's PHY does not support the function.
     *    Hook a driver callback if need to provide other functions that listed in rt_phydrv_t.
     */
    pPhydrv->phydrv_index = RT_PHYDRV_CUST4;
    pPhydrv->fPhydrv_init = phy_cust4_init;
    pPhydrv->fPhydrv_media_get = phy_cust4_media_get;
    pPhydrv->fPhydrv_autoNegoEnable_get = phy_cust4_autoNegoEnable_get;
    pPhydrv->fPhydrv_autoNegoEnable_set = phy_cust4_autoNegoEnable_set;
    pPhydrv->fPhydrv_autoNegoAbility_get = phy_cust4_autoNegoAbility_get;
    pPhydrv->fPhydrv_autoNegoAbility_set = phy_cust4_autoNegoAbility_set;
    pPhydrv->fPhydrv_duplex_get = phy_cust4_duplex_get;
    pPhydrv->fPhydrv_duplex_set = phy_cust4_duplex_set;
    pPhydrv->fPhydrv_speed_get = phy_cust4_speed_get;
    pPhydrv->fPhydrv_speed_set = phy_cust4_speed_set;
    pPhydrv->fPhydrv_enable_set = phy_cust4_enable_set;

    pPhydrv->fPhydrv_reg_mmd_get = phy_cust4_reg_mmd_get;
    pPhydrv->fPhydrv_reg_mmd_set = phy_cust4_reg_mmd_set;

#if !defined(__BOOTLOADER__)
    pPhydrv->fPhydrv_eeeEnable_get = phy_cust4_eeeEnable_get;
    pPhydrv->fPhydrv_eeeEnable_set = phy_cust4_eeeEnable_set;
    pPhydrv->fPhydrv_crossOverMode_get = phy_cust4_crossOverMode_get;
    pPhydrv->fPhydrv_crossOverMode_set = phy_cust4_crossOverMode_set;
    pPhydrv->fPhydrv_crossOverStatus_get = phy_cust4_crossOverStatus_get;
    pPhydrv->fPhydrv_masterSlave_get = phy_cust4_masterSlave_get;
    pPhydrv->fPhydrv_masterSlave_set = phy_cust4_masterSlave_set;
    pPhydrv->fPhydrv_loopback_get = phy_cust4_loopback_get;
    pPhydrv->fPhydrv_loopback_set = phy_cust4_loopback_set;
    pPhydrv->fPhydrv_ieeeTestMode_set = phy_cust4_ieeeTestMode_set;
    pPhydrv->fPhydrv_rtctResult_get = phy_cust4_rtctResult_get;
    pPhydrv->fPhydrv_rtct_start =  phy_cust4_rtct_start;
#endif

}



/* Function Name:
 *      phy_cust4_sampleCodePortingMdio_write
 * Description:
 *      !! This is a sample code of how to use MDIO API for porting vender PHY driver MDIO write
 * Input:
 *      unit - unit id
 *      port - port id
 *      mmd  - MMD
 *      address  - register address
 *      data - write data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
void
phy_cust4_sampleCodePortingMdio_write (unsigned int unit, unsigned int port, unsigned int mmd, unsigned int address, unsigned int data)
{
    int32           ret;

    RT_LOG(LOG_INFO, (MOD_HAL|MOD_PHY), "unit=%d, port=%d %u.%u data=%x", unit, port, mmd, address, data);

    if ((ret = hal_miim_mmd_write(unit, port, mmd, address, data)) != RT_ERR_OK)
    {
        RT_LOG(LOG_INFO, (MOD_HAL|MOD_PHY), "unit %u port %u mmd 0x%X reg 0x%X write fail %x", unit, port, mmd, address, ret);
    }

    return;
}

/* Function Name:
 *      phy_cust4_sampleCodePortingMdio_read
 * Description:
 *      !! This is a sample code of how to use MDIO API for porting vender PHY driver MDIO read.
 * Input:
 *      unit - unit id
 *      port - port id
 *      mmd  - MMD
 *      address  - register address
 * Output:
 *      None
 * Return:
 *      data - read data
 * Note:
 *      None
 */
unsigned int
phy_cust4_sampleCodePortingMdio_read (unsigned int unit, unsigned int port, unsigned int mmd, unsigned int address)
{
    uint32          data = 0;
    int32           ret;

    if ((ret = hal_miim_mmd_read(unit, port, mmd, address, &data)) != RT_ERR_OK)
    {
        RT_LOG(LOG_WARNING, (MOD_HAL|MOD_PHY), "unit %u port %u mmd 0x%X reg 0x%X read fail %x", unit, port, mmd, address, ret);
        return 0;
    }

    return data;
}
