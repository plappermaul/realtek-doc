/*
 * Copyright (C) 2012 Realtek Semiconductor Corp.
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
 * Purpose : Definition of L34  API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) L34 Networking Interface configuration
 *           (2) L34 Routing Table configuration
 *           (3) L34 ARP Table configuration
 *           (4) L34 NAPT connection configuration
 *           (5) L34 System configuration
 *           (6) L34 NAPTR configuration
 *           (7) L34 NEXT-HOP configuration
 *           (8) L34 External_Internal IP configuration
 *           (9) L34 Binding configuration
 *           (10) L34 IPv6 configuration
 *
 */

#ifndef __RTK_L34_H__
#define __RTK_L34_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <rtk/l34_bind_config.h>
/*
 * Symbol Definition
 */
#define L34_MAC_PORT_MAX 		6
#define L34_EXT_PORT_MAX 		5
#define L34_DSLVC_PORT_MAX 		16
#define L34_TF_NAT_ALL			HAL_L34_NAPT_ENTRY_MAX()+1
#define L34_TF_PPP_ALL			HAL_L34_PPPOE_ENTRY_MAX()+1
#define L34_TF_ARP_ALL			HAL_L34_ARP_ENTRY_MAX()+1
/*
 * Data Declaration
 */

typedef enum rtk_l34_table_type_e
{
    L34_ROUTING_TABLE,
    L34_PPPOE_TABLE,
    L34_NEXTHOP_TABLE,
    L34_NETIF_TABLE,
    L34_INTIP_TABLE,
    L34_ARP_TABLE,
    L34_NAPTR_TABLE,
    L34_NAPT_TABLE,
    L34_IPV6_ROUTING_TABLE,
    L34_BINDING_TABLE,
    L34_IPV6_NEIGHBOR_TABLE,
    L34_WAN_TYPE_TABLE,
    L34_FLOW_ROUTING_TABLE,
    L34_TABLE_END
}rtk_l34_table_type_t;


typedef enum rtk_l34_hsba_mode_e
{
    L34_HSBA_BOTH_LOG,
    L34_HSBA_NO_LOG,
    L34_HSBA_LOG_ALL,
    L34_HSBA_LOG_FIRST_DROP,
    L34_HSBA_LOG_FIRST_PASS,
    L34_HSBA_LOG_FIRST_TO_CPU ,
    L34_HSBA_LOG,
    L34_HSBA_END
} rtk_l34_hsba_mode_t;


typedef enum rtk_l34_routing_type_e
{
    L34_PROCESS_CPU  = 0,
    L34_PROCESS_DROP = 1,
    L34_PROCESS_ARP =  2,
    L34_PROCESS_NH =   3,
    L34_PROCESS_END
}rtk_l34_routing_type_t;



typedef struct rtk_l34_netif_entry_s{
	uint8      isIpv6;
	uint8      isCtagIf;
	ipaddr_t   ipAddr;
	uint8      isL34;
	uint8      dslite_idx;
	uint8      dslite_state;
	uint16     mtu;
	uint8      enable_rounting;
	rtk_mac_t  gateway_mac;
	uint8      mac_mask;
	uint16     vlan_id;
	uint8      valid ;
#if defined(CONFIG_RG_FLOW_BASED_PLATFORM)	
	uint8 	   deny_ipv4;
	uint8      deny_ipv6;
#endif
}rtk_l34_netif_entry_t;


typedef struct rtk_l34_pppoe_entry_s{
    uint32          sessionID;
} rtk_l34_pppoe_entry_t;



typedef struct rtk_l34_arp_entry_s {
	uint32  index;
	ipaddr_t ipAddr;
	uint32  nhIdx;
	uint32  valid;
} rtk_l34_arp_entry_t;



typedef enum rtk_l34_extip_table_type_e
{
    L34_EXTIP_TYPE_NAPT,
    L34_EXTIP_TYPE_NAT,
    L34_EXTIP_TYPE_LP,
    L34_EXTIP_TYPE_END,
}rtk_l34_extip_table_type_t;





typedef struct rtk_l34_ext_intip_entry_s {
	    ipaddr_t 	extIpAddr;
	    ipaddr_t 	intIpAddr;
        uint8       valid;
        rtk_l34_extip_table_type_t   type;
        uint32 		nhIdx; /*index of next hop table*/
        uint8       prival;
        uint8       pri;
} rtk_l34_ext_intip_entry_t;



typedef enum rtk_l34_nexthop_type_e
{
    L34_NH_ETHER  = 0,
    L34_NH_PPPOE  = 1,
    L34_NH_END
}rtk_l34_nexthop_type_t;

typedef enum rtk_l34_nexthop_keep_pppoe_e
{
    L34_NH_PPPOE_REPLACE_BY_PPPIDX  = 0,
    L34_NH_PPPOE_KEEP_ORIGINAL = 1,
    L34_NH_PPPOE_KEEP_ORIGINAL_OR_ADD_BY_PPPIDX = 2,
    L34_NH_PPPOE_END
}rtk_l34_nexthop_keep_pppoe_t;

typedef struct rtk_l34_nexthop_entry_s {
	rtk_l34_nexthop_type_t  type;
	uint8  ifIdx;
	uint8  pppoeIdx;
    uint16 nhIdx;
    uint8  keepPppoe;
} rtk_l34_nexthop_entry_t;




typedef struct rtk_l34_routing_entry_s {
	    ipaddr_t ipAddr;
	    uint32   ipMask; /*0: 0x80000000  ... 30:0xFFFFFFFE 31:0xFFFFFFFF maks all*/
	    rtk_l34_routing_type_t process; /*0: CPU, 1:drop, 2:ARP(local route), 4:nexthop(global route)*/
        uint8  valid;
        uint8  internal;
        /* process = ARP */
        uint32 netifIdx;
        uint32 arpStart;
	    uint32 arpEnd;
        /* process = nexthop */
	    uint8 nhStart; /*exact index*/
	    uint8 nhNum;   /*exact number*/
	    uint8 nhNxt;
	    uint8 nhAlgo;
	    uint8 ipDomain;
	    uint32 rt2waninf;

} rtk_l34_routing_entry_t;




typedef enum rtk_l34_naptInboundType_e
{
    L34_NAPTR_ENTRY_INVALID          = 0,
    L34_NAPTR_ENTRY_NO_HASH          = 1,
    L34_NAPTR_ENTRY_IP_PORT_HASH     = 2,
    L34_NAPTR_ENTRY_IP_HASH          = 3,
    L34_NAPTR_ENTRY_END
} rtk_l34_naptInboundType_t;


typedef struct rtk_l34_naptInbound_entry_s {
	uint32  intIp;
    uint16  intPort;
    uint16  remHash;
    uint8   extIpIdx;
    uint8	extPortLSB;
    uint8	extPortHSB;
	uint8	isTcp;
    uint8 	valid;
    uint8  	priValid;
    uint8  	priId;
} rtk_l34_naptInbound_entry_t;


typedef struct rtk_l34_naptOutbound_entry_s {
    uint16	    hashIdx;
    uint8	    valid;
    uint8	    priValid;
    uint8	    priValue;
} rtk_l34_naptOutbound_entry_t;



/*IPv6 routing table*/

typedef enum rtk_l34_ipv6RouteType_e
{
    L34_IPV6_ROUTE_TYPE_TRAP    = 0,
    L34_IPV6_ROUTE_TYPE_DROP    = 1,
    L34_IPV6_ROUTE_TYPE_LOCAL   = 2,
    L34_IPV6_ROUTE_TYPE_GLOBAL  = 3,
    L34_IPV6_ROUTE_TYPE_END
} rtk_l34_ipv6RouteType_t;


typedef struct rtk_ipv6Routing_entry_s {
    uint8	                     valid;
    rtk_l34_ipv6RouteType_t      type;
    uint32	                     nhOrIfidIdx;  /*local: index to interface table   global: index to nexthop table*/
    uint32	                     ipv6PrefixLen;
    rtk_ipv6_addr_t              ipv6Addr;
    uint32                       rt2waninf;
} rtk_ipv6Routing_entry_t;




/*IPv6 Neighbor table*/
typedef struct rtk_ipv6Neighbor_entry_s {
    uint8	    valid;
    uint32      l2Idx;
    uint8	    ipv6RouteIdx;
    uint64	    ipv6Ifid;
} rtk_ipv6Neighbor_entry_t;



typedef enum rtk_l34_pppoeAct_t
{
    L34_PPPOE_ACT_NO     = 0,
    L34_PPPOE_ACT_REMOVE = 1,
    L34_PPPOE_ACT_MODIFY = 2,
    L34_PPPOE_ACT_KEEP   = 3,
    L34_PPPOE_ACT_END
} rtk_l34_pppoeAct_t;




typedef struct rtk_l34_ipmcTrans_entry_s
{
    uint32  netifIdx;
    uint32  sipTransEnable;
    uint32  extipIdx;
    rtk_l34_pppoeAct_t  pppoeAct;
    uint32  pppoeIdx;
	rtk_portmask_t  untagMbr;
}rtk_l34_ipmcTrans_entry_t;


typedef enum rtk_l34_flowRoute_type_e
{
    L34_FLOWRT_TYPE_CPU  	= 0,
    L34_FLOWRT_TYPE_DROP 	= 1,
    L34_FLOWRT_TYPE_LOCAL 	= 2,
    L34_FLOWRT_TYPE_GLOBAL 	= 3,
    L34_FLOWRT_TYPE_END
} rtk_l34_flowRoute_type_t;

typedef struct rtk_l34_flowRoute_entry_s
{
	uint32 index;       /*IPv6 index must be even number, 0,2,4...etc. And occupy 2 entries for 1 flow*/
	uint8  valid;
	uint8  isIpv4;      /*0: IPv6, 1: IPv4*/
	union{
		rtk_ip_addr_t ipv4;
		rtk_ipv6_addr_t ipv6;
	}sip;
	union{
		rtk_ip_addr_t ipv4;
		rtk_ipv6_addr_t ipv6;
	}dip;
	uint16	sport;
	uint16 	dport;
	uint8 	isTcp;
	uint32	nexthopIdx;
	rtk_l34_flowRoute_type_t	process;
	uint8	pri_valid;
	uint8	priority;
}rtk_l34_flowRoute_entry_t;


typedef enum rtk_l34_hsb_type_e
{
    L34_HSB_ETHERNET,
    L34_HSB_RESERVED_PPTP,
    L34_HSB_IPV4,
    L34_HSB_ICMP,
    L34_HSB_IGMP,
    L34_HSB_TCP,
    L34_HSB_UDP,
    L34_HSB_IPV6,
    L34_HSB_END
} rtk_l34_hsb_type_t;


typedef enum rtk_l34_hsa_pppoe_act_e
{
    L34_HSA_PPPOE_INACT,
    L34_HSA_PPPOE_TAGGING,
    L34_HSA_PPPOE_REMOVE,
    L34_HSA_PPPOE_MODIFY,
    L34_HSA_END
} rtk_l34_hsa_pppoe_act_t;


typedef enum rtk_l34_l4_trf_e
{
    RTK_L34_L4TRF_TABLE0  = 0,
    RTK_L34_L4TRF_TABLE1  = 1,
    RTK_L34_L4TRF_TABLE_END,
} rtk_l34_l4_trf_t;


typedef enum rtk_l34_arp_trf_e
{
    RTK_L34_ARPTRF_TABLE0  = 0,
    RTK_L34_ARPTRF_TABLE1  = 1,
    RTK_L34_ARPTRF_TABLE_END,
} rtk_l34_arp_trf_t;


typedef struct rtk_l34_hsb_s
{
	uint32  da_traslate;
	uint32  cfi_1;
	uint32  sa_idx;
	uint32  sa_idx_valid;
	uint32  ip6_type_ext;
	uint32  policy_nh_idx;
	uint32  is_policy_route;
    uint32  isFromWan;
    uint32  l2bridge;
    uint32  ipfrag_s;
    uint32  ipmf;
    uint32  l4_chksum_ok;
    uint32  l3_chksum_ok;
    uint32  cpu_direct_tx;
    uint32  udp_no_chksum;
    uint32  parse_fail;
    uint32  pppoe_if;
    uint32  svlan_if;
    uint32  ttls;   /*0b00 TTL=0, 0b01 TTL=1, 0b10 TTL>1 */
    rtk_l34_hsb_type_t  type;
    uint32   tcp_flag;
    uint32  cvlan_if;
    uint32   spa;
    uint32  cvid;
    uint32  len;
    uint32  dport_l4chksum;
    uint32  pppoe_id;
    rtk_ipv6_addr_t dip;  /*For IPv4 and IPv6*/
    rtk_ipv6_addr_t sip;  /*For IPv4 and IPv6*/
    uint32  sport_icmpid_chksum;
    rtk_mac_t  dmac;
}rtk_l34_hsb_t;


typedef struct rtk_l34_hsa_s
{
    uint32  dslite_Idx;
    uint32  dslite_valid;
    uint32  is_policy;
    uint32  pppoeKeep;
    uint32  bindVidTrans;
    uint32  interVlanIf;
    uint32  reason;
    uint32  l34trans;
    uint32  l2trans;
    uint32  action;
    uint32  l4_pri_valid;
    uint32  l4_pri_sel;
    uint32  frag;
    uint32  difid;
    rtk_l34_hsa_pppoe_act_t  pppoe_if;
    uint32  pppid_idx;
    uint32  nexthop_mac_idx;
    uint32  l4_chksum;
    uint32  l3_chksum;
    uint32  dvid;
    uint32  port;
    rtk_ip_addr_t ip;
}rtk_l34_hsa_t;


typedef enum rtk_l34_dsliteFrag_act_s
{
	RTK_L34_DSLITE_FRAG_ACT_TRAP = 0,
	RTK_L34_DSLITE_FRAG_ACT_DROP,
	RTK_L34_DSLITE_FRAG_ACT_FWD,
	RTK_L34_DSLITE_FRAG_ACT_END
}rtk_l34_dsliteFrag_act_t;

typedef enum rtk_l34_dsliteNextHeader_act_s
{
	RTK_L34_DSLITE_NH_ACT_TRAP_EXTHEADER_OVER_0 = 0,
	RTK_L34_DSLITE_NH_ACT_TRAP_EXTHEADER_OVER_240,
	RTK_L34_DSLITE_NH_ACT_END
}rtk_l34_dsliteNextHeader_act_t;

typedef enum rtk_l34_dsliteTcOpt_s
{
	RTK_L34_DSLITE_TC_OPT_ASSIGN = 0,
	RTK_L34_DSLITE_TC_OPT_COPY_FROM_TOS,
	RTK_L34_DSLITE_TC_OPT_END
}rtk_l34_dsliteTcOpt_t;

typedef enum rtk_l34_dsliteUnmatch_act_s
{
	RTK_L34_DSLITE_UNMATCH_ACT_DROP = 0,
	RTK_L34_DSLITE_UNMATCH_ACT_TRAP,
	RTK_L34_DSLITE_UNMATCH_ACT_END
}rtk_l34_dsliteUnmatch_act_t;

typedef struct rtk_l34_dsliteInf_entry_s
{
	uint32 index;
	uint32 hopLimit;
	uint32 flowLabel;
	rtk_l34_dsliteTcOpt_t tcOpt;
	uint32 tc;
	rtk_enable_t valid;
	rtk_ipv6_addr_t ipAftr;
	rtk_ipv6_addr_t ipB4;
}rtk_l34_dsliteInf_entry_t;


typedef struct rtk_l34_dsliteMc_entry_s
{
	uint32 index;
	rtk_ipv6_addr_t ipUPrefix64Mask;
	rtk_ipv6_addr_t ipUPrefix64;
	rtk_ipv6_addr_t ipMPrefix64Mask;
	rtk_ipv6_addr_t ipMPrefix64;
}rtk_l34_dsliteMc_entry_t;

typedef struct rtk_l34_mib_s
{
	uint32 ifIndex;
	uint64 ifInOctets;
	uint32 ifInUcstPkts;
	uint64 ifOutOctets;
	uint32 ifOutUcstPkts;
}rtk_l34_mib_t;

/*
 * Function Declaration
 */



/* Module Name    : L34  */
/* Sub-module Name: Network Interface Table */

/* Function Name:
 *      rtk_l34_init
 * Description:
 *      Initialize l34 module.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Must initialize l34  module before calling any l34  APIs.
 */
extern int32
rtk_l34_init(void);


/* Function Name:
 *      rtk_l34_netifTable_set
 * Description:
 *      Set netif table entry
 * Input:
 *      idx - index of netif table
 *      *netifEntry -point of netif entry 
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
rtk_l34_netifTable_set(uint32 idx, rtk_l34_netif_entry_t *netifEntry);


/* Function Name:
 *      rtk_l34_netifTable_get
 * Description:
 *      Get netif table entry
 * Input:
 *      idx - index of netif table
 * Output:
 *      *netifEntry -point of netif entry result
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
rtk_l34_netifTable_get(uint32 idx, rtk_l34_netif_entry_t *netifEntry);


/* Sub-module Name: ARP Table */

/* Function Name:
 *      rtk_l34_arpTable_set
 * Description:
 *      Set arp table entry
 * Input:
 *      idx - index of arp table
 *      *arpEntry -point of arp entry
 * Output:
 *     None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
rtk_l34_arpTable_set(uint32 idx, rtk_l34_arp_entry_t *arpEntry);


/* Function Name:
 *      rtk_l34_arpTable_get
 * Description:
 *      Get arp table entry
 * Input:
 *      idx - index of arp table
 * Output:
 *      *arpEntry -point of arp entry result
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
rtk_l34_arpTable_get(uint32 idx, rtk_l34_arp_entry_t *arpEntry);


/* Function Name:
 *      rtk_l34_arpTable_del
 * Description:
 *      Delete arp table entry
 * Input:
 *      idx - index of arp table
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
rtk_l34_arpTable_del(uint32 idx);



/* Sub-module Name: PPPoE Table */

/* Function Name:
 *      rtk_l34_pppoeTable_set
 * Description:
 *      Set PPPoE table entry
 * Input:
 *      idx - index of PPPoE table
 *      *pppEntry -point of PPPoE entry
 * Output:
 *     None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
rtk_l34_pppoeTable_set(uint32 idx, rtk_l34_pppoe_entry_t *pppEntry);


/* Function Name:
 *      rtk_l34_pppoeTable_get
 * Description:
 *      Get PPPoE table entry
 * Input:
 *      idx - index of PPPoE table
 * Output:
 *      *pppEntry -point of PPPoE entry result
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
rtk_l34_pppoeTable_get(uint32 idx, rtk_l34_pppoe_entry_t *pppEntry);



/* Sub-module Name: Routing Table */

/* Function Name:
 *      rtk_l34_routingTable_set
 * Description:
 *      Set Routing table entry
 * Input:
 *      idx - index of Routing table
 *      *routEntry -point of Routing entry
 * Output:
 *     None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
rtk_l34_routingTable_set(uint32 idx, rtk_l34_routing_entry_t *routEntry);


/* Function Name:
 *      rtk_l34_routingTable_get
 * Description:
 *      Get Routing table entry
 * Input:
 *      idx - index of Routing table
 * Output:
 *      *routEntry -point of Routing entry result
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
rtk_l34_routingTable_get(uint32 idx, rtk_l34_routing_entry_t *routEntry);


/* Function Name:
 *      rtk_l34_routingTable_del
 * Description:
 *      Delete arp Routing entry
 * Input:
 *      idx - index of Routing table
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
rtk_l34_routingTable_del(uint32 idx);



/* Sub-module Name: Next-Hop Table */

/* Function Name:
 *      rtk_l34_nexthopTable_set
 * Description:
 *      Set  Next-Hop table entry
 * Input:
 *      idx - index of  Next-Hop table
 *      *nextHopEntry -point of  Next-Hop entry
 * Output:
 *     None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
rtk_l34_nexthopTable_set(uint32 idx, rtk_l34_nexthop_entry_t *nextHopEntry);


/* Function Name:
 *      rtk_l34_nexthopTable_get
 * Description:
 *      Get Next-Hop table entry
 * Input:
 *      idx - index of Next-Hop table
 * Output:
 *      *nextHopEntry -point of Next-Hop entry result
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
rtk_l34_nexthopTable_get(uint32 idx, rtk_l34_nexthop_entry_t *nextHopEntry);


/* Sub-module Name: External_Internal IP Table */

/* Function Name:
 *      rtk_l34_extIntIPTable_set
 * Description:
 *      Set  External_Internal IP table entry
 * Input:
 *      idx - index of  External_Internal IP table
 *      *extIpEntry -point of External_Internal IP entry
 * Output:
 *     None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
rtk_l34_extIntIPTable_set(uint32 idx, rtk_l34_ext_intip_entry_t *extIpEntry);


/* Function Name:
 *      rtk_l34_extIntIPTable_get
 * Description:
 *      Get External_Internal IP table entry
 * Input:
 *      idx - index of External_Internal IP table
 * Output:
 *      *extIpEntry -point of External_Internal IP entry result
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
rtk_l34_extIntIPTable_get(uint32 idx, rtk_l34_ext_intip_entry_t *extIpEntry);


/* Function Name:
 *      rtk_l34_extIntIPTable_del
 * Description:
 *      Delete arp  External_Internal entry
 * Input:
 *      idx - index of  External_Internal table
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
rtk_l34_extIntIPTable_del(uint32 idx);


/* Sub-module Name: NAPTR  Table */

/* Function Name:
 *      rtk_l34_naptInboundTable_set
 * Description:
 *      Set  NAPTR table entry
 * Input:
 *      forced - force set to NAPTR table
 *      *naptrEntry - value of NAPTR table entry
 * Output:
 *     None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
rtk_l34_naptInboundTable_set(int8 forced, uint32 idx,rtk_l34_naptInbound_entry_t *naptrEntry);


/* Function Name:
 *      rtk_l34_naptInboundTable_get
 * Description:
 *      Get napt table entry
 * Input:
 *      idx - index of NAPTR table
 * Output:
 *      *naptrEntry -point of NAPTR entry result
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
rtk_l34_naptInboundTable_get(uint32 idx,rtk_l34_naptInbound_entry_t *naptrEntry);


/* Sub-module Name: NAPT  Table */

/* Function Name:
 *      rtk_l34_naptOutboundTable_set
 * Description:
 *      Set  napt table entry
 * Input:
 *      forced - force set to napt table
 *      *naptEntry - value of napt table entry
 * Output:
 *     None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
rtk_l34_naptOutboundTable_set(int8 forced, uint32 idx,rtk_l34_naptOutbound_entry_t *naptEntry);


/* Function Name:
 *      rtk_l34_naptOutboundTable_get
 * Description:
 *      Get napt table entry
 * Input:
 *      idx - index of napt table
 * Output:
 *      *naptEntry -point of napt entry result
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
rtk_l34_naptOutboundTable_get(uint32 idx,rtk_l34_naptOutbound_entry_t *naptEntry);


/* Sub-module Name: IPMC Transfer Table */

/* Function Name:
 *      rtk_l34_ipmcTransTable_set
 * Description:
 *      Set  IPMC Transfer table entry
 * Input:
 *      forced - force set to IPMC Transfer table
 *      *ipmcEntry - value of  IPMC Transfer entry
 * Output:
 *     None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
rtk_l34_ipmcTransTable_set(uint32 idx, rtk_l34_ipmcTrans_entry_t *ipmcEntry);


/* Function Name:
 *      rtk_l34_ipmcTransTable_get
 * Description:
 *      Get IPMC Transfer table entry
 * Input:
 *      idx - index of IPMC Transfer table
 * Output:
 *      *ipmcEntry -point of IPMC Transfer entry result
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
rtk_l34_ipmcTransTable_get(uint32 idx, rtk_l34_ipmcTrans_entry_t *ipmcEntry);

/* Sub-module Name: L34 System Configure */

/* Function Name:
 *      rtk_l34_table_reset
 * Description:
 *      Reset a specific  L34 table entries
 * Input:
 *      type - L34 Table type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
rtk_l34_table_reset(rtk_l34_table_type_t type);



/* Sub-module Name: IPv6 Routing Table */


/* Function Name:
 *      rtk_l34_ipv6RoutingTable_set
 * Description:
 *      Set a IPv6 routing entry by idx.
 * Input:
 *      idx     - index of ipv6 routing entry
 *      *ipv6RoutEntry - point of ipv6 routing table
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
rtk_l34_ipv6RoutingTable_set(uint32 idx, rtk_ipv6Routing_entry_t *ipv6RoutEntry);


/* Function Name:
 *      rtk_l34_ipv6RoutingTable_get
 * Description:
 *      Get a IPv6 routing entry by idx.
 * Input:
 *      idx     - index of ipv6 routing entry
 * Output:
 *      *ipv6RoutEntry - point of ipv6 routing table
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
rtk_l34_ipv6RoutingTable_get(uint32 idx, rtk_ipv6Routing_entry_t *ipv6RoutEntry);



/* Sub-module Name: IPv6 Neighbor Table */

/* Function Name:
 *      rtk_l34_ipv6NeighborTable_set
 * Description:
 *      Set neighbor table
 * Input:
 *      idx - index of neighbor table
 *      *ipv6NeighborEntry - point of neighbor data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
rtk_l34_ipv6NeighborTable_set(uint32 idx,rtk_ipv6Neighbor_entry_t *ipv6NeighborEntry);


/* Function Name:
 *      rtk_l34_ipv6NeighborTable_get
 * Description:
 *      Get neighbor table
 * Input:
 *      idx - index of neighbor table
 * Output:
 *      *ipv6NeighborEntry - point of neighbor data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
rtk_l34_ipv6NeighborTable_get(uint32 idx,rtk_ipv6Neighbor_entry_t *ipv6NeighborEntry);


/* Function Name:
 *      rtk_l34_hsabMode_set
 * Description:
 *      Set L34 hsab mode
 * Input:
 *      hsabMode - L34 hsab 
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
rtk_l34_hsabMode_set(rtk_l34_hsba_mode_t hsabMode);


/* Function Name:
 *      rtk_l34_hsabMode_get
 * Description:
 *      Get L34 hsab mode
 * Input:
 *      None
 * Output:
 *      *pHsabMode - point of L34 hsab 
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
rtk_l34_hsabMode_get(rtk_l34_hsba_mode_t *pHsabMode);



/* Function Name:
 *      rtk_l34_hsaData_get
 * Description:
 *      Get L34 hsa data
 * Input:
 *      None
 * Output:
 *      pHsaData - point of hsa data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
rtk_l34_hsaData_get(rtk_l34_hsa_t *pHsaData);


/* Function Name:
 *      rtk_l34_hsbData_get
 * Description:
 *      Get L34 hsab mode
 * Input:
 *      None
 * Output:
 *      pHsaData - point of hsa data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
rtk_l34_hsbData_get(rtk_l34_hsb_t *pHsbData);

/* Function Name:
 *      rtk_l34_hsdState_set
 * Description:
 *      Set L34 hsd state
 * Input:
 *      hsdState - L34 hsd state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
rtk_l34_hsdState_set(rtk_enable_t hsdState);


/* Function Name:
 *      rtk_l34_hsdState_get
 * Description:
 *      Get L34 hsab mode
 * Input:
 *      None
 * Output:
 *      phsdState - point of hsd state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
rtk_l34_hsdState_get(rtk_enable_t *phsdState);


/* Function Name:
 *      rtk_l34_hsdState_get
 * Description:
 *      Get L34 hsab mode
 * Input:
 *      None
 * Output:
 *      phsdState - point of hsd state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
rtk_l34_hsdState_get(rtk_enable_t *phsdState);


/* Function Name:
 *      rtk_l34_hwL4TrfWrkTbl_set
 * Description:
 *      Set HW working table id for L4 trf.
 * Input:
 *      rtk_l34_l4_trf_t l4TrfTable
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 				- Success
 *      RT_ERR_SMI  			- SMI access error
 * Note:
 *      None
 */
extern int32 
rtk_l34_hwL4TrfWrkTbl_set(rtk_l34_l4_trf_t l4TrfTable);


/* Function Name:
 *      rtk_l34_hwL4TrfWrkTbl_get
 * Description:
 *      Get HW working table id for L4 trf.
 * Input:
 *      None
 * Output:
 *      rtk_l34_l4_trf_t *pl4TrfTable
 * Return:
 *      RT_ERR_OK 				- Success
 *      RT_ERR_SMI  			- SMI access error
 * Note:
 *      None
 */
extern int32 
rtk_l34_hwL4TrfWrkTbl_get(rtk_l34_l4_trf_t *pl4TrfTable);

/* Function Name:
 *      rtk_l34_l4TrfTb_get
 * Description:
 *      Get HW working table id for L4 trf.
 * Input:
 *      l4TrfTable - table index
 *      l4EntryIndex - index of l4 table that went to get
 * Output:
 *      pIndicator - indicator for result of state
 * Return:
 *      RT_ERR_OK 				- Success
 *      RT_ERR_SMI  			- SMI access error
 * Note:
 *      None
 */
extern int32 
rtk_l34_l4TrfTb_get(rtk_l34_l4_trf_t l4TrfTable,uint32 l4EntryIndex,rtk_enable_t *pIndicator);


/* Function Name:
 *      rtk_l34_hwL4TrfWrkTbl_Clear
 * Description:
 *      Clear HW working table id for ARP trf.
 * Input:
 *      l4TrfTable
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 				- Success
 *      RT_ERR_SMI  			- SMI access error
 * Note:
 *      None
 */
extern int32 
rtk_l34_hwL4TrfWrkTbl_Clear(rtk_l34_l4_trf_t l4TrfTable);


/* Function Name:
 *      rtk_l34_hwArpTrfWrkTbl_set
 * Description:
 *      Set HW working table id for ARP trf.
 * Input:
 *      rtk_l34_arp_trf_t arpTrfTable
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 				- Success
 *      RT_ERR_SMI  			- SMI access error
 * Note:
 *      None
 */
extern int32 
rtk_l34_hwArpTrfWrkTbl_set(rtk_l34_arp_trf_t arpTrfTable);


/* Function Name:
 *      rtk_l34_hwArpTrfWrkTbl_get
 * Description:
 *      Get HW working table id for ARP trf.
 * Input:
 *      None
 * Output:
 *      rtk_l34_l4_trf_t *pArpTrfTable
 * Return:
 *      RT_ERR_OK 				- Success
 *      RT_ERR_SMI  			- SMI access error
 * Note:
 *      None
 */
extern int32 
rtk_l34_hwArpTrfWrkTbl_get(rtk_l34_arp_trf_t *pArpTrfTable);

/* Function Name:
 *      rtk_l34_arpTrfTb_get
 * Description:
 *      Get HW working table id for ARP trf.
 * Input:
 *      arpTrfTable - table index
 *      arpEntryIndex - index of l4 table that went to get
 * Output:
 *      pIndicator - indicator for result of state
 * Return:
 *      RT_ERR_OK 				- Success
 *      RT_ERR_SMI  			- SMI access error
 * Note:
 *      None
 */
extern int32 
rtk_l34_arpTrfTb_get(rtk_l34_arp_trf_t arpTrfTable,uint32 arpEntryIndex,rtk_enable_t *pIndicator);


/* Function Name:
 *      rtk_l34_hwArpTrfWrkTbl_Clear
 * Description:
 *      Clear HW working table id for ARP trf.
 * Input:
 *      l4TrfTable
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 				- Success
 *      RT_ERR_SMI  			- SMI access error
 * Note:
 *      None
 */
extern int32 
rtk_l34_hwArpTrfWrkTbl_Clear(rtk_l34_arp_trf_t arpTrfTable);

/* Function Name:
 *      rtk_l34_ip6mcRoutingTransIdx_set
 * Description:
 *      Set per-port IPv6 Multicast routing translation table index
 * Input:
 *      idx          - table index
 *      port         - port id
 *      ipmcTransIdx - ip multicast translation table index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_ENTRY_INDEX  - invalid index
 * Note:
 *      None
 */
extern int32
rtk_l34_ip6mcRoutingTransIdx_set(uint32 idx, rtk_port_t port, uint32 ipmcTransIdx);

/* Function Name:
 *      rtk_l34_ip6mcRoutingTransIdx_set
 * Description:
 *      Get per-port IPv6 Multicast routing translation table index
 * Input:
 *      idx          - table index
 *      port         - port id
 * Output:
 *      pIpmcTransIdx - ip multicast translation table index to be get
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_ENTRY_INDEX  - invalid index
 * Note:
 *      None
 */
extern int32
rtk_l34_ip6mcRoutingTransIdx_get(uint32 idx, rtk_port_t port, uint32 *pIpmcTransIdx);

/* Function Name:
 *      rtk_l34_flowRouteTable_set
 * Description:
 *      Set Flow Routing Table.
 * Input:
 *		entry  : entry content
 * Output:
 *		None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_ENTRY_INDEX  - invalid index
 * Note:
 *      None
 */
extern int32
rtk_l34_flowRouteTable_set(rtk_l34_flowRoute_entry_t *pEntry);

/* Function Name:
 *      rtk_l34_flowRouteTable_get
 * Description:
 *      Get Flow Routing Table.
 * Input:
 *		None
 * Output:
 *		entry  : entry content
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_ENTRY_INDEX  - invalid index
 * Note:
 *      None
 */
extern int32
rtk_l34_flowRouteTable_get(rtk_l34_flowRoute_entry_t *pEntry);

/* Function Name:
 *      rtk_l34_flowTrfIndicator_get
 * Description:
 *      get  flow routing traffic indicator by index
 * Input:
 *      index: traffic table index
 * Output:
 * 	  *pArpIndicator	- point of traffic indicator for flow routing 
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
rtk_l34_flowTrfIndicator_get(uint32 index, rtk_enable_t *pFlowIndicator);

/* Function Name:
 *      rtk_l34_dsliteInfTable_set
 * Description:
 *      Set DS-Lite interface table entry
 * Input:
 *      *dsliteInfEntry - point of DS-Lite interface entry 
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
rtk_l34_dsliteInfTable_set(rtk_l34_dsliteInf_entry_t *pDsliteInfEntry);

/* Function Name:
 *      rtk_l34_dsliteInfTable_get
 * Description:
 *      Get DS-Lite interface table entry
 * Input:
 *      dsliteInfEntry->index - index of DS-Lite interface table
 * Output:
 *      *dsliteInfEntry - point of DS-Lite interface table entry result
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
rtk_l34_dsliteInfTable_get(rtk_l34_dsliteInf_entry_t *pDsliteInfEntry);

/* Function Name:
 *      rtk_l34_dsliteMcTable_set
 * Description:
 *      Set DS-Lite multicast table entry
 * Input:
 *      *dsliteMcEntry - point of DS-Lite multicast table entry 
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
rtk_l34_dsliteMcTable_set(rtk_l34_dsliteMc_entry_t *pDsliteMcEntry);

/* Function Name:
 *      rtk_l34_dsliteMcTable_get
 * Description:
 *      Get DS-Lite multicast table entry
 * Input:
 *      dsliteMcEntry->index - index of DS-Lite multicast table 
 * Output:
 *      *dsliteMcEntry - point of DS-Lite multicast table entry result
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
rtk_l34_dsliteMcTable_get(rtk_l34_dsliteMc_entry_t *pDsliteMcEntry);

/* Function Name:
 *      rtk_l34_dsliteState_set
 * Description:
 *      Set DS-Lite state
 * Input:
 *      ctrlType  - Control type
 *      act - action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
rtk_l34_dsliteControl_set(rtk_l34_dsliteCtrlType_t ctrlType, uint32 act);

/* Function Name:
 *      rtk_l34_dsliteState_get
 * Description:
 *      Get DS-Lite state
 * Input:
 *      ctrlType  - Control type
 * Output:
 *      pAct - action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
rtk_l34_dsliteControl_get(rtk_l34_dsliteCtrlType_t ctrlType, uint32 *pAct);

/* Function Name:
 *      rtk_stat_l34_mib_get
 * Description:
 *      Get per L34 interface counters
 * Input:
 *      ifIndex    - L34 interface index
 * Output:
 *      counters   - counters
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
rtk_l34_mib_get(rtk_l34_mib_t *pL34Cnt);

/* Function Name:
 *      rtk_stat_l34_mib_reset
 * Description:
 *      Reset per L34 interface counters
 * Input:
 *      ifIndex    - L34 interface index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
rtk_l34_mib_reset(uint32 ifIndex);

/* Function Name:
 *      rtk_l34_lutLookupMiss_set
 * Description:
 *      set L34 MAC table lookup miss action
 * Input:
 * 	  lutMissAct	- L34 MAC table lookup miss action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
rtk_l34_lutLookupMiss_set(rtk_l34_lutMissAct_t lutMissAct);

/* Function Name:
 *      rtk_l34_lutLookupMiss_get
 * Description:
 *      Get L34 MAC table lookup miss action
 * Input:
 * 	    None
 * Output:
 *      lutMissAct	- L34 MAC table lookup miss action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
rtk_l34_lutLookupMiss_get(rtk_l34_lutMissAct_t *pLutMissAct);
#endif /* __RTK_L34_H__ */
