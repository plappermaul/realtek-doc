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
 * Purpose : Definition of TIME API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) IEEE 1588
 *
 */

#ifndef __DAL_CYPRESS_TIME_H__
#define __DAL_CYPRESS_TIME_H__


/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/time.h>
#include <dal/dal_mapper.h>


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

/* Module Name : TIME */

/* Function Name:
 *      dal_cypress_timeMapper_init
 * Description:
 *      Hook time module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must Hook time module before calling any time APIs.
 */
extern int32
dal_cypress_timeMapper_init(dal_mapper_t *pMapper);

/* Function Name:
 *      dal_cypress_time_init
 * Description:
 *      Initialize time module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize time module before calling any time APIs.
 */
extern int32
dal_cypress_time_init(uint32 unit);

/* Function Name:
 *      dal_cypress_time_portPtpEnable_get
 * Description:
 *      Get TIME status of the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT     - invalid port id
 * Note:
 *      None
 */
extern int32
dal_cypress_time_portPtpEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_cypress_time_portPtpEnable_set
 * Description:
 *      Set TIME status of the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      enable  - status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT     - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_cypress_time_portPtpEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      dal_cypress_time_portRxTimestamp_get
 * Description:
 *      Get TIME timstamp of the TIME identifier of the dedicated port from the specified device.
 * Input:
 *      unit       - unit id
 *      port       - port id
 *      identifier - indentifier of TIME packet
 * Output:
 *      pTimeStamp - pointer buffer of TIME timestamp
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_time_portPtpRxTimestamp_get(
    uint32 unit,
    rtk_port_t port,
    rtk_time_ptpIdentifier_t identifier,
    rtk_time_timeStamp_t *pTimeStamp);

/* Function Name:
 *      dal_cypress_time_portTxTimestamp_get
 * Description:
 *      Get TIME Tx timstamp of the TIME identifier of the dedicated port from the specified device.
 * Input:
 *      unit       - unit id
 *      port       - port id
 *      identifier - indentifier of TIME packet
 * Output:
 *      pTimeStamp - pointer buffer of TIME timestamp
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_time_portPtpTxTimestamp_get(
    uint32 unit,
    rtk_port_t port,
    rtk_time_ptpIdentifier_t identifier,
    rtk_time_timeStamp_t *pTimeStamp);

/* Function Name:
 *      dal_cypress_time_portPtpTxTimestampCallback_register
 * Description:
 *      Register TIME transmission callback function of the TIME identifier of the dedicated port to the specified device.
 * Input:
 *      unit                  - unit id
 *      port                  - port id
 *      identifier            - indentifier of TIME packet
 *      rtk_time_ptpTime_cb_f - callback function
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 * Note:
 *      None
 */
extern int32
dal_cypress_time_portPtpTxTimestampCallback_register(
    uint32 unit,
    rtk_port_t port,
    rtk_time_ptpIdentifier_t identifier,
    rtk_time_ptpTime_cb_f *fCb);

/* Function Name:
 *      dal_cypress_time_portRefTime_get
 * Description:
 *      Get the reference time of TIME of the specified port.
 * Input:
 *      unit  - unit id
 *      port    - port id
 * Output:
 *      pTimeStamp - pointer buffer of TIME reference time
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_time_portRefTime_get(uint32 unit, rtk_port_t port, rtk_time_timeStamp_t *pTimeStamp);

/* Function Name:
 *      dal_cypress_time_portRefTime_set
 * Description:
 *      Set the reference time of TIME of the specified portmask.
 * Input:
 *      unit      - unit id
 *      portmask    - portmask
 *      timeStamp - reference timestamp value
 *      exec        - 0 : do not execute, 1: execute  
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
dal_cypress_time_portRefTime_set(uint32 unit, rtk_portmask_t portmask, rtk_time_timeStamp_t timeStamp, uint32 exec);

/* Function Name:
 *      dal_cypress_ptp_portTimeAdjust_set
 * Description:
 *      Adjust TIME reference time.
 * Input:
 *      unit      - unit id
 *      portmask    - portmask
 *      sign      - significant
 *      timeStamp - reference timestamp value
 *      exec        - 0 : do not execute, 1: execute  
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_cypress_time_portRefTimeAdjust_set(uint32 unit, rtk_portmask_t portmask, uint32 sign, rtk_time_timeStamp_t timeStamp, uint32 exec);

/* Function Name:
 *      dal_cypress_time_refTimeEnable_get
 * Description:
 *      Get the enable state of reference time of the specified device.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_time_portRefTimeEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_cypress_time_portRefTimeEnable_set
 * Description:
 *      Set the enable state of reference time of the specified portmask.
 * Input:
 *      unit   - unit id
 *      portmask - portmask
 *      enable - status
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
dal_cypress_time_portRefTimeEnable_set(uint32 unit, rtk_portmask_t portmask, rtk_enable_t enable);

#endif /* __DAL_CYPRESS_TIME_H__ */

