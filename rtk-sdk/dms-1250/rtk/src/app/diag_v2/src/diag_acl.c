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
 * Purpose : Definition those ACL command and APIs in the SDK diagnostic shell.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) acl command
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
#include <osal/lib.h>
#include <osal/memory.h>
#include <soc/type.h>
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
  #include <rtrpc/rtrpc_debug.h>
#endif

/*
 * Symbol Definition
 */
typedef struct diag_acl_field_s
{
    rtk_acl_fieldType_t type;
    char                user_info[32];
    char                desc[104];
} diag_acl_field_t;

/*
 * Data Declaration
 */

#ifdef CMD_ACL_INIT_ENTRY_BUFFER
static uint8 *diag_acl_entry_buffer = NULL;
#endif

diag_acl_field_t diag_acl_field_list[] =
{
    {USER_FIELD_TEMPLATE_ID,                "template-id",          "template ID the entry maps to "},
    {USER_FIELD_FRAME_TYPE,                 "frame-type",           "frame type (0b00: ARP, 0b01: L2 only, 0b10: IPv4, 0b11: IPv6)"},
    {USER_FIELD_SPM,                        "spm",                  "source port mask"},
    {USER_FIELD_DMAC,                       "dmac",                 "destination MAC address"},
    {USER_FIELD_SMAC,                       "smac",                 "source MAC address"},
    {USER_FIELD_ITAG_EXIST,                 "itag-exist",           "packet with inner tag"},
    {USER_FIELD_OTAG_EXIST,                 "otag-exist",           "packet with outer tag"},
    {USER_FIELD_ITAG_FMT,                   "itag-fmt",             "0b0: inner tag packet, 0b1: untag/priority tag packet"},
    {USER_FIELD_OTAG_FMT,                   "otag-fmt",             "0b0: outer tag packet, 0b1: untag/priority tag packet"},
    {USER_FIELD_FRAME_TYPE_L2,              "L2-frame-type",        "L2 frame type(0b00: Ethernet, 0b01: LLC_SNAP, 0b10: LLC_Other)"},
    {USER_FIELD_ETAG_EXIST,                 "etag-exist",           "packet with extra tag"},
    {USER_FIELD_ETHERTYPE,                  "ethertype",            "ethernet type/length"},
    {USER_FIELD_ARPOPCODE,                  "arp-opcode",           "ARP/RARP Opcode"},
    {USER_FIELD_OTAG_PRI,                   "opri",                 "O-TAG priority"},
    {USER_FIELD_DEI_VALUE,                  "dei",                  "O-TAG DEI field"},
    {USER_FIELD_OTAG_VID,                   "ovid",                 "O-TAG VID"},
    {USER_FIELD_ITAG_PRI,                   "ipri",                 "I-TAG priority"},
    {USER_FIELD_CFI_VALUE,                  "cfi",                  "I-TAG CFI field"},
    {USER_FIELD_ITAG_VID,                   "ivid",                 "I-TAG VID"},
    {USER_FIELD_MGNT_VLAN,                  "mgnt-vlan",            "mangement VLAN"},
    {USER_FIELD_FWD_VID,                    "fvid",                 "forward VID"},
    {USER_FIELD_L4_PROTO,                   "L4-frame-type",        "layer 4 format (0b000: UDP, 0b001: TCP, 0b010: ICMP/ICMPv6, 0b011: IGMP, 0x1XXX: L4 other)"},
    {USER_FIELD_IP4_SIP,                    "ip4-sip",              "IPv4 source IP"},
    {USER_FIELD_IP4_DIP,                    "ip4-dip",              "IPv4 destination IP"},
    {USER_FIELD_IP6_SIP,                    "ip6-sip",              "IPv6 srouce address"},
    {USER_FIELD_IP6_DIP,                    "ip6-dip",              "IPv6 destinaction address"},
    {USER_FIELD_IP4TOS_IP6TC,               "tos-tc",               "IPv4 TOS, IPv6 Traffic Class"},
    {USER_FIELD_IP4PROTO_IP6NH,             "proto-nh",             "IPv4 protocol, IPv6 Next Header"},
    {USER_FIELD_IP_FLAG,                    "ip-flag",              "IP flag"},
    {USER_FIELD_IP4_TTL_IP6_HOPLIMIT,       "ttl-hoplimit",         "IPv4 TTL, IPv6 hop limit (0b00: TTL = 0, 0b01: TTL = 1, 0b10: 2<= TTL < 255, 0b11: TTL = 255)"},
    {USER_FIELD_L4_SRC_PORT,                "l4-sport",             "TCP/UDP source port"},
    {USER_FIELD_L4_DST_PORT,                "l4-dport",             "TCP/UDP destination port"},
    {USER_FIELD_IP6_AUTH_HDR_EXIST,         "ip6-auth-hdr-exist",   "IPv6 packet with authentication header"},
    {USER_FIELD_IP6_DEST_HDR_EXIST,         "ip6-dest-hdr-exist",   "IPv6 packet with destination option header"},
    {USER_FIELD_IP6_FRAG_HDR_EXIST,         "ip6-frag-hdr-exist",   "IPv6 packet with fragment header"},
    {USER_FIELD_IP6_ROUTING_HDR_EXIST,      "ip6-routing-hdr-exist","IPv6 packet with routing header"},
    {USER_FIELD_IP6_HOP_HDR_EXIST,          "ip6-hop-hdr-exist",    "IPv6 packet with hop-by-hop header"},
    {USER_FIELD_IGMP_TYPE,                  "igmp-type",            "IGMP type"},
    {USER_FIELD_TCP_ECN,                    "tcp-ecn",              "TCP ECN"},
    {USER_FIELD_TCP_FLAG,                   "tcp-flag",             "TCP flag"},
    {USER_FIELD_TCP_NONZEROSEQ,             "tcp-nonzero-seq",      "TCP packet with non zero sequence"},
    {USER_FIELD_ICMP_CODE,                  "icmp-code",            "ICMP/ICMPv6 code"},
    {USER_FIELD_ICMP_TYPE,                  "icmp-type",            "ICMP/ICMPv6 type"},
    {USER_FIELD_IP_NONZEROOFFSET,           "ip-nonzero-offset",    "IPv4/IPv6 fragment offset isn't 0"},
    {USER_FIELD_IP_RANGE,                   "range-ip",             "IPv4/IPv6 range check result"},
    {USER_FIELD_LEN_RANGE,                  "range-len",            "Packet length(CRC included) range check result"},
    {USER_FIELD_FIELD_SELECTOR_VALID_MSK,   "field-selector-mask",  "Field selector valid mask"},
    {USER_FIELD_FIELD_SELECTOR0,            "field-selector0",      "Field selector 0 output"},
    {USER_FIELD_FIELD_SELECTOR1,            "field-selector1",      "Field selector 1 output"},
    {USER_FIELD_FIELD_SELECTOR2,            "field-selector2",      "Field selector 2 output"},
    {USER_FIELD_FIELD_SELECTOR3,            "field-selector3",      "Field selector 3 output"},
    {USER_FIELD_L2_CRC_ERROR,               "l2-crc-error",         "L2 crc error packet"},
#if defined(CONFIG_SDK_RTL8390)
    {USER_FIELD_IP_FRAGMENT_OFFSET,         "ip-offset",            "IP fragment offset"},
    {USER_FIELD_VID_RANGE1,                 "range-vid1",           "VID range check configuration 31-16 result"},
    {USER_FIELD_DPMM,                       "dpmm",                 "destination port mask configurations check result decided before egress ACL"},
    {USER_FIELD_UCAST_DA,                   "ucast-da",             "unicast DA"},
    {USER_FIELD_MCAST_DA,                   "mcast-da",             "multicast DA"},
    {USER_FIELD_NONZERO_DPM,                "nonzero-dpm",          "non-zero destination port mask"},
    {USER_FIELD_L2_DPM,                     "l2-dpm",               "destination port mask decided by LUT"},
    {USER_FIELD_L2_DPN,                     "l2-dpn",               "destination port number decided by LUT"},
    {USER_FIELD_INT_PRI,                    "int-pri",              "internal priority"},
    {USER_FIELD_IVID,                       "inner-vlan",           "inner VID given by ingress VLAN decision"},
    {USER_FIELD_OVID,                       "outer-vlan",           "outer VID given by ingress VLAN decision"},
    {USER_FIELD_IP4_CHKSUM_ERROR,           "ip-checksum-error",    "IPv4 checksum error packet"},
    {USER_FIELD_IGR_ACL_DROP_HIT,           "iacl-drop",            "Ingress ACL drop action hit"},
    {USER_FIELD_IGR_ACL_COPY_HIT,           "iacl-copy",            "Ingress ACL copy action hit"},
    {USER_FIELD_IGR_ACL_REDIRECT_HIT,       "iacl-redirect",        "Ingress ACL redirect action hit"},
    {USER_FIELD_IGR_ACL_ROUTING_HIT,        "iacl-routing",         "Ingress ACL routing action hit"},
#endif
#if defined(CONFIG_SDK_RTL8380)
    {USER_FIELD_PPPOE,                      "pppoe",                "pppoe packet"},
    {USER_FIELD_FLDSEL_RANGE,               "range-field-selector", "field selector range check result"},
    {USER_FIELD_IGMP_GROUPIP,               "igmp-group-ip",        "IGMP group IP address"},
    {USER_FIELD_DROPPED,                    "dropped",              "packet dropped before entering ACL"},
    {USER_FIELD_RTKPP,                      "rtkpp",                "0b00: unknown, 0b01: RRCP, 0b10: RLPP, 0b11: RLDP"},
    {USER_FIELD_RRCPHLNULLMAC,              "rrcp-nullmac",         "NULL uplink MAC in RRCP hello reply"},
    {USER_FIELD_RRCPOP,                     "rrcp-opcode",          "0b00: RRCP hello packet, 0b01: Reserved, 0b10: RRCP get, 0b11: RRCP set"},
    {USER_FIELD_RRCPREPLY,                  "rrcp-reply",           "RRCP reply"},
    {USER_FIELD_RRCPVER,                    "rrcp-version",         "0b00: RRCPv1, 0b01: RRCPv3"},
    {USER_FIELD_RRCPKEY,                    "rrcp-key",             "RRCP key match system setting"},
#endif
#if defined(CONFIG_SDK_RTL9300)
    {USER_FIELD_TRK_PRESENT,              "trunk-present",        "trunk-present"},
    {USER_FIELD_TRK_ID,                        "trk-id",                    "trunk id"},
    {USER_FIELD_STACKING_PORT,              "stacking-port",        "stacking port"},
    {USER_FIELD_SRC_FWD_VID,                "src-fwd-vid",          "forwarding vid before route"},
#endif
#if defined(CONFIG_SDK_RTL9310)
    {USER_FIELD_SA_NH_HIT,                  "sa-nh-hit",            "SMAC hit L2 entry with NH bit"},
    {USER_FIELD_SA_STATIC_HIT,              "sa-static-hit",        "SMAC hit L2 entry with static bit"},
    {USER_FIELD_SIP_HOST_HIT,               "sip-host-hit",         "SIP exists in L3 host routing table"},
#endif
#if defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL8390)
    {USER_FIELD_VID_RANGE0,                 "range-vid0",           "VID range check configuration 15-0 result"},
    {USER_FIELD_PORT_RANGE,                 "range-port",           "TCP/UDP port range check result"},
    {USER_FIELD_TELNET,                     "telnet",               "telnet packet, tcp and destination port=23"},
    {USER_FIELD_SSH,                        "ssh",                  "ssh packet, tcp and destination port=22"},
    {USER_FIELD_HTTP,                       "http",                 "http packet, tcp and destination port=80"},
    {USER_FIELD_HTTPS,                      "https",                "https packet, tcp and destination port=443"},
    {USER_FIELD_SNMP,                       "snmp",                 "snmp packet, udp and destination port=161, or destination port=10161"},
    {USER_FIELD_UNKNOWN_L7,                 "unknown-l7",           "packet is not TELNET/SSH/HTTP/HTTPS/SNMP"},
    {USER_FIELD_SPMM,                       "spmm",                 "source port mask configurations check result"},
    {USER_FIELD_SPMM_0_1,                   "spmm01",               "source port mask [1:0] of check result in fix field"},
    {USER_FIELD_SWITCHMAC,                  "switch-mac",           "destination MAC address is switch MAC address"},
#endif
#if defined(CONFIG_SDK_RTL8390)
    {USER_FIELD_DPN,                        "dpn",                  "destination port number decided before egress ACL"},
#endif
#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    {USER_FIELD_CONTENT_TOO_DEEP,           "content-too-deep",     "packet length is longer than the length parser supports"},
    {USER_FIELD_SPP,                        "spp",                  "soure physical port"},
    {USER_FIELD_IGR_NML_PORT,               "igr-nml-port",         "soure port is not CPU or stacking port"},
    {USER_FIELD_SENDER_ADDR,                "sender-addr",          "sender hardware address' of ARP header for ARP/RARP packet"},
    {USER_FIELD_TARGET_ADDR,                "target-addr",          "target hardware address' of ARP header for ARP/RARP packet"},
    {USER_FIELD_DEV_DMAC,                   "dev-dmac",             "DMAC is one of the router MAC TCAM entries with routing action"},
    {USER_FIELD_DSAP,                       "dsap",                 "DSAP for LLC/SNAP packet"},
    {USER_FIELD_SSAP,                       "ssap",                 "SSAP for LLC/SNAP packet"},
    {USER_FIELD_SNAP_OUI,                   "snap-oui",             "OUI in SNAP header"},
    {USER_FIELD_IGMP_MAX_RESP_CODE,         "igmp-max-resp-code",   "IGMP max response code"},
    {USER_FIELD_IP_FRAG,                    "ip-frag",              "IPv4 or IPv6 fragment , include first fragment"},
    {USER_FIELD_L4_HDR,                     "l4-hdr",               "L4 header byte0-byte3"},
    {USER_FIELD_IPUC_ROUT,                  "ipuc-rout",            "packet to do IP unicast routing"},
    {USER_FIELD_IPMC_ROUT,                  "ipmc-rout",            "packet to do IP mulitcast routing"},
    {USER_FIELD_VID_RANGE,                  "range-vid",            "VID range check result"},
    {USER_FIELD_L4_PORT_RANGE,              "range-l4port",         "TCP/UDP/SCTP port range check result"},
    {USER_FIELD_MAC_BASED_HIT,              "mac-based-hit",        "mac based vlan hit"},
    {USER_FIELD_IP_SUBNET_BASED_HIT,        "ip-subnet-based-hit",  "ip subnet based vlan hit"},
    {USER_FIELD_IVC_HIT,                    "ivc-hit",              "IVC hit"},
    {USER_FIELD_DIP_HOST_HIT,               "dip-host-hit",         "DIP exists in L3 host routing table"},
    {USER_FIELD_DIP_PREFIX_HIT,             "dip-prefix-hit",       "DIP exists in L3 prefix routing table"},
    {USER_FIELD_URPF_CHK_FAIL,              "urpf-chk-fail",        "uRPF check fail"},
    {USER_FIELD_PORT_MV,                    "port-mv",              "port move"},
    {USER_FIELD_IGR_VLAN_DROP,              "igr-vlan-drop",        "ingress vlan filter drop"},
    {USER_FIELD_STP_DROP,                   "stp-drop",             "stp drop"},
    {USER_FIELD_META_DATA,                  "meta-data",            "meta data"},
    {USER_FIELD_VLAN_GRPMSK,                "vlan-grpmsk",          "vlan group mask"},
    {USER_FIELD_IGR_TRK_PRESENT,            "igr-trunk-present",    "ingress trunk present"},
    {USER_FIELD_IGR_DEV_ID,                   "igr-dev-id",             "ingress device id"},
    {USER_FIELD_IGR_TRK_ID,                   "igr-trk-id",             "ingress trunk id"},
    {USER_FIELD_SLP,                        "slp",                  "soure logic port"},
    {USER_FIELD_LB_TIMES,                   "lb-times",             "loopback times"},
    {USER_FIELD_LB_PKT,                     "lb-pkt",               "loopback packet"},
    {USER_FIELD_VACL_DROP_HIT,              "vacl-drop",            "VACL drop action hit"},
    {USER_FIELD_VACL_COPY_HIT,              "vacl-copy",            "VACL copy action hit"},
    {USER_FIELD_VACL_REDIRECT_HIT,          "vacl-redirect",        "VACL redirect action hit"},
    {USER_FIELD_EGR_TRK_PRESENT,            "egr-trunk-present",    "egress trunk present"},
    {USER_FIELD_EGR_TRK_ID,                   "egr-trk-id",             "egress trunk id"},
    {USER_FIELD_EGR_DEV_ID,                   "egr-dev-id",             "egress device id"},
    {USER_FIELD_DLP,                        "dlp",                  "destination logic port"},
    {USER_FIELD_ETHER_AUX,                  "ether-aux",            "Ethernet auxiliary"},
#endif

#if defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL9300)
    {USER_FIELD_SPN,                        "spn",                  "source port number"},
#endif

#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310) || defined(CONFIG_SDK_RTL8390)
    {USER_FIELD_IP6_ESP_HDR_EXIST,          "ip6-esp-hdr-exist",    "IPv6 packet with ESP header"},
    {USER_FIELD_FIELD_SELECTOR4,            "field-selector4",      "Field selector 4 output"},
    {USER_FIELD_FIELD_SELECTOR5,            "field-selector5",      "Field selector 5 output"},
    {USER_FIELD_FIELD_SELECTOR6,            "field-selector6",      "Field selector 6 output"},
    {USER_FIELD_FIELD_SELECTOR7,            "field-selector7",      "Field selector 7 output"},
    {USER_FIELD_FIELD_SELECTOR8,            "field-selector8",      "Field selector 8 output"},
    {USER_FIELD_FIELD_SELECTOR9,            "field-selector9",      "Field selector 9 output"},
    {USER_FIELD_FIELD_SELECTOR10,           "field-selector10",     "Field selector 10 output"},
    {USER_FIELD_FIELD_SELECTOR11,           "field-selector11",     "Field selector 11 output"},
    {USER_FIELD_SA_LUT_RESULT,              "sa-l2hit",             "SA lookup result. 0: lookup miss 1: lookup hit"},
    {USER_FIELD_DA_LUT_RESULT,              "da-l2hit",             "DA lookup result. 0: lookup miss 1: lookup hit"},
    {USER_FIELD_ATTACK,                     "attack-pkt",           "packet hit attack prevention criteria"},
    {USER_FIELD_IP4_HDR_ERR,                "ip-hdr-error",         "IPv4 header error packet"},
    {USER_FIELD_DP,                         "dp",                   "drop precedence"},
#endif

#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)|| defined(CONFIG_SDK_RTL8380)
    {USER_FIELD_DATYPE,                     "datype",               "0b0: unicast dmac, 0b1: broadcast dmac, 0b10: reserved, 0b11: multicast dmac"},
    {USER_FIELD_IP6_MOB_HDR_EXIST,          "ip6-mob-hdr-exist",    "IPv6 packet with mobility header"},
    {USER_FIELD_FLOW_LABEL,                 "flow-label",           "IPv6 flow label"},
#endif

#if defined(CONFIG_SDK_RTL9310)|| defined(CONFIG_SDK_RTL8390)
    {USER_FIELD_IP_DSCP,                    "dscp",                 "IP DSCP"},
    {USER_FIELD_DPM,                        "dpm",                  "destination port mask decided before egress ACL"},
    {USER_FIELD_OMPLS_LABEL,                "ompls-label",          "outer MPLS label"},
    {USER_FIELD_OMPLS_EXP,                  "ompls-exp",            "outer MPLS label EXP"},
    {USER_FIELD_OMPLS_LABEL_EXIST,          "ompls-exist",          "outer MPLS label exist"},
    {USER_FIELD_IMPLS_LABEL,                "impls-label",          "inner MPLS label"},
    {USER_FIELD_IMPLS_EXP,                  "impls-exp",            "inner MPLS label EXP"},
    {USER_FIELD_IMPLS_LABEL_EXIST,          "impls-exist",          "inner MPLS label exist"},
#endif

#if defined(CONFIG_SDK_RTL9310)
    {USER_FIELD_DPP,                        "dpp",                  "destination physical port"},
    {USER_FIELD_EGR_NML_PORT,               "egr-nml-port",         "destination port is not CPU or stacking port"},
    {USER_FIELD_OMPLS_BOS,                  "ompls-bos",            "outer MPLS label bottom bit"},
    {USER_FIELD_IMPLS_BOS,                  "impls-bos",            "inner MPLS label bottom bit"},
    {USER_FIELD_IP6_HDR_UNSEQ,              "ip6-hdr-unseq",        "IPv6 header is unsequence"},
    {USER_FIELD_IP6_HDR_UNREP,              "ip6-hdr-unrep",        "IPv6 packet has unexcepted repeat header"},
    {USER_FIELD_IP6_NONEXT_HDR_EXIST,       "ip6-no-next-hdr-exist","IPv6 packet with no next extended header"},
    {USER_FIELD_FIELD_SELECTOR12,           "field-selector12",     "Field selector 12 output"},
    {USER_FIELD_FIELD_SELECTOR13,           "field-selector13",     "Field selector 13 output"},
    {USER_FIELD_VRFID,                      "vrf-id",               "VRF-ID"},
    {USER_FIELD_APP_TYPE,                   "app-type",             "0x0: MPLS, 0x1: GRE, 0x2: GTP, 0x3: VxLAN, 0x4: CAPWAP, 0x5: Diameter, 0x6: Other else"},
    {USER_FIELD_CAPWAP_KBIT,                "capwap-k-bit",         "K bit of CAPWAP packet"},
    {USER_FIELD_CAPWAP_MBIT,                "capwap-m-bit",         "M bit of CAPWAP packet"},
    {USER_FIELD_CAPWAP_WBIT,                "capwap-w-bit",         "W bit of CAPWAP packet"},
    {USER_FIELD_CAPWAP_LBIT,                "capwap-l-bit",         "L bit of CAPWAP packet"},
    {USER_FIELD_CAPWAP_FBIT,                "capwap-f-bit",         "F bit of CAPWAP packet"},
    {USER_FIELD_CAPWAP_TBIT,                "capwap-t-bit",         "T bit of CAPWAP packet"},
    {USER_FIELD_CAPWAP_RID,                 "capwap-rid",           "RID of CAPWAP packet"},
    {USER_FIELD_CAPWAP_HDR,                 "capwap-hdr-type",      "header type of CAPWAP packet"},
    {USER_FIELD_CAPWAP_TT_HIT,              "capwap-tt",            "the packet hit CAPWAP tunnel terminate"},
    {USER_FIELD_CAPWAP_TS_HIT,              "capwap-ts",            "the packet hit CAPWAP tunnel start"},
    {USER_FIELD_CAPWAP_80211_WEP,           "capwap-80211-wep-bit", "WEP bit in 802.11 header of CAPWAP packet"},
    {USER_FIELD_CAPWAP_80211_MF,            "capwap-80211-mf",      "more fragment bit in 802.11 header of CAPWAP packet"},
    {USER_FIELD_CAPWAP_80211_FRAME_TYPE,    "capwap-80211-type",    "802.11 frame type of CAPWAP packet"},
    {USER_FIELD_CAPWAP_IGR_BSSID_IDX,       "capwap-igr-bssid",     "ingress BSSID index of CAPWAP packet"},
    {USER_FIELD_CAPWAP_EGR_BSSID_IDX,       "capwap-egr-bssid",     "ergress BSSID index of CAPWAP packet"},
    {USER_FIELD_GRE_KEY,                    "gre-key",              "key of GRE packet"},
    {USER_FIELD_GTP_TEID,                   "gtp-teid",             "TEID of GTP packet"},
    {USER_FIELD_TT_HIT,                     "tt-hit",               "the packet hit tunnel terminate"},
    {USER_FIELD_TT_IDX,                     "tt-idx",               "the packet hit tunnel terminate index"},
    {USER_FIELD_TS_HIT,                     "ts-hit",               "the packet hit tunnel start"},
    {USER_FIELD_TS_IDX,                     "ts-idx",               "the packet hit tunnel start index"},
    {USER_FIELD_VXLAN_VNI,                  "vxlan-vni",            "VNI of VXLAN packet"},
    {USER_FIELD_IGR_INTF_IDX,               "igr-intf-idx",         "the packet ingress interface index"},
    {USER_FIELD_1BR_PCP,                    "1br-pcp",              "PCP of PE tag"},
    {USER_FIELD_1BR_DEI,                    "1br-dei",              "DEI of PE tag"},
    {USER_FIELD_1BR_IGR_ECID_BASE,          "1br-igr-ecid-base",    "ingress ECID base"},
    {USER_FIELD_1BR_TAG_EXIST,              "1br-tag-exist",        "PE tag exist"},
    {USER_FIELD_1BR_ECID_GRP,               "1br-ecid-grp",         "ECID group"},
    {USER_FIELD_1BR_ECID_BASE,              "1br-ecid-base",        "ECID base"},
    {USER_FIELD_1BR_IGR_ECID_EXT,           "1br-igr-ecid-ext",     "ingress ECID extend"},
    {USER_FIELD_1BR_ECID_EXT,               "1br-ecid-ext",         "ECID extend"},
#endif
};

/*
 * Macro Declaration
 */
#define DIAG_UTIL_ACL_PARAM_LEN_CHK(field_len, input)           \
do {                                                            \
    if ((((int)field_len + 3) / 4) < ((int)strlen(input) - 2))  \
    {                                                           \
        diag_util_printf("field data or mask is too long!\n");  \
        return CPARSER_NOT_OK;                                  \
    }                                                           \
} while(0)

/*
 * Function Declaration
 */
rtk_acl_phase_t _diag_acl_phaseTrans(uint32 unit, uint32 inPhase)
{
    #if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380)
    if (DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID) ||
            DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
        switch (inPhase)
        {
            case 0:
                return ACL_PHASE_IGR_ACL;
            #if defined(CONFIG_SDK_RTL8390)
            case 1:
                return ACL_PHASE_EGR_ACL;
            #endif  /* defined(CONFIG_SDK_RTL8390) */
            default:
                return ACL_PHASE_END;
        }
    }
    #endif  /* defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380) */

    #if (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310))
    if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        switch (inPhase)
        {
            case 0:
                return ACL_PHASE_VACL;
            case 1:
                return ACL_PHASE_IACL;
            #if defined(CONFIG_SDK_RTL9310)
            case 2:
                return ACL_PHASE_EACL;
            #endif  /* defined(CONFIG_SDK_RTL9310) */
            default:
                return ACL_PHASE_END;
        }
    }
    #endif  /* (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)) */

    return ACL_PHASE_END;
}

char* _diag_acl_phaseStr_get(rtk_acl_phase_t phase)
{
    switch (phase)
    {
        #if (defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL8390))
        case ACL_PHASE_IGR_ACL:
            return "Ingress ACL";
        #endif  /* (defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL8390)) */
        #if defined(CONFIG_SDK_RTL8390)
        case ACL_PHASE_EGR_ACL:
            return "Egress ACL";
        #endif  /* defined(CONFIG_SDK_RTL8390) */
        #if (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310))
        case ACL_PHASE_VACL:
            return "VLAN ACL";
        case ACL_PHASE_IACL:
            return "Ingress ACL";
        #endif  /* (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)) */
        #if defined(CONFIG_SDK_RTL9310)
        case ACL_PHASE_EACL:
            return "Egress ACL";
        #endif  /* defined(CONFIG_SDK_RTL9310) */
        default:
            break;
    }
    return "None";
}

#ifdef CMD_ACL_GET_ENTRY_PHASE_PHASE_FIELD_LIST
/*
 * acl get entry phase <UINT:phase> field-list
 */
cparser_result_t
cparser_cmd_acl_get_entry_phase_phase_field_list(cparser_context_t *context,
        uint32_t *phase_ptr)
{
    uint32  i;
    uint32  unit = 0;
    uint32  ori_level;
    int32   ret;
    rtk_acl_phase_t phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    rt_log_level_get(&ori_level);
    rt_log_level_set(0);

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);

    diag_util_mprintf("%-25s %s\n", "field keyword", "field desciption");
    diag_util_mprintf("=====================================================\n");

    for (i = 0; i < (sizeof(diag_acl_field_list)/sizeof(diag_acl_field_t)); ++i)
    {
        ret = rtk_acl_ruleEntryField_check(unit, phase,
                diag_acl_field_list[i].type);
        if (RT_ERR_OK != ret)
            continue;

        diag_util_mprintf("%-25s %s\n", diag_acl_field_list[i].user_info,
                diag_acl_field_list[i].desc);
    }

    rt_log_level_set(ori_level);

    return CPARSER_OK;
}
#endif

#ifdef CMD_ACL_GET_ENTRY_PHASE_PHASE_ENTRY_INDEX
/*
 * acl get entry phase <UINT:phase> entry <UINT:index>
 */
cparser_result_t cparser_cmd_acl_get_entry_phase_phase_entry_index(cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *index_ptr)
{
    int32  ret = RT_ERR_FAILED;
    uint32 unit = 0, i;
    uint32 entry_size = 0, entry_mask_position;
    uint8  *pEntry_buffer = NULL;
    rtk_acl_phase_t phase;
    uint32 *p32;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleEntrySize_get(unit, phase, &entry_size), ret);

    if ((pEntry_buffer = malloc(entry_size)) == NULL)
    {
        diag_util_printf("malloc() fail!\n");
        return CPARSER_NOT_OK;
    }

    if ((ret = rtk_acl_ruleEntry_read(unit, phase, *index_ptr, pEntry_buffer)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        free(pEntry_buffer);
        return CPARSER_NOT_OK;
    }

    p32 = (uint32 *)pEntry_buffer;
    for (i = 0; i < (entry_size / 4); ++i)
    {
        p32[i] = CPU_to_BE32(p32[i]);
    }

    entry_mask_position = (entry_size / 2);
    for (i = 0; i < entry_mask_position; i++)
    {
        if((i % 8) == 0)
        {
            diag_util_mprintf("\n");
        }
        diag_util_printf("byte %2u | data:%02x mask:%02x\n", i, *(pEntry_buffer + i),
            *(pEntry_buffer + i + entry_mask_position));
    }
    diag_util_mprintf("\n");

    free(pEntry_buffer);
    return CPARSER_OK;
}
#endif

#ifdef CMD_ACL_GET_ENTRY_PHASE_PHASE_ENTRY_INDEX_FIELD_FIELD_NAME
/*
 * acl get entry phase <UINT:phase> entry <UINT:index> field <STRING:field_name>
 */
cparser_result_t
cparser_cmd_acl_get_entry_phase_phase_entry_index_field_field_name(
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
    uint8   field_data[RTK_MAX_SIZE_OF_ACL_USER_FIELD];
    uint8   field_mask[RTK_MAX_SIZE_OF_ACL_USER_FIELD];
    rtk_acl_phase_t phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    num_element = (sizeof(diag_acl_field_list)/sizeof(diag_acl_field_t));
    osal_memset(field_data, 0, RTK_MAX_SIZE_OF_ACL_USER_FIELD);
    osal_memset(field_mask, 0, RTK_MAX_SIZE_OF_ACL_USER_FIELD);

    for (i = 0; i < num_element; ++i)
    {
        if (!strcasecmp(*field_name_ptr, diag_acl_field_list[i].user_info))
            break;
    }

    if (num_element <= i)
    {
        diag_util_printf("Invalid field %s!\n", *field_name_ptr);
        return CPARSER_NOT_OK;
    }

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);

    ret = rtk_acl_ruleEntryField_check(unit, phase,
            diag_acl_field_list[i].type);
    if (RT_ERR_OK != ret)
    {
        diag_util_printf("Field %s isn't supported in the phase!\n",
                *field_name_ptr);
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleEntryFieldSize_get(unit, diag_acl_field_list[i].type, &field_size), ret);

    if ((field_size % 8) == 0)
    {
        field_size = (field_size / 8);
    }
    else
    {
        field_size = (field_size / 8) + 1;
    }

    ret = rtk_acl_ruleEntryField_read(unit, phase, *index_ptr,
            diag_acl_field_list[i].type, field_data, field_mask);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
    diag_util_mprintf("index %d field %s\n", *index_ptr, *field_name_ptr);

    diag_util_mprintf("\tdata=0x");
    for (i = 0; i < field_size ; ++i)
    {
        diag_util_mprintf("%02x", field_data[i]);
    }
    diag_util_mprintf("\n");

    diag_util_mprintf("\tmask=0x");
    for (i = 0; i < field_size ; ++i)
    {
        diag_util_mprintf("%02x", field_mask[i]);
    }
    diag_util_mprintf("\n");

    return CPARSER_OK;
}
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_FIELD_FIELD_NAME_DATA_DATA_MASK_MASK
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> field <STRING:field_name> data <STRING:data> mask <STRING:mask>
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_field_field_name_data_data_mask_mask(
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
    uint8   field_data[RTK_MAX_SIZE_OF_ACL_USER_FIELD];
    uint8   field_mask[RTK_MAX_SIZE_OF_ACL_USER_FIELD];
    rtk_acl_phase_t phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    num_element = (sizeof(diag_acl_field_list)/sizeof(diag_acl_field_t));

    for (i = 0; i < num_element; ++i)
    {
        if (!strcasecmp(*field_name_ptr, diag_acl_field_list[i].user_info))
            break;
    }

    if (num_element <= i)
    {
        diag_util_printf("Invalid field %s!\n", *field_name_ptr);
        return CPARSER_NOT_OK;
    }

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
    ret = rtk_acl_ruleEntryField_check(unit, phase,
            diag_acl_field_list[i].type);
    if (RT_ERR_OK != ret)
    {
        diag_util_printf("Field %s isn't supported in the phase!\n",
                *field_name_ptr);
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleEntryFieldSize_get(unit, diag_acl_field_list[i].type, &field_size), ret);

    DIAG_UTIL_ACL_PARAM_LEN_CHK(field_size, *data_ptr);
    DIAG_UTIL_ACL_PARAM_LEN_CHK(field_size, *mask_ptr);
    osal_memset(field_data, 0x0, RTK_MAX_SIZE_OF_ACL_USER_FIELD);
    osal_memset(field_mask, 0x0, RTK_MAX_SIZE_OF_ACL_USER_FIELD);

    field_size = ((field_size + 7) / 8);

    if (diag_util_str2IntArray (field_data, *data_ptr, field_size) != RT_ERR_OK)
    {
        diag_util_printf("field data error!\n");
        return CPARSER_NOT_OK;
    }

    if (diag_util_str2IntArray (field_mask, *mask_ptr, field_size) != RT_ERR_OK)
    {
        diag_util_printf("field mask error!\n");
        return CPARSER_NOT_OK;
    }

    ret = rtk_acl_ruleEntryField_write(unit, phase, *index_ptr,
            diag_acl_field_list[i].type, field_data, field_mask);

    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_ACL_GET_PORT_PORT_LOOKUP_MISS_ACTION
/*
 * acl get port <UINT:port> lookup-miss action
 */
cparser_result_t
cparser_cmd_acl_get_port_port_lookup_miss_action(cparser_context_t *context,
    uint32_t *port_ptr)
{
    uint32                      unit;
    int32                       ret;
    rtk_acl_lookupMissAct_t     lmAct;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    ret = rtk_acl_lookupMissAct_get(unit, *port_ptr, &lmAct);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("Port %d lookup miss action is ");
    if (ACL_LOOKUPMISS_ACTION_PERMIT == lmAct)
        diag_util_mprintf("Permit\n");
    else
        diag_util_mprintf("Drop\n");

    return CPARSER_OK;
}
#endif

#ifdef CMD_ACL_SET_PORT_PORT_LOOKUP_MISS_ACTION_PERMIT_DROP
/*
 * acl set port <UINT:port> lookup-miss action ( permit  drop )
 */
cparser_result_t
cparser_cmd_acl_set_port_port_lookup_miss_action_permit_drop(
    cparser_context_t *context,
    uint32_t *port_ptr)
{
    uint32                      unit;
    int32                       ret;
    rtk_acl_lookupMissAct_t     lmAct;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if('p' == TOKEN_CHAR(6, 0))
    {
        lmAct = ACL_LOOKUPMISS_ACTION_PERMIT;
    }
    else
    {
        lmAct = ACL_LOOKUPMISS_ACTION_DROP;
    }

    ret = rtk_acl_lookupMissAct_set(unit, *port_ptr, lmAct);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_ACL_GET_PORT_PORT_LOOKUP_STATE
/*
 * acl get port <UINT:port> lookup state
 */
cparser_result_t
cparser_cmd_acl_get_port_port_lookup_state(cparser_context_t *context,
    uint32_t *port_ptr)
{
    uint32          unit;
    int32           ret;
    rtk_enable_t    enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    ret = rtk_acl_portLookupEnable_get(unit, *port_ptr, &enable);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("Port %d lookup state is ", *port_ptr);
    if (ENABLED == enable)
        diag_util_mprintf("%s\n", DIAG_STR_ENABLE);
    else
        diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

    return CPARSER_OK;
}
#endif

#ifdef CMD_ACL_SET_PORT_PORT_LOOKUP_STATE_DISABLE_ENABLE
/*
 * acl set port <UINT:port> lookup state ( disable | enable )
 */
cparser_result_t
cparser_cmd_acl_set_port_port_lookup_state_disable_enable(
    cparser_context_t *context,
    uint32_t *port_ptr)
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

    ret = rtk_acl_portLookupEnable_set(unit, *port_ptr, enable);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_ACL_GET_ENTRY_PHASE_PHASE_ENTRY_INDEX_VALIDATE
/*
 * acl get entry phase <UINT:phase> entry <UINT:index> validate
 */
cparser_result_t
cparser_cmd_acl_get_entry_phase_phase_entry_index_validate(cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *index_ptr)
{
    uint32  unit;
    uint32  valid;
    int32   ret;
    rtk_acl_phase_t phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
    ret = rtk_acl_ruleValidate_get(unit, phase, *index_ptr, &valid);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
    diag_util_mprintf("entry index %d is ", *index_ptr);

    if (0 == valid)
        diag_util_mprintf("Invalidate\n");
    else
        diag_util_mprintf("Validate\n");

    return CPARSER_OK;
}
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_STATE_VALID_INVALID
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> state ( valid | invalid )
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_state_valid_invalid(
    cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *index_ptr)
{
    uint32  unit;
    uint32  valid;
    int32   ret;
    rtk_acl_phase_t phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if('v' == TOKEN_CHAR(8, 0))
    {
        valid = 1;
    }
    else
    {
        valid = 0;
    }

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
    ret = rtk_acl_ruleValidate_set(unit, phase, *index_ptr, valid);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}
#endif

#ifdef CMD_ACL_GET_ENTRY_PHASE_PHASE_ENTRY_INDEX_OPERATION_REVERSE_STATE
/*
 * acl get entry phase <UINT:phase> entry <UINT:index> operation reverse state
 */
cparser_result_t
cparser_cmd_acl_get_entry_phase_phase_entry_index_operation_reverse_state(
    cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *index_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_operation_t oper;
    rtk_acl_phase_t phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&oper, 0, sizeof(rtk_acl_operation_t));

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
    ret = rtk_acl_ruleOperation_get(unit, phase, *index_ptr, &oper);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
    diag_util_mprintf("entry index %d reverse state is ", *index_ptr);

    if(ENABLED == oper.reverse)
    {
        diag_util_mprintf(DIAG_STR_ENABLE);
    }
    else
    {
        diag_util_mprintf(DIAG_STR_DISABLE);
    }
    diag_util_mprintf("\n");

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_get_entry_phase_entry_operation_reverse_state */
#endif

#ifdef CMD_ACL_GET_ENTRY_PHASE_PHASE_ENTRY_INDEX_OPERATION_AGGREGATE1_STATE
/*
 * acl get entry phase <UINT:phase> entry <UINT:index> operation aggregate1 state
 */
cparser_result_t
cparser_cmd_acl_get_entry_phase_phase_entry_index_operation_aggregate1_state(
    cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *index_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_operation_t oper;
    rtk_acl_phase_t phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
#if defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        if (ACL_PHASE_EACL == phase)
        {
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("ACL doesn't support the configuration\n");
            return CPARSER_NOT_OK;
        }
    }
#endif

    osal_memset(&oper, 0, sizeof(rtk_acl_operation_t));

    ret = rtk_acl_ruleOperation_get(unit, phase, *index_ptr, &oper);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
    diag_util_mprintf("entry index %d aggregate 1 state is ", *index_ptr);

    if(ENABLED == oper.aggr_1)
    {
        diag_util_mprintf(DIAG_STR_ENABLE);
    }
    else
    {
        diag_util_mprintf(DIAG_STR_DISABLE);
    }
    diag_util_mprintf("\n");

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_get_entry_phase_entry_operation_aggregate1_state */
#endif

#ifdef CMD_ACL_GET_ENTRY_PHASE_PHASE_ENTRY_INDEX_OPERATION_AGGREGATE2_STATE
/*
 * acl get entry phase <UINT:phase> entry <UINT:index> operation aggregate2 state
 */
cparser_result_t
cparser_cmd_acl_get_entry_phase_phase_entry_index_operation_aggregate2_state(
    cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *index_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_operation_t oper;
    rtk_acl_phase_t phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&oper, 0, sizeof(rtk_acl_operation_t));

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
    ret = rtk_acl_ruleOperation_get(unit, phase, *index_ptr, &oper);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
    diag_util_mprintf("entry index %d aggregate 2 state is ", *index_ptr);

    if(ENABLED == oper.aggr_2)
    {
        diag_util_mprintf(DIAG_STR_ENABLE);
    }
    else
    {
        diag_util_mprintf(DIAG_STR_DISABLE);
    }
    diag_util_mprintf("\n");

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_get_entry_phase_entry_operation_aggregate2_state */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_OPERATION_REVERSE_STATE_DISABLE_ENABLE
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> operation reverse state ( disable | enable )
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_operation_reverse_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_operation_t oper;
    rtk_acl_phase_t phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&oper, 0, sizeof(rtk_acl_operation_t));

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
    ret = rtk_acl_ruleOperation_get(unit, phase, *index_ptr, &oper);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    if('e' == TOKEN_CHAR(10, 0))
    {
        oper.reverse = ENABLED;
    }
    else
    {
        oper.reverse = DISABLED;
    }

    ret = rtk_acl_ruleOperation_set(unit, phase, *index_ptr, &oper);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_entry_operation_reverse_state_disable_enable */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_OPERATION_AGGREGATE1_STATE_DISABLE_ENABLE
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> operation aggregate1 state ( disable | enable )
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_operation_aggregate1_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_operation_t oper;
    rtk_acl_phase_t phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
#if defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        if (ACL_PHASE_EACL == phase)
        {
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("ACL doesn't support the configuration\n");
            return CPARSER_NOT_OK;
        }
    }
#endif

    osal_memset(&oper, 0, sizeof(rtk_acl_operation_t));

    ret = rtk_acl_ruleOperation_get(unit, phase, *index_ptr, &oper);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    if('e' == TOKEN_CHAR(10, 0))
    {
        oper.aggr_1 = ENABLED;
    }
    else
    {
        oper.aggr_1 = DISABLED;
    }

    ret = rtk_acl_ruleOperation_set(unit, phase, *index_ptr, &oper);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_entry_operation_aggregate1_state_disable_enable */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_OPERATION_AGGREGATE2_STATE_DISABLE_ENABLE
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> operation aggregate2 state ( disable | enable )
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_operation_aggregate2_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_operation_t oper;
    rtk_acl_phase_t phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&oper, 0, sizeof(rtk_acl_operation_t));

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
    ret = rtk_acl_ruleOperation_get(unit, phase, *index_ptr, &oper);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    if('e' == TOKEN_CHAR(10, 0))
    {
        oper.aggr_2 = ENABLED;
    }
    else
    {
        oper.aggr_2 = DISABLED;
    }

    ret = rtk_acl_ruleOperation_set(unit, phase, *index_ptr, &oper);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_entry_operation_aggregate2_state_disable_enable */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_FORWARD_STATE_DISABLE_ENABLE
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action forward state ( disable | enable )
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_forward_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    rtk_enable_t        enable;
    rtk_acl_phase_t phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
#if defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        if (ACL_PHASE_EACL == phase)
        {
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
        }
    }
#endif
    ret = rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    if('e' == TOKEN_CHAR(10, 0))
    {
        enable = ENABLED;
    }
    else
    {
        enable = DISABLED;
    }

    switch (phase)
    {
        #if (defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL8390))
        case ACL_PHASE_IGR_ACL:
            action.igr_acl.fwd_en = enable;
            break;
        #endif  /* (defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL8390)) */
        #if defined(CONFIG_SDK_RTL8390)
        case ACL_PHASE_EGR_ACL:
            action.egr_acl.fwd_en = enable;
            break;
        #endif  /* defined(CONFIG_SDK_RTL8390) */
        #if (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310))
        case ACL_PHASE_VACL:
            action.vact.fwd_en = enable;
            break;
        case ACL_PHASE_IACL:
            action.iact.fwd_en = enable;
            break;
        #endif  /* (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)) */
        default:
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_entry_action_forward_state_disable_enable */
#endif

/*
*   acl set entry phase <UINT:phase> entry <UINT:index> action drop ( drop | withdraw )
*/
#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_DROP_DROP_WITHDRAW

cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_drop_drop_withdraw(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    rtk_acl_phase_t phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    #ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_DROP_PERMIT_DROP
    if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID))
    {
        if('w' == TOKEN_CHAR(9, 0))
            return CPARSER_NOT_OK;
        return cparser_cmd_acl_set_entry_phase_phase_entry_index_action_drop_permit_drop(context, phase_ptr, index_ptr);
    }
    #endif

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
    ret = rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    if (ACL_PHASE_IGR_ACL == phase)
    {
        if('d' == TOKEN_CHAR(9, 0))
        {
            action.igr_acl.drop_data = ACL_IGR_ACTION_DROP_DROP;
        }
        else
        {
            action.igr_acl.drop_data = ACL_IGR_ACTION_DROP_WITHDRAW;
        }
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_entry_action_forward_state_disable_enable */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_DROP_STATE_DISABLE_ENABLE
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action drop state ( disable | enable )
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_drop_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    rtk_enable_t        enable;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action), ret);

    if('e' == TOKEN_CHAR(10, 0))
    {
        enable = ENABLED;
    }
    else
    {
        enable = DISABLED;
    }

    switch (phase)
    {
        case ACL_PHASE_VACL:
            action.vact.green_drop_en = enable;
            break;
        case ACL_PHASE_IACL:
            action.iact.green_drop_en = enable;
            break;
        #if defined(CONFIG_SDK_RTL9310)
        case ACL_PHASE_EACL:
            if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
            {
                action.eact.green_drop_en = enable;
                break;
            }
        #endif
        default:
            DIAG_ERR_PRINT(RT_ERR_ACL_PHASE);
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_entry_action_forward_state_disable_enable */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_YELLOW_DROP_STATE_DISABLE_ENABLE
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action yellow-drop state ( disable | enable )
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_yellow_drop_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    rtk_enable_t        enable;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
    ret = rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    if('e' == TOKEN_CHAR(10, 0))
    {
        enable = ENABLED;
    }
    else
    {
        enable = DISABLED;
    }

    switch (phase)
    {
        case ACL_PHASE_VACL:
            action.vact.yellow_drop_en = enable;
            break;
        case ACL_PHASE_IACL:
            action.iact.yellow_drop_en = enable;
            break;
        #if defined(CONFIG_SDK_RTL9310)
        case ACL_PHASE_EACL:
            if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
            {
                action.eact.yellow_drop_en = enable;
                break;
            }
        #endif
        default:
            DIAG_ERR_PRINT(RT_ERR_ACL_PHASE);
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_entry_action_forward_state_disable_enable */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_RED_DROP_STATE_DISABLE_ENABLE
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action red-drop state ( disable | enable )
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_red_drop_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    rtk_enable_t        enable;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
    ret = rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    if('e' == TOKEN_CHAR(10, 0))
    {
        enable = ENABLED;
    }
    else
    {
        enable = DISABLED;
    }

    switch (phase)
    {
        case ACL_PHASE_VACL:
            action.vact.red_drop_en = enable;
            break;
        case ACL_PHASE_IACL:
            action.iact.red_drop_en = enable;
            break;
        #if defined(CONFIG_SDK_RTL9310)
        case ACL_PHASE_EACL:
            if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
            {
                action.eact.red_drop_en = enable;
                break;
            }
        #endif
        default:
            DIAG_ERR_PRINT(RT_ERR_ACL_PHASE);
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_entry_action_forward_state_disable_enable */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_DROP_PERMIT_DROP
/*
*   acl set entry phase <UINT:phase> entry <UINT:index> action drop ( permit | drop )
*/
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_drop_permit_drop(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr)
{
    rtk_acl_actionColorDrop_t   dropAct;
    rtk_acl_action_t            action;
    rtk_acl_phase_t             phase;
    uint32                      unit;
    int32                       ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
    ret = rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    if('p' == TOKEN_CHAR(9, 0))
    {
        dropAct = ACL_ACTION_COLOR_DROP_PERMIT;
    }
    else
    {
        dropAct = ACL_ACTION_COLOR_DROP_DROP;
    }

    switch (phase)
    {
        case ACL_PHASE_VACL:
            action.vact.green_drop_data = dropAct;
            break;
        case ACL_PHASE_IACL:
            action.iact.green_drop_data = dropAct;
            break;
        #if defined(CONFIG_SDK_RTL9310)
        case ACL_PHASE_EACL:
            if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
            {
                action.eact.green_drop_data = dropAct;
                break;
            }
        #endif
        default:
            DIAG_ERR_PRINT(RT_ERR_ACL_PHASE);
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_entry_action_forward_state_disable_enable */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_YELLOW_DROP_PERMIT_DROP
/*
*   acl set entry phase <UINT:phase> entry <UINT:index> action yellow-drop ( permit | drop )
*/
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_yellow_drop_permit_drop(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr)
{
    rtk_acl_actionColorDrop_t   dropAct;
    rtk_acl_action_t            action;
    rtk_acl_phase_t             phase;
    uint32                      unit;
    int32                       ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
    ret = rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    if('p' == TOKEN_CHAR(9, 0))
    {
        dropAct = ACL_ACTION_COLOR_DROP_PERMIT;
    }
    else
    {
        dropAct = ACL_ACTION_COLOR_DROP_DROP;
    }

    switch (phase)
    {
        case ACL_PHASE_VACL:
            action.vact.yellow_drop_data = dropAct;
            break;
        case ACL_PHASE_IACL:
            action.iact.yellow_drop_data = dropAct;
            break;
        #if defined(CONFIG_SDK_RTL9310)
        case ACL_PHASE_EACL:
            if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
            {
                action.eact.yellow_drop_data = dropAct;
                break;
            }
        #endif
        default:
            DIAG_ERR_PRINT(RT_ERR_ACL_PHASE);
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_entry_action_forward_state_disable_enable */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_RED_DROP_PERMIT_DROP
/*
*   acl set entry phase <UINT:phase> entry <UINT:index> action red-drop ( permit | drop )
*/
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_red_drop_permit_drop(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr)
{
    rtk_acl_actionColorDrop_t   dropAct;
    rtk_acl_action_t            action;
    rtk_acl_phase_t             phase;
    uint32                      unit;
    int32                       ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
    ret = rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    if('p' == TOKEN_CHAR(9, 0))
    {
        dropAct = ACL_ACTION_COLOR_DROP_PERMIT;
    }
    else
    {
        dropAct = ACL_ACTION_COLOR_DROP_DROP;
    }

    switch (phase)
    {
        case ACL_PHASE_VACL:
            action.vact.red_drop_data = dropAct;
            break;
        case ACL_PHASE_IACL:
            action.iact.red_drop_data = dropAct;
            break;
        #if defined(CONFIG_SDK_RTL9310)
        case ACL_PHASE_EACL:
            if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
            {
                action.eact.red_drop_data = dropAct;
                break;
            }
        #endif
        default:
            DIAG_ERR_PRINT(RT_ERR_ACL_PHASE);
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_entry_action_forward_state_disable_enable */
#endif


#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_PERMIT
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action permit
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_permit(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
#if defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        if (ACL_PHASE_EACL == phase)
        {
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
        }
    }
#endif

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    ret = rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    switch (phase)
    {
        #if (defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL8390))
        case ACL_PHASE_IGR_ACL:
            action.igr_acl.fwd_data.fwd_type = ACL_IGR_ACTION_FWD_PERMIT;
            break;
        #endif  /* (defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL8390)) */
        #if defined(CONFIG_SDK_RTL8390)
        case ACL_PHASE_EGR_ACL:
            action.egr_acl.fwd_data.fwd_type = ACL_EGR_ACTION_FWD_PERMIT;
            break;
        #endif  /* defined(CONFIG_SDK_RTL8390) */
        #if (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310))
        case ACL_PHASE_VACL:
            action.vact.fwd_data.fwd_type = ACL_ACTION_FWD_PERMIT;
            break;
        case ACL_PHASE_IACL:
            action.iact.fwd_data.fwd_type = ACL_ACTION_FWD_PERMIT;
            break;
        #endif  /* (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)) */
        default:
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_entry_action_permit */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_FORWARD_DROP
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action forward drop
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_forward_drop(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *entry_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
#if defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        if (ACL_PHASE_EACL == phase)
        {
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
        }
    }
#endif

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    ret = rtk_acl_ruleAction_get(unit, phase, *entry_ptr, &action);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    switch (phase)
    {
        #if (defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL8390))
        case ACL_PHASE_IGR_ACL:
            action.igr_acl.fwd_data.fwd_type = ACL_IGR_ACTION_FWD_DROP;
            break;
        #endif  /* (defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL8390)) */
        #if defined(CONFIG_SDK_RTL8390)
        case ACL_PHASE_EGR_ACL:
            action.egr_acl.fwd_data.fwd_type = ACL_EGR_ACTION_FWD_DROP;
            break;
        #endif  /* defined(CONFIG_SDK_RTL8390) */
        #if (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310))
        case ACL_PHASE_VACL:
            action.vact.fwd_data.fwd_type = ACL_ACTION_FWD_DROP;
            break;
        case ACL_PHASE_IACL:
            action.iact.fwd_data.fwd_type = ACL_ACTION_FWD_DROP;
            break;
        #endif  /* (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)) */
        default:
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
    }

    ret = rtk_acl_ruleAction_set(unit, phase, *entry_ptr, &action);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_entry_action_drop */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_COPY_PORT_PORT
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action copy port <UINT:port>
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_copy_port_port(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr,
    uint32_t  *port_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
    ret = rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    switch (phase)
    {
        #if (defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL8390))
        case ACL_PHASE_IGR_ACL:
            action.igr_acl.fwd_data.fwd_type = ACL_IGR_ACTION_FWD_COPY_TO_PORTID;
            #if defined(CONFIG_SDK_RTL8380)
            if(DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
            {
                action.igr_acl.fwd_data.info.copy_redirect_port.fwd_port_id = *port_ptr;
            }
            #endif  /* defined(CONFIG_SDK_RTL8380) */

            #if defined(CONFIG_SDK_RTL8390)
            if(DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
            {
                action.igr_acl.fwd_data.fwd_info = *port_ptr;
            }
            #endif  /* defined(CONFIG_SDK_RTL8390) */
            break;
        #endif  /* (defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL8390)) */
        #if defined(CONFIG_SDK_RTL8390)
        case ACL_PHASE_EGR_ACL:
            action.egr_acl.fwd_data.fwd_type = ACL_EGR_ACTION_FWD_COPY_TO_PORTID;
            action.egr_acl.fwd_data.fwd_info = *port_ptr;
            break;
        #endif  /* defined(CONFIG_SDK_RTL8390) */
        #if (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310))
        case ACL_PHASE_VACL:
            action.vact.fwd_data.fwd_type = ACL_ACTION_FWD_COPY_TO_PORTID;
            action.vact.fwd_data.fwd_info = *port_ptr;
            break;
        case ACL_PHASE_IACL:
            action.iact.fwd_data.fwd_type = ACL_ACTION_FWD_COPY_TO_PORTID;
            action.iact.fwd_data.fwd_info = *port_ptr;
            break;
        #endif  /* (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)) */
        default:
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_entry_action_copy_uni_dpn */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_COPY_DEV_DEVID_PORT_PORT
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action copy dev <UINT:devID> port <UINT:port>
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_copy_dev_devID_port_port(
    cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *index_ptr,
    uint32_t *devId_ptr,
    uint32_t *port_ptr)
{
    rtk_acl_action_t    action;
    rtk_acl_phase_t     phase;
    uint32              unit;
    int32               ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
#if defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        if (ACL_PHASE_EACL == phase)
        {
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
        }
    }
#endif

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    ret = rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    switch (phase)
    {
        #if (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310))
        case ACL_PHASE_VACL:
            action.vact.fwd_data.flags &= ~RTK_ACL_FWD_FLAG_TRUNK;
            action.vact.fwd_data.fwd_type = ACL_ACTION_FWD_COPY_TO_PORTID;
            action.vact.fwd_data.devID = *devId_ptr;
            action.vact.fwd_data.fwd_info = *port_ptr;
            break;
        case ACL_PHASE_IACL:
            action.iact.fwd_data.flags &= ~RTK_ACL_FWD_FLAG_TRUNK;
            action.iact.fwd_data.fwd_type = ACL_ACTION_FWD_COPY_TO_PORTID;
            action.iact.fwd_data.devID = *devId_ptr;
            action.iact.fwd_data.fwd_info = *port_ptr;
            break;
        #endif  /* (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)) */
        default:
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_phase_entry_index_action_copy_unit_unitId_port_port */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_COPY_TRUNK_TID
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action copy trunk <UINT:tid>
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_copy_trunk_tid(
    cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *index_ptr,
    uint32_t *tid_ptr)
{
    rtk_acl_action_t    action;
    rtk_acl_phase_t     phase;
    uint32              unit;
    int32               ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
#if defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        if (ACL_PHASE_EACL == phase)
        {
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
        }
    }
#endif

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    ret = rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    switch (phase)
    {
        #if (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310))
        case ACL_PHASE_VACL:
            action.vact.fwd_data.flags |= RTK_ACL_FWD_FLAG_TRUNK;
            action.vact.fwd_data.fwd_type = ACL_ACTION_FWD_COPY_TO_PORTID;
            action.vact.fwd_data.fwd_info = *tid_ptr;
            break;
        case ACL_PHASE_IACL:
            action.iact.fwd_data.flags |= RTK_ACL_FWD_FLAG_TRUNK;
            action.iact.fwd_data.fwd_type = ACL_ACTION_FWD_COPY_TO_PORTID;
            action.iact.fwd_data.fwd_info = *tid_ptr;;
            break;
        #endif  /* (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)) */
        default:
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_phase_entry_index_action_copy_trunk_tid */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_COPY_PORTMASK_INDEX_PORTMASK_INDEX
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action copy portmask-index <UINT:portmask_index>
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_copy_portmask_index_portmask_index(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr,
    uint32_t  *portmask_index_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
#if defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        if (ACL_PHASE_EACL == phase)
        {
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
        }
    }
#endif

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    ret = rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    switch (phase)
    {
        #if (defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL8390))
        case ACL_PHASE_IGR_ACL:
            action.igr_acl.fwd_data.fwd_type = ACL_IGR_ACTION_FWD_COPY_TO_PORTMASK;

            #if defined(CONFIG_SDK_RTL8380)
            if(DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
            {
                action.igr_acl.fwd_data.info.copy_redirect_portMsk.fwd_idx = *portmask_index_ptr;
            }
            #endif
            #if defined(CONFIG_SDK_RTL8390)
            if(DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
            {
                action.igr_acl.fwd_data.fwd_info = *portmask_index_ptr;
            }
            #endif
            break;
        #endif  /* (defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL8390)) */
        #if defined(CONFIG_SDK_RTL8390)
        case ACL_PHASE_EGR_ACL:
            action.egr_acl.fwd_data.fwd_type = ACL_EGR_ACTION_FWD_COPY_TO_PORTMASK;
            action.egr_acl.fwd_data.fwd_info = *portmask_index_ptr;
            break;
        #endif  /* defined(CONFIG_SDK_RTL8390) */
        #if (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310))
        case ACL_PHASE_VACL:
            action.vact.fwd_data.fwd_type = ACL_ACTION_FWD_COPY_TO_PORTMASK;
            action.vact.fwd_data.fwd_info = *portmask_index_ptr;
            break;
        case ACL_PHASE_IACL:
            action.iact.fwd_data.fwd_type = ACL_ACTION_FWD_COPY_TO_PORTMASK;
            action.iact.fwd_data.fwd_info = *portmask_index_ptr;
            break;
        #endif  /* (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)) */
        default:
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_entry_action_copy_multi_portmask_index */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_REDIRECT_PORT_PORT
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action redirect port <UINT:port>
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_redirect_port_port(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *entry_ptr,
    uint32_t  *dpn_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
    ret = rtk_acl_ruleAction_get(unit, phase, *entry_ptr, &action);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    switch (phase)
    {
        #if (defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL8390))
        case ACL_PHASE_IGR_ACL:
            action.igr_acl.fwd_data.fwd_type = ACL_IGR_ACTION_FWD_REDIRECT_TO_PORTID;
            #if defined(CONFIG_SDK_RTL8380)
            if(DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
            {
                action.igr_acl.fwd_data.info.copy_redirect_port.fwd_port_id = *dpn_ptr;
            }
            #endif  /* defined(CONFIG_SDK_RTL8380) */
            #if defined(CONFIG_SDK_RTL8390)
            if(DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
            {
                action.igr_acl.fwd_data.fwd_info = *dpn_ptr;
            }
            #endif  /* defined(CONFIG_SDK_RTL8390) */
            break;
        #endif  /* (defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL8390)) */
        #if defined(CONFIG_SDK_RTL8390)
        case ACL_PHASE_EGR_ACL:
            action.egr_acl.fwd_data.fwd_type = ACL_EGR_ACTION_FWD_REDIRECT_TO_PORTID;
            action.egr_acl.fwd_data.fwd_info = *dpn_ptr;
            break;
        #endif  /* defined(CONFIG_SDK_RTL8390) */
        #if defined(CONFIG_SDK_RTL9300)
        case ACL_PHASE_VACL:
            action.vact.fwd_data.fwd_type = ACL_ACTION_FWD_REDIRECT_TO_PORTID;
            action.vact.fwd_data.fwd_info = *dpn_ptr;
            break;
        case ACL_PHASE_IACL:
            action.iact.fwd_data.fwd_type = ACL_ACTION_FWD_REDIRECT_TO_PORTID;
            action.iact.fwd_data.fwd_info = *dpn_ptr;
            break;
        #endif  /* defined(CONFIG_SDK_RTL9300) */
        default:
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
    }

    ret = rtk_acl_ruleAction_set(unit, phase, *entry_ptr, &action);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }
    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_entry_action_redirect_uni_dpn */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_REDIRECT_DEV_DEVID_PORT_PORT
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action redirect dev <UINT:devId> port <UINT:port>
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_redirect_dev_devID_port_port(
    cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *index_ptr,
    uint32_t *devId_ptr,
    uint32_t *port_ptr)
{
    rtk_acl_action_t    action;
    rtk_acl_phase_t     phase;
    uint32              unit;
    int32               ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
#if defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        if (ACL_PHASE_EACL == phase)
        {
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
        }
    }
#endif

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    ret = rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    switch (phase)
    {
        #if (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310))
        case ACL_PHASE_VACL:
            action.vact.fwd_data.fwd_type = ACL_ACTION_FWD_REDIRECT_TO_PORTID;
            action.vact.fwd_data.flags &= ~RTK_ACL_FWD_FLAG_TRUNK;
            action.vact.fwd_data.devID = *devId_ptr;
            action.vact.fwd_data.fwd_info = *port_ptr;
            break;
        case ACL_PHASE_IACL:
            action.iact.fwd_data.fwd_type = ACL_ACTION_FWD_REDIRECT_TO_PORTID;
            action.iact.fwd_data.flags &= ~RTK_ACL_FWD_FLAG_TRUNK;
            action.iact.fwd_data.devID = *devId_ptr;
            action.iact.fwd_data.fwd_info = *port_ptr;
            break;
        #endif  /* (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)) */
        default:
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_phase_entry_index_action_redirect_unit_unitId_port_port */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_REDIRECT_TRUNK_TID
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action redirect trunk <UINT:tid>
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_redirect_trunk_tid(
    cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *index_ptr,
    uint32_t *tid_ptr)
{
    rtk_acl_action_t    action;
    rtk_acl_phase_t     phase;
    uint32              unit;
    int32               ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
#if defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        if (ACL_PHASE_EACL == phase)
        {
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
        }
    }
#endif

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    ret = rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    switch (phase)
    {
        #if (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310))
        case ACL_PHASE_VACL:
            action.vact.fwd_data.fwd_type = ACL_ACTION_FWD_REDIRECT_TO_PORTID;
            action.vact.fwd_data.flags |= RTK_ACL_FWD_FLAG_TRUNK;
            action.vact.fwd_data.fwd_info = *tid_ptr;
            break;
        case ACL_PHASE_IACL:
            action.iact.fwd_data.fwd_type = ACL_ACTION_FWD_REDIRECT_TO_PORTID;
            action.iact.fwd_data.flags |= RTK_ACL_FWD_FLAG_TRUNK;
            action.iact.fwd_data.fwd_info = *tid_ptr;
            break;
        #endif  /* (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)) */
        default:
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_phase_entry_index_action_redirect_trunk_tid */
#endif



#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_REDIRECT_PORTMASK_INDEX_PORTMASK_INDEX
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action redirect portmask-index <UINT:portmask_index>
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_redirect_portmask_index_portmask_index(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr,
    uint32_t  *portmask_index_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
#if defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        if (ACL_PHASE_EACL == phase)
        {
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
        }
    }
#endif

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    ret = rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    switch (phase)
    {
        #if (defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL8390))
        case ACL_PHASE_IGR_ACL:
            action.igr_acl.fwd_data.fwd_type = ACL_IGR_ACTION_FWD_REDIRECT_TO_PORTMASK;
            #if defined(CONFIG_SDK_RTL8380)
            if(DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
            {
                action.igr_acl.fwd_data.info.copy_redirect_portMsk.fwd_idx = *portmask_index_ptr;
            }
            #endif  /* defined(CONFIG_SDK_RTL8380) */
            #if defined(CONFIG_SDK_RTL8390)
            if(DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
            {
                action.igr_acl.fwd_data.fwd_info = *portmask_index_ptr;
            }
            #endif  /* defined(CONFIG_SDK_RTL8390) */
            break;
        #endif  /* (defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL8390)) */
        #if defined(CONFIG_SDK_RTL8390)
        case ACL_PHASE_EGR_ACL:
            action.egr_acl.fwd_data.fwd_type = ACL_EGR_ACTION_FWD_REDIRECT_TO_PORTMASK;
            action.egr_acl.fwd_data.fwd_info = *portmask_index_ptr;
            break;
        #endif  /* defined(CONFIG_SDK_RTL8390) */
        #if (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310))
        case ACL_PHASE_VACL:
            action.vact.fwd_data.fwd_type = ACL_ACTION_FWD_REDIRECT_TO_PORTMASK;
            action.vact.fwd_data.fwd_info = *portmask_index_ptr;
            break;
        case ACL_PHASE_IACL:
            action.iact.fwd_data.fwd_type = ACL_ACTION_FWD_REDIRECT_TO_PORTMASK;
            action.iact.fwd_data.fwd_info = *portmask_index_ptr;
            break;
        #endif  /* (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)) */
        default:
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_entry_action_redirect_multi_portmask_index */
#endif

/*
* acl set entry phase <UINT:phase> entry <UINT:index> action copy skip-ingress-vlan ( disable | enable )
*/
#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_COPY_SKIP_INGRESS_VLAN_DISABLE_ENABLE
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_copy_skip_ingress_vlan_disable_enable(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    rtk_enable_t        enable;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if('e' == TOKEN_CHAR(10, 0))
    {
        enable = ENABLED;
    }
    else
    {
        enable = DISABLED;
    }

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
    ret = rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    if (ACL_PHASE_IGR_ACL == phase)
    {
        action.igr_acl.fwd_data.info.copy_redirect_port.skip_storm_igrVlan = enable;
        action.igr_acl.fwd_data.info.copy_redirect_portMsk.skip_storm_igrVlan = enable;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}

#endif

/*
*    acl set entry phase <UINT:phase> entry <UINT:index> action redirect skip-storm ( disable | enable )
*/
#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_REDIRECT_SKIP_STORM_DISABLE_ENABLE
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_redirect_skip_storm_disable_enable(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    rtk_enable_t        enable;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if('e' == TOKEN_CHAR(10, 0))
    {
        enable = ENABLED;
    }
    else
    {
        enable = DISABLED;
    }

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
    ret = rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    if (ACL_PHASE_IGR_ACL == phase)
    {
        action.igr_acl.fwd_data.info.copy_redirect_port.skip_storm_igrVlan = enable;
        action.igr_acl.fwd_data.info.copy_redirect_portMsk.skip_storm_igrVlan = enable;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}
#endif

/*
*   acl set entry phase <UINT:phase> entry <UINT:index> action redirect force ( disable | enable )
*/
#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_REDIRECT_FORCE_DISABLE_ENABLE
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_redirect_force_disable_enable(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    rtk_enable_t        enable;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if('e' == TOKEN_CHAR(10, 0))
    {
        enable = ENABLED;
    }
    else
    {
        enable = DISABLED;
    }

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
    ret = rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    if (ACL_PHASE_IGR_ACL == phase)
    {
        action.igr_acl.fwd_data.info.copy_redirect_port.force = enable;
        action.igr_acl.fwd_data.info.copy_redirect_portMsk.force = enable;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}
#endif

/*
*  acl set entry phase <UINT:phase> entry <UINT:index> action forward cpu-tag ( disable | enable )
*/
#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_FORWARD_CPU_TAG_DISABLE_ENABLE
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_forward_cpu_tag_disable_enable(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    rtk_enable_t        enable;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if('e' == TOKEN_CHAR(10, 0))
    {
        enable = ENABLED;
    }
    else
    {
        enable = DISABLED;
    }

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
    ret = rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    if (ACL_PHASE_IGR_ACL == phase)
    {
        action.igr_acl.fwd_data.info.copy_redirect_port.cpu_tag = enable;
        action.igr_acl.fwd_data.info.copy_redirect_portMsk.cpu_tag = enable;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}
#endif

/*
* acl set entry phase <UINT:phase> entry <UINT:index> action forward skip-ingress-stp ( disable | enable )

*/
#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_FORWARD_SKIP_INGRESS_STP_DISABLE_ENABLE
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_forward_skip_ingress_stp_disable_enable(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    rtk_enable_t        enable;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if('e' == TOKEN_CHAR(10, 0))
    {
        enable = ENABLED;
    }
    else
    {
        enable = DISABLED;
    }

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
    ret = rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    if (ACL_PHASE_IGR_ACL == phase)
    {
        action.igr_acl.fwd_data.info.copy_redirect_port.skip_igrStpDrop = enable;
        action.igr_acl.fwd_data.info.copy_redirect_portMsk.skip_igrStpDrop = enable;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}
#endif

/*
*  acl set entry phase <UINT:phase> entry <UINT:index> action forward vlan-leaky ( disable | enable )
*/
#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_FORWARD_VLAN_LEAKY_DISABLE_ENABLE
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_forward_vlan_leaky_disable_enable(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
    ret = rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    if (ACL_PHASE_IGR_ACL == phase)
    {
        action.igr_acl.fwd_data.fwd_type = ACL_IGR_ACTION_FWD_VLAN_LEAKY;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_ROUTING_UNICAST_NEXT_HOP_INDEX
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action routing unicast <UINT:next_hop_index>
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_routing_unicast_next_hop_index(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr,
    uint32_t  *next_hop_index_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    ret = rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    switch (phase)
    {
        #if (defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL8390))
        case ACL_PHASE_IGR_ACL:
            osal_memset(&action.igr_acl.fwd_data, 0, sizeof(rtk_acl_igrActionFwd_t));
            action.igr_acl.fwd_data.fwd_type = ACL_IGR_ACTION_FWD_UNICAST_ROUTING;
            #if defined(CONFIG_SDK_RTL8380)
            if(DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
            {
                action.igr_acl.fwd_data.info.route.idx = *next_hop_index_ptr;
            }
            #endif  /* defined(CONFIG_SDK_RTL8380) */
            #if defined(CONFIG_SDK_RTL8390)
            if(DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
            {
                action.igr_acl.fwd_data.fwd_info = *next_hop_index_ptr;
            }
            #endif  /* defined(CONFIG_SDK_RTL8390) */
            break;
        #endif  /* (defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL8390)) */
        #if (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310))
        case ACL_PHASE_VACL:
            osal_memset(&action.vact.fwd_data, 0, sizeof(rtk_acl_vActFwd_t));
            action.vact.fwd_data.fwd_type = ACL_ACTION_FWD_UNICAST_ROUTING;
            action.vact.fwd_data.fwd_info = *next_hop_index_ptr;
            break;
        #endif  /* (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)) */
        default:
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_entry_action_routing_unicast_next_hop_entry_index */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_DEFAULT_ROUTING_UNICAST_NEXT_HOP_INDEX
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action default routing unicast <UINT:next_hop_index>
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_default_routing_unicast_next_hop_index(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr,
    uint32_t  *next_hop_index_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    ret = rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    switch (phase)
    {
        #if (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310))
        case ACL_PHASE_VACL:
            osal_memset(&action.vact.fwd_data, 0, sizeof(rtk_acl_vActFwd_t));
            action.vact.fwd_data.fwd_type = ACL_ACTION_FWD_DFLT_UNICAST_ROUTING;
            action.vact.fwd_data.fwd_info = *next_hop_index_ptr;
            break;
        #endif  /* (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)) */
        default:
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf(" doesn't support the action\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_entry_action_routing_unicast_next_hop_entry_index */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_FORWARD_CPU_PACKET_FORMAT_ORIGINAL_MODIFIED
/*
* acl set entry phase <UINT:phase> entry <UINT:index> action forward cpu-packet-format ( original | modified )
*/
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_forward_cpu_packet_format_original_modified(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    uint32              fmt;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
#if defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        if (ACL_PHASE_EACL == phase)
        {
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
        }
    }
#endif

    if('o' == TOKEN_CHAR(10, 0))
    {
        fmt = ORIGINAL_PACKET;
    }
    else
    {
        fmt = MODIFIED_PACKET;
    }

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    ret = rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    if (ACL_PHASE_VACL == phase)
    {
        action.vact.fwd_data.fwd_cpu_fmt = fmt;
    }
    else
    {
        action.iact.fwd_data.fwd_cpu_fmt = fmt;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_FORWARD_PRECEDENCE_SELECT_DROP_SELECT_FORWARD
/*
* acl set entry phase <UINT:phase> entry <UINT:index> action forward precedence ( select-drop | select-forward )
*/
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_forward_precedence_select_drop_select_forward(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
#if defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        if (ACL_PHASE_EACL == phase)
        {
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
        }
    }
#endif

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    ret = rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    if('d' == TOKEN_CHAR(10, 7))
    {
        switch (phase)
        {
            case ACL_PHASE_VACL:
                action.vact.fwd_data.flags &= (~RTK_ACL_FWD_FLAG_OVERWRITE_DROP);
                break;
            case ACL_PHASE_IACL:
                action.iact.fwd_data.flags &= (~RTK_ACL_FWD_FLAG_OVERWRITE_DROP);
                break;
            default:
                DIAG_ERR_PRINT(ret);
                return CPARSER_NOT_OK;
        }
    }
    else
    {
        switch (phase)
        {
            case ACL_PHASE_VACL:
                action.vact.fwd_data.flags |= RTK_ACL_FWD_FLAG_OVERWRITE_DROP;
                break;
            case ACL_PHASE_IACL:
                action.iact.fwd_data.flags |= RTK_ACL_FWD_FLAG_OVERWRITE_DROP;
                break;
            default:
                DIAG_ERR_PRINT(ret);
                return CPARSER_NOT_OK;
        }
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_FORWARD_SA_LEARN_NULL_NOT_LEARN
/*
* acl set entry phase <UINT:phase> entry <UINT:index> action forward sa-learn ( null | not-learn )
*/
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_forward_sa_learn_null_not_learn(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
    if (ACL_PHASE_VACL != phase)
    {
        diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
        diag_util_printf("doesn't support the action\n");
        return CPARSER_NOT_OK;
    }

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    ret = rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    if('u' == TOKEN_CHAR(10, 1))
    {
        action.vact.fwd_data.flags &= (~RTK_ACL_FWD_FLAG_SA_NOT_LEARN);
    }
    else
    {
        action.vact.fwd_data.flags |= RTK_ACL_FWD_FLAG_SA_NOT_LEARN;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_STATISTIC_STATE_DISABLE_ENABLE
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action statistic state ( disable | enable )
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_statistic_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    rtk_enable_t        enable;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
    ret = rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    if('e' == TOKEN_CHAR(10, 0))
    {
        enable = ENABLED;
    }
    else
    {
        enable = DISABLED;
    }

    switch (phase)
    {
        #if (defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL8390))
        case ACL_PHASE_IGR_ACL:
            action.igr_acl.stat_en = enable;
            break;
        #endif  /* (defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL8390)) */
        #if defined(CONFIG_SDK_RTL8390)
        case ACL_PHASE_EGR_ACL:
            action.egr_acl.stat_en = enable;
            break;
        #endif  /* defined(CONFIG_SDK_RTL8390) */
        #if (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310))
        case ACL_PHASE_VACL:
            action.vact.stat_en = enable;
            break;
        case ACL_PHASE_IACL:
            action.iact.stat_en = enable;
            break;
        #endif  /* (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)) */
        #if defined(CONFIG_SDK_RTL9310)
        case ACL_PHASE_EACL:
            action.eact.stat_en = enable;
            break;
        #endif  /* defined(CONFIG_SDK_RTL9310) */
        default:
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_entry_action_statistic_state_disable_enable */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_FILTERING_PORT_PORT
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action filtering port <UINT:port>
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_filtering_port_port(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr,
    uint32_t  *port_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
    ret = rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    if (ACL_PHASE_IGR_ACL == phase)
    {
        action.igr_acl.filter_data.flt_act = ACL_IGR_ACTION_FLT_SINGLE_PORT;
        action.igr_acl.filter_data.flt_info = *port_ptr;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_entry_action_filtering_portmask_index */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_FILTERING_PORTMASK_INDEX_PORTMASK_INDEX
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action filtering portmask-index <UINT:portmask_index>
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_filtering_portmask_index_portmask_index(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr,
    uint32_t  *portmask_index_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
#if defined(CONFIG_SDK_RTL8390)
    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) ||
            DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
        if (ACL_PHASE_EGR_ACL != phase)
        {
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
        }
    }
#endif
#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) ||
            DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID) ||
            DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) ||
            DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        if (ACL_PHASE_IACL != phase)
        {
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
        }
    }
#endif

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    ret = rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    switch (phase)
    {
        #if defined(CONFIG_SDK_RTL8380)
        case ACL_PHASE_IGR_ACL:
            action.igr_acl.filter_data.flt_act = ACL_IGR_ACTION_FLT_MULTIPLE_PORTS;
            action.igr_acl.filter_data.flt_info = *portmask_index_ptr;
            break;
        #endif  /* defined(CONFIG_SDK_RTL8380) */
        #if defined(CONFIG_SDK_RTL8390)
        case ACL_PHASE_EGR_ACL:
            action.egr_acl.fwd_data.fwd_type = ACL_EGR_ACTION_FWD_FILTERING;
            action.egr_acl.fwd_data.fwd_info = *portmask_index_ptr;
            break;
        #endif  /* defined(CONFIG_SDK_RTL8390) */
        #if (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310))
        case ACL_PHASE_IACL:
            action.iact.fwd_data.fwd_type = ACL_ACTION_FWD_FILTERING;
            action.iact.fwd_data.fwd_info = *portmask_index_ptr;
            break;
        #endif  /* (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)) */
        default:
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_entry_action_filtering_portmask_index */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_FILTER_STATE_DISABLE_ENABLE
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action filter state ( disable | enable )
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_filter_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    rtk_enable_t        enable;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
    ret = rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    if('e' == TOKEN_CHAR(10, 0))
    {
        enable = ENABLED;
    }
    else
    {
        enable = DISABLED;
    }

    if (ACL_PHASE_IGR_ACL == phase)
    {
        action.igr_acl.filter_en = enable;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_entry_action_statistic_state_disable_enable */
#endif

#ifdef CMD_ACL_SET_LOOPBACK_STATE_DISABLE_ENABLE
/*
 * acl set loopback state ( disable | enable )
 */
cparser_result_t
cparser_cmd_acl_set_loopback_state_disable_enable(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret;
    rtk_enable_t  enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if('e' == TOKEN_CHAR(4, 0))
    {
        enable = ENABLED;
    }
    else
    {
        enable = DISABLED;
    }

    ret = rtk_acl_loopBackEnable_set(unit, enable);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_loopback_state_disable_enable */
#endif

#ifdef CMD_ACL_SET_LIMIT_LOOPBACK_TIMES
/*
 * acl set limit-loopback <UINT:times>
 */
cparser_result_t
cparser_cmd_acl_set_limit_loopback_times(
    cparser_context_t *context,
    uint32_t *times_ptr)
{
    uint32  unit;
    int32   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    ret = rtk_acl_limitLoopbackTimes_set(unit, *times_ptr);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_limit_loopback_times */
#endif

#ifdef CMD_ACL_GET_LOOPBACK
/*
 * acl get loopback
 */
cparser_result_t
cparser_cmd_acl_get_loopback(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret;
    uint32        lb_times;
    #if defined(CONFIG_SDK_RTL9300)
    rtk_enable_t  enable;
    #endif  /* CONFIG_SDK_RTL9300 */

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    #if defined(CONFIG_SDK_RTL9300)
    if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID))
    {
        ret = rtk_acl_loopBackEnable_get(unit, &enable);
        if (RT_ERR_OK != ret)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }
    #endif  /* CONFIG_SDK_RTL9300 */

    ret = rtk_acl_limitLoopbackTimes_get(unit, &lb_times);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    #if defined(CONFIG_SDK_RTL9300)
    if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID))
    {
        diag_util_mprintf("Global loopback state: %10s\n",(ENABLED == enable)?"Enable":"Disable");
    }
    #endif  /* CONFIG_SDK_RTL9300 */
    diag_util_mprintf("Maximum loopback times: %5u\n",lb_times);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_get_loopback_state */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_LOOPBACK_DISABLE_ENABLE
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action loopback ( disable | enable )
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_loopback_disable_enable(
    cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *index_ptr)
{
    uint32  unit;
    int32   ret;
    rtk_acl_action_t    action;
    rtk_enable_t        enable;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
#if defined(CONFIG_SDK_RTL9300)
    if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID))
    {
        if (ACL_PHASE_IACL != phase)
        {
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
        }
    }
#endif
#if defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        if (ACL_PHASE_EACL == phase)
        {
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
        }
    }
#endif

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    ret = rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    if('e' == TOKEN_CHAR(9, 0))
    {
        enable = ENABLED;
    }
    else
    {
        enable = DISABLED;
    }

    if (ENABLED == enable)
    {
        if (ACL_PHASE_VACL == phase)
        {
            action.vact.fwd_en = ENABLED;
            action.vact.fwd_data.fwd_type = ACL_ACTION_FWD_LOOPBACK;
        }
        else
        {
            action.iact.fwd_en = ENABLED;
            action.iact.fwd_data.fwd_type = ACL_ACTION_FWD_LOOPBACK;
        }
    }
    else
    {
        if (ACL_PHASE_VACL == phase)
        {
            if(ACL_ACTION_FWD_LOOPBACK == action.vact.fwd_data.fwd_type)
            {
                action.vact.fwd_en = DISABLED;
                action.vact.fwd_data.fwd_type = ACL_ACTION_FWD_PERMIT;//restore default
            }
        }
        else
        {
            if(ACL_ACTION_FWD_LOOPBACK == action.iact.fwd_data.fwd_type)
            {
                action.iact.fwd_en = DISABLED;
                action.iact.fwd_data.fwd_type = ACL_ACTION_FWD_PERMIT;//restore default
            }
        }
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_phase_entry_index_action_loopback_disable_enable */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_STATISTIC_PACKET32_COUNTER_INDEX
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action statistic packet32 <UINT:counter_index>
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_statistic_packet32_counter_index(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr,
    uint32_t  *counter_index_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
    ret = rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    if (ACL_PHASE_IGR_ACL == phase)
    {
        action.igr_acl.stat_data.stat_type = STAT_TYPE_PACKET_BASED_32BIT;
        action.igr_acl.stat_data.stat_idx = *counter_index_ptr;
    }
#if defined(CONFIG_SDK_RTL8390)
    else
    {
        action.egr_acl.stat_data.stat_type = STAT_TYPE_PACKET_BASED_32BIT;
        action.egr_acl.stat_data.stat_idx = *counter_index_ptr;
    }
#endif
    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_entry_action_statistic_packet32_counter_index */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_STATISTIC_BYTE64_COUNTER_INDEX
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action statistic byte64 <UINT:counter_index>
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_statistic_byte64_counter_index(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr,
    uint32_t  *counter_index_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
    ret = rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    if (ACL_PHASE_IGR_ACL == phase)
    {
        action.igr_acl.stat_data.stat_type = STAT_TYPE_BYTE_BASED_64BIT;
        action.igr_acl.stat_data.stat_idx = *counter_index_ptr;
    }
#if defined(CONFIG_SDK_RTL8390)
    else
    {
        action.egr_acl.stat_data.stat_type = STAT_TYPE_BYTE_BASED_64BIT;
        action.egr_acl.stat_data.stat_idx = *counter_index_ptr;
    }
#endif
    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_entry_action_statistic_byte64_counter_index */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_STATISTIC_PACKET32_BYTE64
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action statistic ( packet32 | byte64 )
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_statistic_packet32_byte64(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    rtk_acl_statType_t  type;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
    ret = rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    if('p' == TOKEN_CHAR(9, 0))
    {
        type = STAT_TYPE_PACKET_BASED_32BIT;
    }
    else
    {
        type = STAT_TYPE_BYTE_BASED_64BIT;
    }

    if (ACL_PHASE_VACL == phase)
    {
        action.vact.stat_data.stat_type = type;
    }
    else
    {
        action.iact.stat_data.stat_type = type;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_entry_action_statistic_byte64_counter_index */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_MIRROR_STATE_DISABLE_ENABLE
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action mirror state ( disable | enable )
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_mirror_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    rtk_enable_t        enable;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
#if defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        if (ACL_PHASE_EACL == phase)
        {
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
        }
    }
#endif

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    ret = rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    if('e' == TOKEN_CHAR(10, 0))
    {
        enable = ENABLED;
    }
    else
    {
        enable = DISABLED;
    }

    switch (phase)
    {
        #if (defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL8390))
        case ACL_PHASE_IGR_ACL:
            action.igr_acl.mirror_en = enable;
            break;
        #endif  /* (defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL8390)) */
        #if defined(CONFIG_SDK_RTL8390)
        case ACL_PHASE_EGR_ACL:
            action.egr_acl.mirror_en = enable;
            break;
        #endif  /* defined(CONFIG_SDK_RTL8390) */
        #if (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310))
        case ACL_PHASE_VACL:
            action.vact.mirror_en = enable;
            break;
        case ACL_PHASE_IACL:
            action.iact.mirror_en = enable;
            break;
        #endif  /* (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)) */
        default:
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_entry_action_mirror_state_disable_enable */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_MIRROR_MIRROR_INDEX_ORIGINAL_MODIFIED
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action mirror <UINT:mirror_index> ( original | modified )
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_mirror_mirror_index_original_modified(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr,
    uint32_t  *mirror_index_ptr)
{
    uint32                      unit;
    int32                       ret;
    rtk_acl_action_t            action;
    rtk_acl_actionMirrorType_t  type;
    rtk_acl_phase_t             phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
#if defined(CONFIG_SDK_RTL8380)
    if(DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
    {
        if (ACL_PHASE_IGR_ACL != phase)
        {
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
        }
    }
#endif
#if defined(CONFIG_SDK_RTL8390)
    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
        if (ACL_PHASE_IGR_ACL == phase)
        {
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
        }
    }
#endif
#if defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        if (ACL_PHASE_EACL == phase)
        {
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
        }
    }
#endif

    if('o' == TOKEN_CHAR(10, 0))
    {
        type = ACL_ACTION_MIRROR_ORIGINAL;
    }
    else
    {
        #if defined(CONFIG_SDK_RTL9310) || defined(CONFIG_SDK_RTL9300)
        if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID))
        {
            if (ACL_PHASE_VACL == phase)
            {
                diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
                diag_util_printf("doesn't support the action\n");
                return CPARSER_NOT_OK;
            }
        }
        #endif
        type = ACL_ACTION_MIRROR_MODIFIED;
    }

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    ret = rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    switch (phase)
    {
        #if (defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL8390))
        case ACL_PHASE_IGR_ACL:
            action.igr_acl.mirror_data.mirror_type = type;
            action.igr_acl.mirror_data.mirror_set_idx = *mirror_index_ptr;
            break;
        #endif  /* (defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL8390)) */
        #if defined(CONFIG_SDK_RTL8390)
        case ACL_PHASE_EGR_ACL:
            action.egr_acl.mirror_data.mirror_type = type;
            action.egr_acl.mirror_data.mirror_set_idx = *mirror_index_ptr;
            break;
        #endif  /* defined(CONFIG_SDK_RTL8390) */
        #if (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310))
        case ACL_PHASE_VACL:
            action.vact.mirror_data.mirror_type = type;
            action.vact.mirror_data.mirror_set_idx = *mirror_index_ptr;
            break;
        case ACL_PHASE_IACL:
            action.iact.mirror_data.mirror_type = type;
            action.iact.mirror_data.mirror_set_idx = *mirror_index_ptr;
            break;
        #endif  /* (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)) */
        default:
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_entry_action_mirror_mirror_index_original_modified */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_MIRROR_CANCEL
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action mirror cancel
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_mirror_cancel(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr)
{
    uint32                      unit;
    int32                       ret;
    rtk_acl_action_t            action;
    rtk_acl_actionMirrorType_t  type;
    rtk_acl_phase_t             phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
#if defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        if (ACL_PHASE_EACL == phase)
        {
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
        }
    }
#endif

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    type = ACL_ACTION_MIRROR_CANCEL;

    ret = rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    switch (phase)
    {
        case ACL_PHASE_VACL:
            action.vact.mirror_data.mirror_type = type;
            break;
        case ACL_PHASE_IACL:
            action.iact.mirror_data.mirror_type = type;
            break;
        default:
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_entry_action_mirror_cancel */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_METER_STATE_DISABLE_ENABLE
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action meter state ( disable | enable )
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_meter_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    rtk_enable_t        enable;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
    ret = rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    if('e' == TOKEN_CHAR(10, 0))
    {
        enable = ENABLED;
    }
    else
    {
        enable = DISABLED;
    }

    switch (phase)
    {
        #if (defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL8390))
        case ACL_PHASE_IGR_ACL:
            action.igr_acl.meter_en = enable;
            break;
        #endif  /* (defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL8390)) */
        #if defined(CONFIG_SDK_RTL8390)
        case ACL_PHASE_EGR_ACL:
            action.egr_acl.meter_en = enable;
            break;
        #endif  /* defined(CONFIG_SDK_RTL8390) */
        #if (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310))
        case ACL_PHASE_VACL:
            action.vact.meter_en = enable;
            break;
        case ACL_PHASE_IACL:
            action.iact.meter_en = enable;
            break;
        #endif  /* (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)) */
        #if defined(CONFIG_SDK_RTL9310)
        case ACL_PHASE_EACL:
            action.eact.meter_en = enable;
            break;
        #endif  /* defined(CONFIG_SDK_RTL9310) */
        default:
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_entry_action_meter_state_disable_enable */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_METER_METER_INDEX
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action meter <UINT:meter_index>
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_meter_meter_index(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr,
    uint32_t  *meter_index_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
    ret = rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    switch (phase)
    {
        #if (defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL8390))
        case ACL_PHASE_IGR_ACL:
            action.igr_acl.meter_data.meter_idx = *meter_index_ptr;
            break;
        #endif  /* (defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL8390)) */
        #if defined(CONFIG_SDK_RTL8390)
        case ACL_PHASE_EGR_ACL:
            action.egr_acl.meter_data.meter_idx = *meter_index_ptr;
            break;
        #endif  /* defined(CONFIG_SDK_RTL8390) */
        #if (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310))
        case ACL_PHASE_VACL:
            action.vact.meter_data.meter_idx = *meter_index_ptr;
            break;
        case ACL_PHASE_IACL:
            action.iact.meter_data.meter_idx = *meter_index_ptr;
            break;
        #endif  /* (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)) */
        #if defined(CONFIG_SDK_RTL9310)
        case ACL_PHASE_EACL:
            action.eact.meter_data.meter_idx = *meter_index_ptr;
            break;
        #endif  /* defined(CONFIG_SDK_RTL9310) */
        default:
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_entry_action_meter_meter_index */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_VLAN_XLATE_INNER_OUTER_STATE_DISABLE_ENABLE
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action vlan-xlate ( inner | outer ) state ( disable | enable )
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_vlan_xlate_inner_outer_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    rtk_enable_t        enable;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
    ret = rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    if('e' == TOKEN_CHAR(11, 0))
    {
        enable = ENABLED;
    }
    else
    {
        enable = DISABLED;
    }

    switch (phase)
    {
        #if (defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL8390))
        case ACL_PHASE_IGR_ACL:
            if('i' == TOKEN_CHAR(9, 0))
            {
                action.igr_acl.inner_vlan_assign_en = enable;
            }
            else
            {
                action.igr_acl.outer_vlan_assign_en = enable;
            }
            break;
        #endif  /* (defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL8390)) */
        #if defined(CONFIG_SDK_RTL8390)
        case ACL_PHASE_EGR_ACL:
            if('i' == TOKEN_CHAR(9, 0))
            {
                action.egr_acl.inner_vlan_assign_en = enable;
            }
            else
            {
                action.egr_acl.outer_vlan_assign_en = enable;
            }
            break;
        #endif  /* defined(CONFIG_SDK_RTL8390) */
        #if (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310))
        case ACL_PHASE_VACL:
            if('i' == TOKEN_CHAR(9, 0))
            {
                action.vact.inner_vlan_assign_en = enable;
            }
            else
            {
                action.vact.outer_vlan_assign_en = enable;
            }
            break;
        case ACL_PHASE_IACL:
            if('i' == TOKEN_CHAR(9, 0))
            {
                action.iact.inner_vlan_assign_en = enable;
            }
            else
            {
                action.iact.outer_vlan_assign_en = enable;
            }
            break;
        #endif  /* (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)) */
        #if defined(CONFIG_SDK_RTL9310)
        case ACL_PHASE_EACL:
            if('i' == TOKEN_CHAR(9, 0))
            {
                action.eact.inner_vlan_assign_en = enable;
            }
            else
            {
                action.eact.outer_vlan_assign_en = enable;
            }
            break;
        #endif  /* defined(CONFIG_SDK_RTL9310) */
        default:
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_entry_action_vlan_xlate_inner_outer_state_disable_enable */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_VLAN_XLATE_INNER_TPID_STATE_DISABLE_ENABLE
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action vlan-xlate inner-tpid state ( disable | enable )
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_vlan_xlate_inner_tpid_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    rtk_enable_t        enable;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
#if defined(CONFIG_SDK_RTL8390)
    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) ||
            DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
        if (ACL_PHASE_IGR_ACL == phase)
        {
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
        }
    }
#endif  /* defined(CONFIG_SDK_RTL8390) */
#if (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310))
    if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) ||
            DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        if (ACL_PHASE_VACL == phase)
        {
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
        }
    }
#endif  /* (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)) */

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action), ret);

    if('e' == TOKEN_CHAR(11, 0))
    {
        enable = ENABLED;
    }
    else
    {
        enable = DISABLED;
    }

    switch (phase)
    {
        #if defined(CONFIG_SDK_RTL8380)
        case ACL_PHASE_IGR_ACL:
            action.igr_acl.itpid_en = enable;
            break;
        #endif  /* defined(CONFIG_SDK_RTL8380) */
        #if defined(CONFIG_SDK_RTL8390)
        case ACL_PHASE_EGR_ACL:
            action.egr_acl.inner_vlan_data.tpid_assign = enable;
            break;
        #endif  /* defined(CONFIG_SDK_RTL8390) */
        #if (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310))
        case ACL_PHASE_IACL:
            action.iact.inner_vlan_data.tpid_assign = enable;
            break;
        #endif  /* (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)) */
        #if defined(CONFIG_SDK_RTL9310)
        case ACL_PHASE_EACL:
            action.eact.inner_vlan_data.tpid_assign = enable;
            break;
        #endif  /* defined(CONFIG_SDK_RTL9310) */
        default:
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_entry_action_vlan_xlate_inner_tpid_state_disable_enable */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_VLAN_XLATE_INNER_TPID_INDEX_TPID_INDEX
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action vlan-xlate inner-tpid-index <UINT:tpid_index>
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_vlan_xlate_inner_tpid_index_tpid_index(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr,
    uint32_t  *tpid_index_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
#if defined(CONFIG_SDK_RTL8390)
    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) ||
            DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
        if (ACL_PHASE_IGR_ACL == phase)
        {
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
        }
    }
#endif  /* defined(CONFIG_SDK_RTL8390) */

#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) ||
            DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        if (ACL_PHASE_VACL == phase)
        {
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
        }
    }
#endif  /* defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310) */

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action), ret);

    switch (phase)
    {
        #if defined(CONFIG_SDK_RTL8380)
        case ACL_PHASE_IGR_ACL:
            action.igr_acl.itpid_data.tpid_idx = *tpid_index_ptr;
            break;
        #endif  /* defined(CONFIG_SDK_RTL8380) */
        #if defined(CONFIG_SDK_RTL8390)
        case ACL_PHASE_EGR_ACL:
            action.egr_acl.inner_vlan_data.tpid_idx = *tpid_index_ptr;
            break;
        #endif  /* defined(CONFIG_SDK_RTL8390) */
        #if (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310))
        case ACL_PHASE_IACL:
            action.iact.inner_vlan_data.tpid_idx = *tpid_index_ptr;
            break;
        #endif  /* (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)) */
        #if defined(CONFIG_SDK_RTL9310)
        case ACL_PHASE_EACL:
            action.eact.inner_vlan_data.tpid_idx = *tpid_index_ptr;
            break;
        #endif  /* defined(CONFIG_SDK_RTL9310) */
        default:
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_entry_action_vlan_xlate_inner_tpid_tpid_index */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_VLAN_XLATE_INNER_VID_SHIFT_POSITIVE_SHIFT_NEGATIVE_VALUE
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action vlan-xlate inner-vid ( shift-positive | shift-negative ) <UINT:value>
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_vlan_xlate_inner_vid_shift_positive_shift_negative_value(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr,
    uint32_t  *value_ptr)
{
    uint32                          unit;
    uint32                          shift_sel = 0;
    int32                           ret;
    rtk_acl_action_t                action;
    rtk_acl_actionVlanAssignType_t  type;
    rtk_acl_phase_t                 phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action), ret);

    if('a' == TOKEN_CHAR(10, 0))
    {
        type = ACL_ACTION_VLAN_ASSIGN_NEW_VID;
    }
    else if (0 == osal_strcmp(TOKEN_STR(10), "shift-positive"))
    {
        type = ACL_ACTION_VLAN_ASSIGN_SHIFT_VID;
    }
    else if (0 == osal_strcmp(TOKEN_STR(10), "shift-negative"))
    {
        type = ACL_ACTION_VLAN_ASSIGN_SHIFT_VID;
        shift_sel = 1;
    }
    else
    {
        type = ACL_ACTION_VLAN_ASSIGN_SHIFT_FROM_OUTER_VID;
    }

    if (ACL_PHASE_IGR_ACL == phase)
    {
        action.igr_acl.inner_vlan_data.vid_assign_type = type;
        action.igr_acl.inner_vlan_data.vid_shift_sel = shift_sel;
        action.igr_acl.inner_vlan_data.vid_value = *value_ptr;
    }
#if defined(CONFIG_SDK_RTL8390)
    else
    {
        action.egr_acl.inner_vlan_data.vid_assign_type = type;
        action.egr_acl.inner_vlan_data.vid_shift_sel = shift_sel;
        action.egr_acl.inner_vlan_data.vid_value = *value_ptr;
    }
#endif
    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_phase_entry_index_action_vlan_xlate_inner_vid_shift_positive_shift_negative_value */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_VLAN_XLATE_INNER_VID_ASSIGN_SHIFT_SHIFT_FROM_OUTER_VALUE
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action vlan-xlate inner-vid ( assign | shift | shift-from-outer ) <UINT:value>
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_vlan_xlate_inner_vid_assign_shift_shift_from_outer_value(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr,
    uint32_t  *value_ptr)
{
    uint32                          unit;
    int32                           ret;
    rtk_acl_action_t                action;
    rtk_acl_actionVlanAssignType_t  type;
    rtk_acl_phase_t                 phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action), ret);

    if('a' == TOKEN_CHAR(10, 0))
    {
        type = ACL_ACTION_VLAN_ASSIGN_NEW_VID;
    }
    else if (0 == osal_strcmp(TOKEN_STR(10), "shift"))
    {
        type = ACL_ACTION_VLAN_ASSIGN_SHIFT_VID;
    }
    else
    {
        type = ACL_ACTION_VLAN_ASSIGN_SHIFT_FROM_OUTER_VID;
    }

    switch (phase)
    {
        #if (defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL8390))
        case ACL_PHASE_IGR_ACL:
            action.igr_acl.inner_vlan_data.vid_assign_type = type;
            action.igr_acl.inner_vlan_data.vid_value = *value_ptr;
            break;
        #endif  /* (defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL8390)) */
        #if defined(CONFIG_SDK_RTL8390)
        case ACL_PHASE_EGR_ACL:
            action.egr_acl.inner_vlan_data.vid_assign_type = type;
            action.egr_acl.inner_vlan_data.vid_value = *value_ptr;
            break;
        #endif  /* defined(CONFIG_SDK_RTL8390) */
        #if (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310))
        case ACL_PHASE_VACL:
            action.vact.inner_vlan_data.vid_assign_type = type;
            action.vact.inner_vlan_data.vid_value = *value_ptr;
            break;
        case ACL_PHASE_IACL:
            action.iact.inner_vlan_data.vid_assign_type = type;
            action.iact.inner_vlan_data.vid_value = *value_ptr;
            break;
        #endif  /* (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)) */
        #if defined(CONFIG_SDK_RTL9310)
        case ACL_PHASE_EACL:
            action.eact.inner_vlan_data.vid_assign_type = type;
            action.eact.inner_vlan_data.vid_value = *value_ptr;
            break;
        #endif  /* defined(CONFIG_SDK_RTL9310) */
        default:
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_phase_entry_index_action_vlan_xlate_inner_vid_assign_shift_shift_from_outer_value */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_VLAN_XLATE_INNER_VID_ASSIGN_PORT_BASED
/*
 *    acl set entry phase <UINT:phase> entry <UINT:index> action vlan-xlate inner-vid assign-port-based
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_vlan_xlate_inner_vid_assign_port_based(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action), ret);

    if (ACL_PHASE_IGR_ACL == phase)
    {
        action.igr_acl.inner_vlan_data.vid_assign_type = ACL_IGR_ACTION_IVLAN_ASSIGN_PORT_BASED_INNER_VID;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_phase_entry_index_action_vlan_xlate_inner_vid_assign_port_based */
#endif


#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_VLAN_XLATE_INNER_VID_COPY_FROM_OUTER
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action vlan-xlate inner-vid copy-from-outer
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_vlan_xlate_inner_vid_copy_from_outer(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
    if (ACL_PHASE_IGR_ACL == phase)
    {
        diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
        diag_util_printf("doesn't support the action\n");
        return CPARSER_NOT_OK;
    }

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action), ret);

    action.egr_acl.inner_vlan_data.vid_assign_type =
            ACL_EGR_ACTION_IVLAN_ASSIGN_COPY_FROM_OUTER_VID;

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_entry_action_vlan_xlate_inner_vid_copy_from_outer */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_VLAN_XLATE_OUTER_TPID_STATE_DISABLE_ENABLE
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action vlan-xlate outer-tpid state ( disable | enable )
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_vlan_xlate_outer_tpid_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    rtk_enable_t        enable;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
#if defined(CONFIG_SDK_RTL8390)
    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) ||
            DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
        if (ACL_PHASE_IGR_ACL == phase)
        {
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
        }
    }
#endif
#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) ||
            DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        if (ACL_PHASE_VACL == phase)
        {
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
        }
    }
#endif  /* defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310) */

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action), ret);

    if('e' == TOKEN_CHAR(11, 0))
    {
        enable = ENABLED;
    }
    else
    {
        enable = DISABLED;
    }
#if defined(CONFIG_SDK_RTL8390)
    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
        action.egr_acl.outer_vlan_data.tpid_assign = enable;
    }
#endif

#if defined(CONFIG_SDK_RTL8380)
    if(DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
    {
        action.igr_acl.otpid_en = enable;
    }
#endif

#if defined(CONFIG_SDK_RTL9300)
    if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID))
    {
        action.iact.outer_vlan_data.tpid_assign = enable;
    }
#endif

#if defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        switch (phase)
        {
            case ACL_PHASE_IACL:
                action.iact.outer_vlan_data.tpid_assign = enable;
                break;
            case ACL_PHASE_EACL:
                action.eact.outer_vlan_data.tpid_assign = enable;
                break;
            default:
                DIAG_ERR_PRINT(ret);
                return CPARSER_NOT_OK;
        }
    }
#endif

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_entry_action_vlan_xlate_outer_tpid_state_disable_enable */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_VLAN_XLATE_OUTER_TPID_INDEX_TPID_INDEX
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action vlan-xlate outer-tpid-index <UINT:tpid_index>
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_vlan_xlate_outer_tpid_index_tpid_index(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr,
    uint32_t  *tpid_index_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
#if defined(CONFIG_SDK_RTL8390)
    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) ||
            DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
        if (ACL_PHASE_IGR_ACL == phase)
        {
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
        }
    }
#endif
#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) ||
            DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        if (ACL_PHASE_VACL == phase)
        {
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
        }
    }
#endif  /* defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310) */

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action), ret);
#if defined(CONFIG_SDK_RTL8390)
    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
        action.egr_acl.outer_vlan_data.tpid_idx = *tpid_index_ptr;
    }
#endif

#if defined(CONFIG_SDK_RTL8380)
    if(DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
    {
        action.igr_acl.otpid_data.tpid_idx = *tpid_index_ptr;
    }
#endif
#if defined(CONFIG_SDK_RTL9300)
    if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID))
    {
        action.iact.outer_vlan_data.tpid_idx = *tpid_index_ptr;
    }
#endif
#if defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        switch (phase)
        {
            case ACL_PHASE_IACL:
                action.iact.outer_vlan_data.tpid_idx = *tpid_index_ptr;
                break;
            case ACL_PHASE_EACL:
                action.eact.outer_vlan_data.tpid_idx = *tpid_index_ptr;
                break;
            default:
                DIAG_ERR_PRINT(ret);
                return CPARSER_NOT_OK;
        }
    }
#endif

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_entry_action_vlan_xlate_outer_tpid_tpid_index */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_VLAN_XLATE_OUTER_VID_SHIFT_POSITIVE_SHIFT_NEGATIVE_VALUE
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action vlan-xlate outer-vid ( shift-positive | shift-negative ) <UINT:value>
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_vlan_xlate_outer_vid_shift_positive_shift_negative_value(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr,
    uint32_t  *value_ptr)
{
    uint32                          unit;
    uint32                          shift_sel = 0;
    int32                           ret;
    rtk_acl_action_t                action;
    rtk_acl_actionVlanAssignType_t  type;
    rtk_acl_phase_t                 phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action), ret);

    if('a' == TOKEN_CHAR(10, 0))
    {
        type = ACL_ACTION_VLAN_ASSIGN_NEW_VID;
    }
    else if (0 == osal_strcmp(TOKEN_STR(10), "shift-positive"))
    {
        type = ACL_ACTION_VLAN_ASSIGN_SHIFT_VID;
    }
    else if (0 == osal_strcmp(TOKEN_STR(10), "shift-negative"))
    {
        type = ACL_ACTION_VLAN_ASSIGN_SHIFT_VID;
        shift_sel = 1;
    }
    else
    {
        type = ACL_ACTION_VLAN_ASSIGN_SHIFT_FROM_INNER_VID;
    }

    if (ACL_PHASE_IGR_ACL == phase)
    {
        action.igr_acl.outer_vlan_data.vid_assign_type = type;
        action.igr_acl.outer_vlan_data.vid_shift_sel = shift_sel;
        action.igr_acl.outer_vlan_data.vid_value = *value_ptr;
    }
#if defined(CONFIG_SDK_RTL8390)
    else
    {
        action.egr_acl.outer_vlan_data.vid_assign_type = type;
        action.egr_acl.outer_vlan_data.vid_shift_sel = shift_sel;
        action.egr_acl.outer_vlan_data.vid_value = *value_ptr;
    }
#endif

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_phase_entry_index_action_vlan_xlate_outer_vid_shift_positive_shift_negative_value */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_VLAN_XLATE_OUTER_VID_ASSIGN_SHIFT_SHIFT_FROM_INNER_VALUE
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action vlan-xlate outer-vid ( assign | shift | shift-from-inner ) <UINT:value>
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_vlan_xlate_outer_vid_assign_shift_shift_from_inner_value(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr,
    uint32_t  *value_ptr)
{
    uint32                          unit;
    int32                           ret;
    rtk_acl_action_t                action;
    rtk_acl_actionVlanAssignType_t  type;
    rtk_acl_phase_t                 phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action), ret);

    if('a' == TOKEN_CHAR(10, 0))
    {
        type = ACL_ACTION_VLAN_ASSIGN_NEW_VID;
    }
    else if (0 == osal_strcmp(TOKEN_STR(10), "shift"))
    {
        type = ACL_ACTION_VLAN_ASSIGN_SHIFT_VID;
    }
    else
    {
        type = ACL_ACTION_VLAN_ASSIGN_SHIFT_FROM_INNER_VID;
    }

    switch (phase)
    {
        #if (defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL8390))
        case ACL_PHASE_IGR_ACL:
            action.igr_acl.outer_vlan_data.vid_assign_type = type;
            action.igr_acl.outer_vlan_data.vid_value = *value_ptr;
            break;
        #endif  /* (defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL8390)) */
        #if defined(CONFIG_SDK_RTL8390)
        case ACL_PHASE_EGR_ACL:
            action.egr_acl.outer_vlan_data.vid_assign_type = type;
            action.egr_acl.outer_vlan_data.vid_value = *value_ptr;
            break;
        #endif  /* defined(CONFIG_SDK_RTL8390) */
        #if (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310))
        case ACL_PHASE_VACL:
            action.vact.outer_vlan_data.vid_assign_type = type;
            action.vact.outer_vlan_data.vid_value = *value_ptr;
            break;
        case ACL_PHASE_IACL:
            action.iact.outer_vlan_data.vid_assign_type = type;
            action.iact.outer_vlan_data.vid_value = *value_ptr;
            break;
        #endif  /* (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)) */
        #if defined(CONFIG_SDK_RTL9310)
        case ACL_PHASE_EACL:
            action.eact.outer_vlan_data.vid_assign_type = type;
            action.eact.outer_vlan_data.vid_value = *value_ptr;
            break;
        #endif  /* defined(CONFIG_SDK_RTL9310) */
        default:
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_entry_action_vlan_xlate_outer_vid_assign_shift_positive_shift_negative_shift_from_inner_value */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_VLAN_XLATE_OUTER_VID_ASSIGN_PORT_BASED
/*
 *   acl set entry phase <UINT:phase> entry <UINT:index> action vlan-xlate outer-vid assign-port-based
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_vlan_xlate_outer_vid_assign_port_based(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action), ret);

    if (ACL_PHASE_IGR_ACL == phase)
    {
        action.igr_acl.outer_vlan_data.vid_assign_type = ACL_IGR_ACTION_OVLAN_ASSIGN_PORT_BASED_OUTER_VID;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_entry_action_vlan_xlate_inner_vid_assign_shift_shift_from_outer_value */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_VLAN_XLATE_OUTER_VID_COPY_FROM_INNER
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action vlan-xlate outer-vid copy-from-inner
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_vlan_xlate_outer_vid_copy_from_inner(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
    if (ACL_PHASE_IGR_ACL == phase)
    {
        diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
        diag_util_printf("doesn't support the action\n");
        return CPARSER_NOT_OK;
    }

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action), ret);

    action.egr_acl.outer_vlan_data.vid_assign_type =
            ACL_ACTION_VLAN_ASSIGN_COPY_FROM_INNER_VID;

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_entry_action_vlan_xlate_outer_vid_copy_from_inner */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_INNER_PRIORITY_STATE_DISABLE_ENABLE
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action inner-priority state ( disable | enable )
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_inner_priority_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    rtk_enable_t        enable;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action), ret);

    if('e' == TOKEN_CHAR(10, 0))
    {
        enable = ENABLED;
    }
    else
    {
        enable = DISABLED;
    }

    switch (phase)
    {
        case ACL_PHASE_VACL:
            action.vact.inner_pri_en = enable;
            break;
        case ACL_PHASE_IACL:
            action.iact.inner_pri_en = enable;
            break;
        #if defined(CONFIG_SDK_RTL9310)
        case ACL_PHASE_EACL:
            action.eact.inner_pri_en = enable;
            break;
        #endif  /* defined(CONFIG_SDK_RTL9310) */
        default:
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_entry_action_vlan_xlate_inner_outer_state_disable_enable */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_OUTER_PRIORITY_STATE_DISABLE_ENABLE
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action outer-priority state ( disable | enable )
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_outer_priority_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    rtk_enable_t        enable;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action), ret);

    if('e' == TOKEN_CHAR(10, 0))
    {
        enable = ENABLED;
    }
    else
    {
        enable = DISABLED;
    }

    switch (phase)
    {
        case ACL_PHASE_VACL:
            action.vact.outer_pri_en = enable;
            break;
        case ACL_PHASE_IACL:
            action.iact.outer_pri_en = enable;
            break;
        #if defined(CONFIG_SDK_RTL9310)
        case ACL_PHASE_EACL:
            action.eact.outer_pri_en = enable;
            break;
        #endif  /* defined(CONFIG_SDK_RTL9310) */
        default:
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_entry_action_vlan_xlate_inner_outer_state_disable_enable */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_INNER_PRIORITY_ASSIGN_VALUE
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action inner-priority assign <UINT:value>
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_inner_priority_assign_value(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr,
    uint32_t  *value_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action), ret);

    switch (phase)
    {
        case ACL_PHASE_VACL:
            action.vact.inner_pri_data.act = ACL_ACTION_INNER_PRI_ASSIGN_NEW_PRI;
            action.vact.inner_pri_data.pri = *value_ptr;
            break;
        case ACL_PHASE_IACL:
            action.iact.inner_pri_data.act = ACL_ACTION_INNER_PRI_ASSIGN_NEW_PRI;
            action.iact.inner_pri_data.pri = *value_ptr;
            break;
        #if defined(CONFIG_SDK_RTL9310)
        case ACL_PHASE_EACL:
            action.eact.inner_pri_data.act = ACL_ACTION_INNER_PRI_ASSIGN_NEW_PRI;
            action.eact.inner_pri_data.pri = *value_ptr;
            break;
        #endif  /* defined(CONFIG_SDK_RTL9310) */
        default:
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_phase_entry_index_action_vlan_xlate_inner_vid_assign_shift_positive_shift_negative_shift_from_outer_value */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_OUTER_PRIORITY_ASSIGN_VALUE
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action outer-priority assign <UINT:value>
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_outer_priority_assign_value(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr,
    uint32_t  *value_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action), ret);

    switch (phase)
    {
        case ACL_PHASE_VACL:
            action.vact.outer_pri_data.act = ACL_ACTION_INNER_PRI_ASSIGN_NEW_PRI;
            action.vact.outer_pri_data.pri = *value_ptr;
            break;
        case ACL_PHASE_IACL:
            action.iact.outer_pri_data.act = ACL_ACTION_INNER_PRI_ASSIGN_NEW_PRI;
            action.iact.outer_pri_data.pri = *value_ptr;
            break;
        #if defined(CONFIG_SDK_RTL9310)
        case ACL_PHASE_EACL:
            action.eact.outer_pri_data.act = ACL_ACTION_INNER_PRI_ASSIGN_NEW_PRI;
            action.eact.outer_pri_data.pri = *value_ptr;
            break;
        #endif  /* defined(CONFIG_SDK_RTL9310) */
        default:
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_phase_entry_index_action_vlan_xlate_inner_vid_assign_shift_positive_shift_negative_shift_from_outer_value */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_INNER_PRIORITY_COPY_FROM_OUTER_KEEP
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action inner-priority ( copy-from-outer | keep )
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_inner_priority_copy_from_outer_keep(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr)
{
    uint32                  unit;
    int32                   ret;
    rtk_acl_action_t        action;
    rtk_acl_phase_t         phase;
    rtk_acl_innerPriAct_t   priAct;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action), ret);

    if('c' == TOKEN_CHAR(9, 0))
    {
        priAct = ACL_ACTION_INNER_PRI_COPY_FROM_OUTER;
    }
    else
    {
        priAct = ACL_ACTION_INNER_PRI_KEEP;
    }

    switch (phase)
    {
        case ACL_PHASE_VACL:
            action.vact.inner_pri_data.act = priAct;
            break;
        case ACL_PHASE_IACL:
            action.iact.inner_pri_data.act = priAct;
            break;
        #if defined(CONFIG_SDK_RTL9310)
        case ACL_PHASE_EACL:
            action.eact.inner_pri_data.act = priAct;
            break;
        #endif  /* defined(CONFIG_SDK_RTL9310) */
        default:
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_entry_action_vlan_xlate_inner_outer_state_disable_enable */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_OUTER_PRIORITY_COPY_FROM_INNER_KEEP
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action outer-priority ( copy-from-inner | keep )
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_outer_priority_copy_from_inner_keep(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr)
{
    uint32                  unit;
    int32                   ret;
    rtk_acl_action_t        action;
    rtk_acl_phase_t         phase;
    rtk_acl_innerPriAct_t   priAct;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action), ret);

    if('c' == TOKEN_CHAR(9, 0))
    {
        priAct = ACL_ACTION_INNER_PRI_COPY_FROM_INNER;
    }
    else
    {
        priAct = ACL_ACTION_INNER_PRI_KEEP;
    }

    switch (phase)
    {
        case ACL_PHASE_VACL:
            action.vact.outer_pri_data.act = priAct;
            break;
        case ACL_PHASE_IACL:
            action.iact.outer_pri_data.act = priAct;
            break;
        #if defined(CONFIG_SDK_RTL9310)
        case ACL_PHASE_EACL:
            action.eact.outer_pri_data.act = priAct;
            break;
        #endif  /* defined(CONFIG_SDK_RTL9310) */
        default:
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_entry_action_vlan_xlate_inner_outer_state_disable_enable */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_PRIORITY_STATE_DISABLE_ENABLE
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action priority state ( disable | enable )
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_priority_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    rtk_enable_t        enable;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
#if defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        if (ACL_PHASE_EACL == phase)
        {
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
        }
    }
#endif

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action), ret);

    if('e' == TOKEN_CHAR(10, 0))
    {
        enable = ENABLED;
    }
    else
    {
        enable = DISABLED;
    }

    switch (phase)
    {
        #if (defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL8390))
        case ACL_PHASE_IGR_ACL:
            action.igr_acl.pri_en = enable;
            break;
        #endif  /* (defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL8390)) */
        #if defined(CONFIG_SDK_RTL8390)
        case ACL_PHASE_EGR_ACL:
            action.egr_acl.pri_en = enable;
            break;
        #endif  /* defined(CONFIG_SDK_RTL8390) */
        #if (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310))
        case ACL_PHASE_VACL:
            action.vact.pri_en = enable;
            break;
        case ACL_PHASE_IACL:
            action.iact.pri_en = enable;
            break;
        #endif  /* (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)) */
        default:
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_entry_action_priority_state_disable_enable */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_PRIORITY_CPU_PORT_ONLY_STATE_DISABLE_ENABLE
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action priority cpu-port-only state ( disable | enable )
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_priority_cpu_port_only_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    rtk_enable_t        enable;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
#if defined(CONFIG_SDK_RTL8390)
    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
        if (ACL_PHASE_IGR_ACL == phase)
        {
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
        }
    }
#endif

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action), ret);

    if('e' == TOKEN_CHAR(11, 0))
    {
        enable = 1;
    }
    else
    {
        enable = 0;
    }
#if defined(CONFIG_SDK_RTL8390)
    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
        action.egr_acl.pri_data.pri_act = enable;
    }
#endif

#if defined(CONFIG_SDK_RTL8380)
    if(DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
    {
        action.igr_acl.cpu_pri_en = enable;
    }
#endif

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_entry_action_priority_cpu_port_only_state_disable_enable */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_PRIORITY_CPU_PORT_ONLY_PRIORITY
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action priority cpu-port-only <UINT:priority>
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_priority_cpu_port_only_priority(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr,
    uint32_t  *priority_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action), ret);

#if defined(CONFIG_SDK_RTL8380)
    if(DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
    {
        if (ACL_PHASE_IGR_ACL == phase)
        {
            action.igr_acl.cpu_pri_data.pri = *priority_ptr;
        }
    }
#endif

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_entry_action_priority_priority */
#endif


#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_PRIORITY_PRIORITY
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action priority <UINT:priority>
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_priority_priority(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr,
    uint32_t  *priority_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
#if defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        if (ACL_PHASE_EACL == phase)
        {
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
        }
    }
#endif

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action), ret);

    switch (phase)
    {
        #if (defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL8390))
        case ACL_PHASE_IGR_ACL:
            action.igr_acl.pri_data.pri = *priority_ptr;
            break;
        #endif  /* (defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL8390)) */
        #if defined(CONFIG_SDK_RTL8390)
        case ACL_PHASE_EGR_ACL:
            action.egr_acl.pri_data.pri = *priority_ptr;
            break;
        #endif  /* defined(CONFIG_SDK_RTL8390) */
        #if (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310))
        case ACL_PHASE_VACL:
            action.vact.pri_data.pri = *priority_ptr;
            break;
        case ACL_PHASE_IACL:
            action.iact.pri_data.pri = *priority_ptr;
            break;
        #endif  /* (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)) */
        default:
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_entry_action_priority_priority */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_MPLS_STATE_DISABLE_ENABLE
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action mpls state ( disable | enable )
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_mpls_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    rtk_enable_t        enable;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);

    if (ACL_PHASE_EGR_ACL == phase)
    {
        diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
        diag_util_printf("doesn't support the action\n");
        return CPARSER_NOT_OK;
    }

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action), ret);

    if('e' == TOKEN_CHAR(10, 0))
    {
        enable = ENABLED;
    }
    else
    {
        enable = DISABLED;
    }

    action.igr_acl.mpls_en = enable;

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_entry_action_mpls_state_disable_enable */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_MPLS_PUSH_OUTER_PUSH_INNER_OUTER_LIB_INDEX
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action mpls ( push-outer | push-inner-outer ) <UINT:lib_index>
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_mpls_push_outer_push_inner_outer_lib_index(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr,
    uint32_t  *lib_index_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);

    if (ACL_PHASE_EGR_ACL == phase)
    {
        diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
        diag_util_printf("doesn't support the action\n");
        return CPARSER_NOT_OK;
    }

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action), ret);

    if (0 == osal_strcmp(TOKEN_STR(9), "push-outer"))
    {
        action.igr_acl.mpls_data.mpls_act = 0;
    }
    else
    {
        action.igr_acl.mpls_data.mpls_act = 1;
    }

    action.igr_acl.mpls_data.mpls_idx = *lib_index_ptr;

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_entry_action_mpls_push_outer_push_inner_outer_lib_index */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_BYPASS_STATE_DISABLE_ENABLE
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action bypass state ( disable | enable )
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_bypass_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    rtk_enable_t        enable;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
#if defined(CONFIG_SDK_RTL8390)
    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
        if (ACL_PHASE_EGR_ACL == phase)
        {
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
        }
    }
#endif
#if defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        if (ACL_PHASE_EACL == phase)
        {
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
        }
    }
#endif
    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action), ret);

    if('e' == TOKEN_CHAR(10, 0))
    {
        enable = ENABLED;
    }
    else
    {
        enable = DISABLED;
    }

    switch (phase)
    {
        #if defined(CONFIG_SDK_RTL8390)
        case ACL_PHASE_IGR_ACL:
            action.igr_acl.bypass_en = enable;
            break;
        #endif  /* (defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL8390)) */
        #if (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310))
        case ACL_PHASE_VACL:
            action.vact.bypass_en = enable;
            break;
        case ACL_PHASE_IACL:
            action.iact.bypass_en = enable;
            break;
        #endif  /* (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)) */
        default:
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_entry_action_bypass_state_disable_enable */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_BYPASS_INGRESS_BANDWIDTH_CONTROL_STATE_DISABLE_ENABLE
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action bypass ingress-bandwidth-control state ( disable | enable )
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_bypass_ingress_bandwidth_control_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    rtk_enable_t        enable;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
#if defined(CONFIG_SDK_RTL8390)
    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
        if (ACL_PHASE_EGR_ACL == phase)
        {
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
        }
    }
#endif
#if defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        if (ACL_PHASE_EACL == phase)
        {
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
        }
    }
#endif
    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action), ret);

    if('e' == TOKEN_CHAR(11, 0))
    {
        enable = ENABLED;

    }
    else
    {
        enable = DISABLED;
    }

    switch (phase)
    {
        #if (defined(CONFIG_SDK_RTL8390))
        case ACL_PHASE_IGR_ACL:
            action.igr_acl.bypass_data.ibc_sc = enable;
            break;
        #endif  /* (defined(CONFIG_SDK_RTL8390)) */
        #if (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310))
        case ACL_PHASE_VACL:
            action.vact.bypass_data.ibc_sc = enable;
            break;
        case ACL_PHASE_IACL:
            action.iact.bypass_data.ibc_sc = enable;
            break;
        #endif  /* (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)) */
        default:
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_entry_action_bypass_igr_bw_ctrl_state_disable_enable */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_BYPASS_INGRESS_STP_STATE_DISABLE_ENABLE
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action bypass ingress-stp state ( disable | enable )
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_bypass_ingress_stp_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    rtk_enable_t        enable;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
#if defined(CONFIG_SDK_RTL8390)
    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
        if (ACL_PHASE_IGR_ACL == phase)
        {
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
        }
    }
#endif
#if (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310))
    if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        if (ACL_PHASE_VACL != phase)
        {
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
        }
    }
#endif

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action), ret);

    if('e' == TOKEN_CHAR(11, 0))
    {
        enable = ENABLED;

    }
    else
    {
        enable = DISABLED;
    }

    switch (phase)
    {
        #if (defined(CONFIG_SDK_RTL8390))
        case ACL_PHASE_IGR_ACL:
            action.igr_acl.bypass_data.igr_stp = enable;
            break;
        #endif  /* (defined(CONFIG_SDK_RTL8390)) */
        #if (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310))
        case ACL_PHASE_VACL:
            action.vact.bypass_data.igr_stp = enable;
            break;
        case ACL_PHASE_IACL:
            action.iact.bypass_data.ibc_sc = enable;
            break;
        #endif  /* (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)) */
        default:
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_entry_action_bypass_igr_stp_state_disable_enable */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_BYPASS_INGRESS_VLAN_STATE_DISABLE_ENABLE
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action bypass ingress-vlan state ( disable | enable )
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_bypass_ingress_vlan_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
    if (ACL_PHASE_VACL != phase)
    {
        diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
        diag_util_printf("doesn't support the action\n");
        return CPARSER_NOT_OK;
    }

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action), ret);

    if('e' == TOKEN_CHAR(11, 0))
    {
        action.vact.bypass_data.igr_vlan = 1;
    }
    else
    {
        action.vact.bypass_data.igr_vlan = 0;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_entry_action_bypass_igr_drop_state_disable_enable */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_BYPASS_INGRESS_DROP_STATE_DISABLE_ENABLE
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action bypass ingress-drop state ( disable | enable )
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_bypass_ingress_drop_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);

    if (ACL_PHASE_EGR_ACL == phase)
    {
        diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
        diag_util_printf("doesn't support the action\n");
        return CPARSER_NOT_OK;
    }

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action), ret);

    if('e' == TOKEN_CHAR(11, 0))
    {
        action.igr_acl.bypass_data.all = 1;
    }
    else
    {
        action.igr_acl.bypass_data.all = 0;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_entry_action_bypass_igr_drop_state_disable_enable */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_REMARK_STATE_DISABLE_ENABLE
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action remark state ( disable | enable )
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_remark_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    rtk_enable_t        enable;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
#if defined(CONFIG_SDK_RTL8390)
    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
        if (ACL_PHASE_IGR_ACL == phase)
        {
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
        }
    }
#endif

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action), ret);

    if('e' == TOKEN_CHAR(10, 0))
    {
        enable = ENABLED;
    }
    else
    {
        enable = DISABLED;
    }

    switch (phase)
    {
        #if defined(CONFIG_SDK_RTL8380)
        case ACL_PHASE_IGR_ACL:
            action.igr_acl.rmk_en = enable;
            break;
        #endif  /* defined(CONFIG_SDK_RTL8380) */
        #if defined(CONFIG_SDK_RTL8390)
        case ACL_PHASE_EGR_ACL:
            action.egr_acl.rmk_en = enable;
            break;
        #endif  /* defined(CONFIG_SDK_RTL8390) */
        #if (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310))
        case ACL_PHASE_VACL:
            action.vact.rmk_en = enable;
            break;
        case ACL_PHASE_IACL:
            action.iact.rmk_en = enable;
            break;
        #endif  /* (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)) */
        #if defined(CONFIG_SDK_RTL9310)
        case ACL_PHASE_EACL:
            action.eact.rmk_en = enable;
            break;
        #endif  /* defined(CONFIG_SDK_RTL9310) */
        default:
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_entry_action_remark_state_disable_enable */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_YELLOW_REMARK_STATE_DISABLE_ENABLE
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action yellow-remark state ( disable | enable )
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_yellow_remark_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    rtk_enable_t        enable;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action), ret);

    if('e' == TOKEN_CHAR(10, 0))
    {
        enable = ENABLED;
    }
    else
    {
        enable = DISABLED;
    }

    switch (phase)
    {
        case ACL_PHASE_VACL:
            action.vact.yellow_rmk_en = enable;
            break;
        case ACL_PHASE_IACL:
            action.iact.yellow_rmk_en = enable;
            break;
        #if defined(CONFIG_SDK_RTL9310)
        case ACL_PHASE_EACL:
            action.eact.yellow_rmk_en = enable;
            break;
        #endif  /* defined(CONFIG_SDK_RTL9310) */
        default:
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_entry_action_remark_state_disable_enable */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_YELLOW_REMARK_DSCP_IP_PRECEDENCE_TOS_EAV_VALUE
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action yellow-remark ( dscp | ip-precedence | tos | eav ) <UINT:value>
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_yellow_remark_dscp_ip_precedence_tos_eav_value(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr,
    uint32_t  *value_ptr)
{
    uint32                      unit;
    int32                       ret;
    rtk_acl_action_t            action;
    rtk_acl_actionRemarkType_t  type;
    rtk_acl_phase_t             phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);

    if('d' == TOKEN_CHAR(9, 0))
    {
        type = ACL_ACTION_REMARK_DSCP;
    }
    else if('i' == TOKEN_CHAR(9, 0))
    {
        type = ACL_ACTION_REMARK_IP_PRECEDENCE;
    }
    else if('t' == TOKEN_CHAR(9, 0))
    {
        type = ACL_ACTION_REMARK_TOS;
    }
    else
    {
        type = ACL_ACTION_REMARK_EAV;
    }

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action), ret);

    switch (phase)
    {
        case ACL_PHASE_VACL:
            action.vact.yellow_rmk_data.rmk_act = type;
            action.vact.yellow_rmk_data.rmk_info = *value_ptr;
            break;
        case ACL_PHASE_IACL:
            action.iact.yellow_rmk_data.rmk_act = type;
            action.iact.yellow_rmk_data.rmk_info = *value_ptr;
            break;
        #if defined(CONFIG_SDK_RTL9310)
        case ACL_PHASE_EACL:
            action.eact.yellow_rmk_data.rmk_act = type;
            action.eact.yellow_rmk_data.rmk_info = *value_ptr;
            break;
        #endif  /* defined(CONFIG_SDK_RTL9310) */
        default:
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}
#endif


#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_RED_REMARK_STATE_DISABLE_ENABLE
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action red-remark state ( disable | enable )
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_red_remark_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    rtk_enable_t        enable;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action), ret);

    if('e' == TOKEN_CHAR(10, 0))
    {
        enable = ENABLED;
    }
    else
    {
        enable = DISABLED;
    }

    switch (phase)
    {
        case ACL_PHASE_VACL:
            action.vact.red_rmk_en = enable;
            break;
        case ACL_PHASE_IACL:
            action.iact.red_rmk_en = enable;
            break;
        #if defined(CONFIG_SDK_RTL9310)
        case ACL_PHASE_EACL:
            action.eact.red_rmk_en = enable;
            break;
        #endif  /* defined(CONFIG_SDK_RTL9310) */
        default:
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_entry_action_remark_state_disable_enable */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_RED_REMARK_DSCP_IP_PRECEDENCE_TOS_EAV_VALUE
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action red-remark ( dscp | ip-precedence | tos | eav ) <UINT:value>
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_red_remark_dscp_ip_precedence_tos_eav_value(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr,
    uint32_t  *value_ptr)
{
    uint32                      unit;
    int32                       ret;
    rtk_acl_action_t            action;
    rtk_acl_actionRemarkType_t  type;
    rtk_acl_phase_t             phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);

    if('d' == TOKEN_CHAR(9, 0))
    {
        type = ACL_ACTION_REMARK_DSCP;
    }
    else if('i' == TOKEN_CHAR(9, 0))
    {
        type = ACL_ACTION_REMARK_IP_PRECEDENCE;
    }
    else if('t' == TOKEN_CHAR(9, 0))
    {
        type = ACL_ACTION_REMARK_TOS;
    }
    else
    {
        type = ACL_ACTION_REMARK_EAV;
    }

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action), ret);

    switch (phase)
    {
        case ACL_PHASE_VACL:
            action.vact.red_rmk_data.rmk_act = type;
            action.vact.red_rmk_data.rmk_info = *value_ptr;
            break;
        case ACL_PHASE_IACL:
            action.iact.red_rmk_data.rmk_act = type;
            action.iact.red_rmk_data.rmk_info = *value_ptr;
            break;
        #if defined(CONFIG_SDK_RTL9310)
        case ACL_PHASE_EACL:
            action.eact.red_rmk_data.rmk_act = type;
            action.eact.red_rmk_data.rmk_info = *value_ptr;
            break;
        #endif  /* defined(CONFIG_SDK_RTL9310) */
        default:
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_REMARK_INNER_PRIORITY_OUTER_PRIORITY_PRIORITY
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action remark ( inner-priority | outer-priority ) <UINT:priority>
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_remark_inner_priority_outer_priority_priority(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr,
    uint32_t  *priority_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);

#if defined(CONFIG_SDK_RTL8390)
    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
        if (ACL_PHASE_IGR_ACL == phase)
        {
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
        }
    }
#endif

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action), ret);
#if defined(CONFIG_SDK_RTL8390)
    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
        if('i' == TOKEN_CHAR(9, 0))
        {
            action.egr_acl.rmk_data.rmk_act = ACL_ACTION_REMARK_INNER_USER_PRI;
        }
        else
        {
            action.egr_acl.rmk_data.rmk_act = ACL_ACTION_REMARK_OUTER_USER_PRI;
        }

        action.egr_acl.rmk_data.rmk_info = *priority_ptr;
    }
#endif

#if defined(CONFIG_SDK_RTL8380)
    if(DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
    {
        if('i' == TOKEN_CHAR(9, 0))
        {
            action.igr_acl.rmk_data.rmk_act = ACL_ACTION_REMARK_INNER_USER_PRI;
        }
        else
        {
            action.igr_acl.rmk_data.rmk_act = ACL_ACTION_REMARK_OUTER_USER_PRI;
        }

        action.igr_acl.rmk_data.rmk_info = *priority_ptr;
    }
#endif

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_entry_action_remark_inner_pri_outer_pri_priority */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_REMARK_KEEP_INNER_PRIORITY_OUTER_PRIORITY
/*
 *  acl set entry phase <UINT:phase> entry <UINT:index> action remark keep ( inner-priority | outer-priority )
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_remark_keep_inner_priority_outer_priority(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action), ret);
    if (ACL_PHASE_IGR_ACL == phase)
    {
        if('i' == TOKEN_CHAR(10, 0))
        {
            action.igr_acl.rmk_data.rmk_act = ACL_ACTION_REMARK_KEEP_INNER_USER_PRI;
        }
        else
        {
            action.igr_acl.rmk_data.rmk_act = ACL_ACTION_REMARK_KEEP_OUTER_USER_PRI;
        }
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_entry_action_remark_inner_pri_outer_pri_priority */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_REMARK_DSCP_DSCP
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action remark dscp <UINT:dscp>
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_remark_dscp_dscp(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr,
    uint32_t  *dscp_value_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
#if defined(CONFIG_SDK_RTL8390)
    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
        if (ACL_PHASE_IGR_ACL == phase)
        {
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
        }
    }
#endif

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action), ret);

    switch (phase)
    {
        #if defined(CONFIG_SDK_RTL8380)
        case ACL_PHASE_IGR_ACL:
            action.igr_acl.rmk_data.rmk_act = ACL_ACTION_REMARK_DSCP;
            action.igr_acl.rmk_data.rmk_info = *dscp_value_ptr;
            break;
        #endif  /* defined(CONFIG_SDK_RTL8380) */
        #if defined(CONFIG_SDK_RTL8390)
        case ACL_PHASE_EGR_ACL:
            action.egr_acl.rmk_data.rmk_act = ACL_ACTION_REMARK_DSCP;
            action.egr_acl.rmk_data.rmk_info = *dscp_value_ptr;
            break;
        #endif  /* defined(CONFIG_SDK_RTL8390) */
        #if (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310))
        case ACL_PHASE_VACL:
            action.vact.rmk_data.rmk_act = ACL_ACTION_REMARK_DSCP;
            action.vact.rmk_data.rmk_info = *dscp_value_ptr;
            break;
        case ACL_PHASE_IACL:
            action.iact.rmk_data.rmk_act = ACL_ACTION_REMARK_DSCP;
            action.iact.rmk_data.rmk_info = *dscp_value_ptr;
            break;
        #endif  /* (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)) */
        #if defined(CONFIG_SDK_RTL9310)
        case ACL_PHASE_EACL:
            action.eact.rmk_data.rmk_act = ACL_ACTION_REMARK_DSCP;
            action.eact.rmk_data.rmk_info = *dscp_value_ptr;
            break;
        #endif  /* defined(CONFIG_SDK_RTL9310) */
        default:
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_entry_action_remark_dscp_dscp_value */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_REMARK_IP_PRECEDENCE_PRECEDENCE
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action remark ip-precedence <UINT:precedence>
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_remark_ip_precedence_precedence(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr,
    uint32_t  *precedence_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
#if defined(CONFIG_SDK_RTL8390)
    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
        if (ACL_PHASE_IGR_ACL == phase)
        {
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
        }
    }
#endif

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action), ret);

    switch (phase)
    {
        #if defined(CONFIG_SDK_RTL8380)
        case ACL_PHASE_IGR_ACL:
            action.igr_acl.rmk_data.rmk_act = ACL_ACTION_REMARK_IP_PRECEDENCE;
            action.igr_acl.rmk_data.rmk_info = *precedence_ptr;
            break;
        #endif  /* defined(CONFIG_SDK_RTL8380) */
        #if defined(CONFIG_SDK_RTL8390)
        case ACL_PHASE_EGR_ACL:
            action.egr_acl.rmk_data.rmk_act = ACL_ACTION_REMARK_IP_PRECEDENCE;
            action.egr_acl.rmk_data.rmk_info = *precedence_ptr;
            break;
        #endif  /* defined(CONFIG_SDK_RTL8390) */
        #if (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310))
        case ACL_PHASE_VACL:
            action.vact.rmk_data.rmk_act = ACL_ACTION_REMARK_IP_PRECEDENCE;
            action.vact.rmk_data.rmk_info = *precedence_ptr;
            break;
        case ACL_PHASE_IACL:
            action.iact.rmk_data.rmk_act = ACL_ACTION_REMARK_IP_PRECEDENCE;
            action.iact.rmk_data.rmk_info = *precedence_ptr;
            break;
        #endif  /* (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)) */
        #if defined(CONFIG_SDK_RTL9310)
        case ACL_PHASE_EACL:
            action.eact.rmk_data.rmk_act = ACL_ACTION_REMARK_IP_PRECEDENCE;
            action.eact.rmk_data.rmk_info = *precedence_ptr;
            break;
        #endif  /* defined(CONFIG_SDK_RTL9310) */
        default:
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_entry_action_remark_ip_precedence_precedence */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_REMARK_TOS_TOS
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action remark tos <UINT:tos>
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_remark_tos_tos(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr,
    uint32_t  *tos_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action), ret);

    switch (phase)
    {
        case ACL_PHASE_VACL:
            action.vact.rmk_data.rmk_act = ACL_ACTION_REMARK_TOS;
            action.vact.rmk_data.rmk_info = *tos_ptr;
            break;
        case ACL_PHASE_IACL:
            action.iact.rmk_data.rmk_act = ACL_ACTION_REMARK_TOS;
            action.iact.rmk_data.rmk_info = *tos_ptr;
            break;
        #if defined(CONFIG_SDK_RTL9310)
        case ACL_PHASE_EACL:
            action.eact.rmk_data.rmk_act = ACL_ACTION_REMARK_TOS;
            action.eact.rmk_data.rmk_info = *tos_ptr;
            break;
        #endif  /* defined(CONFIG_SDK_RTL9310) */
        default:
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_entry_action_remark_ip_precedence_precedence */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_REMARK_EAV_EAV
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action remark eav <UINT:eav>
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_remark_eav_eav(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr,
    uint32_t  *eav_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
#if defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        if (ACL_PHASE_VACL != phase)
        {
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
        }
    }
#endif

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action), ret);

    switch (phase)
    {
        case ACL_PHASE_VACL:
            action.vact.rmk_data.rmk_act = ACL_ACTION_REMARK_EAV;
            action.vact.rmk_data.rmk_info = *eav_ptr;
            break;
        case ACL_PHASE_IACL:
            action.iact.rmk_data.rmk_act = ACL_ACTION_REMARK_EAV;
            action.iact.rmk_data.rmk_info = *eav_ptr;
            break;
        #if defined(CONFIG_SDK_RTL9310)
        case ACL_PHASE_EACL:
            action.eact.rmk_data.rmk_act = ACL_ACTION_REMARK_EAV;
            action.eact.rmk_data.rmk_info = *eav_ptr;
            break;
        #endif  /* defined(CONFIG_SDK_RTL9310) */
        default:
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_phase_entry_index_action_remark_eav_eav */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_IP_RSVD_FLAG_INVERT_DISABLE_ENABLE
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action ip-rsvd-flag invert ( disable | enable ) */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_ip_rsvd_flag_invert_disable_enable(
    cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *index_ptr)
{
    uint32  unit;
    int32   ret;
    rtk_enable_t        enable;
    rtk_acl_action_t    action;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action), ret);

    if('e' == TOKEN_CHAR(10, 0))
    {
        enable = ENABLED;
    }
    else
    {
        enable = DISABLED;
    }

    switch (phase)
    {
        case ACL_PHASE_VACL:
            action.vact.invert_en = enable;
            break;
        case ACL_PHASE_IACL:
            action.iact.invert_en = enable;
            break;
        default:
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_phase_entry_index_action_ip_rsvd_invert_disable_enable */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_REMARK_COPY_INNER_TO_OUTER
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action remark copy-inner-to-outer
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_remark_copy_inner_to_outer(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
#if defined(CONFIG_SDK_RTL8390)
    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
        if (ACL_PHASE_IGR_ACL == phase)
        {
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
        }
    }
#endif

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action), ret);
#if defined(CONFIG_SDK_RTL8390)
    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
        action.egr_acl.rmk_data.rmk_act = ACL_ACTION_REMARK_COPY_IPRI_TO_OPRI;
    }
#endif

#if defined(CONFIG_SDK_RTL8380)
    if(DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
    {
        action.igr_acl.rmk_data.rmk_act = ACL_ACTION_REMARK_COPY_IPRI_TO_OPRI;
    }
#endif

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_entry_action_remark_copy_inner_to_outer */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_REMARK_COPY_OUTER_TO_INNER
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action remark copy-outer-to-inner
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_remark_copy_outer_to_inner(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
#if defined(CONFIG_SDK_RTL8390)
    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
        if (ACL_PHASE_IGR_ACL == phase)
        {
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
        }
    }
#endif

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action), ret);
#if defined(CONFIG_SDK_RTL8390)
    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
        action.egr_acl.rmk_data.rmk_act = ACL_ACTION_REMARK_COPY_OPRI_TO_IPRI;
    }
#endif

#if defined(CONFIG_SDK_RTL8380)
    if(DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
    {
        action.igr_acl.rmk_data.rmk_act = ACL_ACTION_REMARK_COPY_OPRI_TO_IPRI;
    }
#endif

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_entry_action_remark_copy_outer_to_inner */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_TAG_STATE_DISABLE_ENABLE
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action tag state ( disable | enable )
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_tag_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    rtk_enable_t        enable;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
#if defined(CONFIG_SDK_RTL8390)
    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
        if (ACL_PHASE_IGR_ACL == phase)
        {
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
        }
    }
#endif
#if defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        if (ACL_PHASE_EACL == phase)
        {
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
        }
    }
#endif

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action), ret);

    if('e' == TOKEN_CHAR(10, 0))
    {
        enable = ENABLED;
    }
    else
    {
        enable = DISABLED;
    }

    switch (phase)
    {
        #if defined(CONFIG_SDK_RTL8380)
        case ACL_PHASE_IGR_ACL:
            action.igr_acl.tag_sts_en = enable;
            break;
        #endif  /* defined(CONFIG_SDK_RTL8380) */
        #if defined(CONFIG_SDK_RTL8390)
        case ACL_PHASE_EGR_ACL:
            action.egr_acl.tag_sts_en = enable;
            break;
        #endif  /* defined(CONFIG_SDK_RTL8390) */
        #if (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310))
        case ACL_PHASE_VACL:
            action.vact.tag_sts_en = enable;
            break;
        case ACL_PHASE_IACL:
            action.iact.tag_sts_en = enable;
            break;
        #endif  /* (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)) */
        default:
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_entry_action_tag_state_disable_enable */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_TAG_INNER_OUTER_UNTAG_TAG_KEEP_RESERVED
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action tag ( inner | outer ) ( untag | tag | keep | reserved )
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_tag_inner_outer_untag_tag_keep_reserved(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr)
{
    uint32                      unit;
    int32                       ret;
    rtk_acl_action_t            action;
    rtk_acl_actionTagStsFmt_t   status;
    rtk_acl_phase_t             phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
#if defined(CONFIG_SDK_RTL8390)
    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
        if (ACL_PHASE_IGR_ACL == phase)
        {
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
        }
    }
#endif
#if defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        if (ACL_PHASE_EACL == phase)
        {
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
        }
    }
#endif

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action), ret);

    if('u' == TOKEN_CHAR(10, 0))
    {
        status = ACL_ACTION_TAG_STS_UNTAG;
    }
    else if('t' == TOKEN_CHAR(10, 0))
    {
        status = ACL_ACTION_TAG_STS_TAG;
    }
    else if('k' == TOKEN_CHAR(10, 0))
    {
        if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID) ||
                DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
        {
            status = ACL_ACTION_TAG_STS_KEEP_CONTENT;
        }
        else
        {
            status = ACL_ACTION_TAG_STS_KEEP_FORMAT;
        }
    }
    else
    {
        status = ACL_ACTION_TAG_STS_NOP;
    }

    switch (phase)
    {
        #if defined(CONFIG_SDK_RTL8380)
        case ACL_PHASE_IGR_ACL:
            if('i' == TOKEN_CHAR(9, 0))
            {
                action.igr_acl.tag_sts_data.itag_sts = status;
            }
            else
            {
                action.igr_acl.tag_sts_data.otag_sts = status;
            }
            break;
        #endif  /* defined(CONFIG_SDK_RTL8380) */
        #if defined(CONFIG_SDK_RTL8390)
        case ACL_PHASE_EGR_ACL:
            if('i' == TOKEN_CHAR(9, 0))
            {
                action.egr_acl.tag_sts_data.itag_sts = status;
            }
            else
            {
                action.egr_acl.tag_sts_data.otag_sts = status;
            }
            break;
        #endif  /* defined(CONFIG_SDK_RTL8390) */
        #if (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310))
        case ACL_PHASE_VACL:
            if('i' == TOKEN_CHAR(9, 0))
            {
                action.vact.tag_sts_data.itag_sts = status;
            }
            else
            {
                action.vact.tag_sts_data.otag_sts = status;
            }
            break;
        case ACL_PHASE_IACL:
            if('i' == TOKEN_CHAR(9, 0))
            {
                action.iact.tag_sts_data.itag_sts = status;
            }
            else
            {
                action.iact.tag_sts_data.otag_sts = status;
            }
            break;
        #endif  /* (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)) */
        default:
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_entry_action_tag_inner_outer_untag_tag_keep_reserved */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_CPU_QID_STATE_DISABLE_ENABLE
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action cpu-qid state ( disable | enable )
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_cpu_qid_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    rtk_enable_t        enable;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
#if defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        if (ACL_PHASE_EACL == phase)
        {
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
        }
    }
#endif

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action), ret);

    if('e' == TOKEN_CHAR(10, 0))
    {
        enable = ENABLED;
    }
    else
    {
        enable = DISABLED;
    }

    switch (phase)
    {
        case ACL_PHASE_VACL:
            action.vact.cpu_qid_en = enable;
            break;
        case ACL_PHASE_IACL:
            action.iact.cpu_qid_en = enable;
            break;
        default:
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_entry_action_tag_state_disable_enable */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_CPU_QID_QID
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action cpu-qid <UINT:qid>
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_cpu_qid_qid(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr,
    uint32_t  *qid_ptr)
{
    rtk_acl_action_t    action;
    rtk_acl_phase_t     phase;
    uint32              unit;
    int32               ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
#if defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        if (ACL_PHASE_EACL == phase)
        {
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
        }
    }
#endif

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action), ret);

    switch (phase)
    {
        case ACL_PHASE_VACL:
            action.vact.cpu_qid.qid = *qid_ptr;
            #if defined(CONFIG_SDK_RTL9310)
            action.vact.cpu_qid.act = ACL_ACTION_QID_CPU;
            #endif
            break;
        case ACL_PHASE_IACL:
            action.iact.cpu_qid.qid = *qid_ptr;
            break;
        default:
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_entry_action_remark_ip_precedence_precedence */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_IGR_QID_QID
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action igr-qid <UINT:qid>
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_igr_qid_qid(
    cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *index_ptr,
    uint32_t *qid_ptr)
{
    rtk_acl_action_t    action;
    rtk_acl_phase_t     phase;
    uint32              unit;
    int32               ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
#if defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        if (ACL_PHASE_VACL != phase)
        {
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
        }
    }
#endif

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action), ret);

    action.vact.cpu_qid.qid = *qid_ptr;
    action.vact.cpu_qid.act = ACL_ACTION_QID_IGR;

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_phase_entry_index_action_igr_qid_qid */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_META_DATA_STATE_DISABLE_ENABLE
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action meta-data state ( disable | enable )
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_meta_data_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    rtk_enable_t        enable;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
#if defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        if (ACL_PHASE_EACL == phase)
        {
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
        }
    }
#endif

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action), ret);

    if('e' == TOKEN_CHAR(10, 0))
    {
        enable = ENABLED;
    }
    else
    {
        enable = DISABLED;
    }

    switch (phase)
    {
        case ACL_PHASE_VACL:
            action.vact.meta_data_en = enable;
            break;
        case ACL_PHASE_IACL:
            action.iact.meta_data_en = enable;
            break;
        default:
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_phase_entry_index_action_meta_data_state_disable_enable */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_META_DATA_DATA
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action meta-data <UINT:data>
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_meta_data_data(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *index_ptr,
    uint32_t  *data_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
#if defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        if (ACL_PHASE_EACL == phase)
        {
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
        }
    }
#endif

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action), ret);

    switch (phase)
    {
        case ACL_PHASE_VACL:
            action.vact.meta_data.data = *data_ptr;
            break;
        case ACL_PHASE_IACL:
            action.iact.meta_data.data = *data_ptr;
            break;
        default:
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_phase_entry_index_action_meta_data_data */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_META_DATA_MASK_MASK
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action meta-data-mask <UINT:mask>
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_meta_data_mask_mask(
    cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *index_ptr,
    uint32_t *mask_ptr)
{
    rtk_acl_action_t    action;
    rtk_acl_phase_t     phase;
    uint32              unit;
    int32               ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
#if defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        if (ACL_PHASE_EACL == phase)
        {
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
        }
    }
#endif

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action), ret);

    switch (phase)
    {
        case ACL_PHASE_VACL:
            action.vact.meta_data.mask = *mask_ptr;
            break;
        case ACL_PHASE_IACL:
            action.iact.meta_data.mask = *mask_ptr;
            break;
        default:
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_phase_entry_index_action_meta_data_mask_mask */
#endif

#ifdef CMD_ACL_DEL_ENTRY_PHASE_PHASE_START_START_END_END
/*
 * acl del entry phase <UINT:phase> start <UINT:start> end <UINT:end>
 */
cparser_result_t
cparser_cmd_acl_del_entry_phase_phase_start_start_end_end(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *start_ptr,
    uint32_t  *end_ptr)
{
    uint32          unit;
    int32           ret;
    rtk_acl_clear_t clear_info;
    rtk_acl_phase_t phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);

    clear_info.start_idx = *start_ptr;
    clear_info.end_idx = *end_ptr;
    DIAG_UTIL_ERR_CHK(rtk_acl_rule_del(unit, phase, &clear_info), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_del_entry_phase_start_end */
#endif

#ifdef CMD_ACL_GET_SELECTOR_INDEX
/*
 * acl get selector <UINT:index>
 */
cparser_result_t
cparser_cmd_acl_get_selector_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    uint32                  unit;
    int32                   ret;
    uint32                  i, numOftemp;
    rtk_acl_templateIdx_t   temp_info;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_acl_templateSelector_get(unit, *index_ptr, &temp_info), ret);

    DIAG_OM_GET_CHIP_CAPACITY(unit, numOftemp, max_num_of_pie_block_templateSelector);
    diag_util_mprintf("Block %d:\n", *index_ptr);

    for(i = 0; i < numOftemp; i ++)
    {
        diag_util_mprintf("\tTemplate idx %d: %d\n", i, temp_info.template_id[i]);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_get_selector_block_index */
#endif

#ifdef CMD_ACL_SET_SELECTOR_BLOCK_INDEX_TEMPLATE_INDEX0_TEMPLATE_INDEX
/*
 * acl set selector block <UINT:index> template-index0 <UINT:template_index>
 */
cparser_result_t
cparser_cmd_acl_set_selector_block_index_template_index0_template_index(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *template_index_ptr)
{
    uint32                  unit;
    int32                   ret;
    rtk_acl_templateIdx_t   temp_info;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(rtk_acl_templateSelector_get(unit, *index_ptr, &temp_info), ret);

    temp_info.template_id[0] = *template_index_ptr;
    DIAG_UTIL_ERR_CHK(rtk_acl_templateSelector_set(unit, *index_ptr, temp_info), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_selector_block_index_template_idx1_template_idx2 */
#endif

#ifdef CMD_ACL_SET_SELECTOR_BLOCK_INDEX_TEMPLATE_INDEX1_TEMPLATE_INDEX
/*
 * acl set selector block <UINT:index> template-index1 <UINT:template_index>
 */
cparser_result_t
cparser_cmd_acl_set_selector_block_index_template_index1_template_index(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *template_index_ptr)
{
    uint32                  unit;
    int32                   ret;
    rtk_acl_templateIdx_t   temp_info;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(rtk_acl_templateSelector_get(unit, *index_ptr, &temp_info), ret);

    temp_info.template_id[1] = *template_index_ptr;
    DIAG_UTIL_ERR_CHK(rtk_acl_templateSelector_set(unit, *index_ptr, temp_info), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_selector_block_index_template_idx1_template_idx2 */
#endif

#ifdef CMD_ACL_SET_SELECTOR_BLOCK_INDEX_TEMPLATE_INDEX2_TEMPLATE_INDEX
/*
 * acl set selector block <UINT:index> template-index2 <UINT:template_index>
 */
cparser_result_t
cparser_cmd_acl_set_selector_block_index_template_index2_template_index(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *template_index_ptr)
{
    uint32                  unit;
    int32                   ret;
    rtk_acl_templateIdx_t   temp_info;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(rtk_acl_templateSelector_get(unit, *index_ptr, &temp_info), ret);

    temp_info.template_id[2] = *template_index_ptr;
    DIAG_UTIL_ERR_CHK(rtk_acl_templateSelector_set(unit, *index_ptr, temp_info), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_selector_block_index_template_idx1_template_idx2 */
#endif

#ifdef CMD_ACL_GET_ENTRY_PHASE_PHASE_ENTRY_INDEX_HIT_INDICATION
/*
 * acl get entry phase <UINT:phase> entry <UINT:index> hit-indication
 */
cparser_result_t cparser_cmd_acl_get_entry_phase_phase_entry_index_hit_indication(cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *index_ptr)
{
    uint32 unit;
    int32  ret;
    uint32 status;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleHitIndication_get(unit, phase, *index_ptr, FALSE, &status), ret);

    diag_util_mprintf("Hit Status [%d]: %s\n", *index_ptr, (status == TRUE)?"True":"False");

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_get_entry_phase_phase_entry_index_hit_indication */
#endif

#ifdef CMD_ACL_GET_ENTRY_PHASE_PHASE_HIT_INDICATION_MASK
/*
 * acl get entry phase <UINT:phase> hit-indication-mask
 */
cparser_result_t cparser_cmd_acl_get_entry_phase_phase_hit_indication_mask(cparser_context_t *context,
    uint32_t *phase_ptr)
{
    uint32 unit;
    int32  ret;
    uint32 idx;
    rtk_acl_phase_t     phase;
    rtk_acl_hitMask_t   hit_mask;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleHitIndicationMask_get(unit, phase, FALSE, &hit_mask), ret);

    for (idx = 0; idx < RTK_MAX_NUM_OF_ACL_ENTRY; idx++)
    {
        if (BITMAP_IS_SET(hit_mask.bits, idx))
        {
            diag_util_mprintf("Hit Status [%d]: %s\n", idx, "True");
        }
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_get_entry_phase_phase_entry_index_hit_indication */
#endif

#ifdef CMD_ACL_RESET_ENTRY_PHASE_PHASE_ENTRY_INDEX_HIT_INDICATION
/*
 * acl reset entry phase <UINT:phase> entry <UINT:index> hit-indication
 */
cparser_result_t cparser_cmd_acl_reset_entry_phase_phase_entry_index_hit_indication(cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *index_ptr)
{
    uint32          unit;
    int32           ret;
    uint32          status;
    rtk_acl_phase_t phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
    DIAG_UTIL_ERR_CHK(rtk_acl_ruleHitIndication_get(unit, phase, *index_ptr, TRUE, &status), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_reset_entry_phase_phase_entry_index_hit_indication */
#endif

#ifdef CMD_ACL_MOVE_ENTRY_PHASE_PHASE_FROM_FROM_TO_TO_ENTRY_NUM_ENTRY_NUM
/*
 * acl move entry phase <UINT:phase> from <UINT:from> to <UINT:to> entry_num <UINT:entry_num>
 */
cparser_result_t
cparser_cmd_acl_move_entry_phase_phase_from_from_to_to_entry_num_entry_num(
    cparser_context_t *context,
    uint32_t  *phase_ptr,
    uint32_t  *from_ptr,
    uint32_t  *to_ptr,
    uint32_t  *entry_num_ptr)
{
    uint32          unit;
    int32           ret;
    rtk_acl_move_t  move_info;
    rtk_acl_phase_t phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);

    move_info.move_from = *from_ptr;
    move_info.move_to = *to_ptr;
    move_info.length = *entry_num_ptr;
    DIAG_UTIL_ERR_CHK(rtk_acl_rule_move(unit, phase, &move_info), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_move_entry_phase_from_to_entry_num */
#endif

#ifdef CMD_ACL_SET_BLOCK_INDEX_MULTIPLE_RESULT_STATE_DISABLE_ENABLE
/*
 * acl set block <UINT:index> multiple-result state ( disable | enable )
 */
cparser_result_t
cparser_cmd_acl_set_block_index_multiple_result_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    uint32                      unit;
    int32                       ret;
    rtk_acl_blockResultMode_t   mode;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if('e' == TOKEN_CHAR(6, 0))
    {
        mode = ACL_BLOCK_RESULT_MULTIPLE;
    }
    else
    {
        mode = ACL_BLOCK_RESULT_SINGLE;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_blockResultMode_set(unit, *index_ptr, mode), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_block_index_multi_result_state_disable_enable */
#endif

#ifdef CMD_ACL_GET_BLOCK_INDEX_MULTI_RESULT_STATE
/*
 * acl get block <UINT:index> multi-result state
 */
cparser_result_t
cparser_cmd_acl_get_block_index_multi_result_state(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    uint32                      unit;
    int32                       ret;
    rtk_acl_blockResultMode_t   mode;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_acl_blockResultMode_get(unit, *index_ptr, &mode), ret);

    diag_util_mprintf("Block %d multi-result state: ", *index_ptr);
    if (ACL_BLOCK_RESULT_SINGLE == mode)
        diag_util_mprintf(DIAG_STR_DISABLE);
    else
        diag_util_mprintf(DIAG_STR_ENABLE);

    diag_util_mprintf("\n");

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_get_block_index_multi_result_state */
#endif

/*
*acl set block <UINT:index> group group_1 state ( disable | enable )
*/
#ifdef CMD_ACL_SET_BLOCK_INDEX_GROUP_GROUP_1_STATE_DISABLE_ENABLE
cparser_result_t
cparser_cmd_acl_set_block_index_group_group_1_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    uint32                      unit;
    int32                       ret;
    rtk_enable_t                enable;
    rtk_acl_blockGroup_t        group_type;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if('e' == TOKEN_CHAR(7, 0))
    {
        enable = ENABLED;
    }
    else
    {
        enable = DISABLED;
    }

    group_type = ACL_BLOCK_GROUP_1;

    DIAG_UTIL_ERR_CHK(rtk_acl_blockGroupEnable_set(unit, *index_ptr, group_type, enable), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_block_index_group_group_2_group_4_group_8_all_state_disable_enable */
#endif

#ifdef CMD_ACL_GET_BLOCK_INDEX_GROUP_GROUP_1_STATE
/*
 * acl get block <UINT:index> group group_1 state
 */
cparser_result_t
cparser_cmd_acl_get_block_index_group_group_1_state(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    uint32                      unit;
    int32                       ret;
    rtk_enable_t                enable;
    rtk_acl_blockGroup_t        group_type;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    group_type = ACL_BLOCK_GROUP_1;

    DIAG_UTIL_ERR_CHK(rtk_acl_blockGroupEnable_get(unit, *index_ptr, group_type, &enable), ret);

    diag_util_mprintf("Block %d grouping state: ", *index_ptr);
    if (ENABLED == enable)
        diag_util_mprintf(DIAG_STR_ENABLE);
    else
        diag_util_mprintf(DIAG_STR_DISABLE);

    diag_util_mprintf("\n");

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_get_block_index_group_group_2_group_4_group_8_all_state */
#endif

#ifdef CMD_ACL_SET_BLOCK_INDEX_GROUP_GROUP_2_GROUP_4_GROUP_8_ALL_STATE_DISABLE_ENABLE
/*
 * acl set block <UINT:index> group ( group_2 | group_4 | group_8 | all ) state ( disable | enable )
 */
cparser_result_t
cparser_cmd_acl_set_block_index_group_group_2_group_4_group_8_all_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    uint32                      unit;
    int32                       ret;
    rtk_enable_t                enable;
    rtk_acl_blockGroup_t        group_type;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if('e' == TOKEN_CHAR(7, 0))
    {
        enable = ENABLED;
    }
    else
    {
        enable = DISABLED;
    }

    if (0 == osal_strcmp(TOKEN_STR(5), "all"))
    {
        group_type = ACL_BLOCK_GROUP_ALL;
    }
    else if('2' == TOKEN_CHAR(5, 6))
    {
        group_type = ACL_BLOCK_GROUP_2;
    }
    else if('4' == TOKEN_CHAR(5, 6))
    {
        group_type = ACL_BLOCK_GROUP_4;
    }
    else
    {
        group_type = ACL_BLOCK_GROUP_8;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_blockGroupEnable_set(unit, *index_ptr, group_type, enable), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_block_index_group_group_2_group_4_group_8_all_state_disable_enable */
#endif

#ifdef CMD_ACL_GET_BLOCK_INDEX_GROUP_GROUP_2_GROUP_4_GROUP_8_ALL_STATE
/*
 * acl get block <UINT:index> group ( group_2 | group_4 | group_8 | all ) state
 */
cparser_result_t
cparser_cmd_acl_get_block_index_group_group_2_group_4_group_8_all_state(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    uint32                      unit;
    int32                       ret;
    rtk_enable_t                enable;
    rtk_acl_blockGroup_t        group_type;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (0 == osal_strcmp(TOKEN_STR(5), "all"))
    {
        group_type = ACL_BLOCK_GROUP_ALL;
    }
    else if('2' == TOKEN_CHAR(5, 6))
    {
        group_type = ACL_BLOCK_GROUP_2;
    }
    else if('4' == TOKEN_CHAR(5, 6))
    {
        group_type = ACL_BLOCK_GROUP_4;
    }
    else
    {
        group_type = ACL_BLOCK_GROUP_8;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_blockGroupEnable_get(unit, *index_ptr, group_type, &enable), ret);

    diag_util_mprintf("Block %d aggregator %s state: ", *index_ptr, TOKEN_STR(5));
    if (ENABLED == enable)
        diag_util_mprintf(DIAG_STR_ENABLE);
    else
        diag_util_mprintf(DIAG_STR_DISABLE);

    diag_util_mprintf("\n");

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_get_block_index_group_group_2_group_4_group_8_all_state */
#endif

#ifdef CMD_ACL_GET_COUNTER_BYTE_PACKET_INDEX
/*
 * acl get counter ( byte | packet ) <UINT:index>
 */
cparser_result_t
cparser_cmd_acl_get_counter_byte_packet_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    uint32  unit;
    int32   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if('b' == TOKEN_CHAR(3, 0))
    {
        uint64  byte_cnt;

        DIAG_UTIL_ERR_CHK(rtk_acl_statByteCnt_get(unit, *index_ptr, &byte_cnt), ret);

        diag_util_mprintf("Byte counter index %d: %llu\n", *index_ptr, byte_cnt);
    }
    else
    {
        uint32  pkt_cnt;

        DIAG_UTIL_ERR_CHK(rtk_acl_statPktCnt_get(unit, *index_ptr, (uint32 *)&pkt_cnt), ret);

        diag_util_mprintf("Packet counter index %d: %lu\n", *index_ptr, pkt_cnt);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_get_counter_byte_packet_index */
#endif

#ifdef CMD_ACL_CLEAR_COUNTER_ALL
/*
 * acl clear counter all
 */
cparser_result_t
cparser_cmd_acl_clear_counter_all(
    cparser_context_t *context)
{
    uint32  unit;
    int32   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(rtk_acl_stat_clearAll(unit), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_clear_counter_all */
#endif

#ifdef CMD_ACL_CLEAR_COUNTER_BYTE_PACKET_INDEX
/*
 * acl clear counter ( byte | packet ) <UINT:index>
 */
cparser_result_t
cparser_cmd_acl_clear_counter_byte_packet_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    uint32  unit;
    int32   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if('b' == TOKEN_CHAR(3, 0))
    {
        DIAG_UTIL_ERR_CHK(rtk_acl_statByteCnt_clear(unit, *index_ptr), ret);
    }
    else
    {
        DIAG_UTIL_ERR_CHK(rtk_acl_statPktCnt_clear(unit, *index_ptr), ret);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_clear_counter_byte_packet_index */
#endif

#ifdef CMD_ACL_GET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION
#if defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL8390)
static void
_diag_acl_igrAction_show(uint32 unit, rtk_acl_igrAction_t *action)
{
#if defined(CONFIG_SDK_RTL8390)
    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
        /* forward action */
        diag_util_mprintf("\tForward action state: ");
        if (ENABLED == action->fwd_en)
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);

            diag_util_mprintf("\t\taction: ");
            switch (action->fwd_data.fwd_type)
            {
                case ACL_ACTION_FWD_PERMIT:
                    diag_util_mprintf("permit\n");
                    break;
                case ACL_ACTION_FWD_DROP:
                    diag_util_mprintf("drop\n");
                    break;
                case ACL_ACTION_FWD_COPY_TO_PORTID:
                    diag_util_mprintf("copy uni port:%d\n", action->fwd_data.fwd_info);
                    break;
                case ACL_ACTION_FWD_COPY_TO_PORTMASK:
                    diag_util_mprintf("copy multi index:%d\n", action->fwd_data.fwd_info);
                    break;
                case ACL_ACTION_FWD_REDIRECT_TO_PORTID:
                    diag_util_mprintf("redirect uni port:%d\n", action->fwd_data.fwd_info);
                    break;
                case ACL_ACTION_FWD_REDIRECT_TO_PORTMASK:
                    diag_util_mprintf("redirect multi index:%d\n", action->fwd_data.fwd_info);
                    break;
                case ACL_ACTION_FWD_UNICAST_ROUTING:
                    diag_util_mprintf("route unicast index:%d\n", action->fwd_data.fwd_info);
                    break;
                default:
                    diag_util_mprintf("\n");
            }   /* end of switch (action->fwd_data.fwd_type) */
        }   /* if (ENABLED == action->fwd_en) */
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

        /* Counter action */
        diag_util_mprintf("\tStatistic action state: ");
        if (ENABLED == action->stat_en)
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);

            diag_util_mprintf("\t\taction: ");
            switch (action->stat_data.stat_type)
            {
                case STAT_TYPE_PACKET_BASED_32BIT:
                    diag_util_mprintf("packet32 index:%d\n", action->stat_data.stat_idx);
                    break;
                case STAT_TYPE_BYTE_BASED_64BIT:
                    diag_util_mprintf("byte64 index:%d\n", action->stat_data.stat_idx);
                    break;
                default:
                    diag_util_mprintf("\n");
            }   /* end of switch (action->stat_data.stat_type) */
        }   /* if (ENABLED == action->stat_en) */
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

        /* Mirror action */
        diag_util_mprintf("\tMirror action state: ");
        if (ENABLED == action->mirror_en)
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);

            diag_util_mprintf("\t\tmirror index:%d\n", action->mirror_data.mirror_set_idx);
        }   /* end of if (ENABLED == action->mirror_en) */
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

        /* Meter action */
        diag_util_mprintf("\tMeter action state: ");
        if (ENABLED == action->meter_en)
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);
            diag_util_mprintf("\t\tmeter index:%d\n", action->meter_data.meter_idx);
        }   /* end of if (ENABLED == action->meter_en) */
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

        /* inner vlan translate action */
        diag_util_mprintf("\tInner Vlan translate action state: ");
        if (ENABLED == action->inner_vlan_assign_en)
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);

            diag_util_mprintf("\t\tinner vlan-xlate action: ");
            switch (action->inner_vlan_data.vid_assign_type)
            {
                case ACL_IGR_ACTION_IVLAN_ASSIGN_NEW_VID:
                    diag_util_mprintf("new vid:%d\n", action->inner_vlan_data.vid_value);
                    break;
                case ACL_IGR_ACTION_IVLAN_ASSIGN_SHIFT_VID:
                    diag_util_mprintf("shift vid:%d\n", action->inner_vlan_data.vid_value);
                    break;
                case ACL_IGR_ACTION_IVLAN_ASSIGN_SHIFT_FROM_OUTER_VID:
                    diag_util_mprintf("shift from outer vid:%d\n", action->inner_vlan_data.vid_value);
                    break;
                default:
                    diag_util_mprintf("\n");
            }   /* end of switch (action->inner_vlan_data.vid_assign_type) */
        }   /* end of if (ENABLED == action->inner_vlan_assign_en) */
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

        /* outer vlan translate action */
        diag_util_mprintf("\tOuter Vlan translate action state: ");
        if (ENABLED == action->outer_vlan_assign_en)
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);

            diag_util_mprintf("\t\touter vlan-xlate action: ");
            switch (action->outer_vlan_data.vid_assign_type)
            {
                case ACL_IGR_ACTION_OVLAN_ASSIGN_NEW_VID:
                    diag_util_mprintf("new vid:%d\n", action->outer_vlan_data.vid_value);
                    break;
                case ACL_IGR_ACTION_OVLAN_ASSIGN_SHIFT_VID:
                    diag_util_mprintf("shift vid:%d\n", action->outer_vlan_data.vid_value);
                    break;
                case ACL_IGR_ACTION_OVLAN_ASSIGN_SHIFT_FROM_INNER_VID:
                    diag_util_mprintf("shift from inner vid:%d\n", action->outer_vlan_data.vid_value);
                    break;
                default:
                    diag_util_mprintf("\n");
            }   /* end of switch (action->outer_vlan_data.vid_assign_type) */
        }   /* end of if (ENABLED == action->outer_vlan_assign_en) */
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

        /* priority action */
        diag_util_mprintf("\tPriority action state: ");
        if (ENABLED == action->pri_en)
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);
            diag_util_mprintf("\t\tpriority:%d\n", action->pri_data.pri);
        }   /* end of if (ENABLED == action->pri_en) */
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

        /* MPLS action */
        diag_util_mprintf("\tMPLS action state: ");
        if (ENABLED == action->mpls_en)
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);
            diag_util_mprintf("\t\tmpls action: ");

            if (0 == action->mpls_data.mpls_act)
                diag_util_mprintf("push outer ");
            else
                diag_util_mprintf("push inner and outer ");

            diag_util_mprintf("index:%d \n", action->mpls_data.mpls_idx);
        }   /* end of if (ENABLED == action->mpls_en) */
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

        /* bypass action */
        diag_util_mprintf("\tBypass action state: ");
        if (ENABLED == action->bypass_en)
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);

            diag_util_mprintf("\t\tigr-bw-ctrl state: ");
            if (ENABLED == action->bypass_data.ibc_sc)
                diag_util_mprintf("%s\n", DIAG_STR_ENABLE);
            else
                diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

            diag_util_mprintf("\t\tigr-stp state: ");
            if (ENABLED == action->bypass_data.igr_stp)
                diag_util_mprintf("%s\n", DIAG_STR_ENABLE);
            else
                diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

            diag_util_mprintf("\t\tigr-drop state: ");
            if (ENABLED == action->bypass_data.all)
                diag_util_mprintf("%s\n", DIAG_STR_ENABLE);
            else
                diag_util_mprintf("%s\n", DIAG_STR_DISABLE);
        }   /* end of if (ENABLED == action->bypass_en) */
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);
    }
#endif

#if defined(CONFIG_SDK_RTL8380)
    if(DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
    {
        /* forward action */
        diag_util_mprintf("\tForward action state: ");
        if (ENABLED == action->fwd_en)
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);

            diag_util_mprintf("\t\taction: ");
            switch (action->fwd_data.fwd_type)
            {
                case ACL_ACTION_FWD_PERMIT:
                    diag_util_mprintf("permit\n");
                    break;
                case ACL_ACTION_FWD_DROP:
                    diag_util_mprintf("drop\n");
                    break;
                case ACL_ACTION_FWD_COPY_TO_PORTID:
                    diag_util_mprintf("copy uni port:%d\n", action->fwd_data.info.copy_redirect_port.fwd_port_id);
                    diag_util_mprintf("\t\t        cpu tag:%d\n", action->fwd_data.info.copy_redirect_port.cpu_tag);
                    diag_util_mprintf("\t\t        skip ingress stp:%d\n", action->fwd_data.info.copy_redirect_port.skip_igrStpDrop);
                    diag_util_mprintf("\t\t        skip ingress vlan filter:%d\n", action->fwd_data.info.copy_redirect_port.skip_storm_igrVlan);
                    break;
                case ACL_ACTION_FWD_COPY_TO_PORTMASK:
                    diag_util_mprintf("copy multi index:%d\n", action->fwd_data.info.copy_redirect_portMsk.fwd_idx);
                    diag_util_mprintf("\t\t        cpu tag:%d\n", action->fwd_data.info.copy_redirect_portMsk.cpu_tag);
                    diag_util_mprintf("\t\t        skip ingress stp:%d\n", action->fwd_data.info.copy_redirect_portMsk.skip_igrStpDrop);
                    diag_util_mprintf("\t\t        skip ingress vlan filter:%d\n", action->fwd_data.info.copy_redirect_portMsk.skip_storm_igrVlan);
                    break;
                case ACL_ACTION_FWD_REDIRECT_TO_PORTID:
                    diag_util_mprintf("redirect uni port:%d\n", action->fwd_data.info.copy_redirect_port.fwd_port_id);
                    diag_util_mprintf("\t\t        cpu tag:%d\n", action->fwd_data.info.copy_redirect_port.cpu_tag);
                    diag_util_mprintf("\t\t        skip ingress stp:%d\n", action->fwd_data.info.copy_redirect_port.skip_igrStpDrop);
                    diag_util_mprintf("\t\t        skip storm suppression filter:%d\n", action->fwd_data.info.copy_redirect_port.skip_storm_igrVlan);
                    diag_util_mprintf("\t\t        force:%d\n", action->fwd_data.info.copy_redirect_port.force);
                    break;
                case ACL_ACTION_FWD_REDIRECT_TO_PORTMASK:
                    diag_util_mprintf("redirect multi index:%d\n", action->fwd_data.info.copy_redirect_portMsk.fwd_idx);
                    diag_util_mprintf("\t\t        cpu tag:%d\n", action->fwd_data.info.copy_redirect_portMsk.cpu_tag);
                    diag_util_mprintf("\t\t        skip ingress stp:%d\n", action->fwd_data.info.copy_redirect_portMsk.skip_igrStpDrop);
                    diag_util_mprintf("\t\t        skip storm suppression filter:%d\n", action->fwd_data.info.copy_redirect_portMsk.skip_storm_igrVlan);
                    diag_util_mprintf("\t\t        force:%d\n", action->fwd_data.info.copy_redirect_portMsk.force);
                    break;
                case ACL_ACTION_FWD_UNICAST_ROUTING:
                    diag_util_mprintf("route unicast index:%d\n", action->fwd_data.info.route.idx);
                    break;
                case ACL_ACTION_FWD_VLAN_LEAKY:
                    diag_util_mprintf("vlan leaky\n");
                    break;

                default:
                    diag_util_mprintf("\n");
            }   /* end of switch (action->fwd_data.fwd_type) */
        }   /* if (ENABLED == action->fwd_en) */
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

        /* Counter action */
        diag_util_mprintf("\tStatistic action state: ");
        if (ENABLED == action->stat_en)
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);

            diag_util_mprintf("\t\taction: ");
            switch (action->stat_data.stat_type)
            {
                case STAT_TYPE_PACKET_BASED_32BIT:
                    diag_util_mprintf("packet32 index:%d\n", action->stat_data.stat_idx);
                    break;
                case STAT_TYPE_BYTE_BASED_64BIT:
                    diag_util_mprintf("byte64 index:%d\n", action->stat_data.stat_idx);
                    break;
                default:
                    diag_util_mprintf("\n");
            }   /* end of switch (action->stat_data.stat_type) */
        }   /* if (ENABLED == action->stat_en) */
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

        /* Mirror action */
        diag_util_mprintf("\tMirror action state: ");
        if (ENABLED == action->mirror_en)
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);

            diag_util_mprintf("\t\tmirror index:%d\n", action->mirror_data.mirror_set_idx);
            if(ACL_ACTION_MIRROR_ORIGINAL == action->mirror_data.mirror_type)
            {
                diag_util_mprintf("\t\tmirror type: original\n");
            }
            else
            {
                diag_util_mprintf("\t\tmirror type: modified\n");
            }
        }   /* end of if (ENABLED == action->mirror_en) */
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

        /* Meter action */
        diag_util_mprintf("\tMeter action state: ");
        if (ENABLED == action->meter_en)
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);
            diag_util_mprintf("\t\tmeter index:%d\n", action->meter_data.meter_idx);
        }   /* end of if (ENABLED == action->meter_en) */
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

        /* inner vlan translate action */
        diag_util_mprintf("\tInner Vlan translate action state: ");
        if (ENABLED == action->inner_vlan_assign_en)
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);

            diag_util_mprintf("\t\tinner vlan-xlate action: ");
            switch (action->inner_vlan_data.vid_assign_type)
            {
                case ACL_IGR_ACTION_IVLAN_ASSIGN_NEW_VID:
                    diag_util_mprintf("new vid:%d\n", action->inner_vlan_data.vid_value);
                    break;
                case ACL_IGR_ACTION_IVLAN_ASSIGN_SHIFT_VID:
                    diag_util_mprintf("shift vid:%d\n", action->inner_vlan_data.vid_value);
                    break;
                case ACL_IGR_ACTION_IVLAN_ASSIGN_SHIFT_FROM_OUTER_VID:
                    diag_util_mprintf("shift from outer vid:%d\n", action->inner_vlan_data.vid_value);
                    break;
                case ACL_IGR_ACTION_IVLAN_ASSIGN_PORT_BASED_INNER_VID:
                    diag_util_mprintf("shift from inner port-based vid\n");
                    break;
                default:
                    diag_util_mprintf("\n");
            }   /* end of switch (action->inner_vlan_data.vid_assign_type) */
        }   /* end of if (ENABLED == action->inner_vlan_assign_en) */
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

        /* outer vlan translate action */
        diag_util_mprintf("\tOuter Vlan translate action state: ");
        if (ENABLED == action->outer_vlan_assign_en)
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);

            diag_util_mprintf("\t\touter vlan-xlate action: ");
            switch (action->outer_vlan_data.vid_assign_type)
            {
                case ACL_IGR_ACTION_OVLAN_ASSIGN_NEW_VID:
                    diag_util_mprintf("new vid:%d\n", action->outer_vlan_data.vid_value);
                    break;
                case ACL_IGR_ACTION_OVLAN_ASSIGN_SHIFT_VID:
                    diag_util_mprintf("shift vid:%d\n", action->outer_vlan_data.vid_value);
                    break;
                case ACL_IGR_ACTION_OVLAN_ASSIGN_SHIFT_FROM_INNER_VID:
                    diag_util_mprintf("shift from inner vid:%d\n", action->outer_vlan_data.vid_value);
                    break;
                case ACL_IGR_ACTION_OVLAN_ASSIGN_PORT_BASED_OUTER_VID:
                    diag_util_mprintf("shift from outer port-based vid\n");
                    break;
                default:
                    diag_util_mprintf("\n");
            }   /* end of switch (action->outer_vlan_data.vid_assign_type) */
        }   /* end of if (ENABLED == action->outer_vlan_assign_en) */
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

        /* priority action */
        diag_util_mprintf("\tNormal Port Priority action state: ");
        if (ENABLED == action->pri_en)
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);
            diag_util_mprintf("\t\tpriority:%d\n", action->pri_data.pri);
        }   /* end of if (ENABLED == action->pri_en) */
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

        /* priority for cpu port action */
        diag_util_mprintf("\tCPU Port Priority action state: ");
        if (ENABLED == action->cpu_pri_en)
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);
            diag_util_mprintf("\t\tpriority:%d\n", action->cpu_pri_data.pri);
        }   /* end of if (ENABLED == action->pri_en) */
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

        /*drop action */
        diag_util_mprintf("\tDrop action state: ");
        if (ACL_IGR_ACTION_DROP_DROP == action->drop_data)
        {
            diag_util_mprintf("Drop\n");
        }
        else if (ACL_IGR_ACTION_DROP_WITHDRAW == action->drop_data)
        {
            diag_util_mprintf("Withdraw Drop\n");
        }
        else
        {
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);
        }

        /* O-TPID action */
        diag_util_mprintf("\tO-TPID action state: ");
        if (ENABLED == action->otpid_en)
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);
            diag_util_mprintf("\t\tTPID Index:%d\n", action->otpid_data.tpid_idx);
        }   /* end of if (ENABLED == action->otpid_en) */
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

        /* I-TPID action */
        diag_util_mprintf("\tI-TPID action state: ");
        if (ENABLED == action->otpid_en)
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);
            diag_util_mprintf("\t\tTPID Index:%d\n", action->itpid_data.tpid_idx);
        }   /* end of if (ENABLED == action->otpid_en) */
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

        /*Filter action */
        diag_util_mprintf("\tFilter action state: ");
        if (ENABLED == action->filter_en)
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);
            if(ACL_IGR_ACTION_FLT_SINGLE_PORT == action->filter_data.flt_act)
            {
                diag_util_mprintf("filter singel port:%d\n", action->filter_data.flt_info);
            }
            else if(ACL_IGR_ACTION_FLT_MULTIPLE_PORTS == action->filter_data.flt_act)
            {
                diag_util_mprintf("filter multi ports, index:%d\n", action->filter_data.flt_info);
            }
        }   /* end of if (ENABLED == action->filter_en) */
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

        /* remark action */
        diag_util_mprintf("\tRemark action state: ");
        if (ENABLED == action->rmk_en)
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);
            diag_util_mprintf("\t\tremark ");

            switch (action->rmk_data.rmk_act)
            {
                case ACL_ACTION_REMARK_INNER_USER_PRI:
                    diag_util_mprintf("inner tag priority %d\n", action->rmk_data.rmk_info);
                    break;
                case ACL_ACTION_REMARK_OUTER_USER_PRI:
                    diag_util_mprintf("outer tag priority %d\n", action->rmk_data.rmk_info);
                    break;
                case ACL_ACTION_REMARK_DSCP:
                    diag_util_mprintf("DSCP %d\n", action->rmk_data.rmk_info);
                    break;
                case ACL_ACTION_REMARK_IP_PRECEDENCE:
                    diag_util_mprintf("IP percedence %d\n", action->rmk_data.rmk_info);
                    break;
                case ACL_ACTION_REMARK_COPY_IPRI_TO_OPRI:
                    diag_util_mprintf("outer tag priority from inner tag priority\n");
                    break;
                case ACL_ACTION_REMARK_COPY_OPRI_TO_IPRI:
                    diag_util_mprintf("inner tag priority from outer tag priority\n");
                    break;
                case ACL_ACTION_REMARK_KEEP_INNER_USER_PRI:
                    diag_util_mprintf("keep inner 1p priority\n");
                    break;
                case ACL_ACTION_REMARK_KEEP_OUTER_USER_PRI:
                    diag_util_mprintf("keep outer 1p priority\n");
                    break;
                default:
                    diag_util_mprintf("\n");
            }   /* end of switch (action->rmk_data.rmk_act) */
        }   /* end of if (ENABLED == action->rmk_en) */
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

        /* tag status action */
        diag_util_mprintf("\tTag status action state: ");
        if (ENABLED == action->tag_sts_en)
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);

            diag_util_mprintf("\t\tinner tag status: ");
            switch (action->tag_sts_data.itag_sts)
            {
                case ACL_ACTION_TAG_STS_UNTAG:
                    diag_util_mprintf("untagged\n");
                    break;
                case ACL_ACTION_TAG_STS_TAG:
                    diag_util_mprintf("tagged\n");
                    break;
                case ACL_ACTION_TAG_STS_KEEP_CONTENT:
                    diag_util_mprintf("keep-content\n");
                    break;
                case ACL_ACTION_TAG_STS_NOP:
                    diag_util_mprintf("reserved\n");
                    break;
                default:
                    diag_util_mprintf("\n");
            }   /* switch (action->tag_sts_data.itag_sts) */

            diag_util_mprintf("\t\touter tag status: ");
            switch (action->tag_sts_data.otag_sts)
            {
                case ACL_ACTION_TAG_STS_UNTAG:
                    diag_util_mprintf("untagged\n");
                    break;
                case ACL_ACTION_TAG_STS_TAG:
                    diag_util_mprintf("tagged\n");
                    break;
                case ACL_ACTION_TAG_STS_KEEP_CONTENT:
                    diag_util_mprintf("keep-content\n");
                    break;
                case ACL_ACTION_TAG_STS_NOP:
                    diag_util_mprintf("reserved\n");
                    break;
                default:
                    diag_util_mprintf("\n");
            }   /* switch (action->tag_sts_data.itag_sts) */
        }   /* end of if (ENABLED == action->tag_sts_en) */
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);
    }
#endif

    return ;
}   /* end of _diag_acl_igrAction_show */
#endif

#if defined(CONFIG_SDK_RTL8390)
static void
_diag_acl_egrAction_show(uint32 unit, rtk_acl_egrAction_t *action)
{
    /* forward action */
    diag_util_mprintf("\tForward action state: ");
    if (ENABLED == action->fwd_en)
    {
        diag_util_mprintf("%s\n", DIAG_STR_ENABLE);

        diag_util_mprintf("\t\taction: ");
        switch (action->fwd_data.fwd_type)
        {
            case ACL_ACTION_FWD_PERMIT:
                diag_util_mprintf("permit\n");
                break;
            case ACL_ACTION_FWD_DROP:
                diag_util_mprintf("drop\n");
                break;
            case ACL_ACTION_FWD_COPY_TO_PORTID:
                diag_util_mprintf("copy uni port:%d\n", action->fwd_data.fwd_info);
                break;
            case ACL_ACTION_FWD_COPY_TO_PORTMASK:
                diag_util_mprintf("copy multi index:%d\n", action->fwd_data.fwd_info);
                break;
            case ACL_ACTION_FWD_REDIRECT_TO_PORTID:
                diag_util_mprintf("redirect uni port:%d\n", action->fwd_data.fwd_info);
                break;
            case ACL_ACTION_FWD_REDIRECT_TO_PORTMASK:
                diag_util_mprintf("redirect multi index:%d\n", action->fwd_data.fwd_info);
                break;
            case ACL_ACTION_FWD_FILTERING:
                diag_util_mprintf("filtering index:%d\n", action->fwd_data.fwd_info);
                break;
            default:
                diag_util_mprintf("\n");
        }   /* end of switch (action->fwd_data.fwd_type) */
    }   /* if (ENABLED == action->fwd_en) */
    else
        diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

    /* Counter action */
    diag_util_mprintf("\tStatistic action state: ");
    if (ENABLED == action->stat_en)
    {
        diag_util_mprintf("%s\n", DIAG_STR_ENABLE);

        diag_util_mprintf("\t\taction: ");
        switch (action->stat_data.stat_type)
        {
            case STAT_TYPE_PACKET_BASED_32BIT:
                diag_util_mprintf("packet32 index:%d\n", action->stat_data.stat_idx);
                break;
            case STAT_TYPE_BYTE_BASED_64BIT:
                diag_util_mprintf("byte64 index:%d\n", action->stat_data.stat_idx);
                break;
            default:
                diag_util_mprintf("\n");
        }   /* end of switch (action->stat_data.stat_type) */
    }   /* if (ENABLED == action->stat_en) */
    else
        diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

    /* Mirror action */
    diag_util_mprintf("\tMirror action state: ");
    if (ENABLED == action->mirror_en)
    {
        diag_util_mprintf("%s\n", DIAG_STR_ENABLE);

        diag_util_mprintf("\t\tmirror type: ");
        if (ACL_ACTION_MIRROR_ORIGINAL == action->mirror_data.mirror_type)
            diag_util_mprintf("original\n");
        else
            diag_util_mprintf("modified\n");

        diag_util_mprintf("\t\tmirror index:%d\n", action->mirror_data.mirror_set_idx);
    }   /* end of if (ENABLED == action->mirror_en) */
    else
        diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

    /* Meter action */
    diag_util_mprintf("\tMeter action state: ");
    if (ENABLED == action->meter_en)
    {
        diag_util_mprintf("%s\n", DIAG_STR_ENABLE);
        diag_util_mprintf("\t\tmeter index:%d\n", action->meter_data.meter_idx);
    }   /* end of if (ENABLED == action->meter_en) */
    else
        diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

    /* inner vlan translate action */
    diag_util_mprintf("\tInner Vlan translate action state: ");
    if (ENABLED == action->inner_vlan_assign_en)
    {
        diag_util_mprintf("%s\n", DIAG_STR_ENABLE);

        diag_util_mprintf("\t\tinner vlan-xlate action: ");
        switch (action->inner_vlan_data.vid_assign_type)
        {
            case ACL_EGR_ACTION_IVLAN_ASSIGN_NEW_VID:
                diag_util_mprintf("new vid:%d\n", action->inner_vlan_data.vid_value);
                break;
            case ACL_EGR_ACTION_IVLAN_ASSIGN_SHIFT_VID:
                diag_util_mprintf("shift vid:%d\n", action->inner_vlan_data.vid_value);
                break;
            case ACL_EGR_ACTION_IVLAN_ASSIGN_SHIFT_FROM_OUTER_VID:
                diag_util_mprintf("shift from outer vid:%d\n", action->inner_vlan_data.vid_value);
                break;
            case ACL_ACTION_VLAN_ASSIGN_COPY_FROM_OUTER_VID:
                diag_util_mprintf("copy from outer vid:%d\n", action->inner_vlan_data.vid_value);
                break;
            default:
                diag_util_mprintf("\n");
        }   /* end of switch (action->inner_vlan_data.vid_assign_type) */

        /* inner TPID action */
        diag_util_mprintf("\tInner TPID action state: ");
        if (ENABLED == action->inner_vlan_data.tpid_assign)
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);
            diag_util_mprintf("\t\ttpid index:%d\n", action->inner_vlan_data.tpid_idx);
        }   /* end of if (ENABLED == action->tpid_assign) */
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);
    }   /* end of if (ENABLED == action->inner_vlan_assign_en) */
    else
        diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

    /* outer vlan translate action */
    diag_util_mprintf("\tOuter Vlan translate action state: ");
    if (ENABLED == action->outer_vlan_assign_en)
    {
        diag_util_mprintf("%s\n", DIAG_STR_ENABLE);

        diag_util_mprintf("\t\touter vlan-xlate action: ");
        switch (action->outer_vlan_data.vid_assign_type)
        {
            case ACL_EGR_ACTION_OVLAN_ASSIGN_NEW_VID:
                diag_util_mprintf("new vid:%d\n", action->outer_vlan_data.vid_value);
                break;
            case ACL_EGR_ACTION_OVLAN_ASSIGN_SHIFT_VID:
                diag_util_mprintf("shift vid:%d\n", action->outer_vlan_data.vid_value);
                break;
            case ACL_EGR_ACTION_OVLAN_ASSIGN_SHIFT_FROM_INNER_VID:
                diag_util_mprintf("shift from inner vid:%d\n", action->outer_vlan_data.vid_value);
                break;
            case ACL_EGR_ACTION_OVLAN_ASSIGN_COPY_FROM_INNER_VID:
                diag_util_mprintf("copy from inner vid:%d\n", action->outer_vlan_data.vid_value);
                break;
            default:
                diag_util_mprintf("\n");
        }   /* end of switch (action->outer_vlan_data.vid_assign_type) */

        /* outer TPID action */
        diag_util_mprintf("\tOuter TPID action state: ");
        if (ENABLED == action->outer_vlan_data.tpid_assign)
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);
            diag_util_mprintf("\t\ttpid index:%d\n", action->outer_vlan_data.tpid_idx);
        }   /* end of if (ENABLED == action->tpid_assign) */
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);
    }   /* end of if (ENABLED == action->outer_vlan_assign_en) */
    else
        diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

    /* priority action */
    diag_util_mprintf("\tPriority action state: ");
    if (ENABLED == action->pri_en)
    {
        diag_util_mprintf("%s\n", DIAG_STR_ENABLE);

        diag_util_mprintf("\t\tpriority type: ");
        if (0 == action->pri_data.pri_act)
            diag_util_mprintf("all ports\n");
        else
            diag_util_mprintf("cpu port only\n");

        diag_util_mprintf("\t\tpriority:%d\n", action->pri_data.pri);
    }   /* end of if (ENABLED == action->pri_en) */
    else
        diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

    /* remark action */
    diag_util_mprintf("\tRemark action state: ");
    if (ENABLED == action->rmk_en)
    {
        diag_util_mprintf("%s\n", DIAG_STR_ENABLE);
        diag_util_mprintf("\t\tremark ");

        switch (action->rmk_data.rmk_act)
        {
            case ACL_ACTION_REMARK_INNER_USER_PRI:
                diag_util_mprintf("inner tag priority %d\n", action->rmk_data.rmk_info);
                break;
            case ACL_ACTION_REMARK_OUTER_USER_PRI:
                diag_util_mprintf("outer tag priority %d\n", action->rmk_data.rmk_info);
                break;
            case ACL_ACTION_REMARK_DSCP:
                diag_util_mprintf("DSCP %d\n", action->rmk_data.rmk_info);
                break;
            case ACL_ACTION_REMARK_IP_PRECEDENCE:
                diag_util_mprintf("IP percedence %d\n", action->rmk_data.rmk_info);
                break;
            case ACL_ACTION_REMARK_COPY_IPRI_TO_OPRI:
                diag_util_mprintf("outer tag priority from inner tag priority\n");
                break;
            case ACL_ACTION_REMARK_COPY_OPRI_TO_IPRI:
                diag_util_mprintf("inner tag priority from outer tag priority\n");
                break;
            default:
                diag_util_mprintf("\n");
        }   /* end of switch (action->rmk_data.rmk_act) */
    }   /* end of if (ENABLED == action->rmk_en) */
    else
        diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

    /* tag status action */
    diag_util_mprintf("\tTag status action state: ");
    if (ENABLED == action->tag_sts_en)
    {
        diag_util_mprintf("%s\n", DIAG_STR_ENABLE);

        diag_util_mprintf("\t\tinner tag status: ");
        switch (action->tag_sts_data.itag_sts)
        {
            case ACL_ACTION_TAG_STS_UNTAG:
                diag_util_mprintf("untagged\n");
                break;
            case ACL_ACTION_TAG_STS_TAG:
                diag_util_mprintf("tagged\n");
                break;
            case ACL_ACTION_TAG_STS_KEEP_CONTENT:
                diag_util_mprintf("keep-content\n");
                break;
            case ACL_ACTION_TAG_STS_NOP:
                diag_util_mprintf("reserved\n");
                break;
            default:
                diag_util_mprintf("\n");
        }   /* switch (action->tag_sts_data.itag_sts) */

        diag_util_mprintf("\t\touter tag status: ");
        switch (action->tag_sts_data.otag_sts)
        {
            case ACL_ACTION_TAG_STS_UNTAG:
                diag_util_mprintf("untagged\n");
                break;
            case ACL_ACTION_TAG_STS_TAG:
                diag_util_mprintf("tagged\n");
                break;
            case ACL_ACTION_TAG_STS_KEEP_CONTENT:
                diag_util_mprintf("keep-content\n");
                break;
            case ACL_ACTION_TAG_STS_NOP:
                diag_util_mprintf("reserved\n");
                break;
            default:
                diag_util_mprintf("\n");
        }   /* switch (action->tag_sts_data.itag_sts) */
    }   /* end of if (ENABLED == action->tag_sts_en) */
    else
        diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

    return ;
}   /* end of _diag_acl_egrAction_show */
#endif

#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
static void
_diag_acl_vAct_show(uint32 unit, rtk_acl_vAct_t *action)
{
#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) ||
            DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        /* drop action */
        diag_util_mprintf("\tDrop action state: ");
        if (ENABLED == action->green_drop_en)
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);

            diag_util_mprintf("\t\taction: ");
            switch (action->green_drop_data)
            {
                case ACL_ACTION_COLOR_DROP_PERMIT:
                    diag_util_mprintf("permit\n");
                    break;
                case ACL_ACTION_COLOR_DROP_DROP:
                    diag_util_mprintf("drop\n");
                    break;
                default:
                    diag_util_mprintf("\n");
            }
        }
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

        diag_util_mprintf("\tYellow Drop action state: ");
        if (ENABLED == action->yellow_drop_en)
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);

            diag_util_mprintf("\t\taction: ");
            switch (action->yellow_drop_data)
            {
                case ACL_ACTION_COLOR_DROP_PERMIT:
                    diag_util_mprintf("permit\n");
                    break;
                case ACL_ACTION_COLOR_DROP_DROP:
                    diag_util_mprintf("drop\n");
                    break;
                default:
                    diag_util_mprintf("\n");
            }
        }
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

        diag_util_mprintf("\tRed Drop action state: ");
        if (ENABLED == action->red_drop_en)
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);

            diag_util_mprintf("\t\taction: ");
            switch (action->red_drop_data)
            {
                case ACL_ACTION_COLOR_DROP_PERMIT:
                    diag_util_mprintf("permit\n");
                    break;
                case ACL_ACTION_COLOR_DROP_DROP:
                    diag_util_mprintf("drop\n");
                    break;
                default:
                    diag_util_mprintf("\n");
            }
        }
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

        /* forward action */
        diag_util_mprintf("\tForward action state: ");
        if (ENABLED == action->fwd_en)
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);

            diag_util_mprintf("\t\taction: ");
            switch (action->fwd_data.fwd_type)
            {
                case ACL_ACTION_FWD_PERMIT:
                    diag_util_mprintf("permit\n");
                    break;
                case ACL_ACTION_FWD_DROP:
                    diag_util_mprintf("drop\n");
                    break;
                case ACL_ACTION_FWD_COPY_TO_PORTID:
                    if (action->fwd_data.flags & RTK_ACL_FWD_FLAG_TRUNK)
                    {
                        diag_util_mprintf("copy trunk:%d\n", action->fwd_data.fwd_info);
                    }
                    else
                    {
                        diag_util_mprintf("copy devID %d port:%d\n",
                                action->fwd_data.devID, action->fwd_data.fwd_info);
                    }
                    break;
                case ACL_ACTION_FWD_COPY_TO_PORTMASK:
                    diag_util_mprintf("copy multi index:%d\n", action->fwd_data.fwd_info);
                    break;
                case ACL_ACTION_FWD_LOOPBACK:
                    diag_util_mprintf("loopback\n");
                    break;
                case ACL_ACTION_FWD_REDIRECT_TO_PORTID:
                    if (action->fwd_data.flags & RTK_ACL_FWD_FLAG_TRUNK)
                    {
                        diag_util_mprintf("redirect trunk:%d\n", action->fwd_data.fwd_info);
                    }
                    else
                    {
                        diag_util_mprintf("redirect devID %d port:%d\n",
                                action->fwd_data.devID, action->fwd_data.fwd_info);
                    }
                    break;
                case ACL_ACTION_FWD_REDIRECT_TO_PORTMASK:
                    diag_util_mprintf("redirect multi index:%d\n", action->fwd_data.fwd_info);
                    break;
                case ACL_ACTION_FWD_UNICAST_ROUTING:
                    if (action->fwd_data.flags & RTK_ACL_FWD_FLAG_ECMP)
                        diag_util_mprintf("force route unicast ECMP index:%d\n", action->fwd_data.fwd_info);
                    else if (action->fwd_data.flags & RTK_ACL_FWD_FLAG_NULL_INTF_DROP)
                        diag_util_mprintf("force route unicast null interface drop\n");
                    else if (action->fwd_data.flags & RTK_ACL_FWD_FLAG_NULL_INTF_TRAP_LOCAL_CPU)
                        diag_util_mprintf("force route unicast null interface trap to local CPU\n");
                    else if (action->fwd_data.flags & RTK_ACL_FWD_FLAG_NULL_INTF_TRAP_MASTER_CPU)
                        diag_util_mprintf("force route unicast null interface trap to master CPU\n");
                    else
                        diag_util_mprintf("force route unicast index:%d\n", action->fwd_data.fwd_info);
                    break;
                case ACL_ACTION_FWD_DFLT_UNICAST_ROUTING:
                    if (action->fwd_data.flags & RTK_ACL_FWD_FLAG_ECMP)
                        diag_util_mprintf("default route unicast ECMP index:%d\n", action->fwd_data.fwd_info);
                    else if (action->fwd_data.flags & RTK_ACL_FWD_FLAG_NULL_INTF_DROP)
                        diag_util_mprintf("default route unicast null interface drop\n");
                    else if (action->fwd_data.flags & RTK_ACL_FWD_FLAG_NULL_INTF_TRAP_LOCAL_CPU)
                        diag_util_mprintf("default route unicast null interface trap to local CPU\n");
                    else if (action->fwd_data.flags & RTK_ACL_FWD_FLAG_NULL_INTF_TRAP_MASTER_CPU)
                        diag_util_mprintf("default route unicast null interface trap to master CPU\n");
                    else
                        diag_util_mprintf("default route unicast index:%d\n", action->fwd_data.fwd_info);
                    break;
                default:
                    diag_util_mprintf("\n");
            }   /* end of switch (action->fwd_data.fwd_type) */

            diag_util_mprintf("\t\tPacket format to CPU: ");
            if(ORIGINAL_PACKET == action->fwd_data.fwd_cpu_fmt)
                diag_util_mprintf("original packet\n");
            else if(MODIFIED_PACKET == action->fwd_data.fwd_cpu_fmt)
                diag_util_mprintf("modified packet\n");
            else
                diag_util_mprintf("\n");

            diag_util_mprintf("\t\tSA learning: ");
            if (action->fwd_data.flags & RTK_ACL_FWD_FLAG_SA_NOT_LEARN)
                diag_util_mprintf("not learn\n");
            else
                diag_util_mprintf("NULL operation\n");

            diag_util_mprintf("\t\tDrop and Forward action precedence select: \n");
            if (action->fwd_data.flags & RTK_ACL_FWD_FLAG_OVERWRITE_DROP)
                diag_util_mprintf("\t\t\tForward action has higher priority\n");
            else
                diag_util_mprintf("\t\t\tDrop action has higher priority\n");
        }   /* if (ENABLED == action->fwd_en) */
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

        /* Counter action */
        diag_util_mprintf("\tStatistic action state: ");
        if (ENABLED == action->stat_en)
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);
            #if defined(CONFIG_SDK_RTL9300)
            if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID))
            {
                diag_util_mprintf("\t\taction: ");
                switch (action->stat_data.stat_type)
                {
                    case STAT_TYPE_PACKET_BASED_32BIT:
                        diag_util_mprintf("packet32\n");
                        break;
                    case STAT_TYPE_BYTE_BASED_64BIT:
                        diag_util_mprintf("byte64\n");
                        break;
                    default:
                        diag_util_mprintf("\n");
                }   /* end of switch (action->stat_data.stat_type) */
            }
            #endif
        }   /* if (ENABLED == action->stat_en) */
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

        /* Mirror action */
        diag_util_mprintf("\tMirror action state: ");
        if (ENABLED == action->mirror_en)
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);
            switch (action->mirror_data.mirror_type)
            {
                case ACL_ACTION_MIRROR_ORIGINAL:
                    diag_util_mprintf("\t\tmirror type: original\n");
                    break;
                case ACL_ACTION_MIRROR_CANCEL:
                    diag_util_mprintf("\t\tcancel mirror\n");
                    break;
                default:
                    diag_util_mprintf("\n");
            }

            diag_util_mprintf("\t\tmirror index:%d\n", action->mirror_data.mirror_set_idx);
        }   /* end of if (ENABLED == action->mirror_en) */
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

        /* Meter action */
        diag_util_mprintf("\tMeter action state: ");
        if (ENABLED == action->meter_en)
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);
            diag_util_mprintf("\t\tmeter index:%d\n", action->meter_data.meter_idx);
        }   /* end of if (ENABLED == action->meter_en) */
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

        /* inner vlan translate action */
        diag_util_mprintf("\tInner Vlan translate action state: ");
        if (ENABLED == action->inner_vlan_assign_en)
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);

            diag_util_mprintf("\t\tinner vlan-xlate action: ");
            switch (action->inner_vlan_data.vid_assign_type)
            {
                case ACL_ACTION_VLAN_ASSIGN_NEW_VID:
                    diag_util_mprintf("new vid:%d\n", action->inner_vlan_data.vid_value);
                    break;
                case ACL_ACTION_VLAN_ASSIGN_SHIFT_VID:
                    diag_util_mprintf("shift vid:%d\n", action->inner_vlan_data.vid_value);
                    break;
                case ACL_ACTION_VLAN_ASSIGN_SHIFT_FROM_OUTER_VID:
                    diag_util_mprintf("shift from outer vid:%d\n", action->inner_vlan_data.vid_value);
                    break;
                default:
                    diag_util_mprintf("\n");
            }   /* end of switch (action->inner_vlan_data.vid_assign_type) */
        }   /* end of if (ENABLED == action->inner_vlan_assign_en) */
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

        /* inner priority action */
        diag_util_mprintf("\tInner Priority action state: ");
        if (ENABLED == action->inner_pri_en)
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);

            diag_util_mprintf("\t\taction: ");
            switch (action->inner_pri_data.act)
            {
                case ACL_ACTION_INNER_PRI_ASSIGN_NEW_PRI:
                    diag_util_mprintf("new priority:%d\n", action->inner_pri_data.pri);
                    break;
                case ACL_ACTION_INNER_PRI_COPY_FROM_OUTER:
                    diag_util_mprintf("copy from outer tag priority\n");
                    break;
                case ACL_ACTION_INNER_PRI_KEEP:
                    diag_util_mprintf("keep inner tag priority\n");
                    break;
                default:
                    diag_util_mprintf("\n");
            }
        }
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

        /* outer vlan translate action */
        diag_util_mprintf("\tOuter Vlan translate action state: ");
        if (ENABLED == action->outer_vlan_assign_en)
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);

            diag_util_mprintf("\t\touter vlan-xlate action: ");
            switch (action->outer_vlan_data.vid_assign_type)
            {
                case ACL_ACTION_VLAN_ASSIGN_NEW_VID:
                    diag_util_mprintf("new vid:%d\n", action->outer_vlan_data.vid_value);
                    break;
                case ACL_ACTION_VLAN_ASSIGN_SHIFT_VID:
                    diag_util_mprintf("shift vid:%d\n", action->outer_vlan_data.vid_value);
                    break;
                case ACL_ACTION_VLAN_ASSIGN_SHIFT_FROM_INNER_VID:
                    diag_util_mprintf("shift from inner vid:%d\n", action->outer_vlan_data.vid_value);
                    break;
                default:
                    diag_util_mprintf("\n");
            }   /* end of switch (action->outer_vlan_data.vid_assign_type) */
        }   /* end of if (ENABLED == action->outer_vlan_assign_en) */
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

        /* outer priority action */
        diag_util_mprintf("\tOuter Priority action state: ");
        if (ENABLED == action->outer_pri_en)
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);

            diag_util_mprintf("\t\taction: ");
            switch (action->outer_pri_data.act)
            {
                case ACL_ACTION_OUTER_PRI_ASSIGN_NEW_PRI:
                    diag_util_mprintf("new priority:%d\n", action->outer_pri_data.pri);
                    break;
                case ACL_ACTION_OUTER_PRI_COPY_FROM_INNER:
                    diag_util_mprintf("copy from inner tag priority\n");
                    break;
                case ACL_ACTION_OUTER_PRI_KEEP:
                    diag_util_mprintf("keep outer tag priority\n");
                    break;
                default:
                    diag_util_mprintf("\n");
            }
        }
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

       /* tag status action */
        diag_util_mprintf("\tTag status action state: ");
        if (ENABLED == action->tag_sts_en)
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);

            diag_util_mprintf("\t\tinner tag status: ");
            switch (action->tag_sts_data.itag_sts)
            {
                case ACL_ACTION_TAG_STS_UNTAG:
                    diag_util_mprintf("untagged\n");
                    break;
                case ACL_ACTION_TAG_STS_TAG:
                    diag_util_mprintf("tagged\n");
                    break;
                case ACL_ACTION_TAG_STS_KEEP_FORMAT:
                    diag_util_mprintf("keep-format\n");
                    break;
                case ACL_ACTION_TAG_STS_NOP:
                    diag_util_mprintf("reserved\n");
                    break;
                default:
                    diag_util_mprintf("\n");
            }   /* switch (action->tag_sts_data.itag_sts) */

            diag_util_mprintf("\t\touter tag status: ");
            switch (action->tag_sts_data.otag_sts)
            {
                case ACL_ACTION_TAG_STS_UNTAG:
                    diag_util_mprintf("untagged\n");
                    break;
                case ACL_ACTION_TAG_STS_TAG:
                    diag_util_mprintf("tagged\n");
                    break;
                case ACL_ACTION_TAG_STS_KEEP_FORMAT:
                    diag_util_mprintf("keep-format\n");
                    break;
                case ACL_ACTION_TAG_STS_NOP:
                    diag_util_mprintf("reserved\n");
                    break;
                default:
                    diag_util_mprintf("\n");
            }   /* switch (action->tag_sts_data.itag_sts) */
        }   /* end of if (ENABLED == action->tag_sts_en) */
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

        /* priority action */
        diag_util_mprintf("\tInternal Priority action state: ");
        if (ENABLED == action->pri_en)
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);
            diag_util_mprintf("\t\tpriority:%d\n", action->pri_data.pri);
        }   /* end of if (ENABLED == action->pri_en) */
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

        /* bypass action */
        diag_util_mprintf("\tBypass action state: ");
        if (ENABLED == action->bypass_en)
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);

            diag_util_mprintf("\t\tigr-bw-ctrl state: ");
            if (ENABLED == action->bypass_data.ibc_sc)
                diag_util_mprintf("%s\n", DIAG_STR_ENABLE);
            else
                diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

            diag_util_mprintf("\t\tigr-stp state: ");
            if (ENABLED == action->bypass_data.igr_stp)
                diag_util_mprintf("%s\n", DIAG_STR_ENABLE);
            else
                diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

            diag_util_mprintf("\t\tigr-vlan state: ");
            if (ENABLED == action->bypass_data.igr_vlan)
                diag_util_mprintf("%s\n", DIAG_STR_ENABLE);
            else
                diag_util_mprintf("%s\n", DIAG_STR_DISABLE);
        }   /* end of if (ENABLED == action->bypass_en) */
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

        /* meta data action */
        diag_util_mprintf("\tMeta Data action state: ");
        if (ENABLED == action->meta_data_en)
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);
            diag_util_mprintf("\t\tmeta-data:%d\n", action->meta_data.data);
            #if defined(CONFIG_SDK_RTL9310)
            if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
            {
                diag_util_mprintf("\t\tmeta-data mask:%d\n", action->meta_data.mask);
            }
            #endif
        }
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

        diag_util_mprintf("\tIP Reserved Flag Invert: %s\n",(ENABLED == action->invert_en)?"Enable":"Disable");

        /* cpu qid action */
        diag_util_mprintf("\tCPU QID action state: ");
        if (ENABLED == action->cpu_qid_en)
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);
            #if defined(CONFIG_SDK_RTL9310)
            if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
            {
                if (ACL_ACTION_QID_IGR == action->cpu_qid.act)
                {
                    diag_util_mprintf("\t\tAction: ingress queue\n");
                }
                else
                {
                    diag_util_mprintf("\t\tAction: CPU queue\n");
                }
            }
            #endif
            diag_util_mprintf("\t\tqid:%d\n", action->cpu_qid.qid);
        }
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

        /* remark action */
        diag_util_mprintf("\tRemark action state: ");
        if (ENABLED == action->rmk_en)
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);
            diag_util_mprintf("\t\tremark ");

            switch (action->rmk_data.rmk_act)
            {
                case ACL_ACTION_REMARK_DSCP:
                    diag_util_mprintf("DSCP %d\n", action->rmk_data.rmk_info);
                    break;
                case ACL_ACTION_REMARK_IP_PRECEDENCE:
                    diag_util_mprintf("IP percedence %d\n", action->rmk_data.rmk_info);
                    break;
                case ACL_ACTION_REMARK_TOS:
                    diag_util_mprintf("TOS %d\n", action->rmk_data.rmk_info);
                    break;
                case ACL_ACTION_REMARK_EAV:
                    {
                        if(0 == action->rmk_data.rmk_info)
                            diag_util_mprintf("EAV: non-EAV\n");
                        else if(1 == action->rmk_data.rmk_info)
                            diag_util_mprintf("EAV: class A\n");
                        else if(2 == action->rmk_data.rmk_info)
                            diag_util_mprintf("EAV: class B\n");
                        else
                            diag_util_mprintf("EAV: \n");
                    }
                    break;
                default:
                    diag_util_mprintf("\n");
            }   /* end of switch (action->rmk_data.rmk_act) */
        }   /* end of if (ENABLED == action->rmk_en) */
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

        diag_util_mprintf("\tYellow Remark action state: ");
        if (ENABLED == action->yellow_rmk_en)
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);
            diag_util_mprintf("\t\tremark ");

            switch (action->yellow_rmk_data.rmk_act)
            {
                case ACL_ACTION_REMARK_DSCP:
                    diag_util_mprintf("DSCP %d\n", action->yellow_rmk_data.rmk_info);
                    break;
                case ACL_ACTION_REMARK_IP_PRECEDENCE:
                    diag_util_mprintf("IP percedence %d\n", action->yellow_rmk_data.rmk_info);
                    break;
                case ACL_ACTION_REMARK_TOS:
                    diag_util_mprintf("TOS %d\n", action->yellow_rmk_data.rmk_info);
                    break;
                case ACL_ACTION_REMARK_EAV:
                    {
                        if(0 == action->yellow_rmk_data.rmk_info)
                            diag_util_mprintf("EAV: non-EAV\n");
                        else if(1 == action->yellow_rmk_data.rmk_info)
                            diag_util_mprintf("EAV: class A\n");
                        else if(2 == action->yellow_rmk_data.rmk_info)
                            diag_util_mprintf("EAV: class B\n");
                        else
                            diag_util_mprintf("EAV: \n");
                    }
                    break;
                default:
                    diag_util_mprintf("\n");
            }   /* end of switch (action->rmk_data.rmk_act) */
        }   /* end of if (ENABLED == action->rmk_en) */
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

        diag_util_mprintf("\tRed Remark action state: ");
        if (ENABLED == action->red_rmk_en)
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);
            diag_util_mprintf("\t\tremark ");

            switch (action->red_rmk_data.rmk_act)
            {
                case ACL_ACTION_REMARK_DSCP:
                    diag_util_mprintf("DSCP %d\n", action->red_rmk_data.rmk_info);
                    break;
                case ACL_ACTION_REMARK_IP_PRECEDENCE:
                    diag_util_mprintf("IP percedence %d\n", action->red_rmk_data.rmk_info);
                    break;
                case ACL_ACTION_REMARK_TOS:
                    diag_util_mprintf("TOS %d\n", action->red_rmk_data.rmk_info);
                    break;
                case ACL_ACTION_REMARK_EAV:
                    {
                        if(0 == action->red_rmk_data.rmk_info)
                            diag_util_mprintf("EAV: non-EAV\n");
                        else if(1 == action->red_rmk_data.rmk_info)
                            diag_util_mprintf("EAV: class A\n");
                        else if(2 == action->red_rmk_data.rmk_info)
                            diag_util_mprintf("EAV: class B\n");
                        else
                            diag_util_mprintf("EAV: \n");
                    }
                    break;
                default:
                    diag_util_mprintf("\n");
            }   /* end of switch (action->rmk_data.rmk_act) */
        }   /* end of if (ENABLED == action->rmk_en) */
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);
    }
#endif

    return ;
}   /* end of _diag_acl_vAct_show */

static void
_diag_acl_iAct_show(uint32 unit, rtk_acl_iAct_t *action)
{
#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID) ||
            DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        /* drop action */
        diag_util_mprintf("\tDrop action state: ");
        if (ENABLED == action->green_drop_en)
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);

            diag_util_mprintf("\t\taction: ");
            switch (action->green_drop_data)
            {
                case ACL_ACTION_COLOR_DROP_PERMIT:
                    diag_util_mprintf("permit\n");
                    break;
                case ACL_ACTION_COLOR_DROP_DROP:
                    diag_util_mprintf("drop\n");
                    break;
                default:
                    diag_util_mprintf("\n");
            }
        }
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

        diag_util_mprintf("\tYellow Drop action state: ");
        if (ENABLED == action->yellow_drop_en)
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);

            diag_util_mprintf("\t\taction: ");
            switch (action->yellow_drop_data)
            {
                case ACL_ACTION_COLOR_DROP_PERMIT:
                    diag_util_mprintf("permit\n");
                    break;
                case ACL_ACTION_COLOR_DROP_DROP:
                    diag_util_mprintf("drop\n");
                    break;
                default:
                    diag_util_mprintf("\n");
            }
        }
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

        diag_util_mprintf("\tRed Drop action state: ");
        if (ENABLED == action->red_drop_en)
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);

            diag_util_mprintf("\t\taction: ");
            switch (action->red_drop_data)
            {
                case ACL_ACTION_COLOR_DROP_PERMIT:
                    diag_util_mprintf("permit\n");
                    break;
                case ACL_ACTION_COLOR_DROP_DROP:
                    diag_util_mprintf("drop\n");
                    break;
                default:
                    diag_util_mprintf("\n");
            }
        }
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

        /* forward action */
        diag_util_mprintf("\tForward action state: ");
        if (ENABLED == action->fwd_en)
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);

            diag_util_mprintf("\t\taction: ");
            switch (action->fwd_data.fwd_type)
            {
                case ACL_ACTION_FWD_PERMIT:
                    diag_util_mprintf("permit\n");
                    break;
                case ACL_ACTION_FWD_DROP:
                    diag_util_mprintf("drop\n");
                    break;
                case ACL_ACTION_FWD_COPY_TO_PORTID:
                    if (action->fwd_data.flags & RTK_ACL_FWD_FLAG_TRUNK)
                    {
                        diag_util_mprintf("copy trunk:%d\n", action->fwd_data.fwd_info);
                    }
                    else
                    {
                        diag_util_mprintf("copy devID %d port:%d\n",
                                action->fwd_data.devID, action->fwd_data.fwd_info);
                    }
                    break;
                case ACL_ACTION_FWD_COPY_TO_PORTMASK:
                    diag_util_mprintf("copy multi index:%d\n", action->fwd_data.fwd_info);
                    break;
                case ACL_ACTION_FWD_REDIRECT_TO_PORTID:
                    if (action->fwd_data.flags & RTK_ACL_FWD_FLAG_TRUNK)
                    {
                        diag_util_mprintf("redirect trunk:%d\n", action->fwd_data.fwd_info);
                    }
                    else
                    {
                        diag_util_mprintf("redirect devID %d port:%d\n",
                                action->fwd_data.devID, action->fwd_data.fwd_info);
                    }
                    break;
                case ACL_ACTION_FWD_REDIRECT_TO_PORTMASK:
                    diag_util_mprintf("redirect multi index:%d\n", action->fwd_data.fwd_info);
                    break;
                case ACL_ACTION_FWD_FILTERING:
                    diag_util_mprintf("filtering index:%d\n", action->fwd_data.fwd_info);
                    break;
                case ACL_ACTION_FWD_LOOPBACK:
                    diag_util_mprintf("loopback\n");
                    break;
                default:
                    diag_util_mprintf("\n");
            }   /* end of switch (action->fwd_data.fwd_type) */

            diag_util_mprintf("\t\tPacket format to CPU: ");
            if(ORIGINAL_PACKET == action->fwd_data.fwd_cpu_fmt)
                diag_util_mprintf("original packet\n");
            else if(MODIFIED_PACKET == action->fwd_data.fwd_cpu_fmt)
                diag_util_mprintf("modified packet\n");
            else
                diag_util_mprintf("\n");

            diag_util_mprintf("\t\tDrop and Forward action precedence select: \n");
            if (action->fwd_data.flags & RTK_ACL_FWD_FLAG_OVERWRITE_DROP)
                diag_util_mprintf("\t\t\tForward action has higher priority\n");
            else
                diag_util_mprintf("\t\t\tDrop action has higher priority\n");
        }   /* if (ENABLED == action->fwd_en) */
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

        /* Counter action */
        diag_util_mprintf("\tStatistic action state: ");
        if (ENABLED == action->stat_en)
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);
            #if defined(CONFIG_SDK_RTL9300)
            if (DIAG_OM_GET_FAMILYID(RTL9300_FAMILY_ID))
            {
                diag_util_mprintf("\t\taction: ");
                switch (action->stat_data.stat_type)
                {
                    case STAT_TYPE_PACKET_BASED_32BIT:
                        diag_util_mprintf("packet32\n");
                        break;
                    case STAT_TYPE_BYTE_BASED_64BIT:
                        diag_util_mprintf("byte64\n");
                        break;
                    default:
                        diag_util_mprintf("\n");
                }   /* end of switch (action->stat_data.stat_type) */
            }
            #endif
        }   /* if (ENABLED == action->stat_en) */
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

        /* Mirror action */
        diag_util_mprintf("\tMirror action state: ");
        if (ENABLED == action->mirror_en)
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);
            switch (action->mirror_data.mirror_type)
            {
                case ACL_ACTION_MIRROR_ORIGINAL:
                    diag_util_mprintf("\t\tmirror type: original\n");
                    break;
                case ACL_ACTION_MIRROR_MODIFIED:
                    diag_util_mprintf("\t\tmirror type: modified\n");
                    break;
                case ACL_ACTION_MIRROR_CANCEL:
                    diag_util_mprintf("\t\tcancel mirror\n");
                    break;
                default:
                    diag_util_mprintf("\n");
            }
            diag_util_mprintf("\t\tmirror index:%d\n", action->mirror_data.mirror_set_idx);
        }   /* end of if (ENABLED == action->mirror_en) */
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

        /* Meter action */
        diag_util_mprintf("\tMeter action state: ");
        if (ENABLED == action->meter_en)
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);
            diag_util_mprintf("\t\tmeter index:%d\n", action->meter_data.meter_idx);
        }   /* end of if (ENABLED == action->meter_en) */
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

        /* inner vlan translate action */
        diag_util_mprintf("\tInner Vlan translate action state: ");
        if (ENABLED == action->inner_vlan_assign_en)
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);

            diag_util_mprintf("\t\tinner vlan-xlate action: ");
            switch (action->inner_vlan_data.vid_assign_type)
            {
                case ACL_ACTION_VLAN_ASSIGN_NEW_VID:
                    diag_util_mprintf("new vid:%d\n", action->inner_vlan_data.vid_value);
                    break;
                case ACL_ACTION_VLAN_ASSIGN_SHIFT_VID:
                    diag_util_mprintf("shift vid:%d\n", action->inner_vlan_data.vid_value);
                    break;
                case ACL_ACTION_VLAN_ASSIGN_SHIFT_FROM_OUTER_VID:
                    diag_util_mprintf("shift from outer vid:%d\n", action->inner_vlan_data.vid_value);
                    break;
                default:
                    diag_util_mprintf("\n");
            }   /* end of switch (action->inner_vlan_data.vid_assign_type) */

            diag_util_mprintf("\t\tinner tpid action: ");
            if(ENABLED == action->inner_vlan_data.tpid_assign)
                diag_util_mprintf("%s\ttpid index:%d\n", DIAG_STR_ENABLE, action->inner_vlan_data.tpid_idx);
            else
                diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

        }   /* end of if (ENABLED == action->inner_vlan_assign_en) */
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

        /* inner priority action */
        diag_util_mprintf("\tInner Priority action state: ");
        if (ENABLED == action->inner_pri_en)
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);

            diag_util_mprintf("\t\taction: ");
            switch (action->inner_pri_data.act)
            {
                case ACL_ACTION_INNER_PRI_ASSIGN_NEW_PRI:
                    diag_util_mprintf("new priority:%d\n", action->inner_pri_data.pri);
                    break;
                case ACL_ACTION_INNER_PRI_COPY_FROM_OUTER:
                    diag_util_mprintf("copy from outer tag priority\n");
                    break;
                case ACL_ACTION_INNER_PRI_KEEP:
                    diag_util_mprintf("keep inner tag priority\n");
                    break;
                default:
                    diag_util_mprintf("\n");
            }
        }
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

        /* outer vlan translate action */
        diag_util_mprintf("\tOuter Vlan translate action state: ");
        if (ENABLED == action->outer_vlan_assign_en)
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);

            diag_util_mprintf("\t\touter vlan-xlate action: ");
            switch (action->outer_vlan_data.vid_assign_type)
            {
                case ACL_ACTION_VLAN_ASSIGN_NEW_VID:
                    diag_util_mprintf("new vid:%d\n", action->outer_vlan_data.vid_value);
                    break;
                case ACL_ACTION_VLAN_ASSIGN_SHIFT_VID:
                    diag_util_mprintf("shift vid:%d\n", action->outer_vlan_data.vid_value);
                    break;
                case ACL_ACTION_VLAN_ASSIGN_SHIFT_FROM_INNER_VID:
                    diag_util_mprintf("shift from inner vid:%d\n", action->outer_vlan_data.vid_value);
                    break;
                default:
                    diag_util_mprintf("\n");
            }   /* end of switch (action->outer_vlan_data.vid_assign_type) */
            diag_util_mprintf("\t\touter tpid action: ");
            if(ENABLED == action->outer_vlan_data.tpid_assign)
                diag_util_mprintf("Enabled\ttpid index:%d\n", action->outer_vlan_data.tpid_idx);
            else
                diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

        }   /* end of if (ENABLED == action->outer_vlan_assign_en) */
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

        /* outer priority action */
        diag_util_mprintf("\tOuter Priority action state: ");
        if (ENABLED == action->outer_pri_en)
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);

            diag_util_mprintf("\t\taction: ");
            switch (action->outer_pri_data.act)
            {
                case ACL_ACTION_OUTER_PRI_ASSIGN_NEW_PRI:
                    diag_util_mprintf("new priority:%d\n", action->outer_pri_data.pri);
                    break;
                case ACL_ACTION_OUTER_PRI_COPY_FROM_INNER:
                    diag_util_mprintf("copy from inner tag priority\n");
                    break;
                case ACL_ACTION_OUTER_PRI_KEEP:
                    diag_util_mprintf("keep outer tag priority\n");
                    break;
                default:
                    diag_util_mprintf("\n");
            }
        }
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

       /* tag status action */
        diag_util_mprintf("\tTag status action state: ");
        if (ENABLED == action->tag_sts_en)
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);

            diag_util_mprintf("\t\tinner tag status: ");
            switch (action->tag_sts_data.itag_sts)
            {
                case ACL_ACTION_TAG_STS_UNTAG:
                    diag_util_mprintf("untagged\n");
                    break;
                case ACL_ACTION_TAG_STS_TAG:
                    diag_util_mprintf("tagged\n");
                    break;
                case ACL_ACTION_TAG_STS_KEEP_FORMAT:
                    diag_util_mprintf("keep-content\n");
                    break;
                case ACL_ACTION_TAG_STS_NOP:
                    diag_util_mprintf("reserved\n");
                    break;
                default:
                    diag_util_mprintf("\n");
            }   /* switch (action->tag_sts_data.itag_sts) */

            diag_util_mprintf("\t\touter tag status: ");
            switch (action->tag_sts_data.otag_sts)
            {
                case ACL_ACTION_TAG_STS_UNTAG:
                    diag_util_mprintf("untagged\n");
                    break;
                case ACL_ACTION_TAG_STS_TAG:
                    diag_util_mprintf("tagged\n");
                    break;
                case ACL_ACTION_TAG_STS_KEEP_FORMAT:
                    diag_util_mprintf("keep-content\n");
                    break;
                case ACL_ACTION_TAG_STS_NOP:
                    diag_util_mprintf("reserved\n");
                    break;
                default:
                    diag_util_mprintf("\n");
            }   /* switch (action->tag_sts_data.itag_sts) */
        }   /* end of if (ENABLED == action->tag_sts_en) */
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

        /* priority action */
        diag_util_mprintf("\tInternal Priority action state: ");
        if (ENABLED == action->pri_en)
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);
            diag_util_mprintf("\t\tpriority:%d\n", action->pri_data.pri);
        }   /* end of if (ENABLED == action->pri_en) */
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

        /* bypass action */
        diag_util_mprintf("\tBypass action state: ");
        if (ENABLED == action->bypass_en)
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);

            diag_util_mprintf("\t\tigr-bw-ctrl state: ");
            if (ENABLED == action->bypass_data.ibc_sc)
                diag_util_mprintf("%s\n", DIAG_STR_ENABLE);
            else
                diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

            #if defined(CONFIG_SDK_RTL9310)
            if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
            {
                diag_util_mprintf("\t\tegr-vlan state: ");
                if (ENABLED == action->bypass_data.egr_vlan)
                    diag_util_mprintf("%s\n", DIAG_STR_ENABLE);
                else
                    diag_util_mprintf("%s\n", DIAG_STR_DISABLE);
            }
            #endif  /*  defined(CONFIG_SDK_RTL9310) */
        }   /* end of if (ENABLED == action->bypass_en) */
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

        /* cpu qid action */
        diag_util_mprintf("\tCPU QID action state: ");
        if (ENABLED == action->cpu_qid_en)
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);
            diag_util_mprintf("\t\tqid:%d\n", action->cpu_qid.qid);
        }
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

        /* remark action */
        diag_util_mprintf("\tRemark action state: ");
        if (ENABLED == action->rmk_en)
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);
            diag_util_mprintf("\t\tremark ");

            switch (action->rmk_data.rmk_act)
            {
                case ACL_ACTION_REMARK_DSCP:
                    diag_util_mprintf("DSCP %d\n", action->rmk_data.rmk_info);
                    break;
                case ACL_ACTION_REMARK_IP_PRECEDENCE:
                    diag_util_mprintf("IP percedence %d\n", action->rmk_data.rmk_info);
                    break;
                case ACL_ACTION_REMARK_TOS:
                    diag_util_mprintf("TOS %d\n", action->rmk_data.rmk_info);
                    break;
                case ACL_ACTION_REMARK_EAV:
                    {
                        if(0 == action->rmk_data.rmk_info)
                            diag_util_mprintf("EAV: non-EAV\n");
                        else if(1 == action->rmk_data.rmk_info)
                            diag_util_mprintf("EAV: class A\n");
                        else if(2 == action->rmk_data.rmk_info)
                            diag_util_mprintf("EAV: class B\n");
                        else
                            diag_util_mprintf("EAV: \n");
                    }
                    break;
                default:
                    diag_util_mprintf("\n");
            }   /* end of switch (action->rmk_data.rmk_act) */
        }   /* end of if (ENABLED == action->rmk_en) */
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

        diag_util_mprintf("\tYellow Remark action state: ");
        if (ENABLED == action->yellow_rmk_en)
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);
            diag_util_mprintf("\t\tremark ");

            switch (action->yellow_rmk_data.rmk_act)
            {
                case ACL_ACTION_REMARK_DSCP:
                    diag_util_mprintf("DSCP %d\n", action->yellow_rmk_data.rmk_info);
                    break;
                case ACL_ACTION_REMARK_IP_PRECEDENCE:
                    diag_util_mprintf("IP percedence %d\n", action->yellow_rmk_data.rmk_info);
                    break;
                case ACL_ACTION_REMARK_TOS:
                    diag_util_mprintf("TOS %d\n", action->yellow_rmk_data.rmk_info);
                    break;
                case ACL_ACTION_REMARK_EAV:
                    {
                        if(0 == action->yellow_rmk_data.rmk_info)
                            diag_util_mprintf("EAV: non-EAV\n");
                        else if(1 == action->yellow_rmk_data.rmk_info)
                            diag_util_mprintf("EAV: class A\n");
                        else if(2 == action->yellow_rmk_data.rmk_info)
                            diag_util_mprintf("EAV: class B\n");
                        else
                            diag_util_mprintf("EAV: \n");
                    }
                    break;
                default:
                    diag_util_mprintf("\n");
            }   /* end of switch (action->rmk_data.rmk_act) */
        }   /* end of if (ENABLED == action->rmk_en) */
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

        diag_util_mprintf("\tRed Remark action state: ");
        if (ENABLED == action->red_rmk_en)
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);
            diag_util_mprintf("\t\tremark ");

            switch (action->red_rmk_data.rmk_act)
            {
                case ACL_ACTION_REMARK_DSCP:
                    diag_util_mprintf("DSCP %d\n", action->red_rmk_data.rmk_info);
                    break;
                case ACL_ACTION_REMARK_IP_PRECEDENCE:
                    diag_util_mprintf("IP percedence %d\n", action->red_rmk_data.rmk_info);
                    break;
                case ACL_ACTION_REMARK_TOS:
                    diag_util_mprintf("TOS %d\n", action->red_rmk_data.rmk_info);
                    break;
                case ACL_ACTION_REMARK_EAV:
                    {
                        if(0 == action->red_rmk_data.rmk_info)
                            diag_util_mprintf("EAV: non-EAV\n");
                        else if(1 == action->red_rmk_data.rmk_info)
                            diag_util_mprintf("EAV: class A\n");
                        else if(2 == action->red_rmk_data.rmk_info)
                            diag_util_mprintf("EAV: class B\n");
                        else
                            diag_util_mprintf("EAV: \n");
                    }
                    break;
                default:
                    diag_util_mprintf("\n");
            }   /* end of switch (action->rmk_data.rmk_act) */
        }   /* end of if (ENABLED == action->rmk_en) */
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

        diag_util_mprintf("\tMeta Data action state: ");
        if (ENABLED == action->meta_data_en)
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);
            diag_util_mprintf("\t\tmeta-data:%d\n", action->meta_data.data);
            #if defined(CONFIG_SDK_RTL9310)
            if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
            {
                diag_util_mprintf("\t\tmeta-data mask:%d\n", action->meta_data.mask);
            }
            #endif
        }
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

        diag_util_mprintf("\tIP Reserved Flag Invert: %s\n",(ENABLED == action->invert_en)?"Enable":"Disable");
    }
#endif

    return ;
}   /* end of _diag_acl_iAct_show */
#endif  /* defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310) */

#if defined(CONFIG_SDK_RTL9310)
static void
_diag_acl_eAct_show(uint32 unit, rtk_acl_eAct_t *action)
{
    if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        /* drop action */
        diag_util_mprintf("\tDrop action state: ");
        if (ENABLED == action->green_drop_en)
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);

            diag_util_mprintf("\t\taction: ");
            switch (action->green_drop_data)
            {
                case ACL_ACTION_COLOR_DROP_PERMIT:
                    diag_util_mprintf("permit\n");
                    break;
                case ACL_ACTION_COLOR_DROP_DROP:
                    diag_util_mprintf("drop\n");
                    break;
                default:
                    diag_util_mprintf("\n");
            }
        }
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

        diag_util_mprintf("\tYellow Drop action state: ");
        if (ENABLED == action->yellow_drop_en)
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);

            diag_util_mprintf("\t\taction: ");
            switch (action->yellow_drop_data)
            {
                case ACL_ACTION_COLOR_DROP_PERMIT:
                    diag_util_mprintf("permit\n");
                    break;
                case ACL_ACTION_COLOR_DROP_DROP:
                    diag_util_mprintf("drop\n");
                    break;
                default:
                    diag_util_mprintf("\n");
            }
        }
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

        diag_util_mprintf("\tRed Drop action state: ");
        if (ENABLED == action->red_drop_en)
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);

            diag_util_mprintf("\t\taction: ");
            switch (action->red_drop_data)
            {
                case ACL_ACTION_COLOR_DROP_PERMIT:
                    diag_util_mprintf("permit\n");
                    break;
                case ACL_ACTION_COLOR_DROP_DROP:
                    diag_util_mprintf("drop\n");
                    break;
                default:
                    diag_util_mprintf("\n");
            }
        }
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

        /* log action */
        diag_util_mprintf("\tStatistic action state: ");
        if (ENABLED == action->stat_en)
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);
        }
        else
        {
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);
        }

        /* Meter action */
        diag_util_mprintf("\tMeter action state: ");
        if (ENABLED == action->meter_en)
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);
            diag_util_mprintf("\t\tmeter index:%d\n", action->meter_data.meter_idx);
        }   /* end of if (ENABLED == action->meter_en) */
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

        /* inner vlan translate action */
        diag_util_mprintf("\tInner Vlan translate action state: ");
        if (ENABLED == action->inner_vlan_assign_en)
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);

            diag_util_mprintf("\t\tinner vlan-xlate action: ");
            switch (action->inner_vlan_data.vid_assign_type)
            {
                case ACL_ACTION_VLAN_ASSIGN_NEW_VID:
                    diag_util_mprintf("new vid:%d\n", action->inner_vlan_data.vid_value);
                    break;
                case ACL_ACTION_VLAN_ASSIGN_SHIFT_VID:
                    diag_util_mprintf("shift vid:%d\n", action->inner_vlan_data.vid_value);
                    break;
                case ACL_ACTION_VLAN_ASSIGN_SHIFT_FROM_OUTER_VID:
                    diag_util_mprintf("shift from outer vid:%d\n", action->inner_vlan_data.vid_value);
                    break;
                default:
                    diag_util_mprintf("\n");
            }   /* end of switch (action->inner_vlan_data.vid_assign_type) */

            diag_util_mprintf("\t\tinner tpid action: ");
            if(ENABLED == action->inner_vlan_data.tpid_assign)
                diag_util_mprintf("Enabled\t\ttpid index%d\n", action->inner_vlan_data.tpid_idx);
            else
                diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

        }   /* end of if (ENABLED == action->inner_vlan_assign_en) */
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

        /* inner priority action */
        diag_util_mprintf("\tInner Priority action state: ");
        if (ENABLED == action->inner_pri_en)
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);

            diag_util_mprintf("\t\taction: ");
            switch (action->inner_pri_data.act)
            {
                case ACL_ACTION_INNER_PRI_ASSIGN_NEW_PRI:
                    diag_util_mprintf("new priority:%d\n", action->inner_pri_data.pri);
                    break;
                case ACL_ACTION_INNER_PRI_COPY_FROM_OUTER:
                    diag_util_mprintf("copy from outer tag priority\n");
                    break;
                case ACL_ACTION_INNER_PRI_KEEP:
                    diag_util_mprintf("keep inner tag priority\n");
                    break;
                default:
                    diag_util_mprintf("\n");
            }
        }
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

        /* outer vlan translate action */
        diag_util_mprintf("\tOuter Vlan translate action state: ");
        if (ENABLED == action->outer_vlan_assign_en)
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);

            diag_util_mprintf("\t\touter vlan-xlate action: ");
            switch (action->outer_vlan_data.vid_assign_type)
            {
                case ACL_ACTION_VLAN_ASSIGN_NEW_VID:
                    diag_util_mprintf("new vid:%d\n", action->outer_vlan_data.vid_value);
                    break;
                case ACL_ACTION_VLAN_ASSIGN_SHIFT_VID:
                    diag_util_mprintf("shift vid:%d\n", action->outer_vlan_data.vid_value);
                    break;
                case ACL_ACTION_VLAN_ASSIGN_SHIFT_FROM_INNER_VID:
                    diag_util_mprintf("shift from inner vid:%d\n", action->outer_vlan_data.vid_value);
                    break;
                default:
                    diag_util_mprintf("\n");
            }   /* end of switch (action->outer_vlan_data.vid_assign_type) */
            diag_util_mprintf("\t\touter tpid action: ");
            if(ENABLED == action->outer_vlan_data.tpid_assign)
                diag_util_mprintf("Enabled\t\ttpid index%d\n", action->outer_vlan_data.tpid_idx);
            else
                diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

        }   /* end of if (ENABLED == action->outer_vlan_assign_en) */
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

        /* outer priority action */
        diag_util_mprintf("\tOuter Priority action state: ");
        if (ENABLED == action->outer_pri_en)
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);

            diag_util_mprintf("\t\taction: ");
            switch (action->outer_pri_data.act)
            {
                case ACL_ACTION_OUTER_PRI_ASSIGN_NEW_PRI:
                    diag_util_mprintf("new priority:%d\n", action->outer_pri_data.pri);
                    break;
                case ACL_ACTION_OUTER_PRI_COPY_FROM_INNER:
                    diag_util_mprintf("copy from inner tag priority\n");
                    break;
                case ACL_ACTION_OUTER_PRI_KEEP:
                    diag_util_mprintf("keep outer tag priority\n");
                    break;
                default:
                    diag_util_mprintf("\n");
            }
        }
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

        /* remark action */
        diag_util_mprintf("\tRemark action state: ");
        if (ENABLED == action->rmk_en)
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);
            diag_util_mprintf("\t\tremark ");

            switch (action->rmk_data.rmk_act)
            {
                case ACL_ACTION_REMARK_DSCP:
                    diag_util_mprintf("DSCP %d\n", action->rmk_data.rmk_info);
                    break;
                case ACL_ACTION_REMARK_IP_PRECEDENCE:
                    diag_util_mprintf("IP percedence %d\n", action->rmk_data.rmk_info);
                    break;
                case ACL_ACTION_REMARK_TOS:
                    diag_util_mprintf("TOS %d\n", action->rmk_data.rmk_info);
                    break;
                default:
                    diag_util_mprintf("\n");
            }   /* end of switch (action->rmk_data.rmk_act) */
        }   /* end of if (ENABLED == action->rmk_en) */
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

        diag_util_mprintf("\tYellow Remark action state: ");
        if (ENABLED == action->yellow_rmk_en)
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);
            diag_util_mprintf("\t\tremark ");

            switch (action->yellow_rmk_data.rmk_act)
            {
                case ACL_ACTION_REMARK_DSCP:
                    diag_util_mprintf("DSCP %d\n", action->yellow_rmk_data.rmk_info);
                    break;
                case ACL_ACTION_REMARK_IP_PRECEDENCE:
                    diag_util_mprintf("IP percedence %d\n", action->yellow_rmk_data.rmk_info);
                    break;
                case ACL_ACTION_REMARK_TOS:
                    diag_util_mprintf("TOS %d\n", action->yellow_rmk_data.rmk_info);
                    break;
                default:
                    diag_util_mprintf("\n");
            }   /* end of switch (action->rmk_data.rmk_act) */
        }   /* end of if (ENABLED == action->rmk_en) */
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

        diag_util_mprintf("\tRed Remark action state: ");
        if (ENABLED == action->red_rmk_en)
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);
            diag_util_mprintf("\t\tremark ");

            switch (action->red_rmk_data.rmk_act)
            {
                case ACL_ACTION_REMARK_DSCP:
                    diag_util_mprintf("DSCP %d\n", action->red_rmk_data.rmk_info);
                    break;
                case ACL_ACTION_REMARK_IP_PRECEDENCE:
                    diag_util_mprintf("IP percedence %d\n", action->red_rmk_data.rmk_info);
                    break;
                case ACL_ACTION_REMARK_TOS:
                    diag_util_mprintf("TOS %d\n", action->red_rmk_data.rmk_info);
                    break;
                default:
                    diag_util_mprintf("\n");
            }   /* end of switch (action->rmk_data.rmk_act) */
        }   /* end of if (ENABLED == action->rmk_en) */
        else
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);
    }

    return ;
}   /* end of _diag_acl_eAct_show */
#endif  /* defined(CONFIG_SDK_RTL9310) */

#ifdef CMD_RANGE_CHECK_SET_VID_ENTRY_INDEX_LOW_BOUND_VID
/*
 * range-check set vid entry <UINT:index> low-bound <UINT:vid>
 */
cparser_result_t
cparser_cmd_range_check_set_vid_entry_index_low_bound_vid(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *vid_ptr)
{
    uint32                          unit;
    int32                           ret;
    rtk_acl_rangeCheck_vid_t        vid_range;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&vid_range, 0, sizeof(rtk_acl_rangeCheck_vid_t));

    ret = rtk_acl_rangeCheckVid_get(unit, *index_ptr, &vid_range);
#if defined(CONFIG_SDK_RTL8390)
    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
        if (RT_ERR_OK != ret)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }
#endif
#if defined(CONFIG_SDK_RTL8380)
    if(DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
    {
        if ((RT_ERR_OK != ret) && (RT_ERR_RANGE_CHECK_TYPE != ret))
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }
#endif

    vid_range.vid_lower_bound   = *vid_ptr;
    DIAG_UTIL_ERR_CHK(rtk_acl_rangeCheckVid_set(unit, *index_ptr, &vid_range), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_range_check_set_entry_index_vid_inner_outer_lower_upper */
#endif

#ifdef CMD_RANGE_CHECK_SET_VID_ENTRY_INDEX_UP_BOUND_VID
/*
 * range-check set vid entry <UINT:index> up-bound <UINT:vid>
 */
cparser_result_t
cparser_cmd_range_check_set_vid_entry_index_up_bound_vid(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *vid_ptr)
{
    uint32                          unit;
    int32                           ret;
    rtk_acl_rangeCheck_vid_t        vid_range;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&vid_range, 0, sizeof(rtk_acl_rangeCheck_vid_t));

    ret = rtk_acl_rangeCheckVid_get(unit, *index_ptr, &vid_range);
#if defined(CONFIG_SDK_RTL8390)
    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
        if (RT_ERR_OK != ret)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }
#endif
#if defined(CONFIG_SDK_RTL8380)
    if(DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
    {
        if ((RT_ERR_OK != ret) && (RT_ERR_RANGE_CHECK_TYPE != ret))
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }
#endif

    vid_range.vid_upper_bound   = *vid_ptr;
    DIAG_UTIL_ERR_CHK(rtk_acl_rangeCheckVid_set(unit, *index_ptr, &vid_range), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_range_check_set_entry_index_vid_inner_outer_lower_upper */
#endif

#ifdef CMD_RANGE_CHECK_SET_VID_ENTRY_INDEX_TYPE_INNER_OUTER
/*
 * range-check set vid entry <UINT:index> type ( inner | outer )
 */
cparser_result_t
cparser_cmd_range_check_set_vid_entry_index_type_inner_outer(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32                          unit;
    int32                           ret;
    rtk_acl_rangeCheck_vid_t        vid_range;
    rtk_acl_rangeCheck_vid_type_t   type;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if('i' == TOKEN_CHAR(6, 0))
    {
        type = RNGCHK_VID_TYPE_INNER;
    }
    else
    {
        type = RNGCHK_VID_TYPE_OUTER;
    }

    osal_memset(&vid_range, 0, sizeof(rtk_acl_rangeCheck_vid_t));

    ret = rtk_acl_rangeCheckVid_get(unit, *index_ptr, &vid_range);
#if defined(CONFIG_SDK_RTL8390)
    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
        if (RT_ERR_OK != ret)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }
#endif
#if defined(CONFIG_SDK_RTL8380)
    if(DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
    {
        if ((RT_ERR_OK != ret) && (RT_ERR_RANGE_CHECK_TYPE != ret))
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }
#endif

    vid_range.vid_type          = type;
    DIAG_UTIL_ERR_CHK(rtk_acl_rangeCheckVid_set(unit, *index_ptr, &vid_range), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_range_check_set_entry_index_vid_inner_outer_lower_upper */
#endif


#ifdef CMD_RANGE_CHECK_GET_VID_ENTRY_INDEX
/*
 * range-check get vid entry <UINT:index>
 */
cparser_result_t
cparser_cmd_range_check_get_vid_entry_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    uint32                      unit;
    int32                       ret;
    rtk_acl_rangeCheck_vid_t    vid_range;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&vid_range, 0, sizeof(rtk_acl_rangeCheck_vid_t));

    DIAG_UTIL_ERR_CHK(rtk_acl_rangeCheckVid_get(unit, *index_ptr, &vid_range), ret);

    diag_util_mprintf("VLAN id range check index %d:\n", *index_ptr);

    diag_util_mprintf("\tType: ");
    if (RNGCHK_VID_TYPE_INNER == vid_range.vid_type)
    {
        diag_util_mprintf("Inner\n");
    }
    else
    {
        diag_util_mprintf("Outer\n");
    }

    diag_util_mprintf("\tLower: %d\n", vid_range.vid_lower_bound);
    diag_util_mprintf("\tUpper: %d\n", vid_range.vid_upper_bound);

#if defined(CONFIG_SDK_RTL8390)
    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
        diag_util_mprintf("\tReverse: ");
        if (0 == vid_range.reverse)
        {
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);
        }
        else
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);
        }
    }
#endif

    return CPARSER_OK;
}   /* end of cparser_cmd_range_check_get_entry_index_vid */
#endif

#ifdef CMD_RANGE_CHECK_SET_VID_ENTRY_INDEX_REVERSE_DISABLE_ENABLE
/*
 * range-check set vid entry <UINT:index> reverse ( disable | enable )
 */
cparser_result_t
cparser_cmd_range_check_set_vid_entry_index_reverse_disable_enable(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    uint32                      unit;
    int32                       ret;
    rtk_acl_rangeCheck_vid_t    vid_range;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&vid_range, 0, sizeof(rtk_acl_rangeCheck_vid_t));

    DIAG_UTIL_ERR_CHK(rtk_acl_rangeCheckVid_get(unit, *index_ptr, &vid_range), ret);

    if('e' == TOKEN_CHAR(6, 0))
    {
        vid_range.reverse = 1;
    }
    else
    {
        vid_range.reverse = 0;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_rangeCheckVid_set(unit, *index_ptr, &vid_range), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_range_check_set_entry_index_vid_reverse_state_disable_enable */
#endif

#ifdef CMD_RANGE_CHECK_GET_FIELD_SELECTOR_ENTRY_INDEX
/*
 * range-check get field selector entry <UINT:index>
 */
cparser_result_t
cparser_cmd_range_check_get_field_selector_entry_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    uint32                              unit;
    int32                               ret;
    rtk_acl_rangeCheck_fieldSelector_t  field_range;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&field_range, 0, sizeof(rtk_acl_rangeCheck_fieldSelector_t));

    DIAG_UTIL_ERR_CHK(rtk_acl_rangeCheckFieldSelector_get(unit, *index_ptr, &field_range), ret);

    diag_util_mprintf("Field selector range check index %d:\n", *index_ptr);

    diag_util_mprintf("\tType: ");
    switch (field_range.fieldSelector_type)
    {
        case RNGCHK_FIELDSELECTOR_TYPE_FIELDSELECTOR0:
            diag_util_mprintf("Field selector 0");
            break;
        case RNGCHK_FIELDSELECTOR_TYPE_FIELDSELECTOR1:
            diag_util_mprintf("Field selector 1");
            break;
        default:
            return CPARSER_NOT_OK;
    }

    diag_util_mprintf("\n");

    diag_util_mprintf("\tLower: %x\n", field_range.lower_bound);
    diag_util_mprintf("\tUpper: %x\n", field_range.upper_bound);


    return CPARSER_OK;
}   /* end of cparser_cmd_range_check_get_entry_index_ip */
#endif


#ifdef CMD_RANGE_CHECK_SET_FIELD_SELECTOR_ENTRY_INDEX_LOW_BOUND_FIELDSEL
/*
 * range-check set field-selector entry <UINT:index> low-bound <UINT:field-selector>
 */
cparser_result_t
cparser_cmd_range_check_set_field_selector_entry_index_low_bound_fieldsel(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *field_selector_ptr)
{
    uint32                              unit;
    int32                               ret;
    rtk_acl_rangeCheck_fieldSelector_t  field_range;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&field_range, 0, sizeof(rtk_acl_rangeCheck_fieldSelector_t));

    ret = rtk_acl_rangeCheckFieldSelector_get(unit, *index_ptr, &field_range);
    if ((RT_ERR_OK != ret) && (RT_ERR_RANGE_CHECK_TYPE != ret))
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    field_range.lower_bound = *field_selector_ptr;
    DIAG_UTIL_ERR_CHK(rtk_acl_rangeCheckFieldSelector_set(unit, *index_ptr, &field_range), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_range_check_set_entry_index_l4port_source_dest_lower_upper */
#endif

#ifdef CMD_RANGE_CHECK_SET_FIELD_SELECTOR_ENTRY_INDEX_UP_BOUND_FIELDSEL
/*
 * range-check set field-selector entry <UINT:index> up-bound <UINT:field-selector>
 */
cparser_result_t
cparser_cmd_range_check_set_field_selector_entry_index_up_bound_fieldsel(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *field_selector_ptr)
{
    uint32                              unit;
    int32                               ret;
    rtk_acl_rangeCheck_fieldSelector_t  field_range;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&field_range, 0, sizeof(rtk_acl_rangeCheck_fieldSelector_t));

    ret = rtk_acl_rangeCheckFieldSelector_get(unit, *index_ptr, &field_range);
    if ((RT_ERR_OK != ret) && (RT_ERR_RANGE_CHECK_TYPE != ret))
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    field_range.upper_bound = *field_selector_ptr;
    DIAG_UTIL_ERR_CHK(rtk_acl_rangeCheckFieldSelector_set(unit, *index_ptr, &field_range), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_range_check_set_entry_index_l4port_source_dest_lower_upper */
#endif

#ifdef CMD_RANGE_CHECK_SET_FIELD_SELECTOR_ENTRY_INDEX_TYPE_FIELD_SELECTOR_0_FIELD_SELECTOR_1
/*
 *range-check set field-selector entry <UINT:index> type ( field-selector-0 | field-selector-1 )
 */
cparser_result_t
cparser_cmd_range_check_set_field_selector_entry_index_type_field_selector_0_field_selector_1(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    uint32                              unit;
    int32                               ret;
    rtk_acl_rangeCheck_fieldSelector_t  field_range;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&field_range, 0, sizeof(rtk_acl_rangeCheck_fieldSelector_t));

    ret = rtk_acl_rangeCheckFieldSelector_get(unit, *index_ptr, &field_range);
    if ((RT_ERR_OK != ret) && (RT_ERR_RANGE_CHECK_TYPE != ret))
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    if('0' == TOKEN_CHAR(6, 15))
    {
        field_range.fieldSelector_type = RNGCHK_FIELDSELECTOR_TYPE_FIELDSELECTOR0;
    }
    else
    {
        field_range.fieldSelector_type = RNGCHK_FIELDSELECTOR_TYPE_FIELDSELECTOR1;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_rangeCheckFieldSelector_set(unit, *index_ptr, &field_range), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_range_check_set_entry_index_l4port_source_dest_lower_upper */
#endif


#ifdef CMD_RANGE_CHECK_SET_L4PORT_ENTRY_INDEX_LOW_BOUND_L4PORT
/*
 * range-check set l4port entry <UINT:index> low-bound <UINT:l4port>
 */
cparser_result_t
cparser_cmd_range_check_set_l4port_entry_index_low_bound_l4port(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *l4port_ptr)
{
    uint32                      unit;
    int32                       ret;
    rtk_acl_rangeCheck_l4Port_t l4port_range;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&l4port_range, 0, sizeof(rtk_acl_rangeCheck_l4Port_t));

    ret = rtk_acl_rangeCheckL4Port_get(unit, *index_ptr, &l4port_range);
#if defined(CONFIG_SDK_RTL8390)
    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
        if (RT_ERR_OK != ret)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }
#endif
#if defined(CONFIG_SDK_RTL8380)
    if(DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
    {
        if ((RT_ERR_OK != ret) && (RT_ERR_RANGE_CHECK_TYPE != ret))
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }
#endif

    l4port_range.lower_bound = *l4port_ptr;
    DIAG_UTIL_ERR_CHK(rtk_acl_rangeCheckL4Port_set(unit, *index_ptr, &l4port_range), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_range_check_set_entry_index_l4port_source_dest_lower_upper */
#endif

#ifdef CMD_RANGE_CHECK_SET_L4PORT_ENTRY_INDEX_UP_BOUND_L4PORT
/*
 * range-check set l4port entry <UINT:index> up-bound <UINT:l4port>
 */
cparser_result_t
cparser_cmd_range_check_set_l4port_entry_index_up_bound_l4port(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *l4port_ptr)
{
    uint32                      unit;
    int32                       ret;
    rtk_acl_rangeCheck_l4Port_t l4port_range;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&l4port_range, 0, sizeof(rtk_acl_rangeCheck_l4Port_t));

    ret = rtk_acl_rangeCheckL4Port_get(unit, *index_ptr, &l4port_range);
#if defined(CONFIG_SDK_RTL8390)
    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
        if (RT_ERR_OK != ret)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }
#endif
#if defined(CONFIG_SDK_RTL8380)
    if(DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
    {
        if ((RT_ERR_OK != ret) && (RT_ERR_RANGE_CHECK_TYPE != ret))
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }
#endif

    l4port_range.upper_bound = *l4port_ptr;
    DIAG_UTIL_ERR_CHK(rtk_acl_rangeCheckL4Port_set(unit, *index_ptr, &l4port_range), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_range_check_set_entry_index_l4port_source_dest_lower_upper */
#endif

#ifdef CMD_RANGE_CHECK_SET_L4PORT_ENTRY_INDEX_TYPE_SRC_PORT_DST_PORT
/*
 * range-check set l4port entry <UINT:index> type ( src-port | dst-port )
 */
cparser_result_t
cparser_cmd_range_check_set_l4port_entry_index_type_src_port_dst_port(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    uint32                      unit;
    int32                       ret;
    rtk_acl_rangeCheck_l4Port_t l4port_range;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&l4port_range, 0, sizeof(rtk_acl_rangeCheck_l4Port_t));

    ret = rtk_acl_rangeCheckL4Port_get(unit, *index_ptr, &l4port_range);
#if defined(CONFIG_SDK_RTL8390)
    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
        if (RT_ERR_OK != ret)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }
#endif
#if defined(CONFIG_SDK_RTL8380)
    if(DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
    {
        if ((RT_ERR_OK != ret) && (RT_ERR_RANGE_CHECK_TYPE != ret))
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }
#endif

    if('s' == TOKEN_CHAR(6, 0))
    {
        l4port_range.l4port_dir = RNGCHK_L4PORT_DIRECTION_SRC;
    }
    else
    {
        l4port_range.l4port_dir = RNGCHK_L4PORT_DIRECTION_DST;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_rangeCheckL4Port_set(unit, *index_ptr, &l4port_range), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_range_check_set_entry_index_l4port_source_dest_lower_upper */
#endif

#ifdef CMD_RANGE_CHECK_SET_L4PORT_ENTRY_INDEX_TYPE_SRC_PORT_DST_PORT_SRC_OR_DST_PORT
/*
 * range-check set l4port entry <UINT:index> type ( src-port | dst-port |src-or-dst-port  )
 */
cparser_result_t
cparser_cmd_range_check_set_l4port_entry_index_type_src_port_dst_port_src_or_dst_port(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    uint32                      unit;
    int32                       ret;
    rtk_acl_rangeCheck_l4Port_t l4port_range;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&l4port_range, 0, sizeof(rtk_acl_rangeCheck_l4Port_t));

    ret = rtk_acl_rangeCheckL4Port_get(unit, *index_ptr, &l4port_range);
    if ((RT_ERR_OK != ret) && (RT_ERR_RANGE_CHECK_TYPE != ret))
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    if(('s' == TOKEN_CHAR(6, 0)) && ('o' == TOKEN_CHAR(6, 4)))
    {
        l4port_range.l4port_dir = RNGCHK_L4PORT_DIRECTION_SRC_DST;
    }
    else if('s' == TOKEN_CHAR(6, 0))
    {
        l4port_range.l4port_dir = RNGCHK_L4PORT_DIRECTION_SRC;
    }
    else
    {
        l4port_range.l4port_dir = RNGCHK_L4PORT_DIRECTION_DST;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_rangeCheckL4Port_set(unit, *index_ptr, &l4port_range), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_range_check_set_entry_index_l4port_source_dest_lower_upper */
#endif

#ifdef CMD_RANGE_CHECK_SET_L4PORT_ENTRY_INDEX_REVERSE_DISABLE_ENABLE
/*
 * range-check set l4port entry <UINT:index> reverse ( disable | enable )
 */
cparser_result_t
cparser_cmd_range_check_set_l4port_entry_index_reverse_disable_enable(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    uint32                      unit;
    int32                       ret;
    rtk_acl_rangeCheck_l4Port_t l4port_range;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&l4port_range, 0, sizeof(rtk_acl_rangeCheck_l4Port_t));

    DIAG_UTIL_ERR_CHK(rtk_acl_rangeCheckL4Port_get(unit, *index_ptr, &l4port_range), ret);

    if('e' == TOKEN_CHAR(6, 0))
    {
        l4port_range.reverse = 1;
    }
    else
    {
        l4port_range.reverse = 0;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_rangeCheckL4Port_set(unit, *index_ptr, &l4port_range), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_range_check_set_entry_index_l4port_reverse_state_disable_enable */
#endif

#ifdef CMD_RANGE_CHECK_GET_L4PORT_ENTRY_INDEX
/*
 * range-check get l4port entry <UINT:index>
 */
cparser_result_t
cparser_cmd_range_check_get_l4port_entry_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    uint32                      unit;
    int32                       ret;
    rtk_acl_rangeCheck_l4Port_t l4port_range;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&l4port_range, 0, sizeof(rtk_acl_rangeCheck_l4Port_t));

    DIAG_UTIL_ERR_CHK(rtk_acl_rangeCheckL4Port_get(unit, *index_ptr, &l4port_range), ret);

    diag_util_mprintf("L4 port range check index %d:\n", *index_ptr);

    diag_util_mprintf("\tType: ");
    if (RNGCHK_L4PORT_DIRECTION_SRC == l4port_range.l4port_dir)
    {
        diag_util_mprintf("source direction");
    }
    else if (RNGCHK_L4PORT_DIRECTION_DST == l4port_range.l4port_dir)
    {
        diag_util_mprintf("destination direction");
    }

#if defined(CONFIG_SDK_RTL8380)
    if (DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
    {
        if (RNGCHK_L4PORT_DIRECTION_SRC_DST == l4port_range.l4port_dir)
        {
            diag_util_mprintf("source or destination direction");
        }
    }
#endif

    diag_util_mprintf("\n");

    diag_util_mprintf("\tLower: %d\n", l4port_range.lower_bound);
    diag_util_mprintf("\tUpper: %d\n", l4port_range.upper_bound);

#if defined(CONFIG_SDK_RTL8390)
    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
        diag_util_mprintf("\tReverse: ");
        if (0 == l4port_range.reverse)
        {
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);
        }
        else
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);
        }
    }
#endif

    return CPARSER_OK;
}   /* end of cparser_cmd_range_check_get_l4port_entry_index */
#endif

#ifdef CMD_RANGE_CHECK_SET_LENGTH_ENTRY_INDEX_LOW_BOUND_LENGTH
/*
 * range-check set length entry <UINT:index> low-bound <UINT:length>
 */
cparser_result_t
cparser_cmd_range_check_set_length_entry_index_low_bound_length(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *length_ptr)
{
    uint32                          unit;
    int32                           ret;
    rtk_acl_rangeCheck_packetLen_t  pktlen_range;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&pktlen_range, 0, sizeof(rtk_acl_rangeCheck_packetLen_t));

    ret = rtk_acl_rangeCheckPacketLen_get(unit, *index_ptr, &pktlen_range);
#if defined(CONFIG_SDK_RTL8390)
    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
        if (RT_ERR_OK != ret)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }
#endif
#if defined(CONFIG_SDK_RTL8380)
    if(DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
    {
        if ((RT_ERR_OK != ret) && (RT_ERR_RANGE_CHECK_TYPE != ret))
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }
#endif

    pktlen_range.lower_bound = *length_ptr;
    DIAG_UTIL_ERR_CHK(rtk_acl_rangeCheckPacketLen_set(unit, *index_ptr, &pktlen_range), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_range_check_set_entry_index_pkt_len_lower_upper */
#endif

#ifdef CMD_RANGE_CHECK_SET_LENGTH_ENTRY_INDEX_UP_BOUND_LENGTH
/*
 * range-check set length entry <UINT:index> up-bound <UINT:length>
 */
cparser_result_t
cparser_cmd_range_check_set_length_entry_index_up_bound_length(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *length_ptr)
{
    uint32                          unit;
    int32                           ret;
    rtk_acl_rangeCheck_packetLen_t  pktlen_range;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&pktlen_range, 0, sizeof(rtk_acl_rangeCheck_packetLen_t));

    ret = rtk_acl_rangeCheckPacketLen_get(unit, *index_ptr, &pktlen_range);
#if defined(CONFIG_SDK_RTL8390)
    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
        if (RT_ERR_OK != ret)
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }
#endif
#if defined(CONFIG_SDK_RTL8380)
    if(DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
    {
        if ((RT_ERR_OK != ret) && (RT_ERR_RANGE_CHECK_TYPE != ret))
        {
            DIAG_ERR_PRINT(ret);
            return CPARSER_NOT_OK;
        }
    }
#endif

    pktlen_range.upper_bound = *length_ptr;
    DIAG_UTIL_ERR_CHK(rtk_acl_rangeCheckPacketLen_set(unit, *index_ptr, &pktlen_range), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_range_check_set_entry_index_pkt_len_lower_upper */
#endif

#ifdef CMD_RANGE_CHECK_SET_LENGTH_ENTRY_INDEX_REVERSE_DISABLE_ENABLE
/*
 * range-check set length entry <UINT:index> reverse ( disable | enable )
 */
cparser_result_t
cparser_cmd_range_check_set_length_entry_index_reverse_disable_enable(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    uint32                          unit;
    int32                           ret;
    rtk_acl_rangeCheck_packetLen_t  pktlen_range;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    osal_memset(&pktlen_range, 0, sizeof(rtk_acl_rangeCheck_packetLen_t));

    DIAG_UTIL_ERR_CHK(rtk_acl_rangeCheckPacketLen_get(unit, *index_ptr, &pktlen_range), ret);

    if('e' == TOKEN_CHAR(6, 0))
    {
        pktlen_range.reverse = 1;
    }
    else
    {
        pktlen_range.reverse = 0;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_rangeCheckPacketLen_set(unit, *index_ptr, &pktlen_range), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_range_check_set_entry_index_pkt_len_reverse_state_disable_enable */
#endif

#ifdef CMD_RANGE_CHECK_GET_LENGTH_ENTRY_INDEX
/*
 * range-check get length entry <UINT:index>
 */
cparser_result_t
cparser_cmd_range_check_get_length_entry_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    uint32                          unit;
    int32                           ret;
    rtk_acl_rangeCheck_packetLen_t  pktlen_range;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&pktlen_range, 0, sizeof(rtk_acl_rangeCheck_packetLen_t));

    DIAG_UTIL_ERR_CHK(rtk_acl_rangeCheckPacketLen_get(unit, *index_ptr, &pktlen_range), ret);

    diag_util_mprintf("Packet len range check index %d:\n", *index_ptr);

    diag_util_mprintf("\tLower: %d\n", pktlen_range.lower_bound);
    diag_util_mprintf("\tUpper: %d\n", pktlen_range.upper_bound);

#if defined(CONFIG_SDK_RTL8390)
    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
        diag_util_mprintf("\tReverse: ");
        if (0 == pktlen_range.reverse)
        {
            diag_util_mprintf("%s\n", DIAG_STR_DISABLE);
        }
        else
        {
            diag_util_mprintf("%s\n", DIAG_STR_ENABLE);
        }
    }
#endif

    return CPARSER_OK;
}   /* end of cparser_cmd_range_check_get_entry_index_pkt_len */
#endif

/*
 * acl get entry phase <UINT:phase> entry <UINT:index> action
 */
cparser_result_t
cparser_cmd_acl_get_entry_phase_phase_entry_index_action(
    cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *index_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
    diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
    diag_util_mprintf("entry index %d:\n", *index_ptr);

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action), ret);

    switch (phase)
    {
        #if (defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL8390))
        case ACL_PHASE_IGR_ACL:
            _diag_acl_igrAction_show(unit, &action.igr_acl);
            break;
        #endif  /* (defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL8390)) */
        #if defined(CONFIG_SDK_RTL8390)
        case ACL_PHASE_EGR_ACL:
            _diag_acl_egrAction_show(unit, &action.egr_acl);
            break;
        #endif  /* defined(CONFIG_SDK_RTL8390) */
        #if (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310))
        case ACL_PHASE_VACL:
            _diag_acl_vAct_show(unit, &action.vact);
            break;
        case ACL_PHASE_IACL:
            _diag_acl_iAct_show(unit, &action.iact);
            break;
        #endif  /* (defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)) */
        #if defined(CONFIG_SDK_RTL9310)
        case ACL_PHASE_EACL:
            _diag_acl_eAct_show(unit, &action.eact);
            break;
        #endif  /* defined(CONFIG_SDK_RTL9310) */
        default:
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_get_entry_phase_entry_action */
#endif

#ifdef CMD_ACL_INIT_ENTRY_BUFFER
/*
 * acl init entry buffer
 */
cparser_result_t
cparser_cmd_acl_init_entry_buffer(
    cparser_context_t *context)
{
    rtk_acl_phase_t     phase;
    uint32  unit;
    uint32  entry_size;
    int32   ret;

    DIAG_OM_GET_UNIT_ID(unit);

    phase = _diag_acl_phaseTrans(unit, 0);
    DIAG_UTIL_ERR_CHK(rtk_acl_ruleEntrySize_get(unit, phase, &entry_size), ret);

    if (!diag_acl_entry_buffer)
    {
        if ((diag_acl_entry_buffer = malloc(entry_size)) == NULL)
        {
            diag_util_printf("malloc() fail!\n");
            return CPARSER_NOT_OK;
        }
    }

    osal_memset(diag_acl_entry_buffer, 0, entry_size);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_init_entry_buffer */
#endif

#ifdef CMD_ACL_SET_ENTRY_BUFFER_PHASE_PHASE_ENTRY_INDEX_FIELD_FIELD_NAME_DATA_DATA_MASK_MASK
/*
 * acl set entry buffer phase <UINT:phase> entry <UINT:index> field <STRING:field_name> data <STRING:data> mask <STRING:mask>
 */
cparser_result_t
cparser_cmd_acl_set_entry_buffer_phase_phase_entry_index_field_field_name_data_data_mask_mask(
    cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *index_ptr,
    char **field_name_ptr,
    char **data_ptr,
    char **mask_ptr)
{
    rtk_acl_phase_t     phase;
    uint32  num_element, i;
    uint32  unit = 0;
    uint32  field_size = 0;
    int32   ret;
    uint8   field_data[RTK_MAX_SIZE_OF_ACL_USER_FIELD];
    uint8   field_mask[RTK_MAX_SIZE_OF_ACL_USER_FIELD];

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if (!diag_acl_entry_buffer)
    {
        diag_util_printf("Entry buffer is not init!\n");
        return CPARSER_NOT_OK;
    }

    num_element = (sizeof(diag_acl_field_list)/sizeof(diag_acl_field_t));

    for (i = 0; i < num_element; ++i)
    {
        if (!strcasecmp(*field_name_ptr, diag_acl_field_list[i].user_info))
            break;
    }

    if (num_element <= i)
    {
        diag_util_printf("Invalid field %s!\n", *field_name_ptr);
        return CPARSER_NOT_OK;
    }

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
    ret = rtk_acl_ruleEntryField_check(unit, phase,
            diag_acl_field_list[i].type);
    if (RT_ERR_OK != ret)
    {
        diag_util_printf("Field %s isn't supported in the phase!\n",
                *field_name_ptr);
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleEntryFieldSize_get(unit, diag_acl_field_list[i].type, &field_size), ret);

    DIAG_UTIL_ACL_PARAM_LEN_CHK(field_size, *data_ptr);
    DIAG_UTIL_ACL_PARAM_LEN_CHK(field_size, *mask_ptr);
    osal_memset(field_data, 0x0, RTK_MAX_SIZE_OF_ACL_USER_FIELD);
    osal_memset(field_mask, 0x0, RTK_MAX_SIZE_OF_ACL_USER_FIELD);

    field_size = ((field_size + 7) / 8);

    if (diag_util_str2IntArray (field_data, *data_ptr, field_size) != RT_ERR_OK)
    {
        diag_util_printf("field data error!\n");
        return CPARSER_NOT_OK;
    }

    if (diag_util_str2IntArray (field_mask, *mask_ptr, field_size) != RT_ERR_OK)
    {
        diag_util_printf("field mask error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleEntryField_set(unit, phase, *index_ptr,
            diag_acl_entry_buffer, diag_acl_field_list[i].type, field_data, field_mask), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_buffer_phase_phase_entry_index_field_field_name_data_data_mask_mask */
#endif

#ifdef CMD_ACL_GET_ENTRY_BUFFER
/*
 * acl get entry buffer
 */
cparser_result_t
cparser_cmd_acl_get_entry_buffer(
    cparser_context_t *context)
{
    rtk_acl_phase_t     phase;
    uint32  unit;
    uint32  entry_size, entry_mask_position;
    uint32  *p32;
    int32   ret;
    uint8   *endian_entry;
    uint8   i;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (!diag_acl_entry_buffer)
    {
        diag_util_printf("Entry buffer is not init!\n");
        return CPARSER_NOT_OK;
    }

    phase = _diag_acl_phaseTrans(unit, 0);
    DIAG_UTIL_ERR_CHK(rtk_acl_ruleEntrySize_get(unit, phase, &entry_size), ret);

    if ((endian_entry = malloc(entry_size)) == NULL)
    {
        diag_util_printf("malloc() fail!\n");
        return CPARSER_NOT_OK;
    }

    osal_memcpy(endian_entry, diag_acl_entry_buffer, entry_size);
    p32 = (uint32 *)endian_entry;
    for (i = 0; i < (entry_size / 4); ++i)
    {
        p32[i] = CPU_to_BE32(p32[i]);
    }

    entry_mask_position = (entry_size / 2);
    for (i = 0; i < entry_mask_position; i++)
    {
        if((i % 8) == 0)
        {
            diag_util_mprintf("\n");
        }
        diag_util_printf("byte %2d | data:%02x mask:%02x\n", i,
                *(endian_entry + i),
                *(endian_entry + i + entry_mask_position));
    }
    diag_util_mprintf("\n");

    free(endian_entry);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_get_entry_buffer */
#endif

#ifdef CMD_ACL_GET_ENTRY_BUFFER_PHASE_PHASE_ENTRY_INDEX_FIELD_FIELD_NAME
/*
 * acl get entry buffer phase <UINT:phase> entry <UINT:index> field <STRING:field_name>
 */
cparser_result_t
cparser_cmd_acl_get_entry_buffer_phase_phase_entry_index_field_field_name(
    cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *index_ptr,
    char **field_name_ptr)
{
    rtk_acl_phase_t     phase;
    uint32  num_element;
    uint32  unit = 0;
    uint32  field_size = 0;
    int32   ret;
    uint32  i;
    uint8   field_data[RTK_MAX_SIZE_OF_ACL_USER_FIELD];
    uint8   field_mask[RTK_MAX_SIZE_OF_ACL_USER_FIELD];

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    if (!diag_acl_entry_buffer)
    {
        diag_util_printf("Entry buffer is not init!\n");
        return CPARSER_NOT_OK;
    }

    num_element = (sizeof(diag_acl_field_list)/sizeof(diag_acl_field_t));
    osal_memset(field_data, 0, RTK_MAX_SIZE_OF_ACL_USER_FIELD);
    osal_memset(field_mask, 0, RTK_MAX_SIZE_OF_ACL_USER_FIELD);

    for (i = 0; i < num_element; ++i)
    {
        if (!strcasecmp(*field_name_ptr, diag_acl_field_list[i].user_info))
            break;
    }

    if (num_element <= i)
    {
        diag_util_printf("Invalid field %s!\n", *field_name_ptr);
        return CPARSER_NOT_OK;
    }

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
    ret = rtk_acl_ruleEntryField_check(unit, phase, diag_acl_field_list[i].type);
    if (RT_ERR_OK != ret)
    {
        diag_util_printf("Field %s isn't supported in the phase!\n",
                *field_name_ptr);
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleEntryFieldSize_get(unit,
            diag_acl_field_list[i].type, &field_size), ret);

    if ((field_size % 8) == 0)
    {
        field_size = (field_size / 8);
    }
    else
    {
        field_size = (field_size / 8) + 1;
    }

    ret = rtk_acl_ruleEntryField_get(unit, phase, *index_ptr,
            diag_acl_entry_buffer, diag_acl_field_list[i].type, field_data,
            field_mask);
    if (RT_ERR_OK != ret)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));

    diag_util_mprintf("index %d field %s\n", *index_ptr, *field_name_ptr);

    diag_util_mprintf("\tdata=0x");
    for (i = 0; i < field_size ; ++i)
    {
        diag_util_mprintf("%02x", field_data[i]);
    }
    diag_util_mprintf("\n");

    diag_util_mprintf("\tmask=0x");
    for (i = 0; i < field_size ; ++i)
    {
        diag_util_mprintf("%02x", field_mask[i]);
    }
    diag_util_mprintf("\n");

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_get_entry_buffer_phase_phase_entry_index_field_field_name */
#endif

#ifdef CMD_ACL_WRITE_ENTRY_BUFFER_PHASE_PHASE_ENTRY_INDEX
/*
 * acl write entry buffer phase <UINT:phase> entry <UINT:index>
 */
cparser_result_t
cparser_cmd_acl_write_entry_buffer_phase_phase_entry_index(
    cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *index_ptr)
{
    rtk_acl_phase_t     phase;
    uint32  unit;
    int32   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if (!diag_acl_entry_buffer)
    {
        diag_util_printf("Entry buffer is not init!\n");
        return CPARSER_NOT_OK;
    }

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
    DIAG_UTIL_ERR_CHK(rtk_acl_ruleEntry_write(unit, phase, *index_ptr,
            diag_acl_entry_buffer), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_write_entry_buffer_phase_phase_entry_index */
#endif

#ifdef CMD_ACL_GET_PORT_PORT_IGR_LOOKUP_STATE
/*
 * acl get port <UINT:port> igr lookup state
 */
cparser_result_t
cparser_cmd_acl_get_port_port_igr_lookup_state(
    cparser_context_t *context,
    uint32_t *port_ptr)
{
    rtk_enable_t    enable;
    rtk_acl_phase_t phase;
    uint32          unit;
    int32           ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    phase = _diag_acl_phaseTrans(unit, 0);
    DIAG_UTIL_ERR_CHK(rtk_acl_portPhaseLookupEnable_get(unit, *port_ptr,
            phase, &enable), ret);

    diag_util_mprintf("Port %d ingress stage ACL lookup state is ", *port_ptr);
    if (ENABLED == enable)
        diag_util_mprintf("%s\n", DIAG_STR_ENABLE);
    else
        diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_get_port_port_igr_lookup_state */
#endif

#ifdef CMD_ACL_SET_PORT_PORT_IGR_LOOKUP_STATE_DISABLE_ENABLE
/*
 * acl set port <UINT:port> igr lookup state ( disable | enable )
 */
cparser_result_t
cparser_cmd_acl_set_port_port_igr_lookup_state_disable_enable(
    cparser_context_t *context,
    uint32_t *port_ptr)
{
    rtk_enable_t    enable;
    rtk_acl_phase_t phase;
    uint32          unit;
    int32           ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if('e' == TOKEN_CHAR(7, 0))
    {
        enable = ENABLED;
    }
    else
    {
        enable = DISABLED;
    }

    phase = _diag_acl_phaseTrans(unit, 0);
    DIAG_UTIL_ERR_CHK(rtk_acl_portPhaseLookupEnable_set(unit, *port_ptr,
            phase, enable), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_port_port_igr_lookup_state_disable_enable */
#endif

#ifdef CMD_ACL_GET_PORT_PORT_EGR_LOOKUP_STATE
/*
 * acl get port <UINT:port> egr lookup state
 */
cparser_result_t
cparser_cmd_acl_get_port_port_egr_lookup_state(
    cparser_context_t *context,
    uint32_t *port_ptr)
{
    rtk_enable_t    enable;
    uint32          unit;
    int32           ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_acl_portPhaseLookupEnable_get(unit, *port_ptr,
            ACL_PHASE_EACL, &enable), ret);

    diag_util_mprintf("Port %d egress stage ACL lookup state is ", *port_ptr);
    if (ENABLED == enable)
        diag_util_mprintf("%s\n", DIAG_STR_ENABLE);
    else
        diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_get_port_port_egr_lookup_state */
#endif

#ifdef CMD_ACL_SET_PORT_PORT_EGR_LOOKUP_STATE_DISABLE_ENABLE
/*
 * acl set port <UINT:port> egr lookup state ( disable | enable )
 */
cparser_result_t
cparser_cmd_acl_set_port_port_egr_lookup_state_disable_enable(
    cparser_context_t *context,
    uint32_t *port_ptr)
{
    rtk_enable_t    enable;
    uint32          unit;
    int32           ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if('e' == TOKEN_CHAR(7, 0))
    {
        enable = ENABLED;
    }
    else
    {
        enable = DISABLED;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_portPhaseLookupEnable_set(unit, *port_ptr,
            ACL_PHASE_EACL, enable), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_port_port_egr_lookup_state_disable_enable */
#endif

#ifdef CMD_ACL_GET_PARTITION
/*
 * acl get partition
 */
cparser_result_t cparser_cmd_acl_get_partition(cparser_context_t *context)
{
    uint32  cute_line;
    uint32  unit;
    int32   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_acl_partition_get(unit, &cute_line), ret);

    diag_util_mprintf("The partition is %d\n", cute_line);

    return CPARSER_OK;
}
#endif

#ifdef CMD_ACL_SET_PARTITION_VALUE
/*
 * acl set partition <UINT:value>
 */
cparser_result_t
cparser_cmd_acl_set_partition_value(cparser_context_t *context,
    uint32_t *value_ptr)
{
    uint32  unit;
    int32   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(rtk_acl_partition_set(unit, *value_ptr), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_ACL_SET_METER_MODE_BLOCK_INDEX_BYTE_PACKET
/*
 * acl set meter mode block <UINT:index> ( byte | packet )
 */
cparser_result_t cparser_cmd_acl_set_meter_mode_block_index_byte_packet(cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    uint32          blockIdx;
    rtk_acl_meterMode_t    mode = METER_MODE_BYTE;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    blockIdx = *index_ptr;

    if ('b' == TOKEN_CHAR(6,0))
    {
        mode = METER_MODE_BYTE;
    }
    else if ('p' == TOKEN_CHAR(6,0))
    {
        mode = METER_MODE_PACKET;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_meterMode_set(unit, blockIdx, mode), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_ACL_SET_METER_BURST_SIZE_BYTE_PACKET_SLB_LB0BS_SLB_LB1BS_LB0_BURST_SIZE_LB0_BURST_SIZE_LB1BS_BURST_SIZE_LB1BS_BURST_SIZE
/*
 * acl set meter burst-size ( byte | packet ) ( slb_lb0bs | slb_lb1bs ) lb0-burst-size <UINT:lb0_burst_size> lb1-burst-size <UINT:lb1_burst_size>
 */
cparser_result_t cparser_cmd_acl_set_meter_burst_size_byte_packet_slb_lb0bs_slb_lb1bs_lb0_burst_size_lb0_burst_size_lb1_burst_size_lb1_burst_size(
    cparser_context_t *context,
    uint32_t *lb0_burst_size_ptr,
    uint32_t *lb1_burst_size_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_acl_meterMode_t    mode = METER_MODE_BYTE;
    rtk_acl_meterBurstSize_t  burstSize;
    uint32          lb0bs, lb1bs;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('b' == TOKEN_CHAR(4,0))
    {
        mode = METER_MODE_BYTE;
    }
    else if ('p' == TOKEN_CHAR(4,0))
    {
        mode = METER_MODE_PACKET;
    }

    lb0bs = *lb0_burst_size_ptr;
    lb1bs = *lb1_burst_size_ptr;

    DIAG_UTIL_ERR_CHK(rtk_acl_meterBurstSize_get(unit, mode, &burstSize), ret);

    burstSize.slb_lb0bs = lb0bs;
    burstSize.slb_lb1bs = lb1bs;

    DIAG_UTIL_ERR_CHK(rtk_acl_meterBurstSize_set(unit, mode, &burstSize), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_ACL_SET_METER_BURST_SIZE_BYTE_PACKET_DLB_SRTCM_TRTCM_LB0_BURST_SIZE_LB0_BURST_SIZE_LB1_BURST_SIZE_LB1_BURST_SIZE
/*
 * acl set meter burst-size ( byte | packet ) ( dlb | srtcm | trtcm ) lb0-burst-size <UINT:lb0_burst_size> lb1-burst-size <UINT:lb1_burst_size>
 */
cparser_result_t cparser_cmd_acl_set_meter_burst_size_byte_packet_dlb_srtcm_trtcm_lb0_burst_size_lb0_burst_size_lb1_burst_size_lb1_burst_size(
    cparser_context_t *context,
    uint32_t *lb0_burst_size_ptr,
    uint32_t *lb1_burst_size_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    rtk_acl_meterMode_t    mode = METER_MODE_BYTE;
    rtk_pie_meterType_t    type = METER_TYPE_DLB;
    rtk_acl_meterBurstSize_t  burstSize;
    uint32          lb0bs, lb1bs;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('b' == TOKEN_CHAR(4,0))
    {
        mode = METER_MODE_BYTE;
    }
    else if ('p' == TOKEN_CHAR(4,0))
    {
        mode = METER_MODE_PACKET;
    }

    if ('d' == TOKEN_CHAR(5,0))
    {
        type = METER_TYPE_DLB;
    }
    else if ('s' == TOKEN_CHAR(5,0))
    {
        type = METER_TYPE_SRTCM;
    }
    else if ('t' == TOKEN_CHAR(5,0))
    {
        type = METER_TYPE_TRTCM;
    }

    lb0bs = *lb0_burst_size_ptr;
    lb1bs = *lb1_burst_size_ptr;

    DIAG_UTIL_ERR_CHK(rtk_acl_meterBurstSize_get(unit, mode, &burstSize), ret);
    switch(type)
    {
        case METER_TYPE_DLB:
            burstSize.dlb_lb0bs = lb0bs;
            burstSize.dlb_lb1bs = lb1bs;
            break;
        case METER_TYPE_SRTCM:
            burstSize.srtcm_cbs = lb0bs;
            burstSize.srtcm_ebs = lb1bs;
            break;
        case METER_TYPE_TRTCM:
            burstSize.trtcm_cbs = lb0bs;
            burstSize.trtcm_pbs = lb1bs;
            break;
        default:
            break;
    }
    DIAG_UTIL_ERR_CHK(rtk_acl_meterBurstSize_set(unit, mode, &burstSize), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_ACL_SET_METER_ENTRY_INDEX_DLB_LB0_RATE_LB0_RATE_LB1_RATE_LB1_RATE
/*
 * acl set meter entry <UINT:index> dlb lb0-rate <UINT:lb0_rate> lb1-rate <UINT:lb1_rate>
 */
cparser_result_t cparser_cmd_acl_set_meter_entry_index_dlb_lb0_rate_lb0_rate_lb1_rate_lb1_rate(
    cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *lb0_rate_ptr,
    uint32_t *lb1_rate_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    uint32          meterId;
    rtk_pie_meterEntry_t   meterEntry;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    meterId = *index_ptr;

    osal_memset(&meterEntry, 0, sizeof(meterEntry));

    meterEntry.type = METER_TYPE_DLB;
    meterEntry.color_aware = FALSE;
    meterEntry.lb0_rate = *lb0_rate_ptr;
    meterEntry.lb1_rate = *lb1_rate_ptr;

    DIAG_UTIL_ERR_CHK(rtk_pie_meterEntry_set(unit, meterId, &meterEntry), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_ACL_SET_METER_ENTRY_INDEX_SRTCM_COLOR_AWARE_COLOR_UNAWARE_CIR_CIR
/*
 * acl set meter entry <UINT:index> srtcm ( color-aware | color-unaware ) cir <UINT:cir>
 */
cparser_result_t cparser_cmd_acl_set_meter_entry_index_srtcm_color_aware_color_unaware_cir_cir(
    cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *cir_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    uint32          meterId;
    rtk_pie_meterEntry_t   meterEntry;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    meterId = *index_ptr;

    osal_memset(&meterEntry, 0, sizeof(meterEntry));

    if ('a' == TOKEN_CHAR(6,6))
    {
        meterEntry.color_aware = TRUE;
    }
    else if ('u' == TOKEN_CHAR(6,6))
    {
        meterEntry.color_aware = FALSE;
    }

    meterEntry.type = METER_TYPE_SRTCM;
    meterEntry.lb0_rate = *cir_ptr;
    meterEntry.lb1_rate = 0;

    DIAG_UTIL_ERR_CHK(rtk_pie_meterEntry_set(unit, meterId, &meterEntry), ret);
    return CPARSER_OK;
}
#endif

#ifdef CMD_ACL_SET_METER_ENTRY_INDEX_TRTCM_COLOR_AWARE_COLOR_UNAWARE_CIR_CIR_PIR_PIR
/*
 * acl set meter entry <UINT:index> trtcm ( color-aware | color-unaware ) cir <UINT:cir> pir <UINT:pir>
 */
cparser_result_t cparser_cmd_acl_set_meter_entry_index_trtcm_color_aware_color_unaware_cir_cir_pir_pir(cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *cir_ptr,
    uint32_t *pir_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    uint32          meterId;
    rtk_pie_meterEntry_t   meterEntry;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    meterId = *index_ptr;

    osal_memset(&meterEntry, 0, sizeof(meterEntry));

    if ('a' == TOKEN_CHAR(6,6))
    {
        meterEntry.color_aware = TRUE;
    }
    else if ('u' == TOKEN_CHAR(6,6))
    {
        meterEntry.color_aware = FALSE;
    }

    meterEntry.type = METER_TYPE_TRTCM;
    meterEntry.lb0_rate = *cir_ptr;
    meterEntry.lb1_rate = *pir_ptr;

    DIAG_UTIL_ERR_CHK(rtk_pie_meterEntry_set(unit, meterId, &meterEntry), ret);
    return CPARSER_OK;
}
#endif

#ifdef CMD_ACL_SET_METER_ENTRY_INDEX_SLB_STATE_DISABLE_ENABLE_RATE_RATE_GROUP_GROUP
/*
 * acl set meter entry <UINT:index> slb state ( disable | enable ) rate <UINT:rate> group <UINT:group>
 */
cparser_result_t cparser_cmd_acl_set_meter_entry_index_slb_state_disable_enable_rate_rate_group_group(
    cparser_context_t *context,
    uint32_t *index_ptr,
    uint32_t *rate_ptr,
    uint32_t *threshold_group_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    uint32          meterId;
    rtk_acl_meterEntry_t    meterEntry;
    rtk_enable_t            enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if('e' == TOKEN_CHAR(7, 0))
    {
        enable = ENABLED;
    }
    else
    {
        enable = DISABLED;
    }

    meterId = *index_ptr;

    osal_memset(&meterEntry, 0, sizeof(meterEntry));

    meterEntry.enable = enable;
    meterEntry.rate = *rate_ptr;
    meterEntry.thr_grp = *threshold_group_ptr;

    DIAG_UTIL_ERR_CHK(rtk_pie_meterEntry_set(unit, meterId, &meterEntry), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_ACL_GET_METER_MODE_INDEX
/*
 * acl get meter mode <UINT:index>
 */
cparser_result_t cparser_cmd_acl_get_meter_mode_index(cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32          unit = 0;
    int32           ret = RT_ERR_FAILED;
    uint32          blockIdx;
    rtk_acl_meterMode_t    mode = METER_MODE_BYTE;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    blockIdx = *index_ptr;

    DIAG_UTIL_ERR_CHK(rtk_acl_meterMode_get(unit, blockIdx, &mode), ret);

    diag_util_printf("Block %u meter mode: ", blockIdx);

    if(METER_MODE_BYTE == mode)
        diag_util_mprintf("Byte Based\n");
    else
        diag_util_mprintf("Packet Based\n");

    return CPARSER_OK;
}
#endif

#ifdef CMD_ACL_GET_METER_BURST_SIZE
/*
 * acl get meter burst-size
 */
cparser_result_t cparser_cmd_acl_get_meter_burst_size(cparser_context_t *context)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_acl_meterBurstSize_t  burstSize;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_acl_meterBurstSize_get(unit, METER_MODE_BYTE, &burstSize), ret);
#if defined(CONFIG_SDK_RTL8390)
    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
        diag_util_mprintf("Meter Byte Based Mode\n");
        diag_util_mprintf(" Dual Leaky Bucket\n");
        diag_util_mprintf("     Leaky Bucket 0 Burst Size:%d\n", burstSize.dlb_lb0bs);
        diag_util_mprintf("     Leaky Bucket 1 Burst Size:%d\n", burstSize.dlb_lb1bs);
        diag_util_mprintf(" Single Rate Three Color Marker\n");
        diag_util_mprintf("     Committed Burst Size:%d\n", burstSize.srtcm_cbs);
        diag_util_mprintf("     Excess Burst Size:%d\n", burstSize.srtcm_ebs);
        diag_util_mprintf(" Two Rate Three Color Marker\n");
        diag_util_mprintf("     Committed Burst Size:%d\n", burstSize.trtcm_cbs);
        diag_util_mprintf("     Peak Burst Size:%d\n", burstSize.trtcm_pbs);
    }
#endif

#if defined(CONFIG_SDK_RTL8380)
    if(DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
    {
        diag_util_mprintf("Meter Byte Based Mode\n");
        diag_util_mprintf(" Single Leaky Bucket\n");
        diag_util_mprintf("     Leaky Bucket 0 Burst Size:%d\n", burstSize.slb_lb0bs);
        diag_util_mprintf("     Leaky Bucket 1 Burst Size:%d\n", burstSize.slb_lb1bs);
    }
#endif

    DIAG_UTIL_ERR_CHK(rtk_acl_meterBurstSize_get(unit, METER_MODE_PACKET, &burstSize), ret);
#if defined(CONFIG_SDK_RTL8390)
    if (DIAG_OM_GET_FAMILYID(RTL8390_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8350_FAMILY_ID))
    {
        diag_util_mprintf("Meter Packet Based Mode\n");
        diag_util_mprintf(" Dual Leaky Bucket\n");
        diag_util_mprintf("     Leaky Bucket 0 Burst Size:%d\n", burstSize.dlb_lb0bs);
        diag_util_mprintf("     Leaky Bucket 1 Burst Size:%d\n", burstSize.dlb_lb1bs);
        diag_util_mprintf(" Single Rate Three Color Marker\n");
        diag_util_mprintf("     Committed Burst Size:%d\n", burstSize.srtcm_cbs);
        diag_util_mprintf("     Excess Burst Size:%d\n", burstSize.srtcm_ebs);
        diag_util_mprintf(" Two Rate Three Color Marker\n");
        diag_util_mprintf("     Committed Burst Size:%d\n", burstSize.trtcm_cbs);
        diag_util_mprintf("     Peak Burst Size:%d\n", burstSize.trtcm_pbs);
    }
#endif

#if defined(CONFIG_SDK_RTL8380)
    if(DIAG_OM_GET_FAMILYID(RTL8380_FAMILY_ID) || DIAG_OM_GET_FAMILYID(RTL8330_FAMILY_ID))
    {
        diag_util_mprintf("Meter Packet Based Mode\n");
        diag_util_mprintf(" Single Leaky Bucket\n");
        diag_util_mprintf("     Leaky Bucket 0 Burst Size:%d\n", burstSize.slb_lb0bs);
        diag_util_mprintf("     Leaky Bucket 1 Burst Size:%d\n", burstSize.slb_lb1bs);
    }
#endif

    return CPARSER_OK;
}
#endif

#ifdef CMD_RANGE_CHECK_GET_PORT_ENTRY_INDEX_SOURCE
/*
 * range-check get port entry <UINT:index> source
 */
cparser_result_t
cparser_cmd_range_check_get_port_entry_index_source(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32                          unit;
    int32                           ret;
    char                            port_list[DIAG_UTIL_PORT_MASK_STRING_LEN];
    rtk_acl_rangeCheck_portMask_t   port_range;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&port_range, 0, sizeof(rtk_acl_rangeCheck_portMask_t));

    DIAG_UTIL_ERR_CHK(rtk_acl_rangeCheckSrcPort_get(unit, *index_ptr, &port_range), ret);

    diag_util_mprintf("Source port range check index %d:\n", *index_ptr);
    diag_util_lPortMask2str(port_list, &port_range.port_mask);
    diag_util_mprintf("\tconfigured ports: %s\n", port_list);

    return CPARSER_OK;
}   /* end of cparser_cmd_range_check_get_entry_index_port_check_source */
#endif

#ifdef CMD_RANGE_CHECK_SET_PORT_ENTRY_INDEX_PORT_PORTS_NONE_SOURCE
/*
 * range-check set port entry <UINT:index> port ( <PORT_LIST:ports> | none ) source
 */
cparser_result_t
cparser_cmd_range_check_set_port_entry_index_port_ports_none_source(
    cparser_context_t *context,
    uint32_t *index_ptr,
    char **ports_ptr)
{
    uint32                          unit;
    int32                           ret;
    rtk_acl_rangeCheck_portMask_t   port_range;
    rtk_portmask_t                  port_mask;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&port_range, 0, sizeof(rtk_acl_rangeCheck_portMask_t));
    osal_memset(&port_mask, 0, sizeof(rtk_portmask_t));

    DIAG_UTIL_ERR_CHK(rtk_acl_rangeCheckSrcPort_get(unit, *index_ptr, &port_range), ret);

    if('n' != TOKEN_CHAR(6,0))
    {
        osal_memset(&port_mask, 0, sizeof(rtk_portmask_t));
        DIAG_UTIL_ERR_CHK(diag_util_str2LPortMask(*ports_ptr, &port_mask), ret);
    }

    osal_memcpy(&port_range.port_mask, &port_mask, sizeof(rtk_portmask_t));

    DIAG_UTIL_ERR_CHK(rtk_acl_rangeCheckSrcPort_set(unit, *index_ptr, &port_range), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_range_check_set_entry_index_port_check_source_port_none */
#endif

#ifdef CMD_RANGE_CHECK_GET_PORT_ENTRY_INDEX_DESTINATION
/*
 * range-check get port entry <UINT:index> destination
 */
cparser_result_t
cparser_cmd_range_check_get_port_entry_index_destination(
    cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32                          unit;
    int32                           ret;
    char                            port_list[DIAG_UTIL_PORT_MASK_STRING_LEN];
    rtk_acl_rangeCheck_portMask_t   port_range;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&port_range, 0, sizeof(rtk_acl_rangeCheck_portMask_t));

    DIAG_UTIL_ERR_CHK(rtk_acl_rangeCheckDstPort_get(unit, *index_ptr, &port_range), ret);

    diag_util_mprintf("Destination port range check index %d:\n", *index_ptr);
    diag_util_lPortMask2str(port_list, &port_range.port_mask);
    diag_util_mprintf("\tconfigured ports: %s\n", port_list);

    return CPARSER_OK;
}   /* end of cparser_cmd_range_check_get_entry_index_port_check_destination */
#endif

#ifdef CMD_RANGE_CHECK_SET_PORT_ENTRY_INDEX_PORT_PORTS_NONE_DESTINATION
/*
 * range-check set port entry <UINT:index> port ( <PORT_LIST:ports> | none ) destination
 */
cparser_result_t
cparser_cmd_range_check_set_port_entry_index_port_ports_none_destination(
    cparser_context_t *context,
    uint32_t *index_ptr,
    char **ports_ptr)
{
    uint32                          unit;
    int32                           ret;
    rtk_acl_rangeCheck_portMask_t   port_range;
    rtk_portmask_t                  port_mask;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&port_range, 0, sizeof(rtk_acl_rangeCheck_portMask_t));
    osal_memset(&port_mask, 0, sizeof(rtk_portmask_t));

    DIAG_UTIL_ERR_CHK(rtk_acl_rangeCheckDstPort_get(unit, *index_ptr, &port_range), ret);

    if('n' != TOKEN_CHAR(6,0))
    {
        osal_memset(&port_mask, 0, sizeof(rtk_portmask_t));
        DIAG_UTIL_ERR_CHK(diag_util_str2LPortMask(*ports_ptr, &port_mask), ret);
    }

    osal_memcpy(&port_range.port_mask, &port_mask, sizeof(rtk_portmask_t));

    DIAG_UTIL_ERR_CHK(rtk_acl_rangeCheckDstPort_set(unit, *index_ptr, &port_range), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_range_check_set_entry_index_port_check_destination_port_none */
#endif

#ifdef CMD_ACL_GET_TEMPLATE_FIELD_INTENT_VLAN_TAG
/*
 * acl get template field-intent vlan-tag
 */
cparser_result_t
cparser_cmd_acl_get_template_field_intent_vlan_tag(
    cparser_context_t *context)
{
    rtk_vlan_tagType_t  tagType;
    uint32              unit;
    int32               ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_ERR_CHK(rtk_acl_templateFieldIntentVlanTag_get(unit, &tagType), ret);

    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("Template field intent VLAN tag: ");
    if (VLAN_TAG_TYPE_INNER == tagType)
        diag_util_mprintf("inner\n");
    else
        diag_util_mprintf("outer\n");

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_get_template_field_intent_vlan_tag */
#endif

#ifdef CMD_ACL_SET_TEMPLATE_FIELD_INTENT_VLAN_TAG_INNER_OUTER
/*
 * acl set template field-intent vlan-tag ( inner | outer )
 */
cparser_result_t
cparser_cmd_acl_set_template_field_intent_vlan_tag_inner_outer(
    cparser_context_t *context)
{
    rtk_vlan_tagType_t  tagType;
    uint32              unit;
    int32               ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('i' == TOKEN_CHAR(5, 0))
        tagType = VLAN_TAG_TYPE_INNER;
    else
        tagType = VLAN_TAG_TYPE_OUTER;

    DIAG_UTIL_ERR_CHK(rtk_acl_templateFieldIntentVlanTag_set(unit, tagType), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_template_field_intent_vlan_tag_inner_outer */
#endif

#ifdef CMD_ACL_GET_BLOCK_INDEX_POWER_STATE
/*
 * acl get block <UINT:index> power state
 */
cparser_result_t
cparser_cmd_acl_get_block_index_power_state(cparser_context_t *context,
    uint32_t *index_ptr)
{
    uint32          unit;
    int32           ret;
    rtk_enable_t    enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_acl_blockPwrEnable_get(unit, *index_ptr, &enable), ret);

    diag_util_mprintf("The block %d power state is ", *index_ptr);
    if (ENABLED == enable)
        diag_util_mprintf("%s\n", DIAG_STR_ENABLE);
    else
        diag_util_mprintf("%s\n", DIAG_STR_DISABLE);

    return CPARSER_OK;
}
#endif

#ifdef CMD_ACL_SET_BLOCK_INDEX_POWER_STATE_DISABLE_ENABLE
/*
 * acl set block <UINT:index> power state ( disable | enable )
 */
cparser_result_t
cparser_cmd_acl_set_block_index_power_state_disable_enable(
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

    DIAG_UTIL_ERR_CHK(rtk_acl_blockPwrEnable_set(unit, *index_ptr, enable), ret);

    return CPARSER_OK;
}
#endif

#ifdef CMD_ACL_GET_PHASE_PHASE_ENTRY_INDEX_COUNTER_BYTE_PACKET_ALL
/*
 * acl get phase <UINT:phase> entry <UINT:index> counter ( byte | packet | all )
 */
cparser_result_t
cparser_cmd_acl_get_phase_phase_entry_index_counter_byte_packet_all(
    cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *index_ptr)
{
    uint64  cnt;
    rtk_acl_phase_t     phase;
    uint32  unit;
    int32   ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("Counter index %d: \n", *index_ptr);

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
    if ('p' != TOKEN_CHAR(7, 0))
    {
        DIAG_UTIL_ERR_CHK(rtk_acl_statCnt_get(unit, phase, *index_ptr,
                STAT_MODE_BYTE, &cnt), ret);

        diag_util_mprintf("Byte: %llu\n", cnt);
    }

    if ('b' != TOKEN_CHAR(7, 0))
    {
        DIAG_UTIL_ERR_CHK(rtk_acl_statCnt_get(unit, phase, *index_ptr,
                STAT_MODE_PACKET, &cnt), ret);

        diag_util_mprintf("Packet: %llu\n", cnt);
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_get_phase_phase_entry_index_counter_byte_packet_all */
#endif

#ifdef CMD_ACL_CLEAR_PHASE_PHASE_ENTRY_INDEX_COUNTER_BYTE_PACKET_ALL
/*
 * acl clear phase <UINT:phase> entry <UINT:index> counter ( byte | packet | all )
 */
cparser_result_t
cparser_cmd_acl_clear_phase_phase_entry_index_counter_byte_packet_all(
    cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *index_ptr)
{
    rtk_acl_statMode_t  mode;
    rtk_acl_phase_t     phase;
    uint32              unit;
    int32               ret;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    if ('b' == TOKEN_CHAR(7, 0))
        mode = STAT_MODE_BYTE;
    else if ('p' == TOKEN_CHAR(7, 0))
        mode = STAT_MODE_PACKET;
    else
        mode = STAT_MODE_ALL;

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
    DIAG_UTIL_ERR_CHK(rtk_acl_statCnt_clear(unit, phase, *index_ptr, mode), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_clear_phase_phase_entry_index_counter_byte_packet_all */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_ROUTING_UNICAST_ECMP_ECMP_INDEX
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action routing unicast ecmp <UINT:ecmp_index>
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_routing_unicast_ecmp_ecmp_index(
    cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *index_ptr,
    uint32_t *ecmp_index_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
    if (ACL_PHASE_VACL != phase)
    {
        diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
        diag_util_printf(" doesn't support the action\n");
        return CPARSER_NOT_OK;
    }

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action), ret);

    osal_memset(&action.vact.fwd_data, 0, sizeof(rtk_acl_vActFwd_t));
    action.vact.fwd_data.fwd_type = ACL_ACTION_FWD_UNICAST_ROUTING;
    action.vact.fwd_data.flags |= RTK_ACL_FWD_FLAG_ECMP;
    action.vact.fwd_data.fwd_info = *ecmp_index_ptr;

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_phase_entry_index_action_routing_unicast_ecmp_ecmp_index */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_DEFAULT_ROUTING_UNICAST_ECMP_ECMP_INDEX
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action default routing unicast ecmp <UINT:ecmp_index>
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_default_routing_unicast_ecmp_ecmp_index(
    cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *index_ptr,
    uint32_t *ecmp_index_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
    if (ACL_PHASE_VACL != phase)
    {
        diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
        diag_util_printf(" doesn't support the action\n");
        return CPARSER_NOT_OK;
    }

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action), ret);

    osal_memset(&action.vact.fwd_data, 0, sizeof(rtk_acl_vActFwd_t));
    action.vact.fwd_data.fwd_type = ACL_ACTION_FWD_DFLT_UNICAST_ROUTING;
    action.vact.fwd_data.flags |= RTK_ACL_FWD_FLAG_ECMP;
    action.vact.fwd_data.fwd_info = *ecmp_index_ptr;

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_phase_entry_index_action_default_routing_unicast_ecmp_ecmp_index */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_ROUTING_UNICAST_NULL_INTF_DROP_TRAP_LOCAL_CPU_TRAP_MASTER_CPU
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action routing unicast null-intf ( drop | trap-local-cpu | trap-master-cpu )
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_routing_unicast_null_intf_drop_trap_local_cpu_trap_master_cpu(
    cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *index_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
    if (ACL_PHASE_VACL != phase)
    {
        diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
        diag_util_printf(" doesn't support the action\n");
        return CPARSER_NOT_OK;
    }

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action), ret);

    osal_memset(&action.vact.fwd_data, 0, sizeof(rtk_acl_vActFwd_t));
    action.vact.fwd_data.fwd_type = ACL_ACTION_FWD_UNICAST_ROUTING;

    if('d' == TOKEN_CHAR(11, 0))
    {
        action.vact.fwd_data.flags |= RTK_ACL_FWD_FLAG_NULL_INTF_DROP;
    }
    else if('l' == TOKEN_CHAR(11, 5))
    {
        action.vact.fwd_data.flags |= RTK_ACL_FWD_FLAG_NULL_INTF_TRAP_LOCAL_CPU;
    }
    else
    {
        action.vact.fwd_data.flags |= RTK_ACL_FWD_FLAG_NULL_INTF_TRAP_MASTER_CPU;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_phase_entry_index_action_routing_unicast_null_intf_drop_trap_local_cpu_trap_master_cpu */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_DEFAULT_ROUTING_UNICAST_NULL_INTF_DROP_TRAP_LOCAL_CPU_TRAP_MASTER_CPU
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action default routing unicast null-intf ( drop | trap-local-cpu | trap-master-cpu )
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_default_routing_unicast_null_intf_drop_trap_local_cpu_trap_master_cpu(
    cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *index_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
    if (ACL_PHASE_VACL != phase)
    {
        diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
        diag_util_printf(" doesn't support the action\n");
        return CPARSER_NOT_OK;
    }

    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action), ret);

    osal_memset(&action.vact.fwd_data, 0, sizeof(rtk_acl_vActFwd_t));
    action.vact.fwd_data.fwd_type = ACL_ACTION_FWD_DFLT_UNICAST_ROUTING;

    if('d' == TOKEN_CHAR(12, 0))
    {
        action.vact.fwd_data.flags |= RTK_ACL_FWD_FLAG_NULL_INTF_DROP;
    }
    else if('l' == TOKEN_CHAR(12, 5))
    {
        action.vact.fwd_data.flags |= RTK_ACL_FWD_FLAG_NULL_INTF_TRAP_LOCAL_CPU;
    }
    else
    {
        action.vact.fwd_data.flags |= RTK_ACL_FWD_FLAG_NULL_INTF_TRAP_MASTER_CPU;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_phase_entry_index_action_default_routing_unicast_null_intf_drop_trap_local_cpu_trap_master_cpu */
#endif


#ifdef CMD_ACL_DUMP_ENTRY_PHASE_PHASE_ENTRY_INDEX_INDEX_END
extern char *diag_pie_templateAbbreviateName_get(rtk_pie_templateFieldType_t templFieldType);
extern int32 diag_pie_templateNames_get(rtk_pie_templateFieldType_t templFieldType, char **abbre, char **user_info);

/* diag acl dump entry information */
typedef struct diag_acl_dumpEntInfo_s {
    uint32              disp_hdr;       /* display header line */
    uint32              entry_size;
    uint8               *pEntry_buffer;
} diag_acl_dumpEntInfo_t;

/*
 * display one rule entry
 */
int32
_cparser_cmd_acl_dump_entry_phase_entry(uint32 unit, rtk_acl_phase_t phase, uint32 entry_idx,
                                        diag_acl_dumpEntInfo_t *pAclDumpEntInfo)
{
    int32   ret = RT_ERR_FAILED;
    uint32  i, width = 6;
    uint32  entry_size = pAclDumpEntInfo->entry_size, entry_mask_position;
    uint32  template_id;
    uint8  *pEntry_buffer = pAclDumpEntInfo->pEntry_buffer;
    char    *str, strBuf[16];
    rtk_pie_template_t  template_info;
    rtk_switch_devInfo_t devInfo;
    uint32  *p32;

    osal_memset(&devInfo, 0, sizeof(rtk_switch_devInfo_t));
    if((ret = diag_om_get_deviceInfo(unit, &devInfo)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    if ((ret = rtk_acl_ruleEntry_read(unit, phase, entry_idx, pEntry_buffer)) != RT_ERR_OK)
    {
        diag_util_printf("rtk_acl_ruleEntry_read() fail!\n");
        DIAG_ERR_PRINT(ret);
        goto EXIT;
    }

    if ((ret = rtk_acl_templateId_get(unit, phase, entry_idx, &template_id)) != RT_ERR_OK)
    {
        diag_util_printf("rtk_acl_templateId_get() fail!\n");
        DIAG_ERR_PRINT(ret);
        goto EXIT;
    }

    osal_memset(&template_info, 0, sizeof(template_info));
    if ((ret = rtk_pie_template_get(unit, template_id, &template_info)) != RT_ERR_OK)
    {
        diag_util_printf("rtk_pie_template_get() fail!\n");
        DIAG_ERR_PRINT(ret);
        goto EXIT;
    }

    if (pAclDumpEntInfo->disp_hdr == FALSE)
    {
        diag_util_printf("phase: %s\n", _diag_acl_phaseStr_get(phase));

        for (i = 0; i < devInfo.capacityInfo.max_num_of_pie_template_field; i++)
        {
            diag_util_printf("f%-*d", (width-1), (devInfo.capacityInfo.max_num_of_pie_template_field-i-1));
        }
        diag_util_mprintf("\n");
        pAclDumpEntInfo->disp_hdr = TRUE;
    }

    diag_util_printf("entry %u, template %u\n", entry_idx, template_id);
    for (i = 0; i < devInfo.capacityInfo.max_num_of_pie_template_field; i++)
    {
        str = diag_pie_templateAbbreviateName_get(template_info.field[(devInfo.capacityInfo.max_num_of_pie_template_field-i-1)]);
        if (strlen(str) >= width)
        {
            strBuf[0] = '!';
            osal_memset(&strBuf[1], 0, sizeof(strBuf)-1);
            osal_strncpy(&strBuf[1], str, (width-2));
            str = strBuf;
        }
        diag_util_printf("%-*s", width, ((str != NULL) ? str : "NULL"));
    }
    diag_util_printf("%-*s", width, "fix");
    diag_util_mprintf("\n");

    /*
     * dump data
     */
    p32 = (uint32 *)pEntry_buffer;
    for (i = 0; i < (entry_size / 4); ++i)
    {
        p32[i] = CPU_to_BE32(p32[i]);
    }

    entry_mask_position = (entry_size / 2);
    for (i = 0; i < entry_mask_position; i++)
    {
        if ((i != 0) && ((i % 2) == 0))
        {
            diag_util_printf("%*s", (width-4), "");
        }
        diag_util_printf("%02x", *(pEntry_buffer + i));
    }
    diag_util_printf("\n");

    /*
     * dump mask
     */
    for (i = 0; i < entry_mask_position; i++)
    {
        if ((i != 0) && ((i % 2) == 0))
        {
            diag_util_printf("%*s", (width-4), "");
        }
        diag_util_printf("%02x", *(pEntry_buffer + i + entry_mask_position));
    }
    diag_util_mprintf("\n");

  EXIT:

    return CPARSER_OK;
}

/*
 * display template field's abbreviation and full name mapping
 */
void
_cparser_cmd_acl_dump_entry_phase_entry_template_name_mapping_display(void)
{
    char    *abbreName, *fullName;
    int     i;
    int     cnt;

    /* display template fileld name's full name */
    cnt = 0;
    for (i = 0; i < TMPLTE_FIELD_END; i++)
    {
        if (diag_pie_templateNames_get(i, &abbreName, &fullName) != RT_ERR_OK)
            continue;

        if (!strcmp(abbreName, fullName))
            continue;

        if ((cnt != 0) && ((cnt % 3) == 0))
        {
            diag_util_printf("\n");
        }

        diag_util_printf("%s(%s); ", abbreName, fullName);
        cnt++;
    }
    diag_util_printf("\n");
}
#endif/* #ifdef CMD_ACL_DUMP_ENTRY_PHASE_PHASE_ENTRY_INDEX_INDEX_END */

#ifdef CMD_ACL_DUMP_ENTRY_PHASE_PHASE_ENTRY_INDEX_INDEX_END
/*
 * acl dump entry phase <UINT:phase> entry <UINT:index> { <UINT:index_end> }
 */
cparser_result_t
cparser_cmd_acl_dump_entry_phase_phase_entry_index_index_end(cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *index_ptr,
    uint32_t *index_end_ptr)
{
    int32   ret;
    uint32  unit = 0, i;
    uint32  entry_idx_end;
    rtk_acl_phase_t     phase;
    diag_acl_dumpEntInfo_t   aclDumpEntInfo;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_UNIT_ID(unit);
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&aclDumpEntInfo, 0, sizeof(diag_acl_dumpEntInfo_t));

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);
    entry_idx_end = (index_end_ptr != NULL) ? *index_end_ptr : *index_ptr;


    if ((ret = rtk_acl_ruleEntrySize_get(unit, phase, &aclDumpEntInfo.entry_size)) != RT_ERR_OK)
    {
        diag_util_printf("rtk_acl_ruleEntry_read() fail!\n");
        DIAG_ERR_PRINT(ret);
        return CPARSER_OK;
    }

    /* obtain buffer for etnry */
    if ((aclDumpEntInfo.pEntry_buffer = malloc(aclDumpEntInfo.entry_size)) == NULL)
    {
        diag_util_printf("malloc() fail!\n");
        return CPARSER_OK;
    }

    for (i = *index_ptr; i <= entry_idx_end; i++)
    {
        osal_memset(aclDumpEntInfo.pEntry_buffer, 0, aclDumpEntInfo.entry_size);
        _cparser_cmd_acl_dump_entry_phase_entry(unit, phase, i, &aclDumpEntInfo);
        diag_util_mprintf("\n");
    }

    if (aclDumpEntInfo.pEntry_buffer)
    {
        free(aclDumpEntInfo.pEntry_buffer);
    }

    _cparser_cmd_acl_dump_entry_phase_entry_template_name_mapping_display();

    return CPARSER_OK;

}/* end cparser_cmd_acl_dump_entry_phase_phase_entry_index_index_end */
#endif

#ifdef CMD_ACL_SET_ENTRY_PHASE_PHASE_ENTRY_INDEX_ACTION_BYPASS_EGRESS_VLAN_STATE_DISABLE_ENABLE
/*
 * acl set entry phase <UINT:phase> entry <UINT:index> action bypass egress-vlan state ( disable | enable )
 */
cparser_result_t
cparser_cmd_acl_set_entry_phase_phase_entry_index_action_bypass_egress_vlan_state_disable_enable(
    cparser_context_t *context,
    uint32_t *phase_ptr,
    uint32_t *index_ptr)
{
    uint32              unit;
    int32               ret;
    rtk_acl_action_t    action;
    rtk_enable_t        enable;
    rtk_acl_phase_t     phase;

    DIAG_UTIL_FUNC_INIT(unit);

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);

#if defined(CONFIG_SDK_RTL9310)
    if (DIAG_OM_GET_FAMILYID(RTL9310_FAMILY_ID))
    {
        if (ACL_PHASE_IACL != phase)
        {
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
        }
    }
#endif
    osal_memset(&action, 0, sizeof(rtk_acl_action_t));

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_get(unit, phase, *index_ptr, &action), ret);

    if('e' == TOKEN_CHAR(11, 0))
    {
        enable = ENABLED;
    }
    else
    {
        enable = DISABLED;
    }

    switch (phase)
    {
        #if defined(CONFIG_SDK_RTL9310)
        case ACL_PHASE_IACL:
            action.iact.bypass_data.egr_vlan = enable;
            break;
        #endif  /* defined(CONFIG_SDK_RTL9310)) */
        default:
            diag_util_printf("%s ", _diag_acl_phaseStr_get(phase));
            diag_util_printf("doesn't support the action\n");
            return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_ruleAction_set(unit, phase, *index_ptr, &action), ret);

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_set_entry_phase_phase_entry_index_action_bypass_egress_vlan_state_disable_enable */
#endif

#ifdef CMD_ACL_GET_PHASE_PHASE_FIELD_FIELD_NAME
/*
 * acl get phase <UINT:phase> field <STRING:field_name>
 */
cparser_result_t
cparser_cmd_acl_get_phase_phase_field_field_name(
    cparser_context_t *context,
    uint32_t *phase_ptr,
    char **field_name_ptr)
{
    rtk_acl_phase_t              phase;
    rtk_acl_fieldUsr2Template_t  info;
    uint32                       unit, i, j;
    uint32                       num_element;
    int32                        ret;

    DIAG_UTIL_FUNC_INIT(unit);

    phase = _diag_acl_phaseTrans(unit, *phase_ptr);

    num_element = (sizeof(diag_acl_field_list)/sizeof(diag_acl_field_t));

    for (i = 0; i < num_element; ++i)
    {
        if (!strcasecmp(*field_name_ptr, diag_acl_field_list[i].user_info))
            break;
    }

    if (num_element <= i)
    {
        diag_util_printf("Invalid field %s!\n", *field_name_ptr);
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_acl_fieldUsr2Template_get(unit, phase, diag_acl_field_list[i].type, &info), ret);

    i = 0;
    while(diag_pie_template_list[i].type != TMPLTE_FIELD_END)
    {
        for (j = 0; j < RTK_ACL_USR2TMPLTE_MAX; ++j)
        {
            if (TMPLTE_FIELD_NONE == info.fields[j])
                continue;
            else if (info.fields[j] == diag_pie_template_list[i].type)
                diag_util_printf("%s\n", diag_pie_template_list[i].user_info);
        }

        ++i;
    }

    return CPARSER_OK;
}   /* end of cparser_cmd_acl_get_phase_phase_field_field_name */
#endif

