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
#include <hal/chipdef/apollo/apollo_reg_struct.h>
#include <hal/chipdef/apollomp/rtk_apollomp_reg_struct.h>
#include <rtk/rate.h>
#include <hal/mac/mem.h>
#include <hal/mac/reg.h>

/*
 * storm-control get ( broadcast | multicast | unknown-multicast | unknown-unicast | arp-storm | dhcp-storm | igmp-mld-storm )
 */
cparser_result_t
cparser_cmd_storm_control_get_broadcast_multicast_unknown_multicast_unknown_unicast_arp_storm_dhcp_storm_igmp_mld_storm(
    cparser_context_t *context)
{
    int32 ret;
    diag_portlist_t portlist;
    rtk_port_t port;
    rtk_rate_storm_group_ctrl_t  stormCtrl;
    rtk_enable_t state;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_rate_stormControlEnable_get(&stormCtrl), ret);

    if(!osal_strcmp(TOKEN_STR(2),"broadcast"))
        state = stormCtrl.broadcast_enable;
    else if(!osal_strcmp(TOKEN_STR(2),"multicast"))
        state = stormCtrl.multicast_enable;
    else if(!osal_strcmp(TOKEN_STR(2),"unknown-multicast"))
        state = stormCtrl.unknown_multicast_enable;
    else if(!osal_strcmp(TOKEN_STR(2),"unknown-unicast"))
        state = stormCtrl.unknown_unicast_enable;
    else if(!osal_strcmp(TOKEN_STR(2),"dhcp-storm"))
        state = stormCtrl.dhcp_enable;
    else if(!osal_strcmp(TOKEN_STR(2),"arp-storm"))
        state = stormCtrl.arp_enable;
    else if(!osal_strcmp(TOKEN_STR(2),"igmp-mld-storm"))
        state = stormCtrl.igmp_mld_enable;
    else
        return CPARSER_ERR_INVALID_PARAMS;

    diag_util_mprintf("State: %s\n", diagStr_enable[state]);

    return CPARSER_OK;
}    /* end of cparser_cmd_storm_control_get_broadcast_multicast_unknown_multicast_unknown_unicast_dhcp_arp_igmp_mld */

/*
 * storm-control set ( broadcast | multicast | unknown-multicast | unknown-unicast | arp-storm | dhcp-storm | igmp-mld-storm ) state ( disable | enable )
 */
cparser_result_t
cparser_cmd_storm_control_set_broadcast_multicast_unknown_multicast_unknown_unicast_arp_storm_dhcp_storm_igmp_mld_storm_state_disable_enable(
    cparser_context_t *context)
{
    int32 ret;
    rtk_enable_t state;
    rtk_rate_storm_group_ctrl_t  stormCtrl;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_rate_stormControlEnable_get(&stormCtrl), ret);

    if ('d' == TOKEN_CHAR(4, 0))
        state = DISABLED;
    else if ('e' == TOKEN_CHAR(4, 0))
        state = ENABLED;
    else
        return CPARSER_ERR_INVALID_PARAMS;

    if(!osal_strcmp(TOKEN_STR(2),"broadcast"))
        stormCtrl.broadcast_enable = state;
    else if(!osal_strcmp(TOKEN_STR(2),"multicast"))
        stormCtrl.multicast_enable = state;
    else if(!osal_strcmp(TOKEN_STR(2),"unknown-multicast"))
        stormCtrl.unknown_multicast_enable = state;
    else if(!osal_strcmp(TOKEN_STR(2),"unknown-unicast"))
        stormCtrl.unknown_unicast_enable = state;
    else if(!osal_strcmp(TOKEN_STR(2),"dhcp-storm"))
        stormCtrl.dhcp_enable = state;
    else if(!osal_strcmp(TOKEN_STR(2),"arp-storm"))
        stormCtrl.arp_enable = state;
    else if(!osal_strcmp(TOKEN_STR(2),"igmp-mld-storm"))
        stormCtrl.igmp_mld_enable = state;
    else
        return CPARSER_ERR_INVALID_PARAMS;

    DIAG_UTIL_ERR_CHK(rtk_rate_stormControlEnable_set(&stormCtrl), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_storm_control_set_broadcast_multicast_unknown_multicast_unknown_unicast_arp_storm_dhcp_storm_igmp_mld_storm_state_disable_enable */

/*
 * storm-control get ( broadcast | multicast | unknown-multicast | unknown-unicast | arp-storm | dhcp-storm | igmp-mld-storm ) port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_storm_control_get_broadcast_multicast_unknown_multicast_unknown_unicast_arp_storm_dhcp_storm_igmp_mld_storm_port_ports_all(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    diag_portlist_t portlist;
    rtk_port_t port;
    rtk_rate_storm_group_t  stormType;
    rtk_enable_t state;
    uint32 meter;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    if(!osal_strcmp(TOKEN_STR(2),"broadcast"))
        stormType = STORM_GROUP_BROADCAST;
    else if(!osal_strcmp(TOKEN_STR(2),"multicast"))
        stormType = STORM_GROUP_MULTICAST;
    else if(!osal_strcmp(TOKEN_STR(2),"unknown-multicast"))
        stormType = STORM_GROUP_UNKNOWN_MULTICAST;
    else if(!osal_strcmp(TOKEN_STR(2),"unknown-unicast"))
        stormType = STORM_GROUP_UNKNOWN_UNICAST;
    else if(!osal_strcmp(TOKEN_STR(2),"dhcp-storm"))
        stormType = STORM_GROUP_DHCP;
    else if(!osal_strcmp(TOKEN_STR(2),"arp-storm"))
        stormType = STORM_GROUP_ARP;
    else if(!osal_strcmp(TOKEN_STR(2),"igmp-mld-storm"))
        stormType = STORM_GROUP_IGMP_MLD;
    else
        return CPARSER_ERR_INVALID_PARAMS;

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    diag_util_mprintf("Port State   Meter\n");

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_stormControlPortEnable_get(port, stormType, &state), ret);
        DIAG_UTIL_ERR_CHK(rtk_rate_stormControlMeterIdx_get(port, stormType, &meter), ret);

        diag_util_mprintf("%-4d %-7s %d\n", port, diagStr_enable[state], meter);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_storm_control_get_broadcast_multicast_unknown_multicast_unknown_unicast_arp_storm_dhcp_storm_igmp_mld_storm_port_ports_all */

/*
 * storm-control set ( broadcast | multicast | unknown-multicast | unknown-unicast | arp-storm | dhcp-storm | igmp-mld-storm ) port ( <PORT_LIST:ports> | all ) state ( disable | enable )
 */
cparser_result_t
cparser_cmd_storm_control_set_broadcast_multicast_unknown_multicast_unknown_unicast_arp_storm_dhcp_storm_igmp_mld_storm_port_ports_all_state_disable_enable(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    diag_portlist_t portlist;
    rtk_port_t port;
    rtk_rate_storm_group_t  stormType;
    rtk_enable_t state;

    DIAG_UTIL_PARAM_CHK();

    if(!osal_strcmp(TOKEN_STR(2),"broadcast"))
        stormType = STORM_GROUP_BROADCAST;
    else if(!osal_strcmp(TOKEN_STR(2),"multicast"))
        stormType = STORM_GROUP_MULTICAST;
    else if(!osal_strcmp(TOKEN_STR(2),"unknown-multicast"))
        stormType = STORM_GROUP_UNKNOWN_MULTICAST;
    else if(!osal_strcmp(TOKEN_STR(2),"unknown-unicast"))
        stormType = STORM_GROUP_UNKNOWN_UNICAST;
    else if(!osal_strcmp(TOKEN_STR(2),"dhcp-storm"))
        stormType = STORM_GROUP_DHCP;
    else if(!osal_strcmp(TOKEN_STR(2),"arp-storm"))
        stormType = STORM_GROUP_ARP;
    else if(!osal_strcmp(TOKEN_STR(2),"igmp-mld-storm"))
        stormType = STORM_GROUP_IGMP_MLD;
    else
        return CPARSER_ERR_INVALID_PARAMS;

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if ('d' == TOKEN_CHAR(6, 0))
        state = DISABLED;
    else if ('e' == TOKEN_CHAR(6, 0))
        state = ENABLED;
    else
        return CPARSER_ERR_INVALID_PARAMS;


    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_stormControlPortEnable_set(port, stormType, state), ret);
    }


    return CPARSER_OK;
}    /* end of cparser_cmd_storm_control_set_broadcast_multicast_unknown_multicast_unknown_unicast_arp_storm_dhcp_storm_igmp_mld_storm_port_ports_all_state_disable_enable */

/*
 * storm-control set ( broadcast | multicast | unknown-multicast | unknown-unicast | arp-storm | dhcp-storm | igmp-mld-storm ) port ( <PORT_LIST:ports> | all ) meter <UINT:index>
 */
cparser_result_t
cparser_cmd_storm_control_set_broadcast_multicast_unknown_multicast_unknown_unicast_arp_storm_dhcp_storm_igmp_mld_storm_port_ports_all_meter_index(
    cparser_context_t *context,
    char * *ports_ptr,
    uint32_t  *index_ptr)
{
    int32 ret;
    diag_portlist_t portlist;
    rtk_port_t port;
    rtk_rate_storm_group_t  stormType;
    rtk_enable_t state;

    DIAG_UTIL_PARAM_CHK();

    if(!osal_strcmp(TOKEN_STR(2),"broadcast"))
        stormType = STORM_GROUP_BROADCAST;
    else if(!osal_strcmp(TOKEN_STR(2),"multicast"))
        stormType = STORM_GROUP_MULTICAST;
    else if(!osal_strcmp(TOKEN_STR(2),"unknown-multicast"))
        stormType = STORM_GROUP_UNKNOWN_MULTICAST;
    else if(!osal_strcmp(TOKEN_STR(2),"unknown-unicast"))
        stormType = STORM_GROUP_UNKNOWN_UNICAST;
    else if(!osal_strcmp(TOKEN_STR(2),"dhcp-storm"))
        stormType = STORM_GROUP_DHCP;
    else if(!osal_strcmp(TOKEN_STR(2),"arp-storm"))
        stormType = STORM_GROUP_ARP;
    else if(!osal_strcmp(TOKEN_STR(2),"igmp-mld-storm"))
        stormType = STORM_GROUP_IGMP_MLD;
    else
        return CPARSER_ERR_INVALID_PARAMS;

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_stormControlMeterIdx_set(port, stormType, *index_ptr), ret);
    }


    return CPARSER_OK;
}    /* end of cparser_cmd_storm_control_set_broadcast_multicast_unknown_multicast_unknown_unicast_arp_storm_dhcp_storm_igmp_mld_storm_port_ports_all_meter_index */

/*
 * storm-control get ( broadcast | multicast | unknown-multicast | unknown-unicast ) alternated
 */
cparser_result_t
cparser_cmd_storm_control_get_broadcast_multicast_unknown_multicast_unknown_unicast_alternated(
    cparser_context_t *context)
{
    int32 ret;
    uint32 field;
    uint32 reg;
    uint32 type;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:

            reg = STORM_CTRL_ALT_TYPE_SELr;

            if(!osal_strcmp(TOKEN_STR(2),"broadcast"))
                field = BC_TYPEf;
            else if(!osal_strcmp(TOKEN_STR(2),"multicast"))
                field = MC_TYPEf;
            else if(!osal_strcmp(TOKEN_STR(2),"unknown-multicast"))
                field = UNMC_TYPEf;
            else if(!osal_strcmp(TOKEN_STR(2),"unknown-unicast"))
                field = UNDA_TYPEf;
            else
                return CPARSER_ERR_INVALID_PARAMS;
            
            DIAG_UTIL_ERR_CHK(reg_field_read(reg, field, &type), ret);

            diag_util_printf("Alternative Type: %s\n\n", diagStr_stormAltType[type]);

            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:

            reg = APOLLOMP_STORM_CTRL_ALT_TYPE_SELr;

            if(!osal_strcmp(TOKEN_STR(2),"broadcast"))
                field = APOLLOMP_BC_TYPEf;
            else if(!osal_strcmp(TOKEN_STR(2),"multicast"))
                field = APOLLOMP_MC_TYPEf;
            else if(!osal_strcmp(TOKEN_STR(2),"unknown-multicast"))
                field = APOLLOMP_UNMC_TYPEf;
            else if(!osal_strcmp(TOKEN_STR(2),"unknown-unicast"))
                field = APOLLOMP_UNDA_TYPEf;
            else
                return CPARSER_ERR_INVALID_PARAMS;

            DIAG_UTIL_ERR_CHK(reg_field_read(reg, field, &type), ret);

            diag_util_printf("Alternative Type: %s\n\n", diagStr_stormAltType[type]);

            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_storm_control_get_broadcast_multicast_unknown_multicast_unknown_unicast_alternated */

/*
 * storm-control set ( broadcast | multicast | unknown-multicast | unknown-unicast ) alternated ( disabled | arp-storm | dhcp-storm | igmp-mld-storm )
 */
cparser_result_t
cparser_cmd_storm_control_set_broadcast_multicast_unknown_multicast_unknown_unicast_alternated_disabled_arp_storm_dhcp_storm_igmp_mld_storm(
    cparser_context_t *context)
{
    int32 ret;
    uint32 field;
    uint32 reg;
    uint32 type;
    rtk_enable_t state;

    DIAG_UTIL_PARAM_CHK();

    if(!osal_strcmp(TOKEN_STR(4),"disabled"))
        type = 0;
    else if(!osal_strcmp(TOKEN_STR(4),"arp-storm"))
        type = 1;
    else if(!osal_strcmp(TOKEN_STR(4),"dhcp-storm"))
        type = 2;
    else if(!osal_strcmp(TOKEN_STR(4),"igmp-mld-storm"))
        type = 3;
    else
        return CPARSER_ERR_INVALID_PARAMS;


    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:

            reg = STORM_CTRL_ALT_TYPE_SELr;

            if(!osal_strcmp(TOKEN_STR(2),"broadcast"))
                field = BC_TYPEf;
            else if(!osal_strcmp(TOKEN_STR(2),"multicast"))
                field = MC_TYPEf;
            else if(!osal_strcmp(TOKEN_STR(2),"unknown-multicast"))
                field = UNMC_TYPEf;
            else if(!osal_strcmp(TOKEN_STR(2),"unknown-unicast"))
                field = UNDA_TYPEf;
            else
                return CPARSER_ERR_INVALID_PARAMS;

            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:

            reg = APOLLOMP_STORM_CTRL_ALT_TYPE_SELr;

            if(!osal_strcmp(TOKEN_STR(2),"broadcast"))
                field = APOLLOMP_BC_TYPEf;
            else if(!osal_strcmp(TOKEN_STR(2),"multicast"))
                field = APOLLOMP_MC_TYPEf;
            else if(!osal_strcmp(TOKEN_STR(2),"unknown-multicast"))
                field = APOLLOMP_UNMC_TYPEf;
            else if(!osal_strcmp(TOKEN_STR(2),"unknown-unicast"))
                field = APOLLOMP_UNDA_TYPEf;
            else
                return CPARSER_ERR_INVALID_PARAMS;

            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    DIAG_UTIL_ERR_CHK(reg_field_write(reg, field, &type), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_storm_control_set_broadcast_multicast_unknown_multicast_unknown_unicast_alternated_disabled_arp_storm_dhcp_storm_igmp_mld_storm */

/*
 * storm-control get bypass-packet ( igmp | cdp | csstp ) state
 */
cparser_result_t
cparser_cmd_storm_control_get_bypass_packet_igmp_cdp_csstp_state(
    cparser_context_t *context)
{
    int32 ret;
    rtk_enable_t state;
    rtk_storm_bypass_t type;


    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    if(!osal_strcmp(TOKEN_STR(3),"igmp"))
    {
        type = BYPASS_IGMP;
    }
    else if(!osal_strcmp(TOKEN_STR(3),"cdp"))
    {
        type = BYPASS_CDP;
    }
    else if(!osal_strcmp(TOKEN_STR(3),"csstp"))
    {
        type = BYPASS_CSSTP;
    }
    else
        return CPARSER_ERR_INVALID_PARAMS;

    DIAG_UTIL_ERR_CHK(rtk_rate_stormBypass_get(type, &state), ret);

    if(type == BYPASS_IGMP)
        diag_util_mprintf("IGMP ");
    else if(type == BYPASS_CDP)
        diag_util_mprintf("CDP ");
    else if(type == BYPASS_CSSTP)
        diag_util_mprintf("CSSTP ");

    diag_util_mprintf("bypass state: %s\n", diagStr_enable[state]);

    return CPARSER_OK;
}    /* end of cparser_cmd_storm_control_get_bypass_packet_igmp_cdp_csstp_state */

/*
 * storm-control set bypass-packet ( igmp | cdp | csstp ) state ( disable | enable )
 */
cparser_result_t
cparser_cmd_storm_control_set_bypass_packet_igmp_cdp_csstp_state_disable_enable(
    cparser_context_t *context)
{
    int32 ret;
    rtk_enable_t state;
    rtk_storm_bypass_t type;

    DIAG_UTIL_PARAM_CHK();

    if(!osal_strcmp(TOKEN_STR(3),"igmp"))
    {
        type = BYPASS_IGMP;
    }
    else if(!osal_strcmp(TOKEN_STR(3),"cdp"))
    {
        type = BYPASS_CDP;
    }
    else if(!osal_strcmp(TOKEN_STR(3),"csstp"))
    {
        type = BYPASS_CSSTP;
    }
    else
        return CPARSER_ERR_INVALID_PARAMS;

    if ('d' == TOKEN_CHAR(5, 0))
        state = DISABLED;
    else if ('e' == TOKEN_CHAR(5, 0))
        state = ENABLED;
    else
        return CPARSER_ERR_INVALID_PARAMS;

    DIAG_UTIL_ERR_CHK(rtk_rate_stormBypass_set(type, state), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_storm_control_set_bypass_packet_igmp_cdp_csstp_state_disable_enable */

/*
 * storm-control get bypass-packet rma <UINT:rma_tail> state
 */
cparser_result_t
cparser_cmd_storm_control_get_bypass_packet_rma_rma_tail_state(
    cparser_context_t *context,
    uint32_t  *rma_tail_ptr)
{
    int32 ret;
    rtk_enable_t state;
    rtk_storm_bypass_t type;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_PARAM_RANGE_CHK((*rma_tail_ptr > 0x2F), RT_ERR_OUT_OF_RANGE);

    type = BYPASS_BRG_GROUP + (*rma_tail_ptr);

    DIAG_UTIL_ERR_CHK(rtk_rate_stormBypass_get(type, &state), ret);
    diag_util_printf("RMA 01-80-C2-00-00-%2.2x bypass Storm-Control state: %s\n", *rma_tail_ptr, diagStr_enable[state]);

    return CPARSER_OK;
}    /* end of cparser_cmd_storm_control_get_bypass_packet_rma_rma_tail_state */

/*
 * storm-control set bypass-packet rma <UINT:rma_tail> state  ( disable | enable )
 */
cparser_result_t
cparser_cmd_storm_control_set_bypass_packet_rma_rma_tail_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *rma_tail_ptr)
{
    int32 ret;
    rtk_enable_t state;
    rtk_storm_bypass_t type;

    DIAG_UTIL_PARAM_CHK();

    if ('d' == TOKEN_CHAR(6, 0))
        state = DISABLED;
    else if ('e' == TOKEN_CHAR(6, 0))
        state = ENABLED;
    else
        return CPARSER_ERR_INVALID_PARAMS;

    DIAG_UTIL_PARAM_RANGE_CHK((*rma_tail_ptr > 0x2F), RT_ERR_OUT_OF_RANGE);


    type = BYPASS_BRG_GROUP + (*rma_tail_ptr);

    DIAG_UTIL_ERR_CHK(rtk_rate_stormBypass_set(type, state), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_storm_control_set_bypass_packet_rma_rma_tail_state_disable_enable */

