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
 * Purpose : Definition those mirror command and APIs in the SDK diagnostic shell.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) port mirror
 *           2) rspan
 *           3) sflow
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
#include <rtk/mirror.h>
#include <diag_util.h>
#include <diag_om.h>
#include <diag_str.h>
#include <parser/cparser_priv.h>

#ifdef CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE
  #include <rtrpc/rtrpc_switch.h>
  #include <rtrpc/rtrpc_port.h>
  #include <rtrpc/rtrpc_mirror.h>
#endif

#ifdef CMD_MIRROR_GET_MIRROR_ID_INDEX
/*
 * mirror get mirror-id <UINT:index>
 */
cparser_result_t cparser_cmd_mirror_get_mirror_id_index(cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32          unit = 0;
    uint32          index = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_mirror_entry_t  mirror_entry;
    char            rxPortList[DIAG_UTIL_PORT_MASK_STRING_LEN];
    char            txPortList[DIAG_UTIL_PORT_MASK_STRING_LEN];

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&mirror_entry, 0, sizeof(rtk_mirror_entry_t));
    index = *index_ptr;
    DIAG_UTIL_ERR_CHK(rtk_mirror_group_get(unit, index, &mirror_entry), ret);

    diag_util_mprintf("Mirror Index : %d\n", index);

  #if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380)
    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID) ||
        DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
        {
            diag_util_mprintf("\tMirror Enable : %s\n", (mirror_entry.mirror_enable== ENABLED)?DIAG_STR_ENABLE:DIAG_STR_DISABLE);
            diag_util_mprintf("\tMirroring Port         : %d\n", mirror_entry.mirroring_port);
        }
  #endif

  #if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID) )
    {
        /* Mirror Type */
        if(mirror_entry.mirror_type == DISABLE_MIRROR)
        {
            diag_util_mprintf("\tMirror Type : %s\n", DIAG_STR_DISABLE);
        }
        else if(mirror_entry.mirror_type == PORT_BASED_MIRROR)
        {
            diag_util_mprintf("\tMirror Type : %s\n", "Port-Based Mirror");
        }
        else if(mirror_entry.mirror_type == RSPAN_BASED_MIRROR)
        {
            diag_util_mprintf("\tMirror Type : %s\n", "RSPAN Mirror");
        }
        else
        {
            diag_util_mprintf("\tMirror Type : %s\n", "Flow-Based Mirror");
        }

        /* Mirror Port Type */
        if(mirror_entry.mtp_type == MTP_TYPE_IS_TRK)
        {
            diag_util_mprintf("\tMTP Port Type : %s\n", "Trunk Port");
            diag_util_mprintf("\tTrunk ID         : %d\n", mirror_entry.mirroring_port);
        }
        else
        {
            diag_util_mprintf("\tMTP Port Type : %s\n", "Not Trunk Port");
            diag_util_mprintf("\tUnit ID         : %d\n", mirror_entry.mirroring_devID);
            diag_util_mprintf("\tPort ID         : %d\n", mirror_entry.mirroring_port);
        }
     }
  #endif

    osal_memset(rxPortList, 0, sizeof(rxPortList));
    osal_memset(txPortList, 0, sizeof(txPortList));

    diag_util_lPortMask2str(rxPortList, &mirror_entry.mirrored_igrPorts);
    diag_util_mprintf("\tMirroring Ingress Port : %s\n", rxPortList);
    diag_util_lPortMask2str(txPortList, &mirror_entry.mirrored_egrPorts);
    diag_util_mprintf("\tMirroring Egress Port  : %s\n", txPortList);
    diag_util_mprintf("\tOper of Igr & Egr Port : %s\n", (mirror_entry.oper_of_igr_and_egr_ports == MIRROR_OP_OR)?"OR":"AND");
    diag_util_mprintf("\tMirror Original Packet : %s\n", (mirror_entry.mirror_orginalPkt == ENABLED)?DIAG_STR_ENABLE:DIAG_STR_DISABLE);


  #if defined(CONFIG_SDK_RTL8380)  || defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
  if (DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID) ||
      DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
  {
      diag_util_mprintf("\tMTP Self Filter        : %s\n", (mirror_entry.self_flter == ENABLED)?DIAG_STR_ENABLE:DIAG_STR_DISABLE);
      diag_util_mprintf("\tMirroring Port TX Packets : %s\n", (mirror_entry.mirroring_port_egrMode == FORWARD_MIRRORED_PKTS_ONLY)?"Mirrored Packets Only":"All Packets");
      diag_util_mprintf("\tDuplicate Filter       : %s\n", (mirror_entry.duplicate_fltr == ENABLED)?DIAG_STR_ENABLE:DIAG_STR_DISABLE);
      diag_util_mprintf("\tMirror VLAN Mode       : %s\n", (mirror_entry.mir_mode == 1)?"Follow MTP VLAN Configuration":"Depend On Mirrored Packet");
  }
  #endif

  #if defined(CONFIG_SDK_RTL8380)
  if (DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
  {
      diag_util_mprintf("\tMirror Cross vlan             : %s\n", (mirror_entry.cross_vlan == ENABLED)?DIAG_STR_ENABLE:DIAG_STR_DISABLE);
      diag_util_mprintf("\tMirror Flow-based Only : %s\n", (mirror_entry.flowBasedOnly == ENABLED)?DIAG_STR_ENABLE:DIAG_STR_DISABLE);
      diag_util_mprintf("\tFlow-based Mirror Portmask Ignore    : %s\n", (mirror_entry.flowBased_pmsk_ignore == ENABLED)?DIAG_STR_ENABLE:DIAG_STR_DISABLE);
      diag_util_mprintf("\tMirror QID Enable      : %s\n", (mirror_entry.mir_qid_en == ENABLED)?DIAG_STR_ENABLE:DIAG_STR_DISABLE);
      diag_util_mprintf("\tMirror QID             : %u\n", mirror_entry.mir_qid);
  }
  #endif

    return CPARSER_OK;
} /* end of cparser_cmd_mirror_get_mirror_id_index */
#endif

#ifdef CMD_MIRROR_SET_MIRROR_ID_INDEX_MIRRORING_PORT_INGRESS_MIRRORED_INGRESS_PORTS_NONE_EGRESS_MIRRORED_EGRESS_PORTS_NONE_OPTION_90_80_IGR_AND_EGR_MIRRORED_ONLY_ORIGINAL_PKT
/*
 *  mirror set mirror-id <UINT:index> mirroring <UINT:port> ingress-mirrored ( <PORT_LIST:ingress_ports> | none ) egress-mirrored ( <PORT_LIST:egress_ports> | none ) option-90-80 { igr-and-egr } { mirrored-only } { original-pkt }
 */
cparser_result_t cparser_cmd_mirror_set_mirror_id_index_mirroring_port_ingress_mirrored_ingress_ports_none_egress_mirrored_egress_ports_none_option_90_80_igr_and_egr_mirrored_only_original_pkt(cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *port_ptr,
    char **ingress_ports_ptr,
    char **egress_ports_ptr)
{
    uint32          unit = 0;
    uint32          index = 0, i;
    int32           ret = RT_ERR_FAILED;
    rtk_mirror_entry_t  mirror_entry;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&mirror_entry, 0, sizeof(rtk_mirror_entry_t));

    index = *index_ptr;
    rtk_mirror_group_get(unit, index, &mirror_entry);

    mirror_entry.mirroring_port = *port_ptr;

    if ('n' != TOKEN_CHAR(7,0))
        DIAG_UTIL_ERR_CHK(diag_util_str2LPortMask(TOKEN_STR(7), &mirror_entry.mirrored_igrPorts), ret);

    if ('n' != TOKEN_CHAR(9,0))
        DIAG_UTIL_ERR_CHK(diag_util_str2LPortMask(TOKEN_STR(9), &mirror_entry.mirrored_egrPorts), ret);

    mirror_entry.oper_of_igr_and_egr_ports = MIRROR_OP_OR;
    mirror_entry.mirror_orginalPkt = DISABLED;
    mirror_entry.mirroring_port_egrMode = FORWARD_ALL_PKTS;

    for (i = 11; i < TOKEN_NUM; i++)
    {
        if ('i' == TOKEN_CHAR(i,0))
            mirror_entry.oper_of_igr_and_egr_ports = MIRROR_OP_AND;
        else if ('o' == TOKEN_CHAR(i,0))
            mirror_entry.mirror_orginalPkt = ENABLED;
        else if ('m' == TOKEN_CHAR(i,0))
            mirror_entry.mirroring_port_egrMode = FORWARD_MIRRORED_PKTS_ONLY;
        else
        {
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
        }
    }
    mirror_entry.mirror_enable = ENABLED;

    DIAG_UTIL_ERR_CHK(rtk_mirror_group_set(unit, index, &mirror_entry), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_MIRROR_SET_MIRROR_ID_INDEX_TYPE_PORT_BASED_RSPAN_BASED_FLOW_BASED_MIRRORING_DEV_PORT_INGRESS_MIRRORED_INGRESS_PORTS_NONE_EGRESS_MIRRORED_EGRESS_PORTS_NONE_OPTION_IGR_AND_EGR_MIRRORED_ONLY_ORIGINAL_PKT_DUPLICATE_FILTER_SELF_FILTER_MTP_VLAN
/*
 * mirror set mirror-id <UINT:index> type ( port-based | rspan-based | flow-based ) mirroring <UINT:devID> <UINT:port> ingress-mirrored ( <PORT_LIST:ingress_ports> | none ) egress-mirrored ( <PORT_LIST:egress_ports> | none ) option { igr-and-egr } { mirrored-only } { original-pkt } { duplicate-filter } { self-filter } { mtp-vlan } */
cparser_result_t
cparser_cmd_mirror_set_mirror_id_index_type_port_based_rspan_based_flow_based_mirroring_devID_port_ingress_mirrored_ingress_ports_none_egress_mirrored_egress_ports_none_option_igr_and_egr_mirrored_only_original_pkt_duplicate_filter_self_filter_mtp_vlan(
    cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *unit_ptr,
    uint32_t *port_ptr,
    char **ingress_ports_ptr,
    char **egress_ports_ptr)
{
    uint32          unit = 0;
    uint32          index = 0, i;
    int32           ret = RT_ERR_FAILED;
    rtk_mirror_entry_t  mirror_entry;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&mirror_entry, 0, sizeof(rtk_mirror_entry_t));

    index = *index_ptr;
    mirror_entry.mtp_type = MTP_TYPE_NOT_TRK;
    mirror_entry.mirroring_devID = *unit_ptr;
    mirror_entry.mirroring_port = *port_ptr;

    if ('p' == TOKEN_CHAR(5,0))
        mirror_entry.mirror_type = PORT_BASED_MIRROR;
    else if ('r' == TOKEN_CHAR(5,0))
        mirror_entry.mirror_type = RSPAN_BASED_MIRROR;
    else if ('f' == TOKEN_CHAR(5,0))
        mirror_entry.mirror_type = FLOW_BASED_MIRROR;
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    if ('n' != TOKEN_CHAR(10,0))
        DIAG_UTIL_ERR_CHK(diag_util_str2LPortMask(TOKEN_STR(10), &mirror_entry.mirrored_igrPorts), ret);

    if ('n' != TOKEN_CHAR(12,0))
        DIAG_UTIL_ERR_CHK(diag_util_str2LPortMask(TOKEN_STR(12), &mirror_entry.mirrored_egrPorts), ret);

    for (i = 14; i < TOKEN_NUM; i++)
    {
        if ('i' == TOKEN_CHAR(i,0))
            mirror_entry.oper_of_igr_and_egr_ports = MIRROR_OP_AND;
        else if ('o' == TOKEN_CHAR(i,0))
            mirror_entry.mirror_orginalPkt = ENABLED;
        else if ('m' == TOKEN_CHAR(i,0) && 'i' == TOKEN_CHAR(i,1))
            mirror_entry.mirroring_port_egrMode = FORWARD_MIRRORED_PKTS_ONLY;
        else if ('d' == TOKEN_CHAR(i,0))
            mirror_entry.duplicate_fltr = ENABLED;
        else if ('s' == TOKEN_CHAR(i,0))
            mirror_entry.self_flter = ENABLED;
        else if ('m' == TOKEN_CHAR(i,0) && 't' == TOKEN_CHAR(i,1))
            mirror_entry.mir_mode = MIRROR_VLAN_MODEL_SPECIAL;
        else
        {
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
        }
    }

    DIAG_UTIL_ERR_CHK(rtk_mirror_group_set(unit, index, &mirror_entry), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_mirror_set_mirror_id_index_type_port_based_rspan_based_flow_based_mirroring_dev_port_ingress_mirrored_ingress_ports_none_egress_mirrored_egress_ports_none_option_igr_and_egr_mirrored_only_original_pkt_duplicate_filter_self_filter_mtp_vlan */
#endif


#ifdef CMD_MIRROR_SET_MIRROR_ID_INDEX_TYPE_PORT_BASED_RSPAN_BASED_FLOW_BASED_MIRRORING_TRUNK_GROUP_TRUNK_ID_INGRESS_MIRRORED_INGRESS_PORTS_NONE_EGRESS_MIRRORED_EGRESS_PORTS_NONE_OPTION_IGR_AND_EGR_MIRRORED_ONLY_ORIGINAL_PKT_DUPLICATE_FILTER_SELF_FILTER_MTP_VLAN
/*
 * mirror set mirror-id <UINT:index> type ( port-based | rspan-based | flow-based ) mirroring trunk-group <UINT:trunk_id> ingress-mirrored ( <PORT_LIST:ingress_ports> | none ) egress-mirrored ( <PORT_LIST:egress_ports> | none ) option { igr-and-egr } { mirrored-only } { original-pkt } { duplicate-filter } { self-filter } { mtp-vlan } */
cparser_result_t
cparser_cmd_mirror_set_mirror_id_index_type_port_based_rspan_based_flow_based_mirroring_trunk_group_trunk_id_ingress_mirrored_ingress_ports_none_egress_mirrored_egress_ports_none_option_igr_and_egr_mirrored_only_original_pkt_duplicate_filter_self_filter_mtp_vlan(
    cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *trunk_id_ptr,
    char **ingress_ports_ptr,
    char **egress_ports_ptr)
{
    uint32          unit = 0;
    uint32          index = 0, i;
    int32           ret = RT_ERR_FAILED;
    rtk_mirror_entry_t  mirror_entry;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&mirror_entry, 0, sizeof(rtk_mirror_entry_t));

    index = *index_ptr;
    mirror_entry.mtp_type = MTP_TYPE_IS_TRK;
    mirror_entry.mirroring_devID = 0;
    mirror_entry.mirroring_port = *trunk_id_ptr;

    if ('p' == TOKEN_CHAR(5,0))
        mirror_entry.mirror_type = PORT_BASED_MIRROR;
    else if ('r' == TOKEN_CHAR(5,0))
        mirror_entry.mirror_type = RSPAN_BASED_MIRROR;
    else if ('f' == TOKEN_CHAR(5,0))
        mirror_entry.mirror_type = FLOW_BASED_MIRROR;
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    if ('n' != TOKEN_CHAR(10,0))
        DIAG_UTIL_ERR_CHK(diag_util_str2LPortMask(TOKEN_STR(10), &mirror_entry.mirrored_igrPorts), ret);

    if ('n' != TOKEN_CHAR(12,0))
        DIAG_UTIL_ERR_CHK(diag_util_str2LPortMask(TOKEN_STR(12), &mirror_entry.mirrored_egrPorts), ret);

    for (i = 14; i < TOKEN_NUM; i++)
    {
        if ('i' == TOKEN_CHAR(i,0))
            mirror_entry.oper_of_igr_and_egr_ports = MIRROR_OP_AND;
        else if ('o' == TOKEN_CHAR(i,0))
            mirror_entry.mirror_orginalPkt = ENABLED;
        else if ('m' == TOKEN_CHAR(i,0) && 'i' == TOKEN_CHAR(i,1))
            mirror_entry.mirroring_port_egrMode = FORWARD_MIRRORED_PKTS_ONLY;
        else if ('d' == TOKEN_CHAR(i,0))
            mirror_entry.duplicate_fltr = ENABLED;
        else if ('s' == TOKEN_CHAR(i,0))
            mirror_entry.self_flter = ENABLED;
        else if ('m' == TOKEN_CHAR(i,0) && 't' == TOKEN_CHAR(i,1))
            mirror_entry.mir_mode = MIRROR_VLAN_MODEL_SPECIAL;
        else
        {
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
        }
    }

    DIAG_UTIL_ERR_CHK(rtk_mirror_group_set(unit, index, &mirror_entry), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_mirror_set_mirror_id_index_type_port_based_rspan_based_flow_based_mirroring_trunk_group_trunk_id_ingress_mirrored_ingress_ports_none_egress_mirrored_egress_ports_none_option_igr_and_egr_mirrored_only_original_pkt_duplicate_filter_self_filter_mtp_vlan */
#endif

#ifdef CMD_MIRROR_SET_MIRROR_ID_INDEX_STATE_DISABLE_ENABLE
/*
 * mirror set mirror-id <UINT:index> state ( disable | enable )
 */
cparser_result_t cparser_cmd_mirror_set_mirror_id_index_state_disable_enable(cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32          unit = 0;
    uint32          index = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_mirror_entry_t  mirror_entry;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    index = *index_ptr;

    osal_memset(&mirror_entry, 0, sizeof(mirror_entry));

    DIAG_UTIL_ERR_CHK(rtk_mirror_group_get(unit, index, &mirror_entry), ret);

  #if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380)
    if (DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID)
         || DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
        if ('e' == TOKEN_CHAR(5,0))
        {
             mirror_entry.mirror_enable = ENABLED;
        }
        else
        {
             mirror_entry.mirror_enable = DISABLED;
        }
    }
  #endif
  #if defined(CONFIG_SDK_RTL9310) || defined(CONFIG_SDK_RTL9300)
    {
        if ('e' == TOKEN_CHAR(5,0))
            return RT_ERR_CHIP_NOT_SUPPORTED;
        else
        {
            mirror_entry.mirror_type = DISABLE_MIRROR;
        }
    }
  #endif

    DIAG_UTIL_ERR_CHK(rtk_mirror_group_set(unit, index, &mirror_entry), ret);
    return CPARSER_OK;
}
#endif

#ifdef CMD_MIRROR_GET_MIRROR_ID_INDEX_STATE
/*
 * mirror get mirror-id <UINT:index> state
 */
cparser_result_t cparser_cmd_mirror_get_mirror_id_index_state(cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    uint32  index = 0;
    rtk_mirror_entry_t  mirror_entry;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    index = *index_ptr;

    osal_memset(&mirror_entry, 0, sizeof(mirror_entry));

    DIAG_UTIL_ERR_CHK(rtk_mirror_group_get(unit, index, &mirror_entry), ret);

    diag_util_mprintf("Mirror Index : %d\n", index);

  #if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380)
    if (DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID)
         || DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
        diag_util_mprintf("\tMirror Enable : %s\n", (mirror_entry.mirror_enable== ENABLED)?DIAG_STR_ENABLE:DIAG_STR_DISABLE);
    }
  #else
    {
        diag_util_mprintf("\tMirror Enable : %s\n", (mirror_entry.mirror_type != DISABLE_MIRROR)?DIAG_STR_ENABLE:DIAG_STR_DISABLE);
    }
  #endif

    return CPARSER_OK;
}
#endif


#ifdef CMD_MIRROR_SET_MIRROR_ID_INDEX_MIRROR_MODE_MIRROR_QID_MIRROR_QID_EN_CROSS_VLAN_FLOW_BASED_ONLY_DUPLICATE_FILTER_SELF_FILTER_FLOW_BASED_PMSK_IGNORE
/*
 * mirror set mirror-id <UINT:index> <UINT:mirror-mode> <UINT:mirror-qid> { mirror-qid-en } { cross-vlan } { flow-based-only } { duplicate-filter } { self-filter } { flow-based-pmsk-ignore }
 */
cparser_result_t cparser_cmd_mirror_set_mirror_id_index_mirror_mode_mirror_qid_mirror_qid_en_cross_vlan_flow_based_only_duplicate_filter_self_filter_flow_based_pmsk_ignore(cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *mirror_mode_ptr,
    uint32_t *mirror_qid_ptr)
{
    uint32          unit = 0;
    uint32          index = 0;
    int32           i;
    int32           ret = RT_ERR_FAILED;
    rtk_mirror_entry_t  mirror_entry;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&mirror_entry, 0, sizeof(rtk_mirror_entry_t));

    index = *index_ptr;

    rtk_mirror_group_get(unit, index, &mirror_entry);

    mirror_entry.cross_vlan = 0;
    mirror_entry.flowBasedOnly = 0;
    mirror_entry.duplicate_fltr = 0;
    mirror_entry.self_flter = 0;
    mirror_entry.flowBased_pmsk_ignore = 0;
    mirror_entry.mir_qid_en = 0;

    mirror_entry.mir_mode = *mirror_mode_ptr;
    mirror_entry.mir_qid = *mirror_qid_ptr;

    for (i = 6; i < TOKEN_NUM; i++)
    {
        if ('f' == TOKEN_CHAR(i,0))
        {
            if (0 == strcmp(context->parser->tokens[i].buf, "flow-based-only"))
                mirror_entry.flowBasedOnly = ENABLED;
            else if (0 == strcmp(context->parser->tokens[i].buf, "flow-based-pmsk-ignore"))
                mirror_entry.flowBased_pmsk_ignore = ENABLED;
            else
            {
                diag_util_printf("User config: Error!\n");
                return CPARSER_NOT_OK;
            }
        }
        else if ('m' == TOKEN_CHAR(i,0))
            mirror_entry.mir_qid_en = ENABLED;
        else if ('c' == TOKEN_CHAR(i,0))
            mirror_entry.cross_vlan = ENABLED;
        else if ('d' == TOKEN_CHAR(i,0))
            mirror_entry.duplicate_fltr = ENABLED;
        else if ('s' == TOKEN_CHAR(i,0))
            mirror_entry.self_flter = ENABLED;
        else
        {
            diag_util_printf("User config: Error!\n");
            return CPARSER_NOT_OK;
        }
    }

    DIAG_UTIL_ERR_CHK(rtk_mirror_group_set(unit, index, &mirror_entry), ret);
    return CPARSER_OK;
} /* end of cparser_cmd_mirror_set_index_mirroring_port_id_ingress_mirrored_ingress_port_none_egress_mirrored_egress_port_none_igr_and_egr_cross_vlan_ucast_mcast_bcast_good_pkt_bad_pkt_original_pkt_flow_based_only */
#endif

#ifdef CMD_MIRROR_SET_QUEUE_ID_QID_STATE_DISABLE_ENABLE
/*
 * mirror set queue-id <UINT:qid> state ( disable | enable )
 */
cparser_result_t
cparser_cmd_mirror_set_queue_id_qid_state_disable_enable(
    cparser_context_t *context,
    uint32_t *qid_ptr)
{
    uint32          unit = 0;
    rtk_enable_t enable;
    int32           ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('e' == TOKEN_CHAR(5,0))
    {
        enable = ENABLED;
    }
    else
    {
        enable = DISABLED;
    }

    DIAG_UTIL_ERR_CHK(rtk_mirror_egrQueue_set(unit, enable, *qid_ptr), ret);
    return CPARSER_OK;
}   /* end of cparser_cmd_mirror_set_queue_id_qid_state_disable_enable */
#endif

#ifdef CMD_MIRROR_GET_QUEUE_ID
/*
 * mirror get queue-id
 */
cparser_result_t
cparser_cmd_mirror_get_queue_id(
    cparser_context_t *context)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_enable_t enable;
    rtk_qid_t qid;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_mirror_egrQueue_get(unit, &enable, &qid), ret);

    diag_util_mprintf("Mirror Egress Queue : %d State: %s\n", qid, (enable== ENABLED)?DIAG_STR_ENABLE:DIAG_STR_DISABLE);

    return CPARSER_OK;
}   /* end of cparser_cmd_mirror_get_queue_id */
#endif

#ifdef CMD_RSPAN_GET_EGRESS_MODE_MIRROR_ID_INDEX
cparser_result_t cparser_cmd_rspan_get_egress_mode_mirror_id_index(cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_mirror_rspanEgrMode_t   mode;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_mirror_rspanEgrMode_get(unit, *index_ptr, &mode), ret);
    if (RSPAN_EGR_REMOVE_TAG == mode)
        diag_util_mprintf("Mirror Entry %d : remove tag\n", *index_ptr);
    else if (RSPAN_EGR_ADD_TAG == mode)
        diag_util_mprintf("Mirror Entry %d : add tag\n", *index_ptr);
    else
        diag_util_mprintf("Mirror Entry %d : no modify\n", *index_ptr);

    return CPARSER_OK;
}
#endif

#ifdef CMD_RSPAN_SET_EGRESS_MODE_MIRROR_ID_INDEX_ADD_TAG_NO_MODIFY_REMOVE_TAG
cparser_result_t cparser_cmd_rspan_set_egress_mode_mirror_id_index_add_tag_no_modify_remove_tag(cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_mirror_rspanEgrMode_t   mode;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('a' == TOKEN_CHAR(5,0))
        mode = RSPAN_EGR_ADD_TAG;
    else if ('n' == TOKEN_CHAR(5,0))
        mode = RSPAN_EGR_NO_MODIFY;
    else if ('r' == TOKEN_CHAR(5,0))
        mode = RSPAN_EGR_REMOVE_TAG;
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_mirror_rspanEgrMode_set(unit, *index_ptr, mode), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_RSPAN_GET_INGRESS_MODE_MIRROR_ID_INDEX
cparser_result_t cparser_cmd_rspan_get_ingress_mode_mirror_id_index(cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_mirror_rspanIgrMode_t   mode;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_mirror_rspanIgrMode_get(unit, *index_ptr, &mode), ret);
    if (RSPAN_IGR_HANDLE_RSPAN_TAG == mode)
        diag_util_mprintf("Mirror Entry %d : handle rspan tag\n", *index_ptr);
    else
        diag_util_mprintf("Mirror Entry %d : ignore rspan tag\n", *index_ptr);

    return CPARSER_OK;
}
#endif

#ifdef CMD_RSPAN_SET_INGRESS_MODE_MIRROR_ID_INDEX_HANDLE_TAG_IGNORE_TAG
cparser_result_t cparser_cmd_rspan_set_ingress_mode_mirror_id_index_handle_tag_ignore_tag(cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_mirror_rspanIgrMode_t   mode;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('h' == TOKEN_CHAR(5,0))
        mode = RSPAN_IGR_HANDLE_RSPAN_TAG;
    else if ('i' == TOKEN_CHAR(5,0))
        mode = RSPAN_IGR_IGNORE_RSPAN_TAG;
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;

    }

    DIAG_UTIL_ERR_CHK(rtk_mirror_rspanIgrMode_set(unit, *index_ptr, mode), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_RSPAN_GET_TAG_MIRROR_ID_INDEX
cparser_result_t cparser_cmd_rspan_get_tag_mirror_id_index(cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_mirror_rspanTag_t   tag;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_mirror_rspanTag_get(unit, *index_ptr, &tag), ret);
    diag_util_mprintf("Mirror Entry %d :\n", *index_ptr);
#if defined(CONFIG_SDK_RTL8380) ||defined(CONFIG_SDK_RTL9310) || defined(CONFIG_SDK_RTL9300)
    if (DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID) ||
        DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID) ||DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID))
    {
        diag_util_mprintf("\tTPID : 0x%x\n", tag.tpid);
    }
#endif
#if defined(CONFIG_SDK_RTL8390)
    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
        diag_util_mprintf("\tTPID IDX: 0x%x\n", tag.tpidIdx);
    }
#endif
    diag_util_mprintf("\tVID  : %d\n", tag.vid);
    diag_util_mprintf("\tPRI  : %d\n", tag.pri);
    diag_util_mprintf("\tCFI  : %d\n", tag.cfi);

    return CPARSER_OK;
}
#endif

#ifdef CMD_RSPAN_SET_TAG_MIRROR_ID_INDEX_TPID_IDX_VID_PRI_CFI
cparser_result_t cparser_cmd_rspan_set_tag_mirror_id_index_tpid_idx_vid_pri_cfi(cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *tpid_idx_ptr,
    uint32_t *vid_ptr,
    uint32_t *pri_ptr,
    uint32_t *cfi_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_mirror_rspanTag_t   tag;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    tag.tpidIdx = *tpid_idx_ptr;
    tag.vid = *vid_ptr;
    tag.pri = *pri_ptr;
    tag.cfi = *cfi_ptr;

    DIAG_UTIL_ERR_CHK(rtk_mirror_rspanTag_set(unit, *index_ptr, &tag), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_RSPAN_SET_TAG_MIRROR_ID_INDEX_TPID_VID_PRI_CFI
cparser_result_t cparser_cmd_rspan_set_tag_mirror_id_index_tpid_vid_pri_cfi(cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *tpid_ptr,
    uint32_t *vid_ptr,
    uint32_t *pri_ptr,
    uint32_t *cfi_ptr)
{
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    rtk_mirror_rspanTag_t   tag;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    tag.tpid = *tpid_ptr;
    tag.vid = *vid_ptr;
    tag.pri = *pri_ptr;
    tag.cfi = *cfi_ptr;

    DIAG_UTIL_ERR_CHK(rtk_mirror_rspanTag_set(unit, *index_ptr, &tag), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_SFLOW_GET_MIRROR_GROUP_INDEX
/*
 * sflow get mirror-group <UINT:index>
 */
cparser_result_t cparser_cmd_sflow_get_mirror_group_index(cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32          unit = 0;
    uint32          index = 0;
    uint32          rate = 0;
    int32           ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    index = *index_ptr;

    DIAG_UTIL_ERR_CHK(rtk_mirror_sflowMirrorSampleRate_get(unit, index, &rate), ret);
    diag_util_mprintf("Mirror Index %2d :\n", index);
    diag_util_mprintf("\tSample Rate   : %d (0x%x)\n", rate, rate);

    return CPARSER_OK;
} /* end of cparser_cmd_sflow_get_mirror_group_index */
#endif

#ifdef CMD_SFLOW_SET_MIRROR_GROUP_INDEX_SAMPLE_RATE
/*
 * sflow set mirror-group <UINT:index> sample <UINT:rate>
 */
cparser_result_t cparser_cmd_sflow_set_mirror_group_index_sample_rate(cparser_context_t *context,
    uint32_t *index_ptr, uint32_t *rate_ptr)
{
    uint32          unit = 0;
    uint32          index = 0, rate = 0;
    int32           ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    index = *index_ptr;
    rate = *rate_ptr;
    DIAG_UTIL_ERR_CHK(rtk_mirror_sflowMirrorSampleRate_set(unit, index, rate), ret);
    return CPARSER_OK;
} /* end of cparser_cmd_sflow_set_mirror_group_index_sample_rate */
#endif

#ifdef CMD_SFLOW_GET_SAMPLE_CONTROL
/*
 * sflow get sample control
 */
cparser_result_t
cparser_cmd_sflow_get_sample_control(cparser_context_t *context)
{
    uint32                  unit = 0;
    int32                   ret = RT_ERR_FAILED;
    rtk_sflowSampleCtrl_t   ctrl;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_mirror_sflowSampleCtrl_get(unit, &ctrl), ret);

    diag_util_mprintf("sFlow sample control : ");
    if (SFLOW_CTRL_INGRESS == ctrl)
        diag_util_mprintf("Ingress\n");
    else if (SFLOW_CTRL_EGRESS == ctrl)
        diag_util_mprintf("Egress\n");

    return CPARSER_OK;
}   /* end of cparser_cmd_sflow_get_sample_control */
#endif

#ifdef CMD_SFLOW_SET_SAMPLE_CONTROL_INGRESS_EGRESS
/*
 * sflow set sample control ( ingress | egress )
 */
cparser_result_t
cparser_cmd_sflow_set_sample_control_ingress_egress(cparser_context_t *context)
{
    uint32                  unit = 0;
    int32                   ret = RT_ERR_FAILED;
    rtk_sflowSampleCtrl_t   ctrl;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('i' == TOKEN_CHAR(4,0))
        ctrl = SFLOW_CTRL_INGRESS;
    else
        ctrl = SFLOW_CTRL_EGRESS;

    DIAG_UTIL_ERR_CHK(rtk_mirror_sflowSampleCtrl_set(unit, ctrl), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_sflow_set_sample_control_ingress_egress */
#endif

#ifdef CMD_SFLOW_GET_EGRESS_INGRESS_PORT_ALL
/*
 * sflow get ( egress | ingress ) ( <PORT_LIST:port> | all )
 */
cparser_result_t cparser_cmd_sflow_get_egress_ingress_port_all(cparser_context_t *context,
    char **port_ptr)
{
    uint32          unit = 0;
    uint32          rate = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_port_t      port = 0;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    DIAG_UTIL_OUTPUT_INIT();

    if ('e' == TOKEN_CHAR(2,0))
    {
        DIAG_UTIL_PORTMASK_SCAN(portlist, port)
        {
            diag_util_mprintf("Port %2d :\n", port);
            if (RT_ERR_OK == (ret = rtk_mirror_sflowPortEgrSampleRate_get(unit, port, &rate)))
            {
                diag_util_mprintf("\tEgress Sample Rate   : %d (0x%x)\n", rate, rate);
            }
            else
            {
                DIAG_ERR_PRINT(ret);
            }
        }
    }
    else
    {
        DIAG_UTIL_PORTMASK_SCAN(portlist, port)
        {
            diag_util_mprintf("Port %2d :\n", port);
            if (RT_ERR_OK == (ret = rtk_mirror_sflowPortIgrSampleRate_get(unit, port, &rate)))
            {
                diag_util_mprintf("\tIngress Sample Rate   : %d (0x%x)\n", rate, rate);
            }
            else
            {
                DIAG_ERR_PRINT(ret);
            }
        }
    }

    return CPARSER_OK;
} /* end of cparser_cmd_sflow_get_egress_ingress_port_all */
#endif

#ifdef CMD_SFLOW_SET_EGRESS_INGRESS_PORT_ALL_SAMPLE_RATE
/*
 * sflow set ( egress | ingress ) ( <PORT_LIST:port> | all ) sample <UINT:rate>
 */
cparser_result_t cparser_cmd_sflow_set_egress_ingress_port_all_sample_rate(cparser_context_t *context,
    char **port_ptr,
    uint32_t *rate_ptr)
{
    uint32          unit = 0;
    uint32          rate = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_port_t      port = 0;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    rate = *rate_ptr;

    if ('e' == TOKEN_CHAR(2,0))
    {
        DIAG_UTIL_PORTMASK_SCAN(portlist, port)
        {
            DIAG_UTIL_ERR_CHK(rtk_mirror_sflowPortEgrSampleRate_set(unit, port, rate), ret);
        }
    }
    else
    {
        DIAG_UTIL_PORTMASK_SCAN(portlist, port)
        {
            DIAG_UTIL_ERR_CHK(rtk_mirror_sflowPortIgrSampleRate_set(unit, port, rate), ret);
        }
    }

    return CPARSER_OK;
} /* end of cparser_cmd_sflow_set_egress_ingress_port_all_sample_rate */
#endif

#ifdef CMD_SFLOW_GET_SAMPLE_TARGET
/*
 * sflow get sample-target
 */
cparser_result_t
cparser_cmd_sflow_get_sample_target(
    cparser_context_t *context)
{
    rtk_sflow_sampleTarget_t    target;
    uint32                      unit;
    int32                       ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_mirror_sflowSampleTarget_get(unit, &target), ret);
    diag_util_mprintf("Sample target: ");
    if (RTK_SFLOW_SAMPLE_LOCAL == target)
        diag_util_mprintf("Local\n");
    else
        diag_util_mprintf("Master\n");

    return CPARSER_OK;
}   /* end of cparser_cmd_sflow_get_sample_target */
#endif

#ifdef CMD_SFLOW_SET_SAMPLE_TARGET_LOCAL_MASTER
/*
 * sflow set sample-target ( local | master )
 */
cparser_result_t
cparser_cmd_sflow_set_sample_target_local_master(
    cparser_context_t *context)
{
    rtk_sflow_sampleTarget_t    target;
    uint32                      unit;
    int32                       ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('l' == TOKEN_CHAR(3,0))
        target = RTK_SFLOW_SAMPLE_LOCAL;
    else
        target = RTK_SFLOW_SAMPLE_MASTER;

    DIAG_UTIL_ERR_CHK(rtk_mirror_sflowSampleTarget_set(unit, target), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_sflow_set_sample_target_local_master */
#endif



