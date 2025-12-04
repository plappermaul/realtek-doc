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

/*
 * Include Files
 */
#include <linux/capability.h>
#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <linux/init.h>
#include <linux/rtnetlink.h>
#include <linux/notifier.h>
#include <net/rtnetlink.h>
#include <net/net_namespace.h>
#include <net/netns/generic.h>
#include <asm/uaccess.h>
#include <module/netvd/netvd.h>


#define MODULENAME 	"netvd"

extern int drv_nic_register_rxhook(int portmask,int priority,p2rfunc_t rx);
extern int drv_nic_unregister_rxhook(int portmask,int priority,p2rfunc_t rx);

/* Global VLAN variables */
struct list_head vdhead;
int vd_debug=0;




static void
vd_list_init(void)
{

	INIT_LIST_HEAD(&vdhead);
	return;
}


static void
vd_list_clear(void)
{

	struct list_head *next = NULL, *tmp=NULL;
	vd_group_t  *cur = NULL;

	list_for_each_safe(next,tmp,&vdhead){
	
		cur = list_entry(next,vd_group_t,list);
		
	    	if(cur!=NULL){
			list_del(&cur->list);
			kfree(cur);
		}
	}
	return;
}


static int 
vd_list_add(struct net_device *dev)
{
	vd_group_t  *entry = NULL;
	entry = (vd_group_t*) kmalloc(sizeof(vd_group_t),GFP_KERNEL);

	if(!entry)
	{
		return VD_ERR_ADD_FAILED;
	}
	
	entry->dev = dev;
	list_add(&entry->list,&vdhead);
	return VD_ERR_OK;
}

static int 
vd_list_remove(struct net_device *dev)
{
	struct list_head *next = NULL, *tmp=NULL;
	vd_group_t  *cur = NULL;
	struct vd_dev_info *curData,*devData;

	if(!is_vd_dev(dev)){
		return 0;
	}
	
	devData = vd_dev_info(dev);

	list_for_each_safe(next,tmp,&vdhead){
		cur = list_entry(next,vd_group_t,list);
		curData = vd_dev_info(cur->dev);

		if(curData->real_dev == devData->real_dev && 
		   curData->port == devData->port && 
		   curData->vlanId == devData->vlanId)
		{
			list_del(&cur->list);
			return RT_ERR_OK;
		}
	}

	return VD_ERR_REMOVE_FAILED;
}



struct net_device* __find_vd_dev(struct net_device *dev, u16 vid, uint8 port){

	struct list_head *next = NULL, *tmp=NULL;
	vd_group_t  *cur = NULL;
	struct vd_dev_info *privData;

	list_for_each_safe(next,tmp,&vdhead){
	
		cur = list_entry(next,vd_group_t,list);
	    	if(cur){
			privData = vd_dev_info(cur->dev);
			if(privData->real_dev == dev && privData->vlanId == vid && privData->port == port)
			{
				return cur->dev;
			}
		}
	}
	return NULL;
}


int 
vd_check_real_dev(struct net_device *real_dev, u16 vlan_id, uint8 port)
{
	/* The real device must be up and operating in order to
	 * assosciate a Virtual device with it.
	 */
	if (!(real_dev->flags & IFF_UP))
		return -ENETDOWN;

	if (__find_vd_dev(real_dev, vlan_id,port) != NULL)
		return -EEXIST;

	return 0;
}



void 
unregister_vd_dev(struct net_device *dev)
{
	struct vd_dev_info * vd = vd_dev_info(dev);
	struct net_device *real_dev = vd->real_dev;
	
	if(dev->flags & IFF_UP)
	{	
		VD_DEBUG(NETVD_LOG_LEVEL_LOW,"should down the interface first!\n");
		return ;
	}

	/*remove form list*/
	vd_list_remove(dev);
	synchronize_net();	

	/*unregister from system*/
	rtnl_lock();
	unregister_netdevice(dev);
	rtnl_unlock();
	/* Get rid of the vlan's reference to real_dev */
	dev_put(real_dev);
}


int 
register_vd_dev(struct net_device *dev)
{
	struct vd_dev_info *vd = vd_dev_info(dev);
	struct net_device *real_dev = vd->real_dev;
	int err;

	/*add dev to list*/
	err = vd_list_add(dev);
	if(err < 0)
		goto out_free_list;
	/*register to system*/
	rtnl_lock();
	err = register_netdevice(dev);
	rtnl_unlock();
	if (err < 0)
		goto out_uninit_applicant;
	
	/* Account for reference in struct vd_dev_info */
	dev_hold(real_dev);

	vd_transfer_operstate(real_dev, dev);
	
	return 0;

out_uninit_applicant:
	vd_list_remove(dev);
out_free_list:
	return err;
}



/*  Attach a Virtual Device device to a mac address (ie Ethernet Card).
 *  Returns 0 if the device was created or a negative error code otherwise.
 */
int register_vd_device(struct net_device *real_dev,uint8 port, u16 vlan_id)
{
	struct net_device *new_dev;
	struct net *net = dev_net(real_dev);
	struct vd_dev_info *pri;
	char name[IFNAMSIZ]="";
	//struct vd_net *vn = net_generic(net, vd_net_id);
	int err;

	if (vlan_id >= VLAN_VID_MASK)
		return -ERANGE;

	err = vd_check_real_dev(real_dev, vlan_id,port);
	if (err < 0)
		return err;

	snprintf(name, IFNAMSIZ, "%s.%d.%d", real_dev->name, port,vlan_id);
	new_dev = alloc_netdev(sizeof(struct vd_dev_info), name,vd_setup);
	/* Gotta set up the fields for the device. */

	if (new_dev == NULL)
		return -ENOBUFS;

	dev_net_set(new_dev, net);
	/*assign MAC addr*/
	memcpy(&new_dev->dev_addr[0],&real_dev->dev_addr[0],MAX_ADDR_LEN);
	/*assign mtu*/
	new_dev->mtu = real_dev->mtu;
	new_dev->rtnl_link_ops = real_dev->rtnl_link_ops;
	new_dev->priv_flags |=IFF_VIRTUAL_DEV;
	/*assign private data*/
	pri = vd_dev_info(new_dev);
	pri->vlanId = vlan_id;
	pri->real_dev = real_dev;
	pri->dent = NULL;
	pri->flags = VD_FLAG_REORDER_HDR;
	pri->port = port;
	pri->pbit = 0;
	memset(pri->flowId,0,sizeof(pri->flowId));

	err = register_vd_dev(new_dev);
	if (err < 0)
		goto out_free_newdev;

	return 0;

out_free_newdev:
	free_netdev(new_dev);
	return err;
}


static int __init netvd_init_module(void)
{
	int err;
	/*init group*/
	vd_list_init();
	/*register proc*/
	err = vd_proc_init();
	if (err < 0)
		goto err0;

	err = netvd_event_init();
	if(err < 0)
		goto err1;
	/*register NIC rx handler*/
	err = drv_nic_register_rxhook(0x7f,RE8686_RXPRI_L34LITE,vd_rx_skb);
	if(err < 0)
		goto err1;

	/*register netifter hook point for mark ct*/
	netvd_ct_mark_init();
	/*register netvd thread and start it!*/
	netvd_thread_init();
	VD_DEBUG(NETVD_LOG_LEVEL_LOW,"init thread ok!\n");
	return 0;


err1:
	netvd_event_exit();
err0:
	return err;
}

static void __exit netvd_cleanup_module(void)
{
	vd_proc_cleanup();
	netvd_thread_exit();
	netvd_ct_mark_exit();
	netvd_event_exit();
	drv_nic_unregister_rxhook(0x7f,RE8686_RXPRI_L34LITE,vd_rx_skb);
	/* This table must be empty if there are no module references left. */
	/*remove virtual device*/
	vd_list_clear();

}


module_init(netvd_init_module);
module_exit(netvd_cleanup_module);

MODULE_LICENSE("GPL");
MODULE_VERSION(NETVD_VERSION);
MODULE_DESCRIPTION(NETVD_FULLNAME);
MODULE_AUTHOR("RealTek");

