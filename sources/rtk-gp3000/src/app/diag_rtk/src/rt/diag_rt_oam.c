#include <stdio.h>
#include <string.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <diag_util.h>
#include <diag_str.h>
#include <parser/cparser_priv.h>
#include <rtk/rt/rt_trap.h>

/*
 * oam get state
 */
cparser_result_t
cparser_cmd_rt_oam_get_state(
    cparser_context_t *context)
{
    rt_action_t action;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rt_trap_oamPduAction_get(&action), ret);

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
}    /* end of cparser_cmd_rt_oam_get_state */

/*
 * oam set state ( disable | enable )
 */
cparser_result_t
cparser_cmd_rt_oam_set_state_disable_enable(
    cparser_context_t *context)
{
    rt_action_t action = ACTION_TRAP2CPU;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    if('e'==TOKEN_CHAR(3,0))
        action = ACTION_TRAP2CPU;
    else if('d'==TOKEN_CHAR(3,0))
        action = ACTION_FORWARD;

    DIAG_UTIL_ERR_CHK(rt_trap_oamPduAction_set(action), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_oam_set_state_disable_enable */

