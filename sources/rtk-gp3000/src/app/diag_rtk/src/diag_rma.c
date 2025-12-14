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


#include <rtk/trap.h>

/*
 * rma dump
 */
cparser_result_t
cparser_cmd_rma_dump(
    cparser_context_t *context)
{
    rtk_trap_rma_action_t action;
    uint32 ret = CPARSER_NOT_OK;
    rtk_mac_t rmaMac;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    rmaMac.octet[0] = 0x01;
    rmaMac.octet[1] = 0x80;
    rmaMac.octet[2] = 0xC2;
    rmaMac.octet[3] = 0x00;
    rmaMac.octet[4] = 0x00;
	diag_util_mprintf("RMA 01-80-C2-00-00-xx\n");
	diag_util_mprintf("trail action       \n");
	for(rmaMac.octet[5] = 0x00;rmaMac.octet[5]<=0x2F;rmaMac.octet[5]++)
	{
	    DIAG_UTIL_ERR_CHK(rtk_trap_rmaAction_get(&rmaMac, &action), ret);

	    diag_util_printf("%2.2x    ",  rmaMac.octet[5]);

	    switch(action)
	    {
	        case ACTION_DROP:
	            diag_util_printf("%-14s",DIAG_STR_DROP);
	            break;
	        case ACTION_TRAP2CPU:
	            diag_util_printf("%-14s",DIAG_STR_TRAP2CPU);
	            break;
	        case ACTION_FORWARD:
	            diag_util_printf("%-14s",DIAG_STR_FORWARD);
	            break;
	    }
        if((rmaMac.octet[5]%3)==2)
            diag_util_printf("\n");
	}
    return CPARSER_OK;
}    /* end of cparser_cmd_rma_dump */

/*
 * rma set priority <UINT:priority>
 */
cparser_result_t
cparser_cmd_rma_set_priority_priority(
    cparser_context_t *context,
    uint32_t  *priority_ptr)
{
    int32 ret = CPARSER_NOT_OK;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(rtk_trap_rmaPri_set(*priority_ptr), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rma_set_priority_priority */

/*
 * rma get priority
 */
cparser_result_t
cparser_cmd_rma_get_priority(
    cparser_context_t *context)
{
    int32 ret = CPARSER_NOT_OK;
    uint32 priority;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(rtk_trap_rmaPri_get(&priority), ret);
    diag_util_mprintf("The RMA trap priorit = %u\n", priority);

    return CPARSER_OK;
}    /* end of cparser_cmd_rma_get_priority */

/*
 * rma set address <UINT:rma_tail> action ( drop | forward | forward-exclude-cpu | trap-to-cpu )
 */
cparser_result_t
cparser_cmd_rma_set_address_rma_tail_action_drop_forward_forward_exclude_cpu_trap_to_cpu(
    cparser_context_t *context,
    uint32_t  *rma_tail_ptr)
{
    int32 ret = CPARSER_NOT_OK;
    rtk_action_t action;
    rtk_mac_t rmaMac;

    DIAG_UTIL_PARAM_CHK();

    if ('d' == TOKEN_CHAR(5, 0))
    {
        action = ACTION_DROP;
    }
    else if ('t' == TOKEN_CHAR(5, 0))
    {
        action = ACTION_TRAP2CPU;
    }
	else if(!osal_strcmp(TOKEN_STR(5),"forward"))
	{
		action = ACTION_FORWARD;
	}
	else if(!osal_strcmp(TOKEN_STR(5),"forward-exclude-cpu"))
	{
		action = ACTION_FORWARD_EXCLUDE_CPU;
	}
    else
        return CPARSER_ERR_INVALID_PARAMS;

    rmaMac.octet[0] = 0x01;
    rmaMac.octet[1] = 0x80;
    rmaMac.octet[2] = 0xC2;
    rmaMac.octet[3] = 0x00;
    rmaMac.octet[4] = 0x00;
    rmaMac.octet[5] = *rma_tail_ptr;

    DIAG_UTIL_ERR_CHK(rtk_trap_rmaAction_set(&rmaMac, action), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rma_set_address_rma_tail_action_drop_forward_forward_exclude_cpu_trap_to_cpu */

/*
 * rma get address <UINT:rma_tail> action
 */
cparser_result_t
cparser_cmd_rma_get_address_rma_tail_action(
    cparser_context_t *context,
    uint32_t  *rma_tail_ptr)
{
    rtk_trap_rma_action_t action;
    uint32 ret = CPARSER_NOT_OK;
    rtk_mac_t rmaMac;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    rmaMac.octet[0] = 0x01;
    rmaMac.octet[1] = 0x80;
    rmaMac.octet[2] = 0xC2;
    rmaMac.octet[3] = 0x00;
    rmaMac.octet[4] = 0x00;
    rmaMac.octet[5] = (uint8)*rma_tail_ptr;

    DIAG_UTIL_ERR_CHK(rtk_trap_rmaAction_get(&rmaMac, &action), ret);

    diag_util_printf("RMA 01-80-C2-00-00-%2.2x action: ",  rmaMac.octet[5]);

    switch(action)
    {
        case ACTION_DROP:
            diag_util_mprintf("%s\n",DIAG_STR_DROP);
            break;
        case ACTION_TRAP2CPU:
            diag_util_mprintf("%s\n",DIAG_STR_TRAP2CPU);
            break;
        case ACTION_FORWARD:
            diag_util_mprintf("%s\n",DIAG_STR_FORWARD);
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_rma_get_address_rma_tail_action */


