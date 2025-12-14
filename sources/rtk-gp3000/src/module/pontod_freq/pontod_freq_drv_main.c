/*
 * Copyright (C) 2017 Realtek Semiconductor Corp.
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
 * $Revision:  $
 * $Date:  $
 *
 * Purpose : PON ToD tune Time Frequency kernel drivers
 *
 * Feature : This module install PON ToD tune Time Frequency kernel callbacks and other 
 * kernel functions
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kthread.h>
#include <linux/math64.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/version.h>
#include <net/sock.h>
#include <net/netlink.h>
#include <linux/skbuff.h>
#include <linux/kmod.h>

#if defined(CONFIG_SDK_RTL9602C)
#include <dal/rtl9602c/dal_rtl9602c.h>
#elif defined(CONFIG_SDK_RTL9607C)
#include <dal/rtl9607c/dal_rtl9607c.h>
#else
ERROR!! chip not support!!
#endif

#include "common/error.h"

#include "rtk/time.h"
#include "rtk/ponmac.h"

#include "pontod_freq_drv_main.h"
#include "pontod_freq_drv_queue.h"

struct proc_dir_entry* pontod_freq_proc_dir = NULL;
struct proc_dir_entry* pontod_freq_enable_entry = NULL;
struct proc_dir_entry* pontod_freq_nl_type_entry = NULL;
struct proc_dir_entry* pontod_freq_recollect_entry = NULL;
struct proc_dir_entry* pontod_freq_set_freq_entry = NULL;
struct proc_dir_entry* pontod_freq_sample_method_entry = NULL;
struct proc_dir_entry* pontod_freq_sample_ms_entry = NULL;
struct proc_dir_entry* pontod_freq_print_entry = NULL;

static rtk_enable_t pontod_freq_enable=DISABLED;
static uint32 pontod_freq_nl_type=PONTOD_FREQ_NETLINK_TYPE;
static uint32 pontod_freq_recollect=0;
static rtk_enable_t pontod_freq_set_freq=DISABLED;
static uint32 sample_method=0;
static uint32 timeSampleMs=5000;
static rtk_enable_t pontod_freq_print=DISABLED;

static struct task_struct *pPONToDColToDThread=NULL;
static Queue_t *pPONToDColToDQueue=NULL;

static struct sock *sk=NULL;

static int userAppPid=-1;

static rtk_ponmac_mode_t PON_MODE;

static rtk_enable_t PONToDFirstFlag=ENABLED;
static rtk_enable_t curTickTimeFlag=DISABLED;

/*this tod time*/
static uint32 TOD_tick=0;
static uint64 TOD_time=0;
/*previous tod time*/
static uint32 S_tick=0;
static uint64 S_time=0;
/*previous system time*/
static uint64 SS_time=0;
/*previous freerun time*/
static uint64 SF_time=0;

void pontod_freq_fetch_free_time_send_queue(void)
{
    uint64 time=0;
    uint32 tick=0;
    int32 ret = RT_ERR_OK;
    rtk_time_timeStamp_t timeStamp;
    PonToDNLData_t *item=NULL;

    //Check the sample method
    if(sample_method != 0)
    {
        return;
    }

    //First round no need to featch freerun time and send to queue
    if(PONToDFirstFlag==ENABLED)
    {
        PONToDFirstFlag=DISABLED;
    }
    else 
    {
        if(PON_MODE==PONMAC_MODE_EPON)
        {
#ifdef CONFIG_SDK_RTL9602C
            if ((ret = reg_field_read(RTL9602C_EPON_LOCAL_TIMEr,RTL9602C_LOCAL_TIMEf,&tick)) != RT_ERR_OK)
            {
                printk("[%s:%d] Rtk API Error! %s\n", __FUNCTION__, __LINE__,rt_error_numToStr(ret));
                return ;
            }
#elif defined(CONFIG_SDK_RTL9607C)
            if ((ret = reg_field_read(RTL9607C_EPON_LOCAL_TIMEr,RTL9607C_LOCAL_TIMEf,&tick)) != RT_ERR_OK)
            {
                printk("[%s:%d] Rtk API Error! %s\n", __FUNCTION__, __LINE__,rt_error_numToStr(ret));
                return ;
            }
#else
            ERROR!! chip not support!!
#endif
        }
        else if(PON_MODE==PONMAC_MODE_GPON)
        {
#if defined(CONFIG_SDK_RTL9602C)
            if ((ret = reg_field_read(RTL9602C_GPON_GTC_DS_SUPERFRAME_CNTr,RTL9602C_SF_CNTRf,&tick)) != RT_ERR_OK)
            {
                printk("[%s:%d] Rtk API Error! %s\n", __FUNCTION__, __LINE__,rt_error_numToStr(ret));
                return ;
            }
#elif defined(CONFIG_SDK_RTL9607C)
            if ((ret = reg_field_read(RTL9607C_GPON_GTC_DS_SUPERFRAME_CNTr,RTL9607C_SF_CNTRf,&tick)) != RT_ERR_OK)
            {
                printk("[%s:%d] Rtk API Error! %s\n", __FUNCTION__, __LINE__,rt_error_numToStr(ret));
                return ;
            }
#else
            ERROR!! chip not support!!
#endif
        }
        else
        {
            if(pontod_freq_print==ENABLED)
                printk("[%s %d]:Wrong PON_MODE=%d\n",__FUNCTION__,__LINE__,PON_MODE);
            return ;
        }

        if(tick==0) //if use manuall set pon tod and ONU is not register, it will let F_tick=0
        {
            if(pontod_freq_print==ENABLED)
                printk("[%s %d]:Free_tick is zero\n",__FUNCTION__,__LINE__);
            return ;
        }

        if ((ret = rtk_time_curTime_latch()) != RT_ERR_OK)
        {
            printk("[%s:%d] Rtk API Error! %s\n", __FUNCTION__, __LINE__,rt_error_numToStr(ret));
            return ;
        }

        if ((ret = rtk_time_curTime_get(&timeStamp)) != RT_ERR_OK)
        {
            printk("[%s:%d] Rtk API Error! %s\n", __FUNCTION__, __LINE__,rt_error_numToStr(ret));
            return ;
        }

        time=timeStamp.sec*1000000000+timeStamp.nsec;

        if(pontod_freq_print==ENABLED)
            printk("[%s %d]:Freerun tick=%u time=%llu\n",__FUNCTION__,__LINE__,tick,time);

        //inform kernel thread to calculate frequency ratio
        if(pPONToDColToDQueue==NULL)
        {
            if(pontod_freq_print==ENABLED)
            {
                printk("[%s %d]:Queue is not ready\n",__FUNCTION__,__LINE__);
                return ;
            }
        }
        else
        {
            item=kmalloc(sizeof(PonToDNLData_t), GFP_ATOMIC);
            if(item==NULL)
            {
                if(pontod_freq_print==ENABLED)
                {
                   printk("[%s %d]:Can't create queue item\n",__FUNCTION__,__LINE__);
                   return ;
                }
            }
            else
            {
                item->type=TYPE_PONTOD_TIME_TICK;
                item->val.timeTick.S_tick=S_tick;
                item->val.timeTick.S_time=S_time;
                item->val.timeTick.F_tick=tick;
                item->val.timeTick.F_time=time;
                item->val.timeTick.E_tick=TOD_tick;
                item->val.timeTick.E_time=TOD_time;
                qEnqueue(pPONToDColToDQueue,(void *)item);
            }
        }
    }
    //Store tick and time for next round use
    S_tick=TOD_tick;
    S_time=TOD_time;
}

void pontod_freq_intr_tod_update(void)
{
    rtk_time_timeStamp_t sysTime;
    rtk_time_freerun_t freeTime;
    uint64 time=0;
    int32 ret = RT_ERR_OK;
    PonToDNLData_t *item=NULL;

    //Check the sample method
    if(sample_method != 1)
    {
        return;
    }

    if ((ret = rtk_time_curTime_get(&sysTime)) != RT_ERR_OK)
    {
        printk("[%s:%d] Rtk API Error! %s\n", __FUNCTION__, __LINE__,rt_error_numToStr(ret));
        return ;
    }
    time=sysTime.sec*1000000000+sysTime.nsec;

    if ((ret = rtk_time_freeTime_get(&freeTime)) != RT_ERR_OK)
    {
        printk("[%s:%d] Rtk API Error! %s\n", __FUNCTION__, __LINE__,rt_error_numToStr(ret));
        return ;
    }
    //First round no need to featch freerun time and send to queue
    if(PONToDFirstFlag==ENABLED)
    {
        PONToDFirstFlag=DISABLED;
    }
    else 
    {
        item=kmalloc(sizeof(PonToDNLData_t), GFP_ATOMIC);
        if(item==NULL)
        {
            if(pontod_freq_print==ENABLED)
            {
               printk("[%s %d]:Can't create queue item\n",__FUNCTION__,__LINE__);
               return ;
            }
        }
        else
        {
            item->type=TYPE_PONTOD_TIME_INTR;
            item->val.timeIntr.S_tick=S_tick;
            item->val.timeIntr.S_time=S_time;
            item->val.timeIntr.SS_time=SS_time;
            item->val.timeIntr.SF_time=SF_time;
            item->val.timeIntr.ES_time=time;
            item->val.timeIntr.EF_time=freeTime.nsec;
            item->val.timeIntr.E_tick=TOD_tick;
            item->val.timeIntr.E_time=TOD_time;
            qEnqueue(pPONToDColToDQueue,(void *)item);
        }
    }
    //Store tick and time for next round use
    S_tick=TOD_tick;
    S_time=TOD_time;
    SS_time=time;
    SF_time=freeTime.nsec;
}

void pontod_freq_polling_sample(void)
{
    rtk_time_timeStamp_t sysTime;
    rtk_time_freerun_t freeTime;
    uint64 time=0;
    int32 ret = RT_ERR_OK;
    PonToDNLData_t *item=NULL;

    if ((ret = rtk_time_curTime_latch()) != RT_ERR_OK)
    {
        printk("[%s:%d] Rtk API Error! %s\n", __FUNCTION__, __LINE__,rt_error_numToStr(ret));
        return ;
    }

    if ((ret = rtk_time_curTime_get(&sysTime)) != RT_ERR_OK)
    {
        printk("[%s:%d] Rtk API Error! %s\n", __FUNCTION__, __LINE__,rt_error_numToStr(ret));
        return ;
    }
    time=sysTime.sec*1000000000+sysTime.nsec;

    if ((ret = rtk_time_freeTime_get(&freeTime)) != RT_ERR_OK)
    {
        printk("[%s:%d] Rtk API Error! %s\n", __FUNCTION__, __LINE__,rt_error_numToStr(ret));
        return ;
    }

    //First round no need to featch freerun time and send to queue
    if(PONToDFirstFlag==ENABLED)
    {
        PONToDFirstFlag=DISABLED;
    }
    else 
    {
        item=kmalloc(sizeof(PonToDNLData_t), GFP_ATOMIC);
        if(item==NULL)
        {
            if(pontod_freq_print==ENABLED)
            {
               printk("[%s %d]:Can't create queue item\n",__FUNCTION__,__LINE__);
               return ;
            }
        }
        else
        {
            item->type=TYPE_PONTOD_TIME_AUTO;
            item->val.timeAuto.SS_time=SS_time;
            item->val.timeAuto.SF_time=SF_time;
            item->val.timeAuto.ES_time=time;
            item->val.timeAuto.EF_time=freeTime.nsec;

            /*send to userspace not to queue*/
            nl_send((char *)item,sizeof(PonToDNLData_t));
            kfree(item);
        }
    }
    //Store tick and time for next round use
    SS_time=time;
    SF_time=freeTime.nsec;
}

void pontod_freq_tod_update_time(rtk_pon_tod_t tod_ticktime)
{
    if(pontod_freq_print==ENABLED)
        printk("%s:%d PON ToD update time\n", __FILE__, __LINE__);

    if(pontod_freq_enable==DISABLED)
    {
        if(pontod_freq_print==ENABLED)
            printk("[%s %d]:Thread is not ready\n",__FUNCTION__,__LINE__);
        return ;
    }

    TOD_time=tod_ticktime.timeStamp.sec*1000000000+tod_ticktime.timeStamp.nsec;

    if(tod_ticktime.ponMode==TOD_PON_MODE_EPON && PON_MODE == PONMAC_MODE_EPON)
    {
        TOD_tick=tod_ticktime.startPoint.localTime;
    }
    else if(tod_ticktime.ponMode==TOD_PON_MODE_GPON && PON_MODE == PONMAC_MODE_GPON)
    {
        TOD_tick=tod_ticktime.startPoint.superFrame;
    }
    else
    {
        if(pontod_freq_print==ENABLED)
            printk("[%s %d]:Wrong tod_ticktime.ponMode=%d PON_MODE=%d\n",__FUNCTION__,__LINE__,tod_ticktime.ponMode,PON_MODE);
        return ;
    }

    if(pontod_freq_print==ENABLED)
        printk("[%s %d]:ToD tick=%u time=%llu\n",__FUNCTION__,__LINE__,TOD_tick,TOD_time);

    curTickTimeFlag=ENABLED;
}

void pontod_freq_tod_update_enable(void)
{
    if(pontod_freq_print==ENABLED)
        printk("%s:%d PON ToD update enable\n", __FILE__, __LINE__);

    if(pontod_freq_enable==DISABLED)
    {
        if(pontod_freq_print==ENABLED)
            printk("[%s %d]:Thread is not ready\n",__FUNCTION__,__LINE__);
        return ;
    }

    //ToD tick and time is not set
    if(curTickTimeFlag==DISABLED)
    {
        if(pontod_freq_print==ENABLED)
            printk("[%s %d]:ToD Time is not set\n",__FUNCTION__,__LINE__);
        return ;
    }


    //Software latch time
    pontod_freq_fetch_free_time_send_queue();

    curTickTimeFlag=DISABLED;
}

static int PONToD_Col_ToD_thread(void *data)
{
    while(!kthread_should_stop())
    {
        if(pPONToDColToDQueue==NULL)
        {
            printk("[%s %d]:Queue is not ready\n",__FUNCTION__,__LINE__);
        }
        else
        {
            if(sample_method == 2)
            {
                pontod_freq_polling_sample();

                set_current_state(TASK_UNINTERRUPTIBLE);
                schedule_timeout(msecs_to_jiffies(timeSampleMs));
            }
            else
            {
                if (qIsEmpty(pPONToDColToDQueue))
                {
                    set_current_state(TASK_UNINTERRUPTIBLE);
                    schedule_timeout(msecs_to_jiffies(1000));
                }
                else
                {
                    PonToDNLData_t* item=NULL;

                    item=(PonToDNLData_t *)qDequeue(pPONToDColToDQueue);
                    nl_send((char *)item,sizeof(PonToDNLData_t));

                    kfree(item);
                }
            }
        }
    }
    return 0;
}

static int nl_send(char *data,uint16 dataLen)
{
    struct sk_buff *skb;
    struct nlmsghdr *nlh;
    int len,ret;
    void *pData;

    if(pontod_freq_print==ENABLED)
        printk("%s:%d send skb to user space userAppPid=%d\n", __FILE__, __LINE__,userAppPid);

    if((-1 == userAppPid) || (NULL == data))
    {
        if(pontod_freq_print==ENABLED)
            printk("%s:%d PON ToD Collect ToD userAppPid is -1 or data is NULL\n", __FILE__, __LINE__);
        return -1;
    }

    len = NLMSG_SPACE(dataLen);
    skb = alloc_skb(len, GFP_ATOMIC);
    if (!skb)
    {
        return -1;
    }
#if defined(CONFIG_KERNEL_2_6_30)
    nlh = NLMSG_PUT(skb, userAppPid, 0, 0, dataLen);
#else
    nlh = nlmsg_put(skb, userAppPid, 0, 0, dataLen,0);
#endif
    nlh->nlmsg_flags = 0;
    pData = NLMSG_DATA(nlh);
    memcpy(pData, data, dataLen);
#if defined(CONFIG_KERNEL_2_6_30)   
    NETLINK_CB(skb).pid = 0; /* from kernel */ 
#else
    NETLINK_CB(skb).portid = 0; /* from kernel */ 
#endif
    NETLINK_CB(skb).dst_group = 0; /* unicast */

    ret = netlink_unicast(sk, skb, userAppPid, MSG_DONTWAIT);
    if (ret < 0) 
    { 
        if(pontod_freq_print==ENABLED)
            printk("%s:%d PON ToD Collect ToD Netlink Send Failed!! ret=%d\n", __FILE__, __LINE__,ret);
        return -2;
    }

    return 0;

nlmsg_failure: /* Used by NLMSG_PUT */ 
    if (skb)
    {
        kfree_skb(skb);
    }
    return -3;
}

static void nl_recv(struct sk_buff *skb)
{
    int uid, pid, seq, sid;
    uint16 dataLen;
    char* data;
    struct nlmsghdr *nlh;
    PonToDNLData_t * pData;

    nlh = (struct nlmsghdr *)skb->data;
    pid = NETLINK_CREDS(skb)->pid;
#if defined(CONFIG_KERNEL_2_6_30)
    uid = NETLINK_CREDS(skb)->uid;
    sid = NETLINK_CB(skb).sid;
#else
    uid = NETLINK_CREDS(skb)->uid.val;
#endif
    seq = nlh->nlmsg_seq;
    data = NLMSG_DATA(nlh);
    dataLen = nlh->nlmsg_len;

    if(pontod_freq_print==ENABLED)
        printk("%s:%d recv skb from user space uid:%d pid:%d seq:%d,sid:%d\n", __FILE__, __LINE__,uid, pid, seq, sid);

    pData = (PonToDNLData_t *) data;

    /* Check the packet prOpType to decide operation */
    switch(pData->type)
    {
        case TYPE_PONTOD_PID_REG:
            if(pontod_freq_print==ENABLED)
                printk("%s:%d TYPE_PONTOD_PID_REG pData->val.pid=%d\n", __FILE__, __LINE__,pData->val.pid);

            userAppPid=pData->val.pid;

            pData->type=TYPE_PONTOD_PID_REG_ACK;
            nl_send((char *)pData,sizeof(PonToDNLData_t));
            break;
        case TYPE_PONTOD_PID_DREG:
            if(pontod_freq_print==ENABLED)
                printk("%s:%d TYPE_PONTOD_PID_DREG\n", __FILE__, __LINE__);
            
            userAppPid=-1;
            break;
        default:
            break;
    }
}

static int pontod_freq_enable_read(struct seq_file *seq, void *v)
{
    seq_printf(seq, "pontod_freq enable status: %u\n", pontod_freq_enable);

    return 0;
}

static int pontod_freq_enable_open_proc(struct inode *inode, struct file *file)
{
    return single_open(file, pontod_freq_enable_read, inode->i_private);
}

static int pontod_freq_enable_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos)
{
    unsigned char tmpBuf[16] = {0};
    int len = (count > 15) ? 15 : count;
    
    if (buffer && !copy_from_user(tmpBuf, buffer, len))
    {
        if((pontod_freq_enable == DISABLED) && (simple_strtoul(tmpBuf, NULL, 16) == ENABLED))
        {
            //init thread
            pPONToDColToDThread = kthread_create(PONToD_Col_ToD_thread, NULL, "PONToDCollectToD");
            if(IS_ERR(pPONToDColToDThread))
            {
                printk("%s:%d PON ToD Collect ToD Thread init failed %ld!\n", __FILE__, __LINE__, PTR_ERR(pPONToDColToDThread));
                pPONToDColToDThread = NULL;
                return -EFAULT;
            }

            //init queue for thread
            pPONToDColToDQueue=qCreate();
            if(pPONToDColToDQueue==NULL)
            {
                printk("%s:%d PON ToD Collect ToD Queue init failed!\n", __FILE__, __LINE__);
                if(pPONToDColToDThread!=NULL)
                    kthread_stop(pPONToDColToDThread);
                return -EFAULT;
            }
            //init netlink for sending message to userspace
#if defined(CONFIG_KERNEL_2_6_30)
            sk = netlink_kernel_create(&init_net, pontod_freq_nl_type,0, nl_recv, NULL, THIS_MODULE);
#else
            struct netlink_kernel_cfg cfg = {
                .input = nl_recv,
            };

            sk = netlink_kernel_create(&init_net, pontod_freq_nl_type, &cfg);
#endif 
            if(NULL == sk)
            {
                printk("%s:%d PON ToD Collect ToD netlink init failed!\n", __FILE__, __LINE__);
                
                if(pPONToDColToDThread!=NULL)
                    kthread_stop(pPONToDColToDThread);
                if(pPONToDColToDQueue!=NULL)
                {
                    PonToDNLData_t* item=NULL;
                    while(!qIsEmpty(pPONToDColToDQueue))
                    {
                        item=NULL;
                        item=(PonToDNLData_t *)qDequeue(pPONToDColToDQueue);
                        kfree(item);
                    }
                }
                return -EFAULT;
            }

            wake_up_process(pPONToDColToDThread);
        }
        else
        {
            if(pPONToDColToDThread!=NULL)
                kthread_stop(pPONToDColToDThread);
            if(pPONToDColToDQueue!=NULL)
            {
                PonToDNLData_t* item=NULL;
                while(!qIsEmpty(pPONToDColToDQueue))
                {
                    item=NULL;
                    item=(PonToDNLData_t *)qDequeue(pPONToDColToDQueue);
                    if(item!=NULL)
                        kfree(item);
                }
            }

            netlink_kernel_release(sk);
        }

        pontod_freq_enable = simple_strtoul(tmpBuf, NULL, 16);
        printk("\nwrite pontod_freq enable to %u\n", pontod_freq_enable);

        return count;
    }
    return -EFAULT;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
static const struct proc_ops pontod_freq_enable_fop = {
    .proc_open      = pontod_freq_enable_open_proc,
    .proc_write     = pontod_freq_enable_write,
    .proc_read      = seq_read,
    .proc_lseek     = seq_lseek,
    .proc_release   = single_release,
};
#else
static const struct file_operations pontod_freq_enable_fop = {
    .owner          = THIS_MODULE,
    .open           = pontod_freq_enable_open_proc,
    .write          = pontod_freq_enable_write,
    .read           = seq_read,
    .llseek         = seq_lseek,
    .release        = single_release,
};
#endif

static int pontod_freq_nl_type_read(struct seq_file *seq, void *v)
{
    seq_printf(seq, "pontod_freq netlink type status: %u\n", pontod_freq_nl_type);

    return 0;
}

static int pontod_freq_nl_type_open_proc(struct inode *inode, struct file *file)
{
    return single_open(file, pontod_freq_nl_type_read, inode->i_private);
}

static int pontod_freq_nl_type_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos)
{
    unsigned char tmpBuf[16] = {0};
    int len = (count > 15) ? 15 : count;
    uint32 type;
    
    if (buffer && !copy_from_user(tmpBuf, buffer, len))
    {
        type = simple_strtoul(tmpBuf, NULL, 10);

        //netlink range 0~31
        if(type>=32)
        {
            printk("netlink type support range 0~31\n");
            return -EFAULT;
        }
        
        pontod_freq_nl_type=type;
        printk("\nwrite pontod_freq netlink type to %u\n", pontod_freq_nl_type);

        return count;
    }
    return -EFAULT;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
static const struct proc_ops pontod_freq_nl_type_fop = {
    .proc_open      = pontod_freq_nl_type_open_proc,
    .proc_write     = pontod_freq_nl_type_write,
    .proc_read      = seq_read,
    .proc_lseek     = seq_lseek,
    .proc_release   = single_release,
};
#else
static const struct file_operations pontod_freq_nl_type_fop = {
    .owner          = THIS_MODULE,
    .open           = pontod_freq_nl_type_open_proc,
    .write          = pontod_freq_nl_type_write,
    .read           = seq_read,
    .llseek         = seq_lseek,
    .release        = single_release,
};
#endif

static int pontod_freq_recollect_read(struct seq_file *seq, void *v)
{
    seq_printf(seq, "pontod_freq recollect status: %u\n", pontod_freq_recollect);

    return 0;
}

static int pontod_freq_recollect_open_proc(struct inode *inode, struct file *file)
{
    return single_open(file, pontod_freq_recollect_read, inode->i_private);
}

static int pontod_freq_recollect_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos)
{
    unsigned char tmpBuf[16] = {0};
    int len = (count > 15) ? 15 : count;
    uint32 reset;
    PonToDNLData_t item;

    if (buffer && !copy_from_user(tmpBuf, buffer, len))
    {
        reset = simple_strtoul(tmpBuf, NULL, 10);
        
        if(reset == 1)
        {
            pontod_freq_recollect=reset;
            PONToDFirstFlag=ENABLED;

            item.type=TYPE_PONTOD_RECOLLECT;
            nl_send(&item,sizeof(PonToDNLData_t));
        }

        printk("\nwrite pontod_freq_recollect to %u\n", pontod_freq_recollect);
        
        pontod_freq_recollect=0;
        
        return count;
    }
    return -EFAULT;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
static const struct proc_ops pontod_freq_recollect_fop = {
    .proc_open      = pontod_freq_recollect_open_proc,
    .proc_write     = pontod_freq_recollect_write,
    .proc_read      = seq_read,
    .proc_lseek     = seq_lseek,
    .proc_release   = single_release,
};
#else
static const struct file_operations pontod_freq_recollect_fop = {
    .owner          = THIS_MODULE,
    .open           = pontod_freq_recollect_open_proc,
    .write          = pontod_freq_recollect_write,
    .read           = seq_read,
    .llseek         = seq_lseek,
    .release        = single_release,
};
#endif


static int pontod_freq_set_freq_read(struct seq_file *seq, void *v)
{
    seq_printf(seq, "pontod_freq set frequency status: %u\n", pontod_freq_set_freq);
    return 0;
}

static int pontod_freq_set_freq_open_proc(struct inode *inode, struct file *file)
{
    return single_open(file, pontod_freq_set_freq_read, inode->i_private);
}

static int pontod_freq_set_freq_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos)
{
    unsigned char tmpBuf[16] = {0};
    int len = (count > 15) ? 15 : count;
    uint32 reset;
    PonToDNLData_t item;
    
    if (buffer && !copy_from_user(tmpBuf, buffer, len))
    {
        reset = simple_strtoul(tmpBuf, NULL, 10);
        
        if(reset == 1)
        {
            pontod_freq_set_freq=ENABLED;

            item.type=TYPE_PONTOD_SET_FREQ;
            nl_send(&item,sizeof(PonToDNLData_t));
        }
        
        pontod_freq_set_freq=DISABLED;

        return count;
    }
    return -EFAULT;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
static const struct proc_ops pontod_freq_set_freq_fop = {
    .proc_open      = pontod_freq_set_freq_open_proc,
    .proc_write     = pontod_freq_set_freq_write,
    .proc_read      = seq_read,
    .proc_lseek     = seq_lseek,
    .proc_release   = single_release,
};
#else
static const struct file_operations pontod_freq_set_freq_fop = {
    .owner          = THIS_MODULE,
    .open           = pontod_freq_set_freq_open_proc,
    .write          = pontod_freq_set_freq_write,
    .read           = seq_read,
    .llseek         = seq_lseek,
    .release        = single_release,
};
#endif

static int pontod_freq_sample_method_read(struct seq_file *seq, void *v)
{
    seq_printf(seq, "pontod_freq sample method(0:software 1:interrupt 2:polling): %u\n", sample_method);
    return 0;
}

static int pontod_freq_sample_method_open_proc(struct inode *inode, struct file *file)
{
    return single_open(file, pontod_freq_sample_method_read, inode->i_private);
}

static int pontod_freq_sample_method_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos)
{
    unsigned char tmpBuf[16] = {0};
    int len = (count > 15) ? 15 : count;
    uint32 val;
    
    if (buffer && !copy_from_user(tmpBuf, buffer, len))
    {
        val = simple_strtoul(tmpBuf, NULL, 10);
        
        if(val > 2)
        {
            return -EFAULT;
        }

        sample_method = val;

        return count;
    }
    return -EFAULT;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
static const struct proc_ops pontod_freq_sample_method_fop = {
    .proc_open      = pontod_freq_sample_method_open_proc,
    .proc_write     = pontod_freq_sample_method_write,
    .proc_read      = seq_read,
    .proc_lseek     = seq_lseek,
    .proc_release   = single_release,
};
#else
static const struct file_operations pontod_freq_sample_method_fop = {
    .owner          = THIS_MODULE,
    .open           = pontod_freq_sample_method_open_proc,
    .write          = pontod_freq_sample_method_write,
    .read           = seq_read,
    .llseek         = seq_lseek,
    .release        = single_release,
};
#endif

static int pontod_freq_sample_ms_read(struct seq_file *seq, void *v)
{
    seq_printf(seq, "pontod_freq sample miniseconds when sample method is polling: %u\n", timeSampleMs);
    return 0;
}

static int pontod_freq_sample_ms_open_proc(struct inode *inode, struct file *file)
{
    return single_open(file, pontod_freq_sample_ms_read, inode->i_private);
}

static int pontod_freq_sample_ms_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos)
{
    unsigned char tmpBuf[16] = {0};
    int len = (count > 15) ? 15 : count;
    
    if (buffer && !copy_from_user(tmpBuf, buffer, len))
    {
        timeSampleMs = simple_strtoul(tmpBuf, NULL, 10);
        return count;
    }
    return -EFAULT;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
static const struct proc_ops pontod_freq_sample_ms_fop = {
    .proc_open      = pontod_freq_sample_ms_open_proc,
    .proc_write     = pontod_freq_sample_ms_write,
    .proc_read      = seq_read,
    .proc_lseek     = seq_lseek,
    .proc_release   = single_release,
};
#else
static const struct file_operations pontod_freq_sample_ms_fop = {
    .owner          = THIS_MODULE,
    .open           = pontod_freq_sample_ms_open_proc,
    .write          = pontod_freq_sample_ms_write,
    .read           = seq_read,
    .llseek         = seq_lseek,
    .release        = single_release,
};
#endif


static int pontod_freq_print_read(struct seq_file *seq, void *v)
{
    seq_printf(seq, "pontod_freq print: %u\n", pontod_freq_print);

    return 0;
}

static int pontod_freq_print_open_proc(struct inode *inode, struct file *file)
{
    return single_open(file, pontod_freq_print_read, inode->i_private);
}

static int pontod_freq_print_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos)
{
    unsigned char tmpBuf[16] = {0};
    int len = (count > 15) ? 15 : count;
    uint32 tmp;
    
    if (buffer && !copy_from_user(tmpBuf, buffer, len))
    {
        tmp = simple_strtoul(tmpBuf, NULL, 10);
        
        if(tmp == 1)
        {
            pontod_freq_print=ENABLED;
        }
        else
        {
            pontod_freq_print=DISABLED;
        }

        printk("\nwrite pontod_freq_print to %u\n", pontod_freq_print);
        
        return count;
    }
    return -EFAULT;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
static const struct proc_ops pontod_freq_print_fop = {
    .proc_open      = pontod_freq_print_open_proc,
    .proc_write     = pontod_freq_print_write,
    .proc_read      = seq_read,
    .proc_lseek     = seq_lseek,
    .proc_release   = single_release,
};
#else
static const struct file_operations pontod_freq_print_fop = {
    .owner          = THIS_MODULE,
    .open           = pontod_freq_print_open_proc,
    .write          = pontod_freq_print_write,
    .read           = seq_read,
    .llseek         = seq_lseek,
    .release        = single_release,
};
#endif

int __init pontod_freq_drv_init(void)
{
    if (NULL == pontod_freq_proc_dir) {
        pontod_freq_proc_dir = proc_mkdir ("pontod_freq_drv", NULL);
    }

    if(pontod_freq_proc_dir)
    {
        pontod_freq_enable_entry = proc_create("enable", 0644, pontod_freq_proc_dir, &pontod_freq_enable_fop);
        if (!pontod_freq_enable_entry) {
            printk("pontod_freq_enable_entry, create proc failed!");
        }

        pontod_freq_nl_type_entry = proc_create("nl_type", 0644, pontod_freq_proc_dir, &pontod_freq_nl_type_fop);
        if (!pontod_freq_nl_type_entry) {
            printk("pontod_freq_nl_type_entry, create proc failed!");
        }

        pontod_freq_recollect_entry = proc_create("recollect", 0644, pontod_freq_proc_dir, &pontod_freq_recollect_fop);
        if (!pontod_freq_recollect_entry) {
            printk("pontod_freq_recollect_entry, create proc failed!");
        }

        pontod_freq_set_freq_entry = proc_create("set_freq", 0644, pontod_freq_proc_dir, &pontod_freq_set_freq_fop);
        if (!pontod_freq_set_freq_entry) {
            printk("pontod_freq_set_freq_entry, create proc failed!");
        }

        pontod_freq_sample_method_entry = proc_create("sample_method", 0644, pontod_freq_proc_dir, &pontod_freq_sample_method_fop);
        if (!pontod_freq_sample_method_entry) {
            printk("pontod_freq_sample_method_entry, create proc failed!");
        }

        pontod_freq_sample_ms_entry = proc_create("sample_ms", 0644, pontod_freq_proc_dir, &pontod_freq_sample_ms_fop);
        if (!pontod_freq_sample_ms_entry) {
            printk("pontod_freq_sample_ms_entry, create proc failed!");
        }

        pontod_freq_print_entry = proc_create("print", 0644, pontod_freq_proc_dir, &pontod_freq_print_fop);
        if (!pontod_freq_print_entry) {
            printk("pontod_freq_print_entry, create proc failed!");
        }
    }


    if(rtk_ponmac_mode_get(&PON_MODE) != RT_ERR_OK)
    {
        printk("%s:%d PON ToD Collect ToD rtk_ponmac_mode_get failed!\n", __FILE__, __LINE__);
    }

    return 0;
}

void __exit pontod_freq_drv_exit(void)
{
    if(pontod_freq_print_entry)
    {
        remove_proc_entry("print", pontod_freq_proc_dir);
        pontod_freq_print_entry = NULL;
    }

    if(pontod_freq_sample_ms_entry)
    {
        remove_proc_entry("sample_ms", pontod_freq_proc_dir);
        pontod_freq_sample_ms_entry = NULL;
    }

    if(pontod_freq_sample_method_entry)
    {
        remove_proc_entry("sample_method", pontod_freq_proc_dir);
        pontod_freq_sample_method_entry = NULL;
    }

    if(pontod_freq_set_freq_entry)
    {
        remove_proc_entry("set_freq", pontod_freq_proc_dir);
        pontod_freq_set_freq_entry = NULL;
    }

    if(pontod_freq_recollect_entry)
    {
        remove_proc_entry("recollect", pontod_freq_proc_dir);
        pontod_freq_recollect_entry = NULL;
    }

    if(pontod_freq_nl_type_entry)
    {
        remove_proc_entry("nl_type", pontod_freq_proc_dir);
        pontod_freq_nl_type_entry = NULL;
    }

    if(pontod_freq_enable_entry)
    {
        remove_proc_entry("enable", pontod_freq_proc_dir);
        pontod_freq_enable_entry = NULL;
    }
    
    if(pontod_freq_proc_dir)
    {
        remove_proc_entry("pontod_freq_drv", NULL);
        pontod_freq_proc_dir = NULL;
    }

    if(pPONToDColToDThread!=NULL)
        kthread_stop(pPONToDColToDThread);
    if(pPONToDColToDQueue!=NULL)
    {
        PonToDNLData_t* item=NULL;
        while(!qIsEmpty(pPONToDColToDQueue))
        {
            item=NULL;
            item=(PonToDNLData_t *)qDequeue(pPONToDColToDQueue);
            kfree(item);
        }
    }

    netlink_kernel_release(sk);
}

EXPORT_SYMBOL(pontod_freq_tod_update_time);
EXPORT_SYMBOL(pontod_freq_tod_update_enable);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("RealTek PONToD tune Time Frequency drive module");
MODULE_AUTHOR("Realtek");
module_init(pontod_freq_drv_init);
module_exit(pontod_freq_drv_exit);
