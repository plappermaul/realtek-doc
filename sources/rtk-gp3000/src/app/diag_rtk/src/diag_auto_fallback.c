/*
 * Copyright (C) 2012 Realtek Semiconductor Corp.
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
 * Purpose : Definition those Layer2 command and APIs in the SDK diagnostic shell.
 *
 * Feature : The file have include the following module and sub-modules
 *
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
#include <hal/mac/reg.h>

#ifdef CONFIG_SDK_APOLLOMP
#include <hal/chipdef/apollomp/rtk_apollomp_reg_struct.h>
#endif

typedef enum diag_autofallback_monitor_e
{
    DIAG_MONITOR_8K = 0,
    DIAG_MONITOR_16K,
    DIAG_MONITOR_32K,
    DIAG_MONITOR_64K,
    DIAG_MONITOR_128K,
    DIAG_MONITOR_256K,
    DIAG_MONITOR_512K,
    DIAG_MONITOR_1M,
    DIAG_MONITOR_END
} diag_autofallback_monitor_t;

typedef enum diag_autofallback_error_e
{
    DIAG_ERROR_1_PKT = 0,
    DIAG_ERROR_2_PKT,
    DIAG_ERROR_4_PKT,
    DIAG_ERROR_8_PKT,
    DIAG_ERROR_16_PKT,
    DIAG_ERROR_32_PKT,
    DIAG_ERROR_64_PKT,
    DIAG_ERROR_128_PKT,
    DIAG_ERROR_END
} diag_autofallback_error_t;




/*
 * auto-fallback get ignore-timeout state
 */
cparser_result_t
cparser_cmd_auto_fallback_get_ignore_timeout_state(
    cparser_context_t *context)
{
    int32 ret;
    rtk_enable_t state=ENABLED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollo_raw_autofallback_timeoutIgnore_get(&state), ret);

            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(APOLLOMP_FB_CTRLr, APOLLOMP_TO_IGNOREf, &state), ret);

            break;
#endif
        case UNKNOWN_CHIP_ID:
            /*for coverity dead code issue*/
            diag_util_mprintf("chip id:%x %s\n",DIAG_UTIL_CHIP_TYPE,DIAG_STR_NOTSUPPORT);
            break;
            
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    diag_util_printf("\n Ignore Timeout State: %s", diagStr_enable[state]);
    diag_util_printf("\n\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_auto_fallback_get_ignore_timeout_state */

/*
 * auto-fallback get monitor-count
 */
cparser_result_t
cparser_cmd_auto_fallback_get_monitor_count(
    cparser_context_t *context)
{
    int32 ret;
    //raw_autofallback_monitor_t mon_count;
    diag_autofallback_monitor_t mon_count=DIAG_MONITOR_8K;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollo_raw_autofallback_maxMonitorCount_get(&mon_count), ret);

            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(APOLLOMP_FB_CTRLr, APOLLOMP_MAX_THf, &mon_count), ret);

            break;
#endif
        case UNKNOWN_CHIP_ID:
            /*for coverity dead code issue*/
            diag_util_mprintf("chip id:%x %s\n",DIAG_UTIL_CHIP_TYPE,DIAG_STR_NOTSUPPORT);
            break;
            
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
    diag_util_printf("\n Max Monitor Count: %s", diagStr_AfbMonCount[mon_count]);
    diag_util_printf("\n\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_auto_fallback_get_monitor_count */

/*
 * auto-fallback get port <PORT_LIST:ports> error-counter
 */
cparser_result_t
cparser_cmd_auto_fallback_get_port_ports_error_counter(
    cparser_context_t *context,
    char * *port_ptr)
{
    int32 ret;
    uint32 err_count=0;
    rtk_port_t port=0;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        switch(DIAG_UTIL_CHIP_TYPE)
        {
#if defined(CONFIG_SDK_APOLLO)
            case APOLLO_CHIP_ID:
                DIAG_UTIL_ERR_CHK(apollo_raw_autofallback_errorCount_get(port, &err_count), ret);

                break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
            case APOLLOMP_CHIP_ID:

                DIAG_UTIL_ERR_CHK(reg_array_field_read(APOLLOMP_FB_PORT_ERR_CNTr, (uint32)port, REG_ARRAY_INDEX_NONE, APOLLOMP_CNTf, &err_count), ret);

                break;
#endif
            case UNKNOWN_CHIP_ID:
                /*for coverity dead code issue*/
                diag_util_mprintf("chip id:%x %s\n",DIAG_UTIL_CHIP_TYPE,DIAG_STR_NOTSUPPORT);
                break;
            
            default:
                diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
                return CPARSER_NOT_OK;
                break;
        }
        diag_util_printf("\n Port %d Error counter: %d", port, err_count);
    }

    diag_util_printf("\n\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_auto_fallback_get_port_ports_error_counter */

/*
 * auto-fallback get port <PORT_LIST:ports> monitor-counter
 */
cparser_result_t
cparser_cmd_auto_fallback_get_port_ports_monitor_counter(
    cparser_context_t *context,
    char * *port_ptr)
{
    int32 ret;
    uint32 mon_count=0;
    rtk_port_t port=0;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        switch(DIAG_UTIL_CHIP_TYPE)
        {
#if defined(CONFIG_SDK_APOLLO)
            case APOLLO_CHIP_ID:
                DIAG_UTIL_ERR_CHK(apollo_raw_autofallback_monitorCount_get(port, &mon_count), ret);
                break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
            case APOLLOMP_CHIP_ID:

                DIAG_UTIL_ERR_CHK(reg_array_field_read(APOLLOMP_FB_PORT_MONITOR_CNTr, (uint32)port, REG_ARRAY_INDEX_NONE, APOLLOMP_CNTf, &mon_count), ret);

                break;
#endif
            case UNKNOWN_CHIP_ID:
                break;

            default:
                diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
                return CPARSER_NOT_OK;
                break;
        }
        diag_util_printf("\n Port %d Monitort counter: %d", port, mon_count);
    }

    diag_util_printf("\n\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_auto_fallback_get_port_ports_monitor_counter */

/*
 * auto-fallback get port <PORT_LIST:ports> restore
 */
cparser_result_t
cparser_cmd_auto_fallback_get_port_ports_restore(
    cparser_context_t *context,
    char * *port_ptr)
{
    int32 ret;
    //raw_autofallback_restorePL_t state;
    uint32 state=ENABLED;
    rtk_port_t port=0;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        switch(DIAG_UTIL_CHIP_TYPE)
        {
#if defined(CONFIG_SDK_APOLLO)
            case APOLLO_CHIP_ID:
                DIAG_UTIL_ERR_CHK(apollo_raw_autofallback_restorePL_get(port, &state), ret);
                break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
            case APOLLOMP_CHIP_ID:

                DIAG_UTIL_ERR_CHK(reg_array_field_read(APOLLOMP_FB_PORT_CFGr, (uint32)port, REG_ARRAY_INDEX_NONE, APOLLOMP_RST_PLf, &state), ret);

                break;
#endif
            case UNKNOWN_CHIP_ID:
                /*for coverity dead code issue*/
                diag_util_mprintf("chip id:%x %s\n",DIAG_UTIL_CHIP_TYPE,DIAG_STR_NOTSUPPORT);
                break;
            
            default:
                diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
                return CPARSER_NOT_OK;
                break;
        }

        diag_util_printf("\n Port %d Restore Power Level: %s", port, diagStr_AfbRestorePL[state]);
    }

    diag_util_printf("\n\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_auto_fallback_get_port_ports_restore */

/*
 * auto-fallback get port <PORT_LIST:ports> state
 */
cparser_result_t
cparser_cmd_auto_fallback_get_port_ports_state(
    cparser_context_t *context,
    char * *port_ptr)
{
    int32 ret;
    rtk_enable_t state=ENABLED;
    rtk_port_t port=0;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        switch(DIAG_UTIL_CHIP_TYPE)
        {
#if defined(CONFIG_SDK_APOLLO)
            case APOLLO_CHIP_ID:
                DIAG_UTIL_ERR_CHK(apollo_raw_autofallback_enable_get(port, &state), ret);

                break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
            case APOLLOMP_CHIP_ID:

                DIAG_UTIL_ERR_CHK(reg_array_field_read(APOLLOMP_FB_PORT_CFGr, (uint32)port, REG_ARRAY_INDEX_NONE, APOLLOMP_ENf, &state), ret);

                break;
#endif
            case UNKNOWN_CHIP_ID:
                /*for coverity dead code issue*/
                diag_util_mprintf("chip id:%x %s\n",DIAG_UTIL_CHIP_TYPE,DIAG_STR_NOTSUPPORT);
                break;
            
            default:
                diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
                return CPARSER_NOT_OK;
                break;
        }
        diag_util_printf("\n Port %d Auto Fallback State: %s", port, diagStr_enable[state]);
    }

    diag_util_printf("\n\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_auto_fallback_get_port_ports_state */

/*
 * auto-fallback get port <PORT_LIST:ports> current-power-level
 */
cparser_result_t
cparser_cmd_auto_fallback_get_port_ports_current_power_level(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rtk_port_t port;
    diag_portlist_t portlist;
    uint32 cpl;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        switch(DIAG_UTIL_CHIP_TYPE)
        {
#if defined(CONFIG_SDK_APOLLO)
            case APOLLO_CHIP_ID:
                diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
                break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
            case APOLLOMP_CHIP_ID:

                DIAG_UTIL_ERR_CHK(reg_array_field_read(APOLLOMP_FB_PORT_CFGr, (uint32)port, REG_ARRAY_INDEX_NONE, APOLLOMP_CPLf, &cpl), ret);

                diag_util_printf("\n Port %d Auto Fallback Current Power Level: %d", port, cpl);
                break;
#endif
            case UNKNOWN_CHIP_ID:
                /*for coverity dead code issue*/
                diag_util_mprintf("chip id:%x %s\n",DIAG_UTIL_CHIP_TYPE,DIAG_STR_NOTSUPPORT);
                break;
            
            default:
                diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
                return CPARSER_NOT_OK;
                break;
        }
    }

    diag_util_printf("\n\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_auto_fallback_get_port_ports_current_power_level */

/*
 * auto-fallback get port <PORT_LIST:ports> valid-flow
 */
cparser_result_t
cparser_cmd_auto_fallback_get_port_ports_valid_flow(
    cparser_context_t *context,
    char * *port_ptr)
{
    int32 ret;
    uint32 state=ENABLED;
    rtk_port_t port=0;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        switch(DIAG_UTIL_CHIP_TYPE)
        {
#if defined(CONFIG_SDK_APOLLO)
            case APOLLO_CHIP_ID:
                DIAG_UTIL_ERR_CHK(apollo_raw_autofallback_validFlow_get(port, &state), ret);

                break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
            case APOLLOMP_CHIP_ID:

                DIAG_UTIL_ERR_CHK(reg_array_field_read(APOLLOMP_FB_PORT_CFGr, (uint32)port, REG_ARRAY_INDEX_NONE, APOLLOMP_VALID_FLOWf, &state), ret);

                break;
#endif

        case UNKNOWN_CHIP_ID:
            /*for coverity dead code issue*/
            diag_util_mprintf("chip id:%x %s\n",DIAG_UTIL_CHIP_TYPE,DIAG_STR_NOTSUPPORT);
            break;

            default:
                diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
                return CPARSER_NOT_OK;
                break;
        }
        diag_util_printf("\n Port %d Valid Flow state: %s", port, diagStr_AfbvalidFlow[state]);
    }

    diag_util_printf("\n\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_auto_fallback_get_port_ports_valid_flow */

/*
 * auto-fallback get reduce-power-level state
 */
cparser_result_t
cparser_cmd_auto_fallback_get_reduce_power_level_state(
    cparser_context_t *context)
{    int32 ret;
    rtk_enable_t state = ENABLED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollo_raw_autofallback_reducePL_get(&state), ret);

            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
#endif
        case UNKNOWN_CHIP_ID:
            /*for coverity dead code issue*/
            diag_util_mprintf("chip id:%x %s\n",DIAG_UTIL_CHIP_TYPE,DIAG_STR_NOTSUPPORT);
            break;
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    diag_util_printf("\n Reduce Power Level State: %s", diagStr_enable[state]);
    diag_util_printf("\n\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_auto_fallback_get_reduce_power_level_state */

/*
 * auto-fallback get timer
 */
cparser_result_t
cparser_cmd_auto_fallback_get_timer(
    cparser_context_t *context)
{
    int32 ret;
    uint32 timer_th=0;
    uint32 val;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollo_raw_autofallback_timeoutTH_get(&timer_th), ret);

            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(APOLLOMP_FB_CTRLr, APOLLOMP_TO_THf, &val), ret);

            timer_th = val << 2;
            break;
#endif
        case UNKNOWN_CHIP_ID:
            /*for coverity dead code issue*/
            diag_util_mprintf("chip id:%x %s\n",DIAG_UTIL_CHIP_TYPE,DIAG_STR_NOTSUPPORT);
            break;
            
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    diag_util_printf("\n Timeout threshold: %dms", timer_th);

    diag_util_printf("\n\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_auto_fallback_get_timer */

/*
 * auto-fallback get timer state
 */
cparser_result_t
cparser_cmd_auto_fallback_get_timer_state(
    cparser_context_t *context)
{
    int32 ret;
    rtk_enable_t state=ENABLED;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();


    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollo_raw_autofallback_timer_get(&state), ret);

            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(APOLLOMP_FB_CTRLr, APOLLOMP_STOP_TMRf, &state), ret);

            state = !(state);
            break;
#endif
        case UNKNOWN_CHIP_ID:
            /*for coverity dead code issue*/
            diag_util_mprintf("chip id:%x %s\n",DIAG_UTIL_CHIP_TYPE,DIAG_STR_NOTSUPPORT);
            break;
            
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
    diag_util_printf("\n Timer State: %s", diagStr_enable[state]);
    diag_util_printf("\n\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_auto_fallback_get_timer_state */

/*
 * auto-fallback get error-count
 */
cparser_result_t
cparser_cmd_auto_fallback_get_error_count(
    cparser_context_t *context)
{
    int32 ret;
    //raw_autofallback_error_t err_count;
    diag_autofallback_error_t err_count=DIAG_ERROR_1_PKT;	

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();


    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollo_raw_autofallback_maxErrorCount_get(&err_count), ret);

            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(APOLLOMP_FB_CTRLr, APOLLOMP_ERR_THf, &err_count), ret);

            break;
#endif
        case UNKNOWN_CHIP_ID:
            /*for coverity dead code issue*/
            diag_util_mprintf("chip id:%x %s\n",DIAG_UTIL_CHIP_TYPE,DIAG_STR_NOTSUPPORT);
            break;
                        
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
    diag_util_printf("\n Max Error Count: %s", diagStr_AfbErrCount[err_count]);
    diag_util_printf("\n\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_auto_fallback_get_error_count */

/*
 * auto-fallback set error-count ( 1 | 2 | 4 | 8 | 16 | 32 | 64 | 128 )
 */
cparser_result_t
cparser_cmd_auto_fallback_set_error_count_1_2_4_8_16_32_64_128(
    cparser_context_t *context)
{
    int32 ret;
    //raw_autofallback_error_t err_count;
    diag_autofallback_error_t err_count;

    DIAG_UTIL_PARAM_CHK();

    if(osal_strcmp("1", TOKEN_STR(3)) == 0)
        err_count = DIAG_ERROR_1_PKT;
    else if(osal_strcmp("2", TOKEN_STR(3)) == 0)
        err_count = DIAG_ERROR_2_PKT;
    else if(osal_strcmp("4", TOKEN_STR(3)) == 0)
        err_count = DIAG_ERROR_4_PKT; 
    else if(osal_strcmp("8", TOKEN_STR(3)) == 0)
        err_count = DIAG_ERROR_8_PKT;
    else if(osal_strcmp("16", TOKEN_STR(3)) == 0)
        err_count = DIAG_ERROR_16_PKT;
    else if(osal_strcmp("32", TOKEN_STR(3)) == 0)
        err_count = DIAG_ERROR_32_PKT;
    else if(osal_strcmp("64", TOKEN_STR(3)) == 0)
        err_count = DIAG_ERROR_64_PKT;
    else if(osal_strcmp("128", TOKEN_STR(3)) == 0)
        err_count = DIAG_ERROR_128_PKT;
    else
        return CPARSER_ERR_INVALID_PARAMS;



    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollo_raw_autofallback_maxErrorCount_set(err_count), ret);
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_FB_CTRLr, APOLLOMP_ERR_THf,&err_count), ret);

            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_auto_fallback_set_error_count_1_2_4_8_16_32_64_128 */

/*
 * auto-fallback set ignore-timeout state ( disable | enable )
 */
cparser_result_t
cparser_cmd_auto_fallback_set_ignore_timeout_state_disable_enable(
    cparser_context_t *context)
{
    int32 ret;
    rtk_enable_t state;

    DIAG_UTIL_PARAM_CHK();

    if ('d' == TOKEN_CHAR(4, 0))
        state = DISABLED;
    else if ('e' == TOKEN_CHAR(4, 0))
        state = ENABLED;
    else
        return CPARSER_ERR_INVALID_PARAMS;



    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollo_raw_autofallback_timeoutIgnore_set(state), ret);
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_FB_CTRLr, APOLLOMP_TO_IGNOREf, (uint32 *)&state), ret);

            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_auto_fallback_set_ignore_timeout_state_disable_enable */

/*
 * auto-fallback set monitor-count ( 8K | 16K | 32K | 64K | 128K | 256K | 512K | 1M )
 */
cparser_result_t
cparser_cmd_auto_fallback_set_monitor_count_8K_16K_32K_64K_128K_256K_512K_1M(
    cparser_context_t *context)
{
    int32 ret;
    //raw_autofallback_monitor_t mon_count;
    diag_autofallback_monitor_t mon_count;
    DIAG_UTIL_PARAM_CHK();

    if(osal_strcmp("8K", TOKEN_STR(3)) == 0)
        mon_count = DIAG_MONITOR_8K;
    else if(osal_strcmp("16K", TOKEN_STR(3)) == 0)
        mon_count = DIAG_MONITOR_16K;
    else if(osal_strcmp("32K", TOKEN_STR(3)) == 0)
        mon_count = DIAG_MONITOR_32K;
    else if(osal_strcmp("64K", TOKEN_STR(3)) == 0)
        mon_count = DIAG_MONITOR_64K;
    else if(osal_strcmp("128K", TOKEN_STR(3)) == 0)
        mon_count = DIAG_MONITOR_128K;
    else if(osal_strcmp("256K", TOKEN_STR(3)) == 0)
        mon_count = DIAG_MONITOR_256K;
    else if(osal_strcmp("512K", TOKEN_STR(3)) == 0)
        mon_count = DIAG_MONITOR_512K;
    else if(osal_strcmp("1M", TOKEN_STR(3)) == 0)
        mon_count = DIAG_MONITOR_1M;
    else
        return CPARSER_ERR_INVALID_PARAMS;



    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollo_raw_autofallback_maxMonitorCount_set(mon_count), ret);

            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_FB_CTRLr, APOLLOMP_MAX_THf, &mon_count), ret);
            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }



    return CPARSER_OK;
}    /* end of cparser_cmd_auto_fallback_set_monitor_count_8k_16k_32k_64k_128k_256k_512k_1m */

/*
 * auto-fallback set port <PORT_LIST:ports> state ( disable | enable )
 */
cparser_result_t
cparser_cmd_auto_fallback_set_port_ports_state_disable_enable(
    cparser_context_t *context,
    char * *port_ptr)
{
    int32 ret;
    rtk_port_t port;
    diag_portlist_t portlist;
    rtk_enable_t state;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    if ('d' == TOKEN_CHAR(5, 0))
        state = DISABLED;
    else if ('e' == TOKEN_CHAR(5, 0))
        state = ENABLED;
    else
        return CPARSER_ERR_INVALID_PARAMS;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {

        switch(DIAG_UTIL_CHIP_TYPE)
        {
#if defined(CONFIG_SDK_APOLLO)
            case APOLLO_CHIP_ID:
                DIAG_UTIL_ERR_CHK(apollo_raw_autofallback_enable_set(port, state), ret);

                break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
            case APOLLOMP_CHIP_ID:

                DIAG_UTIL_ERR_CHK(reg_array_field_write(APOLLOMP_FB_PORT_CFGr, (uint32)port, REG_ARRAY_INDEX_NONE, APOLLOMP_ENf, &state), ret);

                break;
#endif
            default:
                diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
                return CPARSER_NOT_OK;
                break;
        }


    }
    return CPARSER_OK;
}    /* end of cparser_cmd_auto_fallback_set_port_ports_state_disable_enable */

/*
 * auto-fallback set reduce-power-level state ( disable | enable )
 */
cparser_result_t
cparser_cmd_auto_fallback_set_reduce_power_level_state_disable_enable(
    cparser_context_t *context)
{
    int32 ret;
    rtk_enable_t state;

    DIAG_UTIL_PARAM_CHK();

    if ('d' == TOKEN_CHAR(3, 0))
        state = DISABLED;
    else if ('e' == TOKEN_CHAR(3, 0))
        state = ENABLED;
    else
        return CPARSER_ERR_INVALID_PARAMS;


    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollo_raw_autofallback_reducePL_set(state), ret);

            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_auto_fallback_set_reduce_power_level_state_disable_enable */

/*
 * auto-fallback set timer state ( disable | enable )
 */
cparser_result_t
cparser_cmd_auto_fallback_set_timer_state_disable_enable(
    cparser_context_t *context)
{
    int32 ret;
    rtk_enable_t state;

    DIAG_UTIL_PARAM_CHK();

     if ('d' == TOKEN_CHAR(4, 0))
        state = DISABLED;
    else if ('e' == TOKEN_CHAR(4, 0))
        state = ENABLED;
    else
        return CPARSER_ERR_INVALID_PARAMS;



    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollo_raw_autofallback_timer_set(state), ret);
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            state = !state;
            DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_FB_CTRLr, APOLLOMP_STOP_TMRf, &state), ret);

            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }


    return CPARSER_OK;
}    /* end of cparser_cmd_auto_fallback_set_timer_state_disable_enable */

/*
 * auto-fallback set timer <UINT:timer>
 */
cparser_result_t
cparser_cmd_auto_fallback_set_timer_timer(
    cparser_context_t *context,
    uint32_t  *timer_ptr)
{
    int32 ret;
    uint32 val;

    DIAG_UTIL_PARAM_CHK();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollo_raw_autofallback_timeoutTH_set(*timer_ptr), ret);

            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            val = (*timer_ptr) >> 2;
            DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_FB_CTRLr, APOLLOMP_TO_THf, &val), ret);

            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_auto_fallback_set_timer_timer */

