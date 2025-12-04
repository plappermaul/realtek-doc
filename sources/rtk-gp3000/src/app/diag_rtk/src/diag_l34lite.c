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
 * Purpose : Definition those XXX command and APIs in the SDK diagnostic shell.
 *
 * Feature : The file have include the following module and sub-modules
 *
 */

/*
 * Include Files
 */
#include <stdio.h>
#include <string.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <diag_util.h>
#include <parser/cparser_priv.h>
#include <diag_str.h>
#include <rtk/l34lite.h>
#include <hal/common/halctrl.h>

static void
stateToString(rtk_enable_t state,char *str){
	char tmpStr[128]="ENABLED";
	memset(str,0,sizeof(tmpStr));
	if(str!=NULL){
		if(state != ENABLED){
			strncpy(tmpStr,"DISABLED",sizeof("DISABLED"));
		}
		memcpy(str,(char*)tmpStr,sizeof(tmpStr));
	}
}

static void
netIfTypeToStr(rtk_l34_netifType_t netifType,char *str){

	char tmpStr[128]="LAN";
	memset(str,0,sizeof(tmpStr));

	if(str!=NULL){
		if(netifType !=L34_NETIF_TYPE_LAN){
			strncpy(tmpStr,"WAN",sizeof("WAN"));
		}
		memcpy(str,(char*)tmpStr,sizeof(tmpStr));
	}
}

static void
routeTypeToString(rtk_l34_routeType_t routType, char *str)
{
	char tmpStr[128]="";
	memset(str,0,sizeof(tmpStr));

	if(str!=NULL){
		switch(routType){
		case L34_ROUTE_CPU:
			strncpy(tmpStr,"TRAP",sizeof("TRAP"));
		break;
		case L34_ROUTE_DROP:
			strncpy(tmpStr,"DROP",sizeof("DROP"));
		break;
		case L34_ROUTE_LOCAL:
			strncpy(tmpStr,"LOCAL",sizeof("LOCAL"));
		break;
		case L34_ROUTE_STATIC:
			strncpy(tmpStr,"STATIC",sizeof("STATIC"));
		break;
		case L34_ROUTE_DEFAULT:
			strncpy(tmpStr,"DEFAULT",sizeof("DEFAULT"));
		break;
		case L34_ROUTE_POLICY:
			strncpy(tmpStr,"POLICY",sizeof("POLICY"));
		break;
		case L34_ROUTE_PPP:
			strncpy(tmpStr,"PPPoE",sizeof("PPPoE"));
		break;
		default:
		break;
		}

		memcpy(str,(char*)tmpStr,sizeof(tmpStr));
	}
}


static void
route6TypeToString(rtk_l34_ipv6RouteType_t routType, char *str)
{
	char tmpStr[128]="";
	memset(str,0,sizeof(tmpStr));

	if(str!=NULL){
		switch(routType){
		case L34_IPV6_ROUTE_TYPE_TRAP:
			strncpy(tmpStr,"TRAP",sizeof("TRAP"));
		break;
		case L34_IPV6_ROUTE_TYPE_DROP:
			strncpy(tmpStr,"DROP",sizeof("DROP"));
		break;
		case L34_IPV6_ROUTE_TYPE_LOCAL:
			strncpy(tmpStr,"LOCAL",sizeof("LOCAL"));
		break;
		case L34_IPV6_ROUTE_TYPE_GLOBAL:
			strncpy(tmpStr,"GLOBAL",sizeof("GLOBAL"));
		break;
		default:
		break;
		}

		memcpy(str,(char*)tmpStr,sizeof(tmpStr));
	}
}


static void
natTypeToString(rtk_l34_natType_t natType, char *str){
	char tmpStr[128]="";
	memset(str,0,sizeof(tmpStr));

	if(str!=NULL){
		switch(natType){
		case L34_NAT:
			strncpy(tmpStr,"nat",sizeof("nat"));
		break;
		case L34_NAPT_FULL_CONE:
			strncpy(tmpStr,"full cone",sizeof("full cone"));
		break;
		case L34_NAPT_PORT_RESTRICTED:
			strncpy(tmpStr,"port restricted cone",sizeof("port restricted cone"));
		break;
		case L34_NAPT_RESTRICTED_CONE:
			strncpy(tmpStr,"restricted cone",sizeof("restricted cone"));
		break;
		case L34_LOCAL_PUBLIC:
			strncpy(tmpStr,"local public",sizeof("local public"));
		break;
		default:
		break;
		}

		memcpy(str,(char*)tmpStr,sizeof(tmpStr));
	}
}


static void
dirToString(rtk_l34_direct_t dir,char *str){
	char tmpStr[128]="";
	memset(str,0,sizeof(tmpStr));

	if(str!=NULL){
		switch(dir){
		case L34_DIR_UPSTREAM:
			strncpy(tmpStr,"upstream",sizeof("upstream"));
		break;
		case L34_DIR_DOWNSTREAM:
			strncpy(tmpStr,"downstream",sizeof("downstream"));
		break;
		default:
		break;
		}

		memcpy(str,(char*)tmpStr,sizeof(tmpStr));
	}
}

static void
protoToString(uint32 proto,char *str){
	char tmpStr[128]="";
	memset(str,0,sizeof(tmpStr));

	if(str!=NULL){
		switch(proto){
		case L34_PROTO_TCP:
			strncpy(tmpStr,"tcp",sizeof("tcp"));
		break;
		case L34_PROTO_UDP:
			strncpy(tmpStr,"udp",sizeof("udp"));
		break;
		default:
		break;
		}

		memcpy(str,(char*)tmpStr,sizeof(tmpStr));
	}
}


static void
lookupModeToStr(rtk_l34_lookupMode_t mode,char *str){
	char tmpStr[128]="";
	memset(str,0,sizeof(tmpStr));

	if(str!=NULL){
		switch(mode){
		case L34_LOOKUP_VLAN_BASE:
			strncpy(tmpStr,"vlan-base",sizeof("vlan-base"));
		break;
		case L34_LOOKUP_MAC_BASE:
			strncpy(tmpStr,"mac-base",sizeof("mac-base"));
		break;
		case L34_LOOKUP_PORT_BASE:
			strncpy(tmpStr,"port-base",sizeof("port-base"));
		default:
		break;
		}

		memcpy(str,(char*)tmpStr,sizeof(tmpStr));
	}
}


static void
wanRouteModeToStr(rtk_l34_wanRouteMode_t mode,char *str){
	char tmpStr[128]="";
	memset(str,0,sizeof(tmpStr));

	if(str!=NULL){
		switch(mode){
		case L34_WANROUTE_FORWARD:
			strncpy(tmpStr,"forward",sizeof("forward"));
		break;
		case L34_WANROUTE_FORWARD2CPU:
			strncpy(tmpStr,"forward-to-cpu",sizeof("forward-to-cpu"));
		break;
		case L34_WANROUTE_DROP:
			strncpy(tmpStr,"drop",sizeof("drop"));
		default:
		break;
		}

		memcpy(str,(char*)tmpStr,sizeof(tmpStr));
	}
}


static void
l34_print_head(char *str){

	diag_util_mprintf("*************************************************\n");
	diag_util_mprintf("*\t%s\t\t*\n",str);
	diag_util_mprintf("*************************************************\n");
}



static void
diag_l34_ifconfig(uint32 netifId,rtk_enable_t force){

	uint32 ret,i;
	rtk_l34_netifInfo_t netifInfo;
	char tmpStr[128]="";

	osal_memset(&netifInfo,0,sizeof(rtk_l34_netifInfo_t));
	if((ret = rtk_l34_netif_get(netifId,&netifInfo)) != RT_ERR_OK){
		return ;
	}

	if(netifInfo.l2NetIfInfo.ifState != ENABLED && !force){
		return;
	}
	sprintf(tmpStr,"Network Interface %d",netifId);
	l34_print_head(tmpStr);
	diag_util_mprintf("Interface ID:\t%d\n",netifInfo.id);
	netIfTypeToStr(netifInfo.ipInfo.netifType,tmpStr);
	diag_util_mprintf("Type:\t\t%s\n",tmpStr);
	stateToString(netifInfo.l2NetIfInfo.ifState,tmpStr);
	diag_util_mprintf("State:\t\t%s\n",tmpStr);
	diag_util_ip2str((uint8*)tmpStr, (uint32)netifInfo.ipInfo.ipaddr);
	diag_util_mprintf("IPv4 Addr:\t%s/%d\n",tmpStr,netifInfo.ipInfo.ipmask);
	for(i=0;i<netifInfo.ip6size;i++){
		diag_util_ipv62str((uint8*)tmpStr, (uint8*)netifInfo.ip6Info[i].ipaddr.ipv6_addr);
		diag_util_mprintf("IPv6 Addr:\t%s/%d\n",tmpStr,netifInfo.ip6Info[i].prefixLen);
	}
	diag_util_mac2str((uint8*)tmpStr,netifInfo.l2NetIfInfo.ifmac.octet);
	diag_util_mprintf("HW Addr:\t%s\n",tmpStr);
	if(netifInfo.pppInfo.pppState){
		diag_util_mprintf("IF Type:\tPPPoE\n");
		diag_util_mprintf("SessionID:\t%d\n",netifInfo.pppInfo.sessionId);
		stateToString(netifInfo.pppInfo.passThroughState,tmpStr);
		diag_util_mprintf("PassThrough:\t%s\n",tmpStr);
	}else
	{
		diag_util_mprintf("IF Type:\tEthernet\n");
	}
	diag_util_mprintf("VID:\t\t%d\n",netifInfo.l2NetIfInfo.vid);
	if(netifInfo.ipInfo.defaultPri!=L34_VLAN_PRI_DISABLE){
		diag_util_mprintf("Priority:\t%d\n",netifInfo.ipInfo.defaultPri);
	}else{
		diag_util_mprintf("Priority:\tDISABLED\n");
	}
	diag_util_mprintf("MTU:\t\t%d\n",netifInfo.l2NetIfInfo.mtu);
	stateToString(netifInfo.l2NetIfInfo.routingState,tmpStr);
	diag_util_mprintf("Routing State:\t%s\n",tmpStr);
	if(netifInfo.l2NetIfInfo.routingState){
		natTypeToString(netifInfo.ipInfo.natType,tmpStr);
		diag_util_mprintf("NAT Type:\t%s\n",tmpStr);
		if(netifInfo.ipInfo.natType == L34_NAT){
			diag_util_ip2str((uint8*)tmpStr, (uint32)netifInfo.ipInfo.internalIp);
			diag_util_mprintf("Internal IP:\t%s\n",tmpStr);
		}
	}

}


static void
diag_l34_print_arp(rtk_l34_arpInfo_t arpInfo,rtk_ip_addr_t ipaddr){
	char ipStr[128]="",macStr[128]="";
	diag_util_ip2str((uint8*)ipStr, (uint32)ipaddr);
	diag_util_mac2str(macStr, arpInfo.mac.octet);
	diag_util_mprintf("%s\t%s\t%d\t%d\t%d\n",ipStr,macStr,arpInfo.arpId,arpInfo.l2Id,arpInfo.netifId);
}

static void
diag_l34_print_neigh(rtk_l34_neigh6Info_t neighInfo){
	char tmpStr[128]="";
	/*print result*/
	sprintf(tmpStr,"Neighbor Entry %d",neighInfo.neighborId);
	l34_print_head(tmpStr);
	diag_util_ipv62str((uint8*)tmpStr, (uint8*)&neighInfo.ipaddr);
	diag_util_printf("Address:%s\n",tmpStr);
	diag_util_mac2str(tmpStr, neighInfo.mac.octet);
	diag_util_printf("MAC:\t%s\n",tmpStr);
	diag_util_eui642str((uint8*)tmpStr,&neighInfo.neighIfId);
	diag_util_printf("IP6IfId:%s\n",tmpStr);
	diag_util_printf("L2Id:\t%d\n",neighInfo.l2Id);
	diag_util_printf("RouteID:%d\n",neighInfo.routeId);
	diag_util_printf("Iface:\t%d\n",neighInfo.netifId);
}


static void
diag_l34_print_route(rtk_l34_routeInfo_t routeInfo){

	uint32 i;
	char tmpStr[128]="";
	sprintf(tmpStr,"Route rule %d",routeInfo.id);
	l34_print_head(tmpStr);
	diag_util_mprintf("Route ID:\t%d\n",routeInfo.id);
	stateToString(routeInfo.vaild,tmpStr);
	diag_util_mprintf("State:\t\t%s\n",tmpStr);
	diag_util_ip2str((uint8*)tmpStr, (uint32)routeInfo.routeTable.ipaddr);
	diag_util_mprintf("IP Addr:\t%s/%d\n",tmpStr,routeInfo.routeTable.ipmask);
	diag_util_mprintf("OutDev:\t\t%d\n",routeInfo.routeTable.netifId);
	routeTypeToString(routeInfo.routeType,tmpStr);
	diag_util_mprintf("Type:\t\t%s\n",tmpStr);
	if(routeInfo.routeType==L34_ROUTE_LOCAL)
	{
		diag_util_mprintf("ARP Range:\t%d~%d\n",routeInfo.arpStart,routeInfo.arpEnd);
	}else
	if(routeInfo.routeType==L34_ROUTE_STATIC || routeInfo.routeType==L34_ROUTE_DEFAULT || routeInfo.routeType==L34_ROUTE_POLICY)
	{
		diag_util_mprintf("Gateway Infomation\t\n");
		diag_util_mprintf("NextHop ID:\t%d\n",routeInfo.nhId);
		diag_util_mprintf("L2 Entry ID:\t%d\n",routeInfo.l2tableId);
		diag_util_mac2str(tmpStr, routeInfo.routeTable.gatewayMac.octet);
		diag_util_mprintf("Gateway MAC:\t%s\n",tmpStr);
	}

}

static void
diag_l34_print_route6(rtk_l34_route6Info_t routeInfo){

	uint32 i;
	char tmpStr[128]="";
	sprintf(tmpStr,"Route rule %d",routeInfo.routeId);
	l34_print_head(tmpStr);
	diag_util_mprintf("Route ID:\t%d\n",routeInfo.routeId);
	stateToString(routeInfo.valid,tmpStr);
	diag_util_mprintf("State:\t\t%s\n",tmpStr);
	diag_util_ipv62str((uint8*)tmpStr, routeInfo.route6Table.ipaddr.ipv6_addr);
	diag_util_mprintf("IP Addr:\t%s/%d\n",tmpStr,routeInfo.route6Table.prefixLen);
	diag_util_mprintf("OutDev:\t\t%d\n",routeInfo.route6Table.netifId);
	route6TypeToString(routeInfo.route6Type,tmpStr);
	diag_util_mprintf("Type:\t\t%s\n",tmpStr);

	if(routeInfo.route6Type==L34_IPV6_ROUTE_TYPE_GLOBAL)
	{
		diag_util_mprintf("Gateway Infomation\t\n");
		diag_util_mprintf("NextHop ID:\t%d\n",routeInfo.nhId);
		diag_util_mprintf("L2 Entry ID:\t%d\n",routeInfo.l2Id);
		diag_util_mac2str(tmpStr, routeInfo.route6Table.gatewayMac.octet);
		diag_util_mprintf("Gateway MAC:\t%s\n",tmpStr);
	}

}


/*
 * l34 lite init
 */
cparser_result_t
cparser_cmd_l34_lite_init(
    cparser_context_t *context)
{
	int32 ret;
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_ERR_CHK(rtk_l34_lite_init(),ret);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_lite_init */


/*
 * l34 ip link list
 */
cparser_result_t
cparser_cmd_l34_ip_link_list(
    cparser_context_t *context)
{
    	int32 i;
	DIAG_UTIL_PARAM_CHK();
	for(i=0;i< L34_IF_NUM_MAX;i++){
		diag_l34_ifconfig(i,DISABLED);
	}
	diag_util_mprintf("--End--\n");
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_ip_link_list */

/*
 * l34 ip link list <UINT:index>
 */
cparser_result_t
cparser_cmd_l34_ip_link_list_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
	int32 netifId ;
	DIAG_UTIL_PARAM_CHK();
	netifId = *index_ptr;
	diag_l34_ifconfig(netifId,ENABLED);
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_ip_link_list_index */

/*
 * l34 ip link add netif <UINT:index> address <MACADDR:macaddr> type ( wan | lan )
 */
cparser_result_t
cparser_cmd_l34_ip_link_add_netif_index_address_macaddr_type_wan_lan(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    cparser_macaddr_t  *macaddr_ptr)
{
   	int32 ret;
	rtk_l34_netifType_t netifType;
	rtk_mac_t mac;

	DIAG_UTIL_PARAM_CHK();
	osal_memcpy(&mac.octet, macaddr_ptr->octet, ETHER_ADDR_LEN);

    	if(!osal_strcmp(TOKEN_STR(9),"wan")){
		netifType = L34_NETIF_TYPE_WAN;
    	}else{
		netifType = L34_NETIF_TYPE_LAN;
	}
	DIAG_UTIL_ERR_CHK(rtk_l34_netif_create(*index_ptr,netifType,mac),ret);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_ip_link_add_netif_index_address_macaddr_type_wan_lan */

/*
 * l34 ip link set netif <UINT:index> vid <UINT:vid> default-pri <UINT:pri>
 */
cparser_result_t
cparser_cmd_l34_ip_link_set_netif_index_vid_vid_default_pri_pri(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *vid_ptr,
    uint32_t  *pri_ptr)
{
    	int32 ret;

	DIAG_UTIL_PARAM_CHK();

	DIAG_UTIL_ERR_CHK(rtk_l34_netifVlan_set(*index_ptr,*vid_ptr,*pri_ptr),ret);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_ip_link_set_netif_index_vid_vid_default_pri_pri */

/*
 * l34 ip link set netif <UINT:index> pppoe state disable
 */
cparser_result_t
cparser_cmd_l34_ip_link_set_netif_index_pppoe_state_disable(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    	int32 ret;
	DIAG_UTIL_PARAM_CHK();

	DIAG_UTIL_ERR_CHK( rtk_l34_netifPPPoE_set(*index_ptr,DISABLED,0,DISABLED),ret);


	return CPARSER_OK;
}    /* end of cparser_cmd_l34_ip_link_set_netif_index_pppoe_state_disable */


/*
 * l34 ip link set netif <UINT:index> pppoe state enable sessionId <UINT:id> pass-through ( enable | disable )
 */
cparser_result_t
cparser_cmd_l34_ip_link_set_netif_index_pppoe_state_enable_sessionId_id_pass_through_enable_disable(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *id_ptr)
{
    	int32 ret;
	rtk_enable_t state;
	DIAG_UTIL_PARAM_CHK();

	if(!osal_strcmp(TOKEN_STR(12),"enable")){
		state = ENABLED;
    	}else{
		state = DISABLED;
	}

	DIAG_UTIL_ERR_CHK( rtk_l34_netifPPPoE_set(*index_ptr,ENABLED,*id_ptr,state),ret);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_ip_link_set_netif_index_pppoe_state_enable_sessionid_id_pass_through_enable_disable */

/*
 * l34 ip link set netif <UINT:index> routing state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l34_ip_link_set_netif_index_routing_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
      	uint32 ret;
	rtk_enable_t state;

	DIAG_UTIL_PARAM_CHK();

	if(!osal_strcmp(TOKEN_STR(8),"enable")){
		state = ENABLED;
		}else{
		state = DISABLED;
	}
	DIAG_UTIL_ERR_CHK( rtk_l34_netifRoutingState_set(*index_ptr,state),ret);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_ip_link_set_netif_index_routing_state_disable_enable */

/*
 * l34 ip link set netif <UINT:index> mtu <UINT:mtu>
 */
cparser_result_t
cparser_cmd_l34_ip_link_set_netif_index_mtu_mtu(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *mtu_ptr)
{
     	uint32 ret;

	DIAG_UTIL_PARAM_CHK();

	DIAG_UTIL_ERR_CHK( rtk_l34_netifMtu_set(*index_ptr,*mtu_ptr),ret);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_ip_link_set_netif_index_mtu_mtu */


/*
 * l34 ip addr set <IPV4ADDR:ipaddr>  prefix-len <UINT:len> dev <UINT:netifId>
 */
cparser_result_t
cparser_cmd_l34_ip_addr_set_ipaddr_prefix_len_len_dev_netifId(
    cparser_context_t *context,
    uint32_t  *ipaddr_ptr,
    uint32_t  *len_ptr,
    uint32_t  *netifId_ptr)
{
	int32 ret;
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_ERR_CHK(rtk_l34_netifIpaddr_set(*netifId_ptr,*ipaddr_ptr,*len_ptr),ret);
      return CPARSER_OK;
}    /* end of cparser_cmd_l34_ip_addr_set_ipaddr_prefix_len_len_dev_netifid */

/*
 * l34 ip addr set <IPV4ADDR:ipaddr>  prefix-len <UINT:len> { via <MACADDR:macaddr> } dev <UINT:netifId>
 */
cparser_result_t
cparser_cmd_l34_ip_addr_set_ipaddr_prefix_len_len_via_macaddr_dev_netifId(
    cparser_context_t *context,
    uint32_t  *ipaddr_ptr,
    uint32_t  *len_ptr,
    cparser_macaddr_t  *macaddr_ptr,
    uint32_t  *netifId_ptr)
{
	int32 ret;
	rtk_mac_t mac;
	DIAG_UTIL_PARAM_CHK();

	DIAG_UTIL_ERR_CHK(rtk_l34_netifIpaddr_set(*netifId_ptr,*ipaddr_ptr,*len_ptr),ret);

	osal_memcpy(&mac.octet, macaddr_ptr->octet, ETHER_ADDR_LEN);
	DIAG_UTIL_ERR_CHK(rtk_l34_netifGateway_set(*netifId_ptr,mac),ret);
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_ip_addr_set_ipaddr_prefix_len_len_via_macaddr_dev_netifid */



/*
 * l34 ip link set netif <UINT:index> nat-type nat internal-ip <IPV4ADDR:ipaddr>
 */
cparser_result_t
cparser_cmd_l34_ip_link_set_netif_index_nat_type_nat_internal_ip_ipaddr(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *ipaddr_ptr)
{
    	int32 ret;
	rtk_l34_natType_t natType;
	DIAG_UTIL_PARAM_CHK();
	natType = L34_NAT;

	DIAG_UTIL_ERR_CHK(rtk_l34_netifNat_set(*index_ptr,natType,(rtk_ip_addr_t)*ipaddr_ptr),ret);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_ip_link_set_netif_index_nat_type_nat_internal_ip_ipaddr */

/*
 * l34 ip link set netif <UINT:index> nat-type ( local-public | full-cone-napt | restricted-napt | port-restricted-napt )
 */
cparser_result_t
cparser_cmd_l34_ip_link_set_netif_index_nat_type_local_public_full_cone_napt_restricted_napt_port_restricted_napt(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    	int32 ret;
	rtk_l34_natType_t natType;
	DIAG_UTIL_PARAM_CHK();
	if(!osal_strcmp(TOKEN_STR(7),"local-public")){
		natType = L34_LOCAL_PUBLIC;
      }else
      if(!osal_strcmp(TOKEN_STR(7),"full-cone-napt")){
		natType = L34_NAPT_FULL_CONE;
	}else
	if(!osal_strcmp(TOKEN_STR(7),"port-restricted-napt")){
		natType = L34_NAPT_PORT_RESTRICTED;
	}else
	//if(!osal_strcmp(TOKEN_STR(7),"restricted-napt"))
	{
		natType = L34_NAPT_RESTRICTED_CONE;
	}

	DIAG_UTIL_ERR_CHK(rtk_l34_netifNat_set(*index_ptr,natType,0),ret);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_ip_link_set_netif_index_nat_type_local_public_full_cone_napt_restricted_napt_port_restricted_napt */

/*
 * l34 ip link set netif <UINT:index> ( down | up )
 */
cparser_result_t
cparser_cmd_l34_ip_link_set_netif_index_down_up(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
     	int32 ret;
	rtk_enable_t state;
	DIAG_UTIL_PARAM_CHK();

	if(!osal_strcmp(TOKEN_STR(6),"down")){
		state = DISABLED;
	}else
	//if(!osal_strcmp(TOKEN_STR(6),"up"))
	{
		state = ENABLED;
	}
	DIAG_UTIL_ERR_CHK(rtk_l34_netifState_set(*index_ptr,state),ret);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_ip_link_set_netif_index_down_up */

/*
 * l34 ip link del netif <UINT:index>
 */
cparser_result_t
cparser_cmd_l34_ip_link_del_netif_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
	int32 ret;
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_ERR_CHK(rtk_l34_netif_del(*index_ptr),ret);
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_ip_link_del_netif_index */

/*
 * l34 ip neigh add <IPV4ADDR:ipaddr>  <MACADDR:macaddr>
 */
cparser_result_t
cparser_cmd_l34_ip_neigh_add_ipaddr_macaddr(
    cparser_context_t *context,
    uint32_t  *ipaddr_ptr,
    cparser_macaddr_t  *macaddr_ptr)
{
    	int32 ret;
	rtk_mac_t 	mac;
	DIAG_UTIL_PARAM_CHK();
	osal_memcpy(&mac.octet, macaddr_ptr->octet, ETHER_ADDR_LEN);
	DIAG_UTIL_ERR_CHK(rtk_l34_arp_add(*ipaddr_ptr,mac),ret);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_ip_neigh_add_ipaddr_macaddr */

/*
 * l34 ip neigh list { <IPV4ADDR:ipaddr> }
 */
cparser_result_t
cparser_cmd_l34_ip_neigh_list_ipaddr(
    cparser_context_t *context,
    uint32_t  *ipaddr_ptr)
{
    int32 ret,i,j;
	rtk_ip_addr_t 	ipaddr;
	rtk_l34_arpInfo_t arpInfo;
	rtk_l34_arp_entry_t arp;
	uint32 			length;
	rtk_l34_routeInfo_t routeInfo;
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();


	if(ipaddr_ptr){
		diag_util_mprintf("Address\t\tHWaddress\t\tARPId\tL2Id\tIface\n");
		DIAG_UTIL_ERR_CHK(rtk_l34_arp_get(*ipaddr_ptr,&arpInfo),ret);
		diag_l34_print_arp(arpInfo,*ipaddr_ptr);

	}else
	{
		diag_util_mprintf("ARPId\tL2Id\n");
		for(i=0;i<HAL_L34_ARP_ENTRY_MAX();i++){
			osal_memset(&arp,0,sizeof(rtk_l34_arp_entry_t));
			if((ret = rtk_l34_arpTable_get(i,&arp)) != RT_ERR_OK){
				continue;
			}
			if(arp.valid==ENABLED)
				diag_util_mprintf("%d\t%d\n",arp.index,arp.nhIdx);
		}
	}
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_ip_neigh_list_ipaddr */

/*
 * l34 ip neigh del <IPV4ADDR:ipaddr>
 */
cparser_result_t
cparser_cmd_l34_ip_neigh_del_ipaddr(
    cparser_context_t *context,
    uint32_t  *ipaddr_ptr)
{
    	int32 ret;
	DIAG_UTIL_PARAM_CHK();

	DIAG_UTIL_ERR_CHK(rtk_l34_arp_del(*ipaddr_ptr),ret);
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_ip_neigh_del_ipaddr */

/*
 * l34 ip route add default <MACADDR:macaddr> dev <UINT:netifId>
 */
cparser_result_t
cparser_cmd_l34_ip_route_add_default_macaddr_dev_netifId(
    cparser_context_t *context,
    cparser_macaddr_t  *macaddr_ptr,
    uint32_t  *netifId_ptr)
{
     	int32 ret;
	rtk_l34_routeType_t routeType;
	rtk_l34_routeTable_t routeTable;

	DIAG_UTIL_PARAM_CHK();

	routeType = L34_ROUTE_DEFAULT;

	osal_memcpy(&routeTable.gatewayMac.octet, macaddr_ptr->octet, ETHER_ADDR_LEN);
	routeTable.netifId = *netifId_ptr;
	routeTable.ipaddr = 0;
	routeTable.ipmask = 0;
	DIAG_UTIL_ERR_CHK(rtk_l34_route_add(routeType,routeTable),ret);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_ip_route_add_default_macaddr_dev_netifid */

/*
 * l34 ip route add <IPV4ADDR:ipaddr> prefix-len <UINT:len> via  <MACADDR:macaddr>  dev <UINT:netifId>
 */
cparser_result_t
cparser_cmd_l34_ip_route_add_ipaddr_prefix_len_len_via_macaddr_dev_netifId(
    cparser_context_t *context,
    uint32_t  *ipaddr_ptr,
    uint32_t  *len_ptr,
    cparser_macaddr_t  *macaddr_ptr,
    uint32_t  *netifId_ptr)
{
      int32 ret;
	rtk_l34_routeType_t routeType;
	rtk_l34_routeTable_t routeTable;
	DIAG_UTIL_PARAM_CHK();

	routeType = L34_ROUTE_STATIC;
	osal_memcpy(&routeTable.gatewayMac.octet, macaddr_ptr->octet, ETHER_ADDR_LEN);
	routeTable.ipaddr = *ipaddr_ptr;
	routeTable.ipmask = *len_ptr;
	routeTable.netifId = *netifId_ptr;

	DIAG_UTIL_ERR_CHK(rtk_l34_route_add(routeType,routeTable),ret);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_ip_route_add_ipaddr_prefix_len_len_via_macaddr_dev_netifid */

/*
 * l34 ip route add <IPV4ADDR:ipaddr> prefix-len <UINT:len> ( trap | drop ) dev <UINT:netifId>
 */
cparser_result_t
cparser_cmd_l34_ip_route_add_ipaddr_prefix_len_len_trap_drop_dev_netifId(
    cparser_context_t *context,
    uint32_t  *ipaddr_ptr,
    uint32_t  *len_ptr,
    uint32_t  *netifId_ptr)
{
      int32 ret;
	rtk_l34_routeType_t routeType;
	rtk_l34_routeTable_t routeTable;
	DIAG_UTIL_PARAM_CHK();
	if(!osal_strcmp(TOKEN_STR(7),"trap")){
		routeType = L34_ROUTE_CPU;
	}else{
		routeType = L34_ROUTE_DROP;
	}
	osal_memset(&routeTable,0,sizeof(rtk_l34_routeTable_t));
	routeTable.ipaddr = *ipaddr_ptr;
	routeTable.ipmask = *len_ptr;
	routeTable.netifId = *netifId_ptr;

	DIAG_UTIL_ERR_CHK(rtk_l34_route_add(routeType,routeTable),ret);
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_ip_route_add_ipaddr_prefix_len_len_trap_drop_dev_netifid */

/*
 * l34 ip route list { <UINT:index> }
 */
cparser_result_t
cparser_cmd_l34_ip_route_list_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
     	int32 ret,i;
	rtk_l34_netifInfo_t netifInfo;
	rtk_l34_routeInfo_t routeInfo;

	uint32 index;
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	if(index_ptr){
		index = *index_ptr;
		DIAG_UTIL_ERR_CHK(rtk_l34_route_get(index,&routeInfo),ret);
		if(routeInfo.vaild == ENABLED){
			diag_l34_print_route(routeInfo);
		}
	}else{

		for(i=0;i<L34_ROUTE_TABLE_SIZE;i++)
		{
			if((ret = rtk_l34_route_get(i,&routeInfo)) != RT_ERR_OK){
				continue;
			}
			if(routeInfo.vaild != ENABLED){
				continue;
			}
			diag_l34_print_route(routeInfo);
		}
	}
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_ip_route_list_index */

/*
 * l34 ip route del <IPV4ADDR:ipaddr> prefix-len <UINT:len>
 */
cparser_result_t
cparser_cmd_l34_ip_route_del_ipaddr_prefix_len_len(
    cparser_context_t *context,
    uint32_t  *ipaddr_ptr,
    uint32_t  *len_ptr)
{
    	int32 ret;

	DIAG_UTIL_PARAM_CHK();

	DIAG_UTIL_ERR_CHK(rtk_l34_route_del(*ipaddr_ptr,*len_ptr),ret);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_ip_route_del_ipaddr_prefix_len_len */

/*
 * l34 ip route del default
 */
cparser_result_t
cparser_cmd_l34_ip_route_del_default(
    cparser_context_t *context)
{
       int32 ret;

	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_ERR_CHK(rtk_l34_route_del(0,0),ret);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_ip_route_del_default */

/*
 * l34 conntrack add dir ( upsteram | downstream ) srcip <IPV4ADDR:sip> srcport <UINT:sport> dstip <IPV4ADDR:dip> dstport <UINT:dport> proto ( tcp | udp ) natip <IPV4ADDR:nip> natport <UINT:nport> priority <UINT:pri>
 */
cparser_result_t
cparser_cmd_l34_conntrack_add_dir_upsteram_downstream_srcip_sip_srcport_sport_dstip_dip_dstport_dport_proto_tcp_udp_natip_nip_natport_nport_priority_pri(
    cparser_context_t *context,
    uint32_t  *sip_ptr,
    uint32_t  *sport_ptr,
    uint32_t  *dip_ptr,
    uint32_t  *dport_ptr,
    uint32_t  *nip_ptr,
    uint32_t  *nport_ptr,
    uint32_t  *pri_ptr)
{
      int32 ret;
	rtk_l34_direct_t dir;
	rtk_l34_tuple_t tuple;
    rtk_l34_nat_info_t natInfo;

	DIAG_UTIL_PARAM_CHK();

	if(!osal_strcmp(TOKEN_STR(4),"upsteram")){
		dir = L34_DIR_UPSTREAM;
	}else
	//if(!osal_strcmp(TOKEN_STR(4),"downstream"))
	{
		dir = L34_DIR_DOWNSTREAM;
	}

	if(!osal_strcmp(TOKEN_STR(14),"tcp")){
		tuple.proto= L34_PROTO_TCP;
	}else
	//if(!osal_strcmp(TOKEN_STR(14),"udp"))
	{
		tuple.proto = L34_PROTO_UDP;
	}
	tuple.dstIp = *dip_ptr;
	tuple.dstport = *dport_ptr;
	tuple.srcIp = *sip_ptr;
	tuple.srcport = *sport_ptr;
	natInfo.natIp = *nip_ptr;
	natInfo.natPort= *nport_ptr;
    natInfo.priValid = 1;
    natInfo.priority = *pri_ptr;

	DIAG_UTIL_ERR_CHK(rtk_l34_connectTrack_add(dir,tuple,natInfo),ret);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_conntrack_add_dir_upsteram_downstream_srcip_sip_srcport_sport_dstip_dip_dstport_dport_proto_tcp_udp_natip_nip_natport_nport_priority_pri */

/*
 * l34 conntrack add dir ( upsteram | downstream ) srcip <IPV4ADDR:sip> srcport <UINT:sport> dstip <IPV4ADDR:dip> dstport <UINT:dport> proto ( tcp | udp ) natip <IPV4ADDR:nip> natport <UINT:nport>
 */
cparser_result_t
cparser_cmd_l34_conntrack_add_dir_upsteram_downstream_srcip_sip_srcport_sport_dstip_dip_dstport_dport_proto_tcp_udp_natip_nip_natport_nport(
    cparser_context_t *context,
    uint32_t  *sip_ptr,
    uint32_t  *sport_ptr,
    uint32_t  *dip_ptr,
    uint32_t  *dport_ptr,
    uint32_t  *nip_ptr,
    uint32_t  *nport_ptr)
{
      int32 ret;
	rtk_l34_direct_t dir;
	rtk_l34_tuple_t tuple;
    rtk_l34_nat_info_t natInfo;

	DIAG_UTIL_PARAM_CHK();

	if(!osal_strcmp(TOKEN_STR(4),"upsteram")){
		dir = L34_DIR_UPSTREAM;
	}else
	//if(!osal_strcmp(TOKEN_STR(4),"downstream"))
	{
		dir = L34_DIR_DOWNSTREAM;
	}

	if(!osal_strcmp(TOKEN_STR(14),"tcp")){
		tuple.proto= L34_PROTO_TCP;
	}else
	//if(!osal_strcmp(TOKEN_STR(14),"udp"))
	{
		tuple.proto = L34_PROTO_UDP;
	}
	tuple.dstIp = *dip_ptr;
	tuple.dstport = *dport_ptr;
	tuple.srcIp = *sip_ptr;
	tuple.srcport = *sport_ptr;
	natInfo.natIp = *nip_ptr;
	natInfo.natPort= *nport_ptr;
    natInfo.priValid = 0;

	DIAG_UTIL_ERR_CHK(rtk_l34_connectTrack_add(dir,tuple,natInfo),ret);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_conntrack_add_dir_upsteram_downstream_srcip_sip_srcport_sport_dstip_dip_dstport_dport_proto_tcp_udp_natip_nip_natport_nport */

/*
 * l34 conntrack get srcip <IPV4ADDR:sip> srcport <UINT:sport> dstip <IPV4ADDR:dip> dstport <UINT:dport> proto ( tcp | udp )
 */
cparser_result_t
cparser_cmd_l34_conntrack_get_srcip_sip_srcport_sport_dstip_dip_dstport_dport_proto_tcp_udp(
    cparser_context_t *context,
    uint32_t  *sip_ptr,
    uint32_t  *sport_ptr,
    uint32_t  *dip_ptr,
    uint32_t  *dport_ptr)
{
   	int32 ret;
	rtk_l34_tuple_t tuple;
	rtk_l34_connectInfo_t connInfo;
	char tmpStr[128]="";
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	if(!osal_strcmp(TOKEN_STR(12),"tcp")){
		tuple.proto= L34_PROTO_TCP;
	}else
	//if(!osal_strcmp(TOKEN_STR(12),"udp"))
	{
		tuple.proto = L34_PROTO_UDP;
	}

	tuple.dstIp = *dip_ptr;
	tuple.dstport = *dport_ptr;
	tuple.srcIp = *sip_ptr;
	tuple.srcport = *sport_ptr;

	DIAG_UTIL_ERR_CHK(rtk_l34_connectTrack_get(tuple,&connInfo),ret);

	dirToString(connInfo.direction,tmpStr);
	diag_util_mprintf("Direction:\t %s\n",tmpStr);
	diag_util_mprintf("NAPT ID:\t %d\n",connInfo.naptId);
	diag_util_mprintf("NAPTR ID:\t %d\n",connInfo.naptrId);
	natTypeToString(connInfo.natType,tmpStr);
	diag_util_mprintf("NAT Type:\t %s\n",tmpStr);
	diag_util_ip2str(tmpStr, connInfo.tuple.srcIp);
	diag_util_mprintf("SRC IP:\t\t %s\n",tmpStr);
	diag_util_mprintf("SRC PORT:\t %d\n",connInfo.tuple.srcport);
	diag_util_ip2str(tmpStr, connInfo.tuple.dstIp);
	diag_util_mprintf("DST IP:\t\t %s\n",tmpStr);
	diag_util_mprintf("DST PORT:\t %d\n",connInfo.tuple.dstport);
	diag_util_ip2str(tmpStr, connInfo.natIp);
	diag_util_mprintf("NAT IP:\t\t %s\n",tmpStr);
	diag_util_mprintf("NAT PORT:\t %d\n",connInfo.natport);
	protoToString(connInfo.tuple.proto,tmpStr);
	diag_util_mprintf("Proto:\t\t %s\n",tmpStr);
	stateToString(connInfo.priValied,tmpStr);
	diag_util_mprintf("Priority State:\t %s\n",tmpStr);
	diag_util_mprintf("Priority Value:\t %d\n",connInfo.priValue);
	stateToString(connInfo.trafficIndicator,tmpStr);
	diag_util_mprintf("TRF Indicator:\t %s\n",tmpStr);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_conntrack_get_srcip_sip_srcport_sport_dstip_dip_dstport_dport_proto_tcp_udp */

/*
 * l34 conntrack del  srcip <IPV4ADDR:sip> srcport <UINT:sport> dstip <IPV4ADDR:dip> dstport <UINT:dport> proto ( tcp | udp )
 */
cparser_result_t
cparser_cmd_l34_conntrack_del_srcip_sip_srcport_sport_dstip_dip_dstport_dport_proto_tcp_udp(
    cparser_context_t *context,
    uint32_t  *sip_ptr,
    uint32_t  *sport_ptr,
    uint32_t  *dip_ptr,
    uint32_t  *dport_ptr)
{
    	int32 ret;
	rtk_l34_tuple_t tuple;

	DIAG_UTIL_PARAM_CHK();

	tuple.dstIp = *dip_ptr;
	tuple.dstport = *dport_ptr;
	tuple.srcIp = *sip_ptr;
	tuple.srcport = *sport_ptr;

	if(!osal_strcmp(TOKEN_STR(12),"tcp")){
		tuple.proto= L34_PROTO_TCP;
	}else
	//if(!osal_strcmp(TOKEN_STR(12),"udp"))
	{
		tuple.proto = L34_PROTO_UDP;
	}

	DIAG_UTIL_ERR_CHK(rtk_l34_connectTrack_del(tuple),ret);
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_conntrack_del_srcip_sip_srcport_sport_dstip_dip_dstport_dport_proto_tcp_udp */

/*
 * l34 sys ( l34-route | l3-nat | l4-nat | l3-checksum-error-allow | l4-checksum-error-allow | nat-attack-log | ttl-minus | fragement-to-cpu | binding ) state ( enable | disable )
 */
cparser_result_t
cparser_cmd_l34_sys_l34_route_l3_nat_l4_nat_l3_checksum_error_allow_l4_checksum_error_allow_nat_attack_log_ttl_minus_fragement_to_cpu_binding_state_enable_disable(
    cparser_context_t *context)
{
    	int32 ret;
	rtk_l34_globalStateType_t type;
	rtk_enable_t state;
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	if(!osal_strcmp(TOKEN_STR(2),"l34-route")){
		type= L34_GLOBAL_L34_STATE;
	}else
	if(!osal_strcmp(TOKEN_STR(2),"l3-nat")){
		type = L34_GLOBAL_L3NAT_STATE;
	}else
	if(!osal_strcmp(TOKEN_STR(2),"l4-nat")){
		type = L34_GLOBAL_L4NAT_STATE;
	}else
	if(!osal_strcmp(TOKEN_STR(2),"l3-checksum-error-allow")){
		type = L34_GLOBAL_L3CHKSERRALLOW_STATE;
	}else
	if(!osal_strcmp(TOKEN_STR(2),"l4-checksum-error-allow")){
		type = L34_GLOBAL_L4CHKSERRALLOW_STATE;
	}else
	if(!osal_strcmp(TOKEN_STR(2),"nat-attack-log")){
		type = L34_GLOBAL_NAT2LOG_STATE;
	}else
	if(!osal_strcmp(TOKEN_STR(2),"ttl-minus")){
		type = L34_GLOBAL_TTLMINUS_STATE;
	}else
	if(!osal_strcmp(TOKEN_STR(2),"fragement-to-cpu")){
		type = L34_GLOBAL_FRAG2CPU_STATE;
	}else
	//if(!osal_strcmp(TOKEN_STR(2),"binding"))
	{
		type = L34_GLOBAL_BIND_STATE;
	}

	if(!osal_strcmp(TOKEN_STR(4),"enable")){
		state = ENABLED;
	}else
	//if(!osal_strcmp(TOKEN_STR(4),"disable"))
	{
		state = DISABLED;
	}
	DIAG_UTIL_ERR_CHK(rtk_l34_globalState_set(type,state),ret);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_sys_l34_route_l3_nat_l4_nat_l3_checksum_error_allow_l4_checksum_error_allow_nat_attack_log_ttl_minus_fragement_to_cpu_binding_state_enable_disable */

/*
 * l34 sys interface-decision-mode mode ( vlan-based | port-based | mac-based )
 */
cparser_result_t
cparser_cmd_l34_sys_interface_decision_mode_mode_vlan_based_port_based_mac_based(
    cparser_context_t *context)
{
      int32 ret;
	rtk_l34_lookupMode_t lookupMode;
	DIAG_UTIL_PARAM_CHK();

	if(!osal_strcmp(TOKEN_STR(4),"vlan-based")){
		lookupMode= L34_LOOKUP_VLAN_BASE;
	}else
	if(!osal_strcmp(TOKEN_STR(4),"port-based")){
		lookupMode = L34_LOOKUP_PORT_BASE;
	}else
	//if(!osal_strcmp(TOKEN_STR(4),"mac-based"))
	{
		lookupMode = L34_LOOKUP_MAC_BASE;
	}
	DIAG_UTIL_ERR_CHK(rtk_l34_lookupMode_set(lookupMode),ret);
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_sys_interface_decision_mode_mode_vlan_based_port_based_mac_based */

/*
 * l34 sys  port-map  ( mac-port | ext-port | dslvc-port ) ( <PORT_LIST:ports> | all ) dev <UINT:index>
 */
cparser_result_t
cparser_cmd_l34_sys_port_map_mac_port_ext_port_dslvc_port_ports_all_dev_index(
    cparser_context_t *context,
    char * *ports_ptr,
    uint32_t  *index_ptr)
{
    	 int32 ret;
	rtk_l34_portType_t type;
	diag_portlist_t portlist;
	rtk_port_t port;
	uint32 netifId ;
	DIAG_UTIL_PARAM_CHK();

	netifId = *index_ptr;

	if(!osal_strcmp(TOKEN_STR(3),"mac-port")){
		type= L34_PORT_MAC;
	}else
	if(!osal_strcmp(TOKEN_STR(3),"ext-port")){
		type = L34_PORT_EXTENSION;
	}else
	if(!osal_strcmp(TOKEN_STR(3),"dslvc-port")){
		type = L34_PORT_DSLVC;
	}

	DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
	DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    	{
		DIAG_UTIL_ERR_CHK(rtk_l34_lookupPortMap_set(type,port,netifId),ret);

    	}
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_sys_port_map_mac_port_ext_port_dslvc_port_ports_all_dev_index */

/*
 * l34 sys wan-route action ( drop | trap-to-cpu | forward )
 */
cparser_result_t
cparser_cmd_l34_sys_wan_route_action_drop_trap_to_cpu_forward(
    cparser_context_t *context)
{
     	int32 ret;
	rtk_l34_wanRouteMode_t wanRouteMode;
	DIAG_UTIL_PARAM_CHK();

	if(!osal_strcmp(TOKEN_STR(4),"drop")){
		wanRouteMode= L34_WANROUTE_DROP;
	}else
	if(!osal_strcmp(TOKEN_STR(4),"trap-to-cpu")){
		wanRouteMode = L34_WANROUTE_FORWARD2CPU;
	}else
	//if(!osal_strcmp(TOKEN_STR(4),"forward"))
	{
		wanRouteMode = L34_WANROUTE_FORWARD;
	}
	DIAG_UTIL_ERR_CHK(rtk_l34_wanRoutMode_set(wanRouteMode),ret);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_sys_wan_route_action_drop_trap_to_cpu_forward */

/*
 * l34 sys list
 */
cparser_result_t
cparser_cmd_l34_sys_list(
    cparser_context_t *context)
{
      	rtk_l34_global_cfg_t globalCfg;
	char tmpStr[128]="";
	char map[2][128];
	int32 ret;
	uint32 i;
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_OUTPUT_INIT();

	DIAG_UTIL_ERR_CHK(rtk_l34_globalCfg_get(&globalCfg),ret);

	stateToString(globalCfg.globalState[L34_GLOBAL_L34_STATE], tmpStr);
	diag_util_mprintf("L34 State:\t\t\t%s\n",tmpStr);
	stateToString(globalCfg.globalState[L34_GLOBAL_L3NAT_STATE], tmpStr);
	diag_util_mprintf("L3 NAT State:\t\t\t%s\n",tmpStr);
	stateToString(globalCfg.globalState[L34_GLOBAL_L4NAT_STATE], tmpStr);
	diag_util_mprintf("L4 NAT State:\t\t\t%s\n",tmpStr);
	stateToString(globalCfg.globalState[L34_GLOBAL_L3CHKSERRALLOW_STATE], tmpStr);
	diag_util_mprintf("L3 CheckSum Error Allow State:\t%s\n",tmpStr);
	stateToString(globalCfg.globalState[L34_GLOBAL_L4CHKSERRALLOW_STATE], tmpStr);
	diag_util_mprintf("L4 CheckSum Error Allow State:\t%s\n",tmpStr);
	stateToString(globalCfg.globalState[L34_GLOBAL_NAT2LOG_STATE], tmpStr);
	diag_util_mprintf("NAT Attack Log:\t\t\t%s\n",tmpStr);
	stateToString(globalCfg.globalState[L34_GLOBAL_TTLMINUS_STATE], tmpStr);
	diag_util_mprintf("TTL Minus:\t\t\t%s\n",tmpStr);
	stateToString(globalCfg.globalState[L34_GLOBAL_FRAG2CPU_STATE], tmpStr);
	diag_util_mprintf("Fragment Trap:\t\t\t%s\n",tmpStr);
	stateToString(globalCfg.globalState[L34_GLOBAL_BIND_STATE], tmpStr);
	diag_util_mprintf("Binding State:\t\t\t%s\n",tmpStr);
	lookupModeToStr(globalCfg.lookupMode,tmpStr);
	diag_util_mprintf("LIMDBC:\t\t\t\t%s\n",tmpStr);
	wanRouteModeToStr(globalCfg.wanRouteMode,tmpStr);
	diag_util_mprintf("WAN Route Mode:\t\t\t%s\n",tmpStr);

#if defined(CONFIG_SDK_APOLLO) || defined(CONFIG_SDK_APOLLOMP)
   if((APOLLO_CHIP_ID == DIAG_UTIL_CHIP_TYPE) ||
       (APOLLOMP_CHIP_ID == DIAG_UTIL_CHIP_TYPE))
    {
		l34_print_head("MAC Port Map To Interface");
		for(i=0;i<L34_MAC_PORT_MAX;i+=2){
			if((i+1) <L34_MAC_PORT_MAX){
				diag_util_mprintf("%d:\t%d\t%d:\t%d\n",i,globalCfg.macPortMap[i],i+1,globalCfg.macPortMap[i+1]);
			}else{
				diag_util_mprintf("%d:\t%d\n",i,globalCfg.macPortMap[i]);
			}
		}
		l34_print_head("Ext Port Map To Interface");

		for(i=0;i<L34_EXT_PORT_MAX;i+=2){
			if((i+1) <L34_EXT_PORT_MAX){
				diag_util_mprintf("%d:\t%d\t%d:\t%d\n",i,globalCfg.extPortMap[i],i+1,globalCfg.extPortMap[i+1]);
			}else{
				diag_util_mprintf("%d:\t%d\n",i,globalCfg.extPortMap[i]);
			}
		}
		l34_print_head("VC Port Map To Interface");
		for(i=0;i<L34_DSLVC_PORT_MAX;i+=2){
			if((i+1) <L34_DSLVC_PORT_MAX){
				diag_util_mprintf("%d:\t%d\t%d:\t%d\n",i,globalCfg.dslVcPortMap[i],i+1,globalCfg.dslVcPortMap[i+1]);
			}else{
				diag_util_mprintf("%d:\t%d\n",i,globalCfg.dslVcPortMap[i]);
			}
		}
   	}
#endif

	diag_util_mprintf("--End--\n");
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_sys_list */

/*
 * l34 ip6 route add default <MACADDR:macaddr> dev <UINT:netifId>
 */
cparser_result_t
cparser_cmd_l34_ip6_route_add_default_macaddr_dev_netifId(
    cparser_context_t *context,
    cparser_macaddr_t  *macaddr_ptr,
    uint32_t  *netifId_ptr)
{
	int32 ret;
	rtk_l34_route6Table_t route;
	DIAG_UTIL_PARAM_CHK();

	osal_memset(&route,0,sizeof(rtk_l34_route6Table_t));
	route.prefixLen = 0;
	route.netifId = *netifId_ptr;
	osal_memcpy(&route.gatewayMac.octet, macaddr_ptr->octet, ETHER_ADDR_LEN);
       DIAG_UTIL_ERR_CHK(rtk_l34_route6_add(L34_IPV6_ROUTE_TYPE_GLOBAL, route), ret);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_ip6_route_add_default_macaddr_dev_netifid */

/*
 * l34 ip6 route add <IPV6ADDR:ip6addr> prefix-len <UINT:prefixlen> via  <MACADDR:macaddr>  dev <UINT:netifId>
 */
cparser_result_t
cparser_cmd_l34_ip6_route_add_ip6addr_prefix_len_prefixlen_via_macaddr_dev_netifId(
    cparser_context_t *context,
    char * *ip6addr_ptr,
    uint32_t  *prefixlen_ptr,
    cparser_macaddr_t  *macaddr_ptr,
    uint32_t  *netifId_ptr)
{
	int32 ret;
	rtk_l34_route6Table_t route;
	DIAG_UTIL_PARAM_CHK();

	osal_memset(&route,0,sizeof(rtk_l34_route6Table_t));
	route.prefixLen = *prefixlen_ptr;
	route.netifId = *netifId_ptr;
	osal_memcpy(&route.gatewayMac.octet, macaddr_ptr->octet, ETHER_ADDR_LEN);
	DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(&route.ipaddr.ipv6_addr[0],TOKEN_STR(4)), ret);
       DIAG_UTIL_ERR_CHK(rtk_l34_route6_add(L34_IPV6_ROUTE_TYPE_GLOBAL, route), ret);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_ip6_route_add_ip6addr_prefix_len_prefixlen_via_macaddr_dev_netifid */

/*
 * l34 ip6 route add <IPV6ADDR:ip6addr> prefix-len <UINT:prefixlen> ( trap | drop ) dev <UINT:netifId>
 */
cparser_result_t
cparser_cmd_l34_ip6_route_add_ip6addr_prefix_len_prefixlen_trap_drop_dev_netifId(
    cparser_context_t *context,
    char * *ip6addr_ptr,
    uint32_t  *prefixlen_ptr,
    uint32_t  *netifId_ptr)
{
	int32 ret;
	rtk_l34_route6Table_t route;
	rtk_l34_ipv6RouteType_t type;

	DIAG_UTIL_PARAM_CHK();

	if(!osal_strcmp(TOKEN_STR(7),"trap")){
		type= L34_IPV6_ROUTE_TYPE_TRAP;
	}else{
		type= L34_IPV6_ROUTE_TYPE_DROP;
	}

	osal_memset(&route,0,sizeof(rtk_l34_route6Table_t));
	route.prefixLen = *prefixlen_ptr;
	route.netifId = *netifId_ptr;
	DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(&route.ipaddr.ipv6_addr[0],TOKEN_STR(4)), ret);
       DIAG_UTIL_ERR_CHK(rtk_l34_route6_add(type, route), ret);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_ip6_route_add_ip6addr_prefix_len_prefixlen_trap_drop_dev_netifid */

/*
 * l34 ip6 route list { <UINT:index> }
 */
cparser_result_t
cparser_cmd_l34_ip6_route_list_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
	int32 ret;
	uint32 i;
	rtk_l34_route6Info_t routeInfo;
	DIAG_UTIL_PARAM_CHK();

	if(index_ptr){
		if(rtk_l34_route6_get(*index_ptr,&routeInfo)==RT_ERR_OK)
		{
			diag_l34_print_route6(routeInfo);
		}
	}else{
		for(i=0;i<HAL_L34_IPV6_ROUTING_ENTRY_MAX();i++)
		{
			if(rtk_l34_route6_get(i,&routeInfo)==RT_ERR_OK && routeInfo.valid==ENABLED)
			{
				diag_l34_print_route6(routeInfo);
			}
		}
	}
	diag_util_mprintf("--End--\n");
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_ip6_route_list_index */

/*
 * l34 ip6 route del  <IPV6ADDR:ip6addr> prefix-len <UINT:prefixlen>
 */
cparser_result_t
cparser_cmd_l34_ip6_route_del_ip6addr_prefix_len_prefixlen(
    cparser_context_t *context,
    char * *ip6addr_ptr,
    uint32_t  *prefixlen_ptr)
{
	int32 ret;
	rtk_ipv6_addr_t ipaddr;
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(&ipaddr.ipv6_addr[0],TOKEN_STR(4)), ret);
	DIAG_UTIL_ERR_CHK(rtk_l34_route6_del(ipaddr,*prefixlen_ptr), ret);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_ip6_route_del_ip6addr_prefix_len_prefixlen */

/*
 * l34 ip6 route del default
 */
cparser_result_t
cparser_cmd_l34_ip6_route_del_default(
    cparser_context_t *context)
{
    	int32 ret;
	rtk_ipv6_addr_t ipaddr;
	osal_memset(&ipaddr,0,sizeof(rtk_ipv6_addr_t));
	DIAG_UTIL_ERR_CHK(rtk_l34_route6_del(ipaddr,0), ret);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_ip6_route_del_default */

/*
 * l34 ip6 neigh add <IPV6ADDR:ip6addr> lladdr <MACADDR:macaddr>
 */
cparser_result_t
cparser_cmd_l34_ip6_neigh_add_ip6addr_lladdr_macaddr(
    cparser_context_t *context,
    char * *ip6addr_ptr,
    cparser_macaddr_t  *macaddr_ptr)
{
	int32 ret;
	rtk_ipv6_addr_t ipaddr;
	rtk_mac_t	   mac;
	DIAG_UTIL_PARAM_CHK();

	osal_memcpy(&mac.octet, macaddr_ptr->octet, ETHER_ADDR_LEN);
	DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(&ipaddr.ipv6_addr[0],TOKEN_STR(4)), ret);
	DIAG_UTIL_ERR_CHK(rtk_l34_neigh6_add(ipaddr,mac), ret);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_ip6_neigh_add_ip6addr_lladdr_macaddr_dev_netifid */

/*
 * l34 ip6 neigh del <IPV6ADDR:ip6addr>
 */
cparser_result_t
cparser_cmd_l34_ip6_neigh_del_ip6addr(
    cparser_context_t *context,
    char * *ip6addr_ptr)
{
	int32 ret;
	rtk_ipv6_addr_t ipaddr;

	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(&ipaddr.ipv6_addr[0],TOKEN_STR(4)), ret);
	DIAG_UTIL_ERR_CHK(rtk_l34_neigh6_del(ipaddr), ret);

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_ip6_neigh_del_ip6addr */

/*
 * l34 ip6 neigh list { <IPV6ADDR:ip6addr> }
 */
cparser_result_t
cparser_cmd_l34_ip6_neigh_list_ip6addr(
    cparser_context_t *context,
    char * *ip6addr_ptr)
{
    int32 ret;
	int32 i;
	rtk_l34_neigh6Info_t neighInfo;
	rtk_ipv6Neighbor_entry_t neigh;
	rtk_ipv6_addr_t ipaddr;
	char tmp[128]="";

	DIAG_UTIL_PARAM_CHK();
	if(ip6addr_ptr){
		DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(&ipaddr.ipv6_addr[0],TOKEN_STR(4)), ret);
		osal_memset(&neighInfo,0,sizeof(rtk_l34_neigh6Info_t));
		if(rtk_l34_neigh6_get(ipaddr,&neighInfo)==RT_ERR_OK)
		{
			diag_l34_print_neigh(neighInfo);
		}
	}else
	{
		diag_util_mprintf("NeighborId\tNeighIfId\t\tRouteId\tL2Id\n");
		for(i=0;i<HAL_L34_IPV6_NEIGHBOR_ENTRY_MAX();i++)
		{
			if(rtk_l34_ipv6NeighborTable_get(i,&neigh)==RT_ERR_OK && neigh.valid==ENABLED)
			{
				diag_util_eui642str(tmp, &neigh.ipv6Ifid);
				diag_util_printf("%d\t\t%s\t%d\t%d\n",i,tmp,neigh.ipv6RouteIdx,neigh.l2Idx);
			}
		}
	}

	return CPARSER_OK;
}    /* end of cparser_cmd_l34_ip6_neigh_list_ip6addr */

/*
 * l34 ip6 addr add <IPV6ADDR:ip6addr> prefix-len <UINT:prefixlen> dev <UINT:netifId>
 */
cparser_result_t
cparser_cmd_l34_ip6_addr_add_ip6addr_prefix_len_prefixlen_dev_netifId(
    cparser_context_t *context,
    char * *ip6addr_ptr,
    uint32_t  *prefixlen_ptr,
    uint32_t  *netifId_ptr)
{
    	int32 ret;
	rtk_ipv6_addr_t ipaddr;
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(&ipaddr.ipv6_addr[0],TOKEN_STR(4)), ret);
	DIAG_UTIL_ERR_CHK(rtk_l34_netifIp6addr_add(*netifId_ptr,ipaddr,*prefixlen_ptr), ret);
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_ip6_addr_add_ip6addr_prefix_len_prefixlen_dev_netifid */

/*
 * l34 ip6 addr del <IPV6ADDR:ip6addr> prefix-len <UINT:prefixlen> dev <UINT:netifId>
 */
cparser_result_t
cparser_cmd_l34_ip6_addr_del_ip6addr_prefix_len_prefixlen_dev_netifId(
    cparser_context_t *context,
    char * *ip6addr_ptr,
    uint32_t  *prefixlen_ptr,
    uint32_t  *netifId_ptr)
{
    	int32 ret;
	rtk_ipv6_addr_t ipaddr;
	DIAG_UTIL_PARAM_CHK();
	DIAG_UTIL_ERR_CHK(diag_util_str2ipv6(&ipaddr.ipv6_addr[0],TOKEN_STR(4)), ret);
	DIAG_UTIL_ERR_CHK(rtk_l34_netifIp6addr_del(*netifId_ptr,ipaddr,*prefixlen_ptr), ret);
	return CPARSER_OK;
}    /* end of cparser_cmd_l34_ip6_addr_del_ip6addr_prefix_len_prefixlen_dev_netifid */

