/*
 * Copyright (C) 2010 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision:  $
 * $Date:  $
 *
 * Purpose : Export the public APIs
 *
 * Feature : Export the public APIs
 *
 */

#ifndef __IGMP_H__
#define __IGMP_H__

#include <sys_def.h>

typedef enum igmp_mode_e
{
	IGMP_MODE_SNOOPING = 0,
	IGMP_MODE_CTC,
	IGMP_MODE_PROXY,
	IGMP_MODE_SPR, /*snopping with proxy reporting*/
	IGMP_MODE_STOP
} igmp_mode_t;

typedef enum mcast_group_type_e
{
 	IGMP_GROUP_ENTRY_DYNAMIC,
	IGMP_GROUP_ENTRY_STATIC,
	IGMP_GROUP_ENTRY_ALL,
	IGMP_GROUP_ENTRY_ALLOWED_PREVIEW,
	IGMP_GROUP_ENTRY_ALLOWED_PREVIEW_UNLIMITED,
	IGMP_GROUP_ENTRY_END
}mcast_group_type_t;

typedef enum multicast_ipType_e
{
	MULTICAST_TYPE_IPV4,
	MULTICAST_TYPE_IPV6,
	MULTICAST_TYPE_END
} multicast_ipType_t;

typedef enum igmp_lookup_mode_e
{
	IGMP_DMAC_VID_MODE,  /* DMAC + VID*/
	IGMP_DIP_SIP_MODE,   /* SIP + DIP*/
	IGMP_DIP_VID_MODE,
	IGMP_DIP_MODE,
	IGMP_LOOKUP_MODE_END
} igmp_lookup_mode_t;

typedef enum mcvlan_tag_oper_mode_e
{
	TAG_OPER_MODE_TRANSPARENT = 0,
	TAG_OPER_MODE_STRIP,
	TAG_OPER_MODE_TRANSLATION
} mcvlan_tag_oper_mode_t;

typedef enum igmp_fwdFlag_e
{
	IGMPV3_FWD_NORMAL,
	IGMPV3_FWD_CONFLICT,
	IGMPV3_FWD_NOT
} igmp_fwdFlag_t;

typedef enum igmp_maxGroupAct_e
{
	IGMP_ACT_DENY,
	IGMP_ACT_REPLACE,
	IGMP_ACT_PERMIT,
	IGMP_ACT_END,
}igmp_maxGroupAct_t;

typedef struct ipv6_addr_s
{
	uint8 addr[IPV6_ADDR_LEN];
} ipv6_addr_t;

typedef struct mcast_groupAddr_s
{
	uint32                      ipv4;
	ipv6_addr_t                 ipv6;
}mcast_groupAddr_t;

typedef struct
{
	uint8                       isIpv6B;
	union
	{
		uint32                  ipv4Addr;
		ipv6_addr_t             ipv6Addr;
	} ipAddr;
}ipAddr_t;

typedef struct igmp_group_head_entry_s
{
	multicast_ipType_t          ipType;
	uint32					    dip;
	uint32					    sip;
	uint16					    vid;
	uint8					    mac[MAC_ADDR_LEN];
	uint16					    *p_mbr_timer;
	// uint64				    sortKey;
	sys_logic_portmask_t	    fmode;  /*0: include,   1:exclude*/
}igmp_group_head_entry_t;

typedef enum mcast_previewType_e
{
	MCAST_NON_PREVIEW,
	MCAST_PREVIEW,
	MCAST_ALLOWED_PREVIEW
}mcast_previewType_t;

typedef struct mcast_preview_info_s
{
	mcast_previewType_t         previewType;
	uint16                      allowedPreviewUniVid;
	uint32                      length;
}mcast_preview_info_t;

LIST_HEAD(hostHead, igmp_host_list_s);

typedef struct igmp_host_s
{
	ipAddr_t                    hostIp;
	uint16                      mbrTimer;
}__attribute__((packed)) igmp_host_t;

typedef struct igmp_host_list_s
{
	igmp_host_t                  tableEntry;
	LIST_ENTRY(igmp_host_list_s) entries;
} __attribute__((aligned)) igmp_host_list_t;

typedef struct igmp_group_entry_s
{
    multicast_ipType_t          ipType;
    uint32						dip;
    uint32						sip;
    ipv6_addr_t					dipv6;
    ipv6_addr_t					sipv6;
    uint16						vid;
    uint8						mac[MAC_ADDR_LEN];
    uint64						sortKey;
    sys_logic_portmask_t		mbr;
    sys_logic_portmask_t		fmode;  /*0: include,   1:exclude*/
    uint16						*p_mbr_timer;//src timer
    uint16						groupTimer; //grp timer
    uint8						*p_gsQueryCnt;
    uint8						lookupTableFlag;
    uint8						suppreFlag;

    struct
    {
        sys_logic_portmask_t	isGssQueryFlag;  /*flag the src is grp-src-query*/
        igmp_fwdFlag_t			*p_mbr_fwd_flag; // src mode is include or exclude
        uint8					*p_gss_query_cnt;
        igmp_group_head_entry_t	*pGroupHead;
    }v3;

    mcast_group_type_t			form;
    sys_logic_portmask_t		staticMbr;
    sys_logic_portmask_t		staticInMbr;
    struct igmp_group_entry_s	*next_subgroup;
    struct igmp_group_entry_s	*next_free;
    uint32						imputedGrpBw;   /* for tick thread update current group bandwidth when grouop is timeout */

    sys_enable_t                care_vid;       /* operate L2 table entry without IVL flag when unknown mvlan group channel in GPON mode.
                                                 * In EPON mode, non-control multicast maybe unkown mvlan group channel, but control multicast known mvlan */
    struct hostHead *p_hostListHead;
} igmp_group_entry_t;

typedef struct igmp_acl_entry_s
{
    uint32      id;       /*GPON: McastOper.entityId + ACL rowKey*/
	uint16		aniVid;
	union
	{
		uint32	ipv4;
		uint8	ipv6[IPV6_ADDR_LEN];
	}sip;
	union
	{
		uint32	ipv4;
		uint8	ipv6[IPV6_ADDR_LEN];
	}dipStart;
	union
	{
		uint32	ipv4;
		uint8	ipv6[IPV6_ADDR_LEN];
	}dipEnd;
	uint32		imputedGrpBW;
	uint16		previewLen; /* 0: means full authorized */
	uint16		previewRepeatTime;
	uint16		previewRepeatCnt;
	uint16		previewReset;
} igmp_acl_entry_t;

typedef struct multicast_dip_entry_s
{
    uint32      id;       /*GPON: GEM port-ID (2 Bytes) + Secondary key (2 Bytes)*/
	union
	{
		uint32	ipv4;
		uint8	ipv6[IPV6_ADDR_LEN];
	}dipStart;
	union
	{
		uint32	ipv4;
		uint8	ipv6[IPV6_ADDR_LEN];
	}dipEnd;
} multicast_dip_entry_t;

enum {
	CTAG,
	STAG
};

typedef enum
{
	IGMP_DS_TAG_ACTION_PASS,
	IGMP_DS_TAG_ACTION_STRIP_TAG,
	IGMP_DS_TAG_ACTION_ADD_TAG,
	IGMP_DS_TAG_ACTION_REPLACE_TAG,
	IGMP_DS_TAG_ACTION_REPLACE_VID,
	IGMP_DS_TAG_ACTION_ADD_VIDUNI_TAG,
	IGMP_DS_TAG_ACTION_REPLACE_VIDUNI_TAG,
	IGMP_DS_TAG_ACTION_REPLACE_VID2VIDUNI,
	IGMP_DS_TAG_ACTION_RESERVED,
}igmp_ds_tag_action_e;


typedef enum
{
	IGMP_TRANSPARENT	=(1 << 0),
	IGMP_ADD_VID		=(1 << 1),
	IGMP_ADD_PRI		=(1 << 2),
	IGMP_ADD_TPID		=(1 << 3),
	IGMP_COPY_INNER_VID	=(1 << 4),
    IGMP_COPY_INNER_PRI =(1 << 5),
    IGMP_COPY_INNER_TPID=(1 << 6),
    IGMP_COPY_OUTER_VID	=(1 << 7),
	IGMP_COPY_OUTER_PRI	=(1 << 8),
	IGMP_COPY_OUTER_TPID=(1 << 9),
	IGMP_REMOVE_VLAN	=(1 << 10),
	IGMP_MODIFY_VID		=(1 << 11),
	IGMP_MODIFY_PRI		=(1 << 12),
	IGMP_MODIFY_TPID	=(1 << 13)
}igmp_vlan_action_mode_e;

typedef enum
{
	IGMP_NO_FILTER		=(1 << 0), //no care
	IGMP_FILTER_UNTAG	=(1 << 1), //filter untag
	IGMP_FILTER_TAG	    =(1 << 2), //care tag
	IGMP_FILTER_PRI		=(1 << 3),
	IGMP_FILTER_VID		=(1 << 4),
	IGMP_FILTER_TPID	=(1 << 5),
	IGMP_FILTER_ETYPE	=(1 << 6),
}igmp_tag_filter_mode_e;

typedef struct igmp_tag_s
{
	uint16 tpid;
	union
    {
        struct
        {
            uint16 pri : 3;
            uint16 dei : 1;
            uint16 vid : 12;
        } bit;
        uint16 val;
    } tci;
    uint16 etype;
	union
	{
		igmp_vlan_action_mode_e	tagAct;
		igmp_tag_filter_mode_e	tagFilter;
	} tagOp;
} __attribute__((packed)) igmp_tag_t;

#define DFLT_IGMP_EBL                   ENABLED
#define DFLT_IGMP_VLAN_EBL              DISABLED
#define DFLT_IGMP_FAST_LEAVE            DISABLED
#define DFLT_IGMP_ROUTER_LEARN          ENABLED
#define DFLT_IGMP_ROUTER_PIMDVMRP_LEARN ENABLED
#define DFLT_IGMP_ROUTER_TIMEOUT        105
#define DFLT_IGMP_GROUP_MEMBERSHIP_INTV 260
#define DFLT_IGMP_ROBUSTNESS_VAR        2
#define DFLT_IGMP_QUERY_INTV            60
#define DFLT_IGMP_GS_QUERY_RESP_INTV    1
#define DFLT_IGMP_QUERY_RESPONSE_INTV   10
#define DFLT_IGMP_LOOKUP_MODE           IGMP_DIP_SIP_MODE//TBD change IGMP_DMAC_VID_MODE
#define DFLT_IGMP_SUPPRE_EBL            ENABLED
#define DFLT_IGMP_OPERATION_VER         IGMP_VERSION_V3_FULL //TBD change IGMP_VERSION_V2
#define DFLT_VLAN_VID                   1
#define DFLT_UNKNOW_MCAST_EBL           ENABLED
#define DLFT_PORT_FASTLEAVE_STATU       DISABLED
#define DLEF_PORT_GROUP_NUM             256
#define DFLT_IGMP_UNKNOWN_MCAST_ACTION  IGMP_UNKNOWN_MCAST_ACTION_DROP
#define DFLT_IGMP_MAX_GROUP_ACT         IGMP_ACT_DENY

#define DFLT_MLD_EBL                    ENABLED//DISABLED
#define DFLT_MLD_SUPPRE_EBL             ENABLED
#define DFLT_MLD_VLAN_EBL               DISABLED
#define DFLT_MLD_FAST_LEAVE             DISABLED
#define DFLT_MLD_ROUTER_PIMDVMRP_LEARN  ENABLED
#define DFLT_MLD_QUERIER                DISABLED
#define DFLT_MLD_ROUTER_TIMEOUT         260
#define DFLT_MLD_ROBUSTNESS_VAR         1
#define DFLT_MLD_GROUP_MEMBERSHIP_INTV  260
#define DFLT_MLD_QUERY_RESPONSE_INTV    10
#define DFLT_MLD_QUERY_INTV             125
#define DFLT_MLD_GS_QUERY_RESP_INTV     1
#define DFLT_MLD_LOOKUP_MODE            IGMP_DMAC_VID_MODE
#define DFLT_MLD_OPERATION_VER          MLD_VERSION_V1
#define DFLT_MLD_MAX_GROUP_ACT          IGMP_ACT_DENY
#define DFLT_MLD_PORT_PROFILE_ID        0x0
#define DFLT_MLD_QUERIER_VERSION        MLD_QUERY_V1
#define DLFT_MLD_PORT_FASTLEAVE_STATU   DISABLED
#define DLEF_MLD_PORT_GROUP_NUM         256

#endif
