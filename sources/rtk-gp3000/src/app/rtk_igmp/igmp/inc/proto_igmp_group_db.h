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
 * $Revision: 10455 $
 * $Date: 2010-06-25 18:27:53 +0800 (Fri, 25 Jun 2010) $
 *
 * Purpose : Export the public APIs in lower layer module
 *
 * Feature : Export the public APIs in lower layer module
 *
 */

#ifndef __PROTO_IGMP_GROUP_DB_H__
#define __PROTO_IGMP_GROUP_DB_H__

/*
 * Include Files
 */
#include <sys_def.h>

/*
 * Symbol Definition
 */

/*
 * Data Type Declaration
 */
typedef enum igmp_router_port_type_e
{
    IGMP_ROUTER_PORT_DYNAMIC,
    IGMP_ROUTER_PORT_STATIC,
    IGMP_ROUTER_PORT_FORBID
}igmp_router_port_type_t;

typedef enum igmp_leave_mode_s
{
    IGMP_LEAVE_MODE_NON_FAST_LEAVE = 1,
    IGMP_LEAVE_MODE_FAST
} igmp_leave_mode_t;

typedef struct igmp_router_entry_s
{
    multicast_ipType_t      ipType;
    sys_vid_t               vid;
    uint8                   rtVersion;
    uint16                  *p_port_timer;
    sys_logic_portmask_t    router_pmsk;

    sys_logic_portmask_t    router_static_pmsk;
    sys_logic_portmask_t    router_forbid_pmsk;
} igmp_router_entry_t;

typedef struct igmp_router_db_s
{
    igmp_router_entry_t     *p_rt_entry;
    uint16                  entry_num;
} igmp_router_db_t;

typedef struct igmp_aggregate_entry_s
{
    uint64                  sortKey;    /* vid + mac*/
    uint8                   *p_port_ref_cnt;
    uint16                  group_ref_cnt;
} igmp_aggregate_entry_t;

typedef struct igmp_aggregate_db_s
{
    igmp_aggregate_entry_t  *p_aggregate_entry;
    uint16                  entry_num;
} igmp_aggregate_db_t;

typedef struct igmp_group_head_s
{
    uint32                  freeListNumber;
    igmp_group_entry_t      *item;
} igmp_group_head_t;

typedef struct igmp_static_group_s
{
    igmp_group_entry_t      *pGroup;
    uint32                  groupCnt;
}igmp_static_group_t;

typedef struct mcast_vlan_entry_s
{
    sys_vid_t               vid;
    multicast_ipType_t      ipType;
    sys_enable_t            enable;
    sys_enable_t            fastleave_enable;
    sys_enable_t            pimDvmrpLearn;  /*learn router port by pim-dvmp*/
    sys_logic_portmask_t    staticMbr;  /*every group will and this mbr*/
    sys_logic_portmask_t    forbidMbr;  /*every group will not this mbr*/

    uint32                  router_port_timeout;
    uint32                  grpMembershipIntv;

    uint32                  robustnessVar;
    uint32                  operRobNessVar;
    uint32                  responseTime;
    uint32                  operRespTime;
    uint32                  queryIntv;
    uint32                  operQueryIntv;
    uint32                  grpSpecificQueryIntv;       /* Last Member Query Interval */
    uint32                  operGsqueryIntv;

    uint32                  lastMmbrQueryCnt; /* Num of GS-QUERY to be sent by QUERIER */
    uint32                  operLastMmbrQueryCnt;
    uint32                  otherQuerierPresentInterval; /* Querier election time */
} mcast_vlan_entry_t;

typedef struct mcast_vlan_db_s
{
    mcast_vlan_entry_t      *p_vlan_entry;
    uint16                  entry_num;
} mcast_vlan_db_t;

typedef enum igmp_del_port_type_e
{
    IGMP_DEL_PORT_FROM_ONE_VLAN,
    IGMP_DEL_PORT_EXCLUDE_ONE_VLAN,
    IGMP_DEL_PORT_FROM_ALL_VLAN,
    IGMP_DEL_PORT_TYPE_END
} igmp_del_port_type_t;

typedef enum intv_type_e
{
	INTV_TYPE_SPECIFIC_RSP,
    INTV_TYPE_GROUP_MBR
}intv_type_t;

/*
 * Macro Definition
 */
#define PORT_IS_FILTER_EXCLUDE      IS_LOGIC_PORTMASK_PORTSET
#define PORT_SET_FILTER_EXCLUDE     LOGIC_PORTMASK_SET_PORT
#define PORT_SET_FILTER_INCLUDE     LOGIC_PORTMASK_CLEAR_PORT

/*port member to express excluded mode fwd  0- fwd, 1-not fwd */
#define PORT_IS_EXCLUDE_NOT_FWD     IS_LOGIC_PORTMASK_PORTSET
#define PORT_SET_EXCLUDE_NOT_FWD    LOGIC_PORTMASK_SET_PORT
#define PORT_SET_EXCLUDE_FWD        LOGIC_PORTMASK_CLEAR_PORT

#define PORT_IS_SRC_QUERY           IS_LOGIC_PORTMASK_PORTSET
#define PORT_SET_SRC_NOT_QUERY      LOGIC_PORTMASK_CLEAR_PORT
#define PORT_SET_SRC_QUERY          LOGIC_PORTMASK_SET_PORT

/*
* Function Declaration
*/
extern int32 mcast_vlan_db_add(uint16 vid, multicast_ipType_t ipType);
extern int32 mcast_vlan_db_get(uint16 vid, multicast_ipType_t ipType, mcast_vlan_entry_t **ppEntry);
extern int32 mcast_vlan_db_set(mcast_vlan_entry_t *pEntry);
extern int32 mcast_vlan_db_del(uint16 vid, multicast_ipType_t ipType);
extern int32 mcast_vlan_array_search(uint16 search, multicast_ipType_t ipType,uint16 *idx, mcast_vlan_entry_t **entry);

extern int32 mcast_filter_treatment(sys_nic_pkt_t *pPkt, uint16 vid, sys_logic_port_t port, uint32 aclEntryId, mcast_preview_info_t previewInfo);
extern int32 mcast_query_treatment(sys_nic_pkt_t *pPacket, sys_logic_port_t port);//sys_logic_portmask_t txPmsk

extern int32 mcast_db_init(void);

extern int32 mcast_router_db_get(uint16 vid,  multicast_ipType_t ipType, igmp_router_entry_t **ppEntry);
extern int32 mcast_router_db_set(igmp_router_entry_t *pEntry);
extern int32 mcast_router_db_del(uint16 vid, multicast_ipType_t ipType);
extern int32 mcast_routerPort_add_wrapper(uint16 vid, multicast_ipType_t ipType, sys_logic_portmask_t pmsk);
extern int32 mcast_routerPort_get(uint16 vid, multicast_ipType_t ipType, sys_logic_portmask_t *portmask);
extern int32 mcast_routerPortMbr_remove(multicast_ipType_t ipType, sys_vlanmask_t *pVlanMsk, sys_logic_portmask_t *pdelMbr);

extern int32 mcast_aggregate_db_get(igmp_group_entry_t *pGroup, igmp_aggregate_entry_t **ppEntry);
extern int32 mcast_aggregate_db_del(igmp_group_entry_t *pGroup);
extern int32 mcast_aggregate_db_add(igmp_group_entry_t *pGroup, sys_logic_port_t port);
extern int32 mcast_aggregatePort_get(igmp_group_entry_t *pGroup, sys_logic_portmask_t *portmask);

extern igmp_group_head_entry_t *mcast_igmp_group_head_get(uint32 groupAddr, uint16 vid, igmp_group_head_entry_t **pGrpHead);
extern int32 mcast_igmp_group_head_del(uint32 groupAddr,uint16 vid);
extern igmp_group_head_entry_t *mcast_igmp_free_group_head_get(void);

extern int32 mcast_build_groupEntryKey(sys_vid_t vid, multicast_ipType_t ipType, uint32 dip,  uint8 *ipv6Addr,  igmp_group_entry_t *pGroup);

extern void mcast_group_rx_timer_stop(void);
extern void mcast_group_rx_timer_start(void);
extern int32 mcast_group_sortedArray_search(uint64 search, uint16 *idx, igmp_group_entry_t **groupHead);
extern void mcast_group_num_cal(multicast_ipType_t ipType, uint8 isInc);
extern uint64 mcast_group_sortKey_ret(igmp_group_entry_t *pGroup);
extern int32 mcast_group_add(igmp_group_entry_t *pNewEntry, ipAddr_t clientIp, uint16 portId, uint32 timer);
extern int32 mcast_group_del(igmp_group_entry_t *pGroup);
extern igmp_group_entry_t* mcast_group_get(igmp_group_entry_t *pGroup);
extern igmp_group_entry_t* mcast_group_get_with_index(uint32 index);
extern int32 mcast_group_del_all(void);
extern igmp_group_entry_t* mcast_group_freeEntry_get(multicast_ipType_t ipType);
extern int32 mcast_group_entry_release(igmp_group_entry_t *entry);
extern int32 mcast_group_portTime_update(multicast_ipType_t ipType , sys_vid_t vid, mcast_groupAddr_t *pAddr, uint16 srcNum, uint32 *pSrcList, uint32 lastMbrIntv);
extern int32 mcast_groupPort_remove(igmp_group_entry_t *pGroup, sys_logic_port_t port);
extern int32 mcast_group_learnByData_del(void);
extern int32 mcast_group_sortedArray_ins(uint16 sortedIdx, igmp_group_entry_t *entry);

extern int32 mcast_groupMbrPort_add_wrapper(
    multicast_ipType_t ipType, uint16 vid, sys_nic_pkt_t *pBuf, uint32 groupSrcIp, mcast_groupAddr_t *pAddr,
    sys_logic_port_t port, uint8 *pSendFlag, ipAddr_t clientIp, struct timespec *pRecvTime);
extern int32 mcast_groupMbrPort_del_wrapper_dipsip(
    multicast_ipType_t ipType, uint16 vid, sys_nic_pkt_t *pBuf, mcast_groupAddr_t *pAddr, sys_logic_port_t port, ipAddr_t clientIp);
extern int32 mcast_groupMbrPort_del_wrapper_dmacvid(
    multicast_ipType_t ipType, uint16 vid, sys_nic_pkt_t *pBuf, mcast_groupAddr_t *pAddr,  sys_logic_port_t port, ipAddr_t clientIp);
extern int32 mcast_igmp_group_mcstData_add_wrapper_dipsip(
    multicast_ipType_t ipType, uint16 vid, sys_nic_pkt_t *pBuf, mcast_groupAddr_t *pGroupDip,
    mcast_groupAddr_t *pSip,  sys_logic_port_t port, ipAddr_t clientIp);
extern int32 mcast_group_mcstData_add_wrapper_dmacvid(
    multicast_ipType_t ipType,uint16 vid, sys_nic_pkt_t *pBuf, mcast_groupAddr_t *pGroupDip,
    mcast_groupAddr_t *pSip,sys_logic_port_t port, ipAddr_t clientIp);

extern int32 mcast_suppreFlag_clear(sys_vid_t vid, multicast_ipType_t ipType);

extern int32 mcast_igmpv3_fwdPmsk_get(
    sys_logic_portmask_t *pPortMsk,  igmp_group_entry_t *group);
extern int32 mcast_igmpv3_groupMbrPort_isIn_proc(
    igmpv3_grec_t *pRcd, uint16 vid, uint16 portid, ipAddr_t clientIp, uint32 grpMbrTimerIntv, mcast_preview_info_t previewInfo);
extern int32 mcast_igmpv3_groupMbrPort_isEx_proc(
    igmpv3_grec_t *pRcd, uint16 vid, uint16 portid, uint16 rcdType, ipAddr_t clientIp, uint32 grpMbrTimerIntv, mcast_preview_info_t	previewInfo);
extern int32 mcast_igmpv3_groupMbrPort_toIn_proc(
    igmpv3_grec_t *pRcd, uint16 vid, uint16 portid, ipAddr_t clientIp, uint32 grpMbrTimerIntv, mcast_preview_info_t	previewInfo);
extern int32 mcast_igmpv3_groupMbrPort_block_proc(
    igmpv3_grec_t *pRcd, uint16 vid, uint16 portid, ipAddr_t clientIp, uint32 grpMbrTimerIntv, mcast_preview_info_t	previewInfo);
extern int32 mcast_igmpv3_ipMcstEntry_update_dipsip(
    uint16 vid, uint32 dip, uint32 sip, igmp_group_entry_t *pEntry);
extern int32 mcast_igmpv3_group_mcstData_add_wrapper_dipsip(
    multicast_ipType_t ipType, uint16 vid, sys_nic_pkt_t *pBuf, mcast_groupAddr_t *pGroupDip,
    mcast_groupAddr_t *pSip, sys_logic_port_t port, ipAddr_t clientIp);
extern int32 mcast_igmpv3_basic_groupMbrPort_isIn_proc(
    igmpv3_grec_t *pRcd, uint16 vid,  uint16 portid, ipAddr_t clientIp, struct timespec *pRecvTime);
extern int32 mcast_igmpv3_basic_groupMbrPort_isEx_proc(
    igmpv3_grec_t *pRcd , uint16 vid,  uint16 portid, ipAddr_t clientIp, struct timespec *pRecvTime);
extern int32 mcast_igmpv3_basic_groupMbrPort_toEx_proc(
    igmpv3_grec_t *pRcd , uint16 vid,  uint16 portid, ipAddr_t clientIp, struct timespec *pRecvTime);
extern int32 mcast_igmpv3_basic_groupMbrPort_toIn_proc(
    igmpv3_grec_t *pRcd , uint16 vid,  uint16 portid, ipAddr_t clientIp, struct timespec *pRecvTime);
extern int32 mcast_igmpv3_basic_groupMbrPort_block_proc(
    igmpv3_grec_t *pRcd , uint16 vid,  uint16 portid, ipAddr_t clientIp);

extern int32 mcast_static_group_save(sys_vlanmask_t vlanMsk,multicast_ipType_t ipType);
extern int32 mcast_static_group_restore(void);

extern int32 mcast_filter_check(
    struct timespec *pJoinTime, multicast_ipType_t ipType, sys_logic_port_t port, uint32 groupSrcIp,
	mcast_groupAddr_t *pGroupAddr, uint32 *pAclEntryId, uint16 *pGrpVid, uint32 *pImputedGrpBw, mcast_preview_info_t *pPreviewInfo);

extern int32 mcast_del_port_from_vlan(sys_vid_t vid, sys_logic_port_t port);

extern int32 mcast_mldv2_basic_groupMbrPort_isIn_proc(
    mldv2_grec_t *pRcd, uint16 vid, uint16 portid, ipAddr_t clientIp, struct timespec *pRecvTime);
extern int32 mcast_mldv2_basic_groupMbrPort_isEx_proc(
    mldv2_grec_t *pRcd, uint16 vid, uint16 portid, ipAddr_t clientIp, struct timespec *pRecvTime);
extern int32 mcast_mldv2_basic_groupMbrPort_toEx_proc(
    mldv2_grec_t *pRcd , uint16 vid,  uint16 portid, ipAddr_t clientIp, struct timespec *pRecvTime);
extern int32 mcast_mldv2_basic_groupMbrPort_toIn_proc(
    mldv2_grec_t *pRcd , uint16 vid,  uint16 portid, ipAddr_t clientIp, struct timespec *pRecvTime);
extern int32 mcast_mldv2_basic_groupMbrPort_block_proc(
    mldv2_grec_t *pRcd , uint16 vid,  uint16 portid, ipAddr_t clientIp, struct timespec *pRecvTime);

extern int32 mcast_host_tracking_del_per_port(igmp_group_entry_t *pGroupHead, uint16 portId);
extern int32 mcast_host_tracking_update_member_time_per_port(igmp_group_entry_t *pGroupHead, uint16 portId);

extern int32 mcast_group_ref_get(igmp_group_entry_t *group_entry);
extern int32 mcast_profile_time_intv_get(intv_type_t type, uint32 aclEntryId, mcast_preview_info_t *pPreviewInfo, uint32 *pIntv);

extern int32 mcast_mop_profile_per_port_find(sys_logic_port_t port, uint32 *pAclEntryId);

#endif /* __PROTO_IGMP_GROUP_DB_H__ */

