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

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_bitop.h>
#include <common/util/rt_util.h>
#include <common/debug/rt_log.h>
#include <osal/sem.h>
#include <osal/lib.h>
#include <osal/memory.h>
#include <hal/chipdef/allmem.h>
#include <hal/chipdef/allreg.h>
#include <hal/chipdef/maple/rtk_maple_table_struct.h>
#include <hal/chipdef/maple/rtk_maple_reg_struct.h>
#include <hal/mac/reg.h>
#include <hal/mac/mem.h>
#include <hal/common/halctrl.h>
#include <dal/maple/dal_maple_time.h>
#include <dal/dal_mgmt.h>
#include <rtk/default.h>
#include <rtk/time.h>

/*
 * Symbol Definition
 */
#define PTP_INTERNAL_PORTMASK 0x0500ff00

/*
 * Data Declaration
 */
static uint32               time_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};
static osal_mutex_t         time_sem[RTK_MAX_NUM_OF_UNIT];

/*
 * Macro Definition
 */
/* time semaphore handling */
#define TIME_SEM_LOCK(unit)    \
do {\
    if (osal_sem_mutex_take(time_sem[unit], OSAL_SEM_WAIT_FOREVER) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_LOCK_FAILED, (MOD_DAL|MOD_TIME),"semaphore lock failed");\
        return RT_ERR_SEM_LOCK_FAILED;\
    }\
} while(0)
#define TIME_SEM_UNLOCK(unit)   \
do {\
    if (osal_sem_mutex_give(time_sem[unit]) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_UNLOCK_FAILED, (MOD_DAL|MOD_TIME),"semaphore unlock failed");\
        return RT_ERR_SEM_UNLOCK_FAILED;\
    }\
} while(0)
#define RT_IF_ERR_GOTO_HANDLE(op, errHandle, ret) \
do {\
    if ((ret = (op)) != RT_ERR_OK)\
        goto errHandle;\
} while(0)

/*
 * Function Declaration
 */
static int32 _dal_maple_time_ptpPortRegBaseAddr_get(rtk_port_t port, uint32 *pRegBaseAddr);

/* Module Name : TIME */
static int32 _dal_maple_time_ptpPortRegBaseAddr_get(rtk_port_t port, uint32 *pRegBaseAddr)
{
    switch (port)
    {
        case 8:
            *pRegBaseAddr = MAPLE_PTP_P8_CTRL0r;
            break;
        case 9:
            *pRegBaseAddr = MAPLE_PTP_P9_CTRL0r;
            break;
        case 10:
            *pRegBaseAddr = MAPLE_PTP_P10_CTRL0r;
            break;
        case 11:
            *pRegBaseAddr = MAPLE_PTP_P11_CTRL0r;
            break;
        case 12:
            *pRegBaseAddr = MAPLE_PTP_P12_CTRL0r;
            break;
        case 13:
            *pRegBaseAddr = MAPLE_PTP_P13_CTRL0r;
            break;
        case 14:
            *pRegBaseAddr = MAPLE_PTP_P14_CTRL0r;
            break;
        case 15:
            *pRegBaseAddr = MAPLE_PTP_P15_CTRL0r;
            break;
        case 24:
            *pRegBaseAddr = MAPLE_PTP_P24_CTRL0r;
            break;
        case 26:
            *pRegBaseAddr = MAPLE_PTP_P26_CTRL0r;
            break;
        default:
            return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of _dal_maple_time_ptpPortRegBaseAddr_get */

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
int32
dal_maple_timeMapper_init(dal_mapper_t *pMapper)
{
    pMapper->time_init = dal_maple_time_init;
    pMapper->time_portPtpEnable_get = dal_maple_time_portPtpEnable_get;
    pMapper->time_portPtpEnable_set = dal_maple_time_portPtpEnable_set;
    pMapper->time_portPtpRxTimestamp_get = dal_maple_time_portPtpRxTimestamp_get;
    pMapper->time_portPtpTxTimestampCallback_register = dal_maple_time_portPtpTxTimestampCallback_register;
    pMapper->time_portRefTime_get = dal_maple_time_portRefTime_get;
    pMapper->time_portRefTime_set = dal_maple_time_portRefTime_set;
    pMapper->time_portRefTimeAdjust_set = dal_maple_time_portRefTimeAdjust_set;
    pMapper->time_portRefTimeEnable_get = dal_maple_time_portRefTimeEnable_get;
    pMapper->time_portRefTimeEnable_set = dal_maple_time_portRefTimeEnable_set;


    return RT_ERR_OK;
}

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
int32
dal_maple_time_init(uint32 unit)
{
    RT_INIT_REENTRY_CHK(time_init[unit]);
    time_init[unit] = INIT_NOT_COMPLETED;

    /* create semaphore */
    time_sem[unit] = osal_sem_mutex_create();
    if (0 == time_sem[unit])
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_TIME), "semaphore create failed");
        return RT_ERR_FAILED;
    }

    /* set init flag to complete init */
    time_init[unit] = INIT_COMPLETED;

    return RT_ERR_OK;
} /* end of dal_maple_time_init */

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
int32
dal_maple_time_portPtpEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    uint32  val;
    uint32  regBaseAddr, regAddr;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TIME), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(time_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if ((1<<port) & PTP_INTERNAL_PORTMASK)
    {
        if (RT_ERR_OK != _dal_maple_time_ptpPortRegBaseAddr_get(port, &regBaseAddr))
            return RT_ERR_PORT_ID;

        regAddr = regBaseAddr + 12;

        TIME_SEM_LOCK(unit);

        if ((ret = reg_field_read(unit, regAddr, MAPLE_ENf, &val)) != RT_ERR_OK)
        {
            TIME_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
            return ret;
        }

        TIME_SEM_UNLOCK(unit);

        *pEnable = (val)? ENABLED : DISABLED;
    }
    else
    {
        /*to be added*/
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TIME), "pEnable=%x", *pEnable);

    return RT_ERR_OK;
} /* end of dal_maple_time_portPtpEnable_get */

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
int32
dal_maple_time_portPtpEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    uint32  val;
    uint32  regBaseAddr, regAddr;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TIME), "unit=%d, port=%d, enable=%d", unit, port, enable);

    /* check Init status */
    RT_INIT_CHK(time_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    if ((1<<port) & PTP_INTERNAL_PORTMASK)
    {
        if (RT_ERR_OK != _dal_maple_time_ptpPortRegBaseAddr_get(port, &regBaseAddr))
            return RT_ERR_PORT_ID;

        regAddr = regBaseAddr + 12;

        val = (enable == ENABLED) ? 1: 0;

        TIME_SEM_LOCK(unit);

        if ((ret = reg_field_write(unit, regAddr, MAPLE_ENf, &val)) != RT_ERR_OK)
        {
            TIME_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
            return ret;
        }

        TIME_SEM_UNLOCK(unit);
    }
    else
    {
        /*to be added*/
    }

    return RT_ERR_OK;
} /* end of dal_maple_time_portPtpEnable_set */

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
int32
dal_maple_time_portPtpRxTimestamp_get(
    uint32                      unit,
    rtk_port_t                  port,
    rtk_time_ptpIdentifier_t    identifier,
    rtk_time_timeStamp_t        *pTimeStamp)
{
    uint32  val;
    uint32  sequenceId;
    uint32  regBaseAddr;
    uint32  regAddr;
    uint32  regField;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TIME), "unit=%d, port=%d, msgType=%d, sequenceId=%d", unit, port, identifier.msgType, identifier.sequenceId);

    /* check Init status */
    RT_INIT_CHK(time_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((identifier.msgType > PTP_MSG_TYPE_PDELAY_RESP), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pTimeStamp), RT_ERR_NULL_POINTER);

    if ((1<<port) & PTP_INTERNAL_PORTMASK)
    {
        if (RT_ERR_OK != _dal_maple_time_ptpPortRegBaseAddr_get(port, &regBaseAddr))
            return RT_ERR_PORT_ID;

        switch (identifier.msgType)
        {
            case PTP_MSG_TYPE_SYNC:
                regAddr = regBaseAddr + 4;
                regField = MAPLE_RX_SYNC_SEQ_IDf;
                break;
            case PTP_MSG_TYPE_DELAY_REQ:
                regAddr = regBaseAddr + 5;
                regField = MAPLE_RX_DELAY_REQ_SEQ_IDf;
                break;
            case PTP_MSG_TYPE_PDELAY_REQ:
                regAddr = regBaseAddr + 6;
                regField = MAPLE_RX_PDELAY_REQ_SEQ_IDf;
                break;
            case PTP_MSG_TYPE_PDELAY_RESP:
                regAddr = regBaseAddr + 7;
                regField = MAPLE_RX_PDELAY_RESP_SEQ_IDf;
                break;
            default:
                return RT_ERR_INPUT;
        }

        TIME_SEM_LOCK(unit);

        if ((ret = reg_field_read(unit, regAddr, regField, &sequenceId)) != RT_ERR_OK)
        {
            TIME_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
            return ret;
        }

        if(sequenceId != identifier.sequenceId)
        {
            TIME_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
            return RT_ERR_PTP_SEQUENCE_ID;
        }

        regAddr = regBaseAddr + 8;
        if ((ret = reg_field_read(unit, regAddr, MAPLE_NSEC_Lf, &val)) != RT_ERR_OK)
        {
            TIME_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
            return ret;
        }
        pTimeStamp->nsec = (pTimeStamp->nsec & 0x3ff80000) | ((val & 0xffff)<<3);

        regAddr = regBaseAddr + 9;
        if ((ret = reg_field_read(unit, regAddr, MAPLE_NSEC_Hf, &val)) != RT_ERR_OK)
        {
            TIME_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
            return ret;
        }
        pTimeStamp->nsec = (pTimeStamp->nsec & 0x7ffff) | ((val & 0x7ff)<<19);

        regAddr = regBaseAddr + 10;
        if ((ret = reg_field_read(unit, regAddr, MAPLE_SEC_Lf, &val)) != RT_ERR_OK)
        {
            TIME_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
            return ret;
        }
        pTimeStamp->sec = (pTimeStamp->sec & 0xffff0000) | (val & 0xffff);

        regAddr = regBaseAddr + 11;
        if ((ret = reg_field_read(unit, regAddr, MAPLE_SEC_Hf, &val)) != RT_ERR_OK)
        {
            TIME_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
            return ret;
        }
        pTimeStamp->sec = (pTimeStamp->sec & 0xffff) | ((val << 16) & 0xffff0000);

        TIME_SEM_UNLOCK(unit);
    }
    else
    {
        /*to be added*/
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TIME), "pTimeStamp->sec=%d, pTimeStamp->nsec = %d", pTimeStamp->sec, pTimeStamp->nsec);

    return RT_ERR_OK;
} /* end of dal_maple_time_portPtpRxTimestamp_get */

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
int32
dal_maple_time_portPtpTxTimestampCallback_register(
    uint32                      unit,
    rtk_port_t                  port,
    rtk_time_ptpIdentifier_t    identifier,
    rtk_time_ptpTime_cb_f       *fCb)
{
    /* need to implement after ISR handler ready */

    /* lowlevel must create a thread to notify which has registered callback function */

    return RT_ERR_OK;
} /* end of dal_maple_time_portPtpTxTimestampCallback_register */

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
int32
dal_maple_time_portRefTime_get(uint32 unit, rtk_port_t port, rtk_time_timeStamp_t *pTimeStamp)
{
    uint32  val;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TIME), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(time_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pTimeStamp), RT_ERR_NULL_POINTER);

    TIME_SEM_LOCK(unit);

    /* execute reading command */
    val = 0x0;  /* CMD = READ */
    if ((ret = reg_field_write(unit, MAPLE_PTP_GLB_CTRL1r, MAPLE_CMDf, &val)) != RT_ERR_OK)
    {
        TIME_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
        return ret;
    }

    val = 0x1;  /* EXEC */
    if ((ret = reg_field_write(unit, MAPLE_PTP_GLB_CTRL1r, MAPLE_EXECf, &val)) != RT_ERR_OK)
    {
        TIME_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
        return ret;
    }

    do {
            if ((ret = reg_field_read(unit, MAPLE_PTP_GLB_CTRL1r, MAPLE_EXECf, &val)) != RT_ERR_OK)
            {
                TIME_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
                return ret;
            }
    } while (val != 0);   /* busy watting */

    if ((ret = reg_field_read(unit, MAPLE_PTP_GLB_CTRL10r, MAPLE_RD_PTP_TIME_NSEC_Lf, &val)) != RT_ERR_OK)
    {
        TIME_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
        return ret;
    }
    pTimeStamp->nsec = (pTimeStamp->nsec & 0x3ff80000) | ((val & 0xffff)<<3);

    if ((ret = reg_field_read(unit, MAPLE_PTP_GLB_CTRL11r, MAPLE_RD_PTP_TIME_NSEC_Hf, &val)) != RT_ERR_OK)
    {
        TIME_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
        return ret;
    }
    pTimeStamp->nsec = (pTimeStamp->nsec & 0x7ffff) | ((val & 0x7ff)<<19);

    if ((ret = reg_field_read(unit, MAPLE_PTP_GLB_CTRL12r, MAPLE_RD_PTP_TIME_SEC_Lf, &val)) != RT_ERR_OK)
    {
        TIME_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
        return ret;
    }
    pTimeStamp->sec = (pTimeStamp->sec & 0xffff0000) | (val & 0xffff);

    if ((ret = reg_field_read(unit, MAPLE_PTP_GLB_CTRL13r, MAPLE_RD_PTP_TIME_SEC_Hf, &val)) != RT_ERR_OK)
    {
        TIME_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
        return ret;
    }
    pTimeStamp->sec = (pTimeStamp->sec & 0xffff) | ((val << 16) & 0xffff0000);

    TIME_SEM_UNLOCK(unit);
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TIME), "pTimeStamp->sec=%d, pTimeStamp->nsec = %d", pTimeStamp->sec, pTimeStamp->nsec);

    return RT_ERR_OK;
} /* end of dal_maple_time_refTime_get */

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
int32
dal_maple_time_portRefTime_set(uint32 unit, rtk_portmask_t portmask, rtk_time_timeStamp_t timeStamp, uint32 exec)
{
    uint32  val;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TIME), "timeStamp.sec=%d, timeStamp.nsec = %d", timeStamp.sec, timeStamp.nsec);

    /* check Init status */
    RT_INIT_CHK(time_init[unit]);

    TIME_SEM_LOCK(unit);

    val = (timeStamp.nsec>>3) & 0xffff;
    if ((ret = reg_field_write(unit, MAPLE_PTP_GLB_CTRL0r, MAPLE_PTP_TIME_NSEC_Lf, &val)) != RT_ERR_OK)
    {
        TIME_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
        return ret;
    }

    val = (timeStamp.nsec >>19) & 0x7ff;
    if ((ret = reg_field_write(unit, MAPLE_PTP_GLB_CTRL1r, MAPLE_PTP_TIME_NSEC_Hf, &val)) != RT_ERR_OK)
    {
        TIME_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
        return ret;
    }

    val = timeStamp.sec  & 0xffff;
    if ((ret = reg_field_write(unit, MAPLE_PTP_GLB_CTRL2r, MAPLE_PTP_TIME_SEC_Lf, &val)) != RT_ERR_OK)
    {
        TIME_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
        return ret;
    }

    val = (timeStamp.sec >>16) & 0xffff;
    if ((ret = reg_field_write(unit, MAPLE_PTP_GLB_CTRL3r, MAPLE_PTP_TIME_SEC_Hf, &val)) != RT_ERR_OK)
    {
        TIME_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
        return ret;
    }

    /* execute reading command */
    val = 0x1;  /* CMD = WRITE */
    if ((ret = reg_field_write(unit, MAPLE_PTP_GLB_CTRL1r, MAPLE_CMDf, &val)) != RT_ERR_OK)
    {
        TIME_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
        return ret;
    }

    val = exec;  /* EXEC */
    if ((ret = reg_field_write(unit, MAPLE_PTP_GLB_CTRL1r, MAPLE_EXECf, &val)) != RT_ERR_OK)
    {
        TIME_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
        return ret;
    }

    do {
            if ((ret = reg_field_read(unit, MAPLE_PTP_GLB_CTRL1r, MAPLE_EXECf, &val)) != RT_ERR_OK)
            {
                TIME_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
                return ret;
            }
    } while (val != 0);   /* busy watting */

    TIME_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_time_refTime_set */

/* Function Name:
 *      dal_maple_ptp_portTimeAdjust_set
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
int32
dal_maple_time_portRefTimeAdjust_set(uint32 unit, rtk_portmask_t portmask, uint32 sign, rtk_time_timeStamp_t timeStamp, uint32 exec)
{
    uint32  val;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TIME), "sign = %d, timeStamp.sec=%d, timeStamp.nsec = %d", sign, timeStamp.sec, timeStamp.nsec);

    /* check Init status */
    RT_INIT_CHK(time_init[unit]);

    TIME_SEM_LOCK(unit);

    val = (timeStamp.nsec>>3) & 0xffff;
    if ((ret = reg_field_write(unit, MAPLE_PTP_GLB_CTRL0r, MAPLE_PTP_TIME_NSEC_Lf, &val)) != RT_ERR_OK)
    {
        TIME_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
        return ret;
    }

    val = (timeStamp.nsec >>19) & 0x7ff;
    if ((ret = reg_field_write(unit, MAPLE_PTP_GLB_CTRL1r, MAPLE_PTP_TIME_NSEC_Hf, &val)) != RT_ERR_OK)
    {
        TIME_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
        return ret;
    }

    val = timeStamp.sec  & 0xffff;
    if ((ret = reg_field_write(unit, MAPLE_PTP_GLB_CTRL2r, MAPLE_PTP_TIME_SEC_Lf, &val)) != RT_ERR_OK)
    {
        TIME_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
        return ret;
    }

    val = (timeStamp.sec >>16) & 0xffff;
    if ((ret = reg_field_write(unit, MAPLE_PTP_GLB_CTRL3r, MAPLE_PTP_TIME_SEC_Hf, &val)) != RT_ERR_OK)
    {
        TIME_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
        return ret;
    }

    /* execute reading command */
    val = (sign == 0)? 0x2 : 0x3;   /* CMD = (2) Increase, (3) Decrease */
    if ((ret = reg_field_write(unit, MAPLE_PTP_GLB_CTRL1r, MAPLE_CMDf, &val)) != RT_ERR_OK)
    {
        TIME_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
        return ret;
    }

    val = exec;  /* EXEC */
    if ((ret = reg_field_write(unit, MAPLE_PTP_GLB_CTRL1r, MAPLE_EXECf, &val)) != RT_ERR_OK)
    {
        TIME_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
        return ret;
    }

    do {
            if ((ret = reg_field_read(unit, MAPLE_PTP_GLB_CTRL1r, MAPLE_EXECf, &val)) != RT_ERR_OK)
            {
                TIME_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
                return ret;
            }
    } while (val != 0);   /* busy watting */


    TIME_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_time_refTimeAdjust_set */

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
int32
dal_maple_time_portRefTimeEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    uint32  val;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TIME), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(time_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(NULL == pEnable, RT_ERR_NULL_POINTER);

    TIME_SEM_LOCK(unit);

    if ((ret = reg_field_read(unit, MAPLE_PTP_GLB_CTRL4r, MAPLE_CFG_TIMER_1588_ENf, &val)) != RT_ERR_OK)
    {
        TIME_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
        return ret;
    }

    TIME_SEM_UNLOCK(unit);

    *pEnable = (val)? ENABLED : DISABLED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TIME), "pEnable=%x", *pEnable);

    return RT_ERR_OK;
} /* end of dal_maple_time_refTimeEnable_get */

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
int32
dal_maple_time_portRefTimeEnable_set(uint32 unit, rtk_portmask_t portmask, rtk_enable_t enable)
{
    uint32  val;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TIME), "unit=%d, enable=%d", unit, enable);

    /* check Init status */
    RT_INIT_CHK(time_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    val = (enable == ENABLED) ? 1: 0;

    TIME_SEM_LOCK(unit);

    if ((ret = reg_field_write(unit, MAPLE_PTP_GLB_CTRL4r, MAPLE_CFG_TIMER_1588_ENf, &val)) != RT_ERR_OK)
    {
        TIME_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
        return ret;
    }

    TIME_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_time_refTimeEnable_set */

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
int32
dal_maple_time_refTimeForceEnable_get(uint32 unit, rtk_enable_t *pEnable)
{
    uint32  val;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TIME), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(time_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(NULL == pEnable, RT_ERR_NULL_POINTER);

    TIME_SEM_LOCK(unit);

    if ((ret = reg_field_read(unit, MAPLE_PTP_GLB_CTRL4r, MAPLE_CFG_TIMER_EN_FORCEf, &val)) != RT_ERR_OK)
    {
        TIME_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
        return ret;
    }

    TIME_SEM_UNLOCK(unit);

    *pEnable = (val)? ENABLED : DISABLED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TIME), "pEnable=%x", *pEnable);

    return RT_ERR_OK;
} /* end of dal_maple_time_refTimeEnable_get */

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
int32
dal_maple_time_refTimeForceEnable_set(uint32 unit, rtk_enable_t enable)
{
    uint32  val;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TIME), "unit=%d, enable=%d", unit, enable);

    /* check Init status */
    RT_INIT_CHK(time_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    val = (enable == ENABLED) ? 1: 0;

    TIME_SEM_LOCK(unit);

    if ((ret = reg_field_write(unit, MAPLE_PTP_GLB_CTRL4r, MAPLE_CFG_TIMER_EN_FORCEf, &val)) != RT_ERR_OK)
    {
        TIME_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
        return ret;
    }

    TIME_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_time_refTimeEnable_set */


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
int32
dal_maple_time_itagTpid_get(uint32 unit, uint32 *pItagTpid)
{
    int32 ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TIME), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(time_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(NULL == pItagTpid, RT_ERR_NULL_POINTER);

    TIME_SEM_LOCK(unit);

    if ((ret = reg_field_read(unit, MAPLE_PTP_GLB_CTRL6r, MAPLE_ITAG_TPIDf, pItagTpid)) != RT_ERR_OK)
    {
        TIME_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
        return ret;
    }

    TIME_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TIME), "pItagTpid=%x", *pItagTpid);
    return RT_ERR_OK;
} /* end of dal_maple_time_itagTpid_get */

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
int32
dal_maple_time_itagTpid_set(uint32 unit, uint32 itagTpid)
{
    int32 ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TIME), "unit=%d, itagTpid=0x%x", unit, itagTpid);

    /* check Init status */
    RT_INIT_CHK(time_init[unit]);

    TIME_SEM_LOCK(unit);

    if ((ret = reg_field_write(unit, MAPLE_PTP_GLB_CTRL6r, MAPLE_ITAG_TPIDf, &itagTpid)) != RT_ERR_OK)
    {
        TIME_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
        return ret;
    }

    TIME_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_time_itagTpid_set */

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
int32
dal_maple_time_otagTpid_get(uint32 unit, uint32 *pOtagTpid)
{
    int32 ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TIME), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(time_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(NULL == pOtagTpid, RT_ERR_NULL_POINTER);

    TIME_SEM_LOCK(unit);

    if ((ret = reg_field_read(unit, MAPLE_PTP_GLB_CTRL5r, MAPLE_OTAG_TPIDf, pOtagTpid)) != RT_ERR_OK)
    {
        TIME_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
        return ret;
    }

    TIME_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TIME), "pOtagTpid=%x", *pOtagTpid);

    return RT_ERR_OK;
} /* end of dal_maple_time_otagTpid_get */

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
int32
dal_maple_time_otagTpid_set(uint32 unit, uint32 otagTpid)
{
    int32 ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TIME), "unit=%d, otagTpid=0x%x", unit, otagTpid);

    /* check Init status */
    RT_INIT_CHK(time_init[unit]);

    TIME_SEM_LOCK(unit);

    if ((ret = reg_field_write(unit, MAPLE_PTP_GLB_CTRL5r, MAPLE_OTAG_TPIDf, &otagTpid)) != RT_ERR_OK)
    {
        TIME_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
        return ret;
    }

    TIME_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_time_otagTpid_set */

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
int32
dal_maple_time_mac_get(uint32 unit, rtk_mac_t *pMac)
{
    uint32  val;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TIME), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(time_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(NULL == pMac, RT_ERR_NULL_POINTER);

    TIME_SEM_LOCK(unit);

    if ((ret = reg_field_read(unit, MAPLE_PTP_GLB_CTRL9r, MAPLE_MAC_ADDR_Hf, &val)) != RT_ERR_OK)
    {
        TIME_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
        return ret;
    }
    pMac->octet[0] = (val >> 8) & 0xff;
    pMac->octet[1] = val  & 0xff;

    if ((ret = reg_field_read(unit, MAPLE_PTP_GLB_CTRL8r, MAPLE_MAC_ADDR_Mf, &val)) != RT_ERR_OK)
    {
        TIME_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
        return ret;
    }
    pMac->octet[2] = (val >> 8) & 0xff;
    pMac->octet[3] = val  & 0xff;

    if ((ret = reg_field_read(unit, MAPLE_PTP_GLB_CTRL7r, MAPLE_MAC_ADDR_Lf, &val)) != RT_ERR_OK)
    {
        TIME_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
        return ret;
    }
    pMac->octet[4] = (val >> 8) & 0xff;
    pMac->octet[5] = val  & 0xff;

    TIME_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TIME), "pMac = 0x%x:0x%x:0x%x:0x%x:0x%x:0x%x", pMac->octet[0],pMac->octet[1],pMac->octet[2],pMac->octet[3],pMac->octet[4],pMac->octet[5]);
    return RT_ERR_OK;
} /* end of dal_maple_time_mac_get */

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
int32
dal_maple_time_mac_set(uint32 unit, rtk_mac_t *pMac)
{
    uint32  val;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TIME), "unit=%d, pMac = 0x%x:0x%x:0x%x:0x%x:0x%x:0x%x", unit, pMac->octet[0],pMac->octet[1],pMac->octet[2],pMac->octet[3],pMac->octet[4],pMac->octet[5]);

    /* check Init status */
    RT_INIT_CHK(time_init[unit]);

    TIME_SEM_LOCK(unit);

    val = (pMac->octet[0]<<8) | pMac->octet[1];
    if ((ret = reg_field_write(unit, MAPLE_PTP_GLB_CTRL9r, MAPLE_MAC_ADDR_Hf, &val)) != RT_ERR_OK)
    {
        TIME_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
        return ret;
    }

    val = (pMac->octet[2]<<8) | pMac->octet[3];
    if ((ret = reg_field_write(unit, MAPLE_PTP_GLB_CTRL8r, MAPLE_MAC_ADDR_Mf, &val)) != RT_ERR_OK)
    {
        TIME_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
        return ret;
    }

    val = (pMac->octet[4]<<8) | pMac->octet[5];
    if ((ret = reg_field_write(unit, MAPLE_PTP_GLB_CTRL7r, MAPLE_MAC_ADDR_Lf, &val)) != RT_ERR_OK)
    {
        TIME_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
        return ret;
    }

    TIME_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_time_mac_set */

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
int32
dal_maple_time_offLoadEnable_get(uint32 unit, rtk_enable_t *pEnable)
{
    uint32  val;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TIME), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(time_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(NULL == pEnable, RT_ERR_NULL_POINTER);

    TIME_SEM_LOCK(unit);

    if ((ret = reg_field_read(unit, MAPLE_PTP_GLB_CTRL14r, MAPLE_EN_OFFLOADf, &val)) != RT_ERR_OK)
    {
        TIME_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
        return ret;
    }

    TIME_SEM_UNLOCK(unit);

    *pEnable = (val)? ENABLED : DISABLED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TIME), "pEnable=%x", *pEnable);
    return RT_ERR_OK;
} /* end of dal_maple_time_offLoadEnable_get */

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
int32
dal_maple_time_offLoadEnable_set(uint32 unit, rtk_enable_t enable)
{
    uint32  val;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TIME), "unit=%d, enable=%d", unit, enable);

    /* check Init status */
    RT_INIT_CHK(time_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    val = (enable == ENABLED) ? 1: 0;

    TIME_SEM_LOCK(unit);

    if ((ret = reg_field_write(unit, MAPLE_PTP_GLB_CTRL14r, MAPLE_EN_OFFLOADf, &val)) != RT_ERR_OK)
    {
        TIME_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
        return ret;
    }

    TIME_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_time_offLoadEnable_set */

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
int32
dal_maple_time_offLoadSaveTsEnable_get(uint32 unit, rtk_enable_t *pEnable)
{
    uint32  val;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TIME), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(time_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(NULL == pEnable, RT_ERR_NULL_POINTER);

    TIME_SEM_LOCK(unit);

    if ((ret = reg_field_read(unit, MAPLE_PTP_GLB_CTRL14r, MAPLE_SAVE_OFF_TSf, &val)) != RT_ERR_OK)
    {
        TIME_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
        return ret;
    }

    TIME_SEM_UNLOCK(unit);

    *pEnable = (val)? ENABLED : DISABLED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TIME), "pEnable=%x", *pEnable);
    return RT_ERR_OK;
} /* end of dal_maple_time_offLoadSaveTsEnable_get */

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
int32
dal_maple_time_offLoadSaveTsEnable_set(uint32 unit, rtk_enable_t enable)
{
    uint32  val;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TIME), "unit=%d, enable=%d", unit, enable);

    /* check Init status */
    RT_INIT_CHK(time_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    val = (enable == ENABLED) ? 1: 0;

    TIME_SEM_LOCK(unit);

    if ((ret = reg_field_write(unit, MAPLE_PTP_GLB_CTRL14r, MAPLE_SAVE_OFF_TSf, &val)) != RT_ERR_OK)
    {
        TIME_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
        return ret;
    }

    TIME_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_time_offLoadSaveTsEnable_set */


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
int32
dal_maple_time_portIntFlag_get(uint32 unit, rtk_port_t port, uint32 * pFlag)
{
    uint32  regField;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TIME), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(time_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(NULL == pFlag, RT_ERR_NULL_POINTER);

    switch (port)
    {
        case 8:
            regField = MAPLE_PORT_8_PTP_INTf;
            break;
        case 9:
            regField = MAPLE_PORT_9_PTP_INTf;
            break;
        case 10:
            regField = MAPLE_PORT_10_PTP_INTf;
            break;
        case 11:
            regField = MAPLE_PORT_11_PTP_INTf;
            break;
        case 12:
            regField = MAPLE_PORT_12_PTP_INTf;
            break;
        case 13:
            regField = MAPLE_PORT_13_PTP_INTf;
            break;
        case 14:
            regField = MAPLE_PORT_14_PTP_INTf;
            break;
        case 15:
            regField = MAPLE_PORT_15_PTP_INTf;
            break;
        case 24:
            regField = MAPLE_PORT_24_PTP_INTf;
            break;
        case 26:
            regField = MAPLE_PORT_26_PTP_INTf;
            break;
        default:
            return RT_ERR_PORT_ID;
    }

    TIME_SEM_LOCK(unit);

    if ((ret = reg_field_read(unit, MAPLE_PTP_GLB_CTRL15r, regField, pFlag)) != RT_ERR_OK)
    {
        TIME_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
        return ret;
    }

    TIME_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TIME), "pFlag=%x", *pFlag);
    return RT_ERR_OK;
} /* end of dal_maple_time_txIntEnable_get */


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
int32
dal_maple_time_rxIntEnable_get(uint32 unit, rtk_time_ptpMsgType_t msgType, rtk_enable_t *pEnable)
{
    uint32  val;
    uint32  regField;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TIME), "unit=%d, msgType=%d", unit, msgType);

    /* check Init status */
    RT_INIT_CHK(time_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((msgType > PTP_MSG_TYPE_PDELAY_RESP), RT_ERR_INPUT);
    RT_PARAM_CHK(NULL == pEnable, RT_ERR_NULL_POINTER);

    switch (msgType)
    {
        case PTP_MSG_TYPE_SYNC:
            regField = MAPLE_RX_SYNCf;
            break;
        case PTP_MSG_TYPE_DELAY_REQ:
            regField = MAPLE_RX_DELAYf;
            break;
        case PTP_MSG_TYPE_PDELAY_REQ:
            regField = MAPLE_RX_PDELAY_REQf;
            break;
        case PTP_MSG_TYPE_PDELAY_RESP:
            regField = MAPLE_RX_PDELAY_RESPf;
            break;
        default:
            return RT_ERR_INPUT;
    }

    TIME_SEM_LOCK(unit);

    if ((ret = reg_field_read(unit, MAPLE_PTP_GLB_CTRL14r, regField, &val)) != RT_ERR_OK)
    {
        TIME_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
        return ret;
    }

    TIME_SEM_UNLOCK(unit);

    *pEnable = (val == 1)? ENABLED : DISABLED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TIME), "pEnable=%x", *pEnable);
    return RT_ERR_OK;
} /* end of dal_maple_time_rxIntEnable_get */

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
int32
dal_maple_time_rxIntEnable_set(uint32 unit, rtk_time_ptpMsgType_t msgType, rtk_enable_t enable)
{
    uint32  val;
    uint32  regField;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TIME), "unit=%d, msgType=%d, enable=%d", unit, msgType, enable);

    /* check Init status */
    RT_INIT_CHK(time_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((msgType > PTP_MSG_TYPE_PDELAY_RESP), RT_ERR_INPUT);
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    switch (msgType)
    {
        case PTP_MSG_TYPE_SYNC:
            regField = MAPLE_RX_SYNCf;
            break;
        case PTP_MSG_TYPE_DELAY_REQ:
            regField = MAPLE_RX_DELAYf;
            break;
        case PTP_MSG_TYPE_PDELAY_REQ:
            regField = MAPLE_RX_PDELAY_REQf;
            break;
        case PTP_MSG_TYPE_PDELAY_RESP:
            regField = MAPLE_RX_PDELAY_RESPf;
            break;
        default:
            return RT_ERR_INPUT;
    }

    val = (enable == ENABLED) ? 1: 0;

    TIME_SEM_LOCK(unit);

    if ((ret = reg_field_write(unit, MAPLE_PTP_GLB_CTRL14r, regField, &val)) != RT_ERR_OK)
    {
        TIME_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
        return ret;
    }

    TIME_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_time_rxIntEnable_set */

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
int32
dal_maple_time_txIntEnable_get(uint32 unit, rtk_time_ptpMsgType_t msgType, rtk_enable_t *pEnable)
{
    uint32  val;
    uint32  regField;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TIME), "unit=%d, msgType=%d", unit, msgType);

    /* check Init status */
    RT_INIT_CHK(time_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((msgType > PTP_MSG_TYPE_PDELAY_RESP), RT_ERR_INPUT);
    RT_PARAM_CHK(NULL == pEnable, RT_ERR_NULL_POINTER);

    switch (msgType)
    {
        case PTP_MSG_TYPE_SYNC:
            regField = MAPLE_TX_SYNCf;
            break;
        case PTP_MSG_TYPE_DELAY_REQ:
            regField = MAPLE_TX_DELAYf;
            break;
        case PTP_MSG_TYPE_PDELAY_REQ:
            regField = MAPLE_TX_PDELAY_REQf;
            break;
        case PTP_MSG_TYPE_PDELAY_RESP:
            regField = MAPLE_TX_PDELAY_RESPf;
            break;
        default:
            return RT_ERR_INPUT;
    }

    TIME_SEM_LOCK(unit);

    if ((ret = reg_field_read(unit, MAPLE_PTP_GLB_CTRL14r, regField, &val)) != RT_ERR_OK)
    {
        TIME_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
        return ret;
    }

    TIME_SEM_UNLOCK(unit);

    *pEnable = (val == 1)? ENABLED : DISABLED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TIME), "pEnable=%x", *pEnable);
    return RT_ERR_OK;
} /* end of dal_maple_time_txIntEnable_get */

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
int32
dal_maple_time_txIntEnable_set(uint32 unit, rtk_time_ptpMsgType_t msgType, rtk_enable_t enable)
{
    uint32  val;
    uint32  regField;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TIME), "unit=%d, msgType=%d, enable=%d", unit, msgType, enable);

    /* check Init status */
    RT_INIT_CHK(time_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((msgType > PTP_MSG_TYPE_PDELAY_RESP), RT_ERR_INPUT);
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    switch (msgType)
    {
        case PTP_MSG_TYPE_SYNC:
            regField = MAPLE_TX_SYNCf;
            break;
        case PTP_MSG_TYPE_DELAY_REQ:
            regField = MAPLE_TX_DELAYf;
            break;
        case PTP_MSG_TYPE_PDELAY_REQ:
            regField = MAPLE_TX_PDELAY_REQf;
            break;
        case PTP_MSG_TYPE_PDELAY_RESP:
            regField = MAPLE_TX_PDELAY_RESPf;
            break;
        default:
            return RT_ERR_INPUT;
    }

    val = (enable == ENABLED) ? 1: 0;

    TIME_SEM_LOCK(unit);

    if ((ret = reg_field_write(unit, MAPLE_PTP_GLB_CTRL14r, regField, &val)) != RT_ERR_OK)
    {
        TIME_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
        return ret;
    }

    TIME_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_time_txIntEnable_set */

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
int32
dal_maple_time_portRxIntFlag_get(uint32 unit, rtk_port_t port, rtk_time_ptpMsgType_t msgType, uint32 *pIntFlag)
{
    uint32  regBaseAddr;
    uint32  regAddr;
    uint32  regField;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TIME), "unit=%d, port = %d, msgType=%d", unit, port, msgType);

    /* check Init status */
    RT_INIT_CHK(time_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((msgType > PTP_MSG_TYPE_PDELAY_RESP), RT_ERR_INPUT);
    RT_PARAM_CHK(NULL == pIntFlag, RT_ERR_NULL_POINTER);

    if ((1<<port) & PTP_INTERNAL_PORTMASK)
    {
        if (RT_ERR_OK != _dal_maple_time_ptpPortRegBaseAddr_get(port, &regBaseAddr))
            return RT_ERR_PORT_ID;

        regAddr = regBaseAddr + 12;

        switch (msgType)
        {
            case PTP_MSG_TYPE_SYNC:
                regField = MAPLE_RX_SYNCf;
                break;
            case PTP_MSG_TYPE_DELAY_REQ:
                regField = MAPLE_RX_DELAY_REQf;
                break;
            case PTP_MSG_TYPE_PDELAY_REQ:
                regField = MAPLE_RX_PDELAY_REQf;
                break;
            case PTP_MSG_TYPE_PDELAY_RESP:
                regField = MAPLE_RX_PDELAY_RESPf;
                break;
            default:
                return RT_ERR_INPUT;
        }

        TIME_SEM_LOCK(unit);

        if ((ret = reg_field_read(unit, regAddr, regField, pIntFlag)) != RT_ERR_OK)
        {
            TIME_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
            return ret;
        }

        TIME_SEM_UNLOCK(unit);
    }
    else
    {
        /*to be added*/
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TIME), "pIntFlag=%x", *pIntFlag);

    return RT_ERR_OK;
} /* end of dal_maple_time_portRxIntFlag_get */

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
int32
dal_maple_time_portRxIntFlag_clear(uint32 unit, rtk_port_t port, rtk_time_ptpMsgType_t msgType)
{
    uint32  val = 1;
    uint32  regBaseAddr;
    uint32  regAddr;
    uint32  regField;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TIME), "unit=%d, port = %d, msgType=%d", unit, port, msgType);

    /* check Init status */
    RT_INIT_CHK(time_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((msgType > PTP_MSG_TYPE_PDELAY_RESP), RT_ERR_INPUT);

    if ((1<<port) & PTP_INTERNAL_PORTMASK)
    {
        if (RT_ERR_OK != _dal_maple_time_ptpPortRegBaseAddr_get(port, &regBaseAddr))
            return RT_ERR_PORT_ID;

        regAddr = regBaseAddr + 12;

        switch (msgType)
        {
            case PTP_MSG_TYPE_SYNC:
                regField = MAPLE_RX_SYNCf;
                break;
            case PTP_MSG_TYPE_DELAY_REQ:
                regField = MAPLE_RX_DELAY_REQf;
                break;
            case PTP_MSG_TYPE_PDELAY_REQ:
                regField = MAPLE_RX_PDELAY_REQf;
                break;
            case PTP_MSG_TYPE_PDELAY_RESP:
                regField = MAPLE_RX_PDELAY_RESPf;
                break;
            default:
                return RT_ERR_INPUT;
        }

        TIME_SEM_LOCK(unit);

        if ((ret = reg_field_write(unit, regAddr, regField, &val)) != RT_ERR_OK)
        {
            TIME_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
            return ret;
        }

        TIME_SEM_UNLOCK(unit);
    }
    else
    {
        /*to be added*/
    }

    return RT_ERR_OK;
} /* end of dal_maple_time_portRxIntFlag_clear */

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
int32
dal_maple_time_portTxIntFlag_get(uint32 unit, rtk_port_t port, rtk_time_ptpMsgType_t msgType, uint32 *pIntFlag)
{
    uint32  regBaseAddr;
    uint32  regAddr;
    uint32  regField;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TIME), "unit=%d, port = %d, msgType=%d", unit, port, msgType);

    /* check Init status */
    RT_INIT_CHK(time_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((msgType > PTP_MSG_TYPE_PDELAY_RESP), RT_ERR_INPUT);
    RT_PARAM_CHK(NULL == pIntFlag, RT_ERR_NULL_POINTER);

    if ((1<<port) & PTP_INTERNAL_PORTMASK)
    {
        if (RT_ERR_OK != _dal_maple_time_ptpPortRegBaseAddr_get(port, &regBaseAddr))
            return RT_ERR_PORT_ID;

        regAddr = regBaseAddr + 12;

        switch (msgType)
        {
            case PTP_MSG_TYPE_SYNC:
                regField = MAPLE_TX_SYNCf;
                break;
            case PTP_MSG_TYPE_DELAY_REQ:
                regField = MAPLE_TX_DELAY_REQf;
                break;
            case PTP_MSG_TYPE_PDELAY_REQ:
                regField = MAPLE_TX_PDELAY_REQf;
                break;
            case PTP_MSG_TYPE_PDELAY_RESP:
                regField = MAPLE_TX_PDELAY_RESPf;
                break;
            default:
                return RT_ERR_INPUT;
        }

        TIME_SEM_LOCK(unit);

        if ((ret = reg_field_read(unit, regAddr, regField, pIntFlag)) != RT_ERR_OK)
        {
            TIME_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
            return ret;
        }

        TIME_SEM_UNLOCK(unit);
    }
    else
    {
        /*to be added*/
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TIME), "pIntFlag=%x", *pIntFlag);

    return RT_ERR_OK;
} /* end of dal_maple_time_portTxIntFlag_get */

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
int32
dal_maple_time_portTxIntFlag_clear(uint32 unit, rtk_port_t port, rtk_time_ptpMsgType_t msgType)
{
    uint32  val = 1;
    uint32  regBaseAddr;
    uint32  regAddr;
    uint32  regField;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TIME), "unit=%d, port = %d, msgType=%d", unit, port, msgType);

    /* check Init status */
    RT_INIT_CHK(time_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((msgType > PTP_MSG_TYPE_PDELAY_RESP), RT_ERR_INPUT);

    if ((1<<port) & PTP_INTERNAL_PORTMASK)
    {
        if (RT_ERR_OK != _dal_maple_time_ptpPortRegBaseAddr_get(port, &regBaseAddr))
            return RT_ERR_PORT_ID;

        regAddr = regBaseAddr + 12;

        switch (msgType)
        {
            case PTP_MSG_TYPE_SYNC:
                regField = MAPLE_TX_SYNCf;
                break;
            case PTP_MSG_TYPE_DELAY_REQ:
                regField = MAPLE_TX_DELAY_REQf;
                break;
            case PTP_MSG_TYPE_PDELAY_REQ:
                regField = MAPLE_TX_PDELAY_REQf;
                break;
            case PTP_MSG_TYPE_PDELAY_RESP:
                regField = MAPLE_TX_PDELAY_RESPf;
                break;
            default:
                return RT_ERR_INPUT;
        }

        TIME_SEM_LOCK(unit);

        if ((ret = reg_field_write(unit, regAddr, regField, &val)) != RT_ERR_OK)
        {
            TIME_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
            return ret;
        }

        TIME_SEM_UNLOCK(unit);
    }
    else
    {
        /*to be added*/
    }

    return RT_ERR_OK;
} /* end of dal_maple_time_portTxIntFlag_clear */


