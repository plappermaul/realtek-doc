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

#ifndef __RTK_TIME_H__
#define __RTK_TIME_H__

/*
 * Include Files
 */
#include <common/rt_type.h>

/*
 * Symbol Definition
 */

typedef enum rtk_time_clkOutMode_e
{
    PTP_CLK_OUT_REPEAT = 0,
    PTP_CLK_OUT_PULSE = 1,
    PTP_CLK_OUT_END
} rtk_time_clkOutMode_t;

typedef enum rtk_time_outSigSel_e
{
    PTP_OUT_SIG_SEL_CLOCK = 0,
    PTP_OUT_SIG_SEL_1PPS = 1,
    PTP_OUT_SIG_SEL_DISABLE = 2,
    PTP_OUT_SIG_SEL_END
} rtk_time_outSigSel_t;

/* TIME transmission callback function prototype */
typedef int32 (rtk_time_ptpTime_cb_f)(
    uint32                      unit,
    rtk_port_t                  port,
    rtk_time_ptpIdentifier_t    identifier,
    rtk_time_timeStamp_t        time);


typedef struct rtk_time_txTimeEntry_s
{
    uint8 valid;
    rtk_port_t port;
    rtk_time_ptpMsgType_t msg_type;
    uint32 seqId;
    rtk_time_timeStamp_t txTime;
} rtk_time_txTimeEntry_t;

typedef struct rtk_time_clkOutput_s
{
    rtk_time_clkOutMode_t mode;
    rtk_time_timeStamp_t startTime;
    uint32 halfPeriodNsec;
    uint32 halfPeriodFractionalNsec;
    rtk_enable_t enable;
    uint8 runing; //Only valid for get API
} rtk_time_clkOutput_t;

#define RTK_TIME_NSEC_MAX 999999999

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
 *      rtk_time_init
 * Description:
 *      Initialize Time module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8380, 8390, 9310
 * Note:
 *      Must initialize Time module before calling any Time APIs.
 * Changes:
 *      None
 */
extern int32
rtk_time_init(uint32 unit);

/* Function Name:
 *      rtk_time_portPtpTxTimestampCallback_register
 * Description:
 *      Register PTP transmission callback function of the PTP identifier on the dedicated port to the specified device.
 * Input:
 *      unit       - unit id
 *      port       - port id
 *      identifier - indentifier of PTP packet
 *      fCb        - callback function
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_time_portPtpTxTimestampCallback_register(
    uint32                      unit,
    rtk_port_t                  port,
    rtk_time_ptpIdentifier_t    identifier,
    rtk_time_ptpTime_cb_f       *fCb);

#endif /* __RTK_TIME_H__ */
