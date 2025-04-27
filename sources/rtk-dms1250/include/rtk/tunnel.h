/*
 * Copyright (C) 2009-2016 Realtek Semiconductor Corp.
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Definition those public tunneling APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *           (1) Tunneling APIs
 */

#ifndef __RTK_TUNNEL_H__
#define __RTK_TUNNEL_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <rtk/l3.h>

/*
 * Symbol Definition
 */

/* tunnel information */
typedef struct rtk_tunnel_info_s
{
    int32 tunnel_intf_count;        /* number of the current existing tunnel interfaces */
    int32 tunnel_decap_max;         /* maximum IPv4 decapsulations that can be used */
    int32 tunnel_decap_used;        /* current used number of IPv4 tunnel decapsulations */
    int32 tunnel_encap_max;         /* maximum IPv4 encapsulations that can be used */
    int32 tunnel_encap_used;        /* current used number of IPv4 tunnel decapsulations */
} rtk_tunnel_info_t;

typedef enum rtk_tunnel_type_e
{
    RTK_TUNNEL_TYPE_NONE = 0,

    /* L3/IP tunnel */
    RTK_TUNNEL_TYPE_IP_IN_IP,           /* IPv4-in-IPv4 tunnel (RFC 1853/2003/2893) */
    RTK_TUNNEL_TYPE_IP6_IN_IP,          /* IPv6-in-IPv4 tunnel (RFC 1853/2003/2893) */
    RTK_TUNNEL_TYPE_IPANY_IN_IP,        /* IPv6/4-in-IPv4 tunnel (RFC 1853/2003/2893) */
    RTK_TUNNEL_TYPE_IP_IN_IP6,          /* IPv4-in-IPv6 tunnel (RFC 1853/2003/2893) */
    RTK_TUNNEL_TYPE_IP6_IN_IP6,         /* IPv6-in-IPv6 tunnel (RFC 1853/2003/2893) */
    RTK_TUNNEL_TYPE_IPANY_IN_IP6,       /* IPv6/4-in-IPv6 tunnel (RFC 1853/2003/2893) */

    RTK_TUNNEL_TYPE_ISATAP,             /* ISATAP tunnel (RFC 4214) */
    RTK_TUNNEL_TYPE_6TO4,               /* 6to4 tunnel (RFC 3964) */
    RTK_TUNNEL_TYPE_6RD,                /* 6rd tunnel (RFC 5969) */

    /* GRE tunnel */
    RTK_TUNNEL_TYPE_GRE_IP_IN_IP,       /* GRE IPv4-in-IPv4 tunnel (RFC 2784/2890) */
    RTK_TUNNEL_TYPE_GRE_IP6_IN_IP,      /* GRE IPv6-in-IPv4 tunnel (RFC 2784/2890) */
    RTK_TUNNEL_TYPE_GRE_IPANY_IN_IP,    /* GRE IPv6/4-in-IPv4 tunnel (RFC 2784/2890) */
    RTK_TUNNEL_TYPE_GRE_IP_IN_IP6,      /* GRE IPv4-in-IPv4 tunnel (RFC 2784/2890) */
    RTK_TUNNEL_TYPE_GRE_IP6_IN_IP6,     /* GRE IPv6-in-IPv4 tunnel (RFC 2784/2890) */
    RTK_TUNNEL_TYPE_GRE_IPANY_IN_IP6,   /* GRE IPv6/4-in-IPv4 tunnel (RFC 2784/2890) */

    /* L2 tunnel */
    RTK_TUNNEL_TYPE_VXLAN_IP,           /* VxLAN-in-IPv4 tunnel */
    RTK_TUNNEL_TYPE_VXLAN_IP6,          /* VxLAN-in-IPv6 tunnel */

    RTK_TUNNEL_TYPE_VXLAN_GPE_IP,       /* VxLAN-GPE-in-IPv4 tunnel */
    RTK_TUNNEL_TYPE_VXLAN_GPE_IP6,      /* VxLAN-GPE-in-IPv6 tunnel */

    RTK_TUNNEL_TYPE_END
} rtk_tunnel_type_t;

/* tunnel common flags */
typedef enum rtk_tunnel_flag_e
{
    RTK_TUNNEL_FLAG_NONE                        = 0x00000000,   /* none */

    RTK_TUNNEL_FLAG_REPLACE                     = 0x00000001,   /* replace the existing entry with the new info */
    RTK_TUNNEL_FLAG_WITH_L3_INTF_ID             = 0x00000002,   /* specify L3 interface ID by caller */

    RTK_TUNNEL_FLAG_DECAP_DISABLE               = 0x00000100,   /* indicate that the tunnel is disabled at decapsulation */
    RTK_TUNNEL_FLAG_DECAP_USE_CARRIER_TTL       = 0x00000200,   /* inherit the carrier's TTL which would be used by the passenger */
    RTK_TUNNEL_FLAG_DECAP_USE_CARRIER_DSCP      = 0x00000400,   /* inherit the carrier's DSCP which would be used by the passenger */
    RTK_TUNNEL_FLAG_DECAP_KEEP_PASSENGER_DSCP   = 0x00000800,   /* [PRIVATE/NOT_SUPPORT_AT_BEGINING] keep using the original passenger's DSCP, referred by Egress Remarking */

    RTK_TUNNEL_FLAG_ENCAP_DISABLE               = 0x00100000,   /* [PRIVATE/NOT_SUPPORT_AT_BEGINING] indicate that the tunnel is disabled at encapsulation */
    RTK_TUNNEL_FLAG_ENCAP_TTL_DEC_IGNORE        = 0x00200000,   /* ignore the process of decreasing TTL value by one */
    RTK_TUNNEL_FLAG_ENCAP_TTL_ASSIGN            = 0x00400000,   /* force to use the specified TTL value */
    RTK_TUNNEL_FLAG_ENCAP_DONT_FRAG_INHERIT     = 0x00800000,   /* inherit passenger's dont-fragment bit for IPv4 tunnel (ignore encap.dontFrag_en) */
} rtk_tunnel_flag_t;

typedef struct rtk_tunnel_intf_s
{
    rtk_intf_id_t       intf_id;   /* interface ID */

    rtk_vrf_id_t        vrf_id;    /* VRF ID */

    uint32              ttl;       /* TTL/HL scope */

    rtk_tunnel_type_t   type;      /* tunnel type */
    uint32              flags;     /* flags (rtk_tunnel_flag_t) */

    /* Decapsulation-related parameters */
    struct
    {
        rtk_intf_id_t   l3_intf_id;     /* ingress L3 interface ID (0 means dont-care) */
        uint32          priSelGrp_idx;  /* index of priority selection group */
        rtk_pri_t       int_pri;        /* internal priority value (range: 0 ~ 7) */
    } decap;

    /* Encapsulation-related parameters */
    struct
    {
        rtk_l3_pathId_t path_id;        /* nexthop path ID */
        #if defined(CONFIG_SDK_RTL9310)
        /* [RTL9310 only]
         * When path_id = 0 and path.l3_egr_intf_idx != 0,
         * API will use nh_dmac_idx and l3_egr_inf_idx to program encap entry.
         * it CAN NOT be used with path_id at the same time.
         */
        struct
        {
            uint32      nh_dmac_idx;        /* nexthop (L2) DMAC entry index */
            uint32      l3_egr_intf_idx;    /* l3 egress interface index */
        } path;
        #endif

        uint32          flow_label;     /* flow label ID of IPv6 tunnel */
        rtk_enable_t    dontFrag_en;    /* don't frag bit in IPv4 tunnel */
        uint32          ttl;            /* TTL/HL value */
        uint32          qosProfile_idx; /* index of the QoS profile */

        /* verification */
        uint32          fvid_select;    /* 0: inner, 1: outer */
        rtk_enable_t    dbl_tag_sts;    /* double tag's status */
        rtk_vlan_t      dbl_tag_vid;    /* double tag's VID */
        uint32          vlan_inner_tag_tpid_idx;    /* TPID index of inner VLAN tag */
        uint32          vlan_outer_tag_tpid_idx;    /* TPID index of outer VLAN tag */
        rtk_bpe_ecid_t  ecid;                       /* ECID (ext [19:12], base [11:0]) */
    } encap;

    union
    {
        /* RTK_TUNNEL_TYPE_IP_IN_IP */
        struct
        {
            struct
            {
                rtk_ip_addr_t local;
                rtk_ip_addr_t remote;
            } ip;
        } ip_in_ip;

        /* RTK_TUNNEL_TYPE_IP6_IN_IP */
        struct
        {
            struct
            {
                rtk_ip_addr_t local;
                rtk_ip_addr_t remote;
            } ip;
        } ip6_in_ip;

        /* RTK_TUNNEL_TYPE_IPANY_IN_IP */
        struct
        {
            struct
            {
                rtk_ip_addr_t local;
                rtk_ip_addr_t remote;
            } ip;
        } ipany_in_ip;

        /* RTK_TUNNEL_TYPE_IP_IN_IP6 */
        struct
        {
            struct
            {
                rtk_ipv6_addr_t local;
                rtk_ipv6_addr_t remote;
            } ip6;
        } ip_in_ip6;

        /* RTK_TUNNEL_TYPE_IP6_IN_IP6 */
        struct
        {
            struct
            {
                rtk_ipv6_addr_t local;
                rtk_ipv6_addr_t remote;
            } ip6;
        } ip6_in_ip6;

        /* RTK_TUNNEL_TYPE_IPANY_IN_IP6 */
        struct
        {
            struct
            {
                rtk_ipv6_addr_t local;
                rtk_ipv6_addr_t remote;
            } ip6;
        } ipany_in_ip6;

        /* RTK_TUNNEL_TYPE_ISATAP */
        struct
        {
            struct
            {
                rtk_ip_addr_t local;
            } ip;
        } isatap;

        /* RTK_TUNNEL_TYPE_6TO4 */
        struct
        {
            struct
            {
                rtk_ip_addr_t local;
            } ip;
        } ip_6to4;

        /* RTK_TUNNEL_TYPE_6RD */
        struct
        {
            struct
            {
                rtk_ip_addr_t local;
                rtk_ip_addr_t remote;
                uint32 prefix_len;
            } ip;
            struct
            {
                rtk_ipv6_addr_t prefix;
                uint32 prefix_len;
            } ip6;
        } ip_6rd;

        /* RTK_TUNNEL_TYPE_GRE_IP_IN_IP */
        struct
        {
            struct
            {
                rtk_ip_addr_t local;
                rtk_ip_addr_t remote;
            } ip;

            uint32 key;
            rtk_enable_t key_en;
        } gre_ip_in_ip;

        /* RTK_TUNNEL_TYPE_GRE_IP6_IN_IP */
        struct
        {
            struct
            {
                rtk_ip_addr_t local;
                rtk_ip_addr_t remote;
            } ip;

            uint32 key;
            rtk_enable_t key_en;
        } gre_ip6_in_ip;

        /* RTK_TUNNEL_TYPE_GRE_IPANY_IN_IP */
        struct
        {
            struct
            {
                rtk_ip_addr_t local;
                rtk_ip_addr_t remote;
            } ip;

            uint32 key;
            rtk_enable_t key_en;
        } gre_ipany_in_ip;

        /* RTK_TUNNEL_TYPE_GRE_IP_IN_IP6 */
        struct
        {
            struct
            {
                rtk_ipv6_addr_t local;
                rtk_ipv6_addr_t remote;
            } ip6;

            uint32 key;
            rtk_enable_t key_en;
        } gre_ip_in_ip6;

        /* RTK_TUNNEL_TYPE_GRE_IP6_IN_IP6 */
        struct
        {
            struct
            {
                rtk_ipv6_addr_t local;
                rtk_ipv6_addr_t remote;
            } ip6;

            uint32 key;
            rtk_enable_t key_en;
        } gre_ip6_in_ip6;

        /* RTK_TUNNEL_TYPE_GRE_IPANY_IN_IP6 */
        struct
        {
            struct
            {
                rtk_ipv6_addr_t local;
                rtk_ipv6_addr_t remote;
            } ip6;

            uint32 key;
            rtk_enable_t key_en;
        } gre_ipany_in_ip6;

        /* RTK_TUNNEL_TYPE_VXLAN_IP */
        struct
        {
            struct
            {
                uint32 inner_tag_status;    /* 0 - untagged, 1 - tagged */
                uint32 outer_tag_status;    /* 0 - untagged, 1 - tagged */
            } vlan;
            struct
            {
                rtk_ip_addr_t local;
                rtk_ip_addr_t remote;
            } ip;
            struct
            {
                uint32 local_port;          /* use for decapsulation only */
                uint32 remote_port;         /* use for encapsulation only */
            } l4;
            uint32 vni;     /* for Openflow only */
        } vxlan_ip;

        /* RTK_TUNNEL_TYPE_VXLAN_IP6 */
        struct
        {
            struct
            {
                uint32 inner_tag_status;    /* 0 - untagged, 1 - tagged */
                uint32 outer_tag_status;    /* 0 - untagged, 1 - tagged */
            } vlan;
            struct
            {
                rtk_ipv6_addr_t local;
                rtk_ipv6_addr_t remote;
            } ip6;
            struct
            {
                uint32 local_port;          /* use for decapsulation only */
                uint32 remote_port;         /* use for encapsulation only */
            } l4;
            uint32 vni;     /* for Openflow only */
        } vxlan_ip6;

        /* RTK_TUNNEL_TYPE_VXLAN_GPE_IP */
        struct
        {
            struct
            {
                uint32 inner_tag_status;    /* 0 - untagged, 1 - tagged */
                uint32 outer_tag_status;    /* 0 - untagged, 1 - tagged */
            } vlan;
            struct
            {
                rtk_ip_addr_t local;
                rtk_ip_addr_t remote;
            } ip;
            struct
            {
                uint32 local_port;
                uint32 remote_port;
            } l4;
            uint32 vni;     /* for Openflow only */
        } vxlan_gpe_ip;

        /* RTK_TUNNEL_TYPE_VXLAN_GPE_IP6 */
        struct
        {
            struct
            {
                uint32 inner_tag_status;    /* 0 - untagged, 1 - tagged */
                uint32 outer_tag_status;    /* 0 - untagged, 1 - tagged */
            } vlan;
            struct
            {
                rtk_ipv6_addr_t local;
                rtk_ipv6_addr_t remote;
            } ip6;
            struct
            {
                uint32 local_port;
                uint32 remote_port;
            } l4;
            uint32 vni;     /* for Openflow only */
        } vxlan_gpe_ip6;
    };

} rtk_tunnel_intf_t;

typedef struct rtk_tunnel_intf_stats_s
{
    /* RX */
    struct {
        uint64  octets;                 /* Byte count of packets at incoming */
        uint64  pkts;                   /* Packet count of packets at incoming */
        uint64  drops;                  /* Packet count of dropped packets at incoming */
    } rx;

    /* TX */
    struct {
        uint64  octets;                 /* Byte count of packets at outgoing */
        uint64  pkts;                   /* Packet count of packets at outgoing */
        uint64  drops;                  /* Packet count of dropped packets at outgoing */
    } tx;
} rtk_tunnel_intf_stats_t;


typedef enum rtk_tunnel_encap1pPriSrc_e
{
    RTK_TUNNEL_ENCAP_1P_PRI_SRC_FORCE = 0,          /* force to use the specified priority */
    RTK_TUNNEL_ENCAP_1P_PRI_SRC_PASSENGER,          /* use the passenger's priority */
    RTK_TUNNEL_ENCAP_1P_PRI_SRC_INT_PRI,            /* internal priority */
    RTK_TUNNEL_ENCAP_1P_PRI_SRC_END
} rtk_tunnel_encap1pPriSrc_t;

typedef enum rtk_tunnel_encapDscpSrc_e
{
    RTK_TUNNEL_ENCAP_DSCP_SRC_FORCE = 0,        /* force to use the specified DSCP value */
    RTK_TUNNEL_ENCAP_DSCP_SRC_PASSENGER,            /* use the passenger's DSCP value */
    RTK_TUNNEL_ENCAP_DSCP_SRC_INT_PRI_REMARKING,    /* internal priority to DSCP value (remarking) */
    RTK_TUNNEL_ENCAP_DSCP_SRC_END
} rtk_tunnel_encapDscpSrc_t;


typedef struct rtk_tunnel_qosProfile_s
{
    rtk_tunnel_encap1pPriSrc_t  inner_pri_src;      /* 802.1p inner tag priority source */
    rtk_tunnel_encap1pPriSrc_t  outer_pri_src;      /* 802.1p outer tag priority source */
    rtk_tunnel_encapDscpSrc_t   dscp_src;           /* DSCP source */

    /* specified values */
    rtk_pri_t inner_pri;
    rtk_pri_t outer_pri;
    uint32 dscp;
} rtk_tunnel_qosProfile_t;

/* tunnel global control type */
typedef enum rtk_tunnel_globalCtrlType_e
{
    RTK_TUNNEL_GCT_NONE = 0,                            /* Dummy */

    /* specific-tunnel-related configuration */
    RTK_TUNNEL_GCT_VXLAN_UDP_DPORT,                     /* uint16 */

    /* Decapsulation-related configuration */
    RTK_TUNNEL_GCT_DECAP_IP4_SIP_FAIL_ACT,              /* rtk_l3_act_t */
    RTK_TUNNEL_GCT_DECAP_IP6_SIP_IP4CMPT_CHK,           /* rtk_enable_t */
    RTK_TUNNEL_GCT_DECAP_IP6_SIP_IP4MAP_CHK,            /* rtk_enable_t */
    RTK_TUNNEL_GCT_DECAP_IP6_SIP_FAIL_ACT,              /* rtk_l3_act_t */
    RTK_TUNNEL_GCT_DECAP_ISATAP_SIP_TYPE_CHK,           /* rtk_enable_t */
    RTK_TUNNEL_GCT_DECAP_ISATAP_SIP_MAP_CHK,            /* rtk_enable_t */
    RTK_TUNNEL_GCT_DECAP_ISATAP_SIP_FAIL_ACT,           /* rtk_l3_act_t */
    RTK_TUNNEL_GCT_DECAP_6TO4_SIP_CHK,                  /* rtk_enable_t */
    RTK_TUNNEL_GCT_DECAP_6TO4_SIP_FAIL_ACT,             /* rtk_l3_act_t */
    RTK_TUNNEL_GCT_DECAP_6TO4_DIP_CHK,                  /* rtk_enable_t */
    RTK_TUNNEL_GCT_DECAP_6TO4_DIP_FAIL_ACT,             /* rtk_l3_act_t */
    RTK_TUNNEL_GCT_DECAP_6RD_DIP_CHK,                   /* rtk_enable_t */
    RTK_TUNNEL_GCT_DECAP_6RD_DIP_FAIL_ACT,              /* rtk_l3_act_t */

    /* Encapsulation-related configuration */
    RTK_TUNNEL_GCT_ENCAP_IP_HDR_IDENTIFICATION,         /* uint16 */
    RTK_TUNNEL_GCT_ENCAP_MTU_FAIL_ACT,                  /* rtk_l3_act_t */
    RTK_TUNNEL_GCT_ENCAP_TTL_FAIL_ACT,                  /* rtk_l3_act_t */
    RTK_TUNNEL_GCT_ROUTE_TO_TUNNEL_ACT,                 /* rtk_l3_act_t */

    RTK_TUNNEL_GCT_END                                  /* end of enum */
} rtk_tunnel_globalCtrlType_t;

/* union structure for calculating the length of the longest parameter */
typedef union rtk_tunnel_intfCtrlParam_u
{
    rtk_vrf_id_t    vrf_id;
    rtk_intf_id_t   intf_id;
    uint32          uint_32;
    rtk_pri_t       pri;
    rtk_mac_t       mac;
} rtk_tunnel_intfCtrlParam_t;

#if 0
/* tunnel interface control type (ICT) */
typedef enum rtk_tunnel_intfCtrlType_e
{
    RTK_TUNNEL_ICT_NONE = 0,                        /* Dummy */

    RTK_TUNNEL_ICT_VRF_ID,                          /* rtk_vrf_id_t (VRF ID) */

    RTK_TUNNEL_ICT_DECAP_L3_INTF_ID,                /* rtk_intf_id_t (ingress L3 interface ID) */
    RTK_TUNNEL_ICT_DECAP_PRISELGRP_IDX,             /* uint32 (index of priority selection group) */
    RTK_TUNNEL_ICT_DECAP_INT_PRI,                   /* rtk_pri_t (internal priority) */

    RTK_TUNNEL_ICT_ENCAP_L3_INTF_ID,                /* rtk_intf_id_t (egress L3 interface ID) */
    RTK_TUNNEL_ICT_ENCAP_DMAC,                      /* rtk_mac_t (tunnel destination MAC address) */
    RTK_TUNNEL_ICT_ENCAP_SMAC,                      /* rtk_mac_t (tunnel source MAC address) */

    RTK_TUNNEL_ICT_END                              /* end of enum */
} rtk_tunnel_intfCtrlType_t;
#endif

/*
 * Data Declaration
 */


/*
 * Macro Declaration
 */

/*
 * Function Declaration
 */

/* Function Name:
 *      rtk_tunnel_init
 * Description:
 *      Initialize Tunnel module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      9310
 * Note:
 *      Must initialize Tunnel module before calling any tunneling APIs.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_tunnel_init(uint32 unit);

/* Module Name    : Tunneling                */
/* Sub-module Name: Tunneling error handling */

/* Function Name:
 *      rtk_tunnel_info_t_init
 * Description:
 *      Initialize a rtk_tunnel_info_t_init structure
 * Input:
 *      unit  - unit id
 *      pInfo - pointer to tunnel information
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_tunnel_info_t_init(rtk_tunnel_info_t *pInfo);

/* Function Name:
 *      rtk_tunnel_info_get
 * Description:
 *      Get tunnel-related information
 * Input:
 *      unit  - unit id
 *      pInfo - pointer to tunnel information
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_tunnel_info_get(uint32 unit, rtk_tunnel_info_t *pInfo);

/* Function Name:
 *      rtk_tunnel_intf_t_init
 * Description:
 *      Initialize a rtk_tunnel_info_t_init structure
 * Input:
 *      unit  - unit id
 *      pInfo - pointer to tunnel information
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_tunnel_intf_t_init(rtk_tunnel_intf_t *pIntf);

/* Function Name:
 *      rtk_tunnel_intf_create
 * Description:
 *      Create a new tunnel interface
 * Input:
 *      unit  - unit id
 *      pIntf - pointer to tunnel interface containing the basic inputs
 * Output:
 *      pIntf - pointer to tunnel interface (including all info)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID                  - invalid unit id
 *      RT_ERR_NOT_INIT                 - the module is not initial
 *      RT_ERR_NULL_POINTER             - input parameter may be a null pointer
 *      RT_ERR_TBL_FULL                 - table is full
 *      RT_ERR_INTF_MTU_VARIETY_EXCEEDS - variety of MTU values is over the maximum H/W supports
 * Applicable:
 *      9310
 * Note:
 *      (1) Basic required input parameters of the pRoute as input:
 *          type and corresponding fields about that tunnel.
 *      (2) RTK_TUNNEL_FLAG_WITH_L3_INTF_ID flag is used to specify L3 interface ID,
 *          it is only available while the created tunnel is an L3 tunnel interface.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_tunnel_intf_create(uint32 unit, rtk_tunnel_intf_t *pIntf);

/* Function Name:
 *      rtk_tunnel_intf_destroy
 * Description:
 *      Destroy a tunnel interface
 * Input:
 *      unit   - unit id
 *      intfId - tunnel interface ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_NOT_INIT       - the module is not initial
 *      RT_ERR_ENTRY_NOTFOUND - entry is not found
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_tunnel_intf_destroy(uint32 unit, rtk_intf_id_t intfId);

/* Function Name:
 *      rtk_tunnel_intf_destroyAll
 * Description:
 *      Destroy all tunnel interfaces
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_tunnel_intf_destroyAll(uint32 unit);

/* Function Name:
 *      rtk_tunnel_intf_get
 * Description:
 *      Get a tunnel interface by interface ID.
 * Input:
 *      unit  - unit id
 *      pIntf - pointer to tunnel interface (interface id)
 * Output:
 *      pIntf - pointer to tunnel interface (all info)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_tunnel_intf_get(uint32 unit, rtk_tunnel_intf_t *pIntf);

/* Function Name:
 *      rtk_tunnel_intf_set
 * Description:
 *      Set a tunnel interface by interface ID.
 * Input:
 *      unit  - unit id
 *      pIntf - pointer to tunnel interface (interface id)
 * Output:
 *      pIntf - pointer to tunnel interface (all info)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID                  - invalid unit id
 *      RT_ERR_NOT_INIT                 - the module is not initial
 *      RT_ERR_NULL_POINTER             - input parameter may be a null pointer
 *      RT_ERR_INPUT                    - invalid input parameter
 *      RT_ERR_OUT_OF_RANGE             - input parameter out of range
 *      RT_ERR_INTF_MTU_VARIETY_EXCEEDS - variety of MTU values is over the maximum H/W supports
 * Applicable:
 *      9310
 * Note:
 *      (1) It cannot change the type of tunnel interface. (pIntf->type won't be referred.)
 *          Need to destroy it then create a new one.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_tunnel_intf_set(uint32 unit, rtk_tunnel_intf_t *pIntf);

/* Function Name:
 *      rtk_tunnel_intfPathId_get
 * Description:
 *      Get the path ID of the specified tunnel interface
 * Input:
 *      unit    - unit id
 *      intfId  - tunnel interface id
 * Output:
 *      pPathId - pointer to the argurment
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_INPUT          - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.2.0]
 *          New added function.
 */
extern int32
rtk_tunnel_intfPathId_get(uint32 unit, rtk_intf_id_t intfId, rtk_l3_pathId_t *pPathId);

/* Function Name:
 *      rtk_tunnel_intfPathId_set
 * Description:
 *      Set the path ID of the specified tunnel interface
 * Input:
 *      unit   - unit id
 *      intfId - tunnel interface id
 *      pathId - control type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_NOT_INIT       - the module is not initial
 *      RT_ERR_INPUT          - invalid input parameter
 *      RT_ERR_OUT_OF_RANGE   - input parameter out of range
 *      RT_ERR_ENTRY_NOTFOUND - entry not found
 * Applicable:
 *      9310
 * Note:
 *      (1) 9310 only supports Nexthop-type path ID (not ECMP)
 * Changes:
 *      [SDK_3.2.0]
 *          New added function.
 */
extern int32
rtk_tunnel_intfPathId_set(uint32 unit, rtk_intf_id_t intfId, rtk_l3_pathId_t pathId);

/* Function Name:
 *      rtk_tunnel_intfPath_get
 * Description:
 *      Get the path information of the specified tunnel interface
 * Input:
 *      unit          - unit id
 *      intfId        - tunnel interface id
 * Output:
 *      pNhDmacIdx    - pointer to nexthop DMAC entry index
 *      pL3EgrIntfIdx - pointer to L3 egress interface index
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.7.0]
 *          New added function.
 */
extern int32
rtk_tunnel_intfPath_get(uint32 unit, rtk_intf_id_t intfId, uint32 *pNhDmacIdx, uint32 *pL3EgrIntfIdx);

/* Function Name:
 *      rtk_tunnel_intfPath_set
 * Description:
 *      Set the path ID of the specified tunnel interface
 * Input:
 *      unit         - unit id
 *      intfId       - tunnel interface id
 *      nhDmacIdx    - nexthop DMAC entry index
 *      l3EgrIntfIdx - L3 egress interface index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_NOT_INIT       - the module is not initial
 *      RT_ERR_INPUT          - invalid input parameter
 *      RT_ERR_OUT_OF_RANGE   - input parameter out of range
 *      RT_ERR_ENTRY_NOTFOUND - entry not found
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.7.0]
 *          New added function.
 */
extern int32
rtk_tunnel_intfPath_set(uint32 unit, rtk_intf_id_t intfId, uint32 nhDmacIdx, uint32 l3EgrIntfIdx);

/* Function Name:
 *      rtk_tunnel_intfStats_get
 * Description:
 *      Get statistic counters of the specified tunnel interface
 * Input:
 *      unit   - unit id
 *      intfId - tunnel interface id
 * Output:
 *      pStats - pointer to the statistic data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID         - invalid unit id
 *      RT_ERR_NOT_INIT        - the module is not initial
 *      RT_ERR_NULL_POINTER    - input parameter may be a null pointer
 *      RT_ERR_NOT_TUNNEL_INTF - input interface type is not tunnel
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [3.2.0]
 *          New added function.
 */
extern int32
rtk_tunnel_intfStats_get(uint32 unit, rtk_intf_id_t intfId, rtk_tunnel_intf_stats_t *pStats);

/* Function Name:
 *      rtk_tunnel_intfStats_reset
 * Description:
 *      Reset statistic counters of the specified tunnel interface
 * Input:
 *      unit   - unit id
 *      intfId - tunnel interface id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID         - invalid unit id
 *      RT_ERR_NOT_INIT        - the module is not initial
 *      RT_ERR_NULL_POINTER    - input parameter may be a null pointer
 *      RT_ERR_NOT_TUNNEL_INTF - input interface type is not tunnel
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [3.2.0]
 *          New added function.
 */
extern int32
rtk_tunnel_intfStats_reset(uint32 unit, rtk_intf_id_t intfId);

/* Function Name:
 *      rtk_tunnel_qosProfile_get
 * Description:
 *      Get the QoS profile with the specified index.
 * Input:
 *      unit     - unit id
 *      idx      - index of QoS profile
 * Output:
 *      pProfile - pointer to the QoS prifle
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_tunnel_qosProfile_get(uint32 unit, uint32 idx, rtk_tunnel_qosProfile_t *pProfile);

/* Function Name:
 *      rtk_tunnel_qosProfile_set
 * Description:
 *      Set the QoS profile with the specified index.
 * Input:
 *      unit     - unit id
 *      idx      - index of QoS profile
 *      pProfile - pointer to the QoS prifle
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - the module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_tunnel_qosProfile_set(uint32 unit, uint32 idx, rtk_tunnel_qosProfile_t profile);

/* Function Name:
 *      rtk_tunnel_globalCtrl_get
 * Description:
 *      Get the global configuration of the specified control type
 * Input:
 *      unit - unit id
 *      type - control type
 * Output:
 *      pArg - pointer to the argurment
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_tunnel_globalCtrl_get(uint32 unit, rtk_tunnel_globalCtrlType_t type, int32 *pArg);

/* Function Name:
 *      rtk_tunnel_globalCtrl_set
 * Description:
 *      Set the global configuration of the specified control type
 * Input:
 *      unit - unit id
 *      type - control type
 *      arg  - argurment
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_tunnel_globalCtrl_set(uint32 unit, rtk_tunnel_globalCtrlType_t type, int32 arg);

/* Function Name:
 *      rtk_capwap_udpPort_get
 * Description:
 *      Get UDP port number of CAPWAP control and data packet.
 * Input:
 *      unit	    - unit id
 * Output:
 *      pCtrl_port	- pointer to udp control port
 *      pData_port 	- pointer to udp data port
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  	- invalid unit id
 *      RT_ERR_NOT_INIT 	- The module is not initial
 *      RT_ERR_NULL_POINTER	- input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_capwap_udpPort_get(uint32 unit, uint32 *pCtrl_port, uint32 *pData_port);

/* Function Name:
 *      rtk_capwap_udpPort_set
 * Description:
 *      Set UDP port number of CAPWAP control and data packet.
 * Input:
 *      unit	    - unit id
 *      ctrl_port	- udp control port
 *      data_port 	- udp data port
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_capwap_udpPort_set(uint32 unit, uint32 ctrl_port, uint32 data_port);

#endif /* __RTK_TUNNEL_H__ */

