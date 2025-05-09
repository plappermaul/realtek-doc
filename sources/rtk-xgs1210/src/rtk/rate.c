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
 * $Revision: 94857 $
 * $Date: 2019-01-16 13:46:55 +0800 (Wed, 16 Jan 2019) $
 *
 * Purpose : Definition of Port Bandwidth Control and Storm Control API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) Configuration of Ingress Port Bandwidth Control [Ingress Rate Limit]
 *           (2) Configuration of Egress  Port Bandwidth Control [Egress  Rate Limit]
 *           (3) Configuration of Storm Control
 *
 */

/*
 * Include Files
 */
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <hal/common/halctrl.h>
#include <dal/dal_mgmt.h>
#include <rtk/default.h>
#include <rtk/rate.h>

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
 *      rtk_rate_init
 * Description:
 *      Initial the rate module of the specified device..
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
 *      Must initialize rate module before calling any rate APIs.
 * Changes:
 *      None
 */
int32
rtk_rate_init(uint32 unit)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_init(unit);
} /* end of rtk_rate_init */

/* Module Name    : Rate                                            */
/* Sub-module Name: Configuration of ingress port bandwidth control */

#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
/* Function Name:
 *      rtk_rate_igrBandwidthCtrlIncludeIfg_get
 * Description:
 *      Get the status of ingress bandwidth control includes IFG or not.
 * Input:
 *      unit         - unit id
 * Output:
 *      pIfg_include - include IFG or not
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - Invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) Ingress bandwidth control includes/excludes the Preamble & IFG (20 Bytes).
 *      (2) The status of ifg_include:
 *          - DISABLED
 *          - ENABLED
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_rate_includeIfg_get(unit, module, pIfg_include)
 *          Parameters:
 *              module                   -  RATE_MODULE_IGR
 *              pIfg_include             -  pIfg_include
 */
int32
rtk_rate_igrBandwidthCtrlIncludeIfg_get(uint32 unit, rtk_enable_t *pIfg_include)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_includeIfg_get(unit, RATE_MODULE_IGR, pIfg_include);
} /* end of rtk_rate_igrBandwidthCtrlIncludeIfg_get */

/* Function Name:
 *      rtk_rate_igrBandwidthCtrlIncludeIfg_set
 * Description:
 *      Set the status of ingress bandwidth control includes IFG or not.
 * Input:
 *      unit        - unit id
 *      ifg_include - include IFG or not
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - Invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - Invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) Ingress bandwidth control includes/excludes the Preamble & IFG (20 Bytes).
 *      (2) The status of ifg_include:
 *          - DISABLED
 *          - ENABLED
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_rate_includeIfg_set(unit, module, ifg_include)
 *          Parameters:
 *              module                   -  RATE_MODULE_IGR
 *              ifg_include             -   ifg_include
 */
int32
rtk_rate_igrBandwidthCtrlIncludeIfg_set(uint32 unit, rtk_enable_t ifg_include)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_includeIfg_set(unit, RATE_MODULE_IGR, ifg_include);
} /* end of rtk_rate_igrBandwidthCtrlIncludeIfg_set */

/* Function Name:
 *      rtk_rate_egrBandwidthCtrlIncludeIfg_get
 * Description:
 *      Get the status of egress bandwidth control includes IFG or not.
 * Input:
 *      unit         - unit id
 * Output:
 *      pIfg_include - include IFG or not
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - Invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) Egress bandwidth control includes/excludes the Preamble & IFG (20 Bytes).
 *      (2) The status of ifg_include:
 *          - DISABLED
 *          - ENABLED
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_rate_includeIfg_get(unit, module, pIfg_include)
 *          Parameters:
 *              module                   -  RATE_MODULE_EGR
 *              pIfg_include             -  pIfg_include
 */
int32
rtk_rate_egrBandwidthCtrlIncludeIfg_get(uint32 unit, rtk_enable_t *pIfg_include)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_includeIfg_get(unit, RATE_MODULE_EGR, pIfg_include);
} /* end of rtk_rate_egrBandwidthCtrlIncludeIfg_get */

/* Function Name:
 *      rtk_rate_egrBandwidthCtrlIncludeIfg_set
 * Description:
 *      Set the status of egress bandwidth control includes IFG or not.
 * Input:
 *      unit        - unit id
 *      ifg_include - include IFG or not
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - Invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - Invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) Egress bandwidth control includes/excludes the Preamble & IFG (20 Bytes).
 *      (2) The status of ifg_include:
 *          - DISABLED
 *          - ENABLED
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_rate_includeIfg_set(unit, module, ifg_include)
 *          Parameters:
 *              module                   -  RATE_MODULE_EGR
 *              ifg_include             -   ifg_include
 */
int32
rtk_rate_egrBandwidthCtrlIncludeIfg_set(uint32 unit, rtk_enable_t ifg_include)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_includeIfg_set(unit, RATE_MODULE_EGR, ifg_include);
} /* end of rtk_rate_egrBandwidthCtrlIncludeIfg_set */

/* Function Name:
 *      rtk_rate_stormControlIncludeIfg_get
 * Description:
 *      Get enable status of includes IFG for storm control.
 * Input:
 *      unit         - unit id
 * Output:
 *      pIfg_include - pointer to enable status of includes IFG
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) Storm control includes/excludes the preamble & IFG (20 Bytes).
 *      (2) The status of ifg_include:
 *          - DISABLED: exclude preamble & IFG
 *          - ENABLED: include preamble & IFG
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_rate_includeIfg_get(unit, module, pIfg_include)
 *          Parameters:
 *              module                   -  RATE_MODULE_STORM
 *              pIfg_include             -  pIfg_include
 */
int32
rtk_rate_stormControlIncludeIfg_get(uint32 unit, rtk_enable_t *pIfg_include)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_includeIfg_get(unit, RATE_MODULE_STORM, pIfg_include);
} /* end of rtk_rate_stormControlIncludeIfg_get */

/* Function Name:
 *      rtk_rate_stormControlIncludeIfg_set
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
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) Storm control includes/excludes the preamble & IFG (20 Bytes).
 *      (2) The status of ifg_include:
 *          - DISABLED: exclude preamble & IFG
 *          - ENABLED: include preamble & IFG
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_rate_includeIfg_set(unit, module, ifg_include)
 *          Parameters:
 *              module                   -  RATE_MODULE_STORM
 *              ifg_include             -   ifg_include
 */
int32
rtk_rate_stormControlIncludeIfg_set(uint32 unit, rtk_enable_t ifg_include)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_includeIfg_set(unit, RATE_MODULE_STORM, ifg_include);
} /* end of rtk_rate_stormControlIncludeIfg_set */
#endif

/* Function Name:
 *      rtk_rate_includeIfg_get
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
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) Includes/excludes the preamble & IFG (20 Bytes).
 *      (2) The status of ifg_include:
 *          - DISABLED: exclude preamble & IFG
 *          - ENABLED: include preamble & IFG
 * Changes:
 *      [SDK_3.0.0]
 *          (1)New added function.
 *          (2)It obsoletes these functions:
 *              rtk_rate_egrBandwidthCtrlIncludeIfg_get
 *              rtk_rate_igrBandwidthCtrlIncludeIfg_get
 *              rtk_rate_stormControlIncludeIfg_get
 */
int32
rtk_rate_includeIfg_get(uint32 unit, rtk_rate_module_t module, rtk_enable_t *pIfg_include)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_includeIfg_get(unit, module, pIfg_include);
} /* end of rtk_rate_includeIfg_get */

/* Function Name:
 *      rtk_rate_includeIfg_set
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
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) Includes/excludes the preamble & IFG (20 Bytes).
 *      (2) The status of ifg_include:
 *          - DISABLED: exclude preamble & IFG
 *          - ENABLED: include preamble & IFG
 * Changes:
 *      [SDK_3.0.0]
 *          (1)New added function.
 *          (2)It obsoletes these functions:
 *              rtk_rate_egrBandwidthCtrlIncludeIfg_set
 *              rtk_rate_igrBandwidthCtrlIncludeIfg_set
 *              rtk_rate_stormControlIncludeIfg_set
 */
int32
rtk_rate_includeIfg_set(uint32 unit, rtk_rate_module_t module, rtk_enable_t ifg_include)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_includeIfg_set(unit, module, ifg_include);
} /* end of rtk_rate_includeIfg_set */

/* Function Name:
 *      rtk_rate_portIgrBwCtrlEnable_get
 * Description:
 *      Get the ingress bandwidth control status.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - status of ingress bandwidth control
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - Invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      The status of ingress bandwidth control is as following:
 *      - DISABLED
 *      - ENABLED
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_rate_igrBandwidthCtrlEnable_get)
 */
int32
rtk_rate_portIgrBwCtrlEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_portIgrBwCtrlEnable_get(unit, port, pEnable);

} /* end of rtk_rate_portIgrBwCtrlEnable_get */

/* Function Name:
 *      rtk_rate_portIgrBwCtrlEnable_set
 * Description:
 *      Set the ingress bandwidth control status.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - status of ingress bandwidth control
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - Invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - Invalid port id
 *      RT_ERR_INPUT    - Invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      The status of ingress bandwidth control is as following:
 *      - DISABLED
 *      - ENABLED
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_rate_igrBandwidthCtrlEnable_set)
 */
int32
rtk_rate_portIgrBwCtrlEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_portIgrBwCtrlEnable_set(unit, port, enable);

} /* end of rtk_rate_portIgrBwCtrlEnable_set */

/* Function Name:
 *      rtk_rate_portIgrBwCtrlRate_get
 * Description:
 *      Get the ingress bandwidth control rate.
 * Input:
 *      unit  - unit id
 *      port  - port id
 * Output:
 *      pRate - ingress bandwidth control rate
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - Invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The actual rate is "rate * chip granularity".
 *      (2) The unit of granularity is 16Kbps.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_rate_igrBandwidthCtrlRate_get)
 */
int32
rtk_rate_portIgrBwCtrlRate_get(uint32 unit, rtk_port_t port, uint32 *pRate)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_portIgrBwCtrlRate_get(unit, port, pRate);

} /* end of rtk_rate_portIgrBwCtrlRate_get */

/* Function Name:
 *      rtk_rate_portIgrBwCtrlRate_set
 * Description:
 *      Set the ingress bandwidth control rate.
 * Input:
 *      unit - unit id
 *      port - port id
 *      rate - ingress bandwidth control rate
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - Invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - Invalid port id
 *      RT_ERR_RATE     - Invalid input rate
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The actual rate is "rate * chip granularity".
 *      (2) The unit of granularity is 16Kbps in 8390 and 8380.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_rate_igrBandwidthCtrlRate_set)
 */
int32
rtk_rate_portIgrBwCtrlRate_set(uint32 unit, rtk_port_t port, uint32 rate)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_portIgrBwCtrlRate_set(unit, port, rate);

} /* end of rtk_rate_portIgrBwCtrlRate_set */

/* Function Name:
 *      rtk_rate_igrBandwidthLowThresh_get
 * Description:
 *      Get the flow control turn ON/OFF low threshold of ingress bandwidth.
 * Input:
 *      unit         - unit id
 * Output:
 *      pLowThresh   - pointer to flow control turn ON/OFF low threshold
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390, 9300
 * Note:
 *      The threshold unit is 128 Byte.
 * Changes:
 *      None
 */
int32
rtk_rate_igrBandwidthLowThresh_get(uint32 unit, uint32 *pLowThresh)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_igrBandwidthLowThresh_get(unit, pLowThresh);
} /* end of rtk_rate_igrBandwidthLowThresh_get */

/* Function Name:
 *      rtk_rate_igrBandwidthLowThresh_set
 * Description:
 *      Set the flow control turn ON/OFF low threshold of ingress bandwidth.
 * Input:
 *      unit        - unit id
 *      lowThresh   - flow control turn ON/OFF low threshold
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - Invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - Invalid input parameter
 * Applicable:
 *      8390, 9300
 * Note:
 *      The threshold unit is 128 Byte.
 *      After setting the API, please adjust the low threshold by rtk_rate_igrBwCtrlBurstSize_set.
 * Changes:
 *      None
 */
int32
rtk_rate_igrBandwidthLowThresh_set(uint32 unit, uint32 lowThresh)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_igrBandwidthLowThresh_set(unit, lowThresh);
} /* end of rtk_rate_igrBandwidthLowThresh_set */

#if defined(CONFIG_SDK_DRIVER_RTK_LEGACY_API)
/* Function Name:
 *      rtk_rate_portIgrBandwidthHighThresh_get
 * Description:
 *      Get the flow control turn ON/OFF high threshold of ingress bandwidth for specified port.
 * Input:
 *      unit        - unit id
 *      port        - port id
 * Output:
 *      pHighThresh - pointer to flow control turn ON/OFF high threshold
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - Invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390
 * Note:
 *      The threshold unit is 128 Byte.
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_rate_portIgrBwCtrlBurstSize_get(unit, port, pCfg)
 *          Parameters:
 *              port                -  port
 *              pCfg->burst_high    -  *pHighThresh
 *              pCfg->burst_low     -  *pHighThresh
 */
int32
rtk_rate_portIgrBandwidthHighThresh_get(uint32 unit, rtk_port_t port, uint32 *pHighThresh)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_portIgrBandwidthHighThresh_get(unit, port, pHighThresh);
} /* end of rtk_rate_portIgrBandwidthHighThresh_get */

/* Function Name:
 *      rtk_rate_portIgrBandwidthHighThresh_set
 * Description:
 *      Set the flow control turn ON/OFF high threshold of ingress bandwidth for specified port.
 * Input:
 *      unit       - unit id
 *      port       - port id
 *      highThresh - flow control turn ON/OFF high threshold
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - Invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - Invalid port id
 *      RT_ERR_INPUT    - Invalid input parameter
 * Applicable:
 *      8390
 * Note:
 *      The threshold unit is 128 Byte.
 *      The (minimum, maximum) of highThresh setting range is (22, 65535).
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_rate_portIgrBwCtrlBurstSize_set(unit, port, pCfg)
 *          Parameters:
 *              port                -  port
 *              pCfg->burst_high    -  highThresh
 *              pCfg->burst_low    -  highThresh
 */
int32
rtk_rate_portIgrBandwidthHighThresh_set(uint32 unit, rtk_port_t port, uint32 highThresh)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_portIgrBandwidthHighThresh_set(unit, port, highThresh);
} /* end of rtk_rate_portIgrBandwidthHighThresh_set */
#endif

/* Function Name:
 *      rtk_rate_igrBwCtrlBurstSize_get
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
 * Applicable:
 *      8390, 9300
 * Note:
 *      In 8390, the burst size unit is 128 Bytes.
 *      In 9300, the burst size unit is 1 Byte.
 * Changes:
 *      None
 */
int32
rtk_rate_igrBwCtrlBurstSize_get(uint32 unit, uint32 *pBurst_size)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_igrBwCtrlBurstSize_get(unit, pBurst_size);
}/* end of rtk_rate_igrBwCtrlBurstSize_get */

/* Function Name:
 *      rtk_rate_igrBwCtrlBurstSize_set
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
 * Applicable:
 *      8390, 9300
 * Note:
 *      In 8390, the burst_size unit is 128 Bytes and the (minimum, maximum) of burst_size setting range is (22, 65535).
 *      In 9300, the burst_size unit is 1 Byte and the (minimum, maximum) of burst_size setting range is (393, 0x7FFFFFFF).
 *      After setting the API, please adjust the low threshold by rtk_rate_igrBandwidthLowThresh_set.
 * Changes:
 *      None
 */
int32
rtk_rate_igrBwCtrlBurstSize_set(uint32 unit, uint32 burst_size)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_igrBwCtrlBurstSize_set(unit, burst_size);
}/* end of rtk_rate_igrBwCtrlBurstSize_set */

/* Function Name:
 *      rtk_rate_igrPortBwCtrlBurstSize_get
 * Description:
 *      Get burst size of port ingress bandwidth
 * Input:
 *      unit        - unit id
 * Output:
 *      pBurst_size - pointer to burst size
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 9300
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
rtk_rate_igrPortBwCtrlBurstSize_get(
    uint32                  unit,
    uint32                  *pBurst_size)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_igrPortBwCtrlBurstSize_get(unit, pBurst_size);
} /* end of rtk_rate_igrPortBwCtrlBurstSize_get */

/* Function Name:
 *      rtk_rate_igrPortBwCtrlBurstSize_set
 * Description:
 *      Set burst size of port ingress bandwidth
 * Input:
 *      unit        - unit id
 *      burst_size - burst size
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 * Applicable:
 *      8380, 9300
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
rtk_rate_igrPortBwCtrlBurstSize_set(
    uint32                  unit,
    uint32                  burst_size)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_igrPortBwCtrlBurstSize_set(unit, burst_size);
} /* end of rtk_rate_igrPortBwCtrlBurstSize_set */

/* Function Name:
 *      rtk_rate_portIgrBwCtrlBurstSize_get
 * Description:
 *      Get the ingress bandwidth control burst on specified port.
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
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      In 8390 and 9310, pCfg.burst_low is same as pCfg.burst_high.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_rate_portIgrBandwidthHighThresh_get)
 */
int32
rtk_rate_portIgrBwCtrlBurstSize_get(uint32 unit, rtk_port_t port, rtk_rate_igrBwBurst_cfg_t *pCfg)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_portIgrBwCtrlBurstSize_get(unit, port, pCfg);

} /* end of rtk_rate_portIgrBwCtrlBurstSize_get */

/* Function Name:
 *      rtk_rate_portIgrBwCtrlBurstSize_set
 * Description:
 *      Set the ingress bandwidth control burst on specified port.
 * Input:
 *      unit - unit id
 *      port - port id
 *      cfg  - ingress bandwidth control burst
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - Invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - Invalid port id
 *      RT_ERR_RATE     - Invalid input rate
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      In 8390 and 9310, pCfg.burst_low will be set as pCfg.burst_high.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_rate_portIgrBandwidthHighThresh_set)
 */
int32
rtk_rate_portIgrBwCtrlBurstSize_set(uint32 unit, rtk_port_t port, rtk_rate_igrBwBurst_cfg_t  *pCfg)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_portIgrBwCtrlBurstSize_set(unit, port, pCfg);

} /* end of rtk_rate_portIgrBwCtrlBurstSize_set */

/* Function Name:
 *      rtk_rate_portIgrBandwidthCtrlExceed_get
 * Description:
 *      Get exceed status of ingress bandwidth control on specified port.
 * Input:
 *      unit      - unit id
 *      port      - port id
 * Output:
 *      pIsExceed - pointer to exceed status
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
 *      Exceed status is as following
 *      - TRUE      - flow control high on threshold ever exceeds.
 *      - FALSE     - flow control high on threshold never exceeds.
 * Changes:
 *      None
 */
int32
rtk_rate_portIgrBandwidthCtrlExceed_get(
    uint32                  unit,
    rtk_port_t              port,
    uint32                  *pIsExceed)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_portIgrBandwidthCtrlExceed_get(unit, port, pIsExceed);

} /* end of rtk_rate_portIgrBandwidthCtrlExceed_get */

/* Function Name:
 *      rtk_rate_portIgrBandwidthCtrlExceed_reset
 * Description:
 *      Reset exceed status of ingress bandwidth control on specified port.
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
rtk_rate_portIgrBandwidthCtrlExceed_reset(
    uint32                  unit,
    rtk_port_t              port)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_portIgrBandwidthCtrlExceed_reset(unit, port);

} /* end of rtk_rate_portIgrBandwidthCtrlExceed_reset */

/* Function Name:
 *      rtk_rate_igrBandwidthCtrlBypass_get
 * Description:
 *      Get the status of bypass ingress bandwidth control for specified packet type.
 * Input:
 *      unit       - unit id
 *      bypassType - bypass type
 * Output:
 *      pEnable    - pointer to enable status of bypass ingress bandwidth control
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - Invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      If a certain packet type is selected to bypass the ingress bandwidth control, the packet will
 *      not be blocked by the ingress bandwidth control and will not consume the bandwidth.
 * Changes:
 *      None
 */
int32
rtk_rate_igrBandwidthCtrlBypass_get(uint32 unit, rtk_rate_igr_bypassType_t bypassType, rtk_enable_t  *pEnable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_igrBandwidthCtrlBypass_get(unit, bypassType, pEnable);
} /* end of rtk_rate_igrBandwidthCtrlBypass_get */

/* Function Name:
 *      rtk_rate_igrBandwidthCtrlBypass_set
 * Description:
 *      Set the status of bypass ingress bandwidth control for specified packet type.
 * Input:
 *      unit       - unit id
 *      bypassType - bypass type
 *      enable     - status of bypass ingress bandwidth control
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - Invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - Invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      If a certain packet type is selected to bypass the ingress bandwidth control, the packet will
 *      not be blocked by the ingress bandwidth control and will not consume the bandwidth.
 * Changes:
 *      None
 */
int32
rtk_rate_igrBandwidthCtrlBypass_set(uint32 unit, rtk_rate_igr_bypassType_t bypassType, rtk_enable_t enable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_igrBandwidthCtrlBypass_set(unit, bypassType, enable);
} /* end of rtk_rate_igrBandwidthCtrlBypass_set */

/* Function Name:
 *      rtk_rate_portIgrBwFlowctrlEnable_get
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
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_rate_igrBandwidthFlowctrlEnable_get)
 */
int32
rtk_rate_portIgrBwFlowctrlEnable_get(
    uint32              unit,
    rtk_port_t          port,
    rtk_enable_t        *pEnable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_portIgrBwFlowctrlEnable_get(unit, port, pEnable);

} /* end of rtk_rate_portIgrBwFlowctrlEnable_get */

/* Function Name:
 *      rtk_rate_portIgrBwFlowctrlEnable_set
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
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_rate_igrBandwidthFlowctrlEnable_set)
 */
int32
rtk_rate_portIgrBwFlowctrlEnable_set(
    uint32              unit,
    rtk_port_t          port,
    rtk_enable_t        enable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_portIgrBwFlowctrlEnable_set(unit, port, enable);

} /* end of rtk_rate_portIgrBwFlowctrlEnable_set */


/* Function Name:
 *      rtk_rate_portIgrQueueBwCtrlEnable_get
 * Description:
 *      Get enable status of ingress bandwidth control on specified queue.
 * Input:
 *      unit  - unit id
 *      port  - port id
 *      queue - queue id
 * Output:
 *      pEnable - Pointer to enable status of ingress queue bandwidth control
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_QUEUE_ID     - invalid queue id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_rate_igrQueueBwCtrlEnable_get)
 */
int32
rtk_rate_portIgrQueueBwCtrlEnable_get(
    uint32          unit,
    rtk_port_t      port,
    rtk_qid_t       queue,
    rtk_enable_t    *pEnable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_portIgrQueueBwCtrlEnable_get(unit, port, queue, pEnable);

} /* end of rtk_rate_portIgrQueueBwCtrlEnable_get */

/* Function Name:
 *      rtk_rate_portIgrQueueBwCtrlEnable_set
 * Description:
 *      Set enable status of ingress bandwidth control on specified queue.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      queue  - queue id
 *      enable - enable status of ingress queue bandwidth control
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_QUEUE_ID - invalid queue id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_rate_igrQueueBwCtrlEnable_get)
 */
int32
rtk_rate_portIgrQueueBwCtrlEnable_set(
    uint32          unit,
    rtk_port_t      port,
    rtk_qid_t       queue,
    rtk_enable_t    enable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_portIgrQueueBwCtrlEnable_set(unit, port, queue, enable);

} /* end of rtk_rate_portIgrQueueBwCtrlEnable_set */

/* Function Name:
 *      rtk_rate_portIgrQueueBwCtrlRate_get
 * Description:
 *      Get rate of ingress bandwidth control on specified queue.
 * Input:
 *      unit  - unit id
 *      port  - port id
 *      queue - queue id
 * Output:
 *      pRate - pointer to rate of ingress queue bandwidth control
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_QUEUE_ID     - invalid queue id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 9310
 * Note:
 *      The actual rate is "rate * chip granularity".
 *      The unit of granularity is 16Kbps.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_rate_igrQueueBwCtrlRate_get)
 */
int32
rtk_rate_portIgrQueueBwCtrlRate_get(
    uint32      unit,
    rtk_port_t  port,
    rtk_qid_t   queue,
    uint32      *pRate)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_portIgrQueueBwCtrlRate_get(unit, port, queue, pRate);

} /* end of rtk_rate_portIgrQueueBwCtrlRate_get */

/* Function Name:
 *      rtk_rate_portIgrQueueBwCtrlRate_set
 * Description:
 *      Set rate of ingress bandwidth control on specified queue.
 * Input:
 *      unit  - unit id
 *      port  - port id
 *      queue - queue id
 *      rate  - rate of ingress queue bandwidth control
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_QUEUE_ID - invalid queue id
 *      RT_ERR_RATE     - Invalid input rate
 * Applicable:
 *      8380, 9310
 * Note:
 *      The actual rate is "rate * chip granularity".
 *      The unit of granularity is 16Kbps.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_rate_igrQueueBwCtrlRate_set)
 */
int32
rtk_rate_portIgrQueueBwCtrlRate_set(
    uint32      unit,
    rtk_port_t  port,
    rtk_qid_t   queue,
    uint32      rate)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_portIgrQueueBwCtrlRate_set(unit, port, queue, rate);

} /* end of rtk_rate_portIgrQueueBwCtrlRate_set */

/* Function Name:
 *      rtk_rate_igrQueueBwCtrlBurstSize_get
 * Description:
 *      Get burst size of queue ingress bandwidth
 * Input:
 *      unit        - unit id
 * Output:
 *      pBurst_size - pointer to burst size
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
rtk_rate_igrQueueBwCtrlBurstSize_get(
    uint32                  unit,
    uint32                  *pBurst_size)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_igrQueueBwCtrlBurstSize_get(unit, pBurst_size);

} /* end of rtk_rate_igrQueueBwCtrlBurstSize_get */

/* Function Name:
 *      rtk_rate_igrQueueBwCtrlBurstSize_set
 * Description:
 *      Set burst size of queue ingress bandwidth
 * Input:
 *      unit        - unit id
 *      burst_size - burst size
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 * Applicable:
 *      8380, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
rtk_rate_igrQueueBwCtrlBurstSize_set(
    uint32                  unit,
    uint32                  burst_size)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_igrQueueBwCtrlBurstSize_set(unit, burst_size);

} /* end of rtk_rate_igrQueueBwCtrlBurstSize_set */

/* Function Name:
 *      rtk_rate_portIgrQueueBwCtrlBurstSize_get
 * Description:
 *      Get the ingress bandwidth control rate and burst.
 * Input:
 *      unit  - unit id
 *      port  - port id
 * Output:
 *      pCfg - ingress queue bandwidth control rate and burst
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - Invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_QUEUE_ID - invalid queue id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_rate_portIgrQueueBwCtrlBurstSize_get(uint32 unit, rtk_port_t port, rtk_qid_t queue, uint32 *pBurst_size)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_portIgrQueueBwCtrlBurstSize_get(unit, port, queue, pBurst_size);

} /* end of rtk_rate_portIgrQueueBwCtrlBurstSize_get */

/* Function Name:
 *      rtk_rate_portIgrQueueBwCtrlBurstSize_set
 * Description:
 *      Set the ingress bandwidth control rate and burst.
 * Input:
 *      unit - unit id
 *      port - port id
 *      pCfg - ingress queue bandwidth control rate and burst
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - Invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - Invalid port id
 *      RT_ERR_QUEUE_ID - invalid queue id
 *      RT_ERR_RATE     - Invalid input rate
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_rate_portIgrQueueBwCtrlBurstSize_set(uint32 unit, rtk_port_t port, rtk_qid_t queue, uint32 burst_size)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_portIgrQueueBwCtrlBurstSize_set(unit, port, queue, burst_size);

} /* end of rtk_rate_portIgrQueueBwCtrlBurstSize_set */

/* Function Name:
 *      rtk_rate_portIgrQueueBwCtrlExceed_get
 * Description:
 *      Get exceed status of ingress bandwidth control on specified queue.
 * Input:
 *      unit       - unit id
 *      port       - port id
 *      queue    - queue id
 * Output:
 *      pIsExceed  - pointer to exceed status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_PORT_ID           - invalid port id
 *      RT_ERR_QUEUE_ID         - invalid queue id
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 * Applicable:
 *      8380, 9310
 * Note:
 *      Exceed status is as following
 *      - TRUE      - ingress queue bandwidth rate is more than configured rate.
 *      - FALSE     -ingress queue bandwidth rate is never over then configured rate.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_rate_igrQueueBwCtrlExceed_get)
 */
int32
rtk_rate_portIgrQueueBwCtrlExceed_get(
    uint32                  unit,
    rtk_port_t              port,
    rtk_qid_t               queue,
    uint32                  *pIsExceed)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_portIgrQueueBwCtrlExceed_get(unit, port, queue, pIsExceed);

} /* end of rtk_rate_portIgrQueueBwCtrlExceed_get */

/* Function Name:
 *      rtk_rate_portIgrQueueBwCtrlExceed_reset
 * Description:
 *      Reset exceed status of ingress bandwidth on specified queue.
 * Input:
 *      unit       - unit id
 *      port       - port id
 *      queue   - queue id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_QUEUE_ID     - invalid queue id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_rate_igrQueueBwCtrlExceed_reset)
 */
int32
rtk_rate_portIgrQueueBwCtrlExceed_reset(
    uint32                  unit,
    rtk_port_t              port,
    rtk_qid_t               queue)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_portIgrQueueBwCtrlExceed_reset(unit, port, queue);

} /* end of rtk_rate_portIgrQueueBwCtrlExceed_reset */

/* Module Name    : Rate                                           */
/* Sub-module Name: Configuration of egress port bandwidth control */

/* Function Name:
 *      rtk_rate_portEgrBwCtrlEnable_get
 * Description:
 *      Get the egress bandwidth control status.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - status of egress bandwidth control
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - Invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      The status of egress bandwidth control is as following:
 *      - DISABLED
 *      - ENABLED
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_rate_egrBandwidthCtrlEnable_get)
 */
int32
rtk_rate_portEgrBwCtrlEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_portEgrBwCtrlEnable_get(unit, port, pEnable);

} /* end of rtk_rate_portEgrBwCtrlEnable_get */

/* Function Name:
 *      rtk_rate_portEgrBwCtrlEnable_set
 * Description:
 *      Set the egress bandwidth control status.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - status of egress bandwidth control
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - Invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - Invalid port id
 *      RT_ERR_INPUT    - Invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      The status of egress bandwidth control is as following:
 *      - DISABLED
 *      - ENABLED
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_rate_egrBandwidthCtrlEnable_set)
 */
int32
rtk_rate_portEgrBwCtrlEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_portEgrBwCtrlEnable_set(unit, port, enable);

} /* end of rtk_rate_portEgrBwCtrlEnable_set */

/* Function Name:
 *      rtk_rate_portEgrBwCtrlRate_get
 * Description:
 *      Get the egress bandwidth control rate.
 * Input:
 *      unit  - unit id
 *      port  - port id
 * Output:
 *      pRate - egress bandwidth control rate
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - Invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The actual rate is "rate * chip granularity".
 *      (2) The unit of granularity is 16Kbps in 8380, 8390, 9300 and 9310.
 *      (3) The unit of granularity is 16Kbps or 1pps in 8380/8390/9300/9310 CPU port depend on mode set by 'rtk_rate_cpuEgrBandwidthCtrlRateMode_set'.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_rate_egrBandwidthCtrlRate_get)
 */
int32
rtk_rate_portEgrBwCtrlRate_get(uint32 unit, rtk_port_t port, uint32 *pRate)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_portEgrBwCtrlRate_get(unit, port, pRate);

} /* end of rtk_rate_portEgrBwCtrlRate_get */

/* Function Name:
 *      rtk_rate_portEgrBwCtrlRate_set
 * Description:
 *      Set the egress bandwidth control rate.
 * Input:
 *      unit - unit id
 *      port - port id
 *      rate - egress bandwidth control rate
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - Invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - Invalid port id
 *      RT_ERR_RATE     - Invalid input rate
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The actual rate is "rate * chip granularity".
 *      (2) The unit of granularity is 16Kbps in 8380, 8390, 9300 and 9310.
 *      (3) The unit of granularity is 16Kbps or 1pps in 8380/8390/9300/9310 CPU port depend on mode set by 'rtk_rate_cpuEgrBandwidthCtrlRateMode_set'.
 *      (4) In 9310, valid range of rate is 0x1~0xFFFFF.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_rate_egrBandwidthCtrlRate_set)
 */
int32
rtk_rate_portEgrBwCtrlRate_set(uint32 unit, rtk_port_t port, uint32 rate)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_portEgrBwCtrlRate_set(unit, port, rate);

} /* end of rtk_rate_portEgrBwCtrlRate_set */

/* Function Name:
 *      rtk_rate_portEgrBwCtrlBurstSize_get
 * Description:
 *      Get the egress bandwidth control burst.
 * Input:
 *      unit  - unit id
 *      port  - port id
 * Output:
 *      pBurst_size - egress bandwidth control burst
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - Invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_rate_portEgrBwCtrlBurstSize_get(uint32 unit, rtk_port_t port, uint32 *pBurst_size)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_portEgrBwCtrlBurstSize_get(unit, port, pBurst_size);

} /* end of rtk_rate_portEgrBwCtrlBurstSize_get */

/* Function Name:
 *      rtk_rate_portEgrBwCtrlBurstSize_set
 * Description:
 *      Set the egress bandwidth control burst.
 * Input:
 *      unit - unit id
 *      port - port id
 *      burst_size - egress bandwidth control burst
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - Invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - Invalid port id
 *      RT_ERR_RATE     - Invalid input rate
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_rate_portEgrBwCtrlBurstSize_set(uint32 unit, rtk_port_t port, uint32 burst_size)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_portEgrBwCtrlBurstSize_set(unit, port, burst_size);

} /* end of rtk_rate_portEgrBwCtrlBurstSize_set */

/* Function Name:
 *      rtk_rate_cpuEgrBandwidthCtrlRateMode_get
 * Description:
 *      Get rate counting mode of CPU port egress bandwidth control.
 * Input:
 *      unit       - unit id
 * Output:
 *      pRate_mode - pointer to rate counting mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID           - invalid unit id
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      The rate mode are as following:
 *      - RATE_MODE_BYTE
 *      - RATE_MODE_PKT
 * Changes:
 *      None
 */
int32
rtk_rate_cpuEgrBandwidthCtrlRateMode_get(uint32 unit, rtk_rate_rateMode_t *pRate_mode)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_cpuEgrBandwidthCtrlRateMode_get(unit, pRate_mode);
} /* end of rtk_rate_cpuEgrBandwidthCtrlRateMode_get */

/* Function Name:
 *      rtk_rate_cpuEgrBandwidthCtrlRateMode_set
 * Description:
 *      Set rate counting mode of CPU port egress bandwidth control.
 * Input:
 *      unit      - unit id
 *      rate_mode - Rate counting mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      The rate mode are as following:
 *      - RATE_MODE_BYTE
 *      - RATE_MODE_PKT
 * Changes:
 *      None
 */
int32
rtk_rate_cpuEgrBandwidthCtrlRateMode_set(uint32 unit, rtk_rate_rateMode_t rate_mode)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_cpuEgrBandwidthCtrlRateMode_set(unit, rate_mode);
} /* end of rtk_rate_cpuEgrBandwidthCtrlRateMode_set */

/* Function Name:
 *      rtk_rate_egrPortBwCtrlBurstSize_get
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
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
rtk_rate_egrPortBwCtrlBurstSize_get(
    uint32                  unit,
    uint32                  *pBurst_size)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_egrPortBwCtrlBurstSize_get(unit, pBurst_size);
} /* end of rtk_rate_egrPortBwCtrlBurstSize_get */

/* Function Name:
 *      rtk_rate_egrPortBwCtrlBurstSize_set
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
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
rtk_rate_egrPortBwCtrlBurstSize_set(
    uint32                  unit,
    uint32                  burst_size)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_egrPortBwCtrlBurstSize_set(unit, burst_size);
} /* end of rtk_rate_egrPortBwCtrlBurstSize_set */

/* Function Name:
 *      rtk_rate_portEgrQueueBwCtrlEnable_get
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
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_QUEUE_ID     - invalid queue id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      In 9300 and 9310 stacking uplink port, the number of queue is 12 (queue 0~11). Before configuring the
 *      queue 8~11, please enable stacking ability of the uplink port by API rtk_stack_port_set.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_rate_egrQueueBwCtrlEnable_get)
 */
int32
rtk_rate_portEgrQueueBwCtrlEnable_get(
    uint32          unit,
    rtk_port_t      port,
    rtk_qid_t       queue,
    rtk_enable_t    *pEnable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_portEgrQueueBwCtrlEnable_get(unit, port, queue, pEnable);

} /* end of rtk_rate_portEgrQueueBwCtrlEnable_get */

/* Function Name:
 *      rtk_rate_portEgrQueueBwCtrlEnable_set
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
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_QUEUE_ID - invalid queue id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      In 9300 and 9310 stacking uplink port, the number of queue is 12 (queue 0~11). Before configuring the
 *      queue 8~11, please enable stacking ability of the uplink port by API rtk_stack_port_set.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_rate_egrQueueBwCtrlEnable_set)
 */
int32
rtk_rate_portEgrQueueBwCtrlEnable_set(
    uint32          unit,
    rtk_port_t      port,
    rtk_qid_t       queue,
    rtk_enable_t    enable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_portEgrQueueBwCtrlEnable_set(unit, port, queue, enable);

} /* end of rtk_rate_portEgrQueueBwCtrlEnable_set */

/* Function Name:
 *      rtk_rate_portEgrQueueBwCtrlRate_get
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
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_QUEUE_ID     - invalid queue id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The actual rate is "rate * chip granularity".
 *      (2) The unit of granularity is 16Kbps.
 *      (3) In 9300 and 9310 stacking uplink port, the number of queue is 12 (queue 0~11). Before configuring the
 *          queue 8~11, please enable stacking ability of the uplink port by API rtk_stack_port_set.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_rate_egrQueueBwCtrlRate_get)
 */
int32
rtk_rate_portEgrQueueBwCtrlRate_get(
    uint32      unit,
    rtk_port_t  port,
    rtk_qid_t   queue,
    uint32      *pRate)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_portEgrQueueBwCtrlRate_get(unit, port, queue, pRate);

} /* end of rtk_rate_portEgrQueueBwCtrlRate_get */

/* Function Name:
 *      rtk_rate_portEgrQueueBwCtrlRate_set
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
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_QUEUE_ID - invalid queue id
 *      RT_ERR_RATE     - Invalid input rate
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The actual rate is "rate * chip granularity".
 *      (2) The unit of granularity is 16Kbps.
 *      (3) In 9300 and 9310 stacking uplink port, the number of queue is 12 (queue 0~11). Before configuring the
 *          queue 8~11, please enable stacking ability of the uplink port by API rtk_stack_port_set.
 *      (4) In 9310, valid range of rate is 0x1~0xFFFFF.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_rate_egrQueueBwCtrlRate_set)
 */
int32
rtk_rate_portEgrQueueBwCtrlRate_set(
    uint32      unit,
    rtk_port_t  port,
    rtk_qid_t   queue,
    uint32      rate)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_portEgrQueueBwCtrlRate_set(unit, port, queue, rate);

} /* end of rtk_rate_portEgrQueueBwCtrlRate_set */

/* Function Name:
 *      rtk_rate_portEgrQueueBwCtrlBurstSize_get
 * Description:
 *      Get the egress queue max bandwidth control burst.
 * Input:
 *      unit  - unit id
 *      port  - port id
 *      queue - queue id
 * Output:
 *      pBurst_size - egress queue max bandwidth control burst
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - Invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_QUEUE_ID - invalid queue id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      In stacking uplink port, the number of queue is 12 (queue 0~11). Before configuring the queue 8~11, please
 *      enable stacking ability of the uplink port by API rtk_stack_port_set.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_rate_portEgrQueueBwCtrlBurstSize_get(uint32 unit, rtk_port_t port, rtk_qid_t queue, uint32 *pBurst_size)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_portEgrQueueBwCtrlBurstSize_get(unit, port, queue, pBurst_size);

} /* end of rtk_rate_portEgrQueueBwCtrlBurstSize_get */

/* Function Name:
 *      rtk_rate_portEgrQueueBwCtrlBurstSize_set
 * Description:
 *      Set the egress queue max bandwidth control burst.
 * Input:
 *      unit       - unit id
 *      port       - port id
 *      queue      - queue id
 *      burst_size - egress queue max bandwidth control burst
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - Invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - Invalid port id
 *      RT_ERR_QUEUE_ID - invalid queue id
 *      RT_ERR_RATE     - Invalid input rate
 * Applicable:
 *      9300, 9310
 * Note:
 *      In stacking uplink port, the number of queue is 12 (queue 0~11). Before configuring the queue 8~11, please
 *      enable stacking ability of the uplink port by API rtk_stack_port_set.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_rate_portEgrQueueBwCtrlBurstSize_set(uint32 unit, rtk_port_t port, rtk_qid_t queue, uint32 burst_size)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_portEgrQueueBwCtrlBurstSize_set(unit, port, queue, burst_size);

} /* end of rtk_rate_portEgrQueueBwCtrlBurstSize_set */

/* Function Name:
 *      rtk_rate_portEgrQueueAssuredBwCtrlEnable_get
 * Description:
 *      Get enable status of egress assured bandwidth control on specified queue.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      queue   - queue id
 * Output:
 *      pEnable - Pointer to enable status of egress queue assured bandwidth control
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_QUEUE_ID     - invalid queue id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      In stacking uplink port, the number of queue is 12 (queue 0~11). Before configuring the queue 8~11, please
 *      enable stacking ability of the uplink port by API rtk_stack_port_set.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_rate_portEgrQueueAssuredBwCtrlEnable_get(
    uint32          unit,
    rtk_port_t      port,
    rtk_qid_t       queue,
    rtk_enable_t    *pEnable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_portEgrQueueAssuredBwCtrlEnable_get(unit, port, queue, pEnable);

} /* end of rtk_rate_portEgrQueueAssuredBwCtrlEnable_get */

/* Function Name:
 *      rtk_rate_portEgrQueueAssuredBwCtrlEnable_set
 * Description:
 *      Set enable status of egress assured bandwidth control on specified queue.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      queue  - queue id
 *      enable - enable status of egress queue assured bandwidth control
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
 *      In stacking uplink port, the number of queue is 12 (queue 0~11). Before configuring the queue 8~11, please
 *      enable stacking ability of the uplink port by API rtk_stack_port_set.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_rate_portEgrQueueAssuredBwCtrlEnable_set(
    uint32          unit,
    rtk_port_t      port,
    rtk_qid_t       queue,
    rtk_enable_t    enable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_portEgrQueueAssuredBwCtrlEnable_set(unit, port, queue, enable);

} /* end of rtk_rate_portEgrQueueAssuredBwCtrlEnable_set */

/* Function Name:
 *      rtk_rate_portEgrQueueAssuredBwCtrlRate_get
 * Description:
 *      Get the egress queue assured bandwidth control rate.
 * Input:
 *      unit  - unit id
 *      port  - port id
 *      queue - queue id
 * Output:
 *      pRate - egress queue assured bandwidth control rate
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - Invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_QUEUE_ID - invalid queue id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) The actual rate is "rate * chip granularity".
 *      (2) The unit of granularity is 16Kbps in 9300 and 9310.
 *      (3) In stacking uplink port, the number of queue is 12 (queue 0~11). Before configuring the queue 8~11, please
 *          enable stacking ability of the uplink port by API rtk_stack_port_set.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_rate_portEgrQueueAssuredBwCtrlRate_get(uint32 unit, rtk_port_t port, rtk_qid_t queue, uint32 *pRate)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_portEgrQueueAssuredBwCtrlRate_get(unit, port, queue, pRate);

} /* end of rtk_rate_portEgrQueueAssuredBwCtrlRate_get */

/* Function Name:
 *      rtk_rate_portEgrQueueAssuredBwCtrlRate_set
 * Description:
 *      Set the egress queue assured bandwidth control rate.
 * Input:
 *      unit - unit id
 *      port - port id
 *      queue - queue id
 *      rate - egress queue assured bandwidth control rate
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - Invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - Invalid port id
 *      RT_ERR_QUEUE_ID - invalid queue id
 *      RT_ERR_RATE     - Invalid input rate
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) The actual rate is "rate * chip granularity".
 *      (2) The unit of granularity is 16Kbps in 9300 and 9310.
 *      (3) In stacking uplink port, the number of queue is 12 (queue 0~11). Before configuring the queue 8~11, please
 *          enable stacking ability of the uplink port by API rtk_stack_port_set.
 *      (4) In 9310, valid range of rate is 0x1~0xFFFFF.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_rate_portEgrQueueAssuredBwCtrlRate_set(uint32 unit, rtk_port_t port, rtk_qid_t queue, uint32 rate)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_portEgrQueueAssuredBwCtrlRate_set(unit, port, queue, rate);

} /* end of rtk_rate_portEgrQueueAssuredBwCtrlRate_set */

/* Function Name:
 *      rtk_rate_portEgrQueueAssuredBwCtrlBurstSize_get
 * Description:
 *      Get the egress queue assured bandwidth control burst.
 * Input:
 *      unit  - unit id
 *      port  - port id
 *      queue - queue id
 * Output:
 *      pBurst_size - egress queue assured bandwidth control burst
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - Invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_QUEUE_ID - invalid queue id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      In stacking uplink port, the number of queue is 12 (queue 0~11). Before configuring the queue 8~11, please
 *      enable stacking ability of the uplink port by API rtk_stack_port_set.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_rate_portEgrQueueAssuredBwCtrlBurstSize_get(uint32 unit, rtk_port_t port, rtk_qid_t queue, uint32 *pBurst_size)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_portEgrQueueAssuredBwCtrlBurstSize_get(unit, port, queue, pBurst_size);

} /* end of rtk_rate_portEgrQueueAssuredBwCtrlBurstSize_get */

/* Function Name:
 *      rtk_rate_portEgrQueueAssuredBwCtrlBurstSize_set
 * Description:
 *      Set the egress queue assured bandwidth control burst.
 * Input:
 *      unit - unit id
 *      port - port id
 *      queue - queue id
 *      burst_size - egress queue assured bandwidth control burst
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - Invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - Invalid port id
 *      RT_ERR_QUEUE_ID - invalid queue id
 *      RT_ERR_RATE     - Invalid input rate
 * Applicable:
 *      9300, 9310
 * Note:
 *      In stacking uplink port, the number of queue is 12 (queue 0~11). Before configuring the queue 8~11, please
 *      enable stacking ability of the uplink port by API rtk_stack_port_set.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_rate_portEgrQueueAssuredBwCtrlBurstSize_set(uint32 unit, rtk_port_t port, rtk_qid_t queue, uint32 burst_size)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_portEgrQueueAssuredBwCtrlBurstSize_set(unit, port, queue, burst_size);

} /* end of rtk_rate_portEgrQueueAssuredBwCtrlBurstSize_set */

/* Function Name:
 *      rtk_rate_portEgrQueueAssuredBwCtrlMode_get
 * Description:
 *      Get mode configuration of egress assured bandwidth control on specified queue.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      queue - queue id
 * Output:
 *      pMode - Pointer to mode of egress queue assured bandwidth control
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - invalid unit id
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_PORT_ID         - invalid port id
 *      RT_ERR_QUEUE_ID       - invalid queue id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      In stacking uplink port, the number of queue is 12 (queue 0~11). Before configuring the queue 8~11, please
 *      enable stacking ability of the uplink port by API rtk_stack_port_set.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_rate_portEgrQueueAssuredBwCtrlMode_get(
    uint32          unit,
    rtk_port_t      port,
    rtk_qid_t       queue,
    rtk_rate_assuredMode_t    *pMode)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_portEgrQueueAssuredBwCtrlMode_get(unit, port, queue, pMode);
} /* end of rtk_rate_portEgrQueueAssuredBwCtrlMode_get */

/* Function Name:
 *      rtk_rate_portEgrQueueAssuredBwCtrlMode_set
 * Description:
 *      Set mode of egress assured bandwidth control on specified queue.
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      queue  - queue id
 *      mode   - mode of egress queue assured bandwidth control
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
 *      (1) The mode are as following:
 *          - ASSURED_MODE_SHARE
 *          - ASSURED_MODE_FIX
 *      (2) In stacking uplink port, the number of queue is 12 (queue 0~11). Before configuring the queue 8~11, please
 *          enable stacking ability of the uplink port by API dal_mango_stack_port_set.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_rate_portEgrQueueAssuredBwCtrlMode_set(
    uint32          unit,
    rtk_port_t      port,
    rtk_qid_t       queue,
    rtk_rate_assuredMode_t    mode)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_portEgrQueueAssuredBwCtrlMode_set(unit, port, queue, mode);
} /* end of rtk_rate_portEgrQueueAssuredBwCtrlMode_set */

/* Function Name:
 *      rtk_rate_egrQueueFixedBandwidthEnable_get
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
 *      RT_ERR_UNIT_ID      - Invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_QUEUE_ID     - invalid queue id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
rtk_rate_egrQueueFixedBandwidthEnable_get(
    uint32          unit,
    rtk_port_t      port,
    rtk_qid_t       queue,
    rtk_enable_t    *pEnable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_egrQueueFixedBandwidthEnable_get(unit, port, queue, pEnable);

} /* end of rtk_rate_egrQueueFixedBandwidthEnable_get */

/* Function Name:
 *      rtk_rate_egrQueueFixedBandwidthEnable_set
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
 *      RT_ERR_UNIT_ID  - Invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - Invalid port id
 *      RT_ERR_QUEUE_ID - invalid queue id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
rtk_rate_egrQueueFixedBandwidthEnable_set(
    uint32          unit,
    rtk_port_t      port,
    rtk_qid_t       queue,
    rtk_enable_t    enable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_egrQueueFixedBandwidthEnable_set(unit, port, queue, enable);

} /* end of rtk_rate_egrQueueFixedBandwidthEnable_set */

/* Function Name:
 *      rtk_rate_egrQueueBwCtrlBurstSize_get
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
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
rtk_rate_egrQueueBwCtrlBurstSize_get(
    uint32                  unit,
    uint32                  *pBurst_size)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_egrQueueBwCtrlBurstSize_get(unit, pBurst_size);
} /* end of rtk_rate_egrQueueBwCtrlBurstSize_get */

/* Function Name:
 *      rtk_rate_egrQueueBwCtrlBurstSize_set
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
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
rtk_rate_egrQueueBwCtrlBurstSize_set(
    uint32                  unit,
    uint32                  burst_size)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_egrQueueBwCtrlBurstSize_set(unit, burst_size);
} /* end of rtk_rate_egrQueueBwCtrlBurstSize_set */

/* Module Name    : Rate                           */
/* Sub-module Name: Configuration of storm control */

/* Function Name:
 *      rtk_rate_portStormCtrlEnable_get
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
 *      RT_ERR_UNIT_ID           - invalid unit id
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_PORT_ID           - invalid port id
 *      RT_ERR_SFC_UNKNOWN_GROUP - Unknown storm group
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The storm group types are as following:
 *          - STORM_GROUP_MULTICAST
 *          - STORM_GROUP_BROADCAST
 *          - STORM_GROUP_UNICAST
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_rate_stormControlEnable_get)
 *          In 8380 and 8390, storm group typesSTORM_GROUP_UNKNOWN_UNICAST and STORM_GROUP_UNKNOWN_MULTICAST
 *          are obsoleted by STORM_GROUP_UNICAST and STORM_GROUP_MULTICAST, respectively.
 */
int32
rtk_rate_portStormCtrlEnable_get(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_group_t  storm_type,
    rtk_enable_t            *pEnable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_portStormCtrlEnable_get(unit, port, storm_type, pEnable);

} /* end of rtk_rate_portStormCtrlEnable_get */

/* Function Name:
 *      rtk_rate_portStormCtrlEnable_set
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
 *      RT_ERR_UNIT_ID           - invalid unit id
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_PORT_ID           - invalid port id
 *      RT_ERR_SFC_UNKNOWN_GROUP - Unknown storm group
 *      RT_ERR_INPUT             - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The storm group types are as following:
 *          - STORM_GROUP_MULTICAST
 *          - STORM_GROUP_BROADCAST
 *          - STORM_GROUP_UNICAST
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_rate_stormControlEnable_set)
 *          In 8380 and 8390, storm group typesSTORM_GROUP_UNKNOWN_UNICAST and STORM_GROUP_UNKNOWN_MULTICAST
 *          are obsoleted by STORM_GROUP_UNICAST and STORM_GROUP_MULTICAST, respectively.
 */
int32
rtk_rate_portStormCtrlEnable_set(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_group_t  storm_type,
    rtk_enable_t            enable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_portStormCtrlEnable_set(unit, port, storm_type, enable);

} /* end of rtk_rate_portStormCtrlEnable_set */

/* Function Name:
 *      rtk_rate_portStormCtrlRate_get
 * Description:
 *      Get the storm control rate.
 * Input:
 *      unit       - unit id
 *      port       - port id
 *      storm_type - storm group type
 * Output:
 *      pRate      - storm control rate
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - Invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_INPUT        - Invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) Supported management frame is as following:
 *          rtk_rate_storm_group_t \ Chip :   8380  8390  9300  9310
 *          - STORM_GROUP_MULTICAST           O     O     O     O
 *          - STORM_GROUP_BROADCAST           O     O     O     O
 *          - STORM_GROUP_UNICAST             O     O     O     O
 *      (2) The actual rate is "rate * chip granularity".
 *      (3) The unit of granularity is 16Kbps or 1pps in 8380, 8390 depend on mode set by 'rtk_rate_stormControlRateMode_set'.
 *      (4) The unit of granularity is 16Kbps or 1pps in 9300, 9310 depend on mode set by 'rtk_rate_portStormCtrlRateMode_set'.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_rate_stormControlRate_get)
 *          In 8380, storm group typesSTORM_GROUP_UNKNOWN_UNICAST and STORM_GROUP_UNKNOWN_MULTICAST
 *          are obsoleted by STORM_GROUP_UNICAST and STORM_GROUP_MULTICAST, respectively.
 */
int32
rtk_rate_portStormCtrlRate_get(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_group_t  storm_type,
    uint32                  *pRate)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_portStormCtrlRate_get(unit, port, storm_type, pRate);

} /* end of rtk_rate_portStormCtrlRate_get */

/* Function Name:
 *      rtk_rate_portStormCtrlRate_set
 * Description:
 *      Set the storm control rate.
 * Input:
 *      unit       - unit id
 *      port       - port id
 *      storm_type - storm group type
 *      rate       - storm control rate
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - Invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - Invalid port id
 *      RT_ERR_INPUT    - Invalid input parameter
 *      RT_ERR_RATE     - Invalid input bandwidth
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) Supported management frame is as following:
 *          rtk_rate_storm_group_t \ Chip :   8380  8390  9300  9310
 *          - STORM_GROUP_MULTICAST           O     O     O     O
 *          - STORM_GROUP_BROADCAST           O     O     O     O
 *          - STORM_GROUP_UNICAST             O     O     O     O
 *      (2) The actual rate is "rate * chip granularity".
 *      (3) The unit of granularity is 16Kbps or 1pps in 8380, 8390 depend on mode set by 'rtk_rate_stormControlRateMode_set'.
 *      (4) The unit of granularity is 16Kbps or 1pps in 9300, 9310 depend on mode set by 'rtk_rate_portStormCtrlRateMode_set'.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_rate_stormControlRate_set)
 *          In 8380, storm group typesSTORM_GROUP_UNKNOWN_UNICAST and STORM_GROUP_UNKNOWN_MULTICAST
 *          are obsoleted by STORM_GROUP_UNICAST and STORM_GROUP_MULTICAST, respectively.
 */
int32
rtk_rate_portStormCtrlRate_set(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_group_t  storm_type,
    uint32                  rate)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_portStormCtrlRate_set(unit, port, storm_type, rate);

} /* end of rtk_rate_portStormCtrlRate_set */

/* Function Name:
 *      rtk_rate_portStormCtrlBurstSize_get
 * Description:
 *      Get burst size of storm control on specified port.
 * Input:
 *      unit        - unit id
 *      port        - port id
 *      storm_type  - storm group type
 * Output:
 *      pBurst_size - pointer to burst size
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID           - invalid unit id
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_PORT_ID           - invalid port id
 *      RT_ERR_SFC_UNKNOWN_GROUP - Unknown storm group
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      (1) The storm group types are as following:
 *          rtk_rate_storm_group_t \ Chip :   8390  9300  9310
 *          - STORM_GROUP_MULTICAST           O     O     O
 *          - STORM_GROUP_BROADCAST           O     O     O
 *          - STORM_GROUP_UNICAST             O     O     O
 *      (2) The unit of burst size is 1 byte or 1 packet in 8390, 9300 and 9310 depend on mode.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_rate_portStormControlBurstSize_get)
 */
int32
rtk_rate_portStormCtrlBurstSize_get(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_group_t  storm_type,
    uint32                  *pBurst_size)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_portStormCtrlBurstSize_get(unit, port, storm_type, pBurst_size);

} /* end of rtk_rate_portStormCtrlBurstSize_get */

/* Function Name:
 *      rtk_rate_portStormCtrlBurstSize_set
 * Description:
 *      Set burst size of storm control on specified port.
 * Input:
 *      unit       - unit id
 *      port       - port id
 *      storm_type - storm group type
 *      burst_size - burst size
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID           - invalid unit id
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_PORT_ID           - invalid port id
 *      RT_ERR_SFC_UNKNOWN_GROUP - Unknown storm group
 *      RT_ERR_INPUT             - invalid input parameter
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      (1) The storm group types are as following:
 *          rtk_rate_storm_group_t \ Chip :   8390  9300  9310
 *          - STORM_GROUP_MULTICAST           O     O     O
 *          - STORM_GROUP_BROADCAST           O     O     O
 *          - STORM_GROUP_UNICAST             O     O     O
 *      (2) The unit of burst size is 1 byte or 1 packet in 8390, 9300 and 9310 depend on mode.
 *      (3) burst_size range of 8390 is as following:
 *          The (minimum, maximum) of burst_rate setting range of normal port is (1700, 65535).
 *          The (minimum, maximum) of burst_rate setting range of 10G port is (2650, 1048575).
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_rate_portStormControlBurstSize_set)
 */
int32
rtk_rate_portStormCtrlBurstSize_set(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_group_t  storm_type,
    uint32                  burst_size)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_portStormCtrlBurstSize_set(unit, port, storm_type, burst_size);

} /* end of rtk_rate_portStormCtrlBurstSize_set */

/* Function Name:
 *      rtk_rate_portStormCtrlExceed_get
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
 *      RT_ERR_UNIT_ID           - invalid unit id
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_PORT_ID           - invalid port id
 *      RT_ERR_SFC_UNKNOWN_GROUP - Unknown storm group
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) Exceed status is as following
 *          - TRUE      storm rate is more than configured rate.
 *          - FALSE     storm rate is never over then configured rate.
 *      (2) The routine get exceed status and clear it after read
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_rate_stormControlExceed_get)
 */
int32
rtk_rate_portStormCtrlExceed_get(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_group_t  storm_type,
    uint32                  *pIsExceed)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_portStormCtrlExceed_get(unit, port, storm_type, pIsExceed);

} /* end of rtk_rate_portStormCtrlExceed_get */

/* Function Name:
 *      rtk_rate_portStormCtrlExceed_reset
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
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - Invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_rate_stormControlExceed_reset)
 */
int32
rtk_rate_portStormCtrlExceed_reset(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_group_t  storm_type)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_portStormCtrlExceed_reset(unit, port, storm_type);

} /* end of rtk_rate_portStormCtrlExceed_reset */

/* Function Name:
 *      rtk_rate_portStormCtrlRateMode_get
 * Description:
 *      Get rate counting mode of storm control.
 * Input:
 *      unit       - unit id
 * Output:
 *      pRate_mode - pointer to rate counting mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID           - invalid unit id
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_PORT_ID           - invalid port id
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) The rate mode are as following:
 *          - BASED_ON_PKT
 *          - BASED_ON_BYTE
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_rate_portStormCtrlRateMode_get(
    uint32                      unit,
    rtk_port_t              port,
    rtk_rate_storm_rateMode_t   *pRate_mode)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_portStormCtrlRateMode_get(unit, port, pRate_mode);

} /* end of rtk_rate_portStormCtrlRateMode_get */

/* Function Name:
 *      rtk_rate_portStormCtrlRateMode_set
 * Description:
 *      Set rate counting mode of storm control.
 * Input:
 *      unit      - unit id
 *      rate_mode - Rate counting mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID           - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) The rate mode are as following:
 *          - BASED_ON_PKT
 *          - BASED_ON_BYTE
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_rate_portStormCtrlRateMode_set(
    uint32                      unit,
    rtk_port_t              port,
    rtk_rate_storm_rateMode_t   rate_mode)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_portStormCtrlRateMode_set(unit, port, rate_mode);

} /* end of rtk_rate_portStormCtrlRateMode_set */

/* Function Name:
 *      rtk_rate_portStormCtrlTypeSel_get
 * Description:
 *      Get the configuration of storm control type selection.
 * Input:
 *      unit       - unit id
 *      port       - port id
 *      storm_type - storm group type
 * Output:
 *      pStorm_sel - storm selection
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - Invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_INPUT        - Invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      The API is only supported in unicast and multicast, the storm group types are as following:
 *      - STORM_GROUP_UNICAST
 *      - STORM_GROUP_MULTICAST
 *
 *      The storm selection are as following:
 *      - STORM_SEL_UNKNOWN
 *      - STORM_SEL_UNKNOWN_AND_KNOWN
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_rate_stormControlTypeSel_get)
 */
int32
rtk_rate_portStormCtrlTypeSel_get(uint32 unit, rtk_port_t port, rtk_rate_storm_group_t storm_type, rtk_rate_storm_sel_t *pStorm_sel)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_portStormCtrlTypeSel_get(unit, port, storm_type, pStorm_sel);

} /* end of rtk_rate_portStormCtrlTypeSel_get */

/* Function Name:
 *      rtk_rate_portStormCtrlTypeSel_set
 * Description:
 *      Set the configuration of storm control type selection.
 * Input:
 *      unit       - unit id
 *      port       - port id
 *      storm_type - storm group type
 *      storm_sel  - storm selection
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - Invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - Invalid port id
 *      RT_ERR_INPUT    - Invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      The API is only supported in unicast and multicast, the storm group types are as following:
 *      - STORM_GROUP_UNICAST
 *      - STORM_GROUP_MULTICAST
 *
 *      The storm selection are as following:
 *      - STORM_SEL_UNKNOWN
 *      - STORM_SEL_UNKNOWN_AND_KNOWN
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_rate_stormControlTypeSel_set)
 */
int32
rtk_rate_portStormCtrlTypeSel_set(uint32 unit, rtk_port_t port, rtk_rate_storm_group_t storm_type, rtk_rate_storm_sel_t storm_sel)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_portStormCtrlTypeSel_set(unit, port, storm_type, storm_sel);

} /* end of rtk_rate_portStormCtrlTypeSel_set */

/* Function Name:
 *      rtk_rate_portStormCtrlProtoEnable_get
 * Description:
 *      Get enable status of protocol storm control on specified port.
 * Input:
 *      unit       - unit id
 *      port       - port id
 *      storm_type - storm group type
 * Output:
 *      pEnable    - pointer to enable status of protocol storm control
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID           - invalid unit id
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_PORT_ID           - invalid port id
 *      RT_ERR_SFC_UNKNOWN_GROUP - Unknown storm group
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_rate_portStormCtrlProtoEnable_get(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_proto_group_t  storm_type,
    rtk_enable_t            *pEnable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_portStormCtrlProtoEnable_get(unit, port, storm_type, pEnable);

} /* end of rtk_rate_portStormCtrlProtoEnable_get */

/* Function Name:
 *      rtk_rate_portStormCtrlProtoEnable_set
 * Description:
 *      Set enable status of protocol storm control on specified port.
 * Input:
 *      unit       - unit id
 *      port       - port id
 *      storm_type - storm group type
 *      enable     - enable status of protocol storm control
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID           - invalid unit id
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_PORT_ID           - invalid port id
 *      RT_ERR_SFC_UNKNOWN_GROUP - Unknown storm group
 *      RT_ERR_INPUT             - invalid input parameter
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_rate_portStormCtrlProtoEnable_set(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_proto_group_t  storm_type,
    rtk_enable_t            enable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_portStormCtrlProtoEnable_set(unit, port, storm_type, enable);

} /* end of rtk_rate_portStormCtrlProtoEnable_set */

/* Function Name:
 *      rtk_rate_portStormCtrlProtoRate_get
 * Description:
 *      Get the protocol storm control rate.
 * Input:
 *      unit       - unit id
 *      port       - port id
 *      storm_type - storm group type
 * Output:
 *      pRate      - storm control rate (packet-per-second).
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - Invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_INPUT        - Invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      The rate unit is 1 pps(packet-per-second).
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_rate_stormControlProtoRate_get)
 */
int32
rtk_rate_portStormCtrlProtoRate_get(
    uint32                          unit,
    rtk_port_t                      port,
    rtk_rate_storm_proto_group_t    storm_type,
    uint32                          *pRate)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_portStormCtrlProtoRate_get(unit, port, storm_type, pRate);

} /* end of rtk_rate_portStormCtrlProtoRate_get */

/* Function Name:
 *      rtk_rate_portStormCtrlProtoRate_set
 * Description:
 *      Set the protocol storm control rate.
 * Input:
 *      unit       - unit id
 *      port       - port id
 *      storm_type - storm group type
 *      rate       - storm control rate (packet-per-second).
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - Invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - Invalid port id
 *      RT_ERR_INPUT    - Invalid input parameter
 *      RT_ERR_RATE     - Invalid input bandwidth
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      The rate unit is 1 pps(packet-per-second).
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_rate_stormControlProtoRate_set)
 */
int32
rtk_rate_portStormCtrlProtoRate_set(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_proto_group_t  storm_type,
    uint32                  rate)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_portStormCtrlProtoRate_set(unit, port, storm_type, rate);

} /* end of rtk_rate_portStormCtrlProtoRate_set */

/* Function Name:
 *      rtk_rate_portStormCtrlProtoBurstSize_get
 * Description:
 *      Get the storm control burst.
 * Input:
 *      unit  - unit id
 *      port  - port id
 *      storm_type - storm group type
 * Output:
 *      pBurst_size - storm control burst
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - Invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      The unit of granularity is 1pps in 9300 and 9310.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_rate_portStormCtrlProtoBurstSize_get(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_proto_group_t  storm_type,
    uint32                 *pBurst_size)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_portStormCtrlProtoBurstSize_get(unit, port, storm_type, pBurst_size);

} /* end of rtk_rate_portStormCtrlProtoBurstSize_get */

/* Function Name:
 *      rtk_rate_portStormCtrlProtoBurstSize_set
 * Description:
 *      Set the storm control burst.
 * Input:
 *      unit - unit id
 *      port - port id
 *      storm_type - storm group type
 *      burst_size - storm control burst
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - Invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - Invalid port id
 *      RT_ERR_RATE     - Invalid input rate
 * Applicable:
 *      9300, 9310
 * Note:
 *      The unit of granularity is 1pps in 9300 and 9310.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_rate_portStormCtrlProtoBurstSize_set(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_proto_group_t  storm_type,
    uint32                 burst_size)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_portStormCtrlProtoBurstSize_set(unit, port, storm_type, burst_size);

} /* end of rtk_rate_portStormCtrlProtoBurstSize_set */

/* Function Name:
 *      rtk_rate_portStormCtrlProtoExceed_get
 * Description:
 *      Get exceed status of protocol storm control on specified port.
 * Input:
 *      unit       - unit id
 *      port       - port id
 *      storm_type - storm group type
 * Output:
 *      pIsExceed  - pointer to exceed status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_INPUT        - Invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      Exceed status is as following
 *      - TRUE      - storm rate is more than configured rate.
 *      - FALSE     - storm rate is never over then configured rate.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_rate_stormControlProtoExceed_get)
 */
int32
rtk_rate_portStormCtrlProtoExceed_get(
    uint32                          unit,
    rtk_port_t                      port,
    rtk_rate_storm_proto_group_t    storm_type,
    uint32                          *pIsExceed)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_portStormCtrlProtoExceed_get(unit, port, storm_type, pIsExceed);

} /* end of rtk_rate_portStormCtrlProtoExceed_get */

/* Function Name:
 *      rtk_rate_portStormCtrlProtoExceed_reset
 * Description:
 *      Reset exceed status of protocol storm control on specified port.
 * Input:
 *      unit       - unit id
 *      port       - port id
 *      storm_type - storm group type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - Invalid input parameter
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 *          This function name is changed compared with sdk-2(rtk_rate_stormControlProtoExceed_reset)
 */
int32
rtk_rate_portStormCtrlProtoExceed_reset(
    uint32                          unit,
    rtk_port_t                      port,
    rtk_rate_storm_proto_group_t    storm_type)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_portStormCtrlProtoExceed_reset(unit, port, storm_type);

} /* end of rtk_rate_portStormCtrlProtoExceed_reset */

/* Function Name:
 *      rtk_rate_stormControlBypass_get
 * Description:
 *      Get the status of bypass storm control for specified packet type.
 * Input:
 *      unit       - unit id
 *      bypassType - bypass type
 * Output:
 *      pEnable    - pointer to enable status of bypass storm control
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - Invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      If a certain packet type is selected to bypass the storm control, the packet will
 *      not be blocked by the storm control and will not consume the bandwidth.
 * Changes:
 *      None
 */
int32
rtk_rate_stormControlBypass_get(uint32 unit, rtk_rate_storm_bypassType_t bypassType, rtk_enable_t  *pEnable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_stormControlBypass_get(unit, bypassType, pEnable);
} /* end of rtk_rate_stormControlBypass_get */

/* Function Name:
 *      rtk_rate_stormControlBypass_set
 * Description:
 *      Set the status of bypass storm control for specified packet type.
 * Input:
 *      unit       - unit id
 *      bypassType - bypass type
 *      enable     - status of bypass storm control
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - Invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - Invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      If a certain packet type is selected to bypass the storm control, the packet will
 *      not be blocked by the storm control and will not consume the bandwidth.
 * Changes:
 *      None
 */
int32
rtk_rate_stormControlBypass_set(uint32 unit, rtk_rate_storm_bypassType_t bypassType, rtk_enable_t enable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_stormControlBypass_set(unit, bypassType, enable);
} /* end of rtk_rate_stormControlBypass_set */

/* Function Name:
 *      rtk_rate_stormControlBurstSize_get
 * Description:
 *      Get burst size of storm control.
 * Input:
 *      unit        - unit id
 *      storm_type  - storm group type
 * Output:
 *      pBurst_size - pointer to burst size
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 9300, 9310
 * Note:
 *      The storm group types are as following:
 *      - STORM_GROUP_MULTICAST
 *      - STORM_GROUP_BROADCAST
 *      - STORM_GROUP_UNICAST
 * Changes:
 *      None
 */
int32
rtk_rate_stormControlBurstSize_get(
    uint32                  unit,
    rtk_rate_storm_group_t  storm_type,
    uint32                  *pBurst_size)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_stormControlBurstSize_get(unit, storm_type, pBurst_size);
} /* end of rtk_rate_stormControlBurstSize_get */

/* Function Name:
 *      rtk_rate_stormControlBurstSize_set
 * Description:
 *      Set burst size of storm control.
 * Input:
 *      unit       - unit id
 *      storm_type - storm group type
 *      burst_size - burst size
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 9300, 9310
 * Note:
 *      The storm group types are as following:
 *      - STORM_GROUP_MULTICAST
 *      - STORM_GROUP_BROADCAST
 *      - STORM_GROUP_UNICAST
 * Changes:
 *      None
 */
int32
rtk_rate_stormControlBurstSize_set(
    uint32                  unit,
    rtk_rate_storm_group_t  storm_type,
    uint32                  burst_size)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_stormControlBurstSize_set(unit, storm_type, burst_size);
} /* end of rtk_rate_stormControlBurstSize_set */

/* Function Name:
 *      rtk_rate_stormControlRateMode_get
 * Description:
 *      Get rate counting mode of storm control.
 * Input:
 *      unit       - unit id
 * Output:
 *      pRate_mode - pointer to rate counting mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID           - invalid unit id
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The rate mode are as following:
 *          - BASED_ON_PKT
 *          - BASED_ON_BYTE
 * Changes:
 *      None
 */
int32
rtk_rate_stormControlRateMode_get(
    uint32                      unit,
    rtk_rate_storm_rateMode_t   *pRate_mode)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_stormControlRateMode_get(unit, pRate_mode);
} /* end of rtk_rate_stormControlRateMode_get */

/* Function Name:
 *      rtk_rate_stormControlRateMode_set
 * Description:
 *      Set rate counting mode of storm control.
 * Input:
 *      unit      - unit id
 *      rate_mode - Rate counting mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The rate mode are as following:
 *          - BASED_ON_PKT
 *          - BASED_ON_BYTE
 * Changes:
 *      None
 */
int32
rtk_rate_stormControlRateMode_set(
    uint32                      unit,
    rtk_rate_storm_rateMode_t   rate_mode)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_stormControlRateMode_set(unit, rate_mode);
} /* end of rtk_rate_stormControlRateMode_set */

/* Function Name:
 *      rtk_rate_stormCtrlProtoVlanConstrtEnable_get
 * Description:
 *      Get enable status of protocol storm control vlan constrain.
 * Input:
 *      unit    - unit id
 *      storm_type    - storm type
 * Output:
 *      pEnable - Pointer to enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      Supported protocol storm group types are as following:
 *      - STORM_PROTO_GROUP_ARP
 *      - STORM_PROTO_GROUP_DHCP
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_rate_stormCtrlProtoVlanConstrtEnable_get(uint32 unit, rtk_rate_storm_proto_group_t storm_type, rtk_enable_t *pEnable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_stormCtrlProtoVlanConstrtEnable_get(unit, storm_type, pEnable);
} /* end of rtk_rate_stormCtrlProtoVlanConstrtEnable_get */

/* Function Name:
 *      rtk_rate_stormCtrlProtoVlanConstrtEnable_set
 * Description:
 *      Set enable status of protocol storm control vlan constrain.
 * Input:
 *      unit    - unit id
 *      storm_type    - storm type
 *      enable -  enable status
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      9300, 9310
 * Note:
 *      Supported protocol storm group types are as following:
 *      - STORM_PROTO_GROUP_ARP
 *      - STORM_PROTO_GROUP_DHCP
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_rate_stormCtrlProtoVlanConstrtEnable_set(uint32 unit, rtk_rate_storm_proto_group_t storm_type, rtk_enable_t enable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->rate_stormCtrlProtoVlanConstrtEnable_set(unit, storm_type, enable);
} /* end of rtk_rate_stormCtrlProtoVlanConstrtEnable_set */

