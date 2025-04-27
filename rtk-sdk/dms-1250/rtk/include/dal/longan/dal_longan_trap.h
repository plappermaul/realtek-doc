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
#ifndef __DAL_longan_TRAP_H__
#define __DAL_longan_TRAP_H__

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
 *      dal_longan_trapMapper_init
 * Description:
 *      Hook trap module of the specified device.
 * Input:
 *      pMapper - pointer of mapper
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      Must Hook trap module before calling any trap APIs.
 */
extern int32
dal_longan_trapMapper_init(dal_mapper_t *pMapper);

/* Function Name:
 *      dal_longan_trap_init
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
dal_longan_trap_init(uint32 unit);

/* Function Name:
 *      dal_longan_trap_rmaAction_get
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
dal_longan_trap_rmaAction_get(uint32 unit, rtk_mac_t *pRma_frame, rtk_mgmt_action_t *pRma_action);

/* Function Name:
 *      dal_longan_trap_rmaAction_set
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
 *      -   MGMT_ACTION_FORWARD
 *      -   MGMT_ACTION_DROP
 *      -   MGMT_ACTION_TRAP2CPU
 */
extern int32
dal_longan_trap_rmaAction_set(uint32 unit, rtk_mac_t *pRma_frame, rtk_mgmt_action_t rma_action);

/* Function Name:
 *      dal_longan_trap_rmaLearningEnable_get
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
dal_longan_trap_rmaLearningEnable_get(uint32 unit, rtk_mac_t *pRma_frame, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_longan_trap_rmaLearningEnable_set
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
dal_longan_trap_rmaLearningEnable_set(uint32 unit, rtk_mac_t *pRma_frame, rtk_enable_t enable);

/* Function Name:
 *      dal_longan_trap_rmaBypassStpEnable_get
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
dal_longan_trap_rmaBypassStpEnable_get(uint32 unit, rtk_mac_t *pRma_frame, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_longan_trap_rmaBypassStpEnable_set
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
dal_longan_trap_rmaBypassStpEnable_set(uint32 unit, rtk_mac_t *pRma_frame, rtk_enable_t enable);

/* Function Name:
 *      dal_longan_trap_bypassStp_get
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
 *    - BYPASS_STP_TYPE_USER_DEF_2
 *    - BYPASS_STP_TYPE_USER_DEF_3
 */
extern int32
dal_longan_trap_bypassStp_get(uint32 unit, rtk_trap_bypassStpType_t frameType, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_longan_trap_bypassStp_set
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
 *    - BYPASS_STP_TYPE_USER_DEF_2
 *    - BYPASS_STP_TYPE_USER_DEF_3
 */
extern int32
dal_longan_trap_bypassStp_set(uint32 unit, rtk_trap_bypassStpType_t frameType, rtk_enable_t enable);

/* Function Name:
 *      dal_longan_trap_bypassVlan_get
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
 *    - BYPASS_STP_TYPE_USER_DEF_0
 *    - BYPASS_STP_TYPE_USER_DEF_1
 *    - BYPASS_STP_TYPE_USER_DEF_2
 *    - BYPASS_STP_TYPE_USER_DEF_3
 */
extern int32
dal_longan_trap_bypassVlan_get(uint32 unit, rtk_trap_bypassVlanType_t frameType, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_longan_trap_bypassVlan_set
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
 *    - BYPASS_STP_TYPE_USER_DEF_0
 *    - BYPASS_STP_TYPE_USER_DEF_1
 *    - BYPASS_STP_TYPE_USER_DEF_2
 *    - BYPASS_STP_TYPE_USER_DEF_3
 */
extern int32
dal_longan_trap_bypassVlan_set(uint32 unit, rtk_trap_bypassVlanType_t frameType, rtk_enable_t enable);

/* Function Name:
 *      dal_longan_trap_userDefineRma_get
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
dal_longan_trap_userDefineRma_get(
    uint32                      unit,
    uint32                      userDefine_idx,
    rtk_trap_userDefinedRma_t   *pUserDefinedRma);

/* Function Name:
 *      dal_longan_trap_userDefineRma_set
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
dal_longan_trap_userDefineRma_set(
    uint32                      unit,
    uint32                      userDefine_idx,
    rtk_trap_userDefinedRma_t   *pUserDefinedRma);

/* Function Name:
 *      dal_longan_trap_userDefineRmaEnable_get
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
dal_longan_trap_userDefineRmaEnable_get(uint32 unit, uint32 userDefine_idx, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_longan_trap_userDefineRmaEnable_set
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
dal_longan_trap_userDefineRmaEnable_set(uint32 unit, uint32 userDefine_idx, rtk_enable_t enable);

/* Function Name:
 *      dal_longan_trap_userDefineRmaAction_get
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
 *      - MGMT_ACTION_FORWARD
 *      - MGMT_ACTION_TRAP2CPU
 *      - MGMT_ACTION_DROP
 */
extern int32
dal_longan_trap_userDefineRmaAction_get(uint32 unit, uint32 userDefine_idx, rtk_mgmt_action_t *pAction);

/* Function Name:
 *      dal_longan_trap_userDefineRmaAction_set
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
 *      - MGMT_ACTION_FORWARD
 *      - MGMT_ACTION_TRAP2CPU
 *      - MGMT_ACTION_DROP
 */
extern int32
dal_longan_trap_userDefineRmaAction_set(uint32 unit, uint32 userDefine_idx, rtk_mgmt_action_t action);

/* Function Name:
 *      dal_longan_trap_userDefineRmaLearningEnable_get
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
dal_longan_trap_userDefineRmaLearningEnable_get(uint32 unit, uint32 userDefine_idx, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_longan_trap_userDefineRmaLearningEnable_set
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
dal_longan_trap_userDefineRmaLearningEnable_set(uint32 unit, uint32 userDefine_idx, rtk_enable_t enable);



/* Function Name:
 *      dal_longan_trap_oamPDUAction_get
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
dal_longan_trap_oamPDUAction_get(uint32 unit, rtk_action_t *pAction);

/* Function Name:
 *      dal_longan_trap_oamPDUAction_set
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
dal_longan_trap_oamPDUAction_set(uint32 unit, rtk_action_t action);

/* Function Name:
 *      dal_longan_trap_portOamLoopbackParAction_get
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
dal_longan_trap_portOamLoopbackParAction_get(uint32 unit,
        rtk_port_t port, rtk_trap_oam_action_t *pAction);

/* Function Name:
 *      dal_longan_trap_portOamLoopbackParAction_set
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
dal_longan_trap_portOamLoopbackParAction_set(uint32 unit,
        rtk_port_t port, rtk_trap_oam_action_t action);

/*
 * Function Name:
 *      dal_longan_trap_oamTarget_get
 * Description:
 *      Get information of OAM PDU trap packet to local or master CPU.
 * Input:
 *      unit    - unit id
 * Output:
 *      pTarget - pointer to the information of OAM PDU trap packet
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Note:
 */
extern int32
dal_longan_trap_oamTarget_get(uint32 unit, rtk_trapTarget_t *pTarget);

/* Function Name:
 *      dal_longan_trap_oamTarget_set
 * Description:
 *      Set information of OAM PDU trap packet to local or master CPU.
 * Input:
 *      unit    - unit id
 *      target  - the status of OAM PDU trap packet
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 */
extern int32
dal_longan_trap_oamTarget_set(uint32 unit, rtk_trapTarget_t target);




/* Function Name:
 *      dal_longan_trap_mgmtFrameAction_get
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
 *                                          9300
 *          - MGMT_TYPE_L2_CRC_ERR          (1110000)
 *          - MGMT_TYPE_IP4_CHKSUM_ERR      (1110000)
 */
extern int32
dal_longan_trap_mgmtFrameAction_get(uint32 unit, rtk_trap_mgmtType_t frameType, rtk_mgmt_action_t *pAction);

/* Function Name:
 *      dal_longan_trap_mgmtFrameAction_set
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
 *                                          9300
 *          - MGMT_TYPE_L2_CRC_ERR          (1110000)
 *          - MGMT_TYPE_IP4_CHKSUM_ERR      (1110000)
 *          - MGMT_TYPE_RLDP_RLPP           (1010100)
 */
extern int32
dal_longan_trap_mgmtFrameAction_set(uint32 unit, rtk_trap_mgmtType_t frameType, rtk_mgmt_action_t action);

/*
 * Function Name:
 *      dal_longan_trap_mgmtFrameTarget_get
 * Description:
 *      Get information of management frame trap packet to local or master CPU.
 * Input:
 *      unit    - unit id
 * Output:
 *      pTarget - pointer to the information of management frame trap
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Note:
 *      None
 */
extern int32
dal_longan_trap_mgmtFrameTarget_get(uint32 unit, rtk_trapTarget_t *pTarget);

/* Function Name:
 *      dal_longan_trap_mgmtFrameTarget_set
 * Description:
 *      Set information of management frame trap packet to local or master CPU.
 * Input:
 *      unit    - unit id
 *      target  - the status of management frame trap
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_longan_trap_mgmtFrameTarget_set(uint32 unit, rtk_trapTarget_t target);

/* Function Name:
 *      dal_longan_trap_mgmtFrameQueue_get
 * Description:
 *      Get queue id of trapped packet.
 * Input:
 *      unit      - unit id
 *      qType - type of trapped packet
 * Output:
 *      pQid - pointer to queue id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_longan_trap_mgmtFrameQueue_get(uint32 unit, rtk_trap_qType_t qType, rtk_qid_t *pQid);

/* Function Name:
 *      dal_longan_trap_mgmtFrameQueue_set
 * Description:
 *      Set queue id of trapped packet.
 * Input:
 *      unit      - unit id
 *      qType - type of trapped packet
 *      qid      - qid
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_QUEUE_ID         - invalid queue id
 * Note:
 *      None
 */
extern int32
dal_longan_trap_mgmtFrameQueue_set(uint32 unit, rtk_trap_qType_t qType, rtk_qid_t qid);

/* Function Name:
 *      dal_longan_trap_mgmtFrameLearningEnable_get
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
 *      - MGMT_TYPE_EAPOL
 */
extern int32
dal_longan_trap_mgmtFrameLearningEnable_get(uint32 unit, rtk_trap_mgmtType_t frameType, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_longan_trap_mgmtFrameLearningEnable_set
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
 *      - MGMT_TYPE_EAPOL
 */
extern int32
dal_longan_trap_mgmtFrameLearningEnable_set(uint32 unit, rtk_trap_mgmtType_t frameType, rtk_enable_t enable);

/* Function Name:
 *      dal_longan_trap_portMgmtFrameAction_get
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
 *                                    	      9300
 *          - MGMT_TYPE_BPDU                (1110101)
 *          - MGMT_TYPE_PTP                 (1110100)
 *          - MGMT_TYPE_PTP_UDP             (1110100)
 *          - MGMT_TYPE_PTP_ETH2            (1110100)
 *          - MGMT_TYPE_LLDP                (1110101)
 *          - MGMT_TYPE_EAPOL               (1110101)
 *          - MGMT_GRATUITOUS_ARP           (1111000)
 *
 *          MGMT_TYPE_PTP is accepted by 9300 and 9310 only when BACKWARD-COMPATIBLE is configured.
 */
extern int32
dal_longan_trap_portMgmtFrameAction_get(uint32 unit, rtk_port_t port, rtk_trap_mgmtType_t frameType, rtk_mgmt_action_t *pAction);

/* Function Name:
 *      dal_longan_trap_portMgmtFrameAction_set
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
 *                                    	      9300
 *          - MGMT_TYPE_BPDU                (1110101)
 *          - MGMT_TYPE_PTP                 (1110100)
 *          - MGMT_TYPE_PTP_UDP             (1110100)
 *          - MGMT_TYPE_PTP_ETH2            (1110100)
 *          - MGMT_TYPE_LLDP                (1110101)
 *          - MGMT_TYPE_EAPOL               (1110101)
 *          - MGMT_GRATUITOUS_ARP           (1111000)
 *
 *          MGMT_TYPE_PTP is accepted by 9300 and 9310 only when BACKWARD-COMPATIBLE is configured.
 */
extern int32
dal_longan_trap_portMgmtFrameAction_set(uint32 unit, rtk_port_t port, rtk_trap_mgmtType_t frameType, rtk_mgmt_action_t action);

/* Function Name:
 *      dal_longan_trap_pktWithCFIAction_get
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
dal_longan_trap_pktWithCFIAction_get(uint32 unit, rtk_action_t *pAction);

/* Function Name:
 *      dal_longan_trap_pktWithCFIAction_set
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
dal_longan_trap_pktWithCFIAction_set(uint32 unit, rtk_action_t action);

/* Function Name:
 *      dal_longan_trap_pktWithOuterCFIAction_get
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
dal_longan_trap_pktWithOuterCFIAction_get(uint32 unit, rtk_action_t *pAction);

/* Function Name:
 *      dal_longan_trap_pktWithOuterCFIAction_set
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
dal_longan_trap_pktWithOuterCFIAction_set(uint32 unit, rtk_action_t action);

/* Function Name:
 *      dal_longan_trap_bpduFloodPortmask_get
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
dal_longan_trap_bpduFloodPortmask_get(uint32 unit, rtk_portmask_t *pflood_portmask);

/* Function Name:
 *      dal_longan_trap_bpduFloodPortmask_set
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
dal_longan_trap_bpduFloodPortmask_set(uint32 unit, rtk_portmask_t *pflood_portmask);

/* Function Name:
 *      dal_longan_trap_eapolFloodPortmask_get
 * Description:
 *      Get EAPOL flooding portmask.
 * Input:
 *      unit            - unit id
 *      pflood_portmask - eapol flooding portmask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 */
extern int32
dal_longan_trap_eapolFloodPortmask_get(uint32 unit, rtk_portmask_t *pflood_portmask);

/* Function Name:
 *      dal_longan_trap_eapolFloodPortmask_set
 * Description:
 *      Set EAPOL flooding portmask.
 * Input:
 *      unit            - unit id
 *      pflood_portmask - EAPOL flooding portmask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 */
extern int32
dal_longan_trap_eapolFloodPortmask_set(uint32 unit, rtk_portmask_t *pflood_portmask);

/* Function Name:
 *      dal_longan_trap_lldpFloodPortmask_get
 * Description:
 *      Get LLDP flooding portmask.
 * Input:
 *      unit            - unit id
 *      pflood_portmask - LLDP flooding portmask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 */
extern int32
dal_longan_trap_lldpFloodPortmask_get(uint32 unit, rtk_portmask_t *pflood_portmask);

/* Function Name:
 *      dal_longan_trap_lldpFloodPortmask_set
 * Description:
 *      Set LLDP flooding portmask.
 * Input:
 *      unit            - unit id
 *      pflood_portmask - LLDP flooding portmask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 */
extern int32
dal_longan_trap_lldpFloodPortmask_set(uint32 unit, rtk_portmask_t *pflood_portmask);

/* Function Name:
 *      dal_longan_trap_userDefineFloodPortmask_get
 * Description:
 *      Get user define flooding portmask.
 * Input:
 *      unit            - unit id
 *      pflood_portmask - user define flooding portmask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 */
extern int32
dal_longan_trap_userDefineFloodPortmask_get(uint32 unit, rtk_portmask_t *pflood_portmask);

/* Function Name:
 *      dal_longan_trap_userDefineFloodPortmask_set
 * Description:
 *      Set user define flooding portmask.
 * Input:
 *      unit            - unit id
 *      pflood_portmask - user define flooding portmask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 */
extern int32
dal_longan_trap_userDefineFloodPortmask_set(uint32 unit, rtk_portmask_t *pflood_portmask);

/* Function Name:
 *      dal_longan_trap_rmaFloodPortmask_get
 * Description:
 *      Get RMA flooding portmask.
 * Input:
 *      unit            - unit id
 *      pflood_portmask - rma flooding portmask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 */
extern int32
dal_longan_trap_rmaFloodPortmask_get(uint32 unit, rtk_portmask_t *pflood_portmask);

/* Function Name:
 *      dal_longan_trap_eapolFloodPortmask_set
 * Description:
 *      Set RMA flooding portmask.
 * Input:
 *      unit            - unit id
 *      pflood_portmask - rma  flooding portmask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 */
int32
dal_longan_trap_rmaFloodPortmask_set(uint32 unit, rtk_portmask_t *pflood_portmask);


/* Function Name:
 *      dal_longan_trap_rmaCancelMirror_get
 * Description:
 *      Get RMA Cancel mirror configuration
 * Input:
 *      unit            - unit id
 * Output:
 *      pEnable        - pointer to enable status of RMA Cancel Mirror Setting
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - Invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 */
extern int32
dal_longan_trap_rmaCancelMirror_get(uint32 unit,  rtk_enable_t *pEnable);

/* Function Name:
 *      dal_longan_trap_rmaCancelMirror_set
 * Description:
 *      Set enable status of RMA Cancel mirror
 * Input:
 *      unit            - unit id
 *      enable        - enable status of  RMA Cancel Mirror
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - Invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_longan_trap_rmaCancelMirror_set(uint32 unit,  rtk_enable_t enable);

#endif /* __DAL_longan_TRAP_H__ */

