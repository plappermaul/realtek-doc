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
#ifndef __DAL_CYPRESS_TRAP_H__
#define __DAL_CYPRESS_TRAP_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <rtk/trap.h>
#include <dal/dal_mapper.h>

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
 *      dal_cypress_trapMapper_init
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
dal_cypress_trapMapper_init(dal_mapper_t *pMapper);


/* Function Name:
 *      dal_cypress_trap_init
 * Description:
 *      Initial the trap module of the specified device..
 * Input:
 *      unit     - unit id
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
extern int32
dal_cypress_trap_init(uint32 unit);

/* Function Name:
 *      dal_cypress_trap_rmaAction_get
 * Description:
 *      Get action of reserved multicast address(RMA) frame.
 * Input:
 *      unit                - unit id
 *      pRma_frame         - Reserved multicast address.
 * Output:
 *      pRma_action        - RMA action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT         - Invalid input parameter
 *      RT_ERR_NULL_POINTER  - NULL pointer
 * Note:
 *      None.
 */
extern int32
dal_cypress_trap_rmaAction_get(uint32 unit, rtk_mac_t *pRma_frame, rtk_mgmt_action_t *pRma_action);

/* Function Name:
 *      dal_cypress_trap_rmaAction_set
 * Description:
 *      Set action of reserved multicast address(RMA) frame.
 * Input:
 *      unit                - unit id
 *      pRma_frame         - Reserved multicast address.
 *      rma_action          - RMA action
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT         - Invalid input parameter
 *      RT_ERR_RMA_ADDR      - invalid RMA address
 *      RT_ERR_RMA_ACTION    - Invalid RMA action
 * Note:
 *      The supported Reserved Multicast Address frame:
 *      Assignment                                                                  Address
 *      RMA_BRG_GROUP (Bridge Group Address)                                        01-80-C2-00-00-00
 *      RMA_FD_PAUSE (IEEE Std 802.3, 1988 Edition, Full Duplex PAUSE operation)    01-80-C2-00-00-01
 *      RMA_SP_MCAST (IEEE Std 802.3ad Slow Protocols-Multicast address)            01-80-C2-00-00-02
 *      RMA_1X_PAE (IEEE Std 802.1X PAE address)                                    01-80-C2-00-00-03
 *      RMA_RESERVED04 (Reserved)                                                   01-80-C2-00-00-04
 *      RMA_MEDIA_ACCESS_USE (Media Access Method Specific Use)                     01-80-C2-00-00-05
 *      RMA_RESERVED06 (Reserved)                                                   01-80-C2-00-00-06
 *      RMA_RESERVED07 (Reserved)                                                   01-80-C2-00-00-07
 *      RMA_PVD_BRG_GROUP (Provider Bridge Group Address)                           01-80-C2-00-00-08
 *      RMA_RESERVED09 (Reserved)                                                   01-80-C2-00-00-09
 *      RMA_RESERVED0A (Reserved)                                                   01-80-C2-00-00-0A
 *      RMA_RESERVED0B (Reserved)                                                   01-80-C2-00-00-0B
 *      RMA_RESERVED0C (Reserved)                                                   01-80-C2-00-00-0C
 *      RMA_MVRP (Provider Bridge MVRP Address)                                     01-80-C2-00-00-0D
 *      RMA_1ab_LL_DISCOVERY (802.1ab Link Layer Discover Protocol Address)         01-80-C2-00-00-0E
 *      RMA_RESERVED0F (Reserved)                                                   01-80-C2-00-00-0F
 *      RMA_BRG_MNGEMENT (All LANs Bridge Management Group Address)                 01-80-C2-00-00-10
 *      RMA_LOAD_SERV_GENERIC_ADDR (Load Server Generic Address)                    01-80-C2-00-00-11
 *      RMA_LOAD_DEV_GENERIC_ADDR (Loadable Device Generic Address)                 01-80-C2-00-00-12
 *      RMA_RESERVED13 (Reserved)                                                   01-80-C2-00-00-13
 *      RMA_RESERVED14 (Reserved)                                                   01-80-C2-00-00-14
 *      RMA_RESERVED15 (Reserved)                                                   01-80-C2-00-00-15
 *      RMA_RESERVED16 (Reserved)                                                   01-80-C2-00-00-16
 *      RMA_RESERVED17 (Reserved)                                                   01-80-C2-00-00-17
 *      RMA_MANAGER_STA_GENERIC_ADDR (Generic Address for All Manager Stations)     01-80-C2-00-00-18
 *      RMA_RESERVED19 (Reserved)                                                   01-80-C2-00-00-19
 *      RMA_AGENT_STA_GENERIC_ADDR (Generic Address for All Agent Stations)         01-80-C2-00-00-1A
 *      RMA_RESERVED1B (Reserved)                                                   01-80-C2-00-00-1B
 *      RMA_RESERVED1C (Reserved)                                                   01-80-C2-00-00-1C
 *      RMA_RESERVED1D (Reserved)                                                   01-80-C2-00-00-1D
 *      RMA_RESERVED1E (Reserved)                                                   01-80-C2-00-00-1E
 *      RMA_RESERVED1F (Reserved)                                                   01-80-C2-00-00-1F
 *      RMA_GMRP (GMRP Address)                                                     01-80-C2-00-00-20
 *      RMA_GVRP (GVRP address)                                                     01-80-C2-00-00-21
 *      RMA_UNDEF_GARP22~2F (Undefined GARP address)                                01-80-C2-00-00-22
 *                                                                                ~ 01-80-C2-00-00-2F
 *
 *      The supported Reserved Multicast Address action:
 *      -   ACTION_FORWARD
 *      -   ACTION_DROP
 *      -   ACTION_TRAP2CPU
 */
extern int32
dal_cypress_trap_rmaAction_set(uint32 unit, rtk_mac_t *pRma_frame, rtk_mgmt_action_t rma_action);

/* Function Name:
 *      dal_cypress_trap_rmaLearningEnable_get
 * Description:
 *      Get enable status of SMAC learning for this RMA frame.
 * Input:
 *      unit       - unit id
 *      pRma_frame - Reserved multicast address.
 * Output:
 *      pEnable    - pointer to enable status of SMAC learning
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_RMA_ADDR         - invalid invalid RMA address
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_trap_rmaLearningEnable_get(uint32 unit, rtk_mac_t *pRma_frame, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_cypress_trap_rmaLearningEnable_set
 * Description:
 *      Set enable status of SMAC learning for this RMA frame.
 * Input:
 *      unit       - unit id
 *      pRma_frame - Reserved multicast address.
 *      enable     - enable status of SMAC learning
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_RMA_ADDR         - invalid invalid RMA address
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_cypress_trap_rmaLearningEnable_set(uint32 unit, rtk_mac_t *pRma_frame, rtk_enable_t enable);

/* Function Name:
 *      dal_cypress_trap_rmaBypassStpEnable_get
 * Description:
 *      Get enable status of bypassing spanning tree for this RMA frame.
 * Input:
 *      unit       - unit id
 *      pRma_frame - Reserved multicast address.
 * Output:
 *      pEnable    - pointer to enable status of vlan checking
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_RMA_ADDR         - invalid invalid RMA address
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_trap_rmaBypassStpEnable_get(uint32 unit, rtk_mac_t *pRma_frame, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_cypress_trap_rmaBypassStpEnable_set
 * Description:
 *      Set enable status of bypassing spanning tree for this RMA frame.
 * Input:
 *      unit       - unit id
 *      pRma_frame - Reserved multicast address.
 *      enable     - enable status of vlan checking
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_RMA_ADDR         - invalid invalid RMA address
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_cypress_trap_rmaBypassStpEnable_set(uint32 unit, rtk_mac_t *pRma_frame, rtk_enable_t enable);

/* Function Name:
 *      dal_cypress_trap_bypassStp_get
 * Description:
 *      Get enable status of bypassing spanning tree for specified frame type
 * Input:
 *      unit       - unit id
 *      frameType  - frame type
 * Output:
 *      pEnable    - pointer to enable status of bypassing STP
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *    The frame type selection is as following:
 *    - BYPASS_STP_TYPE_USER_DEF_0
 *    - BYPASS_STP_TYPE_USER_DEF_1
 *    - BYPASS_STP_TYPE_RMA_0X
 *    - BYPASS_STP_TYPE_SLOW_PROTO
 *    - BYPASS_STP_TYPE_EAPOL
 *    - BYPASS_STP_TYPE_PTP
 *    - BYPASS_STP_TYPE_LLDP
 */
extern int32
dal_cypress_trap_bypassStp_get(uint32 unit, rtk_trap_bypassStpType_t frameType, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_cypress_trap_bypassStp_set
 * Description:
 *      Set enable status of bypassing spanning tree for specified frame type
 * Input:
 *      unit       - unit id
 *      frameType  - frame type
 *      enable     - enable status of bypassing STP
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_RMA_ADDR         - invalid invalid RMA address
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *    The frame type selection is as following:
 *    - BYPASS_STP_TYPE_USER_DEF_0
 *    - BYPASS_STP_TYPE_USER_DEF_1
 *    - BYPASS_STP_TYPE_RMA_0X
 *    - BYPASS_STP_TYPE_SLOW_PROTO
 *    - BYPASS_STP_TYPE_EAPOL
 *    - BYPASS_STP_TYPE_PTP
 *    - BYPASS_STP_TYPE_LLDP
 */
extern int32
dal_cypress_trap_bypassStp_set(uint32 unit, rtk_trap_bypassStpType_t frameType, rtk_enable_t enable);

/* Function Name:
 *      dal_cypress_trap_bypassVlan_get
 * Description:
 *      Get enable status of bypassing VLAN drop for specified frame type
 * Input:
 *      unit       - unit id
 *      frameType  - frame type
 * Output:
 *      pEnable    - pointer to enable status of bypassing STP
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *    The frame type selection is as following:
 *    - BYPASS_VLAN_TYPE_USER_DEF_0
 *    - BYPASS_VLAN_TYPE_USER_DEF_1
 *    - BYPASS_VLAN_TYPE_RMA_00
 *    - BYPASS_VLAN_TYPE_RMA_02
 *    - BYPASS_VLAN_TYPE_RMA_0E
 *    - BYPASS_VLAN_TYPE_RMA_0X
 *    - BYPASS_VLAN_TYPE_EAPOL,
 *    - BYPASS_VLAN_TYPE_PTP
 *    - BYPASS_VLAN_TYPE_LLDP
 */
extern int32
dal_cypress_trap_bypassVlan_get(uint32 unit, rtk_trap_bypassVlanType_t frameType, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_cypress_trap_bypassVlan_set
 * Description:
 *      Set enable status of bypassing VLAN drop for specified frame type
 * Input:
 *      unit       - unit id
 *      frameType  - frame type
 *      enable     - enable status of bypassing STP
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *    The frame type selection is as following:
 *    - BYPASS_VLAN_TYPE_USER_DEF_0
 *    - BYPASS_VLAN_TYPE_USER_DEF_1
 *    - BYPASS_VLAN_TYPE_RMA_00
 *    - BYPASS_VLAN_TYPE_RMA_02
 *    - BYPASS_VLAN_TYPE_RMA_0E
 *    - BYPASS_VLAN_TYPE_RMA_0X
 *    - BYPASS_VLAN_TYPE_PTP
 *    - BYPASS_VLAN_TYPE_LLDP
 */
extern int32
dal_cypress_trap_bypassVlan_set(uint32 unit, rtk_trap_bypassVlanType_t frameType, rtk_enable_t enable);

/* Function Name:
 *      dal_cypress_trap_userDefineRma_get
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
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_OUT_OF_RANGE     - userDefine_idx is out of range
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_trap_userDefineRma_get(
    uint32                      unit,
    uint32                      userDefine_idx,
    rtk_trap_userDefinedRma_t   *pUserDefinedRma);

/* Function Name:
 *      dal_cypress_trap_userDefineRma_set
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
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_OUT_OF_RANGE     - userDefine_idx is out of range
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_trap_userDefineRma_set(
    uint32                      unit,
    uint32                      userDefine_idx,
    rtk_trap_userDefinedRma_t   *pUserDefinedRma);

/* Function Name:
 *      dal_cypress_trap_userDefineRmaEnable_get
 * Description:
 *      Get enable status of user defined RMA.
 * Input:
 *      unit            - unit id
 *      userDefine_idx  - index of user defined RMA entry
 * Output:
 *      pEnable         - pointer to enable status of RMA entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_OUT_OF_RANGE     - userDefine_idx is out of range
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_trap_userDefineRmaEnable_get(uint32 unit, uint32 userDefine_idx, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_cypress_trap_userDefineRmaEnable_set
 * Description:
 *      Set enable status of user defined RMA.
 * Input:
 *      unit            - unit id
 *      userDefine_idx  - index of user defined RMA entry
 *      enable          - enable status of RMA entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_OUT_OF_RANGE     - userDefine_idx is out of range
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_cypress_trap_userDefineRmaEnable_set(uint32 unit, uint32 userDefine_idx, rtk_enable_t enable);

/* Function Name:
 *      dal_cypress_trap_userDefineRmaAction_get
 * Description:
 *      Get forwarding action of user defined RMA.
 * Input:
 *      unit            - unit id
 *      userDefine_idx  - index of user defined RMA entry
 * Output:
 *      pActoin         - pointer to forwarding action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_OUT_OF_RANGE     - userDefine_idx is out of range
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      Forwarding action is as following:
 *      - ACTION_FORWARD
 *      - ACTION_TRAP2CPU
 *      - ACTION_DROP
 */
extern int32
dal_cypress_trap_userDefineRmaAction_get(uint32 unit, uint32 userDefine_idx, rtk_mgmt_action_t *pAction);

/* Function Name:
 *      dal_cypress_trap_userDefineRmaAction_set
 * Description:
 *      Set forwarding action of user defined RMA.
 * Input:
 *      unit            - unit id
 *      userDefine_idx  - index of user defined RMA entry
 *      actoin          - forwarding action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_OUT_OF_RANGE     - userDefine_idx is out of range
 *      RT_ERR_FWD_ACTION       - invalid forwarding action
 * Note:
 *      Forwarding action is as following:
 *      - ACTION_FORWARD
 *      - ACTION_TRAP2CPU
 *      - ACTION_DROP
 */
extern int32
dal_cypress_trap_userDefineRmaAction_set(uint32 unit, uint32 userDefine_idx, rtk_mgmt_action_t action);

/* Function Name:
 *      dal_cypress_trap_userDefineRmaLearningEnable_get
 * Description:
 *      Get enable status of SMAC learning for user-defined RMA frame.
 * Input:
 *      unit            - unit id
 *      userDefine_idx  - index of user defined RMA entry
 * Output:
 *      pEnable         - pointer to enable status of SMAC learning
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_OUT_OF_RANGE     - userDefine_idx is out of range
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_trap_userDefineRmaLearningEnable_get(uint32 unit, uint32 userDefine_idx, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_cypress_trap_userDefineRmaLearningEnable_set
 * Description:
 *      Set enable status of SMAC learning for this user-defined RMA frame.
 * Input:
 *      unit            - unit id
 *      userDefine_idx  - index of user defined RMA entry
 *      enable          - enable status of SMAC learning
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_OUT_OF_RANGE     - userDefine_idx is out of range
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_cypress_trap_userDefineRmaLearningEnable_set(uint32 unit, uint32 userDefine_idx, rtk_enable_t enable);

/* Function Name:
 *      dal_cypress_trap_userDefineRmaBypassStpEnable_get
 * Description:
 *      Get enable status of bypassing spanning tree for user-defined RMA frame.
 * Input:
 *      unit            - unit id
 *      userDefine_idx  - index of user defined RMA entry
 * Output:
 *      pEnable         - pointer to enable status of bypassing STP
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_RMA_ADDR         - invalid invalid RMA address
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_trap_userDefineRmaBypassStpEnable_get(uint32 unit, uint32 userDefine_idx, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_cypress_trap_userDefineRmaBypassStpEnable_set
 * Description:
 *      Set enable status of bypassing spanning tree for user-defined RMA frame.
 * Input:
 *      unit            - unit id
 *      userDefine_idx  - index of user defined RMA entry
 *      enable          - enable status of bypassing STP
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_RMA_ADDR         - invalid invalid RMA address
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_cypress_trap_userDefineRmaBypassStpEnable_set(uint32 unit, uint32 userDefine_idx, rtk_enable_t enable);

/* Function Name:
 *      dal_cypress_trap_mgmtFrameAction_get
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
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_RMA_MGMT_TYPE    - invalid type of management frame
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
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
 *                                    	      8390
 *          - MGMT_TYPE_ARP                 (1001000)
 *          - MGMT_TYPE_MLD                 (1010000)
 *          - MGMT_TYPE_IGMP                (1010000)
 *          - MGMT_TYPE_EAPOL               (1010000)
 *          - MGMT_TYPE_IPV6ND              (1001000)
 *          - MGMT_TYPE_SELFMAC             (1110000)
 *          - MGMT_TYPE_IPV6_HOP_POS_ERR    (1010000)
 *          - MGMT_TYPE_IPV6_HDR_UNKWN      (1010000)
 *          - MGMT_TYPE_L2_CRC_ERR          (1110000)
 *          - MGMT_TYPE_IP4_CHKSUM_ERR      (1110000)
 */
extern int32
dal_cypress_trap_mgmtFrameAction_get(uint32 unit, rtk_trap_mgmtType_t frameType, rtk_mgmt_action_t *pAction);

/* Function Name:
 *      dal_cypress_trap_mgmtFrameAction_set
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
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_RMA_MGMT_TYPE    - invalid type of management frame
 *      RT_ERR_FWD_ACTION       - invalid forwarding action
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
 *                                    	      8390
 *          - MGMT_TYPE_ARP                 (1001000)
 *          - MGMT_TYPE_MLD                 (1010000)
 *          - MGMT_TYPE_IGMP                (1010000)
 *          - MGMT_TYPE_EAPOL               (1010000)
 *          - MGMT_TYPE_IPV6ND              (1001000)
 *          - MGMT_TYPE_SELFMAC             (1110000)
 *          - MGMT_TYPE_IPV6_HOP_POS_ERR    (1010000)
 *          - MGMT_TYPE_IPV6_HDR_UNKWN      (1010000)
 *          - MGMT_TYPE_L2_CRC_ERR          (1110000)
 *          - MGMT_TYPE_IP4_CHKSUM_ERR      (1110000)
 */
extern int32
dal_cypress_trap_mgmtFrameAction_set(uint32 unit, rtk_trap_mgmtType_t frameType, rtk_mgmt_action_t action);

/* Function Name:
 *      dal_cypress_trap_mgmtFramePri_get
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
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_RMA_MGMT_TYPE    - invalid type of management frame
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      Supported management frame is as following:
 *      - MGMT_TYPE_OTHER
 *      - MGMT_TYPE_OAM
 *      - MGMT_TYPE_CFM
 *      - MGMT_TYPE_IGR_VLAN_FLTR
 *      - MGMT_TYPE_VLAN_ERR
 *      - MGMT_TYPE_CFI
 *      - MGMT_TYPE_RMA_USR_DEF_1
 *      - MGMT_TYPE_RMA_USR_DEF_2
 *      - MGMT_TYPE_BPDU
 *      - MGMT_TYPE_LACP
 *      - MGMT_TYPE_PTP
 *      - MGMT_TYPE_LLDP
 *      - MGMT_TYPE_RMA_0X
 *      - MGMT_TYPE_RMA_1X
 *      - MGMT_TYPE_RMA_2X
 *      - MGMT_TYPE_IPV4_HDR_ERR
 *      - MGMT_TYPE_IPV4_TTL_EXCEED
 *      - MGMT_TYPE_IPV4_OPT
 *      - MGMT_TYPE_IPV6_HDR_ERR
 *      - MGMT_TYPE_IPV6_HL_EXCEED
 *      - MGMT_TYPE_IPV6_HOPBYHOP
 *      - MGMT_TYPE_GW_MAC_ERR
 *      - MGMT_TYPE_IGMP
 *      - MGMT_TYPE_MLD
 *      - MGMT_TYPE_EAPOL
 *      - MGMT_TYPE_ARP
 *      - MGMT_TYPE_IPV6ND
 *      - MGMT_TYPE_UNKNOWN_DA
 *      - MGMT_TYPE_SELFMAC
 */
extern int32
dal_cypress_trap_mgmtFramePri_get(uint32 unit, rtk_trap_mgmtType_t frameType, rtk_pri_t *pPriority);

/* Function Name:
 *      dal_cypress_trap_mgmtFramePri_set
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
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_RMA_MGMT_TYPE    - invalid type of management frame
 *      RT_ERR_PRIORITY         - invalid priority value
 * Note:
 *      Supported management frame is as following:
 *      - MGMT_TYPE_OTHER
 *      - MGMT_TYPE_OAM
 *      - MGMT_TYPE_CFM
 *      - MGMT_TYPE_IGR_VLAN_FLTR
 *      - MGMT_TYPE_VLAN_ERR
 *      - MGMT_TYPE_CFI
 *      - MGMT_TYPE_RMA_USR_DEF_1
 *      - MGMT_TYPE_RMA_USR_DEF_2
 *      - MGMT_TYPE_BPDU
 *      - MGMT_TYPE_LACP
 *      - MGMT_TYPE_PTP
 *      - MGMT_TYPE_LLDP
 *      - MGMT_TYPE_RMA_0X
 *      - MGMT_TYPE_RMA_1X
 *      - MGMT_TYPE_RMA_2X
 *      - MGMT_TYPE_IPV4_HDR_ERR
 *      - MGMT_TYPE_IPV4_TTL_EXCEED
 *      - MGMT_TYPE_IPV4_OPT
 *      - MGMT_TYPE_IPV6_HDR_ERR
 *      - MGMT_TYPE_IPV6_HL_EXCEED
 *      - MGMT_TYPE_IPV6_HOPBYHOP
 *      - MGMT_TYPE_GW_MAC_ERR
 *      - MGMT_TYPE_IGMP
 *      - MGMT_TYPE_MLD
 *      - MGMT_TYPE_EAPOL
 *      - MGMT_TYPE_ARP
 *      - MGMT_TYPE_IPV6ND
 *      - MGMT_TYPE_UNKNOWN_DA
 *      - MGMT_TYPE_SELFMAC
 */
extern int32
dal_cypress_trap_mgmtFramePri_set(uint32 unit, rtk_trap_mgmtType_t frameType, rtk_pri_t priority);

/* Function Name:
 *      dal_cypress_trap_mgmtFrameLearningEnable_get
 * Description:
 *      Get enable status of SMAC learning for the management frame.
 * Input:
 *      unit        - unit id
 *      frameType   - type of management frame
 * Output:
 *      pEnable     - pointer to enable status of SMAC learning
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_RMA_MGMT_TYPE    - invalid type of management frame
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      Supported management frame is as following:
 *      - MGMT_TYPE_PTP
 *      - MGMT_TYPE_LLDP
 */
extern int32
dal_cypress_trap_mgmtFrameLearningEnable_get(uint32 unit, rtk_trap_mgmtType_t frameType, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_cypress_trap_mgmtFrameLearningEnable_set
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
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_RMA_MGMT_TYPE    - invalid type of management frame
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      Supported management frame is as following:
 *      - MGMT_TYPE_PTP
 *      - MGMT_TYPE_LLDP
 */
extern int32
dal_cypress_trap_mgmtFrameLearningEnable_set(uint32 unit, rtk_trap_mgmtType_t frameType, rtk_enable_t enable);

/* Function Name:
 *      dal_cypress_trap_portMgmtFrameAction_get
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
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_RMA_MGMT_TYPE    - invalid type of management frame
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
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
 *                                    	      8390
 *          - MGMT_TYPE_BPDU                (1110001)
 *          - MGMT_TYPE_PTP                 (1110000)
 *          - MGMT_TYPE_LLDP                (1110001)
 */
extern int32
dal_cypress_trap_portMgmtFrameAction_get(uint32 unit, rtk_port_t port, rtk_trap_mgmtType_t frameType, rtk_mgmt_action_t *pAction);

/* Function Name:
 *      dal_cypress_trap_portMgmtFrameAction_set
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
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_RMA_MGMT_TYPE    - invalid type of management frame
 *      RT_ERR_FWD_ACTION       - invalid forwarding action
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
 *                                    	      8390
 *          - MGMT_TYPE_BPDU                (1110001)
 *          - MGMT_TYPE_PTP                 (1110000)
 *          - MGMT_TYPE_LLDP                (1110001)
 */
extern int32
dal_cypress_trap_portMgmtFrameAction_set(uint32 unit, rtk_port_t port, rtk_trap_mgmtType_t frameType, rtk_mgmt_action_t action);

/* Function Name:
 *      dal_cypress_trap_oamPDUAction_get
 * Description:
 *      Get forwarding action of trapped oam PDU.
 * Input:
 *      unit    - unit id
 * Output:
 *      pAction - pointer to forwarding action of trapped oam PDU
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 */
extern int32
dal_cypress_trap_oamPDUAction_get(uint32 unit, rtk_action_t *pAction);

/* Function Name:
 *      dal_cypress_trap_oamPDUAction_set
 * Description:
 *      Set forwarding action of trapped oam PDU.
 * Input:
 *      unit   - unit id
 *      action - forwarding action of trapped oam PDU
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 */
extern int32
dal_cypress_trap_oamPDUAction_set(uint32 unit, rtk_action_t action);

/* Function Name:
 *      dal_cypress_trap_oamPDUPri_get
 * Description:
 *      Get priority of trapped OAM PDU.
 * Input:
 *      unit      - unit id
 * Output:
 *      pPriority - pointer to priority
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_trap_oamPDUPri_get(uint32 unit, rtk_pri_t *pPriority);

/* Function Name:
 *      dal_cypress_trap_oamPDUPri_set
 * Description:
 *      Set priority of trapped OAM PDU.
 * Input:
 *      unit     - unit id
 *      priority - priority
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_PRIORITY         - invalid priority value
 * Note:
 *      None
 */
extern int32
dal_cypress_trap_oamPDUPri_set(uint32 unit, rtk_pri_t priority);
#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
/* Function Name:
 *      dal_cypress_trap_cfmUnknownFrameAct_get
 * Description:
 *      Get action for receive unknown type of CFM frame.
 * Input:
 *      unit    - unit id
 * Output:
 *      action  - pointer buffer
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_trap_cfmUnknownFrameAct_get(uint32 unit, rtk_action_t *action);

/* Function Name:
 *      dal_cypress_trap_cfmUnknownFrameAct_set
 * Description:
 *      Set action for receive unknown type of CFM frame.
 * Input:
 *      unit    - unit id
 *      action  - receive unknown type of CFM frame action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_cypress_trap_cfmUnknownFrameAct_set(uint32 unit,
                                        rtk_action_t action);

/* Function Name:
 *      dal_cypress_trap_cfmLoopbackAct_get
 * Description:
 *      Get action for receive CFM loopback frame.
 * Input:
 *      unit    - unit id
 *      level   - MD level
 * Output:
 *      action  - pointer buffer
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      loopback action combine with linktrace in RTL8390
 */
extern int32
dal_cypress_trap_cfmLoopbackAct_get(uint32 unit, uint32 level,
                                    rtk_action_t *action);

/* Function Name:
 *      dal_cypress_trap_cfmLoopbackAct_set
 * Description:
 *      Set action for receive CFM loopback frame.
 * Input:
 *      unit    - unit id
 *      level   - MD level
 *      action  - receive CFM loopback frame action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      loopback action combine with linktrace in RTL8390
 */
extern int32
dal_cypress_trap_cfmLoopbackAct_set(uint32 unit, uint32 level,
                                    rtk_action_t action);

/* Function Name:
 *      dal_cypress_trap_cfmCcmAct_get
 * Description:
 *      Get action for receive CFM CCM frame.
 * Input:
 *      unit    - unit id
 *      level   - MD level
 * Output:
 *      action  - pointer buffer
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_trap_cfmCcmAct_get(uint32 unit, uint32 level,
                               rtk_trap_oam_action_t *action);

/* Function Name:
 *      dal_cypress_trap_cfmCcmAct_set
 * Description:
 *      Set action for receive CFM CCM frame.
 * Input:
 *      unit    - unit id
 *      level   - MD level
 *      action  - receive CFM CCM frame action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_cypress_trap_cfmCcmAct_set(uint32 unit, uint32 level,
                               rtk_trap_oam_action_t action);

/* Function Name:
 *      dal_cypress_trap_cfmEthDmAct_get
 * Description:
 *      Get action for receive CFM ETH-DM frame in specified MD level.
 * Input:
 *      unit    - unit id
 *      mdl     - MD level
 * Output:
 *      pAction - pointer to action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      ETH-DM frame action is as following:
 *         chip value 0 - ACTION_DROP
 *         chip value 1 - ACTION_TRAP2CPU
 *         chip value 2 - ACTION_FORWARD
 */
extern int32
dal_cypress_trap_cfmEthDmAct_get(uint32 unit, uint32 mdl, rtk_action_t *pAction);

/* Function Name:
 *      dal_cypress_trap_cfmEthDmAct_set
 * Description:
 *      Set action for receive CFM ETH-DM frame in specified MD level.
 * Input:
 *      unit    - unit id
 *      mdl     - MD level
 *      action  - action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_cypress_trap_cfmEthDmAct_set(uint32 unit, uint32 mdl, rtk_action_t action);
#endif  /* CONFIG_SDK_DRIVER_RTK_LEGACY_API */
/* Function Name:
 *      dal_cypress_trap_cfmFrameTrapPri_get
 * Description:
 *      Get priority of CFM packets trapped to CPU.
 * Input:
 *      unit      - unit id
 * Output:
 *      pPriority - pointer to priority
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      ETH-DM frame action is as following:
 *         chip value 0 - ACTION_DROP
 *         chip value 1 - ACTION_TRAP2CPU
 *         chip value 2 - ACTION_FORWARD
 */
extern int32
dal_cypress_trap_cfmFrameTrapPri_get(uint32 unit, rtk_pri_t *pPriority);

/* Function Name:
 *      dal_cypress_trap_cfmFrameTrapPri_set
 * Description:
 *      Set priority of CFM packets trapped to CPU.
 * Input:
 *      unit     - unit id
 *      priority - priority
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PRIORITY         - invalid priority value
 * Note:
 *      None
 */
extern int32
dal_cypress_trap_cfmFrameTrapPri_set(uint32 unit, rtk_pri_t priority);


/* Function Name:
 *      dal_cypress_trap_portOamLoopbackParAction_get
 * Description:
 *      Get action of parser on specific port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pAction - pointer to parser action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      Parser action is as following:
 *         chip value 0 - TRAP_OAM_ACTION_DROP
 *         chip value 1 - TRAP_OAM_ACTION_FORWARD
 *         chip value 2 - TRAP_OAM_ACTION_LOOPBACK
 *         chip value 3 - TRAP_OAM_ACTION_TRAP2CPU
 */
extern int32
dal_cypress_trap_portOamLoopbackParAction_get(uint32 unit,
        rtk_port_t port, rtk_trap_oam_action_t *pAction);

/* Function Name:
 *      dal_cypress_trap_portOamLoopbackParAction_set
 * Description:
 *      Set action of parser on specific port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      action  - parser action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      Parser action is as following:
 *         chip value 0 - TRAP_OAM_ACTION_DROP
 *         chip value 1 - TRAP_OAM_ACTION_FORWARD
 *         chip value 2 - TRAP_OAM_ACTION_LOOPBACK
 *         chip value 3 - TRAP_OAM_ACTION_TRAP2CPU
 */
extern int32
dal_cypress_trap_portOamLoopbackParAction_set(uint32 unit,
        rtk_port_t port, rtk_trap_oam_action_t action);

/* Function Name:
 *      dal_cypress_trap_pktWithCFIAction_get
 * Description:
 *      Get the configuration of inner CFI operation.
 * Input:
 *      unit    - unit id
 * Output:
 *      rtk_action_t    - pointer to CFI operation
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      CFI is also known as DEI while appeared in service tag.
 */
extern int32
dal_cypress_trap_pktWithCFIAction_get(uint32 unit, rtk_action_t *pAction);

/* Function Name:
 *      dal_cypress_trap_pktWithCFIAction_set
 * Description:
 *      Set the configuration of inner CFI operation.
 * Input:
 *      unit    - unit id
 *      action  - CFI operation
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      CFI is also known as DEI while appeared in service tag.
 */
extern int32
dal_cypress_trap_pktWithCFIAction_set(uint32 unit, rtk_action_t action);

/* Function Name:
 *      dal_cypress_trap_pktWithOuterCFIAction_get
 * Description:
 *      Get the configuration of outer CFI operation.
 * Input:
 *      unit    - unit id
 * Output:
 *      rtk_action_t    - pointer to CFI operation
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      CFI is also known as DEI while appeared in service tag.
 */
extern int32
dal_cypress_trap_pktWithOuterCFIAction_get(uint32 unit, rtk_action_t *pAction);

/* Function Name:
 *      dal_cypress_trap_pktWithOuterCFIAction_set
 * Description:
 *      Set the configuration of outer CFI operation.
 * Input:
 *      unit    - unit id
 *      action  - CFI operation
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      CFI is also known as DEI while appeared in service tag.
 */
extern int32
dal_cypress_trap_pktWithOuterCFIAction_set(uint32 unit, rtk_action_t action);

/* Function Name:
 *      dal_cypress_trap_pktWithCFIPri_get
 * Description:
 *      Get priority of packets trapped to CPU.
 * Input:
 *      unit      - unit id
 * Output:
 *      pPriority - pointer to priority
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_trap_pktWithCFIPri_get(uint32 unit, rtk_pri_t *pPriority);

/* Function Name:
 *      dal_cypress_trap_pktWithCFIPri_set
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
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PRIORITY         - invalid priority value
 * Note:
 *      None
 */
extern int32
dal_cypress_trap_pktWithCFIPri_set(uint32 unit, rtk_pri_t priority);

/* Function Name:
 *      dal_cypress_trap_routeExceptionAction_get
 * Description:
 *      Get the configuration of outer CFI operation.
 * Input:
 *      unit    - unit id
 *      type    - configure action for which route exception
 * Output:
 *      rtk_action_t    - pointer to CFI operation
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      CFI is also known as DEI while appeared in service tag.
 */
extern int32
dal_cypress_trap_routeExceptionAction_get(uint32 unit,
        rtk_trap_routeExceptionType_t type, rtk_action_t *pAction);

/* Function Name:
 *      dal_cypress_trap_routeExceptionAction_set
 * Description:
 *      Set the configuration of outer CFI operation.
 * Input:
 *      unit    - unit id
 *      type    - configure action for which route exception
 *      action  - route exception operation
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_cypress_trap_routeExceptionAction_set(uint32 unit,
        rtk_trap_routeExceptionType_t type, rtk_action_t action);

/* Function Name:
 *      dal_cypress_trap_routeExceptionPri_get
 * Description:
 *      Get priority of route exception packets trapped to CPU.
 * Input:
 *      unit    - unit id
 *      type    - configure priority for which route exception
 * Output:
 *      pPriority - pointer to priority
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_trap_routeExceptionPri_get(uint32 unit,
        rtk_trap_routeExceptionType_t type, rtk_pri_t *pPriority);

/* Function Name:
 *      dal_cypress_trap_routeExceptionPri_set
 * Description:
 *      Set priority of route exception packets trapped to CPU.
 * Input:
 *      unit        - unit id
 *      type        - configure priority for which route exception
 *      priority    - priority
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_PRIORITY         - invalid priority value
 * Note:
 *      None
 */
extern int32
dal_cypress_trap_routeExceptionPri_set(uint32 unit,
        rtk_trap_routeExceptionType_t type, rtk_pri_t priority);


/* Function Name:
 *      dal_cypress_trap_mgmtFrameMgmtVlanEnable_get
 * Description:
 *      Get compare forwarding VID ability with PVID of CPU port to
 *      IPv6 neighbor discovery, ARP request and Switch MAC packets.
 * Input:
 *      unit                - unit id
 * Output:
 *      pEnable            - status of trap to CPU
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER  - NULL pointer
 * Note:
 *      The status of trap to CPU:
 *      - DISABLED
 *      - ENABLED
 */
extern int32
dal_cypress_trap_mgmtFrameMgmtVlanEnable_get(uint32 unit, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_cypress_trap_mgmtFrameMgmtVlanEnable_set
 * Description:
 *      Set compare forwarding VID ability with PVID of CPU port to
 *      IPv6 neighbor discovery, ARP request and Switch MAC packets.
 * Input:
 *      unit            - unit id
 *      enable          - status of trap to CPU
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT     - Invalid input parameter
 * Note:
 *      The status of trap to CPU:
 *      - DISABLED
 *      - ENABLED
 */
extern int32
dal_cypress_trap_mgmtFrameMgmtVlanEnable_set(uint32 unit, rtk_enable_t enable);

/* Function Name:
 *      dal_cypress_trap_mgmtFrameSelfARPEnable_get
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
dal_cypress_trap_mgmtFrameSelfARPEnable_get(uint32 unit, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_cypress_trap_mgmtFrameSelfARPEnable_set
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
dal_cypress_trap_mgmtFrameSelfARPEnable_set(uint32 unit, rtk_enable_t enable);

/* Function Name:
 *      dal_cypress_trap_bpduFloodPortmask_get
 * Description:
 *      Get BPDU flooding portmask.
 * Input:
 *      unit            - unit id
 *      pflood_portmask - BPDU flooding portmask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 */
extern int32
dal_cypress_trap_bpduFloodPortmask_get(uint32 unit, rtk_portmask_t *pflood_portmask);

/* Function Name:
 *      dal_cypress_trap_bpduFloodPortmask_set
 * Description:
 *      Set BPDU flooding portmask.
 * Input:
 *      unit            - unit id
 *      pflood_portmask - BPDU flooding portmask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 */
extern int32
dal_cypress_trap_bpduFloodPortmask_set(uint32 unit, rtk_portmask_t *pflood_portmask);

/* Function Name:
 *      dal_cypress_trap_rmaLookupMissActionEnable_get
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
dal_cypress_trap_rmaLookupMissActionEnable_get(uint32 unit, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_cypress_trap_rmaLookupMissActionEnable_set
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
dal_cypress_trap_rmaLookupMissActionEnable_set(uint32 unit, rtk_enable_t enable);

/*
 * Function Name:
 *      dal_cypress_trap_cfmAct_get
 * Description:
 *      Get action for receive specified type of CFM frame.
 * Input:
 *      unit    - unit id
 *      type    - CFM frame type
 *      mdl     - MD level (for unknow type, the field is not used)
 * Output:
 *      pAction - pointer buffer of action for receive specified type of CFM frame
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_LINK_FAULT_DETECT (only for CCM)
 */
extern int32
dal_cypress_trap_cfmAct_get(uint32 unit, rtk_trap_cfmType_t type, uint32 mdl,
    rtk_action_t *pAction);

/* Function Name:
 *      dal_cypress_trap_cfmAct_set
 * Description:
 *      Set action for receive specified type of CFM frame.
 * Input:
 *      unit    - unit id
 *      type    - CFM frame type
 *      mdl     - MD level (for unknow type, the field is not used)
 *      action  - receive specified type of CFM frame action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_LINK_FAULT_DETECT (only for CCM)
 */
extern int32
dal_cypress_trap_cfmAct_set(uint32 unit, rtk_trap_cfmType_t type, uint32 mdl,
    rtk_action_t action);
#endif /* __DAL_CYPRESS_TRAP_H__ */

