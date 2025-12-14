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
 * Purpose : EPON polling kernel thread
 *
 * Feature : Use kernel thread to perodically polling the LOS state
 *
 */

#ifdef CONFIG_EPON_FEATURE
/*
 * Include Files
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/delay.h>
#include <linux/version.h>

/* For RTK APIs */
#include <common/rt_error.h>
#include <rtk/epon.h>
#include <rtk/switch.h>

#include <pkt_redirect.h>
#include <net/rtl/rtl_types.h>
#include <linux/seq_file.h>
#if defined(CONFIG_SDK_APOLLOMP)
#include <dal/apollomp/dal_apollomp.h>
#endif

#if defined(CONFIG_SDK_RTL9601B)
#include <dal/rtl9601b/dal_rtl9601b.h>
#endif


/*
 * Symbol Definition
 */
#define EPON_POLLING_FLAG_STOPPOLLING       0x00000001UL

/*
 * Data Declaration
 */
__DRAM unsigned int polling_flag = 0;
struct proc_dir_entry *epon_proc_dir = NULL;
struct proc_dir_entry *polling_flag_entry = NULL;
struct task_struct *pEponPollTask;


uint32 chipId, rev, subtype;



#ifdef CONFIG_EPON_LOS_RECOVER

__DRAM unsigned int losRecover_polling_flag = 0;
struct task_struct *pEponLosRecoverPollTask;
struct proc_dir_entry *losRecover_flag_entry = NULL;
#endif

/*
 * Macro Definition
 */

/*
 * Function Declaration
 */

int epon_polling_thread(void *pData)
{
    int ret;
    unsigned char losMsg[] = {
        0x15, 0x68, /* Magic key */
        0xde, 0xad  /* Message body */
    };
#ifdef CONFIG_EPON_LOS_RECOVER
	unsigned char losRecoverMsg[] = {
		0x15, 0x68, /* Magic key */
        0x55, 0x55  /* Message body */
	};
#endif
    rtk_enable_t state, currState = ENABLED;
    rtk_epon_llid_entry_t llidEntry;
    
    if(pData){}
    
    while(!kthread_should_stop())
    {
        /* No need to wake up earlier */
        set_current_state(TASK_UNINTERRUPTIBLE);
        schedule_timeout(2 * HZ / 100);

        if(polling_flag & EPON_POLLING_FLAG_STOPPOLLING)
        {
            /* Stop polling, just wati until next round */
            continue;
        }

        ret = rtk_epon_losState_get(&state);
        do {
            if((RT_ERR_OK == ret) && (currState != state))
            {
                if(DISABLED == state)
                {
                    uint32 chipId, rev, subtype;
                    uint32  data;

                    ret = rtk_switch_version_get(&chipId, &rev, &subtype);

                    if(RT_ERR_OK != ret)
                    {
                        printk("%s:%d rtk_switch_version_get failed %d!\n", __FILE__, __LINE__,ret);
                    }

                    switch(rev)
                    {
                        case CHIP_REV_ID_A:
                        case CHIP_REV_ID_B:
                        case CHIP_REV_ID_C:
                        case CHIP_REV_ID_D:
                        case CHIP_REV_ID_E:
                        case CHIP_REV_ID_F:
                        default:
                            /*re-lock gphy*/
#if defined(CONFIG_SDK_APOLLOMP)
                            data = 0x0;
                            ret = reg_field_write(APOLLOMP_WSDS_ANA_15r, APOLLOMP_REG_CMU_EN_GPHYf, &data);
                            if(RT_ERR_OK != ret)
                            {
                                printk("%s:%d reg_field_write failed %d!\n", __FILE__, __LINE__,ret);
                            }
                            data = 0x1;
                            ret = reg_field_write(APOLLOMP_WSDS_ANA_15r, APOLLOMP_REG_CMU_EN_GPHYf, &data);

                            if(RT_ERR_OK != ret)
                            {
                                printk("%s:%d reg_field_write failed %d!\n", __FILE__, __LINE__,ret);
                            }
#endif
                            break;
                    }
#ifndef CONFIG_EPON_LOS_RECOVER
                    /* Trigger register */
                    llidEntry.llidIdx = 0;
                    ret = rtk_epon_llid_entry_get(&llidEntry);
                    if(RT_ERR_OK != ret)
                    {
                        break;
                    }

                    llidEntry.valid = DISABLED;
                    llidEntry.llid = 0x7fff;
                    ret = rtk_epon_llid_entry_set(&llidEntry);
                    if(RT_ERR_OK != ret)
                    {
                        break;
                    }
#else
					pkt_redirect_kernelApp_sendPkt(PR_USER_UID_EPONOAM, 1, sizeof(losRecoverMsg), losRecoverMsg);
#endif
                }
                else
                {
#if defined(CONFIG_SDK_APOLLOMP)                    
                    unsigned int tmpData1=0;
                    /*reset EPON*/
                    tmpData1 = 0;
                    ret = reg_field_write(APOLLOMP_WSDS_DIG_00r,APOLLOMP_CFG_SFT_RSTB_EPONf,&tmpData1);
                    if(RT_ERR_OK != ret)
                    {
                        printk("%s:%d reg_field_write failed %d!\n", __FILE__, __LINE__,ret);
                    }
                    tmpData1 = 1;
                    ret = reg_field_write(APOLLOMP_WSDS_DIG_00r,APOLLOMP_CFG_SFT_RSTB_EPONf,&tmpData1);                      
                    if(RT_ERR_OK != ret)
                    {
                        printk("%s:%d reg_field_write failed %d!\n", __FILE__, __LINE__,ret);
                    }
#endif                    
                    
#ifndef CONFIG_EPON_LOS_RECOVER
                    /* Trigger lost of link */
                    llidEntry.llidIdx = 0;
                    ret = rtk_epon_llid_entry_get(&llidEntry);
                    if(RT_ERR_OK != ret)
                    {
                        break;
                    }

                    llidEntry.valid = DISABLED;
                    llidEntry.llid = 0x7fff;
                    ret = rtk_epon_llid_entry_set(&llidEntry);
                    if(RT_ERR_OK != ret)
                    {
                        break;
                    }
#endif
                    pkt_redirect_kernelApp_sendPkt(PR_USER_UID_EPONOAM, 1, sizeof(losMsg), losMsg);
                }
                currState = state;
            }
        }while(0);
    }

    return 0;
}





#ifdef CONFIG_EPON_LOS_RECOVER


uint32 lastGateRx=0;
uint32 lastReportTx=0;

uint32 currentGateRx;
uint32 currentReportTx;


int epon_losRecover_polling_thread(void *pData)
{
    int ret;
    rtk_epon_llid_entry_t llidEntry;
    rtk_epon_counter_t mibCnt;

    if(pData){}
    
    while(!kthread_should_stop())
    {
        /* No need to wake up earlier */
        set_current_state(TASK_UNINTERRUPTIBLE);
        schedule_timeout(2 * HZ / 100);

        if(losRecover_polling_flag & EPON_POLLING_FLAG_STOPPOLLING)
        {
            /* Stop polling, just wati until next round */
            continue;
        }
         
        /*check los state */
        rtk_enable_t losState;
        ret = rtk_epon_losState_get(&losState);
        
        if (ret != RT_ERR_OK)        
            continue;
        
        if(losState == ENABLED)
        {/*only laser link up need check handover status*/
            continue;
        }
              
        /*get llid valid*/
        llidEntry.llidIdx = 0;
        ret = rtk_epon_llid_entry_get(&llidEntry); 
        if (ret != RT_ERR_OK)        
            continue;
        
        if(0==llidEntry.valid)
        {
            lastGateRx = 0;
            lastReportTx = 0;
            
            currentGateRx=0;
            currentReportTx=0;
            continue;    
        }
        
        /*get current gate report counter*/
        mibCnt.llidIdx=0;
        ret = rtk_epon_mibCounter_get(&mibCnt);
        if (ret != RT_ERR_OK)        
            continue;
        
        currentGateRx   = mibCnt.llidIdxCnt.mpcpRxGate;
        currentReportTx = mibCnt.llidIdxCnt.mpcpTxReport;

#if defined(CONFIG_SDK_APOLLOMP)   
        if(APOLLOMP_CHIP_ID==chipId)
        {
            uint32 tmpData1;
            if(lastGateRx!=0 && lastReportTx!=0)
            {
                if(lastGateRx ==currentGateRx)
                {
                    //printk("\n gate cnt not changed!! %d   %d disc:%u\n",lastGateRx,currentGateRx,mibCnt.mpcpRxDiscGate);
                    {
                        rtk_enable_t state, currState = ENABLED;
                       
                        rtk_epon_losState_get(&state);
                        //printk("\nlos status:%d \n",state);   
                        /*check symbol error cnt*/
                        if(state == DISABLED)
                        {
                            uint32 data;
                            /*check laser state*/
                            data = 0x0;
                            if ((ret = reg_field_write(APOLLOMP_SDS_EXT_REG24r,APOLLOMP_CFG_SYMBOLERR_CNTf,&data)) != RT_ERR_OK)
                            {
                                RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
                                return ret;
                            }
                            /*check symbol error cnt*/
                            if ((ret = reg_field_read(APOLLOMP_SDS_EXT_REG25r,APOLLOMP_MUX_SYMBOLERR_CNTf,&data)) != RT_ERR_OK)
                            {
                                RT_ERR(ret, (MOD_EPON | MOD_DAL), "");
                                return ret;
                            }
                            if(data > 0)
                            {
                                #if 1
                                    osal_printf("\n symbol error occurs(%d), change laser rx polarity to sync rx again\n",data);
                                #endif
                               
                                
                                /*relock gphy*/
                                data = 0x0;
                                ret = reg_field_write(APOLLOMP_WSDS_ANA_15r, APOLLOMP_REG_CMU_EN_GPHYf, &data);
                                if (ret != RT_ERR_OK)        
                                    continue;
    
                                data = 0x1;
                                ret = reg_field_write(APOLLOMP_WSDS_ANA_15r, APOLLOMP_REG_CMU_EN_GPHYf, &data);
                                if (ret != RT_ERR_OK)        
                                    continue;
     
                                /*reset epon mac*/
                                /*reset EPON*/
                                tmpData1 = 0;
                                ret = reg_field_write(APOLLOMP_WSDS_DIG_00r,APOLLOMP_CFG_SFT_RSTB_EPONf,&tmpData1);
                                if (ret != RT_ERR_OK)        
                                    continue;
    
                                tmpData1 = 1;
                                ret = reg_field_write(APOLLOMP_WSDS_DIG_00r,APOLLOMP_CFG_SFT_RSTB_EPONf,&tmpData1);
                                if (ret != RT_ERR_OK)        
                                    continue;

                            }
                        }
                    }
                }
				else
				{
					/*check if report increase*/
					if(lastReportTx == currentReportTx)
					{ /*grant list hang up reset epon mac*/
						/*reset EPON*/
						tmpData1 = 0;
						ret = reg_field_write(APOLLOMP_WSDS_DIG_00r,APOLLOMP_CFG_SFT_RSTB_EPONf,&tmpData1);
                        if (ret != RT_ERR_OK)        
                            continue;
						tmpData1 = 1;
						ret = reg_field_write(APOLLOMP_WSDS_DIG_00r,APOLLOMP_CFG_SFT_RSTB_EPONf,&tmpData1);                     
                        if (ret != RT_ERR_OK)        
                            continue;
					} 
				}
            }
            lastGateRx   = currentGateRx;
            lastReportTx = currentReportTx;            
            continue;
        }    
#endif
    }

    return 0;
}





static int losRecover_polling_flag_read(struct seq_file *seq, void *v)
{
	seq_printf(seq, "[losRecover_polling_flag = 0x%x]\n", losRecover_polling_flag);
	seq_printf(seq, "EPON_POLLING_FLAG_STOPPOLLING\t0x%x\n", EPON_POLLING_FLAG_STOPPOLLING);

	return 0;
}

static int losRecover_polling_flag_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos)
{
	unsigned char tmpBuf[16] = {0};
	int len = (count > 15) ? 15 : count;

	if (buffer && !copy_from_user(tmpBuf, buffer, len))
	{
		polling_flag = simple_strtoul(tmpBuf, NULL, 16);
		printk("write losRecover_polling_flag to 0x%08x\n", losRecover_polling_flag);
		return count;
	}
	return -EFAULT;
}

static int losRecover_polling_flag_open(struct inode *inode, struct file *file)
{
        return single_open(file, losRecover_polling_flag_read, inode->i_private);
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
static const struct proc_ops losRecover_polling_flag_fops = {
        .proc_open      = losRecover_polling_flag_open,
        .proc_read      = seq_read,
        .proc_write     = losRecover_polling_flag_write,
        .proc_lseek     = seq_lseek,
        .proc_release   = single_release,
};
#else
static const struct file_operations losRecover_polling_flag_fops = {
        .owner          = THIS_MODULE,
        .open           = losRecover_polling_flag_open,
        .read           = seq_read,
        .write          = losRecover_polling_flag_write,
        .llseek         = seq_lseek,
        .release        = single_release,
};
#endif

static void epon_losRecover_polling_dbg_init(void)
{

    if(epon_proc_dir)
    {
		proc_create("losRecover_polling_flag", 0644, epon_proc_dir, &losRecover_polling_flag_fops);
    }
}

    
#endif




static int polling_flag_read(struct seq_file *seq, void *v)
{
	seq_printf(seq, "[polling_flag = 0x%x]\n", polling_flag);
	seq_printf(seq, "EPON_POLLING_FLAG_STOPPOLLING\t0x%x\n", EPON_POLLING_FLAG_STOPPOLLING);

	return 0;
}

static int polling_flag_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos)
{
	unsigned char tmpBuf[16] = {0};
	int len = (count > 15) ? 15 : count;

	if (buffer && !copy_from_user(tmpBuf, buffer, len))
	{
		polling_flag = simple_strtoul(tmpBuf, NULL, 16);
		printk("write polling_flag to 0x%08x\n", polling_flag);
		return count;
	}
	return -EFAULT;
}

static int polling_flag_open(struct inode *inode, struct file *file)
{
        return single_open(file, polling_flag_read, inode->i_private);
}

static const struct file_operations polling_flag_fops = {
        .owner          = THIS_MODULE,
        .open           = polling_flag_open,
        .read           = seq_read,
        .write          = polling_flag_write,
        .llseek         = seq_lseek,
        .release        = single_release,
};

static void epon_polling_dbg_init(void)
{
    /* Create proc debug commands */
    if(NULL == epon_proc_dir)
    {
        epon_proc_dir = proc_mkdir("epon", NULL);
    }
    if(epon_proc_dir)
    {
		proc_create("polling_flag", 0644, epon_proc_dir, &polling_flag_fops);
    }
}

static void epon_polling_dbg_exit(void)
{
    /* Remove proc debug commands */
    if(polling_flag_entry)
    {
    	remove_proc_entry("polling_flag", epon_proc_dir);
        polling_flag_entry = NULL;
    }
    if(epon_proc_dir)
    {
        remove_proc_entry("epon", NULL);
        epon_proc_dir = NULL;
    }
}

int __init epon_poling_init(void)
{
    uint32 chipId, rev, subtype;
    int ret;
    
    
    ret = rtk_switch_version_get(&chipId, &rev, &subtype);
    
    if(RT_ERR_OK != ret)
    {
        printk("%s:%d epon_poling_init rtk_switch_version_get failed %d!\n", __FILE__, __LINE__,ret);
    }
    
    if(APOLLOMP_CHIP_ID==chipId)
    {/*only allo need polling*/

        pEponPollTask = kthread_create(epon_polling_thread, NULL, "epon_polling");
        if(IS_ERR(pEponPollTask))
        {
            printk("%s:%d epon_poling_init failed %ld!\n", __FILE__, __LINE__, PTR_ERR(pEponPollTask));
        }
        else
        {
            wake_up_process(pEponPollTask);
            printk("%s:%d epon_poling_init complete!\n", __FILE__, __LINE__);
        }
        epon_polling_dbg_init();
    }


#ifdef CONFIG_EPON_LOS_RECOVER

    #if defined(CONFIG_SDK_APOLLOMP)
    pEponLosRecoverPollTask = kthread_create(epon_losRecover_polling_thread, NULL, "epon_los_Recovery_task");
    if(IS_ERR(pEponLosRecoverPollTask))
    {
        printk("%s:%d pEponLosRecoverPollTask failed %ld!\n", __FILE__, __LINE__, PTR_ERR(pEponLosRecoverPollTask));
    }
    else
    {
        wake_up_process(pEponLosRecoverPollTask);
        printk("%s:%d pEponLosRecoverPollTask complete!\n", __FILE__, __LINE__);
    }
    epon_losRecover_polling_dbg_init();
    #endif
    
#endif



    return 0;
}

void __exit epon_polling_exit(void)
{
    kthread_stop(pEponPollTask);
    epon_polling_dbg_exit();
}

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("RealTek EPON polling module");
MODULE_AUTHOR("Realtek");
module_init(epon_poling_init);
module_exit(epon_polling_exit);
#endif

