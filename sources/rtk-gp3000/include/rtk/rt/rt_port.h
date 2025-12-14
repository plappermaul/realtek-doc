/*
 * Copyright (C) 2012 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 82413 $
 * $Date: 2017-09-22 16:38:09 +0800 (Fri, 22 Sep 2017) $
 *
 * Purpose : Definition of Port API
 *
 * Feature : The file have include the following module and sub-modules
 *           (1) Parameter settings for the port-based view
 */

#ifndef __RT_PORT_H__
#define __RT_PORT_H__

/*
 * Include Files
 */

/*
 * Symbol Definition
 */
typedef enum rt_port_linkStatus_e
{
    RT_PORT_LINKDOWN = 0,
    RT_PORT_LINKUP,
    RT_PORT_LINKSTATUS_END
} rt_port_linkStatus_t;

typedef enum rt_port_duplex_e
{
    RT_PORT_HALF_DUPLEX = 0,
    RT_PORT_FULL_DUPLEX,
    RT_PORT_DUPLEX_END
} rt_port_duplex_t;

typedef enum rt_port_speed_e
{
    RT_PORT_SPEED_10M = 0,
    RT_PORT_SPEED_100M,
    RT_PORT_SPEED_1000M,
    RT_PORT_SPEED_500M,    
    RT_PORT_SPEED_10G,
    RT_PORT_SPEED_2G5,
    RT_PORT_SPEED_5G,
    RT_PORT_SPEED_2G5LITE,   
    RT_PORT_SPEED_5GLITE,
    RT_PORT_SPEED_END
} rt_port_speed_t;

typedef struct rt_port_phy_ability_s
{
    uint32 Half_10:1;
    uint32 Full_10:1;
    uint32 Half_100:1;
    uint32 Full_100:1;
    uint32 Half_1000:1;
    uint32 Full_1000:1;
    uint32 Full_2500:1;
    uint32 Full_5000:1;
    uint32 Full_10000:1;
    uint32 FC:1;            /* Flow control */
    uint32 AsyFC:1;         /* Asymmetric flow control */
} rt_port_phy_ability_t;

typedef struct rt_port_macAbility_s
{
    rt_port_speed_t        speed;
    rt_port_duplex_t       duplex;
	rt_enable_t 		    linkFib1g;
	rt_port_linkStatus_t 	linkStatus;
	rt_enable_t 		    txFc;
	rt_enable_t 		    rxFc;
	rt_enable_t 		    nwayAbility;
	rt_enable_t 		    masterMod;
	rt_enable_t 		    nwayFault;
	rt_enable_t 		    lpi_100m;
	rt_enable_t 		    lpi_giga;
} rt_port_macAbility_t;

typedef enum rt_port_phy_reg_e
{
    RT_PHY_REG_CONTROL             = 0,
    RT_PHY_REG_STATUS,
    RT_PHY_REG_IDENTIFIER_1,
    RT_PHY_REG_IDENTIFIER_2,
    RT_PHY_REG_AN_ADVERTISEMENT,
    RT_PHY_REG_AN_LINKPARTNER,
    RT_PHY_REG_1000_BASET_CONTROL  = 9,
    RT_PHY_REG_1000_BASET_STATUS,
    RT_PHY_REG_END                 = 32
} rt_port_phy_reg_t;

typedef enum rt_port_isoConfig_e
{
    RT_PORT_ISO_CFG_0 = 0,      /* Only use for RTL9607 series, for others use RT_PORT_ISO_CFG_0 */
    RT_PORT_ISO_CFG_1,          /* Only use for RTL9607 series, for others use RT_PORT_ISO_CFG_0 */
    RT_PORT_ISO_CFG_END
} rt_port_isoConfig_t;

typedef enum rt_port_uniType_e
{
    RT_PORT_UNI_TYPE_VEIP = 0,
    RT_PORT_UNI_TYPE_PPTP,
    RT_PORT_UNI_TYPE_END
} rt_port_uniType_t;

/*
 * Function Declaration
 */

/* Function Name:
 *      rt_port_init
 * Description:
 *      Initialize port module of the specified device.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - failed
 * Note:
 *      Module must be initialized before using all of APIs in this module
 */
extern int32
rt_port_init(void);

/* Function Name:
 *      rt_port_portDef_init
 * Description:
 *      Initialize port define initializations
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - failed
 * Note:
 *      Only user space should call this to initialize the port definitions
 */
extern int32
rt_port_portDef_init(void);

/* Function Name:
 *      rt_port_link_get
 * Description:
 *      Get the link status of the specific port
 * Input:
 *      port    - port id
 * Output:
 *      pLinkStatus - pointer to the link status
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - failed
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The link status of the port is as following:
 *      - LINKDOWN
 *      - LINKUP
 */
extern int32
rt_port_link_get(rt_port_t port, rt_port_linkStatus_t *pLinkStatus);

/* Function Name:
 *      rt_port_speedDuplex_get
 * Description:
 *      Get the negotiated port speed and duplex status of the specific port
 * Input:
 *      port    - port id
 * Output:
 *      pSpeed  - pointer to the port speed
 *      pDuplex - pointer to the port duplex
 * Return:
 *      RT_ERR_OK            - OK
 *      RT_ERR_FAILED        - failed
 *      RT_ERR_UNIT_ID       - invalid unit id
 *      RT_ERR_PORT_ID       - invalid port id
 *      RT_ERR_NULL_POINTER  - input parameter may be null pointer
 *      RT_ERR_PORT_LINKDOWN - link down port status
 * Note:
 *      (1) The speed type of the port is as following:
 *          - PORT_SPEED_10M
 *          - PORT_SPEED_100M
 *          - PORT_SPEED_1000M
 *
 *      (2) The duplex mode of the port is as following:
 *          - HALF_DUPLEX
 *          - FULL_DUPLEX
 */
extern int32
rt_port_speedDuplex_get(
    rt_port_t        port,
    rt_port_speed_t  *pSpeed,
    rt_port_duplex_t *pDuplex);

/* Function Name:
 *      rt_port_flowctrl_get
 * Description:
 *      Get the negotiated flow control status of the specific port
 * Input:
 *      port      - port id
 * Output:
 *      pTxStatus - pointer to the negotiation result of the Tx flow control
 *      pRxStatus - pointer to the negotiation result of the Rx flow control
 * Return:
 *      RT_ERR_OK            - OK
 *      RT_ERR_FAILED        - failed
 *      RT_ERR_PORT_ID       - invalid port id
 *      RT_ERR_NULL_POINTER  - input parameter may be null pointer
 *      RT_ERR_PORT_LINKDOWN - link down port status
 * Note:
 *      None
 */
extern int32
rt_port_flowctrl_get(
    rt_port_t  port,
    uint32      *pTxStatus,
    uint32      *pRxStatus);

/* Function Name:
 *      rt_port_phyAutoNegoEnable_get
 * Description:
 *      Get PHY ability of the specific port
 * Input:
 *      port    - port id
 * Output:
 *      pEnable - pointer to PHY auto negotiation status
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - failed
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
rt_port_phyAutoNegoEnable_get(rt_port_t port, rt_enable_t *pEnable);

/* Function Name:
 *      rt_port_phyAutoNegoEnable_set
 * Description:
 *      Set PHY ability of the specific port
 * Input:
 *      port   - port id
 *      enable - enable PHY auto negotiation
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK      - OK
 *      RT_ERR_FAILED  - failed
 *      RT_ERR_UNIT_ID - invalid unit id
 *      RT_ERR_PORT_ID - invalid port id
 *      RT_ERR_INPUT   - input parameter out of range
 * Note:
 *      - ENABLED : switch to PHY auto negotiation mode
 *      - DISABLED: switch to PHY force mode
 *      - Once the abilities of both auto-nego and force mode are set,
 *        you can freely swtich the mode without calling ability setting API again
 */
extern int32
rt_port_phyAutoNegoEnable_set(rt_port_t port, rt_enable_t enable);

/* Function Name:
 *      rt_port_phyAutoNegoAbility_get
 * Description:
 *      Get PHY auto negotiation ability of the specific port
 * Input:
 *      port    - port id
 * Output:
 *      pAbility - pointer to the PHY ability
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - failed
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
rt_port_phyAutoNegoAbility_get(
    rt_port_t              port,
    rt_port_phy_ability_t  *pAbility);

/* Function Name:
 *      rt_port_phyAutoNegoAbility_set
 * Description:
 *      Set PHY auto negotiation ability of the specific port
 * Input:
 *      port     - port id
 *      pAbility - pointer to the PHY ability
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - failed
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      You can set these abilities no matter which mode PHY currently stays on
 */
extern int32
rt_port_phyAutoNegoAbility_set(
    rt_port_t              port,
    rt_port_phy_ability_t  *pAbility);

/* Function Name:
 *      rt_port_phyForceModeAbility_get
 * Description:
 *      Get PHY ability status of the specific port
 * Input:
 *      port         - port id
 * Output:
 *      pSpeed       - pointer to the port speed
 *      pDuplex      - pointer to the port duplex
 *      pFlowControl - pointer to the flow control enable status
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - failed
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
rt_port_phyForceModeAbility_get(
    rt_port_t          port,
    rt_port_speed_t    *pSpeed,
    rt_port_duplex_t   *pDuplex,
    rt_enable_t        *pFlowControl);

/* Function Name:
 *      rt_port_phyForceModeAbility_set
 * Description:
 *      Set the port speed/duplex mode/pause/asy_pause in the PHY force mode
 * Input:
 *      port        - port id
 *      speed       - port speed
 *      duplex      - port duplex mode
 *      flowControl - enable flow control
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK         - OK
 *      RT_ERR_FAILED     - failed
 *      RT_ERR_PORT_ID    - invalid port id
 *      RT_ERR_PHY_SPEED  - invalid PHY speed setting
 *      RT_ERR_PHY_DUPLEX - invalid PHY duplex setting
 *      RT_ERR_INPUT      - invalid input parameter
 * Note:
 *      (1) You can set these abilities no matter which mode PHY currently stays on
 *
 *      (2) The speed type of the port is as following:
 *          - PORT_SPEED_10M
 *          - PORT_SPEED_100M
 *
 *      (3) The duplex mode of the port is as following:
 *          - HALF_DUPLEX
 *          - FULL_DUPLEX
 */
extern int32
rt_port_phyForceModeAbility_set(
    rt_port_t          port,
    rt_port_speed_t    speed,
    rt_port_duplex_t   duplex,
    rt_enable_t        flowControl);

/* Function Name:
 *      rt_port_phyReg_get
 * Description:
 *      Get PHY register data of the specific port
 * Input:
 *      port  - port id
 *      page  - page id
 *      reg   - reg id
 * Output:
 *      pData - pointer to the PHY reg data
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - failed
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_PHY_PAGE_ID  - invalid page id
 *      RT_ERR_PHY_REG_ID   - invalid reg id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
rt_port_phyReg_get(
    rt_port_t          port,
    uint32              page,
    rt_port_phy_reg_t  reg,
    uint32              *pData);

/* Function Name:
 *      rt_port_phyReg_set
 * Description:
 *      Set PHY register data of the specific port
 * Input:
 *      port - port id
 *      page - page id
 *      reg  - reg id
 *      data - reg data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK          - OK
 *      RT_ERR_FAILED      - failed
 *      RT_ERR_PORT_ID     - invalid port id
 *      RT_ERR_PHY_PAGE_ID - invalid page id
 *      RT_ERR_PHY_REG_ID  - invalid reg id
 * Note:
 *      None
 */
extern int32
rt_port_phyReg_set(
    rt_port_t          port,
    uint32              page,
    rt_port_phy_reg_t  reg,
    uint32              data);

/* Function Name:
 *      rt_port_phyPowerDown_get
 * Description:
 *      Get PHY power down state of the specific port
 * Input:
 *      port    - port id
 *      pEnable - enable state PHY power down function
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - failed
 *      RT_ERR_PORT_ID     - invalid port id
 * Note:
 *      None
 */
extern int32
rt_port_phyPowerDown_get(rt_port_t port, rt_enable_t *pEnable);

/* Function Name:
 *      rt_port_phyPowerDown_set
 * Description:
 *      Set PHY power down of the specific port
 * Input:
 *      port   - port id
 *      enable - enable PHY power down function
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - failed
 *      RT_ERR_PORT_ID     - invalid port id
 * Note:
 *      None
 */
extern int32
rt_port_phyPowerDown_set(rt_port_t port, rt_enable_t enable);

/* Function Name:
 *      rt_port_adminEnable_get
 * Description:
 *      Get port admin status of the specific port
 * Input:
 *      port    - port id
 * Output:
 *      pEnable - pointer to the port admin status
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - failed
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
rt_port_adminEnable_get(rt_port_t port, rt_enable_t *pEnable);

/* Function Name:
 *      rt_port_adminEnable_set
 * Description:
 *      Set port admin status of the specific port
 * Input:
 *      port    - port id
 *      enable  - port admin status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - failed
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      None
 */
extern int32
rt_port_adminEnable_set(rt_port_t port, rt_enable_t enable);

/* Function Name:
 *      rt_port_isolationEntry_get
 * Description:
 *      Get Port isolation portmask
 * Input:
 *      mode            - Configuration 0 or 1, only use for RTL9607 series
 *      port            - Ingress port
 * Output:
 *      pPortmask       - Isolation portmask for specified ingress port.
 *      pExtPortmask    - Isolation extension portmask for specified ingress port.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - failed
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None.
 */
extern int32
rt_port_isolationEntry_get(rt_port_isoConfig_t mode, rt_port_t port, rt_portmask_t *pPortmask, rt_portmask_t *pExtPortmask);

/* Function Name:
 *      rt_port_isolationEntry_set
 * Description:
 *      Set Port isolation portmask
 * Input:
 *      mode            - Configuration 0 or 1, only use for RTL9607 series
 *      port            - Ingress port
 *      pPortmask       - Isolation portmask for specified ingress port.
 *      pExtPortmask    - Isolation extension portmask for specified ingress port.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - failed
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      pExtPortmask is the extension egress portmask toward CPU port.
 *      If users specify an empty extension portmask and CPU port is set in pPortmask,
 *      the packets will be restricted to be forwarded to CPU.
 *      Likewise, If users specify an non-empty extension portmask and CPU port is not set in pPortmask,
 *      the packets will be restricted to be forwarded to CPU. too.
 */
extern int32
rt_port_isolationEntry_set(rt_port_isoConfig_t mode, rt_port_t port, rt_portmask_t *pPortmask, rt_portmask_t *pExtPortmask);


/* Function Name:
 *      rt_port_uniType_get
 * Description:
 *      Get port uni type of the specific port
 * Input:
 *      port     - port id
 * Output:
 *      pUniType - pointer to the port uni type
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - failed
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
rt_port_uniType_get(rt_port_t port, rt_port_uniType_t *pUniType);

/* Function Name:
 *      rt_port_uniType_set
 * Description:
 *      Set port uni type of the specific port
 * Input:
 *      port    - port id
 *      uniType - port type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - failed
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      None
 */
extern int32
rt_port_uniType_set(rt_port_t port, rt_port_uniType_t uniType);

/* Function Name:
 *      rt_port_macForceAbility_set
 * Description:
 *      Set MAC forece ability
 * Input:
 *      port - the ports for set ability
 *      macAbility - mac ability value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      For some ASIC that support QXGMII, this API only use to change the 
 *      ability of QXGMII ports.
 */
extern int32
rt_port_macForceAbility_set(rt_port_t port, rt_port_macAbility_t macAbility);

/* Function Name:
 *      rt_port_macForceAbility_get
 * Description:
 *      Set MAC forece ability
 * Input:
 *      port - the ports for get ability
 *      macAbility - mac ability value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      For some ASIC that support QXGMII, this API only use to the 
 *      ability of QXGMII ports.
 */
extern int32
rt_port_macForceAbility_get(rt_port_t port, rt_port_macAbility_t *pMacAbility);

/* Function Name:
 *      rt_port_macForceAbility_set
 * Description:
 *      Set MAC forece ability state
 * Input:
 *      port - the ports for set ability
 *      state - mac ability state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      For some ASIC that support QXGMII, this API doesn't support at all
 */
extern int32
rt_port_macForceAbilityState_set(rt_port_t port, rt_enable_t state);


/* Function Name:
 *      rt_port_macForceAbilityState_get
 * Description:
 *      Get MAC forece ability state
 * Input:
 *      port - the ports for get ability
 *      pState - mac ability state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      For some ASIC that support QXGMII, this API only use to get the 
 *      MAC force state of QXGMII ports.
 */
extern int32
rt_port_macForceAbilityState_get(rt_port_t port, rt_enable_t *pState);

/* Function Name:
 *      rt_port_eeeEnable_set
 * Description:
 *      Set EEE enable/disable state 
 * Input:
 *      port   - port id
 *      enable - enable status of EEE
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 * Note:
 *      None
 */
extern int32
rt_port_eeeEnable_set(rt_port_t port, rt_enable_t enable);

/* Function Name:
 *      rt_port_eeeEnable_get
 * Description:
 *      Get EEE enable/disable state 
 * Input:
 *      port   - port id
 * Output:
 *      pEnable - enable status of EEE
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
rt_port_eeeEnable_get(rt_port_t port, rt_enable_t *pEnable);

/* Function Name:
 *      rt_port_macLoopbackEnable_get
 * Description:
 *      Get the mac loopback enable status of the specific port
 * Input:
 *      port    - port id
 * Output:
 *      pEnable     - pointer to the enable status of mac local loopback
 *      pSwapEnable - pointer to the enable status of mac DA/SA swap
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      
 */
extern int32
rt_port_macLoopbackEnable_get(rt_port_t port, rt_enable_t *pEnable, rt_enable_t *pSwapEnable);

/* Function Name:
 *      rt_port_macLoopbackEnable_set
 * Description:
 *      Set the mac loopback enable status of the specific port
 * Input:
 *      port       - port id
 *      enable     - enable status of mac local loopback
 *      swapEnable - mac DA/SA swap
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 * Note:
 *      
 */
extern int32
rt_port_macLoopbackEnable_set(rt_port_t port, rt_enable_t enable, rt_enable_t swapEnable);
#endif

