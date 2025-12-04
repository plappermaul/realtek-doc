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
 * $Revision: 81643 $
 * $Date: 2017-08-24 13:28:53 +0800 (Thu, 24 Aug 2017) $
 *
 * Purpose : Definition of L2 API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) Mac address flush
 *           (2) Address learning limit
 *           (3) Parameter for L2 lookup and learning engine
 *           (4) Unicast address
 *           (5) L2 multicast
 *           (6) IP multicast
 *           (7) Multicast forwarding table
 *           (8) CPU mac
 *           (9) Port move
 *           (10) Parameter for lookup miss
 *           (11) Parameter for MISC
 *
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


/*
 * Macro Declaration
 */


/*
 * Function Declaration
 */


/* Function Name:
 *      rt_l2_init
 * Description:
 *      Initialize l2 module of the specified device.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize l2 module before calling any l2 APIs.
 */
int32
rt_l2_init(void)
{
    rtdrv_rt_l2Cfg_t rt_l2_cfg;
    memset(&rt_l2_cfg, 0x0, sizeof(rtdrv_rt_l2Cfg_t));

    /* function body */
    SETSOCKOPT(RTDRV_RT_L2_INIT, &rt_l2_cfg, rtdrv_rt_l2Cfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_l2_init */

/* Function Name:
 *      rt_l2_portLimitLearningCnt_get
 * Description:
 *      Get the maximum mac learning counts of the port.
 * Input:
 *      port     - port id
 * Output:
 *      pMacCnt - pointer buffer of maximum mac learning counts
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) The maximum mac learning counts only limit for dynamic learning mac
 *          address, not apply to static mac address.
 */
int32
rt_l2_portLimitLearningCnt_get(rt_port_t port, int32 *pMacCnt)
{
    rtdrv_rt_l2Cfg_t rt_l2_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pMacCnt), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_l2_cfg.port, &port, sizeof(rt_port_t));
    GETSOCKOPT(RTDRV_RT_L2_PORTLIMITLEARNINGCNT_GET, &rt_l2_cfg, rtdrv_rt_l2Cfg_t, 1);
    osal_memcpy(pMacCnt, &rt_l2_cfg.macCnt, sizeof(int32));

    return RT_ERR_OK;
}   /* end of rt_l2_portLimitLearningCnt_get */


/* Function Name:
 *      rt_l2_portLimitLearningCnt_set
 * Description:
 *      Set the maximum mac learning counts of the port.
 * Input:
 *      port    - port id
 *      macCnt  - maximum mac learning counts
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID             - invalid port id
 *      RT_ERR_LIMITED_L2ENTRY_NUM - invalid limited L2 entry number
 * Note:
 *      (1) The maximum mac learning counts only limit for dynamic learning mac
 *          address, not apply to static mac address.
 *      (2) Set the macCnt to 0 mean disable learning in the port.
 */
int32
rt_l2_portLimitLearningCnt_set(rt_port_t port, int32 macCnt)
{
    rtdrv_rt_l2Cfg_t rt_l2_cfg;
    memset(&rt_l2_cfg, 0x0, sizeof(rtdrv_rt_l2Cfg_t));

    /* function body */
    osal_memcpy(&rt_l2_cfg.port, &port, sizeof(rt_port_t));
    osal_memcpy(&rt_l2_cfg.macCnt, &macCnt, sizeof(int32));
    SETSOCKOPT(RTDRV_RT_L2_PORTLIMITLEARNINGCNT_SET, &rt_l2_cfg, rtdrv_rt_l2Cfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_l2_portLimitLearningCnt_set */


/* Function Name:
 *      rt_l2_portLimitLearningCntAction_get
 * Description:
 *      Get the action when exceed the maximum mac learning counts of the port.
 * Input:
 *      port     - port id
 * Output:
 *      pLearnCntAc - mac learning exceed action
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Action support forward/drop/trap
 */
int32
rt_l2_portLimitLearningCntAction_get(rt_port_t port, rt_l2_limitLearnCntAction_t *pLearnCntAct)
{
    rtdrv_rt_l2Cfg_t rt_l2_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pLearnCntAct), RT_ERR_NULL_POINTER);

    /* function body */
    memset(&rt_l2_cfg, 0x0, sizeof(rtdrv_rt_l2Cfg_t));
    osal_memcpy(&rt_l2_cfg.port, &port, sizeof(rt_port_t));
    GETSOCKOPT(RTDRV_RT_L2_PORTLIMITLEARNINGCNTACTION_GET, &rt_l2_cfg, rtdrv_rt_l2Cfg_t, 1);
    osal_memcpy(pLearnCntAct, &rt_l2_cfg.learnCntAct, sizeof(rt_l2_limitLearnCntAction_t));

    return RT_ERR_OK;
}   /* end of rt_l2_portLimitLearningCntAction_get */


/* Function Name:
 *      rt_l2_portLimitLearningCntAction_set
 * Description:
 *      Set the action when exceed the maximum mac learning counts of the port.
 * Input:
 *      port    - port id
 *      learnCntAc - mac learning exceed action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                  - OK
 *      RT_ERR_FAILED              - Failed
 *      RT_ERR_PORT_ID             - invalid port id
 * Note:
 *      Action support forward/drop/trap
 */
int32
rt_l2_portLimitLearningCntAction_set(rt_port_t port, rt_l2_limitLearnCntAction_t learnCntAct)
{
    rtdrv_rt_l2Cfg_t rt_l2_cfg;

    /* function body */
    memset(&rt_l2_cfg, 0x0, sizeof(rtdrv_rt_l2Cfg_t));
    osal_memcpy(&rt_l2_cfg.port, &port, sizeof(rt_port_t));
    osal_memcpy(&rt_l2_cfg.learnCntAct, &learnCntAct, sizeof(rt_l2_limitLearnCntAction_t));
    SETSOCKOPT(RTDRV_RT_L2_PORTLIMITLEARNINGCNTACTION_SET, &rt_l2_cfg, rtdrv_rt_l2Cfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_l2_portLimitLearningCntAction_set */

/* Function Name:
 *      rt_l2_newMacOp_get
 * Description:
 *      Get learning mode and forwarding action of new learned address on specified port.
 * Input:
 *      port       - port id
 * Output:
 *      pLrnMode   - pointer to learning mode
 *      pFwdAction - pointer to forwarding action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 */
int32
rt_l2_newMacOp_get(rt_port_t port,rt_action_t *pFwdAction)
{
    rtdrv_rt_l2Cfg_t rt_l2_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pFwdAction), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_l2_cfg.port, &port, sizeof(rt_port_t));
    GETSOCKOPT(RTDRV_RT_L2_NEWMACOP_GET, &rt_l2_cfg, rtdrv_rt_l2Cfg_t, 1);
    osal_memcpy(pFwdAction, &rt_l2_cfg.fwdAction, sizeof(rt_action_t));

    return RT_ERR_OK;
}

/* Function Name:
 *      rt_l2_newMacOp_set
 * Description:
 *      Set learning mode and forwarding action of new learned address on specified port.
 * Input:
 *      port      - port id
 *      lrnMode   - learning mode
 *      fwdAction - forwarding action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_PORT_ID    - invalid port id
 *      RT_ERR_INPUT      - invalid input parameter
 *      RT_ERR_FWD_ACTION - invalid forwarding action
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 */
int32
rt_l2_newMacOp_set(rt_port_t port,rt_action_t fwdAction)
{
    rtdrv_rt_l2Cfg_t rt_l2_cfg;
    memset(&rt_l2_cfg, 0x0, sizeof(rtdrv_rt_l2Cfg_t));

    /* function body */
    osal_memcpy(&rt_l2_cfg.port, &port, sizeof(rt_port_t));
    osal_memcpy(&rt_l2_cfg.fwdAction, &fwdAction, sizeof(rt_action_t));
    SETSOCKOPT(RTDRV_RT_L2_NEWMACOP_SET, &rt_l2_cfg, rtdrv_rt_l2Cfg_t, 1);

    return RT_ERR_OK;
}

/* Function Name:
 *      rt_l2_addr_add
 * Description:
 *      Add L2 entry to ASIC.
 * Input:
 *      pL2_addr - L2 entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_MAC          - invalid mac address
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      Need to initialize L2 entry before add it.
 */
int32
rt_l2_addr_add(rt_l2_ucastAddr_t *pL2Addr)
{
    rtdrv_rt_l2Cfg_t rt_l2_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pL2Addr), RT_ERR_NULL_POINTER);
    memset(&rt_l2_cfg, 0x0, sizeof(rtdrv_rt_l2Cfg_t));

    /* function body */
    osal_memcpy(&rt_l2_cfg.l2UcastData, pL2Addr, sizeof(rt_l2_ucastAddr_t));
    SETSOCKOPT(RTDRV_RT_L2_ADDR_ADD, &rt_l2_cfg, rtdrv_rt_l2Cfg_t, 1);

    return RT_ERR_OK;
}

/* Function Name:
 *      rt_l2_addr_del
 * Description:
 *      Delete a L2 unicast address entry.
 * Input:
 *      pL2Addr  - L2 entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_MAC               - invalid mac address
 *      RT_ERR_L2_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      If the mac has existed in the LUT, it will be deleted. Otherwise, it will return RT_ERR_L2_ENTRY_NOTFOUND.
 */
int32
rt_l2_addr_del(rt_l2_ucastAddr_t *pL2Addr)
{
    rtdrv_rt_l2Cfg_t rt_l2_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pL2Addr), RT_ERR_NULL_POINTER);
    memset(&rt_l2_cfg, 0x0, sizeof(rtdrv_rt_l2Cfg_t));

    /* function body */
    osal_memcpy(&rt_l2_cfg.l2UcastData, pL2Addr, sizeof(rt_l2_ucastAddr_t));
    SETSOCKOPT(RTDRV_RT_L2_ADDR_DEL, &rt_l2_cfg, rtdrv_rt_l2Cfg_t, 1);

    return RT_ERR_OK;
}

/* Function Name:
 *      rt_l2_addr_get
 * Description:
 *      Get L2 entry based on MAC address
 * Input:
 *      None
 * Output:
 *      pL2Addr - pointer to L2 entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_MAC               - invalid mac address
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 *      RT_ERR_L2_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      If the unicast mac address existed in LUT, it will return the port and fid where
 *      the mac is learned. Otherwise, it will return a RT_ERR_L2_ENTRY_NOTFOUND error.
 */
int32
rt_l2_addr_get(rt_l2_ucastAddr_t *pL2Addr)
{
    rtdrv_rt_l2Cfg_t rt_l2_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pL2Addr), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_l2_cfg.l2UcastData, pL2Addr, sizeof(rt_l2_ucastAddr_t));
    GETSOCKOPT(RTDRV_RT_L2_ADDR_GET, &rt_l2_cfg, rtdrv_rt_l2Cfg_t, 1);
    osal_memcpy(pL2Addr, &rt_l2_cfg.l2UcastData, sizeof(rt_l2_ucastAddr_t));

    return RT_ERR_OK;
}

/* Function Name:
 *      rt_l2_mcastAddr_add
 * Description:
 *      Add L2 mcast entry to ASIC.
 * Input:
 *      pMcastAddr - L2 entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_MAC              - invalid mac address
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_ENTRY_FULL       - entry is full 
 * Note:
 *      Need to initialize L2 entry before add it.
 */
int32
rt_l2_mcastAddr_add(rt_l2_mcastAddr_t *pMcastAddr)
{
    rtdrv_rt_l2Cfg_t rt_l2_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pMcastAddr), RT_ERR_NULL_POINTER);
    memset(&rt_l2_cfg, 0x0, sizeof(rtdrv_rt_l2Cfg_t));

    /* function body */
    osal_memcpy(&rt_l2_cfg.mcastAddr, pMcastAddr, sizeof(rt_l2_mcastAddr_t));
    SETSOCKOPT(RTDRV_RT_L2_MCASTADDR_ADD, &rt_l2_cfg, rtdrv_rt_l2Cfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_l2_mcastAddr_add */

/* Function Name:
 *      rt_l2_mcastAddr_del
 * Description:
 *      Delete a L2 multicast address entry.
 * Input:
 *      pMcastAddr  - L2 entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                - OK
 *      RT_ERR_FAILED            - Failed
 *      RT_ERR_MAC               - invalid mac address
 *      RT_ERR_L2_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      If the mac has existed in the LUT, it will be deleted. Otherwise, it will return RT_ERR_L2_ENTRY_NOTFOUND.
 */
int32
rt_l2_mcastAddr_del(rt_l2_mcastAddr_t *pMcastAddr)
{
    rtdrv_rt_l2Cfg_t rt_l2_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pMcastAddr), RT_ERR_NULL_POINTER);
    memset(&rt_l2_cfg, 0x0, sizeof(rtdrv_rt_l2Cfg_t));

    /* function body */
    osal_memcpy(&rt_l2_cfg.mcastAddr, pMcastAddr, sizeof(rt_l2_mcastAddr_t));
    SETSOCKOPT(RTDRV_RT_L2_MCASTADDR_DEL, &rt_l2_cfg, rtdrv_rt_l2Cfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_l2_mcastAddr_del */

/* Function Name:
 *      rt_l2_mcastAddr_get
 * Description:
 *      Get L2 mcast entry based on MAC address
 * Input:
 *      None
 * Output:
 *      pMcastAddr - pointer to L2 entry
 * Return:
 *      RT_ERR_OK                - OK
 *      RT_ERR_FAILED            - Failed
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_MAC               - invalid mac address
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 *      RT_ERR_L2_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      If the multicast mac address existed in LUT, it will return the port and vid where
 *      the mac is learned. Otherwise, it will return a RT_ERR_L2_ENTRY_NOTFOUND error.
 */
int32
rt_l2_mcastAddr_get(rt_l2_mcastAddr_t *pMcastAddr)
{
    rtdrv_rt_l2Cfg_t rt_l2_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pMcastAddr), RT_ERR_NULL_POINTER);
    memset(&rt_l2_cfg, 0x0, sizeof(rtdrv_rt_l2Cfg_t));

    /* function body */

    osal_memcpy(&rt_l2_cfg.mcastAddr, pMcastAddr, sizeof(rt_l2_mcastAddr_t));
    GETSOCKOPT(RTDRV_RT_L2_MCASTADDR_GET, &rt_l2_cfg, rtdrv_rt_l2Cfg_t, 1);
    osal_memcpy(pMcastAddr, &rt_l2_cfg.mcastAddr, sizeof(rt_l2_mcastAddr_t));

    return RT_ERR_OK;
}   /* end of rt_l2_mcastAddr_get */

/* Function Name:
 *      rt_l2_nextValidAddr_get
 * Description:
 *      Get next valid L2 unicast address entry from the specified device.
 * Input:
 *      pScanIdx       - currently scan index of l2 table to get next.
 * Output:
 *      pL2Addr        - structure of l2 address data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_VLAN_VID          - invalid vid
 *      RT_ERR_MAC               - invalid mac address
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 *      RT_ERR_L2_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      (1) The function will skip valid l2 multicast and ip multicast entry and
 *          reply next valid L2 unicast address is based on index order of l2 table.
 *      (2) Please input 0 for get the first entry of l2 table.
 *      (3) The pScanIdx is the input and also is the output argument.
 */
int32
rt_l2_nextValidAddr_get(
    int32               *pScanIdx,
    rt_l2_ucastAddr_t   *pL2Addr)
{
    rtdrv_rt_l2Cfg_t rt_l2_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pScanIdx), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pL2Addr), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_l2_cfg.scanIdx, pScanIdx, sizeof(int32));
    GETSOCKOPT(RTDRV_RT_L2_NEXTVALIDADDR_GET, &rt_l2_cfg, rtdrv_rt_l2Cfg_t, 1);
    osal_memcpy(pScanIdx, &rt_l2_cfg.scanIdx, sizeof(int32));
    osal_memcpy(pL2Addr, &rt_l2_cfg.l2UcastData, sizeof(rt_l2_ucastAddr_t));

    return RT_ERR_OK;
}   /* end of rt_l2_nextValidAddr_get */


/* Function Name:
 *      rt_l2_nextValidAddrOnPort_get
 * Description:
 *      Get next valid L2 unicast address entry from specify port.
 * Input:
 *      pScanIdx      - currently scan index of l2 table to get next.
 * Output:
 *      pL2Addr       - structure of l2 address data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_VLAN_VID          - invalid vid
 *      RT_ERR_MAC               - invalid mac address
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 *      RT_ERR_L2_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      (1) The function will skip valid l2 multicast and ip multicast entry and
 *          reply next valid L2 unicast address is based on index order of l2 table.
 *      (2) Please input 0 for get the first entry of l2 table.
 *      (3) The pScanIdx is the input and also is the output argument.
 */
int32
rt_l2_nextValidAddrOnPort_get(
    rt_port_t           port,
    int32               *pScanIdx,
    rt_l2_ucastAddr_t   *pL2Addr)
{
    rtdrv_rt_l2Cfg_t rt_l2_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pScanIdx), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pL2Addr), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_l2_cfg.port, &port, sizeof(rt_port_t));
    osal_memcpy(&rt_l2_cfg.scanIdx, pScanIdx, sizeof(int32));
    GETSOCKOPT(RTDRV_RT_L2_NEXTVALIDADDRONPORT_GET, &rt_l2_cfg, rtdrv_rt_l2Cfg_t, 1);
    osal_memcpy(pScanIdx, &rt_l2_cfg.scanIdx, sizeof(int32));
    osal_memcpy(pL2Addr, &rt_l2_cfg.l2UcastData, sizeof(rt_l2_ucastAddr_t));

    return RT_ERR_OK;
}   /* end of rt_l2_nextValidAddrOnPort_get */

/* Function Name:
 *      rt_l2_illegalPortMoveAction_get
 * Description:
 *      Get forwarding action when illegal port moving happen on specified port.
 * Input:
 *      port       - port id
 * Output:
 *      pFwdAction - pointer to forwarding action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_COPY2CPU
 */
int32
rt_l2_illegalPortMoveAction_get(rt_port_t port,rt_action_t *pFwdAction)
{
    rtdrv_rt_l2Cfg_t rt_l2_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pFwdAction), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&rt_l2_cfg.port, &port, sizeof(rt_port_t));
    GETSOCKOPT(RTDRV_RT_L2_ILLEGALPORTMOVEACTION_GET, &rt_l2_cfg, rtdrv_rt_l2Cfg_t, 1);
    osal_memcpy(pFwdAction, &rt_l2_cfg.fwdAction, sizeof(rt_action_t));

    return RT_ERR_OK;
}   /* end of rt_l2_illegalPortMoveAction_get */

/* Function Name:
 *      rt_l2_illegalPortMoveAction_set
 * Description:
 *      Set forwarding action when illegal port moving happen on specified port.
 * Input:
 *      port      - port id
 *      fwdAction - forwarding action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_PORT_ID    - invalid port id
 *      RT_ERR_FWD_ACTION - invalid forwarding action
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_COPY2CPU
 */
int32
rt_l2_illegalPortMoveAction_set(rt_port_t port,rt_action_t fwdAction)
{
    rtdrv_rt_l2Cfg_t rt_l2_cfg;
    memset(&rt_l2_cfg, 0x0, sizeof(rtdrv_rt_l2Cfg_t));

    /* function body */
    osal_memcpy(&rt_l2_cfg.port, &port, sizeof(rt_port_t));
    osal_memcpy(&rt_l2_cfg.fwdAction, &fwdAction, sizeof(rt_action_t));
    SETSOCKOPT(RTDRV_RT_L2_ILLEGALPORTMOVEACTION_SET, &rt_l2_cfg, rtdrv_rt_l2Cfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_l2_illegalPortMoveAction_set */

/* Function Name:
 *      rt_l2_ageTime_get
 * Description:
 *      Get L2 age time
 * Input:
 *      None
 * Output:
 *      pAgeTime  - age time
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      
 */
int32
rt_l2_ageTime_get(uint32 *pAgeTime)
{
    rtdrv_rt_l2Cfg_t rt_l2_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pAgeTime), RT_ERR_NULL_POINTER);

    /* function body */
    GETSOCKOPT(RTDRV_RT_L2_AGETIME_GET, &rt_l2_cfg, rtdrv_rt_l2Cfg_t, 1);
    osal_memcpy(pAgeTime, &rt_l2_cfg.ageTime, sizeof(uint32));

    return RT_ERR_OK;

} /* end of rt_l2_ageTime_get */


/* Function Name:
 *      rt_l2_ageTime_set
 * Description:
 *      Set L2 age time.
 * Input:
 *      ageTime  -ageTime
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      
 */
int32
rt_l2_ageTime_set(uint32 ageTime)
{
    rtdrv_rt_l2Cfg_t rt_l2_cfg;
    memset(&rt_l2_cfg, 0x0, sizeof(rtdrv_rt_l2Cfg_t));

    /* function body */
    osal_memcpy(&rt_l2_cfg.ageTime, &ageTime, sizeof(uint32));
    SETSOCKOPT(RTDRV_RT_L2_AGETIME_SET, &rt_l2_cfg, rtdrv_rt_l2Cfg_t, 1);

    return RT_ERR_OK;


} /* end of rt_l2_ageTime_set */

/* Function Name:
 *      rt_l2_ivlSvl_get
 * Description:
 *      Get L2 IVL SVL mode
 * Input:
 *      None
 * Output:
 *      pIvlEn  - IVL enable
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      
 */
int32
rt_l2_ivlSvl_get(rt_enable_t *pIvlEn)
{

    rtdrv_rt_l2Cfg_t rt_l2_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pIvlEn), RT_ERR_NULL_POINTER);

    /* function body */
    GETSOCKOPT(RTDRV_RT_L2_IVLSVL_GET, &rt_l2_cfg, rtdrv_rt_l2Cfg_t, 1);
    osal_memcpy(pIvlEn, &rt_l2_cfg.ivlEn, sizeof(rt_enable_t));

    return RT_ERR_OK;

} /* end of rt_l2_ivlSvl_get */


/* Function Name:
 *      rt_l2_ivlSvl_set
 * Description:
 *      Set L2 IVL SVL mode
 * Input:
 *      ivlEn  - IVL enable
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *
 */
int32
rt_l2_ivlSvl_set(rt_enable_t ivlEn)
{

    rtdrv_rt_l2Cfg_t rt_l2_cfg;
    memset(&rt_l2_cfg, 0x0, sizeof(rtdrv_rt_l2Cfg_t));

    /* function body */
    osal_memcpy(&rt_l2_cfg.ivlEn, &ivlEn, sizeof(rt_enable_t));
    SETSOCKOPT(RTDRV_RT_L2_IVLSVL_SET, &rt_l2_cfg, rtdrv_rt_l2Cfg_t, 1);

    return RT_ERR_OK;


} /* end of rt_l2_ivlSvl_set */

/* Function Name:
 *      rt_l2_fwdGroupId_set
 * Description:
 *      Set l2 forward group action mask
 * Input:
 *      idx   - forward group index
 *      mask  - forward group mask
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *
 */
int32
rt_l2_fwdGroupId_set(uint8 idx, uint64 mask)
{
    rtdrv_rt_l2Cfg_t rt_l2_cfg;
    memset(&rt_l2_cfg, 0x0, sizeof(rtdrv_rt_l2Cfg_t));

    /* function body */
    osal_memcpy(&rt_l2_cfg.idx, &idx, sizeof(uint8));
    osal_memcpy(&rt_l2_cfg.mask, &mask, sizeof(uint64));
    SETSOCKOPT(RTDRV_RT_L2_FWDGROUPID_SET, &rt_l2_cfg, rtdrv_rt_l2Cfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_l2_fwdGroupId_set */

/* Function Name:
 *      rt_l2_fwdGroupId_get
 * Description:
 *      Get l2 forward group action mask
 * Input:
 *      idx   - forward group index
 * Output:
 *      pMask  - forward group mask
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *
 */
int32
rt_l2_fwdGroupId_get(uint8 idx, uint64 *pMask)
{
    rtdrv_rt_l2Cfg_t rt_l2_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pMask), RT_ERR_NULL_POINTER);
    memset(&rt_l2_cfg, 0x0, sizeof(rtdrv_rt_l2Cfg_t));

    /* function body */
    osal_memcpy(&rt_l2_cfg.idx, &idx, sizeof(uint8));
    GETSOCKOPT(RTDRV_RT_L2_FWDGROUPID_GET, &rt_l2_cfg, rtdrv_rt_l2Cfg_t, 1);
    osal_memcpy(pMask, &rt_l2_cfg.mask, sizeof(uint64));

    return RT_ERR_OK;
}   /* end of rt_l2_fwdGroupId_get */

/* Function Name:
 *      rt_l2_fwdGroupAction_set
 * Description:
 *      Set forward group action
 * Input:
 *      idx     - forward group action index
 *      portAct - port forward action
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *
 */
int32
rt_l2_fwdGroupAction_set(uint8 idx, rt_l2_portAction_t portAct)
{
    rtdrv_rt_l2Cfg_t rt_l2_cfg;
    memset(&rt_l2_cfg, 0x0, sizeof(rtdrv_rt_l2Cfg_t));

    /* function body */
    osal_memcpy(&rt_l2_cfg.idx, &idx, sizeof(uint8));
    osal_memcpy(&rt_l2_cfg.portAct, &portAct, sizeof(rt_l2_portAction_t));
    SETSOCKOPT(RTDRV_RT_L2_FWDGROUPACTION_SET, &rt_l2_cfg, rtdrv_rt_l2Cfg_t, 1);

    return RT_ERR_OK;
}   /* end of rt_l2_fwdGroupAction_set */

/* Function Name:
 *      rt_l2_fwdGroupAction_get
 * Description:
 *      Get forward group action
 * Input:
 *      idx     - forward group action index
 * Output:
 *      pPortAct - port forward action
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *
 */
int32
rt_l2_fwdGroupAction_get(uint8 idx, rt_l2_portAction_t *pPortAct)
{
    rtdrv_rt_l2Cfg_t rt_l2_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortAct), RT_ERR_NULL_POINTER);
    memset(&rt_l2_cfg, 0x0, sizeof(rtdrv_rt_l2Cfg_t));

    /* function body */
    osal_memcpy(&rt_l2_cfg.idx, &idx, sizeof(uint8));
    GETSOCKOPT(RTDRV_RT_L2_FWDGROUPACTION_GET, &rt_l2_cfg, rtdrv_rt_l2Cfg_t, 1);
    osal_memcpy(pPortAct, &rt_l2_cfg.portAct, sizeof(rt_l2_portAction_t));

    return RT_ERR_OK;
}   /* end of rt_l2_fwdGroupAction_get */
