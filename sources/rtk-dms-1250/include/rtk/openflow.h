/*
 * Copyright(c) Realtek Semiconductor Corporation, 2008-2014
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Definition those public ACL APIs and its data type in the SDK .
 *
 * Feature : The file have include the following module and sub-modules
 *            1) OpenFlow
 *
 */

#ifndef __RTK_OPENFLOW_H__
#define __RTK_OPENFLOW_H__

/*
 * Include Files
 */
#include <common/rt_autoconf.h>
#include <common/rt_type.h>
#include <common/rt_error.h>

/*
 * Symbol Definition
 */
#define RTK_OF_TEMPLATE_FIELD_MAX   12
#define RTK_OF_BLOCK_TEMPLATE_MAX   2
#define RTK_OF_MATCH_FIELD_MAX      28  /* maximum user field size among chips*/
#define RTK_OF_ENTRY_MAX            56  /* maximum entry size among chips*/

typedef uint32  rtk_of_flow_id_t;           /* OpenFlow flow entry id */

/* Flow entry flags */
#define RTK_OF_FLAG_FLOWENTRY_MD_CMP       0x00000001/* matching matadata match field */
#define RTK_OF_FLAG_FLOWENTRY_REPLACE      0x00000002/* replace the existing entry with the new info */

/*
 * Data Declaration
 */
typedef enum rtk_of_classifierType_e
{
    OF_CLASSIFIER_TYPE_PORT,
    OF_CLASSIFIER_TYPE_VLAN,
    OF_CLASSIFIER_TYPE_VLAN_AND_PORT,
    OF_CLASSIFIER_TYPE_END
} rtk_of_classifierType_t;

typedef struct rtk_of_classifierData_e
{
    union
    {
        rtk_port_t	port;/* used when OF_CLASSIFIER_TYPE_PORT is specified */
        rtk_vlan_t	vlan;/* used when OF_CLASSIFIER_TYPE_VLAN or OF_CLASSIFIER_TYPE_VLAN_AND_PORT is specified */
    };

    rtk_enable_t	enable;
} rtk_of_classifierData_t;

typedef enum rtk_of_flowtable_phase_e
{
    FT_PHASE_IGR_FT_0,/* full match flow table */
    FT_PHASE_IGR_FT_1,/* L2 flow table */
    FT_PHASE_IGR_FT_2,/* L3 flow table */
    FT_PHASE_IGR_FT_3,/* full match flow table */
    FT_PHASE_EGR_FT_0,/* full match flow table */
    FT_PHASE_END
} rtk_of_flowtable_phase_t;

/* match field in user view */
typedef enum rtk_of_matchfieldType_e
{
/*
 * Misc
 */
    MATCH_FIELD_DROP_PRECEDENCE,
    MATCH_FIELD_LOOPBACK_TIME,               /* target loopback time */
    MATCH_FIELD_OUTPUT_ACTION,               /* output action forwards a packet to a specified OpenFlow port */
    MATCH_FIELD_OUTPUT_DATA,                 /* data corresponds to output action */
    MATCH_FIELD_METADATA,
    MATCH_FIELD_TEMPLATE_ID,                 /* template ID the entry maps to */
    MATCH_FIELD_FRAME_TYPE,                  /* frame type
                                              * 0b00: ARP
                                              * 0b01: L2 only
                                              * 0b10: IPv4
                                              * 0b11: IPv6
                                              */

/*
 * L2
 */
    MATCH_FIELD_TRUNK_PRESENT,               /* trunk group present bit */
    MATCH_FIELD_TRUNK_ID,                    /* trunk group ID */
    MATCH_FIELD_IN_PHY_PORT,                 /* switch physical input port */
    MATCH_FIELD_IN_PHY_PORTMASK,             /* switch physical input port mask */
    MATCH_FIELD_OUT_PHY_PORT,                /* switch physical output port */
    MATCH_FIELD_OUT_PHY_PORTMASK,            /* switch physical output port mask */

    MATCH_FIELD_ETH_DST,                     /* destination MAC address. The field can optional represent
                                              * 'destination hardware address' of ARP header for ARP/RARP packet
                                              */
    MATCH_FIELD_ETH_SRC,                     /* source MAC address. The field can optional represent
                                              * 'source hardware address' of ARP header for ARP/RARP packet
                                              */
    MATCH_FIELD_ITAG_EXIST,                  /* packet with inner tag */
    MATCH_FIELD_OTAG_EXIST,                  /* packet with outer tag */
    MATCH_FIELD_ITAG_FMT,                    /* 0b0: inner tag packet
                                              * 0b1: inner priority tag or untag packet
                                              */
    MATCH_FIELD_OTAG_FMT,                    /* 0b0: outer tag packet
                                              * 0b1: outer priority tag or untag packet
                                              */
    MATCH_FIELD_OTAG_PCP,                    /* O-TAG priority */
    MATCH_FIELD_OTAG_DEI,                    /* O-TAG DEI field */
    MATCH_FIELD_OTAG_VID,                    /* O-TAG VID */
    MATCH_FIELD_ITAG_PCP,                    /* I-TAG priority */
    MATCH_FIELD_ITAG_CFI,                    /* I-TAG CFI field */
    MATCH_FIELD_ITAG_VID,                    /* I-TAG VID */

    MATCH_FIELD_FRAME_TYPE_L2,               /* L2 frame type
                                              * 0b00: Ethernet
                                              * 0b01: LLC_SNAP
                                              * 0b10: LLC_Other
                                              * 0b11: Reserved
                                              */
    MATCH_FIELD_ETH_TYPE,                    /* ethernet type/length */

    MATCH_FIELD_ARP_SHA,                     /* 'source hardware address' of ARP header for ARP/RARP packet */
    MATCH_FIELD_ARP_THA,                     /* 'destination hardware address' of ARP header for ARP/RARP packet */
    MATCH_FIELD_ARP_OP,                      /* ARP/RARP Opcode */

    MATCH_FIELD_OMPLS_LABEL,                 /* outer MPLS label */
    MATCH_FIELD_OMPLS_TC,                    /* outer MPLS label EXP */
    MATCH_FIELD_OMPLS_LABEL_EXIST,           /* outer MPLS label exist */
    MATCH_FIELD_OMPLS_BOS,                   /* outer MPLS label BoS */
    MATCH_FIELD_IMPLS_LABEL,                 /* inner MPLS label */
    MATCH_FIELD_IMPLS_TC,                    /* inner MPLS label EXP */
    MATCH_FIELD_IMPLS_LABEL_EXIST,           /* inner MPLS label exist */
    MATCH_FIELD_IMPLS_BOS,                   /* inner MPLS label BoS */

    MATCH_FIELD_DSAP,                        /* DSAP for LLC/SNAP packet*/
    MATCH_FIELD_SSAP,                        /* SSAP for LLC/SNAP packet*/
    MATCH_FIELD_SNAP_OUI,                    /* OUI in SNAP header*/

/*
 * L3/L4
 */
    MATCH_FIELD_L4_PROTO,                    /* Layer 4 format
                                              * 0b000: UDP
                                              * 0b001: TCP
                                              * 0b010: ICMP/ICMPv6
                                              * 0b011: IGMP(Exculde MLD)
                                              * 0b100: MLD
                                              * 0b101: SCTP
                                              * 0b110: Layer4 Unknown
                                              * 0b111: Reserved
                                              */
    MATCH_FIELD_IPV4_SRC,                    /* IPv4 source IP. The field represents 'source protocol address' of
                                              * ARP header for ARP/RARP packet
                                              */
    MATCH_FIELD_IPV4_DST,                    /* IPv4 destination IP. The field represents 'destination protocol address' of
                                              * ARP header for ARP/RARP packet
                                              */
    MATCH_FIELD_IPV6_SRC,
    MATCH_FIELD_IPV6_DST,
    MATCH_FIELD_IPV6_FLABEL,                 /* IPv6 flow label */
    MATCH_FIELD_IP4TOS_IP6TC,                /* IPv4 TOS byte/IPv6 Traffic Class byte */
    MATCH_FIELD_IP_ECN,                      /* IP ECN */
    MATCH_FIELD_IP_DSCP,                     /* IP DSCP */
    MATCH_FIELD_IP_PROTO,                    /* IPv4 protocol/IPv6 next header */
    MATCH_FIELD_IP_FLAG,                     /* IPv4 flags */
    MATCH_FIELD_IP_FRAG,                     /* IPv4 or IPv6 fragment , include first fragment */
    MATCH_FIELD_IP_TTL,                      /* IPv4 TTL/IPv6 hop limit
                                              * 0b00: TTL = 0
                                              * 0b01: TTL = 1
                                              * 0b10: 2<= TTL < 255
                                              * 0b11: TTL = 255
                                              */
    MATCH_FIELD_IP6_HDR_UNSEQ,               /* IPv6 packet headers were not in the order */
    MATCH_FIELD_IP6_HDR_UNREP,               /* IPv6 packet has unexcepted repeat header */
    MATCH_FIELD_IP6_NONEXT_HDR_EXIST,        /* IPv6 packet with no next extended header */
    MATCH_FIELD_IP6_MOB_HDR_EXIST,           /* IPv6 packet with mobility header*/
    MATCH_FIELD_IP6_ESP_HDR_EXIST,           /* IPv6 packet with ESP header */
    MATCH_FIELD_IP6_AUTH_HDR_EXIST,          /* IPv6 packet with authentication header */
    MATCH_FIELD_IP6_DEST_HDR_EXIST,          /* IPv6 packet with destination option header */
    MATCH_FIELD_IP6_FRAG_HDR_EXIST,          /* IPv6 packet with fragment header */
    MATCH_FIELD_IP6_ROUTING_HDR_EXIST,       /* IPv6 packet with routing header */
    MATCH_FIELD_IP6_HOP_HDR_EXIST,           /* IPv6 packet with hop-by-hop header */

    MATCH_FIELD_IGMP_TYPE,                   /* IGMP type */
    MATCH_FIELD_IGMP_MAX_RESP_CODE,          /* IGMP max response code */
    MATCH_FIELD_ICMP_CODE,                   /* ICMP/ICMPv6 code */
    MATCH_FIELD_ICMP_TYPE,                   /* ICMP/ICMPv6 type */

    MATCH_FIELD_L4_HDR,                      /* unknown L4 header byte 0~3 */
    MATCH_FIELD_L4_SRC_PORT,                 /* TCP/UDP/SCTP source port */
    MATCH_FIELD_L4_DST_PORT,                 /* TCP/UDP/SCTP destination port */
    MATCH_FIELD_TCP_ECN,                     /* ECN in TCP flag */
    MATCH_FIELD_TCP_FLAG,                    /* TCP flag */
    MATCH_FIELD_TCP_NONZEROSEQ,              /* TCP sequence non-zero */

/*
 * Application
 */
    MATCH_FIELD_VID_RANGE,                   /* VID range check result */
    MATCH_FIELD_L4_PORT_RANGE,               /* TCP/UDP/SCTP port range check result */
    MATCH_FIELD_IP_RANGE,                    /* IPv4/IPv6 address range check result */
    MATCH_FIELD_LEN_RANGE,                   /* Packet length(L2 or L3 length is configurable) range check result */

    MATCH_FIELD_FIELD_SELECTOR_VALID_MSK,    /* Field selector valid mask */
    MATCH_FIELD_FIELD_SELECTOR0,             /* Field selector 0 output */
    MATCH_FIELD_FIELD_SELECTOR1,             /* Field selector 1 output */
    MATCH_FIELD_FIELD_SELECTOR2,             /* Field selector 2 output */
    MATCH_FIELD_FIELD_SELECTOR3,             /* Field selector 3 output */
    MATCH_FIELD_FIELD_SELECTOR4,             /* Field selector 4 output */
    MATCH_FIELD_FIELD_SELECTOR5,             /* Field selector 5 output */
    MATCH_FIELD_FIELD_SELECTOR6,             /* Field selector 6 output */
    MATCH_FIELD_FIELD_SELECTOR7,             /* Field selector 7 output */
    MATCH_FIELD_FIELD_SELECTOR8,             /* Field selector 8 output */
    MATCH_FIELD_FIELD_SELECTOR9,             /* Field selector 9 output */
    MATCH_FIELD_FIELD_SELECTOR10,            /* Field selector 10 output */
    MATCH_FIELD_FIELD_SELECTOR11,            /* Field selector 11 output */
    MATCH_FIELD_FIELD_SELECTOR12,            /* Field selector 12 output */
    MATCH_FIELD_FIELD_SELECTOR13,            /* Field selector 13 output */

    MATCH_FIELD_GRE_KEY,                     /* GRE key */
    MATCH_FIELD_VXLAN_VNI,                   /* VNI of VxLAN packet */
    MATCH_FIELD_GTP_TEID,                    /* TEID of GTP(GPRS Tunneling Protocol) packet */

    MATCH_FIELD_END
} rtk_of_matchfieldType_t;

/* match field list in bitmap which the bit order is corresponding to rtk_of_matchfieldType_t */
typedef struct rtk_of_matchfieldList_e
{
    rtk_bitmap_t  field_bmp[BITMAP_ARRAY_CNT(MATCH_FIELD_END)];
} rtk_of_matchfieldList_t;

typedef struct rtk_of_ftTemplateIdx_s
{
    uint32  template_id[RTK_OF_BLOCK_TEMPLATE_MAX];/* index to templates */
} rtk_of_ftTemplateIdx_t;

typedef struct rtk_of_flowClear_s
{
    uint32  start_idx;  /* delete from this entry */
    uint32  end_idx;    /* delete to this entry */
} rtk_of_flowClear_t;

typedef struct rtk_of_flowMove_s
{
    uint32  length;     /* number of entries to move */
    uint32  move_from;  /* move from this entry */
    uint32  move_to;    /* move to this entry */
} rtk_of_flowMove_t;

typedef struct rtk_of_flowOperation_s
{
    rtk_enable_t reverse;/* reverset the result */
    rtk_enable_t aggr_1; /* aggregate the result of two consecutive rules (entry 0&1, 2&3, ¡K, 2n&2n+1)
                            in the same block */
    rtk_enable_t aggr_2; /* aggregate the result of two entries resided in consecutive blocks
                            (block 0&1, 2&3, ¡K, 2n&2n+1) */
} rtk_of_flowOperation_t;

typedef enum rtk_of_meterType_e
{
    OF_METER_TYPE_DROP,
    OF_METER_TYPE_DSCP_REMARK,
    OF_METER_TYPE_END
} rtk_of_meterType_t;

typedef struct rtk_of_flowInsMeter_s
{
    uint32  			meter_id;	/* index to retrieve meter/marking information */
    rtk_of_meterType_t	yellow;
    rtk_of_meterType_t	red;
} rtk_of_flowInsMeter_t;

typedef struct rtk_of_flowInsWriteMd_s
{
    uint32 data;
    uint32 mask;
} rtk_of_flowInsWriteMd_t;

typedef enum rtk_of_gotoTblType_e
{
    OF_GOTOTBL_TYPE_NORMAL,			/* normal Goto operation */
    OF_GOTOTBL_TYPE_APPLY_AND_LB,	/* apply current action set and loopback packet to OpenFlow pipeline */
    OF_GOTOTBL_TYPE_LB,				/* don't apply current action set, loopback packet to OpenFlow pipeline directly */
    OF_GOTOTBL_TYPE_END
} rtk_of_gotoTblType_t;

typedef struct rtk_of_flowInsGotoTbl_s
{
    rtk_of_gotoTblType_t type;
    uint32 tbl_id;
} rtk_of_flowInsGotoTbl_t;

typedef enum rtk_of_flowActPopMplsType_e
{
    OF_POP_MPLS_TYPE_OUTERMOST_LABEL,/* pop outermost label */
    OF_POP_MPLS_TYPE_DOUBLE_LABEL,	 /* pop double label */
    OF_POP_MPLS_TYPE_END
} rtk_of_flowActPopMplsType_t, rtk_of_actBucketPopMplsType_t;

typedef enum rtk_of_pushMplsMode_e
{
    OF_PUSH_MPLS_MODE_NORMAL,		/* normal push operation */
    OF_PUSH_MPLS_MODE_ENCAPTBL,		/* push/swap MPLS label according to mpls_encap_idx */
    OF_PUSH_MPLS_MODE_END
} rtk_of_pushMplsMode_t;

typedef enum rtk_of_pushMplsVpnType_e
{
    OF_PUSH_MPLS_VPN_TYPE_L2,		/* encap packet to L2 MPLS VPN */
    OF_PUSH_MPLS_VPN_TYPE_L3,		/* encap packet to L3 MPLS VPN */
    OF_PUSH_MPLS_VPN_TYPE_END
} rtk_of_pushMplsVpnType_t;

typedef struct rtk_of_flowActPushMpls_s
{
    rtk_of_pushMplsMode_t		push_mode;
    rtk_of_pushMplsVpnType_t	vpn_type;
    uint32						mpls_encap_idx; /* index to MPLS encap table. only used when vpn_type=OF_PUSH_MPLS_MODE_ENCAPTBL */
    uint32						etherType;		/* 0:0x8847, 1:0x8848 */
} rtk_of_flowActPushMpls_t, rtk_of_actBucketPushMpls_t;

typedef struct rtk_of_flowActPushVlan_s
{
    uint32	etherType_idx;/* index to inner TPID pool for VLAN EtherType */
} rtk_of_flowActPushVlan_t, rtk_of_actBucketPushVlan_t;

typedef enum rtk_of_setFieldType_e
{
    OF_SET_FIELD_TYPE_NONE,
    OF_SET_FIELD_TYPE_SA,
    OF_SET_FIELD_TYPE_DA,
    OF_SET_FIELD_TYPE_VLAN_PRI,
    OF_SET_FIELD_TYPE_VLAN_ID,
    OF_SET_FIELD_TYPE_MPLS_TC,
    OF_SET_FIELD_TYPE_MPLS_TTL,
    OF_SET_FIELD_TYPE_MPLS_LABEL,
    OF_SET_FIELD_TYPE_IP_DSCP,
    OF_SET_FIELD_TYPE_IP_TTL,
    OF_SET_FIELD_TYPE_IP_FLAG_RSVD,
    OF_SET_FIELD_TYPE_IP4_SIP,
    OF_SET_FIELD_TYPE_IP4_DIP,
    OF_SET_FIELD_TYPE_L4_SPORT,
    OF_SET_FIELD_TYPE_L4_DPORT,
    OF_SET_FIELD_TYPE_END
} rtk_of_setFieldType_t;

typedef enum rtk_of_igrFT0SFType0_e
{
    OF_IGR_FT0_SF_TYPE0_NONE = OF_SET_FIELD_TYPE_NONE,
    OF_IGR_FT0_SF_TYPE0_SA = OF_SET_FIELD_TYPE_SA,
    OF_IGR_FT0_SF_TYPE0_VLAN_PRI = OF_SET_FIELD_TYPE_VLAN_PRI,
    OF_IGR_FT0_SF_TYPE0_MPLS_TC = OF_SET_FIELD_TYPE_MPLS_TC,
    OF_IGR_FT0_SF_TYPE0_MPLS_TTL = OF_SET_FIELD_TYPE_MPLS_TTL,
    OF_IGR_FT0_SF_TYPE0_IP_DSCP = OF_SET_FIELD_TYPE_IP_DSCP,
    OF_IGR_FT0_SF_TYPE0_IP_TTL = OF_SET_FIELD_TYPE_IP_TTL,
    OF_IGR_FT0_SF_TYPE0_IP_FLAG_RSVD = OF_SET_FIELD_TYPE_IP_FLAG_RSVD,
    OF_IGR_FT0_SF_TYPE0_END = OF_SET_FIELD_TYPE_END
} rtk_of_igrFT0SFType0_t;

typedef struct rtk_of_igrFT0ActSF0_s
{
    rtk_of_igrFT0SFType0_t	field_type;
    union
    {
        rtk_mac_t           mac;        /* OF_SET_FIELD_TYPE0_SA:       SMAC */
        uint32				field_data;	/* OF_SET_FIELD_TYPE0_VLAN_PRI: VLAN Priority
                                         * OF_SET_FIELD_TYPE0_MPLS_TC:  MPLS Traffic Class
                                         * OF_SET_FIELD_TYPE0_MPLS_TTL: MPLS TTL
                                         * OF_SET_FIELD_TYPE0_IP_DSCP:  IPv4/IPv6 DSCP
                                         * OF_SET_FIELD_TYPE0_IP_TTL: 	IPv4 TTL/IPv6 Hop Limit
                                         */
    };
} rtk_of_igrFT0ActSF0_t, rtk_of_actBucketSF0_t;

typedef enum rtk_of_igrFT0SFType1_t
{
    OF_IGR_FT0_SF_TYPE1_NONE = OF_SET_FIELD_TYPE_NONE,
    OF_IGR_FT0_SF_TYPE1_DA = OF_SET_FIELD_TYPE_DA,
    OF_IGR_FT0_SF_TYPE1_VLAN_PRI = OF_SET_FIELD_TYPE_VLAN_PRI,
    OF_IGR_FT0_SF_TYPE1_MPLS_LABEL = OF_SET_FIELD_TYPE_MPLS_LABEL,
    OF_IGR_FT0_SF_TYPE1_MPLS_TC = OF_SET_FIELD_TYPE_MPLS_TC,
    OF_IGR_FT0_SF_TYPE1_MPLS_TTL = OF_SET_FIELD_TYPE_MPLS_TTL,
    OF_IGR_FT0_SF_TYPE1_IP_DSCP = OF_SET_FIELD_TYPE_IP_DSCP,
    OF_IGR_FT0_SF_TYPE1_IP_TTL = OF_SET_FIELD_TYPE_IP_TTL,
    OF_IGR_FT0_SF_TYPE1_END = OF_SET_FIELD_TYPE_END
} rtk_of_igrFT0SFType1_t;

typedef struct rtk_of_igrFT0ActSF1_s
{
    rtk_of_igrFT0SFType1_t  field_type;
    union
    {
        rtk_mac_t           mac;        /* OF_SET_FIELD_TYPE1_DA:         DMAC */
        uint32				field_data;	/* OF_SET_FIELD_TYPE1_VLAN_PRI:   VLAN Priority
                                         * OF_SET_FIELD_TYPE1_MPLS_LABEL: MPLS Label
                                         * OF_SET_FIELD_TYPE1_MPLS_TC:    MPLS Traffic Class
                                         * OF_SET_FIELD_TYPE1_MPLS_TTL:   MPLS TTL
                                         * OF_SET_FIELD_TYPE1_IP_DSCP:    IPv4/IPv6 DSCP
                                         * OF_SET_FIELD_TYPE1_IP_TTL: 	  IPv4 TTL/IPv6 Hop Limit
                                         */
    };
} rtk_of_igrFT0ActSF1_t, rtk_of_actBucketSF1_t;

typedef enum rtk_of_igrFT0SFType2_e
{
    OF_IGR_FT0_SF_TYPE2_NONE = OF_SET_FIELD_TYPE_NONE,
    OF_IGR_FT0_SF_TYPE2_VLAN_ID = OF_SET_FIELD_TYPE_VLAN_ID,
    OF_IGR_FT0_SF_TYPE2_VLAN_PRI = OF_SET_FIELD_TYPE_VLAN_PRI,
    OF_IGR_FT0_SF_TYPE2_MPLS_LABEL = OF_SET_FIELD_TYPE_MPLS_LABEL,
    OF_IGR_FT0_SF_TYPE2_MPLS_TC = OF_SET_FIELD_TYPE_MPLS_TC,
    OF_IGR_FT0_SF_TYPE2_MPLS_TTL = OF_SET_FIELD_TYPE_MPLS_TTL,
    OF_IGR_FT0_SF_TYPE2_IP_DSCP = OF_SET_FIELD_TYPE_IP_DSCP,
    OF_IGR_FT0_SF_TYPE2_IP_TTL = OF_SET_FIELD_TYPE_IP_TTL,
    OF_IGR_FT0_SF_TYPE2_END = OF_SET_FIELD_TYPE_END
} rtk_of_igrFT0SFType2_t;

typedef struct rtk_of_igrFT0ActSF2_s
{
    rtk_of_igrFT0SFType2_t	field_type;
    uint32					field_data;	/* OF_SET_FIELD_TYPE2_VLAN_ID:    VLAN ID
                                         * OF_SET_FIELD_TYPE2_VLAN_PRI:   VLAN Priority
                                         * OF_SET_FIELD_TYPE2_MPLS_LABEL: MPLS label
                                         * OF_SET_FIELD_TYPE2_MPLS_TC:    MPLS Traffic Class
                                         * OF_SET_FIELD_TYPE2_MPLS_TTL:   MPLS TTL
                                         * OF_SET_FIELD_TYPE2_IP_DSCP:    IPv4/IPv6 DSCP
                                         * OF_SET_FIELD_TYPE2_IP_TTL: 	  IPv4 TTL/IPv6 Hop Limit
                                         */
} rtk_of_igrFT0ActSF2_t, rtk_of_actBucketSF2_t;

typedef enum rtk_of_igrFT0SFType3_e
{
    OF_IGR_FT0_SF_TYPE3_NONE = OF_SET_FIELD_TYPE_NONE,
    OF_IGR_FT0_SF_TYPE3_SIP = OF_SET_FIELD_TYPE_IP4_SIP,
    OF_IGR_FT0_SF_TYPE3_DIP = OF_SET_FIELD_TYPE_IP4_DIP,
    OF_IGR_FT0_SF_TYPE3_VLAN_PRI = OF_SET_FIELD_TYPE_VLAN_PRI,
    OF_IGR_FT0_SF_TYPE3_MPLS_LABEL = OF_SET_FIELD_TYPE_MPLS_LABEL,
    OF_IGR_FT0_SF_TYPE3_MPLS_TC = OF_SET_FIELD_TYPE_MPLS_TC,
    OF_IGR_FT0_SF_TYPE3_MPLS_TTL = OF_SET_FIELD_TYPE_MPLS_TTL,
    OF_IGR_FT0_SF_TYPE3_IP_DSCP = OF_SET_FIELD_TYPE_IP_DSCP,
    OF_IGR_FT0_SF_TYPE3_END = OF_SET_FIELD_TYPE_END
} rtk_of_igrFT0SFType3_t;

typedef struct rtk_of_igrFT0ActSF3_s
{
    rtk_of_igrFT0SFType3_t	field_type;
    union
    {
        ipaddr_t                ip;         /* OF_SET_FIELD_TYPE3_SIP:    	  Source IPv4 address
                                             * OF_SET_FIELD_TYPE3_SIP:    	  Dest. IPv4 address
                                             */
        uint32					field_data;	/* OF_SET_FIELD_TYPE3_VLAN_PRI:   VLAN Priority
                                             * OF_SET_FIELD_TYPE3_MPLS_LABEL: MPLS label
                                             * OF_SET_FIELD_TYPE3_MPLS_TC:    MPLS Traffic Class
                                             * OF_SET_FIELD_TYPE3_MPLS_TTL:   MPLS TTL
                                             * OF_SET_FIELD_TYPE3_IP_DSCP:    IPv4/IPv6 DSCP
                                             */
    };
} rtk_of_igrFT0ActSF3_t;

typedef enum rtk_of_igrFT0SFType4_e
{
    OF_IGR_FT0_SF_TYPE4_NONE = OF_SET_FIELD_TYPE_NONE,
    OF_IGR_FT0_SF_TYPE4_L4_SPORT = OF_SET_FIELD_TYPE_L4_SPORT,
    OF_IGR_FT0_SF_TYPE4_L4_DPORT = OF_SET_FIELD_TYPE_L4_DPORT,
    OF_IGR_FT0_SF_TYPE4_VLAN_PRI = OF_SET_FIELD_TYPE_VLAN_PRI,
    OF_IGR_FT0_SF_TYPE4_MPLS_LABEL = OF_SET_FIELD_TYPE_MPLS_LABEL,
    OF_IGR_FT0_SF_TYPE4_MPLS_TC = OF_SET_FIELD_TYPE_MPLS_TC,
    OF_IGR_FT0_SF_TYPE4_MPLS_TTL = OF_SET_FIELD_TYPE_MPLS_TTL,
    OF_IGR_FT0_SF_TYPE4_IP_DSCP = OF_SET_FIELD_TYPE_IP_DSCP,
    OF_IGR_FT0_SF_TYPE4_END = OF_SET_FIELD_TYPE_END
} rtk_of_igrFT0SFType4_t;

typedef struct rtk_of_igrFT0ActSF4_e
{
    rtk_of_igrFT0SFType4_t	field_type;
    uint32					field_data;/* OF_SET_FIELD_TYPE4_L4_SPORT:   L4 source port
                                        * OF_SET_FIELD_TYPE4_L4_DPORT:   L4 dest. port
                                        * OF_SET_FIELD_TYPE4_VLAN_PRI:   VLAN Priority
                                        * OF_SET_FIELD_TYPE4_MPLS_LABEL: MPLS label
                                        * OF_SET_FIELD_TYPE4_MPLS_TC:    MPLS Traffic Class
                                        * OF_SET_FIELD_TYPE4_MPLS_TTL:   MPLS TTL
                                        * OF_SET_FIELD_TYPE4_IP_DSCP:    IPv4/IPv6 DSCP
                                        */
} rtk_of_igrFT0ActSF4_t;

typedef enum rtk_of_outputType_e
{
    OF_OUTPUT_TYPE_NONE,                /* disable output action */
    OF_OUTPUT_TYPE_PHY_PORT_EX_SRC_PORT,/* physical port and excludes source port */
    OF_OUTPUT_TYPE_PHY_PORT,			/* physical port */
    OF_OUTPUT_TYPE_TRK_PORT_EX_SRC_PORT,/* trunk port and excludes source port */
    OF_OUTPUT_TYPE_TRK_PORT,			/* trunk port */
    OF_OUTPUT_TYPE_MULTI_EGR_PORT,      /* multiple egress ports and excludes source port */
    OF_OUTPUT_TYPE_IN_PORT,             /* packet's ingress port */
    OF_OUTPUT_TYPE_FLOOD,               /* FLOOD reserved port(all ports with VLAN, trunk, spanning tree filter and excludes source port) */
    OF_OUTPUT_TYPE_LB,                  /* NORMAL reserved port(loopback to traditional pipeline for forwarding) */
    OF_OUTPUT_TYPE_TUNNEL,              /* tunnel interface */
    OF_OUTPUT_TYPE_FAILOVER,            /* failover to the first link up port */
    OF_OUTPUT_TYPE_END
} rtk_of_outputType_t, rtk_of_igrFT0ActOutputType_t, rtk_of_igrFT2ActOutputType_t;

typedef struct rtk_of_igrFT0ActOutput_s
{
    rtk_of_igrFT0ActOutputType_t    type;

    union
    {
        rtk_port_t                  port;      /* OF_OUTPUT_TYPE_PHY_PORT_EX_SRC_PORT: physical egress port
                                                * OF_OUTPUT_TYPE_PHY_PORT:             physical egress port
                                                */
        rtk_trunk_t                 trunk;     /* OF_OUTPUT_TYPE_TRK_PORT_EX_SRC_PORT: trunk ID
                                                * OF_OUTPUT_TYPE_TRK_PORT:             trunk ID
                                                */
        rtk_portmask_t              portmask;  /* OF_OUTPUT_TYPE_MULTI_EGR_PORT:       egress portmask
                                                * OF_OUTPUT_TYPE_FAILOVER:             egress portmask
                                                */
        uint32 				        intf;      /* OF_OUTPUT_TYPE_TUNNEL:               interface ID */
    };

    uint32 				            devID;     /* OF_OUTPUT_TYPE_PHY_PORT_EX_SRC_PORT: stacking device ID
                                                * OF_OUTPUT_TYPE_PHY_PORT:             stacking device ID
                                                */
} rtk_of_igrFT0ActOutput_t, rtk_of_actBucketOutput_t;

typedef struct rtk_of_igrFT0InsWriteAct_s
{
    rtk_enable_t 				    cp_ttl_inward;

    rtk_enable_t 				    pop_vlan;

    rtk_enable_t 				    pop_mpls;
    rtk_of_flowActPopMplsType_t	    pop_mpls_type;

    rtk_enable_t 				    push_mpls;
    rtk_of_flowActPushMpls_t	    push_mpls_data;

    rtk_enable_t 				    push_vlan;
    rtk_of_flowActPushVlan_t	    push_vlan_data;

    rtk_enable_t 				    cp_ttl_outward;

    rtk_enable_t 				    dec_mpls_ttl;

    rtk_enable_t 				    dec_ip_ttl;

    rtk_of_igrFT0ActSF0_t	        set_field_0_data;
    rtk_of_igrFT0ActSF1_t	        set_field_1_data;
    rtk_of_igrFT0ActSF2_t	        set_field_2_data;
    rtk_of_igrFT0ActSF3_t	        set_field_3_data;
    rtk_of_igrFT0ActSF4_t	        set_field_4_data;

    rtk_enable_t 				    set_queue;
    uint32						    qid;

    rtk_enable_t 				    group;
    uint32						    gid;

    rtk_of_igrFT0ActOutput_t	    output_data;
} rtk_of_igrFT0InsWriteAct_t;

typedef struct rtk_of_igrFTIns_s
{
    rtk_enable_t                meter_en;
    rtk_of_flowInsMeter_t       meter_data;

    rtk_enable_t                clearAct_en;

    rtk_enable_t                writeAct_en;
    rtk_of_igrFT0InsWriteAct_t  wa_data;

    rtk_enable_t                writeMetadata_en;
    rtk_of_flowInsWriteMd_t     wm_data;

    rtk_enable_t                gotoTbl_en;
    rtk_of_flowInsGotoTbl_t     gt_data;
} rtk_of_igrFT0Ins_t, rtk_of_igrFT3Ins_t;

typedef enum rtk_of_igrFT1ActSFType0_e
{
    OF_IGR_FT_SF_TYPE0_NONE = OF_SET_FIELD_TYPE_NONE,
    OF_IGR_FT_SF_TYPE0_VLAN_ID = OF_SET_FIELD_TYPE_VLAN_ID,
    OF_IGR_FT_SF_TYPE0_IP_DSCP = OF_SET_FIELD_TYPE_IP_DSCP,
    OF_IGR_FT_SF_TYPE0_END = OF_SET_FIELD_TYPE_END
} rtk_of_igrFT1ActSFType0_t;

typedef struct rtk_of_igrFT1ActSF0_e
{
    rtk_of_igrFT1ActSFType0_t   field_type;
    uint32					    field_data;/* OF_EGR_FT_SF_TYPE0_VLAN_ID: VLAN ID
                                            * OF_EGR_FT_SF_TYPE0_IP_DSCP: IPv4/IPv6 DSCP
                                            */
} rtk_of_igrFT1ActSF0_t;

typedef enum rtk_of_igrFT1ActSFType1_e
{
    OF_IGR_FT_SF_TYPE1_NONE = OF_SET_FIELD_TYPE_NONE,
    OF_IGR_FT_SF_TYPE1_VLAN_PRI = OF_SET_FIELD_TYPE_VLAN_PRI,
    OF_IGR_FT_SF_TYPE1_END = OF_SET_FIELD_TYPE_END
} rtk_of_igrFT1ActSFType1_t;

typedef struct rtk_of_igrFT1ActSF1_e
{
    rtk_of_igrFT1ActSFType1_t   field_type;
    uint32					    field_data;/* OF_EGR_FT_SF_TYPE0_VLAN_PRI: VLAN Priority */
} rtk_of_igrFT1ActSF1_t;

typedef enum rtk_of_igrFT1ActOutputType_e
{
    OF_IGR_FT1_ACT_OUTPUT_TYPE_NONE = OF_OUTPUT_TYPE_NONE,
    OF_IGR_FT1_ACT_OUTPUT_TYPE_PHY_PORT = OF_OUTPUT_TYPE_PHY_PORT,
    OF_IGR_FT1_ACT_OUTPUT_TYPE_TRK_PORT = OF_OUTPUT_TYPE_TRK_PORT,
    OF_IGR_FT1_ACT_OUTPUT_TYPE_MULT_EGR_PORT = OF_OUTPUT_TYPE_MULTI_EGR_PORT,
    OF_IGR_FT1_ACT_OUTPUT_TYPE_IN_PORT = OF_OUTPUT_TYPE_IN_PORT,
    OF_IGR_FT1_ACT_OUTPUT_TYPE_FLOOD = OF_OUTPUT_TYPE_FLOOD,
    OF_IGR_FT1_ACT_OUTPUT_TYPE_LB = OF_OUTPUT_TYPE_LB,
    OF_IGR_FT1_ACT_OUTPUT_TYPE_TUNNEL = OF_OUTPUT_TYPE_TUNNEL,
    OF_IGR_FT1_ACT_OUTPUT_TYPE_END = OF_OUTPUT_TYPE_END
} rtk_of_igrFT1ActOutputType_t;

typedef struct rtk_of_igrFT1ActOutput_s
{
    rtk_of_igrFT1ActOutputType_t    type;

    union
    {
        rtk_port_t                  port;      /* OF_OUTPUT_TYPE_PHY_PORT_EX_SRC_PORT: physical egress port */
        rtk_trunk_t                 trunk;     /* OF_OUTPUT_TYPE_TRK_PORT_EX_SRC_PORT: trunk ID */
        rtk_portmask_t              portmask;  /* OF_OUTPUT_TYPE_MULTI_EGR_PORT:       egress portmask */
        uint32 				        intf;      /* OF_OUTPUT_TYPE_TUNNEL:               interface ID */
    };

    uint32 				            devID;     /* OF_OUTPUT_TYPE_PHY_PORT_EX_SRC_PORT: stacking device ID */
} rtk_of_igrFT1ActOutput_t;

typedef struct rtk_of_igrFT1InsWriteAct_s
{
    rtk_enable_t 				    push_vlan;
    rtk_of_flowActPushVlan_t	    push_vlan_data;

    rtk_of_igrFT1ActSF0_t           set_field_0_data;
    rtk_of_igrFT1ActSF1_t           set_field_1_data;

    rtk_enable_t 				    set_queue;
    uint32						    qid;

    rtk_of_igrFT1ActOutput_t	    output_data;
} rtk_of_igrFT1InsWriteAct_t;

typedef struct rtk_of_igrFT1Ins_s
{
    rtk_enable_t                clearAct_en;

    rtk_enable_t                writeAct_en;
    rtk_of_igrFT1InsWriteAct_t  wa_data;

    rtk_enable_t                writeMetadata_en;
    rtk_of_flowInsWriteMd_t     wm_data;

    rtk_enable_t                gotoTbl_en;
    rtk_of_flowInsGotoTbl_t     gt_data;
} rtk_of_igrFT1Ins_t;

typedef enum rtk_of_igrFT2ActSFType0_e
{
    OF_IGR_FT2_ACT_SF_TYPE0_NONE = OF_SET_FIELD_TYPE_NONE,
    OF_IGR_FT2_ACT_SF_TYPE0_SA = OF_SET_FIELD_TYPE_SA,
    OF_IGR_FT2_ACT_SF_TYPE0_VLAN_PRI = OF_SET_FIELD_TYPE_VLAN_PRI,
    OF_IGR_FT2_ACT_SF_TYPE0_IP_DSCP = OF_SET_FIELD_TYPE_IP_DSCP,
    OF_IGR_FT2_ACT_SF_TYPE0_END = OF_SET_FIELD_TYPE_END
} rtk_of_igrFT2ActSFType0_t;

typedef struct rtk_of_igrFT2ActSF0_e
{
    rtk_of_igrFT2ActSFType0_t   field_type;
    union
    {
        rtk_mac_t               mac;       /* OF_IGR_FT2_ACT_SF_TYPE0_SA:       Source MAC */
        uint32					field_data;/* OF_IGR_FT2_ACT_SF_TYPE0_VLAN_PRI: VLAN priority
                                            * OF_IGR_FT2_ACT_SF_TYPE0_IP_DSCP:  IPv4/IPv6 DSCP
                                            */
    };
} rtk_of_igrFT2ActSF0_t;

typedef enum rtk_of_igrFT2ActSFType1_e
{
    OF_IGR_FT2_ACT_SF_TYPE1_NONE = OF_SET_FIELD_TYPE_NONE,
    OF_IGR_FT2_ACT_SF_TYPE1_DA = OF_SET_FIELD_TYPE_DA,
    OF_IGR_FT2_ACT_SF_TYPE1_VLAN_PRI = OF_SET_FIELD_TYPE_VLAN_PRI,
    OF_IGR_FT2_ACT_SF_TYPE1_IP_DSCP = OF_SET_FIELD_TYPE_IP_DSCP,
    OF_IGR_FT2_ACT_SF_TYPE1_END = OF_SET_FIELD_TYPE_END
} rtk_of_igrFT2ActSFType1_t;

typedef struct rtk_of_igrFT2ActSF1_e
{
    rtk_of_igrFT2ActSFType1_t   field_type;
    union
    {
        rtk_mac_t               mac;       /* OF_IGR_FT2_ACT_SF_TYPE1_DA:       Destination MAC */
        uint32					field_data;/* OF_IGR_FT2_ACT_SF_TYPE1_VLAN_PRI: VLAN priority
                                            * OF_IGR_FT2_ACT_SF_TYPE1_IP_DSCP:  IPv4/IPv6 DSCP                                            */
    };
} rtk_of_igrFT2ActSF1_t;

typedef enum rtk_of_igrFT2ActSFType2_e
{
    OF_IGR_FT2_ACT_SF_TYPE2_NONE = OF_SET_FIELD_TYPE_NONE,
    OF_IGR_FT2_ACT_SF_TYPE2_VLAN_ID = OF_SET_FIELD_TYPE_VLAN_ID,
    OF_IGR_FT2_ACT_SF_TYPE2_VLAN_PRI = OF_SET_FIELD_TYPE_VLAN_PRI,
    OF_IGR_FT2_ACT_SF_TYPE2_IP_DSCP = OF_SET_FIELD_TYPE_IP_DSCP,
    OF_IGR_FT2_ACT_SF_TYPE2_END = OF_SET_FIELD_TYPE_END
} rtk_of_igrFT2ActSFType2_t;

typedef struct rtk_of_igrFT2ActSF2_e
{
    rtk_of_igrFT2ActSFType2_t   field_type;
    uint32					    field_data;/* OF_IGR_FT2_ACT_SF_TYPE2_VLAN_ID:  VLAN ID
                                            * OF_IGR_FT2_ACT_SF_TYPE2_VLAN_PRI: VLAN priority
                                            * OF_IGR_FT2_ACT_SF_TYPE2_IP_DSCP:  IPv4/IPv6 DSCP
                                            */
} rtk_of_igrFT2ActSF2_t;

typedef struct rtk_of_igrFT2ActOutput_s
{
    rtk_of_igrFT2ActOutputType_t    type;

    union
    {
        rtk_port_t                  port;      /* OF_OUTPUT_TYPE_PHY_PORT_EX_SRC_PORT: physical egress port
                                                * OF_OUTPUT_TYPE_PHY_PORT:             physical egress port
                                                */
        rtk_trunk_t                 trunk;     /* OF_OUTPUT_TYPE_TRK_PORT_EX_SRC_PORT: trunk ID
                                                * OF_OUTPUT_TYPE_TRK_PORT:             trunk ID
                                                */
        rtk_portmask_t              portmask;  /* OF_OUTPUT_TYPE_MULTI_EGR_PORT:       egress portmask
                                                * OF_OUTPUT_TYPE_FAILOVER:             egress portmask
                                                */
        uint32 				        intf;      /* OF_OUTPUT_TYPE_TUNNEL:               interface ID */
    };

    uint32 				            devID;     /* OF_OUTPUT_TYPE_PHY_PORT_EX_SRC_PORT: stacking device ID
                                                * OF_OUTPUT_TYPE_PHY_PORT:             stacking device ID
                                                */

} rtk_of_igrFT2ActOutput_t;

typedef struct rtk_of_igrFT2InsWriteAct_s
{
    rtk_enable_t 				push_vlan;
    rtk_of_flowActPushVlan_t	push_vlan_data;

    rtk_enable_t 				dec_ip_ttl;

    rtk_of_igrFT2ActSF0_t       set_field_0_data;
    rtk_of_igrFT2ActSF1_t       set_field_1_data;
    rtk_of_igrFT2ActSF2_t       set_field_2_data;

    rtk_enable_t 				set_queue;
    uint32						qid;

    rtk_enable_t 				group;
    uint32						gid;

    rtk_of_igrFT2ActOutput_t	output_data;
} rtk_of_igrFT2InsWriteAct_t;

typedef struct rtk_of_igrFT2Ins_s
{
    rtk_enable_t                clearAct_en;

    rtk_enable_t                writeAct_en;
    rtk_of_igrFT2InsWriteAct_t  wa_data;

    rtk_enable_t                writeMetadata_en;
    rtk_of_flowInsWriteMd_t     wm_data;

    rtk_enable_t                gotoTbl_en;
    rtk_of_flowInsGotoTbl_t     gt_data;
} rtk_of_igrFT2Ins_t;

typedef enum rtk_of_egrFTActSFType0_e
{
    OF_EGR_FT_SF_TYPE0_NONE = OF_SET_FIELD_TYPE_NONE,
    OF_EGR_FT_SF_TYPE0_VLAN_PRI = OF_SET_FIELD_TYPE_VLAN_PRI,
    OF_EGR_FT_SF_TYPE0_IP_DSCP = OF_SET_FIELD_TYPE_IP_DSCP,
    OF_EGR_FT_SF_TYPE0_END = OF_SET_FIELD_TYPE_END
} rtk_of_egrFTActSFType0_t;

typedef struct rtk_of_egrFTActSF0_e
{
    rtk_of_egrFTActSFType0_t    field_type;
    uint32					    field_data;/* OF_EGR_FT_SF_TYPE0_VLAN_PRI: VLAN Priority
                                            * OF_EGR_FT_SF_TYPE0_IP_DSCP:  IPv4/IPv6 DSCP
                                            */
} rtk_of_egrFTActSF0_t;

typedef enum rtk_of_egrFTActSFType1_e
{
    OF_EGR_FT_SF_TYPE1_NONE = OF_SET_FIELD_TYPE_NONE,
    OF_EGR_FT_SF_TYPE1_VLAN_ID = OF_SET_FIELD_TYPE_VLAN_ID,
    OF_EGR_FT_SF_TYPE1_END = OF_SET_FIELD_TYPE_END
} rtk_of_egrFTActSFType1_t;

typedef struct rtk_of_egrFTActSF1_e
{
    rtk_of_egrFTActSFType1_t    field_type;
    uint32                      field_data;/* OF_EGR_FT_SF_TYPE0_VLAN_PRI: VLAN ID */
} rtk_of_egrFTActSF1_t;

typedef struct rtk_of_egrFTInsWriteAct_s
{
    rtk_enable_t 				pop_vlan;

    rtk_enable_t 				push_vlan;
    rtk_of_flowActPushVlan_t    push_vlan_data;

    rtk_of_egrFTActSF0_t        set_field_0_data;
    rtk_of_egrFTActSF1_t	    set_field_1_data;

    rtk_enable_t 				drop;/* drop the packet instead forward it */
} rtk_of_egrFTInsWriteAct_t;

typedef struct rtk_of_egrFTIns_s
{
    rtk_enable_t                meter_en;
    rtk_of_flowInsMeter_t       meter_data;

    rtk_enable_t                writeAct_en;
    rtk_of_egrFTInsWriteAct_t   wa_data;
} rtk_of_egrFTIns_t;

typedef union rtk_of_flowIns_s
{
    rtk_of_igrFT0Ins_t  igrFt_0;
    rtk_of_igrFT3Ins_t  igrFt_3;
    rtk_of_egrFTIns_t   egrFt_0;
} rtk_of_flowIns_t;

typedef enum rtk_of_flowCntMode_e
{
    OF_FLOW_CNTMODE_PACKET_CNT_AND_BYTE_CNT = 0, /* packet counter and byte counter */
    OF_FLOW_CNTMODE_PACKET_CNT_AND_PACKET_CNT_TH,/* packet counter and packet counter trigger threshold */
    OF_FLOW_CNTMODE_BYTE_CNT_AND_BYTE_CNT_TH,    /* byte counter and byte counter trigger threshold */
    OF_FLOW_CNTMODE_DISABLE,
    OF_FLOW_CNTMODE_END
} rtk_of_flowCntMode_t;

typedef enum rtk_of_flowCntType_e
{
    OF_FLOW_CNT_TYPE_BYTE = 0,
    OF_FLOW_CNT_TYPE_PACKET,
    OF_FLOW_CNT_TYPE_END
} rtk_of_flowCntType_t;

typedef enum rtk_of_flowTblCntType_e
{
    OF_FLOW_TBL_CNT_TYPE_LOOKUP = 0,/* packet that lookup the flow table */
    OF_FLOW_TBL_CNT_TYPE_MATCH,     /* packet that match the flow table */
    OF_FLOW_TBL_CNT_TYPE_END
} rtk_of_flowTblCntType_t;

typedef enum rtk_of_l2FlowEntryType_e
{
    OF_L2_FLOW_ENTRY_TYPE_SA = 0,
    OF_L2_FLOW_ENTRY_TYPE_VID_SA,
    OF_L2_FLOW_ENTRY_TYPE_DA,
    OF_L2_FLOW_ENTRY_TYPE_VID_DA,
    OF_L2_FLOW_ENTRY_TYPE_SA_DA,
    OF_L2_FLOW_ENTRY_TYPE_5_TUPLE_IPPROTO,
    OF_L2_FLOW_ENTRY_TYPE_5_TUPLE_VID,
    OF_L2_FLOW_ENTRY_TYPE_END
} rtk_of_l2FlowEntryType_t;

typedef struct vidMac_s
{
    rtk_vlan_t vid;     /* used when type = OF_L2_FLOW_ENTRY_TYPE_VID_SA/OF_L2_FLOW_ENTRY_TYPE_VID_DA */
    rtk_mac_t  smac;    /* used when type = OF_L2_FLOW_ENTRY_TYPE_SA/OF_L2_FLOW_ENTRY_TYPE_VID_SA/OF_L2_FLOW_ENTRY_TYPE_SA_DA */
    rtk_mac_t  dmac;    /* used when type = OF_L2_FLOW_ENTRY_TYPE_DA/OF_L2_FLOW_ENTRY_TYPE_VID_DA/OF_L2_FLOW_ENTRY_TYPE_SA_DA */
    uint32     metadata;/* optional match field. set flags for metadata matching. */
} vidMac_t;

typedef struct fiveTuple_s
{
    struct
    {
        rtk_vlan_t vid;    /* used when type = OF_L2_FLOW_ENTRY_TYPE_5_TUPLE_VID */
        uint32     ipproto;/* used when type = OF_L2_FLOW_ENTRY_TYPE_5_TUPLE_IPPROTO */
    } fifthData;

    rtk_ip_addr_t  sip;
    rtk_ip_addr_t  dip;
    uint32         l4_sport;
    uint32         l4_dport;
} fiveTuple_t;

typedef struct rtk_of_l2FlowEntry_s
{
    rtk_of_l2FlowEntryType_t type;/* L2 flow entry type */
    struct
    {
        vidMac_t vidMac;
        fiveTuple_t fiveTp;
    } matchfield;

    uint32 flags;/* refer to RTK_OF_FLAG_FLOWENTRY_XXX */
    rtk_of_igrFT1Ins_t ins;

    uint32 ret_idx;/* OUTPUT. returned physical entry index when getting entry */
} rtk_of_l2FlowEntry_t;

/* L2 flow table match field for the 1st and 2nd lookup */
typedef enum rtk_of_l2FlowTblMatchField_e
{
    OF_L2_FT_MATCH_FIELD_VID_SA_SA = 0,/* (VID,SA) and (0,SA)   */
    OF_L2_FT_MATCH_FIELD_VID_SA_VID_DA,/* (VID,SA) and (VID,DA) */
    OF_L2_FT_MATCH_FIELD_VID_SA_DA,    /* (VID,SA) and (0,DA)   */
    OF_L2_FT_MATCH_FIELD_SA_VID_DA,    /* (0,SA) and (VID,DA)   */
    OF_L2_FT_MATCH_FIELD_SA_DA,        /* (0,SA) and (0,DA)     */
    OF_L2_FT_MATCH_FIELD_VID_DA_DA,    /* (VID,DA) and (0,DA)   */
    OF_L2_FT_MATCH_FIELD_DA_SA_VID_SA, /* (DA,SA) and (VID,SA)  */
    OF_L2_FT_MATCH_FIELD_DA_SA_SA,     /* (DA,SA) and (0,SA)    */
    OF_L2_FT_MATCH_FIELD_DA_SA_VID_DA, /* (DA,SA) and (VID,DA)  */
    OF_L2_FT_MATCH_FIELD_DA_SA_DA,     /* (DA,SA) and (0,DA)    */
    OF_L2_FT_MATCH_FIELD_FIVE_TUPLE,   /* (IP PROTOCOL,L4_DPORT,L4_SPORT,DIP,SIP) and (VID,L4_DPORT,L4_SPORT,DIP,SIP) */
    OF_L2_FT_MATCH_FIELD_END
} rtk_of_l2FlowTblMatchField_t;

typedef enum rtk_of_l3FlowTblList_e
{
    OF_L3_FLOW_TBL_LIST_CAM,
    OF_L3_FLOW_TBL_LIST_HASH,
    OF_L3_FLOW_TBL_LIST_END
} rtk_of_l3FlowTblList_t;

/* L3 CAM-based flow table match field for the 1st and 2nd lookup */
typedef enum rtk_of_l3CamFlowTblMatchField_e
{
    OF_L3_CAM_FT_MATCH_FIELD_MD_SIP_MD_DIP,    /* (Metadata,SIP) and (Metadata,DIP)     */
    OF_L3_CAM_FT_MATCH_FIELD_MD_SIP_DIP_MD_SIP,/* (Metadata,SIP,DIP) and (Metadata,SIP) */
    OF_L3_CAM_FT_MATCH_FIELD_MD_SIP_DIP_MD_DIP,/* (Metadata,SIP,DIP) and (Metadata,DIP) */
    OF_L3_CAM_FT_MATCH_FIELD_END
} rtk_of_l3CamFlowTblMatchField_t;

/* L3 Hash-based flow table match field for the 1st and 2nd lookup */
typedef enum rtk_of_l3HashFlowTblMatchField_e
{
    OF_L3_HASH_FT_MATCH_FIELD_SIP_DIP,    /* SIP and DIP       */
    OF_L3_HASH_FT_MATCH_FIELD_SIP_DIP_SIP,/* (SIP,DIP) and SIP */
    OF_L3_HASH_FT_MATCH_FIELD_SIP_DIP_DIP,/* (SIP,DIP) and DIP */
    OF_L3_HASH_FT_MATCH_FIELD_END
} rtk_of_l3HashFlowTblMatchField_t;

typedef enum rtk_of_l3FlowEntryType_e
{
    OF_L3_FLOW_ENTRY_TYPE_IP4_SIP = 0,
    OF_L3_FLOW_ENTRY_TYPE_IP4_DIP,
    OF_L3_FLOW_ENTRY_TYPE_IP4_SIP_DIP,
    OF_L3_FLOW_ENTRY_TYPE_IP6_SIP,
    OF_L3_FLOW_ENTRY_TYPE_IP6_DIP,
    OF_L3_FLOW_ENTRY_TYPE_IP6_SIP_DIP,
    OF_L3_FLOW_ENTRY_TYPE_END
} rtk_of_l3FlowEntryType_t;

typedef struct rtk_of_l3CamFlowEntry_s
{
    rtk_of_l3FlowEntryType_t    type; /* L3 flow entry type */

    union
    {
        rtk_ip_addr_t   ipv4;/* used when type = OF_L3_FLOW_ENTRY_TYPE_IP4_SIP/OF_L3_FLOW_ENTRY_TYPE_IP4_SIP_DIP */
        rtk_ipv6_addr_t ipv6;/* used when type = OF_L3_FLOW_ENTRY_TYPE_IP6_SIP/OF_L3_FLOW_ENTRY_TYPE_IP6_SIP_DIP */
    } sip;
    union
    {
        rtk_ip_addr_t   ipv4;/* used when type = OF_L3_FLOW_ENTRY_TYPE_IP4_SIP/OF_L3_FLOW_ENTRY_TYPE_IP4_SIP_DIP */
        rtk_ipv6_addr_t ipv6;/* used when type = OF_L3_FLOW_ENTRY_TYPE_IP6_SIP/OF_L3_FLOW_ENTRY_TYPE_IP6_SIP_DIP */
    } sip_msk;

    union
    {
        rtk_ip_addr_t   ipv4;/* used when type = OF_L3_FLOW_ENTRY_TYPE_IP4_DIP/OF_L3_FLOW_ENTRY_TYPE_IP4_SIP_DIP */
        rtk_ipv6_addr_t ipv6;/* used when type = OF_L3_FLOW_ENTRY_TYPE_IP6_DIP/OF_L3_FLOW_ENTRY_TYPE_IP6_SIP_DIP */
    } dip;
    union
    {
        rtk_ip_addr_t   ipv4;/* used when type = OF_L3_FLOW_ENTRY_TYPE_IP4_DIP/OF_L3_FLOW_ENTRY_TYPE_IP4_SIP_DIP */
        rtk_ipv6_addr_t ipv6;/* used when type = OF_L3_FLOW_ENTRY_TYPE_IP6_DIP/OF_L3_FLOW_ENTRY_TYPE_IP6_SIP_DIP */
    } dip_msk;

    uint32                      metadata;    /* used for all types */
    uint32                      metadata_msk;/* used for all types */
    uint32                      flags;       /* refer to RTK_OF_FLAG_XXX */

    rtk_of_igrFT2Ins_t          ins;
} rtk_of_l3CamFlowEntry_t;

typedef struct rtk_of_l3HashFlowEntry_s
{
    rtk_of_l3FlowEntryType_t    type;/* L3 flow entry type */

    union
    {
        rtk_ip_addr_t   ipv4;/* used when type = OF_L3_FLOW_ENTRY_TYPE_IP4_SIP/OF_L3_FLOW_ENTRY_TYPE_IP4_SIP_DIP */
        rtk_ipv6_addr_t ipv6;/* used when type = OF_L3_FLOW_ENTRY_TYPE_IP6_SIP/OF_L3_FLOW_ENTRY_TYPE_IP6_SIP_DIP */
    } sip;
    union
    {
        rtk_ip_addr_t   ipv4;/* used when type = OF_L3_FLOW_ENTRY_TYPE_IP4_DIP/OF_L3_FLOW_ENTRY_TYPE_IP4_SIP_DIP */
        rtk_ipv6_addr_t ipv6;/* used when type = OF_L3_FLOW_ENTRY_TYPE_IP6_DIP/OF_L3_FLOW_ENTRY_TYPE_IP6_SIP_DIP */
    } dip;
    uint32                      flags;   /* refer to RTK_OF_FLAG_XXX */
    uint32                      metadata;/* optional match field */

    rtk_of_igrFT2Ins_t          ins;

    uint32 ret_idx;/* OUTPUT. returned physical entry index when getting entry */
 } rtk_of_l3HashFlowEntry_t;

typedef enum rtk_of_groupType_e
{
    OF_GROUP_TYPE_ALL,
    OF_GROUP_TYPE_SELECT,
    OF_GROUP_TYPE_END
} rtk_of_groupType_t;

typedef struct rtk_of_groupEntry_s
{
    rtk_of_groupType_t  type;
    uint32              bucket_num;/* the number of consecutive action buckets */
    uint32              bucket_id; /* index to the first bucket of consecutive action buckets */
} rtk_of_groupEntry_t;

typedef struct rtk_of_groupTblHashPara_s
{
    rtk_enable_t    igr_port;
    rtk_enable_t    smac;
    rtk_enable_t    dmac;
    rtk_enable_t    sip;
    rtk_enable_t    dip;
    rtk_enable_t    l4_sport;
    rtk_enable_t    l4_dport;
} rtk_of_groupTblHashPara_t;

typedef struct rtk_of_actionBucket_s
{
    rtk_enable_t 				    cp_ttl_inward;

    rtk_enable_t 				    pop_vlan;

    rtk_enable_t 				    pop_mpls;
    rtk_of_actBucketPopMplsType_t   pop_mpls_type;

    rtk_enable_t 				    push_mpls;
    rtk_of_actBucketPushMpls_t	    push_mpls_data;

    rtk_enable_t 				    push_vlan;
    rtk_of_actBucketPushVlan_t	    push_vlan_data;

    rtk_enable_t 				    cp_ttl_outward;

    rtk_enable_t 				    dec_mpls_ttl;

    rtk_enable_t 				    dec_ip_ttl;

    rtk_of_actBucketSF0_t	        set_field_0_data;
    rtk_of_actBucketSF1_t	        set_field_1_data;
    rtk_of_actBucketSF2_t	        set_field_2_data;

    rtk_enable_t 				    set_queue;
    uint32						    qid;

    rtk_of_actBucketOutput_t	    output_data;
} rtk_of_actionBucket_t;

typedef enum rtk_of_tblMissAct_e
{
    OF_TBLMISS_ACT_DROP,
    OF_TBLMISS_ACT_TRAP,
    OF_TBLMISS_ACT_FORWARD_NEXT_TBL,/* forward to next table */
    OF_TBLMISS_ACT_EXEC_ACTION_SET, /* execute current action set */
    OF_TBLMISS_ACT_END
} rtk_of_tblMissAct_t;


/*
 * Macro Declaration
 */

/*
 * Function Declaration
 */

/* Module Name     : OpenFlow */

/* Function Name:
 *      rtk_of_init
 * Description:
 *      Initialize OpenFlow module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 * Applicable:
 *      9310
 * Note:
 *      Must initialize OpenFlow module before calling any OpenFlow APIs.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_of_init(uint32 unit);

/* Function Name:
 *      rtk_of_classifier_get
 * Description:
 *      Get OpenFlow classification mechanism.
 * Input:
 *      unit	- unit id
 *      type	- classifier type
 * Output:
 *      pData 	- classifier data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  	- invalid unit id
 *      RT_ERR_NOT_INIT 	- The module is not initial
 *      RT_ERR_INPUT    	- invalid input parameter
 *      RT_ERR_VLAN_VID     - invalid vlan id
 *      RT_ERR_NULL_POINTER	- input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      OF_CLASSIFIER_TYPE_PORT: Packet from OpenFlow enabled port is sent to OpenFlow Pipeline
 *      OF_CLASSIFIER_TYPE_VLAN: Packet from OpenFlow enabled VLAN is sent to OpenFlow Pipeline
 *      OF_CLASSIFIER_TYPE_VLAN_AND_PORT: Send packet to OpenFlow Pipeline only if packet's corresponding
 *		OF_PORT_EN and OF_VLAN_EN are both enabled
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_of_classifier_get(uint32 unit, rtk_of_classifierType_t type, rtk_of_classifierData_t *pData);

/* Function Name:
 *      rtk_of_classifier_set
 * Description:
 *      Set OpenFlow classification mechanism.
 * Input:
 *      unit	- unit id
 *      type	- classifier type
 *      data 	- classifier data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_VLAN_VID     - invalid vlan id
 * Applicable:
 *      9310
 * Note:
 *      OF_CLASSIFIER_TYPE_PORT: Packet from OpenFlow enabled port is sent to OpenFlow Pipeline
 *      OF_CLASSIFIER_TYPE_VLAN: Packet from OpenFlow enabled VLAN is sent to OpenFlow Pipeline
 *      OF_CLASSIFIER_TYPE_VLAN_AND_PORT: Send packet to OpenFlow Pipeline only if packet's corresponding
 *		OF_PORT_EN and OF_VLAN_EN are both enabled
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_of_classifier_set(uint32 unit, rtk_of_classifierType_t type, rtk_of_classifierData_t data);

/* Function Name:
 *      rtk_of_flowMatchFieldSize_get
 * Description:
 *      Get the match field size of flow entry.
 * Input:
 *      unit        - unit id
 *      type        - type of entry field
 * Output:
 *      pField_size - match field size
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_UNIT_ID          - invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_OF_FIELD_TYPE	- invalid match field type
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      The unit of size is bit.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_of_flowMatchFieldSize_get(uint32 unit, rtk_of_matchfieldType_t type, uint32 *pField_size);

/* Function Name:
 *      rtk_of_flowEntrySize_get
 * Description:
 *      Get the flow entry size
 * Input:
 *      unit        - unit id
 *      phase       - Flow Table phase
 * Output:
 *      pEntry_size - flow entry size
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OF_FT_PHASE	- invalid Flow Table phase
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      (1) The API isn't applicable to L2 and L3 flow tables.
 *      (2) The unit of size is byte.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_of_flowEntrySize_get(uint32 unit, rtk_of_flowtable_phase_t phase, uint32 *pEntry_size);

/* Function Name:
 *      rtk_of_flowEntryValidate_get
 * Description:
 *      Validate flow entry without modifying the content
 * Input:
 *      unit      - unit id
 *      phase     - Flow Table phase
 *      entry_idx - entry index
 * Output:
 *      pValid    - pointer buffer of valid state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OF_FT_PHASE	- invalid Flow Table phase
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      The API isn't applicable to L2 and L3 flow tables.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_of_flowEntryValidate_get(
    uint32              		unit,
    rtk_of_flowtable_phase_t	phase,
    rtk_of_flow_id_t        	entry_idx,
    uint32              		*pValid);

/* Function Name:
 *      rtk_of_flowEntryValidate_set
 * Description:
 *      Validate flow entry without modifying the content
 * Input:
 *      unit      - unit id
 *      phase     - Flow Table phase
 *      entry_idx - entry index
 *      valid     - valid state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID     	- invalid unit id
 *      RT_ERR_NOT_INIT    	- The module is not initial
 *      RT_ERR_OF_FT_PHASE	- invalid Flow Table phase
 *      RT_ERR_ENTRY_INDEX 	- invalid entry index
 * Applicable:
 *      9310
 * Note:
 *      The API isn't applicable to L2 and L3 flow tables.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_of_flowEntryValidate_set(
    uint32              		unit,
    rtk_of_flowtable_phase_t	phase,
    rtk_of_flow_id_t        	entry_idx,
    uint32              		valid);

/* Function Name:
 *      rtk_of_flowEntryFieldList_get
 * Description:
 *      Get supported match field list of the specified phase.
 * Input:
 *      unit  - unit id
 *      phase - Flow Table phase
 *      type  - match field type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OF_FT_PHASE  - invalid Flow Table phase
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_of_flowEntryFieldList_get(
    uint32                   unit,
    rtk_of_flowtable_phase_t phase,
    rtk_of_matchfieldList_t  *list);

/* Function Name:
 *      rtk_of_flowEntryField_check
 * Description:
 *      Check whether the match field type is supported on the specified phase.
 * Input:
 *      unit  - unit id
 *      phase - Flow Table phase
 *      type  - match field type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_OF_FT_PHASE    - invalid Flow Table phase
 *      RT_ERR_OF_FIELD_TYPE  - invalid match field type
 * Applicable:
 *      9310
 * Note:
 *      The API isn't applicable to L2 and L3 flow tables.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_of_flowEntryField_check(
    uint32                   unit,
    rtk_of_flowtable_phase_t phase,
    rtk_of_matchfieldType_t  type);

/* Function Name:
 *      rtk_of_flowEntrySetField_check
 * Description:
 *      Check whether the set-field type is supported on the specified phase and field ID.
 * Input:
 *      unit     - unit id
 *      phase    - Flow Table phase
 *      field_id - set field ID
 *      type     - set field type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID              - invalid unit id
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_OF_FT_PHASE          - invalid Flow Table phase
 *      RT_ERR_OF_SET_FIELD_ID      - invalid set field ID
 *      RT_ERR_OF_SET_FIELD_TYPE    - invalid set field type
 * Applicable:
 *      9310
 * Note:
 *      The API isn't applicable to L2 and L3 flow tables.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_of_flowEntrySetField_check(
    uint32                   unit,
    rtk_of_flowtable_phase_t phase,
    uint32                   field_id,
    rtk_of_setFieldType_t    type);

/* Function Name:
 *      rtk_of_flowEntryField_read
 * Description:
 *      Read match field data from specified flow entry.
 * Input:
 *      unit      - unit id
 *      phase     - Flow Table phase
 *      entry_idx - flow entry index
 *      type      - match field type
 * Output:
 *      pData     - pointer buffer of field data
 *      pMask     - pointer buffer of field mask
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID			- invalid unit id
 *      RT_ERR_NOT_INIT			- The module is not initial
 *      RT_ERR_OF_FT_PHASE		- invalid Flow Table phase
 *      RT_ERR_ENTRY_INDEX		- invalid entry index
 *      RT_ERR_OF_FIELD_TYPE	- invalid match field type
 *      RT_ERR_NULL_POINTER		- input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      The API isn't applicable to L2 and L3 flow tables.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_of_flowEntryField_read(
    uint32              		unit,
    rtk_of_flowtable_phase_t	phase,
    rtk_of_flow_id_t			entry_idx,
    rtk_of_matchfieldType_t		type,
    uint8               		*pData,
    uint8               		*pMask);

/* Function Name:
 *      rtk_of_flowEntryField_write
 * Description:
 *      Write match field data to specified flow entry.
 * Input:
 *      unit      - unit id
 *      phase     - Flow Table phase
 *      entry_idx - flow entry index
 *      type      - match field type
 *      pData     - pointer buffer of field data
 *      pMask     - pointer buffer of field mask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID			- invalid unit id
 *      RT_ERR_NOT_INIT			- The module is not initial
 *      RT_ERR_OF_FT_PHASE      - invalid Flow Table phase
 *      RT_ERR_ENTRY_INDEX		- invalid entry index
 *      RT_ERR_OF_FIELD_TYPE	- invalid match field type
 *      RT_ERR_NULL_POINTER		- input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      The API isn't applicable to L2 and L3 flow tables.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_of_flowEntryField_write(
    uint32              		unit,
    rtk_of_flowtable_phase_t	phase,
    rtk_of_flow_id_t			entry_idx,
    rtk_of_matchfieldType_t		type,
    uint8               		*pData,
    uint8               		*pMask);

/* Function Name:
 *      rtk_of_flowEntryOperation_get
 * Description:
 *      Get flow entry operation.
 * Input:
 *      unit       - unit id
 *      phase      - Flow Table phase
 *      entry_idx  - flow entry index
 * Output:
 *      pOperation - operation configuration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OF_FT_PHASE  - invalid Flow Table phase
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      (1) The API isn't applicable to L2 and L3 flow tables.
 *      (2) For reverse operation, valid index is N where N = 0,1,(2) ..
 *      (3) For aggr_1 operation, index must be 2N where N = 0,1,(2) ..
 *      (4) For aggr_2 operation, index must be 2N+256M where N,M = 0,1,(2) ..
 *      (5) For aggregating 4 entries, both aggr_1 and aggr_2 must be enabled.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_of_flowEntryOperation_get(
    uint32                  	unit,
    rtk_of_flowtable_phase_t	phase,
    rtk_of_flow_id_t            entry_idx,
    rtk_of_flowOperation_t     	*pOperation);

/* Function Name:
 *      rtk_of_flowEntryOperation_set
 * Description:
 *      Set flow entry operation.
 * Input:
 *      unit       - unit id
 *      phase      - Flow Table phase
 *      entry_idx  - flow entry index
 *      pOperation - operation configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OF_FT_PHASE  - invalid Flow Table phase
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Applicable:
 *      9310
 * Note:
 *      (1) The API isn't applicable to L2 and L3 flow tables.
 *      (2) For reverse operation, valid index is N where N = 0,1,(2) ..
 *      (3) For aggr_1 operation, index must be 2N where N = 0,1,(2) ..
 *      (4) For aggr_2 operation, index must be 2N+256M where N,M = 0,1,(2) ..
 *      (5) For aggregating 4 entries, both aggr_1 and aggr_2 must be enabled.
 *      (6) Egress flow table 0 doesn't support aggr_1 operation.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_of_flowEntryOperation_set(
    uint32                  	unit,
    rtk_of_flowtable_phase_t	phase,
    rtk_of_flow_id_t            entry_idx,
    rtk_of_flowOperation_t     	*pOperation);

/* Function Name:
 *      rtk_of_flowEntryInstruction_get
 * Description:
 *      Get the flow entry instruction configuration.
 * Input:
 *      unit      - unit id
 *      phase     - Flow Table phase
 *      entry_idx - flow entry index
 * Output:
 *      pAction   - instruction mask and data configuration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OF_FT_PHASE  - invalid Flow Table phase
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      The API isn't applicable to L2 and L3 flow tables.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_of_flowEntryInstruction_get(
    uint32               		unit,
    rtk_of_flowtable_phase_t	phase,
    rtk_of_flow_id_t         	entry_idx,
    rtk_of_flowIns_t	        *pData);

/* Function Name:
 *      rtk_of_flowEntryInstruction_set
 * Description:
 *      Set the flow entry instruction configuration.
 * Input:
 *      unit      - unit id
 *      phase     - Flow Table phase
 *      entry_idx - flow entry index
 *      pData     - instruction mask and data configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OF_FT_PHASE  - invalid Flow Table phase
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Applicable:
 *      9310
 * Note:
 *      The API isn't applicable to L2 and L3 flow tables.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_of_flowEntryInstruction_set(
    uint32               		unit,
    rtk_of_flowtable_phase_t	phase,
    rtk_of_flow_id_t         	entry_idx,
    rtk_of_flowIns_t	        *pData);

/* Function Name:
 *      rtk_of_flowEntryHitSts_get
 * Description:
 *      Get the flow entry hit status.
 * Input:
 *      unit        - unit id
 *      phase       - Flow Table phase
 *      entry_idx   - flow entry index
 *      reset       - reset the hit status
 * Output:
 *      pIsHit      - pointer to hit status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OF_FT_PHASE  - invalid Flow Table phase
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      (1) The API isn't applicable to L2 and L3 flow tables.
 *      (2) The hit status can be cleared by configuring parameter 'reset' to (1)
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_of_flowEntryHitSts_get(
    uint32                   unit,
    rtk_of_flowtable_phase_t phase,
    rtk_of_flow_id_t         entry_idx,
    uint32                   reset,
    uint32                   *pIsHit);

/* Function Name:
 *      rtk_of_flowEntry_del
 * Description:
 *      Delete the specified flow entries.
 * Input:
 *      unit    - unit id
 *      phase   - Flow Table phase
 *      pClrIdx - entry index to clear
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID         - invalid unit id
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_OF_FT_PHASE     - invalid Flow Table phase
 *      RT_ERR_INPUT           - invalid input parameter
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 *      RT_ERR_OF_CLEAR_INDEX  - end index is lower than start index
 *      RT_ERR_ENTRY_INDEX     - invalid entry index
 * Applicable:
 *      9310
 * Note:
 *      (1) The API isn't applicable to L2 and L3 flow tables.
 *      (2) Match fields, Operations, Priority and Instructions are all cleared.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_of_flowEntry_del(uint32 unit, rtk_of_flowtable_phase_t phase, rtk_of_flowClear_t *pClrIdx);

/* Function Name:
 *      rtk_of_flowEntry_move
 * Description:
 *      Move the specified flow entries.
 * Input:
 *      unit  - unit id
 *      phase - Flow Table phase
 *      pData - movement info
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OF_FT_PHASE  - invalid Flow Table phase
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 * Applicable:
 *      9310
 * Note:
 *      (1) The API isn't applicable to L2 and L3 flow tables.
 *      (2) Match fields, Operations, Priority and Instructions are all moved.
 *      (3) The vacant entries due to movement are auto cleared to be invalid by H/W.
 *      (4) (move_from + length) and (move_to + length) must less than or equal to the number of flow entries for the specified phase.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_of_flowEntry_move(uint32 unit, rtk_of_flowtable_phase_t phase, rtk_of_flowMove_t *pData);

/* Function Name:
 *      rtk_of_ftTemplateSelector_get
 * Description:
 *      Get the mapping template of specific Flow Table block.
 * Input:
 *      unit          - unit id
 *      phase         - Flow Table phase
 *      block_idx     - block index
 * Output:
 *      pTemplate_idx - template index
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_OF_FT_PHASE      - invalid Flow Table phase
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_OF_BLOCK_PHASE   - The block is not belonged to Flow Table
 * Applicable:
 *      9310
 * Note:
 *      The API isn't applicable to L2 and L3 flow tables.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_of_ftTemplateSelector_get(
    uint32                   unit,
    rtk_of_flowtable_phase_t phase,
    uint32                   block_idx,
    rtk_of_ftTemplateIdx_t   *pTemplate_idx);

/* Function Name:
 *      rtk_of_ftTemplateSelector_set
 * Description:
 *      Set the mapping template of specific Flow Table block.
 * Input:
 *      unit         - unit id
 *      phase        - Flow Table phase
 *      block_idx    - block index
 *      template_idx - template index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID              - invalid unit id
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_OF_FT_PHASE          - invalid Flow Table phase
 *      RT_ERR_PIE_TEMPLATE_INDEX   - invalid template index
 *      RT_ERR_INPUT                - invalid input parameter
 *      RT_ERR_OF_BLOCK_PHASE       - The block is not belonged to Flow Table
 * Applicable:
 *      9310
 * Note:
 *      The API isn't applicable to L2 and L3 flow tables.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_of_ftTemplateSelector_set(
    uint32                   unit,
    rtk_of_flowtable_phase_t phase,
    uint32                   block_idx,
    rtk_of_ftTemplateIdx_t   template_idx);

/* Function Name:
 *      rtk_of_flowCntMode_get
 * Description:
 *      Get counter mode for specific flow counters.
 * Input:
 *      unit      - unit id
 *      phase     - Flow Table phase
 *      entry_idx - entry index
 * Output:
 *      pMode     - counter mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OF_FT_PHASE	- invalid Flow Table phase
 *      RT_ERR_ENTRY_INDEX 	- invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      (1) The API isn't applicable to L2 and L3 flow tables.
 *      (2) Each flow entry has a counter mode configuration, a 36bit counter and a 42bit counter.
 *          In different counter mode, these two counters have different meaning as below:
 *          OF_FLOW_CNTMODE_PACKET_CNT_AND_BYTE_CNT: 36bit packet counter and 42bit byte counter
 *          OF_FLOW_CNTMODE_PACKET_CNT_AND_PACKET_CNT_TH: 36bit packet counter and 42bit packet counter trigger threshold
 *          OF_FLOW_CNTMODE_BYTE_CNT_AND_BYTE_CNT_TH: 42bit byte counter and 36bit byte counter trigger threshold
 *      (3) If mode is configured to OF_FLOW_CNTMODE_PACKET_CNT_AND_PACKET_CNT_TH/OF_FLOW_CNTMODE_BYTE_CNT_AND_BYTE_CNT_TH,
 *          an interrupt is triggered when the packet/byte counter exceeds the packet/byte counter threshold for the first time.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_of_flowCntMode_get(
    uint32                   unit,
    rtk_of_flowtable_phase_t phase,
    rtk_of_flow_id_t         entry_idx,
    rtk_of_flowCntMode_t     *pMode);

/* Function Name:
 *      rtk_of_flowCntMode_set
 * Description:
 *      Set counter mode for specific flow counter.
 * Input:
 *      unit      - unit id
 *      phase     - Flow Table phase
 *      entry_idx - entry index
 *      mode      - counter mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OF_FT_PHASE	- invalid Flow Table phase
 *      RT_ERR_ENTRY_INDEX 	- invalid entry index
 *      RT_ERR_INPUT        - invalid input parameter
 * Applicable:
 *      9310
 * Note:
 *      (1) The API isn't applicable to L2 and L3 flow tables.
 *      (2) Each flow entry has a counter mode configuration, a 36bit counter and a 42bit counter.
 *          In different counter mode, these two counters have different meaning as below:
 *          OF_FLOW_CNTMODE_PACKET_CNT_AND_BYTE_CNT: 36bit packet counter and 42bit byte counter
 *          OF_FLOW_CNTMODE_PACKET_CNT_AND_PACKET_CNT_TH: 36bit packet counter and 42bit packet counter trigger threshold
 *          OF_FLOW_CNTMODE_BYTE_CNT_AND_BYTE_CNT_TH: 42bit byte counter and 36bit byte counter trigger threshold
 *      (3) If mode is configured to OF_FLOW_CNTMODE_PACKET_CNT_AND_PACKET_CNT_TH/OF_FLOW_CNTMODE_BYTE_CNT_AND_BYTE_CNT_TH,
 *          an interrupt is triggered when the packet/byte counter exceeds the packet/byte counter threshold for the first time.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_of_flowCntMode_set(
    uint32                   unit,
    rtk_of_flowtable_phase_t phase,
    rtk_of_flow_id_t         entry_idx,
    rtk_of_flowCntMode_t     mode);

/* Function Name:
 *      rtk_of_flowCnt_get
 * Description:
 *      Get packet counter or byte counter of specific flow counter.
 * Input:
 *      unit      - unit id
 *      phase     - Flow Table phase
 *      entry_idx - entry index
 *      type      - counter type
 * Output:
 *      pCnt      - pointer buffer of counter
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OF_FT_PHASE	- invalid Flow Table phase
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      (1) The API isn't applicable to L2 and L3 flow tables.
 *      (2) When the counter mode of specified counter is OF_FLOW_CNTMODE_PACKET_CNT_AND_PACKET_CNT_TH,
 *          parameter type can only be OF_FLOW_CNT_TYPE_PACKET.
 *      (3) When the counter mode of specified counter is OF_FLOW_CNTMODE_BYTE_CNT_AND_BYTE_CNT_TH,
 *          parameter type can only be OF_FLOW_CNT_TYPE_BYTE.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_of_flowCnt_get(
    uint32                   unit,
    rtk_of_flowtable_phase_t phase,
    rtk_of_flow_id_t         entry_idx,
    rtk_of_flowCntType_t     type,
    uint64                   *pCnt);

/* Function Name:
 *      rtk_of_flowCnt_clear
 * Description:
 *      Clear counter of specific flow counter.
 * Input:
 *      unit      - unit id
 *      phase     - Flow Table phase
 *      entry_idx - entry index
 *      type      - counter type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID     - invalid unit id
 *      RT_ERR_NOT_INIT    - The module is not initial
 *      RT_ERR_OF_FT_PHASE - invalid Flow Table phase
 *      RT_ERR_ENTRY_INDEX - invalid entry index
 *      RT_ERR_INPUT       - invalid input parameter
 * Applicable:
 *      9310
 * Note:
 *      (1) The API isn't applicable to L2 and L3 flow tables.
 *      (2) When the counter mode of specified counter is OF_FLOW_CNTMODE_PACKET_CNT_AND_PACKET_CNT_TH,
 *          parameter type can only be OF_FLOW_CNT_TYPE_PACKET.
 *      (3) When the counter mode of specified counter is OF_FLOW_CNTMODE_BYTE_CNT_AND_BYTE_CNT_TH,
 *          parameter type can only be OF_FLOW_CNT_TYPE_BYTE.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_of_flowCnt_clear(
    uint32                   unit,
    rtk_of_flowtable_phase_t phase,
    rtk_of_flow_id_t         entry_idx,
    rtk_of_flowCntType_t     type);

/* Function Name:
 *      rtk_of_flowCntThresh_get
 * Description:
 *      Get packet counter or byte counter interrupt trigger threshold of specific flow counter.
 * Input:
 *      unit       - unit id
 *      phase      - Flow Table phase
 *      entry_idx  - entry index
 * Output:
 *      pThreshold - pointer buffer of interrupt trigger threshold
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OF_FT_PHASE  - invalid Flow Table phase
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      (1) The API isn't applicable to L2 and L3 flow tables.
 *      (2) When the counter mode of specified counter is OF_FLOW_CNTMODE_PACKET_CNT_AND_PACKET_CNT_TH,
 *          threshold is the packet counter threshold.
 *      (3) When the counter mode of specified counter is OF_FLOW_CNTMODE_BYTE_CNT_AND_BYTE_CNT_TH,
 *          threshold is the byte counter threshold.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_of_flowCntThresh_get(
    uint32                   unit,
    rtk_of_flowtable_phase_t phase,
    rtk_of_flow_id_t         entry_idx,
    uint64                   *pThreshold);

/* Function Name:
 *      rtk_of_flowCntThresh_set
 * Description:
 *      Set packet counter or byte counter interrupt trigger threshold of specific flow counter.
 * Input:
 *      unit       - unit id
 *      phase      - Flow Table phase
 *      entry_idx  - entry index
 *      threshold  - interrupt trigger threshold
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OF_FT_PHASE  - invalid Flow Table phase
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Applicable:
 *      9310
 * Note:
 *      (1) The API isn't applicable to L2 and L3 flow tables.
 *      (2) When the counter mode of specified counter is OF_FLOW_CNTMODE_PACKET_CNT_AND_PACKET_CNT_TH,
 *          threshold is the packet counter threshold.
 *      (3) When the counter mode of specified counter is OF_FLOW_CNTMODE_BYTE_CNT_AND_BYTE_CNT_TH,
 *          threshold is the byte counter threshold.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_of_flowCntThresh_set(
    uint32                   unit,
    rtk_of_flowtable_phase_t phase,
    rtk_of_flow_id_t         entry_idx,
    uint64                   threshold);

/* Function Name:
 *      rtk_of_ttlExcpt_get
 * Description:
 *      Get action for invalid IP/MPLS TTL.
 * Input:
 *      unit    - unit id
 * Output:
 *      pAction - pointer buffer of action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      (1) Invalid TTL checking is executed when applying a decrement TTL action to the packet.
 *      (2) Valid action is ACTION_DROP/ACTION_TRAP.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_of_ttlExcpt_get(uint32 unit, rtk_action_t *pAction);

/* Function Name:
 *      rtk_of_ttlExcpt_set
 * Description:
 *      Set action for invalid IP/MPLS TTL.
 * Input:
 *      unit   - unit id
 *      action - action to take
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 * Applicable:
 *      9310
 * Note:
 *      (1) Invalid TTL checking is executed when applying a decrement TTL action to the packet.
 *      (2) Valid action is ACTION_DROP/ACTION_TRAP.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_of_ttlExcpt_set(uint32 unit, rtk_action_t action);

/* Function Name:
 *      rtk_of_maxLoopback_get
 * Description:
 *      Get maximum loopback times of openflow pipeline.
 * Input:
 *      unit   - unit id
 * Output:
 *      pTimes - pointer buffer of loopback times
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      Valid times ranges from 0~63. 0 and 63 means no limitation for loopback times.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_of_maxLoopback_get(uint32 unit, uint32 *pTimes);

/* Function Name:
 *      rtk_of_maxLoopback_set
 * Description:
 *      Set maximum loopback times of openflow pipeline.
 * Input:
 *      unit   - unit id
 *      times  - loopback times
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 * Applicable:
 *      9310
 * Note:
 *      Valid times ranges from 0~63. 0 and 63 means no limitation for loopback times.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_of_maxLoopback_set(uint32 unit, uint32 times);

/* Function Name:
 *      rtk_of_l2FlowTblMatchField_get
 * Description:
 *      Get match field of L2 flow table.
 * Input:
 *      unit    - unit id
 * Output:
 *      pField  - pointer buffer of match field
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      (1) L2 flow table can lookup twice for a certain packet using different match field. The API is used to select
 *          the match field for the 1st and 2nd lookup.
 *      (2) If both lookups are hit, the hit flow entry of 1st lookup is taken.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_of_l2FlowTblMatchField_get(uint32 unit, rtk_of_l2FlowTblMatchField_t *pField);

/* Function Name:
 *      rtk_of_l2FlowTblMatchField_set
 * Description:
 *      Set match field of L2 flow table.
 * Input:
 *      unit    - unit id
 *      field   - match filed
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 * Applicable:
 *      9310
 * Note:
 *      (1) L2 flow table can lookup twice for a packet using different match field. The API is used to select
 *          the match field for the 1st and 2nd lookup.
 *      (2) If both lookups are hit, the hit flow entry of 1st lookup is taken.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_of_l2FlowTblMatchField_set(uint32 unit, rtk_of_l2FlowTblMatchField_t field);

/* Function Name:
 *      rtk_of_l2FlowEntrySetField_check
 * Description:
 *      Check whether the set-field type is supported on the specified field ID.
 * Input:
 *      unit     - unit id
 *      field_id - set field ID
 *      type     - set field type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID              - invalid unit id
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_OF_FT_PHASE          - invalid Flow Table phase
 *      RT_ERR_OF_SET_FIELD_ID      - invalid set field ID
 *      RT_ERR_OF_SET_FIELD_TYPE    - invalid set field type
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_of_l2FlowEntrySetField_check(
    uint32                   unit,
    uint32                   field_id,
    rtk_of_setFieldType_t    type);

/* Function Name:
 *      rtk_of_l2FlowEntry_get
 * Description:
 *      Get L2 flow entry.
 * Input:
 *      unit    - unit id
 *      pEntry  - pointer buffer of entry data
 * Output:
 *      pEntry  - pointer buffer of entry data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_VLAN_VID         - invalid vlan id
 *      RT_ERR_ENTRY_NOTFOUND   - specified entry not found
 * Applicable:
 *      9310
 * Note:
 *      (1) To match metadata, set RTK_OF_FLAG_FLOWENTRY_MD_CMP in entry.flags.
 *      (2) The metadata in L2 flow entry is 6 bit width.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_of_l2FlowEntry_get(uint32 unit, rtk_of_l2FlowEntry_t *pEntry);

/* Function Name:
 *      rtk_of_l2FlowEntryNextValid_get
 * Description:
 *      Get next valid L2 flow entry from the specified device.
 * Input:
 *      unit         - unit id
 *      pScan_idx    - currently scan index of l2 flow table to get next.
 * Output:
 *      pEntry       - pointer buffer of entry data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID           - invalid unit id
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 *      RT_ERR_INPUT             - invalid input parameter
 *      RT_ERR_OUT_OF_RANGE      - input parameter out of range
 *      RT_ERR_ENTRY_NOTFOUND    - no next valid entry found
 * Applicable:
 *      9310
 * Note:
 *      (1) Input -1 for getting the first valid entry of l2 flow table.
 *      (2) The pScan_idx is both the input and output argument.
 *      (3) The pScan_idx must be multiple of 2.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_of_l2FlowEntryNextValid_get(
    uint32               unit,
    int32                *pScan_idx,
    rtk_of_l2FlowEntry_t *pEntry);

/* Function Name:
 *      rtk_of_l2FlowEntry_add
 * Description:
 *      Add a L2 flow entry.
 * Input:
 *      unit    - unit id
 *      pEntry  - pointer buffer of entry data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Applicable:
 *      9310
 * Note:
 *      (1) To match metadata, set RTK_OF_FLAG_FLOWENTRY_MD_CMP in entry.flags.
 *      (2) To overwrite existed entry, set RTK_OF_FLAG_FLOWENTRY_REPLACE in entry.flags.
 *      (3) The metadata in L2 flow entry is 6 bit width.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_of_l2FlowEntry_add(uint32 unit, rtk_of_l2FlowEntry_t *pEntry);

/* Function Name:
 *      rtk_of_l2FlowEntry_del
 * Description:
 *      Delete a L2 flow entry.
 * Input:
 *      unit    - unit id
 *      pEntry  - pointer buffer of entry data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_VLAN_VID         - invalid vlan id
 *      RT_ERR_ENTRY_NOTFOUND   - specified entry not found
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_of_l2FlowEntry_del(uint32 unit, rtk_of_l2FlowEntry_t *pEntry);

/* Function Name:
 *      rtk_of_l2FlowEntry_delAll
 * Description:
 *      Delete all L2 flow entry.
 * Input:
 *      unit    - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_of_l2FlowEntry_delAll(uint32 unit);

/* Function Name:
 *      rtk_of_l2FlowEntryHitSts_get
 * Description:
 *      Get the L2 flow entry hit status.
 * Input:
 *      unit        - unit id
 *      pEntry      - pointer buffer of entry data
 *      reset       - reset the hit status
 * Output:
 *      pIsHit      - pointer to hit status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_VLAN_VID         - invalid vlan id
 *      RT_ERR_ENTRY_NOTFOUND   - specified entry not found
 * Applicable:
 *      9310
 * Note:
 *      The hit status can be cleared by configuring parameter 'reset' to 1.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
rtk_of_l2FlowEntryHitSts_get(
    uint32                   unit,
    rtk_of_l2FlowEntry_t     *pEntry,
    uint32                   reset,
    uint32                   *pIsHit);

/* Function Name:
 *      rtk_of_l2FlowTblHashAlgo_get
 * Description:
 *      Get hash algorithm of L2 flow table.
 * Input:
 *      unit    - unit id
 *      block   - memory block id
 * Output:
 *      pAlgo   - pointer to hash algorithm
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      (1) L2 flow table is composed of two hash-based memeory blocks and each memory block can specify a hash algorithm.
 *          To reduce the hash collision ratio, two memory blocks should specify different algorithm.
 *      (2) There are two hash algorithm supported in 9310.
 *      (3) Each memeory block can lookup twice for a packet using different match field, refer to rtk_of_l2FlowTblMatchField_set.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_of_l2FlowTblHashAlgo_get(uint32 unit, uint32 block, uint32 *pAlgo);

/* Function Name:
 *      rtk_of_l2FlowTblHashAlgo_set
 * Description:
 *      Set hash algorithm of L2 flow table.
 * Input:
 *      unit    - unit id
 *      block   - memory block id
 *      algo    - hash algorithm
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 * Applicable:
 *      9310
 * Note:
 *      (1) L2 flow table is composed of two hash-based memeory blocks and each memory block can specify a hash algorithm.
 *          To reduce the hash collision ratio, two memory blocks should specify different algorithm.
 *      (2) There are two hash algorithm supported in 9310.
 *      (3) Each memeory block can lookup twice for a packet using different match field, refer to rtk_of_l2FlowTblMatchField_set.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_of_l2FlowTblHashAlgo_set(uint32 unit, uint32 block, uint32 algo);

/* Function Name:
 *      rtk_of_l3FlowTblPri_get
 * Description:
 *      Get precedence of L3 CAM-based and hash-based flow tables
 * Input:
 *      unit    - unit id
 * Output:
 *      pTable  - pointer to target table
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      (1) L3 flow table is composed of one CAM-based and two hash-based flow tables.
 *      (2) L3 CAM-based/Hash-based flow tables are physically combo with L3 Prefix/L3 host tables respectively.
 *      (3) The precedence configuration takes effect if a packet hit both L3 CAM-based and L3 hash-based flow tables concurrently.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_of_l3FlowTblPri_get(uint32 unit, rtk_of_l3FlowTblList_t *pTable);

/* Function Name:
 *      rtk_of_l3FlowTblPri_set
 * Description:
 *      Set precedence of L3 CAM-based and hash-based flow tables
 * Input:
 *      unit    - unit id
 *      table   - target table
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 * Applicable:
 *      9310
 * Note:
 *      (1) L3 flow table is composed of one CAM-based and two hash-based flow tables.
 *      (2) L3 CAM-based/Hash-based flow tables are physically combo with L3 Prefix/L3 host tables respectively.
 *      (3) The precedence configuration takes effect if a packet hit both L3 CAM-based and L3 hash-based flow tables concurrently.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_of_l3FlowTblPri_set(uint32 unit, rtk_of_l3FlowTblList_t table);

/* Function Name:
 *      rtk_of_l3CamFlowTblMatchField_get
 * Description:
 *      Get match field of L3 CAM-based flow table.
 * Input:
 *      unit    - unit id
 * Output:
 *      pField  - pointer buffer of match field
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      (1) L3 CAM-based flow table can lookup twice for a packet using different match field. The API is used to select
 *          the match field for the 1st and 2nd lookup.
 *      (2) If both lookups are hit, the flow entry of 1st lookup is taken.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_of_l3CamFlowTblMatchField_get(uint32 unit, rtk_of_l3CamFlowTblMatchField_t *pField);

/* Function Name:
 *      rtk_of_l3CamFlowTblMatchField_set
 * Description:
 *      Set match field of L3 CAM-based flow table.
 * Input:
 *      unit    - unit id
 *      field   - match filed
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 * Applicable:
 *      9310
 * Note:
 *      (1) L3 CAM-based flow table can lookup twice for a packet using different match field. The API is used to select
 *          the match field for the 1st and 2nd lookup.
 *      (2) If both lookups are hit, the flow entry of 1st lookup is taken.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_of_l3CamFlowTblMatchField_set(uint32 unit, rtk_of_l3CamFlowTblMatchField_t field);

/* Function Name:
 *      rtk_of_l3HashFlowTblMatchField_get
 * Description:
 *      Get match field of L3 Hash-based flow table.
 * Input:
 *      unit    - unit id
 * Output:
 *      pField  - pointer buffer of match field
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      (1) L3 Hash-based flow table can lookup twice for a packet using different match field. The API is used to select
 *          the match field for the 1st and 2nd lookup.
 *      (2) If both lookups are hit, the flow entry of 1st lookup is taken.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_of_l3HashFlowTblMatchField_get(uint32 unit, rtk_of_l3HashFlowTblMatchField_t *pField);

/* Function Name:
 *      rtk_of_l3HashFlowTblMatchField_set
 * Description:
 *      Set match field of L3 Hash-based flow table.
 * Input:
 *      unit    - unit id
 *      field   - match filed
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 * Applicable:
 *      9310
 * Note:
 *      (1) L3 Hash-based flow table can lookup twice for a packet using different match field. The API is used to select
 *          the match field for the 1st and 2nd lookup.
 *      (2) If both lookups are hit, the flow entry of 1st lookup is taken.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_of_l3HashFlowTblMatchField_set(uint32 unit, rtk_of_l3HashFlowTblMatchField_t field);

/* Function Name:
 *      rtk_of_l3HashFlowTblHashAlgo_get
 * Description:
 *      Get hash algorithm of L3 Hash-based flow table.
 * Input:
 *      unit    - unit id
 *      block   - memory block id
 * Output:
 *      pAlgo   - pointer to hash algorithm
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      (1) L3 Hash-based flow table is composed of two hash-based memeory blocks and each memory block can specify a hash algorithm.
 *          To reduce the hash collision ratio, two memory blocks should specify different algorithm.
 *      (2) There are two hash algorithm supported in 9310.
 *      (3) Each memeory block can lookup twice for a packet using different match field, refer to rtk_of_l3HashFlowTblMatchField_set.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_of_l3HashFlowTblHashAlgo_get(uint32 unit, uint32 block, uint32 *pAlgo);

/* Function Name:
 *      rtk_of_l3HashFlowTblHashAlgo_set
 * Description:
 *      Set hash algorithm of L3 Hash-based flow table.
 * Input:
 *      unit    - unit id
 *      block   - memory block id
 *      algo    - hash algorithm
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 * Applicable:
 *      9310
 * Note:
 *      (1) L3 Hash-based flow table is composed of two hash-based memeory blocks and each memory block can specify a hash algorithm.
 *          To reduce the hash collision ratio, two memory blocks should specify different algorithm.
 *      (2) There are two hash algorithm supported in 9310.
 *      (3) Each memeory block can lookup twice for a packet using different match field, refer to rtk_of_l3HashFlowTblMatchField_set.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_of_l3HashFlowTblHashAlgo_set(uint32 unit, uint32 block, uint32 algo);

/* Function Name:
 *      rtk_of_l3FlowEntrySetField_check
 * Description:
 *      Check whether the set-field type is supported on the specified field ID.
 * Input:
 *      unit     - unit id
 *      field_id - set field ID
 *      type     - set field type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID              - invalid unit id
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_OF_FT_PHASE          - invalid Flow Table phase
 *      RT_ERR_OF_SET_FIELD_ID      - invalid set field ID
 *      RT_ERR_OF_SET_FIELD_TYPE    - invalid set field type
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_of_l3FlowEntrySetField_check(
    uint32                   unit,
    uint32                   field_id,
    rtk_of_setFieldType_t    type);

/* Function Name:
 *      rtk_of_l3CamFlowEntry_get
 * Description:
 *      Get a L3 CAM-based flow entry.
 * Input:
 *      unit        - unit id
 *      entry_idx   - physical entry index
 * Output:
 *      pEntry      - pointer buffer of entry data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_ENTRY_INDEX      - invalid entry index
 *      RT_ERR_ENTRY_NOTFOUND   - specified entry not found
 * Applicable:
 *      9310
 * Note:
 *      (1) An IPv4 SIP/DIP entry occupies two physical entries (index 2N & 2N+1). Valid index is 2N.
 *          An IPv4 SIP+DIP entry occupies two physical entries (index 2N & 2N+1). Valid index is 2N.
 *          An IPv6 SIP/DIP entry occupies three physical entries (index 3N & 3N+1 & 3N+2). Valid index is 3N.
 *          An IPv6 SIP+DIP entry occupies six physical entries (index 6N & 6N+1 & 6N+2 & 6N+3 & 6N+4 & 6N+5). Valid index is 6N.
 *      (2) The entry with lowest index is taken for multiple matching.
 *      (3) The metadata in L3 flow entry is 12 bit width.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_of_l3CamFlowEntry_get(uint32 unit, uint32 entry_idx, rtk_of_l3CamFlowEntry_t *pEntry);

/* Function Name:
 *      rtk_of_l3CamFlowEntry_add
 * Description:
 *      Add a L3 CAM-based flow entry.
 * Input:
 *      unit        - unit id
 *      entry_idx   - physical entry index
 *      pEntry      - pointer buffer of entry data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_VALID_ENTRY_EXIST- Valid entry is existed
 *      RT_ERR_INPUT            - invalid input parameter
 * Applicable:
 *      9310
 * Note:
 *      (1) An IPv4 SIP/DIP entry occupies two physical entries (index 2N & 2N+1). Valid index is 2N.
 *          An IPv4 SIP+DIP entry occupies two physical entries (index 2N & 2N+1). Valid index is 2N.
 *          An IPv6 SIP/DIP entry occupies three physical entries (index 3N & 3N+1 & 3N+2). Valid index is 3N.
 *          An IPv6 SIP+DIP entry occupies six physical entries (index 6N & 6N+1 & 6N+2 & 6N+3 & 6N+4 & 6N+5). Valid index is 6N.
 *      (2) The entry with lowest index is taken for multiple matching.
 *      (3) The metadata in L3 flow entry is 12 bit width.
 *      (4) To overwrite existed entry, set RTK_OF_FLAG_FLOWENTRY_REPLACE in entry.flags.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_of_l3CamFlowEntry_add(uint32 unit, uint32 entry_idx, rtk_of_l3CamFlowEntry_t *pEntry);

/* Function Name:
 *      rtk_of_l3CamFlowEntry_del
 * Description:
 *      Delete the specified L3 CAM-based flow entry.
 * Input:
 *      unit      - unit id
 *      entry_idx - physical entry index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID         - invalid unit id
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 *      RT_ERR_ENTRY_INDEX     - invalid entry index
 * Applicable:
 *      9310
 * Note:
 *      To delete an IPv4 SIP/DIP entry, valid index is 2N.
 *      To delete an IPv4 SIP+DIP entry, valid index is 2N.
 *      To delete an IPv6 SIP/DIP entry, valid index is 3N.
 *      To delete an IPv6 SIP+DIP entry, valid index is 6N.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_of_l3CamFlowEntry_del(uint32 unit, uint32 entry_idx);

/* Function Name:
 *      rtk_of_l3CamFlowEntry_move
 * Description:
 *      Move the specified L3 CAM-based flow entries.
 * Input:
 *      unit    - unit id
 *      pClrIdx - entry index to clear
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID         - invalid unit id
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 *      RT_ERR_ENTRY_INDEX     - invalid entry index
 * Applicable:
 *      9310
 * Note:
 *		In addition to entry data, hit status is also moved.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_of_l3CamFlowEntry_move(uint32 unit, rtk_of_flowMove_t *pClrIdx);

/* Function Name:
 *      rtk_of_l3CamflowEntryHitSts_get
 * Description:
 *      Get the L3 Cam-based flow entry hit status.
 * Input:
 *      unit        - unit id
 *      entry_idx   - flow entry index
 *      reset       - reset the hit status
 * Output:
 *      pIsHit      - pointer to hit status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_ENTRY_INDEX      - invalid entry index
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_ENTRY_NOTFOUND   - specified entry not found
 * Applicable:
 *      9310
 * Note:
 *      The hit status can be cleared by configuring parameter 'reset' to 1.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_of_l3CamflowEntryHitSts_get(
    uint32    unit,
    uint32    entry_idx,
    uint32    reset,
    uint32    *pIsHit);

/* Function Name:
 *      rtk_of_l3HashFlowEntry_get
 * Description:
 *      Add a L3 Hash-based flow entry.
 * Input:
 *      unit    - unit id
 *      pEntry  - pointer buffer of entry data
 * Output:
 *      pEntry  - pointer buffer of entry data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_ENTRY_NOTFOUND   - specified entry not found
 * Applicable:
 *      9310
 * Note:
 *      (1) To match metadata, set RTK_OF_FLAG_FLOWENTRY_MD_CMP in entry.flags.
 *      (2) The metadata in L3 flow entry is 12 bit width.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_of_l3HashFlowEntry_get(uint32 unit, rtk_of_l3HashFlowEntry_t *pEntry);

/* Function Name:
 *      rtk_of_l3HashFlowEntryNextValid_get
 * Description:
 *      Get next valid L3 Hash-based flow entry from the specified device.
 * Input:
 *      unit         - unit id
 *      pScan_idx    - currently scan index of l3 hash-based flow table to get next.
 * Output:
 *      pEntry       - pointer buffer of entry data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID           - invalid unit id
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 *      RT_ERR_OUT_OF_RANGE      - input parameter out of range
 *      RT_ERR_INPUT             - invalid input parameter
 *      RT_ERR_ENTRY_NOTFOUND    - no next valid entry found
 * Applicable:
 *      9310
 * Note:
 *      (1) Input -1 for getting the first valid entry of l3 hash-based flow table.
 *      (2) The pScan_idx is both the input and output argument.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_of_l3HashFlowEntryNextValid_get(
    uint32                   unit,
    int32                    *pScan_idx,
    rtk_of_l3HashFlowEntry_t *pEntry);

/* Function Name:
 *      rtk_of_l3HashFlowEntry_add
 * Description:
 *      Add a L3 Hash-based flow entry.
 * Input:
 *      unit    - unit id
 *      pEntry  - pointer buffer of entry data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_ENTRY_EXIST  - entry existed
 *      RT_ERR_TBL_FULL     - The table is full
 * Applicable:
 *      9310
 * Note:
 *      (1) An IPv4 SIP/DIP entry occupies two physical entries.
 *          An IPv4 SIP+DIP entry occupies two physical entries.
 *          An IPv6 SIP/DIP entry occupies three physical entries.
 *          An IPv6 SIP+DIP entry occupies six physical entries.
 *      (2) To match metadata, set RTK_OF_FLAG_FLOWENTRY_MD_CMP in entry.flags.
 *          The metadata in L3 flow entry is 12 bit width.
 *      (3) To overwrite existed entry, set RTK_OF_FLAG_FLOWENTRY_REPLACE in entry.flags.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_of_l3HashFlowEntry_add(uint32 unit, rtk_of_l3HashFlowEntry_t *pEntry);

/* Function Name:
 *      rtk_of_l3HashFlowEntry_del
 * Description:
 *      Delete a L3 Hash-based flow entry.
 * Input:
 *      unit    - unit id
 *      pEntry  - pointer buffer of entry data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_ENTRY_NOTFOUND   - specified entry not found
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_of_l3HashFlowEntry_del(uint32 unit, rtk_of_l3HashFlowEntry_t *pEntry);

/* Function Name:
 *      rtk_of_l3HashFlowEntry_delAll
 * Description:
 *      Delete all L3 Hash-based flow entry.
 * Input:
 *      unit    - unit id
 *      pEntry  - pointer buffer of entry data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_of_l3HashFlowEntry_delAll(uint32 unit);

/* Function Name:
 *      rtk_of_l3HashflowEntryHitSts_get
 * Description:
 *      Get the L3 Hash-based flow entry hit status.
 * Input:
 *      unit        - unit id
 *      pEntry      - pointer buffer of entry data
 *      reset       - reset the hit status
 * Output:
 *      pIsHit      - pointer to hit status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      The hit status can be cleared by configuring parameter 'reset' to 1.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_of_l3HashflowEntryHitSts_get(
    uint32                   unit,
    rtk_of_l3HashFlowEntry_t *pEntry,
    uint32                   reset,
    uint32                   *pIsHit);

/* Function Name:
 *      rtk_of_groupEntry_get
 * Description:
 *      Get a group entry.
 * Input:
 *      unit        - unit id
 *      entry_idx   - entry index
 * Output:
 *      pEntry      - pointer buffer of entry data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      (1) For group type OF_GROUP_TYPE_ALL, 'bucket_num' consecutive buckets index by 'bucket_id' are executed.
 *      (2) For group type OF_GROUP_TYPE_SELECT, only one of the consecutive buckets is executed.
 *      (3) For group type OF_GROUP_TYPE_INDIRECT, 'bucket_num' is not used.
 *      (4) In 9310, max. bucket_num is 128 and max. bucket_id is 8191.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_of_groupEntry_get(uint32 unit, uint32 entry_idx, rtk_of_groupEntry_t *pEntry);

/* Function Name:
 *      rtk_of_groupEntry_set
 * Description:
 *      Set a group entry.
 * Input:
 *      unit        - unit id
 *      entry_idx   - entry index
 *      pEntry      - pointer buffer of entry data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 * Applicable:
 *      9310
 * Note:
 *      (1) For group type OF_GROUP_TYPE_ALL, 'bucket_num' consecutive buckets index by 'bucket_id' are executed.
 *      (2) For group type OF_GROUP_TYPE_SELECT, only one of the consecutive buckets is executed.
 *      (3) For group type INDIRECT, use OF_GROUP_TYPE_ALL with bucket_num=1.
 *      (4) In 9310, max. bucket_num is 128 and max. bucket_id is 8191.  Refer to rtk_of_actionBucket_set.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_of_groupEntry_set(uint32 unit, uint32 entry_idx, rtk_of_groupEntry_t *pEntry);

/* Function Name:
 *      rtk_of_groupTblHashPara_get
 * Description:
 *      Get parameters of the hash algorithm used by group entry type 'select'.
 * Input:
 *      unit    - unit id
 * Output:
 *      para    - pointer buffer of hash parameters
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      Each parameter can be included/excluded separately.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_of_groupTblHashPara_get(uint32 unit, rtk_of_groupTblHashPara_t *para);

/* Function Name:
 *      rtk_of_groupTblHashPara_set
 * Description:
 *      Set parameters of the hash algorithm used by group entry type 'select'.
 * Input:
 *      unit    - unit id
 *      para    - pointer buffer of hash parameters
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      Each parameter can be included/excluded separately.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_of_groupTblHashPara_set(uint32 unit, rtk_of_groupTblHashPara_t *para);

/* Function Name:
 *      rtk_of_actionBucket_get
 * Description:
 *      Get a action bucket entry.
 * Input:
 *      unit        - unit id
 *      entry_idx   - entry index
 *      pEntry      - pointer buffer of entry data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      (1) The actino bucket(s) are refered by group entry. Refer to rtk_of_groupEntry_set.
 *      (2) There are 8192 action buckets supported in 9310.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_of_actionBucket_get(uint32 unit, uint32 entry_idx, rtk_of_actionBucket_t *pEntry);

/* Function Name:
 *      rtk_of_actionBucket_set
 * Description:
 *      Set a action bucket entry.
 * Input:
 *      unit        - unit id
 *      entry_idx   - entry index
 *      pEntry      - pointer buffer of entry data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      (1) The actino bucket(s) are refered by group entry. Refer to rtk_of_groupEntry_set.
 *      (2) There are 8192 action buckets supported in 9310.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_of_actionBucket_set(uint32 unit, uint32 entry_idx, rtk_of_actionBucket_t *pEntry);

/* Function Name:
 *      rtk_of_trapTarget_get
 * Description:
 *      Get target device for trap packet.
 * Input:
 *      unit	- unit id
 * Output:
 *      pTarget - pointer to target device
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *		The API is for stacking system to specify the trap target. Trap target can be either local CPU or master CPU.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_of_trapTarget_get(uint32 unit, rtk_trapTarget_t *pTarget);

/* Function Name:
 *      rtk_of_trapTarget_set
 * Description:
 *      Set target device for trap packet.
 * Input:
 *      unit	- unit id
 *      target 	- target device
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 * Applicable:
 *      9310
 * Note:
 *		The API is for stacking system to specify the trap target. Trap target can be either local CPU or master CPU.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_of_trapTarget_set(uint32 unit, rtk_trapTarget_t target);

/* Function Name:
 *      rtk_of_tblMissAction_get
 * Description:
 *      Get table miss action of flow tables.
 * Input:
 *      unit	- unit id
 *      phase   - Flow Table phase
 * Output:
 *      pAct    - pointer to table miss action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OF_FT_PHASE  - invalid Flow Table phase
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *		None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_of_tblMissAction_get(uint32 unit, rtk_of_flowtable_phase_t phase, rtk_of_tblMissAct_t *pAct);

/* Function Name:
 *      rtk_of_tblMissAction_set
 * Description:
 *      Set table miss action of flow tables.
 * Input:
 *      unit	- unit id
 *      phase   - Flow Table phase
 *      act     - table miss action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OF_FT_PHASE  - invalid Flow Table phase
 *      RT_ERR_INPUT        - invalid input parameter
 * Applicable:
 *      9310
 * Note:
 *		(1) OF_TBLMISS_ACT_FORWARD_NEXT_TBL isn't applicable for the last ingress flow table.
 *		(2) OF_TBLMISS_ACT_TRAP and OF_TBLMISS_ACT_FORWARD_NEXT_TBL aren't applicable for the egress flow table.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_of_tblMissAction_set(uint32 unit, rtk_of_flowtable_phase_t phase, rtk_of_tblMissAct_t act);

/* Function Name:
 *      rtk_of_flowTblCnt_get
 * Description:
 *      Get lookup and matched packet counters of flow tables.
 * Input:
 *      unit     - unit id
 *      phase    - Flow Table phase
 *      type     - counter type
 * Output:
 *      pCnt     - pointer buffer of counter
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_OF_FT_PHASE  - invalid Flow Table phase
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      The counter is cleared after reading.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_of_flowTblCnt_get(uint32 unit, rtk_of_flowtable_phase_t phase, rtk_of_flowTblCntType_t type, uint32 *pCnt);

#endif /* __RTK_OPENFLOW_H__ */

