/*
 * Copyright (C) 2009-2016 Realtek Semiconductor Corp.
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
 * Purpose : Definition those SEC command and APIs in the SDK diagnostic shell.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) SEC enable/disable
 *           2) Parameter for SEC
 *
 */

/*
 * Include Files
 */
#include <common/debug/rt_log.h>
#include <common/rt_error.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <rtk/switch.h>
#include <rtk/port.h>
#include <rtk/sec.h>
#include <rtk/trap.h>
#include <diag_util.h>
#include <diag_om.h>
#include <diag_str.h>
#include <parser/cparser_priv.h>

#ifdef CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE
  #include <rtrpc/rtrpc_switch.h>
  #include <rtrpc/rtrpc_port.h>
  #include <rtrpc/rtrpc_sec.h>
  #include <rtrpc/rtrpc_trap.h>
#endif


/*
 * Macro Declaration
 */
#define DIAG_UTIL_MPRINTF   diag_util_mprintf

#define DUMP_IPMACBIND_ENTRY(__i, __e) \
do{                     \
    char __is[128];     \
    char __ms[32];      \
    char __ps[8];       \
    char __vs[8];       \
    char __pis[8];      \
    uint32  __pii;      \
    osal_memset(__is, 0 , sizeof(__is));    \
    osal_memset(__ms, 0 , sizeof(__ms));    \
    osal_memset(__ps, 0, sizeof(__ps));     \
    diag_util_ip2str(__is, __e.ipAddr);     \
    diag_util_mac2str(__ms, __e.macAddr.octet);   \
    if (__e.flags & RTK_SEC_IPMACBIND_FLAG_BIND_PORT) {         \
        if (__e.flags & RTK_SEC_IPMACBIND_FLAG_PORT_TRUNK) {    \
            osal_strcpy(__ps, "trunk");                         \
            __pii = __e.port;                                   \
        } else {                                                \
            osal_strcpy(__ps, "port");                          \
            __pii = __e.trunkId;                                \
        }                                                       \
        osal_sprintf(__pis, "%u", __pii);                       \
    } else {                                                    \
        osal_strcpy(__ps, "any");                               \
        osal_strcpy(__pis, "");                                 \
    }                                                           \
    if (__e.flags & RTK_SEC_IPMACBIND_FLAG_BIND_VLAN) {         \
        osal_sprintf(__vs, "%4u", __e.vid);                     \
    } else {                                                    \
        osal_strcpy(__vs, "any");                               \
    }                                                           \
    DIAG_UTIL_MPRINTF(" %4d %15s %17s %5s %-3s %4s\n",          \
                        __i, __is, __ms, __ps, __pis, __vs);    \
}while(0);


#ifdef CMD_SECURITY_GET_ARP_VALIDATION_PORT_ALL_ACTION
/*
 * security get arp-validation ( <PORT_LIST:port> | all ) action
 */
cparser_result_t cparser_cmd_security_get_arp_validation_port_all_action(cparser_context_t *context,
    char **port_ptr)
{
    uint32              unit = 0;
    int32               ret = RT_ERR_FAILED;
    rtk_port_t          port = 0;
    rtk_action_t        action;
    diag_portlist_t     portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    diag_util_mprintf("ARP Validation Action Configuration\n");
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_sec_portAttackPrevent_get(unit, port, ARP_INVALID, &action), ret);
        diag_util_mprintf("\tPort %2d : %s\n", port, (action == ACTION_DROP)? "Drop" : (action == ACTION_FORWARD)? "Forward" : "Trap-to-CPU");
    }

    return CPARSER_OK;
} /* end of cparser_cmd_security_get_arp_validation_port_all_action */
#endif

#ifdef CMD_SECURITY_GET_ATTACK_PREVENT
/*
 * security get attack-prevent
 */
cparser_result_t cparser_cmd_security_get_attack_prevent(cparser_context_t *context)
{
    uint32                  unit = 0;
    int32                   ret = RT_ERR_FAILED;
    rtk_sec_attackType_t    type = 0;
    rtk_action_t            action = 0;
    uint8  *actStr[] = { (uint8 *)"FORWARD", (uint8 *)"DROP", (uint8 *)"TRAP-TO-CPU" };
    uint8  *typeStr[] = {
        (uint8 *)"DAEQSA-DENY",
        (uint8 *)"LAND-DENY",
        (uint8 *)"UDPBLAT-DENY",
        (uint8 *)"TCPBLAT-DENY",
        (uint8 *)"POD-DENY",
        (uint8 *)"IPV6-MIN-FRAG-SIZE-CHECK",
        (uint8 *)"ICMP-FRAG-PKTS-DENY",
        (uint8 *)"ICMPV4-PING-MAX-CHECK",
        (uint8 *)"ICMPV6-PING-MAX-CHECK",
        (uint8 *)"SMURF-DENY",
        (uint8 *)"TCPHDR-MIN-CHECK",
        (uint8 *)"SYN-SPORTL1024-DENY",
        (uint8 *)"NULLSCAN-DENY",
        (uint8 *)"XMA-DENY",
        (uint8 *)"SYNFIN-DENY",
        (uint8 *)"SYNRST-DENY",
        (uint8 *)"TCP-FRAG-OFF-MIN-CHECK",
        #if defined(CONFIG_SDK_RTL9300)
        (uint8 *)"IPV4-INVALID-LEN",
        #endif
        #if defined(CONFIG_SDK_RTL9310)
        (uint8 *)"IPV4-INVALID-HEADER",
        #endif
        };
    uint32  typeSym[] = {
        DAEQSA_DENY,
        LAND_DENY,
        UDPBLAT_DENY,
        TCPBLAT_DENY,
        POD_DENY,
        IPV6_MIN_FRAG_SIZE_CHECK,
        ICMP_FRAG_PKTS_DENY,
        ICMPV4_PING_MAX_CHECK,
        ICMPV6_PING_MAX_CHECK,
        SMURF_DENY,
        TCPHDR_MIN_CHECK,
        SYN_SPORTL1024_DENY,
        NULLSCAN_DENY,
        XMA_DENY,
        SYNFIN_DENY,
        SYNRST_DENY,
        TCP_FRAG_OFF_MIN_CHECK,
        #if defined(CONFIG_SDK_RTL9300)
        IPV4_INVALID_LEN,
        #endif
        #if defined(CONFIG_SDK_RTL9310)
        IPV4_INVALID_HDR,
        #endif
        };

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    for (type = 0; type < (sizeof(typeStr)/sizeof(uint8 *)); type++)
    {
        DIAG_UTIL_ERR_CHK(rtk_sec_attackPreventAction_get(unit, typeSym[type], &action), ret);
        diag_util_mprintf("\t%25s action : %s\n", typeStr[type], actStr[action]);
    }

    return CPARSER_OK;
} /* end of cparser_cmd_security_get_attack_prevent */
#endif

#ifdef CMD_SECURITY_GET_ATTACK_PREVENT_PORT_ALL_STATE
/*
 * security get attack-prevent ( <PORT_LIST:port> | all ) state
 */
cparser_result_t cparser_cmd_security_get_attack_prevent_port_all_state(cparser_context_t *context,
    char **port_ptr)
{
    uint32              unit = 0;
    int32               ret = RT_ERR_FAILED;
    rtk_port_t          port = 0;
    rtk_enable_t        enable = 0;
    diag_portlist_t     portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    diag_util_mprintf("Attack Prevention Port State\n");
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_sec_portAttackPreventEnable_get(unit, port, &enable), ret);
        diag_util_mprintf("\tPort %2d : %s\n", port, (enable)? DIAG_STR_ENABLE : DIAG_STR_DISABLE);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_SECURITY_GET_MAX_PING_MIN_IPV6_FRAG_MIN_TCP_HEADER_SMURF_NETMASK
/*
 * security get ( max-ping | min-ipv6-frag | min-tcp-header | smurf-netmask )
 */
cparser_result_t cparser_cmd_security_get_max_ping_min_ipv6_frag_min_tcp_header_smurf_netmask(cparser_context_t *context)
{
    uint32              unit = 0;
    uint32              length = 0;
    int32               ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ('m' == TOKEN_CHAR(2,0))
    {
        if ('a' == TOKEN_CHAR(2,1))
        {
            DIAG_UTIL_ERR_CHK(rtk_sec_maxPingLen_get(unit, &length), ret);
            diag_util_mprintf("\tMax Ping Length : %5d\n", length);
        }
        else if ('i' == TOKEN_CHAR(2,4))
        {
            DIAG_UTIL_ERR_CHK(rtk_sec_minIPv6FragLen_get(unit, &length), ret);
            diag_util_mprintf("\tMin IPv6 Fragment Length : %5d\n", length);
        }
        else
        {
            DIAG_UTIL_ERR_CHK(rtk_sec_minTCPHdrLen_get(unit, &length), ret);
            diag_util_mprintf("\tMin TCP Header Length : %3d\n", length);
        }
    }
    else if ('s' == TOKEN_CHAR(2,0))
    {
        DIAG_UTIL_ERR_CHK(rtk_sec_smurfNetmaskLen_get(unit, &length), ret);
        diag_util_mprintf("\tSMURF Netmask Length : %2d\n", length);
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_SECURITY_GET_GRATUITOUS_ARP_PORT_ALL_ACTION
/*
 * security get gratuitous-arp ( <PORT_LIST:port> | all ) action
 */
cparser_result_t cparser_cmd_security_get_gratuitous_arp_port_all_action(cparser_context_t *context,
    char **port_ptr)
{
    uint32              unit = 0;
    int32               ret = RT_ERR_FAILED;
    rtk_port_t          port = 0;
    rtk_action_t        action;
    diag_portlist_t     portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    diag_util_mprintf("Gratuitous ARP Action Configuration\n");

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if (DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) ||
                DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID) ||
                DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) ||
                DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
        {
            DIAG_UTIL_ERR_CHK(rtk_sec_portAttackPrevent_get(unit, port,
                    GRATUITOUS_ARP, &action), ret);

            diag_util_mprintf("\tPort %2d : ", port);
            if (action == ACTION_DROP)
                diag_util_mprintf("Drop\n");
            else if (action == ACTION_FORWARD)
                diag_util_mprintf("Forward\n");
            else if (action == ACTION_TRAP2CPU)
                diag_util_mprintf("Trap to CPU\n");
            #if defined(CONFIG_SDK_RTL8380)
            else if (action == ACTION_COPY2CPU)
                diag_util_mprintf("Copy to CPU\n");
            #endif
        }
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_SECURITY_SET_ARP_VALIDATION_PORT_ALL_ACTION_DROP_FORWARD_TRAP_TO_CPU
/*
 * security set arp-validation ( <PORT_LIST:port> | all ) action ( drop | forward | trap-to-cpu )
 */
cparser_result_t cparser_cmd_security_set_arp_validation_port_all_action_drop_forward_trap_to_cpu(cparser_context_t *context,
    char **port_ptr)
{
    uint32              unit = 0;
    int32               ret = RT_ERR_FAILED;
    rtk_port_t          port;
    rtk_action_t        action;
    diag_portlist_t     portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    if ('d' == TOKEN_CHAR(5,0))
        action = ACTION_DROP;
    else if ('f' == TOKEN_CHAR(5,0))
        action = ACTION_FORWARD;
    else if ('t' == TOKEN_CHAR(5,0))
        action = ACTION_TRAP2CPU;
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_sec_portAttackPrevent_set(unit, port, ARP_INVALID, action), ret);
    }

    return CPARSER_OK;
} /* end of cparser_cmd_security_set_arp_validation_port_all_action_drop_forward_trap_to_cpu */
#endif

#ifdef CMD_SECURITY_SET_ATTACK_PREVENT_DAEQSA_DENY_ICMP_FRAG_PKTS_DENY_ICMPV4_PING_MAX_CHECK_ICMPV6_PING_MAX_CHECK_IPV6_MIN_FRAG_SIZE_CHECK_LAND_DENY_NULLSCAN_DENY_POD_DENY_SMURF_DENY_SYN_SPORTL1024_DENY_SYNFIN_DENY_SYNRST_DENY_TCP_FRAG_OFF_MIN_CHECK_TCPBLAT_DENY_TCPHDR_MIN_CHECK_UDPBLAT_DENY_XMA_DENY_ACTION_DROP_FORWARD_TRAP_TO_CPU
/*
 * security set attack-prevent ( daeqsa-deny | icmp-frag-pkts-deny | icmpv4-ping-max-check | icmpv6-ping-max-check | ipv6-min-frag-size-check | land-deny | nullscan-deny | pod-deny | smurf-deny | syn-sportl1024-deny | synfin-deny | synrst-deny | tcp-frag-off-min-check | tcpblat-deny | tcphdr-min-check | udpblat-deny | xma-deny ) action ( drop | forward | trap-to-cpu )
 */
cparser_result_t cparser_cmd_security_set_attack_prevent_daeqsa_deny_icmp_frag_pkts_deny_icmpv4_ping_max_check_icmpv6_ping_max_check_ipv6_min_frag_size_check_land_deny_nullscan_deny_pod_deny_smurf_deny_syn_sportl1024_deny_synfin_deny_synrst_deny_tcp_frag_off_min_check_tcpblat_deny_tcphdr_min_check_udpblat_deny_xma_deny_action_drop_forward_trap_to_cpu(cparser_context_t *context)
{
    uint32              unit = 0;
    int32               ret = RT_ERR_FAILED;
    rtk_sec_attackType_t    type = 0;
    rtk_action_t        action = 0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('d' == TOKEN_CHAR(3,0))
    {
        type = DAEQSA_DENY;
    }
    else if ('i' == TOKEN_CHAR(3,0))
    {
        if ('p' == TOKEN_CHAR(3,1))
            type = IPV6_MIN_FRAG_SIZE_CHECK;
        else
        {
            if ('-' == TOKEN_CHAR(3,4))
                type = ICMP_FRAG_PKTS_DENY;
            else if ('4' == TOKEN_CHAR(3,5))
                type = ICMPV4_PING_MAX_CHECK;
            else
                type = ICMPV6_PING_MAX_CHECK;
        }
    }
    else if ('l' == TOKEN_CHAR(3,0))
    {
        type = LAND_DENY;
    }
    else if ('n' == TOKEN_CHAR(3,0))
    {
        type = NULLSCAN_DENY;
    }
    else if ('p' == TOKEN_CHAR(3,0))
    {
        type = POD_DENY;
    }
    else if ('s' == TOKEN_CHAR(3,0))
    {
        if ('m' == TOKEN_CHAR(3,1))
            type = SMURF_DENY;
        else if ('-' == TOKEN_CHAR(3,3))
            type = SYN_SPORTL1024_DENY;
        else if ('f' == TOKEN_CHAR(3,3))
            type = SYNFIN_DENY;
        else
            type = SYNRST_DENY;
    }
    else if ('t' == TOKEN_CHAR(3,0))
    {
        if ('-' == TOKEN_CHAR(3,3))
            type = TCP_FRAG_OFF_MIN_CHECK;
        else if ('b' == TOKEN_CHAR(3,3))
            type = TCPBLAT_DENY;
        else
            type = TCPHDR_MIN_CHECK;
    }
    else if ('u' == TOKEN_CHAR(3,0))
    {
        type = UDPBLAT_DENY;
    }
    else if ('x' == TOKEN_CHAR(3,0))
    {
        type = XMA_DENY;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    if ('d' == TOKEN_CHAR(5,0))
        action = ACTION_DROP;
    else if ('f' == TOKEN_CHAR(5,0))
        action = ACTION_FORWARD;
    else if ('t' == TOKEN_CHAR(5,0))
        action = ACTION_TRAP2CPU;
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_sec_attackPreventAction_set(unit, type, action), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_SECURITY_SET_ATTACK_PREVENT_PORT_ALL_STATE_DISABLE_ENABLE
/*
 * security set attack-prevent ( <PORT_LIST:port> | all ) state ( disable | enable )
 */
cparser_result_t cparser_cmd_security_set_attack_prevent_port_all_state_disable_enable(cparser_context_t *context,
    char **port_ptr)
{
    uint32              unit = 0;
    int32               ret = RT_ERR_FAILED;
    rtk_enable_t        enable = 0;
    rtk_port_t          port;
    diag_portlist_t     portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    if ('d' == TOKEN_CHAR(5,0))
        enable = DISABLED;
    else if ('e' == TOKEN_CHAR(5,0))
        enable = ENABLED;
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_sec_portAttackPreventEnable_set(unit, port, enable), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_SECURITY_SET_MAX_PING_MIN_IPV6_FRAG_MIN_TCP_HEADER_SMURF_NETMASK_LENGTH
/*
 * security set ( max-ping | min-ipv6-frag | min-tcp-header | smurf-netmask ) <UINT:length>
 */
cparser_result_t cparser_cmd_security_set_max_ping_min_ipv6_frag_min_tcp_header_smurf_netmask_length(cparser_context_t *context,    uint32_t *length_ptr)
{
    uint32              unit = 0;
    uint32              length = 0;
    int32               ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    length = *length_ptr;

    if ('m' == TOKEN_CHAR(2,0))
    {
        if ('a' == TOKEN_CHAR(2,1))
        {
            DIAG_UTIL_ERR_CHK(rtk_sec_maxPingLen_set(unit, length), ret);
        }
        else if ('i' == TOKEN_CHAR(2,4))
        {
            DIAG_UTIL_ERR_CHK(rtk_sec_minIPv6FragLen_set(unit, length), ret);
        }
        else
        {
            DIAG_UTIL_ERR_CHK(rtk_sec_minTCPHdrLen_set(unit, length), ret);
        }
    }
    else if ('s' == TOKEN_CHAR(2,0))
    {
        DIAG_UTIL_ERR_CHK(rtk_sec_smurfNetmaskLen_set(unit, length), ret);
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_SECURITY_SET_GRATUITOUS_ARP_PORT_ALL_ACTION_DROP_FORWARD_TRAP_TO_CPU
/*
 * security set gratuitous-arp ( <PORT_LIST:port> | all ) action ( drop | forward | trap-to-cpu )
 */
cparser_result_t cparser_cmd_security_set_gratuitous_arp_port_all_action_drop_forward_trap_to_cpu(
    cparser_context_t *context,
    char **port_ptr)
{
    uint32              unit = 0;
    int32               ret = RT_ERR_FAILED;
    rtk_port_t          port;
    rtk_action_t        action;
    diag_portlist_t     portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    if ('d' == TOKEN_CHAR(5,0))
        action = ACTION_DROP;
    else if ('f' == TOKEN_CHAR(5,0))
        action = ACTION_FORWARD;
    else if ('t' == TOKEN_CHAR(5,0))
        action = ACTION_TRAP2CPU;
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_sec_portAttackPrevent_set(unit, port, GRATUITOUS_ARP, action), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_SECURITY_SET_GRATUITOUS_ARP_PORT_ALL_ACTION_COPY_TO_CPU
/*
 * security set gratuitous-arp ( <PORT_LIST:port> | all ) action copy-to-cpu
 */
cparser_result_t cparser_cmd_security_set_gratuitous_arp_port_all_action_copy_to_cpu(
    cparser_context_t *context,
    char **port_ptr)
{
    uint32              unit = 0;
    int32               ret = RT_ERR_FAILED;
    rtk_port_t          port;
    diag_portlist_t     portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_sec_portAttackPrevent_set(unit, port,
                GRATUITOUS_ARP, ACTION_COPY2CPU), ret);
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_SECURITY_GET_TRAP_TARGET
/*
 * security get trap-target
 */
cparser_result_t
cparser_cmd_security_get_trap_target(
    cparser_context_t *context)
{
    rtk_trapTarget_t    target;
    uint32              unit;
    int32               ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    ret = rtk_sec_trapTarget_get(unit, &target);
    if (ret != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("MPLS trap target: ");
    if (RTK_TRAP_LOCAL == target)
        diag_util_mprintf("Local\n");
    else
        diag_util_mprintf("Master\n");

    return CPARSER_OK;
}   /* end of cparser_cmd_security_get_trap_target */
#endif

#ifdef CMD_SECURITY_SET_TRAP_TARGET_LOCAL_MASTER
/*
 * security set trap-target ( local | master )
 */
cparser_result_t
cparser_cmd_security_set_trap_target_local_master(
    cparser_context_t *context)
{
    rtk_trapTarget_t    target;
    uint32              unit;
    int32               ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('l' == TOKEN_CHAR(3, 0))
    {
        target = RTK_TRAP_LOCAL;
    }
    else
    {
        target = RTK_TRAP_MASTER;
    }

    DIAG_UTIL_ERR_CHK(rtk_sec_trapTarget_set(unit, target), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_security_set_trap_target_local_master */
#endif

#ifdef CMD_SECURITY_SET_IP_MAC_BIND_LOOK_UP_MISS_MATCH_MISMATCH_ACTION_FORWARD_DROP_TRAP_TO_CPU_COPY_TO_CPU_TRAP_TO_MASTER_COPY_TO_MASTER
/*
 * security set ip-mac-bind ( look-up-miss | match | mismatch ) action ( forward | drop | trap-to-cpu | copy-to-cpu | trap-to-master | copy-to-master )
 */
cparser_result_t
cparser_cmd_security_set_ip_mac_bind_look_up_miss_match_mismatch_action_forward_drop_trap_to_cpu_copy_to_cpu_trap_to_master_copy_to_master(
    cparser_context_t *context)
{
    uint32              unit = 0;
    int32               ret = RT_ERR_FAILED;
    rtk_action_t        action = ACTION_FORWARD;
    rtk_action_t        lumisAct, matchAct, mismatchAct;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_sec_ipMacBindAction_get(unit, &lumisAct, &matchAct, &mismatchAct), ret);

    /* action */
    if ('f' == TOKEN_CHAR(5, 0))
    {
        action = ACTION_FORWARD;
    }
    else if ('d' == TOKEN_CHAR(5, 0))
    {
        action = ACTION_DROP;
    }
    else if ('t' == TOKEN_CHAR(5, 0))
    {
        if ('c' == TOKEN_CHAR(5, 8))
            action = ACTION_TRAP2CPU;
        else if ('m' == TOKEN_CHAR(5, 8))
            action = ACTION_TRAP2MASTERCPU;
    }
    else if ('c' == TOKEN_CHAR(5, 0))
    {
        if ('c' == TOKEN_CHAR(5, 8))
            action = ACTION_COPY2CPU;
        else if ('m' == TOKEN_CHAR(5, 8))
            action = ACTION_COPY2MASTERCPU;
    }

    if ('o' == TOKEN_CHAR(3,1))
    {
        /* look-up miss */
        lumisAct = action;
    }
    else if ('a' == TOKEN_CHAR(3,1))
    {
        /* match */
        matchAct = action;
    }
    else if ('i' == TOKEN_CHAR(3,1))
    {
        /* mismatch */
        mismatchAct = action;
    }

    DIAG_UTIL_ERR_CHK(rtk_sec_ipMacBindAction_set(unit, lumisAct, matchAct, mismatchAct), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_security_set_ip_mac_bind_look_up_miss_match_mismatch_action_forward_drop_trap_to_cpu_copy_to_cpu_trap_to_master_copy_to_master */
#endif

#ifdef CMD_SECURITY_GET_IP_MAC_BIND_LOOK_UP_MISS_MATCH_MISMATCH_ACTION
/*
 * security get ip-mac-bind ( look-up-miss | match | mismatch ) action
 */
cparser_result_t
cparser_cmd_security_get_ip_mac_bind_look_up_miss_match_mismatch_action(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_action_t        lumisAct, matchAct, mismatchAct;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_sec_ipMacBindAction_get(unit, &lumisAct, &matchAct, &mismatchAct), ret);

    if ('o' == TOKEN_CHAR(3,1))
    {
        /* look-up miss */
        diag_util_mprintf(" IP-MAC Bind - Look-up-miss Action : %s\n", diag_util_act2str(lumisAct));
    }
    else if ('a' == TOKEN_CHAR(3,1))
    {
        /* match */
        diag_util_mprintf(" IP-MAC Bind - Match Action : %s\n", diag_util_act2str(matchAct));
    }
    else if ('i' == TOKEN_CHAR(3,1))
    {
        /* mismatch */
        diag_util_mprintf(" IP-MAC Bind - Mismatch Action : %s\n", diag_util_act2str(mismatchAct));
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_security_get_ip_mac_bind_look_up_miss_match_mismatch_action */
#endif

#ifdef CMD_SECURITY_SET_PORT_PORTS_ALL_IP_MAC_BIND_IP_ARP_STATE_ENABLE_DISABLE
/*
 * security set port ( <PORT_LIST:ports> | all ) ip-mac-bind ( ip | arp ) state ( enable | disable )
 */
cparser_result_t
cparser_cmd_security_set_port_ports_all_ip_mac_bind_ip_arp_state_enable_disable(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_sec_ipMacBindPktType_t type = RTK_SEC_IPMACBIND_PKTTYPE_NONE;
    rtk_enable_t enable;
    rtk_port_t port;
    diag_portlist_t portmask;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (DIAG_UTIL_EXTRACT_PORTLIST(portmask, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    if ('i' == TOKEN_CHAR(5,0))
    {
        type = RTK_SEC_IPMACBIND_PKTTYPE_IP;
    }
    else if ('a' == TOKEN_CHAR(5,0))
    {
        type = RTK_SEC_IPMACBIND_PKTTYPE_ARP;
    }

    enable = ('e' == TOKEN_CHAR(7,0))? ENABLED : DISABLED;

    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_sec_portIpMacBindEnable_set(unit, port, type, enable), ret);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_security_set_port_ports_all_ip_mac_bind_ip_arp_state_enable_disable */
#endif

#ifdef CMD_SECURITY_GET_PORT_PORTS_ALL_IP_MAC_BIND_IP_ARP_STATE
/*
 * security get port ( <PORT_LIST:ports> | all ) ip-mac-bind ( ip | arp ) state
 */
cparser_result_t
cparser_cmd_security_get_port_ports_all_ip_mac_bind_ip_arp_state(
    cparser_context_t *context,
    char **ports_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_sec_ipMacBindPktType_t type = RTK_SEC_IPMACBIND_PKTTYPE_NONE;
    rtk_enable_t enable;
    rtk_port_t port;
    diag_portlist_t portmask;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_MPRINTF("IP-MAC binding status of ports\n");

    if (DIAG_UTIL_EXTRACT_PORTLIST(portmask, 3) != RT_ERR_OK)
    {
        return CPARSER_NOT_OK;
    }

    if ('i' == TOKEN_CHAR(5,0))
    {
        type = RTK_SEC_IPMACBIND_PKTTYPE_IP;
        DIAG_UTIL_MPRINTF("Packet type : IP packet\n");
    }
    else if ('a' == TOKEN_CHAR(5,0))
    {
        type = RTK_SEC_IPMACBIND_PKTTYPE_ARP;
        DIAG_UTIL_MPRINTF("Packet type : ARP packet\n");
    }

    DIAG_UTIL_PORTMASK_SCAN(portmask, port)
    {
        diag_util_printf("Port %u : ", port);

        DIAG_UTIL_ERR_CHK(rtk_sec_portIpMacBindEnable_get(unit, port, type, &enable), ret);

        if (ENABLED == enable)
        {
            DIAG_UTIL_MPRINTF("ENABLE\n");
        }
        else
        {
            DIAG_UTIL_MPRINTF("DISABLE\n");
        }
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_security_get_port_ports_all_ip_mac_bind_ip_arp_state */
#endif

#ifdef CMD_SECURITY_ADD_IP_MAC_BIND_ENTRY_IP_IP_MAC_MAC_VID_VID_ANY_PORT_TRUNK_ID_ANY
/*
 * security add ip-mac-bind entry ip <IPV4ADDR:ip> mac <MACADDR:mac> vid ( <UINT:vid> | any ) ( port | trunk ) ( <UINT:id> | any )
 */
cparser_result_t
cparser_cmd_security_add_ip_mac_bind_entry_ip_ip_mac_mac_vid_vid_any_port_trunk_id_any(
    cparser_context_t *context,
    uint32_t *ip_ptr,
    cparser_macaddr_t *mac_ptr,
    uint32_t *vid_ptr,
    uint32_t *id_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_sec_ipMacBindEntry_t entry;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();
    osal_memset(&entry, 0, sizeof(entry));
    entry.ipAddr = *ip_ptr;
    osal_memcpy(&entry.macAddr, mac_ptr, sizeof(rtk_mac_t));

    /* Flags */
    if ((NULL != vid_ptr) && ('a' != TOKEN_CHAR(9,0)))
    {
        entry.flags |= RTK_SEC_IPMACBIND_FLAG_BIND_VLAN;
        entry.vid = *vid_ptr;
    }
    if ((NULL != id_ptr) && ('a' != TOKEN_CHAR(11,0)))
    {
        entry.flags |= RTK_SEC_IPMACBIND_FLAG_BIND_PORT;
        if ('p' == TOKEN_CHAR(10,0))
        {
            entry.port = *id_ptr;
        }
        else
        {
            entry.flags |= RTK_SEC_IPMACBIND_FLAG_PORT_TRUNK;
            entry.trunkId = *id_ptr;
        }
    }
    DIAG_UTIL_ERR_CHK(rtk_sec_ipMacBindEntry_add(unit, &entry), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_security_add_ip_mac_bind_entry_ip_ip_mac_mac_vid_vid_any_port_trunk_id_any */
#endif

#ifdef CMD_SECURITY_DEL_IP_MAC_BIND_ENTRY_IP_IP_MAC_MAC_VID_VID_ANY_PORT_TRUNK_ID_ANY
/*
 * security del ip-mac-bind entry ip <IPV4ADDR:ip> mac <MACADDR:mac> vid ( <UINT:vid> | any ) ( port | trunk ) ( <UINT:id> | any )
 */
cparser_result_t
cparser_cmd_security_del_ip_mac_bind_entry_ip_ip_mac_mac_vid_vid_any_port_trunk_id_any(
    cparser_context_t *context,
    uint32_t *ip_ptr,
    cparser_macaddr_t *mac_ptr,
    uint32_t *vid_ptr,
    uint32_t *id_ptr)
{
    uint32  unit;
    int32   ret = RT_ERR_FAILED;
    rtk_sec_ipMacBindEntry_t entry;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();
    osal_memset(&entry, 0, sizeof(entry));
    entry.ipAddr = *ip_ptr;
    osal_memcpy(&entry.macAddr, mac_ptr, sizeof(rtk_mac_t));

    /* Flags */
    if ((NULL != vid_ptr) && ('a' != TOKEN_CHAR(9,0)))
    {
        entry.flags |= RTK_SEC_IPMACBIND_FLAG_BIND_VLAN;
        entry.vid = *vid_ptr;
    }
    if ((NULL != id_ptr) && ('a' != TOKEN_CHAR(11,0)))
    {
        entry.flags |= RTK_SEC_IPMACBIND_FLAG_BIND_PORT;
        if ('p' == TOKEN_CHAR(10,0))
        {
            entry.port = *id_ptr;
        }
        else
        {
            entry.flags |= RTK_SEC_IPMACBIND_FLAG_PORT_TRUNK;
            entry.trunkId = *id_ptr;
        }
    }
    DIAG_UTIL_ERR_CHK(rtk_sec_ipMacBindEntry_del(unit, &entry), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_security_del_ip_mac_bind_entry_ip_ip_mac_mac_vid_vid_any_port_trunk_id_any */
#endif

#ifdef CMD_SECURITY_DUMP_IP_MAC_BIND_ENTRY_FROM_BEGIN_INDEX_BEGIN_TO_END_INDEX_END
/*
 * security dump ip-mac-bind entry from ( <UINT:begin_index> | begin ) to ( <UINT:end_index> | end )
 */
cparser_result_t
cparser_cmd_security_dump_ip_mac_bind_entry_from_begin_index_begin_to_end_index_end(
    cparser_context_t *context,
    uint32_t *begin_index_ptr,
    uint32_t *end_index_ptr)
{
    uint32 unit;
    uint32 begin, end;
    int32  idx = 0;
    int32  ret = RT_ERR_FAILED;
    rtk_sec_ipMacBindEntry_t entry;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_MPRINTF("\nIndex IP address      MAC address        Port ID  VLAN\n");
    DIAG_UTIL_MPRINTF("----- --------------- ----------------- ----- --- ----\n");

    begin = ((NULL != begin_index_ptr) && ('b' != TOKEN_CHAR(5,0)))? *begin_index_ptr : 0;
    end = ((NULL != end_index_ptr) && ('e' != TOKEN_CHAR(7,0)))? *end_index_ptr : (uint32)-1;
    for (idx = begin; idx <= end ; idx++)
    {
        osal_memset(&entry, 0x00, sizeof(entry));

        DIAG_UTIL_ERR_CHK(rtk_sec_ipMacBindEntry_getNext(unit, &idx, &entry), ret);

        if (-1 == idx)
            break;

        DUMP_IPMACBIND_ENTRY(idx, entry);
    }
    DIAG_UTIL_MPRINTF("\n");

    return CPARSER_OK;
}   /* end of cparser_cmd_security_dump_ip_mac_bind_entry_from_begin_index_begin_to_end_index_end */
#endif

#ifdef CMD_SECURITY_SET_ATTACK_PREVENT_IP4_INVALID_LEN_ACTION_DROP_FORWARD_TRAP_TO_CPU
/*
 * security set attack-prevent ip4-invalid-len action ( drop | forward | trap-to-cpu )
 */
cparser_result_t
cparser_cmd_security_set_attack_prevent_ip4_invalid_len_action_drop_forward_trap_to_cpu(
    cparser_context_t *context)
{
    rtk_sec_attackType_t    type;
    rtk_action_t            action;
    uint32                  unit;
    int32                   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    type = IPV4_INVALID_LEN;

    if ('d' == TOKEN_CHAR(5,0))
        action = ACTION_DROP;
    else if ('f' == TOKEN_CHAR(5,0))
        action = ACTION_FORWARD;
    else if ('t' == TOKEN_CHAR(5,0))
        action = ACTION_TRAP2CPU;
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_sec_attackPreventAction_set(unit, type, action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_security_set_attack_prevent_ip4_invalid_len_action_drop_forward_trap_to_cpu */
#endif

#ifdef CMD_SECURITY_SET_ATTACK_PREVENT_IP4_INVALID_HEADER_ACTION_DROP_FORWARD_TRAP_TO_CPU
/*
 * security set attack-prevent ip4-invalid-header action ( drop | forward | trap-to-cpu )
 */
cparser_result_t
cparser_cmd_security_set_attack_prevent_ip4_invalid_header_action_drop_forward_trap_to_cpu(
    cparser_context_t *context)
{
    rtk_sec_attackType_t    type;
    rtk_action_t            action;
    uint32                  unit;
    int32                   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    type = IPV4_INVALID_HDR;

    if ('d' == TOKEN_CHAR(5,0))
        action = ACTION_DROP;
    else if ('f' == TOKEN_CHAR(5,0))
        action = ACTION_FORWARD;
    else if ('t' == TOKEN_CHAR(5,0))
        action = ACTION_TRAP2CPU;
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_sec_attackPreventAction_set(unit, type, action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_security_set_attack_prevent_ip4_invalid_header_action_drop_forward_trap_to_cpu */
#endif

#ifdef CMD_SECURITY_DUMP_ATTACK_PREVENT_HIT_INDICATION
/*
 * security dump attack-prevent hit-indication
 */
cparser_result_t
cparser_cmd_security_dump_attack_prevent_hit_indication(
    cparser_context_t *context)
{
    uint32  unit;
    uint32  hit;
    int32   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_sec_attackPreventHit_get(unit, TCP_FRAG_OFF_MIN_CHECK, &hit), ret);
    diag_util_mprintf("TCP_FRAG_OFF_MIN_CHECK: %d\n", hit);

    DIAG_UTIL_ERR_CHK(rtk_sec_attackPreventHit_get(unit, SYNRST_DENY, &hit), ret);
    diag_util_mprintf("SYNRST_DENY: %d\n", hit);

    DIAG_UTIL_ERR_CHK(rtk_sec_attackPreventHit_get(unit, SYNFIN_DENY, &hit), ret);
    diag_util_mprintf("SYNFIN_DENY: %d\n", hit);

    DIAG_UTIL_ERR_CHK(rtk_sec_attackPreventHit_get(unit, XMA_DENY, &hit), ret);
    diag_util_mprintf("XMA_DENY: %d\n", hit);

    DIAG_UTIL_ERR_CHK(rtk_sec_attackPreventHit_get(unit, NULLSCAN_DENY, &hit), ret);
    diag_util_mprintf("NULLSCAN_DENY: %d\n", hit);

    DIAG_UTIL_ERR_CHK(rtk_sec_attackPreventHit_get(unit, SYN_SPORTL1024_DENY, &hit), ret);
    diag_util_mprintf("SYN_SPORTL1024_DENY: %d\n", hit);

    DIAG_UTIL_ERR_CHK(rtk_sec_attackPreventHit_get(unit, TCPHDR_MIN_CHECK, &hit), ret);
    diag_util_mprintf("TCPHDR_MIN_CHECK: %d\n", hit);

    DIAG_UTIL_ERR_CHK(rtk_sec_attackPreventHit_get(unit, SMURF_DENY, &hit), ret);
    diag_util_mprintf("SMURF_DENY: %d\n", hit);

    DIAG_UTIL_ERR_CHK(rtk_sec_attackPreventHit_get(unit, ICMPV6_PING_MAX_CHECK, &hit), ret);
    diag_util_mprintf("ICMPV6_PING_MAX_CHECK: %d\n", hit);

    DIAG_UTIL_ERR_CHK(rtk_sec_attackPreventHit_get(unit, ICMPV4_PING_MAX_CHECK, &hit), ret);
    diag_util_mprintf("ICMPV4_PING_MAX_CHECK: %d\n", hit);

    DIAG_UTIL_ERR_CHK(rtk_sec_attackPreventHit_get(unit, ICMP_FRAG_PKTS_DENY, &hit), ret);
    diag_util_mprintf("ICMP_FRAG_PKTS_DENY: %d\n", hit);

    DIAG_UTIL_ERR_CHK(rtk_sec_attackPreventHit_get(unit, IPV6_MIN_FRAG_SIZE_CHECK, &hit), ret);
    diag_util_mprintf("IPV6_MIN_FRAG_SIZE_CHECK: %d\n", hit);

    DIAG_UTIL_ERR_CHK(rtk_sec_attackPreventHit_get(unit, POD_DENY, &hit), ret);
    diag_util_mprintf("POD_DENY: %d\n", hit);

    DIAG_UTIL_ERR_CHK(rtk_sec_attackPreventHit_get(unit, TCPBLAT_DENY, &hit), ret);
    diag_util_mprintf("TCPBLAT_DENY: %d\n", hit);

    DIAG_UTIL_ERR_CHK(rtk_sec_attackPreventHit_get(unit, UDPBLAT_DENY, &hit), ret);
    diag_util_mprintf("UDPBLAT_DENY: %d\n", hit);

    DIAG_UTIL_ERR_CHK(rtk_sec_attackPreventHit_get(unit, LAND_DENY, &hit), ret);
    diag_util_mprintf("LAND_DENY: %d\n", hit);

    DIAG_UTIL_ERR_CHK(rtk_sec_attackPreventHit_get(unit, DAEQSA_DENY, &hit), ret);
    diag_util_mprintf("DAEQSA_DENY: %d\n", hit);

    DIAG_UTIL_ERR_CHK(rtk_sec_attackPreventHit_get(unit, IPV4_INVALID_HDR, &hit), ret);
    diag_util_mprintf("IPV4_INVALID_HDR: %d\n", hit);

    return CPARSER_OK;
}   /* end of cparser_cmd_security_dump_attack_prevent_hit_indication */
#endif

