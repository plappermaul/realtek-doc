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
/*
 * Symbol Definition
 */
typedef struct diag_of_field_s
{
    rtk_of_matchfieldType_t type;
    char user_info[40];
    char desc[148];
} diag_of_field_t;

/*
 * Data Declaration
 */
diag_of_field_t diag_of_field_list[] =
{
    {MATCH_FIELD_DROP_PRECEDENCE,       "dp",                   "drop precedence"},
    {MATCH_FIELD_LOOPBACK_TIME,         "loopback-time",        "packet loopbacked times"},
    {MATCH_FIELD_OUTPUT_ACTION,         "output-action",        "output action forwards a packet to a specified OpenFlow port"},
    {MATCH_FIELD_OUTPUT_DATA,           "output-data",          "data corresponds to output action"},
    {MATCH_FIELD_METADATA,              "metadata",             "metadata"},
    {MATCH_FIELD_TEMPLATE_ID,           "template-id",          "template ID the entry maps to"},
    {MATCH_FIELD_FRAME_TYPE,            "frame-type",           "frame type(0x0: ARP, 0x1: L2 only, 0x2: IPv4, 0x3: IPv6"},
    {MATCH_FIELD_TRUNK_ID,              "trunk-id",             "trunk group ID"},
    {MATCH_FIELD_IN_PHY_PORT,           "src-phy-port",         "switch physical input port"},
    {MATCH_FIELD_IN_PHY_PORTMASK,       "src-phy-portmask",     "switch physical input portmask"},
    {MATCH_FIELD_OUT_PHY_PORT,          "dest-phy-port",        "switch physical output port"},
    {MATCH_FIELD_OUT_PHY_PORTMASK,      "dest-phy-portmask",    "switch physical output portmask"},
    {MATCH_FIELD_ETH_DST,               "dmac",                 "destination MAC address. The field can optional represent 'destination hardware address' of ARP header for ARP/RARP packet"},
    {MATCH_FIELD_ETH_SRC,               "smac",                 "source MAC address. The field can optional represent'source hardware address' of ARP header for ARP/RARP packet"},
    {MATCH_FIELD_ITAG_EXIST,            "itag-exist",           "packet with inner tag"},
    {MATCH_FIELD_OTAG_EXIST,            "otag-exist",           "packet with outer tag"},
    {MATCH_FIELD_ITAG_FMT,              "itag-fmt",             "0b0: inner tag packet, 0b1: inner priority tag or untag packet"},
    {MATCH_FIELD_OTAG_FMT,              "otag-fmt",             "0b0: outer tag packet, 0b1: outer priority tag or untag packet"},
    {MATCH_FIELD_OTAG_PCP,              "otag-pcp",             "O-TAG priority"},
    {MATCH_FIELD_OTAG_DEI,              "otag-dei",             "O-TAG DEI field"},
    {MATCH_FIELD_OTAG_VID,              "otag-vid",             "O-TAG VID"},
    {MATCH_FIELD_ITAG_PCP,              "itag-pcp",             "I-TAG priority"},
    {MATCH_FIELD_ITAG_CFI,              "itag-cfi",             "I-TAG CFI field"},
    {MATCH_FIELD_ITAG_VID,              "itag-vid",             "I-TAG VID"},
    {MATCH_FIELD_FRAME_TYPE_L2,         "L2-frame-type",        "L2 frame type(0x0: Ethernet, 0x1: LLC_SNAP, 0x2: LLC_Other, 0x3: Reserved"},
    {MATCH_FIELD_ETH_TYPE,              "eth-type",             "ethernet type/length"},
    {MATCH_FIELD_ARP_SHA,               "arp-sha",              "'source hardware address' of ARP header for ARP/RARP packet"},
    {MATCH_FIELD_ARP_THA,               "arp-tha",              "'destination hardware address' of ARP header for ARP/RARP packet"},
    {MATCH_FIELD_ARP_OP,                "arp-op",               "ARP/RARP Opcode"},
    {MATCH_FIELD_OMPLS_LABEL,           "ompls-label",          "outer MPLS label"},
    {MATCH_FIELD_OMPLS_TC,              "ompls-exp",            "outer MPLS label EXP"},
    {MATCH_FIELD_OMPLS_LABEL_EXIST,     "ompls-exist",          "outer MPLS label exist"},
    {MATCH_FIELD_OMPLS_BOS,             "ompls-bos",            "outer MPLS label BoS"},
    {MATCH_FIELD_IMPLS_LABEL,           "impls-label",          "inner MPLS label"},
    {MATCH_FIELD_IMPLS_TC,              "impls-exp",            "inner MPLS label EXP"},
    {MATCH_FIELD_IMPLS_LABEL_EXIST,     "impls-exist",          "inner MPLS label exist"},
    {MATCH_FIELD_IMPLS_BOS,             "impls-bos",            "inner MPLS label BoS"},
    {MATCH_FIELD_DSAP,                  "dsap",                 "DSAP for LLC/SNAP packet"},
    {MATCH_FIELD_SSAP,                  "ssap",                 "SSAP for LLC/SNAP packet"},
    {MATCH_FIELD_SNAP_OUI,              "snap-oui",             "OUI in SNAP header"},
    {MATCH_FIELD_L4_PROTO,              "L4-frame-type",        "Layer 4 format(0x0: UDP, 0x1: TCP, 0x2: ICMP/ICMPv6, 0x3: IGMP(Exculde MLD), 0x40: MLD, 0x5: SCTP, 0x6: Layer4 Unknown, 0x7: Reserved"},
    {MATCH_FIELD_IPV4_SRC,              "ip4-sip",              "IPv4 source IP. The field represents 'source protocol address' of ARP header for ARP/RARP packet"},
    {MATCH_FIELD_IPV4_DST,              "ip4-dip",              "IPv4 destination IP. The field represents 'destination protocol address' of ARP header for ARP/RARP packet"},
    {MATCH_FIELD_IPV6_SRC,              "ip6-sip",              "IPv6 source IP."},
    {MATCH_FIELD_IPV6_DST,              "ip6-sip",              "IPv6 destination IP."},
    {MATCH_FIELD_IPV6_FLABEL,           "ip6-flow-label",       "IPv6 flow label"},
    {MATCH_FIELD_IP4TOS_IP6TC,          "tos-tc",               "IPv4 TOS byte/IPv6 Traffic Class byte"},
    {MATCH_FIELD_IP_DSCP,               "dscp",                 "IP DSCP"},
    {MATCH_FIELD_IP_PROTO,              "proto-nh",             "IPv4 protocol/IPv6 next header"},
    {MATCH_FIELD_IP_FLAG,               "ip-flag",              "IPv4 flags"},
    {MATCH_FIELD_IP_FRAG,               "ip-frag",              "IPv4 or IPv6 fragment , include first fragment"},
    {MATCH_FIELD_IP_TTL,                "ttl-hoplimit",         "IPv4 TTL/IPv6 hop limit(0x0: TTL = 0, 0x1: TTL = 1, 0x2: 2<= TTL < 255, 0x3: TTL = 255)"},
    {MATCH_FIELD_IP6_HDR_UNSEQ,         "ip6-hdr-unseq",        "IPv6 packet headers were not in the order"},
    {MATCH_FIELD_IP6_HDR_UNREP,         "ip6-hdr-unrep",        "IPv6 packet has unexcepted repeat header"},
    {MATCH_FIELD_IP6_NONEXT_HDR_EXIST,  "ip6-no-ext-hdr",       "IPv6 packet with no next extended header"},
    {MATCH_FIELD_IP6_MOB_HDR_EXIST,     "ip6-mob-hdr-exist",    "IPv6 packet with mobility header"},
    {MATCH_FIELD_IP6_ESP_HDR_EXIST,     "ip6-esp-hdr-exist",    "IPv6 packet with ESP header"},
    {MATCH_FIELD_IP6_AUTH_HDR_EXIST,    "ip6-auth-hdr-exist",   "IPv6 packet with authentication header"},
    {MATCH_FIELD_IP6_DEST_HDR_EXIST,    "ip6-dest-hdr-exist",   "IPv6 packet with destination option header"},
    {MATCH_FIELD_IP6_FRAG_HDR_EXIST,    "ip6-frag-hdr-exist",   "IPv6 packet with fragment header"},
    {MATCH_FIELD_IP6_ROUTING_HDR_EXIST, "ip6-routing-hdr-exist","IPv6 packet with routing header"},
    {MATCH_FIELD_IP6_HOP_HDR_EXIST,     "ip6-hop-hdr-exist",    "IPv6 packet with hop-by-hop header"},
    {MATCH_FIELD_IGMP_TYPE,             "igmp-type",            "IGMP type"},
    {MATCH_FIELD_IGMP_MAX_RESP_CODE,    "igmp-max-resp-code",   "IGMP max response code"},
    {MATCH_FIELD_ICMP_CODE,             "icmp-code",            "ICMP/ICMPv6 code"},
    {MATCH_FIELD_ICMP_TYPE,             "icmp-type",            "ICMP/ICMPv6 type"},
    {MATCH_FIELD_L4_HDR,                "l4-hdr",               "unknown L4 header byte 0~3"},
    {MATCH_FIELD_L4_SRC_PORT,           "l4-sport",             "TCP/UDP/SCTP source port"},
    {MATCH_FIELD_L4_DST_PORT,           "l4-dport",             "TCP/UDP/SCTP destination port"},
    {MATCH_FIELD_TCP_ECN,               "tcp-ecn",              "ECN in TCP flag"},
    {MATCH_FIELD_TCP_FLAG,              "tcp-flag",             "TCP flag"},
    {MATCH_FIELD_TCP_NONZEROSEQ,        "tcp-nonzero-seq",      "TCP sequence non-zero"},
    {MATCH_FIELD_VID_RANGE,             "range-vid",            "VID range check result"},
    {MATCH_FIELD_L4_PORT_RANGE,         "range-l4port",         "TCP/UDP/SCTP port range check result"},
    {MATCH_FIELD_IP_RANGE,              "range-ip",             "IPv4/IPv6 address range check result"},
    {MATCH_FIELD_LEN_RANGE,             "range-len",            "Packet length(L2 or L3 length is configurable) range check result"},
    {MATCH_FIELD_FIELD_SELECTOR_VALID_MSK, "field-selector-mask", "Field selector valid mask"},
    {MATCH_FIELD_FIELD_SELECTOR0,       "field-selector0",      "Field selector 0 output"},
    {MATCH_FIELD_FIELD_SELECTOR1,       "field-selector1",      "Field selector 1 output"},
    {MATCH_FIELD_FIELD_SELECTOR2,       "field-selector2",      "Field selector 2 output"},
    {MATCH_FIELD_FIELD_SELECTOR3,       "field-selector3",      "Field selector 3 output"},
    {MATCH_FIELD_FIELD_SELECTOR4,       "field-selector4",      "Field selector 4 output"},
    {MATCH_FIELD_FIELD_SELECTOR5,       "field-selector5",      "Field selector 5 output"},
    {MATCH_FIELD_FIELD_SELECTOR6,       "field-selector6",      "Field selector 6 output"},
    {MATCH_FIELD_FIELD_SELECTOR7,       "field-selector7",      "Field selector 7 output"},
    {MATCH_FIELD_FIELD_SELECTOR8,       "field-selector8",      "Field selector 8 output"},
    {MATCH_FIELD_FIELD_SELECTOR9,       "field-selector9",      "Field selector 9 output"},
    {MATCH_FIELD_FIELD_SELECTOR10,      "field-selector10",     "Field selector 10 output"},
    {MATCH_FIELD_FIELD_SELECTOR11,      "field-selector11",     "Field selector 11 output"},
    {MATCH_FIELD_FIELD_SELECTOR12,      "field-selector12",     "Field selector 12 output"},
    {MATCH_FIELD_FIELD_SELECTOR13,      "field-selector13",     "Field selector 13 output"},
    {MATCH_FIELD_GRE_KEY,               "grp-key",              "GRE key"},
    {MATCH_FIELD_VXLAN_VNI,             "vxlan-vni",            "VNI of VxLAN packet"},
    {MATCH_FIELD_GTP_TEID,              "gtp-teid",             "TEID of GTP(GPRS Tunneling Protocol) packet"},
};

static const char text_meter_type[OF_METER_TYPE_END][16] =
{
    "Drop",
    "DSCP Remark",
};

static const char text_pop_mpls_type[OF_POP_MPLS_TYPE_END][16] =
{
    "outermost label",
    "double label",
};


static const char text_push_mpls_mode[OF_PUSH_MPLS_MODE_END][16] =
{
    "normal",
    "encaptbl",
};

static const char text_push_mpls_vpn_type[OF_PUSH_MPLS_VPN_TYPE_END][8] =
{
    "L2",
    "L3",
};

static const char text_output_type[OF_OUTPUT_TYPE_END][32] =
{
    "NONE",
    "PHY_PORT_EX_SRC_PORT",
    "PHY_PORT",
    "TRK_PORT_EX_SRC_PORT",
    "TRK_PORT",
    "MULTI_EGR_PORT",
    "IN_PORT",
    "FLOOD",
    "NORMAL",
    "TUNNEL",
    "FAILOVER",
};

static const char text_egr_output_type[OF_OUTPUT_TYPE_END][32] =
{
    "FORWARD",
    "NONE",
};

static const char text_gototbl_type[OF_GOTOTBL_TYPE_END][32] =
{
    "NORMAL",
    "APPLY_AND_LB",
    "LB",
};

static const char text_setfield_type[OF_SET_FIELD_TYPE_END][32] =
{
    "None",
    "SMAC",
    "DMAC",
    "VLAN Priority",
    "VLAN ID",
    "MPLS Traffic Class",
    "MPLS TTL",
    "MPLS Label",
    "IPv4/IPv6 DSCP",
    "IPv4 TTL/IPv6 Hop Limit",
    "IPv4 Flag Reserved Bit",
    "Source IP",
    "Destination IP",
    "L4 Source Port",
    "L4 Destination Port",
};

static const char text_counter_mode[OF_FLOW_CNTMODE_END][64] =
{
    "packet counter and byte counter",
    "packet counter and packet counter trigger threshold",
    "byte counter and byte counter trigger threshold",
    "Disable",
};


static const char text_l2_match_field[OF_L2_FT_MATCH_FIELD_END][100] =
{
    "(VID,SA) and (0,SA)",
    "(VID,SA) and (VID,DA)",
    "(VID,SA) and (0,DA)",
    "(0,SA) and (VID,DA)",
    "(0,SA) and (0,DA)",
    "(VID,DA) and (0,DA)",
    "(DA,SA) and (VID,SA)",
    "(DA,SA) and (0,SA)",
    "(DA,SA) and (VID,DA)",
    "(DA,SA) and (0,DA)",
    "(IP PROTOCOL,L4_DPORT,L4_SPORT,DIP,SIP) and (VID,L4_DPORT,L4_SPORT,DIP,SIP)",
};

static const char text_l2_flow_entry_type[OF_L2_FLOW_ENTRY_TYPE_END][64] =
{
    "SA",
    "VID_SA",
    "DA",
    "VID_DA",
    "SA_DA",
    "5_TUPLE_IPPROTO",
    "5_TUPLE_VID",
};


static const char text_l3_flow_tbl_list[OF_L3_FLOW_TBL_LIST_END][8] =
{
    "CAM",
    "Hash",
};

static const char text_l3_cam_match_field[OF_L3_CAM_FT_MATCH_FIELD_END][48] =
{
    "(Metadata,SIP) and (Metadata,DIP)",
    "(Metadata,SIP,DIP) and (Metadata,SIP)",
    "(Metadata,SIP,DIP) and (Metadata,DIP)",
};

static const char text_l3_hash_match_field[OF_L3_HASH_FT_MATCH_FIELD_END][40] =
{
    "1st lookup: SIP, 2nd lookup: DIP",
    "1st lookup:(SIP,DIP), 2nd lookup: SIP",
    "1st lookup:(SIP,DIP), 2nd lookup: DIP",
};

static const char text_l3_flow_entry_type[OF_L3_FLOW_ENTRY_TYPE_END][13] =
{
    "IPv4 SIP",
    "IPv4 DIP",
    "IPv4 SIP+DIP",
    "IPv6 SIP",
    "IPv6 DIP",
    "IPv6 SIP+DIP",
};

static const char text_group_type[OF_GROUP_TYPE_END][16] =
{
    "All",
    "Select",
};

static const char text_trap_target[RTK_TRAP_END][8] =
{
    "Local",
    "Master",
};

static const char text_table_miss_act[OF_TBLMISS_ACT_END][32] =
{
    "Drop",
    "Trap",
    "Forward to next table",
    "Execute current action set",
};

typedef enum rtk_of_ins_action_type_e
{
    OF_INS_ACT_TYPE_METER,
    OF_INS_ACT_TYPE_CLEARACT,
    OF_INS_ACT_TYPE_WRITEACT,
    OF_INS_ACT_TYPE_WRITEMETA,
    OF_INS_ACT_TYPE_GOTOTBL,
    OF_INS_ACT_TYPE_DROP,
    OF_INS_ACT_TYPE_END,
} rtk_of_ins_action_type_t;


#define DIAG_UTIL_OF_PARAM_LEN_CHK(field_len, input)            \
do {                                                            \
    if ((((int)field_len + 3) / 4) < ((int)strlen(input) - 2))  \
    {                                                           \
        diag_util_printf("field data or mask is too long!\n");  \
        return CPARSER_NOT_OK;                                  \
    }                                                           \
} while(0)

#define EGR_PHASE_CHK_RET(__p)  if (FT_PHASE_EGR_FT_0 == __p)   \
    {DIAG_UTIL_MPRINTF("Attribute is not support in specified phase\n"); return CPARSER_NOT_OK;}

#define IGR_SET_EGR_RET(__p, __i, __n, __v)  \
do{                         \
    EGR_PHASE_CHK_RET(__p); \
    __i->__n = __v;         \
}while(0);

#define IGR_EGR_SET(__p, __i, __e, __n, __v)  \
do{                  \
    if(FT_PHASE_EGR_FT_0 == __p)    \
        __e->__n = __v;     \
    else                    \
        __i->__n = __v;     \
}while(0);

#define IGR_PTR_EXTRACT(__p, __i, __d, __n) \
do{                                         \
    if (FT_PHASE_IGR_FT_0 == __p)           \
        __i = &__d.igrFt_0.__n;             \
    else if (FT_PHASE_IGR_FT_3 == __p)      \
        __i = &__d.igrFt_3.__n;             \
}while(0);

#define DIAG_OF_PHASE_INSTRUCTION(__idx, __t)\
do {\
    if ('m' == TOKEN_CHAR(__idx, 0))    \
        __t = OF_INS_ACT_TYPE_METER;    \
    else if ('c' == TOKEN_CHAR(__idx, 0))   \
        __t = OF_INS_ACT_TYPE_CLEARACT;     \
    else if ('w' == TOKEN_CHAR(__idx, 0))   \
    {                                       \
        if ('a' == TOKEN_CHAR(__idx, 6))    \
            __t = OF_INS_ACT_TYPE_WRITEACT; \
        else if ('m' == TOKEN_CHAR(__idx, 6))   \
            __t = OF_INS_ACT_TYPE_WRITEMETA;    \
    }                                           \
    else if ('g' == TOKEN_CHAR(__idx, 0))       \
        __t = OF_INS_ACT_TYPE_GOTOTBL;          \
} while(0);

/* Convert logical phase ID input of diagShell to chip physical phase */
#define DIAG_OF_PHASE_MAPPING(logical, physical)\
do {\
    if (logical == 0)\
        physical = FT_PHASE_IGR_FT_0;\
    if (logical == 3)\
        physical = FT_PHASE_IGR_FT_3;\
    if (logical == 4)\
        physical = FT_PHASE_EGR_FT_0;\
    if (logical == 1)\
    {\
        DIAG_UTIL_MPRINTF("For phase 1, please use 'l2/l2-entry' for the phase token.\n\n");\
        return CPARSER_NOT_OK;\
    }\
    if (logical == 2)\
    {\
        DIAG_UTIL_MPRINTF("For phase 2, please use 'l3/l3-hash-entry' for the phase token.\n\n");\
        return CPARSER_NOT_OK;\
    }\
} while(0);

/* Convert physical phase ID to logic diagShell phase */
#define DIAG_OF_PHASE_MAPPING_P2L(logical, physical)\
do {\
    if (physical == FT_PHASE_IGR_FT_0)\
        logical = 0;\
    if (physical == FT_PHASE_IGR_FT_3)\
        logical = 3;\
    if (physical == FT_PHASE_EGR_FT_0)\
        logical = 4;\
} while(0);

#define L2_HASH_CMD_PARSE(__v, __m, __dm, __midx)    \
do{ \
    if (__m != __dm) { \
        entry.type = OF_L2_FLOW_ENTRY_TYPE_SA_DA;   \
        osal_memcpy(entry.matchfield.vidMac.smac.octet, __m, ETHER_ADDR_LEN);    \
        osal_memcpy(entry.matchfield.vidMac.dmac.octet, __dm, ETHER_ADDR_LEN);    \
    } else if (!__v) {  \
        if ('s' == TOKEN_CHAR(4, 0)) {  \
            entry.type= OF_L2_FLOW_ENTRY_TYPE_SA;   \
            osal_memcpy(entry.matchfield.vidMac.smac.octet, __m, ETHER_ADDR_LEN);     \
        } else if ('d' == TOKEN_CHAR(4, 0)) {   \
            entry.type= OF_L2_FLOW_ENTRY_TYPE_DA;   \
            osal_memcpy(entry.matchfield.vidMac.dmac.octet, __m, ETHER_ADDR_LEN); }   \
    } else {                            \
        if ('s' == TOKEN_CHAR(4, 4)) {  \
            entry.type= OF_L2_FLOW_ENTRY_TYPE_VID_SA;   \
            osal_memcpy(entry.matchfield.vidMac.smac.octet, __m, ETHER_ADDR_LEN);     \
        } else if ('d' == TOKEN_CHAR(4, 4)) {   \
            entry.type= OF_L2_FLOW_ENTRY_TYPE_VID_DA;   \
            osal_memcpy(entry.matchfield.vidMac.dmac.octet, __m, ETHER_ADDR_LEN); }   \
    }   \
    if ('n' != TOKEN_CHAR(__midx, 0)) {           \
        entry.matchfield.vidMac.metadata = *metadata_ptr;           \
        entry.flags |= RTK_OF_FLAG_FLOWENTRY_MD_CMP;}    \
}while(0);

#define L2_HASH_5_TUPLE_CMD_PARSE(__typeIdx, __sip, __dip, __sport, __dport, __proto_vid)    \
do{ \
    entry.matchfield.fiveTp.sip = *__sip; \
    entry.matchfield.fiveTp.dip = *__dip; \
    entry.matchfield.fiveTp.l4_sport = *__sport; \
    entry.matchfield.fiveTp.l4_dport = *__dport; \
    if ('i' == TOKEN_CHAR(__typeIdx, 8)) { \
        entry.type = OF_L2_FLOW_ENTRY_TYPE_5_TUPLE_IPPROTO;   \
        entry.matchfield.fiveTp.fifthData.ipproto = *__proto_vid; \
    } else {                            \
        entry.type= OF_L2_FLOW_ENTRY_TYPE_5_TUPLE_VID;   \
        entry.matchfield.fiveTp.fifthData.vid = *__proto_vid; \
    }   \
}while(0);

#define L3_HASH_CMD_V4_PARSE(__i, __di, __midx)    \
do{ \
    if (__i != __di) { \
        entry.type = OF_L3_FLOW_ENTRY_TYPE_IP4_SIP_DIP;   \
        entry.sip.ipv4 = *__i;  \
        entry.dip.ipv4 = *__di; \
    } else {  \
        if ('s' == TOKEN_CHAR(4, 0)) {  \
            entry.type= OF_L3_FLOW_ENTRY_TYPE_IP4_SIP;   \
            entry.sip.ipv4 = *__i;  \
        } else if ('d' == TOKEN_CHAR(4, 0)) {   \
            entry.type= OF_L3_FLOW_ENTRY_TYPE_IP4_DIP;   \
            entry.dip.ipv4 = *__i;  }}   \
    if ('n' != TOKEN_CHAR(__midx, 0)) {           \
        entry.metadata = *metadata_ptr;           \
        entry.flags |= RTK_OF_FLAG_FLOWENTRY_MD_CMP;}    \
}while(0);

#define L3_HASH_CMD_V6_PARSE(__i, __di, __midx)    \
do{ \
    if (__i != __di) { \
        entry.type = OF_L3_FLOW_ENTRY_TYPE_IP6_SIP_DIP;   \
        DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(entry.sip.ipv6.octet, *__i), ret); \
        DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(entry.dip.ipv6.octet, *__di), ret);    \
    } else {  \
        if ('s' == TOKEN_CHAR(4, 0)) {  \
            entry.type= OF_L3_FLOW_ENTRY_TYPE_IP6_SIP;   \
            DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(entry.sip.ipv6.octet, *__i), ret); \
        } else if ('d' == TOKEN_CHAR(4, 0)) {   \
            entry.type= OF_L3_FLOW_ENTRY_TYPE_IP6_DIP;   \
            DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(entry.dip.ipv6.octet, *__di), ret); }}    \
    if ('n' != TOKEN_CHAR(__midx, 0)) {           \
        entry.metadata = *metadata_ptr;           \
        entry.flags |= RTK_OF_FLAG_FLOWENTRY_MD_CMP;}    \
}while(0);

#define WRITE_SETFIELD(__t, __v)\
do {\
    if (0 == *field_id_ptr) \
    {                       \
        IGR_EGR_SET(phase, igrwa, egrwa, set_field_0_data.field_type, __t); \
        IGR_EGR_SET(phase, igrwa, egrwa, set_field_0_data.field_data, __v); \
    }   \
    else if (1 == *field_id_ptr)    \
    {   \
        IGR_EGR_SET(phase, igrwa, egrwa, set_field_1_data.field_type, __t); \
        IGR_EGR_SET(phase, igrwa, egrwa, set_field_1_data.field_data, __v); \
    }   \
    else if (2 == *field_id_ptr)    \
    {   \
        IGR_SET_EGR_RET(phase, igrwa, set_field_2_data.field_type, __t);    \
        IGR_SET_EGR_RET(phase, igrwa, set_field_2_data.field_data, __v);    \
    }   \
    else if (3 == *field_id_ptr)    \
    {   \
        IGR_SET_EGR_RET(phase, igrwa, set_field_3_data.field_type, __t);    \
        IGR_SET_EGR_RET(phase, igrwa, set_field_3_data.field_data, __v);    \
    }   \
    else if (4 == *field_id_ptr)    \
    {   \
        IGR_SET_EGR_RET(phase, igrwa, set_field_4_data.field_type, __t);    \
        IGR_SET_EGR_RET(phase, igrwa, set_field_4_data.field_data, __v);    \
    }   \
}while(0);

static inline void DUMP_INSTRUCTION(rtk_of_flowtable_phase_t phase, rtk_of_flowIns_t ins, rtk_of_ins_action_type_t type, rtk_of_flow_id_t idx)
{
    char portStr[DIAG_UTIL_PORT_MASK_STRING_LEN];
    char macStr[32];
    char ipStr[32];
    char lp = 0;
    struct rtk_of_igrFTIns_s *igr = NULL;

    if (FT_PHASE_IGR_FT_0 == phase)
        igr = &ins.igrFt_0;
    else if (FT_PHASE_IGR_FT_3 == phase)
        igr = &ins.igrFt_3;

    DIAG_OF_PHASE_MAPPING_P2L(lp, phase);

    DIAG_UTIL_MPRINTF("phase %d entry %u instruction:\n", lp, idx);

    if (FT_PHASE_IGR_FT_0 == phase || FT_PHASE_IGR_FT_3 == phase)
    {
        if (type == OF_INS_ACT_TYPE_END || type == OF_INS_ACT_TYPE_METER)
        {
            DIAG_UTIL_MPRINTF("\tmeter:\n");
            DIAG_UTIL_MPRINTF("\t\tstate : %s\n", text_state[igr->meter_en]);
            DIAG_UTIL_MPRINTF("\t\tid    : %u\n", igr->meter_data.meter_id);
            DIAG_UTIL_MPRINTF("\t\tyellow: %s\n", text_meter_type[igr->meter_data.yellow]);
            DIAG_UTIL_MPRINTF("\t\tred   : %s\n", text_meter_type[igr->meter_data.red]);
        }
        if (type == OF_INS_ACT_TYPE_END || type == OF_INS_ACT_TYPE_CLEARACT)
        {
            DIAG_UTIL_MPRINTF("\tclear action:\n");
            DIAG_UTIL_MPRINTF("\t\tstate : %s\n", text_state[igr->clearAct_en]);
        }
        if (type == OF_INS_ACT_TYPE_END || type == OF_INS_ACT_TYPE_WRITEACT)
        {
            DIAG_UTIL_MPRINTF("\twrite action:\n");
            DIAG_UTIL_MPRINTF("\t\tstate         : %s\n", text_state[igr->writeAct_en]);
            DIAG_UTIL_MPRINTF("\t\tcp_ttl_inward : %s\n", text_state[igr->wa_data.cp_ttl_inward]);
            DIAG_UTIL_MPRINTF("\t\tpop_vlan      : %s\n", text_state[igr->wa_data.pop_vlan]);
            DIAG_UTIL_MPRINTF("\t\tpop_mpls      : %s\n", text_state[igr->wa_data.pop_mpls]);
            DIAG_UTIL_MPRINTF("\t\tpop_mpls_type : %s\n", text_pop_mpls_type[igr->wa_data.pop_mpls_type]);
            DIAG_UTIL_MPRINTF("\t\tpush_mpls: %s\n", text_state[igr->wa_data.push_mpls]);
            DIAG_UTIL_MPRINTF("\t\t\tpush_mode      : %s\n", text_push_mpls_mode[igr->wa_data.push_mpls_data.push_mode]);
            DIAG_UTIL_MPRINTF("\t\t\tvpn_type       : %s\n", text_push_mpls_vpn_type[igr->wa_data.push_mpls_data.vpn_type]);
            DIAG_UTIL_MPRINTF("\t\t\tmpls_encap_idx : %u\n", igr->wa_data.push_mpls_data.mpls_encap_idx);
            DIAG_UTIL_MPRINTF("\t\t\tetherType      : 0x%x\n", igr->wa_data.push_mpls_data.etherType + 0x8847);
            DIAG_UTIL_MPRINTF("\t\tpush_vlan: %s\n", text_state[igr->wa_data.push_vlan]);
            DIAG_UTIL_MPRINTF("\t\t\tetherType_idx : %u\n", igr->wa_data.push_vlan_data.etherType_idx);
            DIAG_UTIL_MPRINTF("\t\tcp_ttl_outward: %s\n", text_state[igr->wa_data.cp_ttl_outward]);
            DIAG_UTIL_MPRINTF("\t\tdec_mpls_ttl: %s\n", text_state[igr->wa_data.dec_mpls_ttl]);
            DIAG_UTIL_MPRINTF("\t\tdec_ip_ttl: %s\n", text_state[igr->wa_data.dec_ip_ttl]);
            DIAG_UTIL_MPRINTF("\t\tfield 0 type: %s\n", text_setfield_type[igr->wa_data.set_field_0_data.field_type]);
            DIAG_UTIL_MPRINTF("\t\tfield 0 data: ");
            switch(igr->wa_data.set_field_0_data.field_type)
            {
                case OF_IGR_FT0_SF_TYPE0_SA:
                    osal_memset(&macStr, 0, sizeof(macStr));
                    diag_util_mac2str(macStr, igr->wa_data.set_field_0_data.mac.octet);
                    DIAG_UTIL_MPRINTF("%s\n", macStr);
                    break;
                case OF_IGR_FT0_SF_TYPE0_VLAN_PRI:
                case OF_IGR_FT0_SF_TYPE0_MPLS_TC:
                case OF_IGR_FT0_SF_TYPE0_MPLS_TTL:
                case OF_IGR_FT0_SF_TYPE0_IP_DSCP:
                case OF_IGR_FT0_SF_TYPE0_IP_TTL:
                    DIAG_UTIL_MPRINTF("%u\n", igr->wa_data.set_field_0_data.field_data);
                    break;
                case OF_IGR_FT0_SF_TYPE0_NONE:
                default:
                    break;
            }
            DIAG_UTIL_MPRINTF("\n");
            DIAG_UTIL_MPRINTF("\t\tfield 1 type: %s\n", text_setfield_type[igr->wa_data.set_field_1_data.field_type]);
            DIAG_UTIL_MPRINTF("\t\tfield 1 data: ");
            switch(igr->wa_data.set_field_1_data.field_type)
            {
                case OF_IGR_FT0_SF_TYPE1_DA:
                    osal_memset(&macStr, 0, sizeof(macStr));
                    diag_util_mac2str(macStr, igr->wa_data.set_field_1_data.mac.octet);
                    DIAG_UTIL_MPRINTF("%s\n", macStr);
                    break;
                case OF_IGR_FT0_SF_TYPE1_VLAN_PRI:
                case OF_IGR_FT0_SF_TYPE1_MPLS_LABEL:
                case OF_IGR_FT0_SF_TYPE1_MPLS_TC:
                case OF_IGR_FT0_SF_TYPE1_MPLS_TTL:
                case OF_IGR_FT0_SF_TYPE1_IP_DSCP:
                case OF_IGR_FT0_SF_TYPE1_IP_TTL:
                    DIAG_UTIL_MPRINTF("%u\n", igr->wa_data.set_field_1_data.field_data);
                    break;
                case OF_IGR_FT0_SF_TYPE1_NONE:
                default:
                    break;
            }
            DIAG_UTIL_MPRINTF("\n");
            DIAG_UTIL_MPRINTF("\t\tfield 2 type: %s\n", text_setfield_type[igr->wa_data.set_field_2_data.field_type]);
            DIAG_UTIL_MPRINTF("\t\tfield 2 data: ");
            switch(igr->wa_data.set_field_2_data.field_type)
            {
                case OF_IGR_FT0_SF_TYPE2_VLAN_ID:
                case OF_IGR_FT0_SF_TYPE2_VLAN_PRI:
                case OF_IGR_FT0_SF_TYPE2_MPLS_LABEL:
                case OF_IGR_FT0_SF_TYPE2_MPLS_TC:
                case OF_IGR_FT0_SF_TYPE2_MPLS_TTL:
                case OF_IGR_FT0_SF_TYPE2_IP_DSCP:
                case OF_IGR_FT0_SF_TYPE2_IP_TTL:
                    DIAG_UTIL_MPRINTF("%u\n", igr->wa_data.set_field_2_data.field_data);
                    break;
                case OF_IGR_FT0_SF_TYPE1_NONE:
                default:
                    break;
            }
            DIAG_UTIL_MPRINTF("\n");
            DIAG_UTIL_MPRINTF("\t\tfield 3 type: %s\n", text_setfield_type[igr->wa_data.set_field_3_data.field_type]);
            DIAG_UTIL_MPRINTF("\t\tfield 3 data: ");
            switch(igr->wa_data.set_field_3_data.field_type)
            {
                case OF_IGR_FT0_SF_TYPE3_SIP:
                case OF_IGR_FT0_SF_TYPE3_DIP:
                    osal_memset(&ipStr, 0, sizeof(ipStr));
                    diag_util_ip2str(ipStr, igr->wa_data.set_field_3_data.ip);
                    DIAG_UTIL_MPRINTF("%s\n", ipStr);
                    break;
                case OF_IGR_FT0_SF_TYPE3_VLAN_PRI:
                case OF_IGR_FT0_SF_TYPE3_MPLS_LABEL:
                case OF_IGR_FT0_SF_TYPE3_MPLS_TC:
                case OF_IGR_FT0_SF_TYPE3_MPLS_TTL:
                case OF_IGR_FT0_SF_TYPE3_IP_DSCP:
                    DIAG_UTIL_MPRINTF("%u\n", igr->wa_data.set_field_3_data.field_data);
                    break;
                case OF_IGR_FT0_SF_TYPE1_NONE:
                default:
                    break;
            }
            DIAG_UTIL_MPRINTF("\n");
            DIAG_UTIL_MPRINTF("\t\tfield 4 type: %s\n", text_setfield_type[igr->wa_data.set_field_4_data.field_type]);
            DIAG_UTIL_MPRINTF("\t\tfield 4 data: ");
            switch(igr->wa_data.set_field_4_data.field_type)
            {
                case OF_IGR_FT0_SF_TYPE4_L4_SPORT:
                case OF_IGR_FT0_SF_TYPE4_L4_DPORT:
                case OF_IGR_FT0_SF_TYPE4_VLAN_PRI:
                case OF_IGR_FT0_SF_TYPE4_MPLS_LABEL:
                case OF_IGR_FT0_SF_TYPE4_MPLS_TC:
                case OF_IGR_FT0_SF_TYPE4_MPLS_TTL:
                case OF_IGR_FT0_SF_TYPE4_IP_DSCP:
                    DIAG_UTIL_MPRINTF("%u\n", igr->wa_data.set_field_4_data.field_data);
                    break;
                case OF_IGR_FT0_SF_TYPE1_NONE:
                default:
                    break;
            }
            DIAG_UTIL_MPRINTF("\n");
            DIAG_UTIL_MPRINTF("\t\tset_queue: %s\n", text_state[igr->wa_data.set_queue]);
            DIAG_UTIL_MPRINTF("\t\t\tqid: %u\n", igr->wa_data.qid);
            DIAG_UTIL_MPRINTF("\t\tgroup: %s\n", text_state[igr->wa_data.group]);
            DIAG_UTIL_MPRINTF("\t\t\tgid: %u\n", igr->wa_data.gid);
            DIAG_UTIL_MPRINTF("\t\toutput: %s\n", text_output_type[igr->wa_data.output_data.type]);
            switch(igr->wa_data.output_data.type)
            {
                case OF_OUTPUT_TYPE_PHY_PORT_EX_SRC_PORT:
                case OF_OUTPUT_TYPE_PHY_PORT:
                    DIAG_UTIL_MPRINTF("\t\t\tunit: %u\n", igr->wa_data.output_data.devID);
                    DIAG_UTIL_MPRINTF("\t\t\tport: %u\n", igr->wa_data.output_data.port);
                    break;
                case OF_OUTPUT_TYPE_TRK_PORT_EX_SRC_PORT:
                case OF_OUTPUT_TYPE_TRK_PORT:
                    DIAG_UTIL_MPRINTF("\t\t\ttrunk id: %u\n", igr->wa_data.output_data.trunk);
                    break;
                case OF_OUTPUT_TYPE_MULTI_EGR_PORT:
                case OF_OUTPUT_TYPE_FAILOVER:
                    osal_memset(&portStr, 0, sizeof(portStr));
                    diag_util_lPortMask2str(portStr, &igr->wa_data.output_data.portmask);
                    DIAG_UTIL_MPRINTF("\t\t\tportmask: %s\n", portStr);
                    break;
                case OF_OUTPUT_TYPE_TUNNEL:
                    DIAG_UTIL_MPRINTF("\t\t\tinterface: 0x%x\n", igr->wa_data.output_data.intf);
                    break;
                default:
                    break;
            }
        }
        if (type == OF_INS_ACT_TYPE_END || type == OF_INS_ACT_TYPE_WRITEMETA)
        {
            DIAG_UTIL_MPRINTF("\twrite metadata:\n");
            DIAG_UTIL_MPRINTF("\t\tstate : %s\n", text_state[igr->writeMetadata_en]);
            DIAG_UTIL_MPRINTF("\t\tdata  : 0x%x\n", igr->wm_data.data);
            DIAG_UTIL_MPRINTF("\t\tmask  : 0x%x\n", igr->wm_data.mask);
        }
        if (type == OF_INS_ACT_TYPE_END || type == OF_INS_ACT_TYPE_GOTOTBL)
        {
            DIAG_UTIL_MPRINTF("\tgoto table:\n");
            DIAG_UTIL_MPRINTF("\t\tstate : %s\n", text_state[igr->gotoTbl_en]);
            DIAG_UTIL_MPRINTF("\t\ttype  : %s\n", text_gototbl_type[igr->gt_data.type]);
            DIAG_UTIL_MPRINTF("\t\ttbl_id: %u\n", igr->gt_data.tbl_id);
        }
    }
    else if (FT_PHASE_EGR_FT_0 == phase)
    {
        if (type == OF_INS_ACT_TYPE_END || type == OF_INS_ACT_TYPE_METER)
        {
            DIAG_UTIL_MPRINTF("\tmeter:\n");
            DIAG_UTIL_MPRINTF("\t\tstate : %s\n", text_state[ins.egrFt_0.meter_en]);
            DIAG_UTIL_MPRINTF("\t\tid    : %u\n", ins.egrFt_0.meter_data.meter_id);
            DIAG_UTIL_MPRINTF("\t\tyellow: %s\n", text_meter_type[ins.egrFt_0.meter_data.yellow]);
            DIAG_UTIL_MPRINTF("\t\tred   : %s\n", text_meter_type[ins.egrFt_0.meter_data.red]);
        }
        if (type == OF_INS_ACT_TYPE_END || type == OF_INS_ACT_TYPE_WRITEACT)
        {
            DIAG_UTIL_MPRINTF("\twrite action:\n");
            DIAG_UTIL_MPRINTF("\t\tstate         : %s\n", text_state[ins.egrFt_0.writeAct_en]);
            DIAG_UTIL_MPRINTF("\t\tpop_vlan      : %s\n", text_state[ins.egrFt_0.wa_data.pop_vlan]);
            DIAG_UTIL_MPRINTF("\t\tpush_vlan: %s\n", text_state[ins.egrFt_0.wa_data.push_vlan]);
            DIAG_UTIL_MPRINTF("\t\t\tetherType_idx : %u\n", ins.egrFt_0.wa_data.push_vlan_data.etherType_idx);
            DIAG_UTIL_MPRINTF("\t\tfield 0 type: %s\n", text_setfield_type[ins.egrFt_0.wa_data.set_field_0_data.field_type]);
            DIAG_UTIL_MPRINTF("\t\tfield 0 data: ");
            switch(ins.egrFt_0.wa_data.set_field_0_data.field_type)
            {
                case OF_EGR_FT_SF_TYPE0_VLAN_PRI:
                case OF_EGR_FT_SF_TYPE0_IP_DSCP:
                    DIAG_UTIL_MPRINTF("%u", ins.egrFt_0.wa_data.set_field_0_data.field_data);
                    break;
                case OF_IGR_FT0_SF_TYPE0_NONE:
                default:
                    break;
            }
            DIAG_UTIL_MPRINTF("\n");
            DIAG_UTIL_MPRINTF("\t\tfield 1 type: %s\n", text_setfield_type[ins.egrFt_0.wa_data.set_field_1_data.field_type]);
            DIAG_UTIL_MPRINTF("\t\tfield 1 data: ");
            switch(ins.egrFt_0.wa_data.set_field_1_data.field_type)
            {
                case OF_EGR_FT_SF_TYPE1_VLAN_ID:
                    DIAG_UTIL_MPRINTF("%u", ins.egrFt_0.wa_data.set_field_1_data.field_data);
                    break;
                case OF_IGR_FT0_SF_TYPE0_NONE:
                default:
                    break;
            }
            DIAG_UTIL_MPRINTF("\n");
            DIAG_UTIL_MPRINTF("\t\toutput: %s\n", text_egr_output_type[ins.egrFt_0.wa_data.drop]);
            DIAG_UTIL_MPRINTF("\n");
        }
    }

    return;
}

static inline void DUMP_L2_INSTRUCTION(rtk_of_l2FlowEntry_t entry, rtk_of_ins_action_type_t type)
{
    char portStr[DIAG_UTIL_PORT_MASK_STRING_LEN];
    char macStr[32], ipStr[16];

    DIAG_UTIL_MPRINTF("L2 Flow Entry:\n");
    DIAG_UTIL_MPRINTF("\ttype: %s\n", text_l2_flow_entry_type[entry.type]);

    if ((entry.type == OF_L2_FLOW_ENTRY_TYPE_5_TUPLE_IPPROTO) || (entry.type == OF_L2_FLOW_ENTRY_TYPE_5_TUPLE_VID))
    {
        diag_util_ip2str(ipStr, entry.matchfield.fiveTp.sip);
        DIAG_UTIL_MPRINTF("\tsip: %s\n", ipStr);
        diag_util_ip2str(ipStr, entry.matchfield.fiveTp.dip);
        DIAG_UTIL_MPRINTF("\tdip: %s\n", ipStr);
        DIAG_UTIL_MPRINTF("\tL4_sport: %u\n", entry.matchfield.fiveTp.l4_sport);
        DIAG_UTIL_MPRINTF("\tL4_dport: %u\n", entry.matchfield.fiveTp.l4_dport);
        if (entry.type == OF_L2_FLOW_ENTRY_TYPE_5_TUPLE_IPPROTO)
            DIAG_UTIL_MPRINTF("\tipproto: %u\n", entry.matchfield.fiveTp.fifthData.ipproto);
        else
            DIAG_UTIL_MPRINTF("\tvid: %u\n", entry.matchfield.fiveTp.fifthData.vid);
    }
    else
    {
        DIAG_UTIL_MPRINTF("\tvid : %u\n", entry.matchfield.vidMac.vid);
        osal_memset(&macStr, 0, sizeof(macStr));
        diag_util_mac2str(macStr, entry.matchfield.vidMac.smac.octet);
        DIAG_UTIL_MPRINTF("\tsmac: %s\n", macStr);
        osal_memset(&macStr, 0, sizeof(macStr));
        diag_util_mac2str(macStr, entry.matchfield.vidMac.dmac.octet);
        DIAG_UTIL_MPRINTF("\tdmac: %s\n", macStr);
        DIAG_UTIL_MPRINTF("\tmetadata match: %s\n", (entry.flags & RTK_OF_FLAG_FLOWENTRY_MD_CMP) ? "enabled" : "disabled");
        DIAG_UTIL_MPRINTF("\tmetadata: 0x%x\n", entry.matchfield.vidMac.metadata);
    }

    if (type == OF_INS_ACT_TYPE_END || type == OF_INS_ACT_TYPE_CLEARACT)
    {
        DIAG_UTIL_MPRINTF("\tclear action:\n");
        DIAG_UTIL_MPRINTF("\t\tstate : %s\n", text_state[entry.ins.clearAct_en]);
    }
    if (type == OF_INS_ACT_TYPE_END || type == OF_INS_ACT_TYPE_WRITEACT)
    {
        DIAG_UTIL_MPRINTF("\twrite action:\n");
        DIAG_UTIL_MPRINTF("\t\tstate: %s\n", text_state[entry.ins.writeAct_en]);
        DIAG_UTIL_MPRINTF("\t\tpush_vlan: %s\n", text_state[entry.ins.wa_data.push_vlan]);
        DIAG_UTIL_MPRINTF("\t\t\tetherType_idx : %u\n", entry.ins.wa_data.push_vlan_data.etherType_idx);
        DIAG_UTIL_MPRINTF("\t\tfield 0 type: %s\n", text_setfield_type[entry.ins.wa_data.set_field_0_data.field_type]);
        DIAG_UTIL_MPRINTF("\t\tfield 0 data: ");
        switch(entry.ins.wa_data.set_field_0_data.field_type)
        {
            case OF_IGR_FT_SF_TYPE0_VLAN_ID:
            case OF_IGR_FT_SF_TYPE0_IP_DSCP:
                DIAG_UTIL_MPRINTF("%u\n", entry.ins.wa_data.set_field_0_data.field_data);
                break;
            case OF_IGR_FT_SF_TYPE0_NONE:
            default:
                break;
        }
        DIAG_UTIL_MPRINTF("\n");
        DIAG_UTIL_MPRINTF("\t\tfield 1 type: %s\n", text_setfield_type[entry.ins.wa_data.set_field_1_data.field_type]);
        DIAG_UTIL_MPRINTF("\t\tfield 1 data: ");
        switch(entry.ins.wa_data.set_field_1_data.field_type)
        {
            case OF_IGR_FT_SF_TYPE1_VLAN_PRI:
                DIAG_UTIL_MPRINTF("%u\n", entry.ins.wa_data.set_field_1_data.field_data);
                break;
            case OF_IGR_FT_SF_TYPE1_NONE:
            default:
                break;
        }
        DIAG_UTIL_MPRINTF("\n");
        DIAG_UTIL_MPRINTF("\t\tset_queue: %s\n", text_state[entry.ins.wa_data.set_queue]);
        DIAG_UTIL_MPRINTF("\t\t\tqid: %u\n", entry.ins.wa_data.qid);
        DIAG_UTIL_MPRINTF("\t\toutput: type (%s)\n", text_output_type[entry.ins.wa_data.output_data.type]);
        switch(entry.ins.wa_data.output_data.type)
        {
            case OF_IGR_FT1_ACT_OUTPUT_TYPE_PHY_PORT:
                DIAG_UTIL_MPRINTF("\t\t\tunit: %u\n", entry.ins.wa_data.output_data.devID);
                DIAG_UTIL_MPRINTF("\t\t\tport: %u\n", entry.ins.wa_data.output_data.port);
                break;
            case OF_IGR_FT1_ACT_OUTPUT_TYPE_TRK_PORT:
                DIAG_UTIL_MPRINTF("\t\t\ttrunk id: %u\n", entry.ins.wa_data.output_data.trunk);
                break;
            case OF_IGR_FT1_ACT_OUTPUT_TYPE_MULT_EGR_PORT:
                osal_memset(&portStr, 0, sizeof(portStr));
                diag_util_lPortMask2str(portStr, &entry.ins.wa_data.output_data.portmask);
                DIAG_UTIL_MPRINTF("\t\t\tportmask: %s\n", portStr);
                break;
            case OF_OUTPUT_TYPE_TUNNEL:
                DIAG_UTIL_MPRINTF("\t\t\tinterface: 0x%x\n", entry.ins.wa_data.output_data.intf);
                break;
            default:
                break;
        }
    }
    if (type == OF_INS_ACT_TYPE_END || type == OF_INS_ACT_TYPE_WRITEMETA)
    {
        DIAG_UTIL_MPRINTF("\twrite metadata:\n");
        DIAG_UTIL_MPRINTF("\t\tstate : %s\n", text_state[entry.ins.writeMetadata_en]);
        DIAG_UTIL_MPRINTF("\t\tdata  : 0x%x\n", entry.ins.wm_data.data);
        DIAG_UTIL_MPRINTF("\t\tmask  : 0x%x\n", entry.ins.wm_data.mask);
    }
    if (type == OF_INS_ACT_TYPE_END || type == OF_INS_ACT_TYPE_GOTOTBL)
    {
        DIAG_UTIL_MPRINTF("\tgoto table:\n");
        DIAG_UTIL_MPRINTF("\t\tstate : %s\n", text_state[entry.ins.gotoTbl_en]);
        DIAG_UTIL_MPRINTF("\t\ttype  : %s\n", text_gototbl_type[entry.ins.gt_data.type]);
        DIAG_UTIL_MPRINTF("\t\ttbl_id: %u\n", entry.ins.gt_data.tbl_id);
    }

    return;
}

static inline void DUMP_ACTION_BUCKET(rtk_of_actionBucket_t ab, uint32 idx)
{
    char portStr[DIAG_UTIL_PORT_MASK_STRING_LEN];
    char macStr[32];

    DIAG_UTIL_MPRINTF("Openflow group action bucket %u:\n", idx);
    DIAG_UTIL_MPRINTF("\tcp_ttl_inward : %s\n", text_state[ab.cp_ttl_inward]);
    DIAG_UTIL_MPRINTF("\tpop_vlan      : %s\n", text_state[ab.pop_vlan]);
    DIAG_UTIL_MPRINTF("\tpop_mpls      : %s\n", text_state[ab.pop_mpls]);
    DIAG_UTIL_MPRINTF("\tpop_mpls_type : %s\n", text_pop_mpls_type[ab.pop_mpls_type]);
    DIAG_UTIL_MPRINTF("\tpush_mpls: %s\n", text_state[ab.push_mpls]);
    DIAG_UTIL_MPRINTF("\t\tpush_mode      : %s\n", text_push_mpls_mode[ab.push_mpls_data.push_mode]);
    DIAG_UTIL_MPRINTF("\t\tvpn_type       : %s\n", text_push_mpls_vpn_type[ab.push_mpls_data.vpn_type]);
    DIAG_UTIL_MPRINTF("\t\tmpls_encap_idx : %u\n", ab.push_mpls_data.mpls_encap_idx);
    DIAG_UTIL_MPRINTF("\t\tetherType      : 0x%x\n", ab.push_mpls_data.etherType + 0x8847);
    DIAG_UTIL_MPRINTF("\tpush_vlan: %s\n", text_state[ab.push_vlan]);
    DIAG_UTIL_MPRINTF("\t\tetherType_idx : %u\n", ab.push_vlan_data.etherType_idx);
    DIAG_UTIL_MPRINTF("\tcp_ttl_outward: %s\n", text_state[ab.cp_ttl_outward]);
    DIAG_UTIL_MPRINTF("\tdec_mpls_ttl: %s\n", text_state[ab.dec_mpls_ttl]);
    DIAG_UTIL_MPRINTF("\tdec_ip_ttl: %s\n", text_state[ab.dec_ip_ttl]);
    DIAG_UTIL_MPRINTF("\tfield 0 type: %s\n", text_setfield_type[ab.set_field_0_data.field_type]);
    DIAG_UTIL_MPRINTF("\tfield 0 data: ");
    switch(ab.set_field_0_data.field_type)
    {
        case OF_IGR_FT0_SF_TYPE0_SA:
            osal_memset(&macStr, 0, sizeof(macStr));
            diag_util_mac2str(macStr, ab.set_field_0_data.mac.octet);
            DIAG_UTIL_MPRINTF("%s\n", macStr);
            break;
        case OF_IGR_FT0_SF_TYPE0_VLAN_PRI:
        case OF_IGR_FT0_SF_TYPE0_MPLS_TC:
        case OF_IGR_FT0_SF_TYPE0_MPLS_TTL:
        case OF_IGR_FT0_SF_TYPE0_IP_DSCP:
        case OF_IGR_FT0_SF_TYPE0_IP_TTL:
            DIAG_UTIL_MPRINTF("%u\n", ab.set_field_0_data.field_data);
            break;
        case OF_IGR_FT0_SF_TYPE0_NONE:
        default:
            break;
    }
    DIAG_UTIL_MPRINTF("\n");
    DIAG_UTIL_MPRINTF("\tfield 1 type: %s\n", text_setfield_type[ab.set_field_1_data.field_type]);
    DIAG_UTIL_MPRINTF("\tfield 1 data: ");
    switch(ab.set_field_1_data.field_type)
    {
        case OF_IGR_FT0_SF_TYPE1_DA:
            osal_memset(&macStr, 0, sizeof(macStr));
            diag_util_mac2str(macStr, ab.set_field_1_data.mac.octet);
            DIAG_UTIL_MPRINTF("%s\n", macStr);
            break;
        case OF_IGR_FT0_SF_TYPE1_VLAN_PRI:
        case OF_IGR_FT0_SF_TYPE1_MPLS_LABEL:
        case OF_IGR_FT0_SF_TYPE1_MPLS_TC:
        case OF_IGR_FT0_SF_TYPE1_MPLS_TTL:
        case OF_IGR_FT0_SF_TYPE1_IP_DSCP:
        case OF_IGR_FT0_SF_TYPE1_IP_TTL:
            DIAG_UTIL_MPRINTF("%u\n", ab.set_field_1_data.field_data);
            break;
        case OF_IGR_FT0_SF_TYPE1_NONE:
        default:
            break;
    }
    DIAG_UTIL_MPRINTF("\n");
    DIAG_UTIL_MPRINTF("\tfield 2 type: %s\n", text_setfield_type[ab.set_field_2_data.field_type]);
    DIAG_UTIL_MPRINTF("\tfield 2 data: ");
    switch(ab.set_field_2_data.field_type)
    {
        case OF_IGR_FT0_SF_TYPE2_VLAN_ID:
        case OF_IGR_FT0_SF_TYPE2_VLAN_PRI:
        case OF_IGR_FT0_SF_TYPE2_MPLS_LABEL:
        case OF_IGR_FT0_SF_TYPE2_MPLS_TC:
        case OF_IGR_FT0_SF_TYPE2_MPLS_TTL:
        case OF_IGR_FT0_SF_TYPE2_IP_DSCP:
        case OF_IGR_FT0_SF_TYPE2_IP_TTL:
            DIAG_UTIL_MPRINTF("%u\n", ab.set_field_2_data.field_data);
            break;
        case OF_IGR_FT0_SF_TYPE1_NONE:
        default:
            break;
    }
    DIAG_UTIL_MPRINTF("\n");
    DIAG_UTIL_MPRINTF("\tset_queue: %s\n", text_state[ab.set_queue]);
    DIAG_UTIL_MPRINTF("\t\tqid: %u\n", ab.qid);
    DIAG_UTIL_MPRINTF("\toutput: %s\n", text_output_type[ab.output_data.type]);
    switch(ab.output_data.type)
    {
        case OF_OUTPUT_TYPE_PHY_PORT_EX_SRC_PORT:
        case OF_OUTPUT_TYPE_PHY_PORT:
            DIAG_UTIL_MPRINTF("\t\tunit: %u\n", ab.output_data.devID);
            DIAG_UTIL_MPRINTF("\t\tport: %u\n", ab.output_data.port);
            break;
        case OF_OUTPUT_TYPE_TRK_PORT_EX_SRC_PORT:
        case OF_OUTPUT_TYPE_TRK_PORT:
            DIAG_UTIL_MPRINTF("\t\ttrunk id: %u\n", ab.output_data.trunk);
            break;
        case OF_OUTPUT_TYPE_MULTI_EGR_PORT:
        case OF_OUTPUT_TYPE_FAILOVER:
            osal_memset(&portStr, 0, sizeof(portStr));
            diag_util_lPortMask2str(portStr, &ab.output_data.portmask);
            DIAG_UTIL_MPRINTF("\t\tportmask: %s\n", portStr);
            break;
        case OF_OUTPUT_TYPE_TUNNEL:
            DIAG_UTIL_MPRINTF("\t\tinterface: 0x%x\n", ab.output_data.intf);
            break;
        default:
            break;
    }

    return;
}

static inline void DUMP_L3CAM_INS(rtk_of_igrFT2Ins_t ins, rtk_of_ins_action_type_t type)
{
    char portStr[DIAG_UTIL_PORT_MASK_STRING_LEN];
    char macStr[32];

    if (type == OF_INS_ACT_TYPE_END || type == OF_INS_ACT_TYPE_CLEARACT)
    {
        DIAG_UTIL_MPRINTF("\tclear action:\n");
        DIAG_UTIL_MPRINTF("\t\tstate : %s\n", text_state[ins.clearAct_en]);
    }
    if (type == OF_INS_ACT_TYPE_END || type == OF_INS_ACT_TYPE_WRITEACT)
    {
        DIAG_UTIL_MPRINTF("\twrite action:\n");
        DIAG_UTIL_MPRINTF("\t\tstate: %s\n", text_state[ins.writeAct_en]);
        DIAG_UTIL_MPRINTF("\t\tpush_vlan: %s\n", text_state[ins.wa_data.push_vlan]);
        DIAG_UTIL_MPRINTF("\t\tetherType_idx : %u\n", ins.wa_data.push_vlan_data.etherType_idx);
        DIAG_UTIL_MPRINTF("\t\tdec_ip_ttl : %s\n", text_state[ins.wa_data.dec_ip_ttl]);
        DIAG_UTIL_MPRINTF("\t\tfield 0 type: %s\n", text_setfield_type[ins.wa_data.set_field_0_data.field_type]);
        DIAG_UTIL_MPRINTF("\t\tfield 0 data: ");
            switch(ins.wa_data.set_field_0_data.field_type)
            {
                case OF_IGR_FT2_ACT_SF_TYPE0_SA:
                    osal_memset(&macStr, 0, sizeof(macStr));
                    diag_util_mac2str(macStr, ins.wa_data.set_field_0_data.mac.octet);
                    DIAG_UTIL_MPRINTF("%s\n", macStr);
                    break;
                case OF_IGR_FT2_ACT_SF_TYPE0_VLAN_PRI:
                case OF_IGR_FT2_ACT_SF_TYPE0_IP_DSCP:
                    DIAG_UTIL_MPRINTF("%u\n", ins.wa_data.set_field_0_data.field_data);
                    break;
                case OF_IGR_FT2_ACT_SF_TYPE0_END:
                default:
                    break;
            }
            DIAG_UTIL_MPRINTF("\n");
            DIAG_UTIL_MPRINTF("\t\tfield 1 type: %s\n", text_setfield_type[ins.wa_data.set_field_1_data.field_type]);
            DIAG_UTIL_MPRINTF("\t\tfield 1 data: ");
            switch(ins.wa_data.set_field_1_data.field_type)
            {
                case OF_IGR_FT2_ACT_SF_TYPE1_DA:
                    osal_memset(&macStr, 0, sizeof(macStr));
                    diag_util_mac2str(macStr, ins.wa_data.set_field_1_data.mac.octet);
                    DIAG_UTIL_MPRINTF("%s\n", macStr);
                    break;
                case OF_IGR_FT2_ACT_SF_TYPE1_VLAN_PRI:
                case OF_IGR_FT2_ACT_SF_TYPE1_IP_DSCP:
                    DIAG_UTIL_MPRINTF("%u\n", ins.wa_data.set_field_1_data.field_data);
                    break;
                case OF_IGR_FT2_ACT_SF_TYPE1_NONE:
                default:
                    break;
            }
            DIAG_UTIL_MPRINTF("\n");
            DIAG_UTIL_MPRINTF("\t\tfield 2 type: %s\n", text_setfield_type[ins.wa_data.set_field_2_data.field_type]);
            DIAG_UTIL_MPRINTF("\t\tfield 2 data: ");
            switch(ins.wa_data.set_field_2_data.field_type)
            {
                case OF_IGR_FT2_ACT_SF_TYPE2_VLAN_ID:
                case OF_IGR_FT2_ACT_SF_TYPE2_VLAN_PRI:
                case OF_IGR_FT2_ACT_SF_TYPE2_IP_DSCP:
                    DIAG_UTIL_MPRINTF("%u\n", ins.wa_data.set_field_2_data.field_data);
                    break;
                case OF_IGR_FT2_ACT_SF_TYPE2_NONE:
                default:
                    break;
            }
            DIAG_UTIL_MPRINTF("\n");
        DIAG_UTIL_MPRINTF("\t\tset_queue: %s\n", text_state[ins.wa_data.set_queue]);
        DIAG_UTIL_MPRINTF("\t\t\tqid: %u\n", ins.wa_data.qid);
        DIAG_UTIL_MPRINTF("\t\tgroup: %s\n", text_state[ins.wa_data.group]);
        DIAG_UTIL_MPRINTF("\t\t\tgid: %u\n", ins.wa_data.gid);
        DIAG_UTIL_MPRINTF("\t\toutput: type (%s)\n", text_output_type[ins.wa_data.output_data.type]);
        switch(ins.wa_data.output_data.type)
        {
            case OF_OUTPUT_TYPE_PHY_PORT_EX_SRC_PORT:
            case OF_OUTPUT_TYPE_PHY_PORT:
                DIAG_UTIL_MPRINTF("\t\t\tunit: %u\n", ins.wa_data.output_data.devID);
                DIAG_UTIL_MPRINTF("\t\t\tport: %u\n", ins.wa_data.output_data.port);
                break;
            case OF_OUTPUT_TYPE_TRK_PORT_EX_SRC_PORT:
            case OF_OUTPUT_TYPE_TRK_PORT:
                DIAG_UTIL_MPRINTF("\t\t\ttrunk id: %u\n", ins.wa_data.output_data.trunk);
                break;
            case OF_OUTPUT_TYPE_MULTI_EGR_PORT:
            case OF_OUTPUT_TYPE_FAILOVER:
                osal_memset(&portStr, 0, sizeof(portStr));
                diag_util_lPortMask2str(portStr, &ins.wa_data.output_data.portmask);
                DIAG_UTIL_MPRINTF("\t\t\tportmask: %s\n", portStr);
                break;
            case OF_OUTPUT_TYPE_TUNNEL:
                DIAG_UTIL_MPRINTF("\t\t\tinterface: 0x%x\n", ins.wa_data.output_data.intf);
                break;
            default:
                break;
        }
    }

    if (type == OF_INS_ACT_TYPE_END || type == OF_INS_ACT_TYPE_WRITEMETA)
    {
        DIAG_UTIL_MPRINTF("\twrite metadata:\n");
        DIAG_UTIL_MPRINTF("\t\tstate : %s\n", text_state[ins.writeMetadata_en]);
        DIAG_UTIL_MPRINTF("\t\tdata  : 0x%x\n", ins.wm_data.data);
        DIAG_UTIL_MPRINTF("\t\tmask  : 0x%x\n", ins.wm_data.mask);
    }
    if (type == OF_INS_ACT_TYPE_END || type == OF_INS_ACT_TYPE_GOTOTBL)
    {
        DIAG_UTIL_MPRINTF("\tgoto table:\n");
        DIAG_UTIL_MPRINTF("\t\tstate : %s\n", text_state[ins.gotoTbl_en]);
        DIAG_UTIL_MPRINTF("\t\ttype  : %s\n", text_gototbl_type[ins.gt_data.type]);
        DIAG_UTIL_MPRINTF("\t\ttbl_id: %u\n", ins.gt_data.tbl_id);
    }

    return;
}

static inline void DUMP_L3CAM_FLOWENTRY(uint32 idx, rtk_of_l3CamFlowEntry_t entry, rtk_of_ins_action_type_t type)
{
    char ipStr[128];

    DIAG_UTIL_MPRINTF("L3 entry index %u\n", idx);
    DIAG_UTIL_MPRINTF("\ttype :%s\n", text_l3_flow_entry_type[entry.type]);

    if (entry.type == OF_L3_FLOW_ENTRY_TYPE_IP4_SIP || entry.type == OF_L3_FLOW_ENTRY_TYPE_IP4_SIP_DIP)
    {
        osal_memset(ipStr, 0, sizeof(ipStr));
        diag_util_ip2str(ipStr, entry.sip.ipv4);
        DIAG_UTIL_MPRINTF("\tSIP :%s\n", ipStr);
        osal_memset(ipStr, 0, sizeof(ipStr));
        diag_util_ip2str(ipStr, entry.sip_msk.ipv4);
        DIAG_UTIL_MPRINTF("\tSIP Mask :%s\n", ipStr);
    }
    if (entry.type == OF_L3_FLOW_ENTRY_TYPE_IP4_DIP || entry.type == OF_L3_FLOW_ENTRY_TYPE_IP4_SIP_DIP)
    {
        osal_memset(ipStr, 0, sizeof(ipStr));
        diag_util_ip2str(ipStr, entry.dip.ipv4);
        DIAG_UTIL_MPRINTF("\tDIP :%s\n", ipStr);
        osal_memset(ipStr, 0, sizeof(ipStr));
        diag_util_ip2str(ipStr, entry.dip_msk.ipv4);
        DIAG_UTIL_MPRINTF("\tDIP Mask :%s\n", ipStr);
    }
    if (entry.type == OF_L3_FLOW_ENTRY_TYPE_IP6_SIP || entry.type == OF_L3_FLOW_ENTRY_TYPE_IP6_SIP_DIP)
    {
        osal_memset(ipStr, 0, sizeof(ipStr));
        diag_util_ipv62str(ipStr, entry.sip.ipv6.octet);
        DIAG_UTIL_MPRINTF("\tSIP :%s\n", ipStr);
        osal_memset(ipStr, 0, sizeof(ipStr));
        diag_util_ipv62str(ipStr, entry.sip_msk.ipv6.octet);
        DIAG_UTIL_MPRINTF("\tSIP Mask:%s\n", ipStr);
    }
    if (entry.type == OF_L3_FLOW_ENTRY_TYPE_IP6_DIP || entry.type == OF_L3_FLOW_ENTRY_TYPE_IP6_SIP_DIP)
    {
        osal_memset(ipStr, 0, sizeof(ipStr));
        diag_util_ipv62str(ipStr, entry.dip.ipv6.octet);
        DIAG_UTIL_MPRINTF("\tDIP :%s\n", ipStr);
        osal_memset(ipStr, 0, sizeof(ipStr));
        diag_util_ipv62str(ipStr, entry.dip_msk.ipv6.octet);
        DIAG_UTIL_MPRINTF("\tDIP Mask:%s\n", ipStr);
    }

    DIAG_UTIL_MPRINTF("\tmetadata      :0x%x\n", entry.metadata);
    DIAG_UTIL_MPRINTF("\tmetadata mask :0x%x\n", entry.metadata_msk);
    DUMP_L3CAM_INS(entry.ins, type);

    return;
}

static inline void DUMP_L3CAM_HASHENTRY(rtk_of_l3HashFlowEntry_t entry, rtk_of_ins_action_type_t type)
{
    char ipStr[128];

    DIAG_UTIL_MPRINTF("L3 hash entry\n");
    DIAG_UTIL_MPRINTF("\ttype :%s\n", text_l3_flow_entry_type[entry.type]);

    if (entry.type == OF_L3_FLOW_ENTRY_TYPE_IP4_SIP || entry.type == OF_L3_FLOW_ENTRY_TYPE_IP4_SIP_DIP)
    {
        osal_memset(ipStr, 0, sizeof(ipStr));
        diag_util_ip2str(ipStr, entry.sip.ipv4);
        DIAG_UTIL_MPRINTF("\tSIP :%s\n", ipStr);
    }
    if (entry.type == OF_L3_FLOW_ENTRY_TYPE_IP4_DIP || entry.type == OF_L3_FLOW_ENTRY_TYPE_IP4_SIP_DIP)
    {
        osal_memset(ipStr, 0, sizeof(ipStr));
        diag_util_ip2str(ipStr, entry.dip.ipv4);
        DIAG_UTIL_MPRINTF("\tDIP :%s\n", ipStr);
    }
    if (entry.type == OF_L3_FLOW_ENTRY_TYPE_IP6_SIP || entry.type == OF_L3_FLOW_ENTRY_TYPE_IP6_SIP_DIP)
    {
        osal_memset(ipStr, 0, sizeof(ipStr));
        diag_util_ipv62str(ipStr, entry.sip.ipv6.octet);
        DIAG_UTIL_MPRINTF("\tSIP :%s\n", ipStr);
    }
    if (entry.type == OF_L3_FLOW_ENTRY_TYPE_IP6_DIP || entry.type == OF_L3_FLOW_ENTRY_TYPE_IP6_SIP_DIP)
    {
        osal_memset(ipStr, 0, sizeof(ipStr));
        diag_util_ipv62str(ipStr, entry.dip.ipv6.octet);
        DIAG_UTIL_MPRINTF("\tDIP :%s\n", ipStr);
    }

    DIAG_UTIL_MPRINTF("\tflag          :0x%x\n", entry.flags);
    DIAG_UTIL_MPRINTF("\tmetadata      :0x%x\n", entry.metadata);
    DUMP_L3CAM_INS(entry.ins, type);

    return;
}

#ifdef CMD_OPENFLOW_GET_CLASSIFIER_PORT_PORT_ALL
/*
 * openflow get classifier port ( <PORT_LIST:port> | all )
 */
cparser_result_t
cparser_cmd_openflow_get_classifier_port_port_all(
    cparser_context_t *context,
    char **port_ptr)
{
    uint32 unit;
    int32 ret;
    diag_portlist_t portlist;
    rtk_of_classifierData_t data;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&portlist, 0 , sizeof(portlist));
    osal_memset(&data, 0 , sizeof(data));
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_MPRINTF("Classifier Port: \n");
    DIAG_UTIL_PORTMASK_SCAN(portlist, data.port)
    {
        DIAG_UTIL_ERR_CHK(rtk_of_classifier_get(unit, OF_CLASSIFIER_TYPE_PORT, &data), ret);
        DIAG_UTIL_MPRINTF("Port %2d: %s\n", data.port, text_state[data.enable]);
    }
    DIAG_UTIL_MPRINTF("\n");

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_get_classifier_port_port_all */
#endif

#ifdef CMD_OPENFLOW_GET_CLASSIFIER_VLAN_VID_ALL
/*
 * openflow get classifier vlan ( <MASK_LIST:vid> | all )
 */
cparser_result_t
cparser_cmd_openflow_get_classifier_vlan_vid_all(
    cparser_context_t *context,
    char **vid_ptr)
{
    uint32 unit;
    int32 ret;
    diag_mask_t mask;
    rtk_of_classifierData_t data;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&mask, 0 , sizeof(mask));
    osal_memset(&data, 0 , sizeof(data));
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK(mask, 4, DIAG_MASKTYPE_VLAN), ret);
    DIAG_UTIL_MPRINTF("Classifier VLAN: \n");
    DIAG_UTIL_MASK_SCAN(mask, data.vlan)
    {
        DIAG_UTIL_ERR_CHK(rtk_of_classifier_get(unit, OF_CLASSIFIER_TYPE_VLAN, &data), ret);
        DIAG_UTIL_MPRINTF("VLAN %4d: %s\n", data.vlan, text_state[data.enable]);
    }
    DIAG_UTIL_MPRINTF("\n");

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_get_classifier_vlan_vid_all */
#endif

#ifdef CMD_OPENFLOW_GET_CLASSIFIER_VLAN_AND_PORT_VID_ALL
/*
 * openflow get classifier vlan-and-port ( <MASK_LIST:vid> | all )
 */
cparser_result_t
cparser_cmd_openflow_get_classifier_vlan_and_port_vid_all(
    cparser_context_t *context,
    char **vid_ptr)
{
    uint32 unit;
    int32 ret;
    diag_mask_t mask;
    rtk_of_classifierData_t data;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&mask, 0 , sizeof(mask));
    osal_memset(&data, 0 , sizeof(data));
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK(mask, 4, DIAG_MASKTYPE_VLAN), ret);
    DIAG_UTIL_MPRINTF("Classifier VLAN and Port: \n");
    DIAG_UTIL_MASK_SCAN(mask, data.vlan)
    {
        DIAG_UTIL_ERR_CHK(rtk_of_classifier_get(unit, OF_CLASSIFIER_TYPE_VLAN_AND_PORT, &data), ret);
        DIAG_UTIL_MPRINTF("VLAN %4d: %s\n", data.vlan, text_state[data.enable]);
    }
    DIAG_UTIL_MPRINTF("\n");

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_get_classifier_vlan_and_port_vid_all */
#endif

#ifdef CMD_OPENFLOW_SET_CLASSIFIER_PORT_PORT_ALL_STATE_DISABLE_ENABLE
/*
 * openflow set classifier port ( <PORT_LIST:port> | all ) state ( disable | enable )
 */
cparser_result_t
cparser_cmd_openflow_set_classifier_port_port_all_state_disable_enable(
    cparser_context_t *context,
    char **port_ptr)
{
    uint32 unit;
    int32 ret;
    diag_portlist_t portlist;
    rtk_of_classifierData_t data;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&data, 0 , sizeof(rtk_of_classifierData_t));
    osal_memset(&portlist, 0 , sizeof(portlist));
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PARSE_STATE(6, data.enable);
    DIAG_UTIL_MPRINTF("Classifier Port: \n");
    DIAG_UTIL_PORTMASK_SCAN(portlist, data.port)
    {
        DIAG_UTIL_ERR_CHK(rtk_of_classifier_set(unit, OF_CLASSIFIER_TYPE_PORT, data), ret);
        DIAG_UTIL_MPRINTF("Port %2d: %s\n", data.port, text_state[data.enable]);
    }
    DIAG_UTIL_MPRINTF("\n");

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_classifier_port_port_all_state_disable_enable */
#endif

#ifdef CMD_OPENFLOW_SET_CLASSIFIER_VLAN_VID_ALL_STATE_DISABLE_ENABLE
/*
 * openflow set classifier vlan ( <MASK_LIST:vid> | all ) state ( disable | enable )
 */
cparser_result_t
cparser_cmd_openflow_set_classifier_vlan_vid_all_state_disable_enable(
    cparser_context_t *context,
    char **vid_ptr)
{
    uint32 unit;
    int32 ret;
    diag_mask_t mask;
    rtk_of_classifierData_t data;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&mask, 0 , sizeof(mask));
    osal_memset(&data, 0 , sizeof(data));
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK(mask, 4, DIAG_MASKTYPE_VLAN), ret);
    DIAG_UTIL_PARSE_STATE(6, data.enable);
    DIAG_UTIL_MPRINTF("Classifier VLAN: \n");
    DIAG_UTIL_MASK_SCAN(mask, data.vlan)
    {
        DIAG_UTIL_ERR_CHK(rtk_of_classifier_set(unit, OF_CLASSIFIER_TYPE_VLAN, data), ret);
        DIAG_UTIL_MPRINTF("VLAN %4d: %s\n", data.vlan, text_state[data.enable]);
    }
    DIAG_UTIL_MPRINTF("\n");

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_classifier_vlan_vid_all_state_disable_enable */
#endif

#ifdef CMD_OPENFLOW_SET_CLASSIFIER_VLAN_AND_PORT_VID_ALL_STATE_DISABLE_ENABLE
/*
 * openflow set classifier vlan-and-port ( <MASK_LIST:vid> | all ) state ( disable | enable )
 */
cparser_result_t
cparser_cmd_openflow_set_classifier_vlan_and_port_vid_all_state_disable_enable(
    cparser_context_t *context,
    char **vid_ptr)
{
    uint32 unit;
    int32 ret;
    diag_mask_t mask;
    rtk_of_classifierData_t data;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&mask, 0 , sizeof(mask));
    osal_memset(&data, 0 , sizeof(data));
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK(mask, 4, DIAG_MASKTYPE_VLAN), ret);
    DIAG_UTIL_PARSE_STATE(6, data.enable);
    DIAG_UTIL_MPRINTF("Classifier VLAN and Port: \n");
    DIAG_UTIL_MASK_SCAN(mask, data.vlan)
    {
        DIAG_UTIL_ERR_CHK(rtk_of_classifier_set(unit, OF_CLASSIFIER_TYPE_VLAN_AND_PORT, data), ret);
        DIAG_UTIL_MPRINTF("VLAN %4d: %s\n", data.vlan, text_state[data.enable]);
    }
    DIAG_UTIL_MPRINTF("\n");

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_classifier_vlan_and_port_vid_all_state_disable_enable */
#endif

#ifdef CMD_OPENFLOW_GET_FLOWTABLE_PHASE_MAPPING
/*
 * openflow get flowtable phase-mapping
 */
cparser_result_t
cparser_cmd_openflow_get_flowtable_phase_mapping(
    cparser_context_t *context)
{
    uint32  unit;
    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_MPRINTF("Phase 0 is Ingress Flow Table 0\n");
    DIAG_UTIL_MPRINTF("Phase 1 is Ingress Flow Table 1\n");
    DIAG_UTIL_MPRINTF("Phase 2 is Ingress Flow Table 2\n");
    DIAG_UTIL_MPRINTF("Phase 3 is Ingress Flow Table 3\n");
    DIAG_UTIL_MPRINTF("Phase 4 is Egress Flow Table 0\n");

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_get_flowtable_phase_mapping */
#endif

#ifdef CMD_OPENFLOW_GET_ENTRY_PHASE_ENTRY_SIZE
/*
 * openflow get entry <UINT:phase> entry-size
 */
cparser_result_t
cparser_cmd_openflow_get_entry_phase_entry_size(
    cparser_context_t *context,
    uint32_t *phase_ptr)
{
    uint32 unit;
    uint32 size = 0;
    int32 ret;
    rtk_of_flowtable_phase_t phase = FT_PHASE_END;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_OF_PHASE_MAPPING(*phase_ptr, phase);
    DIAG_UTIL_ERR_CHK(rtk_of_flowEntrySize_get(unit, phase, &size), ret);
    DIAG_UTIL_MPRINTF("phase %d entry size: %d\n", *phase_ptr, size);
    DIAG_UTIL_MPRINTF("\n");

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_get_entry_phase_entry_size */
#endif

#ifdef CMD_OPENFLOW_GET_ENTRY_PHASE_INDEX_STATE
/*
 * openflow get entry <UINT:phase> <UINT:index> state
 */
cparser_result_t
cparser_cmd_openflow_get_entry_phase_index_state(
    cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *index_ptr)
{
    uint32 unit;
    uint32 valid = 0;
    int32 ret;
    rtk_of_flowtable_phase_t phase = FT_PHASE_END;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_OF_PHASE_MAPPING(*phase_ptr, phase);
    DIAG_UTIL_ERR_CHK(rtk_of_flowEntryValidate_get(unit, phase, *index_ptr, &valid), ret);
    DIAG_UTIL_MPRINTF("phase %d entry %u valid: %s\n", *phase_ptr, *index_ptr, text_state[valid]);
    DIAG_UTIL_MPRINTF("\n");

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_get_entry_phase_index_state */
#endif

#ifdef CMD_OPENFLOW_SET_ENTRY_PHASE_INDEX_STATE_DISABLE_ENABLE
/*
 * openflow set entry <UINT:phase> <UINT:index> state ( disable | enable )
 */
cparser_result_t
cparser_cmd_openflow_set_entry_phase_index_state_disable_enable(
    cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *index_ptr)
{
    uint32 unit;
    uint32 valid = 0;
    int32 ret;
    rtk_of_flowtable_phase_t phase = FT_PHASE_END;

    DIAG_UTIL_FUNC_INIT(unit);

    DIAG_OF_PHASE_MAPPING(*phase_ptr, phase);
    DIAG_UTIL_PARSE_STATE(6, valid);
    DIAG_UTIL_ERR_CHK(rtk_of_flowEntryValidate_set(unit, phase, *index_ptr, valid), ret);
    DIAG_UTIL_MPRINTF("phase %d entry %u valid: %s\n", *phase_ptr, *index_ptr, text_state[valid]);
    DIAG_UTIL_MPRINTF("\n");

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_entry_phase_index_state_disable_enable */
#endif

#ifdef CMD_OPENFLOW_GET_ENTRY_PHASE_MATCH_FIELD_LIST
/*
 * openflow get entry <UINT:phase> match-field-list
 */
cparser_result_t
cparser_cmd_openflow_get_entry_phase_match_field_list(
    cparser_context_t *context,
    uint32_t *phase_ptr)
{
    uint32  i;
    uint32  unit = 0;
    int32   ret;
    rtk_of_flowtable_phase_t phase = FT_PHASE_END;
    rtk_of_matchfieldList_t  list;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_OF_PHASE_MAPPING(*phase_ptr, phase);

    DIAG_UTIL_ERR_CHK(rtk_of_flowEntryFieldList_get(unit, phase, &list), ret);

    DIAG_UTIL_MPRINTF("%-25s %s\n", "field keyword", "field desciption");
    DIAG_UTIL_MPRINTF("=====================================================\n");

    for (i = 0; i < (sizeof(diag_of_field_list)/sizeof(diag_of_field_t)); ++i)
    {

#if 0
        ret = rtk_of_flowEntryField_check(unit, phase, diag_of_field_list[i].type);

        if (RT_ERR_OK != ret)
            continue;
#endif

        if (BITMAP_IS_SET(list.field_bmp, diag_of_field_list[i].type))
            DIAG_UTIL_MPRINTF("%-25s %s\n", diag_of_field_list[i].user_info, diag_of_field_list[i].desc);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_get_entry_phase_match_field_list */
#endif

#ifdef CMD_OPENFLOW_GET_ENTRY_PHASE_INDEX_MATCH_FIELD_FIELD_NAME
/*
 * openflow get entry <UINT:phase> <UINT:index> match-field <STRING:field_name>
 */
cparser_result_t
cparser_cmd_openflow_get_entry_phase_index_match_field_field_name(
    cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *index_ptr,
    char **field_name_ptr)
{
    uint32  num_element;
    uint32  unit = 0;
    uint32  field_size = 0;
    int32   ret;
    uint32  i;
    uint8   field_data[RTK_OF_MATCH_FIELD_MAX];
    uint8   field_mask[RTK_OF_MATCH_FIELD_MAX];
    rtk_of_flowtable_phase_t phase = FT_PHASE_END;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_OF_PHASE_MAPPING(*phase_ptr, phase);
    num_element = (sizeof(diag_of_field_list)/sizeof(diag_of_field_t));
    osal_memset(field_data, 0, RTK_OF_MATCH_FIELD_MAX);
    osal_memset(field_mask, 0, RTK_OF_MATCH_FIELD_MAX);

    for (i = 0; i < num_element; ++i)
    {
        if (!strcasecmp(*field_name_ptr, diag_of_field_list[i].user_info))
            break;
    }

    if (num_element <= i)
    {
        DIAG_UTIL_MPRINTF("Invalid field %s!\n", *field_name_ptr);
        return CPARSER_NOT_OK;
    }

    ret = rtk_of_flowEntryField_check(unit, phase, diag_of_field_list[i].type);

    if (RT_ERR_OK != ret)
    {
        DIAG_UTIL_MPRINTF("Field %s isn't supported in the phase!\n", *field_name_ptr);
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_of_flowMatchFieldSize_get(unit, diag_of_field_list[i].type, &field_size), ret);

    if ((field_size % 8) == 0)
        field_size = (field_size / 8);
    else
        field_size = (field_size / 8) + 1;

    ret = rtk_of_flowEntryField_read(unit, phase, *index_ptr, diag_of_field_list[i].type, field_data, field_mask);

    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    if (0 == *phase_ptr)
        DIAG_UTIL_MPRINTF("Ingress Flow Table 0");
    else if (1 == *phase_ptr)
        DIAG_UTIL_MPRINTF("Ingress Flow Table 3");
    else
        DIAG_UTIL_MPRINTF("Egress Flow Table 0");

    DIAG_UTIL_MPRINTF(" index %d field %s\n", *index_ptr, *field_name_ptr);
    DIAG_UTIL_MPRINTF("\tdata=0x");

    for (i = 0; i < field_size ; ++i)
        DIAG_UTIL_MPRINTF("%02x", field_data[i]);

    DIAG_UTIL_MPRINTF("\n");
    DIAG_UTIL_MPRINTF("\tmask=0x");

    for (i = 0; i < field_size ; ++i)
        DIAG_UTIL_MPRINTF("%02x", field_mask[i]);

    DIAG_UTIL_MPRINTF("\n");

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_get_entry_phase_index_match_field_field_name */
#endif

#ifdef CMD_OPENFLOW_SET_ENTRY_PHASE_INDEX_MATCH_FIELD_FIELD_NAME_DATA_MASK
/*
 * openflow set entry <UINT:phase> <UINT:index> match-field <STRING:field_name> <STRING:data> <STRING:mask>
 */
cparser_result_t
cparser_cmd_openflow_set_entry_phase_index_match_field_field_name_data_mask(
    cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *index_ptr,
    char **field_name_ptr,
    char **data_ptr,
    char **mask_ptr)
{
    uint32  num_element, i;
    uint32  unit = 0;
    uint32  field_size = 0;
    int32   ret;
    uint8   field_data[RTK_OF_MATCH_FIELD_MAX];
    uint8   field_mask[RTK_OF_MATCH_FIELD_MAX];
    rtk_of_flowtable_phase_t phase = FT_PHASE_END;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_OF_PHASE_MAPPING(*phase_ptr, phase);
    num_element = (sizeof(diag_of_field_list)/sizeof(diag_of_field_t));

    for (i = 0; i < num_element; ++i)
    {
        if (!strcasecmp(*field_name_ptr, diag_of_field_list[i].user_info))
            break;
    }

    if (num_element <= i)
    {
        DIAG_UTIL_MPRINTF("Invalid field %s!\n", *field_name_ptr);
        return CPARSER_NOT_OK;
    }

    ret = rtk_of_flowEntryField_check(unit, phase, diag_of_field_list[i].type);

    if (RT_ERR_OK != ret)
    {
        DIAG_UTIL_MPRINTF("Field %s isn't supported in the phase!\n", *field_name_ptr);
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_of_flowMatchFieldSize_get(unit, diag_of_field_list[i].type, &field_size), ret);

    DIAG_UTIL_OF_PARAM_LEN_CHK(field_size, *data_ptr);
    DIAG_UTIL_OF_PARAM_LEN_CHK(field_size, *mask_ptr);
    osal_memset(field_data, 0x0, RTK_OF_MATCH_FIELD_MAX);
    osal_memset(field_mask, 0x0, RTK_OF_MATCH_FIELD_MAX);

    field_size = ((field_size + 7) / 8);
    if (diag_util_str2IntArray (field_data, *data_ptr, field_size) != RT_ERR_OK)
    {
        DIAG_UTIL_MPRINTF("field data error!\n");
        return CPARSER_NOT_OK;
    }

    if (diag_util_str2IntArray (field_mask, *mask_ptr, field_size) != RT_ERR_OK)
    {
        DIAG_UTIL_MPRINTF("field mask error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_of_flowEntryField_write(unit, phase, *index_ptr, diag_of_field_list[i].type, field_data, field_mask), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_entry_phase_index_match_field_field_name_data_mask */
#endif

#ifdef CMD_OPENFLOW_GET_ENTRY_PHASE_INDEX_OPERATION
/*
 * openflow get entry <UINT:phase> <UINT:index> operation
 */
cparser_result_t
cparser_cmd_openflow_get_entry_phase_index_operation(
    cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *index_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_flowtable_phase_t phase = FT_PHASE_END;
    rtk_of_flowOperation_t oper;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&oper, 0, sizeof(oper));
    DIAG_OF_PHASE_MAPPING(*phase_ptr, phase);
    DIAG_UTIL_ERR_CHK(rtk_of_flowEntryOperation_get(unit, phase, *index_ptr, &oper), ret);
    DIAG_UTIL_MPRINTF("phase %d entry %u operation:\n", *phase_ptr, *index_ptr);
    DIAG_UTIL_MPRINTF("Reverse: %s\n", text_state[oper.reverse]);
    DIAG_UTIL_MPRINTF("Two Entry Aggreation:    %s\n", text_state[oper.aggr_1]);
    DIAG_UTIL_MPRINTF("Four Entry Aggreation:   %s\n", text_state[oper.aggr_2]);
    DIAG_UTIL_MPRINTF("\n");

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_get_entry_phase_index_operation */
#endif

#ifdef CMD_OPENFLOW_SET_ENTRY_PHASE_INDEX_OPERATION_REVERSE_STATE_DISABLE_ENABLE
/*
 * openflow set entry <UINT:phase> <UINT:index> operation reverse state ( disable | enable )
 */
cparser_result_t
cparser_cmd_openflow_set_entry_phase_index_operation_reverse_state_disable_enable(
    cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *index_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_flowtable_phase_t phase = FT_PHASE_END;
    rtk_of_flowOperation_t oper;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&oper, 0, sizeof(oper));
    DIAG_OF_PHASE_MAPPING(*phase_ptr, phase);
    DIAG_UTIL_ERR_CHK(rtk_of_flowEntryOperation_get(unit, phase, *index_ptr, &oper), ret);
    DIAG_UTIL_PARSE_STATE(8, oper.reverse);
    DIAG_UTIL_ERR_CHK(rtk_of_flowEntryOperation_set(unit, phase, *index_ptr, &oper), ret);
    DIAG_UTIL_MPRINTF("phase %d entry %u operation:\n", *phase_ptr, *index_ptr);
    DIAG_UTIL_MPRINTF("reverse: %s\n", text_state[oper.reverse]);
    DIAG_UTIL_MPRINTF("agg 2:   %s\n", text_state[oper.aggr_1]);
    DIAG_UTIL_MPRINTF("agg 4:   %s\n", text_state[oper.aggr_2]);
    DIAG_UTIL_MPRINTF("\n");

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_entry_phase_index_operation_reverse_state_disable_enable */
#endif

#ifdef CMD_OPENFLOW_SET_ENTRY_PHASE_INDEX_OPERATION_AGGREGATE_2_ENTRY_STATE_DISABLE_ENABLE
/*
 * openflow set entry <UINT:phase> <UINT:index> operation aggregate-2-entry state ( disable | enable )
 */
cparser_result_t
cparser_cmd_openflow_set_entry_phase_index_operation_aggregate_2_entry_state_disable_enable(
    cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *index_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_flowtable_phase_t phase = FT_PHASE_END;
    rtk_of_flowOperation_t oper;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&oper, 0, sizeof(oper));
    DIAG_OF_PHASE_MAPPING(*phase_ptr, phase);
    DIAG_UTIL_ERR_CHK(rtk_of_flowEntryOperation_get(unit, phase, *index_ptr, &oper), ret);
    DIAG_UTIL_PARSE_STATE(8, oper.aggr_1);
    DIAG_UTIL_ERR_CHK(rtk_of_flowEntryOperation_set(unit, phase, *index_ptr, &oper), ret);
    DIAG_UTIL_MPRINTF("phase %d entry %u operation:\n", *phase_ptr, *index_ptr);
    DIAG_UTIL_MPRINTF("reverse: %s\n", text_state[oper.reverse]);
    DIAG_UTIL_MPRINTF("agg 2:   %s\n", text_state[oper.aggr_1]);
    DIAG_UTIL_MPRINTF("agg 4:   %s\n", text_state[oper.aggr_2]);
    DIAG_UTIL_MPRINTF("\n");

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_entry_phase_index_operation_aggregate_2_entry_state_disable_enable */
#endif

#ifdef CMD_OPENFLOW_SET_ENTRY_PHASE_INDEX_OPERATION_AGGREGATE_4_ENTRY_STATE_DISABLE_ENABLE
/*
 * openflow set entry <UINT:phase> <UINT:index> operation aggregate-4-entry state ( disable | enable )
 */
cparser_result_t
cparser_cmd_openflow_set_entry_phase_index_operation_aggregate_4_entry_state_disable_enable(
    cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *index_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_flowtable_phase_t phase = FT_PHASE_END;
    rtk_of_flowOperation_t oper;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&oper, 0, sizeof(oper));
    DIAG_OF_PHASE_MAPPING(*phase_ptr, phase);
    DIAG_UTIL_ERR_CHK(rtk_of_flowEntryOperation_get(unit, phase, *index_ptr, &oper), ret);
    DIAG_UTIL_PARSE_STATE(8, oper.aggr_2);
    DIAG_UTIL_ERR_CHK(rtk_of_flowEntryOperation_set(unit, phase, *index_ptr, &oper), ret);
    DIAG_UTIL_MPRINTF("phase %d entry %u operation:\n", *phase_ptr, *index_ptr);
    DIAG_UTIL_MPRINTF("reverse: %s\n", text_state[oper.reverse]);
    DIAG_UTIL_MPRINTF("Two Entry Aggreation:  %s\n", text_state[oper.aggr_1]);
    DIAG_UTIL_MPRINTF("Four Entry Aggreation: %s\n", text_state[oper.aggr_2]);
    DIAG_UTIL_MPRINTF("\n");

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_entry_phase_index_operation_aggregate_4_entry_state_disable_enable */
#endif

#ifdef CMD_OPENFLOW_GET_ENTRY_PHASE_INDEX_INSTRUCTION
/*
 * openflow get entry <UINT:phase> <UINT:index> instruction
 */
cparser_result_t
cparser_cmd_openflow_get_entry_phase_index_instruction(
    cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *index_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_flowtable_phase_t phase = FT_PHASE_END;
    rtk_of_flowIns_t ins;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&ins, 0, sizeof(ins));
    DIAG_OF_PHASE_MAPPING(*phase_ptr, phase);
    DIAG_UTIL_ERR_CHK(rtk_of_flowEntryInstruction_get(unit, phase, *index_ptr, &ins), ret);
    DUMP_INSTRUCTION(phase, ins, OF_INS_ACT_TYPE_END, *index_ptr);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_get_entry_phase_index_instruction */
#endif

#ifdef CMD_OPENFLOW_SET_ENTRY_PHASE_INDEX_INSTRUCTION_METER_CLEAR_ACTION_WRITE_ACTION_WRITE_METADATA_GOTO_TABLE_STATE_DISABLE_ENABLE
/*
 * openflow set entry <UINT:phase> <UINT:index> instruction ( meter | clear-action | write-action | write-metadata | goto-table ) state ( disable | enable )
 */
cparser_result_t
cparser_cmd_openflow_set_entry_phase_index_instruction_meter_clear_action_write_action_write_metadata_goto_table_state_disable_enable(
    cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *index_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_enable_t state = DISABLED;
    rtk_of_ins_action_type_t type = OF_INS_ACT_TYPE_END;
    rtk_of_flowtable_phase_t phase = FT_PHASE_END;
    rtk_of_flowIns_t ins;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&ins, 0, sizeof(ins));
    DIAG_UTIL_PARSE_STATE(8, state);
    DIAG_OF_PHASE_MAPPING(*phase_ptr, phase);
    DIAG_UTIL_ERR_CHK(rtk_of_flowEntryInstruction_get(unit, phase, *index_ptr, &ins), ret);

    DIAG_OF_PHASE_INSTRUCTION(6, type);

    if (type == OF_INS_ACT_TYPE_METER)
    {
        if (FT_PHASE_IGR_FT_0 == phase)
            ins.igrFt_0.meter_en = state;
        else if (FT_PHASE_IGR_FT_3 == phase)
            ins.igrFt_3.meter_en = state;
        else if (FT_PHASE_EGR_FT_0 == phase)
            ins.egrFt_0.meter_en = state;
    }
    else if (type == OF_INS_ACT_TYPE_CLEARACT)
    {
        EGR_PHASE_CHK_RET(phase);
        if (FT_PHASE_IGR_FT_0 == phase)
            ins.igrFt_0.clearAct_en= state;
        else if (FT_PHASE_IGR_FT_3 == phase)
            ins.igrFt_3.clearAct_en = state;
    }
    else if (type == OF_INS_ACT_TYPE_WRITEACT)
    {
        if (FT_PHASE_IGR_FT_0 == phase)
            ins.igrFt_0.writeAct_en= state;
        else if (FT_PHASE_IGR_FT_3 == phase)
            ins.igrFt_3.writeAct_en = state;
        else if (FT_PHASE_EGR_FT_0 == phase)
            ins.egrFt_0.writeAct_en = state;
    }
    else if (type == OF_INS_ACT_TYPE_WRITEMETA)
    {
        EGR_PHASE_CHK_RET(phase);
        if (FT_PHASE_IGR_FT_0 == phase)
            ins.igrFt_0.writeMetadata_en = state;
        else if (FT_PHASE_IGR_FT_3 == phase)
            ins.igrFt_3.writeMetadata_en = state;
    }
    else if (type == OF_INS_ACT_TYPE_GOTOTBL)
    {
        EGR_PHASE_CHK_RET(phase);
        if (FT_PHASE_IGR_FT_0 == phase)
            ins.igrFt_0.gotoTbl_en= state;
        else if (FT_PHASE_IGR_FT_3 == phase)
            ins.igrFt_3.gotoTbl_en = state;
    }

    DIAG_UTIL_ERR_CHK(rtk_of_flowEntryInstruction_set(unit, phase, *index_ptr, &ins), ret);
    DUMP_INSTRUCTION(phase, ins, OF_INS_ACT_TYPE_END, *index_ptr);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_entry_phase_index_instruction_meter_clear_action_write_action_write_metadata_goto_table_state_disable_enable */
#endif

#ifdef CMD_OPENFLOW_SET_ENTRY_PHASE_INDEX_INSTRUCTION_METER_METER_INDEX_YELLOW_DROP_DSCP_REMARK_RED_DROP_DSCP_REMARK
/*
 * openflow set entry <UINT:phase> <UINT:index> instruction meter <UINT:meter_index> yellow ( drop | dscp-remark ) red ( drop | dscp-remark )
 */
cparser_result_t
cparser_cmd_openflow_set_entry_phase_index_instruction_meter_meter_index_yellow_drop_dscp_remark_red_drop_dscp_remark(
    cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *index_ptr,
    uint32_t *meter_index_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_flowtable_phase_t phase = FT_PHASE_END;
    rtk_of_meterType_t yellow = OF_METER_TYPE_END;
    rtk_of_meterType_t red = OF_METER_TYPE_END;
    rtk_of_flowIns_t ins;
    rtk_of_flowInsMeter_t *meter = NULL;;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&ins, 0, sizeof(ins));
    DIAG_OF_PHASE_MAPPING(*phase_ptr, phase);
    DIAG_UTIL_ERR_CHK(rtk_of_flowEntryInstruction_get(unit, phase, *index_ptr, &ins), ret);

#define DIAG_OF_PARSE_METER_TYPE(__i, __t)  \
do{                                         \
    if ('r' == TOKEN_CHAR(__i, 1))          \
        __t = OF_METER_TYPE_DROP;           \
    else if ('s' == TOKEN_CHAR(__i, 1))     \
        __t = OF_METER_TYPE_DSCP_REMARK;    \
}while(0);

    DIAG_OF_PARSE_METER_TYPE(9, yellow);
    DIAG_OF_PARSE_METER_TYPE(11, red);

    if (FT_PHASE_IGR_FT_0 == phase)
        meter = &ins.igrFt_0.meter_data;
    else if (FT_PHASE_IGR_FT_3 == phase)
        meter = &ins.igrFt_3.meter_data;
    else if (FT_PHASE_EGR_FT_0 == phase)
        meter = &ins.egrFt_0.meter_data;

    meter->meter_id = *meter_index_ptr;
    meter->yellow = yellow;
    meter->red = red;
    DIAG_UTIL_ERR_CHK(rtk_of_flowEntryInstruction_set(unit, phase, *index_ptr, &ins), ret);
    DUMP_INSTRUCTION(phase, ins, OF_INS_ACT_TYPE_METER, *index_ptr);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_entry_phase_index_instruction_meter_meter_index_yellow_drop_dscp_remark_red_drop_dscp_remark */
#endif

#ifdef CMD_OPENFLOW_SET_ENTRY_PHASE_INDEX_INSTRUCTION_WRITE_ACTION_COPY_TTL_INWARD_COPY_TTL_OUTWARD_PUSH_VLAN_POP_VLAN_PUSH_MPLS_POP_MPLS_DEC_MPLS_TTL_DEC_IP_TTL_SET_QUEUE_GROUP_STATE_DISABLE_ENABLE
/*
 * openflow set entry <UINT:phase> <UINT:index> instruction write-action ( copy-ttl-inward | copy-ttl-outward | push-vlan | pop-vlan | push-mpls | pop-mpls | dec-mpls-ttl | dec-ip-ttl | set-queue | group ) state ( disable | enable )
 */
cparser_result_t
cparser_cmd_openflow_set_entry_phase_index_instruction_write_action_copy_ttl_inward_copy_ttl_outward_push_vlan_pop_vlan_push_mpls_pop_mpls_dec_mpls_ttl_dec_ip_ttl_set_queue_group_state_disable_enable(
    cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *index_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_enable_t state = DISABLED;
    rtk_of_flowtable_phase_t phase = FT_PHASE_END;
    rtk_of_flowIns_t ins;
    rtk_of_igrFT0InsWriteAct_t *igrwa = NULL;
    rtk_of_egrFTInsWriteAct_t *egrwa = NULL;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&ins, 0, sizeof(ins));
    DIAG_UTIL_PARSE_STATE(9, state);
    DIAG_OF_PHASE_MAPPING(*phase_ptr, phase);
    DIAG_UTIL_ERR_CHK(rtk_of_flowEntryInstruction_get(unit, phase, *index_ptr, &ins), ret);
    IGR_PTR_EXTRACT(phase, igrwa, ins, wa_data);
    egrwa = &ins.egrFt_0.wa_data;

    if ('c' == TOKEN_CHAR(7, 0))
    {
        if ('i' == TOKEN_CHAR(7, 9))
        {
            IGR_SET_EGR_RET(phase, igrwa, cp_ttl_inward, state);
        }
        else if ('o' == TOKEN_CHAR(7, 9))
        {
            IGR_SET_EGR_RET(phase, igrwa, cp_ttl_outward, state);
        }
    }
    else if ('p' == TOKEN_CHAR(7, 0))
    {
        if ('o' == TOKEN_CHAR(7, 1))
        {
            if ('v' == TOKEN_CHAR(7, 4))
            {
                IGR_EGR_SET(phase, igrwa, egrwa, pop_vlan, state);
            }
            else if ('m' == TOKEN_CHAR(7, 4))
            {
                IGR_SET_EGR_RET(phase, igrwa, pop_mpls, state);
            }
        }
        else if ('u' == TOKEN_CHAR(7, 1))
        {
            if ('v' == TOKEN_CHAR(7, 5))
            {
                IGR_EGR_SET(phase, igrwa, egrwa, push_vlan, state);
            }
            else if ('m' == TOKEN_CHAR(7, 5))
            {
                IGR_SET_EGR_RET(phase, igrwa, push_mpls, state);
            }
        }
    }
    else if ('d' == TOKEN_CHAR(7, 0))
    {
        if ('m' == TOKEN_CHAR(7, 4))
        {
            IGR_SET_EGR_RET(phase, igrwa, dec_mpls_ttl, state);
        }
        else if ('i' == TOKEN_CHAR(7, 4))
        {
            IGR_SET_EGR_RET(phase, igrwa, dec_ip_ttl, state);
        }
    }
    else if ('s' == TOKEN_CHAR(7, 0))
    {
        IGR_SET_EGR_RET(phase, igrwa, set_queue, state);
    }
    else if ('g' == TOKEN_CHAR(7, 0))
    {
        IGR_SET_EGR_RET(phase, igrwa, group, state);
    }

    DIAG_UTIL_ERR_CHK(rtk_of_flowEntryInstruction_set(unit, phase, *index_ptr, &ins), ret);
    DUMP_INSTRUCTION(phase, ins, OF_INS_ACT_TYPE_WRITEACT, *index_ptr);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_entry_phase_index_instruction_write_action_copy_ttl_inward_copy_ttl_outward_push_vlan_pop_vlan_push_mpls_pop_mpls_dec_mpls_ttl_dec_ip_ttl_set_queue_group_state_disable_enable */
#endif

#ifdef CMD_OPENFLOW_SET_ENTRY_PHASE_INDEX_INSTRUCTION_WRITE_ACTION_POP_MPLS_LABEL_NUM_OUTERMOST_DOUBLE
/*
 * openflow set entry <UINT:phase> <UINT:index> instruction write-action pop-mpls label-num ( outermost | double )
 */
cparser_result_t
cparser_cmd_openflow_set_entry_phase_index_instruction_write_action_pop_mpls_label_num_outermost_double(
    cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *index_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_flowtable_phase_t phase = FT_PHASE_END;
    rtk_of_flowIns_t ins;
    rtk_of_igrFT0InsWriteAct_t *igrwa = NULL;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&ins, 0, sizeof(ins));
    DIAG_OF_PHASE_MAPPING(*phase_ptr, phase);
    EGR_PHASE_CHK_RET(phase);
    DIAG_UTIL_ERR_CHK(rtk_of_flowEntryInstruction_get(unit, phase, *index_ptr, &ins), ret);
    IGR_PTR_EXTRACT(phase, igrwa, ins, wa_data);

    if ('o' == TOKEN_CHAR(9, 0))
        igrwa->pop_mpls_type = OF_POP_MPLS_TYPE_OUTERMOST_LABEL;
    else if ('d' == TOKEN_CHAR(9, 0))
        igrwa->pop_mpls_type = OF_POP_MPLS_TYPE_DOUBLE_LABEL;

    DIAG_UTIL_ERR_CHK(rtk_of_flowEntryInstruction_set(unit, phase, *index_ptr, &ins), ret);
    DUMP_INSTRUCTION(phase, ins, OF_INS_ACT_TYPE_WRITEACT, *index_ptr);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_entry_phase_index_instruction_write_action_pop_mpls_label_num_outermost_double */
#endif

#ifdef CMD_OPENFLOW_SET_ENTRY_PHASE_INDEX_INSTRUCTION_WRITE_ACTION_PUSH_MPLS_ETHERTYPE_0X8847_0X8848
/*
 * openflow set entry <UINT:phase> <UINT:index> instruction write-action push-mpls ethertype ( 0x8847 | 0x8848 )
 */
cparser_result_t
cparser_cmd_openflow_set_entry_phase_index_instruction_write_action_push_mpls_ethertype_0x8847_0x8848(
    cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *index_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_flowtable_phase_t phase = FT_PHASE_END;
    rtk_of_flowIns_t ins;
    rtk_of_igrFT0InsWriteAct_t *igrwa = NULL;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&ins, 0, sizeof(ins));
    DIAG_OF_PHASE_MAPPING(*phase_ptr, phase);
    EGR_PHASE_CHK_RET(phase);
    DIAG_UTIL_ERR_CHK(rtk_of_flowEntryInstruction_get(unit, phase, *index_ptr, &ins), ret);
    IGR_PTR_EXTRACT(phase, igrwa, ins, wa_data);

    if ('7' == TOKEN_CHAR(9, 5))
        igrwa->push_mpls_data.etherType = 0;
    else if ('8' == TOKEN_CHAR(9, 5))
        igrwa->push_mpls_data.etherType = 1;

    DIAG_UTIL_ERR_CHK(rtk_of_flowEntryInstruction_set(unit, phase, *index_ptr, &ins), ret);
    DUMP_INSTRUCTION(phase, ins, OF_INS_ACT_TYPE_WRITEACT, *index_ptr);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_entry_phase_index_instruction_write_action_push_mpls_ethertype_0x8847_0x8848 */
#endif

#ifdef CMD_OPENFLOW_SET_ENTRY_PHASE_INDEX_INSTRUCTION_WRITE_ACTION_PUSH_MPLS_MODE_NORMAL_ENCAP_TABLE_ENCAP_IDX
/*
 * openflow set entry <UINT:phase> <UINT:index> instruction write-action push-mpls mode ( normal | encap-table ) { <UINT:encap_idx> }
 */
cparser_result_t
cparser_cmd_openflow_set_entry_phase_index_instruction_write_action_push_mpls_mode_normal_encap_table_encap_idx(
    cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *index_ptr,
    uint32_t *encap_idx_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_flowtable_phase_t phase = FT_PHASE_END;
    rtk_of_flowIns_t ins;
    rtk_of_igrFT0InsWriteAct_t *igrwa = NULL;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&ins, 0, sizeof(ins));
    DIAG_OF_PHASE_MAPPING(*phase_ptr, phase);
    EGR_PHASE_CHK_RET(phase);
    DIAG_UTIL_ERR_CHK(rtk_of_flowEntryInstruction_get(unit, phase, *index_ptr, &ins), ret);
    IGR_PTR_EXTRACT(phase, igrwa, ins, wa_data);

    if ('n' == TOKEN_CHAR(9, 0))
        igrwa->push_mpls_data.push_mode = OF_PUSH_MPLS_MODE_NORMAL;
    else if ('e' == TOKEN_CHAR(9, 0))
        igrwa->push_mpls_data.push_mode = OF_PUSH_MPLS_MODE_ENCAPTBL;

    if (NULL != encap_idx_ptr)
        igrwa->push_mpls_data.mpls_encap_idx = *encap_idx_ptr;

    DIAG_UTIL_ERR_CHK(rtk_of_flowEntryInstruction_set(unit, phase, *index_ptr, &ins), ret);
    DUMP_INSTRUCTION(phase, ins, OF_INS_ACT_TYPE_WRITEACT, *index_ptr);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_entry_phase_index_instruction_write_action_push_mpls_mode_normal_encap_table_encap_idx */
#endif

#ifdef CMD_OPENFLOW_SET_ENTRY_PHASE_INDEX_INSTRUCTION_WRITE_ACTION_PUSH_MPLS_VPN_L2_L3
/*
 * openflow set entry <UINT:phase> <UINT:index> instruction write-action push-mpls vpn ( l2 | l3 )
 */
cparser_result_t
cparser_cmd_openflow_set_entry_phase_index_instruction_write_action_push_mpls_vpn_l2_l3(
    cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *index_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_flowtable_phase_t phase = FT_PHASE_END;
    rtk_of_flowIns_t ins;
    rtk_of_igrFT0InsWriteAct_t *igrwa = NULL;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&ins, 0, sizeof(ins));
    DIAG_OF_PHASE_MAPPING(*phase_ptr, phase);
    EGR_PHASE_CHK_RET(phase);
    DIAG_UTIL_ERR_CHK(rtk_of_flowEntryInstruction_get(unit, phase, *index_ptr, &ins), ret);
    IGR_PTR_EXTRACT(phase, igrwa, ins, wa_data);

    if ('2' == TOKEN_CHAR(9, 1))
        igrwa->push_mpls_data.vpn_type = OF_PUSH_MPLS_VPN_TYPE_L2;
    else if ('3' == TOKEN_CHAR(9, 1))
        igrwa->push_mpls_data.vpn_type = OF_PUSH_MPLS_VPN_TYPE_L3;

    DIAG_UTIL_ERR_CHK(rtk_of_flowEntryInstruction_set(unit, phase, *index_ptr, &ins), ret);
    DUMP_INSTRUCTION(phase, ins, OF_INS_ACT_TYPE_WRITEACT, *index_ptr);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_entry_phase_index_instruction_write_action_push_mpls_vpn_l2_l3 */
#endif

#ifdef CMD_OPENFLOW_SET_ENTRY_PHASE_INDEX_INSTRUCTION_WRITE_ACTION_PUSH_VLAN_ETHERTYPE_TPID_IDX
/*
 * openflow set entry <UINT:phase> <UINT:index> instruction write-action push-vlan ethertype <UINT:tpid_idx>
 */
cparser_result_t
cparser_cmd_openflow_set_entry_phase_index_instruction_write_action_push_vlan_ethertype_tpid_idx(
    cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *index_ptr,
    uint32_t *tpid_idx_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_flowtable_phase_t phase = FT_PHASE_END;
    rtk_of_flowIns_t ins;
    rtk_of_igrFT0InsWriteAct_t *igrwa = NULL;
    rtk_of_egrFTInsWriteAct_t *egrwa = NULL;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&ins, 0, sizeof(ins));
    DIAG_OF_PHASE_MAPPING(*phase_ptr, phase);
    DIAG_UTIL_ERR_CHK(rtk_of_flowEntryInstruction_get(unit, phase, *index_ptr, &ins), ret);
    IGR_PTR_EXTRACT(phase, igrwa, ins, wa_data);
    egrwa = &ins.egrFt_0.wa_data;

    IGR_EGR_SET(phase, igrwa, egrwa, push_vlan_data.etherType_idx, *tpid_idx_ptr);

    DIAG_UTIL_ERR_CHK(rtk_of_flowEntryInstruction_set(unit, phase, *index_ptr, &ins), ret);
    DUMP_INSTRUCTION(phase, ins, OF_INS_ACT_TYPE_WRITEACT, *index_ptr);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_entry_phase_index_instruction_write_action_push_vlan_ethertype_tpid_idx */
#endif

#ifdef CMD_OPENFLOW_SET_ENTRY_PHASE_INDEX_INSTRUCTION_WRITE_ACTION_SET_FIELD_FIELD_ID_FIELD_TYPE_NONE
/*
 * openflow set entry <UINT:phase> <UINT:index> instruction write-action set-field <UINT:field_id> field-type none
 */
cparser_result_t
cparser_cmd_openflow_set_entry_phase_index_instruction_write_action_set_field_field_id_field_type_none(
    cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *index_ptr,
    uint32_t *field_id_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_flowtable_phase_t phase = FT_PHASE_END;
    rtk_of_flowIns_t ins;
    rtk_of_igrFT0InsWriteAct_t *igrwa = NULL;
    rtk_of_egrFTInsWriteAct_t *egrwa = NULL;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&ins, 0, sizeof(ins));
    DIAG_OF_PHASE_MAPPING(*phase_ptr, phase);
    DIAG_UTIL_ERR_CHK(rtk_of_flowEntryInstruction_get(unit, phase, *index_ptr, &ins), ret);
    DIAG_UTIL_ERR_CHK(rtk_of_flowEntrySetField_check(unit, phase, *field_id_ptr, OF_SET_FIELD_TYPE_NONE), ret);
    IGR_PTR_EXTRACT(phase, igrwa, ins, wa_data);
    egrwa = &ins.egrFt_0.wa_data;

    WRITE_SETFIELD(OF_SET_FIELD_TYPE_NONE, 0);
    DIAG_UTIL_ERR_CHK(rtk_of_flowEntryInstruction_set(unit, phase, *index_ptr, &ins), ret);
    DUMP_INSTRUCTION(phase, ins, OF_INS_ACT_TYPE_WRITEACT, *index_ptr);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_entry_phase_index_instruction_write_action_set_field_field_id_field_type_none */
#endif

#ifdef CMD_OPENFLOW_SET_ENTRY_PHASE_INDEX_INSTRUCTION_WRITE_ACTION_SET_FIELD_FIELD_ID_FIELD_TYPE_SMAC_DMAC_MAC
/*
 * openflow set entry <UINT:phase> <UINT:index> instruction write-action set-field <UINT:field_id> field-type ( smac | dmac ) <MACADDR:mac>
 */
cparser_result_t
cparser_cmd_openflow_set_entry_phase_index_instruction_write_action_set_field_field_id_field_type_smac_dmac_mac(
    cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *index_ptr,
    uint32_t *field_id_ptr,
    cparser_macaddr_t *mac_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_flowtable_phase_t phase = FT_PHASE_END;
    rtk_of_setFieldType_t type = OF_SET_FIELD_TYPE_END;
    rtk_of_flowIns_t ins;
    rtk_of_igrFT0InsWriteAct_t *igrwa = NULL;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&ins, 0, sizeof(ins));
    DIAG_OF_PHASE_MAPPING(*phase_ptr, phase);

    if ('s' == TOKEN_CHAR(10, 0))
        type = OF_SET_FIELD_TYPE_SA;
    else if ('d' == TOKEN_CHAR(10, 0))
        type = OF_SET_FIELD_TYPE_DA;

    DIAG_UTIL_ERR_CHK(rtk_of_flowEntryInstruction_get(unit, phase, *index_ptr, &ins), ret);
    DIAG_UTIL_ERR_CHK(rtk_of_flowEntrySetField_check(unit, phase, *field_id_ptr, type), ret);
    IGR_PTR_EXTRACT(phase, igrwa, ins, wa_data);

    if (0 == *field_id_ptr)
    {
        IGR_SET_EGR_RET(phase, igrwa, set_field_0_data.field_type, type);
        osal_memcpy(igrwa->set_field_0_data.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    }
    else if (1 == *field_id_ptr)
    {
        IGR_SET_EGR_RET(phase, igrwa, set_field_1_data.field_type, type);
        osal_memcpy(igrwa->set_field_1_data.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    }

    DIAG_UTIL_ERR_CHK(rtk_of_flowEntryInstruction_set(unit, phase, *index_ptr, &ins), ret);
    DUMP_INSTRUCTION(phase, ins, OF_INS_ACT_TYPE_WRITEACT, *index_ptr);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_entry_phase_index_instruction_write_action_set_field_field_id_field_type_smac_dmac_mac */
#endif

#ifdef CMD_OPENFLOW_SET_ENTRY_PHASE_INDEX_INSTRUCTION_WRITE_ACTION_SET_FIELD_FIELD_ID_FIELD_TYPE_VLAN_PRIORITY_VID_DSCP_IP_TTL_L4_SPORT_L4_DPORT_VALUE
/*
 * openflow set entry <UINT:phase> <UINT:index> instruction write-action set-field <UINT:field_id> field-type ( vlan-priority | vid | dscp | ip-ttl | l4-sport | l4-dport ) <UINT:value>
 */
cparser_result_t
cparser_cmd_openflow_set_entry_phase_index_instruction_write_action_set_field_field_id_field_type_vlan_priority_vid_dscp_ip_ttl_l4_sport_l4_dport_value(
    cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *index_ptr,
    uint32_t *field_id_ptr,
    uint32_t *value_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_flowtable_phase_t phase = FT_PHASE_END;
    rtk_of_setFieldType_t type = OF_SET_FIELD_TYPE_END;
    rtk_of_flowIns_t ins;
    rtk_of_igrFT0InsWriteAct_t *igrwa = NULL;
    rtk_of_egrFTInsWriteAct_t *egrwa = NULL;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&ins, 0, sizeof(ins));
    DIAG_OF_PHASE_MAPPING(*phase_ptr, phase);

    if ('l' == TOKEN_CHAR(10, 1))
        type = OF_SET_FIELD_TYPE_VLAN_PRI;
    else if ('i' == TOKEN_CHAR(10, 1))
        type = OF_SET_FIELD_TYPE_VLAN_ID;
    else if ('s' == TOKEN_CHAR(10, 1))
        type = OF_SET_FIELD_TYPE_IP_DSCP;
    else if ('p' == TOKEN_CHAR(10, 1))
        type = OF_SET_FIELD_TYPE_IP_TTL;
    else if ('s' == TOKEN_CHAR(10, 3))
        type = OF_SET_FIELD_TYPE_L4_SPORT;
    else if ('d' == TOKEN_CHAR(10, 3))
        type = OF_SET_FIELD_TYPE_L4_DPORT;

    DIAG_UTIL_ERR_CHK(rtk_of_flowEntryInstruction_get(unit, phase, *index_ptr, &ins), ret);
    DIAG_UTIL_ERR_CHK(rtk_of_flowEntrySetField_check(unit, phase, *field_id_ptr, type), ret);
    IGR_PTR_EXTRACT(phase, igrwa, ins, wa_data);
    egrwa = &ins.egrFt_0.wa_data;

    WRITE_SETFIELD(type, *value_ptr);
    DIAG_UTIL_ERR_CHK(rtk_of_flowEntryInstruction_set(unit, phase, *index_ptr, &ins), ret);
    DUMP_INSTRUCTION(phase, ins, OF_INS_ACT_TYPE_WRITEACT, *index_ptr);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_entry_phase_index_instruction_write_action_set_field_field_id_field_type_vlan_priority_vid_dscp_ip_ttl_l4_sport_l4_dport_value */
#endif

#ifdef CMD_OPENFLOW_SET_ENTRY_PHASE_INDEX_INSTRUCTION_WRITE_ACTION_SET_FIELD_FIELD_ID_FIELD_TYPE_MPLS_LABEL_MPLS_TC_MPLS_TTL_VALUE
/*
 * openflow set entry <UINT:phase> <UINT:index> instruction write-action set-field <UINT:field_id> field-type ( mpls-label | mpls-tc | mpls-ttl ) <UINT:value>
 */
cparser_result_t
cparser_cmd_openflow_set_entry_phase_index_instruction_write_action_set_field_field_id_field_type_mpls_label_mpls_tc_mpls_ttl_value(
    cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *index_ptr,
    uint32_t *field_id_ptr,
    uint32_t *value_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_flowtable_phase_t phase = FT_PHASE_END;
    rtk_of_setFieldType_t type = OF_SET_FIELD_TYPE_END;
    rtk_of_flowIns_t ins;
    rtk_of_igrFT0InsWriteAct_t *igrwa = NULL;
    rtk_of_egrFTInsWriteAct_t *egrwa = NULL;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&ins, 0, sizeof(ins));
    DIAG_OF_PHASE_MAPPING(*phase_ptr, phase);

    if ('a' == TOKEN_CHAR(10, 6))
        type = OF_SET_FIELD_TYPE_MPLS_LABEL;
    else if ('c' == TOKEN_CHAR(10, 6))
        type = OF_SET_FIELD_TYPE_MPLS_TC;
    else if ('t' == TOKEN_CHAR(10, 6))
        type = OF_SET_FIELD_TYPE_MPLS_TTL;

    DIAG_UTIL_ERR_CHK(rtk_of_flowEntryInstruction_get(unit, phase, *index_ptr, &ins), ret);
    DIAG_UTIL_ERR_CHK(rtk_of_flowEntrySetField_check(unit, phase, *field_id_ptr, type), ret);
    IGR_PTR_EXTRACT(phase, igrwa, ins, wa_data);
    egrwa = &ins.egrFt_0.wa_data;

    WRITE_SETFIELD(type, *value_ptr);
    DIAG_UTIL_ERR_CHK(rtk_of_flowEntryInstruction_set(unit, phase, *index_ptr, &ins), ret);
    DUMP_INSTRUCTION(phase, ins, OF_INS_ACT_TYPE_WRITEACT, *index_ptr);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_entry_phase_index_instruction_write_action_set_field_field_id_field_type_mpls_label_mpls_tc_mpls_ttl_value */
#endif

#ifdef CMD_OPENFLOW_SET_ENTRY_PHASE_INDEX_INSTRUCTION_WRITE_ACTION_SET_FIELD_FIELD_ID_FIELD_TYPE_SIP_DIP_IP
/*
 * openflow set entry <UINT:phase> <UINT:index> instruction write-action set-field <UINT:field_id> field-type ( sip | dip ) <IPV4ADDR:ip>
 */
cparser_result_t
cparser_cmd_openflow_set_entry_phase_index_instruction_write_action_set_field_field_id_field_type_sip_dip_ip(
    cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *index_ptr,
    uint32_t *field_id_ptr,
    uint32_t *ip_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_flowtable_phase_t phase = FT_PHASE_END;
    rtk_of_setFieldType_t type = OF_SET_FIELD_TYPE_END;
    rtk_of_flowIns_t ins;
    rtk_of_igrFT0InsWriteAct_t *igrwa = NULL;
    rtk_of_egrFTInsWriteAct_t *egrwa = NULL;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&ins, 0, sizeof(ins));
    DIAG_OF_PHASE_MAPPING(*phase_ptr, phase);

    if ('s' == TOKEN_CHAR(10, 0))
        type = OF_SET_FIELD_TYPE_IP4_SIP;
    else if ('d' == TOKEN_CHAR(10, 0))
        type = OF_SET_FIELD_TYPE_IP4_DIP;

    DIAG_UTIL_ERR_CHK(rtk_of_flowEntryInstruction_get(unit, phase, *index_ptr, &ins), ret);
    DIAG_UTIL_ERR_CHK(rtk_of_flowEntrySetField_check(unit, phase, *field_id_ptr, type), ret);
    IGR_PTR_EXTRACT(phase, igrwa, ins, wa_data);
    egrwa = &ins.egrFt_0.wa_data;

    WRITE_SETFIELD(type, *ip_ptr);
    DIAG_UTIL_ERR_CHK(rtk_of_flowEntryInstruction_set(unit, phase, *index_ptr, &ins), ret);
    DUMP_INSTRUCTION(phase, ins, OF_INS_ACT_TYPE_WRITEACT, *index_ptr);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_entry_phase_index_instruction_write_action_set_field_field_id_field_type_sip_dip_ip */
#endif

#ifdef CMD_OPENFLOW_SET_ENTRY_PHASE_INDEX_INSTRUCTION_WRITE_ACTION_SET_FIELD_FIELD_ID_FIELD_TYPE_IP_FLAG_RSVD
/*
 * openflow set entry <UINT:phase> <UINT:index> instruction write-action set-field <UINT:field_id> field-type ip-flag-rsvd
 */
cparser_result_t
cparser_cmd_openflow_set_entry_phase_index_instruction_write_action_set_field_field_id_field_type_ip_flag_rsvd(
    cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *index_ptr,
    uint32_t *field_id_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_flowtable_phase_t phase = FT_PHASE_END;
    rtk_of_setFieldType_t type = OF_SET_FIELD_TYPE_END;
    rtk_of_flowIns_t ins;
    rtk_of_igrFT0InsWriteAct_t *igrwa = NULL;
    rtk_of_egrFTInsWriteAct_t *egrwa = NULL;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&ins, 0, sizeof(ins));
    DIAG_OF_PHASE_MAPPING(*phase_ptr, phase);

    type = OF_SET_FIELD_TYPE_IP_FLAG_RSVD;

    DIAG_UTIL_ERR_CHK(rtk_of_flowEntryInstruction_get(unit, phase, *index_ptr, &ins), ret);
    DIAG_UTIL_ERR_CHK(rtk_of_flowEntrySetField_check(unit, phase, *field_id_ptr, type), ret);
    IGR_PTR_EXTRACT(phase, igrwa, ins, wa_data);
    egrwa = &ins.egrFt_0.wa_data;

    WRITE_SETFIELD(type, 0);
    DIAG_UTIL_ERR_CHK(rtk_of_flowEntryInstruction_set(unit, phase, *index_ptr, &ins), ret);
    DUMP_INSTRUCTION(phase, ins, OF_INS_ACT_TYPE_WRITEACT, *index_ptr);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_entry_phase_index_instruction_write_action_set_field_field_id_field_type_ip_flag_rsvd */
#endif

#ifdef CMD_OPENFLOW_SET_ENTRY_PHASE_INDEX_INSTRUCTION_WRITE_ACTION_SET_QUEUE_QID
/*
 * openflow set entry <UINT:phase> <UINT:index> instruction write-action set-queue <UINT:qid>
 */
cparser_result_t
cparser_cmd_openflow_set_entry_phase_index_instruction_write_action_set_queue_qid(
    cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *index_ptr,
    uint32_t *qid_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_flowtable_phase_t phase = FT_PHASE_END;
    rtk_of_flowIns_t ins;
    rtk_of_igrFT0InsWriteAct_t *igrwa = NULL;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&ins, 0, sizeof(ins));
    DIAG_OF_PHASE_MAPPING(*phase_ptr, phase);
    DIAG_UTIL_ERR_CHK(rtk_of_flowEntryInstruction_get(unit, phase, *index_ptr, &ins), ret);
    IGR_PTR_EXTRACT(phase, igrwa, ins, wa_data);

    IGR_SET_EGR_RET(phase, igrwa, qid, *qid_ptr);
    DIAG_UTIL_ERR_CHK(rtk_of_flowEntryInstruction_set(unit, phase, *index_ptr, &ins), ret);
    DUMP_INSTRUCTION(phase, ins, OF_INS_ACT_TYPE_WRITEACT, *index_ptr);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_entry_phase_index_instruction_write_action_set_queue_qid */
#endif

#ifdef CMD_OPENFLOW_SET_ENTRY_PHASE_INDEX_INSTRUCTION_WRITE_ACTION_GROUP_GID
/*
 * openflow set entry <UINT:phase> <UINT:index> instruction write-action group <UINT:gid>
 */
cparser_result_t
cparser_cmd_openflow_set_entry_phase_index_instruction_write_action_group_gid(
    cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *index_ptr,
    uint32_t *gid_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_flowtable_phase_t phase = FT_PHASE_END;
    rtk_of_flowIns_t ins;
    rtk_of_igrFT0InsWriteAct_t *igrwa = NULL;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&ins, 0, sizeof(ins));
    DIAG_OF_PHASE_MAPPING(*phase_ptr, phase);
    DIAG_UTIL_ERR_CHK(rtk_of_flowEntryInstruction_get(unit, phase, *index_ptr, &ins), ret);
    IGR_PTR_EXTRACT(phase, igrwa, ins, wa_data);

    IGR_SET_EGR_RET(phase, igrwa, gid, *gid_ptr);
    DIAG_UTIL_ERR_CHK(rtk_of_flowEntryInstruction_set(unit, phase, *index_ptr, &ins), ret);
    DUMP_INSTRUCTION(phase, ins, OF_INS_ACT_TYPE_WRITEACT, *index_ptr);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_entry_phase_index_instruction_write_action_group_gid */
#endif

#ifdef CMD_OPENFLOW_SET_ENTRY_PHASE_INDEX_INSTRUCTION_WRITE_ACTION_OUTPUT_NONE
/*
 * openflow set entry <UINT:phase> <UINT:index> instruction write-action output none
 */
cparser_result_t
cparser_cmd_openflow_set_entry_phase_index_instruction_write_action_output_none(
    cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *index_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_flowtable_phase_t phase = FT_PHASE_END;
    rtk_of_outputType_t type = OF_OUTPUT_TYPE_END;
    rtk_of_flowIns_t ins;
    rtk_of_igrFT0InsWriteAct_t *igrwa = NULL;
    rtk_of_egrFTInsWriteAct_t *egrwa = NULL;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&ins, 0, sizeof(ins));
    DIAG_OF_PHASE_MAPPING(*phase_ptr, phase);

    DIAG_UTIL_ERR_CHK(rtk_of_flowEntryInstruction_get(unit, phase, *index_ptr, &ins), ret);
    IGR_PTR_EXTRACT(phase, igrwa, ins, wa_data);
    egrwa = &ins.egrFt_0.wa_data;

    if (phase == FT_PHASE_EGR_FT_0)
    {
        egrwa->drop = ENABLED;
    }
    else
    {
        type = OF_OUTPUT_TYPE_NONE;
        IGR_SET_EGR_RET(phase, igrwa, output_data.type, type);
    }
    DIAG_UTIL_ERR_CHK(rtk_of_flowEntryInstruction_set(unit, phase, *index_ptr, &ins), ret);
    DUMP_INSTRUCTION(phase, ins, OF_INS_ACT_TYPE_WRITEACT, *index_ptr);
    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_entry_phase_index_instruction_write_action_output_none */
#endif

#ifdef CMD_OPENFLOW_SET_ENTRY_PHASE_INDEX_INSTRUCTION_WRITE_ACTION_OUTPUT_FORWARD
/*
 * openflow set entry <UINT:phase> <UINT:index> instruction write-action output forward
 */
cparser_result_t
cparser_cmd_openflow_set_entry_phase_index_instruction_write_action_output_forward(
    cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *index_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_flowtable_phase_t phase = FT_PHASE_END;
    rtk_of_flowIns_t ins;
    rtk_of_egrFTInsWriteAct_t *egrwa = NULL;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&ins, 0, sizeof(ins));
    DIAG_OF_PHASE_MAPPING(*phase_ptr, phase);

    DIAG_UTIL_ERR_CHK(rtk_of_flowEntryInstruction_get(unit, phase, *index_ptr, &ins), ret);
    egrwa = &ins.egrFt_0.wa_data;

    if (phase == FT_PHASE_EGR_FT_0)
    {
        egrwa->drop = DISABLED;
    }
    else
    {
        DIAG_UTIL_MPRINTF("Attribute is not support in specified phase\n");
        return CPARSER_NOT_OK;
    }
    DIAG_UTIL_ERR_CHK(rtk_of_flowEntryInstruction_set(unit, phase, *index_ptr, &ins), ret);
    DUMP_INSTRUCTION(phase, ins, OF_INS_ACT_TYPE_WRITEACT, *index_ptr);
    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_entry_phase_index_instruction_write_action_output_forward */
#endif

#ifdef CMD_OPENFLOW_SET_ENTRY_PHASE_INDEX_INSTRUCTION_WRITE_ACTION_OUTPUT_PHY_PORT_PHY_PORT_SRC_PORT_FILTER_PORT_UID
/*
 * openflow set entry <UINT:phase> <UINT:index> instruction write-action output ( phy-port | phy-port-src-port-filter ) <UINT:port> { <UINT:devid> }
 */
cparser_result_t
cparser_cmd_openflow_set_entry_phase_index_instruction_write_action_output_phy_port_phy_port_src_port_filter_port_devid(
    cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *index_ptr,
    uint32_t *port_ptr,
    uint32_t *devid_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_flowtable_phase_t phase = FT_PHASE_END;
    rtk_of_outputType_t type = OF_OUTPUT_TYPE_END;
    rtk_of_flowIns_t ins;
    rtk_of_igrFT0InsWriteAct_t *igrwa = NULL;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&ins, 0, sizeof(ins));
    DIAG_OF_PHASE_MAPPING(*phase_ptr, phase);

    if ( osal_strlen(TOKEN_STR(8)) > 8)
        type = OF_OUTPUT_TYPE_PHY_PORT_EX_SRC_PORT;
    else
        type = OF_OUTPUT_TYPE_PHY_PORT;

    DIAG_UTIL_ERR_CHK(rtk_of_flowEntryInstruction_get(unit, phase, *index_ptr, &ins), ret);
    IGR_PTR_EXTRACT(phase, igrwa, ins, wa_data);

    IGR_SET_EGR_RET(phase, igrwa, output_data.type, type);
    IGR_SET_EGR_RET(phase, igrwa, output_data.port, *port_ptr);
    if ( devid_ptr == NULL )
    {
        IGR_SET_EGR_RET(phase, igrwa, output_data.devID, 0);
    }
    else
    {
        IGR_SET_EGR_RET(phase, igrwa, output_data.devID, *devid_ptr);
    }
    DIAG_UTIL_ERR_CHK(rtk_of_flowEntryInstruction_set(unit, phase, *index_ptr, &ins), ret);
    DUMP_INSTRUCTION(phase, ins, OF_INS_ACT_TYPE_WRITEACT, *index_ptr);
    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_entry_phase_index_instruction_write_action_output_phy_port_phy_port_src_port_filter_port_devid */
#endif

#ifdef CMD_OPENFLOW_SET_ENTRY_PHASE_INDEX_INSTRUCTION_WRITE_ACTION_OUTPUT_TRUNK_TRUNK_SRC_PORT_FILTER_TID
/*
 * openflow set entry <UINT:phase> <UINT:index> instruction write-action output ( trunk | trunk-src-port-filter ) <UINT:tid>
 */
cparser_result_t
cparser_cmd_openflow_set_entry_phase_index_instruction_write_action_output_trunk_trunk_src_port_filter_tid(
    cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *index_ptr,
    uint32_t *tid_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_flowtable_phase_t phase = FT_PHASE_END;
    rtk_of_outputType_t type = OF_OUTPUT_TYPE_END;
    rtk_of_flowIns_t ins;
    rtk_of_igrFT0InsWriteAct_t *igrwa = NULL;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&ins, 0, sizeof(ins));
    DIAG_OF_PHASE_MAPPING(*phase_ptr, phase);

    if ( osal_strlen(TOKEN_STR(8)) > 5)
        type = OF_OUTPUT_TYPE_TRK_PORT_EX_SRC_PORT;
    else
        type = OF_OUTPUT_TYPE_TRK_PORT;

    DIAG_UTIL_ERR_CHK(rtk_of_flowEntryInstruction_get(unit, phase, *index_ptr, &ins), ret);
    IGR_PTR_EXTRACT(phase, igrwa, ins, wa_data);

    IGR_SET_EGR_RET(phase, igrwa, output_data.type, type);
    IGR_SET_EGR_RET(phase, igrwa, output_data.trunk, *tid_ptr);
    DIAG_UTIL_ERR_CHK(rtk_of_flowEntryInstruction_set(unit, phase, *index_ptr, &ins), ret);
    DUMP_INSTRUCTION(phase, ins, OF_INS_ACT_TYPE_WRITEACT, *index_ptr);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_entry_phase_index_instruction_write_action_output_trunk_trunk_src_port_filter_tid */
#endif

#ifdef CMD_OPENFLOW_SET_ENTRY_PHASE_INDEX_INSTRUCTION_WRITE_ACTION_OUTPUT_MULTI_EGRESS_PORT_PORT_ALL
/*
 * openflow set entry <UINT:phase> <UINT:index> instruction write-action output multi-egress-port ( <PORT_LIST:port> | all )
 */
cparser_result_t
cparser_cmd_openflow_set_entry_phase_index_instruction_write_action_output_multi_egress_port_port_all(
    cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *index_ptr,
    char **port_ptr)
{
    uint32 unit;
    int32 ret;
    diag_portlist_t portlist;
    rtk_of_flowtable_phase_t phase = FT_PHASE_END;
    rtk_of_outputType_t type = OF_OUTPUT_TYPE_MULTI_EGR_PORT;
    rtk_of_flowIns_t ins;
    rtk_of_igrFT0InsWriteAct_t *igrwa = NULL;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&ins, 0, sizeof(ins));
    osal_memset(&portlist, 0, sizeof(portlist));
    DIAG_UTIL_EXTRACT_PORTLIST(portlist, 9);
    DIAG_OF_PHASE_MAPPING(*phase_ptr, phase);
    DIAG_UTIL_ERR_CHK(rtk_of_flowEntryInstruction_get(unit, phase, *index_ptr, &ins), ret);
    IGR_PTR_EXTRACT(phase, igrwa, ins, wa_data);

    IGR_SET_EGR_RET(phase, igrwa, output_data.type, type);
    IGR_SET_EGR_RET(phase, igrwa, output_data.portmask, portlist.portmask);
    DIAG_UTIL_ERR_CHK(rtk_of_flowEntryInstruction_set(unit, phase, *index_ptr, &ins), ret);
    DUMP_INSTRUCTION(phase, ins, OF_INS_ACT_TYPE_WRITEACT, *index_ptr);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_entry_phase_index_instruction_write_action_output_multi_egress_port_port_all */
#endif

#ifdef CMD_OPENFLOW_SET_ENTRY_PHASE_INDEX_INSTRUCTION_WRITE_ACTION_OUTPUT_INGRESS_PORT
/*
 * openflow set entry <UINT:phase> <UINT:index> instruction write-action output ingress-port
 */
cparser_result_t
cparser_cmd_openflow_set_entry_phase_index_instruction_write_action_output_ingress_port(
    cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *index_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_flowtable_phase_t phase = FT_PHASE_END;
    rtk_of_outputType_t type = OF_OUTPUT_TYPE_END;
    rtk_of_flowIns_t ins;
    rtk_of_igrFT0InsWriteAct_t *igrwa = NULL;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&ins, 0, sizeof(ins));
    DIAG_OF_PHASE_MAPPING(*phase_ptr, phase);

    type = OF_OUTPUT_TYPE_IN_PORT;

    DIAG_UTIL_ERR_CHK(rtk_of_flowEntryInstruction_get(unit, phase, *index_ptr, &ins), ret);
    IGR_PTR_EXTRACT(phase, igrwa, ins, wa_data);

    IGR_SET_EGR_RET(phase, igrwa, output_data.type, type);
    DIAG_UTIL_ERR_CHK(rtk_of_flowEntryInstruction_set(unit, phase, *index_ptr, &ins), ret);
    DUMP_INSTRUCTION(phase, ins, OF_INS_ACT_TYPE_WRITEACT, *index_ptr);
    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_entry_phase_index_instruction_write_action_output_ingress_port */
#endif

#ifdef CMD_OPENFLOW_SET_ENTRY_PHASE_INDEX_INSTRUCTION_WRITE_ACTION_OUTPUT_FLOOD
/*
 * openflow set entry <UINT:phase> <UINT:index> instruction write-action output flood
 */
cparser_result_t
cparser_cmd_openflow_set_entry_phase_index_instruction_write_action_output_flood(
    cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *index_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_flowtable_phase_t phase = FT_PHASE_END;
    rtk_of_outputType_t type = OF_OUTPUT_TYPE_END;
    rtk_of_flowIns_t ins;
    rtk_of_igrFT0InsWriteAct_t *igrwa = NULL;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&ins, 0, sizeof(ins));
    DIAG_OF_PHASE_MAPPING(*phase_ptr, phase);

    type = OF_OUTPUT_TYPE_FLOOD;

    DIAG_UTIL_ERR_CHK(rtk_of_flowEntryInstruction_get(unit, phase, *index_ptr, &ins), ret);
    IGR_PTR_EXTRACT(phase, igrwa, ins, wa_data);

    IGR_SET_EGR_RET(phase, igrwa, output_data.type, type);
    DIAG_UTIL_ERR_CHK(rtk_of_flowEntryInstruction_set(unit, phase, *index_ptr, &ins), ret);
    DUMP_INSTRUCTION(phase, ins, OF_INS_ACT_TYPE_WRITEACT, *index_ptr);
    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_entry_phase_index_instruction_write_action_output_flood */
#endif

#ifdef CMD_OPENFLOW_SET_ENTRY_PHASE_INDEX_INSTRUCTION_WRITE_ACTION_OUTPUT_NORMAL
/*
 * openflow set entry <UINT:phase> <UINT:index> instruction write-action output normal
 */
cparser_result_t
cparser_cmd_openflow_set_entry_phase_index_instruction_write_action_output_normal(
    cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *index_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_flowtable_phase_t phase = FT_PHASE_END;
    rtk_of_outputType_t type = OF_OUTPUT_TYPE_END;
    rtk_of_flowIns_t ins;
    rtk_of_igrFT0InsWriteAct_t *igrwa = NULL;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&ins, 0, sizeof(ins));
    DIAG_OF_PHASE_MAPPING(*phase_ptr, phase);

    type = OF_OUTPUT_TYPE_LB;

    DIAG_UTIL_ERR_CHK(rtk_of_flowEntryInstruction_get(unit, phase, *index_ptr, &ins), ret);
    IGR_PTR_EXTRACT(phase, igrwa, ins, wa_data);

    IGR_SET_EGR_RET(phase, igrwa, output_data.type, type);
    DIAG_UTIL_ERR_CHK(rtk_of_flowEntryInstruction_set(unit, phase, *index_ptr, &ins), ret);
    DUMP_INSTRUCTION(phase, ins, OF_INS_ACT_TYPE_WRITEACT, *index_ptr);
    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_entry_phase_index_instruction_write_action_output_normal */
#endif


#ifdef CMD_OPENFLOW_SET_ENTRY_PHASE_INDEX_INSTRUCTION_WRITE_ACTION_OUTPUT_FAILOVER_PORT_ALL
/*
 * openflow set entry <UINT:phase> <UINT:index> instruction write-action output failover ( <PORT_LIST:port> | all )
 */
cparser_result_t
cparser_cmd_openflow_set_entry_phase_index_instruction_write_action_output_failover_port_all(
    cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *index_ptr,
    char **port_ptr)
{
    uint32 unit;
    int32 ret;
    diag_portlist_t portlist;
    rtk_of_flowtable_phase_t phase = FT_PHASE_END;
    rtk_of_outputType_t type = OF_OUTPUT_TYPE_FAILOVER;
    rtk_of_flowIns_t ins;
    rtk_of_igrFT0InsWriteAct_t *igrwa = NULL;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&ins, 0, sizeof(ins));
    osal_memset(&portlist, 0, sizeof(portlist));
    DIAG_UTIL_EXTRACT_PORTLIST(portlist, 9);
    DIAG_OF_PHASE_MAPPING(*phase_ptr, phase);
    DIAG_UTIL_ERR_CHK(rtk_of_flowEntryInstruction_get(unit, phase, *index_ptr, &ins), ret);
    IGR_PTR_EXTRACT(phase, igrwa, ins, wa_data);

    IGR_SET_EGR_RET(phase, igrwa, output_data.type, type);
    IGR_SET_EGR_RET(phase, igrwa, output_data.portmask, portlist.portmask);
    DIAG_UTIL_ERR_CHK(rtk_of_flowEntryInstruction_set(unit, phase, *index_ptr, &ins), ret);
    DUMP_INSTRUCTION(phase, ins, OF_INS_ACT_TYPE_WRITEACT, *index_ptr);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_entry_phase_index_instruction_write_action_output_failover_port_all */
#endif

#ifdef CMD_OPENFLOW_SET_ENTRY_PHASE_INDEX_INSTRUCTION_WRITE_ACTION_OUTPUT_TUNNEL_IFID
/*
 * openflow set entry <UINT:phase> <UINT:index> instruction write-action output tunnel <UINT:ifid>
 */
cparser_result_t
cparser_cmd_openflow_set_entry_phase_index_instruction_write_action_output_tunnel_ifid(
    cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *index_ptr,
    uint32_t *ifid_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_flowtable_phase_t phase = FT_PHASE_END;
    rtk_of_outputType_t type = OF_OUTPUT_TYPE_TUNNEL;
    rtk_of_flowIns_t ins;
    rtk_of_igrFT0InsWriteAct_t *igrwa = NULL;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&ins, 0, sizeof(ins));
    DIAG_OF_PHASE_MAPPING(*phase_ptr, phase);
    DIAG_UTIL_ERR_CHK(rtk_of_flowEntryInstruction_get(unit, phase, *index_ptr, &ins), ret);
    IGR_PTR_EXTRACT(phase, igrwa, ins, wa_data);

    IGR_SET_EGR_RET(phase, igrwa, output_data.type, type);
    IGR_SET_EGR_RET(phase, igrwa, output_data.intf, *ifid_ptr);
    DIAG_UTIL_ERR_CHK(rtk_of_flowEntryInstruction_set(unit, phase, *index_ptr, &ins), ret);
    DUMP_INSTRUCTION(phase, ins, OF_INS_ACT_TYPE_WRITEACT, *index_ptr);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_entry_phase_index_instruction_write_action_output_tunnel_ifid */
#endif

#ifdef CMD_OPENFLOW_SET_ENTRY_PHASE_INDEX_INSTRUCTION_WRITE_METADATA_DATA_MASK
/*
 * openflow set entry <UINT:phase> <UINT:index> instruction write-metadata <UINT:data> <UINT:mask>
 */
cparser_result_t
cparser_cmd_openflow_set_entry_phase_index_instruction_write_metadata_data_mask(
    cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *index_ptr,
    uint32_t *data_ptr,
    uint32_t *mask_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_flowtable_phase_t phase = FT_PHASE_END;
    rtk_of_flowIns_t ins;
    rtk_of_flowInsWriteMd_t *igrwm = NULL;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&ins, 0, sizeof(ins));
    DIAG_OF_PHASE_MAPPING(*phase_ptr, phase);
    DIAG_UTIL_ERR_CHK(rtk_of_flowEntryInstruction_get(unit, phase, *index_ptr, &ins), ret);
    IGR_PTR_EXTRACT(phase, igrwm, ins, wm_data);

    IGR_SET_EGR_RET(phase, igrwm, data, *data_ptr);
    IGR_SET_EGR_RET(phase, igrwm, mask, *mask_ptr);
    DIAG_UTIL_ERR_CHK(rtk_of_flowEntryInstruction_set(unit, phase, *index_ptr, &ins), ret);
    DUMP_INSTRUCTION(phase, ins, OF_INS_ACT_TYPE_WRITEMETA, *index_ptr);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_entry_phase_index_instruction_write_metadata_data_mask */
#endif

#ifdef CMD_OPENFLOW_SET_ENTRY_PHASE_INDEX_INSTRUCTION_GOTO_TABLE_NORMAL_TID
/*
 * openflow set entry <UINT:phase> <UINT:index> instruction goto-table normal <UINT:tid>
 */
cparser_result_t
cparser_cmd_openflow_set_entry_phase_index_instruction_goto_table_normal_tid(
    cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *index_ptr,
    uint32_t *tid_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_flowtable_phase_t phase = FT_PHASE_END;
    rtk_of_flowIns_t ins;
    rtk_of_flowInsGotoTbl_t *igrgt = NULL;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&ins, 0, sizeof(ins));
    DIAG_OF_PHASE_MAPPING(*phase_ptr, phase);
    DIAG_UTIL_ERR_CHK(rtk_of_flowEntryInstruction_get(unit, phase, *index_ptr, &ins), ret);
    IGR_PTR_EXTRACT(phase, igrgt, ins, gt_data);

    IGR_SET_EGR_RET(phase, igrgt, type, OF_GOTOTBL_TYPE_NORMAL);
    IGR_SET_EGR_RET(phase, igrgt, tbl_id, *tid_ptr);
    DIAG_UTIL_ERR_CHK(rtk_of_flowEntryInstruction_set(unit, phase, *index_ptr, &ins), ret);
    DUMP_INSTRUCTION(phase, ins, OF_INS_ACT_TYPE_GOTOTBL, *index_ptr);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_entry_phase_index_instruction_goto_table_normal_tid */
#endif

#ifdef CMD_OPENFLOW_SET_ENTRY_PHASE_INDEX_INSTRUCTION_GOTO_TABLE_LOOPBACK_APPLY_THEN_LOOPBACK_TID
/*
 * openflow set entry <UINT:phase> <UINT:index> instruction goto-table ( loopback | apply-then-loopback ) <UINT:tid>
 */
cparser_result_t
cparser_cmd_openflow_set_entry_phase_index_instruction_goto_table_loopback_apply_then_loopback_tid(
    cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *index_ptr,
    uint32_t *tid_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_gotoTblType_t type = OF_GOTOTBL_TYPE_END;
    rtk_of_flowtable_phase_t phase = FT_PHASE_END;
    rtk_of_flowIns_t ins;
    rtk_of_flowInsGotoTbl_t *igrgt = NULL;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&ins, 0, sizeof(ins));

    if ('l' == TOKEN_CHAR(7, 0))
        type = OF_GOTOTBL_TYPE_LB;
    else if ('a' == TOKEN_CHAR(7, 0))
        type = OF_GOTOTBL_TYPE_APPLY_AND_LB;

    DIAG_OF_PHASE_MAPPING(*phase_ptr, phase);
    DIAG_UTIL_ERR_CHK(rtk_of_flowEntryInstruction_get(unit, phase, *index_ptr, &ins), ret);
    IGR_PTR_EXTRACT(phase, igrgt, ins, gt_data);

    IGR_SET_EGR_RET(phase, igrgt, type, type);
    IGR_SET_EGR_RET(phase, igrgt, tbl_id, *tid_ptr);
    DIAG_UTIL_ERR_CHK(rtk_of_flowEntryInstruction_set(unit, phase, *index_ptr, &ins), ret);
    DUMP_INSTRUCTION(phase, ins, OF_INS_ACT_TYPE_GOTOTBL, *index_ptr);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_entry_phase_index_instruction_goto_table_loopback_apply_then_loopback_tid */
#endif

#ifdef CMD_OPENFLOW_GET_ENTRY_PHASE_INDEX_HIT_INDICATION
/*
 * openflow get entry <UINT:phase> <UINT:index> hit-indication
 */
cparser_result_t
cparser_cmd_openflow_get_entry_phase_index_hit_indication(
    cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *index_ptr)
{
    uint32 unit;
    uint32 isHit = FALSE;
    int32 ret;
    rtk_of_flowtable_phase_t phase = FT_PHASE_END;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_OF_PHASE_MAPPING(*phase_ptr, phase);
    DIAG_UTIL_ERR_CHK(rtk_of_flowEntryHitSts_get(unit, phase, *index_ptr, FALSE, &isHit), ret);
    DIAG_UTIL_MPRINTF("Phase %u entey %u hit status: %s\n", *phase_ptr, *index_ptr, isHit ?"True":"False");

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_get_entry_phase_index_hit_indication */
#endif

#ifdef CMD_OPENFLOW_RESET_ENTRY_PHASE_INDEX_HIT_INDICATION
/*
 * openflow reset entry <UINT:phase> <UINT:index> hit-indication
 */
cparser_result_t
cparser_cmd_openflow_reset_entry_phase_index_hit_indication(
    cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *index_ptr)
{
    uint32 unit;
    uint32 isHit = FALSE;
    int32 ret;
    rtk_of_flowtable_phase_t phase = FT_PHASE_END;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_OF_PHASE_MAPPING(*phase_ptr, phase);
    DIAG_UTIL_ERR_CHK(rtk_of_flowEntryHitSts_get(unit, phase, *index_ptr, TRUE, &isHit), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_reset_entry_phase_index_hit_indication */
#endif

#ifdef CMD_OPENFLOW_DEL_ENTRY_PHASE_START_END
/*
 * openflow del entry <UINT:phase> <UINT:start> <UINT:end>
 */
cparser_result_t
cparser_cmd_openflow_del_entry_phase_start_end(
    cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *start_ptr,
    uint32_t *end_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_flowClear_t idxs;
    rtk_of_flowtable_phase_t phase = FT_PHASE_END;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_OF_PHASE_MAPPING(*phase_ptr, phase);
    idxs.start_idx = *start_ptr;
    idxs.end_idx = *end_ptr;

    DIAG_UTIL_ERR_CHK(rtk_of_flowEntry_del(unit, phase, &idxs), ret);
    DIAG_UTIL_MPRINTF("Delete entry %u to %u\n", *start_ptr, *end_ptr);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_del_entry_phase_start_end */
#endif

#ifdef CMD_OPENFLOW_MOVE_ENTRY_PHASE_START_END_ENTRY_NUM
/*
 * openflow move entry <UINT:phase> <UINT:start> <UINT:end> <UINT:entry_num>
 */
cparser_result_t
cparser_cmd_openflow_move_entry_phase_start_end_entry_num(
    cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *start_ptr,
    uint32_t *end_ptr,
    uint32_t *entry_num_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_flowMove_t moves;
    rtk_of_flowtable_phase_t phase = FT_PHASE_END;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_OF_PHASE_MAPPING(*phase_ptr, phase);
    moves.move_from = *start_ptr;
    moves.move_to = *end_ptr;
    moves.length = *entry_num_ptr;

    DIAG_UTIL_ERR_CHK(rtk_of_flowEntry_move(unit, phase, &moves), ret);
    DIAG_UTIL_MPRINTF("Move entry from %u to %u\n", *start_ptr, *end_ptr);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_move_entry_phase_start_end_entry_num */
#endif

#ifdef CMD_OPENFLOW_GET_FLOWTABLE_SELECTOR_PHASE_BLOCK_ID
/*
 * openflow get flowtable selector <UINT:phase> <UINT:block_id>
 */
cparser_result_t
cparser_cmd_openflow_get_flowtable_selector_phase_block_id(
    cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *block_id_ptr)
{
    uint32 unit;
    uint32 max;
    uint32 i = 0;
    int32 ret;
    rtk_of_ftTemplateIdx_t idx;
    rtk_of_flowtable_phase_t phase = FT_PHASE_END;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_OF_PHASE_MAPPING(*phase_ptr, phase);
    osal_memset(&idx, 0 ,sizeof(idx));

    DIAG_UTIL_ERR_CHK(rtk_of_ftTemplateSelector_get(unit, phase, *block_id_ptr, &idx), ret);

    DIAG_OM_GET_CHIP_CAPACITY(unit, max, max_num_of_pie_block_templateSelector);
    DIAG_UTIL_MPRINTF("Block %d:\n", *block_id_ptr);

    for(i = 0; i < max; i ++)
        DIAG_UTIL_MPRINTF("\tTemplate idx %d: %d\n", i, idx.template_id[i]);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_get_flowtable_selector_block_id */
#endif

#ifdef CMD_OPENFLOW_SET_FLOWTABLE_SELECTOR_PHASE_BLOCK_ID_TEMPLATE_INDEX0_TEMPLATE_INDEX
/*
 * openflow set flowtable selector <UINT:phase> <UINT:block_id> template-index0 <UINT:template_index>
 */
cparser_result_t
cparser_cmd_openflow_set_flowtable_selector_phase_block_id_template_index0_template_index(
    cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *block_id_ptr,
    uint32_t *template_index_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_ftTemplateIdx_t idx;
    rtk_of_flowtable_phase_t phase = FT_PHASE_END;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_OF_PHASE_MAPPING(*phase_ptr, phase);
    osal_memset(&idx, 0 ,sizeof(idx));

    DIAG_UTIL_ERR_CHK(rtk_of_ftTemplateSelector_get(unit, phase, *block_id_ptr, &idx), ret);
    idx.template_id[0] = *template_index_ptr;
    DIAG_UTIL_ERR_CHK(rtk_of_ftTemplateSelector_set(unit, phase, *block_id_ptr, idx), ret);
    DIAG_UTIL_MPRINTF("Phase %u Block id %u template-index0 set to template %u\n", phase, *block_id_ptr, *template_index_ptr);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_flowtable_selector_block_id_template_index0_template_index */
#endif

#ifdef CMD_OPENFLOW_SET_FLOWTABLE_SELECTOR_PHASE_BLOCK_ID_TEMPLATE_INDEX1_TEMPLATE_INDEX
/*
 * openflow set flowtable selector <UINT:phase> <UINT:block_id> template-index1 <UINT:template_index>
 */
cparser_result_t
cparser_cmd_openflow_set_flowtable_selector_phase_block_id_template_index1_template_index(
    cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *block_id_ptr,
    uint32_t *template_index_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_ftTemplateIdx_t idx;
    rtk_of_flowtable_phase_t phase = FT_PHASE_END;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_OF_PHASE_MAPPING(*phase_ptr, phase);
    osal_memset(&idx, 0 ,sizeof(idx));

    DIAG_UTIL_ERR_CHK(rtk_of_ftTemplateSelector_get(unit, phase, *block_id_ptr, &idx), ret);
    idx.template_id[1] = *template_index_ptr;
    DIAG_UTIL_ERR_CHK(rtk_of_ftTemplateSelector_set(unit, phase, *block_id_ptr, idx), ret);
    DIAG_UTIL_MPRINTF("Phase %u Block id %u template-index1 set to template %u\n", phase, *block_id_ptr, *template_index_ptr);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_flowtable_selector_block_id_template_index1_template_index */
#endif

#ifdef CMD_OPENFLOW_GET_ENTRY_PHASE_INDEX_COUNTER_MODE
/*
 * openflow get entry <UINT:phase> <UINT:index> counter mode
 */
cparser_result_t
cparser_cmd_openflow_get_entry_phase_index_counter_mode(
    cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *index_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_flowCntMode_t mode = OF_FLOW_CNTMODE_END;
    rtk_of_flowtable_phase_t phase = FT_PHASE_END;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_OF_PHASE_MAPPING(*phase_ptr, phase);
    DIAG_UTIL_ERR_CHK(rtk_of_flowCntMode_get(unit, phase, *index_ptr, &mode), ret);
    DIAG_UTIL_MPRINTF("Phase %u index %u mode is %s\n", *phase_ptr, *index_ptr, text_counter_mode[mode]);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_get_entry_phase_index_counter_mode */
#endif

#ifdef CMD_OPENFLOW_SET_ENTRY_PHASE_INDEX_COUNTER_MODE_PACKET_AND_BYTE_COUNTER_PACKET_COUNTER_AND_THRESHOLD_BYTE_COUNTER_AND_THRESHOLD_DISABLE
/*
 * openflow set entry <UINT:phase> <UINT:index> counter mode ( packet-and-byte-counter |  packet-counter-and-threshold | byte-counter-and-threshold | disable )
 */
cparser_result_t
cparser_cmd_openflow_set_entry_phase_index_counter_mode_packet_and_byte_counter_packet_counter_and_threshold_byte_counter_and_threshold_disable(
    cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *index_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_flowCntMode_t mode = OF_FLOW_CNTMODE_END;
    rtk_of_flowtable_phase_t phase = FT_PHASE_END;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_OF_PHASE_MAPPING(*phase_ptr, phase);

    if ('a' == TOKEN_CHAR(7, 7))
        mode = OF_FLOW_CNTMODE_PACKET_CNT_AND_BYTE_CNT;
    else if ('c' == TOKEN_CHAR(7, 7))
        mode = OF_FLOW_CNTMODE_PACKET_CNT_AND_PACKET_CNT_TH;
    else if ('u' == TOKEN_CHAR(7, 7))
        mode = OF_FLOW_CNTMODE_BYTE_CNT_AND_BYTE_CNT_TH;
    else if ('d' == TOKEN_CHAR(7, 0))
        mode = OF_FLOW_CNTMODE_DISABLE;

    DIAG_UTIL_ERR_CHK(rtk_of_flowCntMode_set(unit, phase, *index_ptr, mode), ret);
    DIAG_UTIL_MPRINTF("Phase %u index %u counter mode set to %s\n", *phase_ptr, *index_ptr, text_counter_mode[mode]);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_entry_phase_index_counter_mode_packet_and_byte_counter_packet_counter_and_threshold_byte_counter_and_threshold */
#endif

#ifdef CMD_OPENFLOW_GET_ENTRY_PHASE_INDEX_COUNTER_TYPE_BYTE_PACKET
/*
 * openflow get entry <UINT:phase> <UINT:index> counter type ( byte | packet )
 */
cparser_result_t
cparser_cmd_openflow_get_entry_phase_index_counter_type_byte_packet(
    cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *index_ptr)
{
    uint32 unit;
    uint64 cnt = 0;
    int32 ret;
    rtk_of_flowCntType_t type = OF_FLOW_CNT_TYPE_END;
    rtk_of_flowtable_phase_t phase = FT_PHASE_END;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_OF_PHASE_MAPPING(*phase_ptr, phase);

    if ('b' == TOKEN_CHAR(7, 0))
        type = OF_FLOW_CNT_TYPE_BYTE;
    else if ('p' == TOKEN_CHAR(7, 0))
        type = OF_FLOW_CNT_TYPE_PACKET;

    DIAG_UTIL_ERR_CHK(rtk_of_flowCnt_get(unit, phase, *index_ptr, type, &cnt), ret);
    DIAG_UTIL_MPRINTF("Phase %u index %u counter: %llu\n", *phase_ptr, *index_ptr, cnt);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_get_entry_phase_index_counter_type_byte_packet */
#endif

#ifdef CMD_OPENFLOW_CLEAR_ENTRY_PHASE_INDEX_COUNTER_TYPE_BYTE_PACKET
/*
 * openflow clear entry <UINT:phase> <UINT:index> counter type ( byte | packet )
 */
cparser_result_t
cparser_cmd_openflow_clear_entry_phase_index_counter_type_byte_packet(
    cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *index_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_flowCntType_t type = OF_FLOW_CNT_TYPE_END;
    rtk_of_flowtable_phase_t phase = FT_PHASE_END;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_OF_PHASE_MAPPING(*phase_ptr, phase);

    if ('b' == TOKEN_CHAR(7, 0))
        type = OF_FLOW_CNT_TYPE_BYTE;
    else if ('p' == TOKEN_CHAR(7, 0))
        type = OF_FLOW_CNT_TYPE_PACKET;

    DIAG_UTIL_ERR_CHK(rtk_of_flowCnt_clear(unit, phase, *index_ptr, type), ret);
    DIAG_UTIL_MPRINTF("Clear phase %u index %u counter\n", *phase_ptr, *index_ptr);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_clear_entry_phase_index_counter_type_byte_packet */
#endif

#ifdef CMD_OPENFLOW_GET_ENTRY_PHASE_INDEX_COUNTER_THRESHOLD
/*
 * openflow get entry <UINT:phase> <UINT:index> counter threshold
 */
cparser_result_t
cparser_cmd_openflow_get_entry_phase_index_counter_threshold(
    cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *index_ptr)
{
    uint32 unit;
    uint64 threshold = 0;
    int32 ret;
    rtk_of_flowtable_phase_t phase = FT_PHASE_END;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_OF_PHASE_MAPPING(*phase_ptr, phase);
    DIAG_UTIL_ERR_CHK(rtk_of_flowCntThresh_get(unit, phase, *index_ptr, &threshold), ret);
    DIAG_UTIL_MPRINTF("Phase %u index %u counter threshold: %llu\n", *phase_ptr, *index_ptr, threshold);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_get_entry_phase_index_counter_threshold */
#endif

#ifdef CMD_OPENFLOW_SET_ENTRY_PHASE_INDEX_COUNTER_THRESHOLD_THRESHOLD
/*
 * openflow set entry <UINT:phase> <UINT:index> counter threshold <UINT64:threshold>
 */
cparser_result_t
cparser_cmd_openflow_set_entry_phase_index_counter_threshold_threshold(
    cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *index_ptr,
    uint64_t *threshold_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_flowtable_phase_t phase = FT_PHASE_END;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_OF_PHASE_MAPPING(*phase_ptr, phase);
    DIAG_UTIL_ERR_CHK(rtk_of_flowCntThresh_set(unit, phase, *index_ptr, *threshold_ptr), ret);
    DIAG_UTIL_MPRINTF("Phase %u index %u counter threshold set to %llu\n", *phase_ptr, *index_ptr, *threshold_ptr);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_entry_phase_index_counter_threshold_threshold */
#endif

#ifdef CMD_OPENFLOW_GET_EXCEPTION_TTL
/*
 * openflow get exception ttl
 */
cparser_result_t
cparser_cmd_openflow_get_exception_ttl(
    cparser_context_t *context)
{
    uint32 unit;
    int32 ret;
    rtk_action_t action = ACTION_END;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(rtk_of_ttlExcpt_get(unit, &action), ret);
    DIAG_UTIL_MPRINTF("TTL exception action is %s\n", text_action[action]);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_get_exception_ttl */
#endif

#ifdef CMD_OPENFLOW_SET_EXCEPTION_TTL_DROP_FORWARD
/*
 * openflow set exception ttl ( drop | forward )
 */
cparser_result_t
cparser_cmd_openflow_set_exception_ttl_drop_forward(
    cparser_context_t *context)
{
    uint32 unit;
    int32 ret;
    rtk_action_t action = ACTION_END;

    if ('d' == TOKEN_CHAR(4, 0))
        action = ACTION_DROP;
    else if ('f' == TOKEN_CHAR(4, 0))
        action = ACTION_FORWARD;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(rtk_of_ttlExcpt_set(unit, action), ret);
    DIAG_UTIL_MPRINTF("TTL exception action set to %s\n", text_action[action]);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_exception_ttl_drop_forward */
#endif

#ifdef CMD_OPENFLOW_GET_FLOWTABLE_MAX_LOOPBACK
/*
 * openflow get flowtable max-loopback
 */
cparser_result_t
cparser_cmd_openflow_get_flowtable_max_loopback(
    cparser_context_t *context)
{
    uint32 unit;
    uint32 time = 0;
    int32 ret;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(rtk_of_maxLoopback_get(unit, &time), ret);
    if (time == 0)
        DIAG_UTIL_MPRINTF("Flow table max-loopback time is no limitation\n");
    else
        DIAG_UTIL_MPRINTF("Flow table max-loopback time is %u\n", time);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_get_flowtable_max_loopback */
#endif

#ifdef CMD_OPENFLOW_SET_FLOWTABLE_MAX_LOOPBACK_TIME
/*
 * openflow set flowtable max-loopback <UINT:time>
 */
cparser_result_t
cparser_cmd_openflow_set_flowtable_max_loopback_time(
    cparser_context_t *context,
    uint32_t *time_ptr)
{
    uint32 unit;
    int32 ret;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(rtk_of_maxLoopback_set(unit, *time_ptr), ret);
        if (*time_ptr == 0)
        DIAG_UTIL_MPRINTF("Flow table max-loopback time set to no limitation\n");
    else
        DIAG_UTIL_MPRINTF("Flow table max-loopback time set to %u\n", *time_ptr);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_flowtable_max_loopback_time */
#endif

#ifdef CMD_OPENFLOW_GET_FLOWTABLE_L2_MATCH_FIELD
/*
 * openflow get flowtable l2 match-field
 */
cparser_result_t
cparser_cmd_openflow_get_flowtable_l2_match_field(
    cparser_context_t *context)
{
    uint32 unit;
    rtk_of_l2FlowTblMatchField_t field = OF_L2_FT_MATCH_FIELD_END;
    int32 ret;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowTblMatchField_get(unit, &field), ret);
    DIAG_UTIL_MPRINTF("Flow table l2 match field is %s\n", text_l2_match_field[field]);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_get_flowtable_l2_match_field */
#endif

#ifdef CMD_OPENFLOW_SET_FLOWTABLE_L2_MATCH_FIELD_VID_SA_SA_VID_SA_VID_DA_VID_SA_DA_SA_VID_DA_SA_DA_VID_DA_DA_DA_SA_VID_SA_DA_SA_SA_DA_SA_VID_DA_DA_SA_DA_5_TUPLE
/*
 * openflow set flowtable l2 match-field ( vid-sa-sa | vid-sa-vid-da | vid-sa-da | sa-vid-da | sa-da | vid-da-da | da-sa-vid-sa | da-sa-sa | da-sa-vid-da | da-sa-da | 5-tuple )
 */
cparser_result_t
cparser_cmd_openflow_set_flowtable_l2_match_field_vid_sa_sa_vid_sa_vid_da_vid_sa_da_sa_vid_da_sa_da_vid_da_da_da_sa_vid_sa_da_sa_sa_da_sa_vid_da_da_sa_da_5_tuple(
    cparser_context_t *context)
{
    uint32 unit;
    int32 ret;
    rtk_of_l2FlowTblMatchField_t field = OF_L2_FT_MATCH_FIELD_END;

    DIAG_UTIL_FUNC_INIT(unit);

    if ('v' == TOKEN_CHAR(5, 0))
    {
        if ('s' == TOKEN_CHAR(5, 7))
            field = OF_L2_FT_MATCH_FIELD_VID_SA_SA;
        else if ('v' == TOKEN_CHAR(5, 7))
            field = OF_L2_FT_MATCH_FIELD_VID_SA_VID_DA;
        else if ('d' == TOKEN_CHAR(5, 7))
        {
            if ('s' == TOKEN_CHAR(5, 4))
                field = OF_L2_FT_MATCH_FIELD_VID_SA_DA;
            else if ('d' == TOKEN_CHAR(5, 4))
                field = OF_L2_FT_MATCH_FIELD_VID_DA_DA;
        }
    }
    else if ('s' == TOKEN_CHAR(5, 0))
    {
        if ('v' == TOKEN_CHAR(5, 3))
            field = OF_L2_FT_MATCH_FIELD_SA_VID_DA;
        else if ('d' == TOKEN_CHAR(5, 3))
            field = OF_L2_FT_MATCH_FIELD_SA_DA;
    }
    else if ('d' == TOKEN_CHAR(5, 0))
    {
        if ('v' == TOKEN_CHAR(5, 6))
        {
            if ('s' == TOKEN_CHAR(5, 10))
                field = OF_L2_FT_MATCH_FIELD_DA_SA_VID_SA;
            else if ('d' == TOKEN_CHAR(5, 10))
                field = OF_L2_FT_MATCH_FIELD_DA_SA_VID_DA;
        }
        else if ('s' == TOKEN_CHAR(5, 6))
            field = OF_L2_FT_MATCH_FIELD_DA_SA_SA;
        else if ('d' == TOKEN_CHAR(5, 6))
            field = OF_L2_FT_MATCH_FIELD_DA_SA_DA;
    }
    else if ('5' == TOKEN_CHAR(5, 0))
        field = OF_L2_FT_MATCH_FIELD_FIVE_TUPLE;

    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowTblMatchField_set(unit, field), ret);
    DIAG_UTIL_MPRINTF("Flow table l2 match field set to %s\n", text_l2_match_field[field]);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_flowtable_l2_match_field_vid_sa_sa_vid_sa_vid_da_vid_sa_da_sa_vid_da_sa_da_vid_da_da_da_sa_vid_sa_da_sa_sa_da_sa_vid_da_da_sa_da */
#endif

#ifdef CMD_OPENFLOW_GET_L2_ENTRY_TYPE_SMAC_DMAC_MAC_METADATA_NONE
/*
 * openflow get l2-entry type ( smac | dmac ) <MACADDR:mac> ( <UINT:metadata> | none )
 */
cparser_result_t
cparser_cmd_openflow_get_l2_entry_type_smac_dmac_mac_metadata_none(
    cparser_context_t *context,
    cparser_macaddr_t *mac_ptr,
    uint32_t *metadata_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l2FlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L2_HASH_CMD_PARSE(NULL, mac_ptr, mac_ptr, 6);
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_get(unit, &entry), ret);
    DUMP_L2_INSTRUCTION(entry, OF_INS_ACT_TYPE_END);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_get_l2_entry_type_smac_dmac_mac_metadata_none */
#endif

#ifdef CMD_OPENFLOW_GET_L2_ENTRY_TYPE_VID_SMAC_VID_DMAC_VID_MAC_METADATA_NONE
/*
 * openflow get l2-entry type ( vid-smac | vid-dmac ) <UINT:vid> <MACADDR:mac> ( <UINT:metadata> | none )
 */
cparser_result_t
cparser_cmd_openflow_get_l2_entry_type_vid_smac_vid_dmac_vid_mac_metadata_none(
    cparser_context_t *context,
    uint32_t *vid_ptr,
    cparser_macaddr_t *mac_ptr,
    uint32_t *metadata_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l2FlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    entry.matchfield.vidMac.vid = *vid_ptr;
    L2_HASH_CMD_PARSE(vid_ptr, mac_ptr, mac_ptr, 7);
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_get(unit, &entry), ret);
    DUMP_L2_INSTRUCTION(entry, OF_INS_ACT_TYPE_END);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_get_l2_entry_type_vid_smac_vid_dmac_vid_mac_metadata_none */
#endif

#ifdef CMD_OPENFLOW_GET_L2_ENTRY_TYPE_SMAC_DMAC_SMAC_DMAC_METADATA_NONE
/*
 * openflow get l2-entry type smac-dmac <MACADDR:smac> <MACADDR:dmac> ( <UINT:metadata> | none )
 */
cparser_result_t
cparser_cmd_openflow_get_l2_entry_type_smac_dmac_smac_dmac_metadata_none(
    cparser_context_t *context,
    cparser_macaddr_t *smac_ptr,
    cparser_macaddr_t *dmac_ptr,
    uint32_t *metadata_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l2FlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L2_HASH_CMD_PARSE(NULL, smac_ptr, dmac_ptr, 7);
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_get(unit, &entry), ret);
    DUMP_L2_INSTRUCTION(entry, OF_INS_ACT_TYPE_END);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_get_l2_entry_type_smac_dmac_smac_dmac_metadata_none */
#endif

#ifdef CMD_OPENFLOW_GET_L2_ENTRY_TYPE_5_TUPLE_IPPROTO_5_TUPLE_VID_SIP_DIP_L4_SPORT_L4_DPORT_IPPROTO_VID
/*
 * openflow get l2-entry type ( 5-tuple-ipproto | 5-tuple-vid ) <IPV4ADDR:sip> <IPV4ADDR:dip> <UINT:l4_sport> <UINT:l4_dport> <UINT:ipproto_vid>
 */
cparser_result_t
cparser_cmd_openflow_get_l2_entry_type_5_tuple_ipproto_5_tuple_vid_sip_dip_l4_sport_l4_dport_ipproto_vid(
    cparser_context_t *context,
    uint32_t *sip_ptr,
    uint32_t *dip_ptr,
    uint32_t *l4_sport_ptr,
    uint32_t *l4_dport_ptr,
    uint32_t *ipproto_vid_ptr)
{
    uint32  unit;
    int32   ret;
    rtk_of_l2FlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L2_HASH_5_TUPLE_CMD_PARSE(4, sip_ptr, dip_ptr, l4_sport_ptr, l4_dport_ptr, ipproto_vid_ptr);
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_get(unit, &entry), ret);
    DUMP_L2_INSTRUCTION(entry, OF_INS_ACT_TYPE_END);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_get_l2_entry_type_5_tuple_ipproto_5_tuple_vid_sip_dip_l4_sport_l4_dport_ipproto_vid */
#endif

#ifdef CMD_OPENFLOW_GET_L2_ENTRY_TYPE_SMAC_DMAC_MAC_METADATA_NONE_HIT_INDICATION
/*
 * openflow get l2-entry type ( smac | dmac ) <MACADDR:mac> ( <UINT:metadata> | none ) hit-indication
 */
cparser_result_t
cparser_cmd_openflow_get_l2_entry_type_smac_dmac_mac_metadata_none_hit_indication(
    cparser_context_t *context,
    cparser_macaddr_t *mac_ptr,
    uint32_t *metadata_ptr)
{
    uint32 unit, isHit;
    int32 ret;
    rtk_of_l2FlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L2_HASH_CMD_PARSE(NULL, mac_ptr, mac_ptr, 6);
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntryHitSts_get(unit, &entry, FALSE, &isHit), ret);
    DIAG_UTIL_MPRINTF("hit indication is %s\n\n", ((isHit==TRUE) ? "True" : "False"));

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_get_l2_entry_type_smac_dmac_mac_metadata_none_hit_indication */
#endif

#ifdef CMD_OPENFLOW_GET_L2_ENTRY_TYPE_VID_SMAC_VID_DMAC_VID_MAC_METADATA_NONE_HIT_INDICATION
/*
 * openflow get l2-entry type ( vid-smac | vid-dmac ) <UINT:vid> <MACADDR:mac> ( <UINT:metadata> | none ) hit-indication
 */
cparser_result_t
cparser_cmd_openflow_get_l2_entry_type_vid_smac_vid_dmac_vid_mac_metadata_none_hit_indication(
    cparser_context_t *context,
    uint32_t *vid_ptr,
    cparser_macaddr_t *mac_ptr,
    uint32_t *metadata_ptr)
{
    uint32 unit, isHit;
    int32 ret;
    rtk_of_l2FlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    entry.matchfield.vidMac.vid = *vid_ptr;
    L2_HASH_CMD_PARSE(vid_ptr, mac_ptr, mac_ptr, 7);
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntryHitSts_get(unit, &entry, FALSE, &isHit), ret);
    DIAG_UTIL_MPRINTF("hit indication is %s\n\n", ((isHit==TRUE) ? "True" : "False"));

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_get_l2_entry_type_vid_smac_vid_dmac_vid_mac_metadata_none_hit_indication */
#endif

#ifdef CMD_OPENFLOW_GET_L2_ENTRY_TYPE_SMAC_DMAC_SMAC_DMAC_METADATA_NONE_HIT_INDICATION
/*
 * openflow get l2-entry type smac-dmac <MACADDR:smac> <MACADDR:dmac> ( <UINT:metadata> | none ) hit-indication
 */
cparser_result_t
cparser_cmd_openflow_get_l2_entry_type_smac_dmac_smac_dmac_metadata_none_hit_indication(
    cparser_context_t *context,
    cparser_macaddr_t *smac_ptr,
    cparser_macaddr_t *dmac_ptr,
    uint32_t *metadata_ptr)
{
    uint32 unit, isHit;
    int32 ret;
    rtk_of_l2FlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L2_HASH_CMD_PARSE(NULL, smac_ptr, dmac_ptr, 7);
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntryHitSts_get(unit, &entry, FALSE, &isHit), ret);
    DIAG_UTIL_MPRINTF("hit indication is %s\n\n", ((isHit==TRUE) ? "True" : "False"));

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_get_l2_entry_type_smac_dmac_smac_dmac_metadata_none_hit_indication */
#endif

#ifdef CMD_OPENFLOW_GET_L2_ENTRY_TYPE_5_TUPLE_IPPROTO_5_TUPLE_VID_SIP_DIP_L4_SPORT_L4_DPORT_IPPROTO_VID_HIT_INDICATION
/*
 * openflow get l2-entry type ( 5-tuple-ipproto | 5-tuple-vid ) <IPV4ADDR:sip> <IPV4ADDR:dip> <UINT:l4_sport> <UINT:l4_dport> <UINT:ipproto_vid> hit-indication
 */
cparser_result_t
cparser_cmd_openflow_get_l2_entry_type_5_tuple_ipproto_5_tuple_vid_sip_dip_l4_sport_l4_dport_ipproto_vid_hit_indication(
    cparser_context_t *context,
    uint32_t *sip_ptr,
    uint32_t *dip_ptr,
    uint32_t *l4_sport_ptr,
    uint32_t *l4_dport_ptr,
    uint32_t *ipproto_vid_ptr)
{
    uint32  unit, isHit;
    int32   ret;
    rtk_of_l2FlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L2_HASH_5_TUPLE_CMD_PARSE(4, sip_ptr, dip_ptr, l4_sport_ptr, l4_dport_ptr, ipproto_vid_ptr);
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntryHitSts_get(unit, &entry, FALSE, &isHit), ret);
    DIAG_UTIL_MPRINTF("hit indication is %s\n\n", ((isHit==TRUE) ? "True" : "False"));

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_get_l2_entry_type_5_tuple_ipproto_5_tuple_vid_sip_dip_l4_sport_l4_dport_ipproto_vid_hit_indication */
#endif

#ifdef CMD_OPENFLOW_RESET_L2_ENTRY_TYPE_SMAC_DMAC_MAC_METADATA_NONE_HIT_INDICATION
/*
 * openflow reset l2-entry type ( smac | dmac ) <MACADDR:mac> ( <UINT:metadata> | none ) hit-indication
 */
cparser_result_t
cparser_cmd_openflow_reset_l2_entry_type_smac_dmac_mac_metadata_none_hit_indication(
    cparser_context_t *context,
    cparser_macaddr_t *mac_ptr,
    uint32_t *metadata_ptr)
{
    uint32 unit, isHit;
    int32 ret;
    rtk_of_l2FlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L2_HASH_CMD_PARSE(NULL, mac_ptr, mac_ptr, 6);
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntryHitSts_get(unit, &entry, TRUE, &isHit), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_reset_l2_entry_type_smac_dmac_mac_metadata_none_hit_indication */
#endif

#ifdef CMD_OPENFLOW_RESET_L2_ENTRY_TYPE_VID_SMAC_VID_DMAC_VID_MAC_METADATA_NONE_HIT_INDICATION
/*
 * openflow reset l2-entry type ( vid-smac | vid-dmac ) <UINT:vid> <MACADDR:mac> ( <UINT:metadata> | none ) hit-indication
 */
cparser_result_t
cparser_cmd_openflow_reset_l2_entry_type_vid_smac_vid_dmac_vid_mac_metadata_none_hit_indication(
    cparser_context_t *context,
    uint32_t *vid_ptr,
    cparser_macaddr_t *mac_ptr,
    uint32_t *metadata_ptr)
{
    uint32 unit, isHit;
    int32 ret;
    rtk_of_l2FlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    entry.matchfield.vidMac.vid = *vid_ptr;
    L2_HASH_CMD_PARSE(vid_ptr, mac_ptr, mac_ptr, 7);
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntryHitSts_get(unit, &entry, TRUE, &isHit), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_reset_l2_entry_type_vid_smac_vid_dmac_vid_mac_metadata_none_hit_indication */
#endif

#ifdef CMD_OPENFLOW_RESET_L2_ENTRY_TYPE_SMAC_DMAC_SMAC_DMAC_METADATA_NONE_HIT_INDICATION
/*
 * openflow reset l2-entry type smac-dmac <MACADDR:smac> <MACADDR:dmac> ( <UINT:metadata> | none ) hit-indication
 */
cparser_result_t
cparser_cmd_openflow_reset_l2_entry_type_smac_dmac_smac_dmac_metadata_none_hit_indication(
    cparser_context_t *context,
    cparser_macaddr_t *smac_ptr,
    cparser_macaddr_t *dmac_ptr,
    uint32_t *metadata_ptr)
{
    uint32 unit, isHit;
    int32 ret;
    rtk_of_l2FlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L2_HASH_CMD_PARSE(NULL, smac_ptr, dmac_ptr, 7);
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntryHitSts_get(unit, &entry, TRUE, &isHit), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_reset_l2_entry_type_smac_dmac_smac_dmac_metadata_none_hit_indication */
#endif

#ifdef CMD_OPENFLOW_RESET_L2_ENTRY_TYPE_5_TUPLE_IPPROTO_5_TUPLE_VID_SIP_DIP_L4_SPORT_L4_DPORT_IPPROTO_VID_HIT_INDICATION
/*
 * openflow reset l2-entry type ( 5-tuple-ipproto | 5-tuple-vid ) <IPV4ADDR:sip> <IPV4ADDR:dip> <UINT:l4_sport> <UINT:l4_dport> <UINT:ipproto_vid> hit-indication
 */
cparser_result_t
cparser_cmd_openflow_reset_l2_entry_type_5_tuple_ipproto_5_tuple_vid_sip_dip_l4_sport_l4_dport_ipproto_vid_hit_indication(
    cparser_context_t *context,
    uint32_t *sip_ptr,
    uint32_t *dip_ptr,
    uint32_t *l4_sport_ptr,
    uint32_t *l4_dport_ptr,
    uint32_t *ipproto_vid_ptr)
{
    uint32  unit, isHit;
    int32   ret;
    rtk_of_l2FlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L2_HASH_5_TUPLE_CMD_PARSE(4, sip_ptr, dip_ptr, l4_sport_ptr, l4_dport_ptr, ipproto_vid_ptr);
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntryHitSts_get(unit, &entry, TRUE, &isHit), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_reset_l2_entry_type_5_tuple_ipproto_5_tuple_vid_sip_dip_l4_sport_l4_dport_ipproto_vid_hit_indication */
#endif

#ifdef CMD_OPENFLOW_DUMP_L2_ENTRY
/*
 * openflow dump l2-entry
 */
cparser_result_t
cparser_cmd_openflow_dump_l2_entry(
    cparser_context_t *context)
{
    uint32 unit;
    int32 scan_idx = -1;
    int32 ret;
    uint32 totalEntries = 0;
    rtk_of_l2FlowEntry_t entry;
    char typeStr[20] = "";
    char sipStr[16] = "", dipStr[16]="";
    uint32 tmp = 0;

    DIAG_UTIL_FUNC_INIT(unit);

    diag_util_mprintf("L2 Flow Table Entries\n\n");
    diag_util_mprintf(" Index | Type            | Vid          | Source MAC        | Destination MAC   | MD_State | MD  \n");
    diag_util_mprintf(" Index | Type            | Vid/IPProto  | Source IP         | Destination IP    | L4 SPORT | L4 DPORT  \n");
    diag_util_mprintf("-----------------------------------------------------------------------------------------------------\n");

    while (1)
    {
        osal_memset(&entry, 0 , sizeof(entry));
        ret = rtk_of_l2FlowEntryNextValid_get(unit, &scan_idx, &entry);

        if (ret == RT_ERR_ENTRY_NOTFOUND || ret == RT_ERR_OUT_OF_RANGE)
            break;

        if (ret != RT_ERR_OK)
        {

            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }

        switch (entry.type)
        {
            case OF_L2_FLOW_ENTRY_TYPE_SA:
                 strcpy(typeStr, "SA");
                 break;
            case OF_L2_FLOW_ENTRY_TYPE_VID_SA:
                 strcpy(typeStr, "VID_SA");
                 break;
            case OF_L2_FLOW_ENTRY_TYPE_DA:
                 strcpy(typeStr, "DA");
                 break;
            case OF_L2_FLOW_ENTRY_TYPE_VID_DA:
                 strcpy(typeStr, "VID_DA");
                 break;
            case OF_L2_FLOW_ENTRY_TYPE_SA_DA:
                 strcpy(typeStr, "SA_DA");
                 break;
            case OF_L2_FLOW_ENTRY_TYPE_5_TUPLE_IPPROTO:
                 strcpy(typeStr, "5_TUPLE_IPPROTO");
                 break;
            case OF_L2_FLOW_ENTRY_TYPE_5_TUPLE_VID:
                 strcpy(typeStr, "5_TUPLE_VID");
                 break;
            default:
                 return CPARSER_NOT_OK;
        }

        if ((entry.type == OF_L2_FLOW_ENTRY_TYPE_5_TUPLE_IPPROTO) || (entry.type == OF_L2_FLOW_ENTRY_TYPE_5_TUPLE_VID))
        {
            if (entry.type == OF_L2_FLOW_ENTRY_TYPE_5_TUPLE_IPPROTO)
                tmp = entry.matchfield.fiveTp.fifthData.ipproto;
            else
                tmp = entry.matchfield.fiveTp.fifthData.vid;
            diag_util_ip2str(sipStr, entry.matchfield.fiveTp.sip);
            diag_util_ip2str(dipStr, entry.matchfield.fiveTp.dip);
            diag_util_mprintf(" %5d | %15s | %12d | %17s | %17s | %8u | %8u\n",\
                scan_idx, typeStr, tmp, sipStr, dipStr, entry.matchfield.fiveTp.l4_sport, entry.matchfield.fiveTp.l4_dport);
        }
        else
        {
            diag_util_mprintf(" %5d | %15s | %12d | %02X:%02X:%02X:%02X:%02X:%02X | %02X:%02X:%02X:%02X:%02X:%02X | %s | 0x%02X\n",\
                scan_idx, typeStr,entry.matchfield.vidMac.vid,\
                entry.matchfield.vidMac.smac.octet[0],entry.matchfield.vidMac.smac.octet[1],entry.matchfield.vidMac.smac.octet[2],\
                entry.matchfield.vidMac.smac.octet[3],entry.matchfield.vidMac.smac.octet[4],entry.matchfield.vidMac.smac.octet[5],\
                entry.matchfield.vidMac.dmac.octet[0],entry.matchfield.vidMac.dmac.octet[1],entry.matchfield.vidMac.dmac.octet[2],\
                entry.matchfield.vidMac.dmac.octet[3],entry.matchfield.vidMac.dmac.octet[4],entry.matchfield.vidMac.dmac.octet[5],\
                (entry.flags&RTK_OF_FLAG_FLOWENTRY_MD_CMP)?"enable  ":"disable ",entry.matchfield.vidMac.metadata);
        }

        totalEntries++;
    }

    diag_util_mprintf("\nTotal Number Of Entries : %d\n",totalEntries);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_dump_l2_entry */
#endif

#ifdef CMD_OPENFLOW_SET_L2_ENTRY_TYPE_SMAC_DMAC_MAC_METADATA_NONE_INSTRUCTION_CLEAR_ACTION_WRITE_ACTION_WRITE_METADATA_GOTO_TABLE_STATE_DISABLE_ENABLE
/*
 * openflow set l2-entry type ( smac | dmac ) <MACADDR:mac> ( <UINT:metadata> | none ) instruction ( clear-action | write-action | write-metadata | goto-table ) state ( disable | enable )
 */
cparser_result_t
cparser_cmd_openflow_set_l2_entry_type_smac_dmac_mac_metadata_none_instruction_clear_action_write_action_write_metadata_goto_table_state_disable_enable(
    cparser_context_t *context,
    cparser_macaddr_t *mac_ptr,
    uint32_t *metadata_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l2FlowEntry_t entry;
    rtk_of_ins_action_type_t type = OF_INS_ACT_TYPE_END;
    rtk_enable_t state = DISABLED;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L2_HASH_CMD_PARSE(NULL, mac_ptr, mac_ptr, 6);
    DIAG_OF_PHASE_INSTRUCTION(8, type);
    DIAG_UTIL_PARSE_STATE(10, state);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l2FlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    if (type == OF_INS_ACT_TYPE_CLEARACT)
        entry.ins.clearAct_en = state;
    else if (type == OF_INS_ACT_TYPE_WRITEACT)
        entry.ins.writeAct_en = state;
    else if (type == OF_INS_ACT_TYPE_WRITEMETA)
        entry.ins.writeMetadata_en = state;
    else if (type == OF_INS_ACT_TYPE_GOTOTBL)
        entry.ins.gotoTbl_en = state;

    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_add(unit, &entry), ret);
    DUMP_L2_INSTRUCTION(entry, type);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l2_entry_type_smac_dmac_mac_metadata_none_instruction_clear_action_write_action_write_metadata_goto_table_state_disable_enable */
#endif

#ifdef CMD_OPENFLOW_SET_L2_ENTRY_TYPE_VID_SMAC_VID_DMAC_VID_MAC_METADATA_NONE_INSTRUCTION_CLEAR_ACTION_WRITE_ACTION_WRITE_METADATA_GOTO_TABLE_STATE_DISABLE_ENABLE
/*
 * openflow set l2-entry type ( vid-smac | vid-dmac ) <UINT:vid> <MACADDR:mac> ( <UINT:metadata> | none ) instruction ( clear-action | write-action | write-metadata | goto-table ) state ( disable | enable )
 */
cparser_result_t
cparser_cmd_openflow_set_l2_entry_type_vid_smac_vid_dmac_vid_mac_metadata_none_instruction_clear_action_write_action_write_metadata_goto_table_state_disable_enable(
    cparser_context_t *context,
    uint32_t *vid_ptr,
    cparser_macaddr_t *mac_ptr,
    uint32_t *metadata_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l2FlowEntry_t entry;
    rtk_of_ins_action_type_t type = OF_INS_ACT_TYPE_END;
    rtk_enable_t state = DISABLED;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    entry.matchfield.vidMac.vid = *vid_ptr;
    L2_HASH_CMD_PARSE(vid_ptr, mac_ptr, mac_ptr, 7);
    DIAG_OF_PHASE_INSTRUCTION(9, type);
    DIAG_UTIL_PARSE_STATE(11, state);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l2FlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    if (type == OF_INS_ACT_TYPE_CLEARACT)
        entry.ins.clearAct_en = state;
    else if (type == OF_INS_ACT_TYPE_WRITEACT)
        entry.ins.writeAct_en = state;
    else if (type == OF_INS_ACT_TYPE_WRITEMETA)
        entry.ins.writeMetadata_en = state;
    else if (type == OF_INS_ACT_TYPE_GOTOTBL)
        entry.ins.gotoTbl_en = state;

    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_add(unit, &entry), ret);
    DUMP_L2_INSTRUCTION(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l2_entry_type_vid_smac_vid_dmac_vid_mac_metadata_none_instruction_clear_action_write_action_write_metadata_goto_table_state_disable_enable */
#endif

#ifdef CMD_OPENFLOW_SET_L2_ENTRY_TYPE_SMAC_DMAC_SMAC_DMAC_METADATA_NONE_INSTRUCTION_CLEAR_ACTION_WRITE_ACTION_WRITE_METADATA_GOTO_TABLE_STATE_DISABLE_ENABLE
/*
 * openflow set l2-entry type smac-dmac <MACADDR:smac> <MACADDR:dmac> ( <UINT:metadata> | none ) instruction ( clear-action | write-action | write-metadata | goto-table ) state ( disable | enable )
 */
cparser_result_t
cparser_cmd_openflow_set_l2_entry_type_smac_dmac_smac_dmac_metadata_none_instruction_clear_action_write_action_write_metadata_goto_table_state_disable_enable(
    cparser_context_t *context,
    cparser_macaddr_t *smac_ptr,
    cparser_macaddr_t *dmac_ptr,
    uint32_t *metadata_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l2FlowEntry_t entry;
    rtk_of_ins_action_type_t type = OF_INS_ACT_TYPE_END;
    rtk_enable_t state = DISABLED;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    entry.type = OF_L2_FLOW_ENTRY_TYPE_SA_DA;
    L2_HASH_CMD_PARSE(NULL, smac_ptr, dmac_ptr, 7);
    DIAG_OF_PHASE_INSTRUCTION(9, type);
    DIAG_UTIL_PARSE_STATE(11, state);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l2FlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    if (type == OF_INS_ACT_TYPE_CLEARACT)
        entry.ins.clearAct_en = state;
    else if (type == OF_INS_ACT_TYPE_WRITEACT)
        entry.ins.writeAct_en = state;
    else if (type == OF_INS_ACT_TYPE_WRITEMETA)
        entry.ins.writeMetadata_en = state;
    else if (type == OF_INS_ACT_TYPE_GOTOTBL)
        entry.ins.gotoTbl_en = state;

    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_add(unit, &entry), ret);
    DUMP_L2_INSTRUCTION(entry, type);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l2_entry_type_smac_dmac_smac_dmac_metadata_none_instruction_clear_action_write_action_write_metadata_goto_table_state_disable_enable */
#endif

#ifdef CMD_OPENFLOW_SET_L2_ENTRY_TYPE_5_TUPLE_IPPROTO_5_TUPLE_VID_SIP_DIP_L4_SPORT_L4_DPORT_IPPROTO_VID_INSTRUCTION_CLEAR_ACTION_WRITE_ACTION_WRITE_METADATA_GOTO_TABLE_STATE_DISABLE_ENABLE
/*
 * openflow set l2-entry type ( 5-tuple-ipproto | 5-tuple-vid ) <IPV4ADDR:sip> <IPV4ADDR:dip> <UINT:l4_sport> <UINT:l4_dport> <UINT:ipproto_vid> instruction ( clear-action | write-action | write-metadata | goto-table ) state ( disable | enable )
 */
cparser_result_t
cparser_cmd_openflow_set_l2_entry_type_5_tuple_ipproto_5_tuple_vid_sip_dip_l4_sport_l4_dport_ipproto_vid_instruction_clear_action_write_action_write_metadata_goto_table_state_disable_enable(
    cparser_context_t *context,
    uint32_t *sip_ptr,
    uint32_t *dip_ptr,
    uint32_t *l4_sport_ptr,
    uint32_t *l4_dport_ptr,
    uint32_t *ipproto_vid_ptr)
{
    uint32  unit;
    int32   ret;
    rtk_of_l2FlowEntry_t entry;
    rtk_of_ins_action_type_t type = OF_INS_ACT_TYPE_END;
    rtk_enable_t state = DISABLED;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L2_HASH_5_TUPLE_CMD_PARSE(4, sip_ptr, dip_ptr, l4_sport_ptr, l4_dport_ptr, ipproto_vid_ptr);
    DIAG_OF_PHASE_INSTRUCTION(11, type);
    DIAG_UTIL_PARSE_STATE(13, state);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l2FlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    if (type == OF_INS_ACT_TYPE_CLEARACT)
        entry.ins.clearAct_en = state;
    else if (type == OF_INS_ACT_TYPE_WRITEACT)
        entry.ins.writeAct_en = state;
    else if (type == OF_INS_ACT_TYPE_WRITEMETA)
        entry.ins.writeMetadata_en = state;
    else if (type == OF_INS_ACT_TYPE_GOTOTBL)
        entry.ins.gotoTbl_en = state;

    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_add(unit, &entry), ret);
    DUMP_L2_INSTRUCTION(entry, type);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l2_entry_type_5_tuple_ipproto_5_tuple_vid_sip_dip_l4_sport_l4_dport_ipproto_vid_instruction_clear_action_write_action_write_metadata_goto_table_state_disable_enable */
#endif

#ifdef CMD_OPENFLOW_SET_L2_ENTRY_TYPE_SMAC_DMAC_MAC_METADATA_NONE_INSTRUCTION_WRITE_ACTION_PUSH_VLAN_SET_QUEUE_STATE_DISABLE_ENABLE
/*
 * openflow set l2-entry type ( smac | dmac ) <MACADDR:mac> ( <UINT:metadata> | none ) instruction write-action ( push-vlan | set-queue ) state ( disable | enable )
 */
cparser_result_t
cparser_cmd_openflow_set_l2_entry_type_smac_dmac_mac_metadata_none_instruction_write_action_push_vlan_set_queue_state_disable_enable(
    cparser_context_t *context,
    cparser_macaddr_t *mac_ptr,
    uint32_t *metadata_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l2FlowEntry_t entry;
    rtk_enable_t state = DISABLED;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L2_HASH_CMD_PARSE(NULL, mac_ptr, mac_ptr, 6);
    DIAG_UTIL_PARSE_STATE(11, state);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l2FlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    if ('p' == TOKEN_CHAR(9, 0))
        entry.ins.wa_data.push_vlan = state;
    else if ('s' == TOKEN_CHAR(9, 0))
        entry.ins.wa_data.set_queue = state;

    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_add(unit, &entry), ret);
    DUMP_L2_INSTRUCTION(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l2_entry_type_smac_dmac_mac_metadata_none_instruction_write_action_push_vlan_set_queue_state_disable_enable */
#endif

#ifdef CMD_OPENFLOW_SET_L2_ENTRY_TYPE_VID_SMAC_VID_DMAC_VID_MAC_METADATA_NONE_INSTRUCTION_WRITE_ACTION_PUSH_VLAN_SET_QUEUE_STATE_DISABLE_ENABLE
/*
 * openflow set l2-entry type ( vid-smac | vid-dmac ) <UINT:vid> <MACADDR:mac> ( <UINT:metadata> | none ) instruction write-action ( push-vlan | set-queue ) state ( disable | enable )
 */
cparser_result_t
cparser_cmd_openflow_set_l2_entry_type_vid_smac_vid_dmac_vid_mac_metadata_none_instruction_write_action_push_vlan_set_queue_state_disable_enable(
    cparser_context_t *context,
    uint32_t *vid_ptr,
    cparser_macaddr_t *mac_ptr,
    uint32_t *metadata_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l2FlowEntry_t entry;
    rtk_enable_t state = DISABLED;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    entry.matchfield.vidMac.vid = *vid_ptr;
    L2_HASH_CMD_PARSE(vid_ptr, mac_ptr, mac_ptr, 7);
    DIAG_UTIL_PARSE_STATE(12, state);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l2FlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    if ('p' == TOKEN_CHAR(10, 0))
        entry.ins.wa_data.push_vlan = state;
    else if ('s' == TOKEN_CHAR(10, 0))
        entry.ins.wa_data.set_queue = state;

    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_add(unit, &entry), ret);
    DUMP_L2_INSTRUCTION(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l2_entry_type_vid_smac_vid_dmac_vid_mac_metadata_none_instruction_write_action_push_vlan_set_queue_state_disable_enable */
#endif

#ifdef CMD_OPENFLOW_SET_L2_ENTRY_TYPE_SMAC_DMAC_SMAC_DMAC_METADATA_NONE_INSTRUCTION_WRITE_ACTION_PUSH_VLAN_SET_QUEUE_STATE_DISABLE_ENABLE
/*
 * openflow set l2-entry type smac-dmac <MACADDR:smac> <MACADDR:dmac> ( <UINT:metadata> | none ) instruction write-action ( push-vlan | set-queue ) state ( disable | enable )
 */
cparser_result_t
cparser_cmd_openflow_set_l2_entry_type_smac_dmac_smac_dmac_metadata_none_instruction_write_action_push_vlan_set_queue_state_disable_enable(
    cparser_context_t *context,
    cparser_macaddr_t *smac_ptr,
    cparser_macaddr_t *dmac_ptr,
    uint32_t *metadata_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l2FlowEntry_t entry;
    rtk_enable_t state = DISABLED;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L2_HASH_CMD_PARSE(NULL, smac_ptr, dmac_ptr, 7);
    DIAG_UTIL_PARSE_STATE(12, state);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l2FlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    if ('p' == TOKEN_CHAR(10, 0))
        entry.ins.wa_data.push_vlan = state;
    else if ('s' == TOKEN_CHAR(10, 0))
        entry.ins.wa_data.set_queue = state;

    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_add(unit, &entry), ret);
    DUMP_L2_INSTRUCTION(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l2_entry_type_smac_dmac_smac_dmac_metadata_none_instruction_write_action_push_vlan_set_queue_state_disable_enable */
#endif

#ifdef CMD_OPENFLOW_SET_L2_ENTRY_TYPE_5_TUPLE_IPPROTO_5_TUPLE_VID_SIP_DIP_L4_SPORT_L4_DPORT_IPPROTO_VID_INSTRUCTION_WRITE_ACTION_PUSH_VLAN_SET_QUEUE_STATE_DISABLE_ENABLE
/*
 * openflow set l2-entry type ( 5-tuple-ipproto | 5-tuple-vid ) <IPV4ADDR:sip> <IPV4ADDR:dip> <UINT:l4_sport> <UINT:l4_dport> <UINT:ipproto_vid> instruction write-action ( push-vlan | set-queue ) state ( disable | enable )
 */
cparser_result_t
cparser_cmd_openflow_set_l2_entry_type_5_tuple_ipproto_5_tuple_vid_sip_dip_l4_sport_l4_dport_ipproto_vid_instruction_write_action_push_vlan_set_queue_state_disable_enable(
    cparser_context_t *context,
    uint32_t *sip_ptr,
    uint32_t *dip_ptr,
    uint32_t *l4_sport_ptr,
    uint32_t *l4_dport_ptr,
    uint32_t *ipproto_vid_ptr)
{
    uint32  unit;
    int32   ret;
    rtk_of_l2FlowEntry_t entry;
    rtk_enable_t state = DISABLED;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L2_HASH_5_TUPLE_CMD_PARSE(4, sip_ptr, dip_ptr, l4_sport_ptr, l4_dport_ptr, ipproto_vid_ptr);
    DIAG_UTIL_PARSE_STATE(14, state);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l2FlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    if ('p' == TOKEN_CHAR(12, 0))
        entry.ins.wa_data.push_vlan = state;
    else if ('s' == TOKEN_CHAR(12, 0))
        entry.ins.wa_data.set_queue = state;

    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_add(unit, &entry), ret);
    DUMP_L2_INSTRUCTION(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l2_entry_type_5_tuple_ipproto_5_tuple_vid_sip_dip_l4_sport_l4_dport_ipproto_vid_instruction_write_action_push_vlan_set_queue_state_disable_enable */
#endif

#ifdef CMD_OPENFLOW_SET_L2_ENTRY_TYPE_SMAC_DMAC_MAC_METADATA_NONE_INSTRUCTION_WRITE_ACTION_PUSH_VLAN_ETHERTYPE_TPID_IDX
/*
 * openflow set l2-entry type ( smac | dmac ) <MACADDR:mac> ( <UINT:metadata> | none ) instruction write-action push-vlan ethertype <UINT:tpid_idx>
 */
cparser_result_t
cparser_cmd_openflow_set_l2_entry_type_smac_dmac_mac_metadata_none_instruction_write_action_push_vlan_ethertype_tpid_idx(
    cparser_context_t *context,
    cparser_macaddr_t *mac_ptr,
    uint32_t *metadata_ptr,
    uint32_t *tpid_idx_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l2FlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L2_HASH_CMD_PARSE(NULL, mac_ptr, mac_ptr, 6);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l2FlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);
    entry.ins.wa_data.push_vlan_data.etherType_idx = *tpid_idx_ptr;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_add(unit, &entry), ret);
    DUMP_L2_INSTRUCTION(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l2_entry_type_smac_dmac_mac_metadata_none_instruction_write_action_push_vlan_ethertype_tpid_idx */
#endif

#ifdef CMD_OPENFLOW_SET_L2_ENTRY_TYPE_VID_SMAC_VID_DMAC_VID_MAC_METADATA_NONE_INSTRUCTION_WRITE_ACTION_PUSH_VLAN_ETHERTYPE_TPID_IDX
/*
 * openflow set l2-entry type ( vid-smac | vid-dmac ) <UINT:vid> <MACADDR:mac> ( <UINT:metadata> | none ) instruction write-action push-vlan ethertype <UINT:tpid_idx>
 */
cparser_result_t
cparser_cmd_openflow_set_l2_entry_type_vid_smac_vid_dmac_vid_mac_metadata_none_instruction_write_action_push_vlan_ethertype_tpid_idx(
    cparser_context_t *context,
    uint32_t *vid_ptr,
    cparser_macaddr_t *mac_ptr,
    uint32_t *metadata_ptr,
    uint32_t *tpid_idx_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l2FlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    entry.matchfield.vidMac.vid = *vid_ptr;
    L2_HASH_CMD_PARSE(vid_ptr, mac_ptr, mac_ptr, 7);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l2FlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);
    entry.ins.wa_data.push_vlan_data.etherType_idx = *tpid_idx_ptr;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_add(unit, &entry), ret);
    DUMP_L2_INSTRUCTION(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l2_entry_type_vid_smac_vid_dmac_vid_mac_metadata_none_instruction_write_action_push_vlan_ethertype_tpid_idx */
#endif

#ifdef CMD_OPENFLOW_SET_L2_ENTRY_TYPE_SMAC_DMAC_SMAC_DMAC_METADATA_NONE_INSTRUCTION_WRITE_ACTION_PUSH_VLAN_ETHERTYPE_TPID_IDX
/*
 * openflow set l2-entry type smac-dmac <MACADDR:smac> <MACADDR:dmac> ( <UINT:metadata> | none ) instruction write-action push-vlan ethertype <UINT:tpid_idx>
 */
cparser_result_t
cparser_cmd_openflow_set_l2_entry_type_smac_dmac_smac_dmac_metadata_none_instruction_write_action_push_vlan_ethertype_tpid_idx(
    cparser_context_t *context,
    cparser_macaddr_t *smac_ptr,
    cparser_macaddr_t *dmac_ptr,
    uint32_t *metadata_ptr,
    uint32_t *tpid_idx_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l2FlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L2_HASH_CMD_PARSE(NULL, smac_ptr, dmac_ptr, 7);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l2FlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);
    entry.ins.wa_data.push_vlan_data.etherType_idx = *tpid_idx_ptr;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_add(unit, &entry), ret);
    DUMP_L2_INSTRUCTION(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l2_entry_type_smac_dmac_smac_dmac_metadata_none_instruction_write_action_push_vlan_ethertype_tpid_idx */
#endif

#ifdef CMD_OPENFLOW_SET_L2_ENTRY_TYPE_5_TUPLE_IPPROTO_5_TUPLE_VID_SIP_DIP_L4_SPORT_L4_DPORT_IPPROTO_VID_INSTRUCTION_WRITE_ACTION_PUSH_VLAN_ETHERTYPE_TPID_IDX
/*
 * openflow set l2-entry type ( 5-tuple-ipproto | 5-tuple-vid ) <IPV4ADDR:sip> <IPV4ADDR:dip> <UINT:l4_sport> <UINT:l4_dport> <UINT:ipproto_vid> instruction write-action push-vlan ethertype <UINT:tpid_idx>
 */
cparser_result_t
cparser_cmd_openflow_set_l2_entry_type_5_tuple_ipproto_5_tuple_vid_sip_dip_l4_sport_l4_dport_ipproto_vid_instruction_write_action_push_vlan_ethertype_tpid_idx(
    cparser_context_t *context,
    uint32_t *sip_ptr,
    uint32_t *dip_ptr,
    uint32_t *l4_sport_ptr,
    uint32_t *l4_dport_ptr,
    uint32_t *ipproto_vid_ptr,
    uint32_t *tpid_idx_ptr)
{
    uint32  unit;
    int32   ret;
    rtk_of_l2FlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L2_HASH_5_TUPLE_CMD_PARSE(4, sip_ptr, dip_ptr, l4_sport_ptr, l4_dport_ptr, ipproto_vid_ptr);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l2FlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);
    entry.ins.wa_data.push_vlan_data.etherType_idx = *tpid_idx_ptr;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_add(unit, &entry), ret);
    DUMP_L2_INSTRUCTION(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l2_entry_type_5_tuple_ipproto_5_tuple_vid_sip_dip_l4_sport_l4_dport_ipproto_vid_instruction_write_action_push_vlan_ethertype_tpid_idx */
#endif

#ifdef CMD_OPENFLOW_SET_L2_ENTRY_TYPE_SMAC_DMAC_MAC_METADATA_NONE_INSTRUCTION_WRITE_ACTION_SET_FIELD_FIELD_ID_FIELD_TYPE_NONE
/*
 * openflow set l2-entry type ( smac | dmac ) <MACADDR:mac> ( <UINT:metadata> | none ) instruction write-action set-field <UINT:field_id> field-type none
 */
cparser_result_t
cparser_cmd_openflow_set_l2_entry_type_smac_dmac_mac_metadata_none_instruction_write_action_set_field_field_id_field_type_none(
    cparser_context_t *context,
    cparser_macaddr_t *mac_ptr,
    uint32_t *metadata_ptr,
    uint32_t *field_id_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l2FlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L2_HASH_CMD_PARSE(NULL, mac_ptr, mac_ptr, 6);
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntrySetField_check(unit, *field_id_ptr, OF_SET_FIELD_TYPE_NONE), ret);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l2FlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    if (0 == *field_id_ptr)
    {
        entry.ins.wa_data.set_field_0_data.field_type = OF_SET_FIELD_TYPE_NONE;
        entry.ins.wa_data.set_field_0_data.field_data= 0;
    }
    else
    {
        entry.ins.wa_data.set_field_1_data.field_type = OF_SET_FIELD_TYPE_NONE;
        entry.ins.wa_data.set_field_1_data.field_data= 0;
    }

    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_add(unit, &entry), ret);
    DUMP_L2_INSTRUCTION(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l2_entry_type_smac_dmac_mac_metadata_none_instruction_write_action_set_field_field_id_field_type_none */
#endif

#ifdef CMD_OPENFLOW_SET_L2_ENTRY_TYPE_VID_SMAC_VID_DMAC_VID_MAC_METADATA_NONE_INSTRUCTION_WRITE_ACTION_SET_FIELD_FIELD_ID_FIELD_TYPE_NONE
/*
 * openflow set l2-entry type ( vid-smac | vid-dmac ) <UINT:vid> <MACADDR:mac> ( <UINT:metadata> | none ) instruction write-action set-field <UINT:field_id> field-type none
 */
cparser_result_t
cparser_cmd_openflow_set_l2_entry_type_vid_smac_vid_dmac_vid_mac_metadata_none_instruction_write_action_set_field_field_id_field_type_none(
    cparser_context_t *context,
    uint32_t *vid_ptr,
    cparser_macaddr_t *mac_ptr,
    uint32_t *metadata_ptr,
    uint32_t *field_id_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l2FlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    entry.matchfield.vidMac.vid = *vid_ptr;
    L2_HASH_CMD_PARSE(vid_ptr, mac_ptr, mac_ptr, 7);
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntrySetField_check(unit, *field_id_ptr, OF_SET_FIELD_TYPE_NONE), ret);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l2FlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    if (0 == *field_id_ptr)
    {
        entry.ins.wa_data.set_field_0_data.field_type = OF_SET_FIELD_TYPE_NONE;
        entry.ins.wa_data.set_field_0_data.field_data= 0;
    }
    else
    {
        entry.ins.wa_data.set_field_1_data.field_type = OF_SET_FIELD_TYPE_NONE;
        entry.ins.wa_data.set_field_1_data.field_data= 0;
    }

    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_add(unit, &entry), ret);
    DUMP_L2_INSTRUCTION(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l2_entry_type_vid_smac_vid_dmac_vid_mac_metadata_none_instruction_write_action_set_field_field_id_field_type_none */
#endif

#ifdef CMD_OPENFLOW_SET_L2_ENTRY_TYPE_SMAC_DMAC_SMAC_DMAC_METADATA_NONE_INSTRUCTION_WRITE_ACTION_SET_FIELD_FIELD_ID_FIELD_TYPE_NONE
/*
 * openflow set l2-entry type smac-dmac <MACADDR:smac> <MACADDR:dmac> ( <UINT:metadata> | none ) instruction write-action set-field <UINT:field_id> field-type none
 */
cparser_result_t
cparser_cmd_openflow_set_l2_entry_type_smac_dmac_smac_dmac_metadata_none_instruction_write_action_set_field_field_id_field_type_none(
    cparser_context_t *context,
    cparser_macaddr_t *smac_ptr,
    cparser_macaddr_t *dmac_ptr,
    uint32_t *metadata_ptr,
    uint32_t *field_id_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l2FlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L2_HASH_CMD_PARSE(NULL, smac_ptr, dmac_ptr, 7);
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntrySetField_check(unit, *field_id_ptr, OF_SET_FIELD_TYPE_NONE), ret);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l2FlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    if (0 == *field_id_ptr)
    {
        entry.ins.wa_data.set_field_0_data.field_type = OF_SET_FIELD_TYPE_NONE;
        entry.ins.wa_data.set_field_0_data.field_data= 0;
    }
    else
    {
        entry.ins.wa_data.set_field_1_data.field_type = OF_SET_FIELD_TYPE_NONE;
        entry.ins.wa_data.set_field_1_data.field_data= 0;
    }

    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_add(unit, &entry), ret);
    DUMP_L2_INSTRUCTION(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l2_entry_type_smac_dmac_smac_dmac_metadata_none_instruction_write_action_set_field_field_id_field_type_none */
#endif

#ifdef CMD_OPENFLOW_SET_L2_ENTRY_TYPE_5_TUPLE_IPPROTO_5_TUPLE_VID_SIP_DIP_L4_SPORT_L4_DPORT_IPPROTO_VID_INSTRUCTION_WRITE_ACTION_SET_FIELD_FIELD_ID_FIELD_TYPE_NONE
/*
 * openflow set l2-entry type ( 5-tuple-ipproto | 5-tuple-vid ) <IPV4ADDR:sip> <IPV4ADDR:dip> <UINT:l4_sport> <UINT:l4_dport> <UINT:ipproto_vid> instruction write-action set-field <UINT:field_id> field-type none
 */
cparser_result_t
cparser_cmd_openflow_set_l2_entry_type_5_tuple_ipproto_5_tuple_vid_sip_dip_l4_sport_l4_dport_ipproto_vid_instruction_write_action_set_field_field_id_field_type_none(
    cparser_context_t *context,
    uint32_t *sip_ptr,
    uint32_t *dip_ptr,
    uint32_t *l4_sport_ptr,
    uint32_t *l4_dport_ptr,
    uint32_t *ipproto_vid_ptr,
    uint32_t *field_id_ptr)
{
    uint32  unit;
    int32   ret;
    rtk_of_l2FlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L2_HASH_5_TUPLE_CMD_PARSE(4, sip_ptr, dip_ptr, l4_sport_ptr, l4_dport_ptr, ipproto_vid_ptr);
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntrySetField_check(unit, *field_id_ptr, OF_SET_FIELD_TYPE_NONE), ret);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l2FlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    if (0 == *field_id_ptr)
    {
        entry.ins.wa_data.set_field_0_data.field_type = OF_SET_FIELD_TYPE_NONE;
        entry.ins.wa_data.set_field_0_data.field_data= 0;
    }
    else
    {
        entry.ins.wa_data.set_field_1_data.field_type = OF_SET_FIELD_TYPE_NONE;
        entry.ins.wa_data.set_field_1_data.field_data= 0;
    }

    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_add(unit, &entry), ret);
    DUMP_L2_INSTRUCTION(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l2_entry_type_5_tuple_ipproto_5_tuple_vid_sip_dip_l4_sport_l4_dport_ipproto_vid_instruction_write_action_set_field_field_id_field_type_none */
#endif

#ifdef CMD_OPENFLOW_SET_L2_ENTRY_TYPE_SMAC_DMAC_MAC_METADATA_NONE_INSTRUCTION_WRITE_ACTION_SET_FIELD_FIELD_ID_FIELD_TYPE_VLAN_PRIORITY_VID_DSCP_VALUE
/*
 * openflow set l2-entry type ( smac | dmac ) <MACADDR:mac> ( <UINT:metadata> | none ) instruction write-action set-field <UINT:field_id> field-type ( vlan-priority | vid | dscp ) <UINT:value>
 */
cparser_result_t
cparser_cmd_openflow_set_l2_entry_type_smac_dmac_mac_metadata_none_instruction_write_action_set_field_field_id_field_type_vlan_priority_vid_dscp_value(
    cparser_context_t *context,
    cparser_macaddr_t *mac_ptr,
    uint32_t *metadata_ptr,
    uint32_t *field_id_ptr,
    uint32_t *value_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l2FlowEntry_t entry;
    rtk_of_setFieldType_t type = OF_SET_FIELD_TYPE_END;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L2_HASH_CMD_PARSE(NULL, mac_ptr, mac_ptr, 6);
    if ('i' == TOKEN_CHAR(12, 1))
        type = OF_SET_FIELD_TYPE_VLAN_ID;
    else if ('s' == TOKEN_CHAR(12, 1))
        type = OF_SET_FIELD_TYPE_IP_DSCP;
    else if ('l' == TOKEN_CHAR(12, 1))
        type = OF_SET_FIELD_TYPE_VLAN_PRI;
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntrySetField_check(unit, *field_id_ptr, type), ret);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l2FlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    if (0 == *field_id_ptr)
    {
        entry.ins.wa_data.set_field_0_data.field_type = type;
        entry.ins.wa_data.set_field_0_data.field_data= *value_ptr;
    }
    else
    {
        entry.ins.wa_data.set_field_1_data.field_type = type;
        entry.ins.wa_data.set_field_1_data.field_data= *value_ptr;
    }

    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_add(unit, &entry), ret);
    DUMP_L2_INSTRUCTION(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l2_entry_type_smac_dmac_mac_metadata_none_instruction_write_action_set_field_field_id_field_type_vlan_priority_vid_dscp_value */
#endif

#ifdef CMD_OPENFLOW_SET_L2_ENTRY_TYPE_VID_SMAC_VID_DMAC_VID_MAC_METADATA_NONE_INSTRUCTION_WRITE_ACTION_SET_FIELD_FIELD_ID_FIELD_TYPE_VLAN_PRIORITY_VID_DSCP_VALUE
/*
 * openflow set l2-entry type ( vid-smac | vid-dmac ) <UINT:vid> <MACADDR:mac> ( <UINT:metadata> | none ) instruction write-action set-field <UINT:field_id> field-type ( vlan-priority | vid | dscp ) <UINT:value>
 */
cparser_result_t
cparser_cmd_openflow_set_l2_entry_type_vid_smac_vid_dmac_vid_mac_metadata_none_instruction_write_action_set_field_field_id_field_type_vlan_priority_vid_dscp_value(
    cparser_context_t *context,
    uint32_t *vid_ptr,
    cparser_macaddr_t *mac_ptr,
    uint32_t *metadata_ptr,
    uint32_t *field_id_ptr,
    uint32_t *value_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l2FlowEntry_t entry;
    rtk_of_setFieldType_t type = OF_SET_FIELD_TYPE_END;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    entry.matchfield.vidMac.vid = *vid_ptr;
    L2_HASH_CMD_PARSE(vid_ptr, mac_ptr, mac_ptr, 7);
    if ('i' == TOKEN_CHAR(13, 1))
        type = OF_SET_FIELD_TYPE_VLAN_ID;
    else if ('s' == TOKEN_CHAR(13, 1))
        type = OF_SET_FIELD_TYPE_IP_DSCP;
    else if ('l' == TOKEN_CHAR(13, 1))
        type = OF_SET_FIELD_TYPE_VLAN_PRI;
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntrySetField_check(unit, *field_id_ptr, type), ret);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l2FlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    if (0 == *field_id_ptr)
    {
        entry.ins.wa_data.set_field_0_data.field_type = type;
        entry.ins.wa_data.set_field_0_data.field_data= *value_ptr;
    }
    else
    {
        entry.ins.wa_data.set_field_1_data.field_type = type;
        entry.ins.wa_data.set_field_1_data.field_data= *value_ptr;
    }

    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_add(unit, &entry), ret);
    DUMP_L2_INSTRUCTION(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l2_entry_type_vid_smac_vid_dmac_vid_mac_metadata_none_instruction_write_action_set_field_field_id_field_type_vlan_priority_vid_dscp_value */
#endif

#ifdef CMD_OPENFLOW_SET_L2_ENTRY_TYPE_SMAC_DMAC_SMAC_DMAC_METADATA_NONE_INSTRUCTION_WRITE_ACTION_SET_FIELD_FIELD_ID_FIELD_TYPE_VLAN_PRIORITY_VID_DSCP_VALUE
/*
 * openflow set l2-entry type smac-dmac <MACADDR:smac> <MACADDR:dmac> ( <UINT:metadata> | none ) instruction write-action set-field <UINT:field_id> field-type ( vlan-priority | vid | dscp ) <UINT:value>
 */
cparser_result_t
cparser_cmd_openflow_set_l2_entry_type_smac_dmac_smac_dmac_metadata_none_instruction_write_action_set_field_field_id_field_type_vlan_priority_vid_dscp_value(
    cparser_context_t *context,
    cparser_macaddr_t *smac_ptr,
    cparser_macaddr_t *dmac_ptr,
    uint32_t *metadata_ptr,
    uint32_t *field_id_ptr,
    uint32_t *value_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l2FlowEntry_t entry;
    rtk_of_setFieldType_t type = OF_SET_FIELD_TYPE_END;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L2_HASH_CMD_PARSE(NULL, smac_ptr, dmac_ptr, 7);
    if ('i' == TOKEN_CHAR(13, 1))
        type = OF_SET_FIELD_TYPE_VLAN_ID;
    else if ('s' == TOKEN_CHAR(13, 1))
        type = OF_SET_FIELD_TYPE_IP_DSCP;
    else if ('l' == TOKEN_CHAR(13, 1))
        type = OF_SET_FIELD_TYPE_VLAN_PRI;
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntrySetField_check(unit, *field_id_ptr, type), ret);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l2FlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    if (0 == *field_id_ptr)
    {
        entry.ins.wa_data.set_field_0_data.field_type = type;
        entry.ins.wa_data.set_field_0_data.field_data= *value_ptr;
    }
    else
    {
        entry.ins.wa_data.set_field_1_data.field_type = type;
        entry.ins.wa_data.set_field_1_data.field_data= *value_ptr;
    }

    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_add(unit, &entry), ret);
    DUMP_L2_INSTRUCTION(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l2_entry_type_smac_dmac_smac_dmac_metadata_none_instruction_write_action_set_field_field_id_field_type_vlan_priority_vid_dscp_value */
#endif

#ifdef CMD_OPENFLOW_SET_L2_ENTRY_TYPE_5_TUPLE_IPPROTO_5_TUPLE_VID_SIP_DIP_L4_SPORT_L4_DPORT_IPPROTO_VID_INSTRUCTION_WRITE_ACTION_SET_FIELD_FIELD_ID_FIELD_TYPE_VLAN_PRIORITY_VID_DSCP_VALUE
/*
 * openflow set l2-entry type ( 5-tuple-ipproto | 5-tuple-vid ) <IPV4ADDR:sip> <IPV4ADDR:dip> <UINT:l4_sport> <UINT:l4_dport> <UINT:ipproto_vid> instruction write-action set-field <UINT:field_id> field-type ( vlan-priority | vid | dscp ) <UINT:value>
 */
cparser_result_t
cparser_cmd_openflow_set_l2_entry_type_5_tuple_ipproto_5_tuple_vid_sip_dip_l4_sport_l4_dport_ipproto_vid_instruction_write_action_set_field_field_id_field_type_vlan_priority_vid_dscp_value(
    cparser_context_t *context,
    uint32_t *sip_ptr,
    uint32_t *dip_ptr,
    uint32_t *l4_sport_ptr,
    uint32_t *l4_dport_ptr,
    uint32_t *ipproto_vid_ptr,
    uint32_t *field_id_ptr,
    uint32_t *value_ptr)
{
    uint32  unit;
    int32   ret;
    rtk_of_l2FlowEntry_t entry;
    rtk_of_setFieldType_t type = OF_SET_FIELD_TYPE_END;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L2_HASH_5_TUPLE_CMD_PARSE(4, sip_ptr, dip_ptr, l4_sport_ptr, l4_dport_ptr, ipproto_vid_ptr);
    if ('i' == TOKEN_CHAR(15, 1))
        type = OF_SET_FIELD_TYPE_VLAN_ID;
    else if ('s' == TOKEN_CHAR(15, 1))
        type = OF_SET_FIELD_TYPE_IP_DSCP;
    else if ('l' == TOKEN_CHAR(15, 1))
        type = OF_SET_FIELD_TYPE_VLAN_PRI;
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntrySetField_check(unit, *field_id_ptr, type), ret);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l2FlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    if (0 == *field_id_ptr)
    {
        entry.ins.wa_data.set_field_0_data.field_type = type;
        entry.ins.wa_data.set_field_0_data.field_data= *value_ptr;
    }
    else
    {
        entry.ins.wa_data.set_field_1_data.field_type = type;
        entry.ins.wa_data.set_field_1_data.field_data= *value_ptr;
    }

    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_add(unit, &entry), ret);
    DUMP_L2_INSTRUCTION(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l2_entry_type_5_tuple_ipproto_5_tuple_vid_sip_dip_l4_sport_l4_dport_ipproto_vid_instruction_write_action_set_field_field_id_field_type_vlan_priority_vid_dscp_value */
#endif

#ifdef CMD_OPENFLOW_SET_L2_ENTRY_TYPE_SMAC_DMAC_MAC_METADATA_NONE_INSTRUCTION_WRITE_ACTION_SET_QUEUE_QID
/*
 * openflow set l2-entry type ( smac | dmac ) <MACADDR:mac> ( <UINT:metadata> | none ) instruction write-action set-queue <UINT:qid>
 */
cparser_result_t
cparser_cmd_openflow_set_l2_entry_type_smac_dmac_mac_metadata_none_instruction_write_action_set_queue_qid(
    cparser_context_t *context,
    cparser_macaddr_t *mac_ptr,
    uint32_t *metadata_ptr,
    uint32_t *qid_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l2FlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L2_HASH_CMD_PARSE(NULL, mac_ptr, mac_ptr, 6);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l2FlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);
    entry.ins.wa_data.qid = *qid_ptr;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_add(unit, &entry), ret);
    DUMP_L2_INSTRUCTION(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l2_entry_type_smac_dmac_mac_metadata_none_instruction_write_action_set_queue_qid */
#endif

#ifdef CMD_OPENFLOW_SET_L2_ENTRY_TYPE_VID_SMAC_VID_DMAC_VID_MAC_METADATA_NONE_INSTRUCTION_WRITE_ACTION_SET_QUEUE_QID
/*
 * openflow set l2-entry type ( vid-smac | vid-dmac ) <UINT:vid> <MACADDR:mac> ( <UINT:metadata> | none ) instruction write-action set-queue <UINT:qid>
 */
cparser_result_t
cparser_cmd_openflow_set_l2_entry_type_vid_smac_vid_dmac_vid_mac_metadata_none_instruction_write_action_set_queue_qid(
    cparser_context_t *context,
    uint32_t *vid_ptr,
    cparser_macaddr_t *mac_ptr,
    uint32_t *metadata_ptr,
    uint32_t *qid_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l2FlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    entry.matchfield.vidMac.vid = *vid_ptr;
    L2_HASH_CMD_PARSE(vid_ptr, mac_ptr, mac_ptr, 7);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l2FlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);
    entry.ins.wa_data.qid = *qid_ptr;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_add(unit, &entry), ret);
    DUMP_L2_INSTRUCTION(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l2_entry_type_vid_smac_vid_dmac_vid_mac_metadata_none_instruction_write_action_set_queue_qid */
#endif

#ifdef CMD_OPENFLOW_SET_L2_ENTRY_TYPE_SMAC_DMAC_SMAC_DMAC_METADATA_NONE_INSTRUCTION_WRITE_ACTION_SET_QUEUE_QID
/*
 * openflow set l2-entry type smac-dmac <MACADDR:smac> <MACADDR:dmac> ( <UINT:metadata> | none ) instruction write-action set-queue <UINT:qid>
 */
cparser_result_t
cparser_cmd_openflow_set_l2_entry_type_smac_dmac_smac_dmac_metadata_none_instruction_write_action_set_queue_qid(
    cparser_context_t *context,
    cparser_macaddr_t *smac_ptr,
    cparser_macaddr_t *dmac_ptr,
    uint32_t *metadata_ptr,
    uint32_t *qid_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l2FlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L2_HASH_CMD_PARSE(NULL, smac_ptr, dmac_ptr, 7);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l2FlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);
    entry.ins.wa_data.qid = *qid_ptr;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_add(unit, &entry), ret);
    DUMP_L2_INSTRUCTION(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l2_entry_type_smac_dmac_smac_dmac_metadata_none_instruction_write_action_set_queue_qid */
#endif

#ifdef CMD_OPENFLOW_SET_L2_ENTRY_TYPE_5_TUPLE_IPPROTO_5_TUPLE_VID_SIP_DIP_L4_SPORT_L4_DPORT_IPPROTO_VID_INSTRUCTION_WRITE_ACTION_SET_QUEUE_QID
/*
 * openflow set l2-entry type ( 5-tuple-ipproto | 5-tuple-vid ) <IPV4ADDR:sip> <IPV4ADDR:dip> <UINT:l4_sport> <UINT:l4_dport> <UINT:ipproto_vid> instruction write-action set-queue <UINT:qid>
 */
cparser_result_t
cparser_cmd_openflow_set_l2_entry_type_5_tuple_ipproto_5_tuple_vid_sip_dip_l4_sport_l4_dport_ipproto_vid_instruction_write_action_set_queue_qid(
    cparser_context_t *context,
    uint32_t *sip_ptr,
    uint32_t *dip_ptr,
    uint32_t *l4_sport_ptr,
    uint32_t *l4_dport_ptr,
    uint32_t *ipproto_vid_ptr,
    uint32_t *qid_ptr)
{
    uint32  unit;
    int32   ret;
    rtk_of_l2FlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L2_HASH_5_TUPLE_CMD_PARSE(4, sip_ptr, dip_ptr, l4_sport_ptr, l4_dport_ptr, ipproto_vid_ptr);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l2FlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);
    entry.ins.wa_data.qid = *qid_ptr;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_add(unit, &entry), ret);
    DUMP_L2_INSTRUCTION(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l2_entry_type_5_tuple_ipproto_5_tuple_vid_sip_dip_l4_sport_l4_dport_ipproto_vid_instruction_write_action_set_queue_qid */
#endif

#ifdef CMD_OPENFLOW_SET_L2_ENTRY_TYPE_SMAC_DMAC_MAC_METADATA_NONE_INSTRUCTION_WRITE_ACTION_OUTPUT_NONE
/*
 * openflow set l2-entry type ( smac | dmac ) <MACADDR:mac> ( <UINT:metadata> | none ) instruction write-action output none
 */
cparser_result_t
cparser_cmd_openflow_set_l2_entry_type_smac_dmac_mac_metadata_none_instruction_write_action_output_none(
    cparser_context_t *context,
    cparser_macaddr_t *mac_ptr,
    uint32_t *metadata_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l2FlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L2_HASH_CMD_PARSE(NULL, mac_ptr, mac_ptr, 6);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l2FlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    entry.ins.wa_data.output_data.type = OF_IGR_FT1_ACT_OUTPUT_TYPE_NONE;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_add(unit, &entry), ret);
    DUMP_L2_INSTRUCTION(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l2_entry_type_smac_dmac_mac_metadata_none_instruction_write_action_output_none */
#endif

#ifdef CMD_OPENFLOW_SET_L2_ENTRY_TYPE_VID_SMAC_VID_DMAC_VID_MAC_METADATA_NONE_INSTRUCTION_WRITE_ACTION_OUTPUT_NONE
/*
 * openflow set l2-entry type ( vid-smac | vid-dmac ) <UINT:vid> <MACADDR:mac> ( <UINT:metadata> | none ) instruction write-action output none
 */
cparser_result_t
cparser_cmd_openflow_set_l2_entry_type_vid_smac_vid_dmac_vid_mac_metadata_none_instruction_write_action_output_none(
    cparser_context_t *context,
    uint32_t *vid_ptr,
    cparser_macaddr_t *mac_ptr,
    uint32_t *metadata_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l2FlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    entry.matchfield.vidMac.vid = *vid_ptr;
    L2_HASH_CMD_PARSE(vid_ptr, mac_ptr, mac_ptr, 7);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l2FlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);
    entry.ins.wa_data.output_data.type = OF_IGR_FT1_ACT_OUTPUT_TYPE_NONE;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_add(unit, &entry), ret);
    DUMP_L2_INSTRUCTION(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l2_entry_type_vid_smac_vid_dmac_vid_mac_metadata_none_instruction_write_action_output_none */
#endif

#ifdef CMD_OPENFLOW_SET_L2_ENTRY_TYPE_SMAC_DMAC_SMAC_DMAC_METADATA_NONE_INSTRUCTION_WRITE_ACTION_OUTPUT_NONE
/*
 * openflow set l2-entry type smac-dmac <MACADDR:smac> <MACADDR:dmac> ( <UINT:metadata> | none ) instruction write-action output none
 */
cparser_result_t
cparser_cmd_openflow_set_l2_entry_type_smac_dmac_smac_dmac_metadata_none_instruction_write_action_output_none(
    cparser_context_t *context,
    cparser_macaddr_t *smac_ptr,
    cparser_macaddr_t *dmac_ptr,
    uint32_t *metadata_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l2FlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L2_HASH_CMD_PARSE(NULL, smac_ptr, dmac_ptr, 7);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l2FlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);
    entry.ins.wa_data.output_data.type = OF_IGR_FT1_ACT_OUTPUT_TYPE_NONE;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_add(unit, &entry), ret);
    DUMP_L2_INSTRUCTION(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l2_entry_type_smac_dmac_smac_dmac_metadata_none_instruction_write_action_output_none */
#endif

#ifdef CMD_OPENFLOW_SET_L2_ENTRY_TYPE_5_TUPLE_IPPROTO_5_TUPLE_VID_SIP_DIP_L4_SPORT_L4_DPORT_IPPROTO_VID_INSTRUCTION_WRITE_ACTION_OUTPUT_NONE
/*
 * openflow set l2-entry type ( 5-tuple-ipproto | 5-tuple-vid ) <IPV4ADDR:sip> <IPV4ADDR:dip> <UINT:l4_sport> <UINT:l4_dport> <UINT:ipproto_vid> instruction write-action output none
 */
cparser_result_t
cparser_cmd_openflow_set_l2_entry_type_5_tuple_ipproto_5_tuple_vid_sip_dip_l4_sport_l4_dport_ipproto_vid_instruction_write_action_output_none(
    cparser_context_t *context,
    uint32_t *sip_ptr,
    uint32_t *dip_ptr,
    uint32_t *l4_sport_ptr,
    uint32_t *l4_dport_ptr,
    uint32_t *ipproto_vid_ptr)
{
    uint32  unit;
    int32   ret;
    rtk_of_l2FlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L2_HASH_5_TUPLE_CMD_PARSE(4, sip_ptr, dip_ptr, l4_sport_ptr, l4_dport_ptr, ipproto_vid_ptr);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l2FlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);
    entry.ins.wa_data.output_data.type = OF_IGR_FT1_ACT_OUTPUT_TYPE_NONE;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_add(unit, &entry), ret);
    DUMP_L2_INSTRUCTION(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l2_entry_type_5_tuple_ipproto_5_tuple_vid_sip_dip_l4_sport_l4_dport_ipproto_vid_instruction_write_action_output_none */
#endif

#ifdef CMD_OPENFLOW_SET_L2_ENTRY_TYPE_SMAC_DMAC_MAC_METADATA_NONE_INSTRUCTION_WRITE_ACTION_OUTPUT_PHY_PORT_SRC_PORT_FILTER_PORT_UID
/*
 * openflow set l2-entry type ( smac | dmac ) <MACADDR:mac> ( <UINT:metadata> | none ) instruction write-action output phy-port-src-port-filter <UINT:port> { <UINT:devid> }
 */
cparser_result_t
cparser_cmd_openflow_set_l2_entry_type_smac_dmac_mac_metadata_none_instruction_write_action_output_phy_port_src_port_filter_port_devid(
    cparser_context_t *context,
    cparser_macaddr_t *mac_ptr,
    uint32_t *metadata_ptr,
    uint32_t *port_ptr,
    uint32_t *devid_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l2FlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L2_HASH_CMD_PARSE(NULL, mac_ptr, mac_ptr, 6);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l2FlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);
    entry.ins.wa_data.output_data.type = OF_IGR_FT1_ACT_OUTPUT_TYPE_PHY_PORT;
    entry.ins.wa_data.output_data.port = *port_ptr;
    if (devid_ptr)
        entry.ins.wa_data.output_data.devID = *devid_ptr;

    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_add(unit, &entry), ret);
    DUMP_L2_INSTRUCTION(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l2_entry_type_smac_dmac_mac_metadata_none_instruction_write_action_output_phy_port_src_port_filter_port_devid */
#endif

#ifdef CMD_OPENFLOW_SET_L2_ENTRY_TYPE_VID_SMAC_VID_DMAC_VID_MAC_METADATA_NONE_INSTRUCTION_WRITE_ACTION_OUTPUT_PHY_PORT_SRC_PORT_FILTER_PORT_UID
/*
 * openflow set l2-entry type ( vid-smac | vid-dmac ) <UINT:vid> <MACADDR:mac> ( <UINT:metadata> | none ) instruction write-action output phy-port-src-port-filter <UINT:port> { <UINT:devid> }
 */
cparser_result_t
cparser_cmd_openflow_set_l2_entry_type_vid_smac_vid_dmac_vid_mac_metadata_none_instruction_write_action_output_phy_port_src_port_filter_port_devid(
    cparser_context_t *context,
    uint32_t *vid_ptr,
    cparser_macaddr_t *mac_ptr,
    uint32_t *metadata_ptr,
    uint32_t *port_ptr,
    uint32_t *devid_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l2FlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    entry.matchfield.vidMac.vid = *vid_ptr;
    L2_HASH_CMD_PARSE(vid_ptr, mac_ptr, mac_ptr, 7);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l2FlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);
    entry.ins.wa_data.output_data.type = OF_IGR_FT1_ACT_OUTPUT_TYPE_PHY_PORT;
    entry.ins.wa_data.output_data.port = *port_ptr;
    if (devid_ptr)
        entry.ins.wa_data.output_data.devID = *devid_ptr;

    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_add(unit, &entry), ret);
    DUMP_L2_INSTRUCTION(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l2_entry_type_vid_smac_vid_dmac_vid_mac_metadata_none_instruction_write_action_output_phy_port_src_port_filter_port_devid */
#endif

#ifdef CMD_OPENFLOW_SET_L2_ENTRY_TYPE_SMAC_DMAC_SMAC_DMAC_METADATA_NONE_INSTRUCTION_WRITE_ACTION_OUTPUT_PHY_PORT_SRC_PORT_FILTER_PORT_UID
/*
 * openflow set l2-entry type smac-dmac <MACADDR:smac> <MACADDR:dmac> ( <UINT:metadata> | none ) instruction write-action output phy-port-src-port-filter <UINT:port> { <UINT:devid> }
 */
cparser_result_t
cparser_cmd_openflow_set_l2_entry_type_smac_dmac_smac_dmac_metadata_none_instruction_write_action_output_phy_port_src_port_filter_port_devid(
    cparser_context_t *context,
    cparser_macaddr_t *smac_ptr,
    cparser_macaddr_t *dmac_ptr,
    uint32_t *metadata_ptr,
    uint32_t *port_ptr,
    uint32_t *devid_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l2FlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L2_HASH_CMD_PARSE(NULL, smac_ptr, dmac_ptr, 7);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l2FlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);
    entry.ins.wa_data.output_data.type = OF_IGR_FT1_ACT_OUTPUT_TYPE_PHY_PORT;
    entry.ins.wa_data.output_data.port = *port_ptr;
    if (devid_ptr)
        entry.ins.wa_data.output_data.devID = *devid_ptr;

    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_add(unit, &entry), ret);
    DUMP_L2_INSTRUCTION(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l2_entry_type_smac_dmac_smac_dmac_metadata_none_instruction_write_action_output_phy_port_src_port_filter_port_devid */
#endif

#ifdef CMD_OPENFLOW_SET_L2_ENTRY_TYPE_5_TUPLE_IPPROTO_5_TUPLE_VID_SIP_DIP_L4_SPORT_L4_DPORT_IPPROTO_VID_INSTRUCTION_WRITE_ACTION_OUTPUT_PHY_PORT_SRC_PORT_FILTER_PORT_UID
/*
 * openflow set l2-entry type ( 5-tuple-ipproto | 5-tuple-vid ) <IPV4ADDR:sip> <IPV4ADDR:dip> <UINT:l4_sport> <UINT:l4_dport> <UINT:ipproto_vid> instruction write-action output phy-port-src-port-filter <UINT:port> { <UINT:devid> }
 */
cparser_result_t
cparser_cmd_openflow_set_l2_entry_type_5_tuple_ipproto_5_tuple_vid_sip_dip_l4_sport_l4_dport_ipproto_vid_instruction_write_action_output_phy_port_src_port_filter_port_devid(
    cparser_context_t *context,
    uint32_t *sip_ptr,
    uint32_t *dip_ptr,
    uint32_t *l4_sport_ptr,
    uint32_t *l4_dport_ptr,
    uint32_t *ipproto_vid_ptr,
    uint32_t *port_ptr,
    uint32_t *devid_ptr)
{
    uint32  unit;
    int32   ret;
    rtk_of_l2FlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L2_HASH_5_TUPLE_CMD_PARSE(4, sip_ptr, dip_ptr, l4_sport_ptr, l4_dport_ptr, ipproto_vid_ptr);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l2FlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);
    entry.ins.wa_data.output_data.type = OF_IGR_FT1_ACT_OUTPUT_TYPE_PHY_PORT;
    entry.ins.wa_data.output_data.port = *port_ptr;
    if (devid_ptr)
        entry.ins.wa_data.output_data.devID = *devid_ptr;

    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_add(unit, &entry), ret);
    DUMP_L2_INSTRUCTION(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l2_entry_type_5_tuple_ipproto_5_tuple_vid_sip_dip_l4_sport_l4_dport_ipproto_vid_instruction_write_action_output_phy_port_src_port_filter_port_devid */
#endif

#ifdef CMD_OPENFLOW_SET_L2_ENTRY_TYPE_SMAC_DMAC_MAC_METADATA_NONE_INSTRUCTION_WRITE_ACTION_OUTPUT_TRUNK_SRC_PORT_FILTER_TID
/*
 * openflow set l2-entry type ( smac | dmac ) <MACADDR:mac> ( <UINT:metadata> | none ) instruction write-action output trunk-src-port-filter <UINT:tid>
 */
cparser_result_t
cparser_cmd_openflow_set_l2_entry_type_smac_dmac_mac_metadata_none_instruction_write_action_output_trunk_src_port_filter_tid(
    cparser_context_t *context,
    cparser_macaddr_t *mac_ptr,
    uint32_t *metadata_ptr,
    uint32_t *tid_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l2FlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L2_HASH_CMD_PARSE(NULL, mac_ptr, mac_ptr, 6);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l2FlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);
    entry.ins.wa_data.output_data.type = OF_IGR_FT1_ACT_OUTPUT_TYPE_TRK_PORT;
    entry.ins.wa_data.output_data.trunk = *tid_ptr;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_add(unit, &entry), ret);
    DUMP_L2_INSTRUCTION(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l2_entry_type_smac_dmac_mac_metadata_none_instruction_write_action_output_trunk_src_port_filter_tid */
#endif

#ifdef CMD_OPENFLOW_SET_L2_ENTRY_TYPE_VID_SMAC_VID_DMAC_VID_MAC_METADATA_NONE_INSTRUCTION_WRITE_ACTION_OUTPUT_TRUNK_SRC_PORT_FILTER_TID
/*
 * openflow set l2-entry type ( vid-smac | vid-dmac ) <UINT:vid> <MACADDR:mac> ( <UINT:metadata> | none ) instruction write-action output trunk-src-port-filter <UINT:tid>
 */
cparser_result_t
cparser_cmd_openflow_set_l2_entry_type_vid_smac_vid_dmac_vid_mac_metadata_none_instruction_write_action_output_trunk_src_port_filter_tid(
    cparser_context_t *context,
    uint32_t *vid_ptr,
    cparser_macaddr_t *mac_ptr,
    uint32_t *metadata_ptr,
    uint32_t *tid_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l2FlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    entry.matchfield.vidMac.vid = *vid_ptr;
    L2_HASH_CMD_PARSE(vid_ptr, mac_ptr, mac_ptr, 7);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l2FlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);
    entry.ins.wa_data.output_data.type = OF_IGR_FT1_ACT_OUTPUT_TYPE_TRK_PORT;
    entry.ins.wa_data.output_data.trunk = *tid_ptr;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_add(unit, &entry), ret);
    DUMP_L2_INSTRUCTION(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l2_entry_type_vid_smac_vid_dmac_vid_mac_metadata_none_instruction_write_action_output_trunk_src_port_filter_tid */
#endif

#ifdef CMD_OPENFLOW_SET_L2_ENTRY_TYPE_SMAC_DMAC_SMAC_DMAC_METADATA_NONE_INSTRUCTION_WRITE_ACTION_OUTPUT_TRUNK_SRC_PORT_FILTER_TID
/*
 * openflow set l2-entry type smac-dmac <MACADDR:smac> <MACADDR:dmac> ( <UINT:metadata> | none ) instruction write-action output trunk-src-port-filter <UINT:tid>
 */
cparser_result_t
cparser_cmd_openflow_set_l2_entry_type_smac_dmac_smac_dmac_metadata_none_instruction_write_action_output_trunk_src_port_filter_tid(
    cparser_context_t *context,
    cparser_macaddr_t *smac_ptr,
    cparser_macaddr_t *dmac_ptr,
    uint32_t *metadata_ptr,
    uint32_t *tid_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l2FlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L2_HASH_CMD_PARSE(NULL, smac_ptr, dmac_ptr, 7);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l2FlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);
    entry.ins.wa_data.output_data.type = OF_IGR_FT1_ACT_OUTPUT_TYPE_TRK_PORT;
    entry.ins.wa_data.output_data.trunk = *tid_ptr;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_add(unit, &entry), ret);
    DUMP_L2_INSTRUCTION(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l2_entry_type_smac_dmac_smac_dmac_metadata_none_instruction_write_action_output_trunk_src_port_filter_tid */
#endif

#ifdef CMD_OPENFLOW_SET_L2_ENTRY_TYPE_5_TUPLE_IPPROTO_5_TUPLE_VID_SIP_DIP_L4_SPORT_L4_DPORT_IPPROTO_VID_INSTRUCTION_WRITE_ACTION_OUTPUT_TRUNK_SRC_PORT_FILTER_TID
/*
 * openflow set l2-entry type ( 5-tuple-ipproto | 5-tuple-vid ) <IPV4ADDR:sip> <IPV4ADDR:dip> <UINT:l4_sport> <UINT:l4_dport> <UINT:ipproto_vid> instruction write-action output trunk-src-port-filter <UINT:tid>
 */
cparser_result_t
cparser_cmd_openflow_set_l2_entry_type_5_tuple_ipproto_5_tuple_vid_sip_dip_l4_sport_l4_dport_ipproto_vid_instruction_write_action_output_trunk_src_port_filter_tid(
    cparser_context_t *context,
    uint32_t *sip_ptr,
    uint32_t *dip_ptr,
    uint32_t *l4_sport_ptr,
    uint32_t *l4_dport_ptr,
    uint32_t *ipproto_vid_ptr,
    uint32_t *tid_ptr)
{
    uint32  unit;
    int32   ret;
    rtk_of_l2FlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L2_HASH_5_TUPLE_CMD_PARSE(4, sip_ptr, dip_ptr, l4_sport_ptr, l4_dport_ptr, ipproto_vid_ptr);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l2FlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);
    entry.ins.wa_data.output_data.type = OF_IGR_FT1_ACT_OUTPUT_TYPE_TRK_PORT;
    entry.ins.wa_data.output_data.trunk = *tid_ptr;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_add(unit, &entry), ret);
    DUMP_L2_INSTRUCTION(entry, OF_INS_ACT_TYPE_WRITEACT);;

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l2_entry_type_5_tuple_ipproto_5_tuple_vid_sip_dip_l4_sport_l4_dport_ipproto_vid_instruction_write_action_output_trunk_src_port_filter_tid */
#endif

#ifdef CMD_OPENFLOW_SET_L2_ENTRY_TYPE_SMAC_DMAC_MAC_METADATA_NONE_INSTRUCTION_WRITE_ACTION_OUTPUT_MULTI_EGRESS_PORT_PORT_ALL
/*
 * openflow set l2-entry type ( smac | dmac ) <MACADDR:mac> ( <UINT:metadata> | none ) instruction write-action output multi-egress-port ( <PORT_LIST:port> | all )
 */
cparser_result_t
cparser_cmd_openflow_set_l2_entry_type_smac_dmac_mac_metadata_none_instruction_write_action_output_multi_egress_port_port_all(
    cparser_context_t *context,
    cparser_macaddr_t *mac_ptr,
    uint32_t *metadata_ptr,
    char **port_ptr)
{
    uint32 unit;
    int32 ret;
    diag_portlist_t portlist;
    rtk_of_l2FlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    osal_memset(&portlist, 0, sizeof(portlist));
    L2_HASH_CMD_PARSE(NULL, mac_ptr, mac_ptr, 6);
    DIAG_UTIL_EXTRACT_PORTLIST(portlist, 11);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l2FlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);
    entry.ins.wa_data.output_data.type = OF_IGR_FT1_ACT_OUTPUT_TYPE_MULT_EGR_PORT;
    entry.ins.wa_data.output_data.portmask = portlist.portmask;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_add(unit, &entry), ret);
    DUMP_L2_INSTRUCTION(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l2_entry_type_smac_dmac_mac_metadata_none_instruction_write_action_output_multi_egress_port_port_all */
#endif

#ifdef CMD_OPENFLOW_SET_L2_ENTRY_TYPE_VID_SMAC_VID_DMAC_VID_MAC_METADATA_NONE_INSTRUCTION_WRITE_ACTION_OUTPUT_MULTI_EGRESS_PORT_PORT_ALL
/*
 * openflow set l2-entry type ( vid-smac | vid-dmac ) <UINT:vid> <MACADDR:mac> ( <UINT:metadata> | none ) instruction write-action output multi-egress-port ( <PORT_LIST:port> | all )
 */
cparser_result_t
cparser_cmd_openflow_set_l2_entry_type_vid_smac_vid_dmac_vid_mac_metadata_none_instruction_write_action_output_multi_egress_port_port_all(
    cparser_context_t *context,
    uint32_t *vid_ptr,
    cparser_macaddr_t *mac_ptr,
    uint32_t *metadata_ptr,
    char **port_ptr)
{
    uint32 unit;
    int32 ret;
    diag_portlist_t portlist;
    rtk_of_l2FlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    osal_memset(&portlist, 0, sizeof(portlist));
    entry.matchfield.vidMac.vid = *vid_ptr;
    L2_HASH_CMD_PARSE(vid_ptr, mac_ptr, mac_ptr, 7);
    DIAG_UTIL_EXTRACT_PORTLIST(portlist, 12);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l2FlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);
    entry.ins.wa_data.output_data.type = OF_IGR_FT1_ACT_OUTPUT_TYPE_MULT_EGR_PORT;
    entry.ins.wa_data.output_data.portmask = portlist.portmask;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_add(unit, &entry), ret);
    DUMP_L2_INSTRUCTION(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l2_entry_type_vid_smac_vid_dmac_vid_mac_metadata_none_instruction_write_action_output_multi_egress_port_port_all */
#endif

#ifdef CMD_OPENFLOW_SET_L2_ENTRY_TYPE_SMAC_DMAC_SMAC_DMAC_METADATA_NONE_INSTRUCTION_WRITE_ACTION_OUTPUT_MULTI_EGRESS_PORT_PORT_ALL
/*
 * openflow set l2-entry type smac-dmac <MACADDR:smac> <MACADDR:dmac> ( <UINT:metadata> | none ) instruction write-action output multi-egress-port ( <PORT_LIST:port> | all )
 */
cparser_result_t
cparser_cmd_openflow_set_l2_entry_type_smac_dmac_smac_dmac_metadata_none_instruction_write_action_output_multi_egress_port_port_all(
    cparser_context_t *context,
    cparser_macaddr_t *smac_ptr,
    cparser_macaddr_t *dmac_ptr,
    uint32_t *metadata_ptr,
    char **port_ptr)
{
    uint32 unit;
    int32 ret;
    diag_portlist_t portlist;
    rtk_of_l2FlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    osal_memset(&portlist, 0, sizeof(portlist));
    L2_HASH_CMD_PARSE(NULL, smac_ptr, dmac_ptr, 7);
    DIAG_UTIL_EXTRACT_PORTLIST(portlist, 12);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l2FlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);
    entry.ins.wa_data.output_data.type = OF_IGR_FT1_ACT_OUTPUT_TYPE_MULT_EGR_PORT;
    entry.ins.wa_data.output_data.portmask = portlist.portmask;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_add(unit, &entry), ret);
    DUMP_L2_INSTRUCTION(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l2_entry_type_smac_dmac_smac_dmac_metadata_none_instruction_write_action_output_multi_egress_port_port_all */
#endif

#ifdef CMD_OPENFLOW_SET_L2_ENTRY_TYPE_5_TUPLE_IPPROTO_5_TUPLE_VID_SIP_DIP_L4_SPORT_L4_DPORT_IPPROTO_VID_INSTRUCTION_WRITE_ACTION_OUTPUT_MULTI_EGRESS_PORT_PORT_ALL
/*
 * openflow set l2-entry type ( 5-tuple-ipproto | 5-tuple-vid ) <IPV4ADDR:sip> <IPV4ADDR:dip> <UINT:l4_sport> <UINT:l4_dport> <UINT:ipproto_vid> instruction write-action output multi-egress-port ( <PORT_LIST:port> | all )
 */
cparser_result_t
cparser_cmd_openflow_set_l2_entry_type_5_tuple_ipproto_5_tuple_vid_sip_dip_l4_sport_l4_dport_ipproto_vid_instruction_write_action_output_multi_egress_port_port_all(
    cparser_context_t *context,
    uint32_t *sip_ptr,
    uint32_t *dip_ptr,
    uint32_t *l4_sport_ptr,
    uint32_t *l4_dport_ptr,
    uint32_t *ipproto_vid_ptr,
    char **port_ptr)
{
    uint32  unit;
    int32   ret;
    diag_portlist_t portlist;
    rtk_of_l2FlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    osal_memset(&portlist, 0, sizeof(portlist));
    L2_HASH_5_TUPLE_CMD_PARSE(4, sip_ptr, dip_ptr, l4_sport_ptr, l4_dport_ptr, ipproto_vid_ptr);
    DIAG_UTIL_EXTRACT_PORTLIST(portlist, 14);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l2FlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);
    entry.ins.wa_data.output_data.type = OF_IGR_FT1_ACT_OUTPUT_TYPE_MULT_EGR_PORT;
    entry.ins.wa_data.output_data.portmask = portlist.portmask;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_add(unit, &entry), ret);
    DUMP_L2_INSTRUCTION(entry, OF_INS_ACT_TYPE_WRITEACT);;

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l2_entry_type_5_tuple_ipproto_5_tuple_vid_sip_dip_l4_sport_l4_dport_ipproto_vid_instruction_write_action_output_multi_egress_port_port_all */
#endif

#ifdef CMD_OPENFLOW_SET_L2_ENTRY_TYPE_SMAC_DMAC_MAC_METADATA_NONE_INSTRUCTION_WRITE_ACTION_OUTPUT_INGRESS_PORT
/*
 * openflow set l2-entry type ( smac | dmac ) <MACADDR:mac> ( <UINT:metadata> | none ) instruction write-action output ingress-port
 */
cparser_result_t
cparser_cmd_openflow_set_l2_entry_type_smac_dmac_mac_metadata_none_instruction_write_action_output_ingress_port(
    cparser_context_t *context,
    cparser_macaddr_t *mac_ptr,
    uint32_t *metadata_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l2FlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L2_HASH_CMD_PARSE(NULL, mac_ptr, mac_ptr, 6);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l2FlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);
    entry.ins.wa_data.output_data.type = OF_IGR_FT1_ACT_OUTPUT_TYPE_IN_PORT;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_add(unit, &entry), ret);
    DUMP_L2_INSTRUCTION(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l2_entry_type_smac_dmac_mac_metadata_none_instruction_write_action_output_ingress_port */
#endif

#ifdef CMD_OPENFLOW_SET_L2_ENTRY_TYPE_VID_SMAC_VID_DMAC_VID_MAC_METADATA_NONE_INSTRUCTION_WRITE_ACTION_OUTPUT_INGRESS_PORT
/*
 * openflow set l2-entry type ( vid-smac | vid-dmac ) <UINT:vid> <MACADDR:mac> ( <UINT:metadata> | none ) instruction write-action output ingress-port
 */
cparser_result_t
cparser_cmd_openflow_set_l2_entry_type_vid_smac_vid_dmac_vid_mac_metadata_none_instruction_write_action_output_ingress_port(
    cparser_context_t *context,
    uint32_t *vid_ptr,
    cparser_macaddr_t *mac_ptr,
    uint32_t *metadata_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l2FlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    entry.matchfield.vidMac.vid = *vid_ptr;
    L2_HASH_CMD_PARSE(vid_ptr, mac_ptr, mac_ptr, 7);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l2FlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);
    entry.ins.wa_data.output_data.type = OF_IGR_FT1_ACT_OUTPUT_TYPE_IN_PORT;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_add(unit, &entry), ret);
    DUMP_L2_INSTRUCTION(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l2_entry_type_vid_smac_vid_dmac_vid_mac_metadata_none_instruction_write_action_output_ingress_port */
#endif

#ifdef CMD_OPENFLOW_SET_L2_ENTRY_TYPE_SMAC_DMAC_SMAC_DMAC_METADATA_NONE_INSTRUCTION_WRITE_ACTION_OUTPUT_INGRESS_PORT
/*
 * openflow set l2-entry type smac-dmac <MACADDR:smac> <MACADDR:dmac> ( <UINT:metadata> | none ) instruction write-action output ingress-port
 */
cparser_result_t
cparser_cmd_openflow_set_l2_entry_type_smac_dmac_smac_dmac_metadata_none_instruction_write_action_output_ingress_port(
    cparser_context_t *context,
    cparser_macaddr_t *smac_ptr,
    cparser_macaddr_t *dmac_ptr,
    uint32_t *metadata_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l2FlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L2_HASH_CMD_PARSE(NULL, smac_ptr, dmac_ptr, 7);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l2FlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);
    entry.ins.wa_data.output_data.type = OF_IGR_FT1_ACT_OUTPUT_TYPE_IN_PORT;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_add(unit, &entry), ret);
    DUMP_L2_INSTRUCTION(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l2_entry_type_smac_dmac_smac_dmac_metadata_none_instruction_write_action_output_ingress_port */
#endif

#ifdef CMD_OPENFLOW_SET_L2_ENTRY_TYPE_5_TUPLE_IPPROTO_5_TUPLE_VID_SIP_DIP_L4_SPORT_L4_DPORT_IPPROTO_VID_INSTRUCTION_WRITE_ACTION_OUTPUT_INGRESS_PORT
/*
 * openflow set l2-entry type ( 5-tuple-ipproto | 5-tuple-vid ) <IPV4ADDR:sip> <IPV4ADDR:dip> <UINT:l4_sport> <UINT:l4_dport> <UINT:ipproto_vid> instruction write-action output ingress-port
 */
cparser_result_t
cparser_cmd_openflow_set_l2_entry_type_5_tuple_ipproto_5_tuple_vid_sip_dip_l4_sport_l4_dport_ipproto_vid_instruction_write_action_output_ingress_port(
    cparser_context_t *context,
    uint32_t *sip_ptr,
    uint32_t *dip_ptr,
    uint32_t *l4_sport_ptr,
    uint32_t *l4_dport_ptr,
    uint32_t *ipproto_vid_ptr)
{
    uint32  unit;
    int32   ret;
    rtk_of_l2FlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L2_HASH_5_TUPLE_CMD_PARSE(4, sip_ptr, dip_ptr, l4_sport_ptr, l4_dport_ptr, ipproto_vid_ptr);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l2FlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);
    entry.ins.wa_data.output_data.type = OF_IGR_FT1_ACT_OUTPUT_TYPE_IN_PORT;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_add(unit, &entry), ret);
    DUMP_L2_INSTRUCTION(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l2_entry_type_5_tuple_ipproto_5_tuple_vid_sip_dip_l4_sport_l4_dport_ipproto_vid_instruction_write_action_output_ingress_port */
#endif

#ifdef CMD_OPENFLOW_SET_L2_ENTRY_TYPE_SMAC_DMAC_MAC_METADATA_NONE_INSTRUCTION_WRITE_ACTION_OUTPUT_FLOOD
/*
 * openflow set l2-entry type ( smac | dmac ) <MACADDR:mac> ( <UINT:metadata> | none ) instruction write-action output flood
 */
cparser_result_t
cparser_cmd_openflow_set_l2_entry_type_smac_dmac_mac_metadata_none_instruction_write_action_output_flood(
    cparser_context_t *context,
    cparser_macaddr_t *mac_ptr,
    uint32_t *metadata_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l2FlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L2_HASH_CMD_PARSE(NULL, mac_ptr, mac_ptr, 6);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l2FlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);
    entry.ins.wa_data.output_data.type = OF_IGR_FT1_ACT_OUTPUT_TYPE_FLOOD;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_add(unit, &entry), ret);
    DUMP_L2_INSTRUCTION(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l2_entry_type_smac_dmac_mac_metadata_none_instruction_write_action_output_flood */
#endif

#ifdef CMD_OPENFLOW_SET_L2_ENTRY_TYPE_VID_SMAC_VID_DMAC_VID_MAC_METADATA_NONE_INSTRUCTION_WRITE_ACTION_OUTPUT_FLOOD
/*
 * openflow set l2-entry type ( vid-smac | vid-dmac ) <UINT:vid> <MACADDR:mac> ( <UINT:metadata> | none ) instruction write-action output flood
 */
cparser_result_t
cparser_cmd_openflow_set_l2_entry_type_vid_smac_vid_dmac_vid_mac_metadata_none_instruction_write_action_output_flood(
    cparser_context_t *context,
    uint32_t *vid_ptr,
    cparser_macaddr_t *mac_ptr,
    uint32_t *metadata_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l2FlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    entry.matchfield.vidMac.vid = *vid_ptr;
    L2_HASH_CMD_PARSE(vid_ptr, mac_ptr, mac_ptr, 7);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l2FlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);
    entry.ins.wa_data.output_data.type = OF_IGR_FT1_ACT_OUTPUT_TYPE_FLOOD;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_add(unit, &entry), ret);
    DUMP_L2_INSTRUCTION(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l2_entry_type_vid_smac_vid_dmac_vid_mac_metadata_none_instruction_write_action_output_flood */
#endif

#ifdef CMD_OPENFLOW_SET_L2_ENTRY_TYPE_SMAC_DMAC_SMAC_DMAC_METADATA_NONE_INSTRUCTION_WRITE_ACTION_OUTPUT_FLOOD
/*
 * openflow set l2-entry type smac-dmac <MACADDR:smac> <MACADDR:dmac> ( <UINT:metadata> | none ) instruction write-action output flood
 */
cparser_result_t
cparser_cmd_openflow_set_l2_entry_type_smac_dmac_smac_dmac_metadata_none_instruction_write_action_output_flood(
    cparser_context_t *context,
    cparser_macaddr_t *smac_ptr,
    cparser_macaddr_t *dmac_ptr,
    uint32_t *metadata_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l2FlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L2_HASH_CMD_PARSE(NULL, smac_ptr, dmac_ptr, 7);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l2FlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);
    entry.ins.wa_data.output_data.type = OF_IGR_FT1_ACT_OUTPUT_TYPE_FLOOD;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_add(unit, &entry), ret);
    DUMP_L2_INSTRUCTION(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l2_entry_type_smac_dmac_smac_dmac_metadata_none_instruction_write_action_output_flood */
#endif

#ifdef CMD_OPENFLOW_SET_L2_ENTRY_TYPE_5_TUPLE_IPPROTO_5_TUPLE_VID_SIP_DIP_L4_SPORT_L4_DPORT_IPPROTO_VID_INSTRUCTION_WRITE_ACTION_OUTPUT_FLOOD
/*
 * openflow set l2-entry type ( 5-tuple-ipproto | 5-tuple-vid ) <IPV4ADDR:sip> <IPV4ADDR:dip> <UINT:l4_sport> <UINT:l4_dport> <UINT:ipproto_vid> instruction write-action output flood
 */
cparser_result_t
cparser_cmd_openflow_set_l2_entry_type_5_tuple_ipproto_5_tuple_vid_sip_dip_l4_sport_l4_dport_ipproto_vid_instruction_write_action_output_flood(
    cparser_context_t *context,
    uint32_t *sip_ptr,
    uint32_t *dip_ptr,
    uint32_t *l4_sport_ptr,
    uint32_t *l4_dport_ptr,
    uint32_t *ipproto_vid_ptr)
{
    uint32  unit;
    int32   ret;
    rtk_of_l2FlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L2_HASH_5_TUPLE_CMD_PARSE(4, sip_ptr, dip_ptr, l4_sport_ptr, l4_dport_ptr, ipproto_vid_ptr);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l2FlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);
    entry.ins.wa_data.output_data.type = OF_IGR_FT1_ACT_OUTPUT_TYPE_FLOOD;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_add(unit, &entry), ret);
    DUMP_L2_INSTRUCTION(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l2_entry_type_5_tuple_ipproto_5_tuple_vid_sip_dip_l4_sport_l4_dport_ipproto_vid_instruction_write_action_output_flood */
#endif

#ifdef CMD_OPENFLOW_SET_L2_ENTRY_TYPE_SMAC_DMAC_MAC_METADATA_NONE_INSTRUCTION_WRITE_ACTION_OUTPUT_NORMAL
/*
 * openflow set l2-entry type ( smac | dmac ) <MACADDR:mac> ( <UINT:metadata> | none ) instruction write-action output normal
 */
cparser_result_t
cparser_cmd_openflow_set_l2_entry_type_smac_dmac_mac_metadata_none_instruction_write_action_output_normal(
    cparser_context_t *context,
    cparser_macaddr_t *mac_ptr,
    uint32_t *metadata_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l2FlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L2_HASH_CMD_PARSE(NULL, mac_ptr, mac_ptr, 6);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l2FlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);
    entry.ins.wa_data.output_data.type = OF_IGR_FT1_ACT_OUTPUT_TYPE_LB;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_add(unit, &entry), ret);
    DUMP_L2_INSTRUCTION(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l2_entry_type_smac_dmac_mac_metadata_none_instruction_write_action_output_normal */
#endif

#ifdef CMD_OPENFLOW_SET_L2_ENTRY_TYPE_VID_SMAC_VID_DMAC_VID_MAC_METADATA_NONE_INSTRUCTION_WRITE_ACTION_OUTPUT_NORMAL
/*
 * openflow set l2-entry type ( vid-smac | vid-dmac ) <UINT:vid> <MACADDR:mac> ( <UINT:metadata> | none ) instruction write-action output normal
 */
cparser_result_t
cparser_cmd_openflow_set_l2_entry_type_vid_smac_vid_dmac_vid_mac_metadata_none_instruction_write_action_output_normal(
    cparser_context_t *context,
    uint32_t *vid_ptr,
    cparser_macaddr_t *mac_ptr,
    uint32_t *metadata_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l2FlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    entry.matchfield.vidMac.vid = *vid_ptr;
    L2_HASH_CMD_PARSE(vid_ptr, mac_ptr, mac_ptr, 7);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l2FlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);
    entry.ins.wa_data.output_data.type = OF_IGR_FT1_ACT_OUTPUT_TYPE_LB;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_add(unit, &entry), ret);
    DUMP_L2_INSTRUCTION(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;;
}   /* end of cparser_cmd_openflow_set_l2_entry_type_vid_smac_vid_dmac_vid_mac_metadata_none_instruction_write_action_output_normal */
#endif

#ifdef CMD_OPENFLOW_SET_L2_ENTRY_TYPE_SMAC_DMAC_SMAC_DMAC_METADATA_NONE_INSTRUCTION_WRITE_ACTION_OUTPUT_NORMAL
/*
 * openflow set l2-entry type smac-dmac <MACADDR:smac> <MACADDR:dmac> ( <UINT:metadata> | none ) instruction write-action output normal
 */
cparser_result_t
cparser_cmd_openflow_set_l2_entry_type_smac_dmac_smac_dmac_metadata_none_instruction_write_action_output_normal(
    cparser_context_t *context,
    cparser_macaddr_t *smac_ptr,
    cparser_macaddr_t *dmac_ptr,
    uint32_t *metadata_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l2FlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L2_HASH_CMD_PARSE(NULL, smac_ptr, dmac_ptr, 7);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l2FlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);
    entry.ins.wa_data.output_data.type = OF_IGR_FT1_ACT_OUTPUT_TYPE_LB;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_add(unit, &entry), ret);
    DUMP_L2_INSTRUCTION(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l2_entry_type_smac_dmac_smac_dmac_metadata_none_instruction_write_action_output_normal */
#endif

#ifdef CMD_OPENFLOW_SET_L2_ENTRY_TYPE_5_TUPLE_IPPROTO_5_TUPLE_VID_SIP_DIP_L4_SPORT_L4_DPORT_IPPROTO_VID_INSTRUCTION_WRITE_ACTION_OUTPUT_NORMAL
/*
 * openflow set l2-entry type ( 5-tuple-ipproto | 5-tuple-vid ) <IPV4ADDR:sip> <IPV4ADDR:dip> <UINT:l4_sport> <UINT:l4_dport> <UINT:ipproto_vid> instruction write-action output normal
 */
cparser_result_t
cparser_cmd_openflow_set_l2_entry_type_5_tuple_ipproto_5_tuple_vid_sip_dip_l4_sport_l4_dport_ipproto_vid_instruction_write_action_output_normal(
    cparser_context_t *context,
    uint32_t *sip_ptr,
    uint32_t *dip_ptr,
    uint32_t *l4_sport_ptr,
    uint32_t *l4_dport_ptr,
    uint32_t *ipproto_vid_ptr)
{
    uint32  unit;
    int32   ret;
    rtk_of_l2FlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L2_HASH_5_TUPLE_CMD_PARSE(4, sip_ptr, dip_ptr, l4_sport_ptr, l4_dport_ptr, ipproto_vid_ptr);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l2FlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);
    entry.ins.wa_data.output_data.type = OF_IGR_FT1_ACT_OUTPUT_TYPE_LB;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_add(unit, &entry), ret);
    DUMP_L2_INSTRUCTION(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l2_entry_type_5_tuple_ipproto_5_tuple_vid_sip_dip_l4_sport_l4_dport_ipproto_vid_instruction_write_action_output_normal */
#endif

#ifdef CMD_OPENFLOW_SET_L2_ENTRY_TYPE_SMAC_DMAC_MAC_METADATA_NONE_INSTRUCTION_WRITE_ACTION_OUTPUT_TUNNEL_IFID
/*
 * openflow set l2-entry type ( smac | dmac ) <MACADDR:mac> ( <UINT:metadata> | none ) instruction write-action output tunnel <UINT:ifid>
 */
cparser_result_t
cparser_cmd_openflow_set_l2_entry_type_smac_dmac_mac_metadata_none_instruction_write_action_output_tunnel_ifid(
    cparser_context_t *context,
    cparser_macaddr_t *mac_ptr,
    uint32_t *metadata_ptr,
    uint32_t *ifid_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l2FlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L2_HASH_CMD_PARSE(NULL, mac_ptr, mac_ptr, 6);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l2FlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);
    entry.ins.wa_data.output_data.type = OF_IGR_FT1_ACT_OUTPUT_TYPE_TUNNEL;
    entry.ins.wa_data.output_data.intf = *ifid_ptr;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_add(unit, &entry), ret);
    DUMP_L2_INSTRUCTION(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l2_entry_type_smac_dmac_mac_metadata_none_instruction_write_action_output_tunnel_ifid */
#endif

#ifdef CMD_OPENFLOW_SET_L2_ENTRY_TYPE_VID_SMAC_VID_DMAC_VID_MAC_METADATA_NONE_INSTRUCTION_WRITE_ACTION_OUTPUT_TUNNEL_IFID
/*
 * openflow set l2-entry type ( vid-smac | vid-dmac ) <UINT:vid> ( <UINT:metadata> | none ) instruction write-action output tunnel <UINT:ifid>
 */
cparser_result_t
cparser_cmd_openflow_set_l2_entry_type_vid_smac_vid_dmac_vid_mac_metadata_none_instruction_write_action_output_tunnel_ifid(
    cparser_context_t *context,
    uint32_t *vid_ptr,
    cparser_macaddr_t *mac_ptr,
    uint32_t *metadata_ptr,
    uint32_t *ifid_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l2FlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    entry.matchfield.vidMac.vid = *vid_ptr;
    L2_HASH_CMD_PARSE(vid_ptr, mac_ptr, mac_ptr, 7);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l2FlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);
    entry.ins.wa_data.output_data.type = OF_IGR_FT1_ACT_OUTPUT_TYPE_TUNNEL;
    entry.ins.wa_data.output_data.intf = *ifid_ptr;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_add(unit, &entry), ret);
    DUMP_L2_INSTRUCTION(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l2_entry_type_vid_smac_vid_dmac_vid_metadata_none_instruction_write_action_output_tunnel_ifid */
#endif

#ifdef CMD_OPENFLOW_SET_L2_ENTRY_TYPE_SMAC_DMAC_SMAC_DMAC_METADATA_NONE_INSTRUCTION_WRITE_ACTION_OUTPUT_TUNNEL_IFID
/*
 * openflow set l2-entry type smac-dmac <MACADDR:smac> <MACADDR:dmac> ( <UINT:metadata> | none ) instruction write-action output tunnel <UINT:ifid>
 */
cparser_result_t
cparser_cmd_openflow_set_l2_entry_type_smac_dmac_smac_dmac_metadata_none_instruction_write_action_output_tunnel_ifid(
    cparser_context_t *context,
    cparser_macaddr_t *smac_ptr,
    cparser_macaddr_t *dmac_ptr,
    uint32_t *metadata_ptr,
    uint32_t *ifid_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l2FlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L2_HASH_CMD_PARSE(NULL, smac_ptr, dmac_ptr, 7);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l2FlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);
    entry.ins.wa_data.output_data.type = OF_IGR_FT1_ACT_OUTPUT_TYPE_TUNNEL;
    entry.ins.wa_data.output_data.intf = *ifid_ptr;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_add(unit, &entry), ret);
    DUMP_L2_INSTRUCTION(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l2_entry_type_smac_dmac_smac_dmac_metadata_none_instruction_write_action_output_tunnel_ifid */
#endif

#ifdef CMD_OPENFLOW_SET_L2_ENTRY_TYPE_5_TUPLE_IPPROTO_5_TUPLE_VID_SIP_DIP_L4_SPORT_L4_DPORT_IPPROTO_VID_INSTRUCTION_WRITE_ACTION_OUTPUT_TUNNEL_IFID
/*
 * openflow set l2-entry type ( 5-tuple-ipproto | 5-tuple-vid ) <IPV4ADDR:sip> <IPV4ADDR:dip> <UINT:l4_sport> <UINT:l4_dport> <UINT:ipproto_vid> instruction write-action output tunnel <UINT:ifid>
 */
cparser_result_t
cparser_cmd_openflow_set_l2_entry_type_5_tuple_ipproto_5_tuple_vid_sip_dip_l4_sport_l4_dport_ipproto_vid_instruction_write_action_output_tunnel_ifid(
    cparser_context_t *context,
    uint32_t *sip_ptr,
    uint32_t *dip_ptr,
    uint32_t *l4_sport_ptr,
    uint32_t *l4_dport_ptr,
    uint32_t *ipproto_vid_ptr,
    uint32_t *ifid_ptr)
{
    uint32  unit;
    int32   ret;
    rtk_of_l2FlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L2_HASH_5_TUPLE_CMD_PARSE(4, sip_ptr, dip_ptr, l4_sport_ptr, l4_dport_ptr, ipproto_vid_ptr);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l2FlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);
    entry.ins.wa_data.output_data.type = OF_IGR_FT1_ACT_OUTPUT_TYPE_TUNNEL;
    entry.ins.wa_data.output_data.intf = *ifid_ptr;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_add(unit, &entry), ret);
    DUMP_L2_INSTRUCTION(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l2_entry_type_5_tuple_ipproto_5_tuple_vid_sip_dip_l4_sport_l4_dport_ipproto_vid_instruction_write_action_output_tunnel_ifid */
#endif

#ifdef CMD_OPENFLOW_SET_L2_ENTRY_TYPE_SMAC_DMAC_MAC_METADATA_NONE_INSTRUCTION_WRITE_METADATA_DATA_MASK
/*
 * openflow set l2-entry type ( smac | dmac ) <MACADDR:mac> ( <UINT:metadata> | none ) instruction write-metadata <UINT:data>  <UINT:mask>
 */
cparser_result_t
cparser_cmd_openflow_set_l2_entry_type_smac_dmac_mac_metadata_none_instruction_write_metadata_data_mask(
    cparser_context_t *context,
    cparser_macaddr_t *mac_ptr,
    uint32_t *metadata_ptr,
    uint32_t *data_ptr,
    uint32_t *mask_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l2FlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L2_HASH_CMD_PARSE(NULL, mac_ptr, mac_ptr, 6);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l2FlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);
    entry.ins.wm_data.data = *data_ptr;
    entry.ins.wm_data.mask = *mask_ptr;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_add(unit, &entry), ret);
    DUMP_L2_INSTRUCTION(entry, OF_INS_ACT_TYPE_WRITEMETA);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l2_entry_type_smac_dmac_mac_metadata_none_instruction_write_metadata_data_mask */
#endif

#ifdef CMD_OPENFLOW_SET_L2_ENTRY_TYPE_VID_SMAC_VID_DMAC_VID_MAC_METADATA_NONE_INSTRUCTION_WRITE_METADATA_DATA_MASK
/*
 * openflow set l2-entry type ( vid-smac | vid-dmac ) <UINT:vid> ( <UINT:metadata> | none ) instruction write-metadata <UINT:data>  <UINT:mask>
 */
cparser_result_t
cparser_cmd_openflow_set_l2_entry_type_vid_smac_vid_dmac_vid_mac_metadata_none_instruction_write_metadata_data_mask(
    cparser_context_t *context,
    uint32_t *vid_ptr,
    cparser_macaddr_t *mac_ptr,
    uint32_t *metadata_ptr,
    uint32_t *data_ptr,
    uint32_t *mask_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l2FlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    entry.matchfield.vidMac.vid = *vid_ptr;
    L2_HASH_CMD_PARSE(vid_ptr, mac_ptr, mac_ptr, 7);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l2FlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);
    entry.ins.wm_data.data = *data_ptr;
    entry.ins.wm_data.mask = *mask_ptr;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_add(unit, &entry), ret);
    DUMP_L2_INSTRUCTION(entry, OF_INS_ACT_TYPE_WRITEMETA);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l2_entry_type_vid_smac_vid_dmac_vid_metadata_none_instruction_write_metadata_data_mask */
#endif

#ifdef CMD_OPENFLOW_SET_L2_ENTRY_TYPE_SMAC_DMAC_SMAC_DMAC_METADATA_NONE_INSTRUCTION_WRITE_METADATA_DATA_MASK
/*
 * openflow set l2-entry type smac-dmac <MACADDR:smac> <MACADDR:dmac> ( <UINT:metadata> | none ) instruction write-metadata <UINT:data>  <UINT:mask>
 */
cparser_result_t
cparser_cmd_openflow_set_l2_entry_type_smac_dmac_smac_dmac_metadata_none_instruction_write_metadata_data_mask(
    cparser_context_t *context,
    cparser_macaddr_t *smac_ptr,
    cparser_macaddr_t *dmac_ptr,
    uint32_t *metadata_ptr,
    uint32_t *data_ptr,
    uint32_t *mask_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l2FlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L2_HASH_CMD_PARSE(NULL, smac_ptr, dmac_ptr, 7);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l2FlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);
    entry.ins.wm_data.data = *data_ptr;
    entry.ins.wm_data.mask = *mask_ptr;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_add(unit, &entry), ret);
    DUMP_L2_INSTRUCTION(entry, OF_INS_ACT_TYPE_WRITEMETA);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l2_entry_type_smac_dmac_smac_dmac_metadata_none_instruction_write_metadata_data_mask */
#endif

#ifdef CMD_OPENFLOW_SET_L2_ENTRY_TYPE_5_TUPLE_IPPROTO_5_TUPLE_VID_SIP_DIP_L4_SPORT_L4_DPORT_IPPROTO_VID_INSTRUCTION_WRITE_METADATA_DATA_MASK
/*
 * openflow set l2-entry type ( 5-tuple-ipproto | 5-tuple-vid ) <IPV4ADDR:sip> <IPV4ADDR:dip> <UINT:l4_sport> <UINT:l4_dport> <UINT:ipproto_vid> instruction write-metadata <UINT:data>  <UINT:mask>
 */
cparser_result_t
cparser_cmd_openflow_set_l2_entry_type_5_tuple_ipproto_5_tuple_vid_sip_dip_l4_sport_l4_dport_ipproto_vid_instruction_write_metadata_data_mask(
    cparser_context_t *context,
    uint32_t *sip_ptr,
    uint32_t *dip_ptr,
    uint32_t *l4_sport_ptr,
    uint32_t *l4_dport_ptr,
    uint32_t *ipproto_vid_ptr,
    uint32_t *data_ptr,
    uint32_t *mask_ptr)
{
    uint32  unit;
    int32   ret;
    rtk_of_l2FlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L2_HASH_5_TUPLE_CMD_PARSE(4, sip_ptr, dip_ptr, l4_sport_ptr, l4_dport_ptr, ipproto_vid_ptr);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l2FlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);
    entry.ins.wm_data.data = *data_ptr;
    entry.ins.wm_data.mask = *mask_ptr;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_add(unit, &entry), ret);
    DUMP_L2_INSTRUCTION(entry, OF_INS_ACT_TYPE_WRITEMETA);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l2_entry_type_5_tuple_ipproto_5_tuple_vid_sip_dip_l4_sport_l4_dport_ipproto_vid_instruction_write_metadata_data_mask */
#endif

#ifdef CMD_OPENFLOW_SET_L2_ENTRY_TYPE_SMAC_DMAC_MAC_METADATA_NONE_INSTRUCTION_GOTO_TABLE_NORMAL_TID
/*
 * openflow set l2-entry type ( smac | dmac ) <MACADDR:mac> ( <UINT:metadata> | none ) instruction goto-table normal <UINT:tid>
 */
cparser_result_t
cparser_cmd_openflow_set_l2_entry_type_smac_dmac_mac_metadata_none_instruction_goto_table_normal_tid(
    cparser_context_t *context,
    cparser_macaddr_t *mac_ptr,
    uint32_t *metadata_ptr,
    uint32_t *tid_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l2FlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L2_HASH_CMD_PARSE(NULL, mac_ptr, mac_ptr, 6);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l2FlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);
    entry.ins.gt_data.type = OF_GOTOTBL_TYPE_NORMAL;
    entry.ins.gt_data.tbl_id = *tid_ptr;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_add(unit, &entry), ret);
    DUMP_L2_INSTRUCTION(entry, OF_INS_ACT_TYPE_GOTOTBL);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l2_entry_type_smac_dmac_mac_metadata_none_instruction_goto_table_normal_tid */
#endif

#ifdef CMD_OPENFLOW_SET_L2_ENTRY_TYPE_VID_SMAC_VID_DMAC_VID_MAC_METADATA_NONE_INSTRUCTION_GOTO_TABLE_NORMAL_TID
/*
 * openflow set l2-entry type ( vid-smac | vid-dmac ) <UINT:vid> ( <UINT:metadata> | none ) instruction goto-table normal <UINT:tid>
 */
cparser_result_t
cparser_cmd_openflow_set_l2_entry_type_vid_smac_vid_dmac_vid_mac_metadata_none_instruction_goto_table_normal_tid(
    cparser_context_t *context,
    uint32_t *vid_ptr,
    cparser_macaddr_t *mac_ptr,
    uint32_t *metadata_ptr,
    uint32_t *tid_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l2FlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    entry.matchfield.vidMac.vid = *vid_ptr;
    L2_HASH_CMD_PARSE(vid_ptr, mac_ptr, mac_ptr, 7);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l2FlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);
    entry.ins.gt_data.type = OF_GOTOTBL_TYPE_NORMAL;
    entry.ins.gt_data.tbl_id = *tid_ptr;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_add(unit, &entry), ret);
    DUMP_L2_INSTRUCTION(entry, OF_INS_ACT_TYPE_GOTOTBL);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l2_entry_type_vid_smac_vid_dmac_vid_metadata_none_instruction_goto_table_normal_tid */
#endif

#ifdef CMD_OPENFLOW_SET_L2_ENTRY_TYPE_SMAC_DMAC_SMAC_DMAC_METADATA_NONE_INSTRUCTION_GOTO_TABLE_NORMAL_TID
/*
 * openflow set l2-entry type smac-dmac <MACADDR:smac> <MACADDR:dmac> ( <UINT:metadata> | none ) instruction goto-table normal <UINT:tid>
 */
cparser_result_t
cparser_cmd_openflow_set_l2_entry_type_smac_dmac_smac_dmac_metadata_none_instruction_goto_table_normal_tid(
    cparser_context_t *context,
    cparser_macaddr_t *smac_ptr,
    cparser_macaddr_t *dmac_ptr,
    uint32_t *metadata_ptr,
    uint32_t *tid_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l2FlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L2_HASH_CMD_PARSE(NULL, smac_ptr, dmac_ptr, 7);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l2FlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);
    entry.ins.gt_data.type = OF_GOTOTBL_TYPE_NORMAL;
    entry.ins.gt_data.tbl_id = *tid_ptr;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_add(unit, &entry), ret);
    DUMP_L2_INSTRUCTION(entry, OF_INS_ACT_TYPE_GOTOTBL);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l2_entry_type_smac_dmac_smac_dmac_metadata_none_instruction_goto_table_normal_tid */
#endif

#ifdef CMD_OPENFLOW_SET_L2_ENTRY_TYPE_5_TUPLE_IPPROTO_5_TUPLE_VID_SIP_DIP_L4_SPORT_L4_DPORT_IPPROTO_VID_INSTRUCTION_GOTO_TABLE_NORMAL_TID
/*
 * openflow set l2-entry type ( 5-tuple-ipproto | 5-tuple-vid ) <IPV4ADDR:sip> <IPV4ADDR:dip> <UINT:l4_sport> <UINT:l4_dport> <UINT:ipproto_vid> instruction goto-table normal <UINT:tid>
 */
cparser_result_t
cparser_cmd_openflow_set_l2_entry_type_5_tuple_ipproto_5_tuple_vid_sip_dip_l4_sport_l4_dport_ipproto_vid_instruction_goto_table_normal_tid(
    cparser_context_t *context,
    uint32_t *sip_ptr,
    uint32_t *dip_ptr,
    uint32_t *l4_sport_ptr,
    uint32_t *l4_dport_ptr,
    uint32_t *ipproto_vid_ptr,
    uint32_t *tid_ptr)
{
    uint32  unit;
    int32   ret;
    rtk_of_l2FlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L2_HASH_5_TUPLE_CMD_PARSE(4, sip_ptr, dip_ptr, l4_sport_ptr, l4_dport_ptr, ipproto_vid_ptr);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l2FlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);
    entry.ins.gt_data.type = OF_GOTOTBL_TYPE_NORMAL;
    entry.ins.gt_data.tbl_id = *tid_ptr;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_add(unit, &entry), ret);
    DUMP_L2_INSTRUCTION(entry, OF_INS_ACT_TYPE_GOTOTBL);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l2_entry_type_5_tuple_ipproto_5_tuple_vid_sip_dip_l4_sport_l4_dport_ipproto_vid_instruction_goto_table_normal_tid */
#endif

#ifdef CMD_OPENFLOW_SET_L2_ENTRY_TYPE_SMAC_DMAC_MAC_METADATA_NONE_INSTRUCTION_GOTO_TABLE_LOOPBACK_APPLY_THEN_LOOPBACK_TID
/*
 * openflow set l2-entry type ( smac | dmac ) <MACADDR:mac> ( <UINT:metadata> | none ) instruction goto-table ( loopback | apply-then-loopback ) <UINT:tid>
 */
cparser_result_t
cparser_cmd_openflow_set_l2_entry_type_smac_dmac_mac_metadata_none_instruction_goto_table_loopback_apply_then_loopback_tid(
    cparser_context_t *context,
    cparser_macaddr_t *mac_ptr,
    uint32_t *metadata_ptr,
    uint32_t *tid_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l2FlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L2_HASH_CMD_PARSE(NULL, mac_ptr, mac_ptr, 6);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l2FlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);
    if ('l' == TOKEN_CHAR(9,0))
        entry.ins.gt_data.type = OF_GOTOTBL_TYPE_LB;
    else if ('a' == TOKEN_CHAR(9,0))
        entry.ins.gt_data.type = OF_GOTOTBL_TYPE_APPLY_AND_LB;
    entry.ins.gt_data.tbl_id = *tid_ptr;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_add(unit, &entry), ret);
    DUMP_L2_INSTRUCTION(entry, OF_INS_ACT_TYPE_GOTOTBL);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l2_entry_type_smac_dmac_mac_metadata_none_instruction_goto_table_loopback_apply_then_loopback_tid */
#endif

#ifdef CMD_OPENFLOW_SET_L2_ENTRY_TYPE_VID_SMAC_VID_DMAC_VID_MAC_METADATA_NONE_INSTRUCTION_GOTO_TABLE_LOOPBACK_APPLY_THEN_LOOPBACK_TID
/*
 * openflow set l2-entry type ( vid-smac | vid-dmac ) <UINT:vid> ( <UINT:metadata> | none ) instruction goto-table ( loopback | apply-then-loopback ) <UINT:tid>
 */
cparser_result_t
cparser_cmd_openflow_set_l2_entry_type_vid_smac_vid_dmac_vid_mac_metadata_none_instruction_goto_table_loopback_apply_then_loopback_tid(
    cparser_context_t *context,
    uint32_t *vid_ptr,
    cparser_macaddr_t *mac_ptr,
    uint32_t *metadata_ptr,
    uint32_t *tid_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l2FlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    entry.matchfield.vidMac.vid = *vid_ptr;
    L2_HASH_CMD_PARSE(vid_ptr, mac_ptr, mac_ptr, 7);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l2FlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);
    if ('l' == TOKEN_CHAR(10,0))
        entry.ins.gt_data.type = OF_GOTOTBL_TYPE_LB;
    else if ('a' == TOKEN_CHAR(10,0))
        entry.ins.gt_data.type = OF_GOTOTBL_TYPE_APPLY_AND_LB;
    entry.ins.gt_data.tbl_id = *tid_ptr;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_add(unit, &entry), ret);
    DUMP_L2_INSTRUCTION(entry, OF_INS_ACT_TYPE_GOTOTBL);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l2_entry_type_vid_smac_vid_dmac_vid_metadata_none_instruction_goto_table_loopback_apply_then_loopback_tid */
#endif

#ifdef CMD_OPENFLOW_SET_L2_ENTRY_TYPE_SMAC_DMAC_SMAC_DMAC_METADATA_NONE_INSTRUCTION_GOTO_TABLE_LOOPBACK_APPLY_THEN_LOOPBACK_TID
/*
 * openflow set l2-entry type smac-dmac <MACADDR:smac> <MACADDR:dmac> ( <UINT:metadata> | none ) instruction goto-table ( loopback | apply-then-loopback ) <UINT:tid>
 */
cparser_result_t
cparser_cmd_openflow_set_l2_entry_type_smac_dmac_smac_dmac_metadata_none_instruction_goto_table_loopback_apply_then_loopback_tid(
    cparser_context_t *context,
    cparser_macaddr_t *smac_ptr,
    cparser_macaddr_t *dmac_ptr,
    uint32_t *metadata_ptr,
    uint32_t *tid_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l2FlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L2_HASH_CMD_PARSE(NULL, smac_ptr, dmac_ptr, 7);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l2FlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);
    if ('l' == TOKEN_CHAR(10,0))
        entry.ins.gt_data.type = OF_GOTOTBL_TYPE_LB;
    else if ('a' == TOKEN_CHAR(10,0))
        entry.ins.gt_data.type = OF_GOTOTBL_TYPE_APPLY_AND_LB;
    entry.ins.gt_data.tbl_id = *tid_ptr;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_add(unit, &entry), ret);
    DUMP_L2_INSTRUCTION(entry, OF_INS_ACT_TYPE_GOTOTBL);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l2_entry_type_smac_dmac_smac_dmac_metadata_none_instruction_goto_table_loopback_apply_then_loopback_tid */
#endif

#ifdef CMD_OPENFLOW_SET_L2_ENTRY_TYPE_5_TUPLE_IPPROTO_5_TUPLE_VID_SIP_DIP_L4_SPORT_L4_DPORT_IPPROTO_VID_INSTRUCTION_GOTO_TABLE_LOOPBACK_APPLY_THEN_LOOPBACK_TID
/*
 * openflow set l2-entry type ( 5-tuple-ipproto | 5-tuple-vid ) <IPV4ADDR:sip> <IPV4ADDR:dip> <UINT:l4_sport> <UINT:l4_dport> <UINT:ipproto_vid> instruction goto-table ( loopback | apply-then-loopback ) <UINT:tid>
 */
cparser_result_t
cparser_cmd_openflow_set_l2_entry_type_5_tuple_ipproto_5_tuple_vid_sip_dip_l4_sport_l4_dport_ipproto_vid_instruction_goto_table_loopback_apply_then_loopback_tid(
    cparser_context_t *context,
    uint32_t *sip_ptr,
    uint32_t *dip_ptr,
    uint32_t *l4_sport_ptr,
    uint32_t *l4_dport_ptr,
    uint32_t *ipproto_vid_ptr,
    uint32_t *tid_ptr)
{
    uint32  unit;
    int32   ret;
    rtk_of_l2FlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L2_HASH_5_TUPLE_CMD_PARSE(4, sip_ptr, dip_ptr, l4_sport_ptr, l4_dport_ptr, ipproto_vid_ptr);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l2FlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);
    if ('l' == TOKEN_CHAR(12,0))
        entry.ins.gt_data.type = OF_GOTOTBL_TYPE_LB;
    else if ('a' == TOKEN_CHAR(12,0))
        entry.ins.gt_data.type = OF_GOTOTBL_TYPE_APPLY_AND_LB;
    entry.ins.gt_data.tbl_id = *tid_ptr;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_add(unit, &entry), ret);
    DUMP_L2_INSTRUCTION(entry, OF_INS_ACT_TYPE_GOTOTBL);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l2_entry_type_5_tuple_ipproto_5_tuple_vid_sip_dip_l4_sport_l4_dport_ipproto_vid_instruction_goto_table_loopback_apply_then_loopback_tid */
#endif

#ifdef CMD_OPENFLOW_DEL_L2_ENTRY_TYPE_SMAC_DMAC_MAC_METADATA_NONE
/*
 * openflow del l2-entry type ( smac | dmac ) <MACADDR:mac> ( <UINT:metadata> | none )
 */
cparser_result_t
cparser_cmd_openflow_del_l2_entry_type_smac_dmac_mac_metadata_none(
    cparser_context_t *context,
    cparser_macaddr_t *mac_ptr,
    uint32_t *metadata_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l2FlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L2_HASH_CMD_PARSE(NULL, mac_ptr, mac_ptr, 6);
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_del(unit, &entry), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_del_l2_entry_type_smac_dmac_mac_metadata_none */
#endif

#ifdef CMD_OPENFLOW_DEL_L2_ENTRY_TYPE_VID_SMAC_VID_DMAC_VID_MAC_METADATA_NONE
/*
 * openflow del l2-entry type ( vid-smac | vid-dmac ) <UINT:vid> ( <UINT:metadata> | none )
 */
cparser_result_t
cparser_cmd_openflow_del_l2_entry_type_vid_smac_vid_dmac_vid_mac_metadata_none(
    cparser_context_t *context,
    uint32_t *vid_ptr,
    cparser_macaddr_t *mac_ptr,
    uint32_t *metadata_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l2FlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    entry.matchfield.vidMac.vid = *vid_ptr;
    L2_HASH_CMD_PARSE(vid_ptr, mac_ptr, mac_ptr, 7);
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_del(unit, &entry), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_del_l2_entry_type_vid_smac_vid_dmac_vid_metadata_none */
#endif

#ifdef CMD_OPENFLOW_DEL_L2_ENTRY_TYPE_SMAC_DMAC_SMAC_DMAC_METADATA_NONE
/*
 * openflow del l2-entry type smac-dmac <MACADDR:smac> <MACADDR:dmac> ( <UINT:metadata> | none )
 */
cparser_result_t
cparser_cmd_openflow_del_l2_entry_type_smac_dmac_smac_dmac_metadata_none(
    cparser_context_t *context,
    cparser_macaddr_t *smac_ptr,
    cparser_macaddr_t *dmac_ptr,
    uint32_t *metadata_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l2FlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L2_HASH_CMD_PARSE(NULL, smac_ptr, dmac_ptr, 7);
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_del(unit, &entry), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_del_l2_entry_type_smac_dmac_smac_dmac_metadata_none */
#endif

#ifdef CMD_OPENFLOW_DEL_L2_ENTRY_TYPE_5_TUPLE_IPPROTO_5_TUPLE_VID_SIP_DIP_L4_SPORT_L4_DPORT_IPPROTO_VID
/*
 * openflow del l2-entry type ( 5-tuple-ipproto | 5-tuple-vid ) <IPV4ADDR:sip> <IPV4ADDR:dip> <UINT:l4_sport> <UINT:l4_dport> <UINT:ipproto_vid>
 */
cparser_result_t
cparser_cmd_openflow_del_l2_entry_type_5_tuple_ipproto_5_tuple_vid_sip_dip_l4_sport_l4_dport_ipproto_vid(
    cparser_context_t *context,
    uint32_t *sip_ptr,
    uint32_t *dip_ptr,
    uint32_t *l4_sport_ptr,
    uint32_t *l4_dport_ptr,
    uint32_t *ipproto_vid_ptr)
{
    uint32  unit;
    int32   ret;
    rtk_of_l2FlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L2_HASH_5_TUPLE_CMD_PARSE(4, sip_ptr, dip_ptr, l4_sport_ptr, l4_dport_ptr, ipproto_vid_ptr);
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_del(unit, &entry), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_del_l2_entry_type_5_tuple_ipproto_5_tuple_vid_sip_dip_l4_sport_l4_dport_ipproto_vid */
#endif

#ifdef CMD_OPENFLOW_DEL_L2_ENTRY_ALL
/*
 * openflow del l2-entry all
 */
cparser_result_t
cparser_cmd_openflow_del_l2_entry_all(
    cparser_context_t *context)
{
    uint32 unit;
    int32 ret;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowEntry_delAll(unit), ret);
    DIAG_UTIL_MPRINTF("Delete done\n");

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_del_l2_entry_all */
#endif

#ifdef CMD_OPENFLOW_GET_FLOWTABLE_L2_HASH_ALGORITHM_BLOCK_ID
/*
 * openflow get flowtable l2 hash-algorithm <UINT:block_id>
 */
cparser_result_t
cparser_cmd_openflow_get_flowtable_l2_hash_algorithm_block_id(
    cparser_context_t *context,
    uint32_t *block_id_ptr)
{
    uint32 unit;
    uint32 algo = 0;
    int32 ret;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowTblHashAlgo_get(unit, *block_id_ptr, &algo), ret);
    DIAG_UTIL_MPRINTF("Flow table block %u l2 hash-algorithm is %u\n", *block_id_ptr, algo);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_get_flowtable_l2_hash_algorithm_block_id */
#endif

#ifdef CMD_OPENFLOW_SET_FLOWTABLE_L2_HASH_ALGORITHM_BLOCK_ID_ALGO_ID
/*
 * openflow set flowtable l2 hash-algorithm <UINT:block_id> <UINT:algo_id>
 */
cparser_result_t
cparser_cmd_openflow_set_flowtable_l2_hash_algorithm_block_id_algo_id(
    cparser_context_t *context,
    uint32_t *block_id_ptr,
    uint32_t *algo_id_ptr)
{
    uint32 unit;
    int32 ret;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(rtk_of_l2FlowTblHashAlgo_set(unit, *block_id_ptr, *algo_id_ptr), ret);
    DIAG_UTIL_MPRINTF("Flow table block %u l2 hash-algorithm set to %u\n", *block_id_ptr, *algo_id_ptr);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_flowtable_l2_hash_algorithm_block_id_algo_id */
#endif

#ifdef CMD_OPENFLOW_GET_FLOWTABLE_L2_TABLE_MISS
/*
 * openflow get flowtable l2 table-miss
 */
cparser_result_t
cparser_cmd_openflow_get_flowtable_l2_table_miss(
    cparser_context_t *context)
{
    uint32 unit;
    int32 ret;
    rtk_of_tblMissAct_t act = OF_TBLMISS_ACT_END;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(rtk_of_tblMissAction_get(unit, FT_PHASE_IGR_FT_1, &act), ret);
    DIAG_UTIL_MPRINTF("L2 flow table action is %s\n", text_table_miss_act[act]);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_get_flowtable_l2_table_miss */
#endif

#ifdef CMD_OPENFLOW_SET_FLOWTABLE_L2_TABLE_MISS_DROP_TRAP_NEXT_TABLE_EXECUTE
/*
 * openflow set flowtable l2 table-miss ( drop | trap | next-table | execute )
 */
cparser_result_t
cparser_cmd_openflow_set_flowtable_l2_table_miss_drop_trap_next_table_execute(
    cparser_context_t *context)
{
    uint32 unit;
    int32 ret;
    rtk_of_tblMissAct_t act = OF_TBLMISS_ACT_END;

    DIAG_UTIL_FUNC_INIT(unit);

    if ('d' == TOKEN_CHAR(5, 0))
        act = OF_TBLMISS_ACT_DROP;
    else if ('t' == TOKEN_CHAR(5, 0))
        act = OF_TBLMISS_ACT_TRAP;
    else if ('n' == TOKEN_CHAR(5, 0))
        act = OF_TBLMISS_ACT_FORWARD_NEXT_TBL;
    else if ('e' == TOKEN_CHAR(5, 0))
        act = OF_TBLMISS_ACT_EXEC_ACTION_SET;

    DIAG_UTIL_ERR_CHK(rtk_of_tblMissAction_set(unit, FT_PHASE_IGR_FT_1, act), ret);
    DIAG_UTIL_MPRINTF("L2 flow table miss action set to %s\n", text_table_miss_act[act]);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_get_flowtable_l2_table_miss_drop_trap_next_table_execute */
#endif

#ifdef CMD_OPENFLOW_GET_FLOWTABLE_L2_COUNTER
/*
 * openflow get flowtable l2 counter ( loopkup | match )
 */
cparser_result_t
cparser_cmd_openflow_get_flowtable_l2_counter_loopkup_match(
    cparser_context_t *context)
{
    uint32 unit;
    uint32 cnt = 0;
    int32 ret;
    rtk_of_flowTblCntType_t type = OF_FLOW_TBL_CNT_TYPE_END;

    DIAG_UTIL_FUNC_INIT(unit);

    if ('l' == TOKEN_CHAR(5, 0))
        type = OF_FLOW_TBL_CNT_TYPE_LOOKUP;
    else if ('m' == TOKEN_CHAR(5, 0))
        type = OF_FLOW_TBL_CNT_TYPE_MATCH;

    DIAG_UTIL_ERR_CHK(rtk_of_flowTblCnt_get(unit, FT_PHASE_IGR_FT_1, type, &cnt), ret);
    DIAG_UTIL_MPRINTF("L2 flow table %s counter: %d\n", TOKEN_STR(5), cnt);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_get_flowtable_l2_counter_loopkup_match */
#endif

#ifdef CMD_OPENFLOW_GET_FLOWTABLE_L3_TABLE_MISS
/*
 * openflow get flowtable l3 table-miss
 */
cparser_result_t
cparser_cmd_openflow_get_flowtable_l3_table_miss(
    cparser_context_t *context)
{
    uint32 unit;
    int32 ret;
    rtk_of_tblMissAct_t act = OF_TBLMISS_ACT_END;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(rtk_of_tblMissAction_get(unit, FT_PHASE_IGR_FT_2, &act), ret);
    DIAG_UTIL_MPRINTF("L3 flow table action is %s\n", text_table_miss_act[act]);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_get_flowtable_l3_table_miss */
#endif

#ifdef CMD_OPENFLOW_SET_FLOWTABLE_L3_TABLE_MISS_DROP_TRAP_NEXT_TABLE_EXECUTE
/*
 * openflow set flowtable l3 table-miss ( drop | trap | next-table | execute )
 */
cparser_result_t
cparser_cmd_openflow_set_flowtable_l3_table_miss_drop_trap_next_table_execute(
    cparser_context_t *context)
{
    uint32 unit;
    int32 ret;
    rtk_of_tblMissAct_t act = OF_TBLMISS_ACT_END;

    DIAG_UTIL_FUNC_INIT(unit);

    if ('d' == TOKEN_CHAR(5, 0))
        act = OF_TBLMISS_ACT_DROP;
    else if ('t' == TOKEN_CHAR(5, 0))
        act = OF_TBLMISS_ACT_TRAP;
    else if ('n' == TOKEN_CHAR(5, 0))
        act = OF_TBLMISS_ACT_FORWARD_NEXT_TBL;
    else if ('e' == TOKEN_CHAR(5, 0))
        act = OF_TBLMISS_ACT_EXEC_ACTION_SET;

    DIAG_UTIL_ERR_CHK(rtk_of_tblMissAction_set(unit, FT_PHASE_IGR_FT_2, act), ret);
    DIAG_UTIL_MPRINTF("L3 flow table miss action set to %s\n", text_table_miss_act[act]);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_get_flowtable_l3_table_miss_drop_trap_next_table_execute */
#endif

#ifdef CMD_OPENFLOW_GET_FLOWTABLE_L3_COUNTER
/*
 * openflow get flowtable l3 counter ( loopkup | match )
 */
cparser_result_t
cparser_cmd_openflow_get_flowtable_l3_counter_loopkup_match(
    cparser_context_t *context)
{
    uint32 unit;
    uint32 cnt = 0;
    int32 ret;
    rtk_of_flowTblCntType_t type = OF_FLOW_TBL_CNT_TYPE_END;

    DIAG_UTIL_FUNC_INIT(unit);

    if ('l' == TOKEN_CHAR(5, 0))
        type = OF_FLOW_TBL_CNT_TYPE_LOOKUP;
    else if ('m' == TOKEN_CHAR(5, 0))
        type = OF_FLOW_TBL_CNT_TYPE_MATCH;

    DIAG_UTIL_ERR_CHK(rtk_of_flowTblCnt_get(unit, FT_PHASE_IGR_FT_2, type, &cnt), ret);
    DIAG_UTIL_MPRINTF("L3 flow table %s counter: %d\n", TOKEN_STR(5), cnt);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_get_flowtable_l3_counter_loopkup_match */
#endif

#ifdef CMD_OPENFLOW_GET_FLOWTABLE_L3_PRECEDENCE
/*
 * openflow get flowtable l3 precedence
 */
cparser_result_t
cparser_cmd_openflow_get_flowtable_l3_precedence(
    cparser_context_t *context)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3FlowTblList_t list = OF_L3_FLOW_TBL_LIST_END;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(rtk_of_l3FlowTblPri_get(unit, &list), ret);
    DIAG_UTIL_MPRINTF("Flow table l3 precedence is %s\n", text_l3_flow_tbl_list[list]);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_get_flowtable_l3_precedence */
#endif

#ifdef CMD_OPENFLOW_SET_FLOWTABLE_L3_PRECEDENCE_CAM_HASH
/*
 * openflow set flowtable l3 precedence ( cam | hash )
 */
cparser_result_t
cparser_cmd_openflow_set_flowtable_l3_precedence_cam_hash(
    cparser_context_t *context)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3FlowTblList_t list = OF_L3_FLOW_TBL_LIST_END;

    DIAG_UTIL_FUNC_INIT(unit);

    if ('c' == TOKEN_CHAR(5, 0))
        list = OF_L3_FLOW_TBL_LIST_CAM;
    else if ('h' == TOKEN_CHAR(5, 0))
        list = OF_L3_FLOW_TBL_LIST_HASH;

    DIAG_UTIL_ERR_CHK(rtk_of_l3FlowTblPri_set(unit, list), ret);
    DIAG_UTIL_MPRINTF("Flow table l3 precedence set to %s\n", text_l3_flow_tbl_list[list]);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_flowtable_l3_precedence_cam_hash */
#endif

#ifdef CMD_OPENFLOW_GET_FLOWTABLE_L3_CAM_MATCH_FIELD
/*
 * openflow get flowtable l3-cam match-field
 */
cparser_result_t
cparser_cmd_openflow_get_flowtable_l3_cam_match_field(
    cparser_context_t *context)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3CamFlowTblMatchField_t field = OF_L3_CAM_FT_MATCH_FIELD_END;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(rtk_of_l3CamFlowTblMatchField_get(unit, &field), ret);
    DIAG_UTIL_MPRINTF("Flow table l3-cam match-field is %s\n", text_l3_cam_match_field[field]);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_get_flowtable_l3_cam_match_field */
#endif

#ifdef CMD_OPENFLOW_SET_FLOWTABLE_L3_CAM_MATCH_FIELD_SIP_MD_DIP_MD_SIP_DIP_MD_SIP_MD_SIP_DIP_MD_DIP_MD
/*
 * openflow set flowtable l3-cam match-field ( sip-md-dip-md | sip-dip-md-sip-md | sip-dip-md-dip-md )
 */
cparser_result_t
cparser_cmd_openflow_set_flowtable_l3_cam_match_field_sip_md_dip_md_sip_dip_md_sip_md_sip_dip_md_dip_md(
    cparser_context_t *context)
{
    uint32 unit;
    int32 ret;
     rtk_of_l3CamFlowTblMatchField_t field = OF_L3_CAM_FT_MATCH_FIELD_END;

    DIAG_UTIL_FUNC_INIT(unit);

    if ('m' == TOKEN_CHAR(5, 11))
        field = OF_L3_CAM_FT_MATCH_FIELD_MD_SIP_MD_DIP;
    else if ('s' == TOKEN_CHAR(5, 11))
        field = OF_L3_CAM_FT_MATCH_FIELD_MD_SIP_DIP_MD_SIP;
    else if ('d' == TOKEN_CHAR(5, 11))
        field = OF_L3_CAM_FT_MATCH_FIELD_MD_SIP_DIP_MD_DIP;

    DIAG_UTIL_ERR_CHK(rtk_of_l3CamFlowTblMatchField_set(unit, field), ret);
    DIAG_UTIL_MPRINTF("Flow table l3-cam match-field set to %s\n", text_l3_cam_match_field[field]);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_flowtable_l3_cam_match_field_sip_md_dip_md_sip_dip_md_sip_md_sip_dip_md_dip_md */
#endif

#ifdef CMD_OPENFLOW_GET_FLOWTABLE_L3_HASH_MATCH_FIELD
/*
 * openflow get flowtable l3-hash match-field
 */
cparser_result_t
cparser_cmd_openflow_get_flowtable_l3_hash_match_field(
    cparser_context_t *context)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowTblMatchField_t field = OF_L3_HASH_FT_MATCH_FIELD_END;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowTblMatchField_get(unit, &field), ret);
    DIAG_UTIL_MPRINTF("Flow table l3-hash match-field: %s\n", text_l3_hash_match_field[field]);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_get_flowtable_l3_hash_match_field */
#endif

#ifdef CMD_OPENFLOW_SET_FLOWTABLE_L3_HASH_MATCH_FIELD_SIP_DIP_SIP_DIP_SIP_SIP_DIP_DIP
/*
 * openflow set flowtable l3-hash match-field ( sip-dip | sip-dip-sip | sip-dip-dip )
 */
cparser_result_t
cparser_cmd_openflow_set_flowtable_l3_hash_match_field_sip_dip_sip_dip_sip_sip_dip_dip(
    cparser_context_t *context)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowTblMatchField_t field = OF_L3_HASH_FT_MATCH_FIELD_END;

    DIAG_UTIL_FUNC_INIT(unit);

    if (osal_strlen(TOKEN_STR(5)) <= 7)
        field = OF_L3_HASH_FT_MATCH_FIELD_SIP_DIP;
    else if ('s' == TOKEN_CHAR(5, 8))
        field = OF_L3_HASH_FT_MATCH_FIELD_SIP_DIP_SIP;
    else if ('d' == TOKEN_CHAR(5, 8))
        field = OF_L3_HASH_FT_MATCH_FIELD_SIP_DIP_DIP;

    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowTblMatchField_set(unit, field), ret);

    DIAG_UTIL_MPRINTF("Flow table l3-hash match-field set to %s\n", text_l3_hash_match_field[field]);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_flowtable_l3_hash_match_field_sip_dip_sip_dip_sip_sip_dip_dip */
#endif

#ifdef CMD_OPENFLOW_GET_FLOWTABLE_L3_HASH_HASH_ALGORITHM_BLOCK_ID
/*
 * openflow get flowtable l3-hash hash-algorithm <UINT:block_id>
 */
cparser_result_t
cparser_cmd_openflow_get_flowtable_l3_hash_hash_algorithm_block_id(
    cparser_context_t *context,
    uint32_t *block_id_ptr)
{
    uint32 unit;
    uint32 algo = 0;
    int32 ret;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowTblHashAlgo_get(unit, *block_id_ptr, &algo), ret);
    DIAG_UTIL_MPRINTF("Flow table l3-hash algorithm of block %u is algo %u\n", *block_id_ptr, algo);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_get_flowtable_l3_hash_hash_algorithm_block_id */
#endif

#ifdef CMD_OPENFLOW_SET_FLOWTABLE_L3_HASH_HASH_ALGORITHM_BLOCK_ID_ALGO_ID
/*
 * openflow set flowtable l3-hash hash-algorithm <UINT:block_id> <UINT:algo_id>
 */
cparser_result_t
cparser_cmd_openflow_set_flowtable_l3_hash_hash_algorithm_block_id_algo_id(
    cparser_context_t *context,
    uint32_t *block_id_ptr,
    uint32_t *algo_id_ptr)
{
    uint32 unit;
    int32 ret;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowTblHashAlgo_set(unit, *block_id_ptr, *algo_id_ptr), ret);
    DIAG_UTIL_MPRINTF("Flow table l3-hash algorithm of block %u set to index %u\n", *block_id_ptr, *algo_id_ptr);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_flowtable_l3_hash_hash_algorithm_block_id_algo_id */
#endif

#ifdef CMD_OPENFLOW_GET_L3_CAM_ENTRY_INDEX
/*
 * openflow get l3-cam-entry <UINT:index>
 */
cparser_result_t
cparser_cmd_openflow_get_l3_cam_entry_index(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3CamFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3CamFlowEntry_get(unit, *index_ptr, &entry)))? RT_ERR_OK : ret, ret);
    DUMP_L3CAM_FLOWENTRY(*index_ptr, entry, OF_INS_ACT_TYPE_END);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_get_l3_cam_entry_index */
#endif

#ifdef CMD_OPENFLOW_ADD_L3_CAM_ENTRY_INDEX_TYPE_SIP_DIP_IP_MASK_MD_MD_MSK
/*
 * openflow add l3-cam-entry <UINT:index> type ( sip | dip ) <IPV4ADDR:ip> <IPV4ADDR:mask> <UINT:md> <UINT:md_msk>
 */
cparser_result_t
cparser_cmd_openflow_add_l3_cam_entry_index_type_sip_dip_ip_mask_md_md_msk(
    cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *ip_ptr,
    uint32_t *mask_ptr,
    uint32_t *md_ptr,
    uint32_t *md_msk_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3CamFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3CamFlowEntry_get(unit, *index_ptr, &entry)))? RT_ERR_OK : ret, ret);

    if ('s' == TOKEN_CHAR(5, 0))
    {
        entry.type = OF_L3_FLOW_ENTRY_TYPE_IP4_SIP;
        entry.sip.ipv4 = *ip_ptr;
        entry.sip_msk.ipv4 = *mask_ptr;
    }
    else if ('d' == TOKEN_CHAR(5, 0))
    {
        entry.type = OF_L3_FLOW_ENTRY_TYPE_IP4_DIP;
        entry.dip.ipv4 = *ip_ptr;
        entry.dip_msk.ipv4 = *mask_ptr;
    }
    entry.metadata = *md_ptr;
    entry.metadata_msk = *md_msk_ptr;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3CamFlowEntry_add(unit, *index_ptr, &entry), ret);
    DUMP_L3CAM_FLOWENTRY(*index_ptr, entry, OF_INS_ACT_TYPE_END);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_add_l3_cam_entry_index_type_sip_dip_ip_mask_md_md_msk */
#endif

#ifdef CMD_OPENFLOW_ADD_L3_CAM_ENTRY_INDEX_TYPE_SIP_DIP_SIP_SMASK_DIP_DMASK_MD_MD_MSK
/*
 * openflow add l3-cam-entry <UINT:index> type sip-dip <IPV4ADDR:sip> <IPV4ADDR:smask> <IPV4ADDR:dip> <IPV4ADDR:dmask> <UINT:md> <UINT:md_msk>
 */
cparser_result_t
cparser_cmd_openflow_add_l3_cam_entry_index_type_sip_dip_sip_smask_dip_dmask_md_md_msk(
    cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *sip_ptr,
    uint32_t *smask_ptr,
    uint32_t *dip_ptr,
    uint32_t *dmask_ptr,
    uint32_t *md_ptr,
    uint32_t *md_msk_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3CamFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3CamFlowEntry_get(unit, *index_ptr, &entry)))? RT_ERR_OK : ret, ret);
    entry.type = OF_L3_FLOW_ENTRY_TYPE_IP4_SIP_DIP;
    entry.sip.ipv4 = *sip_ptr;
    entry.sip_msk.ipv4 = *smask_ptr;
    entry.dip.ipv4 = *dip_ptr;
    entry.dip_msk.ipv4 = *dmask_ptr;
    entry.metadata = *md_ptr;
    entry.metadata_msk = *md_msk_ptr;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3CamFlowEntry_add(unit, *index_ptr, &entry), ret);
    DUMP_L3CAM_FLOWENTRY(*index_ptr, entry, OF_INS_ACT_TYPE_END);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_add_l3_cam_entry_index_type_sip_dip_sip_smask_dip_dmask_md_md_msk */
#endif

#ifdef CMD_OPENFLOW_ADD_L3_CAM_ENTRY_INDEX_TYPE_SIP6_DIP6_IP_MASK_MD_MD_MSK
/*
 * openflow add l3-cam-entry <UINT:index> type ( sip6 | dip6 ) <IPV6ADDR:ip> <IPV6ADDR:mask> <UINT:md> <UINT:md_msk>
 */
cparser_result_t
cparser_cmd_openflow_add_l3_cam_entry_index_type_sip6_dip6_ip_mask_md_md_msk(
    cparser_context_t *context,
    uint32_t *index_ptr,
    char **ip_ptr,
    char **mask_ptr,
    uint32_t *md_ptr,
    uint32_t *md_msk_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3CamFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3CamFlowEntry_get(unit, *index_ptr, &entry)))? RT_ERR_OK : ret, ret);

    if ('s' == TOKEN_CHAR(5, 0))
    {
        entry.type = OF_L3_FLOW_ENTRY_TYPE_IP6_SIP;
        DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(entry.sip.ipv6.octet, *ip_ptr), ret);
        DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(entry.sip_msk.ipv6.octet, *mask_ptr), ret);
    }
    else if ('d' == TOKEN_CHAR(5, 0))
    {
        entry.type = OF_L3_FLOW_ENTRY_TYPE_IP6_DIP;
        DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(entry.dip.ipv6.octet, *ip_ptr), ret);
        DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(entry.dip_msk.ipv6.octet, *mask_ptr), ret);
    }
    entry.metadata = *md_ptr;
    entry.metadata_msk = *md_msk_ptr;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3CamFlowEntry_add(unit, *index_ptr, &entry), ret);
    DUMP_L3CAM_FLOWENTRY(*index_ptr, entry, OF_INS_ACT_TYPE_END);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_add_l3_cam_entry_index_type_sip6_dip6_ip_mask_md_md_msk */
#endif

#ifdef CMD_OPENFLOW_ADD_L3_CAM_ENTRY_INDEX_TYPE_SIP6_DIP6_SIP_SMASK_DIP_DMASK_MD_MD_MSK
/*
 * openflow add l3-cam-entry <UINT:index> type sip6-dip6 <IPV6ADDR:sip> <IPV6ADDR:smask> <IPV6ADDR:dip> <IPV6ADDR:dmask> <UINT:md> <UINT:md_msk>
 */
cparser_result_t
cparser_cmd_openflow_add_l3_cam_entry_index_type_sip6_dip6_sip_smask_dip_dmask_md_md_msk(
    cparser_context_t *context,
    uint32_t *index_ptr,
    char **sip_ptr,
    char **smask_ptr,
    char **dip_ptr,
    char **dmask_ptr,
    uint32_t *md_ptr,
    uint32_t *md_msk_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3CamFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3CamFlowEntry_get(unit, *index_ptr, &entry)))? RT_ERR_OK : ret, ret);
    entry.type = OF_L3_FLOW_ENTRY_TYPE_IP6_SIP_DIP;
    DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(entry.sip.ipv6.octet, *sip_ptr), ret);
    DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(entry.sip_msk.ipv6.octet, *smask_ptr), ret);
    DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(entry.dip.ipv6.octet, *dip_ptr), ret);
    DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(entry.dip_msk.ipv6.octet, *dmask_ptr), ret);
    entry.metadata = *md_ptr;
    entry.metadata_msk = *md_msk_ptr;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3CamFlowEntry_add(unit, *index_ptr, &entry), ret);
    DUMP_L3CAM_FLOWENTRY(*index_ptr, entry, OF_INS_ACT_TYPE_END);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_add_l3_cam_entry_index_type_sip6_dip6_sip_smask_dip_dmask_md_md_msk */
#endif

#ifdef CMD_OPENFLOW_ADD_L3_CAM_ENTRY_INDEX_INSTRUCTION_CLEAR_ACTION_WRITE_ACTION_WRITE_METADATA_GOTO_TABLE_STATE_DISABLE_ENABLE
/*
 * openflow add l3-cam-entry <UINT:index> instruction ( clear-action | write-action | write-metadata | goto-table ) state ( disable | enable )
 */
cparser_result_t
cparser_cmd_openflow_add_l3_cam_entry_index_instruction_clear_action_write_action_write_metadata_goto_table_state_disable_enable(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3CamFlowEntry_t entry;
    rtk_of_ins_action_type_t type = OF_INS_ACT_TYPE_END;
    rtk_enable_t state = DISABLED;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    DIAG_OF_PHASE_INSTRUCTION(5, type);
    DIAG_UTIL_PARSE_STATE(7, state);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3CamFlowEntry_get(unit, *index_ptr, &entry)))? RT_ERR_OK : ret, ret);

    if (type == OF_INS_ACT_TYPE_CLEARACT)
        entry.ins.clearAct_en = state;
    else if (type == OF_INS_ACT_TYPE_WRITEACT)
        entry.ins.writeAct_en = state;
    else if (type == OF_INS_ACT_TYPE_WRITEMETA)
        entry.ins.writeMetadata_en = state;
    else if (type == OF_INS_ACT_TYPE_GOTOTBL)
        entry.ins.gotoTbl_en = state;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3CamFlowEntry_add(unit, *index_ptr, &entry), ret);
    DUMP_L3CAM_FLOWENTRY(*index_ptr, entry, OF_INS_ACT_TYPE_END);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_add_l3_cam_entry_index_instruction_clear_action_write_action_write_metadata_goto_table_state_disable_enable */
#endif

#ifdef CMD_OPENFLOW_ADD_L3_CAM_ENTRY_INDEX_INSTRUCTION_WRITE_ACTION_PUSH_VLAN_DEC_IP_TTL_SET_QUEUE_GROUP_STATE_DISABLE_ENABLE
/*
 * openflow add l3-cam-entry <UINT:index> instruction write-action ( push-vlan | dec-ip-ttl | set-queue | group ) state ( disable | enable )
 */
cparser_result_t
cparser_cmd_openflow_add_l3_cam_entry_index_instruction_write_action_push_vlan_dec_ip_ttl_set_queue_group_state_disable_enable(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3CamFlowEntry_t entry;
    rtk_enable_t state = DISABLED;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    DIAG_UTIL_PARSE_STATE(8, state);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3CamFlowEntry_get(unit, *index_ptr, &entry)))? RT_ERR_OK : ret, ret);

    if ('p' == TOKEN_CHAR(6, 0))
        entry.ins.wa_data.push_vlan = state;
    else if ('d' == TOKEN_CHAR(6, 0))
        entry.ins.wa_data.dec_ip_ttl = state;
    else if ('s' == TOKEN_CHAR(6, 0))
        entry.ins.wa_data.set_queue = state;
    else if ('g' == TOKEN_CHAR(6, 0))
        entry.ins.wa_data.group = state;

    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3CamFlowEntry_add(unit, *index_ptr, &entry), ret);
    DUMP_L3CAM_FLOWENTRY(*index_ptr, entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_add_l3_cam_entry_index_instruction_write_action_push_vlan_dec_ip_ttl_set_queue_group_state_disable_enable */
#endif

#ifdef CMD_OPENFLOW_ADD_L3_CAM_ENTRY_INDEX_INSTRUCTION_WRITE_ACTION_PUSH_VLAN_ETHERTYPE_TPID_IDX
/*
 * openflow add l3-cam-entry <UINT:index> instruction write-action push-vlan ethertype <UINT:tpid_idx>
 */
cparser_result_t
cparser_cmd_openflow_add_l3_cam_entry_index_instruction_write_action_push_vlan_ethertype_tpid_idx(
    cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *tpid_idx_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3CamFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3CamFlowEntry_get(unit, *index_ptr, &entry)))? RT_ERR_OK : ret, ret);
    entry.ins.wa_data.push_vlan_data.etherType_idx = *tpid_idx_ptr;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3CamFlowEntry_add(unit, *index_ptr, &entry), ret);
    DUMP_L3CAM_FLOWENTRY(*index_ptr, entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_add_l3_cam_entry_index_instruction_write_action_push_vlan_ethertype_tpid_idx */
#endif

#ifdef CMD_OPENFLOW_ADD_L3_CAM_ENTRY_INDEX_INSTRUCTION_WRITE_ACTION_SET_FIELD_FIELD_ID_FIELD_TYPE_NONE
/*
 * openflow add l3-cam-entry <UINT:index> instruction write-action set-field <UINT:field_id> field-type none
 */
cparser_result_t
cparser_cmd_openflow_add_l3_cam_entry_index_instruction_write_action_set_field_field_id_field_type_none(
    cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *field_id_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3CamFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    DIAG_UTIL_ERR_CHK(rtk_of_l3FlowEntrySetField_check(unit, *field_id_ptr, OF_IGR_FT2_ACT_SF_TYPE0_NONE), ret);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3CamFlowEntry_get(unit, *index_ptr, &entry)))? RT_ERR_OK : ret, ret);

    if (0 == *field_id_ptr)
        entry.ins.wa_data.set_field_0_data.field_type = OF_IGR_FT2_ACT_SF_TYPE0_NONE;
    else if (1 == *field_id_ptr)
        entry.ins.wa_data.set_field_1_data.field_type = OF_IGR_FT2_ACT_SF_TYPE1_NONE;
    else if (2 == *field_id_ptr)
        entry.ins.wa_data.set_field_2_data.field_type = OF_IGR_FT2_ACT_SF_TYPE2_NONE;
    else
        DIAG_UTIL_ERR_CHK(RT_ERR_INPUT, ret);

    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3CamFlowEntry_add(unit, *index_ptr, &entry), ret);
    DUMP_L3CAM_FLOWENTRY(*index_ptr, entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_add_l3_cam_entry_index_instruction_write_action_set_field_field_id_field_type_none */
#endif

#ifdef CMD_OPENFLOW_ADD_L3_CAM_ENTRY_INDEX_INSTRUCTION_WRITE_ACTION_SET_FIELD_FIELD_ID_FIELD_TYPE_SMAC_DMAC_MAC
/*
 * openflow add l3-cam-entry <UINT:index> instruction write-action set-field <UINT:field_id> field-type ( smac | dmac ) <MACADDR:mac>
 */
cparser_result_t
cparser_cmd_openflow_add_l3_cam_entry_index_instruction_write_action_set_field_field_id_field_type_smac_dmac_mac(
    cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *field_id_ptr,
    cparser_macaddr_t *mac_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3CamFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3CamFlowEntry_get(unit, *index_ptr, &entry)))? RT_ERR_OK : ret, ret);

    if ('s' == TOKEN_CHAR(9, 0))
    {
        DIAG_UTIL_ERR_CHK(rtk_of_l3FlowEntrySetField_check(unit, *field_id_ptr, OF_IGR_FT2_ACT_SF_TYPE0_SA), ret);
        entry.ins.wa_data.set_field_0_data.field_type = OF_IGR_FT2_ACT_SF_TYPE0_SA;
        osal_memcpy(entry.ins.wa_data.set_field_0_data.mac.octet, mac_ptr, ETHER_ADDR_LEN);
    }
    else if ('d' == TOKEN_CHAR(9, 0))
    {
        DIAG_UTIL_ERR_CHK(rtk_of_l3FlowEntrySetField_check(unit, *field_id_ptr, OF_IGR_FT2_ACT_SF_TYPE1_DA), ret);
        entry.ins.wa_data.set_field_1_data.field_type = OF_IGR_FT2_ACT_SF_TYPE1_DA;
        osal_memcpy(entry.ins.wa_data.set_field_1_data.mac.octet, mac_ptr, ETHER_ADDR_LEN);
    }

    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3CamFlowEntry_add(unit, *index_ptr, &entry), ret);
    DUMP_L3CAM_FLOWENTRY(*index_ptr, entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_add_l3_cam_entry_index_instruction_write_action_set_field_field_id_field_type_smac_dmac_mac */
#endif

#ifdef CMD_OPENFLOW_ADD_L3_CAM_ENTRY_INDEX_INSTRUCTION_WRITE_ACTION_SET_FIELD_FIELD_ID_FIELD_TYPE_VLAN_PRIORITY_VID_DSCP_VALUE
/*
 * openflow add l3-cam-entry <UINT:index> instruction write-action set-field <UINT:field_id> field-type ( vlan-priority | vid | dscp ) <UINT:value>
 */
cparser_result_t
cparser_cmd_openflow_add_l3_cam_entry_index_instruction_write_action_set_field_field_id_field_type_vlan_priority_vid_dscp_value(
    cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *field_id_ptr,
    uint32_t *value_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3CamFlowEntry_t entry;
    rtk_of_setFieldType_t type = OF_SET_FIELD_TYPE_NONE;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3CamFlowEntry_get(unit, *index_ptr, &entry)))? RT_ERR_OK : ret, ret);

    if ('l' == TOKEN_CHAR(9, 1))
        type = OF_SET_FIELD_TYPE_VLAN_PRI;
    else if ('i' == TOKEN_CHAR(9, 1))
        type = OF_SET_FIELD_TYPE_VLAN_ID;
    else if ('s' == TOKEN_CHAR(9, 1))
        type = OF_SET_FIELD_TYPE_IP_DSCP;

    DIAG_UTIL_ERR_CHK(rtk_of_l3FlowEntrySetField_check(unit, *field_id_ptr, type), ret);

    if (0 == *field_id_ptr)
    {
        entry.ins.wa_data.set_field_0_data.field_type = type;
        entry.ins.wa_data.set_field_0_data.field_data = *value_ptr;
    }
    else if (1 == *field_id_ptr)
    {
        entry.ins.wa_data.set_field_1_data.field_type = type;
        entry.ins.wa_data.set_field_1_data.field_data = *value_ptr;
    }
    else if (2 == *field_id_ptr)
    {
        entry.ins.wa_data.set_field_2_data.field_type = type;
        entry.ins.wa_data.set_field_2_data.field_data = *value_ptr;
    }

    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3CamFlowEntry_add(unit, *index_ptr, &entry), ret);
    DUMP_L3CAM_FLOWENTRY(*index_ptr, entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_add_l3_cam_entry_index_instruction_write_action_set_field_field_id_field_type_vlan_priority_vid_dscp_value */
#endif

#ifdef CMD_OPENFLOW_ADD_L3_CAM_ENTRY_INDEX_INSTRUCTION_WRITE_ACTION_SET_QUEUE_QID
/*
 * openflow add l3-cam-entry <UINT:index> instruction write-action set-queue <UINT:qid>
 */
cparser_result_t
cparser_cmd_openflow_add_l3_cam_entry_index_instruction_write_action_set_queue_qid(
    cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *qid_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3CamFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3CamFlowEntry_get(unit, *index_ptr, &entry)))? RT_ERR_OK : ret, ret);
    entry.ins.wa_data.qid = *qid_ptr;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3CamFlowEntry_add(unit, *index_ptr, &entry), ret);
    DUMP_L3CAM_FLOWENTRY(*index_ptr, entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_add_l3_cam_entry_index_instruction_write_action_set_queue_qid */
#endif

#ifdef CMD_OPENFLOW_ADD_L3_CAM_ENTRY_INDEX_INSTRUCTION_WRITE_ACTION_GROUP_GID
/*
 * openflow add l3-cam-entry <UINT:index> instruction write-action group <UINT:gid>
 */
cparser_result_t
cparser_cmd_openflow_add_l3_cam_entry_index_instruction_write_action_group_gid(
    cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *gid_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3CamFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3CamFlowEntry_get(unit, *index_ptr, &entry)))? RT_ERR_OK : ret, ret);
    entry.ins.wa_data.gid = *gid_ptr;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3CamFlowEntry_add(unit, *index_ptr, &entry), ret);
    DUMP_L3CAM_FLOWENTRY(*index_ptr, entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_add_l3_cam_entry_index_instruction_write_action_group_gid */
#endif

#ifdef CMD_OPENFLOW_ADD_L3_CAM_ENTRY_INDEX_INSTRUCTION_WRITE_ACTION_OUTPUT_NONE
/*
 * openflow add l3-cam-entry <UINT:index> instruction write-action output none
 */
cparser_result_t
cparser_cmd_openflow_add_l3_cam_entry_index_instruction_write_action_output_none(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3CamFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3CamFlowEntry_get(unit, *index_ptr, &entry)))? RT_ERR_OK : ret, ret);
    entry.ins.wa_data.output_data.type = OF_OUTPUT_TYPE_NONE;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3CamFlowEntry_add(unit, *index_ptr, &entry), ret);
    DUMP_L3CAM_FLOWENTRY(*index_ptr, entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_add_l3_cam_entry_index_instruction_write_action_output_none */
#endif

#ifdef CMD_OPENFLOW_ADD_L3_CAM_ENTRY_INDEX_INSTRUCTION_WRITE_ACTION_OUTPUT_PHY_PORT_PHY_PORT_SRC_PORT_FILTER_PORT_UID
/*
 * openflow add l3-cam-entry <UINT:index> instruction write-action output ( phy-port | phy-port-src-port-filter ) <UINT:port> { <UINT:devid> }
 */
cparser_result_t
cparser_cmd_openflow_add_l3_cam_entry_index_instruction_write_action_output_phy_port_phy_port_src_port_filter_port_devid(
    cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *port_ptr,
    uint32_t *devid_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3CamFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3CamFlowEntry_get(unit, *index_ptr, &entry)))? RT_ERR_OK : ret, ret);
    if (osal_strlen(TOKEN_STR(7)) <= 8)
        entry.ins.wa_data.output_data.type = OF_OUTPUT_TYPE_PHY_PORT;
    else
        entry.ins.wa_data.output_data.type = OF_OUTPUT_TYPE_PHY_PORT_EX_SRC_PORT;

    entry.ins.wa_data.output_data.port = *port_ptr;
    if (devid_ptr)
        entry.ins.wa_data.output_data.devID = *devid_ptr;

    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3CamFlowEntry_add(unit, *index_ptr, &entry), ret);
    DUMP_L3CAM_FLOWENTRY(*index_ptr, entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_add_l3_cam_entry_index_instruction_write_action_output_phy_port_phy_port_src_port_filter_port_devid */
#endif

#ifdef CMD_OPENFLOW_ADD_L3_CAM_ENTRY_INDEX_INSTRUCTION_WRITE_ACTION_OUTPUT_TRUNK_TRUNK_SRC_PORT_FILTER_TID
/*
 * openflow add l3-cam-entry <UINT:index> instruction write-action output ( trunk | trunk-src-port-filter ) <UINT:tid>
 */
cparser_result_t
cparser_cmd_openflow_add_l3_cam_entry_index_instruction_write_action_output_trunk_trunk_src_port_filter_tid(
    cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *tid_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3CamFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3CamFlowEntry_get(unit, *index_ptr, &entry)))? RT_ERR_OK : ret, ret);
    if (osal_strlen(TOKEN_STR(7)) <= 5)
        entry.ins.wa_data.output_data.type = OF_OUTPUT_TYPE_TRK_PORT;
    else
        entry.ins.wa_data.output_data.type = OF_OUTPUT_TYPE_TRK_PORT_EX_SRC_PORT;

    entry.ins.wa_data.output_data.trunk = *tid_ptr;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3CamFlowEntry_add(unit, *index_ptr, &entry), ret);
    DUMP_L3CAM_FLOWENTRY(*index_ptr, entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_add_l3_cam_entry_index_instruction_write_action_output_trunk_trunk_src_port_filter_tid */
#endif

#ifdef CMD_OPENFLOW_ADD_L3_CAM_ENTRY_INDEX_INSTRUCTION_WRITE_ACTION_OUTPUT_MULTI_EGRESS_PORT_PORT_ALL
/*
 * openflow add l3-cam-entry <UINT:index> instruction write-action output multi-egress-port ( <PORT_LIST:port> | all )
 */
cparser_result_t
cparser_cmd_openflow_add_l3_cam_entry_index_instruction_write_action_output_multi_egress_port_port_all(
    cparser_context_t *context,
    uint32_t *index_ptr,
    char **port_ptr)
{
    uint32 unit;
    int32 ret;
    diag_portlist_t portlist;
    rtk_of_l3CamFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    osal_memset(&portlist, 0, sizeof(portlist));
    DIAG_UTIL_EXTRACT_PORTLIST(portlist, 8);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3CamFlowEntry_get(unit, *index_ptr, &entry)))? RT_ERR_OK : ret, ret);
    entry.ins.wa_data.output_data.type = OF_OUTPUT_TYPE_MULTI_EGR_PORT;
    entry.ins.wa_data.output_data.portmask = portlist.portmask;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3CamFlowEntry_add(unit, *index_ptr, &entry), ret);
    DUMP_L3CAM_FLOWENTRY(*index_ptr, entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_add_l3_cam_entry_index_instruction_write_action_output_multi_egress_port_port_all */
#endif

#ifdef CMD_OPENFLOW_ADD_L3_CAM_ENTRY_INDEX_INSTRUCTION_WRITE_ACTION_OUTPUT_INGRESS_PORT
/*
 * openflow add l3-cam-entry <UINT:index> instruction write-action output ingress-port
 */
cparser_result_t
cparser_cmd_openflow_add_l3_cam_entry_index_instruction_write_action_output_ingress_port(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3CamFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3CamFlowEntry_get(unit, *index_ptr, &entry)))? RT_ERR_OK : ret, ret);
    entry.ins.wa_data.output_data.type = OF_OUTPUT_TYPE_IN_PORT;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3CamFlowEntry_add(unit, *index_ptr, &entry), ret);
    DUMP_L3CAM_FLOWENTRY(*index_ptr, entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_add_l3_cam_entry_index_instruction_write_action_output_ingress_port */
#endif

#ifdef CMD_OPENFLOW_ADD_L3_CAM_ENTRY_INDEX_INSTRUCTION_WRITE_ACTION_OUTPUT_FLOOD
/*
 * openflow add l3-cam-entry <UINT:index> instruction write-action output flood
 */
cparser_result_t
cparser_cmd_openflow_add_l3_cam_entry_index_instruction_write_action_output_flood(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3CamFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3CamFlowEntry_get(unit, *index_ptr, &entry)))? RT_ERR_OK : ret, ret);
    entry.ins.wa_data.output_data.type = OF_OUTPUT_TYPE_FLOOD;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3CamFlowEntry_add(unit, *index_ptr, &entry), ret);
    DUMP_L3CAM_FLOWENTRY(*index_ptr, entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;;
}   /* end of cparser_cmd_openflow_add_l3_cam_entry_index_instruction_write_action_output_flood */
#endif

#ifdef CMD_OPENFLOW_ADD_L3_CAM_ENTRY_INDEX_INSTRUCTION_WRITE_ACTION_OUTPUT_NORMAL
/*
 * openflow add l3-cam-entry <UINT:index> instruction write-action output normal
 */
cparser_result_t
cparser_cmd_openflow_add_l3_cam_entry_index_instruction_write_action_output_normal(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3CamFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3CamFlowEntry_get(unit, *index_ptr, &entry)))? RT_ERR_OK : ret, ret);
    entry.ins.wa_data.output_data.type = OF_OUTPUT_TYPE_LB;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3CamFlowEntry_add(unit, *index_ptr, &entry), ret);
    DUMP_L3CAM_FLOWENTRY(*index_ptr, entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_add_l3_cam_entry_index_instruction_write_action_output_normal */
#endif

#ifdef CMD_OPENFLOW_ADD_L3_CAM_ENTRY_INDEX_INSTRUCTION_WRITE_ACTION_OUTPUT_FAILOVER_PORT_ALL
/*
 * openflow add l3-cam-entry <UINT:index> instruction write-action output failover ( <PORT_LIST:port> | all )
 */
cparser_result_t
cparser_cmd_openflow_add_l3_cam_entry_index_instruction_write_action_output_failover_port_all(
    cparser_context_t *context,
    uint32_t *index_ptr,
    char **port_ptr)
{
    uint32 unit;
    int32 ret;
    diag_portlist_t portlist;
    rtk_of_l3CamFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    osal_memset(&portlist, 0, sizeof(portlist));
    DIAG_UTIL_EXTRACT_PORTLIST(portlist, 8);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3CamFlowEntry_get(unit, *index_ptr, &entry)))? RT_ERR_OK : ret, ret);
    entry.ins.wa_data.output_data.type = OF_OUTPUT_TYPE_FAILOVER;
    entry.ins.wa_data.output_data.portmask = portlist.portmask;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3CamFlowEntry_add(unit, *index_ptr, &entry), ret);
    DUMP_L3CAM_FLOWENTRY(*index_ptr, entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_add_l3_cam_entry_index_instruction_write_action_output_failover_port_all */
#endif

#ifdef CMD_OPENFLOW_ADD_L3_CAM_ENTRY_INDEX_INSTRUCTION_WRITE_METADATA_DATA_MASK
/*
 * openflow add l3-cam-entry <UINT:index> instruction write-metadata <UINT:data> <UINT:mask>
 */
cparser_result_t
cparser_cmd_openflow_add_l3_cam_entry_index_instruction_write_metadata_data_mask(
    cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *data_ptr,
    uint32_t *mask_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3CamFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3CamFlowEntry_get(unit, *index_ptr, &entry)))? RT_ERR_OK : ret, ret);
    entry.ins.wm_data.data = *data_ptr;
    entry.ins.wm_data.mask = *mask_ptr;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3CamFlowEntry_add(unit, *index_ptr, &entry), ret);
    DUMP_L3CAM_FLOWENTRY(*index_ptr, entry, OF_INS_ACT_TYPE_WRITEMETA);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_add_l3_cam_entry_index_instruction_write_metadata_data_mask */
#endif

#ifdef CMD_OPENFLOW_ADD_L3_CAM_ENTRY_INDEX_INSTRUCTION_GOTO_TABLE_NORMAL_TID
/*
 * openflow add l3-cam-entry <UINT:index> instruction goto-table normal <UINT:tid>
 */
cparser_result_t
cparser_cmd_openflow_add_l3_cam_entry_index_instruction_goto_table_normal_tid(
    cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *tid_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3CamFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3CamFlowEntry_get(unit, *index_ptr, &entry)))? RT_ERR_OK : ret, ret);
    entry.ins.gt_data.type = OF_GOTOTBL_TYPE_NORMAL;
    entry.ins.gt_data.tbl_id = *tid_ptr;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3CamFlowEntry_add(unit, *index_ptr, &entry), ret);
    DUMP_L3CAM_FLOWENTRY(*index_ptr, entry, OF_INS_ACT_TYPE_GOTOTBL);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_add_l3_cam_entry_index_instruction_goto_table_normal_tid */
#endif

#ifdef CMD_OPENFLOW_ADD_L3_CAM_ENTRY_INDEX_INSTRUCTION_GOTO_TABLE_LOOPBACK_APPLY_THEN_LOOPBACK_TID
/*
 * openflow add l3-cam-entry <UINT:index> instruction goto-table ( loopback | apply-then-loopback ) <UINT:tid>
 */
cparser_result_t
cparser_cmd_openflow_add_l3_cam_entry_index_instruction_goto_table_loopback_apply_then_loopback_tid(
    cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *tid_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3CamFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3CamFlowEntry_get(unit, *index_ptr, &entry)))? RT_ERR_OK : ret, ret);
    if ('l' == TOKEN_CHAR(6, 0))
        entry.ins.gt_data.type = OF_GOTOTBL_TYPE_LB;
    else if ('a' == TOKEN_CHAR(6, 0))
        entry.ins.gt_data.type = OF_GOTOTBL_TYPE_APPLY_AND_LB;
    entry.ins.gt_data.tbl_id = *tid_ptr;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3CamFlowEntry_add(unit, *index_ptr, &entry), ret);
    DUMP_L3CAM_FLOWENTRY(*index_ptr, entry, OF_INS_ACT_TYPE_GOTOTBL);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_add_l3_cam_entry_index_instruction_goto_table_loopback_apply_then_loopback_tid */
#endif

#ifdef CMD_OPENFLOW_DEL_L3_CAM_ENTRY_INDEX
/*
 * openflow del l3-cam-entry <UINT:index>
 */
cparser_result_t
cparser_cmd_openflow_del_l3_cam_entry_index(
    cparser_context_t *context,
    uint32_t *index)
{
    uint32 unit;
    int32 ret;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(rtk_of_l3CamFlowEntry_del(unit, *index), ret);
    DIAG_UTIL_MPRINTF("Delete l3 cam entry with index %u\n", *index);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_del_l3_cam_entry_start_end */
#endif

#ifdef CMD_OPENFLOW_MOVE_L3_CAM_ENTRY_START_END_ENTRY_NUM
/*
 * openflow move l3-cam-entry <UINT:start> <UINT:end> <UINT:entry_num>
 */
cparser_result_t
cparser_cmd_openflow_move_l3_cam_entry_start_end_entry_num(
    cparser_context_t *context,
    uint32_t *start_ptr,
    uint32_t *end_ptr,
    uint32_t *entry_num_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_flowMove_t fm;

    DIAG_UTIL_FUNC_INIT(unit);
    fm.move_from = *start_ptr;
    fm.move_to = *end_ptr;
    fm.length = *entry_num_ptr;
    DIAG_UTIL_ERR_CHK(rtk_of_l3CamFlowEntry_move(unit, &fm), ret);
    DIAG_UTIL_MPRINTF("Move %u physical entry from index %u to %u\n", *entry_num_ptr, *start_ptr, *end_ptr);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_move_l3_cam_entry_start_end_entry_num */
#endif

#ifdef CMD_OPENFLOW_GET_L3_CAM_ENTRY_INDEX_HIT_INDICATION
/*
 * openflow get l3-cam-entry <UINT:index> hit-indication
 */
cparser_result_t
cparser_cmd_openflow_get_l3_cam_entry_index_hit_indication(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32 unit;
    uint32 isHit = 0;
    int32 ret;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(rtk_of_l3CamflowEntryHitSts_get(unit, *index_ptr, FALSE, &isHit), ret);
    DIAG_UTIL_MPRINTF("L3 cam entry %u hit-indication is %u\n", *index_ptr, isHit);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_get_l3_cam_entry_index_hit_indication */
#endif

#ifdef CMD_OPENFLOW_RESET_L3_CAM_ENTRY_INDEX_HIT_INDICATION
/*
 * openflow reset l3-cam-entry <UINT:index> hit-indication
 */
cparser_result_t
cparser_cmd_openflow_reset_l3_cam_entry_index_hit_indication(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32 unit;
    uint32 isHit = 0;
    int32 ret;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(rtk_of_l3CamflowEntryHitSts_get(unit, *index_ptr, TRUE, &isHit), ret);
    DIAG_UTIL_MPRINTF("L3 cam entry %u hit-indication is %u\n", *index_ptr, isHit);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_reset_l3_cam_entry_index_hit_indication */
#endif

#ifdef CMD_OPENFLOW_GET_L3_HASH_ENTRY_TYPE_SIP_DIP_IP_METADATA_NONE
/*
 * openflow get l3-hash-entry type ( sip | dip ) <IPV4ADDR:ip> ( <UINT:metadata> | none )
 */
cparser_result_t
cparser_cmd_openflow_get_l3_hash_entry_type_sip_dip_ip_metadata_none(
    cparser_context_t *context,
    uint32_t *ip_ptr,
    uint32_t *metadata_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V4_PARSE(ip_ptr, ip_ptr, 6);
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_get(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_END);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_get_l3_hash_entry_type_sip_dip_ip_metadata_none */
#endif

#ifdef CMD_OPENFLOW_GET_L3_HASH_ENTRY_TYPE_SIP_DIP_SIP_DIP_METADATA_NONE
/*
 * openflow get l3-hash-entry type sip-dip <IPV4ADDR:sip> <IPV4ADDR:dip> ( <UINT:metadata> | none )
 */
cparser_result_t
cparser_cmd_openflow_get_l3_hash_entry_type_sip_dip_sip_dip_metadata_none(
    cparser_context_t *context,
    uint32_t *sip_ptr,
    uint32_t *dip_ptr,
    uint32_t *metadata_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V4_PARSE(sip_ptr, dip_ptr, 7);
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_get(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_END);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_get_l3_hash_entry_type_sip_dip_sip_dip_metadata_none */
#endif

#ifdef CMD_OPENFLOW_GET_L3_HASH_ENTRY_TYPE_SIP6_DIP6_IP_METADATA_NONE
/*
 * openflow get l3-hash-entry type ( sip6 | dip6 ) <IPV6ADDR:ip> ( <UINT:metadata> | none )
 */
cparser_result_t
cparser_cmd_openflow_get_l3_hash_entry_type_sip6_dip6_ip_metadata_none(
    cparser_context_t *context,
    char **ip_ptr,
    uint32_t *metadata_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V6_PARSE(ip_ptr, ip_ptr, 6);
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_get(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_END);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_get_l3_hash_entry_type_sip6_dip6_ip_metadata_none */
#endif

#ifdef CMD_OPENFLOW_GET_L3_HASH_ENTRY_TYPE_SIP6_DIP6_SIP_DIP_METADATA_NONE
/*
 * openflow get l3-hash-entry type sip6-dip6 <IPV6ADDR:sip> <IPV6ADDR:dip> ( <UINT:metadata> | none )
 */
cparser_result_t
cparser_cmd_openflow_get_l3_hash_entry_type_sip6_dip6_sip_dip_metadata_none(
    cparser_context_t *context,
    char **sip_ptr,
    char **dip_ptr,
    uint32_t *metadata_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V6_PARSE(sip_ptr, dip_ptr, 7);
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_get(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_END);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_get_l3_hash_entry_type_sip6_dip6_sip_dip_metadata_none */
#endif

#ifdef CMD_OPENFLOW_DUMP_L3_HASH_ENTRY
/*
 * openflow dump l3-hash-entry
 */
cparser_result_t
cparser_cmd_openflow_dump_l3_hash_entry(
    cparser_context_t *context)
{
    uint32 unit;
    int32 scan_idx = -1;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;
    char sipStr[128] = "", dipStr[128] = "";
    uint32 totalEntries = 0;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));

    diag_util_mprintf("L3 Hash-based Flow Table Entries\n\n");
    diag_util_mprintf(" Index | Type         | Source IP                               | Destination IP                          | MD_State | MD  \n");
    diag_util_mprintf("---------------------------------------------------------------------------------------------------------------------------\n");

    while (1)
    {
        osal_memset(&entry, 0 , sizeof(entry));
        osal_memset(sipStr, 0 , 128);
        osal_memset(dipStr, 0 , 128);

        ret = rtk_of_l3HashFlowEntryNextValid_get(unit, &scan_idx, &entry);

        if (ret == RT_ERR_ENTRY_NOTFOUND || ret == RT_ERR_OUT_OF_RANGE)
            break;

        if (ret != RT_ERR_OK)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }

        if (entry.type == OF_L3_FLOW_ENTRY_TYPE_IP4_SIP || entry.type == OF_L3_FLOW_ENTRY_TYPE_IP4_SIP_DIP)
            diag_util_ip2str(sipStr, entry.sip.ipv4);
        if (entry.type == OF_L3_FLOW_ENTRY_TYPE_IP4_DIP || entry.type == OF_L3_FLOW_ENTRY_TYPE_IP4_SIP_DIP)
            diag_util_ip2str(dipStr, entry.dip.ipv4);
        if (entry.type == OF_L3_FLOW_ENTRY_TYPE_IP6_SIP || entry.type == OF_L3_FLOW_ENTRY_TYPE_IP6_SIP_DIP)
            diag_util_ipv62str(sipStr, entry.sip.ipv6.octet);
        if (entry.type == OF_L3_FLOW_ENTRY_TYPE_IP6_DIP || entry.type == OF_L3_FLOW_ENTRY_TYPE_IP6_SIP_DIP)
            diag_util_ipv62str(dipStr, entry.dip.ipv6.octet);

        diag_util_mprintf(" %5d | %12s | %39s | %39s | %s | 0x%02X\n",\
            scan_idx, text_l3_flow_entry_type[entry.type], sipStr, dipStr,\
            (entry.flags&RTK_OF_FLAG_FLOWENTRY_MD_CMP)?"enable  ":"disable ", entry.metadata);

        totalEntries++;
    }

    diag_util_mprintf("\nTotal Number Of Entries : %d\n",totalEntries);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_dump_l3_hash_entry */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP_DIP_IP_METADATA_NONE_INSTRUTCION_CLEAR_ACTION_WRITE_ACTION_WRITE_METADATA_GOTO_TABLE_STATE_DISABLE_ENABLE
/*
 * openflow set l3-hash-entry type ( sip | dip ) <IPV4ADDR:ip> ( <UINT:metadata> | none ) instrutcion ( clear-action | write-action | write-metadata | goto-table ) state ( disable | enable )
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_ip_metadata_none_instrutcion_clear_action_write_action_write_metadata_goto_table_state_disable_enable(
    cparser_context_t *context,
    uint32_t *ip_ptr,
    uint32_t *metadata_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;
    rtk_of_ins_action_type_t type = OF_INS_ACT_TYPE_END;
    rtk_enable_t state = DISABLED;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V4_PARSE(ip_ptr, ip_ptr, 6);
    DIAG_OF_PHASE_INSTRUCTION(8, type);
    DIAG_UTIL_PARSE_STATE(10, state);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    if (type == OF_INS_ACT_TYPE_CLEARACT)
        entry.ins.clearAct_en = state;
    else if (type == OF_INS_ACT_TYPE_WRITEACT)
        entry.ins.writeAct_en = state;
    else if (type == OF_INS_ACT_TYPE_WRITEMETA)
        entry.ins.writeMetadata_en = state;
    else if (type == OF_INS_ACT_TYPE_GOTOTBL)
        entry.ins.gotoTbl_en = state;

    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_ip_metadata_none_instrutcion_clear_action_write_action_write_metadata_goto_table_state_disable_enable */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP_DIP_SIP_DIP_METADATA_NONE_INSTRUTCION_CLEAR_ACTION_WRITE_ACTION_WRITE_METADATA_GOTO_TABLE_STATE_DISABLE_ENABLE
/*
 * openflow set l3-hash-entry type sip-dip <IPV4ADDR:sip> <IPV4ADDR:dip> ( <UINT:metadata> | none ) instrutcion ( clear-action | write-action | write-metadata | goto-table ) state ( disable | enable )
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_sip_dip_metadata_none_instrutcion_clear_action_write_action_write_metadata_goto_table_state_disable_enable(
    cparser_context_t *context,
    uint32_t *sip_ptr,
    uint32_t *dip_ptr,
    uint32_t *metadata_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;
    rtk_of_ins_action_type_t type = OF_INS_ACT_TYPE_END;
    rtk_enable_t state = DISABLED;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V4_PARSE(sip_ptr, dip_ptr, 7);
    DIAG_OF_PHASE_INSTRUCTION(9, type);
    DIAG_UTIL_PARSE_STATE(11, state);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    if (type == OF_INS_ACT_TYPE_CLEARACT)
        entry.ins.clearAct_en = state;
    else if (type == OF_INS_ACT_TYPE_WRITEACT)
        entry.ins.writeAct_en = state;
    else if (type == OF_INS_ACT_TYPE_WRITEMETA)
        entry.ins.writeMetadata_en = state;
    else if (type == OF_INS_ACT_TYPE_GOTOTBL)
        entry.ins.gotoTbl_en = state;

    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_sip_dip_metadata_none_instrutcion_clear_action_write_action_write_metadata_goto_table_state_disable_enable */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP6_DIP6_IP_METADATA_NONE_INSTRUTCION_CLEAR_ACTION_WRITE_ACTION_WRITE_METADATA_GOTO_TABLE_STATE_DISABLE_ENABLE
/*
 * openflow set l3-hash-entry type ( sip6 | dip6 ) <IPV6ADDR:ip> ( <UINT:metadata> | none ) instrutcion ( clear-action | write-action | write-metadata | goto-table ) state ( disable | enable )
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip6_dip6_ip_metadata_none_instrutcion_clear_action_write_action_write_metadata_goto_table_state_disable_enable(
    cparser_context_t *context,
    char **ip_ptr,
    uint32_t *metadata_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;
    rtk_of_ins_action_type_t type = OF_INS_ACT_TYPE_END;
    rtk_enable_t state = DISABLED;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V6_PARSE(ip_ptr, ip_ptr, 6);
    DIAG_OF_PHASE_INSTRUCTION(8, type);
    DIAG_UTIL_PARSE_STATE(10, state);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    if (type == OF_INS_ACT_TYPE_CLEARACT)
        entry.ins.clearAct_en = state;
    else if (type == OF_INS_ACT_TYPE_WRITEACT)
        entry.ins.writeAct_en = state;
    else if (type == OF_INS_ACT_TYPE_WRITEMETA)
        entry.ins.writeMetadata_en = state;
    else if (type == OF_INS_ACT_TYPE_GOTOTBL)
        entry.ins.gotoTbl_en = state;

    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip6_dip6_ip_metadata_none_instrutcion_clear_action_write_action_write_metadata_goto_table_state_disable_enable */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP6_DIP6_SIP_DIP_METADATA_NONE_INSTRUTCION_CLEAR_ACTION_WRITE_ACTION_WRITE_METADATA_GOTO_TABLE_STATE_DISABLE_ENABLE
/*
 * openflow set l3-hash-entry type sip6-dip6 <IPV6ADDR:sip> <IPV6ADDR:dip> ( <UINT:metadata> | none ) instrutcion ( clear-action | write-action | write-metadata | goto-table ) state ( disable | enable )
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip6_dip6_sip_dip_metadata_none_instrutcion_clear_action_write_action_write_metadata_goto_table_state_disable_enable(
    cparser_context_t *context,
    char **sip_ptr,
    char **dip_ptr,
    uint32_t *metadata_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;
    rtk_of_ins_action_type_t type = OF_INS_ACT_TYPE_END;
    rtk_enable_t state = DISABLED;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V6_PARSE(sip_ptr, dip_ptr, 7);
    DIAG_OF_PHASE_INSTRUCTION(9, type);
    DIAG_UTIL_PARSE_STATE(11, state);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    if (type == OF_INS_ACT_TYPE_CLEARACT)
        entry.ins.clearAct_en = state;
    else if (type == OF_INS_ACT_TYPE_WRITEACT)
        entry.ins.writeAct_en = state;
    else if (type == OF_INS_ACT_TYPE_WRITEMETA)
        entry.ins.writeMetadata_en = state;
    else if (type == OF_INS_ACT_TYPE_GOTOTBL)
        entry.ins.gotoTbl_en = state;

    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip6_dip6_sip_dip_metadata_none_instrutcion_clear_action_write_action_write_metadata_goto_table_state_disable_enable */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP_DIP_IP_METADATA_NONE_INSTRUTCION_WRITE_ACTION_PUSH_VLAN_DEC_IP_TTL_SET_QUEUE_GROUP_STATE_DISABLE_ENABLE
/*
 * openflow set l3-hash-entry type ( sip | dip ) <IPV4ADDR:ip> ( <UINT:metadata> | none ) instrutcion write-action ( push-vlan | dec-ip-ttl | set-queue | group ) state ( disable | enable )
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_ip_metadata_none_instrutcion_write_action_push_vlan_dec_ip_ttl_set_queue_group_state_disable_enable(
    cparser_context_t *context,
    uint32_t *ip_ptr,
    uint32_t *metadata_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;
    rtk_enable_t state = DISABLED;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V4_PARSE(ip_ptr, ip_ptr, 6);
    DIAG_UTIL_PARSE_STATE(11, state);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    if ('p' == TOKEN_CHAR(9, 0))
        entry.ins.wa_data.push_vlan = state;
    else if ('d' == TOKEN_CHAR(9, 0))
        entry.ins.wa_data.dec_ip_ttl = state;
    else if ('s' == TOKEN_CHAR(9, 0))
        entry.ins.wa_data.set_queue = state;
    else if ('g' == TOKEN_CHAR(9, 0))
        entry.ins.wa_data.group = state;

    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_ip_metadata_none_instrutcion_write_action_push_vlan_dec_ip_ttl_set_queue_group_state_disable_enable */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP_DIP_SIP_DIP_METADATA_NONE_INSTRUTCION_WRITE_ACTION_PUSH_VLAN_DEC_IP_TTL_SET_QUEUE_GROUP_STATE_DISABLE_ENABLE
/*
 * openflow set l3-hash-entry type sip-dip <IPV4ADDR:sip> <IPV4ADDR:dip> ( <UINT:metadata> | none ) instrutcion write-action ( push-vlan | dec-ip-ttl | set-queue | group ) state ( disable | enable )
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_sip_dip_metadata_none_instrutcion_write_action_push_vlan_dec_ip_ttl_set_queue_group_state_disable_enable(
    cparser_context_t *context,
    uint32_t *sip_ptr,
    uint32_t *dip_ptr,
    uint32_t *metadata_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;
    rtk_enable_t state = DISABLED;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V4_PARSE(sip_ptr, dip_ptr, 7);
    DIAG_UTIL_PARSE_STATE(12, state);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    if ('p' == TOKEN_CHAR(10, 0))
        entry.ins.wa_data.push_vlan = state;
    else if ('d' == TOKEN_CHAR(10, 0))
        entry.ins.wa_data.dec_ip_ttl = state;
    else if ('s' == TOKEN_CHAR(10, 0))
        entry.ins.wa_data.set_queue = state;
    else if ('g' == TOKEN_CHAR(10, 0))
        entry.ins.wa_data.group = state;

    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_sip_dip_metadata_none_instrutcion_write_action_push_vlan_dec_ip_ttl_set_queue_group_state_disable_enable */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP6_DIP6_IP_METADATA_NONE_INSTRUTCION_WRITE_ACTION_PUSH_VLAN_DEC_IP_TTL_SET_QUEUE_GROUP_STATE_DISABLE_ENABLE
/*
 * openflow set l3-hash-entry type ( sip6 | dip6 ) <IPV6ADDR:ip> ( <UINT:metadata> | none ) instrutcion write-action ( push-vlan | dec-ip-ttl | set-queue | group ) state ( disable | enable )
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip6_dip6_ip_metadata_none_instrutcion_write_action_push_vlan_dec_ip_ttl_set_queue_group_state_disable_enable(
    cparser_context_t *context,
    char **ip_ptr,
    uint32_t *metadata_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;
    rtk_enable_t state = DISABLED;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V6_PARSE(ip_ptr, ip_ptr, 6);
    DIAG_UTIL_PARSE_STATE(11, state);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    if ('p' == TOKEN_CHAR(9, 0))
        entry.ins.wa_data.push_vlan = state;
    else if ('d' == TOKEN_CHAR(9, 0))
        entry.ins.wa_data.dec_ip_ttl = state;
    else if ('s' == TOKEN_CHAR(9, 0))
        entry.ins.wa_data.set_queue = state;
    else if ('g' == TOKEN_CHAR(9, 0))
        entry.ins.wa_data.group = state;

    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip6_dip6_ip_metadata_none_instrutcion_write_action_push_vlan_dec_ip_ttl_set_queue_group_state_disable_enable */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP6_DIP6_SIP_DIP_METADATA_NONE_INSTRUTCION_WRITE_ACTION_PUSH_VLAN_DEC_IP_TTL_SET_QUEUE_GROUP_STATE_DISABLE_ENABLE
/*
 * openflow set l3-hash-entry type sip6-dip6 <IPV6ADDR:sip> <IPV6ADDR:dip> ( <UINT:metadata> | none ) instrutcion write-action ( push-vlan | dec-ip-ttl | set-queue | group ) state ( disable | enable )
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip6_dip6_sip_dip_metadata_none_instrutcion_write_action_push_vlan_dec_ip_ttl_set_queue_group_state_disable_enable(
    cparser_context_t *context,
    char **sip_ptr,
    char **dip_ptr,
    uint32_t *metadata_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;
    rtk_enable_t state = DISABLED;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V6_PARSE(sip_ptr, dip_ptr, 7);
    DIAG_UTIL_PARSE_STATE(12, state);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    if ('p' == TOKEN_CHAR(10, 0))
        entry.ins.wa_data.push_vlan = state;
    else if ('d' == TOKEN_CHAR(10, 0))
        entry.ins.wa_data.dec_ip_ttl = state;
    else if ('s' == TOKEN_CHAR(10, 0))
        entry.ins.wa_data.set_queue = state;
    else if ('g' == TOKEN_CHAR(10, 0))
        entry.ins.wa_data.group = state;

    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip6_dip6_sip_dip_metadata_none_instrutcion_write_action_push_vlan_dec_ip_ttl_set_queue_group_state_disable_enable */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP_DIP_IP_METADATA_NONE_INSTRUTCION_WRITE_ACTION_PUSH_VLAN_ETHERTYPE_TPID_IDX
/*
 * openflow set l3-hash-entry type ( sip | dip ) <IPV4ADDR:ip> ( <UINT:metadata> | none ) instrutcion write-action push-vlan ethertype <UINT:tpid_idx>
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_ip_metadata_none_instrutcion_write_action_push_vlan_ethertype_tpid_idx(
    cparser_context_t *context,
    uint32_t *ip_ptr,
    uint32_t *metadata_ptr,
    uint32_t *tpid_idx_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V4_PARSE(ip_ptr, ip_ptr, 6);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);
    entry.ins.wa_data.push_vlan_data.etherType_idx = *tpid_idx_ptr;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_ip_metadata_none_instrutcion_write_action_push_vlan_ethertype_tpid_idx */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP_DIP_SIP_DIP_METADATA_NONE_INSTRUTCION_WRITE_ACTION_PUSH_VLAN_ETHERTYPE_TPID_IDX
/*
 * openflow set l3-hash-entry type sip-dip <IPV4ADDR:sip> <IPV4ADDR:dip> ( <UINT:metadata> | none ) instrutcion write-action push-vlan ethertype <UINT:tpid_idx>
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_sip_dip_metadata_none_instrutcion_write_action_push_vlan_ethertype_tpid_idx(
    cparser_context_t *context,
    uint32_t *sip_ptr,
    uint32_t *dip_ptr,
    uint32_t *metadata_ptr,
    uint32_t *tpid_idx_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V4_PARSE(sip_ptr, dip_ptr, 7);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);
    entry.ins.wa_data.push_vlan_data.etherType_idx = *tpid_idx_ptr;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_sip_dip_metadata_none_instrutcion_write_action_push_vlan_ethertype_tpid_idx */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP6_DIP6_IP_METADATA_NONE_INSTRUTCION_WRITE_ACTION_PUSH_VLAN_ETHERTYPE_TPID_IDX
/*
 * openflow set l3-hash-entry type ( sip6 | dip6 ) <IPV6ADDR:ip> ( <UINT:metadata> | none ) instrutcion write-action push-vlan ethertype <UINT:tpid_idx>
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip6_dip6_ip_metadata_none_instrutcion_write_action_push_vlan_ethertype_tpid_idx(
    cparser_context_t *context,
    char **ip_ptr,
    uint32_t *metadata_ptr,
    uint32_t *tpid_idx_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V6_PARSE(ip_ptr, ip_ptr, 6);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);
    entry.ins.wa_data.push_vlan_data.etherType_idx = *tpid_idx_ptr;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_WRITEACT);
    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip6_dip6_ip_metadata_none_instrutcion_write_action_push_vlan_ethertype_tpid_idx */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP6_DIP6_SIP_DIP_METADATA_NONE_INSTRUTCION_WRITE_ACTION_PUSH_VLAN_ETHERTYPE_TPID_IDX
/*
 * openflow set l3-hash-entry type sip6-dip6 <IPV6ADDR:sip> <IPV6ADDR:dip> ( <UINT:metadata> | none ) instrutcion write-action push-vlan ethertype <UINT:tpid_idx>
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip6_dip6_sip_dip_metadata_none_instrutcion_write_action_push_vlan_ethertype_tpid_idx(
    cparser_context_t *context,
    char **sip_ptr,
    char **dip_ptr,
    uint32_t *metadata_ptr,
    uint32_t *tpid_idx_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V6_PARSE(sip_ptr, dip_ptr, 7);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);
    entry.ins.wa_data.push_vlan_data.etherType_idx = *tpid_idx_ptr;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip6_dip6_sip_dip_metadata_none_instrutcion_write_action_push_vlan_ethertype_tpid_idx */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP_DIP_IP_METADATA_NONE_INSTRUTCION_WRITE_ACTION_SET_FIELD_FIELD_ID_FIELD_TYPE_NONE
/*
 * openflow set l3-hash-entry type ( sip | dip ) <IPV4ADDR:ip> ( <UINT:metadata> | none ) instrutcion write-action set-field <UINT:field_id> field-type none
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_ip_metadata_none_instrutcion_write_action_set_field_field_id_field_type_none(
    cparser_context_t *context,
    uint32_t *ip_ptr,
    uint32_t *metadata_ptr,
    uint32_t *field_id_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V4_PARSE(ip_ptr, ip_ptr, 6);
    DIAG_UTIL_ERR_CHK(rtk_of_l3FlowEntrySetField_check(unit, *field_id_ptr, OF_SET_FIELD_TYPE_NONE), ret);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    if (0 == *field_id_ptr)
        entry.ins.wa_data.set_field_0_data.field_type = OF_IGR_FT2_ACT_SF_TYPE0_NONE;
    else if (1 == *field_id_ptr)
        entry.ins.wa_data.set_field_1_data.field_type = OF_IGR_FT2_ACT_SF_TYPE1_NONE;
    else if (2 == *field_id_ptr)
        entry.ins.wa_data.set_field_2_data.field_type = OF_IGR_FT2_ACT_SF_TYPE2_NONE;
    else
        DIAG_UTIL_ERR_CHK(RT_ERR_INPUT, ret);

    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_ip_metadata_none_instrutcion_write_action_set_field_field_id_field_type_none */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP_DIP_SIP_DIP_METADATA_NONE_INSTRUTCION_WRITE_ACTION_SET_FIELD_FIELD_ID_FIELD_TYPE_NONE
/*
 * openflow set l3-hash-entry type sip-dip <IPV4ADDR:sip> <IPV4ADDR:dip> ( <UINT:metadata> | none ) instrutcion write-action set-field <UINT:field_id> field-type none
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_sip_dip_metadata_none_instrutcion_write_action_set_field_field_id_field_type_none(
    cparser_context_t *context,
    uint32_t *sip_ptr,
    uint32_t *dip_ptr,
    uint32_t *metadata_ptr,
    uint32_t *field_id_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V4_PARSE(sip_ptr, dip_ptr, 7);
    DIAG_UTIL_ERR_CHK(rtk_of_l3FlowEntrySetField_check(unit, *field_id_ptr, OF_SET_FIELD_TYPE_NONE), ret);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    if (0 == *field_id_ptr)
        entry.ins.wa_data.set_field_0_data.field_type = OF_IGR_FT2_ACT_SF_TYPE0_NONE;
    else if (1 == *field_id_ptr)
        entry.ins.wa_data.set_field_1_data.field_type = OF_IGR_FT2_ACT_SF_TYPE1_NONE;
    else if (2 == *field_id_ptr)
        entry.ins.wa_data.set_field_2_data.field_type = OF_IGR_FT2_ACT_SF_TYPE2_NONE;
    else
        DIAG_UTIL_ERR_CHK(RT_ERR_INPUT, ret);

    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_sip_dip_metadata_none_instrutcion_write_action_set_field_field_id_field_type_none */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP6_DIP6_IP_METADATA_NONE_INSTRUTCION_WRITE_ACTION_SET_FIELD_FIELD_ID_FIELD_TYPE_NONE
/*
 * openflow set l3-hash-entry type ( sip6 | dip6 ) <IPV6ADDR:ip> ( <UINT:metadata> | none ) instrutcion write-action set-field <UINT:field_id> field-type none
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip6_dip6_ip_metadata_none_instrutcion_write_action_set_field_field_id_field_type_none(
    cparser_context_t *context,
    char **ip_ptr,
    uint32_t *metadata_ptr,
    uint32_t *field_id_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V6_PARSE(ip_ptr, ip_ptr, 6);
    DIAG_UTIL_ERR_CHK(rtk_of_l3FlowEntrySetField_check(unit, *field_id_ptr, OF_SET_FIELD_TYPE_NONE), ret);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    if (0 == *field_id_ptr)
        entry.ins.wa_data.set_field_0_data.field_type = OF_IGR_FT2_ACT_SF_TYPE0_NONE;
    else if (1 == *field_id_ptr)
        entry.ins.wa_data.set_field_1_data.field_type = OF_IGR_FT2_ACT_SF_TYPE1_NONE;
    else if (2 == *field_id_ptr)
        entry.ins.wa_data.set_field_2_data.field_type = OF_IGR_FT2_ACT_SF_TYPE2_NONE;
    else
        DIAG_UTIL_ERR_CHK(RT_ERR_INPUT, ret);

    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip6_dip6_ip_metadata_none_instrutcion_write_action_set_field_field_id_field_type_none */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP6_DIP6_SIP_DIP_METADATA_NONE_INSTRUTCION_WRITE_ACTION_SET_FIELD_FIELD_ID_FIELD_TYPE_NONE
/*
 * openflow set l3-hash-entry type sip6-dip6 <IPV6ADDR:sip> <IPV6ADDR:dip> ( <UINT:metadata> | none ) instrutcion write-action set-field <UINT:field_id> field-type none
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip6_dip6_sip_dip_metadata_none_instrutcion_write_action_set_field_field_id_field_type_none(
    cparser_context_t *context,
    char **sip_ptr,
    char **dip_ptr,
    uint32_t *metadata_ptr,
    uint32_t *field_id_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V6_PARSE(sip_ptr, dip_ptr, 7);
    DIAG_UTIL_ERR_CHK(rtk_of_l3FlowEntrySetField_check(unit, *field_id_ptr, OF_SET_FIELD_TYPE_NONE), ret);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    if (0 == *field_id_ptr)
        entry.ins.wa_data.set_field_0_data.field_type = OF_IGR_FT2_ACT_SF_TYPE0_NONE;
    else if (1 == *field_id_ptr)
        entry.ins.wa_data.set_field_1_data.field_type = OF_IGR_FT2_ACT_SF_TYPE1_NONE;
    else if (2 == *field_id_ptr)
        entry.ins.wa_data.set_field_2_data.field_type = OF_IGR_FT2_ACT_SF_TYPE2_NONE;
    else
        DIAG_UTIL_ERR_CHK(RT_ERR_INPUT, ret);

    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip6_dip6_sip_dip_metadata_none_instrutcion_write_action_set_field_field_id_field_type_none */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP_DIP_IP_METADATA_NONE_INSTRUTCION_WRITE_ACTION_SET_FIELD_FIELD_ID_FIELD_TYPE_SMAC_DMAC_MAC
/*
 * openflow set l3-hash-entry type ( sip | dip ) <IPV4ADDR:ip> ( <UINT:metadata> | none ) instrutcion write-action set-field <UINT:field_id> field-type ( smac | dmac ) <MACADDR:mac>
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_ip_metadata_none_instrutcion_write_action_set_field_field_id_field_type_smac_dmac_mac(
    cparser_context_t *context,
    uint32_t *ip_ptr,
    uint32_t *metadata_ptr,
    uint32_t *field_id_ptr,
    cparser_macaddr_t *mac_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V4_PARSE(ip_ptr, ip_ptr, 6);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    if ('s' == TOKEN_CHAR(12, 0))
    {
        DIAG_UTIL_ERR_CHK(rtk_of_l3FlowEntrySetField_check(unit, *field_id_ptr, OF_IGR_FT2_ACT_SF_TYPE0_SA), ret);
        entry.ins.wa_data.set_field_0_data.field_type = OF_IGR_FT2_ACT_SF_TYPE0_SA;
        osal_memcpy(entry.ins.wa_data.set_field_0_data.mac.octet, mac_ptr, ETHER_ADDR_LEN);
    }
    else if ('d' == TOKEN_CHAR(12, 0))
    {
        DIAG_UTIL_ERR_CHK(rtk_of_l3FlowEntrySetField_check(unit, *field_id_ptr, OF_IGR_FT2_ACT_SF_TYPE1_DA), ret);
        entry.ins.wa_data.set_field_1_data.field_type = OF_IGR_FT2_ACT_SF_TYPE1_DA;
        osal_memcpy(entry.ins.wa_data.set_field_1_data.mac.octet, mac_ptr, ETHER_ADDR_LEN);
    }

    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_ip_metadata_none_instrutcion_write_action_set_field_field_id_field_type_smac_dmac_mac */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP_DIP_SIP_DIP_METADATA_NONE_INSTRUTCION_WRITE_ACTION_SET_FIELD_FIELD_ID_FIELD_TYPE_SMAC_DMAC_MAC
/*
 * openflow set l3-hash-entry type sip-dip <IPV4ADDR:sip> <IPV4ADDR:dip> ( <UINT:metadata> | none ) instrutcion write-action set-field <UINT:field_id> field-type ( smac | dmac ) <MACADDR:mac>
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_sip_dip_metadata_none_instrutcion_write_action_set_field_field_id_field_type_smac_dmac_mac(
    cparser_context_t *context,
    uint32_t *sip_ptr,
    uint32_t *dip_ptr,
    uint32_t *metadata_ptr,
    uint32_t *field_id_ptr,
    cparser_macaddr_t *mac_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V4_PARSE(sip_ptr, dip_ptr, 7);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    if ('s' == TOKEN_CHAR(13, 0))
    {
        DIAG_UTIL_ERR_CHK(rtk_of_l3FlowEntrySetField_check(unit, *field_id_ptr, OF_IGR_FT2_ACT_SF_TYPE0_SA), ret);
        entry.ins.wa_data.set_field_0_data.field_type = OF_IGR_FT2_ACT_SF_TYPE0_SA;
        osal_memcpy(entry.ins.wa_data.set_field_0_data.mac.octet, mac_ptr, ETHER_ADDR_LEN);
    }
    else if ('d' == TOKEN_CHAR(13, 0))
    {
        DIAG_UTIL_ERR_CHK(rtk_of_l3FlowEntrySetField_check(unit, *field_id_ptr, OF_IGR_FT2_ACT_SF_TYPE1_DA), ret);
        entry.ins.wa_data.set_field_1_data.field_type = OF_IGR_FT2_ACT_SF_TYPE1_DA;
        osal_memcpy(entry.ins.wa_data.set_field_1_data.mac.octet, mac_ptr, ETHER_ADDR_LEN);
    }

    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_sip_dip_metadata_none_instrutcion_write_action_set_field_field_id_field_type_smac_dmac_mac */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP6_DIP6_IP_METADATA_NONE_INSTRUTCION_WRITE_ACTION_SET_FIELD_FIELD_ID_FIELD_TYPE_SMAC_DMAC_MAC
/*
 * openflow set l3-hash-entry type ( sip6 | dip6 ) <IPV6ADDR:ip> ( <UINT:metadata> | none ) instrutcion write-action set-field <UINT:field_id> field-type ( smac | dmac ) <MACADDR:mac>
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip6_dip6_ip_metadata_none_instrutcion_write_action_set_field_field_id_field_type_smac_dmac_mac(
    cparser_context_t *context,
    char **ip_ptr,
    uint32_t *metadata_ptr,
    uint32_t *field_id_ptr,
    cparser_macaddr_t *mac_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V6_PARSE(ip_ptr, ip_ptr, 6);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    if ('s' == TOKEN_CHAR(12, 0))
    {
        DIAG_UTIL_ERR_CHK(rtk_of_l3FlowEntrySetField_check(unit, *field_id_ptr, OF_IGR_FT2_ACT_SF_TYPE0_SA), ret);
        entry.ins.wa_data.set_field_0_data.field_type = OF_IGR_FT2_ACT_SF_TYPE0_SA;
        osal_memcpy(entry.ins.wa_data.set_field_0_data.mac.octet, mac_ptr, ETHER_ADDR_LEN);
    }
    else if ('d' == TOKEN_CHAR(12, 0))
    {
        DIAG_UTIL_ERR_CHK(rtk_of_l3FlowEntrySetField_check(unit, *field_id_ptr, OF_IGR_FT2_ACT_SF_TYPE1_DA), ret);
        entry.ins.wa_data.set_field_1_data.field_type = OF_IGR_FT2_ACT_SF_TYPE1_DA;
        osal_memcpy(entry.ins.wa_data.set_field_1_data.mac.octet, mac_ptr, ETHER_ADDR_LEN);
    }

    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip6_dip6_ip_metadata_none_instrutcion_write_action_set_field_field_id_field_type_smac_dmac_mac */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP6_DIP6_SIP_DIP_METADATA_NONE_INSTRUTCION_WRITE_ACTION_SET_FIELD_FIELD_ID_FIELD_TYPE_SMAC_DMAC_MAC
/*
 * openflow set l3-hash-entry type sip6-dip6 <IPV6ADDR:sip> <IPV6ADDR:dip> ( <UINT:metadata> | none ) instrutcion write-action set-field <UINT:field_id> field-type ( smac | dmac ) <MACADDR:mac>
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip6_dip6_sip_dip_metadata_none_instrutcion_write_action_set_field_field_id_field_type_smac_dmac_mac(
    cparser_context_t *context,
    char **sip_ptr,
    char **dip_ptr,
    uint32_t *metadata_ptr,
    uint32_t *field_id_ptr,
    cparser_macaddr_t *mac_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V6_PARSE(sip_ptr, dip_ptr, 7);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    if ('s' == TOKEN_CHAR(13, 0))
    {
        DIAG_UTIL_ERR_CHK(rtk_of_l3FlowEntrySetField_check(unit, *field_id_ptr, OF_IGR_FT2_ACT_SF_TYPE0_SA), ret);
        entry.ins.wa_data.set_field_0_data.field_type = OF_IGR_FT2_ACT_SF_TYPE0_SA;
        osal_memcpy(entry.ins.wa_data.set_field_0_data.mac.octet, mac_ptr, ETHER_ADDR_LEN);
    }
    else if ('d' == TOKEN_CHAR(13, 0))
    {
        DIAG_UTIL_ERR_CHK(rtk_of_l3FlowEntrySetField_check(unit, *field_id_ptr, OF_IGR_FT2_ACT_SF_TYPE1_DA), ret);
        entry.ins.wa_data.set_field_1_data.field_type = OF_IGR_FT2_ACT_SF_TYPE1_DA;
        osal_memcpy(entry.ins.wa_data.set_field_1_data.mac.octet, mac_ptr, ETHER_ADDR_LEN);
    }

    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip6_dip6_sip_dip_metadata_none_instrutcion_write_action_set_field_field_id_field_type_smac_dmac_mac */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP_DIP_IP_METADATA_NONE_INSTRUTCION_WRITE_ACTION_SET_FIELD_FIELD_ID_FIELD_TYPE_VLAN_PRIORITY_VID_DSCP_VALUE
/*
 * openflow set l3-hash-entry type ( sip | dip ) <IPV4ADDR:ip> ( <UINT:metadata> | none ) instrutcion write-action set-field <UINT:field_id> field-type ( vlan-priority | vid | dscp ) <UINT:value>
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_ip_metadata_none_instrutcion_write_action_set_field_field_id_field_type_vlan_priority_vid_dscp_value(
    cparser_context_t *context,
    uint32_t *ip_ptr,
    uint32_t *metadata_ptr,
    uint32_t *field_id_ptr,
    uint32_t *value_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;
    rtk_of_setFieldType_t type = OF_SET_FIELD_TYPE_NONE;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V4_PARSE(ip_ptr, ip_ptr, 6);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    if ('l' == TOKEN_CHAR(12, 1))
        type = OF_SET_FIELD_TYPE_VLAN_PRI;
    else if ('i' == TOKEN_CHAR(12, 1))
        type = OF_SET_FIELD_TYPE_VLAN_ID;
    else if ('s' == TOKEN_CHAR(12, 1))
        type = OF_SET_FIELD_TYPE_IP_DSCP;

    DIAG_UTIL_ERR_CHK(rtk_of_l3FlowEntrySetField_check(unit, *field_id_ptr, type), ret);

    if (0 == *field_id_ptr)
    {
        entry.ins.wa_data.set_field_0_data.field_type = type;
        entry.ins.wa_data.set_field_0_data.field_data = *value_ptr;
    }
    else if (1 == *field_id_ptr)
    {
        entry.ins.wa_data.set_field_1_data.field_type = type;
        entry.ins.wa_data.set_field_1_data.field_data = *value_ptr;
    }
    else if (2 == *field_id_ptr)
    {
        entry.ins.wa_data.set_field_2_data.field_type = type;
        entry.ins.wa_data.set_field_2_data.field_data = *value_ptr;
    }

    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_ip_metadata_none_instrutcion_write_action_set_field_field_id_field_type_vlan_priority_vid_dscp_value */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP_DIP_SIP_DIP_METADATA_NONE_INSTRUTCION_WRITE_ACTION_SET_FIELD_FIELD_ID_FIELD_TYPE_VLAN_PRIORITY_VID_DSCP_VALUE
/*
 * openflow set l3-hash-entry type sip-dip <IPV4ADDR:sip> <IPV4ADDR:dip> ( <UINT:metadata> | none ) instrutcion write-action set-field <UINT:field_id> field-type ( vlan-priority | vid | dscp ) <UINT:value>
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_sip_dip_metadata_none_instrutcion_write_action_set_field_field_id_field_type_vlan_priority_vid_dscp_value(
    cparser_context_t *context,
    uint32_t *sip_ptr,
    uint32_t *dip_ptr,
    uint32_t *metadata_ptr,
    uint32_t *field_id_ptr,
    uint32_t *value_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;
    rtk_of_setFieldType_t type = OF_SET_FIELD_TYPE_NONE;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V4_PARSE(sip_ptr, dip_ptr, 7);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    if ('l' == TOKEN_CHAR(13, 1))
        type = OF_SET_FIELD_TYPE_VLAN_PRI;
    else if ('i' == TOKEN_CHAR(13, 1))
        type = OF_SET_FIELD_TYPE_VLAN_ID;
    else if ('s' == TOKEN_CHAR(13, 1))
        type = OF_SET_FIELD_TYPE_IP_DSCP;

    DIAG_UTIL_ERR_CHK(rtk_of_l3FlowEntrySetField_check(unit, *field_id_ptr, type), ret);

    if (0 == *field_id_ptr)
    {
        entry.ins.wa_data.set_field_0_data.field_type = type;
        entry.ins.wa_data.set_field_0_data.field_data = *value_ptr;
    }
    else if (1 == *field_id_ptr)
    {
        entry.ins.wa_data.set_field_1_data.field_type = type;
        entry.ins.wa_data.set_field_1_data.field_data = *value_ptr;
    }
    else if (2 == *field_id_ptr)
    {
        entry.ins.wa_data.set_field_2_data.field_type = type;
        entry.ins.wa_data.set_field_2_data.field_data = *value_ptr;
    }

    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_sip_dip_metadata_none_instrutcion_write_action_set_field_field_id_field_type_vlan_priority_vid_dscp_value */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP6_DIP6_IP_METADATA_NONE_INSTRUTCION_WRITE_ACTION_SET_FIELD_FIELD_ID_FIELD_TYPE_VLAN_PRIORITY_VID_DSCP_VALUE
/*
 * openflow set l3-hash-entry type ( sip6 | dip6 ) <IPV6ADDR:ip> ( <UINT:metadata> | none ) instrutcion write-action set-field <UINT:field_id> field-type ( vlan-priority | vid | dscp ) <UINT:value>
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip6_dip6_ip_metadata_none_instrutcion_write_action_set_field_field_id_field_type_vlan_priority_vid_dscp_value(
    cparser_context_t *context,
    char **ip_ptr,
    uint32_t *metadata_ptr,
    uint32_t *field_id_ptr,
    uint32_t *value_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;
    rtk_of_setFieldType_t type = OF_SET_FIELD_TYPE_NONE;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V6_PARSE(ip_ptr, ip_ptr, 6);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    if ('l' == TOKEN_CHAR(12, 1))
        type = OF_SET_FIELD_TYPE_VLAN_PRI;
    else if ('i' == TOKEN_CHAR(12, 1))
        type = OF_SET_FIELD_TYPE_VLAN_ID;
    else if ('s' == TOKEN_CHAR(12, 1))
        type = OF_SET_FIELD_TYPE_IP_DSCP;

    DIAG_UTIL_ERR_CHK(rtk_of_l3FlowEntrySetField_check(unit, *field_id_ptr, type), ret);

    if (0 == *field_id_ptr)
    {
        entry.ins.wa_data.set_field_0_data.field_type = type;
        entry.ins.wa_data.set_field_0_data.field_data = *value_ptr;
    }
    else if (1 == *field_id_ptr)
    {
        entry.ins.wa_data.set_field_1_data.field_type = type;
        entry.ins.wa_data.set_field_1_data.field_data = *value_ptr;
    }
    else if (2 == *field_id_ptr)
    {
        entry.ins.wa_data.set_field_2_data.field_type = type;
        entry.ins.wa_data.set_field_2_data.field_data = *value_ptr;
    }

    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip6_dip6_ip_metadata_none_instrutcion_write_action_set_field_field_id_field_type_vlan_priority_vid_dscp_value */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP6_DIP6_SIP_DIP_METADATA_NONE_INSTRUTCION_WRITE_ACTION_SET_FIELD_FIELD_ID_FIELD_TYPE_VLAN_PRIORITY_VID_DSCP_VALUE
/*
 * openflow set l3-hash-entry type sip6-dip6 <IPV6ADDR:sip> <IPV6ADDR:dip> ( <UINT:metadata> | none ) instrutcion write-action set-field <UINT:field_id> field-type ( vlan-priority | vid | dscp ) <UINT:value>
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip6_dip6_sip_dip_metadata_none_instrutcion_write_action_set_field_field_id_field_type_vlan_priority_vid_dscp_value(
    cparser_context_t *context,
    char **sip_ptr,
    char **dip_ptr,
    uint32_t *metadata_ptr,
    uint32_t *field_id_ptr,
    uint32_t *value_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;
    rtk_of_setFieldType_t type = OF_SET_FIELD_TYPE_NONE;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V6_PARSE(sip_ptr, dip_ptr, 7);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    if ('l' == TOKEN_CHAR(13, 1))
        type = OF_SET_FIELD_TYPE_VLAN_PRI;
    else if ('i' == TOKEN_CHAR(13, 1))
        type = OF_SET_FIELD_TYPE_VLAN_ID;
    else if ('s' == TOKEN_CHAR(13, 1))
        type = OF_SET_FIELD_TYPE_IP_DSCP;

    DIAG_UTIL_ERR_CHK(rtk_of_l3FlowEntrySetField_check(unit, *field_id_ptr, type), ret);

    if (0 == *field_id_ptr)
    {
        entry.ins.wa_data.set_field_0_data.field_type = type;
        entry.ins.wa_data.set_field_0_data.field_data = *value_ptr;
    }
    else if (1 == *field_id_ptr)
    {
        entry.ins.wa_data.set_field_1_data.field_type = type;
        entry.ins.wa_data.set_field_1_data.field_data = *value_ptr;
    }
    else if (2 == *field_id_ptr)
    {
        entry.ins.wa_data.set_field_2_data.field_type = type;
        entry.ins.wa_data.set_field_2_data.field_data = *value_ptr;
    }

    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip6_dip6_sip_dip_metadata_none_instrutcion_write_action_set_field_field_id_field_type_vlan_priority_vid_dscp_value */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP_DIP_IP_METADATA_NONE_INSTRUTCION_WRITE_ACTION_SET_QUEUE_QID
/*
 * openflow set l3-hash-entry type ( sip | dip ) <IPV4ADDR:ip> ( <UINT:metadata> | none ) instrutcion write-action set-queue <UINT:qid>
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_ip_metadata_none_instrutcion_write_action_set_queue_qid(
    cparser_context_t *context,
    uint32_t *ip_ptr,
    uint32_t *metadata_ptr,
    uint32_t *qid_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V4_PARSE(ip_ptr, ip_ptr, 6);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    entry.ins.wa_data.qid = *qid_ptr;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_ip_metadata_none_instrutcion_write_action_set_queue_qid */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP_DIP_SIP_DIP_METADATA_NONE_INSTRUTCION_WRITE_ACTION_SET_QUEUE_QID
/*
 * openflow set l3-hash-entry type sip-dip <IPV4ADDR:sip> <IPV4ADDR:dip> ( <UINT:metadata> | none ) instrutcion write-action set-queue <UINT:qid>
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_sip_dip_metadata_none_instrutcion_write_action_set_queue_qid(
    cparser_context_t *context,
    uint32_t *sip_ptr,
    uint32_t *dip_ptr,
    uint32_t *metadata_ptr,
    uint32_t *qid_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V4_PARSE(sip_ptr, dip_ptr, 7);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    entry.ins.wa_data.qid = *qid_ptr;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_sip_dip_metadata_none_instrutcion_write_action_set_queue_qid */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP6_DIP6_IP_METADATA_NONE_INSTRUTCION_WRITE_ACTION_SET_QUEUE_QID
/*
 * openflow set l3-hash-entry type ( sip6 | dip6 ) <IPV6ADDR:ip> ( <UINT:metadata> | none ) instrutcion write-action set-queue <UINT:qid>
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip6_dip6_ip_metadata_none_instrutcion_write_action_set_queue_qid(
    cparser_context_t *context,
    char **ip_ptr,
    uint32_t *metadata_ptr,
    uint32_t *qid_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V6_PARSE(ip_ptr, ip_ptr, 6);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    entry.ins.wa_data.qid = *qid_ptr;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip6_dip6_ip_metadata_none_instrutcion_write_action_set_queue_qid */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP6_DIP6_SIP_DIP_METADATA_NONE_INSTRUTCION_WRITE_ACTION_SET_QUEUE_QID
/*
 * openflow set l3-hash-entry type sip6-dip6 <IPV6ADDR:sip> <IPV6ADDR:dip> ( <UINT:metadata> | none ) instrutcion write-action set-queue <UINT:qid>
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip6_dip6_sip_dip_metadata_none_instrutcion_write_action_set_queue_qid(
    cparser_context_t *context,
    char **sip_ptr,
    char **dip_ptr,
    uint32_t *metadata_ptr,
    uint32_t *qid_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V6_PARSE(sip_ptr, dip_ptr, 7);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    entry.ins.wa_data.qid = *qid_ptr;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip6_dip6_sip_dip_metadata_none_instrutcion_write_action_set_queue_qid */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP_DIP_IP_METADATA_NONE_INSTRUTCION_WRITE_ACTION_GROUP_GID
/*
 * openflow set l3-hash-entry type ( sip | dip ) <IPV4ADDR:ip> ( <UINT:metadata> | none ) instrutcion write-action group <UINT:gid>
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_ip_metadata_none_instrutcion_write_action_group_gid(
    cparser_context_t *context,
    uint32_t *ip_ptr,
    uint32_t *metadata_ptr,
    uint32_t *gid_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V4_PARSE(ip_ptr, ip_ptr, 6);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    entry.ins.wa_data.gid = *gid_ptr;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_ip_metadata_none_instrutcion_write_action_group_gid */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP_DIP_SIP_DIP_METADATA_NONE_INSTRUTCION_WRITE_ACTION_GROUP_GID
/*
 * openflow set l3-hash-entry type sip-dip <IPV4ADDR:sip> <IPV4ADDR:dip> ( <UINT:metadata> | none ) instrutcion write-action group <UINT:gid>
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_sip_dip_metadata_none_instrutcion_write_action_group_gid(
    cparser_context_t *context,
    uint32_t *sip_ptr,
    uint32_t *dip_ptr,
    uint32_t *metadata_ptr,
    uint32_t *gid_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V4_PARSE(sip_ptr, dip_ptr, 7);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    entry.ins.wa_data.gid = *gid_ptr;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_sip_dip_metadata_none_instrutcion_write_action_group_gid */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP6_DIP6_IP_METADATA_NONE_INSTRUTCION_WRITE_ACTION_GROUP_GID
/*
 * openflow set l3-hash-entry type ( sip6 | dip6 ) <IPV6ADDR:ip> ( <UINT:metadata> | none ) instrutcion write-action group <UINT:gid>
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip6_dip6_ip_metadata_none_instrutcion_write_action_group_gid(
    cparser_context_t *context,
    char **ip_ptr,
    uint32_t *metadata_ptr,
    uint32_t *gid_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V6_PARSE(ip_ptr, ip_ptr, 6);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    entry.ins.wa_data.gid = *gid_ptr;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip6_dip6_ip_metadata_none_instrutcion_write_action_group_gid */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP6_DIP6_SIP_DIP_METADATA_NONE_INSTRUTCION_WRITE_ACTION_GROUP_GID
/*
 * openflow set l3-hash-entry type sip6-dip6 <IPV6ADDR:sip> <IPV6ADDR:dip> ( <UINT:metadata> | none ) instrutcion write-action group <UINT:gid>
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip6_dip6_sip_dip_metadata_none_instrutcion_write_action_group_gid(
    cparser_context_t *context,
    char **sip_ptr,
    char **dip_ptr,
    uint32_t *metadata_ptr,
    uint32_t *gid_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V6_PARSE(sip_ptr, dip_ptr, 7);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    entry.ins.wa_data.gid = *gid_ptr;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip6_dip6_sip_dip_metadata_none_instrutcion_write_action_group_gid */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP_DIP_IP_METADATA_NONE_INSTRUTCION_WRITE_ACTION_OUTPUT_NONE
/*
 * openflow set l3-hash-entry type ( sip | dip ) <IPV4ADDR:ip> ( <UINT:metadata> | none ) instrutcion write-action output none
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_ip_metadata_none_instrutcion_write_action_output_none(
    cparser_context_t *context,
    uint32_t *ip_ptr,
    uint32_t *metadata_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V4_PARSE(ip_ptr, ip_ptr, 6);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    entry.ins.wa_data.output_data.type = OF_OUTPUT_TYPE_NONE;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_ip_metadata_none_instrutcion_write_action_output_none */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP_DIP_IP_METADATA_NONE_INSTRUTCION_WRITE_ACTION_OUTPUT_PHY_PORT_PHY_PORT_SRC_PORT_FILTER_PORT_UID
/*
 * openflow set l3-hash-entry type ( sip | dip ) <IPV4ADDR:ip> ( <UINT:metadata> | none ) instrutcion write-action output ( phy-port | phy-port-src-port-filter ) <UINT:port> { <UINT:devid> }
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_ip_metadata_none_instrutcion_write_action_output_phy_port_phy_port_src_port_filter_port_devid(
    cparser_context_t *context,
    uint32_t *ip_ptr,
    uint32_t *metadata_ptr,
    uint32_t *port_ptr,
    uint32_t *devid_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V4_PARSE(ip_ptr, ip_ptr, 6);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    if (osal_strlen(TOKEN_STR(10)) <= 8)
        entry.ins.wa_data.output_data.type = OF_OUTPUT_TYPE_PHY_PORT;
    else
        entry.ins.wa_data.output_data.type = OF_OUTPUT_TYPE_PHY_PORT_EX_SRC_PORT;

    entry.ins.wa_data.output_data.port = *port_ptr;
    if (devid_ptr)
        entry.ins.wa_data.output_data.devID = *devid_ptr;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_ip_metadata_none_instrutcion_write_action_output_phy_port_phy_port_src_port_filter_port_devid */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP_DIP_SIP_DIP_METADATA_NONE_INSTRUTCION_WRITE_ACTION_OUTPUT_PHY_PORT_PHY_PORT_SRC_PORT_FILTER_PORT_UID
/*
 * openflow set l3-hash-entry type sip-dip <IPV4ADDR:sip> <IPV4ADDR:dip> ( <UINT:metadata> | none ) instrutcion write-action output ( phy-port | phy-port-src-port-filter ) <UINT:port> { <UINT:devid> }
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_sip_dip_metadata_none_instrutcion_write_action_output_phy_port_phy_port_src_port_filter_port_devid(
    cparser_context_t *context,
    uint32_t *sip_ptr,
    uint32_t *dip_ptr,
    uint32_t *metadata_ptr,
    uint32_t *port_ptr,
    uint32_t *devid_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V4_PARSE(sip_ptr, dip_ptr, 7);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    if (osal_strlen(TOKEN_STR(11)) <= 8)
        entry.ins.wa_data.output_data.type = OF_OUTPUT_TYPE_PHY_PORT;
    else
        entry.ins.wa_data.output_data.type = OF_OUTPUT_TYPE_PHY_PORT_EX_SRC_PORT;

    entry.ins.wa_data.output_data.port = *port_ptr;
    if (devid_ptr)
        entry.ins.wa_data.output_data.devID = *devid_ptr;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_sip_dip_metadata_none_instrutcion_write_action_output_phy_port_phy_port_src_port_filter_port_devid */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP6_DIP6_IP_METADATA_NONE_INSTRUTCION_WRITE_ACTION_OUTPUT_PHY_PORT_PHY_PORT_SRC_PORT_FILTER_PORT_UID
/*
 * openflow set l3-hash-entry type ( sip6 | dip6 ) <IPV6ADDR:ip> ( <UINT:metadata> | none ) instrutcion write-action output ( phy-port | phy-port-src-port-filter ) <UINT:port> { <UINT:devid> }
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip6_dip6_ip_metadata_none_instrutcion_write_action_output_phy_port_phy_port_src_port_filter_port_devid(
    cparser_context_t *context,
    char **ip_ptr,
    uint32_t *metadata_ptr,
    uint32_t *port_ptr,
    uint32_t *devid_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V6_PARSE(ip_ptr, ip_ptr, 6);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    if (osal_strlen(TOKEN_STR(10)) <= 8)
        entry.ins.wa_data.output_data.type = OF_OUTPUT_TYPE_PHY_PORT;
    else
        entry.ins.wa_data.output_data.type = OF_OUTPUT_TYPE_PHY_PORT_EX_SRC_PORT;

    entry.ins.wa_data.output_data.port = *port_ptr;
    if (devid_ptr)
        entry.ins.wa_data.output_data.devID = *devid_ptr;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_WRITEACT);


    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip6_dip6_ip_metadata_none_instrutcion_write_action_output_phy_port_phy_port_src_port_filter_port_devid */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP6_DIP6_SIP_DIP_METADATA_NONE_INSTRUTCION_WRITE_ACTION_OUTPUT_PHY_PORT_PHY_PORT_SRC_PORT_FILTER_PORT_UID
/*
 * openflow set l3-hash-entry type sip6-dip6 <IPV6ADDR:sip> <IPV6ADDR:dip> ( <UINT:metadata> | none ) instrutcion write-action output ( phy-port | phy-port-src-port-filter ) <UINT:port> { <UINT:devid> }
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip6_dip6_sip_dip_metadata_none_instrutcion_write_action_output_phy_port_phy_port_src_port_filter_port_devid(
    cparser_context_t *context,
    char **sip_ptr,
    char **dip_ptr,
    uint32_t *metadata_ptr,
    uint32_t *port_ptr,
    uint32_t *devid_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V6_PARSE(sip_ptr, dip_ptr, 7);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    if (osal_strlen(TOKEN_STR(11)) <= 8)
        entry.ins.wa_data.output_data.type = OF_OUTPUT_TYPE_PHY_PORT;
    else
        entry.ins.wa_data.output_data.type = OF_OUTPUT_TYPE_PHY_PORT_EX_SRC_PORT;

    entry.ins.wa_data.output_data.port = *port_ptr;
    if (devid_ptr)
        entry.ins.wa_data.output_data.devID = *devid_ptr;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip6_dip6_sip_dip_metadata_none_instrutcion_write_action_output_phy_port_phy_port_src_port_filter_port_devid */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP_DIP_IP_METADATA_NONE_INSTRUTCION_WRITE_ACTION_OUTPUT_TRUNK_TRUNK_SRC_PORT_FILTER_TID
/*
 * openflow set l3-hash-entry type ( sip | dip ) <IPV4ADDR:ip> ( <UINT:metadata> | none ) instrutcion write-action output ( trunk | trunk-src-port-filter ) <UINT:tid>
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_ip_metadata_none_instrutcion_write_action_output_trunk_trunk_src_port_filter_tid(
    cparser_context_t *context,
    uint32_t *ip_ptr,
    uint32_t *metadata_ptr,
    uint32_t *tid_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V4_PARSE(ip_ptr, ip_ptr, 6);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    if (osal_strlen(TOKEN_STR(10)) <= 5)
        entry.ins.wa_data.output_data.type = OF_OUTPUT_TYPE_TRK_PORT;
    else
        entry.ins.wa_data.output_data.type = OF_OUTPUT_TYPE_TRK_PORT_EX_SRC_PORT;

    entry.ins.wa_data.output_data.trunk = *tid_ptr;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_ip_metadata_none_instrutcion_write_action_output_trunk_trunk_src_port_filter_tid */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP_DIP_SIP_DIP_METADATA_NONE_INSTRUTCION_WRITE_ACTION_OUTPUT_TRUNK_TRUNK_SRC_PORT_FILTER_TID
/*
 * openflow set l3-hash-entry type sip-dip <IPV4ADDR:sip> <IPV4ADDR:dip> ( <UINT:metadata> | none ) instrutcion write-action output ( trunk | trunk-src-port-filter ) <UINT:tid>
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_sip_dip_metadata_none_instrutcion_write_action_output_trunk_trunk_src_port_filter_tid(
    cparser_context_t *context,
    uint32_t *sip_ptr,
    uint32_t *dip_ptr,
    uint32_t *metadata_ptr,
    uint32_t *tid_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V4_PARSE(sip_ptr, dip_ptr, 7);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    if (osal_strlen(TOKEN_STR(11)) <= 5)
        entry.ins.wa_data.output_data.type = OF_OUTPUT_TYPE_TRK_PORT;
    else
        entry.ins.wa_data.output_data.type = OF_OUTPUT_TYPE_TRK_PORT_EX_SRC_PORT;

    entry.ins.wa_data.output_data.trunk = *tid_ptr;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_sip_dip_metadata_none_instrutcion_write_action_output_trunk_trunk_src_port_filter_tid */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP6_DIP6_IP_METADATA_NONE_INSTRUTCION_WRITE_ACTION_OUTPUT_TRUNK_TRUNK_SRC_PORT_FILTER_TID
/*
 * openflow set l3-hash-entry type ( sip6 | dip6 ) <IPV6ADDR:ip> ( <UINT:metadata> | none ) instrutcion write-action output ( trunk | trunk-src-port-filter ) <UINT:tid>
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip6_dip6_ip_metadata_none_instrutcion_write_action_output_trunk_trunk_src_port_filter_tid(
    cparser_context_t *context,
    char **ip_ptr,
    uint32_t *metadata_ptr,
    uint32_t *tid_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V6_PARSE(ip_ptr, ip_ptr, 6);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    if (osal_strlen(TOKEN_STR(10)) <= 5)
        entry.ins.wa_data.output_data.type = OF_OUTPUT_TYPE_TRK_PORT;
    else
        entry.ins.wa_data.output_data.type = OF_OUTPUT_TYPE_TRK_PORT_EX_SRC_PORT;

    entry.ins.wa_data.output_data.trunk = *tid_ptr;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip6_dip6_ip_metadata_none_instrutcion_write_action_output_trunk_trunk_src_port_filter_tid */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP6_DIP6_SIP_DIP_METADATA_NONE_INSTRUTCION_WRITE_ACTION_OUTPUT_TRUNK_TRUNK_SRC_PORT_FILTER_TID
/*
 * openflow set l3-hash-entry type sip6-dip6 <IPV6ADDR:sip> <IPV6ADDR:dip> ( <UINT:metadata> | none ) instrutcion write-action output ( trunk | trunk-src-port-filter ) <UINT:tid>
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip6_dip6_sip_dip_metadata_none_instrutcion_write_action_output_trunk_trunk_src_port_filter_tid(
    cparser_context_t *context,
    char **sip_ptr,
    char **dip_ptr,
    uint32_t *metadata_ptr,
    uint32_t *tid_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V6_PARSE(sip_ptr, dip_ptr, 7);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    if (osal_strlen(TOKEN_STR(11)) <= 5)
        entry.ins.wa_data.output_data.type = OF_OUTPUT_TYPE_TRK_PORT;
    else
        entry.ins.wa_data.output_data.type = OF_OUTPUT_TYPE_TRK_PORT_EX_SRC_PORT;

    entry.ins.wa_data.output_data.trunk = *tid_ptr;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip6_dip6_sip_dip_metadata_none_instrutcion_write_action_output_trunk_trunk_src_port_filter_tid */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP_DIP_IP_METADATA_NONE_INSTRUTCION_WRITE_ACTION_OUTPUT_MULTI_EGRESS_PORT_PORT_ALL
/*
 * openflow set l3-hash-entry type ( sip | dip ) <IPV4ADDR:ip> ( <UINT:metadata> | none ) instrutcion write-action output multi-egress-port ( <PORT_LIST:port> | all )
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_ip_metadata_none_instrutcion_write_action_output_multi_egress_port_port_all(
    cparser_context_t *context,
    uint32_t *ip_ptr,
    uint32_t *metadata_ptr,
    char **port_ptr)
{
    uint32 unit;
    int32 ret;
    diag_portlist_t portlist;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    osal_memset(&portlist, 0, sizeof(portlist));
    L3_HASH_CMD_V4_PARSE(ip_ptr, ip_ptr, 6);
    DIAG_UTIL_EXTRACT_PORTLIST(portlist, 11);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    entry.ins.wa_data.output_data.type = OF_OUTPUT_TYPE_MULTI_EGR_PORT;
    entry.ins.wa_data.output_data.portmask = portlist.portmask;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_ip_metadata_none_instrutcion_write_action_output_multi_egress_port_port_all */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP_DIP_SIP_DIP_METADATA_NONE_INSTRUTCION_WRITE_ACTION_OUTPUT_MULTI_EGRESS_PORT_PORT_ALL
/*
 * openflow set l3-hash-entry type sip-dip <IPV4ADDR:sip> <IPV4ADDR:dip> ( <UINT:metadata> | none ) instrutcion write-action output multi-egress-port ( <PORT_LIST:port> | all )
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_sip_dip_metadata_none_instrutcion_write_action_output_multi_egress_port_port_all(
    cparser_context_t *context,
    uint32_t *sip_ptr,
    uint32_t *dip_ptr,
    uint32_t *metadata_ptr,
    char **port_ptr)
{
    uint32 unit;
    int32 ret;
    diag_portlist_t portlist;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    osal_memset(&portlist, 0, sizeof(portlist));
    L3_HASH_CMD_V4_PARSE(sip_ptr, dip_ptr, 7);
    DIAG_UTIL_EXTRACT_PORTLIST(portlist, 12);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    entry.ins.wa_data.output_data.type = OF_OUTPUT_TYPE_MULTI_EGR_PORT;
    entry.ins.wa_data.output_data.portmask = portlist.portmask;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_sip_dip_metadata_none_instrutcion_write_action_output_multi_egress_port_port_all */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP6_DIP6_IP_METADATA_NONE_INSTRUTCION_WRITE_ACTION_OUTPUT_MULTI_EGRESS_PORT_PORT_ALL
/*
 * openflow set l3-hash-entry type ( sip6 | dip6 ) <IPV6ADDR:ip> ( <UINT:metadata> | none ) instrutcion write-action output multi-egress-port ( <PORT_LIST:port> | all )
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip6_dip6_ip_metadata_none_instrutcion_write_action_output_multi_egress_port_port_all(
    cparser_context_t *context,
    char **ip_ptr,
    uint32_t *metadata_ptr,
    char **port_ptr)
{
    uint32 unit;
    int32 ret;
    diag_portlist_t portlist;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    osal_memset(&portlist, 0, sizeof(portlist));
    L3_HASH_CMD_V6_PARSE(ip_ptr, ip_ptr, 6);
    DIAG_UTIL_EXTRACT_PORTLIST(portlist, 11);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    entry.ins.wa_data.output_data.type = OF_OUTPUT_TYPE_MULTI_EGR_PORT;
    entry.ins.wa_data.output_data.portmask = portlist.portmask;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip6_dip6_ip_metadata_none_instrutcion_write_action_output_multi_egress_port_port_all */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP6_DIP6_SIP_DIP_METADATA_NONE_INSTRUTCION_WRITE_ACTION_OUTPUT_MULTI_EGRESS_PORT_PORT_ALL
/*
 * openflow set l3-hash-entry type sip6-dip6 <IPV6ADDR:sip> <IPV6ADDR:dip> ( <UINT:metadata> | none ) instrutcion write-action output multi-egress-port ( <PORT_LIST:port> | all )
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip6_dip6_sip_dip_metadata_none_instrutcion_write_action_output_multi_egress_port_port_all(
    cparser_context_t *context,
    char **sip_ptr,
    char **dip_ptr,
    uint32_t *metadata_ptr,
    char **port_ptr)
{
    uint32 unit;
    int32 ret;
    diag_portlist_t portlist;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    osal_memset(&portlist, 0, sizeof(portlist));
    L3_HASH_CMD_V6_PARSE(sip_ptr, dip_ptr, 7);
    DIAG_UTIL_EXTRACT_PORTLIST(portlist, 12);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    entry.ins.wa_data.output_data.type = OF_OUTPUT_TYPE_MULTI_EGR_PORT;
    entry.ins.wa_data.output_data.portmask = portlist.portmask;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_WRITEACT);


    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip6_dip6_sip_dip_metadata_none_instrutcion_write_action_output_multi_egress_port_port_all */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP_DIP_IP_METADATA_NONE_INSTRUTCION_WRITE_ACTION_OUTPUT_INGRESS_PORT
/*
 * openflow set l3-hash-entry type ( sip | dip ) <IPV4ADDR:ip> ( <UINT:metadata> | none ) instrutcion write-action output ingress-port
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_ip_metadata_none_instrutcion_write_action_output_ingress_port(
    cparser_context_t *context,
    uint32_t *ip_ptr,
    uint32_t *metadata_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V4_PARSE(ip_ptr, ip_ptr, 6);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    entry.ins.wa_data.output_data.type = OF_OUTPUT_TYPE_IN_PORT;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_ip_metadata_none_instrutcion_write_action_output_ingress_port */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP_DIP_SIP_DIP_METADATA_NONE_INSTRUTCION_WRITE_ACTION_OUTPUT_INGRESS_PORT
/*
 * openflow set l3-hash-entry type sip-dip <IPV4ADDR:sip> <IPV4ADDR:dip> ( <UINT:metadata> | none ) instrutcion write-action output ingress-port
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_sip_dip_metadata_none_instrutcion_write_action_output_ingress_port(
    cparser_context_t *context,
    uint32_t *sip_ptr,
    uint32_t *dip_ptr,
    uint32_t *metadata_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V4_PARSE(sip_ptr, dip_ptr, 7);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    entry.ins.wa_data.output_data.type = OF_OUTPUT_TYPE_IN_PORT;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_sip_dip_metadata_none_instrutcion_write_action_output_ingress_port */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP6_DIP6_IP_METADATA_NONE_INSTRUTCION_WRITE_ACTION_OUTPUT_INGRESS_PORT
/*
 * openflow set l3-hash-entry type ( sip6 | dip6 ) <IPV6ADDR:ip> ( <UINT:metadata> | none ) instrutcion write-action output ingress-port
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip6_dip6_ip_metadata_none_instrutcion_write_action_output_ingress_port(
    cparser_context_t *context,
    char **ip_ptr,
    uint32_t *metadata_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V6_PARSE(ip_ptr, ip_ptr, 6);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    entry.ins.wa_data.output_data.type = OF_OUTPUT_TYPE_IN_PORT;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip6_dip6_ip_metadata_none_instrutcion_write_action_output_ingress_port */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP6_DIP6_SIP_DIP_METADATA_NONE_INSTRUTCION_WRITE_ACTION_OUTPUT_INGRESS_PORT
/*
 * openflow set l3-hash-entry type sip6-dip6 <IPV6ADDR:sip> <IPV6ADDR:dip> ( <UINT:metadata> | none ) instrutcion write-action output ingress-port
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip6_dip6_sip_dip_metadata_none_instrutcion_write_action_output_ingress_port(
    cparser_context_t *context,
    char **sip_ptr,
    char **dip_ptr,
    uint32_t *metadata_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V6_PARSE(sip_ptr, dip_ptr, 7);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    entry.ins.wa_data.output_data.type = OF_OUTPUT_TYPE_IN_PORT;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip6_dip6_sip_dip_metadata_none_instrutcion_write_action_output_ingress_port */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP_DIP_IP_METADATA_NONE_INSTRUTCION_WRITE_ACTION_OUTPUT_FLOOD
/*
 * openflow set l3-hash-entry type ( sip | dip ) <IPV4ADDR:ip> ( <UINT:metadata> | none ) instrutcion write-action output flood
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_ip_metadata_none_instrutcion_write_action_output_flood(
    cparser_context_t *context,
    uint32_t *ip_ptr,
    uint32_t *metadata_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V4_PARSE(ip_ptr, ip_ptr, 6);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    entry.ins.wa_data.output_data.type = OF_OUTPUT_TYPE_FLOOD;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_ip_metadata_none_instrutcion_write_action_output_flood */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP_DIP_SIP_DIP_METADATA_NONE_INSTRUTCION_WRITE_ACTION_OUTPUT_FLOOD
/*
 * openflow set l3-hash-entry type sip-dip <IPV4ADDR:sip> <IPV4ADDR:dip> ( <UINT:metadata> | none ) instrutcion write-action output flood
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_sip_dip_metadata_none_instrutcion_write_action_output_flood(
    cparser_context_t *context,
    uint32_t *sip_ptr,
    uint32_t *dip_ptr,
    uint32_t *metadata_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V4_PARSE(sip_ptr, dip_ptr, 7);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    entry.ins.wa_data.output_data.type = OF_OUTPUT_TYPE_FLOOD;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_sip_dip_metadata_none_instrutcion_write_action_output_flood */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP6_DIP6_IP_METADATA_NONE_INSTRUTCION_WRITE_ACTION_OUTPUT_FLOOD
/*
 * openflow set l3-hash-entry type ( sip6 | dip6 ) <IPV6ADDR:ip> ( <UINT:metadata> | none ) instrutcion write-action output flood
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip6_dip6_ip_metadata_none_instrutcion_write_action_output_flood(
    cparser_context_t *context,
    char **ip_ptr,
    uint32_t *metadata_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V6_PARSE(ip_ptr, ip_ptr, 6);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    entry.ins.wa_data.output_data.type = OF_OUTPUT_TYPE_FLOOD;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip6_dip6_ip_metadata_none_instrutcion_write_action_output_flood */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP6_DIP6_SIP_DIP_METADATA_NONE_INSTRUTCION_WRITE_ACTION_OUTPUT_FLOOD
/*
 * openflow set l3-hash-entry type sip6-dip6 <IPV6ADDR:sip> <IPV6ADDR:dip> ( <UINT:metadata> | none ) instrutcion write-action output flood
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip6_dip6_sip_dip_metadata_none_instrutcion_write_action_output_flood(
    cparser_context_t *context,
    char **sip_ptr,
    char **dip_ptr,
    uint32_t *metadata_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V6_PARSE(sip_ptr, dip_ptr, 7);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    entry.ins.wa_data.output_data.type = OF_OUTPUT_TYPE_FLOOD;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip6_dip6_sip_dip_metadata_none_instrutcion_write_action_output_flood */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP_DIP_IP_METADATA_NONE_INSTRUTCION_WRITE_ACTION_OUTPUT_NORMAL
/*
 * openflow set l3-hash-entry type ( sip | dip ) <IPV4ADDR:ip> ( <UINT:metadata> | none ) instrutcion write-action output normal
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_ip_metadata_none_instrutcion_write_action_output_normal(
    cparser_context_t *context,
    uint32_t *ip_ptr,
    uint32_t *metadata_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V4_PARSE(ip_ptr, ip_ptr, 6);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    entry.ins.wa_data.output_data.type = OF_OUTPUT_TYPE_LB;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_ip_metadata_none_instrutcion_write_action_output_normal */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP_DIP_SIP_DIP_METADATA_NONE_INSTRUTCION_WRITE_ACTION_OUTPUT_NORMAL
/*
 * openflow set l3-hash-entry type sip-dip <IPV4ADDR:sip> <IPV4ADDR:dip> ( <UINT:metadata> | none ) instrutcion write-action output normal
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_sip_dip_metadata_none_instrutcion_write_action_output_normal(
    cparser_context_t *context,
    uint32_t *sip_ptr,
    uint32_t *dip_ptr,
    uint32_t *metadata_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V4_PARSE(sip_ptr, dip_ptr, 7);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    entry.ins.wa_data.output_data.type = OF_OUTPUT_TYPE_LB;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_sip_dip_metadata_none_instrutcion_write_action_output_normal */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP6_DIP6_IP_METADATA_NONE_INSTRUTCION_WRITE_ACTION_OUTPUT_NORMAL
/*
 * openflow set l3-hash-entry type ( sip6 | dip6 ) <IPV6ADDR:ip> ( <UINT:metadata> | none ) instrutcion write-action output normal
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip6_dip6_ip_metadata_none_instrutcion_write_action_output_normal(
    cparser_context_t *context,
    char **ip_ptr,
    uint32_t *metadata_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V6_PARSE(ip_ptr, ip_ptr, 6);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    entry.ins.wa_data.output_data.type = OF_OUTPUT_TYPE_LB;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip6_dip6_ip_metadata_none_instrutcion_write_action_output_normal */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP6_DIP6_SIP_DIP_METADATA_NONE_INSTRUTCION_WRITE_ACTION_OUTPUT_NORMAL
/*
 * openflow set l3-hash-entry type sip6-dip6 <IPV6ADDR:sip> <IPV6ADDR:dip> ( <UINT:metadata> | none ) instrutcion write-action output normal
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip6_dip6_sip_dip_metadata_none_instrutcion_write_action_output_normal(
    cparser_context_t *context,
    char **sip_ptr,
    char **dip_ptr,
    uint32_t *metadata_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V6_PARSE(sip_ptr, dip_ptr, 7);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    entry.ins.wa_data.output_data.type = OF_OUTPUT_TYPE_LB;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip6_dip6_sip_dip_metadata_none_instrutcion_write_action_output_normal */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP_DIP_IP_METADATA_NONE_INSTRUTCION_WRITE_ACTION_OUTPUT_FAILOVER_PORT_ALL
/*
 * openflow set l3-hash-entry type ( sip | dip ) <IPV4ADDR:ip> ( <UINT:metadata> | none ) instrutcion write-action output failover ( <PORT_LIST:port> | all )
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_ip_metadata_none_instrutcion_write_action_output_failover_port_all(
    cparser_context_t *context,
    uint32_t *ip_ptr,
    uint32_t *metadata_ptr,
    char **port_ptr)
{
    uint32 unit;
    int32 ret;
    diag_portlist_t portlist;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    osal_memset(&portlist, 0, sizeof(portlist));
    L3_HASH_CMD_V4_PARSE(ip_ptr, ip_ptr, 6);
    DIAG_UTIL_EXTRACT_PORTLIST(portlist, 11);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    entry.ins.wa_data.output_data.type = OF_OUTPUT_TYPE_FAILOVER;
    entry.ins.wa_data.output_data.portmask = portlist.portmask;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_ip_metadata_none_instrutcion_write_action_output_failover_port_all */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP_DIP_SIP_DIP_METADATA_NONE_INSTRUTCION_WRITE_ACTION_OUTPUT_FAILOVER_PORT_ALL
/*
 * openflow set l3-hash-entry type sip-dip <IPV4ADDR:sip> <IPV4ADDR:dip> ( <UINT:metadata> | none ) instrutcion write-action output failover ( <PORT_LIST:port> | all )
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_sip_dip_metadata_none_instrutcion_write_action_output_failover_port_all(
    cparser_context_t *context,
    uint32_t *sip_ptr,
    uint32_t *dip_ptr,
    uint32_t *metadata_ptr,
    char **port_ptr)
{
    uint32 unit;
    int32 ret;
    diag_portlist_t portlist;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    osal_memset(&portlist, 0, sizeof(portlist));
    L3_HASH_CMD_V4_PARSE(sip_ptr, dip_ptr, 7);
    DIAG_UTIL_EXTRACT_PORTLIST(portlist, 12);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    entry.ins.wa_data.output_data.type = OF_OUTPUT_TYPE_FAILOVER;
    entry.ins.wa_data.output_data.portmask = portlist.portmask;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_sip_dip_metadata_none_instrutcion_write_action_output_failover_port_all */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP6_DIP6_IP_METADATA_NONE_INSTRUTCION_WRITE_ACTION_OUTPUT_FAILOVER_PORT_ALL
/*
 * openflow set l3-hash-entry type ( sip6 | dip6 ) <IPV6ADDR:ip> ( <UINT:metadata> | none ) instrutcion write-action output failover ( <PORT_LIST:port> | all )
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip6_dip6_ip_metadata_none_instrutcion_write_action_output_failover_port_all(
    cparser_context_t *context,
    char **ip_ptr,
    uint32_t *metadata_ptr,
    char **port_ptr)
{
    uint32 unit;
    int32 ret;
    diag_portlist_t portlist;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    osal_memset(&portlist, 0, sizeof(portlist));
    L3_HASH_CMD_V6_PARSE(ip_ptr, ip_ptr, 6);
    DIAG_UTIL_EXTRACT_PORTLIST(portlist, 11);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    entry.ins.wa_data.output_data.type = OF_OUTPUT_TYPE_FAILOVER;
    entry.ins.wa_data.output_data.portmask = portlist.portmask;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip6_dip6_ip_metadata_none_instrutcion_write_action_output_failover_port_all */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP6_DIP6_SIP_DIP_METADATA_NONE_INSTRUTCION_WRITE_ACTION_OUTPUT_FAILOVER_PORT_ALL
/*
 * openflow set l3-hash-entry type sip6-dip6 <IPV6ADDR:sip> <IPV6ADDR:dip> ( <UINT:metadata> | none ) instrutcion write-action output failover ( <PORT_LIST:port> | all )
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip6_dip6_sip_dip_metadata_none_instrutcion_write_action_output_failover_port_all(
    cparser_context_t *context,
    char **sip_ptr,
    char **dip_ptr,
    uint32_t *metadata_ptr,
    char **port_ptr)
{
    uint32 unit;
    int32 ret;
    diag_portlist_t portlist;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    osal_memset(&portlist, 0, sizeof(portlist));
    L3_HASH_CMD_V6_PARSE(sip_ptr, dip_ptr, 7);
    DIAG_UTIL_EXTRACT_PORTLIST(portlist, 12);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    entry.ins.wa_data.output_data.type = OF_OUTPUT_TYPE_FAILOVER;
    entry.ins.wa_data.output_data.portmask = portlist.portmask;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip6_dip6_sip_dip_metadata_none_instrutcion_write_action_output_failover_port_all */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP_DIP_IP_METADATA_NONE_INSTRUTCION_WRITE_ACTION_OUTPUT_TUNNEL_IFID
/*
 * openflow set l3-hash-entry type ( sip | dip ) <IPV4ADDR:ip> ( <UINT:metadata> | none ) instrutcion write-action output tunnel <UINT:ifid>
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_ip_metadata_none_instrutcion_write_action_output_tunnel_ifid(
    cparser_context_t *context,
    uint32_t *ip_ptr,
    uint32_t *metadata_ptr,
    uint32_t *ifid_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V4_PARSE(ip_ptr, ip_ptr, 6);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    entry.ins.wa_data.output_data.type = OF_OUTPUT_TYPE_TUNNEL;
    entry.ins.wa_data.output_data.intf = *ifid_ptr;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_ip_metadata_none_instrutcion_write_action_output_tunnel_ifid */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP_DIP_SIP_DIP_METADATA_NONE_INSTRUTCION_WRITE_ACTION_OUTPUT_TUNNEL_IFID
/*
 * openflow set l3-hash-entry type sip-dip <IPV4ADDR:sip> <IPV4ADDR:dip> ( <UINT:metadata> | none ) instrutcion write-action output tunnel <UINT:ifid>
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_sip_dip_metadata_none_instrutcion_write_action_output_tunnel_ifid(
    cparser_context_t *context,
    uint32_t *sip_ptr,
    uint32_t *dip_ptr,
    uint32_t *metadata_ptr,
    uint32_t *ifid_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V4_PARSE(sip_ptr, dip_ptr, 7);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    entry.ins.wa_data.output_data.type = OF_OUTPUT_TYPE_TUNNEL;
    entry.ins.wa_data.output_data.intf = *ifid_ptr;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_sip_dip_metadata_none_instrutcion_write_action_output_tunnel_ifid */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP6_DIP6_IP_METADATA_NONE_INSTRUTCION_WRITE_ACTION_OUTPUT_TUNNEL_IFID
/*
 * openflow set l3-hash-entry type ( sip6 | dip6 ) <IPV6ADDR:ip> ( <UINT:metadata> | none ) instrutcion write-action output tunnel <UINT:ifid>
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip6_dip6_ip_metadata_none_instrutcion_write_action_output_tunnel_ifid(
    cparser_context_t *context,
    char **ip_ptr,
    uint32_t *metadata_ptr,
    uint32_t *ifid_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V6_PARSE(ip_ptr, ip_ptr, 6);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    entry.ins.wa_data.output_data.type = OF_OUTPUT_TYPE_TUNNEL;
    entry.ins.wa_data.output_data.intf = *ifid_ptr;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip6_dip6_ip_metadata_none_instrutcion_write_action_output_tunnel_ifid */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP6_DIP6_SIP_DIP_METADATA_NONE_INSTRUTCION_WRITE_ACTION_OUTPUT_TUNNEL_IFID
/*
 * openflow set l3-hash-entry type sip6-dip6 <IPV6ADDR:sip> <IPV6ADDR:dip> ( <UINT:metadata> | none ) instrutcion write-action output tunnel <UINT:ifid>
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip6_dip6_sip_dip_metadata_none_instrutcion_write_action_output_tunnel_ifid(
    cparser_context_t *context,
    char **sip_ptr,
    char **dip_ptr,
    uint32_t *metadata_ptr,
    uint32_t *ifid_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V6_PARSE(sip_ptr, dip_ptr, 7);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    entry.ins.wa_data.output_data.type = OF_OUTPUT_TYPE_TUNNEL;
    entry.ins.wa_data.output_data.intf = *ifid_ptr;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_WRITEACT);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip6_dip6_sip_dip_metadata_none_instrutcion_write_action_output_tunnel_ifid */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP_DIP_IP_METADATA_NONE_INSTRUTCION_WRITE_METADATA_DATA_MASK
/*
 * openflow set l3-hash-entry type ( sip | dip ) <IPV4ADDR:ip> ( <UINT:metadata> | none ) instrutcion write-metadata <UINT:data> <UINT:mask>
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_ip_metadata_none_instrutcion_write_metadata_data_mask(
    cparser_context_t *context,
    uint32_t *ip_ptr,
    uint32_t *metadata_ptr,
    uint32_t *data_ptr,
    uint32_t *mask_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V4_PARSE(ip_ptr, ip_ptr, 6);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    entry.ins.wm_data.data = *data_ptr;
    entry.ins.wm_data.mask = *mask_ptr;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_WRITEMETA);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_ip_metadata_none_instrutcion_write_metadata_data_mask */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP_DIP_SIP_DIP_METADATA_NONE_INSTRUTCION_WRITE_METADATA_DATA_MASK
/*
 * openflow set l3-hash-entry type sip-dip <IPV4ADDR:sip> <IPV4ADDR:dip> ( <UINT:metadata> | none ) instrutcion write-metadata <UINT:data> <UINT:mask>
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_sip_dip_metadata_none_instrutcion_write_metadata_data_mask(
    cparser_context_t *context,
    uint32_t *sip_ptr,
    uint32_t *dip_ptr,
    uint32_t *metadata_ptr,
    uint32_t *data_ptr,
    uint32_t *mask_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V4_PARSE(sip_ptr, dip_ptr, 7);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    entry.ins.wm_data.data = *data_ptr;
    entry.ins.wm_data.mask = *mask_ptr;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_WRITEMETA);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_sip_dip_metadata_none_instrutcion_write_metadata_data_mask */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP6_DIP6_IP_METADATA_NONE_INSTRUTCION_WRITE_METADATA_DATA_MASK
/*
 * openflow set l3-hash-entry type ( sip6 | dip6 ) <IPV6ADDR:ip> ( <UINT:metadata> | none ) instrutcion write-metadata <UINT:data> <UINT:mask>
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip6_dip6_ip_metadata_none_instrutcion_write_metadata_data_mask(
    cparser_context_t *context,
    char **ip_ptr,
    uint32_t *metadata_ptr,
    uint32_t *data_ptr,
    uint32_t *mask_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V6_PARSE(ip_ptr, ip_ptr, 6);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    entry.ins.wm_data.data = *data_ptr;
    entry.ins.wm_data.mask = *mask_ptr;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_WRITEMETA);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip6_dip6_ip_metadata_none_instrutcion_write_metadata_data_mask */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP6_DIP6_SIP_DIP_METADATA_NONE_INSTRUTCION_WRITE_METADATA_DATA_MASK
/*
 * openflow set l3-hash-entry type sip6-dip6 <IPV6ADDR:sip> <IPV6ADDR:dip> ( <UINT:metadata> | none ) instrutcion write-metadata <UINT:data> <UINT:mask>
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip6_dip6_sip_dip_metadata_none_instrutcion_write_metadata_data_mask(
    cparser_context_t *context,
    char **sip_ptr,
    char **dip_ptr,
    uint32_t *metadata_ptr,
    uint32_t *data_ptr,
    uint32_t *mask_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V6_PARSE(sip_ptr, dip_ptr, 7);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    entry.ins.wm_data.data = *data_ptr;
    entry.ins.wm_data.mask = *mask_ptr;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_WRITEMETA);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip6_dip6_sip_dip_metadata_none_instrutcion_write_metadata_data_mask */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP_DIP_IP_METADATA_NONE_INSTRUTCION_GOTO_TABLE_NORMAL_TID
/*
 * openflow set l3-hash-entry type ( sip | dip ) <IPV4ADDR:ip> ( <UINT:metadata> | none ) instrutcion goto-table normal <UINT:tid>
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_ip_metadata_none_instrutcion_goto_table_normal_tid(
    cparser_context_t *context,
    uint32_t *ip_ptr,
    uint32_t *metadata_ptr,
    uint32_t *tid_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V4_PARSE(ip_ptr, ip_ptr, 6);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    entry.ins.gt_data.type = OF_GOTOTBL_TYPE_NORMAL;
    entry.ins.gt_data.tbl_id = *tid_ptr;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_GOTOTBL);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_ip_metadata_none_instrutcion_goto_table_normal_tid */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP_DIP_SIP_DIP_METADATA_NONE_INSTRUTCION_GOTO_TABLE_NORMAL_TID
/*
 * openflow set l3-hash-entry type sip-dip <IPV4ADDR:sip> <IPV4ADDR:dip> ( <UINT:metadata> | none ) instrutcion goto-table normal <UINT:tid>
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_sip_dip_metadata_none_instrutcion_goto_table_normal_tid(
    cparser_context_t *context,
    uint32_t *sip_ptr,
    uint32_t *dip_ptr,
    uint32_t *metadata_ptr,
    uint32_t *tid_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V4_PARSE(sip_ptr, dip_ptr, 7);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    entry.ins.gt_data.type = OF_GOTOTBL_TYPE_NORMAL;
    entry.ins.gt_data.tbl_id = *tid_ptr;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_GOTOTBL);
    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_sip_dip_metadata_none_instrutcion_goto_table_normal_tid */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP6_DIP6_IP_METADATA_NONE_INSTRUTCION_GOTO_TABLE_NORMAL_TID
/*
 * openflow set l3-hash-entry type ( sip6 | dip6 ) <IPV6ADDR:ip> ( <UINT:metadata> | none ) instrutcion goto-table normal <UINT:tid>
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip6_dip6_ip_metadata_none_instrutcion_goto_table_normal_tid(
    cparser_context_t *context,
    char **ip_ptr,
    uint32_t *metadata_ptr,
    uint32_t *tid_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V6_PARSE(ip_ptr, ip_ptr, 6);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    entry.ins.gt_data.type = OF_GOTOTBL_TYPE_NORMAL;
    entry.ins.gt_data.tbl_id = *tid_ptr;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_GOTOTBL);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip6_dip6_ip_metadata_none_instrutcion_goto_table_normal_tid */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP6_DIP6_SIP_DIP_METADATA_NONE_INSTRUTCION_GOTO_TABLE_NORMAL_TID
/*
 * openflow set l3-hash-entry type sip6-dip6 <IPV6ADDR:sip> <IPV6ADDR:dip> ( <UINT:metadata> | none ) instrutcion goto-table normal <UINT:tid>
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip6_dip6_sip_dip_metadata_none_instrutcion_goto_table_normal_tid(
    cparser_context_t *context,
    char **sip_ptr,
    char **dip_ptr,
    uint32_t *metadata_ptr,
    uint32_t *tid_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V6_PARSE(sip_ptr, dip_ptr, 7);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    entry.ins.gt_data.type = OF_GOTOTBL_TYPE_NORMAL;
    entry.ins.gt_data.tbl_id = *tid_ptr;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_GOTOTBL);
    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip6_dip6_sip_dip_metadata_none_instrutcion_goto_table_normal_tid */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP_DIP_IP_METADATA_NONE_INSTRUTCION_GOTO_TABLE_LOOPBACK_APPLY_THEN_LOOPBACK_TID
/*
 * openflow set l3-hash-entry type ( sip | dip ) <IPV4ADDR:ip> ( <UINT:metadata> | none ) instrutcion goto-table ( loopback | apply-then-loopback ) <UINT:tid>
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_ip_metadata_none_instrutcion_goto_table_loopback_apply_then_loopback_tid(
    cparser_context_t *context,
    uint32_t *ip_ptr,
    uint32_t *metadata_ptr,
    uint32_t *tid_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V4_PARSE(ip_ptr, ip_ptr, 6);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    if ('l' == TOKEN_CHAR(9, 0))
        entry.ins.gt_data.type = OF_GOTOTBL_TYPE_LB;
    else if ('a' == TOKEN_CHAR(9, 0))
        entry.ins.gt_data.type = OF_GOTOTBL_TYPE_APPLY_AND_LB;
    entry.ins.gt_data.tbl_id = *tid_ptr;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_GOTOTBL);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_ip_metadata_none_instrutcion_goto_table_loopback_apply_then_loopback_tid */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP_DIP_SIP_DIP_METADATA_NONE_INSTRUTCION_GOTO_TABLE_LOOPBACK_APPLY_THEN_LOOPBACK_TID
/*
 * openflow set l3-hash-entry type sip-dip <IPV4ADDR:sip> <IPV4ADDR:dip> ( <UINT:metadata> | none ) instrutcion goto-table ( loopback | apply-then-loopback ) <UINT:tid>
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_sip_dip_metadata_none_instrutcion_goto_table_loopback_apply_then_loopback_tid(
    cparser_context_t *context,
    uint32_t *sip_ptr,
    uint32_t *dip_ptr,
    uint32_t *metadata_ptr,
    uint32_t *tid_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V4_PARSE(sip_ptr, dip_ptr, 7);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    if ('l' == TOKEN_CHAR(10, 0))
        entry.ins.gt_data.type = OF_GOTOTBL_TYPE_LB;
    else if ('a' == TOKEN_CHAR(10, 0))
        entry.ins.gt_data.type = OF_GOTOTBL_TYPE_APPLY_AND_LB;
    entry.ins.gt_data.tbl_id = *tid_ptr;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_GOTOTBL);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip_dip_sip_dip_metadata_none_instrutcion_goto_table_loopback_apply_then_loopback_tid */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP6_DIP6_IP_METADATA_NONE_INSTRUTCION_GOTO_TABLE_LOOPBACK_APPLY_THEN_LOOPBACK_TID
/*
 * openflow set l3-hash-entry type ( sip6 | dip6 ) <IPV6ADDR:ip> ( <UINT:metadata> | none ) instrutcion goto-table ( loopback | apply-then-loopback ) <UINT:tid>
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip6_dip6_ip_metadata_none_instrutcion_goto_table_loopback_apply_then_loopback_tid(
    cparser_context_t *context,
    char **ip_ptr,
    uint32_t *metadata_ptr,
    uint32_t *tid_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V6_PARSE(ip_ptr, ip_ptr, 6);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    if ('l' == TOKEN_CHAR(9, 0))
        entry.ins.gt_data.type = OF_GOTOTBL_TYPE_LB;
    else if ('a' == TOKEN_CHAR(9, 0))
        entry.ins.gt_data.type = OF_GOTOTBL_TYPE_APPLY_AND_LB;
    entry.ins.gt_data.tbl_id = *tid_ptr;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_GOTOTBL);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip6_dip6_ip_metadata_none_instrutcion_goto_table_loopback_apply_then_loopback_tid */
#endif

#ifdef CMD_OPENFLOW_SET_L3_HASH_ENTRY_TYPE_SIP6_DIP6_SIP_DIP_METADATA_NONE_INSTRUTCION_GOTO_TABLE_LOOPBACK_APPLY_THEN_LOOPBACK_TID
/*
 * openflow set l3-hash-entry type sip6-dip6 <IPV6ADDR:sip> <IPV6ADDR:dip> ( <UINT:metadata> | none ) instrutcion goto-table ( loopback | apply-then-loopback ) <UINT:tid>
 */
cparser_result_t
cparser_cmd_openflow_set_l3_hash_entry_type_sip6_dip6_sip_dip_metadata_none_instrutcion_goto_table_loopback_apply_then_loopback_tid(
    cparser_context_t *context,
    char **sip_ptr,
    char **dip_ptr,
    uint32_t *metadata_ptr,
    uint32_t *tid_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V6_PARSE(sip_ptr, dip_ptr, 7);
    DIAG_UTIL_ERR_CHK((RT_ERR_ENTRY_NOTFOUND == (ret = rtk_of_l3HashFlowEntry_get(unit, &entry)))? RT_ERR_OK : ret, ret);

    if ('l' == TOKEN_CHAR(10, 0))
        entry.ins.gt_data.type = OF_GOTOTBL_TYPE_LB;
    else if ('a' == TOKEN_CHAR(10, 0))
        entry.ins.gt_data.type = OF_GOTOTBL_TYPE_APPLY_AND_LB;
    entry.ins.gt_data.tbl_id = *tid_ptr;
    entry.flags |= RTK_OF_FLAG_FLOWENTRY_REPLACE;
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_add(unit, &entry), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_GOTOTBL);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_l3_hash_entry_type_sip6_dip6_sip_dip_metadata_none_instrutcion_goto_table_loopback_apply_then_loopback_tid */
#endif

#ifdef CMD_OPENFLOW_DEL_L3_HASH_ENTRY_TYPE_SIP_DIP_IP_METADATA_NONE
/*
 * openflow del l3-hash-entry type ( sip | dip ) <IPV4ADDR:ip> ( <UINT:metadata> | none )
 */
cparser_result_t
cparser_cmd_openflow_del_l3_hash_entry_type_sip_dip_ip_metadata_none(
    cparser_context_t *context,
    uint32_t *ip_ptr,
    uint32_t *metadata_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V4_PARSE(ip_ptr, ip_ptr, 6);
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_get(unit, &entry), ret);
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_del(unit, &entry), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_del_l3_hash_entry_type_sip_dip_ip_metadata_none */
#endif

#ifdef CMD_OPENFLOW_DEL_L3_HASH_ENTRY_TYPE_SIP_DIP_SIP_DIP_METADATA_NONE
/*
 * openflow del l3-hash-entry type sip-dip <IPV4ADDR:sip> <IPV4ADDR:dip> ( <UINT:metadata> | none )
 */
cparser_result_t
cparser_cmd_openflow_del_l3_hash_entry_type_sip_dip_sip_dip_metadata_none(
    cparser_context_t *context,
    uint32_t *sip_ptr,
    uint32_t *dip_ptr,
    uint32_t *metadata_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V4_PARSE(sip_ptr, dip_ptr, 7);
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_get(unit, &entry), ret);
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_del(unit, &entry), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_del_l3_hash_entry_type_sip_dip_sip_dip_metadata_none */
#endif

#ifdef CMD_OPENFLOW_DEL_L3_HASH_ENTRY_TYPE_SIP6_DIP6_IP_METADATA_NONE
/*
 * openflow del l3-hash-entry type ( sip6 | dip6 ) <IPV6ADDR:ip> ( <UINT:metadata> | none )
 */
cparser_result_t
cparser_cmd_openflow_del_l3_hash_entry_type_sip6_dip6_ip_metadata_none(
    cparser_context_t *context,
    char **ip_ptr,
    uint32_t *metadata_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V6_PARSE(ip_ptr, ip_ptr, 6);
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_get(unit, &entry), ret);
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_del(unit, &entry), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_del_l3_hash_entry_type_sip6_dip6_ip_metadata_none */
#endif

#ifdef CMD_OPENFLOW_DEL_L3_HASH_ENTRY_TYPE_SIP6_DIP6_SIP_DIP_METADATA_NONE
/*
 * openflow del l3-hash-entry type sip6-dip6 <IPV6ADDR:sip> <IPV6ADDR:dip> ( <UINT:metadata> | none )
 */
cparser_result_t
cparser_cmd_openflow_del_l3_hash_entry_type_sip6_dip6_sip_dip_metadata_none(
    cparser_context_t *context,
    char **sip_ptr,
    char **dip_ptr,
    uint32_t *metadata_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V6_PARSE(sip_ptr, dip_ptr, 7);
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_get(unit, &entry), ret);
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_del(unit, &entry), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_del_l3_hash_entry_type_sip6_dip6_sip_dip_metadata_none */
#endif

#ifdef CMD_OPENFLOW_DEL_L3_HASH_ENTRY_ALL
/*
 * openflow del l3-hash-entry all
 */
cparser_result_t
cparser_cmd_openflow_del_l3_hash_entry_all(
    cparser_context_t *context)
{
    uint32 unit;
    int32 ret;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_delAll(unit), ret);
    DIAG_UTIL_MPRINTF("Delete L3 hash entry all\n");

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_del_l3_hash_entry_all */
#endif

#ifdef CMD_OPENFLOW_GET_L3_HASH_ENTRY_TYPE_SIP_DIP_IP_METADATA_NONE_HIT_INDICATION
/*
 * openflow get l3-hash-entry type ( sip | dip ) <IPV4ADDR:ip> ( <UINT:metadata> | none ) hit-indication
 */
cparser_result_t
cparser_cmd_openflow_get_l3_hash_entry_type_sip_dip_ip_metadata_none_hit_indication(
    cparser_context_t *context,
    uint32_t *ip_ptr,
    uint32_t *metadata_ptr)
{
    uint32 unit;
    uint32 isHit = 0;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V4_PARSE(ip_ptr, ip_ptr, 6);
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashflowEntryHitSts_get(unit, &entry, FALSE, &isHit), ret);
    DIAG_UTIL_MPRINTF("The corresponding hit-indication is %u\n", isHit);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_get_l3_hash_entry_type_sip_dip_ip_metadata_none_hit_indication */
#endif

#ifdef CMD_OPENFLOW_GET_L3_HASH_ENTRY_TYPE_SIP_DIP_SIP_DIP_METADATA_NONE_HIT_INDICATION
/*
 * openflow get l3-hash-entry type sip-dip <IPV4ADDR:sip> <IPV4ADDR:dip> ( <UINT:metadata> | none ) hit-indication
 */
cparser_result_t
cparser_cmd_openflow_get_l3_hash_entry_type_sip_dip_sip_dip_metadata_none_hit_indication(
    cparser_context_t *context,
    uint32_t *sip_ptr,
    uint32_t *dip_ptr,
    uint32_t *metadata_ptr)
{
    uint32 unit;
    uint32 isHit = 0;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V4_PARSE(sip_ptr, dip_ptr, 7);
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_get(unit, &entry), ret);
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashflowEntryHitSts_get(unit, &entry, FALSE, &isHit), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_END);
    DIAG_UTIL_MPRINTF("Entry hit: %u\n", isHit);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_get_l3_hash_entry_type_sip_dip_sip_dip_metadata_none_hit_indication */
#endif

#ifdef CMD_OPENFLOW_GET_L3_HASH_ENTRY_TYPE_SIP6_DIP6_IP_METADATA_NONE_HIT_INDICATION
/*
 * openflow get l3-hash-entry type ( sip6 | dip6 ) <IPV6ADDR:ip> ( <UINT:metadata> | none ) hit-indication
 */
cparser_result_t
cparser_cmd_openflow_get_l3_hash_entry_type_sip6_dip6_ip_metadata_none_hit_indication(
    cparser_context_t *context,
    char **ip_ptr,
    uint32_t *metadata_ptr)
{
    uint32 unit;
    uint32 isHit = 0;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V6_PARSE(ip_ptr, ip_ptr, 6);
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_get(unit, &entry), ret);
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashflowEntryHitSts_get(unit, &entry, FALSE, &isHit), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_END);
    DIAG_UTIL_MPRINTF("Entry hit: %u\n", isHit);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_get_l3_hash_entry_type_sip6_dip6_ip_metadata_none_hit_indication */
#endif

#ifdef CMD_OPENFLOW_GET_L3_HASH_ENTRY_TYPE_SIP6_DIP6_SIP_DIP_METADATA_NONE_HIT_INDICATION
/*
 * openflow get l3-hash-entry type sip6-dip6 <IPV6ADDR:sip> <IPV6ADDR:dip> ( <UINT:metadata> | none ) hit-indication
 */
cparser_result_t
cparser_cmd_openflow_get_l3_hash_entry_type_sip6_dip6_sip_dip_metadata_none_hit_indication(
    cparser_context_t *context,
    char **sip_ptr,
    char **dip_ptr,
    uint32_t *metadata_ptr)
{
    uint32 unit;
    uint32 isHit = 0;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V6_PARSE(sip_ptr, dip_ptr, 7);
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_get(unit, &entry), ret);
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashflowEntryHitSts_get(unit, &entry, FALSE, &isHit), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_END);
    DIAG_UTIL_MPRINTF("Entry hit: %u\n", isHit);


    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_get_l3_hash_entry_type_sip6_dip6_sip_dip_metadata_none_hit_indication */
#endif

#ifdef CMD_OPENFLOW_RESET_L3_HASH_ENTRY_TYPE_SIP_DIP_IP_METADATA_NONE_HIT_INDICATION
/*
 * openflow reset l3-hash-entry type ( sip | dip ) <IPV4ADDR:ip> ( <UINT:metadata> | none ) hit-indication
 */
cparser_result_t
cparser_cmd_openflow_reset_l3_hash_entry_type_sip_dip_ip_metadata_none_hit_indication(
    cparser_context_t *context,
    uint32_t *ip_ptr,
    uint32_t *metadata_ptr)
{
    uint32 unit;
    uint32 isHit = 0;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V4_PARSE(ip_ptr, ip_ptr, 6);
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_get(unit, &entry), ret);
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashflowEntryHitSts_get(unit, &entry, TRUE, &isHit), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_END);
    DIAG_UTIL_MPRINTF("Entry hit: %u\n", isHit);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_reset_l3_hash_entry_type_sip_dip_ip_metadata_none_hit_indication */
#endif

#ifdef CMD_OPENFLOW_RESET_L3_HASH_ENTRY_TYPE_SIP_DIP_SIP_DIP_METADATA_NONE_HIT_INDICATION
/*
 * openflow reset l3-hash-entry type sip-dip <IPV4ADDR:sip> <IPV4ADDR:dip> ( <UINT:metadata> | none ) hit-indication
 */
cparser_result_t
cparser_cmd_openflow_reset_l3_hash_entry_type_sip_dip_sip_dip_metadata_none_hit_indication(
    cparser_context_t *context,
    uint32_t *sip_ptr,
    uint32_t *dip_ptr,
    uint32_t *metadata_ptr)
{
    uint32 unit;
    uint32 isHit = 0;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V4_PARSE(sip_ptr, dip_ptr, 7);
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_get(unit, &entry), ret);
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashflowEntryHitSts_get(unit, &entry, TRUE, &isHit), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_END);
    DIAG_UTIL_MPRINTF("Entry hit: %u\n", isHit);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_reset_l3_hash_entry_type_sip_dip_sip_dip_metadata_none_hit_indication */
#endif

#ifdef CMD_OPENFLOW_RESET_L3_HASH_ENTRY_TYPE_SIP6_DIP6_IP_METADATA_NONE_HIT_INDICATION
/*
 * openflow reset l3-hash-entry type ( sip6 | dip6 ) <IPV6ADDR:ip> ( <UINT:metadata> | none ) hit-indication
 */
cparser_result_t
cparser_cmd_openflow_reset_l3_hash_entry_type_sip6_dip6_ip_metadata_none_hit_indication(
    cparser_context_t *context,
    char **ip_ptr,
    uint32_t *metadata_ptr)
{
    uint32 unit;
    uint32 isHit = 0;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V6_PARSE(ip_ptr, ip_ptr, 6);
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_get(unit, &entry), ret);
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashflowEntryHitSts_get(unit, &entry, TRUE, &isHit), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_END);
    DIAG_UTIL_MPRINTF("Entry hit: %u\n", isHit);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_reset_l3_hash_entry_type_sip6_dip6_ip_metadata_none_hit_indication */
#endif

#ifdef CMD_OPENFLOW_RESET_L3_HASH_ENTRY_TYPE_SIP6_DIP6_SIP_DIP_METADATA_NONE_HIT_INDICATION
/*
 * openflow reset l3-hash-entry type sip6-dip6 <IPV6ADDR:sip> <IPV6ADDR:dip> ( <UINT:metadata> | none ) hit-indication
 */
cparser_result_t
cparser_cmd_openflow_reset_l3_hash_entry_type_sip6_dip6_sip_dip_metadata_none_hit_indication(
    cparser_context_t *context,
    char **sip_ptr,
    char **dip_ptr,
    uint32_t *metadata_ptr)
{
    uint32 unit;
    uint32 isHit = 0;
    int32 ret;
    rtk_of_l3HashFlowEntry_t entry;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&entry, 0, sizeof(entry));
    L3_HASH_CMD_V6_PARSE(sip_ptr, dip_ptr, 7);
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashFlowEntry_get(unit, &entry), ret);
    DIAG_UTIL_ERR_CHK(rtk_of_l3HashflowEntryHitSts_get(unit, &entry, TRUE, &isHit), ret);
    DUMP_L3CAM_HASHENTRY(entry, OF_INS_ACT_TYPE_END);
    DIAG_UTIL_MPRINTF("Entry hit: %u\n", isHit);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_reset_l3_hash_entry_type_sip6_dip6_sip_dip_metadata_none_hit_indication */
#endif

#ifdef CMD_OPENFLOW_GET_GROUP_INDEX
/*
 * openflow get group <UINT:index>
 */
cparser_result_t
cparser_cmd_openflow_get_group_index(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_groupEntry_t ge;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&ge, 0 , sizeof(ge));
    DIAG_UTIL_ERR_CHK(rtk_of_groupEntry_get(unit, *index_ptr, &ge), ret);
    DIAG_UTIL_MPRINTF("Openflow group entry %u:\n", *index_ptr);
    DIAG_UTIL_MPRINTF("\ttype: %s\n", text_group_type[ge.type]);
    DIAG_UTIL_MPRINTF("\tbucket_num: %u\n", ge.bucket_num);
    DIAG_UTIL_MPRINTF("\tbucket_id : %u\n", ge.bucket_id);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_get_group_index */
#endif

#ifdef CMD_OPENFLOW_SET_GROUP_INDEX_ALL_SELECT_BUCKET_NUM_BUCKET_ID
/*
 * openflow set group <UINT:index> ( all | select) <UINT:bucket_num> <UINT:bucket_id>
 */
cparser_result_t
cparser_cmd_openflow_set_group_index_all_select_bucket_num_bucket_id(
    cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *bucket_num_ptr,
    uint32_t *bucket_id_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_groupEntry_t ge;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&ge, 0 , sizeof(ge));

    if ('a' == TOKEN_CHAR(4, 0))
        ge.type = OF_GROUP_TYPE_ALL;
    else if ('s' == TOKEN_CHAR(4, 0))
        ge.type = OF_GROUP_TYPE_SELECT;

    ge.bucket_num= *bucket_num_ptr;
    ge.bucket_id = *bucket_id_ptr;
    DIAG_UTIL_ERR_CHK(rtk_of_groupEntry_set(unit, *index_ptr, &ge), ret);
    DIAG_UTIL_MPRINTF("Openflow group entry %u:\n", *index_ptr);
    DIAG_UTIL_MPRINTF("\ttype: %s\n", text_group_type[ge.type]);
    DIAG_UTIL_MPRINTF("\tbucket_num: %u\n", ge.bucket_num);
    DIAG_UTIL_MPRINTF("\tbucket_id : %u\n", ge.bucket_id);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_group_index_all_select_indirect_bucket_num_bucket_id */
#endif

#ifdef CMD_OPENFLOW_GET_GROUP_HASH_PARAMETER
/*
 * openflow get group hash-parameter
 */
cparser_result_t
cparser_cmd_openflow_get_group_hash_parameter(
    cparser_context_t *context)
{
    uint32 unit;
    int32 ret;
    rtk_of_groupTblHashPara_t gha;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&gha, 0 , sizeof(gha));
    DIAG_UTIL_ERR_CHK(rtk_of_groupTblHashPara_get(unit, &gha), ret);
    DIAG_UTIL_MPRINTF("Openflow group hash parameter\n");
    DIAG_UTIL_MPRINTF("\tigr_port: %s\n", text_state[gha.igr_port]);
    DIAG_UTIL_MPRINTF("\tsmac: %s\n", text_state[gha.smac]);
    DIAG_UTIL_MPRINTF("\tdmac: %s\n", text_state[gha.dmac]);
    DIAG_UTIL_MPRINTF("\tsip: %s\n", text_state[gha.sip]);
    DIAG_UTIL_MPRINTF("\tdip: %s\n", text_state[gha.dip]);
    DIAG_UTIL_MPRINTF("\tl4_sport: %s\n", text_state[gha.l4_sport]);
    DIAG_UTIL_MPRINTF("\tl4_dport: %s\n", text_state[gha.l4_dport]);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_get_group_hash_parameter */
#endif

#ifdef CMD_OPENFLOW_SET_GROUP_HASH_PARAMETER_IGR_PORT_SMAC_DMAC_SIP_DIP_L4_SPORT_L4_DPORT
/*
 * openflow set group hash-parameter { igr-port } { smac } { dmac } { sip } { dip } { l4-sport } { l4-dport }
 */
cparser_result_t
cparser_cmd_openflow_set_group_hash_parameter_igr_port_smac_dmac_sip_dip_l4_sport_l4_dport(
    cparser_context_t *context)
{
    uint32 unit;
    uint32 i = 0;
    int32 ret;
    rtk_of_groupTblHashPara_t gha;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&gha, 0 , sizeof(gha));

    for (i = 4; i < TOKEN_NUM; i++)
    {
        if ('i' == TOKEN_CHAR(i, 0))
            gha.igr_port = ENABLED;
        else if ('s' == TOKEN_CHAR(i, 0))
        {
            if ('m' == TOKEN_CHAR(i, 1))
                gha.smac = ENABLED;
            else if ('i' == TOKEN_CHAR(i, 1))
                gha.sip = ENABLED;
        }
        else if ('d' == TOKEN_CHAR(i, 0))
        {
            if ('m' == TOKEN_CHAR(i, 1))
                gha.dmac = ENABLED;
            else if ('i' == TOKEN_CHAR(i, 1))
                gha.dip = ENABLED;
        }
        else if ('l' == TOKEN_CHAR(i, 0))
        {
            if ('s' == TOKEN_CHAR(i, 3))
                gha.l4_sport = ENABLED;
            else if ('d' == TOKEN_CHAR(i, 3))
                gha.l4_dport= ENABLED;
        }
    }

    DIAG_UTIL_ERR_CHK(rtk_of_groupTblHashPara_set(unit, &gha), ret);
    DIAG_UTIL_MPRINTF("Openflow group hash parameter\n");
    DIAG_UTIL_MPRINTF("\tigr_port: %s\n", text_state[gha.igr_port]);
    DIAG_UTIL_MPRINTF("\tsmac: %s\n", text_state[gha.smac]);
    DIAG_UTIL_MPRINTF("\tdmac: %s\n", text_state[gha.dmac]);
    DIAG_UTIL_MPRINTF("\tsip: %s\n", text_state[gha.sip]);
    DIAG_UTIL_MPRINTF("\tdip: %s\n", text_state[gha.dip]);
    DIAG_UTIL_MPRINTF("\tl4_sport: %s\n", text_state[gha.l4_sport]);
    DIAG_UTIL_MPRINTF("\tl4_dport: %s\n", text_state[gha.l4_dport]);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_group_hash_parameter_igr_port_smac_dmac_sip_dip_l4_sport_l4_dport */
#endif

#ifdef CMD_OPENFLOW_GET_GROUP_ACTION_BUCKET_INDEX
/*
 * openflow get group action-bucket <UINT:index>
 */
cparser_result_t
cparser_cmd_openflow_get_group_action_bucket_index(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_actionBucket_t ab;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&ab, 0 , sizeof(ab));
    DIAG_UTIL_ERR_CHK(rtk_of_actionBucket_get(unit, *index_ptr, &ab), ret);
    DUMP_ACTION_BUCKET(ab, *index_ptr);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_get_group_action_bucket_index */
#endif

#ifdef CMD_OPENFLOW_SET_GROUP_ACTION_BUCKET_INDEX_ACTION_COPY_TTL_INWARD_COPY_TTL_OUTWARD_PUSH_VLAN_POP_VLAN_PUSH_MPLS_POP_MPLS_DEC_MPLS_TTL_DEC_IP_TTL_SET_QUEUE_STATE_DISABLE_ENABLE
/*
 * openflow set group action-bucket <UINT:index> action ( copy-ttl-inward | copy-ttl-outward | push-vlan | pop-vlan | push-mpls | pop-mpls | dec-mpls-ttl | dec-ip-ttl | set-queue ) state ( disable | enable )
 */
cparser_result_t
cparser_cmd_openflow_set_group_action_bucket_index_action_copy_ttl_inward_copy_ttl_outward_push_vlan_pop_vlan_push_mpls_pop_mpls_dec_mpls_ttl_dec_ip_ttl_set_queue_state_disable_enable(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_enable_t state = DISABLED;
    rtk_of_actionBucket_t ab;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&ab, 0 , sizeof(ab));
    DIAG_UTIL_ERR_CHK(rtk_of_actionBucket_get(unit, *index_ptr, &ab), ret);
    DIAG_UTIL_PARSE_STATE(8, state);

    if ('c' == TOKEN_CHAR(6, 0))
    {
        if ('i' == TOKEN_CHAR(6, 9))
            ab.cp_ttl_inward = state;
        else if ('o' == TOKEN_CHAR(6, 9))
            ab.cp_ttl_outward = state;
    }
    else if ('p' == TOKEN_CHAR(6, 0))
    {
        if ('o' == TOKEN_CHAR(6, 1))
        {
            if ('v' == TOKEN_CHAR(6, 4))
                ab.pop_vlan = state;
            else if ('m' == TOKEN_CHAR(6, 4))
                ab.pop_mpls = state;
        }
        else if ('u' == TOKEN_CHAR(6, 1))
        {
            if ('v' == TOKEN_CHAR(6, 5))
                ab.push_vlan = state;
            else if ('m' == TOKEN_CHAR(6, 5))
                ab.push_mpls = state;
        }
    }
    else if ('d' == TOKEN_CHAR(6, 0))
    {
        if ('m' == TOKEN_CHAR(6, 4))
            ab.dec_mpls_ttl = state;
        else if ('i' == TOKEN_CHAR(6, 4))
            ab.dec_ip_ttl = state;
    }
    else if ('s' == TOKEN_CHAR(6, 0))
        ab.set_queue = state;

    DIAG_UTIL_ERR_CHK(rtk_of_actionBucket_set(unit, *index_ptr, &ab), ret);
    DUMP_ACTION_BUCKET(ab, *index_ptr);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_group_action_bucket_index_action_copy_ttl_inward_copy_ttl_outward_push_vlan_pop_vlan_push_mpls_pop_mpls_dec_mpls_ttl_dec_ip_ttl_set_queue_state_disable_enable */
#endif

#ifdef CMD_OPENFLOW_SET_GROUP_ACTION_BUCKET_INDEX_ACTION_POP_MPLS_LABEL_NUM_OUTERMOST_DOUBLE
/*
 * openflow set group action-bucket <UINT:index> action pop-mpls label-num ( outermost | double )
 */
cparser_result_t
cparser_cmd_openflow_set_group_action_bucket_index_action_pop_mpls_label_num_outermost_double(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_actionBucket_t ab;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&ab, 0 , sizeof(ab));
    DIAG_UTIL_ERR_CHK(rtk_of_actionBucket_get(unit, *index_ptr, &ab), ret);

    if ('o' == TOKEN_CHAR(8, 0))
        ab.pop_mpls_type = OF_POP_MPLS_TYPE_OUTERMOST_LABEL;
    else if ('d' == TOKEN_CHAR(8, 0))
        ab.pop_mpls_type = OF_POP_MPLS_TYPE_DOUBLE_LABEL;

    DIAG_UTIL_ERR_CHK(rtk_of_actionBucket_set(unit, *index_ptr, &ab), ret);
    DUMP_ACTION_BUCKET(ab, *index_ptr);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_group_action_bucket_index_action_pop_mpls_label_num_outermost_double */
#endif

#ifdef CMD_OPENFLOW_SET_GROUP_ACTION_BUCKET_INDEX_ACTION_PUSH_MPLS_ETHERTYPE_0X8847_0X8848
/*
 * openflow set group action-bucket <UINT:index> action push-mpls ethertype ( 0x8847 | 0x8848 )
 */
cparser_result_t
cparser_cmd_openflow_set_group_action_bucket_index_action_push_mpls_ethertype_0x8847_0x8848(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_actionBucket_t ab;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&ab, 0 , sizeof(ab));
    DIAG_UTIL_ERR_CHK(rtk_of_actionBucket_get(unit, *index_ptr, &ab), ret);

    if ('7' == TOKEN_CHAR(8, 5))
        ab.push_mpls_data.etherType = 0;
    else if ('8' == TOKEN_CHAR(8, 5))
        ab.push_mpls_data.etherType = 1;

    DIAG_UTIL_ERR_CHK(rtk_of_actionBucket_set(unit, *index_ptr, &ab), ret);
    DUMP_ACTION_BUCKET(ab, *index_ptr);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_group_action_bucket_index_action_push_mpls_ethertype_0x8847_0x8848 */
#endif

#ifdef CMD_OPENFLOW_SET_GROUP_ACTION_BUCKET_INDEX_ACTION_PUSH_MPLS_MODE_NORMAL_ENCAP_TABLE_ENCAP_IDX
/*
 * openflow set group action-bucket <UINT:index> action push-mpls mode ( normal | encap-table ) { <UINT:encap_idx> }
 */
cparser_result_t
cparser_cmd_openflow_set_group_action_bucket_index_action_push_mpls_mode_normal_encap_table_encap_idx(
    cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *encap_idx_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_actionBucket_t ab;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&ab, 0 , sizeof(ab));
    DIAG_UTIL_ERR_CHK(rtk_of_actionBucket_get(unit, *index_ptr, &ab), ret);

    if ('n' == TOKEN_CHAR(8, 0))
        ab.push_mpls_data.push_mode = OF_PUSH_MPLS_MODE_NORMAL;
    else if ('e' == TOKEN_CHAR(8, 0))
        ab.push_mpls_data.push_mode = OF_PUSH_MPLS_MODE_ENCAPTBL;

    if (NULL != encap_idx_ptr)
        ab.push_mpls_data.mpls_encap_idx = *encap_idx_ptr;
    DIAG_UTIL_ERR_CHK(rtk_of_actionBucket_set(unit, *index_ptr, &ab), ret);
    DUMP_ACTION_BUCKET(ab, *index_ptr);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_group_action_bucket_index_action_push_mpls_mode_normal_encap_table_encap_idx */
#endif

#ifdef CMD_OPENFLOW_SET_GROUP_ACTION_BUCKET_INDEX_ACTION_PUSH_MPLS_VPN_L2_L3
/*
 * openflow set group action-bucket <UINT:index> action push-mpls vpn ( l2 | l3 )
 */
cparser_result_t
cparser_cmd_openflow_set_group_action_bucket_index_action_push_mpls_vpn_l2_l3(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_actionBucket_t ab;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&ab, 0 , sizeof(ab));
    DIAG_UTIL_ERR_CHK(rtk_of_actionBucket_get(unit, *index_ptr, &ab), ret);

    if ('2' == TOKEN_CHAR(8, 1))
        ab.push_mpls_data.vpn_type = OF_PUSH_MPLS_VPN_TYPE_L2;
    else if ('3' == TOKEN_CHAR(8, 1))
        ab.push_mpls_data.vpn_type = OF_PUSH_MPLS_VPN_TYPE_L3;

    DIAG_UTIL_ERR_CHK(rtk_of_actionBucket_set(unit, *index_ptr, &ab), ret);
    DUMP_ACTION_BUCKET(ab, *index_ptr);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_group_action_bucket_index_action_push_mpls_vpn_l2_l3 */
#endif

#ifdef CMD_OPENFLOW_SET_GROUP_ACTION_BUCKET_INDEX_ACTION_PUSH_VLAN_ETHERTYPE_TPID_IDX
/*
 * openflow set group action-bucket <UINT:index> action push-vlan ethertype <UINT:tpid_idx>
 */
cparser_result_t
cparser_cmd_openflow_set_group_action_bucket_index_action_push_vlan_ethertype_tpid_idx(
    cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *tpid_idx_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_actionBucket_t ab;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&ab, 0 , sizeof(ab));
    DIAG_UTIL_ERR_CHK(rtk_of_actionBucket_get(unit, *index_ptr, &ab), ret);
    ab.push_vlan_data.etherType_idx = *tpid_idx_ptr;
    DIAG_UTIL_ERR_CHK(rtk_of_actionBucket_set(unit, *index_ptr, &ab), ret);
    DUMP_ACTION_BUCKET(ab, *index_ptr);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_group_action_bucket_index_action_push_vlan_ethertype_tpid_idx */
#endif

#ifdef CMD_OPENFLOW_SET_GROUP_ACTION_BUCKET_INDEX_ACTION_SET_FIELD_FIELD_ID_FIELD_TYPE_NONE
/*
 * openflow set group action-bucket <UINT:index> action set-field <UINT:field_id> field-type none
 */
cparser_result_t
cparser_cmd_openflow_set_group_action_bucket_index_action_set_field_field_id_field_type_none(
    cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *field_id_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_actionBucket_t ab;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&ab, 0 , sizeof(ab));
    DIAG_UTIL_ERR_CHK(rtk_of_actionBucket_get(unit, *index_ptr, &ab), ret);

    if (0 == *field_id_ptr)
        ab.set_field_0_data.field_type = OF_IGR_FT0_SF_TYPE0_NONE;
    else if (1 == *field_id_ptr)
        ab.set_field_1_data.field_type = OF_IGR_FT0_SF_TYPE1_NONE;
    else if (2 == *field_id_ptr)
        ab.set_field_2_data.field_type = OF_IGR_FT0_SF_TYPE2_NONE;
    else
        DIAG_UTIL_ERR_CHK(RT_ERR_INPUT, ret);

    DIAG_UTIL_ERR_CHK(rtk_of_actionBucket_set(unit, *index_ptr, &ab), ret);
    DUMP_ACTION_BUCKET(ab, *index_ptr);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_group_action_bucket_index_action_set_field_field_id_field_type_none */
#endif

#ifdef CMD_OPENFLOW_SET_GROUP_ACTION_BUCKET_INDEX_ACTION_SET_FIELD_FIELD_ID_FIELD_TYPE_SMAC_DMAC_MAC
/*
 * openflow set group action-bucket <UINT:index> action set-field <UINT:field_id> field-type ( smac | dmac ) <MACADDR:mac>
 */
cparser_result_t
cparser_cmd_openflow_set_group_action_bucket_index_action_set_field_field_id_field_type_smac_dmac_mac(
    cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *field_id_ptr,
    cparser_macaddr_t *mac_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_actionBucket_t ab;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&ab, 0 , sizeof(ab));
    DIAG_UTIL_ERR_CHK(rtk_of_actionBucket_get(unit, *index_ptr, &ab), ret);

    if (0 == *field_id_ptr)
    {
        ab.set_field_0_data.field_type = OF_IGR_FT0_SF_TYPE0_SA;
        if ('s' == TOKEN_CHAR(9, 0))
            osal_memcpy(ab.set_field_0_data.mac.octet, mac_ptr, ETHER_ADDR_LEN);
        else
            DIAG_UTIL_ERR_CHK(RT_ERR_INPUT, ret);
    }
    else if (1 == *field_id_ptr)
    {
        ab.set_field_1_data.field_type = OF_IGR_FT0_SF_TYPE1_DA;
        if ('s' == TOKEN_CHAR(9, 0))
            DIAG_UTIL_ERR_CHK(RT_ERR_INPUT, ret);
        else
            osal_memcpy(ab.set_field_1_data.mac.octet, mac_ptr, ETHER_ADDR_LEN);
    }
    else
        DIAG_UTIL_ERR_CHK(RT_ERR_INPUT, ret);

    DIAG_UTIL_ERR_CHK(rtk_of_actionBucket_set(unit, *index_ptr, &ab), ret);
    DUMP_ACTION_BUCKET(ab, *index_ptr);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_group_action_bucket_index_action_set_field_field_id_field_type_smac_dmac_mac */
#endif

#ifdef CMD_OPENFLOW_SET_GROUP_ACTION_BUCKET_INDEX_ACTION_SET_FIELD_FIELD_ID_FIELD_TYPE_VLAN_PRIORITY_VID_DSCP_IP_TTL_VALUE
/*
 * openflow set group action-bucket <UINT:index> action set-field <UINT:field_id> field-type ( vlan-priority | vid | dscp | ip-ttl ) <UINT:value>
 */
cparser_result_t
cparser_cmd_openflow_set_group_action_bucket_index_action_set_field_field_id_field_type_vlan_priority_vid_dscp_ip_ttl_value(
    cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *field_id_ptr,
    uint32_t *value_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_actionBucket_t ab;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&ab, 0 , sizeof(ab));
    DIAG_UTIL_ERR_CHK(rtk_of_actionBucket_get(unit, *index_ptr, &ab), ret);

    if (0 == *field_id_ptr)
    {
        ab.set_field_0_data.field_data = *value_ptr;
        if ('l' == TOKEN_CHAR(9, 1))
            ab.set_field_0_data.field_type = OF_IGR_FT0_SF_TYPE0_VLAN_PRI;
        else if ('i' == TOKEN_CHAR(9, 1))
            DIAG_UTIL_ERR_CHK(RT_ERR_INPUT, ret);
        else if ('s' == TOKEN_CHAR(9, 1))
            ab.set_field_0_data.field_type = OF_IGR_FT0_SF_TYPE0_IP_DSCP;
        else if ('p' == TOKEN_CHAR(9, 1))
            ab.set_field_0_data.field_type = OF_IGR_FT0_SF_TYPE0_IP_TTL;
    }
    else if (1 == *field_id_ptr)
    {
        ab.set_field_1_data.field_data = *value_ptr;
        if ('l' == TOKEN_CHAR(9, 1))
            ab.set_field_1_data.field_type = OF_IGR_FT0_SF_TYPE1_VLAN_PRI;
        else if ('i' == TOKEN_CHAR(9, 1))
            DIAG_UTIL_ERR_CHK(RT_ERR_INPUT, ret);
        else if ('s' == TOKEN_CHAR(9, 1))
            ab.set_field_1_data.field_type = OF_IGR_FT0_SF_TYPE1_IP_DSCP;
        else if ('p' == TOKEN_CHAR(9, 1))
            ab.set_field_1_data.field_type = OF_IGR_FT0_SF_TYPE1_IP_TTL;;
    }
    else if (2 == *field_id_ptr)
    {
        ab.set_field_2_data.field_data = *value_ptr;
        if ('l' == TOKEN_CHAR(9, 1))
            ab.set_field_2_data.field_type = OF_IGR_FT0_SF_TYPE2_VLAN_PRI;
        else if ('i' == TOKEN_CHAR(9, 1))
            ab.set_field_2_data.field_type = OF_IGR_FT0_SF_TYPE2_VLAN_ID;
        else if ('s' == TOKEN_CHAR(9, 1))
            ab.set_field_2_data.field_type = OF_IGR_FT0_SF_TYPE2_IP_DSCP;
        else if ('p' == TOKEN_CHAR(9, 1))
            ab.set_field_2_data.field_type = OF_IGR_FT0_SF_TYPE2_IP_TTL;;
    }
    else
        DIAG_UTIL_ERR_CHK(RT_ERR_INPUT, ret);

    DIAG_UTIL_ERR_CHK(rtk_of_actionBucket_set(unit, *index_ptr, &ab), ret);
    DUMP_ACTION_BUCKET(ab, *index_ptr);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_group_action_bucket_index_action_set_field_field_id_field_type_vlan_priority_vid_dscp_ip_ttl_value */
#endif

#ifdef CMD_OPENFLOW_SET_GROUP_ACTION_BUCKET_INDEX_ACTION_SET_FIELD_FIELD_ID_FIELD_TYPE_MPLS_LABEL_MPLS_TC_MPLS_TTL_VALUE
/*
 * openflow set group action-bucket <UINT:index> action set-field <UINT:field_id> field-type ( mpls-label | mpls-tc | mpls-ttl ) <UINT:value>
 */
cparser_result_t
cparser_cmd_openflow_set_group_action_bucket_index_action_set_field_field_id_field_type_mpls_label_mpls_tc_mpls_ttl_value(
    cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *field_id_ptr,
    uint32_t *value_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_actionBucket_t ab;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&ab, 0 , sizeof(ab));
    DIAG_UTIL_ERR_CHK(rtk_of_actionBucket_get(unit, *index_ptr, &ab), ret);

    if (0 == *field_id_ptr)
    {
        ab.set_field_0_data.field_data = *value_ptr;
        if ('a' == TOKEN_CHAR(9, 6))
            DIAG_UTIL_ERR_CHK(RT_ERR_INPUT, ret);
        else if ('c' == TOKEN_CHAR(9, 6))
            ab.set_field_0_data.field_type = OF_IGR_FT0_SF_TYPE0_MPLS_TC;
        else if ('t' == TOKEN_CHAR(9, 6))
            ab.set_field_0_data.field_type = OF_IGR_FT0_SF_TYPE0_MPLS_TTL;

    }
    else if (1 == *field_id_ptr)
    {
        ab.set_field_1_data.field_data = *value_ptr;
        if ('a' == TOKEN_CHAR(9, 6))
            ab.set_field_1_data.field_type = OF_IGR_FT0_SF_TYPE1_MPLS_LABEL;
        else if ('c' == TOKEN_CHAR(9, 6))
            ab.set_field_1_data.field_type = OF_IGR_FT0_SF_TYPE1_MPLS_TC;
        else if ('t' == TOKEN_CHAR(9, 6))
            ab.set_field_1_data.field_type = OF_IGR_FT0_SF_TYPE1_MPLS_TTL;
    }
    else if (2 == *field_id_ptr)
    {
        ab.set_field_2_data.field_data = *value_ptr;
        if ('a' == TOKEN_CHAR(9, 6))
            ab.set_field_2_data.field_type = OF_IGR_FT0_SF_TYPE2_MPLS_LABEL;
        else if ('c' == TOKEN_CHAR(9, 6))
            ab.set_field_2_data.field_type = OF_IGR_FT0_SF_TYPE2_MPLS_TC;
        else if ('t' == TOKEN_CHAR(9, 6))
            ab.set_field_2_data.field_type = OF_IGR_FT0_SF_TYPE2_MPLS_TTL;
    }
    else
        DIAG_UTIL_ERR_CHK(RT_ERR_INPUT, ret);

    DIAG_UTIL_ERR_CHK(rtk_of_actionBucket_set(unit, *index_ptr, &ab), ret);
    DUMP_ACTION_BUCKET(ab, *index_ptr);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_group_action_bucket_index_action_set_field_field_id_field_type_mpls_label_mpls_tc_mpls_ttl_value */
#endif

#ifdef CMD_OPENFLOW_SET_GROUP_ACTION_BUCKET_INDEX_ACTION_SET_FIELD_FIELD_ID_FIELD_TYPE_IP_FLAG_RSVD
/*
 * openflow set group action-bucket <UINT:index> action set-field <UINT:field_id> field-type ip-flag-rsvd
 */
cparser_result_t
cparser_cmd_openflow_set_group_action_bucket_index_action_set_field_field_id_field_type_ip_flag_rsvd(
    cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *field_id_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_actionBucket_t ab;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&ab, 0 , sizeof(ab));
    DIAG_UTIL_ERR_CHK(rtk_of_actionBucket_get(unit, *index_ptr, &ab), ret);

    if (0 == *field_id_ptr)
    {
        ab.set_field_0_data.field_data = 0;
        ab.set_field_0_data.field_type = OF_IGR_FT0_SF_TYPE0_IP_FLAG_RSVD;
    }
    else
        DIAG_UTIL_ERR_CHK(RT_ERR_INPUT, ret);

    DIAG_UTIL_ERR_CHK(rtk_of_actionBucket_set(unit, *index_ptr, &ab), ret);
    DUMP_ACTION_BUCKET(ab, *index_ptr);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_group_action_bucket_index_action_set_field_field_id_field_ip_flag_rsvd */
#endif

#ifdef CMD_OPENFLOW_SET_GROUP_ACTION_BUCKET_INDEX_ACTION_SET_QUEUE_QID
/*
 * openflow set group action-bucket <UINT:index> action set-queue <UINT:qid>
 */
cparser_result_t
cparser_cmd_openflow_set_group_action_bucket_index_action_set_queue_qid(
    cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *qid_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_actionBucket_t ab;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&ab, 0 , sizeof(ab));
    DIAG_UTIL_ERR_CHK(rtk_of_actionBucket_get(unit, *index_ptr, &ab), ret);
    ab.qid = *qid_ptr;
    DIAG_UTIL_ERR_CHK(rtk_of_actionBucket_set(unit, *index_ptr, &ab), ret);
    DUMP_ACTION_BUCKET(ab, *index_ptr);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_group_action_bucket_index_action_set_queue_qid */
#endif

#ifdef CMD_OPENFLOW_SET_GROUP_ACTION_BUCKET_INDEX_ACTION_OUTPUT_NONE
/*
 * openflow set group action-bucket <UINT:index> action output none
 */
cparser_result_t
cparser_cmd_openflow_set_group_action_bucket_index_action_output_none(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_actionBucket_t ab;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&ab, 0 , sizeof(ab));
    DIAG_UTIL_ERR_CHK(rtk_of_actionBucket_get(unit, *index_ptr, &ab), ret);
    ab.output_data.type = OF_OUTPUT_TYPE_NONE;
    DIAG_UTIL_ERR_CHK(rtk_of_actionBucket_set(unit, *index_ptr, &ab), ret);
    DUMP_ACTION_BUCKET(ab, *index_ptr);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_group_action_bucket_index_action_output_none */
#endif


#ifdef CMD_OPENFLOW_SET_GROUP_ACTION_BUCKET_INDEX_ACTION_OUTPUT_PHY_PORT_PHY_PORT_SRC_PORT_FILTER_PORT_UID
/*
 * openflow set group action-bucket <UINT:index> action output ( phy-port | phy-port-src-port-filter ) <UINT:port> { <UINT:devid> }
 */
cparser_result_t
cparser_cmd_openflow_set_group_action_bucket_index_action_output_phy_port_phy_port_src_port_filter_port_devid(
    cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *port_ptr,
    uint32_t *devid_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_actionBucket_t ab;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&ab, 0 , sizeof(ab));
    DIAG_UTIL_ERR_CHK(rtk_of_actionBucket_get(unit, *index_ptr, &ab), ret);

    if (osal_strlen(TOKEN_STR(7)) < 9)
        ab.output_data.type = OF_OUTPUT_TYPE_PHY_PORT;
    else
        ab.output_data.type = OF_OUTPUT_TYPE_PHY_PORT_EX_SRC_PORT;

    ab.output_data.port = *port_ptr;
    if (devid_ptr)
        ab.output_data.devID = *devid_ptr;
    DIAG_UTIL_ERR_CHK(rtk_of_actionBucket_set(unit, *index_ptr, &ab), ret);
    DUMP_ACTION_BUCKET(ab, *index_ptr);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_group_action_bucket_index_action_output_phy_port_phy_port_src_port_filter_port_devid */
#endif

#ifdef CMD_OPENFLOW_SET_GROUP_ACTION_BUCKET_INDEX_ACTION_OUTPUT_TRUNK_TRUNK_SRC_PORT_FILTER_TID
/*
 * openflow set group action-bucket <UINT:index> action output ( trunk | trunk-src-port-filter ) <UINT:tid>
 */
cparser_result_t
cparser_cmd_openflow_set_group_action_bucket_index_action_output_trunk_trunk_src_port_filter_tid(
    cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *tid_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_actionBucket_t ab;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&ab, 0 , sizeof(ab));
    DIAG_UTIL_ERR_CHK(rtk_of_actionBucket_get(unit, *index_ptr, &ab), ret);

    if (osal_strlen(TOKEN_STR(7)) < 6)
        ab.output_data.type = OF_OUTPUT_TYPE_TRK_PORT;
    else
        ab.output_data.type = OF_OUTPUT_TYPE_TRK_PORT_EX_SRC_PORT;

    ab.output_data.trunk = *tid_ptr;
    DIAG_UTIL_ERR_CHK(rtk_of_actionBucket_set(unit, *index_ptr, &ab), ret);
    DUMP_ACTION_BUCKET(ab, *index_ptr);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_group_action_bucket_index_action_output_trunk_trunk_src_port_filter_tid */
#endif

#ifdef CMD_OPENFLOW_SET_GROUP_ACTION_BUCKET_INDEX_ACTION_OUTPUT_MULTI_EGRESS_PORT_PORT_ALL
/*
 * openflow set group action-bucket <UINT:index> action output multi-egress-port ( <PORT_LIST:port> | all )
 */
cparser_result_t
cparser_cmd_openflow_set_group_action_bucket_index_action_output_multi_egress_port_port_all(
    cparser_context_t *context,
    uint32_t *index_ptr,
    char **port_ptr)
{
    uint32 unit;
    int32 ret;
    diag_portlist_t portlist;
    rtk_of_actionBucket_t ab;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&ab, 0 , sizeof(ab));
    osal_memset(&portlist, 0, sizeof(portlist));
    DIAG_UTIL_EXTRACT_PORTLIST(portlist, 8);
    DIAG_UTIL_ERR_CHK(rtk_of_actionBucket_get(unit, *index_ptr, &ab), ret);
    ab.output_data.type = OF_OUTPUT_TYPE_MULTI_EGR_PORT;
    ab.output_data.portmask = portlist.portmask;
    DIAG_UTIL_ERR_CHK(rtk_of_actionBucket_set(unit, *index_ptr, &ab), ret);
    DUMP_ACTION_BUCKET(ab, *index_ptr);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_group_action_bucket_index_action_output_multi_egress_port_port_all */
#endif

#ifdef CMD_OPENFLOW_SET_GROUP_ACTION_BUCKET_INDEX_ACTION_OUTPUT_FAILOVER_PORT_ALL
/*
 * openflow set group action-bucket <UINT:index> action output failover ( <PORT_LIST:port> | all )
 */
cparser_result_t
cparser_cmd_openflow_set_group_action_bucket_index_action_output_failover_port_all(
    cparser_context_t *context,
    uint32_t *index_ptr,
    char **port_ptr)
{
    uint32 unit;
    int32 ret;
    diag_portlist_t portlist;
    rtk_of_actionBucket_t ab;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&ab, 0 , sizeof(ab));
    osal_memset(&portlist, 0, sizeof(portlist));
    DIAG_UTIL_EXTRACT_PORTLIST(portlist, 8);
    DIAG_UTIL_ERR_CHK(rtk_of_actionBucket_get(unit, *index_ptr, &ab), ret);
    ab.output_data.type = OF_OUTPUT_TYPE_FAILOVER;
    ab.output_data.portmask = portlist.portmask;
    DIAG_UTIL_ERR_CHK(rtk_of_actionBucket_set(unit, *index_ptr, &ab), ret);
    DUMP_ACTION_BUCKET(ab, *index_ptr);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_group_action_bucket_index_action_output_failover_port_all */
#endif

#ifdef CMD_OPENFLOW_SET_GROUP_ACTION_BUCKET_INDEX_ACTION_OUTPUT_TUNNEL_IFID
/*
 * openflow set group action-bucket <UINT:index> action output tunnel <UINT:ifid>
 */
cparser_result_t
cparser_cmd_openflow_set_group_action_bucket_index_action_output_tunnel_ifid(
    cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *ifid_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_actionBucket_t ab;

    DIAG_UTIL_FUNC_INIT(unit);
    osal_memset(&ab, 0 , sizeof(ab));
    DIAG_UTIL_ERR_CHK(rtk_of_actionBucket_get(unit, *index_ptr, &ab), ret);
    ab.output_data.type = OF_OUTPUT_TYPE_TUNNEL;
    ab.output_data.intf = *ifid_ptr;
    DIAG_UTIL_ERR_CHK(rtk_of_actionBucket_set(unit, *index_ptr, &ab), ret);
    DUMP_ACTION_BUCKET(ab, *index_ptr);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_group_action_bucket_index_action_output_tunnel_ifid */
#endif

#ifdef CMD_OPENFLOW_GET_TRAP_TARGET
/*
 * openflow get trap-target
 */
cparser_result_t
cparser_cmd_openflow_get_trap_target(
    cparser_context_t *context)
{
    uint32 unit;
    int32 ret;
    rtk_trapTarget_t target = RTK_TRAP_END;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_UTIL_ERR_CHK(rtk_of_trapTarget_get(unit, &target), ret);
    DIAG_UTIL_MPRINTF("Trap target is %s\n", text_trap_target[target]);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_get_trap_target */
#endif

#ifdef CMD_OPENFLOW_SET_TRAP_TARGET_LOCAL_MASTER
/*
 * openflow set trap-target ( local | master )
 */
cparser_result_t
cparser_cmd_openflow_set_trap_target_local_master(
    cparser_context_t *context)
{
    uint32 unit;
    int32 ret;
    rtk_trapTarget_t target = RTK_TRAP_END;

    DIAG_UTIL_FUNC_INIT(unit);

    if ('l' == TOKEN_CHAR(3, 0))
        target = RTK_TRAP_LOCAL;
    else if ('m' == TOKEN_CHAR(3, 0))
        target = RTK_TRAP_MASTER;

    DIAG_UTIL_ERR_CHK(rtk_of_trapTarget_set(unit, target), ret);
    DIAG_UTIL_MPRINTF("Trap target set to %s\n", text_trap_target[target]);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_trap_target_local_master */
#endif

#ifdef CMD_OPENFLOW_GET_FLOWTABLE_PHASE_TABLE_MISS
/*
 * openflow get flowtable <UINT:phase> table-miss
 */
cparser_result_t
cparser_cmd_openflow_get_flowtable_phase_table_miss(
    cparser_context_t *context,
    uint32_t *phase_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_tblMissAct_t act = OF_TBLMISS_ACT_END;
    rtk_of_flowtable_phase_t phase = FT_PHASE_END;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_OF_PHASE_MAPPING(*phase_ptr, phase);
    DIAG_UTIL_ERR_CHK(rtk_of_tblMissAction_get(unit, phase, &act), ret);
    DIAG_UTIL_MPRINTF("phase %d flowtable action is %s\n", *phase_ptr, text_table_miss_act[act]);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_get_flowtable_phase_table_miss */
#endif

#ifdef CMD_OPENFLOW_SET_FLOWTABLE_PHASE_TABLE_MISS_DROP_TRAP_NEXT_TABLE
/*
 * openflow set flowtable <UINT:phase> table-miss ( drop | trap | next-table )
 */
cparser_result_t
cparser_cmd_openflow_set_flowtable_phase_table_miss_drop_trap_next_table(
    cparser_context_t *context,
    uint32_t *phase_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_tblMissAct_t act = OF_TBLMISS_ACT_END;
    rtk_of_flowtable_phase_t phase = FT_PHASE_END;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_OF_PHASE_MAPPING(*phase_ptr, phase);

    if ('d' == TOKEN_CHAR(5, 0))
        act = OF_TBLMISS_ACT_DROP;
    else if ('t' == TOKEN_CHAR(5, 0))
        act = OF_TBLMISS_ACT_TRAP;
    else if ('n' == TOKEN_CHAR(5, 0))
        act = OF_TBLMISS_ACT_FORWARD_NEXT_TBL;

    DIAG_UTIL_ERR_CHK(rtk_of_tblMissAction_set(unit, phase, act), ret);
    DIAG_UTIL_MPRINTF("phase %d flowtable miss action set to %s\n", *phase_ptr, text_table_miss_act[act]);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_flowtable_phase_table_miss_drop_trap_next_table */
#endif

#ifdef CMD_OPENFLOW_SET_FLOWTABLE_PHASE_TABLE_MISS_EXECUTE
/*
 * openflow set flowtable <UINT:phase> table-miss execute
 */
cparser_result_t
cparser_cmd_openflow_set_flowtable_phase_table_miss_execute(
    cparser_context_t *context,
    uint32_t *phase_ptr)
{
    uint32 unit;
    int32 ret;
    rtk_of_flowtable_phase_t phase = FT_PHASE_END;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_OF_PHASE_MAPPING(*phase_ptr, phase);

    DIAG_UTIL_ERR_CHK(rtk_of_tblMissAction_set(unit, phase, OF_TBLMISS_ACT_EXEC_ACTION_SET), ret);
    DIAG_UTIL_MPRINTF("phase %d flowtable miss action set to %s\n", *phase_ptr, text_table_miss_act[OF_TBLMISS_ACT_EXEC_ACTION_SET]);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_set_flowtable_phase_table_miss_execute */
#endif

#ifdef CMD_OPENFLOW_GET_FLOWTABLE_PHASE_COUNTER_LOOPKUP_MATCH
/*
 * openflow get flowtable <UINT:phase> counter ( loopkup | match )
 */
cparser_result_t
cparser_cmd_openflow_get_flowtable_phase_counter_loopkup_match(
    cparser_context_t *context,
    uint32_t *phase_ptr)
{
    uint32 unit;
    uint32 cnt = 0;
    int32 ret;
    rtk_of_flowTblCntType_t type = OF_FLOW_TBL_CNT_TYPE_END;
    rtk_of_flowtable_phase_t phase = FT_PHASE_END;

    DIAG_UTIL_FUNC_INIT(unit);
    DIAG_OF_PHASE_MAPPING(*phase_ptr, phase);

    if ('l' == TOKEN_CHAR(5, 0))
        type = OF_FLOW_TBL_CNT_TYPE_LOOKUP;
    else if ('m' == TOKEN_CHAR(5, 0))
        type = OF_FLOW_TBL_CNT_TYPE_MATCH;

    DIAG_UTIL_ERR_CHK(rtk_of_flowTblCnt_get(unit, phase, type, &cnt), ret);
    DIAG_UTIL_MPRINTF("phase %d %s counter: %d\n", *phase_ptr, TOKEN_STR(5), cnt);

    return CPARSER_OK;
}   /* end of cparser_cmd_openflow_get_flowtable_phase_counter_loopkup_match */
#endif

