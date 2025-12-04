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
 * Purpose : Definition those Trunk command and APIs in the SDK diagnostic shell.
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
#include <rtk/trunk.h>
//#include <dal/apollo/raw/apollo_raw_trunk.h>
#include <dal/apollomp/raw/apollomp_raw_trunk.h>
/*
 * trunk init
 */
cparser_result_t
cparser_cmd_trunk_init(
    cparser_context_t *context)
{
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_trunk_init(), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_trunk_init */

/*
 * trunk dump
 */
cparser_result_t
cparser_cmd_trunk_dump(
    cparser_context_t *context)
{
    int32 ret;
    uint32 hashsel = 0;
    uint32 algorithm;
    rtk_enable_t state;
    rtk_portmask_t empty_portmask;
    rtk_portmask_t portmask;
    uint8 portStr[20];
    rtk_trunk_hashVal2Port_t hash_map;
    uint32 hash_value;
    rtk_trunk_mode_t mode;
    rtk_trunk_separateType_t sep_mode;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_trunk_port_get(0, &portmask), ret);
    DIAG_UTIL_ERR_CHK(diag_util_lPortMask2str(portStr, &portmask), ret);
    diag_util_printf("\n Trunk portmask: %s", portStr);

    DIAG_UTIL_ERR_CHK(rtk_trunk_distributionAlgorithm_get(0, &hashsel), ret);
    if(0 == hashsel)
    {
        diag_util_printf("\n Distribute Algorithm: None");
    }
    else
    {
        diag_util_printf("\n Distribute Algorithm:\n");
        for(algorithm = 0; algorithm < APOLLOMP_RAW_TRUNK_ALGORITHM_NUM; algorithm++)
        {
            if(hashsel & (0x01 << algorithm))
                diag_util_printf("  %s\n", diagStr_trunkAlgorithm[algorithm]);
        }
    }

    DIAG_UTIL_ERR_CHK(rtk_trunk_mode_get(&mode), ret);
    diag_util_printf("\n Trunk Mode: %s", diagStr_trunkMode[mode]);

    DIAG_UTIL_ERR_CHK(rtk_trunk_hashMappingTable_get(0, &hash_map), ret);
    for(hash_value = 0; hash_value < RTK_MAX_NUM_OF_TRUNK_HASH_VAL; hash_value++)
    {
        diag_util_printf("\n Hash Value %d to Port %d in trunk group", hash_value, hash_map.value[hash_value]);
    }


    DIAG_UTIL_ERR_CHK(rtk_trunk_trafficPause_get(0, &state), ret);
    diag_util_mprintf("\n Trunk Flow Contrl: %s", diagStr_enable[state]);

    DIAG_UTIL_ERR_CHK(rtk_trunk_portQueueEmpty_get(&empty_portmask), ret);
    DIAG_UTIL_ERR_CHK(diag_util_lPortMask2str(portStr, &empty_portmask), ret);
    diag_util_printf("\n Queue Empty Port Mask: %s", portStr);

    DIAG_UTIL_ERR_CHK(rtk_trunk_trafficSeparate_get(0, &sep_mode), ret);
    diag_util_printf("\n Flood Mode: %s\n\n", diagStr_trunkFloodMode[sep_mode]);

    return CPARSER_OK;
}    /* end of cparser_cmd_trunk_dump */

/*
 * trunk get distribute-algorithm
 */
cparser_result_t
cparser_cmd_trunk_get_distribute_algorithm(
    cparser_context_t *context)
{
    int32 ret;
    uint32 hashsel = 0;
    uint32 algorithm;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_trunk_distributionAlgorithm_get(0, &hashsel), ret);
    if(0 == hashsel)
    {
        diag_util_printf("\n Distribute Algorithm: None\n");
    }
    else
    {
        diag_util_printf("\n Distribute Algorithm:\n");
        for(algorithm = 0; algorithm < APOLLOMP_RAW_TRUNK_ALGORITHM_NUM; algorithm++)
        {
            if(hashsel & (0x01 << algorithm))
                diag_util_printf("  %s\n", diagStr_trunkAlgorithm[algorithm]);
        }
    }

    diag_util_mprintf("");

    return CPARSER_OK;
}    /* end of cparser_cmd_trunk_get_distribute_algorithm */

/*
 * trunk set distribute-algorithm { dst-ip } { dst-l4-port } { dst-mac } { src-ip } { src-l4-port } { src-mac } { src-port }
 */
cparser_result_t
cparser_cmd_trunk_set_distribute_algorithm_dst_ip_dst_l4_port_dst_mac_src_ip_src_l4_port_src_mac_src_port(
    cparser_context_t *context)
{
    int32 ret;
    uint32 token;
    uint32 hashsel = 0;

    DIAG_UTIL_PARAM_CHK();

    for(token = 3; token < TOKEN_NUM(); token++)
    {
        if ('d' == TOKEN_CHAR(token, 0))
        {
            if ('i' == TOKEN_CHAR(token, 4))
                hashsel |= TRUNK_DISTRIBUTION_ALGO_DIP_BIT;
            else if ('l' == TOKEN_CHAR(token, 4))
                hashsel |= TRUNK_DISTRIBUTION_ALGO_DST_L4PORT_BIT;
            else if ('m' == TOKEN_CHAR(token, 4))
                hashsel |= TRUNK_DISTRIBUTION_ALGO_DMAC_BIT;
            else
                return CPARSER_ERR_INVALID_PARAMS;
        }
        else if ('s' == TOKEN_CHAR(token, 0))
        {
            if ('i' == TOKEN_CHAR(token, 4))
                hashsel |= TRUNK_DISTRIBUTION_ALGO_SIP_BIT;
            else if ('l' == TOKEN_CHAR(token, 4))
                hashsel |= TRUNK_DISTRIBUTION_ALGO_SRC_L4PORT_BIT;
            else if ('m' == TOKEN_CHAR(token, 4))
                hashsel |= TRUNK_DISTRIBUTION_ALGO_SMAC_BIT;
            else if ('p' == TOKEN_CHAR(token, 4))
                hashsel |= TRUNK_DISTRIBUTION_ALGO_SPA_BIT;
            else
                return CPARSER_ERR_INVALID_PARAMS;
        }
        else
            return CPARSER_ERR_INVALID_PARAMS;
    }

    DIAG_UTIL_ERR_CHK(rtk_trunk_distributionAlgorithm_set(0, hashsel), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_trunk_set_distribute_algorithm_dst_ip_dst_l4_port_dst_mac_src_ip_src_l4_port_src_mac_src_port */

/*
 * trunk get queue-empty
 */
cparser_result_t
cparser_cmd_trunk_get_queue_empty(
    cparser_context_t *context)
{
    int32 ret;
    rtk_portmask_t empty_portmask;
    uint8 portStr[20];

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("");

    DIAG_UTIL_ERR_CHK(rtk_trunk_portQueueEmpty_get(&empty_portmask), ret);
    DIAG_UTIL_ERR_CHK(diag_util_lPortMask2str(portStr, &empty_portmask), ret);

    diag_util_printf("\n Queue Empty Port Mask: %s\n\n", portStr);

    return CPARSER_OK;
}    /* end of cparser_cmd_trunk_get_queue_empty */

/*
 * trunk get flow-control state
 */
cparser_result_t
cparser_cmd_trunk_get_flow_control_state(
    cparser_context_t *context)
{
    int32 ret;
    rtk_enable_t state;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_trunk_trafficPause_get(0, &state), ret);
    diag_util_mprintf("\nTrunk Flow Contrl: %s\n\n", diagStr_enable[state]);

    return CPARSER_OK;
}    /* end of cparser_cmd_trunk_get_flow_control_state */

/*
 * trunk set flow-control state ( disable | enable )
 */
cparser_result_t
cparser_cmd_trunk_set_flow_control_state_disable_enable(
    cparser_context_t *context)
{
    int32 ret;
    rtk_enable_t state;

    DIAG_UTIL_PARAM_CHK();

    if ('e' == TOKEN_CHAR(4, 0))
        state = ENABLED;
    else if ('d' == TOKEN_CHAR(4, 0))
        state = DISABLED;
    else
        return CPARSER_ERR_INVALID_PARAMS;

    DIAG_UTIL_ERR_CHK(rtk_trunk_trafficPause_set(0, state), ret);
    return CPARSER_OK;
}    /* end of cparser_cmd_trunk_set_flow_control_state_disable_enable */

/*
 * trunk get hash-mapping hash-value all
 */
cparser_result_t
cparser_cmd_trunk_get_hash_mapping_hash_value_all(
    cparser_context_t *context)
{
    int32 ret;
    rtk_trunk_hashVal2Port_t hash_map;
    uint32 hash_value;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_trunk_hashMappingTable_get(0, &hash_map), ret);
    for(hash_value = 0; hash_value < RTK_MAX_NUM_OF_TRUNK_HASH_VAL; hash_value++)
    {
        diag_util_printf("\n Hash Value %d to Port %d in trunk group", hash_value, hash_map.value[hash_value]);
    }

    diag_util_printf("\n\n");
    return CPARSER_OK;
}    /* end of cparser_cmd_trunk_get_hash_mapping_hash_value_all */

/*
 * trunk set hash-mapping hash-value <UINT:value> port <UINT:port>
 */
cparser_result_t
cparser_cmd_trunk_set_hash_mapping_hash_value_value_port_port(
    cparser_context_t *context,
    uint32_t  *value_ptr,
    uint32_t  *port_ptr)
{
    int32 ret = RT_ERR_FAILED;
    rtk_trunk_hashVal2Port_t hash_map;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_PARAM_RANGE_CHK( (*value_ptr > RTK_MAX_NUM_OF_TRUNK_HASH_VAL), ret);
    DIAG_UTIL_PARAM_RANGE_CHK( (*port_ptr >= APOLLOMP_RAW_TRUNK_PORT_END), ret);

    DIAG_UTIL_ERR_CHK(rtk_trunk_hashMappingTable_get(0, &hash_map), ret);
    hash_map.value[*value_ptr] = (uint8)(*port_ptr);
    DIAG_UTIL_ERR_CHK(rtk_trunk_hashMappingTable_set(0, &hash_map), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_trunk_set_hash_mapping_hash_value_value_port_port */

/*
 * trunk set member-port ( <PORT_LIST:ports> | none )
 */
cparser_result_t
cparser_cmd_trunk_set_member_port_ports_none(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    DIAG_UTIL_ERR_CHK(rtk_trunk_port_set(0, &(portlist.portmask)), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_trunk_set_member_port_ports_none */

/*
 * trunk get member-port
 */
cparser_result_t
cparser_cmd_trunk_get_member_port(
    cparser_context_t *context)
{
    int32 ret;
    rtk_portmask_t portmask;
    uint8 portStr[20];

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_trunk_port_get(0, &portmask), ret);
    DIAG_UTIL_ERR_CHK(diag_util_lPortMask2str(portStr, &portmask), ret);

    diag_util_printf("\n Trunk portmask: %s\n\n", portStr);

    return CPARSER_OK;
}    /* end of cparser_cmd_trunk_get_member_port */

/*
 * trunk set mode ( dumb | normal )
 */
cparser_result_t
cparser_cmd_trunk_set_mode_dumb_normal(
    cparser_context_t *context)
{
    int32 ret;
    rtk_trunk_mode_t mode;

    DIAG_UTIL_PARAM_CHK();

    if ('d' == TOKEN_CHAR(3,0))
        mode = TRUNK_MODE_DUMB;
    else if ('n' == TOKEN_CHAR(3,0))
        mode = TRUNK_MODE_NORMAL;
    else
        return CPARSER_ERR_INVALID_PARAMS;

    DIAG_UTIL_ERR_CHK(rtk_trunk_mode_set(mode), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_trunk_set_mode_dumb_normal */

/*
 * trunk get mode
 */
cparser_result_t
cparser_cmd_trunk_get_mode(
    cparser_context_t *context)
{
    int32 ret;
    rtk_trunk_mode_t mode;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_trunk_mode_get(&mode), ret);

    diag_util_printf("Trunk Mode: %s\n\n", diagStr_trunkMode[mode]);
    return CPARSER_OK;
}    /* end of cparser_cmd_trunk_get_mode */

/*
 * trunk get traffic-separation flooding state
 */
cparser_result_t
cparser_cmd_trunk_get_traffic_separation_flooding_state(
    cparser_context_t *context)
{
    int32 ret;
    rtk_trunk_separateType_t mode;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_trunk_trafficSeparate_get(0, &mode), ret);
    diag_util_printf("\n Flood Mode: %s\n\n", diagStr_trunkFloodMode[mode]);

    return CPARSER_OK;
}    /* end of cparser_cmd_trunk_get_traffic_separation_flooding_state */

/*
 * trunk set traffic-separation flooding state ( disable | enable ) */
cparser_result_t
cparser_cmd_trunk_set_traffic_separation_flooding_state_disable_enable(
    cparser_context_t *context)
{
    int32 ret;
    rtk_trunk_separateType_t mode;

    DIAG_UTIL_PARAM_CHK();

    if ('e' == TOKEN_CHAR(5, 0))
        mode = SEPARATE_KNOWN_MULTI_AND_FLOOD;
    else if ('d' == TOKEN_CHAR(5, 0))
        mode = SEPARATE_NONE;
    else
        return CPARSER_ERR_INVALID_PARAMS;

    DIAG_UTIL_ERR_CHK(rtk_trunk_trafficSeparate_set(0, mode), ret);
    return CPARSER_OK;
}    /* end of cparser_cmd_trunk_set_traffic_separation_flooding_state_disable_enable */
