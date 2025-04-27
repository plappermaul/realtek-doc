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
 * Purpose : Definition those public global APIs and its data type in the SDK.
 *
 * Feature : (1) Include chip-supported conditions for flow control on/off
 *           (2) Get/set the threshold parameters for the flow control on/off
 *
 */

#ifndef __DAL_CYPRESS_FLOWCTRL_H__
#define __DAL_CYPRESS_FLOWCTRL_H__


/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/flowctrl.h>
#include <dal/dal_mapper.h>


/*
 * Symbol Definition
 */
#define FLOWCTRL_HALF_BKPRES_MTHD       (0) /* JAM mode */


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
 *      dal_cypress_flowctrlMapper_init
 * Description:
 *      Hook flowctrl module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must Hook flowctrl module before calling any flowctrl APIs.
 */
extern int32
dal_cypress_flowctrlMapper_init(dal_mapper_t *pMapper);

/* Function Name:
 *      dal_cypress_flowctrl_init
 * Description:
 *      Initialize flowctrl module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      1. Module must be initialized before using all of APIs in this module
 */
extern int32
dal_cypress_flowctrl_init(uint32 unit);

/*
 * Flow Control ON
 */

/* Function Name:
 *      dal_cypress_flowctrl_portPauseOnAction_get
 * Description:
 *      Get action when packet keeps receiving after pause on frame is sent.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pAction - pointer to action of packet receive
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      Action of packet receive is as following
 *      - PAUSE_ON_RECEIVE
 *      - PAUSE_ON_DROP
 */
extern int32
dal_cypress_flowctrl_portPauseOnAction_get(
    uint32                          unit,
    rtk_port_t                      port,
    rtk_flowctrl_pauseOnAction_t    *pAction);

/* Function Name:
 *      dal_cypress_flowctrl_portPauseOnAction_set
 * Description:
 *      Set action when packet keeps receiving after pause on frame is sent.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      action - action of packet receive
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      Action of packet receive is as following
 *      - PAUSE_ON_RECEIVE
 *      - PAUSE_ON_DROP
 */
extern int32
dal_cypress_flowctrl_portPauseOnAction_set(
    uint32                          unit,
    rtk_port_t                      port,
    rtk_flowctrl_pauseOnAction_t    action);

/* Function Name:
 *      dal_cypress_flowctrl_pauseOnAllowedPktLen_get
 * Description:
 *      Get number of allowed packet length after pause on frame is sent.
 * Input:
 *      unit     - unit id
 *      port     - port id
 * Output:
 *      pPktLen  - pointer to number of received packet length (unit: bytes)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      The length unit is byte.
 */
extern int32
dal_cypress_flowctrl_pauseOnAllowedPktLen_get(uint32 unit, rtk_port_t port, uint32 *pPktLen);

/* Function Name:
 *      dal_cypress_flowctrl_pauseOnAllowedPktLen_set
 * Description:
 *      Set number of allowed packet length after pause on frame is sent.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      pktLen  - number of received packet length (unit: bytes)
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_OUT_OF_RANGE     - input parameter out of range
 * Note:
 *      The length unit is byte.
 */
extern int32
dal_cypress_flowctrl_pauseOnAllowedPktLen_set(uint32 unit, rtk_port_t port, uint32 pktLen);

/* Function Name:
 *      dal_cypress_flowctrl_pauseOnAllowedPktNum_get
 * Description:
 *      Get number of allowed packet when pause on frame sent.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pPktNum - pointer to number of received packet
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_flowctrl_pauseOnAllowedPktNum_get(uint32 unit, rtk_port_t port, uint32 *pPktNum);

/* Function Name:
 *      dal_cypress_flowctrl_pauseOnAllowedPktNum_set
 * Description:
 *      Set number of allowed packet when pause on frame sent.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      pktNum - number of received packet
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_OUT_OF_RANGE     - input parameter out of range
 * Note:
 *      None
 */
extern int32
dal_cypress_flowctrl_pauseOnAllowedPktNum_set(uint32 unit, rtk_port_t port, uint32 pktNum);

/* Function Name:
 *      dal_cypress_flowctrl_igrSystemPauseThresh_get
 * Description:
 *      Get ingress system used page high/low threshold paramters of the specific unit
 * Input:
 *      unit    - unit id
 * Output:
 *      pThresh - pointer to the threshold structure in the system used page count
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER  - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_flowctrl_igrSystemPauseThresh_get(uint32 unit, rtk_flowctrl_thresh_t *pThresh);

/* Function Name:
 *      dal_cypress_flowctrl_igrSystemPauseThresh_set
 * Description:
 *      Set ingress system used page high/low threshold paramters of the specific unit
 * Input:
 *      unit    - unit id
 *      pThresh - pointer to the threshold structure in the system used page count
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *      None
 */
extern int32
dal_cypress_flowctrl_igrSystemPauseThresh_set(uint32 unit, rtk_flowctrl_thresh_t *pThresh);

/* Function Name:
 *      dal_cypress_flowctrl_igrPauseThreshGroup_get
 * Description:
 *      Get ingress port used page high/low threshold for the specified threshold group
 * Input:
 *      unit    - unit id
 *      grp_idx - index of threshold group
 * Output:
 *      pThresh - pointer to the threshold structure for the port used page count
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_cypress_flowctrl_igrPauseThreshGroup_get(uint32 unit, uint32 grp_idx, rtk_flowctrl_thresh_t *pThresh);

/* Function Name:
 *      dal_cypress_flowctrl_igrPauseThreshGroup_set
 * Description:
 *      Set ingress port used page high/low threshold for the specified threshold group
 * Input:
 *      unit    - unit id
 *      grp_idx - index of threshold group
 *      pThresh - pointer to the threshold structure for the port used page count
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      lowOff thresholds is unused.
 */
extern int32
dal_cypress_flowctrl_igrPauseThreshGroup_set(uint32 unit, uint32 grp_idx, rtk_flowctrl_thresh_t *pThresh);

/* Function Name:
 *      dal_cypress_flowctrl_portIgrPortThreshGroupSel_get
 * Description:
 *      Get ingress port used page pause and drop threshold group for the specified port
 * Input:
 *      unit     - unit id
 *      port     - port id
 * Output:
 *      pGrp_idx - pointer to the index of threshold group
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_flowctrl_portIgrPortThreshGroupSel_get(uint32 unit, rtk_port_t port, uint32 *pGrp_idx);

/* Function Name:
 *      dal_cypress_flowctrl_portIgrPortThreshGroupSel_set
 * Description:
 *      Set ingress port used page pause and congest threshold group for the specified port
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      grp_idx - index of threshold group
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_cypress_flowctrl_portIgrPortThreshGroupSel_set(uint32 unit, rtk_port_t port, uint32 grp_idx);

/*
 * Flow Control OFF
 */

/* Function Name:
 *      dal_cypress_flowctrl_igrSystemCongestThresh_get
 * Description:
 *      Get system used page high/low drop threshold paramters of the specific unit
 * Input:
 *      unit    - unit id
 * Output:
 *      pThresh - pointer to the threshold structure in the public used page count
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_flowctrl_igrSystemCongestThresh_get(uint32 unit, rtk_flowctrl_thresh_t *pThresh);

/* Function Name:
 *      dal_cypress_flowctrl_igrSystemCongestThresh_set
 * Description:
 *      Set system used page high/low threshold paramters of the specific unit
 * Input:
 *      unit    - unit id
 *      pThresh - pointer to the threshold structure in the public used page count
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *      None
 */
extern int32
dal_cypress_flowctrl_igrSystemCongestThresh_set(uint32 unit, rtk_flowctrl_thresh_t *pThresh);

/* Function Name:
 *      dal_cypress_flowctrl_igrCongestThreshGroup_get
 * Description:
 *      Get used page high drop threshold for the specified threahold group
 * Input:
 *      unit     - unit id
 *      grp_idx  - the index of threshold group
 * Output:
 *      pThresh - pointer to the threshold structure for the port used page count
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Support in RTL8390 only
 */
extern int32
dal_cypress_flowctrl_igrCongestThreshGroup_get(uint32 unit, uint32 grp_idx, rtk_flowctrl_thresh_t *pThresh);

/* Function Name:
 *      dal_cypress_flowctrl_igrCongestThreshGroup_set
 * Description:
 *      Set used page high drop threshold for the specified threahold group
 * Input:
 *      unit    - unit id
 *      grp_idx - the index of threshold group
 *      pThresh - pointer to the threshold structure for the port used page count
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      1. Support in RTL8390 only
 *      2. For RTL8390 lowOn and lowOff threshold should be set to the same value,
 *         otherwise RT_ERR_INPUT will be returned
 */
extern int32
dal_cypress_flowctrl_igrCongestThreshGroup_set(uint32 unit, uint32 grp_idx, rtk_flowctrl_thresh_t *pThresh);

/* Function Name:
 *      dal_cypress_flowctrl_egrSystemDropThresh_get
 * Description:
 *      Get egress system drop threshold for the specified unit
 * Input:
 *      unit    - unit id
 * Output:
 *      pThresh - pointer to the drop threshold
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER       - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_flowctrl_egrSystemDropThresh_get(uint32 unit, rtk_flowctrl_drop_thresh_t *pThresh);

/* Function Name:
 *      dal_cypress_flowctrl_egrSystemDropThresh_set
 * Description:
 *      Set egress drop threshold for the specified egress port
 * Input:
 *      unit    - unit id
 *      pThresh - pointer to the drop threshold
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER       - input parameter may be null pointer
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 * Note:
 *      None
 */
extern int32
dal_cypress_flowctrl_egrSystemDropThresh_set(uint32 unit, rtk_flowctrl_drop_thresh_t *pThresh);

/* Function Name:
 *      dal_cypress_flowctrl_egrPortDropThresh_get
 * Description:
 *      Get egress drop threshold for the specified egress port
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pThresh - pointer to the drop threshold
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_flowctrl_egrPortDropThresh_get(uint32 unit, rtk_port_t port, rtk_flowctrl_drop_thresh_t *pThresh);

/* Function Name:
 *      dal_cypress_flowctrl_egrPortDropThresh_set
 * Description:
 *      Set egress drop threshold for the specified egress port
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      pThresh - pointer to the drop threshold
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *      Low threshold is unused.
 */
extern int32
dal_cypress_flowctrl_egrPortDropThresh_set(uint32 unit, rtk_port_t port, rtk_flowctrl_drop_thresh_t *pThresh);

/* Function Name:
 *      dal_cypress_flowctrl_portEgrDropRefCongestEnable_get
 * Description:
 *      Get enable status of refering source port congest status for egress drop
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to enable status of refering source port congest status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_flowctrl_portEgrDropRefCongestEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_cypress_flowctrl_portEgrDropRefCongestEnable_set
 * Description:
 *      Set enable status of refering source port congest status for egress drop
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - enable status of refering source port congest status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_cypress_flowctrl_portEgrDropRefCongestEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      dal_cypress_flowctrl_egrPortQueueDropEnable_get
 * Description:
 *      Get egress queue drop ability for the specified port
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - status of egress queue drop ability
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_flowctrl_egrPortQueueDropEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_cypress_flowctrl_egrPortQueueDropEnable_set
 * Description:
 *      Set egress queue drop ability for the specified port
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      enable  - status of egress queue drop ability
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_cypress_flowctrl_egrPortQueueDropEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      dal_cypress_flowctrl_egrQueueDropThresh_get
 * Description:
 *      Get egress global drop threshold for the egress queue
 * Input:
 *      unit    - unit id
 *      queue   - queue id
 * Output:
 *      pThresh - pointer to the global drop threshold
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_QUEUE_ID     - invalid queue id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_flowctrl_egrQueueDropThresh_get(uint32 unit, rtk_qid_t queue, rtk_flowctrl_drop_thresh_t *pThresh);

/* Function Name:
 *      dal_cypress_flowctrl_egrQueueDropThresh_set
 * Description:
 *      Set egress gloabl drop threshold for the egress queue
 * Input:
 *      unit    - unit id
 *      queue   - queue id
 *      pThresh - pointer to the drop threshold
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_QUEUE_ID     - invalid queue id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *      None
 */
extern int32
dal_cypress_flowctrl_egrQueueDropThresh_set(uint32 unit, rtk_qid_t queue, rtk_flowctrl_drop_thresh_t *pThresh);

/* Function Name:
 *      dal_cypress_flowctrl_egrCpuQueueDropThresh_get
 * Description:
 *      Get egress global drop threshold for the egress queue of CPU port
 * Input:
 *      unit    - unit id
 *      queue   - queue id
 * Output:
 *      pThresh - pointer to the global drop threshold
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_QUEUE_ID     - invalid queue id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_flowctrl_egrCpuQueueDropThresh_get(uint32 unit, rtk_qid_t queue, rtk_flowctrl_drop_thresh_t *pThresh);

/* Function Name:
 *      dal_cypress_flowctrl_egrCpuQueueDropThresh_set
 * Description:
 *      Set egress gloabl drop threshold for the egress queue of CPU port
 * Input:
 *      unit    - unit id
 *      queue   - queue id
 *      pThresh - pointer to the drop threshold
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_QUEUE_ID     - invalid queue id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *      None
 */
extern int32
dal_cypress_flowctrl_egrCpuQueueDropThresh_set(uint32 unit, rtk_qid_t queue, rtk_flowctrl_drop_thresh_t *pThresh);

/* Function Name:
 *      dal_cypress_flowctrl_portHolTrafficDropEnable_get
 * Description:
 *      Get dropping ability for dropping flooding traffic when flow control is enabled.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - enable state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) Per ingress port can enable the drop function to drop flooding traffic.
 *      (2) The function takes effect only if the flow control of ingress port is enabled.
 *      (3) Refer to rtk_flowctrl_holTrafficTypeDropEnable_set for dropping specific traffic type.
 */
extern int32
dal_cypress_flowctrl_portHolTrafficDropEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_cypress_flowctrl_portHolTrafficDropEnable_set
 * Description:
 *      Set dropping ability for dropping flooding traffic when flow control is enabled.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      enable  - enable state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      (1) Enable the function to prevent HOL by flooding traffic when flow control is enabled.
 *      (2) Per ingress port can enable the drop function to drop flooding traffic at the congested
 *          egress port.
 *      (3) Refer to rtk_flowctrl_holTrafficTypeDropEnable_set for dropping specific traffic type.
 */
extern int32
dal_cypress_flowctrl_portHolTrafficDropEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      dal_cypress_flowctrl_holTrafficTypeDropEnable_get
 * Description:
 *      Get dropping ability for specific traffic type when flow control is enabled.
 * Input:
 *      unit    - unit id
 *      type    - traffic type
 * Output:
 *      pEnable - enable state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) Each traffic type can enable the drop function individually.
 *      (2) The function takes effect if rtk_flowctrl_portHolTrafficDropEnable_set is enabled.
 */
extern int32
dal_cypress_flowctrl_holTrafficTypeDropEnable_get(uint32 unit, rtk_flowctrl_holTrafficType_t type, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_cypress_flowctrl_holTrafficTypeDropEnable_set
 * Description:
 *      Set dropping ability for specific traffic type when flow control is enabled.
 * Input:
 *      unit    - unit id
 *      type    - traffic type
 *      enable  - enable state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      (1) Each traffic type can enable the drop function individually.
 *      (2) The function takes effect if rtk_flowctrl_portHolTrafficDropEnable_set is enabled.
 */
extern int32
dal_cypress_flowctrl_holTrafficTypeDropEnable_set(uint32 unit, rtk_flowctrl_holTrafficType_t type, rtk_enable_t enable);

/* Function Name:
 *      dal_cypress_flowctrl_specialCongestThreshold_get
 * Description:
 *      Get special congstion threshold
 * Input:
 *      unit    - unit id
 * Output:
 *      pThresh - pointer to the threshold structure for special congstion
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_flowctrl_specialCongestThreshold_get(uint32 unit, rtk_flowctrl_drop_thresh_t *pThresh);

/* Function Name:
 *      dal_cypress_flowctrl_specialCongestThreshold_set
 * Description:
 *      Set special congstion threshold
 * Input:
 *      unit    - unit id
 *      pThresh - the pointer to threshold structure for special congstion
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *      None
 */
int32
dal_cypress_flowctrl_specialCongestThreshold_set(uint32 unit, rtk_flowctrl_drop_thresh_t *pThresh);

#endif /* __DAL_CYPRESS_FLOWCTRL_H__ */

