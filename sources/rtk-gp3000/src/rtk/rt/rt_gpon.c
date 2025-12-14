/*
 * Copyright (C) 2018 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: $
 * $Date: $
 *
 * Purpose : Definition of GPON Global API
 *
 * Feature : The file have include the following module and sub-modules
 *           (1) GPON parameter settings
 *           (2) Management address and vlan configuration.
 *
 */

/*
 * Include Files
 */
#include <common/rt_error.h>
#include <common/rt_type.h>
#include <hal/chipdef/chip.h>
#include <rtk/init.h>
#include <rtk/default.h>
#include <dal/dal_mgmt.h>
#include <common/util/rt_util.h>
#include <hal/common/halctrl.h>
#include <rtk/rt/rt_gpon.h>
#include <rtk/rt/rt_rate.h>

#ifndef CONFIG_LUNA_G3_SERIES
#include <osal/print.h>
#if defined(CONFIG_SDK_KERNEL_LINUX)
    #include <linux/list.h>
    #include <linux/slab.h>
    #include <linux/file.h>
    #include <linux/seq_file.h>
    #include <linux/proc_fs.h>
    #include <linux/uaccess.h>
    #include <linux/version.h>
    #include <linux/proc_fs.h>
#endif
#if (CONFIG_GPON_VERSION > 1)
#include <rtk/gponv2.h>
#include <module/gpon/gpon.h>
#include <module/gpon/gpon_platform.h>
#endif
#ifdef CONFIG_SDK_MODULES
#include <pkt_redirect.h>
#endif
#endif

#ifdef CONFIG_RTK_GPON_EXCEP_LOG
#if defined(CONFIG_SDK_KERNEL_LINUX)
#include <linux/exception_log.h>
#endif
#endif

/*
 * Function Declaration
 */

#define PON_RATE_8KBPS_TO_KBPS(_8kbps)  ((_8kbps) << 3)
#define PON_RATE_KBPS_TO_8KBPS(_kbps)  ((_kbps) >> 3)


#ifndef CONFIG_LUNA_G3_SERIES

static int32 rt_rtk_gpon_usFlow_set(uint32 usFlowId, rt_gpon_usFlow_t *pUsFlow);
static int32 rt_rtk_gpon_tcont_set(uint32 *pTcontId, uint32 allocId);
static void  rt_rtk_gpon_db_dump(void);
static int32 rt_rtk_gpon_tcont_get(uint32 tcontId, uint32 *pAllocId);
static void pon_queue_entry_dump(uint32 tcontId);
static void gemFlow_entry_dump(void);
int32 rt_rtk_gpon_attribute_get(rt_gpon_attribute_type_t attributeType,rt_gpon_attribute_t *pAttributeValue);
int32 rt_rtk_gpon_attribute_set(rt_gpon_attribute_type_t attributeType,rt_gpon_attribute_t *pAttributeValue);

static uint64 cur_time=0, last_time=0, time_diff;

/****************************************************************/
/* Symbol Definition                                            */
/****************************************************************/
#define RT_GPON_INVALID_PHY_QUEUE   (0xFFFF)
#define RT_GPON_INVALID_US_GEM_FLOW   (0xFFFF)
#define RT_GPON_INVALID_ALLOCID_VAL (0xFFFF)


enum
{
	RT_GPON_LOG_LEVEL_OFF = 0,  /*For disable log*/
	RT_GPON_LOG_LEVEL_ERR,
	RT_GPON_LOG_LEVEL_DBG,
	RT_GPON_LOG_LEVEL_END
};


/****************************************************************/
/* Type Definition                                              */
/****************************************************************/

typedef struct {
#if defined(CONFIG_SDK_KERNEL_LINUX)
    struct  list_head   tcontQueueHead;
#endif
    uint16              phyTcontId; // Physical T-CONT ID
    uint16              allocId;   //T-CONT allodID
} rt_tcont_info_t;

typedef struct{
    unsigned int	omccTcont;
    unsigned int	omccQueue;
    unsigned int	omccFlow;
    unsigned int	maxTcont;
    unsigned int	maxTcontQueue;
    unsigned int	maxPonQueue;
    unsigned int	maxQueueRate;
    unsigned int	maxFlow;
    unsigned int	tcontNumPerGroup;
    rt_tcont_info_t     *pTcontInfo;
#if defined(CONFIG_SDK_KERNEL_LINUX)
    struct list_head    usGemFlowHead;
#endif
    unsigned char       *pGemEncrypt;
} rt_rtk_gpon_platform_db_t;


typedef struct {
    uint16              flowId;     //GEM flow ID
    rt_gpon_usFlow_t    gemFlowcfg; //GEM flow config
#if defined(CONFIG_SDK_KERNEL_LINUX)
    struct list_head	list;
#endif
} gem_flow_info_t;


typedef struct {
    uint16              flowId;     //GEM flow ID
    uint16              tcQueueId;  //T-CONT queue ID
    uint16              phyQueueId; //Physical queue ID
    rt_gpon_queueCfg_t  Queuecfg;   //Queue config
#if defined(CONFIG_SDK_KERNEL_LINUX)
    struct list_head	list;
#endif
} pon_queue_info_t;


// to maintain physical queue to tcontId
typedef struct {
    uint8 	valid;
} rt_rtk_phyQueue_t;

typedef struct {
    uint8 	valid;
} rt_rtk_phyTcont_t;


rt_rtk_gpon_platform_db_t gRtkPlatformDb;
static rt_rtk_phyQueue_t *gPhyQueueDb;
static rt_rtk_phyTcont_t *gPhyTcontDb;

#if defined(CONFIG_SDK_KERNEL_LINUX)
extern struct proc_dir_entry* rt_api_proc_dir;
struct proc_dir_entry* rt_gpon_proc_dir = NULL;
struct proc_dir_entry* rt_gpon_info = NULL;
struct proc_dir_entry* rt_gpon_logLvl = NULL;
#endif

#if defined(CONFIG_SDK_KERNEL_LINUX)
static int gRtGponLogLevel = RT_GPON_LOG_LEVEL_ERR;
#endif

/****************************************************************/
/* Macro Declaration                                            */
/****************************************************************/

#if defined(CONFIG_SDK_KERNEL_LINUX)
#define RT_GPON_LOG(loglevel,fmt, arg...) \
           do { if (gRtGponLogLevel >= loglevel) { printk(fmt, ##arg); printk("\n"); } } while (0);
#else
#define RT_GPON_LOG(loglevel,fmt, arg...)
#endif

#define RT_GPON_LOG_ERR(fmt, arg...) RT_GPON_LOG(RT_GPON_LOG_LEVEL_ERR, fmt, ##arg)
#define RT_GPON_LOG_DBG(fmt, arg...) RT_GPON_LOG(RT_GPON_LOG_LEVEL_DBG, fmt, ##arg)

#define LOGIC_TCONID_TO_PHY_TCONID(_logicTcontId) (gRtkPlatformDb.pTcontInfo[_logicTcontId].phyTcontId)

#define RT_SCHTYPE_TO_RTK_SCHTYPE(__rt_schType) (__rt_schType ==RT_GPON_TCONT_QUEUE_SCHEDULER_SP?STRICT_PRIORITY:WFQ_WRR_PRIORITY)

#if defined(CONFIG_SDK_KERNEL_LINUX)
static int rt_gpon_info_read_proc(struct seq_file *seq, void *v)
{
	uint32 allocId = RT_GPON_INVALID_ALLOCID_VAL;
	uint32 tcontId;
	
	rt_rtk_gpon_db_dump();
	gemFlow_entry_dump();

	for(tcontId = 0; tcontId<gRtkPlatformDb.maxTcont;tcontId++)
	{
		if(RT_ERR_OK == rt_rtk_gpon_tcont_get(tcontId, &allocId))
		{
			if(allocId != RT_GPON_INVALID_ALLOCID_VAL)
				pon_queue_entry_dump(tcontId);;
		}
		
	}
	
    return 0;
}

static int rt_gpon_info_open_proc(struct inode *inode, struct file *file)
{
    return single_open(file, rt_gpon_info_read_proc, NULL);
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
struct proc_ops rt_gpon_info_fop = {
    .proc_open           = rt_gpon_info_open_proc,
    .proc_read           = seq_read,
    .proc_lseek          = seq_lseek,
    .proc_release        = single_release,
};
#else
struct file_operations rt_gpon_info_fop = {
    .open =  rt_gpon_info_open_proc,
    .read = seq_read,
    .llseek = seq_lseek,
    .release = single_release,
};
#endif

static int rt_gpon_logLvl_read_proc(struct seq_file *seq, void *v)
{

	seq_printf(seq, "RT GPON API Log level: %d\n\n", gRtGponLogLevel);

	seq_printf(seq, "Off: %d\n", RT_GPON_LOG_LEVEL_OFF);
	seq_printf(seq, "Error: %d\n", RT_GPON_LOG_LEVEL_ERR);
	seq_printf(seq, "Debug: %d\n", RT_GPON_LOG_LEVEL_DBG);
	
    return 0;
}


static int rt_gpon_logLvl_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos)
{
	char tmpbuf[64] = {0};
	int level;

	if (buffer && !copy_from_user(tmpbuf, buffer, count))
	{
		level = simple_strtol(tmpbuf, NULL, 10);
		if(level < RT_GPON_LOG_LEVEL_OFF || level >= RT_GPON_LOG_LEVEL_END)
		{
			printk("only accept %d ~ %d!\n", RT_GPON_LOG_LEVEL_OFF, RT_GPON_LOG_LEVEL_END-1);
			return -EFAULT;
		}

		gRtGponLogLevel = level;
	}

	return count;
}


static int rt_gpon_logLvl_open_proc(struct inode *inode, struct file *file)
{
    return single_open(file, rt_gpon_logLvl_read_proc, inode->i_private);
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
static const struct proc_ops rt_gpon_log_fop = {
    .proc_open           = rt_gpon_logLvl_open_proc,
    .proc_write          = rt_gpon_logLvl_write,
    .proc_read           = seq_read,
    .proc_lseek          = seq_lseek,
    .proc_release        = single_release,
};
#else
static const struct file_operations rt_gpon_log_fop = {
    .owner          = THIS_MODULE,
    .open           = rt_gpon_logLvl_open_proc,
    .write          = rt_gpon_logLvl_write,
    .read           = seq_read,
    .llseek         = seq_lseek,
    .release        = single_release,
};
#endif

/*Remove logic/physical T-CONT ID database*/
static int 
tcont_db_remove(uint32 phyTcontId)
{

    uint32 logicTcontId;

    RT_PARAM_CHK((phyTcontId >= gRtkPlatformDb.maxTcont), RT_ERR_INPUT);

    /*Remove physical T-CONT ID database*/
    gPhyTcontDb[phyTcontId].valid = FALSE;

    /*Remove logic T-CONT ID database*/
    for(logicTcontId = 0;logicTcontId<gRtkPlatformDb.maxTcont;logicTcontId++)
    {
        if(phyTcontId == gRtkPlatformDb.pTcontInfo[logicTcontId].phyTcontId)
        {
            gRtkPlatformDb.pTcontInfo[logicTcontId].phyTcontId = phyTcontId;
            gRtkPlatformDb.pTcontInfo[logicTcontId].allocId = RT_GPON_INVALID_ALLOCID_VAL;
            return RT_ERR_OK;
        }

    }

    return RT_ERR_OK;
}



static int32
tcont_queue_num_get(uint32 tcontId)
{
    uint32 tcontQueueNum = 0;    
	struct list_head    *pEntry     = NULL;
    
	list_for_each(pEntry, &gRtkPlatformDb.pTcontInfo[tcontId].tcontQueueHead)
	{
        tcontQueueNum++;
	}

    return tcontQueueNum;
}


gem_flow_info_t* 
gemFlow_entry_find(uint32 usFlowId)
{

	struct list_head *pEntry     = NULL;
	gem_flow_info_t	 *pEntryData = NULL;

	list_for_each(pEntry, &gRtkPlatformDb.usGemFlowHead)
	{
		pEntryData = list_entry(pEntry, gem_flow_info_t, list);

    	if (pEntryData->flowId == usFlowId)
			return pEntryData;
	}

	return NULL;

}

static int32
gemFlow_entry_set(uint32 usFlowId, rt_gpon_usFlow_t *pUsFlow)
{
	struct list_head    *pEntry    = NULL;
	gem_flow_info_t	    *pCurEntry = NULL;
	gem_flow_info_t     *pNewEntry = NULL;

	list_for_each(pEntry, &gRtkPlatformDb.usGemFlowHead)
	{
		pCurEntry = list_entry(pEntry, gem_flow_info_t, list);

    	if (pCurEntry->flowId == usFlowId)
        {
            osal_memcpy(&pCurEntry->gemFlowcfg, pUsFlow, sizeof(rt_gpon_usFlow_t));
            return RT_ERR_OK;
        }   

	}

    // Add new item
    pNewEntry = kzalloc(sizeof(gem_flow_info_t), GFP_ATOMIC);
	if (!pNewEntry)
		return RT_ERR_FAILED;

    pNewEntry->flowId  = usFlowId;

    osal_memcpy(&pNewEntry->gemFlowcfg, pUsFlow, sizeof(rt_gpon_usFlow_t));

    list_add_tail(&pNewEntry->list, &gRtkPlatformDb.usGemFlowHead);

    return RT_ERR_OK;

}

static int32
gemFlow_entry_del(uint32 usFlowId)
{
    struct list_head *pEntry    = NULL;
    struct list_head *pTmpEntry = NULL;
    gem_flow_info_t  *pEntryData= NULL;

    list_for_each_safe(pEntry, pTmpEntry, &gRtkPlatformDb.usGemFlowHead)
    {
        pEntryData = list_entry(pEntry, gem_flow_info_t, list);

        if (pEntryData->flowId== usFlowId)
        {
            list_del(&pEntryData->list);

            kfree(pEntryData);

            return RT_ERR_OK;
        }
    }

    return RT_ERR_OK;
}


static void 
gemFlow_entry_dump(void)
{
	struct list_head *pEntry     = NULL;
	gem_flow_info_t	 *pEntryData = NULL;

    printk("########### US GEM flow table        ###########\n");
    printk(" %6s | %9s | %7s | %9s | %5s\n",
        "flowId", "gemPortId", "tcontId", "tcQueueId", "aesEn");
	list_for_each(pEntry, &gRtkPlatformDb.usGemFlowHead)
	{
		pEntryData = list_entry(pEntry, gem_flow_info_t, list);
        printk(" %6u | %9u | %7u | %9u | %5u\n",
            pEntryData->flowId,
            pEntryData->gemFlowcfg.gemPortId,
            pEntryData->gemFlowcfg.tcontId,
            pEntryData->gemFlowcfg.tcQueueId,
            pEntryData->gemFlowcfg.aesEn);
	}

    printk("################################################\n\n");
}


pon_queue_info_t* 
pon_queue_entry_find(uint32 tcontId, uint32 tcQueueId)
{
	struct list_head    *pEntry     = NULL;
	pon_queue_info_t	*pEntryData = NULL;

	list_for_each(pEntry, &gRtkPlatformDb.pTcontInfo[tcontId].tcontQueueHead)
	{
		pEntryData = list_entry(pEntry, pon_queue_info_t, list);

    	if (pEntryData->tcQueueId == tcQueueId)
			return pEntryData;
	}

	return NULL;
}

static int
pon_queue_entry_del(uint32 tcontId, uint32 tcQueueId)
{
    struct list_head *pEntry        = NULL;
    struct list_head *pTmpEntry     = NULL;
    pon_queue_info_t *pEntryData    = NULL;

    list_for_each_safe(pEntry, pTmpEntry, &gRtkPlatformDb.pTcontInfo[tcontId].tcontQueueHead)
    {
        pEntryData = list_entry(pEntry, pon_queue_info_t, list);

        if (pEntryData->tcQueueId == tcQueueId)
        {
            list_del(&pEntryData->list);

            kfree(pEntryData);

            return RT_ERR_OK;
        }
    }

    return RT_ERR_OK;
}


static bool 
pon_queue_entry_need_sorting(uint32 tcontId, uint32 tcQueueId)
{
	struct list_head    *pEntry     = NULL;
	pon_queue_info_t	*pEntryData = NULL;

    if (list_empty(&gRtkPlatformDb.pTcontInfo[tcontId].tcontQueueHead))
        return FALSE;

	list_for_each(pEntry, &gRtkPlatformDb.pTcontInfo[tcontId].tcontQueueHead)
	{
		pEntryData = list_entry(pEntry, pon_queue_info_t, list);

    	if (pEntryData->tcQueueId > tcQueueId)
			return TRUE;
	}

	return FALSE;
}


static void 
pon_queue_entry_dump(uint32 tcontId)
{
	struct list_head    *pEntry     = NULL;
	pon_queue_info_t	*pEntryData = NULL;

    printk("\n######  T-CONT:%d %d %d pon queue table  #######\n",
        tcontId, 
        gRtkPlatformDb.pTcontInfo[tcontId].phyTcontId,
        gRtkPlatformDb.pTcontInfo[tcontId].allocId);
    
    printk(" %10s | %10s | %10s  \n",
        "tcQueueId", "phyQueueId", "flowId");
    
	list_for_each(pEntry, &gRtkPlatformDb.pTcontInfo[tcontId].tcontQueueHead)
	{
		pEntryData = list_entry(pEntry, pon_queue_info_t, list);
            printk(" %10u | %10u | %10u \n",
                pEntryData->tcQueueId,
                pEntryData->phyQueueId,
                pEntryData->flowId);

	}
    printk("################################################\n");
}


static void rt_rtk_gpon_db_dump(void)
{
    printk("maxTcont:%d \r\n",gRtkPlatformDb.maxTcont);    
    printk("maxTcontQueue:%d \r\n",gRtkPlatformDb.maxTcontQueue); 
    printk("maxPonQueue:%d \r\n",gRtkPlatformDb.maxPonQueue); 
    printk("maxFlow:%d \r\n",gRtkPlatformDb.maxFlow); 
    printk("tcontNumPerGroup:%d \r\n",gRtkPlatformDb.tcontNumPerGroup);
}
#endif

/*RTK chip 

RTL9603CVD_CHIP_ID:
T-CONT ID 00 => Queue 00~07
T-CONT ID 01 => Queue 08~15
............
T-CONT ID 15 => Queue 120~126

Other CHIP:
T-CONT ID 00~07 => Queue 00~31
T-CONT ID 08~15 => Queue 32~63
T-CONT ID 16~23 => Queue 64~95
T-CONT ID 24~31 => Queue 96~127
*/
static int phy_queue_get_empty(uint32 phyTcontId,uint32 *PhyQueueId)
{
    uint32 queueId,queueIdMin,queueIdMax;

    *PhyQueueId = RT_GPON_INVALID_PHY_QUEUE;

    queueIdMin = phyTcontId / gRtkPlatformDb.tcontNumPerGroup * gRtkPlatformDb.maxTcontQueue;
    queueIdMax = phyTcontId / gRtkPlatformDb.tcontNumPerGroup * gRtkPlatformDb.maxTcontQueue + gRtkPlatformDb.maxTcontQueue;

    if(queueIdMax > gRtkPlatformDb.maxPonQueue)
        queueIdMax = gRtkPlatformDb.maxPonQueue;
    
    for(queueId=queueIdMin; queueId<queueIdMax; queueId++)
    {
        if(FALSE == gPhyQueueDb[queueId].valid)
        {
            *PhyQueueId = queueId%gRtkPlatformDb.maxTcontQueue;
            return RT_ERR_OK;
        }

    }
    return RT_ERR_FAILED;
}

static int phy_queue_db_set(uint32 phyTcontId, uint32 phyQueueId,uint8 enable)
{
    uint32 queueIdx = 0;

    queueIdx = phyTcontId / gRtkPlatformDb.tcontNumPerGroup * gRtkPlatformDb.maxTcontQueue+phyQueueId;
    
    if(queueIdx>=gRtkPlatformDb.maxPonQueue)
    {
        RT_GPON_LOG_ERR("[%s] %d queueIdx=%d,Must less then %d",
            __FUNCTION__, __LINE__, queueIdx, gRtkPlatformDb.maxPonQueue);
        return RT_ERR_FAILED;
    }
    
    if(enable)
        gPhyQueueDb[queueIdx].valid= TRUE;
    else
        gPhyQueueDb[queueIdx].valid= FALSE;

    return RT_ERR_OK;
}

static int
_CheckIsAvaiableQueue(uint32 phyTcontId,uint32 reqNum)    
{
    uint32 queueId,queueIdMin,queueIdMax,numOfEmptyQueue = 0;
    
    queueIdMin = phyTcontId / gRtkPlatformDb.tcontNumPerGroup * gRtkPlatformDb.maxTcontQueue;
    queueIdMax = phyTcontId / gRtkPlatformDb.tcontNumPerGroup * gRtkPlatformDb.maxTcontQueue + gRtkPlatformDb.maxTcontQueue;

    if(queueIdMax > gRtkPlatformDb.maxPonQueue)
        queueIdMax = gRtkPlatformDb.maxPonQueue;
    
    for(queueId=queueIdMin; queueId<queueIdMax; queueId++)
    {
        if(FALSE == gPhyQueueDb[queueId].valid)
        {
            numOfEmptyQueue++;
            if(reqNum == numOfEmptyQueue)
            {
                RT_GPON_LOG_DBG("phyTcontId:%d reqNum:%d queueId:%d",phyTcontId,reqNum,queueId);
                return RT_ERR_OK;
            }

        }

    }
    return RT_ERR_FAILED;
}
static int 
phy_tcont_update(uint32 *pPhyTcontId, uint32 allocId, uint32 numOfReqQueue)
{

    int ret = RT_ERR_FAILED;
    rtk_gpon_tcont_ind_t    ind;
    rtk_gpon_tcont_attr_t   attr;
    uint32 phyTContId,oriPhyTcontId;
#if (CONFIG_GPON_VERSION > 1)

    /* parameter check */
    RT_PARAM_CHK((NULL == pPhyTcontId), RT_ERR_NULL_POINTER);

    /*Only consider mutiple T-CONT per group*/
    if(gRtkPlatformDb.tcontNumPerGroup <= 1)
        return ret;

    memset(&ind, 0, sizeof(rtk_gpon_tcont_ind_t));
    memset(&attr, 0, sizeof(rtk_gpon_tcont_attr_t));

    ind.alloc_id = allocId;
    oriPhyTcontId = *pPhyTcontId;

    for (phyTContId = 0; phyTContId < gRtkPlatformDb.maxTcont; phyTContId++)
    {
        if ((phyTContId / 8) == (oriPhyTcontId / 8))
            continue;

        if(TRUE ==gPhyTcontDb[phyTContId].valid)
            continue;

        if (RT_ERR_OK != _CheckIsAvaiableQueue(phyTContId, numOfReqQueue))
            continue;


        attr.tcont_id = phyTContId;
        ind.type = RTK_GPON_TCONT_TYPE_1;

        // destory old tcont and re-create with new id
        rtk_gponapp_tcont_destroy(&ind);
        ret = rtk_gponapp_tcont_create(&ind, &attr);

        gPhyTcontDb[oriPhyTcontId].valid = FALSE;
        gPhyTcontDb[phyTContId].valid = TRUE;

        *pPhyTcontId = phyTContId;

        break;
    }

#endif

    return ret;

}


static int _AssignNonUsedTcontId(uint32 allocId, uint32 *plogicTcontId, uint32 *pPhyTcontId)
{

    uint32  tcontId=0;
    uint32  phyTcontId=0; /*physical T-CONT ID*/
    uint8   bAvailPhyTcontId = FALSE;

    
    /*T-CONT allocId exist*/
    for(tcontId = 0;tcontId<gRtkPlatformDb.maxTcont;tcontId++)
    {
        if(gRtkPlatformDb.pTcontInfo[tcontId].allocId == allocId)
        {
            *pPhyTcontId = gRtkPlatformDb.pTcontInfo[tcontId].phyTcontId;
            *plogicTcontId = tcontId;
            return RT_ERR_ENTRY_EXIST;
        }

    }

    /*Set new T-CONT ID to driver*/
    for(phyTcontId = 0;phyTcontId<gRtkPlatformDb.maxTcont;phyTcontId++)
    {
        if(FALSE == gPhyTcontDb[phyTcontId].valid)
        {
            bAvailPhyTcontId = TRUE;
            break;
        }

    }

    if(FALSE == bAvailPhyTcontId)
        return RT_ERR_FAILED;


    /*Find new T-CONT ID */
    for(tcontId = 0;tcontId<gRtkPlatformDb.maxTcont;tcontId++)
    {
        if(RT_GPON_INVALID_ALLOCID_VAL == gRtkPlatformDb.pTcontInfo[tcontId].allocId)
        {
            *pPhyTcontId = phyTcontId;
            *plogicTcontId = tcontId;
            gPhyTcontDb[phyTcontId].valid = TRUE;
            gRtkPlatformDb.pTcontInfo[tcontId].phyTcontId = phyTcontId;
            gRtkPlatformDb.pTcontInfo[tcontId].allocId = allocId;
            return RT_ERR_OK;
        }

    }

    return RT_ERR_FAILED;

}

#if (CONFIG_GPON_VERSION > 1)
static void rt_rtk_gpon_AllocateTcont(rtk_gpon_ploam_t *ploam)
{
    int ret;
    int alloc;
    uint32 logicTcontId;
    rtk_gpon_tcont_ind_t ind;
    rtk_gpon_tcont_attr_t attr;

    alloc = (ploam->msg[0]<<4)|(ploam->msg[1]>>4);

    if(alloc==ploam->onuid)
        return;

    memset(&ind,0,sizeof(rtk_gpon_tcont_ind_t));
    memset(&attr,0,sizeof(rtk_gpon_tcont_attr_t));

    ind.alloc_id = alloc;
    ind.type  =RTK_GPON_TCONT_TYPE_1;

    ret = rtk_gponapp_tcont_get(&ind,&attr);

    /* de-allocate this alloc-id */
    if(ploam->msg[2]==0xFF && ret == RT_ERR_OK)
    {
#if defined(CONFIG_SDK_KERNEL_LINUX)    
        tcont_db_remove(attr.tcont_id);
#endif
        rtk_gponapp_tcont_destroy(&ind);
        RT_GPON_LOG_DBG("[%s] %d  de-allocate allocId:%d",
            __FUNCTION__, __LINE__, ind.alloc_id);
    }
    /* allocate this GEM alloc-id */
    else if(ploam->msg[2]==1 /*&& ret !=RT_ERR_OK*/)
    {
       ret = _AssignNonUsedTcontId(alloc, &logicTcontId, &attr.tcont_id);
       if(RT_ERR_OK == ret || RT_ERR_ENTRY_EXIST == ret)
       {
            ret = rtk_gponapp_tcont_create(&ind,&attr);
       }
        RT_GPON_LOG_DBG("[%s] %d  allocate allocId:%d ret:%d",
            __FUNCTION__, __LINE__, ind.alloc_id, ret);
    }
    return;
}


static int rt_rtk_gpon_ploam_callback(rtk_gpon_ploam_t *ploam)
{
    int8    onuId;

    if (RT_ERR_OK != rtk_gpon_onuId_get(&onuId))
        return GPON_PLOAM_CONTINUE;

    if (ploam->onuid != onuId)
        return GPON_PLOAM_CONTINUE;

    if (ploam->type == GPON_PLOAM_DS_ASSIGNEDALLOCID)
    {
        rt_rtk_gpon_AllocateTcont(ploam);
        return GPON_PLOAM_STOP_WITH_ACK;
    }

    if (GPON_PLOAM_DS_CONFIGPORT == ploam->type)
    {
        if (ploam->msg[0] & 0x1)
        {
            memset(gRtkPlatformDb.pGemEncrypt, 0, sizeof(unsigned char) * 512);
        }
    }

    if (GPON_PLOAM_DS_ENCRYPTPORT == ploam->type)
    {
        if (ploam->msg[0] & 0x2)
        {
            uint8   bEncrypted;
            uint16  portId;
            uint16  shiftByte;
            uint8   shiftBit;

            bEncrypted = (ploam->msg[0] & 0x1);
            portId = (ploam->msg[1]<<4) | (ploam->msg[2]>>4);
            shiftByte = portId / 8;
            shiftBit = portId % 8;

            if (bEncrypted)
                gRtkPlatformDb.pGemEncrypt[shiftByte] |= (1 << shiftBit);
            else
                gRtkPlatformDb.pGemEncrypt[shiftByte] &= ~(1 << shiftBit);
        }
    }

    return GPON_PLOAM_CONTINUE;
}
#endif




static int32
rt_rtk_gpon_omcc_create(void)
{
    int ret;
    rtk_ponmac_queue_t queue;
    rtk_ponmac_queueCfg_t queueCfg;

    /*queue assign*/
    queue.schedulerId = gRtkPlatformDb.omccTcont;
    queue.queueId = gRtkPlatformDb.omccQueue;

    /*queue configure assign*/
    memset(&queueCfg,0,sizeof(rtk_ponmac_queueCfg_t));
    queueCfg.cir = 0;
    queueCfg.pir = gRtkPlatformDb.maxQueueRate;
    queueCfg.type = STRICT_PRIORITY;

    /*add omcc tcont & queue*/
    if((ret = rtk_ponmac_queue_add(&queue,&queueCfg))!=RT_ERR_OK)
    {
        osal_printf("rtk_ponmac_queue_add err: 0x%x\n\r",ret);
        return ret;
    }

    /*assign strem id to tcont & queue*/
    if((ret = rtk_ponmac_flow2Queue_set(gRtkPlatformDb.omccFlow, &queue))!=RT_ERR_OK)
    {
        osal_printf("rtk_ponmac_flow2Queue_set err: 0x%x\n\r",ret);
        return ret;
    }
    return RT_ERR_OK;
}


static int32
rt_rtk_gpon_init(void)
{
#if defined(CONFIG_SDK_KERNEL_LINUX)
    int32   tcontId;
#endif
    int32   ret;
    rtk_switch_devInfo_t devInfo;
    rt_register_capacity_t      *pCapacityInfo;
    rtk_gpon_schedule_info_t    scheInfo;
#ifdef CONFIG_SDK_MODULES
    uint8 isRegistered = TRUE;
#endif

#if (CONFIG_GPON_VERSION > 1)
    ret = rtk_gponapp_driver_initialize();
    if(ret != RT_ERR_OK)
    {
        osal_printf("rtk_gponapp_driver_initialize err: 0x%x\n\r",ret);
        return ret;
    }

    ret = rtk_gponapp_device_initialize();
    if(ret != RT_ERR_OK)
    {
        osal_printf("rtk_gponapp_device_initialize err: 0x%x\n\r",ret);
        return ret;
    }

    ret = rtk_gpon_scheInfo_get(&scheInfo);
    if (ret != RT_ERR_OK)
    {
        osal_printf("rtk_gpon_scheInfo_get err: 0x%x\n\r",ret);
        return ret;
    }
    gRtkPlatformDb.omccTcont = scheInfo.omcc_tcont;
    gRtkPlatformDb.omccQueue = scheInfo.omcc_queue;
    gRtkPlatformDb.omccFlow = scheInfo.omcc_flow;

    ret = rtk_switch_deviceInfo_get(&devInfo);
    if(ret != RT_ERR_OK)
    {
        osal_printf("rtk_switch_deviceInfo_get err: 0x%x\n\r",ret);
        return ret;
    }

    pCapacityInfo = &devInfo.capacityInfo;
    gRtkPlatformDb.maxTcont = pCapacityInfo->gpon_tcont_max-1;//Last T-CONT for OMCC use
    gRtkPlatformDb.maxFlow = pCapacityInfo->classify_sid_max-1;//Last flow for OMCC use
    gRtkPlatformDb.maxPonQueue = pCapacityInfo->max_num_of_pon_queue-1;//Last T-CONT queue for OMCC use
    gRtkPlatformDb.maxTcontQueue = pCapacityInfo->ponmac_tcont_queue_max;
    gRtkPlatformDb.maxQueueRate = pCapacityInfo->ponmac_pir_cir_rate_max;

    if(RTL9603CVD_CHIP_ID == devInfo.chipId)
        gRtkPlatformDb.tcontNumPerGroup = 1;
    else
        gRtkPlatformDb.tcontNumPerGroup = 8;
	
    ret = rt_rtk_gpon_omcc_create();
    if(ret != RT_ERR_OK)
    {
        osal_printf("rt_rtk_gpon_omcc_create err: 0x%x\n\r",ret);
        return ret;
    }


    ret = rtk_gponapp_evtHdlPloam_reg(rt_rtk_gpon_ploam_callback);
    if(ret != RT_ERR_OK)
    {
        RT_GPON_LOG_ERR("rtk_gponapp_evtHdlPloam_reg err: 0x%x",ret);
        return ret;
    }

#if defined(CONFIG_SDK_KERNEL_LINUX)
    gRtkPlatformDb.pGemEncrypt = kzalloc(sizeof(unsigned char) * 512, GFP_ATOMIC);
    if (!gRtkPlatformDb.pGemEncrypt){
        RT_GPON_LOG_ERR("gemEncrypt kzalloc fail");
        return RT_ERR_FAILED;
    }

    gRtkPlatformDb.pTcontInfo = kzalloc(sizeof(rt_tcont_info_t) * gRtkPlatformDb.maxTcont, GFP_ATOMIC);
    if (!gRtkPlatformDb.pTcontInfo){
        RT_GPON_LOG_ERR("pTcontInfo kzalloc fail");
        return RT_ERR_FAILED;
    }


    gPhyQueueDb = kzalloc(sizeof(rt_rtk_phyQueue_t) * gRtkPlatformDb.maxPonQueue, GFP_ATOMIC);
    if (!gPhyQueueDb){
        RT_GPON_LOG_ERR("gPhyQueueDb kzalloc fail");
        return RT_ERR_FAILED;
    }

    gPhyTcontDb = kzalloc(sizeof(rt_rtk_phyTcont_t) * gRtkPlatformDb.maxTcont, GFP_ATOMIC);
    if (!gPhyTcontDb){
        RT_GPON_LOG_ERR("gPhyTcontDb kzalloc fail");
        return RT_ERR_FAILED;
    }

    for(tcontId = 0 ;tcontId<gRtkPlatformDb.maxTcont;tcontId++)
    {
        INIT_LIST_HEAD(&gRtkPlatformDb.pTcontInfo[tcontId].tcontQueueHead);
        gRtkPlatformDb.pTcontInfo[tcontId].allocId = RT_GPON_INVALID_ALLOCID_VAL;
    }
    INIT_LIST_HEAD(&gRtkPlatformDb.usGemFlowHead);
#endif



#endif

#if defined(CONFIG_SDK_KERNEL_LINUX)
    if(NULL == rt_api_proc_dir) 
	{
    	rt_api_proc_dir = proc_mkdir ("rt_api", NULL);
   	}
	
    if (NULL == rt_gpon_proc_dir)
	{
        rt_gpon_proc_dir = proc_mkdir ("rt_gpon", rt_api_proc_dir);
    }

    rt_gpon_info = proc_create("info", 0, rt_gpon_proc_dir, &rt_gpon_info_fop);
    if (!rt_gpon_info)
	{
        RT_GPON_LOG_ERR("rt gpon info , create proc failed!");
    }

    rt_gpon_logLvl = proc_create("loglvl", 0, rt_gpon_proc_dir, &rt_gpon_log_fop);
    if (!rt_gpon_logLvl)
	{
        RT_GPON_LOG_ERR("rt gpon log, create proc failed!");
    }
#endif

#ifdef CONFIG_SDK_MODULES
    if(0 != pkt_redirect_kernelApp_sendPkt(PR_KERNEL_UID_BCASTER,
        0, sizeof(uint8), &isRegistered))
    {
        RT_GPON_LOG_ERR("send registered command failed !! \n");
    }
#endif

    return RT_ERR_OK;
}

static int32
rt_rtk_gpon_serialNumber_set(rt_gpon_serialNumber_t *pSN)
{
    int32   ret;
    rtk_gpon_serialNumber_t gponSN;

#if (CONFIG_GPON_VERSION > 1)
    if(NULL == pSN)
    {
        return RT_ERR_INPUT;
    }

    osal_memcpy(&gponSN, pSN, sizeof(rtk_gpon_serialNumber_t));
    ret = rtk_gponapp_serialNumber_set(&gponSN);
#endif

    return ret;
}

static int32
rt_rtk_gpon_serialNumber_get(rt_gpon_serialNumber_t *pSN)
{
    int32   ret;
    rtk_gpon_serialNumber_t gponSN;

#if (CONFIG_GPON_VERSION > 1)
    if(NULL == pSN)
    {
        return RT_ERR_INPUT;
    }

    ret = rtk_gponapp_serialNumber_get(&gponSN);
    osal_memcpy(pSN, &gponSN, sizeof(rtk_gpon_serialNumber_t));
#endif

    return ret;
}

static int32
rt_rtk_gpon_registrationId_set(rt_gpon_registrationId_t *pRegId)
{
    int32   ret;
    rtk_gpon_password_t gponPasswd;

#if (CONFIG_GPON_VERSION > 1)
    if(NULL == pRegId)
    {
        return RT_ERR_INPUT;
    }

    osal_memcpy(&gponPasswd, pRegId, sizeof(rtk_gpon_password_t));
    ret = rtk_gponapp_password_set(&gponPasswd);
#endif

    return ret;
}

static int32
rt_rtk_gpon_registrationId_get(rt_gpon_registrationId_t *pRegId)
{
    int32   ret;
    rtk_gpon_password_t gponPasswd;

#if (CONFIG_GPON_VERSION > 1)
    if(NULL == pRegId)
    {
        return RT_ERR_INPUT;
    }

    ret = rtk_gponapp_password_get(&gponPasswd);
    osal_memcpy(pRegId, &gponPasswd, sizeof(rtk_gpon_password_t));
#endif

    return ret;
}

static int32
rt_rtk_gpon_activate(rt_gpon_initialState_t initState)
{
    int32   ret;

#if (CONFIG_GPON_VERSION > 1)
    ret = rtk_gponapp_activate(initState);
#endif

    return ret;
}

static int32
rt_rtk_gpon_deactivate(void)
{
    int32   ret;

#if (CONFIG_GPON_VERSION > 1)
    ret = rtk_gponapp_deActivate();
#endif

    return ret;
}

static int32
rt_rtk_gpon_onuState_get(rt_gpon_onuState_t *pOnuState)
{
    int32   ret;

#if (CONFIG_GPON_VERSION > 1)
    rtk_gpon_fsm_status_t onuState;
    ret = rtk_gponapp_ponStatus_get(&onuState);

    if(ret == RT_ERR_OK)
    {
        switch(onuState)
        {
            case GPON_STATE_O1: //Initial state(O1)
                *pOnuState = RT_GPON_ONU_STATES_O1;
                break;
            case GPON_STATE_O2: //standby state(O2)
                *pOnuState = RT_GPON_ONU_STATES_O2;
                break;
            case GPON_STATE_O3: //Serail number state(O3)
                *pOnuState = RT_GPON_ONU_STATES_O3;
                break;
            case GPON_STATE_O4: //Ranging state(O4)
                *pOnuState = RT_GPON_ONU_STATES_O4;
                break;
            case GPON_STATE_O5: //operation state(O5)
                *pOnuState = RT_GPON_ONU_STATES_O5;
                break;
            case GPON_STATE_O6: //Intermittent LODS state(O6)
                *pOnuState = RT_GPON_ONU_STATES_O6;
                break;
            case GPON_STATE_O7: //Emergency stop state(O7)
                *pOnuState = RT_GPON_ONU_STATES_O7;
                break;
            default:
                osal_printf("%s: unknow onu state %d \n\r", __FUNCTION__, onuState);
                return RT_ERR_FAILED;
        }
    }
#endif

    return ret;
}

static int32
rt_rtk_gpon_omci_tx(uint32 msgLen, uint8 *pMsg)
{
    int32   ret;
#if (CONFIG_GPON_VERSION > 1)
    rtk_gpon_omci_msg_t omci;


    if(NULL == pMsg || msgLen > RTK_GPON_OMCI_MSG_LEN_TX)
    {
        return RT_ERR_INPUT;
    }
	
    osal_memcpy(omci.msg, pMsg, msgLen);
    omci.len = msgLen;
    ret = rtk_gponapp_omci_tx(&omci);
#endif

    return ret;
}

static rt_gpon_omci_rx_callback rtGponOmciRx = NULL;
#if (CONFIG_GPON_VERSION > 1)
static void rt_rtk_gpon_omci_rx(rtk_gpon_omci_msg_t* omci)
{
    if(NULL == omci)
        return;

    if(rtGponOmciRx)
    {
        (*rtGponOmciRx)(omci->len, omci->msg);
    }
    else
        osal_printf("%s: omci RX callback not registered. \n\r", __FUNCTION__);

    return;
}
#endif

static int32
rt_rtk_gpon_tcont_set(uint32 *pTcontId, uint32 allocId)
{
    int32 ret = RT_ERR_OK;
#if (CONFIG_GPON_VERSION > 1) 
    rtk_gpon_tcont_ind_t ind;
    rtk_gpon_tcont_attr_t attr;
 
    /* parameter check */
    RT_PARAM_CHK((NULL == pTcontId), RT_ERR_NULL_POINTER);

    memset(&ind,0,sizeof(rtk_gpon_tcont_ind_t));
    memset(&attr,0,sizeof(rtk_gpon_tcont_attr_t));

    ind.alloc_id = allocId;
    ind.type  =RTK_GPON_TCONT_TYPE_1;

    ret = _AssignNonUsedTcontId(allocId, pTcontId, &attr.tcont_id);

    if(RT_ERR_ENTRY_EXIST == ret)
    {
        return RT_ERR_OK;
    }   
    else if(RT_ERR_OK == ret)
    {
		int32 orig_tcont_id = attr.tcont_id;
		ret = rtk_gponapp_tcont_get_physical(&ind,&attr);
		if(RT_ERR_OK == ret)
		{
			if(attr.tcont_id != orig_tcont_id)
            {
                RT_PARAM_CHK((*pTcontId >= gRtkPlatformDb.maxTcont), RT_ERR_INPUT);
                gRtkPlatformDb.pTcontInfo[*pTcontId].phyTcontId = attr.tcont_id; 
                gPhyTcontDb[orig_tcont_id].valid = FALSE;
                gPhyTcontDb[attr.tcont_id].valid = TRUE;
                RT_GPON_LOG_DBG("[%s] %d  allocate allocId:%d tconid:%d orig_tcont_id:%d",
                    __FUNCTION__, __LINE__, ind.alloc_id, attr.tcont_id, orig_tcont_id);
            }
		}
        ret = rtk_gponapp_tcont_create(&ind,&attr);
    }
    RT_GPON_LOG_DBG("[%s] %d  allocate allocId:%d",
        __FUNCTION__, __LINE__, ind.alloc_id);

#endif
    return ret;

}

static int32
rt_rtk_gpon_tcont_get(uint32 tcontId, uint32 *pAllocId)
{

    /* parameter check */
    RT_PARAM_CHK((NULL == pAllocId), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((tcontId >= gRtkPlatformDb.maxTcont), RT_ERR_INPUT);

    *pAllocId = gRtkPlatformDb.pTcontInfo[tcontId].allocId; 

    return RT_ERR_OK;
}

static int32 
rt_rtk_gpon_tcont_del(uint32 tcontId)
{
#if (CONFIG_GPON_VERSION > 1)
    int32 ret;
    rtk_gpon_tcont_ind_t ind;
    uint32  phyTcontId=0; /*physical T-CONT ID*/
    
    /* parameter check */
    RT_PARAM_CHK((tcontId >= gRtkPlatformDb.maxTcont), RT_ERR_INPUT);

    ind.alloc_id = gRtkPlatformDb.pTcontInfo[tcontId].allocId;

    ret = rtk_gponapp_tcont_destroy_logical(&ind);
    if(RT_ERR_OK !=ret)
    {
        RT_GPON_LOG_ERR("[%s] %d tcont_destroy fail ret:%d",
            __FUNCTION__, __LINE__, ret);
        return ret;
    }

    /*reset T-CONT data base*/
    if(RT_GPON_INVALID_ALLOCID_VAL != gRtkPlatformDb.pTcontInfo[tcontId].allocId)
    {
        gRtkPlatformDb.pTcontInfo[tcontId].allocId = RT_GPON_INVALID_ALLOCID_VAL;
        phyTcontId = gRtkPlatformDb.pTcontInfo[tcontId].phyTcontId;

        if(phyTcontId < gRtkPlatformDb.maxTcont)
        {
            /*Remove physical T-CONT ID database*/
            gPhyTcontDb[phyTcontId].valid = FALSE;
        }
    }


#endif
    return RT_ERR_OK;
}

static int32
rt_rtk_gpon_usFlow_set(uint32 usFlowId, rt_gpon_usFlow_t *pUsFlow)
{
#if (CONFIG_GPON_VERSION > 1)
#if defined(CONFIG_SDK_KERNEL_LINUX)
    int32   ret;
    rtk_gpon_usFlow_attr_t usFlow;
    rtk_ponmac_queue_t queue;
    uint32  phyTcontId;

	pon_queue_info_t	*pPonQueueEntry = NULL;
#endif

    /* parameter check */
    RT_PARAM_CHK((NULL == pUsFlow), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((usFlowId >= gRtkPlatformDb.maxFlow), RT_ERR_INPUT);
    RT_PARAM_CHK((pUsFlow->tcontId >= gRtkPlatformDb.maxTcont && pUsFlow->tcontId!= RT_GPON_OMCC_TCONT_ID), RT_ERR_INPUT);
    RT_PARAM_CHK((pUsFlow->tcQueueId >= gRtkPlatformDb.maxTcontQueue), RT_ERR_INPUT);

#if defined(CONFIG_SDK_KERNEL_LINUX)
    phyTcontId = (pUsFlow->tcontId == RT_GPON_OMCC_TCONT_ID)?
        gRtkPlatformDb.omccTcont:LOGIC_TCONID_TO_PHY_TCONID(pUsFlow->tcontId);

    memset(&usFlow, 0, sizeof(rtk_gpon_usFlow_attr_t));
    
    usFlow.type         = RTK_GPON_FLOW_TYPE_ETH;
    usFlow.gem_port_id  = pUsFlow->gemPortId;
    usFlow.tcont_id     = phyTcontId; //get physical T-CONT ID
    ret = rtk_gponapp_usFlow_set(usFlowId, &usFlow);
    if(RT_ERR_OK !=ret)
    {
        RT_GPON_LOG_ERR("[%s] %d rtk_gponapp_usFlow_set fail ret:%d",
            __FUNCTION__, __LINE__, ret);
        return ret;
    }

    if(RT_GPON_OMCC_TCONT_ID == pUsFlow->tcontId)
    {
        queue.queueId = gRtkPlatformDb.omccQueue;
    }
    else
    {
    pPonQueueEntry = pon_queue_entry_find(pUsFlow->tcontId,pUsFlow->tcQueueId);

    if(NULL == pPonQueueEntry)
    {
        RT_GPON_LOG_ERR("[%s] %d tcQueueId is not created",__FUNCTION__, __LINE__);    
        return RT_ERR_FAILED;
    }
    
    queue.queueId = pPonQueueEntry->phyQueueId;
    }


    if(RT_GPON_INVALID_PHY_QUEUE == queue.queueId)
     {
        RT_GPON_LOG_ERR("[%s] %d invalid queue",__FUNCTION__, __LINE__);
        return ret;
    }   
    queue.schedulerId = phyTcontId;
    
    ret = rtk_ponmac_flow2Queue_set(usFlowId,&queue);
    if(RT_ERR_OK !=ret)
    {
        RT_GPON_LOG_ERR("[%s] %d rtk_ponmac_flow2Queue_set fail ret:%d",
            __FUNCTION__, __LINE__, ret);
        return ret;
    }
    
    if(pUsFlow->tcontId != RT_GPON_OMCC_TCONT_ID)
    {
    pPonQueueEntry->flowId = usFlowId;
    }    

    ret = gemFlow_entry_set(usFlowId, pUsFlow);
    if(RT_ERR_OK !=ret)
    {
        RT_GPON_LOG_ERR("[%s] %d gemFlow_entry_set fail",
            __FUNCTION__, __LINE__);
        return ret;
    }
#endif
#endif
    return RT_ERR_OK;

}

static int32 
rt_rtk_gpon_usFlow_get(uint32 usFlowId, rt_gpon_usFlow_t *pUsFlow)
{
#if defined(CONFIG_SDK_KERNEL_LINUX)
	gem_flow_info_t	*pEntryData = NULL;
#endif

    /* parameter check */
    RT_PARAM_CHK((NULL == pUsFlow), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((usFlowId >= gRtkPlatformDb.maxFlow), RT_ERR_INPUT);

#if defined(CONFIG_SDK_KERNEL_LINUX)
    if((pEntryData = gemFlow_entry_find(usFlowId)))
    {
        osal_memcpy(pUsFlow, &pEntryData->gemFlowcfg, sizeof(rt_gpon_usFlow_t));
        return RT_ERR_OK;
       
    }
#endif

    return RT_ERR_ENTRY_NOTFOUND;

}


static int32 
rt_rtk_gpon_usFlow_del(uint32 usFlowId)
{

    int32   ret = RT_ERR_OK;

#if (CONFIG_GPON_VERSION > 1)
    rtk_gpon_usFlow_attr_t usFlow;

    /* parameter check */    
    RT_PARAM_CHK((usFlowId >= gRtkPlatformDb.maxFlow), RT_ERR_INPUT);

    /* delete SW database*/
#if defined(CONFIG_SDK_KERNEL_LINUX)
    gemFlow_entry_del(usFlowId);
#endif

    memset(&usFlow, 0, sizeof(rtk_gpon_usFlow_attr_t));
    usFlow.gem_port_id  = RTK_GPON_GEMPORT_ID_NOUSE;
    ret = rtk_gponapp_usFlow_set(usFlowId, &usFlow);
#endif
    return ret;
}

static int32 
rt_rtk_gpon_usFlow_delAll(void)
{
#if (CONFIG_GPON_VERSION > 1)
#if defined(CONFIG_SDK_KERNEL_LINUX)
    int32   ret;
    struct list_head        *pEntry;
    struct list_head        *pTmpEntry;
    gem_flow_info_t     *pEntryData;
    rtk_gpon_usFlow_attr_t usFlow;

    list_for_each_safe(pEntry, pTmpEntry, &gRtkPlatformDb.usGemFlowHead)
    {
        pEntryData = list_entry(pEntry, gem_flow_info_t, list);

        memset(&usFlow, 0, sizeof(rtk_gpon_usFlow_attr_t));
        usFlow.gem_port_id  = RTK_GPON_GEMPORT_ID_NOUSE;
        ret = rtk_gponapp_usFlow_set(pEntryData->flowId, &usFlow);

        if(RT_ERR_OK != ret)
        {
            RT_GPON_LOG_ERR("[%s] %d del us flow fail ret:%d",
                __FUNCTION__, __LINE__, ret);
           return ret; 
        }
        list_del(&pEntryData->list);
        kfree(pEntryData);
    }
#endif	
#endif
    return RT_ERR_OK;
}


static int32 
rt_rtk_gpon_dsFlow_set(uint32 dsFlowId, rt_gpon_dsFlow_t *pDsFlow)
{
    int32   ret = RT_ERR_OK;
#if (CONFIG_GPON_VERSION > 1)

    rtk_gpon_dsFlow_attr_t dsFlow;
    uint16  shiftByte;
    uint8   shiftBit;
    
    /* parameter check */
    RT_PARAM_CHK((NULL == pDsFlow), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((dsFlowId >= gRtkPlatformDb.maxFlow), RT_ERR_INPUT);

    shiftByte = pDsFlow->gemPortId / 8;
    shiftBit  = pDsFlow->gemPortId % 8;

    dsFlow.gem_port_id = pDsFlow->gemPortId;
    dsFlow.type        = RTK_GPON_FLOW_TYPE_ETH;
    dsFlow.multicast   = DISABLED;
    dsFlow.aes_en      = (gRtkPlatformDb.pGemEncrypt[shiftByte] & (1 << shiftBit)) ? ENABLED : DISABLED;
    ret = rtk_gponapp_dsFlow_set(dsFlowId, &dsFlow);
#endif
    return ret;
}


static int32 
rt_rtk_gpon_dsFlow_get(uint32 dsFlowId, rt_gpon_dsFlow_t *pDsFlow)
{
#if (CONFIG_GPON_VERSION > 1)
    int32   ret;
    rtk_gpon_dsFlow_attr_t dsFlow;

    /* parameter check */
    RT_PARAM_CHK((NULL == pDsFlow), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((dsFlowId >= gRtkPlatformDb.maxFlow), RT_ERR_INPUT);
    
    ret = rtk_gponapp_dsFlow_get(dsFlowId, &dsFlow);

    if(RT_ERR_OK != ret)
    {
        return RT_ERR_ENTRY_NOTFOUND;
    }

    pDsFlow->gemPortId = dsFlow.gem_port_id;
    pDsFlow->aesEn    =  dsFlow.aes_en;
#endif
    return RT_ERR_OK;
}


static int32 
rt_rtk_gpon_dsFlow_del(uint32 dsFlowId)
{
#if (CONFIG_GPON_VERSION > 1)
    int32   ret;
    rtk_gpon_dsFlow_attr_t dsFlow;

    /* parameter check */
    RT_PARAM_CHK((dsFlowId >= gRtkPlatformDb.maxFlow), RT_ERR_INPUT);

    dsFlow.gem_port_id  = RTK_GPON_GEMPORT_ID_NOUSE;
    dsFlow.type         = 0;
    dsFlow.multicast    = DISABLED;
    dsFlow.aes_en       = DISABLED;
    ret = rtk_gponapp_dsFlow_set(dsFlowId, &dsFlow);

    if (ret != RT_ERR_OK)
    {
        RT_GPON_LOG_ERR("[%s] %d del ds flow fail ret:%d",
            __FUNCTION__, __LINE__, ret);

        return ret;
    }
#endif
    return RT_ERR_OK;
}


static int32 
rt_rtk_gpon_dsFlow_delAll(void)
{
#if (CONFIG_GPON_VERSION > 1)
    int32   ret;
    rtk_gpon_dsFlow_attr_t dsFlow;
    uint32 dsFlowId;
 
    for(dsFlowId=0;dsFlowId<gRtkPlatformDb.maxFlow;dsFlowId++)
    {
        if(RT_ERR_OK == rtk_gponapp_dsFlow_get(dsFlowId, &dsFlow))    
        {
            ret = rt_rtk_gpon_dsFlow_del(dsFlowId);

            if(ret!=RT_ERR_OK)
            {
                RT_GPON_LOG_ERR("[%s] %d del ds flow:%d fail ret:%d",
                    __FUNCTION__, __LINE__, dsFlowId, ret);

            }

        }
            
    }
#endif
    return RT_ERR_OK;
}

static int32 
rt_rtk_gpon_ponQueue_set(uint32 tcontId, uint32 tcQueueId, rt_gpon_queueCfg_t *pQueuecfg)
{
#if defined(CONFIG_SDK_KERNEL_LINUX)
    int32   ret;
	pon_queue_info_t    *pCurData = NULL;
	pon_queue_info_t    *pNewData = NULL;
	pon_queue_info_t    *pPrevData = NULL;
    struct list_head    *pEntry     = NULL;

	gem_flow_info_t	    *pGemFlowData = NULL;


    rtk_ponmac_queue_t  queue;
    rtk_ponmac_queueCfg_t queueCfg;
    uint32 phyQueueId, phyTcontId,oriPhyTcontId, allocId,tcontQueueNum;
    uint8 isAddToList=0;
#endif

    /* parameter check */
    RT_PARAM_CHK((NULL == pQueuecfg), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((tcontId >= gRtkPlatformDb.maxTcont), RT_ERR_INPUT);
    RT_PARAM_CHK((tcQueueId >= gRtkPlatformDb.maxTcontQueue), RT_ERR_INPUT);
    RT_PARAM_CHK((pQueuecfg->scheduleType>= RT_GPON_TCONT_QUEUE_SCHEDULER_SP_WRR), RT_ERR_INPUT);

#if defined(CONFIG_SDK_KERNEL_LINUX)
    if(RT_GPON_PON_QUEUE_PIR_UNLIMITED == pQueuecfg->pir || 
        pQueuecfg->pir > gRtkPlatformDb.maxQueueRate)
    {
        pQueuecfg->pir  = gRtkPlatformDb.maxQueueRate;
    }

    phyTcontId = LOGIC_TCONID_TO_PHY_TCONID(tcontId);
    allocId = gRtkPlatformDb.pTcontInfo[tcontId].allocId;

    if((pCurData = pon_queue_entry_find(tcontId,tcQueueId)))
    {
        RT_GPON_LOG_DBG("[%s] pon queue exist, update queue config",__FUNCTION__);
        memcpy(&pCurData->Queuecfg, pQueuecfg, sizeof(rt_gpon_queueCfg_t));
        queue.queueId = pCurData->phyQueueId;
        queue.schedulerId = phyTcontId;

        queueCfg.type       = RT_SCHTYPE_TO_RTK_SCHTYPE(pQueuecfg->scheduleType);
        queueCfg.cir        = pQueuecfg->cir;
        queueCfg.pir        = pQueuecfg->pir;
        queueCfg.egrssDrop  = DISABLED;
        queueCfg.weight     = pQueuecfg->weight;
        if((ret = rtk_ponmac_queue_add(&queue,&queueCfg))!=RT_ERR_OK)
        {
            RT_GPON_LOG_ERR("[%s] %d ret:%d queue add fail queueId:%d schedulerId:%d"
                ,__FUNCTION__, __LINE__, ret, queue.queueId, queue.schedulerId);
        }
        return ret;
       
    }
    else
    {
        /*Check if empty queue*/
        if(RT_ERR_OK == phy_queue_get_empty(phyTcontId, &phyQueueId))
        {
            if(FALSE == pon_queue_entry_need_sorting(tcontId, tcQueueId))//Not need sorting add the tail      
            {

                RT_GPON_LOG_DBG("[%s] Add new queue the tail of T-CONT:%d tcQueueId:%d phyTcontId:%d phyQueueId:%d", 
                    __FUNCTION__, tcontId, tcQueueId, phyTcontId, phyQueueId);
                
                queue.queueId       = phyQueueId;
                queue.schedulerId   = phyTcontId;
                queueCfg.type       = RT_SCHTYPE_TO_RTK_SCHTYPE(pQueuecfg->scheduleType);
                queueCfg.cir        = pQueuecfg->cir;
                queueCfg.pir        = pQueuecfg->pir;
                queueCfg.egrssDrop  = DISABLED;
                queueCfg.weight     = pQueuecfg->weight;
                if((ret = rtk_ponmac_queue_add(&queue,&queueCfg))!=RT_ERR_OK)
                {
                    RT_GPON_LOG_ERR("[%s] %d ret:%d queue add fail queueId:%d schedulerId:%d"
                        ,__FUNCTION__, __LINE__, ret, queue.queueId, queue.schedulerId);

                    return RT_ERR_FAILED;
                }

                phy_queue_db_set(phyTcontId, phyQueueId, TRUE);
                

                // new item
        	    pNewData = kzalloc(sizeof(pon_queue_info_t), GFP_ATOMIC);
            	if (!pNewData)
            		return RT_ERR_FAILED;


                pNewData->phyQueueId  = phyQueueId;
                pNewData->tcQueueId   = tcQueueId;
                pNewData->flowId      = RT_GPON_INVALID_PHY_QUEUE;
                memcpy(&pNewData->Queuecfg, pQueuecfg, sizeof(rt_gpon_queueCfg_t));

        	    list_add_tail(&pNewData->list, &gRtkPlatformDb.pTcontInfo[tcontId].tcontQueueHead);
                return ret;
       
            }
            else
            {
                RT_GPON_LOG_DBG("[%s] Sorting and add new queue the tail of T-CONT:%d phyTcontId:%d phyQueueId:%d", 
                    __FUNCTION__, tcontId, phyTcontId, phyQueueId);
                
                /*Add  new queue to the correct loacation*/

                // new item
        	    pNewData = kzalloc(sizeof(pon_queue_info_t), GFP_ATOMIC);
            	if (!pNewData)
            		return RT_ERR_FAILED;
                
                pNewData->phyQueueId  = RT_GPON_INVALID_PHY_QUEUE;
                pNewData->tcQueueId   = tcQueueId;
                pNewData->flowId      = RT_GPON_INVALID_US_GEM_FLOW;
                memcpy(&pNewData->Queuecfg, pQueuecfg, sizeof(rt_gpon_queueCfg_t));
                isAddToList = 0;
                list_for_each(pEntry, &gRtkPlatformDb.pTcontInfo[tcontId].tcontQueueHead)
            	{
            		pCurData = list_entry(pEntry, pon_queue_info_t, list);

                	if (pCurData->tcQueueId > tcQueueId)
                    {

                        if(NULL == pPrevData)
                            list_add(&pNewData->list, &gRtkPlatformDb.pTcontInfo[tcontId].tcontQueueHead);//Add to healder
                        else
                            __list_add(&pNewData->list, &pPrevData->list, &pCurData->list);
                        isAddToList = 1;
                        break;
                    }
                    pPrevData = pCurData;
            	}
                
                if(isAddToList==0)
                {
                    kfree(pNewData);
                }
                /*Delete orignal queue */
            	list_for_each(pEntry, &gRtkPlatformDb.pTcontInfo[tcontId].tcontQueueHead)
            	{
            		pCurData = list_entry(pEntry, pon_queue_info_t, list);

                	if (pCurData->tcQueueId > tcQueueId)
                    {
                        queue.queueId = pCurData->phyQueueId;
                        queue.schedulerId = phyTcontId;

                        if((ret = rtk_ponmac_queue_del(&queue))!=RT_ERR_OK)
                        {
                            RT_GPON_LOG_ERR("[%s] %d ret:%d queue delete fail queueId:%d schedulerId:%d"
                                ,__FUNCTION__, __LINE__, ret, queue.queueId, queue.schedulerId);
                        }

                        phy_queue_db_set(phyTcontId, pCurData->phyQueueId, FALSE);
                        pCurData->phyQueueId = RT_GPON_INVALID_PHY_QUEUE;
                    }   
            	}

                /*Assign new queue */
            	list_for_each(pEntry, &gRtkPlatformDb.pTcontInfo[tcontId].tcontQueueHead)
            	{
            		pCurData = list_entry(pEntry, pon_queue_info_t, list);

                    /*Re-assign T-CONT queue phyQueueId mapping*/
                	if (pCurData->phyQueueId == RT_GPON_INVALID_PHY_QUEUE)
                    {
                        if(RT_ERR_OK != phy_queue_get_empty(phyTcontId, &phyQueueId))
                        {
                            RT_GPON_LOG_ERR("[%s] %d phy_queue_get_empty, phyTcontId %d, phyQueueId %d"
                                ,__FUNCTION__, __LINE__, phyTcontId, phyQueueId);

                        }
                        queue.queueId       = phyQueueId;
                        queue.schedulerId   = phyTcontId;
                        
                        queueCfg.type       = RT_SCHTYPE_TO_RTK_SCHTYPE(pQueuecfg->scheduleType);
                        queueCfg.cir        = pCurData->Queuecfg.cir;
                        queueCfg.pir        = pCurData->Queuecfg.pir;
                        queueCfg.egrssDrop  = DISABLED;
                        queueCfg.weight     = pCurData->Queuecfg.weight;
                        if((ret = rtk_ponmac_queue_add(&queue,&queueCfg))!=RT_ERR_OK)
                        {
                            RT_GPON_LOG_ERR("[%s] %d ret:%d queue add fail, queueId:%d schedulerId:%d"
                                ,__FUNCTION__, __LINE__, ret, queue.queueId, queue.schedulerId);
                        }
                        phy_queue_db_set(phyTcontId, phyQueueId, TRUE);
                        pCurData->phyQueueId = phyQueueId;

                        /*Update flow to phy queue*/
                        if(pCurData->flowId != RT_GPON_INVALID_US_GEM_FLOW)
                        {
                            queue.queueId = phyQueueId;
                            queue.schedulerId = phyTcontId;
                            ret = rtk_ponmac_flow2Queue_set(pCurData->flowId,&queue);
                            if(RT_ERR_OK !=ret)
                            {
                                RT_GPON_LOG_ERR("rtk_ponmac_flow2Queue_set fail");
                                return ret;
                            }             
                        }
                    }   
            	}
                
            }

        }
        else
        {
            /*Check other T-CONT is available queue*/
            tcontQueueNum = tcont_queue_num_get(tcontId);
            tcontQueueNum++;
            oriPhyTcontId = phyTcontId;
            if(RT_ERR_OK == phy_tcont_update(&phyTcontId, allocId, tcontQueueNum))
            {
                RT_GPON_LOG_DBG("[%s] T-CONT:%d Update phyTcontId from %d to %d", 
                    __FUNCTION__, tcontId, oriPhyTcontId, phyTcontId);
                
                gRtkPlatformDb.pTcontInfo[tcontId].phyTcontId = phyTcontId;

                /*Delete orignal phy queue */

            	list_for_each(pEntry, &gRtkPlatformDb.pTcontInfo[tcontId].tcontQueueHead)
            	{
            		pCurData = list_entry(pEntry, pon_queue_info_t, list);

                    queue.queueId = pCurData->phyQueueId;
                    queue.schedulerId = oriPhyTcontId;

                    if((ret = rtk_ponmac_queue_del(&queue))!=RT_ERR_OK)
                    {
                            RT_GPON_LOG_ERR("[%s] %d ret:%d queue del fail queueId:%d schedulerId:%d"
                                ,__FUNCTION__, __LINE__, ret, queue.queueId, queue.schedulerId);
                    }

                    phy_queue_db_set(oriPhyTcontId, pCurData->phyQueueId, FALSE);
                    pCurData->phyQueueId = RT_GPON_INVALID_PHY_QUEUE; 
            	}


                // new item
        	    pNewData = kzalloc(sizeof(pon_queue_info_t), GFP_ATOMIC);
            	if (!pNewData)
            		return RT_ERR_FAILED;
                
                pNewData->phyQueueId  = RT_GPON_INVALID_PHY_QUEUE;
                pNewData->tcQueueId   = tcQueueId;
                pNewData->flowId      = RT_GPON_INVALID_US_GEM_FLOW;
                memcpy(&pNewData->Queuecfg, pQueuecfg, sizeof(rt_gpon_queueCfg_t));

                if(FALSE == pon_queue_entry_need_sorting(tcontId, tcQueueId))//Not need sorting add the tail      
                {
            	    list_add_tail(&pNewData->list, &gRtkPlatformDb.pTcontInfo[tcontId].tcontQueueHead);
                }
                else
                {
                    isAddToList = 0;
                    /*Add  new queue to the correct loacation*/
                	list_for_each(pEntry, &gRtkPlatformDb.pTcontInfo[tcontId].tcontQueueHead)
                	{
                		pCurData = list_entry(pEntry, pon_queue_info_t, list);

                    	if (pCurData->tcQueueId > tcQueueId)
                        {

                            if(NULL == pPrevData)
                                list_add(&pNewData->list, &gRtkPlatformDb.pTcontInfo[tcontId].tcontQueueHead);//Add to healder
                            else
                                __list_add(&pNewData->list, &pPrevData->list, &pCurData->list);
                            isAddToList = 1;
                            break;
                        }
                        pPrevData = pCurData;

                	}
                    if(isAddToList == 0)
                    {
                        kfree(pNewData);
                    }

                }


            	list_for_each(pEntry, &gRtkPlatformDb.pTcontInfo[tcontId].tcontQueueHead)
            	{
            		pCurData = list_entry(pEntry, pon_queue_info_t, list);

                    /*Re-assign T-CONT queue phyQueueId mapping*/
                	if (RT_GPON_INVALID_PHY_QUEUE == pCurData->phyQueueId)
                    {
                        if(RT_ERR_OK != phy_queue_get_empty(phyTcontId, &phyQueueId))
                        {
                            RT_GPON_LOG_ERR("[%s] %d phy_queue_get_empty, phyTcontId %d, phyQueueId %d"
                                ,__FUNCTION__, __LINE__, phyTcontId, phyQueueId);
                        }
                        queue.queueId       = phyQueueId;
                        queue.schedulerId   = phyTcontId;
                        
                        queueCfg.type       = RT_SCHTYPE_TO_RTK_SCHTYPE(pQueuecfg->scheduleType);
                        queueCfg.cir        = pCurData->Queuecfg.cir;
                        queueCfg.pir        = pCurData->Queuecfg.pir;
                        queueCfg.egrssDrop  = DISABLED;
                        queueCfg.weight     = pCurData->Queuecfg.weight;
                        if((ret = rtk_ponmac_queue_add(&queue,&queueCfg))!=RT_ERR_OK)
                        {
                            RT_GPON_LOG_ERR("[%s] %d queue add fail ret:%d queueId:%d schedulerId:%d"
                                ,__FUNCTION__, __LINE__, ret, queue.queueId, queue.schedulerId);
                        }
                        phy_queue_db_set(phyTcontId, phyQueueId, TRUE);
                        pCurData->phyQueueId = phyQueueId;

                        /*Update US GEM flow setting, due to phy T-CONT change*/
                        if(pCurData->flowId != RT_GPON_INVALID_US_GEM_FLOW)
                        {
                            if((pGemFlowData = gemFlow_entry_find(pCurData->flowId)))
                            {
                                if(RT_ERR_OK !=rt_rtk_gpon_usFlow_set(pGemFlowData->flowId, &pGemFlowData->gemFlowcfg))
                                {
                                    RT_GPON_LOG_ERR("[%s] %d flow set fail, flowId %d, tcontId %d tcQueueId %d",
                                        __FUNCTION__, __LINE__, pGemFlowData->flowId, 
                                        pGemFlowData->gemFlowcfg.tcontId,pGemFlowData->gemFlowcfg.tcQueueId);
                                }
                               
                            }             
                        }
                    }   
        	    }


            }


        }

    }
#endif

    return RT_ERR_OK;
    
}

static int32 
rt_rtk_gpon_ponQueue_get(uint32 tcontId, uint32 tcQueueId, rt_gpon_queueCfg_t *pQueuecfg)
{
#if defined(CONFIG_SDK_KERNEL_LINUX)
	pon_queue_info_t	*pEntryData = NULL;
#endif

    /* parameter check */
    RT_PARAM_CHK((NULL == pQueuecfg), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((tcontId >= gRtkPlatformDb.maxTcont), RT_ERR_INPUT);
    RT_PARAM_CHK((tcQueueId >= gRtkPlatformDb.maxTcontQueue), RT_ERR_INPUT);   

#if defined(CONFIG_SDK_KERNEL_LINUX)
    if((pEntryData = pon_queue_entry_find(tcontId,tcQueueId)))
    {
        osal_memcpy(pQueuecfg, &pEntryData->Queuecfg, sizeof(rt_gpon_queueCfg_t));
        return RT_ERR_OK;
       
    }
#endif
    return RT_ERR_ENTRY_NOTFOUND; 

}


static int32 
rt_rtk_gpon_ponQueue_del(uint32 tcontId, uint32 tcQueueId)
{
    int32 ret = RT_ERR_OK;

#if (CONFIG_GPON_VERSION > 1)

#if defined(CONFIG_SDK_KERNEL_LINUX)
    rtk_ponmac_queue_t  queue;
	pon_queue_info_t	*pEntryData = NULL;
#endif

    /* parameter check */
    RT_PARAM_CHK((tcontId >= gRtkPlatformDb.maxTcont), RT_ERR_INPUT);
    RT_PARAM_CHK((tcQueueId >= gRtkPlatformDb.maxTcontQueue), RT_ERR_INPUT);   

#if defined(CONFIG_SDK_KERNEL_LINUX)
    if((pEntryData = pon_queue_entry_find(tcontId,tcQueueId)))
    {
        queue.queueId       = pEntryData->phyQueueId;
        queue.schedulerId   = LOGIC_TCONID_TO_PHY_TCONID(tcontId);;
        if((ret = rtk_ponmac_queue_del(&queue))!=RT_ERR_OK)
        {
            RT_GPON_LOG_ERR("[%s] %d queue del fail ret:%d tcQueueId:%d queueId:%d schedulerId:%d"
                ,__FUNCTION__, __LINE__, ret, tcQueueId, queue.queueId, queue.schedulerId);

            return ret;
        }

        pon_queue_entry_del(tcontId, tcQueueId); 
        phy_queue_db_set(queue.schedulerId, queue.queueId, FALSE);
    }
#endif	
#endif
    return ret;
}



static int32
rt_rtk_gpon_omci_rx_callback_register(rt_gpon_omci_rx_callback omciRx)
{
    int32   ret;

#if (CONFIG_GPON_VERSION > 1)
    if(NULL == omciRx)
        return RT_ERR_INPUT;
	
    rtGponOmciRx = omciRx;
    if((ret = rtk_gponapp_evtHdlOmci_reg(&rt_rtk_gpon_omci_rx)) != RT_ERR_OK)
    {
        osal_printf("%s: failed to register omci rx callback function!\n", __FUNCTION__);
        return ret;
    }
#endif

    return ret;
}

static int32
rt_rtk_gpon_scheInfo_get(rt_gpon_schedule_info_t *pScheInfo)
{
    int32   ret = RT_ERR_OK;

#if (CONFIG_GPON_VERSION > 1)
    pScheInfo->max_tcont = gRtkPlatformDb.maxTcont;
    pScheInfo->max_flow = gRtkPlatformDb.maxFlow;
    pScheInfo->max_pon_queue = gRtkPlatformDb.maxPonQueue;
    pScheInfo->max_tcon_queue =  gRtkPlatformDb.maxTcontQueue;
#endif

    return ret;
}

static int32
rt_rtk_gpon_flowCounter_get(uint32 flowId, rt_gpon_flow_counter_type_t flowCntType, rt_gpon_flow_counter_t *pFlowCnt)
{
    int32   ret = RT_ERR_OK;
    rtk_gpon_flow_performance_type_t rtkFlowPmType;
    rtk_gpon_flow_counter_t rtkFlowPm;

#if (CONFIG_GPON_VERSION > 1)
    if(NULL == pFlowCnt)
    {
        return RT_ERR_INPUT;
    }

    switch(flowCntType)
    {
        case RT_GPON_CNT_TYPE_FLOW_DS_GEM: 
            rtkFlowPmType = RTK_GPON_PMTYPE_FLOW_DS_GEM;
            break;
        case RT_GPON_CNT_TYPE_FLOW_DS_ETH: 
            rtkFlowPmType = RTK_GPON_PMTYPE_FLOW_DS_ETH;
            break;
        case RT_GPON_CNT_TYPE_FLOW_US_GEM: 
            rtkFlowPmType = RTK_GPON_PMTYPE_FLOW_US_GEM;
            break;
        case RT_GPON_CNT_TYPE_FLOW_US_ETH: 
            rtkFlowPmType = RTK_GPON_PMTYPE_FLOW_US_ETH;
            break;
        default:
            return RT_ERR_INPUT;
    }

    if((ret = rtk_gponapp_flowCounter_get(flowId, rtkFlowPmType, &rtkFlowPm)) != RT_ERR_OK)
    {
        osal_printf("Read Counter fail\r\n");
        return ret;
    }

     switch(flowCntType)
    {
        case RT_GPON_CNT_TYPE_FLOW_DS_GEM: 
            pFlowCnt->dsgem.gem_block = rtkFlowPm.dsgem.gem_block;
            pFlowCnt->dsgem.gem_byte= rtkFlowPm.dsgem.gem_byte;
            break;
        case RT_GPON_CNT_TYPE_FLOW_DS_ETH: 
            pFlowCnt->dseth.eth_pkt_rx = rtkFlowPm.dseth.eth_pkt_rx;
            pFlowCnt->dseth.eth_pkt_fwd = rtkFlowPm.dseth.eth_pkt_fwd;
            break;
        case RT_GPON_CNT_TYPE_FLOW_US_GEM: 
            pFlowCnt->usgem.gem_block = rtkFlowPm.usgem.gem_block;
            pFlowCnt->usgem.gem_byte= rtkFlowPm.usgem.gem_byte;
            break;
        case RT_GPON_CNT_TYPE_FLOW_US_ETH: 
        /*for coverity dead code issue*/
        default:
            pFlowCnt->useth.eth_cnt = rtkFlowPm.useth.eth_cnt;
            break;
    }
#endif

    return ret;
}

static int32
rt_rtk_gpon_pmCounter_get(rt_gpon_pm_type_t pmCntType, rt_gpon_pm_counter_t *pPmCnt)
{
    int32   ret = RT_ERR_OK;
    rtk_gpon_global_counter_t counter;

#if (CONFIG_GPON_VERSION > 1)
    memset(pPmCnt, 0x0, sizeof(rt_gpon_pm_counter_t));

    switch(pmCntType)
    {
        case RT_GPON_PM_TYPE_PHY_LOS: 
            if(rtk_gponapp_globalCounter_get(RTK_GPON_PMTYPE_DS_PHY,&counter)!=RT_ERR_OK)
            {
                osal_printf("Read Counter fail\r\n");
                return RT_ERR_FAILED;
            }

            pPmCnt->phyLos.bip_error_count = counter.dsphy.rx_bip_err_bit;
            pPmCnt->phyLos.total_lods_event_count = counter.dsphy.rx_lom;

            cur_time = jiffies_to_msecs(jiffies);
            if(cur_time > last_time)
                time_diff = cur_time-last_time;
            else
                time_diff = 0xffffffffffffffff/8;
            last_time = cur_time;

            pPmCnt->phyLos.total_words_protected_by_bip = time_diff*8;

            if(rtk_gponapp_globalCounter_get(RTK_GPON_PMTYPE_DS_GEM, &counter)!=RT_ERR_OK)
            {
                osal_printf("Read Counter fail\r\n");
                return RT_ERR_FAILED;
            }
            pPmCnt->phyLos.psbd_hec_errors_corrected = counter.dsgem.rx_hec_correct;
			
            break;
        case RT_GPON_PM_TYPE_FEC: 
            if(rtk_gponapp_globalCounter_get(RTK_GPON_PMTYPE_DS_PHY, &counter)!=RT_ERR_OK)
            {
                osal_printf("Read Counter fail\r\n");
                return RT_ERR_FAILED;
            }

            pPmCnt->fec.corrected_fec_bytes = counter.dsphy.rx_fec_correct_byte;
            pPmCnt->fec.corrected_fec_codewords = counter.dsphy.rx_fec_correct_cw;
            pPmCnt->fec.uncorrected_fec_codewords = counter.dsphy.rx_fec_uncor_cw;
            pPmCnt->fec.total_fec_codewords = counter.dsphy.rx_fec_total_block;
            
            break;
        case RT_GPON_PM_TYPE_XGEM: 
            if(rtk_gponapp_globalCounter_get(RTK_GPON_PMTYPE_DS_GEM, &counter)!=RT_ERR_OK)
            {
                osal_printf("Read Counter fail\r\n");
                return RT_ERR_FAILED;
            }
            pPmCnt->xgem.fs_words_lost_to_xgem_header_hec_errors = counter.dsgem.rx_gem_los;
            pPmCnt->xgem.total_received_xgem_frames = counter.dsgem.rx_gem_non_idle;
            pPmCnt->xgem.received_xgem_header_hec_errors = counter.dsgem.rx_gem_los;
			
            if(rtk_gponapp_globalCounter_get(RTK_GPON_PMTYPE_US_GEM, &counter)!=RT_ERR_OK)
            {
                osal_printf("Read Counter fail\r\n");
                return RT_ERR_FAILED;
            }
            pPmCnt->xgem.total_transmitted_xgem_frames = counter.usgem.tx_gem_cnt;
            pPmCnt->xgem.total_transmitted_bytes_in_non_idle_xgem_frames = counter.usgem.tx_gem_byte;

            break;
        case RT_GPON_PM_TYPE_PLOAM_PM1: 
            if(rtk_gponapp_globalCounter_get(RTK_GPON_PMTYPE_DS_PLOAM, &counter)!=RT_ERR_OK)
            {
                osal_printf("Read Counter fail\r\n");
                return RT_ERR_FAILED;
            }

            pPmCnt->ploam1.downstream_ploam_message_count = counter.dsploam.rx_ploam_cnt;
            pPmCnt->ploam1.ploam_mic_errors = counter.dsploam.rx_ploam_err;

            if(rtk_gponapp_globalCounter_get(RTK_GPON_PMTYPE_ONU_ACTIVATION, &counter)!=RT_ERR_OK)
            {
                osal_printf("Read Counter fail\r\n");
                return RT_ERR_FAILED;
            }

            pPmCnt->ploam1.ranging_time_message_count = counter.activate.rx_ranging_req;

            break;
        case RT_GPON_PM_TYPE_PLOAM_PM2: 

            break;
        case RT_GPON_PM_TYPE_PLOAM_PM3: 
            if(rtk_gponapp_globalCounter_get(RTK_GPON_PMTYPE_US_PLOAM, &counter)!=RT_ERR_OK)
            {
                osal_printf("Read Counter fail\r\n");
                return RT_ERR_FAILED;
            }

            pPmCnt->ploam3.upstream_ploam_message_count = counter.usploam.tx_ploam_proc;
            pPmCnt->ploam3.serial_number_onu_in_band_message_count = counter.usploam.tx_ploam_sn;

            break;
        case RT_GPON_PM_TYPE_OMCI: 
            if(rtk_gponapp_globalCounter_get(RTK_GPON_PMTYPE_DS_OMCI, &counter)!=RT_ERR_OK)
            {
                osal_printf("Read Counter fail\r\n");
                return RT_ERR_FAILED;
            }

            pPmCnt->omci.omci_rx_msg_count = counter.dsomci.rx_omci;

            if(rtk_gponapp_globalCounter_get(RTK_GPON_PMTYPE_US_OMCI, &counter)!=RT_ERR_OK)
            {
                osal_printf("Read Counter fail\r\n");
                return RT_ERR_FAILED;
            }

            pPmCnt->omci.omci_tx_msg_count = counter.usomci.tx_omci;

            break;
        case RT_GPON_PM_TYPE_CHAN_TUNING_PM1: //fall through
        case RT_GPON_PM_TYPE_CHAN_TUNING_PM2: //fall through 
        case RT_GPON_PM_TYPE_CHAN_TUNING_PM3:
            ret = RT_ERR_CHIP_NOT_SUPPORTED;
            break;
        default:
            ret = RT_ERR_INPUT;
            break;
    }

#endif

    return ret;
}

int32 rt_rtk_gpon_attribute_get(rt_gpon_attribute_type_t attributeType,rt_gpon_attribute_t *pAttributeValue)
{
    int32 ret = RT_ERR_OK;
#if (CONFIG_GPON_VERSION > 1)
    uint32 gemBlockLength = 0;
    rtk_gpon_us_dbr_para_t dbrStatus;
#if defined(CONFIG_SDK_KERNEL_LINUX)
    rtk_gpon_gtc_info_t gtc; 
#endif
    uint8 onuId;

    if(pAttributeValue == NULL)
        return RT_ERR_NULL_POINTER;
    
    switch(attributeType)
    {
        case RT_GPON_ATTRIBUTE_TYPE_DBRU_STATUS:
            if (RT_ERR_OK != rtk_gponapp_parameter_get(RTK_GPON_PARA_TYPE_US_DBR, &dbrStatus))
            {
                ret = RT_ERR_FAILED;
                break;
            }
            pAttributeValue->dbruStatus = dbrStatus.us_dbru_en;
            break;
        case RT_GPON_ATTRIBUTE_TYPE_GEM_BLOCK_LENGTH:
            if (RT_ERR_OK != rtk_gponapp_dbruBlockSize_get(&gemBlockLength))
            {
                ret = RT_ERR_FAILED;
                break;
            }
            pAttributeValue->gemBlockLength = gemBlockLength;
            break;
        case RT_GPON_ATTRIBUTE_TYPE_SD_THRESHOLD:
        case RT_GPON_ATTRIBUTE_TYPE_SF_THRESHOLD:
            ret = RT_ERR_CHIP_NOT_SUPPORTED;
            break;
        case RT_GPON_ATTRIBUTE_TYPE_GTC_INFO:
#if defined(CONFIG_SDK_KERNEL_LINUX)
            if ((ret = rtk_gponapp_gtc_get(&gtc)) != RT_ERR_OK)
            {
                break;
            }
            pAttributeValue->gtc_info.guard_bits = gtc.guard_bits;
            pAttributeValue->gtc_info.type1_bits = gtc.type1_bits;
            pAttributeValue->gtc_info.type2_bits = gtc.type2_bits;
            pAttributeValue->gtc_info.type3_bits = gtc.type3_bits;
            pAttributeValue->gtc_info.type3_ptn = gtc.type3_ptn;
            osal_memcpy(pAttributeValue->gtc_info.delimiter, gtc.delimiter, RT_GPON_DELIMITER_LEN);
            pAttributeValue->gtc_info.eqd = gtc.eqd;
            pAttributeValue->gtc_info.responTime = 35750;/*ns*/
#endif
            if ((ret = rtk_gponapp_powerLevel_get(&pAttributeValue->gtc_info.powerLevel)) != RT_ERR_OK)
            {
                break;
            }
            break;
        case RT_GPON_ATTRIBUTE_TYPE_ONUID:
            if ((ret = rtk_gponapp_parameter_get(RTK_GPON_PARA_TYPE_ONUID, (void *)&onuId)) != RT_ERR_OK )
            {
                break;
            }
            pAttributeValue->onuId = onuId;
            break;
        case RT_GPON_ATTRIBUTE_TYPE_ADMINSTATE:
            if ((ret = rtk_gponapp_parameter_get(RTK_GPON_PARA_TYPE_DRV_STATUS, (void *)&pAttributeValue->adminState)) != RT_ERR_OK )
            {
                break;
            }
            break;

        default:
            ret = RT_ERR_INPUT;
            break;
    }
#endif
    return ret;
}

int32 rt_rtk_gpon_attribute_set(rt_gpon_attribute_type_t attributeType,rt_gpon_attribute_t *pAttributeValue)
{
    int32 ret = RT_ERR_OK;
#if (CONFIG_GPON_VERSION > 1)
    rtk_gpon_us_dbr_para_t dbrStatus;

#if defined(CONFIG_SDK_KERNEL_LINUX)
    rtk_gpon_sig_para_t gpon_sig_para;
#endif

    if(pAttributeValue == NULL)
        return RT_ERR_NULL_POINTER;
    
    switch(attributeType)
    {
        case RT_GPON_ATTRIBUTE_TYPE_DBRU_STATUS:
            dbrStatus.us_dbru_en = pAttributeValue->dbruStatus;
            if(RT_ERR_OK != rtk_gponapp_parameter_set(RTK_GPON_PARA_TYPE_US_DBR,&dbrStatus))
                ret = RT_ERR_FAILED;
            break;
        case RT_GPON_ATTRIBUTE_TYPE_GEM_BLOCK_LENGTH:
            if (RT_ERR_OK != rtk_gponapp_dbruBlockSize_set(pAttributeValue->gemBlockLength))
                ret = RT_ERR_FAILED;
            break;
#if defined(CONFIG_SDK_KERNEL_LINUX)
        case RT_GPON_ATTRIBUTE_TYPE_SF_THRESHOLD:
            gpon_sig_para.type		= RTK_GPON_ALARM_SF;
            gpon_sig_para.threshold = pAttributeValue->sfThreshold;
            if (RT_ERR_OK != rtk_gponapp_signal_parameter_set(&gpon_sig_para))
            	ret = RT_ERR_FAILED;
            break;
        case RT_GPON_ATTRIBUTE_TYPE_SD_THRESHOLD:
            gpon_sig_para.type		= RTK_GPON_ALARM_SD;
            gpon_sig_para.threshold = pAttributeValue->sdThreshold;
            if (RT_ERR_OK != rtk_gponapp_signal_parameter_set(&gpon_sig_para))
            	ret = RT_ERR_FAILED;
            break;
#endif
        default:
            ret = RT_ERR_INPUT;
            break;
    }
#endif
    return ret;
}

static int32
rt_rtk_gpon_omci_mirror_set(rt_enable_t enable, rt_port_t mirroringPort)
{
    unsigned int mirroring_port = 0xFFFFFFFF;

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(mirroringPort), RT_ERR_PORT_ID);
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    if(ENABLED == enable)
        mirroring_port = mirroringPort;
    else
        mirroring_port = 0xFFFFFFFF;//disable mirror function

#if defined(CONFIG_SDK_KERNEL_LINUX)
    rtk_gponapp_omci_mirror_set(&mirroring_port);
#endif

    return RT_ERR_OK;
}

static int32
rt_rtk_gpon_forceEmergencyStop_set(rt_enable_t state)
{
    /* parameter check */
    RT_PARAM_CHK((state >= RTK_ENABLE_END), RT_ERR_INPUT);
#if defined(CONFIG_SDK_KERNEL_LINUX)    
    return rtk_gponapp_forceEmergencyStop_set(state);
#else
	return RT_ERR_OK;
#endif
}

static int32
rt_rtk_gpon_tcontCounter_get(rt_gpon_tcont_counter_t *pTcontCnt)
{
    int32   ret = RT_ERR_OK;
    rtk_gpon_tcont_counter_t rtkTcontPm;

#if (CONFIG_GPON_VERSION > 1)
    if(NULL == pTcontCnt)
    {
        return RT_ERR_NULL_POINTER;
    }

    if((ret = rtk_gponapp_tcontCounter_get (pTcontCnt->tcont_id, RTK_GPON_PMTYPE_TCONT_IDLE, &rtkTcontPm)) != RT_ERR_OK)
    {
        osal_printf("Read idle Counter fail ret 0x%x\r\n", ret);
        return ret;
    }
    if((ret = rtk_gponapp_tcontCounter_get (pTcontCnt->tcont_id, RTK_GPON_PMTYPE_TCONT_ETH, &rtkTcontPm)) != RT_ERR_OK)
    {
        osal_printf("Read eth Counter fail ret 0x%x\r\n", ret);
        return ret;
    }
    if((ret = rtk_gponapp_tcontCounter_get (pTcontCnt->tcont_id, RTK_GPON_PMTYPE_TCONT_GEM, &rtkTcontPm)) != RT_ERR_OK)
    {
        osal_printf("Read gem Counter fail ret 0x%x\r\n", ret);
        return ret;
    }
    
    pTcontCnt->tcont_idle_byte_count = rtkTcontPm.idle.idle_byte;
    pTcontCnt->tcont_eth_pkt_count = rtkTcontPm.eth.eth_pkt;
    pTcontCnt->tcont_gem_pkt_count = rtkTcontPm.gem.gem_pkt;
#endif

    return ret;
}
#endif


#ifdef CONFIG_LUNA_G3_SERIES
#define TCONIDTO_DEEQ_FLAG(_tcontId) ((_tcontId>=0x20)?0:1)

rt_ponmisc_ponQueueMode_t ponQueueMode = RT_PON_QUEUE_FIXED_MODE;

static int32
rt_g3_pon_queue_mode_sync(void)
{
    int ret = RT_ERR_OK;

#ifdef CONFIG_SDK_CA8277B

    if (NULL == RT_MAPPER->rt_ponmisc_ponQueue_mode_get)
        return RT_ERR_OK;

    /*Get PON queue mode*/
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_ponmisc_ponQueue_mode_get(&ponQueueMode);
    RTK_API_UNLOCK()

#endif

    return ret;

}


static int32
rt_g3_sid_set(rt_enable_t enable, uint32 sid)
{
    int ret = RT_ERR_OK;
    
#ifdef CONFIG_COMMON_RT_PONMISC
#if defined(CONFIG_CA8279_SERIES)
   /*Not support */
#else
    rt_ponmisc_sidInfo_t sidInfo;
    uint32 tcontId;
    uint32 tcQueueId;
    uint32 flowId;

    if (NULL == RT_MAPPER->rt_gpon_usFlow_phyData_get)
        return RT_ERR_OK;

    if(ENABLED == enable)
    {

        /*Get T-CONT,Cos*/
        RTK_API_LOCK();
        ret = RT_MAPPER->rt_gpon_usFlow_phyData_get(
            sid, &tcontId, &tcQueueId, &flowId);
        RTK_API_UNLOCK()
        
        if(ret!= RT_ERR_OK)
            return ret;


        if(RT_PON_QUEUE_FIXED_MODE == ponQueueMode)
        {
            sidInfo.enFlag      = 1;
            sidInfo.dstPort     = tcontId;
            sidInfo.dqFlag      = TCONIDTO_DEEQ_FLAG(tcontId);
            sidInfo.pri         = tcQueueId;
            sidInfo.flowId      = flowId;
        }
        else
        {
            sidInfo.enFlag      = 1;
            sidInfo.dqFlag      = (flowId >=32 && flowId <=63);
            sidInfo.dstPort     = sidInfo.dqFlag?(0x20+flowId/8):0x2F;
            sidInfo.pri         = flowId%8;
            sidInfo.flowId      = flowId;
        }
    }
    else
    {
        sidInfo.enFlag      = 0;
        sidInfo.dqFlag      = 0;
        sidInfo.dstPort     = 0;
        sidInfo.pri         = 0;
        sidInfo.flowId      = 0;

    }

    ret = rt_ponmisc_sid_set(sid, sidInfo);
    if(ret!= RT_ERR_OK)
        printk("[%s] %d set sid:%d fail\n",
            __FUNCTION__, __LINE__, sid);
#endif
#endif
    return ret;


}
#endif


/* Module Name    : Gpon     */
/* Sub-module Name: Gpon parameter settings */

/* Function Name:
 *      rt_gpon_init
 * Description:
 *      Initialize GPON MAC.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Module must be initialized before using all of APIs in this module
 */
int32
rt_gpon_init(void)
{
    int32   ret;

    /* function body */
#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_ponmisc_init)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_ponmisc_init();
    RTK_API_UNLOCK();
    if(ret!= RT_ERR_OK)
    {
#ifdef CONFIG_RTK_GPON_EXCEP_LOG
#if defined(CONFIG_SDK_KERNEL_LINUX)
        exc_log_add(EXC_PON_DRIVER_LOADING);
#endif
#endif
        return ret;
    }

    if (NULL != RT_MAPPER->gpon_init)
	{
		RTK_API_LOCK();
		ret = RT_MAPPER->gpon_init();
		RTK_API_UNLOCK();
		if(ret!= RT_ERR_OK)
        {
#ifdef CONFIG_RTK_GPON_EXCEP_LOG
#if defined(CONFIG_SDK_KERNEL_LINUX)
            exc_log_add(EXC_PON_DRIVER_LOADING);
#endif
#endif
			return ret;
        }
	}

    if (NULL == RT_MAPPER->rt_gpon_init)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_gpon_init();
    RTK_API_UNLOCK();
    if(ret!= RT_ERR_OK)
    {
#ifdef CONFIG_RTK_GPON_EXCEP_LOG
#if defined(CONFIG_SDK_KERNEL_LINUX)
        exc_log_add(EXC_PON_DRIVER_LOADING);
#endif
#endif
        return ret;
    }

    if (NULL == RT_MAPPER->ponmac_init)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_init();
    RTK_API_UNLOCK();
    if(ret!= RT_ERR_OK)
    {
#ifdef CONFIG_RTK_GPON_EXCEP_LOG
#if defined(CONFIG_SDK_KERNEL_LINUX)
        exc_log_add(EXC_PON_DRIVER_LOADING);
#endif
#endif
        return ret;
    }

    ret = rt_g3_pon_queue_mode_sync();
    
#else /* map to legacy RTK GPON APIs */
    ret = rt_rtk_gpon_init();
#endif

#ifdef CONFIG_RTK_GPON_EXCEP_LOG
#if defined(CONFIG_SDK_KERNEL_LINUX)
    if(ret != RT_ERR_OK)
        exc_log_add(EXC_PON_DRIVER_LOADING);
#endif
#endif

    return ret;
}   /* end of rt_gpon_init */

/* Function Name:
 *      rt_gpon_serialNumber_set
 * Description:
 *      Set GPON serial number
 * Input:
 *      pSN   - pointer to the GPON serial number
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32
rt_gpon_serialNumber_set(rt_gpon_serialNumber_t *pSN)
{
    int32   ret;

    /* function body */
#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_gpon_serialNumber_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_gpon_serialNumber_set(pSN);
    RTK_API_UNLOCK();
#else /* map to legacy RTK GPON APIs */
    ret = rt_rtk_gpon_serialNumber_set(pSN);
#endif

    return ret;
}   /* end of rt_gpon_serialNumber_set */

/* Function Name:
 *      rt_gpon_serialNumber_get
 * Description:
 *      Get GPON serial number
 * Input:
 *      pSN   - pointer to the GPON serial number
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32
rt_gpon_serialNumber_get(rt_gpon_serialNumber_t *pSN)
{
    int32   ret;

    /* function body */
#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_gpon_serialNumber_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_gpon_serialNumber_get(pSN);
    RTK_API_UNLOCK();
#else /* map to legacy RTK GPON APIs */
    ret = rt_rtk_gpon_serialNumber_get(pSN);
#endif

    return ret;
}   /* end of rt_gpon_serialNumber_get */

/* Function Name:
 *      rt_gpon_registrationId_set
 * Description:
 *      Set GPON registration ID (password in 984.3).
 * Input:
 *      pRegId   - pointer to the GPON registration id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32
rt_gpon_registrationId_set(rt_gpon_registrationId_t *pRegId)
{
    int32   ret;

    /* function body */
#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_gpon_registrationId_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_gpon_registrationId_set(pRegId);
    RTK_API_UNLOCK();
#else
    ret = rt_rtk_gpon_registrationId_set(pRegId);
#endif

    return ret;
}   /* end of rt_gpon_registrationId_set */

/* Function Name:
 *      rt_gpon_registrationId_get
 * Description:
 *      Get GPON registration ID (password in 984.3).
 * Input:
 *      pRegId   - pointer to the GPON registration id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32
rt_gpon_registrationId_get(rt_gpon_registrationId_t *pRegId)
{
    int32   ret;

    /* function body */
#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_gpon_registrationId_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_gpon_registrationId_get(pRegId);
    RTK_API_UNLOCK();
#else
    ret = rt_rtk_gpon_registrationId_get(pRegId);
#endif

    return ret;
}   /* end of rt_gpon_registrationId_set */

/* Function Name:
 *      rt_gpon_activate
 * Description:
 *      GPON MAC Activating.
 * Input:
 *      initState   - the initial state when ONU active
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      The GPON MAC is starting to work now.
 */
int32
rt_gpon_activate(rt_gpon_initialState_t initState)
{
    int32   ret;

    /* function body */
#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_gpon_activate)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_gpon_activate(initState);
    RTK_API_UNLOCK();
#else
    ret = rt_rtk_gpon_activate(initState);
#endif

    return ret;
}   /* end of rt_gpon_activate */

/* Function Name:
 *      rt_gpon_deactivate
 * Description:
 *      GPON MAC Deactivate.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 * Note:
 *      The GPON MAC is out of work now.
*/
int32
rt_gpon_deactivate(void)
{
    int32   ret;

    /* function body */
#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_gpon_deactivate)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_gpon_deactivate();
    RTK_API_UNLOCK();
#else
    ret = rt_rtk_gpon_deactivate();
#endif

    return ret;
}   /* end of rt_gpon_deactivate */

/* Function Name:
 *      rt_gpon_onuState_get
 * Description:
 *      Get the GPON ONU state.
 * Input:
 *      None
 * Output:
 *      pOnuState  - pointer to the ONU state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      If the device is not activated, an unknown status is returned.
 */
int32
rt_gpon_onuState_get(rt_gpon_onuState_t *pOnuState)
{
    int32   ret;

    /* function body */
#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_gpon_onuState_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_gpon_onuState_get(pOnuState);
    RTK_API_UNLOCK();
#else
    ret = rt_rtk_gpon_onuState_get(pOnuState);
#endif

    return ret;
}   /* end of rt_gpon_onuState_get */

/* Function Name:
 *      rt_gpon_omci_tx
 * Description:
 *      Transmit OMCI message.
 * Input:
 *      msgLen   - length of the OMCI message to be transmitted
 *      pMsg     - pointer to the OMCI message data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32
rt_gpon_omci_tx(uint32 msgLen, uint8 *pMsg)
{
    int32   ret;

    /* function body */
#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_gpon_omci_tx)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_gpon_omci_tx(msgLen, pMsg);
    RTK_API_UNLOCK();
#else
    ret = rt_rtk_gpon_omci_tx(msgLen, pMsg);
#endif

    return ret;
}   /* end of rt_gpon_omci_tx */

/* Function Name:
 *      rt_gpon_omci_rx_callback_register
 * Description:
 *      Register OMCI RX callback function.
 * Input:
 *      omciRx   - pointer to the callback function for OMCI RX
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32
rt_gpon_omci_rx_callback_register(rt_gpon_omci_rx_callback omciRx)
{
    int32   ret;

    /* function body */
#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_gpon_omci_rx_callback_register)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_gpon_omci_rx_callback_register(omciRx);
    RTK_API_UNLOCK();
#else
    ret = rt_rtk_gpon_omci_rx_callback_register(omciRx);
#endif
    return ret;
}   /* end of rt_gpon_omci_rx_callback_register */

/* Function Name:
 *      rt_gpon_tcont_set
 * Description:
 *      Set TCONT by assigning an allocation id.
 * Input:
 *      allocId   - allocation id 
 * Output:
 *      pTcontId  - pointer to the logic TCONT id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32 
rt_gpon_tcont_set(uint32 *pTcontId, uint32 allocId)
{
    int32   ret;

    /* function body */
#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_gpon_tcont_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_gpon_tcont_set(pTcontId, allocId);
    RTK_API_UNLOCK();
#else
    ret = rt_rtk_gpon_tcont_set(pTcontId, allocId);
#endif
    return ret;
}   /* end of rt_gpon_tcont_set */

/* Function Name:
 *      rt_gpon_tcont_get
 * Description:
 *      Get allocation id by assigned logic TCONT id.
 * Input:
 *      tcontId  -  logic TCONT id
 * Output:
 *      pAllocId   -  pointer to the allocation id 
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32 
rt_gpon_tcont_get(uint32 tcontId, uint32 *pAllocId)
{
    int32   ret;

    /* function body */
#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_gpon_tcont_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_gpon_tcont_get(tcontId, pAllocId);
    RTK_API_UNLOCK();
#else
    ret = rt_rtk_gpon_tcont_get(tcontId, pAllocId); 
#endif
    return ret;
}   /* end of rt_gpon_tcont_get */

/* Function Name:
 *      rt_gpon_tcont_del
 * Description:
 *      Delete the TCONT id
 * Input:
 *      tcontId   - TCONT id 
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 */
int32 
rt_gpon_tcont_del(uint32 tcontId)
{
    int32   ret;

    /* function body */
#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_gpon_tcont_del)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_gpon_tcont_del(tcontId);
    RTK_API_UNLOCK();
#else
    ret = rt_rtk_gpon_tcont_del(tcontId);
#endif
    return ret;
}   /* end of rt_gpon_tcont_del */

/* Function Name:
 *      rt_gpon_usFlow_set
 * Description:
 *      Set the upstream flow
 * Input:
 *      usFlowId  - logical flow id
 *      pUsFlow   - pointer to the upstream flow configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32 
rt_gpon_usFlow_set(uint32 usFlowId, rt_gpon_usFlow_t *pUsFlow)
{
    int32   ret;

    /* function body */
#ifdef CONFIG_LUNA_G3_SERIES

    if (NULL == RT_MAPPER->rt_gpon_usFlow_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_gpon_usFlow_set(usFlowId, pUsFlow);    
    RTK_API_UNLOCK();

    if(ret!= RT_ERR_OK)
        return ret;
    
    ret = rt_g3_sid_set(ENABLED, usFlowId);
#else
    ret = rt_rtk_gpon_usFlow_set(usFlowId, pUsFlow);
#endif
    return ret;
}   /* end of rt_gpon_usFlow_set */

/* Function Name:
 *      rt_gpon_usFlow_get
 * Description:
 *      Get the upstream flow
 * Input:
 *      usFlowId  - logical flow id
 * Output:
 *      pUsFlow   - pointer to the upstream flow configuration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32 
rt_gpon_usFlow_get(uint32 usFlowId, rt_gpon_usFlow_t *pUsFlow)
{
    int32   ret;

    /* function body */
#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_gpon_usFlow_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_gpon_usFlow_get(usFlowId, pUsFlow);
    RTK_API_UNLOCK();
#else
    ret = rt_rtk_gpon_usFlow_get(usFlowId, pUsFlow);
#endif
    return ret;
}   /* end of rt_gpon_usFlow_get */

/* Function Name:
 *      rt_gpon_usFlow_del
 * Description:
 *      Delete the upstream flow
 * Input:
 *      usFlowId  - logical flow id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 */
int32 
rt_gpon_usFlow_del(uint32 usFlowId)
{
    int32   ret;

    /* function body */
#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_gpon_usFlow_del)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_gpon_usFlow_del(usFlowId);
    RTK_API_UNLOCK();

    if(ret!= RT_ERR_OK)
        return ret;

    ret = rt_g3_sid_set(DISABLED, usFlowId);
#else
    ret = rt_rtk_gpon_usFlow_del(usFlowId);
#endif
    return ret;
}   /* end of rt_gpon_usFlow_del */

/* Function Name:
 *      rt_gpon_usFlow_delAll
 * Description:
 *      Delete all the upstream flows
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 * Note:
 */
int32 
rt_gpon_usFlow_delAll(void)
{
    int32   ret;

    /* function body */
#ifdef CONFIG_LUNA_G3_SERIES
    rt_gpon_schedule_info_t scheInfo;
    uint32 sid;
    rt_gpon_usFlow_t usFlow;


    if (NULL == RT_MAPPER->rt_gpon_usFlow_delAll)
        return RT_ERR_DRIVER_NOT_FOUND;

    if(RT_ERR_OK == rt_gpon_scheInfo_get(&scheInfo))
    {

        for(sid = 0; sid <scheInfo.max_flow; sid++)
        {

            if(RT_ERR_OK == rt_gpon_usFlow_get(sid, &usFlow))
            {
                ret = rt_g3_sid_set(DISABLED, sid);
            }
        }

    }

    RTK_API_LOCK();
    ret = RT_MAPPER->rt_gpon_usFlow_delAll();
    RTK_API_UNLOCK();
    
#else
    ret = rt_rtk_gpon_usFlow_delAll();
#endif
    return ret;
}   /* end of rt_gpon_usFlow_delAll */

/* Function Name:
 *      rt_gpon_dsFlow_set
 * Description:
 *      Set the downstream flow 
 * Input:
 *      dsFlowId  - logical flow id
 *      pDsFlow   - pointer to the downstream flow configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32 
rt_gpon_dsFlow_set(uint32 dsFlowId, rt_gpon_dsFlow_t *pDsFlow)
{
    int32   ret;

    /* function body */
#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_gpon_dsFlow_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_gpon_dsFlow_set(dsFlowId, pDsFlow);
    RTK_API_UNLOCK();
#else
    ret = rt_rtk_gpon_dsFlow_set(dsFlowId, pDsFlow);
#endif
    return ret;

}   /* end of rt_gpon_dsFlow_set */

/* Function Name:
 *      rt_gpon_dsFlow_get
 * Description:
 *      Get the downstream flow 
 * Input:
 *      dsFlowId  - logical flow id
 * Output:
 *      pDsFlow   - pointer to the downstream flow configuration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32 
rt_gpon_dsFlow_get(uint32 dsFlowId, rt_gpon_dsFlow_t *pDsFlow)
{
    int32   ret;

    /* function body */
#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_gpon_dsFlow_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_gpon_dsFlow_get(dsFlowId, pDsFlow);
    RTK_API_UNLOCK();
#else
    ret = rt_rtk_gpon_dsFlow_get(dsFlowId, pDsFlow);
#endif
    return ret;
}   /* end of rt_gpon_dsFlow_get */

/* Function Name:
 *      rt_gpon_dsFlow_del
 * Description:
 *      Delete the downstream flow
 * Input:
 *      dsFlowId  - logical flow id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 */
int32 
rt_gpon_dsFlow_del(uint32 dsFlowId)
{
    int32   ret;

    /* function body */
#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_gpon_dsFlow_del)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_gpon_dsFlow_del(dsFlowId);
    RTK_API_UNLOCK();
#else
    ret = rt_rtk_gpon_dsFlow_del(dsFlowId);    
#endif
    return ret;
}   /* end of rt_gpon_dsFlow_del */

/* Function Name:
 *      rt_gpon_dsFlow_delAll
 * Description:
 *      Delete all the downstream flows
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 * Note:
 */
int32 
rt_gpon_dsFlow_delAll(void)
{
    int32   ret;

    /* function body */
#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_gpon_dsFlow_delAll)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_gpon_dsFlow_delAll();
    RTK_API_UNLOCK();
#else
    ret = rt_rtk_gpon_dsFlow_delAll();
#endif
    return ret;
}   /* end of rt_gpon_dsFlow_delAll */

/* Function Name:
 *      rt_gpon_loop_gemport_set
 * Description:
 *      Delete all the downstream flows
 * Input:
 *      gemPortId - gem port Index
 *      enable - enable/disable loop function of specified GEM  
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_DRIVER_NOT_FOUND     - chip not support 
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_INPUT                - Invalid input parameters.
 *      RT_ERR_ENTRY_NOTFOUND       - specified entry not found
 * Note:
 */
int32 
rt_gpon_loop_gemport_set(uint32 gemPortId, rt_enable_t enable)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->rt_gpon_loop_gemport_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_gpon_loop_gemport_set(gemPortId, enable);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rt_gpon_loop_gemport_set */

/* Function Name:
 *      rt_gpon_loop_gemport_get
 * Description:
 *      Delete all the downstream flows
 * Input:
 *      gemPortId - gem port Index
 * Output:
 *      pEnable    - get Enable/disble loop of specified GEM
 * Return:
 *      RT_ERR_OK                   - OK
 *      RT_ERR_FAILED               - Failed
 *      RT_ERR_DRIVER_NOT_FOUND     - chip not support 
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_INPUT                - Invalid input parameters.
 *      RT_ERR_ENTRY_NOTFOUND       - specified entry not found
 * Note:
 */
int32 
rt_gpon_loop_gemport_get(uint32 gemPortId, rt_enable_t *pEnable)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->rt_gpon_loop_gemport_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_gpon_loop_gemport_get(gemPortId, pEnable);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rt_gpon_loop_gemport_set */


/* Function Name:
 *      rt_gpon_ponQueue_set
 * Description:
 *      Set the pon queue
 * Input:
 *      tcontId    - logical tcont index
 *      tcQueueId  - logical tcont queue id
 *      pQueuecfg  - pointer to the queue configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32 
rt_gpon_ponQueue_set(uint32 tcontId, uint32 tcQueueId, rt_gpon_queueCfg_t *pQueuecfg)
{
    int32   ret;
    /* function body */
    if(pQueuecfg->cir == RT_MAX_RATE)
        pQueuecfg->cir = RT_RATE_PON_EGR_PIR_CIR_RATE_MAX;

    if(pQueuecfg->pir == RT_MAX_RATE)
        pQueuecfg->pir = RT_RATE_PON_EGR_PIR_CIR_RATE_MAX;

#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_gpon_ponQueue_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_gpon_ponQueue_set(tcontId, tcQueueId, pQueuecfg);
    RTK_API_UNLOCK();
#else
    ret = rt_rtk_gpon_ponQueue_set(tcontId, tcQueueId, pQueuecfg);
#endif
    return ret;
}   /* end of rt_gpon_ponQueue_set */

/* Function Name:
 *      rt_gpon_ponQueue_get
 * Description:
 *      Get the pon queue
 * Input:
 *      tcontId    - logical tcont index
 *      tcQueueId  - logical tcont queue id
 * Output:
 *      pQueuecfg - pointer to the queue configuration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32 
rt_gpon_ponQueue_get(uint32 tcontId, uint32 tcQueueId, rt_gpon_queueCfg_t *pQueuecfg)
{
    int32   ret;

    /* function body */
#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_gpon_ponQueue_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_gpon_ponQueue_get(tcontId, tcQueueId, pQueuecfg);
    RTK_API_UNLOCK();
#else
    ret = rt_rtk_gpon_ponQueue_get(tcontId, tcQueueId, pQueuecfg);    
#endif
    return ret;
}   /* end of rt_gpon_ponQueue_get */

/* Function Name:
 *      rt_gpon_ponQueue_del
 * Description:
 *      Delete the pon queue
 * Input:
 *      tcontId    - logical tcont index
 *      tcQueueId  - logical tcont queue id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 */
int32 
rt_gpon_ponQueue_del(uint32 tcontId, uint32 tcQueueId)
{
    int32   ret;

    /* function body */
#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_gpon_ponQueue_del)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_gpon_ponQueue_del(tcontId, tcQueueId);
    RTK_API_UNLOCK();
#else
    ret = rt_rtk_gpon_ponQueue_del(tcontId, tcQueueId);     
#endif
    return ret;
}   /* end of rt_gpon_ponQueue_del */

/* Function Name:
 *      rt_gpon_scheInfo_get
 * Description:
 *      Get scheduling information
 * Input:
 *      None
 * Output:
 *      pScheInfo  - pointer to the scheduling information
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32 
rt_gpon_scheInfo_get(rt_gpon_schedule_info_t *pScheInfo)
{
    int32   ret;

    /* function body */
#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_gpon_scheInfo_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_gpon_scheInfo_get(pScheInfo);
    RTK_API_UNLOCK();
#else
    ret = rt_rtk_gpon_scheInfo_get(pScheInfo);
#endif
    return ret;
}   /* end of rt_gpon_scheInfo_get */

/* Function Name:
 *      rt_gpon_flowCounter_get
 * Description:
 *      Get the flow counters.
 * Input:
 *      flowId   - logical flow ID
 *      type     - counter type
 * Output:
 *      pFlowCnt - pointer to the flow counters
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32 
rt_gpon_flowCounter_get(uint32 flowId, rt_gpon_flow_counter_type_t flowCntType, rt_gpon_flow_counter_t *pFlowCnt)
{
    int32   ret;

    /* function body */
#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_gpon_flowCounter_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_gpon_flowCounter_get(flowId, flowCntType, pFlowCnt);
    RTK_API_UNLOCK();
#else
    ret = rt_rtk_gpon_flowCounter_get(flowId, flowCntType, pFlowCnt);
#endif
    return ret;
}   /* end of rt_gpon_flowCounter_get */

/* Function Name:
 *      rt_gpon_pmCounter_get
 * Description:
 *      Get the pm counters.
 * Input:
 *      pmCntType       - counter type
 * Output:
 *      pPmCnt       - pointer to the global counters
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32 
rt_gpon_pmCounter_get(rt_gpon_pm_type_t pmCntType, rt_gpon_pm_counter_t *pPmCnt)
{
    int32   ret;

    /* function body */
#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_gpon_pmCounter_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_gpon_pmCounter_get(pmCntType, pPmCnt);
    RTK_API_UNLOCK();
#else
    ret = rt_rtk_gpon_pmCounter_get(pmCntType, pPmCnt);
#endif
    return ret;
}   /* end of rt_gpon_pmCounter_get */

/* Function Name:
 *      rt_gpon_ponTag_get
 * Description:
 *      Get the PON Tag.
 * Input:
 *      None
 * Output:
 *      pPonTag       - pointer to the PON TAG
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32 
rt_gpon_ponTag_get(rt_gpon_ponTag_t *pPonTag)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->rt_gpon_ponTag_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_gpon_ponTag_get(pPonTag);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rt_gpon_ponTag_get */

/* Function Name:
 *      rt_gpon_msk_set
 * Description:
 *      Set the Master Session Key.
 * Input:
 *      pMsk - pointer to the Master Session Key
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32 
rt_gpon_msk_set(rt_gpon_msk_t *pMsk)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->rt_gpon_msk_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_gpon_msk_set(pMsk);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rt_gpon_msk_set */

/* Function Name:
 *      rt_gpon_omci_mic_generate
 * Description:
 *      Generate the OMCI MIC value according to the inputs.
 * Input:
 *      dir     - the direction of the OMCI msg, 0x01 for Downstream and 0x02 for Upstream
 *      pMsg    - pointer to the OMCI message data
 *      msgLen  - length of the OMCI message, not include MIC
 * Output:
 *      mic     - pointer to the generated MIC
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32
rt_gpon_omci_mic_generate(uint32 dir, uint8 *pMsg, uint32 msgLen, uint32 *mic)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->rt_gpon_omci_mic_generate)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_gpon_omci_mic_generate(dir, pMsg, msgLen, mic);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rt_gpon_omci_mic_generate */

/* Function Name:
 *      rt_gpon_mcKey_set
 * Description:
 *      Set the Multicast Key.
 * Input:
 *      pMcKey - pointer to the Multicast Key
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32 
rt_gpon_mcKey_set(rt_gpon_mc_key_t *pMcKey)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->rt_gpon_mcKey_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_gpon_mcKey_set(pMcKey);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rt_gpon_mcKey_set */

/* Function Name:
 *      rt_gpon_egrBandwidthCtrlRate_get
 * Description:
 *      Get the egress bandwidth control rate.
 * Input:
 *      None
 * Output:
 *      pRate - egress bandwidth control rate
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      The unit of granularity is 1Kbps.
 */
int32
rt_gpon_egrBandwidthCtrlRate_get(uint32 *rate)
{
    int32   ret;
    int32   getRate = 0;

    /* function body */
    if (NULL == RT_MAPPER->ponmac_egrBandwidthCtrlRate_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_egrBandwidthCtrlRate_get(&getRate);
    RTK_API_UNLOCK();

    *rate = getRate;

    return ret;
}   /* end of rt_gpon_egrBandwidthCtrlRate_get */

/* Function Name:
 *      rt_gpon_egrBandwidthCtrlRate_set
 * Description:
 *      Set the pon port egress bandwidth control rate.
 * Input:
 *      rate - egress bandwidth control rate
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - Invalid port id
 *      RT_ERR_RATE    - Invalid input rate
 * Note:
 *      The unit of granularity is 1Kbps.
 */
int32
rt_gpon_egrBandwidthCtrlRate_set(uint32 rate)
{
    int32   ret;
    int32   setRate = 0;

    /* function body */
    if (NULL == RT_MAPPER->ponmac_egrBandwidthCtrlRate_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    if(rate == RT_MAX_RATE)
        setRate = RT_RATE_PON_EGR_RATE_MAX;
    else
    {
        setRate = rate;
    }

    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_egrBandwidthCtrlRate_set(setRate);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rt_epon_egrBandwidthCtrlRate_set */

/* Function Name:
 *      rt_gpon_attribute_get
 * Description:
 *      Get the GPON related attribute.
 * Input:
 *      attributeType  - attribute type
 * Output:
 *      pAttributeValue       - pointer to the attribute value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32
rt_gpon_attribute_get(rt_gpon_attribute_type_t attributeType,rt_gpon_attribute_t *pAttributeValue)
{
    int32   ret;
    /* function body */
#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_gpon_attribute_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_gpon_attribute_get(attributeType, pAttributeValue);
    RTK_API_UNLOCK();
#else
    ret = rt_rtk_gpon_attribute_get(attributeType, pAttributeValue);
#endif
    return ret;
}   /* end of rt_gpon_attribute_get */

/* Function Name:
 *      rt_gpon_attribute_set
 * Description:
 *      Set the GPON related attribute.
 * Input:
 *      attributeType  - attribute type
 * Output:
 *      pAttributeValue       - pointer to the attribute value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
extern int32
rt_gpon_attribute_set(rt_gpon_attribute_type_t attributeType,rt_gpon_attribute_t *pAttributeValue)
{
    int32   ret;
    /* function body */
#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_gpon_attribute_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_gpon_attribute_set(attributeType, pAttributeValue);
    RTK_API_UNLOCK();
#else
    ret = rt_rtk_gpon_attribute_set(attributeType, pAttributeValue);
#endif
    return ret;
}   /* end of rt_gpon_attribute_set */

/* Function Name:
 *      rt_gpon_omci_mirror_set
 * Description:
 *      Set the GPON OMCI mirror function.
 * Input:
 *      enable - enable/disable mirro function 
 *      mirroringPort - mirroring port 
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - Invalid port id
 *      RT_ERR_INPUT   - Invalid input 
 * Note:
 */
int32
rt_gpon_omci_mirror_set(rt_enable_t enable, rt_port_t mirroringPort)
{
    int32   ret;

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(mirroringPort), RT_ERR_PORT_ID);
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    /* function body */
#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_gpon_omci_mirror_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_gpon_omci_mirror_set(enable, mirroringPort);
    RTK_API_UNLOCK();

#else
    ret = rt_rtk_gpon_omci_mirror_set(enable, mirroringPort);
#endif
    return ret;
}   /* end of rt_gpon_omci_mirror_set */

/* Function Name:
 *      rt_gpon_forceEmergencyStop_set
 * Description:
 *      Set the GPON force emergency Stop state.
 * Input:
 *      State  - enable/disable emergency stop state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 */
int32
rt_gpon_forceEmergencyStop_set(rt_enable_t state)
{
    int32   ret;
    /* function body */
#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_gpon_forceEmergencyStop_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_gpon_forceEmergencyStop_set(state);
    RTK_API_UNLOCK();
#else
    ret = rt_rtk_gpon_forceEmergencyStop_set(state);
#endif
    return ret;
}   /* end of rt_gpon_forceEmergencyStop_set */

/* Function Name:
 *      rt_gpon_tcontCounter_get
 * Description:
 *      Get the tcont counters.
 * Input:
 *      pTcontCnt->tconId  - tcont ID
 * Output:
 *      pTcontCnt - pointer to the flow counters
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32 
rt_gpon_tcontCounter_get(rt_gpon_tcont_counter_t *pTcontCnt)
{
    int32   ret;

    /* function body */
#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_gpon_tcontCounter_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_gpon_tcontCounter_get(pTcontCnt);
    RTK_API_UNLOCK();
#else
    ret = rt_rtk_gpon_tcontCounter_get(pTcontCnt);
#endif
    return ret;
}   /* end of rt_gpon_tcontCounter_get */

/* Function Name:
 *      rt_gpon_omcc_get
 * Description:
 *      get omcc information.
 * Input:
 *      
 * Output:
 *      pOmcc                 -   omcc information
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 * Note:
 */
int32 
rt_gpon_omcc_get(rt_gpon_omcc_t *pOmcc)
{
    int32   ret;
    rt_gpon_onuState_t onuState;
    rtk_gpon_usFlow_attr_t attr;

    ret = rt_gpon_onuState_get(&onuState);
    if(ret != RT_ERR_OK)
        return ret;

    pOmcc->allocId = 0;
    pOmcc->gemId = 0;

#ifdef CONFIG_LUNA_G3_SERIES
    if(onuState != RT_NGP2_ONU_STATE_O5_1 && onuState != RT_NGP2_ONU_STATE_O5_2)
    {
        return RT_ERR_ENTRY_NOTFOUND;
    }

    if (NULL == RT_MAPPER->rt_gpon_omcc_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->rt_gpon_omcc_get(pOmcc);
    RTK_API_UNLOCK();

#else
    if(onuState != RT_GPON_ONU_STATES_O5)
    {
        return RT_ERR_ENTRY_NOTFOUND;
    }

    ret = rtk_gpon_usTcont_get(gRtkPlatformDb.omccTcont,&(pOmcc->allocId));
    if(ret != RT_ERR_OK)
        return ret;

    ret = rtk_gponapp_usFlow_get(gRtkPlatformDb.omccFlow,&attr);
    if(ret != RT_ERR_OK)
        return ret;

    pOmcc->gemId = attr.gem_port_id;
#endif
    return ret;
} /* end of rt_gpon_omcc_get */

/* Function Name:
 *      rt_gpon_fec_get
 * Description:
 *      Get the GPON FEC status function.
 * Input:
 *
 * Output:
 *      pFecStatus          -   fec status information
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32
rt_gpon_fec_get(rt_gpon_fec_status_t *pFecStatus)
{
    int32   ret;

    /* function body */
#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_gpon_fec_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_gpon_fec_get(pFecStatus);
    RTK_API_UNLOCK();
#else
#if defined(CONFIG_SDK_KERNEL_LINUX)
    ret = rtk_gponapp_fec_get((rtk_gpon_fec_status_t *)pFecStatus);
#else
    //for X86 simulation only
    pFecStatus->us_fec_status = 0;
    pFecStatus->ds_fec_status = 0;
#endif
#endif

    return ret;
} /* end of rt_gpon_fec_get */

/* Function Name:
 *      dal_rt_rtl8277c_gpon_ploam_rx_register
 * Description:
 *      register ploam rx callback
 * Input:
 *      func            - callback func
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - OK
 *      RT_ERR_FAILED   - Failed
 *      RT_ERR_INPUT    - null callback func
 * Note:
 */
int32
rt_gpon_ploam_rx_register(rt_gpon_ploam_rx_callback func)
{
    int32   ret=RT_ERR_OK;

    /* function body */
#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_gpon_ploam_rx_register)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_gpon_ploam_rx_register(func);
    RTK_API_UNLOCK();
#else

#endif

    return ret;
} /* end of rt_gpon_ploam_rx_register */


/* Function Name:
 *      rt_gpon_fttr_omci_rx_callback_register
 * Description:
 *      Register OMCI RX callback function.
 * Input:
 *      omciRx   - pointer to the callback function for OMCI RX
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_DRIVER_NOT_FOUND - input parameter may be null pointer
 * Note:
 */
int32
rt_gpon_fttr_omci_rx_callback_register(rt_gpon_omci_rx_callback fttrOmciRx)
{
    int32   ret;

    /* function body */
#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_gpon_fttr_omci_rx_callback_register)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_gpon_fttr_omci_rx_callback_register(fttrOmciRx);
    RTK_API_UNLOCK();
#else
    ret = RT_ERR_DRIVER_NOT_FOUND;
#endif
    return ret;
}   /* end of rt_gpon_fttr_omci_rx_callback_register */
