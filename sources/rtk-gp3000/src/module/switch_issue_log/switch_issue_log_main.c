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
 * $Revision:  $
 * $Date:  $
 *
 * Purpose : Switch Issue Log kernel drivers
 *
 * Feature : This module install Switch Issue Log kernel callbacks and other 
 * kernel functions
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/fs.h>
#include <linux/time.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/version.h>
#include "soc/type.h"

#if defined(CONFIG_SDK_RTL9602C)
#include <dal/rtl9602c/dal_rtl9602c.h>
#include <dal/rtl9602c/dal_rtl9602c_intr.h>
#else if defined(CONFIG_SDK_RTL9607C)
#include <dal/rtl9607c/dal_rtl9607c.h>
#include <dal/rtl9607c/dal_rtl9607c_intr.h>
#endif

struct proc_dir_entry* switch_issue_log_proc_dir = NULL;
struct proc_dir_entry* switch_issue_log_trigger_entry = NULL;
struct proc_dir_entry* switch_issue_log_file_size_entry = NULL;
struct proc_dir_entry* switch_issue_log_print_entry = NULL;
struct proc_dir_entry* switch_issue_log_path_entry = NULL;

static struct task_struct *pSwIssueLogThread=NULL;

DECLARE_WAIT_QUEUE_HEAD(wqueue);

#define MAX_FILE_PATH_LEN 200

#define REG_API_ERR_CHK(op, ret)\
    do {\
        if ((ret = (op)) != RT_ERR_OK)\
        {\
            printk("[%s:%d] Reg API Error! ret=%d \n", __FUNCTION__, __LINE__,ret); \
        }\
    } while(0)

static char filePath[MAX_FILE_PATH_LEN]="/var/config/switch_issue.log";

static uint32 file_size=81920; 
static uint8 print_log=0; 

char logBuf[1000];

void log_save_to_file(char * buf,uint32 len)
{
    struct file *f;
    mm_segment_t old_fs;

    if(print_log==1)
    {
        printk("%s",buf);
    }

    old_fs = get_fs();//Save the current FS segment
    set_fs(get_ds());

    f=filp_open(filePath,O_RDWR|O_CREAT|O_APPEND ,0644);
    if(IS_ERR(f))
    {
        printk("Switch Issue Log: can't create log file\n");
    }
    else
    {

        if(f->f_dentry->d_inode->i_size>file_size)
        {
            if(print_log==1)
                printk("file size=%lld is over max log file size %u dumping file to console as following\n",f->f_dentry->d_inode->i_size,file_size);
        }
        else
        {
            vfs_write(f, (char *)buf, len, &f->f_pos);
        }

    }
    set_fs(old_fs); //Reset to save fs
}


void switch_issue_log_isr(void)
{
    wake_up_interruptible(&wqueue);
}


static int SW_Issue_Log_thread(void *thread_data)
{
    struct timeval tv;
    uint32 len=0,data,value;
    int32 ret = RT_ERR_OK;

    while(!kthread_should_stop())
    {
        DECLARE_WAITQUEUE(wait,current);
        set_current_state(TASK_INTERRUPTIBLE);
        add_wait_queue(&wqueue, &wait);
        len=0;
        ret=RT_ERR_OK;

        schedule();

        do_gettimeofday(&tv);

        len=sprintf(logBuf+len,"***Switch Issue %usec %ums***\n",tv.tv_sec,tv.tv_usec);

#if defined(CONFIG_SDK_RTL9602C)
        data=0x0;
        REG_API_ERR_CHK(reg_read(RTL9602C_CRASH_INTRr, &data),ret);

        value=0x0;
        REG_API_ERR_CHK(reg_field_get(RTL9602C_CRASH_INTRr,RTL9602C_GPHY_FATAL_ERRORf,&value,&data),ret);
        len+=sprintf(logBuf+len,"GPHY_FATAL_ERRORf=0x%x\n",value);

        value=0x0;
        REG_API_ERR_CHK(reg_field_get(RTL9602C_CRASH_INTRr,RTL9602C_TXESD_FAILf,&value,&data),ret);
        len+=sprintf(logBuf+len,"TXESD_FAILf=0x%x\n",value);

        value=0x0;
        REG_API_ERR_CHK(reg_field_get(RTL9602C_CRASH_INTRr,RTL9602C_BUFFRST_SYSOVERf,&value,&data),ret);
        len+=sprintf(logBuf+len,"BUFFRST_SYSOVERf=0x%x\n",value);

        value=0x0;
        REG_API_ERR_CHK(reg_field_get(RTL9602C_CRASH_INTRr,RTL9602C_BUFFRST_QOVERf,&value,&data),ret);
        len+=sprintf(logBuf+len,"BUFFRST_QOVERf=0x%x\n",value);

        value=0x0;
        REG_API_ERR_CHK(reg_field_get(RTL9602C_CRASH_INTRr,RTL9602C_BUFFRST_DSCOVERf,&value,&data),ret);
        len+=sprintf(logBuf+len,"BUFFRST_DSCOVERf=0x%x\n",value);

#else if defined(CONFIG_SDK_RTL9607C)
        data=0;
        REG_API_ERR_CHK(reg_read(RTL9607C_CRASH_INTR_STS_0r, &data),ret);

        value=0x0;
        REG_API_ERR_CHK(reg_field_get(RTL9607C_CRASH_INTR_STS_0r,RTL9607C_PHY_FATAL_ERRf,&value,&data),ret);
        len+=sprintf(logBuf+len,"PHY_FATAL_ERRf=0x%x\n",value);

        value=0x0;
        REG_API_ERR_CHK(reg_field_get(RTL9607C_CRASH_INTR_STS_0r,RTL9607C_BUFFRST_SYSOVERf,&value,&data),ret);
        len+=sprintf(logBuf+len,"BUFFRST_SYSOVERf=0x%x\n",value);

        value=0x0;
        REG_API_ERR_CHK(reg_field_get(RTL9607C_CRASH_INTR_STS_0r,RTL9607C_BUFFRST_QOVERf,&value,&data),ret);
        len+=sprintf(logBuf+len,"BUFFRST_QOVERf=0x%x\n",value);

        value=0x0;
        REG_API_ERR_CHK(reg_field_get(RTL9607C_CRASH_INTR_STS_0r,RTL9607C_BUFFRST_DSCOVERf,&value,&data),ret);
        len+=sprintf(logBuf+len,"BUFFRST_DSCOVERf=0x%x\n",value);

        data=0;
        REG_API_ERR_CHK(reg_read(RTL9607C_CRASH_INTR_STS_1r, &data),ret);

        value=0x0;
        REG_API_ERR_CHK(reg_field_get(RTL9607C_CRASH_INTR_STS_1r,RTL9607C_RX_DSC_ERRf,&value,&data),ret);
        len+=sprintf(logBuf+len,"RX_DSC_ERRf=0x%x\n",value);
        
        value=0x0;
        REG_API_ERR_CHK(reg_field_get(RTL9607C_CRASH_INTR_STS_1r,RTL9607C_TXESD_FAILf,&value,&data),ret);
        len+=sprintf(logBuf+len,"TXESD_FAILf=0x%x\n",value);
#endif
        log_save_to_file(logBuf,len);

        remove_wait_queue(&wqueue,&wait);
    }

    set_current_state(TASK_RUNNING);
    
    return 0;
}

static int switch_issue_log_trigger_read(struct seq_file *seq, void *v)
{

    seq_printf(seq, "Using software to trigger interupt function check status\n");

    return 0;
}

static int switch_issue_log_trigger_open_proc(struct inode *inode, struct file *file)
{
    return single_open(file, switch_issue_log_trigger_read, inode->i_private);
}

static int switch_issue_log_trigger_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos)
{
    unsigned char tmpBuf[16] = {0};
    int len = (count > 15) ? 15 : count;
    
    if (buffer && !copy_from_user(tmpBuf, buffer, len))
    {
        if(simple_strtoul(tmpBuf, NULL, 16)!=0)
        {
            switch_issue_log_isr();
        }
        return count;
    }
    return -EFAULT;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
static const struct proc_ops switch_issue_log_trigger_fop = {
    .proc_open      = switch_issue_log_trigger_open_proc,
    .proc_write     = switch_issue_log_trigger_write,
    .proc_read      = seq_read,
    .proc_lseek     = seq_lseek,
    .proc_release   = single_release,
};
#else
static const struct file_operations switch_issue_log_trigger_fop = {
    .owner          = THIS_MODULE,
    .open           = switch_issue_log_trigger_open_proc,
    .write          = switch_issue_log_trigger_write,
    .read           = seq_read,
    .llseek         = seq_lseek,
    .release        = single_release,
};
#endif


static int switch_issue_log_file_size_read(struct seq_file *seq, void *v)
{

    seq_printf(seq, "max file size=%u\n",file_size);

    return 0;
}

static int switch_issue_log_file_size_open_proc(struct inode *inode, struct file *file)
{
    return single_open(file, switch_issue_log_file_size_read, inode->i_private);
}

static int switch_issue_log_file_size_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos)
{
    unsigned char tmpBuf[16] = {0};
    int len = (count > 15) ? 15 : count;
    
    if (buffer && !copy_from_user(tmpBuf, buffer, len))
    {
        file_size=simple_strtoul(tmpBuf, NULL, 10);
        return count;
    }
    return -EFAULT;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
static const struct proc_ops switch_issue_log_file_size_fop = {
    .proc_open      = switch_issue_log_file_size_open_proc,
    .proc_write     = switch_issue_log_file_size_write,
    .proc_read      = seq_read,
    .proc_lseek     = seq_lseek,
    .proc_release   = single_release,
};
#else
static const struct file_operations switch_issue_log_file_size_fop = {
    .owner          = THIS_MODULE,
    .open           = switch_issue_log_file_size_open_proc,
    .write          = switch_issue_log_file_size_write,
    .read           = seq_read,
    .llseek         = seq_lseek,
    .release        = single_release,
};
#endif

static int switch_issue_log_print_read(struct seq_file *seq, void *v)
{

    seq_printf(seq, "print log to console=%u\n 1:enable 0:disable\n",print_log);

    return 0;
}

static int switch_issue_log_print_open_proc(struct inode *inode, struct file *file)
{
    return single_open(file, switch_issue_log_print_read, inode->i_private);
}

static int switch_issue_log_print_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos)
{
    unsigned char tmpBuf[16] = {0};
    int len = (count > 15) ? 15 : count;
    
    if (buffer && !copy_from_user(tmpBuf, buffer, len))
    {
        print_log=simple_strtoul(tmpBuf, NULL, 10);
        return count;
    }
    return -EFAULT;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
static const struct proc_ops switch_issue_log_print_fop = {
    .proc_open      = switch_issue_log_print_open_proc,
    .proc_write     = switch_issue_log_print_write,
    .proc_read      = seq_read,
    .proc_lseek     = seq_lseek,
    .proc_release   = single_release,
};
#else
static const struct file_operations switch_issue_log_print_fop = {
    .owner          = THIS_MODULE,
    .open           = switch_issue_log_print_open_proc,
    .write          = switch_issue_log_print_write,
    .read           = seq_read,
    .llseek         = seq_lseek,
    .release        = single_release,
};
#endif

static int switch_issue_log_path_read(struct seq_file *seq, void *v)
{
    seq_printf(seq, "filePath=%s\n",filePath);
    return 0;
}

static int switch_issue_log_path_open_proc(struct inode *inode, struct file *file)
{
    return single_open(file, switch_issue_log_path_read, inode->i_private);
}

static int switch_issue_log_path_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos)
{
    unsigned char tmpBuf[MAX_FILE_PATH_LEN] = {0};
    int len = (count > (MAX_FILE_PATH_LEN-1)) ? (MAX_FILE_PATH_LEN-1) : count;
    
    if (buffer && !copy_from_user(tmpBuf, buffer, len))
    {
        memset(filePath,0,MAX_FILE_PATH_LEN);
        memcpy(filePath,tmpBuf,len-1);
        printk("filePath=%s\n",filePath);
        return count;
    }
    return -EFAULT;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
static const struct proc_ops switch_issue_log_path_fop = {
    .proc_open      = switch_issue_log_path_open_proc,
    .proc_write     = switch_issue_log_path_write,
    .proc_read      = seq_read,
    .proc_lseek     = seq_lseek,
    .proc_release   = single_release,
};
#else
static const struct file_operations switch_issue_log_path_fop = {
    .owner          = THIS_MODULE,
    .open           = switch_issue_log_path_open_proc,
    .write          = switch_issue_log_path_write,
    .read           = seq_read,
    .llseek         = seq_lseek,
    .release        = single_release,
};
#endif

int __init switch_issue_log_init(void)
{
    uint32 data,value;
    int32 ret = RT_ERR_OK;

    init_waitqueue_head(&wqueue);

    pSwIssueLogThread = kthread_create(SW_Issue_Log_thread, NULL, "Switch Issue Log");
    if(IS_ERR(pSwIssueLogThread))
    {
        printk("%s:%d Switch Issue init failed %ld!\n", __FILE__, __LINE__, PTR_ERR(pSwIssueLogThread));
        pSwIssueLogThread = NULL;
        return -EFAULT;
    }

    if (NULL == switch_issue_log_proc_dir) {
        switch_issue_log_proc_dir = proc_mkdir ("switch_issue_log", NULL);
    }

    if(switch_issue_log_proc_dir)
    {
        switch_issue_log_trigger_entry = proc_create("trigger", 0644, switch_issue_log_proc_dir, &switch_issue_log_trigger_fop);
        if (!switch_issue_log_trigger_entry) {
            printk("switch_issue_log_trigger_entry, create proc failed!");
        }

        switch_issue_log_file_size_entry = proc_create("file_size", 0644, switch_issue_log_proc_dir, &switch_issue_log_file_size_fop);
        if (!switch_issue_log_file_size_entry) {
            printk("switch_issue_log_file_size_entry, create proc failed!");
        }

        switch_issue_log_print_entry = proc_create("print", 0644, switch_issue_log_proc_dir, &switch_issue_log_print_fop);
        if (!switch_issue_log_print_entry) {
            printk("switch_issue_log_print_entry, create proc failed!");
        }

        switch_issue_log_path_entry = proc_create("path", 0644, switch_issue_log_proc_dir, &switch_issue_log_path_fop);
        if (!switch_issue_log_path_entry) {
            printk("switch_issue_log_path_entry, create proc failed!");
        }

    }

    wake_up_process(pSwIssueLogThread);

#if defined(CONFIG_SDK_RTL9602C)
    /*RTL9602C can't enable [25:22] CFG_EN_RXDSC_FAIL [8] CFG_EN_BUFFRST_INDSCOVER*/

    data=0;
    /*enable event*/
    value=0x1;
    REG_API_ERR_CHK(reg_field_set(RTL9602C_CRASH_INTRr,RTL9602C_CFG_EN_GPHY_FATAL_ERRORf,&value,&data),ret);

    value=0xf;
    REG_API_ERR_CHK(reg_field_set(RTL9602C_CRASH_INTRr,RTL9602C_CFG_EN_TXESD_FAILf,&value,&data),ret);
     
    value=0x1;
    REG_API_ERR_CHK(reg_field_set(RTL9602C_CRASH_INTRr,RTL9602C_CFG_EN_BUFFRST_SYSOVERf,&value,&data),ret);

    value=0x1;
    REG_API_ERR_CHK(reg_field_set(RTL9602C_CRASH_INTRr,RTL9602C_CFG_EN_BUFFRST_QOVERf,&value,&data),ret);

    value=0x1;
    REG_API_ERR_CHK(reg_field_set(RTL9602C_CRASH_INTRr,RTL9602C_CFG_EN_BUFFRST_DSCOVERf,&value,&data),ret);

    /*clear status*/
    value=0x1;
    REG_API_ERR_CHK(reg_field_set(RTL9602C_CRASH_INTRr,RTL9602C_GPHY_FATAL_ERRORf,&value,&data),ret);

    value=0xf;
    REG_API_ERR_CHK(reg_field_set(RTL9602C_CRASH_INTRr,RTL9602C_TXESD_FAILf,&value,&data),ret);

    value=0x1;
    REG_API_ERR_CHK(reg_field_set(RTL9602C_CRASH_INTRr,RTL9602C_BUFFRST_SYSOVERf,&value,&data),ret);

    value=0x1;
    REG_API_ERR_CHK(reg_field_set(RTL9602C_CRASH_INTRr,RTL9602C_BUFFRST_QOVERf,&value,&data),ret);

    value=0x1;
    REG_API_ERR_CHK(reg_field_set(RTL9602C_CRASH_INTRr,RTL9602C_BUFFRST_DSCOVERf,&value,&data),ret);


    REG_API_ERR_CHK(reg_write(RTL9602C_CRASH_INTRr, &data),ret);

    /*init imr and isr*/
    if(dal_rtl9602c_intr_imr_set(INTR_TYPE_SWITCH_ERROR,ENABLED)!=0)
    {
        printk("switch issue can't set imr!\n");
    }
    if(dal_rtl9602c_intr_isr_set(INTR_TYPE_SWITCH_ERROR,ENABLED)!=0)
    {
        printk("switch issue can't set isr!\n");
    }

#else if defined(CONFIG_SDK_RTL9607C)

    /*enable event*/
    /*RTL9607C can't enable [0] CFG_EN_BUFFRST_INDSCOVER*/
    data=0;
    value=0x1f;
    REG_API_ERR_CHK(reg_field_set(RTL9607C_CRASH_INTR_EN_0r,RTL9607C_CFG_EN_PHY_FATAL_ERRf,&value,&data),ret);

    value=0x1;
    REG_API_ERR_CHK(reg_field_set(RTL9607C_CRASH_INTR_EN_0r,RTL9607C_CFG_EN_BUFFRST_SYSOVERf,&value,&data),ret);

    value=0x1;
    REG_API_ERR_CHK(reg_field_set(RTL9607C_CRASH_INTR_EN_0r,RTL9607C_CFG_EN_BUFFRST_QOVERf,&value,&data),ret);

    value=0x1;
    REG_API_ERR_CHK(reg_field_set(RTL9607C_CRASH_INTR_EN_0r,RTL9607C_CFG_EN_BUFFRST_DSCOVERf,&value,&data),ret);

    REG_API_ERR_CHK(reg_write(RTL9607C_CRASH_INTR_EN_0r, &data),ret);

    data=0;
    value=0x7ff;
    REG_API_ERR_CHK(reg_field_set(RTL9607C_CRASH_INTR_EN_1r,RTL9607C_CFG_EN_RX_DSC_ERRf,&value,&data),ret);
    
    value=0x7ff;
    REG_API_ERR_CHK(reg_field_set(RTL9607C_CRASH_INTR_EN_1r,RTL9607C_CFG_EN_TXESD_FAILf,&value,&data),ret);

    REG_API_ERR_CHK(reg_write(RTL9607C_CRASH_INTR_EN_1r, &data),ret);

    /*clear status*/
    data=0;
    value=0x1f;
    REG_API_ERR_CHK(reg_field_set(RTL9607C_CRASH_INTR_STS_0r,RTL9607C_PHY_FATAL_ERRf,&value,&data),ret);

    value=0x1;
    REG_API_ERR_CHK(reg_field_set(RTL9607C_CRASH_INTR_STS_0r,RTL9607C_BUFFRST_SYSOVERf,&value,&data),ret);

    value=0x1;
    REG_API_ERR_CHK(reg_field_set(RTL9607C_CRASH_INTR_STS_0r,RTL9607C_BUFFRST_QOVERf,&value,&data),ret);

    value=0x1;
    REG_API_ERR_CHK(reg_field_set(RTL9607C_CRASH_INTR_STS_0r,RTL9607C_BUFFRST_DSCOVERf,&value,&data),ret);

    REG_API_ERR_CHK(reg_write(RTL9607C_CRASH_INTR_STS_0r, &data),ret);

    data=0;
    value=0x7ff;
    REG_API_ERR_CHK(reg_field_set(RTL9607C_CRASH_INTR_STS_1r,RTL9607C_RX_DSC_ERRf,&value,&data),ret);
    
    value=0x7ff;
    REG_API_ERR_CHK(reg_field_set(RTL9607C_CRASH_INTR_STS_1r,RTL9607C_TXESD_FAILf,&value,&data),ret);

    REG_API_ERR_CHK(reg_write(RTL9607C_CRASH_INTR_STS_1r, &data),ret);

    /*init imr and isr*/
    if(dal_rtl9607c_intr_imr_set(INTR_TYPE_SWITCH_ERROR,ENABLED)!=0)
    {
        printk("switch issue can't set imr!\n");
    }
    if(dal_rtl9607c_intr_isr_set(INTR_TYPE_SWITCH_ERROR,ENABLED)!=0)
    {
        printk("switch issue can't set isr!\n");
    }
#endif

    return 0;
}

void __exit switch_issue_log_exit(void)
{
    if(switch_issue_log_trigger_entry)
    {
        remove_proc_entry("trigger", switch_issue_log_proc_dir);
        switch_issue_log_trigger_entry = NULL;
    }

    if(switch_issue_log_file_size_entry)
    {
        remove_proc_entry("file_size", switch_issue_log_proc_dir);
        switch_issue_log_file_size_entry = NULL;
    }

    if(switch_issue_log_print_entry)
    {
        remove_proc_entry("print", switch_issue_log_proc_dir);
        switch_issue_log_print_entry = NULL;
    }

    if(switch_issue_log_path_entry)
    {
        remove_proc_entry("path", switch_issue_log_proc_dir);
        switch_issue_log_path_entry = NULL;
    }

    if(switch_issue_log_proc_dir)
    {
        remove_proc_entry("switch_issue_log", NULL);
        switch_issue_log_proc_dir = NULL;
    }

    if(pSwIssueLogThread!=NULL)
        kthread_stop(pSwIssueLogThread);
}

EXPORT_SYMBOL(switch_issue_log_isr);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("RealTek Switch Issue Log drive module");
MODULE_AUTHOR("Realtek");
module_init(switch_issue_log_init);
module_exit(switch_issue_log_exit);

