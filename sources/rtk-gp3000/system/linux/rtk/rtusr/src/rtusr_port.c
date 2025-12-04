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
 * $Revision$
 * $Date$
 *
 * Purpose : Definition of Port API
 *
 * Feature : The file have include the following module and sub-modules
 *           (1) Parameter settings for the port-based view
 *           (2) RTCT
 */




/*
 * Include Files
 */
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <osal/lib.h>
#include <rtk/rtusr/include/rtusr_util.h>
#include <rtdrv/rtdrv_netfilter.h>
#include <common/rt_type.h>
#include <rtk/vlan.h>

/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */


/*
 * Function Declaration
 */

/* Function Name:
 *      rtk_port_init
 * Description:
 *      Initialize port module of the specified device.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Module must be initialized before using all of APIs in this module
 */
int32
rtk_port_init(void)
{
    rtdrv_portCfg_t port_cfg;

    /* function body */
    osal_memset(&port_cfg, 0x0, sizeof(rtdrv_portCfg_t));
    SETSOCKOPT(RTDRV_PORT_INIT, &port_cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_port_init */

/* Module Name    : Port                                       */
/* Sub-module Name: Parameter settings for the port-based view */

/* Function Name:
 *      rtk_port_link_get
 * Description:
 *      Get the link status of the specific port
 * Input:
 *      port    - port id
 * Output:
 *      pLinkStatus - pointer to the link status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The link status of the port is as following:
 *      - LINKDOWN
 *      - LINKUP
 */
int32
rtk_port_link_get(rtk_port_t port, rtk_port_linkStatus_t *pLinkStatus)
{
    rtdrv_portCfg_t port_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pLinkStatus), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&port_cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_PORT_LINK_GET, &port_cfg, rtdrv_portCfg_t, 1);
    osal_memcpy(pLinkStatus, &port_cfg.linkStatus, sizeof(rtk_port_linkStatus_t));

    return RT_ERR_OK;
}   /* end of rtk_port_link_get */

/* Function Name:
 *      rtk_port_speedDuplex_get
 * Description:
 *      Get the negotiated port speed and duplex status of the specific port
 * Input:
 *      port    - port id
 * Output:
 *      pSpeed  - pointer to the port speed
 *      pDuplex - pointer to the port duplex
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
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
int32
rtk_port_speedDuplex_get(
    rtk_port_t        port,
    rtk_port_speed_t  *pSpeed,
    rtk_port_duplex_t *pDuplex)
{
    rtdrv_portCfg_t port_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pSpeed), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pDuplex), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&port_cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_PORT_SPEEDDUPLEX_GET, &port_cfg, rtdrv_portCfg_t, 1);
    osal_memcpy(pSpeed, &port_cfg.speed, sizeof(rtk_port_speed_t));
    osal_memcpy(pDuplex, &port_cfg.duplex, sizeof(rtk_port_duplex_t));

    return RT_ERR_OK;
}   /* end of rtk_port_speedDuplex_get */

/* Function Name:
 *      rtk_port_flowctrl_get
 * Description:
 *      Get the negotiated flow control status of the specific port
 * Input:
 *      port      - port id
 * Output:
 *      pTxStatus - pointer to the negotiation result of the Tx flow control
 *      pRxStatus - pointer to the negotiation result of the Rx flow control
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID       - invalid port id
 *      RT_ERR_NULL_POINTER  - input parameter may be null pointer
 *      RT_ERR_PORT_LINKDOWN - link down port status
 * Note:
 *      None
 */
int32
rtk_port_flowctrl_get(
    rtk_port_t  port,
    uint32      *pTxStatus,
    uint32      *pRxStatus)
{
    rtdrv_portCfg_t port_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pTxStatus), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pRxStatus), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&port_cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_PORT_FLOWCTRL_GET, &port_cfg, rtdrv_portCfg_t, 1);
    osal_memcpy(pTxStatus, &port_cfg.txStatus, sizeof(uint32));
    osal_memcpy(pRxStatus, &port_cfg.rxStatus, sizeof(uint32));

    return RT_ERR_OK;
}   /* end of rtk_port_flowctrl_get */

/* Function Name:
 *      rtk_port_phyAutoNegoEnable_get
 * Description:
 *      Get PHY ability of the specific port
 * Input:
 *      port    - port id
 * Output:
 *      pEnable - pointer to PHY auto negotiation status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtk_port_phyAutoNegoEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    rtdrv_portCfg_t port_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&port_cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_PORT_PHYAUTONEGOENABLE_GET, &port_cfg, rtdrv_portCfg_t, 1);
    osal_memcpy(pEnable, &port_cfg.enable, sizeof(rtk_enable_t));

    return RT_ERR_OK;
}   /* end of rtk_port_phyAutoNegoEnable_get */

/* Function Name:
 *      rtk_port_phyAutoNegoEnable_set
 * Description:
 *      Set PHY ability of the specific port
 * Input:
 *      port   - port id
 *      enable - enable PHY auto negotiation
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 *      RT_ERR_PORT_ID - invalid port id
 *      RT_ERR_INPUT   - input parameter out of range
 * Note:
 *      - ENABLED : switch to PHY auto negotiation mode
 *      - DISABLED: switch to PHY force mode
 *      - Once the abilities of both auto-nego and force mode are set,
 *        you can freely swtich the mode without calling ability setting API again
 */
int32
rtk_port_phyAutoNegoEnable_set(rtk_port_t port, rtk_enable_t enable)
{
    rtdrv_portCfg_t port_cfg;

    /* function body */
    osal_memset(&port_cfg, 0x0, sizeof(rtdrv_portCfg_t));
    osal_memcpy(&port_cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&port_cfg.enable, &enable, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_PORT_PHYAUTONEGOENABLE_SET, &port_cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_port_phyAutoNegoEnable_set */

/* Function Name:
 *      rtk_port_phyAutoNegoAbility_get
 * Description:
 *      Get PHY auto negotiation ability of the specific port
 * Input:
 *      port    - port id
 * Output:
 *      pAbility - pointer to the PHY ability
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtk_port_phyAutoNegoAbility_get(
    rtk_port_t              port,
    rtk_port_phy_ability_t  *pAbility)
{
    rtdrv_portCfg_t port_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pAbility), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&port_cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_PORT_PHYAUTONEGOABILITY_GET, &port_cfg, rtdrv_portCfg_t, 1);
    osal_memcpy(pAbility, &port_cfg.ability, sizeof(rtk_port_phy_ability_t));

    return RT_ERR_OK;
}   /* end of rtk_port_phyAutoNegoAbility_get */

/* Function Name:
 *      rtk_port_phyAutoNegoAbility_set
 * Description:
 *      Set PHY auto negotiation ability of the specific port
 * Input:
 *      port     - port id
 *      pAbility - pointer to the PHY ability
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      You can set these abilities no matter which mode PHY currently stays on
 */
int32
rtk_port_phyAutoNegoAbility_set(
    rtk_port_t              port,
    rtk_port_phy_ability_t  *pAbility)
{
    rtdrv_portCfg_t port_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pAbility), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&port_cfg, 0x0, sizeof(rtdrv_portCfg_t));
    osal_memcpy(&port_cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&port_cfg.ability, pAbility, sizeof(rtk_port_phy_ability_t));
    SETSOCKOPT(RTDRV_PORT_PHYAUTONEGOABILITY_SET, &port_cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_port_phyAutoNegoAbility_set */

/* Function Name:
 *      rtk_port_phyForceModeAbility_get
 * Description:
 *      Get PHY ability status of the specific port
 * Input:
 *      port         - port id
 * Output:
 *      pSpeed       - pointer to the port speed
 *      pDuplex      - pointer to the port duplex
 *      pFlowControl - pointer to the flow control enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtk_port_phyForceModeAbility_get(
    rtk_port_t          port,
    rtk_port_speed_t    *pSpeed,
    rtk_port_duplex_t   *pDuplex,
    rtk_enable_t        *pFlowControl)
{
    rtdrv_portCfg_t port_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pSpeed), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pDuplex), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pFlowControl), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&port_cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_PORT_PHYFORCEMODEABILITY_GET, &port_cfg, rtdrv_portCfg_t, 1);
    osal_memcpy(pSpeed, &port_cfg.speed, sizeof(rtk_port_speed_t));
    osal_memcpy(pDuplex, &port_cfg.duplex, sizeof(rtk_port_duplex_t));
    osal_memcpy(pFlowControl, &port_cfg.flowControl, sizeof(rtk_enable_t));

    return RT_ERR_OK;
}   /* end of rtk_port_phyForceModeAbility_get */

/* Function Name:
 *      rtk_port_phyForceModeAbility_set
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
 *      RT_ERR_OK
 *      RT_ERR_FAILED
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
int32
rtk_port_phyForceModeAbility_set(
    rtk_port_t          port,
    rtk_port_speed_t    speed,
    rtk_port_duplex_t   duplex,
    rtk_enable_t        flowControl)
{
    rtdrv_portCfg_t port_cfg;

    /* function body */
    osal_memset(&port_cfg, 0x0, sizeof(rtdrv_portCfg_t));
    osal_memcpy(&port_cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&port_cfg.speed, &speed, sizeof(rtk_port_speed_t));
    osal_memcpy(&port_cfg.duplex, &duplex, sizeof(rtk_port_duplex_t));
    osal_memcpy(&port_cfg.flowControl, &flowControl, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_PORT_PHYFORCEMODEABILITY_SET, &port_cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_port_phyForceModeAbility_set */

/* Function Name:
 *      rtk_port_phyReg_get
 * Description:
 *      Get PHY register data of the specific port
 * Input:
 *      port  - port id
 *      page  - page id
 *      reg   - reg id
 * Output:
 *      pData - pointer to the PHY reg data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_PHY_PAGE_ID  - invalid page id
 *      RT_ERR_PHY_REG_ID   - invalid reg id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtk_port_phyReg_get(
    rtk_port_t          port,
    uint32              page,
    rtk_port_phy_reg_t  reg,
    uint32              *pData)
{
    rtdrv_portCfg_t port_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&port_cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&port_cfg.page, &page, sizeof(uint32));
    osal_memcpy(&port_cfg.reg, &reg, sizeof(rtk_port_phy_reg_t));
    GETSOCKOPT(RTDRV_PORT_PHYREG_GET, &port_cfg, rtdrv_portCfg_t, 1);
    osal_memcpy(pData, &port_cfg.data, sizeof(uint32));

    return RT_ERR_OK;
}   /* end of rtk_port_phyReg_get */

/* Function Name:
 *      rtk_port_phyReg_set
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
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID     - invalid port id
 *      RT_ERR_PHY_PAGE_ID - invalid page id
 *      RT_ERR_PHY_REG_ID  - invalid reg id
 * Note:
 *      None
 */
int32
rtk_port_phyReg_set(
    rtk_port_t          port,
    uint32              page,
    rtk_port_phy_reg_t  reg,
    uint32              data)
{
    rtdrv_portCfg_t port_cfg;

    /* function body */
    osal_memset(&port_cfg, 0x0, sizeof(rtdrv_portCfg_t));
    osal_memcpy(&port_cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&port_cfg.page, &page, sizeof(uint32));
    osal_memcpy(&port_cfg.reg, &reg, sizeof(rtk_port_phy_reg_t));
    osal_memcpy(&port_cfg.data, &data, sizeof(uint32));
    SETSOCKOPT(RTDRV_PORT_PHYREG_SET, &port_cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_port_phyReg_set */

/* Function Name:
 *      rtk_port_phyMasterSlave_get
 * Description:
 *      Get PHY configuration of master/slave mode of the specific port
 * Input:
 *      port                - port id
 * Output:
 *      pMasterSlave        - pointer to the PHY master slave configuration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID       - invalid port id
 *      RT_ERR_NULL_POINTER  - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtk_port_phyMasterSlave_get(
    rtk_port_t          port,
    rtk_port_masterSlave_t   *pMasterSlave)
{
    rtdrv_portCfg_t port_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pMasterSlave), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&port_cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_PORT_PHYMASTERSLAVE_GET, &port_cfg, rtdrv_portCfg_t, 1);
    osal_memcpy(pMasterSlave, &port_cfg.masterSlave, sizeof(rtk_port_masterSlave_t));

    return RT_ERR_OK;
}   /* end of rtk_port_phyMasterSlave_get */

/* Function Name:
 *      rtk_port_phyMasterSlave_set
 * Description:
 *      Set PHY configuration of master/slave mode of the specific port
 * Input:
 *      port                - port id
 *      masterSlave         - PHY master slave configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_INPUT        - RT_ERR_INPUT
 * Note:
 *      None
 */
int32
rtk_port_phyMasterSlave_set(
    rtk_port_t          port,
    rtk_port_masterSlave_t   masterSlave)
{
    rtdrv_portCfg_t port_cfg;

    /* function body */
    osal_memset(&port_cfg, 0x0, sizeof(rtdrv_portCfg_t));
    osal_memcpy(&port_cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&port_cfg.masterSlave, &masterSlave, sizeof(rtk_port_masterSlave_t));
    SETSOCKOPT(RTDRV_PORT_PHYMASTERSLAVE_SET, &port_cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_port_phyMasterSlave_set */

/* Function Name:
 *      rtk_port_phyTestMode_get
 * Description:
 *      Get PHY in which test mode.
 * Input:
 *      port - Port id.
 * Output:
 *      pTestMode - PHY test mode 0:normal 1:test mode 1 2:test mode 2 3: test mode 3 4:test mode 4 5~7:reserved
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_PORT_ID          - Invalid port number.
 *      RT_ERR_INPUT            - Invalid input parameters.
 *      RT_ERR_BUSYWAIT_TIMEOUT - PHY access busy
 * Note:
 *      Get test mode of PHY from register setting 9.15 to 9.13.
 */
int32
rtk_port_phyTestMode_get(rtk_port_t port, rtk_port_phy_test_mode_t *pTestMode)
{
    rtdrv_portCfg_t port_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pTestMode), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&port_cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_PORT_PHYTESTMODE_GET, &port_cfg, rtdrv_portCfg_t, 1);
    osal_memcpy(pTestMode, &port_cfg.testMode, sizeof(rtk_port_phy_test_mode_t));

    return RT_ERR_OK;
}   /* end of rtk_port_phyTestMode_get */

/* Function Name:
 *      rtk_port_phyTestMode_set
 * Description:
 *      Set PHY in test mode.
 * Input:
 *      port - port id.
 *      testMode - PHY test mode 0:normal 1:test mode 1 2:test mode 2 3: test mode 3 4:test mode 4 5~7:reserved
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_SMI              - SMI access error
 *      RT_ERR_PORT_ID          - Invalid port number.
 *      RT_ERR_BUSYWAIT_TIMEOUT - PHY access busy
 * Note:
 *      None.
 */
int32
rtk_port_phyTestMode_set(rtk_port_t port, rtk_port_phy_test_mode_t testMode)
{
    rtdrv_portCfg_t port_cfg;

    /* function body */
    osal_memset(&port_cfg, 0x0, sizeof(rtdrv_portCfg_t));
    osal_memcpy(&port_cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&port_cfg.testMode, &testMode, sizeof(rtk_port_phy_test_mode_t));
    SETSOCKOPT(RTDRV_PORT_PHYTESTMODE_SET, &port_cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_port_phyTestMode_set */

/* Function Name:
 *      rtk_port_cpuPortId_get
 * Description:
 *      Get CPU port id of the specific unit
 * Input:
 *      None
 * Output:
 *      pPort - pointer to CPU port id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtk_port_cpuPortId_get(rtk_port_t *pPort)
{
    rtdrv_portCfg_t port_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pPort), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&port_cfg.port, pPort, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_PORT_CPUPORTID_GET, &port_cfg, rtdrv_portCfg_t, 1);
    osal_memcpy(pPort, &port_cfg.port, sizeof(rtk_port_t));

    return RT_ERR_OK;
}   /* end of rtk_port_cpuPortId_get */

/* Function Name:
 *      rtk_port_isolation_get
 * Description:
 *      Get the portmask of the port isolation
 * Input:
 *      port      - port id
 * Output:
 *      pPortmask - pointer to the portmask
 *      pExtPortmask - pointer to extension portmask
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) Default value of each port is 1
 *      (2) Enable port isolation in the certain ports if relative portmask bits are set to 1
 */
int32
rtk_port_isolation_get(rtk_port_t port, rtk_portmask_t *pPortmask, rtk_portmask_t *pExtPortmask)
{
    rtdrv_portCfg_t port_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pExtPortmask), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&port_cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_PORT_ISOLATION_GET, &port_cfg, rtdrv_portCfg_t, 1);
    osal_memcpy(pPortmask, &port_cfg.portmask, sizeof(rtk_portmask_t));
    osal_memcpy(pExtPortmask, &port_cfg.extPortmask, sizeof(rtk_portmask_t));

    return RT_ERR_OK;
}   /* end of rtk_port_isolation_get */

/* Function Name:
 *      rtk_port_isolation_set
 * Description:
 *      Set the portmask of the port isolation
 * Input:
 *      port      - port id
 *      pPortmask - pointer to the portmask
 *      pExtPortmask - pointer to extension portmask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID   - invalid port id
 *      RT_ERR_PORT_MASK - invalid port mask
 * Note:
 *      (1) Default value of each port is 1
 *      (2) Enable port isolation in the certain ports if relative portmask bits are set to 1
 */
int32
rtk_port_isolation_set(rtk_port_t port, rtk_portmask_t *pPortmask, rtk_portmask_t *pExtPortmask)
{
    rtdrv_portCfg_t port_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pExtPortmask), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&port_cfg, 0x0, sizeof(rtdrv_portCfg_t));
    osal_memcpy(&port_cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&port_cfg.portmask, pPortmask, sizeof(rtk_portmask_t));
    osal_memcpy(&port_cfg.extPortmask, pExtPortmask, sizeof(rtk_portmask_t));
    SETSOCKOPT(RTDRV_PORT_ISOLATION_SET, &port_cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_port_isolation_set */

/* Function Name:
 *      rtk_port_isolationExt_get
 * Description:
 *      Get the portmask of 2nd port isolation configuration
 * Input:
 *      port      - port id
 * Output:
 *      pPortmask - pointer to the portmask
 *      pExtPortmask - pointer to extension portmask
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) Default value of each port is 1
 *      (2) Enable port isolation in the certain ports if relative portmask bits are set to 1
 */
int32
rtk_port_isolationExt_get(rtk_port_t port, rtk_portmask_t *pPortmask, rtk_portmask_t *pExtPortmask)
{
    rtdrv_portCfg_t port_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pExtPortmask), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&port_cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_PORT_ISOLATIONEXT_GET, &port_cfg, rtdrv_portCfg_t, 1);
    osal_memcpy(pPortmask, &port_cfg.portmask, sizeof(rtk_portmask_t));
    osal_memcpy(pExtPortmask, &port_cfg.extPortmask, sizeof(rtk_portmask_t));

    return RT_ERR_OK;
}   /* end of rtk_port_isolationExt_get */

/* Function Name:
 *      rtk_port_isolationExt_set
 * Description:
 *      Set the portmask of the 2nd port isolation configuration
 * Input:
 *      port      - port id
 *      portmask - pointer to the portmask
 *      pExtPortmask - pointer to extension portmask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID   - invalid port id
 *      RT_ERR_PORT_MASK - invalid port mask
 * Note:
 *      (1) Default value of each port is 1
 *      (2) Enable port isolation in the certain ports if relative portmask bits are set to 1
 */
int32
rtk_port_isolationExt_set(rtk_port_t port, rtk_portmask_t *pPortmask, rtk_portmask_t *pExtPortmask)
{
    rtdrv_portCfg_t port_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pExtPortmask), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&port_cfg, 0x0, sizeof(rtdrv_portCfg_t));
    osal_memcpy(&port_cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&port_cfg.portmask, pPortmask, sizeof(rtk_portmask_t));
    osal_memcpy(&port_cfg.extPortmask, pExtPortmask, sizeof(rtk_portmask_t));
    SETSOCKOPT(RTDRV_PORT_ISOLATIONEXT_SET, &port_cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_port_isolationExt_set */

/* Function Name:
 *      rtk_port_isolationL34_get
 * Description:
 *      Get the portmask of the port isolation
 * Input:
 *      port      - port id
 * Output:
 *      pPortmask - pointer to the portmask
 *      pExtPortmask - pointer to extension portmask
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) Default value of each port is 1
 *      (2) Enable port isolation in the certain ports if relative portmask bits are set to 1
 */
int32
rtk_port_isolationL34_get(rtk_port_t port, rtk_portmask_t *pPortmask, rtk_portmask_t *pExtPortmask)
{
    rtdrv_portCfg_t port_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pExtPortmask), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&port_cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_PORT_ISOLATIONL34_GET, &port_cfg, rtdrv_portCfg_t, 1);
    osal_memcpy(pPortmask, &port_cfg.portmask, sizeof(rtk_portmask_t));
    osal_memcpy(pExtPortmask, &port_cfg.extPortmask, sizeof(rtk_portmask_t));

    return RT_ERR_OK;
}   /* end of rtk_port_isolationL34_get */

/* Function Name:
 *      rtk_port_isolationL34_set
 * Description:
 *      Set the portmask of the port isolation
 * Input:
 *      port      - port id
 *      pPortmask - pointer to the portmask
 *      pExtPortmask - pointer to extension portmask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID   - invalid port id
 *      RT_ERR_PORT_MASK - invalid port mask
 * Note:
 *      (1) Default value of each port is 1
 *      (2) Enable port isolation in the certain ports if relative portmask bits are set to 1
 */
int32
rtk_port_isolationL34_set(rtk_port_t port, rtk_portmask_t *pPortmask, rtk_portmask_t *pExtPortmask)
{
    rtdrv_portCfg_t port_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pExtPortmask), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&port_cfg, 0x0, sizeof(rtdrv_portCfg_t));
    osal_memcpy(&port_cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&port_cfg.portmask, pPortmask, sizeof(rtk_portmask_t));
    osal_memcpy(&port_cfg.extPortmask, pExtPortmask, sizeof(rtk_portmask_t));
    SETSOCKOPT(RTDRV_PORT_ISOLATIONL34_SET, &port_cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_port_isolationL34_set */

/* Function Name:
 *      rtk_port_isolationExtL34_get
 * Description:
 *      Get the portmask of 2nd port isolation configuration
 * Input:
 *      port      - port id
 * Output:
 *      pPortmask - pointer to the portmask
 *      pExtPortmask - pointer to extension portmask
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) Default value of each port is 1
 *      (2) Enable port isolation in the certain ports if relative portmask bits are set to 1
 */
int32
rtk_port_isolationExtL34_get(rtk_port_t port, rtk_portmask_t *pPortmask, rtk_portmask_t *pExtPortmask)
{
    rtdrv_portCfg_t port_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pExtPortmask), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&port_cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_PORT_ISOLATIONEXTL34_GET, &port_cfg, rtdrv_portCfg_t, 1);
    osal_memcpy(pPortmask, &port_cfg.portmask, sizeof(rtk_portmask_t));
    osal_memcpy(pExtPortmask, &port_cfg.extPortmask, sizeof(rtk_portmask_t));

    return RT_ERR_OK;
}   /* end of rtk_port_isolationExtL34_get */

/* Function Name:
 *      rtk_port_isolationExtL34_set
 * Description:
 *      Set the portmask of the 2nd port isolation configuration
 * Input:
 *      port      - port id
 *      portmask - pointer to the portmask
 *      pExtPortmask - pointer to extension portmask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID   - invalid port id
 *      RT_ERR_PORT_MASK - invalid port mask
 * Note:
 *      (1) Default value of each port is 1
 *      (2) Enable port isolation in the certain ports if relative portmask bits are set to 1
 */
int32
rtk_port_isolationExtL34_set(rtk_port_t port, rtk_portmask_t *pPortmask, rtk_portmask_t *pExtPortmask)
{
    rtdrv_portCfg_t port_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pExtPortmask), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&port_cfg, 0x0, sizeof(rtdrv_portCfg_t));
    osal_memcpy(&port_cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&port_cfg.portmask, pPortmask, sizeof(rtk_portmask_t));
    osal_memcpy(&port_cfg.extPortmask, pExtPortmask, sizeof(rtk_portmask_t));
    SETSOCKOPT(RTDRV_PORT_ISOLATIONEXTL34_SET, &port_cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_port_isolationExtL34_set */

/* Function Name:
 *      rtk_port_isolationEntry_get
 * Description:
 *      Get Port isolation portmask
 * Input:
 *      mode            - Configuration 0 or 1
 *      port            - Ingress port
 * Output:
 *      pPortmask       - Isolation portmask for specified ingress port.
 *      pExtPortmask    - Isolation extension portmask for specified ingress port.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None.
 */
int32
rtk_port_isolationEntry_get(rtk_port_isoConfig_t mode, rtk_port_t port, rtk_portmask_t *pPortmask, rtk_portmask_t *pExtPortmask)
{
    rtdrv_portCfg_t port_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pExtPortmask), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&port_cfg.mode, &mode, sizeof(rtk_port_isoConfig_t));
    osal_memcpy(&port_cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_PORT_ISOLATIONENTRY_GET, &port_cfg, rtdrv_portCfg_t, 1);
    osal_memcpy(pPortmask, &port_cfg.portmask, sizeof(rtk_portmask_t));
    osal_memcpy(pExtPortmask, &port_cfg.extPortmask, sizeof(rtk_portmask_t));

    return RT_ERR_OK;
}   /* end of rtk_port_isolationEntry_get */

/* Function Name:
 *      rtk_port_isolationEntry_set
 * Description:
 *      Set Port isolation portmask
 * Input:
 *      mode            - Configuration 0 or 1
 *      port            - Ingress port
 *      pPortmask       - Isolation portmask for specified ingress port.
 *      pExtPortmask    - Isolation extension portmask for specified ingress port.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      pExtPortmask is the extension egress portmask toward CPU port.
 *      If users specify an empty extension portmask and CPU port is set in pPortmask,
 *      the packets will be restricted to be forwarded to CPU.
 *      Likewise, If users specify an non-empty extension portmask and CPU port is not set in pPortmask,
 *      the packets will be restricted to be forwarded to CPU. too.
 */
int32
rtk_port_isolationEntry_set(rtk_port_isoConfig_t mode, rtk_port_t port, rtk_portmask_t *pPortmask, rtk_portmask_t *pExtPortmask)
{
    rtdrv_portCfg_t port_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pExtPortmask), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&port_cfg, 0x0, sizeof(rtdrv_portCfg_t));
    osal_memcpy(&port_cfg.mode, &mode, sizeof(rtk_port_isoConfig_t));
    osal_memcpy(&port_cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&port_cfg.portmask, pPortmask, sizeof(rtk_portmask_t));
    osal_memcpy(&port_cfg.extPortmask, pExtPortmask, sizeof(rtk_portmask_t));
    SETSOCKOPT(RTDRV_PORT_ISOLATIONENTRY_SET, &port_cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_port_isolationEntry_set */

/* Function Name:
 *      rtk_port_isolationEntryExt_get
 * Description:
 *      Get Port isolation portmask
 * Input:
 *      mode            - Configuration 0 or 1
 *      port            - Ingress port
 * Output:
 *      pPortmask       - Isolation portmask for specified ingress port.
 *      pExtPortmask    - Isolation extension portmask for specified ingress port.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None.
 */
int32
rtk_port_isolationEntryExt_get(rtk_port_isoConfig_t mode, rtk_port_t port, rtk_portmask_t *pPortmask, rtk_portmask_t *pExtPortmask)
{
    rtdrv_portCfg_t port_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pExtPortmask), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&port_cfg.mode, &mode, sizeof(rtk_port_isoConfig_t));
    osal_memcpy(&port_cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_PORT_ISOLATIONENTRYEXT_GET, &port_cfg, rtdrv_portCfg_t, 1);
    osal_memcpy(pPortmask, &port_cfg.portmask, sizeof(rtk_portmask_t));
    osal_memcpy(pExtPortmask, &port_cfg.extPortmask, sizeof(rtk_portmask_t));

    return RT_ERR_OK;
}   /* end of rtk_port_isolationEntryExt_get */

/* Function Name:
 *      rtk_port_isolationEntryExt_set
 * Description:
 *      Set Port isolation portmask
 * Input:
 *      mode            - Configuration 0 or 1
 *      port            - Ingress port
 *      pPortmask       - Isolation portmask for specified ingress port.
 *      pExtPortmask    - Isolation extension portmask for specified ingress port.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      pExtPortmask is the extension egress portmask toward CPU port.
 *      If users specify an empty extension portmask and CPU port is set in pPortmask,
 *      the packets will be restricted to be forwarded to CPU.
 *      Likewise, If users specify an non-empty extension portmask and CPU port is not set in pPortmask,
 *      the packets will be restricted to be forwarded to CPU. too.
 */
int32
rtk_port_isolationEntryExt_set(rtk_port_isoConfig_t mode, rtk_port_t port, rtk_portmask_t *pPortmask, rtk_portmask_t *pExtPortmask)
{
    rtdrv_portCfg_t port_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pExtPortmask), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&port_cfg, 0x0, sizeof(rtdrv_portCfg_t));
    osal_memcpy(&port_cfg.mode, &mode, sizeof(rtk_port_isoConfig_t));
    osal_memcpy(&port_cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&port_cfg.portmask, pPortmask, sizeof(rtk_portmask_t));
    osal_memcpy(&port_cfg.extPortmask, pExtPortmask, sizeof(rtk_portmask_t));
    SETSOCKOPT(RTDRV_PORT_ISOLATIONENTRYEXT_SET, &port_cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_port_isolationEntryExt_set */

/* Function Name:
 *      rtk_port_isolationCtagPktConfig_get
 * Description:
 *      Isolation configuration selection for ingress Ctag packets
 * Input:
 *      None
 * Output:
 *      pMode   - Isolation configuration selection
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None.
 */
int32
rtk_port_isolationCtagPktConfig_get(rtk_port_isoConfig_t *pMode)
{
    rtdrv_portCfg_t port_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pMode), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&port_cfg.mode, pMode, sizeof(rtk_port_isoConfig_t));
    GETSOCKOPT(RTDRV_PORT_ISOLATIONCTAGPKTCONFIG_GET, &port_cfg, rtdrv_portCfg_t, 1);
    osal_memcpy(pMode, &port_cfg.mode, sizeof(rtk_port_isoConfig_t));

    return RT_ERR_OK;
}   /* end of rtk_port_isolationCtagPktConfig_get */

/* Function Name:
 *      rtk_port_isolationCtagPktConfig_set
 * Description:
 *      Isolation configuration selection for ingress Ctag packets
 * Input:
 *      mode    - Isolation configuration selection
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
int32
rtk_port_isolationCtagPktConfig_set(rtk_port_isoConfig_t mode)
{
    rtdrv_portCfg_t port_cfg;

    /* function body */
    osal_memset(&port_cfg, 0x0, sizeof(rtdrv_portCfg_t));
    osal_memcpy(&port_cfg.mode, &mode, sizeof(rtk_port_isoConfig_t));
    SETSOCKOPT(RTDRV_PORT_ISOLATIONCTAGPKTCONFIG_SET, &port_cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_port_isolationCtagPktConfig_set */

/* Function Name:
 *      rtk_port_isolationL34PktConfig_get
 * Description:
 *      Isolation configuration selection for ingress Ctag packets
 * Input:
 *      None
 * Output:
 *      pMode   - Isolation configuration selection
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None.
 */
int32
rtk_port_isolationL34PktConfig_get(rtk_port_isoConfig_t *pMode)
{
    rtdrv_portCfg_t port_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pMode), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&port_cfg.mode, pMode, sizeof(rtk_port_isoConfig_t));
    GETSOCKOPT(RTDRV_PORT_ISOLATIONL34PKTCONFIG_GET, &port_cfg, rtdrv_portCfg_t, 1);
    osal_memcpy(pMode, &port_cfg.mode, sizeof(rtk_port_isoConfig_t));

    return RT_ERR_OK;
}   /* end of rtk_port_isolationL34PktConfig_get */

/* Function Name:
 *      rtk_port_isolationL34PktConfig_set
 * Description:
 *      Isolation configuration selection for ingress Ctag packets
 * Input:
 *      mode    - Isolation configuration selection
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
int32
rtk_port_isolationL34PktConfig_set(rtk_port_isoConfig_t mode)
{
    rtdrv_portCfg_t port_cfg;

    /* function body */
    osal_memset(&port_cfg, 0x0, sizeof(rtdrv_portCfg_t));
    osal_memcpy(&port_cfg.mode, &mode, sizeof(rtk_port_isoConfig_t));
    SETSOCKOPT(RTDRV_PORT_ISOLATIONL34PKTCONFIG_SET, &port_cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_port_isolationL34PktConfig_set */

/* Function Name:
 *      rtk_port_isolationIpmcLeaky_get
 * Description:
 *      Get the ip multicast leaky state of the port isolation
 * Input:
 *      port      - port id
 * Output:
 *      pEnable   - status of port isolation leaky for ip multicast packets
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      none
 */
int32
rtk_port_isolationIpmcLeaky_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    rtdrv_portCfg_t port_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&port_cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_PORT_ISOLATIONIPMCLEAKY_GET, &port_cfg, rtdrv_portCfg_t, 1);
    osal_memcpy(pEnable, &port_cfg.enable, sizeof(rtk_enable_t));

    return RT_ERR_OK;
}   /* end of rtk_port_isolationIpmcLeaky_get */

/* Function Name:
 *      rtk_port_isolationIpmcLeaky_set
 * Description:
 *      Set the ip multicast leaky state of the port isolation
 * Input:
 *      port      - port id
 *      enable    - status of port isolation leaky for ip multicast packets
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      none
 */
int32
rtk_port_isolationIpmcLeaky_set(rtk_port_t port, rtk_enable_t enable)
{
    rtdrv_portCfg_t port_cfg;

    /* function body */
    osal_memset(&port_cfg, 0x0, sizeof(rtdrv_portCfg_t));
    osal_memcpy(&port_cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&port_cfg.enable, &enable, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_PORT_ISOLATIONIPMCLEAKY_SET, &port_cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_port_isolationIpmcLeaky_set */

/* Function Name:
 *      rtk_port_isolationPortLeaky_get
 * Description:
 *      Get the per port isolation leaky state for given type
 * Input:
 *      port      - port id
 *      type      - Packet type for isolation leaky.
 * Output:
 *      pEnable   - status of port isolation leaky for given leaky type
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      none
 */
int32
rtk_port_isolationPortLeaky_get(rtk_port_t port, rtk_leaky_type_t type, rtk_enable_t *pEnable)
{
    rtdrv_portCfg_t port_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&port_cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&port_cfg.type, &type, sizeof(rtk_leaky_type_t));
    GETSOCKOPT(RTDRV_PORT_ISOLATIONPORTLEAKY_GET, &port_cfg, rtdrv_portCfg_t, 1);
    osal_memcpy(pEnable, &port_cfg.enable, sizeof(rtk_enable_t));

    return RT_ERR_OK;
}   /* end of rtk_port_isolationPortLeaky_get */

/* Function Name:
 *      rtk_port_isolationPortLeaky_set
 * Description:
 *      Set the per port isolation leaky state for given type
 * Input:
 *      port      - port id
 *      type      - Packet type for isolation leaky.
 *      enable    - status of port isolation leaky for given leaky type
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      none
 */
int32
rtk_port_isolationPortLeaky_set(rtk_port_t port, rtk_leaky_type_t type, rtk_enable_t enable)
{
    rtdrv_portCfg_t port_cfg;

    /* function body */
    osal_memset(&port_cfg, 0x0, sizeof(rtdrv_portCfg_t));
    osal_memcpy(&port_cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&port_cfg.type, &type, sizeof(rtk_leaky_type_t));
    osal_memcpy(&port_cfg.enable, &enable, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_PORT_ISOLATIONPORTLEAKY_SET, &port_cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_port_isolationPortLeaky_set */

/* Function Name:
 *      rtk_port_isolationLeaky_get
 * Description:
 *      Get the per port isolation leaky state for given type
 * Input:
 *      type      - Packet type for isolation leaky.
 * Output:
 *      pEnable   - status of port isolation leaky for given leaky type
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      none
 */
int32
rtk_port_isolationLeaky_get(rtk_leaky_type_t type, rtk_enable_t *pEnable)
{
    rtdrv_portCfg_t port_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&port_cfg.type, &type, sizeof(rtk_leaky_type_t));
    GETSOCKOPT(RTDRV_PORT_ISOLATIONLEAKY_GET, &port_cfg, rtdrv_portCfg_t, 1);
    osal_memcpy(pEnable, &port_cfg.enable, sizeof(rtk_enable_t));

    return RT_ERR_OK;
}   /* end of rtk_port_isolationLeaky_get */

/* Function Name:
 *      rtk_port_isolationLeaky_set
 * Description:
 *      Set the per port isolation leaky state for given type
 * Input:
 *      type      - Packet type for isolation leaky.
 *      enable    - status of port isolation leaky for given leaky type
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      none
 */
int32
rtk_port_isolationLeaky_set(rtk_leaky_type_t type, rtk_enable_t enable)
{
    rtdrv_portCfg_t port_cfg;

    /* function body */
    osal_memset(&port_cfg, 0x0, sizeof(rtdrv_portCfg_t));
    osal_memcpy(&port_cfg.type, &type, sizeof(rtk_leaky_type_t));
    osal_memcpy(&port_cfg.enable, &enable, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_PORT_ISOLATIONLEAKY_SET, &port_cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_port_isolationLeaky_set */


/* Function Name:
 *      rtk_port_macRemoteLoopbackEnable_get
 * Description:
 *      Get the mac remote loopback enable status of the specific port
 * Input:
 *      port    - port id
 * Output:
 *      pEnable - pointer to the enable status of mac remote loopback
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      (1) The mac remote loopback enable status of the port is as following:
 *          - DISABLE
 *          - ENABLE
 *      (2) Remote loopback is used to loopback packet RX to switch core back to the outer interface.
 */
int32
rtk_port_macRemoteLoopbackEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    rtdrv_portCfg_t port_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&port_cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_PORT_MACREMOTELOOPBACKENABLE_GET, &port_cfg, rtdrv_portCfg_t, 1);
    osal_memcpy(pEnable, &port_cfg.enable, sizeof(rtk_enable_t));

    return RT_ERR_OK;
}   /* end of rtk_port_macRemoteLoopbackEnable_get */

/* Function Name:
 *      rtk_port_macRemoteLoopbackEnable_set
 * Description:
 *      Set the mac remote loopback enable status of the specific port
 * Input:
 *      port   - port id
 *      enable - enable status of mac remote loopback
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      (1) The mac remote loopback enable status of the port is as following:
 *          - DISABLE
 *          - ENABLE
 *      (2) Remote loopback is used to loopback packet RX to switch core back to the outer interface.
 */
int32
rtk_port_macRemoteLoopbackEnable_set(rtk_port_t port, rtk_enable_t enable)
{
    rtdrv_portCfg_t port_cfg;

    /* function body */
    osal_memset(&port_cfg, 0x0, sizeof(rtdrv_portCfg_t));
    osal_memcpy(&port_cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&port_cfg.enable, &enable, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_PORT_MACREMOTELOOPBACKENABLE_SET, &port_cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_port_macRemoteLoopbackEnable_set */

/* Function Name:
 *      rtk_port_macLocalLoopbackEnable_get
 * Description:
 *      Get the mac local loopback enable status of the specific port
 * Input:
 *      port    - port id
 * Output:
 *      pEnable - pointer to the enable status of mac local loopback
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      (1) The mac local loopback enable status of the port is as following:
 *          - DISABLE
 *          - ENABLE
 *      (2) Local loopback is used to loopback packet TX from switch core back to switch core.
 */
int32
rtk_port_macLocalLoopbackEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    rtdrv_portCfg_t port_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&port_cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_PORT_MACLOCALLOOPBACKENABLE_GET, &port_cfg, rtdrv_portCfg_t, 1);
    osal_memcpy(pEnable, &port_cfg.enable, sizeof(rtk_enable_t));

    return RT_ERR_OK;
}   /* end of rtk_port_macLocalLoopbackEnable_get */

/* Function Name:
 *      rtk_port_macLocalLoopbackEnable_set
 * Description:
 *      Set the mac local loopback enable status of the specific port
 * Input:
 *      port   - port id
 *      enable - enable status of mac local loopback
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      (1) The mac local loopback enable status of the port is as following:
 *          - DISABLE
 *          - ENABLE
 *      (2) Local loopback is used to loopback packet TX from switch core back to switch core.
 */
int32
rtk_port_macLocalLoopbackEnable_set(rtk_port_t port, rtk_enable_t enable)
{
    rtdrv_portCfg_t port_cfg;

    /* function body */
    osal_memset(&port_cfg, 0x0, sizeof(rtdrv_portCfg_t));
    osal_memcpy(&port_cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&port_cfg.enable, &enable, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_PORT_MACLOCALLOOPBACKENABLE_SET, &port_cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_port_macLocalLoopbackEnable_set */

/* Function Name:
 *      rtk_port_adminEnable_get
 * Description:
 *      Get port admin status of the specific port
 * Input:
 *      port    - port id
 * Output:
 *      pEnable - pointer to the port admin status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtk_port_adminEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    rtdrv_portCfg_t port_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&port_cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_PORT_ADMINENABLE_GET, &port_cfg, rtdrv_portCfg_t, 1);
    osal_memcpy(pEnable, &port_cfg.enable, sizeof(rtk_enable_t));

    return RT_ERR_OK;
}   /* end of rtk_port_adminEnable_get */

/* Function Name:
 *      rtk_port_adminEnable_set
 * Description:
 *      Set port admin status of the specific port
 * Input:
 *      port    - port id
 *      enable  - port admin status
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
rtk_port_adminEnable_set(rtk_port_t port, rtk_enable_t enable)
{
    rtdrv_portCfg_t port_cfg;

    /* function body */
    osal_memset(&port_cfg, 0x0, sizeof(rtdrv_portCfg_t));
    osal_memcpy(&port_cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&port_cfg.enable, &enable, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_PORT_ADMINENABLE_SET, &port_cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_port_adminEnable_set */

/* Function Name:
 *      rtk_port_specialCongest_get
 * Description:
 *      Set the congest seconds of the specific port
 * Input:
 *      port    - port id
 * Output:
 *      pSecond - congest timer (seconds)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 *      RT_ERR_INPUT   - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtk_port_specialCongest_get(rtk_port_t port, uint32 *pSecond)
{
    rtdrv_portCfg_t port_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pSecond), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&port_cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_PORT_SPECIALCONGEST_GET, &port_cfg, rtdrv_portCfg_t, 1);
    osal_memcpy(pSecond, &port_cfg.second, sizeof(uint32));

    return RT_ERR_OK;
}   /* end of rtk_port_specialCongest_get */

/* Function Name:
 *      rtk_port_specialCongest_set
 * Description:
 *      Set the congest seconds of the specific port
 * Input:
 *      port   - port id
 *      second - congest timer (seconds)
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 *      RT_ERR_INPUT   - invalid input parameter
 * Note:
 *      None
 */
int32
rtk_port_specialCongest_set(rtk_port_t port, uint32 second)
{
    rtdrv_portCfg_t port_cfg;

    /* function body */
    osal_memset(&port_cfg, 0x0, sizeof(rtdrv_portCfg_t));
    osal_memcpy(&port_cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&port_cfg.second, &second, sizeof(uint32));
    SETSOCKOPT(RTDRV_PORT_SPECIALCONGEST_SET, &port_cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_port_specialCongest_set */

/* Function Name:
 *      rtk_port_specialCongestStatus_get
 * Description:
 *      Get the congest status of the specific port
 * Input:
 *      port   - port id
 * Output:
 *      pStatus - Congest status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 *      RT_ERR_INPUT   - invalid input parameter
 * Note:
 *      None
 */
int32
rtk_port_specialCongestStatus_get(rtk_port_t port, uint32 *pStatus)
{
    rtdrv_portCfg_t port_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pStatus), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&port_cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_PORT_SPECIALCONGESTSTATUS_GET, &port_cfg, rtdrv_portCfg_t, 1);
    osal_memcpy(pStatus, &port_cfg.status, sizeof(uint32));

    return RT_ERR_OK;
}   /* end of rtk_port_specialCongestStatus_get */

/* Function Name:
 *      rtk_port_specialCongestStatus_get
 * Description:
 *      Get the congest status of the specific port
 * Input:
 *      port   - port id
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 *      RT_ERR_INPUT   - invalid input parameter
 * Note:
 *      None
 */
int32
rtk_port_specialCongestStatus_clear(rtk_port_t port)
{
    rtdrv_portCfg_t port_cfg;

    /* function body */
    osal_memset(&port_cfg, 0x0, sizeof(rtdrv_portCfg_t));
    osal_memcpy(&port_cfg.port, &port, sizeof(rtk_port_t));
    SETSOCKOPT(RTDRV_PORT_SPECIALCONGESTSTATUS_CLEAR, &port_cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_port_specialCongestStatus_clear */

/* Function Name:
 *      rtk_port_greenEnable_get
 * Description:
 *      Get the statue of green feature of the specific port in the specific unit
 * Input:
 *      port    - port id
 * Output:
 *      pEnable - pointer to status of green feature
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtk_port_greenEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    rtdrv_portCfg_t port_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&port_cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_PORT_GREENENABLE_GET, &port_cfg, rtdrv_portCfg_t, 1);
    osal_memcpy(pEnable, &port_cfg.enable, sizeof(rtk_enable_t));

    return RT_ERR_OK;
}   /* end of rtk_port_greenEnable_get */

/* Function Name:
 *      rtk_port_greenEnable_set
 * Description:
 *      Set the statue of green feature of the specific port in the specific unit
 * Input:
 *      port   - port id
 *      enable - status of green feature
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
rtk_port_greenEnable_set(rtk_port_t port, rtk_enable_t enable)
{
    rtdrv_portCfg_t port_cfg;

    /* function body */
    osal_memset(&port_cfg, 0x0, sizeof(rtdrv_portCfg_t));
    osal_memcpy(&port_cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&port_cfg.enable, &enable, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_PORT_GREENENABLE_SET, &port_cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_port_greenEnable_set */


/* Function Name:
 *      rtk_port_phyCrossOverMode_get
 * Description:
 *      Get cross over mode in the specified port.
 * Input:
 *      port  - port id
 * Output:
 *      pCrossoverMode - pointer to cross over mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Following value is valid
 *      - PORT_CROSSOVER_MODE_AUTO
 *      - PORT_CROSSOVER_MODE_MDI
 *      - PORT_CROSSOVER_MODE_MDIX
 */
int32
rtk_port_phyCrossOverMode_get(rtk_port_t port, rtk_port_crossOver_mode_t *pCrossoverMode)
{
    rtdrv_portCfg_t port_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pCrossoverMode), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&port_cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_PORT_PHYCROSSOVERMODE_GET, &port_cfg, rtdrv_portCfg_t, 1);
    osal_memcpy(pCrossoverMode, &port_cfg.crossoverMode, sizeof(rtk_port_crossOver_mode_t));

    return RT_ERR_OK;
}   /* end of rtk_port_phyCrossOverMode_get */

/* Function Name:
 *      rtk_port_phyCrossOverMode_set
 * Description:
 *      Set cross over mode in the specified port.
 * Input:
 *      port - port id
 *      crossoverMode - cross over mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 *      RT_ERR_INPUT   - invalid input parameter
 * Note:
 *      Following value is valid
 *      - PORT_CROSSOVER_MODE_AUTO
 *      - PORT_CROSSOVER_MODE_MDI
 *      - PORT_CROSSOVER_MODE_MDIX
 */
int32
rtk_port_phyCrossOverMode_set(rtk_port_t port, rtk_port_crossOver_mode_t crossoverMode)
{
    rtdrv_portCfg_t port_cfg;

    /* function body */
    osal_memset(&port_cfg, 0x0, sizeof(rtdrv_portCfg_t));
    osal_memcpy(&port_cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&port_cfg.crossoverMode, &crossoverMode, sizeof(rtk_port_crossOver_mode_t));
    SETSOCKOPT(RTDRV_PORT_PHYCROSSOVERMODE_SET, &port_cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_port_phyCrossOverMode_set */

/* Function Name:
 *      rtk_port_phyPowerDown_get
 * Description:
 *      Get PHY power down state of the specified port.
 * Input:
 *      port    - port id
 * Output:
 *      pEnable - pointer to power down enable state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      none
 */
int32
rtk_port_phyPowerDown_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    rtdrv_portCfg_t port_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&port_cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_PORT_PHYPOWERDOWN_GET, &port_cfg, rtdrv_portCfg_t, 1);
    osal_memcpy(pEnable, &port_cfg.enable, sizeof(rtk_enable_t));

    return RT_ERR_OK;
} /* end of rtk_port_phyPowerDown_get */

/* Function Name:
 *      rtk_port_phyPowerDown_set
 * Description:
 *      Set PHY power down state of the specified port.
 * Input:
 *      port   - port id
 *      enable - PHY power down enable state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 *      RT_ERR_INPUT   - invalid input parameter
 * Note:
 *      none
 */
int32
rtk_port_phyPowerDown_set(rtk_port_t port, rtk_enable_t enable)
{
    rtdrv_portCfg_t port_cfg;

    /* function body */
    osal_memset(&port_cfg, 0x0, sizeof(rtdrv_portCfg_t));
    osal_memcpy(&port_cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&port_cfg.enable, &enable, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_PORT_PHYPOWERDOWN_SET, &port_cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
} /* end of rtk_port_phyPowerDown_set */

/* Function Name:
 *      rtk_port_phyPowerDown_tryset
 * Description:
 *      Try to set PHY power down state of the specified port.
 * Input:
 *      port   - port id
 *      enable - PHY power down enable state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 *      RT_ERR_INPUT   - invalid input parameter
 * Note:
 *      This API will try to enable/disable specified port's phy power
 * It might failed due to admin not allow enable/disable.
 * The admin allow enable/disable only after
 * rtk_port_phyPowerDown_set being called since boot
 * This API is used for CONFIG_SWITCH_INIT_LINKDOWN config
 * Without CONFIG_SWITCH_INIT_LINKDOWN, it works exectly the same as
 * rtk_port_phyPowerDown_set
 */
int32
rtk_port_phyPowerDown_tryset(rtk_port_t port, rtk_enable_t enable)
{
    rtdrv_portCfg_t port_cfg;

    /* function body */
    osal_memset(&port_cfg, 0x0, sizeof(rtdrv_portCfg_t));
    osal_memcpy(&port_cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&port_cfg.enable, &enable, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_PORT_PHYPOWERDOWN_TRYSET, &port_cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
} /* end of rtk_port_phyPowerDown_tryset */

/* Function Name:
 *      rtk_port_enhancedFid_get
 * Description:
 *      Get port EFID
 * Input:
 *      port      - port id
 * Output:
 *      pEfid     - EFID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      none
 */
int32
rtk_port_enhancedFid_get(rtk_port_t port, rtk_efid_t *pEfid)
{
    rtdrv_portCfg_t port_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pEfid), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&port_cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_PORT_ENHANCEDFID_GET, &port_cfg, rtdrv_portCfg_t, 1);
    osal_memcpy(pEfid, &port_cfg.efid, sizeof(rtk_efid_t));

    return RT_ERR_OK;
}   /* end of rtk_port_enhancedFid_get */

/* Function Name:
 *      rtk_port_enhancedFid_set
 * Description:
 *      Set port EFID
 * Input:
 *      port      - port id
 *      efid      - EFID
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      none
 */
int32
rtk_port_enhancedFid_set(rtk_port_t port, rtk_efid_t efid)
{
    rtdrv_portCfg_t port_cfg;

    /* function body */
    osal_memset(&port_cfg, 0x0, sizeof(rtdrv_portCfg_t));
    osal_memcpy(&port_cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&port_cfg.efid, &efid, sizeof(rtk_efid_t));
    SETSOCKOPT(RTDRV_PORT_ENHANCEDFID_SET, &port_cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_port_enhancedFid_set */

/* Function Name:
 *      rtk_port_rtctResult_get
 * Description:
 *      Get test result of RTCT.
 * Input:
 *      port      - port id
 * Output:
 *      pResult   - Test Result
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      If linkType is PORT_SPEED_1000M, test result will be stored in ge_result.
 *      If linkType is PORT_SPEED_10M or PORT_SPEED_100M, test result will be stored in fe_result.
 */
int32
rtk_port_rtctResult_get(rtk_port_t port, rtk_rtctResult_t *pResult)
{
    rtdrv_portCfg_t port_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pResult), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&port_cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_PORT_RTCTRESULT_GET, &port_cfg, rtdrv_portCfg_t, 1);
    osal_memcpy(pResult, &port_cfg.result, sizeof(rtk_rtctResult_t));

    return RT_ERR_OK;
}   /* end of rtk_port_rtctResult_get */

/* Function Name:
 *      rtk_port_rtct_start
 * Description:
 *      Start RTCT for ports.
 *      When enable RTCT, the port won't transmit and receive normal traffic.
 * Input:
 *      pPortmask   - the ports for RTCT test
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      none
 */
int32
rtk_port_rtct_start(rtk_portmask_t *pPortmask)
{
    rtdrv_portCfg_t port_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortmask), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&port_cfg, 0x0, sizeof(rtdrv_portCfg_t));
    osal_memcpy(&port_cfg.portmask, pPortmask, sizeof(rtk_portmask_t));
    GETSOCKOPT(RTDRV_PORT_RTCT_START, &port_cfg, rtdrv_portCfg_t, 1);
    osal_memcpy(pPortmask, &port_cfg.portmask, sizeof(rtk_portmask_t));

    return RT_ERR_OK;
}   /* end of rtk_port_rtct_start */



/* Function Name:
 *      rtk_port_macForceAbility_set
 * Description:
 *      Set port mac force ability
 * Input:
 *      port      - port id
 *      ability      - ability
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      none
 */
int32
rtk_port_macForceAbility_set(rtk_port_t port, rtk_port_macAbility_t ability)
{
    rtdrv_portCfg_t port_cfg;

    /* function body */
    osal_memset(&port_cfg, 0x0, sizeof(rtdrv_portCfg_t));
    osal_memcpy(&port_cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&port_cfg.macAbility, &ability, sizeof(rtk_port_macAbility_t));
    SETSOCKOPT(RTDRV_PORT_MACFORCEABILITY_SET, &port_cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_port_macForceAbility_set */

/* Function Name:
 *      rtk_port_macForceAbility_get
 * Description:
 *      Get mac force mac ability
 * Input:
 *      port      - port id
 *
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      If linkType is PORT_SPEED_1000M, test result will be stored in ge_result.
 *      If linkType is PORT_SPEED_10M or PORT_SPEED_100M, test result will be stored in fe_result.
 */
int32
rtk_port_macForceAbility_get(rtk_port_t port, rtk_port_macAbility_t *pAbility)
{
    rtdrv_portCfg_t port_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pAbility), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&port_cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_PORT_MACFORCEABILITY_GET, &port_cfg, rtdrv_portCfg_t, 1);
    osal_memcpy(pAbility, &port_cfg.macAbility, sizeof(rtk_port_macAbility_t));

    return RT_ERR_OK;
}   /* end of rtk_port_macForceAbility_get */



/* Function Name:
 *      rtk_port_macForceAbilityState_set
 * Description:
 *      Set port mac force ability state
 * Input:
 *      port      - port id
 *      state      - state
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      none
 */
int32
rtk_port_macForceAbilityState_set(rtk_port_t port, rtk_enable_t state)
{
    rtdrv_portCfg_t port_cfg;

    /* function body */
    osal_memset(&port_cfg, 0x0, sizeof(rtdrv_portCfg_t));
    osal_memcpy(&port_cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&port_cfg.enable, &state, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_PORT_MACFORCEABILITYSTATE_SET, &port_cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_port_macForceAbilityState_set */

/* Function Name:
 *      rtk_port_macForceAbilityState_get
 * Description:
 *      Get mac force mac ability state
 * Input:
 *      port      - port id
 *      pState    - state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      If linkType is PORT_SPEED_1000M, test result will be stored in ge_result.
 *      If linkType is PORT_SPEED_10M or PORT_SPEED_100M, test result will be stored in fe_result.
 */
int32
rtk_port_macForceAbilityState_get(rtk_port_t port, rtk_enable_t *pState)
{
    rtdrv_portCfg_t port_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&port_cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_PORT_MACFORCEABILITYSTATE_GET, &port_cfg, rtdrv_portCfg_t, 1);
    osal_memcpy(pState, &port_cfg.enable, sizeof(rtk_enable_t));

    return RT_ERR_OK;
}   /* end of rtk_port_macForceAbilityState_get */

/* Function Name:
 *      rtk_port_macExtMode_set
 * Description:
 *      Set extension MAC mode
 * Input:
 *      port - the ports number
 *      ext_mode - the mode setting
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 * Note:
 *      None
 */
int32
rtk_port_macExtMode_set(rtk_port_t port, rtk_port_ext_mode_t ext_mode)
{
    rtdrv_portCfg_t port_cfg;

    /* function body */
    osal_memset(&port_cfg, 0x0, sizeof(rtdrv_portCfg_t));
    osal_memcpy(&port_cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&port_cfg.ext_mode, &ext_mode, sizeof(rtk_port_ext_mode_t));
    SETSOCKOPT(RTDRV_PORT_MACEXTMODE_SET, &port_cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_port_macExtMode_set */

/* Function Name:
 *      rtk_port_macExtMode_get
 * Description:
 *      Get extension MAC mode
 * Input:
 *      port - the ports number
 * Output:
 *      pExt_mode - the mode setting
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtk_port_macExtMode_get(rtk_port_t port, rtk_port_ext_mode_t *pExt_mode)
{
    rtdrv_portCfg_t port_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pExt_mode), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&port_cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_PORT_MACEXTMODE_GET, &port_cfg, rtdrv_portCfg_t, 1);
    osal_memcpy(pExt_mode, &port_cfg.ext_mode, sizeof(rtk_port_ext_mode_t));

    return RT_ERR_OK;
}   /* end of rtk_port_macExtMode_get */

/* Function Name:
 *      rtk_port_macExtRgmiiDelay_set
 * Description:
 *      Set RGMII TX/RX delay
 * Input:
 *      port - the ports number
 *      txDelay - the TX delay (0 ~ 1)
 *      rxDelay - the RX delay (0 ~ 7)
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 * Note:
 *      None
 */
int32
rtk_port_macExtRgmiiDelay_set(rtk_port_t port, uint32 txDelay, uint32 rxDelay)
{
    rtdrv_portCfg_t port_cfg;

    /* function body */
    osal_memset(&port_cfg, 0x0, sizeof(rtdrv_portCfg_t));
    osal_memcpy(&port_cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&port_cfg.txDelay, &txDelay, sizeof(uint32));
    osal_memcpy(&port_cfg.rxDelay, &rxDelay, sizeof(uint32));
    SETSOCKOPT(RTDRV_PORT_MACEXTRGMIIDELAY_SET, &port_cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_port_macExtRgmiiDelay_set */

/* Function Name:
 *      rtk_port_macExtRgmiiDelay_get
 * Description:
 *      Get RGMII TX/RX delay
 * Input:
 *      port - the ports number
 * Output:
 *      pTxDelay - the TX delay (0 ~ 1)
 *      pRxDelay - the RX delay (0 ~ 7)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtk_port_macExtRgmiiDelay_get(rtk_port_t port, uint32 *pTxDelay, uint32 *pRxDelay)
{
    rtdrv_portCfg_t port_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pTxDelay), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pRxDelay), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&port_cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_PORT_MACEXTRGMIIDELAY_GET, &port_cfg, rtdrv_portCfg_t, 1);
    osal_memcpy(pTxDelay, &port_cfg.txDelay, sizeof(uint32));
    osal_memcpy(pRxDelay, &port_cfg.rxDelay, sizeof(uint32));

    return RT_ERR_OK;
}   /* end of rtk_port_macExtRgmiiDelay_get */

/* Function Name:
 *      rtk_port_gigaLiteEnable_set
 * Description:
 *      set giga-lite enable state 
 * Input:
 *      port   - port id
 *      enable - enable status of giga-lite
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 * Note:
 *      None
 */
int32
rtk_port_gigaLiteEnable_set(rtk_port_t port, rtk_enable_t enable)
{
    rtdrv_portCfg_t port_cfg;

    /* function body */
    osal_memset(&port_cfg, 0x0, sizeof(rtdrv_portCfg_t));
    osal_memcpy(&port_cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&port_cfg.enable, &enable, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_PORT_GIGALITEENABLE_SET, &port_cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_port_gigaLiteEnable_set */

/* Function Name:
 *      rtk_port_gigaLiteEnable_get
 * Description:
 *      Get RGMII TX/RX delay
 * Input:
 *      port   - port id
 * Output:
 *      pEnable - enable status of giga-lite
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtk_port_gigaLiteEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    rtdrv_portCfg_t port_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&port_cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_PORT_GIGALITEENABLE_GET, &port_cfg, rtdrv_portCfg_t, 1);
    osal_memcpy(pEnable, &port_cfg.enable, sizeof(rtk_enable_t));

    return RT_ERR_OK;
}   /* end of rtk_port_gigaLiteEnable_get */

/* Function Name:
 *      rtk_port_eeeEnable_set
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
int32
rtk_port_eeeEnable_set(rtk_port_t port, rtk_enable_t enable)
{
    rtdrv_portCfg_t port_cfg;

    /* function body */
    osal_memset(&port_cfg, 0x0, sizeof(rtdrv_portCfg_t));
    osal_memcpy(&port_cfg.port, &port, sizeof(rtk_port_t));
    osal_memcpy(&port_cfg.enable, &enable, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_PORT_EEEENABLE_SET, &port_cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
} /* end of rtk_port_eeeEnable_set */

/* Function Name:
 *      rtk_port_eeeEnable_get
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
int32
rtk_port_eeeEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    rtdrv_portCfg_t port_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&port_cfg.port, &port, sizeof(rtk_port_t));
    GETSOCKOPT(RTDRV_PORT_EEEENABLE_GET, &port_cfg, rtdrv_portCfg_t, 1);
    osal_memcpy(pEnable, &port_cfg.enable, sizeof(rtk_enable_t));

    return RT_ERR_OK;
} /* end of rtk_port_eeeEnable_get */

/* Function Name:
 *      rtk_port_serdesMode_set
 * Description:
 *      Set Serdes Mode
 * Input:
 *      num   - serdes number
 *      cfg   - serdes mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 * Note:
 *      None
 */
int32
rtk_port_serdesMode_set(uint8 num, uint8 cfg)
{
    rtdrv_portCfg_t port_cfg;

    /* function body */
    osal_memset(&port_cfg, 0x0, sizeof(rtdrv_portCfg_t));
    osal_memcpy(&port_cfg.num, &num, sizeof(uint8));
    osal_memcpy(&port_cfg.cfg, &cfg, sizeof(uint8));
    SETSOCKOPT(RTDRV_PORT_SERDESMODE_SET, &port_cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_port_serdesMode_set */

/* Function Name:
 *      rtk_port_serdesMode_get
 * Description:
 *      Get Serdes Mode
 * Input:
 *      num   - serdes number
 * Output:
 *      cfg   - serdes mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtk_port_serdesMode_get(uint8 num, uint8 *cfg)
{
    rtdrv_portCfg_t port_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == cfg), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&port_cfg.num, &num, sizeof(uint8));
    GETSOCKOPT(RTDRV_PORT_SERDESMODE_GET, &port_cfg, rtdrv_portCfg_t, 1);
    osal_memcpy(cfg, &port_cfg.cfg, sizeof(uint8));

    return RT_ERR_OK;
}   /* end of rtk_port_serdesMode_get */

/* Function Name:
 *      rtk_port_serdesModeSpeed_set
 * Description:
 *      Set Serdes Mode and speed
 * Input:
 *      num   - serdes number
 *      mode  - serdes mode
 *      speed - serdes speed
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32
rtk_port_serdesModeSpeed_set(uint8 num, rtk_port_sds_mode_t mode, rtk_port_speed_t speed)
{
    rtdrv_portCfg_t port_cfg;

    /* function body */
    osal_memset(&port_cfg, 0x0, sizeof(rtdrv_portCfg_t));
    port_cfg.num = num;
    port_cfg.cfg = mode;
    port_cfg.speed = speed;
    SETSOCKOPT(RTDRV_PORT_SERDESMODESPEED_SET, &port_cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_port_serdesModeSpeed_get
 * Description:
 *      Get Serdes Mode
 * Input:
 *      num   - serdes number
 * Output:
 *      pMode - serdes mode
 *      pSpeed - serdes speed
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtk_port_serdesModeSpeed_get(uint8 num, rtk_port_sds_mode_t *pMode, rtk_port_speed_t *pSpeed)
{
    rtdrv_portCfg_t port_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pMode), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pSpeed), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&port_cfg.num, &num, sizeof(uint8));
    GETSOCKOPT(RTDRV_PORT_SERDESMODESPEED_GET, &port_cfg, rtdrv_portCfg_t, 1);
    *pMode = port_cfg.cfg;
    *pSpeed = port_cfg.speed;

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_port_serdesNWay_set
 * Description:
 *      Set Serdes N-Way Mode
 * Input:
 *      num   - serdes number
 *      cfg   - serdes n-way mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 * Note:
 *      None
 */
int32
rtk_port_serdesNWay_set(uint8 num, uint8 cfg)
{
    rtdrv_portCfg_t port_cfg;

    /* function body */
    osal_memset(&port_cfg, 0x0, sizeof(rtdrv_portCfg_t));
    osal_memcpy(&port_cfg.num, &num, sizeof(uint8));
    osal_memcpy(&port_cfg.cfg, &cfg, sizeof(uint8));
    SETSOCKOPT(RTDRV_PORT_SERDESNWAY_SET, &port_cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_port_serdesNWay_set */

/* Function Name:
 *      rtk_port_serdesNWay_get
 * Description:
 *      Get Serdes N-Way Mode
 * Input:
 *      num   - serdes number
 * Output:
 *      cfg   - serdes n-way mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtk_port_serdesNWay_get(uint8 num, uint8 *cfg)
{
    rtdrv_portCfg_t port_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == cfg), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&port_cfg.num, &num, sizeof(uint8));
    GETSOCKOPT(RTDRV_PORT_SERDESNWAY_GET, &port_cfg, rtdrv_portCfg_t, 1);
    osal_memcpy(cfg, &port_cfg.cfg, sizeof(uint8));

    return RT_ERR_OK;
}   /* end of rtk_port_serdesNWay_get */

/* Function Name:
 *      rtk_port_serdesTxRxPolarity_set
 * Description:
 *      Get Serdes Tx/Rx P/N polarity
 * Input:
 *      num   - serdes number
 *      tx_polairty   - Serdes Tx P/N polairty
 *      rx_polairty   - Serdes Rx P/N polairty
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters
 *      RT_ERR_NOT_ALLOWED  - Cannot configure the PN polarity to current SDS mode
 *      RT_ERR_FEATURE_NOT_SUPPORTED - Cannot configure the PN polarity on specific SDS index
 * Note:
 *      None
 */
int32
rtk_port_serdesTxRxPolarity_set(uint8 num, rtk_port_sds_pn_t tx_polarity, rtk_port_sds_pn_t rx_polarity)
{
    rtdrv_portCfg_t port_cfg;

    /* function body */
    osal_memset(&port_cfg, 0x0, sizeof(rtdrv_portCfg_t));
    osal_memcpy(&port_cfg.num, &num, sizeof(uint8));
    osal_memcpy(&port_cfg.txPolarity, &tx_polarity, sizeof(rtk_port_sds_pn_t));
    osal_memcpy(&port_cfg.rxPolarity, &rx_polarity, sizeof(rtk_port_sds_pn_t));
    SETSOCKOPT(RTDRV_PORT_SERDESTXRXPOLARITY_SET, &port_cfg, rtdrv_portCfg_t, 1);

    return RT_ERR_OK;
} /* end of rtk_port_serdesTxRxPolarity_set */

/* Function Name:
 *      rtk_port_serdesTxRxPolarity_get
 * Description:
 *      Get Serdes Tx/Rx P/N polarity
 * Input:
 *      num   - serdes number
 * Output:
 *      tx_polairty   - Serdes Tx P/N polairty
 *      rx_polairty   - Serdes Rx P/N polairty
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_NOT_ALLOWED  - Cannot retrieve the PN polarity from current SDS mode
 *      RT_ERR_FEATURE_NOT_SUPPORTED - Cannot retrieve the PN polarity on specific SDS index
 * Note:
 *      None
 */
int32
rtk_port_serdesTxRxPolarity_get(uint8 num, rtk_port_sds_pn_t *tx_polarity, rtk_port_sds_pn_t *rx_polarity)
{
    rtdrv_portCfg_t port_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == tx_polarity), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == rx_polarity), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&port_cfg.num, &num, sizeof(uint8));
    GETSOCKOPT(RTDRV_PORT_SERDESTXRXPOLARITY_GET, &port_cfg, rtdrv_portCfg_t, 1);
    osal_memcpy(tx_polarity, &port_cfg.txPolarity, sizeof(uint8));
    osal_memcpy(rx_polarity, &port_cfg.rxPolarity, sizeof(uint8));

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_port_serdesStatus_get
 * Description:
 *      Get Serdes status
 * Input:
 *      num   - serdes number
 * Output:
 *      pStatus   - serdes status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Incorrect input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtk_port_serdesStatus_get(uint8 num, rtk_port_sds_status_t *pStatus)
{
    rtdrv_portCfg_t port_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pStatus), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&port_cfg.num, &num, sizeof(uint8));
    GETSOCKOPT(RTDRV_PORT_SERDESSTATUS_GET, &port_cfg, rtdrv_portCfg_t, 1);
    osal_memcpy(pStatus, &port_cfg.sdsStatus, sizeof(rtk_port_sds_status_t));

    return RT_ERR_OK;
}
