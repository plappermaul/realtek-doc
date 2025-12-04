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
 * Purpose : Definition those OAM command and APIs in the SDK diagnostic shell.
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
//#include <dal/apollo/raw/apollo_raw_oam.h>
#include <rtk/oam.h>
#include <rtk/trap.h>

/*
 * oam init
 */
cparser_result_t
cparser_cmd_oam_init(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_oam_init(), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_init */


/*
 * oam dump
 */
cparser_result_t
cparser_cmd_oam_dump(
    cparser_context_t *context)
{
    rtk_action_t action;
    uint32 priority;
    int32 ret = RT_ERR_FAILED;
    rtk_port_t port = 0;
    rtk_oam_parser_act_t actionP;
    rtk_oam_multiplexer_act_t actionM;


    DIAG_UTIL_ERR_CHK(rtk_trap_oamPduAction_get(&action), ret);
    
    diag_util_mprintf("OAM State is ");
    if(action == ACTION_FORWARD)
    {
        diag_util_mprintf("%s\n",diagStr_enable[ENABLED]);
    }
    else
    {
        diag_util_mprintf("%s\n",diagStr_enable[DISABLED]);
    }

    DIAG_UTIL_ERR_CHK(rtk_trap_oamPduPri_get(&priority), ret);

    diag_util_mprintf("OAM Trap Priority is %d\n",priority);	

    diag_util_mprintf("Port Multiplexer Parser\n"); 	

    HAL_SCAN_ALL_PORT(port)
    {    
        DIAG_UTIL_ERR_CHK(rtk_oam_multiplexerAction_get(port, &actionM), ret); 			
        DIAG_UTIL_ERR_CHK(rtk_oam_parserAction_get(port, &actionP), ret); 			
        diag_util_mprintf("%-4d %-11s %s\n", port, 
                                            diagStr_oamMuxAct[actionM],
                                            diagStr_oamParserAct[actionP]);   
    }
    
    return CPARSER_OK;
}    /* end of cparser_cmd_oam_dump */

/*
 * oam get state
 */
cparser_result_t
cparser_cmd_oam_get_state(
    cparser_context_t *context)
{
    rtk_action_t action;
    int32 ret = RT_ERR_FAILED;
        
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_trap_oamPduAction_get(&action), ret);

    diag_util_mprintf("OAM State is ");
    if(action == ACTION_FORWARD)
    {
        diag_util_mprintf("%s\n",diagStr_enable[DISABLED]);
    }
    else
    {
        diag_util_mprintf("%s\n",diagStr_enable[ENABLED]);
    }
    
    return CPARSER_OK;
}    /* end of cparser_cmd_oam_get_state */

/*
 * oam set state ( disable | enable ) 
 */
cparser_result_t
cparser_cmd_oam_set_state_disable_enable(
    cparser_context_t *context)
{
    rtk_action_t action=ACTION_FORWARD;
    int32 ret = RT_ERR_FAILED;
        
    DIAG_UTIL_PARAM_CHK();

    if('e'==TOKEN_CHAR(3,0))
        action = ACTION_TRAP2CPU;
    else if('d'==TOKEN_CHAR(3,0))
        action = ACTION_FORWARD;

    DIAG_UTIL_ERR_CHK(rtk_trap_oamPduAction_set(action), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_oam_set_state_disable_enable */

/*
 * oam get port ( <PORT_LIST:ports> | all ) state
 */
cparser_result_t
cparser_cmd_oam_get_port_ports_all_state(
    cparser_context_t* context,
    char * *port_ptr)
{
    diag_portlist_t portlist;
    rtk_action_t action;
    rtk_port_t   port;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_OUTPUT_INIT();

    /*init portlist*/    
	DIAG_UTIL_ERR_CHK(diag_util_extract_portlist("t", DIAG_PORTTYPE_ALL, &portlist), ret);
	
	if (TOKEN_CHAR(3, 0) == 'a') {
		DIAG_UTIL_ERR_CHK(diag_util_extract_portlist("t", DIAG_PORTTYPE_ALL, &portlist), ret);
	} 
	else 
	{
		DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);
	}
	
    diag_util_mprintf("Port       OAM State\n");
    DIAG_UTIL_PORTMASK_SCAN(portlist, port) {
        DIAG_UTIL_ERR_CHK(rtk_trap_portOamPduAction_get (port, &action), ret);
        diag_util_mprintf("%-10u  %s\n", port, diagStr_actionStr [action]);
    }

    return (CPARSER_OK);
}    /* end of cparser_cmd_oam_get_state */

/*
 * oam set port ( <PORT_LIST:ports> | all ) state ( disable | enable )
 */
cparser_result_t
cparser_cmd_oam_set_port_ports_all_state_disable_enable (
    cparser_context_t* context,
    char * *port_ptr)
{
    diag_portlist_t portlist;
    rtk_action_t action=ACTION_FORWARD;
    rtk_port_t   port;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    if('e' == TOKEN_CHAR(5, 0)) {
        action = ACTION_TRAP2CPU;
    } else if('d' == TOKEN_CHAR(5, 0)) {
        action = ACTION_FORWARD;
    }

    DIAG_UTIL_OUTPUT_INIT();

    /*init portlist*/    
	DIAG_UTIL_ERR_CHK(diag_util_extract_portlist("t", DIAG_PORTTYPE_ALL, &portlist), ret);


	if (TOKEN_CHAR(3, 0) == 'a') {
		DIAG_UTIL_ERR_CHK(diag_util_extract_portlist("t", DIAG_PORTTYPE_ALL, &portlist), ret);
	} 
	else 
	{
		DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);
	}
    DIAG_UTIL_PORTMASK_SCAN(portlist, port) {
        DIAG_UTIL_ERR_CHK(rtk_trap_portOamPduAction_set(port, action), ret);
    }

    return (CPARSER_OK);
}    /* end of cparser_cmd_oam_set_state_disable_enable */

/*
 * oam get multiplexer port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_oam_get_multiplexer_port_ports_all(
    cparser_context_t *context,
    char * *ports_ptr)
{
    diag_portlist_t portlist;
    rtk_port_t port;
    int32 ret = RT_ERR_FAILED;
    rtk_oam_multiplexer_act_t action;
        
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    /*init portlist*/    
	DIAG_UTIL_ERR_CHK(diag_util_extract_portlist("t", DIAG_PORTTYPE_ALL, &portlist), ret);

	
	if (TOKEN_CHAR(4, 0) == 'a') {
		DIAG_UTIL_ERR_CHK(diag_util_extract_portlist("t", DIAG_PORTTYPE_ALL, &portlist), ret);
	} 
	else 
	{
		DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
	}

    diag_util_mprintf("Port       Multiplexer Action \n"); 	
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {    
        DIAG_UTIL_ERR_CHK(rtk_oam_multiplexerAction_get(port, &action), ret); 			
        diag_util_mprintf("%-10u  %s\n", port, diagStr_oamMuxAct[action]);   
    }	


    return CPARSER_OK;
}    /* end of cparser_cmd_oam_get_multiplexer_port_ports_all */

/*
 * oam set multiplexer port ( <PORT_LIST:ports> | all ) action ( forward | discard | from-cpu-only )
 */
cparser_result_t
cparser_cmd_oam_set_multiplexer_port_ports_all_action_forward_discard_from_cpu_only(
    cparser_context_t *context,
    char * *ports_ptr)
{
    rtk_oam_multiplexer_act_t action;
    diag_portlist_t portlist;
    rtk_port_t port;
    int32 ret = RT_ERR_FAILED;
        
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    if(!osal_strcmp(TOKEN_STR(6),"forward"))
    {
        action = OAM_MULTIPLEXER_ACTION_FORWARD;
    }
    else if(!osal_strcmp(TOKEN_STR(6),"discard"))
    {
        action = OAM_MULTIPLEXER_ACTION_DISCARD;
    }
    else if(!osal_strcmp(TOKEN_STR(6),"from-cpu-only"))
    {
        action = OAM_MULTIPLEXER_ACTION_CPUONLY;
    }
    else
    {
        diag_util_printf("User config : Error!\n");
        return CPARSER_NOT_OK;
    }	

    /*init portlist*/    
	DIAG_UTIL_ERR_CHK(diag_util_extract_portlist("t", DIAG_PORTTYPE_ALL, &portlist), ret);

	
	if (TOKEN_CHAR(4, 0) == 'a') {
		
		DIAG_UTIL_ERR_CHK(diag_util_extract_portlist("t", DIAG_PORTTYPE_ALL, &portlist), ret);
	} 
	else 
	{
		DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
	}

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {    
        DIAG_UTIL_ERR_CHK(rtk_oam_multiplexerAction_set(port, action), ret); 			
        diag_util_mprintf("%-10u  %s\n", port, diagStr_oamMuxAct[action]);   
    }	

    return CPARSER_OK;
}    /* end of cparser_cmd_oam_set_multiplexer_port_ports_all_action_forward_discard_from_cpu_only */

/*
 * oam get parser port ( <PORT_LIST:port> | all )
 */
cparser_result_t
cparser_cmd_oam_get_parser_port_port_all(
    cparser_context_t *context,
    char * *port_ptr)
{
    rtk_oam_parser_act_t action;
    diag_portlist_t portlist;
    rtk_port_t port;
    int32 ret = RT_ERR_FAILED;
        
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    /*init portlist*/    
	DIAG_UTIL_ERR_CHK(diag_util_extract_portlist("t", DIAG_PORTTYPE_ALL, &portlist), ret);

	
	if (TOKEN_CHAR(4, 0) == 'a') {
		DIAG_UTIL_ERR_CHK(diag_util_extract_portlist("t", DIAG_PORTTYPE_ALL, &portlist), ret);
	} 
	else 
	{
		DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
	}
    diag_util_mprintf("Get OAM Parser\n"); 
    diag_util_mprintf("Port       Parser Action \n"); 	
    diag_util_mprintf("-----------------------------\n"); 	
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {    
        DIAG_UTIL_ERR_CHK(rtk_oam_parserAction_get(port, &action), ret); 			
        diag_util_mprintf("%-10u  %s\n", port, diagStr_oamParserAct[action]);   
    }	

    return CPARSER_OK;
}    /* end of cparser_cmd_oam_get_parser_port_port_all */

/*
 * oam set parser port ( <PORT_LIST:port> | all ) action ( forward | loopback | discard )
 */
cparser_result_t
cparser_cmd_oam_set_parser_port_port_all_action_forward_loopback_discard(
    cparser_context_t *context,
    char * *port_ptr)
{
    rtk_oam_parser_act_t action;
    diag_portlist_t portlist;
    rtk_port_t port;
    int32 ret = RT_ERR_FAILED;
        
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    if ('f' == TOKEN_CHAR(6,0))
    {
        action = OAM_PARSER_ACTION_FORWARD;
    }
    else if ('l' == TOKEN_CHAR(6,0))
    {
        action = OAM_PARSER_ACTION_LOOPBACK;
    }
    else if ('d' == TOKEN_CHAR(6,0))
    {
        action = OAM_PARSER_ACTION_DISCARD;
    }	
    else
    {
        diag_util_printf("User config : Error!\n");
        return CPARSER_NOT_OK;
    }	

    /*init portlist*/    
	DIAG_UTIL_ERR_CHK(diag_util_extract_portlist("t", DIAG_PORTTYPE_ALL, &portlist), ret);

	
	if (TOKEN_CHAR(4, 0) == 'a') {
		DIAG_UTIL_ERR_CHK(diag_util_extract_portlist("t", DIAG_PORTTYPE_ALL, &portlist), ret);

	} 
	else 
	{
		DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
	}
    diag_util_mprintf("Set OAM Parser\n"); 
    diag_util_mprintf("Port       Parser Action \n"); 	
    diag_util_mprintf("-----------------------------\n"); 	
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {    
        DIAG_UTIL_ERR_CHK(rtk_oam_parserAction_set(port, action), ret); 			
        diag_util_mprintf("%-10u  %s\n", port, diagStr_oamParserAct[action]);   
    }	


    return CPARSER_OK;
}    /* end of cparser_cmd_oam_set_parser_port_port_all_action_forward_loopback_discard */

/*
 * oam set trap-priority <UINT:priority>
 */
cparser_result_t
cparser_cmd_oam_set_trap_priority_priority(
    cparser_context_t *context,
    uint32_t  *priority_ptr)
{
    int32 ret = RT_ERR_FAILED;
        
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_trap_oamPduPri_set(*priority_ptr), ret);


    return CPARSER_OK;
}    /* end of cparser_cmd_oam_set_trap_priority_priority */

/*
 * oam get trap-priority
 */
cparser_result_t
cparser_cmd_oam_get_trap_priority(
    cparser_context_t *context)
{
    uint32 priority;
    int32 ret = RT_ERR_FAILED;
        
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_trap_oamPduPri_get(&priority), ret);

    diag_util_mprintf("OAM Trap Priority is %d\n",priority);	


    return CPARSER_OK;
}    /* end of cparser_cmd_oam_get_trap_priority */

