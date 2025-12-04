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
#include <parser/cparser_priv.h>
#include <diag_str.h>
#include <rtk/l2.h>
#include <rtk/trap.h>
#ifdef CONFIG_SDK_APOLLO
#include <dal/apollo/raw/apollo_raw_trap.h>
#endif
#ifdef CONFIG_SDK_APOLLOMP
#include <dal/apollomp/dal_apollomp.h>
#include <dal/apollomp/raw/apollomp_raw_l2.h>
#include <hal/chipdef/apollomp/rtk_apollomp_reg_struct.h>
#endif

typedef enum diag_igmpChecksumErrAction_e
{
    DIAG_CHECKSUM_ERR_FORWARD = 0,
    DIAG_CHECKSUM_ERR_DROP,
    DIAG_CHECKSUM_ERR_TRAP,
    DIAG_CHECKSUM_ERR_END,
}diag_igmpChecksumErrAction_t;

typedef enum diag_igmpAction_e
{
    DIAG_IGMP_ACT_FORWARD  = 0,
    DIAG_IGMP_ACT_DROP,
    DIAG_IGMP_ACT_TRAP,
    DIAG_IGMP_ACT_END,
} diag_igmpAction_t;



/*
 * igmp get ip-mcast-lookup-mode
 */
cparser_result_t
cparser_cmd_igmp_get_ip_mcast_lookup_mode(
    cparser_context_t *context)
{
    int32 ret;
    rtk_l2_ipmcHashOp_t mode;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_l2_ipmcGroupLookupMissHash_get(&mode), ret);
    diag_util_printf("\n IPMC lookup mode: %s", diagStr_l2IpMcHashOpStr[(HASH_DIP_AND_SIP == mode) ? 1 : 0]);

    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_igmp_get_ip_mcast_lookup_mode */

/*
 * igmp set ip-mcast-lookup-mode ( dip-and-sip | dip-only )
 */
cparser_result_t
cparser_cmd_igmp_set_ip_mcast_lookup_mode_dip_and_sip_dip_only(
    cparser_context_t *context)
{
    int32 ret;
    rtk_l2_ipmcHashOp_t mode;

    DIAG_UTIL_PARAM_CHK();

    if('a' == TOKEN_CHAR(3, 4))
        mode = HASH_DIP_AND_SIP;
    else if('o' == TOKEN_CHAR(3, 4))
        mode = HASH_DIP_ONLY;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_l2_ipmcGroupLookupMissHash_set(mode), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_igmp_set_ip_mcast_lookup_mode_dip_and_sip_dip_only */


/*
 * igmp set ( igmpv1 | igmpv2 | igmpv3 | mldv1 | mldv2 ) port ( <PORT_LIST:ports> | all ) action ( drop | forward | trap-to-cpu )
 */
cparser_result_t
cparser_cmd_igmp_set_igmpv1_igmpv2_igmpv3_mldv1_mldv2_port_ports_all_action_drop_forward_trap_to_cpu(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rtk_trap_igmpMld_type_t type;
    rtk_action_t action;
    rtk_port_t port;
    diag_portlist_t portlist;
    uint32 field;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if ('i' == TOKEN_CHAR(2, 0))
    {
        if ('1' == TOKEN_CHAR(2, 5))
            type = IGMPMLD_TYPE_IGMPV1;
        else if ('2' == TOKEN_CHAR(2, 5))
            type = IGMPMLD_TYPE_IGMPV2;
        else if ('3' == TOKEN_CHAR(2, 5))
            type = IGMPMLD_TYPE_IGMPV3;
        else
            return CPARSER_NOT_OK;
    }
    else if ('m' == TOKEN_CHAR(2, 0))
    {
        if ('1' == TOKEN_CHAR(2, 4))
            type = IGMPMLD_TYPE_MLDV1;
        else if ('2' == TOKEN_CHAR(2, 4))
            type = IGMPMLD_TYPE_MLDV2;
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
        DIAG_UTIL_ERR_CHK(rtk_trap_portIgmpMldCtrlPktAction_set(port, type, action), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_igmp_set_igmpv1_igmpv2_igmpv3_mldv1_mldv2_port_ports_all_action_drop_forward_trap_to_cpu */

/*
 * igmp get ( igmpv1 | igmpv2 | igmpv3 | mldv1 | mldv2 ) port ( <PORT_LIST:ports> | all ) action
 */
cparser_result_t
cparser_cmd_igmp_get_igmpv1_igmpv2_igmpv3_mldv1_mldv2_port_ports_all_action(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rtk_trap_igmpMld_type_t type;
    rtk_action_t action;
    rtk_port_t port;
    diag_portlist_t portlist;
    uint32 igmpAct;
    uint32 field;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if ('i' == TOKEN_CHAR(2, 0))
    {
        if ('1' == TOKEN_CHAR(2, 5))
            type = IGMPMLD_TYPE_IGMPV1;
        else if ('2' == TOKEN_CHAR(2, 5))
            type = IGMPMLD_TYPE_IGMPV2;
        else if ('3' == TOKEN_CHAR(2, 5))
            type = IGMPMLD_TYPE_IGMPV3;
        else
            return CPARSER_NOT_OK;
    }
    else if ('m' == TOKEN_CHAR(2, 0))
    {
        if ('1' == TOKEN_CHAR(2, 4))
            type = IGMPMLD_TYPE_MLDV1;
        else if ('2' == TOKEN_CHAR(2, 4))
            type = IGMPMLD_TYPE_MLDV2;
        else
            return CPARSER_NOT_OK;
    }
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_trap_portIgmpMldCtrlPktAction_get(port, type, &action), ret);
        diag_util_printf("\n Port: %d, %s : %s", port, diagStr_igmpTypeStr[type], diagStr_actionStr[action]);
    }

    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_igmp_get_igmpv1_igmpv2_igmpv3_mldv1_mldv2_port_ports_all_action */

#if (!defined(CONFIG_REDUCED_DIAG))
/*
 * igmp set ip-mcast-table index <UINT:index> group-ip <IPV4ADDR:dip> port ( <PORT_LIST:ports> | all | none )
 */
cparser_result_t
cparser_cmd_igmp_set_ip_mcast_table_index_index_group_ip_dip_port_ports_all_none(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *dip_ptr,
    char * *ports_ptr)
{
    int32 ret;
    rtk_port_t port;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 8), ret);

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollo_raw_l2_igmp_Mc_table_set(*index_ptr, (rtk_ip_addr_t)*dip_ptr, portlist.portmask.bits[0]), ret);
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollomp_raw_l2_igmp_Mc_table_set(*index_ptr, (rtk_ip_addr_t)*dip_ptr, portlist.portmask.bits[0]), ret);
            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_igmp_set_ip_mcast_table_index_index_group_ip_dip_port_ports_all_none */

/*
 * igmp get ip-mcast-table index <UINT:index>
 */
cparser_result_t
cparser_cmd_igmp_get_ip_mcast_table_index_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32 ret;
    rtk_portmask_t portmask;
    rtk_ip_addr_t dip;
    uint8 strDip[64];

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollo_raw_l2_igmp_Mc_table_get(*index_ptr, &dip, &portmask), ret);

            diag_util_ip2str(strDip, dip);
            diag_util_printf("\n Index     : %d", *index_ptr);
            diag_util_printf("\n DIP       : %s", strDip);
            diag_util_printf("\n Portmask  : %s", diag_util_mask32tostr(portmask.bits[0]));
            diag_util_printf("\n");
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollomp_raw_l2_igmp_Mc_table_get(*index_ptr, &dip, (uint32 *)&portmask), ret);

            diag_util_ip2str(strDip, dip);
            diag_util_printf("\n Index     : %d", *index_ptr);
            diag_util_printf("\n DIP       : %s", strDip);
            diag_util_printf("\n Portmask  : %s", diag_util_mask32tostr(portmask.bits[0]));
            diag_util_printf("\n");
            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_igmp_get_ip_mcast_table_index_index */

/*
 * igmp set ip-mcast-table group-ip <IPV4ADDR:dip> port ( <PORT_LIST:ports> | all | none )
 */
cparser_result_t
cparser_cmd_igmp_set_ip_mcast_table_group_ip_dip_port_ports_all_none(
    cparser_context_t *context,
    uint32_t  *dip_ptr,
    char * *ports_ptr)
{
    int32 ret;
    rtk_port_t port;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6), ret);

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(rtk_l2_ipmcGroup_add((rtk_ip_addr_t)*dip_ptr, &portlist.portmask), ret);
            break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(rtk_l2_ipmcGroup_add((rtk_ip_addr_t)*dip_ptr, &portlist.portmask), ret);
            break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
            DIAG_UTIL_ERR_CHK(rtk_l2_ipmcGroup_add((rtk_ip_addr_t)*dip_ptr, &portlist.portmask), ret);
            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_igmp_set_ip_mcast_table_group_ip_dip_port_ports_all_none */

/*
 * igmp set ip-mcast-table group-ip <IPV4ADDR:dip> ext-port-idx <UINT:index>
 */
cparser_result_t
cparser_cmd_igmp_set_ip_mcast_table_group_ip_dip_ext_port_idx_index(
    cparser_context_t *context,
    uint32_t  *dip_ptr,
    uint32_t  *index_ptr)
{
    int32 ret;
    rtk_port_t port;

    DIAG_UTIL_PARAM_CHK();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(rtk_l2_ipmcGroupExtMemberIdx_add((rtk_ip_addr_t)*dip_ptr, *index_ptr), ret);
            break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(rtk_l2_ipmcGroupExtMemberIdx_add((rtk_ip_addr_t)*dip_ptr, *index_ptr), ret);
            break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
            DIAG_UTIL_ERR_CHK(rtk_l2_ipmcGroupExtMemberIdx_add((rtk_ip_addr_t)*dip_ptr, *index_ptr), ret);
            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_igmp_set_ip_mcast_table_group_ip_dip_ext_port_idx_index */

/*
 * igmp get ip-mcast-table group-ip <IPV4ADDR:dip>
 */
cparser_result_t
cparser_cmd_igmp_get_ip_mcast_table_group_ip_dip(
    cparser_context_t *context,
    uint32_t  *dip_ptr)
{
    int32 ret;
    rtk_portmask_t portmask;
    uint8 strDip[64];
    uint32 index;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(rtk_l2_ipmcGroup_get((ipaddr_t )*dip_ptr, &portmask), ret);

            diag_util_ip2str(strDip, *dip_ptr);
            diag_util_printf("\n DIP       : %s", strDip);
            diag_util_printf("\n Portmask  : %s", diag_util_mask32tostr(portmask.bits[0]));
            diag_util_printf("\n");
            break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(rtk_l2_ipmcGroup_get((ipaddr_t)*dip_ptr, &portmask), ret);
            DIAG_UTIL_ERR_CHK(rtk_l2_ipmcGroupExtMemberIdx_get((ipaddr_t)*dip_ptr, (uint32 *)&index), ret);

            diag_util_ip2str(strDip, *dip_ptr);
            diag_util_printf("\n DIP       : %s", strDip);
            diag_util_printf("\n Portmask  : %s", diag_util_mask32tostr(portmask.bits[0]));
            diag_util_printf("\n ExtMbrIdx : %u", index);
            diag_util_printf("\n");
            break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
            DIAG_UTIL_ERR_CHK(rtk_l2_ipmcGroup_get((ipaddr_t)*dip_ptr, &portmask), ret);
            DIAG_UTIL_ERR_CHK(rtk_l2_ipmcGroupExtMemberIdx_get((ipaddr_t)*dip_ptr, (uint32 *)&index), ret);

            diag_util_ip2str(strDip, *dip_ptr);
            diag_util_printf("\n DIP       : %s", strDip);
            diag_util_printf("\n Portmask  : %s", diag_util_mask32tostr(portmask.bits[0]));
            diag_util_printf("\n ExtMbrIdx : %u", index);
            diag_util_printf("\n");
            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_igmp_get_ip_mcast_table_group_ip_dip */

/*
 * igmp del ip-mcast-table group-ip <IPV4ADDR:dip>
 */
cparser_result_t
cparser_cmd_igmp_del_ip_mcast_table_group_ip_dip(
    cparser_context_t *context,
    uint32_t  *dip_ptr)
{
    int32 ret;
    rtk_portmask_t portmask;
    uint8 strDip[64];
    uint32 index;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(rtk_l2_ipmcGroup_del((ipaddr_t)*dip_ptr), ret);
            break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
        case RTL9607C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(rtk_l2_ipmcGroup_del((ipaddr_t)*dip_ptr), ret);
            break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
        case RTL9603CVD_CHIP_ID:
            DIAG_UTIL_ERR_CHK(rtk_l2_ipmcGroup_del((ipaddr_t)*dip_ptr), ret);
            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_igmp_del_ip_mcast_table_group_ip_dip */

/*
 * igmp set ip-mcast-sip-table index <UINT:index> sip <IPV4ADDR:dip>
 */
cparser_result_t
cparser_cmd_igmp_set_ip_mcast_sip_table_index_index_sip_dip(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *dip_ptr)
{
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(rtk_l2_ipmcSipFilter_set(*index_ptr, *dip_ptr), ret);
            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_igmp_set_ip_mcast_sip_table_index_index_sip_dip */

/*
 * igmp get ip-mcast-sip-table index <UINT:index>
 */
cparser_result_t
cparser_cmd_igmp_get_ip_mcast_sip_table_index_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32 ret;
    rtk_ip_addr_t sip;
    uint8 strDip[64];

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_RTL9602C)
        case RTL9602C_CHIP_ID:
            DIAG_UTIL_ERR_CHK(rtk_l2_ipmcSipFilter_get(*index_ptr, &sip), ret);

            diag_util_ip2str(strDip, sip);
            diag_util_printf("\n Index     : %d", *index_ptr);
            diag_util_printf("\n SIP       : %s", strDip);
            diag_util_printf("\n");

            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_igmp_get_ip_mcast_sip_table_index_index */

/*
 * igmp set igmp-mld ( vlan-leaky | isolation-leaky ) state ( disable | enable )
 */
cparser_result_t
cparser_cmd_igmp_set_igmp_mld_vlan_leaky_isolation_leaky_state_disable_enable(
    cparser_context_t *context)
{
    int32 ret;
    rtk_enable_t state;

    DIAG_UTIL_PARAM_CHK();

    if('e' == TOKEN_CHAR(5, 0))
        state = ENABLED;
    else if('d' == TOKEN_CHAR(5, 0))
        state = DISABLED;
    else
        return CPARSER_NOT_OK;


    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            if('v' == TOKEN_CHAR(3, 0))
            {
                DIAG_UTIL_ERR_CHK(apollo_raw_trap_igmpVLANLeaky_set(state), ret);
                diag_util_printf("\n");
            }
            else if('i' == TOKEN_CHAR(3, 0))
            {
                DIAG_UTIL_ERR_CHK(apollo_raw_trap_igmpIsoLeaky_set(state), ret);
                diag_util_printf("\n");
            }
            else
                return CPARSER_NOT_OK;

            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            if('v' == TOKEN_CHAR(3, 0))
            {
                DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_IGMP_GLB_CTRLr, APOLLOMP_VLAN_LEAKYf, (uint32 *)&state), ret);
                diag_util_printf("\n");

            }
            else if('i' == TOKEN_CHAR(3, 0))
            {
                DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_IGMP_GLB_CTRLr, APOLLOMP_PISO_LEAKYf, (uint32 *)&state), ret);
                diag_util_printf("\n");

            }
            else
                return CPARSER_NOT_OK;
            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_igmp_set_igmp_mld_vlan_leaky_isolation_leaky_state_disable_enable */

/*
 * igmp get igmp-mld ( vlan-leaky | isolation-leaky ) state
 */
cparser_result_t
cparser_cmd_igmp_get_igmp_mld_vlan_leaky_isolation_leaky_state(
    cparser_context_t *context)
{
    int32 ret;
    rtk_enable_t state;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            if('v' == TOKEN_CHAR(3, 0))
            {
                DIAG_UTIL_ERR_CHK(apollo_raw_trap_igmpVLANLeaky_get(&state), ret);
                diag_util_printf("IGMP/MLD VLAN Leaky: %s", diagStr_enable[state]);
            }
            else if('i' == TOKEN_CHAR(3, 0))
            {
                DIAG_UTIL_ERR_CHK(apollo_raw_trap_igmpIsoLeaky_get(&state), ret);
                diag_util_printf("IGMP/MLD Isolation Leaky: %s", diagStr_enable[state]);
            }
            else
                return CPARSER_NOT_OK;

            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            if('v' == TOKEN_CHAR(3, 0))
            {
                DIAG_UTIL_ERR_CHK(reg_field_read(APOLLOMP_IGMP_GLB_CTRLr, APOLLOMP_VLAN_LEAKYf, &state), ret);

                diag_util_printf("IGMP/MLD VLAN Leaky: %s\n", diagStr_enable[state]);
            }
            else if('i' == TOKEN_CHAR(3, 0))
            {
                DIAG_UTIL_ERR_CHK(reg_field_read(APOLLOMP_IGMP_GLB_CTRLr, APOLLOMP_PISO_LEAKYf, &state), ret);

                diag_util_printf("IGMP/MLD Isolation Leaky: %s\n", diagStr_enable[state]);
            }
            else
                return CPARSER_NOT_OK;
            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_igmp_get_igmp_mld_vlan_leaky_isolation_leaky_state */

/*
 * igmp set igmp-mld checksum-error action ( drop | trap-to-cpu | forward )
 */
cparser_result_t
cparser_cmd_igmp_set_igmp_mld_checksum_error_action_drop_trap_to_cpu_forward(
    cparser_context_t *context)
{
    int32 ret;
    rtk_action_t action;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            if('d' == TOKEN_CHAR(5, 0))
                action = ACTION_DROP;
            else if('f' == TOKEN_CHAR(5, 0))
                action = ACTION_FORWARD;
            else if('t' == TOKEN_CHAR(5, 0))
                action = ACTION_TRAP2CPU;
            else
                return CPARSER_NOT_OK;

            DIAG_UTIL_ERR_CHK(apollo_raw_trap_igmpChechsumError_set(action), ret);
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            if('d' == TOKEN_CHAR(5, 0))
                action = DIAG_CHECKSUM_ERR_DROP;
            else if('f' == TOKEN_CHAR(5, 0))
                action = DIAG_CHECKSUM_ERR_FORWARD;
            else if('t' == TOKEN_CHAR(5, 0))
                action = DIAG_CHECKSUM_ERR_TRAP;
            else
                return CPARSER_NOT_OK;

            DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_IGMP_GLB_CTRLr, APOLLOMP_CKS_ERR_OPf, &action), ret);

            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_igmp_set_igmp_mld_checksum_error_action_drop_trap_to_cpu_forward */

/*
 * igmp get igmp-mld checksum-error action
 */
cparser_result_t
cparser_cmd_igmp_get_igmp_mld_checksum_error_action(
    cparser_context_t *context)
{
    int32 ret;
    rtk_action_t action;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollo_raw_trap_igmpChechsumError_get(&action), ret);
            diag_util_printf("IGMP/MLD Checksum error Action: %s\n", diagStr_actionStr[action]);
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(APOLLOMP_IGMP_GLB_CTRLr, APOLLOMP_CKS_ERR_OPf, &action), ret);

            switch(action)
            {
                case DIAG_CHECKSUM_ERR_FORWARD:
                    action = ACTION_FORWARD;
                    diag_util_printf("IGMP/MLD Checksum error Action: %s\n", diagStr_actionStr[action]);
                    break;
                case DIAG_CHECKSUM_ERR_TRAP:
                    action = ACTION_TRAP2CPU;
                    diag_util_printf("IGMP/MLD Checksum error Action: %s\n", diagStr_actionStr[action]);
                    break;
                case DIAG_CHECKSUM_ERR_DROP:
                    action = ACTION_DROP;
                    diag_util_printf("IGMP/MLD Checksum error Action: %s\n", diagStr_actionStr[action]);
                    break;
                default:
                    return RT_ERR_CHIP_NOT_SUPPORTED;
            }
            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }


    return CPARSER_OK;
}    /* end of cparser_cmd_igmp_get_igmp_mld_checksum_error_action */

/*
 * igmp set igmp-mld priority <UINT:priority>
 */
cparser_result_t
cparser_cmd_igmp_set_igmp_mld_priority_priority(
    cparser_context_t *context,
    uint32_t  *priority_ptr)
{
    int32 ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(rtk_trap_reasonTrapToCpuPriority_set(TRAP_REASON_IPV4IGMP, *priority_ptr), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_igmp_set_igmp_mld_priority_priority */

/*
 * igmp get igmp-mld priority
 */
cparser_result_t
cparser_cmd_igmp_get_igmp_mld_priority(
    cparser_context_t *context)
{
    int32 ret;
    rtk_pri_t priority;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_trap_reasonTrapToCpuPriority_get(TRAP_REASON_IPV4IGMP, &priority), ret);

    diag_util_printf("\n IGMP/MLD trap-priority: %d", priority);
    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_igmp_get_igmp_mld_priority */

#endif

