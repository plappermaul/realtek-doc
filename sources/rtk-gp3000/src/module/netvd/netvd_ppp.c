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
#include <linux/kernel.h>
#include <linux/slab.h> 
#include <linux/unistd.h> 
#include <linux/sched.h> 
#include <linux/fs.h> 
#include <linux/file.h> 
#include <linux/mm.h> 
#include <asm/uaccess.h> 
#include <linux/netdevice.h>
#include <module/netvd/netvd_config.h>


mm_segment_t oldfs; 

#define INIT_KERNEL_ENV()  {  oldfs = get_fs(); set_fs(KERNEL_DS); } 
#define PPPOE_PATH "/proc/net/pppoe"

extern int getNetIfIdByDevName(const char *devName);
extern int getPPPoeIdByDevName(const char *devName);
 
static struct file *netvd_open_file(char *path,int flag,int mode) 
{ 
	struct file *fp; 
	 
	fp=filp_open(path, flag, 0); 
	if (!IS_ERR(fp)) return fp; 
	else return NULL;
	
} 
 
static int netvd_read_file(struct file *fp,char *buf,int readlen) 
{ 
	if (fp->f_op && fp->f_op->read) 
	return fp->f_op->read(fp,buf,readlen, &fp->f_pos); 
	else 
	return -1; 
} 
 
static int netvd_close_file(struct file *fp) 
{ 
	filp_close(fp,NULL); 
	return 0; 
} 


int netvd_read_file_init(char *filename,char *data) 
{ 
	char buf[1024]; 
	struct file *fp; 
	int ret; 

	INIT_KERNEL_ENV(); 
	fp=netvd_open_file(filename,O_RDONLY,0); 
	if (fp!=NULL && data!=NULL) 
	{ 
		memset(buf,0,1024); 
		if ((ret=netvd_read_file(fp,buf,1024))>0) {
			memcpy(data,buf,sizeof(buf));
		}
		netvd_close_file(fp); 
	} 
	set_fs(oldfs); 
	return 0; 
} 


int netvd_read_file_callback(char *filename, int (*doRead)(char *data,void *info),void *info) 
{ 
	char buf[1024]; 
	struct file *fp; 
	int ret; 

	INIT_KERNEL_ENV(); 
	fp=netvd_open_file(filename,O_RDONLY,0); 
	if (fp!=NULL && info!=NULL && doRead!=NULL) 
	{ 
		memset(buf,0,1024); 
		while ((ret=netvd_read_file(fp,buf,1024))>0) {
				if(doRead(buf,info)==VD_ERR_OK){
					netvd_close_file(fp); 
					set_fs(oldfs); 
					return VD_ERR_OK;
				}
				memset(buf,0,1024);
		}
		netvd_close_file(fp); 
	} 
	set_fs(oldfs); 
	return VD_ERR_ENTRY_NOTFOUNT; 
} 

 
int netvd_get_pppInfo_by_devName(const char *devName,pppInfo_t *info)
{
	char buf[1024]="";
	int ret;
	char *pch,*result;
	int id,i;
	int dst[ETHER_ADDR_LEN] = {0};
	char dev[IFNAMSIZ]="",mac[32]="";
	
	int pppId = getPPPoeIdByDevName(devName);
	int netifId;
	ret = netvd_read_file_init(PPPOE_PATH,&buf[0]);
	pch = buf;

	/*for title*/
	/*Id       Address              Device*/
	/*parsing it!!*/
	result = strsep(&pch,"\n");
	
	while(pch !=NULL){
		
		result = strsep(&pch,"\n");
		ret = sscanf(result,"%x\t%s\t%s\n",&id,mac,dev);
		if(ret <=0){
			continue;
		}
		netifId = getNetIfIdByDevName(dev);
		if(netifId != pppId){
			continue;
		}
		info->id = id;
		ret = sscanf(mac,"%02X:%02X:%02X:%02X:%02X:%02X",&dst[0],&dst[1],&dst[2],&dst[3],&dst[4],&dst[5]);
		for(i=0;i<ETHER_ADDR_LEN;i++) { info->remote[i] = (unsigned char) dst[i]; }
		VD_DEBUG(NETVD_LOG_LEVEL_LOW,"devName=%s,Id=%d,MAC=%02X:%02X:%02X:%02X:%02X:%02X\n",
		dev,info->id,
		info->remote[0],info->remote[1],info->remote[2],
		info->remote[3],info->remote[4],info->remote[5]);
		info->netifId = netifId; 
		return VD_ERR_OK;
	}

	return RT_ERR_ENTRY_NOTFOUND;
}


