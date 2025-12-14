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
 * Purpose : Definition those CPU command and APIs in the SDK diagnostic shell.
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
#include <rtk/cpu.h>

/*
 * cpu init
 */
cparser_result_t
cparser_cmd_cpu_init(
    cparser_context_t *context)
{
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_cpu_init(), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_cpu_init */

/*
 * cpu set aware-port ( <PORT_LIST:ports> | all | none )
 */
cparser_result_t
cparser_cmd_cpu_set_aware_port_ports_all_none(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32                       ret;
    diag_portlist_t             portlist;
    rtk_port_t                  port = 0;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(rtk_cpu_awarePortMask_set(portlist.portmask), ret);
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(rtk_cpu_awarePortMask_set(portlist.portmask), ret);
            break;
#endif
#if defined(CONFIG_SDK_RTL9601B)
        case RTL9601B_CHIP_ID:
            DIAG_UTIL_ERR_CHK(rtk_cpu_awarePortMask_set(portlist.portmask), ret);
            break;
#endif

        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_cpu_set_aware_port_ports_all_none */

/*
 * cpu get aware-port
 */
cparser_result_t
cparser_cmd_cpu_get_aware_port(
    cparser_context_t *context)
{
    int32                       ret;
    rtk_portmask_t lPortMask;
    rtk_port_t port = 0;
    rtk_enable_t enable;
    uint8  port_list[DIAG_UTIL_PORT_MASK_STRING_LEN];

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(rtk_cpu_awarePortMask_get(&lPortMask), ret);
            diag_util_lPortMask2str(port_list, &lPortMask);
            diag_util_printf("CPU aware port: %s\n", port_list);
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(rtk_cpu_awarePortMask_get(&lPortMask), ret);
            diag_util_lPortMask2str(port_list, &lPortMask);
            diag_util_printf("CPU aware port: %s\n", port_list);
            break;
#endif
#if defined(CONFIG_SDK_RTL9601B)
        case RTL9601B_CHIP_ID:
            DIAG_UTIL_ERR_CHK(rtk_cpu_awarePortMask_get(&lPortMask), ret);
            diag_util_lPortMask2str(port_list, &lPortMask);
            diag_util_printf("CPU aware port: %s\n", port_list);
            break;
#endif

        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_cpu_get_aware_port */

/*
 * cpu set tag-format ( apollo | normal )
 */
cparser_result_t
cparser_cmd_cpu_set_tag_format_apollo_normal(
    cparser_context_t *context)
{
    rtk_cpu_tag_fmt_t mode;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    if('a' == TOKEN_CHAR(3,0))
        mode = CPU_TAG_FMT_APOLLO;
    else if('n' == TOKEN_CHAR(3,0))
        mode = CPU_TAG_FMT_NORMAL;

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(rtk_cpu_tagFormat_set(mode), ret);
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(rtk_cpu_tagFormat_set(mode), ret);
            break;
#endif

        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_cpu_set_tag_format_apollo_normal */

/*
 * cpu get tag-format
 */
cparser_result_t
cparser_cmd_cpu_get_tag_format(
    cparser_context_t *context)
{
    int32 ret;
    rtk_cpu_tag_fmt_t mode;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(rtk_cpu_tagFormat_get(&mode), ret);
            diag_util_printf("CPU tag format: %s\n\r", diagStr_cpuTagFormat[mode]);
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(rtk_cpu_tagFormat_get(&mode), ret);
            diag_util_printf("CPU tag format: %s\n\r", diagStr_cpuTagFormat[mode]);
            break;
#endif

        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_cpu_get_tag_format */

/*
 * cpu set trap-insert-tag state ( disable | enable )
 */
cparser_result_t
cparser_cmd_cpu_set_trap_insert_tag_state_disable_enable(
    cparser_context_t *context)
{
    rtk_enable_t state=DISABLED;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    if('e' == TOKEN_CHAR(4,0))
        state = ENABLED;
    else if('d' == TOKEN_CHAR(4,0))
        state = DISABLED;

    DIAG_UTIL_ERR_CHK(rtk_cpu_trapInsertTag_set(state), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_cpu_set_trap_insert_tag_enable_disable */

/*
 * cpu get trap-insert-tag state
 */
cparser_result_t
cparser_cmd_cpu_get_trap_insert_tag_state(
    cparser_context_t *context)
{
    int32 ret;
    rtk_enable_t state;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_cpu_trapInsertTag_get(&state), ret);

    diag_util_printf("CPU trap target insert tag state: %s\n\r", diagStr_enable[state]);

    return CPARSER_OK;
}    /* end of cparser_cmd_cpu_get_trap_insert_tag */

/*
 * cpu set tag-aware state ( disable | enable )
 */
cparser_result_t
cparser_cmd_cpu_set_tag_aware_state_disable_enable(
    cparser_context_t *context)
{
    rtk_enable_t state;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    if('e' == TOKEN_CHAR(4,0))
        state = ENABLED;
    else if('d' == TOKEN_CHAR(4,0))
        state = DISABLED;

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(rtk_cpu_tagAware_set(state), ret);
            break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
            DIAG_UTIL_ERR_CHK(rtk_cpu_tagAware_set(state), ret);
            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_cpu_set_tag_aware_state_disable_enable */

/*
 * cpu get tag-aware state
 */
cparser_result_t
cparser_cmd_cpu_get_tag_aware_state(
    cparser_context_t *context)
{
    int32 ret;
    rtk_enable_t state;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(rtk_cpu_tagAware_get(&state), ret);
            diag_util_printf("CPU tag aware state: %s\n\r", diagStr_enable[state]);
            break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
            DIAG_UTIL_ERR_CHK(rtk_cpu_tagAware_get(&state), ret);
            diag_util_printf("CPU tag aware state: %s\n\r", diagStr_enable[state]);
            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_cpu_get_tag_aware_state */

/*
 * cpu set trap-insert-tag port ( <PORT_LIST:ports> | all ) state ( disable | enable )
 */
cparser_result_t
cparser_cmd_cpu_set_trap_insert_tag_port_ports_all_state_disable_enable(
    cparser_context_t *context)
{
    int32               ret;
    diag_portlist_t     portlist;
    rtk_port_t          port;
    rtk_enable_t        state = DISABLED;

    DIAG_UTIL_PARAM_CHK();

    if('e' == TOKEN_CHAR(6,0))
        state = ENABLED;
    else if('d' == TOKEN_CHAR(6,0))
        state = DISABLED;
#if defined(CONFIG_SDK_RTL9607C)
    {
        rtk_portmask_t check_portmask;
        char cpuPorStr[10]="7,9,10";

        portlist.min=7;
        portlist.max=10;
        
        if('a' == TOKEN_CHAR(4,0))
        {
            DIAG_UTIL_ERR_CHK(diag_util_str2LPortMask(cpuPorStr,&portlist.portmask),ret);
        }
        else if ('c' == TOKEN_CHAR(4,0))
        {
            DIAG_UTIL_ERR_CHK(diag_util_str2LPortMask(cpuPorStr,&portlist.portmask),ret);
        }
        else
        {
            DIAG_UTIL_ERR_CHK(diag_util_str2LPortMask(TOKEN_STR(4),&portlist.portmask),ret);
            DIAG_UTIL_ERR_CHK(diag_util_str2LPortMask(cpuPorStr,&check_portmask),ret);
            DIAG_UTIL_ERR_CHK(diag_util_portMaskRangeCheck(&check_portmask,&portlist.portmask),ret);
        }
    }
#else
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_CPULIST(portlist, 4), ret);
#endif
    switch (DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
            DIAG_UTIL_PORTMASK_SCAN(portlist, port)
            {
                DIAG_UTIL_ERR_CHK(rtk_cpu_trapInsertTagByPort_set(port, state), ret);
            }
            break;
#endif

        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_cpu_set_trap_insert_tag_port_ports_all_state_disable_enable */

/*
 * cpu get trap-insert-tag port ( <PORT_LIST:ports> | all ) state
 */
cparser_result_t
cparser_cmd_cpu_get_trap_insert_tag_port_ports_all_state(
    cparser_context_t *context)
{
    int32               ret;
    diag_portlist_t     portlist;
    rtk_port_t          port;
    rtk_enable_t        state;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
#if defined(CONFIG_SDK_RTL9607C)
    {
        rtk_portmask_t check_portmask;
        char cpuPorStr[10]="7,9,10";

        portlist.min=7;
        portlist.max=10;
        
        if('a' == TOKEN_CHAR(4,0))
        {
            DIAG_UTIL_ERR_CHK(diag_util_str2LPortMask(cpuPorStr,&portlist.portmask),ret);
        }
        else if ('c' == TOKEN_CHAR(4,0))
        {
            DIAG_UTIL_ERR_CHK(diag_util_str2LPortMask(cpuPorStr,&portlist.portmask),ret);
        }
        else
        {
            DIAG_UTIL_ERR_CHK(diag_util_str2LPortMask(TOKEN_STR(4),&portlist.portmask),ret);
            DIAG_UTIL_ERR_CHK(diag_util_str2LPortMask(cpuPorStr,&check_portmask),ret);
            DIAG_UTIL_ERR_CHK(diag_util_portMaskRangeCheck(&check_portmask,&portlist.portmask),ret);
        }
    }
#else
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_CPULIST(portlist, 4), ret);
#endif
    diag_util_mprintf("Port State\n");

    switch (DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
            DIAG_UTIL_PORTMASK_SCAN(portlist, port)
            {
                DIAG_UTIL_ERR_CHK(rtk_cpu_trapInsertTagByPort_get(port, &state), ret);

                diag_util_mprintf("%-4d %-7s\n", port, diagStr_enable[state]);
            }
            break;
#endif

        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_cpu_get_trap_insert_tag_port_ports_all_state */

/*
 * cpu set tag-aware port ( <PORT_LIST:ports> | all ) state ( disable | enable )
 */
cparser_result_t
cparser_cmd_cpu_set_tag_aware_port_ports_all_state_disable_enable(
    cparser_context_t *context)
{
    int32               ret;
    diag_portlist_t     portlist;
    rtk_port_t          port;
    rtk_enable_t        state = DISABLED;

    DIAG_UTIL_PARAM_CHK();

    if('e' == TOKEN_CHAR(6,0))
        state = ENABLED;
    else if('d' == TOKEN_CHAR(6,0))
        state = DISABLED;
#if defined(CONFIG_SDK_RTL9607C)
    {
        rtk_portmask_t check_portmask;
        char cpuPorStr[10]="7,9,10";

        portlist.min=7;
        portlist.max=10;
        
        if('a' == TOKEN_CHAR(4,0))
        {
            DIAG_UTIL_ERR_CHK(diag_util_str2LPortMask(cpuPorStr,&portlist.portmask),ret);
        }
        else if ('c' == TOKEN_CHAR(4,0))
        {
            DIAG_UTIL_ERR_CHK(diag_util_str2LPortMask(cpuPorStr,&portlist.portmask),ret);
        }
        else
        {
            DIAG_UTIL_ERR_CHK(diag_util_str2LPortMask(TOKEN_STR(4),&portlist.portmask),ret);
            DIAG_UTIL_ERR_CHK(diag_util_str2LPortMask(cpuPorStr,&check_portmask),ret);
            DIAG_UTIL_ERR_CHK(diag_util_portMaskRangeCheck(&check_portmask,&portlist.portmask),ret);
        }
    }
#else
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_CPULIST(portlist, 4), ret);
#endif
    switch (DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
            DIAG_UTIL_PORTMASK_SCAN(portlist, port)
            {
                DIAG_UTIL_ERR_CHK(rtk_cpu_tagAwareByPort_set(port, state), ret);
            }
            break;
#endif

        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_cpu_set_tag_aware_port_ports_all_state_disable_enable */

/*
 * cpu get tag-aware port ( <PORT_LIST:ports> | all ) state
 */
cparser_result_t
cparser_cmd_cpu_get_tag_aware_port_ports_all_state(
    cparser_context_t *context)
{
    int32               ret;
    diag_portlist_t     portlist;
    rtk_port_t          port;
    rtk_enable_t        state;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
#if defined(CONFIG_SDK_RTL9607C)
    {
        rtk_portmask_t check_portmask;
        char cpuPorStr[10]="7,9,10";

        portlist.min=7;
        portlist.max=10;
        
        if('a' == TOKEN_CHAR(4,0))
        {
            DIAG_UTIL_ERR_CHK(diag_util_str2LPortMask(cpuPorStr,&portlist.portmask),ret);
        }
        else if ('c' == TOKEN_CHAR(4,0))
        {
            DIAG_UTIL_ERR_CHK(diag_util_str2LPortMask(cpuPorStr,&portlist.portmask),ret);
        }
        else
        {
            DIAG_UTIL_ERR_CHK(diag_util_str2LPortMask(TOKEN_STR(4),&portlist.portmask),ret);
            DIAG_UTIL_ERR_CHK(diag_util_str2LPortMask(cpuPorStr,&check_portmask),ret);
            DIAG_UTIL_ERR_CHK(diag_util_portMaskRangeCheck(&check_portmask,&portlist.portmask),ret);
        }
    }
#else
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_CPULIST(portlist, 4), ret);
#endif
    diag_util_mprintf("Port State\n");

    switch (DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
            DIAG_UTIL_PORTMASK_SCAN(portlist, port)
            {
                DIAG_UTIL_ERR_CHK(rtk_cpu_tagAwareByPort_get(port, &state), ret);

                diag_util_mprintf("%-4d %-7s\n", port, diagStr_enable[state]);
            }
            break;
#endif

        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_cpu_get_tag_aware_port_ports_all_state */
