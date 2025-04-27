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
#include <hal/chipdef/cypress/rtk_cypress_table_struct.h>
#include <hal/chipdef/cypress/rtk_cypress_reg_struct.h>
#include <hal/mac/reg.h>
#include <hal/mac/mem.h>
#include <hal/common/halctrl.h>
#include <dal/cypress/dal_cypress_time.h>
#include <dal/dal_mgmt.h>
#include <dal/dal_phy.h>
#include <rtk/default.h>
#include <rtk/time.h>

/*
 * Symbol Definition
 */


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
int32
dal_cypress_timeMapper_init(dal_mapper_t *pMapper)
{
    pMapper->time_init = dal_cypress_time_init;
    pMapper->time_portPtpEnable_get = dal_cypress_time_portPtpEnable_get;
    pMapper->time_portPtpEnable_set = dal_cypress_time_portPtpEnable_set;
    pMapper->time_portPtpRxTimestamp_get = dal_cypress_time_portPtpRxTimestamp_get;
    pMapper->time_portPtpTxTimestamp_get = dal_cypress_time_portPtpTxTimestamp_get;
    pMapper->time_portPtpTxTimestampCallback_register = dal_cypress_time_portPtpTxTimestampCallback_register;
    pMapper->time_portRefTime_get = dal_cypress_time_portRefTime_get;
    pMapper->time_portRefTime_set = dal_cypress_time_portRefTime_set;
    pMapper->time_portRefTimeAdjust_set = dal_cypress_time_portRefTimeAdjust_set;
    pMapper->time_portRefTimeEnable_get = dal_cypress_time_portRefTimeEnable_get;
    pMapper->time_portRefTimeEnable_set = dal_cypress_time_portRefTimeEnable_set;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_time_init
 * Description:
 *      Initialize ptp module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize ptp module before calling any ptp APIs.
 */
int32
dal_cypress_time_init(uint32 unit)
{
    int32 ret;
    uint32 val;
    rtk_enable_t enable;
    rtk_mac_t macAddr;
    rtk_time_timeStamp_t timeStamp;
    rtk_port_t baseMacId;
    uint32  phyIdx;
    rtk_portmask_t portmask;

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

    /* PTP Initialization */
    if ((ret = reg_field_read(unit, CYPRESS_MAC_ADDR_CTRLr, CYPRESS_SW_MAC_ADDR_31_0f, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }
    macAddr.octet[2] = (val >> 24) & 0xff;
    macAddr.octet[3] = (val >> 16) & 0xff;
    macAddr.octet[4] = (val >> 8) & 0xff;
    macAddr.octet[5] = (val >> 0) & 0xff;

    if ((ret = reg_field_read(unit, CYPRESS_MAC_ADDR_CTRLr, CYPRESS_SW_MAC_ADDR_47_32f, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
        return ret;
    }
    macAddr.octet[0] = (val >> 8) & 0xff;
    macAddr.octet[1] = (val >> 0) & 0xff;

    enable = DISABLED;
    osal_memset(&timeStamp, 0, sizeof(timeStamp));
    osal_memset(&portmask, 0, sizeof(portmask));

    /* Sync PTP configuration for all PHYs */
    HWP_PHY_TRAVS(unit, phyIdx)
    {
        baseMacId = HWP_PHY_BASE_MACID_BY_IDX(unit, phyIdx);
        if (!HWP_PORT_EXIST(unit, baseMacId))
        {
            continue;
        }

        ret = dal_phy_portSwitchMacAddr_set(unit, baseMacId, &macAddr);
        if ((ret != RT_ERR_OK) && (ret != RT_ERR_CHIP_NOT_SUPPORTED) && (ret != RT_ERR_PORT_NOT_SUPPORTED))
        {
            RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
            return ret;
        }

        RTK_PORTMASK_PORT_SET(portmask, baseMacId);

    }/* HWP_PHY_TRAVS */

    if ((ret = dal_cypress_time_portRefTimeEnable_set(unit, portmask, enable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
        return ret;
    }
    if ((ret = dal_cypress_time_portRefTime_set(unit, portmask, timeStamp, 1)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_cypress_time_init */

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
int32
dal_cypress_time_portPtpEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32 ret;
    uint32 reg_val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TIME), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(time_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(port > RTK_MAX_PORT_ID_PER_UNIT, RT_ERR_PORT_ID);

    TIME_SEM_LOCK(unit);

    if (HWP_PHY_EXIST(unit, port))
    {
        /* call API of PHY */
        ret = dal_phy_portPtpEnable_get(unit, port, pEnable);
        if ((ret != RT_ERR_OK) && (ret != RT_ERR_CHIP_NOT_SUPPORTED) && (ret != RT_ERR_PORT_NOT_SUPPORTED))
        {
            TIME_SEM_UNLOCK(unit);
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TIME), "dal_phy_portPtpEnable_get (ret = 0x%x) unit=%d, port=%d", ret, unit, port);
            return ret;
        }

        if (ret == RT_ERR_OK)
        {
            TIME_SEM_UNLOCK(unit);
            return RT_ERR_OK;
        }
    }

    if (HWP_10GE_PORT(unit, port))
    {
        if ((ret = reg_array_field_read(unit, CYPRESS_PTP_PORT_ENr, port, REG_ARRAY_INDEX_NONE, CYPRESS_ENf, &reg_val)) != RT_ERR_OK)
        {
            TIME_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
            return ret;
        }

        *pEnable = (reg_val) ? ENABLED : DISABLED;
    }
    else
    {
        ret = RT_ERR_CHIP_NOT_SUPPORTED;
    }

    TIME_SEM_UNLOCK(unit);

    return ret;
} /* end of dal_cypress_time_portPtpEnable_get */


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
int32
dal_cypress_time_portPtpEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32 ret;
    uint32 reg_val[2];

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TIME), "unit=%d, port=%d, enable=%d", unit, port, enable);

    /* check Init status */
    RT_INIT_CHK(time_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);
    RT_PARAM_CHK(port > RTK_MAX_PORT_ID_PER_UNIT, RT_ERR_PORT_ID);


    TIME_SEM_LOCK(unit);

    if (HWP_PHY_EXIST(unit, port))
    {
        /* call API of PHY */
        ret = dal_phy_portPtpEnable_set(unit, port, enable);
        if ((ret != RT_ERR_OK) && (ret != RT_ERR_CHIP_NOT_SUPPORTED) && (ret != RT_ERR_PORT_NOT_SUPPORTED))
        {
            TIME_SEM_UNLOCK(unit);
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TIME), "dal_phy_portPtpEnable_set (ret = 0x%x) unit=%d, port=%d", ret, unit, port);
            return ret;
        }

        if (ret == RT_ERR_OK)
        {
            TIME_SEM_UNLOCK(unit);
            return RT_ERR_OK;
        }
    }

    osal_memset(reg_val, 0, sizeof(reg_val));

    if (HWP_10GE_PORT(unit, port))
    {
        reg_val[0] = (ENABLED == enable) ? 1 : 0;

        if ((ret = reg_array_write(unit, CYPRESS_PTP_PORT_ENr, port, REG_ARRAY_INDEX_NONE, reg_val)) != RT_ERR_OK)
        {
            TIME_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
            return ret;
        }
    }
    else
    {
        ret = RT_ERR_CHIP_NOT_SUPPORTED;
    }

    TIME_SEM_UNLOCK(unit);

    return ret;
} /* end of dal_cypress_time_portPtpEnable_set */

/* Function Name:
 *      dal_cypress_time_portPtpRxTimestamp_get
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
int32
dal_cypress_time_portPtpRxTimestamp_get(
    uint32 unit,
    rtk_port_t port,
    rtk_time_ptpIdentifier_t identifier,
    rtk_time_timeStamp_t *pTimeStamp)
{
    int32 ret;
    uint32 data, field;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TIME), "unit=%d, port=%d, msgType=%d, sequenceId=0x%x",
            unit, port, identifier.msgType, identifier.sequenceId);

    /* check Init status */
    RT_INIT_CHK(time_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(port > RTK_MAX_PORT_ID_PER_UNIT, RT_ERR_PORT_ID);

    /* [FIXME] need to implement after ISR handler ready */
    /* lookup the rx timestamp table to find out the corresponding data */

    if (HWP_PHY_EXIST(unit, port))
    {
        TIME_SEM_LOCK(unit);

        /* call API of PHY */
        ret = dal_phy_portPtpRxTimestamp_get(unit, port, identifier, pTimeStamp);
        if ((ret != RT_ERR_OK) && (ret != RT_ERR_CHIP_NOT_SUPPORTED) && (ret != RT_ERR_PORT_NOT_SUPPORTED))
        {
            TIME_SEM_UNLOCK(unit);
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TIME), "dal_phy_portPtpRxTimestamp_get failure (ret = 0x%x) unit=%d, port=%d", ret, unit, port);
            return ret;
        }

        TIME_SEM_UNLOCK(unit);
        if (ret == RT_ERR_OK)
        {
            return RT_ERR_OK;
        }
    }

    /* [NOTE] Following implementation doesn't used timestamp table. */
    if (HWP_10GE_PORT(unit, port))
    {
        if (PTP_MSG_TYPE_SYNC == identifier.msgType)
            field = CYPRESS_SYNC_SEQ_IDf;
        else if (PTP_MSG_TYPE_DELAY_REQ == identifier.msgType)
            field = CYPRESS_DELAY_REQ_SEQ_IDf;
        else if (PTP_MSG_TYPE_PDELAY_REQ == identifier.msgType)
            field = CYPRESS_PDELAY_REQ_SEQ_IDf;
        else
            field = CYPRESS_PDELAY_RESP_SEQ_IDf;

        TIME_SEM_LOCK(unit);
        /* get sequence ID */
        if ((ret = reg_array_field_read(unit, CYPRESS_PTP_PORT_RX_TIMEr, port,
                        REG_ARRAY_INDEX_NONE, field, &data)) != RT_ERR_OK)
        {
            TIME_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
            return ret;
        }
        TIME_SEM_UNLOCK(unit);

        /* Input sequence ID NEED match currently sequence ID of chip */
        if (data != identifier.sequenceId)
        {
            return RT_ERR_INPUT;
        }

        TIME_SEM_LOCK(unit);
        /* Get value */
        if ((ret = reg_array_field_read(unit, CYPRESS_PTP_PORT_RX_TIMEr, port,
                        REG_ARRAY_INDEX_NONE, CYPRESS_SECf, &data)) != RT_ERR_OK)
        {
            TIME_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
            return ret;
        }
        pTimeStamp->sec = data;

        if ((ret = reg_array_field_read(unit, CYPRESS_PTP_PORT_RX_TIMEr, port,
                        REG_ARRAY_INDEX_NONE, CYPRESS_NSECf, &data)) != RT_ERR_OK)
        {
            TIME_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
            return ret;
        }
        pTimeStamp->nsec = data;
        TIME_SEM_UNLOCK(unit);
    }
    else
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    return RT_ERR_OK;
} /* end of dal_cypress_time_portPtpRxTimestamp_get */

/* Function Name:
 *      dal_cypress_time_portPtpTxTimestamp_get
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
int32
dal_cypress_time_portPtpTxTimestamp_get(
    uint32 unit,
    rtk_port_t port,
    rtk_time_ptpIdentifier_t identifier,
    rtk_time_timeStamp_t *pTimeStamp)
{
    int32 ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TIME), "unit=%d, port=%d, msgType=%d, sequenceId=0x%x",
            unit, port, identifier.msgType, identifier.sequenceId);

    /* check Init status */
    RT_INIT_CHK(time_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);

    TIME_SEM_LOCK(unit);

    /* call API of PHY */
    ret = dal_phy_portPtpTxTimestamp_get(unit, port, identifier, pTimeStamp);
    if (ret != RT_ERR_OK)
    {
        TIME_SEM_UNLOCK(unit);
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TIME), "dal_phy_portPtpTxTimestamp_get failure (ret = 0x%x) unit=%d, port=%d", ret, unit, port);
        return ret;
    }

    TIME_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_time_portPtpTxTimestamp_get */

/* Function Name:
 *      dal_cypress_time_portPtpTxTimestampCallback_register
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
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 * Note:
 *      None
 */
int32
dal_cypress_time_portPtpTxTimestampCallback_register(
    uint32 unit,
    rtk_port_t port,
    rtk_time_ptpIdentifier_t identifier,
    rtk_time_ptpTime_cb_f *fCb)
{
    /* Not support currently */

    return RT_ERR_CHIP_NOT_SUPPORTED;
} /* end of dal_cypress_time_portPtpTxTimestampCallback_register */

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
int32
dal_cypress_time_portRefTime_get(uint32 unit, rtk_port_t port, rtk_time_timeStamp_t *pTimeStamp)
{
    uint32 val;
    int32 ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TIME), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(time_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pTimeStamp), RT_ERR_NULL_POINTER);

    TIME_SEM_LOCK(unit);

    /* execute reading command */
    val = 0x0;  /* CMD = READ */
    if ((ret = reg_field_write(unit, CYPRESS_REF_TIME_CTRLr, CYPRESS_CMDf, &val)) != RT_ERR_OK)
    {
        TIME_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
        return ret;
    }
    val = 0x1;  /* EXEC */
    if ((ret = reg_field_write(unit, CYPRESS_REF_TIME_CTRLr, CYPRESS_EXECf, &val)) != RT_ERR_OK)
    {
        TIME_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
        return ret;
    }

    do {
        if ((ret = reg_field_read(unit, CYPRESS_REF_TIME_CTRLr, CYPRESS_EXECf, &val)) != RT_ERR_OK)
        {
            TIME_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
            return ret;
        }
    } while (val != 0);   /* busy watting */

    if ((ret = reg_field_read(unit, CYPRESS_REF_TIME_SECr, CYPRESS_SECf, &val)) != RT_ERR_OK)
    {
        TIME_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
        return ret;
    }
    pTimeStamp->sec = val;

    if ((ret = reg_field_read(unit, CYPRESS_REF_TIME_NSECr, CYPRESS_NSECf, &val)) != RT_ERR_OK)
    {
        TIME_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
        return ret;
    }
    pTimeStamp->nsec = val << 3; /* convert 8nsec to nsec */

    TIME_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_time_refTime_get */


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
int32
dal_cypress_time_portRefTime_set(uint32 unit, rtk_portmask_t portmask, rtk_time_timeStamp_t timeStamp, uint32 exec)
{
    int32 ret;
    uint32 val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TIME), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(time_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((timeStamp.nsec > RTK_TIME_NSEC_MAX), RT_ERR_INPUT);

    /* adjust timer for MAC and PHYs */
    TIME_SEM_LOCK(unit);

    /* adjust Timer for PHYs */
    ret = dal_phy_portRefTime_set(unit, portmask, timeStamp, 1);
    if ((ret != RT_ERR_OK) && (ret != RT_ERR_CHIP_NOT_SUPPORTED) && (ret != RT_ERR_PORT_NOT_SUPPORTED))
    {
        TIME_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
        return ret;
    }

    /* adjust Timer of MAC */
    val = timeStamp.sec;
    if ((ret = reg_field_write(unit, CYPRESS_REF_TIME_SECr, CYPRESS_SECf, &val)) != RT_ERR_OK)
    {
        TIME_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
        return ret;
    }
    val = timeStamp.nsec >> 3;   /* convert nsec to 8nsec */
    if ((ret = reg_field_write(unit, CYPRESS_REF_TIME_NSECr, CYPRESS_NSECf, &val)) != RT_ERR_OK)
    {
        TIME_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
        return ret;
    }

    /* execute writing command */
    val = 0x1;  /* CMD = WRITE */
    if ((ret = reg_field_write(unit, CYPRESS_REF_TIME_CTRLr, CYPRESS_CMDf, &val)) != RT_ERR_OK)
    {
        TIME_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
        return ret;
    }
    val = exec;  /* EXEC */
    if ((ret = reg_field_write(unit, CYPRESS_REF_TIME_CTRLr, CYPRESS_EXECf, &val)) != RT_ERR_OK)
    {
        TIME_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
        return ret;
    }
    do {
        if ((ret = reg_field_read(unit, CYPRESS_REF_TIME_CTRLr, CYPRESS_EXECf, &val)) != RT_ERR_OK)
        {
            TIME_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
            return ret;
        }
    } while (val != 0);   /* busy watting */

    TIME_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_time_refTime_set */


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
int32
dal_cypress_time_portRefTimeAdjust_set(uint32 unit, rtk_portmask_t portmask, uint32 sign, rtk_time_timeStamp_t timeStamp, uint32 exec)
{
    int32 ret;
    uint32 val;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TIME), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(time_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(sign >= 2, RT_ERR_INPUT);

    /* adjust timer for MAC and PHYs */
    TIME_SEM_LOCK(unit);

    /* adjust Timer for all PHYs */
    ret = dal_phy_portRefTimeAdjust_set(unit, portmask, sign, timeStamp, 1);
    if ((ret != RT_ERR_OK) && (ret != RT_ERR_CHIP_NOT_SUPPORTED) && (ret != RT_ERR_PORT_NOT_SUPPORTED))
    {
        TIME_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
        return ret;
    }

    /* adjust Timer of MAC */
    val = timeStamp.sec;
    if ((ret = reg_field_write(unit, CYPRESS_REF_TIME_SECr, CYPRESS_SECf, &val)) != RT_ERR_OK)
    {
        TIME_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
        return ret;
    }

    val = timeStamp.nsec >> 3;   /* convert nsec to 8nsec */
    if ((ret = reg_field_write(unit, CYPRESS_REF_TIME_NSECr, CYPRESS_NSECf, &val)) != RT_ERR_OK)
    {
        TIME_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
        return ret;
    }

    /* execute writing command */
    val = (sign == 0)? 0x2 : 0x3;   /* CMD = (2) Increase, (3) Decrease */
    if ((ret = reg_field_write(unit, CYPRESS_REF_TIME_CTRLr, CYPRESS_CMDf, &val)) != RT_ERR_OK)
    {
        TIME_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
        return ret;
    }
    val = exec;  /* EXEC */
    if ((ret = reg_field_write(unit, CYPRESS_REF_TIME_CTRLr, CYPRESS_EXECf, &val)) != RT_ERR_OK)
    {
        TIME_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
        return ret;
    }
    do {
        if ((ret = reg_field_read(unit, CYPRESS_REF_TIME_CTRLr, CYPRESS_EXECf, &val)) != RT_ERR_OK)
        {
            TIME_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
            return ret;
        }
    } while (val != 0);   /* busy watting */

    TIME_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_time_portRefTimeAdjust_set */

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
int32
dal_cypress_time_portRefTimeEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    uint32 en;
    int32 ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TIME), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(time_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(NULL == pEnable, RT_ERR_NULL_POINTER);

    TIME_SEM_LOCK(unit);

    if ((ret = reg_field_read(unit, CYPRESS_REF_TIME_ENr, CYPRESS_ENf, &en)) != RT_ERR_OK)
    {
        TIME_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
        return ret;
    }

    *pEnable = (en) ? ENABLED : DISABLED;

    TIME_SEM_UNLOCK(unit);
    return RT_ERR_OK;
} /* end of dal_cypress_time_portRefTimeEnable_get */

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
int32
dal_cypress_time_portRefTimeEnable_set(uint32 unit, rtk_portmask_t portmask, rtk_enable_t enable)
{
    int32 ret;
    uint32 en;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_TIME), "unit=%d, enable=%d", unit, enable);

    /* check Init status */
    RT_INIT_CHK(time_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    en = (ENABLED == enable)? 1 : 0;

    /* configure MAC */
    TIME_SEM_LOCK(unit);

    if ((ret = reg_field_write(unit, CYPRESS_REF_TIME_ENr, CYPRESS_ENf, &en)) != RT_ERR_OK)
    {
        TIME_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
        return ret;
    }

    TIME_SEM_UNLOCK(unit);


    /* configure PHY */
    TIME_SEM_LOCK(unit);

    ret = dal_phy_portRefTimeEnable_set(unit, portmask, enable);
    if ((ret != RT_ERR_OK) && (ret != RT_ERR_CHIP_NOT_SUPPORTED) && (ret != RT_ERR_PORT_NOT_SUPPORTED))
    {
        TIME_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_TIME), "");
        return ret;
    }

    TIME_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_time_portRefTimeEnable_set */


