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
 *           1) Configuration of ingress port bandwidth control (ingress rate limit ).
 *           2) Configuration of egress port bandwidth control (egress rate limit).
 *           3) Configuration of storm control
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
#include <osal/time.h>
#include <hal/chipdef/allmem.h>
#include <hal/chipdef/allreg.h>
#include <hal/chipdef/maple/rtk_maple_table_struct.h>
#include <hal/chipdef/maple/rtk_maple_reg_struct.h>
#include <hal/mac/reg.h>
#include <hal/mac/mem.h>
#include <hal/common/halctrl.h>
#include <dal/maple/dal_maple_rate.h>
#include <dal/maple/dal_maple_port.h>
#include <dal/maple/dal_maple_qos.h>
#include <dal/dal_mgmt.h>
#include <rtk/default.h>
#include <rtk/rate.h>

/*
 * Symbol Definition
 */

typedef struct dal_maple_rate_egr_bandwidth_ctrl_s
{
    uint32  status;
    uint32  rate;
} dal_maple_rate_egr_bandwidth_ctrl_t;

typedef enum dal_maple_rate_igr_bandwidth_ctrl_act_e
{
    IGR_ACT_RCV = 0,
    IGR_ACT_DROP,
    IGR_ACT_END
} dal_maple_rate_igr_bandwidth_ctrl_act_t;

/*
 * Data Declaration
 */
static uint32               rate_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};
static osal_mutex_t         rate_sem[RTK_MAX_NUM_OF_UNIT];


static uint32               egrPortRate[RTK_MAX_NUM_OF_UNIT][29];
static rtk_enable_t      egrPortRateEnable[RTK_MAX_NUM_OF_UNIT][29];

/*
 * Macro Definition
 */
/* rate semaphore handling */
#define RATE_SEM_LOCK(unit)    \
do {\
    if (osal_sem_mutex_take(rate_sem[unit], OSAL_SEM_WAIT_FOREVER) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_LOCK_FAILED, (MOD_DAL|MOD_RATE), "semaphore lock failed");\
        return RT_ERR_SEM_LOCK_FAILED;\
    }\
} while(0)
#define RATE_SEM_UNLOCK(unit)   \
do {\
    if (osal_sem_mutex_give(rate_sem[unit]) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_UNLOCK_FAILED, (MOD_DAL|MOD_RATE), "semaphore unlock failed");\
        return RT_ERR_SEM_UNLOCK_FAILED;\
    }\
} while(0)

/*
 * Function Declaration
 */
//static int32 _dal_maple_rate_init_config(uint32 unit);
//static int32 _dal_maple_rate_igrBandwidthCtrlFCCnt_set(uint32 unit, rtk_port_t port, uint32 fccnt);
//static int32 _dal_maple_rate_igrBandwidthCtrlAct_set(uint32 unit, rtk_port_t port, dal_maple_rate_igr_bandwidth_ctrl_act_t act);

int32 dal_maple_rate_portScheChange_set(uint32 unit, rtk_port_t port, rtk_qos_scheduling_type_t newSche)
{
    int32 ret;
    uint32 value;

    RATE_SEM_LOCK(unit);

    if(newSche==WFQ)
    {
        value=1;
        if ((ret = reg_array_field_write(unit, MAPLE_SCHED_LB_CTRLr, port, REG_ARRAY_INDEX_NONE, MAPLE_P_EGR_LB_ENf, &value)) != RT_ERR_OK)
        {
            RATE_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }
    }

    RATE_SEM_UNLOCK(unit);

    if ((ret = dal_maple_rate_portEgrBwCtrlEnable_set(unit, port, egrPortRateEnable[unit][port])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }
    if ((ret = dal_maple_rate_portEgrBwCtrlRate_set(unit, port, egrPortRate[unit][port])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    return RT_ERR_OK;
}



/* Function Name:
 *      dal_maple_rateMapper_init
 * Description:
 *      Hook rate module of the specified device.
 * Input:
 *      pMapper - pointer of mapper
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      Must Hook rate module before calling any rate APIs.
 */
int32
dal_maple_rateMapper_init(dal_mapper_t *pMapper)
{

    pMapper->rate_init = dal_maple_rate_init;
    pMapper->rate_includeIfg_get = dal_maple_rate_includeIfg_get;
    pMapper->rate_includeIfg_set = dal_maple_rate_includeIfg_set;
    pMapper->rate_portIgrBwCtrlEnable_get = dal_maple_rate_portIgrBwCtrlEnable_get;
    pMapper->rate_portIgrBwCtrlEnable_set = dal_maple_rate_portIgrBwCtrlEnable_set;
    pMapper->rate_portIgrBwCtrlRate_get = dal_maple_rate_portIgrBwCtrlRate_get;
    pMapper->rate_portIgrBwCtrlRate_set = dal_maple_rate_portIgrBwCtrlRate_set;
    pMapper->rate_igrPortBwCtrlBurstSize_get = dal_maple_rate_igrPortBwCtrlBurstSize_get;
    pMapper->rate_igrPortBwCtrlBurstSize_set = dal_maple_rate_igrPortBwCtrlBurstSize_set;
    pMapper->rate_portIgrBandwidthCtrlExceed_get = dal_maple_rate_portIgrBandwidthCtrlExceed_get;
    pMapper->rate_portIgrBandwidthCtrlExceed_reset = dal_maple_rate_portIgrBandwidthCtrlExceed_reset;
    pMapper->rate_igrBandwidthCtrlBypass_get = dal_maple_rate_igrBandwidthCtrlBypass_get;
    pMapper->rate_igrBandwidthCtrlBypass_set = dal_maple_rate_igrBandwidthCtrlBypass_set;
    pMapper->rate_portIgrQueueBwCtrlEnable_get = dal_maple_rate_portIgrQueueBwCtrlEnable_get;
    pMapper->rate_portIgrQueueBwCtrlEnable_set = dal_maple_rate_portIgrQueueBwCtrlEnable_set;
    pMapper->rate_portIgrQueueBwCtrlRate_get = dal_maple_rate_portIgrQueueBwCtrlRate_get;
    pMapper->rate_portIgrQueueBwCtrlRate_set = dal_maple_rate_portIgrQueueBwCtrlRate_set;
    pMapper->rate_igrQueueBwCtrlBurstSize_get = dal_maple_rate_igrQueueBwCtrlBurstSize_get;
    pMapper->rate_igrQueueBwCtrlBurstSize_set = dal_maple_rate_igrQueueBwCtrlBurstSize_set;
    pMapper->rate_portIgrQueueBwCtrlExceed_get = dal_maple_rate_portIgrQueueBwCtrlExceed_get;
    pMapper->rate_portIgrQueueBwCtrlExceed_reset = dal_maple_rate_portIgrQueueBwCtrlExceed_reset;
    pMapper->rate_portEgrBwCtrlEnable_get = dal_maple_rate_portEgrBwCtrlEnable_get;
    pMapper->rate_portEgrBwCtrlEnable_set = dal_maple_rate_portEgrBwCtrlEnable_set;
    pMapper->rate_portEgrBwCtrlRate_get = dal_maple_rate_portEgrBwCtrlRate_get;
    pMapper->rate_portEgrBwCtrlRate_set = dal_maple_rate_portEgrBwCtrlRate_set;
    pMapper->rate_cpuEgrBandwidthCtrlRateMode_get = dal_maple_rate_cpuEgrBandwidthCtrlRateMode_get;
    pMapper->rate_cpuEgrBandwidthCtrlRateMode_set = dal_maple_rate_cpuEgrBandwidthCtrlRateMode_set;
    pMapper->rate_egrPortBwCtrlBurstSize_get = dal_maple_rate_egrPortBwCtrlBurstSize_get;
    pMapper->rate_egrPortBwCtrlBurstSize_set = dal_maple_rate_egrPortBwCtrlBurstSize_set;
    pMapper->rate_portEgrQueueBwCtrlEnable_get = dal_maple_rate_portEgrQueueBwCtrlEnable_get;
    pMapper->rate_portEgrQueueBwCtrlEnable_set = dal_maple_rate_portEgrQueueBwCtrlEnable_set;
    pMapper->rate_portEgrQueueBwCtrlRate_get = dal_maple_rate_portEgrQueueBwCtrlRate_get;
    pMapper->rate_portEgrQueueBwCtrlRate_set = dal_maple_rate_portEgrQueueBwCtrlRate_set;
    pMapper->rate_egrQueueFixedBandwidthEnable_get = dal_maple_rate_egrQueueFixedBandwidthEnable_get;
    pMapper->rate_egrQueueFixedBandwidthEnable_set = dal_maple_rate_egrQueueFixedBandwidthEnable_set;
    pMapper->rate_egrQueueBwCtrlBurstSize_get = dal_maple_rate_egrQueueBwCtrlBurstSize_get;
    pMapper->rate_egrQueueBwCtrlBurstSize_set = dal_maple_rate_egrQueueBwCtrlBurstSize_set;
    pMapper->rate_portStormCtrlEnable_get = dal_maple_rate_portStormCtrlEnable_get;
    pMapper->rate_portStormCtrlEnable_set = dal_maple_rate_portStormCtrlEnable_set;
    pMapper->rate_portStormCtrlRate_get = dal_maple_rate_portStormCtrlRate_get;
    pMapper->rate_portStormCtrlRate_set = dal_maple_rate_portStormCtrlRate_set;
    pMapper->rate_stormControlBurstSize_get = dal_maple_rate_stormControlBurstSize_get;
    pMapper->rate_stormControlBurstSize_set = dal_maple_rate_stormControlBurstSize_set;
    pMapper->rate_portStormCtrlExceed_get = dal_maple_rate_portStormCtrlExceed_get;
    pMapper->rate_portStormCtrlExceed_reset = dal_maple_rate_portStormCtrlExceed_reset;
    pMapper->rate_stormControlRateMode_get = dal_maple_rate_stormControlRateMode_get;
    pMapper->rate_stormControlRateMode_set = dal_maple_rate_stormControlRateMode_set;
    pMapper->rate_portStormCtrlTypeSel_get = dal_maple_rate_portStormCtrlTypeSel_get;
    pMapper->rate_portStormCtrlTypeSel_set = dal_maple_rate_portStormCtrlTypeSel_set;
    pMapper->rate_stormControlBypass_get = dal_maple_rate_stormControlBypass_get;
    pMapper->rate_stormControlBypass_set = dal_maple_rate_stormControlBypass_set;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_maple_rate_init
 * Description:
 *      Initial the rate module of the specified device..
 * Input:
 *      unit - unit id
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
int32
dal_maple_rate_init(uint32 unit)
{
    int32 ret;
    uint32 burstSize = 0x8000;
    uint32 val;
    uint32 port;

    RT_INIT_REENTRY_CHK(rate_init[unit]);
    rate_init[unit] = INIT_NOT_COMPLETED;

    /* create semaphore */
    rate_sem[unit] = osal_sem_mutex_create();
    if (0 == rate_sem[unit])
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_RATE), "semaphore create failed");
        return RT_ERR_FAILED;
    }

    /* set init flag to complete init */
    rate_init[unit] = INIT_COMPLETED;

    for(port=0; port<29; port++)
    {
        egrPortRate[unit][port] = 0x3ffff;
        egrPortRateEnable[unit][port] = DISABLED;
    }

    RATE_SEM_LOCK(unit);

    /*E0014008 */
    if((HWP_CHIP_ID(unit) ==  RTL8381M_CHIP_ID) || \
        (HWP_CHIP_ID(unit) ==  RTL8380M_CHIP_ID))
    {
        val = 434;
        if ((ret = reg_field_write(unit, MAPLE_SCHED_LB_TICK_TKN_CTRLr, MAPLE_TICK_PERIOD_PPSf, &val)) != RT_ERR_OK)
        {
            RATE_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }
    }

    /* write value from CHIP*/
    burstSize = 0x8000;
    if ((ret = reg_field_write(unit, MAPLE_STORM_CTRL_BURSTr, MAPLE_UCf, &burstSize)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }
    if ((ret = reg_field_write(unit, MAPLE_STORM_CTRL_BURSTr, MAPLE_MCf, &burstSize)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }
    if ((ret = reg_field_write(unit, MAPLE_STORM_CTRL_BURSTr, MAPLE_BCf, &burstSize)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    /* write value from CHIP*/
    burstSize = 32;
    if ((ret = reg_field_write(unit, MAPLE_STORM_CTRL_BURSTr, MAPLE_UC_BURST_PPSf, &burstSize)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }
    if ((ret = reg_field_write(unit, MAPLE_STORM_CTRL_BURSTr, MAPLE_MC_BURST_PPSf, &burstSize)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }
    if ((ret = reg_field_write(unit, MAPLE_STORM_CTRL_BURSTr, MAPLE_BC_BURST_PPSf, &burstSize)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    if ((ret = dal_maple_rate_stormControlIncludeIfg_set(unit, ENABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    if ((ret = dal_maple_rate_stormControlRateMode_set(unit, BASED_ON_BYTE)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    if ((ret = dal_maple_rate_bandwidthCtrlIncludeIfg_set(unit, ENABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    if ((ret = dal_maple_rate_cpuEgrBandwidthCtrlRateMode_set(unit, RATE_MODE_PKT)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    if(RTL8382M_CHIP_ID == HWP_CHIP_ID(unit))
    {

    }
    else
    {
        if ((ret = dal_maple_rate_egrPortBwCtrlBurstSize_set(unit, 0x800)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }
    }

    return RT_ERR_OK;
} /* end of dal_maple_rate_init */

/* Function Name:
 *      dal_maple_rate_portIgrBwCtrlEnable_get
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
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The status of ingress bandwidth control is as following:
 *      - DISABLED
 *      - ENABLED
 */
int32
dal_maple_rate_portIgrBwCtrlEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d\n", unit, port);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_array_field_read(unit, MAPLE_IGR_BWCTRL_LB_CTRLr, port, REG_ARRAY_INDEX_NONE, MAPLE_P_LB_ENf, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    if(value == 1)
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
} /* end of dal_maple_rate_portIgrBwCtrlEnable_get */

/* Function Name:
 *      dal_maple_rate_portIgrBwCtrlEnable_set
 * Description:
 *      Set the ingress bandwidth control status.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - status of ingress bandwidth control
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - Invalid port id
 *      RT_ERR_INPUT    - Invalid input parameter
 * Note:
 *      The status of ingress bandwidth control is as following:
 *      - DISABLED
 *      - ENABLED
 */
int32
dal_maple_rate_portIgrBwCtrlEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, enable=%d\n", unit, port, enable);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    if(enable == ENABLED)
        value = 1;
    else
        value = 0;

    RATE_SEM_LOCK(unit);

    /* write value to CHIP*/
    if ((ret = reg_array_field_write(unit, MAPLE_IGR_BWCTRL_LB_CTRLr, port, REG_ARRAY_INDEX_NONE, MAPLE_P_LB_ENf, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_rate_portIgrBwCtrlEnable_set */

/* Function Name:
 *      dal_maple_rate_portIgrBwCtrlRate_get
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
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The actual rate is "rate * chip granularity".
 *      The unit of granularity in RTL8380 is 16Kbps.
 */
int32
dal_maple_rate_portIgrBwCtrlRate_get(uint32 unit, rtk_port_t port, uint32 *pRate)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d\n", unit, port);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pRate), RT_ERR_NULL_POINTER);

    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_array_field_read(unit, MAPLE_IGR_BWCTRL_PORT_RATE_CTRLr, port, REG_ARRAY_INDEX_NONE, MAPLE_P_RATEf, pRate)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "pRate=%d", *pRate);

    return RT_ERR_OK;
} /* end of dal_maple_rate_portIgrBwCtrlRate_get */

/* Function Name:
 *      dal_maple_rate_portIgrBwCtrlRate_set
 * Description:
 *      Set the ingress bandwidth control rate.
 * Input:
 *      unit - unit id
 *      port - port id
 *      rate - ingress bandwidth control rate
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - Invalid port id
 *      RT_ERR_RATE     - Invalid input rate
 * Note:
 *      The actual rate is "rate * chip granularity".
 *      The unit of granularity in RTL8380 is 16Kbps.
 */
int32
dal_maple_rate_portIgrBwCtrlRate_set(uint32 unit, rtk_port_t port, uint32 rate)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, rate=%d\n", unit, port, rate);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((rate > HAL_RATE_OF_BANDWIDTH_MAX(unit)), RT_ERR_RATE);

    RATE_SEM_LOCK(unit);

    /* write value to CHIP*/
    if ((ret = reg_array_field_write(unit, MAPLE_IGR_BWCTRL_PORT_RATE_CTRLr, port, REG_ARRAY_INDEX_NONE, MAPLE_P_RATEf, &rate)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_rate_portIgrBwCtrlRate_set */

/* Function Name:
 *      dal_maple_rate_bandwidthCtrlIncludeIfg_get
 * Description:
 *      Get the status of  bandwidth control includes IFG or not.
 * Input:
 *      unit         - unit id
 * Output:
 *      pIfg_include - include IFG or not
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      1) bandwidth control includes/excludes the Preamble & IFG (20 Bytes).
 *      2) The status of ifg_include:
 *         - DISABLED
 *         - ENABLED
 */
int32
dal_maple_rate_bandwidthCtrlIncludeIfg_get(uint32 unit, rtk_enable_t *pIfg_include)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIfg_include), RT_ERR_NULL_POINTER);

    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_field_read(unit, MAPLE_SCHED_CTRLr, MAPLE_INC_IFGf, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    if (value == 1)
        *pIfg_include = ENABLED;
    else
        *pIfg_include = DISABLED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "pIfg_include=%d", *pIfg_include);

    return RT_ERR_OK;
} /* end of dal_maple_rate_bandwidthCtrlIncludeIfg_get */

/* Function Name:
 *      dal_maple_rate_bandwidthCtrlIncludeIfg_set
 * Description:
 *      Set the status of bandwidth control includes IFG or not.
 * Input:
 *      unit        - unit id
 *      ifg_include - include IFG or not
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - Invalid input parameter
 * Note:
 *      1) bandwidth control includes/excludes the Preamble & IFG (20 Bytes).
 *      2) The status of ifg_include:
 *         - DISABLED
 *         - ENABLED
 */
int32
dal_maple_rate_bandwidthCtrlIncludeIfg_set(uint32 unit, rtk_enable_t ifg_include)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, enable=%d\n", unit, ifg_include);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((ifg_include >= RTK_ENABLE_END), RT_ERR_INPUT);

    if (ifg_include == ENABLED)
        value = 1;
    else
        value = 0;

    RATE_SEM_LOCK(unit);

    /* write value to CHIP*/
    if ((ret = reg_field_write(unit, MAPLE_SCHED_CTRLr, MAPLE_INC_IFGf, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_rate_igrBandwidthCtrlIncludeIfg_set */

/* Function Name:
 *      dal_maple_rate_igrBandwidthCtrlBypass_get
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
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None.
 */
int32
dal_maple_rate_igrBandwidthCtrlBypass_get(uint32 unit, rtk_rate_igr_bypassType_t bypassType, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  value;
    uint32  regField = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((bypassType >= IGR_BYPASS_END), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    switch (bypassType)
    {
        case BYPASS_TYPE_ARP:
            regField = MAPLE_ARPREQ_ADMITf;
            break;
        case BYPASS_TYPE_RMA:
            regField = MAPLE_RMA_ADMITf;
            break;
        case BYPASS_TYPE_BPDU:
            regField = MAPLE_BPDU_ADMITf;
            break;
        case BYPASS_TYPE_RTKPKT:
            regField = MAPLE_RTKPKT_ADMITf;
            break;
        case BYPASS_TYPE_IGMP:
            regField = MAPLE_IGMP_ADMITf;
            break;
        default:
            return RT_ERR_INPUT;
    }

    RATE_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, MAPLE_IGR_BWCTRL_CTRLr, REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, regField, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    /* translate chip's value to definition */
    if (1 == value)
    {
        *pEnable = ENABLED;
    }
    else
    {
        *pEnable = DISABLED;
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
} /* end of dal_maple_rate_igrBandwidthCtrlBypass_get */

/* Function Name:
 *      dal_maple_rate_igrBandwidthCtrlBypass_set
 * Description:
 *      Set the status of bypass ingress bandwidth control for specified packet type.
 * Input:
 *      unit       - unit id
 *      byasssType - bypass type
 *      enable     - status of bypass ingress bandwidth control
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - Invalid input parameter
 * Note:
 *      None.
 */
int32
dal_maple_rate_igrBandwidthCtrlBypass_set(uint32 unit, rtk_rate_igr_bypassType_t bypassType, rtk_enable_t enable)
{
    int32   ret;
    uint32  value;
    uint32  regField = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_QOS), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((bypassType >= IGR_BYPASS_END), RT_ERR_INPUT);
    RT_PARAM_CHK((ENABLED != enable && DISABLED != enable), RT_ERR_INPUT);

    switch (bypassType)
    {
        case BYPASS_TYPE_ARP:
            regField = MAPLE_ARPREQ_ADMITf;
            break;
        case BYPASS_TYPE_RMA:
            regField = MAPLE_RMA_ADMITf;
            break;
        case BYPASS_TYPE_BPDU:
            regField = MAPLE_BPDU_ADMITf;
            break;
        case BYPASS_TYPE_RTKPKT:
            regField = MAPLE_RTKPKT_ADMITf;
            break;
        case BYPASS_TYPE_IGMP:
            regField = MAPLE_IGMP_ADMITf;
            break;
        default:
            return RT_ERR_INPUT;
    }

    /* translate definition to chip's value  */
    if (ENABLED == enable)
    {
        value = 1;
    }
    else
    {
        value = 0;
    }

    RATE_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_write(unit, MAPLE_IGR_BWCTRL_CTRLr, REG_ARRAY_INDEX_NONE, REG_ARRAY_INDEX_NONE, regField, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
        return ret;
    }
    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_rate_igrBandwidthCtrlBypass_set */

/* Function Name:
 *      dal_maple_rate_portEgrBwCtrlEnable_get
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
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The status of egress bandwidth control is as following:
 *      - DISABLED
 *      - ENABLED
 */
int32
dal_maple_rate_portEgrBwCtrlEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d\n", unit, port);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    *pEnable = egrPortRateEnable[unit][port];

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
} /* end of dal_maple_rate_portEgrBwCtrlEnable_get */

/* Function Name:
 *      dal_maple_rate_portEgrBwCtrlEnable_set
 * Description:
 *      Set the egress bandwidth control status.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - status of egress bandwidth control
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - Invalid port id
 *      RT_ERR_INPUT    - Invalid input parameter
 * Note:
 *      The status of egress bandwidth control is as following:
 *      - DISABLED
 *      - ENABLED
 */
int32
dal_maple_rate_portEgrBwCtrlEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
    uint32  value= 0x3ffff;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, enable=%d\n", unit, port, enable);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    RATE_SEM_LOCK(unit);

    if(portSche_type[unit][port]==WRR)
    {
        value = enable==ENABLED ? 1 : 0;

        /* program value to CHIP*/
        if ((ret = reg_array_field_write(unit, MAPLE_SCHED_LB_CTRLr, port, REG_ARRAY_INDEX_NONE, MAPLE_P_EGR_LB_ENf, &value)) != RT_ERR_OK)
        {
            RATE_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }
    }

    value = enable==ENABLED ? egrPortRate[unit][port] : 0x3ffff;

    if ((ret = reg_array_field_write(unit, MAPLE_SCHED_P_EGR_RATE_CTRLr, port, REG_ARRAY_INDEX_NONE, MAPLE_P_EGR_RATEf, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    egrPortRateEnable[unit][port] = enable;

    return RT_ERR_OK;
} /* end of dal_maple_rate_portEgrBwCtrlEnable_set */

/* Function Name:
 *      dal_maple_rate_portEgrBwCtrlRate_get
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
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *    The actual rate is "rate * chip granularity".
 *    The unit of granularity in RTL8380 is 16Kbps.
 */
int32
dal_maple_rate_portEgrBwCtrlRate_get(uint32 unit, rtk_port_t port, uint32 *pRate)
{
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d\n", unit, port);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pRate), RT_ERR_NULL_POINTER);

    *pRate = egrPortRate[unit][port];

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "pRate=%d", *pRate);

    return RT_ERR_OK;
} /* end of dal_maple_rate_portEgrBwCtrlRate_get */

/* Function Name:
 *      dal_maple_rate_portEgrBwCtrlRate_set
 * Description:
 *      Set the egress bandwidth control rate.
 * Input:
 *      unit - unit id
 *      port - port id
 *      rate - egress bandwidth control rate
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - Invalid port id
 *      RT_ERR_RATE     - Invalid input rate
 * Note:
 *      The actual rate is "rate * chip granularity".
 *      The unit of granularity in RTL8380 is 16Kbps.
 */
int32
dal_maple_rate_portEgrBwCtrlRate_set(uint32 unit, rtk_port_t port, uint32 rate)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, rate=%d\n", unit, port, rate);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((rate > HAL_GET_MAX_BANDWIDTH_OF_PORT(unit, port) && rate!=HAL_RATE_OF_BANDWIDTH_MAX(unit)), RT_ERR_RATE);

    egrPortRate[unit][port] = rate;

    if(egrPortRateEnable[unit][port] == ENABLED)
    {
        RATE_SEM_LOCK(unit);

        /* read value from CHIP*/
        if ((ret = reg_array_field_write(unit, MAPLE_SCHED_P_EGR_RATE_CTRLr, port, REG_ARRAY_INDEX_NONE, MAPLE_P_EGR_RATEf, &egrPortRate[unit][port])) != RT_ERR_OK)
        {
            RATE_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }

        RATE_SEM_UNLOCK(unit);
    }

    return RT_ERR_OK;
} /* end of dal_maple_rate_portEgrBwCtrlRate_set */

/* Function Name:
 *      dal_maple_rate_portEgrQueueBwCtrlEnable_get
 * Description:
 *      Get enable status of egress bandwidth control on specified queue.
 * Input:
 *      unit  - unit id
 *      port  - port id
 *      queue - queue id
 * Output:
 *      pEnable - Pointer to enable status of egress queue bandwidth control
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_QUEUE_ID     - invalid queue id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_maple_rate_portEgrQueueBwCtrlEnable_get(
    uint32          unit,
    rtk_port_t      port,
    rtk_qid_t       queue,
    rtk_enable_t    *pEnable)
{
    int32   ret;
    uint32  value;
    uint32  regField;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, queue=%d\n", unit, port, queue);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((queue >= HAL_MAX_NUM_OF_QUEUE(unit)), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    switch (queue)
    {
        case 0:
            regField = MAPLE_Q0_EGR_LB_APR_ENf;
            break;
        case 1:
            regField = MAPLE_Q1_EGR_LB_APR_ENf;
            break;
        case 2:
            regField = MAPLE_Q2_EGR_LB_APR_ENf;
            break;
        case 3:
            regField = MAPLE_Q3_EGR_LB_APR_ENf;
            break;
        case 4:
            regField = MAPLE_Q4_EGR_LB_APR_ENf;
            break;
        case 5:
            regField = MAPLE_Q5_EGR_LB_APR_ENf;
            break;
        case 6:
            regField = MAPLE_Q6_EGR_LB_APR_ENf;
            break;
        case 7:
            regField = MAPLE_Q7_EGR_LB_APR_ENf;
            break;
        default:
            return RT_ERR_QUEUE_ID;
    }

    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_array_field_read(unit, MAPLE_SCHED_LB_CTRLr, port, REG_ARRAY_INDEX_NONE, regField, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    if (value == 1)
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
} /* end of dal_maple_rate_portportEgrQueueBwCtrlEnable_get */

/* Function Name:
 *      dal_maple_rate_portEgrQueueBwCtrlEnable_set
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
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_QUEUE_ID - invalid queue id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
dal_maple_rate_portEgrQueueBwCtrlEnable_set(
    uint32          unit,
    rtk_port_t      port,
    rtk_qid_t       queue,
    rtk_enable_t    enable)
{
    int32   ret;
    uint32  value;
    uint32  regField;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, queue=%d, enable=%d\n", unit, port, queue, enable);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((queue >= HAL_MAX_NUM_OF_QUEUE(unit)), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    if (enable == ENABLED)
        value = 1;
    else
        value = 0;

    switch(queue)
    {
        case 0:
            regField = MAPLE_Q0_EGR_LB_APR_ENf;
            break;
        case 1:
            regField = MAPLE_Q1_EGR_LB_APR_ENf;
            break;
        case 2:
            regField = MAPLE_Q2_EGR_LB_APR_ENf;
            break;
        case 3:
            regField = MAPLE_Q3_EGR_LB_APR_ENf;
            break;
        case 4:
            regField = MAPLE_Q4_EGR_LB_APR_ENf;
            break;
        case 5:
            regField = MAPLE_Q5_EGR_LB_APR_ENf;
            break;
        case 6:
            regField = MAPLE_Q6_EGR_LB_APR_ENf;
            break;
        case 7:
            regField = MAPLE_Q7_EGR_LB_APR_ENf;
            break;
        default:
            return RT_ERR_QUEUE_ID;
    }

    RATE_SEM_LOCK(unit);

    /* write value to CHIP*/
    if ((ret = reg_array_field_write(unit, MAPLE_SCHED_LB_CTRLr, port, REG_ARRAY_INDEX_NONE, regField, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_rate_portportEgrQueueBwCtrlEnable_set */

/* Function Name:
 *      dal_maple_rate_portEgrQueueBwCtrlRate_get
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
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_QUEUE_ID     - invalid queue id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The actual rate is "rate * chip granularity".
 *      The unit of granularity is 16Kbps.
 */
int32
dal_maple_rate_portEgrQueueBwCtrlRate_get(
    uint32      unit,
    rtk_port_t  port,
    rtk_qid_t   queue,
    uint32      *pRate)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, queue=%d\n", unit, port, queue);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((queue >= HAL_MAX_NUM_OF_QUEUE(unit)), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((NULL == pRate), RT_ERR_NULL_POINTER);

    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_array_field_read(unit, MAPLE_SCHED_Q_EGR_RATE_CTRLr, port, queue, MAPLE_Q_EGR_RATEf, pRate)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "pRate=%d", *pRate);

    return RT_ERR_OK;
} /* end of dal_maple_rate_portEgrQueueBwCtrlRate_get */

/* Function Name:
 *      dal_maple_rate_portEgrQueueBwCtrlRate_set
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
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_QUEUE_ID - invalid queue id
 *      RT_ERR_RATE     - Invalid input rate
 * Note:
 *      The actual rate is "rate * chip granularity".
 *      The unit of granularity is 16Kbps.
 */
int32
dal_maple_rate_portEgrQueueBwCtrlRate_set(
    uint32      unit,
    rtk_port_t  port,
    rtk_qid_t   queue,
    uint32      rate)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, queue=%d, rate=%d\n", unit, port, queue, rate);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((queue >= HAL_MAX_NUM_OF_QUEUE(unit)), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((rate > HAL_GET_MAX_BANDWIDTH_OF_PORT(unit, port)), RT_ERR_RATE);

    RATE_SEM_LOCK(unit);

    /* write value to CHIP*/
    if ((ret = reg_array_field_write(unit, MAPLE_SCHED_Q_EGR_RATE_CTRLr, port, queue, MAPLE_Q_EGR_RATEf, &rate)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_rate_portEgrQueueBwCtrlRate_set */

/* Function Name:
 *      dal_maple_rate_egrQueueFixedBandwidthEnable_get
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
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_QUEUE_ID     - invalid queue id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_maple_rate_egrQueueFixedBandwidthEnable_get(
    uint32          unit,
    rtk_port_t      port,
    rtk_qid_t       queue,
    rtk_enable_t    *pEnable)
{
    int32   ret;
    uint32  value;;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, queue=%d", unit, port, queue);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((queue >= HAL_MAX_NUM_OF_QUEUE(unit)), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_array_field_read(unit, MAPLE_SCHED_Q_CTRLr, port, queue, MAPLE_Q_BORW_TKNf, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    if (value == 0)
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
} /* end of dal_maple_rate_egrQueueFixedBandwidthEnable_get */

/* Function Name:
 *      dal_maple_rate_egrQueueFixedBandwidthEnable_set
 * Description:
 *      Set enable status of fixed bandwidth ability on specified queue.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      queue  - queue id
 *      enable - enable status of fixed bandwidth ability
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - Invalid port id
 *      RT_ERR_QUEUE_ID - invalid queue id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
dal_maple_rate_egrQueueFixedBandwidthEnable_set(
    uint32          unit,
    rtk_port_t      port,
    rtk_qid_t       queue,
    rtk_enable_t    enable)
{
    int32   ret;
    uint32  value;;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, queue=%d, enable=%d", unit, port, queue, enable);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((queue >= HAL_MAX_NUM_OF_QUEUE(unit)), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    if (enable == ENABLED)
        value = 0;
    else
        value = 1;

    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_array_field_write(unit, MAPLE_SCHED_Q_CTRLr, port, queue, MAPLE_Q_BORW_TKNf, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_rate_egrQueueFixedBandwidthEnable_set */

/* Function Name:
 *      dal_maple_rate_portStormCtrlEnable_get
 * Description:
 *      Get the storm control ability.
 * Input:
 *      unit       - unit id
 *      port       - port id
 *      storm_type - storm group type
 * Output:
 *      pEnable    - storm control ability
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_INPUT        - Invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The storm group types are as following:
 *      - STORM_GROUP_MULTICAST
 *      - STORM_GROUP_BROADCAST
 *      - STORM_GROUP_UNICAST
 */
int32
dal_maple_rate_portStormCtrlEnable_get(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_group_t  storm_type,
    rtk_enable_t            *pEnable)
{
    int32   ret;
    uint32  reg_idx;
    uint32  field_idx;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, storm_type=%d",
           unit, port, storm_type);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    switch (storm_type)
    {
        case STORM_GROUP_UNICAST:
            reg_idx = MAPLE_STORM_CTRL_LB_CTRLr;
            field_idx = MAPLE_UC_LB_ENf;
            break;
        case STORM_GROUP_MULTICAST:
            reg_idx = MAPLE_STORM_CTRL_LB_CTRLr;
            field_idx = MAPLE_MC_LB_ENf;
            break;
        case STORM_GROUP_BROADCAST:
            reg_idx = MAPLE_STORM_CTRL_LB_CTRLr;
            field_idx = MAPLE_BC_LB_ENf;
            break;
        default:
            return RT_ERR_INPUT;
    }

    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_array_field_read(unit, reg_idx, port, REG_ARRAY_INDEX_NONE, field_idx, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    /* chip's value translate */
    switch (value)
    {
        case 0:
            *pEnable = DISABLED;
            break;
        case 1:
            *pEnable = ENABLED;
            break;
        default:
            return RT_ERR_FAILED;
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
} /* end of dal_maple_rate_portStormCtrlEnable_get */

/* Function Name:
 *      dal_maple_rate_portStormCtrlEnable_set
 * Description:
 *      Set the storm control ability.
 * Input:
 *      unit       - unit id
 *      port       - port id
 *      storm_type - storm group type
 *      enable     - storm control ability
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - Invalid port id
 *      RT_ERR_INPUT    - Invalid input parameter
 * Note:
 *      The storm group types are as following:
 *      - STORM_GROUP_MULTICAST
 *      - STORM_GROUP_BROADCAST
 *      - STORM_GROUP_UNICAST
 */
int32
dal_maple_rate_portStormCtrlEnable_set(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_group_t  storm_type,
    rtk_enable_t            enable)
{
    int32   ret;
    uint32  reg_idx;
    uint32  field_idx;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, storm_type=%d",
           unit, port, storm_type);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);

    switch (storm_type)
    {
        case STORM_GROUP_UNICAST:
            reg_idx = MAPLE_STORM_CTRL_LB_CTRLr;
            field_idx = MAPLE_UC_LB_ENf;
            break;
        case STORM_GROUP_MULTICAST:
            reg_idx = MAPLE_STORM_CTRL_LB_CTRLr;
            field_idx = MAPLE_MC_LB_ENf;
            break;
        case STORM_GROUP_BROADCAST:
            reg_idx = MAPLE_STORM_CTRL_LB_CTRLr;
            field_idx = MAPLE_BC_LB_ENf;
            break;
        default:
            return RT_ERR_INPUT;
    }

    /* chip's value translate */
    switch (enable)
    {
        case DISABLED:
            value = 0;
            break;
        case ENABLED:
            value = 1;
            break;
        default:
            return RT_ERR_INPUT;
    }

    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_array_field_write(unit, reg_idx, port, REG_ARRAY_INDEX_NONE, field_idx, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_rate_portStormCtrlEnable_set */

/* Function Name:
 *      dal_maple_rate_portStormCtrlRate_get
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
 * Note:
 *      (1) Supported management frame is as following:
 *          rtk_rate_storm_group_t
 *          - STORM_GROUP_MULTICAST
 *          - STORM_GROUP_BROADCAST
 *          - STORM_GROUP_UNICAST
 *      (2) The actual rate is "rate * chip granularity".
 *      (3) The unit of granularity is 16Kbps or 1pps depend on mode set by 'rtk_rate_stormControlRateMode_set'.
 */
int32
dal_maple_rate_portStormCtrlRate_get(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_group_t  storm_type,
    uint32                  *pRate)
{
    int32   ret;
    uint32  reg_idx;
    uint32  field_idx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, storm_type=%d",
           unit, port, storm_type);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((storm_type >= STORM_GROUP_END), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pRate), RT_ERR_NULL_POINTER);

    switch (storm_type)
    {
        case STORM_GROUP_UNICAST:
            reg_idx = MAPLE_STORM_CTRL_PORT_UCr;
            field_idx = MAPLE_UC_RATEf;
            break;
        case STORM_GROUP_MULTICAST:
            reg_idx = MAPLE_STORM_CTRL_PORT_MCr;
            field_idx = MAPLE_MC_RATEf;
            break;
        case STORM_GROUP_BROADCAST:
            reg_idx = MAPLE_STORM_CTRL_PORT_BCr;
            field_idx = MAPLE_BC_RATEf;
            break;
        default:
            return RT_ERR_INPUT;
    }

    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_array_field_read(unit, reg_idx, port, REG_ARRAY_INDEX_NONE, field_idx, pRate)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "pRate=%d", *pRate);

    return RT_ERR_OK;
} /* end of dal_maple_rate_portStormCtrlRate_get */

/* Function Name:
 *      dal_maple_rate_portStormCtrlRate_set
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
 * Note:
 *      (1) Supported management frame is as following:
 *          rtk_rate_storm_group_t
 *          - STORM_GROUP_MULTICAST
 *          - STORM_GROUP_BROADCAST
 *          - STORM_GROUP_UNICAST
 *      (2) The actual rate is "rate * chip granularity".
 *      (3) The unit of granularity is 16Kbps or 1pps depend on mode set by 'rtk_rate_stormControlRateMode_set'.
 */
int32
dal_maple_rate_portStormCtrlRate_set(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_group_t  storm_type,
    uint32                  rate)
{
    int32   ret;
    uint32  reg_idx;
    uint32  field_idx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, storm_type=%d,\
           rate=%d", unit, port, storm_type, rate);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((storm_type >= STORM_GROUP_END), RT_ERR_INPUT);
    RT_PARAM_CHK((rate > HAL_RATE_OF_STORM_CONTROL_MAX(unit)), RT_ERR_RATE);

    switch (storm_type)
    {
        case STORM_GROUP_UNICAST:
            reg_idx = MAPLE_STORM_CTRL_PORT_UCr;
            field_idx = MAPLE_UC_RATEf;
            break;
        case STORM_GROUP_MULTICAST:
            reg_idx = MAPLE_STORM_CTRL_PORT_MCr;
            field_idx = MAPLE_MC_RATEf;
            break;
        case STORM_GROUP_BROADCAST:
            reg_idx = MAPLE_STORM_CTRL_PORT_BCr;
            field_idx = MAPLE_BC_RATEf;
            break;
        default:
            return RT_ERR_INPUT;
    }

    RATE_SEM_LOCK(unit);

    /* program value to CHIP*/
    if ((ret = reg_array_field_write(unit, reg_idx, port, REG_ARRAY_INDEX_NONE, field_idx, &rate)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_rate_portStormCtrlRate_set */

/* Function Name:
 *      dal_maple_rate_stormControlRateMode_get
 * Description:
 *      Get rate counting mode of storm control.
 * Input:
 *      unit       - unit id
 * Output:
 *      pRate_mode - pointer to rate counting mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *    1) The rate mode are as following:
 *       - BASED_ON_PKT
 *       - BASED_ON_BYTE
 */
int32
dal_maple_rate_stormControlRateMode_get(
    uint32                      unit,
    rtk_rate_storm_rateMode_t   *pRate_mode)
{
    uint32  value;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pRate_mode), RT_ERR_NULL_POINTER);

    RATE_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_field_read(unit, MAPLE_STORM_CTRL_CTRLr, MAPLE_MODEf, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    if (1 == value)
        *pRate_mode = BASED_ON_BYTE;
    else
        *pRate_mode = BASED_ON_PKT;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "pRate_mode=%d", *pRate_mode);

    return RT_ERR_OK;
} /* end of dal_maple_rate_stormControlRateMode_get */

/* Function Name:
 *      dal_maple_rate_stormControlRateMode_set
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
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *    1) The rate mode are as following:
 *       - BASED_ON_PKT
 *       - BASED_ON_BYTE
 */
int32
dal_maple_rate_stormControlRateMode_set(
    uint32                      unit,
    rtk_rate_storm_rateMode_t   rate_mode)
{
    uint32  value;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, rate_mode=%d", unit, rate_mode);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((rate_mode >= STORM_RATE_MODE_END), RT_ERR_INPUT);

    if (rate_mode == BASED_ON_BYTE)
        value = 1;
    else
        value = 0;

    RATE_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_field_write(unit, MAPLE_STORM_CTRL_CTRLr, MAPLE_MODEf, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_rate_stormControlRateMode_set */

/* Function Name:
 *      dal_maple_rate_stormCtrlExceed_get
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
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_PORT_ID           - invalid port id
 *      RT_ERR_SFC_UNKNOWN_GROUP - Unknown storm group
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 * Note:
 *      Exceed status is as following
 *      - TRUE      - storm rate is more than configured rate.
 *      - FALSE     - storm rate is never over then configured rate.
 */
int32
dal_maple_rate_portStormCtrlExceed_get(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_group_t  storm_type,
    uint32                  *pIsExceed)
{
    int32   ret;
    uint32  value;
    uint32  reg_idx;
    uint32  field_idx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, storm_type=%d",
           unit, port, storm_type);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((storm_type >= STORM_GROUP_END), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pIsExceed), RT_ERR_NULL_POINTER);

    switch (storm_type)
    {
        case STORM_GROUP_UNICAST:
            reg_idx = MAPLE_STORM_CTRL_PORT_UC_EXCEED_FLGr;
            field_idx = MAPLE_UC_EXCEED_FLGf;
            break;
        case STORM_GROUP_MULTICAST:
            reg_idx = MAPLE_STORM_CTRL_PORT_MC_EXCEED_FLGr;
            field_idx = MAPLE_MC_EXCEED_FLGf;
            break;
        case STORM_GROUP_BROADCAST:
            reg_idx = MAPLE_STORM_CTRL_PORT_BC_EXCEED_FLGr;
            field_idx = MAPLE_BC_EXCEED_FLGf;
            break;
        default:
            return RT_ERR_INPUT;
    }

    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_array_field_read(unit, reg_idx, port, REG_ARRAY_INDEX_NONE, field_idx, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    /* chip's value translate */
    switch (value)
    {
        case 0:
            *pIsExceed = FALSE;
            break;
        case 1:
            *pIsExceed = TRUE;
            break;
        default:
            return RT_ERR_FAILED;
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "pIsExceed=%d", *pIsExceed);

    return RT_ERR_OK;
} /* end of dal_maple_rate_portStormCtrlExceed_get */

/* Function Name:
 *      dal_maple_rate_portStormCtrlExceed_reset
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
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
dal_maple_rate_portStormCtrlExceed_reset(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_group_t  storm_type)
{
    int32   ret;
    uint32  value = 1;
    uint32  reg_idx;
    uint32  field_idx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, storm_type=%d",
           unit, port, storm_type);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((storm_type >= STORM_GROUP_END), RT_ERR_INPUT);

    switch (storm_type)
    {
        case STORM_GROUP_UNICAST:
            reg_idx = MAPLE_STORM_CTRL_PORT_UC_EXCEED_FLGr;
            field_idx = MAPLE_UC_EXCEED_FLGf;
            break;
        case STORM_GROUP_MULTICAST:
            reg_idx = MAPLE_STORM_CTRL_PORT_MC_EXCEED_FLGr;
            field_idx = MAPLE_MC_EXCEED_FLGf;
            break;
        case STORM_GROUP_BROADCAST:
            reg_idx = MAPLE_STORM_CTRL_PORT_BC_EXCEED_FLGr;
            field_idx = MAPLE_BC_EXCEED_FLGf;
            break;
        default:
            return RT_ERR_INPUT;
    }

    RATE_SEM_LOCK(unit);

    /* write value from CHIP*/
    if ((ret = reg_array_field_read(unit, reg_idx, port, REG_ARRAY_INDEX_NONE, field_idx, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit, reg_idx, port, REG_ARRAY_INDEX_NONE, field_idx, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_rate_portStormCtrlExceed_reset */

/* Function Name:
 *      dal_maple_rate_stormControlBurstSize_get
 * Description:
 *      Get burst size of storm control
 * Input:
 *      unit        - unit id
 *      storm_type  - storm group type
 * Output:
 *      pBurst_size - pointer to burst size
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The storm group types are as following:
 *      - STORM_GROUP_MULTICAST
 *      - STORM_GROUP_BROADCAST
 *      - STORM_GROUP_UNICAST
 */
int32
dal_maple_rate_stormControlBurstSize_get(
    uint32                  unit,
    rtk_rate_storm_group_t  storm_type,
    uint32                  *pBurst_size)
{
    int32   ret;
    uint32  field_idx;
    uint32  stormMode;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, storm_type=%d",
           unit, storm_type);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((storm_type >= STORM_GROUP_END), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pBurst_size), RT_ERR_NULL_POINTER);

    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_field_read(unit, MAPLE_STORM_CTRL_CTRLr, MAPLE_MODEf, &stormMode)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }
    RATE_SEM_UNLOCK(unit);

    switch (storm_type)
    {
        case STORM_GROUP_UNICAST:
            if (stormMode == 0)
                field_idx = MAPLE_UC_BURST_PPSf;
            else
                field_idx = MAPLE_UCf;
            break;
        case STORM_GROUP_MULTICAST:
            if (stormMode == 0)
                field_idx = MAPLE_MC_BURST_PPSf;
            else
                field_idx = MAPLE_MCf;
            break;
        case STORM_GROUP_BROADCAST:
            if (stormMode == 0)
                field_idx = MAPLE_BC_BURST_PPSf;
            else
                field_idx = MAPLE_BCf;
            break;
        default:
            return RT_ERR_INPUT;
    }

    RATE_SEM_LOCK(unit);
    /* read value from CHIP*/
    if ((ret = reg_field_read(unit, MAPLE_STORM_CTRL_BURSTr, field_idx, pBurst_size)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "pBurst_size=%d", *pBurst_size);

    return RT_ERR_OK;
} /* end of dal_maple_rate_stormControlBurstSize_get */

/* Function Name:
 *      dal_maple_rate_stormControlBurstSize_set
 * Description:
 *      Set burst size of storm control
 * Input:
 *      unit       - unit id
 *      storm_type - storm group type
 *      burst_size - burst size
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      The storm group types are as following:
 *      - STORM_GROUP_MULTICAST
 *      - STORM_GROUP_BROADCAST
 *      - STORM_GROUP_UNICAST
 */
int32
dal_maple_rate_stormControlBurstSize_set(
    uint32                  unit,
    rtk_rate_storm_group_t  storm_type,
    uint32                  burst_size)
{
    int32   ret;
    uint32  field_idx;
    uint32  stormMode;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, storm_type=%d, burst_size =%d",
           unit, storm_type, burst_size);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((storm_type >= STORM_GROUP_END), RT_ERR_INPUT);
    RT_PARAM_CHK((burst_size > HAL_BURST_RATE_OF_STORM_CONTROL_MAX(unit)), RT_ERR_INPUT);

    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_field_read(unit, MAPLE_STORM_CTRL_CTRLr, MAPLE_MODEf, &stormMode)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    switch (storm_type)
    {
        case STORM_GROUP_UNICAST:
            if (stormMode == 0)
                field_idx = MAPLE_UC_BURST_PPSf;
            else
                field_idx = MAPLE_UCf;
            break;
        case STORM_GROUP_MULTICAST:
            if (stormMode == 0)
                field_idx = MAPLE_MC_BURST_PPSf;
            else
                field_idx = MAPLE_MCf;
            break;
        case STORM_GROUP_BROADCAST:
            if (stormMode == 0)
                field_idx = MAPLE_BC_BURST_PPSf;
            else
                field_idx = MAPLE_BCf;
            break;
        default:
            RATE_SEM_UNLOCK(unit);
            return RT_ERR_INPUT;
    }

    /* read value from CHIP*/
    if ((ret = reg_field_write(unit, MAPLE_STORM_CTRL_BURSTr, field_idx, &burst_size)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_rate_stormControlBurstsize_set */

/* Function Name:
 *      dal_maple_rate_portStormCtrlTypeSel_get
 * Description:
 *      Get the storm control type.
 * Input:
 *      unit       - unit id
 *      port       - port id
 *      storm_type - storm group type
 * Output:
 *      pStorm_sel - storm selection
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_INPUT        - Invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The API is only supported in unicast and multicast, the storm group types are as following:
 *      - STORM_GROUP_UNICAST
 *      - STORM_GROUP_MULTICAST
 *
 *      The storm selection are as following:
 *      - STORM_SEL_UNKNOWN
 *      - STORM_SEL_UNKNOWN_AND_KNOWN
 */
int32
dal_maple_rate_portStormCtrlTypeSel_get(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_group_t  storm_type,
    rtk_rate_storm_sel_t    *pStorm_sel)
{
    int32   ret;
    uint32  reg_idx;
    uint32  field_idx;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, storm_type=%d",
           unit, port, storm_type);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((storm_type >= STORM_GROUP_END), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pStorm_sel), RT_ERR_NULL_POINTER);

    switch (storm_type)
    {
        case STORM_GROUP_UNICAST:
            reg_idx = MAPLE_STORM_CTRL_PORT_UCr;
            field_idx = MAPLE_UC_TYPEf;
            break;
        case STORM_GROUP_MULTICAST:
            reg_idx = MAPLE_STORM_CTRL_PORT_MCr;
            field_idx = MAPLE_MC_TYPEf;
            break;
        default:
            return RT_ERR_INPUT;
    }

    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_array_field_read(unit, reg_idx, port, REG_ARRAY_INDEX_NONE, field_idx, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    /* chip's value translate */
    switch (value)
    {
        case 0:
            *pStorm_sel = STORM_SEL_UNKNOWN;
            break;
        case 1:
            *pStorm_sel = STORM_SEL_UNKNOWN_AND_KNOWN;
            break;
        default:
            return RT_ERR_FAILED;
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "pStorm_sel=%d", *pStorm_sel);

    return RT_ERR_OK;
} /* end of dal_maple_rate_portStormCtrlTypeSel_get */

/* Function Name:
 *      dal_maple_rate_portStormCtrlTypeSel_set
 * Description:
 *      Set the storm control type.
 * Input:
 *      unit       - unit id
 *      port       - port id
 *      storm_type - storm group type
 *      storm_sel  - storm selection
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - Invalid port id
 *      RT_ERR_INPUT    - Invalid input parameter
 * Note:
 *      The API is only supported in unicast and multicast, the storm group types are as following:
 *      - STORM_GROUP_UNICAST
 *      - STORM_GROUP_MULTICAST
 *
 *      The storm selection are as following:
 *      - STORM_SEL_ONLY_UNKNOWN
 *      - STORM_SEL_BOTH_UNKNOWN_KNOWN
 */
int32
dal_maple_rate_portStormCtrlTypeSel_set(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_group_t  storm_type,
    rtk_rate_storm_sel_t    storm_sel)
{
    int32   ret;
    uint32  reg_idx;
    uint32  field_idx;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, storm_type=%d\
           storm_sel=%d", unit, port, storm_type, storm_sel);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((storm_type >= STORM_GROUP_END), RT_ERR_INPUT);
    RT_PARAM_CHK((storm_sel > STORM_SEL_END), RT_ERR_INPUT);

    switch (storm_type)
    {
        case STORM_GROUP_UNICAST:
            reg_idx = MAPLE_STORM_CTRL_PORT_UCr;
            field_idx = MAPLE_UC_TYPEf;
            break;
        case STORM_GROUP_MULTICAST:
            reg_idx = MAPLE_STORM_CTRL_PORT_MCr;
            field_idx = MAPLE_MC_TYPEf;
            break;
        default:
            return RT_ERR_INPUT;
    }

    /* translate to chip value */
    switch (storm_sel)
    {
        case STORM_SEL_UNKNOWN:
            value = 0;
            break;
        case STORM_SEL_UNKNOWN_AND_KNOWN:
            value = 1;
            break;
        default:
            return RT_ERR_INPUT;
    }

    RATE_SEM_LOCK(unit);

    /* program value to CHIP*/
    if ((ret = reg_array_field_write(unit, reg_idx, port, REG_ARRAY_INDEX_NONE, field_idx, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_rate_portStormCtrlTypeSel_set */

/* Function Name:
 *      dal_maple_rate_stormControlBypass_get
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
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - Invalid input parameter
 * Note:
 *      None.
 */
int32
dal_maple_rate_stormControlBypass_get(uint32 unit, rtk_rate_storm_bypassType_t bypassType, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, bypassType=%d", unit, bypassType);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(bypassType >= STORM_BYPASS_END, RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    RATE_SEM_LOCK(unit);

    /* get value from CHIP*/
    switch (bypassType)
    {
        case STORM_BYPASS_RMA:
            ret = reg_field_read(unit, MAPLE_STORM_CTRL_CTRLr, MAPLE_RMA_ADMITf, &value);
            break;
        case STORM_BYPASS_BPDU:
            ret = reg_field_read(unit, MAPLE_STORM_CTRL_CTRLr, MAPLE_BPDU_ADMITf, &value);
            break;
        case STORM_BYPASS_RTKPKT:
            ret = reg_field_read(unit, MAPLE_STORM_CTRL_CTRLr, MAPLE_RTKPKT_ADMITf, &value);
            break;
        case STORM_BYPASS_IGMP:
            ret = reg_field_read(unit, MAPLE_STORM_CTRL_CTRLr, MAPLE_IGMP_ADMITf, &value);
            break;
        case STORM_BYPASS_ARP:
            ret = reg_field_read(unit, MAPLE_STORM_CTRL_CTRLr, MAPLE_ARPREQ_ADMITf, &value);
            break;
        default:
            ret = RT_ERR_INPUT;
    }
    if (ret != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    /* chip's value translate */
    switch (value)
    {
        case 0:
            *pEnable = DISABLED;
            break;
        case 1:
            *pEnable = ENABLED;
            break;
        default:
            return RT_ERR_FAILED;
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
} /* end of dal_maple_rate_stormControlBypass_get */

/* Function Name:
 *      dal_maple_rate_stormControlBypass_set
 * Description:
 *      Set the status of bypass storm control for specified packet type.
 * Input:
 *      unit       - unit id
 *      bypassType - bypass type
 *      enable     - status of bypass storm control
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - Invalid input parameter
 * Note:
 *      None.
 */
int32
dal_maple_rate_stormControlBypass_set(uint32 unit, rtk_rate_storm_bypassType_t bypassType, rtk_enable_t enable)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, bypassType=%d, enable=%d",
           unit, bypassType, enable);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(bypassType >= STORM_BYPASS_END, RT_ERR_INPUT);

    /* translate to chip value */
    switch (enable)
    {
        case DISABLED:
            value = 0;
            break;
        case ENABLED:
            value = 1;
            break;
        default:
            return RT_ERR_INPUT;
    }

    RATE_SEM_LOCK(unit);

    /* program value into CHIP*/
    switch (bypassType)
    {
        case STORM_BYPASS_RMA:
            ret = reg_field_write(unit, MAPLE_STORM_CTRL_CTRLr, MAPLE_RMA_ADMITf, &value);
            break;
        case STORM_BYPASS_BPDU:
            ret = reg_field_write(unit, MAPLE_STORM_CTRL_CTRLr, MAPLE_BPDU_ADMITf, &value);
            break;
        case STORM_BYPASS_RTKPKT:
            ret = reg_field_write(unit, MAPLE_STORM_CTRL_CTRLr, MAPLE_RTKPKT_ADMITf, &value);
            break;
        case STORM_BYPASS_IGMP:
            ret = reg_field_write(unit, MAPLE_STORM_CTRL_CTRLr, MAPLE_IGMP_ADMITf, &value);
            break;
        case STORM_BYPASS_ARP:
            ret = reg_field_write(unit, MAPLE_STORM_CTRL_CTRLr, MAPLE_ARPREQ_ADMITf, &value);
            break;
        default:
            ret = RT_ERR_INPUT;
    }

    if (ret != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_rate_stormControlBypass_set */

/* Function Name:
 *      dal_maple_rate_stormControlIncludeIfg_get
 * Description:
 *      Get enable status of includes IFG for storm control.
 * Input:
 *      unit         - unit id
 * Output:
 *      pIfg_include - pointer to enable status of includes IFG
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_maple_rate_stormControlIncludeIfg_get(uint32 unit, rtk_enable_t *pIfg_include)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIfg_include), RT_ERR_NULL_POINTER);

    RATE_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_field_read(unit, MAPLE_STORM_CTRL_CTRLr, MAPLE_INC_IFGf, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }
    RATE_SEM_UNLOCK(unit);

    /* translate to chip value */
    switch (value)
    {
        case 0:
            *pIfg_include = DISABLED;
            break;
        case 1:
            *pIfg_include = ENABLED;
            break;
        default:
            return RT_ERR_FAILED;
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "pIfg_include=%d", *pIfg_include);

    return RT_ERR_OK;
} /* end of dal_maple_rate_stormControlIncludeIfg_get */

/* Function Name:
 *      dal_maple_rate_stormControlIncludeIfg_set
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
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
dal_maple_rate_stormControlIncludeIfg_set(uint32 unit, rtk_enable_t ifg_include)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, ifg_include=%d", unit, ifg_include);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((ifg_include >= RTK_ENABLE_END), RT_ERR_INPUT);

    /* translate to chip value */
    switch (ifg_include)
    {
        case DISABLED:
            value = 0;
            break;
        case ENABLED:
            value = 1;
            break;
        default:
            return RT_ERR_INPUT;
    }

    RATE_SEM_LOCK(unit);

    /* program value to CHIP */
    if ((ret = reg_field_write(unit, MAPLE_STORM_CTRL_CTRLr, MAPLE_INC_IFGf, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_rate_stormControlIncludeIfg_set */


/* Function Name:
 *      dal_maple_rate_portIgrQueueBwCtrlEnable_get
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
 * Note:
 *      None
 */
int32
dal_maple_rate_portIgrQueueBwCtrlEnable_get(
    uint32          unit,
    rtk_port_t      port,
    rtk_qid_t       queue,
    rtk_enable_t    *pEnable)
{
    int32   ret;
    uint32  value;
    uint32  regField;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, queue=%d\n", unit, port, queue);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((queue >= (HAL_MAX_NUM_OF_IGR_QUEUE(unit) - 1)), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    switch (queue)
    {
        case 0:
            regField = MAPLE_Q0_LB_ENf;
            break;
        case 1:
            regField = MAPLE_Q1_LB_ENf;
            break;
        case 2:
            regField = MAPLE_Q2_LB_ENf;
            break;
        default:
            return RT_ERR_QUEUE_ID;
    }

    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_array_field_read(unit, MAPLE_IGR_BWCTRL_LB_CTRLr, port, REG_ARRAY_INDEX_NONE, regField, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    if (value == 1)
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
} /* end of dal_maple_rate_portIgrQueueBwCtrlEnable_get */

/* Function Name:
 *      dal_maple_rate_portIgrQueueBwCtrlEnable_set
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
 * Note:
 *      None
 */
int32
dal_maple_rate_portIgrQueueBwCtrlEnable_set(
    uint32          unit,
    rtk_port_t      port,
    rtk_qid_t       queue,
    rtk_enable_t    enable)
{
    int32   ret;
    uint32  value;
    uint32  regField;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, queue=%d, enable=%d\n", unit, port, queue, enable);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((queue >= (HAL_MAX_NUM_OF_IGR_QUEUE(unit) - 1)), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    if (enable == ENABLED)
        value = 1;
    else
        value = 0;

    switch (queue)
    {
        case 0:
            regField = MAPLE_Q0_LB_ENf;
            break;
        case 1:
            regField = MAPLE_Q1_LB_ENf;
            break;
        case 2:
            regField = MAPLE_Q2_LB_ENf;
            break;
        default:
            return RT_ERR_QUEUE_ID;
    }

    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_array_field_write(unit, MAPLE_IGR_BWCTRL_LB_CTRLr, port, REG_ARRAY_INDEX_NONE, regField, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_rate_portIgrQueueBwCtrlEnable_set */

/* Function Name:
 *      dal_maple_rate_portIgrQueueBwCtrlRate_get
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
 * Note:
 *      The actual rate is "rate * chip granularity".
 *      The unit of granularity is 16Kbps.
 */
int32
dal_maple_rate_portIgrQueueBwCtrlRate_get(
    uint32      unit,
    rtk_port_t  port,
    rtk_qid_t   queue,
    uint32      *pRate)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, queue=%d\n", unit, port, queue);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((queue >= (HAL_MAX_NUM_OF_IGR_QUEUE(unit) - 1)), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((NULL == pRate), RT_ERR_NULL_POINTER);

    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_array_field_read(unit, MAPLE_IGR_BWCTRL_QUEUE_RATE_CTRLr, port, queue, MAPLE_Q_RATEf, pRate)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "pRate=%d", *pRate);

    return RT_ERR_OK;
} /* end of dal_maple_rate_portIgrQueueBwCtrlRate_get */

/* Function Name:
 *      dal_maple_rate_portIgrQueueBwCtrlRate_set
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
 * Note:
 *      The actual rate is "rate * chip granularity".
 *      The unit of granularity is 16Kbps.
 */
int32
dal_maple_rate_portIgrQueueBwCtrlRate_set(
    uint32      unit,
    rtk_port_t  port,
    rtk_qid_t   queue,
    uint32      rate)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, queue=%d, rate=%d\n", unit, port, queue, rate);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((queue >= (HAL_MAX_NUM_OF_IGR_QUEUE(unit) - 1)), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((rate > HAL_RATE_OF_BANDWIDTH_MAX(unit)), RT_ERR_RATE);

    RATE_SEM_LOCK(unit);

    /* write value to CHIP*/
    if ((ret = reg_array_field_write(unit, MAPLE_IGR_BWCTRL_QUEUE_RATE_CTRLr, port, queue, MAPLE_Q_RATEf, &rate)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_rate_portIgrQueueBwCtrlRate_set */

/* Function Name:
 *      dal_maple_rate_portIgrBandwidthCtrlExceed_get
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
 *      - TRUE      - ingress bandwidth rate is more than configured rate.
 *      - FALSE     -ingress bandwidth rate is never over then configured rate.
 */
int32
dal_maple_rate_portIgrBandwidthCtrlExceed_get(
    uint32                  unit,
    rtk_port_t              port,
    uint32                  *pIsExceed)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d\n", unit, port);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pIsExceed), RT_ERR_NULL_POINTER);

    RATE_SEM_LOCK(unit);

    /* write value to CHIP*/
    if ((ret = reg_array_field_read(unit, MAPLE_IGR_BWCTRL_EXCEED_FLGr, port, REG_ARRAY_INDEX_NONE, MAPLE_P_EXCEED_FLGf, pIsExceed)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "pIsExceed=%d", *pIsExceed);

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_maple_rate_portIgrBandwidthCtrlExceed_reset
 * Description:
 *      Reset exceed status of ingress bandwidth on specified port.
 * Input:
 *      unit       - unit id
 *      port       - port id
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
dal_maple_rate_portIgrBandwidthCtrlExceed_reset(
    uint32                  unit,
    rtk_port_t              port)
{
    int32   ret;
    uint32 val = 1;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d\n", unit, port);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);

    RATE_SEM_LOCK(unit);

    /* write value to CHIP*/
    if ((ret = reg_array_field_write(unit, MAPLE_IGR_BWCTRL_EXCEED_FLGr, port, REG_ARRAY_INDEX_NONE, MAPLE_P_EXCEED_FLGf, &val)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_maple_rate_portIgrQueueBwCtrlExceed_get
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
 * Note:
 *      Exceed status is as following
 *      - TRUE      - ingress queue bandwidth rate is more than configured rate.
 *      - FALSE     -ingress queue bandwidth rate is never over then configured rate.
 */
int32
dal_maple_rate_portIgrQueueBwCtrlExceed_get(
    uint32                  unit,
    rtk_port_t              port,
    rtk_qid_t               queue,
    uint32                  *pIsExceed)
{
    int32   ret;
    uint32 regField;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, queue=%d\n", unit, port, queue);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((queue >= (HAL_MAX_NUM_OF_IGR_QUEUE(unit) - 1)), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((NULL == pIsExceed), RT_ERR_NULL_POINTER);

    switch (queue)
    {
        case 0:
            regField = MAPLE_Q0_EXCEED_FLGf;
            break;
        case 1:
            regField = MAPLE_Q1_EXCEED_FLGf;
            break;
        case 2:
            regField = MAPLE_Q2_EXCEED_FLGf;
            break;
        default:
            return RT_ERR_QUEUE_ID;
    }

    RATE_SEM_LOCK(unit);

    /* write value to CHIP*/
    if ((ret = reg_array_field_read(unit, MAPLE_IGR_BWCTRL_EXCEED_FLGr, port, REG_ARRAY_INDEX_NONE, regField, pIsExceed)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "pIsExceed=%d", *pIsExceed);

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_maple_rate_portIgrQueueBwCtrlExceed_reset
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
 * Note:
 *      None
 */
int32
dal_maple_rate_portIgrQueueBwCtrlExceed_reset(
    uint32                  unit,
    rtk_port_t              port,
    rtk_qid_t               queue)
{
    int32   ret;
    uint32 val = 1;
    uint32 regField;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, queue=%d\n", unit, port, queue);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((queue >= (HAL_MAX_NUM_OF_IGR_QUEUE(unit) - 1)), RT_ERR_QUEUE_ID);

    switch (queue)
    {
        case 0:
            regField = MAPLE_Q0_EXCEED_FLGf;
            break;
        case 1:
            regField = MAPLE_Q1_EXCEED_FLGf;
            break;
        case 2:
            regField = MAPLE_Q2_EXCEED_FLGf;
            break;
        default:
            return RT_ERR_QUEUE_ID;
    }

    RATE_SEM_LOCK(unit);

    /* write value to CHIP*/
    if ((ret = reg_array_field_write(unit, MAPLE_IGR_BWCTRL_EXCEED_FLGr, port, REG_ARRAY_INDEX_NONE, regField, &val)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_maple_rate_igrPortBwCtrlBurstSize_get
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
 * Note:
 *      None
 */
int32
dal_maple_rate_igrPortBwCtrlBurstSize_get(
    uint32                  unit,
    uint32                  *pBurst_size)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pBurst_size), RT_ERR_NULL_POINTER);

    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_field_read(unit, MAPLE_IGR_BWCTRL_LB_THr, MAPLE_P_BURST_SIZEf, pBurst_size)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "pBurst_size=%d", *pBurst_size);

    return RT_ERR_OK;
} /* end of dal_maple_rate_igrPortBwCtrlBurstSize_get */

/* Function Name:
 *      dal_maple_rate_igrPortBwCtrlBurstSize_set
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
 * Note:
 *      None
 */
int32
dal_maple_rate_igrPortBwCtrlBurstSize_set(
    uint32                  unit,
    uint32                  burst_size)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, burst_size=0x%x", unit, burst_size);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    RT_PARAM_CHK((burst_size > HAL_BURST_OF_IGR_BANDWIDTH_MAX(unit)), RT_ERR_INPUT);

    /* parameter check */
    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_field_write(unit, MAPLE_IGR_BWCTRL_LB_THr, MAPLE_P_BURST_SIZEf, &burst_size)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_rate_igrPortBwCtrlBurstSize_set */


/* Function Name:
 *      dal_maple_rate_igrQueueBwCtrlBurstSize_get
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
 * Note:
 *      None
 */
int32
dal_maple_rate_igrQueueBwCtrlBurstSize_get(
    uint32                  unit,
    uint32                  *pBurst_size)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pBurst_size), RT_ERR_NULL_POINTER);

    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_field_read(unit, MAPLE_IGR_BWCTRL_LB_THr, MAPLE_Q_BURST_SIZEf, pBurst_size)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "pBurst_size=%d", *pBurst_size);

    return RT_ERR_OK;
} /* end of dal_maple_rate_igrQueueBwCtrlBurstSize_get */

/* Function Name:
 *      dal_maple_rate_igrQueueBwCtrlBurstSize_set
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
 * Note:
 *      None
 */
int32
dal_maple_rate_igrQueueBwCtrlBurstSize_set(
    uint32                  unit,
    uint32                  burst_size)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, burst_size=0x%x", unit, burst_size);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    RT_PARAM_CHK((burst_size > HAL_BURST_OF_IGR_BANDWIDTH_MAX(unit)), RT_ERR_INPUT);

    /* parameter check */
    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_field_write(unit, MAPLE_IGR_BWCTRL_LB_THr, MAPLE_Q_BURST_SIZEf, &burst_size)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_rate_igrQueueBwCtrlBurstSize_set */

/* Function Name:
 *      dal_maple_rate_egrPortBwCtrlBurstSize_get
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
int32
dal_maple_rate_egrPortBwCtrlBurstSize_get(
    uint32                  unit,
    uint32                  *pBurst_size)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pBurst_size), RT_ERR_NULL_POINTER);

    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_field_read(unit, MAPLE_SCHED_LB_THRr, MAPLE_WFQ_LB_SIZEf, pBurst_size)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "pBurst_size=%d", *pBurst_size);

    return RT_ERR_OK;
} /* end of dal_maple_rate_egrPortBwCtrlBurstSize_get */

/* Function Name:
 *      dal_maple_rate_egrPortBwCtrlBurstSize_set
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
int32
dal_maple_rate_egrPortBwCtrlBurstSize_set(
    uint32                  unit,
    uint32                  burst_size)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, burst_size=0x%x", unit, burst_size);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    RT_PARAM_CHK((burst_size > HAL_BURST_OF_BANDWIDTH_MAX(unit)), RT_ERR_INPUT);

    /* parameter check */
    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_field_write(unit, MAPLE_SCHED_LB_THRr, MAPLE_WFQ_LB_SIZEf, &burst_size)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_rate_egrPortBwCtrlBurstSize_set */


/* Function Name:
 *      dal_maple_rate_egrQueueBwCtrlBurstSize_get
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
int32
dal_maple_rate_egrQueueBwCtrlBurstSize_get(
    uint32                  unit,
    uint32                  *pBurst_size)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pBurst_size), RT_ERR_NULL_POINTER);

    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_field_read(unit, MAPLE_SCHED_LB_THRr, MAPLE_APR_LB_SIZEf, pBurst_size)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "pBurst_size=%d", *pBurst_size);

    return RT_ERR_OK;
} /* end of dal_maple_rate_egrQueueBwCtrlBurstSize_get */

/* Function Name:
 *      dal_maple_rate_egrQueueBwCtrlBurstSize_set
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
int32
dal_maple_rate_egrQueueBwCtrlBurstSize_set(
    uint32                  unit,
    uint32                  burst_size)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, burst_size=0x%x", unit, burst_size);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((burst_size > HAL_BURST_OF_BANDWIDTH_MAX(unit)), RT_ERR_INPUT);

    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_field_write(unit, MAPLE_SCHED_LB_THRr, MAPLE_APR_LB_SIZEf, &burst_size)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_rate_egrQueueBwCtrlBurstSize_set */


/* Function Name:
 *      dal_maple_rate_cpuEgrBandwidthCtrlRateMode_get
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
int32
dal_maple_rate_cpuEgrBandwidthCtrlRateMode_get(uint32 unit, rtk_rate_rateMode_t *pMode)
{
    uint32  value;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pMode), RT_ERR_NULL_POINTER);


    RATE_SEM_LOCK(unit);

    /* program value to CHIP */
    if ((ret = reg_field_read(unit, MAPLE_SCHED_CTRLr, MAPLE_P28_LB_MODEf, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    if (0 == value)
        *pMode = RATE_MODE_BYTE;
    else
        *pMode = RATE_MODE_PKT;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_maple_rate_cpuEgrBandwidthCtrlRateMode_set
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
int32
dal_maple_rate_cpuEgrBandwidthCtrlRateMode_set(uint32 unit, rtk_rate_rateMode_t mode)
{
    uint32  value;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, mode=%d", unit, mode);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    if (RATE_MODE_BYTE == mode)
        value = 0;
    else if (RATE_MODE_PKT == mode)
        value = 1;
    else
        return RT_ERR_INPUT;

    RATE_SEM_LOCK(unit);

    /* program value to CHIP */
    if ((ret = reg_field_write(unit, MAPLE_SCHED_CTRLr, MAPLE_P28_LB_MODEf, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    if(RATE_MODE_PKT == mode)
    {
        if ((ret = dal_maple_qos_schedulingAlgorithm_set(unit, HWP_CPU_MACID(unit), WRR)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_maple_rate_cpuEgrBandwidthCtrl_workaround
 * Description:
 *      Reset cpu port egress bandwidth control
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      Reset CPU Port Egress port/queue rate control.
 */
int32 dal_maple_rate_cpuEgrBandwidthCtrl_workaround(uint32 unit)
{
    uint32 lb_sts, value, port28_rate;
    uint32 i;
    uint32 queue_rate[8];
    int32 ret;

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    RATE_SEM_LOCK(unit);

    /* read value from CHIP */
    if ((ret = reg_field_read(unit, MAPLE_SCHED_CTRLr, MAPLE_P28_LB_MODEf, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    if( 1 == value)
    {
        /* read value from CHIP */
        if((ret = reg_array_read(unit, MAPLE_SCHED_LB_CTRLr, 28, REG_ARRAY_INDEX_NONE, &lb_sts)) != RT_ERR_OK)
        {
            RATE_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }

        //osal_printf("lb_sts=0x%x\n", lb_sts);

        if(lb_sts!=0)
        {
            if((ret = reg_array_read(unit, MAPLE_SCHED_P_EGR_RATE_CTRLr, 28, REG_ARRAY_INDEX_NONE, &port28_rate)) != RT_ERR_OK)
            {
                RATE_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
                return ret;
            }

            for(i=0; i<8; i++)
            {
                if((ret = reg_array_read(unit, MAPLE_SCHED_Q_EGR_RATE_CTRLr, 28, i, &queue_rate[i])) != RT_ERR_OK)
                {
                    RATE_SEM_UNLOCK(unit);
                    RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
                    return ret;
                }

                value = 0;
                if((ret = reg_array_write(unit, MAPLE_SCHED_Q_EGR_RATE_CTRLr, 28, i, &value)) != RT_ERR_OK)
                {
                    RATE_SEM_UNLOCK(unit);
                    RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
                    return ret;
                }
            }

            value = 0;
            if((ret = reg_array_write(unit, MAPLE_SCHED_P_EGR_RATE_CTRLr, 28, REG_ARRAY_INDEX_NONE, &value)) != RT_ERR_OK)
            {
                RATE_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
                return ret;
            }

            value = 0xff;
            if((ret = reg_array_write(unit, MAPLE_SCHED_LB_CTRLr, 28, REG_ARRAY_INDEX_NONE, &value)) != RT_ERR_OK)
            {
                RATE_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
                return ret;
            }

            value = 0x100;
            if((ret = reg_array_write(unit, MAPLE_SCHED_LB_CTRLr, 28, REG_ARRAY_INDEX_NONE, &value)) != RT_ERR_OK)
            {
                RATE_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
                return ret;
            }

            if((ret = reg_array_write(unit, MAPLE_SCHED_LB_CTRLr, 28, REG_ARRAY_INDEX_NONE, &lb_sts)) != RT_ERR_OK)
            {
                RATE_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
                return ret;
            }


            for(i=0; i<8; i++)
            {
                if((ret = reg_array_write(unit, MAPLE_SCHED_Q_EGR_RATE_CTRLr, 28, i, &queue_rate[i])) != RT_ERR_OK)
                {
                    RATE_SEM_UNLOCK(unit);
                    RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
                    return ret;
                }
            }

            if((ret = reg_array_write(unit, MAPLE_SCHED_P_EGR_RATE_CTRLr, 28, REG_ARRAY_INDEX_NONE, &port28_rate)) != RT_ERR_OK)
            {
                RATE_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
                return ret;
            }
        }
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;

}
/* Function Name:
 *      dal_maple_rate_includeIfg_get
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
int32
dal_maple_rate_includeIfg_get(uint32 unit, rtk_rate_module_t module, rtk_enable_t *pIfg_include)
{
    switch(module)
    {
        case RATE_MODULE_IGR:
            return dal_maple_rate_bandwidthCtrlIncludeIfg_get(unit, pIfg_include);
        case RATE_MODULE_EGR:
            return dal_maple_rate_bandwidthCtrlIncludeIfg_get(unit, pIfg_include);
        case RATE_MODULE_STORM:
            return dal_maple_rate_stormControlIncludeIfg_get(unit, pIfg_include);
        default:
            return RT_ERR_INPUT;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_maple_rate_includeIfg_set
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
int32
dal_maple_rate_includeIfg_set(uint32 unit, rtk_rate_module_t module, rtk_enable_t ifg_include)
{
    switch(module)
    {
        case RATE_MODULE_IGR:
            return dal_maple_rate_bandwidthCtrlIncludeIfg_set(unit, ifg_include);
        case RATE_MODULE_EGR:
            return dal_maple_rate_bandwidthCtrlIncludeIfg_set(unit, ifg_include);
        case RATE_MODULE_STORM:
            return dal_maple_rate_stormControlIncludeIfg_set(unit, ifg_include);
        default:
            return RT_ERR_INPUT;
    }

    return RT_ERR_OK;
}

