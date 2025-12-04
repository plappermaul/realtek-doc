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

#define __RTK_l34_LITE__

/*
 * Include Files
 */
#include <common/rt_error.h>
#include <common/rt_type.h>
#include <common/util/rt_util.h>
#include <common/debug/rt_log.h>
#include <hal/common/halctrl.h>
#include <dal/dal_mgmt.h>
#include <rtk/init.h>
#include <rtk/vlan.h>
#include <rtk/l34.h>
#include <rtk/l2.h>
#include <rtk/l34lite.h>

/*
 * Symbol Definition
 */
static uint32 l34_us_connection_get(rtk_l34_tuple_t tuple,rtk_l34_connectInfo_t *connectInfo);
static uint32 l34_ds_connection_get(rtk_l34_tuple_t tuple,rtk_l34_connectInfo_t *connectInfo);


/*
 * Data Declaration
 */


 #define INVALID_NEXT_HOP 8

/*
 * Data Declaration
 */
static uint32   l34_lite_init = {INIT_NOT_COMPLETED};

static rtk_l34_natType_t netif_nat[L34_IF_NUM_MAX] = {0};

#ifdef CONFIG_SDK_RTL9602C
static uint32 arpEntry_valid[128/32] = {0};
#endif

static uint32
l34_hashOut(uint32 proto,rtk_ip_addr_t srcIp, uint16 srcport, rtk_ip_addr_t dstIp, uint16 dstport ){

    uint32  hashId;
#ifdef CONFIG_SDK_APOLLOMP
#if 1
    hashId = (((dstIp&0x3)<<16) | srcport) + ((dstIp>>2)&0x3ffff);
    hashId = (hashId&0x3ffff) + (hashId>>18);
    hashId += (((srcIp&0x3f)<<12) | (dstIp>>20));
    hashId = (hashId&0x3ffff) + (hashId>>18);
    hashId += ((srcIp>>6)&0x3ffff);
    hashId = (hashId&0x3ffff) + (hashId>>18);
    hashId += (((dstport&0x3ff)<<8) | (srcIp>>24));
    hashId = (hashId&0x3ffff) + (hashId>>18);
    hashId = (hashId&0x1ff) + (hashId>>9);
    hashId = hashId     + ((proto<<8) | ((dstport>>10)<<2));
    hashId = ((hashId&0x1ff) + (hashId>>9))&0x1ff;

#else
    uint16 X;
    X = srcIp & 0xffff;
    X ^= (srcIp >> 16) & 0xffff;
    X ^= srcport;
    X ^= dstIp & 0xffff;
    X ^= (dstIp >> 16) & 0xffff;
    X ^= dstport;

    hashId = X & 0x1ff;
    hashId ^= (X >> 9)  & 0x1ff;
    hashId ^= proto << 8;
    hashId = hashId & 0x1ff;
#endif

#else
/*CONFIG_SDK_RTL9602C*/
    uint32 X,Y,Z;
    X = (srcIp&0xffff) + (dstIp&0xffff) + (srcport&0xffff) + (dstport&0xffff);
    Y = (X&0x3ff) + ((X>>10)&0xff);
    Z = ((srcIp>>16)&0x3ff) ^ (((proto&0x1)<<9) + ((srcIp>>26)&0x3f)) ^ ((dstIp>>16)&0x3ff) ^ (((dstIp>>26)&0x3f)<<4);
    hashId = Y ^ Z;
#endif

    return hashId;
}


static uint32
l34_hashIn(uint32 proto,rtk_ip_addr_t dstIp,uint16 dstport){

    uint32 hashId;
#ifdef CONFIG_SDK_APOLLOMP
#if 1

        hashId = ((dstIp&0xff) + ((dstIp>>8)&0xff) + ((dstIp>>16)&0xff) + ((dstIp>>24)&0xff) + (dstport&0xff));
         hashId = ((hashId&0xff) + ((hashId>>8)&0xff)) & 0xff;
        hashId = (hashId&0xff) ^ ((dstport>>8)&0xff) ^ (proto << 7);

#else
    uint8 X;
    X = dstIp & 0xff;
    X ^= ((dstIp >> 8)  & 0xff);
    X ^= ((dstIp >> 16) & 0xff);
    X ^= ((dstIp >> 24) & 0xff);
    X ^= (dstport & 0xff);
    X ^= ((dstport >> 8) & 0xff);
    X ^= (proto << 7);
    hashId = (X & 0xff);
#endif

#else
/*CONFIG_SDK_RTL9602C*/
    uint32 X,Y,Z;
    X = dstport + (dstIp&0xffff);
    Y = (X&0x3ff) + ((X>>10)&0x7f);
    Z = ((dstIp>>16)&0x3ff) ^ (((proto&0x1)<<9) + ((dstIp>>26)&0x3f));
    hashId = (Y&0x3ff) ^ (Z&0x3ff);
#endif
    return hashId;
}

static uint32
l34_hashIpPort(rtk_ip_addr_t ipaddr,uint16 port){

    uint32 hashId;
    uint16 X;
    X = ipaddr & 0xffff;
    X ^= (ipaddr >> 16 ) & 0xffff;
    X ^= port & 0xffff;
    hashId = X & 0xffff;
    return hashId;
}


static uint32
l34_hashIp(rtk_ip_addr_t ipaddr){

    uint32 hashId;
    uint16 X;
    X = ipaddr & 0xffff;
    X ^= (ipaddr >> 16 ) & 0xffff;
    hashId = X & 0xffff;
    return hashId;
}


static int32
l34_remoteHash(rtk_l34_natType_t type,rtk_ip_addr_t ip,uint16 port,uint16*hash,uint8 *valid){

    switch(type){
    case L34_NAPT_FULL_CONE:
        *valid =     L34_NAPTR_ENTRY_NO_HASH;
        *hash = 0;
    break;
    case L34_NAPT_PORT_RESTRICTED:
        *valid = L34_NAPTR_ENTRY_IP_PORT_HASH;
        *hash = l34_hashIpPort(ip,port);
    break;
    case L34_NAPT_RESTRICTED_CONE:
        *valid = L34_NAPTR_ENTRY_IP_HASH;
        *hash = l34_hashIp(ip);
    break;
    default:
        *valid = L34_NAPTR_ENTRY_INVALID;
        *hash = 0;
    break;
    }
    return RT_ERR_OK;
}


static int32
l34_arp_range_assign(uint32 isWan, uint32 *arpStart,uint32 *arpEnd)
{
    uint32 arpSize;
    arpSize     = (HAL_L34_ARP_ENTRY_MAX()/2);

    *arpStart   = (isWan*arpSize) >> 2;
    *arpEnd     = ((isWan*arpSize) + arpSize -1) >> 2;
    return RT_ERR_OK;
}


static int32
l34_get_mac_by_l2entryId(uint32 l2Id,rtk_mac_t *mac)
{
    int32 ret;
    rtk_l2_ucastAddr_t l2Addr;

    if(!mac)
    {
        return RT_ERR_NULL_POINTER;
    }
    osal_memset(&l2Addr,0,sizeof(rtk_l2_ucastAddr_t));
    if((ret = rtk_l2_nextValidAddr_get(&l2Id, &l2Addr))!=RT_ERR_OK){
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }

    *mac = l2Addr.mac;
    return RT_ERR_OK;
}

static int32
l34_get_l2entry_by_mac(const uint32 netifId,rtk_mac_t mac,rtk_l2_ucastAddr_t *pl2Addr)
{
    int32               ret=0;
    rtk_fidMode_t           mode;
    rtk_fid_t               fid;
    rtk_l2_ucastAddr_t  l2Addr;
    rtk_l34_netif_entry_t   netif;
    rtk_vlan_t          vid;

    if(!pl2Addr)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_NULL_POINTER;
    }

    if((ret = rtk_l34_netifTable_get(netifId,&netif))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    vid = netif.vlan_id;

    /* get vlan configure*/
    if((ret=rtk_vlan_fidMode_get(vid,&mode))!= RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }

    if((ret=rtk_vlan_fid_get(vid, &fid))!= RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }

    /*clear ucastAddr for get right l2 entry*/
    osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
    l2Addr.mac = mac;
    if(mode==VLAN_FID_SVL){
        l2Addr.fid = fid;
        l2Addr.efid = 0;
    }else{
        l2Addr.vid = vid;
        l2Addr.efid = 0;
        l2Addr.flags |= RTK_L2_UCAST_FLAG_IVL;
    }

    /*search l2 table by mac*/
    if((ret=rtk_l2_addr_get(&l2Addr)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    osal_memcpy(pl2Addr,&l2Addr,sizeof(rtk_l2_ucastAddr_t));
    return RT_ERR_OK;
}


static int32
l34_get_route_by_ip(rtk_ip_addr_t ipaddr,rtk_l34_routing_entry_t *route,uint32 *map)
{
    int32 ret;
    uint32 i;
    uint32 tmpVal;
    uint32 length;

    for(i=0;i< HAL_L34_ROUTING_ENTRY_MAX();i++)
    {
        osal_memset(route,0,sizeof(rtk_l34_routing_entry_t));
        if((ret = rtk_l34_routingTable_get(i,route))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
        }
        if(1 == route->valid)
        {
            length = (31- (route->ipMask));
            tmpVal = (route->ipAddr) >> length;
            if(tmpVal == (ipaddr >> length))
            {
                *map = (ipaddr >> length) << length;
                return RT_ERR_OK;
            }
        }
    }

    return RT_ERR_FAILED;
}


static int32
l34_get_routeId_by_ip(rtk_ip_addr_t ipaddr,uint32 ipmask,uint32 *routeId,rtk_l34_routing_entry_t *route)
{
    int32 ret;
    uint32 i;
    for(i=0;i< HAL_L34_ROUTING_ENTRY_MAX();i++)
    {
        osal_memset(route,0,sizeof(rtk_l34_routing_entry_t));
        if((ret = rtk_l34_routingTable_get(i,route))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
        }
        if(route->valid && route->ipAddr==ipaddr && ipmask == route->ipMask)
        {
            *routeId = i;
            return RT_ERR_OK;
        }
    }
    *routeId = 0;
    return RT_ERR_FAILED;
}


static int32
l34_get_netifId_by_ip(rtk_ip_addr_t extIp,uint32 *netifId)
{
    int32 ret;
    uint32 i;
    rtk_l34_ext_intip_entry_t iptable;

    for(i=0;i<L34_IF_NUM_MAX;i++)
    {
        osal_memset(&iptable,0,sizeof(rtk_l34_ext_intip_entry_t));
        if((ret = rtk_l34_extIntIPTable_get(i,&iptable))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
        }
        if(iptable.extIpAddr == extIp)
        {
            *netifId = i;
            return RT_ERR_OK;
        }
    }
    return RT_ERR_ENTRY_NOTFOUND;
}


static int32
l34_get_arpInfo_by_ip(rtk_ip_addr_t ipaddr,uint32 *netifId,uint32 *arpId)
{
    int32   ret;
    uint32  map,index;

    rtk_l34_routing_entry_t     route;

    /* get route by ipaddr*/
    if((ret = l34_get_route_by_ip(ipaddr,&route,&map))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
    }
    if(L34_PROCESS_ARP != route.process)
    {
        return RT_ERR_INPUT;
    }

    /*assign arp index*/
#ifdef CONFIG_SDK_APOLLOMP
{
        uint32 offset;
        offset = ipaddr & ~(map);
        index = (route.arpStart << 2) + offset;
        if(index > (((route.arpEnd+1) << 2)-1))
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return RT_ERR_ENTRY_FULL;
        }
        *netifId = route.netifIdx;
        *arpId   = index;
}
#else
/*CONFIG_SDK_RTL9602C*/
{
        uint32 i;
        rtk_l34_arp_entry_t arpEntry;

        index = 65535;
        /*find free arp entry or same ip entry*/
        for(i = 0; i < HAL_L34_ARP_ENTRY_MAX(); i++)
        {
            if((ret = rtk_l34_arpTable_get(i,&arpEntry))!=RT_ERR_OK)
            {
                RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
                    return ret;
            }
            if(arpEntry.ipAddr == ipaddr)
            {
                index = i;
                break;
            }
            else if(arpEntry.valid != 1 && index == 65535)
            {
                index = i;
            }
        }
        *netifId = route.netifIdx;
        *arpId   = index;
}
#endif

    return RT_ERR_OK;
}

static int32
l34_get_dir_by_ip(rtk_ip_addr_t srcIp,rtk_ip_addr_t distIp,rtk_l34_direct_t *dir)
{
    int32   ret;
    uint32  map;
    rtk_l34_routing_entry_t route;

    ret = l34_get_route_by_ip(srcIp,&route,&map);

    if(ret == RT_ERR_OK)
    {

        *dir = route.internal ? L34_DIR_UPSTREAM : L34_DIR_DOWNSTREAM;

    }else
    {
        ret = l34_get_route_by_ip(distIp,&route,&map);
        if(ret != RT_ERR_OK)
        {
            return RT_ERR_ENTRY_NOTFOUND;
        }
        *dir = route.internal ? L34_DIR_UPSTREAM : L34_DIR_DOWNSTREAM;
    }
    return RT_ERR_OK;
}

/*RAW and configure transfer*/
static int32
l34_netifType_cfg2raw(rtk_l34_netifType_t cfg, uint32 *raw)
{

    switch(cfg){
    case L34_NETIF_TYPE_LAN:
        *raw = 1;
    break;
    case L34_NETIF_TYPE_WAN:
        *raw = 0;
    break;
    default:
        return RT_ERR_INPUT;
    break;
    }
    return RT_ERR_OK;
}

static int32
l34_netifType_raw2cfg(uint32 raw,rtk_l34_netifType_t *cfg)
{

    switch(raw){
    case 0:
        *cfg = L34_NETIF_TYPE_WAN;
    break;
    case 1:
        *cfg = L34_NETIF_TYPE_LAN;
    break;
    default:
        return RT_ERR_INPUT;
    break;
    }
    return RT_ERR_OK;
}


static int32
l34_natType_cfg2raw(rtk_l34_natType_t type,uint32 *raw)
{

    switch(type){
    case L34_NAT:
        *raw = L34_EXTIP_TYPE_NAT;
    break;
    case L34_NAPT_FULL_CONE:
    case L34_NAPT_RESTRICTED_CONE:
    case L34_NAPT_PORT_RESTRICTED:
        *raw = L34_EXTIP_TYPE_NAPT;
    break;
    case L34_LOCAL_PUBLIC:
        *raw = L34_EXTIP_TYPE_LP;
    break;
    default:
        return  RT_ERR_INPUT;
    break;
    }
    return RT_ERR_OK;
}

#if 0
static int32
l34_natType_raw2cfg(uint32 raw,rtk_l34_natType_t *type)
{

    switch(raw){
    case L34_EXTIP_TYPE_NAT:
        *type = L34_NAT;
    break;
    case L34_EXTIP_TYPE_NAPT:
        *type = *type;
    break;
    case L34_EXTIP_TYPE_LP:
        *type = L34_LOCAL_PUBLIC;
    break;
    default:
        return RT_ERR_INPUT;
    break;
    }
    return RT_ERR_OK;
}
#endif

static int32
l34_naptType_raw2cfg(uint32 raw,rtk_l34_natType_t *type)
{

    switch(raw){
    case 1:
        *type = L34_NAPT_FULL_CONE;
    break;
    case 2:
        *type = L34_NAPT_PORT_RESTRICTED;
    break;
    case 3:
        *type = L34_NAPT_RESTRICTED_CONE;
    break;
    default:
        return RT_ERR_INPUT;
    break;
    }
    return RT_ERR_OK;
}


static int32
l34_routeType_raw2cfg(uint32 raw, uint32 routeId, rtk_l34_routeType_t *cfg)
{
    switch(raw){
    case L34_PROCESS_ARP:
        *cfg = L34_ROUTE_LOCAL;
    break;
    case L34_PROCESS_DROP:
        *cfg = L34_ROUTE_DROP;
    break;
    case L34_PROCESS_CPU:
        *cfg = L34_ROUTE_CPU;
    break;
    case L34_PROCESS_NH:
        if(routeId== L34_DEFAULT_ROUTE)
        {
            *cfg = L34_ROUTE_DEFAULT;
        }else
        if(routeId== L34_STATIC_ROUTE)
        {
            *cfg = L34_ROUTE_STATIC;
        }else
        {
            *cfg = L34_ROUTE_POLICY;
        }
    break;
    default:
        return RT_ERR_FAILED;
    break;
    }
    return RT_ERR_OK;
}

static int32
is_empty_mac_check(rtk_mac_t mac){
    int i;

    for(i=0;i<ETHER_ADDR_LEN;i++){
        if(mac.octet[i]!=0){
            return 0;
        }
    }
    return 1;
}

static int32
l34_default_globalRoute_set(uint32 nhId,rtk_l34_routing_entry_t *route)
{
    route->process  = L34_PROCESS_NH;
    route->ipDomain  = L34_DEFAULT_IPDOMAIN;
    route->nhAlgo      = L34_DEFAULT_NHALGO;
    route->nhNum    = L34_DEFAULT_NHNUM;
    route->nhNxt        = nhId;
    route->nhStart  = nhId;
    return RT_ERR_OK;
}

static int32
l34_default_localRoute_set(uint32 netifId,rtk_l34_netifType_t netifType,rtk_l34_routing_entry_t *route)
{
    uint32 tmp;
    uint32 isWan = (netifType==L34_NETIF_TYPE_WAN) ?  1 : 0;

    l34_arp_range_assign(isWan,&route->arpStart,&route->arpEnd);
    route->netifIdx     = netifId;
    route->process      = L34_PROCESS_ARP;
    route->ipAddr       = 0;
    route->ipMask   = 0;
    route->rt2waninf    =  isWan;
    l34_netifType_cfg2raw(netifType,&tmp);
    route->internal     = (uint8)tmp;
    return RT_ERR_OK;
}

static int32
l34_localRoute_reassign(uint32 netifId,rtk_l34_netifType_t netifType,rtk_l34_routing_entry_t *route)
{
    uint32 tmp;
    uint32 isWan = (netifType==L34_NETIF_TYPE_WAN) ?  1 : 0;

    l34_arp_range_assign(isWan,&route->arpStart,&route->arpEnd);
    route->netifIdx     = netifId;
    route->process      = L34_PROCESS_ARP;
    route->rt2waninf    =  isWan;
    l34_netifType_cfg2raw(netifType,&tmp);
    route->internal     = (uint8)tmp;
    return RT_ERR_OK;
}


static int32
l34_default_netif_set(rtk_mac_t mac,rtk_l34_netif_entry_t *netif)
{
    netif->enable_rounting  = ENABLED;
    netif->gateway_mac      = mac;
    netif->mtu              = L34_DEFAULT_MTU;
    netif->valid            = ENABLED;
    netif->vlan_id          = L34_DEFAULT_VLAN;
    netif->mac_mask         = L34_DEFAULT_MACMASK;
    return RT_ERR_OK;
}


static int32
l34_default_nextHop_set(uint32 netifId,rtk_l34_nexthop_entry_t *nextHop)
{
    nextHop->ifIdx      = netifId;
    nextHop->keepPppoe  = DISABLED;
    nextHop->nhIdx      = 0;
    nextHop->pppoeIdx   = netifId;
    nextHop->type       = L34_NH_ETHER;
    return RT_ERR_OK;
}

static int32
l34_default_iptable_set(uint32 netifId,rtk_l34_ext_intip_entry_t *iptable)
{
    iptable->extIpAddr  = 0;
    iptable->intIpAddr  = 0;
    iptable->nhIdx      = netifId;
    iptable->pri            = 0;
    iptable->prival     = DISABLED;
    iptable->type           = L34_EXTIP_TYPE_NAPT;
    iptable->valid      = DISABLED;
    /*software nat type*/
    netif_nat[netifId]      = L34_NAPT_PORT_RESTRICTED;
    return RT_ERR_OK;
}

static int32
l34_netif_type_get(uint32 netifId,rtk_l34_netifType_t *netifType)
{
    int32 ret =0;
    rtk_l34_routing_entry_t route;

    if((ret = rtk_l34_routingTable_get(netifId,&route))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }

    *netifType = route.internal ? L34_NETIF_TYPE_LAN : L34_NETIF_TYPE_WAN;
    return RT_ERR_OK;
}

static int32
l34_nextHopPPP_set(uint32 netifId,rtk_enable_t pppState,rtk_enable_t passThroughState)
{
    int32   ret = 0;
    rtk_l34_nexthop_entry_t nextHop;

    if((ret = rtk_l34_nexthopTable_get(netifId,&nextHop))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    if(pppState==ENABLED)
    {
        nextHop.keepPppoe = passThroughState;
        nextHop.type         = L34_NH_PPPOE;

    }else
    {
        nextHop.keepPppoe   = DISABLED;
        nextHop.type           = L34_NH_ETHER;
    }

    if((ret = rtk_l34_nexthopTable_set(netifId,&nextHop))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
    }
    return RT_ERR_OK;
}


static int32
l34_nextHopMac_set(uint32 netifId,rtk_mac_t mac)
{
    int32   ret = 0;
    rtk_l34_nexthop_entry_t nextHop;
    rtk_l2_ucastAddr_t   l2Addr;


    if((ret = rtk_l34_nexthopTable_get(netifId,&nextHop))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }

    /*search l2table*/
    if((ret = l34_get_l2entry_by_mac(netifId,mac,&l2Addr))!=RT_ERR_OK){
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    nextHop.nhIdx = l2Addr.index;

    if((ret = rtk_l34_nexthopTable_set(netifId,&nextHop))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
    }
    /*update l2 table*/
    l2Addr.flags|=RTK_L2_UCAST_FLAG_ARP_USED;

    if((ret=rtk_l2_addr_add(&l2Addr)) != RT_ERR_OK){
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }

    return RT_ERR_OK;
}


static int32
l34_local_route_set(uint32 routeId,rtk_l34_routing_type_t type,rtk_ip_addr_t ipaddr,uint32 ipmask)
{
    int32 ret;
    uint32 netifId = routeId;
    rtk_l34_routing_entry_t route;
    uint32 isWan;

    osal_memset(&route,0,sizeof(rtk_l34_routing_entry_t));
    if((ret = rtk_l34_routingTable_get(routeId,&route))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }

    route.process = type;
    route.ipAddr  = ipaddr;
    route.ipMask  = ipmask;
    route.valid   = ENABLED;


    if(type == L34_PROCESS_ARP)
    {

        route.netifIdx = netifId;
        isWan = route.rt2waninf;
        l34_arp_range_assign(isWan,&route.arpStart,&route.arpEnd);
    }

    if((ret = rtk_l34_routingTable_set(routeId,&route))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }

    return RT_ERR_OK;
}

static int32
l34_global_route_set(uint32 routeId,uint32 netifId,rtk_ip_addr_t ipaddr, uint32 ipmask,rtk_mac_t gatewayMac)
{
    int32 ret = 0;
    rtk_l34_routing_entry_t     route;
    rtk_l34_nexthop_entry_t nextHop;
    rtk_l2_ucastAddr_t   l2Addr;

    /*get original wan routing information*/
    osal_memset(&route,0,sizeof(rtk_l34_routing_entry_t));
    if((ret = rtk_l34_routingTable_get(netifId,&route))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    /*assign global route setting*/
    l34_default_globalRoute_set(routeId,&route);
    route.ipAddr = ipaddr;
    route.ipMask = ipmask;
    route.valid  = ENABLED;
    /*set to routing table*/
    if((ret = rtk_l34_routingTable_set(routeId,&route))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }

    /*get original nexthop information*/
    if((ret = rtk_l34_nexthopTable_get(netifId,&nextHop))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }

    /*search l2table*/
    if((ret = l34_get_l2entry_by_mac(netifId,gatewayMac,&l2Addr))!=RT_ERR_OK){
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    nextHop.nhIdx = l2Addr.index;
    nextHop.ifIdx  = netifId;

    /*set to new nexthop*/
    if((ret = rtk_l34_nexthopTable_set(routeId,&nextHop))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }

    /*update original nexthop table*/
    if((ret = rtk_l34_nexthopTable_set(netifId,&nextHop))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }

    /*update l2 table*/
    l2Addr.flags|=RTK_L2_UCAST_FLAG_ARP_USED;

    if((ret=rtk_l2_addr_add(&l2Addr)) != RT_ERR_OK){
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }

    return RT_ERR_OK;
}


static int32
l34_empty_napt_find(uint32 index,uint32 *emptyId){

    int32 ret =0;
    uint32 cnt=0;
    rtk_l34_naptOutbound_entry_t napt;
    do{
        if((ret=rtk_l34_naptOutboundTable_get(index+cnt, &napt))!=RT_ERR_OK){
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
        }

        if(napt.valid == DISABLED){
            *emptyId = index+cnt;
            return RT_ERR_OK;
        }
        cnt++;
    }while(cnt<=3);

    return RT_ERR_ENTRY_FULL;
}


static int32
l34_empty_naptr_find(uint32 index,rtk_l34_naptInbound_entry_t cNaptr,uint32 *emptyId){

    int32 ret = 0;
    uint32 cnt=0;
    rtk_l34_naptInbound_entry_t naptr;
    do{
        osal_memset(&naptr,0,sizeof(rtk_l34_naptInbound_entry_t));
        if((ret=rtk_l34_naptInboundTable_get(index+cnt, &naptr))!=RT_ERR_OK){
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
        }
        if(!osal_memcmp(&naptr,&cNaptr,sizeof(rtk_l34_naptInbound_entry_t)))
        {
            *emptyId = index+cnt;
            return RT_ERR_OK;
        }

        if(naptr.valid == DISABLED){
            *emptyId = index+cnt;
            return RT_ERR_OK;
        }
        cnt++;
    }while(cnt<=3);

    return RT_ERR_ENTRY_FULL;
}

static int32
l34_naptr_find(uint32 index, rtk_ip_addr_t srcip,uint32 srcport,uint32 *hitId,rtk_l34_naptInbound_entry_t* hitEntry){

    int32 ret =0 ;
    uint32 cnt=0;
    rtk_l34_naptInbound_entry_t naptr;
    uint16 hash;
    uint8  valid;
    do{
        if((ret=rtk_l34_naptInboundTable_get(index+cnt, &naptr))!=RT_ERR_OK){
            cnt++;
            continue;
        }
        if(naptr.valid==DISABLED){
            cnt++;
            continue;
        }

        l34_remoteHash(netif_nat[naptr.extIpIdx], srcip, srcport, &hash,&valid);
        if(naptr.remHash!=hash){
            cnt++;
            continue;
        }
        *hitId = index+cnt;
        osal_memcpy(hitEntry,&naptr,sizeof(rtk_l34_naptInbound_entry_t));
        return RT_ERR_OK;

    }while(cnt <=3);

    return RT_ERR_ENTRY_NOTFOUND;
}


static int32
l34_napt_find(uint32 index,uint32 naptrId,uint32 *hitId,rtk_l34_naptOutbound_entry_t *hitEntry ){

    uint32 cnt=0;
    int32 ret = 0;
    rtk_l34_naptOutbound_entry_t napt;
    do{
        if((ret=rtk_l34_naptOutboundTable_get(index+cnt, &napt))!=RT_ERR_OK){
            cnt ++;
            continue;
        }
        if(napt.valid==DISABLED){
            cnt ++;
            continue;
        }
        if(napt.hashIdx != naptrId){
            cnt ++;
            continue;
        }
        *hitId = index+cnt;
        osal_memcpy(hitEntry,&napt,sizeof(rtk_l34_naptOutbound_entry_t));

        return RT_ERR_OK;
    }while( cnt <=3);

    return RT_ERR_ENTRY_NOTFOUND;
}


static int32
l34_us_connection_add(uint32 netifId, rtk_l34_tuple_t tuple, rtk_l34_nat_info_t natInfo)
{

    int32 ret = 0;
    rtk_l34_naptOutbound_entry_t    napt;
    rtk_l34_naptInbound_entry_t     naptr;
    rtk_l34_ext_intip_entry_t       iptable;
    rtk_l34_connectInfo_t           connInfo;

    uint32 idx,hashId;
    rtk_enable_t isTcp;

    if((ret = rtk_l34_extIntIPTable_get(netifId,&iptable))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }


    osal_memset(&naptr,0,sizeof(rtk_l34_naptInbound_entry_t));
    osal_memset(&napt,0,sizeof(rtk_l34_naptOutbound_entry_t));


    isTcp = (tuple.proto == L34_PROTO_TCP) ? ENABLED : DISABLED;
    idx = l34_hashOut(isTcp,tuple.srcIp,tuple.srcport,tuple.dstIp,tuple.dstport);
    hashId = l34_hashIn(isTcp, natInfo.natIp, natInfo.natPort);
    /*start to assign naptr table*/
    naptr.extIpIdx      = netifId;
    naptr.extPortLSB    = natInfo.natPort & 0xff;
#ifdef CONFIG_SDK_RTL9602C
    naptr.extPortHSB    = (natInfo.natPort >> 8) & 0xff;
#endif
    naptr.isTcp = isTcp;
    naptr.intIp = tuple.srcIp;
    naptr.intPort = tuple.srcport;
    if(natInfo.priValid)
    {
        naptr.priId = natInfo.priority;
        naptr.priValid = natInfo.priValid;
    }
    else
    {
        naptr.priId = iptable.pri;
        naptr.priValid = iptable.prival;
    }

    l34_remoteHash(netif_nat[netifId], tuple.dstIp,tuple.dstport,&naptr.remHash,&naptr.valid);

    if((ret=l34_empty_naptr_find(hashId << 2,naptr,&hashId))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }


    if((ret=l34_empty_napt_find(idx << 2,&idx))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }

    /*check connection is exist or not*/
    if((ret = l34_us_connection_get(tuple,&connInfo)) ==RT_ERR_OK)
    {
        hashId  = connInfo.naptrId;
        idx     = connInfo.naptId;
    }
    napt.valid = ENABLED;
    napt.hashIdx = hashId;
    if(natInfo.priValid)
    {
        napt.priValid = natInfo.priValid;
        napt.priValue = natInfo.priority;
    }
    else
    {
        napt.priValid = iptable.prival;
        napt.priValue = iptable.pri;
    }

    if((ret=rtk_l34_naptInboundTable_set(ENABLED, hashId, &naptr))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "add upstream connection to naptr id %d\n",hashId);


    if((ret=rtk_l34_naptOutboundTable_set(ENABLED, idx, &napt))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "add upstream connection to napt id %d\n",idx);
    return RT_ERR_OK;
}


static int32
l34_ds_connection_add(uint32 netifId, rtk_l34_tuple_t tuple, rtk_l34_nat_info_t natInfo)
{

    int32 ret = 0;
    rtk_l34_naptOutbound_entry_t    napt;
    rtk_l34_naptInbound_entry_t     naptr;
    rtk_l34_ext_intip_entry_t       iptable;
    uint32 idx,hashId;
    rtk_enable_t                    isTcp;
    rtk_l34_connectInfo_t           connInfo;

    if((ret = rtk_l34_extIntIPTable_get(netifId,&iptable))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }

    osal_memset(&naptr,0,sizeof(rtk_l34_naptInbound_entry_t));
    osal_memset(&napt,0,sizeof(rtk_l34_naptOutbound_entry_t));

    isTcp = (tuple.proto == L34_PROTO_TCP) ? ENABLED : DISABLED;

    idx = l34_hashIn(isTcp,tuple.dstIp,tuple.dstport);
    hashId = l34_hashOut(isTcp,natInfo.natIp,natInfo.natPort,tuple.srcIp,tuple.srcport);

    /*assigned naptr table*/
    osal_memset(&naptr,0,sizeof(rtk_l34_naptInbound_entry_t));
    naptr.extIpIdx  = netifId;
    naptr.extPortLSB    = tuple.dstport & 0xff;
    naptr.extPortHSB    = (tuple.dstport >> 8) & 0xff;
    naptr.isTcp = isTcp;
    naptr.intIp = natInfo.natIp;
    naptr.intPort = natInfo.natPort;
    if(natInfo.priValid)
    {
        naptr.priId = natInfo.priority;
        naptr.priValid = natInfo.priValid;
    }
    else
    {
        naptr.priId = iptable.pri;
        naptr.priValid = iptable.prival;
    }
    l34_remoteHash(netif_nat[netifId],tuple.srcIp,tuple.srcport,&naptr.remHash,&naptr.valid);

    if((ret=l34_empty_naptr_find(idx<<2,naptr,&idx))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }


    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "add upstream connection to naptr id %d\n",hashId);
    if((ret=l34_empty_napt_find(hashId << 2,&hashId))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }

    /*check connection is exist or not*/
    if((ret = l34_ds_connection_get(tuple,&connInfo)) ==RT_ERR_OK)
    {
        idx     = connInfo.naptrId;
        hashId  = connInfo.naptId;
    }


    /*assigned napt table*/
    napt.valid  = ENABLED;
    if(natInfo.priValid)
    {
        napt.priValid   = natInfo.priValid;
        napt.priValue   = natInfo.priority;
    }
    else
    {
        napt.priValid   = iptable.prival;
        napt.priValue   = iptable.pri;
    }
    napt.hashIdx    = idx;


    if((ret=rtk_l34_naptInboundTable_set(ENABLED,idx , &naptr))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }

    if((ret=rtk_l34_naptOutboundTable_set(ENABLED, hashId, &napt))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "add upstream connection to napt id %d\n",idx);

    return RT_ERR_OK;
}


static uint32
l34_us_connection_get(rtk_l34_tuple_t tuple,rtk_l34_connectInfo_t *connectInfo)
{

    rtk_l34_naptOutbound_entry_t napt;
    rtk_l34_naptInbound_entry_t naptr;
    rtk_l34_ext_intip_entry_t   iptable;
    int32 ret = 0;
    uint32 idx;
    uint16 hash;
    uint8 valid;
    uint32 cnt=0;
    rtk_enable_t    isTcp;
    rtk_l34_natType_t natType;

    isTcp = (tuple.proto == L34_PROTO_TCP) ? ENABLED : DISABLED;

    idx = l34_hashOut(isTcp,tuple.srcIp,tuple.srcport,tuple.dstIp,tuple.dstport);
    do{
        if((ret=rtk_l34_naptOutboundTable_get((idx <<2)+cnt, &napt))!=RT_ERR_OK){
            cnt++;
            continue;
        }

        if(napt.valid!=ENABLED){
            cnt++;
            continue;
        }
        if((ret=rtk_l34_naptInboundTable_get(napt.hashIdx, &naptr))!=RT_ERR_OK){
            cnt++;
            continue;
        }

        if((naptr.intIp != tuple.srcIp) || (naptr.intPort != tuple.srcport) || (naptr.isTcp != isTcp)) {
            cnt++;
            continue;
        }

        if(naptr.valid==DISABLED){
            cnt++;
            continue;
        }

        l34_remoteHash(netif_nat[naptr.extIpIdx], tuple.dstIp, tuple.dstport, &hash,&valid);

        if(naptr.remHash!=hash ){
            cnt++;
            continue;
        }
        break;

    }while(cnt <=3);

    if(4 <= cnt)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "can't find connection");
        return RT_ERR_ENTRY_NOTFOUND;
    }

    if((ret = rtk_l34_extIntIPTable_get(naptr.extIpIdx,&iptable))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }


    if((ret =   l34_naptType_raw2cfg(naptr.valid,&natType))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }

#ifdef CONFIG_SDK_APOLLOMP
{
    uint16 tmpPort;

    tmpPort = naptr.extPortLSB | (((napt.hashIdx >>2) & 0xff ) << 8);
    connectInfo->natport        = l34_hashIn(naptr.isTcp,iptable.extIpAddr,tmpPort);
    connectInfo->natport        = naptr.extPortLSB | (connectInfo->natport << 8);
}
#endif
#ifdef CONFIG_SDK_RTL9602C
    connectInfo->natport        = naptr.extPortLSB | (naptr.extPortHSB << 8);
#endif
    connectInfo->direction  = L34_DIR_UPSTREAM;
    connectInfo->naptId         = (idx <<2)+cnt;
    connectInfo->naptrId        = napt.hashIdx;
    connectInfo->natIp      = iptable.extIpAddr;

    connectInfo->netifId        = naptr.extIpIdx;
    connectInfo->tuple      = tuple;
    connectInfo->tuple.srcIp    = naptr.intIp;
    connectInfo->tuple.srcport  = naptr.intPort;
    connectInfo->priValied  = naptr.priValid;
    connectInfo->priValue   = naptr.priId;
    connectInfo->tuple.proto   = naptr.isTcp ? L34_PROTO_TCP : L34_PROTO_UDP;
    connectInfo->natType       = natType;

    return RT_ERR_OK;
}


static uint32
l34_ds_connection_get(rtk_l34_tuple_t tuple,rtk_l34_connectInfo_t *connectInfo)
{
    rtk_l34_naptOutbound_entry_t napt;
    rtk_l34_naptInbound_entry_t naptr;
    int32 ret =0 ;
    uint32 idx,naptId;
    rtk_enable_t isTcp;
    rtk_l34_natType_t natType;


    isTcp = (tuple.proto == L34_PROTO_TCP) ? ENABLED : DISABLED;

    idx = l34_hashIn(isTcp,tuple.dstIp,tuple.dstport);
    if((ret=l34_naptr_find(idx<<2,tuple.srcIp,tuple.srcport,&idx,&naptr))!=RT_ERR_OK){
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    naptId = l34_hashOut(isTcp,naptr.intIp,naptr.intPort,tuple.srcIp,tuple.srcport);

    if((ret=l34_napt_find(naptId<<2,idx,&naptId,&napt))!=RT_ERR_OK){
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }

    if((ret =   l34_naptType_raw2cfg(naptr.valid,&natType))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }

    connectInfo->direction  = L34_DIR_DOWNSTREAM;
    connectInfo->naptId         = naptId;
    connectInfo->naptrId        = idx;
    connectInfo->natIp      = naptr.intIp;
    connectInfo->natport        = naptr.intPort;
    connectInfo->netifId        = naptr.extIpIdx;
    connectInfo->tuple      = tuple;
    connectInfo->priValied  = naptr.priValid;
    connectInfo->priValue   = naptr.priId;
    connectInfo->tuple.proto   = naptr.isTcp ? L34_PROTO_TCP : L34_PROTO_UDP;
    connectInfo->natType       = natType;

    return RT_ERR_OK;
}



static int32
l34_connection_delete(rtk_l34_natType_t natType,uint32 naptId,uint32 naptrId)
{
    int32 ret = 0;
    uint32 i;
    rtk_l34_naptOutbound_entry_t napt;
    rtk_l34_naptInbound_entry_t naptr;

    osal_memset(&napt,0,sizeof(rtk_l34_naptOutbound_entry_t));
    osal_memset(&naptr,0,sizeof(rtk_l34_naptInbound_entry_t));

    if((ret=rtk_l34_naptOutboundTable_set(ENABLED,naptId,&napt))!=RT_ERR_OK){
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    /*check no other outbound use this inbound when nat type is not port restrict*/
    if(natType != L34_NAPT_PORT_RESTRICTED)
    {
        for(i=0;i<HAL_L34_NAPT_ENTRY_MAX();i++)
        {
            if((ret = rtk_l34_naptOutboundTable_get(i,&napt))!=RT_ERR_OK)
            {
                RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
                return ret;
            }
            /*still have entry*/
            if(napt.hashIdx==naptrId)
            {
                return RT_ERR_OK;
            }
        }
    }
    /*real delete naptr entry*/
    if((ret=rtk_l34_naptInboundTable_set(ENABLED,naptrId,&naptr))!=RT_ERR_OK){
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
     return RT_ERR_OK;
}


static int32
l34_globalState_get(rtk_enable_t *status)
{
    int32 ret = 0;
    uint32 stateType;

    for(stateType = L34_GLOBAL_STATE_START; stateType < L34_GLOBAL_STATE_END; stateType++)
    {
        ret=rtk_l34_globalState_get(stateType, &status[stateType]);
        if( (ret != RT_ERR_OK) && (ret != RT_ERR_INPUT))
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
        }
    }
    return RT_ERR_OK;
}

static int32
l34_portMapNetif_get(rtk_l34_portType_t type,uint32 *map)
{

    uint32 i,size;
    uint32 value;
    int32 ret = 0;
    switch(type){
    case L34_PORT_MAC:
        size = L34_MAC_PORT_MAX;
    break;
    case L34_PORT_EXTENSION:
        size = L34_EXT_PORT_MAX;
    break;
    case L34_PORT_DSLVC:
        size = L34_DSLVC_PORT_MAX;
    break;
    default:
        return RT_ERR_FAILED;
    break;
    }

    for(i=0;i<size;i++)
    {
        if((ret =rtk_l34_lookupPortMap_get(type,i,&value))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
        map[i] = value;
    }
    return RT_ERR_OK;
}

/*used to check netif is exist or not*/
static rtk_enable_t
l34_netif_exist(uint32 netifId)
{
    int32 ret = 0;
    rtk_l34_netif_entry_t   entry;

    if((ret = rtk_l34_netifTable_get(netifId,&entry))!=RT_ERR_OK)
    {
        RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
    }
    if(entry.mtu !=0)
    {
        return ENABLED;
    }else
    {
        return DISABLED;
    }
}


static int32
l34_route6_find(rtk_ipv6_addr_t ipaddr, uint32 prefixLen,uint32 *emptyId)
{
    uint32 i;
    int32 ret = 0;
    rtk_ipv6Routing_entry_t route;

    /*search not include default route, the last entry*/
    for(i=0;i < HAL_L34_IPV6_ROUTING_ENTRY_MAX()-1;i++)
    {
        osal_memset(&route,0,sizeof(rtk_ipv6Routing_entry_t));
        if((ret = rtk_l34_ipv6RoutingTable_get(i, &route))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
        }

        if(route.valid!=ENABLED)
        {
            *emptyId = i;
            return RT_ERR_OK;

        }
        if(!osal_memcmp(&route.ipv6Addr,&ipaddr,IPV6_ADDR_LEN) && prefixLen== route.ipv6PrefixLen)
        {
            *emptyId = i;
            return RT_ERR_OK;
        }

    }
    return RT_ERR_ENTRY_FULL;
}

/*return result: equal- 1, not equal - 0*/
static int32
l34_compare_ip6_addr(rtk_ipv6_addr_t ipA,rtk_ipv6_addr_t ipB,uint32 prefixLen)
{
    uint32 i;
    uint32 max,shiftBit,compareByte;
    uint8 a,b;
    max   = prefixLen >> 3;
    shiftBit = prefixLen % 8;

    for(i=0;i<max;i++)
    {
         compareByte = ipA.ipv6_addr[i]^ipB.ipv6_addr[i];
         if(compareByte)
         {
            return 0;
        }
    }
    if(shiftBit)
    {
        a = ipA.ipv6_addr[max]  >> (8-shiftBit);
        b = ipB.ipv6_addr[max]  >> (8-shiftBit);

        if(a!=b)
        {
            return 0;
        }
    }
    return 1;

}

static int32
l34_get_route6_by_ip6(rtk_ipv6_addr_t ipaddr,uint32 *routeId,rtk_ipv6Routing_entry_t *route)
{
    int32 ret = 0;
    uint32 i;

    for(i=0;i<HAL_L34_IPV6_ROUTING_ENTRY_MAX();i++)
    {
        osal_memset(route,0,sizeof(rtk_ipv6Routing_entry_t));
        if((ret = rtk_l34_ipv6RoutingTable_get(i,route))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
        }

        if(route->valid==ENABLED && l34_compare_ip6_addr(route->ipv6Addr,ipaddr,route->ipv6PrefixLen))
        {
            *routeId = i;
            return RT_ERR_OK;
        }

    }
    return RT_ERR_ENTRY_NOTFOUND;
}


static uint32
l34_neigh6_hash(uint64 ip6IfId, uint32 routeId)
{
    uint8 nb_8bhash_idx;
    uint8 nb_hash_idx;
    uint32 hashId;
    uint32 i;

    /*get nb_8bhash_idx*/
    nb_8bhash_idx = ip6IfId & 0xff;
    for(i=1;i<8;i++){
        nb_8bhash_idx  ^= (ip6IfId >> (8*i)) & 0xff;
    }
    /*get nb_hash_idx*/
    nb_hash_idx   = (nb_8bhash_idx & 0xf)^((nb_8bhash_idx >> 4) & 0xf);
    nb_hash_idx ^= ((((routeId & 0x1) <<3) & 0xf) | (((routeId >> 1 & 0x1) << 2) & 0xf) | ((routeId & 0x3) & 0xf));
    /*start to get hashId*/
    hashId =  nb_hash_idx*8;
    return hashId;
}



static int32
l34_empty_neigh6_find(rtk_ipv6_addr_t ipaddr,uint32 routeId,uint32 *emptyId,uint64 *ip64)
{
    int32 ret=0;
    int32 i=0;
    int32 hashId;
    uint32 hit;
    uint64 tmp=0;
    rtk_ipv6Neighbor_entry_t neigh;


    for(i=8;i < IPV6_ADDR_LEN ;i++)
    {
        tmp  = tmp << 8;
        tmp |= ipaddr.ipv6_addr[i];
    }
    *ip64 = tmp;

    hashId = l34_neigh6_hash(*ip64,routeId);

    for(i=0;i<8;i++)
    {
        *emptyId = hashId+i;
        if((ret = rtk_l34_ipv6NeighborTable_get(*emptyId, &neigh))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
                return ret;
        }
        hit = neigh.valid && (neigh.ipv6RouteIdx==routeId)  && (neigh.ipv6Ifid==tmp);

        if(hit)
        {
            return RT_ERR_ENTRY_EXIST;
        }else
        if(neigh.valid!=ENABLED)
        {
            return RT_ERR_OK;
        }
    }
    return RT_ERR_ENTRY_FULL;
}

static int32
l34_netifIp6Info_get(rtk_l34_netifInfo_t *netifInfo)
{
    int32 i;
    uint32 ip6size = 0;
    rtk_l34_route6Info_t route6Info;

    for(i=0;i<(L34_IPV6_ROUTE_MAX_SIZE-1);i++)
    {
        if(rtk_l34_route6_get(i, &route6Info)==RT_ERR_OK)
        {
            if( route6Info.route6Table.netifId==netifInfo->id)
            {
                osal_memcpy(&netifInfo->ip6Info[ip6size],&route6Info.route6Table,sizeof(rtk_l34_route6Table_t));
                ip6size += 1;
            }
        }
    }
    netifInfo->ip6size = ip6size;

    return RT_ERR_OK;
}


static int32
l34_get_neighInfo_by_ip(rtk_ipv6_addr_t ipaddr,uint32 *routeId,uint32 *netifId,uint32 *neighId,uint64 *ipv6Ifid)
{
    int32 ret = 0;
    rtk_ipv6Routing_entry_t  route;
    rtk_l34_nexthop_entry_t nexthop;

    /*search route6 by ipaddr*/
    if((ret = l34_get_route6_by_ip6(ipaddr,routeId,&route))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }

    /*get netif id*/
    if(route.type== L34_IPV6_ROUTE_TYPE_GLOBAL)
    {
        if((ret = rtk_l34_nexthopTable_get(route.nhOrIfidIdx,&nexthop))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
        }
        *netifId= nexthop.ifIdx;
    }else
    {
        *netifId = route.nhOrIfidIdx;
    }

    /*find empty neighbor table, and transfer ipv6IfId*/
    if((ret = l34_empty_neigh6_find(ipaddr,*routeId,neighId,ipv6Ifid))!=RT_ERR_ENTRY_EXIST)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    return RT_ERR_OK;
}




/*
 * Function Declaration
 */
/* Function Name:
 *      rtk_l34_net_init
 * Description:
 *      Initialize classification module.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
int32
rtk_l34_lite_init(void)
{
        int32 ret=0,port;
    rtk_l34_globalStateType_t cfg;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "");

    /* function body */
    l34_lite_init = INIT_COMPLETED ;
    /*for demo*/

    if((ret = rtk_vlan_vlanFunctionEnable_set(DISABLED))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
    }
    HAL_SCAN_ALL_PORT(port)
    {
        if((ret = rtk_qos_dscpRemarkEnable_set( port, ENABLED))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
                return ret;
        }
    }
    /*initial l34 module first*/
    if((ret = rtk_l34_init()) != RT_ERR_OK){
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
    }
    /*start to init l34 lite*/
    /*enable l3 nat*/
    cfg = L34_GLOBAL_L3NAT_STATE;
    if((ret = rtk_l34_globalState_set(cfg, ENABLED))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        l34_lite_init = INIT_NOT_COMPLETED ;
        return ret;
    }
    /*enable l4 nat*/
    cfg = L34_GLOBAL_L4NAT_STATE;
    if((ret = rtk_l34_globalState_set(cfg, ENABLED))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        l34_lite_init = INIT_NOT_COMPLETED ;
        return ret;
    }
    /*enable ttl minus*/
    cfg = L34_GLOBAL_TTLMINUS_STATE;
    if((ret = rtk_l34_globalState_set(cfg, ENABLED))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        l34_lite_init = INIT_NOT_COMPLETED ;
        return ret;
    }
    /*set lookup mode mac base*/
    if((ret = rtk_l34_lookupMode_set(L34_LOOKUP_MAC_BASE))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        l34_lite_init = INIT_NOT_COMPLETED ;
        return ret;
    }

    /*enable l4 checksum allow for 0371*/
    cfg = L34_GLOBAL_L4CHKSERRALLOW_STATE;
    if((ret = rtk_l34_globalState_set(cfg, ENABLED))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        l34_lite_init = INIT_NOT_COMPLETED ;
        return ret;
    }
    return RT_ERR_OK;
}   /* end of rtk_l34_net_init */

/*
 * Function Declaration
 */
/* Function Name:
 *      rtk_l34_netif_create
 * Description:
 *      create a new interface
 * Input:
 *      netifId: interface index
 *    netifType: WAN | LAN
 *      ifmac: MAC address for this interface
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
int32
rtk_l34_netif_create(uint32 netifId,rtk_l34_netifType_t netifType, rtk_mac_t ifmac)
{
        int32 ret = 0;
    rtk_l34_routing_entry_t     route;
    rtk_l34_netif_entry_t       entry;
    rtk_l34_ext_intip_entry_t   iptable;
    rtk_l34_nexthop_entry_t     nextHop;
    rtk_l34_pppoe_entry_t       ppp;

    //RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "netifId=%d,netifType=%d,ifmac=%d",netifId, netifType, ifmac);

    /* check Init status */
    RT_INIT_CHK(l34_lite_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_L34_NETIF_ENTRY_MAX() <=netifId), RT_ERR_INPUT);
    RT_PARAM_CHK((L34_NETIF_TYPE_END <=netifType), RT_ERR_INPUT);
    RT_PARAM_CHK((L34_IF_NUM_MAX <=netifId), RT_ERR_INPUT);

    /*check netif exist*/
    if(l34_netif_exist(netifId)!=DISABLED)
    {
        RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "netif exist");
        return RT_ERR_ENTRY_EXIST;
    }

    /*get netif table for assigned value*/
    if((ret = rtk_l34_netifTable_get(netifId,&entry))!=RT_ERR_OK)
    {
        RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    /*start to create a net interface*/
    osal_memset(&entry,0,sizeof(rtk_l34_netif_entry_t));
    l34_default_netif_set(ifmac,&entry);
    /*set to netif table*/
    if((ret = rtk_l34_netifTable_set(netifId,&entry)) != RT_ERR_OK)
    {
        RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
    }
    /*create a local route*/
    osal_memset(&route,0,sizeof(rtk_l34_routing_entry_t));
    l34_default_localRoute_set(netifId,netifType,&route);

    if((ret=rtk_l34_routingTable_set(netifId,&route)) != RT_ERR_OK)
    {
        RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
    }
    /*initial ppp table*/
    ppp.sessionID = 0;
    if((ret=rtk_l34_pppoeTable_set(netifId,&ppp)) != RT_ERR_OK)
    {
        RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    /*initial next-hop table*/
    osal_memset(&nextHop,0,sizeof(rtk_l34_nexthop_entry_t));
    l34_default_nextHop_set(netifId,&nextHop);
    if((ret=rtk_l34_nexthopTable_set(netifId,&nextHop)) != RT_ERR_OK)
    {
        RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
    }
    /*initial external_interal ip table*/
    osal_memset(&iptable,0,sizeof(rtk_l34_ext_intip_entry_t));
    l34_default_iptable_set(netifId,&iptable);
    if((ret=rtk_l34_extIntIPTable_set(netifId,&iptable)) != RT_ERR_OK)
    {
        RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
    }
    return RT_ERR_OK;
}   /* end of rtk_l34_netif_create */


/*
 * Function Declaration
 */
/* Function Name:
 *      rtk_l34_netifPPPoE_set
 * Description:
 *      set pppoe relation parameters
 * Input:
 *      netifId: interface index
 *    pppState: enable or disable pppoe functionality
 *    sessionId: PPPoE session id
 *      passThroughState: enable or disable PPPoE pass through function in this interface.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
int32
rtk_l34_netifPPPoE_set(uint32 netifId,rtk_enable_t pppState,uint32 sessionId, rtk_enable_t passThroughState)
{
    int32 ret = 0;
    rtk_l34_netifType_t     netifType;
    rtk_l34_pppoe_entry_t   pppEntry;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "netifId=%d,sessionId=%d,passThroughState=%d",netifId, sessionId, passThroughState);

    /* check Init status */
    RT_INIT_CHK(l34_lite_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_L34_NETIF_ENTRY_MAX() <=netifId), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= passThroughState), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= pppState), RT_ERR_INPUT);
    RT_PARAM_CHK((L34_IF_NUM_MAX <=netifId), RT_ERR_INPUT);

    /*check netif exist*/
    if(l34_netif_exist(netifId)!=ENABLED)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
    }
    /*get routing table for check netif is wan type, else reject (only wan interface can change to pppoe)*/
    if((ret = l34_netif_type_get(netifId,&netifType))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
    }
    if(netifType != L34_NETIF_TYPE_WAN)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
    }

    /*get and set to next-hop for pppoe core*/
    if((ret = l34_nextHopPPP_set(netifId,pppState,passThroughState))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
    }
    /*set to ppp table*/
    pppEntry.sessionID = sessionId;
    if((ret = rtk_l34_pppoeTable_set(netifId,&pppEntry)) != RT_ERR_OK){
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
    }

    return RT_ERR_OK;
}   /* end of rtk_l34_netifPPPoE_set */


/*
 * Function Declaration
 */
/* Function Name:
 *      rtk_l34_netifVlan_set
 * Description:
 *      set interface vlan configure
 * Input:
 *      netifId: interface id
 *    vid: vlan id for interface
 *      defaultPri: default priority for interface, value 8 means disable
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
int32
rtk_l34_netifVlan_set(uint32 netifId, rtk_vlan_t vid, rtk_pri_t defaultPri)
{
        int32 ret =0;
    rtk_l34_netif_entry_t       netif;
    rtk_l34_ext_intip_entry_t   ipTable;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "netifId=%d,vid=%d,defaultPri=%d",netifId, vid, defaultPri);

    /* check Init status */
    RT_INIT_CHK(l34_lite_init);

    /* parameter check */
    RT_PARAM_CHK((vid > RTK_VLAN_ID_MAX), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((L34_VLAN_PRI_DISABLE < defaultPri), RT_ERR_INPUT);
    RT_PARAM_CHK((L34_IF_NUM_MAX <=netifId), RT_ERR_INPUT);

    /*check netif is exist*/
    if((l34_netif_exist(netifId))!=ENABLED)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
         return RT_ERR_FAILED;
    }
    /*get netif table */
    osal_memset(&netif,0,sizeof(rtk_l34_netif_entry_t));
    if((ret = rtk_l34_netifTable_get(netifId,&netif))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
    }
    netif.vlan_id = (uint16) vid;
    /*set netif table */
    if((ret = rtk_l34_netifTable_set(netifId,&netif))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
    }
    /*get external ip*/
    osal_memset(&ipTable,0,sizeof(rtk_l34_ext_intip_entry_t));
    if((ret = rtk_l34_extIntIPTable_get(netifId,&ipTable)) != RT_ERR_OK){
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
    }
    if(defaultPri!=L34_VLAN_PRI_DISABLE)
    {
        ipTable.pri     = defaultPri;
        ipTable.prival  = ENABLED;
    }else
    {
        ipTable.pri     = 0;
        ipTable.prival  = DISABLED;
    }
    /*set to external ip*/
    if((ret = rtk_l34_extIntIPTable_set(netifId,&ipTable)) != RT_ERR_OK){
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
    }
    return RT_ERR_OK;
}   /* end of rtk_l34_netifVlan */


/*
 * Function Declaration
 */
/* Function Name:
 *      rtk_l34_netifRoutingState_set
 * Description:
 *      set interface routing state
 * Input:
 *      netifId: interface id
 *    routingState: disable or enable
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
int32
rtk_l34_netifRoutingState_set(uint32 netifId, rtk_enable_t routingState)
{
        int32 ret =0;
    rtk_l34_netif_entry_t netif;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "netifId=%d,routingState=%d",netifId, routingState);

    /* check Init status */
    RT_INIT_CHK(l34_lite_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= routingState), RT_ERR_INPUT);
    RT_PARAM_CHK((L34_IF_NUM_MAX <=netifId), RT_ERR_INPUT);

    /* check is exist*/
    if(l34_netif_exist(netifId)!=ENABLED)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
    }
    /* get netif data */
    osal_memset(&netif,0,sizeof(rtk_l34_netif_entry_t));
    if((ret = rtk_l34_netifTable_get(netifId,&netif))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
    }
    netif.enable_rounting = routingState;
    /*set to netif data*/
    if((ret = rtk_l34_netifTable_set(netifId,&netif))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
    }
    return RT_ERR_OK;
}   /* end of rtk_l34_netifRoutingState_set */


/*
 * Function Declaration
 */
/* Function Name:
 *      rtk_l34_netifMtu_set
 * Description:
 *      set interface mtu size
 * Input:
 *      netifId: interface id
 *    mtu: interface mtu size
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
int32
rtk_l34_netifMtu_set(uint32 netifId, uint32 mtu)
{
    int32 ret =0;
    rtk_l34_netif_entry_t netif;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "netifId=%d,mtu=%d",netifId, mtu);


    /* check Init status */
    RT_INIT_CHK(l34_lite_init);

    /* parameter check */
    RT_PARAM_CHK((mtu >= L34_MTU_MAX), RT_ERR_INPUT);
    RT_PARAM_CHK((L34_IF_NUM_MAX <=netifId), RT_ERR_INPUT);

    /* check is exist*/
    if(l34_netif_exist(netifId)!=ENABLED)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    /* get netif data */
    osal_memset(&netif,0,sizeof(rtk_l34_netif_entry_t));
    if((ret = rtk_l34_netifTable_get(netifId,&netif))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
    }
    netif.mtu = (uint16)mtu;
    /*set to netif data*/
    if((ret = rtk_l34_netifTable_set(netifId,&netif))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    return RT_ERR_OK;
}   /* end of rtk_l34_netifMtu_set */


/*
 * Function Declaration
 */
/* Function Name:
 *      rtk_l34_netifIpaddr_set
 * Description:
 *      set interface ipaddress
 * Input:
 *      netifId: interface id
 *    ipaddr: interface ip address
 *      ipmask: ip mask length
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
int32
rtk_l34_netifIpaddr_set(uint32 netifId, rtk_ip_addr_t ipaddr,rtk_ip_addr_t ipmask)
{
        int32 ret =0 ;
    rtk_l34_routing_entry_t     route;
    rtk_l34_ext_intip_entry_t   iptable;
    rtk_l34_netifType_t         netifType;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "netifId=%d,ipaddr=%d,ipmask=%d",netifId, ipaddr, ipmask);

    /* check Init status */
    RT_INIT_CHK(l34_lite_init);

    /* parameter check */
    RT_PARAM_CHK((L34_IF_NUM_MAX <=netifId), RT_ERR_INPUT);

    /* check is exist*/
    if(l34_netif_exist(netifId)!=ENABLED)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
    }
    /*get and set routing table*/
    osal_memset(&route,0,sizeof(rtk_l34_routing_entry_t));

    if(ipaddr!=0){
        if((ret = rtk_l34_routingTable_get(netifId,&route))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
                return ret;
        }
        route.ipAddr  = ipaddr;
        route.ipMask = ipmask;
        route.valid    = ENABLED;
    }else{
        if((ret = l34_netif_type_get(netifId,&netifType))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
                return ret;
        }
        l34_default_localRoute_set(netifId, netifType, &route);
    }

    if((ret = rtk_l34_routingTable_set(netifId,&route))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }

    /*get and set  external ip table*/
    if((ret = rtk_l34_extIntIPTable_get(netifId,&iptable))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    iptable.extIpAddr = ipaddr;
    iptable.valid = ENABLED;
    if(iptable.type ==L34_EXTIP_TYPE_LP)
    {
        iptable.intIpAddr = ipaddr;
    }

    if(route.internal){
        iptable.valid = DISABLED;
    }
    if((ret = rtk_l34_extIntIPTable_set(netifId,&iptable))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
    }

    return RT_ERR_OK;
}   /* end of rtk_l34_netifIpaddr_set */


/*
 * Function Declaration
 */
/* Function Name:
 *      rtk_l34_netifMtu_set
 * Description:
 *      set interface nat related configuration
 * Input:
 *      netifId: interface id
 *    natType: interface nat type for connections
 *      internalIp: for nat mode, interanl ip address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
int32
rtk_l34_netifNat_set(uint32 netifId, rtk_l34_natType_t natType ,rtk_ip_addr_t  internalp)
{
        int32 ret=0;
    rtk_l34_netifType_t     netifType;
    rtk_l34_ext_intip_entry_t   iptable;
    rtk_l34_routing_entry_t     route;
    uint32                  tmp;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "netifId=%d,natType=%d,internalp=%d",netifId, natType, internalp);

    /* check Init status */
    RT_INIT_CHK(l34_lite_init);

    /* parameter check */
    RT_PARAM_CHK((L34_NAT_TYPE_END <=natType), RT_ERR_INPUT);
    RT_PARAM_CHK((L34_IF_NUM_MAX <=netifId), RT_ERR_INPUT);

    /* check is exist*/
    if(l34_netif_exist(netifId)!=ENABLED)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    /*get routing table for check netif is wan type, else reject*/
    osal_memset(&route,0,sizeof(rtk_l34_routing_entry_t));
    if((ret = rtk_l34_routingTable_get(netifId,&route))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
    }
    /*software nat type*/
    netif_nat[netifId]  = natType;

    l34_netifType_raw2cfg(route.internal,&netifType);
    if(netifType!= L34_NETIF_TYPE_WAN){
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return RT_ERR_INPUT;
    }
    /*get exnternal ip table*/
    osal_memset(&iptable,0,sizeof(rtk_l34_ext_intip_entry_t));
    if((ret = rtk_l34_extIntIPTable_get(netifId,&iptable))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
    }

    if((ret = l34_natType_cfg2raw(natType,&tmp))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
    }

    iptable.type = tmp;
    if(L34_NAT==natType)
    {
        iptable.intIpAddr = internalp;
    }else
    if(L34_LOCAL_PUBLIC==natType)
    {
        iptable.intIpAddr = iptable.extIpAddr;
    }else
    {
        iptable.intIpAddr = 0;
    }

    if((ret = rtk_l34_extIntIPTable_set(netifId,&iptable))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    return RT_ERR_OK;
}   /* end of rtk_l34_netifNat_set */



/*
 * Function Declaration
 */
/* Function Name:
 *      rtk_l34_netifState_set
 * Description:
 *      set interface status
 * Input:
 *      netifId: interface id
 *    ifState: interface status, up/down
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
int32
rtk_l34_netifState_set(uint32 netifId,rtk_enable_t ifState)
{
        int32 ret=0;
    rtk_l34_netif_entry_t       netif;
    rtk_l34_routing_entry_t     route;
    rtk_l34_ext_intip_entry_t   iptable;


    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "netifId=%d,ifState=%d",netifId, ifState);

    /* check Init status */
    RT_INIT_CHK(l34_lite_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= ifState), RT_ERR_INPUT);
    RT_PARAM_CHK((L34_IF_NUM_MAX <=netifId), RT_ERR_INPUT);

    /* check is exist*/
    if(l34_netif_exist(netifId)!=ENABLED)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
    }
    /*check state chang or not*/
    osal_memset(&netif,0,sizeof(rtk_l34_netif_entry_t));
    if((ret = rtk_l34_netifTable_get(netifId,&netif))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
         return ret;
    }
    if(netif.valid == ifState)
    {
        return RT_ERR_OK;
    }
    /*start to handle state change*/
    /*change iptable*/
    osal_memset(&iptable,0,sizeof(rtk_l34_ext_intip_entry_t));
    if((ret = rtk_l34_extIntIPTable_get(netifId,&iptable))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    iptable.valid = ifState;
    if((ret = rtk_l34_extIntIPTable_set(netifId,&iptable))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    /*change route state*/
    osal_memset(&route,0,sizeof(rtk_l34_routing_entry_t));
    if((ret = rtk_l34_routingTable_get(netifId,&route))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    route.valid = ifState;
    if((ret = rtk_l34_routingTable_set(netifId,&route))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    /*change netif*/
    netif.valid = ifState;
    if((ret = rtk_l34_netifTable_set(netifId,&netif))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    return RT_ERR_OK;
}   /* end of rtk_l34_netifState_set */


/*
 * Function Declaration
 */
/* Function Name:
 *      rtk_l34_netif_get
 * Description:
 *      get interface information
 * Input:
 *      netifId: interface id
 *    ifState: interface status, up/down
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
int32
rtk_l34_netif_get(uint32 netifId,rtk_l34_netifInfo_t *netifInfo)
{
        int32 ret=0;
    rtk_l34_routing_entry_t     route;
    rtk_l34_netif_entry_t       netif;
    rtk_l34_ext_intip_entry_t   iptable;
    rtk_l34_nexthop_entry_t nextHop;
    rtk_l34_pppoe_entry_t       ppp;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "netifId=%d",netifId);

    /* check Init status */
    RT_INIT_CHK(l34_lite_init);
    /* parameter check */
    RT_PARAM_CHK((HAL_L34_NETIF_ENTRY_MAX() <=netifId), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == netifInfo), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((L34_IF_NUM_MAX <=netifId), RT_ERR_INPUT);

    /*check if is exist*/
    if(l34_netif_exist(netifId)!=ENABLED)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    /*get netif*/
    if((ret = rtk_l34_netifTable_get(netifId,&netif))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    netifInfo->id = netifId;
    netifInfo->l2NetIfInfo.ifmac        = netif.gateway_mac;
    netifInfo->l2NetIfInfo.ifState      = netif.valid;
    netifInfo->l2NetIfInfo.mtu          = netif.mtu;
    netifInfo->l2NetIfInfo.routingState     = netif.enable_rounting;
    netifInfo->l2NetIfInfo.vid          = netif.vlan_id;

    /*get ip*/
    if((ret = rtk_l34_extIntIPTable_get(netifId,&iptable))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    if(iptable.prival==ENABLED){
        netifInfo->ipInfo.defaultPri= iptable.pri;
    }else{
        netifInfo->ipInfo.defaultPri= L34_VLAN_PRI_DISABLE;
    }
    netifInfo->ipInfo.internalIp    = iptable.intIpAddr;
    netifInfo->ipInfo.ipaddr        = iptable.extIpAddr;
    netifInfo->ipInfo.natType       = netif_nat[netifId];

    /*get route*/
    if((ret = rtk_l34_routingTable_get(netifId,&route))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
    }
    netifInfo->ipInfo.netifType = route.internal ? L34_NETIF_TYPE_LAN : L34_NETIF_TYPE_WAN;
    netifInfo->ipInfo.ipmask    = route.ipMask;

    /*get nexthop*/
    if((ret = rtk_l34_nexthopTable_get(netifId,&nextHop))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
    }
    netifInfo->pppInfo.pppState = (nextHop.type==L34_NH_PPPOE) ? ENABLED : DISABLED;

    if(netifInfo->pppInfo.pppState != DISABLED)
    {
        netifInfo->pppInfo.passThroughState = nextHop.keepPppoe;

        /*get l2 mac from index*/
        if((ret = l34_get_mac_by_l2entryId(nextHop.nhIdx,&netifInfo->pppInfo.serverMac))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
                return ret;
        }

        /*get ppp*/
        osal_memset(&ppp,0,sizeof(rtk_l34_pppoe_entry_t));
        if((ret = rtk_l34_pppoeTable_get(netifId,&ppp))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
                return ret;
        }
        netifInfo->pppInfo.sessionId= ppp.sessionID;
    }
    if((ret = l34_netifIp6Info_get(netifInfo))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    return RT_ERR_OK;
}   /* end of rtk_l34_netif_get */



/*
 * Function Declaration
 */
/* Function Name:
 *      rtk_l34_netif_del
 * Description:
 *      delete interface
 * Input:
 *      netifId: interface id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
int32
rtk_l34_netif_del(uint32 netifId)
{
        int32 ret=0;
    rtk_l34_routing_entry_t     route;
    rtk_l34_netif_entry_t       netif;
    rtk_l34_ext_intip_entry_t   iptable;
    rtk_l34_nexthop_entry_t     nextHop;
    rtk_l34_pppoe_entry_t       ppp;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "netifId=%d",netifId);

    /* check Init status */
    RT_INIT_CHK(l34_lite_init);

    /* parameter check */
    RT_PARAM_CHK((L34_IF_NUM_MAX <=netifId), RT_ERR_INPUT);

    if(l34_netif_exist(netifId)!=ENABLED)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    /*set netif*/
    osal_memset(&netif,0,sizeof(rtk_l34_netif_entry_t));
    if((ret = rtk_l34_netifTable_set(netifId,&netif))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    /*set route*/
    osal_memset(&route,0,sizeof(rtk_l34_routing_entry_t));
    if((ret = rtk_l34_routingTable_set(netifId,&route))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    /*set iptable*/
    osal_memset(&iptable,0,sizeof(rtk_l34_ext_intip_entry_t));
    if((ret = rtk_l34_extIntIPTable_set(netifId,&iptable))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    netif_nat[netifId] = 0;
    /*set next-hop*/
    osal_memset(&nextHop,0,sizeof(rtk_l34_nexthop_entry_t));
    if((ret = rtk_l34_nexthopTable_set(netifId,&nextHop))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    /*set ppp*/
    osal_memset(&ppp,0,sizeof(rtk_l34_pppoe_entry_t));
    if((ret = rtk_l34_pppoeTable_set(netifId,&ppp))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    return RT_ERR_OK;
}   /* end of rtk_l34_netif_del */


/*
 * Function Declaration
 */
/* Function Name:
 *      rtk_l34_arp_add
 * Description:
 *      add an arp entry
 * Input:
 *      ipaddr: ip address
 *    mac: mac address for ipaddr
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
int32
rtk_l34_arp_add(rtk_ip_addr_t ipaddr, rtk_mac_t mac)
{
    int32 ret=0;
    uint32 netifId,arpId;
    rtk_l2_ucastAddr_t   l2Addr;
    rtk_l34_arp_entry_t  arpEntry;

    /* check Init status */
    RT_INIT_CHK(l34_lite_init);

    if((ret = l34_get_arpInfo_by_ip(ipaddr,&netifId,&arpId))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }

    /*get l2 entry */
    if((ret = l34_get_l2entry_by_mac(netifId,mac,&l2Addr))!=RT_ERR_OK){
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }

    /*set arp table*/
    arpEntry.nhIdx = l2Addr.index;
    arpEntry.valid = ENABLED;
    arpEntry.index = arpId;
#ifdef CONFIG_SDK_RTL9602C
    arpEntry.ipAddr = ipaddr;
#endif
    if((ret=rtk_l34_arpTable_set(arpId,&arpEntry))!= RT_ERR_OK){
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
#ifdef CONFIG_SDK_RTL9602C
{
    uint32 tableId, indexId;
    tableId = arpId / 32;
    indexId = arpId % 32;
    arpEntry_valid[tableId] |= (1 << indexId);
}
#endif
    /*update l2 table*/
    l2Addr.flags|=RTK_L2_UCAST_FLAG_ARP_USED;

    if((ret=rtk_l2_addr_add(&l2Addr)) != RT_ERR_OK){
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }

    return RT_ERR_OK;
}   /* end of rtk_l34_arp_add */


/*
 * Function Declaration
 */
/* Function Name:
 *      rtk_l34_arp_get
 * Description:
 *      get mac address from ip address
 * Input:
 *      ipaddr: ip address for get arp entry
 *     *arpInfo: point of arp information
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
int32
rtk_l34_arp_get(rtk_ip_addr_t ipaddr,rtk_l34_arpInfo_t *arpInfo)
{
        int32 ret=0;
    rtk_l2_ucastAddr_t  lutTable;
    rtk_l34_arp_entry_t     arpEntry;
    uint32 netifId,arpId;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "ipaddr=%d",ipaddr);

    /* check Init status */
    RT_INIT_CHK(l34_lite_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == arpInfo), RT_ERR_NULL_POINTER);

    /* get route by ipaddr*/
    if((ret = l34_get_arpInfo_by_ip(ipaddr,&netifId,&arpId))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    /*get arp table*/
    if((ret=rtk_l34_arpTable_get(arpId,&arpEntry))!= RT_ERR_OK){
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    /*check arp vaild*/
    if(arpEntry.valid != ENABLED)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_ENTRY_NOTFOUND;
    }
    /*get l2 mac*/
    osal_memset(&lutTable,0,sizeof(rtk_l2_ucastAddr_t));
    if((ret = rtk_l2_nextValidAddr_get(&arpEntry.nhIdx,&lutTable))!=RT_ERR_OK){
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }

    arpInfo->arpId  = arpId;
    arpInfo->mac    = lutTable.mac;
    arpInfo->l2Id   = lutTable.index;
    arpInfo->netifId    = netifId;

     return RT_ERR_OK;
}   /* end of rtk_l34_arp_get */


/*
 * Function Declaration
 */
/* Function Name:
 *      rtk_l34_arp_del
 * Description:
 *      delete an arp entry
 * Input:
 *      ipaddr: ip address for delete arp entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
int32
rtk_l34_arp_del(rtk_ip_addr_t ipaddr)
{
        int32 ret=0;
    rtk_l2_ucastAddr_t  lutTable;
    rtk_l34_arp_entry_t     arpEntry;
    uint32 netifId,arpId;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "ipaddr=%d",ipaddr);

    /* check Init status */
    RT_INIT_CHK(l34_lite_init);

    /* get route by ipaddr*/
    if((ret = l34_get_arpInfo_by_ip(ipaddr,&netifId,&arpId))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    /*get arp table*/
    if((ret=rtk_l34_arpTable_get(arpId,&arpEntry))!= RT_ERR_OK){
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    /*check arp vaild*/
    if(arpEntry.valid != ENABLED)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_ENTRY_NOTFOUND;
    }

    osal_memset(&lutTable,0,sizeof(rtk_l2_ucastAddr_t));
    if((ret = rtk_l2_nextValidAddr_get(&arpEntry.nhIdx, &lutTable))!=RT_ERR_OK){
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    /*update l2 table*/
    lutTable.flags &= ~(RTK_L2_UCAST_FLAG_ARP_USED);
    if((ret = rtk_l34_arpTable_del(arpId)) != RT_ERR_OK){
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }

    if((ret=rtk_l2_addr_add(&lutTable)) != RT_ERR_OK){
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
     return RT_ERR_OK;
}   /* end of rtk_l34_arp_del */


/*
 * Function Declaration
 */
/* Function Name:
 *      rtk_l34_route_add
 * Description:
 *      add a routing table entry
 * Input:
 *    routeType: type of this routing table
 *      routeInfo: routing table necessary information
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
int32
rtk_l34_route_add(rtk_l34_routeType_t routeType,rtk_l34_routeTable_t routeTable)
{
    int32 ret=0;
    //RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "routeType=%d,routeTable=%d",routeType, routeTable);

    /* check Init status */
    RT_INIT_CHK(l34_lite_init);

    /* parameter check */
    RT_PARAM_CHK((L34_ROUTE_TYPE_END <=routeType), RT_ERR_INPUT);
    RT_PARAM_CHK(L34_IF_NUM_MAX <= routeTable.netifId,RT_ERR_INPUT);

    /*check if is exist*/
    if(l34_netif_exist(routeTable.netifId)!=ENABLED)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_INPUT;
    }

    switch(routeType){
    case L34_ROUTE_CPU:
    {
        if((ret = l34_local_route_set(routeTable.netifId,L34_PROCESS_CPU,routeTable.ipaddr,routeTable.ipmask))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
        }
    }
    break;
    case L34_ROUTE_DROP:
    {
        if((ret = l34_local_route_set(routeTable.netifId,L34_PROCESS_DROP,routeTable.ipaddr,routeTable.ipmask))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
        }
    }
    break;
    case L34_ROUTE_LOCAL:
    {
        if((ret = l34_local_route_set(routeTable.netifId,L34_PROCESS_ARP,routeTable.ipaddr,routeTable.ipmask))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
        }
    }
    break;
    case L34_ROUTE_STATIC:
    {
        if((ret = l34_global_route_set(routeTable.netifId,routeTable.netifId,routeTable.ipaddr,routeTable.ipmask,routeTable.gatewayMac))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
        }
    }
    break;
    case L34_ROUTE_DEFAULT:
    {
        if((ret = l34_global_route_set(L34_DEFAULT_ROUTE,routeTable.netifId,0,0,routeTable.gatewayMac))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
        }
    }
    break;
    default:
        return RT_ERR_INPUT;
    break;
    }
    return RT_ERR_OK;
}   /* end of rtk_l34_route_add */


/*
 * Function Declaration
 */
/* Function Name:
 *      rtk_l34_route_get
 * Description:
 *      get a routing table entry
 * Input:
 *      routeIndex: routing table index for get routing information
 *      routeInfo: point of routing information
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
int32
rtk_l34_route_get(uint32 routeIndex,rtk_l34_routeInfo_t *routeInfo)
{
    int32 ret=0;
    rtk_l34_routing_entry_t  route;
    rtk_l34_routeTable_t    *table;
    rtk_l34_nexthop_entry_t nextHop;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "routeIndex=%d",routeIndex);

    /* check Init status */
    RT_INIT_CHK(l34_lite_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_L34_ROUTING_ENTRY_MAX() <=routeIndex), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == routeInfo), RT_ERR_NULL_POINTER);

    /*get route*/
    osal_memset(&route,0,sizeof(rtk_l34_routing_entry_t));
    if((ret = rtk_l34_routingTable_get(routeIndex,&route))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    /*check is vaild or not*/
    if(route.valid!=ENABLED)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_ENTRY_NOTFOUND;
    }
    /* assign value to route info */
    table = &routeInfo->routeTable;
    table->ipaddr = route.ipAddr;
    table->ipmask = route.ipMask;

    switch(route.process){
    case L34_PROCESS_CPU:
    case L34_PROCESS_DROP:
        /*do nothing*/
    break;
    case L34_PROCESS_ARP:
    {
        table->netifId = routeIndex;
        routeInfo->arpStart = route.arpStart << 2;
        routeInfo->arpEnd   = (route.arpEnd <<2)+3;
    }
    break;
    case L34_PROCESS_NH:
    {
        osal_memset(&nextHop,0,sizeof(rtk_l34_nexthop_entry_t));
        if((ret = rtk_l34_nexthopTable_get(route.nhStart,&nextHop))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
        }
        routeInfo->nhId = route.nhStart;
        routeInfo->l2tableId = nextHop.nhIdx;
        table->netifId = nextHop.ifIdx;
        if((ret = l34_get_mac_by_l2entryId(nextHop.nhIdx,&table->gatewayMac))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
        }
    }
    break;
    default:
        return RT_ERR_INPUT;
    break;
    }
    /*get other infomation*/
    routeInfo->id = routeIndex;
    l34_routeType_raw2cfg(route.process,routeIndex,&routeInfo->routeType);
    routeInfo->vaild = route.valid;

    return RT_ERR_OK;
}   /* end of rtk_l34_route_get */


/*
 * Function Declaration
 */
/* Function Name:
 *      rtk_l34_route_del
 * Description:
 *      delete a routing table entry
 * Input:
 *      ipaddr: ip address for routing match
 *    ipmask: ip mask length for routing match
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
int32
rtk_l34_route_del(rtk_ip_addr_t ipaddr,rtk_ip_addr_t ipmask)
{
        int32   ret=0;
    uint32  routeId;
    rtk_l34_routing_entry_t route;
    rtk_l34_nexthop_entry_t nexthop;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "ipaddr=%d,ipmask=%d",ipaddr, ipmask);

    /* check Init status */
    RT_INIT_CHK(l34_lite_init);

    /* parameter check */
    RT_PARAM_CHK((31 < ipmask), RT_ERR_INPUT);

    /* get route table */
    if((ret = l34_get_routeId_by_ip(ipaddr,ipmask,&routeId,&route))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    /*check if routeId is netif local route, can't delete it*/
    if(route.process== L34_PROCESS_ARP  && routeId < L34_IF_NUM_MAX)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    /*else delete nexthop first*/
    osal_memset(&nexthop,0,sizeof(rtk_l34_nexthop_entry_t));
    if((ret = rtk_l34_nexthopTable_set(route.nhStart,&nexthop))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    osal_memset(&route,0,sizeof(rtk_l34_routing_entry_t));
    if((ret = rtk_l34_routingTable_set(routeId,&route))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }

    return RT_ERR_OK;
}   /* end of rtk_l34_route_del */


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
rtk_l34_connectTrack_add(rtk_l34_direct_t dir, rtk_l34_tuple_t tuple, rtk_l34_nat_info_t natInfo)
{
        int32 ret=0;
    rtk_ip_addr_t extIp;
    uint32 netifId;

    /* check Init status */
    RT_INIT_CHK(l34_lite_init);

    /* parameter check */

    /*get external ip addr*/
    extIp = (dir==L34_DIR_UPSTREAM) ?  natInfo.natIp :  tuple.dstIp;

    if((ret = l34_get_netifId_by_ip(extIp,&netifId))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }

    if(l34_netif_exist(netifId)!=ENABLED)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if(netif_nat[netifId] ==L34_NAT || netif_nat[netifId] == L34_LOCAL_PUBLIC){
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_INPUT;
    }

    switch(dir){
    case L34_DIR_UPSTREAM:
        if((ret =l34_us_connection_add(netifId,tuple,natInfo)) != RT_ERR_OK){
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
    break;
    case L34_DIR_DOWNSTREAM:
        if((ret =l34_ds_connection_add(netifId,tuple,natInfo)) != RT_ERR_OK){
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
    break;
    default:
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_CHIP_NOT_SUPPORTED;
    break;
    }
    return RT_ERR_OK;
}   /* end of rtk_l34_connectTrack_add */


/*
 * Function Declaration
 */
/* Function Name:
 *      rtk_l34_connectTrack_get
 * Description:
 *      get a connection information
 * Input:
 *    tuple: five tuple for connection
 *    connectInfo: point of connection information
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
int32
rtk_l34_connectTrack_get(rtk_l34_tuple_t tuple, rtk_l34_connectInfo_t *connectInfo)
{
    int32 ret=0;
    rtk_l34_direct_t dir;

    //RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "tuple=%d,connectInfo=%d",tuple, connectInfo);

    /* check Init status */
    RT_INIT_CHK(l34_lite_init);

    /* parameter check */
    RT_PARAM_CHK((NULL ==connectInfo), RT_ERR_NULL_POINTER);

    if((ret = l34_get_dir_by_ip(tuple.srcIp,tuple.dstIp,&dir))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }

    if(dir==L34_DIR_UPSTREAM)
    {
        if((ret=l34_us_connection_get(tuple,connectInfo))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
        }
    }else
    {
        if((ret=l34_ds_connection_get(tuple,connectInfo)) !=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
        }
    }

    if((ret=rtk_l34_naptTrfIndicator_get(connectInfo->naptId,&connectInfo->trafficIndicator))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    return RT_ERR_OK;
}   /* end of rtk_l34_connectTrack_get */


/*
 * Function Declaration
 */
/* Function Name:
 *      rtk_l34_connectTrack_del
 * Description:
 *      delete a connection
 * Input:
 *    tuple: five tuple for connection
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
int32
rtk_l34_connectTrack_del(rtk_l34_tuple_t tuple)
{
        int32               ret=0;
    rtk_l34_direct_t        dir;
    rtk_l34_connectInfo_t   connectInfo;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), " ");

    /* check Init status */
    RT_INIT_CHK(l34_lite_init);

    if((ret = l34_get_dir_by_ip(tuple.srcIp,tuple.dstIp,&dir))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }

    if(dir==L34_DIR_UPSTREAM)
    {
        if((ret=l34_us_connection_get(tuple,&connectInfo))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
        }
    }else
    {
        if((ret=l34_ds_connection_get(tuple,&connectInfo)) !=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
        }
    }

    if((ret=l34_connection_delete(connectInfo.natType,connectInfo.naptId,connectInfo.naptrId))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    return RT_ERR_OK;

}   /* end of rtk_l34_connectTrack_del */


/*
 * Function Declaration
 */
/* Function Name:
 *      rtk_l34_globalCfg_get
 * Description:
 *      get l34 global configuration
 * Input:
 *    globalCfg: point of global configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
int32
rtk_l34_globalCfg_get(rtk_l34_global_cfg_t *globalCfg)
{
        int32 ret=0;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "");

    /* check Init status */
    RT_INIT_CHK(l34_lite_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == globalCfg), RT_ERR_NULL_POINTER);

    /* function body */
    if((ret = l34_globalState_get(globalCfg->globalState)) != RT_ERR_OK){
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }

    if((ret = rtk_l34_lookupMode_get(&globalCfg->lookupMode)) != RT_ERR_OK){
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    if((ret = rtk_l34_wanRoutMode_get(&globalCfg->wanRouteMode)) != RT_ERR_OK){
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    if((ret = l34_portMapNetif_get(L34_PORT_MAC,globalCfg->macPortMap)) != RT_ERR_OK){
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    if((ret = l34_portMapNetif_get(L34_PORT_EXTENSION,globalCfg->extPortMap)) != RT_ERR_OK){
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    if((ret = l34_portMapNetif_get(L34_PORT_DSLVC,globalCfg->dslVcPortMap)) != RT_ERR_OK){
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
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
    int32 ret=0;
    uint32 index;
    rtk_ipv6Routing_entry_t ipv6RoutEntry;
    rtk_l34_nexthop_entry_t nexthop;
    rtk_l34_netifType_t  netifType;
    uint32 isDefault = route6Table.prefixLen ? DISABLED : ENABLED;
    rtk_l2_ucastAddr_t   l2Addr;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "route6Type=%d",route6Type);

    /* check Init status */
    RT_INIT_CHK(l34_lite_init);

    /* parameter check */
    RT_PARAM_CHK((L34_IPV6_ROUTE_TYPE_END <=route6Type), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_L34_NETIF_ENTRY_MAX()<=route6Table.netifId), RT_ERR_INPUT);

    /*find netif is exist or not*/
    if(l34_netif_exist(route6Table.netifId)!=ENABLED)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_INPUT;
    }

    /*get unused routing table, prefix length is 0 means default route, choose the last entry*/
    if(isDefault)
    {
        index = HAL_L34_IPV6_ROUTING_ENTRY_MAX()-1;
    }else{
        /*find empty route6 table*/
        if((ret = l34_route6_find(route6Table.ipaddr,route6Table.prefixLen,&index))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
        }
    }
    /*assign nexhop or netifId*/
    switch(route6Type){
    case L34_IPV6_ROUTE_TYPE_TRAP:
    case L34_IPV6_ROUTE_TYPE_DROP:
    case L34_IPV6_ROUTE_TYPE_LOCAL:
    {
        ipv6RoutEntry.nhOrIfidIdx = route6Table.netifId;
    }
    break;
    case L34_IPV6_ROUTE_TYPE_GLOBAL:
    {
        /*nexthop index 0~7 for ipv4, 8~15 for ipv6, last entry for IPv6 default route*/
        if(isDefault)
        {
            ipv6RoutEntry.nhOrIfidIdx = HAL_L34_NH_ENTRY_MAX()-1;
        }else
        {
            ipv6RoutEntry.nhOrIfidIdx = route6Table.netifId + HAL_L34_NETIF_ENTRY_MAX();
        }
        osal_memset(&nexthop,0,sizeof(rtk_l34_nexthop_entry_t));
        /*get netif related nexthop information from ipv4 nexthop*/
        if((ret = rtk_l34_nexthopTable_get(route6Table.netifId,&nexthop))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
        }
        /*search l2table from gateway mac*/
        if((ret = l34_get_l2entry_by_mac(route6Table.netifId,route6Table.gatewayMac,&l2Addr))!=RT_ERR_OK){
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
        }
        nexthop.nhIdx = l2Addr.index;
        nexthop.ifIdx  = route6Table.netifId;
        if((ret = rtk_l34_nexthopTable_set(ipv6RoutEntry.nhOrIfidIdx,&nexthop))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
        }
        /*update l2 table*/
        l2Addr.flags|=RTK_L2_UCAST_FLAG_ARP_USED;

        if((ret=rtk_l2_addr_add(&l2Addr)) != RT_ERR_OK){
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
        }
    }
    break;
    default:
        return RT_ERR_INPUT;
    break;
    }

    if((ret = l34_netif_type_get(route6Table.netifId,&netifType))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }

    /*assign other route6 value*/
    ipv6RoutEntry.ipv6Addr      = route6Table.ipaddr;
    ipv6RoutEntry.ipv6PrefixLen  = route6Table.prefixLen;
    ipv6RoutEntry.type      = route6Type;
    ipv6RoutEntry.valid         = ENABLED;
    ipv6RoutEntry.rt2waninf       = (netifType==L34_NETIF_TYPE_WAN) ?  1 : 0;

    if((ret = rtk_l34_ipv6RoutingTable_set(index,&ipv6RoutEntry))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }

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
rtk_l34_route6_del(rtk_ipv6_addr_t ipaddr, uint32 prefixLen)
{
        int32 ret=0;
    uint32 index;
    rtk_ipv6Routing_entry_t route;

    //RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "ipaddr=%d,prefixLen=%d",ipaddr, prefixLen);

    /* check Init status */
    RT_INIT_CHK(l34_lite_init);

    /* parameter check */
    RT_PARAM_CHK((128 <=prefixLen), RT_ERR_INPUT);

    if(prefixLen == 0)
    {
        index = HAL_L34_IPV6_ROUTING_ENTRY_MAX() -1;
    }else
    {
        /*find route6 table*/
        if((ret = l34_route6_find(ipaddr,prefixLen,&index))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
        }
    }
    /*clear ipv6 entry*/
    osal_memset(&route,0,sizeof(rtk_ipv6Routing_entry_t));
    if((ret = rtk_l34_ipv6RoutingTable_set(index,&route))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }

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
    int32 ret=0;
    rtk_l34_route6Table_t *routeTable = NULL;
    rtk_ipv6Routing_entry_t route;
    rtk_l34_nexthop_entry_t nexthop;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "index=%d",index);

    /* check Init status */
    RT_INIT_CHK(l34_lite_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_L34_IPV6_ROUTING_ENTRY_MAX()<=index), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pRoute6Info), RT_ERR_NULL_POINTER);
    /*get route entry from register*/
    osal_memset(&route,0,sizeof(rtk_ipv6Routing_entry_t));
    if((ret = rtk_l34_ipv6RoutingTable_get(index,&route))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    /*check route valid or not*/
    if(route.valid!=ENABLED)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_ENTRY_NOTFOUND;
    }
    pRoute6Info->valid      = route.valid;
    pRoute6Info->route6Type     = route.type;
    pRoute6Info->routeId    = index;
    routeTable = &pRoute6Info->route6Table;
    /*get netif id*/
    if(route.type==L34_IPV6_ROUTE_TYPE_GLOBAL)
    {
        if((ret = rtk_l34_nexthopTable_get(route.nhOrIfidIdx,&nexthop))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
        }
        /*get l2 table*/
        if((ret = l34_get_mac_by_l2entryId(nexthop.nhIdx, &routeTable->gatewayMac))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
        }
        routeTable->netifId  = nexthop.ifIdx;
        pRoute6Info->nhId = route.nhOrIfidIdx;
        pRoute6Info->l2Id  = nexthop.nhIdx;
    }else
    {
        routeTable->netifId = route.nhOrIfidIdx;
    }
    /*assign other value*/
    routeTable->ipaddr  = route.ipv6Addr;
    routeTable->prefixLen   = route.ipv6PrefixLen;

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
rtk_l34_neigh6_add(rtk_ipv6_addr_t ipaddr,rtk_mac_t mac)
{
    int32 ret=0;
    uint32 neighId;
    uint32 netifId;
    uint32 routeId;
    uint64 ip64;
    rtk_l2_ucastAddr_t l2Addr;
    rtk_ipv6Neighbor_entry_t neigh;


    //RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "ipaddr=%d,mac=%d",ipaddr, mac);

    /* check Init status */
    RT_INIT_CHK(l34_lite_init);

    /*get neighbor information by ipv6 address*/
    if((ret = l34_get_neighInfo_by_ip(ipaddr,&routeId,&netifId,&neighId,&ip64))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    neigh.ipv6RouteIdx= routeId;
    neigh.ipv6Ifid      = ip64;
    /*get l2 index by mac and netif id*/
    if((ret = l34_get_l2entry_by_mac(netifId,mac,&l2Addr))!=RT_ERR_OK){
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    neigh.l2Idx = l2Addr.index;
    neigh.valid = ENABLED;

    /*set to neighbor table*/
    if((ret = rtk_l34_ipv6NeighborTable_set(neighId, &neigh))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }

    /*update l2 table*/
    l2Addr.flags|=RTK_L2_UCAST_FLAG_ARP_USED;

    if((ret=rtk_l2_addr_add(&l2Addr)) != RT_ERR_OK){
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }


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
rtk_l34_neigh6_del(rtk_ipv6_addr_t ipaddr)
{
    int32 ret=0;
    rtk_ipv6Neighbor_entry_t neigh;
    uint32 netifId,neighId;
    uint32 routeId;
    uint64 ip64;
    rtk_l2_ucastAddr_t  lutTable;


    //RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "ipaddr=%d",ipaddr);

    /* check Init status */
    RT_INIT_CHK(l34_lite_init);

    /*get neighbor information*/
    if((ret = l34_get_neighInfo_by_ip(ipaddr,&routeId,&netifId,&neighId,&ip64))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    /*get l2 table for update*/
    osal_memset(&lutTable,0,sizeof(rtk_l2_ucastAddr_t));
    if((ret = rtk_l2_nextValidAddr_get(&neigh.l2Idx, &lutTable))!=RT_ERR_OK){
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    /*update l2 table*/
    lutTable.flags &= ~(RTK_L2_UCAST_FLAG_ARP_USED);
    if((ret=rtk_l2_addr_add(&lutTable)) != RT_ERR_OK){
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    /*del neigh entry*/
    osal_memset(&neigh,0,sizeof(rtk_ipv6Neighbor_entry_t));
    if((ret = rtk_l34_ipv6NeighborTable_set(neighId,&neigh))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }

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
rtk_l34_neigh6_get(rtk_ipv6_addr_t ipaddr,rtk_l34_neigh6Info_t *pNeigh6Info)
{
        int32 ret=0;
    rtk_ipv6Neighbor_entry_t neigh;
    uint32 netifId,neighId;
    uint32 routeId;
    uint64 ip64;

    /* check Init status */
    RT_INIT_CHK(l34_lite_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pNeigh6Info), RT_ERR_NULL_POINTER);

    /*get neighbor information*/
    if((ret = l34_get_neighInfo_by_ip(ipaddr,&routeId,&netifId,&neighId,&ip64))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }

    /*get neighbor information*/
    if((ret = rtk_l34_ipv6NeighborTable_get(neighId,&neigh))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    if(neigh.valid!=ENABLED)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_ENTRY_NOTFOUND;
    }
    /*assign to neighbor information*/
    pNeigh6Info->netifId        = netifId;
    pNeigh6Info->neighborId     = neighId;
    pNeigh6Info->l2Id       = neigh.l2Idx;
    pNeigh6Info->ipaddr         = ipaddr;
    pNeigh6Info->routeId        = neigh.ipv6RouteIdx;
    pNeigh6Info->neighIfId      = neigh.ipv6Ifid;

    /*get l2 mac address*/
    if((ret = l34_get_mac_by_l2entryId(neigh.l2Idx,&pNeigh6Info->mac))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }

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
rtk_l34_netifIp6addr_add(uint32 netifId, rtk_ipv6_addr_t ipaddr,uint32 prefixLen)
{
        int32 ret=0;
    rtk_l34_route6Table_t route;

    //RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "netifId=%d",netifId);

    /* check Init status */
    RT_INIT_CHK(l34_lite_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_L34_NETIF_ENTRY_MAX()<=netifId), RT_ERR_INPUT);
    RT_PARAM_CHK((128 <=prefixLen), RT_ERR_INPUT);

    osal_memset(&route,0,sizeof(rtk_l34_route6Table_t));
    route.ipaddr      = ipaddr;
    route.prefixLen = prefixLen;
    route.netifId     = netifId;

    if((ret = rtk_l34_route6_add(L34_IPV6_ROUTE_TYPE_LOCAL,route))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }

    return RT_ERR_OK;
}   /* end of rtk_l34_netifIp6addr_set */


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
rtk_l34_netifIp6addr_del(uint32 netifId, rtk_ipv6_addr_t ipaddr,uint32 prefixLen)
{
        int32 ret=0;

    //RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "netifId=%d,ipaddr=%d,prefixLen=%d",netifId, ipaddr, prefixLen);

    /* check Init status */
    RT_INIT_CHK(l34_lite_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_L34_NETIF_ENTRY_MAX()<=netifId), RT_ERR_INPUT);
    RT_PARAM_CHK((128 <=prefixLen), RT_ERR_INPUT);


    if((ret = rtk_l34_route6_del(ipaddr,prefixLen))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }

    return RT_ERR_OK;
}   /* end of rtk_l34_netifIp6addr_set */


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
    int32 ret=0;
    rtk_l34_netifType_t     netifType;
    rtk_l34_routing_entry_t route;

    //RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "netifId=%d,gatewayMac=%d",netifId, gatewayMac);

    /* check Init status */
    RT_INIT_CHK(l34_lite_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_L34_NETIF_ENTRY_MAX()<=netifId), RT_ERR_INPUT);

    /*check netif exist*/
    if(l34_netif_exist(netifId)!=ENABLED)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
    }
    /*get routing table for check netif is wan type, else reject (only wan interface can change to pppoe)*/
    if((ret = l34_netif_type_get(netifId,&netifType))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
    }
    if(netifType != L34_NETIF_TYPE_WAN)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
    }

    /*change local route to global route*/
    osal_memset(&route,0,sizeof(rtk_l34_routing_entry_t));
    if((ret = rtk_l34_routingTable_get(netifId,&route))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
    }

    if(!is_empty_mac_check(gatewayMac)){
        /*change to global route*/
        /*get and set to next-hop for  gateway add*/
        if((ret = l34_nextHopMac_set(netifId,gatewayMac))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
                return ret;
        }
        l34_default_globalRoute_set(netifId,&route);
        if((ret = rtk_l34_routingTable_set(netifId,&route))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
                return ret;
        }
    }
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
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_L34), "netifId=%d,netifInfo=%d",netifId, pNetifInfo->id);

    /* check Init status */
    RT_INIT_CHK(l34_lite_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_L34_NETIF_ENTRY_MAX()<=netifId), RT_ERR_INPUT);

    //RT_PARAM_CHK((RTK_XXX <=netifInfo), RT_ERR_INPUT);

    /* function body */

    return RT_ERR_OK;
}   /* end of rtk_l34_netif_set */


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
int32
rtk_l34_netifMac_set(uint32 netifId, rtk_mac_t hwAddr)
{
    int32   ret =0;
    rtk_l34_netif_entry_t entry;
    /* check Init status */
    RT_INIT_CHK(l34_lite_init);
    /* function body */
    RT_PARAM_CHK((HAL_L34_NETIF_ENTRY_MAX()<=netifId), RT_ERR_INPUT);

    /*check netif exist*/
    if(l34_netif_exist(netifId)!=ENABLED)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
    }
    osal_memset(&entry,0,sizeof(rtk_l34_netif_entry_t));
    if((ret = rtk_l34_netifTable_get(netifId,&entry))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
    }

    entry.gateway_mac = hwAddr;

    if((ret = rtk_l34_netifTable_set(netifId,&entry))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
    }
    return RT_ERR_OK;
}   /* end of rtk_l34_netifMac_set */

