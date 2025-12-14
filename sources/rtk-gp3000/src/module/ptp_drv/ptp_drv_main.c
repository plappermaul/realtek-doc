/*
 * Copyright (C) 2016 Realtek Semiconductor Corp.
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
 * $Revision: 51194 $
 * $Date:  $
 *
 * Purpose : PTP kernel drivers
 *
 * Feature : This module install PTP kernel callbacks and other 
 * kernel functions
 *
 */

/*
 * Include Files
 */
#include <linux/module.h> 
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/skbuff.h>
#include <net/sock.h> 
#include <linux/proc_fs.h>
#include <linux/version.h>

/* For RTK APIs */
#include <common/rt_error.h>
#include <rtk/epon.h>
#include <rtk/port.h>
#include <rtk/switch.h>
#include <rtk/trap.h>

#include <pkt_redirect.h>

#if defined(CONFIG_RTL9600_SERIES)
#include "re8686.h"
#elif defined(CONFIG_RTL9601B_SERIES)
#include "re8686_rtl9601b.h"
#elif defined(CONFIG_RTL9602C_SERIES)
#include "re8686_rtl9602c.h"
#elif defined(CONFIG_RTL9607C_SERIES)
#include "re8686_rtl9607c.h"
#else
#include "re8686_nic.h"
#endif

#define PTP_Sync                   0x0
#define PTP_Delay_Req              0x1
#define PTP_Pdelay_Req             0x2
#define PTP_Pdelay_Resp            0x3
#define PTP_Follow_Up              0x8
#define PTP_Delay_Resp             0x9
#define PTP_Pdelay_Resp_Follow_Up  0xA
#define PTP_Announce               0xB
#define PTP_Rx_Mirror              0xff

#define PTP_msg_num                9
#define PTP_Rx_Mirror_idx          PTP_msg_num-1


typedef struct PTP_TRAP_to_UID_s{
    char name[32];
    int mID;
    int uID;
}PTP_TRAP_to_UID_t;

static PTP_TRAP_to_UID_t ptp_rx_uid[PTP_msg_num]=
{
    {"Sync"                     ,PTP_Sync                     ,-1},//PR_USER_UID_PTPSLAVE
    {"Delay_Req"                ,PTP_Delay_Req                ,-1},//PR_USER_UID_PTPMASTER
    {"Pdelay_Req"               ,PTP_Pdelay_Req               ,-1},//PR_USER_UID_PTPMASTER
    {"Pdelay_Resp"              ,PTP_Pdelay_Resp              ,-1},//PR_USER_UID_PTPMASTER
    {"Follow_Up"                ,PTP_Follow_Up                ,-1},//PR_USER_UID_PTPSLAVE
    {"Delay_Resp"               ,PTP_Delay_Resp               ,-1},//PR_USER_UID_PTPSLAVE
    {"Pdelay_Resp_Follow_Up"    ,PTP_Pdelay_Resp_Follow_Up    ,-1},//PR_USER_UID_PTPMASTER
    {"Announce"                 ,PTP_Announce                 ,-1},//PR_USER_UID_PTPSLAVE
    {"Rx_Mirror"                ,PTP_Rx_Mirror                ,-1},//PR_USER_UID_PTPMASTER
};


struct proc_dir_entry* ptp_proc_dir = NULL; 
struct proc_dir_entry* ptp_rx_debug_entry = NULL;
struct proc_dir_entry* ptp_rx_trap_uid_entry = NULL;
static rtk_enable_t debug_rx=DISABLED;

static uint32 RX_PORTMASK=0xf;

#if defined(CONFIG_RTL9607C_SERIES)
struct proc_dir_entry* ptp_rx_gmac_entry = NULL;
static int gmac_rx=0;
#endif

#define RTK_API_ERR_CHK(op, ret)\
    do {\
        if ((ret = (op)) != RT_ERR_OK)\
        {\
            printk("[%s:%d] Rtk API Error! ret=%d \n", __FUNCTION__, __LINE__,ret); \
        }\
    } while(0)

void ptp_pkt_tx(unsigned short dataLen, unsigned char *data)
{
    struct tx_info txInfo;
    extern int re8686_tx_with_Info(unsigned char *pPayload, unsigned short length, void *pInfo);

#if 0
    int i;
    printk("%s %d 0x%x 0x%x 0x%x %d\n", __FUNCTION__, __LINE__, data[0], data[1], data[2], dataLen);
    for(i=0;i<dataLen;i++)
    {
        printk("0x%02x ",data[i]);
    }
    printk("\n");
#endif    
    memset(&txInfo,0x0,sizeof(struct tx_info));
  
    txInfo.opts2.bit.cputag = 1;
    
    re8686_tx_with_Info(data, dataLen , &txInfo);
}

int ptp_pkt_rx(
    struct re_private *cp,
    struct sk_buff *skb,
    struct rx_info *pRxInfo)
{
    int i;
    int etype_offset = 12;

#if defined(CONFIG_RTL9607C_SERIES)
    if(gmac_rx!=0xf&&cp->gmac!=gmac_rx)
    {
        if(debug_rx!=0)
        {
            printk("%s %d rx_gmac=%d is not correct, should be %d\n", __FUNCTION__, __LINE__, cp->gmac,gmac_rx);
        }
        return RE8670_RX_CONTINUE;
    }
#endif

    if((skb->data[0] == 0x01) && (skb->data[1] == 0x1b) && (skb->data[2] == 0x19)
        && (skb->data[3] == 0x00) && (skb->data[4] == 0x00) && (skb->data[5] == 0x00))
    {
        if(debug_rx!=0)
        {
#if defined(CONFIG_RTL9602C_SERIES)
            printk("%s %d reason=%d srcPort=%d len=%d data=\n", __FUNCTION__, __LINE__, pRxInfo->opts3.bit.reason,pRxInfo->opts3.bit.src_port_num, skb->len);
#elif defined(CONFIG_RTL9607C_SERIES)
            printk("%s %d rx_gmac=%d reason=%d srcPort=%d len=%d data=\n", __FUNCTION__, __LINE__, cp->gmac,pRxInfo->opts2.bit.reason,pRxInfo->opts3.bit.src_port_num, skb->len);
#else
            #error "Please check the field of reason at structure rx info"
#endif
            for(i=0;i<skb->len;i++)
            {
                printk("0x%02x ",skb->data[i]);
            }
            printk("\n");
        }
        
        if((skb->data[etype_offset] == 0x88 ) && (skb->data[etype_offset+1] == 0x99)
            && (skb->data[etype_offset+3] == 240) )
        { 
            etype_offset +=16;

            if((skb->data[etype_offset] == 0x88) && (skb->data[etype_offset+1] == 0xa8))
            {
                etype_offset +=4;
            }

            if((skb->data[etype_offset] == 0x81) && (skb->data[etype_offset+1] == 0x00))
            {
                etype_offset +=4;
            }

            if((skb->data[etype_offset] == 0x88) && (skb->data[etype_offset+1] == 0xf7))
            {
                for(i=0;i<PTP_msg_num;i++)
                {
                    if((skb->data[etype_offset+2]&0xf)==ptp_rx_uid[i].mID)
                    {
                        if(ptp_rx_uid[i].uID==PR_USER_UID_PTPMASTER||ptp_rx_uid[i].uID==PR_USER_UID_PTPSLAVE)
                        {
                            pkt_redirect_kernelApp_sendPkt(ptp_rx_uid[i].uID, 1, skb->len, skb->data);
                            if(debug_rx!=0)
                                printk("Trap %s packet to uID %d\n",ptp_rx_uid[i].name,ptp_rx_uid[i].uID);
                            return RE8670_RX_STOP;
                        }
                    }
                }
            }
        }
    }

    if((skb->data[0] == 0x01) && (skb->data[1] == 0x80) && (skb->data[2] == 0xc2)
        && (skb->data[3] == 0x00) && (skb->data[4] == 0x00) && (skb->data[5] == 0x0e))
    {
        if(debug_rx!=0)
        {
#if defined(CONFIG_RTL9602C_SERIES)
            printk("%s %d reason=%d srcPort=%d len=%d data=\n", __FUNCTION__, __LINE__, pRxInfo->opts3.bit.reason,pRxInfo->opts3.bit.src_port_num, skb->len);
#elif defined(CONFIG_RTL9607C_SERIES)
            printk("%s %d rx_gmac=%d reason=%d srcPort=%d len=%d data=\n", __FUNCTION__, __LINE__, cp->gmac,pRxInfo->opts2.bit.reason,pRxInfo->opts3.bit.src_port_num, skb->len);
#else
            #error "Please check the field of reason at structure rx info"
#endif
            for(i=0;i<skb->len;i++)
            {
                printk("0x%02x ",skb->data[i]);
            }
            printk("\n");
        }
        
        if((skb->data[etype_offset] == 0x88 ) && (skb->data[etype_offset+1] == 0x99)
            && (skb->data[etype_offset+3] == 240))
        {
            etype_offset +=16;

            if((skb->data[etype_offset] == 0x88) && (skb->data[etype_offset+1] == 0xa8))
            {
                etype_offset +=4;
            }

            if((skb->data[etype_offset] == 0x81) && (skb->data[etype_offset+1] == 0x00))
            {
                etype_offset +=4;
            }

            for(i=0;i<PTP_msg_num;i++)
            {
                if((skb->data[etype_offset+2]&0xf)==ptp_rx_uid[i].mID)
                {
                    if(ptp_rx_uid[i].uID==PR_USER_UID_PTPMASTER||ptp_rx_uid[i].uID==PR_USER_UID_PTPSLAVE)
                    {
                        pkt_redirect_kernelApp_sendPkt(ptp_rx_uid[i].uID, 1, skb->len, skb->data);
                        if(debug_rx!=0)
                            printk("Trap %s packet to uID %d\n",ptp_rx_uid[i].name,ptp_rx_uid[i].uID);
                        return RE8670_RX_STOP;
                    }
                }
            }
        }
    }

    /*special case for reason 243, the packet format is not following the internal spec*/
    if((skb->data[0] == 0xff) && (skb->data[1] == 0xff) && (skb->data[2] == 0xff)
        && (skb->data[3] == 0xff) && (skb->data[4] == 0xff) && (skb->data[5] == 0xff))
    {
        if(debug_rx!=0)
        {
#if defined(CONFIG_RTL9602C_SERIES)
            printk("%s %d reason=%d srcPort=%d len=%d data=\n", __FUNCTION__, __LINE__, pRxInfo->opts3.bit.reason,pRxInfo->opts3.bit.src_port_num, skb->len);
#elif defined(CONFIG_RTL9607C_SERIES)
            printk("%s %d rx_gmac=%d reason=%d srcPort=%d len=%d data=\n", __FUNCTION__, __LINE__, cp->gmac,pRxInfo->opts2.bit.reason,pRxInfo->opts3.bit.src_port_num, skb->len);
#else
            #error "Please check the field of reason at structure rx info"
#endif
            for(i=0;i<skb->len;i++)
            {
                printk("0x%02x ",skb->data[i]);
            }
            printk("\n");
        }
        /*send-back of sync and pdelay_resp*/
#if defined(CONFIG_RTL9602C_SERIES)
        if( pRxInfo->opts3.bit.reason == 243)
#elif defined(CONFIG_RTL9607C_SERIES)
        if( pRxInfo->opts2.bit.reason == 243)
#else
        #error "Please check the field of reason at structure rx info"
#endif
        {
            /*For 9602C test chip rx mirror packet is not following spec to include CPU tag, adding CPU tag*/
            if(skb->data[etype_offset]!=0x88&&skb->data[etype_offset+1]!=0x99)
            {
                /*insert cpu tag manually*/
                memmove(skb->data+20,skb->data+12,skb->len-12);
                memset(skb->data+12,0,8);
                skb->data[12]=0x88;
                skb->data[13]=0x99;
#if defined(CONFIG_RTL9602C_SERIES)
                skb->data[15]=pRxInfo->opts3.bit.reason;
#elif defined(CONFIG_RTL9607C_SERIES)
                skb->data[15]=pRxInfo->opts2.bit.reason;
#else
                #error "Please check the field of reason at structure rx info"
#endif
                skb->data[17]=pRxInfo->opts3.bit.src_port_num&0x03;
                skb->len+=8;
            }
            if(ptp_rx_uid[PTP_Rx_Mirror_idx].uID==PR_USER_UID_PTPMASTER||ptp_rx_uid[PTP_Rx_Mirror_idx].uID==PR_USER_UID_PTPSLAVE)
            {
                pkt_redirect_kernelApp_sendPkt(ptp_rx_uid[PTP_Rx_Mirror_idx].uID, 1, skb->len, skb->data);
                if(debug_rx!=0)
                    printk("Trap %s packet to uID %d\n",ptp_rx_uid[i].name,ptp_rx_uid[i].uID);
                return RE8670_RX_STOP;
            }
        }
    }
    
    return RE8670_RX_CONTINUE;
}

static int ptp_rx_debug_read(struct seq_file *seq, void *v)
{

    seq_printf(seq, "ptp rx debug status: %u\n", debug_rx);

    return 0;
}

static int ptp_rx_debug_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos)
{
    unsigned char tmpBuf[16] = {0};
    int len = (count > 15) ? 15 : count;
    
    if (buffer && !copy_from_user(tmpBuf, buffer, len))
    {
        debug_rx = simple_strtoul(tmpBuf, NULL, 16);
        printk("\nwrite ptp rx debug to %u\n", debug_rx);
        return count;
    }
    return -EFAULT;
}

static int ptp_rx_debug_open_proc(struct inode *inode, struct file *file)
{
    return single_open(file, ptp_rx_debug_read, inode->i_private);
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
static const struct proc_ops ptp_rx_debug_fop = {
    .proc_open      = ptp_rx_debug_open_proc,
    .proc_write     = ptp_rx_debug_write,
    .proc_read      = seq_read,
    .proc_lseek     = seq_lseek,
    .proc_release   = single_release,
};
#else
static const struct file_operations ptp_rx_debug_fop = {
    .owner          = THIS_MODULE,
    .open           = ptp_rx_debug_open_proc,
    .write          = ptp_rx_debug_write,
    .read           = seq_read,
    .llseek         = seq_lseek,
    .release        = single_release,
};
#endif

static int ptp_rx_trap_uid_read(struct seq_file *seq, void *v)
{
    int i;

    for(i=0;i<PTP_msg_num;i++)
    {
        seq_printf(seq, "%-23s MessageID=%-3d trap to User ID=%-3d(%s)\n"
            ,ptp_rx_uid[i].name,ptp_rx_uid[i].mID,ptp_rx_uid[i].uID
            ,(ptp_rx_uid[i].uID==PR_USER_UID_PTPMASTER)?"ptpmaster":((ptp_rx_uid[i].uID==PR_USER_UID_PTPSLAVE)?"ptpslave":"unknown"));
    }
    return 0;
}

static int ptp_rx_trap_uid_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos)
{
    unsigned char tmp[10] = { 0 },*p,*t1=NULL,*t2=NULL;
    int len = 9,i,mID=0,uID=0;

    if (buffer && !copy_from_user(tmp, buffer, len)) {
        p=tmp;
        t1=p;
        for(i=0;i<10||i<count;i++)
        {
            if(*p=='\n')
                break;
            if(*p==' ')
            {
                *p='\0';
                t2=++p;
                break;
            }
            p++;
        }

        if(t1==NULL||t2==NULL)
            return (count);

        mID=simple_strtoul(t1, NULL, 10);
        uID=simple_strtoul(t2, NULL, 10);

        for(i=0;i<PTP_msg_num;i++)
        {
            if(ptp_rx_uid[i].mID==mID)
            {
                ptp_rx_uid[i].uID=uID;
            }
        }
        
    }
    return (count);
}

static int ptp_rx_trap_uid_open_proc(struct inode *inode, struct file *file)
{
    return single_open(file, ptp_rx_trap_uid_read, inode->i_private);
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
static const struct proc_ops ptp_rx_trap_uid_fop = {
    .proc_open      = ptp_rx_trap_uid_open_proc,
    .proc_write     = ptp_rx_trap_uid_write,
    .proc_read      = seq_read,
    .proc_lseek     = seq_lseek,
    .proc_release   = single_release,
};
#else
static const struct file_operations ptp_rx_trap_uid_fop = {
    .owner          = THIS_MODULE,
    .open           = ptp_rx_trap_uid_open_proc,
    .write          = ptp_rx_trap_uid_write,
    .read           = seq_read,
    .llseek         = seq_lseek,
    .release        = single_release,
};
#endif

#if defined(CONFIG_RTL9607C_SERIES)
static int ptp_rx_gmac_read(struct seq_file *seq, void *v)
{

    seq_printf(seq, "ptp rx gmac status: 0x%x\n", gmac_rx);

    return 0;
}

static int ptp_rx_gmac_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos)
{
    unsigned char tmpBuf[16] = {0};
    int len = (count > 15) ? 15 : count;
    int tmpInt=0;
    
    if (buffer && !copy_from_user(tmpBuf, buffer, len))
    {
        tmpInt = simple_strtoul(tmpBuf, NULL, 16);
        if(tmpInt!=0&&tmpInt!=1&&tmpInt!=0xf)
        {
            printk("\ngmac_rx should be 0,1,0xf\n");
        }
        else
        {
            gmac_rx=tmpInt;
            printk("\nwrite ptp rx gmac to 0x%x\n", gmac_rx);
        }
        return count;
    }
    return -EFAULT;
}

static int ptp_rx_gmac_open_proc(struct inode *inode, struct file *file)
{
    return single_open(file, ptp_rx_gmac_read, inode->i_private);
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
static const struct proc_ops ptp_rx_gmac_fop = {
    .proc_open      = ptp_rx_gmac_open_proc,
    .proc_write     = ptp_rx_gmac_write,
    .proc_read      = seq_read,
    .proc_lseek     = seq_lseek,
    .proc_release   = single_release,
};
#else
static const struct file_operations ptp_rx_gmac_fop = {
    .owner          = THIS_MODULE,
    .open           = ptp_rx_gmac_open_proc,
    .write          = ptp_rx_gmac_write,
    .read           = seq_read,
    .llseek         = seq_lseek,
    .release        = single_release,
};
#endif
#endif

int __init ptp_drv_init(void)
{
    extern int drv_nic_register_rxhook(int portmask,int priority,p2rfunc_t rx);
    int ret = RT_ERR_OK;
    rtk_switch_devInfo_t devInfo;

    RTK_API_ERR_CHK(rtk_switch_deviceInfo_get(&devInfo),ret);

    if (NULL == ptp_proc_dir) {
        ptp_proc_dir = proc_mkdir ("ptp_drv", NULL);
    }

    if(ptp_proc_dir)
    {
        ptp_rx_debug_entry = proc_create("debug", 0644, ptp_proc_dir, &ptp_rx_debug_fop);
        if (!ptp_rx_debug_entry) {
            printk("ptp_rx_debug_entry, create proc failed!");
        }
    }
    
    if(ptp_proc_dir)
    {
        ptp_rx_trap_uid_entry = proc_create("rx_trap_uid", 0644, ptp_proc_dir, &ptp_rx_trap_uid_fop);
        if (!ptp_rx_trap_uid_entry) {
            printk("ptp_rx_trap_uid, create proc failed!");
        }
    }
#if defined(CONFIG_RTL9607C_SERIES)
    if(ptp_proc_dir)
    {
        ptp_rx_gmac_entry = proc_create("rx_gmac", 0644, ptp_proc_dir, &ptp_rx_gmac_fop);
        if (!ptp_rx_gmac_entry) {
            printk("ptp_rx_gmac_entry, create proc failed!");
        }
    }
#endif
    printk("Register PTP for pkt_redirect module rx port mask 0x%x\n",devInfo.ether.portmask.bits[0]);

    RX_PORTMASK=devInfo.ether.portmask.bits[0];

    /* For packet redirect to user space protocol */
    pkt_redirect_kernelApp_reg(PR_KERNEL_UID_PTP, ptp_pkt_tx);

    drv_nic_register_rxhook(RX_PORTMASK, RE8686_RXPRI_MUTICAST, ptp_pkt_rx);
    
    return 0;
}

void __exit ptp_drv_exit(void)
{
    extern int drv_nic_unregister_rxhook(int portmask,int priority,p2rfunc_t rx);
    
    pkt_redirect_kernelApp_dereg(PR_KERNEL_UID_PTP);
    drv_nic_unregister_rxhook(RX_PORTMASK, RE8686_RXPRI_MUTICAST, ptp_pkt_rx);

    if(ptp_rx_debug_entry)
    {
        remove_proc_entry("debug", ptp_proc_dir);
        ptp_rx_debug_entry = NULL;
    }
    if(ptp_rx_trap_uid_entry)
    {
        remove_proc_entry("rx_trap_uid", ptp_proc_dir);
        ptp_rx_trap_uid_entry = NULL;
    }
#if defined(CONFIG_RTL9607C_SERIES)
    if(ptp_rx_gmac_entry)
    {
        remove_proc_entry("rx_gmac", ptp_proc_dir);
        ptp_rx_gmac_entry = NULL;
    }
#endif
    if(ptp_proc_dir)
    {
        remove_proc_entry("ptp_drv", NULL);
        ptp_proc_dir = NULL;
    }
}

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("RealTek PTP drive module");
MODULE_AUTHOR("Realtek");
module_init(ptp_drv_init);
module_exit(ptp_drv_exit);

