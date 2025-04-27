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
 * Purpose : Definition of VLAN API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) Vlan table configure and modification
 *           (2) Accept frame type
 *           (3) Vlan ingress/egress filter
 *           (4) Port based and protocol based vlan
 *           (5) TPID configuration
 *           (6) Ingress tag handling
 *           (7) Tag format handling
 *
 */

#ifndef __RTK_VLAN_H__
#define __RTK_VLAN_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/port.h>
#include <rtk/l2.h>

/*
 * Symbol Definition
 */
typedef enum rtk_vlan_svlMode_e
{
    VLAN_SVL_MODE_FID_MAC_TYPE = 0, /* MAC type (unicast/multicast) mode */
    VLAN_SVL_MODE_FID_VLAN,         /* VLAN mode */
    VLAN_SVL_MODE_END
} rtk_vlan_svlMode_t;

/* SA learning mode */
typedef enum rtk_vlan_saLrnMode_e
{
    VLAN_SALRN_MODE_LEARN_ALL_END = 0,
    VLAN_SALRN_MODE_LEARN_BY_VLAN_MEMBER,  /* do not accept non vlan member sa learning */
    VLAN_SALRN_MODE_END,
} rtk_vlan_saLrnMode_t;

typedef enum rtk_vlan_l2LookupMode_e
{
    VLAN_L2_LOOKUP_MODE_VID = 0,
    VLAN_L2_LOOKUP_MODE_FID,
    VLAN_L2_LOOKUP_MODE_END
} rtk_vlan_l2LookupMode_t;

/* acceptable frame type of 802.1Q and 802.1ad */
typedef enum rtk_vlan_acceptFrameType_e
{
    ACCEPT_FRAME_TYPE_ALL = 0,          /* untagged, priority-tagged and tagged  */
    ACCEPT_FRAME_TYPE_TAG_ONLY,         /* tagged                                */
    ACCEPT_FRAME_TYPE_UNTAG_ONLY,       /* untagged and priority-tagged          */
    ACCEPT_FRAME_TYPE_END
} rtk_vlan_acceptFrameType_t;

/* Port-based VLAN mode */
typedef enum rtk_vlan_pbVlan_mode_e
{
    PBVLAN_MODE_UNTAG_AND_PRITAG = 0,
    PBVLAN_MODE_UNTAG_ONLY,
    PBVLAN_MODE_ALL_PKT,
    PBVLAN_MODE_END
} rtk_vlan_pbVlan_mode_t;

/* VLAN ingress filtering */
typedef enum rtk_vlan_ifilter_e
{
    INGRESS_FILTER_FWD = ACTION_FORWARD,
    INGRESS_FILTER_DROP = ACTION_DROP,
    INGRESS_FILTER_TRAP = ACTION_TRAP2CPU,
    INGRESS_FILTER_TRAP2MASTERCPU = ACTION_TRAP2MASTERCPU,
    INGRESS_FILTER_END
} rtk_vlan_ifilter_t;

/* Ingress VLAN conversion block mode */
typedef enum rtk_vlan_igrVlanCnvtBlk_mode_e
{
    CONVERSION_MODE_DISABLED = 0,
    CONVERSION_MODE_C2SC,
    CONVERSION_MODE_MAC_BASED,
    CONVERSION_MODE_IP_SUBNET_BASED,
    CONVERSION_MODE_END
} rtk_vlan_igrVlanCnvtBlk_mode_t;

/* Egress tag status */
typedef enum rtk_vlan_tagSts_e
{
    TAG_STATUS_UNTAG = 0,
    TAG_STATUS_TAGGED,
    TAG_STATUS_PRIORITY_TAGGED,
    TAG_STATUS_INTERNAL,
    TAG_STATUS_END
} rtk_vlan_tagSts_t;

/*VLAN tag keep type*/
typedef enum rtk_vlan_tagKeepType_e
{
    TAG_KEEP_TYPE_NOKEEP = 0,
    TAG_KEEP_TYPE_FORMAT,
    TAG_KEEP_TYPE_CONTENT,
    TAG_KEEP_TYPE_END
} rtk_vlan_tagKeepType_t;

/*VLAN Translation Control Lookupmiss Act*/
typedef enum rtk_vlan_lookupMissAct_e
{
    LOOKUPMISS_FWD = ACTION_FORWARD,
    LOOKUPMISS_DROP = ACTION_DROP,
    LOOKUPMISS_END
} rtk_vlan_lookupMissAct_t;

typedef enum rtk_vlan_priTagVidSrc_e
{
    LEARNING_VID_PRI = 0,
    LEARNING_VID_PBASED,
    LEARNING_VID_END
} rtk_vlan_priTagVidSrc_t;

typedef enum rtk_vlan_l2tblCnvtUnknAct_e
{
    UNKN_TFC_NMLFWD = 0,
    UNKN_TFC_UNTAG,
    UNKN_TFC_PRITAG,
    UNKN_TFC_PBASEDOVERWT,
    UNKN_TFC_END
} rtk_vlan_l2tblCnvtUnknAct_t;

typedef enum rtk_vlan_l2mactype_e
{
    VLAN_L2_MAC_TYPE_UC = 0,
    VLAN_L2_MAC_TYPE_MC,
    VLAN_L2_MAC_TYPE_END
} rtk_vlan_l2mactype_t;

typedef enum rtk_vlan_tagType_e
{
    VLAN_TAG_TYPE_INNER,
    VLAN_TAG_TYPE_OUTER,
    VLAN_TAG_TYPE_EXTRA,
    VLAN_TAG_TYPE_END
} rtk_vlan_tagType_t;

typedef enum rtk_vlan_pktTagMode_e
{
    VLAN_PKT_TAG_MODE_UNTAG = 0,
    VLAN_PKT_TAG_MODE_INNERTAG,
    VLAN_PKT_TAG_MODE_OUTERTAG,
    VLAN_PKT_TAG_MODE_DBLTAG,
    VLAN_PKT_TAG_MODE_ALL,
    VLAN_PKT_TAG_MODE_END
} rtk_vlan_pktTagMode_t;

typedef enum rtk_vlan_tagTpidAct_e {
    VLAN_TAG_TPID_ACT_NONE = 0,
    VLAN_TAG_TPID_ACT_ASSIGN,
    VLAN_TAG_TPID_ACT_END
} rtk_vlan_tagTpidAct_t;

typedef enum rtk_vlan_tagStatusAct_e {
    VLAN_TAG_STATUS_ACT_UNTAGGED = 0,
    VLAN_TAG_STATUS_ACT_TAGGED,
    VLAN_TAG_STATUS_ACT_NONE, /* do nothing */
    VLAN_TAG_STATUS_ACT_END
} rtk_vlan_tagStatusAct_t;

typedef enum rtk_vlan_tagVidAct_e {
    VLAN_TAG_VID_ACT_NONE = 0,
    VLAN_TAG_VID_ACT_ASSIGN, /* assign a new VID */
    VLAN_TAG_VID_ACT_SHIFT, /* shift VID according to the value of VID field */
    VLAN_TAG_VID_ACT_COPY_FROM_INNER,
    VLAN_TAG_VID_ACT_COPY_FROM_OUTER,
    VLAN_TAG_VID_ACT_END
} rtk_vlan_tagVidAct_t;

typedef enum rtk_vlan_tagPriorityAct_e {
    VLAN_TAG_PRIORITY_ACT_NONE = 0,
    VLAN_TAG_PRIORITY_ACT_ASSIGN,
    VLAN_TAG_PRIORITY_ACT_COPY_FROM_INNER,
    VLAN_TAG_PRIORITY_ACT_COPY_FROM_OUTER,
    VLAN_TAG_PRIORITY_ACT_END
} rtk_vlan_tagPriorityAct_t;

typedef enum rtk_vlan_vlanCnvtTagSts_e{
    VLAN_CNVT_TAG_STS_UNTAGGED = 0 ,
    VLAN_CNVT_TAG_STS_TAGGED,
    VLAN_CNVT_TAG_STS_ALL, /* don't care */
    VLAN_CNVT_TAG_STS_END
} rtk_vlan_vlanCnvtTagSts_t;

/* Source of egress TPID */
typedef enum rtk_vlan_egrTpidSrc_e
{
    VLAN_TPID_SRC_ORIG = 0,     /* original TPID if the packet has one */
    VLAN_TPID_SRC_EGR,          /* follow egress port's configuration */
    VLAN_TPID_SRC_END
} rtk_vlan_egrTpidSrc_t;

typedef enum rtk_vlan_bridgeMode_e {
    VLAN_BRIDGE_MODE_MAC_BASED = 0,
    VLAN_BRIDGE_MODE_IP_BASED,
    VLAN_BRIDGE_MODE_END
} rtk_vlan_bridgeMode_t;

/*
 * Data Declaration
 */

/* for 9300/9310  */
typedef struct rtk_vlan_vlanCnvtRangeCheck_vid_s
{
    rtk_vlanType_t      vid_type;
    uint32              vid_upper_bound;    /* VID range upper bound */
    uint32              vid_lower_bound;    /* VID range lower bound */
#if defined(CONFIG_SDK_RTL8390) || defined(CONFIG_SDK_RTL8380)
     /* for 8390/8380 */
    uint32              reverse;            /* reverse operation */
#endif
} rtk_vlan_vlanCnvtRangeCheck_vid_t;

/* Content of protocol vlan group */
typedef struct rtk_vlan_protoGroup_s
{
    rtk_vlan_protoVlan_frameType_t  frametype;
    uint32                          framevalue;
} rtk_vlan_protoGroup_t;

/* configuration of each protocol vlan */
typedef struct rtk_vlan_protoVlanCfg_s
{
#if defined(CONFIG_SDK_RTL8390)
    uint32      valid;
#endif
#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    rtk_vlanType_t  vlan_type;
    rtk_enable_t    vid_assign;
    rtk_enable_t    pri_assign;
#endif
    rtk_vlan_t  vid;
    rtk_pri_t   pri;
} rtk_vlan_protoVlanCfg_t;

typedef struct rtk_vlan_macVlanEntry_s
{
    uint32                      valid;
#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    uint32                      port_care;
    uint32                      port_type;
    uint32                      port;
#endif
    rtk_vlan_acceptFrameType_t  ivid_aft;
    rtk_vlan_acceptFrameType_t  ovid_aft;
    rtk_mac_t                   mac;
    rtk_mac_t                   mac_care;

    rtk_action_t                fwd_action;
    rtk_enable_t                igrvlanfilter_ignore;
    rtk_vlanType_t              vlan_type;
    rtk_vlan_tagTpidAct_t       tpid_action;
    uint32                      tpid_idx;
    rtk_vlan_tagStatusAct_t     sts_action;

    rtk_enable_t                pri_assign;
    rtk_pri_t                   pri_new;
    rtk_enable_t                vid_assign;
    rtk_vlan_t                  vid_new;
}rtk_vlan_macVlanEntry_t;

typedef struct rtk_vlan_macBasedEntry_s
{
    rtk_mac_t                   mac;
    rtk_pri_t                   pri_new;
    rtk_vlan_t                  vid_new;
} rtk_vlan_macBasedEntry_t;

typedef struct rtk_vlan_ipSubnetVlanEntry_s
{
    uint32                      valid;
#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    uint32                      port_care;
    uint32                      port_type;
    uint32                      port;
#endif
    rtk_vlan_acceptFrameType_t  ivid_aft;
    rtk_vlan_acceptFrameType_t  ovid_aft;
    ipaddr_t                    sip;
    ipaddr_t                    sip_care;

    rtk_action_t                fwd_action;
    rtk_enable_t                igrvlanfilter_ignore;
    rtk_vlanType_t              vlan_type;
    rtk_vlan_tagTpidAct_t       tpid_action;
    uint32                      tpid_idx;
    rtk_vlan_tagStatusAct_t     sts_action;

    rtk_enable_t                pri_assign;
    rtk_pri_t                   pri_new;
    rtk_enable_t                vid_assign;
    rtk_vlan_t                  vid_new;
} rtk_vlan_ipSubnetVlanEntry_t;

/* configuration of ingress VLAN conversion (C2SC) table */
typedef struct rtk_vlan_igrVlanCnvtEntry_s
{
    uint32      valid;

#if defined (CONFIG_SDK_RTL8390)
    uint8       vid_ignore;         /* set to 1 to ignore the search key 'vid' */
    rtk_vlan_t  vid;                /* search key */
    uint32      rngchk_result;      /* search key, bitmask result of range check configurations */
    uint32      rngchk_result_mask; /* mask bits, each bit maps to the corresponding range check
                                     * configurations. set a bit to 0 to ignore comparing result
                                     * for the corresponding range check configuration.
                                     */
    uint8       priority_ignore;    /* set to 1 to ignore the search key 'priority' */
    rtk_pri_t   priority;           /* search key, inner user priority */
    uint8       port_ignore;        /* set to 1 to ignore the search key 'port' */
    rtk_port_t  port;               /* search key, ingress port */

    uint32      vid_cnvt_sel;       /* 0: convert to new inner VID 1: convert to outer VID */
    uint32      vid_shift;          /* 0: directly assign with 'vid_new' 1: shift with 'vid_new' */
    uint32      vid_shift_sel;      /* 0: positive, 1: negative */
    rtk_vlan_t  vid_new;            /* value to assign or shift */
    uint32      pri_assign;         /* convert user priority to 'pri_new' */
    rtk_pri_t   pri_new;            /* the field is valid only if 'pri_assign' = 1.
                                     * 'vid_cnvt_sel' = 0, user priority is converted to 'pri_new'
                                     * 'vid_cnvt_sel' = 1, 'pri_new' is used as user priority of
                                     * outer tagged packet when transmitting.
                                     */
    uint32      inner_tag_sts;      /* force egress inner tag status for outgoing packet
                                     * 0: inner untag 1: inner tag 2: don't force
                                     */
    uint32      outer_tag_sts;      /* force egress outer tag status for outgoing packet
                                     * 0: outer untag 1: outer tag 2: don't force
                                     */
    uint32      tpid_assign;        /* force TPID value of egress inner/outer tagged packet
                                     * 0: don't force 1: force to the value indicated by 'tpid_idx'
                                     */
    uint32      tpid_idx;           /* the field is valid only if 'tpid_assign' = 1.
                                     * 'vid_cnvt_sel' = 0, index to inner TPID list
                                     * 'vid_cnvt_sel' = 1, index to outer TPID list
                                     */
#endif /* defined (CONFIG_SDK_RTL8390) */

#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)

    /* match key */
    struct {
        rtk_vlan_vlanCnvtTagSts_t   inner_tag_sts;
        rtk_vlan_vlanCnvtTagSts_t   outer_tag_sts;
        rtk_enable_t        inner_vid_ignore;       /* set to 1 to ignore the search key 'ivid' */
        rtk_vlan_t          inner_vid;              /* search key */
        rtk_enable_t        outer_vid_ignore;       /* set to 1 to ignore the search key 'ovid' */
        rtk_vlan_t          outer_vid;              /* search key */
        uint32              rngchk_result;          /* search key, bitmask result of range check configurations */
        uint32              rngchk_result_mask;     /* mask bits, each bit maps to the corresponding range check
                                                     * configurations. set a bit to 0 to ignore comparing result
                                                     * for the corresponding range check configuration.
                                                     */
        rtk_enable_t        inner_priority_ignore;  /* set to 1 to ignore the search key 'inner priority' */
        rtk_pri_t           inner_priority;         /* search key, inner user priority */
        rtk_enable_t        outer_priority_ignore;  /* set to 1 to ignore the search key 'outer priority' */
        rtk_pri_t           outer_priority;         /* search key, outer user priority */
        rtk_enable_t        port_ignore;            /* set to 1 to ignore the search key 'port' */
        uint32              port_type;              /* port type
                                                     * 0x0 - individual port
                                                     * 0x1 - trunk port
                                                     */
        rtk_port_t          port;                   /* search key, ingress port */
    } match_key;

    /* action */
    struct {
        rtk_vlan_tagStatusAct_t     inner_tag_sts;          /* inner VLAN tag status */
        rtk_vlan_tagStatusAct_t     outer_tag_sts;          /* outer VLAN tag status */
        rtk_vlan_tagVidAct_t        inner_vid_action;       /* inner VLAN ID action */
        rtk_vlan_t                  inner_vid_value;        /* value for assign or shift used */
        rtk_vlan_tagVidAct_t        outer_vid_action;       /* outer VLAN ID action */
        rtk_vlan_t                  outer_vid_value;        /* value for assign or shift used */
        rtk_vlan_tagPriorityAct_t   inner_priority_action;  /* inner VLAN priority action */
        rtk_pri_t                   inner_priority;         /* inner VLAN tag priority */
        rtk_vlan_tagPriorityAct_t   outer_priority_action;  /* outer VLAN priority action */
        rtk_pri_t                   outer_priority;         /* outer VLAN tag priority */
        rtk_vlan_tagTpidAct_t       inner_tpid_action;      /* inner VLAN TPID action */
        uint32                      inner_tpid_idx;         /* inner VLAN TPID index */
        rtk_vlan_tagTpidAct_t       outer_tpid_action;      /* outer VLAN TPID action */
        uint32                      outer_tpid_idx;         /* outer VLAN TPID index */
#if defined(CONFIG_SDK_RTL9310)
        rtk_enable_t                intf_id_assign;         /* interface ID assignment */
        rtk_intf_id_t               intf_id;                /* interface ID */
#endif
    } action;

#endif /*defined (CONFIG_SDK_RTL9300) || CONFIG_SDK_RTL931x)*/
} rtk_vlan_igrVlanCnvtEntry_t;

/* configuration of egress VLAN conversion (SC2C) table */
typedef struct rtk_vlan_egrVlanCnvtEntry_s
{
    uint32      valid;

#if defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL8390)
    uint8       vid_ignore;         /* set to 1 to ignore the search key 'vid' */
    rtk_vlan_t  vid;                /* search key, vid either from inner(C' -> C) or outer(S->C) tag,
                                     * refer to 'rtk_vlan_egrVlanCnvtVidSource_set'
                                     */

    uint8       orgpri_ignore;      /* set to 1 to ignore the search key 'vid' */
    rtk_pri_t   orgpri;             /* search key, priority either from inner(C' -> C) or outer(S->C) tag,
                                     * refer to 'rtk_vlan_egrVlanCnvtVidSource_set'
                                     */

    uint32      rngchk_result;      /* search key, bitmask result of range check configurations */
    uint32      rngchk_result_mask; /* mask bits, each bit maps to the corresponding range check
                                     * configurations. set a bit to 0 to ignore comparing result for
                                     * the corresponding range check configuration.
                                     */

    uint8       port_ignore;        /* set to 1 to ignore the search key 'port' */
    rtk_port_t  port;               /* search key, egress port */

    uint32      vid_shift;          /* 0: directly assign with 'vid_new' 1: shift with 'vid_new' */
    uint32      vid_shift_sel;      /* 0: positive, 1: negative */
    rtk_vlan_t  vid_new;            /* value to assign or shift */
    uint32      pri_assign;         /* convert user priority to 'pri_new' */
    rtk_pri_t   pri_new;            /* the field is valid only if 'pri_assign' = 1 */

    uint32      itpid_assign;       /* force TPID value of egress inner tagged packet
                                     * 0: don't force 1: force to the value indicated by 'tpid_idx'
                                     */
    uint32      itpid_idx;          /* index to inner TPID list, the field is valid only if 'tpid_assign' = 1 */
#endif  /* defined (CONFIG_SDK_RTL8390) */

#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    /* match key */
    struct {
        rtk_vlan_vlanCnvtTagSts_t   inner_tag_sts;
        rtk_vlan_vlanCnvtTagSts_t   outer_tag_sts;
        rtk_enable_t        inner_vid_ignore;       /* set to 1 to ignore the search key 'ivid' */
        rtk_vlan_t          inner_vid;              /* search key */
        rtk_enable_t        outer_vid_ignore;       /* set to 1 to ignore the search key 'ovid' */
        rtk_vlan_t          outer_vid;              /* search key */
        uint32              rngchk_result;          /* search key, bitmask result of range check configurations */
        uint32              rngchk_result_mask;     /* mask bits, each bit maps to the corresponding range check
                                                     * configurations. set a bit to 0 to ignore comparing result
                                                     * for the corresponding range check configuration.
                                                     */
        rtk_enable_t        inner_priority_ignore;  /* set to 1 to ignore the search key 'inner priority' */
        rtk_pri_t           inner_priority;         /* search key, inner user priority */
        rtk_enable_t        outer_priority_ignore;  /* set to 1 to ignore the search key 'outer priority' */
        rtk_pri_t           outer_priority;         /* search key, outer user priority */
        rtk_enable_t        port_ignore;            /* set to 1 to ignore the search key 'port' */
        uint32              port_type;              /* port type
                                                     * 0x0 - individual port
                                                     * 0x1 - trunk port
                                                     */
        rtk_port_t          port;                   /* search key, ingress port */
    } match_key;

    /* action */
    struct {
        rtk_vlan_tagStatusAct_t     inner_tag_sts;          /* inner VLAN tag status */
        rtk_vlan_tagStatusAct_t     outer_tag_sts;          /* outer VLAN tag status */
        rtk_vlan_tagVidAct_t        inner_vid_action;       /* inner VLAN ID action */
        rtk_vlan_t                  inner_vid_value;        /* value for assign or shift used */
        rtk_vlan_tagVidAct_t        outer_vid_action;       /* outer VLAN ID action */
        rtk_vlan_t                  outer_vid_value;        /* value for assign or shift used */
        rtk_vlan_tagPriorityAct_t   inner_priority_action;  /* inner VLAN priority action */
        rtk_pri_t                   inner_priority;         /* inner VLAN tag priority */
        rtk_vlan_tagPriorityAct_t   outer_priority_action;  /* outer VLAN priority action */
        rtk_pri_t                   outer_priority;         /* outer VLAN tag priority */
        rtk_vlan_tagTpidAct_t       inner_tpid_action;      /* inner VLAN TPID action */
        uint32                      inner_tpid_idx;         /* inner VLAN TPID index */
        rtk_vlan_tagTpidAct_t       outer_tpid_action;      /* outer VLAN TPID action */
        uint32                      outer_tpid_idx;         /* outer VLAN TPID index */
    } action;
#endif /*defined (CONFIG_SDK_RTL9300) || CONFIG_SDK_RTL931x) */

} rtk_vlan_egrVlanCnvtEntry_t;

typedef struct rtk_vlan_aggrCtrl_s
{
    rtk_vlanType_t vid_src;
    rtk_vlan_priTagVidSrc_t pri_src;
} rtk_vlan_aggrCtrl_t;

/* vlan profile configuration */
typedef struct rtk_vlan_profile_s
{
#if defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL8390)
    rtk_enable_t learn;                  /* ENABLED  - allow to learn source MAC address in the VLAN
                                            DISABLED - disallow to learn */
#endif

#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    rtk_l2_newMacLrnMode_t  lrnMode;     /* learning mode:
                                            HARDWARE_LEARNING - normal learn (done by H/W)
                                            SOFTWARE_LEARNING - learn as suspend entry (the first packet will be forwarded to CPU)
                                            NOT_LEARNING      - do not learn */
#endif

#if defined(CONFIG_SDK_RTL8380) || defined(CONFIG_SDK_RTL8390)
    uint32  l2_mcast_dlf_pm_idx;    /* index to multicast table which stores the unknown L2 multicast
                                       flooding portmask */
    uint32  ip4_mcast_dlf_pm_idx;   /* index to multicast table which stores the unknown IP4 multicast
                                       flooding portmask */
    uint32  ip6_mcast_dlf_pm_idx;   /* index to multicast table which stores the unknown IP6 multicast
                                       flooding portmask */
#endif

#if defined(CONFIG_SDK_RTL9300) || defined(CONFIG_SDK_RTL9310)
    rtk_action_t   l2_newMacAct;    /* new source address action */
    rtk_vlan_bridgeMode_t ipmcBdgMode;   /* 0:MAC-based (forward as L2 multicast) 1:IP-Based */
    rtk_vlan_bridgeMode_t ip6mcBdgMode;
    rtk_action_t   l2mc_bdgLuMisAct; /* L2 Multicast Packet look up miss action,support the following action:
                                                drop/trap/copy to cpu/:forward (flooding to l2 unknown multicast flooding portmask as below)*/
    rtk_portmask_t l2mc_unknFloodPm;
    rtk_action_t   ipmc_bdgLuMisAct; /* IPv4 Multicast Packet look up miss action,support the following action:
                                                drop/trap/copy to cpu/:forward (flooding to l2 unknown multicast flooding portmask as below)*/
    rtk_portmask_t ipmc_unknFloodPm;
    rtk_action_t   ip6mc_bdgLuMisAct; /* IPv6 Multicast Packet look up miss action,support the following action:
                                                drop/trap/copy to cpu/:forward (flooding to l2 unknown multicast flooding portmask as below)*/
    rtk_portmask_t ip6mc_unknFloodPm;
#endif

#if defined(CONFIG_SDK_RTL9300)
    rtk_enable_t ip4_uRoute;
    rtk_enable_t ip6_uRoute;
    rtk_enable_t ip4_mRoute;
    rtk_enable_t ip6_mRoute;
    rtk_enable_t ip4_uRPF_chk;
    rtk_enable_t ip6_uRPF_chk;
    rtk_enable_t ip4_uRPF_dfltRoute;
    rtk_enable_t ip6_uRPF_dfltRoute;
    uint32       ip4_uRPF_chkMode;  /* 0: loose mode 1:strict mode */
    uint32       ip6_uRPF_chkMode;
    rtk_action_t ip4_uRPF_failAct;  /* support drop/trap/forward/copy to cpu */
    rtk_action_t ip6_uRPF_failAct;

    /* the following action support forward/trap/copy to cpu/drop */
    rtk_action_t ipmc_224_0_act;    /* DIP = 224.0.0.0 ~ 224.0.0.255 */
    rtk_action_t ipmc_224_1_act;    /* DIP = 224.0.1.0 ~ 224.0.1.255 */
    rtk_action_t ipmc_239_0_act;    /* DIP = 239.0.0.0 ~ 239.255.255.255*/
    rtk_action_t ip6mc_00xx_act;    /* whose last 32 bits is 0000:00XX */
    rtk_action_t ip6mc_mld_0x_act;  /* IPv6 MLD Packet action for packet which is of this form FF0X:0:0:0:0:0:X:X*/
    rtk_action_t ip6mc_mld_db8_act; /* IPv6 MLD Packet action for packet which is of this form FF0X::DB8:0:0 */
    rtk_action_t ip6_nd_act;        /* IPv6 ND Packet action for packet which is IP_PROTO = 0x3A (ICMPv6) and HSB.ICMPv6.Type = 0x85 ~ 0x89 */
#endif
} rtk_vlan_profile_t;

/*
 * Macro Declaration
 */

/*
 * Function Declaration
 */

/* Function Name:
 *      rtk_vlan_init
 * Description:
 *      Initialize vlan module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Must initialize vlan module before calling any vlan APIs.
 * Changes:
 *      None
 */
extern int32
rtk_vlan_init(uint32 unit);

/* Module Name    : Vlan                                  */
/* Sub-module Name: Vlan table configure and modification */

/* Function Name:
 *      rtk_vlan_create
 * Description:
 *      Create the vlan in the specified device.
 * Input:
 *      unit - unit id
 *      vid  - vlan id to be created
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID    - invalid unit id
 *      RT_ERR_NOT_INIT   - the module is not initial
 *      RT_ERR_VLAN_VID   - invalid vid
 *      RT_ERR_VLAN_EXIST - vlan is exist
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The valid range of vid is 0~4095
 *      (2) Default FID and STG is assigned after vlan creation.
 *      (3) Default FID is equal with VID (IVL mode) and default STG is CIST.
 *      (4) STG can be reassigned later by following APIs.
 *          - rtk_vlan_stg_set
 *      (5) 9300,9310 have only one FID(0) in SVL Mode.
 * Changes:
 *      None
 */
extern int32
rtk_vlan_create(uint32 unit, rtk_vlan_t vid);

/* Function Name:
 *      rtk_vlan_destroy
 * Description:
 *      Destroy the vlan in the specified device.
 * Input:
 *      unit - unit id
 *      vid  - vlan id to be destroyed
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID              - invalid unit id
 *      RT_ERR_NOT_INIT             - the module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The valid range of vid is 0~4095
 * Changes:
 *      None
 */
extern int32
rtk_vlan_destroy(uint32 unit, rtk_vlan_t vid);

/* Function Name:
 *      rtk_vlan_destroyAll
 * Description:
 *      Destroy all vlans except default vlan in the specified device.
 * Input:
 *      unit                 - unit id
 *      restore_default_vlan - keep and restore default vlan id or not?
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - the module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      The restore argument is permit following value:
 *      - 0: remove default vlan
 *      - 1: restore default vlan
 * Changes:
 *      None
 */
extern int32
rtk_vlan_destroyAll(uint32 unit, uint32 restore_default_vlan);

/* Function Name:
 *      rtk_vlan_port_add
 * Description:
 *      Add one vlan member to the specified device.
 * Input:
 *      unit     - unit id
 *      vid      - vlan id
 *      port     - port id for add
 *      is_untag - untagged or tagged member
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID              - invalid unit id
 *      RT_ERR_NOT_INIT             - the module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_PORT_ID              - invalid port id
 *      RT_ERR_VLAN_PORT_MBR_EXIST  - member port exist in the specified vlan
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_OUT_OF_RANGE         - input parameter out of range
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The valid range of vid is 0~4095
 *      (2) The valid value of is_untag are {0: tagged, 1: untagged}
 * Changes:
 *      None
 */
extern int32
rtk_vlan_port_add(uint32 unit, rtk_vlan_t vid, rtk_port_t port, uint32 is_untag);

/* Function Name:
 *      rtk_vlan_port_del
 * Description:
 *      Delete one vlan member from the specified device.
 * Input:
 *      unit - unit id
 *      vid  - vlan id
 *      port - port id for delete
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID              - invalid unit id
 *      RT_ERR_NOT_INIT             - the module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_PORT_ID              - invalid port id
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The valid range of vid is 0~4095
 * Changes:
 *      None
 */
extern int32
rtk_vlan_port_del(uint32 unit, rtk_vlan_t vid, rtk_port_t port);

/* Function Name:
 *      rtk_vlan_port_get
 * Description:
 *      Get the vlan members from the specified device.
 * Input:
 *      unit             - unit id
 *      vid              - vlan id
 * Output:
 *      pMember_portmask - pointer buffer of member ports
 *      pUntag_portmask  - pointer buffer of untagged member ports
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID              - invalid unit id
 *      RT_ERR_NOT_INIT             - the module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The valid range of vid is 0~4095
 * Changes:
 *      None
 */
extern int32
rtk_vlan_port_get(uint32 unit, rtk_vlan_t vid, rtk_portmask_t *pMember_portmask, rtk_portmask_t *pUntag_portmask);

/* Function Name:
 *      rtk_vlan_port_set
 * Description:
 *      Replace the vlan members in the specified device.
 * Input:
 *      unit             - unit id
 *      vid              - vlan id
 *      pMember_portmask - member ports
 *      pUntag_portmask  - untagged member ports
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID              - invalid unit id
 *      RT_ERR_NOT_INIT             - the module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 *      RT_ERR_PORT_MASK            - invalid portmask
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The valid range of vid is 0~4095
 *      (2) Replace with new specified VLAN member regardless of original setting.
 * Changes:
 *      None
 */
extern int32
rtk_vlan_port_set(uint32 unit, rtk_vlan_t vid, rtk_portmask_t *pMember_portmask, rtk_portmask_t *pUntag_portmask);

/* Function Name:
 *      rtk_vlan_mcastGroup_get
 * Description:
 *      Get specific VLAN mcast (flooding) group id.
 * Input:
 *      unit     - unit id
 *      vid      - vlan id
 * Output:
 *      pGroupId - pointer to mcast group id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_OUT_OF_RANGE - profile index is out of range
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.1.1]
 *          New added function.
 */
extern int32
rtk_vlan_mcastGroup_get(uint32 unit, rtk_vlan_t vid, rtk_mcast_group_t *pGroupId);

/* Function Name:
 *      rtk_vlan_mcastGroup_set
 * Description:
 *      Set specific VLAN mcast (flooding) group id.
 * Input:
 *      unit    - unit id
 *      vid     - vlan id
 *      groupId - pointer to mcast group id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *     (1) used for flooding L2 bridging traffic to virtual port member or L2 tunnel.
 *     (2) mcast group should be created by calling rtk_mcast_group_create() API.
 * Changes:
 *      [SDK_3.1.1]
 *          New added function.
 */
extern int32
rtk_vlan_mcastGroup_set(uint32 unit, rtk_vlan_t vid, rtk_mcast_group_t groupId);

/* Function Name:
 *      rtk_vlan_svlMode_get
 * Description:
 *      Get VLAN SVL mode from the specified device.
 * Input:
 *      unit  - unit id
 * Output:
 *      pMode - pointer to SVL mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      (1) When L2 tunnel is disabled, the SVL mode could be set to PER-VLAN mode.
 *          Otherwise, it will be fixed to PER-MAC-TYPE mode.
 *      (2) After the mode is set to PER-VLAN mode, use rtk_vlan_svlFid_set() to
 *          set the SVL FID for each VLAN.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_vlan_svlMode_get(uint32 unit, rtk_vlan_svlMode_t *pMode);

/* Function Name:
 *      rtk_vlan_svlMode_set
 * Description:
 *      Set VLAN SVL mode from the specified device.
 * Input:
 *      unit - unit id
 *      mode - SVL mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - the module is not initial
 * Applicable:
 *      9310
 * Note:
 *      (1) When L2 tunnel is disabled, the SVL mode could be set to PER-VLAN mode.
 *          Otherwise, it will be fixed to PER-MAC-TYPE mode.
 *      (2) After the mode is set to PER-VLAN mode, use rtk_vlan_svlFid_set() to
 *          set the SVL FID for each VLAN.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_vlan_svlMode_set(uint32 unit, rtk_vlan_svlMode_t mode);

/* Function Name:
 *      rtk_vlan_svlFid_get
 * Description:
 *      Get the SVL FID of the vlan from the specified device.
 * Input:
 *      unit - unit id
 *      vid  - VLAN ID
 * Output:
 *      pFid - pointer to SVL FID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      (1) The SVL mode must be set to PER-VLAN mode, or the API cannot be used.
 *      (2) The valid range of FID is 0~4095
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_vlan_svlFid_get(uint32 unit, rtk_vlan_t vid, rtk_fid_t *pFid);

/* Function Name:
 *      rtk_vlan_svlFid_set
 * Description:
 *      Set the SVL FID of the vlan from the specified device.
 * Input:
 *      unit - unit id
 *      vid  - VLAN ID
 *      fid  - SVL FID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_VLAN_VID     - invalid vid
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Applicable:
 *      9310
 * Note:
 *      (1) The SVL mode must be set to PER-VLAN mode, or the API cannot be used.
 *      (2) The valid range of FID is 0~4095
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_vlan_svlFid_set(uint32 unit, rtk_vlan_t vid, rtk_fid_t fid);

/* Function Name:
 *      rtk_vlan_stg_get
 * Description:
 *      Get spanning tree group instance of the vlan from the specified device.
 * Input:
 *      unit - unit id
 *      vid  - vlan id
 * Output:
 *      pStg - pointer buffer of spanning tree group instance
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID              - invalid unit id
 *      RT_ERR_NOT_INIT             - the module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The valid range of vid is 0~4095
 *      (2) STG instance also represents FID in 8390 and 8380
 * Changes:
 *      None
 */
extern int32
rtk_vlan_stg_get(uint32 unit, rtk_vlan_t vid, rtk_stg_t *pStg);

/* Function Name:
 *      rtk_vlan_stg_set
 * Description:
 *      Set spanning tree group instance of the vlan to the specified device.
 * Input:
 *      unit - unit id
 *      vid  - vlan id
 *      stg  - spanning tree group instance
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID              - invalid unit id
 *      RT_ERR_NOT_INIT             - the module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_MSTI                 - invalid msti
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The valid range of vid is 0~4095
 *      (2) STG instance also represents FID in 8390 and 8380
 * Changes:
 *      None
 */
extern int32
rtk_vlan_stg_set(uint32 unit, rtk_vlan_t vid, rtk_stg_t stg);

/* Function Name:
 *      rtk_vlan_l2LookupSvlFid_get
 * Description:
 *      Get the filtering database ID of the vlan from the specified device.
 * Input:
 *      unit - unit id
 *      vid  - vlan id
 *      type - mac type
 * Output:
 *      pFid - pointer buffer of filtering database id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID              - invalid unit id
 *      RT_ERR_NOT_INIT             - the module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      FID is useless when running in IVL mode.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_vlan_l2LookupSvlFid_get(uint32 unit, rtk_vlan_l2mactype_t type, rtk_fid_t *pFid);

/* Function Name:
 *      rtk_vlan_l2LookupSvlFid_set
 * Description:
 *      Set the filter database ID of the vlan to the specified device.
 * Input:
 *      unit - unit id
 *      vid  - vlan id
 *      fid  - filter id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID              - invalid unit id
 *      RT_ERR_NOT_INIT             - the module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_OUT_OF_RANGE         - input parameter out of range
 * Applicable:
 *      9300, 9310
 * Note:
 *      Don't need to care FID when you use the IVL mode.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_vlan_l2LookupSvlFid_set(uint32 unit, rtk_vlan_l2mactype_t type, rtk_fid_t fid);

/* Function Name:
 *      rtk_vlan_l2LookupMode_get
 * Description:
 *      Get L2 lookup mode for L2 traffic.
 * Input:
 *      unit        - unit id
 *      vid         - vlan id
 *      type        - dmac type
 * Output:
 *      pLookupMode - lookup mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID              - invalid unit id
 *      RT_ERR_NOT_INIT             - the module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      Each VLAN can have its own lookup mode for L2  traffic
 * Changes:
 *      [SDK_3.0.0]
 *          (1)New added function.
 *          (2)It obsoletes these functions:
 *              rtk_vlan_l2McastLookupMode_get
 *              rtk_vlan_l2UcastLookupMode_get
 */
extern int32
rtk_vlan_l2LookupMode_get(uint32 unit, rtk_vlan_t vid, rtk_vlan_l2mactype_t type, rtk_vlan_l2LookupMode_t *pLookupMode);

/* Function Name:
 *      rtk_vlan_l2LookupMode_set
 * Description:
 *      Set L2 lookup mode for L2 traffic.
 * Input:
 *      unit       - unit id
 *      vid        - vlan id
 *      type       - dmac type
 *      lookupMode - lookup mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID              - invalid unit id
 *      RT_ERR_NOT_INIT             - the module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_INPUT                - Invalid input parameter
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      Each VLAN can have its own lookup mode for L2  traffic
 * Changes:
 *      [SDK_3.0.0]
 *          (1)New added function.
 *          (2)It obsoletes these functions:
 *              rtk_vlan_l2McastLookupMode_set
 *              rtk_vlan_l2UcastLookupMode_set
 */
extern int32
rtk_vlan_l2LookupMode_set(uint32 unit, rtk_vlan_t vid, rtk_vlan_l2mactype_t type, rtk_vlan_l2LookupMode_t lookupMode);

/* Function Name:
 *      rtk_vlan_groupMask_get
 * Description:
 *      Get specific VLAN Groupmask.
 * Input:
 *      unit       - unit id
 *      vid        - vlan id
 * Output:
 *      pGroupmask - VLAN groupmask configuration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_OUT_OF_RANGE - profile index is out of range
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *     (1) ACL can refer the bits as search key for VLAN based application
 *     (2) Bits[0-5] used for special Application Packet(IGMP/MLD/DHCP/DHCPV6/ARP/ARP_REPLY),
 *      refer the following API
 *          -rtk_trap_portMgmtFrameAction_get/set
 *     (3) Priority : ACL > Application Packet Trap
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_vlan_groupMask_get(uint32 unit, rtk_vlan_t vid, rtk_vlan_groupMask_t *pGroupmask);

/* Function Name:
 *      rtk_vlan_groupMask_set
 * Description:
 *      Set specific VLAN Groupmask.
 * Input:
 *      unit       - unit id
 *      vid        - vlan id
 *      pGroupmask - VLAN Groupmask configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *     (1) ACL can refer the bits as search key for VLAN based application
 *     (2) Bits[0-5] used for special Application Packet(IGMP/MLD/DHCP/DHCPV6/ARP/ARP_REPLY),
 *      refer the following API
 *          -rtk_trap_portMgmtFrameAction_get/set
 *     (3) Priority : ACL > Application Packet Trap
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_vlan_groupMask_set(uint32 unit, rtk_vlan_t vid, rtk_vlan_groupMask_t *pGroupmask);

/* Function Name:
 *      rtk_vlan_profileIdx_get
 * Description:
 *      Get VLAN profile index of specified VLAN.
 * Input:
 *      unit - unit id
 *      vid  - vlan id
 * Output:
 *      pIdx - VLAN profile index
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID              - invalid unit id
 *      RT_ERR_NOT_INIT             - the module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Each VLAN can bind to a VLAN profile.
 * Changes:
 *      None
 */
extern int32
rtk_vlan_profileIdx_get(uint32 unit, rtk_vlan_t vid, uint32 *pIdx);

/* Function Name:
 *      rtk_vlan_profileIdx_set
 * Description:
 *      Set VLAN profile index of specified VLAN.
 * Input:
 *      unit - unit id
 *      vid  - vlan id
 *      idx  - VLAN profile index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID              - invalid unit id
 *      RT_ERR_NOT_INIT             - the module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_OUT_OF_RANGE         - profile index is out of range
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Each VLAN can bind to a VLAN profile.
 * Changes:
 *      None
 */
extern int32
rtk_vlan_profileIdx_set(uint32 unit, rtk_vlan_t vid, uint32 idx);

/* Module Name     : vlan         */
/* Sub-module Name : vlan profile */

/* Function Name:
 *      rtk_vlan_profile_get
 * Description:
 *      Get specific VLAN profile.
 * Input:
 *      unit     - unit id
 *      idx      - VLAN profile index
 * Output:
 *      pProfile - VLAN profile configuration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID              - invalid unit id
 *      RT_ERR_NOT_INIT             - the module is not initial
 *      RT_ERR_OUT_OF_RANGE         - profile index is out of range
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_vlan_profile_get(uint32 unit, uint32 idx, rtk_vlan_profile_t *pProfile);

/* Function Name:
 *      rtk_vlan_profile_set
 * Description:
 *      Set specific VLAN profile.
 * Input:
 *      unit     - unit id
 *      idx      - VLAN profile index
 *      pProfile - VLAN profile configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_OUT_OF_RANGE - profile index is out of range
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_vlan_profile_set(uint32 unit, uint32 idx, rtk_vlan_profile_t *pProfile);

/* Function Name:
 *      rtk_vlan_portFwdVlan_get
 * Description:
 *      Get forwarding vlan(inner/outer vlan)  on specified port.
 * Input:
 *      unit      - unit id
 *      port      - port id
 *      tagType   - packet tag type
 * Output:
 *      pVlanType - pointer to inner/outer vlan
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - the module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      1) Vlan Type is as following
 *          - BASED_ON_INNER_VLAN
 *          - BASED_ON_OUTER_VLAN
 *      2) 8380/8390 only support tag type of VLAN_PKT_TAG_TYPE_ALL
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_vlan_portFwdVlan_get (uint32 unit, rtk_port_t port, rtk_vlan_pktTagMode_t tagMode, rtk_vlanType_t *pVlanType);

/* Function Name:
 *      rtk_vlan_portFwdVlan_set
 * Description:
 *      Set forwarding vlan(inner/outer vlan)  on specified port.
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      tagType  - packet tag type
 *      vlanType - inner/outer vlan
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - the module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      1) Vlan Type is as following
 *          - BASED_ON_INNER_VLAN
 *          - BASED_ON_OUTER_VLAN
 *      2) 8380/8390 only support tag type of VLAN_PKT_TAG_TYPE_ALL
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_vlan_portFwdVlan_set (uint32 unit, rtk_port_t port, rtk_vlan_pktTagMode_t tagMode, rtk_vlanType_t vlanType);

/* Module Name     : vlan           */
/* Sub-module Name : vlan attribute */

/* Module Name     : vlan                */
/* Sub-module Name : vlan port attribute */

/* Function Name:
 *      rtk_vlan_portAcceptFrameType_get
 * Description:
 *      Get vlan accept frame type of the port from the specified device.
 * Input:
 *      unit               - unit id
 *      port               - port id
 *      type               - vlan type
 * Output:
 *      pAccept_frame_type - pointer buffer of accept frame type
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The accept frame type as following:
 *          - ACCEPT_FRAME_TYPE_ALL
 *          - ACCEPT_FRAME_TYPE_TAG_ONLY
 *          - ACCEPT_FRAME_TYPE_UNTAG_ONLY
 *      (2) The API is used get accept frame type for 802.1Q or 802.1ad VLAN
 * Changes:
 *      [SDK_3.0.0]
 *          (1)Usage changed.
 *          (2)It obsoletes these functions:
 *              rtk_vlan_portInnerAcceptFrameType_get
 *              rtk_vlan_portOuterAcceptFrameType_get
 */
extern int32
rtk_vlan_portAcceptFrameType_get(uint32 unit, rtk_port_t port, rtk_vlanType_t type, rtk_vlan_acceptFrameType_t *pAccept_frame_type);

/* Function Name:
 *      rtk_vlan_portAcceptFrameType_set
 * Description:
 *      Set vlan accept frame type of the port to the specified device.
 * Input:
 *      unit              - unit id
 *      port              - port id
 *      accept_frame_type - accept frame type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID                - invalid unit id
 *      RT_ERR_NOT_INIT               - the module is not initial
 *      RT_ERR_PORT_ID                - invalid port id
 *      RT_ERR_VLAN_ACCEPT_FRAME_TYPE - invalid accept frame type
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The accept frame type as following:
 *          - ACCEPT_FRAME_TYPE_ALL
 *          - ACCEPT_FRAME_TYPE_TAG_ONLY
 *          - ACCEPT_FRAME_TYPE_UNTAG_ONLY
 *      (2) The API is used to set accept frame type for 802.1Q or 802.1ad VLAN
 * Changes:
 *      [SDK_3.0.0]
 *          (1)Usage changed.
 *          (2)It obsoletes these functions:
 *              rtk_vlan_portInnerAcceptFrameType_set
 *              rtk_vlan_portOuterAcceptFrameType_set
 */
extern int32
rtk_vlan_portAcceptFrameType_set(uint32 unit, rtk_port_t port, rtk_vlanType_t type, rtk_vlan_acceptFrameType_t accept_frame_type);

/* Module Name    : Vlan                       */
/* Sub-module Name: Vlan ingress/egress filter */

/* Function Name:
 *      rtk_vlan_portIgrFilter_get
 * Description:
 *      Get vlan ingress filter configuration of the port from the specified device.
 * Input:
 *      unit        - unit id
 *      port        - port id
 * Output:
 *      pIgr_filter - pointer buffer of ingress filter configuration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      VLAN ingress filter checks if the packet's ingress port belongs to the given(forwarding) VLAN
 *      when the packet comes in the switch.
 * Changes:
 *      None
 */
extern int32
rtk_vlan_portIgrFilter_get(uint32 unit, rtk_port_t port, rtk_vlan_ifilter_t *pIgr_filter);

/* Function Name:
 *      rtk_vlan_portIgrFilter_set
 * Description:
 *      Set vlan ingress filter configuration of the port from the specified device.
 * Input:
 *      unit       - unit id
 *      port       - port id
 *      igr_filter - ingress filter configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - the module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      VLAN ingress filter checks if the packet's ingress port belongs to the given(forwarding) VLAN
 *      when the packet comes in the switch.
 * Changes:
 *      None
 */
extern int32
rtk_vlan_portIgrFilter_set(uint32 unit, rtk_port_t port, rtk_vlan_ifilter_t igr_filter);

/* Function Name:
 *      rtk_vlan_portEgrFilterEnable_get
 * Description:
 *      Get enable status of egress filtering on specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to enable status of egress filtering
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_vlan_portEgrFilterEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_vlan_portEgrFilterEnable_set
 * Description:
 *      Set enable status of egress filtering on specified port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - enable status of egress filtering
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - the module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_vlan_portEgrFilterEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      rtk_vlan_mcastLeakyEnable_get
 * Description:
 *      Get vlan egress leaky status of the system from the specified device.
 * Input:
 *      unit   - unit id
 * Output:
 *      pLeaky - pointer buffer of vlan leaky of egress
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) Enable leaky function to allow known L2/IP multicast traffic to across VLAN.
 *          That is, egress VLAN filtering is bypassed by known L2/IP multicast traffic.
 *      (2) The status of vlan egress multicast leaky is as following:
 *          - DISABLED
 *          - ENABLED
 * Changes:
 *      None
 */
extern int32
rtk_vlan_mcastLeakyEnable_get(uint32 unit, rtk_enable_t *pLeaky);

/* Function Name:
 *      rtk_vlan_mcastLeakyEnable_set
 * Description:
 *      Set vlan egress leaky configure of the system to the specified device.
 * Input:
 *      unit  - unit id
 *      leaky - vlan leaky of egress
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - the module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) Enable leaky function to allow known L2/IP MULTICAST traffic to across VLAN.
 *          That is, egress VLAN filtering is bypassed by known L2/IP multicast traffic.
 *      (2) The status of vlan egress multicast leaky is as following:
 *          - DISABLED
 *          - ENABLED
 * Changes:
 *      None
 */
extern int32
rtk_vlan_mcastLeakyEnable_set(uint32 unit, rtk_enable_t leaky);

/* Module Name    : Vlan                               */
/* Sub-module Name: Port based and protocol based vlan */

/* Function Name:
 *      rtk_vlan_portPvidMode_get
 * Description:
 *      Get the configuration of port-based VLAN mode.
 * Input:
 *      unit  - unit id
 *      port  - port id
 *      type  - vlan type
 * Output:
 *      pMode - pointer to port-based VLAN mode configuration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      port-based VLAN can apply on different tag format, refer to enum rtk_vlan_pbVlan_mode_t.
 * Changes:
 *      [SDK_3.0.0]
 *          (1)Usage changed.
 *          (2)It obsoletes these functions:
 *              rtk_vlan_portInnerPvidMode_get
 *              rtk_vlan_portOuterPvidMode_get
 */
extern int32
rtk_vlan_portPvidMode_get(uint32 unit, rtk_port_t port, rtk_vlanType_t type, rtk_vlan_pbVlan_mode_t *pMode);

/* Function Name:
 *      rtk_vlan_portPvidMode_set
 * Description:
 *      Set the configuration of  port-based VLAN mode.
 * Input:
 *      unit - unit id
 *      port - port id
 *      type - vlan type
 *      mode - port-based VLAN mode configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - the module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      port-based VLAN can apply on different tag format, refer to enum rtk_vlan_pbVlan_mode_t.
 * Changes:
 *      [SDK_3.0.0]
 *          (1)Usage changed.
 *          (2)It obsoletes these functions:
 *              rtk_vlan_portInnerPvidMode_set
 *              rtk_vlan_portOuterPvidMode_set
 */
extern int32
rtk_vlan_portPvidMode_set(uint32 unit, rtk_port_t port, rtk_vlanType_t type, rtk_vlan_pbVlan_mode_t mode);

/* Function Name:
 *      rtk_vlan_portPvid_get
 * Description:
 *      Get port default vlan id from the specified device.
 * Input:
 *      unit  - unit id
 *      port  - port id
 *      type  - vlan type
 * Output:
 *      pPvid - pointer buffer of port default vlan id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          (1)Usage changed.
 *          (2)It obsoletes these functions:
 *              rtk_vlan_portInnerPvid_get
 *              rtk_vlan_portOuterPvid_get
 */
extern int32
rtk_vlan_portPvid_get(uint32 unit, rtk_port_t port, rtk_vlanType_t type, rtk_vlan_t *pPvid);

/* Function Name:
 *      rtk_vlan_portPvid_set
 * Description:
 *      Set port default vlan id to the specified device.
 * Input:
 *      unit - unit id
 *      port - port id
 *      type - vlan type
 *      pvid - port default vlan id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - the module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_VLAN_VID - invalid vid
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          (1)Usage changed.
 *          (2)It obsoletes these functions:
 *              rtk_vlan_portInnerPvid_set
 *              rtk_vlan_portOuterPvid_set
 */
extern int32
rtk_vlan_portPvid_set(uint32 unit, rtk_port_t port, rtk_vlanType_t type, rtk_vlan_t pvid);

/* Function Name:
 *      rtk_vlan_portPrivateVlanEnable_get
 * Description:
 *      Get the status of Private VLAN function on the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      (1) When the Private VLAN function is enabled on a port,
 *          the incoming packet will be filtered if its forwarding VLAN
 *          is not equal to the PVID of the ingress port.
 * Changes:
 *      [SDK_3.1.1]
 *          New added function.
 */
extern int32
rtk_vlan_portPrivateVlanEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_vlan_portPrivateVlanEnable_set
 * Description:
 *      Set the status of Private VLAN function on the specified port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - the module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      9310
 * Note:
 *      (1) When the Private VLAN function is enabled on a port,
 *          the incoming packet will be filtered if its forwarding VLAN
 *          is not equal to the PVID of the ingress port.
 * Changes:
 *      [SDK_3.1.1]
 *          New added function.
 */
extern int32
rtk_vlan_portPrivateVlanEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      rtk_vlan_protoGroup_get
 * Description:
 *      Get protocol group for protocol based vlan.
 * Input:
 *      unit           - unit id
 *      protoGroup_idx - protocol group index
 * Output:
 *      pProtoGroup    - pointer to protocol group
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_OUT_OF_RANGE - protocol group index is out of range
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      (1) The valid range of protoGroup_idx is 0~7
 * Changes:
 *      None
 */
extern int32
rtk_vlan_protoGroup_get(uint32 unit, uint32 protoGroup_idx, rtk_vlan_protoGroup_t *pProtoGroup);

/* Function Name:
 *      rtk_vlan_protoGroup_set
 * Description:
 *      Set protocol group for protocol based vlan.
 * Input:
 *      unit           - unit id
 *      protoGroup_idx - protocol group index
 *      protoGroup     - protocol group
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID         - invalid unit id
 *      RT_ERR_NOT_INIT        - the module is not initial
 *      RT_ERR_VLAN_FRAME_TYPE - invalid frame type
 *      RT_ERR_OUT_OF_RANGE    - protocol group index is out of range
 *      RT_ERR_INPUT           - invalid input parameter
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      (1) The valid range of protoGroup_idx is 0~7
 *      (2) Frame type is as following:
 *          - FRAME_TYPE_ETHERNET
 *          - FRAME_TYPE_RFC1042 (SNAP)
 *          - FRAME_TYPE_LLCOTHER
 * Changes:
 *      None
 */
extern int32
rtk_vlan_protoGroup_set(uint32 unit, uint32 protoGroup_idx, rtk_vlan_protoGroup_t *pProtoGroup);

/* Function Name:
 *      rtk_vlan_portProtoVlan_get
 * Description:
 *      Get VLAN and priority configuration of specified protocol group on port.
 * Input:
 *      unit           - unit id
 *      port           - port id
 *      protoGroup_idx - protocol group index
 * Output:
 *      pVlan_cfg      - pointer to vlan configuration of protocol group
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_OUT_OF_RANGE - protocol group index is out of range
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      (1) The valid range of protoGroup_idx is 0~7
 * Changes:
 *      None
 */
extern int32
rtk_vlan_portProtoVlan_get(uint32 unit, rtk_port_t port, uint32 protoGroup_idx, rtk_vlan_protoVlanCfg_t *pVlan_cfg);

/* Function Name:
 *      rtk_vlan_portProtoVlan_set
 * Description:
 *      Set VLAN and priority configuration of specified protocol group on port.
 * Input:
 *      unit           - unit id
 *      port           - port id
 *      protoGroup_idx - protocol group index
 *      pVlan_cfg      - vlan configuration of protocol group
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID         - invalid unit id
 *      RT_ERR_NOT_INIT        - the module is not initial
 *      RT_ERR_PORT_ID         - invalid port id
 *      RT_ERR_OUT_OF_RANGE    - protocol group index is out of range
 *      RT_ERR_VLAN_VID        - invalid vlan id
 *      RT_ERR_QOS_1P_PRIORITY - Invalid 802.1p priority
 *      RT_ERR_INPUT           - invalid input parameter
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      (1) The valid range of protoGroup_idx is 0~7
 *      (2) Frame type is as following:
 *          - FRAME_TYPE_ETHERNET
 *          - FRAME_TYPE_RFC1042 (SNAP)
 *          - FRAME_TYPE_LLCOTHER
 * Changes:
 *      None
 */
extern int32
rtk_vlan_portProtoVlan_set(uint32 unit, rtk_port_t port, uint32 protoGroup_idx, rtk_vlan_protoVlanCfg_t *pVlan_cfg);

/* Function Name:
 *      rtk_vlan_macBasedVlan_get
 * Description:
 *      Get MAC-based vlan.
 * Input:
 *      unit      - unit id
 *      index     - entry index
 * Output:
 *      pValid    - pointer buffer of validate or invalidate the entry
 *      pSmac     - pointer buffer of source mac address
 *      pVid      - pointer buffer of MAC-based VLAN ID
 *      pPriority - pointer buffer of assign internal priority for untag packet
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID              - invalid unit id
 *      RT_ERR_NOT_INIT             - the module is not initial
 *      RT_ERR_OUT_OF_RANGE         - index is out of range
 *      RT_ERR_VLAN_C2SC_BLOCK_MODE - try to set/get a entry to an incompatiable block
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Applicable:
 *      8390
 * Note:
 *      Change to use rtk_vlan_macBasedVlanEntry_get(unit, index, pEntry) for 9300, 9310
 *      (1) Totally 1024 entries are supported but the entries are shared by ingress VLAN
 *          conversion, MAC-based VLAN and IP-Subnet-based VLAN functions per-block based.
 *      (2) The corresponding block must be in MAC-based VLAN mode before calling this function,
 *          refer to rtk_vlan_igrVlanCnvtBlkMode_set.
 * Changes:
 *      None
 */
extern int32
rtk_vlan_macBasedVlan_get(
    uint32      unit,
    uint32      index,
    uint32      *pValid,
    rtk_mac_t   *pSmac,
    rtk_vlan_t  *pVid,
    rtk_pri_t   *pPriority);

/* Function Name:
 *      rtk_vlan_macBasedVlan_set
 * Description:
 *      Set MAC-based vlan.
 * Input:
 *      unit     - unit id
 *      index    - entry index
 *      valid    - validate or invalidate the entry
 *      pSmac    - source mac address
 *      vid      - MAC-based VLAN ID
 *      priority - assign internal priority for untag packet
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID              - invalid unit id
 *      RT_ERR_NOT_INIT             - the module is not initial
 *      RT_ERR_OUT_OF_RANGE         - index is out of range
 *      RT_ERR_VLAN_C2SC_BLOCK_MODE - try to set/get a entry to an incompatiable block
 *      RT_ERR_MAC                  - invalid mac address
 *      RT_ERR_VLAN_VID             - invalid vlan id
 *      RT_ERR_PRIORITY             - invalid priority value
 * Applicable:
 *      8390
 * Note:
 *      Change to use rtk_vlan_macBasedVlanEntry_set(unit, index, pEntry) for 9300, 9310
 *      (1) Totally 1024 entries are supported but the entries are shared by ingress VLAN
 *          conversion, MAC-based VLAN and IP-Subnet-based VLAN functions per-block based.
 *      (2) The corresponding block must be in MAC-based VLAN mode before calling this function,
 *          refer to rtk_vlan_igrVlanCnvtBlkMode_set.
 * Changes:
 *      None
 */
extern int32
rtk_vlan_macBasedVlan_set(
    uint32      unit,
    uint32      index,
    uint32      valid,
    rtk_mac_t   *pSmac,
    rtk_vlan_t  vid,
    rtk_pri_t   priority);

/* Function Name:
 *      rtk_vlan_macBasedVlanWithMsk_get
 * Description:
 *      Get MAC-based vlan with source MAC address mask.
 * Input:
 *      unit      - unit id
 *      index     - entry index
 * Output:
 *      pValid    - pointer buffer of validate or invalidate the entry
 *      pSmac     - pointer buffer of source mac address
 *      pSmsk     - pointer buffer of source mac address mask
 *      pVid      - pointer buffer of MAC-based VLAN ID
 *      pPriority - pointer buffer of assign internal priority for untag packet
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID              - invalid unit id
 *      RT_ERR_NOT_INIT             - the module is not initial
 *      RT_ERR_OUT_OF_RANGE         - index is out of range
 *      RT_ERR_VLAN_C2SC_BLOCK_MODE - try to set/get a entry to an incompatiable block
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Applicable:
 *      8390
 * Note:
 *      (1) Totally 1024 entries are supported but the entries are shared by ingress VLAN
 *          conversion, MAC-based VLAN and IP-Subnet-based VLAN functions per-block based.
 *      (2) The corresponding block must be in MAC-based VLAN mode before calling this function,
 *          refer to rtk_vlan_igrVlanCnvtBlkMode_set.
 * Changes:
 *      None
 */
extern int32
rtk_vlan_macBasedVlanWithMsk_get(
    uint32      unit,
    uint32      index,
    uint32      *pValid,
    rtk_mac_t   *pSmac,
    rtk_mac_t   *pSmsk,
    rtk_vlan_t  *pVid,
    rtk_pri_t   *pPriority);

/* Function Name:
 *      rtk_vlan_macBasedVlanWithMsk_set
 * Description:
 *      Set MAC-based vlan with source MAC address mask.
 * Input:
 *      unit     - unit id
 *      index    - entry index
 *      valid    - validate or invalidate the entry
 *      pSmac    - source mac address
 *      pSmsk    - source mac address mask
 *      vid      - MAC-based VLAN ID
 *      priority - assign internal priority for untag packet
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID              - invalid unit id
 *      RT_ERR_NOT_INIT             - the module is not initial
 *      RT_ERR_OUT_OF_RANGE         - index is out of range
 *      RT_ERR_VLAN_C2SC_BLOCK_MODE - try to set/get a entry to an incompatiable block
 *      RT_ERR_MAC                  - invalid mac address
 *      RT_ERR_VLAN_VID             - invalid vlan id
 *      RT_ERR_PRIORITY             - invalid priority value
 * Applicable:
 *      8390
 * Note:
 *      (1) Totally 1024 entries are supported but the entries are shared by ingress VLAN
 *          conversion, MAC-based VLAN and IP-Subnet-based VLAN functions per-block based.
 *      (2) The corresponding block must be in MAC-based VLAN mode before calling this function,
 *          refer to rtk_vlan_igrVlanCnvtBlkMode_set.
 * Changes:
 *      None
 */
extern int32
rtk_vlan_macBasedVlanWithMsk_set(uint32 unit, uint32 index, uint32 valid, rtk_mac_t *pSmac, rtk_mac_t *pSmsk, rtk_vlan_t vid, rtk_pri_t priority);

/* Function Name:
 *      rtk_vlan_macBasedVlanWithPort_get
 * Description:
 *      Get MAC-based vlan with source port ID mask.
 * Input:
 *      unit      - unit id
 *      index     - entry index
 * Output:
 *      pValid    - pointer buffer of validate or invalidate the entry
 *      pSmac     - pointer buffer of source mac address
 *      pSmsk     - pointer buffer of source mac address mask
 *      pPort     - pointer buffer of source port ID
 *      pPmsk     - pointer buffer of source port ID mask
 *      pVid      - pointer buffer of MAC-based VLAN ID
 *      pPriority - pointer buffer of assign internal priority for untag packet
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID              - invalid unit id
 *      RT_ERR_NOT_INIT             - the module is not initial
 *      RT_ERR_OUT_OF_RANGE         - index is out of range
 *      RT_ERR_VLAN_C2SC_BLOCK_MODE - try to set/get a entry to an incompatiable block
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Applicable:
 *      8390
 * Note:
 *      (1) Totally 1024 entries are supported but the entries are shared by ingress VLAN
 *          conversion, MAC-based VLAN and IP-Subnet-based VLAN functions per-block based.
 *      (2) The corresponding block must be in MAC-based VLAN mode before calling this function,
 *          refer to rtk_vlan_igrVlanCnvtBlkMode_set.
 * Changes:
 *      None
 */
extern int32
rtk_vlan_macBasedVlanWithPort_get(
    uint32      unit,
    uint32      index,
    uint32      *pValid,
    rtk_mac_t   *pSmac,
    rtk_mac_t   *pSmsk,
    rtk_port_t  *pPort,
    rtk_port_t  *pPmsk,
    rtk_vlan_t  *pVid,
    rtk_pri_t   *pPriority);

/* Function Name:
 *      rtk_vlan_macBasedVlanWithPort_set
 * Description:
 *      Set MAC-based vlan with source port ID mask.
 * Input:
 *      unit     - unit id
 *      index    - entry index
 *      valid    - validate or invalidate the entry
 *      pSmac    - source mac address
 *      pSmsk    - source mac address mask
 *      port     - source port ID
 *      pmsk     - source port ID mask
 *      vid      - MAC-based VLAN ID
 *      priority - assign internal priority for untag packet
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID              - invalid unit id
 *      RT_ERR_NOT_INIT             - the module is not initial
 *      RT_ERR_OUT_OF_RANGE         - index or port ID bit-mask is out of range
 *      RT_ERR_VLAN_C2SC_BLOCK_MODE - try to set/get a entry to an incompatiable block
 *      RT_ERR_MAC                  - invalid mac address
 *      RT_ERR_VLAN_VID             - invalid vlan id
 *      RT_ERR_PRIORITY             - invalid priority value
 * Applicable:
 *      8390
 * Note:
 *      (1) Totally 1024 entries are supported but the entries are shared by ingress VLAN
 *          conversion, MAC-based VLAN and IP-Subnet-based VLAN functions per-block based.
 *      (2) The corresponding block must be in MAC-based VLAN mode before calling this function,
 *          refer to rtk_vlan_igrVlanCnvtBlkMode_set.
 * Changes:
 *      None
 */
extern int32
rtk_vlan_macBasedVlanWithPort_set(
    uint32      unit,
    uint32      index,
    uint32      valid,
    rtk_mac_t   *pSmac,
    rtk_mac_t   *pSmsk,
    rtk_port_t  port,
    rtk_port_t  pmsk,
    rtk_vlan_t  vid,
    rtk_pri_t   priority);

/* Function Name:
 *      rtk_vlan_portMacBasedVlanEnable_get
 * Description:
 *      Get enable status of MAC-based VLAN on specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to enable status of MAC-based VLAN
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - the module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_vlan_portMacBasedVlanEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_vlan_portMacBasedVlanEnable_set
 * Description:
 *      Set enable status of MAC-based VLAN on specified port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - enable status of MAC-based VLAN
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - the module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_vlan_portMacBasedVlanEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      rtk_vlan_macBasedVlanEntry_get
 * Description:
 *      Get MAC-based vlan.
 * Input:
 *      unit      - unit id
 *      index     - entry index
 * Output:
 *      pEntry    - pointer buffer of mac based vlan entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID              - invalid unit id
 *      RT_ERR_NOT_INIT             - the module is not initial
 *      RT_ERR_OUT_OF_RANGE         - index is out of range
 *      RT_ERR_VLAN_C2SC_BLOCK_MODE - try to set/get a entry to an incompatiable block
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) Totally 1024(9300),2048(9310) entries are supported but the entries are shared by ingress VLAN
 *          conversion, MAC-based VLAN and IP-Subnet-based VLAN functions per-block based.
 *      (2) The corresponding block must be in MAC-based VLAN mode before calling this function,
 *          refer to rtk_vlan_igrVlanCnvtBlkMode_set.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_vlan_macBasedVlanEntry_get(uint32 unit, uint32 index, rtk_vlan_macVlanEntry_t *pEntry);

/* Function Name:
 *      rtk_vlan_macBasedVlanEntry_set
 * Description:
 *      Set MAC-based vlan.
 * Input:
 *      unit   - unit id
 *      index  - entry index
 * Output:
 *      pEntry - pointer buffer of Mac-based VLAN entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID              - invalid unit id
 *      RT_ERR_NOT_INIT             - the module is not initial
 *      RT_ERR_OUT_OF_RANGE         - index is out of range
 *      RT_ERR_VLAN_C2SC_BLOCK_MODE - try to set/get a entry to an incompatiable block
 *      RT_ERR_MAC                  - invalid mac address
 *      RT_ERR_VLAN_VID             - invalid vlan id
 *      RT_ERR_PRIORITY             - invalid priority value
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) Totally 1024(9300),2048(9310) entries are supported but the entries are shared by ingress VLAN
 *          conversion, MAC-based VLAN and IP-Subnet-based VLAN functions per-block based.
 *      (2) The corresponding block must be in MAC-based VLAN mode before calling this function,
 *          refer to rtk_vlan_igrVlanCnvtBlkMode_set.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_vlan_macBasedVlanEntry_set(uint32 unit, uint32 index, rtk_vlan_macVlanEntry_t *pEntry);

/* Function Name:
 *      rtk_vlan_macBasedVlanEntry_add
 * Description:
 *      add a mac-based VLAN entry
 * Input:
 *      unit   - unit id
 *      pEntry - pointer buffer of Mac-based VLAN entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID              - invalid unit id
 *      RT_ERR_NOT_INIT             - the module is not initial
 *      RT_ERR_TBL_FULL             - the table is full
 *      RT_ERR_MAC                  - invalid mac address
 *      RT_ERR_VLAN_VID             - invalid vlan id
 *      RT_ERR_PRIORITY             - invalid priority value
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_vlan_macBasedVlanEntry_add(uint32 unit, rtk_vlan_macBasedEntry_t *pEntry);

/* Function Name:
 *      rtk_vlan_macBasedVlanEntry_del
 * Description:
 *      delete a mac-based VLAN entry
 * Input:
 *      unit   - unit id
 *      pEntry - pointer buffer of Mac-based VLAN entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID              - invalid unit id
 *      RT_ERR_NOT_INIT             - the module is not initial
 *      RT_ERR_MAC                  - invalid mac address
 *      RT_ERR_ENTRY_NOTFOUND       - specified entry not found
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_vlan_macBasedVlanEntry_del(uint32 unit, rtk_vlan_macBasedEntry_t *pEntry);

/* Function Name:
 *      rtk_vlan_ipSubnetBasedVlan_get
 * Description:
 *      Get IP-Subnet-based vlan.
 * Input:
 *      unit      - unit id
 *      index     - entry index
 * Output:
 *      pValid    - pointer buffer of validate or invalidate the entry
 *      pSip      - pointer buffer of source IP address
 *      pSip_mask - pointer buffer of source IP address mask
 *      pVid      - pointer buffer of VLAN ID
 *      pPriority - pointer buffer of assign internal priority for untag packet
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID              - invalid unit id
 *      RT_ERR_NOT_INIT             - the module is not initial
 *      RT_ERR_OUT_OF_RANGE         - index is out of range
 *      RT_ERR_VLAN_C2SC_BLOCK_MODE - try to set/get a entry to an incompatiable block
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Applicable:
 *      8390
 * Note:
 *      Change to use rtk_vlan_ipSubnetBasedVlanEntry_get(unit, index, pEntry) for 9300, 9310
 *      (1) Totally 1024 entries are supported but the entries are shared by ingress VLAN
 *          conversion, MAC-based VLAN and IP-Subnet-based VLAN functions per-block based.
 *      (2) The corresponding block must be in IP-Subnet-based VLAN mode before calling this function,
 *          refer to rtk_vlan_igrVlanCnvtBlkMode_set.
 * Changes:
 *      None
 */
extern int32
rtk_vlan_ipSubnetBasedVlan_get(
    uint32      unit,
    uint32      index,
    uint32      *pValid,
    ipaddr_t    *pSip,
    ipaddr_t    *pSip_mask,
    rtk_vlan_t  *pVid,
    rtk_pri_t   *pPriority);

/* Function Name:
 *      rtk_vlan_ipSubnetBasedVlan_set
 * Description:
 *      Set IP-Subnet-based vlan.
 * Input:
 *      unit     - unit id
 *      index    - entry index
 *      valid    - validate or invalidate the entry
 *      sip      - source IP address
 *      sip_mask - source IP address mask
 *      vid      - VLAN ID
 *      priority - assign internal priority for untag packet
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID              - invalid unit id
 *      RT_ERR_NOT_INIT             - the module is not initial
 *      RT_ERR_OUT_OF_RANGE         - index is out of range
 *      RT_ERR_VLAN_C2SC_BLOCK_MODE - try to set/get a entry to an incompatiable block
 *      RT_ERR_VLAN_VID             - invalid vlan id
 *      RT_ERR_PRIORITY             - invalid priority value
 * Applicable:
 *      8390
 * Note:
 *      Change to use rtk_vlan_ipSubnetBasedVlanEntry_set(unit, index, pEntry) for 9300, 9310
 *      (1) Totally 1024 entries are supported but the entries are shared by ingress VLAN
 *          conversion, MAC-based VLAN and IP-Subnet-based VLAN functions per-block based.
 *      (2) The corresponding block must be in IP-Subnet-based VLAN mode before calling this function,
 *          refer to rtk_vlan_igrVlanCnvtBlkMode_set.
 * Changes:
 *      None
 */
extern int32
rtk_vlan_ipSubnetBasedVlan_set(
    uint32      unit,
    uint32      index,
    uint32      valid,
    ipaddr_t    sip,
    ipaddr_t    sip_mask,
    rtk_vlan_t  vid,
    rtk_pri_t   priority);

/* Function Name:
 *      rtk_vlan_ipSubnetBasedVlanWithPort_get
 * Description:
 *      Get IP-Subnet-based vlan with source port ID mask.
 * Input:
 *      unit       - unit id
 *      index      - entry index
 * Output:
 *      pValid     - pointer buffer of validate or invalidate the entry
 *      pSip       - pointer buffer of source IP address
 *      pSip_mask  - pointer buffer of source IP address mask
 *      pPort      - pointer buffer of source IP address
 *      pPort_mask - pointer buffer of source IP address mask
 *      pVid       - pointer buffer of VLAN ID
 *      pPriority  - pointer buffer of assign internal priority for untag packet
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID              - invalid unit id
 *      RT_ERR_NOT_INIT             - the module is not initial
 *      RT_ERR_OUT_OF_RANGE         - index is out of range
 *      RT_ERR_VLAN_C2SC_BLOCK_MODE - try to set/get a entry to an incompatiable block
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Applicable:
 *      8390
 * Note:
 *      (1) Totally 1024 entries are supported but the entries are shared by ingress VLAN
 *          conversion, MAC-based VLAN and IP-Subnet-based VLAN functions per-block based.
 *      (2) The corresponding block must be in IP-Subnet-based VLAN mode before calling this function,
 *          refer to rtk_vlan_igrVlanCnvtBlkMode_set.
 * Changes:
 *      None
 */
extern int32
rtk_vlan_ipSubnetBasedVlanWithPort_get(
    uint32      unit,
    uint32      index,
    uint32      *pValid,
    ipaddr_t    *pSip,
    ipaddr_t    *pSip_mask,
    rtk_port_t  *pPort,
    rtk_port_t  *pPort_mask,
    rtk_vlan_t  *pVid,
    rtk_pri_t   *pPriority);

/* Function Name:
 *      rtk_vlan_ipSubnetBasedVlanWithPort_set
 * Description:
 *      Set IP-Subnet-based vlan with source port ID mask.
 * Input:
 *      unit      - unit id
 *      index     - entry index
 *      valid     - validate or invalidate the entry
 *      sip       - source IP address
 *      sip_mask  - source IP address mask
 *      port      - source port ID
 *      port_mask - source port ID mask
 *      vid       - VLAN ID
 *      priority  - assign internal priority for untag packet
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID              - invalid unit id
 *      RT_ERR_NOT_INIT             - the module is not initial
 *      RT_ERR_OUT_OF_RANGE         - index or port ID mask is out of range
 *      RT_ERR_VLAN_C2SC_BLOCK_MODE - try to set/get a entry to an incompatiable block
 *      RT_ERR_VLAN_VID             - invalid vlan id
 *      RT_ERR_PRIORITY             - invalid priority value
 * Applicable:
 *      8390
 * Note:
 *      (1) Totally 1024 entries are supported but the entries are shared by ingress VLAN
 *          conversion, MAC-based VLAN and IP-Subnet-based VLAN functions per-block based.
 *      (2) The corresponding block must be in IP-Subnet-based VLAN mode before calling this function,
 *          refer to rtk_vlan_igrVlanCnvtBlkMode_set.
 * Changes:
 *      None
 */

extern int32
rtk_vlan_ipSubnetBasedVlanWithPort_set(
    uint32      unit,
    uint32      index,
    uint32      valid,
    ipaddr_t    sip,
    ipaddr_t    sip_mask,
    rtk_port_t  port,
    rtk_port_t  port_mask,
    rtk_vlan_t  vid,
    rtk_pri_t   priority);

/* Function Name:
 *      rtk_vlan_portIpSubnetBasedVlanEnable_get
 * Description:
 *      Get enable status of IPSubet-based VLAN on specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to enable status of IPSubet-based VLAN
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - the module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_vlan_portIpSubnetBasedVlanEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_vlan_portIpSubnetBasedVlanEnable_set
 * Description:
 *      Set enable status of IPSubet-based VLAN on specified port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - enable status of IPSubet-based VLAN
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - the module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_vlan_portIpSubnetBasedVlanEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      rtk_vlan_ipSubnetBasedVlanEntry_get
 * Description:
 *      Get IP-Subnet-based vlan.
 * Input:
 *      unit   - unit id
 *      index  - entry index
 * Output:
 *      pEntry - pointer buffer of ipSubnet-based VLAN entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID              - invalid unit id
 *      RT_ERR_NOT_INIT             - the module is not initial
 *      RT_ERR_OUT_OF_RANGE         - index is out of range
 *      RT_ERR_VLAN_C2SC_BLOCK_MODE - try to set/get a entry to an incompatiable block
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) Totally 1024(9300),2048(9310) entries are supported but the entries are shared by ingress VLAN
 *          conversion, MAC-based VLAN and IP-Subnet-based VLAN functions per-block based.
 *      (2) The corresponding block must be in IP-Subnet-based VLAN mode before calling this function,
 *          refer to rtk_vlan_igrVlanCnvtBlkMode_set.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_vlan_ipSubnetBasedVlanEntry_get(uint32 unit, uint32 index, rtk_vlan_ipSubnetVlanEntry_t *pEntry);

/* Function Name:
 *      rtk_vlan_ipSubnetBasedVlanEntry_set
 * Description:
 *      Set IP-Subnet-based vlan.
 * Input:
 *      unit   - unit id
 *      index  - entry index
 *      pEntry - pointer buffer of ipSubnet-based VLAN entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID              - invalid unit id
 *      RT_ERR_NOT_INIT             - the module is not initial
 *      RT_ERR_OUT_OF_RANGE         - index is out of range
 *      RT_ERR_VLAN_C2SC_BLOCK_MODE - try to set/get a entry to an incompatiable block
 *      RT_ERR_VLAN_VID             - invalid vlan id
 *      RT_ERR_PRIORITY             - invalid priority value
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) Totally 1024(9300),2048(9310) entries are supported but the entries are shared by ingress VLAN
 *          conversion, MAC-based VLAN and IP-Subnet-based VLAN functions per-block based.
 *      (2) The corresponding block must be in IP-Subnet-based VLAN mode before calling this function,
 *          refer to rtk_vlan_igrVlanCnvtBlkMode_set.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_vlan_ipSubnetBasedVlanEntry_set(uint32 unit, uint32 index, rtk_vlan_ipSubnetVlanEntry_t *pEntry);

/* Function Name:
 *      rtk_vlan_ipSubnetBasedVlanEntry_add
 * Description:
 *      add an IP-subnet-based VLAN entry
 * Input:
 *      unit   - unit id
 *      pEntry - pointer buffer of ipSubnet-based VLAN entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - the module is not initial
 *      RT_ERR_TBL_FULL - the table is full
 *      RT_ERR_MAC      - invalid mac address
 *      RT_ERR_VLAN_VID - invalid vlan id
 *      RT_ERR_PRIORITY - invalid priority value
 * Applicable:
 *      9310
 * Note:
 *      1. Cannot update the entry
 *      2. Valid bit will be always true
 *      3. Allow valid netmask only
 * Changes:
 *      [SDK_3.7.0]
 *          New added function.
 */
extern int32
rtk_vlan_ipSubnetBasedVlanEntry_add(uint32 unit, rtk_vlan_ipSubnetVlanEntry_t *pEntry);

/* Function Name:
 *      rtk_vlan_ipSubnetBasedVlanEntry_del
 * Description:
 *      delete an IP-subnet-based VLAN entry
 * Input:
 *      unit   - unit id
 *      pEntry - pointer buffer of Mac-based VLAN entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_NOT_INIT       - the module is not initial
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.7.0]
 *          New added function.
 */
extern int32
rtk_vlan_ipSubnetBasedVlanEntry_del(uint32 unit, rtk_vlan_ipSubnetVlanEntry_t *pEntry);


/* Module Name    : Vlan               */
/* Sub-module Name: TPID configuration */

/* Function Name:
 *      rtk_vlan_tpidEntry_get
 * Description:
 *      Get TPID value from global  TPID pool.
 * Input:
 *      unit     - unit id
 *      type     - vlan tag type
 *      tpid_idx - index of TPID entry
 * Output:
 *      pTpid    - pointer to TPID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Global four inner TPID can be specified.
 *      Global four outer TPID can be specified
 *      Global one extra TPID can be specified
 * Changes:
 *      [SDK_3.0.0]
 *          (1)New added function.
 *          (2)It obsoletes these functions:
 *              rtk_vlan_extraTpidEntry_get
 *              rtk_vlan_innerTpidEntry_get
 *              rtk_vlan_outerTpidEntry_get
 */
extern int32
rtk_vlan_tpidEntry_get(uint32 unit, rtk_vlan_tagType_t type, uint32 tpid_idx, uint32 *pTpid);

/* Function Name:
 *      rtk_vlan_tpidEntry_set
 * Description:
 *      Set TPID value to global TPID pool.
 * Input:
 *      unit     - unit id
 *      type     - vlan tag type
 *      tpid_idx - index of TPID entry
 *      tpid     - TPID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Global four inner TPID can be specified.
 *      Global four outer TPID can be specified
 *      Global one extra TPID can be specified
 * Changes:
 *      [SDK_3.0.0]
 *          (1)New added function.
 *          (2)It obsoletes these functions:
 *              rtk_vlan_extraTpidEntry_set
 *              rtk_vlan_innerTpidEntry_set
 *              rtk_vlan_outerTpidEntry_set
 */
extern int32
rtk_vlan_tpidEntry_set(uint32 unit, rtk_vlan_tagType_t type, uint32 tpid_idx, uint32 tpid);

/* Function Name:
 *      rtk_vlan_portIgrTpid_get
 * Description:
 *      Get inner/outer TPIDs comparsion configration on specified port.
 * Input:
 *      unit           - unit id
 *      port           - port id
 *      type           - vlan type
 * Output:
 *      pTpid_idx_mask - pointer to mask for index of tpid entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Specify which TPID to compare from TPID pool for parsing a inner/outer-tagged packet.
 *      The valid mask bits of tpid_idx is bit[3:0].
 * Changes:
 *      [SDK_3.0.0]
 *          (1)New added function.
 *          (2)It obsoletes these functions:
 *              rtk_vlan_portIgrInnerTpid_get
 *              rtk_vlan_portIgrOuterTpid_get
 */
extern int32
rtk_vlan_portIgrTpid_get(uint32 unit, rtk_port_t port, rtk_vlanType_t type, uint32 *pTpid_idx_mask);

/* Function Name:
 *      rtk_vlan_portIgrTpid_set
 * Description:
 *      Set inner/outer TPIDs comparsion configration on specified port
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      type          - vlan type
 *      tpid_idx_mask - mask for index of tpid entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - the module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Specify which TPID to compare from TPID pool for parsing a inner/outer-tagged packet.
 *      The valid mask bits of tpid_idx is bit[3:0].
 * Changes:
 *      [SDK_3.0.0]
 *          (1)New added function.
 *          (2)It obsoletes these functions:
 *              rtk_vlan_portIgrInnerTpid_set
 *              rtk_vlan_portIgrOuterTpid_set
 */
extern int32
rtk_vlan_portIgrTpid_set(uint32 unit, rtk_port_t port, rtk_vlanType_t type, uint32 tpid_idx_mask);

/* Function Name:
 *      rtk_vlan_portEgrTpid_get
 * Description:
 *      Get inner/outer TPID for inner/outer tag encapsulation when transmiiting a inner/outer-tagged pacekt.
 * Input:
 *      unit      - unit id
 *      port      - port id
 * Output:
 *      pTpid_idx - pointer to index of inner TPID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The valid range of tpid_idx is 0~3.
 *      (2) For 8380/8390,The inner/outer TPID is used when transmiiting a inner/outer-tagged packet
 *          only if it is received as an inner/outer-untag pacekt
 *      (3) For 9300/9310,Refer the API rtk_vlan_portEgrTpidSrc_get/set
 * Changes:
 *      [SDK_3.0.0]
 *          (1)New added function.
 *          (2)It obsoletes these functions:
 *              rtk_vlan_portEgrInnerTpid_get
 *              rtk_vlan_portEgrOuterTpid_get
 */
extern int32
rtk_vlan_portEgrTpid_get(uint32 unit, rtk_port_t port, rtk_vlanType_t type, uint32 *pTpid_idx);

/* Function Name:
 *      rtk_vlan_portEgrTpid_set
 * Description:
 *      Set inner/outer TPID for inner/outer tag encapsulation when transmiiting a inner/outer-tagged pacekt.
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      tpid_idx - index of inner TPID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The valid range of tpid_idx is 0~3.
 *      (2) For 8380/8390,The inner/outer TPID is used when transmiiting a inner/outer-tagged packet
 *          only if it is received as an inner/outer-untag pacekt
 *      (3) For 9300/9310,Refer the API rtk_vlan_portEgrTpidSrc_get/set
 * Changes:
 *      [SDK_3.0.0]
 *          (1)New added function.
 *          (2)It obsoletes these functions:
 *              rtk_vlan_portEgrInnerTpid_set
 *              rtk_vlan_portEgrOuterTpid_set
 */
extern int32
rtk_vlan_portEgrTpid_set(uint32 unit, rtk_port_t port, rtk_vlanType_t type, uint32 tpid_idx);

/* Function Name:
 *      rtk_vlan_portEgrTpidSrc_get
 * Description:
 *      Get source of Inner/Outer TPID at egress.
 * Input:
 *      unit      - unit id
 *      port      - port id
 * Output:
 *      pTpid_src - pointer to inner/outer TPID src at egress
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      The TPID_SRC_FROM_ORIG_INNER/OUTER work only for inner/outer tagged packet.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_vlan_portEgrTpidSrc_get(uint32 unit, rtk_port_t port, rtk_vlanType_t type, rtk_vlan_egrTpidSrc_t *pTpid_src);

/* Function Name:
 *      rtk_vlan_portEgrTpidSrc_set
 * Description:
 *      Set Inner/Outer TPID Source at egress.
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      tpid_src - Inner/Outer TPID src at egress
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - the module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      9300, 9310
 * Note:
 *      The TPID_SRC_FROM_ORIG_INNER/OUTER work only for inner/outer tagged packet.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_vlan_portEgrTpidSrc_set(uint32 unit, rtk_port_t port, rtk_vlanType_t type, rtk_vlan_egrTpidSrc_t tpid_src);

/* Module Name    : Vlan                 */
/* Sub-module Name: Ingress tag handling */

/* Module Name    : Vlan                */
/* Sub-module Name: Egress tag handling */

/* Function Name:
 *      rtk_vlan_portIgrExtraTagEnable_get
 * Description:
 *      Get enable status of recognizing extra tag at ingress.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to enable status of recognizing extra tag
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_vlan_portIgrExtraTagEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_vlan_portIgrExtraTagEnable_set
 * Description:
 *      Set enable status of recognizing extra tag at ingress.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - enable status of recognizing extra tag
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - the module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_vlan_portIgrExtraTagEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      rtk_vlan_portEgrTagSts_get
 * Description:
 *      Get tag  status of egress port.
 * Input:
 *      unit - unit id
 *      port - port id
 *      type - vlan type
 * Output:
 *      pSts - tag status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      For 8380/8390,Egress packet will send out with tag when following is true.
 *      - tag status is specified to be tagged
 *      - this port is not in VLAN untag set
 * Changes:
 *      [SDK_3.0.0]
 *          (1)New added function.
 *          (2)It obsoletes these functions:
 *              rtk_vlan_portEgrInnerTagSts_get
 *              rtk_vlan_portEgrOuterTagSts_get
 */
extern int32
rtk_vlan_portEgrTagSts_get(uint32 unit, rtk_port_t port, rtk_vlanType_t type, rtk_vlan_tagSts_t *pSts);

/* Function Name:
 *      rtk_vlan_portEgrTagSts_set
 * Description:
 *      Set tag status of egress port.
 * Input:
 *      unit - unit id
 *      port - port id
 *      type - vlan type
 *      sts  - tag status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - the module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      For 8380/8390,Egress packet will send out with tag when following is true.
 *      - tag status is specified to be tagged
 *      - this port is not in VLAN untag set
 * Changes:
 *      [SDK_3.0.0]
 *          (1)New added function.
 *          (2)It obsoletes these functions:
 *              rtk_vlan_portEgrInnerTagSts_set
 *              rtk_vlan_portEgrOuterTagSts_set
 */
extern int32
rtk_vlan_portEgrTagSts_set(uint32 unit, rtk_port_t port, rtk_vlanType_t type,rtk_vlan_tagSts_t sts);

/* Module Name    : Vlan                */
/* Sub-module Name: Tag format handling */

/* Function Name:
 *      rtk_vlan_portIgrVlanTransparentEnable_get
 * Description:
 *      Get enable status of keep tag format at ingress.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      type    - vlan type
 * Output:
 *      pEnable - enable status of keep tag format
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointers
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Packet will be transmitted with original tag format when following condition are true.
 *      - Enable keep tag format at ingress
 *      - Enable keep tag format at egress
 * Changes:
 *      [SDK_3.0.0]
 *          (1)New added function.
 *          (2)It obsoletes these functions:
 *              rtk_vlan_portEgrTagKeepEnable_get
 *              rtk_vlan_portIgrTagKeepEnable_get
 */
extern int32
rtk_vlan_portIgrVlanTransparentEnable_get(uint32 unit, rtk_port_t port, rtk_vlanType_t type, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_vlan_portIgrVlanTransparentEnable_set
 * Description:
 *      Set enable status of keep tag format at ingress.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      type   - vlan type
 *      enable - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - the module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Packet will be transmitted with original tag format when following condition are true.
 *      - Enable keep tag format at ingress
 *      - Enable keep tag format at egress
 * Changes:
 *      [SDK_3.0.0]
 *          (1)New added function.
 *          (2)It obsoletes these functions:
 *              rtk_vlan_portIgrTagKeepEnable_set
 */
extern int32
rtk_vlan_portIgrVlanTransparentEnable_set(uint32 unit, rtk_port_t port, rtk_vlanType_t type, rtk_enable_t enable);

/* Function Name:
 *      rtk_vlan_portEgrVlanTransparentEnable_get
 * Description:
 *      Get enable status of keep tag format at egress.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      type    - vlan type
 * Output:
 *      pEnable - enable status of keep tag format
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Packet will be transmitted with original tag format when following condition are true.
 *      - Enable keep tag format at ingress
 *      - Enable keep tag format at egress
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_vlan_portEgrVlanTransparentEnable_get(uint32 unit, rtk_port_t port, rtk_vlanType_t type, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_vlan_portEgrVlanTransparentEnable_set
 * Description:
 *      Set enable status of keep tag format at egress.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      type   - vlan type
 *      enable - enable stauts
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - the module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Packet will be transmitted with original tag format when following condition are true.
 *      - Enable keep tag format at ingress
 *      - Enable keep tag format at egress
 * Changes:
 *      [SDK_3.0.0]
 *          (1)New added function.
 *          (2)It obsoletes these functions:
 *              rtk_vlan_portEgrTagKeepEnable_set
 */
extern int32
rtk_vlan_portEgrVlanTransparentEnable_set(uint32 unit, rtk_port_t port, rtk_vlanType_t type, rtk_enable_t enable);

/*
 * MISC
 */

/* Function Name:
 *      rtk_vlan_igrVlanCnvtBlkMode_get
 * Description:
 *      Get the operation mode of ingress VLAN conversion table block.
 * Input:
 *      unit    - unit id
 *      blk_idx - block index
 * Output:
 *      pMode   - operation mode of ingress VLAN conversion block
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_OUT_OF_RANGE - block index is out of range
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      (1) Valid block index ranges from 0 to 3(8390),0 to 7(9300),0 to 15(9310)..
 *      (2) Ingress VLAN conversion table block can be used for doing ingress VLAN conversion
 *         or MAC-basd VLAN or IP-Subnet-based VLAN.
 * Changes:
 *      None
 */
extern int32
rtk_vlan_igrVlanCnvtBlkMode_get(uint32 unit, uint32 blk_idx, rtk_vlan_igrVlanCnvtBlk_mode_t *pMode);

/* Function Name:
 *      rtk_vlan_igrVlanCnvtBlkMode_set
 * Description:
 *      Set the operation mode of ingress VLAN conversion table block.
 * Input:
 *      unit    - unit id
 *      blk_idx - block index
 *      mode    - operation mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_OUT_OF_RANGE - block index is out of range
 *      RT_ERR_INPUT        - invalid input parameter
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      1. Valid block index ranges from 0 to 7.
 *      2. Ingress VLAN conversion table block can be used for doing ingress VLAN conversion
 *         or MAC-basd VLAN or IP-Subnet-based VLAN.
 *
 *      Limitation for block mode set (Using rtk_vlan_ipSubnetBasedVlanEntry_add /
 *          rtk_vlan_ipSubnetBasedVlanEntry_delete / rtk_vlan_macBasedVlanEntry_add /
 *          rtk_vlan_macBasedVlanEntry_delete):
 *      (a) Cannot modify an in-used block mode
 *      (b) Must allocate a sequential block for IVC / MAC-based / IP-Subnet-based VLAN
 * Changes:
 *      None
 */
extern int32
rtk_vlan_igrVlanCnvtBlkMode_set(uint32 unit, uint32 blk_idx, rtk_vlan_igrVlanCnvtBlk_mode_t mode);

/* Function Name:
 *      rtk_vlan_igrVlanCnvtEntry_get
 * Description:
 *      Get ingress VLAN conversion entry.
 * Input:
 *      unit  - unit id
 *      index - entry index
 * Output:
 *      pData - configuration of ingress VLAN conversion entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID              - invalid unit id
 *      RT_ERR_NOT_INIT             - the module is not initial
 *      RT_ERR_OUT_OF_RANGE         - entry index is out of range
 *      RT_ERR_VLAN_C2SC_BLOCK_MODE - try to set/get a entry to an incompatiable block
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      (1) Totally 1024(8390,9300), 2048(9310) entries are supported but the entries are shared by ingress VLAN
 *          conversion, MAC-based VLAN and IP-Subnet-based VLAN functions per-block based.
 *      (2) The corresponding block must be in ingress VLAN conversion mode before calling this function,
 *          refer to rtk_vlan_igrVlanCnvtBlkMode_set.
 * Changes:
 *      None
 */
extern int32
rtk_vlan_igrVlanCnvtEntry_get(uint32 unit, uint32 index, rtk_vlan_igrVlanCnvtEntry_t *pData);

/* Function Name:
 *      rtk_vlan_igrVlanCnvtEntry_set
 * Description:
 *      Set ingress VLAN conversion entry.
 * Input:
 *      unit  - unit id
 *      index - entry index
 * Output:
 *      pData - configuration of ingress VLAN conversion entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID              - invalid unit id
 *      RT_ERR_NOT_INIT             - the module is not initial
 *      RT_ERR_OUT_OF_RANGE         - entry index is out of range
 *      RT_ERR_VLAN_C2SC_BLOCK_MODE - try to set/get a entry to an incompatiable block
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_PORT_ID              - invalid port id
 *      RT_ERR_QOS_1P_PRIORITY      - Invalid 802.1p priority
 *      RT_ERR_VLAN_TPID_INDEX      - Invalid TPID index
 *      RT_ERR_INPUT                - invalid input parameter
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      (1) Totally  1024(8390,9300), 2048(9310) entries are supported but the entries are shared by ingress VLAN
 *          conversion, MAC-based VLAN and IP-Subnet-based VLAN functions per-block based.
 *      (2) The corresponding block must be in ingress VLAN conversion mode before calling this function,
 *          refer to rtk_vlan_igrVlanCnvtBlkMode_set.
 * Changes:
 *      None
 */
extern int32
rtk_vlan_igrVlanCnvtEntry_set(uint32 unit, uint32 index, rtk_vlan_igrVlanCnvtEntry_t *pData);

/* Function Name:
 *      rtk_vlan_portIgrVlanCnvtEnable_get
 * Description:
 *      Get enable status of ingress VLAN conversion on specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to enable status of ingress VLAN conversion
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - the module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_vlan_portIgrVlanCnvtEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_vlan_portIgrVlanCnvtEnable_set
 * Description:
 *      Set enable status of ingress VLAN conversion on specified port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - enable status of ingress VLAN conversion
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - the module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_vlan_portIgrVlanCnvtEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      rtk_vlan_egrVlanCnvtDblTagEnable_get
 * Description:
 *      Enable egress VLAN conversion for double(outer+inner) tagged packet.
 * Input:
 *      unit    - unit id
 * Output:
 *      pEnable - pointer to enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390
 * Note:
 *      (1) Enable egress VLAN conversion for double tagged packet.
 *      (2) If the function is disabled, egress VLAN conversion only applies to single tagged packet.
 * Changes:
 *      None
 */
extern int32
rtk_vlan_egrVlanCnvtDblTagEnable_get(uint32 unit, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_vlan_egrVlanCnvtDblTagEnable_set
 * Description:
 *      Enable egress VLAN conversion for double(outer+inner) tagged packet.
 * Input:
 *      unit   - unit id
 *      enable - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - the module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8390
 * Note:
 *      (1) Enable egress VLAN conversion for double tagged packet.
 *      (2) If the function is disabled, egress VLAN conversion only applies to single tagged packet.
 * Changes:
 *      None
 */
extern int32
rtk_vlan_egrVlanCnvtDblTagEnable_set(uint32 unit, rtk_enable_t enable);

/* Function Name:
 *      rtk_vlan_egrVlanCnvtVidSource_get
 * Description:
 *      Specify the VID source for doing egress VLAN conversion.
 * Input:
 *      unit - unit id
 * Output:
 *      pSrc - pointer to VID source
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390
 * Note:
 *      Change to use rtk_vlan_egrVlanCnvtEntry_get(unit, index, pData) for 9300, 9310
 *      (1) VID source can be either from inner tag or outer tag.
 *      (2) The selected VID will be used as comparing key for doing egress VLAN conversion.
 * Changes:
 *      None
 */
extern int32
rtk_vlan_egrVlanCnvtVidSource_get(uint32 unit, rtk_l2_vlanMode_t *pSrc);

/* Function Name:
 *      rtk_vlan_egrVlanCnvtVidSource_set
 * Description:
 *      Specify the VID source for doing egress VLAN conversion.
 * Input:
 *      unit - unit id
 *      src  - VID source
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - the module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8390
 * Note:
 *      Change to use rtk_vlan_egrVlanCnvtEntry_set(unit, index, pData) for 9300, 9310
 *      (1) VID source can be either from inner tag or outer tag.
 *      (2) The selected VID will be used as comparing key for doing egress VLAN conversion.
 * Changes:
 *      None
 */
extern int32
rtk_vlan_egrVlanCnvtVidSource_set(uint32 unit, rtk_l2_vlanMode_t src);

/* Function Name:
 *      rtk_vlan_egrVlanCnvtEntry_get
 * Description:
 *      Set egress VLAN conversion entry.
 * Input:
 *      unit  - unit id
 *      index - entry index
 * Output:
 *      pData - configuration of egress VLAN conversion entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_OUT_OF_RANGE - entry index is out of range
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Totally 1024 entries are supported.
 * Changes:
 *      None
 */
extern int32
rtk_vlan_egrVlanCnvtEntry_get(uint32 unit, uint32 index, rtk_vlan_egrVlanCnvtEntry_t *pData);

/* Function Name:
 *      rtk_vlan_egrVlanCnvtEntry_set
 * Description:
 *      Set egress VLAN conversion entry.
 * Input:
 *      unit  - unit id
 *      index - entry index
 * Output:
 *      pData - configuration of egress VLAN conversion entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID              - invalid unit id
 *      RT_ERR_NOT_INIT             - the module is not initial
 *      RT_ERR_OUT_OF_RANGE         - entry index is out of range
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_PORT_ID              - invalid port id
 *      RT_ERR_QOS_1P_PRIORITY      - Invalid 802.1p priority
 *      RT_ERR_VLAN_TPID_INDEX      - Invalid TPID index
 *      RT_ERR_INPUT                - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Totally 1024 entries are supported.
 * Changes:
 *      None
 */
extern int32
rtk_vlan_egrVlanCnvtEntry_set(uint32 unit, uint32 index, rtk_vlan_egrVlanCnvtEntry_t *pData);

/* Function Name:
 *      rtk_vlan_portEgrVlanCnvtEnable_get
 * Description:
 *      Get enable status of egress VLAN conversion on specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to enable status of egress VLAN conversion
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - the module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_vlan_portEgrVlanCnvtEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_vlan_portEgrVlanCnvtEnable_set
 * Description:
 *      Set enable status of egress VLAN conversion on specified port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - enable status of egress VLAN conversion
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - the module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_vlan_portEgrVlanCnvtEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      rtk_vlan_aggrEnable_get
 * Description:
 *      Enable N:1 VLAN aggregation support.
 * Input:
 *      unit    - unit id
 * Output:
 *      pEnable - pointer to enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      (1) When the N:1 VLAN aggregation function is enabled,
 *          BPE (ECID) function will be unavailable.
 * Changes:
 *      None
 */
extern int32
rtk_vlan_aggrEnable_get(uint32 unit, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_vlan_aggrEnable_set
 * Description:
 *      Enable N:1 VLAN aggregation support.
 * Input:
 *      unit   - unit id
 *      enable - enable status
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
 *      (1) When the N:1 VLAN aggregation function is enabled,
 *          BPE (ECID) function will be unavailable.
 *      (2) While Flex Table is set to L2_TUNNEL,
 *          this function is unavailable (return RT_ERR_FAILED).
 * Changes:
 *      None
 */
extern int32
rtk_vlan_aggrEnable_set(uint32 unit, rtk_enable_t enable);

/* Function Name:
 *      rtk_vlan_portVlanAggrEnable_get
 * Description:
 *      Enable N:1 VLAN aggregation support on egress port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) When the N:1 VLAN aggregation function is enabled on a egress port, the inner VID
 *          of the downstream packet will be replaced by the VID which learnt in L2 table from
 *          upstream packet.
 *      (2) N:1 VLAN replacement only applies to the inner tagged and known unicast packet.
 * Changes:
 *      None
 */
extern int32
rtk_vlan_portVlanAggrEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_vlan_portVlanAggrEnable_set
 * Description:
 *      Enable N:1 VLAN aggregation support on egress port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - the module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) When the N:1 VLAN aggregation function is enabled on a egress port, the inner VID
 *          of the downstream packet will be replaced by the VID which learnt in L2 table from
 *          upstream packet.
 *      (2) N:1 VLAN replacement only applies to the inner tagged and known unicast packet.
 * Changes:
 *      None
 */
extern int32
rtk_vlan_portVlanAggrEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      rtk_vlan_portVlanAggrPriEnable_get
 * Description:
 *      Enable N:1 VLAN-Priority aggregation support on egress port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      (1) When the N:1 VLAN-Priority aggregation function is enabled on a egress port,
 *          the inner or outer priority (selected by rtk_vlan_portVlanAggrCtrl_set() API)
 *          of the downstream packet will be replaced by the priority which learnt in L2 table
 *          from upstream packet.
 *      (2) N:1 VLAN-Priority replacement only applies to the tagged and known unicast packet.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_vlan_portVlanAggrPriEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_vlan_portVlanAggrPriEnable_set
 * Description:
 *      Enable N:1 VLAN-Priority aggregation support on egress port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - the module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      9310
 * Note:
 *      (1) When the N:1 VLAN-Priority aggregation function is enabled on a egress port,
 *          the inner or outer priority (selected by rtk_vlan_portVlanAggrCtrl_set() API)
 *          of the downstream packet will be replaced by the priority which learnt in L2 table
 *          from upstream packet.
 *      (2) N:1 VLAN-Priority replacement only applies to the tagged and known unicast packet.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_vlan_portVlanAggrPriEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      rtk_vlan_portVlanAggrCtrl_get
 * Description:
 *      Get port vlan-aggragation configration.
 * Input:
 *      unit          - unit id
 *      port          - port id
 * Output:
 *      pVlanAggrCtrl - pointer to vlan-aggr ctrl
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          (1)New added function.
 *          (2)It obsoletes these functions:
 *              rtk_vlan_portVlanAggrPriTagVidSource_get
 *              rtk_vlan_portVlanAggrVidSource_get
 */
extern int32
rtk_vlan_portVlanAggrCtrl_get(uint32 unit, rtk_port_t port, rtk_vlan_aggrCtrl_t *pVlanAggrCtrl);

/* Function Name:
 *      rtk_vlan_portVlanAggrCtrl_set
 * Description:
 *      Set port vlan aggragation .
 * Input:
 *      unit         - unit id
 *      port         - port id
 *      vlanAggrCtrl - vlan-aggr ctrl
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - the module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          (1)New added function.
 *          (2)It obsoletes these functions:
 *              rtk_vlan_portVlanAggrPriTagVidSource_set
 *              rtk_vlan_portVlanAggrVidSource_set
 */
extern int32
rtk_vlan_portVlanAggrCtrl_set(uint32 unit, rtk_port_t port, rtk_vlan_aggrCtrl_t vlanAggrCtrl);
/* Function Name:
 *      rtk_vlan_leakyStpFilter_get
 * Description:
 *      Get leaky STP filter status for multicast leaky is enabled.
 * Input:
 *      unit    - unit id
 * Output:
 *      pEnable - pointer to status of leaky STP filter
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_vlan_leakyStpFilter_get(uint32 unit, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_vlan_leakyStpFilter_set
 * Description:
 *      Set leaky STP filter status for multicast leaky is enabled.
 * Input:
 *      unit   - unit id
 *      enable - status of leaky STP filter
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - the module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_vlan_leakyStpFilter_set(uint32 unit, rtk_enable_t enable);

/* Function Name:
 *      rtk_vlan_except_get
 * Description:
 *      Get VLAN except action.
 * Input:
 *      unit    - unit id
 * Output:
 *      pAction - pointer to action of VLAN except
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390
 * Note:
 *      (1) VLAN exception is asserted if
 *          received packet with VID=4095 or its VLAN member=0.
 *      (2) Forwarding action is as following
 *          - ACTION_DROP
 *          - ACTION_FORWARD
 *          - ACTION_TRAP2CPU
 * Changes:
 *      None
 */
extern int32
rtk_vlan_except_get(uint32 unit, rtk_action_t *pAction);

/* Function Name:
 *      rtk_vlan_except_set
 * Description:
 *      Set VLAN except action.
 * Input:
 *      unit   - unit id
 *      action - action of VLAN except
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - the module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8390
 * Note:
 *      (1) VLAN exception is asserted if
 *          received packet with VID=4095 or its VLAN member=0.
 *      (2) Forwarding action is as following
 *          - ACTION_DROP
 *          - ACTION_FORWARD
 *          - ACTION_TRAP2CPU
 * Changes:
 *      None
 */
extern int32
rtk_vlan_except_set(uint32 unit, rtk_action_t action);

/* Function Name:
 *      rtk_vlan_portIgrCnvtDfltAct_get
 * Description:
 *      Get action for packet that doesn't hit ingress ACL and C2SC.
 * Input:
 *      unit    - unit id
 *      port    - port id for configure
 * Output:
 *      pAction - pointer to VLAN conversion default action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390
 * Note:
 *      Forwarding action is as following
 *      - ACTION_DROP
 *      - ACTION_FORWARD
 * Changes:
 *      None
 */
extern int32
rtk_vlan_portIgrCnvtDfltAct_get(uint32 unit, rtk_port_t port, rtk_action_t *pAction);

/* Function Name:
 *      rtk_vlan_portIgrCnvtDfltAct_set
 * Description:
 *      Set action for packet that doesn't hit ingress ACL and C2SC.
 * Input:
 *      unit   - unit id
 *      port   - port id for configure
 *      action - VLAN conversion default action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - the module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8390
 * Note:
 *      Forwarding action is as following
 *      - ACTION_DROP
 *      - ACTION_FORWARD
 * Changes:
 *      None
 */
extern int32
rtk_vlan_portIgrCnvtDfltAct_set(uint32 unit, rtk_port_t port, rtk_action_t action);

/* Function Name:
 *      rtk_vlan_portIgrVlanCnvtLuMisAct_get
 * Description:
 *      Get action for packet that doesn't hit any Ingress VLAN Conversion (IVC) entry.
 * Input:
 *      unit    - unit id
 *      port    - port id for configure
 *      type    - vlan type
 * Output:
 *      pAction - pointer to VLAN conversion default action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      1) Forwarding action is as following
 *      - ACTION_DROP
 *      - ACTION_FORWARD
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_vlan_portIgrVlanCnvtLuMisAct_get(uint32 unit, rtk_port_t port, rtk_vlanType_t type, rtk_vlan_lookupMissAct_t *pAction);

/* Function Name:
 *      rtk_vlan_portIgrVlanCnvtLuMisAct_set
 * Description:
 *      Set action for packet that doesn't hit Ingress VLAN Conversion (IVC) entry.
 * Input:
 *      unit   - unit id
 *      port   - port id for configure
 *      type   - vlan type
 *      action - VLAN conversion default action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - the module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      9300, 9310
 * Note:
 *      1) Forwarding action is as following
 *      - ACTION_DROP
 *      - ACTION_FORWARD
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_vlan_portIgrVlanCnvtLuMisAct_set(uint32 unit, rtk_port_t port, rtk_vlanType_t type, rtk_vlan_lookupMissAct_t action);

/* Function Name:
 *      rtk_vlan_portEgrVlanCnvtLuMisAct_get
 * Description:
 *      Get action for packet that doesn't hit any Egress VLAN Conversion (EVC) entry.
 * Input:
 *      unit    - unit id
 *      port    - port id for configure
 *      type    - vlan type
 * Output:
 *      pAction - pointer to VLAN conversion default action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 9300, 9310
 * Note:
 *      1) Forwarding action is as following
 *      - ACTION_DROP
 *      - ACTION_FORWARD
 * Changes:
 *      [SDK_3.0.0]
 *          (1)New added function.
 *          (2)It obsoletes these functions:
 *              rtk_vlan_portEgrVlanCnvtLookupMissAct_get
 */
extern int32
rtk_vlan_portEgrVlanCnvtLuMisAct_get(uint32 unit, rtk_port_t port, rtk_vlanType_t type, rtk_vlan_lookupMissAct_t *pAction);

/* Function Name:
 *      rtk_vlan_portEgrVlanCnvtLuMisAct_set
 * Description:
 *      Set action for packet that doesn't hit Egress VLAN Conversion (EVC) entry.
 * Input:
 *      unit   - unit id
 *      port   - port id for configure
 *      type   - vlan type
 *      action - VLAN conversion default action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - the module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 9300, 9310
 * Note:
 *      2) Forwarding action is as following
 *      - ACTION_DROP
 *      - ACTION_FORWARD
 * Changes:
 *      [SDK_3.0.0]
 *          (1)New added function.
 *          (2)It obsoletes these functions:
 *              rtk_vlan_portEgrVlanCnvtLookupMissAct_set
 */
extern int32
rtk_vlan_portEgrVlanCnvtLuMisAct_set(uint32 unit, rtk_port_t port, rtk_vlanType_t type, rtk_vlan_lookupMissAct_t action);

/* Function Name:
 *      rtk_vlan_portIgrTagKeepType_get
 * Description:
 *      Get keep type of keep tag format at igress.
 * Input:
 *      unit           - unit id
 *      port           - port id
 * Output:
 *      pKeeptypeOuter - pointer to enable status of keep outer tag format
 *      pKeeptypeInner - pointer to enable status of keep inner tag format
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380
 * Note:
 *      Packet will be transmitted with original tag format when following condition are true.
 *      - Enable keep tag format at ingress
 *      - Enable keep tag format at egress
 * Changes:
 *      None
 */
extern int32
rtk_vlan_portIgrTagKeepType_get(
    uint32                  unit,
    rtk_port_t              port,
    rtk_vlan_tagKeepType_t  *pKeeptypeOuter,
    rtk_vlan_tagKeepType_t  *pKeeptypeInner);

/* Function Name:
 *      rtk_vlan_portIgrTagKeepType_set
 * Description:
 *      Set keep type of keep tag format at igress.
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      keeptypeOuter - keep type of keep outer tag format
 *      keeptypeInner - keep type of keep inner tag format
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - the module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380
 * Note:
 *      Packet will be transmitted with original tag format when following condition are true.
 *      - Set keep type of keep tag format at ingress
 *      - Set keep type of keep tag format at egress
 * Changes:
 *      None
 */
extern int32
rtk_vlan_portIgrTagKeepType_set(
    uint32                  unit,
    rtk_port_t              port,
    rtk_vlan_tagKeepType_t  keeptypeOuter,
    rtk_vlan_tagKeepType_t  keeptypeInner);

/* Function Name:
 *      rtk_vlan_portEgrTagKeepType_get
 * Description:
 *      Get keep type of keep tag format at egress.
 * Input:
 *      unit           - unit id
 *      port           - port id
 * Output:
 *      pKeeptypeOuter - pointer to enable status of keep outer tag format
 *      pKeeptypeInner - pointer to enable status of keep inner tag format
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380
 * Note:
 *      Packet will be transmitted with original tag format when following condition are true.
 *      - Enable keep tag format at ingress
 *      - Enable keep tag format at egress
 * Changes:
 *      None
 */
extern int32
rtk_vlan_portEgrTagKeepType_get(
    uint32                  unit,
    rtk_port_t              port,
    rtk_vlan_tagKeepType_t  *pKeeptypeOuter,
    rtk_vlan_tagKeepType_t  *pKeeptypeInner);

/* Function Name:
 *      rtk_vlan_portEgrTagKeepType_set
 * Description:
 *      Set keep type of keep tag format at egress.
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      keeptypeOuter - keep type of keep outer tag format
 *      keeptypeInner - keep type of keep inner tag format
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - the module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380
 * Note:
 *      Packet will be transmitted with original tag format when following condition are true.
 *      - Set keep type of keep tag format at ingress
 *      - Set keep type of keep tag format at egress
 * Changes:
 *      None
 */
extern int32
rtk_vlan_portEgrTagKeepType_set(
    uint32                  unit,
    rtk_port_t              port,
    rtk_vlan_tagKeepType_t  keeptypeOuter,
    rtk_vlan_tagKeepType_t  keeptypeInner);


/* Function Name:
 *      rtk_vlan_portEgrVlanCnvtVidSource_get
 * Description:
 *      Specify the VID source for doing egress VLAN conversion.
 * Input:
 *      unit - unit id
 * Output:
 *      pSrc - pointer to VID source
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8390
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_vlan_portEgrVlanCnvtVidSource_get(uint32 unit, rtk_port_t port, rtk_l2_vlanMode_t *pSrc);

/* Function Name:
 *      rtk_vlan_portEgrVlanCnvtVidSource_set
 * Description:
 *      Specify the VID source for doing egress VLAN conversion.
 * Input:
 *      unit - unit id
 *      src  - VID source
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - the module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_vlan_portEgrVlanCnvtVidSource_set(uint32 unit, rtk_port_t port, rtk_l2_vlanMode_t src);

/* Function Name:
 *      rtk_vlan_portEgrVlanCnvtVidTarget_get
 * Description:
 *      Specify the VID target for doing egress VLAN conversion.
 * Input:
 *      unit - unit id
 * Output:
 *      pTgt - pointer to VID source
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_vlan_portEgrVlanCnvtVidTarget_get(uint32 unit, rtk_port_t port, rtk_l2_vlanMode_t *pTgt);

/* Function Name:
 *      rtk_vlan_portEgrVlanCnvtVidTarget_set
 * Description:
 *      Specify the VID target for doing egress VLAN conversion.
 * Input:
 *      unit - unit id
 *      tgt  - VID source
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - the module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_vlan_portEgrVlanCnvtVidTarget_set(uint32 unit, rtk_port_t port, rtk_l2_vlanMode_t tgt);

/* Function Name:
 *      rtk_vlan_igrVlanCnvtHitIndication_get
 * Description:
 *      Get Ingress VLAN Conversion entry hit indication.
 * Input:
 *      unit      - unit id
 *      entry_idx - IVC entry index
 *      flag      - hit indication flag(ex,reset the hit status)
 * Output:
 *      pIsHit    - pointer to hit status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      The hit status can be cleared by configuring para. reset 'flag' to 1.
 * Changes:
 *      None
 */
extern int32
rtk_vlan_igrVlanCnvtHitIndication_get(uint32 unit, uint32 entry_idx, uint32 flag, uint32 *pIsHit);

/* Function Name:
 *      rtk_vlan_egrVlanCnvtHitIndication_get
 * Description:
 *      Get Egress VLAN Conversion entry hit indication.
 * Input:
 *      unit      - unit id
 *      entry_idx - EVC entry indexs
 *      flag      - hit indication flag(ex,reset the hit status)
 * Output:
 *      pIsHit    - pointer to hit status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      The hit status can be cleared by configuring para reset 'flag' to 1.
 * Changes:
 *      None
 */
extern int32
rtk_vlan_egrVlanCnvtHitIndication_get(uint32 unit, uint32 entry_idx, uint32 flag, uint32 *pIsHit);

/* Function Name:
 *      rtk_vlan_igrVlanCnvtEntry_delAll
 * Description:
 *      Delete all ingress VLAN conversion entry.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - the module is not initial
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      All ingress VLAN conversion entries are removed regardless of the operation mode.
 * Changes:
 *      None
 */
extern int32
rtk_vlan_igrVlanCnvtEntry_delAll(uint32 unit);

/* Function Name:
 *      rtk_vlan_egrVlanCnvtEntry_delAll
 * Description:
 *      Delete all egress VLAN conversion entry.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - the module is not initial
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_vlan_egrVlanCnvtEntry_delAll(uint32 unit);

/* Function Name:
 *      rtk_vlan_portIgrVlanCnvtRangeCheckSet_get
 * Description:
 *      Get the VID range check Set for ingress VLAN conversion which the specified port refered.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pSetIdx - pointer to evc vlan range check set
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      9300 support 2 sets of IVC VID range check table,9310 support 4 sets,
 *      each set has 32 vlan range check vid entries
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_vlan_portIgrVlanCnvtRangeCheckSet_get(uint32 unit, rtk_port_t port, uint32 *pSetIdx);

/* Function Name:
 *      rtk_vlan_portIgrVlanCnvtRangeCheckSet_set
 * Description:
 *      configure the VID range check Set  for ingress VLAN conversion which the specified port refered.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      setIdx - ivc vlan range check set index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      9300 support 2 sets of IVC VID range check table,9310 support 4 sets,
 *      each set has 32 vlan range check vid entries
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_vlan_portIgrVlanCnvtRangeCheckSet_set(uint32 unit, rtk_port_t port, uint32 setIdx);

/* Function Name:
 *      rtk_vlan_igrVlanCnvtRangeCheckEntry_get
 * Description:
 *      Get the configuration of VID range check for ingress VLAN conversion.
 * Input:
 *      unit   - unit id
 *      setIdx - vlan range check set index
 *      index  - entry index
 * Output:
 *      pData  - configuration of VID Range
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_vlan_igrVlanCnvtRangeCheckEntry_get(uint32 unit, uint32 setIdx, uint32 index, rtk_vlan_vlanCnvtRangeCheck_vid_t *pData);

/* Function Name:
 *      rtk_vlan_igrVlanCnvtRangeCheckEntry_set
 * Description:
 *      Set the configuration of VID range check for ingress VLAN conversion.
 * Input:
 *      unit   - unit id
 *      setIdx - vlan range check set index
 *      index  - entry index
 *      pData  - configuration of VID Range
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_vlan_igrVlanCnvtRangeCheckEntry_set(uint32 unit, uint32 setIdx, uint32 index, rtk_vlan_vlanCnvtRangeCheck_vid_t *pData);

/* Function Name:
 *      rtk_vlan_portEgrVlanCnvtRangeCheckSet_get
 * Description:
 *      Get the VID range check Set for egress VLAN conversion which the specified port refered.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pSetIdx - pointer to evc vlan range check set
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      9300 support 2 sets of EVC VID range check table,9310 support 4 sets,
 *      each set has 32 vlan range check vid entries
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_vlan_portEgrVlanCnvtRangeCheckSet_get(uint32 unit, rtk_port_t port, uint32 *pSetIdx);

/* Function Name:
 *      rtk_vlan_portEgrVlanCnvtRangeCheckSet_set
 * Description:
 *      configure the VID range check Set for egress VLAN conversion which the specified port refered.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      setIdx - evc vlan range check set index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      9300 support 2 sets of EVC VID range check table,9310 support 4 sets,
 *      each set has 32 vlan range check vid entries
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_vlan_portEgrVlanCnvtRangeCheckSet_set(uint32 unit, rtk_port_t port, uint32 setIdx);

/* Function Name:
 *      rtk_vlan_egrVlanCnvtRangeCheckEntry_get
 * Description:
 *      Get the configuration of VID range check for egress VLAN conversion.
 * Input:
 *      unit   - unit id
 *      setIdx - vlan range check set index
 *      index  - entry index
 * Output:
 *      pData  - configuration of VID Range
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          (1)New added function.
 *          (2)It obsoletes these functions:
 *              rtk_vlan_egrVlanCnvtRangeCheckVid_get
 */
extern int32
rtk_vlan_egrVlanCnvtRangeCheckEntry_get(uint32 unit, uint32 setIdx, uint32 index, rtk_vlan_vlanCnvtRangeCheck_vid_t *pData);

/* Function Name:
 *      rtk_vlan_egrVlanCnvtRangeCheckEntry_set
 * Description:
 *      Set the configuration of VID range check for egress VLAN conversion.
 * Input:
 *      unit   - unit id
 *      setIdx - vlan range check set index
 *      index  - entry index
 *      pData  - configuration of VID Range
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          (1)New added function.
 *          (2)It obsoletes these functions:
 *              rtk_vlan_egrVlanCnvtRangeCheckVid_set
 */
extern int32
rtk_vlan_egrVlanCnvtRangeCheckEntry_set(uint32 unit, uint32 setIdx, uint32 index, rtk_vlan_vlanCnvtRangeCheck_vid_t *pData);

#ifdef CONFIG_SDK_DRIVER_RTK_LEGACY_API
/* Function Name:
 *      rtk_vlan_portVlanAggrVidSource_get
 * Description:
 *      Get port ingress learning int address table and egress conversion VID selection inner VID or outer VID.
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pSrc - pointer to source selection
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_vlan_portVlanAggrCtrl_get(unit, port, pVlanAggrCtrl)
 *          Parameters:
 *              VlanAggrCtrl->vid_src   - *pSrc
 */
extern int32
rtk_vlan_portVlanAggrVidSource_get(uint32 unit, rtk_port_t port, rtk_vlanType_t *pSrc);

/* Function Name:
 *      rtk_vlan_portVlanAggrVidSource_set
 * Description:
 *      Set port ingress learning int address table and egress conversion VID selection inner VID or outer VID.
 * Input:
 *      unit - unit id
 *      port - port id
 *      src  - vlan mode source
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - the module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_vlan_portVlanAggrCtrl_set(unit, port, vlanAggrCtrl)
 *          Parameters:
 *              VlanAggrCtrl.vid_src    - src
 */
extern int32
rtk_vlan_portVlanAggrVidSource_set(uint32 unit, rtk_port_t port, rtk_vlanType_t src);

/* Function Name:
 *      rtk_vlan_portVlanAggrPriTagVidSource_get
 * Description:
 *      Get ingress port priority-tagged packet learning VID.
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pSrc - pointer to priority-tagged packet learning VID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_vlan_portVlanAggrCtrl_get(unit, port, pVlanAggrCtrl)
 *          Parameters:
 *              VlanAggrCtrl->pri_src   - *pSrc
 */
extern int32
rtk_vlan_portVlanAggrPriTagVidSource_get(uint32 unit, rtk_port_t port, rtk_vlan_priTagVidSrc_t *pSrc);

/* Function Name:
 *      rtk_vlan_portVlanAggrPriTagVidSource_set
 * Description:
 *      Set ingress port priority-tagged packet learning VID 0 or port-based VID.
 * Input:
 *      unit - unit id
 *      port - port id
 *      src  - priority-tagged packet learning VID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - the module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_vlan_portVlanAggrCtrl_set(unit, port, vlanAggrCtrl)
 *          Parameters:
 *              VlanAggrCtrl.pri_src    - src
 */
extern int32
rtk_vlan_portVlanAggrPriTagVidSource_set(uint32 unit, rtk_port_t port, rtk_vlan_priTagVidSrc_t src);

/* Function Name:
 *      rtk_vlan_portInnerAcceptFrameType_get
 * Description:
 *      Get vlan accept frame type of the port from the specified device.
 * Input:
 *      unit               - unit id
 *      port               - port id
 * Output:
 *      pAccept_frame_type - pointer buffer of accept frame type
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The accept frame type as following:
 *          - ACCEPT_FRAME_TYPE_ALL
 *          - ACCEPT_FRAME_TYPE_TAG_ONLY
 *          - ACCEPT_FRAME_TYPE_UNTAG_ONLY
 *      (2) The API is used for 802.1Q tagged  and if you want to get the 802.1ad
 *          accept frame type, please use rtk_vlan_portOuterAcceptFrameType_get.
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_vlan_portAcceptFrameType_get(unit, port, type, pAccept_frame_type)
 *          Parameters:
 *              type        - INNER_VLAN
 */
extern int32
rtk_vlan_portInnerAcceptFrameType_get(uint32 unit, rtk_port_t port, rtk_vlan_acceptFrameType_t *pAccept_frame_type);

/* Function Name:
 *      rtk_vlan_portInnerAcceptFrameType_set
 * Description:
 *      Set vlan accept frame type of the port to the specified device.
 * Input:
 *      unit              - unit id
 *      port              - port id
 *      accept_frame_type - accept frame type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID                - invalid unit id
 *      RT_ERR_NOT_INIT               - the module is not initial
 *      RT_ERR_PORT_ID                - invalid port id
 *      RT_ERR_VLAN_ACCEPT_FRAME_TYPE - invalid accept frame type
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The accept frame type as following:
 *          - ACCEPT_FRAME_TYPE_ALL
 *          - ACCEPT_FRAME_TYPE_TAG_ONLY
 *          - ACCEPT_FRAME_TYPE_UNTAG_ONLY
 *      (2) The API is used for 802.1Q tagged  and if you want to set the 802.1ad
 *          accept frame type, please use rtk_vlan_portOuterAcceptFrameType_set.
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_vlan_portAcceptFrameType_set(unit, port, type, accept_frame_type)
 *          Parameters:
 *              type        - INNER_VLAN
 */
extern int32
rtk_vlan_portInnerAcceptFrameType_set(uint32 unit, rtk_port_t port, rtk_vlan_acceptFrameType_t accept_frame_type);

/* Function Name:
 *      rtk_vlan_portOuterAcceptFrameType_get
 * Description:
 *      Get accept frame type of outer tag on specified port.
 * Input:
 *      unit               - unit id
 *      port               - port id
 * Output:
 *      pAccept_frame_type - pointer to accept frame type
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Accept frame type is as following:
 *      - ACCEPT_FRAME_TYPE_ALL
 *      - ACCEPT_FRAME_TYPE_TAG_ONLY
 *      - ACCEPT_FRAME_TYPE_UNTAG_ONLY
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_vlan_portAcceptFrameType_get(unit, port, type, pAccept_frame_type)
 *          Parameters:
 *              type        - OUTER_VLAN
 */
extern int32
rtk_vlan_portOuterAcceptFrameType_get(uint32 unit, rtk_port_t port, rtk_vlan_acceptFrameType_t *pAccept_frame_type);

/* Function Name:
 *      rtk_vlan_portOuterAcceptFrameType_set
 * Description:
 *      Set accept frame type of outer tag on specified port.
 * Input:
 *      unit              - unit id
 *      port              - port id
 *      accept_frame_type - accept frame type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID                - invalid unit id
 *      RT_ERR_NOT_INIT               - the module is not initial
 *      RT_ERR_PORT_ID                - invalid port id
 *      RT_ERR_VLAN_ACCEPT_FRAME_TYPE - invalid accept frame type
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Accept frame type is as following:
 *      - ACCEPT_FRAME_TYPE_ALL
 *      - ACCEPT_FRAME_TYPE_TAG_ONLY
 *      - ACCEPT_FRAME_TYPE_UNTAG_ONLY
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_vlan_portAcceptFrameType_set(unit, port, type, accept_frame_type)
 *          Parameters:
 *              type        - OUTER_VLAN
 */
extern int32
rtk_vlan_portOuterAcceptFrameType_set(uint32 unit, rtk_port_t port, rtk_vlan_acceptFrameType_t accept_frame_type);

/* Function Name:
 *      rtk_vlan_l2UcastLookupMode_get
 * Description:
 *      Get L2 lookup mode for L2 unicast traffic.
 * Input:
 *      unit  - unit id
 *      vid   - vlan id
 * Output:
 *      pMode - lookup mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID              - invalid unit id
 *      RT_ERR_NOT_INIT             - the module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Each VLAN can have its own lookup mode for L2 unicast traffic
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_vlan_l2LookupMode_get(unit, vid, type, *pLookupMode)
 *          Parameters:
 *              type        - VLAN_L2_MAC_TYPE_UC
 *              pLookupMode - pMode
 */
extern int32
rtk_vlan_l2UcastLookupMode_get(uint32 unit, rtk_vlan_t vid, rtk_vlan_l2LookupMode_t *pMode);

/* Function Name:
 *      rtk_vlan_l2UcastLookupMode_set
 * Description:
 *      Set L2 lookup mode for L2 unicast traffic.
 * Input:
 *      unit - unit id
 *      vid  - vlan id
 *      mode - lookup mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID              - invalid unit id
 *      RT_ERR_NOT_INIT             - the module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_INPUT                - Invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Each VLAN can have its own lookup mode for L2 unicast traffic
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_vlan_l2LookupMode_set(unit, vid, type, lookupMode)
 *          Parameters:
 *              type        - VLAN_L2_MAC_TYPE_UC
 *              lookupMode  - mode
 */
extern int32
rtk_vlan_l2UcastLookupMode_set(uint32 unit, rtk_vlan_t vid, rtk_vlan_l2LookupMode_t mode);

/* Function Name:
 *      rtk_vlan_l2McastLookupMode_get
 * Description:
 *      Get L2 lookup mode for L2 multicast and IP multicast traffic.
 * Input:
 *      unit  - unit id
 *      vid   - vlan id
 * Output:
 *      pMode - lookup mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID              - invalid unit id
 *      RT_ERR_NOT_INIT             - the module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Each VLAN can have its own lookup mode for L2 and IP mulicast traffic.
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_vlan_l2LookupMode_get(unit, vid, type, *pLookupMode)
 *          Parameters:
 *              type        - VLAN_L2_MAC_TYPE_MC
 *              pLookupMode - pMode
 */
extern int32
rtk_vlan_l2McastLookupMode_get(uint32 unit, rtk_vlan_t vid, rtk_vlan_l2LookupMode_t *pMode);

/* Function Name:
 *      rtk_vlan_l2McastLookupMode_set
 * Description:
 *      Set L2 lookup mode for L2 multicast and IP multicast traffic.
 * Input:
 *      unit  - unit id
 *      vid   - vlan id
 *      mode  - lookup mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID              - invalid unit id
 *      RT_ERR_NOT_INIT             - the module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_INPUT                - Invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Each VLAN can have its own lookup mode for L2 and IP mulicast traffic.
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_vlan_l2LookupMode_set(unit, vid, type, lookupMode)
 *          Parameters:
 *              type        - VLAN_L2_MAC_TYPE_MC
 *              lookupMode  - mode
 */
extern int32
rtk_vlan_l2McastLookupMode_set(uint32 unit, rtk_vlan_t vid, rtk_vlan_l2LookupMode_t mode);

/* Function Name:
 *      rtk_vlan_innerTpidEntry_get
 * Description:
 *      Get inner TPID value from global inner TPID pool.
 * Input:
 *      unit     - unit id
 *      tpid_idx - index of TPID entry
 * Output:
 *      pTpid    - pointer to TPID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Global four inner TPID can be specified.
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_vlan_tpidEntry_get(unit, type, tpid_idx, *pTpid)
 *          Parameters:
 *              type        - VLAN_TAG_TYPE_INNER
 */
extern int32
rtk_vlan_innerTpidEntry_get(uint32 unit, uint32 tpid_idx, uint32 *pTpid);

/* Function Name:
 *      rtk_vlan_innerTpidEntry_set
 * Description:
 *      Set inner TPID value to global inner TPID pool.
 * Input:
 *      unit     - unit id
 *      tpid_idx - index of TPID entry
 *      tpid     - TPID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Global four inner TPID can be specified.
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_vlan_tpidEntry_set(unit, type, tpid_idx, tpid)
 *          Parameters:
 *              type        - VLAN_TAG_TYPE_INNER
 */
extern int32
rtk_vlan_innerTpidEntry_set(uint32 unit, uint32 tpid_idx, uint32 tpid);

/* Function Name:
 *      rtk_vlan_outerTpidEntry_get
 * Description:
 *      Get outer TPID value from global outer TPID pool.
 * Input:
 *      unit     - unit id
 *      tpid_idx - index of TPID entry
 * Output:
 *      pTpid    - pointer to TPID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Global four outer TPID can be specified.
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_vlan_tpidEntry_get(unit, type, tpid_idx, *pTpid)
 *          Parameters:
 *              type        - VLAN_TAG_TYPE_OUTER
 */
extern int32
rtk_vlan_outerTpidEntry_get(uint32 unit, uint32 tpid_idx, uint32 *pTpid);

/* Function Name:
 *      rtk_vlan_outerTpidEntry_set
 * Description:
 *      Set outer TPID value to global outer TPID pool.
 * Input:
 *      unit     - unit id
 *      tpid_idx - index of TPID entry
 *      tpid     - TPID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Global four outer TPID can be specified.
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_vlan_tpidEntry_set(unit, type, tpid_idx, tpid)
 *          Parameters:
 *              type        - VLAN_TAG_TYPE_OUTER
 */
extern int32
rtk_vlan_outerTpidEntry_set(uint32 unit, uint32 tpid_idx, uint32 tpid);

/* Function Name:
 *      rtk_vlan_extraTpidEntry_get
 * Description:
 *      Get the TPID value of extra tag.
 * Input:
 *      unit     - unit id
 *      tpid_idx - index of TPID entry
 * Output:
 *      pTpid    - pointer to TPID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) Only one extra tag is supported. System will bypass extra tag
 *          and keeps parsing the remaining payload.
 *      (2) Following tag combination are supported for parsing an extra tag:
 *          outer+innet+extra, outer+extra, inner+extra
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_vlan_tpidEntry_get(unit, type, tpid_idx, *pTpid)
 *          Parameters:
 *              type        - VLAN_TAG_TYPE_EXTRA
 */
extern int32
rtk_vlan_extraTpidEntry_get(uint32 unit, uint32 tpid_idx, uint32 *pTpid);

/* Function Name:
 *      rtk_vlan_extraTpidEntry_set
 * Description:
 *      Set TPID value of extra tag.
 * Input:
 *      unit     - unit id
 *      tpid_idx - index of TPID entry
 *      tpid     - TPID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) Only one extra tag is supported. System will bypass extra tag
 *          and keeps parsing the remaining payload.
 *      (2) Following tag combination are supported for parsing an extra tag:
 *          outer+innet+extra, outer+extra, inner+extra
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_vlan_tpidEntry_set(unit, type, tpid_idx, tpid)
 *          Parameters:
 *              type        - VLAN_TAG_TYPE_EXTRA
 */
extern int32
rtk_vlan_extraTpidEntry_set(uint32 unit, uint32 tpid_idx, uint32 tpid);

/* Function Name:
 *      rtk_vlan_portInnerPvidMode_get
 * Description:
 *      Get the configuration of inner port-based VLAN mode.
 * Input:
 *      unit  - unit id
 *      port  - port id
 * Output:
 *      pMode - pointer to inner port-based VLAN mode configuration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Inner port-based VLAN can apply on different tag format, refer to enum rtk_vlan_pbVlan_mode_t.
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_vlan_portPvidMode_get(unit, port, type, *pMode)
 *          Parameters:
 *              type        - INNER_VLAN
 */
extern int32
rtk_vlan_portInnerPvidMode_get(uint32 unit, rtk_port_t port, rtk_vlan_pbVlan_mode_t *pMode);

/* Function Name:
 *      rtk_vlan_portInnerPvidMode_set
 * Description:
 *      Set the configuration of inner port-based VLAN mode.
 * Input:
 *      unit - unit id
 *      port - port id
 *      mode - inner port-based VLAN mode configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - the module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Inner port-based VLAN can apply on different tag format, refer to enum rtk_vlan_pbVlan_mode_t.
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_vlan_portPvidMode_set(unit, port, type, mode)
 *          Parameters:
 *              type        - INNER_VLAN
 */
extern int32
rtk_vlan_portInnerPvidMode_set(uint32 unit, rtk_port_t port, rtk_vlan_pbVlan_mode_t mode);

/* Function Name:
 *      rtk_vlan_portOuterPvidMode_get
 * Description:
 *      Get the configuration of outer port-based VLAN mode.
 * Input:
 *      unit  - unit id
 *      port  - port id
 * Output:
 *      pMode - pointer to outer port-based VLAN mode configuration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Outer port-based VLAN can apply on different tag format, refer to enum rtk_vlan_pbVlan_mode_t.
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_vlan_portPvidMode_get(unit, port, type, *pMode)
 *          Parameters:
 *              type      - OUTER_VLAN
 */
extern int32
rtk_vlan_portOuterPvidMode_get(uint32 unit, rtk_port_t port, rtk_vlan_pbVlan_mode_t *pMode);

/* Function Name:
 *      rtk_vlan_portOuterPvidMode_set
 * Description:
 *      Set the configuration of outer port-based VLAN mode.
 * Input:
 *      unit - unit id
 *      port - port id
 *      mode - outer port-based VLAN mode configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - the module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Outer port-based VLAN can apply on different tag format, refer to enum rtk_vlan_pbVlan_mode_t.
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_vlan_portPvidMode_set(unit, port, type, mode)
 *          Parameters:
 *              type        - OUTER_VLAN
 */
extern int32
rtk_vlan_portOuterPvidMode_set(uint32 unit, rtk_port_t port, rtk_vlan_pbVlan_mode_t mode);

/* Function Name:
 *      rtk_vlan_portInnerPvid_get
 * Description:
 *      Get port default vlan id from the specified device.
 * Input:
 *      unit  - unit id
 *      port  - port id
 * Output:
 *      pPvid - pointer buffer of port default vlan id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_vlan_portPvid_get(unit, port, type, *pPvid)
 *          Parameters:
 *              type        - INNER_VLAN
 */
extern int32
rtk_vlan_portInnerPvid_get(uint32 unit, rtk_port_t port, rtk_vlan_t *pPvid);

/* Function Name:
 *      rtk_vlan_portInnerPvid_set
 * Description:
 *      Set port default vlan id to the specified device.
 * Input:
 *      unit - unit id
 *      port - port id
 *      pvid - port default vlan id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - the module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_VLAN_VID - invalid vid
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_vlan_portPvid_set(unit, port,type, pvid)
 *          Parameters:
 *              type    - INNER_VLAN
 */
extern int32
rtk_vlan_portInnerPvid_set(uint32 unit, rtk_port_t port, rtk_vlan_t pvid);

/* Function Name:
 *      rtk_vlan_portOuterPvid_get
 * Description:
 *      Get outer port based vlan on specified port.
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pVid - pointer to outer port based vlan id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_vlan_portPvid_get(unit, port, type, *pPvid)
 *          Parameters:
 *              type        - OUTER_VLAN
 */
extern int32
rtk_vlan_portOuterPvid_get(uint32 unit, rtk_port_t port, rtk_vlan_t *pPvid);

/* Function Name:
 *      rtk_vlan_portOuterPvid_set
 * Description:
 *      Set outer port based vlan on specified port.
 * Input:
 *      unit - unit id
 *      port - port id
 *      vid  - outer port based vlan id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - the module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_VLAN_VID - invalid vlan id
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_vlan_portPvid_set(unit, port,type, pvid)
 *          Parameters:
 *              type        - OUTER_VLAN
 */
extern int32
rtk_vlan_portOuterPvid_set(uint32 unit, rtk_port_t port, rtk_vlan_t pvid);

/* Function Name:
 *      rtk_vlan_portIgrInnerTpid_get
 * Description:
 *      Get inner TPIDs comparsion configration on specified port.
 * Input:
 *      unit           - unit id
 *      port           - port id
 * Output:
 *      pTpid_idx_mask - pointer to mask for index of tpid entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Specify which TPID to compare from TPID pool for parsing a inner-tagged packet.
 *      The valid mask bits of tpid_idx is bit[3:0].
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_vlan_portIgrTpid_get(unit, port, type, *pTpid_idx_mask)
 *          Parameters:
 *              type        - INNER_VLAN
 */
extern int32
rtk_vlan_portIgrInnerTpid_get(uint32 unit, rtk_port_t port, uint32 *pTpid_idx_mask);
/* Function Name:
 *      rtk_vlan_portIgrInnerTpid_set
 * Description:
 *      Set inner TPIDs comparsion configration on specified port
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      tpid_idx_mask - mask for index of tpid entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - the module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Specify which TPID to compare from TPID pool for parsing a inner-tagged packet.
 *      The valid mask bits of tpid_idx is bit[3:0].
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_vlan_portIgrTpid_set(unit, port, type, tpid_idx)
 *          Parameters:
 *              type        - INNER_VLAN
 */
extern int32
rtk_vlan_portIgrInnerTpid_set(uint32 unit, rtk_port_t port, uint32 tpid_idx_mask);

/* Function Name:
 *      rtk_vlan_portIgrOuterTpid_get
 * Description:
 *      Get outer TPIDs comparsion configration on specified port.
 * Input:
 *      unit           - unit id
 *      port           - port id
 * Output:
 *      pTpid_idx_mask - pointer to mask for index of tpid entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Specify which TPID to compare from TPID pool for parsing a outer-tagged packet.
 *      The valid mask bits of tpid_idx is bit[3:0].
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_vlan_portIgrTpid_get(unit, port, type, *pTpid_idx_mask)
 *          Parameters:
 *              type        - OUTER_VLAN
 */
extern int32
rtk_vlan_portIgrOuterTpid_get(uint32 unit, rtk_port_t port, uint32 *pTpid_idx_mask);

/* Function Name:
 *      rtk_vlan_portIgrOuterTpid_set
 * Description:
 *      Set outer TPIDs comparsion configration on specified port.
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      tpid_idx_mask - mask for index of tpid entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - the module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Specify which TPID to compare from TPID pool for parsing a outer-tagged packet.
 *      The valid mask bits of tpid_idx is bit[3:0].
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_vlan_portIgrTpid_set(unit, port, type, tpid_idx)
 *          Parameters:
 *              type        - OUTER_VLAN
 */
extern int32
rtk_vlan_portIgrOuterTpid_set(uint32 unit, rtk_port_t port, uint32 tpid_idx_mask);

/* Function Name:
 *      rtk_vlan_portEgrInnerTpid_get
 * Description:
 *      Get inner TPID for inner tag encapsulation when transmiiting a inner-tagged pacekt.
 * Input:
 *      unit      - unit id
 *      port      - port id
 * Output:
 *      pTpid_idx - pointer to index of inner TPID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The valid range of tpid_idx is 0~3.
 *      (2) The inner TPID is used when transmiiting a inner-tagged pacekt only if it is received as
 *          an inner-untag pacekt.
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_vlan_portEgrTpid_get(unit, port, type, pTpid_idx)
 *          Parameters:
 *              type        - INNER_VLAN
 */
extern int32
rtk_vlan_portEgrInnerTpid_get(uint32 unit, rtk_port_t port, uint32 *pTpid_idx);

/* Function Name:
 *      rtk_vlan_portEgrInnerTpid_set
 * Description:
 *      Set inner TPID for inner tag encapsulation when transmiiting a inner-tagged pacekt.
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      tpid_idx - index of inner TPID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The valid range of tpid_idx is 0~3.
 *      (2) The inner TPID is used when transmiiting a inner-tagged pacekt only if it is received as
 *          an inner-untag pacekt.
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_vlan_portEgrTpid_set(unit, port, type, tpid_idx)
 *          Parameters:
 *              type        - INNER_VLAN
 */
extern int32
rtk_vlan_portEgrInnerTpid_set(uint32 unit, rtk_port_t port, uint32 tpid_idx);

/* Function Name:
 *      rtk_vlan_portEgrOuterTpid_get
 * Description:
 *      Get outer TPID for outer tag encapsulation when transmiiting a outer-tagged pacekt.
 * Input:
 *      unit      - unit id
 *      port      - port id
 * Output:
 *      pTpid_idx - pointer to index of outer TPID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The valid range of tpid_idx is 0~3.
 *      (2) The outer TPID is used when transmiiting a outer-tagged pacekt only if it is received as
 *          an outer-untag pacekt.
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_vlan_portEgrTpid_get(unit, port, type, pTpid_idx)
 *          Parameters:
 *              type        - OUTER_VLAN
 */
extern int32
rtk_vlan_portEgrOuterTpid_get(uint32 unit, rtk_port_t port, uint32 *pTpid_idx);

/* Function Name:
 *      rtk_vlan_portEgrOuterTpid_set
 * Description:
 *      Set outer TPID for outer tag encapsulation when transmiiting a outer-tagged pacekt.
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      tpid_idx - index of outer TPID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      (1) The valid range of tpid_idx is 0~3.
 *      (2) The outer TPID is used when transmiiting a outer-tagged pacekt only if it is received as
 *          an outer-untag pacekt.
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_vlan_portEgrTpid_set(unit, port, type, tpid_idx)
 *          Parameters:
 *              type        - OUTER_VLAN
 */
extern int32
rtk_vlan_portEgrOuterTpid_set(uint32 unit, rtk_port_t port, uint32 tpid_idx);

/* Function Name:
 *      rtk_vlan_portEgrInnerTagSts_get
 * Description:
 *      Set inner tag status of egress port.
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pSts - tag status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Egress packet will send out with inner tag when following is true.
 *      - inner tag status is specified to be tagged
 *      - this port is not in VLAN untag set
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_vlan_portEgrTagSts_get(unit, port, type, *pSts)
 *          Parameters:
 *              type        - INNER_VLAN
 */
extern int32
rtk_vlan_portEgrInnerTagSts_get(uint32 unit, rtk_port_t port, rtk_vlan_tagSts_t *pSts);

/* Function Name:
 *      rtk_vlan_portEgrInnerTagSts_set
 * Description:
 *      Set inner tag status of egress port.
 * Input:
 *      unit - unit id
 *      port - port id
 *      sts  - tag status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - the module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Egress packet will send out with inner tag when following is true.
 *      - inner tag status is specified to be tagged
 *      - this port is not in VLAN untag set
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_vlan_portEgrTagSts_set(unit, port, type, sts)
 *          Parameters:
 *              type        - INNER_VLAN
 */
extern int32
rtk_vlan_portEgrInnerTagSts_set(uint32 unit, rtk_port_t port, rtk_vlan_tagSts_t sts);

/* Function Name:
 *      rtk_vlan_portEgrOuterTagSts_get
 * Description:
 *      Set inner tag status of egress port.
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pSts - tag status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Egress packet will send out with outer tag when following is true.
 *      - outer tag status is specified to be tagged
 *      - this port is not in VLAN untag set
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_vlan_portEgrTagSts_get(unit, port, type, *pSts)
 *          Parameters:
 *              type        - OUTER_VLAN
 */
extern int32
rtk_vlan_portEgrOuterTagSts_get(uint32 unit, rtk_port_t port, rtk_vlan_tagSts_t *pSts);

/* Function Name:
 *      rtk_vlan_portEgrOuterTagSts_set
 * Description:
 *      Set inner tag status of egress port.
 * Input:
 *      unit - unit id
 *      port - port id
 *      sts  - tag status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - the module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Egress packet will send out with outer tag when following is true.
 *      - outer tag status is specified to be tagged
 *      - this port is not in VLAN untag set
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_vlan_portEgrTagSts_set(unit, port, type, sts)
 *          Parameters:
 *              type        - OUTER_VLAN
 */
extern int32
rtk_vlan_portEgrOuterTagSts_set(uint32 unit, rtk_port_t port, rtk_vlan_tagSts_t sts);

/* Function Name:
 *      rtk_vlan_portIgrTagKeepEnable_get
 * Description:
 *      Get enable status of keep tag format at ingress.
 * Input:
 *      unit       - unit id
 *      port       - port id
 * Output:
 *      pKeepOuter - enable status of keep outer tag format
 *      pKeepInner - enable status of keep inner tag format
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointers
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Packet will be transmitted with original tag format when following condition are true.
 *      - Enable keep tag format at ingress
 *      - Enable keep tag format at egress
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_vlan_portIgrVlanTransparentEnable_get(unit, port, type, *pEnable)
 *          Parameters:
 *              type        - INNER_VLAN
 *              pEnable     - pKeepInner
 *          and
 *              type        - OUTER_VLAN
 *              pEnable     - pKeepOuter
 */
extern int32
rtk_vlan_portIgrTagKeepEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pKeepOuter, rtk_enable_t *pKeepInner);

/* Function Name:
 *      rtk_vlan_portIgrTagKeepEnable_set
 * Description:
 *      Set enable status of keep tag format at ingress.
 * Input:
 *      unit      - unit id
 *      port      - port id
 *      keepOuter - enable status of keep outer tag format
 *      keepInner - enable status of keep inner tag format
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - the module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Packet will be transmitted with original tag format when following condition are true.
 *      - Enable keep tag format at ingress
 *      - Enable keep tag format at egress
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_vlan_portIgrVlanTransparentEnable_get(unit, port, type, enable)
 *          Parameters:
 *              type        - INNER_VLAN
 *              enable      - keepInner
 *          and
 *              type        - OUTER_VLAN
 *              enable      - keepOuter
 */
extern int32
rtk_vlan_portIgrTagKeepEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t keepOuter, rtk_enable_t keepInner);

/* Function Name:
 *      rtk_vlan_portEgrTagKeepEnable_get
 * Description:
 *      Get enable status of keep tag format at egress.
 * Input:
 *      unit       - unit id
 *      port       - port id
 * Output:
 *      pKeepOuter - pointer to enable status of keep outer tag format
 *      pKeepInner - pointer to enable status of keep inner tag format
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Packet will be transmitted with original tag format when following condition are true.
 *      - Enable keep tag format at ingress
 *      - Enable keep tag format at egress
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_vlan_portEgrVlanTransparentEnable_get(unit, port, type, *pEnable)
 *          Parameters:
 *              type        - INNER_VLAN
 *              pEnable     - pKeepInner
 *          and
 *              type        - OUTER_VLAN
 *              pEnable     - pKeepOuter
 */
extern int32
rtk_vlan_portEgrTagKeepEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pKeepOuter, rtk_enable_t *pKeepInner);

/* Function Name:
 *      rtk_vlan_portEgrTagKeepEnable_set
 * Description:
 *      Set enable status of keep tag format at egress.
 * Input:
 *      unit      - unit id
 *      port      - port id
 *      keepOuter - enable status of keep outer tag format
 *      keepInner - enable status of keep inner tag format
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - the module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Packet will be transmitted with original tag format when following condition are true.
 *      - Enable keep tag format at ingress
 *      - Enable keep tag format at egress
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_vlan_portEgrVlanTransparentEnable_get(unit, port, type, enable)
 *          Parameters:
 *              type        - INNER_VLAN
 *              enable      - keepInner
 *          and
 *              type        - OUTER_VLAN
 *              enable      - keepOuter
 */
extern int32
rtk_vlan_portEgrTagKeepEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t keepOuter, rtk_enable_t keepInner);

/* Function Name:
 *      rtk_vlan_portEgrVlanCnvtLookupMissAct_get
 * Description:
 *      Specify egress port VID conversion table lookup miss action.
 * Input:
 *      unit - unit id
 * Output:
 *      pAct - pointer to action selection
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_vlan_portEgrVlanCnvtLuMisAct_get(unit, port, type, pAction)
 *          Parameters:
 *              type        - INNER_VLAN/OUTER_VLAN
 *              pAction     - pAct
 */
extern int32
rtk_vlan_portEgrVlanCnvtLookupMissAct_get(uint32 unit, rtk_port_t port, rtk_vlan_lookupMissAct_t *pAct);

/* Function Name:
 *      rtk_vlan_portEgrVlanCnvtLookupMissAct_set
 * Description:
 *      Specify egress port VID conversion table lookup miss action.
 * Input:
 *      unit - unit id
 *      act  - action selection
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - the module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      8380, 9300, 9310
 * Note:
 *      None
 * Changes:
 *          This function will be obsoleted by rtk_vlan_portEgrVlanCnvtLuMisAct_set(unit, port, type, action)
 *          Parameters:
 *              type        - INNER_VLAN/OUTER_VLAN
 *              action      - act
 */
extern int32
rtk_vlan_portEgrVlanCnvtLookupMissAct_set(uint32 unit, rtk_port_t port, rtk_vlan_lookupMissAct_t act);
#endif

/* Function Name:
 *      rtk_vlan_egrVlanCnvtRangeCheckVid_get
 * Description:
 *      Get the configuration of VID range check for egress VLAN conversion.
 * Input:
 *      unit  - unit id
 *      index - entry index
 * Output:
 *      pData - configuration of VID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
*      (1) For 9300 and 9310, the function is backward compatible RTL8380/90 APIs.
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_vlan_egrVlanCnvtRangeCheckEntry_get(unit, setIdx, index, pData)
 *          Parameters:
 *              setIdx      - 0 or 1
 */
extern int32
rtk_vlan_egrVlanCnvtRangeCheckVid_get(uint32 unit, uint32 index, rtk_vlan_vlanCnvtRangeCheck_vid_t *pData);

/* Function Name:
 *      rtk_vlan_egrVlanCnvtRangeCheckVid_set
 * Description:
 *      Set the configuration of VID range check for egress VLAN conversion.
 * Input:
 *      unit  - unit id
 *      index - entry index
 *      pData - configuration of VID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
*      (1) For 9300 and 9310, the function is backward compatible RTL8380/90 APIs.
 * Changes:
 *      [SDK_3.0.0]
 *          This function will be obsoleted by rtk_vlan_egrVlanCnvtRangeCheckEntry_set(unit, setIdx, index, pData)
 *          Parameters:
 *              setIdx      - 0 or 1
 */
extern int32
rtk_vlan_egrVlanCnvtRangeCheckVid_set(uint32 unit, uint32 index, rtk_vlan_vlanCnvtRangeCheck_vid_t *pData);

/* Function Name:
 *      rtk_vlan_ecidPmsk_add
 * Description:
 *      Add E-CID member ports to the VLAN on the specified device.
 * Input:
 *      unit   - unit id
 *      vid    - vlan id
 *      pEntry - pointer to ECID_PMSK entry
 * Output:
 *      pEntry - pointer to ECID_PMSK entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_PORT_MASK    - invalid portmask
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_vlan_ecidPmsk_add(uint32 unit, rtk_vlan_t vid, rtk_bpe_pmskEntry_t *pEntry);

/* Function Name:
 *      rtk_vlan_ecidPmsk_del
 * Description:
 *      Delete E-CID member ports from the VLAN on the specified device.
 * Input:
 *      unit   - unit id
 *      vid    - vlan id
 *      pEntry - pointer to ECID_PMSK entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_vlan_ecidPmsk_del(uint32 unit, rtk_vlan_t vid, rtk_bpe_pmskEntry_t *pEntry);

/* Function Name:
 *      rtk_vlan_ecidPmsk_get
 * Description:
 *      Get the portmask of an E-CID of a VLAN on the specified device.
 * Input:
 *      unit   - unit id
 *      vid    - vlan id
 *      pEntry - pointer to ECID_PMSK entry
 * Output:
 *      pEntry - pointer to ECID_PMSK entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_vlan_ecidPmsk_get(uint32 unit, rtk_vlan_t vid, rtk_bpe_pmskEntry_t *pEntry);

/* Function Name:
 *      rtk_vlan_ecidPmskNextValid_get
 * Description:
 *      Get a next valid portmask of an E-CID of a VLAN on the specified device.
 * Input:
 *      unit   - unit id
 *      vid    - vlan id
 *      pEntry - pointer to ECID_PMSK entry
 * Output:
 *      pEntry - pointer to ECID_PMSK entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - invalid unit id
 *      RT_ERR_NOT_INIT         - the module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_ENTRY_NOT_EXIST  - entry is not existed
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_vlan_ecidPmskNextValid_get(uint32 unit, rtk_vlan_t vid, rtk_bpe_pmskEntry_t *pEntry);

/* Function Name:
 *      rtk_vlan_trkVlanAggrEnable_get
 * Description:
 *      Enable N:1 VLAN aggregation support on egress trunk port.
 * Input:
 *      unit    - unit id
 *      tid     - trunk id
 * Output:
 *      pEnable - pointer to enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_TRUNK_ID     - invalid trunk id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) When the N:1 VLAN aggregation function is enabled on a egress trunk port, the inner VID
 *          of the downstream packet will be replaced by the VID which learnt in L2 table from
 *          upstream packet.
 *      (2) N:1 VLAN replacement only applies to the inner tagged and known unicast packet.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_vlan_trkVlanAggrEnable_get(uint32 unit, rtk_trk_t tid, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_vlan_trkVlanAggrEnable_set
 * Description:
 *      Enable N:1 VLAN aggregation support on egress trunk port.
 * Input:
 *      unit   - unit id
 *      tid    - trunk id
 *      enable - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - the module is not initial
 *      RT_ERR_TRUNK_ID - invalid trunk id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      9300, 9310
 * Note:
 *      (1) When the N:1 VLAN aggregation function is enabled on a egress trunk port, the inner VID
 *          of the downstream packet will be replaced by the VID which learnt in L2 table from
 *          upstream packet.
 *      (2) N:1 VLAN replacement only applies to the inner tagged and known unicast packet.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_vlan_trkVlanAggrEnable_set(uint32 unit, rtk_trk_t tid, rtk_enable_t enable);

/* Function Name:
 *      rtk_vlan_trkVlanAggrPriEnable_get
 * Description:
 *      Enable N:1 VLAN-Priority aggregation support on egress trunk port.
 * Input:
 *      unit    - unit id
 *      tid     - trunk id
 * Output:
 *      pEnable - pointer to enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_TRUNK_ID     - invalid trunk id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      (1) When the N:1 VLAN-Priority aggregation function is enabled on a egress trunk port,
 *          the inner or outer priority (selected by rtk_vlan_trkVlanAggrCtrl_set() API)
 *          of the downstream packet will be replaced by the priority which learnt in L2 table
 *          from upstream packet.
 *      (2) N:1 VLAN-Priority replacement only applies to the tagged and known unicast packet.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_vlan_trkVlanAggrPriEnable_get(uint32 unit, rtk_trk_t tid, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_vlan_trkVlanAggrPriEnable_set
 * Description:
 *      Enable N:1 VLAN-Priority aggregation support on egress trunk port.
 * Input:
 *      unit   - unit id
 *      tid    - trunk id
 *      enable - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - the module is not initial
 *      RT_ERR_TRUNK_ID - invalid trunk id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      9310
 * Note:
 *      (1) When the N:1 VLAN-Priority aggregation function is enabled on a egress trunk port,
 *          the inner or outer priority (selected by rtk_vlan_trkVlanAggrCtrl_set() API)
 *          of the downstream packet will be replaced by the priority which learnt in L2 table
 *          from upstream packet.
 *      (2) N:1 VLAN-Priority replacement only applies to the tagged and known unicast packet.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_vlan_trkVlanAggrPriEnable_set(uint32 unit, rtk_trk_t tid, rtk_enable_t enable);

/* Function Name:
 *      rtk_vlan_trkVlanAggrCtrl_get
 * Description:
 *      Get trunk vlan-aggragation configuration
 * Input:
 *      unit          - unit id
 *      tid           - trunk id
 * Output:
 *      pVlanAggrCtrl - pointer to vlan-aggr ctrl
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_TRUNK_ID     - invalid trunk id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_vlan_trkVlanAggrCtrl_get(uint32 unit, rtk_trk_t tid, rtk_vlan_aggrCtrl_t *pVlanAggrCtrl);

/* Function Name:
 *      rtk_vlan_trkVlanAggrCtrl_set
 * Description:
 *      Set trunk vlan aggragation configuration
 * Input:
 *      unit         - unit id
 *      tid          - trunk id
 *      vlanAggrCtrl - vlan-aggr ctrl
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - the module is not initial
 *      RT_ERR_TRUNK_ID - invalid trunk id
 *      RT_ERR_INPUT    - invalid input parameter
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_vlan_trkVlanAggrCtrl_set(uint32 unit, rtk_trk_t tid, rtk_vlan_aggrCtrl_t vlanAggrCtrl);

/* Function Name:
 *      rtk_vlan_intfId_get
 * Description:
 *      Get the associated interface ID of the specifed VLAN.
 * Input:
 *      unit    - unit id
 *      vid     - vlan id
 * Output:
 *      pIntfId - interface ID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_VLAN_VID       - invalid vid
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.7.0]
 *          New added function.
 */
extern int32
rtk_vlan_intfId_get(uint32 unit, rtk_vlan_t vid, rtk_intf_id_t *pIntfId);

/* Function Name:
 *      rtk_vlan_intfId_set
 * Description:
 *      Set the associated interface ID of the specifed VLAN.
 * Input:
 *      unit   - unit id
 *      vid    - vlan id
 *      intfId - interface ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_VLAN_VID       - invalid vid
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 * Applicable:
 *      9310
 * Note:
 *      (1) this API is intended for sharing existing L3 interfaces with other VLANs.
 *      (2) should NOT create any L3 interface with this vid before calling this API,
 *          since the associated interface ID could be overwritten
 *          by L3 interface create/destroy APIs once .
 * Changes:
 *      [SDK_3.7.0]
 *          New added function.
 */
extern int32
rtk_vlan_intfId_set(uint32 unit, rtk_vlan_t vid, rtk_intf_id_t intfId);

#endif /* __RTK_VLAN_H__ */

