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
 * Purpose : Definition those PIE command and APIs in the SDK diagnostic shell.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) pie command
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
#include <osal/memory.h>
#include <rtk/switch.h>
#include <rtk/port.h>
#include <rtk/pie.h>
#include <rtk/acl.h>
#include <diag_util.h>
#include <diag_om.h>
#include <diag_str.h>
#include <diag_pie.h>
#include <parser/cparser_priv.h>

#ifdef CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE
  #include <rtrpc/rtrpc_switch.h>
  #include <rtrpc/rtrpc_port.h>
  #include <rtrpc/rtrpc_pie.h>
  #include <rtrpc/rtrpc_acl.h>
#endif

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */

diag_pie_template_t diag_pie_template_list[] =
{
    {TMPLTE_FIELD_NONE,                 "None",                     "None",     "Unset template"},
    {TMPLTE_FIELD_SPM0,                 "spm0",                     "spm0",     "source portmask for port 0-15"},
    {TMPLTE_FIELD_SPM1,                 "spm1",                     "spm1",     "source portmask for port 16-31"},
    {TMPLTE_FIELD_DMAC0,                "dmac0",                    "dmac0",    "destination MAC [15:0"},
    {TMPLTE_FIELD_DMAC1,                "dmac1",                    "dmac1",    "destination MAC [31:16]"},
    {TMPLTE_FIELD_DMAC2,                "dmac2",                    "dmac2",    "destination MAC [47:32]"},
    {TMPLTE_FIELD_SMAC0,                "smac0",                    "smac0",    "source MAC [15:0]"},
    {TMPLTE_FIELD_SMAC1,                "smac1",                    "smac1",    "source MAC [31:16]"},
    {TMPLTE_FIELD_SMAC2,                "smac2",                    "smac2",    "source MAC [47:32]"},
    {TMPLTE_FIELD_ETHERTYPE,            "ethertype",                "ether",    "ethernet type"},
    {TMPLTE_FIELD_OTAG,                 "otag",                     "otag",     "outer tag"},
    {TMPLTE_FIELD_ITAG,                 "itag",                     "itag",     "inner tag"},
    {TMPLTE_FIELD_SIP0,                 "sip0",                     "sip0",     "IPv4 or IPv6 source IP[15:0] or ARP/RARP source protocol address in header"},
    {TMPLTE_FIELD_SIP1,                 "sip1",                     "sip1",     "IPv4 or IPv6 source IP[31:16] or ARP/RARP source protocol address in header"},
    {TMPLTE_FIELD_SIP2,                 "sip2",                     "sip2",     "IPv6 source IP[47:32]"},
    {TMPLTE_FIELD_SIP3,                 "sip3",                     "sip3",     "IPv6 source IP[63:48]"},
    {TMPLTE_FIELD_SIP4,                 "sip4",                     "sip4",     "IPv6 source IP[79:64]"},
    {TMPLTE_FIELD_SIP5,                 "sip5",                     "sip5",     "IPv6 source IP[95:80]"},
    {TMPLTE_FIELD_SIP6,                 "sip6",                     "sip6",     "IPv6 source IP[111:96]"},
    {TMPLTE_FIELD_SIP7,                 "sip7",                     "sip7",     "IPv6 source IP[127:112]"},
    {TMPLTE_FIELD_DIP0,                 "dip0",                     "dip0",     "IPv4 or IPv6 destination IP[15:0] or ARP/RARP destination protocol address in header"},
    {TMPLTE_FIELD_DIP1,                 "dip1",                     "dip1",     "IPv4 or IPv6 destination IP[31:16] or ARP/RARP destination protocol address in header"},
    {TMPLTE_FIELD_DIP2,                 "dip2",                     "dip2",     "IPv6 destination IP[47:32]"},
    {TMPLTE_FIELD_DIP3,                 "dip3",                     "dip3",     "IPv6 destination IP[63:48]"},
    {TMPLTE_FIELD_DIP4,                 "dip4",                     "dip4",     "IPv6 destination IP[79:64]"},
    {TMPLTE_FIELD_DIP5,                 "dip5",                     "dip5",     "IPv6 destination IP[95:80]"},
    {TMPLTE_FIELD_DIP6,                 "dip6",                     "dip6",     "IPv6 destination IP[111:96]"},
    {TMPLTE_FIELD_DIP7,                 "dip7",                     "dip7",     "IPv6 destination IP[127:112]"},
    {TMPLTE_FIELD_IP_TOS_PROTO,         "ip-tos-proto",             "IPTO",     "IPv4 TOS/IPv6 traffic class and IPv4 protocold/IPv6 next header"},
    {TMPLTE_FIELD_L4_SPORT,             "l4-sport",                 "L4SP",     "TCP/UDP source port"},
    {TMPLTE_FIELD_L4_DPORT,             "l4-dport",                 "L4DP",     "TCP/UDP destination port"},
    {TMPLTE_FIELD_L34_HEADER,           "l34-header",               "L34H",     "packet with extra tag and IPv6 with auth, dest, frag, route, hop-by-hop option header, IGMP type, TCP flag"},
    {TMPLTE_FIELD_FIELD_SELECTOR_VALID, "field_selector_valid_msk", "FSMSK",    "field selector valid mask"},
    {TMPLTE_FIELD_FIELD_SELECTOR_0,     "field_selector0",          "FS0",      "field selector 0"},
    {TMPLTE_FIELD_FIELD_SELECTOR_1,     "field_selector1",          "FS1",      "field selector 1"},
    {TMPLTE_FIELD_FIELD_SELECTOR_2,     "field_selector2",          "FS2",      "field selector 2"},
    {TMPLTE_FIELD_FIELD_SELECTOR_3,     "field_selector3",          "FS3",      "field selector 3"},
    {TMPLTE_FIELD_FWD_VID,              "fwd-vid",                  "FWVID",    "forwarding VID"},
#if defined(CONFIG_SDK_RTL8390)
    {TMPLTE_FIELD_IP_FLAG_OFFSET,       "ip-flag-offset",           "IPFO",     "IP flags, IP fragment offset, IPv6 DIP[63:48]"},
    {TMPLTE_FIELD_VID_RANG0,            "vid-range0",               "VIDR0",    "VID range check0"},
    {TMPLTE_FIELD_VID_RANG1,            "vid-range1",               "VIDR1",    "VID range check1"},
    {TMPLTE_FIELD_IP_L4PORT_RANG,       "ip-l4port-range",          "L4PR",     "IP address and TCP/UDP port range check"},
    {TMPLTE_FIELD_IP_LEN_RANG,          "ip-len-range",             "IPLEN",    "IP address and packet len range check"},
    {TMPLTE_FIELD_OLABEL,               "inner-label-lsb",          "InnLb",    "outer MPLS label[15:0]"},
    {TMPLTE_FIELD_ILABEL,               "outer-label-lsb",          "OutLb",    "inner MPLS label[15:0]"},
    {TMPLTE_FIELD_OILABEL,              "label_msb",                "LbMsb",    "inner/outer MPLS label[19:16], inner/outer exp"},
    {TMPLTE_FIELD_DPMMASK,              "dpmm",                     "dpmm",     "dest. portmask configuration result"},
    {TMPLTE_FIELD_L2DPM0,               "l2-dpm0",                  "L2Dp0",    "L2 looup result dest. portmask for port 0-15"},
    {TMPLTE_FIELD_L2DPM1,               "l2-dpm1",                  "L2Dp1",    "L2 looup result dest. portmask for port 16-31"},
    {TMPLTE_FIELD_L2DPM2,               "l2-dpm2",                  "L2Dp2",    "L2 looup result dest. portmask for port 32-47"},
    {TMPLTE_FIELD_L2DPM3,               "l2-dpm3",                  "L2Dp3",    "L2 looup result dest. portmask for port 48-63"},
    {TMPLTE_FIELD_IVLAN,                "inner-vlan",               "InnV",     "inner VALN given by ingress inner VLAN decision"},
    {TMPLTE_FIELD_OVLAN,                "outer-vlan",               "OutV",     "outer VALN given by ingress outer VLAN decision"},
#endif
#if defined(CONFIG_SDK_RTL8380)
    {TMPLTE_FIELD_IP_RANGE,             "ip-range",                 "IP-R",     "IPv4/v6 address range check and inner untag/priority-tag, outer untag/priority-tag, flow label MSB 4-bit"},
#endif
#if defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL8390)
    {TMPLTE_FIELD_SPMMASK,              "spmm",                     "spmm",     "soure portmask configuration result"},
    {TMPLTE_FIELD_ICMP_IGMP,            "icmp-igmp",                "ICMP",     "ICMP/ICMPv6 and IGMP info"},
#endif
#if defined(CONFIG_SDK_RTL9310) || defined(CONFIG_SDK_RTL8390)
    {TMPLTE_FIELD_SPM2,                 "spm2",                     "spm2",     "source portmask for port 32-47"},
    {TMPLTE_FIELD_SPM3,                 "spm3",                     "spm3",     "source portmask for port 48-52"},
    {TMPLTE_FIELD_DPM0,                 "dpm0",                     "dpm0",     "dest. portmask for port 0-15"},
    {TMPLTE_FIELD_DPM1,                 "dpm1",                     "dpm1",     "dest. portmask for port 16-31"},
    {TMPLTE_FIELD_DPM2,                 "dpm2",                     "dpm2",     "dest. portmask for port 32-47"},
    {TMPLTE_FIELD_DPM3,                 "dpm3",                     "dpm0",     "dest. portmask for port 48-63"},
#endif
#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310) || defined(CONFIG_SDK_RTL8380)
    {TMPLTE_FIELD_RANGE_CHK,            "range-check",              "R-CHK",    "layer4 port/VID/packet length/field selector range check"},
    {TMPLTE_FIELD_FLOW_LABEL,           "flow-label",               "FL",       "flow label LSB 15-bit"},
#endif
#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310) || defined(CONFIG_SDK_RTL8390)
    {TMPLTE_FIELD_FIELD_SELECTOR_4,     "field_selector4",          "FS4",      "field selector 4"},
    {TMPLTE_FIELD_FIELD_SELECTOR_5,     "field_selector5",          "FS5",      "field selector 5"},
    {TMPLTE_FIELD_FIELD_SELECTOR_6,     "field_selector6",          "FS6",      "field selector 6"},
    {TMPLTE_FIELD_FIELD_SELECTOR_7,     "field_selector7",          "FS7",      "field selector 7"},
    {TMPLTE_FIELD_FIELD_SELECTOR_8,     "field_selector8",          "FS8",      "field selector 8"},
    {TMPLTE_FIELD_FIELD_SELECTOR_9,     "field_selector9",          "FS9",      "field selector 9"},
    {TMPLTE_FIELD_FIELD_SELECTOR_10,    "field_selector10",         "FS10",     "field selector 10"},
    {TMPLTE_FIELD_FIELD_SELECTOR_11,    "field_selector11",         "FS11",     "field selector 11"},
#endif
#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    {TMPLTE_FIELD_VLAN,                 "vlan",                     "vlan",     "Inner or Outer VLAN"},
    {TMPLTE_FIELD_TCP_INFO,             "tcp-info",                 "TCP",      "TCP info"},
    {TMPLTE_FIELD_PKT_INFO,             "packet-info",              "PKT",      "Packet lookup info"},
    {TMPLTE_FIELD_DSAP_SSAP,            "dsap-ssap",                "Dsap",     "DSAP/SSAP for LLC/SNAP packet"},
    {TMPLTE_FIELD_SNAP_OUI,             "snap-oui",                 "SNAPo",    "OUI in SNAP header"},
    {TMPLTE_FIELD_VLAN_GMSK,            "vlan-group-mask",          "VGM",      "VLAN Group Mask/IP range check"},
    {TMPLTE_FIELD_DLP,                  "dlp",                      "dlp",      "Destination Logical Port"},
    {TMPLTE_FIELD_META_DATA,            "meta-data",                "META",     "meta data"},
    {TMPLTE_FIELD_SLP,                  "slp",                      "slp",      "source logic port"},
#endif
#if defined(CONFIG_SDK_RTL9300)
    {TMPLTE_FIELD_SRC_FWD_VID,          "src-fwd-vlan",             "SFV",      "VLAN before routing"},
#endif
#if defined(CONFIG_SDK_RTL9310)
    {TMPLTE_FIELD_FIELD_SELECTOR_12,    "field_selector12",         "FS12",     "field selector 12"},
    {TMPLTE_FIELD_FIELD_SELECTOR_13,    "field_selector13",         "FS13",     "field selector 13"},
    {TMPLTE_FIELD_TTID,                 "ttid",                     "ttid",     "tunnel terminate index"},
    {TMPLTE_FIELD_TSID,                 "tsid",                     "tsid",     "tunnel start index"},
    {TMPLTE_FIELD_FIRST_MPLS1,          "first-mpls1",              "mpls1",    "first mpls label 1"},
    {TMPLTE_FIELD_FIRST_MPLS2,          "first-mpls2",              "mpls2",    "first mpls label 2"},
#endif
    {TMPLTE_FIELD_FIX,                  "fix field",                "fix",      "fixed field"},
    {TMPLTE_FIELD_END,                  "null",                     "null",     "Not configure"},
};

/*
 * Macro Declaration
 */

/*
 * Function Declaration
 */
char* _diag_pie_phaseStr_get(rtk_pie_phase_t phase)
{
    switch (phase)
    {
        case PIE_PHASE_VACL:
            return "VLAN ACL";
        case PIE_PHASE_IACL:
            return "Ingress ACL";
        #if defined(CONFIG_SDK_RTL9310)
        case PIE_PHASE_EACL:
            return "Egress ACL";
        #endif  /* defined(CONFIG_SDK_RTL9310) */
        case PIE_PHASE_IGR_FLOW_TABLE_0:
            return "Ingress Flow Table 0\n";
        case PIE_PHASE_IGR_FLOW_TABLE_3:
            return "Ingress Flow Table 3\n";
        case PIE_PHASE_EGR_FLOW_TABLE_0:
            return "Egress Flow Table 0\n";
        default:
            break;
    }

    return "None";
}
#ifdef CMD_PIE_GET_BLOCK_INDEX_LOOKUP_STATE
/*
 * pie get block <UINT:index> lookup state
 */
cparser_result_t
cparser_cmd_pie_get_block_index_lookup_state(cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32          unit;
    int32           ret;
    rtk_enable_t    enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    ret = rtk_pie_blockLookupEnable_get(unit, *index_ptr, &enable);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("The block %d lookup state is ", *index_ptr);
    if (ENABLED == enable)
        diag_util_mprintf("%s\n",DIAG_STR_ENABLE);
    else
        diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

    return CPARSER_OK;
}
#endif

#ifdef CMD_PIE_SET_BLOCK_INDEX_LOOKUP_STATE_DISABLE_ENABLE
/*
 * pie set block <UINT:index> lookup state ( disable | enable )
 */
cparser_result_t
cparser_cmd_pie_set_block_index_lookup_state_disable_enable(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32          unit;
    int32           ret;
    rtk_enable_t    enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if('e' == TOKEN_CHAR(6, 0))
    {
        enable = ENABLED;
    }
    else
    {
        enable = DISABLED;
    }

    ret = rtk_pie_blockLookupEnable_set(unit, *index_ptr, enable);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_PIE_GET_BLOCK_INDEX_GROUP
/*
 * pie get block <UINT:index> group
 */
cparser_result_t
cparser_cmd_pie_get_block_index_group(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32  unit;
    uint32  grp, logic;
    int32   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    ret = rtk_pie_blockGrouping_get(unit, *index_ptr, &grp, &logic);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("Block %d\n", *index_ptr);
    diag_util_mprintf(" group %d\n", grp);
    diag_util_mprintf(" logic %d\n", logic);

    return CPARSER_OK;
}   /* end of cparser_cmd_pie_get_block_index_group */
#endif

#ifdef CMD_PIE_SET_BLOCK_INDEX_GROUP_GROUP_ID_GROUP_ID_LOGIC_ID_LOGIC_ID
/*
 * pie set block <UINT:index> group group_id <UINT:group_id> logic_id <UINT:logic_id>
 */
cparser_result_t
cparser_cmd_pie_set_block_index_group_group_id_group_id_logic_id_logic_id(
    cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *group_id_ptr,
    uint32_t *logic_id_ptr)
{
    uint32  unit;
    int32   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    ret = rtk_pie_blockGrouping_set(unit, *index_ptr, *group_id_ptr, *logic_id_ptr);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pie_set_block_index_group_group_id_group_id_logic_id_logic_id */
#endif

#ifdef CMD_PIE_GET_TEMPLATE_PHASE_PHASE_FIELD_LIST
rtk_pie_phase_t _diag_pie_phaseTrans(uint32 unit, uint32 inPhase)
{
    switch (inPhase)
    {
        case 0:
            return PIE_PHASE_VACL;
        case 1:
            return PIE_PHASE_IACL;
        #if defined(CONFIG_SDK_RTL9310)
        case 2:
            return PIE_PHASE_EACL;
        case 3:
            return PIE_PHASE_IGR_FLOW_TABLE_0;
        case 4:
            return PIE_PHASE_IGR_FLOW_TABLE_3;
        case 5:
            return PIE_PHASE_EGR_FLOW_TABLE_0;
        #endif  /* defined(CONFIG_SDK_RTL9310) */
        default:
            return PIE_PHASE_END;
    }

    return PIE_PHASE_END;
}

/*
 * pie get template phase <UINT:phase> field-list
 */
cparser_result_t
cparser_cmd_pie_get_template_phase_phase_field_list(
    cparser_context_t *context,
    uint32_t  *phase_ptr)
{
    rtk_pie_phase_t phase;
    uint32          unit, i;
    int32           ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    phase = _diag_pie_phaseTrans(unit, *phase_ptr);

    diag_util_mprintf("%-25s %s\n", "template field keyword", "desciption");
    diag_util_mprintf("=====================================================\n");

    for (i = 0; i < (sizeof(diag_pie_template_list)/sizeof(diag_pie_template_t)); ++i)
    {
        ret = rtk_pie_templateField_check(unit, phase,
                diag_pie_template_list[i].type);
        if (RT_ERR_OK != ret)
        {
            continue;
        }
        diag_util_mprintf("%-25s %s\n", diag_pie_template_list[i].user_info,
                diag_pie_template_list[i].desc);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pie_get_template_phase_phase_field_list */
#endif

#ifdef CMD_PIE_SET_TEMPLATE_TEMPLATE_INDEX_FIELD_INDEX_INDEX_FIELD_TYPE_FIELD_NAME
/*
 * pie set template <UINT:template_index> field_index <UINT:index> field_type <STRING:field_name>
 */
cparser_result_t
cparser_cmd_pie_set_template_template_index_field_index_index_field_type_field_name(
    cparser_context_t *context,
    uint32_t  *template_index_ptr,
    uint32_t  *index_ptr,
    char * *field_name_ptr)
{
    uint32              unit;
    uint32              num_element, i;
    int32               ret;
    rtk_pie_template_t  template_info;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if (*index_ptr >= RTK_MAX_NUM_OF_PIE_TEMPLATE_FIELD)
    {
        diag_util_printf("Invalid field index!\n");
        return CPARSER_NOT_OK;
    }

    num_element = (sizeof(diag_pie_template_list)/sizeof(diag_pie_template_t));

    for (i = 0; i < num_element; ++i)
    {
        if (!strcasecmp(*field_name_ptr, diag_pie_template_list[i].user_info))
            break;
    }

    if (num_element <= i)
    {
        diag_util_printf("Invalid field %s!\n", *field_name_ptr);
        return CPARSER_NOT_OK;
    }

    osal_memset(&template_info, 0, sizeof(rtk_pie_template_t));

    ret = rtk_pie_template_get(unit, *template_index_ptr, &template_info);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    template_info.field[*index_ptr] = diag_pie_template_list[i].type;

    ret = rtk_pie_template_set(unit, *template_index_ptr, &template_info);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pie_set_template_template_index_field_index_index_field_type_field_name */
#endif

#ifdef CMD_PIE_GET_TEMPLATE_INDEX
/*
 * pie get template <UINT:index>
 */
cparser_result_t
cparser_cmd_pie_get_template_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    uint32              unit;
    uint32              i, j, templ_field_num;
    int32               ret;
    rtk_pie_template_t  template_info;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&template_info, 0, sizeof(rtk_pie_template_t));

    ret = rtk_pie_template_get(unit, *index_ptr, &template_info);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("Template %d field information:\n", *index_ptr);

#if defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        templ_field_num = 14;
    }
    else
#endif
    {
        templ_field_num = 12;
    }

    for (j = 0; j < templ_field_num; ++j)
    {
        diag_util_mprintf("\tfield %d: ", j);


        for (i = 0; diag_pie_template_list[i].type != TMPLTE_FIELD_END; ++i)
        {
            if (template_info.field[j] == diag_pie_template_list[i].type)
            {
                diag_util_mprintf("%s", diag_pie_template_list[i].user_info);
                break;
            }
        }

        if (diag_pie_template_list[i].type == TMPLTE_FIELD_END)
            diag_util_mprintf("%d", template_info.field[j]);

        diag_util_mprintf("\n");
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pie_get_template_index */
#endif

#ifdef CMD_RANGE_CHECK_SET_IP_ENTRY_INDEX_LOW_BOUND_IP
/*
 * range-check set ip entry <UINT:index> low-bound <IPV4ADDR:ip>
 */
cparser_result_t
cparser_cmd_range_check_set_ip_entry_index_low_bound_ip(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *ip_ptr)
{
    uint32                          unit;
    int32                           ret;
    rtk_pie_rangeCheck_ip_t         ip_range;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&ip_range, 0, sizeof(rtk_pie_rangeCheck_ip_t));

    ret = rtk_pie_rangeCheckIp_get(unit, *index_ptr, &ip_range);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    ip_range.ip_lower_bound = *ip_ptr;
    ret = rtk_pie_rangeCheckIp_set(unit, *index_ptr, &ip_range);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_range_check_set_entry_index_ip_ip4_sip_ip4_dip_ip6_sip_ip6_dip_lower_upper */
#endif

#ifdef CMD_RANGE_CHECK_SET_IP_ENTRY_INDEX_UP_BOUND_IP
/*
 * range-check set ip entry <UINT:index> up-bound <IPV4ADDR:ip>
 */
cparser_result_t
cparser_cmd_range_check_set_ip_entry_index_up_bound_ip(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *ip_ptr)
{
    uint32                          unit;
    int32                           ret;
    rtk_pie_rangeCheck_ip_t         ip_range;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&ip_range, 0, sizeof(rtk_pie_rangeCheck_ip_t));

    ret = rtk_pie_rangeCheckIp_get(unit, *index_ptr, &ip_range);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    ip_range.ip_upper_bound = *ip_ptr;
    ret = rtk_pie_rangeCheckIp_set(unit, *index_ptr, &ip_range);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_range_check_set_entry_index_ip_ip4_sip_ip4_dip_ip6_sip_ip6_dip_lower_upper */
#endif

#ifdef CMD_RANGE_CHECK_SET_IP_ENTRY_INDEX_LOW_BOUND_LIP_UP_BOUND_UIP
/*
 * range-check set ip entry <UINT:index> low-bound <IPV4ADDR:lip> up-bound <IPV4ADDR:uip>
 */
cparser_result_t
cparser_cmd_range_check_set_ip_entry_index_low_bound_lip_up_bound_uip(
    cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *lip_ptr,
    uint32_t *uip_ptr)
{
    rtk_pie_rangeCheck_ip_t         ipRng;
    uint32                          unit;
    int32                           ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&ipRng, 0, sizeof(rtk_pie_rangeCheck_ip_t));

    ret = rtk_pie_rangeCheckIp_get(unit, *index_ptr, &ipRng);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    ipRng.ip_lower_bound = *lip_ptr;
    ipRng.ip_upper_bound = *uip_ptr;
    ret = rtk_pie_rangeCheckIp_set(unit, *index_ptr, &ipRng);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_range_check_set_ip_entry_index_low_bound_lip_up_bound_uip */
#endif


#ifdef CMD_RANGE_CHECK_SET_IP_ENTRY_INDEX_TYPE_SIP_DIP_SIP6_DIP6
/*
 * range-check set ip entry <UINT:index> type ( sip | dip | sip6 | dip6 )
 */
cparser_result_t
cparser_cmd_range_check_set_ip_entry_index_type_sip_dip_sip6_dip6(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    uint32                          unit;
    int32                           ret;
    rtk_pie_rangeCheck_ip_t         ip_range;
    rtk_pie_rangeCheck_ip_type_t    type;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&ip_range, 0, sizeof(rtk_pie_rangeCheck_ip_t));

    ret = rtk_pie_rangeCheckIp_get(unit, *index_ptr, &ip_range);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    if('6' == TOKEN_CHAR(6, 3))
    {
        if('s' == TOKEN_CHAR(6, 0))
            type = RNGCHK_IP_TYPE_IPV6_SRC;
        else
            type = RNGCHK_IP_TYPE_IPV6_DST;
    }
    else if('s' == TOKEN_CHAR(6, 0))
    {
        type = RNGCHK_IP_TYPE_IPV4_SRC;
    }
    else
    {
        type = RNGCHK_IP_TYPE_IPV4_DST;
    }

    ip_range.ip_type = type;
    ret = rtk_pie_rangeCheckIp_set(unit, *index_ptr, &ip_range);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_range_check_set_entry_index_ip_ip4_sip_ip4_dip_ip6_sip_ip6_dip_lower_upper */
#endif

#ifdef CMD_RANGE_CHECK_SET_IP_ENTRY_INDEX_TYPE_SIP6_SUFFIX_DIP6_SUFFIX
/*
 * range-check set ip entry <UINT:index> type ( sip6-suffix | dip6-suffix )
 */
cparser_result_t
cparser_cmd_range_check_set_ip_entry_index_type_sip6_suffix_dip6_suffix(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    uint32                          unit;
    int32                           ret;
    rtk_pie_rangeCheck_ip_t         ip_range;
    rtk_pie_rangeCheck_ip_type_t    type;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&ip_range, 0, sizeof(rtk_pie_rangeCheck_ip_t));

    ret = rtk_pie_rangeCheckIp_get(unit, *index_ptr, &ip_range);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    if('s' == TOKEN_CHAR(6, 0))
    {
        type = RNGCHK_IP_TYPE_IPV6_SRC_SUFFIX;
    }
    else
    {
        type = RNGCHK_IP_TYPE_IPV6_DST_SUFFIX;
    }

    ip_range.ip_type = type;
    ret = rtk_pie_rangeCheckIp_set(unit, *index_ptr, &ip_range);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_range_check_set_entry_index_ip_ip4_sip_ip4_dip_ip6_sip_ip6_dip_lower_upper */
#endif

#ifdef CMD_RANGE_CHECK_SET_IP_ENTRY_INDEX_REVERSE_DISABLE_ENABLE
/*
 * range-check set ip entry <UINT:index> reverse ( disable | enable )
 */
cparser_result_t
cparser_cmd_range_check_set_ip_entry_index_reverse_disable_enable(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    uint32                  unit;
    int32                   ret;
    rtk_pie_rangeCheck_ip_t ip_range;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&ip_range, 0, sizeof(rtk_pie_rangeCheck_ip_t));

    ret = rtk_pie_rangeCheckIp_get(unit, *index_ptr, &ip_range);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    if('e' == TOKEN_CHAR(6, 0))
    {
        ip_range.reverse = 1;
    }
    else
    {
        ip_range.reverse = 0;
    }

    ret = rtk_pie_rangeCheckIp_set(unit, *index_ptr, &ip_range);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_range_check_set_entry_index_ip_reverse_state_disable_enable */
#endif

#ifdef CMD_RANGE_CHECK_GET_IP_ENTRY_INDEX
/*
 * range-check get ip entry <UINT:index>
 */
cparser_result_t
cparser_cmd_range_check_get_ip_entry_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    uint32                  unit;
    int32                   ret;
    rtk_pie_rangeCheck_ip_t ip_range;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&ip_range, 0, sizeof(rtk_pie_rangeCheck_ip_t));

    ret = rtk_pie_rangeCheckIp_get(unit, *index_ptr, &ip_range);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("IP range check index %d:\n", *index_ptr);

    diag_util_mprintf("\tType: ");
    switch (ip_range.ip_type)
    {
        case RNGCHK_IP_TYPE_IPV4_SRC:
            diag_util_mprintf("IPv4 source IP");
            break;
        case RNGCHK_IP_TYPE_IPV4_DST:
            diag_util_mprintf("IPv4 destination IP");
            break;
        case RNGCHK_IP_TYPE_IPV6_SRC:
            diag_util_mprintf("IPv6 source IP");
            break;
        case RNGCHK_IP_TYPE_IPV6_DST:
            diag_util_mprintf("IPv6 destination IP");
            break;
        case RNGCHK_IP_TYPE_IPV6_SRC_SUFFIX:
            diag_util_mprintf("IPv6 suffix source IP");
            break;
        case RNGCHK_IP_TYPE_IPV6_DST_SUFFIX:
            diag_util_mprintf("IPv6 suffix destination IP");
            break;
        default:
            return CPARSER_NOT_OK;
    }

    diag_util_mprintf("\n");

    diag_util_mprintf("\tLower: 0x%x\n", ip_range.ip_lower_bound);
    diag_util_mprintf("\tUpper: 0x%x\n", ip_range.ip_upper_bound);

#if defined(CONFIG_SDK_RTL8390)
    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
        diag_util_mprintf("\tReverse: ");
        if (0 == ip_range.reverse)
        {
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);
        }
        else
        {
            diag_util_mprintf("%s\n",DIAG_STR_ENABLE);
        }
    }
#endif

    return CPARSER_OK;
}   /* end of cparser_cmd_range_check_get_entry_index_ip */
#endif

#ifdef CMD_FIELD_SELECTOR_SET_INDEX_INDEX_FORMAT_RAW_L2_L3_L4_OFFSET_OFFSET
/*
 * field-selector set index <UINT:index> format ( raw | l2 | l3  | l4 ) offset <UINT:offset>
 */
cparser_result_t
cparser_cmd_field_selector_set_index_index_format_raw_l2_l3_l4_offset_offset(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *offset_ptr)
{
    uint32                          unit;
    int32                           ret;
    rtk_pie_fieldSelector_data_t    fs;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if (0 == osal_strcmp(TOKEN_STR(5), "raw"))
    {
        fs.start = FS_START_POS_RAW;
    }
    else if (0 == osal_strcmp(TOKEN_STR(5), "l2"))
    {
        fs.start = FS_START_POS_L2;
    }
    else if (0 == osal_strcmp(TOKEN_STR(5), "l3"))
    {
        fs.start = FS_START_POS_L3;
    }
    else
    {
        fs.start = FS_START_POS_L4;
    }

    fs.offset = *offset_ptr;
    ret = rtk_pie_fieldSelector_set(unit, *index_ptr, &fs);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_field_selector_set_index_content_raw_llc_l3_arp_ipv4_ipv6_ip_l4_offset */
#endif


#ifdef CMD_FIELD_SELECTOR_SET_INDEX_INDEX_FORMAT_RAW_LLC_L3_ARP_IP_HEADER_IP_PAYLOAD_L4_PAYLOAD_OFFSET_OFFSET
/*
 * field-selector set index <UINT:index> format ( raw | llc | l3 | arp | ip-header | ip-payload | l4-payload ) offset <UINT:offset>
 */
cparser_result_t
cparser_cmd_field_selector_set_index_index_format_raw_llc_l3_arp_ip_header_ip_payload_l4_payload_offset_offset(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *offset_ptr)
{
    uint32                          unit;
    int32                           ret;
    rtk_pie_fieldSelector_data_t    fs;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    ret = rtk_pie_fieldSelector_get(unit, *index_ptr, &fs);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    if (0 == osal_strcmp(TOKEN_STR(5), "raw"))
    {
        fs.start = FS_START_POS_RAW;
    }
    else if (0 == osal_strcmp(TOKEN_STR(5), "llc"))
    {
        fs.start = FS_START_POS_LLC;
    }
    else if (0 == osal_strcmp(TOKEN_STR(5), "l3"))
    {
        fs.start = FS_START_POS_L3;
    }
    else if (0 == osal_strcmp(TOKEN_STR(5), "arp"))
    {
        fs.start = FS_START_POS_ARP;
    }
    else if (0 == strncmp(TOKEN_STR(5), "ip-h", 4))
    {
        fs.start = PIE_FS_START_POS_IP_HDR;
    }
    else if (0 == strncmp(TOKEN_STR(5), "ip-p", 4))
    {
        fs.start = FS_START_POS_IP;
    }
    else
    {
        fs.start = FS_START_POS_L4;
    }

    fs.offset = *offset_ptr;
    ret = rtk_pie_fieldSelector_set(unit, *index_ptr, &fs);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_field_selector_set_index_index_format_raw_llc_l3_arp_ip_header_ip_payload_l4_payload_offset_offset */
#endif

#ifdef CMD_FIELD_SELECTOR_GET_INDEX_INDEX
/*
 * field-selector get index <UINT:index>
 */
cparser_result_t
cparser_cmd_field_selector_get_index_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    uint32                          unit;
    int32                           ret;
    rtk_pie_fieldSelector_data_t    fs;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    ret = rtk_pie_fieldSelector_get(unit, *index_ptr, &fs);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("Field selector %d:\n", *index_ptr);

    diag_util_mprintf("\tcontent from: ");
    switch (fs.start)
    {
        case FS_START_POS_RAW:
            diag_util_mprintf("Raw");
            break;
        case FS_START_POS_LLC:
            diag_util_mprintf("LLC");
            break;
        case FS_START_POS_L2:
            diag_util_mprintf("L2");
            break;
        case FS_START_POS_L3:
            diag_util_mprintf("L3");
            break;
        case FS_START_POS_ARP:
            diag_util_mprintf("ARP");
            break;
        case FS_START_POS_IPV4:
            diag_util_mprintf("IPv4");
            break;
        case FS_START_POS_IPV6:
            diag_util_mprintf("IPv6");
            break;
        case PIE_FS_START_POS_IP_HDR:
            diag_util_mprintf("IP-header");
            break;
        case FS_START_POS_IP:
            diag_util_mprintf("IP");
            break;
        case FS_START_POS_L4:
            diag_util_mprintf("L4");
            break;
        default:
            return CPARSER_NOT_OK;
    }
    diag_util_mprintf("\n");

    diag_util_mprintf("\toffset: %d\n", fs.offset);

    #if defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        diag_util_mprintf("\tpayload select: ");
        if (PIE_FS_INNER == fs.payloadSel)
            diag_util_mprintf("Inner\n");
        else
            diag_util_mprintf("Outer\n");
    }
    #endif  /* defined(CONFIG_SDK_RTL9310) */

    return CPARSER_OK;
}   /* end of cparser_cmd_field_selector_get_index */
#endif

#ifdef CMD_FIELD_SELECTOR_SET_INDEX_INDEX_PAYLOAD_SELECT_INNER_OUTER
/*
 * field-selector set index <UINT:index> payload-select ( inner | outer )
 */
cparser_result_t
cparser_cmd_field_selector_set_index_index_payload_select_inner_outer(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    rtk_pie_fieldSelector_data_t    fs;
    uint32                          unit;
    int32                           ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    ret = rtk_pie_fieldSelector_get(unit, *index_ptr, &fs);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    if ('i' == TOKEN_CHAR(5,0))
    {
        fs.payloadSel = PIE_FS_INNER;
    }
    else
    {
        fs.payloadSel = PIE_FS_OUTER;
    }

    ret = rtk_pie_fieldSelector_set(unit, *index_ptr, &fs);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_field_selector_set_index_index_payload_select_inner_outer */
#endif


#ifdef CMD_PIE_SET_METER_IFG_EXCLUDE_INCLUDE
/*
 * pie set meter ifg ( exclude | include )
 */
cparser_result_t cparser_cmd_pie_set_meter_ifg_exclude_include(cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_enable_t    includeIfg = DISABLED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('e' == TOKEN_CHAR(4,0))
    {
        includeIfg = DISABLED;
    }
    else if ('i' == TOKEN_CHAR(4,0))
    {
        includeIfg = ENABLED;
    }

    DIAG_UTIL_ERR_CHK(rtk_pie_meterIncludeIfg_set(unit, includeIfg), ret);

    return CPARSER_OK;
} /* end of cparser_cmd_pie_set_meter_ifg_exclude_include */
#endif

#ifdef CMD_PIE_GET_METER_IFG
/*
 * pie get meter ifg
 */
cparser_result_t cparser_cmd_pie_get_meter_ifg(cparser_context_t *context)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_enable_t    includeIfg = DISABLED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_pie_meterIncludeIfg_get(unit, &includeIfg), ret);

    if (ENABLED == includeIfg)
    {
        diag_util_mprintf("Meter Include IFG\n");
    }
    else
    {
        diag_util_mprintf("Meter Exclude IFG\n");
    }

    return CPARSER_OK;
} /* end of cparser_cmd_pie_get_meter_ifg */
#endif

#ifdef CMD_PIE_GET_METER_ENTRY_INDEX
/*
 * pie get meter entry <UINT:index>
 */
cparser_result_t cparser_cmd_pie_get_meter_entry_index(cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    uint32  meterId;
    rtk_pie_meterEntry_t    meterEntry;


    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    meterId = *index_ptr;

    DIAG_UTIL_ERR_CHK(rtk_pie_meterEntry_get(unit, meterId, &meterEntry), ret);

    diag_util_mprintf("Meter Entry %d\n", meterId);

    diag_util_printf("Meter Type: ");

#if defined(CONFIG_SDK_RTL8390)
    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
        switch(meterEntry.type)
        {
            case METER_TYPE_DLB:
                diag_util_mprintf("Double Leaky Bucket\n");
                diag_util_mprintf("Leaky Bucket 0 Rate: %d\n", meterEntry.lb0_rate);
                diag_util_mprintf("Leaky Bucket 1 Rate: %d\n", meterEntry.lb1_rate);
                break;
            case METER_TYPE_SRTCM:
                diag_util_mprintf("Single Rate Three Color Marker\n");
                diag_util_printf("Color Aware: ");
                if(TRUE == meterEntry.color_aware)
                    diag_util_mprintf("Yes\n");
                else
                    diag_util_mprintf("No\n");

                diag_util_mprintf("Committed Information Rate: %d\n", meterEntry.lb0_rate);
                break;
            case METER_TYPE_TRTCM:
                diag_util_mprintf("Two Rate Three Color Marker\n");
                diag_util_printf("Color Aware: ");
                if(TRUE == meterEntry.color_aware)
                    diag_util_mprintf("Yes\n");
                else
                    diag_util_mprintf("No\n");

                diag_util_mprintf("Committed Information Rate: %d\n", meterEntry.lb0_rate);
                diag_util_mprintf("Peak Information Rate: %d\n", meterEntry.lb1_rate);
                break;
            default:
                diag_util_mprintf("Invalid Entry\n");
                break;
        }
    }
#endif

#if defined(CONFIG_SDK_RTL8380)
    if(DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
    {
        diag_util_mprintf("Single Leaky Bucket\n");
        diag_util_mprintf("Enable: %d\n", meterEntry.enable);
        diag_util_mprintf("Leaky Bucket Rate: %d\n", meterEntry.rate);
        diag_util_mprintf("Threshold group: %d\n", meterEntry.thr_grp);
    }
#endif

#if (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310))
    if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) ||
            DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        switch(meterEntry.type)
        {
            case METER_TYPE_DLB:
                diag_util_mprintf("Double Leaky Bucket\n");
                break;
            case METER_TYPE_SRTCM:
                diag_util_mprintf("Single Rate Three Color Marker\n");
                break;
            case METER_TYPE_TRTCM:
                diag_util_mprintf("Two Rate Three Color Marker\n");
                break;
            default:
                diag_util_mprintf("Invalid Entry\n");
                break;
        }
        diag_util_printf("Color Aware: ");
        if(TRUE == meterEntry.color_aware)
            diag_util_mprintf("Yes\n");
        else
            diag_util_mprintf("No\n");
        diag_util_mprintf("Leaky Bucket 0 Rate: %d\n", meterEntry.lb0_rate);
        diag_util_mprintf("Leaky Bucket 0 Burst Size: %d\n", meterEntry.lb0_bs);
        diag_util_mprintf("Leaky Bucket 1 Rate: %d\n", meterEntry.lb1_rate);
        diag_util_mprintf("Leaky Bucket 1 Burst Size: %d\n", meterEntry.lb1_bs);
            diag_util_mprintf("\n");
    }
#endif

    return CPARSER_OK;
}
#endif

#ifdef CMD_PIE_GET_METER_EXCEED_FLAG
/*
 * pie get meter exceed-flag
 */
cparser_result_t cparser_cmd_pie_get_meter_exceed_flag(cparser_context_t *context)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    uint32  meterId;
    uint32  isExceed;
    uint32  numOfmeter;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_OM_GET_CHIP_CAPACITY(unit, numOfmeter, max_num_of_metering);

    diag_util_mprintf("Exceed Meter Entry: ");
    for(meterId = 0; meterId < numOfmeter; meterId ++)
    {
        DIAG_UTIL_ERR_CHK(rtk_pie_meterExceed_get(unit, meterId, &isExceed), ret);
        if(TRUE == isExceed)
        {
            diag_util_mprintf("%d\n", meterId);
        }
    }

    diag_util_mprintf("\n");

    return CPARSER_OK;
}
#endif

#ifdef CMD_PIE_GET_METER_EXCEED_FLAG_AGGREGATION
/*
 * pie get meter exceed-flag aggregation
 */
cparser_result_t cparser_cmd_pie_get_meter_exceed_flag_aggregation(cparser_context_t *context)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    uint32  exceedIdx;
    uint32  exceedMask;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_pie_meterExceedAggregation_get(unit, &exceedMask), ret);
    diag_util_mprintf("Exceed Meter Entry Index Range (one or more meter entries in the range exceed):");

    for(exceedIdx = 0; exceedIdx < 32; exceedIdx ++)
    {
        if(exceedMask & (1 << exceedIdx))
        {
            diag_util_mprintf("%d - %d\n", exceedIdx * 16, (exceedIdx + 1) * 16);
        }
    }

    diag_util_mprintf("\n");

    return CPARSER_OK;
}
#endif

#ifdef CMD_PIE_GET_PHASE_VACL_IACL_EACL_TEMPLATE_INDEX_VLAN_SEL
/*
 * pie get phase ( vacl | iacl | eacl ) template <UINT:index> vlan-sel
 */
cparser_result_t
cparser_cmd_pie_get_phase_vacl_iacl_eacl_template_index_vlan_sel(cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32                      unit;
    int32                       ret;
    rtk_pie_phase_t             phase;
    rtk_pie_templateVlanSel_t   vlanSel;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if('v' == TOKEN_CHAR(3, 0))
    {
        phase = PIE_PHASE_VACL;
    }
    else if('i' == TOKEN_CHAR(3, 0))
    {
        phase = PIE_PHASE_IACL;
    }
    else
    {
        #if defined(CONFIG_SDK_RTL9310)
        if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
        {
            phase = PIE_PHASE_EACL;
        }
        else
        #endif
        {
            DIAG_ERR_PRINT(RT_ERR_CHIP_NOT_SUPPORTED);
            return CPARSER_NOT_OK;
        }
    }

    ret = rtk_pie_templateVlanSel_get(unit, phase, *index_ptr, &vlanSel);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("Pre-template %d field VLAN: ", *index_ptr);
    if (TMPLTE_VLAN_SEL_INNER == vlanSel)
        diag_util_mprintf("inner\n");
    else if (TMPLTE_VLAN_SEL_OUTER == vlanSel)
        diag_util_mprintf("outer\n");
    else if (TMPLTE_VLAN_SEL_FWD == vlanSel)
        diag_util_mprintf("forwarding vlan\n");
    else
        diag_util_mprintf("forwarding vlan before routing\n");

    return CPARSER_OK;
}
#endif

#ifdef CMD_PIE_SET_PHASE_VACL_IACL_EACL_TEMPLATE_INDEX_VLAN_SEL_INNER_OUTER_FORWARD_SOURCE_FORWARD
/*
 * pie set phase ( vacl | iacl | eacl ) template <UINT:index> vlan-sel ( inner | outer | forward | source_forward )
 */
cparser_result_t
cparser_cmd_pie_set_phase_vacl_iacl_eacl_template_index_vlan_sel_inner_outer_forward_source_forward(cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32                      unit;
    int32                       ret;
    rtk_pie_phase_t             phase;
    rtk_pie_templateVlanSel_t   vlanSel;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if('v' == TOKEN_CHAR(3, 0))
    {
        phase = PIE_PHASE_VACL;
    }
    else if('i' == TOKEN_CHAR(3, 0))
    {
        phase = PIE_PHASE_IACL;
    }
    else
    {
        #if defined(CONFIG_SDK_RTL9310)
        if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
        {
            phase = PIE_PHASE_EACL;
        }
        else
        #endif
        {
            DIAG_ERR_PRINT(RT_ERR_CHIP_NOT_SUPPORTED);
            return CPARSER_NOT_OK;
        }
    }

    if('i' == TOKEN_CHAR(7, 0))
    {
        vlanSel = TMPLTE_VLAN_SEL_INNER;
    }
    else if('o' == TOKEN_CHAR(7, 0))
    {
        vlanSel = TMPLTE_VLAN_SEL_OUTER;
    }
    else if('f' == TOKEN_CHAR(7, 0))
    {
        vlanSel = TMPLTE_VLAN_SEL_FWD;
    }
    else
    {
        #if defined(CONFIG_SDK_RTL9300)
        if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID))
        {
            vlanSel = TMPLTE_VLAN_SEL_SRC_FWD;
        }
        else
        #endif
        {
            DIAG_ERR_PRINT(RT_ERR_CHIP_NOT_SUPPORTED);
            return CPARSER_NOT_OK;
        }
    }

    ret = rtk_pie_templateVlanSel_set(unit, phase, *index_ptr, vlanSel);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_PIE_GET_BLOCK_INDEX_PHASE
/*
 * pie get block <UINT:index> phase
 */
cparser_result_t
cparser_cmd_pie_get_block_index_phase(cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32          unit;
    int32           ret;
    rtk_pie_phase_t phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    ret = rtk_pie_phase_get(unit, *index_ptr, &phase);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("The block %d phase is ", *index_ptr);

    diag_util_mprintf("%s\n", _diag_pie_phaseStr_get(phase));

    return CPARSER_OK;
}
#endif

#ifdef CMD_PIE_SET_BLOCK_INDEX_PHASE_VACL_IACL
/*
 * pie set block <UINT:index> phase ( vacl | iacl )
 */
cparser_result_t
cparser_cmd_pie_set_block_index_phase_vacl_iacl(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32          unit;
    int32           ret;
    rtk_pie_phase_t phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('v' == TOKEN_CHAR(5, 0))
    {
        phase = PIE_PHASE_VACL;
    }
    else
    {
        phase = PIE_PHASE_IACL;
    }

    ret = rtk_pie_phase_set(unit, *index_ptr, phase);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_PIE_SET_BLOCK_INDEX_PHASE_EACL
/*
 * pie set block <UINT:index> phase eacl
 */
cparser_result_t
cparser_cmd_pie_set_block_index_phase_eacl(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32          unit;
    int32           ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    ret = rtk_pie_phase_set(unit, *index_ptr, PIE_PHASE_EACL);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_PIE_SET_BLOCK_INDEX_PHASE_NONE
/*
 * pie set block <UINT:index> phase none
 */
cparser_result_t
cparser_cmd_pie_set_block_index_phase_none(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32          unit;
    int32           ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    ret = rtk_pie_phase_set(unit, *index_ptr, PIE_PHASE_DIS);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_PIE_SET_BLOCK_INDEX_PHASE_INGRESS_FLOWTBL_0_INGRESS_FLOWTBL_3_EGRESS_FLOWTBL_0
/*
 * pie set block <UINT:index> phase ( ingress-flowtbl-0 | ingress-flowtbl-3 | egress-flowtbl-0 )
 */
cparser_result_t
cparser_cmd_pie_set_block_index_phase_ingress_flowtbl_0_ingress_flowtbl_3_egress_flowtbl_0(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32          unit;
    int32           ret;
    rtk_pie_phase_t phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('i' == TOKEN_CHAR(5, 0))
    {
        if ('0' == TOKEN_CHAR(5, 16))
            phase = PIE_PHASE_IGR_FLOW_TABLE_0;
        else
            phase = PIE_PHASE_IGR_FLOW_TABLE_3;
    }
    else
    {
        phase = PIE_PHASE_EGR_FLOW_TABLE_0;
    }

    ret = rtk_pie_phase_set(unit, *index_ptr, phase);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_PIE_GET_METER_DP_SEL
/*
 * pie get meter dp-sel
 */
cparser_result_t
cparser_cmd_pie_get_meter_dp_sel(
    cparser_context_t *context)
{
    uint32                      unit;
    int32                       ret;
    rtk_pie_meterDpSel_t        dpSel;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    ret = rtk_pie_meterDpSel_get(unit, &dpSel);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("DP select: ");
    if (METER_DP_SEL_DEEPEST_COLOR == dpSel)
        diag_util_mprintf("the deepest color\n");
    else
        diag_util_mprintf("the lowest index\n");

    return CPARSER_OK;
}   /* end of cparser_cmd_range_check_get_entry_index_pkt_len */
#endif

#ifdef CMD_PIE_SET_METER_DP_SEL_DEEPEST_COLOR_LOWEST_INDEX
/*
 * pie set meter dp-sel ( deepest-color | lowest-index )
 */
cparser_result_t
cparser_cmd_pie_set_meter_dp_sel_deepest_color_lowest_index(
    cparser_context_t *context)
{
    uint32                      unit;
    int32                       ret;
    rtk_pie_meterDpSel_t        dpSel;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if('d' == TOKEN_CHAR(4, 0))
        dpSel = METER_DP_SEL_DEEPEST_COLOR;
    else
        dpSel = METER_DP_SEL_LOWEST_IDX;

    ret = rtk_pie_meterDpSel_set(unit, dpSel);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_PIE_GET_ARP_MAC_SEL
/*
 * pie get arp-mac-sel
 */
cparser_result_t
cparser_cmd_pie_get_arp_mac_sel(
    cparser_context_t *context)
{
    rtk_pie_arpMacSel_t selType;
    uint32              unit;
    int32               ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    ret = rtk_pie_arpMacSel_get(unit, &selType);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("ARP MAC selection: ");
    if (ARP_MAC_SEL_L2 == selType)
    {
        diag_util_mprintf("L2\n");
    }
    else
    {
        diag_util_mprintf("ARP\n");
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pie_get_arp_mac_sel */
#endif

#ifdef CMD_PIE_SET_ARP_MAC_SEL_L2_ARP
/*
 * pie set arp-mac-sel ( l2 | arp )
 */
cparser_result_t
cparser_cmd_pie_set_arp_mac_sel_l2_arp(
    cparser_context_t *context)
{
    rtk_pie_arpMacSel_t selType;
    uint32              unit;
    int32               ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('l' == TOKEN_CHAR(3, 0))
    {
        selType = ARP_MAC_SEL_L2;
    }
    else
    {
        selType = ARP_MAC_SEL_ARP;
    }

    ret = rtk_pie_arpMacSel_set(unit, selType);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pie_set_arp_mac_sel_l2_arp */
#endif

#ifdef CMD_RANGE_CHECK_GET_ENTRY_INDEX
/*
 * range-check get entry <UINT:index>
 */
cparser_result_t
cparser_cmd_range_check_get_entry_index(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    rtk_pie_rangeCheck_t    rng;
    uint32                  unit;
    int32                   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    ret = rtk_pie_rangeCheck_get(unit, *index_ptr, &rng);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("Range check %d\n", *index_ptr);
    diag_util_mprintf(" type: ");
    switch (rng.type)
    {
        case RTK_RNGCHK_IVID:
            diag_util_mprintf("IVID\n");
            break;
        case RTK_RNGCHK_OVID:
            diag_util_mprintf("OVID\n");
            break;
        case RTK_RNGCHK_L4SPORT:
            diag_util_mprintf("L4 source port\n");
            break;
        case RTK_RNGCHK_L4DPORT:
            diag_util_mprintf("L4 destination port\n");
            break;
        case RTK_RNGCHK_L4PORT:
            diag_util_mprintf("L4 source or destination port\n");
            break;
        case RTK_RNGCHK_PKTLEN:
            diag_util_mprintf("packet length\n");
            break;
        case RTK_RNGCHK_L3LEN:
            diag_util_mprintf("packet L3 length\n");
            break;
        default:
            return CPARSER_NOT_OK;
    }

    diag_util_mprintf(" upper bound:%d\n", rng.upper_bound);
    diag_util_mprintf(" lower bound:%d\n", rng.lower_bound);

    return CPARSER_OK;
}   /* end of cparser_cmd_range_check_get_entry_index */
#endif

#ifdef CMD_RANGE_CHECK_SET_ENTRY_INDEX_IVID_OVID_L4_SPORT_L4_DPORT_L4_PORT_PKT_LEN_L3_LEN_UP_BOUND_UPPER_LOW_BOUND_LOWER
/*
 * range-check set entry <UINT:index> ( ivid | ovid | l4-sport | l4-dport | l4-port | pkt-len | l3-len ) up-bound <UINT:upper> low-bound <UINT:lower>
 */
cparser_result_t
cparser_cmd_range_check_set_entry_index_ivid_ovid_l4_sport_l4_dport_l4_port_pkt_len_l3_len_up_bound_upper_low_bound_lower(
    cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *upper_ptr,
    uint32_t *lower_ptr)
{
    rtk_pie_rangeCheck_t    rng;
    uint32                  unit;
    int32                   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if ('i' == TOKEN_CHAR(4, 0))
        rng.type = RTK_RNGCHK_IVID;
    else if ('o' == TOKEN_CHAR(4, 0))
        rng.type = RTK_RNGCHK_OVID;
    else if ('p' == TOKEN_CHAR(4, 0))
        rng.type = RTK_RNGCHK_PKTLEN;
    else
    {
        if ('3' == TOKEN_CHAR(4, 1))
            rng.type = RTK_RNGCHK_L3LEN;
        else if ('s' == TOKEN_CHAR(4, 3))
            rng.type = RTK_RNGCHK_L4SPORT;
        else if ('d' == TOKEN_CHAR(4, 3))
            rng.type = RTK_RNGCHK_L4DPORT;
        else
            rng.type = RTK_RNGCHK_L4PORT;
    }

    rng.upper_bound = *upper_ptr;
    rng.lower_bound = *lower_ptr;

    ret = rtk_pie_rangeCheck_set(unit, *index_ptr, &rng);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_range_check_set_entry_index_ivid_ovid_l4_sport_l4_dport_l4_port_pkt_len_l3_len_up_bound_upper_low_bound_lower */
#endif

#ifdef CMD_PIE_SET_METER_ENTRY_INDEX_DLB_LB0_RATE_LB0_RATE_LB0_BURST_LB0_BURST_LB1_RATE_LB1_RATE_LB1_BURST_LB1_BURST
/*
 * pie set meter entry <UINT:index> dlb lb0-rate <UINT:lb0_rate> lb0-burst <UINT:lb0_burst> lb1-rate <UINT:lb1_rate> lb1-burst <UINT:lb1_burst>
 */
cparser_result_t
cparser_cmd_pie_set_meter_entry_index_dlb_lb0_rate_lb0_rate_lb0_burst_lb0_burst_lb1_rate_lb1_rate_lb1_burst_lb1_burst(
    cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *lb0_rate_ptr,
    uint32_t *lb0_burst_ptr,
    uint32_t *lb1_rate_ptr,
    uint32_t *lb1_burst_ptr)
{
    rtk_pie_meterEntry_t    meter;
    uint32                  unit;
    int32                   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    ret = rtk_pie_meterEntry_get(unit, *index_ptr, &meter);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    meter.type = METER_TYPE_DLB;
    meter.lb0_rate = *lb0_rate_ptr;
    meter.lb0_bs = *lb0_burst_ptr;
    meter.lb1_rate = *lb1_rate_ptr;
    meter.lb1_bs = *lb1_burst_ptr;

    ret = rtk_pie_meterEntry_set(unit, *index_ptr, &meter);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pie_set_meter_entry_index_dlb_lb0_rate_lb0_rate_lb0_burst_lb0_burst_lb1_rate_lb1_rate_lb1_burst_lb1_burst */
#endif

#ifdef CMD_PIE_SET_METER_ENTRY_INDEX_SRTCM_COLOR_AWARE_COLOR_BLIND_CIR_CIR_CBS_CBS_EBS_EBS
/*
 * pie set meter entry <UINT:index> srtcm ( color-aware | color-blind ) cir <UINT:cir> cbs <UINT:cbs> ebs <UINT:ebs>
 */
cparser_result_t
cparser_cmd_pie_set_meter_entry_index_srtcm_color_aware_color_blind_cir_cir_cbs_cbs_ebs_ebs(
    cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *cir_ptr,
    uint32_t *cbs_ptr,
    uint32_t *ebs_ptr)
{
    rtk_pie_meterEntry_t    meter;
    uint32                  unit;
    int32                   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    ret = rtk_pie_meterEntry_get(unit, *index_ptr, &meter);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    meter.type = METER_TYPE_SRTCM;
    meter.lb0_rate = *cir_ptr;
    meter.lb0_bs = *cbs_ptr;
    meter.lb1_rate = *cir_ptr;
    meter.lb1_bs = *ebs_ptr;

    if('a' == TOKEN_CHAR(6, 6))
    {
        meter.color_aware = 1;
    }
    else
    {
        meter.color_aware = 0;
    }

    ret = rtk_pie_meterEntry_set(unit, *index_ptr, &meter);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pie_set_meter_entry_index_srtcm_color_aware_color_blind_cir_cir_cbs_cbs_ebs_ebs */
#endif

#ifdef CMD_PIE_SET_METER_ENTRY_INDEX_TRTCM_COLOR_AWARE_COLOR_BLIND_CIR_CIR_CBS_CBS_PIR_PIR_PBS_PBS
/*
 * pie set meter entry <UINT:index> trtcm ( color-aware | color-blind ) cir <UINT:cir> cbs <UINT:cbs> pir <UINT:pir> pbs <UINT:pbs>
 */
cparser_result_t
cparser_cmd_pie_set_meter_entry_index_trtcm_color_aware_color_blind_cir_cir_cbs_cbs_pir_pir_pbs_pbs(
    cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *cir_ptr,
    uint32_t *cbs_ptr,
    uint32_t *pir_ptr,
    uint32_t *pbs_ptr)
{
    rtk_pie_meterEntry_t    meter;
    uint32                  unit;
    int32                   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    ret = rtk_pie_meterEntry_get(unit, *index_ptr, &meter);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    meter.type = METER_TYPE_TRTCM;
    meter.lb0_rate = *cir_ptr;
    meter.lb0_bs = *cbs_ptr;
    meter.lb1_rate = *pir_ptr;
    meter.lb1_bs = *pbs_ptr;

    if('a' == TOKEN_CHAR(6, 6))
    {
        meter.color_aware = 1;
    }
    else
    {
        meter.color_aware = 0;
    }

    ret = rtk_pie_meterEntry_set(unit, *index_ptr, &meter);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pie_set_meter_entry_index_trtcm_color_aware_color_blind_cir_cir_cbs_cbs_pir_pir_pbs_pbs */
#endif

#ifdef CMD_PIE_GET_METER_ENTRY_INDEX_MODE
/*
 * pie get meter entry <UINT:index> mode
 */
cparser_result_t
cparser_cmd_pie_get_meter_entry_index_mode(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    rtk_pie_meterEntry_t    meter;
    uint32                  unit;
    int32                   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    ret = rtk_pie_meterEntry_get(unit, *index_ptr, &meter);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("Meter entry %u mode: ", *index_ptr);
    if (METER_MODE_BYTE == meter.mode)
    {
        diag_util_mprintf("Byte\n");
    }
    else
    {
        diag_util_mprintf("Packet\n");
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pie_get_meter_entry_index_mode */
#endif

#ifdef CMD_PIE_SET_METER_ENTRY_INDEX_MODE_BYTE_PACKET
/*
 * pie set meter entry <UINT:index> mode ( byte | packet ) */
cparser_result_t
cparser_cmd_pie_set_meter_entry_index_mode_byte_packet(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    rtk_pie_meterEntry_t    meter;
    uint32                  unit;
    int32                   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    ret = rtk_pie_meterEntry_get(unit, *index_ptr, &meter);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    if('b' == TOKEN_CHAR(6, 0))
    {
        meter.mode = METER_MODE_BYTE;
    }
    else
    {
        meter.mode = METER_MODE_PACKET;
    }

    ret = rtk_pie_meterEntry_set(unit, *index_ptr, &meter);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pie_set_meter_entry_index_mode_byte_packet */
#endif

#ifdef CMD_PIE_GET_FILTER_INTERFACE_SELECT
/*
 * pie get filter interface-select
 */
cparser_result_t
cparser_cmd_pie_get_filter_interface_select(
    cparser_context_t *context)
{
    rtk_pie_intfSel_t   intfSel;
    uint32              unit;
    int32               ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    ret = rtk_pie_intfSel_get(unit, &intfSel);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("Filter interface type ");
    if (PIE_INTF_SEL_L3 == intfSel)
    {
        diag_util_mprintf("L3\n");
    }
    else
    {
        diag_util_mprintf("Tunnel\n");
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pie_get_filter_interface_select */
#endif

#ifdef CMD_PIE_SET_FILTER_INTERFACE_L3_TUNNEL
/*
 * pie set filter interface ( l3 | tunnel )
 */
cparser_result_t
cparser_cmd_pie_set_filter_interface_l3_tunnel(
    cparser_context_t *context)
{
    rtk_pie_intfSel_t   intfSel;
    uint32              unit;
    int32               ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if('l' == TOKEN_CHAR(4, 0))
    {
        intfSel = PIE_INTF_SEL_L3;
    }
    else
    {
        intfSel = PIE_INTF_SEL_TUNNEL;
    }

    ret = rtk_pie_intfSel_set(unit, intfSel);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pie_set_filter_interface_l3_tunnel */
#endif

#ifdef CMD_PIE_GET_PHASE_VACL_TEMPLATE_INDEX_VLAN_FMT_SEL
/*
 * pie get phase vacl template <UINT:index> vlan-fmt-sel
 */
cparser_result_t
cparser_cmd_pie_get_phase_vacl_template_index_vlan_fmt_sel(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    rtk_pie_templateVlanFmtSel_t    vlanFmtSel;
    uint32  unit;
    int32   ret;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_pie_templateVlanFmtSel_get(unit, PIE_PHASE_VACL,
            *index_ptr, &vlanFmtSel), ret);

    diag_util_mprintf("Template %d VLAN format select : ", *index_ptr);
    if (TMPLTE_VLAN_FMT_SEL_ORIGINAL == vlanFmtSel)
    {
        diag_util_mprintf("Original\n");
    }
    else
    {
        diag_util_mprintf("Modified\n");
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pie_get_phase_vacl_template_index_vlan_fmt_sel */
#endif

#ifdef CMD_PIE_SET_PHASE_VACL_TEMPLATE_INDEX_VLAN_FMT_SEL_ORIGINAL_MODIFIED
/*
 * pie set phase vacl template <UINT:index> vlan-fmt-sel ( original | modified )
 */
cparser_result_t
cparser_cmd_pie_set_phase_vacl_template_index_vlan_fmt_sel_original_modified(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    rtk_pie_templateVlanFmtSel_t    vlanFmtSel;
    uint32  unit;
    int32   ret;

    DIAG_UTIL_FUNC_INIT(unit);

    if ('o' == TOKEN_CHAR(7, 0))
    {
        vlanFmtSel = TMPLTE_VLAN_FMT_SEL_ORIGINAL;
    }
    else
    {
        vlanFmtSel = TMPLTE_VLAN_FMT_SEL_MODIFIED;
    }

    DIAG_UTIL_ERR_CHK(rtk_pie_templateVlanFmtSel_set(unit, PIE_PHASE_VACL,
            *index_ptr, vlanFmtSel), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_pie_set_phase_vacl_template_index_vlan_fmt_sel_original_modified */
#endif

char *
diag_pie_templateAbbreviateName_get(rtk_pie_templateFieldType_t templFieldType)
{
    int     i;
    for (i = 0; i < (sizeof(diag_pie_template_list)/sizeof(diag_pie_template_t)); ++i)
    {
        if (templFieldType == diag_pie_template_list[i].type)
        {
            return diag_pie_template_list[i].abbre;
        }
    }
    return NULL;
}

int32
diag_pie_templateNames_get(rtk_pie_templateFieldType_t templFieldType, char **abbre, char **user_info)
{
    int     i;
    for (i = 0; i < (sizeof(diag_pie_template_list)/sizeof(diag_pie_template_t)); ++i)
    {
        if (templFieldType == diag_pie_template_list[i].type)
        {
            *abbre = diag_pie_template_list[i].abbre;
            *user_info = diag_pie_template_list[i].user_info;
            return RT_ERR_OK;
        }
    }
    return RT_ERR_FAILED;
}

#ifdef CMD_PIE_GET_METER_TRTCM_TYPE
/*
 * pie get meter trtcm-type
 */
cparser_result_t
cparser_cmd_pie_get_meter_trtcm_type(
    cparser_context_t *context)
{
    rtk_pie_meterTrtcmType_t    type;
    uint32                      unit;
    int32                       ret;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_pie_meterTrtcmType_get(unit, &type), ret);

    diag_util_mprintf("Meter trTCM type : ");
    if (METER_TRTCM_TYPE_ORIGINAL == type)
    {
        diag_util_mprintf("Original\n");
    }
    else
    {
        diag_util_mprintf("Modified\n");
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pie_get_meter_trtcm_type */
#endif

#ifdef CMD_PIE_SET_METER_TRTCM_TYPE_ORIGINAL_MODIFIED
/*
 * pie set meter trtcm-type ( original | modified )
 */
cparser_result_t
cparser_cmd_pie_set_meter_trtcm_type_original_modified(
    cparser_context_t *context)
{
    rtk_pie_meterTrtcmType_t    type;
    uint32                      unit;
    int32                       ret;

    DIAG_UTIL_FUNC_INIT(unit);

    if ('o' == TOKEN_CHAR(4, 0))
    {
        type = METER_TRTCM_TYPE_ORIGINAL;
    }
    else
    {
        type = METER_TRTCM_TYPE_MODIFIED;
    }

    DIAG_UTIL_ERR_CHK(rtk_pie_meterTrtcmType_set(unit, type), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_pie_set_meter_trtcm_type_original_modified */
#endif

#ifdef CMD_PIE_GET_FILTER_8021BR
/*
 * pie get filter 8021br
 */
cparser_result_t
cparser_cmd_pie_get_filter_8021br(
    cparser_context_t *context)
{
    rtk_enable_t    en;
    uint32          unit;
    int32           ret;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_UTIL_ERR_CHK(rtk_pie_filter1BR_get(unit, &en), ret);

    diag_util_mprintf("Filter 802.1BR status : ");
    if (DISABLED == en)
    {
        diag_util_mprintf("%s\n", DIAG_STR_DISABLE);
    }
    else
    {
        diag_util_mprintf("%s\n", DIAG_STR_ENABLE);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_pie_get_filter_8021br */
#endif

#ifdef CMD_PIE_SET_FILTER_8021BR_DISABLE_ENABLE
/*
 * pie set filter 8021br ( disable | enable )
 */
cparser_result_t
cparser_cmd_pie_set_filter_8021br_disable_enable(
    cparser_context_t *context)
{
    rtk_enable_t    en;
    uint32          unit;
    int32           ret;

    DIAG_UTIL_FUNC_INIT(unit);

    en = ('e' == TOKEN_CHAR(4, 0))? ENABLED : DISABLED;

    DIAG_UTIL_ERR_CHK(rtk_pie_filter1BR_set(unit, en), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_pie_set_filter_8021br_disable_enable */
#endif

