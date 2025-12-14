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
 * Purpose : Definition those RLDP command and APIs in the SDK diagnostic shell.
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
#include <rtk/rldp.h>
#include <hal/mac/reg.h>


#ifdef CONFIG_SDK_APOLLOMP
#include <hal/chipdef/apollomp/rtk_apollomp_reg_struct.h>
#endif

/*
 * rldp init
 */
cparser_result_t
cparser_cmd_rldp_init(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(rtk_rldp_init(), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rldp_init */

/*
 * rldp set state ( disable | enable )
 */
cparser_result_t
cparser_cmd_rldp_set_state_disable_enable(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_rldp_config_t config;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(rtk_rldp_config_get(&config), ret);
    diag_util_printf("RLDP: %s ->",
        diagStr_enable[(config.rldp_enable == ENABLED) ? ENABLED : DISABLED]);

    if('e' == TOKEN_CHAR(3,0))
        config.rldp_enable = ENABLED;
    else if('d' == TOKEN_CHAR(3,0))
        config.rldp_enable = DISABLED;
    else
        return CPARSER_NOT_OK;

    diag_util_printf(" %s\n",
        diagStr_enable[(config.rldp_enable == ENABLED) ? ENABLED : DISABLED]);
    DIAG_UTIL_ERR_CHK(rtk_rldp_config_set(&config), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rldp_set_state_disable_enable */

/*
 * rldp get state
 */
cparser_result_t
cparser_cmd_rldp_get_state(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_rldp_config_t config;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(rtk_rldp_config_get(&config), ret); 	
    diag_util_printf("RLDP %s\n",
        diagStr_enable[(config.rldp_enable == ENABLED) ? ENABLED : DISABLED]);

    return CPARSER_OK;
}    /* end of cparser_cmd_rldp_get_state */

/*
 * rldp set bypass-flow-control state ( disable | enable )
 */
cparser_result_t
cparser_cmd_rldp_set_bypass_flow_control_state_disable_enable(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_rldp_config_t config;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(rtk_rldp_config_get(&config), ret);
    diag_util_printf("RLDP Bypass: %s ->",
        diagStr_enable[(config.fc_mode == RTK_RLDP_FLOWCTRL_BYPASS) ? ENABLED : DISABLED]);

    if('e' == TOKEN_CHAR(4,0))
        config.fc_mode = RTK_RLDP_FLOWCTRL_BYPASS;
    else if('d' == TOKEN_CHAR(4,0))
        config.fc_mode = RTK_RLDP_FLOWCTRL_DROP;
    else
        return CPARSER_NOT_OK;

    diag_util_printf("%s\n",
        diagStr_enable[(config.fc_mode == RTK_RLDP_FLOWCTRL_BYPASS) ? ENABLED : DISABLED]);
    DIAG_UTIL_ERR_CHK(rtk_rldp_config_set(&config), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rldp_set_bypass_flow_control_state_disable_enable */

/*
 * rldp get bypass-flow-control state
 */
cparser_result_t
cparser_cmd_rldp_get_bypass_flow_control_state(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_rldp_config_t config;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(rtk_rldp_config_get(&config), ret); 	
    diag_util_printf("RLDP Bypass: %s\n",
        diagStr_enable[(config.fc_mode == RTK_RLDP_FLOWCTRL_BYPASS) ? ENABLED : DISABLED]);

    return CPARSER_OK;
}    /* end of cparser_cmd_rldp_get_bypass_flow_control_state */

/*
 * rldp set mode ( sa-moving | periodic )
 */
cparser_result_t
cparser_cmd_rldp_set_mode_sa_moving_periodic(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
#if defined(CONFIG_SDK_APOLLOMP)
    uint32 apollomp_trigger = 0;
#endif

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();	

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            if('s' == TOKEN_CHAR(3,0))
                DIAG_UTIL_ERR_CHK(apollo_raw_rldp_trigger_set(RAW_RLDP_TRIG_SAMOVING), ret); 
            else if ('p' == TOKEN_CHAR(3,0))
                DIAG_UTIL_ERR_CHK(apollo_raw_rldp_trigger_set(RAW_RLDP_TRIG_PERIODIC), ret); 
            else
                return CPARSER_NOT_OK;
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            if('s' == TOKEN_CHAR(3,0))
                apollomp_trigger = 0; /* SA Moving */
            else if ('p' == TOKEN_CHAR(3,0))
                apollomp_trigger = 1; /* Peroidical */
            else
                return CPARSER_NOT_OK;
            
            DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_RLDP_CTRL_1r, APOLLOMP_TRIG_MODEf, &apollomp_trigger), ret); 

            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_rldp_set_mode_sa_moving_periodic */

/*
 * rldp get mode
 */
cparser_result_t
cparser_cmd_rldp_get_mode(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
#if defined(CONFIG_SDK_APOLLO)
    apollo_raw_rldp_triggerMode_t apollo_trigger = 0;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
    uint32 apollomp_trigger = 0;
#endif

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollo_raw_rldp_trigger_get(&apollo_trigger), ret); 
            diag_util_mprintf("RLDP Mode: %s\n",
                (RAW_RLDP_TRIG_SAMOVING == apollo_trigger) ? "SA-Moving" : "Peroidical");
           break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(APOLLOMP_RLDP_CTRL_1r, APOLLOMP_TRIG_MODEf, &apollomp_trigger), ret); 

            diag_util_mprintf("RLDP Mode: %s\n",
                (0 /* SA Moving*/ == apollomp_trigger) ? "SA-Moving" : "Peroidical");
            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
    
    return CPARSER_OK;
}    /* end of cparser_cmd_rldp_get_mode */

/*
 * rldp set magic <MACADDR:mac>
 */
cparser_result_t
cparser_cmd_rldp_set_magic_mac(
    cparser_context_t *context,
    cparser_macaddr_t  *mac_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_rldp_config_t config;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(rtk_rldp_config_get(&config), ret);
    diag_util_printf("RLDP Magic: %s ->",
        diag_util_inet_mactoa(&config.magic.octet[0]));
    osal_memcpy(&(config.magic.octet), mac_ptr->octet, ETHER_ADDR_LEN);

    diag_util_printf(" %s\n",
        diag_util_inet_mactoa(&config.magic.octet[0]));
    DIAG_UTIL_ERR_CHK(rtk_rldp_config_set(&config), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rldp_set_magic_mac */

/*
 * rldp get magic
 */
cparser_result_t
cparser_cmd_rldp_get_magic(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_rldp_config_t config;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(rtk_rldp_config_get(&config), ret); 	
    diag_util_printf("RLDP Magic: %s\n",
        diag_util_inet_mactoa(&config.magic.octet[0]));

    return CPARSER_OK;
}    /* end of cparser_cmd_rldp_get_magic */

/*
 * rldp get identifier
 */
cparser_result_t
cparser_cmd_rldp_get_identifier(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_rldp_status_t status;

    DIAG_UTIL_PARAM_CHK();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;			
#endif
        default:
            DIAG_UTIL_ERR_CHK(rtk_rldp_status_get(&status), ret); 	
            diag_util_printf("RLDP Identifier: %s\n",
                diag_util_inet_mactoa(&status.id.octet[0]));
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_rldp_get_identifier */

/*
 * rldp set compare-type ( magic-and-identifier | magic-only )
 */
cparser_result_t
cparser_cmd_rldp_set_compare_type_magic_and_identifier_magic_only(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_rldp_config_t config;
    
    DIAG_UTIL_PARAM_CHK();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;			
#endif
        default:
            DIAG_UTIL_ERR_CHK(rtk_rldp_config_get(&config), ret);
            diag_util_printf("RLDP Compare Type: %s ->",
                (config.compare_type == RTK_RLDP_CMPTYPE_MAGIC) ? "Magic Number" : "Magic Number + Identifier");
        
            if('a' == TOKEN_CHAR(3,6))
                config.compare_type = RTK_RLDP_CMPTYPE_MAGIC_ID;
            else if('o' == TOKEN_CHAR(3,6))
                config.compare_type = RTK_RLDP_CMPTYPE_MAGIC;
            else
                return CPARSER_NOT_OK;
        
            diag_util_printf(" %s\n",
                (config.compare_type == RTK_RLDP_CMPTYPE_MAGIC) ? "Magic Number" : "Magic Number + Identifier");
            DIAG_UTIL_ERR_CHK(rtk_rldp_config_set(&config), ret);
            break;
    }
	
    return CPARSER_OK;
}    /* end of cparser_cmd_rldp_set_compare_type_magic_and_identifier_magic_only */

/*
 * rldp get compare-type
 */
cparser_result_t
cparser_cmd_rldp_get_compare_type(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_rldp_config_t config;

    DIAG_UTIL_PARAM_CHK();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;			
#endif
        default:
            DIAG_UTIL_ERR_CHK(rtk_rldp_config_get(&config), ret); 	
            diag_util_printf("RLDP Compare Type: %s\n",
                (config.compare_type == RTK_RLDP_CMPTYPE_MAGIC) ? "Magic Number" : "Magic Number + Identifier");
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_rldp_get_compare_type */

/*
 * rldp set handle ( hardware | software )
 */
cparser_result_t
cparser_cmd_rldp_set_handle_hardware_software(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
#if defined(CONFIG_SDK_APOLLOMP)
    uint32 apollomp_handle = 0;
#endif

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();	

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            if('h' == TOKEN_CHAR(3,0))
                DIAG_UTIL_ERR_CHK(apollo_raw_rldp_handle_set(RAW_RLDP_HANDLE_ASIC), ret); 
            else if ('s' == TOKEN_CHAR(3,0))
                DIAG_UTIL_ERR_CHK(apollo_raw_rldp_handle_set(RAW_RLDP_HANDLE_CPU), ret); 
            else
                return CPARSER_NOT_OK;		
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            if('h' == TOKEN_CHAR(3,0))
                apollomp_handle = 1; /* ASIC process */
            else if ('s' == TOKEN_CHAR(3,0))
                apollomp_handle = 0; /* CPU process */
            else
                return CPARSER_NOT_OK;		
            DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_RLDP_CTRL_1r, APOLLOMP_CPU_HANDLEf, &apollomp_handle), ret); 

            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_rldp_set_handle_hardware_software */

/*
 * rldp get handle
 */
cparser_result_t
cparser_cmd_rldp_get_handle(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
#if defined(CONFIG_SDK_APOLLO)
    apollo_raw_rldp_handle_t apollo_handle = 0;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
    uint32 apollomp_handle = 0;
#endif

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollo_raw_rldp_handle_get(&apollo_handle), ret); 
            diag_util_mprintf("RLDP Handle: %s\n",
                (RAW_RLDP_HANDLE_ASIC == apollo_handle) ? "Hardware" : "Software");
           break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(APOLLOMP_RLDP_CTRL_1r, APOLLOMP_CPU_HANDLEf, &apollomp_handle), ret); 

            diag_util_mprintf("RLDP Handle: %s\n",
                (1 /* ASIC process */ == apollomp_handle) ? "Hardware" : "Software");
            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
    
    return CPARSER_OK;
}    /* end of cparser_cmd_rldp_get_handle */

/*
 * rldp set re-generate-identifier
 */
cparser_result_t
cparser_cmd_rldp_set_re_generate_identifier(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
#if defined(CONFIG_SDK_APOLLOMP)
    uint32 gen = 1;
#endif

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();	

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollo_raw_rldp_genRandom_set(ENABLED), ret); 
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_RLDP_CTRL_1r, APOLLOMP_GEN_RNDMf, &gen), ret);

            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_rldp_set_re_generate_identifier */


/*
 * rldp set ( check | loop ) period <UINT:time>
 */
cparser_result_t
cparser_cmd_rldp_set_check_loop_period_time(
    cparser_context_t *context,
    uint32_t  *time_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_rldp_config_t config;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(rtk_rldp_config_get(&config), ret);

    if('c' == TOKEN_CHAR(2,0))
    {
        diag_util_printf("RLDP check period: %d ms -> %d ms\n",
            config.interval_check, *time_ptr);
        config.interval_check = *time_ptr;
    }
    else if('l' == TOKEN_CHAR(2,0))
    {
        diag_util_printf("RLDP loop period: %d ms -> %d ms\n",
            config.interval_loop, *time_ptr);
        config.interval_loop = *time_ptr;
    }
    else
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_rldp_config_set(&config), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rldp_set_check_loop_period_time */

/*
 * rldp get ( check | loop ) period
 */
cparser_result_t
cparser_cmd_rldp_get_check_loop_period(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_rldp_config_t config;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(rtk_rldp_config_get(&config), ret); 	

    if('c' == TOKEN_CHAR(2,0))
        diag_util_printf("RLDP check period: %d ms\n", config.interval_check);
    else if('l' == TOKEN_CHAR(2,0))
        diag_util_printf("RLDP loop period: %d ms\n", config.interval_loop);
    else
        return CPARSER_NOT_OK;

    return CPARSER_OK;
}    /* end of cparser_cmd_rldp_get_check_loop_period */

/*
 * rldp set ( check | loop ) number <UINT:count>
 */
cparser_result_t
cparser_cmd_rldp_set_check_loop_number_count(
    cparser_context_t *context,
    uint32_t  *count_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_rldp_config_t config;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(rtk_rldp_config_get(&config), ret);

    if('c' == TOKEN_CHAR(2,0))
    {
        diag_util_printf("RLDP check number count: %d -> %d\n",
            config.num_check, *count_ptr);
        config.num_check = *count_ptr;
    }
    else if('l' == TOKEN_CHAR(2,0))
    {
        diag_util_printf("RLDP loop number count: %d -> %d\n",
            config.num_loop, *count_ptr);
        config.num_loop = *count_ptr;
    }
    else
    {
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_rldp_config_set(&config), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rldp_set_check_loop_number_count */

/*
 * rldp get ( check | loop ) number
 */
cparser_result_t
cparser_cmd_rldp_get_check_loop_number(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_rldp_config_t config;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(rtk_rldp_config_get(&config), ret); 	

    if('c' == TOKEN_CHAR(2,0))
        diag_util_printf("RLDP check number count: %d\n", config.num_check);
    else if('l' == TOKEN_CHAR(2,0))
        diag_util_printf("RLDP loop number count: %d\n", config.num_loop);
    else
        return CPARSER_NOT_OK;

    return CPARSER_OK;
}    /* end of cparser_cmd_rldp_get_check_loop_number */

/*
 * rldp set port ( <PORT_LIST:port> | all ) state ( disable | enable )
 */
cparser_result_t
cparser_cmd_rldp_set_port_port_all_state_disable_enable(
    cparser_context_t *context,
    char * *port_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint32 state;
    rtk_port_t port;
    diag_portlist_t portlist;
    rtk_rldp_portConfig_t config;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    if('e' == TOKEN_CHAR(5,0))
        state = ENABLED;
    else if('d' == TOKEN_CHAR(5,0))
        state = DISABLED;
    else
        return CPARSER_NOT_OK;

    diag_util_mprintf("Set RLDP port state\n");
    diag_util_mprintf("Port state\n");
    diag_util_mprintf("---- ------------------\n");

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_rldp_portConfig_get(port, &config);
        if(ret == RT_ERR_PORT_ID) {
            continue;
        } else if(ret != RT_ERR_OK) {
            return (CPARSER_NOT_OK);
        }

        diag_util_printf("%-4u %s ->", port,
            diagStr_enable[(config.tx_enable == ENABLED) ? ENABLED : DISABLED]);
        config.tx_enable = state;
        diag_util_mprintf(" %s\n",
            diagStr_enable[(state == ENABLED) ? ENABLED : DISABLED]);
        DIAG_UTIL_ERR_CHK(rtk_rldp_portConfig_set(port, &config), ret);
    }	

    return CPARSER_OK;
}    /* end of cparser_cmd_rldp_set_port_port_all_state_disable_enable */

/*
 * rldp get port ( <PORT_LIST:port> | all ) state
 */
cparser_result_t
cparser_cmd_rldp_get_port_port_all_state(
    cparser_context_t *context,
    char * *port_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_port_t port;
    diag_portlist_t portlist;
    rtk_rldp_portConfig_t config;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    diag_util_mprintf("Get RLDP port state\n");
    diag_util_mprintf("Port state\n");
    diag_util_mprintf("---- --------\n");

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_rldp_portConfig_get(port, &config);
        if(ret == RT_ERR_PORT_ID) {
            continue;
        } else if (ret != RT_ERR_OK) {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
        diag_util_mprintf("%-4u %s\n", port,
            diagStr_enable[(config.tx_enable == ENABLED) ? ENABLED : DISABLED]);
    }	

    return CPARSER_OK;
}    /* end of cparser_cmd_rldp_get_port_port_all_state */

/*
 * rldp clear port ( <PORT_LIST:port> | all ) status ( entering | leaving | all )
 */
cparser_result_t
cparser_cmd_rldp_clear_port_port_all_status_entering_leaving_all (
    cparser_context_t *context,
    char * *port_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_port_t port;
    diag_portlist_t portlist;
    rtk_rldp_portStatus_t status;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);
    osal_memset(&status, 0, sizeof(rtk_rldp_portStatus_t));

    if('e' == TOKEN_CHAR(5,0)) {
        status.loop_enter = RTK_RLDP_LOOPSTS_LOOPING;
    } else if('l' == TOKEN_CHAR(5,0)) {
        status.loop_leave = RTK_RLDP_LOOPSTS_LOOPING;
    } else if ('a' == TOKEN_CHAR (5, 0)) {
        status.loop_enter = RTK_RLDP_LOOPSTS_LOOPING;
        status.loop_leave = RTK_RLDP_LOOPSTS_LOOPING;
    } else {
        return (CPARSER_NOT_OK);
    }

    diag_util_mprintf("Clear RLDP port looped state\n");
    diag_util_mprintf("Port enter leave\n");
    diag_util_mprintf("---- ----- ----- \n");
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {    
        ret = rtk_rldp_portStatus_clear(port, &status);
        if(ret == RT_ERR_PORT_ID) {
            continue;
        } else if(ret != RT_ERR_OK) {
            DIAG_ERR_PRINT(ret);
            return (CPARSER_NOT_OK);
        }
        diag_util_mprintf("%-4u   %c     %c\n", port,
            (status.loop_enter == RTK_RLDP_LOOPSTS_NONE) ? 'X' : 'V',
            (status.loop_leave == RTK_RLDP_LOOPSTS_NONE) ? 'X' : 'V');
    }	

    return CPARSER_OK;
}    /* end of cparser_cmd_rldp_clear_port_port_all_status_entering_leaving */

/*
 * rldp get port ( <PORT_LIST:port> | all ) status
 */
cparser_result_t
cparser_cmd_rldp_get_port_port_all_status(
    cparser_context_t *context,
    char * *port_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_port_t port;
    diag_portlist_t portlist;
    rtk_rldp_portStatus_t status;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    diag_util_mprintf("Get RLDP port looped state\n");
    diag_util_mprintf("Port looping enter leave\n");
    diag_util_mprintf("---- ------- ----- ----- \n");

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9602C) || defined(CONFIG_SDK_RTL9607C) || defined(CONFIG_SDK_RTL9603CVD)
        case RTL9602C_CHIP_ID:
        case RTL9607C_CHIP_ID:
        case RTL9603CVD_CHIP_ID:
            DIAG_UTIL_PORTMASK_SCAN(portlist, port)
            {    
                ret = rtk_rldp_portStatus_get (port, &status);
                if(ret == RT_ERR_PORT_ID) {
                    continue;
                } else if(ret != RT_ERR_OK) {
                    DIAG_ERR_PRINT(ret);
                    return (CPARSER_NOT_OK);
                }

                diag_util_mprintf("%-4u    %c      %c     %c\n", port,
                    (status.loop_status == RTK_RLDP_LOOPSTS_NONE) ? 'X' : 'V',
                    (status.loop_enter == RTK_RLDP_LOOPSTS_NONE) ? 'X' : 'V',
                    (status.loop_leave == RTK_RLDP_LOOPSTS_NONE) ? 'X' : 'V');
            }	
            break;			
#endif
        default:
            DIAG_UTIL_PORTMASK_SCAN(portlist, port)
            {    
                DIAG_UTIL_ERR_CHK(rtk_rldp_portStatus_get(port, &status), ret);
                diag_util_mprintf("%-4u    %c      %c     %c\n", port,
                    (status.loop_status == RTK_RLDP_LOOPSTS_NONE) ? ' ' : 'V',
                    (status.loop_enter == RTK_RLDP_LOOPSTS_NONE) ? ' ' : 'V',
                    (status.loop_leave == RTK_RLDP_LOOPSTS_NONE) ? ' ' : 'V');
            }	
            break;
    }



    return CPARSER_OK;
}    /* end of cparser_cmd_rldp_get_port_port_all_status */

/*
 * rldp set port ( <PORT_LIST:port> | all ) control-state ( none-looping | looping )
 */
cparser_result_t
cparser_cmd_rldp_set_port_port_all_control_state_none_looping_looping(
    cparser_context_t *context,
    char * *port_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_port_t port;
    diag_portlist_t portlist;
#if defined(CONFIG_SDK_APOLLO)
    apollo_raw_rldp_loopState_t apollo_loopState = 0;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
    uint32 apollomp_loopState = 0;
#endif

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();	
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            if('n' == TOKEN_CHAR(5,0))
                apollo_loopState = RAW_RLDP_NORMAL;
            else if ('l' == TOKEN_CHAR(5,0))
                apollo_loopState = RAW_RLDP_LOOPING;
            else
                return CPARSER_NOT_OK;
            
            DIAG_UTIL_PORTMASK_SCAN(portlist, port)
            {    
                DIAG_UTIL_ERR_CHK(apollo_raw_rldp_portForceLoopState_set(port, apollo_loopState), ret); 
            }
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            if('n' == TOKEN_CHAR(5,0))
                apollomp_loopState = 0; /* Not looping */
            else if ('l' == TOKEN_CHAR(5,0))
                apollomp_loopState = 1; /* Looping */
            else
                return CPARSER_NOT_OK;		

            DIAG_UTIL_PORTMASK_SCAN(portlist, port)
            {
                DIAG_UTIL_ERR_CHK(reg_array_field_write(APOLLOMP_RLDP_PORT_CPU_LP_STSr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_STSf, &apollomp_loopState), ret); 
            }
            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_rldp_set_port_port_all_control_state_none_looping_looping */

/*
 * rldp get port ( <PORT_LIST:port> | all ) control-state
 */
cparser_result_t
cparser_cmd_rldp_get_port_port_all_control_state(
    cparser_context_t *context,
    char * *port_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_port_t port;
    diag_portlist_t portlist;
#if defined(CONFIG_SDK_APOLLO)
    apollo_raw_rldp_loopState_t apollo_loopState = 0;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
    uint32 apollomp_loopState = 0;
#endif

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    diag_util_mprintf("RLDP port control-state\n");
    diag_util_mprintf("Port state\n");
    diag_util_mprintf("---- ------------\n");

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        switch(DIAG_UTIL_CHIP_TYPE)
        {
#if defined(CONFIG_SDK_APOLLO)
            case APOLLO_CHIP_ID:
                DIAG_UTIL_ERR_CHK(apollo_raw_rldp_portForceLoopState_get(port, &apollo_loopState), ret); 
                diag_util_mprintf("%4d %s\n", port,
                    (RAW_RLDP_NORMAL == apollo_loopState) ? "None-looping" : "Looping");
               break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
            case APOLLOMP_CHIP_ID:
                DIAG_UTIL_ERR_CHK(reg_array_field_read(APOLLOMP_RLDP_PORT_CPU_LP_STSr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_STSf, &apollomp_loopState), ret); 

                diag_util_mprintf("%4d: %s\n", port,
                    (0 /* Not looping */ == apollomp_loopState) ? "None-looping" : "Looping");
                break;
#endif
            default:
                diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
                return CPARSER_NOT_OK;
                break;
        }
    }
    
    return CPARSER_OK;
}    /* end of cparser_cmd_rldp_get_port_port_all_control_state */

/*
 * rldp get port ( <PORT_LIST:port> | all ) looped-port-id
 */
cparser_result_t
cparser_cmd_rldp_get_port_port_all_looped_port_id(
    cparser_context_t *context,
    char * *port_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_port_t port;
    diag_portlist_t portlist;
    rtk_rldp_portStatus_t status;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    diag_util_mprintf("Get RLDP port looped port id\n");
    diag_util_mprintf("Port looped port\n");
    diag_util_mprintf("---- ----------- \n");

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        ret = rtk_rldp_portStatus_get(port, &status);
        if(ret == RT_ERR_PORT_ID) {
            continue;
        } else if(ret != RT_ERR_OK) {
            DIAG_ERR_PRINT(ret);
            return (CPARSER_NOT_OK);
        }
        diag_util_mprintf("%-4u %11d\n", port, status.looped_port);
    }	

    return CPARSER_OK;
}    /* end of cparser_cmd_rldp_get_port_port_all_looped_port_id */

/*
 * rlpp set trap state ( disable | enable )
 */
cparser_result_t
cparser_cmd_rlpp_set_trap_state_disable_enable(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
            
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();	
    
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            if('e' == TOKEN_CHAR(4,0))
                DIAG_UTIL_ERR_CHK(apollo_raw_rlpp_trap_set(ENABLED), ret); 
            else if ('d' == TOKEN_CHAR(4,0))
                DIAG_UTIL_ERR_CHK(apollo_raw_rlpp_trap_set(DISABLED), ret); 
            else
                return CPARSER_NOT_OK;		
            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_rlpp_set_trap_state_disable_enable */

/*
 * rlpp get trap state
 */
cparser_result_t
cparser_cmd_rlpp_get_trap_state(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
#if defined(CONFIG_SDK_APOLLO)
    rtk_enable_t apollo_enable;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
    rtk_rlpp_trapType_t apollomp_type;
#endif
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollo_raw_rlpp_trap_get(&apollo_enable), ret);
            diag_util_mprintf("RLPP Trap to CPU: %s\n", diagStr_enable[apollo_enable]);
            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
    
    return CPARSER_OK;
}    /* end of cparser_cmd_rlpp_get_trap_state */

