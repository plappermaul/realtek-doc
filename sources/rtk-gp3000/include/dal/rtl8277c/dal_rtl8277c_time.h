/*
 * Copyright (C) 2023 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: $
 * $Date: $
 *
 * Purpose : Definition of TIME API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) IEEE 1588
 *           (2) PON ToD
 *           (3) 1PPS/ToD
 *
 */

#ifndef __DAL_RTL8277C_TIME_H__
#define __DAL_RTL8277C_TIME_H__

/*
 * Include Files
 */
#include <common/rt_error.h>
#include <common/rt_type.h>
#include <rtk/rt/rt_time.h>



/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */

 /*
  * Macro Declaration
  */

#define RTL8277C_TIME_SEC_MAX 0xFFFFFFFFFFFF
#define RTL8277C_TIME_NSEC_MAX 0x3B9ACA00 //1000000000 ns=1 s
#define RTL8277C_TIME_FREQUENCY_MAX 0xFFFFFFF



/* Function Name:
 *      dal_rtl8277c_time_init
 * Description:
 *      Initialize Time module.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize Time module before calling any Time APIs.
 */
extern int32
dal_rtl8277c_time_init(void);

/* Function Name:
 *      dal_rtl8277c_rt_time_curTime_get
 * Description:
 *      Get the current time.
 * Input:
 *      None
 * Output:
 *      pTimeStamp - pointer buffer of the current time
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
dal_rtl8277c_rt_time_curTime_get(rt_time_timeStamp_t *pTimeStamp);

/* Function Name:
 *      dal_rtl8277c_rt_time_offsetTime_set
 * Description:
 *      Set the reference time.
 * Input:
 *      sign      - significant
 *      timeStamp - reference timestamp value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      sign=0 for positive adjustment, sign=1 for negative adjustment.
 */
extern int32
dal_rtl8277c_rt_time_offsetTime_set(uint32 sign, rt_time_timeStamp_t timeStamp);

/* Function Name:
 *      dal_rtl8277c_time_frequency_set
 * Description:
 *      Set frequency of PTP system time.
 * Input:
 *      freq - reference timestamp value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_rtl8277c_time_frequency_set(uint32 freq);

/* Function Name:
 *      dal_rtl8277c_time_frequency_get
 * Description:
 *      Set frequency of PTP system time.
 * Input:
 *      freq - reference timestamp value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_rtl8277c_time_frequency_get(uint32 *freq);

/* Function Name:
 *      dal_rtl8277c_rt_time_ptpPortEnable_set
 * Description:
 *      Set PTP status of the specified port.
 * Input:
 *      port   - port id
 *      enable - status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - Invalid port id.
 *      RT_ERR_INPUT        - Invalid input parameters.
  * Note:
 *      Set enabling ptp function by port
 */
extern int32
dal_rtl8277c_rt_time_ptpPortEnable_set(rt_port_t port, rt_enable_t enable);

/* Function Name:
 *      dal_rtl8277c_rt_time_ptpPortEnable_get
 * Description:
 *      Get PTP status of the specified port.
 * Input:
 *      port    - port id
 * Output:
 *      pEnable - status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      Get enabling status of prp function
 */
extern int32
dal_rtl8277c_rt_time_ptpPortEnable_get(rt_port_t port, rt_enable_t *pEnable);

/* Function Name:
 *      rt_time_ptpIgrAction_set
 * Description:
 *      Set Type of PTP ingress action .
 * Input:
 *      type   - ptp packtet type
 *      action - ptp ingress action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - Invalid input parameters.
  * Note:
 *      Type is Ethertype0x88f7 or udp port 319/320
 *      Action is no-operation or trap-to-cpu
 */
extern int32
dal_rtl8277c_rt_time_ptpIgrAction_set(rt_time_ptpIgr_type_t type,rt_time_ptpIgr_action_t action);

/* Function Name:
 *      dal_rtl8277c_rt_time_ptpIgrAction_get
 * Description:
 *      Get Type of PTP ingress action .
 * Input:
 *      type   - ptp packtet type
 * Output:
 *      pAction - ptp ingress action
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      Type is Ethertype0x88f7 or udp port 319/320
 *      Action is no-operation or trap-to-cpu
 */
extern int32
dal_rtl8277c_rt_time_ptpIgrAction_get(rt_time_ptpIgr_type_t type,rt_time_ptpIgr_action_t *pAction);

/* Function Name:
 *      dal_rtl8277c_rt_time_ptpEgrAction_set
 * Description:
 *      Set Type of PTP egress action .
 * Input:
 *      action - ptp engress action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
  * Note:
 *      Action is no-operation or one-step or two-step
 */
extern int32
dal_rtl8277c_rt_time_ptpEgrAction_set(rt_time_ptpEgr_action_t action);

/* Function Name:
 *      dal_rtl8277c_rt_rt_time_ptpEgrAction_get
 * Description:
 *      Get Type of PTP egress action .
 * Input:
 *      None
 * Output:
 *      pAction - ptp egress action
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *     Action is no-operation or one-step or two-step
 */
extern int32
dal_rtl8277c_rt_rt_time_ptpEgrAction_get(rt_time_ptpEgr_action_t *pAction);

/* Function Name:
 *      dal_rtl8277c_rt_time_ponTodTime_set
 * Description:
 *      Set the PON TOD time.
 * Input:
 *      ponTod   - pon tod configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 * Note:
 *      Set corresponding superfram/localtime to update timer
 */
extern int32
dal_rtl8277c_rt_time_ponTodTime_set(rt_pon_tod_t ponTod);

/* Function Name:
 *      dal_rtl8277c_rt_time_ponTodTime_get
 * Description:
 *      Get the PON TOD time.
 * Input:
 *      None
 * Output:
 *      pPonTod   - pon tod configuration
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 * Note:
 *      Get corresponding superfram/localtime to update timer
 */
extern int32
dal_rtl8277c_rt_time_ponTodTime_get(rt_pon_tod_t *pPonTod);

/* Function Name:
 *      dal_rtl8277c_rt_time_ponTodTimeEnable_set
 * Description:
 *      Set the PON TOD time.
 * Input:
 *      enable   - enable pon tod configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 * Note:
 *      Enable/Disable PON ToD to update time
 */
extern int32
dal_rtl8277c_rt_time_ponTodTimeEnable_set(rt_enable_t enable);

/* Function Name:
 *      dal_rtl8277c_rt_time_ponTodTimeEnable_get
 * Description:
 *      Get the PON TOD time.
 * Input:
 *      None
 * Output:
 *      pEnable   - enable pon tod configuration
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 * Note:
 *      Getting status enable/eisable PON ToD to update time
 */
extern int32
dal_rtl8277c_rt_time_ponTodTimeEnable_get(rt_enable_t *pEnable);

#endif /* __DAL_RTL8277C_TIME_H__ */
