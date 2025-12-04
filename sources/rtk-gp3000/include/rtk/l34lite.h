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
 * Purpose : Definition of L34 Lite API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) L34 Networking Interface configuration
 *           (2) L34 Routing Table configuration
 *           (3) L34 ARP Table configuration
 *           (4) L34 NAPT connection configuration
 *           (5) L34 Global configuration
 *           (6) L34 IPv6 Routing Table
 *      (7) L34 IPv6 Neighbor Table
 *
 */

#ifndef __RTK_L34_LIGHT_H__
#define __RTK_L34_LIGHT_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <rtk/vlan.h>
#include <rtk/l34.h>
/*
 * Symbol Definition
 */
#define L34_MTU_MAX             16384
#define L34_DEFAULT_MTU     1500
#define L34_DEFAULT_VLAN        1
#define L34_DEFAULT_MACMASK 0x7
#define L34_DEFAULT_IPDOMAIN    6
#define L34_DEFAULT_NHALGO  0
#define L34_DEFAULT_NHNUM       0
#define L34_GMAC_NOMASK     7
#define L34_IF_NUM_MAX          6
#define L34_NH_TABLE_SIZE       16
#define L34_DEFAULT_ROUTE       (HAL_L34_ROUTING_ENTRY_MAX() - 1)
#define L34_STATIC_ROUTE        (HAL_L34_ROUTING_ENTRY_MAX() - 2)
#define L34_ROUTE_TABLE_SIZE    8
#define L34_VLAN_PRI_DISABLE    8
#define L34_IPV6_ROUTE_MAX_SIZE  4


#define L34_PROTO_TCP 6
#define L34_PROTO_UDP 17
/*
 * Data Declaration
 */

typedef enum rtk_l34_netifType_e{
    L34_NETIF_TYPE_START=0,
    L34_NETIF_TYPE_WAN=L34_NETIF_TYPE_START,
    L34_NETIF_TYPE_LAN,
    L34_NETIF_TYPE_END,
}rtk_l34_netifType_t;


typedef enum rtk_l34_natType_e{
    L34_NAT_TYPE_START=0,
    L34_NAPT_PORT_RESTRICTED=L34_NAT_TYPE_START,
    L34_NAPT_RESTRICTED_CONE,
    L34_NAPT_FULL_CONE,
    L34_LOCAL_PUBLIC,
    L34_NAT,
    L34_NAT_TYPE_END
}rtk_l34_natType_t;


typedef enum rtk_l34_routeType_e{
    L34_ROUTE_TYPE_START=0,
    L34_ROUTE_CPU=L34_ROUTE_TYPE_START,
    L34_ROUTE_DROP,
    L34_ROUTE_STATIC,                       /*will set to route index 6*/
    L34_ROUTE_DEFAULT,                      /*will set to route index 7*/
    L34_ROUTE_LOCAL,
    L34_ROUTE_POLICY,
    L34_ROUTE_PPP,
    L34_ROUTE_TYPE_END
}rtk_l34_routeType_t;


typedef enum rtk_l34_direct_e{
    L34_DIR_START=0,
    L34_DIR_UPSTREAM=L34_DIR_START,
    L34_DIR_DOWNSTREAM,
    L34_DIR_END
}rtk_l34_direct_t;


typedef struct rtk_l34_pppInfo_s{
    rtk_enable_t        pppState;
    rtk_enable_t        passThroughState;
    uint32          sessionId;
    rtk_mac_t       serverMac;
}rtk_l34_pppInfo_t;

/*for external ip & local route table */
typedef struct rtk_l34_ipInfo_s{
    rtk_ip_addr_t       ipaddr;     /*need get from local route*/
    uint32          ipmask;
    rtk_ip_addr_t       internalIp;
    rtk_l34_natType_t   natType;
    rtk_pri_t           defaultPri;
    rtk_l34_netifType_t netifType;  /*wan: external interface, lan: internal interface*/

}rtk_l34_ipInfo_t;

typedef struct rtk_l34_l2NetIfInfo_s{
    uint32          mtu;
    rtk_mac_t       ifmac;
    rtk_vlan_t      vid;
    rtk_enable_t        ifState;
    rtk_enable_t        routingState;
}rtk_l34_l2NetIfInfo_t;


typedef struct rtk_l34_routeTable_s{
    uint32              netifId;
    rtk_ip_addr_t           ipaddr;
    uint32                      ipmask;
    rtk_mac_t           gatewayMac;
}rtk_l34_routeTable_t;


typedef struct rtk_l34_routeInfo_s{
    uint32                  id;
    rtk_l34_routeTable_t        routeTable;
    rtk_l34_routeType_t     routeType;
    rtk_enable_t                vaild;
    uint32                  arpStart;   /*arp table assign range:start*/
    uint32                  arpEnd;     /*arp table assign rnage:end*/
    uint32                  nhId;      /*next hop information*/
    uint32                  l2tableId;
}rtk_l34_routeInfo_t;


typedef struct rtk_l34_arpInfo_s{

    uint32      arpId;
    uint32      l2Id;
    uint32      netifId;
    rtk_mac_t   mac;

}rtk_l34_arpInfo_t;



typedef struct rtk_l34_tuple_s{
    uint16      srcport;
    uint16      dstport;
    uint32      proto;
    rtk_ip_addr_t   srcIp;
    rtk_ip_addr_t   dstIp;
}rtk_l34_tuple_t;



typedef struct rtk_l34_connectInfo_s{
    rtk_l34_direct_t    direction;
    rtk_l34_tuple_t     tuple;
    rtk_ip_addr_t       natIp;
    rtk_l34_natType_t  natType;
    uint16          natport;
    uint32          netifId;
    uint32          naptId;
    uint32          naptrId;
    rtk_enable_t        priValied;
    uint32          priValue;
    rtk_enable_t        trafficIndicator;
}rtk_l34_connectInfo_t;


typedef struct rtk_l34_global_cfg_s{
    uint32      macPortMap[L34_MAC_PORT_MAX];
    uint32      extPortMap[L34_EXT_PORT_MAX];
    uint32      dslVcPortMap[L34_DSLVC_PORT_MAX];
    rtk_enable_t    globalState[L34_GLOBAL_STATE_END];
    rtk_l34_lookupMode_t        lookupMode;
    rtk_l34_wanRouteMode_t  wanRouteMode;
}rtk_l34_global_cfg_t;


typedef struct rtk_l34_route6Table_s{

    uint32                netifId;
    rtk_ipv6_addr_t       ipaddr;
    uint32                prefixLen;
    rtk_mac_t             gatewayMac;

}rtk_l34_route6Table_t;


typedef struct rtk_l34_rouet6Info_s{
    uint32                routeId;
    uint32                nhId;
    uint32                l2Id;
    rtk_enable_t              valid;
    rtk_l34_route6Table_t     route6Table;
    rtk_l34_ipv6RouteType_t route6Type;
}rtk_l34_route6Info_t;


typedef struct rtk_l34_neigh6Info_s{
    uint32              neighborId;
    uint32              l2Id;
    uint32              netifId;
    uint32              routeId;
    rtk_mac_t           mac;
    rtk_ipv6_addr_t     ipaddr;
    uint64              neighIfId;
}rtk_l34_neigh6Info_t;


/*for network interface information*/
typedef struct rtk_l34_netifInfo_s{
    uint32              id;
    rtk_l34_l2NetIfInfo_t   l2NetIfInfo;
    rtk_l34_pppInfo_t       pppInfo;
    rtk_l34_ipInfo_t        ipInfo;
    /*maxize size of ipv6 per netif is route size -1 since the last one is default route*/
    uint32              ip6size;
    rtk_l34_route6Table_t   ip6Info[L34_IPV6_ROUTE_MAX_SIZE-1];
}rtk_l34_netifInfo_t;


typedef struct rtk_l34_nat_info_s{
    rtk_ip_addr_t   natIp;
    uint16          natPort;
    uint32          priValid;
    uint32          priority;
}rtk_l34_nat_info_t;

/*
 * Function Declaration
 */



/* Module Name    : L34 Lite   */
/* Sub-module Name: Network Interface */

/* Function Name:
 *      rtk_l34_lite_init
 * Description:
 *      Initialize l34 lite module.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize l34 lite module before calling any l34 lite APIs.
 */
extern int32
rtk_l34_lite_init(void);


/* Function Name:
 *      rtk_l34_netif_create
 * Description:
 *      create a new interface
 * Input:
 *      netifId     - interface index
 *    netifType - WAN or LAN
 *      ifmac       - MAC address for this interface
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
rtk_l34_netif_create(uint32 netifId,rtk_l34_netifType_t netifType, rtk_mac_t ifmac);



/* Function Name:
 *      rtk_l34_netifPPPoE_set
 * Description:
 *      set pppoe relation parameters
 * Input:
 *      netifId     - interface index
 *    pppState  - enable or disable pppState
 *    sessionId - PPPoE session id
 *      passThroughState    - enable or disable PPPoE pass through function in this interface.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
rtk_l34_netifPPPoE_set(uint32 netifId,rtk_enable_t pppState,uint32 sessionId, rtk_enable_t passThroughState);



/* Function Name:
 *      rtk_l34_netifVlan_set
 * Description:
 *      set interface vlan configure
 * Input:
 *      netifId: interface id
 *    vid           - vlan id for interface
 *      defaultPri  - default priority for interface, value 8 means disable
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
rtk_l34_netifVlan_set(uint32 netifId, rtk_vlan_t vid, rtk_pri_t defaultPri);



/* Function Name:
 *      rtk_l34_netifRoutingState_set
 * Description:
 *      set interface routing state
 * Input:
 *      netifId     -interface id
 *    routingState  -disable or enable
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
rtk_l34_netifRoutingState_set(uint32 netifId, rtk_enable_t routingState);



/* Function Name:
 *      rtk_l34_netifMtu_set
 * Description:
 *      set interface mtu size
 * Input:
 *      netifId - interface id
 *    mtu   -interface mtu size
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *    None
 */
extern int32
rtk_l34_netifMtu_set(uint32 netifId, uint32 mtu);



/* Function Name:
 *      rtk_l34_netifIpaddr_set
 * Description:
 *      set interface ipaddress
 * Input:
 *      netifId -interface id
 *    ipaddr    -interface ip address
 *      ipmask  -ip mask length
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
rtk_l34_netifIpaddr_set(uint32 netifId, rtk_ip_addr_t ipaddr,uint32 ipmask);



/* Function Name:
 *      rtk_l34_netifNat_set
 * Description:
 *      set interface nat related configuration
 * Input:
 *      netifId     - interface id
 *    natType       -interface nat type for connections
 *      internalIp  - for nat mode, interanl ip address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
rtk_l34_netifNat_set(uint32 netifId, rtk_l34_natType_t natType ,rtk_ip_addr_t  internalp);



/* Function Name:
 *      rtk_l34_netifState_set
 * Description:
 *      set interface status
 * Input:
 *      netifId - interface id
 *    ifState   -interface status, up/down
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
rtk_l34_netifState_set(uint32 netifId,rtk_enable_t ifState);



/* Function Name:
 *      rtk_l34_netif_get
 * Description:
 *      get interface information
 * Input:
 *      netifId     - interface id
 *    *netifInfo    - networking interface infornation
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
rtk_l34_netif_get(uint32 netifId,rtk_l34_netifInfo_t *netifInfo);



/* Function Name:
 *      rtk_l34_netifGateway_set
 * Description:
 *      set net interface gateway configuration
 * Input:
 *      netifId   -interface id
 *    gatewayMac - MAC address for net interface gateway
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
rtk_l34_netifGateway_set(uint32 netifId, rtk_mac_t gatewayMac);



/* Function Name:
 *      rtk_l34_netif_set
 * Description:
 *      set all net interface configuration
 * Input:
 *      netifId   -interface id
 *    pNetifInfo - pointer of net interface information
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
rtk_l34_netif_set(uint32 netifId, rtk_l34_netifInfo_t *pNetifInfo);



/* Function Name:
 *      rtk_l34_netif_del
 * Description:
 *      delete interface
 * Input:
 *      netifId - interface id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
rtk_l34_netif_del(uint32 netifId);


/* Sub-module Name: ARP Table configuration */

/* Function Name:
 *      rtk_l34_arp_add
 * Description:
 *      add an arp entry
 * Input:
 *      ipaddr  - ip address
 *    mac   - mac address for ipaddr
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
rtk_l34_arp_add(rtk_ip_addr_t ipaddr, rtk_mac_t mac);



/* Function Name:
 *      rtk_l34_arp_get
 * Description:
 *      get mac address from ip address
 * Input:
 *      ipaddr  - ip address for get arp entry
 *    *arpInfo - arp related information
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
rtk_l34_arp_get(rtk_ip_addr_t ipaddr,rtk_l34_arpInfo_t *arpInfo);



/* Function Name:
 *      rtk_l34_arp_del
 * Description:
 *      delete an arp entry
 * Input:
 *      ipaddr  - ip address for delete arp entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
rtk_l34_arp_del(rtk_ip_addr_t ipaddr);


/* Sub-module Name: Routing Table configuration */


/* Function Name:
 *      rtk_l34_route_add
 * Description:
 *      add a routing table entry
 * Input:
 *    routeType - type of this routing table
 *      routeInfo   - routing table necessary information
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
rtk_l34_route_add(rtk_l34_routeType_t routeType,rtk_l34_routeTable_t routeTable);




/* Function Name:
 *      rtk_l34_route_get
 * Description:
 *      get a routing table entry
 * Input:
 *      routeIndex  - routing table index for get routing information
 *      *routeInfo  - point of routing information
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
rtk_l34_route_get(uint32 routeIndex,rtk_l34_routeInfo_t *routeInfo);



/* Function Name:
 *      rtk_l34_route_del
 * Description:
 *      delete a routing table entry
 * Input:
 *      ipaddr  - ip address for routing match
 *    ipmask    - ip mask length for routing match
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
rtk_l34_route_del(rtk_ip_addr_t ipaddr,uint32 ipmask);



/* Module Name    : L34 Lite   */
/* Sub-module Name: NAPT Connection */

/*
 * Function Declaration
 */
/* Function Name:
 *      rtk_l34_connectTrack_add
 * Description:
 *      add a connection tracking table
 * Input:
 *      dir: direction of connection
 *      tuple: five tuple for connection
 *      natInfo:  napt transfer information
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
int32
rtk_l34_connectTrack_add(rtk_l34_direct_t dir, rtk_l34_tuple_t tuple, rtk_l34_nat_info_t natInfo);



/* Function Name:
 *      rtk_l34_connectTrack_get
 * Description:
 *      get a connection information
 * Input:
 *    tuple         - five tuple for connection
 *    *connectInfo      - point of connection information
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
rtk_l34_connectTrack_get(rtk_l34_tuple_t tuple, rtk_l34_connectInfo_t *connectInfo);



/* Function Name:
 *      rtk_l34_connectTrack_del
 * Description:
 *      delete a connection
 * Input:
 *    tuple - five tuple for connection
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
rtk_l34_connectTrack_del(rtk_l34_tuple_t tuple);



/* Function Name:
 *      rtk_l34_globalCfg_get
 * Description:
 *      get l34 global configuration
 * Input:
 *    *globalCfg: point of global configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
rtk_l34_globalCfg_get(rtk_l34_global_cfg_t *globalCfg);



/* Sub-module Name: IPv6 Routing Table*/


/* Function Name:
 *      rtk_l34_route6_add
 * Description:
 *      add l34 IPv6 routing table
 * Input:
 *      route6Type  - IPv6 route type
 *    route6Table - IPv6 route value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
rtk_l34_route6_add(rtk_l34_ipv6RouteType_t route6Type ,rtk_l34_route6Table_t route6Table);


/* Function Name:
 *      rtk_l34_route6_del
 * Description:
 *      delete l34 IPv6 routing table
 * Input:
 *    ipaddr        -IPv6 address
 *    prefixLen -prefix length
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
rtk_l34_route6_del(rtk_ipv6_addr_t ip6addr, uint32 prefixLen);


/* Function Name:
 *      rtk_l34_route6_get
 * Description:
 *      Get l34 IPv6 routing table
 * Input:
 *    index      -index of ipv6 information
 * Output:
 *    *pRoute6Info - point of route 6 information
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
rtk_l34_route6_get(uint32 index,rtk_l34_route6Info_t *pRoute6Info);


/* Sub-module Name: IPv6 Neighbor Table*/


/* Function Name:
 *      rtk_l34_neigh6_add
 * Description:
 *      add l34 IPv6 neighbor entry
 * Input:
 *      ipaddr   - ipv6 address
 *    mac   - mac address for ipaddr
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
rtk_l34_neigh6_add(rtk_ipv6_addr_t ip6addr,rtk_mac_t mac);


/* Function Name:
 *      rtk_l34_neigh6_del
 * Description:
 *      delete l34 IPv6 routing table
 * Input:
 *    ipaddr        -IPv6 address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
rtk_l34_neigh6_del(rtk_ipv6_addr_t ip6addr);


/* Function Name:
 *      rtk_l34_neigh6_get
 * Description:
 *      Get l34 IPv6 routing table
 * Input:
 *    ipaddr            -ipv6 address for get
 * Output:
 *    *pNeigh6Info  -point of neighbor information
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
rtk_l34_neigh6_get(rtk_ipv6_addr_t ip6addr,rtk_l34_neigh6Info_t *pNeigh6Info);


/* Function Name:
 *      rtk_l34_netifIp6addr_add
 * Description:
 *      add interface ipv6 address
 * Input:
 *      netifId   -interface id
 *    ipaddr      -interface ip address
 *      prefixLen -ipv6 prefix length
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
rtk_l34_netifIp6addr_add(uint32 netifId, rtk_ipv6_addr_t ip6addr,uint32 prefixLen);

/* Function Name:
 *      rtk_l34_netifIp6addr_del
 * Description:
 *      delete interface ipv6 address
 * Input:
 *      netifId   -interface id
 *    ipaddr      -interface ip address
 *      prefixLen -ipv6 prefix length
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
rtk_l34_netifIp6addr_del(uint32 netifId, rtk_ipv6_addr_t ip6addr,uint32 prefixLen);



/* Function Name:
 *      rtk_l34_netifMac_set
 * Description:
 *      set netif MAC address
 * Input:
 *      netifId   -interface id
 *    hwAddr      -interface HW MAC address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
rtk_l34_netifMac_set(uint32 netifId, rtk_mac_t hwAddr);


#endif /* __RTK_L34_LIGHT_H__ */

