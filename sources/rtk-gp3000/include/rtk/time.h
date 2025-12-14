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
 * $Revision$
 * $Date$
 *
 * Purpose : Definition of TIME API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) IEEE 1588
 *
 */

#ifndef __RTK_TIME_H__
#define __RTK_TIME_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/port.h>

/*
 * Symbol Definition
 */
enum {
    TOD_PON_MODE_EPON,
	TOD_PON_MODE_GPON,
	END_TOD_PON_MODE
};
/* Reference Time */
typedef struct rtk_time_timeStamp_s
{
    uint64 sec;
    uint32 nsec;
} rtk_time_timeStamp_t;

typedef struct rtk_pon_tod_s
{
    uint32 ponMode;
    union
    {   
        uint32 localTime;
        uint32 superFrame;
    } startPoint;
    rtk_time_timeStamp_t timeStamp;
} rtk_pon_tod_t;

/* Message Type */
typedef enum rtk_time_ptpMsgType_e
{
    PTP_MSG_TYPE_SYNC = 0,
    PTP_MSG_TYPE_DELAY_REQ,
    PTP_MSG_TYPE_PDELAY_REQ,
    PTP_MSG_TYPE_PDELAY_RESP,
    PTP_MSG_TYPE_FOLLOW_UP,
    PTP_MSG_TYPE_DELAY_RESP,
    PTP_MSG_TYPE_PDELAY_RESP_FOLLOW_UP,
    PTP_MSG_TYPE_ANNOUNCE_SIGNALING,
    PTP_MSG_TYPE_SYNC_ONE_STEP,
    PTP_MSG_TYPE_PDELAY_REQ_ONE_STEP,
    PTP_MSG_TYPE_END
} rtk_time_ptpMsgType_t;

typedef enum rtk_time_ptpIgrMsg_action_e
{
    PTP_IGR_ACTION_NONE = 0,
    PTP_IGR_ACTION_TRAP2CPU,
    PTP_IGR_ACTION_FORWARD2TRANS,
    PTP_IGR_ACTION_FORWARD2TRANS_AND_MIRROR2CPU,
    PTP_IGR_ACTION_END
} rtk_time_ptpIgrMsg_action_t;

typedef enum rtk_time_ptpEgrMsg_action_e
{
    PTP_EGR_ACTION_NONE = 0,
    PTP_EGR_ACTION_LATCH_TIME,
    PTP_EGR_ACTION_LATCH_TIME_AND_MIRROR2CPU,
    PTP_EGR_ACTION_MODIFY_CORRECTION,
    PTP_EGR_ACTION_END
} rtk_time_ptpEgrMsg_action_t;

typedef enum rtk_time_ptpRefSign_e
{
    PTP_REF_SIGN_POSTIVE = 0,
    PTP_REF_SIGN_NEGATIVE,
    PTP_REF_SIGN_END
} rtk_time_ptpRefSign_t;

typedef enum rtk_time_ptpPpsMode_e
{
    PTP_PPS_MODE_PON = 0,
    PTP_PPS_MODE_PTP,
    PTP_PPS_MODE_END
} rtk_time_ptpPpsMode_t;

typedef struct rtk_time_freerun_s
{
    uint64 nsec;
} rtk_time_freerun_t;

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
 *      rtk_time_transparentPort_set
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
rtk_time_portTransparentEnable_set(rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      rtk_time_transparentPort_get
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
rtk_time_portTransparentEnable_get(rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_time_init
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
rtk_time_init(void);

/* Function Name:
 *      rtk_time_portPtpEnable_get
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
rtk_time_portPtpEnable_get(rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_time_portPtpEnable_set
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
rtk_time_portPtpEnable_set(rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      rtk_time_curTime_get
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
rtk_time_curTime_get(rtk_time_timeStamp_t *pTimeStamp);

/* Function Name:
 *      rtk_time_curTime_latch
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
rtk_time_curTime_latch(void);

/* Function Name:
 *      rtk_time_refTime_get
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
rtk_time_refTime_get(uint32 *pSign, rtk_time_timeStamp_t *pTimeStamp);

/* Function Name:
 *      rtk_time_refTime_set
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
rtk_time_refTime_set(uint32 sign, rtk_time_timeStamp_t timeStamp);

/* Function Name:
 *      rtk_time_frequency_set
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
rtk_time_frequency_set(uint32 freq);

/* Function Name:
 *      rtk_time_frequency_get
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
rtk_time_frequency_get(uint32 *freq);

/* Function Name:
 *      rtk_time_ptpIgrMsgAction_set
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
rtk_time_ptpIgrMsgAction_set(rtk_time_ptpMsgType_t type, rtk_time_ptpIgrMsg_action_t igr_action);

/* Function Name:
 *      rtk_time_ptpIgrMsgAction_get
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
rtk_time_ptpIgrMsgAction_get(rtk_time_ptpMsgType_t type, rtk_time_ptpIgrMsg_action_t *igr_action);

/* Function Name:
 *      rtk_time_ptpEgrMsgAction_set
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
rtk_time_ptpEgrMsgAction_set(rtk_time_ptpMsgType_t type, rtk_time_ptpEgrMsg_action_t egr_action);

/* Function Name:
 *      rtk_time_ptpEgrMsgAction_get
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
rtk_time_ptpEgrMsgAction_get(rtk_time_ptpMsgType_t type, rtk_time_ptpEgrMsg_action_t *egr_action);

/* Function Name:
 *      rtk_time_meanPathDelay_set
 * Description:
 *      Set rtk_time_meanPathDelay_set of PTP system time.
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
rtk_time_meanPathDelay_set(uint32 delay);

/* Function Name:
 *      rtk_time_meanPathDelay_get
 * Description:
 *      Get rtk_time_meanPathDelay_get of PTP system time.
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
rtk_time_meanPathDelay_get(uint32 *delay);

/* Function Name:
 *      rtk_time_rxTime_set
 * Description:
 *      Set the RX time.
 * Input:
 *      timeStamp   - RX timestamp.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              	- OK
 *      RT_ERR_FAILED          	- Failed
 * Note:
 *      
 */
extern int32
rtk_time_rxTime_set(rtk_time_timeStamp_t timeStamp);

/* Function Name:
 *      rtk_time_rxTime_get
 * Description:
 *      Get the RX time.
 * Input:
 *      None.
 * Output:
 *      pTimeStamp - pointer buffer of the RX time.
 * Return:
 *      RT_ERR_OK              	- OK
 *      RT_ERR_FAILED          	- Failed
 *      RT_ERR_NULL_POINTER     - Pointer pTimeStamp point to NULL.
 * Note:
 *      
 */
extern int32
rtk_time_rxTime_get(rtk_time_timeStamp_t *pTimeStamp);

/* Function Name:
 *      rtk_time_ponTodTime_set
 * Description:
 *      Set the PON TOD time.
 * Input:
 *      timeStamp   - RX timestamp.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              	- OK
 *      RT_ERR_FAILED          	- Failed
 * Note:
 *      
 */
extern int32
rtk_time_ponTodTime_set(rtk_pon_tod_t ponTod);

/* Function Name:
 *      rtk_time_gpon_ponTodTime_set
 * Description:
 *      Set the PON TOD time.
 * Input:
 *      timeStamp   - RX timestamp.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 * Note:
 *      
 */
extern int32
rtk_time_gpon_ponTodTime_set(rtk_pon_tod_t ponTod);

/* Function Name:
 *      rtk_time_epon_ponTodTime_set
 * Description:
 *      Set the PON TOD time.
 * Input:
 *      timeStamp   - RX timestamp.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 * Note:
 *      
 */
extern int32
rtk_time_epon_ponTodTime_set(rtk_pon_tod_t ponTod);

/* Function Name:
 *      rtk_time_ponTodTime_get
 * Description:
 *      Get the PON TOD time.
 * Input:
 *      None.
 * Output:
 *      pTimeStamp - pointer buffer of the RX time.
 * Return:
 *      RT_ERR_OK              	- OK
 *      RT_ERR_FAILED          	- Failed
 *      RT_ERR_NULL_POINTER     - Pointer pTimeStamp point to NULL.
 * Note:
 *      
 */
extern int32
rtk_time_ponTodTime_get(rtk_pon_tod_t *pPonTod);

/* Function Name:
 *      rtk_time_portPtpTxIndicator_get
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
rtk_time_portPtpTxIndicator_get(rtk_port_t port, rtk_enable_t *pTxIndicator);

/* Function Name:
 *      rtk_time_todEnable_get
 * Description:
 *      Get the configuration about pon tod synchronization status.
 * Input:
 *      None.
 * Output:
 *      pEnable - status of pon tod synchronization.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      The status of pon tod synchronization:
 *      - DISABLED
 *      - ENABLED
 */
extern int32
rtk_time_todEnable_get(rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_time_todEnable_set
 * Description:
 *      Set the configuration about pon tod synchronization state.
 * Input:
 *      enable - status of pon tod synchronization.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT   - Invalid input parameter
 * Note:
 *      The status of pon tod synchronization:
 *      - DISABLED
 *      - ENABLED
 */
extern int32
rtk_time_todEnable_set(rtk_enable_t enable);

/* Function Name:
 *      rtk_time_ppsEnable_get
 * Description:
 *      Get the configuration about PTP PPS status.
 * Input:
 *      None.
 * Output:
 *      pEnable - status of PTP PPS.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      The status of PTP PPS:
 *      - DISABLED
 *      - ENABLED
 */
extern int32
rtk_time_ppsEnable_get(rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_time_ppsEnable_set
 * Description:
 *      Set the configuration about PTP PPS state.
 * Input:
 *      enable - status of PTP PPS.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT   - Invalid input parameter
 * Note:
 *      The status of PTP PPS:
 *      - DISABLED
 *      - ENABLED
 */
extern int32
rtk_time_ppsEnable_set(rtk_enable_t enable);

/* Function Name:
 *      rtk_time_ppsMode_get
 * Description:
 *      Get the configuration about PTP PPS mode.
 * Input:
 *      None.
 * Output:
 *      pMode - mode of PTP PPS.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      The status of PTP PPS mode:
 *      - PON
 *      - PTP
 */
extern int32
rtk_time_ppsMode_get(rtk_time_ptpPpsMode_t *pMode);

/* Function Name:
 *      rtk_time_ppsMode_set
 * Description:
 *      Set the configuration about PTP PPS mode.
 * Input:
 *      mode - mode of PTP PPS.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT   - Invalid input parameter
 * Note:
 *      The mode of PTP PPS:
 *      - PON
 *      - PTP
 */
extern int32
rtk_time_ppsMode_set(rtk_time_ptpPpsMode_t mode);


/* Function Name:
 *      rtk_time_freeTime_get
 * Description:
 *      Get the freerun time.
 * Input:
 *      None
 * Output:
 *      pFreeTime - pointer buffer of the freerun time
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - Pointer pFreeTime point to NULL.
 * Note:
 *      None
 */
extern int32
rtk_time_freeTime_get(rtk_time_freerun_t *pFreeTime);
#endif /* __RTK_TIME_H__ */
