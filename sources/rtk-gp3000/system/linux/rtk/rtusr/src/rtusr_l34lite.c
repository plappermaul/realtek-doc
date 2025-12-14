/*
 * Copyright (C) 2011 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 */

#define __RTK_L34_LIGHTH__

/*
 * Include Files
 */
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <osal/lib.h>
#include <rtk/rtusr/include/rtusr_util.h>
#include <rtdrv/rtdrv_netfilter.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <rtk/vlan.h>
#ifdef CONFIG_XDSL_RG_DIAGSHELL
#include <rtk_rg_xdsl_struct.h>
#else
#include <rtk/l34lite.h>
#endif
/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */


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
int32
rtk_l34_lite_init(void)
{
    rtdrv_l34liteCfg_t l34lite_cfg;

    /* function body */
    osal_memset(&l34lite_cfg,0x0,sizeof(rtdrv_l34liteCfg_t));
    SETSOCKOPT(RTDRV_L34_LITE_INIT, &l34lite_cfg, rtdrv_l34liteCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_l34_lite_init */


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
int32
rtk_l34_netif_create(uint32 netifId,rtk_l34_netifType_t netifType, rtk_mac_t ifmac)
{
    rtdrv_l34liteCfg_t l34lite_cfg;

    /* function body */
    osal_memset(&l34lite_cfg,0x0,sizeof(rtdrv_l34liteCfg_t));
    osal_memcpy(&l34lite_cfg.netifId, &netifId, sizeof(uint32));
    osal_memcpy(&l34lite_cfg.netifType, &netifType, sizeof(rtk_l34_netifType_t));
    osal_memcpy(&l34lite_cfg.ifmac, &ifmac, sizeof(rtk_mac_t));
    SETSOCKOPT(RTDRV_L34_NETIF_CREATE, &l34lite_cfg, rtdrv_l34liteCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_l34_netif_create */



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
int32
rtk_l34_netifPPPoE_set(uint32 netifId,rtk_enable_t pppState,uint32 sessionId, rtk_enable_t passThroughState)
{
    rtdrv_l34liteCfg_t l34lite_cfg;

    /* function body */
    osal_memset(&l34lite_cfg,0x0,sizeof(rtdrv_l34liteCfg_t));
    osal_memcpy(&l34lite_cfg.netifId, &netifId, sizeof(uint32));
    osal_memcpy(&l34lite_cfg.pppState, &pppState, sizeof(rtk_enable_t));
    osal_memcpy(&l34lite_cfg.sessionId, &sessionId, sizeof(uint32));
    osal_memcpy(&l34lite_cfg.passThroughState, &passThroughState, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_L34_NETIFPPPOE_SET, &l34lite_cfg, rtdrv_l34liteCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_l34_netifPPPoE_set */



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
int32
rtk_l34_netifVlan_set(uint32 netifId, rtk_vlan_t vid, rtk_pri_t defaultPri)
{
    rtdrv_l34liteCfg_t l34lite_cfg;

    /* function body */
    osal_memset(&l34lite_cfg,0x0,sizeof(rtdrv_l34liteCfg_t));
    osal_memcpy(&l34lite_cfg.netifId, &netifId, sizeof(uint32));
    osal_memcpy(&l34lite_cfg.vid, &vid, sizeof(rtk_vlan_t));
    osal_memcpy(&l34lite_cfg.defaultPri, &defaultPri, sizeof(rtk_pri_t));
    SETSOCKOPT(RTDRV_L34_NETIFVLAN_SET, &l34lite_cfg, rtdrv_l34liteCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_l34_netifVlan_set */



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
int32
rtk_l34_netifRoutingState_set(uint32 netifId, rtk_enable_t routingState)
{
    rtdrv_l34liteCfg_t l34lite_cfg;

    /* function body */
    osal_memset(&l34lite_cfg,0x0,sizeof(rtdrv_l34liteCfg_t));
    osal_memcpy(&l34lite_cfg.netifId, &netifId, sizeof(uint32));
    osal_memcpy(&l34lite_cfg.routingState, &routingState, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_L34_NETIFROUTINGSTATE_SET, &l34lite_cfg, rtdrv_l34liteCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_l34_netifRoutingState_set */



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
int32
rtk_l34_netifMtu_set(uint32 netifId, uint32 mtu)
{
    rtdrv_l34liteCfg_t l34lite_cfg;

    /* function body */
    osal_memset(&l34lite_cfg,0x0,sizeof(rtdrv_l34liteCfg_t));
    osal_memcpy(&l34lite_cfg.netifId, &netifId, sizeof(uint32));
    osal_memcpy(&l34lite_cfg.mtu, &mtu, sizeof(uint32));
    SETSOCKOPT(RTDRV_L34_NETIFMTU_SET, &l34lite_cfg, rtdrv_l34liteCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_l34_netifMtu_set */



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
int32
rtk_l34_netifIpaddr_set(uint32 netifId, rtk_ip_addr_t ipaddr,uint32 ipmask)
{
    rtdrv_l34liteCfg_t l34lite_cfg;

    /* function body */
    osal_memset(&l34lite_cfg,0x0,sizeof(rtdrv_l34liteCfg_t));
    osal_memcpy(&l34lite_cfg.netifId, &netifId, sizeof(uint32));
    osal_memcpy(&l34lite_cfg.ipaddr, &ipaddr, sizeof(rtk_ip_addr_t));
    osal_memcpy(&l34lite_cfg.ipmask, &ipmask, sizeof(uint32));
    SETSOCKOPT(RTDRV_L34_NETIFIPADDR_SET, &l34lite_cfg, rtdrv_l34liteCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_l34_netifIpaddr_set */



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
int32
rtk_l34_netifNat_set(uint32 netifId, rtk_l34_natType_t natType ,rtk_ip_addr_t  internalp)
{
    rtdrv_l34liteCfg_t l34lite_cfg;

    /* function body */
    osal_memset(&l34lite_cfg,0x0,sizeof(rtdrv_l34liteCfg_t));
    osal_memcpy(&l34lite_cfg.netifId, &netifId, sizeof(uint32));
    osal_memcpy(&l34lite_cfg.natType, &natType, sizeof(rtk_l34_natType_t));
    osal_memcpy(&l34lite_cfg.internalp, &internalp, sizeof(rtk_ip_addr_t));
    SETSOCKOPT(RTDRV_L34_NETIFNAT_SET, &l34lite_cfg, rtdrv_l34liteCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_l34_netifNat_set */



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
int32
rtk_l34_netifState_set(uint32 netifId,rtk_enable_t ifState)
{
    rtdrv_l34liteCfg_t l34lite_cfg;

    /* function body */
    osal_memset(&l34lite_cfg,0x0,sizeof(rtdrv_l34liteCfg_t));
    osal_memcpy(&l34lite_cfg.netifId, &netifId, sizeof(uint32));
    osal_memcpy(&l34lite_cfg.ifState, &ifState, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_L34_NETIFSTATE_SET, &l34lite_cfg, rtdrv_l34liteCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_l34_netifState_set */



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
int32
rtk_l34_netif_get(uint32 netifId,rtk_l34_netifInfo_t *netifInfo)
{
    rtdrv_l34liteCfg_t l34lite_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == netifInfo), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l34lite_cfg.netifId, &netifId, sizeof(uint32));
    GETSOCKOPT(RTDRV_L34_NETIF_GET, &l34lite_cfg, rtdrv_l34liteCfg_t, 1);
    osal_memcpy(netifInfo, &l34lite_cfg.netifInfo, sizeof(rtk_l34_netifInfo_t));

    return RT_ERR_OK;
}   /* end of rtk_l34_netif_get */




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
int32
rtk_l34_netif_del(uint32 netifId)
{
    rtdrv_l34liteCfg_t l34lite_cfg;

    /* function body */
    osal_memset(&l34lite_cfg,0x0,sizeof(rtdrv_l34liteCfg_t));
    osal_memcpy(&l34lite_cfg.netifId, &netifId, sizeof(uint32));
    SETSOCKOPT(RTDRV_L34_NETIF_DEL, &l34lite_cfg, rtdrv_l34liteCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_l34_netif_del */


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
int32
rtk_l34_arp_add(rtk_ip_addr_t ipaddr, rtk_mac_t mac)
{
    rtdrv_l34liteCfg_t l34lite_cfg;

    /* function body */
    osal_memset(&l34lite_cfg,0x0,sizeof(rtdrv_l34liteCfg_t));
    osal_memcpy(&l34lite_cfg.ipaddr, &ipaddr, sizeof(rtk_ip_addr_t));
    osal_memcpy(&l34lite_cfg.mac, &mac, sizeof(rtk_mac_t));
    SETSOCKOPT(RTDRV_L34_ARP_ADD, &l34lite_cfg, rtdrv_l34liteCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_l34_arp_add */



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
int32
rtk_l34_arp_get(rtk_ip_addr_t ipaddr,rtk_l34_arpInfo_t *arpInfo)
{
    rtdrv_l34liteCfg_t l34lite_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == arpInfo), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l34lite_cfg.ipaddr, &ipaddr, sizeof(rtk_ip_addr_t));
    GETSOCKOPT(RTDRV_L34_ARP_GET, &l34lite_cfg, rtdrv_l34liteCfg_t, 1);
    osal_memcpy(arpInfo, &l34lite_cfg.arpInfo, sizeof(rtk_l34_arpInfo_t));

    return RT_ERR_OK;
}   /* end of rtk_l34_arp_get */



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
int32
rtk_l34_arp_del(rtk_ip_addr_t ipaddr)
{
    rtdrv_l34liteCfg_t l34lite_cfg;

    /* function body */
    osal_memset(&l34lite_cfg,0x0,sizeof(rtdrv_l34liteCfg_t));
    osal_memcpy(&l34lite_cfg.ipaddr, &ipaddr, sizeof(rtk_ip_addr_t));
    SETSOCKOPT(RTDRV_L34_ARP_DEL, &l34lite_cfg, rtdrv_l34liteCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_l34_arp_del */


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
int32
rtk_l34_route_add(rtk_l34_routeType_t routeType,rtk_l34_routeTable_t routeTable)
{
    rtdrv_l34liteCfg_t l34lite_cfg;

    /* function body */
    osal_memset(&l34lite_cfg,0x0,sizeof(rtdrv_l34liteCfg_t));
    osal_memcpy(&l34lite_cfg.routeType, &routeType, sizeof(rtk_l34_routeType_t));
    osal_memcpy(&l34lite_cfg.routeTable, &routeTable, sizeof(rtk_l34_routeTable_t));
    SETSOCKOPT(RTDRV_L34_ROUTE_ADD, &l34lite_cfg, rtdrv_l34liteCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_l34_route_add */




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
int32
rtk_l34_route_get(uint32 routeIndex,rtk_l34_routeInfo_t *routeInfo)
{
    rtdrv_l34liteCfg_t l34lite_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == routeInfo), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l34lite_cfg.routeIndex, &routeIndex, sizeof(uint32));
    GETSOCKOPT(RTDRV_L34_ROUTE_GET, &l34lite_cfg, rtdrv_l34liteCfg_t, 1);
    osal_memcpy(routeInfo, &l34lite_cfg.routeInfo, sizeof(rtk_l34_routeInfo_t));

    return RT_ERR_OK;
}   /* end of rtk_l34_route_get */



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
int32
rtk_l34_route_del(rtk_ip_addr_t ipaddr,uint32 ipmask)
{
    rtdrv_l34liteCfg_t l34lite_cfg;

    /* function body */
    osal_memset(&l34lite_cfg,0x0,sizeof(rtdrv_l34liteCfg_t));
    osal_memcpy(&l34lite_cfg.ipaddr, &ipaddr, sizeof(rtk_ip_addr_t));
    osal_memcpy(&l34lite_cfg.ipmask, &ipmask, sizeof(uint32));
    SETSOCKOPT(RTDRV_L34_ROUTE_DEL, &l34lite_cfg, rtdrv_l34liteCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_l34_route_del */



/* Module Name    : L34 Lite   */
/* Sub-module Name: NAPT Connection */

/* Function Name:
 *      rtk_l34_connectTrack_add
 * Description:
 *      add a connection tracking table
 * Input:
 *      dir     - direction of connection
 *    tuple - five tuple for connection
 *      natIp   - new source ip address
 *      natport - new source port
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
rtk_l34_connectTrack_add(rtk_l34_direct_t dir, rtk_l34_tuple_t tuple, rtk_l34_nat_info_t natInfo)
{
    rtdrv_l34liteCfg_t l34lite_cfg;

    /* function body */
    osal_memset(&l34lite_cfg,0x0,sizeof(rtdrv_l34liteCfg_t));
    osal_memcpy(&l34lite_cfg.dir, &dir, sizeof(rtk_l34_direct_t));
    osal_memcpy(&l34lite_cfg.tuple, &tuple, sizeof(rtk_l34_tuple_t));
    osal_memcpy(&l34lite_cfg.natInfo, &natInfo, sizeof(rtk_l34_nat_info_t));
    SETSOCKOPT(RTDRV_L34_CONNECTTRACK_ADD, &l34lite_cfg, rtdrv_l34liteCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_l34_connectTrack_add */



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
int32
rtk_l34_connectTrack_get(rtk_l34_tuple_t tuple, rtk_l34_connectInfo_t *connectInfo)
{
    rtdrv_l34liteCfg_t l34lite_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == connectInfo), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l34lite_cfg.tuple, &tuple, sizeof(rtk_l34_tuple_t));
    GETSOCKOPT(RTDRV_L34_CONNECTTRACK_GET, &l34lite_cfg, rtdrv_l34liteCfg_t, 1);
    osal_memcpy(connectInfo, &l34lite_cfg.connectInfo, sizeof(rtk_l34_connectInfo_t));

    return RT_ERR_OK;
}   /* end of rtk_l34_connectTrack_get */



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
int32
rtk_l34_connectTrack_del(rtk_l34_tuple_t tuple)
{
    rtdrv_l34liteCfg_t l34lite_cfg;

    /* function body */
    osal_memset(&l34lite_cfg,0x0,sizeof(rtdrv_l34liteCfg_t));
    osal_memcpy(&l34lite_cfg.tuple, &tuple, sizeof(rtk_l34_tuple_t));
    SETSOCKOPT(RTDRV_L34_CONNECTTRACK_DEL, &l34lite_cfg, rtdrv_l34liteCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_l34_connectTrack_del */



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
int32
rtk_l34_globalCfg_get(rtk_l34_global_cfg_t *globalCfg)
{
    rtdrv_l34liteCfg_t l34lite_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == globalCfg), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l34lite_cfg.globalCfg, globalCfg, sizeof(rtk_l34_global_cfg_t));
    GETSOCKOPT(RTDRV_L34_GLOBALCFG_GET, &l34lite_cfg, rtdrv_l34liteCfg_t, 1);
    osal_memcpy(globalCfg, &l34lite_cfg.globalCfg, sizeof(rtk_l34_global_cfg_t));

    return RT_ERR_OK;
}   /* end of rtk_l34_globalCfg_get */



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
int32
rtk_l34_route6_add(rtk_l34_ipv6RouteType_t route6Type ,rtk_l34_route6Table_t route6Table)
{
    rtdrv_l34liteCfg_t l34lite_cfg;

    /* function body */
    osal_memset(&l34lite_cfg,0x0,sizeof(rtdrv_l34liteCfg_t));
    osal_memcpy(&l34lite_cfg.route6Type, &route6Type, sizeof(rtk_l34_ipv6RouteType_t));
    osal_memcpy(&l34lite_cfg.route6Table, &route6Table, sizeof(rtk_l34_route6Table_t));
    SETSOCKOPT(RTDRV_L34_ROUTE6_ADD, &l34lite_cfg, rtdrv_l34liteCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_l34_route6_add */


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
int32
rtk_l34_route6_del(rtk_ipv6_addr_t ip6addr, uint32 prefixLen)
{
    rtdrv_l34liteCfg_t l34lite_cfg;

    /* function body */
    osal_memset(&l34lite_cfg,0x0,sizeof(rtdrv_l34liteCfg_t));
    osal_memcpy(&l34lite_cfg.ip6addr, &ip6addr, sizeof(rtk_ipv6_addr_t));
    osal_memcpy(&l34lite_cfg.prefixLen, &prefixLen, sizeof(uint32));
    SETSOCKOPT(RTDRV_L34_ROUTE6_DEL, &l34lite_cfg, rtdrv_l34liteCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_l34_route6_del */


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
int32
rtk_l34_route6_get(uint32 index,rtk_l34_route6Info_t *pRoute6Info)
{
    rtdrv_l34liteCfg_t l34lite_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pRoute6Info), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l34lite_cfg.index, &index, sizeof(uint32));
    GETSOCKOPT(RTDRV_L34_ROUTE6_GET, &l34lite_cfg, rtdrv_l34liteCfg_t, 1);
    osal_memcpy(pRoute6Info, &l34lite_cfg.route6Info, sizeof(rtk_l34_route6Info_t));

    return RT_ERR_OK;
}   /* end of rtk_l34_route6_get */


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
int32
rtk_l34_neigh6_add(rtk_ipv6_addr_t ip6addr,rtk_mac_t mac)
{
    rtdrv_l34liteCfg_t l34lite_cfg;

    /* function body */
    osal_memset(&l34lite_cfg,0x0,sizeof(rtdrv_l34liteCfg_t));
    osal_memcpy(&l34lite_cfg.ip6addr, &ip6addr, sizeof(rtk_ipv6_addr_t));
    osal_memcpy(&l34lite_cfg.mac, &mac, sizeof(rtk_mac_t));
    SETSOCKOPT(RTDRV_L34_NEIGH6_ADD, &l34lite_cfg, rtdrv_l34liteCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_l34_neigh6_add */


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
int32
rtk_l34_neigh6_del(rtk_ipv6_addr_t ip6addr)
{
    rtdrv_l34liteCfg_t l34lite_cfg;

    /* function body */
    osal_memset(&l34lite_cfg,0x0,sizeof(rtdrv_l34liteCfg_t));
    osal_memcpy(&l34lite_cfg.ip6addr, &ip6addr, sizeof(rtk_ipv6_addr_t));
    SETSOCKOPT(RTDRV_L34_NEIGH6_DEL, &l34lite_cfg, rtdrv_l34liteCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_l34_neigh6_del */


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
int32
rtk_l34_neigh6_get(rtk_ipv6_addr_t ip6addr,rtk_l34_neigh6Info_t *pNeigh6Info)
{
    rtdrv_l34liteCfg_t l34lite_cfg;

    /* parameter check */
    RT_PARAM_CHK((NULL == pNeigh6Info), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memcpy(&l34lite_cfg.ip6addr, &ip6addr, sizeof(rtk_ipv6_addr_t));
    GETSOCKOPT(RTDRV_L34_NEIGH6_GET, &l34lite_cfg, rtdrv_l34liteCfg_t, 1);
    osal_memcpy(pNeigh6Info, &l34lite_cfg.neigh6Info, sizeof(rtk_l34_neigh6Info_t));

    return RT_ERR_OK;
}   /* end of rtk_l34_neigh6_get */


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
int32
rtk_l34_netifIp6addr_add(uint32 netifId, rtk_ipv6_addr_t ip6addr,uint32 prefixLen)
{
    rtdrv_l34liteCfg_t l34lite_cfg;

    /* function body */
    osal_memset(&l34lite_cfg,0x0,sizeof(rtdrv_l34liteCfg_t));
    osal_memcpy(&l34lite_cfg.netifId, &netifId, sizeof(uint32));
    osal_memcpy(&l34lite_cfg.ip6addr, &ip6addr, sizeof(rtk_ipv6_addr_t));
    osal_memcpy(&l34lite_cfg.prefixLen, &prefixLen, sizeof(uint32));
    SETSOCKOPT(RTDRV_L34_NETIFIP6ADDR_ADD, &l34lite_cfg, rtdrv_l34liteCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_l34_netifIp6addr_add */

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
int32
rtk_l34_netifIp6addr_del(uint32 netifId, rtk_ipv6_addr_t ip6addr,uint32 prefixLen)
{
    rtdrv_l34liteCfg_t l34lite_cfg;

    /* function body */
    osal_memset(&l34lite_cfg,0x0,sizeof(rtdrv_l34liteCfg_t));
    osal_memcpy(&l34lite_cfg.netifId, &netifId, sizeof(uint32));
    osal_memcpy(&l34lite_cfg.ip6addr, &ip6addr, sizeof(rtk_ipv6_addr_t));
    osal_memcpy(&l34lite_cfg.prefixLen, &prefixLen, sizeof(uint32));
    SETSOCKOPT(RTDRV_L34_NETIFIP6ADDR_DEL, &l34lite_cfg, rtdrv_l34liteCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_l34_netifIp6addr_del */


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
int32
rtk_l34_netifGateway_set(uint32 netifId, rtk_mac_t gatewayMac)
{
    rtdrv_l34liteCfg_t l34lite_cfg;

    /* function body */
    osal_memset(&l34lite_cfg,0x0,sizeof(rtdrv_l34liteCfg_t));
    osal_memcpy(&l34lite_cfg.netifId, &netifId, sizeof(uint32));
    osal_memcpy(&l34lite_cfg.gatewayMac, &gatewayMac, sizeof(rtk_mac_t));
    SETSOCKOPT(RTDRV_L34_NETIFGATEWAY_SET, &l34lite_cfg, rtdrv_l34liteCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_l34_netifGateway_set */



/* Function Name:
 *      rtk_l34_netif_set
 * Description:
 *      set all net interface configuration
 * Input:
 *      netifId   -interface id
 *    netifInfo - net interface information
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
rtk_l34_netif_set(uint32 netifId, rtk_l34_netifInfo_t *pNetifInfo)
{
    rtdrv_l34liteCfg_t l34lite_cfg;

    /* function body */
    osal_memset(&l34lite_cfg,0x0,sizeof(rtdrv_l34liteCfg_t));
    osal_memcpy(&l34lite_cfg.netifId, &netifId, sizeof(uint32));
    osal_memcpy(&l34lite_cfg.netifInfo, pNetifInfo, sizeof(rtk_l34_netifInfo_t));
    SETSOCKOPT(RTDRV_L34_NETIF_SET, &l34lite_cfg, rtdrv_l34liteCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_l34_netif_set */


/* Function Name:
 *      rtk_l34_netifIp6addr_del
 * Description:
 *      delete interface ipv6 address
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
int32
rtk_l34_netifMac_set(uint32 netifId, rtk_mac_t hwAddr)
{
    rtdrv_l34liteCfg_t l34_cfg;

    /* function body */
    osal_memset(&l34_cfg,0x0,sizeof(rtdrv_l34liteCfg_t));
    osal_memcpy(&l34_cfg.netifId, &netifId, sizeof(uint32));
    osal_memcpy(&l34_cfg.hwAddr, &hwAddr, sizeof(rtk_mac_t));
    SETSOCKOPT(RTDRV_L34_NETIFMAC_SET, &l34_cfg, rtdrv_l34liteCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_l34_netifMac_set */


