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

#ifndef __DAL_MAPLE_TIME_H__
#define __DAL_MAPLE_TIME_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/time.h>
#include <dal/dal_mgmt.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */

/*
 * Macro Definition
 */

/*
 * Function Declaration
 */

/* Module Name : TIME */


/* Function Name:
 *      dal_maple_timeMapper_init
 * Description:
 *      Hook time module of the specified device.
 * Input:
 *      pMapper - pointer of mapper
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      Must Hook time module before calling any time APIs.
 */
extern int32
dal_maple_timeMapper_init(dal_mapper_t *pMapper);

/* Function Name:
 *      dal_maple_time_init
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
dal_maple_time_init(uint32 unit);

/* Function Name:
 *      dal_maple_time_portPtpEnable_get
 * Description:
 *      Get TIME status of the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer buffer of PTP state
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
dal_maple_time_portPtpEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_maple_time_portPtpEnable_set
 * Description:
 *      Set TIME status of the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      enable  - PTP state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_maple_time_portPtpEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      dal_maple_time_portPtpRxTimestamp_get
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
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_maple_time_portPtpRxTimestamp_get(
    uint32                      unit,
    rtk_port_t                  port,
    rtk_time_ptpIdentifier_t    identifier,
    rtk_time_timeStamp_t        *pTimeStamp);

/* Function Name:
 *      dal_maple_time_portPtpTxTimestampCallback_register
 * Description:
 *      Register TIME transmission callback function of the TIME identifier of the dedicated port to the specified device.
 * Input:
 *      unit              - unit id
 *      port              - port id
 *      identifier        - indentifier of TIME packet
 *      rtk_ptp_time_cb_f - callback function
 * Output:
 *      None
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
dal_maple_time_portPtpTxTimestampCallback_register(
    uint32                      unit,
    rtk_port_t                  port,
    rtk_time_ptpIdentifier_t    identifier,
    rtk_time_ptpTime_cb_f       *fCb);

/* Function Name:
 *      dal_maple_time_portRefTime_get
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
dal_maple_time_portRefTime_get(uint32 unit, rtk_port_t port, rtk_time_timeStamp_t *pTimeStamp);

/* Function Name:
 *      dal_maple_time_portRefTime_set
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
dal_maple_time_portRefTime_set(uint32 unit, rtk_portmask_t portmask, rtk_time_timeStamp_t timeStamp, uint32 exec);

/* Function Name:
 *      dal_maple_ptp_portTimeAdjust_set
 * Description:
 *      Adjust TIME reference time.
 * Input:
 *      unit      - unit id
 *      portmask    - portmask
 *      sign      - significant
 *      timeStamp - reference timestamp value
 *      exec       - 0 : do not execute, 1: execute 
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
dal_maple_time_portRefTimeAdjust_set(uint32 unit, rtk_portmask_t portmask, uint32 sign, rtk_time_timeStamp_t timeStamp, uint32 exec);

/* Function Name:
 *      dal_maple_time_refTimeEnable_get
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
dal_maple_time_portRefTimeEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_maple_time_portRefTimeEnable_set
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
dal_maple_time_portRefTimeEnable_set(uint32 unit, rtk_portmask_t portmask, rtk_enable_t enable);

/* Function Name:
 *      dal_maple_time_refTimeForceEnable_get
 * Description:
 *      Get the force enable state of reference time of the specified device.
 * Input:
 *      unit    - unit id
 * Output:
 *      pEnable - pointer buffer of reference time state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_maple_time_refTimeForceEnable_get(uint32 unit, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_maple_time_refTimeForceEnable_set
 * Description:
 *      Set the force enable state of reference time of the specified device.
 * Input:
 *      unit   - unit id
 *      enable - reference time state
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
dal_maple_time_refTimeForceEnable_set(uint32 unit, rtk_enable_t enable);

/* Function Name:
 *      dal_maple_time_itagTpid_get
 * Description:
 *      Get PTP inner tag TPID in the specified device.
 * Input:
 *      unit      - unit id
 * Output:
 *      pItagTpid - pointer buffer of PTP inner tag TPID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_maple_time_itagTpid_get(uint32 unit, uint32 *pItagTpid);

/* Function Name:
 *      dal_maple_time_itagTpid_set
 * Description:
 *      Set PTP inner tag TPID in the specified device.
 * Input:
 *      unit     - unit id
 *      itagTpid - PTP inner tag TPID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 * Note:
 *      None
 */
extern int32
dal_maple_time_itagTpid_set(uint32 unit, uint32 itagTpid);

/* Function Name:
 *      dal_maple_time_otagTpid_get
 * Description:
 *      Get PTP outer tag TPID in the specified device.
 * Input:
 *      unit      - unit id
 * Output:
 *      pOtagTpid - pointer buffer of PTP outer tag TPID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_maple_time_otagTpid_get(uint32 unit, uint32 *pOtagTpid);

/* Function Name:
 *      dal_maple_time_otagTpid_set
 * Description:
 *      Set PTP outer tag TPID in the specified device.
 * Input:
 *      unit     - unit id
 *      otagTpid - PTP outer tag TPID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 * Note:
 *      None
 */
extern int32
dal_maple_time_otagTpid_set(uint32 unit, uint32 otagTpid);

/* Function Name:
 *      dal_maple_time_mac_get
 * Description:
 *      Get PTP mac address in the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      pMac - pointer buffer of PTP mac address
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_maple_time_mac_get(uint32 unit, rtk_mac_t *pMac);

/* Function Name:
 *      dal_maple_time_mac_set
 * Description:
 *      Set PTP mac address in the specified device.
 * Input:
 *      unit - unit id
 *      pMac - pointer buffer of PTP mac address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_maple_time_mac_set(uint32 unit, rtk_mac_t *pMac);

/* Function Name:
 *      dal_maple_time_offLoadEnable_get
 * Description:
 *      Get PTP off load state in the specified device.
 * Input:
 *      unit    - unit id
 * Output:
 *      pEnable - pointer buffer of PTP off load state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_maple_time_offLoadEnable_get(uint32 unit, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_maple_time_offLoadEnable_set
 * Description:
 *      Set PTP off load state in the specified device.
 * Input:
 *      unit   - unit id
 *      enable - PTP off load state
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
dal_maple_time_offLoadEnable_set(uint32 unit, rtk_enable_t enable);

/* Function Name:
 *      dal_maple_time_offLoadSaveTsEnable_get
 * Description:
 *      Get PTP off load save time stampe state in the specified device.
 * Input:
 *      unit    - unit id
 * Output:
 *      pEnable - pointer buffer of PTP off load save time stampe state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_maple_time_offLoadSaveTsEnable_get(uint32 unit, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_maple_time_offLoadSaveTsEnable_set
 * Description:
 *      Set PTP off load save time stampe state in the specified device.
 * Input:
 *      unit   - unit id
 *      enable - PTP off load save time stampe state
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
dal_maple_time_offLoadSaveTsEnable_set(uint32 unit, rtk_enable_t enable);

/* Function Name:
 *      dal_maple_time_portIntFlag_get
 * Description:
 *      Get PTP transmit Int state in the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pFlag - pointer to the interrupt flag
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID        - invalid portid
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_maple_time_portIntFlag_get(uint32 unit, rtk_port_t port, uint32 * pFlag);

/* Function Name:
 *      dal_maple_time_rxIntEnable_get
 * Description:
 *      Get PTP receive Int state in the specified type from the specified device.
 * Input:
 *      unit    - unit id
 *      msgType - PTP message type
 * Output:
 *      pEnable - pointer buffer of PTP receive Int state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_maple_time_rxIntEnable_get(uint32 unit, rtk_time_ptpMsgType_t msgType, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_maple_time_rxIntEnable_set
 * Description:
 *      Set PTP receive Int state in the specified type in the specified device.
 * Input:
 *      unit    - unit id
 *      msgType - PTP message type
 *      enable  - PTP receive Int state
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
dal_maple_time_rxIntEnable_set(uint32 unit, rtk_time_ptpMsgType_t msgType, rtk_enable_t enable);

/* Function Name:
 *      dal_maple_time_txIntEnable_get
 * Description:
 *      Get PTP transmit Int state in the specified type from the specified device.
 * Input:
 *      unit    - unit id
 *      msgType - PTP message type
 * Output:
 *      pEnable - pointer buffer of PTP transmit Int state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_maple_time_txIntEnable_get(uint32 unit, rtk_time_ptpMsgType_t msgType, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_maple_time_txIntEnable_set
 * Description:
 *      Set PTP transmit Int state in the specified type in the specified device.
 * Input:
 *      unit    - unit id
 *      msgType - PTP message type
 *      enable  - PTP transmit Int state
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
dal_maple_time_txIntEnable_set(uint32 unit, rtk_time_ptpMsgType_t msgType, rtk_enable_t enable);

/* Function Name:
 *      dal_maple_time_portRxIntFlag_get
 * Description:
 *      Get PTP receive Int flag in the specified port and type from the specified device.
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      msgType  - PTP message type
 * Output:
 *      pIntFlag - pointer buffer of PTP receive Int flag
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_maple_time_portRxIntFlag_get(uint32 unit, rtk_port_t port, rtk_time_ptpMsgType_t msgType, uint32 *pIntFlag);

/* Function Name:
 *      dal_maple_time_portRxIntFlag_clear
 * Description:
 *      Clear PTP receive Int flag in the specified port and type in the specified device.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      msgType - PTP message type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_maple_time_portRxIntFlag_clear(uint32 unit, rtk_port_t port, rtk_time_ptpMsgType_t msgType);

/* Function Name:
 *      dal_maple_time_portTxIntFlag_get
 * Description:
 *      Get PTP transmit Int flag in the specified port and type from the specified device.
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      msgType  - PTP message type
 * Output:
 *      pIntFlag - pointer buffer of PTP transmit Int flag
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_maple_time_portTxIntFlag_get(uint32 unit, rtk_port_t port, rtk_time_ptpMsgType_t msgType, uint32 *pIntFlag);

/* Function Name:
 *      dal_maple_time_portTxIntFlag_clear
 * Description:
 *      Clear PTP transmit Int flag in the specified port and type in the specified device.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      msgType - PTP message type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_maple_time_portTxIntFlag_clear(uint32 unit, rtk_port_t port, rtk_time_ptpMsgType_t msgType);

#endif /* __DAL_MAPLE_TIME_H__ */

