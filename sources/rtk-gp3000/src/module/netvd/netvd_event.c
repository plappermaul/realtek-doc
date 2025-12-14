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
#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <linux/rtnetlink.h>
#include <linux/notifier.h>
#include <linux/list.h>
#include <net/rtnetlink.h>
#include <net/netns/generic.h>
#include <linux/etherdevice.h>
#include <net/netfilter/nf_conntrack_ecache.h>
#include <net/netevent.h>
#include <linux/inetdevice.h>
#include <net/netfilter/nf_conntrack.h>
#include <net/netfilter/nf_conntrack_core.h>
#include <net/netfilter/ipv4/nf_conntrack_ipv4.h>
#include <net/neighbour.h>
#include <module/netvd/netvd.h>
#include <net/route.h>
#include <net/sock.h>
#include <net/ip_fib.h>
#include <net/if_inet6.h>
#include <net/addrconf.h>
#include <net/arp.h>
#include <br_private.h>

#include <linux/if_smux.h>


enum {
	NETVD_PPP_STATE_INIT,
	NETVD_PPP_STATE_IP_UP,
	NETVD_PPP_STATE_IP_DOWN,
	NETVD_PPP_STATE_GWIP_UP
} PPP_STATE;



extern int ponPort;
extern  struct list_head vdhead;
struct list_head cthead;


netvd_hal_mapper_t *mapper;
char wanName[16]=DEFAULT_WAN_NAME;
char lanName[16]=DEFAULT_LAN_NAME;
/*for interface vlan auto learning when ifconfig up*/
int vd_usage = 0;
static int ppp_state = NETVD_PPP_STATE_INIT;

#define NETVD_MAPPER_CHECK(fun) { \
	if(mapper==NULL)  \
		return VD_ERR_OK; \
	else if(fun==NULL) \
		return VD_ERR_OK; \
}


static int netif_usage[8];

int32
netvd_event_map_register(netvd_hal_mapper_t *map){

	mapper= map;
	return 0;
}

EXPORT_SYMBOL(netvd_event_map_register);

/*common api for all event*/
int getNetIfIdByDevName(const char *devName,netvd_netifType_t *type)
{
	char *p,*end;
	int netifId=-1,len;
	p = strstr(devName,wanName);

	if(p!=NULL)
	{
		len = strlen(wanName);
		*type = NETVD_NETIF_WAN;
		netifId = simple_strtol(p+len,&end,10);
		netifId +=NETVD_WAN_START;
	}else
	{		
		len = strlen(lanName);
		p = strstr(devName,lanName);
		if(p!=NULL)
		{
			*type = NETVD_NETIF_LAN;
			netifId = simple_strtol(p+len,&end,10);
		}
	}
	return netifId;
}

int getPPPoeIdByDevName(const char *devName)
{
	char *p,*end;
	int netifId=-1;
	p = strstr(devName,"ppp");

	if(p!=NULL){
		netifId = simple_strtol(p+3,&end,10);
		netifId+=NETVD_WAN_START;
	}
	return netifId;
}


/*common api for check ipv6 address is link local address or not*/
int is_ip6_link_local(struct in6_addr ip6addr, uint32 prefix)
{
	if(prefix==64 &&
	ip6addr.s6_addr16[0] == 0xfe80 &&
	ip6addr.s6_addr16[1] == 0	&&
	ip6addr.s6_addr16[2] == 0	&&
	ip6addr.s6_addr16[3] == 0)
	{
		return 1;
	}
	return 0;
}


static int checkPPPState(void)
{
	switch(ppp_state){
	case NETVD_PPP_STATE_INIT:
		ppp_state = NETVD_PPP_STATE_IP_UP;
	break;
	case NETVD_PPP_STATE_IP_UP:
		ppp_state = NETVD_PPP_STATE_IP_DOWN;
		return 0;
	break;
	case NETVD_PPP_STATE_IP_DOWN:
		ppp_state = NETVD_PPP_STATE_IP_DOWN;
		return 0;
	break;
	case NETVD_PPP_STATE_GWIP_UP:
		ppp_state = NETVD_PPP_STATE_INIT;
		return 0;
	break;
	default:
		return 0;
	break;
	}
	return 1;
}



/*********************************************************
*	Start to for event handler
**********************************************************/

/*interface related event*/
static void vd_sync_address(struct net_device *dev,
			      struct net_device *vdev)
{
	struct vd_dev_info *vd = vd_dev_info(vdev);

	/* May be called without an actual change */
	if (!compare_ether_addr(vd->real_dev_addr, dev->dev_addr))
		return;

	/* vlan address was different from the old address and is equal to
	 * the new address */
	if (compare_ether_addr(vdev->dev_addr, vd->real_dev_addr) &&
	    !compare_ether_addr(vdev->dev_addr, dev->dev_addr))
		dev_unicast_delete(dev, vdev->dev_addr, ETH_ALEN);

	/* vlan address was equal to the old address and is different from
	 * the new address */
	if (!compare_ether_addr(vdev->dev_addr, vd->real_dev_addr) &&
	    compare_ether_addr(vdev->dev_addr, dev->dev_addr))
		dev_unicast_add(dev, vdev->dev_addr, ETH_ALEN);

	memcpy(vd->real_dev_addr, dev->dev_addr, ETH_ALEN);
}


static void vd_transfer_features(struct net_device *dev,
				   struct net_device *vdev)
{
	unsigned long old_features = vdev->features;

	vdev->features &= ~dev->vlan_features;
	vdev->features |= dev->features & dev->vlan_features;
	vdev->gso_max_size = dev->gso_max_size;

	if (old_features != vdev->features)
		netdev_features_change(vdev);
}

void vd_transfer_operstate(const struct net_device *dev,
				    struct net_device *vdev)
{
	/* Have to respect userspace enforced dormant state
	 * of real device, also must allow supplicant running
	 * on virtual device
	 */
	if (dev->operstate == IF_OPER_DORMANT)
		netif_dormant_on(vdev);
	else
		netif_dormant_off(vdev);

	if (netif_carrier_ok(dev)) {
		if (!netif_carrier_ok(vdev))
			netif_carrier_on(vdev);
	} else {
		if (netif_carrier_ok(vdev))
			netif_carrier_off(vdev);
	}
}



static void 
__vd_device_event(struct net_device *dev, unsigned long event)
{
	switch (event) {
	case NETDEV_CHANGENAME:
		vd_proc_rem_dev(dev);
		if (vd_proc_add_dev(dev) < 0)
			VD_DEBUG(NETVD_LOG_LEVEL_LOW,"Virtual Devce: failed to change proc name for %s\n",
					dev->name);
		break;
	case NETDEV_REGISTER:
		if (vd_proc_add_dev(dev) < 0)
			VD_DEBUG(NETVD_LOG_LEVEL_LOW,"Virtual Devce: failed to add proc entry for %s\n",
					dev->name);
		break;
	case NETDEV_UNREGISTER:
		vd_proc_rem_dev(dev);
		break;
	}
}


static void vd_do_event(unsigned long event,struct net_device *dev,struct net_device *vdev){

	int flgs;

	switch (event) {
	case NETDEV_CHANGE:
		/* Propagate real device state to virtual device devices */
		vd_transfer_operstate(dev,vdev);
		break;

	case NETDEV_CHANGEADDR:
		/* Adjust unicast filters on underlying device */
		flgs = vdev->flags;
		if (!(flgs & IFF_UP))
			return;

		vd_sync_address(dev, vdev);
		break;

	case NETDEV_FEAT_CHANGE:
		/* Propagate device features to underlying device */
		vd_transfer_features(dev, vdev);
		break;

	case NETDEV_DOWN:
		/* Put all VLANs for this dev in the down state too.  */
		flgs = vdev->flags;
		if (!(flgs & IFF_UP))
			return;
		dev_change_flags(vdev, flgs & ~IFF_UP);
		vd_transfer_operstate(dev, vdev);
		break;

	case NETDEV_UP:
		/* Put all Virtual Device for this dev in the up state too.  */
		flgs = vdev->flags;
		if (flgs & IFF_UP)
			return;
		dev_change_flags(vdev, flgs | IFF_UP);
		vd_transfer_operstate(dev, vdev);
		break;

	case NETDEV_UNREGISTER:
		/* Delete all Virtual device for this dev. */
		unregister_vd_dev(vdev);
		break;
	}
}

static int netdev_create_handler(const struct net_device *dev)
{
	int32 netifId = 0,ret;
	netvd_netifType_t type;
	unsigned char mac[ETHER_ADDR_LEN];


	NETVD_MAPPER_CHECK(mapper->hal_netif_create);


	/*get interface id from interface name*/
	netifId = getNetIfIdByDevName(dev->name,&type);

	if(netifId < 0)
		return RT_ERR_INPUT;

	/*create hw netif*/
	memcpy(&mac[0],&dev->dev_addr[0],ETHER_ADDR_LEN);
	ret = mapper->hal_netif_create(type,netifId,mac);

	if(ret == 0)
	{
		netif_usage[netifId] = 1;
	}
	
	return ret;
}

#ifdef CONFIG_PPPOE

static int netdev_ppp_create(const struct net_device *dev)
{
	int32 ret;
	pppInfo_t info;

	/*handle ppp setting*/
	NETVD_MAPPER_CHECK(mapper->hal_netif_ppp_create);
	memset(&info,0,sizeof(pppInfo_t));
	netvd_get_pppInfo_by_devName(dev->name,&info);
	ret = mapper->hal_netif_ppp_create(info.netifId,info.id,info.remote);
	return ret;
}

static int netdev_ppp_del(const struct net_device *dev)
{
	int32 ret;
	pppInfo_t info;

	/*handle ppp setting*/
	NETVD_MAPPER_CHECK(mapper->hal_netif_ppp_create);
	netvd_get_pppInfo_by_devName(dev->name,&info);
	ret = mapper->hal_netif_ppp_create(info.netifId,info.id,info.remote);
	return ret;
}

#endif

static int netdev_mtu_handler(const struct net_device *dev)
{
	int32 netifId = 0,ret;	
	netvd_netifType_t type;
	uint32 mtu = dev->mtu;
	
	NETVD_MAPPER_CHECK(mapper->hal_netif_mtu_set);
	
	netifId = getNetIfIdByDevName(dev->name,&type);

	if(netifId < 0)
		return RT_ERR_INPUT;
	ret = mapper->hal_netif_mtu_set(netifId,mtu);
	
	VD_DEBUG(NETVD_LOG_LEVEL_LOW,"%s,change mtu to %d,iftype %d ret=%d\n",dev->name,mtu,type,ret);

	return ret;
}

static int netdev_mac_handler(const struct net_device *dev)
{
	int32 netifId = 0,ret;	
	netvd_netifType_t type;
	unsigned char mac[ETHER_ADDR_LEN];
	
	NETVD_MAPPER_CHECK(mapper->hal_netif_mac_set);
	
	netifId = getNetIfIdByDevName(dev->name,&type);

	if(netifId < 0)
		return RT_ERR_INPUT;

	memcpy(&mac[0],&dev->dev_addr[0],ETHER_ADDR_LEN);
	ret = mapper->hal_netif_mac_set(netifId,mac);
	
	VD_DEBUG(NETVD_LOG_LEVEL_LOW,"%s, addr=%02X:%02X:%02X:%02X:%02X:%02X, ret=%d\n",
	dev->name,mac[0],mac[1],mac[2],mac[3],mac[4],mac[5],ret);

	return ret;
}


static int netdev_del_handler(const struct net_device *dev)
{
	int32 netifId = 0,ret;	
	netvd_netifType_t type;
	NETVD_MAPPER_CHECK(mapper->hal_netif_delete);
	
	netifId = getNetIfIdByDevName(dev->name,&type);

	if(netifId < 0)
		return RT_ERR_INPUT;
	ret = mapper->hal_netif_delete(netifId);
	
	VD_DEBUG(NETVD_LOG_LEVEL_LOW,"%s, ret=%d\n",dev->name,ret);

	return ret;
}

#if 1

static int32
netdev_vlan_set(const struct net_device *dev)
{
	
	netvd_netifType_t type;	
	int32 ret = 0; 
	uint32 netifId;
	struct smux_dev_info *dev_info;
	if(dev->priv_flags & IFF_VSMUX)
	{
		dev_info = SMUX_DEV_INFO(dev);
		netifId = getNetIfIdByDevName(dev->name,&type);
		VD_DEBUG(NETVD_LOG_LEVEL_LOW,"%s, child dev name %s, vid %d\n",__FUNCTION__,dev->name,dev_info->vid);
		ret = mapper->hal_netif_vid_set(netifId,dev_info->vid,0);	
	}
}

#else
static int netdev_vlan_set(const struct net_device *dev)
{
	struct net_device *cdev;
	struct net_bridge *br = netdev_priv(dev);
	struct net_bridge_port *p, *n;
	struct vd_dev_info *vd; 
	int32 ret = 0; 
	uint32 netifId;
	netvd_netifType_t type;
	
	
	NETVD_MAPPER_CHECK(mapper->hal_netif_vid_set);

	netifId = getNetIfIdByDevName(dev->name,&type);

	/*set hw vlan id*/
	if( br!=NULL){
		list_for_each_entry_safe(p, n, &br->port_list, list) 
		{ 
			cdev = p->dev;
			vd = vd_dev_info(cdev);
			VD_DEBUG(NETVD_LOG_LEVEL_LOW,"%s, child dev name %s, vid %d\n",__FUNCTION__,cdev->name,vd->vlanId);
			ret = mapper->hal_netif_vid_set(netifId,vd->vlanId,0);
		}
	}

	return ret;
}
#endif
static void vd_netifHook_event(struct net_device *dev, unsigned long event){

	int32 ret,netifId;
	netvd_netifType_t type;

	switch (event) {
	case NETDEV_CHANGENAME:
		VD_DEBUG(NETVD_LOG_LEVEL_HIGH,"%s,%s\n",dev->name,"NETDEV_CHANGENAME");
		break;
	case NETDEV_REGISTER:
	{
		VD_DEBUG(NETVD_LOG_LEVEL_HIGH,"%s,%s\n",dev->name,"NETDEV_REGISTER");
		ret = netdev_create_handler(dev);
		break;
	}
	case NETDEV_UNREGISTER:
		VD_DEBUG(NETVD_LOG_LEVEL_HIGH,"%s,%s\n",dev->name,"NETDEV_UNREGISTER");
		ret = netdev_del_handler(dev);
		break;
	case NETDEV_UP:
		VD_DEBUG(NETVD_LOG_LEVEL_HIGH,"%s,%s\n",dev->name,"NETDEV_UP");		
		if(strstr(dev->name,"ppp")){
			ret = netdev_ppp_create(dev);
		}else
		{
			netifId = getNetIfIdByDevName(dev->name,&type);
			if(netif_usage[netifId]!=1)
			{
				ret = netdev_create_handler(dev);				
			}			
			ret = netdev_vlan_set(dev);
		}
		break;
	case NETDEV_DOWN:
		VD_DEBUG(NETVD_LOG_LEVEL_HIGH,"%s,%s\n",dev->name,"NETDEV_DOWN");
		if(strstr(dev->name,"ppp")){
			ret = netdev_ppp_del(dev);
		}
		break;
	case NETDEV_CHANGEADDR:
		VD_DEBUG(NETVD_LOG_LEVEL_LOW,"%s,%s\n",dev->name,"NETDEV_CHANGEADDR");
		ret = netdev_mac_handler(dev);
		break;
	case NETDEV_CHANGEMTU:
		VD_DEBUG(NETVD_LOG_LEVEL_LOW,"%s,%s\n",dev->name,"NETDEV_CHANGEMTU");
		ret = netdev_mtu_handler(dev);
		break;
	}
	return ;
}


static int vd_device_event(struct notifier_block *nb, unsigned long event,void *ptr)
{
	struct net_device *dev = ptr;
	struct list_head *next = NULL, *tmp=NULL;
	vd_group_t  *cur = NULL;

	/*is virtual device*/
 	if (is_vd_dev(dev))
	{
		__vd_device_event(dev, event);
	}

	/*is parent device*/
	list_for_each_safe(next,tmp,&vdhead){
		cur = list_entry(next,vd_group_t,list);
		if(dev == vd_dev_info(cur->dev)->real_dev)
		{
			vd_do_event(event,dev,cur->dev);
		}
	}

	/*if br interface handle br hook function*/
	if(strstr(dev->name,wanName)!=NULL || strstr(dev->name,lanName)!=NULL || strstr(dev->name,"ppp")!=NULL)
	{
		vd_netifHook_event(dev,event);
	}
	return NOTIFY_DONE;
}


/*ip related event*/
static int ip_event(struct notifier_block *nb, unsigned long events, void *ptr)
{
	struct in_ifaddr *ifa = (struct in_ifaddr *) ptr;
	struct net_device *dev = ifa->ifa_dev->dev;	
	netvd_netifType_t type;
	int32 netifId=0;
	int32 ret = 0;
	pppInfo_t pppInfo;
	char gatewayMac[6]="", *pGatewayMac;
		
	NETVD_MAPPER_CHECK(mapper->hal_netif_ip_set);

	if (dev_net(dev) != &init_net)
		goto ip_event_end;

	VD_DEBUG(NETVD_LOG_LEVEL_LOW,"DevName: %s, IPv4: %u.%u.%u.%u, Mask: %d, Gateway: %u.%u.%u.%u \n",
	dev->name,NIPQUAD(ifa->ifa_local),ifa->ifa_prefixlen,NIPQUAD(ifa->ifa_address));

	/*is br interface to configure to HW*/
	if(strstr(dev->name,wanName)!=NULL || strstr(dev->name,lanName)!=NULL)
	{
	
		netifId = getNetIfIdByDevName(dev->name,&type);

	}else if(strstr(dev->name,"ppp")!=NULL)
	{
		memset(&pppInfo,0,sizeof(pppInfo_t));
		if(netvd_get_pppInfo_by_devName(dev->name,&pppInfo)!=VD_ERR_OK)
		{
			goto ip_event_end;
		}
		netifId = pppInfo.netifId;
		if(!checkPPPState())
		{
			goto ip_event_end;
		}
	}else
	{
		goto ip_event_end;
	}

	switch(events){
	case NETDEV_UP:
		VD_DEBUG(NETVD_LOG_LEVEL_LOW,"%s,%s\n",dev->name,"NETDEV_UP");
		pGatewayMac = (ifa->ifa_address!=0 && ifa->ifa_address!= ifa->ifa_local) ? &gatewayMac[0] : NULL;
		ret = mapper->hal_netif_ip_set(netifId,ifa->ifa_local, ifa->ifa_prefixlen,pGatewayMac);
	break;
	case NETDEV_DOWN:
		VD_DEBUG(NETVD_LOG_LEVEL_LOW,"%s,%s\n",dev->name,"NETDEV_DOWN");
		ret = mapper->hal_netif_ip_set(netifId, 0, 0, NULL);
	break;
	}
	VD_DEBUG(NETVD_LOG_LEVEL_LOW,"set ip %u.%u.%u.%u, ret=%d\n",NIPQUAD(ifa->ifa_local),ret);

ip_event_end:
	return NOTIFY_DONE;
}


static int ipv6_event(struct notifier_block *nb, unsigned long events, void *ptr)
{
	struct inet6_ifaddr *ifa = (struct inet6_ifaddr *)ptr;
	struct net_device *dev = ifa->idev->dev;
	int32 netifId=0;
	int32 ret = 0;
	struct in6_addr ip6;
	pppInfo_t pppInfo;	
	netvd_netifType_t type;

	NETVD_MAPPER_CHECK(mapper->hal_netif_ip6_set);
	
	if (dev_net(dev) != &init_net)
		goto ip6_event_end;

	VD_DEBUG(NETVD_LOG_LEVEL_LOW,"DevName: %s, Prefix: %d\n",dev->name,ifa->prefix_len);
	VD_DEBUG(NETVD_LOG_LEVEL_LOW,NIP6QUAD_FMT,NIP6QUAD(ifa->addr));

	/*is br interface to configure to HW*/
	if(strstr(dev->name,wanName)!=NULL || strstr(dev->name,lanName)!=NULL)
	{
		netifId = getNetIfIdByDevName(dev->name,&type);
		
	}else if(strstr(dev->name,"ppp")!=NULL)
	{
		if(netvd_get_pppInfo_by_devName(dev->name,&pppInfo)!=VD_ERR_OK)
		{
			goto ip6_event_end;
		}
		netifId = pppInfo.netifId;
		
	}else
	{
		goto ip6_event_end;
	}
	/*check if link local address, should not add to hw asic*/
	if(is_ip6_link_local(ifa->addr,ifa->prefix_len))
	{
		goto ip6_event_end;
	}
		
	switch(events){
	case NETDEV_UP:
		VD_DEBUG(NETVD_LOG_LEVEL_LOW,"%s,%s\n",dev->name,"NETDEV_UP");
		ret = mapper->hal_netif_ip6_set(netifId,ifa->addr, ifa->prefix_len);
	break;
	case NETDEV_DOWN:
		memset(&ip6,0,sizeof(struct in6_addr));
		VD_DEBUG(NETVD_LOG_LEVEL_LOW,"%s,%s\n",dev->name,"NETDEV_DOWN");
		ret = mapper->hal_netif_ip6_set(netifId, ip6, 0);
	break;
	}

ip6_event_end:
	return NOTIFY_DONE;
}


/*conntrack related event handle*/
static int ct_list_add(uint32 naptId,struct nf_conn *ct){

	ct_hw_t *entry = NULL;

	entry = (ct_hw_t*) kmalloc(sizeof(ct_hw_t),GFP_KERNEL);

	if(!entry)
	{
		return VD_ERR_ADD_FAILED;
	}
	memset(entry,0,sizeof(ct_hw_t));
	entry->naptId = naptId;
	entry->ct = ct;
	list_add(&entry->list,&cthead);
	return VD_ERR_OK;

}


static void ct_list_del(const struct nf_conn *ct)
{

	struct list_head *next = NULL, *tmp=NULL;
	ct_hw_t  *cur = NULL;

	list_for_each_safe(next,tmp,&cthead){
	
		cur = list_entry(next,ct_hw_t,list);
		
	    	if(cur->ct == ct){
			list_del(&cur->list);
			kfree(cur);
		}
	}
	return;
}


/*include software and hw*/
void ct_list_clear(void)
{

	struct list_head *next = NULL, *tmp=NULL;
	ct_hw_t  *cur = NULL;

	printk("start clear\n");

	list_for_each_safe(next,tmp,&cthead){
	
		cur = list_entry(next,ct_hw_t,list);
		
	    	if(cur!=NULL){
			nf_ct_destroy((struct nf_conntrack*)cur->ct);
			/*destory event will call ct_list_del*/
		}
	}
	return;
}

static void refresh_ct(struct nf_conn *ct, unsigned int extra_jiffies)
{
  	NF_CT_ASSERT(ct->timeout.data == (unsigned long)ct);
	spin_lock_bh(&nf_conntrack_lock);

	if (!nf_ct_is_confirmed(ct))	
		ct->timeout.expires = extra_jiffies;
	else {
		/* Need del_timer for race avoidance (may already be dying). */
		if (del_timer(&ct->timeout)) {
			ct->timeout.expires = jiffies + extra_jiffies;
			add_timer(&ct->timeout);			
		}
	}
	spin_unlock_bh(&nf_conntrack_lock);
}

/*update connection tracking timer for HW acceleration*/
unsigned int ctState[CT_ENTRY_MAX];

int update_ct(void){

	struct list_head *next = NULL, *tmp=NULL;
	ct_hw_t  *cur = NULL;
	struct nf_conn *ct;
	unsigned int tcpTimeOut, udpTimeOut;
	int ret;

	NETVD_MAPPER_CHECK(mapper->hal_ct_tf_get_all);
	
	tcpTimeOut =*(unsigned int*) (nf_conntrack_l4proto_tcp4.ctl_table[TCP_CONNTRACK_ESTABLISHED-1].data);
	udpTimeOut =*(unsigned int*) ( nf_conntrack_l4proto_udp4.ctl_table[1].data);

	VD_DEBUG(NETVD_LOG_LEVEL_HIGH,"tcp_timeout=%d,upd_timeout=%d\n",tcpTimeOut,udpTimeOut);

	ret = mapper->hal_ct_tf_get_all(ctState);

	if(ret!=VD_ERR_OK)
		return ret;

	list_for_each_safe(next,tmp,&cthead){

		cur = list_entry(next,ct_hw_t,list);
		if(ctState[cur->naptId]==ENABLED)
		{
			ct = cur->ct;
			if(ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.dst.protonum == IPPROTO_TCP){
				refresh_ct(ct,tcpTimeOut);
				VD_DEBUG(NETVD_LOG_LEVEL_HIGH,"UPDATE Timeout for NAPT[%d],Timeout: %d\n",cur->naptId,tcpTimeOut);

			}else
			{
				refresh_ct(ct,udpTimeOut);
				VD_DEBUG(NETVD_LOG_LEVEL_HIGH,"UPDATE Timeout for NAPT[%d],Timeout: %d\n",cur->naptId,udpTimeOut);
			}
		}
	}

	return VD_ERR_OK;

}

static int conntrack_new_event( struct nf_conn *ct)
{
	uint8 proto;
	int32 ret;
	int naptId;
	int dir;
	
	NETVD_MAPPER_CHECK(mapper->hal_ct_add);
		
	/*check if for local accept and conntinue*/
	if (ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.src.u3.ip == 0)
		return VD_ERR_OK;

	proto = ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.dst.protonum;
	/*check protocol number should tcp or udp*/
	if(proto  != IPPROTO_TCP && proto != IPPROTO_UDP)
		return VD_ERR_OK;

#if defined(CONFIG_NF_CONNTRACK_MARK)
	VD_DEBUG(NETVD_LOG_LEVEL_HIGH,"connmark: %x\n",ct->mark);
	if(!(ct->mark & CONNTRACK_ADD_MARK))
		return VD_ERR_OK;
#endif
	dir = ct->mark & (CT_DIR_UPSTREAM | CT_DIR_DOWNSTREAM);
	/*start to assing needed parameters*/
	ret = mapper->hal_ct_add(ct,&naptId,dir);
	/*add for checking list*/
	if(ret==VD_ERR_OK){
		ct_list_add(naptId,ct);
	}

	return ret;
}

static int conntrack_destory_event( struct nf_conn *ct)
{
	uint8 proto;
	int ret;
	
 	NETVD_MAPPER_CHECK(mapper->hal_ct_del);
		
	/*check if for local accept and conntinue*/
	if (ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.src.u3.ip == 0)
		return VD_ERR_OK;

	proto 	= ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.dst.protonum;
	/*check protocol number should tcp or udp*/
	if(proto  != IPPROTO_TCP && proto != IPPROTO_UDP)
		return VD_ERR_OK;

#if defined(CONFIG_NF_CONNTRACK_MARK)
	VD_DEBUG(NETVD_LOG_LEVEL_HIGH,"connmark: %x\n",ct->mark);
	if(!(ct->mark & CONNTRACK_ADD_MARK))
		return VD_ERR_OK;
#endif

	/*start to assing needed parameters*/
	ret = mapper->hal_ct_del(ct);

	/*remove checking list*/
	if(ret == VD_ERR_OK)
	{
		ct_list_del(ct);
	}
	
	VD_DEBUG(NETVD_LOG_LEVEL_LOW,"delete ct ret=%d\n",ret);
	
	return ret;
}


static int conntrack_show(const struct nf_conn *ct)
{
	uint8 proto;
	uint32 srcIp,dstIp,srcport,dstport,natIp,natport;

		
	/*check if for local accept and conntinue*/
	if (ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.src.u3.ip == 0)
		return VD_ERR_OK;

	proto 	= ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.dst.protonum;
	/*check protocol number should tcp or udp*/
	if(proto  != IPPROTO_TCP && proto != IPPROTO_UDP)
		return VD_ERR_OK;

#if defined(CONFIG_NF_CONNTRACK_MARK)
	VD_DEBUG(NETVD_LOG_LEVEL_HIGH,"connmark: %x\n",ct->mark);
#endif

	/*start to assing needed parameters*/
	srcIp 	= ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.src.u3.ip;
	srcport   = ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.src.u.tcp.port;
	dstIp 	= ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.dst.u3.ip;
	dstport 	= ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.dst.u.tcp.port;

	natIp 	= ct->tuplehash[IP_CT_DIR_REPLY].tuple.dst.u3.ip;
	natport	= ct->tuplehash[IP_CT_DIR_REPLY].tuple.dst.u.tcp.port;

	VD_DEBUG(NETVD_LOG_LEVEL_HIGH,"srcIp=%u.%u.%u.%u,srcPort = %d\n",NIPQUAD(srcIp),srcport);
	VD_DEBUG(NETVD_LOG_LEVEL_HIGH,"dstIp=%u.%u.%u.%u,dstPort = %d\n",NIPQUAD(dstIp),dstport);
	VD_DEBUG(NETVD_LOG_LEVEL_HIGH,"natIp=%u.%u.%u.%u,natPort = %d\n",NIPQUAD(natIp),natport);
	VD_DEBUG(NETVD_LOG_LEVEL_HIGH,"proto=%d\n",proto);
	VD_DEBUG(NETVD_LOG_LEVEL_HIGH,"otherIp=%u.%u.%u.%u,otherPort=%d\n",NIPQUAD(ct->tuplehash[IP_CT_DIR_REPLY].tuple.src.u3.ip),
	ct->tuplehash[IP_CT_DIR_REPLY].tuple.src.u.tcp.port);

	return VD_ERR_OK;
}

static int conntrack_event(struct notifier_block *nb, unsigned long events, void *ptr)
{

	struct nf_ct_event *item = (struct nf_ct_event *)ptr;
	struct nf_conn *ct = item->ct;
	int32 ret;
 

	/* ignore our fake conntrack entry */
	if (ct == &nf_conntrack_untracked)
		goto  conn_end;

	if (events & IPCT_DESTROY) {
		ret = conntrack_destory_event(ct);
		VD_DEBUG(NETVD_LOG_LEVEL_HIGH,"%s\n","NFNLGRP_CONNTRACK_DESTROY");
	} else  
#if defined(CONFIG_NF_CONNTRACK_MARK)
	if(events & IPCT_MARK){
		ret = conntrack_new_event(ct);
		VD_DEBUG(NETVD_LOG_LEVEL_HIGH,"%s, ret=%d\n","NFNLGRP_CONNTRACK_MARK",ret);
	}else
#else
	if (events & (IPCT_NEW | IPCT_RELATED)) {
		ret = conntrack_new_event(ct);
		VD_DEBUG(NETVD_LOG_LEVEL_HIGH,"%s, ret=%d\n","NFNLGRP_CONNTRACK_NEW",ret);
	} else
#endif
	if (events & (IPCT_STATUS | IPCT_PROTOINFO)) {
		VD_DEBUG(NETVD_LOG_LEVEL_HIGH,"%s\n","NFNLGRP_CONNTRACK_UPDATE");
		conntrack_show(ct);
	} else 
	if(events & IPCT_REFRESH) {
		VD_DEBUG(NETVD_LOG_LEVEL_HIGH,"%s\n","NFNLGRP_CONNTRACK_REFRESH");
		conntrack_show(ct);
	}else
	if(events & IPCT_PROTOINFO_VOLATILE){
		VD_DEBUG(NETVD_LOG_LEVEL_HIGH,"%s\n","NFNLGRP_CONNTRACK_PROTOINFO_VOLATILE");
		conntrack_show(ct);
	}

conn_end:
	return NOTIFY_DONE;
}

/*neighbor related event handler*/
static int ipv4_arp_handler(uint32 saddr,unsigned char *smac, int isAdd)
{
	int ret;

	NETVD_MAPPER_CHECK(mapper->hal_arp_add);
	NETVD_MAPPER_CHECK(mapper->hal_arp_del);
	
	if(isAdd){
		ret = mapper->hal_arp_add(saddr,smac);
	}else{
		ret = mapper->hal_arp_del(saddr);
	}
	VD_DEBUG(NETVD_LOG_LEVEL_LOW,"Update ARP Entry (SIP,SMAC) = { %u.%u.%u.%u, %02X:%02X:%02X:%02X:%02X:%02X}, ret=%d\n",
	NIPQUAD(saddr),smac[0],smac[1],smac[2],smac[3],smac[4],smac[5],ret);

	return ret;
}

static int ipv6_neighbor_handler(struct in6_addr *ip6addr ,unsigned char *smac, int isAdd)
{
	int ret;
	VD_DEBUG(NETVD_LOG_LEVEL_LOW,"MAC:%02X:%02X:%02X:%02X:%02X:%02X",
	smac[0],smac[1],smac[2],smac[3],smac[4],smac[5]);
	VD_DEBUG(NETVD_LOG_LEVEL_LOW,NIP6QUAD_FMT,NIP6QUAD(*ip6addr));


	NETVD_MAPPER_CHECK(mapper->hal_neigh_add);
	NETVD_MAPPER_CHECK(mapper->hal_neigh_del);

	if(isAdd){
		ret = mapper->hal_neigh_add(*ip6addr,smac);
	}else{
		ret = mapper->hal_neigh_del(*ip6addr);
	}
	return VD_ERR_OK;
}

static int neighbor_update(struct neighbour *neigh){

	int32 ret,i,isAdd=0;
	unsigned char *smac;

	smac = neigh->ha;

	for (i = 0; i < 6; i++){
		if(smac[i]!=0)
			isAdd=1;
	}
	/*check ipv6 or ipv4*/
	switch(neigh->tbl->family){
	case AF_INET:
	{
		if((ret = ipv4_arp_handler(*(u32*)neigh->primary_key, smac,isAdd))!=VD_ERR_OK)
		{
			VD_DEBUG(NETVD_LOG_LEVEL_LOW,"%s() failed!,ret =%d\n","ipv4_arp_handler",ret);
			return ret;
		}
	}
	break;
	case AF_INET6:
	{
		if((ret = ipv6_neighbor_handler((struct in6_addr*)&neigh->primary_key,smac,isAdd))!=VD_ERR_OK)
		{
			VD_DEBUG(NETVD_LOG_LEVEL_LOW,"%s() failed!,ret =%d\n","ipv6_neighbor_handler",ret);
			return ret;
		}
	}
	break;
	default:
			VD_DEBUG(NETVD_LOG_LEVEL_LOW,"%s() failed!,ret =%d\n",__FUNCTION__,ret);
			return VD_ERR_FAILED;
	break;
	}
	return VD_ERR_OK;
}


static int net_event(struct notifier_block *nb, unsigned long event,void *ptr)
{
	int32 ret;
	
	switch (event){
	case NETEVENT_NEIGH_UPDATE:
	{
		ret = neighbor_update((struct neighbour*)ptr);
	}
	break;
	case NETEVENT_PMTU_UPDATE:
		VD_DEBUG(NETVD_LOG_LEVEL_HIGH,"%s\n","NETEVENT_PMTU_UPDATE");
	break;
	case NETEVENT_REDIRECT:
		VD_DEBUG(NETVD_LOG_LEVEL_HIGH,"%s\n","NETEVENT_REDIRECT");
	break;
	}
      return NOTIFY_DONE;
}

static int
l2addr_get_by_ip(uint32 ipaddr,struct net_device* dev,unsigned char *haddr)
{
	struct neighbour *n;
	
	n = neigh_lookup(&arp_tbl, &ipaddr,dev);

	if(!n)
	{
		return VD_ERR_ENTRY_NOTFOUNT;
	}
	memcpy(haddr, n->ha,ETH_ALEN);
	VD_DEBUG(NETVD_LOG_LEVEL_LOW,"%s(): MAC=%02X:%02X:%02X:%02X:%02X:%02X\n",__FUNCTION__,haddr[0],haddr[1],haddr[2],haddr[3],haddr[4],haddr[5]);
	return VD_ERR_OK;
}


static int
ip_mroute_update(void)
{
	int i=0;
	int32 ret,netifId;
	gwInfo_t gwInfo;	
	netvd_netifType_t type;
	unsigned char mac[ETH_ALEN]="";
	struct net_device *dev;
	
	NETVD_MAPPER_CHECK(mapper->hal_netif_gw_set);
	NETVD_MAPPER_CHECK(mapper->hal_arp_get);

	/*need to update more routing table for policy route, 1~8*/
	for(i=1; i<8 ; i++)
	{	
		memset(&gwInfo,0,sizeof(gwInfo_t));
		if((ret = netvd_get_defaultGw(i,&gwInfo))!=VD_ERR_OK)
		{
			netifId = i;
			memset(&mac[0],0,ETHER_ADDR_LEN);
			VD_DEBUG(NETVD_LOG_LEVEL_LOW,"%s(): DELETE ROUTE TABLE %d DEFAULT GW\n",__FUNCTION__,i);
			
		}else{
		
			netifId = getNetIfIdByDevName(gwInfo.devName,&type);
			dev = dev_get_by_name(&init_net,gwInfo.devName);
			
			if((ret = l2addr_get_by_ip(gwInfo.gwIp,dev,&mac[0]))!=VD_ERR_OK)
			{
				memset(&mac[0],0,ETHER_ADDR_LEN);
				VD_DEBUG(NETVD_LOG_LEVEL_LOW,"%s(): DELETE ROUTE TABLE %d DEFAULT GW\n",__FUNCTION__,i);
			}else{
				VD_DEBUG(NETVD_LOG_LEVEL_LOW,"UPDATE ROUTE TABLE %d DEFAULT GW, DEV=%s,GWIP=%u.%u.%u.%u\n",
				i,gwInfo.devName,NIPQUAD(gwInfo.gwIp));
				VD_DEBUG(NETVD_LOG_LEVEL_LOW,"MAC=%02X:%02X:%02X:%02X:%02X:%02X\n",
				mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
			}
			
		}
		ret = mapper->hal_netif_gw_set(netifId,mac);
	}
	return VD_ERR_OK;
}

static int
ip_defaultRoute_update(void)
{
	int32 ret,netifId;
	gwInfo_t gwInfo;
	unsigned char mac[ETH_ALEN]=""; 
	netvd_netifType_t type;
	pppInfo_t info; 
	struct net_device *dev;
	uint32 ipaddr,mask;	

	NETVD_MAPPER_CHECK(mapper->hal_route_add);
	NETVD_MAPPER_CHECK(mapper->hal_route_del);
	
	memset(&gwInfo,0,sizeof(gwInfo_t));
	/*update main route table for default route*/
	if((ret = netvd_get_defaultGw(RT_TABLE_MAIN,&gwInfo))!=VD_ERR_OK)
	{
		VD_DEBUG(NETVD_LOG_LEVEL_LOW,"%s(): DELETE DEFAULT GW\n",__FUNCTION__);
		ret = mapper->hal_route_del(0,0);
	}else{

		VD_DEBUG(NETVD_LOG_LEVEL_LOW,"UPDATE DEFAULT GW, DEV=%s,GWIP=%u.%u.%u.%u\n",
		gwInfo.devName,NIPQUAD(gwInfo.gwIp));
		if(strstr(gwInfo.devName,wanName)!=NULL)
		{
			netifId = getNetIfIdByDevName(gwInfo.devName,&type);
			memset(&mac[0],0,ETH_ALEN);
			dev = dev_get_by_name(&init_net,gwInfo.devName);
			l2addr_get_by_ip(gwInfo.gwIp,dev,&mac[0]);
		}else
		{			
			netifId = getPPPoeIdByDevName(gwInfo.devName);
			ret = netvd_get_pppInfo_by_devName(gwInfo.devName,&info);
			memcpy(&mac[0],&info.remote[0],ETH_ALEN);
		}
		ret = mapper->hal_route_add(netifId,0,0,mac);
		VD_DEBUG(NETVD_LOG_LEVEL_LOW,"netifId =%d,MAC=%02X:%02X:%02X:%02X:%02X:%02X\n",netifId,
		mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);

		ret = mapper->hal_netif_ip4addr_get(netifId,&ipaddr,&mask);
		ret = mapper->hal_route_add(netifId,ipaddr,mask,NULL);
	}
	return VD_ERR_OK;
}

static int
ip_staticRoute_update(void)
{
	int32 ret,netifId,i;
	uint32 ipaddr,mask;	
	pppInfo_t info; 	
	netvd_netifType_t type;
	struct net_device *dev;
	unsigned char mac[ETH_ALEN]=""; 
	gwInfo_t gwInfo;
	
	NETVD_MAPPER_CHECK(mapper->hal_netif_ip4addr_get);

	/*update main route table for static route*/
	for(i=NETVD_WAN_START;i<NETVD_WAN_STOP;i++)
	{
		ret = mapper->hal_netif_ip4addr_get(i,&ipaddr,&mask);
		if(ipaddr!=0 && mask!=0)
		{
			memset(&gwInfo,0,sizeof(gwInfo_t));
			if((ret = netvd_get_gw(RT_TABLE_MAIN,ipaddr,&gwInfo))!=VD_ERR_OK)
			{
				VD_DEBUG(NETVD_LOG_LEVEL_LOW,"%s(): DELETE GW\n",__FUNCTION__);
				ret = mapper->hal_route_del(ipaddr,mask);
			}else
			{
				VD_DEBUG(NETVD_LOG_LEVEL_LOW,"UPDATE GW, DEV=%s,GWIP=%u.%u.%u.%u\n",
				gwInfo.devName,NIPQUAD(gwInfo.gwIp));

				if(gwInfo.gwIp!=0)
				{
					if(strstr(gwInfo.devName,wanName)!=NULL)
					{
						netifId = getNetIfIdByDevName(gwInfo.devName,&type);
						memset(&mac[0],0,ETH_ALEN);
						dev = dev_get_by_name(&init_net,gwInfo.devName);
						l2addr_get_by_ip(gwInfo.gwIp,dev,&mac[0]);
					}else
					{			
						netifId = getPPPoeIdByDevName(gwInfo.devName);
						ret = netvd_get_pppInfo_by_devName(gwInfo.devName,&info);
						memcpy(&mac[0],&info.remote[0],ETH_ALEN);
					}
					ret = mapper->hal_route_add(netifId,ipaddr,mask,mac);
					VD_DEBUG(NETVD_LOG_LEVEL_LOW,"netifId =%d,MAC=%02X:%02X:%02X:%02X:%02X:%02X\n",netifId,
					mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
				}
			}

		}
	}
	return VD_ERR_OK;

}

static int 
ip_route_update(void)
{
	int ret;
	ret = ip_defaultRoute_update();
	if(ret!=VD_ERR_OK)
	{
		VD_DEBUG(NETVD_LOG_LEVEL_LOW,"update default route failed!");
	}
	ret = ip_staticRoute_update();
	if(ret!=VD_ERR_OK)
	{
		VD_DEBUG(NETVD_LOG_LEVEL_LOW,"update static route failed!");
	}
	/*
	ret = ip_mroute_update();
	if(ret!=VD_ERR_OK)
	{
		VD_DEBUG(NETVD_LOG_LEVEL_LOW,"update multiple  route failed!");
	}
	*/
	
	return ret;
}

static int 
ip6_route_update(void)
{
	int32 ret,netifId;
	gw6Info_t gwInfo;	
	netvd_netifType_t type;
	unsigned char mac[ETH_ALEN]="";
	struct in6_addr n;

	NETVD_MAPPER_CHECK(mapper->hal_route6_add);
	NETVD_MAPPER_CHECK(mapper->hal_route6_del);
	NETVD_MAPPER_CHECK(mapper->hal_neigh_get);
	
	memset(&gwInfo,0,sizeof(gwInfo_t));
	memset(&n,0,sizeof(struct in6_addr));

	if((ret = netvd_get_ip6defaultGw(&gwInfo))!=VD_ERR_OK)
	{
		VD_DEBUG(NETVD_LOG_LEVEL_LOW,"%s(): DELETE DEFAULT GW\n",__FUNCTION__);
		ret = mapper->hal_route6_del(n,0);
	}else{
		VD_DEBUG(NETVD_LOG_LEVEL_LOW,"UPDATE DEFAULT GW:");
		VD_DEBUG(NETVD_LOG_LEVEL_LOW,NIP6QUAD_FMT,NIP6QUAD(gwInfo.gwIp));
		VD_DEBUG(NETVD_LOG_LEVEL_LOW,"DevName: %s\n",gwInfo.devName);
		netifId = getNetIfIdByDevName(gwInfo.devName,&type);
		memset(&mac[0],0,ETH_ALEN);
		ret = mapper->hal_neigh_get( gwInfo.gwIp,&mac[0]);
		if(ret !=VD_ERR_OK) return ret;
		ret = mapper->hal_route6_add(netifId,n,0,mac);
	}

	return ret;
}



static int
netlink_event(struct notifier_block *this,
		 unsigned long event, void *ptr)
{
	struct netlink_notify *n = ptr;
		
	switch(n->protocol ){
	case NETLINK_ROUTE:
	{
		VD_DEBUG(NETVD_LOG_LEVEL_LOW,"%s(),Protocol= NETLINK_ROUTE ,PID=%d\n",__FUNCTION__,n->pid);
		ip_route_update();
		ip6_route_update();
	}
	break;
	case NETLINK_FIREWALL:
		VD_DEBUG(NETVD_LOG_LEVEL_LOW,"%s(),Protocol= NETLINK_FIREWALL ,PID=%d\n",__FUNCTION__,n->pid);
	default:
	break;
	}
	return NOTIFY_DONE;
}

static int chip_ponPort_get(void)
{
	//NETVD_MAPPER_CHECK(mapper->hal_ponPort_get);
	//mapper->hal_ponPort_get(&ponPort);
	ponPort = 4;
	return VD_ERR_OK;
}


/*event register block*/

static struct notifier_block vd_notifier_block __read_mostly = {
	.notifier_call = vd_device_event,
};

static struct notifier_block ip_notifier_block __read_mostly= {
	.notifier_call = ip_event,
};

static struct notifier_block conntrack_notifier_block __read_mostly = {
	.notifier_call	= conntrack_event,
};

static struct notifier_block net_notifier_block __read_mostly = {
	.notifier_call	= net_event,
};

static struct notifier_block ipv6_notifier_block __read_mostly = {
	.notifier_call	= ipv6_event,
};

static struct notifier_block netlink_notifier_block __read_mostly = {
	.notifier_call	= netlink_event,
};


int netvd_event_init(void){

	int err;
	/*init ct list*/
	INIT_LIST_HEAD(&cthead);
	/*init mapper*/
	mapper = NULL;
	/*register netdev event*/
	err = register_netdevice_notifier(&vd_notifier_block);
	if (err < 0)
		goto err1;

	/*register ipaddr event*/
	err = register_inetaddr_notifier(&ip_notifier_block);
	if (err < 0)
		goto err2;
	/*ip conntrack event*/
	err = nf_conntrack_register_notifier(&conntrack_notifier_block);
	if (err < 0)
		goto err3;
	/*arp event*/
	err= register_netevent_notifier(&net_notifier_block);
	if (err < 0)
		goto err4;
	err = register_inet6addr_notifier(&ipv6_notifier_block);
	if(err < 0)
		goto err5;
	/*register route event*/
	err = netlink_register_notifier(&netlink_notifier_block);
	if(err < 0)
		goto err6;
	/*register default mapper*/
       netvd_default_map_init();
	/*init hw nat*/
	if(mapper->hal_hwnat_init)
		mapper->hal_hwnat_init();
	/*get ponPort value*/
#ifdef CONFIG_GPON_FEATURE
	chip_ponPort_get();
#endif	
	return VD_ERR_OK;
err6:
	unregister_inet6addr_notifier(&ipv6_notifier_block);
err5:
	unregister_netevent_notifier(&net_notifier_block);
err4:
	nf_conntrack_unregister_notifier(&conntrack_notifier_block);
err3:
	unregister_inetaddr_notifier(&ip_notifier_block);
err2:
	unregister_netdevice_notifier(&vd_notifier_block);
err1:
	return VD_ERR_ADD_FAILED;
}


void netvd_event_exit(void){
	
	nf_conntrack_unregister_notifier(&conntrack_notifier_block);
	unregister_inetaddr_notifier(&ip_notifier_block);
	unregister_netdevice_notifier(&vd_notifier_block);
	unregister_netevent_notifier(&net_notifier_block);
	unregister_inet6addr_notifier(&ipv6_notifier_block);
	netlink_unregister_notifier(&netlink_notifier_block);
}
