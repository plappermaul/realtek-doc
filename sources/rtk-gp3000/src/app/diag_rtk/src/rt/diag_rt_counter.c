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
 * Purpose : Definition those Counter command and APIs in the SDK diagnostic shell.
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
#include <rtk/rt/rt_stat.h>

void _dump_vlan_counter_filter(uint32_t index, rt_stat_vlanCnt_filter_t filter)
{

    diag_util_printf("%-14s: %d\n","Index",index);
    diag_util_printf("%-14s: %d\n","Enable",filter.enable);
    diag_util_printf("%-14s: %s\n","Direction",filter.direction == RT_STAT_VLAN_CNT_FILTER_DIRECTION_RX ? "RX":"TX");
    diag_util_printf("%-14s: %s\n","Type",filter.type == RT_STAT_VLAN_CNT_FILTER_TYPE_FLOW ? "Flow ID":"Port ID");
    diag_util_printf("%-14s: %d\n","Port/Flow ID",filter.portFlowId);
    if(filter.vid == RT_STAT_VLAN_CNT_FILTER_VID_INVAILD)
        diag_util_printf("%-14s: %s\n","Vid","Disable");
    else
        diag_util_printf("%-14s: %d\n","Vid",filter.vid);

    if(filter.pri == RT_STAT_VLAN_CNT_FILTER_PRI_INVAILD)
        diag_util_printf("%-14s: %s\n","Pri","Disable");
    else
        diag_util_printf("%-14s: %d\n","Pri",filter.pri);

    if(filter.pktType == RT_STAT_VLAN_CNT_FILTER_PKT_TYPE_ALL)
        diag_util_printf("%-14s: %s\n","Packet Type","All");
    else if(filter.pktType == RT_STAT_VLAN_CNT_FILTER_PKT_TYPE_UC)
        diag_util_printf("%-14s: %s\n","Packet Type","Unicast");
    else if(filter.pktType == RT_STAT_VLAN_CNT_FILTER_PKT_TYPE_MC)
        diag_util_printf("%-14s: %s\n","Packet Type","Multicast");
    else
        diag_util_printf("%-14s: %s\n","Packet Type","Broadcast");

    return;
}

/*
 * rt_mib init
 */
cparser_result_t
cparser_cmd_rt_mib_init(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rt_stat_init(), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_mib_init */

/*
 * rt_mib reset counter port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_rt_mib_reset_counter_port_ports_all(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rt_port_t port;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rt_stat_port_reset(port), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_mib_reset_counter_port_ports_all */

int32
dump_all_counter(rt_port_t port,rt_enable_t nonZero)
{
    int32 ret = RT_ERR_FAILED;
    rt_stat_port_cntr_t portCntrs;
    rt_stat_port_type_t type;
    uint64 cntr;

    if(nonZero == DISABLED)
    {
        for (type = 0; type < RT_MIB_PORT_CNTR_END; type++)
        {
            cntr = 0;
            if ((ret = rt_stat_port_get(port,type,&cntr)) != RT_ERR_OK)
            {
                continue;
            }

            diag_util_printf("%-35s: ",diagStr_mibName[type]);
            diag_util_printf("%25llu\n", cntr);

        }
    }
    else
    {
        for (type = 0; type < RT_MIB_PORT_CNTR_END; type++)
        {
            cntr = 0;
            if ((ret = rt_stat_port_get(port,type,&cntr)) != RT_ERR_OK)
            {
                continue;
            }
            if(cntr == 0)
                continue;

            diag_util_printf("%-35s: ",diagStr_mibName[type]);
            diag_util_printf("%25llu\n", cntr);
        }
    }
    return RT_ERR_OK;
}

/*
 * rt_mib dump counter port ( <PORT_LIST:ports> | all ) { nonZero } */
cparser_result_t
cparser_cmd_rt_mib_dump_counter_port_ports_all_nonZero(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rt_port_t port;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        diag_util_mprintf("Port: %d\n", port);
        if(5 == TOKEN_NUM())
        {
            dump_all_counter(port,DISABLED);
        }
        else
        {
            dump_all_counter(port,ENABLED);
        }
        diag_util_mprintf("\n");
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_mib_dump_counter_port_ports_all_nonzero */


/*
 * rt_mib set vlan-counter filter index <UINT:index> state ( disable | enable ) type ( port | flow ) id <UINT:id> dir ( rx | tx )
 */
cparser_result_t
cparser_cmd_rt_mib_set_vlan_counter_filter_index_index_state_disable_enable_type_port_flow_id_id_dir_rx_tx(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *id_ptr)
{
    rt_stat_vlanCnt_filter_t filter;
    int32 ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();

    memset(&filter, 0, sizeof(rt_stat_vlanCnt_filter_t));

    filter.vid = RT_STAT_VLAN_CNT_FILTER_VID_INVAILD;
    filter.pri = RT_STAT_VLAN_CNT_FILTER_PRI_INVAILD;
    filter.pktType = RT_STAT_VLAN_CNT_FILTER_PKT_TYPE_ALL;


    if('e' == TOKEN_CHAR(7,0))
        filter.enable = ENABLED;
    else
        filter.enable = DISABLED;

    if('p' == TOKEN_CHAR(9,0))
        filter.type = RT_STAT_VLAN_CNT_FILTER_TYPE_PORT;
    else
        filter.type = RT_STAT_VLAN_CNT_FILTER_TYPE_FLOW;

    filter.portFlowId = *id_ptr;

    if('r' == TOKEN_CHAR(13,0))
        filter.direction = RT_STAT_VLAN_CNT_FILTER_DIRECTION_RX;
    else
        filter.direction = RT_STAT_VLAN_CNT_FILTER_DIRECTION_TX;

    DIAG_UTIL_ERR_CHK(rt_stat_vlanCnt_filter_set(*index_ptr, filter), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_mib_set_vlan_counter_filter_index_index_state_disable_enable_type_port_flow_id_id_dir_rx_tx */

/*
 * rt_mib set vlan-counter filter index <UINT:index> state ( disable | enable ) type ( port | flow ) id <UINT:id> dir ( rx | tx ) vid <UINT:vid> pri <UINT:pri> pkt-type ( all | uc | mc | bc )
 */
cparser_result_t
cparser_cmd_rt_mib_set_vlan_counter_filter_index_index_state_disable_enable_type_port_flow_id_id_dir_rx_tx_vid_vid_pri_pri_pkt_type_all_uc_mc_bc(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *id_ptr,
    uint32_t  *vid_ptr,
    uint32_t  *pri_ptr)
{
    rt_stat_vlanCnt_filter_t filter;
    int32 ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();

    memset(&filter, 0, sizeof(rt_stat_vlanCnt_filter_t));

    filter.vid = *vid_ptr;
    filter.pri = *pri_ptr;


    if('e' == TOKEN_CHAR(7,0))
        filter.enable = ENABLED;
    else
        filter.enable = DISABLED;

    if('p' == TOKEN_CHAR(9,0))
        filter.type = RT_STAT_VLAN_CNT_FILTER_TYPE_PORT;
    else
        filter.type = RT_STAT_VLAN_CNT_FILTER_TYPE_FLOW;

    filter.portFlowId = *id_ptr;

    if('r' == TOKEN_CHAR(13,0))
        filter.direction = RT_STAT_VLAN_CNT_FILTER_DIRECTION_RX;
    else
        filter.direction = RT_STAT_VLAN_CNT_FILTER_DIRECTION_TX;

    if('a' == TOKEN_CHAR(19,0))
        filter.pktType = RT_STAT_VLAN_CNT_FILTER_PKT_TYPE_ALL;
    else if('u' == TOKEN_CHAR(19,0))
        filter.pktType = RT_STAT_VLAN_CNT_FILTER_PKT_TYPE_UC;
    else if('m' == TOKEN_CHAR(19,0))
        filter.pktType = RT_STAT_VLAN_CNT_FILTER_PKT_TYPE_MC;
    else
        filter.pktType = RT_STAT_VLAN_CNT_FILTER_PKT_TYPE_BC;

    DIAG_UTIL_ERR_CHK(rt_stat_vlanCnt_filter_set(*index_ptr, filter), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_mib_set_vlan_counter_filter_index_index_state_disable_enable_type_port_flow_id_id_vid_vid_pri_pri_pkt_type_all_uc_mc_bc */


/*
 * rt_mib get vlan-counter filter index <UINT:index>
 */
cparser_result_t
cparser_cmd_rt_mib_get_vlan_counter_filter_index_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    rt_stat_vlanCnt_filter_t filter;
    int32 ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rt_stat_vlanCnt_filter_get(*index_ptr, &filter), ret);

    _dump_vlan_counter_filter(*index_ptr, filter);

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_mib_get_vlan_counter_filter_index_index */

/*
 * rt_mib get vlan-counter filter index valid
 */
cparser_result_t
cparser_cmd_rt_mib_get_vlan_counter_filter_index_valid(
    cparser_context_t *context)
{
    rt_stat_vlanCnt_filter_t filter;
    uint32_t index = 0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();


    while(RT_ERR_OK == rt_stat_vlanCnt_filter_get(index, &filter))
    {
        if(filter.enable)
        {
            diag_util_printf("********************************\n");
            _dump_vlan_counter_filter(index, filter);
        }
        index++;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_mib_get_vlan_counter_filter_index_valid */


/*
 * rt_mib dump vlan-counter index <PORT_LIST:index> { nonZero } */
cparser_result_t
cparser_cmd_rt_mib_dump_vlan_counter_index_index_nonZero(
    cparser_context_t *context,
    char * *index_ptr)
{
    int32 ret = RT_ERR_FAILED;
    int32 nonZero = 0;
    rt_stat_port_cntr_t mib;
    diag_portlist_t portlist;
    rt_port_t port;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    memset(&portlist, 0, sizeof(diag_portlist_t));

    if(6 == TOKEN_NUM())
        nonZero = 1;

    if ((ret = diag_util_str2LPortMask(context->parser->tokens[4].buf, &(portlist.portmask))) != RT_ERR_OK)
    {
        diag_util_printf("port list ERROR!\n");
        RT_ERR(ret, (MOD_DIAGSHELL), "port list=%s", context->parser->tokens[4].buf);
        return ret;
    }

    for (port = 0; port < 32; port++)
    {
        if (RTK_PORTMASK_IS_PORT_SET((portlist).portmask, (port)))
        {
            memset(&mib,0,sizeof(rt_stat_port_cntr_t));

            DIAG_UTIL_ERR_CHK(rt_stat_vlanCnt_mib_get(port, &mib), ret);
            diag_util_printf("Index: %d\n",port);
            if(!nonZero || mib.ifInOctets != 0)
                diag_util_printf("%-35s: %22llu\n","ifInOctets",mib.ifInOctets);
            if(!nonZero || mib.ifInUcastPkts != 0)
                diag_util_printf("%-35s: %22u\n","ifInUcastPkts",mib.ifInUcastPkts);
            if(!nonZero || mib.ifInMulticastPkts != 0)
                diag_util_printf("%-35s: %22u\n","ifInMulticastPkts",mib.ifInMulticastPkts);
            if(!nonZero || mib.ifInBroadcastPkts != 0)
                diag_util_printf("%-35s: %22u\n","ifInBroadcastPkts",mib.ifInBroadcastPkts);
            if(!nonZero || mib.ifOutOctets != 0)
                diag_util_printf("%-35s: %22llu\n","ifOutOctets",mib.ifOutOctets);
            if(!nonZero || mib.ifOutUcastPkts != 0)
                diag_util_printf("%-35s: %22u\n","ifOutUcastPkts",mib.ifOutUcastPkts);
            if(!nonZero || mib.ifOutMulticastPkts != 0)
                diag_util_printf("%-35s: %22u\n","ifOutMulticastPkts",mib.ifOutMulticastPkts);
            if(!nonZero || mib.ifOutBrocastPkts != 0)
                diag_util_printf("%-35s: %22u\n","ifOutBrocastPkts",mib.ifOutBrocastPkts);
            if(!nonZero || mib.etherStatsRxPkts64Octets != 0)
                diag_util_printf("%-35s: %22u\n","etherStatsRxPkts64Octets",mib.etherStatsRxPkts64Octets);
            if(!nonZero || mib.etherStatsRxPkts65to127Octets != 0)
                diag_util_printf("%-35s: %22u\n","etherStatsRxPkts65to127Octets",mib.etherStatsRxPkts65to127Octets);
            if(!nonZero || mib.etherStatsRxPkts128to255Octets != 0)
                diag_util_printf("%-35s: %22u\n","etherStatsRxPkts128to255Octets",mib.etherStatsRxPkts128to255Octets);
            if(!nonZero || mib.etherStatsRxPkts256to511Octets != 0)
                diag_util_printf("%-35s: %22u\n","etherStatsRxPkts256to511Octets",mib.etherStatsRxPkts256to511Octets);
            if(!nonZero || mib.etherStatsRxPkts512to1023Octets != 0)
                diag_util_printf("%-35s: %22u\n","etherStatsRxPkts512to1023Octets",mib.etherStatsRxPkts512to1023Octets);
            if(!nonZero || mib.etherStatsRxPkts1024to1518Octets != 0)
                diag_util_printf("%-35s: %22u\n","etherStatsRxPkts1024to1518Octets",mib.etherStatsRxPkts1024to1518Octets);
            if(!nonZero || mib.etherStatsRxOversizePkts != 0)
                diag_util_printf("%-35s: %22u\n","etherStatsRxOversizePkts",mib.etherStatsRxOversizePkts);
            if(!nonZero || mib.etherStatsTxPkts64Octets != 0)
                diag_util_printf("%-35s: %22u\n","etherStatsTxPkts64Octets",mib.etherStatsTxPkts64Octets);
            if(!nonZero || mib.etherStatsTxPkts65to127Octets != 0)
                diag_util_printf("%-35s: %22u\n","etherStatsTxPkts65to127Octets",mib.etherStatsTxPkts65to127Octets);
            if(!nonZero || mib.etherStatsTxPkts128to255Octets != 0)
                diag_util_printf("%-35s: %22u\n","etherStatsTxPkts128to255Octets",mib.etherStatsTxPkts128to255Octets);
            if(!nonZero || mib.etherStatsTxPkts256to511Octets != 0)
                diag_util_printf("%-35s: %22u\n","etherStatsTxPkts256to511Octets",mib.etherStatsTxPkts256to511Octets);
            if(!nonZero || mib.etherStatsTxPkts512to1023Octets != 0)
                diag_util_printf("%-35s: %22u\n","etherStatsTxPkts512to1023Octets",mib.etherStatsTxPkts512to1023Octets);
            if(!nonZero || mib.etherStatsTxPkts1024to1518Octets != 0)
                diag_util_printf("%-35s: %22u\n","etherStatsTxPkts1024to1518Octets",mib.etherStatsTxPkts1024to1518Octets);
            if(!nonZero || mib.etherStatsTxOversizePkts != 0)
                diag_util_printf("%-35s: %22u\n","etherStatsTxOversizePkts",mib.etherStatsTxOversizePkts);
            diag_util_printf("\n");
        }
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_mib_dump_vlan_counter_index_index_nonZero */



/*
 * rt_mib reset vlan-counter index <PORT_LIST:index>
 */
cparser_result_t
cparser_cmd_rt_mib_reset_vlan_counter_index_index(
    cparser_context_t *context,
    char * *index_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rt_port_t port;

    memset(&portlist, 0, sizeof(diag_portlist_t));
    if ((ret = diag_util_str2LPortMask(context->parser->tokens[4].buf, &(portlist.portmask))) != RT_ERR_OK)
    {
        diag_util_printf("port list ERROR!\n");
        RT_ERR(ret, (MOD_DIAGSHELL), "port list=%s", context->parser->tokens[4].buf);
        return ret;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rt_stat_vlanCnt_mib_reset(port), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_rt_mib_reset_vlan_counter_index_index */
