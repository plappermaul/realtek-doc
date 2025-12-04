/*
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 *
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Virual Device for Event handler
 *
 * Feature : Provide the Event Handler for control HW L34 Table
 *
 */

#include <net/netfilter/nf_conntrack.h>
#include <net/netfilter/nf_conntrack_core.h>
#include <module/netvd/netvd.h>
#include <rtk/l34lite.h>
#include <rtk/vlan.h>
#include <rtk/switch.h>
#include <net/if_inet6.h>
#include <net/addrconf.h>


static int32
l34lite_init(void)
{
	rtk_l34_lite_init();
	return VD_ERR_OK;
}


static int32
l34lite_netif_create(netvd_netifType_t type,uint32 netifId,unsigned char *mac)
{
	int32 ret;
	rtk_mac_t gmac;
	rtk_l34_netifType_t localtype;

	memcpy(&gmac.octet[0],&mac[0],ETHER_ADDR_LEN);

	localtype = (type==NETVD_NETIF_WAN) ?  L34_NETIF_TYPE_WAN : L34_NETIF_TYPE_LAN;

	if((ret = rtk_l34_netif_create(netifId,localtype,gmac))!=RT_ERR_OK)
	{
		VD_DEBUG(NETVD_LOG_LEVEL_LOW,"%s(), ret=%d\n",__FUNCTION__,ret);
		return  VD_ERR_ADD_FAILED;
	}

	VD_DEBUG(NETVD_LOG_LEVEL_LOW,"IFID:%d, %02X:%02X:%02X:%02X:%02X:%02X, ret=%d\n",netifId,
	gmac.octet[0],gmac.octet[1],gmac.octet[2],gmac.octet[3],gmac.octet[4],gmac.octet[5],ret);

	return VD_ERR_OK;

}

static int32
l34lite_netif_delete(uint32 netifId)
{
	int ret;

	if((ret = rtk_l34_netif_del(netifId))!=RT_ERR_OK)
	{
		VD_DEBUG(NETVD_LOG_LEVEL_LOW,"%s(), ret=%d\n",__FUNCTION__,ret);
		return  VD_ERR_ADD_FAILED;
	}
	return VD_ERR_OK;
}

static int32
l34lite_netif_ppp_create(uint32 netifId, int32 sessionId, unsigned char *gatewayMac)
{

	int ret;
	rtk_mac_t remote;

	if((ret = rtk_l34_netifPPPoE_set(netifId, ENABLED, sessionId, DISABLED))!=RT_ERR_OK)
	{
		VD_DEBUG(NETVD_LOG_LEVEL_LOW,"%s(), ret=%d\n",__FUNCTION__,ret);
		return  VD_ERR_ADD_FAILED;
	}

	memcpy(&remote.octet[0],&gatewayMac[0],ETHER_ADDR_LEN);

	if((ret= rtk_l34_netifGateway_set(netifId,remote))!=RT_ERR_OK)
	{
		VD_DEBUG(NETVD_LOG_LEVEL_LOW,"%s(), ret=%d\n",__FUNCTION__,ret);
		return  VD_ERR_ADD_FAILED;
	}
	return VD_ERR_OK;

}


static int32
l34lite_netif_mtu_set(uint32 netifId, uint32 mtu)
{
	int32 ret;
	if((ret = rtk_l34_netifMtu_set(netifId,mtu))!=RT_ERR_OK)
	{
		VD_DEBUG(NETVD_LOG_LEVEL_LOW,"%s(), ret=%d\n",__FUNCTION__,ret);
		return  VD_ERR_ADD_FAILED;
	}
	return VD_ERR_OK;
}

static int32
l34lite_netif_mac_set(uint32 netifId, unsigned char * mac)
{
	int32 ret;
	rtk_mac_t hwAddr;
	memcpy(&hwAddr.octet[0],&mac[0],ETHER_ADDR_LEN);
	if((ret = rtk_l34_netifMac_set(netifId,hwAddr))!=RT_ERR_OK)
	{
		VD_DEBUG(NETVD_LOG_LEVEL_LOW,"%s(), ret=%d\n",__FUNCTION__,ret);
		return  VD_ERR_ADD_FAILED;
	}

	return VD_ERR_OK;
}

/*for IPv4 Part*/
static int32
l34lite_netif_ip_set(uint32 netifId, uint32 ipaddr, uint32 prefix,char *gatewayMac)
{
	int32 ret;
	rtk_mac_t mac;
	
	prefix = (prefix > 0) ? (prefix-1) : 0;
	if((ret = rtk_l34_netifIpaddr_set(netifId, (rtk_ip_addr_t)ipaddr, prefix))!=RT_ERR_OK)
	{
		VD_DEBUG(NETVD_LOG_LEVEL_LOW,"%s(), ret=%d\n",__FUNCTION__,ret);
		return  VD_ERR_ADD_FAILED;
	}

	if(gatewayMac)
	{
		memcpy(&mac.octet[0],&gatewayMac[0],ETHER_ADDR_LEN);
		if((ret = rtk_l34_netifGateway_set(netifId,mac))!=RT_ERR_OK)
		{
			VD_DEBUG(NETVD_LOG_LEVEL_LOW,"%s(), ret=%d\n",__FUNCTION__,ret);
			return  VD_ERR_ADD_FAILED;
		}
	}

	return VD_ERR_OK;
}


/*for IPv4 Part*/
static int32
l34lite_netif_vid_set(uint32 netifId, uint32 vid,uint32 pbit)
{
	int32 ret;
	rtk_fid_t fid;
	
	if((ret = rtk_l34_netifVlan_set(netifId,vid,pbit))!=RT_ERR_OK)
	{
		VD_DEBUG(NETVD_LOG_LEVEL_LOW,"%s(), ret=%d\n",__FUNCTION__,ret);
		return  VD_ERR_ADD_FAILED;
	}

	/*also create vlan table*/
	if((ret = rtk_vlan_fid_get(vid,&fid))!=RT_ERR_OK)
	{
		if((ret = rtk_vlan_create(vid))!=RT_ERR_OK)
		{
			VD_DEBUG(NETVD_LOG_LEVEL_LOW,"%s(), ret=%d\n",__FUNCTION__,ret);
			return  VD_ERR_ADD_FAILED;
		}
	}

	return VD_ERR_OK;
}


/*for IPv4 Part*/
static int32
l34lite_netif_ipv4Addr_get(uint32 netifId, uint32 *pIpaddr, uint32 *pMask)
{
	int32 ret;
	rtk_l34_netifInfo_t netifInfo;

	memset(&netifInfo,0,sizeof(rtk_l34_netifInfo_t));
	
	if((ret = rtk_l34_netif_get(netifId,&netifInfo))!=RT_ERR_OK)
	{
		VD_DEBUG(NETVD_LOG_LEVEL_LOW,"%s(), ret=%d\n",__FUNCTION__,ret);
		return  VD_ERR_ADD_FAILED;
	}

	*pIpaddr = netifInfo.ipInfo.ipaddr;
	*pMask   = netifInfo.ipInfo.ipmask;

	return VD_ERR_OK;
}



static int32
l34lite_ct_del(struct nf_conn *ct)
{
	rtk_l34_tuple_t tuple;
	rtk_ip_addr_t    natIp;
	uint32 		  natport;
	rtk_l34_direct_t dir;
	rtk_l34_connectInfo_t info;
	int ret;

	tuple.proto 	= ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.dst.protonum;

	/*start to assing needed parameters*/
	dir = L34_DIR_UPSTREAM;
	tuple.srcIp 	= ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.src.u3.ip;
	tuple.srcport   = ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.src.u.tcp.port;
	tuple.dstIp 	= ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.dst.u3.ip;
	tuple.dstport 	= ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.dst.u.tcp.port;

	natIp 		= ct->tuplehash[IP_CT_DIR_REPLY].tuple.dst.u3.ip;
	natport		= ct->tuplehash[IP_CT_DIR_REPLY].tuple.dst.u.tcp.port;

	VD_DEBUG(NETVD_LOG_LEVEL_HIGH,"srcIp=%u.%u.%u.%u,srcPort = %d\n",NIPQUAD(tuple.srcIp),tuple.srcport);
	VD_DEBUG(NETVD_LOG_LEVEL_HIGH,"dstIp=%u.%u.%u.%u,dstPort = %d\n",NIPQUAD(tuple.dstIp),tuple.dstport);
	VD_DEBUG(NETVD_LOG_LEVEL_HIGH,"natIp=%u.%u.%u.%u,natPort = %d\n",NIPQUAD(natIp),natport);
	VD_DEBUG(NETVD_LOG_LEVEL_HIGH,"proto=%d\n",tuple.proto);
	VD_DEBUG(NETVD_LOG_LEVEL_HIGH,"otherIp=%u.%u.%u.%u,otherPort=%d\n",NIPQUAD(ct->tuplehash[IP_CT_DIR_REPLY].tuple.src.u3.ip),
	ct->tuplehash[IP_CT_DIR_REPLY].tuple.src.u.tcp.port);

	/*check conneciton is exist*/
	if((ret = rtk_l34_connectTrack_get(tuple,&info))!=RT_ERR_OK)
	{
		VD_DEBUG(NETVD_LOG_LEVEL_HIGH,"%s(), ret=%d\n",__FUNCTION__,ret);
		return  VD_ERR_ADD_FAILED;
	}

	if((ret = rtk_l34_connectTrack_del(tuple))!=RT_ERR_OK)
	{
		VD_DEBUG(NETVD_LOG_LEVEL_HIGH,"%s(), ret=%d\n",__FUNCTION__,ret);
		return  VD_ERR_ADD_FAILED;
	}

	VD_DEBUG(NETVD_LOG_LEVEL_HIGH,"delete ct ret=%d\n",ret);

	return VD_ERR_OK;
}


static int32
l34lite_ct_add(struct nf_conn *ct, uint32 *naptId,int l34dir)
{
	rtk_l34_tuple_t tuple;
    rtk_l34_nat_info_t natInfo;
	rtk_l34_direct_t dir;
	rtk_l34_connectInfo_t info;
	int32 ret;

	tuple.proto 	= ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.dst.protonum;
	/*check protocol number should tcp or udp*/
	if(tuple.proto  != IPPROTO_TCP && tuple.proto != IPPROTO_UDP)
		return RT_ERR_OK;

	
	memset(&natInfo,0,sizeof(rtk_l34_nat_info_t));
	/*start to assing needed parameters*/
	if(l34dir==CT_DIR_UPSTREAM)
	{
		dir = L34_DIR_UPSTREAM;
		tuple.srcIp 	= ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.src.u3.ip;
		tuple.srcport   = ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.src.u.tcp.port;
		tuple.dstIp 	= ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.dst.u3.ip;
		tuple.dstport 	= ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.dst.u.tcp.port;

		natInfo.natIp 		= ct->tuplehash[IP_CT_DIR_REPLY].tuple.dst.u3.ip;
		natInfo.natPort		= ct->tuplehash[IP_CT_DIR_REPLY].tuple.dst.u.tcp.port;

		VD_DEBUG(NETVD_LOG_LEVEL_HIGH,"srcIp=%u.%u.%u.%u,srcPort = %d\n",NIPQUAD(tuple.srcIp),tuple.srcport);
		VD_DEBUG(NETVD_LOG_LEVEL_HIGH,"dstIp=%u.%u.%u.%u,dstPort = %d\n",NIPQUAD(tuple.dstIp),tuple.dstport);
		VD_DEBUG(NETVD_LOG_LEVEL_HIGH,"natIp=%u.%u.%u.%u,natPort = %d\n",NIPQUAD(natInfo.natIp),natInfo.natPort);
		VD_DEBUG(NETVD_LOG_LEVEL_HIGH,"proto=%d\n",tuple.proto);
		VD_DEBUG(NETVD_LOG_LEVEL_HIGH,"otherIp=%u.%u.%u.%u,otherPort=%d\n",NIPQUAD(ct->tuplehash[IP_CT_DIR_REPLY].tuple.src.u3.ip),
		ct->tuplehash[IP_CT_DIR_REPLY].tuple.src.u.tcp.port);
	}else
	{
		dir = L34_DIR_DOWNSTREAM;
		tuple.srcIp 	= ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.src.u3.ip;
		tuple.srcport   = ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.src.u.tcp.port;
		tuple.dstIp 	= ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.dst.u3.ip;
		tuple.dstport 	= ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.dst.u.tcp.port;

		natInfo.natIp  		= ct->tuplehash[IP_CT_DIR_REPLY].tuple.src.u3.ip;
		natInfo.natPort		= ct->tuplehash[IP_CT_DIR_REPLY].tuple.src.u.tcp.port;

		VD_DEBUG(NETVD_LOG_LEVEL_HIGH,"srcIp=%u.%u.%u.%u,srcPort = %d\n",NIPQUAD(tuple.srcIp),tuple.srcport);
		VD_DEBUG(NETVD_LOG_LEVEL_HIGH,"dstIp=%u.%u.%u.%u,dstPort = %d\n",NIPQUAD(tuple.dstIp),tuple.dstport);
		VD_DEBUG(NETVD_LOG_LEVEL_HIGH,"natIp=%u.%u.%u.%u,natPort = %d\n",NIPQUAD(natInfo.natIp),natInfo.natPort);
		VD_DEBUG(NETVD_LOG_LEVEL_HIGH,"proto=%d\n",tuple.proto);
		VD_DEBUG(NETVD_LOG_LEVEL_HIGH,"otherIp=%u.%u.%u.%u,otherPort=%d\n",NIPQUAD(ct->tuplehash[IP_CT_DIR_REPLY].tuple.dst.u3.ip),
		ct->tuplehash[IP_CT_DIR_REPLY].tuple.dst.u.tcp.port);
	}

	/*check conneciton is exist*/
	if((ret = rtk_l34_connectTrack_get(tuple,&info))==RT_ERR_OK)
	{
		VD_DEBUG(NETVD_LOG_LEVEL_HIGH,"connection is exist!,index=%d,%d",info.naptId,info.naptrId);
		return  VD_ERR_ADD_FAILED;
	}
 	if((ret = rtk_l34_connectTrack_add(dir,tuple, natInfo))!=RT_ERR_OK)
 	{
 		VD_DEBUG(NETVD_LOG_LEVEL_HIGH,"%s(), ret=%d\n",__FUNCTION__,ret);
		return  VD_ERR_ADD_FAILED;
 	}

	if((ret = rtk_l34_connectTrack_get(tuple,&info))!=RT_ERR_OK)
	{
		VD_DEBUG(NETVD_LOG_LEVEL_HIGH,"%s(), ret=%d\n",__FUNCTION__,ret);
		return  VD_ERR_ADD_FAILED;
	}
	*naptId = info.naptId;

	return VD_ERR_OK;
}

static int32
l34lite_ct_tf_get_all(unsigned int *tfValue)
{
	int32 ret;
	if((ret = rtk_l34_naptTrfIndicator_get(CT_ENTRY_MAX+1,(rtk_enable_t*)tfValue))!=RT_ERR_OK)
	{
		VD_DEBUG(NETVD_LOG_LEVEL_HIGH,"%s(), ret=%d\n",__FUNCTION__,ret);
		return  VD_ERR_ADD_FAILED;
	}

	return VD_ERR_OK;
}


static int32
l34lite_arp_add(uint32 ipaddr, unsigned char *mac)
{
	rtk_mac_t smac;
	int32 ret;

	memcpy(&smac.octet[0],&mac[0],ETHER_ADDR_LEN);

	if((ret = rtk_l34_arp_add(ipaddr,smac))!=RT_ERR_OK)
	{
		VD_DEBUG(NETVD_LOG_LEVEL_LOW,"%s(), ret=%d\n",__FUNCTION__,ret);
		return  VD_ERR_ADD_FAILED;
	}

	VD_DEBUG(NETVD_LOG_LEVEL_LOW,"Add ARP Entry (SIP,SMAC) = { %u.%u.%u.%u, %02X:%02X:%02X:%02X:%02X:%02X}, ret=%d\n",
	NIPQUAD(ipaddr),smac.octet[0],smac.octet[1],smac.octet[2],smac.octet[3],smac.octet[4],smac.octet[5],ret);

	return VD_ERR_OK;
}

static int32
l34lite_arp_del(uint32 ipaddr)
{
	int32 ret;

	if((ret = rtk_l34_arp_del(ipaddr))!=RT_ERR_OK)
	{
		VD_DEBUG(NETVD_LOG_LEVEL_LOW,"%s(), ret=%d\n",__FUNCTION__,ret);
		return  VD_ERR_ADD_FAILED;
	}
	VD_DEBUG(NETVD_LOG_LEVEL_LOW,"Delete ARP Entry SIP = { %u.%u.%u.%u}, ret=%d\n",
	NIPQUAD(ipaddr),ret);

	return VD_ERR_OK;
}

static int32
l34lite_arp_get(uint32 ipaddr,unsigned char *mac)
{
	int32 ret;
	rtk_l34_arpInfo_t arpInfo;

	if((ret = rtk_l34_arp_get(ipaddr,&arpInfo))!=RT_ERR_OK)
	{
		VD_DEBUG(NETVD_LOG_LEVEL_LOW,"%s(), ret=%d\n",__FUNCTION__,ret);
		return  VD_ERR_ADD_FAILED;
	}
	memcpy(&mac[0],&arpInfo.mac.octet[0],ETHER_ADDR_LEN);
	return VD_ERR_OK;
}

static int32
l34lite_route_add(int32 netifId, uint32 ipaddr, uint32 prefix,unsigned char *gatewayMac)
{
	int32 ret = 0;
	rtk_l34_routeType_t type;
	rtk_l34_routeTable_t routeTable;

	if(ipaddr==0 && prefix ==0){
		type = L34_ROUTE_DEFAULT;
	}else
	if(gatewayMac == NULL)
	{
		type = L34_ROUTE_LOCAL;
	}
	else
	{
		type = L34_ROUTE_STATIC;
	}

	memset(&routeTable,0,sizeof(rtk_l34_routeType_t));
	routeTable.ipaddr = ipaddr;
	routeTable.ipmask = prefix;	
	routeTable.netifId = netifId;
	
	if(gatewayMac != NULL)
	{
		memcpy(&routeTable.gatewayMac.octet[0],&gatewayMac[0],ETHER_ADDR_LEN);
	}

	if((ret = rtk_l34_route_add(type,routeTable))!=RT_ERR_OK)
	{
		VD_DEBUG(NETVD_LOG_LEVEL_LOW,"%s(), ret=%d\n",__FUNCTION__,ret);
		return  VD_ERR_ADD_FAILED;
	}

	return VD_ERR_OK;
}

static int32
l34lite_route_del(uint32 ipaddr, uint32 prefix)
{
	int32 ret;
	if((ret = rtk_l34_route_del(ipaddr,prefix))!=RT_ERR_OK)
	{
		VD_DEBUG(NETVD_LOG_LEVEL_LOW,"%s(), ret=%d\n",__FUNCTION__,ret);
		return  VD_ERR_ADD_FAILED;
	}
	return VD_ERR_OK;
}


static int32
l34lite_netif_gw_set(int32 netif,unsigned char *gwMac)
{
	int ret;
	rtk_mac_t mac;

	memcpy(&mac.octet[0],&gwMac[0],ETHER_ADDR_LEN);
	if((ret = rtk_l34_netifGateway_set(netif,mac))!=RT_ERR_OK)
	{
		VD_DEBUG(NETVD_LOG_LEVEL_LOW,"%s(), ret=%d\n",__FUNCTION__,ret);
		return  VD_ERR_ADD_FAILED;
	}
	return VD_ERR_OK;
}


/*for IPv6 Part*/
static int32
l34lite_neigh_add(struct in6_addr ipaddr, unsigned char *mac)
{
	rtk_mac_t smac;
	int32 ret;
	rtk_ipv6_addr_t ip6;

	memset(&smac,0,sizeof(rtk_mac_t));
	memset(&ip6,0,sizeof(rtk_ipv6_addr_t));
	memcpy(&smac.octet[0],&mac[0],ETHER_ADDR_LEN);
	memcpy(&ip6.ipv6_addr[0],&ipaddr.in6_u.u6_addr8[0],IPV6_ADDR_LEN);

	if((ret = rtk_l34_neigh6_add(ip6,smac))!=RT_ERR_OK)
	{
		VD_DEBUG(NETVD_LOG_LEVEL_LOW,"%s(), ret=%d\n",__FUNCTION__,ret);
		return  VD_ERR_ADD_FAILED;
	}

	VD_DEBUG(NETVD_LOG_LEVEL_LOW,"Add Neighbor Entry SMAC = %02X:%02X:%02X:%02X:%02X:%02X, ret=%d\n",
	smac.octet[0],smac.octet[1],smac.octet[2],smac.octet[3],smac.octet[4],smac.octet[5],ret);
	VD_DEBUG(NETVD_LOG_LEVEL_LOW,NIP6QUAD_FMT,NIP6QUAD(ipaddr));

	return VD_ERR_OK;
}

static int32
l34lite_neigh_del(struct in6_addr ipaddr)
{
	int32 ret;
	rtk_ipv6_addr_t ip6;

	memset(&ip6,0,sizeof(rtk_ipv6_addr_t));
	memcpy(&ip6.ipv6_addr[0],&ipaddr.in6_u.u6_addr8[0],IPV6_ADDR_LEN);

	if((ret = rtk_l34_neigh6_del(ip6))!=RT_ERR_OK)
	{
		VD_DEBUG(NETVD_LOG_LEVEL_LOW,"%s(), ret=%d\n",__FUNCTION__,ret);
		return  VD_ERR_ADD_FAILED;
	}
	VD_DEBUG(NETVD_LOG_LEVEL_LOW,NIP6QUAD_FMT,NIP6QUAD(ipaddr));
	VD_DEBUG(NETVD_LOG_LEVEL_LOW,"Result=%d",ret);

	return VD_ERR_OK;
}

static int32
l34lite_netif_ip6_set(uint32 netifId, struct in6_addr ipaddr, uint32 prefix)
{
	int32 ret;
	rtk_ipv6_addr_t ip6;

	memset(&ip6,0,sizeof(rtk_ipv6_addr_t));
	memcpy(&ip6.ipv6_addr[0],&ipaddr.in6_u.u6_addr8[0],IPV6_ADDR_LEN);

	if((ret = rtk_l34_netifIp6addr_add(netifId,ip6, prefix))!=RT_ERR_OK)
	{
		VD_DEBUG(NETVD_LOG_LEVEL_LOW,"%s(), ret=%d\n",__FUNCTION__,ret);
		return  VD_ERR_ADD_FAILED;
	}

	return VD_ERR_OK;
}


static int32
l34lite_route6_add(int32 netifId, struct in6_addr ipaddr, uint32 prefix,unsigned char *gatewayMac)
{
	int32 ret;
	rtk_l34_ipv6RouteType_t type;
	rtk_l34_route6Table_t routeTable;

	if( prefix ==0){
		type = L34_IPV6_ROUTE_TYPE_GLOBAL;
	}

	memcpy(&routeTable.ipaddr.ipv6_addr[0],&ipaddr.in6_u.u6_addr8[0],IPV6_ADDR_LEN);
	routeTable.prefixLen= prefix;
	memcpy(&routeTable.gatewayMac.octet[0],&gatewayMac[0],ETHER_ADDR_LEN);
	routeTable.netifId = netifId;

	if((ret = rtk_l34_route6_add(type,routeTable))!=RT_ERR_OK)
	{
		VD_DEBUG(NETVD_LOG_LEVEL_LOW,"%s(), ret=%d\n",__FUNCTION__,ret);
		return  VD_ERR_ADD_FAILED;
	}

	return VD_ERR_OK;
}

static int32
l34lite_route6_del(struct in6_addr ipaddr, uint32 prefix)
{
	int32 ret;
	rtk_ipv6_addr_t ip6;
	memcpy(&ip6.ipv6_addr[0],&ipaddr.in6_u.u6_addr8[0],IPV6_ADDR_LEN);
	if((ret = rtk_l34_route6_del(ip6,prefix))!=RT_ERR_OK)
	{
		VD_DEBUG(NETVD_LOG_LEVEL_LOW,"%s(), ret=%d\n",__FUNCTION__,ret);
		return  VD_ERR_ADD_FAILED;
	}
	return VD_ERR_OK;
}


static int32
l34lite_neigh_get(struct in6_addr ipaddr,unsigned char *mac)
{
	int32 ret;
	rtk_ipv6_addr_t ip6;
	rtk_l34_neigh6Info_t info;

	memset(&ip6,0,sizeof(rtk_ipv6_addr_t));
	memcpy(&ip6.ipv6_addr[0],&ipaddr.in6_u.u6_addr8[0],IPV6_ADDR_LEN);

	if((ret = rtk_l34_neigh6_get(ip6,&info))!=RT_ERR_OK)
	{
		VD_DEBUG(NETVD_LOG_LEVEL_LOW,"%s(), ret=%d\n",__FUNCTION__,ret);
		return  VD_ERR_ADD_FAILED;
	}
	memcpy(&mac[0],&info.mac.octet[0],ETHER_ADDR_LEN);
	VD_DEBUG(NETVD_LOG_LEVEL_LOW,NIP6QUAD_FMT,NIP6QUAD(ipaddr));
	VD_DEBUG(NETVD_LOG_LEVEL_LOW,"MAC, %02X:%02X:%02X:%02X:%02X:%02X\n",
	mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);

	return VD_ERR_OK;
}

static int32
l34lite_ponPort_get(int *ponPort)
{
	int32 ret;

	if((ret = rtk_switch_phyPortId_get(RTK_PORT_PON,ponPort))!=RT_ERR_OK)
	{
		return VD_ERR_FAILED;
	}

	return VD_ERR_OK;
}


/*define default mapper*/
static netvd_hal_mapper_t mapper_l34lite=  {
	.hal_hwnat_init			=l34lite_init,
	.hal_netif_create		=l34lite_netif_create,
	.hal_netif_delete		=l34lite_netif_delete,
	.hal_netif_ppp_create	=l34lite_netif_ppp_create,
    .hal_netif_mtu_set		=l34lite_netif_mtu_set,
	.hal_netif_ip_set		=l34lite_netif_ip_set,
	.hal_netif_mac_set		=l34lite_netif_mac_set,
	.hal_netif_gw_set		=l34lite_netif_gw_set,
	.hal_netif_vid_set		=l34lite_netif_vid_set,
	.hal_ct_del			=l34lite_ct_del,
	.hal_ct_add			=l34lite_ct_add,
	.hal_ct_tf_get_all		=l34lite_ct_tf_get_all,
	.hal_arp_add			=l34lite_arp_add,
	.hal_arp_del			=l34lite_arp_del,
	.hal_arp_get			=l34lite_arp_get,
	.hal_route_add 		=l34lite_route_add,
	.hal_route_del 		=l34lite_route_del,
	.hal_neigh_add		=l34lite_neigh_add,
	.hal_neigh_del		=l34lite_neigh_del,
	.hal_neigh_get		=l34lite_neigh_get,
	.hal_netif_ip6_set		=l34lite_netif_ip6_set,
	.hal_route6_add		=l34lite_route6_add,
	.hal_route6_del		=l34lite_route6_del,
	.hal_ponPort_get		=l34lite_ponPort_get,
	.hal_netif_ip4addr_get =l34lite_netif_ipv4Addr_get
};

int netvd_default_map_init(void){

	netvd_event_map_register(&mapper_l34lite);
	VD_DEBUG(NETVD_LOG_LEVEL_LOW,"L34 Mapper is attached!\n");
	return 0;
}

