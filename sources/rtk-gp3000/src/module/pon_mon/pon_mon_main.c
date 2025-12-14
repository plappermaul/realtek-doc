/*
 * Copyright (C) 2019 Realtek Semiconductor Corp.
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
 * $Revision: 73516 $
 * $Date: 2019-02-22 15:14:09 +0800 (Tue, 22 Nov 2016) $
 *
 * Purpose : PON monitor kernel thread
 *
 * Feature : Use kernel thread to perodically polling the use-page and dump bwmap
 *
 */

/*
 * Include Files
 */
#include <linux/version.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/slab.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
#include <linux/timekeeping.h>
#include <linux/time64.h>
#else
#include <linux/timer.h>
#include <linux/timex.h>
#endif
#include <linux/rtc.h>



/* For RTK APIs */
#include <common/rt_error.h>
#include <rtk/switch.h>
#include <rtk/pbo.h>
#include <ioal/mem32.h>
#include <osal/time.h>

#if defined(CONFIG_SDK_RTL9601B)
#include <dal/rtl9601b/dal_rtl9601b_pbo.h>
#endif
#if defined(CONFIG_SDK_RTL9602C)
#include <dal/rtl9602c/dal_rtl9602c_pbo.h>
#endif
#if defined(CONFIG_SDK_RTL9607C)
#include <dal/rtl9607c/dal_rtl9607c_pbo.h>
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
#include <dal/rtl9603cvd/dal_rtl9603cvd_pbo.h>
#endif


/*
 * Symbol Definition
 */
typedef struct pon_mon_bwmap_raw_s{
    uint32 bwm_1;
    uint32 bwm_2;
}pon_mon_bwmap_raw_t;

typedef struct pon_mon_bwmap_capture_s{
    uint32 hit_state;
    uint32 max_used_page;
    pon_mon_bwmap_raw_t *bwm_entry;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
	struct timespec64 time;
#else
    struct timeval time;    
#endif

}pon_mon_capture_t;

typedef struct pon_mon_cfg_s{
    uint32 state;
    uint32 interval;
    uint32 page_threshold;
    uint32 capture_times;
}pon_mon_cfg_t;

typedef struct pon_mon_used_page_s{
    uint32 max_page;
    uint32 current_page;
    uint64 time;
    uint64 total;
}pon_mon_used_page_t;

/*
 * Data Declaration
 */
struct proc_dir_entry* pon_mon_proc_dir = NULL;
struct proc_dir_entry* pon_mon_cfg_entry = NULL;
struct proc_dir_entry* pon_mon_enable_entry = NULL;
struct proc_dir_entry* pon_mon_output_entry = NULL;
struct proc_dir_entry* pon_mon_used_page_entry = NULL;

struct task_struct *pPonMonTask;
static pon_mon_capture_t pon_mon_output;
static pon_mon_cfg_t pon_mon_cfg;
static pon_mon_used_page_t pon_mon_used_page;

struct task_struct *pDspboQueueTask;

static uint32 chipId, rev, subtype;


/*
 * Macro Definition
 */
#define DEF_INTERVAL (1000)
#define DEF_TIMES (100)
#define DEF_THRESHOLD (3000)

#define BWM_CAP_PAGE                0x2
#define BWM_CAP_CTL_OFFSET          0x003
#define BWM_CAP_OVERFLOW_OFFSET     0x004
#define BWM_CAP_DATA_START_OFFSET   0x100
#define BWM_CAP_DATA_END_OFFSET     0x1ff
#define BWM_CAP_CTL_ENA             15
#define BWM_CAP_CTL_CLR             14
#define BWM_CAP_CTL_FRAMNUM         0
#define BWM_CAP_OVERFL              8

#define BWM_BUF_ENTRY 32

/*
 * Function Declaration
 */

static int pon_mon_cfg_read(struct seq_file *seq, void *v)
{
    seq_printf(seq, "cfg input: interval(10~60000 ms) used_page_threshold(1~8000) capture_times(1~1000)\n");
    seq_printf(seq, "interval: %u ms\n", pon_mon_cfg.interval);
    seq_printf(seq, "used_page threshold: %u\n", pon_mon_cfg.page_threshold);
    seq_printf(seq, "capture times: %u\n", pon_mon_cfg.capture_times);

    return 0;
}

static int pon_mon_cfg_open_proc(struct inode *inode, struct file *file)
{
    return single_open(file, pon_mon_cfg_read, inode->i_private);
}

static int pon_mon_cfg_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos)
{
    unsigned char tmp[32] = { 0 },*p,*para[3]={NULL};
    int len = (count > 31) ? 31 : count;
    int n=0, i;

    if(pon_mon_cfg.state == 1)
    {
        printk("please set enable to 0 before setting cfg!\n");
        return (count);
    }

    if (buffer && !copy_from_user(tmp, buffer, len)) {
        p=tmp;
        para[0]=p;
        for(i=0;i<10||i<count;i++)
        {
            if(*p=='\n')
                break;
            if(*p==' ')
            {
                *p='\0';
                para[++n]=++p;

            }
            p++;
            if(n>=2)
                break;
        }
        for(n=0;n<3;n++)
        {
            if(para[n]==NULL)
                return (count);
        }
        pon_mon_cfg.interval=simple_strtoul(para[0], NULL, 10);
        if(pon_mon_cfg.interval < 10 || pon_mon_cfg.interval > 60000)
        {
            printk("interval must between 10~60000 ms!\n");
            pon_mon_cfg.interval = DEF_INTERVAL;
            return (count);
        }
        pon_mon_cfg.page_threshold=simple_strtoul(para[1], NULL, 10);
        if(pon_mon_cfg.page_threshold < 1 || pon_mon_cfg.page_threshold > 8000)
        {
            printk("used_page threshold must between 1~8000!\n");
            pon_mon_cfg.page_threshold = DEF_THRESHOLD;
            return (count);
        }
        pon_mon_cfg.capture_times=simple_strtoul(para[2], NULL, 10);
        if(pon_mon_cfg.capture_times < 1 || pon_mon_cfg.capture_times > 1000)
        {
            printk("capture times must between 1~1000!\n");
            pon_mon_cfg.capture_times = DEF_TIMES;
            return (count);
        }

        printk("set pon_mon cfg, interval=%u, used_page_threshold=%u, capture_times=%u\n",
        pon_mon_cfg.interval, pon_mon_cfg.page_threshold, pon_mon_cfg.capture_times);
    }
    return (count);
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
static const struct proc_ops pon_mon_cfg_fop = {
    .proc_open      = pon_mon_cfg_open_proc,
    .proc_write     = pon_mon_cfg_write,
    .proc_read      = seq_read,
    .proc_lseek     = seq_lseek,
    .proc_release   = single_release,
};
#else
static const struct file_operations pon_mon_cfg_fop = {
    .owner          = THIS_MODULE,
    .open           = pon_mon_cfg_open_proc,
    .write          = pon_mon_cfg_write,
    .read           = seq_read,
    .llseek         = seq_lseek,
    .release        = single_release,
};
#endif

static int pon_mon_enable_read(struct seq_file *seq, void *v)
{
    seq_printf(seq, "enable state: %u\n", pon_mon_cfg.state);

    return 0;
}

static int pon_mon_enable_open_proc(struct inode *inode, struct file *file)
{
    return single_open(file, pon_mon_enable_read, inode->i_private);
}

static int pon_mon_enable_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos)
{
    unsigned char tmpBuf[16] = {0};
    int len = (count > 15) ? 15 : count;
    int32 ret;
    
    if (buffer && !copy_from_user(tmpBuf, buffer, len))
    {
        pon_mon_cfg.state = simple_strtoul(tmpBuf, NULL, 16);
        if(pon_mon_cfg.state == 1)
        {
            pon_mon_output.max_used_page = 0;
            pon_mon_output.hit_state = 0;

            if(pon_mon_output.bwm_entry != NULL)
            {
                kfree(pon_mon_output.bwm_entry);
                pon_mon_output.bwm_entry = NULL;
            }

            pon_mon_output.bwm_entry = kmalloc(sizeof(pon_mon_bwmap_raw_t)*pon_mon_cfg.capture_times, GFP_KERNEL);
            if(pon_mon_output.bwm_entry == NULL)
            {
                printk("set pon_mon enable: alloc memory fail!!\n");
                pon_mon_cfg.state = 0;
                return count;
            }

            memset(&pon_mon_used_page, 0, sizeof(pon_mon_used_page_t));
        }

        /* clear max used page */
        switch(chipId)
        {
#if defined(CONFIG_SDK_RTL9601B)
            case RTL9601B_CHIP_ID:
                if((ret = rtl9601b_raw_pbo_maxUsedPageCount_clear())!= RT_ERR_OK)
                {
                    printk("get max used page fail ret=0x%x",ret);
                    return -1;
                }
                break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
            case RTL9602C_CHIP_ID:
                if((ret = rtl9602c_raw_pbo_usMaxUsedPageCount_clear())!= RT_ERR_OK)
                {
                    printk("get max used page fail ret=0x%x",ret);
                    return -1;
                }
                break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
            case RTL9607C_CHIP_ID:
                if((ret = rtl9607c_raw_pbo_usMaxUsedPageCount_clear())!= RT_ERR_OK)
                {
                    printk("get max used page fail ret=0x%x",ret);
                    return -1;
                }
                break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
            case RTL9603CVD_CHIP_ID:
                if((ret = rtl9603cvd_raw_pbo_usMaxUsedPageCount_clear())!= RT_ERR_OK)
                {
                    printk("get max used page fail ret=0x%x",ret);
                    return -1;
                }
                break;
#endif
            default:
            break;
        }

        printk("set pon_mon enable state to %u\n", pon_mon_cfg.state);
        return count;
    }
    return -EFAULT;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
static const struct proc_ops pon_mon_enable_fop = {
    .proc_open      = pon_mon_enable_open_proc,
    .proc_write     = pon_mon_enable_write,
    .proc_read      = seq_read,
    .proc_lseek     = seq_lseek,
    .proc_release   = single_release,
};
#else
static const struct file_operations pon_mon_enable_fop = {
    .owner          = THIS_MODULE,
    .open           = pon_mon_enable_open_proc,
    .write          = pon_mon_enable_write,
    .read           = seq_read,
    .llseek         = seq_lseek,
    .release        = single_release,
};
#endif

static void *pon_mon_output_start(struct seq_file *s, loff_t *pos) 
{ 
    if ((*pos >= pon_mon_cfg.capture_times) || (pon_mon_output.bwm_entry == NULL)) 
        return NULL;

    return (void *)((unsigned long)(*pos)+1);
} 
/**
* This function is called after the beginning of a sequence.
* It's called untill the return is NULL (this ends the sequence).
*
*/ 
static void *pon_mon_output_next(struct seq_file *s, void *v, loff_t *pos) 
{ 
    ++*pos;

    return pon_mon_output_start(s, pos);
} 
/**
* This function is called at the end of a sequence
*
*/ 
static void pon_mon_output_stop(struct seq_file *s, void *v) 
{ 
} 

static int dump_bwm(struct seq_file *s, uint32 bw1, uint32 bw2){

    uint8 frm_count=0, lst= 0, eob =0, sob=0;
    uint8 plm = 0,fec =0 ,dbr =0 ,mf =0 ;
    uint32 tcont =0 ,sstart =0,sstop =0,blen =0;
    uint32 vaild = 0;

    vaild       = (bw1 >> 23) & 0x1;
    frm_count   = (bw1 >> 24) & 0xf;
    lst             = (bw1 >> 22) & 0x1;
    eob             = (bw1 >> 21) & 0x1;
    sob         = (bw1 >> 20) & 0x1;
    plm         = (bw1 >> 19) & 0x1;
    fec         = (bw1 >> 18) & 0x1;
    dbr         = (bw1 >> 16) & 0x3;
    mf          = (bw1 >> 12) & 0x7;
    tcont       = (bw1 & 0x1f);
    sstart      = bw2 & 0xffff;
    sstop       = (bw2 >> 16) & 0xffff;
    blen            = sstop +1 - sstart;

    if(vaild){
        seq_printf(s, "%3d   %1d   %2d  |  %1d   %1d   %1d  | %5d(0x%04x) - %5d(0x%04x) %5d  |  %1d   %1d   %1d\n",
        frm_count,mf,tcont,plm,fec,dbr,sstart,sstart,sstop,sstop,blen,sob,eob,lst);
    }
    return 0;
}

/**
* This function is called for each "step" of a sequence
*
*/ 
static int pon_mon_output_show(struct seq_file *s, void *v) 
{ 
    uint32 n;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
	time64_t local_time;
#else
    unsigned long local_time;
#endif
    struct rtc_time tm;

    n = (uint32)v-1;     

    if(n == 0)
    {
        seq_printf(s, "hit state=%u\n", pon_mon_output.hit_state);
        seq_printf(s, "max_used_page=%u\n", pon_mon_output.max_used_page);

		
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
        local_time = pon_mon_output.time.tv_sec - (sys_tz.tz_minuteswest * 60);
        rtc_time64_to_tm(local_time, &tm);
#else
        local_time = (uint32)(pon_mon_output.time.tv_sec - (sys_tz.tz_minuteswest * 60));
        rtc_time_to_tm(local_time, &tm);
#endif

        seq_printf(s, "date-time: %04d-%02d-%02d %02d:%02d:%02d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
        seq_printf(s, "================================================================================\n");
        seq_printf(s, " FR# MF TCONT   PLM FEC DBR      SStart           SStop      BLen    SoB EoB LST\n");
        seq_printf(s, "--------------------------------------------------------------------------------\n");
    }
    dump_bwm(s, pon_mon_output.bwm_entry[n].bwm_1, pon_mon_output.bwm_entry[n].bwm_2);

    return 0;
} 

static const struct seq_operations pon_mon_output_ops = { 
    .start = pon_mon_output_start,
    .next = pon_mon_output_next,
    .stop = pon_mon_output_stop,
    .show = pon_mon_output_show 
};

static int pon_mon_output_open_proc(struct inode *inode, struct file *file)
{
    return seq_open(file, &pon_mon_output_ops);
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
static const struct proc_ops pon_mon_output_fop = {
    .proc_open      = pon_mon_output_open_proc,
    .proc_read      = seq_read,
    .proc_lseek     = seq_lseek,
    .proc_release   = seq_release,
};
#else
static const struct file_operations pon_mon_output_fop = {
    .owner          = THIS_MODULE,
    .open           = pon_mon_output_open_proc,
    .read           = seq_read,
    .llseek         = seq_lseek,
    .release        = seq_release,
};
#endif


static void pon_mon_bwmap_record(void)
{
    uint32 addr, value;
    int32 ret,offset;
    uint32 i,j,loop_times;

    if(pon_mon_cfg.capture_times == 0)
        return;
    printk("pon_mon_bwmap_record start!!\n");

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
    ktime_get_real_ts64(&pon_mon_output.time);
    printk("sec:%lld, nsec:%ld\n", pon_mon_output.time.tv_sec, pon_mon_output.time.tv_nsec);
#else
    do_gettimeofday(&pon_mon_output.time);
    printk("sec:%d, usec:%d\n", pon_mon_output.time.tv_sec, pon_mon_output.time.tv_usec);
#endif
    
    for(i = 0; i<=((pon_mon_cfg.capture_times-1)/BWM_BUF_ENTRY); i++)
    {
        /*clear bwmap capture*/
        addr = ( BWM_CAP_PAGE << 12) | (BWM_CAP_CTL_OFFSET)*4 | 0x700000;
        value = 0;
        if((ret = ioal_mem32_write(addr, value)) != RT_ERR_OK)
        {
            printk("ioal_mem32_write fail! ret=0x%x\n",ret);
            return;
        }
        value = (1 << BWM_CAP_CTL_CLR) | (BWM_BUF_ENTRY <<  BWM_CAP_CTL_FRAMNUM);
        if((ret = ioal_mem32_write(addr, value)) != RT_ERR_OK)
        {
            printk("ioal_mem32_write fail! ret=0x%x\n",ret);
            return;
        }
        
        /*start to bwmap capture*/
        value = (1 << BWM_CAP_CTL_ENA) | (BWM_BUF_ENTRY <<  BWM_CAP_CTL_FRAMNUM);
        if((ret = ioal_mem32_write(addr, value)) != RT_ERR_OK)
        {
            printk("ioal_mem32_write fail! ret=0x%x\n",ret);
            return;
        }
        osal_time_usleep(5000);

        /*read bwm data*/
        if(i == ((pon_mon_cfg.capture_times-1)/BWM_BUF_ENTRY))
            loop_times = (pon_mon_cfg.capture_times-1)%BWM_BUF_ENTRY;
        else
            loop_times = BWM_BUF_ENTRY-1;

        for(j = 0; j <= loop_times; j ++){
            pon_mon_output.bwm_entry[i*BWM_BUF_ENTRY+j].bwm_1 = pon_mon_output.bwm_entry[i*BWM_BUF_ENTRY+j].bwm_2 = 0;
            offset = BWM_CAP_DATA_START_OFFSET+(j*2);
        
            addr = ( BWM_CAP_PAGE << 12) | (offset)*4 | 0x700000;
            if((ret = ioal_mem32_read(addr, &pon_mon_output.bwm_entry[i*BWM_BUF_ENTRY+j].bwm_1)) != RT_ERR_OK)
            {
                printk("ioal_mem32_write fail! ret=0x%x\n",ret);
                return;
            }
            addr = ( BWM_CAP_PAGE << 12) | (offset+1)*4 | 0x700000;
            if((ret = ioal_mem32_read(addr, &pon_mon_output.bwm_entry[i*BWM_BUF_ENTRY+j].bwm_2)) != RT_ERR_OK)
            {
                printk("ioal_mem32_write fail! ret=0x%x\n",ret);
                return;
            }
        }
    }
    printk("pon_mon_bwmap_record end!!\n");
}

static int pon_mon_used_page_read(struct seq_file *seq, void *v)
{
    uint64 average=0;

    seq_printf(seq, "current used_page: %u\n", pon_mon_used_page.current_page);
    seq_printf(seq, "maximum used_page: %u\n", pon_mon_used_page.max_page);
    if(pon_mon_used_page.time != 0)
    {
        average = pon_mon_used_page.total;
        do_div(average,pon_mon_used_page.time);
    }
    seq_printf(seq, "average used_page: %llu\n", average);

    return 0;
}

static int pon_mon_used_page_open_proc(struct inode *inode, struct file *file)
{
    return single_open(file, pon_mon_used_page_read, inode->i_private);
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
static const struct proc_ops pon_mon_used_page_fop = {
    .proc_open      = pon_mon_used_page_open_proc,
    .proc_read      = seq_read,
    .proc_lseek     = seq_lseek,
    .proc_release   = single_release,
};
#else
static const struct file_operations pon_mon_used_page_fop = {
    .owner          = THIS_MODULE,
    .open           = pon_mon_used_page_open_proc,
    .read           = seq_read,
    .llseek         = seq_lseek,
    .release        = single_release,
};
#endif

int pon_mon_thread(void *data)
{
    uint32 sram, dram;
    int32 ret;

    while(!kthread_should_stop())
    {
        if(pon_mon_cfg.state == 1 && pon_mon_output.hit_state == 0)
        {
            /* monitor the max used page */
            switch(chipId)
            {
#if defined(CONFIG_SDK_RTL9601B)
                case RTL9601B_CHIP_ID:
                    if((ret = rtl9601b_raw_pbo_maxUsedPageCount_get(&sram, &dram))!= RT_ERR_OK)
                    {
                        printk("get max used page fail ret=0x%x",ret);
                        return -1;
                    }
                    break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
                case RTL9602C_CHIP_ID:
                    if((ret = rtl9602c_raw_pbo_usMaxUsedPageCount_get(&sram, &dram))!= RT_ERR_OK)
                    {
                        printk("get max used page fail ret=0x%x",ret);
                        return -1;
                    }
                    break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
                case RTL9607C_CHIP_ID:
                    if((ret = rtl9607c_raw_pbo_usMaxUsedPageCount_get(&sram, &dram))!= RT_ERR_OK)
                    {
                        printk("get max used page fail ret=0x%x",ret);
                        return -1;
                    }
                    break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
                case RTL9603CVD_CHIP_ID:
                    if((ret = rtl9603cvd_raw_pbo_usMaxUsedPageCount_get(&sram, &dram))!= RT_ERR_OK)
                    {
                        printk("get max used page fail ret=0x%x",ret);
                        return -1;
                    }
                    break;
#endif
                default:
                    sram=0;
                    dram=0;
                break;
            }
            /* clear max used page */
             switch(chipId)
            {
#if defined(CONFIG_SDK_RTL9601B)
                case RTL9601B_CHIP_ID:
                    if((ret = rtl9601b_raw_pbo_maxUsedPageCount_clear())!= RT_ERR_OK)
                    {
                        printk("get max used page fail ret=0x%x",ret);
                        return -1;
                    }
                    break;
#endif
#if defined(CONFIG_SDK_RTL9602C)
                case RTL9602C_CHIP_ID:
                    if((ret = rtl9602c_raw_pbo_usMaxUsedPageCount_clear())!= RT_ERR_OK)
                    {
                        printk("get max used page fail ret=0x%x",ret);
                        return -1;
                    }
                    break;
#endif
#if defined(CONFIG_SDK_RTL9607C)
                case RTL9607C_CHIP_ID:
                    if((ret = rtl9607c_raw_pbo_usMaxUsedPageCount_clear())!= RT_ERR_OK)
                    {
                        printk("get max used page fail ret=0x%x",ret);
                        return -1;
                    }
                    break;
#endif
#if defined(CONFIG_SDK_RTL9603CVD)
                case RTL9603CVD_CHIP_ID:
                    if((ret = rtl9603cvd_raw_pbo_usMaxUsedPageCount_clear())!= RT_ERR_OK)
                    {
                        printk("get max used page fail ret=0x%x",ret);
                        return -1;
                    }
                    break;
#endif
                default:
                break;
            }   
            
            /* if max used page > threshold, then record data in output */
            if(dram > pon_mon_cfg.page_threshold)
            {
                pon_mon_bwmap_record();
                pon_mon_output.hit_state = 1;
                pon_mon_output.max_used_page = dram;
            }
            //printk("max used page=%u\n",dram);
            
            /* record used page info*/
            pon_mon_used_page.current_page = dram;
            if(pon_mon_used_page.max_page < dram)
                pon_mon_used_page.max_page = dram;
            pon_mon_used_page.time++;
            pon_mon_used_page.total += dram;

            /* It is NOT possible to wake up earlier */
            set_current_state(TASK_UNINTERRUPTIBLE);
            schedule_timeout( pon_mon_cfg.interval/(1000/HZ) ); /* Wake up every interval ms */
        }
        else
        {
            /* It is NOT possible to wake up earlier */
            set_current_state(TASK_UNINTERRUPTIBLE);
            schedule_timeout( DEF_INTERVAL/(1000/HZ) ); /* Wake up every 1 s*/
        }
    }

    return (RT_ERR_OK);
}


int __init pon_mon_init(void)
{
    pon_mon_cfg.state = 0;
    pon_mon_cfg.interval = DEF_INTERVAL;
    pon_mon_cfg.capture_times = DEF_TIMES;
    pon_mon_cfg.page_threshold = DEF_THRESHOLD;

    /* get chip type */
    if(rtk_switch_version_get(&chipId, &rev, &subtype) != RT_ERR_OK)
        return -1;

    pon_mon_output.max_used_page = 0;
    pon_mon_output.hit_state = 0;
    pon_mon_output.bwm_entry = NULL;

    memset(&pon_mon_used_page, 0, sizeof(pon_mon_used_page_t));

    if (NULL == pon_mon_proc_dir) {
        pon_mon_proc_dir = proc_mkdir ("pon_mon", NULL);
    }

    if(pon_mon_proc_dir) {
        pon_mon_cfg_entry = proc_create("cfg", 0644, pon_mon_proc_dir, &pon_mon_cfg_fop);
        if (!pon_mon_cfg_entry) {
            printk("pon_mon_cfg_entry, create proc failed!");
        }
    }

    if(pon_mon_proc_dir) {
        pon_mon_enable_entry = proc_create("enable", 0644, pon_mon_proc_dir, &pon_mon_enable_fop);
        if (!pon_mon_enable_entry) {
            printk("pon_mon_enable_entry, create proc failed!");
        }
    }

    if(pon_mon_proc_dir) {
        pon_mon_output_entry = proc_create("output", 0444, pon_mon_proc_dir, &pon_mon_output_fop);
        if (!pon_mon_output_entry) {
            printk("pon_mon_output_entry, create proc failed!");
        }
    }
    
    if(pon_mon_proc_dir) {
        pon_mon_used_page_entry = proc_create("used_page", 0444, pon_mon_proc_dir, &pon_mon_used_page_fop);
        if (!pon_mon_used_page_entry) {
            printk("pon_mon_used_page_entry, create proc failed!");
        }
    }

    pPonMonTask = kthread_run(pon_mon_thread, NULL, "pon_mon");
    if(IS_ERR(pPonMonTask)) {
        printk("%s:%d pPonMonTask failed %ld!\n", __FILE__, __LINE__, PTR_ERR(pPonMonTask));
    }
    else {
        printk("%s:%d pPonMonTask complete!\n", __FILE__, __LINE__);
    }

    return 0;
}

void __exit pon_mon_exit(void)
{
    kthread_stop(pPonMonTask);
    if(pon_mon_output.bwm_entry) {
        kfree(pon_mon_output.bwm_entry);
        pon_mon_output.bwm_entry = NULL;
    }

    if(pon_mon_cfg_entry) {
        remove_proc_entry("cfg", pon_mon_cfg_entry);
        pon_mon_cfg_entry = NULL;
    }

    if(pon_mon_enable_entry) {
        remove_proc_entry("enable", pon_mon_enable_entry);
        pon_mon_enable_entry = NULL;
    }

    if(pon_mon_output_entry) {
        remove_proc_entry("output", pon_mon_output_entry);
        pon_mon_output_entry = NULL;
    }
    
    if(pon_mon_used_page_entry) {
        remove_proc_entry("used_page", pon_mon_used_page_entry);
        pon_mon_used_page_entry = NULL;
    }

    if(pon_mon_proc_dir) {
        remove_proc_entry("pon_mon", NULL);
        pon_mon_proc_dir = NULL;
    }
}

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("RealTek PON Monitor module");
MODULE_AUTHOR("Realtek");
module_init(pon_mon_init);
module_exit(pon_mon_exit);

