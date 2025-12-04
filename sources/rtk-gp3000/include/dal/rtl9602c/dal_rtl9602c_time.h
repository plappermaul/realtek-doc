/*
 * Copyright (C) 2012 Realtek Semiconductor Corp. 
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated, 
 * modified or distributed under the authorized license from Realtek. 
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER 
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED. 
 *
 * $Revision: 39101 $
 * $Date: 2013-05-03 17:35:27 +0800 (星期五, 03 五月 2013) $
 *
 * Purpose : Definition of TIME API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) IEEE 1588
 *
 */

#ifndef __DAL_RTL9602C_TIME_H__
#define __DAL_RTL9602C_TIME_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/port.h>

/*
 * Data Declaration
 */

/*
 * Macro Declaration
 */
#define RTL9602C_TIME_FREQUENCY_MAX 0xFFFFFFF
#define RTL9602C_TIME_MEANPATHDELAY_MAX 0x7FFFFFFF
#define RTL9602C_TIME_RXTIME_SEC_MAX 0x08
#define RTL9602C_TIME_RXTIME_NSEC_MAX 0x3B9ACA00 //1000000000 ns=1 s
#define RTL9602C_TIME_REFTIME_SEC_MAX 0xFFFFFFFFFFFFLL
#define RTL9602C_TIME_REFTIME_NSEC_MAX 0x1FFFFFFF
#define RTL9602C_TIME_PONTOD_SEC_MAX 0xFFFFFFFFFFFFLL
#define RTL9602C_TIME_PONTOD_NSEC_MAX 0x1FFFFFFF

/*
 * Function Declaration
 */

/* Module Name : TIME */
 
/* Function Name:
 *      dal_rtl9602c_time_transparentPort_set
 * Description:
 *      Set transparent ports to the specified device.
 * Input:
 *      port   - ports
 *      enable - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
dal_rtl9602c_time_portTransparentEnable_set(rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      dal_rtl9602c_time_transparentPort_get
 * Description:
 *      Get transparent ports to the specified device.
 * Input:
 *      port - ports
 * Output:
 *      pEnable - enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - Pointer enable point to NULL.
 * Note:
 *      None
 */
extern int32
dal_rtl9602c_time_portTransparentEnable_get(rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_rtl9602c_time_init
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
dal_rtl9602c_time_init(void);

/* Function Name:
 *      dal_rtl9602c_time_portPtpEnable_get
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
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      None
 */
extern int32
dal_rtl9602c_time_portPtpEnable_get(rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_rtl9602c_time_portPtpEnable_set
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
 *      None
 */
extern int32
dal_rtl9602c_time_portPtpEnable_set(rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      dal_rtl9602c_time_curTime_get
 * Description:
 *      Get the current time.
 * Input:
 *      None
 * Output:
 *      pTimeStamp - pointer buffer of the current time
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - Pointer pTimeStamp point to NULL.
 * Note:
 *      None
 */
extern int32
dal_rtl9602c_time_curTime_get(rtk_time_timeStamp_t *pTimeStamp);

/* Function Name:
 *      dal_rtl9602c_time_curTime_latch
 * Description:
 *      Latch the current time.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
dal_rtl9602c_time_curTime_latch(void);


/* Function Name:
 *      dal_rtl9602c_time_refTime_get
 * Description:
 *      Get the reference time.
 * Input:
 *      None
 * Output:
 *      pSign      - pointer buffer of sign
 *      pTimeStamp - pointer buffer of the reference time
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - Pointer pTimeStamp/pSign point to NULL.
 * Note:
 *      None
 */
extern int32
dal_rtl9602c_time_refTime_get(uint32 *pSign, rtk_time_timeStamp_t *pTimeStamp);


/* Function Name:
 *      dal_rtl9602c_time_refTime_set
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
dal_rtl9602c_time_refTime_set(uint32 sign, rtk_time_timeStamp_t timeStamp);

/* Function Name:
 *      dal_rtl9602c_time_frequency_set
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
dal_rtl9602c_time_frequency_set(uint32 freq);

/* Function Name:
 *      dal_rtl9602c_time_frequency_get
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
dal_rtl9602c_time_frequency_get(uint32 *freq);

/* Function Name:
 *      dal_rtl9602c_time_ptpIgrMsgAction_set
 * Description:
 *      Set ingress action configuration for PTP message.
 * Input:
 *      type          - PTP message type
 *      igr_action    - ingress action.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              	- OK
 *      RT_ERR_FAILED          	- Failed
 *      RT_ERR_NOT_ALLOWED 		  - Invalid action.
 *      RT_ERR_INPUT 			      - Invalid input parameters.
 * Note:
 *      None
 */
extern int32
dal_rtl9602c_time_ptpIgrMsgAction_set(rtk_time_ptpMsgType_t type, rtk_time_ptpIgrMsg_action_t igr_action);

/* Function Name:
 *      dal_rtl9602c_time_ptpIgrMsgAction_get
 * Description:
 *      Get ingress action configuration for PTP message.
 * Input:
 *      type          - PTP message type
 * Output:
 *      igr_action    - ingress action.
 * Return:
 *      RT_ERR_OK            - OK
 *      RT_ERR_FAILED        - Failed
 *      RT_ERR_NULL_POINTER  - Pointer igr_action point to NULL.
 * Note:
 *      None
 */
extern int32
dal_rtl9602c_time_ptpIgrMsgAction_get(rtk_time_ptpMsgType_t type, rtk_time_ptpIgrMsg_action_t *igr_action);

/* Function Name:
 *      dal_rtl9602c_time_ptpEgrMsgAction_set
 * Description:
 *      Set egress action configuration for PTP message.
 * Input:
 *      type          - PTP message type
 *      egr_action    - egress action.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              	- OK
 *      RT_ERR_FAILED          	- Failed
 *      RT_ERR_NOT_ALLOWED 		  - Invalid action.
 *      RT_ERR_INPUT 			      - Invalid input parameters.
 * Note:
 *      None
 */
extern int32
dal_rtl9602c_time_ptpEgrMsgAction_set(rtk_time_ptpMsgType_t type, rtk_time_ptpEgrMsg_action_t egr_action);

/* Function Name:
 *      dal_rtl9602c_time_ptpEgrMsgAction_get
 * Description:
 *      Get egress action configuration for PTP message.
 * Input:
 *      type          - PTP message type
 * Output:
 *      egr_action    - egress action.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_NULL_POINTER - Pointer egr_action point to NULL.
 * Note:
 *      None
 */
extern int32
dal_rtl9602c_time_ptpEgrMsgAction_get(rtk_time_ptpMsgType_t type, rtk_time_ptpEgrMsg_action_t *egr_action);

/* Function Name:
 *      dal_rtl9602c_time_meanPathDelay_set
 * Description:
 *      Set dal_rtl9602c_time_meanPathDelay_set of PTP system time.
 * Input:
 *      delay - mean path delay value
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
dal_rtl9602c_time_meanPathDelay_set(uint32 delay);

/* Function Name:
 *      dal_rtl9602c_time_meanPathDelay_get
 * Description:
 *      Get dal_rtl9602c_time_meanPathDelay_get of PTP system time.
 * Input:
 *      delay    - mean path delay.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_NULL_POINTER - - Pointer delay point to NULL.
 * Note:
 *      None
 */
extern int32
dal_rtl9602c_time_meanPathDelay_get(uint32 *delay);

/* Function Name:
 *      dal_rtl9602c_time_rxTime_set
 * Description:
 *      Configure user RX timestamp.
 * Input:
 *      timeStamp - RX timestamp value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              	- OK
 *      RT_ERR_FAILED          	- Failed
 * Note:
 *      
 */
extern int32
dal_rtl9602c_time_rxTime_set(rtk_time_timeStamp_t timeStamp);

/* Function Name:
 *      dal_rtl9602c_time_rxTime_get
 * Description:
 *      Get RX timestamp.
 * Input:
 *      None
 * Output:
 *      pTimeStamp - pointer buffer of the RX time
 * Return:
 *      RT_ERR_OK              	- OK
 *      RT_ERR_FAILED          	- Failed
 *      RT_ERR_NULL_POINTER     - Pointer pTimeStamp point to NULL.
 * Note:
 *      
 */
extern int32
dal_rtl9602c_time_rxTime_get(rtk_time_timeStamp_t *pTimeStamp);

/* Function Name:
 *      dal_rtl9602c_time_ponTodTime_set
 * Description:
 *      Configure PON TOD timestamp.
 * Input:
 *      timeStamp - PON TOD timestamp value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              	- OK
 *      RT_ERR_FAILED          	- Failed
 * Note:
 *
 */
extern int32
dal_rtl9602c_time_ponTodTime_set(rtk_pon_tod_t ponTod);

/* Function Name:
 *      dal_rtl9602c_time_ponTodTime_get
 * Description:
 *      Get PON TOD timestamp.
 * Input:
 *      None
 * Output:
 *      pTimeStamp - pointer buffer of the PON TOD time
 * Return:
 *      RT_ERR_OK              	- OK
 *      RT_ERR_FAILED          	- Failed
 *      RT_ERR_NULL_POINTER     - Pointer pTimeStamp point to NULL.
 * Note:
 *
 */
extern int32
dal_rtl9602c_time_ponTodTime_get(rtk_pon_tod_t *pPonTod);

/* Function Name:
 *      dal_rtl9602c_time_portPtpTxIndicator_get
 * Description:
 *      Get PTP tx indicator of the specified port.
 * Input:
 *      port    - port id
 * Output:
 *      pTxIndicator - point of tx indicator
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      None
 */
extern int32
dal_rtl9602c_time_portPtpTxIndicator_get(rtk_port_t port, rtk_enable_t *pTxIndicator);

/* Function Name:
 *      dal_rtl9602c_time_todEnable_get
 * Description:
 *      Get pon tod status.
 * Input:
 *      None.
 * Output:
 *      pEnable - status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      None
 */
extern int32
dal_rtl9602c_time_todEnable_get(rtk_enable_t *pEnable);

/* Function Name:
 *      dal_rtl9602c_time_todEnable_set
 * Description:
 *      Set pon tod status.
 * Input:
 *      enable - status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - Invalid input parameters.
  * Note:
 *      None
 */
extern int32
dal_rtl9602c_time_todEnable_set(rtk_enable_t enable);

/* Function Name:
 *      dal_rtl9602c_time_ppsEnable_get
 * Description:
 *      Get PTP PPS status.
 * Input:
 *      None.
 * Output:
 *      pEnable - status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      None
 */
extern int32
dal_rtl9602c_time_ppsEnable_get(rtk_enable_t *pEnable);

/* Function Name:
 *      dal_rtl9602c_time_ppsEnable_set
 * Description:
 *      Set PTP PPS status.
 * Input:
 *      enable - status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - Invalid input parameters.
  * Note:
 *      None
 */
extern int32
dal_rtl9602c_time_ppsEnable_set(rtk_enable_t enable);

/* Function Name:
 *      dal_rtl9602c_time_ppsMode_get
 * Description:
 *      Get PTP PPS mode.
 * Input:
 *      None.
 * Output:
 *      pMode - pps mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      None
 */
extern int32
dal_rtl9602c_time_ppsMode_get(rtk_time_ptpPpsMode_t *pMode);

/* Function Name:
 *      dal_rtl9602c_time_ppsMode_set
 * Description:
 *      Set PTP PPS mode.
 * Input:
 *      mode - pps mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - Invalid input parameters.
  * Note:
 *      None
 */
extern int32
dal_rtl9602c_time_ppsMode_set(rtk_time_ptpPpsMode_t mode);

#endif /* __DAL_RTL9602C_TIME_H__ */
