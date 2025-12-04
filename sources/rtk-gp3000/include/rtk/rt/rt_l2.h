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
 *           (12) Age time
 *           (13) IVL/SVL mode
 *
 */

#ifndef __RT_L2_H__
#define __RT_L2_H__


/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/rt/rt_port.h>

/*
 * Symbol Definition
 */
 
typedef enum rt_l2_vid_action_e{
    RT_L2_VID_ACTION_NOP,
    RT_L2_VID_ACTION_DELETE,
    RT_L2_VID_ACTION_MODIFY,
    RT_L2_VID_ACTION_END
} rt_l2_vid_action_t;

typedef enum rt_l2_pri_action_e{
    RT_L2_PRI_ACTION_NOP,
    RT_L2_PRI_ACTION_MODIFY,
    RT_L2_PRI_ACTION_END
} rt_l2_pri_action_t;

typedef enum rt_l2_limitLearnCntAction_e
{
    RT_L2_LIMIT_LEARN_CNT_ACTION_DROP = ACTION_DROP,
    RT_L2_LIMIT_LEARN_CNT_ACTION_FORWARD = ACTION_FORWARD,
    RT_L2_LIMIT_LEARN_CNT_ACTION_TRAP = ACTION_TRAP2CPU,
    RT_L2_LIMIT_LEARN_CNT_ACTION_END
} rt_l2_limitLearnCntAction_t;

/* l2 address table - unicast data structure */
typedef struct rt_l2_ucastAddr_s
{
    rt_mac_t       mac;
    rt_port_t      port;
    uint32         age;
    rt_enable_t    staticFlag;
    rt_enable_t    filterFlag;
    uint32         vid;
}rt_l2_ucastAddr_t;

typedef struct rt_l2_mcastAddr_s
{
    rt_mac_t       mac;
    uint32         group;
    uint32         vid;
}rt_l2_mcastAddr_t;

typedef struct rt_l2_portAction_s
{
    rt_port_t           port;
    rt_l2_vid_action_t  vidAct;
    uint16              vid;
    rt_l2_pri_action_t  priAct;
    uint8               pri;
}rt_l2_portAction_t;

/*
 * Data Declaration
 */


/*
 * Macro Declaration
 */


/*
 * Function Declaration
 */

/* Module Name    : L2     */
/* Sub-module Name: Global */

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
extern int32
rt_l2_init(void);

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
extern int32
rt_l2_portLimitLearningCnt_get(rt_port_t port, int32 *pMacCnt);


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
extern int32
rt_l2_portLimitLearningCnt_set(rt_port_t port, int32 macCnt);


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
extern int32
rt_l2_portLimitLearningCntAction_get(rt_port_t port, rt_l2_limitLearnCntAction_t *pLearnCntAct);


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
extern int32
rt_l2_portLimitLearningCntAction_set(rt_port_t port, rt_l2_limitLearnCntAction_t learnCntAct);


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
extern int32
rt_l2_newMacOp_get(rt_port_t port,rt_action_t *pFwdAction);

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
extern int32
rt_l2_newMacOp_set(rt_port_t port,rt_action_t fwdAction);

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
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_MAC              - invalid mac address
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_ENTRY_FULL       - entry is full 
 * Note:
 *      Need to initialize L2 entry before add it.
 */
extern int32
rt_l2_addr_add(rt_l2_ucastAddr_t *pL2Addr);

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
extern int32
rt_l2_addr_del(rt_l2_ucastAddr_t *pL2Addr);

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
extern int32
rt_l2_addr_get(rt_l2_ucastAddr_t *pL2Addr);

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
extern int32
rt_l2_mcastAddr_add(rt_l2_mcastAddr_t *pMcastAddr);

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
extern int32
rt_l2_mcastAddr_del(rt_l2_mcastAddr_t *pMcastAddr);

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
extern int32
rt_l2_mcastAddr_get(rt_l2_mcastAddr_t *pMcastAddr);

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
extern int32
rt_l2_nextValidAddr_get(
    int32               *pScanIdx,
    rt_l2_ucastAddr_t   *pL2Addr);


/* Function Name:
 *      rt_l2_nextValidAddrOnPort_get
 * Description:
 *      Get next valid L2 unicast address entry from specify port.
 * Input:
 *      port          - speific port
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
extern int32
rt_l2_nextValidAddrOnPort_get(
    rt_port_t           port,
    int32               *pScanIdx,
    rt_l2_ucastAddr_t   *pL2Addr);

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
extern int32
rt_l2_illegalPortMoveAction_get(rt_port_t port,rt_action_t *pFwdAction);

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
extern int32
rt_l2_illegalPortMoveAction_set(rt_port_t port,rt_action_t fwdAction);

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
extern int32
rt_l2_ageTime_get(uint32 *pAgeTime);


/* Function Name:
 *      rt_l2_ageTime_set
 * Description:
 *      Set L2 age time.
 * Input:
 *      ageTime  -age time
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      
 */
extern int32
rt_l2_ageTime_set(uint32 ageTime);

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
extern int32
rt_l2_ivlSvl_get(rt_enable_t *pIvlEn);


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
extern int32
rt_l2_ivlSvl_set(rt_enable_t ivlEn);

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
extern int32
rt_l2_fwdGroupId_set(uint8 idx, uint64 mask);

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
extern int32
rt_l2_fwdGroupId_get(uint8 idx, uint64 *pMask);

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
extern int32
rt_l2_fwdGroupAction_set(uint8 idx, rt_l2_portAction_t portAct);

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
extern int32
rt_l2_fwdGroupAction_get(uint8 idx, rt_l2_portAction_t *pPortAct);

#endif /* __RT_L2_H__ */

