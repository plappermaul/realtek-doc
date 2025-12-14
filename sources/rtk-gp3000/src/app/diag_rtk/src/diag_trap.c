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
 * Purpose : Definition those Trap command and APIs in the SDK diagnostic shell.
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

//#include <dal/apollo/raw/apollo_raw_trap.h>
#include <rtk/trap.h>

/*
 * trap init
 */
cparser_result_t
cparser_cmd_trap_init(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_trap_init(), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_trap_init */


/*
 * trap set ( cdp | csstp ) action ( drop | forward | forward-exclude-cpu | trap-to-cpu )
 */
cparser_result_t
cparser_cmd_trap_set_cdp_csstp_action_drop_forward_forward_exclude_cpu_trap_to_cpu(
    cparser_context_t *context)
{
    int32 ret = CPARSER_NOT_OK;
    rtk_trap_rma_action_t action;
    rtk_mac_t rmaMac;

    DIAG_UTIL_PARAM_CHK();

    rmaMac.octet[0] = 0x01;
    rmaMac.octet[1] = 0x00;
    rmaMac.octet[2] = 0x0C;
    rmaMac.octet[3] = 0xCC;
    rmaMac.octet[4] = 0xCC;


    if ('d' == TOKEN_CHAR(2, 1))
    {
        rmaMac.octet[5] = 0xcc;
    }
    else if ('s' == TOKEN_CHAR(2, 1))
    {
        rmaMac.octet[5] = 0xcd;
    }
    else
        return CPARSER_ERR_INVALID_PARAMS;


    if ('d' == TOKEN_CHAR(4, 0))
    {
        action = RMA_ACTION_DROP;
    }
    else if ('t' == TOKEN_CHAR(4, 0))
    {
        action = RMA_ACTION_TRAP2CPU;
    }
	else if(!osal_strcmp(TOKEN_STR(4),"forward"))
	{
		action = RMA_ACTION_FORWARD;
	}
	else if(!osal_strcmp(TOKEN_STR(4),"forward-exclude-cpu"))
	{
		action = RMA_ACTION_FORWARD_EXCLUDE_CPU;
	}
    else
        return CPARSER_ERR_INVALID_PARAMS;


    DIAG_UTIL_ERR_CHK(rtk_trap_rmaAction_set(&rmaMac, action), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_trap_set_cdp_csstp_action_drop_forward_forward_exclude_cpu_trap_to_cpu */

/*
 * trap get ( cdp | csstp ) action
 */
cparser_result_t
cparser_cmd_trap_get_cdp_csstp_action(
    cparser_context_t *context)
{
    rtk_trap_rma_action_t action;
    uint32 ret = CPARSER_NOT_OK;
    rtk_mac_t rmaMac;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    rmaMac.octet[0] = 0x01;
    rmaMac.octet[1] = 0x00;
    rmaMac.octet[2] = 0x0C;
    rmaMac.octet[3] = 0xCC;
    rmaMac.octet[4] = 0xCC;


    if ('d' == TOKEN_CHAR(2, 1))
    {
        rmaMac.octet[5] = 0xcc;
    }
    else if ('s' == TOKEN_CHAR(2, 1))
    {
        rmaMac.octet[5] = 0xcd;
    }
    else
        return CPARSER_ERR_INVALID_PARAMS;

    DIAG_UTIL_ERR_CHK(rtk_trap_rmaAction_get(&rmaMac, &action), ret);

    if(rmaMac.octet[5] == 0xcc)
    {
        diag_util_mprintf("CDP ");
    }
    else if(rmaMac.octet[5] == 0xcd)
    {
        diag_util_mprintf("CSSTP ");
    }

    switch(action)
    {
        case RMA_ACTION_DROP:
            diag_util_mprintf("%s\n",DIAG_STR_DROP);
            break;
        case RMA_ACTION_TRAP2CPU:
            diag_util_mprintf("%s\n",DIAG_STR_TRAP2CPU);
            break;
        case RMA_ACTION_FORWARD:
            diag_util_mprintf("%s\n",DIAG_STR_FORWARD);
            break;
    }


    return CPARSER_OK;
}    /* end of cparser_cmd_trap_get_cdp_csstp_action */

/*
 * trap set uni-trap-priority state ( disable | enable )
 */
cparser_result_t
cparser_cmd_trap_set_uni_trap_priority_state_disable_enable(
    cparser_context_t *context)
{
    int32     ret = RT_ERR_FAILED;
    rtk_enable_t enable = DISABLED;

    DIAG_UTIL_PARAM_CHK();

    if ('d' == TOKEN_CHAR(4,0))
    {
        enable = DISABLED;
    }
    else if ('e' == TOKEN_CHAR(4,0))
    {
        enable = ENABLED;
    }

    DIAG_UTIL_ERR_CHK(rtk_trap_uniTrapPriorityEnable_set(enable), ret);


    return CPARSER_OK;
}    /* end of cparser_cmd_trap_set_uni_trap_priority_state_disable_enable */

/*
 * trap get uni-trap-priority state
 */
cparser_result_t
cparser_cmd_trap_get_uni_trap_priority_state(
    cparser_context_t *context)
{
    int32     ret = RT_ERR_FAILED;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_trap_uniTrapPriorityEnable_get(&enable), ret);

    diag_util_mprintf("UNI trap priority state: %s\n",diagStr_enable[enable]);

    return CPARSER_OK;
}    /* end of cparser_cmd_trap_get_uni_trap_priority_state */

/*
 * trap set uni-trap-priority priority <UINT:priority>
 */
cparser_result_t
cparser_cmd_trap_set_uni_trap_priority_priority_priority(
    cparser_context_t *context,
    uint32_t  *priority_ptr)
{
    int32 ret = CPARSER_NOT_OK;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(rtk_trap_uniTrapPriorityPriority_set(*priority_ptr), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_trap_set_uni_trap_priority_priority_priority */

/*
 * trap get uni-trap-priority priority
 */
cparser_result_t
cparser_cmd_trap_get_uni_trap_priority_priority(
    cparser_context_t *context)
{
    int32 ret = CPARSER_NOT_OK;
    uint32 priority;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_trap_uniTrapPriorityPriority_get(&priority), ret);
    diag_util_mprintf("The uni trap priority = %u\n", priority);

    return CPARSER_OK;
}    /* end of cparser_cmd_trap_get_uni_trap_priority_priority */

cparser_result_t _diag_trap_get_hash_type(char *pName, rtk_trap_hash_t *pType)
{
    if (!pName || !pType)
        return CPARSER_ERR_INVALID_PARAMS;

    if (!osal_strcmp(pName, "spa"))
    {
        *pType = TRAP_HASH_SPA;
    }
    else if (!osal_strcmp(pName, "smac"))
    {
        *pType = TRAP_HASH_SMAC;
    }
    else if (!osal_strcmp(pName, "dmac"))
    {
        *pType = TRAP_HASH_DMAC;
    }
    else if (!osal_strcmp(pName, "sip"))
    {
        *pType = TRAP_HASH_SIP_INNER;
    }
    else if (!osal_strcmp(pName, "dip"))
    {
        *pType = TRAP_HASH_DIP_INNER;
    }
    else if (!osal_strcmp(pName, "sport"))
    {
        *pType = TRAP_HASH_SPORT_INNER;
    }
    else if (!osal_strcmp(pName, "dport"))
    {
        *pType = TRAP_HASH_DPORT_INNER;
    }
    else
        return CPARSER_ERR_INVALID_PARAMS;

    return CPARSER_OK;
}

/*
 * trap set hash select ( spa | smac | dmac | sip | dip | sport | dport ) state ( disable | enable )
 */
cparser_result_t
cparser_cmd_trap_set_hash_select_spa_smac_dmac_sip_dip_sport_dport_state_disable_enable(
    cparser_context_t *context)
{
    int32               ret;
    rtk_trap_hash_t     type;
    rtk_enable_t        state;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(_diag_trap_get_hash_type(TOKEN_STR(4), &type), ret);

    if ('d' == TOKEN_CHAR(6,0))
    {
        state = DISABLED;
    }
    else if ('e' == TOKEN_CHAR(6,0))
    {
        state = ENABLED;
    }
    else
        return CPARSER_ERR_INVALID_PARAMS;

    DIAG_UTIL_ERR_CHK(rtk_trap_cpuTrapHashMask_set(type, state), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_trap_set_hash_select_spa_smac_dmac_sip_dip_sport_dport_state_disable_enable */

/*
 * trap get hash select ( spa | smac | dmac | sip | dip | sport | dport ) state
 */
cparser_result_t
cparser_cmd_trap_get_hash_select_spa_smac_dmac_sip_dip_sport_dport_state(
    cparser_context_t *context)
{
    int32               ret;
    rtk_trap_hash_t     type;
    rtk_enable_t        state;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(_diag_trap_get_hash_type(TOKEN_STR(4), &type), ret);

    DIAG_UTIL_ERR_CHK(rtk_trap_cpuTrapHashMask_get(type, &state), ret);

    diag_util_mprintf("%-32s: %s\n", diagStr_trapHashType[type], diagStr_enable[state]);

    return CPARSER_OK;
}    /* end of cparser_cmd_trap_get_hash_select_spa_smac_dmac_sip_dip_sport_dport_state */

/*
 * trap dump hash select
 */
cparser_result_t
cparser_cmd_trap_dump_hash_select(
    cparser_context_t *context)
{
    int32               ret;
    rtk_trap_hash_t     type;
    rtk_enable_t        state;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    for (type = 0; type < TRAP_HASH_END; type++)
    {
        if (RT_ERR_OK == (ret = rtk_trap_cpuTrapHashMask_get(type, &state)))
        {
            diag_util_mprintf("%-32s: %s\n", diagStr_trapHashType[type], diagStr_enable[state]);
        }
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_trap_dump_hash_select */

/*
 * trap set hash result <UINT:value> port <UINT:port>
 */
cparser_result_t
cparser_cmd_trap_set_hash_result_value_port_port(
    cparser_context_t *context,
    uint32_t  *value_ptr,
    uint32_t  *port_ptr)
{
    int32   ret;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_PARAM_RANGE_CHK((16<=*value_ptr),CPARSER_ERR_INVALID_PARAMS);

    DIAG_UTIL_ERR_CHK(rtk_trap_cpuTrapHashPort_set(*value_ptr, *port_ptr), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_trap_set_hash_result_value_port_port */

/*
 * trap get hash result <UINT:value> port
 */
cparser_result_t
cparser_cmd_trap_get_hash_result_value_port(
    cparser_context_t *context,
    uint32_t  *value_ptr)
{
    int32       ret;
    rtk_port_t  port;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_trap_cpuTrapHashPort_get(*value_ptr, &port), ret);

    diag_util_mprintf("Value Port\n");
    diag_util_mprintf("%-6u%4d\n", *value_ptr, port);

    return CPARSER_OK;
}    /* end of cparser_cmd_trap_get_hash_result_value_port */

/*
 * trap dump hash result
 */
cparser_result_t
cparser_cmd_trap_dump_hash_result(
    cparser_context_t *context)
{
    int32       ret;
    uint8       value;
    rtk_port_t  port;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("Value Port\n");
    for (value = 0; value < 16; value++)
    {
        DIAG_UTIL_ERR_CHK(rtk_trap_cpuTrapHashPort_get(value, &port),ret);
        diag_util_mprintf("%-6u%4d\n", value, port);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_trap_dump_hash_result */

/*
 * trap set hash state ( disable | enable )
 */
cparser_result_t
cparser_cmd_trap_set_hash_state_disable_enable(
    cparser_context_t *context)
{
    int32 ret;
    rtk_enable_t state;

    DIAG_UTIL_PARAM_CHK();

    if ('d' == TOKEN_CHAR(4,0))
    {
        state = DISABLED;
    }
    else if ('e' == TOKEN_CHAR(4,0))
    {
        state = ENABLED;
    }
    else
        return CPARSER_ERR_INVALID_PARAMS;

    DIAG_UTIL_ERR_CHK(rtk_trap_cpuTrapHashState_set(state),ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_trap_set_hash_state_disable_enable */

/*
 * trap get hash state */
cparser_result_t
cparser_cmd_trap_get_hash_state(
    cparser_context_t *context)
{
    int32 ret;
    rtk_enable_t state;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_trap_cpuTrapHashState_get(&state),ret);
    
    diag_util_mprintf("state = %s\n",(state==ENABLED)?"enable":"disable");

    return CPARSER_OK;
}    /* end of cparser_cmd_trap_get_hash_state */

/*
 * trap set hash inic state ( disable | enable )
 */
cparser_result_t
cparser_cmd_trap_set_hash_inic_state_disable_enable(
    cparser_context_t *context)
{
    int32 ret;
    rtk_enable_t state;

    DIAG_UTIL_PARAM_CHK();

    if ('d' == TOKEN_CHAR(5,0))
    {
        state = DISABLED;
    }
    else if ('e' == TOKEN_CHAR(5,0))
    {
        state = ENABLED;
    }
    else
        return CPARSER_ERR_INVALID_PARAMS;

    DIAG_UTIL_ERR_CHK(rtk_trap_cpuTrapInicHashState_set(state),ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_trap_set_hash_inic_state_disable_enable */

/*
 * trap get hash inic state */
cparser_result_t
cparser_cmd_trap_get_hash_inic_state(
    cparser_context_t *context)
{
    int32 ret;
    rtk_enable_t state;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_trap_cpuTrapInicHashState_get(&state),ret);

    diag_util_mprintf("state = %s\n",(state==ENABLED)?"enable":"disable");

    return CPARSER_OK;
}    /* end of cparser_cmd_trap_get_hash_inic_state */

/*
 * trap set hash inic select <UINT:value>
 */
cparser_result_t
cparser_cmd_trap_set_hash_inic_select_value(
    cparser_context_t *context,
    uint32_t  *value_ptr)
{
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_PARAM_RANGE_CHK((2<=*value_ptr),CPARSER_ERR_INVALID_PARAMS);

    DIAG_UTIL_ERR_CHK(rtk_trap_cpuTrapInicHashSelect_set(*value_ptr),ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_trap_set_hash_inic_select_select */

/*
 * trap get hash inic select */
cparser_result_t
cparser_cmd_trap_get_hash_inic_select(
    cparser_context_t *context)
{
    int32 ret;
    uint32 select;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_trap_cpuTrapInicHashSelect_get(&select),ret);

    diag_util_mprintf("select = %d\n",select);

    return CPARSER_OK;
}    /* end of cparser_cmd_trap_get_hash_inic_select */
