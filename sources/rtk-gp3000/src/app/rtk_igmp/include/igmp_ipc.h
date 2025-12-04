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

#ifndef __IGMP_IPC_H__
#define __IGMP_IPC_H__

#include <igmp.h>

#define IPCMSG_DATA_LEN     128//8000//1000
#define SOCKET_PATH         "/tmp/igmp_socket"

#define SHM_KEY_GROUP_ARRAY 4392
#define SHM_SIZE_2048       2048
#define SHM_MAX_SIZE        33554432

typedef enum mcast_msgType_e
{
	MCAST_MSGTYPE_MIN = 0,
	MCAST_MSGTYPE_PORT_GROUP_LIMIT_SET,
	MCAST_MSGTYPE_PORT_GROUP_LIMIT_GET,
	MCAST_MSGTYPE_MCAST_VLAN_ADD,
	MCAST_MSGTYPE_MCAST_VLAN_DEL,
	MCAST_MSGTYPE_MCAST_VLAN_CLEAR,
	MCAST_MSGTYPE_MCTAG_OPER_SET,
	MCAST_MSGTYPE_MCTAG_TRANSLATION_ADD,
	MCAST_MSGTYPE_ROUTERPORT_ADD,
	MCAST_MSGTYPE_MCAST_GROUP_DEL_BY_TYPE,
	MCAST_MSGTYPE_GROUPMBRPORT_ADD,
	MCAST_MSGTYPE_GROUPMBRPORT_DEL,
	MCAST_MSGTYPE_GROUPMCASTDATA_ADD,
	MCAST_MSGTYPE_V3_GROUPMCASTDATA_ADD,
	MCAST_MSGTYPE_IGMP_MODE_SET,
	MCAST_MSGTYPE_FASTLEAVE_MODE_SET,
	MCAST_MSGTYPE_GROUPENTRY_GET,
	MCAST_MSGTYPE_ACLENTRY_SET,
	MCAST_MSGTYPE_ACLENTRY_DEL,
	MCAST_MSGTYPE_PROFILE_SET,
	MCAST_MSGTYPE_PROFILE_DEL,
	MACST_MSGTYPE_IGMPTRAP_SET,
	MCAST_MSGTYPE_ACLENTRY_PRINT,
	MCAST_MSGTYPE_PROFILE_PRINT,
	MCAST_MSGTYPE_PORT_CFG_PRINT,
	MCAST_MSGTYPE_PORT_FLOW_INFO_ADD,
	MCAST_MSGTYPE_PORT_FLOW_INFO_DEL,
	MCAST_MSGTYPE_IGMP_RATELIMIT_SET,
	MCAST_MSGTYPE_COUNTER_GET,
	MCAST_MSGTYPE_RESET_ALL,
	MCAST_MSGTYPE_GLB_CFG_PRINT,
	MCAST_MSGTYPE_ACTIVE_GRP_SHOW,
	MCAST_MSGTYPE_ROBUSTNESS_SET,
	MCAST_MSGTYPE_QUERIERIP_SET,
	MCAST_MSGTYPE_QUERYINTVAL_SET,
	MCAST_MSGTYPE_QUERYMAXRSPTIME_SET,
	MCAST_MSGTYPE_LASTMBRQUERYINTVAL_SET,
	MCAST_MSGTYPE_DSIGMPMCASTTCI_SET,
	MCAST_MSGTYPE_UNAUTHJOINBHVR_SET,
	MCAST_MSGTYPE_MCASTPROF_DEL,
	MCAST_MSGTYPE_MCASTPROF_PRINT,
	MCAST_MSGTYPE_PREVIEW_TIMER_SHOW,
	MCAST_MSGTYPE_ALLOWED_PREVIEW_SET,
	MCAST_MSGTYPE_ALLOWED_PREVIEW_DEL,
	MCAST_MSGTYPE_ALLOWED_PREVIEW_PRINT,
	MCAST_MSGTYPE_PORT_MAX_MCAST_BW_SET,
	MCAST_MSGTYPE_PORT_BW_ENFORCE_SET,
	MCAST_MSGTYPE_PORT_CURR_BW_GET,
	MCAST_MSGTYPE_PORT_JOIN_COUNT_GET,
	MCAST_MSGTYPE_PORT_BW_EXCEED_COUNT_GET,
	MCAST_MSGTYPE_PORT_IPV4_ACTIVE_GRP_COUNT_GET,
	MCAST_MSGTYPE_PORT_IPV6_ACTIVE_GRP_COUNT_GET,
	MCAST_MSGTYPE_PORT_STAT_PRINT,
	MCAST_MSGTYPE_IGMP_LOG_SET,
	MCAST_MSGTYPE_INFO_DEBUG,
	MCAST_MSGTYPE_FILTER_MC_ON_PON_ENTRY_SET,
	MCAST_MSGTYPE_FILTER_MC_ON_PON_ENTRY_DEL,
	MCAST_MSGTYPE_FILTER_MC_ON_PON_ENTRY_PRINT,
	MCAST_MSGTYPE_FILTER_MC_ON_PON_MODE_SET,
	MCAST_MSGTYPE_MAX
}mcast_msgType_t;

/*
 * Struct for IPC message data
 */
typedef struct mcast_portIgmpMode_s
{
	uint32 port;         /*0xFF = all ports*/
	igmp_mode_t mode;
}mcast_portIgmpMode_t;

typedef struct mcast_portFastLeaveMode_s
{
	uint32 port;        /*0xFF = all ports*/
	uint32 enable;
}mcast_portFastLeaveMode_t;

typedef struct mcast_portGroupLimit_s
{
	multicast_ipType_t ipType;
	uint32 port;
	uint16 maxnum;
}mcast_portGroupLimit_t;


typedef struct mcast_routerPort_s
{
	uint16 vid;
	multicast_ipType_t ipType;
	sys_logic_portmask_t pmsk;
}mcast_routerPort_t;

typedef struct mcast_groupInfo_s
{
	multicast_ipType_t ipType;
	uint16 vid;
	sys_nic_pkt_t buf;
	mcast_groupAddr_t groupDip;
	mcast_groupAddr_t sip;
	sys_logic_port_t port;
	ipAddr_t clientIp;
	uint8 sendFlag;     /*For groupMbrPort add*/
}mcast_groupInfo_t;

typedef struct mcast_mcVlan_s
{
	uint32 portId;
	uint32 vlanId;
}mcast_mcVlan_t;


typedef struct mcast_vlanTagMode_s
{
	uint32 portId;
	mcvlan_tag_oper_mode_t tagOper;
}mcast_vlanTagMode_t;

typedef struct mcast_mcTagOper_s
{
	uint32 portId;
	uint32 tagOper;
}mcast_mcTagOper_t;

typedef struct mcast_mcTagTranslation_s{
	uint32 portId;
	uint32 mcVid;
	uint32 userVid;
}mcast_mcTagTranslation_t;

typedef struct mcast_aclEntry_s
{
	multicast_ipType_t ipType;
	mcast_group_type_t aclType;
	igmp_acl_entry_t aclEntry;
}mcast_aclEntry_t;


typedef struct mcast_mcProfile_s
{
	multicast_ipType_t ipType;
	mcast_group_type_t aclType;
	uint32 port;
	uint32 aclEntryId;
}mcast_mcProfile_t;

typedef struct mcast_igmpTrapAct_s
{
	uint32 enable;
	uint32 port;
}mcast_igmpTrapAct_t;

typedef struct mcast_igmpRateLimit_s
{
	uint32 packetRate;  /*0:no rate limit*/
	uint32 port;
}mcast_igmpRateLimit_t;

typedef struct mcast_counter_s
{
	uint32 port;
	uint32 igmpUsRx;
	uint32 igmpUsRxDrop;
}mcast_counter_t;

typedef struct mcast_port_info_s
{
	uint16			mopId;
	uint16			vidUni;
	uint32			uniPort;
	igmp_tag_t		inTag1; /* stag */
	igmp_tag_t		inTag2; /* ctag */
	igmp_tag_t		outTag1; /* stag */
	igmp_tag_t		outTag2; /* ctag */
	int				usFlowId;
} __attribute__((packed)) mcast_port_info_t;

typedef struct mcast_port_info_entry_s
{
	mcast_port_info_t entry;
	LIST_ENTRY(mcast_port_info_entry_s) entries;
}mcast_port_info_entry_t;

typedef struct mcast_prof_s
{
	uint16	mopId;
	uint8	robustness;
	uint8 	unAuthJoinRqtBhvr;
	uint32 	querierIpAddr;
	uint32 	queryIntval;
	uint32	queryMaxRspTime;
	uint32	lastMbrQueryIntval;
	uint8 	dsIgmpTci[3];
} __attribute__((packed)) mcast_prof_t;

typedef struct mcast_prof_entry_s
{
	mcast_prof_t entry;
	LIST_ENTRY(mcast_prof_entry_s) entries;
}mcast_prof_entry_t;

typedef struct mcast_allowed_preview_entry_s
{
	uint32	id; /*GPON: portId + allowed preview rowKey(10bits) */
	uint8	srcIpAddr[IPV6_ADDR_LEN];
	uint16	aniVid;
	uint16	uniVid;
	uint8	dstIpAddr[IPV6_ADDR_LEN];
	uint16	duration;
	uint16	timeLeft;
} __attribute__((packed)) mcast_allowed_preview_entry_t;

typedef struct mcast_allowed_preview_row_entry_s
{
	mcast_allowed_preview_entry_t entry;
	LIST_ENTRY(mcast_allowed_preview_row_entry_s) entries;
}mcast_allowed_preview_row_entry_t;

typedef struct mcast_portBwInfo_s
{
	uint32 port;
	uint32 maxBw;
	uint32 bwEnforceB;
}mcast_portBwInfo_t;

typedef struct mcast_portStat_s
{
	uint32 port;
	uint32 currBw;
	uint32 bwExcdCount;
	uint32 joinCount;
	uint32 ipv4ActiveGrpNum;
	uint32 ipv6ActiveGrpNum;
}mcast_portStat_t;

typedef struct mcast_filter_mc_on_pon_entry_s
{
	multicast_ipType_t ipType;
	multicast_dip_entry_t dipEntry;
}mcast_filter_mc_on_pon_entry_t;

typedef struct ipcMsg_s
{
	mcast_msgType_t msgType;
	uint8 data[IPCMSG_DATA_LEN];
}ipcMsg_t;

uint32 mcast_ipc_set(va_list argp);
uint32 mcast_ipc_get(va_list argp);
uint32 mcast_shm_get(va_list argp);
uint32 mcast_shm_destroy(va_list argp);

#endif
