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
#include <hal/chipdef/longan/rtk_longan_table_struct.h>
#include <hal/chipdef/longan/rtk_longan_reg_struct.h>
#include <hal/mac/reg.h>
#include <hal/mac/mem.h>
#include <hal/common/halctrl.h>
#include <dal/longan/dal_longan_rate.h>
#include <rtk/default.h>
#include <rtk/rate.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
static uint32               rate_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};
static osal_mutex_t         rate_sem[RTK_MAX_NUM_OF_UNIT];

/*LONGAN SS-528*/
static uint32   **rate_db[RTK_MAX_NUM_OF_UNIT];

typedef struct rtk_rate_portAssuredMode_s
{
    rtk_rate_assuredMode_t queueRateMode[12];
} rtk_rate_portAssuredMode_t;

static rtk_rate_portAssuredMode_t egrPortAssuedRateMode[RTK_MAX_NUM_OF_UNIT][28];

typedef struct rtk_rate_egress_rate_s
{
    uint32 port_rate;
    uint32 queue_maxRate[12];
    uint32 queue_assureRate[12];
}rtk_rate_egress_rate_t;

static rtk_rate_egress_rate_t egr_rate[RTK_MAX_NUM_OF_UNIT][28];
typedef enum rtk_rate_e
{
    RTK_RATE_SPEED_10M,
    RTK_RATE_SPEED_100M,
    RTK_RATE_SPEED_500M,
    RTK_RATE_SPEED_1G,
    RTK_RATE_SPEED_1250M,
    RTK_RATE_SPEED_2500M,
    RTK_RATE_SPEED_5G,
    RTK_RATE_SPEED_10G,
    RTK_RATE_SPEED_13G,
    RTK_RATE_SPEED_END
}rtk_rate_t;

#define RTK_SPEED_10M_VAL_IN_UNIT_OF_16Kbps     (0x271)
#define RTK_SPEED_100M_VAL_IN_UNIT_OF_16Kbps     (0x186a)
#define RTK_SPEED_500M_VAL_IN_UNIT_OF_16Kbps     (0x7a12)
#define RTK_SPEED_1G_VAL_IN_UNIT_OF_16Kbps         (0xf424)
#define RTK_SPEED_1250M_VAL_IN_UNIT_OF_16Kbps     (0x1312d)
#define RTK_SPEED_2500M_VAL_IN_UNIT_OF_16Kbps     (0x2625a)
#define RTK_SPEED_5G_VAL_IN_UNIT_OF_16Kbps         (0x4c4b4)
#define RTK_SPEED_10G_VAL_IN_UNIT_OF_16Kbps     (0x98968)
#define RTK_SPEED_13G_VAL_IN_UNIT_OF_16Kbps     (0xc65d4)


typedef struct rtk_rate_rateVal_s
{
    uint32 reg;
    uint32 field;
    uint32 rateVal;
}rtk_rate_rateVal_t;

#define RTK_RATE_EGREE_ADJUST 1096
static rtk_rate_rateVal_t rateVal[RTK_RATE_SPEED_END] = {
    {LONGAN_EGBW_RATE_10M_CTRLr,    LONGAN_RATEf,    RTK_SPEED_10M_VAL_IN_UNIT_OF_16Kbps},
    {LONGAN_EGBW_RATE_100M_CTRLr,    LONGAN_RATEf,    RTK_SPEED_100M_VAL_IN_UNIT_OF_16Kbps},
    {LONGAN_EGBW_RATE_500M_CTRLr,    LONGAN_RATEf,    RTK_SPEED_500M_VAL_IN_UNIT_OF_16Kbps},
    {LONGAN_EGBW_RATE_1G_CTRLr,        LONGAN_RATEf,    RTK_SPEED_1G_VAL_IN_UNIT_OF_16Kbps},
    {LONGAN_EGBW_RATE_1250M_CTRLr,    LONGAN_RATEf,    RTK_SPEED_1250M_VAL_IN_UNIT_OF_16Kbps},
    {LONGAN_EGBW_RATE_2500M_CTRLr,    LONGAN_RATEf,    RTK_SPEED_2500M_VAL_IN_UNIT_OF_16Kbps},
    {LONGAN_EGBW_RATE_5G_CTRLr,        LONGAN_RATEf,    RTK_SPEED_5G_VAL_IN_UNIT_OF_16Kbps},
    {LONGAN_EGBW_RATE_10G_CTRLr,    LONGAN_RATEf,    RTK_SPEED_10G_VAL_IN_UNIT_OF_16Kbps},
    {LONGAN_EGBW_RATE_SXG_CTRLr,    LONGAN_RATEf,    RTK_SPEED_13G_VAL_IN_UNIT_OF_16Kbps}
};


#define RTK_RATE_EGREE_RATE_LIMIT_ADJUST_REG LONGAN_DMY_REG1_EGRESS_CTRLr


#define RTK_LONGAN_EGR_RATE_MAX 0xfffff

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

/*LONGAN SS-528*/
#define RATE_MODE_COUNT     3
#define RATE_TO_DIV     1014
#define ROUND_CRCT      507
#define RATE_TO_MUL     1000
#define RATE_DEFAULT    0xFFFFF

/*
 * Function Declaration
 */
static int32 _dal_longan_rate_egressRate_adjust(uint32 unit, uint32 rate, uint32 *pAdjustRate)
{
    int32 ret = RT_ERR_OK;
    uint32 value;
    uint32 adjust;

    RT_PARAM_CHK((NULL == pAdjustRate), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(unit, LONGAN_EGBW_CTRLr, LONGAN_INC_IFGf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    if(value == 1)
    {
        if ((ret = reg_read(unit, RTK_RATE_EGREE_RATE_LIMIT_ADJUST_REG, &adjust)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }

        *pAdjustRate = adjust*rate/1000 + rate;
    }
    else
        *pAdjustRate = rate;

    if(*pAdjustRate > RTK_LONGAN_EGR_RATE_MAX)
        *pAdjustRate = RTK_LONGAN_EGR_RATE_MAX;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_rateMapper_init
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
dal_longan_rateMapper_init(dal_mapper_t *pMapper)
{
    /* rate function */
    pMapper->rate_init = dal_longan_rate_init;
    pMapper->rate_includeIfg_get = dal_longan_rate_includeIfg_get;
    pMapper->rate_includeIfg_set = dal_longan_rate_includeIfg_set;
    pMapper->rate_portIgrBwCtrlEnable_get = dal_longan_rate_portIgrBwCtrlEnable_get;
    pMapper->rate_portIgrBwCtrlEnable_set = dal_longan_rate_portIgrBwCtrlEnable_set;
    pMapper->rate_portIgrBwCtrlRate_get = dal_longan_rate_portIgrBwCtrlRate_get;
    pMapper->rate_portIgrBwCtrlRate_set = dal_longan_rate_portIgrBwCtrlRate_set;
    pMapper->rate_igrBandwidthLowThresh_get = dal_longan_rate_igrBandwidthLowThresh_get;
    pMapper->rate_igrBandwidthLowThresh_set = dal_longan_rate_igrBandwidthLowThresh_set;
    pMapper->rate_igrBwCtrlBurstSize_get = dal_longan_rate_igrBwCtrlBurstSize_get;
    pMapper->rate_igrBwCtrlBurstSize_set = dal_longan_rate_igrBwCtrlBurstSize_set;
    pMapper->rate_igrPortBwCtrlBurstSize_get = dal_longan_rate_igrPortBwCtrlBurstSize_get;
    pMapper->rate_igrPortBwCtrlBurstSize_set = dal_longan_rate_igrPortBwCtrlBurstSize_set;
    pMapper->rate_portIgrBwCtrlBurstSize_get = dal_longan_rate_portIgrBwCtrlBurstSize_get;
    pMapper->rate_portIgrBwCtrlBurstSize_set = dal_longan_rate_portIgrBwCtrlBurstSize_set;
    pMapper->rate_portIgrBandwidthCtrlExceed_get = dal_longan_rate_portIgrBandwidthCtrlExceed_get;
    pMapper->rate_portIgrBandwidthCtrlExceed_reset = dal_longan_rate_portIgrBandwidthCtrlExceed_reset;
    pMapper->rate_igrBandwidthCtrlBypass_get = dal_longan_rate_igrBandwidthCtrlBypass_get;
    pMapper->rate_igrBandwidthCtrlBypass_set = dal_longan_rate_igrBandwidthCtrlBypass_set;
    pMapper->rate_portIgrBwFlowctrlEnable_get = dal_longan_rate_portIgrBwFlowctrlEnable_get;
    pMapper->rate_portIgrBwFlowctrlEnable_set = dal_longan_rate_portIgrBwFlowctrlEnable_set;
    pMapper->rate_portEgrBwCtrlEnable_get = dal_longan_rate_portEgrBwCtrlEnable_get;
    pMapper->rate_portEgrBwCtrlEnable_set = dal_longan_rate_portEgrBwCtrlEnable_set;
    pMapper->rate_portEgrBwCtrlRate_get = dal_longan_rate_portEgrBwCtrlRate_get;
    pMapper->rate_portEgrBwCtrlRate_set = dal_longan_rate_portEgrBwCtrlRate_set;
    pMapper->rate_portEgrBwCtrlBurstSize_get = dal_longan_rate_portEgrBwCtrlBurstSize_get;
    pMapper->rate_portEgrBwCtrlBurstSize_set = dal_longan_rate_portEgrBwCtrlBurstSize_set;
    pMapper->rate_cpuEgrBandwidthCtrlRateMode_get = dal_longan_rate_cpuEgrBandwidthCtrlRateMode_get;
    pMapper->rate_cpuEgrBandwidthCtrlRateMode_set = dal_longan_rate_cpuEgrBandwidthCtrlRateMode_set;
    pMapper->rate_egrPortBwCtrlBurstSize_get = dal_longan_rate_egrPortBwCtrlBurstSize_get;
    pMapper->rate_egrPortBwCtrlBurstSize_set = dal_longan_rate_egrPortBwCtrlBurstSize_set;
    pMapper->rate_portEgrQueueBwCtrlEnable_get = dal_longan_rate_portEgrQueueBwCtrlEnable_get;
    pMapper->rate_portEgrQueueBwCtrlEnable_set = dal_longan_rate_portEgrQueueBwCtrlEnable_set;
    pMapper->rate_portEgrQueueBwCtrlRate_get = dal_longan_rate_portEgrQueueBwCtrlRate_get;
    pMapper->rate_portEgrQueueBwCtrlRate_set = dal_longan_rate_portEgrQueueBwCtrlRate_set;
    pMapper->rate_portEgrQueueBwCtrlBurstSize_get = dal_longan_rate_portEgrQueueBwCtrlBurstSize_get;
    pMapper->rate_portEgrQueueBwCtrlBurstSize_set = dal_longan_rate_portEgrQueueBwCtrlBurstSize_set;
    pMapper->rate_portEgrQueueAssuredBwCtrlEnable_get = dal_longan_rate_portEgrQueueAssuredBwCtrlEnable_get;
    pMapper->rate_portEgrQueueAssuredBwCtrlEnable_set = dal_longan_rate_portEgrQueueAssuredBwCtrlEnable_set;
    pMapper->rate_portEgrQueueAssuredBwCtrlRate_get = dal_longan_rate_portEgrQueueAssuredBwCtrlRate_get;
    pMapper->rate_portEgrQueueAssuredBwCtrlRate_set = dal_longan_rate_portEgrQueueAssuredBwCtrlRate_set;
    pMapper->rate_portEgrQueueAssuredBwCtrlBurstSize_get = dal_longan_rate_portEgrQueueAssuredBwCtrlBurstSize_get;
    pMapper->rate_portEgrQueueAssuredBwCtrlBurstSize_set = dal_longan_rate_portEgrQueueAssuredBwCtrlBurstSize_set;
    pMapper->rate_portEgrQueueAssuredBwCtrlMode_get = dal_longan_rate_portEgrQueueAssuredBwCtrlMode_get;
    pMapper->rate_portEgrQueueAssuredBwCtrlMode_set = dal_longan_rate_portEgrQueueAssuredBwCtrlMode_set;
    pMapper->rate_egrQueueBwCtrlBurstSize_get = dal_longan_rate_egrQueueBwCtrlBurstSize_get;
    pMapper->rate_egrQueueBwCtrlBurstSize_set = dal_longan_rate_egrQueueBwCtrlBurstSize_set;
    pMapper->rate_portStormCtrlEnable_get = dal_longan_rate_portStormCtrlEnable_get;
    pMapper->rate_portStormCtrlEnable_set = dal_longan_rate_portStormCtrlEnable_set;
    pMapper->rate_portStormCtrlRate_get = dal_longan_rate_portStormCtrlRate_get;
    pMapper->rate_portStormCtrlRate_set = dal_longan_rate_portStormCtrlRate_set;
    pMapper->rate_stormControlBurstSize_get = dal_longan_rate_stormControlBurstSize_get;
    pMapper->rate_stormControlBurstSize_set = dal_longan_rate_stormControlBurstSize_set;
    pMapper->rate_portStormCtrlBurstSize_get = dal_longan_rate_portStormCtrlBurstSize_get;
    pMapper->rate_portStormCtrlBurstSize_set = dal_longan_rate_portStormCtrlBurstSize_set;
    pMapper->rate_portStormCtrlExceed_get = dal_longan_rate_portStormCtrlExceed_get;
    pMapper->rate_portStormCtrlExceed_reset = dal_longan_rate_portStormCtrlExceed_reset;
    pMapper->rate_stormControlRateMode_get = dal_longan_rate_stormControlRateMode_get;
    pMapper->rate_stormControlRateMode_set = dal_longan_rate_stormControlRateMode_set;
    pMapper->rate_portStormCtrlRateMode_get = dal_longan_rate_portStormCtrlRateMode_get;
    pMapper->rate_portStormCtrlRateMode_set = dal_longan_rate_portStormCtrlRateMode_set;
    pMapper->rate_portStormCtrlTypeSel_get = dal_longan_rate_portStormCtrlTypeSel_get;
    pMapper->rate_portStormCtrlTypeSel_set = dal_longan_rate_portStormCtrlTypeSel_set;
    pMapper->rate_stormControlBypass_get = dal_longan_rate_stormControlBypass_get;
    pMapper->rate_stormControlBypass_set = dal_longan_rate_stormControlBypass_set;
    pMapper->rate_portStormCtrlProtoEnable_get =  dal_longan_rate_portStormCtrlProtoEnable_get;
    pMapper->rate_portStormCtrlProtoEnable_set =  dal_longan_rate_portStormCtrlProtoEnable_set;
    pMapper->rate_portStormCtrlProtoRate_get =  dal_longan_rate_portStormCtrlProtoRate_get;
    pMapper->rate_portStormCtrlProtoRate_set =  dal_longan_rate_portStormCtrlProtoRate_set;
    pMapper->rate_portStormCtrlProtoBurstSize_get = dal_longan_rate_portStormCtrlProtoBurstSize_get;
    pMapper->rate_portStormCtrlProtoBurstSize_set = dal_longan_rate_portStormCtrlProtoBurstSize_set;
    pMapper->rate_portStormCtrlProtoExceed_get = dal_longan_rate_portStormCtrlProtoExceed_get;
    pMapper->rate_portStormCtrlProtoExceed_reset = dal_longan_rate_portStormCtrlProtoExceed_reset;
    pMapper->rate_stormCtrlProtoVlanConstrtEnable_get = dal_longan_rate_stormCtrlProtoVlanConstrtEnable_get;
    pMapper->rate_stormCtrlProtoVlanConstrtEnable_set = dal_longan_rate_stormCtrlProtoVlanConstrtEnable_set;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_rate_init
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
 * Note:
 *      Must initialize rate module before calling any rate APIs.
 */
int32
dal_longan_rate_init(uint32 unit)
{
    int32 ret;
    uint32 port, queue;
    uint32 value;
    uint32  mode_idx;


    RT_INIT_REENTRY_CHK(rate_init[unit]);
    rate_init[unit] = INIT_NOT_COMPLETED;

    /* create semaphore */
    rate_sem[unit] = osal_sem_mutex_create();
    if (0 == rate_sem[unit])
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_RATE), "semaphore create failed");
        return RT_ERR_FAILED;
    }

    if (HWP_UNIT_VALID_LOCAL(unit))
    {
        RATE_SEM_LOCK(unit);

        for(port = 0; port < 28; port++)
        {
            egr_rate[unit][port].port_rate = RTK_LONGAN_EGR_RATE_MAX;
            for(queue = 0; queue < 12; queue++)
            {
                if (port < 24 && queue > 7)
                    continue;

                egrPortAssuedRateMode[unit][port].queueRateMode[queue] = ASSURED_MODE_SHARE;

                egr_rate[unit][port].queue_maxRate[queue]         = RTK_LONGAN_EGR_RATE_MAX;
                egr_rate[unit][port].queue_assureRate[queue]    = RTK_LONGAN_EGR_RATE_MAX;
                value=0;

                if((ret = reg_array_field_write(unit, (port>=24 ? LONGAN_EGBW_PORT_Q_ASSURED_LB_CTRL_SET1r : LONGAN_EGBW_PORT_Q_ASSURED_LB_CTRL_SET0r),    port, queue, LONGAN_ENf, &value)) != RT_ERR_OK)
                {
                    RATE_SEM_UNLOCK(unit);
                    RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
                    return ret;
                }
                if((ret = reg_array_field_write(unit, (port>=24 ? LONGAN_EGBW_PORT_Q_FIX_LB_CTRL_SET1r : LONGAN_EGBW_PORT_Q_FIX_LB_CTRL_SET0r),    port, queue, LONGAN_ENf, &value)) != RT_ERR_OK)
                {
                    RATE_SEM_UNLOCK(unit);
                    RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
                    return ret;
                }
            }
        }
        /*LONGAN SS-528*/
        if (NULL == rate_db[unit])
        {
            rate_db[unit] = (uint32 **)osal_alloc(RATE_MODE_COUNT * sizeof(uint32*));
            if (NULL == rate_db[unit])
            {
                RT_ERR(RT_ERR_FAILED, (MOD_RATE|MOD_DAL), "memory allocate failed");
                goto err_free;
            }
            else
            {
                osal_memset(rate_db[unit], 0, sizeof(RATE_MODE_COUNT * sizeof(uint32*)));
            }
            for (mode_idx = 0; mode_idx < RATE_MODE_COUNT; mode_idx++)
            {
                rate_db[unit][mode_idx] = (uint32*)osal_alloc((UNITMAP(unit)->hwp_maxMacId + 1) * sizeof(uint32));
                if (NULL == rate_db[unit][mode_idx])
                {
                    RT_ERR(RT_ERR_FAILED, (MOD_RATE|MOD_DAL), "memory allocate failed");
                    goto err_free;
                }
                else
                {
                    for (port = 0; port < (UNITMAP(unit)->hwp_maxMacId + 1); port++)
                    {
                        rate_db[unit][mode_idx][port] = RATE_DEFAULT;
                    }
                }
            }

        }


        RATE_SEM_UNLOCK(unit);
    }//HWP_UNIT_VALID_LOCAL(unit)

    /* set init flag to complete init */
    /* http://172.21.12.139:8080/query/defects.htm?projectId=10002&cid=30386 */
    if(HWP_UNIT_VALID_IN_RANGE(unit))
    {
        rate_init[unit] = INIT_COMPLETED;
    }

    return RT_ERR_OK;

err_free:
    for (mode_idx = 0; mode_idx < RATE_MODE_COUNT; mode_idx++)
    {
        if (NULL != rate_db[unit][mode_idx])
        {
            osal_free(rate_db[unit][mode_idx]);
        }
    }

    if (NULL != rate_db[unit])
        osal_free(rate_db[unit]);


    RT_ERR(RT_ERR_FAILED, (MOD_TRUNK|MOD_DAL), "init default configuration failed");


    RATE_SEM_UNLOCK(unit);
    if(HWP_UNIT_VALID_IN_RANGE(unit))
    {
        rate_init[unit] = INIT_NOT_COMPLETED;
    }
    return RT_ERR_FAILED;


}

/* Module Name    : Rate                                            */
/* Sub-module Name: Configuration of ingress port bandwidth control */

/* Function Name:
 *      dal_longan_rate_includeIfg_get
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
dal_longan_rate_includeIfg_get(uint32 unit, rtk_rate_module_t module, rtk_enable_t *pIfg_include)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value;
    uint32 reg;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, module", unit, module);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIfg_include), RT_ERR_NULL_POINTER);

    switch(module)
    {
        case RATE_MODULE_IGR:
            reg = LONGAN_IGBW_CTRLr;break;
        case RATE_MODULE_EGR:
           reg = LONGAN_EGBW_CTRLr;break;
        case RATE_MODULE_STORM:
            reg = LONGAN_STORM_CTRLr;break;
        default:
            return RT_ERR_INPUT;
    }

    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_field_read(unit, reg, LONGAN_INC_IFGf, &value)) != RT_ERR_OK)
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
}

/* Function Name:
 *      dal_longan_rate_includeIfg_set
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
dal_longan_rate_includeIfg_set(uint32 unit, rtk_rate_module_t module, rtk_enable_t ifg_include)
{
    int32    ret = RT_ERR_FAILED;
    uint32 i;
    uint32    value, orig_val;
    uint32 reg;
    uint32 port, queue;
    uint32 rateAdjust;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, module=%d, enable=%d\n", unit, module, ifg_include);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((ifg_include >= RTK_ENABLE_END), RT_ERR_INPUT);

    switch(module)
    {
        case RATE_MODULE_IGR:
            reg = LONGAN_IGBW_CTRLr;break;
        case RATE_MODULE_EGR:
           reg = LONGAN_EGBW_CTRLr;break;
        case RATE_MODULE_STORM:
            reg = LONGAN_STORM_CTRLr;break;
        default:
            return RT_ERR_INPUT;
    }

    if (ifg_include == ENABLED)
        value = 1;
    else
        value = 0;

    RATE_SEM_LOCK(unit);

    /*ss-824*/
    if ((ret = reg_field_read(unit, reg, LONGAN_INC_IFGf, &orig_val)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    if(orig_val==value)
    {
        RATE_SEM_UNLOCK(unit);
        return RT_ERR_OK;
    }

    /* write value to CHIP*/
    if ((ret = reg_field_write(unit, reg, LONGAN_INC_IFGf, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    if(module==RATE_MODULE_EGR)
    {
        for(i=0; i<RTK_RATE_SPEED_END; i++)
        {
            value = ifg_include==ENABLED ? rateVal[i].rateVal*RTK_RATE_EGREE_ADJUST/1000 : rateVal[i].rateVal;
            if ((ret = reg_field_write(unit, rateVal[i].reg, rateVal[i].field, &value)) != RT_ERR_OK)
            {
                RATE_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
                return ret;
            }
        }

        HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
        {
            if ((ret = _dal_longan_rate_egressRate_adjust(unit, egr_rate[unit][port].port_rate, &rateAdjust)) != RT_ERR_OK)
            {
                RATE_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
                return ret;
            }
            if ((ret = reg_array_field_write(unit, LONGAN_EGBW_PORT_CTRLr, port, REG_ARRAY_INDEX_NONE, LONGAN_RATEf, &rateAdjust)) != RT_ERR_OK)
            {
                RATE_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
                return ret;
            }

            for(queue = 0; queue < 12; queue++)
            {
                if (port < 24 && queue > 7)
                    continue;

                if ((ret = _dal_longan_rate_egressRate_adjust(unit, egr_rate[unit][port].queue_maxRate[queue], &rateAdjust)) != RT_ERR_OK)
                {
                    RATE_SEM_UNLOCK(unit);
                    RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
                    return ret;
                }

                reg = (HWP_UPLINK_PORT(unit, port) ? LONGAN_EGBW_PORT_Q_MAX_LB_CTRL_SET1r : LONGAN_EGBW_PORT_Q_MAX_LB_CTRL_SET0r);
                if ((ret = reg_array_field_write(unit, reg,  port, queue, LONGAN_RATEf, &rateAdjust)) != RT_ERR_OK)
                {
                    RATE_SEM_UNLOCK(unit);
                    RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
                    return ret;
                }


                if ((ret = _dal_longan_rate_egressRate_adjust(unit, egr_rate[unit][port].queue_assureRate[queue], &rateAdjust)) != RT_ERR_OK)
                {
                    RATE_SEM_UNLOCK(unit);
                    RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
                    return ret;
                }
                reg = (HWP_UPLINK_PORT(unit, port) ? LONGAN_EGBW_PORT_Q_ASSURED_LB_CTRL_SET1r : LONGAN_EGBW_PORT_Q_ASSURED_LB_CTRL_SET0r);
                if ((ret = reg_array_field_write(unit, reg,  port, queue, LONGAN_RATEf, &rateAdjust)) != RT_ERR_OK)
                {
                    RATE_SEM_UNLOCK(unit);
                    RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
                    return ret;
                }
                reg = (HWP_UPLINK_PORT(unit, port) ? LONGAN_EGBW_PORT_Q_FIX_LB_CTRL_SET1r : LONGAN_EGBW_PORT_Q_FIX_LB_CTRL_SET0r);
                if ((ret = reg_array_field_write(unit, reg,  port, queue, LONGAN_RATEf, &rateAdjust)) != RT_ERR_OK)
                {
                    RATE_SEM_UNLOCK(unit);
                    RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
                    return ret;
                }
            }
        }
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_longan_rate_portIgrBwCtrlEnable_get
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
 * Note:
 *      The status of ingress bandwidth control is as following:
 *      - DISABLED
 *      - ENABLED
 */
int32
dal_longan_rate_portIgrBwCtrlEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d\n", unit, port);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_array_field_read(unit, LONGAN_IGBW_PORT_CTRLr, port, REG_ARRAY_INDEX_NONE, LONGAN_BW_ENf, &value)) != RT_ERR_OK)
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
}

/* Function Name:
 *      dal_longan_rate_portIgrBwCtrlEnable_set
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
 * Note:
 *      The status of ingress bandwidth control is as following:
 *      - DISABLED
 *      - ENABLED
 */
int32
dal_longan_rate_portIgrBwCtrlEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret = RT_ERR_FAILED;
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
    if ((ret = reg_array_field_write(unit, LONGAN_IGBW_PORT_CTRLr, port, REG_ARRAY_INDEX_NONE, LONGAN_BW_ENf, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_rate_portIgrBwCtrlRate_get
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
 * Note:
 *      (1) The actual rate is "rate * chip granularity".
 *      (2) The unit of granularity is 16Kbps.
 */
int32
dal_longan_rate_portIgrBwCtrlRate_get(uint32 unit, rtk_port_t port, uint32 *pRate)
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
    if ((ret = reg_array_field_read(unit, LONGAN_IGBW_PORT_CTRLr, port, REG_ARRAY_INDEX_NONE, LONGAN_RATEf, pRate)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_rate_portIgrBwCtrlRate_set
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
 * Note:
 *      (1) The actual rate is "rate * chip granularity".
 *      (2) The unit of granularity is 16Kbps.
 */
int32
dal_longan_rate_portIgrBwCtrlRate_set(uint32 unit, rtk_port_t port, uint32 rate)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d\n", unit, port);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((rate > HAL_RATE_OF_BANDWIDTH_MAX(unit)), RT_ERR_RATE);

    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_array_field_write(unit, LONGAN_IGBW_PORT_CTRLr, port, REG_ARRAY_INDEX_NONE, LONGAN_RATEf, &rate)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_rate_igrBandwidthLowThresh_get
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
 * Note:
 *      The threshold unit is 128 Byte.
 */
int32
dal_longan_rate_igrBandwidthLowThresh_get(uint32 unit, uint32 *pLowThresh)
{
    int32 ret = RT_ERR_FAILED;
    rtk_port_t  port;
    rtk_rate_igrBwBurst_cfg_t cfg;

    RT_PARAM_CHK((NULL == pLowThresh), RT_ERR_NULL_POINTER);

    HWP_PORT_TRAVS(unit, port)
    {
        if(RT_ERR_OK != (ret = dal_longan_rate_portIgrBwCtrlBurstSize_get(unit, port, &cfg)))
            return ret;

        *pLowThresh = cfg.burst_low;
        return RT_ERR_OK;
    }

    return RT_ERR_FAILED;
}

/* Function Name:
 *      rtk__rate_igrBandwidthLowThresh_set
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
 * Note:
 *      The threshold unit is 128 Byte.
 *      After setting the API, please adjust the low threshold by rtk_rate_igrBwCtrlBurstSize_set.
 */
int32
dal_longan_rate_igrBandwidthLowThresh_set(uint32 unit, uint32 lowThresh)
{
    int32 ret = RT_ERR_FAILED;
    rtk_port_t port;
    rtk_rate_igrBwBurst_cfg_t cfg;

    HWP_PORT_TRAVS(unit, port)
    {
        if(RT_ERR_OK != (ret = dal_longan_rate_portIgrBwCtrlBurstSize_get(unit, port, &cfg)))
            return ret;

        cfg.burst_low= lowThresh;

        if(RT_ERR_OK != (ret = dal_longan_rate_portIgrBwCtrlBurstSize_set(unit, port, &cfg)))
            return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_rate_igrBwCtrlBurstSize_get
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
 * Note:
 *      The burst size unit is 128 Byte.
 */
int32
dal_longan_rate_igrBwCtrlBurstSize_get(uint32 unit, uint32 *pBurst_size)
{
    return dal_longan_rate_igrPortBwCtrlBurstSize_get(unit, pBurst_size);
}

/* Function Name:
 *      dal_longan_rate_igrBwCtrlBurstSize_set
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
 * Note:
 *      The burst size unit is 128 Byte.
 *      The (minimum, maximum) burst_size setting range is (22, 65535).
 *      After setting the API, please adjust the low threshold by rtk_rate_igrBandwidthLowThresh_set.
 */
int32
dal_longan_rate_igrBwCtrlBurstSize_set(uint32 unit, uint32 burst_size)
{
    return dal_longan_rate_igrPortBwCtrlBurstSize_set(unit, burst_size);
}

/* Function Name:
 *      dal_longan_rate_igrPortBwCtrlBurstSize_get
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
dal_longan_rate_igrPortBwCtrlBurstSize_get(
    uint32                  unit,
    uint32                  *pBurst_size)
{
    int32 ret = RT_ERR_FAILED;
    rtk_port_t      port;
    rtk_rate_igrBwBurst_cfg_t cfg;

    RT_PARAM_CHK((NULL == pBurst_size), RT_ERR_NULL_POINTER);

    HWP_PORT_TRAVS(unit, port)
    {
        if(RT_ERR_OK != (ret = dal_longan_rate_portIgrBwCtrlBurstSize_get(unit, port, &cfg)))
            return ret;

        *pBurst_size = cfg.burst_high;
        return RT_ERR_OK;
    }

    return RT_ERR_FAILED;
}

/* Function Name:
 *      dal_longan_rate_igrPortBwCtrlBurstSize_set
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
dal_longan_rate_igrPortBwCtrlBurstSize_set(
    uint32                  unit,
    uint32                  burst_size)
{
    int32 ret = RT_ERR_FAILED;
    rtk_port_t port;
    rtk_rate_igrBwBurst_cfg_t cfg;

    HWP_PORT_TRAVS(unit, port)
    {
        if(RT_ERR_OK != (ret = dal_longan_rate_portIgrBwCtrlBurstSize_get(unit, port, &cfg)))
            return ret;

        cfg.burst_low= burst_size;
        cfg.burst_high = burst_size;

        if(RT_ERR_OK != (ret = dal_longan_rate_portIgrBwCtrlBurstSize_set(unit, port, &cfg)))
            return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_rate_portIgrBwCtrlBurstSize_get
 * Description:
 *      Get the ingress bandwidth control burst.
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
 * Note:
 *      None
 */
int32
dal_longan_rate_portIgrBwCtrlBurstSize_get(uint32 unit, rtk_port_t port, rtk_rate_igrBwBurst_cfg_t *pCfg)
{
    int32   ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d\n", unit, port);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pCfg), RT_ERR_NULL_POINTER);

    RATE_SEM_LOCK(unit);
    /* read value from CHIP*/
    if ((ret = reg_array_field_read(unit, LONGAN_IGBW_PORT_BURST_CTRLr, port, REG_ARRAY_INDEX_NONE, LONGAN_HIGH_ONf, &pCfg->burst_high)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    if ((ret = reg_array_field_read(unit, LONGAN_IGBW_PORT_BURST_CTRLr, port, REG_ARRAY_INDEX_NONE, LONGAN_HIGH_OFFf, &pCfg->burst_low)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_rate_portIgrBwCtrlBurstSize_set
 * Description:
 *      Set the ingress bandwidth control burst.
 * Input:
 *      unit - unit id
 *      port - port id
 *      pCfg - ingress bandwidth control burst
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - Invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - Invalid port id
 *      RT_ERR_RATE     - Invalid input rate
 * Note:
 *      None
 */
int32
dal_longan_rate_portIgrBwCtrlBurstSize_set(uint32 unit, rtk_port_t port, rtk_rate_igrBwBurst_cfg_t *pCfg)
{
    int32   ret = RT_ERR_FAILED;
    uint32 ibc_token;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d\n", unit, port);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pCfg), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pCfg->burst_high> HAL_BURST_OF_IGR_BANDWIDTH_MAX(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK((pCfg->burst_low > HAL_BURST_OF_IGR_BANDWIDTH_MAX(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK((pCfg->burst_high < pCfg->burst_low), RT_ERR_INPUT);

    RATE_SEM_LOCK(unit);

    if ((ret = reg_field_read(unit, LONGAN_IGBW_LB_CTRLr,LONGAN_TKNf, &ibc_token)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }
    RATE_SEM_UNLOCK(unit);

    RT_PARAM_CHK((pCfg->burst_high < ( 3*ibc_token)), RT_ERR_INPUT);
    RT_PARAM_CHK((pCfg->burst_low < ( 3*ibc_token)), RT_ERR_INPUT);

    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_array_field_write(unit, LONGAN_IGBW_PORT_BURST_CTRLr, port, REG_ARRAY_INDEX_NONE, LONGAN_HIGH_ONf, &pCfg->burst_high)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit, LONGAN_IGBW_PORT_BURST_CTRLr, port, REG_ARRAY_INDEX_NONE, LONGAN_HIGH_OFFf, &pCfg->burst_low)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_rate_portIgrBandwidthCtrlExceed_get
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
 * Note:
 *      Exceed status is as following
 *      - TRUE      - flow control high on threshold ever exceeds.
 *      - FALSE     - flow control high on threshold never exceeds.
 */
int32
dal_longan_rate_portIgrBandwidthCtrlExceed_get(
    uint32                  unit,
    rtk_port_t              port,
    uint32                  *pIsExceed)
{
    int32   ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d\n", unit, port);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pIsExceed), RT_ERR_NULL_POINTER);

    RATE_SEM_LOCK(unit);

    /* write value to CHIP*/
    if ((ret = reg_array_field_read(unit, LONGAN_IGBW_PORT_EXCEED_FLAGr, port, REG_ARRAY_INDEX_NONE, LONGAN_FLAGf, pIsExceed)) != RT_ERR_OK)
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
 *      dal_longan_rate_portIgrBandwidthCtrlExceed_reset
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
 * Note:
 *      None
 */
int32
dal_longan_rate_portIgrBandwidthCtrlExceed_reset(
    uint32                  unit,
    rtk_port_t              port)
{
    int32   ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d\n", unit, port);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);

    RATE_SEM_LOCK(unit);

    /* write value to CHIP*/
    if ((ret = reg_array_field_write1toClear(unit, LONGAN_IGBW_PORT_EXCEED_FLAGr, port, REG_ARRAY_INDEX_NONE, LONGAN_FLAGf)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_longan_rate_igrBandwidthCtrlBypass_get
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
 * Note:
 *      If a certain packet type is selected to bypass the ingress bandwidth control, the packet will
 *      not be blocked by the ingress bandwidth control and will not consume the bandwidth.
 */
int32
dal_longan_rate_igrBandwidthCtrlBypass_get(uint32 unit, rtk_rate_igr_bypassType_t bypassType, rtk_enable_t  *pEnable)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value;
    uint32  regField = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((bypassType >= IGR_BYPASS_END), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    switch (bypassType)
    {
        case IGR_BYPASS_ARPREQ:
            regField = LONGAN_ADMIT_ARPREQf;
            break;
        case IGR_BYPASS_RMA:
            regField = LONGAN_ADMIT_RMAf;
            break;
        case IGR_BYPASS_BPDU:
            regField = LONGAN_ADMIT_BPDUf;
            break;
        case IGR_BYPASS_RTKPKT:
            regField = LONGAN_ADMIT_RTKPKTf;
            break;
        case IGR_BYPASS_IGMP:
            regField = LONGAN_ADMIT_IGMPf;
            break;
        case IGR_BYPASS_DHCP:
            regField = LONGAN_ADMIT_DHCPf;
            break;
        case IGR_BYPASS_RIP:
            regField = LONGAN_ADMIT_RIPf;
            break;
        default:
            return RT_ERR_INPUT;
    }

    RATE_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_field_read(unit, LONGAN_IGBW_CTRLr, regField, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
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

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_rate_igrBandwidthCtrlBypass_set
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
 * Note:
 *      If a certain packet type is selected to bypass the ingress bandwidth control, the packet will
 *      not be blocked by the ingress bandwidth control and will not consume the bandwidth.
 */
int32
dal_longan_rate_igrBandwidthCtrlBypass_set(uint32 unit, rtk_rate_igr_bypassType_t bypassType, rtk_enable_t enable)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value;
    uint32  regField = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((bypassType >= IGR_BYPASS_END), RT_ERR_INPUT);
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    switch (bypassType)
    {
        case IGR_BYPASS_ARPREQ:
            regField = LONGAN_ADMIT_ARPREQf;
            break;
        case IGR_BYPASS_RMA:
            regField = LONGAN_ADMIT_RMAf;
            break;
        case IGR_BYPASS_BPDU:
            regField = LONGAN_ADMIT_BPDUf;
            break;
        case IGR_BYPASS_RTKPKT:
            regField = LONGAN_ADMIT_RTKPKTf;
            break;
        case IGR_BYPASS_IGMP:
            regField = LONGAN_ADMIT_IGMPf;
            break;
        case IGR_BYPASS_DHCP:
            regField = LONGAN_ADMIT_DHCPf;
            break;
        case IGR_BYPASS_RIP:
            regField = LONGAN_ADMIT_RIPf;
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
    if ((ret = reg_field_write(unit, LONGAN_IGBW_CTRLr, regField, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }
    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_rate_igrBandwidthCtrlBypass_set */

/* Function Name:
 *      dal_longan_rate_portIgrBwFlowctrlEnable_get
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
 * Note:
 *      None
 */
int32
dal_longan_rate_portIgrBwFlowctrlEnable_get(
    uint32              unit,
    rtk_port_t          port,
    rtk_enable_t        *pEnable)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d\n", unit, port);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_array_field_read(unit, LONGAN_IGBW_PORT_FC_CTRLr, port, REG_ARRAY_INDEX_NONE, LONGAN_ENf, &value)) != RT_ERR_OK)
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
}

/* Function Name:
 *      dal_longan_rate_portIgrBwFlowctrlEnable_set
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
 * Note:
 *      None
 */
int32
dal_longan_rate_portIgrBwFlowctrlEnable_set(
    uint32              unit,
    rtk_port_t          port,
    rtk_enable_t        enable)
{
    int32   ret = RT_ERR_FAILED;
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
    if ((ret = reg_array_field_write(unit, LONGAN_IGBW_PORT_FC_CTRLr, port, REG_ARRAY_INDEX_NONE, LONGAN_ENf, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Module Name    : Rate                                           */
/* Sub-module Name: Configuration of egress port bandwidth control */

/* Function Name:
 *      dal_longan_rate_portEgrBwCtrlEnable_get
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
 * Note:
 *      The status of egress bandwidth control is as following:
 *      - DISABLED
 *      - ENABLED
 */
int32
dal_longan_rate_portEgrBwCtrlEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d\n", unit, port);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_array_field_read(unit, LONGAN_EGBW_PORT_CTRLr, port, REG_ARRAY_INDEX_NONE, LONGAN_ENf, &value)) != RT_ERR_OK)
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
}

/* Function Name:
 *      dal_longan_rate_portEgrBwCtrlEnable_set
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
 * Note:
 *      The status of egress bandwidth control is as following:
 *      - DISABLED
 *      - ENABLED
 */
int32
dal_longan_rate_portEgrBwCtrlEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret = RT_ERR_FAILED;
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
    if ((ret = reg_array_field_write(unit, LONGAN_EGBW_PORT_CTRLr, port, REG_ARRAY_INDEX_NONE, LONGAN_ENf, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_rate_portEgrBwCtrlRate_get
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
 * Note:
 *      (1) The actual rate is "rate * chip granularity".
 *      (2) The unit of granularity is 16Kbps
 *      (3) The unit of granularity is 16Kbps or 1pps in CPU port depend on mode set by 'dal_longan_rate_cpuEgrBandwidthCtrlRateMode_set'.
 */
int32
dal_longan_rate_portEgrBwCtrlRate_get(uint32 unit, rtk_port_t port, uint32 *pRate)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d\n", unit, port);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pRate), RT_ERR_NULL_POINTER);

    if(!HWP_IS_CPU_PORT(unit, port))
    {
        *pRate = egr_rate[unit][port].port_rate;
    }
    else
    {
        RATE_SEM_LOCK(unit);

        /* read value from CHIP*/
        if ((ret = reg_array_field_read(unit, LONGAN_EGBW_PORT_CTRLr, port, REG_ARRAY_INDEX_NONE, LONGAN_RATEf, pRate)) != RT_ERR_OK)
        {
            RATE_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }

        RATE_SEM_UNLOCK(unit);
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_rate_portEgrBwCtrlRate_set
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
 * Note:
 *      (1) The actual rate is "rate * chip granularity".
 *      (2) The unit of granularity is 16Kbps.
 *      (3) The unit of granularity is 16Kbps or 1pps in CPU port depend on mode set by 'dal_longan_rate_cpuEgrBandwidthCtrlRateMode_set'.
 */
int32
dal_longan_rate_portEgrBwCtrlRate_set(uint32 unit, rtk_port_t port, uint32 rate)
{
    int32   ret;
    uint32  rateAdjust;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d\n", unit, port);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((rate > HAL_RATE_OF_BANDWIDTH_MAX(unit)), RT_ERR_RATE);

    RATE_SEM_LOCK(unit);

    if(!HWP_IS_CPU_PORT(unit, port))
    {
        if ((ret = _dal_longan_rate_egressRate_adjust(unit, rate, &rateAdjust)) != RT_ERR_OK)
        {
            RATE_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }
    }
    else
        rateAdjust = rate;

    /* read value from CHIP*/
    if ((ret = reg_array_field_write(unit, LONGAN_EGBW_PORT_CTRLr, port, REG_ARRAY_INDEX_NONE, LONGAN_RATEf, &rateAdjust)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }
    if(!HWP_IS_CPU_PORT(unit, port))
    {
        egr_rate[unit][port].port_rate = rate;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_rate_portEgrBwCtrlBurstSize_get
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
 * Note:
 *      None
 */
int32
dal_longan_rate_portEgrBwCtrlBurstSize_get(uint32 unit, rtk_port_t port, uint32 *pBurst_size)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d\n", unit, port);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pBurst_size), RT_ERR_NULL_POINTER);

    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_array_field_read(unit, LONGAN_EGBW_PORT_CTRLr, port, REG_ARRAY_INDEX_NONE, LONGAN_BURSTf, pBurst_size)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_rate_portEgrBwCtrlBurstSize_set
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
 * Note:
 *      None
 */
int32
dal_longan_rate_portEgrBwCtrlBurstSize_set(uint32 unit, rtk_port_t port, uint32 burst_size)
{
    int32   ret;
    uint32  ebc_token;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d\n", unit, port);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((burst_size > (HAL_BURST_OF_BANDWIDTH_MAX(unit) - HAL_MAX_ACCEPT_FRAME_LEN(unit))), RT_ERR_INPUT);

    RATE_SEM_LOCK(unit);

    if ((ret = reg_field_read(unit, LONGAN_EGBW_CTRLr,LONGAN_RATE_MODE_CPUf, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    if ((ret = reg_field_read(unit, LONGAN_EGBW_LB_CTRLr,LONGAN_TKNf, &ebc_token)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    if(!(HWP_IS_CPU_PORT(unit, port) && value==0))
    {
        RT_PARAM_CHK((burst_size < ( 3*ebc_token)), RT_ERR_INPUT);
    }

    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_array_field_write(unit, LONGAN_EGBW_PORT_CTRLr, port, REG_ARRAY_INDEX_NONE, LONGAN_BURSTf, &burst_size)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_rate_cpuEgrBandwidthCtrlRateMode_get
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
 * Note:
 *      The rate mode are as following:
 *      - RATE_MODE_BYTE
 *      - RATE_MODE_PKT
 */
int32
dal_longan_rate_cpuEgrBandwidthCtrlRateMode_get(uint32 unit, rtk_rate_rateMode_t *pRate_mode)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d\n", unit);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pRate_mode), RT_ERR_NULL_POINTER);

    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_field_read(unit, LONGAN_EGBW_CTRLr, LONGAN_RATE_MODE_CPUf, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    if(value == 1)
        *pRate_mode = RATE_MODE_BYTE;
    else
        *pRate_mode = RATE_MODE_PKT;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "pRate_mode=%d", *pRate_mode);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_rate_cpuEgrBandwidthCtrlRateMode_set
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
 * Note:
 *      The rate mode are as following:
 *      - RATE_MODE_BYTE
 *      - RATE_MODE_PKT
 */
int32
dal_longan_rate_cpuEgrBandwidthCtrlRateMode_set(uint32 unit, rtk_rate_rateMode_t rate_mode)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value = 0;
    uint32  burst_size = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, rate_mode=%d\n", unit, rate_mode);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((rate_mode >= RATE_MODE_END), RT_ERR_INPUT);

    if(rate_mode == RATE_MODE_BYTE)
    {
        value = 1;
        burst_size = LONGAN_EGRBW_CPU_BPS_DFLT_BURST_SIZE;
    }
    else
    {
        value = 0;
        burst_size = LONGAN_EGRBW_CPU_PPS_DFLT_BURST_SIZE;
    }

    RATE_SEM_LOCK(unit);

    /* write value to CHIP*/
    if ((ret = reg_field_write(unit, LONGAN_EGBW_CTRLr, LONGAN_RATE_MODE_CPUf, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RT_ERR_CHK_EHDL(reg_array_field_write(unit, LONGAN_EGBW_PORT_CTRLr, HWP_CPU_MACID(unit), REG_ARRAY_INDEX_NONE, LONGAN_BURSTf, &burst_size), ret,
                        RATE_SEM_UNLOCK(unit);RT_ERR(ret, (MOD_DAL|MOD_RATE), ""););
    value = 1;
    RT_ERR_CHK_EHDL(reg_array_field_write(unit, LONGAN_EGBW_PORT_LB_RSTr, HWP_CPU_MACID(unit), REG_ARRAY_INDEX_NONE, LONGAN_RSTf, &value), ret,
                        RATE_SEM_UNLOCK(unit);RT_ERR(ret, (MOD_DAL|MOD_RATE), ""););


    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_rate_egrPortBwCtrlBurstSize_get
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
dal_longan_rate_egrPortBwCtrlBurstSize_get(
    uint32                  unit,
    uint32                  *pBurst_size)
{
    rtk_port_t  port;

    RT_PARAM_CHK((NULL == pBurst_size), RT_ERR_NULL_POINTER);

    HWP_PORT_TRAVS(unit, port)
    {
        return dal_longan_rate_portEgrBwCtrlBurstSize_get(unit, port, pBurst_size);
    }

    return RT_ERR_FAILED;
}

/* Function Name:
 *      dal_longan_rate_egrPortBwCtrlBurstSize_set
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
dal_longan_rate_egrPortBwCtrlBurstSize_set(
    uint32                  unit,
    uint32                  burst_size)
{
    int32       ret;
    rtk_port_t port;

    HWP_PORT_TRAVS(unit, port)
    {
        if(RT_ERR_OK != (ret = dal_longan_rate_portEgrBwCtrlBurstSize_set(unit, port, burst_size)))
            return ret;
    }
    return RT_ERR_OK;

}

/* Function Name:
 *      dal_longan_rate_portEgrQueueBwCtrlEnable_get
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
 * Note:
 *      In stacking uplink port, the number of queue is 12 (queue 0~11). Before configuring the queue 8~11, please
 *      enable stacking ability of the uplink port by API dal_mango_stack_port_set.
 */
int32
dal_longan_rate_portEgrQueueBwCtrlEnable_get(
    uint32          unit,
    rtk_port_t      port,
    rtk_qid_t       queue,
    rtk_enable_t    *pEnable)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, queue=%d\n", unit, port, queue);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);
    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    if(HWP_IS_CPU_PORT(unit, port))
        RT_PARAM_CHK(queue >= HAL_MAX_NUM_OF_CPU_QUEUE(unit), RT_ERR_QUEUE_ID);
    else if ((HWP_UPLINK_PORT(unit, port)) && (HAL_STACK_PORT(unit, port)))
        RT_PARAM_CHK(queue >= HAL_MAX_NUM_OF_STACK_QUEUE(unit), RT_ERR_QUEUE_ID);
    else
        RT_PARAM_CHK((queue >= HAL_MAX_NUM_OF_QUEUE(unit)), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    if(HWP_IS_CPU_PORT(unit, port))
    {
        if ((ret = reg_array_field_read(unit, LONGAN_EGBW_CPU_Q_MAX_LB_CTRLr, REG_ARRAY_INDEX_NONE, queue, LONGAN_ENf, &value)) != RT_ERR_OK)
        {
            RATE_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }

    }
    else if(HWP_UPLINK_PORT(unit, port))
    {
        if ((ret = reg_array_field_read(unit, LONGAN_EGBW_PORT_Q_MAX_LB_CTRL_SET1r,  port, queue, LONGAN_ENf, &value)) != RT_ERR_OK)
        {
            RATE_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }

    }
    else
    {
        if ((ret = reg_array_field_read(unit, LONGAN_EGBW_PORT_Q_MAX_LB_CTRL_SET0r,  port, queue, LONGAN_ENf, &value)) != RT_ERR_OK)
        {
            RATE_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }

    }

    RATE_SEM_UNLOCK(unit);

    if (value == 1)
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_rate_portEgrQueueBwCtrlEnable_set
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
 * Note:
 *      In stacking uplink port, the number of queue is 12 (queue 0~11). Before configuring the queue 8~11, please
 *      enable stacking ability of the uplink port by API dal_mango_stack_port_set.
 */
int32
dal_longan_rate_portEgrQueueBwCtrlEnable_set(
    uint32          unit,
    rtk_port_t      port,
    rtk_qid_t       queue,
    rtk_enable_t    enable)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, queue=%d\n", unit, port, queue);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    if(HWP_IS_CPU_PORT(unit, port))
        RT_PARAM_CHK(queue >= HAL_MAX_NUM_OF_CPU_QUEUE(unit), RT_ERR_QUEUE_ID);
    else if ((HWP_UPLINK_PORT(unit, port)) && (HAL_STACK_PORT(unit, port)))
        RT_PARAM_CHK(queue >= HAL_MAX_NUM_OF_STACK_QUEUE(unit), RT_ERR_QUEUE_ID);
    else
        RT_PARAM_CHK((queue >= HAL_MAX_NUM_OF_QUEUE(unit)), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    if (enable == ENABLED)
        value = 1;
    else
        value = 0;

    RATE_SEM_LOCK(unit);


    /* read value from CHIP*/
    if(HWP_IS_CPU_PORT(unit, port))
    {
        if ((ret = reg_array_field_write(unit, LONGAN_EGBW_CPU_Q_MAX_LB_CTRLr, REG_ARRAY_INDEX_NONE, queue, LONGAN_ENf, &value)) != RT_ERR_OK)
        {
            RATE_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }

    }
    else if(HWP_UPLINK_PORT(unit, port))
    {
        if ((ret = reg_array_field_write(unit, LONGAN_EGBW_PORT_Q_MAX_LB_CTRL_SET1r,  port, queue, LONGAN_ENf, &value)) != RT_ERR_OK)
        {
            RATE_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }

    }
    else
    {
        if ((ret = reg_array_field_write(unit, LONGAN_EGBW_PORT_Q_MAX_LB_CTRL_SET0r,  port, queue, LONGAN_ENf, &value)) != RT_ERR_OK)
        {
            RATE_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }

    }
    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_rate_portEgrQueueBwCtrlRate_get
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
 * Note:
 *      (1) The actual rate is "rate * chip granularity".
 *      (2) The unit of granularity is 16Kbps.
 *      (3) In stacking uplink port, the number of queue is 12 (queue 0~11). Before configuring the queue 8~11, please
 *          enable stacking ability of the uplink port by API dal_mango_stack_port_set.
 */
int32
dal_longan_rate_portEgrQueueBwCtrlRate_get(
    uint32      unit,
    rtk_port_t  port,
    rtk_qid_t   queue,
    uint32      *pRate)
{
    int32   ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, queue=%d\n", unit, port, queue);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    if(HWP_IS_CPU_PORT(unit, port))
        RT_PARAM_CHK(queue >= HAL_MAX_NUM_OF_CPU_QUEUE(unit), RT_ERR_QUEUE_ID);
    else if((HWP_UPLINK_PORT(unit, port)) && (HAL_STACK_PORT(unit, port)))
        RT_PARAM_CHK(queue >= HAL_MAX_NUM_OF_STACK_QUEUE(unit), RT_ERR_QUEUE_ID);
    else
        RT_PARAM_CHK((queue >= HAL_MAX_NUM_OF_QUEUE(unit)), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((NULL == pRate), RT_ERR_NULL_POINTER);

    if(!HWP_IS_CPU_PORT(unit, port))
    {
        *pRate = egr_rate[unit][port].queue_maxRate[queue];
    }
    else
    {
        RATE_SEM_LOCK(unit);

        if ((ret = reg_array_field_read(unit, LONGAN_EGBW_CPU_Q_MAX_LB_CTRLr, REG_ARRAY_INDEX_NONE, queue, LONGAN_RATEf, pRate)) != RT_ERR_OK)
        {
            RATE_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }

        RATE_SEM_UNLOCK(unit);
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_longan_rate_portEgrQueueBwCtrlRate_set
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
 * Note:
 *      (1) The actual rate is "rate * chip granularity".
 *      (2) The unit of granularity is 16Kbps.
 *      (3) In stacking uplink port, the number of queue is 12 (queue 0~11). Before configuring the queue 8~11, please
 *          enable stacking ability of the uplink port by API dal_mango_stack_port_set.
 */
int32
dal_longan_rate_portEgrQueueBwCtrlRate_set(
    uint32      unit,
    rtk_port_t  port,
    rtk_qid_t   queue,
    uint32      rate)
{
    int32   ret = RT_ERR_FAILED;
    uint32     rateAdjust;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, queue=%d\n", unit, port, queue);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    if(HWP_IS_CPU_PORT(unit, port))
        RT_PARAM_CHK(queue >= HAL_MAX_NUM_OF_CPU_QUEUE(unit), RT_ERR_QUEUE_ID);
    else if((HWP_UPLINK_PORT(unit, port)) && (HAL_STACK_PORT(unit, port)))
        RT_PARAM_CHK(queue >= HAL_MAX_NUM_OF_STACK_QUEUE(unit), RT_ERR_QUEUE_ID);
    else
        RT_PARAM_CHK((queue >= HAL_MAX_NUM_OF_QUEUE(unit)), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((rate > HAL_RATE_OF_BANDWIDTH_MAX(unit)), RT_ERR_RATE);


    RATE_SEM_LOCK(unit);

    if(!HWP_IS_CPU_PORT(unit, port))
    {
        if ((ret = _dal_longan_rate_egressRate_adjust(unit, rate, &rateAdjust)) != RT_ERR_OK)
        {
            RATE_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }
    }
    else
        rateAdjust = rate;

    /* write value to CHIP*/
    if(HWP_IS_CPU_PORT(unit, port))
    {
        if ((ret = reg_array_field_write(unit, LONGAN_EGBW_CPU_Q_MAX_LB_CTRLr, REG_ARRAY_INDEX_NONE, queue, LONGAN_RATEf, &rateAdjust)) != RT_ERR_OK)
        {
            RATE_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }

    }
    else if(HWP_UPLINK_PORT(unit, port))
    {
        if ((ret = reg_array_field_write(unit, LONGAN_EGBW_PORT_Q_MAX_LB_CTRL_SET1r,  port, queue, LONGAN_RATEf, &rateAdjust)) != RT_ERR_OK)
        {
            RATE_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }

    }
    else
    {
        if ((ret = reg_array_field_write(unit, LONGAN_EGBW_PORT_Q_MAX_LB_CTRL_SET0r,  port, queue, LONGAN_RATEf, &rateAdjust)) != RT_ERR_OK)
        {
            RATE_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }

    }

    if(!HWP_IS_CPU_PORT(unit, port))
    {
        egr_rate[unit][port].queue_maxRate[queue] = rate;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_rate_portEgrQueueBwCtrlBurstSize_get
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
 * Note:
 *      In stacking uplink port, the number of queue is 12 (queue 0~11). Before configuring the queue 8~11, please
 *      enable stacking ability of the uplink port by API dal_mango_stack_port_set.
 */
int32
dal_longan_rate_portEgrQueueBwCtrlBurstSize_get(uint32 unit, rtk_port_t port, rtk_qid_t queue, uint32 *pBurst_size)
{
    int32   ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, queue=%d\n", unit, port, queue);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    if(HWP_IS_CPU_PORT(unit, port))
        RT_PARAM_CHK(queue >= HAL_MAX_NUM_OF_CPU_QUEUE(unit), RT_ERR_QUEUE_ID);
    else if((HWP_UPLINK_PORT(unit, port)) && (HAL_STACK_PORT(unit, port)))
        RT_PARAM_CHK(queue >= HAL_MAX_NUM_OF_STACK_QUEUE(unit), RT_ERR_QUEUE_ID);
    else
        RT_PARAM_CHK((queue >= HAL_MAX_NUM_OF_QUEUE(unit)), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((NULL == pBurst_size), RT_ERR_NULL_POINTER);

    RATE_SEM_LOCK(unit);


    /* read value from CHIP*/
    if(HWP_IS_CPU_PORT(unit, port))
    {
        if ((ret = reg_array_field_read(unit, LONGAN_EGBW_CPU_Q_MAX_LB_CTRLr, REG_ARRAY_INDEX_NONE, queue, LONGAN_BURSTf, pBurst_size)) != RT_ERR_OK)
        {
            RATE_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }

    }
    else if(HWP_UPLINK_PORT(unit, port))
    {
        if ((ret = reg_array_field_read(unit, LONGAN_EGBW_PORT_Q_MAX_LB_CTRL_SET1r,  port, queue, LONGAN_BURSTf, pBurst_size)) != RT_ERR_OK)
        {
            RATE_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }

    }
    else
    {
        if ((ret = reg_array_field_read(unit, LONGAN_EGBW_PORT_Q_MAX_LB_CTRL_SET0r,  port, queue, LONGAN_BURSTf, pBurst_size)) != RT_ERR_OK)
        {
            RATE_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }

    }
    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_rate_portEgrQueueBwCtrlBurstSize_set
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
 * Note:
 *      In stacking uplink port, the number of queue is 12 (queue 0~11). Before configuring the queue 8~11, please
 *      enable stacking ability of the uplink port by API dal_mango_stack_port_set.
 */
int32
dal_longan_rate_portEgrQueueBwCtrlBurstSize_set(uint32 unit, rtk_port_t port, rtk_qid_t queue, uint32 burst_size)
{
    int32   ret = RT_ERR_FAILED;
    uint32  ebc_token;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, queue=%d\n", unit, port, queue);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    if(HWP_IS_CPU_PORT(unit, port))
        RT_PARAM_CHK(queue >= HAL_MAX_NUM_OF_CPU_QUEUE(unit), RT_ERR_QUEUE_ID);
    else if((HWP_UPLINK_PORT(unit, port)) && (HAL_STACK_PORT(unit, port)))
        RT_PARAM_CHK(queue >= HAL_MAX_NUM_OF_STACK_QUEUE(unit), RT_ERR_QUEUE_ID);
    else
        RT_PARAM_CHK((queue >= HAL_MAX_NUM_OF_QUEUE(unit)), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((burst_size > (HAL_BURST_OF_BANDWIDTH_MAX(unit) - HAL_MAX_ACCEPT_FRAME_LEN(unit))), RT_ERR_INPUT);

    RATE_SEM_LOCK(unit);

    if ((ret = reg_field_read(unit, LONGAN_EGBW_CTRLr,LONGAN_RATE_MODE_CPUf, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    if ((ret = reg_field_read(unit, LONGAN_EGBW_LB_CTRLr,LONGAN_TKNf, &ebc_token)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    if(!(HWP_IS_CPU_PORT(unit, port) && value==0))
    {
        RT_PARAM_CHK((burst_size < ( 3*ebc_token)), RT_ERR_INPUT);
    }

    RATE_SEM_LOCK(unit);

    /* write value to CHIP*/
    if(HWP_IS_CPU_PORT(unit, port))
    {
        if ((ret = reg_array_field_write(unit, LONGAN_EGBW_CPU_Q_MAX_LB_CTRLr, REG_ARRAY_INDEX_NONE, queue, LONGAN_BURSTf, &burst_size)) != RT_ERR_OK)
        {
            RATE_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }

    }
    else if(HWP_UPLINK_PORT(unit, port))
    {
        if ((ret = reg_array_field_write(unit, LONGAN_EGBW_PORT_Q_MAX_LB_CTRL_SET1r,  port, queue, LONGAN_BURSTf, &burst_size)) != RT_ERR_OK)
        {
            RATE_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }

    }
    else
    {
        if ((ret = reg_array_field_write(unit, LONGAN_EGBW_PORT_Q_MAX_LB_CTRL_SET0r,  port, queue, LONGAN_BURSTf, &burst_size)) != RT_ERR_OK)
        {
            RATE_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }

    }
    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_longan_rate_portEgrQueueAssuredBwCtrlEnable_get
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
 * Note:
 *      In stacking uplink port, the number of queue is 12 (queue 0~11). Before configuring the queue 8~11, please
 *      enable stacking ability of the uplink port by API dal_mango_stack_port_set.
 */
int32
dal_longan_rate_portEgrQueueAssuredBwCtrlEnable_get(
    uint32          unit,
    rtk_port_t      port,
    rtk_qid_t       queue,
    rtk_enable_t    *pEnable)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value;
    uint32 reg;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, queue=%d\n", unit, port, queue);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((!HWP_PORT_EXIST(unit, port) || HWP_IS_CPU_PORT(unit, port)), RT_ERR_PORT_ID);
    if((HWP_UPLINK_PORT(unit, port)) && (HAL_STACK_PORT(unit, port)))
        RT_PARAM_CHK(queue >= HAL_MAX_NUM_OF_STACK_QUEUE(unit), RT_ERR_QUEUE_ID);
    else
        RT_PARAM_CHK((queue >= HAL_MAX_NUM_OF_QUEUE(unit)), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    if(egrPortAssuedRateMode[unit][port].queueRateMode[queue] == ASSURED_MODE_SHARE)
    {
        reg = (HWP_UPLINK_PORT(unit, port) ? LONGAN_EGBW_PORT_Q_ASSURED_LB_CTRL_SET1r : LONGAN_EGBW_PORT_Q_ASSURED_LB_CTRL_SET0r);
        if ((ret = reg_array_field_read(unit, reg ,  port, queue, LONGAN_ENf, &value)) != RT_ERR_OK)
        {
            RATE_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }
    }
    else
    {
        reg = (HWP_UPLINK_PORT(unit, port) ? LONGAN_EGBW_PORT_Q_FIX_LB_CTRL_SET1r : LONGAN_EGBW_PORT_Q_FIX_LB_CTRL_SET0r);
        if ((ret = reg_array_field_read(unit, reg,  port, queue, LONGAN_ENf, &value)) != RT_ERR_OK)
        {
            RATE_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }
    }

    RATE_SEM_UNLOCK(unit);

    if (value == 1)
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_rate_portEgrQueueAssuredBwCtrlEnable_set
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
 * Note:
 *      In stacking uplink port, the number of queue is 12 (queue 0~11). Before configuring the queue 8~11, please
 *      enable stacking ability of the uplink port by API dal_mango_stack_port_set.
 */
int32
dal_longan_rate_portEgrQueueAssuredBwCtrlEnable_set(
    uint32          unit,
    rtk_port_t      port,
    rtk_qid_t       queue,
    rtk_enable_t    enable)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value;
    uint32 reg;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, queue=%d\n", unit, port, queue);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((!HWP_PORT_EXIST(unit, port) || HWP_IS_CPU_PORT(unit, port)), RT_ERR_PORT_ID);
    if((HWP_UPLINK_PORT(unit, port)) && (HAL_STACK_PORT(unit, port)))
        RT_PARAM_CHK(queue >= HAL_MAX_NUM_OF_STACK_QUEUE(unit), RT_ERR_QUEUE_ID);
    else
        RT_PARAM_CHK((queue >= HAL_MAX_NUM_OF_QUEUE(unit)), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    if (enable == ENABLED)
        value = 1;
    else
        value = 0;

    RATE_SEM_LOCK(unit);


    /* read value from CHIP*/
    if(egrPortAssuedRateMode[unit][port].queueRateMode[queue] == ASSURED_MODE_SHARE)
    {
        reg = (HWP_UPLINK_PORT(unit, port) ? LONGAN_EGBW_PORT_Q_ASSURED_LB_CTRL_SET1r : LONGAN_EGBW_PORT_Q_ASSURED_LB_CTRL_SET0r);
        if ((ret = reg_array_field_write(unit, reg,  port, queue, LONGAN_ENf, &value)) != RT_ERR_OK)
        {
            RATE_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }

        value=0;
        reg = (HWP_UPLINK_PORT(unit, port) ? LONGAN_EGBW_PORT_Q_FIX_LB_CTRL_SET1r : LONGAN_EGBW_PORT_Q_FIX_LB_CTRL_SET0r);
        if ((ret = reg_array_field_write(unit, reg,  port, queue, LONGAN_ENf, &value)) != RT_ERR_OK)
        {
            RATE_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }
    }
    else
    {
        reg = (HWP_UPLINK_PORT(unit, port) ? LONGAN_EGBW_PORT_Q_FIX_LB_CTRL_SET1r : LONGAN_EGBW_PORT_Q_FIX_LB_CTRL_SET0r);
        if ((ret = reg_array_field_write(unit, reg,  port, queue, LONGAN_ENf, &value)) != RT_ERR_OK)
        {
            RATE_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }

        value=0;
        reg = (HWP_UPLINK_PORT(unit, port) ? LONGAN_EGBW_PORT_Q_ASSURED_LB_CTRL_SET1r : LONGAN_EGBW_PORT_Q_ASSURED_LB_CTRL_SET0r);
        if ((ret = reg_array_field_write(unit, reg, port, queue, LONGAN_ENf, &value)) != RT_ERR_OK)
        {
            RATE_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_rate_portEgrQueueAssuredBwCtrlRate_get
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
 * Note:
 *      (1) The actual rate is "rate * chip granularity".
 *      (2) The unit of granularity is 16Kbps.
 *      (3) In stacking uplink port, the number of queue is 12 (queue 0~11). Before configuring the queue 8~11, please
 *          enable stacking ability of the uplink port by API dal_mango_stack_port_set.
 */
int32
dal_longan_rate_portEgrQueueAssuredBwCtrlRate_get(uint32 unit, rtk_port_t port, rtk_qid_t queue, uint32 *pRate)
{
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, queue=%d\n", unit, port, queue);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((!HWP_PORT_EXIST(unit, port) || HWP_IS_CPU_PORT(unit, port)), RT_ERR_PORT_ID);
    if((HWP_UPLINK_PORT(unit, port)) && (HAL_STACK_PORT(unit, port)))
        RT_PARAM_CHK(queue >= HAL_MAX_NUM_OF_STACK_QUEUE(unit), RT_ERR_QUEUE_ID);
    else
        RT_PARAM_CHK(queue >= HAL_MAX_NUM_OF_QUEUE(unit), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((NULL == pRate), RT_ERR_NULL_POINTER);

    *pRate = egr_rate[unit][port].queue_assureRate[queue];

    return RT_ERR_OK;
}



/* Function Name:
 *      dal_longan_rate_portEgrQueueAssuredBwCtrlRate_set
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
 * Note:
 *      (1) The actual rate is "rate * chip granularity".
 *      (2) The unit of granularity is 16Kbps.
 *      (3) In stacking uplink port, the number of queue is 12 (queue 0~11). Before configuring the queue 8~11, please
 *          enable stacking ability of the uplink port by API dal_mango_stack_port_set.
 */
int32
dal_longan_rate_portEgrQueueAssuredBwCtrlRate_set(uint32 unit, rtk_port_t port, rtk_qid_t queue, uint32 rate)
{
    int32   ret = RT_ERR_FAILED;
    uint32 reg;
    uint32 rateAdjust;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, queue=%d\n", unit, port, queue);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((!HWP_PORT_EXIST(unit, port) || HWP_IS_CPU_PORT(unit, port)), RT_ERR_PORT_ID);
    if((HWP_UPLINK_PORT(unit, port)) && (HAL_STACK_PORT(unit, port)))
        RT_PARAM_CHK(queue >= HAL_MAX_NUM_OF_STACK_QUEUE(unit), RT_ERR_QUEUE_ID);
    else
        RT_PARAM_CHK(queue >= HAL_MAX_NUM_OF_QUEUE(unit), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((rate > HAL_RATE_OF_BANDWIDTH_MAX(unit)), RT_ERR_RATE);

    RATE_SEM_LOCK(unit);

    if ((ret = _dal_longan_rate_egressRate_adjust(unit, rate, &rateAdjust)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    /* read value from CHIP*/
    reg = (HWP_UPLINK_PORT(unit, port) ? LONGAN_EGBW_PORT_Q_ASSURED_LB_CTRL_SET1r : LONGAN_EGBW_PORT_Q_ASSURED_LB_CTRL_SET0r);
    if ((ret = reg_array_field_write(unit, reg,  port, queue, LONGAN_RATEf, &rateAdjust)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }
    reg = (HWP_UPLINK_PORT(unit, port) ? LONGAN_EGBW_PORT_Q_FIX_LB_CTRL_SET1r : LONGAN_EGBW_PORT_Q_FIX_LB_CTRL_SET0r);
    if ((ret = reg_array_field_write(unit, reg,  port, queue, LONGAN_RATEf, &rateAdjust)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    egr_rate[unit][port].queue_assureRate[queue] = rate;

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_mango_rate_portEgrQueueAssuredBwCtrlBurstSize_get
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
 * Note:
 *      In stacking uplink port, the number of queue is 12 (queue 0~11). Before configuring the queue 8~11, please
 *      enable stacking ability of the uplink port by API dal_mango_stack_port_set.
 */
int32
dal_longan_rate_portEgrQueueAssuredBwCtrlBurstSize_get(uint32 unit, rtk_port_t port, rtk_qid_t queue, uint32 *pBurst_size)
{
    int32   ret = RT_ERR_FAILED;
    uint32 reg;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, queue=%d\n", unit, port, queue);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((!HWP_PORT_EXIST(unit, port) || HWP_IS_CPU_PORT(unit, port)), RT_ERR_PORT_ID);
    if((HWP_UPLINK_PORT(unit, port)) && (HAL_STACK_PORT(unit, port)))
        RT_PARAM_CHK(queue >= HAL_MAX_NUM_OF_STACK_QUEUE(unit), RT_ERR_QUEUE_ID);
    else
        RT_PARAM_CHK((queue >= HAL_MAX_NUM_OF_QUEUE(unit)), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((NULL == pBurst_size), RT_ERR_NULL_POINTER);

    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    reg = (HWP_UPLINK_PORT(unit, port) ? LONGAN_EGBW_PORT_Q_ASSURED_FIX_BURST_CTRL_SET1r : LONGAN_EGBW_PORT_Q_ASSURED_FIX_BURST_CTRL_SET0r);
    if ((ret = reg_array_field_read(unit, reg,  port, queue, LONGAN_BURSTf, pBurst_size)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_longan_rate_portEgrQueueAssuredBwCtrlBurstSize_set
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
 * Note:
 *      In stacking uplink port, the number of queue is 12 (queue 0~11). Before configuring the queue 8~11, please
 *      enable stacking ability of the uplink port by API dal_mango_stack_port_set.
 */
int32
dal_longan_rate_portEgrQueueAssuredBwCtrlBurstSize_set(uint32 unit, rtk_port_t port, rtk_qid_t queue, uint32 burst_size)
{
    int32   ret = RT_ERR_FAILED;
    uint32  ebc_token;
    uint32  value;
    uint32 reg;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, queue=%d\n", unit, port, queue);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((!HWP_PORT_EXIST(unit, port) || HWP_IS_CPU_PORT(unit, port)), RT_ERR_PORT_ID);
    if((HWP_UPLINK_PORT(unit, port)) && (HAL_STACK_PORT(unit, port)))
        RT_PARAM_CHK(queue >= HAL_MAX_NUM_OF_STACK_QUEUE(unit), RT_ERR_QUEUE_ID);
    else
        RT_PARAM_CHK((queue >= HAL_MAX_NUM_OF_QUEUE(unit)), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((burst_size > (HAL_BURST_OF_BANDWIDTH_MAX(unit) - HAL_MAX_ACCEPT_FRAME_LEN(unit))), RT_ERR_INPUT);

    RATE_SEM_LOCK(unit);

    if ((ret = reg_field_read(unit, LONGAN_EGBW_CTRLr,LONGAN_RATE_MODE_CPUf, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    if ((ret = reg_field_read(unit, LONGAN_EGBW_LB_CTRLr,LONGAN_TKNf, &ebc_token)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    if(!(HWP_IS_CPU_PORT(unit, port) && value==0))
    {
        RT_PARAM_CHK((burst_size < ( 3*ebc_token)), RT_ERR_INPUT);
    }

    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    reg = (HWP_UPLINK_PORT(unit, port) ? LONGAN_EGBW_PORT_Q_ASSURED_FIX_BURST_CTRL_SET1r : LONGAN_EGBW_PORT_Q_ASSURED_FIX_BURST_CTRL_SET0r);
    if ((ret = reg_array_field_write(unit, reg,  port, queue, LONGAN_BURSTf, &burst_size)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_rate_portEgrQueueAssuredBwCtrlMode_get
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
 * Note:
 *      (1) The mode are as following:
 *          - ASSURED_MODE_SHARE
 *          - ASSURED_MODE_FIX
 *      (2) In stacking uplink port, the number of queue is 12 (queue 0~11). Before configuring the queue 8~11, please
 *          enable stacking ability of the uplink port by API dal_mango_stack_port_set.
 */
int32
dal_longan_rate_portEgrQueueAssuredBwCtrlMode_get(
    uint32          unit,
    rtk_port_t      port,
    rtk_qid_t       queue,
    rtk_rate_assuredMode_t    *pMode)
{
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, queue=%d\n", unit, port, queue);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((!HWP_PORT_EXIST(unit, port) || HWP_IS_CPU_PORT(unit, port)), RT_ERR_PORT_ID);
    if((HWP_UPLINK_PORT(unit, port)) && (HAL_STACK_PORT(unit, port)))
        RT_PARAM_CHK(queue >= HAL_MAX_NUM_OF_STACK_QUEUE(unit), RT_ERR_QUEUE_ID);
    else
        RT_PARAM_CHK((queue >= HAL_MAX_NUM_OF_QUEUE(unit)), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((NULL == pMode), RT_ERR_NULL_POINTER);

    *pMode = egrPortAssuedRateMode[unit][port].queueRateMode[queue];

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "pMode=%d", *pMode);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_rate_portEgrQueueAssuredBwCtrlMode_set
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
 * Note:
 *      (1) The mode are as following:
 *          - ASSURED_MODE_SHARE
 *          - ASSURED_MODE_FIX
 *      (2) In stacking uplink port, the number of queue is 12 (queue 0~11). Before configuring the queue 8~11, please
 *          enable stacking ability of the uplink port by API dal_mango_stack_port_set.
 */
int32
dal_longan_rate_portEgrQueueAssuredBwCtrlMode_set(
    uint32          unit,
    rtk_port_t      port,
    rtk_qid_t       queue,
    rtk_rate_assuredMode_t    mode)
{
    int32   ret;
    uint32  value,valueTmp;
    uint32 reg;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, queue=%d\n", unit, port, queue);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((!HWP_PORT_EXIST(unit, port) || HWP_IS_CPU_PORT(unit, port)), RT_ERR_PORT_ID);
    if((HWP_UPLINK_PORT(unit, port)) && (HAL_STACK_PORT(unit, port)))
        RT_PARAM_CHK(queue >= HAL_MAX_NUM_OF_STACK_QUEUE(unit), RT_ERR_QUEUE_ID);
    else
        RT_PARAM_CHK((queue >= HAL_MAX_NUM_OF_QUEUE(unit)), RT_ERR_QUEUE_ID);

    RT_PARAM_CHK(mode > ASSURED_MODE_FIX, RT_ERR_INPUT);

    if(mode == egrPortAssuedRateMode[unit][port].queueRateMode[queue])
        return RT_ERR_OK;

    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    if(mode == ASSURED_MODE_FIX)
    {
        reg = (HWP_UPLINK_PORT(unit, port) ? LONGAN_EGBW_PORT_Q_ASSURED_LB_CTRL_SET1r : LONGAN_EGBW_PORT_Q_ASSURED_LB_CTRL_SET0r);
        if ((ret = reg_array_field_read(unit, reg,  port, queue, LONGAN_ENf, &value)) != RT_ERR_OK)
        {
            RATE_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }
        valueTmp = 0;
        reg = (HWP_UPLINK_PORT(unit, port) ? LONGAN_EGBW_PORT_Q_ASSURED_LB_CTRL_SET1r : LONGAN_EGBW_PORT_Q_ASSURED_LB_CTRL_SET0r);
        if ((ret = reg_array_field_write(unit, reg, port, queue, LONGAN_ENf, &valueTmp)) != RT_ERR_OK)
        {
            RATE_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }
        reg = (HWP_UPLINK_PORT(unit, port) ? LONGAN_EGBW_PORT_Q_FIX_LB_CTRL_SET1r : LONGAN_EGBW_PORT_Q_FIX_LB_CTRL_SET0r);
        if ((ret = reg_array_field_write(unit, reg, port, queue, LONGAN_ENf, &value)) != RT_ERR_OK)
        {
            RATE_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }
    }
    else
    {
        reg = (HWP_UPLINK_PORT(unit, port) ? LONGAN_EGBW_PORT_Q_FIX_LB_CTRL_SET1r : LONGAN_EGBW_PORT_Q_FIX_LB_CTRL_SET0r);
        if ((ret = reg_array_field_read(unit, reg,  port, queue, LONGAN_ENf, &value)) != RT_ERR_OK)
        {
            RATE_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }
        valueTmp = 0;
        reg = (HWP_UPLINK_PORT(unit, port) ? LONGAN_EGBW_PORT_Q_FIX_LB_CTRL_SET1r : LONGAN_EGBW_PORT_Q_FIX_LB_CTRL_SET0r);
        if ((ret = reg_array_field_write(unit, reg,  port, queue, LONGAN_ENf, &valueTmp)) != RT_ERR_OK)
        {
            RATE_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }

        reg = (HWP_UPLINK_PORT(unit, port) ? LONGAN_EGBW_PORT_Q_ASSURED_LB_CTRL_SET1r : LONGAN_EGBW_PORT_Q_ASSURED_LB_CTRL_SET0r);
        if ((ret = reg_array_field_write(unit, reg, port, queue, LONGAN_ENf, &value)) != RT_ERR_OK)
        {
            RATE_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }
    }


    egrPortAssuedRateMode[unit][port].queueRateMode[queue] = mode;

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_rate_egrQueueBwCtrlBurstSize_get
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
dal_longan_rate_egrQueueBwCtrlBurstSize_get(
    uint32                  unit,
    uint32                  *pBurst_size)
{
    rtk_port_t      port;

    RT_PARAM_CHK((NULL == pBurst_size), RT_ERR_NULL_POINTER);

    HWP_PORT_TRAVS(unit, port)
    {
        return dal_longan_rate_portEgrQueueBwCtrlBurstSize_get(unit, port, 0, pBurst_size);
    }

    return RT_ERR_FAILED;
}
/* Function Name:
 *      dal_longan_rate_egrQueueBwCtrlBurstSize_set
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
dal_longan_rate_egrQueueBwCtrlBurstSize_set(
    uint32                  unit,
    uint32                  burst_size)
{
    int32 ret = RT_ERR_FAILED;
    rtk_port_t port;
    rtk_qid_t queue, max_queue;

    HWP_PORT_TRAVS(unit, port)
    {
        if(HWP_IS_CPU_PORT(unit, port))
            max_queue = HAL_MAX_NUM_OF_CPU_QUEUE(unit);
        else if ((HWP_UPLINK_PORT(unit, port)) && (HAL_STACK_PORT(unit, port)))
            max_queue = HAL_MAX_NUM_OF_STACK_QUEUE(unit);
        else
            max_queue = HAL_MAX_NUM_OF_QUEUE(unit);

        for(queue=0; queue<max_queue; queue++)
        {
            if(RT_ERR_OK != (ret = dal_longan_rate_portEgrQueueBwCtrlBurstSize_set(unit, port, queue, burst_size)))
                return ret;
        }
    }

    return RT_ERR_OK;
}

/* Module Name    : Rate                           */
/* Sub-module Name: Configuration of storm control */

/* Function Name:
 *      dal_longan_rate_portStormCtrlEnable_get
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
 * Note:
 *      The storm group types are as following:
 *          - STORM_GROUP_MULTICAST
 *          - STORM_GROUP_BROADCAST
 *          - STORM_GROUP_UNICAST
 */
int32
dal_longan_rate_portStormCtrlEnable_get(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_group_t  storm_type,
    rtk_enable_t            *pEnable)
{
    int32   ret = RT_ERR_FAILED;
    uint32  reg_idx;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, storm_type=%d",
           unit, port, storm_type);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((storm_type >= STORM_GROUP_END), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    switch (storm_type)
    {
        case STORM_GROUP_UNICAST:
            reg_idx = LONGAN_STORM_PORT_UC_CTRLr;
            break;
        case STORM_GROUP_MULTICAST:
            reg_idx = LONGAN_STORM_PORT_MC_CTRLr;
            break;
        case STORM_GROUP_BROADCAST:
            reg_idx = LONGAN_STORM_PORT_BC_CTRLr;
            break;
        default:
            return RT_ERR_INPUT;
    }

    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_array_field_read(unit, reg_idx, port, REG_ARRAY_INDEX_NONE, LONGAN_ENf, &value)) != RT_ERR_OK)
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
}

/* Function Name:
 *      dal_longan_rate_portStormCtrlEnable_set
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
 * Note:
 *      The storm group types are as following:
 *          - STORM_GROUP_MULTICAST
 *          - STORM_GROUP_BROADCAST
 *          - STORM_GROUP_UNICAST
 */
int32
dal_longan_rate_portStormCtrlEnable_set(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_group_t  storm_type,
    rtk_enable_t            enable)
{
    int32   ret = RT_ERR_FAILED;
    uint32  reg_idx;
    uint32  value;
    uint32  rst_idx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, storm_type=%d",
           unit, port, storm_type);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((storm_type >= STORM_GROUP_END), RT_ERR_INPUT);
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    if (enable == ENABLED)
        value = 1;
    else
        value = 0;

    switch (storm_type)
    {
        case STORM_GROUP_UNICAST:
            reg_idx = LONGAN_STORM_PORT_UC_CTRLr;
            rst_idx = LONGAN_STORM_PORT_UC_LB_RSTr;
            break;
        case STORM_GROUP_MULTICAST:
            reg_idx = LONGAN_STORM_PORT_MC_CTRLr;
            rst_idx = LONGAN_STORM_PORT_MC_LB_RSTr;
            break;
        case STORM_GROUP_BROADCAST:
            reg_idx = LONGAN_STORM_PORT_BC_CTRLr;
            rst_idx = LONGAN_STORM_PORT_BC_LB_RSTr;
            break;
        default:
            return RT_ERR_INPUT;
    }

    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_array_field_write(unit, reg_idx, port, REG_ARRAY_INDEX_NONE, LONGAN_ENf, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    value = 0x1;
    RT_ERR_CHK_EHDL(reg_array_field_write(unit, rst_idx, port, REG_ARRAY_INDEX_NONE, LONGAN_RSTf, &value), ret,
                            RATE_SEM_UNLOCK(unit);RT_ERR(ret, (MOD_DAL|MOD_RATE), ""););

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_rate_portStormCtrlRate_get
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
 *      (3) The unit of granularity is 16Kbps or 1pps depend on mode set by 'dal_longan_rate_portStormCtrlRateMode_set'.
 */
int32
dal_longan_rate_portStormCtrlRate_get(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_group_t  storm_type,
    uint32                  *pRate)
{
    uint32  db_idx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, storm_type=%d",
           unit, port, storm_type);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((storm_type >= STORM_GROUP_END), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pRate), RT_ERR_NULL_POINTER);

    switch (storm_type)
    {
        case STORM_GROUP_UNICAST:
            db_idx = 0;
            break;
        case STORM_GROUP_MULTICAST:
            db_idx = 1;
            break;
        case STORM_GROUP_BROADCAST:
            db_idx = 2;
            break;
        default:
            return RT_ERR_INPUT;
    }

    /*get value from database*/
    *pRate = rate_db[unit][db_idx][port];


    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_rate_portStormCtrlRate_set
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
 *      (3) The unit of granularity is 16Kbps or 1pps depend on mode set by 'dal_longan_rate_portStormCtrlRateMode_set'.
 */

int32
dal_longan_rate_portStormCtrlRate_set(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_group_t  storm_type,
    uint32                  rate)
{
    int32   ret = RT_ERR_FAILED;
    uint32  reg_idx;
    rtk_rate_storm_rateMode_t rate_mode;
    uint32  rate_final;
    uint32  db_idx;
    uint32  rst_idx;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, storm_type=%d",
           unit, port, storm_type);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((storm_type >= STORM_GROUP_END), RT_ERR_INPUT);
    RT_PARAM_CHK((rate > HAL_RATE_OF_STORM_CONTROL_MAX(unit)), RT_ERR_RATE);

    switch (storm_type)
    {
        case STORM_GROUP_UNICAST:
            reg_idx = LONGAN_STORM_PORT_UC_CTRLr;
            rst_idx = LONGAN_STORM_PORT_UC_LB_RSTr;
            db_idx = 0;
            break;
        case STORM_GROUP_MULTICAST:
            reg_idx = LONGAN_STORM_PORT_MC_CTRLr;
            rst_idx = LONGAN_STORM_PORT_MC_LB_RSTr;
            db_idx = 1;
            break;
        case STORM_GROUP_BROADCAST:
            reg_idx = LONGAN_STORM_PORT_BC_CTRLr;
            rst_idx = LONGAN_STORM_PORT_BC_LB_RSTr;
            db_idx = 2;
            break;
        default:
            return RT_ERR_INPUT;
    }

    /*LONGAN SS-528*/
    if (RT_ERR_OK != (ret = dal_longan_rate_portStormCtrlRateMode_get(unit, port, &rate_mode)))
    {
        return ret;
    }

    if (BASED_ON_PKT == rate_mode)
    {
        rate_final = (rate / RATE_TO_DIV * RATE_TO_MUL) + ((rate % RATE_TO_DIV) * RATE_TO_MUL + ROUND_CRCT) / RATE_TO_DIV;    }
    else
    {
        rate_final = rate;
    }


    RATE_SEM_LOCK(unit);
    rate_db[unit][db_idx][port] = rate;

    /* read value from CHIP*/
    if ((ret = reg_array_field_write(unit, reg_idx, port, REG_ARRAY_INDEX_NONE, LONGAN_RATEf, &rate_final)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }
    value = 0x1;
    RT_ERR_CHK_EHDL(reg_array_field_write(unit, rst_idx, port, REG_ARRAY_INDEX_NONE, LONGAN_RSTf, &value), ret,
                            RATE_SEM_UNLOCK(unit);RT_ERR(ret, (MOD_DAL|MOD_RATE), ""););

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_rate_stormControlBurstSize_get
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
 * Note:
 *      The storm group types are as following:
 *      - STORM_GROUP_MULTICAST
 *      - STORM_GROUP_BROADCAST
 *      - STORM_GROUP_UNICAST
 */
int32
dal_longan_rate_stormControlBurstSize_get(
    uint32                  unit,
    rtk_rate_storm_group_t  storm_type,
    uint32                  *pBurst_size)
{
    rtk_port_t      port;

    RT_PARAM_CHK((NULL == pBurst_size), RT_ERR_NULL_POINTER);

    HWP_PORT_TRAVS(unit, port)
    {
        return dal_longan_rate_portStormCtrlBurstSize_get(unit, port, storm_type, pBurst_size);
    }

    return RT_ERR_FAILED;
}

/* Function Name:
 *      dal_longan_rate_stormControlBurstSize_set
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
 * Note:
 *      The storm group types are as following:
 *      - STORM_GROUP_MULTICAST
 *      - STORM_GROUP_BROADCAST
 *      - STORM_GROUP_UNICAST
 */
int32
dal_longan_rate_stormControlBurstSize_set(
    uint32                  unit,
    rtk_rate_storm_group_t  storm_type,
    uint32                  burst_size)
{
    int32 ret = RT_ERR_FAILED;
    rtk_port_t port;

    HWP_PORT_TRAVS(unit, port)
    {
        if(RT_ERR_OK != (ret = dal_longan_rate_portStormCtrlBurstSize_set(unit, port, storm_type, burst_size)))
            return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_rate_portStormCtrlBurstSize_get
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
 * Note:
 *      (1) The storm group types are as following:
 *          rtk_rate_storm_group_t
 *          - STORM_GROUP_MULTICAST
 *          - STORM_GROUP_BROADCAST
 *          - STORM_GROUP_UNICAST
 *      (2) The unit of burst size is 1 byte or 1 packet depend on mode.
 */
int32
dal_longan_rate_portStormCtrlBurstSize_get(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_group_t  storm_type,
    uint32                  *pBurst_size)
{
    int32   ret = RT_ERR_FAILED;
    uint32  reg_idx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, storm_type=%d",
           unit, port, storm_type);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((storm_type >= STORM_GROUP_END), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pBurst_size), RT_ERR_NULL_POINTER);

    switch (storm_type)
    {
        case STORM_GROUP_UNICAST:
            reg_idx = LONGAN_STORM_PORT_UC_CTRLr;
            break;
        case STORM_GROUP_MULTICAST:
            reg_idx = LONGAN_STORM_PORT_MC_CTRLr;
            break;
        case STORM_GROUP_BROADCAST:
            reg_idx = LONGAN_STORM_PORT_BC_CTRLr;
            break;
        default:
            return RT_ERR_INPUT;
    }

    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_array_field_read(unit, reg_idx, port, REG_ARRAY_INDEX_NONE, LONGAN_BURSTf, pBurst_size)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_rate_portStormCtrlBurstSize_set
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
 *      RT_ERR_INPUT             - invalid input parameter0
 * Note:
 *      (1) The storm group types are as following:
 *          rtk_rate_storm_group_t
 *          - STORM_GROUP_MULTICAST
 *          - STORM_GROUP_BROADCAST
 *          - STORM_GROUP_UNICAST
 *      (2) The unit of burst size is 1 byte or 1 packet depend on mode.
 */
int32
dal_longan_rate_portStormCtrlBurstSize_set(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_group_t  storm_type,
    uint32                  burst_size)
{
    int32   ret = RT_ERR_FAILED;
    uint32  reg_idx;
    uint32  value;
    uint32  storm_token;
    uint32  rst_idx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, storm_type=%d",
           unit, port, storm_type);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((storm_type >= STORM_GROUP_END), RT_ERR_INPUT);
    RT_PARAM_CHK((burst_size >HAL_BURST_OF_STORM_CONTROL_MAX(unit)), RT_ERR_INPUT);


    RATE_SEM_LOCK(unit);
    if ((ret = reg_array_field_read(unit, LONGAN_STORM_PORT_CTRLr, port, REG_ARRAY_INDEX_NONE, LONGAN_MODEf, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    if(value==1)
    {
        if ((ret = reg_field_read(unit, LONGAN_STORM_LB_CTRLr,LONGAN_TKNf, &storm_token)) != RT_ERR_OK)
        {
            RATE_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }
    }
    RATE_SEM_UNLOCK(unit);

    if(value==1)
        RT_PARAM_CHK((burst_size < ( 3*storm_token)), RT_ERR_INPUT);


    switch (storm_type)
    {
        case STORM_GROUP_UNICAST:
            reg_idx = LONGAN_STORM_PORT_UC_CTRLr;
            rst_idx = LONGAN_STORM_PORT_UC_LB_RSTr;
            break;
        case STORM_GROUP_MULTICAST:
            reg_idx = LONGAN_STORM_PORT_MC_CTRLr;
            rst_idx = LONGAN_STORM_PORT_MC_LB_RSTr;
            break;
        case STORM_GROUP_BROADCAST:
            reg_idx = LONGAN_STORM_PORT_BC_CTRLr;
            rst_idx = LONGAN_STORM_PORT_BC_LB_RSTr;
            break;
        default:
            return RT_ERR_INPUT;
    }

    RATE_SEM_LOCK(unit);

    /* write value to CHIP */
    if ((ret = reg_array_field_write(unit, reg_idx, port, REG_ARRAY_INDEX_NONE, LONGAN_BURSTf, &burst_size)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }
    value = 0x1;
    RT_ERR_CHK_EHDL(reg_array_field_write(unit, rst_idx, port, REG_ARRAY_INDEX_NONE, LONGAN_RSTf, &value), ret,
                            RATE_SEM_UNLOCK(unit);RT_ERR(ret, (MOD_DAL|MOD_RATE), ""););

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_rate_portStormCtrlExceed_get
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
 * Note:
 *      (1) Exceed status is as following
 *          - TRUE      storm rate is more than configured rate.
 *          - FALSE     storm rate is never over then configured rate.
 *      (2) The routine get exceed status and clear it after read
 */
int32
dal_longan_rate_portStormCtrlExceed_get(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_group_t  storm_type,
    uint32                  *pIsExceed)
{
    int32   ret = RT_ERR_FAILED;
    uint32  reg_idx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, storm_type=%d",
           unit, port, storm_type);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((storm_type >= STORM_GROUP_END), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pIsExceed), RT_ERR_NULL_POINTER);

    switch (storm_type)
    {
        case STORM_GROUP_UNICAST:
            reg_idx = LONGAN_STORM_PORT_UC_EXCEED_FLAGr;
            break;
        case STORM_GROUP_MULTICAST:
            reg_idx = LONGAN_STORM_PORT_MC_EXCEED_FLAGr;
            break;
        case STORM_GROUP_BROADCAST:
            reg_idx = LONGAN_STORM_PORT_BC_EXCEED_FLAGr;
            break;
        default:
            return RT_ERR_INPUT;
    }

    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_array_field_read(unit, reg_idx, port, REG_ARRAY_INDEX_NONE, LONGAN_FLAGf, pIsExceed)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_rate_portStormCtrlExceed_reset
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
 * Note:
 *      None
 */
int32
dal_longan_rate_portStormCtrlExceed_reset(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_group_t  storm_type)
{
    int32   ret = RT_ERR_FAILED;
    uint32  reg_idx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, storm_type=%d",
           unit, port, storm_type);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((storm_type >= STORM_GROUP_END), RT_ERR_INPUT);

    switch (storm_type)
    {
        case STORM_GROUP_UNICAST:
            reg_idx = LONGAN_STORM_PORT_UC_EXCEED_FLAGr;
            break;
        case STORM_GROUP_MULTICAST:
            reg_idx = LONGAN_STORM_PORT_MC_EXCEED_FLAGr;
            break;
        case STORM_GROUP_BROADCAST:
            reg_idx = LONGAN_STORM_PORT_BC_EXCEED_FLAGr;
            break;
        default:
            return RT_ERR_INPUT;
    }

    RATE_SEM_LOCK(unit);

    /*write 1 to clear*/
    if ((ret = reg_array_field_write1toClear(unit, reg_idx, port, REG_ARRAY_INDEX_NONE, LONGAN_FLAGf)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }


    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_rate_stormControlRateMode_get
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
 * Note:
 *      (1) The rate mode are as following:
 *          - BASED_ON_PKT
 *          - BASED_ON_BYTE
 */
int32
dal_longan_rate_stormControlRateMode_get(
    uint32                      unit,
    rtk_rate_storm_rateMode_t   *pRate_mode)
{
    rtk_port_t      port;

    HWP_PORT_TRAVS(unit, port)
    {
        return dal_longan_rate_portStormCtrlRateMode_get(unit, port, pRate_mode);
    }
    return RT_ERR_FAILED;
}

/* Function Name:
 *      dal_longan_rate_stormControlRateMode_set
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
 * Note:
 *      (1) The rate mode are as following:
 *          - BASED_ON_PKT
 *          - BASED_ON_BYTE
 */
int32
dal_longan_rate_stormControlRateMode_set(
    uint32                      unit,
    rtk_rate_storm_rateMode_t   rate_mode)
{
    int32 ret = RT_ERR_FAILED;
    rtk_port_t port;

    HWP_PORT_TRAVS(unit, port)
    {
        if(RT_ERR_OK != (ret = dal_longan_rate_portStormCtrlRateMode_set(unit, port, rate_mode)))
            return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_rate_portStormCtrlRateMode_get
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
 * Note:
 *      (1) The rate mode are as following:
 *          - BASED_ON_PKT
 *          - BASED_ON_BYTE
 */
int32
dal_longan_rate_portStormCtrlRateMode_get(
    uint32                      unit,
    rtk_port_t              port,
    rtk_rate_storm_rateMode_t   *pRate_mode)
{
    uint32  value;
    int32   ret = RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pRate_mode), RT_ERR_NULL_POINTER);

    RATE_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_array_field_read(unit, LONGAN_STORM_PORT_CTRLr, port, REG_ARRAY_INDEX_NONE, LONGAN_MODEf, &value)) != RT_ERR_OK)
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
}

/* Function Name:
 *      dal_longan_rate_portStormCtrlRateMode_set
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
 * Note:
 *      (1) The rate mode are as following:
 *          - BASED_ON_PKT
 *          - BASED_ON_BYTE
 */
int32
dal_longan_rate_portStormCtrlRateMode_set(
    uint32                      unit,
    rtk_port_t              port,
    rtk_rate_storm_rateMode_t   rate_mode)
{
    uint32  value;
    int32   ret = RT_ERR_FAILED;
    uint32  burst_size;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((rate_mode >= STORM_RATE_MODE_END), RT_ERR_INPUT);

    if (rate_mode == BASED_ON_BYTE)
    {
        value = 1;
        burst_size = LONGAN_STORM_BPS_DFLT_BURST_SIZE;
    }
    else
    {
        value = 0;
        burst_size = LONGAN_STORM_PPS_DFLT_BURST_SIZE;
    }

    RATE_SEM_LOCK(unit);

    /* set value to CHIP */
    if ((ret = reg_array_field_write(unit, LONGAN_STORM_PORT_CTRLr, port, REG_ARRAY_INDEX_NONE, LONGAN_MODEf, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    /* write value to CHIP */
    if ((ret = reg_array_field_write(unit, LONGAN_STORM_PORT_UC_CTRLr, port, REG_ARRAY_INDEX_NONE, LONGAN_BURSTf, &burst_size)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }
    value = 1;
    RT_ERR_CHK_EHDL(reg_array_field_write(unit, LONGAN_STORM_PORT_UC_LB_RSTr, port, REG_ARRAY_INDEX_NONE, LONGAN_RSTf, &value), ret,
                            RATE_SEM_UNLOCK(unit);RT_ERR(ret, (MOD_DAL|MOD_RATE), ""););
    /* write value to CHIP */
    if ((ret = reg_array_field_write(unit, LONGAN_STORM_PORT_MC_CTRLr, port, REG_ARRAY_INDEX_NONE, LONGAN_BURSTf, &burst_size)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }
    RT_ERR_CHK_EHDL(reg_array_field_write(unit, LONGAN_STORM_PORT_MC_LB_RSTr, port, REG_ARRAY_INDEX_NONE, LONGAN_RSTf, &value), ret,
                            RATE_SEM_UNLOCK(unit);RT_ERR(ret, (MOD_DAL|MOD_RATE), ""););
    /* write value to CHIP */
    if ((ret = reg_array_field_write(unit, LONGAN_STORM_PORT_BC_CTRLr, port, REG_ARRAY_INDEX_NONE, LONGAN_BURSTf, &burst_size)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }
    RT_ERR_CHK_EHDL(reg_array_field_write(unit, LONGAN_STORM_PORT_BC_LB_RSTr, port, REG_ARRAY_INDEX_NONE, LONGAN_RSTf, &value), ret,
                            RATE_SEM_UNLOCK(unit);RT_ERR(ret, (MOD_DAL|MOD_RATE), ""););


    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_rate_portStormCtrlTypeSel_get
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
dal_longan_rate_portStormCtrlTypeSel_get(uint32 unit, rtk_port_t port, rtk_rate_storm_group_t storm_type, rtk_rate_storm_sel_t *pStorm_sel)
{
    int32   ret = RT_ERR_FAILED;
    uint32  reg_idx;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, storm_type=%d",
           unit, port, storm_type);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((storm_type >= STORM_GROUP_END), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pStorm_sel), RT_ERR_NULL_POINTER);

    switch (storm_type)
    {
        case STORM_GROUP_UNICAST:
            reg_idx = LONGAN_STORM_PORT_UC_CTRLr;
            break;
        case STORM_GROUP_MULTICAST:
            reg_idx = LONGAN_STORM_PORT_MC_CTRLr;
            break;
        default:
            return RT_ERR_INPUT;
    }

    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_array_field_read(unit, reg_idx, port, REG_ARRAY_INDEX_NONE, LONGAN_TYPEf, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    /* chip's value translate */
    if(value==1)
        *pStorm_sel = STORM_SEL_UNKNOWN_AND_KNOWN;
    else
        *pStorm_sel = STORM_SEL_UNKNOWN;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "pStorm_sel=%d", *pStorm_sel);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_rate_portStormCtrlTypeSel_set
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
dal_longan_rate_portStormCtrlTypeSel_set(uint32 unit, rtk_port_t port, rtk_rate_storm_group_t storm_type, rtk_rate_storm_sel_t storm_sel)
{
    int32   ret = RT_ERR_FAILED;
    uint32  reg_idx;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, storm_type=%d",
           unit, port, storm_type);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((storm_type >= STORM_GROUP_END), RT_ERR_INPUT);
    RT_PARAM_CHK((storm_sel > STORM_SEL_END), RT_ERR_INPUT);

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

    switch (storm_type)
    {
        case STORM_GROUP_UNICAST:
            reg_idx = LONGAN_STORM_PORT_UC_CTRLr;
            break;
        case STORM_GROUP_MULTICAST:
            reg_idx = LONGAN_STORM_PORT_MC_CTRLr;
            break;
        default:
            return RT_ERR_INPUT;
    }

    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_array_field_write(unit, reg_idx, port, REG_ARRAY_INDEX_NONE, LONGAN_TYPEf, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_rate_stormControlBypass_get
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
 * Note:
 *      If a certain packet type is selected to bypass the storm control, the packet will
 *      not be blocked by the storm control and will not consume the bandwidth.
 */
int32
dal_longan_rate_stormControlBypass_get(uint32 unit, rtk_rate_storm_bypassType_t bypassType, rtk_enable_t  *pEnable)
{
    int32   ret = RT_ERR_FAILED;
    uint32  reg_field;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, bypassType=%d", unit, bypassType);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(bypassType >= STORM_BYPASS_END, RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    switch (bypassType)
    {
        case STORM_BYPASS_RMA:
            reg_field = LONGAN_ADMIT_RMAf;break;
        case STORM_BYPASS_BPDU:
            reg_field = LONGAN_ADMIT_BPDUf;break;
        case STORM_BYPASS_RTKPKT:
            reg_field = LONGAN_ADMIT_RTKPKTf;break;
        case STORM_BYPASS_IGMP:
            reg_field = LONGAN_ADMIT_IGMPf;break;
        case STORM_BYPASS_ARP:
            reg_field = LONGAN_ADMIT_ARPREQf;break;
        case STORM_BYPASS_DHCP:
            reg_field = LONGAN_ADMIT_DHCPf;break;
        case STORM_BYPASS_RIP:
            reg_field = LONGAN_ADMIT_RIPf;break;
        default:
            return RT_ERR_INPUT;
    }

    RATE_SEM_LOCK(unit);
    /* get value from CHIP */
    if ((ret = reg_field_read(unit, LONGAN_STORM_CTRLr, reg_field, &value)) != RT_ERR_OK)
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
}

/* Function Name:
 *      dal_longan_rate_stormControlBypass_set
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
 * Note:
 *      If a certain packet type is selected to bypass the storm control, the packet will
 *      not be blocked by the storm control and will not consume the bandwidth.
 */
int32
dal_longan_rate_stormControlBypass_set(uint32 unit, rtk_rate_storm_bypassType_t bypassType, rtk_enable_t enable)
{
    int32   ret = RT_ERR_FAILED;
    uint32  reg_field;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, bypassType=%d", unit, bypassType);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(bypassType >= STORM_BYPASS_END, RT_ERR_INPUT);
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    if (enable == ENABLED)
        value = 1;
    else
        value = 0;

    switch (bypassType)
    {
        case STORM_BYPASS_RMA:
            reg_field = LONGAN_ADMIT_RMAf;break;
        case STORM_BYPASS_BPDU:
            reg_field = LONGAN_ADMIT_BPDUf;break;
        case STORM_BYPASS_RTKPKT:
            reg_field = LONGAN_ADMIT_RTKPKTf;break;
        case STORM_BYPASS_IGMP:
            reg_field = LONGAN_ADMIT_IGMPf;break;
        case STORM_BYPASS_ARP:
            reg_field = LONGAN_ADMIT_ARPREQf;break;
        case STORM_BYPASS_DHCP:
            reg_field = LONGAN_ADMIT_DHCPf;break;
        case STORM_BYPASS_RIP:
            reg_field = LONGAN_ADMIT_RIPf;break;
        default:
            return RT_ERR_INPUT;
    }

    RATE_SEM_LOCK(unit);
    /* get value from CHIP */
    if ((ret = reg_field_write(unit, LONGAN_STORM_CTRLr, reg_field, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_longan_rate_portStormCtrlProtoEnable_get
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
 * Note:
 *      None
 */
int32
dal_longan_rate_portStormCtrlProtoEnable_get(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_proto_group_t  storm_type,
    rtk_enable_t            *pEnable)
{
    int32   ret = RT_ERR_FAILED;
    uint32  reg_idx;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, storm_type=%d",
           unit, port, storm_type);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((storm_type >= STORM_PROTO_GROUP_END), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    switch (storm_type)
    {
        case STORM_PROTO_GROUP_BPDU:
            reg_idx = LONGAN_STORM_PORT_PROTO_BPDU_CTRLr;break;
        case STORM_PROTO_GROUP_IGMP:
            reg_idx = LONGAN_STORM_PORT_PROTO_IGMP_CTRLr;break;
        case STORM_PROTO_GROUP_ARP:
            reg_idx = LONGAN_STORM_PORT_PROTO_ARP_CTRLr;break;
        case STORM_PROTO_GROUP_DHCP:
            reg_idx = LONGAN_STORM_PORT_PROTO_DHCP_CTRLr;break;
        default:
            return RT_ERR_INPUT;
    }

    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_array_field_read(unit, reg_idx, port, REG_ARRAY_INDEX_NONE, LONGAN_ENf, &value)) != RT_ERR_OK)
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
}

/* Function Name:
 *      dal_longan_rate_portStormCtrlProtoEnable_set
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
 * Note:
 *      None
 */
int32
dal_longan_rate_portStormCtrlProtoEnable_set(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_proto_group_t  storm_type,
    rtk_enable_t            enable)
{
    int32   ret = RT_ERR_FAILED;
    uint32  reg_idx;
    uint32  value;
    uint32  rst_idx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, storm_type=%d",
           unit, port, storm_type);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((storm_type >= STORM_PROTO_GROUP_END), RT_ERR_INPUT);
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    if (enable == ENABLED)
        value = 1;
    else
        value = 0;

    switch (storm_type)
    {
        case STORM_PROTO_GROUP_BPDU:
            reg_idx = LONGAN_STORM_PORT_PROTO_BPDU_CTRLr;
            rst_idx = LONGAN_STORM_PORT_PROTO_BPDU_LB_RSTr;
            break;
        case STORM_PROTO_GROUP_IGMP:
            reg_idx = LONGAN_STORM_PORT_PROTO_IGMP_CTRLr;
            rst_idx = LONGAN_STORM_PORT_PROTO_IGMP_LB_RSTr;
            break;
        case STORM_PROTO_GROUP_ARP:
            reg_idx = LONGAN_STORM_PORT_PROTO_ARP_CTRLr;
            rst_idx = LONGAN_STORM_PORT_PROTO_ARP_LB_RSTr;
            break;
        case STORM_PROTO_GROUP_DHCP:
            reg_idx = LONGAN_STORM_PORT_PROTO_DHCP_CTRLr;
            rst_idx = LONGAN_STORM_PORT_PROTO_DHCP_LB_RSTr;
            break;
        default:
            return RT_ERR_INPUT;
    }

    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_array_field_write(unit, reg_idx, port, REG_ARRAY_INDEX_NONE, LONGAN_ENf, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }
    value = 1;
    RT_ERR_CHK_EHDL(reg_array_field_write(unit, rst_idx, port, REG_ARRAY_INDEX_NONE, LONGAN_RSTf, &value), ret,
                            RATE_SEM_UNLOCK(unit);RT_ERR(ret, (MOD_DAL|MOD_RATE), ""););

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_longan_rate_portStormCtrlProtoRate_get
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
 * Note:
 *      The rate unit is 1 pps(packet-per-second).
 */
int32
dal_longan_rate_portStormCtrlProtoRate_get(
    uint32                          unit,
    rtk_port_t                      port,
    rtk_rate_storm_proto_group_t    storm_type,
    uint32                          *pRate)
{
    int32   ret = RT_ERR_FAILED;
    uint32  reg_idx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, storm_type=%d",
           unit, port, storm_type);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((storm_type >= STORM_PROTO_GROUP_END), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pRate), RT_ERR_NULL_POINTER);

    switch (storm_type)
    {
        case STORM_PROTO_GROUP_BPDU:
            reg_idx = LONGAN_STORM_PORT_PROTO_BPDU_CTRLr;break;
        case STORM_PROTO_GROUP_IGMP:
            reg_idx = LONGAN_STORM_PORT_PROTO_IGMP_CTRLr;break;
        case STORM_PROTO_GROUP_ARP:
            reg_idx = LONGAN_STORM_PORT_PROTO_ARP_CTRLr;break;
        case STORM_PROTO_GROUP_DHCP:
            reg_idx = LONGAN_STORM_PORT_PROTO_DHCP_CTRLr;break;
        default:
            return RT_ERR_INPUT;
    }

    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_array_field_read(unit, reg_idx, port, REG_ARRAY_INDEX_NONE, LONGAN_RATEf, pRate)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_rate_portStormCtrlProtoRate_set
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
 * Note:
 *      The rate unit is 1 pps(packet-per-second).
 */
int32
dal_longan_rate_portStormCtrlProtoRate_set(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_proto_group_t  storm_type,
    uint32                  rate)
{
    int32   ret = RT_ERR_FAILED;
    uint32  reg_idx;
    uint32  rst_idx;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, storm_type=%d",
           unit, port, storm_type);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((storm_type >= STORM_PROTO_GROUP_END), RT_ERR_INPUT);
    RT_PARAM_CHK((rate > HAL_RATE_OF_STORM_PROTO_CONTROL_MAX(unit)), RT_ERR_RATE);

    switch (storm_type)
    {
        case STORM_PROTO_GROUP_BPDU:
            reg_idx = LONGAN_STORM_PORT_PROTO_BPDU_CTRLr;
            rst_idx = LONGAN_STORM_PORT_PROTO_BPDU_LB_RSTr;
            break;
        case STORM_PROTO_GROUP_IGMP:
            reg_idx = LONGAN_STORM_PORT_PROTO_IGMP_CTRLr;
            rst_idx = LONGAN_STORM_PORT_PROTO_IGMP_LB_RSTr;
            break;
        case STORM_PROTO_GROUP_ARP:
            reg_idx = LONGAN_STORM_PORT_PROTO_ARP_CTRLr;
            rst_idx = LONGAN_STORM_PORT_PROTO_ARP_LB_RSTr;
            break;
        case STORM_PROTO_GROUP_DHCP:
            reg_idx = LONGAN_STORM_PORT_PROTO_DHCP_CTRLr;
            rst_idx = LONGAN_STORM_PORT_PROTO_DHCP_LB_RSTr;
            break;
        default:
            return RT_ERR_INPUT;
    }

    RATE_SEM_LOCK(unit);

    /* write value to CHIP */
    if ((ret = reg_array_field_write(unit, reg_idx, port, REG_ARRAY_INDEX_NONE, LONGAN_RATEf, &rate)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }
    value = 1;
    RT_ERR_CHK_EHDL(reg_array_field_write(unit, rst_idx, port, REG_ARRAY_INDEX_NONE, LONGAN_RSTf, &value), ret,
                            RATE_SEM_UNLOCK(unit);RT_ERR(ret, (MOD_DAL|MOD_RATE), ""););

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_rate_portStormCtrlProtoBurstSize_get
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
 * Note:
 *      The unit of granularity is 1pps.
 */
int32
dal_longan_rate_portStormCtrlProtoBurstSize_get(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_proto_group_t  storm_type,
    uint32                 *pBurst_size)
{
    int32   ret = RT_ERR_FAILED;
    uint32  reg_idx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, storm_type=%d",
           unit, port, storm_type);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((storm_type >= STORM_PROTO_GROUP_END), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pBurst_size), RT_ERR_NULL_POINTER);

    switch (storm_type)
    {
        case STORM_PROTO_GROUP_BPDU:
            reg_idx = LONGAN_STORM_PORT_PROTO_BPDU_CTRLr;break;
        case STORM_PROTO_GROUP_IGMP:
            reg_idx = LONGAN_STORM_PORT_PROTO_IGMP_CTRLr;break;
        case STORM_PROTO_GROUP_ARP:
            reg_idx = LONGAN_STORM_PORT_PROTO_ARP_CTRLr;break;
        case STORM_PROTO_GROUP_DHCP:
            reg_idx = LONGAN_STORM_PORT_PROTO_DHCP_CTRLr;break;
        default:
            return RT_ERR_INPUT;
    }

    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_array_field_read(unit, reg_idx, port, REG_ARRAY_INDEX_NONE, LONGAN_BURSTf, pBurst_size)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_rate_portStormCtrlProtoBurstSize_set
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
 * Note:
 *      The unit of granularity is 1pps.
 */
int32
dal_longan_rate_portStormCtrlProtoBurstSize_set(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_proto_group_t  storm_type,
    uint32                 burst_size)
{
    int32   ret = RT_ERR_FAILED;
    uint32  reg_idx;
    uint32  rst_idx;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, storm_type=%d",
           unit, port, storm_type);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((storm_type >= STORM_PROTO_GROUP_END), RT_ERR_INPUT);
    RT_PARAM_CHK((burst_size >HAL_BURST_RATE_OF_STORM_PROTO_CONTROL_MAX(unit)), RT_ERR_INPUT);

    switch (storm_type)
    {
        case STORM_PROTO_GROUP_BPDU:
            reg_idx = LONGAN_STORM_PORT_PROTO_BPDU_CTRLr;
            rst_idx = LONGAN_STORM_PORT_PROTO_BPDU_LB_RSTr;
            break;
        case STORM_PROTO_GROUP_IGMP:
            reg_idx = LONGAN_STORM_PORT_PROTO_IGMP_CTRLr;
            rst_idx = LONGAN_STORM_PORT_PROTO_IGMP_LB_RSTr;
            break;
        case STORM_PROTO_GROUP_ARP:
            reg_idx = LONGAN_STORM_PORT_PROTO_ARP_CTRLr;
            rst_idx = LONGAN_STORM_PORT_PROTO_ARP_LB_RSTr;
            break;
        case STORM_PROTO_GROUP_DHCP:
            reg_idx = LONGAN_STORM_PORT_PROTO_DHCP_CTRLr;
            rst_idx = LONGAN_STORM_PORT_PROTO_DHCP_LB_RSTr;
            break;
        default:
            return RT_ERR_INPUT;
    }

    RATE_SEM_LOCK(unit);

    /* write value to CHIP */
    if ((ret = reg_array_field_write(unit, reg_idx, port, REG_ARRAY_INDEX_NONE, LONGAN_BURSTf, &burst_size)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }
    value = 1;
    RT_ERR_CHK_EHDL(reg_array_field_write(unit, rst_idx, port, REG_ARRAY_INDEX_NONE, LONGAN_RSTf, &value), ret,
                            RATE_SEM_UNLOCK(unit);RT_ERR(ret, (MOD_DAL|MOD_RATE), ""););

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_rate_portStormCtrlProtoExceed_get
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
 * Note:
 *      Exceed status is as following
 *      - TRUE      - storm rate is more than configured rate.
 *      - FALSE     - storm rate is never over then configured rate.
 */
int32
dal_longan_rate_portStormCtrlProtoExceed_get(
    uint32                          unit,
    rtk_port_t                      port,
    rtk_rate_storm_proto_group_t    storm_type,
    uint32                          *pIsExceed)
{
    int32   ret = RT_ERR_FAILED;
    uint32  reg_idx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, storm_type=%d",
           unit, port, storm_type);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((storm_type >= STORM_PROTO_GROUP_END), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pIsExceed), RT_ERR_NULL_POINTER);

    switch (storm_type)
    {
        case STORM_PROTO_GROUP_BPDU:
            reg_idx = LONGAN_STORM_PORT_PROTO_BPDU_EXCEED_FLAGr;break;
        case STORM_PROTO_GROUP_IGMP:
            reg_idx = LONGAN_STORM_PORT_PROTO_IGMP_EXCEED_FLAGr;break;
        case STORM_PROTO_GROUP_ARP:
            reg_idx = LONGAN_STORM_PORT_PROTO_ARP_EXCEED_FLAGr;break;
        case STORM_PROTO_GROUP_DHCP:
            reg_idx = LONGAN_STORM_PORT_PROTO_DHCP_EXCEED_FLAGr;break;
        default:
            return RT_ERR_INPUT;
    }

    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_array_field_read(unit, reg_idx, port, REG_ARRAY_INDEX_NONE, LONGAN_FLAGf, pIsExceed)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_longan_rate_portStormCtrlProtoExceed_reset
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
 * Note:
 *      None
 */
int32
dal_longan_rate_portStormCtrlProtoExceed_reset(
    uint32                          unit,
    rtk_port_t                      port,
    rtk_rate_storm_proto_group_t    storm_type)
{
    int32   ret = RT_ERR_FAILED;
    uint32  reg_idx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, storm_type=%d",
           unit, port, storm_type);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((storm_type >= STORM_PROTO_GROUP_END), RT_ERR_INPUT);

    switch (storm_type)
    {
        case STORM_PROTO_GROUP_BPDU:
            reg_idx = LONGAN_STORM_PORT_PROTO_BPDU_EXCEED_FLAGr;
            break;
        case STORM_PROTO_GROUP_IGMP:
            reg_idx = LONGAN_STORM_PORT_PROTO_IGMP_EXCEED_FLAGr;
            break;
        case STORM_PROTO_GROUP_ARP:
            reg_idx = LONGAN_STORM_PORT_PROTO_ARP_EXCEED_FLAGr;
            break;
        case STORM_PROTO_GROUP_DHCP:
            reg_idx = LONGAN_STORM_PORT_PROTO_DHCP_EXCEED_FLAGr;
            break;
        default:
            return RT_ERR_INPUT;
    }

    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_array_field_write1toClear(unit, reg_idx, port, REG_ARRAY_INDEX_NONE, LONGAN_FLAGf)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }


    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_longan_rate_stormCtrlProtoVlanConstrtEnable_get
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
 * Note:
 *      Supported protocol storm group types are as following:
 *      - STORM_PROTO_GROUP_ARP
 *      - STORM_PROTO_GROUP_DHCP
 */
int32
dal_longan_rate_stormCtrlProtoVlanConstrtEnable_get(uint32 unit, rtk_rate_storm_proto_group_t storm_type, rtk_enable_t *pEnable)
{
    int32   ret = RT_ERR_FAILED;
    uint32 value;
    uint32  reg_field;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, storm_type=%d",
           unit, storm_type);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    switch (storm_type)
    {
        case STORM_PROTO_GROUP_ARP:
            reg_field = LONGAN_ARP_VLAN_CONSTRTf;break;
        case STORM_PROTO_GROUP_DHCP:
            reg_field = LONGAN_DHCP_VLAN_CONSTRTf;break;
        default:
            return RT_ERR_INPUT;
    }

    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_field_read(unit,  LONGAN_STORM_CTRLr, reg_field, &value)) != RT_ERR_OK)
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
}

/* Function Name:
 *      dal_longan_rate_stormCtrlProtoVlanConstrtEnable_set
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
 * Note:
 *      Supported protocol storm group types are as following:
 *      - STORM_PROTO_GROUP_ARP
 *      - STORM_PROTO_GROUP_DHCP
 */
int32
dal_longan_rate_stormCtrlProtoVlanConstrtEnable_set(uint32 unit, rtk_rate_storm_proto_group_t storm_type, rtk_enable_t enable)
{
    int32   ret = RT_ERR_FAILED;
    uint32 value;
    uint32  reg_field;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, storm_type=%d",
           unit, storm_type);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    /* parameter check */
    switch (storm_type)
    {
        case STORM_PROTO_GROUP_ARP:
            reg_field = LONGAN_ARP_VLAN_CONSTRTf;break;
        case STORM_PROTO_GROUP_DHCP:
            reg_field = LONGAN_DHCP_VLAN_CONSTRTf;break;
        default:
            return RT_ERR_INPUT;
    }

    if (enable == ENABLED)
        value = 1;
    else
        value = 0;

    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_field_write(unit,  LONGAN_STORM_CTRLr, reg_field, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}


