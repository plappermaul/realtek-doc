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
 * Purpose : Definition those public Trap APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *            1) Packets trap to CPU setting.
 *            2) RMA (Reserved MAC address).
 *
 */
#ifndef __DAL_MAPLE_TRAP_H__
#define __DAL_MAPLE_TRAP_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <rtk/trap.h>
#include <dal/dal_mgmt.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */

/*
 * Macro Definition
 */

/*
 * Function Declaration
 */

/* Function Name:
 *      dal_maple_trapMapper_init
 * Description:
 *      Hook trap module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must Hook trap module before calling any trap APIs.
 */
extern int32
dal_maple_trapMapper_init(dal_mapper_t *pMapper);

/* Function Name:
 *      dal_maple_trap_init
 * Description:
 *      Initial the trap module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
extern int32
dal_maple_trap_init(uint32 unit);

/* Function Name:
 *      dal_maple_trap_rmaGroupAction_get
 * Description:
 *      Get action of reserved multicast address(RMA) frame in group.
 * Input:
 *      unit               - unit id
 *      rmaGroup_frameType - Reserved multicast address type.
 * Output:
 *      pRma_action        - RMA action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The supported Reserved Multicast Address action:
 *      - MGMT_ACTION_FORWARD
 *      - MGMT_ACTION_DROP
 *      - MGMT_ACTION_TRAP2CPU
 *      - MGMT_ACTION_FLOOD_TO_ALL_PORT
 */
extern int32
dal_maple_trap_rmaGroupAction_get(uint32 unit, rtk_trap_rmaGroup_frameType_t rmaGroup_frameType, rtk_mgmt_action_t *pRma_action);

/* Function Name:
 *      dal_maple_trap_rmaGroupAction_set
 * Description:
 *      Set action of reserved multicast address(RMA) frame.
 * Input:
 *      unit               - unit id
 *      rmaGroup_frameType - Reserved multicast address type.
 *      rma_action         - RMA action
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_INPUT      - Invalid input parameter
 *      RT_ERR_RMA_ACTION - Invalid RMA action
 * Note:
 *      The supported Reserved Multicast Address frame:
 *      Assignment                      Address
 *      RMA_SLOW_PROTOCOL_OAM           01-80-C2-00-00-02 & ethertype=0x8809 & subtype = 0x3
 *      RMA_SLOW_PROTOCOL_OTHER         01-80-C2-00-00-02 except OAM
 *      RMA_0E_EXCEPT_PTP_LLDP          01-80-C2-00-00-0E
 *      RMA_GMRP (GMRP Address)         01-80-C2-00-00-20
 *      RMA_GVRP (GVRP address)         01-80-C2-00-00-21
 *      RMA_MSRP                        01-80-C2-00-00-22
 *      RMA_0X                          01-80-C2-00-00-01~01-80-C2-00-00-0F, excluding listed above
 *      RMA_1X                          01-80-C2-00-00-01~10-80-C2-00-00-1F, excluding listed above
 *      RMA_2X                          01-80-C2-00-00-01~23-80-C2-00-00-2F, excluding listed above
 *
 *      The supported Reserved Multicast Address action:
 *      - MGMT_ACTION_FORWARD
 *      - MGMT_ACTION_DROP
 *      - MGMT_ACTION_TRAP2CPU
 *      - MGMT_ACTION_FLOOD_TO_ALL_PORT
 */
extern int32
dal_maple_trap_rmaGroupAction_set(uint32 unit, rtk_trap_rmaGroup_frameType_t rmaGroup_frameType, rtk_mgmt_action_t rma_action);

/* Function Name:
 *      dal_maple_trap_rmaGroupLearningEnable_get
 * Description:
 *      Get enable status of SMAC learning for this RMA frame.
 * Input:
 *      unit               - unit id
 *      rmaGroup_frameType - Reserved multicast address type.
 * Output:
 *      pEnable            - pointer to enable status of SMAC learning
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_maple_trap_rmaGroupLearningEnable_get(uint32 unit, rtk_trap_rmaGroup_frameType_t  rmaGroup_frameType, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_maple_trap_rmaGroupLearningEnable_set
 * Description:
 *      Set enable status of SMAC learning for this RMA frame.
 * Input:
 *      unit               - unit id
 *      rmaGroup_frameType - Reserved multicast address type.
 *      enable             - enable status of SMAC learning
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_maple_trap_rmaGroupLearningEnable_set(uint32 unit, rtk_trap_rmaGroup_frameType_t  rmaGroup_frameType, rtk_enable_t enable);

/* Function Name:
 *      dal_maple_trap_bypassStp_get
 * Description:
 *      Get enable status of bypassing spanning tree for specified frame type
 * Input:
 *      unit      - unit id
 *      frameType - frame type
 * Output:
 *      pEnable   - pointer to enable status of bypassing STP
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The frame type selection is as following:
 *      - BYPASS_STP_TYPE_USER_DEF_0
 *      - BYPASS_STP_TYPE_USER_DEF_1
 *      - BYPASS_STP_TYPE_USER_DEF_2
 *      - BYPASS_STP_TYPE_USER_DEF_3
 *      - BYPASS_STP_TYPE_USER_DEF_4
 *      - BYPASS_STP_TYPE_USER_DEF_5
 *      - BYPASS_STP_TYPE_SLOW_PROTO
 *      - BYPASS_STP_TYPE_RMA_03
 *      - BYPASS_STP_TYPE_RMA_0X
 *      - BYPASS_STP_TYPE_EAPOL
 *      - BYPASS_STP_TYPE_PTP
 *      - BYPASS_STP_TYPE_LLDP
 */
extern int32
dal_maple_trap_bypassStp_get(uint32 unit, rtk_trap_bypassStpType_t frameType, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_maple_trap_bypassStp_set
 * Description:
 *      Set enable status of bypassing spanning tree for specified frame type
 * Input:
 *      unit      - unit id
 *      frameType - frame type
 *      enable    - enable status of bypassing STP
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      The frame type selection is as following:
 *      - BYPASS_STP_TYPE_USER_DEF_0
 *      - BYPASS_STP_TYPE_USER_DEF_1
 *      - BYPASS_STP_TYPE_USER_DEF_2
 *      - BYPASS_STP_TYPE_USER_DEF_3
 *      - BYPASS_STP_TYPE_USER_DEF_4
 *      - BYPASS_STP_TYPE_USER_DEF_5
 *      - BYPASS_STP_TYPE_RMA_03
 *      - BYPASS_STP_TYPE_RMA_0X
 *      - BYPASS_STP_TYPE_SLOW_PROTO
 *      - BYPASS_STP_TYPE_EAPOL
 *      - BYPASS_STP_TYPE_PTP
 *      - BYPASS_STP_TYPE_LLDP
 */
extern int32
dal_maple_trap_bypassStp_set(uint32 unit, rtk_trap_bypassStpType_t frameType, rtk_enable_t enable);

/* Function Name:
 *      dal_maple_trap_bypassVlan_get
 * Description:
 *      Get enable status of bypassing VLAN drop for specified frame type
 * Input:
 *      unit      - unit id
 *      frameType - frame type
 * Output:
 *      pEnable   - pointer to enable status of bypassing  ingress fliter/accept frame type/vlan CFI=1
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The frame type selection is as following:
 *      - BYPASS_VLAN_TYPE_USER_DEF_0 = 0,
 *      - BYPASS_VLAN_TYPE_USER_DEF_1,
 *      - BYPASS_VLAN_TYPE_USER_DEF_2,
 *      - BYPASS_VLAN_TYPE_USER_DEF_3,
 *      - BYPASS_VLAN_TYPE_USER_DEF_4,
 *      - BYPASS_VLAN_TYPE_USER_DEF_5,
 *      - BYPASS_VLAN_TYPE_RMA_00,
 *      - BYPASS_VLAN_TYPE_RMA_02,
 *      - BYPASS_VLAN_TYPE_RMA_03,
 *      - BYPASS_VLAN_TYPE_RMA_0E,
 *      - BYPASS_VLAN_TYPE_RMA_0X,
 *      - BYPASS_VLAN_TYPE_OAM,
 *      - BYPASS_VLAN_TYPE_GMRP,
 *      - BYPASS_VLAN_TYPE_GVRP,
 *      - BYPASS_VLAN_TYPE_EAPOL,
 *      - BYPASS_VLAN_TYPE_PTP,
 *      - BYPASS_VLAN_TYPE_LLDP
 */
extern int32
dal_maple_trap_bypassVlan_get(uint32 unit, rtk_trap_bypassVlanType_t frameType, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_maple_trap_bypassVlan_set
 * Description:
 *      Set enable status of bypassing VLAN drop for specified frame type
 * Input:
 *      unit      - unit id
 *      frameType - frame type
 *      enable    - enable status of bypassing vlan ingress fliter/accept frame type/vlan CFI=1
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      The frame type selection is as following:
 *      - BYPASS_VLAN_TYPE_USER_DEF_0 = 0,
 *      - BYPASS_VLAN_TYPE_USER_DEF_1,
 *      - BYPASS_VLAN_TYPE_USER_DEF_2,
 *      - BYPASS_VLAN_TYPE_USER_DEF_3,
 *      - BYPASS_VLAN_TYPE_USER_DEF_4,
 *      - BYPASS_VLAN_TYPE_USER_DEF_5,
 *      - BYPASS_VLAN_TYPE_RMA_00,
 *      - BYPASS_VLAN_TYPE_RMA_02,
 *      - BYPASS_VLAN_TYPE_RMA_03,
 *      - BYPASS_VLAN_TYPE_RMA_0E,
 *      - BYPASS_VLAN_TYPE_RMA_0X,
 *      - BYPASS_VLAN_TYPE_OAM,
 *      - BYPASS_VLAN_TYPE_GMRP,
 *      - BYPASS_VLAN_TYPE_GVRP,
 *      - BYPASS_VLAN_TYPE_EAPOL,
 *      - BYPASS_VLAN_TYPE_PTP,
 *      - BYPASS_VLAN_TYPE_LLDP
 */
extern int32
dal_maple_trap_bypassVlan_set(uint32 unit, rtk_trap_bypassVlanType_t frameType, rtk_enable_t enable);

/* Function Name:
 *      dal_maple_trap_userDefineRma_get
 * Description:
 *      Get user defined RMA.
 * Input:
 *      unit            - unit id
 *      userDefine_idx  - index of user defined RMA entry
 * Output:
 *      pUserDefinedRma - pointer to content of user defined RMA
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - userDefine_idx is out of range
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_maple_trap_userDefineRma_get(
    uint32                      unit,
    uint32                      userDefine_idx,
    rtk_trap_userDefinedRma_t   *pUserDefinedRma);

/* Function Name:
 *      dal_maple_trap_userDefineRma_set
 * Description:
 *      Set user defined RMA.
 * Input:
 *      unit            - unit id
 *      userDefine_idx  - index of user defined RMA entry
 *      pUserDefinedRma - to content of user defined RMA
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - userDefine_idx is out of range
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_maple_trap_userDefineRma_set(
    uint32                      unit,
    uint32                      userDefine_idx,
    rtk_trap_userDefinedRma_t   *pUserDefinedRma);

/* Function Name:
 *      dal_maple_trap_userDefineRmaEnable_get
 * Description:
 *      Get enable status of user defined RMA.
 * Input:
 *      unit           - unit id
 *      userDefine_idx - index of user defined RMA entry
 * Output:
 *      pEnable        - pointer to enable status of RMA entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - userDefine_idx is out of range
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_maple_trap_userDefineRmaEnable_get(uint32 unit, uint32 userDefine_idx, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_maple_trap_userDefineRmaEnable_set
 * Description:
 *      Set enable status of user defined RMA.
 * Input:
 *      unit           - unit id
 *      userDefine_idx - index of user defined RMA entry
 *      enable         - enable status of RMA entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - userDefine_idx is out of range
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_maple_trap_userDefineRmaEnable_set(uint32 unit, uint32 userDefine_idx, rtk_enable_t enable);

/* Function Name:
 *      dal_maple_trap_userDefineRmaAction_get
 * Description:
 *      Get forwarding action of user defined RMA.
 * Input:
 *      unit           - unit id
 *      userDefine_idx - index of user defined RMA entry
 * Output:
 *      pActoin        - pointer to forwarding action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - userDefine_idx is out of range
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Forwarding action is as following:
 *      - ACTION_FORWARD
 *      - ACTION_TRAP2CPU
 *      - ACTION_DROP
 */
extern int32
dal_maple_trap_userDefineRmaAction_get(uint32 unit, uint32 userDefine_idx, rtk_mgmt_action_t *pAction);

/* Function Name:
 *      dal_maple_trap_userDefineRmaAction_set
 * Description:
 *      Set forwarding action of user defined RMA.
 * Input:
 *      unit           - unit id
 *      userDefine_idx - index of user defined RMA entry
 *      actoin         - forwarding action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - userDefine_idx is out of range
 *      RT_ERR_FWD_ACTION   - invalid forwarding action
 * Note:
 *      Forwarding action is as following:
 *      - ACTION_FORWARD
 *      - ACTION_TRAP2CPU
 *      - ACTION_DROP
 */
extern int32
dal_maple_trap_userDefineRmaAction_set(uint32 unit, uint32 userDefine_idx, rtk_mgmt_action_t action);

/* Function Name:
 *      dal_maple_trap_userDefineRmaLearningEnable_get
 * Description:
 *      Get enable status of SMAC learning for user-defined RMA frame.
 * Input:
 *      unit           - unit id
 *      userDefine_idx - index of user defined RMA entry
 * Output:
 *      pEnable        - pointer to enable status of SMAC learning
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - userDefine_idx is out of range
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_maple_trap_userDefineRmaLearningEnable_get(uint32 unit, uint32 userDefine_idx, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_maple_trap_userDefineRmaLearningEnable_set
 * Description:
 *      Set enable status of SMAC learning for this user-defined RMA frame.
 * Input:
 *      unit           - unit id
 *      userDefine_idx - index of user defined RMA entry
 *      enable         - enable status of SMAC learning
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - userDefine_idx is out of range
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_maple_trap_userDefineRmaLearningEnable_set(uint32 unit, uint32 userDefine_idx, rtk_enable_t enable);

/* Function Name:
 *      dal_maple_trap_mgmtFrameMgmtVlanEnable_get
 * Description:
 *      Get compare forwarding VID ability with PVID of CPU port to
 *      IPv6 neighbor discovery, ARP request and Switch MAC packets.
 * Input:
 *      unit    - unit id
 * Output:
 *      pEnable - status of trap to CPU
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The status of trap to CPU:
 *      - DISABLED
 *      - ENABLED
 */
extern int32
dal_maple_trap_mgmtFrameMgmtVlanEnable_get(uint32 unit, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_maple_trap_mgmtFrameMgmtVlanEnable_set
 * Description:
 *      Set compare forwarding VID ability with PVID of CPU port to
 *      IPv6 neighbor discovery, ARP request and Switch MAC packets.
 * Input:
 *      unit   - unit id
 *      enable - status of trap to CPU
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - Invalid input parameter
 * Note:
 *      The status of trap to CPU:
 *      - DISABLED
 *      - ENABLED
 */
extern int32
dal_maple_trap_mgmtFrameMgmtVlanEnable_set(uint32 unit, rtk_enable_t enable);


/* Function Name:
 *      dal_maple_trap_mgmtFrameSelfARPEnable_get
 * Description:
 *      Get state of copy Self-ARP to CPU.
 * Input:
 *      unit   - unit id
 * Output:
 *      pEnable - enable state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - Invalid input parameter
 * Note:
 *      (1) The configuration only applies to ARP request(MGMT_TYPE_ARP) packet.
 *      (2) All the ARP Request packets are copied to CPU by setting rtk_trap_mgmtFrameAction_set(MGMT_TYPE_ARP).
 *          But if the function is enabled, Only ARP Request destined to switch's IP (rtk_switch_IPv4Addr_set)
 *          is copied to CPU.
 */
extern int32
dal_maple_trap_mgmtFrameSelfARPEnable_get(uint32 unit, rtk_enable_t *pEnable);


/* Function Name:
 *      dal_maple_trap_mgmtFrameSelfARPEnable_set
 * Description:
 *      Set state of copy Self-ARP to CPU.
 * Input:
 *      unit   - unit id
 *      enable - enable state
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - Invalid input parameter
 * Note:
 *      (1) The configuration only applies to ARP request(MGMT_TYPE_ARP) packet.
 *      (2) All the ARP Request packets are copied to CPU by setting rtk_trap_mgmtFrameAction_set(MGMT_TYPE_ARP).
 *          But if the function is enabled, Only ARP Request destined to switch's IP (rtk_switch_IPv4Addr_set)
 *          is copied to CPU.
 */
extern int32
dal_maple_trap_mgmtFrameSelfARPEnable_set(uint32 unit, rtk_enable_t enable);


/* Function Name:
 *      dal_maple_trap_mgmtFrameAction_get
 * Description:
 *      Get forwarding action of management frame.
 * Input:
 *      unit      - unit id
 *      frameType - type of management frame
 * Output:
 *      pActoin   - pointer to forwarding action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT      - The module is not initial
 *      RT_ERR_RMA_MGMT_TYPE - invalid type of management frame
 *      RT_ERR_NULL_POINTER  - input parameter may be null pointer
 * Note:
 *      (1) Forwarding action is as following:
 *          - MGMT_ACTION_FORWARD
 *          - MGMT_ACTION_DROP
 *          - MGMT_ACTION_TRAP2CPU
 *          - MGMT_ACTION_COPY2CPU
 *          - MGMT_ACTION_TRAP2MASTERCPU
 *          - MGMT_ACTION_COPY2MASTERCPU
 *          - MGMT_ACTION_FLOOD_TO_ALL_PORT
 *
 *      (2) To show the suppported action of each management frame in each chip,
 *          we use notation to represent. For example, BPDU supports MGMT_ACTION_FORWARD,
 *          MGMT_ACTION_DROP, MGMT_ACTION_TRAP2CPU, MGMT_ACTION_TRAP2MASTERCPU and
 *          MGMT_ACTION_FLOOD_TO_ALL_PORT in 9310, we use (1110101) to represent.
 *
 *          Supported management frame and action are as following:
 *                                    	      8380
 *          - MGMT_TYPE_ARP                 (1001000)
 *          - MGMT_TYPE_MLD                 (1010000)
 *          - MGMT_TYPE_IGMP                (1010000)
 *          - MGMT_TYPE_EAPOL               (1010000)
 *          - MGMT_TYPE_IPV6ND              (1001000)
 *          - MGMT_TYPE_SELFMAC             (1110000)
 *          - MGMT_TYPE_IPV6_HOP_POS_ERR    (1010000)
 *          - MGMT_TYPE_IPV6_HDR_UNKWN      (1010000)
 *          - MGMT_TYPE_L2_CRC_ERR          (1110000)
 */
extern int32
dal_maple_trap_mgmtFrameAction_get(uint32 unit, rtk_trap_mgmtType_t frameType, rtk_mgmt_action_t *pAction);

/* Function Name:
 *      dal_maple_trap_mgmtFrameAction_set
 * Description:
 *      Set forwarding action of management frame.
 * Input:
 *      unit      - unit id
 *      frameType - type of management frame
 *      actoin    - forwarding action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT      - The module is not initial
 *      RT_ERR_RMA_MGMT_TYPE - invalid type of management frame
 *      RT_ERR_FWD_ACTION    - invalid forwarding action
 * Note:
 *      (1) Forwarding action is as following:
 *          - MGMT_ACTION_FORWARD
 *          - MGMT_ACTION_DROP
 *          - MGMT_ACTION_TRAP2CPU
 *          - MGMT_ACTION_COPY2CPU
 *          - MGMT_ACTION_TRAP2MASTERCPU
 *          - MGMT_ACTION_COPY2MASTERCPU
 *          - MGMT_ACTION_FLOOD_TO_ALL_PORT
 *
 *      (2) To show the suppported action of each management frame in each chip,
 *          we use notation to represent. For example, BPDU supports MGMT_ACTION_FORWARD,
 *          MGMT_ACTION_DROP, MGMT_ACTION_TRAP2CPU, MGMT_ACTION_TRAP2MASTERCPU and
 *          MGMT_ACTION_FLOOD_TO_ALL_PORT in 9310, we use (1110101) to represent.
 *
 *          Supported management frame and action are as following:
 *                                    	      8380
 *          - MGMT_TYPE_ARP                 (1001000)
 *          - MGMT_TYPE_MLD                 (1010000)
 *          - MGMT_TYPE_IGMP                (1010000)
 *          - MGMT_TYPE_EAPOL               (1010000)
 *          - MGMT_TYPE_IPV6ND              (1001000)
 *          - MGMT_TYPE_SELFMAC             (1110000)
 *          - MGMT_TYPE_IPV6_HOP_POS_ERR    (1010000)
 *          - MGMT_TYPE_IPV6_HDR_UNKWN      (1010000)
 *          - MGMT_TYPE_L2_CRC_ERR          (1110000)
 */
extern int32
dal_maple_trap_mgmtFrameAction_set(uint32 unit, rtk_trap_mgmtType_t frameType, rtk_mgmt_action_t action);

/* Function Name:
 *      dal_maple_trap_mgmtFrameLearningEnable_get
 * Description:
 *      Get enable status of SMAC learning for the management frame.
 * Input:
 *      unit      - unit id
 *      frameType - type of management frame
 * Output:
 *      pEnable   - pointer to enable status of SMAC learning
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT      - The module is not initial
 *      RT_ERR_RMA_MGMT_TYPE - invalid type of management frame
 *      RT_ERR_NULL_POINTER  - input parameter may be null pointer
 * Note:
 *      Supported management frame is as following:
 *      - MGMT_TYPE_PTP
 *      - MGMT_TYPE_LLDP
 *      - MGMT_TYPE_BPDU
 */
extern int32
dal_maple_trap_mgmtFrameLearningEnable_get(uint32 unit, rtk_trap_mgmtType_t frameType, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_maple_trap_mgmtFrameLearningEnable_set
 * Description:
 *      Set enable status of SMAC learning for the management frame.
 * Input:
 *      unit      - unit id
 *      frameType - type of management frame
 *      enable    - enable status of SMAC learning
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT      - The module is not initial
 *      RT_ERR_RMA_MGMT_TYPE - invalid type of management frame
 *      RT_ERR_NULL_POINTER  - input parameter may be null pointer
 * Note:
 *      Supported management frame is as following:
 *      - MGMT_TYPE_PTP
 *      - MGMT_TYPE_LLDP
 */
extern int32
dal_maple_trap_mgmtFrameLearningEnable_set(uint32 unit, rtk_trap_mgmtType_t frameType, rtk_enable_t enable);

/* Function Name:
 *      dal_maple_trap_portMgmtFrameAction_get
 * Description:
 *      Get forwarding action of management frame on specified port.
 * Input:
 *      unit      - unit id
 *      port      - port id
 *      frameType - type of management frame
 * Output:
 *      pActoin   - pointer to forwarding action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT      - The module is not initial
 *      RT_ERR_PORT_ID       - invalid port id
 *      RT_ERR_RMA_MGMT_TYPE - invalid type of management frame
 *      RT_ERR_NULL_POINTER  - input parameter may be null pointer
 * Note:
 *      (1) Forwarding action is as following:
 *          - MGMT_ACTION_FORWARD
 *          - MGMT_ACTION_DROP
 *          - MGMT_ACTION_TRAP2CPU
 *          - MGMT_ACTION_COPY2CPU
 *          - MGMT_ACTION_TRAP2MASTERCPU
 *          - MGMT_ACTION_COPY2MASTERCPU
 *          - MGMT_ACTION_FLOOD_TO_ALL_PORT
 *
 *      (2) To show the suppported action of each management frame in each chip,
 *          we use notation to represent. For example, BPDU supports MGMT_ACTION_FORWARD,
 *          MGMT_ACTION_DROP, MGMT_ACTION_TRAP2CPU, MGMT_ACTION_TRAP2MASTERCPU and
 *          MGMT_ACTION_FLOOD_TO_ALL_PORT in 9310, we use (1110101) to represent.
 *
 *          Supported management frame and action are as following:
 *                                    	      8380
 *          - MGMT_TYPE_BPDU                (1110001)
 *          - MGMT_TYPE_PTP                 (1110000)
 *          - MGMT_TYPE_LLDP                (1110001)
 */
extern int32
dal_maple_trap_portMgmtFrameAction_get(uint32 unit, rtk_port_t port, rtk_trap_mgmtType_t frameType, rtk_mgmt_action_t *pAction);

/* Function Name:
 *      dal_maple_trap_portMgmtFrameAction_set
 * Description:
 *      Set forwarding action of management frame on specified port.
 * Input:
 *      unit      - unit id
 *      port      - port id
 *      frameType - type of management frame
 *      actoin    - forwarding action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT      - The module is not initial
 *      RT_ERR_PORT_ID       - invalid port id
 *      RT_ERR_RMA_MGMT_TYPE - invalid type of management frame
 *      RT_ERR_FWD_ACTION    - invalid forwarding action
 * Note:
 *      (1) Forwarding action is as following:
 *          - MGMT_ACTION_FORWARD
 *          - MGMT_ACTION_DROP
 *          - MGMT_ACTION_TRAP2CPU
 *          - MGMT_ACTION_COPY2CPU
 *          - MGMT_ACTION_TRAP2MASTERCPU
 *          - MGMT_ACTION_COPY2MASTERCPU
 *          - MGMT_ACTION_FLOOD_TO_ALL_PORT
 *
 *      (2) To show the suppported action of each management frame in each chip,
 *          we use notation to represent. For example, BPDU supports MGMT_ACTION_FORWARD,
 *          MGMT_ACTION_DROP, MGMT_ACTION_TRAP2CPU, MGMT_ACTION_TRAP2MASTERCPU and
 *          MGMT_ACTION_FLOOD_TO_ALL_PORT in 9310, we use (1110101) to represent.
 *
 *          Supported management frame and action are as following:
 *                                    	      8380
 *          - MGMT_TYPE_BPDU                (1110001)
 *          - MGMT_TYPE_PTP                 (1110000)
 *          - MGMT_TYPE_LLDP                (1110001)
 */
extern int32
dal_maple_trap_portMgmtFrameAction_set(uint32 unit, rtk_port_t port, rtk_trap_mgmtType_t frameType, rtk_mgmt_action_t action);

/* Function Name:
 *      dal_maple_trap_mgmtFramePri_get
 * Description:
 *      Get priority of trapped packet.
 * Input:
 *      unit      - unit id
 *      frameType - type of management frame
 * Output:
 *      pPriority - pointer to priority
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT      - The module is not initial
 *      RT_ERR_RMA_MGMT_TYPE - invalid type of management frame
 *      RT_ERR_NULL_POINTER  - input parameter may be null pointer
 * Note:
 *     Supported management frame is as following:
 *     - MGMT_TYPE_SELFMAC,
 *     - MGMT_TYPE_OTHER,
 *     - MGMT_TYPE_IGR_VLAN_FLTR,
 *     - MGMT_TYPE_CFI,
 *     - MGMT_TYPE_UNKNOWN_DA,
 *     - MGMT_TYPE_RLDP_RLPP,
 *     - MGMT_TYPE_RMA,
 *     - MGMT_TYPE_SPECIAL_COPY,
 *     - MGMT_TYPE_SPECIAL_TRAP,
 *     - MGMT_TYPE_ROUT_EXCEPT,
 *     - MGMT_TYPE_MAC_CST_SYS,
 *     - MGMT_TYPE_MAC_CST_PORT,
 *     - MGMT_TYPE_MAC_CST_VLAN,
 */
extern int32
dal_maple_trap_mgmtFramePri_get(uint32 unit, rtk_trap_mgmtType_t frameType, rtk_pri_t *pPriority);

/* Function Name:
 *      dal_maple_trap_mgmtFramePri_set
 * Description:
 *      Set priority of trapped packet.
 * Input:
 *      unit      - unit id
 *      frameType - type of management frame
 *      priority  - priority
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT      - The module is not initial
 *      RT_ERR_RMA_MGMT_TYPE - invalid type of management frame
 *      RT_ERR_PRIORITY      - invalid priority value
 * Note:
 *     Supported management frame is as following:
 *     - MGMT_TYPE_SELFMAC,
 *     - MGMT_TYPE_OTHER,
 *     - MGMT_TYPE_IGR_VLAN_FLTR,
 *     - MGMT_TYPE_CFI,
 *     - MGMT_TYPE_UNKNOWN_DA,
 *     - MGMT_TYPE_RLDP_RLPP,
 *     - MGMT_TYPE_RMA,
 *     - MGMT_TYPE_SPECIAL_COPY,
 *     - MGMT_TYPE_SPECIAL_TRAP,
 *     - MGMT_TYPE_ROUT_EXCEPT,
 *     - MGMT_TYPE_MAC_CST_SYS,
 *     - MGMT_TYPE_MAC_CST_PORT,
 *     - MGMT_TYPE_MAC_CST_VLAN,
 */
extern int32
dal_maple_trap_mgmtFramePri_set(uint32 unit, rtk_trap_mgmtType_t frameType, rtk_pri_t priority);

/* Function Name:
 *      dal_maple_trap_pktWithCFIAction_get
 * Description:
 *      Get the configuration of inner CFI operation.
 * Input:
 *      unit    - unit id
 * Output:
 *      pAction - pointer to CFI operation
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      CFI is also known as DEI while appeared in service tag.
 *      Forwarding action is as following:
 *      - ACTION_FORWARD
 *      - ACTION_TRAP2CPU
 *      - ACTION_DROP
 */
extern int32
dal_maple_trap_pktWithCFIAction_get(uint32 unit, rtk_action_t *pAction);

/* Function Name:
 *      dal_maple_trap_pktWithCFIAction_set
 * Description:
 *      Set the configuration of inner CFI operation.
 * Input:
 *      unit   - unit id
 *      action - CFI operation
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      CFI is also known as DEI while appeared in service tag.
 *      Forwarding action is as following:
 *      - ACTION_FORWARD
 *      - ACTION_TRAP2CPU
 *      - ACTION_DROP
 */
extern int32
dal_maple_trap_pktWithCFIAction_set(uint32 unit, rtk_action_t action);

/* Function Name:
 *      dal_maple_trap_pktWithOuterCFIAction_get
 * Description:
 *      Get the configuration of outer CFI operation.
 * Input:
 *      unit    - unit id
 * Output:
 *      pAction - pointer to CFI operation
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      CFI is also known as DEI while appeared in service tag.
 *      Forwarding action is as following:
 *      - ACTION_FORWARD
 *      - ACTION_TRAP2CPU
 *      - ACTION_DROP
 */
extern int32
dal_maple_trap_pktWithOuterCFIAction_get(uint32 unit, rtk_action_t *pAction);

/* Function Name:
 *      dal_maple_trap_pktWithOuterCFIAction_set
 * Description:
 *      Set the configuration of outer CFI operation.
 * Input:
 *      unit   - unit id
 *      action - CFI operation
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      CFI is also known as DEI while appeared in service tag.
 *      Forwarding action is as following:
 *      - ACTION_FORWARD
 *      - ACTION_TRAP2CPU
 *      - ACTION_DROP
 */
extern int32
dal_maple_trap_pktWithOuterCFIAction_set(uint32 unit, rtk_action_t action);

/* Function Name:
 *      dal_maple_trap_pktWithCFIPri_get
 * Description:
 *      Get priority of packets trapped to CPU.
 * Input:
 *      unit      - unit id
 * Output:
 *      pPriority - pointer to priority
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_maple_trap_pktWithCFIPri_get(uint32 unit, rtk_pri_t *pPriority);

/* Function Name:
 *      dal_maple_trap_pktWithCFIPri_set
 * Description:
 *      Set priority of packets trapped to CPU.
 * Input:
 *      unit     - unit id
 *      priority - priority
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PRIORITY - invalid priority value
 * Note:
 *      None
 */
extern int32
dal_maple_trap_pktWithCFIPri_set(uint32 unit, rtk_pri_t priority);

/* Function Name:
 *      dal_maple_trap_routeExceptionAction_get
 * Description:
 *      Get the configuration of route exception operation.
 * Input:
 *      unit    - unit id
 *      type    - configure action for which route exception
 * Output:
 *      pAction - pointer to CFI operation
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Forwarding action is as following:
 *      - ACTION_FORWARD
 *      - ACTION_TRAP2CPU
 *      - ACTION_DROP
 */
extern int32
dal_maple_trap_routeExceptionAction_get(
    uint32                          unit,
    rtk_trap_routeExceptionType_t   type,
    rtk_action_t                    *pAction);

/* Function Name:
 *      dal_maple_trap_routeExceptionAction_set
 * Description:
 *      Set the configuration of route exception  operation.
 * Input:
 *      unit   - unit id
 *      type   - configure action for which route exception
 *      action - route exception operation
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      Forwarding action is as following:
 *      - ACTION_FORWARD
 *      - ACTION_TRAP2CPU
 *      - ACTION_DROP
 */
extern int32
dal_maple_trap_routeExceptionAction_set(
    uint32                          unit,
    rtk_trap_routeExceptionType_t   type,
    rtk_action_t                    action);


/* Function Name:
 *      dal_maple_trap_bpduFloodPortmask_get
 * Description:
 *      Get BPDU flooding portmask.
 * Input:
 *      unit   - unit id
 *      pBPDU_flood_portmask - BPDU flooding portmask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 */
extern int32
dal_maple_trap_bpduFloodPortmask_get(uint32 unit,  rtk_portmask_t *pBPDU_flood_portmask);


/* Function Name:
 *      dal_maple_trap_bpduFloodPortmask_set
 * Description:
 *      Set BPDU flooding portmask.
 * Input:
 *      unit   - unit id
 *      pBPDU_flood_portmask - BPDU flooding portmask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 */
extern int32
dal_maple_trap_bpduFloodPortmask_set(uint32 unit,  rtk_portmask_t *pBPDU_flood_portmask);




/* Function Name:
 *      dal_maple_trap_rmaLookupMissActionEnable_get
 * Description:
 *      Get enable status of RMA care lookup miss action.
 * Input:
 *      unit    - unit id
 * Output:
 *      pEnable - pointer to enable status of RMA care lookup miss action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_UNIT_ID          - invalid unit id
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      Enable is care lookup miss action.
 */
extern int32
dal_maple_trap_rmaLookupMissActionEnable_get(uint32 unit, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_maple_trap_rmaLookupMissActionEnable_set
 * Description:
 *      Set enable status of RMA care lookup miss action.
 * Input:
 *      unit    - unit id
 *      enable  - enable status of RMA care lookup miss action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_UNIT_ID          - invalid unit id
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      Enable is care lookup miss action.
 */
extern int32
dal_maple_trap_rmaLookupMissActionEnable_set(uint32 unit, rtk_enable_t enable);


#endif /* __DAL_MAPLE_TRAP_H__ */
