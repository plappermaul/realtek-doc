#include <stdio.h>
#include <string.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <diag_util.h>
#include <diag_str.h>
#include <parser/cparser_priv.h>
#include <rtk/rt/rt_sec.h>


/*
 * rt_security init
 */
cparser_result_t
cparser_cmd_rt_security_init(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rt_sec_init(), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_security_init */

/*
 * rt_security get attack-prevent port ( <PORT_LIST:port> | all ) state
 */
cparser_result_t
cparser_cmd_rt_security_get_attack_prevent_port_port_all_state(
    cparser_context_t *context,
    char * *port_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rt_port_t port = 0;
    rt_enable_t enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    diag_util_mprintf("Port State\n");
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rt_sec_portAttackPreventState_get(port, &enable), ret);
        diag_util_mprintf("%-4d %s\n", port, diagStr_enable[enable]);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_security_get_attack_prevent_port_port_all_state */

/*
 * rt_security set attack-prevent port ( <PORT_LIST:port> | all ) state ( disable | enable )
 */
cparser_result_t
cparser_cmd_rt_security_set_attack_prevent_port_port_all_state_disable_enable(
    cparser_context_t *context,
    char * *port_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rt_port_t port;
    rt_enable_t enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if ('e' == TOKEN_CHAR(6,0))
        enable = ENABLED;
    else
        enable = DISABLED;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if ((ret = rt_sec_portAttackPreventState_set(port,enable)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_security_set_attack_prevent_port_port_all_state_disable_enable */

/*
 * rt_security get attack-prevent ( daeqsa-deny | land-deny | blat-deny | synfin-deny | xma-deny | nullscan-deny | tcphdr-min-check | syn-sportl1024-deny | icmp-frag-pkts-deny ) action
 */
cparser_result_t
cparser_cmd_rt_security_get_attack_prevent_daeqsa_deny_land_deny_blat_deny_synfin_deny_xma_deny_nullscan_deny_tcphdr_min_check_syn_sportl1024_deny_icmp_frag_pkts_deny_action(
    cparser_context_t *context)
{
    int32 ret;
    rt_action_t action;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    if ('d' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(rt_sec_attackPrevent_get(RT_DAEQSA_DENY, &action), ret);
        diag_util_printf("%s: ", diagStr_secGetTypeName[RT_DAEQSA_DENY]);
        diag_util_mprintf("%-8s\n", diagStr_actionStr[action]);
    }
    else if ('l' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(rt_sec_attackPrevent_get(RT_LAND_DENY, &action), ret);
        diag_util_printf("%s: ", diagStr_secGetTypeName[RT_LAND_DENY]);
        diag_util_mprintf("%-8s\n", diagStr_actionStr[action]);
    }
    else if ('b' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(rt_sec_attackPrevent_get(RT_BLAT_DENY, &action), ret);
        diag_util_printf("%s: ", diagStr_secGetTypeName[RT_BLAT_DENY]);
        diag_util_mprintf("%-8s\n", diagStr_actionStr[action]);
    }
    else if ('x' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(rt_sec_attackPrevent_get(RT_XMA_DENY, &action), ret);
        diag_util_printf("%s: ", diagStr_secGetTypeName[RT_XMA_DENY]);
        diag_util_mprintf("%-8s\n", diagStr_actionStr[action]);
    }
    else if ('n' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(rt_sec_attackPrevent_get(RT_NULLSCAN_DENY, &action), ret);
        diag_util_printf("%s: ", diagStr_secGetTypeName[RT_NULLSCAN_DENY]);
        diag_util_mprintf("%-8s\n", diagStr_actionStr[action]);
    }
    else if ('t' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(rt_sec_attackPrevent_get(RT_TCPHDR_MIN_CHECK, &action), ret);
        diag_util_printf("%s: ", diagStr_secGetTypeName[RT_TCPHDR_MIN_CHECK]);
        diag_util_mprintf("%-8s\n", diagStr_actionStr[action]);
    }
    else if ('s' == TOKEN_CHAR(3,0))
    {
        if ('f' == TOKEN_CHAR(3,3))
        {
            DIAG_UTIL_ERR_CHK(rt_sec_attackPrevent_get(RT_SYNFIN_DENY, &action), ret);
            diag_util_printf("%s: ", diagStr_secGetTypeName[RT_SYNFIN_DENY]);
            diag_util_mprintf("%-8s\n", diagStr_actionStr[action]);
        }
        else if ('-' == TOKEN_CHAR(3,3))
        {
            DIAG_UTIL_ERR_CHK(rt_sec_attackPrevent_get(RT_SYN_SPORTL1024_DENY, &action), ret);
            diag_util_printf("%s: ", diagStr_secGetTypeName[RT_SYN_SPORTL1024_DENY]);
            diag_util_mprintf("%-8s\n", diagStr_actionStr[action]);
        }
        return CPARSER_ERR_INVALID_PARAMS;
    }
    else if ('i' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(rt_sec_attackPrevent_get(RT_ICMP_FRAG_PKTS_DENY, &action), ret);
        diag_util_printf("%s: ", diagStr_secGetTypeName[RT_ICMP_FRAG_PKTS_DENY]);
        diag_util_mprintf("%-8s\n", diagStr_actionStr[action]);
    }
    else
        return CPARSER_ERR_INVALID_PARAMS;

    diag_util_printf("\n");
    return CPARSER_OK;
}    /* end of cparser_cmd_rt_security_get_attack_prevent_daeqsa_deny_land_deny_blat_deny_synfin_deny_xma_deny_nullscan_deny_tcphdr_min_check_syn_sportl1024_deny_icmp_frag_pkts_deny_action */

/*
 * rt_security set attack-prevent ( daeqsa-deny | land-deny | blat-deny | synfin-deny | xma-deny | nullscan-deny | tcphdr-min-check | syn-sportl1024-deny | icmp-frag-pkts-deny ) action ( forward | drop | trap-to-cpu )
 */
cparser_result_t
cparser_cmd_rt_security_set_attack_prevent_daeqsa_deny_land_deny_blat_deny_synfin_deny_xma_deny_nullscan_deny_tcphdr_min_check_syn_sportl1024_deny_icmp_frag_pkts_deny_action_forward_drop_trap_to_cpu(
    cparser_context_t *context)
{
    int32     ret = RT_ERR_FAILED;
    rt_action_t action;
    rt_sec_attackType_t attackType;

    DIAG_UTIL_PARAM_CHK();

    if ('f' == TOKEN_CHAR(5,0))
    {
        action = ACTION_FORWARD;
    }
    else if ('d' == TOKEN_CHAR(5,0))
    {
        action = ACTION_DROP;
    }
    else if ('t' == TOKEN_CHAR(5,0))
    {
        action = ACTION_TRAP2CPU;
    }
    else
        return CPARSER_ERR_INVALID_PARAMS;

    if ('d' == TOKEN_CHAR(3,0))
    {
        attackType = RT_DAEQSA_DENY;
    }
    else if ('l' == TOKEN_CHAR(3,0))
    {
        attackType = RT_LAND_DENY;
    }
    else if ('b' == TOKEN_CHAR(3,0))
    {
        attackType = RT_BLAT_DENY;
    }
    else if ('t' == TOKEN_CHAR(3,0))
    {
        attackType = RT_TCPHDR_MIN_CHECK;
    }
    else if ('s' == TOKEN_CHAR(3,0))
    {
        if ('f' == TOKEN_CHAR(3,3))
        {
            attackType = RT_SYNFIN_DENY;
        }
        else if ('-' == TOKEN_CHAR(3,3))
        {
            attackType = RT_SYN_SPORTL1024_DENY;
        }
        else
            return CPARSER_ERR_INVALID_PARAMS;
    }
    else if ('x' == TOKEN_CHAR(3,0))
    {
        attackType = RT_XMA_DENY;
    }
    else if ('n' == TOKEN_CHAR(3,0))
    {
        attackType = RT_NULLSCAN_DENY;
    }
    else if ('i' == TOKEN_CHAR(3,0))
    {
        attackType = RT_ICMP_FRAG_PKTS_DENY;
    }
    else
        return CPARSER_ERR_INVALID_PARAMS;

    DIAG_UTIL_ERR_CHK(rt_sec_attackPrevent_set(attackType, action), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_security_set_attack_prevent_daeqsa_deny_land_deny_blat_deny_synfin_deny_xma_deny_nullscan_deny_tcphdr_min_check_syn_sportl1024_deny_icmp_frag_pkts_deny_action_forward_drop_trap_to_cpu */

/*
 * rt_security get attack-prevent ( syn-flood | fin-flood | icmp-flood ) action
 */
cparser_result_t
cparser_cmd_rt_security_get_attack_prevent_syn_flood_fin_flood_icmp_flood_action(
    cparser_context_t *context)
{
    int32 ret;
    rt_action_t action;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    if ('s' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(rt_sec_attackPrevent_get(RT_SYNFLOOD_DENY, &action), ret);
        diag_util_printf("%s: ", diagStr_secGetTypeName[RT_SYNFLOOD_DENY]);
        diag_util_mprintf("%-8s\n", diagStr_actionStr[action]);
    }
    else if ('f' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(rt_sec_attackPrevent_get(RT_FINFLOOD_DENY, &action), ret);
        diag_util_printf("%s: ", diagStr_secGetTypeName[RT_FINFLOOD_DENY]);
        diag_util_mprintf("%-8s\n", diagStr_actionStr[action]);
    }
    else if ('i' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(rt_sec_attackPrevent_get(RT_ICMPFLOOD_DENY, &action), ret);
        diag_util_printf("%s: ", diagStr_secGetTypeName[RT_ICMPFLOOD_DENY]);
        diag_util_mprintf("%-8s\n", diagStr_actionStr[action]);
    }
    else
        return CPARSER_ERR_INVALID_PARAMS;

    diag_util_printf("\n");
    return CPARSER_OK;
}    /* end of cparser_cmd_rt_security_get_attack_prevent_syn_flood_fin_flood_icmp_flood_action */

/*
 * rt_security set attack-prevent ( syn-flood | fin-flood | icmp-flood ) action ( forward | drop | trap-to-cpu )
 */
cparser_result_t
cparser_cmd_rt_security_set_attack_prevent_syn_flood_fin_flood_icmp_flood_action_forward_drop_trap_to_cpu(
    cparser_context_t *context)
{
    int32     ret = RT_ERR_FAILED;
    rt_action_t action;
    rt_sec_attackType_t attackType;

    DIAG_UTIL_PARAM_CHK();

    if ('f' == TOKEN_CHAR(5,0))
    {
        action = ACTION_FORWARD;
    }
    else if ('d' == TOKEN_CHAR(5,0))
    {
        action = ACTION_DROP;
    }
    else if ('t' == TOKEN_CHAR(5,0))
    {
        action = ACTION_TRAP2CPU;
    }
    else
        return CPARSER_ERR_INVALID_PARAMS;

    if ('s' == TOKEN_CHAR(3,0))
    {
        attackType = RT_SYNFLOOD_DENY;
    }
    else if ('f' == TOKEN_CHAR(3,0))
    {
        attackType = RT_FINFLOOD_DENY;
    }
    else if ('i' == TOKEN_CHAR(3,0))
    {
        attackType = RT_ICMPFLOOD_DENY;
    }
    else
        return CPARSER_ERR_INVALID_PARAMS;

    DIAG_UTIL_ERR_CHK(rt_sec_attackPrevent_set(attackType, action), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_security_set_attack_prevent_syn_flood_fin_flood_icmp_flood_action_drop_trap_to_cpu */

/*
 * rt_security get attack-prevent ( syn-flood | fin-flood | icmp-flood ) threshold
 */
cparser_result_t
cparser_cmd_rt_security_get_attack_prevent_syn_flood_fin_flood_icmp_flood_threshold(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32 threshold;

    if ('s' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(rt_sec_attackFloodThresh_get(RT_SEC_SYNCFLOOD, &threshold), ret);
        diag_util_mprintf("%s: %d\n", diagStr_secThresholdName[RT_SEC_SYNCFLOOD], threshold);
    }
    else if ('f' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(rt_sec_attackFloodThresh_get(RT_SEC_FINFLOOD, &threshold), ret);
        diag_util_mprintf("%s: %d\n", diagStr_secThresholdName[RT_SEC_FINFLOOD], threshold);
    }
    else if ('i' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(rt_sec_attackFloodThresh_get(RT_SEC_ICMPFLOOD, &threshold), ret);
        diag_util_mprintf("%s: %d\n", diagStr_secThresholdName[RT_SEC_ICMPFLOOD], threshold);
    }
    else
        return CPARSER_ERR_INVALID_PARAMS;

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_security_get_attack_prevent_syn_flood_fin_flood_icmp_flood_threshold */

/*
 * rt_security set attack-prevent ( syn-flood | fin-flood | icmp-flood ) threshold <UINT:threshold>
 */
cparser_result_t
cparser_cmd_rt_security_set_attack_prevent_syn_flood_fin_flood_icmp_flood_threshold_threshold(
    cparser_context_t *context,
    uint32_t  *threshold_ptr)
{
    int32 ret = RT_ERR_FAILED;

    if ('s' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(rt_sec_attackFloodThresh_set(RT_SEC_SYNCFLOOD, *threshold_ptr), ret);
    }
    else if ('f' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(rt_sec_attackFloodThresh_set(RT_SEC_FINFLOOD, *threshold_ptr), ret);
    }
    else if ('i' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(rt_sec_attackFloodThresh_set(RT_SEC_ICMPFLOOD, *threshold_ptr), ret);
    }
    else
        return CPARSER_ERR_INVALID_PARAMS;

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_security_set_attack_prevent_syn_flood_fin_flood_icmp_flood_threshold_threshold */

/*
 * rt_security get attack-prevent ( syn-flood | fin-flood | icmp-flood ) unit
 */
cparser_result_t
cparser_cmd_rt_security_get_attack_prevent_syn_flood_fin_flood_icmp_flood_unit(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32 unit;

    if ('s' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(rt_sec_attackFloodThreshUnit_get(RT_SEC_SYNCFLOOD, &unit), ret);
        diag_util_mprintf("%s unit: %dms\n", diagStr_secThresholdName[RT_SEC_SYNCFLOOD], (unit));
    }
    else if ('f' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(rt_sec_attackFloodThreshUnit_get(RT_SEC_FINFLOOD, &unit), ret);
        diag_util_mprintf("%s unit: %dms\n", diagStr_secThresholdName[RT_SEC_FINFLOOD],(unit));
    }
    else if ('i' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(rt_sec_attackFloodThreshUnit_get(RT_SEC_ICMPFLOOD, &unit), ret);
        diag_util_mprintf("%s unit: %dms\n", diagStr_secThresholdName[RT_SEC_ICMPFLOOD], (unit));
    }
    else
        return CPARSER_ERR_INVALID_PARAMS;
  
    return CPARSER_OK;
}    /* end of cparser_cmd_rt_security_get_attack_prevent_syn_flood_fin_flood_icmp_flood_unit */

/*
 * rt_security set attack-prevent ( syn-flood | fin-flood | icmp-flood ) uint <UINT:unit>
 */
cparser_result_t
cparser_cmd_rt_security_set_attack_prevent_syn_flood_fin_flood_icmp_flood_unit_unit(
    cparser_context_t *context,
    uint32_t  *unit_ptr)
{
    int32 ret = RT_ERR_FAILED;
    
    if ('s' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(rt_sec_attackFloodThreshUnit_set(RT_SEC_SYNCFLOOD, *unit_ptr), ret);
    }
    else if ('f' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(rt_sec_attackFloodThreshUnit_set(RT_SEC_FINFLOOD, *unit_ptr), ret);
    }
    else if ('i' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(rt_sec_attackFloodThreshUnit_set(RT_SEC_ICMPFLOOD, *unit_ptr), ret);
    }
    else
    {
        DIAG_ERR_PRINT(CPARSER_ERR_INVALID_PARAMS);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_security_set_attack_prevent_syn_flood_fin_flood_icmp_flood_uint_unit */
