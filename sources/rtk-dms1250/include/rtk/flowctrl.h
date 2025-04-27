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

#ifndef __RTK_FLOWCTRL_H__
#define __RTK_FLOWCTRL_H__

/*
 * Include Files
 */
#include <common/rt_type.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
typedef struct rtk_flowctrl_thresh_s
{
    uint32 highOn;
    uint32 highOff;
    uint32 lowOn;
    uint32 lowOff;
    uint32 guar;
} rtk_flowctrl_thresh_t;

typedef struct rtk_flowctrl_drop_thresh_s
{
    uint32 high;
    uint32 low;
} rtk_flowctrl_drop_thresh_t;

typedef enum rtk_flowctrl_pauseOnAction_e
{
    PAUSE_ON_RECEIVE = 0,
    PAUSE_ON_DROP,
    PAUSE_ON_END
} rtk_flowctrl_pauseOnAction_t;

/* HOL traffic type */
typedef enum rtk_flowctrl_holTrafficType_e
{
    HOL_TRAFFIC_TYPE_UNKN_UC = 0,
    HOL_TRAFFIC_TYPE_L2_MC,
    HOL_TRAFFIC_TYPE_IP_MC,
    HOL_TRAFFIC_TYPE_BC,
    HOL_TRAFFIC_TYPE_END
} rtk_flowctrl_holTrafficType_t;

/*
 * Function Declaration
 */

/* Function Name:
 *      rtk_flowctrl_init
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
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Must initialize flowctrl module before calling any flowctrl APIs.
 * Changes:
 *      None
 */
extern int32
rtk_flowctrl_init(uint32 unit);

/* Module Name    : Flow Control               */
/* Sub-module Name: Flow control configuration */

/* Function Name:
 *      rtk_flowctrl_portPauseOnAction_get
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
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Action of packet receive is as following
 *      - PAUSE_ON_RECEIVE: after Pause ON frame is sent, incoming packets always can be received.
 *      - PAUSE_ON_DROP: after Pause ON frame is sent, incoming packets can be received up to one specified number.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_flowctrl_portPauseOnAction_get(
    uint32                          unit,
    rtk_port_t                      port,
    rtk_flowctrl_pauseOnAction_t    *pAction);

/* Function Name:
 *      rtk_flowctrl_portPauseOnAction_set
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
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Action of packet receive is as following
 *      - PAUSE_ON_RECEIVE: after Pause ON frame is sent, incoming packets always can be received.
 *      - PAUSE_ON_DROP: after Pause ON frame is sent, incoming packets can be received up to one specified number.
 *        The specified number can be configured by rtk_flowctrl_pauseOnAllowedPageNum_set,
 *        rtk_flowctrl_pauseOnAllowedPktNum_set, rtk_flowctrl_pauseOnAllowedPktLen_set APIs
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_flowctrl_portPauseOnAction_set(
    uint32                          unit,
    rtk_port_t                      port,
    rtk_flowctrl_pauseOnAction_t    action);

/* Function Name:
 *      rtk_flowctrl_portPauseOnAllowedPageNum_get
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
 * Applicable:
 *      8380, 9300, 9310
 * Note:
 *      (1) The setting is active if pause on action is PAUSE_ON_DROP, refer to rtk_flowctrl_pauseOnAction_set.
 *      (2) The valid range of pageNum is 0 ~ 0xFF, mean allowed receive page number after pause on frame sent.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_flowctrl_portPauseOnAllowedPageNum_get(uint32 unit, rtk_port_t port, uint32 *pPageNum);

/* Function Name:
 *      rtk_flowctrl_portPauseOnAllowedPageNum_set
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
 * Applicable:
 *      8380, 9300, 9310
 * Note:
 *      (1) The setting is active if pause on action is PAUSE_ON_DROP, refer to rtk_flowctrl_pauseOnAction_set.
 *      (2) The valid range of pageNum is 0 ~ 0xFF, mean allowed receive page number after pause on frame sent.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_flowctrl_portPauseOnAllowedPageNum_set(uint32 unit, rtk_port_t port, uint32 pageNum);

/* Function Name:
 *      rtk_flowctrl_pauseOnAllowedPktLen_get
 * Description:
 *      Get allowed receiving packet length after pause on frame is sent.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pPktLen - pointer to number of received packet length
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390
 * Note:
 *      (1) The setting is active if pause on action is PAUSE_ON_DROP, refer to rtk_flowctrl_pauseOnAction_set.
 *      (2) The length unit is byte.
 * Changes:
 *      None
 */
extern int32
rtk_flowctrl_pauseOnAllowedPktLen_get(uint32 unit, rtk_port_t port, uint32 *pPktLen);

/* Function Name:
 *      rtk_flowctrl_pauseOnAllowedPktLen_set
 * Description:
 *      Set allowed receiving packet length after pause on frame is sent.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      pktLen - number of received packet length
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Applicable:
 *      8390
 * Note:
 *      (1) The setting is active if pause on action is PAUSE_ON_DROP, refer to rtk_flowctrl_pauseOnAction_set.
 *      (2) The length unit is byte.
 * Changes:
 *      None
 */
extern int32
rtk_flowctrl_pauseOnAllowedPktLen_set(uint32 unit, rtk_port_t port, uint32 pktLen);

/* Function Name:
 *      rtk_flowctrl_pauseOnAllowedPktNum_get
 * Description:
 *      Get number of allowed receiving packet when pause on frame is sent.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pPktNum - pointer to number of received packet
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390
 * Note:
 *      The setting is active if pause on action is PAUSE_ON_DROP, refer to rtk_flowctrl_pauseOnAction_set.
 * Changes:
 *      None
 */
extern int32
rtk_flowctrl_pauseOnAllowedPktNum_get(uint32 unit, rtk_port_t port, uint32 *pPktNum);

/* Function Name:
 *      rtk_flowctrl_pauseOnAllowedPktNum_set
 * Description:
 *      Set number of allowed receiving packet when pause on frame is sent.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      pktNum - number of received packet
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Applicable:
 *      8390
 * Note:
 *      The setting is active if pause on action is PAUSE_ON_DROP, refer to rtk_flowctrl_pauseOnAction_set.
 * Changes:
 *      None
 */
extern int32
rtk_flowctrl_pauseOnAllowedPktNum_set(uint32 unit, rtk_port_t port, uint32 pktNum);


/*
 * Flow Control ON
 */

/* Function Name:
 *      rtk_flowctrl_igrGuarEnable_get
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
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.7.0]
 *          New added function.
 */
extern int32
rtk_flowctrl_igrGuarEnable_get(uint32 unit, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_flowctrl_igrGuarEnable_set
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
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.7.0]
 *          New added function.
 */
extern int32
rtk_flowctrl_igrGuarEnable_set(uint32 unit, rtk_enable_t enable);

/* Function Name:
 *      rtk_flowctrl_igrSystemPauseThresh_get
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
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) There are four fields in rtk_flowctrl_thresh_t: highOn, highOff, lowOn, lowOff
 *      (2) The four field value are threshold of public page count in system.
 *      (3) The four field value should be highOn > highOff > lowOn > lowOff
 *      (4) The valid range is 0 ~ 0x7FF in 8380, 0 ~ 0xFFF in 9300 and 8390
 * Changes:
 *      None
 */
extern int32
rtk_flowctrl_igrSystemPauseThresh_get(uint32 unit, rtk_flowctrl_thresh_t *pThresh);

/* Function Name:
 *      rtk_flowctrl_igrSystemPauseThresh_set
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
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) There are four fields in rtk_flowctrl_thresh_t: highOn, highOff, lowOn, lowOff
 *      (2) The four field value are threshold of public page count in system.
 *      (3) The four field value should be highOn > highOff > lowOn > lowOff
 *      (4) The valid range is 0 ~ 0x7FF in 8380, 0 ~ 0xFFF in 9300 and 8390, 0 ~ 5825 in 9310.
 * Changes:
 *      None
 */
extern int32
rtk_flowctrl_igrSystemPauseThresh_set(uint32 unit, rtk_flowctrl_thresh_t *pThresh);

/* Function Name:
 *      rtk_flowctrl_igrJumboSystemPauseThresh_get
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
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) There are four fields in rtk_flowctrl_thresh_t: highOn, highOff, lowOn, lowOff
 *      (2) The four field value are threshold of public page count in system.
 *      (3) The four field value should be highOn > highOff > lowOn > lowOff
 *      (4) The valid range is 0 ~ 0x7FF in 8380, 0 ~ 0xFFF in 9300 and 8390
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_flowctrl_igrJumboSystemPauseThresh_get(uint32 unit, rtk_flowctrl_thresh_t *pThresh);

/* Function Name:
 *      rtk_flowctrl_igrJumboSystemPauseThresh_set
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
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) There are four fields in rtk_flowctrl_thresh_t: highOn, highOff, lowOn, lowOff
 *      (2) The four field value are threshold of public page count in system.
 *      (3) The four field value should be highOn > highOff > lowOn > lowOff
 *      (4) The valid range is 0 ~ 0x7FF in 8380, 0 ~ 0xFFF in 9300 and 8390, 0 ~ 5825 in 9310.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_flowctrl_igrJumboSystemPauseThresh_set(uint32 unit, rtk_flowctrl_thresh_t *pThresh);

/* Function Name:
 *      rtk_flowctrl_igrPauseThreshGroup_get
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
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) Each port can map to a threshold group, refer to rtk_flowctrl_portIgrPortThreshGroupSel_set.
 * Changes:
 *      None
 */
extern int32
rtk_flowctrl_igrPauseThreshGroup_get(uint32 unit, uint32 grp_idx, rtk_flowctrl_thresh_t *pThresh);

/* Function Name:
 *      rtk_flowctrl_igrPauseThreshGroup_set
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
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) Each port can map to a threshold group, refer to rtk_flowctrl_portIgrPortThreshGroupSel_set.
 * Changes:
 *      None
 */
extern int32
rtk_flowctrl_igrPauseThreshGroup_set(uint32 unit, uint32 grp_idx, rtk_flowctrl_thresh_t *pThresh);

/* Function Name:
 *      rtk_flowctrl_portIgrPortThreshGroupSel_get
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
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_flowctrl_igrPortPauseThreshGroupSel_get)
 */
extern int32
rtk_flowctrl_portIgrPortThreshGroupSel_get(uint32 unit, rtk_port_t port, uint32 *pGrp_idx);

/* Function Name:
 *      rtk_flowctrl_portIgrPortThreshGroupSel_set
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
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_flowctrl_portIgrPortThreshGroupSel_set)
 */
extern int32
rtk_flowctrl_portIgrPortThreshGroupSel_set(uint32 unit, rtk_port_t port, uint32 grp_idx);


/* Module Name    : Flow Control              */
/* Sub-module Name: Egress drop configuration */

/*
 * Flow Control OFF
 */

/* Function Name:
 *      rtk_flowctrl_igrSystemCongestThresh_get
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
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) There are four fields in rtk_flowctrl_thresh_t: highOn, highOff, lowOn, lowOff
 *      (2) The four field value are threshold of public page count in system.
 *      (3) The four field value should be highOn > highOff > lowOn > lowOff
 *      (4) The valid range is 0 ~ 0x7FF in 8380, 0 ~ 0xFFF in 8390 and 9300.
 * Changes:
 *      None
 */
extern int32
rtk_flowctrl_igrSystemCongestThresh_get(uint32 unit, rtk_flowctrl_thresh_t *pThresh);

/* Function Name:
 *      rtk_flowctrl_igrSystemCongestThresh_set
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
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) There are four fields in rtk_flowctrl_thresh_t: highOn, highOff, lowOn, lowOff
 *      (2) The four field value are threshold of public page count in system.
 *      (3) The four field value should be highOn > highOff > lowOn > lowOff
 *      (4) The valid range is 0 ~ 0x7FF in 8380, 0 ~ 0xFFF in 8390 and 9300, 0 ~ 5825 in 9310.
 * Changes:
 *      None
 */
extern int32
rtk_flowctrl_igrSystemCongestThresh_set(uint32 unit, rtk_flowctrl_thresh_t *pThresh);

/* Function Name:
 *      rtk_flowctrl_igrJumboSystemCongestThresh_get
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
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) There are four fields in rtk_flowctrl_thresh_t: highOn, highOff, lowOn, lowOff
 *      (2) The four field value are threshold of public page count in system.
 *      (3) The four field value should be highOn > highOff > lowOn > lowOff
 *      (4) The valid range is 0 ~ 0x7FF in 8380, 0 ~ 0xFFF in 8390 and 9300.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_flowctrl_igrJumboSystemCongestThresh_get(uint32 unit, rtk_flowctrl_thresh_t *pThresh);

/* Function Name:
 *      rtk_flowctrl_igrJumboSystemCongestThresh_set
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
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) There are four fields in rtk_flowctrl_thresh_t: highOn, highOff, lowOn, lowOff
 *      (2) The four field value are threshold of public page count in system.
 *      (3) The four field value should be highOn > highOff > lowOn > lowOff
 *      (4) The valid range is 0 ~ 0x7FF in 8380, 0 ~ 0xFFF in 8390 and 9300, 0 ~ 5825 in 9310.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_flowctrl_igrJumboSystemCongestThresh_set(uint32 unit, rtk_flowctrl_thresh_t *pThresh);


/* Function Name:
 *      rtk_flowctrl_igrCongestThreshGroup_get
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
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (2) Each port can map to a threshold group, refer to rtk_flowctrl_portIgrPortThreshGroupSel_set.
 * Changes:
 *      None
 */
extern int32
rtk_flowctrl_igrCongestThreshGroup_get(uint32 unit, uint32 grp_idx, rtk_flowctrl_thresh_t *pThresh);

/* Function Name:
 *      rtk_flowctrl_igrCongestThreshGroup_set
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
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (2) Each port can map to a threshold group, refer to rtk_flowctrl_portIgrPortThreshGroupSel_set.
 * Changes:
 *      None
 */
extern int32
rtk_flowctrl_igrCongestThreshGroup_set(uint32 unit, uint32 grp_idx, rtk_flowctrl_thresh_t *pThresh);

/* Function Name:
 *      rtk_flowctrl_jumboModeStatus_get
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
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.7.0]
 *          New added function.
 */
extern int32
rtk_flowctrl_jumboModeStatus_get(uint32 unit, uint32 *pStatus);

/* Function Name:
 *      rtk_flowctrl_jumboModeEnable_get
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
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.7.0]
 *          New added function.
 */
extern int32
rtk_flowctrl_jumboModeEnable_get(uint32 unit, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_flowctrl_jumboModeEnable_set
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
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.7.0]
 *          New added function.
 */
extern int32
rtk_flowctrl_jumboModeEnable_set(uint32 unit, rtk_enable_t enable);

/* Function Name:
 *      rtk_flowctrl_jumboModeLength_get
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
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.7.0]
 *          New added function.
 */
extern int32
rtk_flowctrl_jumboModeLength_get(uint32 unit, uint32 *pLength);

/* Function Name:
 *      rtk_flowctrl_jumboModeLength_set
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
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.7.0]
 *          New added function.
 */
extern int32
rtk_flowctrl_jumboModeLength_set(uint32 unit, uint32 length);

/* Function Name:
 *      rtk_flowctrl_egrSystemUtilThresh_get
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
 * Applicable:
 *      9310
 * Note:
 *      System only uses highOn threshold of rtk_flowctrl_thresh_t.
 * Changes:
 *      [SDK_3.7.0]
 *          New added function.
 */
extern int32
rtk_flowctrl_egrSystemUtilThresh_get(uint32 unit, rtk_flowctrl_thresh_t *pThresh);

/* Function Name:
 *      rtk_flowctrl_egrSystemUtilThresh_set
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
 * Applicable:
 *      9310
 * Note:
 *      System only uses highOn threshold of rtk_flowctrl_thresh_t.
 * Changes:
 *      [SDK_3.7.0]
 *          New added function.
 */
extern int32
rtk_flowctrl_egrSystemUtilThresh_set(uint32 unit, rtk_flowctrl_thresh_t *pThresh);

/* Function Name:
 *      rtk_flowctrl_egrSystemDropThresh_get
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
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) There are two fields in rtk_flowctrl_drop_thresh_t: high, low
 *      (2) The two field value are threshold of public page count in system.
 *      (3) The two field value should be high > low
 *      (4) The valid range is 0 ~ 0xFFF in 8390 and 9300.
 *      (5) The 8390 and 9300 has only one threshold, mean high and low are equal.
 * Changes:
 *      None
 */
extern int32
rtk_flowctrl_egrSystemDropThresh_get(uint32 unit, rtk_flowctrl_drop_thresh_t *pThresh);

/* Function Name:
 *      rtk_flowctrl_egrSystemDropThresh_set
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
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) There are two fields in rtk_flowctrl_drop_thresh_t: high, low
 *      (2) The two field value are threshold of public page count in system.
 *      (3) The two field value should be high > low
 *      (4) The valid range is 0 ~ 0xFFF in 8390 and 9300, 0 ~ 5825 in 9310.
 *      (5) The 8390 and 9300 has only one threshold, mean high and low should be set to the same value.
 * Changes:
 *      None
 */
extern int32
rtk_flowctrl_egrSystemDropThresh_set(uint32 unit, rtk_flowctrl_drop_thresh_t *pThresh);

/* Function Name:
 *      rtk_flowctrl_egrPortDropThresh_get
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
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390
 * Note:
 *      (1) There are two fields in rtk_flowctrl_drop_thresh_t: high, low
 *      (2) The two field value are threshold of public page count in system.
 *      (3) The two field value should be high > low
 *      (4) The valid range is 0 ~ 0xFFF in 8390.
 * Changes:
 *      None
 */
extern int32
rtk_flowctrl_egrPortDropThresh_get(uint32 unit, rtk_port_t port, rtk_flowctrl_drop_thresh_t *pThresh);

/* Function Name:
 *      rtk_flowctrl_egrPortDropThresh_set
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
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Applicable:
 *      8390
 * Note:
 *      (1) There are two fields in rtk_flowctrl_drop_thresh_t: high, low
 *      (2) The two field value are threshold of public page count in system.
 *      (3) The two field value should be high > low
 *      (4) The valid range is 0 ~ 0xFFF in 8390.
 * Changes:
 *      None
 */
extern int32
rtk_flowctrl_egrPortDropThresh_set(uint32 unit, rtk_port_t port, rtk_flowctrl_drop_thresh_t *pThresh);

/* Function Name:
 *      rtk_flowctrl_egrPortDropEnable_get
 * Description:
 *      Get enable status of egress port drop.
 * Input:
 *      unit    - unit id
 *      port    - queue id
 * Output:
 *      pEnable - pointer to enable status of egress port drop
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_flowctrl_egrPortDropEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_flowctrl_egrPortDropEnable_set
 * Description:
 *      Set enable status of egress port drop.
 * Input:
 *      unit   - unit id
 *      port   - queue id
 *      enable - enable status of egress port drop
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380
 * Note:
 *     None
 * Changes:
 *      None
 */
extern int32
rtk_flowctrl_egrPortDropEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);


/* Function Name:
 *      rtk_flowctrl_egrPortQueueDropEnable_get
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
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390
 * Note:
 *      Enable egress Q drop function for the specified port. The packet is dropped if the corresponding
 *      egress Q used page count over the egress Q drop threshold. Use rtk_flowctrl_egrQueueDropThresh_set
 *      to specify the queue drop thresold.
 * Changes:
 *      None
 */
extern int32
rtk_flowctrl_egrPortQueueDropEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_flowctrl_egrPortQueueDropEnable_set
 * Description:
 *      Set egress queue drop ability for the specified port
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - status of egress queue drop ability
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - Invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8390
 * Note:
 *      Enable egress Q drop function for the specified port. The packet is dropped if the corresponding
 *      egress Q used page count over the egress Q drop threshold. Use rtk_flowctrl_egrQueueDropThresh_set
 *      to specify the queue drop thresold.
 * Changes:
 *      None
 */
extern int32
rtk_flowctrl_egrPortQueueDropEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      rtk_flowctrl_egrQueueDropThresh_get
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
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      (1) There are two fields in rtk_flowctrl_drop_thresh_t: high, low
 *      (2) The two field value are threshold of public page count in the queue of all ports.
 *      (3) The two field value should be high > low
 *      (4) The valid range is 0 ~ 0xFFF in 8390
 * Changes:
 *      None
 */
extern int32
rtk_flowctrl_egrQueueDropThresh_get(uint32 unit, rtk_qid_t queue, rtk_flowctrl_drop_thresh_t *pThresh);

/* Function Name:
 *      rtk_flowctrl_egrQueueDropThresh_set
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
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      (1) There are two fields in rtk_flowctrl_drop_thresh_t: high, low
 *      (2) The two field value are threshold of public page count in system.
 *      (3) The two field value should be high > low
 *      (4) The valid range is 0 ~ 0xFFF in 8390
 * Changes:
 *      None
 */
extern int32
rtk_flowctrl_egrQueueDropThresh_set(uint32 unit, rtk_qid_t queue, rtk_flowctrl_drop_thresh_t *pThresh);

/* Function Name:
 *      rtk_flowctrl_egrCpuQueueDropThresh_get
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
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      (1) CPU port has dedicated egress queue drop threshold configuration different from normal port.
 * Changes:
 *      None
 */
extern int32
rtk_flowctrl_egrCpuQueueDropThresh_get(uint32 unit, rtk_qid_t queue, rtk_flowctrl_drop_thresh_t *pThresh);

/* Function Name:
 *      rtk_flowctrl_egrCpuQueueDropThresh_set
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
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      (1) CPU port has dedicated egress queue drop threshold configuration different from normal port.
 *      (2) The valid range is 0 ~ 5825 in 9310.
 * Changes:
 *      None
 */
extern int32
rtk_flowctrl_egrCpuQueueDropThresh_set(uint32 unit, rtk_qid_t queue, rtk_flowctrl_drop_thresh_t *pThresh);

/* Function Name:
 *      rtk_flowctrl_portEgrDropRefCongestEnable_get
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
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_flowctrl_egrPortDropRefCongestEnable_get)
 */
extern int32
rtk_flowctrl_portEgrDropRefCongestEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_flowctrl_portEgrDropRefCongestEnable_set
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
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_flowctrl_egrPortDropRefCongestEnable_set)
 */
extern int32
rtk_flowctrl_portEgrDropRefCongestEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      rtk_flowctrl_egrPortDropThreshGroup_get
 * Description:
 *      Get egress port drop threshold for the specified threshold group
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
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380
 * Note:
 *      (1) Each port can map to a threshold group, refer to rtk_flowctrl_egrPortQueueDropThreshGroupSel_set.
 * Changes:
 *      None
 */
extern int32
rtk_flowctrl_egrPortDropThreshGroup_get(uint32 unit, uint32 grp_idx, rtk_flowctrl_drop_thresh_t * pThresh);

/* Function Name:
 *      rtk_flowctrl_egrPortDropThreshGroup_set
 * Description:
 *      Set egress port drop threshold for the specified threshold group
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
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380
 * Note:
 *      (1) Each port can map to a threshold group, refer to rtk_flowctrl_egrPortQueueDropThreshGroupSel_set.
 * Changes:
 *      None
 */
extern int32
rtk_flowctrl_egrPortDropThreshGroup_set(uint32 unit, uint32 grp_idx, rtk_flowctrl_drop_thresh_t *pThresh);

/* Function Name:
 *      rtk_flowctrl_egrQueueDropThreshGroup_get
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
 * Applicable:
 *      8380, 9300, 9310
 * Note:
 *      (1) Each port can map to a threshold group, refer to rtk_flowctrl_portEgrDropThreshGroupSel_set.
 * Changes:
 *      None
 */
extern int32
rtk_flowctrl_egrQueueDropThreshGroup_get(uint32 unit, uint32 grp_idx, rtk_qid_t queue, rtk_flowctrl_drop_thresh_t *pThresh);

/* Function Name:
 *      rtk_flowctrl_egrQueueDropThreshGroup_set
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
 * Applicable:
 *      8380, 9300, 9310
 * Note:
 *      (1) Each port can map to a threshold group, refer to rtk_flowctrl_portEgrDropThreshGroupSel_set.
 * Changes:
 *      None
 */
extern int32
rtk_flowctrl_egrQueueDropThreshGroup_set(uint32 unit, uint32 grp_idx, rtk_qid_t queue, rtk_flowctrl_drop_thresh_t *pThresh);

/* Function Name:
 *      rtk_flowctrl_portEgrDropThreshGroupSel_get
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
 * Applicable:
 *      8380, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_flowctrl_egrPortQueueDropThreshGroupSel_get)
 */
extern int32
rtk_flowctrl_portEgrDropThreshGroupSel_get(uint32 unit, rtk_port_t port, uint32 *pGrp_idx);

/* Function Name:
 *      rtk_flowctrl_portEgrDropThreshGroupSel_set
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
 * Applicable:
 *      8380, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_flowctrl_egrPortQueueDropThreshGroupSel_set)
 */
extern int32
rtk_flowctrl_portEgrDropThreshGroupSel_set(uint32 unit, rtk_port_t port, uint32 grp_idx);

/* Function Name:
 *      rtk_flowctrl_egrQueueDropEnable_get
 * Description:
 *      Get enable status of egress queue drop.
 * Input:
 *      unit    - unit id
 *      port    - queue id
 *      queue   - queue id
 * Output:
 *      pEnable - pointer to enable status of egress queue drop
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_QUEUE_ID     - invalid queue id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Applicable:
 *      8380
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_flowctrl_egrQueueDropEnable_get(uint32 unit, rtk_port_t port, rtk_qid_t queue, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_flowctrl_egrQueueDropEnable_set
 * Description:
 *      Set enable status of egress queue drop.
 * Input:
 *      unit   - unit id
 *      port   - queue id
 *      queue  - queue id
 *      enable - enable status of egress queue drop
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
 * Applicable:
 *      8380
 * Note:
 *     None
 * Changes:
 *      None
 */
extern int32
rtk_flowctrl_egrQueueDropEnable_set(uint32 unit, rtk_port_t port, rtk_qid_t queue, rtk_enable_t enable);


/* Function Name:
 *      rtk_flowctrl_portEgrQueueDropForceEnable_get
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
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_flowctrl_portEgrQueueDropForceEnable_get(uint32 unit, rtk_port_t port, rtk_qid_t queue, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_flowctrl_portEgrQueueDropForceEnable_set
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
 * Applicable:
 *      9300, 9310
 * Note:
 *     None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_flowctrl_portEgrQueueDropForceEnable_set(uint32 unit, rtk_port_t port, rtk_qid_t queue, rtk_enable_t enable);

/* Function Name:
 *      rtk_flowctrl_igrQueueDropEnable_get
 * Description:
 *      Get enable status of ingress queue drop.
 * Input:
 *      unit  - unit id
 *      port  - queue id
 *      queue - queue id
 * Output:
 *      pEnable - pointer to enable status of ingress queue drop
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_QUEUE_ID     - invalid queue id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_flowctrl_igrQueueDropEnable_get(uint32 unit, rtk_port_t port, rtk_qid_t queue, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_flowctrl_igrQueueDropEnable_set
 * Description:
 *      Set enable status of ingress queue drop.
 * Input:
 *      unit   - unit id
 *      port   - queue id
 *      queue  - queue id
 *      enable - enable status of ingress queue drop
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
 * Applicable:
 *      8380
 * Note:
 *     None
 * Changes:
 *      None
 */
extern int32
rtk_flowctrl_igrQueueDropEnable_set(uint32 unit, rtk_port_t port, rtk_qid_t queue, rtk_enable_t enable);

/* Function Name:
 *      rtk_flowctrl_igrQueuePauseDropThreshGroupSel_get
 * Description:
 *      Get ingress queue used page pause/drop threshold group for the specified port
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
 * Applicable:
 *      8380
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_flowctrl_igrQueuePauseDropThreshGroupSel_get(uint32 unit, rtk_port_t port, uint32 *pGrp_idx);

/* Function Name:
 *      rtk_flowctrl_igrQueuePauseDropThreshGroupSel_set
 * Description:
 *      Set ingress queue used page pause/drop threshold group for the specified port
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
 * Applicable:
 *      8380
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_flowctrl_igrQueuePauseDropThreshGroupSel_set(uint32 unit, rtk_port_t port, uint32 grp_idx);

/* Function Name:
 *      rtk_flowctrl_igrQueueDropThreshGroup_get
 * Description:
 *      Get ingress queue Drop threshold for the specified threshold group
 * Input:
 *      unit    - unit id
 *      grp_idx - index of threshold group
 *      queue   - queue id
 * Output:
 *      pThresh - pointer to the threshold structure for the queue used page count
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_QUEUE_ID         - invalid queue id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380
 * Note:
 *      1) Each port can map to a threshold group, refer to rtk_flowctrl_igrQueuePauseDropThreshGroupSel_set.
 * Changes:
 *      None
 */
extern int32
rtk_flowctrl_igrQueueDropThreshGroup_get(uint32 unit, uint32 grp_idx, rtk_qid_t queue, rtk_flowctrl_drop_thresh_t *pThresh);

/* Function Name:
 *      rtk_flowctrl_igrQueueDropThreshGroup_set
 * Description:
 *      Set ingress queue Drop threshold for the specified threshold group
 * Input:
 *      unit    - unit id
 *      grp_idx - index of threshold group
 *      queue   - queue id
 *      pThresh - pointer to the threshold structure for the queue used page count
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_QUEUE_ID         - invalid queue id
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Applicable:
 *      8380
 * Note:
 *      1) Each port can map to a threshold group, refer to rtk_flowctrl_igrQueuePauseDropThreshGroupSel_set.
 * Changes:
 *      None
 */
extern int32
rtk_flowctrl_igrQueueDropThreshGroup_set(uint32 unit, uint32 grp_idx, rtk_qid_t queue, rtk_flowctrl_drop_thresh_t *pThresh);

/* Function Name:
 *      rtk_flowctrl_igrQueuePauseThreshGroup_get
 * Description:
 *      Get ingress queue Pause threshold for the specified threshold group
 * Input:
 *      unit    - unit id
 *      grp_idx - index of threshold group
 *      queue - queue id
 * Output:
 *      pThresh - pointer to the threshold structure for the queue used page count
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_QUEUE_ID     - invalid queue id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380
 * Note:
 *      (1) Each port can map to a threshold group, refer to rtk_flowctrl_igrQueuePauseDropThreshGroupSel_set.
 * Changes:
 *      None
 */
extern int32
rtk_flowctrl_igrQueuePauseThreshGroup_get(uint32 unit, uint32 grp_idx, rtk_qid_t queue, rtk_flowctrl_drop_thresh_t *pThresh);

/* Function Name:
 *      rtk_flowctrl_igrQueuePauseThreshGroup_set
 * Description:
 *      Set ingress queue Pause threshold for the specified threshold group
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
 * Applicable:
 *      8380
 * Note:
 *      (1) Each port can map to a threshold group, refer to rtk_flowctrl_igrQueuePauseDropThreshGroupSel_set.
 * Changes:
 *      None
 */
extern int32
rtk_flowctrl_igrQueuePauseThreshGroup_set(uint32 unit, uint32 grp_idx, rtk_qid_t queue, rtk_flowctrl_drop_thresh_t *pThresh);

/* Function Name:
 *      rtk_flowctrl_igrQueueDropThresh_get
 * Description:
 *      Get ingress queue drop threshold.
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
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_flowctrl_igrQueueDropThresh_get(uint32 unit, rtk_qid_t queue, rtk_flowctrl_drop_thresh_t *pThresh);

/* Function Name:
 *      rtk_flowctrl_igrQueueDropThresh_set
 * Description:
 *      Set ingress queue drop threshold.
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
 * Applicable:
 *      9310
 * Note:
 *      (1) The valid range is 0 ~ 5825 in 9310.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_flowctrl_igrQueueDropThresh_set(uint32 unit, rtk_qid_t queue, rtk_flowctrl_drop_thresh_t *pThresh);

/* Function Name:
 *      rtk_flowctrl_portHolTrafficDropEnable_get
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
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) Per ingress port can enable the drop function to drop flooding traffic.
 *      (2) The function takes effect only if the flow control of ingress port is enabled.
 *      (3) Refer to rtk_flowctrl_holTrafficTypeDropEnable_set for dropping specific traffic type.
 * Changes:
 *      None
 */
extern int32
rtk_flowctrl_portHolTrafficDropEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_flowctrl_portHolTrafficDropEnable_set
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
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) Enable the function to prevent HOL by flooding traffic when flow control is enabled.
 *      (2) Per ingress port can enable the drop function to drop flooding traffic at the congested
 *          egress port.
 *      (3) Refer to rtk_flowctrl_holTrafficTypeDropEnable_set for dropping specific traffic type.
 * Changes:
 *      None
 */
extern int32
rtk_flowctrl_portHolTrafficDropEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      rtk_flowctrl_holTrafficTypeDropEnable_get
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
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) Each traffic type can enable the drop function individually.
 *      (2) The function takes effect if rtk_flowctrl_portHolTrafficDropEnable_set is enabled.
 * Changes:
 *      None
 */
extern int32
rtk_flowctrl_holTrafficTypeDropEnable_get(uint32 unit, rtk_flowctrl_holTrafficType_t type, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_flowctrl_holTrafficTypeDropEnable_set
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
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) Each traffic type can enable the drop function individually.
 *      (2) The function takes effect if rtk_flowctrl_portHolTrafficDropEnable_set is enabled.
 * Changes:
 *      None
 */
extern int32
rtk_flowctrl_holTrafficTypeDropEnable_set(uint32 unit, rtk_flowctrl_holTrafficType_t type, rtk_enable_t enable);

/* Function Name:
 *      rtk_flowctrl_specialCongestThreshold_get
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
 * Applicable:
 *      8380, 8390
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_flowctrl_specialCongestThreshold_get(uint32 unit, rtk_flowctrl_drop_thresh_t *pThresh);

/* Function Name:
 *      rtk_flowctrl_specialCongestThreshold_set
 * Description:
 *      Set special congstion threshold
 * Input:
 *      unit    - unit id
 *      pThresh -the pointer to threshold structure for special congstion
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Applicable:
 *      8380, 8390
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_flowctrl_specialCongestThreshold_set(uint32 unit, rtk_flowctrl_drop_thresh_t *pThresh);


/* Function Name:
 *      rtk_flowctrl_e2eCascadePortThresh_get
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
 * Applicable:
 *      9300
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_flowctrl_e2eCascadePortThresh_get(uint32 unit, rtk_flowctrl_drop_thresh_t *pThresh);

/* Function Name:
 *      rtk_flowctrl_e2eCascadePortThresh_set
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
 * Applicable:
 *      9300
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_flowctrl_e2eCascadePortThresh_set(uint32 unit, rtk_flowctrl_drop_thresh_t *pThresh);

/* Function Name:
 *      rtk_flowctrl_e2eRemotePortPauseThreshGroup_get
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
 * Applicable:
 *      9300
 * Note:
 *      (1) Each port can map to a threshold group, refer to rtk_flowctrl_portE2eRemotePortThreshGroupSel_get.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_flowctrl_e2eRemotePortPauseThreshGroup_get(uint32 unit, uint32 grp_idx, rtk_flowctrl_drop_thresh_t *pThresh);

/* Function Name:
 *      rtk_flowctrl_e2eRemotePortPauseThreshGroup_set
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
 * Applicable:
 *      9300
 * Note:
 *      (1) Each port can map to a threshold group, refer to rtk_flowctrl_portE2eRemotePortThreshGroupSel_set.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_flowctrl_e2eRemotePortPauseThreshGroup_set(uint32 unit, uint32 grp_idx, rtk_flowctrl_drop_thresh_t *pThresh);

/* Function Name:
 *      rtk_flowctrl_e2eRemotePortCongestThreshGroup_get
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
 * Applicable:
 *      9300
 * Note:
 *      (1) Each port can map to a threshold group, refer to rtk_flowctrl_portE2eRemotePortThreshGroupSel_get.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_flowctrl_e2eRemotePortCongestThreshGroup_get(uint32 unit, uint32 grp_idx, rtk_flowctrl_drop_thresh_t *pThresh);

/* Function Name:
 *      rtk_flowctrl_e2eRemotePortCongestThreshGroup_set
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
 * Applicable:
 *      9300
 * Note:
 *      (1) Each port can map to a threshold group, refer to rtk_flowctrl_portE2eRemotePortThreshGroupSel_set.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_flowctrl_e2eRemotePortCongestThreshGroup_set(uint32 unit, uint32 grp_idx, rtk_flowctrl_drop_thresh_t *pThresh);

/* Function Name:
 *      rtk_flowctrl_portE2eRemotePortThreshGroupSel_get
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
 * Applicable:
 *      9300
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_flowctrl_portE2eRemotePortThreshGroupSel_get(uint32 unit, rtk_port_t port, uint32 *pGrp_idx);

/* Function Name:
 *      rtk_flowctrl_portE2eRemotePortThreshGroupSel_set
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
 * Applicable:
 *      9300
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_flowctrl_portE2eRemotePortThreshGroupSel_set(uint32 unit, rtk_port_t port, uint32 grp_idx);

/* Function Name:
 *      rtk_flowctrl_tagPauseEnable_get
 * Description:
 *      Get forwarding state of the pause frame with tag.
 * Input:
 *      unit   - unit id
 * Output:
 *      pEnable - forwarding state of the pause frame with tag.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      9310
 * Note:
 *      Forwarding state is as following
 *      - ENABLED: forward the pause frame with tag
 *      - DISABLED: drop the pause frame with tag
 * Changes:
 *      [3.2.0]
 *          New added function.
 */
extern int32
rtk_flowctrl_tagPauseEnable_get(uint32 unit, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_flowctrl_tagPauseEnable_set
 * Description:
 *      Set forwarding state of the pause frame with tag.
 * Input:
 *      unit   - unit id
 *      enable - forwarding state of the pause frame with tag.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      9310
 * Note:
 *      Forwarding state is as following
 *      - ENABLED: forward the pause frame with tag
 *      - DISABLED: drop the pause frame with tag
 * Changes:
 *      [3.2.0]
 *          New added function.
 */
extern int32
rtk_flowctrl_tagPauseEnable_set(uint32 unit, rtk_enable_t enable);

/* Function Name:
 *      rtk_flowctrl_halfConsecutiveRetryEnable_get
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
 * Applicable:
 *      9300, 9310
 * Note:
 *      consecutive retry transmit state
 *      - ENABLED: no packet loss, it will retry continue after 16 times collision
 *      - DISABLED: consecutive collision 16 times will drop and the excessive collison will count
 * Changes:
 *      [3.2.0]
 *          New added function.
 */
extern int32
rtk_flowctrl_halfConsecutiveRetryEnable_get(uint32 unit, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_flowctrl_halfConsecutiveRetryEnable_set
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
 * Applicable:
 *      9300, 9310
 * Note:
 *      consecutive retry transmit state
 *      - ENABLED: no packet loss, it will retry continue after 16 times collision
 *      - DISABLED: consecutive collision 16 times will drop and the excessive collison will count
 * Changes:
 *      [3.2.0]
 *          New added function.
 */
extern int32
rtk_flowctrl_halfConsecutiveRetryEnable_set(uint32 unit, rtk_enable_t enable);

#endif  /* __RTK_FLOWCTRL_H__ */

