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

#ifndef __PROTO_IGMP_H__
#define __PROTO_IGMP_H__

#include <igmp_ipc.h>
#include <igmp/inc/proto_igmp_querier.h>
#include <igmp/inc/proto_igmp_rx.h>
#include <igmp/inc/proto_igmp_group_db.h>
#include <igmp/inc/proto_igmp_preview_timer.h>
#include <util/inc/igmp_common_util.h>
#include "igmp_thread.h"
#include <rsd/inc/rsd_mcast.h>
#include <sys_portmask.h>


#define IGMP_IN_CLASSD(a)        ((((long int) (a)) & 0xf0000000) == 0xe0000000)
#define IGMP_IN_MULTICAST(a)     IGMP_IN_CLASSD(a)

#define IGMP_PKT_SEM_LOCK()    \
do {\
    if (osal_sem_mutex_take(igmp_sem_pkt, OSAL_SEM_WAIT_FOREVER) != SYS_ERR_OK)\
    {\
        SYS_PRINTF("IGMP pkt semaphore lock failed");\
    }\
} while(0)

#define IGMP_PKT_SEM_UNLOCK() \
do {\
    if (osal_sem_mutex_give(igmp_sem_pkt) != SYS_ERR_OK)\
    {\
        SYS_PRINTF("IGMP pkt semaphore unlock failed");\
    }\
} while(0)

#define IGMP_DB_SEM_LOCK()    \
do {\
    if (osal_sem_mutex_take(igmp_sem_db, OSAL_SEM_WAIT_FOREVER) != SYS_ERR_OK)\
    {\
        SYS_PRINTF("IGMP db semaphore lock failed");\
    }\
} while(0)

#define IGMP_DB_SEM_UNLOCK() \
do {\
    if (osal_sem_mutex_give(igmp_sem_db) != SYS_ERR_OK)\
    {\
        SYS_PRINTF("IGMP db semaphore unlock failed");\
    }\
} while(0)

#define MCAST_MAKE_AGGR_SORTKEY(ipType,dip, vid) \
			(MULTICAST_TYPE_IPV4 == ipType)  ?	\
			((((uint64)ipType & 0xf) << 60)  | ((uint64)0x01 << 52) | ((uint64)0x00 << 44) | ((uint64)0x5e << 36) | ((uint64)((dip >> 16) & 0x7f) << 28) | \
			(((dip >> 8) & 0xff) << 20) | ((dip & 0xff) << 12) | (vid & 0xfff))  :	\
			((((uint64)ipType & 0xf) << 60)  | ((uint64)0x33 << 52) | ((uint64)0x33 << 44) | ((uint64)((dip >> 24) & 0xff) << 36) | ((uint64)((dip >> 16) & 0xff) << 28) | \
			(((dip >> 8) & 0xff) << 20) | ((dip & 0xff) << 12) | (vid & 0xfff))

#define MCAST_MAKE_GROUP_SORTKEY(ipType,dip, vid) ((uint64)ipType << 44) | ((uint64)dip << 12) | vid

#define IS_IPADDR_EQUAL_UINT32(ip_u32, ip2)   \
    ((((ip_u32 >> 24) & 0xff) == ip2[0]) && \
     (((ip_u32 >> 16) & 0xff) == ip2[1]) && \
     (((ip_u32 >> 8) & 0xff) == ip2[2]) && \
     ((ip_u32 & 0xff) == ip2[3]))

#define IPV6_TO_IPV4(u8ipv6) \
    ((uint32)u8ipv6[12] << 24) | ((uint32)u8ipv6[13] << 16)  |((uint32)u8ipv6[14] << 8) |((uint32)u8ipv6[15] & 0xff)

#define MAC_PRINT "%02x:%02x:%02x:%02x:%02x:%02x"
#define MAC_PRINT_ARG(mac) \
    (mac)[0], (mac)[1], (mac)[2], (mac)[3], (mac)[4], (mac)[5]

#define IPADDR_PRINT "%3d.%3d.%3d.%3d"
#define IPADDR_PRINT_ARG(ip) \
    ((ip) & 0xFF000000) >> 24, ((ip) & 0xFF0000) >> 16, ((ip) & 0xFF00) >> 8, (ip) & 0xFF

#define IPADDRV6_PRINT "%04X:%04X:%04X:%04X:%04X:%04X:%04X:%04X"
#define IPADDRV6_PRINT_ARG(ipv6) \
    *((uint16 *)&ipv6[0]), *((uint16 *)&ipv6[2]), *((uint16 *)&ipv6[4]), *((uint16 *)&ipv6[6]), \
    *((uint16 *)&ipv6[8]), *((uint16 *)&ipv6[10]), *((uint16 *)&ipv6[12]), *((uint16 *)&ipv6[14])

#define IGMP_TEMP_PERIOD                        3

#define DVMRP_PROTO_ID                          2
#define OSPF_PROTO_ID                           89
#define PIM_PROTO_ID                            103

/* Packet type */
#define VALID_OTHER_PACKET                      0
#define SUPPORTED_IGMP_CONTROL_PACKET           1
#define UNSUPPORTED_IGMP_CONTROL_PACKET         2
#define ERRONEOUS_PACKET                        3
#define MULTICAST_DATA_PACKET                   5
#define ROUTER_ROUTING_PACKET                   6

#define IGMP_HOST_MEMBERSHIP_QUERY              0x11    /* From RFC1112 */
#define IGMP_HOST_MEMBERSHIP_REPORT             0x12    /* Ditto */
#define IGMP_PIMV1_ROUTING_PKT                  0x14    /* PIMv1 */
#define IGMP_HOST_NEW_MEMBERSHIP_REPORT         0x16    /* New version of 0x11 */
#define IGMP_HOST_LEAVE_MESSAGE                 0x17
#define IGMPv3_HOST_MEMBERSHIP_REPORT           0x22    /* RFC 3376 */

#define IGMP_QUERY_V2                           2
#define IGMP_QUERY_V3                           3

#define MODE_IS_INCLUDE                         1
#define MODE_IS_EXCLUDE                         2
#define CHANGE_TO_INCLUDE_MODE                  3
#define CHANGE_TO_EXCLUDE_MODE                  4
#define ALLOW_NEW_SOURCES                       5
#define BLOCK_OLD_SOURCES                       6

#define FILTER_MODE_INCLUDE                     0
#define FILTER_MODE_EXCLUDE                     1

#define  L2TABLE_NOT_WROTE                      0
#define  L2TABLE_WROTE                          1

/*igmpv3 exclude mode forward flag*/
#define  IGMPV3_FWD_NORMAL                      0
#define  IGMPV3_FWD_CONFLICT                    1
#define  IGMPV3_FWD_NOT                         2

#define PASS_SECONDS                            1

#define MAX_PAYLOAD                             (1600)

#define VLAN_TRANSLATION_ENTRY                  (16 * (igmpCtrl.sys_max_port_num - 2))


/*For IGMP US Rate limit*/
#define RATE_CHECKTIME_UNIT                     100000 /*mircosecend*/
#define RATE_CHECKTIME_NUM                      (1000000 / RATE_CHECKTIME_UNIT)


#define MAX_GRUP_NUM_PER_PORT                   64
#define MAX_MC_GROUP_NUM                        (((igmpCtrl.sys_max_port_num - 2)) * (MAX_GRUP_NUM_PER_PORT))

typedef enum igmp_version_e
{
    IGMP_VERSION_V1 = 1,
    IGMP_VERSION_V2,
    IGMP_VERSION_V3_BASIC,
    IGMP_VERSION_V3_FULL,
    IGMP_VERSION_END,
} igmp_version_t;

typedef enum igmp_unknMcastAction_e
{
    IGMP_UNKNOWN_MCAST_ACTION_DROP,
    IGMP_UNKNOWN_MCAST_ACTION_FLOOD,
    IGMP_UNKNOWN_MCAST_ACTION_ROUTER_PORT,
    IGMP_UNKNOWN_MCAST_ACTION_END,
} igmp_unknMcastAction_t;

typedef enum mcast_forward_e
{
    MCAST_FORWARD_NONE,
    MCAST_FORWARD_STATIC,
    MCAST_FORWARD_FORBIDDEN,
    MCAST_FORWARD_END,
}mcast_forward_t;

typedef struct {
    uint16	vlanId;
    uint32	srcIpAddr;
    uint32	dstIpAddr;
    uint32	actualBw;
    uint32	clientIpAddr;
    uint32	recentJoinTime;
    uint16	reserved;
} __attribute__((packed)) ipv4ActiveGroup_t;

typedef struct {
    uint16	vlanId;
    uint8	srcIpAddr[IPV6_ADDR_LEN];
    uint8	dstIpAddr[IPV6_ADDR_LEN];
    uint32	actualBw;
    uint8	clientIpAddr[IPV6_ADDR_LEN];
    uint32	recentJoinTime;
} __attribute__((packed)) ipv6ActiveGroup_t;

typedef struct ipv4ActiveGrpTblEntry_s
{
	ipv4ActiveGroup_t tableEntry;
	LIST_ENTRY(ipv4ActiveGrpTblEntry_s) entries;
} __attribute__((aligned))ipv4ActiveGrpTblEntry_t;

typedef struct ipv6ActiveGrpTblEntry_s
{
	ipv6ActiveGroup_t tableEntry;
	LIST_ENTRY(ipv6ActiveGrpTblEntry_s) entries;
} __attribute__((aligned))ipv6ActiveGrpTblEntry_t;

typedef struct mcast_port_stats_s
{
	uint32 maxBw;
	uint32 bwEnforceB;
	uint32 curBw;
	uint32 bwExcdCount;	// over UINT_MAX, reset to 0
	uint32 sJoinCount;	// over UINT_MAX, reset to 0
	uint32 usJoinCount; // over UINT_MAX, reset to 0
	uint32 ipv4ActiveGrpCount;
	uint32 ipv6ActiveGrpCount;
	LIST_HEAD(ipv4ActiveGrpTblHead_t, ipv4ActiveGrpTblEntry_s) ipv4ActiveGrpTblHead;
	LIST_HEAD(ipv6ActiveGrpTblHead_t, ipv6ActiveGrpTblEntry_s) ipv6ActiveGrpTblHead;
}mcast_port_stats_t;

typedef struct igmp_stats_s
{
    sys_enable_t        igmpsnp_enable;
    sys_enable_t        suppre_enable;	//equal to proxy reporting
    igmp_version_t      oper_version;
    igmp_lookup_mode_t  lookup_mode;

//	mcast_unknow_act_t unknowIp4mcastAct;

    uint32              total_pkt_rcvd;
    uint32              valid_pkt_rcvd;
    uint32              invalid_pkt_rcvd;

    uint32              g_query_rcvd;
    uint32              gs_query_rcvd;
    uint32              leave_rcvd;
    uint32              report_rcvd;
    uint32              other_rcvd;

    uint32              g_query_xmit;
    uint32              gs_query_xmit;
    uint32              leave_xmit;
    uint32              report_xmit;

    struct
    {
        uint32          g_queryV3_rcvd;
        uint32          gs_queryV3_rcvd;
        uint32          gss_queryV3_rcvd;
        uint32          isIn_rcvd;
        uint32          isEx_rcvd;
        uint32          toIn_rcvd;
        uint32          toEx_rcvd;
        uint32          allow_rcvd;
        uint32          block_rcvd;
        uint32          g_queryV3_xmit;             /*querier tx packet*/
        uint32          gs_queryV3_xmit;
        uint32          gss_queryV3_xmit;           /*igmpv3 specific group and source  tx packet*/
    }v3;

	mcast_port_stats_t  *p_port_info;
    uint32              *p_port_entry_limit;
    uint32              *p_max_groups_act;
} igmp_stats_t;

extern igmp_stats_t        *p_igmp_stats;

typedef enum igmp_type_e
{
    IGMP_TYPE_MEMBERSHIP_QUERY     = 0x11,  /* Source Quench                */
    IGMPV1_TYPE_MEMBERSHIP_REPORT  = 0x12,  /* Host Membership Report       */
    IGMPV2_TYPE_MEMBERSHIP_REPORT  = 0x16,  /* Redirect (change route)      */
    IGMPV2_TYPE_MEMBERSHIP_LEAVE   = 0x17,  /*                              */
    IGMPV3_TYPE_MEMBERSHIP_REPORT  = 0x22   /*                              */
} igmp_type_t;

typedef struct sys_igmp_info_s
{
    sys_enable_t enable;
    sys_enable_t suppreEnable;
    sys_enable_t debug;

    igmp_unknMcastAction_t unknMcastActn;
    igmp_version_t  operVersion;

    uint32  totalRx;
    uint32  validRx;
    uint32  invalidRx;
    uint32  generalQueryRx;
    uint32  gsQueryRx;
    uint32  leaveRx;
    uint32  reportRx;
    uint32  otherRx;
    uint32  generalQueryTx;
    uint32  gsQueryTx;
    uint32  leaveTx;
    uint32  reportTx;

    struct
    {
        uint32  generalQueryV3Rx;
        uint32  gsqueryV3Rx;
        uint32  gsSrcQueryV3Rx;
        uint32  isInRx;
        uint32  isExRx;
        uint32  toInRx;
        uint32  toExRx;
        uint32  allowRx;
        uint32  blockRx;
        uint32  gQuerierV3Tx;      /*querier tx packet*/
        uint32  gsQueryV3Tx;
        uint32  gSsrcV3Tx;         /*igmpv3 specific group and source  tx packet*/
    }v3;

} sys_igmp_info_t;

typedef struct igmp_acl_entry_list_s
{
    igmp_acl_entry_t                *aclEntry;
    struct igmp_acl_entry_list_s    *next;
}igmp_acl_entry_list_t;

typedef struct multicast_dip_entry_list_s
{
    multicast_dip_entry_t               *dipEntry;
    struct multicast_dip_entry_list_s   *next;
}multicast_dip_entry_list_t;

typedef struct sys_igmp_profile_s
{
    uint32                          valid; /*Not used now, just reserved*/
    igmp_acl_entry_list_t           *aclEntryList[IGMP_GROUP_ENTRY_ALL];
    igmp_maxGroupAct_t              act;
}sys_igmp_profile_t;

typedef enum mc_control_s
{
	MC_CTRL_GDA_MAC			= 0x00,
	MC_CTRL_GDA_MAC_VID		= 0x01,
	MC_CTRL_GDA_GDA_SA_MAC	= 0x02,
	MC_CTRL_GDA_GDA_IP_VID	= 0x03,
	MC_CTRL_END /* End of multicast controls */
} mc_control_e;

typedef struct sys_igmp_func_s
{
    int32 (*igmp_group_timer)(void);
    int32 (*igmp_groupMbrPort_del_wrapper)(multicast_ipType_t ipType, uint16 vid, sys_nic_pkt_t *pBuf, mcast_groupAddr_t *pAddr, sys_logic_port_t port, ipAddr_t clientIp);
    int32 (*igmp_group_mcstData_add_wrapper)(multicast_ipType_t ipType, uint16 vid, sys_nic_pkt_t *pBuf, mcast_groupAddr_t *pGroupDip, mcast_groupAddr_t * pSip, sys_logic_port_t port, ipAddr_t clientIp);
    int32 (*igmpv3_group_mcstData_add_wrapper)(multicast_ipType_t ipType,uint16 vid, sys_nic_pkt_t *pBuf, mcast_groupAddr_t *pGroupDip, mcast_groupAddr_t * pSip, sys_logic_port_t port, ipAddr_t clientIp);
} sys_igmp_func_t;

typedef struct igmp_mc_vlan_entry_s
{
	sys_logic_portmask_t    portmask;
	uint32                  vid;
}igmp_mc_vlan_entry_t;

typedef struct igmp_vlan_translation_entry_s
{
    uint32                  enable;
    uint32                  portId;
    uint32                  mcVid;
    uint32                  userVid;
} igmp_vlan_translation_entry_t;

typedef struct igmp_vlan_translation_entry_vid_s
{
    uint32                  mcVid;
    uint32                  userVid;
} igmp_vlan_translation_entry_vid_t;

typedef struct igmp_control_entry_s
{
	uint16                  port_id;
	uint16                  vlan_id;
	uint8                   mac[MAC_ADDR_LEN];
	uint32                  ip;
}igmp_control_entry_t;

struct lw_l2_ctl_s
{
    unsigned short          sll_port;
    unsigned short          sll_vlanid;
};

/**************MLD Snooping ******************/
#define MLD_QUERY_V1                    1
#define MLD_QUERY_V2                    2

typedef enum mld_version_e
{
    MLD_VERSION_V1                      = 1,
    MLD_VERSION_V2_BASIC,
    MLD_VERSION_V2_FULL,
    MLD_VERSION_END,
} mld_version_t;

#define MLD_TYPE_MEMBERSHIP_QUERY       130
#define MLD_TYPE_MEMBERSHIP_REPORT      131
#define MLD_TYPE_MEMBERSHIP_DONE        132
#define MLD_ROUTER_SOLICITATION         133
#define MLD_ROUTER_ADVERTISEMENT        134
#define MLD_NEIGHBOR_SOLICITATION       135
#define MLD_NEIGHBOR_ADVERTISEMENT      136
#define MLD_REDIRECT                    137
#define MLDV2_TYPE_MEMBERSHIP_REPORT    143

typedef struct mld_stats_s
{
    sys_enable_t            mldsnp_enable;
    sys_enable_t            suppre_enable;
    uint32                  oper_version;
    igmp_lookup_mode_t      lookup_mode;

    uint32                  total_pkt_rcvd;
    uint32                  valid_pkt_rcvd;
    uint32                  invalid_pkt_rcvd;

    uint32                  g_query_rcvd;
    uint32                  gs_query_rcvd;
    uint32                  gss_query_rcvd;
    uint32                  leave_rcvd;
    uint32                  report_rcvd;
    uint32                  other_rcvd;

    uint32                  g_query_xmit;
    uint32                  gs_query_xmit;
    uint32                  gss_query_xmit;
    uint32                  leave_xmit;
    uint32                  report_xmit;

    uint32                  *p_port_entry_limit;
    uint32                  *p_max_groups_act;
} mld_stats_t;

extern mld_stats_t         *p_mld_stats;

LIST_HEAD(mcast_port_info_head_t, mcast_port_info_entry_s);
LIST_HEAD(mcast_prof_head_t, mcast_prof_entry_s);
LIST_HEAD(mcast_allowed_preview_head_t, mcast_allowed_preview_row_entry_s);

extern struct mcast_port_info_head_t portInfoHead;
extern struct mcast_prof_head_t mcastProfHead;
extern struct mcast_allowed_preview_head_t allowedPreviewHead;

/* in proto_igmp_group_db.c */
extern int32                            group_sortedAry_entry_num;
extern igmp_group_entry_t               **pp_group_sorted_array;
extern igmp_acl_entry_list_t            *globalAclEntryListHead[MULTICAST_TYPE_END][IGMP_GROUP_ENTRY_ALL];
extern multicast_dip_entry_list_t       *globalMcDipEntryListHead[MULTICAST_TYPE_END];
extern uint32                           mcast_group_num;

extern igmp_group_head_entry_t          *p_group_head_db;

/* in proto_igmp.c */
extern uint32                           *p_rx_counter_current;
extern uint32                           *p_remain_packets;
extern uint32                           *p_rx_counter;
extern uint32                           *p_mcast_total_rx_counter;
extern uint32                           *p_mcast_rx_drop_counter;
extern sys_igmp_func_t                  igmp_lookup_mode_func[];
extern igmp_vlan_translation_entry_t    *p_igmp_tag_translation_table;
extern uint32                           *p_igmp_tag_oper_per_port;/* transparent, strip, translation */
extern int32                            igmpMcVlanNum;
extern igmp_mc_vlan_entry_t             igspVlanMap[4096];
extern osal_mutex_t                     igmp_sem_db;
extern osal_mutex_t                     igmp_sem_pkt;
extern uint32                           gUnknown_mcast_action;
extern sys_enable_t                     mcast_groupAdd_stop;
extern sys_enable_t                     igmp_packet_stop;
extern sys_enable_t                     mcast_timer_stop;

/* in proto_igmp_preview_timer.c */
extern TimerMgr_t                       tMgr;

/* in proto_rate.c */
extern uint32                           *p_igmp_us_rate_limit;
extern uint32                           **pp_rx_counter_per_interval;

/*
 * Macro Definition
 */

/*************Function define ************************/

/* Function Name:
 *      mcast_table_clear
 * Description:
 *      Clears IGMP table stored in ASIC
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      SYS_ERR_OK
 * Note:
 *      None
 */
extern int32 mcast_table_clear(void);

/* Function Name:
 *      mcast_unknMcastAction_set
 * Description:
 *      Sets unknow multicast action e.g. DROP/FLOOD
 * Input:
 *      action - Unknown multicast action, DROP/FLOOD
 * Output:
 *      None
 * Return:
 *      SYS_ERR_OK
 * Note:
 *      None
 */
extern int32 mcast_unknMcastAction_set(uint8 action);

/* Function Name:
*      mcast_portGroup_excessAct_get
* Description:
*      Gets mcast port excess max-group action
* Input:
*      ipType - ip type
*      port - logic port id
* Output:
*      pAct - excess max-group action
* Return:
*      SYS_ERR_OK
* Note:
*      None
*/
extern int32 mcast_portGroup_excessAct_get(multicast_ipType_t ipType, sys_logic_port_t port, igmp_maxGroupAct_t *pAct);

/* Function Name:
 *      mcast_portGroup_limit_get
 * Description:
 *      Gets igmp port max limit group
 * Input:
 *      ipType - ip type
 *      port - logic port id
 *      pMaxnum - max num of group
 * Output:
 *      None
 * Return:
 *      SYS_ERR_OK
 * Note:
 *      None
 */
extern int32 mcast_portGroup_limit_get(multicast_ipType_t ipType,sys_logic_port_t port, uint16 *pMaxnum);


/* Function Name:
 *      mcast_vlan_set
 * Description:
 *      Configures mcast vlan information
 * Input:
 *      pEntry - Multicast VLAN information
 * Output:
 *      None
 * Return:
 *      SYS_ERR_OK
 * Note:
 *      None
 */
extern int32 mcast_vlan_set(mcast_vlan_entry_t *pEntry);

/* Function Name:
 *      mcast_vlan_get
 * Description:
 *      Get mcast vlan information
 * Input:
 *      pEntry - Multicast VLAN information
 * Output:
 *      pEntry - Multicast VLAN information
 * Return:
 *      SYS_ERR_OK
 * Note:
 *      None
 */
extern int32 mcast_vlan_get(mcast_vlan_entry_t *pEntry);


/* Function Name:
 *      mcast_vlan_del
 * Description:
 *      Delete mcast vlan information
 * Input:
 *      vid - VLAN ID
 *      ipType - ip type ipv4/ipv6
 * Output:
 *     None
 * Return:
 *      SYS_ERR_OK
 * Note:
 *      None
 */
extern int32 mcast_vlan_del(sys_vid_t vid, multicast_ipType_t ipType);

/* Function Name:
 *      mcast_vlan_add
 * Description:
 *      add mcast vlan information
 * Input:
 *      pEntry - Multicast VLAN information
 * Output:
 *      None
 * Return:
 *      SYS_ERR_OK
 * Note:
 *      None
 */
extern int32 mcast_vlan_add(mcast_vlan_entry_t *pEntry);


/* Function Name:
 *      mcast_vlanMskConfig_get
 * Description:
 *      Get mcast VLAN configuration entry
 * Input:
 *      None
 * Output:
 *      pBitmap - pointer to VLAN exist bitmap
 * Return:
 *      SYS_ERR_OK
 * Note:
 *      None
 */
extern int32 mcast_vlanMskConfig_get(multicast_ipType_t ipType, sys_vlanmask_t *pBitmap);


/* Function Name:
 *      mcast_router_get
 * Description:
 *      Retrieves router information
 * Input:
 *      pRouter - Router information
 * Output:
 *      None
 * Return:
 *      SYS_ERR_OK
 * Note:
 *      None
 */
extern int32 mcast_router_get(igmp_router_entry_t *pRouter);

/* Function Name:
*      mcast_router_set
* Description:
*      set router database
* Input:
*      pRouter - Router information
* Output:
*      None
* Return:
*      SYS_ERR_OK
* Note:
*      None
*/
extern int32 mcast_router_set(igmp_router_entry_t *pRouter);

/* Function Name:
*      mcast_router_dynPortMbr_del
* Description:
*      delete dynamic router port
* Input:
*      ipType - ip Type
* Output:
*      None
* Return:
*      SYS_ERR_OK
* Note:
*      None
*/
extern int32 mcast_router_dynPortMbr_del(multicast_ipType_t ipType);

/* Function Name:
 *      mcast_static_group_clear
 * Description:
 *      Clear IGMP static group entry.
 * Input:
 *      pGroup - group entry
 * Output:
 *      None
 * Return:
 *      SYS_ERR_OK
 *      SYS_ERR_FAILED;
 * Note:
 *      None
 */
extern int32 mcast_static_group_clear(igmp_group_entry_t *pGroup);

/* Function Name:
*      mcast_basic_static_group_clear
* Description:
*      Clear IGMP static group entry for v3 basic.
* Input:
*     pGroup - group entry
* Output:
*      None
* Return:
*      SYS_ERR_OK
*      SYS_ERR_FAILED;
* Note:
*      None
*/
extern int32 mcast_basic_static_group_clear(igmp_group_entry_t *pGroup);

/* Function Name:
*      mcast_basic_static_groupEntry_set
* Description:
*      Set IGMP v3-basic  static group entry.
* Input:
*      pGroup - group entry
*      fmode - include /exclude
*      addDel - True/False
* Output:
*      None
* Return:
*      SYS_ERR_OK
*      SYS_ERR_FAILED;
* Note:
*      pGroup->mbr  is the input portmask.
*/
extern int32 mcast_basic_static_groupEntry_set(igmp_group_entry_t *pGroup, uint8 fmode,  uint8 addDel);

/* Function Name:
*      mcast_profile_get
* Description:
*      Get multicast profile.
* Input:
*      ipType - ip Type
*      port - logic port id
*      pProfile - pointer of profile pointer
* Output:
*      None
* Return:
*      SYS_ERR_OK
*      SYS_ERR_MCAST_IPTYPE
*      SYS_ERR_PORT_ID
* Note:
*
*/
extern int32 mcast_profile_get(multicast_ipType_t ipType, sys_logic_port_t port, sys_igmp_profile_t **pProfile);

/* Function Name:
*      mcast_igmpv3_static_filter_group_set
* Description:
*      Set IGMP static filter group entry.
* Input:
*       pGroup -- group entry
*       fmode - include/exclude
* Output:
*      None
* Return:
*      SYS_ERR_OK
*      SYS_ERR_FAILED;
* Note:
*      pGroup will inpurt vid/dip/sip/fmode
*/
extern int32 mcast_igmpv3_static_filter_group_set(igmp_group_entry_t *pGroup, uint8 fmode);

/* Function Name:
 *      mcast_igmpv3_static_groupEntry_set
 * Description:
 *      Set IGMP static group entry.
 * Input:
 *      vid - vlan id
 *      groupAddr - group Ip
 *      sourceAddr - source Ip
 *      pSpmsk - static port list
 *      fmode - include /exclude
 * Output:
 *      None
 * Return:
 *      SYS_ERR_OK
 *      SYS_ERR_FAILED
 * Note:
 *      None
 */
extern int32 mcast_igmpv3_static_groupEntry_set(
    sys_vid_t vid, uint32 groupAddr,  uint32 sourceAddr, sys_logic_portmask_t *pSpmsk,  uint8 fmode);
/* Function Name:
 *      mcast_igmp_statistics_clear
 * Description:
 *      Clears IGMP statistics
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      SYS_ERR_OK
 * Note:
 *      None
 */
extern int32 mcast_igmp_statistics_clear(void);

/* Function Name:
*      mcast_igmp_lookupMode_get
* Description:
*      Get igmp lookup mode
* Input:
*      None
* Output:
*      lookup mode
* Return:
*      SYS_ERR_OK
* Note:
*      None
*/
extern int32 mcast_igmp_lookupMode_get(igmp_lookup_mode_t *mode);


/* Function Name:
*      mcast_igmp_operVersion_get
* Description:
*      Sets igmp operation version
* Input:
*      operVersion - operation version
* Output:
*      None
* Return:
*      SYS_ERR_OK
* Note:
*      None
*/
extern int32 mcast_igmp_operVersion_get(igmp_version_t *pVersion);

/* Function Name:
 *      mcast_igmp_operVersion_set
 * Description:
 *      Sets igmp operation version
 * Input:
 *      operVersion - operation version
 * Output:
 *      None
 * Return:
 *      SYS_ERR_OK
 * Note:
 *      None
 */
extern int32 mcast_igmp_operVersion_set(uint8 operVersion);


/* Function Name:
 *      mcast_igmp_querier_set
 * Description:
 *      Configures/enables IGMP Querier in VLANs
 * Input:
 *      pQuerier - Querier information
 * Output:
 *      None
 * Return:
 *      SYS_ERR_OK
 * Note:
 *      None
 */
extern int32 mcast_igmp_querier_set(sys_igmp_querier_t *pQuerier);

/* Function Name:
 *      mcast_igmp_querier_get
 * Description:
 *      Retrieves IGMP Querier information
 * Input:
 *      pQuerier - Querier information
 * Output:
 *      None
 * Return:
 *      SYS_ERR_OK
 * Note:
 *      None
 */
extern int32 mcast_igmp_querier_get(sys_igmp_querier_t *pQuerier);

/* Function Name:
*      mcast_igmp_lookupMode_set
* Description:
*      Set igmp lookup mode
* Input:
*      lookup mode
* Output:
*      None
* Return:
*      SYS_ERR_OK
*      SYS_ERR_FAILED
* Note:
*      None
*/
extern int32 mcast_igmp_lookupMode_set(igmp_lookup_mode_t mode);

/* Function Name:
*      mcast_igmp_suppre_enable_set
* Description:
*      Enable IGMP suppre report function.
* Input:
*      enable - ENABLE/DISABLE MLD
* Output:
*      None
* Return:
*      SYS_ERR_OK
* Note:
*      None
*/
extern int32 mcast_igmp_suppre_enable_set(sys_enable_t enable);

/* Function Name:
 *      mcast_igmp_enable_set
 * Description:
 *      Enable IGMP module globally
 * Input:
 *      enable - ENABLE/DISABLE IGMP
 * Output:
 *      None
 * Return:
 *      SYS_ERR_OK
 * Note:
 *      None
 */
extern int32 mcast_igmp_enable_set(sys_enable_t enable);

/* Function Name:
 *      mcast_igmp_info_get
 * Description:
 *      Retrieves IGMP information
 * Input:
 *      pInfo - IGMP information
 * Output:
 *      None
 * Return:
 *      SYS_ERR_OK
 * Note:
 *      None
 */
extern int32 mcast_igmp_info_get(sys_igmp_info_t *pInfo);

/* Function Name:
*      mcast_igmp_group_del_byType
* Description:
*     clear igmp group by group type
* Input:
*      type -- group type
* Output:
*      None
* Return:
*      SYS_ERR_OK
* Note:
*      None
*/
extern int32 mcast_igmp_group_del_byType( mcast_group_type_t type);

/*************************mld function define ****************************************************/

/* Function Name:
 *      mcast_mld_statistics_clear
 * Description:
 *      Clears MLD statistics
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      SYS_ERR_OK
 * Note:
 *      None
 */
extern int32 mcast_mld_statistics_clear(void);

/* Function Name:
*      mcast_mld_suppre_enable_set
* Description:
*      Enable MLD suppre report function.
* Input:
*      enable - ENABLE/DISABLE MLD
* Output:
*      None
* Return:
*      SYS_ERR_OK
* Note:
*      None
*/
extern int32 mcast_mld_suppre_enable_set(sys_enable_t enable);

/* Function Name:
 *      mcast_mld_enable_set
 * Description:
 *      Enable MLD module globally
 * Input:
 *      enable - ENABLE/DISABLE MLD
 * Output:
 *      None
 * Return:
 *      SYS_ERR_OK
 * Note:
 *      None
 */
extern int32 mcast_mld_enable_set(sys_enable_t enable);

/* Function Name:
*      mcast_mld_group_del_byType
* Description:
*     clear mld group by group type
* Input:
*      type -- group type
* Output:
*      None
* Return:
*      SYS_ERR_OK
* Note:
*      None
*/
extern int32 mcast_mld_group_del_byType(mcast_group_type_t type);

extern int mcast_igspVlanMapExist(uint32 portId, uint32 vlanId);
extern int32 mcast_igmp_group_vid_by_group_search(multicast_ipType_t ipType, mcast_groupAddr_t grpAddr, uint16 *pGrpVid, uint32 *pAclEntryId);

extern int32 mcast_igmpMode_set(uint32 mode);

extern int mcast_ismvlan(int vid);
extern int mcast_mcVlanExist(uint32 vlanId);

extern int mcast_igmpMcVlanGet(uint32 portId, uint16 *vlanId, uint32 *num);
extern int mcast_igmpTagTranslationTableGetbyUservid(uint32 portId, uint16 userVid, uint16 *mcVid);
extern mcvlan_tag_oper_mode_t mcast_igmpTagOperPerPortGet(uint32 portId);
extern int mcast_igmpTagTranslationEntryGetPerPort(uint32 portId, igmp_vlan_translation_entry_vid_t *p_tranlation_entry, uint32 *num);
extern int mcast_igmpMcTagstripGet(uint32 portId, uint8 *tagOper);
extern int mcast_igmpTagTranslationTableGet(uint32 portId, uint16 mcVid, uint16 *userVid);

extern uint32 mcast_fastLeaveMode_get(uint32 portId);

extern int32 _mcast_vlan_handler(void* pData);

extern int mcast_igmpMcVlanDelete(uint32 portId, uint32 vlanId);
extern int _mcast_igmpMcVlanDelete(uint32 portId, uint32 vlanId);

extern int ext_mcast_vlan_del(int vid);

extern int mcast_ctrlGrpEntry_get(igmp_control_entry_t *ctl_entry_list, int *num);

extern int32 mcast_igmp_mld_ctrl_init(ponMode_t ponMode);

extern int32 igmp_mld_cfg_msg_task_init(void);
extern int32 igmp_mld_rate_limit_timer_init(void);
extern int32 igmp_mld_create_pkt_dev(void);
extern int32 igmp_mld_send_pkt(uint8 *pMsg, uint32 uiLen, rtk_portmask_t portmask, uint32 sid);

extern int32 mcast_init(void);
extern int32 mcast_exit(void);

extern int32 mcast_igmp_mld_statistics_init(void);

extern int mcast_recv(unsigned char *frame, unsigned short frame_len);

extern int32 mcast_max_life_get(igmp_group_entry_t *group_entry);
extern int32 mcast_hw_mcst_mbr_remove(igmp_group_entry_t *group_entry, sys_logic_portmask_t *delPmsk);
extern int32 mcast_hw_mcst_entry_del(igmp_group_entry_t *group_entry);
extern int32 mcast_hw_l2McastEntry_add(multicast_ipType_t ipType, sys_l2McastEntry_t *pEntry);
extern int32 mcast_hw_l2McastEntry_set(multicast_ipType_t ipType,sys_l2McastEntry_t *pEntry);

#endif /* __PROTO_IGMP_H__ */

