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
 * Purpose : Routing Table Parser & Update handler
 *
 * Feature : N/A
 *
 */
#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <linux/rtnetlink.h>
#include <linux/notifier.h>
#include <linux/list.h>
#include <net/rtnetlink.h>
#include <rtk/l34lite.h>
#include <module/netvd/netvd.h>
#include <net/route.h>
#include <net/sock.h>
#include <net/ip_fib.h>


#define IP_ROUTE_PATH   "/proc/net/route"
#define IP6_ROUTE_PATH "/proc/net/ipv6_route"


extern int netvd_read_file_init(char *filename,char *data);


#ifdef CONFIG_IP_MULTIPLE_TABLES

static int netvd_get_route(u32 id, u32* gwIp,char *outif,u32 ifaddr)
{
	struct fib_table *tb = NULL;
	struct hlist_node *node = NULL;
	struct hlist_head *head = NULL;
	struct fib_result res;
	struct fib_info *info = NULL;
	unsigned int h;
	struct net *net = &init_net;
	struct flowi flp = { .nl_u = { .ip4_u = { .daddr = ifaddr } } };

	if (id == 0)
		id = RT_TABLE_MAIN;
	h = id & (FIB_TABLE_HASHSZ - 1);

	rcu_read_lock();
	head = &net->ipv4.fib_table_hash[h];
	hlist_for_each_entry_rcu(tb, node, head, tb_hlist) {
		if (tb->tb_id == id) {
			rcu_read_unlock();
			goto find_res;
		}
	}
	rcu_read_unlock();
	VD_DEBUG(NETVD_LOG_LEVEL_HIGH,"%s(): can't find tb\n",__FUNCTION__);
	return VD_ERR_ENTRY_NOTFOUNT;
find_res:
	memset(&res,0,sizeof(struct fib_result)); 	
	tb->tb_lookup(tb,&flp,&res);
	info = res.fi;	
	if(info != NULL){
		*gwIp = info->fib_nh[0].nh_gw;
		strncpy(outif,info->fib_nh[0].nh_dev->name,IFNAMSIZ);
	}else{
		return VD_ERR_ENTRY_NOTFOUNT;
	}
	return VD_ERR_OK;
}

#endif
 
int netvd_get_defaultGw(int routeId,gwInfo_t *info)
{

#ifndef CONFIG_IP_MULTIPLE_TABLES
	char buf[1024]="";
	int ret;
	char *pch,*result;
	int dst,gw,flag,refCnt,use,metric,mask,mtu,win,irtt;
	char dev[IFNAMSIZ]="";
	
	ret = netvd_read_file_init(IP_ROUTE_PATH,&buf[0]);
	pch = buf;
	/*for title*/
	/*Iface   Destination  Gateway  Flags  RefCnt  Use  Metric  Mask MTU  Window  IRTT*/
	/*parsing it!!*/
	result = strsep(&pch,"\n");
	while(pch !=NULL){
		result = strsep(&pch,"\n");
		ret = sscanf(result,"%s\t%x\t%x\t%x\t%x\t%x\t%x\t%x\t%x\t%x\t%x\n",
			  dev,&dst,&gw,&flag,&refCnt,&use,&metric,&mask,&mtu,&win,&irtt);

		if(dst==0  && mask ==0 && gw!=0)
		{
			memcpy(&info->devName[0],&dev[0],IFNAMSIZ);
			info->gwIp = gw;
			return VD_ERR_OK;
		}
	}
	return VD_ERR_ENTRY_NOTFOUNT;
#else
	int ret;
	ret = netvd_get_route(routeId,&info->gwIp,&info->devName[0],0);
	return ret;
#endif
}


int netvd_get_gw(int routeId,uint32 ipaddr,gwInfo_t *info)
{

#ifndef CONFIG_IP_MULTIPLE_TABLES
	return VD_ERR_OK;
#else
	int ret;
	ret = netvd_get_route(routeId,&info->gwIp,&info->devName[0],ipaddr);
	return ret;
#endif
}



extern int netvd_read_file_callback(char *filename, int (*doRead)(char *data,void *info),void *info) ;

static void netvd_ip6_stringToIp6addr(char *str,struct in6_addr *ip6)
{
	 int i=0;
        unsigned char c,d,e;
        for(i=0;i<32;i+=2){
                c = str[i];
                d = str[i+1];
                if(c > '9')
                        c = c - 'a' + 10;
                else
                        c = c - '0' ;

                if(d > '9')
                        d = d - 'a' +10;
                else
                        d = d - '0' ;
                e =c*16+d;
                ip6->in6_u.u6_addr8[i/2] = e;
        }
	 return ;
}

static int netvd_ip6_route_callback(char *data,void *info)
{
	int ret;
	char *pch,*result;
	char  ip6Str[33],gwStr[33], tmpStr[33]; 
	uint32 prefix,tmp32;
	char dev[IFNAMSIZ]="";
	struct in6_addr dst,gw;
	gw6Info_t *gwInfo = (gw6Info_t *)info;
	pch  = data;
	/*title*/
	/*Destination  Prefix NO  NO  GW Metric  NO NO  Dev*/
	while(pch !=NULL){
		result = strsep(&pch,"\n");
		ret = sscanf(result,"%s\t%x\t%s\t%x\t%s\t%x\t%x\t%x\t%x\t%s\n",
		ip6Str,&prefix,&tmpStr,&tmp32,gwStr,&tmp32,&tmp32,&tmp32,&tmp32,dev);
		netvd_ip6_stringToIp6addr(ip6Str,&dst);
		netvd_ip6_stringToIp6addr(gwStr,&gw);
		if(strstr(dev,wanName)!=NULL){

			if(prefix ==0)
			{
				memcpy(&gwInfo->devName[0],&dev[0],IFNAMSIZ);
				gwInfo->gwIp = gw;
				return VD_ERR_OK;
			}
		}
	}
	return VD_ERR_ENTRY_NOTFOUNT;
}

int netvd_get_ip6defaultGw(gw6Info_t *info)
{
	int ret ;
	if((ret = netvd_read_file_callback(IP6_ROUTE_PATH,netvd_ip6_route_callback,info))!=VD_ERR_OK)
	{
		return ret;
	}

	return VD_ERR_OK;
}


