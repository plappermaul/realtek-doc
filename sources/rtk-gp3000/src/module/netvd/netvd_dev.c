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

#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/ethtool.h>
#include <module/netvd/netvd.h>
#if defined(CONFIG_RTL9600_SERIES)
#include "re8686.h"
#elif defined(CONFIG_RTL9601B_SERIES)
#include "re8686_rtl9601b.h"
#elif defined(CONFIG_RTL9602C_SERIES)
#include "re8686_rtl9602c.h"
#else
#include "re8686_nic.h"
#endif

int ponPort=-1;

__IRAM_NIC static struct net_device* decideRxDevice(struct net_device *real_dev,struct rx_info *pRxInfo){
	unsigned int port = pRxInfo->opts3.bit.src_port_num ;
	unsigned int vid    = pRxInfo->opts2.bit.cvlan_tag;
	return __find_vd_dev(real_dev,vid, port);
}

/*cp is the NIC private data*/
int vd_rx_skb (struct re_private *cp, struct sk_buff *skb,struct rx_info *pRxInfo)
{
	struct net_device_stats *stats;
	rcu_read_lock();
	skb->dev = decideRxDevice(cp->dev,pRxInfo);
	if(!skb->dev || !(skb->dev->flags & IFF_UP) )
		return RE8670_RX_CONTINUE;
	
	skb->vlan_tci =0;

	skb->from_dev=skb->dev;
	skb->protocol = eth_type_trans (skb, skb->dev);
	skb->dev->last_rx = jiffies;
	cp->cp_stats.rx_sw_num++;
	stats = &skb->dev->stats;
	stats->rx_packets++;
	stats->rx_bytes += skb->len;
	if (netif_rx(skb) == NET_RX_DROP){
		goto vd_rx_drop;
	}
	rcu_read_unlock();
	return RE8670_RX_STOP_SKBNOFREE;
vd_rx_drop:
	stats->rx_errors++;
	rcu_read_unlock();
	return RE8670_RX_CONTINUE;
}


__IRAM_NIC int vd_dev_hard_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
	int32 ret;
	struct net_device_stats *stats = &dev->stats;
	struct tx_info ptxInfo;
	struct vd_dev_info *pri = vd_dev_info(dev);
	
	stats->tx_packets++;
	stats->tx_bytes += skb->len;
	skb->dev = pri->real_dev;
	
	memset(&ptxInfo, 0, sizeof(struct tx_info));
	ptxInfo.opts2.bit.cputag  = 1;
	//ptxInfo.opts2.bit.efid  = 1;
	ptxInfo.opts2.bit.enhance_fid  = 0;
	//ptxInfo.opts3.bit.tx_portmask  = (1 << pri->port);
	
	if(pri->vlanId!=0){
		ptxInfo.opts2.bit.tx_vlan_action = TXD_VLAN_INSERT;
		ptxInfo.opts2.bit.vidl = pri->vlanId & 0xff;
		ptxInfo.opts2.bit.vidh = pri->vlanId >>8  & 0xf;
		ptxInfo.opts2.bit.cfi = 0;
		ptxInfo.opts2.bit.prio = pri->pbit;
	}
	
	if(ponPort ==  pri->port){ 
		ptxInfo.opts3.bit.tx_dst_stream_id  = pri->flowId[0];
	}
	ret = re8686_send_with_txInfo(skb, &ptxInfo, 0);
	if(ret != RT_ERR_OK){
		stats->tx_errors++;
		return RT_ERR_FAILED;
	}
	return NETDEV_TX_OK;
}


static int vd_dev_change_mtu(struct net_device *dev, int new_mtu)
{

	if (vd_dev_info(dev)->real_dev->mtu < new_mtu)
		return -ERANGE;

	dev->mtu = new_mtu;

	return 0;
}


int vd_dev_change_flags(const struct net_device *dev, u32 flags, u32 mask)
{
	struct vd_dev_info *vd = vd_dev_info(dev);
	u32 old_flags = vd->flags;

	if (mask & ~(VD_FLAG_REORDER_HDR))
		return -EINVAL;

	vd->flags = (old_flags & ~mask) | (flags & mask);

	return 0;
}

void vd_dev_get_realdev_name(const struct net_device *dev, char *result)
{
	strncpy(result, vd_dev_info(dev)->real_dev->name, 23);
}

static int vd_dev_open(struct net_device *dev)
{
	struct vd_dev_info *vd = vd_dev_info(dev);
	struct net_device *real_dev = vd->real_dev;
	int err;

	if (!(real_dev->flags & IFF_UP))
		return -ENETDOWN;

	if (compare_ether_addr(dev->dev_addr, real_dev->dev_addr)) {
		err = dev_unicast_add(real_dev, dev->dev_addr, ETH_ALEN);
		if (err < 0)
			goto out;
	}

	if (dev->flags & IFF_ALLMULTI) {
		err = dev_set_allmulti(real_dev, 1);
		if (err < 0)
			goto del_unicast;
	}
	if (dev->flags & IFF_PROMISC) {
		err = dev_set_promiscuity(real_dev, 1);
		if (err < 0)
			goto clear_allmulti;
	}

	memcpy(vd->real_dev_addr, real_dev->dev_addr, ETH_ALEN);

	netif_carrier_on(dev);
	return 0;

clear_allmulti:
	if (dev->flags & IFF_ALLMULTI)
		dev_set_allmulti(real_dev, -1);
del_unicast:
	if (compare_ether_addr(dev->dev_addr, real_dev->dev_addr))
		dev_unicast_delete(real_dev, dev->dev_addr, ETH_ALEN);
out:
	netif_carrier_off(dev);
	return err;
}

static int vd_dev_stop(struct net_device *dev)
{
	struct vd_dev_info *vd = vd_dev_info(dev);
	struct net_device *real_dev = vd->real_dev;


	dev_mc_unsync(real_dev, dev);
	dev_unicast_unsync(real_dev, dev);
	if (dev->flags & IFF_ALLMULTI)
		dev_set_allmulti(real_dev, -1);
	if (dev->flags & IFF_PROMISC)
		dev_set_promiscuity(real_dev, -1);

	if (compare_ether_addr(dev->dev_addr, real_dev->dev_addr))
		dev_unicast_delete(real_dev, dev->dev_addr, dev->addr_len);

	netif_carrier_off(dev);
	return 0;
}

static int vd_dev_set_mac_address(struct net_device *dev, void *p)
{
	struct net_device *real_dev = vd_dev_info(dev)->real_dev;
	struct sockaddr *addr = p;
	int err;

	if (!is_valid_ether_addr(addr->sa_data))
		return -EADDRNOTAVAIL;

	if (!(dev->flags & IFF_UP))
		goto out;

	if (compare_ether_addr(addr->sa_data, real_dev->dev_addr)) {
		err = dev_unicast_add(real_dev, addr->sa_data, ETH_ALEN);
		if (err < 0)
			return err;
	}

	if (compare_ether_addr(dev->dev_addr, real_dev->dev_addr))
		dev_unicast_delete(real_dev, dev->dev_addr, ETH_ALEN);

out:
	memcpy(dev->dev_addr, addr->sa_data, ETH_ALEN);
	return 0;
}

static int vd_dev_ioctl(struct net_device *dev, struct ifreq *ifr, int cmd)
{
	struct net_device *real_dev = vd_dev_info(dev)->real_dev;
	const struct net_device_ops *ops = real_dev->netdev_ops;
	struct ifreq ifrr;
	int err = -EOPNOTSUPP;

	strncpy(ifrr.ifr_name, real_dev->name, IFNAMSIZ);
	ifrr.ifr_ifru = ifr->ifr_ifru;

	switch (cmd) {
	case SIOCGMIIPHY:
	case SIOCGMIIREG:
	case SIOCSMIIREG:
		if (netif_device_present(real_dev) && ops->ndo_do_ioctl)
			err = ops->ndo_do_ioctl(real_dev, &ifrr, cmd);
		break;
	}

	if (!err)
		ifr->ifr_ifru = ifrr.ifr_ifru;

	return err;
}

static int vd_dev_neigh_setup(struct net_device *dev, struct neigh_parms *pa)
{
	struct net_device *real_dev = vd_dev_info(dev)->real_dev;
	const struct net_device_ops *ops = real_dev->netdev_ops;
	int err = 0;

	if (netif_device_present(real_dev) && ops->ndo_neigh_setup)
		err = ops->ndo_neigh_setup(real_dev, pa);

	return err;
}

static void vd_dev_change_rx_flags(struct net_device *dev, int change)
{
	struct net_device *real_dev = vd_dev_info(dev)->real_dev;

	if (change & IFF_ALLMULTI)
		dev_set_allmulti(real_dev, dev->flags & IFF_ALLMULTI ? 1 : -1);
	if (change & IFF_PROMISC)
		dev_set_promiscuity(real_dev, dev->flags & IFF_PROMISC ? 1 : -1);
}

static void vd_dev_set_rx_mode(struct net_device *vd_dev)
{
	dev_mc_sync(vd_dev_info(vd_dev)->real_dev, vd_dev);
	dev_unicast_sync(vd_dev_info(vd_dev)->real_dev, vd_dev);
}


static struct lock_class_key vd_netdev_xmit_lock_key;
static struct lock_class_key vd_netdev_addr_lock_key;

static void vd_dev_set_lockdep_one(struct net_device *dev,
				     struct netdev_queue *txq,
				     void *_subclass)
{
	lockdep_set_class_and_subclass(&txq->_xmit_lock,
				       &vd_netdev_xmit_lock_key,
				       *(int *)_subclass);
}

static void vd_dev_set_lockdep_class(struct net_device *dev, int subclass)
{
	lockdep_set_class_and_subclass(&dev->addr_list_lock,
				       &vd_netdev_addr_lock_key,
				       subclass);
	netdev_for_each_tx_queue(dev, vd_dev_set_lockdep_one, &subclass);
}


static const struct net_device_ops vd_netdev_ops,vd_netdev_accel_ops;

static int vd_dev_init(struct net_device *dev)
{
	struct net_device *real_dev = vd_dev_info(dev)->real_dev;
	int subclass = 0;

	netif_carrier_off(dev);

	/* IFF_BROADCAST|IFF_MULTICAST; ??? */
	dev->flags  = real_dev->flags & ~(IFF_UP | IFF_PROMISC | IFF_ALLMULTI);
	dev->iflink = real_dev->ifindex;
	dev->state  = (real_dev->state & ((1<<__LINK_STATE_NOCARRIER) |
					  (1<<__LINK_STATE_DORMANT))) |
		      (1<<__LINK_STATE_PRESENT);

	dev->features |= real_dev->features & real_dev->vlan_features;
	dev->gso_max_size = real_dev->gso_max_size;

	/* ipv6 shared card related stuff */
	dev->dev_id = real_dev->dev_id;

	if (is_zero_ether_addr(dev->dev_addr))
		memcpy(dev->dev_addr, real_dev->dev_addr, dev->addr_len);
	if (is_zero_ether_addr(dev->broadcast))
		memcpy(dev->broadcast, real_dev->broadcast, dev->addr_len);

	dev->header_ops      = real_dev->header_ops;
	dev->hard_header_len = real_dev->hard_header_len;
	dev->netdev_ops         = &vd_netdev_ops;
	
	netdev_resync_ops(dev);

	if (is_vd_dev(real_dev))
		subclass = 1;

	vd_dev_set_lockdep_class(dev, subclass);
	return 0;
}

static void vd_dev_uninit(struct net_device *dev)
{
	while(0){/*do nothing*/};
}

static int vd_ethtool_get_settings(struct net_device *dev,
				     struct ethtool_cmd *cmd)
{
	const struct vd_dev_info *vd = vd_dev_info(dev);
	struct net_device *real_dev = vd->real_dev;

	if (!real_dev->ethtool_ops ||
	    !real_dev->ethtool_ops->get_settings)
		return -EOPNOTSUPP;

	return real_dev->ethtool_ops->get_settings(real_dev, cmd);
}

static void vd_ethtool_get_drvinfo(struct net_device *dev,
				     struct ethtool_drvinfo *info)
{
	strcpy(info->driver,NETVD_FULLNAME);
	strcpy(info->version,NETVD_VERSION);
	strcpy(info->fw_version, "N/A");
}

static u32 vd_ethtool_get_rx_csum(struct net_device *dev)
{
	const struct vd_dev_info *vd = vd_dev_info(dev);
	struct net_device *real_dev = vd->real_dev;

	if (real_dev->ethtool_ops == NULL ||
	    real_dev->ethtool_ops->get_rx_csum == NULL)
		return 0;
	return real_dev->ethtool_ops->get_rx_csum(real_dev);
}

static u32 vd_ethtool_get_flags(struct net_device *dev)
{
	const struct vd_dev_info *vd = vd_dev_info(dev);
	struct net_device *real_dev = vd->real_dev;

	if (real_dev->ethtool_ops == NULL ||real_dev->ethtool_ops->get_flags == NULL)
		return 0;
	return real_dev->ethtool_ops->get_flags(real_dev);
}

static const struct ethtool_ops vd_ethtool_ops = {
	.get_settings	       = vd_ethtool_get_settings,
	.get_drvinfo	       = vd_ethtool_get_drvinfo,
	.get_link			= ethtool_op_get_link,
	.get_rx_csum		= vd_ethtool_get_rx_csum,
	.get_flags		= vd_ethtool_get_flags,
};

static const struct net_device_ops vd_netdev_ops = {
	.ndo_change_mtu		= vd_dev_change_mtu,
	.ndo_init				= vd_dev_init,
	.ndo_uninit			= vd_dev_uninit,
	.ndo_open			= vd_dev_open,
	.ndo_stop			= vd_dev_stop,
	.ndo_start_xmit 		=  vd_dev_hard_start_xmit,
	.ndo_validate_addr		= eth_validate_addr,
	.ndo_set_mac_address	= vd_dev_set_mac_address,
	.ndo_set_rx_mode		= vd_dev_set_rx_mode,
	.ndo_set_multicast_list	= vd_dev_set_rx_mode,
	.ndo_change_rx_flags	= vd_dev_change_rx_flags,
	.ndo_do_ioctl			= vd_dev_ioctl,
	.ndo_neigh_setup		= vd_dev_neigh_setup,
};

static const struct net_device_ops vd_netdev_accel_ops = {
	.ndo_change_mtu		= vd_dev_change_mtu,
	.ndo_init				= vd_dev_init,
	.ndo_uninit			= vd_dev_uninit,
	.ndo_open			= vd_dev_open,
	.ndo_stop			= vd_dev_stop,
	.ndo_start_xmit 		=  vd_dev_hard_start_xmit,
	.ndo_validate_addr		= eth_validate_addr,
	.ndo_set_mac_address	= vd_dev_set_mac_address,
	.ndo_set_rx_mode		= vd_dev_set_rx_mode,
	.ndo_set_multicast_list	= vd_dev_set_rx_mode,
	.ndo_change_rx_flags	= vd_dev_change_rx_flags,
	.ndo_do_ioctl			= vd_dev_ioctl,
	.ndo_neigh_setup		= vd_dev_neigh_setup,
};


void vd_setup(struct net_device *dev)
{
	ether_setup(dev);

	dev->priv_flags		|= IFF_VIRTUAL_DEV;
	dev->tx_queue_len	= 0;
	dev->netdev_ops		= &vd_netdev_ops;
	dev->destructor		= free_netdev;
	dev->ethtool_ops		= &vd_ethtool_ops;
	memset(dev->broadcast, 0, ETH_ALEN);
}
