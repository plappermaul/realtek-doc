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
 * Purpose : Definition those Security command and APIs in the SDK diagnostic shell.
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
#include <parser/cparser_priv.h>
#include <rtk/sec.h>
#include <diag_str.h>
//#include <dal/apollo/raw/apollo_raw_sec.h>


#if 0
int8 * _diag_secGetThresholdName(apollo_raw_dos_flood_t flood)
{
    static int8 name[40];
	switch(flood)
	{
        case SYNFLOOD_DENY: osal_strcpy(name,"SYN flood thershold");break;
        case FINFLOOD_DENY: osal_strcpy(name,"FIN flood thershold");break;
        case ICMPFLOOD_DENY: osal_strcpy(name,"ICMP flood thershold");break;
	}
	return name;
}
#endif
/*
 * security init
 */
cparser_result_t
cparser_cmd_security_init(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_sec_init(), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_svlan_init */

/*
 * security set attack-prevent port ( <PORT_LIST:port> | all ) state ( disable | enable )
 */
cparser_result_t
cparser_cmd_security_set_attack_prevent_port_port_all_state_disable_enable(
    cparser_context_t *context,
    char * *port_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if ('e' == TOKEN_CHAR(6,0))
        enable = ENABLED;
    else
        enable = DISABLED;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if ((ret = rtk_sec_portAttackPreventState_set(port,enable)) != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_security_set_attack_prevent_port_port_all_state_disable_enable */

/*
 * security get attack-prevent port ( <PORT_LIST:port> | all ) state
 */
cparser_result_t
cparser_cmd_security_get_attack_prevent_port_port_all_state(
    cparser_context_t *context,
    char * *port_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port = 0;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    diag_util_mprintf("Port State\n");
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_sec_portAttackPreventState_get(port, &enable), ret);
        diag_util_mprintf("%-4d %s\n", port, diagStr_enable[enable]);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_security_get_attack_prevent_port_port_all_state */

/*
 * security get attack-prevent ( daeqsa-deny | land-deny | blat-deny | synfin-deny | xma-deny | nullscan-deny | tcphdr-min-check | syn-sportl1024-deny ) action
 */
cparser_result_t
cparser_cmd_security_get_attack_prevent_daeqsa_deny_land_deny_blat_deny_synfin_deny_xma_deny_nullscan_deny_tcphdr_min_check_syn_sportl1024_deny_action(
    cparser_context_t *context)
{
    int32 ret;
    rtk_action_t action;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    if ('d' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(rtk_sec_attackPrevent_get(DAEQSA_DENY, &action), ret);
        diag_util_printf("%s: ", diagStr_secGetTypeName[DAEQSA_DENY]);
        diag_util_mprintf("%-8s\n", diagStr_actionStr[action]);
    }
    else if ('l' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(rtk_sec_attackPrevent_get(LAND_DENY, &action), ret);
        diag_util_printf("%s: ", diagStr_secGetTypeName[LAND_DENY]);
        diag_util_mprintf("%-8s\n", diagStr_actionStr[action]);
    }
    else if ('b' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(rtk_sec_attackPrevent_get(BLAT_DENY, &action), ret);
        diag_util_printf("%s: ", diagStr_secGetTypeName[BLAT_DENY]);
        diag_util_mprintf("%-8s\n", diagStr_actionStr[action]);
    }
    else if ('x' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(rtk_sec_attackPrevent_get(XMA_DENY, &action), ret);
        diag_util_printf("%s: ", diagStr_secGetTypeName[XMA_DENY]);
        diag_util_mprintf("%-8s\n", diagStr_actionStr[action]);
    }
    else if ('n' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(rtk_sec_attackPrevent_get(NULLSCAN_DENY, &action), ret);
        diag_util_printf("%s: ", diagStr_secGetTypeName[NULLSCAN_DENY]);
        diag_util_mprintf("%-8s\n", diagStr_actionStr[action]);
    }
    else if ('t' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(rtk_sec_attackPrevent_get(TCPHDR_MIN_CHECK, &action), ret);
        diag_util_printf("%s: ", diagStr_secGetTypeName[TCPHDR_MIN_CHECK]);
        diag_util_mprintf("%-8s\n", diagStr_actionStr[action]);
    }
    else if ('s' == TOKEN_CHAR(3,0))
    {
        if ('f' == TOKEN_CHAR(3,3))
        {
            DIAG_UTIL_ERR_CHK(rtk_sec_attackPrevent_get(SYNFIN_DENY, &action), ret);
            diag_util_printf("%s: ", diagStr_secGetTypeName[SYNFIN_DENY]);
            diag_util_mprintf("%-8s\n", diagStr_actionStr[action]);
        }
        else if ('-' == TOKEN_CHAR(3,3))
        {
            DIAG_UTIL_ERR_CHK(rtk_sec_attackPrevent_get(SYN_SPORTL1024_DENY, &action), ret);
            diag_util_printf("%s: ", diagStr_secGetTypeName[SYN_SPORTL1024_DENY]);
            diag_util_mprintf("%-8s\n", diagStr_actionStr[action]);
        }
        return CPARSER_ERR_INVALID_PARAMS;
    }
    else
        return CPARSER_ERR_INVALID_PARAMS;

    diag_util_printf("\n");
    return CPARSER_OK;
}    /* end of cparser_cmd_security_get_attack_prevent_daeqsa_deny_land_deny_blat_deny_synfin_deny_xma_deny_nullscan_deny_tcphdr_min_check_syn_sportl1024_deny_action */

/*
 * security set attack-prevent ( daeqsa-deny | land-deny | blat-deny | synfin-deny | xma-deny | nullscan-deny | tcphdr-min-check | syn-sportl1024-deny ) action ( forward | drop | trap-to-cpu )
 */
cparser_result_t
cparser_cmd_security_set_attack_prevent_daeqsa_deny_land_deny_blat_deny_synfin_deny_xma_deny_nullscan_deny_tcphdr_min_check_syn_sportl1024_deny_action_forward_drop_trap_to_cpu(
    cparser_context_t *context)
{
    int32     ret = RT_ERR_FAILED;
    rtk_action_t action;
    rtk_sec_attackType_t attackType;

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
        attackType = DAEQSA_DENY;
    }
    else if ('l' == TOKEN_CHAR(3,0))
    {
        attackType = LAND_DENY;
    }
    else if ('b' == TOKEN_CHAR(3,0))
    {
        attackType = BLAT_DENY;
    }
    else if ('t' == TOKEN_CHAR(3,0))
    {
        attackType = TCPHDR_MIN_CHECK;
    }
    else if ('s' == TOKEN_CHAR(3,0))
    {
        if ('f' == TOKEN_CHAR(3,3))
        {
            attackType = SYNFIN_DENY;
        }
        else if ('-' == TOKEN_CHAR(3,3))
        {
            attackType = SYN_SPORTL1024_DENY;
        }
        else
            return CPARSER_ERR_INVALID_PARAMS;
    }
    else if ('x' == TOKEN_CHAR(3,0))
    {
        attackType = XMA_DENY;
    }
    else if ('n' == TOKEN_CHAR(3,0))
    {
        attackType = NULLSCAN_DENY;
    }
    else
        return CPARSER_ERR_INVALID_PARAMS;

    DIAG_UTIL_ERR_CHK(rtk_sec_attackPrevent_set(attackType, action), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_security_set_attack_prevent_daeqsa_deny_land_deny_blat_deny_synfin_deny_xma_deny_nullscan_deny_tcphdr_min_check_syn_sportl1024_deny_action_drop_trap_to_cpu */

/*
 * security get attack-prevent ( syn-flood | fin-flood | icmp-flood ) action
 */
cparser_result_t
cparser_cmd_security_get_attack_prevent_syn_flood_fin_flood_icmp_flood_action(
    cparser_context_t *context)
{
    int32 ret;
    rtk_action_t action;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    if ('s' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(rtk_sec_attackPrevent_get(SYNFLOOD_DENY, &action), ret);
        diag_util_printf("%s: ", diagStr_secGetTypeName[SYNFLOOD_DENY]);
        diag_util_mprintf("%-8s\n", diagStr_actionStr[action]);
    }
    else if ('f' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(rtk_sec_attackPrevent_get(FINFLOOD_DENY, &action), ret);
        diag_util_printf("%s: ", diagStr_secGetTypeName[FINFLOOD_DENY]);
        diag_util_mprintf("%-8s\n", diagStr_actionStr[action]);
    }
    else if ('i' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(rtk_sec_attackPrevent_get(ICMPFLOOD_DENY, &action), ret);
        diag_util_printf("%s: ", diagStr_secGetTypeName[ICMPFLOOD_DENY]);
        diag_util_mprintf("%-8s\n", diagStr_actionStr[action]);
    }
    else
        return CPARSER_ERR_INVALID_PARAMS;

    diag_util_printf("\n");
    return CPARSER_OK;
}    /* end of cparser_cmd_security_get_attack_prevent_syn_flood_fin_flood_icmp_flood_action */

/*
 * security set attack-prevent ( syn-flood | fin-flood | icmp-flood ) action ( forward | drop | trap-to-cpu )
 */
cparser_result_t
cparser_cmd_security_set_attack_prevent_syn_flood_fin_flood_icmp_flood_action_forward_drop_trap_to_cpu(
    cparser_context_t *context)
{
    int32     ret = RT_ERR_FAILED;
    rtk_action_t action;
    rtk_sec_attackType_t attackType;

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
        attackType = SYNFLOOD_DENY;
    }
    else if ('f' == TOKEN_CHAR(3,0))
    {
        attackType = FINFLOOD_DENY;
    }
    else if ('i' == TOKEN_CHAR(3,0))
    {
        attackType = ICMPFLOOD_DENY;
    }
    else
        return CPARSER_ERR_INVALID_PARAMS;

    DIAG_UTIL_ERR_CHK(rtk_sec_attackPrevent_set(attackType, action), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_security_set_attack_prevent_syn_flood_fin_flood_icmp_flood_action_drop_trap_to_cpu */

/*
 * security get attack-prevent ( syn-flood | fin-flood | icmp-flood ) threshold
 */
cparser_result_t
cparser_cmd_security_get_attack_prevent_syn_flood_fin_flood_icmp_flood_threshold(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32 threshold;

    if ('s' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(rtk_sec_attackFloodThresh_get(SEC_SYNCFLOOD, &threshold), ret);
        diag_util_mprintf("%s: %d\n", diagStr_secThresholdName[SEC_SYNCFLOOD], threshold);
    }
    else if ('f' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(rtk_sec_attackFloodThresh_get(SEC_FINFLOOD, &threshold), ret);
        diag_util_mprintf("%s: %d\n", diagStr_secThresholdName[SEC_FINFLOOD], threshold);
    }
    else if ('i' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(rtk_sec_attackFloodThresh_get(SEC_ICMPFLOOD, &threshold), ret);
        diag_util_mprintf("%s: %d\n", diagStr_secThresholdName[SEC_ICMPFLOOD], threshold);
    }
    else
        return CPARSER_ERR_INVALID_PARAMS;
  
    return CPARSER_OK;
}    /* end of cparser_cmd_security_get_attack_prevent_syn_flood_fin_flood_icmp_flood_threshold */

/*
 * security set attack-prevent ( syn-flood | fin-flood | icmp-flood ) threshold <UINT:threshold>
 */
cparser_result_t
cparser_cmd_security_set_attack_prevent_syn_flood_fin_flood_icmp_flood_threshold_threshold(
    cparser_context_t *context,
    uint32_t  *threshold_ptr)
{
    int32 ret = RT_ERR_FAILED;
    
    if ('s' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(rtk_sec_attackFloodThresh_set(SEC_SYNCFLOOD, *threshold_ptr), ret);
    }
    else if ('f' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(rtk_sec_attackFloodThresh_set(SEC_FINFLOOD, *threshold_ptr), ret);
    }
    else if ('i' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(rtk_sec_attackFloodThresh_set(SEC_ICMPFLOOD, *threshold_ptr), ret);
    }
    else
        return CPARSER_ERR_INVALID_PARAMS;

    return CPARSER_OK;
}    /* end of cparser_cmd_security_set_attack_prevent_syn_flood_fin_flood_icmp_flood_threshold_threshold */

/*
 * security get attack-prevent ( syn-flood | fin-flood | icmp-flood ) unit
 */
cparser_result_t
cparser_cmd_security_get_attack_prevent_syn_flood_fin_flood_icmp_flood_unit(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32 unit;

    if ('s' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(rtk_sec_attackFloodThreshUnit_get(SEC_SYNCFLOOD, &unit), ret);
        diag_util_mprintf("%s unit: %dms\n", diagStr_secThresholdName[SEC_SYNCFLOOD], (unit));
    }
    else if ('f' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(rtk_sec_attackFloodThreshUnit_get(SEC_FINFLOOD, &unit), ret);
        diag_util_mprintf("%s unit: %dms\n", diagStr_secThresholdName[SEC_FINFLOOD],(unit));
    }
    else if ('i' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(rtk_sec_attackFloodThreshUnit_get(SEC_ICMPFLOOD, &unit), ret);
        diag_util_mprintf("%s unit: %dms\n", diagStr_secThresholdName[SEC_ICMPFLOOD], (unit));
    }
    else
        return CPARSER_ERR_INVALID_PARAMS;
  
    return CPARSER_OK;
}    /* end of cparser_cmd_security_get_attack_prevent_syn_flood_fin_flood_icmp_flood_unit */

/*
 * security set attack-prevent ( syn-flood | fin-flood | icmp-flood ) uint <UINT:unit>
 */
cparser_result_t
cparser_cmd_security_set_attack_prevent_syn_flood_fin_flood_icmp_flood_unit_unit(
    cparser_context_t *context,
    uint32_t  *unit_ptr)
{
    int32 ret = RT_ERR_FAILED;
    
    if ('s' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(rtk_sec_attackFloodThreshUnit_set(SEC_SYNCFLOOD, *unit_ptr), ret);
    }
    else if ('f' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(rtk_sec_attackFloodThreshUnit_set(SEC_FINFLOOD, *unit_ptr), ret);
    }
    else if ('i' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(rtk_sec_attackFloodThreshUnit_set(SEC_ICMPFLOOD, *unit_ptr), ret);
    }
    else
    {
        DIAG_ERR_PRINT(CPARSER_ERR_INVALID_PARAMS);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_security_set_attack_prevent_syn_flood_fin_flood_icmp_flood_uint_unit */

/*
 * security get attack-prevent ( tcp-frag-off-min-check | icmp-frag-pkts-deny | pod-deny | udp-bomb | syn-with-data ) action
 */
cparser_result_t
cparser_cmd_security_get_attack_prevent_tcp_frag_off_min_check_icmp_frag_pkts_deny_pod_deny_udp_bomb_syn_with_data_action(
    cparser_context_t *context)
{
    int32 ret;
    rtk_action_t action;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    if ('t' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(rtk_sec_attackPrevent_get(TCP_FRAG_OFF_MIN_CHECK, &action), ret);
        diag_util_printf("%s: ",diagStr_secGetTypeName[TCP_FRAG_OFF_MIN_CHECK]);
        diag_util_mprintf("%-8s\n", diagStr_actionStr[action]);
    }
    else if ('i' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(rtk_sec_attackPrevent_get(ICMP_FRAG_PKTS_DENY, &action), ret);
        diag_util_printf("%s: ", diagStr_secGetTypeName[ICMP_FRAG_PKTS_DENY]);
        diag_util_mprintf("%-8s\n", diagStr_actionStr[action]);
    }
    else if ('p' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(rtk_sec_attackPrevent_get(POD_DENY, &action), ret);
        diag_util_printf("%s: ", diagStr_secGetTypeName[POD_DENY]);
        diag_util_mprintf("%-8s\n", diagStr_actionStr[action]);
    }
    else if ('u' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(rtk_sec_attackPrevent_get(UDPDOMB_DENY, &action), ret);
        diag_util_printf("%s: ", diagStr_secGetTypeName[UDPDOMB_DENY]);
        diag_util_mprintf("%-8s\n", diagStr_actionStr[action]);
    }
    else if ('s' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(rtk_sec_attackPrevent_get(SYNWITHDATA_DENY, &action), ret);
        diag_util_printf("%s: ", diagStr_secGetTypeName[SYNWITHDATA_DENY]);
        diag_util_mprintf("%-8s\n", diagStr_actionStr[action]);
    }
    else
        return CPARSER_ERR_INVALID_PARAMS;

    diag_util_printf("\n");
    return CPARSER_OK;
}    /* end of cparser_cmd_security_get_attack_prevent_tcp_frag_off_min_check_icmp_frag_pkts_deny_pod_deny_udp_bomb_syn_with_data_action */

/*
 * security set attack-prevent ( tcp-frag-off-min-check | icmp-frag-pkts-deny | pod-deny | udp-bomb | syn-with-data ) action ( forward | drop | trap-to-cpu )
 */
cparser_result_t
cparser_cmd_security_set_attack_prevent_tcp_frag_off_min_check_icmp_frag_pkts_deny_pod_deny_udp_bomb_syn_with_data_action_forward_drop_trap_to_cpu(
    cparser_context_t *context)
{
    int32     ret = RT_ERR_FAILED;
    rtk_action_t action;
    rtk_sec_attackType_t attackType;

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

    if ('t' == TOKEN_CHAR(3,0))
    {
        attackType = TCP_FRAG_OFF_MIN_CHECK;
    }
    else if ('i' == TOKEN_CHAR(3,0))
    {
        attackType = ICMP_FRAG_PKTS_DENY;
    }
    else if ('p' == TOKEN_CHAR(3,0))
    {
        attackType = POD_DENY;
    }
    else if ('u' == TOKEN_CHAR(3,0))
    {
        attackType = UDPDOMB_DENY;
    }
    else if ('s' == TOKEN_CHAR(3,0))
    {
        attackType = SYNWITHDATA_DENY;
    }
    else
        return CPARSER_ERR_INVALID_PARAMS;

    DIAG_UTIL_ERR_CHK(rtk_sec_attackPrevent_set(attackType, action), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_security_set_attack_prevent_tcp_frag_off_min_check_icmp_frag_pkts_deny_pod_deny_udp_bomb_syn_with_data_action_drop_trap_to_cpu */

