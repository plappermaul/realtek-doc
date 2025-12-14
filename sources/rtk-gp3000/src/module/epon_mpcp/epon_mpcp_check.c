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
 * $Revision: 51194 $
 * $Date: 2014-09-10 16:36:56 +0800 (Wed, 10 Sep 2014) $
 *
 * Purpose : EPON polling kernel thread
 *
 * Feature : Use kernel thread to perodically polling the LOS state
 *
 */

#ifdef CONFIG_EPON_FEATURE



#define VER_F_MPCP_CPU_HANDLE 0

/*
 * Include Files
 */
#include <linux/module.h> 
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/init.h> 
#include <linux/proc_fs.h>
#include <linux/skbuff.h>
#include <linux/version.h>

#include <net/sock.h> 
#include <net/netlink.h> 
#include <linux/delay.h>



/* For RTK APIs */
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <common/rt_type.h>
#include <rtk/epon.h>
#include <rtk/switch.h>
#include <rtk/acl.h>
#include <rtk/irq.h>
#include <rtk/intr.h>
#include <pkt_redirect.h>
#include <net/rtl/rtl_types.h>
#ifdef CONFIG_SDK_APOLLOMP
#include <dal/apollomp/dal_apollomp_epon.h>
#include <dal/apollomp/dal_apollomp.h>
#endif
#include "module/intr_bcaster/intr_bcaster.h"




#if defined(CONFIG_RTL9600_SERIES)
#include "re8686.h"
#elif defined(CONFIG_RTL9601B_SERIES)
#include "re8686_rtl9601b.h"
#elif defined(CONFIG_RTL9602C_SERIES)
#include "re8686_rtl9602c.h"
#else
#include "re8686_nic.h"
#endif

/*
 * Symbol Definition
 */


typedef struct epon_mpcp_gate_info_s
{
    uint32  timeStamp;
    uint32  grantStart;
    uint32  grantLen;
    uint8   grantFlag;
} epon_mpcp_gate_info_t;





typedef struct epon_mpcp_gate_dbg_info_s
{
    uint32  curStartTime;
    uint32  curEndTime;
    uint32  localTime;
    uint32  lastStartTime;
    uint32  lastEndTime;
} epon_mpcp_gate_dbg_info_t;



/*
 * Data Declaration
 */
 
static unsigned int rtl960x_pon_port_mask;
uint32 chipId, rev, subtype; 
static uint32 phy_pon_port;
 
 
 
#ifdef CONFIG_SDK_APOLLOMP  
__DRAM unsigned int gate_ana_flag = 0;
__DRAM unsigned int laser_patch_flag = 1;

__DRAM uint32 mpcpDbDbgFlag = 0;





struct proc_dir_entry *epon_mpcp_proc_dir = NULL;

struct proc_dir_entry *mpcp_laser_check_entry = NULL;
struct proc_dir_entry *mpcp_gate_ana_entry = NULL;
struct proc_dir_entry *gata_ana_dbg_entry = NULL;

struct task_struct *pEponLaserChangeCheckTask;

static uint32 tatalGateCnt;


//static unsigned int isFirstError=1;
static uint32 _eponTimeDriftIntCnt=0;
//static unsigned int mpcpCauseDriftOccurs=0;
static unsigned int inValidLaserDetectErrorCnt=0;
static unsigned int mpcp_intr_cnt=0,inValidLaserCnt=0;
static unsigned int gateRxExpire=0;
//static unsigned int driftTestCnt=0;
//static unsigned int mpcpDriftCnt=0;
static unsigned int asicLaserChanedCnt=0;
static unsigned int laserNeedChaned=0;
static unsigned int laserChangeByDrift=0;

static unsigned int oltDirctBTBCnt=0;

#define LASER_CHECK_FAIL_BUFFER_CNT 10

static unsigned int laserCheckFailForceStop=0;
static int laserCheckFailMpcpBuffer=LASER_CHECK_FAIL_BUFFER_CNT;



static uint32 preMpcpGateEnd;

#define MAX_GATE_END_MPCP_DELTA 256
static uint16 gateEndMpcpDelta[MAX_GATE_END_MPCP_DELTA];
static uint16 negGateEndMpcpDelta[MAX_GATE_END_MPCP_DELTA];



static uint32 minGateEndMpcpDelta=0xFFFFFF;
static uint32 minNegGateEndMpcpDelta=0xFFFFFF;
static uint32 minGateAndGateDelta=0xFFFFFF;
static uint32 maxGateAndGateDelta=0x0;
static uint32 grantUsed[8];
static uint32 tatalBTBCnt=0;
//static uint32 driftTestSafeCnt=0;

static int laser_i=0,laser_j=0;
static unsigned int laser_cnt1,laser_cnt0;


#define EPON_MPCP_MAX_GRANT_ANA_DB  128
static epon_mpcp_gate_info_t grantDb[EPON_MPCP_MAX_GRANT_ANA_DB];
static uint16 currentGrantIdx=0;


#define EPON_MPCP_MAX_GRANT_DBG_DB  128
static epon_mpcp_gate_dbg_info_t grantDbgDb[EPON_MPCP_MAX_GRANT_DBG_DB];
static uint16 currentGrantdbgIdx=0;



/*
 * Macro Definition
 */
/*mpcp gate analysys*/ 
#define EPON_MPCP_ANA_FLAG_START       0x00000001UL
#define EPON_MPCP_ANA_FLAG_STOP        0x00000002UL  
#define EPON_MPCP_ANA_FLAG_ENABLE      0x80000000UL

#define EPON_MPCP_REPORT_CKECK_ENABLE  0x00000004UL


/*laser patch flag*/
#define EPON_LASER_PATCH_FLAG_DBGMSG   0x00000001UL
#define EPON_LASER_CHECK_CLR_CNT       0x00000002UL
#define EPON_LASER_FORCE_LASER_CHANGE  0x00000004UL
#define EPON_LASER_CHANGE_SINGLE       0x00000008UL
#define EPON_LASER_IVALID_NO_FORCE_BEN 0x00000010UL
#define EPON_LASER_FORCE_TURNOFF_BEN   0x00000020UL
#define EPON_LASER_MPCP_TRAP_EN        0x08000000UL
#define EPON_LASER_CHECK_FLAG_STOP     0x80000000UL



#define MAX_MPCP_DRIFT_TESTCNT 1000


#endif

/*
 * Function Declaration
 */



#ifdef CONFIG_SDK_APOLLOMP 
#define _asm_get_cp0_count() ({ \
    u32 __ret=0;                \
    __asm__ __volatile__ (      \
    "   mfc0    %0, $9  "       \
    : "=r"(__ret)               \
    );                          \
    __ret;                      \
})



void epon_mpcp_verF_checking( void );


void epon_apollo_laser_phase_change(void)
{
#ifdef CONFIG_SDK_APOLLOMP
    unsigned int benOn;
    unsigned int laserPhase,preLaserPhase;
    int iCheck=0,iChange=0;


    laser_i=0;
    laser_j=0;
    laser_cnt1=0;
    laser_cnt0=0;


    /*change laser phase*/
    laserPhase = (*((volatile unsigned int *)(0xBB022a34)));
    preLaserPhase = laserPhase;
    if(laserPhase & 0x00008000)
    {
        laserPhase = laserPhase&0xFFFF7FFF;
    }
    else
    {
        laserPhase = laserPhase|0x00008000;
    }

    /*polling until BEN on*/
    (*((volatile unsigned int *)(0xBB036018)))= 0x208B;
    do{
        benOn = (*((volatile unsigned int *)(0xBB03601C)));
        laser_i++;
        if(laser_i>100000)
            break;
        if((benOn & 0x00040000))
            iCheck++;
        else
        {
            if(iCheck)
                iChange=1;    
            iCheck =0;
        }
    }while(iCheck<2);

    /*polling until BEN off*/
    do{
         laser_cnt0 = _asm_get_cp0_count();
         benOn = (*((volatile unsigned int *)(0xBB03601C)));
         laser_j++;
         if(laser_j>100000)
            break;
    }while((benOn & 0x00040000));
                        
        
    /*change laser phase*/

    /*all state change one phase*/
   (*((volatile unsigned int *)(0xBB022a34))) = laserPhase;

    laserChangeByDrift = 0;
    laserNeedChaned=0;
    
    asicLaserChanedCnt++;
    
    laser_cnt1 = _asm_get_cp0_count();    
#endif
} 





int epon_laser_change_polling_thread(void *data)
{
    unsigned long eponIrqFlags;
    int ret;
    while(!kthread_should_stop())
    {
        /* No need to wake up earlier */
        set_current_state(TASK_UNINTERRUPTIBLE);
        /*schedule every 0.1 sec*/    
        schedule_timeout(HZ/10);

        /*check if need laser phase change*/
        if(laser_patch_flag & EPON_LASER_CHECK_FLAG_STOP)
        {
            /* Stop polling, just wait until next round */
            continue;
        }
        
        /*check laset change flag*/
        if(1==laserNeedChaned)
        {

            /*check if no llid entry valid*/
            /*receive deregister packet*/    
            rtk_epon_llid_entry_t llidEntry;

            /*disable LLID*/            
            llidEntry.llidIdx = 0;
            ret = rtk_epon_llid_entry_get(&llidEntry);
            
            if(RT_ERR_OK == ret)
            {
                if(llidEntry.valid == DISABLED)
                {
                   laserNeedChaned = 0;
                }
                else
                {
                    local_irq_save(eponIrqFlags);
                    if(APOLLOMP_CHIP_ID==chipId)
                    {
                        epon_apollo_laser_phase_change();
                    }
                    
                    local_irq_restore(eponIrqFlags);
                    if(laser_patch_flag&EPON_LASER_PATCH_FLAG_DBGMSG)
                    {
                        printk("\n laser change polling:(%d)/(%d) change ticks:%d\n",laser_i,laser_j,(laser_cnt1-laser_cnt0));    
                    }
                }            
            }
        }
    }

    return 0;
}


static int mpcp_gateAnaFlag_read(struct seq_file *seq, void *v)
{
	int i;

	printk("[gate_ana_flag = 0x%08x]\n", gate_ana_flag);
	printk("EPON_MPCP_ANA_FLAG_START  \t0x%08x\n", (unsigned int)EPON_MPCP_ANA_FLAG_START);
	printk("EPON_MPCP_ANA_FLAG_STOP \t0x%08x\n", (unsigned int)EPON_MPCP_ANA_FLAG_STOP);
	printk("EPON_MPCP_ANA_FLAG_ENABLE \t0x%08x\n", (unsigned int)EPON_MPCP_ANA_FLAG_ENABLE);

  	printk("[+++]\n");
    for(i=0;i<MAX_GATE_END_MPCP_DELTA;i++)
    {
    	printk("[%d]:%d \n",i,gateEndMpcpDelta[i]);
        gateEndMpcpDelta[i] = 0;
    }

  	printk("\n\n");
  	printk("[---]\n");
    for(i=0;i<MAX_GATE_END_MPCP_DELTA;i++)
    {
    	printk("[%d]:%d\n",i,negGateEndMpcpDelta[i]);
        negGateEndMpcpDelta[i] = 0;
    }
	printk("\n total[%d] min[%d],[-%d]\n",tatalGateCnt,minGateEndMpcpDelta,minNegGateEndMpcpDelta);

	printk("Grant used\n");
    for(i=0;i<4;i++)
    {
    	printk("%d, ",grantUsed[i]);
        grantUsed[i] = 0;
    }

	printk("\nGrant delta");
  	printk("BTB cnt:%d,gate delta min[0x%x] max[0x%x]\n",
  	                        tatalBTBCnt,minGateAndGateDelta,maxGateAndGateDelta);
    tatalBTBCnt =0;
    minGateAndGateDelta=0xFFFFFFFF;
    maxGateAndGateDelta=0x0;


    tatalGateCnt = 0;
    minGateEndMpcpDelta = 0xFFFFFF;
    minNegGateEndMpcpDelta= 0xFFFFFF;


	return 0;
}

static int mpcp_gateAnaFlag_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos)
{
	unsigned char tmpBuf[16] = {0};
	int len = (count > 15) ? 15 : count;

	if (buffer && !copy_from_user(tmpBuf, buffer, len))
	{
		gate_ana_flag = simple_strtoul(tmpBuf, NULL, 16);
		printk("write gate_ana_flag to 0x%08x\n", gate_ana_flag);
		
		
		if(gate_ana_flag&EPON_MPCP_ANA_FLAG_START)
		{
		    /*start cpapture gate message*/
		    currentGrantIdx=0;        
		}


		return count;
	}
	return -EFAULT;
}





static int mpcp_mpcpDbDbgFlag_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos)
{
	unsigned char tmpBuf[16] = {0};
	int len = (count > 15) ? 15 : count;

	if (buffer && !copy_from_user(tmpBuf, buffer, len))
	{
		mpcpDbDbgFlag = simple_strtoul(tmpBuf, NULL, 16);
		printk("write mpcpDbDbgFlag to 0x%08x\n", mpcpDbDbgFlag);

		return count;
	}
	return -EFAULT;
}


static int mpcp_mpcpDbDbgFlag_read(struct seq_file *seq, void *v)
{
    int i,j;
	uint32 recordData;
	mpcpDbDbgFlag = mpcpDbDbgFlag | EPON_MPCP_ANA_FLAG_STOP;
	
	printk("\n\n index,curSTime,curETime,LCTime  ,lstSTime,lstETime\n");


    for(j=0;j<EPON_MPCP_MAX_GRANT_DBG_DB;j++)
    {
    	i = currentGrantdbgIdx+j;
    	
    	i= i%EPON_MPCP_MAX_GRANT_DBG_DB;
 
    	printk(" %-5d,%-8.8x,%-8.8x,%-8.8x,%-8.8x,%-8.8x\n"
    	      ,i
    	      ,grantDbgDb[i].curStartTime
    	      ,grantDbgDb[i].curEndTime
    	      ,grantDbgDb[i].localTime
    	      ,grantDbgDb[i].lastStartTime
    	      ,grantDbgDb[i].lastEndTime);
    }
    mpcpDbDbgFlag=mpcpDbDbgFlag &(uint32)(~EPON_MPCP_ANA_FLAG_STOP);

    recordData = (*((volatile unsigned int *)(0xBB036230)));
    printk("\ncurrent local time:0x%x\n",recordData);

	return 0;
}


static int mpcp_laserChk_read(struct seq_file *seq, void *v)
{

	seq_printf(seq, "EPON_LASER_PATCH_FLAG_DBGMSG \t0x%08x\n", (unsigned int)EPON_LASER_PATCH_FLAG_DBGMSG);
	seq_printf(seq, "EPON_LASER_CHECK_CLR_CNT     \t0x%08x\n", (unsigned int)EPON_LASER_CHECK_CLR_CNT);
	seq_printf(seq, "EPON_LASER_FORCE_LASER_CHANGE\t0x%08x\n", (unsigned int)EPON_LASER_FORCE_LASER_CHANGE);
	seq_printf(seq, "EPON_LASER_IVALID_NO_FORCE_BEN\t0x%08x\n", (unsigned int)EPON_LASER_IVALID_NO_FORCE_BEN);
	seq_printf(seq, "EPON_LASER_FORCE_TURNOFF_BEN\t0x%08x\n", (unsigned int)EPON_LASER_FORCE_TURNOFF_BEN);
	seq_printf(seq, "EPON_LASER_MPCP_TRAP_EN      \t0x%08x\n", (unsigned int)EPON_LASER_MPCP_TRAP_EN);
	seq_printf(seq, "EPON_LASER_CHECK_FLAG_STOP   \t0x%08x\n\n", (unsigned int)EPON_LASER_CHECK_FLAG_STOP);
	seq_printf(seq, "\ninValidLaserDetectErrorCnt:\t0x%08x\n", (unsigned int)inValidLaserDetectErrorCnt);
	seq_printf(seq, "inValidLaserCnt:           \t0x%08x\n", (unsigned int)inValidLaserCnt);
	seq_printf(seq, "asicLaserChanedCnt:        \t0x%08x\n", (unsigned int)asicLaserChanedCnt);
	seq_printf(seq, "mpcp_intr_cnt:             \t0x%08x\n", (unsigned int)mpcp_intr_cnt);
	seq_printf(seq, "TimeDriftIntCnt:           \t0x%08x\n", (unsigned int)_eponTimeDriftIntCnt);
	seq_printf(seq, "oltDirctBTBCnt:           \t0x%08x\n", (unsigned int)oltDirctBTBCnt);
	
	

	return 0;
}



static void epon_mpcp_acl_set(uint32 mode,uint8 state)
{
#ifndef CONFIG_RTK_L34_ENABLE
    rtk_acl_ingress_entry_t aclRule;
    rtk_acl_field_t fieldHead1,fieldHead2;
    int ret;
    if(EPON_LASER_MPCP_TRAP_EN==mode)
    {
        /*add acl rule drop all gate message and trigger interrupt*/
        osal_memset(&aclRule, 0, sizeof(rtk_acl_ingress_entry_t));
        aclRule.index = 63;
        aclRule.templateIdx = 3;
        /*enable ACL on PON port*/
        RTK_PORTMASK_PORT_SET(aclRule.activePorts,phy_pon_port);
        aclRule.valid = ENABLED;
        aclRule.act.enableAct[ACL_IGR_INTR_ACT] = ENABLED;
        aclRule.act.enableAct[ACL_IGR_FORWARD_ACT] = ENABLED;
        aclRule.act.aclInterrupt = ENABLED;
        
    
        if(1==state)
        {
            aclRule.act.forwardAct.act = ACL_IGR_FORWARD_TRAP_ACT;
        }
        else
        {
            aclRule.act.forwardAct.act = ACL_IGR_FORWARD_DROP_ACT;
        }
        
        fieldHead1.fieldType = ACL_FIELD_ETHERTYPE;
        fieldHead1.fieldUnion.data.value = 0x8808;
        fieldHead1.fieldUnion.data.mask = 0xFFFF;
        fieldHead1.next = NULL;
        if ((ret = rtk_acl_igrRuleField_add(&aclRule,&fieldHead1)) != RT_ERR_OK)
        {
            return;
        }
        
        fieldHead2.fieldType = ACL_FIELD_USER_DEFINED03;
        fieldHead2.fieldUnion.data.value = 0x0002;
        fieldHead2.fieldUnion.data.mask = 0xFFFF;                      
        fieldHead2.next = NULL;
        if ((ret = rtk_acl_igrRuleField_add(&aclRule,&fieldHead2)) != RT_ERR_OK)
        {
            return;
        }
    
    
        if ((ret = rtk_acl_igrRuleEntry_add(&aclRule)) != RT_ERR_OK)
        {
            return;
        }

    }


#else

    rtk_acl_ingress_entry_t aclRule;
    rtk_acl_field_t aclField;
    int ret;
    if(EPON_LASER_MPCP_TRAP_EN==mode)
    {    

        osal_memset(&aclRule, 0, sizeof(rtk_acl_ingress_entry_t));
        osal_memset(&aclField, 0, sizeof(rtk_acl_field_t));

        aclField.fieldType = ACL_FIELD_PATTERN_MATCH;
        aclField.fieldUnion.pattern.fieldIdx = 3; //FS[3] in template[2]:field[3]
        aclField.fieldUnion.data.value = 0x0002; //two byte 0x0002 after ethertype for EPON
        aclField.fieldUnion.data.mask = 0xffff;
        if ((ret = rtk_acl_igrRuleField_add(&aclRule, &aclField)) != RT_ERR_OK)
        {
            return ret;
        }

        aclRule.valid = ENABLED;
        aclRule.index = 0;
        aclRule.templateIdx = 2;//FS[3] in template[2]:field[3]
        RTK_PORTMASK_PORT_SET(aclRule.activePorts,phy_pon_port);
        aclRule.act.enableAct[ACL_IGR_FORWARD_ACT] = ENABLED;
        aclRule.act.forwardAct.act= ACL_IGR_FORWARD_REDIRECT_ACT;

        if(1==state)
        {
            aclRule.act.forwardAct.act = ACL_IGR_FORWARD_TRAP_ACT;
        }
        else
        {
            aclRule.act.forwardAct.act = ACL_IGR_FORWARD_DROP_ACT;
        }

        aclRule.act.enableAct[ACL_IGR_INTR_ACT] = ENABLED;
        aclRule.act.aclInterrupt = ENABLED;
        if ((ret = rtk_acl_igrRuleEntry_add(&aclRule)) != RT_ERR_OK)
            return;
        
    }
#endif


}



static int mpcp_laserChk_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos)
{
	unsigned char tmpBuf[16] = {0};
	int len = (count > 15) ? 15 : count;

	if (buffer && !copy_from_user(tmpBuf, buffer, len))
	{
		laser_patch_flag = simple_strtoul(tmpBuf, NULL, 16);
		printk("write gate_ana_flag to 0x%08x\n", laser_patch_flag);

    	if(EPON_LASER_CHECK_CLR_CNT&laser_patch_flag)
    	{
            printk("\n counter clear!\n");
            inValidLaserDetectErrorCnt = 0;
            inValidLaserCnt = 0;
            mpcp_intr_cnt = 0 ;
            _eponTimeDriftIntCnt = 0;
            asicLaserChanedCnt=0;
            oltDirctBTBCnt =0;
    	}
    	if(EPON_LASER_FORCE_LASER_CHANGE&laser_patch_flag)
    	{
            printk("\n force laser change!\n");
    	    laserNeedChaned = 1;    
    	}

    	if(EPON_LASER_IVALID_NO_FORCE_BEN&laser_patch_flag)
    	{
            printk("\n ivalid BEN force laser off disable!\n");
    	}



    	if(EPON_LASER_MPCP_TRAP_EN&laser_patch_flag)
    	{
            epon_mpcp_acl_set(EPON_LASER_MPCP_TRAP_EN,1);
            printk("\n enable Trap!\n");
    	}
        else
        {
            epon_mpcp_acl_set(EPON_LASER_MPCP_TRAP_EN,0);
            printk("\n disable Trap!\n");
    
        }        


		return count;

	}
	
	return -EFAULT;
}

static int mpcp_gate_ana_read(struct seq_file *seq, void *v)
{
    int i,j;
    uint32 totalTq=0;
    uint32 totalTime;
    uint32 intPart,floatPart;
	
	gate_ana_flag = gate_ana_flag|EPON_MPCP_ANA_FLAG_STOP;
	
	seq_printf(seq, "\n\n index,TimeStmp,GntStart,GrantEnd,GrantLen,  Flag\n");

    for(j=0;j<EPON_MPCP_MAX_GRANT_ANA_DB;j++)
    {
    	i = currentGrantIdx+j;
    	
    	i= i%EPON_MPCP_MAX_GRANT_ANA_DB;
    	
    	seq_printf(seq, " %-5d,%-8.8x,%-8.8x,%-8.8x,%-8d,0x%4x\n"
    	      ,i
    	      ,grantDb[i].timeStamp
    	      ,grantDb[i].grantStart
    	      ,(grantDb[i].grantStart+grantDb[i].grantLen)
    	      ,grantDb[i].grantLen
    	      ,grantDb[i].grantFlag);
    	      
    	if(grantDb[i].grantFlag != 0x09)/*skip mpcp discovery gate*/
    	    totalTq=grantDb[i].grantLen+totalTq;      
    }
    
    totalTime =  (grantDb[EPON_MPCP_MAX_GRANT_ANA_DB-1].grantStart+grantDb[EPON_MPCP_MAX_GRANT_ANA_DB-1].grantLen) - grantDb[0].grantStart;
    if(totalTime)
    {
        intPart = (totalTq*100)/totalTime;
        floatPart = (((totalTq*100) - (intPart*totalTime)) * 1000)/totalTime;
    	seq_printf(seq, "\n\n bandWidth: %d.%d\n", intPart, floatPart);
    }
    
	gate_ana_flag = gate_ana_flag&(uint32)(~EPON_MPCP_ANA_FLAG_STOP);

    laserCheckFailForceStop=0;

	return 0;
}

static int mpcp_gate_ana_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos)
{
	unsigned char tmpBuf[16] = {0};
	int len = (count > 15) ? 15 : count;

	if (buffer && !copy_from_user(tmpBuf, buffer, len))
	{
		simple_strtoul(tmpBuf, NULL, 16);
		
		return count;
	}
	return -EFAULT;
}

static int mpcp_laser_check_open(struct inode *inode, struct file *file)
{
        return single_open(file, mpcp_laserChk_read, inode->i_private);
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
static const struct proc_ops mpcp_laser_check_fops = {
        .proc_open      = mpcp_laser_check_open,
        .proc_read      = seq_read,
        .proc_write     = mpcp_laserChk_write,
        .proc_lseek    = seq_lseek,
        .proc_release   = single_release,
};
#else
static const struct file_operations mpcp_laser_check_fops = {
        .owner          = THIS_MODULE,
        .open           = mpcp_laser_check_open,
        .read           = seq_read,
        .write          = mpcp_laserChk_write,
        .llseek         = seq_lseek,
        .release        = single_release,
};
#endif

static int mpcp_gate_ana_open(struct inode *inode, struct file *file)
{
        return single_open(file, mpcp_gate_ana_read, inode->i_private);
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
static const struct proc_ops mpcp_gate_ana_fops = {
        .proc_open      = mpcp_gate_ana_open,
        .proc_read      = seq_read,
        .proc_write     = mpcp_gate_ana_write,
        .proc_lseek     = seq_lseek,
        .proc_release   = single_release,
};
#else
static const struct file_operations mpcp_gate_ana_fops = {
        .owner          = THIS_MODULE,
        .open           = mpcp_gate_ana_open,
        .read           = seq_read,
        .write          = mpcp_gate_ana_write,
        .llseek         = seq_lseek,
        .release        = single_release,
};
#endif

static int gata_ana_dbg_open(struct inode *inode, struct file *file)
{
        return single_open(file, mpcp_gateAnaFlag_read, inode->i_private);
}


static int mpcp_dbg_db_open(struct inode *inode, struct file *file)
{
        return single_open(file, mpcp_mpcpDbDbgFlag_read, inode->i_private);
}



#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
static const struct proc_ops gata_ana_dbg_fops = {
        .proc_open      = gata_ana_dbg_open,
        .proc_read      = seq_read,
        .proc_write     = mpcp_gateAnaFlag_write,
        .proc_lseek     = seq_lseek,
        .proc_release   = single_release,
};
#else
static const struct file_operations gata_ana_dbg_fops = {
        .owner          = THIS_MODULE,
        .open           = gata_ana_dbg_open,
        .read           = seq_read,
        .write          = mpcp_gateAnaFlag_write,
        .llseek         = seq_lseek,
        .release        = single_release,
};
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
static const struct proc_ops mpcp_dbg_db_fops = {
        .proc_open      = mpcp_dbg_db_open,
        .proc_read      = seq_read,
        .proc_write     = mpcp_mpcpDbDbgFlag_write,
        .proc_lseek     = seq_lseek,
        .proc_release   = single_release,
};
#else
static const struct file_operations mpcp_dbg_db_fops = {
        .owner          = THIS_MODULE,
        .open           = mpcp_dbg_db_open,
        .read           = seq_read,
        .write          = mpcp_mpcpDbDbgFlag_write,
        .llseek         = seq_lseek,
        .release        = single_release,
};
#endif




static void epon_mpcp_packet_dbg_init(void)
{
    /* Create proc debug commands */
    if(NULL == epon_mpcp_proc_dir)
    {
        epon_mpcp_proc_dir = proc_mkdir("epon_mpcp", NULL);
    }
    if(epon_mpcp_proc_dir)
    {
    	/*mpcp debug cnt*/
		proc_create("laser_chk", 0644, epon_mpcp_proc_dir, &mpcp_laser_check_fops);

    	/*mpcp gate analysis*/
		proc_create("gate_ana", 0644, epon_mpcp_proc_dir, &mpcp_gate_ana_fops);

    	/*debug flag*/
		proc_create("gate_ana_dbg_flag", 0644, epon_mpcp_proc_dir, &gata_ana_dbg_fops);


    	/*mpcp inter dbg db*/
		proc_create("mpcp_dbg_db", 0644, epon_mpcp_proc_dir, &mpcp_dbg_db_fops);

    }
}

static void epon_mpcp_dbg_exit(void)
{
    /* Remove proc debug commands */
    if(mpcp_laser_check_entry)
    {
    	remove_proc_entry("laser_chk", epon_mpcp_proc_dir);
        mpcp_laser_check_entry = NULL;
    }

    if(mpcp_gate_ana_entry)
    {
    	remove_proc_entry("gate_ana_dump", epon_mpcp_proc_dir);
        mpcp_gate_ana_entry = NULL;
    }


    if(gata_ana_dbg_entry)
    {
    	remove_proc_entry("gate_ana_dbg_flag", epon_mpcp_proc_dir);
        gata_ana_dbg_entry = NULL;
    }
    
    
    if(epon_mpcp_proc_dir)
    {
        remove_proc_entry("epon_mpcp", NULL);
        epon_mpcp_proc_dir = NULL;
    }
}
#endif


static int epon_register_pkt_accept_check(struct sk_buff *skb)
{
    int ret;
    
    rtk_epon_llid_entry_t regEntry;    
    
    printk("\n receive register packet mac [%2.2x %2.2x %2.2x %2.2x %2.2x %2.2x] flag:%d\n",
                    skb->data[0],
                    skb->data[1],
                    skb->data[2],
                    skb->data[3],
                    skb->data[4],
                    skb->data[5],
                    skb->data[22]);
    regEntry.llidIdx = 0;
    ret = rtk_epon_llid_entry_get(&regEntry);
    if(RT_ERR_OK != ret)
        return 0;    
    
    printk("\n -- local mac addr [%2.2x %2.2x %2.2x %2.2x %2.2x %2.2x]\n",
            regEntry.mac.octet[0],
            regEntry.mac.octet[1],
            regEntry.mac.octet[2],
            regEntry.mac.octet[3],
            regEntry.mac.octet[4],
            regEntry.mac.octet[5]);
    /*mac control packet*/
    if(skb->data[0]==0x01 && 
       skb->data[1]==0x80 &&
       skb->data[2]==0xC2 &&
       skb->data[3]==0x00 &&
       skb->data[4]==0x00 &&
       skb->data[5]==0x01)    
    {
        return 1;
    }
    
    /*local mac address*/
    if(skb->data[0]==regEntry.mac.octet[0] && 
       skb->data[1]==regEntry.mac.octet[1] &&
       skb->data[2]==regEntry.mac.octet[2] &&
       skb->data[3]==regEntry.mac.octet[3] &&
       skb->data[4]==regEntry.mac.octet[4] &&
       skb->data[5]==regEntry.mac.octet[5])    
    {
        return 1;
    }
    return 0;
}

/* For ctc spec, we need to set onu to silent mode when receive a NACK mpcp REGISTER frame */
static void epon_ctc_set_oam_silent_mode(void)
{
	unsigned char silentMsg[] = {
        0x15, 0x68, /* Magic key */
        0x66, 0x66  /* Message body */
    };

	pkt_redirect_kernelApp_sendPkt(PR_USER_UID_EPONOAM, 1, sizeof(silentMsg), silentMsg);
}

int epon_mpcp_pkt_rx(
    struct re_private *cp,
    struct sk_buff *skb,
    struct rx_info *pRxInfo)
{
    int ret;
        
#ifdef CONFIG_SDK_APOLLOMP    
    if(pRxInfo->opts3.bit.reason == 211)
    {
        if(skb->data[12] != 0x88)
        {
            printk("\nmac rx out of order occurs!!\n");        
        }
        if((skb->data[13] != 0x08) && (skb->data[13] != 0x09))
        {
            printk("\nmac rx out of order occurs!!\n");        
        }
        
    }
#endif    
    
    if((skb->data[12] == 0x88) && (skb->data[13] == 0x08))
    {
        /*ony mac control packet(MPCP) need handle by this API*/ 
    }
    else
    {
         return RE8670_RX_CONTINUE;    
    }

    /*filter mpcp  register packet*/
    if((skb->data[14] == 0x00) &&  (skb->data[15] == 0x05))
    {
        if(epon_register_pkt_accept_check(skb)==0)
        {
            printk("\n this register is not for this onu!!\n");
        }
        else
        {
            if(skb->data[22] == 2)/*flag == deregister */
            {
                /*receive deregister packet*/    
                rtk_epon_llid_entry_t llidEntry;
    
                /*disable LLID*/            
                llidEntry.llidIdx = 0;
                ret = rtk_epon_llid_entry_get(&llidEntry);
                if(RT_ERR_OK!=ret)
                {
                    printk("\nget llid entry fail %s %d",__FUNCTION__,__LINE__);    
                }                                        
                
                llidEntry.valid = DISABLED;
                llidEntry.llid = 0x7fff;
                ret = rtk_epon_llid_entry_set(&llidEntry);
                if(RT_ERR_OK!=ret)
                {
                    printk("\nset llid entry fail %s %d",__FUNCTION__,__LINE__);    
                }                                        
            	queue_broadcast(MSG_TYPE_EPON_STATE, RTK_EPON_STATE_MPCP_INIT, 0, ENABLED);
			}
			else if(skb->data[22] == 4)/* flag == NACK */
			{
				epon_ctc_set_oam_silent_mode();
			}
        }
        return RE8670_RX_STOP;    
    }
    
    /*filter mpcp gate message*/
    if((skb->data[14] == 0x00) &&  (skb->data[15] == 0x02))
    {
#ifdef CONFIG_SDK_APOLLOMP 
        uint32 mpcpGateEnd;
        uint16 gateLen;
        uint32 mpcpGateStar,mpcpTimeStamp,grantStart;
        uint32 negCurrGateEndMpcpDelta=0xFFFFFFFF,currGateEndMpcpDelta=0xFFFFFFFF; 

        memcpy(&mpcpTimeStamp,&(skb->data[16]),4); 
        memcpy(&mpcpGateStar,&(skb->data[21]),4); 
        memcpy(&gateLen,&(skb->data[25]),2); 

        if(!(gate_ana_flag&EPON_MPCP_ANA_FLAG_STOP))
        {
            if(laserCheckFailForceStop==0 || laserCheckFailMpcpBuffer>0)
            {
                if(currentGrantIdx >= EPON_MPCP_MAX_GRANT_ANA_DB)
                    currentGrantIdx=0;
                grantDb[currentGrantIdx].timeStamp = mpcpTimeStamp;
                grantDb[currentGrantIdx].grantStart = mpcpGateStar;
                grantDb[currentGrantIdx].grantLen = gateLen;
                grantDb[currentGrantIdx].grantFlag = skb->data[20];
                currentGrantIdx++;
                if(laserCheckFailForceStop==1)
                    laserCheckFailMpcpBuffer--;
            }
        }

        tatalGateCnt++;     

        if(skb->data[20] == 0x09)
        {/*do not handle discovery gate*/
            return RE8670_RX_STOP;
        }

        if(preMpcpGateEnd >= mpcpTimeStamp)
        {
            currGateEndMpcpDelta = preMpcpGateEnd-mpcpTimeStamp;    
            if(currGateEndMpcpDelta < MAX_GATE_END_MPCP_DELTA)
            {
                if(gateEndMpcpDelta[currGateEndMpcpDelta]<0xFFFF)
                    gateEndMpcpDelta[currGateEndMpcpDelta]++;
            }
            if(currGateEndMpcpDelta < minGateEndMpcpDelta)
            {
                minGateEndMpcpDelta = currGateEndMpcpDelta;    
            }
        }
        else
        {
            negCurrGateEndMpcpDelta = mpcpTimeStamp-preMpcpGateEnd;    
            if(negCurrGateEndMpcpDelta < MAX_GATE_END_MPCP_DELTA)
            {
                if(negGateEndMpcpDelta[negCurrGateEndMpcpDelta]<0xFFFF)
                    negGateEndMpcpDelta[negCurrGateEndMpcpDelta]++;
            }
            if(negCurrGateEndMpcpDelta < minNegGateEndMpcpDelta)
            {
                minNegGateEndMpcpDelta = negCurrGateEndMpcpDelta;    
            }
        }
        
        #if 0
        if((currGateEndMpcpDelta)<=2)
            printk("\n[gate will drift]:%d\n",(currGateEndMpcpDelta));   
        #endif    
        /*check gate behavior*/    
        if((preMpcpGateEnd>0xf0000000) && (mpcpGateStar<0xf0000000))
        {/*overflow*/
            //printk("\noverflow: %8.8x %8.8x   delta:%8.8x\n",preMpcpGateEnd,mpcpGateStar,(mpcpGateStar - preMpcpGateEnd));
            if(((0xFFFFFFFF-preMpcpGateEnd)+mpcpGateStar) < minGateAndGateDelta)
            {
                minGateAndGateDelta = ((0xFFFFFFFF-preMpcpGateEnd)+mpcpGateStar);    
            }
            if(((0xFFFFFFFF-preMpcpGateEnd)+mpcpGateStar) > maxGateAndGateDelta)
            {
                maxGateAndGateDelta = ((0xFFFFFFFF-preMpcpGateEnd)+mpcpGateStar);    
            }
        }
        else
        {/*not overflow*/
            if(mpcpGateStar < preMpcpGateEnd)
            {/*BTB occurs*/
                tatalBTBCnt++;
                printk("\nBTB: (PRE)%8.8x (Current)%8.8x  len:%d delta:%8.8x\n",preMpcpGateEnd,mpcpGateStar,gateLen,(preMpcpGateEnd - mpcpGateStar));
            }
            if((mpcpGateStar-preMpcpGateEnd) < minGateAndGateDelta)
            {
                minGateAndGateDelta = (mpcpGateStar-preMpcpGateEnd);    
            }
            if((mpcpGateStar-preMpcpGateEnd) > maxGateAndGateDelta)
            {
                maxGateAndGateDelta = (mpcpGateStar-preMpcpGateEnd);    
            }
        }
        
        mpcpGateEnd = mpcpGateStar + gateLen;
        preMpcpGateEnd = mpcpGateEnd;
        


        if(APOLLOMP_CHIP_ID==chipId)
        {
            grantStart = (*((volatile unsigned int *)(0xBB036064)));
            if(grantStart!=0)
                grantUsed[0]++;
            
            grantStart = (*((volatile unsigned int *)(0xBB036068)));
            if(grantStart!=0)
                grantUsed[1]++;
    
            grantStart = (*((volatile unsigned int *)(0xBB03606C)));
            if(grantStart!=0)
                grantUsed[2]++;
    
            grantStart = (*((volatile unsigned int *)(0xBB036070)));
            if(grantStart!=0)
                grantUsed[3]++;
        }
#endif        
        return RE8670_RX_STOP;
        
    }    
    return RE8670_RX_CONTINUE;
}


#ifdef CONFIG_SDK_APOLLOMP
/*polling till local time greater than given time value*/
int epon_mpcp_waitLocalTimeExpire(unsigned int checkTime)
{
    unsigned int localtime=0,preLocaltime=0;
    int hitCnt=0,noChangeCnt=0,i;

    while(1){
        
        for(i=0;i<1000;i++)
        {
            ;
        }
        
    
        reg_field_read(APOLLOMP_EPON_LOCAL_TIMEr,APOLLOMP_LOCAL_TIMEf,&localtime);
        
        
        if((localtime - checkTime)< 0x0EE6B280)
        {
            hitCnt++;        
        }
        else
        {
            hitCnt =0;    
        }
        
        if(hitCnt >=5)
            break;

        if(localtime == preLocaltime)
        {
            noChangeCnt ++;
        }
        else
        {
            noChangeCnt = 0;    
        }
        preLocaltime = localtime;
        
        if(noChangeCnt > 100)
        {
            printk("\n warrning epon_mpcp_isGateBTB pollig fail maybe fiber pull off\n");    
            break;
        }
      
    }
    
    return 1;


}




int debugCntDump=0;

static unsigned int mpcpPreEndTime=0;
/*
    check if  [curent gate] and [next gate] is direct BTB    
*/
int epon_mpcp_isGateBTB(void)
{
    

    unsigned int tmpData1,tmpData2,data;
    unsigned int curGateStart=0,mpcpEndTime;

    unsigned int needPrint=0;
    int ret;
    
   
    if(debugCntDump>100000)
    {
        debugCntDump = 0;  
        needPrint =1;  
    }

    debugCntDump ++;
 
    
    /*get current mpcp gate start time*/
    /*
        Write  EPON_DEBUG1.DBG_SEL[9:0]      =0x1
        Read   EPON_DEBUG2.PRB_EPMC[23:0] = gate start time[23:0]
        Write  EPON_DEBUG1.DBG_SEL[9:0]      =0x2
        Read  EPON_DEBUG2.PRB_EPMC[7:0]   = gate start time[31:24]
    */
    /*Read   EPON_DEBUG2.PRB_EPMC[23:0]  (nextGateStart[23:0])*/     
    (*((volatile unsigned int *)(0xBB036018)))= 0x2001;
    tmpData1=(*((volatile unsigned int *)(0xBB03601C)));
    curGateStart = tmpData1 & 0x00FFFFFF;
    
    /*Read   EPON_DEBUG2.PRB_EPMC[7:0]  (nextGateStart[31:24])*/
    (*((volatile unsigned int *)(0xBB036018)))= 0x2002;
    tmpData2=(*((volatile unsigned int *)(0xBB03601C)));
    curGateStart = curGateStart+((tmpData2&0x000000FF)<<24);
      

    /*get current mpcpEndTime*/
    /*
    Write  EPON_DEBUG1.DBG_SEL[9:0]      =0x2
    Read   EPON_DEBUG2.PRB_EPMC[23:8] = gate end time[15:0]
    Write  EPON_DEBUG1.DBG_SEL[9:0]      =0x3
    Read   EPON_DEBUG2.PRB_EPMC[15:0] = gate end time[31:16]

    */
    /*Read   EPON_DEBUG2.PRB_EPMC[23:8]  (mpcpEndTime[15:0])*/     
    (*((volatile unsigned int *)(0xBB036018)))= 0x2002;
    tmpData1=(*((volatile unsigned int *)(0xBB03601C)));
    tmpData1 = tmpData1 & 0x00FFFF00;
    mpcpEndTime = tmpData1 >> 8;
    /*Read   EPON_DEBUG2.PRB_EPMC[15:0]  (mpcpEndTime[31:16])*/
    (*((volatile unsigned int *)(0xBB036018)))= 0x2003;
    tmpData2=(*((volatile unsigned int *)(0xBB03601C)));
    mpcpEndTime = mpcpEndTime+((tmpData2&0x0000FFFF)<<16);


#if 0      
    if(needPrint)
    {
        printk("\nmpcpPreEndTime:0x%x curGateStart:0x%x  mpcpEndTime:0x%x  \n",mpcpPreEndTime,curGateStart,mpcpEndTime);    
        
    }  
#endif      
      
    /*check if (curGateStart, mpcpPreEndTime not in the same gate)   and  (curGateStart - mpcpPreEndTime) < 9TQ*/
    if(((curGateStart - mpcpPreEndTime)< 0x0EE6B280) && ((curGateStart - mpcpPreEndTime) < 9))
    {
        /*hit BTB condition*/
       
        /*1. change laser on/off shifht time
            on shift = 10
            off shift = 10
        */
        data = 10;
        ret = reg_field_write(APOLLOMP_EPON_ASIC_TIMING_ADJUST2r,APOLLOMP_LSR_ON_SHIFTf,&data);
        if(RT_ERR_OK!=ret)
            return 0;      
        data = 10;
        ret = reg_field_write(APOLLOMP_EPON_ASIC_TIMING_ADJUST2r,APOLLOMP_LSR_OFF_SHIFTf,&data);
        if(RT_ERR_OK!=ret)
            return 0;      

       /*
        2. polling till local_time > nextGateStart
        
           change laser on/off shifht 
            on shift = 10
            off shift = 17
       */       
        epon_mpcp_waitLocalTimeExpire(curGateStart);
        data = 10;
        ret = reg_field_write(APOLLOMP_EPON_ASIC_TIMING_ADJUST2r,APOLLOMP_LSR_ON_SHIFTf,&data);
        if(RT_ERR_OK!=ret)
            return 0;      

        data = 17;
        ret = reg_field_write(APOLLOMP_EPON_ASIC_TIMING_ADJUST2r,APOLLOMP_LSR_OFF_SHIFTf,&data);
        if(RT_ERR_OK!=ret)
            return 0;      

        
        
        oltDirctBTBCnt++;
        
        mpcpPreEndTime =mpcpEndTime; 
        
        return 1;    
    }
    else
    {
        mpcpPreEndTime =mpcpEndTime; 
        
        return 0;    
    }
}


int epon_mpcp_isValidBen(unsigned int *nextStartTime)
{
    unsigned int startTime=0,tmpData1,tmpData2;
    int validCnt=0,invalidCnt=0,ret=1;
    unsigned int benOn;
    unsigned int localtime;
   
    while(1){

        /*gate grant list start time*/        
        (*((volatile unsigned int *)(0xBB036018)))= 0x2081;
        tmpData1=(*((volatile unsigned int *)(0xBB03601C)));
        startTime = tmpData1 & 0x00FFFFFF;
        (*((volatile unsigned int *)(0xBB036018)))= 0x2082;
        tmpData2=(*((volatile unsigned int *)(0xBB03601C)));
        startTime = startTime+((tmpData2&0x000000FF)<<24);
        
        *nextStartTime = startTime;
        
        
        (*((volatile unsigned int *)(0xBB036018)))= 0x208B;
        benOn = (*((volatile unsigned int *)(0xBB03601C)));
    
        localtime = (*((volatile unsigned int *)(0xBB036230)));
        
        
        if(((startTime - localtime)< 0x0EE6B280) && ((startTime - localtime) > 1))
        {        
            if(benOn & 0x00040000)
            {
                invalidCnt++;
                validCnt=0;     
            }
            else
            {
                invalidCnt=0;
                validCnt++;     
            }
            
        }
        else
        {
            invalidCnt=0;
            validCnt++;     
        }
        
        if(invalidCnt > 5)
        {
            return 0;
        }
        if(validCnt > 5)
        {
            return 1;
        }
    }
    
    if((startTime - localtime) >= 0x0fffffff)
    {
        gateRxExpire++;
    }    

    return ret;    
    
}




unsigned int latestCurrentGateStart=0; 
unsigned int CurrentGateStartNotChangeCnt=0; 
unsigned int lastGateCnt=0;
unsigned int lastReportCnt=0;




unsigned int currentreportCntNotChangeCnt=0;


#define MPCP_CURRENT_START_NO_CHANGE_MAX  5
#define MPCP_CURRENT_REPORT_CNT_NO_CHANGE_MAX 10

int epon_mpcp_gateDbgRecord(void)
{
    unsigned int tmpData1=0;
    unsigned int recordData=0;
    unsigned int currentReportCnt;    
    unsigned int currentGateCnt;
    unsigned int currentDelta;
    unsigned int lastDelta;
    int ret =0;
    int regRet =0;


    if(currentGrantdbgIdx >= EPON_MPCP_MAX_GRANT_DBG_DB)
        currentGrantdbgIdx=0;

    /*
    getcurrent start time
    
    WR  EPON_DEBUG1.DBG_SEL[9:0]=0x4
    RD   PRB_EPMC[23:0]  (current start[23:0])
    WR  EPON_DEBUG1.DBG_SEL[9:0]=0x5
    RD   PRB_EPMC[7:0]  (current start[31:24])
    */
    (*((volatile unsigned int *)(0xBB036018)))= 0x2004;
    tmpData1=(*((volatile unsigned int *)(0xBB03601C)));
    tmpData1 = tmpData1 & 0x00FFFFFF;
    recordData = tmpData1;
   
    (*((volatile unsigned int *)(0xBB036018)))= 0x2005;
    tmpData1=(*((volatile unsigned int *)(0xBB03601C)));
    tmpData1 = tmpData1 & 0x000000FF;
    tmpData1 = tmpData1<<24;
    recordData = recordData+tmpData1;
    
    if(!(mpcpDbDbgFlag&EPON_MPCP_ANA_FLAG_STOP))
        grantDbgDb[currentGrantdbgIdx].curStartTime = recordData;
    
    

    /*check olt register or not*/
    tmpData1 =  (*((volatile unsigned int *)(0xBB036040)));
    
    if(tmpData1 & 0x8000)
    {
    }
    else
    {
        return 1;    
    }
    
    
    if(latestCurrentGateStart==recordData)
    {
        //printk("\n CurrentGateStart not change:0x%x",latestCurrentGateStart);
        CurrentGateStartNotChangeCnt++;    
    }
    else
    {
        CurrentGateStartNotChangeCnt=0;    
    }
    
    if(CurrentGateStartNotChangeCnt >= MPCP_CURRENT_START_NO_CHANGE_MAX)
    {
        mpcpDbDbgFlag |= EPON_MPCP_ANA_FLAG_STOP;
        gate_ana_flag = gate_ana_flag|EPON_MPCP_ANA_FLAG_STOP;
        //printk("\n###### #######\n");
        //printk("\n###### current gate start time not change exceed 5  #######\n");
        //printk("\n###### device will deregister !!#######\n");
        
        /*reset EPON*/
        tmpData1 = 0;
        regRet = reg_field_write(APOLLOMP_WSDS_DIG_00r,APOLLOMP_CFG_SFT_RSTB_EPONf,&tmpData1);
        if(RT_ERR_OK!=regRet)
        {
            printk("\nreg access fail %s %d",__FUNCTION__,__LINE__);    
        } 

        tmpData1 = 1;
        regRet = reg_field_write(APOLLOMP_WSDS_DIG_00r,APOLLOMP_CFG_SFT_RSTB_EPONf,&tmpData1);
        if(RT_ERR_OK!=regRet)
        {
            printk("\nreg access fail %s %d",__FUNCTION__,__LINE__);    
        } 

        ret = 1;
    }
    
    latestCurrentGateStart = recordData;
    /*
     current end time
    WR  EPON_DEBUG1.DBG_SEL[9:0]=0x5
    RD   PRB_EPMC[23:8]  (current end[15:0])
    WR  EPON_DEBUG1.DBG_SEL[9:0]=0x6
    RD   PRB_EPMC[15:0]  (current end[31:16])
     */
    (*((volatile unsigned int *)(0xBB036018)))= 0x2005;
    tmpData1=(*((volatile unsigned int *)(0xBB03601C)));
    tmpData1 = tmpData1 & 0x00FFFF00;
    recordData = tmpData1>>8;
   
    (*((volatile unsigned int *)(0xBB036018)))= 0x2006;
    tmpData1=(*((volatile unsigned int *)(0xBB03601C)));
    tmpData1 = tmpData1 & 0x0000FFFF;
    tmpData1 = tmpData1<<16;
    recordData = recordData+tmpData1;
    if(!(mpcpDbDbgFlag&EPON_MPCP_ANA_FLAG_STOP))
    grantDbgDb[currentGrantdbgIdx].curEndTime = recordData;
        

    /*
     local time
     */
    recordData = (*((volatile unsigned int *)(0xBB036230)));
    if(!(mpcpDbDbgFlag&EPON_MPCP_ANA_FLAG_STOP))
    grantDbgDb[currentGrantdbgIdx].localTime = recordData; 
        

    /*
    last process gate start:
    WR  EPON_DEBUG1.DBG_SEL[9:0]=0x1
    RD   PRB_EPMC[23:0]    (last start[23:0])
    WR  EPON_DEBUG1.DBG_SEL[9:0]=0x2
    RD   PRB_EPMC[7:0]  (last start[31:24])
    */
    
    (*((volatile unsigned int *)(0xBB036018)))= 0x2001;
    tmpData1=(*((volatile unsigned int *)(0xBB03601C)));
    tmpData1 = tmpData1 & 0x00FFFFFF;
    recordData = tmpData1;
   
    (*((volatile unsigned int *)(0xBB036018)))= 0x2002;
    tmpData1=(*((volatile unsigned int *)(0xBB03601C)));
    tmpData1 = tmpData1 & 0x000000FF;
    tmpData1 = tmpData1<<24;
    recordData = recordData+tmpData1;
    if(!(mpcpDbDbgFlag&EPON_MPCP_ANA_FLAG_STOP))
    grantDbgDb[currentGrantdbgIdx].lastStartTime = recordData;
        

    /*
    last process gate end
    WR  EPON_DEBUG1.DBG_SEL[9:0]=0x2
    RD   PRB_EPMC[23:8]    (last end[15:0])
    WR  EPON_DEBUG1.DBG_SEL[9:0]=0x3
    RD   PRB_EPMC[15:0]  (last start[31:16])
    */
    
    (*((volatile unsigned int *)(0xBB036018)))= 0x2002;
    tmpData1=(*((volatile unsigned int *)(0xBB03601C)));
    tmpData1 = tmpData1 & 0x00FFFF00;
    recordData = tmpData1>>8;
   
    (*((volatile unsigned int *)(0xBB036018)))= 0x2003;
    tmpData1=(*((volatile unsigned int *)(0xBB03601C)));
    tmpData1 = tmpData1 & 0x0000FFFF;
    tmpData1 = tmpData1<<16;
    recordData = recordData+tmpData1;
    if(!(mpcpDbDbgFlag&EPON_MPCP_ANA_FLAG_STOP))
    grantDbgDb[currentGrantdbgIdx].lastEndTime = recordData;

    currentGrantdbgIdx++;



    /*check gate or report cnt match or not*/

    /*get llid counter*/
    regRet = reg_array_field_read(APOLLOMP_DOT3_MPCP_TX_REPORTr, REG_ARRAY_INDEX_NONE, 0, APOLLOMP_DOT3MPCPTXREPORTf, &tmpData1);
    if(RT_ERR_OK!=regRet)
    {
        printk("\nreg access fail %s %d",__FUNCTION__,__LINE__);    
    } 



    currentReportCnt=tmpData1;



    if(currentReportCnt == lastReportCnt)
    {
        currentreportCntNotChangeCnt++;    
    }
    else
    {
        currentreportCntNotChangeCnt=0;    
    }


    if(currentreportCntNotChangeCnt >= MPCP_CURRENT_REPORT_CNT_NO_CHANGE_MAX)
    {
        printk("\n !!!! no report tx mpcp have some problem  reset epon\n");
        
        /*reset EPON*/
        tmpData1 = 0;
        regRet = reg_field_write(APOLLOMP_WSDS_DIG_00r,APOLLOMP_CFG_SFT_RSTB_EPONf,&tmpData1);
        if(RT_ERR_OK!=regRet)
        {
            printk("\nreg access fail %s %d",__FUNCTION__,__LINE__);    
        } 

        tmpData1 = 1;
        regRet = reg_field_write(APOLLOMP_WSDS_DIG_00r,APOLLOMP_CFG_SFT_RSTB_EPONf,&tmpData1);
        if(RT_ERR_OK!=regRet)
        {
            printk("\nreg access fail %s %d",__FUNCTION__,__LINE__);    
        } 
        
        ret = 1;        
        currentreportCntNotChangeCnt=0;      
    }
    regRet = reg_array_field_read(APOLLOMP_DOT3_MPCP_EX_GATEr, REG_ARRAY_INDEX_NONE, 0, APOLLOMP_DOT3MPCPRXGATEf, &tmpData1);
    if(RT_ERR_OK!=regRet)
    {
        printk("\nreg access fail %s %d",__FUNCTION__,__LINE__);    
    } 

    currentGateCnt=tmpData1;
    
    currentDelta = currentGateCnt - currentReportCnt;
    lastDelta = lastGateCnt - lastReportCnt;
    

    if(currentDelta > 10)
    {
        if(mpcpDbDbgFlag&EPON_MPCP_REPORT_CKECK_ENABLE)
        {        
            printk("\ngate report not match (c,l)(%d,%d) \n",currentDelta,lastDelta);  
            mpcpDbDbgFlag |= EPON_MPCP_ANA_FLAG_STOP;  
            gate_ana_flag = gate_ana_flag|EPON_MPCP_ANA_FLAG_STOP;
        }
    }


    lastReportCnt = currentReportCnt;
    lastGateCnt = currentGateCnt;
    
    
    return ret;
}


#endif



void epon_mpcp_gate_checking(void)
{
#ifdef CONFIG_SDK_APOLLOMP
    unsigned int localtime,firstLocalTime;
    
    unsigned int nextGateStart=0,tmpData1,tmpData2,data;
    unsigned int EndTime=0;
    unsigned int mpcpTimeStamp=0;
    int isGateBTB=0;
    int ret;
    /*gate record*/
    if(epon_mpcp_gateDbgRecord()==1)
        return;    


    isGateBTB = epon_mpcp_isGateBTB();
    /*
    get mpcpTimeStamp
    Write  EPON_DEBUG1.DBG_SEL[9:6] = 0x0
    Write  EPON_DEBUG1.DBG_SEL[5:0] = 0xA
    Read   EPON_DEBUG2.PRB_EPMC[23:0]  (timestamp[23:0])
    Write  EPON_DEBUG1.DBG_SEL[5:0] = 0xB
    Read   EPON_DEBUG2.PRB_EPMC[7:0]    (timestamp[31:24])
    */
    /*Read   EPON_DEBUG2.PRB_EPMC[23:0]  (timestamp[23:0])*/     
    (*((volatile unsigned int *)(0xBB036018)))= 0x200a;
    tmpData1=(*((volatile unsigned int *)(0xBB03601C)));
    tmpData1 = tmpData1 & 0x00FFFFFF;
    mpcpTimeStamp = tmpData1;
    
    /*Read   EPON_DEBUG2.PRB_EPMC[7:0]    (timestamp[31:24])*/
    (*((volatile unsigned int *)(0xBB036018)))= 0x200b;
    tmpData2=(*((volatile unsigned int *)(0xBB03601C)));
    mpcpTimeStamp = mpcpTimeStamp+((tmpData2&0x000000FF)<<24);


    /*gate grant list end time*/   
    /*Read   EPON_DEBUG2.PRB_EPMC[23:8]  (grant end[15:0])*/     
    (*((volatile unsigned int *)(0xBB036018)))= 0x2005;
    tmpData1=(*((volatile unsigned int *)(0xBB03601C)));
    tmpData1 = tmpData1 & 0x00FFFF00;
    EndTime = tmpData1 >> 8;
    
    /*Read   EPON_DEBUG2.PRB_EPMC[15:0]  (grant end[31:16])*/
    (*((volatile unsigned int *)(0xBB036018)))= 0x2006;
    tmpData2=(*((volatile unsigned int *)(0xBB03601C)));
    EndTime = EndTime+((tmpData2&0x0000FFFF)<<16);


    localtime = (*((volatile unsigned int *)(0xBB036230)));
    firstLocalTime = localtime;
    mpcp_intr_cnt ++;
   


    udelay(2);


    /*chack if invalid BEN occurs*/    
    if(epon_mpcp_isValidBen(&nextGateStart)==1)
    {
        /*do nothing*/
    }
    else
    {
        localtime = (*((volatile unsigned int *)(0xBB036230)));
        /*invalid laser*/
        /* - force BEN off
           - set tcont valid to 0
        */

        /*force turn off BEN till next laser on*/
        data = 0;
        ret=reg_field_write(APOLLOMP_FORCE_BENr, APOLLOMP_BEN_FORCE_VALUEf, &data);
        if(RT_ERR_OK!=ret)
        {
            printk("\nreg access fail %s %d",__FUNCTION__,__LINE__);    
        }
        
        data = 1;
        ret=reg_field_write(APOLLOMP_FORCE_BENr, APOLLOMP_BEN_FORCE_MODEf, &data);
        if(RT_ERR_OK!=ret)
        {
            printk("\nreg access fail %s %d",__FUNCTION__,__LINE__);    
        }

        data = 1;
        ret=reg_field_write(APOLLOMP_WSDS_DIG_18r, APOLLOMP_CFG_FRC_BENf, &data);
        if(RT_ERR_OK!=ret)
        {
            printk("\nreg access fail %s %d",__FUNCTION__,__LINE__);    
        }
        
        /*set tcont 0 disable, force next gate not schedule, prevent next gate no BEN packet tx by EPON mac*/
        data = 0;                        
        ret=reg_array_field_write(APOLLOMP_PON_TCONT_ENr,REG_ARRAY_INDEX_NONE, 0, APOLLOMP_TCONT_ENf, &data);
        if(RT_ERR_OK!=ret)
        {
            printk("\nreg access fail %s %d",__FUNCTION__,__LINE__);    
        }
        
        
        /*polling till local time > next grant start*/
        epon_mpcp_waitLocalTimeExpire(nextGateStart);
        /*
           LocalTime > nextGateStart
           - set BEN to normal
           - set tcont valid to 1 
        */
        
        /*set laser to normal mode*/
        data = 0;
        ret=reg_field_write(APOLLOMP_FORCE_BENr, APOLLOMP_BEN_FORCE_VALUEf, &data);
        if(RT_ERR_OK!=ret)
        {
            printk("\nreg access fail %s %d",__FUNCTION__,__LINE__);    
        }
        data = 0;

        ret=reg_field_write(APOLLOMP_FORCE_BENr, APOLLOMP_BEN_FORCE_MODEf, &data);
        if(RT_ERR_OK!=ret)
        {
            printk("\nreg access fail %s %d",__FUNCTION__,__LINE__);    
        }
        data = 0;
        ret=reg_field_write(APOLLOMP_WSDS_DIG_18r, APOLLOMP_CFG_FRC_BENf, &data);
        if(RT_ERR_OK!=ret)
        {
            printk("\nreg access fail %s %d",__FUNCTION__,__LINE__);    
        }
       
        /*set tcont 0 enable*/
        data = 1;                        
        ret=reg_array_field_write(APOLLOMP_PON_TCONT_ENr,REG_ARRAY_INDEX_NONE, 0, APOLLOMP_TCONT_ENf, &data);
        if(RT_ERR_OK!=ret)
        {
            printk("\nreg access fail %s %d",__FUNCTION__,__LINE__);    
        }
             
        inValidLaserCnt++;
        laserNeedChaned=1;

        laserCheckFailForceStop=1;
        laserCheckFailMpcpBuffer=LASER_CHECK_FAIL_BUFFER_CNT;

        //if(laser_patch_flag&EPON_LASER_PATCH_FLAG_DBGMSG)
        {
            printk("\n TS:0x%8.8x LT:(2)0x%8.8x/(1)0x%8.8x GS:0x%8.8x GD:0x%8.8x",mpcpTimeStamp,localtime,firstLocalTime,nextGateStart,EndTime);
        }


        if(isGateBTB==1)
            printk("\n warnning!! isGateBTB and inValid laser occurs\n");
    }

    
#if 0    
    if(_eponTimeDriftIntCnt%3000 == 0)
    {
        printk("\n (timeStamp) 0x%8.8x LT:(2)0x%8.8x/(1)0x%8.8x GS:0x%8.8x GD:0x%8.8x",mpcpTimeStamp,localtime,firstLocalTime,startTime,EndTime);
        printk("\n Ben:%d\n",(benOn&0x00040000));
    }
#endif
    

#endif 
}

#ifdef CONFIG_SDK_APOLLOMP

void epon_mpcp_verF_checking()
{
    
    if(epon_mpcp_gateDbgRecord()==1)
    {
        return;      
    }
}

#endif

void epon_mpcp_isr_entry(void)
{

    int ret;
    
    /*check if no llid entry valid*/
    rtk_epon_llid_entry_t llidEntry;


    
    /*get LLID*/            
    llidEntry.llidIdx = 0;
    ret = rtk_epon_llid_entry_get(&llidEntry);
 
    if(RT_ERR_OK == ret)
    {
        if(llidEntry.valid != DISABLED)
        { 
            #ifdef CONFIG_SDK_APOLLOMP
            if(APOLLOMP_CHIP_ID==chipId)
                if(rev > CHIP_REV_ID_F)
                {
                    epon_mpcp_verF_checking();
                }           
                else
                {
                    epon_mpcp_gate_checking();
                }
            #endif
        }
    }
    if(rtk_intr_ims_clear(INTR_TYPE_ACL_ACTION) != RT_ERR_OK)
	{
        return;
	}

    return;
}


/*acl trap drop init*/
static int epon_mpcp_pkt_acl_dropIntr_register(void)
{
    int32  ret;

    /*ACL init*/
#ifndef CONFIG_RTK_L34_ENABLE
    rtk_acl_field_entry_t aclField;
    rtk_acl_template_t aclTemplete;
    rtk_acl_ingress_entry_t aclRule;
    rtk_acl_field_t fieldHead1,fieldHead2;
    
    /*drop all mpcp gate message*/
    /*field selector*/
    aclField.index = 3;
    aclField.format = ACL_FORMAT_RAW;
    aclField.offset = 14;
    if ((ret = rtk_acl_fieldSelect_set(&aclField)) != RT_ERR_OK)
    {
        return ret;
    }
    
    /*set templete*/
    aclTemplete.index = 3;
    if ((ret = rtk_acl_template_get(&aclTemplete)) != RT_ERR_OK)
    {
        return ret;
    }
    aclTemplete.fieldType[0]=ACL_FIELD_ETHERTYPE;
    aclTemplete.fieldType[1]=ACL_FIELD_USER_DEFINED03;
    if ((ret = rtk_acl_template_set(&aclTemplete)) != RT_ERR_OK)
    {
        return ret;
    }
    
    /*add acl rule drop all gate message and trigger interrupt*/
    osal_memset(&aclRule, 0, sizeof(rtk_acl_ingress_entry_t));
    aclRule.index = 63;
    aclRule.templateIdx = 3;
    /*enable ACL on PON port*/
    RTK_PORTMASK_PORT_SET(aclRule.activePorts,phy_pon_port);
    aclRule.valid = ENABLED;
    aclRule.act.enableAct[ACL_IGR_INTR_ACT] = ENABLED;
    aclRule.act.enableAct[ACL_IGR_FORWARD_ACT] = ENABLED;
    aclRule.act.aclInterrupt = ENABLED;
    aclRule.act.forwardAct.act = ACL_IGR_FORWARD_DROP_ACT;
    
    fieldHead1.fieldType = ACL_FIELD_ETHERTYPE;
    fieldHead1.fieldUnion.data.value = 0x8808;
    fieldHead1.fieldUnion.data.mask = 0xFFFF;
    fieldHead1.next = NULL;
    if ((ret = rtk_acl_igrRuleField_add(&aclRule,&fieldHead1)) != RT_ERR_OK)
    {
        return ret;
    }

    fieldHead2.fieldType = ACL_FIELD_USER_DEFINED03;
    fieldHead2.fieldUnion.data.value = 0x0002;
    fieldHead2.fieldUnion.data.mask = 0xFFFF;                      
    fieldHead2.next = NULL;
    if ((ret = rtk_acl_igrRuleField_add(&aclRule,&fieldHead2)) != RT_ERR_OK)
    {
        return ret;
    }
    
    if ((ret = rtk_acl_igrRuleEntry_add(&aclRule)) != RT_ERR_OK)
    {
        return ret;
    }

    if ((ret = rtk_acl_igrState_set(phy_pon_port, ENABLED)) != RT_ERR_OK)
    {
        return ret;
    }
#endif

    /*register ACL interrupt*/
       /*register interrupt handle*/
    if((ret = rtk_irq_isr_register(INTR_TYPE_ACL_ACTION,epon_mpcp_isr_entry)) != RT_ERR_OK)
    {
    	return ret;
    }
    
    if((ret = rtk_intr_imr_set(INTR_TYPE_ACL_ACTION,ENABLED)) != RT_ERR_OK)
    {
        return ret;
    }
    
    return 0;
}



static void epon_mpcp_pkt_register(void)
{
    extern int drv_nic_register_rxhook(int portmask,int priority,p2rfunc_t rx);
    
    rtk_portmask_t portMask;
    rtk_switch_portMask_Clear(&portMask);
    rtk_switch_port2PortMask_set(&portMask,RTK_PORT_PON);
    rtl960x_pon_port_mask = portMask.bits[0];

    #if defined(FPGA_DEFINED)
    drv_nic_register_rxhook(0xFF, RE8686_RXPRI_MPCP, epon_mpcp_pkt_rx);
    #else
    drv_nic_register_rxhook(rtl960x_pon_port_mask, RE8686_RXPRI_MPCP, epon_mpcp_pkt_rx);
    #endif
    
}       



int __init epon_mpcp_init(void)
{
    int32  ret;
    uint32  data;
    uint32  needChangeLaser=0;
            
    ret = rtk_switch_version_get(&chipId, &rev, &subtype);
    if(RT_ERR_OK != ret)
    {
        printk("err:%s %d",__FUNCTION__,__LINE__);
        return  ret;   
    }        

    ret =rtk_switch_phyPortId_get(RTK_PORT_PON, &phy_pon_port);
    if(RT_ERR_OK != ret)
    {
        printk("err:%s %d",__FUNCTION__,__LINE__);
        return  ret;   
    }        


#ifdef CONFIG_SDK_APOLLOMP
    if(APOLLOMP_CHIP_ID==chipId)
    {
        switch(rev)        
        {        
            case CHIP_REV_ID_A:
            case CHIP_REV_ID_B:
            case CHIP_REV_ID_C:
            case CHIP_REV_ID_D:
            case CHIP_REV_ID_E:
            case CHIP_REV_ID_F:
                /*accept special packet on pon port, for mpcp register packet*/
                data = 1;
 
                if ((ret = reg_array_field_write(APOLLOMP_P_MISCr, phy_pon_port, REG_ARRAY_INDEX_NONE, APOLLOMP_RX_SPCf, &data)) != RT_ERR_OK)
                {
                    return ret;
                }

                if ((ret = reg_array_field_write(APOLLOMP_P_MISCr, phy_pon_port, REG_ARRAY_INDEX_NONE, APOLLOMP_ACCEPT_RX_ERRORf, &data)) != RT_ERR_OK)
                {
                    return ret;
                }
                /*MPCP Gate frame handle set to asic handle*/
                data = 0x2; /*normal gate to CPU*/
                if ((ret = reg_field_write(APOLLOMP_EPON_MPCP_CTRr,APOLLOMP_GATE_TRAP_TYPEf,&data)) != RT_ERR_OK)
                {
                    return ret;
                }
                data = 0x1; /*gate handle asic and trap to CPU*/
                if ((ret = reg_field_write(APOLLOMP_EPON_MPCP_CTRr,APOLLOMP_GATE_HANDLEf,&data)) != RT_ERR_OK)
                {
                    return ret;
                }            
                /*ACL init*/
                ret = epon_mpcp_pkt_acl_dropIntr_register();
                if(RT_ERR_OK!=ret)
                {
                    printk("\nerr %s %d",__FUNCTION__,__LINE__);    
                }  
                
                /* for mpcp timedrift check
                reg set 0x36038 0x1
                reg set 0x2281c 0x059
                reg set 0x22808 0x5080
                */
    
                data=0x1;
                ret = reg_field_write(APOLLOMP_EPON_TIME_CTRLr,APOLLOMP_RTT_ADJf,&data);
                if(RT_ERR_OK!=ret)
                {
                    printk("\nreg access fail %s %d",__FUNCTION__,__LINE__);    
                }                 
                
                data=0x059;
                ret = reg_write(APOLLOMP_SDS_REG7r, &data);
                if(RT_ERR_OK!=ret)
                {
                    printk("\nreg access fail %s %d",__FUNCTION__,__LINE__);    
                }                 

                data=0x5080;
                ret = reg_write(APOLLOMP_SDS_REG2r, &data);
                if(RT_ERR_OK!=ret)
                {
                    printk("\nreg access fail %s %d",__FUNCTION__,__LINE__);    
                }                 

                needChangeLaser=1;
                
                
                /*
                   for next gate start time read :
                        Write  PRB_GN    =0x1F       (initial)
                        Write  PRB_LST_GN=1          (initail)

                */
                data=0x1f;
                ret = reg_field_write(APOLLOMP_EPON_DEBUG2r,APOLLOMP_PRB_GNf,&data);
                if(RT_ERR_OK!=ret)
                {
                    printk("\nreg access fail %s %d",__FUNCTION__,__LINE__);    
                }                 

                data=0x1;
                ret = reg_field_write(APOLLOMP_EPON_MISC_CFGr,APOLLOMP_PRB_LST_GNf,&data);
                if(RT_ERR_OK!=ret)
                {
                    printk("\nreg access fail %s %d",__FUNCTION__,__LINE__);    
                }                 

            break;
            default:
#if VER_F_MPCP_CPU_HANDLE == 1
                /*accept special packet on pon port, for mpcp register packet*/
                data = 1;
 
                if ((ret = reg_array_field_write(APOLLOMP_P_MISCr, phy_pon_port, REG_ARRAY_INDEX_NONE, APOLLOMP_RX_SPCf, &data)) != RT_ERR_OK)
                {
                    return ret;
                }

                if ((ret = reg_array_field_write(APOLLOMP_P_MISCr, phy_pon_port, REG_ARRAY_INDEX_NONE, APOLLOMP_ACCEPT_RX_ERRORf, &data)) != RT_ERR_OK)
                {
                    return ret;
                }
                /*MPCP Gate frame handle set to asic handle*/
                data = 0x2; /*normal gate to CPU*/
                if ((ret = reg_field_write(APOLLOMP_EPON_MPCP_CTRr,APOLLOMP_GATE_TRAP_TYPEf,&data)) != RT_ERR_OK)
                {
                    return ret;
                }
                data = 0x1; /*gate handle asic and trap to CPU*/
                if ((ret = reg_field_write(APOLLOMP_EPON_MPCP_CTRr,APOLLOMP_GATE_HANDLEf,&data)) != RT_ERR_OK)
                {
                    return ret;
                }            
                /*ACL init*/
                ret = epon_mpcp_pkt_acl_dropIntr_register();
                if(RT_ERR_OK!=ret)
                {
                    printk("\nerr %s %d",__FUNCTION__,__LINE__);    
                }  
                
                /* for mpcp timedrift check
                reg set 0x36038 0x1
                reg set 0x2281c 0x059
                reg set 0x22808 0x5080
                */
                data=0x1;
                ret = reg_field_write(APOLLOMP_EPON_TIME_CTRLr,APOLLOMP_RTT_ADJf,&data);
                if(RT_ERR_OK!=ret)
                {
                    printk("\nreg access fail %s %d",__FUNCTION__,__LINE__);    
                }                 
                
                data=0x059;
                ret = reg_write(APOLLOMP_SDS_REG7r, &data);
                if(RT_ERR_OK!=ret)
                {
                    printk("\nreg access fail %s %d",__FUNCTION__,__LINE__);    
                }                 
                data=0x5080;
                ret = reg_write(APOLLOMP_SDS_REG2r, &data);
                if(RT_ERR_OK!=ret)
                {
                    printk("\nreg access fail %s %d",__FUNCTION__,__LINE__);    
                }                 
                /*
                   for next gate start time read :
                        Write  PRB_GN    =0x1F       (initial)
                        Write  PRB_LST_GN=1          (initail)
                */
                data=0x1f;
                ret = reg_field_write(APOLLOMP_EPON_DEBUG2r,APOLLOMP_PRB_GNf,&data);
                if(RT_ERR_OK!=ret)
                {
                    printk("\nreg access fail %s %d",__FUNCTION__,__LINE__);    
                }                 

                data=0x1;
                ret = reg_field_write(APOLLOMP_EPON_MISC_CFGr,APOLLOMP_PRB_LST_GNf,&data);
                if(RT_ERR_OK!=ret)
                {
                    printk("\nreg access fail %s %d",__FUNCTION__,__LINE__);    
                } 
                needChangeLaser=0;
#else  /*mpcp handle by ASIC*/
                data = 0;
                if ((ret = reg_array_field_write(APOLLOMP_P_MISCr, phy_pon_port, REG_ARRAY_INDEX_NONE, APOLLOMP_RX_SPCf, &data)) != RT_ERR_OK)
                {
                    return ret;
                }
                data = 0;
                if ((ret = reg_array_field_write(APOLLOMP_P_MISCr, phy_pon_port, REG_ARRAY_INDEX_NONE, APOLLOMP_ACCEPT_RX_ERRORf, &data)) != RT_ERR_OK)
                {
                    return ret;
                }
                data = 0x0; /*gate handle asic*/
                if ((ret = reg_field_write(APOLLOMP_EPON_MPCP_CTRr,APOLLOMP_GATE_HANDLEf,&data)) != RT_ERR_OK)
                {
                    return ret;
                }
                needChangeLaser=0;
#endif
            break;        

        }

    }


    if(needChangeLaser==1)
    {
        
        pEponLaserChangeCheckTask = kthread_create(epon_laser_change_polling_thread, NULL, "epon_polling");
        if(IS_ERR(pEponLaserChangeCheckTask))
        {
            printk("%s:%d epon_mpcp_init failed %ld!\n", __FILE__, __LINE__, PTR_ERR(pEponLaserChangeCheckTask));
        }
        else
        {
            wake_up_process(pEponLaserChangeCheckTask);
        }
    }
#endif

    
    epon_mpcp_pkt_register();

#ifdef CONFIG_SDK_APOLLOMP    
    epon_mpcp_packet_dbg_init();
#endif
    printk("\nepon_mpcp_init ok!!\n");

    return 0;
}

void __exit epon_mpcp_exit(void)
{
#ifdef CONFIG_SDK_APOLLOMP    
    kthread_stop(pEponLaserChangeCheckTask);
    epon_mpcp_dbg_exit();
#endif    
}

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("RealTek EPON mpcp check module");
MODULE_AUTHOR("Realtek");
module_init(epon_mpcp_init);
module_exit(epon_mpcp_exit);
#endif
