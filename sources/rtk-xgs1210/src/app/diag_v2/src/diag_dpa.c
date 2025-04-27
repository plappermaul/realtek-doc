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
 * $Revision: 84527 $
 * $Date: 2017-12-18 16:02:37 +0800 (Mon, 18 Dec 2017) $
 *
 * Purpose : Definition those OpenFlow command and APIs in the SDK diagnostic shell.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) OpenFlow command
 *
 */

/*
 * Include Files
 */
#include <common/debug/rt_log.h>
#include <common/rt_error.h>
#include <common/rt_type.h>
#include <common/util/rt_util.h>
#include <osal/memory.h>
#include <rtk/openflow.h>
#include <parser/cparser_priv.h>
#include <diag_util.h>
#include <diag_om.h>
#include <diag_str.h>

#ifdef CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE
  #include <rtrpc/rtrpc_openflow.h>
#endif

#include "rtk_sdn.h"
#include "rtk_dpa.h"
#include "rtk_gtp_fpga.h"


#ifdef CMD_MECDPA_INIT
/*
 * openflow mecdpa init
 */

cparser_result_t cparser_cmd_mecdpa_init(cparser_context_t *context)
{
    int32 ret;

    DIAG_UTIL_ERR_CHK(rtk_dpa_module_init(), ret);
    DIAG_UTIL_MPRINTF("rtk_dpa_module_init() ret : %d\n", ret);

    return CPARSER_OK;

}
#endif

#ifdef CMD_MECDPA_PORT_ADD_DEL
cparser_result_t cparser_cmd_mecdpa_port_port_idx_add_del(cparser_context_t *context,
    uint32_t *port_idx_ptr)
{
    uint32 port;
    int ret = CPARSER_OK;

    port = *port_idx_ptr;

    if ('a' == TOKEN_CHAR(3, 0))
    {
        DIAG_UTIL_MPRINTF("ADD port : %d", port);
        ret = rtk_sdn_port_add(port);
        if(ret != CPARSER_OK)
            DIAG_UTIL_MPRINTF(" Fail.\n");
        else
            DIAG_UTIL_MPRINTF(" Success.\n");
    }
    else if ('d' == TOKEN_CHAR(3, 0))
    {
        DIAG_UTIL_MPRINTF("DEL port : %d\n", port);
        ret = rtk_sdn_port_del(port);
        if(ret != CPARSER_OK)
            DIAG_UTIL_MPRINTF(" Fail.\n");
        else
            DIAG_UTIL_MPRINTF(" Success.\n");
    }
    return CPARSER_OK;
}
#endif

#ifdef CMD_MECDPA_RULE_ADD_DEL_FIND_TABLE_MATCH_UDP_DEST_PORT_ACTION_DATA
#define RULE_OP_ADD                 0
#define RULE_OP_DEL                 1
#define RULE_OP_FIND                2

#define RULE_ACTION_GOTO            0
#define RULE_ACTION_OUTPUT          1
#define RULE_ACTION_ENCAP           2
#define RULE_ACTION_DECAP           3

cparser_result_t cparser_cmd_mecdpa_rule_add_del_find_table_table_idx_match_upd_dest_port_upd_destPort_action_goto_output_encap_decap_data_action_data(cparser_context_t *context,
    uint32_t *table_idx_ptr,
    uint32_t *upd_destPort_ptr,
    uint32_t *action_data_ptr)
{
    uint32                  table;
    uint32                  udp_destPort;
    uint32                  action_data;
    uint32                  rule_op = RULE_OP_ADD;
    sdn_db_match_field_t    *rule_match_field;

    sdn_db_flow_entry_t     *flow_entry = NULL;
    sdn_db_instruction_t    *pInst = NULL;
    sdn_db_inst_goto_table_t *p_goto_table = NULL;

    int                     ret = 0;

    table = *table_idx_ptr;
    udp_destPort = *upd_destPort_ptr;
    action_data = *action_data_ptr;

    flow_entry = (sdn_db_flow_entry_t *) malloc(sizeof(sdn_db_flow_entry_t));
    rule_match_field = (sdn_db_match_field_t *) malloc(sizeof(sdn_db_match_field_t));

    flow_entry->table_id = table;
    flow_entry->len_match = 1;
    flow_entry->priority = 100;
    rule_match_field->oxm_class = EN_OFPXMC_OPENFLOW_BASIC;
    rule_match_field->oxm_type = EN_OFPXMT_OFB_UDP_DST;
    rule_match_field->value.low = (uint64_t)udp_destPort;
    rule_match_field->value.high = (uint64_t)0xffffffff;
    flow_entry->match_field_p = rule_match_field;

    if ('a' == TOKEN_CHAR(2, 0))
    {
        DIAG_UTIL_MPRINTF("ADD table(%d) udp_desrPort = %d", table,udp_destPort);
        rule_op = RULE_OP_ADD;
    }
    else if ('d' == TOKEN_CHAR(2, 0))
    {
        DIAG_UTIL_MPRINTF("DEL table(%d) udp_desrPort = %d", table,udp_destPort);
        rule_op = RULE_OP_DEL;
    }
    else if ('f' == TOKEN_CHAR(2, 0))
    {
        DIAG_UTIL_MPRINTF("FIND table(%d) udp_desrPort = %d", table,udp_destPort);
        rule_op = RULE_OP_FIND;
    }

    if ('g' == TOKEN_CHAR(9, 0))
    {
        DIAG_UTIL_MPRINTF(" ACTION GOTO, action data = 0x%08x\n",action_data);
    }
    else if ('o' == TOKEN_CHAR(9, 0))
    {
        DIAG_UTIL_MPRINTF(" ACTION OUTPUT, action data = 0x%08x\n",action_data);
    }
    else if ('e' == TOKEN_CHAR(9, 0))
    {
        DIAG_UTIL_MPRINTF(" ACTION ENCAP, action data = 0x%08x\n",action_data);
    }
    else if ('d' == TOKEN_CHAR(9, 0))
    {
        DIAG_UTIL_MPRINTF(" ACTION DECAP, action data = 0x%08x\n",action_data);
    }

    pInst = malloc(sizeof(sdn_db_instruction_t) * 1);

    pInst->type = EN_OFPIT_GOTO_TABLE; /* EN_OFPIT_GOTO_TABLE */
    p_goto_table = (sdn_db_inst_goto_table_t *) malloc(sizeof(sdn_db_inst_goto_table_t));
    p_goto_table->table_id = 2;
    pInst->data_p = p_goto_table;

    flow_entry->instruction_p = pInst;

    if(rule_op == RULE_OP_ADD)
        ret = rtk_sdn_add_flow(flow_entry);
    else
        ret = rtk_sdn_delete_flow(table, flow_entry->priority, 1, rule_match_field);

    if(ret != 0)
        DIAG_UTIL_MPRINTF(" Table access FAILED!!!, ret = %d\n", ret);

    return CPARSER_OK;


}
#endif

#ifdef CMD_MECDPA_TABLE_TEID_LEARN_RULE_ADD_DEL_FIND_MATCH_UDP_PORT_SRC_PORT_ACTION_DATA
//mecdpa table teid_learn rule ( add | del | find ) match upd_dest_port <UINT:upd_destPort> phy_src_port <UINT:src_port> action ( goto | output | encap | decap ) data <UINT:action_data>
#define UINT32_FULL_MASK        0xffffffff
#define UINT16_FULL_MASK        0xffff
#define UINT8_FULL_MASK         0xff

cparser_result_t cparser_cmd_mecdpa_table_teid_learn_rule_add_del_find_match_upd_dest_port_upd_destPort_phy_src_port_src_port_action_goto_output_encap_decap_data_action_data(cparser_context_t *context,
    uint32_t *upd_destPort_ptr,
    uint32_t *src_port_ptr,
    uint32_t *action_data_ptr)
{
    rtk_dpa_operation_t             rule_op = RTK_DPA_OPERATION_ADD;
    rtk_dpa_teid_learn_match_t      *rule_match_field;
    int                             match_num = 1;
    rtk_dpa_teid_learn_action_t     *rule_action_field;
    int                             action_num = 1;
    int                             ret = CPARSER_NOT_OK;
    int                             entry_idx = 0;
    int                             entry_found = 0;

    if ('a' == TOKEN_CHAR(4, 0))
    {
        DIAG_UTIL_MPRINTF("ADD TEID LEARN table: PHY src port = %d; udp_desrPort = %d", *src_port_ptr, *upd_destPort_ptr);
        rule_op = RTK_DPA_OPERATION_ADD;
    }
    else if ('d' == TOKEN_CHAR(4, 0))
    {
        DIAG_UTIL_MPRINTF("DEL TEID LEARN table: PHY src port = %d; udp_desrPort = %d", *src_port_ptr, *upd_destPort_ptr);
        rule_op = RTK_DPA_OPERATION_DEL;
    }
    else if ('f' == TOKEN_CHAR(4, 0))
    {
        DIAG_UTIL_MPRINTF("FIND TEID LEARN table: PHY src port = %d; udp_desrPort = %d", *src_port_ptr, *upd_destPort_ptr);
        rule_op = RTK_DPA_OPERATION_FIND;
    }else{
        DIAG_UTIL_MPRINTF("Unknown Table action!!!");
        return CPARSER_NOT_OK;
    }

    /* Fill match fields */
    rule_match_field = (rtk_dpa_teid_learn_match_t *) malloc((sizeof(rtk_dpa_teid_learn_match_t)) * match_num);
    rule_match_field[0].type = RTK_DPA_TEID_LEARN_TYPE_L4_DP;
    rule_match_field[0].data.L4port = *upd_destPort_ptr;
    rule_match_field[0].mask.L4port = UINT32_FULL_MASK;

//    rule_match_field[0].type = RTK_DPA_TEID_LEARN_TYPE_SRC_PORT;
//    rule_match_field[0].data.src_port = (uint8)*src_port_ptr;
//    rule_match_field[0].mask.src_port = UINT8_FULL_MASK;


#if 0
    rule_match_field[0].type = RTK_DPA_TEID_LEARN_TYPE_TEID;
    rule_match_field[0].data.teid = 0x3333;
    rule_match_field[0].mask.teid = 0xffff;

    rule_match_field[1].type = RTK_DPA_TEID_LEARN_TYPE_SRC_PORT;
    rule_match_field[1].data.src_port = (uint8)*src_port_ptr;
    rule_match_field[1].mask.src_port = UINT8_FULL_MASK;

    rule_match_field[2].type = RTK_DPA_TEID_LEARN_TYPE_DIP;
    rule_match_field[2].data.dip = 0xabcdef;
    rule_match_field[2].mask.dip = UINT32_FULL_MASK;

#endif
    /* Fill action fields */
    rule_action_field = (rtk_dpa_teid_learn_action_t *) malloc((sizeof(rtk_dpa_teid_learn_action_t)) * action_num);
    rule_action_field[0].action = RTK_DPA_TEID_LEARN_ACTION_TYPE_OUTPUT;

#if 0
    rule_action_field[1].action = RTK_DPA_TEID_LEARN_ACTION_TYPE_RATELIMIT;
    rule_action_field[1].data.ratelimit.rate = 1000;
    rule_action_field[1].data.ratelimit.rate_limit_type = 0;
#endif
    if ('g' == TOKEN_CHAR(11, 0))
    {
        DIAG_UTIL_MPRINTF("ACTION is GOTO Table (%d)", *action_data_ptr);
        rule_action_field[0].action = RTK_DPA_TEID_LEARN_ACTION_TYPE_GOTO;
        rule_action_field[0].data.tableName = *action_data_ptr;
    }
    else if ('o' == TOKEN_CHAR(11, 0))
    {
        DIAG_UTIL_MPRINTF("ACTION is OUTPUT PORT = 0x%08x", *action_data_ptr);
        rule_action_field[0].action = RTK_DPA_TEID_LEARN_ACTION_TYPE_OUTPUT;
        rule_action_field[0].data.portmask.portmask.bits[0] = *action_data_ptr;
        rule_action_field[0].data.portmask.portmask.bits[1] = 0;
    }
    else if ('e' == TOKEN_CHAR(11, 0))
    {
        DIAG_UTIL_MPRINTF("ACTION is ENCAP output data = 0x%08x", *action_data_ptr);
    }
    else if ('d' == TOKEN_CHAR(11, 0))
    {
        DIAG_UTIL_MPRINTF("ACTION is DECAP output data = 0x%08x", *action_data_ptr);
        rule_action_field[0].action = RTK_DPA_TEID_LEARN_ACTION_TYPE_DECAP;
    }else{
        DIAG_UTIL_MPRINTF("Unknown Table action!!!");
        return CPARSER_NOT_OK;
    }


    if(rule_op != RTK_DPA_OPERATION_FIND)
    {
        ret = rtk_dpa_teid_learn_config(rule_op, rule_match_field, match_num, rule_action_field, action_num);
        if(ret != RT_ERR_OK)
            DIAG_UTIL_MPRINTF("\nrtk_dpa_teid_learn_config() OPEATION Failed!!!\n");
        else
            DIAG_UTIL_MPRINTF("\nrtk_dpa_teid_learn_config() OPEATION Success!!!\n");
    }else{
        ret = rtk_dpa_teid_learn_find(rule_match_field, match_num, &entry_idx, &entry_found);
        if(ret != RT_ERR_OK)
        {
            DIAG_UTIL_MPRINTF("\nrtk_dpa_teid_learn_find() OPEATION Failed!!!\n");
        }else{
            if(entry_found == TRUE)
            {
                DIAG_UTIL_MPRINTF("\nrtk_dpa_teid_learn_find() Entry(%d) is FOUND!!!\n",entry_idx);
            } else {
                DIAG_UTIL_MPRINTF("\nrtk_dpa_teid_learn_find() Entry not FOUND!!!\n");
            }
        }
    }

    free(rule_match_field);
    free(rule_action_field);

    return CPARSER_OK;
}

#endif

#ifdef CMD_MECDPA_TABLE_IP_FWD_RULE_ADD_DEL_FIND_MATCH_DIP_SRC_PORT_ACTION_DATA
//mecdpa table ip_fwd rule ( add | del | find ) match dip <UINT:destIp> phy_src_port <UINT:src_port> action ( goto | output | encap ) data <UINT:action_data>

cparser_result_t cparser_cmd_mecdpa_table_ip_fwd_rule_add_del_find_match_dip_destIp_phy_src_port_src_port_action_goto_output_encap_data_action_data(cparser_context_t *context,
    uint32_t *destIp_ptr,
    uint32_t *src_port_ptr,
    uint32_t *action_data_ptr)

{
    rtk_dpa_operation_t             rule_op = RTK_DPA_OPERATION_ADD;
    rtk_dpa_ip_fwd_match_t          *rule_match_field;
    int                             match_num = 1;
    rtk_dpa_ip_fwd_action_t         *rule_action_field;
    int                             action_num = 1;
    int                             ret = CPARSER_NOT_OK;
    int                             entry_idx = 0;
    int                             entry_found = 0;

    if ('a' == TOKEN_CHAR(4, 0))
    {
        DIAG_UTIL_MPRINTF("ADD TEID LEARN table: PHY src port = %d; DestIP = 0x%08x", *src_port_ptr, *destIp_ptr);
        rule_op = RTK_DPA_OPERATION_ADD;
    }
    else if ('d' == TOKEN_CHAR(4, 0))
    {
        DIAG_UTIL_MPRINTF("DEL TEID LEARN table: PHY src port = %d; DestIP = 0x%08x", *src_port_ptr, *destIp_ptr);
        rule_op = RTK_DPA_OPERATION_DEL;
    }
    else if ('f' == TOKEN_CHAR(4, 0))
    {
        DIAG_UTIL_MPRINTF("FIND TEID LEARN table: PHY src port = %d; DestIP = 0x%08x", *src_port_ptr, *destIp_ptr);
        rule_op = RTK_DPA_OPERATION_FIND;
    }else{
        DIAG_UTIL_MPRINTF("Unknown Table action!!!");
        return CPARSER_NOT_OK;
    }

    /* Fill match fields */
    rule_match_field = (rtk_dpa_ip_fwd_match_t *) malloc((sizeof(rtk_dpa_ip_fwd_match_t)) * match_num);
    rule_match_field[0].type = RTK_DPA_IP_FWD_TYPE_DIP;
    rule_match_field[0].data.dip = *destIp_ptr;
    rule_match_field[0].mask.dip = UINT32_FULL_MASK;
#if 0
    rule_match_field[1].type = RTK_DPA_IP_FWD_TYPE_SRC_PORT;
    rule_match_field[1].data.src_port = (uint8)*src_port_ptr;
    rule_match_field[1].mask.src_port = UINT8_FULL_MASK;

#endif
    rule_action_field = (rtk_dpa_ip_fwd_action_t *) malloc((sizeof(rtk_dpa_ip_fwd_action_t)) * action_num);

    if ('g' == TOKEN_CHAR(11, 0))
    {
        DIAG_UTIL_MPRINTF("ACTION is GOTO Table (%d)", *action_data_ptr);
        rule_action_field[0].action = RTK_DPA_IP_FWD_ACTION_TYPE_GOTO;
        rule_action_field[0].data.tableName = *action_data_ptr;
    }
    else if ('o' == TOKEN_CHAR(11, 0))
    {
        DIAG_UTIL_MPRINTF("ACTION is OUTPUT PORT = 0x%08x", *action_data_ptr);
        rule_action_field[0].action = RTK_DPA_IP_FWD_ACTION_TYPE_OUTPUT;
#if 1
        rule_action_field[0].data.output.portmask.bits[0] = *action_data_ptr;
        rule_action_field[0].data.output.portmask.bits[1] = 0x01000000;
        rule_action_field[0].data.output.dmac.octet[5] = 0x00;
        rule_action_field[0].data.output.dmac.octet[4] = 0xe0;
        rule_action_field[0].data.output.dmac.octet[3] = 0x4c;
        rule_action_field[0].data.output.dmac.octet[2] = 0x00;
        rule_action_field[0].data.output.dmac.octet[1] = 0x00;
        rule_action_field[0].data.output.dmac.octet[0] = 0x00;

        rule_action_field[0].data.output.smac.octet[5] = 0xc0;
        rule_action_field[0].data.output.smac.octet[4] = 0x25;
        rule_action_field[0].data.output.smac.octet[3] = 0xe9;
        rule_action_field[0].data.output.smac.octet[2] = 0x1e;
        rule_action_field[0].data.output.smac.octet[1] = 0xd7;
        rule_action_field[0].data.output.smac.octet[0] = 0x64;
#endif
    }
    else if ('e' == TOKEN_CHAR(11, 0))
    {
        DIAG_UTIL_MPRINTF("ACTION is ENCAP output data = 0x%08x", *action_data_ptr);
        rule_action_field[0].action = RTK_DPA_IP_FWD_ACTION_TYPE_ENCAP;
        /* Fill action fields */
        rule_action_field = (rtk_dpa_ip_fwd_action_t *) malloc((sizeof(rtk_dpa_ip_fwd_action_t)) * action_num);

        rule_action_field[0].data.encap.diffserv        = 0x1234;
        rule_action_field[0].data.encap.ttl             = 0x25;
        rule_action_field[0].data.encap.outer_dip       = 0xc0A801c8;
        rule_action_field[0].data.encap.outer_sip       = 0xc0A80132;
        rule_action_field[0].data.encap.teid            = 0x3333;
        rule_action_field[0].data.encap.outer_udp_destPort = 2152;
        rule_action_field[0].data.encap.outer_udp_srcPort  = 2152;
        rule_action_field[0].data.encap.identification  = 0x4567;
    }
    else if ('d' == TOKEN_CHAR(11, 0))
    {
        DIAG_UTIL_MPRINTF("ACTION is DECAP output data = 0x%08x", *action_data_ptr);
    }else{
        DIAG_UTIL_MPRINTF("Unknown Table action!!!");
        return CPARSER_NOT_OK;
    }


    if(rule_op != RTK_DPA_OPERATION_FIND)
    {
        ret = rtk_dpa_ip_fwd_config(rule_op, rule_match_field, match_num, rule_action_field, action_num);
        if(ret != RT_ERR_OK)
            DIAG_UTIL_MPRINTF("\nrtk_dpa_ip_fwd_config() OPEATION Failed!!!\n");
        else
            DIAG_UTIL_MPRINTF("\nrtk_dpa_ip_fwd_config() OPEATION Success!!!\n");
    }else{
        ret = rtk_dpa_ip_fwd_find(rule_match_field, match_num, &entry_idx, &entry_found);
        if(ret != RT_ERR_OK)
        {
            DIAG_UTIL_MPRINTF("\nrtk_dpa_ip_fwd_find() OPEATION Failed!!!\n");
        }else{
            if(entry_found == TRUE)
            {
                DIAG_UTIL_MPRINTF("\nrtk_dpa_ip_fwd_find() Entry(%d) is FOUND!!!\n",entry_idx);
            } else {
                DIAG_UTIL_MPRINTF("\nrtk_dpa_ip_fwd_find() Entry not FOUND!!!\n");
            }
        }
    }

    free(rule_match_field);
    free(rule_action_field);

    return CPARSER_OK;
}

#endif

#ifdef CMD_MECDPA_TABLE_APP_LIST_RULE_ADD_DEL_FIND_MATCH_L4DP_ETH_TYPE_ACTION_DATA
// mecdpa table app_list rule ( add | del | find ) match l4_dp <UINT:l4dp> eth_type <UINT:ethType> action ( output | decap ) data <UINT:action_data>

cparser_result_t cparser_cmd_mecdpa_table_app_list_rule_add_del_find_match_l4_dp_l4dp_eth_type_ethType_action_output_decap_data_action_data(cparser_context_t *context,
    uint32_t *l4dp_ptr,
    uint32_t *ethType_ptr,
    uint32_t *action_data_ptr)
{
    rtk_dpa_operation_t             rule_op = RTK_DPA_OPERATION_ADD;
    rtk_dpa_app_list_match_t        *rule_match_field;
    int                             match_num = 1;
//    int                             match_num = 1;
    rtk_dpa_app_list_action_t       *rule_action_field;
    int                             action_num = 1;
    int                             ret = CPARSER_NOT_OK;
    int                             entry_idx = 0;
    int                             entry_found = 0;

    if ('a' == TOKEN_CHAR(4, 0))
    {
        DIAG_UTIL_MPRINTF("ADD TEID LEARN table: l4dp_ptr = %d; ethType_ptr = 0x%08x", *l4dp_ptr, *ethType_ptr);
        rule_op = RTK_DPA_OPERATION_ADD;
    }
    else if ('d' == TOKEN_CHAR(4, 0))
    {
        DIAG_UTIL_MPRINTF("DEL TEID LEARN table: l4dp_ptr = %d; ethType_ptr = 0x%08x", *l4dp_ptr, *ethType_ptr);
        rule_op = RTK_DPA_OPERATION_DEL;
    }
    else if ('f' == TOKEN_CHAR(4, 0))
    {
        DIAG_UTIL_MPRINTF("FIND TEID LEARN table: l4dp_ptr = %d; ethType_ptr = 0x%08x", *l4dp_ptr, *ethType_ptr);
        rule_op = RTK_DPA_OPERATION_FIND;
    }else{
        DIAG_UTIL_MPRINTF("Unknown Table action!!!");
        return CPARSER_NOT_OK;
    }

    /* Fill match fields */
    rule_match_field = (rtk_dpa_app_list_match_t *) malloc((sizeof(rtk_dpa_app_list_match_t)) * match_num);
    rule_match_field[0].type = RTK_DPA_APP_LIST_TYPE_OUT_L4DP;
    rule_match_field[0].data.outer_L4port = *l4dp_ptr;
    rule_match_field[0].mask.outer_L4port = UINT32_FULL_MASK;
#if 0
    rule_match_field[1].type = RTK_DPA_APP_LIST_TYPE_IP_FLAG;
    rule_match_field[1].data.ip_flag = 0x5;
    rule_match_field[1].mask.ip_flag = 0x7;

    rule_match_field[3].type = RTK_DPA_APP_LIST_TYPE_IN_L4DP;
    rule_match_field[3].data.inner_L4port = 0x1234;
    rule_match_field[3].mask.inner_L4port = 0x5678;

    rule_match_field[4].type = RTK_DPA_APP_LIST_TYPE_IN_SIP;
    rule_match_field[4].data.inner_sip = 0xaabbccdd;
    rule_match_field[4].mask.inner_sip = 0x55aaFF00;

    rule_match_field[5].type = RTK_DPA_APP_LIST_TYPE_IN_DIP;
    rule_match_field[5].data.inner_dip = 0x11223344;
    rule_match_field[5].mask.inner_dip = 0x5555aaaa;

    rule_match_field[1].type = RTK_DPA_APP_LIST_TYPE_ETH_TYPE;
    rule_match_field[1].data.eth_type = *ethType_ptr;
    rule_match_field[1].mask.eth_type = UINT16_FULL_MASK;

    rule_match_field[0].type = RTK_DPA_APP_LIST_TYPE_IN_SIP;
    rule_match_field[0].data.inner_sip = 0xac1592c4;
    rule_match_field[0].mask.inner_sip = 0xffffffff;

    rule_match_field[0].type = RTK_DPA_APP_LIST_TYPE_IN_DIP;
    rule_match_field[0].data.inner_sip = 0xac15010a;
    rule_match_field[0].mask.inner_sip = 0xffffffff;

    rule_match_field[0].type = RTK_DPA_APP_LIST_TYPE_IN_IPVER;
    rule_match_field[0].data.inner_ip_ver = 0x4;
    rule_match_field[0].mask.inner_ip_ver = 0xf;

#endif


    /* Fill action fields */
    rule_action_field = (rtk_dpa_app_list_action_t *) malloc((sizeof(rtk_dpa_app_list_action_t)) * action_num);


    if ('o' == TOKEN_CHAR(11, 0))
    {
        DIAG_UTIL_MPRINTF("ACTION is OUTPUT PORT = 0x%08x", *action_data_ptr);
        rule_action_field[0].action = RTK_DPA_APP_LIST_ACTION_TYPE_OUTPUT;
        rule_action_field[0].data.portmask.portmask.bits[0] = *action_data_ptr;
        rule_action_field[0].data.portmask.portmask.bits[1] = 0;
    }
    else if ('d' == TOKEN_CHAR(11, 0))
    {
        DIAG_UTIL_MPRINTF("ACTION is DECAP output data = 0x%08x", *action_data_ptr);
    }else{
        DIAG_UTIL_MPRINTF("Unknown Table action!!!");
        return CPARSER_NOT_OK;
    }


    if(rule_op != RTK_DPA_OPERATION_FIND)
    {
        ret = rtk_dpa_app_list_config(rule_op, rule_match_field, match_num, rule_action_field, action_num);
        if(ret != RT_ERR_OK)
            DIAG_UTIL_MPRINTF("\nrtk_dpa_app_list_config() OPEATION Failed!!!\n");
        else
            DIAG_UTIL_MPRINTF("\nrtk_dpa_app_list_config() OPEATION Success!!!\n");
    }else{
        ret = rtk_dpa_app_list_find(rule_match_field, match_num, &entry_idx, &entry_found);
        if(ret != RT_ERR_OK)
        {
            DIAG_UTIL_MPRINTF("\nrtk_dpa_ip_fwd_find() OPEATION Failed!!!\n");
        }else{
            if(entry_found == TRUE)
            {
                DIAG_UTIL_MPRINTF("\nrtk_dpa_app_list_find() Entry(%d) is FOUND!!!\n",entry_idx);
            } else {
                DIAG_UTIL_MPRINTF("\nrtk_dpa_app_list_find() Entry not FOUND!!!\n");
            }
        }
    }

    free(rule_match_field);
    free(rule_action_field);

    return CPARSER_OK;


}
#endif

#ifdef CMD_MECDPA_TABLE_L2_FWD_RULE_ADD_DEL_FIND_MATCH_DIP_L4DP_ETH_TYPE_ACTION_DATA
// mecdpa table l2_fwd rule ( add | del | find ) match l4_dp <UINT:l4dp> eth_type <UINT:ethType> action ( output | goto ) data <UINT:action_data>
cparser_result_t cparser_cmd_mecdpa_table_l2_fwd_rule_add_del_find_match_l4_dp_l4dp_eth_type_ethType_action_output_goto_data_action_data(cparser_context_t *context,
    uint32_t *l4dp_ptr,
    uint32_t *ethType_ptr,
    uint32_t *action_data_ptr)
{
    rtk_dpa_operation_t             rule_op = RTK_DPA_OPERATION_ADD;
    rtk_dpa_l2_fwd_match_t         *rule_match_field;
    int                             match_num = 1;
    rtk_dpa_l2_fwd_action_t        *rule_action_field;
    int                             action_num = 1;
    int                             ret = CPARSER_NOT_OK;
    int                             entry_idx = 0;
    int                             entry_found = 0;

    if ('a' == TOKEN_CHAR(4, 0))
    {
        DIAG_UTIL_MPRINTF("ADD TEID LEARN table: l4dp_ptr = %d; ethType_ptr = 0x%08x", *l4dp_ptr, *ethType_ptr);
        rule_op = RTK_DPA_OPERATION_ADD;
    }
    else if ('d' == TOKEN_CHAR(4, 0))
    {
        DIAG_UTIL_MPRINTF("DEL TEID LEARN table: l4dp_ptr = %d; ethType_ptr = 0x%08x", *l4dp_ptr, *ethType_ptr);
        rule_op = RTK_DPA_OPERATION_DEL;
    }
    else if ('f' == TOKEN_CHAR(4, 0))
    {
        DIAG_UTIL_MPRINTF("FIND TEID LEARN table: l4dp_ptr = %d; ethType_ptr = 0x%08x", *l4dp_ptr, *ethType_ptr);
        rule_op = RTK_DPA_OPERATION_FIND;
    }else{
        DIAG_UTIL_MPRINTF("Unknown Table action!!!");
        return CPARSER_NOT_OK;
    }

    /* Fill match fields */
    rule_match_field = (rtk_dpa_l2_fwd_match_t *) malloc((sizeof(rtk_dpa_l2_fwd_match_t)) * match_num);

    rule_match_field[0].type = RTK_DPA_L2_FWD_TYPE_ETH_TYPE;
    rule_match_field[0].data.eth_type = *ethType_ptr;
    rule_match_field[0].mask.eth_type = 0xffff;

#if 0
    rule_match_field[0].type = RTK_DPA_L2_FWD_TYPE_ETH_TYPE;
    rule_match_field[0].data.eth_type = *ethType_ptr;
    rule_match_field[0].mask.eth_type = 0xffff;

    rule_match_field[0].type = RTK_DPA_L2_FWD_TYPE_L4DP;
    rule_match_field[0].data.L4port = *l4dp_ptr;
    rule_match_field[0].mask.L4port = UINT32_FULL_MASK;

    rule_match_field[0].type = RTK_DPA_L2_FWD_TYPE_DMAC;
    rule_match_field[0].data.dmac.octet[5] = 0x00;
    rule_match_field[0].data.dmac.octet[4] = 0x11;
    rule_match_field[0].data.dmac.octet[3] = 0x22;
    rule_match_field[0].data.dmac.octet[2] = 0x33;
    rule_match_field[0].data.dmac.octet[1] = 0x44;
    rule_match_field[0].data.dmac.octet[0] = 0x55;
    rule_match_field[0].mask.dmac.octet[5] = 0xff;
    rule_match_field[0].mask.dmac.octet[4] = 0xff;
    rule_match_field[0].mask.dmac.octet[3] = 0xff;
    rule_match_field[0].mask.dmac.octet[2] = 0xff;
    rule_match_field[0].mask.dmac.octet[1] = 0xff;
    rule_match_field[0].mask.dmac.octet[0] = 0xff;


    rule_match_field[2].type = RTK_DPA_L2_FWD_TYPE_IPVER;
    rule_match_field[2].data.ip_ver = 0x4;
    rule_match_field[2].mask.ip_ver = 0xf;

    rule_match_field[3].type = RTK_DPA_L2_FWD_TYPE_TCP_FLAG;
    rule_match_field[3].data.tcp_flag = 0x25;
    rule_match_field[3].mask.tcp_flag = 0x3f;

    rule_match_field[0].type = RTK_DPA_L2_FWD_TYPE_GTP_MT;
    rule_match_field[0].data.gtp_mt = 0x11;
    rule_match_field[0].mask.gtp_mt = 0x22;
#endif


    /* Fill action fields */
    rule_action_field = (rtk_dpa_l2_fwd_action_t *) malloc((sizeof(rtk_dpa_l2_fwd_action_t)) * action_num);

    if ('o' == TOKEN_CHAR(11, 0))
    {
        DIAG_UTIL_MPRINTF("ACTION is OUTPUT PORT = 0x%08x", *action_data_ptr);
        rule_action_field[0].action = RTK_DPA_L2_FWD_ACTION_TYPE_OUTPUT;
        rule_action_field[0].data.portmask.portmask.bits[0] = *action_data_ptr;
        rule_action_field[0].data.portmask.portmask.bits[1] = 0x01000000;
    }
    else if ('g' == TOKEN_CHAR(11, 0))
    {
        DIAG_UTIL_MPRINTF("ACTION is GOTO Table (%d)", *action_data_ptr);
        rule_action_field[0].action = RTK_DPA_L2_FWD_ACTION_TYPE_GOTO;
        rule_action_field[0].data.tableName = *action_data_ptr;
    }else{
        DIAG_UTIL_MPRINTF("Unknown Table action!!!");
        return CPARSER_NOT_OK;
    }


    if(rule_op != RTK_DPA_OPERATION_FIND)
    {
        ret = rtk_dpa_l2_fwd_config(rule_op, rule_match_field, match_num, rule_action_field, action_num);
        if(ret != RT_ERR_OK)
            DIAG_UTIL_MPRINTF("\nrtk_dpa_l2_fwd_config() OPEATION Failed!!!\n");
        else
            DIAG_UTIL_MPRINTF("\nrtk_dpa_l2_fwd_config() OPEATION Success!!!\n");
    }else{
        ret = rtk_dpa_l2_fwd_find(rule_match_field, match_num, &entry_idx, &entry_found);
        if(ret != RT_ERR_OK)
        {
            DIAG_UTIL_MPRINTF("\nrtk_dpa_l2_fwd_find() OPEATION Failed!!!\n");
        }else{
            if(entry_found == TRUE)
            {
                DIAG_UTIL_MPRINTF("\nrtk_dpa_l2_fwd_find() Entry(%d) is FOUND!!!\n",entry_idx);
            } else {
                DIAG_UTIL_MPRINTF("\nrtk_dpa_ip_fwd_find() Entry not FOUND!!!\n");
            }
        }
    }

    free(rule_match_field);
    free(rule_action_field);

    return CPARSER_OK;


}
#endif


#ifdef CMD_MECDPA_STATUS_GET_SETDEF_ACTIVE_CAPACITY
// mecdpa status ( get | set_default | active_rule | capacity )
cparser_result_t cparser_cmd_mecdpa_status_get_set_default_active_rule_capacity(cparser_context_t *context)
{
    int                             ret = CPARSER_NOT_OK;
    rtk_dpa_status_t                dpaModule_status;
    uint8                           table_idx = RTK_DPA_TABLE_NANE_TEID_LEARN;
    int                             active_rule = 0;
    rtk_dpa_capacity_t              dpa_cap;

    if ('g' == TOKEN_CHAR(2, 0))
    {
        ret = rtk_dpa_status_get(&dpaModule_status);
        if(dpaModule_status == RTK_DPA_STATUS_READY)
            DIAG_UTIL_MPRINTF("\nDPA Module is READY.\n");
        else
            DIAG_UTIL_MPRINTF("\nDPA Module is NOT READY !!!\n");
    }
    else if ('s' == TOKEN_CHAR(2, 0))
    {
        ret = rtk_dpa_factoryDefault_config();
        if(ret == RT_ERR_OK)
            DIAG_UTIL_MPRINTF("\nFactory default the DPA Module is Done.\n");
        else
            DIAG_UTIL_MPRINTF("\nFactory default the DPA Module is Failed!!!.\n");

    }
    else if ('a' == TOKEN_CHAR(2, 0))
    {

        for(table_idx = RTK_DPA_TABLE_NANE_TEID_LEARN; table_idx < RTK_DPA_TABLE_NANE_END; table_idx++)
        {
            ret = rtk_dpa_table_activeEntryNum_get(table_idx, &active_rule);
            if(ret == RT_ERR_OK)
                DIAG_UTIL_MPRINTF("\nTable(%d) Active entry = %d.\n",table_idx,active_rule);
            else
                DIAG_UTIL_MPRINTF("\nTable(%d) Get Active entry Failed.\n",table_idx);
        }
    }
    else if ('c' == TOKEN_CHAR(2, 0))
    {
        ret = rtk_dpa_capacity_get(&dpa_cap);
        if(ret == RT_ERR_OK)
        {
            DIAG_UTIL_MPRINTF("\nCapacity (max_port_num)        = %d\n",dpa_cap.max_port_num);
            DIAG_UTIL_MPRINTF("\nCapacity (teidLearn_table_max) = %d\n",dpa_cap.teidLearn_table_max);
            DIAG_UTIL_MPRINTF("\nCapacity (ipFwd_table_max)     = %d\n",dpa_cap.ipFwd_table_max);
            DIAG_UTIL_MPRINTF("\nCapacity (appList_table_max)   = %d\n",dpa_cap.appList_table_max);
            DIAG_UTIL_MPRINTF("\nCapacity (l2Fwd_table_max)     = %d\n",dpa_cap.l2Fwd_table_max);
            DIAG_UTIL_MPRINTF("\nCapacity (ue_num_max)          = %d\n",dpa_cap.ue_num_max);
            DIAG_UTIL_MPRINTF("\nCapacity (meter_max)           = %d\n",dpa_cap.meter_max);
        }else{
            DIAG_UTIL_MPRINTF("\nGet DPA capacity is Failed!!!.\n");
        }

    }else{
        DIAG_UTIL_MPRINTF("\nAction Failed !!!\n");
        return CPARSER_NOT_OK;
    }
    return CPARSER_OK;
}
#endif

#ifdef CMD_MECDPA_HIT_STS_GET_TABLE_ID_ENTRY_ID
// mecdpa hit_status get table <UINT:table_id> entry <UINT:entry_id>

cparser_result_t cparser_cmd_mecdpa_hit_status_get_table_table_id_entry_entry_id(cparser_context_t *context,
    uint32_t *table_id_ptr,
    uint32_t *entry_id_ptr)
{

    int ret = CPARSER_NOT_OK;
    rtk_dpa_entry_hit_status_t  hit_sts;
    rtk_dpa_tableName_t         table;
    int                         entry_idx;

    table = *table_id_ptr;
    entry_idx = *entry_id_ptr;

    ret = rtk_dpa_tableEntry_hitStatus_get(table, entry_idx, &hit_sts);
    if(ret == RT_ERR_OK)
    {
        if(hit_sts == RTK_DPA_ENTRY_HIT_STATUS_MISS)
            DIAG_UTIL_MPRINTF("\nTable(%d) Entry(%d) HIT MISS !!!\n",table,entry_idx);
        else
            DIAG_UTIL_MPRINTF("\nTable(%d) Entry(%d) HIT ACTION !!!\n",table,entry_idx);
    }else{
        DIAG_UTIL_MPRINTF("\nAction Failed !!!\n");
        return CPARSER_NOT_OK;
    }
    return CPARSER_OK;
}
#endif

#ifdef CMD_MECDPA_TABLE_TEID_LEARN_COUNTER_MATCH_UDP_PORT_SRC_PORT
// mecdpa table teid_learn counter match upd_dest_port <UINT:upd_destPort> phy_src_port <UINT:src_port>
cparser_result_t cparser_cmd_mecdpa_table_teid_learn_counter_match_upd_dest_port_upd_destPort_phy_src_port_src_port(cparser_context_t *context,
    uint32_t *upd_destPort_ptr,
    uint32_t *src_port_ptr)
{
    rtk_dpa_teid_learn_match_t      *rule_match_field;
    int                             match_num = 1;
    int                             ret = CPARSER_NOT_OK;
    uint64_t                        byteCounter = 10, packetCounter = 10;

    /* Fill match fields */
    rule_match_field = (rtk_dpa_teid_learn_match_t *) malloc((sizeof(rtk_dpa_teid_learn_match_t)) * match_num);
    rule_match_field[0].type = RTK_DPA_TEID_LEARN_TYPE_L4_DP;
    rule_match_field[0].data.L4port = *upd_destPort_ptr;
    rule_match_field[0].mask.L4port = UINT32_FULL_MASK;
#if 0
    rule_match_field[1].type = RTK_DPA_TEID_LEARN_TYPE_SRC_PORT;
    rule_match_field[1].data.src_port = (uint8)*src_port_ptr;
    rule_match_field[1].mask.src_port = UINT8_FULL_MASK;
#endif
    ret = rtk_dpa_ueDownlinkCount_get(rule_match_field, match_num, &packetCounter, &byteCounter);
    if(ret == CPARSER_OK)
    {
        printf("\npacketCounter = %lld; byteCounter = %lld !!!\n",packetCounter,byteCounter);
    }else{
        DIAG_UTIL_MPRINTF("\nGet Counter action failed!!!\n");
    }
    return ret;
}
#endif

#ifdef CMD_MECDPA_FPGA_TX_INBAND_VP_IDX_TX_PORT
cparser_result_t cparser_cmd_mecdpa_fpga_tx_inband_vp_idx_vp_index_tx_port_port(cparser_context_t *context,
    uint32_t *vp_index_ptr,
    uint32_t *port_ptr)
{

    rtk_dpa_encap_t                 inBand_rule;
    int                             vp_id = *vp_index_ptr;
    int                             port = *port_ptr;
    int                             ret = CPARSER_NOT_OK;

    inBand_rule.diffserv            = 0x25;
    inBand_rule.identification      = 0x7799;
    inBand_rule.outer_dip           = 0xc0A801c8;
    inBand_rule.outer_sip           = 0xc0A80132;
    inBand_rule.outer_udp_destPort  = 2152;
    inBand_rule.outer_udp_srcPort   = 2152;
    inBand_rule.teid                = 0x3333;
    inBand_rule.ttl                 = 0x2;
    inBand_rule.gtp_flag            = 0x32;
    inBand_rule.gtp_msgType         = 0xff;


    ret = rtk_gtp_fpga_inBand_config(&inBand_rule, vp_id, port);
    if(ret == CPARSER_OK)
    {
        DIAG_UTIL_MPRINTF("\nrtk_gtp_fpga_inBand_config()!!!\n");
    }

    return ret;

}
#endif

#ifdef CMD_MECDPA_TABLE_APP_LIST_COUNTER_MATCH_UDP_PORT_SRC_PORT
// mecdpa table app_list counter match upd_dest_port <UINT:upd_destPort> phy_src_port <UINT:src_port>
cparser_result_t cparser_cmd_mecdpa_table_app_list_counter_match_upd_dest_port_upd_destPort_phy_src_port_src_port(cparser_context_t *context,
    uint32_t *upd_destPort_ptr,
    uint32_t *src_port_ptr)
{
    rtk_dpa_app_list_match_t      *rule_match_field;
    int                             match_num = 1;
    int                             ret = CPARSER_NOT_OK;
    uint64_t                        byteCounter = 10, packetCounter = 10;

    /* Fill match fields */
    rule_match_field = (rtk_dpa_app_list_match_t *) malloc((sizeof(rtk_dpa_app_list_match_t)) * match_num);
    rule_match_field[0].type = RTK_DPA_APP_LIST_TYPE_OUT_L4DP;
    rule_match_field[0].data.outer_L4port = *upd_destPort_ptr;
    rule_match_field[0].mask.outer_L4port = UINT32_FULL_MASK;
#if 0
    rule_match_field[1].type = RTK_DPA_TEID_LEARN_TYPE_SRC_PORT;
    rule_match_field[1].data.src_port = (uint8)*src_port_ptr;
    rule_match_field[1].mask.src_port = UINT8_FULL_MASK;
#endif
    ret = rtk_dpa_ueUplinkCount_get(rule_match_field, match_num, &packetCounter, &byteCounter);
    if(ret == CPARSER_OK)
    {
        printf("\npacketCounter = %lld; byteCounter = %lld !!!\n",packetCounter,byteCounter);
    }else{
        DIAG_UTIL_MPRINTF("\nGet Counter action failed!!!\n");
    }
    return ret;
}
#endif

