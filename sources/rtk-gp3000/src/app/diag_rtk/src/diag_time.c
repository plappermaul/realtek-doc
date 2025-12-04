 /*
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
 * Purpose : Define diag shell commands for PTP.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) PTP commands.
 */

/*
 * Include Files
 */

#include <stdio.h>
#include <string.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <diag_util.h>
#include <diag_str.h>
#include <parser/cparser_priv.h>
#include <hal/chipdef/apollomp/rtk_apollomp_reg_struct.h>
#include <hal/chipdef/rtl9602c/rtk_rtl9602c_reg_struct.h>
#include <rtk/time.h>

/*
 * time init
 */
cparser_result_t
cparser_cmd_time_init(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(rtk_time_init(), ret);

    return CPARSER_OK;

}    /* end of cparser_cmd_time_init */

/*
 * time get ptp ( <PORT_LIST:ports> | all ) state
 */
cparser_result_t
cparser_cmd_time_get_ptp_ports_all_state(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32           ret = RT_ERR_FAILED;
    rtk_port_t      port = 0;
    diag_portlist_t  portlist;
    rtk_enable_t state;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_time_portPtpEnable_get(port, &state), ret);
        diag_util_mprintf("\n Port %d PTP state: %s", port, diagStr_enable[state]);
    }
    diag_util_mprintf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_time_get_ptp_ports_all_state */

/*
 * time set ptp ( <PORT_LIST:ports> | all ) state ( enable | disable )
 */
cparser_result_t
cparser_cmd_time_set_ptp_ports_all_state_enable_disable(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32           ret = RT_ERR_FAILED;
    rtk_port_t      port = 0;
    diag_portlist_t  portlist;
    rtk_enable_t state;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    if('e' == TOKEN_CHAR(5, 0))
        state = ENABLED;
    else if('d' == TOKEN_CHAR(5, 0))
        state = DISABLED;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_time_portPtpEnable_set(port, state), ret);
    }

    return CPARSER_OK;

}    /* end of cparser_cmd_time_set_ptp_ports_all_state_enable_disable */

/*
 * time get cur-time
 */
cparser_result_t
cparser_cmd_time_get_cur_time(
    cparser_context_t *context)
{
    int32 ret;
    rtk_time_timeStamp_t timeStamp;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_time_curTime_get(&timeStamp), ret);

    diag_util_mprintf("Current Time: %lld sec   %u nanosec\n", timeStamp.sec, timeStamp.nsec);

    return CPARSER_OK;
}    /* end of cparser_cmd_time_get_cur_time */

/*
 * time latch cur-time
 */
cparser_result_t
cparser_cmd_time_latch_cur_time(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(rtk_time_curTime_latch(), ret);

    return CPARSER_OK;

}    /* end of cparser_cmd_time_latch_cur_time */

/*
 * time get ref-time
 */
cparser_result_t
cparser_cmd_time_get_ref_time(
    cparser_context_t *context)
{
    int32 ret;
    uint32 sign;
    rtk_time_timeStamp_t timeStamp;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_time_refTime_get(&sign, &timeStamp), ret);

    diag_util_mprintf("Reference Time: %s  %lld sec   %u nanosec\n", diagStr_timeSign[sign], timeStamp.sec, timeStamp.nsec);

    return CPARSER_OK;
}    /* end of cparser_cmd_time_get_ref_time */

/*
 * time set ref-time ( increase | decrease ) <UINT64:second> <UINT:nanosecond>
 */
cparser_result_t
cparser_cmd_time_set_ref_time_increase_decrease_second_nanosecond(
    cparser_context_t *context,
    uint64_t  *second_ptr,
    uint32_t  *nanosecond_ptr)
{
    int32 ret;
    uint32 sign;
    rtk_time_timeStamp_t timeStamp;

    DIAG_UTIL_PARAM_CHK();

    if('i' == TOKEN_CHAR(3, 0))
        sign = PTP_REF_SIGN_POSTIVE;
    else if('d' == TOKEN_CHAR(3, 0))
        sign = PTP_REF_SIGN_NEGATIVE;
    else
        return CPARSER_NOT_OK;

    timeStamp.sec = *second_ptr;
    timeStamp.nsec = *nanosecond_ptr;
    DIAG_UTIL_ERR_CHK(rtk_time_refTime_set(sign, timeStamp), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_time_set_ref_time_increase_decrease_second_nanosecond */

/*
 * time get frequency
 */
cparser_result_t
cparser_cmd_time_get_frequency(
    cparser_context_t *context)
{
    int32 ret;
    uint32 freq;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_time_frequency_get(&freq), ret);

    diag_util_mprintf("System Frequency: %u\n", freq);

    return CPARSER_OK;
}    /* end of cparser_cmd_time_get_frequency */

/*
 * time set frequency <UINT:frequency>
 */
cparser_result_t
cparser_cmd_time_set_frequency_frequency(
    cparser_context_t *context,
    uint32_t  *frequency_ptr)
{
    int32 ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(rtk_time_frequency_set(*frequency_ptr), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_time_set_frequency_frequency */

/*
 * time get ptp ( <PORT_LIST:ports> | all ) transparent state
 */
cparser_result_t
cparser_cmd_time_get_ptp_ports_all_transparent_state(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32           ret = RT_ERR_FAILED;
    rtk_port_t      port = 0;
    diag_portlist_t  portlist;
    rtk_enable_t state;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_time_portTransparentEnable_get(port, &state), ret);
        diag_util_mprintf("\n Port %d PTP transparent state: %s", port, diagStr_enable[state]);
    }

    diag_util_mprintf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_time_get_ptp_ports_all_transparent_state */

/*
 * time set ptp ( <PORT_LIST:ports> | all ) transparent state ( enable | disable )
 */
cparser_result_t
cparser_cmd_time_set_ptp_ports_all_transparent_state_enable_disable(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32           ret = RT_ERR_FAILED;
    rtk_port_t      port = 0;
    diag_portlist_t  portlist;
    rtk_enable_t state;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    if('e' == TOKEN_CHAR(6, 0))
        state = ENABLED;
    else if('d' == TOKEN_CHAR(6, 0))
        state = DISABLED;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_time_portTransparentEnable_set(port, state), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_time_set_ptp_ports_all_transparent_state_enable_disable */

/*
 * time set ingress type (sync | delay-req | pdelay-req | pdelay-resp | follow-up | delay-resp | pdelay-resp-follow-up | announce-signaling | sync-one-step | pdelay-req-one-step ) action ( nop | trap-to-cpu | forward-to-trans | forward-to-trans-and-mirror-to-cpu )
 */
cparser_result_t
cparser_cmd_time_set_ingress_type_sync_delay_req_pdelay_req_pdelay_resp_follow_up_delay_resp_pdelay_resp_follow_up_announce_signaling_sync_one_step_pdelay_req_one_step_action_nop_trap_to_cpu_forward_to_trans_forward_to_trans_and_mirror_to_cpu(
    cparser_context_t *context)
{
    int32 ret = CPARSER_NOT_OK;
    rtk_time_ptpIgrMsg_action_t action;
    rtk_time_ptpMsgType_t type;

    DIAG_UTIL_PARAM_CHK();
    if ('s' == TOKEN_CHAR(4, 0))
    {
        if ('-' == TOKEN_CHAR(4, 4))
           type = PTP_MSG_TYPE_SYNC_ONE_STEP;
        else
           type = PTP_MSG_TYPE_SYNC;
    }
    else if ('d' == TOKEN_CHAR(4, 0))
    {
        if ('q' == TOKEN_CHAR(4, 8))
           type = PTP_MSG_TYPE_DELAY_REQ;
        else
           type = PTP_MSG_TYPE_DELAY_RESP;
    }
    else if ('p' == TOKEN_CHAR(4, 0))
    {
         if ('q' == TOKEN_CHAR(4, 9))
         {
             if ('-' == TOKEN_CHAR(4, 10))
                type = PTP_MSG_TYPE_PDELAY_REQ_ONE_STEP;
             else
                type = PTP_MSG_TYPE_PDELAY_REQ;
         }
         else
         {
             if ('-' == TOKEN_CHAR(4, 11))
                type = PTP_MSG_TYPE_PDELAY_RESP_FOLLOW_UP;
             else
                type = PTP_MSG_TYPE_PDELAY_RESP;
         }
    }
    else if ('f' == TOKEN_CHAR(4, 0))
    {
        type = PTP_MSG_TYPE_FOLLOW_UP;
    }
    else if ('a' == TOKEN_CHAR(4, 0))
    {
        type = PTP_MSG_TYPE_ANNOUNCE_SIGNALING;
    }
    else
        return CPARSER_ERR_INVALID_PARAMS;

    if ('n' == TOKEN_CHAR(6, 0))
    {
        action = PTP_IGR_ACTION_NONE;
    }
    else if('t' == TOKEN_CHAR(6, 0))
    {
        action = PTP_IGR_ACTION_TRAP2CPU;
    }
    else if('f' == TOKEN_CHAR(6, 0))
    {
        if ('-' == TOKEN_CHAR(6, 16))
           action = PTP_IGR_ACTION_FORWARD2TRANS_AND_MIRROR2CPU;
        else
           action = PTP_IGR_ACTION_FORWARD2TRANS;
    }
    else
        return CPARSER_ERR_INVALID_PARAMS;

    DIAG_UTIL_ERR_CHK(rtk_time_ptpIgrMsgAction_set(type, action), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_time_set_ingress_type_sync_delay_req_pdelay_req_pdelay_resp_follow_up_delay_resp_pdelay_resp_follow_up_announce_signaling_sync_one_step_pdelay_req_one_step_action_nop_trap_to_cpu_forward_to_trans_forward_to_trans_and_mirror_to_cpu */

/*
 * time get ingress type (sync | delay-req | pdelay-req | pdelay-resp | follow-up | delay-resp | pdelay-resp-follow-up | announce-signaling | sync-one-step | pdelay-req-one-step ) action
 */
cparser_result_t
cparser_cmd_time_get_ingress_type_sync_delay_req_pdelay_req_pdelay_resp_follow_up_delay_resp_pdelay_resp_follow_up_announce_signaling_sync_one_step_pdelay_req_one_step_action(
    cparser_context_t *context)
{
    int32 ret = CPARSER_NOT_OK;
    rtk_time_ptpIgrMsg_action_t action;
    rtk_time_ptpMsgType_t type;

    DIAG_UTIL_PARAM_CHK();
    if ('s' == TOKEN_CHAR(4, 0))
    {
        if ('-' == TOKEN_CHAR(4, 4))
           type = PTP_MSG_TYPE_SYNC_ONE_STEP;
        else
           type = PTP_MSG_TYPE_SYNC;
    }
    else if ('d' == TOKEN_CHAR(4, 0))
    {
        if ('q' == TOKEN_CHAR(4, 8))
           type = PTP_MSG_TYPE_DELAY_REQ;
        else
           type = PTP_MSG_TYPE_DELAY_RESP;
    }
    else if ('p' == TOKEN_CHAR(4, 0))
    {
         if ('q' == TOKEN_CHAR(4, 9))
         {
             if ('-' == TOKEN_CHAR(4, 10))
                type = PTP_MSG_TYPE_PDELAY_REQ_ONE_STEP;
             else
                type = PTP_MSG_TYPE_PDELAY_REQ;
         }
         else
         {
             if ('-' == TOKEN_CHAR(4, 11))
                type = PTP_MSG_TYPE_PDELAY_RESP_FOLLOW_UP;
             else
                type = PTP_MSG_TYPE_PDELAY_RESP;
         }
    }
    else if ('f' == TOKEN_CHAR(4, 0))
    {
        type = PTP_MSG_TYPE_FOLLOW_UP;
    }
    else if ('a' == TOKEN_CHAR(4, 0))
    {
        type = PTP_MSG_TYPE_ANNOUNCE_SIGNALING;
    }
    else
        return CPARSER_ERR_INVALID_PARAMS;

    DIAG_UTIL_ERR_CHK(rtk_time_ptpIgrMsgAction_get(type, &action), ret);
    diag_util_mprintf("Ingress type %s Action: %s\n", diagStr_timeType[type], diagStr_timeIgrAct[action]);

    return CPARSER_OK;
}    /* end of cparser_cmd_time_get_ingress_type_sync_delay_req_pdelay_req_pdelay_resp_follow_up_delay_resp_pdelay_resp_follow_up_announce_signaling_sync_one_step_pdelay_req_one_step_action */

/*
 * time set egress type (sync | delay-req | pdelay-req | pdelay-resp | follow-up | delay-resp | pdelay-resp-follow-up | announce-signaling | sync-one-step | pdelay-req-one-step ) action ( nop | latch-time | latch-time-and-mirror-to-cpu | modify-correction )
 */
cparser_result_t
cparser_cmd_time_set_egress_type_sync_delay_req_pdelay_req_pdelay_resp_follow_up_delay_resp_pdelay_resp_follow_up_announce_signaling_sync_one_step_pdelay_req_one_step_action_nop_latch_time_latch_time_and_mirror_to_cpu_modify_correction(
    cparser_context_t *context)
{
    int32 ret = CPARSER_NOT_OK;
    rtk_time_ptpEgrMsg_action_t action;
    rtk_time_ptpMsgType_t type;

    DIAG_UTIL_PARAM_CHK();
    if ('s' == TOKEN_CHAR(4, 0))
    {
        if ('-' == TOKEN_CHAR(4, 4))
           type = PTP_MSG_TYPE_SYNC_ONE_STEP;
        else
           type = PTP_MSG_TYPE_SYNC;
    }
    else if ('d' == TOKEN_CHAR(4, 0))
    {
        if ('q' == TOKEN_CHAR(4, 8))
           type = PTP_MSG_TYPE_DELAY_REQ;
        else
           type = PTP_MSG_TYPE_DELAY_RESP;
    }
    else if ('p' == TOKEN_CHAR(4, 0))
    {
         if ('q' == TOKEN_CHAR(4, 9))
         {
             if ('-' == TOKEN_CHAR(4, 10))
                type = PTP_MSG_TYPE_PDELAY_REQ_ONE_STEP;
             else
                type = PTP_MSG_TYPE_PDELAY_REQ;
         }
         else
         {
             if ('-' == TOKEN_CHAR(4, 11))
                type = PTP_MSG_TYPE_PDELAY_RESP_FOLLOW_UP;
             else
                type = PTP_MSG_TYPE_PDELAY_RESP;
         }
    }
    else if ('f' == TOKEN_CHAR(4, 0))
    {
        type = PTP_MSG_TYPE_FOLLOW_UP;
    }
    else if ('a' == TOKEN_CHAR(4, 0))
    {
        type = PTP_MSG_TYPE_ANNOUNCE_SIGNALING;
    }
    else
        return CPARSER_ERR_INVALID_PARAMS;

    if ('n' == TOKEN_CHAR(6, 0))
    {
        action = PTP_EGR_ACTION_NONE;
    }
    else if('m' == TOKEN_CHAR(6, 0))
    {
        action = PTP_EGR_ACTION_MODIFY_CORRECTION;
    }
    else if('l' == TOKEN_CHAR(6, 0))
    {
        if ('-' == TOKEN_CHAR(6, 10))
           action = PTP_EGR_ACTION_LATCH_TIME_AND_MIRROR2CPU;
        else
           action = PTP_EGR_ACTION_LATCH_TIME;
    }
    else
        return CPARSER_ERR_INVALID_PARAMS;

    DIAG_UTIL_ERR_CHK(rtk_time_ptpEgrMsgAction_set(type, action), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_time_set_egress_type_sync_delay_req_pdelay_req_pdelay_resp_follow_up_delay_resp_pdelay_resp_follow_up_announce_signaling_sync_one_step_pdelay_req_one_step_action_nop_latch_time_latch_time_and_mirror_to_cpu_modify_correction */

/*
 * time get egress type (sync | delay-req | pdelay-req | pdelay-resp | follow-up | delay-resp | pdelay-resp-follow-up | announce-signaling | sync-one-step | pdelay-req-one-step ) action
 */
cparser_result_t
cparser_cmd_time_get_egress_type_sync_delay_req_pdelay_req_pdelay_resp_follow_up_delay_resp_pdelay_resp_follow_up_announce_signaling_sync_one_step_pdelay_req_one_step_action(
    cparser_context_t *context)
{
    int32 ret = CPARSER_NOT_OK;
    rtk_time_ptpEgrMsg_action_t action;
    rtk_time_ptpMsgType_t type;

    DIAG_UTIL_PARAM_CHK();
    if ('s' == TOKEN_CHAR(4, 0))
    {
        if ('-' == TOKEN_CHAR(4, 4))
           type = PTP_MSG_TYPE_SYNC_ONE_STEP;
        else
           type = PTP_MSG_TYPE_SYNC;
    }
    else if ('d' == TOKEN_CHAR(4, 0))
    {
        if ('q' == TOKEN_CHAR(4, 8))
           type = PTP_MSG_TYPE_DELAY_REQ;
        else
           type = PTP_MSG_TYPE_DELAY_RESP;
    }
    else if ('p' == TOKEN_CHAR(4, 0))
    {
         if ('q' == TOKEN_CHAR(4, 9))
         {
             if ('-' == TOKEN_CHAR(4, 10))
                type = PTP_MSG_TYPE_PDELAY_REQ_ONE_STEP;
             else
                type = PTP_MSG_TYPE_PDELAY_REQ;
         }
         else
         {
             if ('-' == TOKEN_CHAR(4, 11))
                type = PTP_MSG_TYPE_PDELAY_RESP_FOLLOW_UP;
             else
                type = PTP_MSG_TYPE_PDELAY_RESP;
         }
    }
    else if ('f' == TOKEN_CHAR(4, 0))
    {
        type = PTP_MSG_TYPE_FOLLOW_UP;
    }
    else if ('a' == TOKEN_CHAR(4, 0))
    {
        type = PTP_MSG_TYPE_ANNOUNCE_SIGNALING;
    }
    else
        return CPARSER_ERR_INVALID_PARAMS;

    DIAG_UTIL_ERR_CHK(rtk_time_ptpEgrMsgAction_get(type, &action), ret);
    diag_util_mprintf("Egress type %s Action: %s\n", diagStr_timeType[type], diagStr_timeEgrAct[action]);

    return CPARSER_OK;
}    /* end of cparser_cmd_time_get_egress_type_sync_delay_req_pdelay_req_pdelay_resp_follow_up_delay_resp_pdelay_resp_follow_up_announce_signaling_sync_one_step_pdelay_req_one_step_action */

/*
 * time get mean-path-delay
 */
cparser_result_t
cparser_cmd_time_get_mean_path_delay(
    cparser_context_t *context)
{
    int32 ret;
    uint32  delay;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_time_meanPathDelay_get(&delay), ret);

    diag_util_mprintf("Mean path delay: %u sec\n", delay);

    return CPARSER_OK;
}    /* end of cparser_cmd_time_get_mean_path_delay */

/*
 * time set mean-path-delay <UINT:nanosecond>
 */
cparser_result_t
cparser_cmd_time_set_mean_path_delay_nanosecond(
    cparser_context_t *context,
    uint32_t  *nanosecond_ptr)
{
    int32 ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(rtk_time_meanPathDelay_set(*nanosecond_ptr), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_time_set_mean_path_delay_nanosecond */

/*
 * time get rxtime
 */
cparser_result_t
cparser_cmd_time_get_rxtime(
    cparser_context_t *context)
{
    int32 ret;
    rtk_time_timeStamp_t timeStamp;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_time_rxTime_get(&timeStamp), ret);

    diag_util_mprintf("Rx Time: %lld sec   %u nanosec\n", timeStamp.sec, timeStamp.nsec);

    return CPARSER_OK;
}    /* end of cparser_cmd_time_get_rxtime */

/*
 * time set rxtime <UINT64:second> <UINT:nanosecond>
 */
cparser_result_t
cparser_cmd_time_set_rxtime_second_nanosecond(
    cparser_context_t *context,
    uint64_t  *second_ptr,
    uint32_t  *nanosecond_ptr)
{
    int32 ret;
    rtk_time_timeStamp_t timeStamp;

    DIAG_UTIL_PARAM_CHK();

    timeStamp.sec = *second_ptr;
    timeStamp.nsec = *nanosecond_ptr;
    DIAG_UTIL_ERR_CHK(rtk_time_rxTime_set(timeStamp), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_time_set_rxtime_second_nanosecond */



/*
 * time get ptp ( <PORT_LIST:ports> | all ) tx-indicator
 */
cparser_result_t
cparser_cmd_time_get_ptp_ports_all_tx_indicator(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32           ret = RT_ERR_FAILED;
    rtk_port_t      port = 0;
    diag_portlist_t  portlist;
    rtk_enable_t state;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_time_portPtpTxIndicator_get(port, &state), ret);
        diag_util_mprintf("\n Port %d PTP TX Indicator: %d", port, state);
    }
    diag_util_mprintf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_time_get_ptp_ports_all_tx_indicator */

/*
 * time get tod state
 */
cparser_result_t
cparser_cmd_time_get_tod_state(
    cparser_context_t *context)
{
    int32     ret = RT_ERR_FAILED;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_time_todEnable_get(&enable), ret);

    diag_util_mprintf("PON TOD state: %s\n",diagStr_enable[enable]);


    return CPARSER_OK;
}    /* end of cparser_cmd_time_get_tod_state */

/*
 * time set pps state ( disable | enable )
 */
cparser_result_t
cparser_cmd_time_set_pps_state_disable_enable(
    cparser_context_t *context)
{
    int32     ret = RT_ERR_FAILED;
    rtk_enable_t enable = ENABLED;

    DIAG_UTIL_PARAM_CHK();

    if ('d' == TOKEN_CHAR(4,0))
    {
        enable = DISABLED;
    }
    else
    {
        enable = ENABLED;
    }

    DIAG_UTIL_ERR_CHK(rtk_time_ppsEnable_set(enable), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_time_set_pps_state_disable_enable */

/*
 * time get pps state
 */
cparser_result_t
cparser_cmd_time_get_pps_state(
    cparser_context_t *context)
{
    int32     ret = RT_ERR_FAILED;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_time_ppsEnable_get(&enable), ret);

    diag_util_mprintf("PTP PSS state: %s\n",diagStr_enable[enable]);


    return CPARSER_OK;
}    /* end of cparser_cmd_time_get_pps_state */

/*
 * time set pps mode ( pon | ptp )
 */
cparser_result_t
cparser_cmd_time_set_pps_mode_pon_ptp(
    cparser_context_t *context)
{
    int32     ret = RT_ERR_FAILED;
    rtk_time_ptpPpsMode_t ppsMode;

    DIAG_UTIL_PARAM_CHK();

    if ('o' == TOKEN_CHAR(4,1))
    {
        ppsMode = PTP_PPS_MODE_PON;
    }
    else
    {
        ppsMode = PTP_PPS_MODE_PTP;
    }

    DIAG_UTIL_ERR_CHK(rtk_time_ppsMode_set(ppsMode), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_time_set_pps_mode_pon_ptp */

/*
 * time get pps mode
 */
cparser_result_t
cparser_cmd_time_get_pps_mode(
    cparser_context_t *context)
{
    int32     ret = RT_ERR_FAILED;
    rtk_time_ptpPpsMode_t ppsMode;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_time_ppsMode_get(&ppsMode), ret);

    diag_util_mprintf("PTP PSS mode: %s\n",diagStr_timePpsMode[ppsMode]);


    return CPARSER_OK;
}    /* end of cparser_cmd_time_get_pps_mode */

/*
 * time set ponmode ( gpon | epon ) startpoint <UINT:cnt> pon-tod-time <UINT64:second> <UINT:nanosecond> state ( disable | enable ) */
cparser_result_t
cparser_cmd_time_set_ponmode_gpon_epon_startpoint_cnt_pon_tod_time_second_nanosecond_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *cnt_ptr,
    uint64_t  *second_ptr,
    uint32_t  *nanosecond_ptr)
{
    int32     ret = RT_ERR_FAILED;
    rtk_enable_t enable;
    rtk_pon_tod_t ponTod;
//    rtk_time_timeStamp_t timeStamp;

    DIAG_UTIL_PARAM_CHK();

    if ('g' == TOKEN_CHAR(3,0))
    {
        //DIAG_UTIL_ERR_CHK(rtk_gpon_gtcDsTodSuperFrame_set(cnt_ptr), ret);
        ponTod.ponMode = 1;//gpon
        ponTod.startPoint.superFrame = *cnt_ptr;
    }
    else //if ('e' == TOKEN_CHAR(3,0))
    {
        ponTod.ponMode = 0;//epon
        ponTod.startPoint.localTime = *cnt_ptr;
    }

    if ('d' == TOKEN_CHAR(10,0))
    {
        enable = DISABLED;
    }
    else //if ('e' == TOKEN_CHAR(10,0))
    {
        enable = ENABLED;
    }

    ponTod.timeStamp.sec = *second_ptr;
    ponTod.timeStamp.nsec = *nanosecond_ptr;
    DIAG_UTIL_ERR_CHK(rtk_time_ponTodTime_set(ponTod), ret);

    DIAG_UTIL_ERR_CHK(rtk_time_todEnable_set(enable), ret);

    return CPARSER_OK;

}    /* end of cparser_cmd_time_set_ponmode_gpon_epon_startpoint_cnt_pon_tod_time_second_nanosecond_state_disable_enable */

/*
 * time get free-time */
cparser_result_t
cparser_cmd_time_get_free_time(
    cparser_context_t *context)
{
    int32     ret = RT_ERR_FAILED;
    rtk_time_freerun_t freeTime;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_time_freeTime_get(&freeTime), ret);

    diag_util_mprintf("Freerun Time: %lld nanosec\n", freeTime.nsec);

    return CPARSER_OK;
}    /* end of cparser_cmd_time_get_free_time */
