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
 * rt_igmp set ( igmpv1 | igmpv2 | igmpv3 | mldv1 | mldv2 ) port ( <PORT_LIST:ports> | all ) action ( drop | forward | trap-to-cpu )
 */
cparser_result_t
cparser_cmd_rt_igmp_set_igmpv1_igmpv2_igmpv3_mldv1_mldv2_port_ports_all_action_drop_forward_trap_to_cpu(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rt_trap_igmpMld_type_t type;
    rt_action_t action;
    rt_port_t port;
    diag_portlist_t portlist;
    uint32 field;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if ('i' == TOKEN_CHAR(2, 0))
    {
        if ('1' == TOKEN_CHAR(2, 5))
            type = RT_IGMPMLD_TYPE_IGMPV1;
        else if ('2' == TOKEN_CHAR(2, 5))
            type = RT_IGMPMLD_TYPE_IGMPV2;
        else if ('3' == TOKEN_CHAR(2, 5))
            type = RT_IGMPMLD_TYPE_IGMPV3;
        else
            return CPARSER_NOT_OK;
    }
    else if ('m' == TOKEN_CHAR(2, 0))
    {
        if ('1' == TOKEN_CHAR(2, 4))
            type = RT_IGMPMLD_TYPE_MLDV1;
        else if ('2' == TOKEN_CHAR(2, 4))
            type = RT_IGMPMLD_TYPE_MLDV2;
        else
            return CPARSER_NOT_OK;
    }
    else
        return CPARSER_NOT_OK;

    if('d' == TOKEN_CHAR(6, 0))
        action = ACTION_DROP;
    else if('f' == TOKEN_CHAR(6, 0))
        action = ACTION_FORWARD;
    else if('t' == TOKEN_CHAR(6, 0))
        action = ACTION_TRAP2CPU;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rt_trap_portIgmpMldCtrlPktAction_set(port, type, action), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_igmp_set_igmpv1_igmpv2_igmpv3_mldv1_mldv2_port_ports_all_action_drop_forward_trap_to_cpu */

/*
 * rt_igmp get ( igmpv1 | igmpv2 | igmpv3 | mldv1 | mldv2 ) port ( <PORT_LIST:ports> | all ) action
 */
cparser_result_t
cparser_cmd_rt_igmp_get_igmpv1_igmpv2_igmpv3_mldv1_mldv2_port_ports_all_action(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rt_trap_igmpMld_type_t type;
    rt_action_t action = RT_ACTION_FORWARD;
    rt_port_t port;
    diag_portlist_t portlist;
    uint32 igmpAct;
    uint32 field;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if ('i' == TOKEN_CHAR(2, 0))
    {
        if ('1' == TOKEN_CHAR(2, 5))
            type = RT_IGMPMLD_TYPE_IGMPV1;
        else if ('2' == TOKEN_CHAR(2, 5))
            type = RT_IGMPMLD_TYPE_IGMPV2;
        else if ('3' == TOKEN_CHAR(2, 5))
            type = RT_IGMPMLD_TYPE_IGMPV3;
        else
            return CPARSER_NOT_OK;
    }
    else if ('m' == TOKEN_CHAR(2, 0))
    {
        if ('1' == TOKEN_CHAR(2, 4))
            type = RT_IGMPMLD_TYPE_MLDV1;
        else if ('2' == TOKEN_CHAR(2, 4))
            type = RT_IGMPMLD_TYPE_MLDV2;
        else
            return CPARSER_NOT_OK;
    }
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rt_trap_portIgmpMldCtrlPktAction_get(port, type, &action), ret);
        diag_util_printf("\n Port: %d, %s : %s", port, diagStr_igmpTypeStr[type], diagStr_actionStr[action]);
    }

    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_igmp_get_igmpv1_igmpv2_igmpv3_mldv1_mldv2_port_ports_all_action */

