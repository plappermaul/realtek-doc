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
#include <hal/chipdef/cypress/rtk_cypress_table_struct.h>
#include <hal/chipdef/cypress/rtk_cypress_reg_struct.h>
#include <hal/mac/reg.h>
#include <hal/mac/mem.h>
#include <hal/common/halctrl.h>
#include <dal/cypress/dal_cypress_common.h>
#include <dal/cypress/dal_cypress_qos.h>
#include <dal/cypress/dal_cypress_rate.h>
#include <dal/cypress/dal_cypress_port.h>
#include <dal/dal_linkMon.h>
#include <dal/dal_mgmt.h>
#include <rtk/default.h>
#include <rtk/rate.h>

/*
 * Symbol Definition
 */
#define DAL_CYPRESS_RATE_DISABLED_EGR_BANDWIDTH_RATE_GE_PORT            0xFFFF
#define DAL_CYPRESS_RATE_DISABLED_EGR_BANDWIDTH_RATE_10GE_PORT          0xFFFFF

typedef struct dal_cypress_rate_egr_bandwidth_ctrl_s {
    uint32  status;
    uint32  rate;
} dal_cypress_rate_egr_bandwidth_ctrl_t;

typedef struct dal_cypress_rate_igr_bandwidth_ctrl_s {
    uint32  rate;
} dal_cypress_rate_igr_bandwidth_ctrl_t;

typedef enum dal_cypress_rate_igr_bandwidth_ctrl_act_e {
    IGR_ACT_RCV = 0,
    IGR_ACT_DROP,
    IGR_ACT_END
} dal_cypress_rate_igr_bandwidth_ctrl_act_t;

typedef struct dal_cypress_rate_info_s {
    uint8   stormControl_enable[RTK_MAX_NUM_OF_PORTS][STORM_GROUP_END];
    uint32  stormControl_rate[RTK_MAX_NUM_OF_PORTS][STORM_GROUP_END];
} dal_cypress_rate_info_t;

/*
 * Data Declaration
 */
static uint32               rate_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};
static osal_mutex_t         rate_sem[RTK_MAX_NUM_OF_UNIT];

static dal_cypress_rate_egr_bandwidth_ctrl_t   egr_bandwidth_ctrl[RTK_MAX_NUM_OF_UNIT][RTK_MAX_NUM_OF_PORTS];
static dal_cypress_rate_egr_bandwidth_ctrl_t   egr_queue_bandwidth_ctrl[RTK_MAX_NUM_OF_UNIT][RTK_MAX_NUM_OF_PORTS][RTK_MAX_NUM_OF_QUEUE];
static dal_cypress_rate_igr_bandwidth_ctrl_t   igr_bandwidth_ctrl[RTK_MAX_NUM_OF_UNIT][RTK_MAX_NUM_OF_PORTS];
const static uint16 egrBandwidth_queue_rate_fieldidx[] = {CYPRESS_SCHED_LB_APR_Q0tf, CYPRESS_SCHED_LB_APR_Q1tf, CYPRESS_SCHED_LB_APR_Q2tf, CYPRESS_SCHED_LB_APR_Q3tf, CYPRESS_SCHED_LB_APR_Q4tf, CYPRESS_SCHED_LB_APR_Q5tf, CYPRESS_SCHED_LB_APR_Q6tf, CYPRESS_SCHED_LB_APR_Q7tf};

const static uint16 egrBandwidth_queue_fix_bw_fieldidx[] = {CYPRESS_SCHED_FIX_TKN_Q0tf, CYPRESS_SCHED_FIX_TKN_Q1tf,
                                              CYPRESS_SCHED_FIX_TKN_Q2tf, CYPRESS_SCHED_FIX_TKN_Q3tf,
                                              CYPRESS_SCHED_FIX_TKN_Q4tf, CYPRESS_SCHED_FIX_TKN_Q5tf,
                                              CYPRESS_SCHED_FIX_TKN_Q6tf, CYPRESS_SCHED_FIX_TKN_Q7tf};
static dal_cypress_rate_info_t   *pRate_info[RTK_MAX_NUM_OF_UNIT];

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

/* Function Name:
 *      dal_cypress_rateMapper_init
 * Description:
 *      Hook rate module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must Hook rate module before calling any rate APIs.
 */
int32
dal_cypress_rateMapper_init(dal_mapper_t *pMapper)
{
    pMapper->rate_init = dal_cypress_rate_init;
    pMapper->rate_includeIfg_get = dal_cypress_rate_includeIfg_get;
    pMapper->rate_includeIfg_set = dal_cypress_rate_includeIfg_set;
    pMapper->rate_portIgrBwCtrlEnable_get = dal_cypress_rate_portIgrBwCtrlEnable_get;
    pMapper->rate_portIgrBwCtrlEnable_set = dal_cypress_rate_portIgrBwCtrlEnable_set;
    pMapper->rate_portIgrBwCtrlRate_get = dal_cypress_rate_portIgrBwCtrlRate_get;
    pMapper->rate_portIgrBwCtrlRate_set = dal_cypress_rate_portIgrBwCtrlRate_set;
    pMapper->rate_igrBandwidthLowThresh_get = dal_cypress_rate_igrBandwidthLowThresh_get;
    pMapper->rate_igrBandwidthLowThresh_set = dal_cypress_rate_igrBandwidthLowThresh_set;
    pMapper->rate_portIgrBandwidthHighThresh_get = dal_cypress_rate_portIgrBandwidthHighThresh_get;
    pMapper->rate_portIgrBandwidthHighThresh_set = dal_cypress_rate_portIgrBandwidthHighThresh_set;
    pMapper->rate_igrBwCtrlBurstSize_get = dal_cypress_rate_igrBwCtrlBurstSize_get;
    pMapper->rate_igrBwCtrlBurstSize_set = dal_cypress_rate_igrBwCtrlBurstSize_set;
    pMapper->rate_portIgrBwCtrlBurstSize_get = dal_cypress_rate_portIgrBwCtrlBurstSize_get;
    pMapper->rate_portIgrBwCtrlBurstSize_set = dal_cypress_rate_portIgrBwCtrlBurstSize_set;
    pMapper->rate_portIgrBandwidthCtrlExceed_get = dal_cypress_rate_portIgrBandwidthCtrlExceed_get;
    pMapper->rate_portIgrBandwidthCtrlExceed_reset = dal_cypress_rate_portIgrBandwidthCtrlExceed_reset;
    pMapper->rate_igrBandwidthCtrlBypass_get = dal_cypress_rate_igrBandwidthCtrlBypass_get;
    pMapper->rate_igrBandwidthCtrlBypass_set = dal_cypress_rate_igrBandwidthCtrlBypass_set;
    pMapper->rate_portIgrBwFlowctrlEnable_get = dal_cypress_rate_portIgrBwFlowctrlEnable_get;
    pMapper->rate_portIgrBwFlowctrlEnable_set = dal_cypress_rate_portIgrBwFlowctrlEnable_set;
    pMapper->rate_portEgrBwCtrlEnable_get = dal_cypress_rate_portEgrBwCtrlEnable_get;
    pMapper->rate_portEgrBwCtrlEnable_set = dal_cypress_rate_portEgrBwCtrlEnable_set;
    pMapper->rate_portEgrBwCtrlRate_get = dal_cypress_rate_portEgrBwCtrlRate_get;
    pMapper->rate_portEgrBwCtrlRate_set = dal_cypress_rate_portEgrBwCtrlRate_set;
    pMapper->rate_cpuEgrBandwidthCtrlRateMode_get = dal_cypress_rate_cpuEgrBandwidthCtrlRateMode_get;
    pMapper->rate_cpuEgrBandwidthCtrlRateMode_set = dal_cypress_rate_cpuEgrBandwidthCtrlRateMode_set;
    pMapper->rate_egrPortBwCtrlBurstSize_get = dal_cypress_rate_egrPortBwCtrlBurstSize_get;
    pMapper->rate_egrPortBwCtrlBurstSize_set = dal_cypress_rate_egrPortBwCtrlBurstSize_set;
    pMapper->rate_portEgrQueueBwCtrlEnable_get = dal_cypress_rate_portEgrQueueBwCtrlEnable_get;
    pMapper->rate_portEgrQueueBwCtrlEnable_set = dal_cypress_rate_portEgrQueueBwCtrlEnable_set;
    pMapper->rate_portEgrQueueBwCtrlRate_get = dal_cypress_rate_portEgrQueueBwCtrlRate_get;
    pMapper->rate_portEgrQueueBwCtrlRate_set = dal_cypress_rate_portEgrQueueBwCtrlRate_set;
    pMapper->rate_egrQueueFixedBandwidthEnable_get = dal_cypress_rate_egrQueueFixedBandwidthEnable_get;
    pMapper->rate_egrQueueFixedBandwidthEnable_set = dal_cypress_rate_egrQueueFixedBandwidthEnable_set;
    pMapper->rate_egrQueueBwCtrlBurstSize_get = dal_cypress_rate_egrQueueBwCtrlBurstSize_get;
    pMapper->rate_egrQueueBwCtrlBurstSize_set = dal_cypress_rate_egrQueueBwCtrlBurstSize_set;
    pMapper->rate_portStormCtrlEnable_get = dal_cypress_rate_portStormCtrlEnable_get;
    pMapper->rate_portStormCtrlEnable_set = dal_cypress_rate_portStormCtrlEnable_set;
    pMapper->rate_portStormCtrlRate_get = dal_cypress_rate_portStormCtrlRate_get;
    pMapper->rate_portStormCtrlRate_set = dal_cypress_rate_portStormCtrlRate_set;
    pMapper->rate_portStormCtrlBurstSize_get = dal_cypress_rate_portStormCtrlBurstSize_get;
    pMapper->rate_portStormCtrlBurstSize_set = dal_cypress_rate_portStormCtrlBurstSize_set;
    pMapper->rate_portStormCtrlTypeSel_get = dal_cypress_rate_portStormCtrlTypeSel_get;
    pMapper->rate_portStormCtrlTypeSel_set = dal_cypress_rate_portStormCtrlTypeSel_set;
    pMapper->rate_portStormCtrlExceed_get = dal_cypress_rate_portStormCtrlExceed_get;
    pMapper->rate_portStormCtrlExceed_reset = dal_cypress_rate_portStormCtrlExceed_reset;
    pMapper->rate_portStormCtrlProtoRate_get = dal_cypress_rate_portStormCtrlProtoRate_get;
    pMapper->rate_portStormCtrlProtoRate_set = dal_cypress_rate_portStormCtrlProtoRate_set;
    pMapper->rate_portStormCtrlProtoExceed_get = dal_cypress_rate_portStormCtrlProtoExceed_get;
    pMapper->rate_portStormCtrlProtoExceed_reset = dal_cypress_rate_portStormCtrlProtoExceed_reset;
    pMapper->rate_stormControlRateMode_get = dal_cypress_rate_stormControlRateMode_get;
    pMapper->rate_stormControlRateMode_set = dal_cypress_rate_stormControlRateMode_set;
    pMapper->rate_stormControlBypass_get = dal_cypress_rate_stormControlBypass_get;
    pMapper->rate_stormControlBypass_set = dal_cypress_rate_stormControlBypass_set;

    return RT_ERR_OK;
}

static int32 _dal_cypress_rate_init_config(uint32 unit);
static int32 _dal_cypress_rate_egrBandwidthCtrlMaxRate_get(uint32 unit, rtk_port_t port, uint32 *pRate);


/* Function Name:
 *      _dal_cypress_rate_egrBandwidthCtrlMaxRate_get
 * Description:
 *      Get the egress bandwidth control maximum rate.
 * Input:
 *      unit                - unit id
 *      port                - port id
 * Output:
 *      pRate               - maximum rate
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID       - Invalid port id
 *      RT_ERR_RATE          - Invalid input rate
 * Note:
 *    None
 */
static int32
_dal_cypress_rate_egrBandwidthCtrlMaxRate_get(uint32 unit, rtk_port_t port, uint32 *pRate)
{
    int32 ret;
    rtk_enable_t enable;
    uint32 speed, value;

    /* read link speed from CHIP*/
    if (HWP_IS_CPU_PORT(unit, port))
    {
        speed = RATE_LINK_SPEED_END; /* unlimited */
    }
    else
    {
        /* Check 500M status first */
        if ((ret = reg_array_field_read(unit, CYPRESS_MAC_LINK_500M_STSr, port, REG_ARRAY_INDEX_NONE, CYPRESS_LINK_500M_STSf, &enable)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }
        if (ENABLED == enable)
        {
            speed = RATE_LINK_SPEED_500M;
        }
        else
        {
            if ((ret = reg_array_field_read(unit, CYPRESS_MAC_FORCE_MODE_CTRLr, port, REG_ARRAY_INDEX_NONE, CYPRESS_MAC_FORCE_ENf, &enable)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
                return ret;
            }
            if (ENABLED == enable) /* Force mode */
            {
                if ((ret = reg_array_field_read(unit, CYPRESS_MAC_FORCE_MODE_CTRLr, port, REG_ARRAY_INDEX_NONE, CYPRESS_SPD_SELf, &value)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
                    return ret;
                }
            }
            else /* NWay mode */
            {
                if ((ret = reg_array_field_read(unit, CYPRESS_MAC_LINK_SPD_STSr, port, REG_ARRAY_INDEX_NONE, CYPRESS_SPD_STSf, &value)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
                    return ret;
                }

                if ((value == 0x0) && (HWP_10GE_PORT(unit, port)))
                {
                    uint32     i, link;
                    for (i = 0; i < 10; i++)
                    {
                        if ((ret = reg_array_field_read(unit, CYPRESS_MAC_LINK_STSr, port, REG_ARRAY_INDEX_NONE, CYPRESS_LINK_STSf, &link)) != RT_ERR_OK)
                        {
                            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
                            return ret;
                        }
                        if ((ret = reg_array_field_read(unit, CYPRESS_MAC_LINK_SPD_STSr, port, REG_ARRAY_INDEX_NONE, CYPRESS_SPD_STSf, &value)) != RT_ERR_OK)
                        {
                            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
                            return ret;
                        }
                        if (link == 1) /* until link state up */
                            break;
                    }
                    if (i >= 10)
                    {
                        value = 0x3;
                    }
                }
            }

            if (value == 0x0)
                speed = RATE_LINK_SPEED_10M;
            else if (value == 0x1)
                speed = RATE_LINK_SPEED_100M;
            else if (value == 0x2)
                speed = RATE_LINK_SPEED_1G;
            else if (value == 0x3)
                speed = RATE_LINK_SPEED_10G;
            else
                speed = RATE_LINK_SPEED_1G;
        }
    }

    switch (speed)
    {
        case RATE_LINK_SPEED_10M:
            *pRate = 625;
            break;
        case RATE_LINK_SPEED_100M:
            *pRate = 6250;
            break;
        case RATE_LINK_SPEED_500M:
            *pRate = 31250;
            break;
        case RATE_LINK_SPEED_1G:
            *pRate = 62500;
            break;
        case RATE_LINK_SPEED_10G:
            *pRate = 625000;
            break;
        default:
            *pRate = 0xFFFFF;
    }

    return RT_ERR_OK;
} /* end of _dal_cypress_rate_egrBandwidthCtrlMaxRate_get */

/* Function Name:
 *      dal_cypress_rate_egrBandwidthCtrlLinkChange_handler
 * Description:
 *      Dispatch maximum token refill rate of egress bandwith by link speed
 * Input:
 *      unit           - unit id
 *      port           - port id
 *      new_linkStatus - new link status
 * Output:
 *      None
 * Return:
 *      SUCCESS
 *      FAILED
 * Note:
 *      None
 */
int32 dal_cypress_rate_egrBandwidthCtrlLinkChange_handler(int32 unit, rtk_port_t port, rtk_port_linkStatus_t new_linkStatus)
{
    int32 ret;
    sched_entry_t pSchedEntry;
    uint32 maxRate;

    /* Break LINK DOWN status */
    if (PORT_LINKDOWN == new_linkStatus)
        return 0;

    RATE_SEM_LOCK(unit);

    if ((ret = _dal_cypress_rate_egrBandwidthCtrlMaxRate_get(unit, port, &maxRate)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    if ((PORT_LINKUP == new_linkStatus) && (DISABLED == egr_bandwidth_ctrl[unit][port].status))
    {
        /* maxRate decided by link speed */
    }
    else if ((PORT_LINKUP == new_linkStatus) && (ENABLED == egr_bandwidth_ctrl[unit][port].status))
    {
        if ((maxRate > egr_bandwidth_ctrl[unit][port].rate))
            maxRate = egr_bandwidth_ctrl[unit][port].rate;

    }

    /* program value to CHIP */
    if ((ret = table_read(unit, CYPRESS_SCHEDt, port, (uint32 *) &pSchedEntry)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    if ((ret = table_field_set(unit, CYPRESS_SCHEDt, CYPRESS_SCHED_EGR_RATEtf,
                    &maxRate, (uint32 *) &pSchedEntry)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    if ((ret = table_write(unit, CYPRESS_SCHEDt, port, (uint32 *) &pSchedEntry)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return 0;
}

/* Function Name:
 *      dal_cypress_rate_init
 * Description:
 *      Initial the rate module of the specified device..
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
int32
dal_cypress_rate_init(uint32 unit)
{
    int32   ret;

    RT_INIT_REENTRY_CHK(rate_init[unit]);
    rate_init[unit] = INIT_NOT_COMPLETED;

    /* create semaphore */
    rate_sem[unit] = osal_sem_mutex_create();
    if (0 == rate_sem[unit])
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_RATE), "semaphore create failed");
        return RT_ERR_FAILED;
    }

    osal_memset(&egr_bandwidth_ctrl, 0, sizeof(egr_bandwidth_ctrl));
    osal_memset(&egr_queue_bandwidth_ctrl, 0, sizeof(egr_queue_bandwidth_ctrl));

    pRate_info[unit] = (dal_cypress_rate_info_t *)osal_alloc(sizeof(dal_cypress_rate_info_t));
    if (NULL == pRate_info[unit])
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_RATE), "memory allocate failed");
        return RT_ERR_FAILED;
    }
    osal_memset(pRate_info[unit], 0, sizeof(dal_cypress_rate_info_t));

    /* set init flag to complete init */
    rate_init[unit] = INIT_COMPLETED;

    if (( ret = _dal_cypress_rate_init_config(unit)) != RT_ERR_OK)
    {
        rate_init[unit] = INIT_NOT_COMPLETED;
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_cypress_rate_init */

/* Function Name:
 *      dal_cypress_rate_portIgrBwCtrlEnable_get
 * Description:
 *      Get the ingress bandwidth control status.
 * Input:
 *      unit                - unit id
 *      port                - port id
 * Output:
 *      pEnable            - status of ingress bandwidth control
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID       - Invalid port id
 *      RT_ERR_NULL_POINTER  - NULL pointer
 * Note:
 *    The status of ingress bandwidth control is as following:
 *    - DISABLED
 *    - ENABLED
 */
int32
dal_cypress_rate_portIgrBwCtrlEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  value, reg_idx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d",
           unit, port);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);

    if (HWP_10GE_PORT(unit, port))
    {
        reg_idx = CYPRESS_IGR_BWCTRL_PORT_CTRL_10Gr;
        if(port == 24)
            port = 0;
        else if(port == 36)
            port = 1;
    }
    else
        reg_idx = CYPRESS_IGR_BWCTRL_PORT_CTRLr;

    RATE_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, reg_idx, port, 0, CYPRESS_PORT_ENf, &value)) != RT_ERR_OK)
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
} /* end of dal_cypress_rate_portIgrBwCtrlEnable_get */

/* Function Name:
 *      dal_cypress_rate_portIgrBwCtrlEnable_set
 * Description:
 *      Set the ingress bandwidth control status.
 * Input:
 *      unit                - unit id
 *      port                - port id
 *      enable              - status of ingress bandwidth control
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID       - Invalid port id
 *      RT_ERR_INPUT         - Invalid input parameter
 * Note:
 *    The status of ingress bandwidth control is as following:
 *    - DISABLED
 *    - ENABLED
 */
int32
dal_cypress_rate_portIgrBwCtrlEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
    uint32  value, reg_idx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, enable",
           unit, port, enable);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(enable >= RTK_ENABLE_END, RT_ERR_INPUT);
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);

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

    if (HWP_10GE_PORT(unit, port))
    {
        reg_idx = CYPRESS_IGR_BWCTRL_PORT_CTRL_10Gr;
        if(port == 24)
            port = 0;
        else if(port == 36)
            port = 1;
    }
    else
        reg_idx = CYPRESS_IGR_BWCTRL_PORT_CTRLr;

    RATE_SEM_LOCK(unit);

    /* program value into CHIP*/
    if ((ret = reg_array_field_write(unit, reg_idx, port, 0, CYPRESS_PORT_ENf, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_rate_portIgrBwCtrlEnable_set */

/* Function Name:
 *      dal_cypress_rate_portIgrBwCtrlRate_get
 * Description:
 *      Get the ingress bandwidth control rate.
 * Input:
 *      unit        - unit id
 *      port        - port id
 * Output:
 *      pRate      - ingress bandwidth control rate
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID       - Invalid port id
 *      RT_ERR_NULL_POINTER  - NULL pointer
 * Note:
 *    The actual rate is "rate * chip granularity".
 *    The unit of granularity in RTL8389/RTL8329 is 16Kbps.
 */
int32
dal_cypress_rate_portIgrBwCtrlRate_get(uint32 unit, rtk_port_t port, uint32 *pRate)
{
    int32   ret;
    uint32  reg_idx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d",
           unit, port);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pRate), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);

    RATE_SEM_LOCK(unit);

    if (HWP_10GE_PORT(unit, port))
        {
        reg_idx = CYPRESS_IGR_BWCTRL_PORT_CTRL_10Gr;
        if(port == 24)
            port = 0;
        else if(port == 36)
            port = 1;
    }
    else
        reg_idx = CYPRESS_IGR_BWCTRL_PORT_CTRLr;

    if (igr_bandwidth_ctrl[unit][port].rate == HAL_GET_MAX_BANDWIDTH_OF_PORT(unit, port))
    {
        *pRate = igr_bandwidth_ctrl[unit][port].rate;
    }
    else
    {
        /* get value from CHIP*/
        if ((ret = reg_array_field_read(unit, reg_idx, port, 0, CYPRESS_RATEf, pRate)) != RT_ERR_OK)
            {
                RATE_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
                return ret;
            }
    }

    RATE_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "pRate=%d",
           *pRate);

    return RT_ERR_OK;
} /* end of dal_cypress_rate_portIgrBwCtrlRate_get */

/* Function Name:
 *      dal_cypress_rate_portIgrBwCtrlRate_set
 * Description:
 *      Set the ingress bandwidth control rate.
 * Input:
 *      unit        - unit id
 *      port        - port id
 *      rate        - ingress bandwidth control rate
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID       - Invalid port id
 *      RT_ERR_RATE          - Invalid input rate
 * Note:
 *    The actual rate is "rate * chip granularity".
 *    The unit of granularity in RTL8389/RTL8329 is 16Kbps.
 */
int32
dal_cypress_rate_portIgrBwCtrlRate_set(uint32 unit, rtk_port_t port, uint32 rate)
{
    int32   ret;
    uint32  reg_idx, chip_rate;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, rate=%d",
           unit, port, rate);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    if (HWP_10GE_PORT(unit, port))
        RT_PARAM_CHK((rate > HAL_RATE_OF_10G_BANDWIDTH_MAX(unit)), RT_ERR_RATE);
    else
        RT_PARAM_CHK((rate > HAL_RATE_OF_BANDWIDTH_MAX(unit)), RT_ERR_RATE);
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);

    RATE_SEM_LOCK(unit);

    /* Setting unlimit rate avoiding giga rate couldn't wire speed */
    if (rate == HAL_GET_MAX_BANDWIDTH_OF_PORT(unit, port)) /* The value of Giga port is 0xF424 */
    {
        if (HWP_10GE_PORT(unit, port))
            chip_rate = HAL_RATE_OF_10G_BANDWIDTH_MAX(unit);
        else
            chip_rate = HAL_RATE_OF_BANDWIDTH_MAX(unit);
    }
    else
    {
        chip_rate = rate;
    }

    if (HWP_10GE_PORT(unit, port))
    {
        reg_idx = CYPRESS_IGR_BWCTRL_PORT_CTRL_10Gr;
        if(port == 24)
            port = 0;
        else if(port == 36)
            port = 1;
    }
    else
        reg_idx = CYPRESS_IGR_BWCTRL_PORT_CTRLr;

    /* program value to CHIP*/
    if ((ret = reg_array_field_write(unit, reg_idx, port, 0, CYPRESS_RATEf, &chip_rate)) != RT_ERR_OK)
        {
            RATE_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }

    igr_bandwidth_ctrl[unit][port].rate = rate;

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_rate_portIgrBwCtrlRate_set */

/* Function Name:
 *      dal_cypress_rate_portIgrBwCtrlBurstSize_get
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
dal_cypress_rate_portIgrBwCtrlBurstSize_get(uint32 unit, rtk_port_t port, rtk_rate_igrBwBurst_cfg_t *pCfg)
{
    int32   ret;
    uint32 highThresh;

    if ((ret = dal_cypress_rate_portIgrBandwidthHighThresh_get(unit, port, &highThresh)) != RT_ERR_OK)
        return ret;

    pCfg->burst_high = pCfg->burst_low = highThresh;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_rate_portIgrBwCtrlBurstSize_set
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
dal_cypress_rate_portIgrBwCtrlBurstSize_set(uint32 unit, rtk_port_t port, rtk_rate_igrBwBurst_cfg_t *pCfg)
{
    int32   ret;
    uint32  highThresh;

    highThresh = pCfg->burst_high;

    if ((ret = dal_cypress_rate_portIgrBandwidthHighThresh_set(unit, port, highThresh)) != RT_ERR_OK)
        return ret;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_rate_igrBandwidthCtrlIncludeIfg_get
 * Description:
 *      Get the status of ingress bandwidth control includes IFG or not.
 * Input:
 *      unit                    - unit id
 * Output:
 *      pIfg_include           - include IFG or not
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER     - NULL pointer
 * Note:
 *      1. Ingress bandwidth control includes/excludes the Preamble & IFG (20 Bytes).
 *
 *      2. The status of ifg_include:
 *         - DISABLED
 *         - ENABLED
 */
int32
dal_cypress_rate_igrBandwidthCtrlIncludeIfg_get(uint32 unit, rtk_enable_t *pIfg_include)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIfg_include), RT_ERR_NULL_POINTER);

    RATE_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_field_read(unit, CYPRESS_IGR_BWCTRL_CTRLr, CYPRESS_INC_IFGf, &value)) != RT_ERR_OK)
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
} /* end of dal_cypress_rate_igrBandwidthCtrlIncludeIfg_get */

/* Function Name:
 *      dal_cypress_rate_igrBandwidthCtrlIncludeIfg_set
 * Description:
 *      Set the status of ingress bandwidth control includes IFG or not.
 * Input:
 *      unit                    - unit id
 *      ifg_include             - include IFG or not
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT                 - Invalid input parameter
 * Note:
 *      1. Ingress bandwidth control includes/excludes the Preamble & IFG (20 Bytes).
 *
 *      2. The status of ifg_include:
 *         - DISABLED
 *         - ENABLED
 */
int32
dal_cypress_rate_igrBandwidthCtrlIncludeIfg_set(uint32 unit, rtk_enable_t ifg_include)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, ifg_include=%d",
           unit, ifg_include);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(ifg_include >= RTK_ENABLE_END, RT_ERR_INPUT);

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

    /* program value into CHIP*/
    if ((ret = reg_field_write(unit, CYPRESS_IGR_BWCTRL_CTRLr, CYPRESS_INC_IFGf, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_rate_igrBandwidthCtrlIncludeIfg_set */

/* Function Name:
 *      dal_cypress_rate_igrBandwidthCtrlBypass_get
 * Description:
 *      Get the status of bypass ingress bandwidth control for specified frame type.
 * Input:
 *      unit                   - unit id
 *      bypassType             - bypass type
 * Output:
 *      pEnable                - pointer to enable status of bypass ingress bandwidth control
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT           - Invalid input parameter
 *      RT_ERR_NULL_POINTER     - NULL pointer
 * Note:
 *      None.
 */
int32
dal_cypress_rate_igrBandwidthCtrlBypass_get(uint32 unit, rtk_rate_igr_bypassType_t bypassType, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, bypassType=%d", unit, bypassType);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(bypassType >= IGR_BYPASS_END, RT_ERR_INPUT);

    RATE_SEM_LOCK(unit);

    /* get value from CHIP*/
    switch (bypassType)
    {
        case IGR_BYPASS_RMA:
            ret = reg_field_read(unit, CYPRESS_IGR_BWCTRL_CTRLr, CYPRESS_RMA_ADMITf, &value);
            break;
        case IGR_BYPASS_BPDU:
            ret = reg_field_read(unit, CYPRESS_IGR_BWCTRL_CTRLr, CYPRESS_BPDU_ADMITf, &value);
            break;
        case IGR_BYPASS_RTKPKT:
            ret = reg_field_read(unit, CYPRESS_IGR_BWCTRL_CTRLr, CYPRESS_RTKPKT_ADMITf, &value);
            break;
        case IGR_BYPASS_IGMP:
            ret = reg_field_read(unit, CYPRESS_IGR_BWCTRL_CTRLr, CYPRESS_IGMP_ADMITf, &value);
            break;
        case IGR_BYPASS_ARPREQ:
            ret = reg_field_read(unit, CYPRESS_IGR_BWCTRL_CTRLr, CYPRESS_ARPREQ_ADMITf, &value);
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
}

/* Function Name:
 *      dal_cypress_rate_igrBandwidthCtrlBypass_set
 * Description:
 *      Set the status of bypass ingress bandwidth control for specified packet type.
 * Input:
 *      unit         - unit id
 *      byasssType   - bypass type
 *      enable       - status of bypass ingress bandwidth control
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT                 - Invalid input parameter
 * Note:
 *      None.
 */
int32
dal_cypress_rate_igrBandwidthCtrlBypass_set(uint32 unit, rtk_rate_igr_bypassType_t bypassType, rtk_enable_t enable)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, bypassType=%d, enable=%d",
           unit, bypassType, enable);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(bypassType >= IGR_BYPASS_END, RT_ERR_INPUT);

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
        case IGR_BYPASS_RMA:
            ret = reg_field_write(unit, CYPRESS_IGR_BWCTRL_CTRLr, CYPRESS_RMA_ADMITf, &value);
            break;
        case IGR_BYPASS_BPDU:
            ret = reg_field_write(unit, CYPRESS_IGR_BWCTRL_CTRLr, CYPRESS_BPDU_ADMITf, &value);
            break;
        case IGR_BYPASS_RTKPKT:
            ret = reg_field_write(unit, CYPRESS_IGR_BWCTRL_CTRLr, CYPRESS_RTKPKT_ADMITf, &value);
            break;
        case IGR_BYPASS_IGMP:
            ret = reg_field_write(unit, CYPRESS_IGR_BWCTRL_CTRLr, CYPRESS_IGMP_ADMITf, &value);
            break;
        case IGR_BYPASS_ARPREQ:
            ret = reg_field_write(unit, CYPRESS_IGR_BWCTRL_CTRLr, CYPRESS_ARPREQ_ADMITf, &value);
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
}

/* Function Name:
 *      dal_cypress_rate_portIgrBwFlowctrlEnable_get
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
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_rate_portIgrBwFlowctrlEnable_get(
    uint32              unit,
    rtk_port_t          port,
    rtk_enable_t        *pEnable)
{
    int32   ret;
    uint32  value, reg_idx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d",
           unit, port);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);

    RATE_SEM_LOCK(unit);

    if (HWP_10GE_PORT(unit, port))
    {
        reg_idx = CYPRESS_IGR_BWCTRL_PORT_CTRL_10Gr;
        if(port == 24)
            port = 0;
        else if(port == 36)
            port = 1;
    }
    else
        reg_idx = CYPRESS_IGR_BWCTRL_PORT_CTRLr;

    /* get value from CHIP */
    if ((ret = reg_array_field_read(unit, reg_idx, port, REG_ARRAY_INDEX_NONE, CYPRESS_FC_ENf, &value)) != RT_ERR_OK)
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
}

/* Function Name:
 *      dal_cypress_rate_portIgrBwFlowctrlEnable_set
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
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None
 */
int32
dal_cypress_rate_portIgrBwFlowctrlEnable_set(
    uint32              unit,
    rtk_port_t          port,
    rtk_enable_t        enable)
{
    int32   ret;
    uint32  value, reg_idx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, enable=%d",
           unit, port, enable);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);

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

    if (HWP_10GE_PORT(unit, port))
    {
        reg_idx = CYPRESS_IGR_BWCTRL_PORT_CTRL_10Gr;
        if(port == 24)
            port = 0;
        else if(port == 36)
            port = 1;
    }
    else
        reg_idx = CYPRESS_IGR_BWCTRL_PORT_CTRLr;

    /* program value to CHIP*/
    if ((ret = reg_array_field_write(unit, reg_idx, port, REG_ARRAY_INDEX_NONE, CYPRESS_FC_ENf, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_rate_igrBandwidthLowThresh_get
 * Description:
 *      Get the flow control turn ON/OFF low threshold of ingress bandwidth.
 * Input:
 *      unit                   - unit id
 * Output:
 *      pLowThresh             - pointer to flow control turn ON/OFF low threshold
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER     - NULL pointer
 * Note:
 *      None.
 */
int32
dal_cypress_rate_igrBandwidthLowThresh_get(
    uint32          unit,
    uint32          *pLowThresh)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d",
           unit);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pLowThresh), RT_ERR_NULL_POINTER);

    RATE_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_field_read(unit, CYPRESS_IGR_BWCTRL_CTRLr, CYPRESS_LOW_THRf, pLowThresh)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "pLowThresh=%d", *pLowThresh);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_rate_igrBandwidthLowThresh_set
 * Description:
 *      Set the flow control turn ON/OFF low threshold of ingress bandwidth.
 * Input:
 *      unit         - unit id
 *      lowThresh    - flow control turn ON/OFF low threshold
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT                 - Invalid input parameter
 * Note:
 *      After setting the API, please adjust the high threshold by dal_cypress_rate_igrBwCtrlBurstSize_set.
 */
int32
dal_cypress_rate_igrBandwidthLowThresh_set(
    uint32          unit,
    uint32          lowThresh)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, lowThresh=%d",
           unit, lowThresh);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((lowThresh < HAL_THRESH_OF_IGR_BW_FLOWCTRL_MIN(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK((lowThresh > HAL_THRESH_OF_IGR_BW_FLOWCTRL_MAX(unit)), RT_ERR_INPUT);

    RATE_SEM_LOCK(unit);

    /* program value to CHIP*/
    if ((ret = reg_field_write(unit, CYPRESS_IGR_BWCTRL_CTRLr, CYPRESS_LOW_THRf, &lowThresh)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }
    if ((ret = reg_field_write(unit, CYPRESS_IGR_BWCTRL_CTRL_LB_THRr, CYPRESS_LOW_THRf, &lowThresh)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_rate_portIgrBandwidthCtrlExceed_get
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
 *      - TRUE      - flow control high on threshold ever exceeds.
 *      - FALSE     - flow control high on threshold never exceeds.
 */
int32
dal_cypress_rate_portIgrBandwidthCtrlExceed_get(
    uint32                  unit,
    rtk_port_t              port,
    uint32                  *pIsExceed)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIsExceed), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);

    RATE_SEM_LOCK(unit);

    /* read value from CHIP*/
    if ((ret = reg_array_field_read(unit, CYPRESS_IGR_BWCTRL_PORT_EXCEED_FLGr, port, REG_ARRAY_INDEX_NONE,
                    CYPRESS_EXCEED_FLGf, &value)) != RT_ERR_OK)
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
}

/* Function Name:
 *      dal_cypress_rate_portIgrBandwidthCtrlExceed_reset
 * Description:
 *      Reset exceed status of ingress bandwidth control on specified port.
 * Input:
 *      unit       - unit id
 *      port       - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_PORT_ID              - invalid port id
 * Note:
 *      None
 */
int32
dal_cypress_rate_portIgrBandwidthCtrlExceed_reset(
    uint32                  unit,
    rtk_port_t              port)
{
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);

    RATE_SEM_LOCK(unit);

    /*write 1 to clear*/
    if ((ret = reg_array_field_write1toClear(unit, CYPRESS_IGR_BWCTRL_PORT_EXCEED_FLGr, port, REG_ARRAY_INDEX_NONE,
                    CYPRESS_EXCEED_FLGf)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_rate_portIgrBandwidthHighThresh_get
 * Description:
 *      Get the flow control turn ON/OFF high threshold of ingress bandwidth for specified port.
 * Input:
 *      unit                   - unit id
 *      port                   - port id
 * Output:
 *      pHighThresh            - pointer to flow control turn ON/OFF high threshold
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID          - Invalid port id
 *      RT_ERR_NULL_POINTER     - NULL pointer
 * Note:
 *      None.
 */
int32
dal_cypress_rate_portIgrBandwidthHighThresh_get(uint32 unit, rtk_port_t port, uint32 *pHighThresh)
{
    int32   ret;
    uint32  reg_idx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pHighThresh), RT_ERR_NULL_POINTER);

    RATE_SEM_LOCK(unit);

    if (HWP_10GE_PORT(unit, port))
    {
        reg_idx = CYPRESS_IGR_BWCTRL_PORT_CTRL_10Gr;
        if(port == 24)
            port = 0;
        else if(port == 36)
            port = 1;
    }
    else
        reg_idx = CYPRESS_IGR_BWCTRL_PORT_CTRLr;

    /* get value from CHIP*/
    if ((ret = reg_array_field_read(unit, reg_idx, port, REG_ARRAY_INDEX_NONE,
                    CYPRESS_HI_THRf, pHighThresh)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }
    RATE_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "pHighThresh=%d", *pHighThresh);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_rate_portIgrBandwidthHighThresh_set
 * Description:
 *      Set the flow control turn ON/OFF high threshold of ingress bandwidth for specified port.
 * Input:
 *      unit         - unit id
 *      port         - port id
 *      highThresh   - flow control turn ON/OFF high threshold
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID               - Invalid port id
 *      RT_ERR_INPUT                 - Invalid input parameter
 * Note:
 *      The (minimum, maximum) of highThresh setting range is (22, 65535).
 */
int32
dal_cypress_rate_portIgrBandwidthHighThresh_set(uint32 unit, rtk_port_t port, uint32 highThresh)
{
    int32   ret;
    uint32  reg_idx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, highThresh=%d",
           unit, port, highThresh);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((highThresh < HAL_THRESH_OF_IGR_BW_FLOWCTRL_MIN(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK((highThresh > HAL_THRESH_OF_IGR_BW_FLOWCTRL_MAX(unit)), RT_ERR_INPUT);

    RATE_SEM_LOCK(unit);

    /* program value into CHIP*/
    if (HWP_10GE_PORT(unit, port))
    {
        reg_idx = CYPRESS_IGR_BWCTRL_PORT_CTRL_10Gr;
        if(port == 24)
            port = 0;
        else if(port == 36)
            port = 1;
    }
    else
        reg_idx = CYPRESS_IGR_BWCTRL_PORT_CTRLr;

    if ((ret = reg_array_field_write(unit, reg_idx, port, REG_ARRAY_INDEX_NONE,
                    CYPRESS_HI_THRf, &highThresh)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_rate_igrBwCtrlBurstSize_get
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
 *      None
 */
int32
dal_cypress_rate_igrBwCtrlBurstSize_get(uint32 unit, uint32 *pBurst_size)
{
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pBurst_size), RT_ERR_NULL_POINTER);

    if ((ret = dal_cypress_rate_portIgrBandwidthHighThresh_get(unit, 0, pBurst_size)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_rate_igrBwCtrlBurstSize_set
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
 *      After setting the API, please adjust the low threshold by dal_cypress_rate_igrBandwidthLowThresh_set.
 *      The burst_size unit is 128 Bytes and the maximum of burst_size setting range is under 65535.
 */
int32
dal_cypress_rate_igrBwCtrlBurstSize_set(uint32 unit, uint32 burst_size)
{
    int32   ret;
    rtk_port_t  port;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, burst_size=0x%x", unit, burst_size);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((burst_size > HAL_BURST_OF_IGR_BANDWIDTH_MAX(unit)), RT_ERR_INPUT);

    HWP_PORT_TRAVS(unit, port)
    {
        if (!HWP_ETHER_PORT(unit, port))
        {
            continue;
        }

        if ((ret = dal_cypress_rate_portIgrBandwidthHighThresh_set(unit, port, burst_size)) != RT_ERR_OK)
        {
            return ret;
        }
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_rate_portEgrBwCtrlEnable_get
 * Description:
 *      Get the egress bandwidth control status.
 * Input:
 *      unit                - unit id
 *      port                - port id
 * Output:
 *      pEnable            - status of egress bandwidth control
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID       - Invalid port id
 *      RT_ERR_NULL_POINTER  - NULL pointer
 * Note:
 *    The status of egress bandwidth control is as following:
 *    - DISABLED
 *    - ENABLED
 */
int32
dal_cypress_rate_portEgrBwCtrlEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable)
{
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d",
           unit, port);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);

    RATE_SEM_LOCK(unit);

    *pEnable = egr_bandwidth_ctrl[unit][port].status;

    RATE_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "pEnable=%d",
           *pEnable);

    return RT_ERR_OK;
} /* end of dal_cypress_rate_portEgrBwCtrlEnable_get */

/* Function Name:
 *      dal_cypress_rate_portEgrBwCtrlEnable_set
 * Description:
 *      Set the egress bandwidth control status.
 * Input:
 *      unit                - unit id
 *      port                - port id
 *      enable              - status of egress bandwidth control
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID       - Invalid port id
 *      RT_ERR_INPUT         - Invalid input parameter
 * Note:
 *    The status of egress bandwidth control is as following:
 *    - DISABLED
 *    - ENABLED
 */
int32
dal_cypress_rate_portEgrBwCtrlEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
    sched_entry_t pSchedEntry;
    uint32 maxRate;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, \
           enable=%d", unit, port, enable);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(enable >= RTK_ENABLE_END, RT_ERR_INPUT);
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);

    if (enable == egr_bandwidth_ctrl[unit][port].status)
    {
        /* same status as original, no need to process */
        return RT_ERR_OK;
    }

    RATE_SEM_LOCK(unit);

    if ((ret = _dal_cypress_rate_egrBandwidthCtrlMaxRate_get(unit, port, &maxRate)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    if (ENABLED == enable)
    {
        if (maxRate > egr_bandwidth_ctrl[unit][port].rate)
            maxRate = egr_bandwidth_ctrl[unit][port].rate;
    }
    else
    {
         /* DISABLED, rate decided by link speed */
    }

    osal_memset(&pSchedEntry, 0, sizeof(sched_entry_t));

    /* program value to CHIP */
    if ((ret = table_read(unit, CYPRESS_SCHEDt, port, (uint32 *) &pSchedEntry)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    if ((ret = table_field_set(unit, CYPRESS_SCHEDt, CYPRESS_SCHED_EGR_RATEtf,
                    &maxRate, (uint32 *) &pSchedEntry)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    if ((ret = table_write(unit, CYPRESS_SCHEDt, port, (uint32 *) &pSchedEntry)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    egr_bandwidth_ctrl[unit][port].status = enable;

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_rate_portEgrBwCtrlEnable_set */

/* Function Name:
 *      dal_cypress_rate_portEgrBwCtrlRate_get
 * Description:
 *      Get the egress bandwidth control rate.
 * Input:
 *      unit            - unit id
 *      port            - port id
 * Output:
 *      pRate          - egress bandwidth control rate
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID       - Invalid port id
 *      RT_ERR_NULL_POINTER  - NULL pointer
 * Note:
 *    The actual rate is "rate * chip granularity".
 *    The unit of granularity in RTL8389/RTL8329/RTL8390 is 16Kbps.
 */
int32
dal_cypress_rate_portEgrBwCtrlRate_get(uint32 unit, rtk_port_t port, uint32 *pRate)
{
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d",
           unit, port);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pRate), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);

    RATE_SEM_LOCK(unit);

    *pRate = egr_bandwidth_ctrl[unit][port].rate;

    RATE_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "pRate=%d", *pRate);

    return RT_ERR_OK;
} /* end of dal_cypress_rate_portEgrBwCtrlRate_get */

/* Function Name:
 *      dal_cypress_rate_portEgrBwCtrlRate_set
 * Description:
 *      Set the egress bandwidth control rate.
 * Input:
 *      unit                - unit id
 *      port                - port id
 *      rate                - egress bandwidth control rate
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID       - Invalid port id
 *      RT_ERR_RATE          - Invalid input rate
 * Note:
 *    The actual rate is "rate * chip granularity".
 *    The unit of granularity in RTL8389/RTL8329/RTL8390 is 16Kbps.
 */
int32
dal_cypress_rate_portEgrBwCtrlRate_set(uint32 unit, rtk_port_t port, uint32 rate)
{
    int32 ret;
    sched_entry_t pSchedEntry;
    rtk_qos_scheduling_type_t sched_type;
    uint32 maxRate, old_rate =0, state;
    rtk_action_t action;
    uint8 p_flag = FALSE;
    rtk_trap_oam_action_t trap_action;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, rate=%d",
           unit, port, rate);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((rate > HAL_GET_MAX_BANDWIDTH_OF_PORT(unit, port)), RT_ERR_RATE);
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);

    if (DISABLED == egr_bandwidth_ctrl[unit][port].status)
    {
        /* if disabled, only config it in database. */
        egr_bandwidth_ctrl[unit][port].rate = rate;
        return RT_ERR_OK;
    }

    /* E0014503: Get current scheduling type and rate <START> */
    if (!HWP_IS_CPU_PORT(unit, port))
    {
        if ((ret = dal_cypress_qos_schedulingAlgorithm_get(unit, port, &sched_type)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_QOS), "");
            return ret;
        }
        if ((ret = dal_cypress_rate_portEgrBwCtrlRate_get(unit, port, &old_rate)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }
    } /* E0014503: <END> */

    RATE_SEM_LOCK(unit);

    /* Force to select minimum value between LinkSpeed or Configure Rate. <START> */
    egr_bandwidth_ctrl[unit][port].rate = rate;

    if ((ret = _dal_cypress_rate_egrBandwidthCtrlMaxRate_get(unit, port, &maxRate)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }
    if (rate > maxRate)
        rate = maxRate;
    /* <END> */

    /* E0014503: Check rate change status and scheduling algorithm. Clean TX queue and leaky bucket <START> */
    if (!HWP_IS_CPU_PORT(unit, port))
    {
        if ((rate > old_rate) && (WFQ == sched_type))
            p_flag = TRUE;

        if (p_flag)
        {
            if ((ret = dal_cypress_common_portEgrQueueEmpty_start(unit, port, &state, &old_rate,
                                &trap_action, &action)) != RT_ERR_OK)
            {
                RATE_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
                return ret;
            }
        }
    } /* E0014503: <END> */

    /* program value to CHIP */
    osal_memset(&pSchedEntry, 0, sizeof(sched_entry_t));
    if ((ret = table_read(unit, CYPRESS_SCHEDt, port, (uint32 *) &pSchedEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        goto Err;
    }
    if ((ret = table_field_set(unit, CYPRESS_SCHEDt, CYPRESS_SCHED_EGR_RATEtf, &rate, (uint32 *) &pSchedEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        goto Err;
    }
    if ((ret = table_write(unit, CYPRESS_SCHEDt, port, (uint32 *) &pSchedEntry)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
    }

Err:
    /* E0014503: Restore TX queue and leaky bucket counting ability <START> */
    if (!HWP_IS_CPU_PORT(unit, port))
    {
        if (p_flag)
        {
            if ((ret = dal_cypress_common_portEgrQueueEmpty_end(unit, port, Q_EMPTY_RATE, state,
                                old_rate, trap_action, action)) != RT_ERR_OK)
            {
                RATE_SEM_UNLOCK(unit);
                RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
                return ret;
            }
        }
    } /* E0014503: <END> */

    RATE_SEM_UNLOCK(unit);

    return ret;
} /* end of dal_cypress_rate_portEgrBwCtrlRate_set */

/* Function Name:
 *      dal_cypress_rate_egrBandwidthCtrlIncludeIfg_get
 * Description:
 *      Get the status of egress bandwidth control includes IFG or not.
 * Input:
 *      unit                    - unit id
 * Output:
 *      pIfg_include           - include IFG or not
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT            - Invalid input parameter
 *      RT_ERR_NULL_POINTER     - NULL pointer
 * Note:
 *      1. Egress bandwidth control includes/excludes the Preamble & IFG (20 Bytes).
 *
 *      2. The status of ifg_include:
 *         - DISABLED
 *         - ENABLED
 */
int32
dal_cypress_rate_egrBandwidthCtrlIncludeIfg_get(uint32 unit, rtk_enable_t *pIfg_include)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIfg_include), RT_ERR_NULL_POINTER);

    RATE_SEM_LOCK(unit);

    /* get value from CHIP*/
    if ((ret = reg_field_read(unit, CYPRESS_SCHED_CTRLr, CYPRESS_INC_IFGf, &value)) != RT_ERR_OK)
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
} /* end of dal_cypress_rate_egrBandwidthCtrlIncludeIfg_get */

/* Function Name:
 *      dal_cypress_rate_egrBandwidthCtrlIncludeIfg_set
 * Description:
 *      Set the status of egress bandwidth control includes IFG or not.
 * Input:
 *      unit                    - unit id
 *      ifg_include             - include IFG or not
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT                 - Invalid input parameter
 * Note:
 *      1. Egress bandwidth control includes/excludes the Preamble & IFG (20 Bytes).
 *
 *      2. The status of ifg_include:
 *         - DISABLED
 *         - ENABLED
 */
int32
dal_cypress_rate_egrBandwidthCtrlIncludeIfg_set(uint32 unit, rtk_enable_t ifg_include)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, ifg_include=%d", unit,
           ifg_include);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(ifg_include >= RTK_ENABLE_END, RT_ERR_INPUT);

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

    /* program value into CHIP*/
    if ((ret = reg_field_write(unit, CYPRESS_SCHED_CTRLr, CYPRESS_INC_IFGf, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_rate_egrBandwidthCtrlIncludeIfg_set */

/* Function Name:
 *      dal_cypress_rate_egrPortBwCtrlBurstSize_get
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
dal_cypress_rate_egrPortBwCtrlBurstSize_get(
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
    if ((ret = reg_field_read(unit, CYPRESS_SCHED_LB_THRr, CYPRESS_WFQ_LB_SIZEf, pBurst_size)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "pBurst_size=%d", *pBurst_size);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_rate_egrPortBwCtrlBurstSize_set
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
dal_cypress_rate_egrPortBwCtrlBurstSize_set(
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
    if ((ret = reg_field_write(unit, CYPRESS_SCHED_LB_THRr, CYPRESS_WFQ_LB_SIZEf, &burst_size)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_cypress_rate_egrQueueBwCtrlBurstSize_get
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
dal_cypress_rate_egrQueueBwCtrlBurstSize_get(
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
    if ((ret = reg_field_read(unit, CYPRESS_SCHED_LB_THRr, CYPRESS_APR_LB_SIZEf, pBurst_size)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "pBurst_size=%d", *pBurst_size);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_rate_egrQueueBwCtrlBurstSize_set
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
dal_cypress_rate_egrQueueBwCtrlBurstSize_set(
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
    if ((ret = reg_field_write(unit, CYPRESS_SCHED_LB_THRr, CYPRESS_APR_LB_SIZEf, &burst_size)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_cypress_rate_cpuEgrBandwidthCtrlRateMode_get
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
dal_cypress_rate_cpuEgrBandwidthCtrlRateMode_get(uint32 unit, rtk_rate_rateMode_t *pMode)
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
    if ((ret = reg_field_read(unit, CYPRESS_SCHED_CTRLr, CYPRESS_LB_MODE_CPU_Pf, &value)) != RT_ERR_OK)
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
 *      dal_cypress_rate_cpuEgrBandwidthCtrlRateMode_set
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
dal_cypress_rate_cpuEgrBandwidthCtrlRateMode_set(uint32 unit, rtk_rate_rateMode_t mode)
{
    uint32  value;
    int32   ret;
    uint32 tick, token, burst;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, mode=%d", unit, mode);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((mode >= RATE_MODE_END), RT_ERR_INPUT);

    if (RATE_MODE_BYTE == mode)
        value = 0;
    else /* RATE_MODE_PKT == mode */
        value = 1;

    RATE_SEM_LOCK(unit);

    /* program value to CHIP */
    if ((ret = reg_field_write(unit, CYPRESS_SCHED_CTRLr, CYPRESS_LB_MODE_CPU_Pf, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    /* leaky bucket configuration for PPS mode */
    if (RATE_MODE_PKT == mode)
    {
        if (HWP_8350_FAMILY_ID(unit)) /* 50MHz */
        {
            tick = 763;
            token = 1;
            burst = 5;
        }
        else /* 250MHz */
        {
            tick = 3815;
            token = 1;
            burst = 5;
        }

        if ((ret = reg_field_write(unit, CYPRESS_SCHED_LB_TICK_TKN_PPS_CTRLr, CYPRESS_TICK_PERIOD_PPSf, &tick)) != RT_ERR_OK)
        {
            RATE_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }
        if ((ret = reg_field_write(unit, CYPRESS_SCHED_LB_TICK_TKN_PPS_CTRLr, CYPRESS_TKN_PPSf, &token)) != RT_ERR_OK)
        {
            RATE_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }

        /* burst size */
        if ((ret = reg_field_write(unit, CYPRESS_SCHED_PPS_CTRLr, CYPRESS_APR_LB_SIZE_PPSf, &burst)) != RT_ERR_OK)
        {
            RATE_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }
        if ((ret = reg_field_write(unit, CYPRESS_SCHED_PPS_CTRLr, CYPRESS_WFQ_LB_SIZE_PPSf, &burst)) != RT_ERR_OK)
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
 *      dal_cypress_rate_portEgrQueueBwCtrlEnable_get
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
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_QUEUE_ID         - invalid queue id
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_rate_portEgrQueueBwCtrlEnable_get(
    uint32          unit,
    rtk_port_t      port,
    rtk_qid_t       queue,
    rtk_enable_t    *pEnable)
{
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, queue=%d",
           unit, port, queue);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((queue >= HAL_MAX_NUM_OF_QUEUE(unit)), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);

    RATE_SEM_LOCK(unit);

    *pEnable = egr_queue_bandwidth_ctrl[unit][port][queue].status;

    RATE_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "pEnable=%d",
           *pEnable);

    return RT_ERR_OK;
} /* end of dal_cypress_rate_portportEgrQueueBwCtrlEnable_get */

/* Function Name:
 *      dal_cypress_rate_portEgrQueueBwCtrlEnable_set
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
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_QUEUE_ID         - invalid queue id
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None
 */
int32
dal_cypress_rate_portEgrQueueBwCtrlEnable_set(
    uint32          unit,
    rtk_port_t      port,
    rtk_qid_t       queue,
    rtk_enable_t    enable)
{
    int32   ret;
    uint32  rate;
    sched_entry_t pSchedEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, queue=%d, enable=%d",
           unit, port, queue, enable);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(enable >= RTK_ENABLE_END, RT_ERR_INPUT);
    RT_PARAM_CHK((queue >= HAL_MAX_NUM_OF_QUEUE(unit)), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);

    if (enable == egr_queue_bandwidth_ctrl[unit][port][queue].status)
    {
        /* same status as original, no need to process */
        return RT_ERR_OK;
    }

    /* translate to chip value */
    if (!HWP_GE_PORT(unit, port))
    {
        switch (enable)
        {
            case DISABLED:
                rate = DAL_CYPRESS_RATE_DISABLED_EGR_BANDWIDTH_RATE_10GE_PORT;
                break;
            case ENABLED:
                rate = egr_queue_bandwidth_ctrl[unit][port][queue].rate;
                break;
            default:
                return RT_ERR_INPUT;
        }
    }
    else
    {
        switch (enable)
        {
            case DISABLED:
                rate = DAL_CYPRESS_RATE_DISABLED_EGR_BANDWIDTH_RATE_GE_PORT;
                break;
            case ENABLED:
                rate = egr_queue_bandwidth_ctrl[unit][port][queue].rate;
                break;
            default:
                return RT_ERR_INPUT;
        }
    }

    RATE_SEM_LOCK(unit);

    osal_memset(&pSchedEntry, 0, sizeof(sched_entry_t));

    /* program value to CHIP */
    if ((ret = table_read(unit, CYPRESS_SCHEDt, port, (uint32 *) &pSchedEntry)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    if ((ret = table_field_set(unit, CYPRESS_SCHEDt, egrBandwidth_queue_rate_fieldidx[queue],
                    &rate, (uint32 *) &pSchedEntry)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    if ((ret = table_write(unit, CYPRESS_SCHEDt, port, (uint32 *) &pSchedEntry)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    egr_queue_bandwidth_ctrl[unit][port][queue].status = enable;

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_rate_portportEgrQueueBwCtrlEnable_set */

/* Function Name:
 *      dal_cypress_rate_portEgrQueueBwCtrlRate_get
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
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_QUEUE_ID         - invalid queue id
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *    The actual rate is "rate * chip granularity".
 *    The unit of granularity is 16Kbps.
 */
int32
dal_cypress_rate_portEgrQueueBwCtrlRate_get(
    uint32      unit,
    rtk_port_t  port,
    rtk_qid_t   queue,
    uint32      *pRate)
{
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d",
           unit, port);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pRate), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((queue >= HAL_MAX_NUM_OF_QUEUE(unit)), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);

    RATE_SEM_LOCK(unit);

    *pRate = egr_queue_bandwidth_ctrl[unit][port][queue].rate;

    RATE_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "pRate=%d", *pRate);

    return RT_ERR_OK;
} /* end of dal_cypress_rate_portEgrQueueBwCtrlRate_get */

/* Function Name:
 *      dal_cypress_rate_portEgrQueueBwCtrlRate_set
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
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_QUEUE_ID         - invalid queue id
 *      RT_ERR_RATE             - Invalid input rate
 * Note:
 *    The actual rate is "rate * chip granularity".
 *    The unit of granularity is 16Kbps.
 */
int32
dal_cypress_rate_portEgrQueueBwCtrlRate_set(
    uint32      unit,
    rtk_port_t  port,
    rtk_qid_t   queue,
    uint32      rate)
{
    int32 ret;
    sched_entry_t pSchedEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, rate=%d",
           unit, port, rate);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((rate > HAL_GET_MAX_BANDWIDTH_OF_PORT(unit, port)), RT_ERR_RATE);
    RT_PARAM_CHK((queue >= HAL_MAX_NUM_OF_QUEUE(unit)), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);

    if (DISABLED == egr_queue_bandwidth_ctrl[unit][port][queue].status)
    {
        /* if disabled, only config it in database. */
        egr_queue_bandwidth_ctrl[unit][port][queue].rate = rate;
        return RT_ERR_OK;
    }

    RATE_SEM_LOCK(unit);

    /* program value to CHIP */
    if ((ret = table_read(unit, CYPRESS_SCHEDt, port, (uint32 *) &pSchedEntry)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    if ((ret = table_field_set(unit, CYPRESS_SCHEDt, egrBandwidth_queue_rate_fieldidx[queue],
                    &rate, (uint32 *) &pSchedEntry)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    if ((ret = table_write(unit, CYPRESS_SCHEDt, port, (uint32 *) &pSchedEntry)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    egr_queue_bandwidth_ctrl[unit][port][queue].rate = rate;

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_rate_portEgrQueueBwCtrlRate_set */

/* Function Name:
 *      dal_cypress_rate_egrQueueFixedBandwidthEnable_get
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
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_QUEUE_ID     - invalid queue id
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      None
 */
int32
dal_cypress_rate_egrQueueFixedBandwidthEnable_get(
    uint32          unit,
    rtk_port_t      port,
    rtk_qid_t       queue,
    rtk_enable_t    *pEnable)
{
    int32       ret;
    rtk_enable_t    fixEbl;
    sched_entry_t pSchedEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((queue >= HAL_MAX_NUM_OF_QUEUE(unit)), RT_ERR_QUEUE_ID);

    RATE_SEM_LOCK(unit);

    osal_memset(&pSchedEntry, 0, sizeof(sched_entry_t));

    /* get value from CHIP */
    if ((ret = table_read(unit, CYPRESS_SCHEDt, port, (uint32 *) &pSchedEntry)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    if ((ret = table_field_get(unit, CYPRESS_SCHEDt, (uint32)egrBandwidth_queue_fix_bw_fieldidx[queue],
                    &fixEbl, (uint32 *) &pSchedEntry)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    if (ENABLED == fixEbl)
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;

    RATE_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "*pEnable=%d", *pEnable);

    return RT_ERR_OK;
} /* end of dal_cypress_rate_egrQueueFixedBandwidthEnable_get */

/* Function Name:
 *      dal_cypress_rate_egrQueueFixedBandwidthEnable_set
 * Description:
 *      Set enable status of fixed bandwidth ability on specified queue.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      queue   - queue id
 *      enable  - enable status of fixed bandwidth ability
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_QUEUE_ID     - invalid queue id
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      None
 */
int32
dal_cypress_rate_egrQueueFixedBandwidthEnable_set(
    uint32          unit,
    rtk_port_t      port,
    rtk_qid_t       queue,
    rtk_enable_t    enable)
{
    int32       ret;
    rtk_enable_t    fixEbl;
    sched_entry_t pSchedEntry;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d", unit, port);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_PORT_EXIST(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((queue >= HAL_MAX_NUM_OF_QUEUE(unit)), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK(enable >= RTK_ENABLE_END, RT_ERR_INPUT);

    /* Display debug message */
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "enable=%d", enable);

    RATE_SEM_LOCK(unit);

    osal_memset(&pSchedEntry, 0, sizeof(sched_entry_t));

    if ((ret = table_read(unit, CYPRESS_SCHEDt, port, (uint32 *) &pSchedEntry)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    if (ENABLED == enable)
        fixEbl = ENABLED;
    else
        fixEbl = DISABLED;

    if ((ret = table_field_set(unit, CYPRESS_SCHEDt, (uint32)egrBandwidth_queue_fix_bw_fieldidx[queue],
                    &fixEbl, (uint32 *) &pSchedEntry)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    if ((ret = table_write(unit, CYPRESS_SCHEDt, port, (uint32 *) &pSchedEntry)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;

} /* end of dal_cypress_rate_egrQueueFixedBandwidthEnable_set */

/* Function Name:
 *      dal_cypress_rate_portStormCtrlEnable_get
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
dal_cypress_rate_portStormCtrlEnable_get(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_group_t  storm_type,
    rtk_enable_t            *pEnable)
{
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, storm_type=%d", unit, port, storm_type);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((storm_type >= STORM_GROUP_END), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    RATE_SEM_LOCK(unit);
    (*pEnable) = pRate_info[unit]->stormControl_enable[port][storm_type];
    RATE_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L2), "pEnable=%d", *pEnable);

    return RT_ERR_OK;
} /* end of dal_cypress_rate_portStormCtrlEnable_get */

/* Function Name:
 *      dal_cypress_rate_portStormCtrlEnable_set
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
dal_cypress_rate_portStormCtrlEnable_set(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_group_t  storm_type,
    rtk_enable_t            enable)
{
    uint32  val, tmp_port;
    uint32  reg_idx, field_idx;
    int32   ret;

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, storm_type=%d, enable=%u"
            , unit, port, storm_type, enable);

    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((storm_type >= STORM_GROUP_END), RT_ERR_INPUT);
    RT_PARAM_CHK((ENABLED != enable && DISABLED != enable), RT_ERR_INPUT);

    switch (storm_type)
    {
        case STORM_GROUP_UNICAST:
            if (HWP_10GE_PORT(unit, port))
                reg_idx = CYPRESS_STORM_CTRL_PORT_UC_RATE_10Gr;
            else
                reg_idx = CYPRESS_STORM_CTRL_PORT_UCr;
            field_idx = CYPRESS_UC_RATEf;
            break;
        case STORM_GROUP_MULTICAST:
            if (HWP_10GE_PORT(unit, port))
                reg_idx = CYPRESS_STORM_CTRL_PORT_MC_RATE_10Gr;
            else
                reg_idx = CYPRESS_STORM_CTRL_PORT_MCr;
            field_idx = CYPRESS_MC_RATEf;
            break;
        case STORM_GROUP_BROADCAST:
            if (HWP_10GE_PORT(unit, port))
                reg_idx = CYPRESS_STORM_CTRL_PORT_BC_RATE_10Gr;
            else
                reg_idx = CYPRESS_STORM_CTRL_PORT_BCr;
            field_idx = CYPRESS_BC_RATEf;
            break;
        default:
            return RT_ERR_INPUT;
    }

    RATE_SEM_LOCK(unit);

    tmp_port = port;
    if (HWP_10GE_PORT(unit, port)) /*10G port 24 and 36 map to index 0 and 1*/
    {
        if(port == 24)
            tmp_port = 0;
        else if(port == 36)
            tmp_port = 1;
    }

    if (ENABLED == enable)
        val = pRate_info[unit]->stormControl_rate[port][storm_type];
    else
    {
        if ((ret = reg_array_field_read(unit, reg_idx, tmp_port, REG_ARRAY_INDEX_NONE, field_idx, &val)) != RT_ERR_OK)
        {
            RATE_SEM_UNLOCK(unit);
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }
        pRate_info[unit]->stormControl_rate[port][storm_type] = val;
        val = 0xFFFFF;
    }

    /* program value to CHIP*/
    if ((ret = reg_array_field_write(unit, reg_idx, tmp_port, REG_ARRAY_INDEX_NONE, field_idx, &val)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    pRate_info[unit]->stormControl_enable[port][storm_type] = enable;
    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_cypress_rate_portStormCtrlEnable_set */

/* Function Name:
 *      dal_cypress_rate_portStormCtrlRate_get
 * Description:
 *      Get the storm control rate.
 * Input:
 *      unit            - unit id
 *      port            - port id
 *      storm_type      - storm group type
 * Output:
 *      pRate          - storm control rate (packet-per-second).
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID       - Invalid port id
 *      RT_ERR_INPUT         - Invalid input parameter
 *      RT_ERR_NULL_POINTER  - NULL pointer
 * Note:
 *      The storm group types are as following:
 *      - STORM_GROUP_MULTICAST
 *      - STORM_GROUP_BROADCAST
 *      - STORM_GROUP_UNICAST
 */
int32
dal_cypress_rate_portStormCtrlRate_get(
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
    RT_PARAM_CHK((NULL == pRate), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((storm_type >= STORM_GROUP_END), RT_ERR_INPUT);
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);

    switch (storm_type)
    {
        case STORM_GROUP_UNICAST:
            if (HWP_10GE_PORT(unit, port))
                reg_idx = CYPRESS_STORM_CTRL_PORT_UC_RATE_10Gr;
            else
                reg_idx = CYPRESS_STORM_CTRL_PORT_UCr;
            field_idx = CYPRESS_UC_RATEf;
            break;
        case STORM_GROUP_MULTICAST:
            if (HWP_10GE_PORT(unit, port))
                reg_idx = CYPRESS_STORM_CTRL_PORT_MC_RATE_10Gr;
            else
                reg_idx = CYPRESS_STORM_CTRL_PORT_MCr;
            field_idx = CYPRESS_MC_RATEf;
            break;
        case STORM_GROUP_BROADCAST:
            if (HWP_10GE_PORT(unit, port))
                reg_idx = CYPRESS_STORM_CTRL_PORT_BC_RATE_10Gr;
            else
                reg_idx = CYPRESS_STORM_CTRL_PORT_BCr;
            field_idx = CYPRESS_BC_RATEf;
            break;
        default:
            return RT_ERR_INPUT;
    }

    RATE_SEM_LOCK(unit);

    if (HWP_10GE_PORT(unit, port)) /*10G port 24 and 36 map to index 0 and 1*/
    {
        if(port == 24)
            port = 0;
        else if(port == 36)
            port = 1;
    }

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
}

/* Function Name:
 *      dal_cypress_rate_portStormCtrlRate_set
 * Description:
 *      Set the storm control rate.
 * Input:
 *      unit            - unit id
 *      port            - port id
 *      storm_type      - storm group type
 *      rate            - storm control rate (packet-per-second).
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID       - Invalid port id
 *      RT_ERR_INPUT         - Invalid input parameter
 *      RT_ERR_RATE          - Invalid input bandwidth
 * Note:
 *      The storm group types are as following:
 *      - STORM_GROUP_MULTICAST
 *      - STORM_GROUP_BROADCAST
 *      - STORM_GROUP_UNICAST
 */
int32
dal_cypress_rate_portStormCtrlRate_set(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_group_t  storm_type,
    uint32                  rate)
{
    int32   ret;
    uint32  reg_idx;
    uint32  field_idx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, storm_type=%d\
           rate=%d", unit, port, storm_type, rate);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((rate > HAL_RATE_OF_STORM_CONTROL_MAX(unit)), RT_ERR_RATE);
    RT_PARAM_CHK((storm_type >= STORM_GROUP_END), RT_ERR_INPUT);
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);

    switch (storm_type)
    {
        case STORM_GROUP_UNICAST:
            if (HWP_10GE_PORT(unit, port))
                reg_idx = CYPRESS_STORM_CTRL_PORT_UC_RATE_10Gr;
            else
                reg_idx = CYPRESS_STORM_CTRL_PORT_UCr;
            field_idx = CYPRESS_UC_RATEf;
            break;
        case STORM_GROUP_MULTICAST:
            if (HWP_10GE_PORT(unit, port))
                reg_idx = CYPRESS_STORM_CTRL_PORT_MC_RATE_10Gr;
            else
                reg_idx = CYPRESS_STORM_CTRL_PORT_MCr;
            field_idx = CYPRESS_MC_RATEf;
            break;
        case STORM_GROUP_BROADCAST:
            if (HWP_10GE_PORT(unit, port))
                reg_idx = CYPRESS_STORM_CTRL_PORT_BC_RATE_10Gr;
            else
                reg_idx = CYPRESS_STORM_CTRL_PORT_BCr;
            field_idx = CYPRESS_BC_RATEf;
            break;
        default:
            return RT_ERR_INPUT;
    }

    RATE_SEM_LOCK(unit);

    if (HWP_10GE_PORT(unit, port)) /*10G port 24 and 36 map to index 0 and 1*/
    {
        if(port == 24)
            port = 0;
        else if(port == 36)
            port = 1;
    }

    /* program value to CHIP*/
    if ((ret = reg_array_field_write(unit, reg_idx, port, REG_ARRAY_INDEX_NONE, field_idx, &rate)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_rate_portStormCtrlBurstSize_get
 * Description:
 *      Get burst size of storm control on specified port.
 * Input:
 *      unit        - unit id
 *      port        - port id
 *      storm_type  - storm group type
 * Output:
 *      pBurst_rate - pointer to burst rate
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_INPUT        - Invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The storm group types are as following:
 *      - STORM_GROUP_MULTICAST
 *      - STORM_GROUP_BROADCAST
 *      - STORM_GROUP_UNICAST
 */
int32
dal_cypress_rate_portStormCtrlBurstSize_get(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_group_t  storm_type,
    uint32                  *pBurst_rate)
{
    int32   ret;
    uint32  reg_idx;
    uint32  field_idx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, storm_type=%d",
           unit, port, storm_type);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pBurst_rate), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((storm_type >= STORM_GROUP_END), RT_ERR_INPUT);
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);

    switch (storm_type)
    {
        case STORM_GROUP_UNICAST:
            if (HWP_10GE_PORT(unit, port))
                reg_idx = CYPRESS_STORM_CTRL_PORT_UC_BURST_10Gr;
            else
                reg_idx = CYPRESS_STORM_CTRL_PORT_UCr;
            field_idx = CYPRESS_UC_BURSTf;
            break;
        case STORM_GROUP_MULTICAST:
            if (HWP_10GE_PORT(unit, port))
                reg_idx = CYPRESS_STORM_CTRL_PORT_MC_BURST_10Gr;
            else
                reg_idx = CYPRESS_STORM_CTRL_PORT_MCr;
            field_idx = CYPRESS_MC_BURSTf;
            break;
        case STORM_GROUP_BROADCAST:
            if (HWP_10GE_PORT(unit, port))
                reg_idx = CYPRESS_STORM_CTRL_PORT_BC_BURST_10Gr;
            else
                reg_idx = CYPRESS_STORM_CTRL_PORT_BCr;
            field_idx = CYPRESS_BC_BURSTf;
            break;
        default:
            return RT_ERR_INPUT;
    }

    RATE_SEM_LOCK(unit);

    if (HWP_10GE_PORT(unit, port)) /*10G port 24 and 36 map to index 0 and 1*/
    {
        if(port == 24)
            port = 0;
        else if(port == 36)
            port = 1;
    }

    /* read value from CHIP*/
    if ((ret = reg_array_field_read(unit, reg_idx, port, REG_ARRAY_INDEX_NONE, field_idx, pBurst_rate)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "pBurst_rate=%d", *pBurst_rate);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_rate_portStormCtrlBurstSize_set
 * Description:
 *      Set burst size of storm control on specified port.
 * Input:
 *      unit       - unit id
 *      port       - port id
 *      storm_type - storm group type
 *      burst_rate - burst rate
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - Invalid input parameter
 * Note:
 *      The storm group types are as following:
 *      - STORM_GROUP_MULTICAST
 *      - STORM_GROUP_BROADCAST
 *      - STORM_GROUP_UNICAST
 *      The (minimum, maximum) of burst_rate setting range of normal port is (1700, 65535).
 *      The (minimum, maximum) of burst_rate setting range of 10G port is (2650, 1048575).
 */
int32
dal_cypress_rate_portStormCtrlBurstSize_set(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_group_t  storm_type,
    uint32                  burst_rate)
{
    int32   ret;
    uint32  reg_idx;
    uint32  field_idx;
    rtk_rate_storm_rateMode_t mode;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, storm_type=%d, burst_rate=%d",
           unit, port, storm_type, burst_rate);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((storm_type >= STORM_GROUP_END), RT_ERR_INPUT);
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);
    if (HWP_10GE_PORT(unit, port))
        RT_PARAM_CHK((burst_rate > HAL_BURST_RATE_OF_10G_STORM_CONTROL_MAX(unit)), RT_ERR_INPUT);
    else
        RT_PARAM_CHK((burst_rate > HAL_BURST_RATE_OF_STORM_CONTROL_MAX(unit)), RT_ERR_INPUT);

    if ((ret = dal_cypress_rate_stormControlRateMode_get(unit, &mode)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }
    if (BASED_ON_BYTE == mode)
    {
        if (HWP_10GE_PORT(unit, port))
            RT_PARAM_CHK((burst_rate < HAL_BURST_RATE_OF_10G_STORM_CONTROL_MIN(unit)), RT_ERR_INPUT);
        else
            RT_PARAM_CHK((burst_rate < HAL_BURST_RATE_OF_STORM_CONTROL_MIN(unit)), RT_ERR_INPUT);
    }

    /* translate to chip value */
    switch (storm_type)
    {
        case STORM_GROUP_UNICAST:
            if (HWP_10GE_PORT(unit, port))
                reg_idx = CYPRESS_STORM_CTRL_PORT_UC_BURST_10Gr;
            else
                reg_idx = CYPRESS_STORM_CTRL_PORT_UCr;
            field_idx = CYPRESS_UC_BURSTf;
            break;
        case STORM_GROUP_MULTICAST:
            if (HWP_10GE_PORT(unit, port))
                reg_idx = CYPRESS_STORM_CTRL_PORT_MC_BURST_10Gr;
            else
                reg_idx = CYPRESS_STORM_CTRL_PORT_MCr;
            field_idx = CYPRESS_MC_BURSTf;
            break;
        case STORM_GROUP_BROADCAST:
            if (HWP_10GE_PORT(unit, port))
                reg_idx = CYPRESS_STORM_CTRL_PORT_BC_BURST_10Gr;
            else
                reg_idx = CYPRESS_STORM_CTRL_PORT_BCr;
            field_idx = CYPRESS_BC_BURSTf;
            break;
        default:
            return RT_ERR_INPUT;
    }

    RATE_SEM_LOCK(unit);

    if (HWP_10GE_PORT(unit, port)) /*10G port 24 and 36 map to index 0 and 1*/
    {
        if(port == 24)
            port = 0;
        else if(port == 36)
            port = 1;
    }

    /* program value to CHIP*/
    if ((ret = reg_array_field_write(unit, reg_idx, port, REG_ARRAY_INDEX_NONE, field_idx, &burst_rate)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_rate_portStormCtrlTypeSel_get
 * Description:
 *      Get the storm control type.
 * Input:
 *      unit            - unit id
 *      port            - port id
 *      storm_type      - storm group type
 * Output:
 *      pStorm_sel      - storm selection
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID       - Invalid port id
 *      RT_ERR_INPUT         - Invalid input parameter
 *      RT_ERR_NULL_POINTER  - NULL pointer
 * Note:
 *    The API is only supported in unicast and multicast, the storm group types are as following:
 *    - STORM_GROUP_UNICAST
 *    - STORM_GROUP_MULTICAST
 *
 *    The storm selection are as following:
 *    - STORM_SEL_UNKNOWN
 *    - STORM_SEL_UNKNOWN_AND_KNOWN
 */
int32
dal_cypress_rate_portStormCtrlTypeSel_get(
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
    RT_PARAM_CHK((NULL == pStorm_sel), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((storm_type >= STORM_GROUP_END), RT_ERR_INPUT);
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);

    switch (storm_type)
    {
        case STORM_GROUP_UNICAST:
            if (HWP_10GE_PORT(unit, port))
                reg_idx = CYPRESS_STORM_CTRL_PORT_UC_TYPE_10Gr;
            else
                reg_idx = CYPRESS_STORM_CTRL_PORT_UCr;
            field_idx = CYPRESS_UC_TYPEf;
            break;
        case STORM_GROUP_MULTICAST:
            if (HWP_10GE_PORT(unit, port))
                reg_idx = CYPRESS_STORM_CTRL_PORT_MC_TYPE_10Gr;
            else
                reg_idx = CYPRESS_STORM_CTRL_PORT_MCr;
            field_idx = CYPRESS_MC_TYPEf;
            break;
        default:
            return RT_ERR_INPUT;
    }

    RATE_SEM_LOCK(unit);
    if (HWP_10GE_PORT(unit, port)) /*10G port 24 and 36 map to index 0 and 1*/
    {
        if(port == 24)
            port = 0;
        else if(port == 36)
            port = 1;
    }

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
}

/* Function Name:
 *      dal_cypress_rate_portStormCtrlTypeSel_set
 * Description:
 *      Set the storm control type.
 * Input:
 *      unit            - unit id
 *      port            - port id
 *      storm_type      - storm group type
 *      storm_sel       - storm selection
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID       - Invalid port id
 *      RT_ERR_INPUT         - Invalid input parameter
 * Note:
 *    The API is only supported in unicast and multicast, the storm group types are as following:
 *    - STORM_GROUP_UNICAST
 *    - STORM_GROUP_MULTICAST
 *
 *    The storm selection are as following:
 *    - STORM_SEL_UNKNOWN
 *    - STORM_SEL_UNKNOWN_AND_KNOWN
 */
int32
dal_cypress_rate_portStormCtrlTypeSel_set(
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
    RT_PARAM_CHK((storm_sel >= STORM_SEL_END), RT_ERR_INPUT);
    RT_PARAM_CHK((storm_type >= STORM_GROUP_END), RT_ERR_INPUT);
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);

    switch (storm_type)
    {
        case STORM_GROUP_UNICAST:
            if (HWP_10GE_PORT(unit, port))
                reg_idx = CYPRESS_STORM_CTRL_PORT_UC_TYPE_10Gr;
            else
                reg_idx = CYPRESS_STORM_CTRL_PORT_UCr;
            field_idx = CYPRESS_UC_TYPEf;
            break;
        case STORM_GROUP_MULTICAST:
            if (HWP_10GE_PORT(unit, port))
                reg_idx = CYPRESS_STORM_CTRL_PORT_MC_TYPE_10Gr;
            else
                reg_idx = CYPRESS_STORM_CTRL_PORT_MCr;
            field_idx = CYPRESS_MC_TYPEf;
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
    if (HWP_10GE_PORT(unit, port)) /*10G port 24 and 36 map to index 0 and 1*/
    {
        if(port == 24)
            port = 0;
        else if(port == 36)
            port = 1;
    }
    /* program value to CHIP*/
    if ((ret = reg_array_field_write(unit, reg_idx, port, REG_ARRAY_INDEX_NONE, field_idx, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_rate_portStormCtrlExceed_get
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
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_PORT_ID              - invalid port id
 *      RT_ERR_INPUT                - Invalid input parameter
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 *      Exceed status is as following
 *      - TRUE      - storm rate is more than configured rate.
 *      - FALSE     - storm rate is never over then configured rate.
 */
int32
dal_cypress_rate_portStormCtrlExceed_get(
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
    RT_PARAM_CHK((NULL == pIsExceed), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((storm_type >= STORM_GROUP_END), RT_ERR_INPUT);
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);

    switch (storm_type)
    {
        case STORM_GROUP_UNICAST:
            reg_idx = CYPRESS_STORM_CTRL_PORT_UC_EXCEED_FLGr;
            field_idx = CYPRESS_UC_EXCEED_FLGf;
            break;
        case STORM_GROUP_MULTICAST:
            reg_idx = CYPRESS_STORM_CTRL_PORT_MC_EXCEED_FLGr;
            field_idx = CYPRESS_MC_EXCEED_FLGf;
            break;
        case STORM_GROUP_BROADCAST:
            reg_idx = CYPRESS_STORM_CTRL_PORT_BC_EXCEED_FLGr;
            field_idx = CYPRESS_BC_EXCEED_FLGf;
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
}

/* Function Name:
 *      dal_cypress_rate_portStormCtrlExceed_reset
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
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_PORT_ID              - invalid port id
 *      RT_ERR_INPUT                - Invalid input parameter
 * Note:
 *      None
 */
int32
dal_cypress_rate_portStormCtrlExceed_reset(
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
    RT_PARAM_CHK((storm_type >= STORM_GROUP_END), RT_ERR_INPUT);
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);

    switch (storm_type)
    {
        case STORM_GROUP_UNICAST:
            reg_idx = CYPRESS_STORM_CTRL_PORT_UC_EXCEED_FLGr;
            field_idx = CYPRESS_UC_EXCEED_FLGf;
            break;
        case STORM_GROUP_MULTICAST:
            reg_idx = CYPRESS_STORM_CTRL_PORT_MC_EXCEED_FLGr;
            field_idx = CYPRESS_MC_EXCEED_FLGf;
            break;
        case STORM_GROUP_BROADCAST:
            reg_idx = CYPRESS_STORM_CTRL_PORT_BC_EXCEED_FLGr;
            field_idx = CYPRESS_BC_EXCEED_FLGf;
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
}

/* Function Name:
 *      dal_cypress_rate_portStormCtrlProtoRate_get
 * Description:
 *      Get the storm control rate.
 * Input:
 *      unit            - unit id
 *      port            - port id
 *      storm_type      - storm group type
 * Output:
 *      pRate          - storm control rate (packet-per-second).
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID       - Invalid port id
 *      RT_ERR_INPUT         - Invalid input parameter
 *      RT_ERR_NULL_POINTER  - NULL pointer
 * Note:
 *    The storm group types are as following:
 *    - STORM_PROTO_GROUP_BPDU
 *    - STORM_PROTO_GROUP_IGMP
 *    - STORM_PROTO_GROUP_ARP
 */
int32
dal_cypress_rate_portStormCtrlProtoRate_get(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_proto_group_t  storm_type,
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
    RT_PARAM_CHK((NULL == pRate), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((storm_type >= STORM_PROTO_GROUP_END), RT_ERR_INPUT);
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);

    switch (storm_type)
    {
        case STORM_PROTO_GROUP_BPDU:
            reg_idx = CYPRESS_STORM_CTRL_SPCL_PORT_RATEr;
            field_idx = CYPRESS_BPDU_RATEf;
            break;
        case STORM_PROTO_GROUP_IGMP:
            reg_idx = CYPRESS_STORM_CTRL_SPCL_PORT_RATEr;
            field_idx = CYPRESS_IGMP_RATEf;
            break;
        case STORM_PROTO_GROUP_ARP:
            reg_idx = CYPRESS_STORM_CTRL_SPCL_PORT_RATEr;
            field_idx = CYPRESS_ARP_RATEf;
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
}

/* Function Name:
 *      dal_cypress_rate_portStormCtrlProtoRate_set
 * Description:
 *      Set the storm control rate.
 * Input:
 *      unit            - unit id
 *      port            - port id
 *      storm_type      - storm group type
 *      rate            - storm control rate (packet-per-second).
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID       - Invalid port id
 *      RT_ERR_INPUT         - Invalid input parameter
 *      RT_ERR_RATE          - Invalid input bandwidth
 * Note:
 *    The storm group types are as following:
 *    - STORM_PROTO_GROUP_BPDU
 *    - STORM_PROTO_GROUP_IGMP
 *    - STORM_PROTO_GROUP_ARP
 */
int32
dal_cypress_rate_portStormCtrlProtoRate_set(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_proto_group_t  storm_type,
    uint32                  rate)
{
    int32   ret;
    uint32  reg_idx;
    uint32  field_idx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, storm_type=%d\
           rate=%d", unit, port, storm_type, rate);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((rate > HAL_RATE_OF_STORM_PROTO_CONTROL_MAX(unit)), RT_ERR_RATE);
    RT_PARAM_CHK((storm_type >= STORM_PROTO_GROUP_END), RT_ERR_INPUT);
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);

    switch (storm_type)
    {
        case STORM_PROTO_GROUP_BPDU:
            reg_idx = CYPRESS_STORM_CTRL_SPCL_PORT_RATEr;
            field_idx = CYPRESS_BPDU_RATEf;
            break;
        case STORM_PROTO_GROUP_IGMP:
            reg_idx = CYPRESS_STORM_CTRL_SPCL_PORT_RATEr;
            field_idx = CYPRESS_IGMP_RATEf;
            break;
        case STORM_PROTO_GROUP_ARP:
            reg_idx = CYPRESS_STORM_CTRL_SPCL_PORT_RATEr;
            field_idx = CYPRESS_ARP_RATEf;
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
}

/* Function Name:
 *      dal_cypress_rate_portStormCtrlProtoExceed_get
 * Description:
 *      Get exceed status of storm protocol control on specified port.
 * Input:
 *      unit       - unit id
 *      port       - port id
 *      storm_type - storm group type
 * Output:
 *      pIsExceed  - pointer to exceed status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_PORT_ID              - invalid port id
 *      RT_ERR_INPUT                - Invalid input parameter
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 *      Exceed status is as following
 *      - TRUE      - storm rate is more than configured rate.
 *      - FALSE     - storm rate is never over then configured rate.
 */
int32
dal_cypress_rate_portStormCtrlProtoExceed_get(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_proto_group_t  storm_type,
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
    RT_PARAM_CHK((NULL == pIsExceed), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((storm_type >= STORM_PROTO_GROUP_END), RT_ERR_INPUT);
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);

    switch (storm_type)
    {
        case STORM_PROTO_GROUP_BPDU:
            reg_idx = CYPRESS_STORM_CTRL_SPCL_PORT_BPDU_EXCEED_FLGr;
            field_idx = CYPRESS_BPDU_EXCEED_FLGf;
            break;
        case STORM_PROTO_GROUP_IGMP:
            reg_idx = CYPRESS_STORM_CTRL_SPCL_PORT_IGMP_EXCEED_FLGr;
            field_idx = CYPRESS_IGMP_EXCEED_FLGf;
            break;
        case STORM_PROTO_GROUP_ARP:
            reg_idx = CYPRESS_STORM_CTRL_SPCL_PORT_ARP_EXCEED_FLGr;
            field_idx = CYPRESS_ARP_EXCEED_FLGf;
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
}

/* Function Name:
 *      dal_cypress_rate_portStormCtrlProtoExceed_reset
 * Description:
 *      Reset exceed status of storm protocol control on specified port.
 * Input:
 *      unit       - unit id
 *      port       - port id
 *      storm_type - storm group type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_PORT_ID              - invalid port id
 *      RT_ERR_INPUT                - Invalid input parameter
 * Note:
 *      None
 */
int32
dal_cypress_rate_portStormCtrlProtoExceed_reset(
    uint32                  unit,
    rtk_port_t              port,
    rtk_rate_storm_proto_group_t  storm_type)
{
    int32   ret;
    uint32  reg_idx;
    uint32  field_idx;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, port=%d, storm_type=%d",
           unit, port, storm_type);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((storm_type >= STORM_PROTO_GROUP_END), RT_ERR_INPUT);
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);

    switch (storm_type)
    {
        case STORM_PROTO_GROUP_BPDU:
            reg_idx = CYPRESS_STORM_CTRL_SPCL_PORT_BPDU_EXCEED_FLGr;
            field_idx = CYPRESS_BPDU_EXCEED_FLGf;
            break;
        case STORM_PROTO_GROUP_IGMP:
            reg_idx = CYPRESS_STORM_CTRL_SPCL_PORT_IGMP_EXCEED_FLGr;
            field_idx = CYPRESS_IGMP_EXCEED_FLGf;
            break;
        case STORM_PROTO_GROUP_ARP:
            reg_idx = CYPRESS_STORM_CTRL_SPCL_PORT_ARP_EXCEED_FLGr;
            field_idx = CYPRESS_ARP_EXCEED_FLGf;
            break;
        default:
            return RT_ERR_INPUT;
    }

    RATE_SEM_LOCK(unit);

    /* write value from CHIP*/
    if ((ret = reg_array_field_write1toClear(unit, reg_idx, port, REG_ARRAY_INDEX_NONE, field_idx)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_rate_stormControlBypass_get
 * Description:
 *      Get the status of bypass storm control for specified packet type.
 * Input:
 *      unit         - unit id
 *      bypassType   - bypass type
 * Output:
 *      pEnable      - pointer to enable status of bypass storm control
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT   - Invalid input parameter
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      None.
 */
int32
dal_cypress_rate_stormControlBypass_get(uint32 unit, rtk_rate_storm_bypassType_t bypassType, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, bypassType=%d", unit, bypassType);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(bypassType >= STORM_BYPASS_END, RT_ERR_INPUT);

    RATE_SEM_LOCK(unit);

    /* get value from CHIP*/
    switch (bypassType)
    {
        case STORM_BYPASS_RMA:
            ret = reg_field_read(unit, CYPRESS_STORM_CTRL_CTRLr, CYPRESS_RMA_ADMITf, &value);
            break;
        case STORM_BYPASS_BPDU:
            ret = reg_field_read(unit, CYPRESS_STORM_CTRL_CTRLr, CYPRESS_BPDU_ADMITf, &value);
            break;
        case STORM_BYPASS_RTKPKT:
            ret = reg_field_read(unit, CYPRESS_STORM_CTRL_CTRLr, CYPRESS_RTKPKT_ADMITf, &value);
            break;
        case STORM_BYPASS_IGMP:
            ret = reg_field_read(unit, CYPRESS_STORM_CTRL_CTRLr, CYPRESS_IGMP_ADMITf, &value);
            break;
        case STORM_BYPASS_ARP:
            ret = reg_field_read(unit, CYPRESS_STORM_CTRL_CTRLr, CYPRESS_ARPREQ_ADMITf, &value);
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

    return RT_ERR_OK;}

/* Function Name:
 *      dal_cypress_rate_stormControlBypass_set
 * Description:
 *      Set the status of bypass storm control for specified packet type.
 * Input:
 *      unit         - unit id
 *      bypassType   - bypass type
 *      enable       - status of bypass storm control
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT   - Invalid input parameter
 * Note:
 *      None.
 */
int32
dal_cypress_rate_stormControlBypass_set(uint32 unit, rtk_rate_storm_bypassType_t bypassType, rtk_enable_t enable)
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
            ret = reg_field_write(unit, CYPRESS_STORM_CTRL_CTRLr, CYPRESS_RMA_ADMITf, &value);
            break;
        case STORM_BYPASS_BPDU:
            ret = reg_field_write(unit, CYPRESS_STORM_CTRL_CTRLr, CYPRESS_BPDU_ADMITf, &value);
            break;
        case STORM_BYPASS_RTKPKT:
            ret = reg_field_write(unit, CYPRESS_STORM_CTRL_CTRLr, CYPRESS_RTKPKT_ADMITf, &value);
            break;
        case STORM_BYPASS_IGMP:
            ret = reg_field_write(unit, CYPRESS_STORM_CTRL_CTRLr, CYPRESS_IGMP_ADMITf, &value);
            break;
        case STORM_BYPASS_ARP:
            ret = reg_field_write(unit, CYPRESS_STORM_CTRL_CTRLr, CYPRESS_ARPREQ_ADMITf, &value);
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
}

/* Function Name:
 *      dal_cypress_rate_stormControlIncludeIfg_get
 * Description:
 *      Get enable status of includes IFG for storm control.
 * Input:
 *      unit         - unit id
 * Output:
 *      pIfg_include - pointer to enable status of includes IFG
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_cypress_rate_stormControlIncludeIfg_get(uint32 unit, rtk_enable_t *pIfg_include)
{
    int32 ret;
    uint32 value;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIfg_include), RT_ERR_NULL_POINTER);

    RATE_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_field_read(unit, CYPRESS_STORM_CTRL_CTRLr, CYPRESS_INC_IFGf, &value)) != RT_ERR_OK)
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
}

/* Function Name:
 *      dal_cypress_rate_stormControlIncludeIfg_set
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
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None
 */
int32
dal_cypress_rate_stormControlIncludeIfg_set(uint32 unit, rtk_enable_t ifg_include)
{
    int32 ret;
    uint32 value;

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
    if ((ret = reg_field_write(unit, CYPRESS_STORM_CTRL_CTRLr, CYPRESS_INC_IFGf, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_rate_stormControlRateMode_get
 * Description:
 *      Get rate counting mode of storm control.
 * Input:
 *      unit  - unit id
 * Output:
 *      pRate_mode - pointer to rate counting mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *    (1) The rate mode are as following:
 *      - BASED_ON_PKT
 *      - BASED_ON_BYTE
 */
int32
dal_cypress_rate_stormControlRateMode_get(uint32 unit, rtk_rate_storm_rateMode_t *pMode)
{
    uint32  value;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pMode), RT_ERR_NULL_POINTER);

    RATE_SEM_LOCK(unit);

    /* get value from CHIP */
    if ((ret = reg_field_read(unit, CYPRESS_STORM_CTRL_CTRLr, CYPRESS_MODEf, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    if (1 == value)
        *pMode = BASED_ON_BYTE;
    else if (0 == value)
        *pMode = BASED_ON_PKT;
    else
        return RT_ERR_FAILED;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "pMode=%d", *pMode);

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_cypress_rate_stormControlRateMode_set
 * Description:
 *      Set rate counting mode of storm control.
 * Input:
 *      unit - unit id
 *      rate_mode  - Rate counting mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *    (1) The rate mode are as following:
 *      - BASED_ON_PKT
 *      - BASED_ON_BYTE
 */
int32
dal_cypress_rate_stormControlRateMode_set(uint32 unit, rtk_rate_storm_rateMode_t mode)
{
    uint32  value;
    int32   ret;
    uint32 tickPeriod1g, tickPeriod10g;
    uint32 tokenLen1g, tokenLen10g;
    uint32 burstSize1g, burstSize10g;
    rtk_port_t port;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_RATE), "unit=%d, mode=%d", unit, mode);

    /* check Init status */
    RT_INIT_CHK(rate_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((mode >= STORM_RATE_MODE_END), RT_ERR_INPUT);

    if (BASED_ON_PKT == mode)
        value = 0;
    else  /* BASED_ON_BYTE == mode */
        value = 1;

    RATE_SEM_LOCK(unit);

    /* program value to CHIP */
    if ((ret = reg_field_write(unit, CYPRESS_STORM_CTRL_CTRLr, CYPRESS_MODEf, &value)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    if (HWP_8350_FAMILY_ID(unit)) /* 50MHz */
    {
        if(BASED_ON_BYTE == mode)
        {
            tickPeriod1g = STORM_LB_BPS_TICK_50M;
            tokenLen1g = STORM_LB_BPS_TOKEN_50M;
            tickPeriod10g = STORM_LB_BPS_TICK_50M_10G;
            tokenLen10g = STORM_LB_BPS_TOKEN_50M_10G;
            burstSize1g = STORM_LB_BPS_BURST;
            burstSize10g = STORM_LB_BPS_BURST_10G;
        }
        else
        {
            tickPeriod1g = STORM_LB_PPS_TICK_50M;
            tokenLen1g = STORM_LB_PPS_TOKEN_50M;
            tickPeriod10g = STORM_LB_PPS_TICK_50M_10G;
            tokenLen10g = STORM_LB_PPS_TOKEN_50M_10G;
            burstSize1g = STORM_LB_PPS_BURST;
            burstSize10g = STORM_LB_PPS_BURST_10G;
        }
    }
    else /* 250MHz */
    {
        if(BASED_ON_BYTE == mode)
        {
            tickPeriod1g = STORM_LB_BPS_TICK_250M;
            tokenLen1g = STORM_LB_BPS_TOKEN_250M;
            tickPeriod10g = STORM_LB_BPS_TICK_250M_10G;
            tokenLen10g = STORM_LB_BPS_TOKEN_250M_10G;
            burstSize1g = STORM_LB_BPS_BURST;
            burstSize10g = STORM_LB_BPS_BURST_10G;
        }
        else
        {
            tickPeriod1g = STORM_LB_PPS_TICK_250M;
            tokenLen1g = STORM_LB_PPS_TOKEN_250M;
            tickPeriod10g = STORM_LB_PPS_TICK_250M_10G;
            tokenLen10g = STORM_LB_PPS_TOKEN_250M_10G;
            burstSize1g = STORM_LB_PPS_BURST;
            burstSize10g = STORM_LB_PPS_BURST_10G;
        }
    }

    if ((ret = reg_field_write(unit, CYPRESS_STORM_CTRL_LB_TICK_TKN_CTRLr, CYPRESS_TICK_PERIOD_10Gf, &tickPeriod10g)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }
    if ((ret = reg_field_write(unit, CYPRESS_STORM_CTRL_LB_TICK_TKN_CTRLr, CYPRESS_TKN_10Gf, &tokenLen10g)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }
    if ((ret = reg_field_write(unit, CYPRESS_STORM_CTRL_LB_TICK_TKN_CTRLr, CYPRESS_TICK_PERIODf, &tickPeriod1g)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }
    if ((ret = reg_field_write(unit, CYPRESS_STORM_CTRL_LB_TICK_TKN_CTRLr, CYPRESS_TKNf, &tokenLen1g)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }
    if ((ret = reg_field_write(unit, CYPRESS_STORM_CTRL_LB_TICK_TKN_CTRLr, CYPRESS_TKNf, &tokenLen1g)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
    {
        if(HWP_PORT_EXIST(unit, port))
        {
            if(HWP_10GE_PORT(unit, port))
            {
                dal_cypress_rate_portStormCtrlBurstSize_set(
                    unit,
                    port,
                    STORM_GROUP_MULTICAST,
                    burstSize10g);
                dal_cypress_rate_portStormCtrlBurstSize_set(
                    unit,
                    port,
                    STORM_GROUP_UNICAST,
                    burstSize10g);
                dal_cypress_rate_portStormCtrlBurstSize_set(
                    unit,
                    port,
                    STORM_GROUP_BROADCAST,
                    burstSize10g);
            }
            else
            {
                dal_cypress_rate_portStormCtrlBurstSize_set(
                    unit,
                    port,
                    STORM_GROUP_MULTICAST,
                    burstSize1g);
                dal_cypress_rate_portStormCtrlBurstSize_set(
                    unit,
                    port,
                    STORM_GROUP_UNICAST,
                    burstSize1g);
                dal_cypress_rate_portStormCtrlBurstSize_set(
                    unit,
                    port,
                    STORM_GROUP_BROADCAST,
                    burstSize1g);
            }
        }
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      _dal_cypress_rate_init_config
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
_dal_cypress_rate_init_config(uint32 unit)
{
    int32   ret;
    rtk_port_t  port;
    rtk_qid_t   queue, max_queue;
    uint32  value;
    uint32  specialTickPeriod;
    uint32  igrBwTickPeriod1g, igrBwTokenLen1g, igrBwTickPeriod10g, igrBwTokenLen10g;
    uint32  egrBwTickPeriod1g, egrBwTokenLen1g, egrBwTickPeriod10g, egrBwTokenLen10g, egrBwTickPeriodCPU, egrBwTokenLenCPU;
    uint32  max_rate;
    rtk_rate_storm_group_t  type;
#if (defined(CONFIG_SDK_LINKMON_POLLING_MODE) || defined(CONFIG_SDK_LINKMON_MIXED_MODE))
    rtk_portmask_t swScan_portmask;
#endif

    max_queue = HAL_MAX_NUM_OF_QUEUE(unit);

    HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
    {
        egr_bandwidth_ctrl[unit][port].rate = 0xFFFF;
    }


#if defined(CONFIG_SDK_LINKMON_POLLING_MODE) || defined(CONFIG_SDK_LINKMON_MIXED_MODE)
    HWP_GET_ATTRI_PORTMASK(unit, HWP_ETH_ID, swScan_portmask);
    if ((ret = dal_linkMon_swScanPorts_set(unit, &swScan_portmask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }
#endif

    RATE_SEM_LOCK(unit);

    /* Tick and Token setting */
    if (HWP_8350_FAMILY_ID(unit)) /* 50MHz */
    {
        specialTickPeriod = STORM_LB_PROTO_TICK_50M;
        igrBwTickPeriod1g = IGRBW_LB_BPS_TICK_50M;
        igrBwTokenLen1g = IGRBW_LB_BPS_TOKEN_50M;
        igrBwTickPeriod10g = IGRBW_LB_BPS_TICK_50M_10G;
        igrBwTokenLen10g = IGRBW_LB_BPS_TOKEN_50M_10G;
        egrBwTickPeriod1g = EGRBW_LB_BPS_TICK_50M;
        egrBwTokenLen1g = EGRBW_LB_BPS_TOKEN_50M;
        egrBwTickPeriod10g = EGRBW_LB_BPS_TICK_50M_10G;
        egrBwTokenLen10g = EGRBW_LB_BPS_TOKEN_50M_10G;
        egrBwTickPeriodCPU = EGRBW_LB_PPS_TICK_50M_CPU;
        egrBwTokenLenCPU = EGRBW_LB_PPS_TOKEN_50M_CPU;
    }
    else /* 250MHz */
    {
        specialTickPeriod = STORM_LB_PROTO_TICK_250M;
        igrBwTickPeriod1g = IGRBW_LB_BPS_TICK_250M;
        igrBwTokenLen1g = IGRBW_LB_BPS_TOKEN_250M;
        igrBwTickPeriod10g = IGRBW_LB_BPS_TICK_250M_10G;
        igrBwTokenLen10g = IGRBW_LB_BPS_TOKEN_250M_10G;
        egrBwTickPeriod1g = EGRBW_LB_BPS_TICK_250M;
        egrBwTokenLen1g = EGRBW_LB_BPS_TOKEN_250M;
        egrBwTickPeriod10g = EGRBW_LB_BPS_TICK_250M_10G;
        egrBwTokenLen10g = EGRBW_LB_BPS_TOKEN_250M_10G;
        egrBwTickPeriodCPU = EGRBW_LB_PPS_TICK_250M_CPU;
        egrBwTokenLenCPU = EGRBW_LB_PPS_TOKEN_250M_CPU;
    }
    if ((ret = reg_field_write(unit, CYPRESS_STORM_CTRL_SPCL_LB_TICK_TKN_CTRLr, CYPRESS_TICK_PERIODf, &specialTickPeriod)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }
    if ((ret = reg_field_write(unit, CYPRESS_IGR_BWCTRL_LB_TICK_TKN_CTRLr, CYPRESS_TICK_PERIOD_10Gf, &igrBwTickPeriod10g)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }
    if ((ret = reg_field_write(unit, CYPRESS_IGR_BWCTRL_LB_TICK_TKN_CTRLr, CYPRESS_TKN_10Gf, &igrBwTokenLen10g)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }
    if ((ret = reg_field_write(unit, CYPRESS_IGR_BWCTRL_LB_TICK_TKN_CTRLr, CYPRESS_TICK_PERIODf, &igrBwTickPeriod1g)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }
    if ((ret = reg_field_write(unit, CYPRESS_IGR_BWCTRL_LB_TICK_TKN_CTRLr, CYPRESS_TKNf, &igrBwTokenLen1g)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }
    if ((ret = reg_field_write(unit, CYPRESS_SCHED_LB_TICK_TKN_CTRLr, CYPRESS_TICK_PERIOD_10Gf, &egrBwTickPeriod10g)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }
    if ((ret = reg_field_write(unit, CYPRESS_SCHED_LB_TICK_TKN_CTRLr, CYPRESS_BYTE_PER_TKN_10Gf, &egrBwTokenLen10g)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }
    if ((ret = reg_field_write(unit, CYPRESS_SCHED_LB_TICK_TKN_CTRLr, CYPRESS_TICK_PERIODf, &egrBwTickPeriod1g)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }
    if ((ret = reg_field_write(unit, CYPRESS_SCHED_LB_TICK_TKN_CTRLr, CYPRESS_BYTE_PER_TKNf, &egrBwTokenLen1g)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    if ((ret = reg_field_write(unit, CYPRESS_SCHED_LB_TICK_TKN_PPS_CTRLr, CYPRESS_TICK_PERIOD_PPSf, &egrBwTickPeriodCPU)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }
    if ((ret = reg_field_write(unit, CYPRESS_SCHED_LB_TICK_TKN_PPS_CTRLr, CYPRESS_TKN_PPSf, &egrBwTokenLenCPU)) != RT_ERR_OK)
    {
        RATE_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    RATE_SEM_UNLOCK(unit);

    /* WFQ burst size setting */
    if ((ret = dal_cypress_rate_egrPortBwCtrlBurstSize_set(unit, EGRBW_LB_WFQ_BURST)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    if ((ret = dal_cypress_rate_stormControlRateMode_set(unit, RTK_DEFAULT_STORM_CONTROL_RATE_MODE)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    HWP_PORT_TRAVS_EXCEPT_CPU(unit, port)
    {

        if ((ret = dal_cypress_rate_portEgrBwCtrlEnable_set(unit, port, RTK_DEFAULT_EGR_BANDWIDTH_CTRL_STATUS)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }
        egr_bandwidth_ctrl[unit][port].rate = HAL_GET_MAX_BANDWIDTH_OF_PORT(unit, port);

        for (queue = 0; queue < max_queue; queue++)
        {
            if ((ret = dal_cypress_rate_portEgrQueueBwCtrlEnable_set(unit, port, queue, RTK_DEFAULT_EGR_BANDWIDTH_CTRL_STATUS)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
                return ret;
            }
            max_rate = HAL_GET_MAX_BANDWIDTH_OF_PORT(unit, port);
            if ((ret = dal_cypress_rate_portEgrQueueBwCtrlRate_set(unit, port, queue, max_rate)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
                return ret;
            }
        }

        for (type = 0; type < STORM_GROUP_END; type++)
        {
            if ((ret = dal_cypress_rate_portStormCtrlRate_get(unit, port, type, &value)) != RT_ERR_OK)
            {
                pRate_info[unit]->stormControl_rate[port][type] = value;
                if (value == 0xFFFFF)
                    pRate_info[unit]->stormControl_enable[port][type] = DISABLED;
                else
                    pRate_info[unit]->stormControl_enable[port][type] = ENABLED;
            }
        }

        /* Ingress Bandwidth Per-port High Threshold */
        if ((ret = dal_cypress_rate_portIgrBandwidthHighThresh_set(unit, port, IGRBW_HIGH_THRESH)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }
    }

    /* Ingress Bandwidth Global Low Threshold */
    if ((ret = dal_cypress_rate_igrBandwidthLowThresh_set(unit, IGRBW_LOW_THRESH)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    if ((ret = dal_cypress_rate_cpuEgrBandwidthCtrlRateMode_set(unit, RTK_DEFAULT_EGR_BANDWIDTH_CPU_PORT_RATE_MODE)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of _dal_cypress_rate_init_config */


/* Function Name:
 *      dal_cypress_rate_includeIfg_get
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
dal_cypress_rate_includeIfg_get(uint32 unit, rtk_rate_module_t module, rtk_enable_t *pIfg_include)
{
    switch(module)
    {
        case RATE_MODULE_IGR:
            return dal_cypress_rate_igrBandwidthCtrlIncludeIfg_get(unit, pIfg_include);
        case RATE_MODULE_EGR:
            return dal_cypress_rate_egrBandwidthCtrlIncludeIfg_get(unit, pIfg_include);
        case RATE_MODULE_STORM:
            return dal_cypress_rate_stormControlIncludeIfg_get(unit, pIfg_include);
        default:
            return RT_ERR_INPUT;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_cypress_rate_includeIfg_set
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
dal_cypress_rate_includeIfg_set(uint32 unit, rtk_rate_module_t module, rtk_enable_t ifg_include)
{
    switch(module)
    {
        case RATE_MODULE_IGR:
            return dal_cypress_rate_igrBandwidthCtrlIncludeIfg_set(unit, ifg_include);
        case RATE_MODULE_EGR:
            return dal_cypress_rate_egrBandwidthCtrlIncludeIfg_set(unit, ifg_include);
        case RATE_MODULE_STORM:
            return dal_cypress_rate_stormControlIncludeIfg_set(unit, ifg_include);
        default:
            return RT_ERR_INPUT;
    }

    return RT_ERR_OK;
}


