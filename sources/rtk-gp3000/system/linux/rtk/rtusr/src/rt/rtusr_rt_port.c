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
#include <rtk/rt/rt_port.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
uint32 rt_port0 = RT_PORT_INVALID;
uint32 rt_port1 = RT_PORT_INVALID;
uint32 rt_port2 = RT_PORT_INVALID;
uint32 rt_port3 = RT_PORT_INVALID;
uint32 rt_port4 = RT_PORT_INVALID;
uint32 rt_port5 = RT_PORT_INVALID;
uint32 rt_port6 = RT_PORT_INVALID;
uint32 rt_port7 = RT_PORT_INVALID;
uint32 rt_port_pon = RT_PORT_INVALID;
uint32 rt_port_cpu0 = RT_PORT_INVALID;
uint32 rt_port_cpu1 = RT_PORT_INVALID;
uint32 rt_port_cpu2 = RT_PORT_INVALID;
uint32 rt_port_cpu3 = RT_PORT_INVALID;
uint32 rt_port_cpu4 = RT_PORT_INVALID;
uint32 rt_port_cpu5 = RT_PORT_INVALID;
uint32 rt_port_cpu6 = RT_PORT_INVALID;
uint32 rt_port_cpu7 = RT_PORT_INVALID;
uint32 rt_port_max = 0;

uint32 rt_port_phy_port0_bit = 0;
uint32 rt_port_phy_port1_bit = 0;
uint32 rt_port_phy_port2_bit = 0;
uint32 rt_port_phy_port3_bit = 0;
uint32 rt_port_phy_port4_bit = 0;
uint32 rt_port_phy_pon_bit = 0;
uint32 rt_port_phy_port0 = RT_PORT_INVALID;
uint32 rt_port_phy_port1 = RT_PORT_INVALID;
uint32 rt_port_phy_port2 = RT_PORT_INVALID;
uint32 rt_port_phy_port3 = RT_PORT_INVALID;
uint32 rt_port_phy_port4 = RT_PORT_INVALID;
uint32 rt_port_phy_pon = RT_PORT_INVALID;


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
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Module must be initialized before using all of APIs in this module
 */
int32
rt_port_init(void)
{
    rtdrv_rt_portCfg_t rt_port_cfg;
    memset(&rt_port_cfg, 0x0, sizeof(rtdrv_rt_portCfg_t));

    /* function body */
    SETSOCKOPT(RTDRV_RT_PORT_INIT, &rt_port_cfg, rtdrv_rt_portCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_port_init */

/* Function Name:
 *      rt_port_portDef_init
 * Description:
 *      Initialize port define initializations
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Only user space should call this to initialize the port definitions
 */
int32
rt_port_portDef_init(void)
{
    rtdrv_rt_portCfg_t rt_port_cfg;
    memset(&rt_port_cfg, 0x0, sizeof(rtdrv_rt_portCfg_t));

    /* function body */
    /* Reteive the port definitions */
    GETSOCKOPT(RTDRV_RT_PORT_PORTDEF_GET, &rt_port_cfg, rtdrv_rt_portCfg_t, 1);
    rt_port0 = rt_port_cfg.rt_port0;
    rt_port1 = rt_port_cfg.rt_port1;
    rt_port2 = rt_port_cfg.rt_port2;
    rt_port3 = rt_port_cfg.rt_port3;
    rt_port4 = rt_port_cfg.rt_port4;
    rt_port5 = rt_port_cfg.rt_port5;
    rt_port6 = rt_port_cfg.rt_port6;
    rt_port7 = rt_port_cfg.rt_port7;
    rt_port_pon = rt_port_cfg.rt_port_pon;
    rt_port_cpu0 = rt_port_cfg.rt_port_cpu0;
    rt_port_cpu1 = rt_port_cfg.rt_port_cpu1;
    rt_port_cpu2 = rt_port_cfg.rt_port_cpu2;
    rt_port_cpu3 = rt_port_cfg.rt_port_cpu3;
    rt_port_cpu4 = rt_port_cfg.rt_port_cpu4;
    rt_port_cpu5 = rt_port_cfg.rt_port_cpu5;
    rt_port_cpu6 = rt_port_cfg.rt_port_cpu6;
    rt_port_cpu7 = rt_port_cfg.rt_port_cpu7;
    rt_port_max = rt_port_cfg.rt_port_max;

	RT_PORT_PHY_PORT_PORT0_BIT = (RT_PORT0 == RT_PORT_INVALID) ? 0 : (0x1<<RT_PORT0);
	RT_PORT_PHY_PORT_PORT1_BIT = (RT_PORT1 == RT_PORT_INVALID) ? 0 : (0x1<<RT_PORT1);
	RT_PORT_PHY_PORT_PORT2_BIT = (RT_PORT2 == RT_PORT_INVALID) ? 0 : (0x1<<RT_PORT2);
	RT_PORT_PHY_PORT_PORT3_BIT = (RT_PORT3 == RT_PORT_INVALID) ? 0 : (0x1<<RT_PORT3);
	RT_PORT_PHY_PORT_PORT4_BIT = (RT_PORT4 == RT_PORT_INVALID) ? 0 : (0x1<<RT_PORT4);
	RT_PORT_PHY_PORT_PON_BIT = (RT_PORT_PON == RT_PORT_INVALID) ? 0 : (0x1<<RT_PORT_PON);

	RT_PORT_PHY_PORT_PORT0 = (RT_PORT0);
	RT_PORT_PHY_PORT_PORT1 = (RT_PORT1);
	RT_PORT_PHY_PORT_PORT2 = (RT_PORT2);
	RT_PORT_PHY_PORT_PORT3 = (RT_PORT3);
	RT_PORT_PHY_PORT_PORT4 = (RT_PORT4);
	RT_PORT_PHY_PORT_PON = (RT_PORT_PON);

    return RT_ERR_OK;
}   /* end of rt_port_portDef_init */

/* Function Name:
 *      rt_port_link_get
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
rt_port_link_get(rt_port_t port, rt_port_linkStatus_t *pLinkStatus)
{
    rtdrv_rt_portCfg_t rt_port_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pLinkStatus), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_port_cfg.port, &port, sizeof(rt_port_t));
    GETSOCKOPT(RTDRV_RT_PORT_LINK_GET, &rt_port_cfg, rtdrv_rt_portCfg_t, 1);
    osal_memcpy(pLinkStatus, &rt_port_cfg.linkStatus, sizeof(rt_port_linkStatus_t));

    return RT_ERR_OK;
}   /* end of rt_port_link_get */

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
rt_port_speedDuplex_get(
    rt_port_t        port,
    rt_port_speed_t  *pSpeed,
    rt_port_duplex_t *pDuplex)
{
    rtdrv_rt_portCfg_t rt_port_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pSpeed), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pDuplex), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_port_cfg.port, &port, sizeof(rt_port_t));
    GETSOCKOPT(RTDRV_RT_PORT_SPEEDDUPLEX_GET, &rt_port_cfg, rtdrv_rt_portCfg_t, 1);
    osal_memcpy(pSpeed, &rt_port_cfg.speed, sizeof(rt_port_speed_t));
    osal_memcpy(pDuplex, &rt_port_cfg.duplex, sizeof(rt_port_duplex_t));

    return RT_ERR_OK;
}   /* end of rt_port_speedDuplex_get */

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
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID       - invalid port id
 *      RT_ERR_NULL_POINTER  - input parameter may be null pointer
 *      RT_ERR_PORT_LINKDOWN - link down port status
 * Note:
 *      None
 */
int32
rt_port_flowctrl_get(
    rt_port_t  port,
    uint32      *pTxStatus,
    uint32      *pRxStatus)
{
    rtdrv_rt_portCfg_t rt_port_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pTxStatus), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pRxStatus), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_port_cfg.port, &port, sizeof(rt_port_t));
    GETSOCKOPT(RTDRV_RT_PORT_FLOWCTRL_GET, &rt_port_cfg, rtdrv_rt_portCfg_t, 1);
    osal_memcpy(pTxStatus, &rt_port_cfg.txStatus, sizeof(uint32));
    osal_memcpy(pRxStatus, &rt_port_cfg.rxStatus, sizeof(uint32));

    return RT_ERR_OK;
}   /* end of rt_port_flowctrl_get */

/* Function Name:
 *      rt_port_phyAutoNegoEnable_get
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
rt_port_phyAutoNegoEnable_get(rt_port_t port, rt_enable_t *pEnable)
{
    rtdrv_rt_portCfg_t rt_port_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_port_cfg.port, &port, sizeof(rt_port_t));
    GETSOCKOPT(RTDRV_RT_PORT_PHYAUTONEGOENABLE_GET, &rt_port_cfg, rtdrv_rt_portCfg_t, 1);
    osal_memcpy(pEnable, &rt_port_cfg.enable, sizeof(rt_enable_t));

    return RT_ERR_OK;
}   /* end of rt_port_phyAutoNegoEnable_get */

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
rt_port_phyAutoNegoEnable_set(rt_port_t port, rt_enable_t enable)
{
    rtdrv_rt_portCfg_t rt_port_cfg;
    memset(&rt_port_cfg, 0x0, sizeof(rtdrv_rt_portCfg_t));

    /* function body */
    osal_memcpy(&rt_port_cfg.port, &port, sizeof(rt_port_t));
    osal_memcpy(&rt_port_cfg.enable, &enable, sizeof(rt_enable_t));
    SETSOCKOPT(RTDRV_RT_PORT_PHYAUTONEGOENABLE_SET, &rt_port_cfg, rtdrv_rt_portCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_port_phyAutoNegoEnable_set */

/* Function Name:
 *      rt_port_phyAutoNegoAbility_get
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
rt_port_phyAutoNegoAbility_get(
    rt_port_t              port,
    rt_port_phy_ability_t  *pAbility)
{
    rtdrv_rt_portCfg_t rt_port_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pAbility), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_port_cfg.port, &port, sizeof(rt_port_t));
    GETSOCKOPT(RTDRV_RT_PORT_PHYAUTONEGOABILITY_GET, &rt_port_cfg, rtdrv_rt_portCfg_t, 1);
    osal_memcpy(pAbility, &rt_port_cfg.ability, sizeof(rt_port_phy_ability_t));

    return RT_ERR_OK;
}   /* end of rt_port_phyAutoNegoAbility_get */

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
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      You can set these abilities no matter which mode PHY currently stays on
 */
int32
rt_port_phyAutoNegoAbility_set(
    rt_port_t              port,
    rt_port_phy_ability_t  *pAbility)
{
    rtdrv_rt_portCfg_t rt_port_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pAbility), RT_ERR_NULL_POINTER);
    memset(&rt_port_cfg, 0x0, sizeof(rtdrv_rt_portCfg_t));

    /* function body */
    osal_memcpy(&rt_port_cfg.port, &port, sizeof(rt_port_t));
    osal_memcpy(&rt_port_cfg.ability, pAbility, sizeof(rt_port_phy_ability_t));
    SETSOCKOPT(RTDRV_RT_PORT_PHYAUTONEGOABILITY_SET, &rt_port_cfg, rtdrv_rt_portCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_port_phyAutoNegoAbility_set */

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
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rt_port_phyForceModeAbility_get(
    rt_port_t          port,
    rt_port_speed_t    *pSpeed,
    rt_port_duplex_t   *pDuplex,
    rt_enable_t        *pFlowControl)
{
    rtdrv_rt_portCfg_t rt_port_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pSpeed), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pDuplex), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pFlowControl), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_port_cfg.port, &port, sizeof(rt_port_t));
    GETSOCKOPT(RTDRV_RT_PORT_PHYFORCEMODEABILITY_GET, &rt_port_cfg, rtdrv_rt_portCfg_t, 1);
    osal_memcpy(pSpeed, &rt_port_cfg.speed, sizeof(rt_port_speed_t));
    osal_memcpy(pDuplex, &rt_port_cfg.duplex, sizeof(rt_port_duplex_t));
    osal_memcpy(pFlowControl, &rt_port_cfg.flowControl, sizeof(rt_enable_t));

    return RT_ERR_OK;
}   /* end of rt_port_phyForceModeAbility_get */

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
rt_port_phyForceModeAbility_set(
    rt_port_t          port,
    rt_port_speed_t    speed,
    rt_port_duplex_t   duplex,
    rt_enable_t        flowControl)
{
    rtdrv_rt_portCfg_t rt_port_cfg;
    memset(&rt_port_cfg, 0x0, sizeof(rtdrv_rt_portCfg_t));

    /* function body */
    osal_memcpy(&rt_port_cfg.port, &port, sizeof(rt_port_t));
    osal_memcpy(&rt_port_cfg.speed, &speed, sizeof(rt_port_speed_t));
    osal_memcpy(&rt_port_cfg.duplex, &duplex, sizeof(rt_port_duplex_t));
    osal_memcpy(&rt_port_cfg.flowControl, &flowControl, sizeof(rt_enable_t));
    SETSOCKOPT(RTDRV_RT_PORT_PHYFORCEMODEABILITY_SET, &rt_port_cfg, rtdrv_rt_portCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_port_phyForceModeAbility_set */

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
rt_port_phyReg_get(
    rt_port_t          port,
    uint32              page,
    rt_port_phy_reg_t  reg,
    uint32              *pData)
{
    rtdrv_rt_portCfg_t rt_port_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_port_cfg.port, &port, sizeof(rt_port_t));
    osal_memcpy(&rt_port_cfg.page, &page, sizeof(uint32));
    osal_memcpy(&rt_port_cfg.reg, &reg, sizeof(rt_port_phy_reg_t));
    GETSOCKOPT(RTDRV_RT_PORT_PHYREG_GET, &rt_port_cfg, rtdrv_rt_portCfg_t, 1);
    osal_memcpy(pData, &rt_port_cfg.data, sizeof(uint32));

    return RT_ERR_OK;
}   /* end of rt_port_phyReg_get */

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
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID     - invalid port id
 *      RT_ERR_PHY_PAGE_ID - invalid page id
 *      RT_ERR_PHY_REG_ID  - invalid reg id
 * Note:
 *      None
 */
int32
rt_port_phyReg_set(
    rt_port_t          port,
    uint32              page,
    rt_port_phy_reg_t  reg,
    uint32              data)
{
    rtdrv_rt_portCfg_t rt_port_cfg;
    memset(&rt_port_cfg, 0x0, sizeof(rtdrv_rt_portCfg_t));

    /* function body */
    osal_memcpy(&rt_port_cfg.port, &port, sizeof(rt_port_t));
    osal_memcpy(&rt_port_cfg.page, &page, sizeof(uint32));
    osal_memcpy(&rt_port_cfg.reg, &reg, sizeof(rt_port_phy_reg_t));
    osal_memcpy(&rt_port_cfg.data, &data, sizeof(uint32));
    SETSOCKOPT(RTDRV_RT_PORT_PHYREG_SET, &rt_port_cfg, rtdrv_rt_portCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_port_phyReg_set */

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
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID     - invalid port id
 * Note:
 *      None
 */
int32
rt_port_phyPowerDown_get(rt_port_t port, rt_enable_t *pEnable)
{
    rtdrv_rt_portCfg_t rt_port_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_port_cfg.port, &port, sizeof(rt_port_t));
    GETSOCKOPT(RTDRV_RT_PORT_PHYPOWERDOWN_GET, &rt_port_cfg, rtdrv_rt_portCfg_t, 1);
    osal_memcpy(pEnable, &rt_port_cfg.enable, sizeof(rt_enable_t));

    return RT_ERR_OK;
}   /* end of rt_port_phyPowerDown_get */

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
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID     - invalid port id
 * Note:
 *      None
 */
int32
rt_port_phyPowerDown_set(rt_port_t port, rt_enable_t enable)
{
    rtdrv_rt_portCfg_t rt_port_cfg;
    memset(&rt_port_cfg, 0x0, sizeof(rtdrv_rt_portCfg_t));

    /* function body */
    osal_memcpy(&rt_port_cfg.port, &port, sizeof(rt_port_t));
    osal_memcpy(&rt_port_cfg.enable, &enable, sizeof(rt_enable_t));
    SETSOCKOPT(RTDRV_RT_PORT_PHYPOWERDOWN_SET, &rt_port_cfg, rtdrv_rt_portCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_port_phyPowerDown_set */

/* Function Name:
 *      rt_port_adminEnable_get
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
rt_port_adminEnable_get(rt_port_t port, rt_enable_t *pEnable)
{
    rtdrv_rt_portCfg_t rt_port_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_port_cfg.port, &port, sizeof(rt_port_t));
    GETSOCKOPT(RTDRV_RT_PORT_ADMINENABLE_GET, &rt_port_cfg, rtdrv_rt_portCfg_t, 1);
    osal_memcpy(pEnable, &rt_port_cfg.enable, sizeof(rt_enable_t));

    return RT_ERR_OK;
}   /* end of rt_port_adminEnable_get */

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
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      None
 */
int32
rt_port_adminEnable_set(rt_port_t port, rt_enable_t enable)
{
    rtdrv_rt_portCfg_t rt_port_cfg;
    memset(&rt_port_cfg, 0x0, sizeof(rtdrv_rt_portCfg_t));

    /* function body */
    osal_memcpy(&rt_port_cfg.port, &port, sizeof(rt_port_t));
    osal_memcpy(&rt_port_cfg.enable, &enable, sizeof(rt_enable_t));
    SETSOCKOPT(RTDRV_RT_PORT_ADMINENABLE_SET, &rt_port_cfg, rtdrv_rt_portCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_port_adminEnable_set */

/* Function Name:
 *      rt_port_isolationEntry_get
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
rt_port_isolationEntry_get(rt_port_isoConfig_t mode, rt_port_t port, rt_portmask_t *pPortmask, rt_portmask_t *pExtPortmask)
{
    rtdrv_rt_portCfg_t rt_port_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pExtPortmask), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_port_cfg.mode, &mode, sizeof(rt_port_isoConfig_t));
    osal_memcpy(&rt_port_cfg.port, &port, sizeof(rt_port_t));
    GETSOCKOPT(RTDRV_RT_PORT_ISOLATIONENTRY_GET, &rt_port_cfg, rtdrv_rt_portCfg_t, 1);
    osal_memcpy(pPortmask, &rt_port_cfg.portmask, sizeof(rt_portmask_t));
    osal_memcpy(pExtPortmask, &rt_port_cfg.extPortmask, sizeof(rt_portmask_t));

    return RT_ERR_OK;
}   /* end of rt_port_isolationEntry_get */

/* Function Name:
 *      rt_port_isolationEntry_set
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
rt_port_isolationEntry_set(rt_port_isoConfig_t mode, rt_port_t port, rt_portmask_t *pPortmask, rt_portmask_t *pExtPortmask)
{
    rtdrv_rt_portCfg_t rt_port_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pExtPortmask), RT_ERR_NULL_POINTER);
    memset(&rt_port_cfg, 0x0, sizeof(rtdrv_rt_portCfg_t));

    /* function body */
    osal_memcpy(&rt_port_cfg.mode, &mode, sizeof(rt_port_isoConfig_t));
    osal_memcpy(&rt_port_cfg.port, &port, sizeof(rt_port_t));
    osal_memcpy(&rt_port_cfg.portmask, pPortmask, sizeof(rt_portmask_t));
    osal_memcpy(&rt_port_cfg.extPortmask, pExtPortmask, sizeof(rt_portmask_t));
    SETSOCKOPT(RTDRV_RT_PORT_ISOLATIONENTRY_SET, &rt_port_cfg, rtdrv_rt_portCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_port_isolationEntry_set */

/* Function Name:
 *      rt_port_uniType_get
 * Description:
 *      Get port uni type of the specific port
 * Input:
 *      port     - port id
 * Output:
 *      pUniType - pointer to the port uni type
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rt_port_uniType_get(rt_port_t port, rt_port_uniType_t *pUniType)
{
    rtdrv_rt_portCfg_t rt_port_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pUniType), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_port_cfg.port, &port, sizeof(rt_port_t));
    GETSOCKOPT(RTDRV_RT_PORT_UNITYPE_GET, &rt_port_cfg, rtdrv_rt_portCfg_t, 1);
    osal_memcpy(pUniType, &rt_port_cfg.uniType, sizeof(rt_port_uniType_t));

    return RT_ERR_OK;
}   /* end of rt_port_uniType_get */

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
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      None
 */
int32
rt_port_uniType_set(rt_port_t port, rt_port_uniType_t uniType)
{
    rtdrv_rt_portCfg_t rt_port_cfg;
    memset(&rt_port_cfg, 0x0, sizeof(rtdrv_rt_portCfg_t));

    /* function body */
    osal_memcpy(&rt_port_cfg.port, &port, sizeof(rt_port_t));
    osal_memcpy(&rt_port_cfg.uniType, &uniType, sizeof(rt_port_uniType_t));
    SETSOCKOPT(RTDRV_RT_PORT_UNITYPE_SET, &rt_port_cfg, rtdrv_rt_portCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_port_uniType_set */

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
int32
rt_port_macForceAbility_set(rt_port_t port, rt_port_macAbility_t ability)
{
    rtdrv_rt_portCfg_t port_cfg;

    /* function body */
    osal_memset(&port_cfg, 0x0, sizeof(rtdrv_rt_portCfg_t));
    osal_memcpy(&port_cfg.port, &port, sizeof(rt_port_t));
    osal_memcpy(&port_cfg.macAbility, &ability, sizeof(rt_port_macAbility_t));
    SETSOCKOPT(RTDRV_RT_PORT_MACFORCEABILITY_SET, &port_cfg, rtdrv_rt_portCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_port_macForceAbility_set */

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
int32
rt_port_macForceAbility_get(rt_port_t port, rt_port_macAbility_t *pAbility)
{
    rtdrv_rt_portCfg_t port_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pAbility), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&port_cfg.port, &port, sizeof(rt_port_t));
    GETSOCKOPT(RTDRV_RT_PORT_MACFORCEABILITY_GET, &port_cfg, rtdrv_rt_portCfg_t, 1);
    osal_memcpy(pAbility, &port_cfg.macAbility, sizeof(rt_port_macAbility_t));

    return RT_ERR_OK;
}   /* end of rt_port_macForceAbility_get */

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
int32
rt_port_macForceAbilityState_set(rt_port_t port, rt_enable_t state)
{
    rtdrv_rt_portCfg_t port_cfg;

    /* function body */
    osal_memset(&port_cfg, 0x0, sizeof(rtdrv_rt_portCfg_t));
    osal_memcpy(&port_cfg.port, &port, sizeof(rt_port_t));
    osal_memcpy(&port_cfg.enable, &state, sizeof(rt_enable_t));
    SETSOCKOPT(RTDRV_RT_PORT_MACFORCEABILITYSTATE_SET, &port_cfg, rtdrv_rt_portCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_port_macForceAbilityState_set */

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
int32
rt_port_macForceAbilityState_get(rt_port_t port, rt_enable_t *pState)
{
    rtdrv_rt_portCfg_t port_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&port_cfg.port, &port, sizeof(rt_port_t));
    GETSOCKOPT(RTDRV_RT_PORT_MACFORCEABILITYSTATE_GET, &port_cfg, rtdrv_rt_portCfg_t, 1);
    osal_memcpy(pState, &port_cfg.enable, sizeof(rt_enable_t));

    return RT_ERR_OK;
}   /* end of rt_port_macForceAbilityState_get */

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
int32
rt_port_eeeEnable_set(rt_port_t port, rt_enable_t enable)
{
    rtdrv_rt_portCfg_t port_cfg;

    /* function body */
    osal_memset(&port_cfg, 0x0, sizeof(rtdrv_rt_portCfg_t));
    osal_memcpy(&port_cfg.port, &port, sizeof(rt_port_t));
    osal_memcpy(&port_cfg.enable, &enable, sizeof(rt_enable_t));
    SETSOCKOPT(RTDRV_RT_PORT_EEEENABLE_SET, &port_cfg, rtdrv_rt_portCfg_t, 1);

    return RT_ERR_OK;
} /* end of rt_port_eeeEnable_set */

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
int32
rt_port_eeeEnable_get(rt_port_t port, rt_enable_t *pEnable)
{
    rtdrv_rt_portCfg_t port_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&port_cfg.port, &port, sizeof(rt_port_t));
    GETSOCKOPT(RTDRV_RT_PORT_EEEENABLE_GET, &port_cfg, rtdrv_rt_portCfg_t, 1);
    osal_memcpy(pEnable, &port_cfg.enable, sizeof(rt_enable_t));

    return RT_ERR_OK;
} /* end of rt_port_eeeEnable_get */

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
int32
rt_port_macLoopbackEnable_get(rt_port_t port, rt_enable_t *pEnable, rt_enable_t *pSwapEnable)
{
    rtdrv_rt_portCfg_t rt_port_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pSwapEnable), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&rt_port_cfg, 0x0, sizeof(rtdrv_rt_portCfg_t));
    osal_memcpy(&rt_port_cfg.port, &port, sizeof(rt_port_t));
    GETSOCKOPT(RTDRV_RT_PORT_MACLOOPBACKENABLE_GET, &rt_port_cfg, rtdrv_rt_portCfg_t, 1);
    osal_memcpy(pEnable, &rt_port_cfg.enable, sizeof(rt_enable_t));
    osal_memcpy(pSwapEnable, &rt_port_cfg.swapEnable, sizeof(rt_enable_t));

    return RT_ERR_OK;
}   /* end of rt_port_macLoopbackEnable_get */

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
int32
rt_port_macLoopbackEnable_set(rt_port_t port, rt_enable_t enable, rt_enable_t swapEnable)
{
    rtdrv_rt_portCfg_t rt_port_cfg;

    /* function body */
    osal_memset(&rt_port_cfg, 0x0, sizeof(rtdrv_rt_portCfg_t));
    osal_memcpy(&rt_port_cfg.port, &port, sizeof(rt_port_t));
    osal_memcpy(&rt_port_cfg.enable, &enable, sizeof(rt_enable_t));
    osal_memcpy(&rt_port_cfg.swapEnable, &swapEnable, sizeof(rt_enable_t));
    SETSOCKOPT(RTDRV_RT_PORT_MACLOOPBACKENABLE_SET, &rt_port_cfg, rtdrv_rt_portCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_port_macLoopbackEnable_set */

