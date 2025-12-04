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
 * Purpose : Net Virual Device for Control NIC API
 *
 * Feature : Provide the APIs to control Virtaul Device
 *
 */
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/fs.h>
#include <linux/netdevice.h>
#include <linux/types.h>
#include <linux/version.h>
#include <net/net_namespace.h>
#include <net/netns/generic.h>
#include <module/netvd/netvd.h>

extern struct list_head cthead;



struct proc_dir_entry *dir = NULL, *config, *log=NULL;	
struct proc_dir_entry *ct = NULL, *time = NULL;
struct proc_dir_entry *wanifname = NULL, *lanifname=NULL;


/* Methods for preparing data for reading proc entries */
static int vd_seq_show(struct seq_file *seq, void *v);
static void *vd_seq_start(struct seq_file *seq, loff_t *pos);
static void *vd_seq_next(struct seq_file *seq, void *v, loff_t *pos);
static void vd_seq_stop(struct seq_file *seq, void *);
static int vd_seq_show(struct seq_file *seq, void *v);
static int vdev_seq_show(struct seq_file *seq, void *v);

/*
 *	Names of the proc directory entries
 */
static const char name_root[]	 = "netvd";
static const char name_conf[]	 = "config";
static const char name_log[]	 = "log";
static const char name_time[]	 = "naptTime";
static const char name_ct[]	 	 = "ct";
static const char name_wanifname[]	 = "wan_if_name";
static const char name_lanifname[]	 = "lan_if_name";

extern char wanName[16];
extern char lanName[16];


/*for create interface*/
static ssize_t vd_write_proc(struct file *file, const char __user *buf,
                       unsigned long count, void *data)
{
	char name[IFNAMSIZ]="", realname[IFNAMSIZ]="";
	int port=0, vid=0,ret=0;
	struct net_device *dev = NULL;

	/* no data be written */
	if (!count) {
	    goto create_err;	
	}

	/* Input size is too large to write our buffer(num) */
	if (count > (sizeof(name) - 1)) {
	   goto create_err;	
	}

	if (copy_from_user(name, buf, count)) {
	   goto create_err	;
	}

	/* parsing name */
	sprintf(name,"%s\n",name);
	ret = sscanf(name,"%s %d %d\n",&realname[0],&port,&vid);
	VD_DEBUG(NETVD_LOG_LEVEL_LOW,"realname %s, port %d, vid %d\n",realname,port,vid);
	if(ret!=3)
	{
		VD_DEBUG(NETVD_LOG_LEVEL_LOW,"parsing failed\n");
		goto create_err;
	}
	/*direct init a default virtual device for test, will mark it*/
	dev = dev_get_by_name(&init_net,realname); 

	if(!dev)
	{
		goto create_err;

	}

	ret = register_vd_device(dev,port,vid);
	
	if(ret != RT_ERR_OK)
	{
		goto create_err;
	}
	return count;
create_err:
	
	return -EFAULT;
}


/*for delete interface*/
static ssize_t vdev_write_proc(struct file *file, const char __user *buf,
                       size_t count, loff_t *ppos)
{

	unregister_vd_dev(data);
   	return count;
}

/*
 *	Generic /proc/net/vd/<file> file and inode operations
 */

static const struct seq_operations vd_seq_ops = {
	.start = vd_seq_start,
	.next = vd_seq_next,
	.stop = vd_seq_stop,
	.show = vd_seq_show,
};

static int vd_seq_open(struct inode *inode, struct file *file)
{
	return seq_open_net(inode, file, &vd_seq_ops,
			sizeof(struct seq_net_private));
}

static const struct file_operations vd_fops = {
	.owner	 = THIS_MODULE,
	.open    = vd_seq_open,
	.read    = seq_read,
	.write  =  vd_write_proc,
	.llseek  = seq_lseek,
	.release = seq_release_net,
};

/*
 *	/proc/net/netvd/<device> file and inode operations
 */

static int vdev_seq_open(struct inode *inode, struct file *file)
{
	return single_open(file, vdev_seq_show, PDE(inode)->data);
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
static const struct proc_ops vdev_fops = {
	.proc_open    = vdev_seq_open,
	.proc_read    = seq_read,
	.proc_write  = vdev_write_proc,
	.proc_lseek  = seq_lseek,
	.proc_release = single_release,
};
#else
static const struct file_operations vdev_fops = {
	.owner = THIS_MODULE,
	.open    = vdev_seq_open,
	.read    = seq_read,
	.write  = vdev_write_proc,
	.llseek  = seq_lseek,
	.release = single_release,
};
#endif



/*
 *	/proc/net/netvd/ct file and inode operations
 */

static int vd_ct_seq_show(struct seq_file *seq, void *offset)
{
	
	struct list_head *next = NULL, *tmp=NULL;
	ct_hw_t  *cur = NULL;
	struct nf_conn *ct;
	int proto;
	uint32 srcIp,srcport,dstIp,dstport;
	
	list_for_each_safe(next,tmp,&cthead){
	
		cur = list_entry(next,ct_hw_t,list);
		ct = cur->ct;
		proto 	= ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.dst.protonum;
		/*start to assing needed parameters*/
		srcIp 	= ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.src.u3.ip;
		srcport    = ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.src.u.tcp.port;
		dstIp 	= ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.dst.u3.ip;
		dstport 	= ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.dst.u.tcp.port;
	    	seq_printf(seq,"id:%d,proto:%x,src:%u.%u.%u.%u/%d,dst:%u.%u.%u.%u/%d",cur->naptId,proto,NIPQUAD(srcIp),srcport,NIPQUAD(dstIp),dstport);
		seq_puts(seq, "\n");
	}

	return 0;
}

static int vd_ct_seq_open(struct inode *inode, struct file *file)
{
	return single_open(file, vd_ct_seq_show, NULL);
}


/*for create interface*/
static ssize_t vd_ct_write_proc(struct file *file, const char __user *buf,
                       size_t count, loff_t *ppos)
{
	char value[128]="";

	/* no data be written */
	if (!count) {
	    goto create_err;	
	}

	/* Input size is too large to write our buffer(num) */
	if (count > (sizeof(value) - 1)) {
	   goto create_err;	
	}

	if (copy_from_user(value, buf, count)) {
	   goto create_err	;
	}
	/*printk("%s(),value=%s\n",__FUNCTION__,value);*/
	/* parsing name */
	if(strstr(value,"del")!=NULL)
	{
		ct_list_clear();
	}
	return count;
create_err:
	
	return -EFAULT;
}


#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
static const struct proc_ops vd_ct_fops = {
	.proc_owner = THIS_MODULE,
	.proc_open    = vd_ct_seq_open,
	.proc_read    = seq_read,
	.proc_write  = vd_ct_write_proc,
	.proc_lseek  = seq_lseek,
	.proc_release = single_release,
};
#else
static const struct file_operations vd_ct_fops = {
	.owner = THIS_MODULE,
	.open    = vd_ct_seq_open,
	.read    = seq_read,
	.write  = vd_ct_write_proc,
	.llseek  = seq_lseek,
	.release = single_release,
};
#endif

/*
 * Proc filesystem derectory entries.
 */

/* Strings */

/*
 *	Interface functions
 */

/*
 *	Clean up /proc/net/vd entries
 */

void vd_proc_cleanup()
{

	if (log)
		remove_proc_entry(name_log, dir);

	if (config)
		remove_proc_entry(name_conf,dir);

	if (wanifname)
		remove_proc_entry(name_conf,dir);

	if (lanifname)
		remove_proc_entry(name_conf,dir);

	if (time)
		remove_proc_entry(name_conf,dir);

	if (ct)
		remove_proc_entry(name_conf,dir);

	if (dir)
		remove_proc_entry(name_root, NULL);

	/* Dynamically added entries should be cleaned up as their vlan_device
	 * is removed, so we should not have to take care of it here...
	 */
}



static int vd_proc_write_pbit(struct file *file, const char __user *buffer,unsigned long count, void *data) 
{ 
	char proc_buffer[10],*endptr;
	struct net_device *dev = (struct net_device *)data;
	struct vd_dev_info *priv = vd_dev_info(dev);

	if (count > 0 ) {
		if (copy_from_user(proc_buffer, buffer, count))
			return -EFAULT;

		priv->pbit = 	simple_strtol(proc_buffer,&endptr,10);
	}
	
	return count;
} 

static int vd_proc_read_pbit( char* page, char** start, off_t off, int count, int* eof, void* data) 
{
    int len = 0 ;
    struct net_device *dev = (struct net_device *)data;
    struct vd_dev_info *priv = vd_dev_info(dev);

    
    page += off ;
    page [ 0 ] = 0 ;

    len += sprintf( ( page + len ), "pbit: %d\n",priv->pbit) ;

    *eof = 1;
    
    return len; 
}

static int vd_proc_write_flowId(struct file *file, const char __user *buffer,unsigned long count, void *data) 
{ 
	char proc_buffer[10],*endptr;
	int id;
	struct net_device *dev = (struct net_device *)data;
	struct vd_dev_info *priv = vd_dev_info(dev);

	if (count > 0 ) {
		if (copy_from_user(proc_buffer, buffer, count))
			return -EFAULT;
		id = simple_strtol(proc_buffer,&endptr,10);
		priv->flowId[id] = simple_strtol(endptr,&endptr,10);
	}
	
	return count;
} 

static int vd_proc_read_flowId( char* page, char** start, off_t off, int count, int* eof, void* data) 
{
	int len = 0,i ;
	struct net_device *dev = (struct net_device *)data;
	struct vd_dev_info *priv = vd_dev_info(dev);


	page += off ;
	page [ 0 ] = 0 ;

	len += sprintf( ( page + len ), "flowId:\n");
	for(i=0;i<8;i++){
		len += sprintf((page + len ), "%d: %d\n",i,priv->flowId[i]) ;
	}

	*eof = 1;

	return len; 
}

static int vd_proc_write_log(struct file *file, const char __user *buffer,unsigned long count, void *data) 
{ 
	char proc_buffer[10],*endptr;

	if (count > 0 ) {
		if (copy_from_user(proc_buffer, buffer, count))
			return -EFAULT;

		vd_debug = simple_strtol(proc_buffer,&endptr,10);
	}
	
	return count;
} 

static int vd_proc_read_log( char* page, char** start, off_t off, int count, int* eof, void* data) 
{
    int len = 0 ;

    
    page += off ;
    page [ 0 ] = 0 ;

    len += sprintf( ( page + len ), "debug state: %d\n",vd_debug) ;

    *eof = 1;
    
    return len; 
}

static int vd_proc_write_time(struct file *file, const char __user *buffer,unsigned long count, void *data) 
{ 
	char proc_buffer[10],*endptr;

	if (count > 0 ) {
		if (copy_from_user(proc_buffer, buffer, count))
			return -EFAULT;

		ctMonitorTime = simple_strtol(proc_buffer,&endptr,10);
	}
	
	return count;
} 

static int vd_proc_read_time( char* page, char** start, off_t off, int count, int* eof, void* data) 
{
    int len = 0 ;

    
    page += off ;
    page [ 0 ] = 0 ;

    len += sprintf( ( page + len ), "%s: %d\n",name_time,ctMonitorTime) ;

    *eof = 1;
    
    return len; 
}

static int vd_proc_write_wanifname(struct file *file, const char __user *buffer,unsigned long count, void *data) 
{ 

	if (count > 0 ) {
		if (copy_from_user(wanName, buffer, count))
			return -EFAULT;
	}
	
	return count;
} 

static int vd_proc_read_wanifname( char* page, char** start, off_t off, int count, int* eof, void* data) 
{
    int len = 0 ;

    
    page += off ;
    page [ 0 ] = 0 ;

    len += sprintf( ( page + len ), "%s: %s\n",name_wanifname,wanName) ;

    *eof = 1;
    
    return len; 
}

static int vd_proc_write_lanifname(struct file *file, const char __user *buffer,unsigned long count, void *data) 
{ 

	if (count > 0 ) {
		if (copy_from_user(lanName, buffer, count))
			return -EFAULT;
	}
	
	return count;
} 

static int vd_proc_read_lanifname( char* page, char** start, off_t off, int count, int* eof, void* data) 
{
    int len = 0 ;

    
    page += off ;
    page [ 0 ] = 0 ;

    len += sprintf( ( page + len ), "%s: %s\n",name_lanifname,lanName) ;

    *eof = 1;
    
    return len; 
}


/*
 *	Create /proc/net/netvd entries
 */

int vd_proc_init()
{

	dir = proc_mkdir(name_root,NULL);
	if (!dir)
		goto err;

	/*for configure netvd interface*/
	config = proc_create(name_conf, S_IFREG|S_IRUSR|S_IWUSR,dir, &vd_fops);
	if (!config)
		goto err;

	/*for ct configureation*/
	ct = proc_create(name_ct, S_IFREG|S_IRUSR|S_IWUSR,dir, &vd_ct_fops);
	if (!ct)
		goto err;
	
	ct->write_proc = vd_ct_write_proc;

	/*for log config*/
	log = proc_create_data(name_log,S_IFREG|S_IRUSR|S_IWUSR,dir,NULL,NULL);
	log->read_proc = vd_proc_read_log;
	log->write_proc = vd_proc_write_log;

	/*for periodic time*/
	time = proc_create_data(name_time,S_IFREG|S_IRUSR|S_IWUSR,dir,NULL,NULL);
	time->read_proc = vd_proc_read_time;
	time->write_proc = vd_proc_write_time;

	/*for wan name*/
	wanifname = proc_create_data(name_wanifname,S_IFREG|S_IRUSR|S_IWUSR,dir,NULL,NULL);
	wanifname->read_proc = vd_proc_read_wanifname;
	wanifname->write_proc = vd_proc_write_wanifname;

		/*for wan name*/
	lanifname = proc_create_data(name_lanifname,S_IFREG|S_IRUSR|S_IWUSR,dir,NULL,NULL);
	lanifname->read_proc = vd_proc_read_lanifname;
	lanifname->write_proc = vd_proc_write_lanifname;
	
	return 0;

err:
	pr_err("%s: can't create entry in proc filesystem!\n", __func__);
	vd_proc_cleanup();
	return -ENOBUFS;
}

/*
 *	Add directory entry for Virtual device.
 */

int vd_proc_add_dev(struct net_device *vdev)
{
	struct vd_dev_info *dev_info = vd_dev_info(vdev);
	struct proc_dir_entry *proc_file_config;

	if(!is_vd_dev(vdev))
		return 0;

	dev_info->dent =
		proc_mkdir(vdev->name, dir);
	proc_file_config = 
		proc_create_data("state", S_IFREG|S_IRUSR|S_IWUSR,
				 dev_info->dent, &vdev_fops, vdev);
	
	/*create config proc and callback function*/
	proc_file_config = proc_create_data("pbit",S_IFREG|S_IRUSR|S_IWUSR,dev_info->dent,NULL,vdev);
	proc_file_config->read_proc = vd_proc_read_pbit;
	proc_file_config->write_proc = vd_proc_write_pbit;
	proc_file_config = proc_create_data("flowId",S_IFREG|S_IRUSR|S_IWUSR,dev_info->dent,NULL,vdev);
	proc_file_config->read_proc = vd_proc_read_flowId;
	proc_file_config->write_proc = vd_proc_write_flowId;
	
	if (!dev_info->dent)
		return -ENOBUFS;
	return 0;
}

/*
 *	Delete directory entry for Virtual device.
 */
int vd_proc_rem_dev(struct net_device *vdev)
{

	/** NOTE:  This will consume the memory pointed to by dent, it seems. */
	if (vd_dev_info(vdev)->dent) {
		remove_proc_entry(vd_dev_info(vdev)->dent->name,dir);
		vd_dev_info(vdev)->dent = NULL;
	}
	return 0;
}

/****** Proc filesystem entry points ****************************************/

/*
 * The following few functions build the content of /proc/net/vd/config
 */

/* start read of /proc/net/netvd/config */
static void *vd_seq_start(struct seq_file *seq, loff_t *pos)
	__acquires(dev_base_lock)
{
	struct net_device *dev;
	struct net *net = seq_file_net(seq);
	loff_t i = 1;

	read_lock(&dev_base_lock);

	if (*pos == 0)
		return SEQ_START_TOKEN;

	for_each_netdev(net, dev) {
		if (!is_vd_dev(dev))
			continue;

		if (i++ == *pos)
			return dev;
	}

	return  NULL;
}

static void *vd_seq_next(struct seq_file *seq, void *v, loff_t *pos)
{
	struct net_device *dev;
	struct net *net = seq_file_net(seq);

	++*pos;

	dev = (struct net_device *)v;
	if (v == SEQ_START_TOKEN)
		dev = net_device_entry(&net->dev_base_head);

	for_each_netdev_continue(net, dev) {
		if (!is_vd_dev(dev))
			continue;

		return dev;
	}

	return NULL;
}

static void vd_seq_stop(struct seq_file *seq, void *v)
	__releases(dev_base_lock)
{
	read_unlock(&dev_base_lock);
}

static int vd_seq_show(struct seq_file *seq, void *v)
{
	if (v == SEQ_START_TOKEN) {
		seq_puts(seq, "RTK Net Virtual Dev\n");
		seq_puts(seq, "Name	 | PortID  |  VID  | Real Dev Name\n");

	} else {
		const struct net_device *vdev = v;
		const struct vd_dev_info *dev_info = vd_dev_info(vdev);

		seq_printf(seq, "%-9s| %-6d  | %-4d  | %s\n",  vdev->name,dev_info->port,
			   dev_info->vlanId, dev_info->real_dev->name);
	}
	return 0;
}

static int vdev_seq_show(struct seq_file *seq, void *offset)
{
	struct net_device *vdev;
	struct net_device_stats *stats;
	static const char fmt[] = "%30s %12lu\n";

	vdev = (struct net_device *) seq->private;
	if (!vdev || !is_vd_dev(vdev))
		return 0;

	stats = &vdev->stats;
	seq_printf(seq, fmt, "total frames received", stats->rx_packets);
	seq_printf(seq, fmt, "total bytes received", stats->rx_bytes);
	seq_printf(seq, fmt, "Broadcast/Multicast Rcvd", stats->multicast);
	seq_puts(seq, "\n");
	seq_printf(seq, fmt, "total frames transmitted", stats->tx_packets);
	seq_printf(seq, fmt, "total bytes transmitted", stats->tx_bytes);
	seq_puts(seq, "\n");
	return 0;
}



