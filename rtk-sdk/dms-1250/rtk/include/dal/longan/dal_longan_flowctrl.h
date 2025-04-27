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
 * Purpose : Definition of Flow Control API
 *
 * Feature : Configure Threshold Parameters of Flow Control
 *
 */

#ifndef __DAL_LONGAN_FLOWCTRL_H__
#define __DAL_LONGAN_FLOWCTRL_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/flowctrl.h>
#include <dal/dal_mapper.h>

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
 *      dal_longan_flowctrlMapper_init
 * Description:
 *      Hook flowctrl module of the specified device.
 * Input:
 *      pMapper - pointer of mapper
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      Must Hook flowctrl module before calling any flowctrl APIs.
 */
extern int32
dal_longan_flowctrlMapper_init(dal_mapper_t *pMapper);

/* Function Name:
 *      dal_longan_flowctrl_init
 * Description:
 *      Initialize flowctrl module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      Must initialize flowctrl module before calling any flowctrl APIs.
 */
extern int32
dal_longan_flowctrl_init(uint32 unit);

/* Module Name    : Flow Control               */
/* Sub-module Name: Flow control configuration */

/* Function Name:
 *      dal_longan_flowctrl_portPauseOnAction_get
 * Description:
 *      Get action for the scenario that packet keeps coming in after pause on frame is sent.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pAction - pointer to action of packet receive
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Action of packet receive is as following
 *      - PAUSE_ON_RECEIVE: after Pause ON frame is sent, incoming packets always can be received.
 *      - PAUSE_ON_DROP: after Pause ON frame is sent, incoming packets can be received up to one specified number.
 */
extern int32
dal_longan_flowctrl_portPauseOnAction_get(
    uint32                          unit,
    rtk_port_t                      port,
    rtk_flowctrl_pauseOnAction_t    *pAction);

/* Function Name:
 *      dal_longan_flowctrl_portPauseOnAction_set
 * Description:
 *      Set action for the scenario that packet keeps coming in after pause on frame is sent.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      action - action of packet receive
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      Action of packet receive is as following
 *      - PAUSE_ON_RECEIVE: after Pause ON frame is sent, incoming packets always can be received.
 *      - PAUSE_ON_DROP: after Pause ON frame is sent, incoming packets can be received up to one specified number.
 *        The specified number can be configured by rtk_flowctrl_pauseOnAllowedPageNum_set,
 *        rtk_flowctrl_pauseOnAllowedPktNum_set, rtk_flowctrl_pauseOnAllowedPktLen_set APIs
 */
extern int32
dal_longan_flowctrl_portPauseOnAction_set(
    uint32                          unit,
    rtk_port_t                      port,
    rtk_flowctrl_pauseOnAction_t    action);

/* Function Name:
 *      dal_longan_flowctrl_portPauseOnAllowedPageNum_get
 * Description:
 *      Get number of allowed page when pause on frame sent.
 * Input:
 *      unit     - unit id
 *      port     - port id
 * Output:
 *      pPageNum - pointer to number of received page
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) The setting is active if pause on action is PAUSE_ON_DROP, refer to rtk_flowctrl_pauseOnAction_set.
 *      (2) The valid range of pageNum is 0 ~ 0xFF, mean allowed receive page number after pause on frame sent.
 */
extern int32
dal_longan_flowctrl_portPauseOnAllowedPageNum_get(uint32 unit, rtk_port_t port, uint32 *pPageNum);

/* Function Name:
 *      dal_longan_flowctrl_portPauseOnAllowedPageNum_set
 * Description:
 *      Set number of allowed page when pause on frame sent.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      pageNum - number of received page
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *      (1) The setting is active if pause on action is PAUSE_ON_DROP, refer to rtk_flowctrl_pauseOnAction_set.
 *      (2) The valid range of pageNum is 0 ~ 0xFF, mean allowed receive page number after pause on frame sent.
 */
extern int32
dal_longan_flowctrl_portPauseOnAllowedPageNum_set(uint32 unit, rtk_port_t port, uint32 pageNum);

/*
 * Flow Control ON
 */

/* Function Name:
 *      dal_longan_flowctrl_igrGuarEnable_get
 * Description:
 *      Get state of ingress guarantee page for all ports
 * Input:
 *      unit    - unit id
 * Output:
 *      pEnable - pointer to enable status of guarantee page
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_longan_flowctrl_igrGuarEnable_get(uint32 unit, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_longan_flowctrl_igrGuarEnable_set
 * Description:
 *      Set state of ingress guarantee page for all ports
 * Input:
 *      unit    - unit id
 *      enable  - enable status of guarantee page
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 * Note:
 *      None
 */
extern int32
dal_longan_flowctrl_igrGuarEnable_set(uint32 unit, rtk_enable_t enable);

/* Function Name:
 *      dal_longan_flowctrl_igrSystemPauseThresh_get
 * Description:
 *      Get ingress system used page high/low threshold paramters of the specific unit
 * Input:
 *      unit    - unit id
 * Output:
 *      pThresh - pointer to the threshold structure in the system used page count
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) There are four fields in rtk_flowctrl_thresh_t: highOn, highOff, lowOn, lowOff
 *      (2) The four field value are threshold of public page count in system.
 *      (3) The four field value should be highOn > highOff > lowOn > lowOff
 *      (4) The valid range is 0 ~ 0x7FF in 8380, 0 ~ 0xFFF in 9300 and 8390
 */
extern int32
dal_longan_flowctrl_igrSystemPauseThresh_get(uint32 unit, rtk_flowctrl_thresh_t *pThresh);

/* Function Name:
 *      dal_longan_flowctrl_igrSystemPauseThresh_set
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
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *      (1) There are four fields in rtk_flowctrl_thresh_t: highOn, highOff, lowOn, lowOff
 *      (2) The four field value are threshold of public page count in system.
 *      (3) The four field value should be highOn > highOff > lowOn > lowOff
 *      (4) The valid range is 0 ~ 0x7FF in 8380, 0 ~ 0xFFF in 9300 and 8390
 */
extern int32
dal_longan_flowctrl_igrSystemPauseThresh_set(uint32 unit, rtk_flowctrl_thresh_t *pThresh);

/* Function Name:
 *      dal_longan_flowctrl_igrJumboSystemPauseThresh_get
 * Description:
 *      Get ingress system used page high/low threshold paramters of the specific unit for jumbo mode
 * Input:
 *      unit    - unit id
 * Output:
 *      pThresh - pointer to the threshold structure in the system used page count
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) There are four fields in rtk_flowctrl_thresh_t: highOn, highOff, lowOn, lowOff
 *      (2) The four field value are threshold of public page count in system.
 *      (3) The four field value should be highOn > highOff > lowOn > lowOff
 *      (4) The valid range is 0 ~ 0x7FF in 8380, 0 ~ 0xFFF in 9300 and 8390
 */
extern int32
dal_longan_flowctrl_igrJumboSystemPauseThresh_get(uint32 unit, rtk_flowctrl_thresh_t *pThresh);

/* Function Name:
 *      dal_longan_flowctrl_igrJumboSystemPauseThresh_set
 * Description:
 *      Set ingress system used page high/low threshold paramters of the specific unit for jumbo mode
 * Input:
 *      unit    - unit id
 *      pThresh - pointer to the threshold structure in the system used page count
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *      (1) There are four fields in rtk_flowctrl_thresh_t: highOn, highOff, lowOn, lowOff
 *      (2) The four field value are threshold of public page count in system.
 *      (3) The four field value should be highOn > highOff > lowOn > lowOff
 *      (4) The valid range is 0 ~ 0x7FF in 8380, 0 ~ 0xFFF in 9300 and 8390
 */
extern int32
dal_longan_flowctrl_igrJumboSystemPauseThresh_set(uint32 unit, rtk_flowctrl_thresh_t *pThresh);

/* Function Name:
 *      dal_longan_flowctrl_igrPauseThreshGroup_get
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
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      (1) Each port can map to a threshold group, refer to rtk_flowctrl_portIgrPortThreshGroupSel_set.
 */
extern int32
dal_longan_flowctrl_igrPauseThreshGroup_get(uint32 unit, uint32 grp_idx, rtk_flowctrl_thresh_t *pThresh);

/* Function Name:
 *      dal_longan_flowctrl_igrPauseThreshGroup_set
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
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      (1) Each port can map to a threshold group, refer to rtk_flowctrl_portIgrPortThreshGroupSel_set.
 */
extern int32
dal_longan_flowctrl_igrPauseThreshGroup_set(uint32 unit, uint32 grp_idx, rtk_flowctrl_thresh_t *pThresh);

/* Function Name:
 *      dal_longan_flowctrl_portIgrPortThreshGroupSel_get
 * Description:
 *      Get ingress port used page pause threshold group for the specified port
 * Input:
 *      unit     - unit id
 *      port     - port id
 * Output:
 *      pGrp_idx - pointer to the index of threshold group
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_longan_flowctrl_portIgrPortThreshGroupSel_get(uint32 unit, rtk_port_t port, uint32 *pGrp_idx);

/* Function Name:
 *      dal_longan_flowctrl_portIgrPortThreshGroupSel_set
 * Description:
 *      Set ingress port used page pause threshold group for the specified port
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      grp_idx - index of threshold group
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_longan_flowctrl_portIgrPortThreshGroupSel_set(uint32 unit, rtk_port_t port, uint32 grp_idx);


/* Module Name    : Flow Control              */
/* Sub-module Name: Egress drop configuration */

/*
 * Flow Control OFF
 */

/* Function Name:
 *      dal_longan_flowctrl_igrSystemCongestThresh_get
 * Description:
 *      Get system used page high/low drop threshold paramters of the specific unit
 * Input:
 *      unit    - unit id
 * Output:
 *      pThresh - pointer to the threshold structure in the public used page count
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) There are four fields in rtk_flowctrl_thresh_t: highOn, highOff, lowOn, lowOff
 *      (2) The four field value are threshold of public page count in system.
 *      (3) The four field value should be highOn > highOff > lowOn > lowOff
 *      (4) The valid range is 0 ~ 0x7FF in 8380, 0 ~ 0xFFF in 8390 and 9300.
 */
extern int32
dal_longan_flowctrl_igrSystemCongestThresh_get(uint32 unit, rtk_flowctrl_thresh_t *pThresh);

/* Function Name:
 *      dal_longan_flowctrl_igrSystemCongestThresh_set
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
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *      (1) There are four fields in rtk_flowctrl_thresh_t: highOn, highOff, lowOn, lowOff
 *      (2) The four field value are threshold of public page count in system.
 *      (3) The four field value should be highOn > highOff > lowOn > lowOff
 *      (4) The valid range is 0 ~ 0x7FF in 8380, 0 ~ 0xFFF in 8390 and 9300.
 */
extern int32
dal_longan_flowctrl_igrSystemCongestThresh_set(uint32 unit, rtk_flowctrl_thresh_t *pThresh);

/* Function Name:
 *      dal_longan_flowctrl_igrJumboSystemCongestThresh_get
 * Description:
 *      Get system used page high/low drop threshold paramters of the specific unit for jumbo mode
 * Input:
 *      unit    - unit id
 * Output:
 *      pThresh - pointer to the threshold structure in the public used page count
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) There are four fields in rtk_flowctrl_thresh_t: highOn, highOff, lowOn, lowOff
 *      (2) The four field value are threshold of public page count in system.
 *      (3) The four field value should be highOn > highOff > lowOn > lowOff
 *      (4) The valid range is 0 ~ 0x7FF in 8380, 0 ~ 0xFFF in 8390 and 9300.
 */
extern int32
dal_longan_flowctrl_igrJumboSystemCongestThresh_get(uint32 unit, rtk_flowctrl_thresh_t *pThresh);

/* Function Name:
 *      dal_longan_flowctrl_igrJumboSystemCongestThresh_set
 * Description:
 *      Set system used page high/low threshold paramters of the specific unit for jumbo mode
 * Input:
 *      unit    - unit id
 *      pThresh - pointer to the threshold structure in the public used page count
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *      (1) There are four fields in rtk_flowctrl_thresh_t: highOn, highOff, lowOn, lowOff
 *      (2) The four field value are threshold of public page count in system.
 *      (3) The four field value should be highOn > highOff > lowOn > lowOff
 *      (4) The valid range is 0 ~ 0x7FF in 8380, 0 ~ 0xFFF in 8390 and 9300.
 */
extern int32
dal_longan_flowctrl_igrJumboSystemCongestThresh_set(uint32 unit, rtk_flowctrl_thresh_t *pThresh);


/* Function Name:
 *      dal_longan_flowctrl_igrCongestThreshGroup_get
 * Description:
 *      Get used page drop threshold for the specified threshold group
 * Input:
 *      unit    - unit id
 *      grp_idx - the index of threshold group
 * Output:
 *      pThresh - pointer to the threshold structure for the port used page count
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (2) Each port can map to a threshold group, refer to rtk_flowctrl_portIgrPortThreshGroupSel_set.
 */
extern int32
dal_longan_flowctrl_igrCongestThreshGroup_get(uint32 unit, uint32 grp_idx, rtk_flowctrl_thresh_t *pThresh);

/* Function Name:
 *      dal_longan_flowctrl_igrCongestThreshGroup_set
 * Description:
 *      Set used page drop threshold for the specified threshold group
 * Input:
 *      unit    - unit id
 *      grp_idx - the index of threshold group
 *      pThresh - pointer to the threshold structure for the port used page count
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      (2) Each port can map to a threshold group, refer to rtk_flowctrl_portIgrPortThreshGroupSel_set.
 */
extern int32
dal_longan_flowctrl_igrCongestThreshGroup_set(uint32 unit, uint32 grp_idx, rtk_flowctrl_thresh_t *pThresh);

/* Function Name:
 *      dal_longan_flowctrl_jumboModeStatus_get
 * Description:
 *      Get jumbo mode current status
 * Input:
 *      unit    - unit id
 * Output:
 *      pStatus - pointer to jumbo mode status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_longan_flowctrl_jumboModeStatus_get(uint32 unit, uint32 *pStatus);

/* Function Name:
 *      dal_longan_flowctrl_jumboModeEnable_get
 * Description:
 *      Get jumbo mode ability
 * Input:
 *      unit    - unit id
 * Output:
 *      pEnable - pointer to enable status of jumbo mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_longan_flowctrl_jumboModeEnable_get(uint32 unit, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_longan_flowctrl_jumboModeEnable_set
 * Description:
 *      Set enable status of jumbo mode.
 * Input:
 *      unit    - unit id
 *      enable  - enable status of jumbo mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_longan_flowctrl_jumboModeEnable_set(uint32 unit, rtk_enable_t enable);

/* Function Name:
 *      dal_longan_flowctrl_jumboModeLength_get
 * Description:
 *      Get jumbo mode packet length boundary
 * Input:
 *      unit    - unit id
 * Output:
 *      pLength - pointer to packet length boundary of jumbo mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_longan_flowctrl_jumboModeLength_get(uint32 unit, uint32 *pLength);

/* Function Name:
 *      dal_longan_flowctrl_jumboModeLength_set
 * Description:
 *      Set packet length boundary of jumbo mode.
 * Input:
 *      unit    - unit id
 *      length  - packet length boundary of jumbo mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_OUT_OF_RANGE - invalid packet length
 * Note:
 *      None
 */
extern int32
dal_longan_flowctrl_jumboModeLength_set(uint32 unit, uint32 length);

/* Function Name:
 *      dal_longan_flowctrl_egrSystemUtilThresh_get
 * Description:
 *      Get egress system utilization threshold for the specified unit
 * Input:
 *      unit    - unit id
 * Output:
 *      pThresh - pointer to the utilization threshold
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      System only uses highOn threshold of rtk_flowctrl_thresh_t.
 */
extern int32
dal_longan_flowctrl_egrSystemUtilThresh_get(uint32 unit, rtk_flowctrl_thresh_t *pThresh);

/* Function Name:
 *      dal_longan_flowctrl_egrSystemUtilThresh_set
 * Description:
 *      Set egress system utilization threshold for the specified unit
 * Input:
 *      unit    - unit id
 *      pThresh - pointer to the utilization threshold
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *      System only uses highOn threshold of rtk_flowctrl_thresh_t.
 */
extern int32
dal_longan_flowctrl_egrSystemUtilThresh_set(uint32 unit, rtk_flowctrl_thresh_t *pThresh);

/* Function Name:
 *      dal_longan_flowctrl_egrSystemDropThresh_get
 * Description:
 *      Get egress system drop threshold for the specified unit
 * Input:
 *      unit    - unit id
 * Output:
 *      pThresh - pointer to the drop threshold
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) There are two fields in rtk_flowctrl_drop_thresh_t: high, low
 *      (2) The two field value are threshold of public page count in system.
 *      (3) The two field value should be high > low
 *      (4) The valid range is 0 ~ 0xFFF in 8390 and 9300.
 *      (5) The 8390 and 9300 has only one threshold, mean high and low are equal.
 */
extern int32
dal_longan_flowctrl_egrSystemDropThresh_get(uint32 unit, rtk_flowctrl_drop_thresh_t *pThresh);

/* Function Name:
 *      dal_longan_flowctrl_egrSystemDropThresh_set
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
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *      (1) There are two fields in rtk_flowctrl_drop_thresh_t: high, low
 *      (2) The two field value are threshold of public page count in system.
 *      (3) The two field value should be high > low
 *      (4) The valid range is 0 ~ 0xFFF in 8390 and 9300..
 *      (5) The 8390 and 9300 has only one threshold, mean high and low should be set to the same value.
 */
extern int32
dal_longan_flowctrl_egrSystemDropThresh_set(uint32 unit, rtk_flowctrl_drop_thresh_t *pThresh);

/* Function Name:
 *      dal_longan_flowctrl_egrQueueDropThresh_get
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
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_QUEUE_ID     - invalid queue id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) There are two fields in rtk_flowctrl_drop_thresh_t: high, low
 *      (2) The two field value are threshold of public page count in the queue of all ports.
 *      (3) The two field value should be high > low
 *      (4) The valid range is 0 ~ 0xFFF in 8390
 */
extern int32
dal_longan_flowctrl_egrQueueDropThresh_get(uint32 unit, rtk_qid_t queue, rtk_flowctrl_drop_thresh_t *pThresh);

/* Function Name:
 *      dal_longan_flowctrl_egrQueueDropThresh_set
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
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_QUEUE_ID     - invalid queue id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *      (1) There are two fields in rtk_flowctrl_drop_thresh_t: high, low
 *      (2) The two field value are threshold of public page count in system.
 *      (3) The two field value should be high > low
 *      (4) The valid range is 0 ~ 0xFFF in 8390
 */
extern int32
dal_longan_flowctrl_egrQueueDropThresh_set(uint32 unit, rtk_qid_t queue, rtk_flowctrl_drop_thresh_t *pThresh);

/* Function Name:
 *      dal_longan_flowctrl_egrCpuQueueDropThresh_get
 * Description:
 *      Get CPU port egress queue drop threshold
 * Input:
 *      unit    - unit id
 *      queue   - queue id
 * Output:
 *      pThresh - pointer to the global drop threshold
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_QUEUE_ID     - invalid queue id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) CPU port has dedicated egress queue drop threshold configuration different from normal port.
 */
extern int32
dal_longan_flowctrl_egrCpuQueueDropThresh_get(uint32 unit, rtk_qid_t queue, rtk_flowctrl_drop_thresh_t *pThresh);

/* Function Name:
 *      dal_longan_flowctrl_egrCpuQueueDropThresh_set
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
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_QUEUE_ID     - invalid queue id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *      (1) CPU port has dedicated egress queue drop threshold configuration different from normal port.
 */
extern int32
dal_longan_flowctrl_egrCpuQueueDropThresh_set(uint32 unit, rtk_qid_t queue, rtk_flowctrl_drop_thresh_t *pThresh);

/* Function Name:
 *      dal_longan_flowctrl_egrPortDropRefCongestEnable_get
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
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_longan_flowctrl_portEgrDropRefCongestEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_longan_flowctrl_portEgrDropRefCongestEnable_set
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
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_longan_flowctrl_portEgrDropRefCongestEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      dal_longan_flowctrl_egrQueueDropThreshGroup_get
 * Description:
 *      Get egress queue drop threshold for the specified threshold group
 * Input:
 *      unit    - unit id
 *      grp_idx - index of threshold group
 *      queue   - queue id
 * Output:
 *      pThresh - pointer to the threshold structure for the queue used page count
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_QUEUE_ID     - invalid queue id
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) Each port can map to a threshold group, refer to rtk_flowctrl_portEgrDropThreshGroupSel_set.
 */
extern int32
dal_longan_flowctrl_egrQueueDropThreshGroup_get(uint32 unit, uint32 grp_idx, rtk_qid_t queue, rtk_flowctrl_drop_thresh_t *pThresh);

/* Function Name:
 *      dal_longan_flowctrl_egrQueueDropThreshGroup_set
 * Description:
 *      Set egress queue drop threshold for the specified threshold group
 * Input:
 *      unit    - unit id
 *      grp_idx - index of threshold group
 *      queue   - queue id
 *      pThresh - pointer to the threshold structure for the queue used page count
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_QUEUE_ID     - invalid queue id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) Each port can map to a threshold group, refer to rtk_flowctrl_portEgrDropThreshGroupSel_set.
 */
extern int32
dal_longan_flowctrl_egrQueueDropThreshGroup_set(uint32 unit, uint32 grp_idx, rtk_qid_t queue, rtk_flowctrl_drop_thresh_t *pThresh);

/* Function Name:
 *      dal_longan_flowctrl_portEgrDropThreshGroupSel_get
 * Description:
 *      Get egress port&queue used page drop threshold group for the specified port
 * Input:
 *      unit     - unit id
 *      port     - port id
 * Output:
 *      pGrp_idx - pointer to the index of threshold group
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_longan_flowctrl_portEgrDropThreshGroupSel_get(uint32 unit, rtk_port_t port, uint32 *pGrp_idx);

/* Function Name:
 *      dal_longan_flowctrl_portEgrDropThreshGroupSel_set
 * Description:
 *      Set egress port&queue used page drop threshold group for the specified port
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      grp_idx - index of threshold group
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_longan_flowctrl_portEgrDropThreshGroupSel_set(uint32 unit, rtk_port_t port, uint32 grp_idx);

/* Function Name:
 *      dal_longan_flowctrl_portEgrQueueDropForceEnable_get
 * Description:
 *      Get enable status of egress queue force drop.
 * Input:
 *      unit    - unit id
 *      port    - queue id
 *      queue   - queue id
 * Output:
 *      pEnable - pointer to enable status of egress queue force drop.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_QUEUE_ID     - invalid queue id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_longan_flowctrl_portEgrQueueDropForceEnable_get(uint32 unit, rtk_port_t port, rtk_qid_t queue, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_longan_flowctrl_portEgrQueueDropForceEnable_set
 * Description:
 *      Set enable status of egress queue force drop.
 * Input:
 *      unit   - unit id
 *      port   - queue id
 *      queue  - queue id
 *      enable - enable status of egress queue force drop.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_QUEUE_ID - invalid queue id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *     None
 */
extern int32
dal_longan_flowctrl_portEgrQueueDropForceEnable_set(uint32 unit, rtk_port_t port, rtk_qid_t queue, rtk_enable_t enable);

/* Function Name:
 *      dal_longan_flowctrl_portHolTrafficDropEnable_get
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
 *      RT_ERR_UNIT_ID      - invalid unit id
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
dal_longan_flowctrl_portHolTrafficDropEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_longan_flowctrl_portHolTrafficDropEnable_set
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
 *      RT_ERR_UNIT_ID      - invalid unit id
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
dal_longan_flowctrl_portHolTrafficDropEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      dal_longan_flowctrl_holTrafficTypeDropEnable_get
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
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) Each traffic type can enable the drop function individually.
 *      (2) The function takes effect if rtk_flowctrl_portHolTrafficDropEnable_set is enabled.
 */
extern int32
dal_longan_flowctrl_holTrafficTypeDropEnable_get(uint32 unit, rtk_flowctrl_holTrafficType_t type, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_longan_flowctrl_holTrafficTypeDropEnable_set
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
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      (1) Each traffic type can enable the drop function individually.
 *      (2) The function takes effect if rtk_flowctrl_portHolTrafficDropEnable_set is enabled.
 */
extern int32
dal_longan_flowctrl_holTrafficTypeDropEnable_set(uint32 unit, rtk_flowctrl_holTrafficType_t type, rtk_enable_t enable);

/* Function Name:
 *      dal_longan_flowctrl_e2eCascadePortThresh_get
 * Description:
 *      Get cascade port flowctrl threshold
 * Input:
 *      unit    - unit id
 * Output:
 *      pThresh - pointer to the threshold structure in the cascade port used page count
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_longan_flowctrl_e2eCascadePortThresh_get(uint32 unit, rtk_flowctrl_drop_thresh_t *pThresh);

/* Function Name:
 *      dal_longan_flowctrl_e2eCascadePortThresh_set
 * Description:
 *      Get cascade port flowctrl threshold
 * Input:
 *      unit    - unit id
 *      pThresh - pointer to the threshold structure in the cascade port used page count
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_longan_flowctrl_e2eCascadePortThresh_set(uint32 unit, rtk_flowctrl_drop_thresh_t *pThresh);

/* Function Name:
 *      dal_longan_flowctrl_e2eRemotePortPauseThreshGroup_get
 * Description:
 *      Get remote port used page high/low threshold for the specified threshold group
 * Input:
 *      unit    - unit id
 *      grp_idx - index of threshold group
 * Output:
 *      pThresh - pointer to the threshold structure for the port used page count
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      (1) Each port can map to a threshold group, refer to rtk_flowctrl_portE2eRemotePortThreshGroupSel_get.
 */
extern int32
dal_longan_flowctrl_e2eRemotePortPauseThreshGroup_get(uint32 unit, uint32 grp_idx, rtk_flowctrl_drop_thresh_t *pThresh);

/* Function Name:
 *      dal_longan_flowctrl_e2eRemotePortPauseThreshGroup_set
 * Description:
 *      Set remote port used page high/low threshold for the specified threshold group
 * Input:
 *      unit    - unit id
 *      grp_idx - index of threshold group
 *      pThresh - pointer to the threshold structure for the port used page count
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      (1) Each port can map to a threshold group, refer to rtk_flowctrl_portE2eRemotePortThreshGroupSel_set.
 */
extern int32
dal_longan_flowctrl_e2eRemotePortPauseThreshGroup_set(uint32 unit, uint32 grp_idx, rtk_flowctrl_drop_thresh_t *pThresh);

/* Function Name:
 *      dal_longan_flowctrl_e2eRemotePortCongestThreshGroup_get
 * Description:
 *      Get remote port used page high/low threshold for the specified threshold group
 * Input:
 *      unit    - unit id
 *      grp_idx - index of threshold group
 * Output:
 *      pThresh - pointer to the threshold structure for the port used page count
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      (1) Each port can map to a threshold group, refer to rtk_flowctrl_portE2eRemotePortThreshGroupSel_get.
 */
extern int32
dal_longan_flowctrl_e2eRemotePortCongestThreshGroup_get(uint32 unit, uint32 grp_idx, rtk_flowctrl_drop_thresh_t *pThresh);

/* Function Name:
 *      dal_longan_flowctrl_e2eRemotePortCongestThreshGroup_set
 * Description:
 *      Set remote port used page high/low threshold for the specified threshold group
 * Input:
 *      unit    - unit id
 *      grp_idx - index of threshold group
 *      pThresh - pointer to the threshold structure for the port used page count
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      (1) Each port can map to a threshold group, refer to rtk_flowctrl_portE2eRemotePortThreshGroupSel_set.
 */
extern int32
dal_longan_flowctrl_e2eRemotePortCongestThreshGroup_set(uint32 unit, uint32 grp_idx, rtk_flowctrl_drop_thresh_t *pThresh);

/* Function Name:
 *      dal_longan_flowctrl_portE2eRemotePortThreshGroupSel_get
 * Description:
 *      Get remote port used page pause threshold group for the specified port
 * Input:
 *      unit     - unit id
 *      port     - port id
 * Output:
 *      pGrp_idx - pointer to the index of threshold group
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_longan_flowctrl_portE2eRemotePortThreshGroupSel_get(uint32 unit, rtk_port_t port, uint32 *pGrp_idx);

/* Function Name:
 *      dal_longan_flowctrl_portE2eRemotePortThreshGroupSel_set
 * Description:
 *      Set remote port used page pause threshold group for the specified port
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      grp_idx - index of threshold group
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_longan_flowctrl_portE2eRemotePortThreshGroupSel_set(uint32 unit, rtk_port_t port, uint32 grp_idx);

/* Function Name:
 *      dal_longan_flowctrl_halfConsecutiveRetryEnable_get
 * Description:
 *      Get half mode collision consecutive retry transmit state.
 * Input:
 *      unit   - unit id
 *      pEnable - enable state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      consecutive retry transmit state
 *      - ENABLED: no packet loss, it will retry continue after 16 times collision
 *      - DISABLED: consecutive collision 16 times will drop and the excessive collison will count
 */
extern int32
dal_longan_flowctrl_halfConsecutiveRetryEnable_get(uint32 unit, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_longan_flowctrl_halfConsecutiveRetryEnable_set
 * Description:
 *      Set half mode collision consecutive retry transmit state.
 * Input:
 *      unit   - unit id
 *      enable - enable state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      consecutive retry transmit state
 *      - ENABLED: no packet loss, it will retry continue after 16 times collision
 *      - DISABLED: consecutive collision 16 times will drop and the excessive collison will count
 */
extern int32
dal_longan_flowctrl_halfConsecutiveRetryEnable_set(uint32 unit, rtk_enable_t enable);


#endif  /* __RTK_FLOWCTRL_H__ */
