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

#ifndef __PROTO_IGMP_QUERIER_H__
#define __PROTO_IGMP_QUERIER_H__


#define IGMPV3_HEAD_LEN         4
#define IGMPV3_OPTIONS_LEN      4

enum
{
    IGMP_QUERIER_STATE_INIT = 0,
    IGMP_QUERIER_STATE_LEAVING_HOST, /* Has rcvd a Leave msg and going to leave */
    IGMP_QUERIER_STATE_OTHER_HOST, /* Non Leaving host, expecting report
                                      * from this host */
};

enum
{
    IGMP_NON_QUERIER,
    IGMP_QUERIER,
};

enum
{
    IGMP_GENERAL_QUERY,
    IGMP_SPECIFIC_GROUP_QUERY,
    IGMP_SPECIFIC_GROUP_SRC_QUERY,
};

typedef struct sys_igmp_querier_s
{
    uint16  vid;
    multicast_ipType_t ipType;
    uint8   enabled;
    uint8   status;
    uint8   version;
    uint32  ipAddr;
} sys_igmp_querier_t;

typedef struct igmp_querier_entry_s
{
    multicast_ipType_t ipType;
    uint32 ip; /* Querier IP of the VLAN, may be the Switch or External Router*/
    uint8   ipv6[IPV6_ADDR_LEN];
    uint32 vid;
    uint32 status;
    uint32 enabled;
    uint32 version;
    uint32 configured; /* this will identify whether wuerier is configured or not */
    uint16 startupQueryCnt;
    uint16 timer;
} igmp_querier_entry_t;

typedef struct igmp_querier_db_s
{
	igmp_querier_entry_t    *p_querier_entry;
	uint16                  entry_num;
} igmp_querier_db_t;

typedef struct igmp_querier_ip_s
{
    uint32 ip;
    uint32 vid;
} igmp_querier_ip_t;



#define IGMP_FILL_QUERY(pkt, field, offset, len) \
do {\
    osal_memcpy(&(pkt)->data[offset], (uint8*)(field), len);\
}while(0)

int32 mcast_querier_db_init(void);
int32 mcast_querier_db_add(uint16 vid, multicast_ipType_t ipType);
int32 mcast_querier_db_get(uint16 vid, multicast_ipType_t ipType, igmp_querier_entry_t **ppEntry);
int32 mcast_querier_db_del(uint16 vid, multicast_ipType_t ipType);
int32 mcast_igmp_querier_check(uint16 vid, uint32 sip, uint8 query_version);
void mcast_igmp_querier_timer(void);
void mcast_igmp_send_grp_specific_query(uint16 vid, uint32 dip, sys_logic_port_t port);
void mcast_igmp_send_grp_src_specific_query(uint16 vid, uint32 dip, uint32 *sip, uint16 numSrc, sys_logic_port_t port);
void mcast_igmp_build_general_query (sys_nic_pkt_t * pkt, sys_vid_t vid,uint16 pktLen, uint8 version, uint32 queryEnable,uint32 gdaddr, uint8 query_type);
void mcast_igmp_send_general_query(igmp_querier_entry_t* qryPtr, uint8 igmp_query_version);


void mcast_send_general_query(igmp_querier_entry_t *qryPtr, uint8 version);
void mcast_send_gs_query(igmp_group_entry_t *pGroup, sys_logic_port_t lPort);

#endif

