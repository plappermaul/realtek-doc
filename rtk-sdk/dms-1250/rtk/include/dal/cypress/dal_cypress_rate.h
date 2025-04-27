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
 * Purpose : Definition those public port bandwidth control and storm control APIs and its data type
 *           in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *             1) Configuration of ingress port bandwidth control (ingress rate limit ).
 *             2) Configuration of egress port bandwidth control (egress rate limit).
 *             3) Configuration of storm control
 *
 */

#ifndef __DAL_CYPRESS_RATE_H__
#define __DAL_CYPRESS_RATE_H__


/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/port.h>
#include <rtk/rate.h>
#include <dal/dal_mapper.h>


/*
 * Symbol Definition
 */
#define EGRBW_LB_WFQ_BURST              4500
#define EGRBW_LB_BPS_TICK_250M          185
#define EGRBW_LB_BPS_TOKEN_250M         97
#define EGRBW_LB_BPS_TICK_50M           37
#define EGRBW_LB_BPS_TOKEN_50M          97
#define EGRBW_LB_BPS_TICK_250M_10G      18
#define EGRBW_LB_BPS_TOKEN_250M_10G     151
#define EGRBW_LB_BPS_TICK_50M_10G       5
#define EGRBW_LB_BPS_TOKEN_50M_10G      42
#define EGRBW_LB_PPS_TICK_250M_CPU      3815
#define EGRBW_LB_PPS_TOKEN_250M_CPU     1
#define EGRBW_LB_PPS_TICK_50M_CPU       763
#define EGRBW_LB_PPS_TOKEN_50M_CPU      1

#define IGRBW_LB_BPS_TICK_250M          245
#define IGRBW_LB_BPS_TOKEN_250M         129
#define IGRBW_LB_BPS_TICK_50M           214
#define IGRBW_LB_BPS_TOKEN_50M          561
#define IGRBW_LB_BPS_TICK_250M_10G      18
#define IGRBW_LB_BPS_TOKEN_250M_10G     151
#define IGRBW_LB_BPS_TICK_50M_10G       18
#define IGRBW_LB_BPS_TOKEN_50M_10G      755
#define IGRBW_HIGH_THRESH               65535
#define IGRBW_LOW_THRESH                65532

#define STORM_LB_BPS_TICK_250M          246
#define STORM_LB_BPS_TOKEN_250M         129
#define STORM_LB_BPS_TICK_50M           214
#define STORM_LB_BPS_TOKEN_50M          561
#define STORM_LB_BPS_TICK_250M_10G      18
#define STORM_LB_BPS_TOKEN_250M_10G     151
#define STORM_LB_BPS_TICK_50M_10G       18
#define STORM_LB_BPS_TOKEN_50M_10G      755

#define STORM_LB_BPS_BURST              (0x8000)
#define STORM_LB_BPS_BURST_10G          (0x8000)

#define STORM_LB_PPS_TICK_250M          238
#define STORM_LB_PPS_TOKEN_250M         1
#define STORM_LB_PPS_TICK_50M           240
#define STORM_LB_PPS_TOKEN_50M          5
#define STORM_LB_PPS_TICK_250M_10G      238
#define STORM_LB_PPS_TOKEN_250M_10G     1
#define STORM_LB_PPS_TICK_50M_10G       240
#define STORM_LB_PPS_TOKEN_50M_10G      5

#define STORM_LB_PPS_BURST              32
#define STORM_LB_PPS_BURST_10G          32

#define STORM_LB_PROTO_TICK_250M        976563
#define STORM_LB_PROTO_TICK_50M         195313


/*
 * Data Declaration
 */
typedef enum dal_cypress_rate_speed_e
{
    RATE_LINK_SPEED_10M = 0,
    RATE_LINK_SPEED_100M,
    RATE_LINK_SPEED_500M,
    RATE_LINK_SPEED_1G,
    RATE_LINK_SPEED_10G,
    RATE_LINK_SPEED_END
} dal_cypress_rate_speed_t;


/*
 * Function Declaration
 */

/* Function Name:
 *      dal_cypress_rateMapper_init
 * Description:
 *      Hook rate module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must Hook rate module before calling any rate APIs.
 */
extern int32
dal_cypress_rateMapper_init(dal_mapper_t *pMapper);

/* Function Name:
 *      dal_cypress_rate_egrBandwidthCtrlLinkChange_handler
 * Description:
 *      Dispatch maximum token refill rate of egress bandwith by link speed
 * Input:
 *      unit           - unit id
 *      port           - port id
 *      new_linkStatus - new link status
 * Output:
 *      None
 * Return:
 *      SUCCESS
 *      FAILED
 * Note:
 *      None
 */
extern int32
dal_cypress_rate_egrBandwidthCtrlLinkChange_handler(int32 unit, rtk_port_t port, rtk_port_linkStatus_t new_linkStatus);


/* Function Name:
 *      dal_cypress_rate_init
 * Description:
 *      Initial the rate module of the specified device..
 * Input:
 *      unit                - unit id
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
extern int32
dal_cypress_rate_init(uint32 unit);

/* Function Name:
 *      dal_cypress_rate_portIgrBwCtrlEnable_get
 * Description:
 *      Get the ingress bandwidth control status.
 * Input:
 *      unit                - unit id
 *      port                - port id
 * Output:
 *      pEnable            - status of ingress bandwidth control
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID       - Invalid port id
 *      RT_ERR_NULL_POINTER  - NULL pointer
 * Note:
 *    The status of ingress bandwidth control is as following:
 *    - DISABLED
 *    - ENABLED
 */
extern int32
dal_cypress_rate_portIgrBwCtrlEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_cypress_rate_portIgrBwCtrlEnable_set
 * Description:
 *      Set the ingress bandwidth control status.
 * Input:
 *      unit                - unit id
 *      port                - port id
 *      enable              - status of ingress bandwidth control
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID       - Invalid port id
 *      RT_ERR_INPUT         - Invalid input parameter
 * Note:
 *    The status of ingress bandwidth control is as following:
 *    - DISABLED
 *    - ENABLED
 */
extern int32
dal_cypress_rate_portIgrBwCtrlEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      dal_cypress_rate_portIgrBwCtrlRate_get
 * Description:
 *      Get the ingress bandwidth control rate.
 * Input:
 *      unit        - unit id
 *      port        - port id
 * Output:
 *      pRate      - ingress bandwidth control rate
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID       - Invalid port id
 *      RT_ERR_NULL_POINTER  - NULL pointer
 * Note:
 *    The actual rate is "rate * chip granularity".
 *    The unit of granularity in RTL8389/RTL8329 is 16Kbps.
 */
extern int32
dal_cypress_rate_portIgrBwCtrlRate_get(uint32 unit, rtk_port_t port, uint32 *pRate);

/* Function Name:
 *      dal_cypress_rate_portIgrBwCtrlRate_set
 * Description:
 *      Set the ingress bandwidth control rate.
 * Input:
 *      unit        - unit id
 *      port        - port id
 *      rate        - ingress bandwidth control rate
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID       - Invalid port id
 *      RT_ERR_RATE          - Invalid input rate
 * Note:
 *    The actual rate is "rate * chip granularity".
 *    The unit of granularity in RTL8389/RTL8329 is 16Kbps.
 */
extern int32
dal_cypress_rate_portIgrBwCtrlRate_set(uint32 unit, rtk_port_t port, uint32 rate);

/* Function Name:
 *      dal_cypress_rate_portIgrBwCtrlBurstSize_get
 * Description:
 *      Get the ingress bandwidth control burst.
 * Input:
 *      unit  - unit id
 *      port  - port id
 * Output:
 *      pCfg - ingress bandwidth control burst
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - Invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None.
 */
extern int32
dal_cypress_rate_portIgrBwCtrlBurstSize_get(uint32 unit, rtk_port_t port, rtk_rate_igrBwBurst_cfg_t *pCfg);

/* Function Name:
 *      dal_cypress_rate_portIgrBwCtrlBurstSize_set
 * Description:
 *      Set the ingress bandwidth control burst.
 * Input:
 *      unit - unit id
 *      port - port id
 *      pCfg - ingress bandwidth control burst
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - Invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - Invalid port id
 *      RT_ERR_RATE     - Invalid input rate
 * Note:
 *      None
 */
extern int32
dal_cypress_rate_portIgrBwCtrlBurstSize_set(uint32 unit, rtk_port_t port, rtk_rate_igrBwBurst_cfg_t *pCfg);

/* Function Name:
 *      dal_cypress_rate_igrBandwidthCtrlIncludeIfg_get
 * Description:
 *      Get the status of ingress bandwidth control includes IFG or not.
 * Input:
 *      unit                    - unit id
 * Output:
 *      pIfg_include           - include IFG or not
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER     - NULL pointer
 * Note:
 *      1. Ingress bandwidth control includes/excludes the Preamble & IFG (20 Bytes).
 *
 *      2. The status of ifg_include:
 *         - DISABLED
 *         - ENABLED
 */
extern int32
dal_cypress_rate_igrBandwidthCtrlIncludeIfg_get(uint32 unit, rtk_enable_t *pIfg_include);

/* Function Name:
 *      dal_cypress_rate_igrBandwidthCtrlIncludeIfg_set
 * Description:
 *      Set the status of ingress bandwidth control includes IFG or not.
 * Input:
 *      unit                    - unit id
 *      ifg_include             - include IFG or not
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT                 - Invalid input parameter
 * Note:
 *      1. Ingress bandwidth control includes/excludes the Preamble & IFG (20 Bytes).
 *
 *      2. The status of ifg_include:
 *         - DISABLED
 *         - ENABLED
 */
extern int32
dal_cypress_rate_igrBandwidthCtrlIncludeIfg_set(uint32 unit, rtk_enable_t ifg_include);

/* Function Name:
 *      dal_cypress_rate_igrBandwidthCtrlBypass_get
 * Description:
 *      Get the status of bypass ingress bandwidth control for specified frame type.
 * Input:
 *      unit       - unit id
 *      bypassType - bypass type
 * Output:
 *      pEnable    - pointer to enable status of bypass ingress bandwidth control
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT        - Invalid input parameter
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      None.
 */
extern int32
dal_cypress_rate_igrBandwidthCtrlBypass_get(uint32 unit, rtk_rate_igr_bypassType_t bypassType, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_cypress_rate_igrBandwidthCtrlBypass_set
 * Description:
 *      Set the status of bypass ingress bandwidth control for specified packet type.
 * Input:
 *      unit         - unit id
 *      byasssType   - bypass type
 *      enable       - status of bypass ingress bandwidth control
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT                 - Invalid input parameter
 * Note:
 *      None.
 */
extern int32
dal_cypress_rate_igrBandwidthCtrlBypass_set(uint32 unit, rtk_rate_igr_bypassType_t bypassType, rtk_enable_t enable);

/* Function Name:
 *      dal_cypress_rate_portIgrBwFlowctrlEnable_get
 * Description:
 *      Get enable status of flowcontrol for ingress bandwidth control on specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to enable status of flowcontrol for ingress bandwidth control
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
dal_cypress_rate_portIgrBwFlowctrlEnable_get(
    uint32              unit,
    rtk_port_t          port,
    rtk_enable_t        *pEnable);

/* Function Name:
 *      dal_cypress_rate_portIgrBwFlowctrlEnable_set
 * Description:
 *      Set enable status of flowcontrol for ingress bandwidth control on specified port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - enable status of flowcontrol for ingress bandwidth control
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
dal_cypress_rate_portIgrBwFlowctrlEnable_set(
    uint32              unit,
    rtk_port_t          port,
    rtk_enable_t        enable);

/* Function Name:
 *      dal_cypress_rate_igrBandwidthLowThresh_get
 * Description:
 *      Get the flow control turn ON/OFF low threshold of ingress bandwidth.
 * Input:
 *      unit                   - unit id
 * Output:
 *      pLowThresh             - pointer to flow control turn ON/OFF low threshold
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER     - NULL pointer
 * Note:
 *      None.
 */
extern int32
dal_cypress_rate_igrBandwidthLowThresh_get(
    uint32          unit,
    uint32          *pLowThresh);

/* Function Name:
 *      dal_cypress_rate_igrBandwidthLowThresh_set
 * Description:
 *      Set the flow control turn ON/OFF low threshold of ingress bandwidth.
 * Input:
 *      unit         - unit id
 *      lowThresh    - flow control turn ON/OFF low threshold
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT                 - Invalid input parameter
 * Note:
 *      None.
 */
extern int32
dal_cypress_rate_igrBandwidthLowThresh_set(
    uint32          unit,
    uint32          lowThresh);

/* Function Name:
 *      dal_cypress_rate_portIgrBandwidthCtrlExceed_get
 * Description:
 *      Get exceed status of ingress bandwidth control on specified port.
 * Input:
 *      unit       - unit id
 *      port       - port id
 * Output:
 *      pIsExceed  - pointer to exceed status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_PORT_ID           - invalid port id
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 * Note:
 *      Exceed status is as following
 *      - TRUE      - flow control high on threshold ever exceeds.
 *      - FALSE     - flow control high on threshold never exceeds.
 */
extern int32
dal_cypress_rate_portIgrBandwidthCtrlExceed_get(
    uint32                  unit,
    rtk_port_t              port,
    uint32                  *pIsExceed);

/* Function Name:
 *      dal_cypress_rate_portIgrBandwidthCtrlExceed_reset
 * Description:
 *      Reset exceed status of ingress bandwidth control on specified port.
 * Input:
 *      unit       - unit id
 *      port       - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_PORT_ID              - invalid port id
 * Note:
 *      None
 */
extern int32
dal_cypress_rate_portIgrBandwidthCtrlExceed_reset(
    uint32                  unit,
    rtk_port_t              port);

/* Function Name:
 *      dal_cypress_rate_portIgrBandwidthHighThresh_get
 * Description:
 *      Get the flow control turn ON/OFF high threshold of ingress bandwidth for specified port.
 * Input:
 *      unit                   - unit id
 *      port                   - port id
 * Output:
 *      pHighThresh            - pointer to flow control turn ON/OFF high threshold
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID          - Invalid port id
 *      RT_ERR_NULL_POINTER     - NULL pointer
 * Note:
 *      None.
 */
extern int32
dal_cypress_rate_portIgrBandwidthHighThresh_get(uint32 unit, rtk_port_t port, uint32 *pHighThresh);

/* Function Name:
 *      dal_cypress_rate_portIgrBandwidthHighThresh_set
 * Description:
 *      Set the flow control turn ON/OFF high threshold of ingress bandwidth for specified port.
 * Input:
 *      unit         - unit id
 *      port         - port id
 *      highThresh   - flow control turn ON/OFF high threshold
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID               - Invalid port id
 *      RT_ERR_INPUT                 - Invalid input parameter
 * Note:
 *      The (minimum, maximum) of highThresh setting range is (22, 65535).
 */
extern int32
dal_cypress_rate_portIgrBandwidthHighThresh_set(uint32 unit, rtk_port_t port, uint32 highThresh);

/* Function Name:
 *      dal_cypress_rate_igrBwCtrlBurstSize_get
 * Description:
 *      Get burst size of ingress bandwidth
 * Input:
 *      unit        - unit id
 * Output:
 *      pBurst_size - pointer to burst size
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_rate_igrBwCtrlBurstSize_get(uint32 unit, uint32 *pBurst_size);

/* Function Name:
 *      dal_cypress_rate_igrBwCtrlBurstSize_set
 * Description:
 *      Set burst size of ingress bandwidth
 * Input:
 *      unit        - unit id
 *      burst_size  - burst size
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 * Note:
 *      After setting the API, please adjust the low threshold by dal_cypress_rate_igrBandwidthLowThresh_set.
 *      The burst_size unit is 128 Bytes and the maximum of burst_size setting range is under 65535.
 */
extern int32
dal_cypress_rate_igrBwCtrlBurstSize_set(uint32 unit, uint32 burst_size);

/* Function Name:
 *      dal_cypress_rate_portEgrBwCtrlEnable_get
 * Description:
 *      Get the egress bandwidth control status.
 * Input:
 *      unit                - unit id
 *      port                - port id
 * Output:
 *      pEnable            - status of egress bandwidth control
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID       - Invalid port id
 *      RT_ERR_NULL_POINTER  - NULL pointer
 * Note:
 *    The status of egress bandwidth control is as following:
 *    - DISABLED
 *    - ENABLED
 */
extern int32
dal_cypress_rate_portEgrBwCtrlEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_cypress_rate_portEgrBwCtrlEnable_set
 * Description:
 *      Set the egress bandwidth control status.
 * Input:
 *      unit                - unit id
 *      port                - port id
 *      enable              - status of egress bandwidth control
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID       - Invalid port id
 *      RT_ERR_INPUT     - Invalid input parameter
 * Note:
 *    The status of egress bandwidth control is as following:
 *    - DISABLED
 *    - ENABLED
 */
extern int32
dal_cypress_rate_portEgrBwCtrlEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      dal_cypress_rate_portEgrBwCtrlRate_get
 * Description:
 *      Get the egress bandwidth control rate.
 * Input:
 *      unit            - unit id
 *      port            - port id
 * Output:
 *      pRate          - egress bandwidth control rate
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID       - Invalid port id
 *      RT_ERR_NULL_POINTER  - NULL pointer
 * Note:
 *    The actual rate is "rate * chip granularity".
 *    The unit of granularity in RTL8389/RTL8329/RTL8390 is 16Kbps.
 */
extern int32
dal_cypress_rate_portEgrBwCtrlRate_get(uint32 unit, rtk_port_t port, uint32 *pRate);

/* Function Name:
 *      dal_cypress_rate_portEgrBwCtrlRate_set
 * Description:
 *      Set the egress bandwidth control rate.
 * Input:
 *      unit                - unit id
 *      port                - port id
 *      rate                - egress bandwidth control rate
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID       - Invalid port id
 *      RT_ERR_RATE          - Invalid input rate
 * Note:
 *    The actual rate is "rate * chip granularity".
 *    The unit of granularity in RTL8389/RTL8329/RTL8390 is 16Kbps.
 */
extern int32
dal_cypress_rate_portEgrBwCtrlRate_set(uint32 unit, rtk_port_t port, uint32 rate);

/* Function Name:
 *      dal_cypress_rate_egrBandwidthCtrlIncludeIfg_get
 * Description:
 *      Get the status of egress bandwidth control includes IFG or not.
 * Input:
 *      unit                    - unit id
 * Output:
 *      pIfg_include           - include IFG or not
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT                 - Invalid input parameter
 *      RT_ERR_NULL_POINTER     - NULL pointer
 * Note:
 *      1. Egress bandwidth control includes/excludes the Preamble & IFG (20 Bytes).
 *
 *      2. The status of ifg_include:
 *         - DISABLED
 *         - ENABLED
 */
extern int32
dal_cypress_rate_egrBandwidthCtrlIncludeIfg_get(uint32 unit, rtk_enable_t *pIfg_include);

/* Function Name:
 *      dal_cypress_rate_egrBandwidthCtrlIncludeIfg_set
 * Description:
 *      Set the status of egress bandwidth control includes IFG or not.
 * Input:
 *      unit                    - unit id
 *      ifg_include             - include IFG or not
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT                 - Invalid input parameter
 * Note:
 *      1. Egress bandwidth control includes/excludes the Preamble & IFG (20 Bytes).
 *
 *      2. The status of ifg_include:
 *         - DISABLED
 *         - ENABLED
 */
extern int32
dal_cypress_rate_egrBandwidthCtrlIncludeIfg_set(uint32 unit, rtk_enable_t ifg_include);

/* Function Name:
 *      dal_cypress_rate_egrPortBwCtrlBurstSize_get
 * Description:
 *      Get burst size of port egress bandwidth
 * Input:
 *      unit        - unit id
 * Output:
 *      pBurst_size - pointer to burst size
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_rate_egrPortBwCtrlBurstSize_get(
    uint32                  unit,
    uint32                  *pBurst_size);

/* Function Name:
 *      dal_cypress_rate_egrPortBwCtrlBurstSize_set
 * Description:
 *      Set burst size of port egress bandwidth
 * Input:
 *      unit        - unit id
 *      burst_size - burst size
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 * Note:
 *      None
 */
extern int32
dal_cypress_rate_egrPortBwCtrlBurstSize_set(
    uint32                  unit,
    uint32                  burst_size);

/* Function Name:
 *      dal_cypress_rate_egrQueueBwCtrlBurstSize_get
 * Description:
 *      Get burst size of queue egress bandwidth
 * Input:
 *      unit        - unit id
 * Output:
 *      pBurst_size - pointer to burst size
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_rate_egrQueueBwCtrlBurstSize_get(
    uint32                  unit,
    uint32                  *pBurst_size);

/* Function Name:
 *      dal_cypress_rate_egrQueueBwCtrlBurstSize_set
 * Description:
 *      Set burst size of queue egress bandwidth
 * Input:
 *      unit        - unit id
 *      burst_size - burst size
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 * Note:
 *      None
 */
extern int32
dal_cypress_rate_egrQueueBwCtrlBurstSize_set(
    uint32                  unit,
    uint32                  burst_size);

/* Function Name:
 *      dal_cypress_rate_cpuEgrBandwidthCtrlRateMode_get
 * Description:
 *      Get rate counting mode of CPU port egress bandwidth control.
 * Input:
 *      unit       - unit id
 * Output:
 *      pMode      - pointer to rate counting mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID           - invalid unit id
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 * Note:
 *      The rate mode are as following:
 *      - RATE_MODE_BYTE
 *      - RATE_MODE_PKT
 */
extern int32
dal_cypress_rate_cpuEgrBandwidthCtrlRateMode_get(uint32 unit, rtk_rate_rateMode_t *pMode);

/* Function Name:
 *      dal_cypress_rate_cpuEgrBandwidthCtrlRateMode_set
 * Description:
 *      Set rate counting mode of CPU port egress bandwidth control.
 * Input:
 *      unit      - unit id
 *      mode      - Rate counting mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      The rate mode are as following:
 *      - RATE_MODE_BYTE
 *      - RATE_MODE_PKT
 */
extern int32
dal_cypress_rate_cpuEgrBandwidthCtrlRateMode_set(uint32 unit, rtk_rate_rateMode_t mode);

/* Function Name:
 *      dal_cypress_rate_portEgrQueueBwCtrlEnable_get
 * Description:
 *      Get enable status of egress bandwidth control on specified queue.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      queue   - queue id
 * Output:
 *      pEnable - Pointer to enable status of egress queue bandwidth control
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_QUEUE_ID         - invalid queue id
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_rate_portEgrQueueBwCtrlEnable_get(
    uint32          unit,
    rtk_port_t      port,
    rtk_qid_t       queue,
    rtk_enable_t    *pEnable);

/* Function Name:
 *      dal_cypress_rate_portEgrQueueBwCtrlEnable_set
 * Description:
 *      Set enable status of egress bandwidth control on specified queue.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      queue  - queue id
 *      enable - enable status of egress queue bandwidth control
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_QUEUE_ID         - invalid queue id
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_cypress_rate_portEgrQueueBwCtrlEnable_set(
    uint32          unit,
    rtk_port_t      port,
    rtk_qid_t       queue,
    rtk_enable_t    enable);

/* Function Name:
 *      dal_cypress_rate_portEgrQueueBwCtrlRate_get
 * Description:
 *      Get rate of egress bandwidth control on specified queue.
 * Input:
 *      unit  - unit id
 *      port  - port id
 *      queue - queue id
 * Output:
 *      pRate - pointer to rate of egress queue bandwidth control
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_QUEUE_ID         - invalid queue id
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *    The actual rate is "rate * chip granularity".
 *    The unit of granularity is 16Kbps.
 */
extern int32
dal_cypress_rate_portEgrQueueBwCtrlRate_get(
    uint32      unit,
    rtk_port_t  port,
    rtk_qid_t   queue,
    uint32      *pRate);

/* Function Name:
 *      dal_cypress_rate_portEgrQueueBwCtrlRate_set
 * Description:
 *      Set rate of egress bandwidth control on specified queue.
 * Input:
 *      unit  - unit id
 *      port  - port id
 *      queue - queue id
 *      rate  - rate of egress queue bandwidth control
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_QUEUE_ID         - invalid queue id
 *      RT_ERR_RATE             - Invalid input rate
 * Note:
 *    The actual rate is "rate * chip granularity".
 *    The unit of granularity is 16Kbps.
 */
extern int32
dal_cypress_rate_portEgrQueueBwCtrlRate_set(
    uint32      unit,
    rtk_port_t  port,
    rtk_qid_t   queue,
    uint32      rate);

/* Function Name:
 *      dal_cypress_rate_egrQueueFixedBandwidthEnable_get
 * Description:
 *      Get enable status of fixed bandwidth ability on specified queue.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      queue   - queue id
 * Output:
 *      pEnable - pointer to enable status of fixed bandwidth ability
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_QUEUE_ID     - invalid queue id
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_rate_egrQueueFixedBandwidthEnable_get(
    uint32          unit,
    rtk_port_t      port,
    rtk_qid_t       queue,
    rtk_enable_t    *pEnable);

/* Function Name:
 *      dal_cypress_rate_egrQueueFixedBandwidthEnable_set
 * Description:
 *      Set enable status of fixed bandwidth ability on specified queue.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      queue   - queue id
 *      enable  - enable status of fixed bandwidth ability
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_QUEUE_ID     - invalid queue id
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_cypress_rate_egrQueueFixedBandwidthEnable_set(
    uint32          unit,
    rtk_port_t      port,
    rtk_qid_t       queue,
    rtk_enable_t    enable);

/* Function Name:
 *      dal_cypress_rate_portStormCtrlEnable_get
 * Description:
 *      Get enable status of storm control on specified port.
 * Input:
 *      unit       - unit id
 *      port       - port id
 *      storm_type - storm group type
 * Output:
 *      pEnable    - pointer to enable status of storm control
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_PORT_ID           - invalid port id
 *      RT_ERR_SFC_UNKNOWN_GROUP - Unknown storm group
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 * Note:
 *      The storm group types are as following:
 *          - STORM_GROUP_MULTICAST
 *          - STORM_GROUP_BROADCAST
 *          - STORM_GROUP_UNICAST
 */
extern int32
dal_cypress_rate_portStormCtrlEnable_get(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_group_t  storm_type,
    rtk_enable_t            *pEnable);

/* Function Name:
 *      dal_cypress_rate_portStormCtrlEnable_set
 * Description:
 *      Set enable status of storm control on specified port.
 * Input:
 *      unit       - unit id
 *      port       - port id
 *      storm_type - storm group type
 *      enable     - enable status of storm control
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_PORT_ID           - invalid port id
 *      RT_ERR_SFC_UNKNOWN_GROUP - Unknown storm group
 *      RT_ERR_INPUT             - invalid input parameter
 * Note:
 *      The storm group types are as following:
 *          - STORM_GROUP_MULTICAST
 *          - STORM_GROUP_BROADCAST
 *          - STORM_GROUP_UNICAST
 */
extern int32
dal_cypress_rate_portStormCtrlEnable_set(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_group_t  storm_type,
    rtk_enable_t            enable);

/* Function Name:
 *      dal_cypress_rate_portStormCtrlRate_get
 * Description:
 *      Get the storm control rate.
 * Input:
 *      unit            - unit id
 *      port            - port id
 *      storm_type      - storm group type
 * Output:
 *      pRate          - storm control rate (packet-per-second).
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID       - Invalid port id
 *      RT_ERR_INPUT         - Invalid input parameter
 *      RT_ERR_NULL_POINTER  - NULL pointer
 * Note:
 *      The storm group types are as following:
 *      - STORM_GROUP_MULTICAST
 *      - STORM_GROUP_BROADCAST
 *      - STORM_GROUP_UNICAST
 */
extern int32
dal_cypress_rate_portStormCtrlRate_get(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_group_t  storm_type,
    uint32                  *pRate);

/* Function Name:
 *      dal_cypress_rate_portStormCtrlRate_set
 * Description:
 *      Set the storm control rate.
 * Input:
 *      unit            - unit id
 *      port            - port id
 *      storm_type      - storm group type
 *      rate            - storm control rate (packet-per-second).
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID       - Invalid port id
 *      RT_ERR_INPUT         - Invalid input parameter
 *      RT_ERR_RATE          - Invalid input bandwidth
 * Note:
 *      The storm group types are as following:
 *      - STORM_GROUP_MULTICAST
 *      - STORM_GROUP_BROADCAST
 *      - STORM_GROUP_UNICAST
 */
extern int32
dal_cypress_rate_portStormCtrlRate_set(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_group_t  storm_type,
    uint32                  rate);

/* Function Name:
 *      dal_cypress_rate_portStormCtrlBurstSize_get
 * Description:
 *      Get burst size of storm control on specified port.
 * Input:
 *      unit        - unit id
 *      port        - port id
 *      storm_type  - storm group type
 * Output:
 *      pBurst_rate - pointer to burst rate
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_INPUT        - Invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The storm group types are as following:
 *      - STORM_GROUP_MULTICAST
 *      - STORM_GROUP_BROADCAST
 *      - STORM_GROUP_UNICAST
 */
extern int32
dal_cypress_rate_portStormCtrlBurstSize_get(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_group_t  storm_type,
    uint32                  *pBurst_rate);

/* Function Name:
 *      dal_cypress_rate_portStormCtrlBurstSize_set
 * Description:
 *      Set burst size of storm control on specified port.
 * Input:
 *      unit       - unit id
 *      port       - port id
 *      storm_type - storm group type
 *      burst_rate - burst rate
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - Invalid input parameter
 * Note:
 *      The storm group types are as following:
 *      - STORM_GROUP_MULTICAST
 *      - STORM_GROUP_BROADCAST
 *      - STORM_GROUP_UNICAST
 *      The (minimum, maximum) of burst_rate setting range of normal port is (1700, 65535).
 *      The (minimum, maximum) of burst_rate setting range of 10G port is (2650, 1048575).
 */
extern int32
dal_cypress_rate_portStormCtrlBurstSize_set(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_group_t  storm_type,
    uint32                  burst_rate);

/* Function Name:
 *      dal_cypress_rate_portStormCtrlTypeSel_get
 * Description:
 *      Get the storm control type.
 * Input:
 *      unit            - unit id
 *      port            - port id
 *      storm_type      - storm group type
 * Output:
 *      pStorm_sel      - storm selection
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID       - Invalid port id
 *      RT_ERR_INPUT         - Invalid input parameter
 *      RT_ERR_NULL_POINTER  - NULL pointer
 * Note:
 *    The API is only supported in unicast and multicast, the storm group types are as following:
 *    - STORM_GROUP_UNICAST
 *    - STORM_GROUP_MULTICAST
 *
 *    The storm selection are as following:
 *    - STORM_SEL_UNKNOWN
 *    - STORM_SEL_UNKNOWN_AND_KNOWN
 */
extern int32
dal_cypress_rate_portStormCtrlTypeSel_get(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_group_t  storm_type,
    rtk_rate_storm_sel_t    *pStorm_sel);

/* Function Name:
 *      dal_cypress_rate_portStormCtrlTypeSel_set
 * Description:
 *      Set the storm control type.
 * Input:
 *      unit            - unit id
 *      port            - port id
 *      storm_type      - storm group type
 *      storm_sel       - storm selection
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID       - Invalid port id
 *      RT_ERR_INPUT         - Invalid input parameter
 * Note:
 *    The API is only supported in unicast and multicast, the storm group types are as following:
 *    - STORM_GROUP_UNICAST
 *    - STORM_GROUP_MULTICAST
 *
 *    The storm selection are as following:
 *    - STORM_SEL_UNKNOWN
 *    - STORM_SEL_UNKNOWN_AND_KNOWN
 */
extern int32
dal_cypress_rate_portStormCtrlTypeSel_set(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_group_t  storm_type,
    rtk_rate_storm_sel_t    storm_sel);

/* Function Name:
 *      dal_cypress_rate_portStormCtrlExceed_get
 * Description:
 *      Get exceed status of storm control on specified port.
 * Input:
 *      unit       - unit id
 *      port       - port id
 *      storm_type - storm group type
 * Output:
 *      pIsExceed  - pointer to exceed status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_PORT_ID              - invalid port id
 *      RT_ERR_INPUT                - Invalid input parameter
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 *      Exceed status is as following
 *      - TRUE      - storm rate is more than configured rate.
 *      - FALSE     - storm rate is never over then configured rate.
 */
extern int32
dal_cypress_rate_portStormCtrlExceed_get(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_group_t  storm_type,
    uint32                  *pIsExceed);

/* Function Name:
 *      dal_cypress_rate_portStormCtrlExceed_reset
 * Description:
 *      Reset exceed status of storm control on specified port.
 * Input:
 *      unit       - unit id
 *      port       - port id
 *      storm_type - storm group type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_PORT_ID              - invalid port id
 *      RT_ERR_INPUT                - Invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_cypress_rate_portStormCtrlExceed_reset(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_group_t  storm_type);

/* Function Name:
 *      dal_cypress_rate_portStormCtrlProtoRate_get
 * Description:
 *      Get the storm control rate.
 * Input:
 *      unit            - unit id
 *      port            - port id
 *      storm_type      - storm group type
 * Output:
 *      pRate          - storm control rate (packet-per-second).
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID       - Invalid port id
 *      RT_ERR_INPUT         - Invalid input parameter
 *      RT_ERR_NULL_POINTER  - NULL pointer
 * Note:
 *    The storm group types are as following:
 *    - STORM_PROTO_GROUP_BPDU
 *    - STORM_PROTO_GROUP_IGMP
 *    - STORM_PROTO_GROUP_ARP
 */
extern int32
dal_cypress_rate_portStormCtrlProtoRate_get(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_proto_group_t  storm_type,
    uint32                  *pRate);

/* Function Name:
 *      dal_cypress_rate_portStormCtrlProtoRate_set
 * Description:
 *      Set the storm control rate.
 * Input:
 *      unit            - unit id
 *      port            - port id
 *      storm_type      - storm group type
 *      rate            - storm control rate (packet-per-second).
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID       - Invalid port id
 *      RT_ERR_INPUT         - Invalid input parameter
 *      RT_ERR_RATE          - Invalid input bandwidth
 * Note:
 *    The storm group types are as following:
 *    - STORM_PROTO_GROUP_BPDU
 *    - STORM_PROTO_GROUP_IGMP
 *    - STORM_PROTO_GROUP_ARP
 */
extern int32
dal_cypress_rate_portStormCtrlProtoRate_set(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_proto_group_t  storm_type,
    uint32                  rate);

/* Function Name:
 *      dal_cypress_rate_portStormCtrlProtoExceed_get
 * Description:
 *      Get exceed status of storm protocol control on specified port.
 * Input:
 *      unit       - unit id
 *      port       - port id
 *      storm_type - storm group type
 * Output:
 *      pIsExceed  - pointer to exceed status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_PORT_ID              - invalid port id
 *      RT_ERR_INPUT                - Invalid input parameter
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 *      Exceed status is as following
 *      - TRUE      - storm rate is more than configured rate.
 *      - FALSE     - storm rate is never over then configured rate.
 */
extern int32
dal_cypress_rate_portStormCtrlProtoExceed_get(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_proto_group_t  storm_type,
    uint32                  *pIsExceed);

/* Function Name:
 *      dal_cypress_rate_portStormCtrlProtoExceed_reset
 * Description:
 *      Reset exceed status of storm protocol control on specified port.
 * Input:
 *      unit       - unit id
 *      port       - port id
 *      storm_type - storm group type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_PORT_ID              - invalid port id
 *      RT_ERR_INPUT                - Invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_cypress_rate_portStormCtrlProtoExceed_reset(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_proto_group_t  storm_type);

/* Function Name:
 *      dal_cypress_rate_stormControlBypass_get
 * Description:
 *      Get the status of bypass storm control for specified packet type.
 * Input:
 *      unit         - unit id
 *      bypassType   - bypass type
 * Output:
 *      pEnable      - pointer to enable status of bypass storm control
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT        - Invalid input parameter
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      None.
 */
extern int32
dal_cypress_rate_stormControlBypass_get(uint32 unit, rtk_rate_storm_bypassType_t bypassType, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_cypress_rate_stormControlBypass_set
 * Description:
 *      Set the status of bypass storm control for specified packet type.
 * Input:
 *      unit         - unit id
 *      bypassType   - bypass type
 *      enable       - status of bypass storm control
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT   - Invalid input parameter
 * Note:
 *      None.
 */
extern int32
dal_cypress_rate_stormControlBypass_set(uint32 unit, rtk_rate_storm_bypassType_t bypassType, rtk_enable_t enable);

/* Function Name:
 *      dal_cypress_rate_stormControlIncludeIfg_get
 * Description:
 *      Get enable status of includes IFG for storm control.
 * Input:
 *      unit         - unit id
 * Output:
 *      pIfg_include - pointer to enable status of includes IFG
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_rate_stormControlIncludeIfg_get(uint32 unit, rtk_enable_t *pIfg_include);

/* Function Name:
 *      dal_cypress_rate_stormControlIncludeIfg_set
 * Description:
 *      Set enable status of includes IFG for storm control.
 * Input:
 *      unit        - unit id
 *      ifg_include - enable status of includes IFG
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
dal_cypress_rate_stormControlIncludeIfg_set(uint32 unit, rtk_enable_t ifg_include);

/* Function Name:
 *      dal_cypress_rate_stormControlRateMode_get
 * Description:
 *      Get rate counting mode of storm control.
 * Input:
 *      unit  - unit id
 * Output:
 *      pRate_mode - pointer to rate counting mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *    (1) The rate mode are as following:
 *      - BASED_ON_PKT
 *      - BASED_ON_BYTE
 */
extern int32
dal_cypress_rate_stormControlRateMode_get(uint32 unit, rtk_rate_storm_rateMode_t *pMode);

/* Function Name:
 *      dal_cypress_rate_stormControlRateMode_set
 * Description:
 *      Set rate counting mode of storm control.
 * Input:
 *      unit - unit id
 *      rate_mode  - Rate counting mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *    (1) The rate mode are as following:
 *      - BASED_ON_PKT
 *      - BASED_ON_BYTE
 */
extern int32
dal_cypress_rate_stormControlRateMode_set(uint32 unit, rtk_rate_storm_rateMode_t mode);


/* Function Name:
 *      dal_cypress_rate_includeIfg_get
 * Description:
 *      Get enable status of includes IFG.
 * Input:
 *      unit         - unit id
 *      module  - rate module
 * Output:
 *      pIfg_include - pointer to enable status of includes IFG
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) Includes/excludes the preamble & IFG (20 Bytes).
 *      (2) The status of ifg_include:
 *          - DISABLED: exclude preamble & IFG
 *          - ENABLED: include preamble & IFG
 */
extern int32
dal_cypress_rate_includeIfg_get(uint32 unit, rtk_rate_module_t module, rtk_enable_t *pIfg_include);

/* Function Name:
 *      dal_cypress_rate_includeIfg_set
 * Description:
 *      Set enable status of includes IFG.
 * Input:
 *      unit        - unit id
 *      module  - rate module
 *      ifg_include - enable status of includes IFG
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      (1) Includes/excludes the preamble & IFG (20 Bytes).
 *      (2) The status of ifg_include:
 *          - DISABLED: exclude preamble & IFG
 *          - ENABLED: include preamble & IFG
 */
extern int32
dal_cypress_rate_includeIfg_set(uint32 unit, rtk_rate_module_t module, rtk_enable_t ifg_include);

#endif /* __DAL_cypress_RATE_H__ */
