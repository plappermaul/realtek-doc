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
#include <common/rt_type.h>
#include <rtk/init.h>
#include <rtk/default.h>
#include <rtk/port.h>
#include <dal/dal_mgmt.h>


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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_init)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->port_init();
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_init */

/* Module Name    : Port                                       */
/* Sub-module Name: Parameter settings for the port-based view */

/* Function Name:
 *      rtk_port_link_get
 * Description:
 *      Get the link status of the specific port
 * Input:
 *      port    - port id
 * Output:
 *      pStatus - pointer to the link status
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
rtk_port_link_get(rtk_port_t port, rtk_port_linkStatus_t *pStatus)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_link_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->port_link_get( port, pStatus);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_link_get */

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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_speedDuplex_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->port_speedDuplex_get( port, pSpeed, pDuplex);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_speedDuplex_get */

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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_flowctrl_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->port_flowctrl_get( port, pTxStatus, pRxStatus);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_flowctrl_get */

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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_phyAutoNegoEnable_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->port_phyAutoNegoEnable_get( port, pEnable);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_phyAutoNegoEnable_get */

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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_phyAutoNegoEnable_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->port_phyAutoNegoEnable_set( port, enable);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_phyAutoNegoEnable_set */

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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_phyAutoNegoAbility_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->port_phyAutoNegoAbility_get( port, pAbility);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_phyAutoNegoAbility_get */

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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_phyAutoNegoAbility_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->port_phyAutoNegoAbility_set( port, pAbility);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_phyAutoNegoAbility_set */

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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_phyForceModeAbility_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->port_phyForceModeAbility_get( port, pSpeed, pDuplex, pFlowControl);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_phyForceModeAbility_get */

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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_phyForceModeAbility_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->port_phyForceModeAbility_set( port, speed, duplex, flowControl);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_phyForceModeAbility_set */

/* Function Name:
 *      rtk_port_phyReg_get
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
rtk_port_phyReg_get(
    rtk_port_t          port,
    uint32              page,
    rtk_port_phy_reg_t  reg,
    uint32              *pData)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_phyReg_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->port_phyReg_get( port, page, reg, pData);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_phyReg_get */

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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_phyReg_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->port_phyReg_set( port, page, reg, data);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_phyReg_set */

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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_phyMasterSlave_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->port_phyMasterSlave_get( port, pMasterSlave);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_phyMasterSlave_get */

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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_phyMasterSlave_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->port_phyMasterSlave_set( port, masterSlave);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_phyMasterSlave_set */

/* Function Name:
 *      rtk_port_phyTestMode_get
 * Description:
 *      Get PHY in which test mode.
 * Input:
 *      port - Port id.
 * Output:
 *      pTestMode - PHY test mode 0:normal 1:test mode 1 2:test mode 2 3: test mode 3 4:test mode 4 5~7:reserved
 * Return:
 *      RT_ERR_OK              	- OK
 *      RT_ERR_FAILED          	- Failed
 *      RT_ERR_SMI             	- SMI access error
 *      RT_ERR_PORT_ID 			- Invalid port number.
 *      RT_ERR_INPUT 			- Invalid input parameters.
 *      RT_ERR_BUSYWAIT_TIMEOUT - PHY access busy
 * Note:
 *      Get test mode of PHY from register setting 9.15 to 9.13.
 */
int32
rtk_port_phyTestMode_get(rtk_port_t port, rtk_port_phy_test_mode_t *pTestMode)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_phyTestMode_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->port_phyTestMode_get(port, pTestMode);
    RTK_API_UNLOCK();
    return ret;
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
 *      RT_ERR_OK              	- OK
 *      RT_ERR_FAILED          	- Failed
 *      RT_ERR_SMI             	- SMI access error
 *      RT_ERR_PORT_ID 			- Invalid port number.
 *      RT_ERR_BUSYWAIT_TIMEOUT - PHY access busy
 * Note:
 *      None.
 */
int32
rtk_port_phyTestMode_set(rtk_port_t port, rtk_port_phy_test_mode_t testMode)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_phyTestMode_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    
    RTK_API_LOCK();
    ret = RT_MAPPER->port_phyTestMode_set(port, testMode);
    RTK_API_UNLOCK();
    return ret;
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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_cpuPortId_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->port_cpuPortId_get( pPort);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_cpuPortId_get */

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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_isolation_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->port_isolation_get( port, pPortmask, pExtPortmask);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_isolation_get */

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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_isolation_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->port_isolation_set( port, pPortmask, pExtPortmask);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_isolation_set */

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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_isolationExt_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->port_isolationExt_get( port, pPortmask, pExtPortmask);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_isolationExt_get */

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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_isolationExt_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->port_isolationExt_set( port, pPortmask, pExtPortmask);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_isolationExt_set */

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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_isolationL34_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->port_isolationL34_get( port, pPortmask, pExtPortmask);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_isolationL34_get */

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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_isolationL34_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->port_isolationL34_set( port, pPortmask, pExtPortmask);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_isolationL34_set */

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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_isolationExtL34_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->port_isolationExtL34_get( port, pPortmask, pExtPortmask);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_isolationExtL34_get */

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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_isolationExtL34_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->port_isolationExtL34_set( port, pPortmask, pExtPortmask);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_isolationExtL34_set */

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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_isolationEntry_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->port_isolationEntry_get( mode, port, pPortmask, pExtPortmask);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_isolationEntry_get */

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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_isolationEntry_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->port_isolationEntry_set( mode, port, pPortmask, pExtPortmask);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_isolationEntry_set */

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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_isolationEntryExt_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->port_isolationEntryExt_get( mode, port, pPortmask, pExtPortmask);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_isolationEntryExt_get */

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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_isolationEntryExt_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->port_isolationEntryExt_set(mode, port, pPortmask, pExtPortmask);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_isolationEntryExt_set */

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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_isolationCtagPktConfig_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->port_isolationCtagPktConfig_get( pMode);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_isolationCtagPktConfig_get */

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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_isolationCtagPktConfig_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->port_isolationCtagPktConfig_set( mode);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_isolationCtagPktConfig_set */

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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_isolationL34PktConfig_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->port_isolationL34PktConfig_get( pMode);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_isolationL34PktConfig_get */

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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_isolationL34PktConfig_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->port_isolationL34PktConfig_set( mode);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_isolationL34PktConfig_set */

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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_isolationIpmcLeaky_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->port_isolationIpmcLeaky_get( port, pEnable);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_isolationIpmcLeaky_get */

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
 * Note:
 *      none
 */
int32
rtk_port_isolationIpmcLeaky_set(rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_isolationIpmcLeaky_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->port_isolationIpmcLeaky_set( port, enable);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_isolationIpmcLeaky_get */

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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_isolationPortLeaky_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->port_isolationPortLeaky_get(port, type, pEnable);
    RTK_API_UNLOCK();
    return ret;
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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_isolationPortLeaky_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->port_isolationPortLeaky_set(port, type, enable);
    RTK_API_UNLOCK();
    return ret;
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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_isolationLeaky_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->port_isolationLeaky_get(type, pEnable);
    RTK_API_UNLOCK();
    return ret;
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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_isolationLeaky_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->port_isolationLeaky_set(type, enable);
    RTK_API_UNLOCK();
    return ret;
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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_macRemoteLoopbackEnable_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->port_macRemoteLoopbackEnable_get( port, pEnable);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_macRemoteLoopbackEnable_get */

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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_macRemoteLoopbackEnable_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->port_macRemoteLoopbackEnable_set( port, enable);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_macRemoteLoopbackEnable_set */

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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_macLocalLoopbackEnable_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->port_macLocalLoopbackEnable_get( port, pEnable);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_macLocalLoopbackEnable_get */

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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_macLocalLoopbackEnable_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->port_macLocalLoopbackEnable_set( port, enable);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_macLocalLoopbackEnable_set */

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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_adminEnable_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->port_adminEnable_get( port, pEnable);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_adminEnable_get */

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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_adminEnable_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->port_adminEnable_set( port, enable);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_adminEnable_set */

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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_specialCongest_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->port_specialCongest_get( port, pSecond);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_specialCongest_get */

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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_specialCongest_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->port_specialCongest_set( port, second);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_specialCongest_set */

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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_specialCongestStatus_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->port_specialCongestStatus_get( port, pStatus);
    RTK_API_UNLOCK();
    return ret;
}

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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_specialCongestStatus_clear)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->port_specialCongestStatus_clear( port);
    RTK_API_UNLOCK();
    return ret;
}

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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_greenEnable_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->port_greenEnable_get( port, pEnable);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_greenEnable_get */

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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_greenEnable_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->port_greenEnable_set( port, enable);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_greenEnable_set */


/* Function Name:
 *      rtk_port_phyCrossOverMode_get
 * Description:
 *      Get cross over mode in the specified port.
 * Input:
 *      port  - port id
 * Output:
 *      pMode - pointer to cross over mode
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
rtk_port_phyCrossOverMode_get(rtk_port_t port, rtk_port_crossOver_mode_t *pMode)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_phyCrossOverMode_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->port_phyCrossOverMode_get( port, pMode);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_phyCrossOverMode_get */

/* Function Name:
 *      rtk_port_phyCrossOverMode_set
 * Description:
 *      Set cross over mode in the specified port.
 * Input:
 *      port - port id
 *      mode - cross over mode
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
rtk_port_phyCrossOverMode_set(rtk_port_t port, rtk_port_crossOver_mode_t mode)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_phyCrossOverMode_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->port_phyCrossOverMode_set( port, mode);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_phyCrossOverMode_set */

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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_phyPowerDown_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->port_phyPowerDown_get( port, pEnable);
    RTK_API_UNLOCK();
    return ret;
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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_phyPowerDown_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->port_phyPowerDown_set( port, enable);
    RTK_API_UNLOCK();
    return ret;
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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_phyPowerDown_tryset)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->port_phyPowerDown_tryset( port, enable);
    RTK_API_UNLOCK();
    return ret;
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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_enhancedFid_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->port_enhancedFid_get( port, pEfid);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_enhancedFid_get */

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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_enhancedFid_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->port_enhancedFid_set( port, efid);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_enhancedFid_set */

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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_rtctResult_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->port_rtctResult_get( port, pResult);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_port_rtctResult_get */

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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_rtct_start)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->port_rtct_start( pPortmask);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_rtct_start */


/* Function Name:
 *      rtk_port_macForceAbility_set
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
 *      None
 */
int32
rtk_port_macForceAbility_set(rtk_port_t port,rtk_port_macAbility_t macAbility)
{
	int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_macForceAbility_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->port_macForceAbility_set(port,macAbility);
    RTK_API_UNLOCK();
    return ret;
}


/* Function Name:
 *      rtk_port_macForceAbility_get
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
 *      None
 */
int32
rtk_port_macForceAbility_get(rtk_port_t port,rtk_port_macAbility_t *pMacAbility)
{
	int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_macForceAbility_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->port_macForceAbility_get(port,pMacAbility);
    RTK_API_UNLOCK();
    return ret;
}


/* Function Name:
 *      rtk_port_macForceAbility_set
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
 *      None
 */
extern int32
rtk_port_macForceAbilityState_set(rtk_port_t port,rtk_enable_t state)
{
	int32	ret;

    /* function body */
    if (NULL == RT_MAPPER->port_macForceAbilityState_set)
        return RT_ERR_DRIVER_NOT_FOUND;

	RTK_API_LOCK();
	ret = RT_MAPPER->port_macForceAbilityState_set(port,state);
	RTK_API_UNLOCK();
	return ret;
}



/* Function Name:
 *      rtk_port_macForceAbilityState_get
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
 *      None
 */
extern int32
rtk_port_macForceAbilityState_get(rtk_port_t port,rtk_enable_t *pState)
{
	int32	ret;

    /* function body */
    if (NULL == RT_MAPPER->port_macForceAbilityState_get)
        return RT_ERR_DRIVER_NOT_FOUND;

	RTK_API_LOCK();
	ret = RT_MAPPER->port_macForceAbilityState_get(port,pState);
	RTK_API_UNLOCK();
	return ret;
}

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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_macExtMode_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->port_macExtMode_set(port, ext_mode);
    RTK_API_UNLOCK();
    return ret;
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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_macExtMode_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->port_macExtMode_get(port, pExt_mode);
    RTK_API_UNLOCK();
    return ret;
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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_macExtRgmiiDelay_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->port_macExtRgmiiDelay_set(port, txDelay, rxDelay);
    RTK_API_UNLOCK();
    return ret;
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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_macExtRgmiiDelay_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->port_macExtRgmiiDelay_get(port, pTxDelay, pRxDelay);
    RTK_API_UNLOCK();
    return ret;
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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_gigaLiteEnable_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->port_gigaLiteEnable_set(port, enable);
    RTK_API_UNLOCK();
    return ret;
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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_gigaLiteEnable_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->port_gigaLiteEnable_get(port, pEnable);
    RTK_API_UNLOCK();
    return ret;
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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_eeeEnable_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->port_eeeEnable_set(port, enable);
    RTK_API_UNLOCK();
    return ret;
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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_eeeEnable_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->port_eeeEnable_get(port, pEnable);
    RTK_API_UNLOCK();
    return ret;
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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_serdesMode_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->port_serdesMode_set(num, cfg);
    RTK_API_UNLOCK();
    return ret;
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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_serdesMode_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->port_serdesMode_get(num, cfg);
    RTK_API_UNLOCK();
    return ret;
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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_serdesModeSpeed_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->port_serdesModeSpeed_set(num, mode, speed);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_port_serdesModeSpeed_set */

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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_serdesModeSpeed_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->port_serdesModeSpeed_get(num, pMode, pSpeed);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_port_serdesModeSpeed_get */

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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_serdesNWay_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->port_serdesNWay_set(num, cfg);
    RTK_API_UNLOCK();
    return ret;
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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_serdesNWay_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->port_serdesNWay_get(num, cfg);
    RTK_API_UNLOCK();
    return ret;
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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_serdesTxRxPolarity_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->port_serdesTxRxPolarity_set(num, tx_polarity, rx_polarity);
    RTK_API_UNLOCK();
    return ret;
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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_serdesTxRxPolarity_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->port_serdesTxRxPolarity_get(num, tx_polarity, rx_polarity);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_serdesTxRxPolarity_get */

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
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->port_serdesStatus_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->port_serdesStatus_get(num, pStatus);
    RTK_API_UNLOCK();
    return ret;
}
