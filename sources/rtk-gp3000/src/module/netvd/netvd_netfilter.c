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
 * Purpose : Netfitler Module for Connection Tracking
 *
 * Feature : N/A
 *
 */
#include <linux/init.h>
#include <linux/types.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <linux/netfilter_ipv4.h>
#include <linux/inet.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <module/netvd/netvd.h>
#include <net/netfilter/nf_conntrack_ecache.h>


extern char wanName[16];
extern char lanName[16];
 
static unsigned int ct_mark(unsigned int hook,
                                               struct sk_buff *pskb,
                                               const struct net_device *in,
                                               const struct net_device *out,
                                               int (*okfn)(struct sk_buff *)
                                               )
{
	struct nf_conn *ct;
	enum ip_conntrack_info ctinfo;
	struct iphdr *iph;
	int dir;

	if(strstr(out->name,wanName)!=NULL || strstr(out->name,"ppp")!=NULL)
	{
		dir = CT_DIR_UPSTREAM;
	}else
	if(strstr(out->name,lanName)!=NULL)
	{
		dir = CT_DIR_DOWNSTREAM;
	}else
	{
		return NF_ACCEPT;
	}
	
	/* This is where we call the helper: as the packet goes out. */
	ct = nf_ct_get(pskb, &ctinfo);

	if (ct == &nf_conntrack_untracked || ct ==NULL)
	{
		return NF_ACCEPT;
	}
	
	iph  = ip_hdr(pskb);
       
       if(iph->protocol == IPPROTO_UDP || iph->protocol == IPPROTO_TCP)
       {
#if defined(CONFIG_NF_CONNTRACK_MARK)
		if(!(ct->mark & CONNTRACK_ADD_MARK)){
			 ct->mark |= (CONNTRACK_ADD_MARK | dir);
			 VD_DEBUG(NETVD_LOG_LEVEL_HIGH,"%s(): mark ct %x, out dev: %s\n",__FUNCTION__,ct->mark,out->name);
			 nf_conntrack_event_cache(IPCT_MARK, ct);
		}
#endif
       }
       
       return NF_ACCEPT;
}

 
static struct nf_hook_ops ct_mark_ops =
{
       .hook = ct_mark,
       .pf = PF_INET,
       .hooknum = NF_IP_POST_ROUTING,
       .priority = NF_IP_PRI_CONNTRACK_CONFIRM,
       
};
 
int  netvd_ct_mark_init(void)
{
       return nf_register_hook(&ct_mark_ops);
}
 
void  netvd_ct_mark_exit(void)
{
       nf_unregister_hook(&ct_mark_ops);
}

