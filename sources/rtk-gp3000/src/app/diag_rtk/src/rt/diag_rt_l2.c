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
 * $Revision: $
 * $Date: $
 *
 * Purpose : Definition those XXX command and APIs in the SDK diagnostic shell.
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
#include <rtk/rt/rt_l2.h>


/*
 * rt_l2-table init
 */
cparser_result_t
cparser_cmd_rt_l2_table_init(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rt_l2_init(), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_l2_table_init */

/*
 * rt_l2-table get limit-learning port ( <PORT_LIST:ports> | all ) count
 */
cparser_result_t
cparser_cmd_rt_l2_table_get_limit_learning_port_ports_all_count(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rt_port_t port;
    diag_portlist_t portlist;
    uint32 cnt;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rt_l2_portLimitLearningCnt_get(port, &cnt), ret);
        diag_util_printf("\n Port %d learning limit: %d", port, cnt);
    }

    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_l2_table_get_limit_learning_port_ports_all_count */

/*
 * rt_l2-table set limit-learning port ( <PORT_LIST:ports> | all ) count <INT:count>
 */
cparser_result_t
cparser_cmd_rt_l2_table_set_limit_learning_port_ports_all_count_count(
    cparser_context_t *context,
    char * *ports_ptr,
    int32_t  *count_ptr)
{
    int32 ret;
    rt_port_t port;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rt_l2_portLimitLearningCnt_set(port, *count_ptr), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_l2_table_set_limit_learning_port_ports_all_count_count */

/*
 * rt_l2-table get limit-learning port ( <PORT_LIST:ports> | all ) action
 */
cparser_result_t
cparser_cmd_rt_l2_table_get_limit_learning_port_ports_all_action(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rt_port_t port;
    diag_portlist_t portlist;
    rt_l2_limitLearnCntAction_t act;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rt_l2_portLimitLearningCntAction_get(port, &act), ret);
        if(act == RT_L2_LIMIT_LEARN_CNT_ACTION_DROP)
            diag_util_printf("\n Port %d learning limit exceed action: %s", port, "Drop");
        else if(act == RT_L2_LIMIT_LEARN_CNT_ACTION_FORWARD)
            diag_util_printf("\n Port %d learning limit exceed action: %s", port, "Forward");
        else
            diag_util_printf("\n Port %d learning limit exceed action: %s", port, "Trap");
    }

    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_l2_table_get_limit_learning_port_ports_all_action */

/*
 * rt_l2-table set limit-learning port ( <PORT_LIST:ports> | all ) action ( drop | forward | trap )
 */
cparser_result_t
cparser_cmd_rt_l2_table_set_limit_learning_port_ports_all_action_drop_forward_trap(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rt_port_t port;
    diag_portlist_t portlist;
    rt_l2_limitLearnCntAction_t act;

    DIAG_UTIL_PARAM_CHK();
    
     if('d' == TOKEN_CHAR(6, 0))
        act = RT_L2_LIMIT_LEARN_CNT_ACTION_DROP;
    else if('f' == TOKEN_CHAR(6, 0))
        act = RT_L2_LIMIT_LEARN_CNT_ACTION_FORWARD;
    else
        act = RT_L2_LIMIT_LEARN_CNT_ACTION_TRAP;

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rt_l2_portLimitLearningCntAction_set(port, act), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_l2_table_set_limit_learning_port_ports_all_action_drop_forward_trap */

/*
 * rt_l2-table get unknown-sa port ( <PORT_LIST:ports> | all ) action
 */
cparser_result_t
cparser_cmd_rt_l2_table_get_unknown_sa_port_ports_all_action(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rtk_port_t port;
    diag_portlist_t portlist;
    rt_action_t action;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rt_l2_newMacOp_get(port, &action), ret);
        diag_util_printf("\n Port %d unknown SA Action: %s", port, diagStr_actionStr[action]);
    }
    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_l2_table_get_unknown_sa_port_ports_all_action */

/*
 * rt_l2-table set unknown-sa port ( <PORT_LIST:ports> | all ) action ( drop | forward )
 */
cparser_result_t
cparser_cmd_rt_l2_table_set_unknown_sa_port_ports_all_action_drop_forward(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rtk_port_t port;
    diag_portlist_t portlist;
    rt_action_t action;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if('d' == TOKEN_CHAR(6, 0))
        action = ACTION_DROP;
    else if('f' == TOKEN_CHAR(6, 0))
        action = ACTION_FORWARD;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rt_l2_newMacOp_set(port, action), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_l2_table_set_unknown_sa_port_ports_all_action_drop_forward */

static int32
_display_l2Ucast_entry(rt_l2_ucastAddr_t *pL2Addr)
{

        diag_util_mprintf("MACAddress         Spa    Age    vid Static Filter\n");
        diag_util_mprintf("----------------- ---- ------ ------ ------ ------\n");
        diag_util_mprintf("%-17s %4d %6d %6d %6s %6s\n"
                                    ,diag_util_inet_mactoa(&pL2Addr->mac.octet[0])
                                    ,pL2Addr->port
                                    ,pL2Addr->age
                                    ,pL2Addr->vid
                                    ,(pL2Addr->staticFlag == ENABLED)?"En":"Dis"
                                    ,(pL2Addr->filterFlag == ENABLED)?"En":"Dis");

    return RT_ERR_OK;
}

/*
 * rt_l2-table add mac-ucast vid  <UINT:vid> mac-address <MACADDR:mac> spn <UINT:port>
 */
cparser_result_t
cparser_cmd_rt_l2_table_add_mac_ucast_vid_vid_mac_address_mac_spn_port(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    cparser_macaddr_t  *mac_ptr,
    uint32_t  *port_ptr)
{
    int32 ret;
    rt_l2_ucastAddr_t l2Addr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&l2Addr, 0x00, sizeof(rt_l2_ucastAddr_t));
    osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    l2Addr.vid = *vid_ptr;

    ret = rt_l2_addr_get(&l2Addr);
    if(RT_ERR_OK != ret)
    {
        osal_memset(&l2Addr, 0x00, sizeof(rt_l2_ucastAddr_t));
        osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
        l2Addr.port = HAL_GET_MIN_PORT();
        l2Addr.vid = *vid_ptr;
    }

    l2Addr.port = *port_ptr;

    DIAG_UTIL_ERR_CHK(rt_l2_addr_add(&l2Addr), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_l2_table_add_mac_ucast_vid_vid_mac_address_mac_spn_port */

/*
 * rt_l2-table add mac-ucast vid <UINT:vid> mac-address <MACADDR:mac> static state ( disable | enable )
 */
cparser_result_t
cparser_cmd_rt_l2_table_add_mac_ucast_vid_vid_mac_address_mac_static_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    cparser_macaddr_t  *mac_ptr)
{
    int32 ret;
    rt_l2_ucastAddr_t l2Addr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&l2Addr, 0x00, sizeof(rt_l2_ucastAddr_t));
    osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    l2Addr.vid = *vid_ptr;

    ret = rt_l2_addr_get(&l2Addr);
    if(RT_ERR_OK != ret)
    {
        osal_memset(&l2Addr, 0x00, sizeof(rt_l2_ucastAddr_t));
        osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
        l2Addr.port = HAL_GET_MIN_PORT();
        l2Addr.vid = *vid_ptr;
    }

    if('e' == TOKEN_CHAR(9, 0))
        l2Addr.staticFlag = 1;
    else if('d' == TOKEN_CHAR(9, 0))
        l2Addr.staticFlag = 0;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rt_l2_addr_add(&l2Addr), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_l2_table_add_mac_ucast_vid_vid_mac_address_mac_static_state_disable_enable */

/*
 * rt_l2-table add mac-ucast vid <UINT:vid> mac-address <MACADDR:mac> filter flag ( disable | enable )
 */
cparser_result_t
cparser_cmd_rt_l2_table_add_mac_ucast_vid_vid_mac_address_mac_filter_flag_disable_enable(
//cparser_cmd_rt_l2_table_add_mac_ucast_mac_address_mac_spn_port(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    cparser_macaddr_t  *mac_ptr)
{
    int32 ret;
    rt_l2_ucastAddr_t l2Addr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&l2Addr, 0x00, sizeof(rt_l2_ucastAddr_t));
    osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    l2Addr.vid = *vid_ptr;

    ret = rt_l2_addr_get(&l2Addr);
    if(RT_ERR_OK != ret)
    {
        osal_memset(&l2Addr, 0x00, sizeof(rt_l2_ucastAddr_t));
        osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
        l2Addr.port = HAL_GET_MIN_PORT();
        l2Addr.vid = *vid_ptr;
    }

    if('e' == TOKEN_CHAR(9, 0))
        l2Addr.filterFlag = 1;
    else if('d' == TOKEN_CHAR(9, 0))
        l2Addr.filterFlag = 0;
    else
        return CPARSER_NOT_OK;

    
    DIAG_UTIL_ERR_CHK(rt_l2_addr_add(&l2Addr), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_l2_table_add_mac_ucast_vid_vid_mac_address_mac_filter_flag_disable_enable */

/*
 * rt_l2-table del mac-ucast vid <UINT:vid> mac-address <MACADDR:mac>
 */
cparser_result_t
cparser_cmd_rt_l2_table_del_mac_ucast_vid_vid_mac_address_mac(
    cparser_context_t *context,
    uint32_t  *vid_ptr,    
    cparser_macaddr_t  *mac_ptr)
{
    int32 ret;
    rt_l2_ucastAddr_t l2Addr;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&l2Addr, 0x00, sizeof(rt_l2_ucastAddr_t));
    osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    l2Addr.vid = *vid_ptr;

    DIAG_UTIL_ERR_CHK(rt_l2_addr_del(&l2Addr), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_l2_table_del_mac_ucast_vid_vid_mac_address_mac */

/*
 * rt_l2-table get mac-ucast vid <UINT:vid> mac-address <MACADDR:mac>
 */
cparser_result_t
cparser_cmd_rt_l2_table_get_mac_ucast_vid_vid_mac_address_mac(
    cparser_context_t *context,
    uint32_t  *vid_ptr,    
    cparser_macaddr_t  *mac_ptr)
{
    int32 ret;
    rt_l2_ucastAddr_t l2Addr;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&l2Addr, 0x00, sizeof(rt_l2_ucastAddr_t));
    osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    l2Addr.vid = *vid_ptr;

    DIAG_UTIL_ERR_CHK(rt_l2_addr_get(&l2Addr), ret);
    _display_l2Ucast_entry(&l2Addr);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_l2_table_get_mac_ucast_vid_vid_mac_address_mac */

/*
 * rt_l2-table add mac-mcast vid <UINT:vid> mac-address <MACADDR:mac> group <UINT:group>
 */
cparser_result_t
cparser_cmd_rt_l2_table_add_mac_mcast_vid_vid_mac_address_mac_group_group(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    cparser_macaddr_t  *mac_ptr,
    uint32_t  *group_ptr)
{
    int32 ret;
    rt_l2_mcastAddr_t mcastAddr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&mcastAddr, 0x00, sizeof(rt_l2_mcastAddr_t));
    osal_memcpy(&mcastAddr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    mcastAddr.vid = *vid_ptr;
    mcastAddr.group = *group_ptr;

    
    DIAG_UTIL_ERR_CHK(rt_l2_mcastAddr_add(&mcastAddr), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_l2_table_add_mac_mcast_vid_vid_mac_address_mac_group_group */

/*
 * rt_l2-table del mac-mcast vid <UINT:vid> mac-address <MACADDR:mac>
 */
cparser_result_t
cparser_cmd_rt_l2_table_del_mac_mcast_vid_vid_mac_address_mac(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    cparser_macaddr_t  *mac_ptr)
{
    int32 ret;
    rt_l2_mcastAddr_t mcastAddr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&mcastAddr, 0x00, sizeof(rt_l2_mcastAddr_t));
    osal_memcpy(&mcastAddr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    mcastAddr.vid = *vid_ptr;

    DIAG_UTIL_ERR_CHK(rt_l2_mcastAddr_del(&mcastAddr), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_l2_table_del_mac_mcast_vid_vid_mac_address_mac */

/*
 * rt_l2-table get mac-mcast vid <UINT:vid> mac-address <MACADDR:mac>
 */
cparser_result_t
cparser_cmd_rt_l2_table_get_mac_mcast_vid_vid_mac_address_mac(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    cparser_macaddr_t  *mac_ptr)
{
    int32 ret;
    rt_l2_mcastAddr_t mcastAddr;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&mcastAddr, 0x00, sizeof(rt_l2_mcastAddr_t));
    osal_memcpy(&mcastAddr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    mcastAddr.vid = *vid_ptr;

    DIAG_UTIL_ERR_CHK(rt_l2_mcastAddr_get(&mcastAddr), ret);


        diag_util_mprintf("MACAddress        Group   vid\n");
        diag_util_mprintf("----------------- ------ ------\n");
        diag_util_mprintf("%-17s %6d %6d\n"
                                    ,diag_util_inet_mactoa(mcastAddr.mac.octet)
                                    ,mcastAddr.group
                                    ,mcastAddr.vid);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_l2_table_get_mac_mcast_vid_vid_mac_address_mac */

/*
 * rt_l2-table get next-entry mac-ucast address <UINT:address>
 */
cparser_result_t
cparser_cmd_rt_l2_table_get_next_entry_mac_ucast_address_address(
    cparser_context_t *context,
    uint32_t  *address_ptr)
{
    int32 ret;
    int32 address;
    rt_l2_ucastAddr_t l2Addr;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    address = *address_ptr;
    osal_memset(&l2Addr, 0x00, sizeof(rt_l2_ucastAddr_t));

    DIAG_UTIL_ERR_CHK(rt_l2_nextValidAddr_get(&address, &l2Addr), ret);
    _display_l2Ucast_entry(&l2Addr);
    diag_util_printf("\n Address = %d", address);
    diag_util_printf("\n");


    return CPARSER_OK;
}    /* end of cparser_cmd_rt_l2_table_get_next_entry_mac_ucast_address_address */

/*
 * rt_l2-table get next-entry mac-ucast address <UINT:address> spn <UINT:port>
 */
cparser_result_t
cparser_cmd_rt_l2_table_get_next_entry_mac_ucast_address_address_spn_port(
    cparser_context_t *context,
    uint32_t  *address_ptr,
    uint32_t  *port_ptr)
{
    int32 ret;
    int32 address;
    rt_l2_ucastAddr_t l2Addr;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    address = *address_ptr;
    osal_memset(&l2Addr, 0x00, sizeof(rt_l2_ucastAddr_t));

    DIAG_UTIL_ERR_CHK(rt_l2_nextValidAddrOnPort_get(*port_ptr, &address, &l2Addr), ret);
    _display_l2Ucast_entry(&l2Addr);
    diag_util_printf("\n Address = %d", address);
    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_l2_table_get_next_entry_mac_ucast_address_address_spn_port */

/*
 * rt_l2-table get port-move port ( <PORT_LIST:ports> | all ) action
 */
cparser_result_t
cparser_cmd_rt_l2_table_get_port_move_port_ports_all_action(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rt_port_t port;
    diag_portlist_t portlist;
    rt_action_t action;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rt_l2_illegalPortMoveAction_get(port, &action), ret);
        diag_util_printf("\n Port %d Port move Action: %s", port, diagStr_actionStr[action]);
    }
    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_l2_table_get_port_move_port_ports_all_action */

/*
 * rt_l2-table set port-move port ( <PORT_LIST:ports> | all ) action ( copy-to-cpu | drop | forward | trap-to-cpu ) */
cparser_result_t
cparser_cmd_rt_l2_table_set_port_move_port_ports_all_action_copy_to_cpu_drop_forward_trap_to_cpu(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rtk_port_t port;
    diag_portlist_t portlist;
    rtk_action_t action;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if('c' == TOKEN_CHAR(6, 0))
        action = ACTION_COPY2CPU;
    else if('d' == TOKEN_CHAR(6, 0))
        action = ACTION_DROP;
    else if('f' == TOKEN_CHAR(6, 0))
        action = ACTION_FORWARD;
    else if('t' == TOKEN_CHAR(6, 0))
        action = ACTION_TRAP2CPU;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rt_l2_illegalPortMoveAction_set(port, action), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_l2_table_set_port_move_port_ports_all_action_copy_to_cpu_drop_forward_trap_to_cpu */

 /*
  *rt_l2-table get age-time 
  */
cparser_result_t
cparser_cmd_rt_l2_table_get_age_time(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32 ageTime;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rt_l2_ageTime_get(&ageTime), ret);
    diag_util_printf("\n Age Time: %d", ageTime);

    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_l2_table_get_age_time */

/*
 *rt_l2-table set age-time <UINT:time> 
 */
cparser_result_t
cparser_cmd_rt_l2_table_set_age_time_time(
    cparser_context_t *context,
    uint32_t  *time_ptr)
{
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rt_l2_ageTime_set(*time_ptr), ret);
    
    return CPARSER_OK;
}       /* end of cparser_cmd_rt_l2_table_set_age_time_time */

 /*
  *rt_l2-table get ivl-svl 
  */
cparser_result_t
cparser_cmd_rt_l2_table_get_ivl_svl(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rt_enable_t ivlEn;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rt_l2_ivlSvl_get(&ivlEn), ret);
    diag_util_mprintf("\n IVL state: %s", diagStr_enable[ivlEn]);

    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_l2_table_get_ivl_svl */    

/*
 *rt_l2-table set ivl-svl ( enable | disable ) 
 */
cparser_result_t
cparser_cmd_rt_l2_table_set_ivl_svl_enable_disable(
    cparser_context_t *context)
{
    rt_enable_t enable;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    if('e'==TOKEN_CHAR(3,0))
        enable = ENABLED;
    else
        enable = DISABLED;

    DIAG_UTIL_ERR_CHK((rt_l2_ivlSvl_set(enable)), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_l2_table_set_ivl_svl_enable_disable */       


/*
 * rt_l2-table set fwdGroup index <UINT:index> mask <UINT64:mask>
 */
cparser_result_t
cparser_cmd_rt_l2_table_set_fwdGroup_index_index_mask_mask(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint64_t  *mask_ptr)
{
    int32 ret;
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_PARAM_RANGE_CHK((*index_ptr > 255), CPARSER_ERR_INVALID_PARAMS);

    DIAG_UTIL_ERR_CHK((rt_l2_fwdGroupId_set(*index_ptr,*mask_ptr)), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_l2_table_set_fwdgroup_index_index_mask_mask */

/*
 * rt_l2-table get fwdGroup index <UINT:index>
 */
cparser_result_t
cparser_cmd_rt_l2_table_get_fwdGroup_index_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    uint64 mask = 0;
    int32 ret;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_PARAM_RANGE_CHK((*index_ptr > 255), CPARSER_ERR_INVALID_PARAMS);

    DIAG_UTIL_ERR_CHK((rt_l2_fwdGroupId_get(*index_ptr, &mask)), ret);

    diag_util_mprintf("Forward Group ID: %d, mask 0x%x\n", *index_ptr, mask);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_l2_table_get_fwdgroup_index_index */

/*
 * rt_l2-table set fwdGroup action index <UINT:index> port <UINT:port> vid ( nop | assign | delete ) <UINT:vid> pri ( nop | assign ) <UINT:pri>
 */
cparser_result_t
cparser_cmd_rt_l2_table_set_fwdGroup_action_index_index_port_port_vid_nop_assign_delete_vid_pri_nop_assign_pri(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *port_ptr,
    uint32_t  *vid_ptr,
    uint32_t  *pri_ptr)
{
    rt_l2_portAction_t portAct;
    int32 ret;
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_PARAM_RANGE_CHK((*index_ptr > 63), CPARSER_ERR_INVALID_PARAMS);

    memset(&portAct, 0, sizeof(rt_l2_portAction_t));
    portAct.port = *port_ptr;

    if('n' == TOKEN_CHAR(9, 0))
        portAct.vidAct = RT_L2_VID_ACTION_NOP;
    else if('a' == TOKEN_CHAR(9, 0))
        portAct.vidAct = RT_L2_VID_ACTION_MODIFY;
    else
        portAct.vidAct = RT_L2_VID_ACTION_DELETE;

    if(vid_ptr != NULL)
    {
        DIAG_UTIL_PARAM_RANGE_CHK((*vid_ptr > 4095), CPARSER_ERR_INVALID_PARAMS);
        portAct.vid = *vid_ptr;
    }

    if('n' == TOKEN_CHAR(12, 0))
        portAct.priAct = RT_L2_PRI_ACTION_NOP;
    else
        portAct.priAct = RT_L2_PRI_ACTION_MODIFY;

    if(pri_ptr != NULL)
    {
        DIAG_UTIL_PARAM_RANGE_CHK((*pri_ptr > 7), CPARSER_ERR_INVALID_PARAMS);
        portAct.pri = *pri_ptr;
    }

    DIAG_UTIL_ERR_CHK((rt_l2_fwdGroupAction_set(*index_ptr, portAct)), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_l2_table_set_fwdgroup_action_index_index_port_port_vid_nop_assign_delete_vid_pri_nop_assign_pri */

/*
 * rt_l2-table get fwdGroup action index <UINT:index> */
cparser_result_t
cparser_cmd_rt_l2_table_get_fwdGroup_action_index_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    rt_l2_portAction_t portAct;
    int32 ret;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_PARAM_RANGE_CHK((*index_ptr > 63), CPARSER_ERR_INVALID_PARAMS);

    memset(&portAct, 0, sizeof(rt_l2_portAction_t));
    DIAG_UTIL_ERR_CHK((rt_l2_fwdGroupAction_get(*index_ptr, &portAct)), ret);

    diag_util_mprintf("Index: %d\n",*index_ptr);
    diag_util_mprintf("Port: %d\n",portAct.port);
    if(portAct.vidAct == RT_L2_VID_ACTION_NOP)
        diag_util_mprintf("VID: NOP\n");
    else if (portAct.vidAct == RT_L2_VID_ACTION_DELETE)
        diag_util_mprintf("VID: Delete\n");
    else
        diag_util_mprintf("VID: Assign %d\n", portAct.vid);

    if(portAct.priAct == RT_L2_PRI_ACTION_NOP)
        diag_util_mprintf("PRI: NOP\n");
    else
        diag_util_mprintf("PRI: Assign %d\n", portAct.pri);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_l2_table_get_fwdgroup_action_index_index */
