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
#include <rtk/rt/rt_rate.h>

/*
 * rt_rate init
 */
cparser_result_t
cparser_cmd_rt_rate_init(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rt_rate_init(), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_rate_init */

/*
 * rt_rate get ingress port ( <PORT_LIST:ports> | all ) rate
 */
cparser_result_t
cparser_cmd_rt_rate_get_ingress_port_ports_all_rate(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rt_port_t port;
    uint32 rate;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rt_rate_portIgrBandwidthCtrlRate_get(port,&rate), ret);

        diag_util_mprintf("port:%2d rate:%d\n",port , rate);
    }


    return CPARSER_OK;
}    /* end of cparser_cmd_rt_rate_get_ingress_port_ports_all_rate */

/*
 * rt_rate set ingress port ( <PORT_LIST:ports> | all ) rate <UINT:rate>
 */
cparser_result_t
cparser_cmd_rt_rate_set_ingress_port_ports_all_rate_rate(
    cparser_context_t *context,
    char * *ports_ptr,
    uint32_t  *rate_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rt_port_t port;
    uint32 rate;

    DIAG_UTIL_PARAM_CHK();

    rate = *rate_ptr;

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rt_rate_portIgrBandwidthCtrlRate_set(port,rate), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_rate_set_ingress_port_ports_all_rate_rate */

/*
 * rt_rate get egress port ( <PORT_LIST:ports> | all ) rate
 */
cparser_result_t
cparser_cmd_rt_rate_get_egress_port_ports_all_rate(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rt_port_t port;
    uint32 rate;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rt_rate_portEgrBandwidthCtrlRate_get(port,&rate), ret);

        diag_util_mprintf("port:%2d rate:%d\n",port , rate);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_rate_get_egress_port_ports_all_rate */

/*
 * rt_rate set egress port ( <PORT_LIST:ports> | all ) rate <UINT:rate>
 */
cparser_result_t
cparser_cmd_rt_rate_set_egress_port_ports_all_rate_rate(
    cparser_context_t *context,
    char * *ports_ptr,
    uint32_t  *rate_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rt_port_t port;
    uint32 rate;

    DIAG_UTIL_PARAM_CHK();

    rate = *rate_ptr;

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rt_rate_portEgrBandwidthCtrlRate_set(port,rate), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_rate_set_egress_port_ports_all_rate_rate */

/*
 * rt_rate get storm-control ( broadcast | multicast | unknown-multicast | unknown-unicast | arp-storm | dhcp-storm | igmp-mld-storm | ethertype-storm | loop-detect-storm | dot1x-storm ) port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_rt_rate_get_storm_control_broadcast_multicast_unknown_multicast_unknown_unicast_arp_storm_dhcp_storm_igmp_mld_storm_ethertype_storm_loop_detect_storm_dot1x_storm_port_ports_all(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    diag_portlist_t portlist;
    rt_port_t port;
    rt_rate_storm_group_t stormType;
    rt_enable_t state;
    uint32 meter;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    if(!osal_strcmp(TOKEN_STR(3),"broadcast"))
        stormType = RT_STORM_GROUP_BROADCAST;
    else if(!osal_strcmp(TOKEN_STR(3),"multicast"))
        stormType = RT_STORM_GROUP_MULTICAST;
    else if(!osal_strcmp(TOKEN_STR(3),"unknown-multicast"))
        stormType = RT_STORM_GROUP_UNKNOWN_MULTICAST;
    else if(!osal_strcmp(TOKEN_STR(3),"unknown-unicast"))
        stormType = RT_STORM_GROUP_UNKNOWN_UNICAST;
    else if(!osal_strcmp(TOKEN_STR(3),"dhcp-storm"))
        stormType = RT_STORM_GROUP_DHCP;
    else if(!osal_strcmp(TOKEN_STR(3),"arp-storm"))
        stormType = RT_STORM_GROUP_ARP;
    else if(!osal_strcmp(TOKEN_STR(3),"igmp-mld-storm"))
        stormType = RT_STORM_GROUP_IGMP_MLD;
    else if(!osal_strcmp(TOKEN_STR(3),"ethertype-storm"))
        stormType = RT_STORM_GROUP_ETHERTYPE;
    else if(!osal_strcmp(TOKEN_STR(3),"loop-detect-storm"))
        stormType = RT_STORM_GROUP_LOOP_DETECT;
    else if(!osal_strcmp(TOKEN_STR(3),"dot1x-storm"))
        stormType = RT_STORM_GROUP_DOT1X;
    else
        return CPARSER_ERR_INVALID_PARAMS;

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    diag_util_mprintf("Port State   Meter\n");

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rt_rate_stormControlPortEnable_get(port, stormType, &state), ret);
        DIAG_UTIL_ERR_CHK(rt_rate_stormControlMeterIdx_get(port, stormType, &meter), ret);

        diag_util_mprintf("%-4d %-7s %d\n", port, diagStr_enable[state], meter);
    }


    return CPARSER_OK;
}    /* end of cparser_cmd_rt_rate_get_storm_control_broadcast_multicast_unknown_multicast_unknown_unicast_arp_storm_dhcp_storm_igmp_mld_storm_ethertype_storm_loop_detect_storm_dot1x_storm_port_ports_all */

/*
 * rt_rate set storm-control ( broadcast | multicast | unknown-multicast | unknown-unicast | arp-storm | dhcp-storm | igmp-mld-storm | ethertype-storm  | loop-detect-storm | dot1x-storm ) port ( <PORT_LIST:ports> | all ) state ( disable | enable )
 */
cparser_result_t
cparser_cmd_rt_rate_set_storm_control_broadcast_multicast_unknown_multicast_unknown_unicast_arp_storm_dhcp_storm_igmp_mld_storm_ethertype_storm_loop_detect_storm_dot1x_storm_port_ports_all_state_disable_enable(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    diag_portlist_t portlist;
    rt_port_t port;
    rt_rate_storm_group_t  stormType;
    rt_enable_t state;

    DIAG_UTIL_PARAM_CHK();

    if(!osal_strcmp(TOKEN_STR(3),"broadcast"))
        stormType = RT_STORM_GROUP_BROADCAST;
    else if(!osal_strcmp(TOKEN_STR(3),"multicast"))
        stormType = RT_STORM_GROUP_MULTICAST;
    else if(!osal_strcmp(TOKEN_STR(3),"unknown-multicast"))
        stormType = RT_STORM_GROUP_UNKNOWN_MULTICAST;
    else if(!osal_strcmp(TOKEN_STR(3),"unknown-unicast"))
        stormType = RT_STORM_GROUP_UNKNOWN_UNICAST;
    else if(!osal_strcmp(TOKEN_STR(3),"dhcp-storm"))
        stormType = RT_STORM_GROUP_DHCP;
    else if(!osal_strcmp(TOKEN_STR(3),"arp-storm"))
        stormType = RT_STORM_GROUP_ARP;
    else if(!osal_strcmp(TOKEN_STR(3),"igmp-mld-storm"))
        stormType = RT_STORM_GROUP_IGMP_MLD;
    else if(!osal_strcmp(TOKEN_STR(3),"ethertype-storm"))
        stormType = RT_STORM_GROUP_ETHERTYPE;
    else if(!osal_strcmp(TOKEN_STR(3),"loop-detect-storm"))
        stormType = RT_STORM_GROUP_LOOP_DETECT;
    else if(!osal_strcmp(TOKEN_STR(3),"dot1x-storm"))
        stormType = RT_STORM_GROUP_DOT1X;
    else
        return CPARSER_ERR_INVALID_PARAMS;

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    if ('d' == TOKEN_CHAR(7, 0))
        state = DISABLED;
    else if ('e' == TOKEN_CHAR(7, 0))
        state = ENABLED;
    else
        return CPARSER_ERR_INVALID_PARAMS;


    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rt_rate_stormControlPortEnable_set(port, stormType, state), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_rate_set_storm_control_broadcast_multicast_unknown_multicast_unknown_unicast_arp_storm_dhcp_storm_igmp_mld_storm_ethertype_storm_loop_detect_storm_dot1x_storm_port_ports_all_state_disable_enable */

/*
 * rt_rate set storm-control ( broadcast | multicast | unknown-multicast | unknown-unicast | arp-storm | dhcp-storm | igmp-mld-storm | ethertype-storm  | loop-detect-storm | dot1x-storm ) port ( <PORT_LIST:ports> | all ) meter <UINT:index>
 */
cparser_result_t
cparser_cmd_rt_rate_set_storm_control_broadcast_multicast_unknown_multicast_unknown_unicast_arp_storm_dhcp_storm_igmp_mld_storm_ethertype_storm_loop_detect_storm_dot1x_storm_port_ports_all_meter_index(
    cparser_context_t *context,
    char * *ports_ptr,
    uint32_t  *index_ptr)
{
    int32 ret;
    diag_portlist_t portlist;
    rt_port_t port;
    rt_rate_storm_group_t  stormType;
    rt_enable_t state;

    DIAG_UTIL_PARAM_CHK();

    if(!osal_strcmp(TOKEN_STR(3),"broadcast"))
        stormType = RT_STORM_GROUP_BROADCAST;
    else if(!osal_strcmp(TOKEN_STR(3),"multicast"))
        stormType = RT_STORM_GROUP_MULTICAST;
    else if(!osal_strcmp(TOKEN_STR(3),"unknown-multicast"))
        stormType = RT_STORM_GROUP_UNKNOWN_MULTICAST;
    else if(!osal_strcmp(TOKEN_STR(3),"unknown-unicast"))
        stormType = RT_STORM_GROUP_UNKNOWN_UNICAST;
    else if(!osal_strcmp(TOKEN_STR(3),"dhcp-storm"))
        stormType = RT_STORM_GROUP_DHCP;
    else if(!osal_strcmp(TOKEN_STR(3),"arp-storm"))
        stormType = RT_STORM_GROUP_ARP;
    else if(!osal_strcmp(TOKEN_STR(3),"igmp-mld-storm"))
        stormType = RT_STORM_GROUP_IGMP_MLD;
    else if(!osal_strcmp(TOKEN_STR(3),"ethertype-storm"))
        stormType = RT_STORM_GROUP_ETHERTYPE;
    else if(!osal_strcmp(TOKEN_STR(3),"loop-detect-storm"))
        stormType = RT_STORM_GROUP_LOOP_DETECT;
    else if(!osal_strcmp(TOKEN_STR(3),"dot1x-storm"))
        stormType = RT_STORM_GROUP_DOT1X;
    else
        return CPARSER_ERR_INVALID_PARAMS;

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rt_rate_stormControlMeterIdx_set(port, stormType, *index_ptr), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_rate_set_storm_control_broadcast_multicast_unknown_multicast_unknown_unicast_arp_storm_dhcp_storm_igmp_mld_storm_ethertype_storm_loop_detect_storm_dot1x_storm_port_ports_all_meter_index */

/*
 * rt_rate add meter type ( storm | host | flow | acl | sw )
 */
cparser_result_t
cparser_cmd_rt_rate_add_meter_type_storm_host_flow_acl_sw(
    cparser_context_t *context)
{
    int32 ret;
    rt_rate_meter_type_t type;
    uint32 index = 0;

    DIAG_UTIL_PARAM_CHK();

    if(!osal_strcmp(TOKEN_STR(4),"storm"))
        type = RT_METER_TYPE_STORM;
    else if(!osal_strcmp(TOKEN_STR(4),"host"))
        type = RT_METER_TYPE_HOST;
    else if(!osal_strcmp(TOKEN_STR(4),"flow"))
        type = RT_METER_TYPE_FLOW;
    else if(!osal_strcmp(TOKEN_STR(4),"acl"))
        type = RT_METER_TYPE_ACL;
    else if(!osal_strcmp(TOKEN_STR(4),"sw"))
        type = RT_METER_TYPE_SW;
    else
        return CPARSER_ERR_INVALID_PARAMS;

    DIAG_UTIL_ERR_CHK(rt_rate_shareMeterType_add(type,&index),ret);

    diag_util_mprintf("Create meter type %s index=%d\n",TOKEN_STR(4),index);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_rate_add_meter_type_storm_host_flow */

/*
 * rt_rate del meter <UINT:index>
 */
cparser_result_t
cparser_cmd_rt_rate_del_meter_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rt_rate_shareMeterType_del(*index_ptr),ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_rate_del_meter_index */

/*
 * rt_rate get meter <UINT:index>
 */
cparser_result_t
cparser_cmd_rt_rate_get_meter_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32 ret;
    rt_rate_meter_type_t type;
    uint32 rate;
    uint32 bucketSize;
    rt_enable_t ifgInclude;
    rt_rate_meter_mode_t mode;
    char tmp[20];

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rt_rate_shareMeterType_get(*index_ptr,&type),ret);

    if(type == RT_METER_TYPE_STORM)
        sprintf(tmp,"storm");
    else if(type == RT_METER_TYPE_HOST)
        sprintf(tmp,"host");
    else if(type == RT_METER_TYPE_FLOW)
        sprintf(tmp,"flow");
    else if(type == RT_METER_TYPE_ACL)
        sprintf(tmp,"acl");
    else if(type == RT_METER_TYPE_SW)
        sprintf(tmp,"sw");
    else
        return CPARSER_ERR_NOT_EXIST;

    DIAG_UTIL_ERR_CHK(rt_rate_shareMeterRate_get(*index_ptr,&rate),ret);

    if(rt_rate_shareMeterMode_get(*index_ptr,&mode) != RT_ERR_OK)
        mode = RT_METER_MODE_BIT_RATE;

    diag_util_mprintf("Meter index=%d type=%s rate=%d %s\n",*index_ptr,tmp,rate,(mode == RT_METER_MODE_PACKET_RATE)?"pps":"kbps");

    DIAG_UTIL_ERR_CHK(rt_rate_shareMeterBucket_get(*index_ptr,&bucketSize),ret);

    diag_util_mprintf("Burst size=%d\n",bucketSize);

    DIAG_UTIL_ERR_CHK(rt_rate_shareMeterIfgInclude_get(*index_ptr,&ifgInclude),ret);

    diag_util_mprintf("IFG include=%s\n",(ifgInclude == ENABLED)?"INCLUDE":"EXCLUDE");

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_rate_get_meter_index */

/*
 * rt_rate set meter <UINT:index> rate <UINT:rate>
 */
cparser_result_t
cparser_cmd_rt_rate_set_meter_index_rate_rate(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *rate_ptr)
{
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rt_rate_shareMeterRate_set(*index_ptr,*rate_ptr),ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_rate_set_meter_index_rate_rate */

/*
 * rt_rate set meter <UINT:index> burst-size <UINT:size> */
cparser_result_t
cparser_cmd_rt_rate_set_meter_index_burst_size_size(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *size_ptr)
{
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rt_rate_shareMeterBucket_set(*index_ptr,*size_ptr),ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_rate_set_meter_index_burst_size_size */

/*
 * rt_rate set meter <UINT:index> ifg ( exclude | include ) */
cparser_result_t
cparser_cmd_rt_rate_set_meter_index_ifg_exclude_include(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    int32 ret;
    rtk_enable_t ifgInclude;

    DIAG_UTIL_PARAM_CHK();


    if ('e' == TOKEN_CHAR(5,0))
        ifgInclude = DISABLED;
    else
        ifgInclude = ENABLED;

    DIAG_UTIL_ERR_CHK(rt_rate_shareMeterIfgInclude_set(*index_ptr,ifgInclude),ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_rate_set_meter_index_ifg_exclude_include */

/*
 * rt_rate set meter <UINT:index> mode ( bps | pps ) */
cparser_result_t
cparser_cmd_rt_rate_set_meter_index_mode_bps_pps(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    int32 ret;
    rt_rate_meter_mode_t mode;

    DIAG_UTIL_PARAM_CHK();

    if ('p' == TOKEN_CHAR(5,0))
        mode = RT_METER_MODE_PACKET_RATE;
    else
        mode = RT_METER_MODE_BIT_RATE;

    DIAG_UTIL_ERR_CHK(rt_rate_shareMeterMode_set(*index_ptr,mode),ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_rate_set_meter_index_mode_bps_pps */
