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
#include <hal/chipdef/driver.h>
#include <hal/chipdef/mango/rtk_mango_table_struct.h>
#include <hal/chipdef/mango/rtk_mango_reg_struct.h>
#include <hal/mac/reg.h>
#include <hal/mac/mem.h>
#include <hal/mac/serdes.h>
#include <hal/mac/drv/drv_rtl9310.h>
#include <hal/common/halctrl.h>
#include <dal/mango/dal_mango_rate.h>
#include <dal/mango/dal_mango_port.h>
#include <rtk/default.h>
#include <rtk/rate.h>

/*
 * Symbol Definition
 */
#define NORMAL_PORT_ID_MAX                  (51)

/*
 * Data Declaration
 */
static uint32               rate_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};
static osal_mutex_t         rate_sem[RTK_MAX_NUM_OF_UNIT];
static uint32               *pAssure_rate[RTK_MAX_NUM_OF_UNIT][RTK_MAX_NUM_OF_PORTS][MANGO_EGRBW_MAX_NUM_OF_ASSUREDQ];
static uint32               *pEgrPort_rate[RTK_MAX_NUM_OF_UNIT][RTK_MAX_NUM_OF_PORTS];
static rtk_bitmap_t         *pAssureQEn[RTK_MAX_NUM_OF_UNIT][RTK_MAX_NUM_OF_PORTS];
static rtk_bitmap_t         *pAssureQShare[RTK_MAX_NUM_OF_UNIT][RTK_MAX_NUM_OF_PORTS];

const static uint16 egrQBwEn_fieldidx[] = {MANGO_EGR_Q_BW_MAX_BW_EN_Q0tf, MANGO_EGR_Q_BW_MAX_BW_EN_Q1tf,
                                           MANGO_EGR_Q_BW_MAX_BW_EN_Q2tf, MANGO_EGR_Q_BW_MAX_BW_EN_Q3tf,
                                           MANGO_EGR_Q_BW_MAX_BW_EN_Q4tf, MANGO_EGR_Q_BW_MAX_BW_EN_Q5tf,
                                           MANGO_EGR_Q_BW_MAX_BW_EN_Q6tf, MANGO_EGR_Q_BW_MAX_BW_EN_Q7tf,
                                           MANGO_EGR_Q_BW_MAX_BW_EN_Q8tf, MANGO_EGR_Q_BW_MAX_BW_EN_Q9tf,
                                           MANGO_EGR_Q_BW_MAX_BW_EN_Q10tf, MANGO_EGR_Q_BW_MAX_BW_EN_Q11tf};

const static uint16 egrQBwRate_fieldidx[] = {MANGO_EGR_Q_BW_MAX_BW_Q0tf,  MANGO_EGR_Q_BW_MAX_BW_Q1tf,
                                             MANGO_EGR_Q_BW_MAX_BW_Q2tf,  MANGO_EGR_Q_BW_MAX_BW_Q3tf,
                                             MANGO_EGR_Q_BW_MAX_BW_Q4tf,  MANGO_EGR_Q_BW_MAX_BW_Q5tf,
                                             MANGO_EGR_Q_BW_MAX_BW_Q6tf,  MANGO_EGR_Q_BW_MAX_BW_Q7tf,
                                             MANGO_EGR_Q_BW_MAX_BW_Q8tf,  MANGO_EGR_Q_BW_MAX_BW_Q9tf,
                                             MANGO_EGR_Q_BW_MAX_BW_Q10tf, MANGO_EGR_Q_BW_MAX_BW_Q11tf};

const static uint16 egrQBwBurst_fieldidx[] = {MANGO_EGR_Q_BW_MAX_LB_BURST_Q0tf, MANGO_EGR_Q_BW_MAX_LB_BURST_Q1tf,
                                              MANGO_EGR_Q_BW_MAX_LB_BURST_Q2tf, MANGO_EGR_Q_BW_MAX_LB_BURST_Q3tf,
                                              MANGO_EGR_Q_BW_MAX_LB_BURST_Q4tf, MANGO_EGR_Q_BW_MAX_LB_BURST_Q5tf,
                                              MANGO_EGR_Q_BW_MAX_LB_BURST_Q6tf, MANGO_EGR_Q_BW_MAX_LB_BURST_Q7tf,
                                              MANGO_EGR_Q_BW_MAX_LB_BURST_Q8tf, MANGO_EGR_Q_BW_MAX_LB_BURST_Q9tf,
                                              MANGO_EGR_Q_BW_MAX_LB_BURST_Q10tf, MANGO_EGR_Q_BW_MAX_LB_BURST_Q11tf};

const static uint16 egrAssureQBwEn_fieldidx[] = {MANGO_EGR_Q_BW_ASSURED_BW_EN_Q0tf, MANGO_EGR_Q_BW_ASSURED_BW_EN_Q1tf,
                                                MANGO_EGR_Q_BW_ASSURED_BW_EN_Q2tf, MANGO_EGR_Q_BW_ASSURED_BW_EN_Q3tf,
                                                MANGO_EGR_Q_BW_ASSURED_BW_EN_Q4tf, MANGO_EGR_Q_BW_ASSURED_BW_EN_Q5tf,
                                                MANGO_EGR_Q_BW_ASSURED_BW_EN_Q6tf, MANGO_EGR_Q_BW_ASSURED_BW_EN_Q7tf,
                                                MANGO_EGR_Q_BW_ASSURED_BW_EN_Q8tf, MANGO_EGR_Q_BW_ASSURED_BW_EN_Q9tf,
                                                MANGO_EGR_Q_BW_ASSURED_BW_EN_Q10tf, MANGO_EGR_Q_BW_ASSURED_BW_EN_Q11tf};

const static uint16 egrAssureQBwRate_fieldidx[] = {MANGO_EGR_Q_BW_ASSURED_BW_Q0tf, MANGO_EGR_Q_BW_ASSURED_BW_Q1tf,
                                                  MANGO_EGR_Q_BW_ASSURED_BW_Q2tf, MANGO_EGR_Q_BW_ASSURED_BW_Q3tf,
                                                  MANGO_EGR_Q_BW_ASSURED_BW_Q4tf, MANGO_EGR_Q_BW_ASSURED_BW_Q5tf,
                                                  MANGO_EGR_Q_BW_ASSURED_BW_Q6tf, MANGO_EGR_Q_BW_ASSURED_BW_Q7tf,
                                                  MANGO_EGR_Q_BW_ASSURED_BW_Q8tf, MANGO_EGR_Q_BW_ASSURED_BW_Q9tf,
                                                  MANGO_EGR_Q_BW_ASSURED_BW_Q10tf, MANGO_EGR_Q_BW_ASSURED_BW_Q11tf};

const static uint16 egrAssureQBwBurst_fieldidx[] = {MANGO_EGR_Q_BW_ASSURED_LB_BURST_Q0tf, MANGO_EGR_Q_BW_ASSURED_LB_BURST_Q1tf,
                                                   MANGO_EGR_Q_BW_ASSURED_LB_BURST_Q2tf, MANGO_EGR_Q_BW_ASSURED_LB_BURST_Q3tf,
                                                   MANGO_EGR_Q_BW_ASSURED_LB_BURST_Q4tf, MANGO_EGR_Q_BW_ASSURED_LB_BURST_Q5tf,
                                                   MANGO_EGR_Q_BW_ASSURED_LB_BURST_Q6tf, MANGO_EGR_Q_BW_ASSURED_LB_BURST_Q7tf,
                                                   MANGO_EGR_Q_BW_ASSURED_LB_BURST_Q8tf, MANGO_EGR_Q_BW_ASSURED_LB_BURST_Q9tf,
                                                   MANGO_EGR_Q_BW_ASSURED_LB_BURST_Q10tf, MANGO_EGR_Q_BW_ASSURED_LB_BURST_Q11tf};

const static uint16 egrAssureQBwMode_fieldidx[] = {MANGO_EGR_Q_BW_ASSURED_MODE_Q0tf, MANGO_EGR_Q_BW_ASSURED_MODE_Q1tf,
                                              MANGO_EGR_Q_BW_ASSURED_MODE_Q2tf, MANGO_EGR_Q_BW_ASSURED_MODE_Q3tf,
                                              MANGO_EGR_Q_BW_ASSURED_MODE_Q4tf, MANGO_EGR_Q_BW_ASSURED_MODE_Q5tf,
                                              MANGO_EGR_Q_BW_ASSURED_MODE_Q6tf, MANGO_EGR_Q_BW_ASSURED_MODE_Q7tf,
                                              MANGO_EGR_Q_BW_ASSURED_MODE_Q8tf, MANGO_EGR_Q_BW_ASSURED_MODE_Q9tf,
                                              MANGO_EGR_Q_BW_ASSURED_MODE_Q10tf, MANGO_EGR_Q_BW_ASSURED_MODE_Q11tf};

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
static int32
_dal_mango_rate_portBurstSize_check(uint32 unit, rtk_port_t port, rtk_rate_module_t type, uint32 burst)
{
    int32   ret = RT_ERR_OK;
    uint32  mode, egr_bps = 0;
    uint32  token;
    uint32  reg;
    uint32  field;


    RATE_SEM_LOCK(unit);

    if (type == RATE_MODULE_IGR)
    {
        reg = MANGO_IGBW_WFQ_LB_CTRLr;
        field = MANGO_TKNf;
    }
    else if (type == RATE_MODULE_EGR)
    {
        /* get pps or bps mode of CPU port from CHIP */
        if (HWP_IS_CPU_PORT(unit, port))
        {
            if ((ret = reg_field_read(unit, MANGO_EGBW_CTRLr, MANGO_RATE_MODE_CPUf, &egr_bps)) != RT_ERR_OK)
            {
                RATE_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
                return ret;
            }
            reg = ((egr_bps == 1) ? MANGO_EGBW_LB_CTRLr : MANGO_EGBW_CPU_PPS_LB_CTRLr);
            field = MANGO_TKNf;
        }
        else
        {
            egr_bps = 1;
            /* non-CPU port always using bps mode*/
            reg = MANGO_EGBW_LB_CTRLr;
            field = MANGO_TKNf;
        }
    }
    else if (type == RATE_MODULE_STORM)
    {
        /* get value from CHIP */
        if ((ret = reg_array_field_read(unit, MANGO_STORM_PORT_CTRLr, port, REG_ARRAY_INDEX_NONE, MANGO_MODEf, &mode)) != RT_ERR_OK)
        {
            RATE_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, mode=%d", unit, mode);
        reg = (mode == 0) ? MANGO_STORM_LB_PPS_CTRLr : MANGO_STORM_LB_CTRLr;
        field = MANGO_TKNf;
    }
    else
    {
        RATE_SEM_UNLOCK(unit);
        ret = RT_ERR_INPUT;
        return ret;
    }

    /* read token value from CHIP*/
    if ((ret = reg_field_read(unit, reg, field, &token)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    /* verify value is valid in the range*/
    if ((1 == egr_bps) && (burst < (token * 8)))
    {
        /* byte mode burst size (high threshold) of leaky bucket should not lower than 8*token */
        ret = RT_ERR_OUT_OF_RANGE;
    }
    else if (burst < token)
    {
        /* burst size (high threshold) of leaky bucket should not lower than token */
        ret = RT_ERR_OUT_OF_RANGE;
    }

    return ret;
}

static int32
_dal_mango_rate_portEgrBwCtrlRate_refill(uint32 unit, rtk_port_t port)
{
    int32           ret, index, budget;
    uint32          rate, egrPort_refill;
    rtk_bitmap_t    budget_array[BITMAP_ARRAY_SIZE(MANGO_EGRBW_MAX_NUM_OF_ASSUREDQ)];

    if (HWP_IS_CPU_PORT(unit,port))
    {
        rate = *pEgrPort_rate[unit][port];
    }
    else
    {
        BITMAP_RESET(budget_array, BITMAP_ARRAY_SIZE(MANGO_EGRBW_MAX_NUM_OF_ASSUREDQ));

        /* Use port rate and assured rate to calcalcute budget and init budget_array */
        budget = *pEgrPort_rate[unit][port];
        for (index = (MANGO_EGRBW_MAX_NUM_OF_ASSUREDQ-1); index >= 0; index--)
        {
            if (budget <= 0)
                 break;

            if (BITMAP_IS_SET(pAssureQEn[unit][port], index)) /* Assured BW enabled */
            {
                budget -= *pAssure_rate[unit][port][index];
                if (BITMAP_IS_SET(pAssureQShare[unit][port], index)) /* Share Mode */
                {
                     BITMAP_SET(budget_array, index);
                }
                else /* Fix Mode */
                {
                    if (budget > 0)
                    {
                        BITMAP_SET(budget_array, index);
                    }
                }
                RT_LOG(LOG_INFO, (MOD_DAL|MOD_RATE), "After allocating unit=%d, port=%d, queue=%2d, remain budget = %d", unit, port, index, budget);
            }
        }
        RT_LOG(LOG_INFO, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, budget_array[]=0x%X", unit, port, *budget_array);

        BITMAP_AND(budget_array, pAssureQShare[unit][port], BITMAP_ARRAY_SIZE(MANGO_EGRBW_MAX_NUM_OF_ASSUREDQ));

        egrPort_refill = rt_bitop_numberOfSetBits(*budget_array);
        RT_LOG(LOG_INFO, (MOD_DAL|MOD_RATE), "egrPort_refill = %d", egrPort_refill);

        /* Refill bandwidth wouldn't over maximum bandwidth */
        if ((*pEgrPort_rate[unit][port] + egrPort_refill) >= HAL_RATE_OF_BANDWIDTH_MAX(unit))
            rate = HAL_RATE_OF_BANDWIDTH_MAX(unit);
        else
            rate = *pEgrPort_rate[unit][port] + egrPort_refill;
    }

    if ((ret = reg_array_field_write(unit, MANGO_EGBW_PORT_CTRLr, port, REG_ARRAY_INDEX_NONE, MANGO_RATEf, &rate)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RT_LOG(LOG_INFO, (MOD_DAL|MOD_RATE), "Set chip port rate = %d", rate);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_rate_portEgrQueueDrain_process
 * Description:
 *      Drain out egress queue of the specified port
 * Input:
 *      unit           - unit id
 *      port           - port id
 * Output:
 *      None
 * Return:
 *      SUCCESS
 *      FAILED
 * Note:
 *      None
 */
int32 dal_mango_rate_portEgrQueueDrain_process(int32 unit, rtk_port_t port)
{
    int32 ret;
    uint32  value, empty_flag = 0, times=0, portEn;
    egr_qBw_entry_t egrQEntry, orgQEntry;
    rtk_qid_t   queue;
    int32       rv1 = RT_ERR_OK, rv2 = RT_ERR_OK;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d\n", unit, port);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);

    osal_memset(&egrQEntry, 0, sizeof(egr_qBw_entry_t));
    osal_memset(&orgQEntry, 0, sizeof(egr_qBw_entry_t));

    RATE_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_array_field_read(unit, MANGO_EGBW_PORT_CTRLr, port, REG_ARRAY_INDEX_NONE, MANGO_ENf, &portEn)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }
    if ((ret = table_read(unit, MANGO_EGR_Q_BWt, port, (uint32 *) &orgQEntry)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    value = DISABLED;
    osal_memcpy(&egrQEntry, &orgQEntry, sizeof(egr_qBw_entry_t));
    for (queue = 0; queue < HAL_MAX_NUM_OF_STACK_QUEUE(unit); queue++)
    {
        if ((ret = table_field_set(unit, MANGO_EGR_Q_BWt, (uint32)egrAssureQBwEn_fieldidx[queue],
                        &value, (uint32 *) &egrQEntry)) != RT_ERR_OK)
        {
            RATE_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }
        if ((ret = table_field_set(unit, MANGO_EGR_Q_BWt, (uint32)egrQBwEn_fieldidx[queue],
                        &value, (uint32 *) &egrQEntry)) != RT_ERR_OK)
        {
            RATE_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }
    }

    if ((ret = table_write(unit, MANGO_EGR_Q_BWt, port, (uint32 *) &egrQEntry)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RT_ERR_HDL(reg_array_field_write(unit, MANGO_EGBW_PORT_CTRLr, port,
            REG_ARRAY_INDEX_NONE, MANGO_ENf, &value), ERR2, ret);

    do {
        RT_ERR_HDL(reg_array_field_read(unit, MANGO_PER_PORT_MAC_DEBUG0r, port, REG_ARRAY_INDEX_NONE,
                MANGO_TX_NO_PKTf, &empty_flag), ERR1, ret);

        osal_time_mdelay(1);
        times++;
    } while ((times<MANGO_EGRBW_Q_MAX_DRAINOUT_TIMER_MS) && (empty_flag==0));

    /* Restore chip value */
ERR1:
    if ((rv1 = reg_array_field_write(unit, MANGO_EGBW_PORT_CTRLr, port, REG_ARRAY_INDEX_NONE, MANGO_ENf, &portEn)) != RT_ERR_OK)
    {
        RT_ERR(rv1, (MOD_DAL|MOD_RATE), "");
    }
ERR2:
    if ((rv2 = table_write(unit, MANGO_EGR_Q_BWt, port, (uint32 *) &orgQEntry)) != RT_ERR_OK)
    {
        RT_ERR(rv2, (MOD_DAL|MOD_RATE), "");
    }

    RATE_SEM_UNLOCK(unit);

    if (rv1 != RT_ERR_OK)
    {
        return rv1;
    }
    else if (rv2 != RT_ERR_OK)
    {
        return rv2;
    }
    else if (empty_flag == 0)
    {
        return RT_ERR_FAILED;
    }

    return ret;
}

/* Function Name:
 *      dal_mango_rateMapper_init
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
dal_mango_rateMapper_init(dal_mapper_t *pMapper)
{


    /* rate function */
    pMapper->rate_init = dal_mango_rate_init;
    pMapper->rate_includeIfg_get = dal_mango_rate_includeIfg_get;
    pMapper->rate_includeIfg_set = dal_mango_rate_includeIfg_set;
    pMapper->rate_igrBwCtrlBurstSize_get = dal_mango_rate_igrBwCtrlBurstSize_get;
    pMapper->rate_igrBwCtrlBurstSize_set = dal_mango_rate_igrBwCtrlBurstSize_set;
    pMapper->rate_igrPortBwCtrlBurstSize_get = dal_mango_rate_igrPortBwCtrlBurstSize_get;
    pMapper->rate_igrPortBwCtrlBurstSize_set = dal_mango_rate_igrPortBwCtrlBurstSize_set;
    pMapper->rate_portIgrBwCtrlRate_get = dal_mango_rate_portIgrBwCtrlRate_get;
    pMapper->rate_portIgrBwCtrlRate_set = dal_mango_rate_portIgrBwCtrlRate_set;
    pMapper->rate_portIgrBwCtrlBurstSize_get = dal_mango_rate_portIgrBwCtrlBurstSize_get;
    pMapper->rate_portIgrBwCtrlBurstSize_set = dal_mango_rate_portIgrBwCtrlBurstSize_set;
    pMapper->rate_portIgrBwCtrlEnable_get = dal_mango_rate_portIgrBwCtrlEnable_get;
    pMapper->rate_portIgrBwCtrlEnable_set = dal_mango_rate_portIgrBwCtrlEnable_set;
    pMapper->rate_portIgrBwFlowctrlEnable_get = dal_mango_rate_portIgrBwFlowctrlEnable_get;
    pMapper->rate_portIgrBwFlowctrlEnable_set = dal_mango_rate_portIgrBwFlowctrlEnable_set;
    pMapper->rate_igrBandwidthCtrlBypass_get = dal_mango_rate_igrBandwidthCtrlBypass_get;
    pMapper->rate_igrBandwidthCtrlBypass_set = dal_mango_rate_igrBandwidthCtrlBypass_set;
    pMapper->rate_portIgrBandwidthCtrlExceed_get = dal_mango_rate_portIgrBandwidthCtrlExceed_get;
    pMapper->rate_portIgrBandwidthCtrlExceed_reset = dal_mango_rate_portIgrBandwidthCtrlExceed_reset;
    pMapper->rate_portIgrQueueBwCtrlRate_get = dal_mango_rate_portIgrQueueBwCtrlRate_get;
    pMapper->rate_portIgrQueueBwCtrlRate_set = dal_mango_rate_portIgrQueueBwCtrlRate_set;
    pMapper->rate_igrQueueBwCtrlBurstSize_get = dal_mango_rate_igrQueueBwCtrlBurstSize_get;
    pMapper->rate_igrQueueBwCtrlBurstSize_set = dal_mango_rate_igrQueueBwCtrlBurstSize_set;
    pMapper->rate_portIgrQueueBwCtrlEnable_get = dal_mango_rate_portIgrQueueBwCtrlEnable_get;
    pMapper->rate_portIgrQueueBwCtrlEnable_set = dal_mango_rate_portIgrQueueBwCtrlEnable_set;
    pMapper->rate_portIgrQueueBwCtrlBurstSize_get = dal_mango_rate_portIgrQueueBwCtrlBurstSize_get;
    pMapper->rate_portIgrQueueBwCtrlBurstSize_set = dal_mango_rate_portIgrQueueBwCtrlBurstSize_set;
    pMapper->rate_portIgrQueueBwCtrlExceed_get = dal_mango_rate_portIgrQueueBwCtrlExceed_get;
    pMapper->rate_portIgrQueueBwCtrlExceed_reset = dal_mango_rate_portIgrQueueBwCtrlExceed_reset;
    pMapper->rate_portEgrBwCtrlEnable_get = dal_mango_rate_portEgrBwCtrlEnable_get;
    pMapper->rate_portEgrBwCtrlEnable_set = dal_mango_rate_portEgrBwCtrlEnable_set;
    pMapper->rate_portEgrBwCtrlRate_get = dal_mango_rate_portEgrBwCtrlRate_get;
    pMapper->rate_portEgrBwCtrlRate_set = dal_mango_rate_portEgrBwCtrlRate_set;
    pMapper->rate_portEgrBwCtrlBurstSize_get = dal_mango_rate_portEgrBwCtrlBurstSize_get;
    pMapper->rate_portEgrBwCtrlBurstSize_set = dal_mango_rate_portEgrBwCtrlBurstSize_set;
    pMapper->rate_cpuEgrBandwidthCtrlRateMode_get = dal_mango_rate_cpuEgrBandwidthCtrlRateMode_get;
    pMapper->rate_cpuEgrBandwidthCtrlRateMode_set = dal_mango_rate_cpuEgrBandwidthCtrlRateMode_set;
    pMapper->rate_egrPortBwCtrlBurstSize_get = dal_mango_rate_egrPortBwCtrlBurstSize_get;
    pMapper->rate_egrPortBwCtrlBurstSize_set = dal_mango_rate_egrPortBwCtrlBurstSize_set;
    pMapper->rate_portEgrQueueBwCtrlEnable_get = dal_mango_rate_portEgrQueueBwCtrlEnable_get;
    pMapper->rate_portEgrQueueBwCtrlEnable_set = dal_mango_rate_portEgrQueueBwCtrlEnable_set;
    pMapper->rate_portEgrQueueBwCtrlRate_get = dal_mango_rate_portEgrQueueBwCtrlRate_get;
    pMapper->rate_portEgrQueueBwCtrlRate_set = dal_mango_rate_portEgrQueueBwCtrlRate_set;
    pMapper->rate_portEgrQueueBwCtrlBurstSize_get = dal_mango_rate_portEgrQueueBwCtrlBurstSize_get;
    pMapper->rate_portEgrQueueBwCtrlBurstSize_set = dal_mango_rate_portEgrQueueBwCtrlBurstSize_set;
    pMapper->rate_portEgrQueueAssuredBwCtrlEnable_get = dal_mango_rate_portEgrQueueAssuredBwCtrlEnable_get;
    pMapper->rate_portEgrQueueAssuredBwCtrlEnable_set = dal_mango_rate_portEgrQueueAssuredBwCtrlEnable_set;
    pMapper->rate_portEgrQueueAssuredBwCtrlRate_get = dal_mango_rate_portEgrQueueAssuredBwCtrlRate_get;
    pMapper->rate_portEgrQueueAssuredBwCtrlRate_set = dal_mango_rate_portEgrQueueAssuredBwCtrlRate_set;
    pMapper->rate_portEgrQueueAssuredBwCtrlBurstSize_get = dal_mango_rate_portEgrQueueAssuredBwCtrlBurstSize_get;
    pMapper->rate_portEgrQueueAssuredBwCtrlBurstSize_set = dal_mango_rate_portEgrQueueAssuredBwCtrlBurstSize_set;
    pMapper->rate_portEgrQueueAssuredBwCtrlMode_get = dal_mango_rate_portEgrQueueAssuredBwCtrlMode_get;
    pMapper->rate_portEgrQueueAssuredBwCtrlMode_set = dal_mango_rate_portEgrQueueAssuredBwCtrlMode_set;
    pMapper->rate_egrQueueBwCtrlBurstSize_get = dal_mango_rate_egrQueueBwCtrlBurstSize_get;
    pMapper->rate_egrQueueBwCtrlBurstSize_set = dal_mango_rate_egrQueueBwCtrlBurstSize_set;
    pMapper->rate_portStormCtrlEnable_get = dal_mango_rate_portStormCtrlEnable_get;
    pMapper->rate_portStormCtrlEnable_set = dal_mango_rate_portStormCtrlEnable_set;
    pMapper->rate_portStormCtrlRate_get = dal_mango_rate_portStormCtrlRate_get;
    pMapper->rate_portStormCtrlRate_set = dal_mango_rate_portStormCtrlRate_set;
    pMapper->rate_portStormCtrlBurstSize_get = dal_mango_rate_portStormCtrlBurstSize_get;
    pMapper->rate_portStormCtrlBurstSize_set = dal_mango_rate_portStormCtrlBurstSize_set;
    pMapper->rate_portStormCtrlExceed_get = dal_mango_rate_portStormCtrlExceed_get;
    pMapper->rate_portStormCtrlExceed_reset = dal_mango_rate_portStormCtrlExceed_reset;
    pMapper->rate_portStormCtrlRateMode_get = dal_mango_rate_portStormCtrlRateMode_get;
    pMapper->rate_portStormCtrlRateMode_set = dal_mango_rate_portStormCtrlRateMode_set;
    pMapper->rate_portStormCtrlTypeSel_get = dal_mango_rate_portStormCtrlTypeSel_get;
    pMapper->rate_portStormCtrlTypeSel_set = dal_mango_rate_portStormCtrlTypeSel_set;
    pMapper->rate_portStormCtrlProtoEnable_get =  dal_mango_rate_portStormCtrlProtoEnable_get;
    pMapper->rate_portStormCtrlProtoEnable_set =  dal_mango_rate_portStormCtrlProtoEnable_set;
    pMapper->rate_portStormCtrlProtoRate_get = dal_mango_rate_portStormCtrlProtoRate_get;
    pMapper->rate_portStormCtrlProtoRate_set = dal_mango_rate_portStormCtrlProtoRate_set;
    pMapper->rate_portStormCtrlProtoBurstSize_get = dal_mango_rate_portStormCtrlProtoBurstSize_get;
    pMapper->rate_portStormCtrlProtoBurstSize_set = dal_mango_rate_portStormCtrlProtoBurstSize_set;
    pMapper->rate_portStormCtrlProtoExceed_get = dal_mango_rate_portStormCtrlProtoExceed_get;
    pMapper->rate_portStormCtrlProtoExceed_reset = dal_mango_rate_portStormCtrlProtoExceed_reset;
    pMapper->rate_stormControlBypass_get = dal_mango_rate_stormControlBypass_get;
    pMapper->rate_stormControlBypass_set = dal_mango_rate_stormControlBypass_set;
    pMapper->rate_stormControlBurstSize_get = dal_mango_rate_stormControlBurstSize_get;
    pMapper->rate_stormControlBurstSize_set = dal_mango_rate_stormControlBurstSize_set;
    pMapper->rate_stormControlRateMode_get = dal_mango_rate_stormControlRateMode_get;
    pMapper->rate_stormControlRateMode_set = dal_mango_rate_stormControlRateMode_set;
    pMapper->rate_stormCtrlProtoVlanConstrtEnable_get = dal_mango_rate_stormCtrlProtoVlanConstrtEnable_get;
    pMapper->rate_stormCtrlProtoVlanConstrtEnable_set = dal_mango_rate_stormCtrlProtoVlanConstrtEnable_set;

#if CODE_TBC
    pMapper->rate_stormControlIncludeIfg_get = dal_mango_rate_stormControlIncludeIfg_get;
    pMapper->rate_stormControlIncludeIfg_set = dal_mango_rate_stormControlIncludeIfg_set;
    pMapper->rate_stormControlExceed_get = dal_mango_rate_stormControlExceed_get;
    pMapper->rate_stormControlExceed_reset = dal_mango_rate_stormControlExceed_reset;
    pMapper->rate_stormControlProtoExceed_get = dal_mango_rate_stormControlProtoExceed_get;
    pMapper->rate_stormControlProtoExceed_reset = dal_mango_rate_stormControlProtoExceed_reset;
    pMapper->rate_stormControlTypeSel_get = dal_mango_rate_stormControlTypeSel_get;
    pMapper->rate_stormControlTypeSel_set = dal_mango_rate_stormControlTypeSel_set;
#endif /* CODE_TBC */

    return RT_ERR_OK;
}

static int32 _dal_mango_rate_init_config(uint32 unit);

/* Function Name:
 *      dal_mango_rate_init
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
dal_mango_rate_init(uint32 unit)
{
    /* [SS-972]-START */
    int32 ret = RT_ERR_OK;
    rtk_port_t  port;
    rtk_qid_t   queue;
    /* [SS-972]-END */

    RT_INIT_REENTRY_CHK(rate_init[unit]);
    rate_init[unit] = INIT_NOT_COMPLETED;

    /* create semaphore */
    rate_sem[unit] = osal_sem_mutex_create();
    if (0 == rate_sem[unit])
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_RATE), "semaphore create failed");
        return RT_ERR_FAILED;
    }

    /* [SS-972]-START */
    /* Allocate and initilize memory for Assured BW database */
    osal_memset(pEgrPort_rate[unit], 0, (sizeof(uint32) * RTK_MAX_NUM_OF_PORTS));
    HWP_PORT_TRAVS(unit, port)
    {
        pEgrPort_rate[unit][port] = (uint32 *)osal_alloc(sizeof(uint32));
        if (NULL == pEgrPort_rate[unit][port])
        {
            RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_RATE), "memory allocate failed");
            goto ERR1;
        }
        osal_memset(pEgrPort_rate[unit][port], 0, sizeof(uint32));
    }

    osal_memset(pAssureQEn[unit], 0, (sizeof(uint32) * RTK_MAX_NUM_OF_PORTS));
    osal_memset(pAssureQShare[unit], 0, (sizeof(uint32) * RTK_MAX_NUM_OF_PORTS));
    HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
    {
        osal_memset(pAssure_rate[unit][port], 0, (sizeof(uint32) * MANGO_EGRBW_MAX_NUM_OF_ASSUREDQ));
    }
    HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
    {
        pAssureQEn[unit][port] = (rtk_bitmap_t *)osal_alloc(BITMAP_ARRAY_SIZE(MANGO_EGRBW_MAX_NUM_OF_ASSUREDQ));
        if (NULL == pAssureQEn[unit][port])
        {
            RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_RATE), "memory allocate failed");
            goto ERR2;
        }
        pAssureQShare[unit][port] = (rtk_bitmap_t *)osal_alloc(BITMAP_ARRAY_SIZE(MANGO_EGRBW_MAX_NUM_OF_ASSUREDQ));
        if (NULL == pAssureQShare[unit][port])
        {
            RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_RATE), "memory allocate failed");
            goto ERR2;
        }
        osal_memset(pAssureQEn[unit][port], 0, BITMAP_ARRAY_SIZE(MANGO_EGRBW_MAX_NUM_OF_ASSUREDQ));
        osal_memset(pAssureQShare[unit][port], 0, BITMAP_ARRAY_SIZE(MANGO_EGRBW_MAX_NUM_OF_ASSUREDQ));

        for (queue = 0; queue < MANGO_EGRBW_MAX_NUM_OF_ASSUREDQ; queue++)
        {
            pAssure_rate[unit][port][queue] = (uint32 *)osal_alloc(sizeof(uint32));
            if ((NULL == pAssure_rate[unit][port][queue]))
            {
                RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_RATE), "memory allocate failed");
                goto ERR2;
            }
            osal_memset(pAssure_rate[unit][port][queue], 0, sizeof(uint32));
        }
    }

    if (( ret = _dal_mango_rate_init_config(unit)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }
    /* [SS-972]-END */

    /* set init flag to complete init */
    rate_init[unit] = INIT_COMPLETED;

    return RT_ERR_OK;

    /* [SS-972]-START */
ERR2:
    HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
    {
        if (NULL != pAssureQEn[unit][port])
        {
            osal_free(pAssureQEn[unit][port]);
            pAssureQEn[unit][port] = NULL;
        }
        if (NULL != pAssureQShare[unit][port])
        {
            osal_free(pAssureQShare[unit][port]);
            pAssureQShare[unit][port] = NULL;
        }
        for (queue = 0; queue < MANGO_EGRBW_MAX_NUM_OF_ASSUREDQ; queue++)
        {
            if (NULL != pAssure_rate[unit][port][queue])
            {
                osal_free(pAssure_rate[unit][port][queue]);
                pAssure_rate[unit][port][queue] = NULL;
            }
        }
    }
ERR1:
    HWP_PORT_TRAVS(unit, port)
    {
        if (NULL != pEgrPort_rate[unit][port])
        {
            osal_free(pEgrPort_rate[unit][port]);
            pEgrPort_rate[unit][port] = NULL;
        }
    }

    return ret;
    /* [SS-972]-END */
}

/* Module Name    : Rate                                            */
/* Sub-module Name: Configuration of ingress port bandwidth control */

/* Function Name:
 *      dal_mango_rate_includeIfg_get
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
dal_mango_rate_includeIfg_get(uint32 unit, rtk_rate_module_t module, rtk_enable_t *pIfg_include)
{
    int32   ret;
    uint32  value;
    uint32 reg;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, module=%d", unit, module);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIfg_include), RT_ERR_NULL_POINTER);

    switch(module)
    {
        case RATE_MODULE_IGR:
            reg = MANGO_IGBW_CTRLr;
            break;
        case RATE_MODULE_EGR:
           reg = MANGO_EGBW_CTRLr;
           break;
        case RATE_MODULE_STORM:
            reg = MANGO_STORM_CTRLr;
            break;
        default:
            return RT_ERR_INPUT;
    }

    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_field_read(unit, reg, MANGO_INC_IFGf, &value)) != RT_ERR_OK)
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
 *      dal_mango_rate_includeIfg_set
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
dal_mango_rate_includeIfg_set(uint32 unit, rtk_rate_module_t module, rtk_enable_t ifg_include)
{
    int32   ret;
    uint32  value;
    uint32 reg;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, module=%d, enable=%d\n", unit, module, ifg_include);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((ifg_include >= RTK_ENABLE_END), RT_ERR_INPUT);

    switch(module)
    {
        case RATE_MODULE_IGR:
            reg = MANGO_IGBW_CTRLr;
            break;
        case RATE_MODULE_EGR:
           reg = MANGO_EGBW_CTRLr;
           break;
        case RATE_MODULE_STORM:
            reg = MANGO_STORM_CTRLr;
            break;
        default:
            return RT_ERR_INPUT;
    }

    if (ifg_include == ENABLED)
        value = 1;
    else
        value = 0;

    RATE_SEM_LOCK(unit);

    /* write value to CHIP*/
    if ((ret = reg_field_write(unit, reg, MANGO_INC_IFGf, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_rate_portIgrBwCtrlEnable_get
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
dal_mango_rate_portIgrBwCtrlEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
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
    if ((ret = reg_array_field_read(unit, MANGO_IGBW_PORT_CTRLr, port, REG_ARRAY_INDEX_NONE, MANGO_ENf, &value)) != RT_ERR_OK)
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
 *      dal_mango_rate_portIgrBwCtrlEnable_set
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
dal_mango_rate_portIgrBwCtrlEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
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
    if ((ret = reg_array_field_write(unit, MANGO_IGBW_PORT_CTRLr, port, REG_ARRAY_INDEX_NONE, MANGO_ENf, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_rate_igrBwCtrlBurstSize_get
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
dal_mango_rate_igrBwCtrlBurstSize_get(uint32 unit, uint32 *pBurst_size)
{
    int32 ret;
    rtk_rate_igrBwBurst_cfg_t cfg;
    rtk_port_t      port;

    RT_PARAM_CHK((NULL == pBurst_size), RT_ERR_NULL_POINTER);

    HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
    {
        if ((ret = dal_mango_rate_portIgrBwCtrlBurstSize_get(unit, port, &cfg)) != RT_ERR_OK)
            return ret;

        *pBurst_size = cfg.burst_high;
        return RT_ERR_OK;
    }

    return RT_ERR_FAILED;
}

/* Function Name:
 *      dal_mango_rate_igrBwCtrlBurstSize_set
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
dal_mango_rate_igrBwCtrlBurstSize_set(uint32 unit, uint32 burst_size)
{
    int32 ret;
    rtk_port_t port;
    rtk_rate_igrBwBurst_cfg_t cfg;

    HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
    {
        cfg.burst_low= cfg.burst_high = burst_size;

        if ((ret = dal_mango_rate_portIgrBwCtrlBurstSize_set(unit, port, &cfg)) != RT_ERR_OK)
            return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_rate_igrPortBwCtrlBurstSize_get
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
dal_mango_rate_igrPortBwCtrlBurstSize_get(
    uint32                  unit,
    uint32                  *pBurst_size)
{
    int32 ret;
    rtk_rate_igrBwBurst_cfg_t cfg;
    rtk_port_t      port;

    RT_PARAM_CHK((NULL == pBurst_size), RT_ERR_NULL_POINTER);

    HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
    {
        if (RT_ERR_OK != (ret = dal_mango_rate_portIgrBwCtrlBurstSize_get(unit, port, &cfg)))
            return ret;

        *pBurst_size = cfg.burst_high;
        return RT_ERR_OK;
    }

    return RT_ERR_FAILED;
}

/* Function Name:
 *      dal_mango_rate_igrPortBwCtrlBurstSize_set
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
dal_mango_rate_igrPortBwCtrlBurstSize_set(
    uint32                  unit,
    uint32                  burst_size)
{
    int32 ret;
    rtk_port_t port;
    rtk_rate_igrBwBurst_cfg_t cfg;

    HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
    {
        cfg.burst_low= cfg.burst_high = burst_size;

        if(RT_ERR_OK != (ret = dal_mango_rate_portIgrBwCtrlBurstSize_set(unit, port, &cfg)))
            return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_rate_portIgrBwCtrlRate_get
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
dal_mango_rate_portIgrBwCtrlRate_get(uint32 unit, rtk_port_t port, uint32 *pRate)
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
    if ((ret = reg_array_field_read(unit, MANGO_IGBW_PORT_CTRLr, port, REG_ARRAY_INDEX_NONE, MANGO_RATEf, pRate)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_rate_portIgrBwCtrlRate_set
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
dal_mango_rate_portIgrBwCtrlRate_set(uint32 unit, rtk_port_t port, uint32 rate)
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
    if ((ret = reg_array_field_write(unit, MANGO_IGBW_PORT_CTRLr, port, REG_ARRAY_INDEX_NONE, MANGO_RATEf, &rate)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_rate_portIgrBwCtrlBurstSize_get
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
dal_mango_rate_portIgrBwCtrlBurstSize_get(uint32 unit, rtk_port_t port, rtk_rate_igrBwBurst_cfg_t *pCfg)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d\n", unit, port);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pCfg), RT_ERR_NULL_POINTER);

    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_array_field_read(unit, MANGO_IGBW_PORT_CTRLr, port, REG_ARRAY_INDEX_NONE, MANGO_BURSTf, &pCfg->burst_high)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }
    pCfg->burst_low = pCfg->burst_high;

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_rate_portIgrBwCtrlBurstSize_set
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
dal_mango_rate_portIgrBwCtrlBurstSize_set(uint32 unit, rtk_port_t port, rtk_rate_igrBwBurst_cfg_t *pCfg)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d\n", unit, port);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pCfg), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pCfg->burst_high > HAL_BURST_OF_IGR_BANDWIDTH_MAX(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK(_dal_mango_rate_portBurstSize_check(unit, port, RATE_MODULE_IGR, pCfg->burst_high), RT_ERR_INPUT);

    RATE_SEM_LOCK(unit);

    /* write value to CHIP*/
    if ((ret = reg_array_field_write(unit, MANGO_IGBW_PORT_CTRLr, port, REG_ARRAY_INDEX_NONE, MANGO_BURSTf, &pCfg->burst_high)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_rate_portIgrBandwidthCtrlExceed_get
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
dal_mango_rate_portIgrBandwidthCtrlExceed_get(
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
    if ((ret = reg_array_field_read(unit, MANGO_IGBW_PORT_EXCEED_FLAGr, port, REG_ARRAY_INDEX_NONE, MANGO_FLAGf, pIsExceed)) != RT_ERR_OK)
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
 *      dal_mango_rate_portIgrBandwidthCtrlExceed_reset
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
dal_mango_rate_portIgrBandwidthCtrlExceed_reset(
    uint32                  unit,
    rtk_port_t              port)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d\n", unit, port);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);

    RATE_SEM_LOCK(unit);

    /*write 1 to clear*/
    if ((ret = reg_array_field_write1toClear(unit, MANGO_IGBW_PORT_EXCEED_FLAGr, port, REG_ARRAY_INDEX_NONE, MANGO_FLAGf)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_mango_rate_igrBandwidthCtrlBypass_get
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
dal_mango_rate_igrBandwidthCtrlBypass_get(uint32 unit, rtk_rate_igr_bypassType_t bypassType, rtk_enable_t  *pEnable)
{
    int32   ret;
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
            regField = MANGO_ADMIT_ARPREQf;
            break;
        case IGR_BYPASS_RMA:
            regField = MANGO_ADMIT_RMAf;
            break;
        case IGR_BYPASS_BPDU:
            regField = MANGO_ADMIT_BPDUf;
            break;
        case IGR_BYPASS_RTKPKT:
            regField = MANGO_ADMIT_RTKPKTf;
            break;
        case IGR_BYPASS_IGMP:
            regField = MANGO_ADMIT_IGMPf;
            break;
        case IGR_BYPASS_DHCP:
            regField = MANGO_ADMIT_DHCPf;
            break;
        case IGR_BYPASS_RIP:
            regField = MANGO_ADMIT_RIPf;
            break;
        default:
            return RT_ERR_INPUT;
    }

    RATE_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_field_read(unit, MANGO_IGBW_CTRLr, regField, &value)) != RT_ERR_OK)
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
 *      dal_mango_rate_igrBandwidthCtrlBypass_set
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
dal_mango_rate_igrBandwidthCtrlBypass_set(uint32 unit, rtk_rate_igr_bypassType_t bypassType, rtk_enable_t enable)
{
    int32   ret;
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
            regField = MANGO_ADMIT_ARPREQf;
            break;
        case IGR_BYPASS_RMA:
            regField = MANGO_ADMIT_RMAf;
            break;
        case IGR_BYPASS_BPDU:
            regField = MANGO_ADMIT_BPDUf;
            break;
        case IGR_BYPASS_RTKPKT:
            regField = MANGO_ADMIT_RTKPKTf;
            break;
        case IGR_BYPASS_IGMP:
            regField = MANGO_ADMIT_IGMPf;
            break;
        case IGR_BYPASS_DHCP:
            regField = MANGO_ADMIT_DHCPf;
            break;
        case IGR_BYPASS_RIP:
            regField = MANGO_ADMIT_RIPf;
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
    if ((ret = reg_field_write(unit, MANGO_IGBW_CTRLr, regField, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }
    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_rate_igrBandwidthCtrlBypass_set */

/* Function Name:
 *      dal_mango_rate_portIgrBwFlowctrlEnable_get
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
dal_mango_rate_portIgrBwFlowctrlEnable_get(
    uint32              unit,
    rtk_port_t          port,
    rtk_enable_t        *pEnable)
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
    if ((ret = reg_array_field_read(unit, MANGO_IGBW_PORT_FC_CTRLr, port, REG_ARRAY_INDEX_NONE, MANGO_ENf, &value)) != RT_ERR_OK)
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
 *      dal_mango_rate_portIgrBwFlowctrlEnable_set
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
dal_mango_rate_portIgrBwFlowctrlEnable_set(
    uint32              unit,
    rtk_port_t          port,
    rtk_enable_t        enable)
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
    if ((ret = reg_array_field_write(unit, MANGO_IGBW_PORT_FC_CTRLr, port, REG_ARRAY_INDEX_NONE, MANGO_ENf, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_rate_portIgrQueueBwCtrlEnable_get
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
dal_mango_rate_portIgrQueueBwCtrlEnable_get(
    uint32          unit,
    rtk_port_t      port,
    rtk_qid_t       queue,
    rtk_enable_t    *pEnable)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, queue=%d\n", unit, port, queue);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((queue >= HAL_MAX_NUM_OF_IGR_QUEUE(unit)), RT_ERR_QUEUE_ID);

    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_array_field_read(unit, MANGO_IGBW_PORT_Q_CTRLr, port, queue, MANGO_BW_ENf, &value)) != RT_ERR_OK)
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
} /* end of dal_mango_rate_portIgrQueueBwCtrlEnable_get */

/* Function Name:
 *      dal_mango_rate_portIgrQueueBwCtrlEnable_set
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
dal_mango_rate_portIgrQueueBwCtrlEnable_set(
    uint32          unit,
    rtk_port_t      port,
    rtk_qid_t       queue,
    rtk_enable_t    enable)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, queue=%d\n", unit, port, queue);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((queue >= HAL_MAX_NUM_OF_IGR_QUEUE(unit)), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    if(enable == ENABLED)
        value = 1;
    else
        value = 0;

    RATE_SEM_LOCK(unit);

    /* write value to CHIP*/
    if ((ret = reg_array_field_write(unit, MANGO_IGBW_PORT_Q_CTRLr, port, queue, MANGO_BW_ENf, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_mango_rate_portIgrQueueBwCtrlEnable_set */

/* Function Name:
 *      dal_mango_rate_portIgrQueueBwCtrlRate_get
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
dal_mango_rate_portIgrQueueBwCtrlRate_get(
    uint32      unit,
    rtk_port_t  port,
    rtk_qid_t   queue,
    uint32      *pRate)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d\n", unit, port);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((queue >= HAL_MAX_NUM_OF_IGR_QUEUE(unit)), RT_ERR_QUEUE_ID);

    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_array_field_read(unit, MANGO_IGBW_PORT_Q_CTRLr, port, queue, MANGO_RATEf, pRate)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_mango_rate_portIgrQueueBwCtrlRate_get */

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
dal_mango_rate_portIgrQueueBwCtrlRate_set(
    uint32      unit,
    rtk_port_t  port,
    rtk_qid_t   queue,
    uint32      rate)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d\n", unit, port);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((queue >= HAL_MAX_NUM_OF_IGR_QUEUE(unit)), RT_ERR_QUEUE_ID);

    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_array_field_write(unit, MANGO_IGBW_PORT_Q_CTRLr, port, queue, MANGO_RATEf, &rate)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_mango_rate_portIgrQueueBwCtrlRate_set */

/* Function Name:
 *      dal_mango_rate_igrQueueBwCtrlBurstSize_get
 * Description:
 *      Get burst size of queue ingress bandwidth
 * Input:
 *      unit        - unit id
 * Output:
 *      pBurst_size - pointer to burst size
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - Invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_mango_rate_igrQueueBwCtrlBurstSize_get(
    uint32                  unit,
    uint32                  *pBurst_size)
{
    int32   ret;
    rtk_port_t port;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d\n", unit);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pBurst_size), RT_ERR_NULL_POINTER);

    /* read value from CHIP*/
    HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
    {
        if ((ret = dal_mango_rate_portIgrQueueBwCtrlBurstSize_get(unit, port, 0, pBurst_size)) != RT_ERR_OK)
        {
            return ret;
        }
        break;
    }

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "pBurst_size=%d", *pBurst_size);

    return RT_ERR_OK;
} /* end of dal_mango_rate_igrQueueBwCtrlBurstSize_get */

/* Function Name:
 *      dal_mango_rate_igrQueueBwCtrlBurstSize_set
 * Description:
 *      Set burst size of queue ingress bandwidth
 * Input:
 *      unit        - unit id
 *      burst_size - burst size
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - Invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_RATE     - Invalid input rate
 * Note:
 *      None
 */
int32
dal_mango_rate_igrQueueBwCtrlBurstSize_set(
    uint32                  unit,
    uint32                  burst_size)
{
    int32   ret;
    rtk_port_t  port;
    rtk_qid_t queue, max_queue;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, burst_size=%d\n", unit, burst_size);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((burst_size > HAL_BURST_OF_IGR_BANDWIDTH_MAX(unit)), RT_ERR_INPUT);

    max_queue = HAL_MAX_NUM_OF_IGR_QUEUE(unit);

    /* set value to CHIP*/
    HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
    {
        for (queue = HAL_MIN_NUM_OF_IGR_QUEUE(unit); queue <= max_queue; queue++)
        {
            if ((ret = dal_mango_rate_portIgrQueueBwCtrlBurstSize_set(unit, port, queue-1, burst_size)) != RT_ERR_OK)
            {
                return ret;
            }
        }
    }

    return RT_ERR_OK;
} /* end of dal_mango_rate_igrQueueBwCtrlBurstSize_set */

/* Function Name:
 *      dal_mango_rate_portIgrQueueBwCtrlBurstSize_get
 * Description:
 *      Get the ingress queue bandwidth control burst.
 * Input:
 *      unit  - unit id
 *      port  - port id
 *      queue - queue id
 * Output:
 *      pBurst_size - ingress queue bandwidth control burst
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - Invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_QUEUE_ID     - invalid queue id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) The actual rate is "rate * chip granularity".
 *      (2) The unit of granularity is 16Kbps.
 */
int32
dal_mango_rate_portIgrQueueBwCtrlBurstSize_get(uint32 unit, rtk_port_t port, rtk_qid_t queue, uint32 *pBurst_size)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d\n", unit, port);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pBurst_size), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((queue >= HAL_MAX_NUM_OF_IGR_QUEUE(unit)), RT_ERR_QUEUE_ID);

    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_array_field_read(unit, MANGO_IGBW_PORT_Q_CTRLr, port, queue, MANGO_BURSTf, pBurst_size)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_mango_rate_portIgrQueueBwCtrlBurstSize_get */

/* Function Name:
 *      dal_mango_rate_portIgrQueueBwCtrlBurstSize_set
 * Description:
 *      Set the ingress queue bandwidth control burst.
 * Input:
 *      unit        - unit id
 *      port        - port id
 *      queue       - queue id
 *      burst_size  - ingress queue bandwidth control burst
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
 */
int32
dal_mango_rate_portIgrQueueBwCtrlBurstSize_set(uint32 unit, rtk_port_t port, rtk_qid_t queue, uint32 burst_size)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d\n", unit, port);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((queue >= HAL_MAX_NUM_OF_IGR_QUEUE(unit)), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((burst_size > HAL_BURST_OF_IGR_BANDWIDTH_MAX(unit)), RT_ERR_INPUT);

    RATE_SEM_LOCK(unit);

    /* write value to CHIP*/
    if ((ret = reg_array_field_write(unit, MANGO_IGBW_PORT_Q_CTRLr, port, queue, MANGO_BURSTf, &burst_size)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_mango_rate_portIgrQueueBwCtrlBurstSize_set */

/* Function Name:
 *      dal_mango_rate_portIgrQueueBwCtrlExceed_get
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
 *      RT_ERR_QUEUE_ID          - invalid queue id
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 * Note:
 *      Exceed status is as following
 *      - TRUE      - ingress queue bandwidth rate is more than configured rate.
 *      - FALSE     - ingress queue bandwidth rate is never over then configured rate.
 */
int32
dal_mango_rate_portIgrQueueBwCtrlExceed_get(
    uint32                  unit,
    rtk_port_t              port,
    rtk_qid_t               queue,
    uint32                  *pIsExceed)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d\n", unit, port);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((queue >= HAL_MAX_NUM_OF_IGR_QUEUE(unit)), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((NULL == pIsExceed), RT_ERR_NULL_POINTER);

    RATE_SEM_LOCK(unit);

    /* write value to CHIP*/
    if ((ret = reg_array_field_read(unit, MANGO_IGBW_PORT_Q_LB_EXCEED_FLAGr, port, queue, MANGO_FLAGf, pIsExceed)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "pIsExceed=%d", *pIsExceed);

    return RT_ERR_OK;
} /* end of dal_mango_rate_portIgrQueueBwCtrlExceed_get */

/* Function Name:
 *      dal_mango_rate_portIgrQueueBwCtrlExceed_reset
 * Description:
 *      Reset exceed status of ingress bandwidth on specified queue.
 * Input:
 *      unit       - unit id
 *      port       - port id
 *      queue      - queue id
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
dal_mango_rate_portIgrQueueBwCtrlExceed_reset(
    uint32                  unit,
    rtk_port_t              port,
    rtk_qid_t               queue)
{
    int32  ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d\n", unit, port);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((queue >= HAL_MAX_NUM_OF_IGR_QUEUE(unit)), RT_ERR_QUEUE_ID);

    RATE_SEM_LOCK(unit);

    /*write 1 to clear*/
    if ((ret = reg_array_field_write1toClear(unit, MANGO_IGBW_PORT_Q_LB_EXCEED_FLAGr, port, queue, MANGO_FLAGf)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_mango_rate_portIgrQueueBwCtrlExceed_reset */

/* Module Name    : Rate                                           */
/* Sub-module Name: Configuration of egress port bandwidth control */

/* Function Name:
 *      dal_mango_rate_portEgrBwCtrlEnable_get
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
dal_mango_rate_portEgrBwCtrlEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
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
    if ((ret = reg_array_field_read(unit, MANGO_EGBW_PORT_CTRLr, port, REG_ARRAY_INDEX_NONE, MANGO_ENf, &value)) != RT_ERR_OK)
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
 *      dal_mango_rate_portEgrBwCtrlEnable_set
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
dal_mango_rate_portEgrBwCtrlEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
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
    if ((ret = reg_array_field_write(unit, MANGO_EGBW_PORT_CTRLr, port, REG_ARRAY_INDEX_NONE, MANGO_ENf, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_rate_portEgrBwCtrlRate_get
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
 *      (2) The unit of granularity is 16Kbps.
 */
int32
dal_mango_rate_portEgrBwCtrlRate_get(uint32 unit, rtk_port_t port, uint32 *pRate)
{
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d\n", unit, port);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pRate), RT_ERR_NULL_POINTER);

    /* [SS-972]-START */
    *pRate = *pEgrPort_rate[unit][port];
    /* [SS-972]-END */

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_rate_portEgrBwCtrlRate_set
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
 *      (3) Valid valud of rate is 0x1~0xFFFF.
 */
int32
dal_mango_rate_portEgrBwCtrlRate_set(uint32 unit, rtk_port_t port, uint32 rate)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d\n", unit, port);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((rate > HAL_RATE_OF_BANDWIDTH_MAX(unit)), RT_ERR_RATE);
    RT_PARAM_CHK((rate < HAL_RATE_OF_EGR_BANDWIDTH_MIN(unit)), RT_ERR_RATE);

    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    /* [SS-972]-START */
    *pEgrPort_rate[unit][port] = rate;
    if ((ret = _dal_mango_rate_portEgrBwCtrlRate_refill(unit, port)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }
    /* [SS-972]-END */

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_rate_portEgrBwCtrlBurstSize_get
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
dal_mango_rate_portEgrBwCtrlBurstSize_get(uint32 unit, rtk_port_t port, uint32 *pBurst_size)
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
    if ((ret = reg_array_field_read(unit, MANGO_EGBW_PORT_CTRLr, port, REG_ARRAY_INDEX_NONE, MANGO_BURSTf, pBurst_size)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_rate_portEgrBwCtrlBurstSize_set
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
dal_mango_rate_portEgrBwCtrlBurstSize_set(uint32 unit, rtk_port_t port, uint32 burst_size)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d\n", unit, port);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((burst_size > HAL_BURST_OF_BANDWIDTH_MAX(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK(_dal_mango_rate_portBurstSize_check(unit, port, RATE_MODULE_EGR, burst_size), RT_ERR_INPUT);

    RATE_SEM_LOCK(unit);

    /* write value to CHIP*/
    if ((ret = reg_array_field_write(unit, MANGO_EGBW_PORT_CTRLr, port, REG_ARRAY_INDEX_NONE, MANGO_BURSTf, &burst_size)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_rate_cpuEgrBandwidthCtrlRateMode_get
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
dal_mango_rate_cpuEgrBandwidthCtrlRateMode_get(uint32 unit, rtk_rate_rateMode_t *pRate_mode)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d\n", unit);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pRate_mode), RT_ERR_NULL_POINTER);

    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_field_read(unit, MANGO_EGBW_CTRLr, MANGO_RATE_MODE_CPUf, &value)) != RT_ERR_OK)
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
 *      dal_mango_rate_cpuEgrBandwidthCtrlRateMode_set
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
dal_mango_rate_cpuEgrBandwidthCtrlRateMode_set(uint32 unit, rtk_rate_rateMode_t rate_mode)
{
    int32   ret;
    uint32  value, burst_size;
    rtk_port_t  port;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, rate_mode=%d\n", unit, rate_mode);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((rate_mode >= RATE_MODE_END), RT_ERR_INPUT);

    if(rate_mode == RATE_MODE_BYTE)
    {
        burst_size = MANGO_EGRBW_CPU_BPS_DFLT_BURST_SIZE;
        value = 1;
    }
    else
    {
        burst_size = MANGO_EGRBW_CPU_PPS_DFLT_BURST_SIZE;
        value = 0;
    }

    RATE_SEM_LOCK(unit);

    /* write value to CHIP*/
    if ((ret = reg_field_write(unit, MANGO_EGBW_CTRLr, MANGO_RATE_MODE_CPUf, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }
    port = HWP_CPU_MACID(unit);
    if ((ret = reg_array_field_write(unit, MANGO_EGBW_PORT_CTRLr, port, REG_ARRAY_INDEX_NONE, MANGO_BURSTf, &burst_size)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_rate_egrPortBwCtrlBurstSize_get
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
dal_mango_rate_egrPortBwCtrlBurstSize_get(
    uint32                  unit,
    uint32                  *pBurst_size)
{
    rtk_port_t      port;

    RT_PARAM_CHK((NULL == pBurst_size), RT_ERR_NULL_POINTER);

    HWP_PORT_TRAVS(unit, port)
    {
        return dal_mango_rate_portEgrBwCtrlBurstSize_get(unit, port, pBurst_size);
    }
    return RT_ERR_FAILED;
}

/* Function Name:
 *      dal_mango_rate_egrPortBwCtrlBurstSize_set
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
dal_mango_rate_egrPortBwCtrlBurstSize_set(
    uint32                  unit,
    uint32                  burst_size)
{
    int32 ret;
    rtk_port_t port;

    HWP_PORT_TRAVS(unit, port)
    {
        if(RT_ERR_OK != (ret = dal_mango_rate_portEgrBwCtrlBurstSize_set(unit, port, burst_size)))
            return ret;
    }
    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_rate_portEgrQueueBwCtrlEnable_get
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
dal_mango_rate_portEgrQueueBwCtrlEnable_get(
    uint32          unit,
    rtk_port_t      port,
    rtk_qid_t       queue,
    rtk_enable_t    *pEnable)
{
    int32   ret;
    uint32  value;
    egr_qBw_entry_t egrQEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, queue=%d\n", unit, port, queue);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    if (HWP_IS_CPU_PORT(unit, port))
        RT_PARAM_CHK(queue >= HAL_MAX_NUM_OF_CPU_QUEUE(unit), RT_ERR_QUEUE_ID);
    else if ((HWP_UPLINK_PORT(unit, port)) && (HAL_STACK_PORT(unit, port)))
        RT_PARAM_CHK(queue >= HAL_MAX_NUM_OF_STACK_QUEUE(unit), RT_ERR_QUEUE_ID);
    else
        RT_PARAM_CHK((queue >= HAL_MAX_NUM_OF_QUEUE(unit)), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    RATE_SEM_LOCK(unit);

    /* get value from CHIP */
    if (HWP_IS_CPU_PORT(unit,port))
    {
        if ((ret = reg_array_field_read(unit, MANGO_EGBW_CPU_Q_MAX_LB_CTRLr,
                                REG_ARRAY_INDEX_NONE, queue, MANGO_ENf, &value)) != RT_ERR_OK)
        {
            RATE_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }
    }
    else
    {
        osal_memset(&egrQEntry, 0, sizeof(egr_qBw_entry_t));

        if ((ret = table_read(unit, MANGO_EGR_Q_BWt, port, (uint32 *) &egrQEntry)) != RT_ERR_OK)
        {
            RATE_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }

        if ((ret = table_field_get(unit, MANGO_EGR_Q_BWt, (uint32)egrQBwEn_fieldidx[queue],
                        &value, (uint32 *) &egrQEntry)) != RT_ERR_OK)
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
 *      dal_mango_rate_portEgrQueueBwCtrlEnable_set
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
dal_mango_rate_portEgrQueueBwCtrlEnable_set(
    uint32          unit,
    rtk_port_t      port,
    rtk_qid_t       queue,
    rtk_enable_t    enable)
{
    int32   ret;
    uint32  value;
    egr_qBw_entry_t egrQEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, queue=%d\n", unit, port, queue);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    if (HWP_IS_CPU_PORT(unit, port))
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

    /* set value to CHIP */
    if (HWP_IS_CPU_PORT(unit,port))
    {
        if ((ret = reg_array_field_write(unit, MANGO_EGBW_CPU_Q_MAX_LB_CTRLr,
                                REG_ARRAY_INDEX_NONE, queue, MANGO_ENf, &value)) != RT_ERR_OK)
        {
            RATE_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }
    }
    else
    {
        osal_memset(&egrQEntry, 0, sizeof(egr_qBw_entry_t));

        if ((ret = table_read(unit, MANGO_EGR_Q_BWt, port, (uint32 *) &egrQEntry)) != RT_ERR_OK)
        {
            RATE_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }

        if ((ret = table_field_set(unit, MANGO_EGR_Q_BWt, (uint32)egrQBwEn_fieldidx[queue],
                        &value, (uint32 *) &egrQEntry)) != RT_ERR_OK)
        {
            RATE_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }
        if ((ret = table_write(unit, MANGO_EGR_Q_BWt, port, (uint32 *) &egrQEntry)) != RT_ERR_OK)
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
 *      dal_mango_rate_portEgrQueueBwCtrlRate_get
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
dal_mango_rate_portEgrQueueBwCtrlRate_get(uint32 unit, rtk_port_t port, rtk_qid_t queue, uint32 *pRate)
{
    int32   ret;
    egr_qBw_entry_t egrQEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, queue=%d\n", unit, port, queue);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    if (HWP_IS_CPU_PORT(unit, port))
        RT_PARAM_CHK(queue >= HAL_MAX_NUM_OF_CPU_QUEUE(unit), RT_ERR_QUEUE_ID);
    else if ((HWP_UPLINK_PORT(unit, port)) && (HAL_STACK_PORT(unit, port)))
        RT_PARAM_CHK(queue >= HAL_MAX_NUM_OF_STACK_QUEUE(unit), RT_ERR_QUEUE_ID);
    else
        RT_PARAM_CHK((queue >= HAL_MAX_NUM_OF_QUEUE(unit)), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((NULL == pRate), RT_ERR_NULL_POINTER);

    RATE_SEM_LOCK(unit);

    /* get value from CHIP */
    if (HWP_IS_CPU_PORT(unit,port))
    {
        if ((ret = reg_array_field_read(unit, MANGO_EGBW_CPU_Q_MAX_LB_CTRLr,
                                REG_ARRAY_INDEX_NONE, queue, MANGO_RATEf, pRate)) != RT_ERR_OK)
        {
            RATE_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }
    }
    else
    {
        osal_memset(&egrQEntry, 0, sizeof(egr_qBw_entry_t));

        if ((ret = table_read(unit, MANGO_EGR_Q_BWt, port, (uint32 *) &egrQEntry)) != RT_ERR_OK)
        {
            RATE_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }

        if ((ret = table_field_get(unit, MANGO_EGR_Q_BWt, (uint32)egrQBwRate_fieldidx[queue],
                        pRate, (uint32 *) &egrQEntry)) != RT_ERR_OK)
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
 *      dal_mango_rate_portEgrQueueBwCtrlRate_set
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
 *      (3) Valid valud of rate is 0x1~0xFFFF.
 *      (4) In stacking uplink port, the number of queue is 12 (queue 0~11). Before configuring the queue 8~11, please
 *          enable stacking ability of the uplink port by API dal_mango_stack_port_set.
 */
int32
dal_mango_rate_portEgrQueueBwCtrlRate_set(uint32 unit, rtk_port_t port, rtk_qid_t queue, uint32 rate)
{
    int32   ret;
    egr_qBw_entry_t egrQEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, queue=%d\n", unit, port, queue);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    if (HWP_IS_CPU_PORT(unit, port))
        RT_PARAM_CHK(queue >= HAL_MAX_NUM_OF_CPU_QUEUE(unit), RT_ERR_QUEUE_ID);
    else if ((HWP_UPLINK_PORT(unit, port)) && (HAL_STACK_PORT(unit, port)))
        RT_PARAM_CHK(queue >= HAL_MAX_NUM_OF_STACK_QUEUE(unit), RT_ERR_QUEUE_ID);
    else
        RT_PARAM_CHK((queue >= HAL_MAX_NUM_OF_QUEUE(unit)), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((rate > HAL_RATE_OF_BANDWIDTH_MAX(unit)), RT_ERR_RATE);
    if (!HWP_IS_CPU_PORT(unit, port))
        RT_PARAM_CHK((rate < HAL_RATE_OF_EGR_BANDWIDTH_MIN(unit)), RT_ERR_RATE);

    RATE_SEM_LOCK(unit);

    /* get value from CHIP */
    if (HWP_IS_CPU_PORT(unit,port))
    {
        if ((ret = reg_array_field_write(unit, MANGO_EGBW_CPU_Q_MAX_LB_CTRLr,
                                REG_ARRAY_INDEX_NONE, queue, MANGO_RATEf, &rate)) != RT_ERR_OK)
        {
            RATE_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }
    }
    else
    {
        osal_memset(&egrQEntry, 0, sizeof(egr_qBw_entry_t));

        if ((ret = table_read(unit, MANGO_EGR_Q_BWt, port, (uint32 *) &egrQEntry)) != RT_ERR_OK)
        {
            RATE_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }
        if ((ret = table_field_set(unit, MANGO_EGR_Q_BWt, (uint32)egrQBwRate_fieldidx[queue],
                        &rate, (uint32 *) &egrQEntry)) != RT_ERR_OK)
        {
            RATE_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }
        if ((ret = table_write(unit, MANGO_EGR_Q_BWt, port, (uint32 *) &egrQEntry)) != RT_ERR_OK)
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
 *      dal_mango_rate_portEgrQueueBwCtrlBurstSize_get
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
dal_mango_rate_portEgrQueueBwCtrlBurstSize_get(uint32 unit, rtk_port_t port, rtk_qid_t queue, uint32 *pBurst_size)
{
    int32   ret;
    egr_qBw_entry_t egrQEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, queue=%d\n", unit, port, queue);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    if (HWP_IS_CPU_PORT(unit, port))
        RT_PARAM_CHK(queue >= HAL_MAX_NUM_OF_CPU_QUEUE(unit), RT_ERR_QUEUE_ID);
    else if ((HWP_UPLINK_PORT(unit, port)) && (HAL_STACK_PORT(unit, port)))
        RT_PARAM_CHK(queue >= HAL_MAX_NUM_OF_STACK_QUEUE(unit), RT_ERR_QUEUE_ID);
    else
        RT_PARAM_CHK((queue >= HAL_MAX_NUM_OF_QUEUE(unit)), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((NULL == pBurst_size), RT_ERR_NULL_POINTER);

    RATE_SEM_LOCK(unit);

    /* get value from CHIP */
    if (HWP_IS_CPU_PORT(unit,port))
    {
        if ((ret = reg_array_field_read(unit, MANGO_EGBW_CPU_Q_MAX_LB_CTRLr,
                                REG_ARRAY_INDEX_NONE, queue, MANGO_BURSTf, pBurst_size)) != RT_ERR_OK)
        {
            RATE_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }
    }
    else
    {
        osal_memset(&egrQEntry, 0, sizeof(egr_qBw_entry_t));

        if ((ret = table_read(unit, MANGO_EGR_Q_BWt, port, (uint32 *) &egrQEntry)) != RT_ERR_OK)
        {
            RATE_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }

        if ((ret = table_field_get(unit, MANGO_EGR_Q_BWt, (uint32)egrQBwBurst_fieldidx[queue],
                        pBurst_size, (uint32 *) &egrQEntry)) != RT_ERR_OK)
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
 *      dal_mango_rate_portEgrQueueBwCtrlBurstSize_set
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
dal_mango_rate_portEgrQueueBwCtrlBurstSize_set(uint32 unit, rtk_port_t port, rtk_qid_t queue, uint32 burst_size)
{
    int32   ret;
    egr_qBw_entry_t egrQEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, queue=%d\n", unit, port, queue);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    if (HWP_IS_CPU_PORT(unit, port))
        RT_PARAM_CHK(queue >= HAL_MAX_NUM_OF_CPU_QUEUE(unit), RT_ERR_QUEUE_ID);
    else if ((HWP_UPLINK_PORT(unit, port)) && (HAL_STACK_PORT(unit, port)))
        RT_PARAM_CHK(queue >= HAL_MAX_NUM_OF_STACK_QUEUE(unit), RT_ERR_QUEUE_ID);
    else
        RT_PARAM_CHK((queue >= HAL_MAX_NUM_OF_QUEUE(unit)), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((burst_size > HAL_BURST_OF_BANDWIDTH_MAX(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK(_dal_mango_rate_portBurstSize_check(unit, port, RATE_MODULE_EGR, burst_size), RT_ERR_INPUT);

    RATE_SEM_LOCK(unit);

    /* get value from CHIP */
    if (HWP_IS_CPU_PORT(unit,port))
    {
        if ((ret = reg_array_field_write(unit, MANGO_EGBW_CPU_Q_MAX_LB_CTRLr,
                                REG_ARRAY_INDEX_NONE, queue, MANGO_BURSTf, &burst_size)) != RT_ERR_OK)
        {
            RATE_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }
    }
    else
    {
        osal_memset(&egrQEntry, 0, sizeof(egr_qBw_entry_t));

        if ((ret = table_read(unit, MANGO_EGR_Q_BWt, port, (uint32 *) &egrQEntry)) != RT_ERR_OK)
        {
            RATE_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }
        if ((ret = table_field_set(unit, MANGO_EGR_Q_BWt, (uint32)egrQBwBurst_fieldidx[queue],
                        &burst_size, (uint32 *) &egrQEntry)) != RT_ERR_OK)
        {
            RATE_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }
        if ((ret = table_write(unit, MANGO_EGR_Q_BWt, port, (uint32 *) &egrQEntry)) != RT_ERR_OK)
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
 *      dal_mango_rate_portEgrQueueAssuredBwCtrlEnable_get
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
dal_mango_rate_portEgrQueueAssuredBwCtrlEnable_get(
    uint32          unit,
    rtk_port_t      port,
    rtk_qid_t       queue,
    rtk_enable_t    *pEnable)
{
    int32   ret;
    uint32  value;
    egr_qBw_entry_t egrQEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, queue=%d\n", unit, port, queue);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);
    if ((HWP_UPLINK_PORT(unit, port)) && (HAL_STACK_PORT(unit, port)))
        RT_PARAM_CHK(queue >= HAL_MAX_NUM_OF_STACK_QUEUE(unit), RT_ERR_QUEUE_ID);
    else
        RT_PARAM_CHK((queue >= HAL_MAX_NUM_OF_QUEUE(unit)), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    osal_memset(&egrQEntry, 0, sizeof(egr_qBw_entry_t));

    RATE_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = table_read(unit, MANGO_EGR_Q_BWt, port, (uint32 *) &egrQEntry)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }
    if ((ret = table_field_get(unit, MANGO_EGR_Q_BWt, (uint32)egrAssureQBwEn_fieldidx[queue],
                    &value, (uint32 *) &egrQEntry)) != RT_ERR_OK)
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
 *      dal_mango_rate_portEgrQueueAssuredBwCtrlEnable_set
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
dal_mango_rate_portEgrQueueAssuredBwCtrlEnable_set(
    uint32          unit,
    rtk_port_t      port,
    rtk_qid_t       queue,
    rtk_enable_t    enable)
{
    int32   ret;
    uint32  value;
    egr_qBw_entry_t egrQEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, queue=%d\n", unit, port, queue);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);
    if ((HWP_UPLINK_PORT(unit, port)) && (HAL_STACK_PORT(unit, port)))
        RT_PARAM_CHK(queue >= HAL_MAX_NUM_OF_STACK_QUEUE(unit), RT_ERR_QUEUE_ID);
    else
        RT_PARAM_CHK((queue >= HAL_MAX_NUM_OF_QUEUE(unit)), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    if (enable == ENABLED)
        value = 1;
    else
        value = 0;

    RATE_SEM_LOCK(unit);

    /* set value to CHIP */
    osal_memset(&egrQEntry, 0, sizeof(egr_qBw_entry_t));

    if ((ret = table_read(unit, MANGO_EGR_Q_BWt, port, (uint32 *) &egrQEntry)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }
    if ((ret = table_field_set(unit, MANGO_EGR_Q_BWt, (uint32)egrAssureQBwEn_fieldidx[queue],
                    &value, (uint32 *) &egrQEntry)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }
    if ((ret = table_write(unit, MANGO_EGR_Q_BWt, port, (uint32 *) &egrQEntry)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    /* [SS-972]-START */
    if (enable == ENABLED)
        BITMAP_SET(pAssureQEn[unit][port], queue);
    else
        BITMAP_CLEAR(pAssureQEn[unit][port], queue);

    RT_LOG(LOG_INFO, (MOD_DAL|MOD_RATE), "unit=%d, port=%2d, pAssureQEn[] = 0x%X", unit, port, *pAssureQEn[unit][port]);

    if ((ret = _dal_mango_rate_portEgrBwCtrlRate_refill(unit, port)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }
    /* [SS-972]-END */

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_rate_portEgrQueueAssuredBwCtrlRate_get
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
dal_mango_rate_portEgrQueueAssuredBwCtrlRate_get(uint32 unit, rtk_port_t port, rtk_qid_t queue, uint32 *pRate)
{
/*    int32   ret; */
/*    egr_qBw_entry_t egrQEntry; */

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, queue=%d\n", unit, port, queue);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);
    if ((HWP_UPLINK_PORT(unit, port)) && (HAL_STACK_PORT(unit, port)))
        RT_PARAM_CHK(queue >= HAL_MAX_NUM_OF_STACK_QUEUE(unit), RT_ERR_QUEUE_ID);
    else
        RT_PARAM_CHK((queue >= HAL_MAX_NUM_OF_QUEUE(unit)), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((NULL == pRate), RT_ERR_NULL_POINTER);

    RATE_SEM_LOCK(unit);

    /* [MANGO-3416]-START */
    *pRate = *pAssure_rate[unit][port][queue];
    /* [MANGO-3416]-END */

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_rate_portEgrQueueAssuredBwCtrlRate_set
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
 *      (3) Valid valud of rate is 0x1~0xFFFF.
 *      (4) In stacking uplink port, the number of queue is 12 (queue 0~11). Before configuring the queue 8~11, please
 *          enable stacking ability of the uplink port by API dal_mango_stack_port_set.
 */
int32
dal_mango_rate_portEgrQueueAssuredBwCtrlRate_set(uint32 unit, rtk_port_t port, rtk_qid_t queue, uint32 rate)
{
    int32   ret;
    egr_qBw_entry_t egrQEntry;
    rtk_rate_assuredMode_t mode;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, queue=%d\n", unit, port, queue);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);
    if ((HWP_UPLINK_PORT(unit, port)) && (HAL_STACK_PORT(unit, port)))
        RT_PARAM_CHK(queue >= HAL_MAX_NUM_OF_STACK_QUEUE(unit), RT_ERR_QUEUE_ID);
    else
        RT_PARAM_CHK((queue >= HAL_MAX_NUM_OF_QUEUE(unit)), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((rate > HAL_RATE_OF_BANDWIDTH_MAX(unit)), RT_ERR_RATE);
    RT_PARAM_CHK((rate < HAL_RATE_OF_EGR_BANDWIDTH_MIN(unit)), RT_ERR_RATE);

    osal_memset(&egrQEntry, 0, sizeof(egr_qBw_entry_t));

    /* [MANGO-3416]-START */
    *pAssure_rate[unit][port][queue] = rate;
    if ((ret = dal_mango_rate_portEgrQueueAssuredBwCtrlMode_get(unit, port, queue, &mode)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    if ((mode == ASSURED_MODE_SHARE) && (rate < (HAL_RATE_OF_BANDWIDTH_MAX(unit))))
    {
        rate++;
    }
    /* [MANGO-3416]-END */

    RATE_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = table_read(unit, MANGO_EGR_Q_BWt, port, (uint32 *) &egrQEntry)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    if ((ret = table_field_set(unit, MANGO_EGR_Q_BWt, (uint32)egrAssureQBwRate_fieldidx[queue],
                    &rate, (uint32 *) &egrQEntry)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    if ((ret = table_write(unit, MANGO_EGR_Q_BWt, port, (uint32 *) &egrQEntry)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    /* [SS-972]-START */
    if ((ret = _dal_mango_rate_portEgrBwCtrlRate_refill(unit, port)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }
    /* [SS-972]-END */

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
dal_mango_rate_portEgrQueueAssuredBwCtrlBurstSize_get(uint32 unit, rtk_port_t port, rtk_qid_t queue, uint32 *pBurst_size)
{
    int32   ret;
    egr_qBw_entry_t egrQEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, queue=%d\n", unit, port, queue);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);
    if ((HWP_UPLINK_PORT(unit, port)) && (HAL_STACK_PORT(unit, port)))
        RT_PARAM_CHK(queue >= HAL_MAX_NUM_OF_STACK_QUEUE(unit), RT_ERR_QUEUE_ID);
    else
        RT_PARAM_CHK((queue >= HAL_MAX_NUM_OF_QUEUE(unit)), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((NULL == pBurst_size), RT_ERR_NULL_POINTER);

    RATE_SEM_LOCK(unit);

    /* get value from CHIP */
    osal_memset(&egrQEntry, 0, sizeof(egr_qBw_entry_t));

    if ((ret = table_read(unit, MANGO_EGR_Q_BWt, port, (uint32 *) &egrQEntry)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    if ((ret = table_field_get(unit, MANGO_EGR_Q_BWt, (uint32)egrAssureQBwBurst_fieldidx[queue],
                    pBurst_size, (uint32 *) &egrQEntry)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_rate_portEgrQueueAssuredBwCtrlBurstSize_set
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
dal_mango_rate_portEgrQueueAssuredBwCtrlBurstSize_set(uint32 unit, rtk_port_t port, rtk_qid_t queue, uint32 burst_size)
{
    int32   ret;
    egr_qBw_entry_t egrQEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, queue=%d\n", unit, port, queue);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);
    if ((HWP_UPLINK_PORT(unit, port)) && (HAL_STACK_PORT(unit, port)))
        RT_PARAM_CHK(queue >= HAL_MAX_NUM_OF_STACK_QUEUE(unit), RT_ERR_QUEUE_ID);
    else
        RT_PARAM_CHK((queue >= HAL_MAX_NUM_OF_QUEUE(unit)), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((burst_size > HAL_BURST_OF_BANDWIDTH_MAX(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK(_dal_mango_rate_portBurstSize_check(unit, port, RATE_MODULE_EGR, burst_size), RT_ERR_INPUT);

    osal_memset(&egrQEntry, 0, sizeof(egr_qBw_entry_t));

    RATE_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = table_read(unit, MANGO_EGR_Q_BWt, port, (uint32 *) &egrQEntry)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    if ((ret = table_field_set(unit, MANGO_EGR_Q_BWt, (uint32)egrAssureQBwBurst_fieldidx[queue],
                    &burst_size, (uint32 *) &egrQEntry)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    if ((ret = table_write(unit, MANGO_EGR_Q_BWt, port, (uint32 *) &egrQEntry)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_rate_portEgrQueueAssuredBwCtrlMode_get
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
 *      In stacking uplink port, the number of queue is 12 (queue 0~11). Before configuring the queue 8~11, please
 *      enable stacking ability of the uplink port by API dal_mango_stack_port_set.
 */
int32
dal_mango_rate_portEgrQueueAssuredBwCtrlMode_get(
    uint32          unit,
    rtk_port_t      port,
    rtk_qid_t       queue,
    rtk_rate_assuredMode_t    *pMode)
{
    int32   ret;
    uint32  value = 0;
    egr_qBw_entry_t egrQEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, queue=%d\n", unit, port, queue);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);
    if ((HWP_UPLINK_PORT(unit, port)) && (HAL_STACK_PORT(unit, port)))
        RT_PARAM_CHK(queue >= HAL_MAX_NUM_OF_STACK_QUEUE(unit), RT_ERR_QUEUE_ID);
    else
        RT_PARAM_CHK((queue >= HAL_MAX_NUM_OF_QUEUE(unit)), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((NULL == pMode), RT_ERR_NULL_POINTER);

    osal_memset(&egrQEntry, 0, sizeof(egr_qBw_entry_t));

    RATE_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = table_read(unit, MANGO_EGR_Q_BWt, port, (uint32 *) &egrQEntry)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    if ((ret = table_field_get(unit, MANGO_EGR_Q_BWt, (uint32)egrAssureQBwMode_fieldidx[queue],
                    &value, (uint32 *) &egrQEntry)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    if (value == 0)
        *pMode = ASSURED_MODE_SHARE;
    else
        *pMode = ASSURED_MODE_FIX;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "pMode=%d", *pMode);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_rate_portEgrQueueAssuredBwCtrlMode_set
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
dal_mango_rate_portEgrQueueAssuredBwCtrlMode_set(
    uint32          unit,
    rtk_port_t      port,
    rtk_qid_t       queue,
    rtk_rate_assuredMode_t    mode)
{
    int32   ret;
    uint32  value, rate;
    egr_qBw_entry_t egrQEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, queue=%d\n", unit, port, queue);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);
    if ((HWP_UPLINK_PORT(unit, port)) && (HAL_STACK_PORT(unit, port)))
        RT_PARAM_CHK(queue >= HAL_MAX_NUM_OF_STACK_QUEUE(unit), RT_ERR_QUEUE_ID);
    else
        RT_PARAM_CHK((queue >= HAL_MAX_NUM_OF_QUEUE(unit)), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((mode >= ASSURED_MODE_END), RT_ERR_INPUT);

    if (mode == ASSURED_MODE_SHARE)
        value = 0;
    else
        value = 1;

    RATE_SEM_LOCK(unit);

    /* set value to CHIP */
    osal_memset(&egrQEntry, 0, sizeof(egr_qBw_entry_t));

    if ((ret = table_read(unit, MANGO_EGR_Q_BWt, port, (uint32 *) &egrQEntry)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    if ((ret = table_field_set(unit, MANGO_EGR_Q_BWt, (uint32)egrAssureQBwMode_fieldidx[queue],
                    &value, (uint32 *) &egrQEntry)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    /* [MANGO-3416]-START */
    if (mode == ASSURED_MODE_SHARE)
        rate = *pAssure_rate[unit][port][queue] + 1;
    else
        rate = *pAssure_rate[unit][port][queue];

    if ((ret = table_field_set(unit, MANGO_EGR_Q_BWt, (uint32)egrAssureQBwRate_fieldidx[queue],
                    &rate, (uint32 *) &egrQEntry)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }
    /* [MANGO-3416]-END */

    if ((ret = table_write(unit, MANGO_EGR_Q_BWt, port, (uint32 *) &egrQEntry)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    /* [SS-972]-START */
    if (mode == ASSURED_MODE_SHARE)
        BITMAP_SET(pAssureQShare[unit][port], queue);
    else
        BITMAP_CLEAR(pAssureQShare[unit][port], queue);
    RT_LOG(LOG_INFO, (MOD_DAL|MOD_RATE), "unit=%d, port=%2d, pAssureQShare[] = 0x%X", unit, port, *pAssureQShare[unit][port]);

    if ((ret = _dal_mango_rate_portEgrBwCtrlRate_refill(unit, port)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }
    /* [SS-972]-END */

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_mango_rate_egrQueueBwCtrlBurstSize_get
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
dal_mango_rate_egrQueueBwCtrlBurstSize_get(
    uint32                  unit,
    uint32                  *pBurst_size)
{
    rtk_port_t  port;

    RT_PARAM_CHK((NULL == pBurst_size), RT_ERR_NULL_POINTER);

    HWP_PORT_TRAVS(unit, port)
    {
        return dal_mango_rate_portEgrQueueBwCtrlBurstSize_get(unit, port, 0, pBurst_size);
    }

    return RT_ERR_FAILED;
}

/* Function Name:
 *      dal_mango_rate_egrQueueBwCtrlBurstSize_set
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
dal_mango_rate_egrQueueBwCtrlBurstSize_set(
    uint32                  unit,
    uint32                  burst_size)
{
    int32 ret = RT_ERR_FAILED;
    rtk_port_t port;
    rtk_qid_t queue, max_queue;

    HWP_PORT_TRAVS(unit, port)
    {
        if (HWP_IS_CPU_PORT(unit, port))
            max_queue = HAL_MAX_NUM_OF_CPU_QUEUE(unit);
        else if ((HWP_UPLINK_PORT(unit, port)) && (HAL_STACK_PORT(unit, port)))
            max_queue = HAL_MAX_NUM_OF_STACK_QUEUE(unit);
        else
            max_queue = HAL_MAX_NUM_OF_QUEUE(unit);

        for (queue = 0; queue < max_queue; queue++)
        {
            if ((ret = dal_mango_rate_portEgrQueueBwCtrlBurstSize_set(unit, port, queue, burst_size)) != RT_ERR_OK)
                return ret;
        }
    }

    return RT_ERR_OK;
}

/* Module Name    : Rate                           */
/* Sub-module Name: Configuration of storm control */

/* Function Name:
 *      dal_mango_rate_portStormCtrlEnable_get
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
 *      (1) The storm group types are as following:
 *          - STORM_GROUP_MULTICAST
 *          - STORM_GROUP_BROADCAST
 *          - STORM_GROUP_UNICAST
 */
int32
dal_mango_rate_portStormCtrlEnable_get(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_group_t  storm_type,
    rtk_enable_t            *pEnable)
{
    int32   ret;
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
            reg_idx = MANGO_STORM_PORT_UC_CTRLr;
            break;
        case STORM_GROUP_MULTICAST:
            reg_idx = MANGO_STORM_PORT_MC_CTRLr;
            break;
        case STORM_GROUP_BROADCAST:
            reg_idx = MANGO_STORM_PORT_BC_CTRLr;
            break;
        default:
            return RT_ERR_INPUT;
    }

    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_array_field_read(unit, reg_idx, port, REG_ARRAY_INDEX_NONE, MANGO_ENf, &value)) != RT_ERR_OK)
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
 *      dal_mango_rate_portStormCtrlEnable_set
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
 *      (1) The storm group types are as following:
 *          - STORM_GROUP_MULTICAST
 *          - STORM_GROUP_BROADCAST
 *          - STORM_GROUP_UNICAST
 */
int32
dal_mango_rate_portStormCtrlEnable_set(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_group_t  storm_type,
    rtk_enable_t            enable)
{
    int32   ret;
    uint32  reg_idx;
    uint32  rst_idx;
    uint32  value;

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
            reg_idx = MANGO_STORM_PORT_UC_CTRLr;
            rst_idx = MANGO_STORM_PORT_UC_LB_RSTr;
            break;
        case STORM_GROUP_MULTICAST:
            reg_idx = MANGO_STORM_PORT_MC_CTRLr;
            rst_idx = MANGO_STORM_PORT_MC_LB_RSTr;
            break;
        case STORM_GROUP_BROADCAST:
            reg_idx = MANGO_STORM_PORT_BC_CTRLr;
            rst_idx = MANGO_STORM_PORT_BC_LB_RSTr;
            break;
        default:
            return RT_ERR_INPUT;
    }

    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_array_field_write(unit, reg_idx, port, REG_ARRAY_INDEX_NONE, MANGO_ENf, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    /* reset leaky bucket when conf change */
    value = 1;
    if ((ret = reg_array_field_write(unit, rst_idx, port, REG_ARRAY_INDEX_NONE, MANGO_RSTf, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }


    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_rate_portStormCtrlRate_get
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
 *      (3) The unit of granularity is 16Kbps or 1pps depend on mode set by 'dal_mango_rate_portStormCtrlRateMode_set'.
 */
int32
dal_mango_rate_portStormCtrlRate_get(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_group_t  storm_type,
    uint32                  *pRate)
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
    RT_PARAM_CHK((NULL == pRate), RT_ERR_NULL_POINTER);

    switch (storm_type)
    {
        case STORM_GROUP_UNICAST:
            reg_idx = MANGO_STORM_PORT_UC_CTRLr;
            break;
        case STORM_GROUP_MULTICAST:
            reg_idx = MANGO_STORM_PORT_MC_CTRLr;
            break;
        case STORM_GROUP_BROADCAST:
            reg_idx = MANGO_STORM_PORT_BC_CTRLr;
            break;
        default:
            return RT_ERR_INPUT;
    }

    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_array_field_read(unit, reg_idx, port, REG_ARRAY_INDEX_NONE, MANGO_RATEf, pRate)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_rate_portStormCtrlRate_set
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
 *      (3) The unit of granularity is 16Kbps or 1pps depend on mode set by 'dal_mango_rate_portStormCtrlRateMode_set'.
 */
int32
dal_mango_rate_portStormCtrlRate_set(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_group_t  storm_type,
    uint32                  rate)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value;
    uint32  reg_idx;
    uint32  rst_idx;

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
            reg_idx = MANGO_STORM_PORT_UC_CTRLr;
            rst_idx = MANGO_STORM_PORT_UC_LB_RSTr;
            break;
        case STORM_GROUP_MULTICAST:
            reg_idx = MANGO_STORM_PORT_MC_CTRLr;
            rst_idx = MANGO_STORM_PORT_MC_LB_RSTr;
            break;
        case STORM_GROUP_BROADCAST:
            reg_idx = MANGO_STORM_PORT_BC_CTRLr;
            rst_idx = MANGO_STORM_PORT_BC_LB_RSTr;
            break;
        default:
            return RT_ERR_INPUT;
    }

    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_array_field_write(unit, reg_idx, port, REG_ARRAY_INDEX_NONE, MANGO_RATEf, &rate)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    /* reset leaky bucket when conf change */
    value = 1;
    if ((ret = reg_array_field_write(unit, rst_idx, port, REG_ARRAY_INDEX_NONE, MANGO_RSTf, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_rate_portStormCtrlBurstSize_get
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
dal_mango_rate_portStormCtrlBurstSize_get(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_group_t  storm_type,
    uint32 *pBurst_size)
{
    int32   ret;
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
            reg_idx = MANGO_STORM_PORT_UC_CTRLr;
            break;
        case STORM_GROUP_MULTICAST:
            reg_idx = MANGO_STORM_PORT_MC_CTRLr;
            break;
        case STORM_GROUP_BROADCAST:
            reg_idx = MANGO_STORM_PORT_BC_CTRLr;
            break;
        default:
            return RT_ERR_INPUT;
    }

    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_array_field_read(unit, reg_idx, port, REG_ARRAY_INDEX_NONE, MANGO_BURSTf, pBurst_size)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_rate_portStormCtrlBurstSize_set
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
 *      RT_ERR_INPUT             - invalid input parameter
 * Note:
 *      (1) The storm group types are as following:
 *          rtk_rate_storm_group_t
 *          - STORM_GROUP_MULTICAST
 *          - STORM_GROUP_BROADCAST
 *          - STORM_GROUP_UNICAST
 *      (2) The unit of burst size is 1 byte or 1 packet depend on mode.
 */
int32
dal_mango_rate_portStormCtrlBurstSize_set(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_group_t  storm_type,
    uint32                  burst_size)
{
    int32   ret;
    uint32  value;
    uint32  reg_idx;
    uint32  rst_idx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, storm_type=%d",
           unit, port, storm_type);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((storm_type >= STORM_GROUP_END), RT_ERR_INPUT);
    RT_PARAM_CHK((burst_size > HAL_BURST_OF_BANDWIDTH_MAX(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK(_dal_mango_rate_portBurstSize_check(unit, port, RATE_MODULE_STORM, burst_size), RT_ERR_INPUT);

    switch (storm_type)
    {
        case STORM_GROUP_UNICAST:
            reg_idx = MANGO_STORM_PORT_UC_CTRLr;
            rst_idx = MANGO_STORM_PORT_UC_LB_RSTr;
            break;
        case STORM_GROUP_MULTICAST:
            reg_idx = MANGO_STORM_PORT_MC_CTRLr;
            rst_idx = MANGO_STORM_PORT_MC_LB_RSTr;
            break;
        case STORM_GROUP_BROADCAST:
            reg_idx = MANGO_STORM_PORT_BC_CTRLr;
            rst_idx = MANGO_STORM_PORT_BC_LB_RSTr;
            break;
        default:
            return RT_ERR_INPUT;
    }

    RATE_SEM_LOCK(unit);

    /* write value to CHIP*/
    if ((ret = reg_array_field_write(unit, reg_idx, port, REG_ARRAY_INDEX_NONE, MANGO_BURSTf, &burst_size)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    /* reset leaky bucket when conf change */
    value = 1;
    if ((ret = reg_array_field_write(unit, rst_idx, port, REG_ARRAY_INDEX_NONE, MANGO_RSTf, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_rate_portStormCtrlExceed_get
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
dal_mango_rate_portStormCtrlExceed_get(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_group_t  storm_type,
    uint32                  *pIsExceed)
{
    int32   ret;
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
            reg_idx = MANGO_STORM_PORT_UC_EXCEED_FLAGr;
            break;
        case STORM_GROUP_MULTICAST:
            reg_idx = MANGO_STORM_PORT_MC_EXCEED_FLAGr;
            break;
        case STORM_GROUP_BROADCAST:
            reg_idx = MANGO_STORM_PORT_BC_EXCEED_FLAGr;
            break;
        default:
            return RT_ERR_INPUT;
    }

    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_array_field_read(unit, reg_idx, port, REG_ARRAY_INDEX_NONE, MANGO_FLAGf, pIsExceed)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_rate_portStormCtrlExceed_reset
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
dal_mango_rate_portStormCtrlExceed_reset(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_group_t  storm_type)
{
    int32   ret;
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
            reg_idx = MANGO_STORM_PORT_UC_EXCEED_FLAGr;
            break;
        case STORM_GROUP_MULTICAST:
            reg_idx = MANGO_STORM_PORT_MC_EXCEED_FLAGr;
            break;
        case STORM_GROUP_BROADCAST:
            reg_idx = MANGO_STORM_PORT_BC_EXCEED_FLAGr;
            break;
        default:
            return RT_ERR_INPUT;
    }

    RATE_SEM_LOCK(unit);

    /*write 1 to clear*/
    if ((ret = reg_array_field_write1toClear(unit, reg_idx, port, REG_ARRAY_INDEX_NONE, MANGO_FLAGf)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_rate_portStormCtrlRateMode_get
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
dal_mango_rate_portStormCtrlRateMode_get(
    uint32                      unit,
    rtk_port_t              port,
    rtk_rate_storm_rateMode_t   *pRate_mode)
{
    uint32  value;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pRate_mode), RT_ERR_NULL_POINTER);

    RATE_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_array_field_read(unit, MANGO_STORM_PORT_CTRLr, port, REG_ARRAY_INDEX_NONE, MANGO_MODEf, &value)) != RT_ERR_OK)
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
 *      dal_mango_rate_portStormCtrlRateMode_set
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
dal_mango_rate_portStormCtrlRateMode_set(
    uint32                      unit,
    rtk_port_t              port,
    rtk_rate_storm_rateMode_t   rate_mode)
{
    uint32  burst_size;
    uint32  value;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((rate_mode >= STORM_RATE_MODE_END), RT_ERR_INPUT);

    if (rate_mode == BASED_ON_BYTE)
    {
        burst_size = MANGO_STORM_BPS_DFLT_BURST_SIZE;
        value = 1;
    }
    else
    {
        burst_size = MANGO_STORM_PPS_DFLT_BURST_SIZE;
        value = 0;
    }

    RATE_SEM_LOCK(unit);


    /* get value from CHIP */
    if ((ret = reg_array_field_write(unit, MANGO_STORM_PORT_CTRLr, port, REG_ARRAY_INDEX_NONE, MANGO_MODEf, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit, MANGO_STORM_PORT_UC_CTRLr, port, REG_ARRAY_INDEX_NONE, MANGO_BURSTf, &burst_size)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit, MANGO_STORM_PORT_MC_CTRLr, port, REG_ARRAY_INDEX_NONE, MANGO_BURSTf, &burst_size)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit, MANGO_STORM_PORT_BC_CTRLr, port, REG_ARRAY_INDEX_NONE, MANGO_BURSTf, &burst_size)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    /* reset leaky bucket when mode change */
    value = 1;
    if ((ret = reg_array_field_write(unit, MANGO_STORM_PORT_UC_LB_RSTr, port, REG_ARRAY_INDEX_NONE, MANGO_RSTf, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit, MANGO_STORM_PORT_MC_LB_RSTr, port, REG_ARRAY_INDEX_NONE, MANGO_RSTf, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    if ((ret = reg_array_field_write(unit, MANGO_STORM_PORT_BC_LB_RSTr, port, REG_ARRAY_INDEX_NONE, MANGO_RSTf, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }


    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_rate_portStormCtrlTypeSel_get
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
dal_mango_rate_portStormCtrlTypeSel_get(uint32 unit, rtk_port_t port, rtk_rate_storm_group_t storm_type, rtk_rate_storm_sel_t *pStorm_sel)
{
    int32   ret;
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
            reg_idx = MANGO_STORM_PORT_UC_CTRLr;
            break;
        case STORM_GROUP_MULTICAST:
            reg_idx = MANGO_STORM_PORT_MC_CTRLr;
            break;
        default:
            return RT_ERR_INPUT;
    }

    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_array_field_read(unit, reg_idx, port, REG_ARRAY_INDEX_NONE, MANGO_TYPEf, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    /* chip's value translate */
    if (value == 1)
        *pStorm_sel = STORM_SEL_UNKNOWN_AND_KNOWN;
    else
        *pStorm_sel = STORM_SEL_UNKNOWN;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "pStorm_sel=%d", *pStorm_sel);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_rate_portStormCtrlTypeSel_set
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
dal_mango_rate_portStormCtrlTypeSel_set(uint32 unit, rtk_port_t port, rtk_rate_storm_group_t storm_type, rtk_rate_storm_sel_t storm_sel)
{
    int32   ret;
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
            reg_idx = MANGO_STORM_PORT_UC_CTRLr;
            break;
        case STORM_GROUP_MULTICAST:
            reg_idx = MANGO_STORM_PORT_MC_CTRLr;
            break;
        default:
            return RT_ERR_INPUT;
    }

    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_array_field_write(unit, reg_idx, port, REG_ARRAY_INDEX_NONE, MANGO_TYPEf, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_rate_portStormCtrlProtoEnable_get
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
dal_mango_rate_portStormCtrlProtoEnable_get(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_proto_group_t  storm_type,
    rtk_enable_t            *pEnable)
{
    int32   ret;
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
            reg_idx = MANGO_STORM_PORT_PROTO_BPDU_CTRLr;
            break;
        case STORM_PROTO_GROUP_IGMP:
            reg_idx = MANGO_STORM_PORT_PROTO_IGMP_CTRLr;
            break;
        case STORM_PROTO_GROUP_ARP:
            reg_idx = MANGO_STORM_PORT_PROTO_ARP_CTRLr;
            break;
        case STORM_PROTO_GROUP_DHCP:
            reg_idx = MANGO_STORM_PORT_PROTO_DHCP_CTRLr;
            break;
        default:
            return RT_ERR_INPUT;
    }

    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_array_field_read(unit, reg_idx, port, REG_ARRAY_INDEX_NONE, MANGO_ENf, &value)) != RT_ERR_OK)
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
 *      dal_mango_rate_portStormCtrlProtoEnable_set
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
dal_mango_rate_portStormCtrlProtoEnable_set(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_proto_group_t  storm_type,
    rtk_enable_t            enable)
{
    int32   ret;
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
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    if (enable == ENABLED)
        value = 1;
    else
        value = 0;

    switch (storm_type)
    {
        case STORM_PROTO_GROUP_BPDU:
            reg_idx = MANGO_STORM_PORT_PROTO_BPDU_CTRLr;
            rst_idx = MANGO_STORM_PORT_PROTO_BPDU_LB_RSTr;
            break;
        case STORM_PROTO_GROUP_IGMP:
            reg_idx = MANGO_STORM_PORT_PROTO_IGMP_CTRLr;
            rst_idx = MANGO_STORM_PORT_PROTO_IGMP_LB_RSTr;
            break;
        case STORM_PROTO_GROUP_ARP:
            reg_idx = MANGO_STORM_PORT_PROTO_ARP_CTRLr;
            rst_idx = MANGO_STORM_PORT_PROTO_ARP_LB_RSTr;
            break;
        case STORM_PROTO_GROUP_DHCP:
            reg_idx = MANGO_STORM_PORT_PROTO_DHCP_CTRLr;
            rst_idx = MANGO_STORM_PORT_PROTO_DHCP_LB_RSTr;
            break;
        default:
            return RT_ERR_INPUT;
    }

    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_array_field_write(unit, reg_idx, port, REG_ARRAY_INDEX_NONE, MANGO_ENf, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    /* reset leaky bucket when conf change */
    value = 1;
    if ((ret = reg_array_field_write(unit, rst_idx, port, REG_ARRAY_INDEX_NONE, MANGO_RSTf, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_rate_portStormCtrlProtoRate_get
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
dal_mango_rate_portStormCtrlProtoRate_get(
    uint32                          unit,
    rtk_port_t                      port,
    rtk_rate_storm_proto_group_t    storm_type,
    uint32                          *pRate)
{
    int32   ret;
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
            reg_idx = MANGO_STORM_PORT_PROTO_BPDU_CTRLr;break;
        case STORM_PROTO_GROUP_IGMP:
            reg_idx = MANGO_STORM_PORT_PROTO_IGMP_CTRLr;break;
        case STORM_PROTO_GROUP_ARP:
            reg_idx = MANGO_STORM_PORT_PROTO_ARP_CTRLr;break;
        case STORM_PROTO_GROUP_DHCP:
            reg_idx = MANGO_STORM_PORT_PROTO_DHCP_CTRLr;break;
        default:
            return RT_ERR_INPUT;
    }

    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_array_field_read(unit, reg_idx, port, REG_ARRAY_INDEX_NONE, MANGO_RATEf, pRate)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_rate_portStormCtrlProtoRate_set
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
dal_mango_rate_portStormCtrlProtoRate_set(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_proto_group_t  storm_type,
    uint32                  rate)
{
    int32   ret;
    uint32  value;
    uint32  reg_idx;
    uint32  rst_idx;

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
            reg_idx = MANGO_STORM_PORT_PROTO_BPDU_CTRLr;
            rst_idx = MANGO_STORM_PORT_PROTO_BPDU_LB_RSTr;
            break;
        case STORM_PROTO_GROUP_IGMP:
            reg_idx = MANGO_STORM_PORT_PROTO_IGMP_CTRLr;
            rst_idx = MANGO_STORM_PORT_PROTO_IGMP_LB_RSTr;
            break;
        case STORM_PROTO_GROUP_ARP:
            reg_idx = MANGO_STORM_PORT_PROTO_ARP_CTRLr;
            rst_idx = MANGO_STORM_PORT_PROTO_ARP_LB_RSTr;
            break;
        case STORM_PROTO_GROUP_DHCP:
            reg_idx = MANGO_STORM_PORT_PROTO_DHCP_CTRLr;
            rst_idx = MANGO_STORM_PORT_PROTO_DHCP_LB_RSTr;
            break;
        default:
            return RT_ERR_INPUT;
    }

    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_array_field_write(unit, reg_idx, port, REG_ARRAY_INDEX_NONE, MANGO_RATEf, &rate)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    /* reset leaky bucket when conf change */
    value = 1;
    if ((ret = reg_array_field_write(unit, rst_idx, port, REG_ARRAY_INDEX_NONE, MANGO_RSTf, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }


    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_rate_portStormCtrlProtoBurstSize_get
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
dal_mango_rate_portStormCtrlProtoBurstSize_get(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_proto_group_t  storm_type,
    uint32                *pBurst_size)
{
    int32   ret;
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
            reg_idx = MANGO_STORM_PORT_PROTO_BPDU_CTRLr;break;
        case STORM_PROTO_GROUP_IGMP:
            reg_idx = MANGO_STORM_PORT_PROTO_IGMP_CTRLr;break;
        case STORM_PROTO_GROUP_ARP:
            reg_idx = MANGO_STORM_PORT_PROTO_ARP_CTRLr;break;
        case STORM_PROTO_GROUP_DHCP:
            reg_idx = MANGO_STORM_PORT_PROTO_DHCP_CTRLr;break;
        default:
            return RT_ERR_INPUT;
    }

    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_array_field_read(unit, reg_idx, port, REG_ARRAY_INDEX_NONE, MANGO_BURSTf, pBurst_size)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_rate_portStormCtrlProtoBurstSize_set
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
dal_mango_rate_portStormCtrlProtoBurstSize_set(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_proto_group_t  storm_type,
    uint32                  burst_size)
{
    int32   ret;
    uint32  value;
    uint32  reg_idx;
    uint32  rst_idx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, storm_type=%d",
           unit, port, storm_type);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((storm_type >= STORM_PROTO_GROUP_END), RT_ERR_INPUT);
    RT_PARAM_CHK((burst_size > HAL_BURST_RATE_OF_STORM_PROTO_CONTROL_MAX(unit)), RT_ERR_INPUT);

    switch (storm_type)
    {
        case STORM_PROTO_GROUP_BPDU:
            reg_idx = MANGO_STORM_PORT_PROTO_BPDU_CTRLr;
            rst_idx = MANGO_STORM_PORT_PROTO_BPDU_LB_RSTr;
            break;
        case STORM_PROTO_GROUP_IGMP:
            reg_idx = MANGO_STORM_PORT_PROTO_IGMP_CTRLr;
            rst_idx = MANGO_STORM_PORT_PROTO_IGMP_LB_RSTr;
            break;
        case STORM_PROTO_GROUP_ARP:
            reg_idx = MANGO_STORM_PORT_PROTO_ARP_CTRLr;
            rst_idx = MANGO_STORM_PORT_PROTO_ARP_LB_RSTr;
            break;
        case STORM_PROTO_GROUP_DHCP:
            reg_idx = MANGO_STORM_PORT_PROTO_DHCP_CTRLr;
            rst_idx = MANGO_STORM_PORT_PROTO_DHCP_LB_RSTr;
            break;
        default:
            return RT_ERR_INPUT;
    }

    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_array_field_write(unit, reg_idx, port, REG_ARRAY_INDEX_NONE, MANGO_BURSTf, &burst_size)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    /* reset leaky bucket when conf change */
    value = 1;
    if ((ret = reg_array_field_write(unit, rst_idx, port, REG_ARRAY_INDEX_NONE, MANGO_RSTf, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_rate_portStormCtrlProtoExceed_get
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
dal_mango_rate_portStormCtrlProtoExceed_get(
    uint32                          unit,
    rtk_port_t                      port,
    rtk_rate_storm_proto_group_t    storm_type,
    uint32                          *pIsExceed)
{
    int32   ret;
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
            reg_idx = MANGO_STORM_PORT_PROTO_BPDU_EXCEED_FLAGr;
            break;
        case STORM_PROTO_GROUP_IGMP:
            reg_idx = MANGO_STORM_PORT_PROTO_IGMP_EXCEED_FLAGr;
            break;
        case STORM_PROTO_GROUP_ARP:
            reg_idx = MANGO_STORM_PORT_PROTO_ARP_EXCEED_FLAGr;
            break;
        case STORM_PROTO_GROUP_DHCP:
            reg_idx = MANGO_STORM_PORT_PROTO_DHCP_EXCEED_FLAGr;
            break;
        default:
            return RT_ERR_INPUT;
    }

    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_array_field_read(unit, reg_idx, port, REG_ARRAY_INDEX_NONE, MANGO_FLAGf, pIsExceed)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_rate_portStormCtrlProtoExceed_reset
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
dal_mango_rate_portStormCtrlProtoExceed_reset(
    uint32                          unit,
    rtk_port_t                      port,
    rtk_rate_storm_proto_group_t    storm_type)
{
    int32   ret;
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
            reg_idx = MANGO_STORM_PORT_PROTO_BPDU_EXCEED_FLAGr;
            break;
        case STORM_PROTO_GROUP_IGMP:
            reg_idx = MANGO_STORM_PORT_PROTO_IGMP_EXCEED_FLAGr;
            break;
        case STORM_PROTO_GROUP_ARP:
            reg_idx = MANGO_STORM_PORT_PROTO_ARP_EXCEED_FLAGr;
            break;
        case STORM_PROTO_GROUP_DHCP:
            reg_idx = MANGO_STORM_PORT_PROTO_DHCP_EXCEED_FLAGr;
            break;
        default:
            return RT_ERR_INPUT;
    }

    RATE_SEM_LOCK(unit);

    /*write 1 to clear*/
    if ((ret = reg_array_field_write1toClear(unit, reg_idx, port, REG_ARRAY_INDEX_NONE, MANGO_FLAGf)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_rate_stormControlBypass_get
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
dal_mango_rate_stormControlBypass_get(uint32 unit, rtk_rate_storm_bypassType_t bypassType, rtk_enable_t  *pEnable)
{
    int32   ret;
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
            reg_field = MANGO_ADMIT_RMAf;
            break;
        case STORM_BYPASS_BPDU:
            reg_field = MANGO_ADMIT_BPDUf;
            break;
        case STORM_BYPASS_RTKPKT:
            reg_field = MANGO_ADMIT_RTKPKTf;
            break;
        case STORM_BYPASS_IGMP:
            reg_field = MANGO_ADMIT_IGMPf;
            break;
        case STORM_BYPASS_ARP:
            reg_field = MANGO_ADMIT_ARPREQf;
            break;
        case STORM_BYPASS_DHCP:
            reg_field = MANGO_ADMIT_DHCPf;
            break;
        case STORM_BYPASS_RIP:
            reg_field = MANGO_ADMIT_RIPf;
            break;
        default:
            return RT_ERR_INPUT;
    }

    RATE_SEM_LOCK(unit);
    /* get value from CHIP */
    if ((ret = reg_field_read(unit, MANGO_STORM_CTRLr, reg_field, &value)) != RT_ERR_OK)
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
 *      dal_mango_rate_stormControlBypass_set
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
dal_mango_rate_stormControlBypass_set(uint32 unit, rtk_rate_storm_bypassType_t bypassType, rtk_enable_t enable)
{
    int32   ret;
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
            reg_field = MANGO_ADMIT_RMAf;
            break;
        case STORM_BYPASS_BPDU:
            reg_field = MANGO_ADMIT_BPDUf;
            break;
        case STORM_BYPASS_RTKPKT:
            reg_field = MANGO_ADMIT_RTKPKTf;
            break;
        case STORM_BYPASS_IGMP:
            reg_field = MANGO_ADMIT_IGMPf;
            break;
        case STORM_BYPASS_ARP:
            reg_field = MANGO_ADMIT_ARPREQf;
            break;
        case STORM_BYPASS_DHCP:
            reg_field = MANGO_ADMIT_DHCPf;
            break;
        case STORM_BYPASS_RIP:
            reg_field = MANGO_ADMIT_RIPf;
            break;
        default:
            return RT_ERR_INPUT;
    }

    RATE_SEM_LOCK(unit);
    /* get value from CHIP */
    if ((ret = reg_field_write(unit, MANGO_STORM_CTRLr, reg_field, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_rate_stormControlBurstSize_get
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
dal_mango_rate_stormControlBurstSize_get(
    uint32                  unit,
    rtk_rate_storm_group_t  storm_type,
    uint32                  *pBurst_size)
{
    rtk_port_t      port;

    RT_PARAM_CHK((NULL == pBurst_size), RT_ERR_NULL_POINTER);

    HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
    {
        return dal_mango_rate_portStormCtrlBurstSize_get(unit, port, storm_type, pBurst_size);
    }
    return RT_ERR_FAILED;
}

/* Function Name:
 *      dal_mango_rate_stormControlBurstSize_set
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
dal_mango_rate_stormControlBurstSize_set(
    uint32                  unit,
    rtk_rate_storm_group_t  storm_type,
    uint32                  burst_size)
{
    int32 ret;
    rtk_port_t port;

    HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
    {
        if ((ret = dal_mango_rate_portStormCtrlBurstSize_set(unit, port, storm_type, burst_size)) != RT_ERR_OK)
            return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_rate_stormControlRateMode_get
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
dal_mango_rate_stormControlRateMode_get(
    uint32                      unit,
    rtk_rate_storm_rateMode_t   *pRate_mode)
{
    rtk_port_t      port;

    HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
    {
        return dal_mango_rate_portStormCtrlRateMode_get(unit, port, pRate_mode);
    }

    return RT_ERR_FAILED;
}

/* Function Name:
 *      dal_mango_rate_stormControlRateMode_set
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
dal_mango_rate_stormControlRateMode_set(
    uint32                      unit,
    rtk_rate_storm_rateMode_t   rate_mode)
{
    int32 ret;
    rtk_port_t port;

    HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
    {
        if ((ret = dal_mango_rate_portStormCtrlRateMode_set(unit, port, rate_mode)) != RT_ERR_OK)
            return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_mango_rate_stormCtrlProtoVlanConstrtEnable_get
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
dal_mango_rate_stormCtrlProtoVlanConstrtEnable_get(uint32 unit, rtk_rate_storm_proto_group_t storm_type, rtk_enable_t *pEnable)
{
    int32   ret;
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
            reg_field = MANGO_ARP_VLAN_CONSTRTf;break;
        case STORM_PROTO_GROUP_DHCP:
            reg_field = MANGO_DHCP_VLAN_CONSTRTf;break;
        default:
            return RT_ERR_INPUT;
    }

    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_field_read(unit,  MANGO_STORM_CTRLr, reg_field, &value)) != RT_ERR_OK)
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
 *      dal_mango_rate_stormCtrlProtoVlanConstrtEnable_set
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
dal_mango_rate_stormCtrlProtoVlanConstrtEnable_set(uint32 unit, rtk_rate_storm_proto_group_t storm_type, rtk_enable_t enable)
{
    int32   ret;
    uint32 value;
    uint32  reg_field;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, storm_type=%d",
           unit, storm_type);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    switch (storm_type)
    {
        case STORM_PROTO_GROUP_ARP:
            reg_field = MANGO_ARP_VLAN_CONSTRTf;break;
        case STORM_PROTO_GROUP_DHCP:
            reg_field = MANGO_DHCP_VLAN_CONSTRTf;break;
        default:
            return RT_ERR_INPUT;
    }

    if (enable == ENABLED)
        value = 1;
    else
        value = 0;

    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_field_write(unit,  MANGO_STORM_CTRLr, reg_field, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      _dal_mango_rate_init_config
 * Description:
 *      Initialize default configuration for  the rate module of the specified device..
 * Input:
 *      unit                - unit id
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
static int32
_dal_mango_rate_init_config(uint32 unit)
{
    /* [SS-972]-START */
    int32   ret;
    rtk_port_t  port;
    rtk_qid_t   queue;
    uint32  value, rate;
    rtk_rate_assuredMode_t  mode;
    egr_qBw_entry_t egrQEntry;
    rtk_enable_t    enable;

    osal_memset(&egrQEntry, 0, sizeof(egr_qBw_entry_t));

    /* Get chip value to init shadow */
    HWP_PORT_TRAVS(unit, port)
    {
        if ((ret = reg_array_field_read(unit, MANGO_EGBW_PORT_CTRLr, port, REG_ARRAY_INDEX_NONE, MANGO_RATEf, &rate)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }
        *pEgrPort_rate[unit][port] = rate;
    }

    HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
    {
        if ((ret = table_read(unit, MANGO_EGR_Q_BWt, port, (uint32 *) &egrQEntry)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }

        for (queue = 0; queue < MANGO_EGRBW_MAX_NUM_OF_ASSUREDQ; queue++)
        {
            /* Assured State */
            if ((ret = table_field_get(unit, MANGO_EGR_Q_BWt, (uint32)egrAssureQBwEn_fieldidx[queue],
                            &value, (uint32 *) &egrQEntry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
                return ret;
            }
            if (value == 1)
                enable = ENABLED;
            else
                enable = DISABLED;

            if (enable == ENABLED)
                BITMAP_SET(pAssureQEn[unit][port], queue);
            else
                BITMAP_CLEAR(pAssureQEn[unit][port], queue);

            /* Assured Mode - Share or Fixed */
            if ((ret = table_field_get(unit, MANGO_EGR_Q_BWt, (uint32)egrAssureQBwMode_fieldidx[queue],
                            &value, (uint32 *) &egrQEntry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
                return ret;
            }
            if (value == 0)
                mode = ASSURED_MODE_SHARE;
            else
                mode = ASSURED_MODE_FIX;

            if (mode == ASSURED_MODE_SHARE)
                BITMAP_SET(pAssureQShare[unit][port], queue);
            else
                BITMAP_CLEAR(pAssureQShare[unit][port], queue);

            /* Assured Rate */
            if ((ret = table_field_get(unit, MANGO_EGR_Q_BWt, (uint32)egrAssureQBwRate_fieldidx[queue],
                            &value, (uint32 *) &egrQEntry)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
                return ret;
            }
            *pAssure_rate[unit][port][queue] = value;
        }
    }
    /* [SS-972]-END */

    return RT_ERR_OK;
} /* end of _dal_cypress_rate_init_config */
