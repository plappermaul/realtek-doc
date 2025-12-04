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
 *
 * Purpose : Definition those register command and APIs in the SDK diagnostic shell.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) register
 *
 */

/*
 * Include Files
 */
#include <common/rt_error.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <common/debug/rt_log.h>
#include <common/debug/mem.h>
#include <ioal/ioal_init.h>
#include <ioal/mem32.h>
#include <hal/chipdef/allreg.h>
#include <hal/mac/reg.h>
#include <diag_util.h>
#include <diag_om.h>
#include <parser/cparser_priv.h>
#include <diag_str.h>
#include <rtk/dot1x.h>

/*
 * dot1x init
 */
cparser_result_t
cparser_cmd_dot1x_init(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_dot1x_init(), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_dot1x_init */


/*
 * dot1x get guest-vlan
 */
cparser_result_t
cparser_cmd_dot1x_get_guest_vlan(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32 vid;
    rtk_dot1x_guestVlanBehavior_t enable;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    if((ret = rtk_dot1x_guestVlan_get(&vid)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }     
    diag_util_mprintf("Guest vlan: %d\n",vid);

    if((ret = rtk_dot1x_guestVlanBehavior_get(&enable)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }  
    diag_util_mprintf("Allow guest vlan talk to auth. DA: %s\n",diagStr_enable[enable]);


    return CPARSER_OK;
}    /* end of cparser_cmd_dot1x_get_guest_vlan */

/*
 * dot1x set guest-vlan to-auth-da ( allow | disallow )
 */
cparser_result_t
cparser_cmd_dot1x_set_guest_vlan_to_auth_da_allow_disallow(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_dot1x_guestVlanBehavior_t enable;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    if('a' == TOKEN_CHAR(4,0))
        enable = ALLOW_TO_AUTH_DA;
    else
        enable = DISALLOW_TO_AUTH_DA;
        
    if((ret = rtk_dot1x_guestVlanBehavior_set(enable)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }     

    return CPARSER_OK;
}    /* end of cparser_cmd_dot1x_set_guest_vlan_to_auth_da_allow_disallow */

/*
 * dot1x set guest-vlan vid <UINT:vid>
 */
cparser_result_t
cparser_cmd_dot1x_set_guest_vlan_vid_vid(
    cparser_context_t *context,
    uint32_t  *vid_ptr)
{
    int32 ret = RT_ERR_FAILED;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    if((ret = rtk_dot1x_guestVlan_set(*vid_ptr)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }     


    return CPARSER_OK;
}    /* end of cparser_cmd_dot1x_set_guest_vlan_vid_vid */

/*
 * dot1x get mac-based direction 
 */
cparser_result_t
cparser_cmd_dot1x_get_mac_based_direction(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_dot1x_direction_t opDir;
    DIAG_UTIL_PARAM_CHK();

    if((ret = rtk_dot1x_macBasedDirection_get(&opDir)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }   

    diag_util_mprintf("Mac-based direction: %s\n",diagStr_1xOpDir[opDir]);
    return CPARSER_OK;
}

/*
 * dot1x set mac-based direction ( both | in )
 */
cparser_result_t
cparser_cmd_dot1x_set_mac_based_direction_both_in(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_dot1x_direction_t opDir;
    DIAG_UTIL_PARAM_CHK();

    if('b' == TOKEN_CHAR(4,0))
        opDir = BOTH;
    else
        opDir = IN;
    
    if((ret = rtk_dot1x_macBasedDirection_set(opDir)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }   

    return CPARSER_OK;
}    /* end of cparser_cmd_dot1x_set_mac_based_direction_both_in */

/*
 * dot1x get mac-based port ( <PORT_LIST:ports> | all ) state 
 */
cparser_result_t
cparser_cmd_dot1x_get_mac_based_port_ports_all_state(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port;
    rtk_enable_t enable;
            
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

   
    diag_util_mprintf("Port State\n");      
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if((ret = rtk_dot1x_macBasedEnable_get(port, &enable)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }        
        diag_util_mprintf("%-4d %s\n",port, diagStr_enable[enable]);     
     
    } 

    return CPARSER_OK;
}    /* end of cparser_cmd_dot1x_set_mac_based_port_ports_all_state_disable_enable */



/*
 * dot1x set mac-based port ( <PORT_LIST:ports> | all ) state ( disable | enable )
 */
cparser_result_t
cparser_cmd_dot1x_set_mac_based_port_ports_all_state_disable_enable(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port;
    rtk_enable_t enable;
            
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if('e' == TOKEN_CHAR(6,0))
        enable = ENABLED;
    else
        enable = DISABLED;
        
    
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if((ret = rtk_dot1x_macBasedEnable_set(port,enable)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }        
     
    } 

    return CPARSER_OK;
}    /* end of cparser_cmd_dot1x_set_mac_based_port_ports_all_state_disable_enable */


/*
 * dot1x get port-based port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_dot1x_get_port_based_port_ports_all(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port;
    rtk_dot1x_direction_t authDirect;
    rtk_enable_t enable;
    rtk_dot1x_auth_status_t auth;
            
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
  
    diag_util_mprintf("Port Direction State   AuthedStatus\n");      
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if((ret = rtk_dot1x_portBasedDirection_get(port, &authDirect)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }        

        if((ret = rtk_dot1x_portBasedEnable_get(port, &enable)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }        

        if((ret = rtk_dot1x_portBasedAuthStatus_get(port, &auth)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }        

        diag_util_mprintf("%-4d %-9s %-7s %s\n",port,
                                                diagStr_1xOpDir[authDirect],
                                                diagStr_enable[enable],
                                                diagStr_authstate[auth]);     
    } 

    return CPARSER_OK;
}    /* end of cparser_cmd_dot1x_set_port_based_port_ports_all_state_disable_enable */



/*
 * dot1x set port-based port ( <PORT_LIST:ports> | all ) state ( disable | enable )
 */
cparser_result_t
cparser_cmd_dot1x_set_port_based_port_ports_all_state_disable_enable(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port;
    rtk_enable_t enable;
            
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if('e' == TOKEN_CHAR(6,0))
        enable = ENABLED;
    else
        enable = DISABLED;
        
    
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if((ret = rtk_dot1x_portBasedEnable_set(port,enable)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }        
     
    } 

    return CPARSER_OK;
}    /* end of cparser_cmd_dot1x_set_port_based_port_ports_all_state_disable_enable */

/*
 * dot1x set port-based port ( <PORT_LIST:ports> | all ) ( auth | unauth )
 */
cparser_result_t
cparser_cmd_dot1x_set_port_based_port_ports_all_auth_unauth(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port;
    rtk_dot1x_auth_status_t auth;
            
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if('a' == TOKEN_CHAR(5,0))
        auth = AUTH;
    else
        auth = UNAUTH;
        
    
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if((ret = rtk_dot1x_portBasedAuthStatus_set(port,auth)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }        
     
    } 

    return CPARSER_OK;
}    /* end of cparser_cmd_dot1x_set_port_based_port_ports_all_auth_unauth */

/*
 * dot1x set port-based port ( <PORT_LIST:ports> | all ) direction ( both | in )
 */
cparser_result_t
cparser_cmd_dot1x_set_port_based_port_ports_all_direction_both_in(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port;
    rtk_dot1x_direction_t authDirect;
            
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if('b' == TOKEN_CHAR(6,0))
        authDirect = BOTH;
    else
        authDirect = IN;
    
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if((ret = rtk_dot1x_portBasedDirection_set(port, authDirect)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }        
        
    } 

    return CPARSER_OK;
}    /* end of cparser_cmd_dot1x_set_port_based_port_ports_all_direction_both_in */

/*
 * dot1x get trap-priority
 */
cparser_result_t
cparser_cmd_dot1x_get_trap_priority(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32 pri;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    if((ret = rtk_dot1x_trapPri_get(&pri)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }     
    diag_util_mprintf("Trap priority:%d\n",pri);

    return CPARSER_OK;
}    /* end of cparser_cmd_dot1x_get_trap_priority */

/*
 * dot1x set trap-priority <UINT:priority>
 */
cparser_result_t
cparser_cmd_dot1x_set_trap_priority_priority(
    cparser_context_t *context,
    uint32_t  *priority_ptr)
{
    int32 ret = RT_ERR_FAILED;
    
    DIAG_UTIL_PARAM_CHK();

    if((ret = rtk_dot1x_trapPri_set(*priority_ptr)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }     

    return CPARSER_OK;
}    /* end of cparser_cmd_dot1x_set_trap_priority_priority */

/*
 * dot1x set unauth-packet port ( <PORT_LIST:ports> | all ) action ( drop | guest-vlan | trap-to-cpu )
 */
cparser_result_t
cparser_cmd_dot1x_set_unauth_packet_port_ports_all_action_drop_guest_vlan_trap_to_cpu(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port;
    rtk_action_t act;
            
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if('d' == TOKEN_CHAR(6,0))
        act = ACTION_DROP;
    else if('g' == TOKEN_CHAR(6,0))
        act = ACTION_TO_GUESTVLAN;
    else        
        act = ACTION_TRAP2CPU;
        
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if((ret = rtk_dot1x_unauthPacketOper_set(port,act)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }        
     
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_dot1x_set_unauth_packet_port_ports_all_action_drop_guest_vlan_trap_to_cpu */

/*
 * dot1x get unauth-packet port ( <PORT_LIST:ports> | all ) action
 */
cparser_result_t
cparser_cmd_dot1x_get_unauth_packet_port_ports_all_action(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_action_t act;
    diag_portlist_t portlist;
    rtk_port_t port;
   
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_OUTPUT_INIT();
    diag_util_mprintf("Port Unauth Action\n");      
      
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if((ret = rtk_dot1x_unauthPacketOper_get(port,&act)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }        
        diag_util_mprintf("%-4d %s\n",port,diagStr_actionStr[act]);      
     
    } 

    return CPARSER_OK;
}    /* end of cparser_cmd_dot1x_get_unauth_packet_port_ports_all_action */

