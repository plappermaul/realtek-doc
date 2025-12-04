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
 * Purpose : Virual Device for Control NIC API
 *
 * Feature : Provide the APIs to control Virtaul Device
 *
 */

#ifndef __VIRTUAL_DEVICE_CONFIG_H__
#define __VIRTUAL_DEVICE_CONFIG_H__

#include <common/rt_type.h>
#include <common/rt_error.h>
#include <linux/list.h>
#include <net/netfilter/nf_conntrack.h>

extern int vd_debug;
#define VD_DEBUG(level,fmt,args...) if(vd_debug >= level) printk("netvd:\t" fmt "\n",## args)

#define NETVD_FULLNAME 				"RealTek Net Vritual Device Driver"
#define NETVD_VERSION 				"1.0.0"
#define VLAN_VID_MASK 				0xfff
#define IFF_VIRTUAL_DEV 			0x70000
#define DRV_NIC_STOP 				1

#define NETVD_WAN_START				1
#define NETVD_WAN_STOP				6

#define CONNTRACK_ADD_MARK 			0x1
#define CT_ENTRY_MAX				2048
#define DEFUALT_CT_PERIODIC_TIME 	10
#define DEFAULT_WAN_NAME			"nas0_"
#define DEFAULT_LAN_NAME			"br"

enum {
	NETVD_LOG_LEVEL_LOW=1,
	NETVD_LOG_LEVEL_HIGH,
	NETVD_LOG_LEVEL_END,
};

typedef enum {
	VD_ERR_OK					=0,
	VD_ERR_ADD_FAILED			=-1,
	VD_ERR_REMOVE_FAILED		=-2,
	VD_ERR_ENTRY_NOTFOUNT		=-3,
	VD_ERR_FAILED				=-5,
}vd_error_t;

typedef enum netvd_netifType_e{
	NETVD_NETIF_LAN=0,
	NETVD_NETIF_WAN,
}netvd_netifType_t;

typedef struct pppInfo_s {
	int id;
	int netifId;
	unsigned char remote[ETHER_ADDR_LEN]; 
}pppInfo_t;

typedef struct gwInfo_s {
	char devName[IFNAMSIZ];
	uint32 gwIp;
}gwInfo_t;

typedef struct gw6Info_s {
	char devName[IFNAMSIZ];
	struct in6_addr gwIp;
}gw6Info_t;



typedef struct netvd_hal_mapper {
	
	int (*hal_hwnat_init)(void);
	int (*hal_netif_create)(netvd_netifType_t type,uint32 netifId,unsigned char *gmac);
	int (*hal_netif_delete)(uint32 netifId);
	int (*hal_netif_ppp_create)(uint32 netifId, int32 sessionId, unsigned char *gatewayMac);
    int (*hal_netif_mtu_set)(uint32 netifId, uint32 mtu);
	int (*hal_netif_mac_set)(uint32 netifId, unsigned char *mac);
	int (*hal_netif_ip_set)(uint32 netifId, uint32 ipaddr, uint32 prefix,char *gatewayMac);
	int (*hal_netif_gw_set)(uint32 netifId, unsigned char *mac);
	int (*hal_netif_vid_set)(uint32 netifId, uint32 vid,uint32 pbit);
	int (*hal_ct_del)(struct nf_conn *ct);
	int (*hal_ct_add)(struct nf_conn *ct, uint32 *naptId,int dir);
	int (*hal_ct_tf_get_all)(unsigned int *tfValue);
	int (*hal_arp_add)(uint32 ipaddr, unsigned char *mac);
	int (*hal_arp_del)(uint32 ipaddr);
	int (*hal_arp_get)(uint32 ipaddr,unsigned char *mac);
	int (*hal_route_add)(int32 netifId, uint32 ipaddr, uint32 prefix,unsigned char *gatewayMac);
	int (*hal_route_del)(uint32 ipaddr, uint32 prefix);
	int (*hal_neigh_add)(struct in6_addr ip6addr,unsigned char *mac);
	int (*hal_neigh_del)(struct in6_addr ip6addr);
	int (*hal_neigh_get)(struct in6_addr ip6addr,unsigned char *mac);
	int (*hal_route6_add)(int32 netifId, struct in6_addr ip6addr,uint32 prefix,unsigned char *gatewayMac);
	int (*hal_route6_del)(struct in6_addr ip6addr,uint32 prefix);
	int (*hal_netif_ip6_set)(uint32 netifId,struct in6_addr ip6addr, uint32 prefix);
	int (*hal_ponPort_get)(int *portId);
	int (*hal_netif_ip4addr_get)(uint32 netifId, uint32 *pIpaddr, uint32 *pMask);
}netvd_hal_mapper_t;


int32 netvd_event_map_register(netvd_hal_mapper_t *map);


#endif
