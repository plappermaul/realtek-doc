#include <stdio.h>
#include <string.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <diag_util.h>
#include <diag_str.h>
#include <parser/cparser_priv.h>
#include <rtk/rt/rt_time.h>

/*
 * rt_time init
 */
cparser_result_t
cparser_cmd_rt_time_init(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(rt_time_init(), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_time_init */

/*
 * rt_time get cur-time
 */
cparser_result_t
cparser_cmd_rt_time_get_cur_time(
    cparser_context_t *context)
{
    int32     ret = RT_ERR_FAILED;
    rt_time_timeStamp_t timeStamp;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rt_time_curTime_get(&timeStamp), ret);

    diag_util_mprintf("Current Time: %lld sec   %u nanosec\n", timeStamp.sec, timeStamp.nsec);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_time_get_cur_time */

/*
 * rt_time set offset-time ( increase | decrease ) <UINT64:second> <UINT:nanosecond>
 */
cparser_result_t
cparser_cmd_rt_time_set_offset_time_increase_decrease_second_nanosecond(
    cparser_context_t *context,
    uint64_t  *second_ptr,
    uint32_t  *nanosecond_ptr)
{
    int32     ret = RT_ERR_FAILED;
    rt_time_ptpSign_t sign;
    rt_time_timeStamp_t timeStamp;

    DIAG_UTIL_PARAM_CHK();

    if('i' == TOKEN_CHAR(3, 0))
        sign = RT_PTP_SIGN_POSTIVE;
    else if('d' == TOKEN_CHAR(3, 0))
        sign = RT_PTP_SIGN_NEGATIVE;
    else
        return CPARSER_NOT_OK;

    timeStamp.sec = *second_ptr;
    timeStamp.nsec = *nanosecond_ptr;
    DIAG_UTIL_ERR_CHK(rt_time_offsetTime_set(sign, timeStamp), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_time_set_offset_time_increase_decrease_second_nanosecond */

/*
 * rt_time get frequency
 */
cparser_result_t
cparser_cmd_rt_time_get_frequency(
    cparser_context_t *context)
{
    int32     ret = RT_ERR_FAILED;
    uint32 freq;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rt_time_frequency_get(&freq), ret);

    diag_util_mprintf("System Time Frequency: %u(0x%x)\n", freq,freq);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_time_get_frequency */

/*
 * rt_time set frequency <UINT:frequency>
 */
cparser_result_t
cparser_cmd_rt_time_set_frequency_frequency(
    cparser_context_t *context,
    uint32_t  *frequency_ptr)
{
    int32     ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rt_time_frequency_set(*frequency_ptr), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_time_set_frequency_frequency */

/*
 * rt_time set ptp port ( <PORT_LIST:ports> | all ) state ( enable | disable )
 */
cparser_result_t
cparser_cmd_rt_time_set_ptp_port_ports_all_state_enable_disable(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rt_port_t port;
    diag_portlist_t portlist;
    rt_enable_t enable;

    DIAG_UTIL_PARAM_CHK();

    if ('d' == TOKEN_CHAR(6,0))
    {
        enable = DISABLED;
    }
    else //if ('e' == TOKEN_CHAR(10,0))
    {
        enable = ENABLED;
    }

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rt_time_ptpPortEnable_set(port,enable), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_time_set_ptp_port_ports_all_state_enable_disable */

/*
 * rt_time get ptp port ( <PORT_LIST:ports> | all ) state
 */
cparser_result_t
cparser_cmd_rt_time_get_ptp_port_ports_all_state(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rt_port_t port;
    diag_portlist_t portlist;
    rt_enable_t enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rt_time_ptpPortEnable_get(port, &enable), ret);
        diag_util_printf("Port %d PTP state: %s\n", port, diagStr_enable[enable]);

    }

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_time_get_ptp_port_ports_all_state */

/*
 * rt_time set ptp ingress ( l2 | udp ) action ( nop | trap-to-cpu )
 */
cparser_result_t
cparser_cmd_rt_time_set_ptp_ingress_l2_udp_action_nop_trap_to_cpu(
    cparser_context_t *context)
{
    int32 ret;
    rt_time_ptpIgr_type_t type;
    rt_time_ptpIgr_action_t action;
    DIAG_UTIL_PARAM_CHK();

    if ('l' == TOKEN_CHAR(4,0))
    {
        type = RT_PTP_IGR_TYPE_L2;
    }
    else //if ('e' == TOKEN_CHAR(10,0))
    {
        type = RT_PTP_IGR_TYPE_UDP;
    }

    if ('n' == TOKEN_CHAR(6,0))
    {
        action = RT_PTP_IGR_ACTION_NONE;
    }
    else //if ('e' == TOKEN_CHAR(10,0))
    {
        action = RT_PTP_IGR_ACTION_TRAP2CPU;
    }

    DIAG_UTIL_ERR_CHK(rt_time_ptpIgrAction_set(type,action), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_time_set_ptp_ingress_l2_udp_action_nop_trap_to_cpu */

/*
 * rt_time get ptp ingress ( l2 | udp ) action
 */
cparser_result_t
cparser_cmd_rt_time_get_ptp_ingress_l2_udp_action(
    cparser_context_t *context)
{
    int32 ret;
    rt_time_ptpIgr_type_t type;
    rt_time_ptpIgr_action_t action;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    if ('l' == TOKEN_CHAR(4,0))
    {
        type = RT_PTP_IGR_TYPE_L2;
    }
    else //if ('e' == TOKEN_CHAR(10,0))
    {
        type = RT_PTP_IGR_TYPE_UDP;
    }

    DIAG_UTIL_ERR_CHK(rt_time_ptpIgrAction_get(type,&action), ret);
    diag_util_printf("PTP ingress type: %s  Action : %s\n", (type==RT_PTP_IGR_TYPE_L2)?"L2":"UDP", (action==RT_PTP_IGR_ACTION_NONE)?"nop":"trap_to_cpu");

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_time_get_ptp_ingress_l2_udp_action */

/*
 * rt_time set ptp egress action ( nop | one-step | two-step )
 */
cparser_result_t
cparser_cmd_rt_time_set_ptp_egress_action_nop_one_step_two_step(
    cparser_context_t *context)
{
    int32 ret;
    rt_time_ptpEgr_action_t action;

    DIAG_UTIL_PARAM_CHK();

    if ('n' == TOKEN_CHAR(5,0))
    {
        action = RT_PTP_EGR_ACTION_NONE;
    }
    if ('o' == TOKEN_CHAR(5,0))
    {
        action = RT_PTP_EGR_ACTION_ONESTEP;
    }
    else
    {
        action = RT_PTP_EGR_ACTION_TWOSTEP;
    }

    DIAG_UTIL_ERR_CHK(rt_time_ptpEgrAction_set(action), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_time_set_ptp_egress_action_nop_one_step_two_step */

/*
 * rt_time get ptp egress action
 */
cparser_result_t
cparser_cmd_rt_time_get_ptp_egress_action(
    cparser_context_t *context)
{
    int32 ret;
    rt_time_ptpEgr_action_t action;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rt_time_ptpEgrAction_get(&action), ret);
    diag_util_printf("PTP egress Action : %s\n", (action==RT_PTP_EGR_ACTION_NONE)?"nop":((action==RT_PTP_EGR_ACTION_ONESTEP)?"one-step":"two-step"));

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_time_get_ptp_egress_action */

/*
 * rt_time set pontod ( xgpon | gpon | epon ) startpoint <UINT64:cnt> time <UINT64:second> <UINT:nanosecond>
 */
cparser_result_t
cparser_cmd_rt_time_set_pontod_xgpon_gpon_epon_startpoint_cnt_time_second_nanosecond(
    cparser_context_t *context,
    uint64_t  *cnt_ptr,
    uint64_t  *second_ptr,
    uint32_t  *nanosecond_ptr)
{
    int32     ret = RT_ERR_FAILED;
    rt_pon_tod_t ponTod;

    DIAG_UTIL_PARAM_CHK();

    if ('x' == TOKEN_CHAR(3,0))
    {
        ponTod.ponMode = 2;//xgpon
        ponTod.startPoint.xSuperFrame = *cnt_ptr;
    }
    else if ('g' == TOKEN_CHAR(3,0))
    {
        ponTod.ponMode = 1;//gpon
        ponTod.startPoint.superFrame = *cnt_ptr;
    }
    else //if ('e' == TOKEN_CHAR(3,0))
    {
        ponTod.ponMode = 0;//epon
        ponTod.startPoint.localTime = *cnt_ptr;
    }

    ponTod.timeStamp.sec = *second_ptr;
    ponTod.timeStamp.nsec = *nanosecond_ptr;
    DIAG_UTIL_ERR_CHK(rt_time_ponTodTime_set(ponTod), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_time_set_pontod_gpon_epon_startpoint_cnt_time_second_nanosecond */

/*
 * rt_time set pontod state ( enable | disable )
 */
cparser_result_t
cparser_cmd_rt_time_set_pontod_state_enable_disable(
    cparser_context_t *context)
{
    int32     ret = RT_ERR_FAILED;
    rt_enable_t enable;

    DIAG_UTIL_PARAM_CHK();

    if ('d' == TOKEN_CHAR(4,0))
    {
        enable = DISABLED;
    }
    else //if ('e' == TOKEN_CHAR(10,0))
    {
        enable = ENABLED;
    }

    DIAG_UTIL_ERR_CHK(rt_time_ponTodTimeEnable_set(enable), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_time_set_pontod_state_enable_disable */

/*
 * rt_time get pontod
 */
cparser_result_t
cparser_cmd_rt_time_get_pontod(
    cparser_context_t *context)
{
    int32     ret = RT_ERR_FAILED;
    rt_pon_tod_t ponTod;
    rt_enable_t enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rt_time_ponTodTimeEnable_get(&enable), ret);
    DIAG_UTIL_ERR_CHK(rt_time_ponTodTime_get(&ponTod), ret);

    diag_util_mprintf("PON ToD state: %s\n",diagStr_enable[enable]);

    if(ponTod.ponMode == 2)
        diag_util_mprintf("XGPON ToD superframe: %llu  timestamp: %llu sec   %u nanosec\n",ponTod.startPoint.xSuperFrame,ponTod.timeStamp.sec,ponTod.timeStamp.nsec);
    else if(ponTod.ponMode == 1)
        diag_util_mprintf("GPON ToD superframe: %u  timestamp: %llu sec   %u nanosec\n",ponTod.startPoint.superFrame,ponTod.timeStamp.sec,ponTod.timeStamp.nsec);
    else
        diag_util_mprintf("EPON ToD localtime: %u  timestamp: %llu sec   %u nanosec\n",ponTod.startPoint.localTime,ponTod.timeStamp.sec,ponTod.timeStamp.nsec);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_time_get_pontod */
