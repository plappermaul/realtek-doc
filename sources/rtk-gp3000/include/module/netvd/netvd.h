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

#ifndef __VIRTUAL_DEVICE_H__
#define __VIRTUAL_DEVICE_H__

#if defined(CONFIG_RTL9600_SERIES)
#include "re8686.h"
#elif defined(CONFIG_RTL9601B_SERIES)
#include "re8686_rtl9601b.h"
#elif defined(CONFIG_RTL9602C_SERIES)
#include "re8686_rtl9602c.h"
#else
#include "re8686_nic.h"
#endif
#include <module/netvd/netvd_config.h>
#include <linux/proc_fs.h>

extern int vd_net_id;
extern int ctMonitorTime;
extern char wanName[16];


#define NIP6QUAD(ip6addr)  \
	ntohs(((u16 *)&(ip6addr))[0]),\
	ntohs(((u16 *)&(ip6addr))[1]), \
	ntohs(((u16 *)&(ip6addr))[2]), \
	ntohs(((u16 *)&(ip6addr))[3]), \
	ntohs(((u16 *)&(ip6addr))[4]), \
	ntohs(((u16 *)&(ip6addr))[5]), \
	ntohs(((u16 *)&(ip6addr))[6]), \
	ntohs(((u16 *)&(ip6addr))[7]) \


#define NIP6QUAD_FMT "%x:%x:%x:%x:%x:%x:%x:%x"

enum {
 	VD_FLAG_REORDER_HDR=1,
};


#define CT_DIR_UPSTREAM 	0x2
#define CT_DIR_DOWNSTREAM 	0x4

static inline struct vd_dev_info *vd_dev_info(const struct net_device *dev)
{
	return netdev_priv(dev);
}

typedef struct vd_dev_info {
	u16					vlanId;
	uint8				pbit;
	u16					flags;
	uint8				port;
	struct net_device		*real_dev;
	unsigned char			real_dev_addr[ETH_ALEN];
	struct proc_dir_entry	*dent;
	uint32 				flowId[8];
}vd_dev_info_t;

typedef struct vd_net {
	/* /proc/net/netvd */
	struct proc_dir_entry *proc_vd_dir;
	/* /proc/net/netvd/config */
	struct proc_dir_entry *proc_vd_conf;
	/* /proc/net/netvd/log */
	struct proc_dir_entry *proc_vd_log;
	/* /proc/net/netvd/naptTime */
	struct proc_dir_entry *proc_vd_time;
	/*/proc/net/netvd/ct*/
	struct proc_dir_entry *proc_vd_ct;
	/*/proc/net/netvd/wan_if_name*/
	struct proc_dir_entry *proc_vd_wanifname;		
	/*/proc/net/netvd/lan_if_name*/
	struct proc_dir_entry *proc_vd_lanifname;

}vd_net_t;

static inline int is_vd_dev(struct net_device *dev)
{
	 int isvd;
	 isvd = (dev->priv_flags & IFF_VIRTUAL_DEV) ? 1:0;
	 return isvd;
}

typedef struct vd_group_s{
	struct net_device *dev;
	struct list_head list;
	spinlock_t lock;
}vd_group_t;


typedef struct ct_hw_s{
	uint32  naptId;
	struct nf_conn *ct;
	struct list_head list;
}ct_hw_t;


struct net_device* __find_vd_dev(struct net_device *dev, u16 vid, uint8 port);

int register_vd_dev(struct net_device *dev);
int register_vd_device(struct net_device *real_dev,uint8 port, u16 vlan_id);
void unregister_vd_dev(struct net_device *dev);
void vd_setup(struct net_device *dev);
int vd_rx_skb (struct re_private *cp, struct sk_buff *skb,struct rx_info *pRxInfo);
void vd_transfer_operstate(const struct net_device *dev,struct net_device *vdev);
void vd_proc_cleanup();
int vd_proc_init();
int vd_proc_rem_dev(struct net_device *vdev);
int vd_proc_add_dev(struct net_device *vdev);
void ct_list_clear(void);
int netvd_get_pppInfo_by_devName(const char *devName,pppInfo_t *info);
int netvd_create_ppp(int netifId,struct net_device *dev);
int netvd_default_map_init(void);
int netvd_get_defaultGw(int routeId,gwInfo_t *info);
int netvd_get_gw(int routeId,uint32 ipaddr,gwInfo_t *info);
int netvd_get_ip6defaultGw(gw6Info_t *info);
int netvd_event_init(void);
void netvd_event_exit(void);
int netvd_thread_init (void);
void netvd_thread_exit(void);
int  netvd_ct_mark_init(void);
void  netvd_ct_mark_exit(void);
int getNetIfIdByDevName(const char *devName,netvd_netifType_t *type);

#endif
