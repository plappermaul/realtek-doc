/*
 * Copyright (C) 2015 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 88116 $
 * $Date: 2018-05-18 15:04:47 +0800 (Fri, 18 May 2018) $
 *
 * Purpose : L2-Notification initialization.
 *
 * Feature : L2-Notification initialization
 *
 */

/*
 * Include Files
 */
#include <common/debug/rt_log.h>
#include <common/error.h>
#include <private/drv/swcore/swcore.h>
#include <private/drv/swcore/chip_probe.h>
#include <drv/nic/nic.h>
#include <drv/l2ntfy/l2ntfy.h>
#include <private/drv/nic/nic_mapper.h>
#include <private/drv/l2ntfy/l2ntfy_mapper.h>
#include <private/drv/l2ntfy/l2ntfy_util.h>
#include <osal/atomic.h>
#include <osal/lib.h>
#include <osal/print.h>
#include <osal/cache.h>
#include <osal/workqueue.h>
#include <osal/spl.h>
#include <osal/time.h>
#include <ioal/mem32.h>
#include <hwp/hw_profile.h>
#include <hal/common/halctrl.h>

/*
 * Symbol Definition
 */
#define         L2_NTFY_BP_THR                      100
#define         MAX_MEM_USAGE                       524288
#define         DEBUG_STOP_RELEASE_BIT              (1 << L2NTFY_DEBUG_STOP_RELEASE)    /* 0x00000001 */

#define         NTFY_ENTRY_IDX(ringID, entryID)     ((ringID)*nBufSize*entrySize + (entryID)*entrySize)
#define         NTFY_RING_ENTRY_IDX(ringID)         ((ringID)*nBufSize*entrySize)

#define         l2DBG(fmt, args...)\
do {\
    if (TRUE == ntfy_debug_flag) {\
        osal_printf("%s,%d:"fmt"\n", __FUNCTION__, __LINE__, ##args);\
    }\
} while (0)


/*
 * Data Type Definition
 */
typedef struct l2ntfy_sizeInfo_s
{
    uint32      ringSize;
    uint32      nBufSize;
    uint32      entrySize;
} l2ntfy_sizeInfo_t;

typedef struct ntfy_collectArrayList_s
{
    uint32              unit;
    rtk_l2ntfy_mode_t   blockMode;   /* Dynamic change mode */
    uint32              blockSize;   /* Unit is different between isr and pkt mode */
    osal_list_head_t    list;
    uint8              *pNBufTmp;
    uint32              blockMemSize;
} ntfy_collectArrayList_t;

typedef struct ntfy_runOutHdlList_s
{
    uint32                  unit;
    rtk_l2ntfy_runOutType_t runOutType;
    osal_list_head_t        list;
} ntfy_runOutHdlList_t;

/*
 * Data Declaration
 */
static uint32                           l2ntfy_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};
static rtk_l2ntfy_dst_t                 l2ntfy_dst = L2NTFY_DST_NIC;
static uint8                            *nBuf = NULL;
static uint32                           *eventRing = NULL;
static uint32                           lastRingID = 0;
static uint32                           ringSize = 0;
static uint32                           nBufSize = 10;
static uint32                           entrySize = 12;
static uint32                           totalEventCount = 0;
static uint32                           runoutCnt = 0;
static uint32                           local_runoutCnt = 0;
static uint32                           memMaxCnt = 0;
static uint32                           ntfy_debug_flag = FALSE;
static uint32                           masterUnitId = 0;
static uint32                           slaveUnitId = 0;
static osal_work_struct_t               workq;
static osal_work_struct_t               runoutWorkq;
static osal_atomic_t                    mem_usage = OSAL_ATOMIC_INIT(0);
static osal_atomic_t                    mem_lock = OSAL_ATOMIC_INIT(0);
static uint32                           lockCnt, releaseCnt;
static osal_atomic_t                    pktDropIsrLock = OSAL_ATOMIC_INIT(0);
static osal_atomic_t                    bufRunoutIsrLock = OSAL_ATOMIC_INIT(0);
static osal_atomic_t                    localBufRunoutIsrLock = OSAL_ATOMIC_INIT(0);

static uint32                           sqcId[RTK_MAX_NUM_OF_UNIT];
static uint32                           sqcIdMax = 0xFFFF;
static uint32                           proLoc = 12;
static uint32                           pktCnt = 0;
static uint32                           pktDropCnt[RTK_MAX_NUM_OF_UNIT];
static uint32                           l2ntfyResetCnt[RTK_MAX_NUM_OF_UNIT];
static uint32                           mgcNum = 0;

ntfy_collectArrayList_t                 ntfyListHead;
ntfy_runOutHdlList_t                    runOutListHead;
drv_l2ntfy_cb_f                         ntfy_callback = NULL;
drv_l2ntfy_runoutcb_f                   runout_callback = NULL;

static                                  osal_spinlock_t l2ntfy_lock;
#define L2NTFY_SEM_LOCK(unit)           osal_spl_spin_lock(&l2ntfy_lock)
#define L2NTFY_SEM_UNLOCK(unit)         osal_spl_spin_unlock(&l2ntfy_lock)

void _l2ntfy_bhWQ(osal_work_struct_arg_t *data);
void _l2ntfy_runoutHandle_bhWQ(osal_work_struct_arg_t *data);

int32 drv_l2ntfy_pktDrop_handler(uint32 unit, rtk_l2ntfy_runOutType_t dropType);

int32 _l2ntfy_intrMask_get(uint32 unit, nic_intr_type_t type, uint32 *pMask)
{
    return NIC_CTRL(unit).intrMask_get(unit, type, pMask);
}

int32 _l2ntfy_intrMask_set(uint32 unit, nic_intr_type_t type, uint32 mask)
{
    return NIC_CTRL(unit).intrMask_set(unit, type, mask);
}

int32 _l2ntfy_intrSts_set(uint32 unit, nic_intr_type_t type, uint32 val)
{
    return NIC_CTRL(unit).intrSts_set(unit, type, val);
}

int32 _l2ntfy_ntfyBaseAddr_get(uint32 unit, uint32 *pAddr)
{
    return NIC_CTRL(unit).ntfyBaseAddr_get(unit, pAddr);
}

int32 _l2ntfy_ntfyBaseAddr_set(uint32 unit, uint32 addr)
{
    return NIC_CTRL(unit).ntfyBaseAddr_set(unit, addr);
}

int32 _l2ntfy_ntfyEntryLen_get(uint32 unit, uint32 *pEntryLen)
{
    return L2NTFY_CTRL(unit).l2ntfy_entryLen_get(unit, pEntryLen);
}

int32 _l2ntfy_ntfyPktCpuQue_set(uint32 unit, uint32 queId)
{
    return L2NTFY_CTRL(unit).l2ntfy_pktCpuQue_set(unit, queId);
}

int32 _l2ntfy_pktCpuQueBwCtrlEnable_set(uint32 unit, uint32 queId, rtk_enable_t enable)
{
    return L2NTFY_CTRL(unit).l2ntfy_pktCpuQueBwCtrlEnable_set(unit, queId, enable);
}

int32 _l2ntfy_pktCpuQueRate_set(uint32 unit, uint32 queId, uint32 rate)
{
    return L2NTFY_CTRL(unit).l2ntfy_pktCpuQueRate_set(unit, queId, rate);
}

int32 _l2ntfy_rawToEvent_cnvt(uint32 unit, uint8 *pRaw, rtk_l2ntfy_eventEntry_t *pEvent)
{
    if (L2NTFY_CTRL(unit).l2ntfy_rawToEvent_cnvt == NULL)
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    return L2NTFY_CTRL(unit).l2ntfy_rawToEvent_cnvt(unit, pRaw, pEvent);
}

/*  Master switch need this action */
static int32
_l2ntfy_pkt_init(uint32 unit)
{
    rtk_l2ntfy_iTagCfg_t    iTagCfg;
    uint32                  i;

    for(i = 0; i < RTK_MAX_NUM_OF_UNIT; i++)
    {
        sqcId[i] = 0;
    }
    drv_l2ntfy_magicNum_get(unit, &mgcNum);

    /* Reset  L2 notification counter */
    drv_l2ntfy_counter_clear(unit);

    /* Set queue */
    _l2ntfy_ntfyPktCpuQue_set(unit, L2NTFY_PKT_CPU_QUEUE_PRI);
    _l2ntfy_pktCpuQueRate_set(unit, L2NTFY_PKT_CPU_QUEUE_PRI, L2NTFY_PKT_CPU_QUEUE_BLK);

    /* Get Parser start location */
    drv_l2ntfy_iTag_get(unit, &iTagCfg);
    if(TRUE == iTagCfg.iTagif)
    {
        proLoc = 16;
    }
    else
    {
        proLoc = 12;
    }

    _l2ntfy_ntfyEntryLen_get(unit, &entrySize);

    /* Enable L2 notification interrupt */
    _l2ntfy_pktCpuQueBwCtrlEnable_set(unit, L2NTFY_PKT_CPU_QUEUE_PRI, DISABLED);
    _l2ntfy_intrMask_set(unit, NIC_NTFY_LOCALBUF_RUNOUT, ENABLED);
    if(TRUE == HWP_CASCADE_MODE())
    {
        /* Todo, not process or special process slave runout, depend on hardware */

    }

    return RT_ERR_OK;
}

static int32
_l2ntfy_pkt_unInit(uint32 unit)
{
    /* Disable L2 notification interrupt */
    _l2ntfy_pktCpuQueBwCtrlEnable_set(unit, L2NTFY_PKT_CPU_QUEUE_PRI, ENABLED);
    _l2ntfy_intrMask_set(unit, NIC_NTFY_LOCALBUF_RUNOUT, DISABLED);
    if(TRUE == HWP_CASCADE_MODE())
    {
        /* Todo, not process or special process slave runout, depend on hardware */

    }

    if (RT_ERR_OK != drv_nic_rx_unregister(unit, L2NTFY_RX_HANDLER_PRIORITY, drv_l2ntfy_pkt_handler))
    {
        osal_printf("[%s,%d]: drv_nic_rx_unregister fail.\n", __FUNCTION__, __LINE__);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

static int32
_l2ntfy_isr_init(uint32 unit)
{
    int32  ret;
    uint32 index, value;

    lastRingID = 0;
    /* Reset  L2 notification counter */
    drv_l2ntfy_counter_clear(unit);

    /* Alloc memory for nBuf */
    drv_l2ntfy_sizeInfo_get(unit, &ringSize, &nBufSize);
    _l2ntfy_ntfyEntryLen_get(unit, &entrySize);
    nBuf = osal_alloc(ringSize * nBufSize * entrySize * sizeof(uint8));
    if (NULL == nBuf)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_L2), "out of memory");
        return RT_ERR_FAILED;
    }
    osal_memset(nBuf, 0, ringSize * nBufSize * entrySize * sizeof(uint8));
    if ((ret = osal_cache_memory_flush((uint32)nBuf, ringSize * nBufSize * entrySize * sizeof(uint8))) != RT_ERR_OK)
    {
        goto error;
    }
    nBuf = (uint8 *)UNCACHE(nBuf);

    /* Alloc memory for Description Ring */
    eventRing = osal_alloc(ringSize * sizeof(uint32));
    if (NULL == eventRing)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_L2), "out of memory");
        ret = RT_ERR_FAILED;
        goto error;
    }
    osal_memset(eventRing, 0, ringSize * sizeof(uint32));
    if ((ret = osal_cache_memory_flush((uint32)eventRing, ringSize * sizeof(uint32))) != RT_ERR_OK)
    {
        goto error;
    }
    eventRing = (uint32 *)UNCACHE(eventRing);

    /* Initial Description Ring */
    for(index = 0; index < ringSize; index ++)
    {
        if(index == ringSize -1)
            eventRing[index] = (uint32)(&nBuf[NTFY_RING_ENTRY_IDX(index)]) | (1 << 1) | L2NTFY_RING_OWNER_SWITCH;
        else
            eventRing[index] = (uint32)(&nBuf[NTFY_RING_ENTRY_IDX(index)]) | L2NTFY_RING_OWNER_SWITCH;
    }
    value = (uint32)(eventRing);
    _l2ntfy_ntfyBaseAddr_set(unit, value);

    /* Enable L2 notification interrupt */
    _l2ntfy_intrMask_set(unit, NIC_NTFY_DONE, ENABLED);
    _l2ntfy_intrMask_set(unit, NIC_NTFY_BUF_RUNOUT, ENABLED);
    _l2ntfy_intrMask_set(unit, NIC_NTFY_LOCALBUF_RUNOUT, ENABLED);

    return RT_ERR_OK;

error:
    if (NULL != nBuf)
        osal_free(nBuf);

    if (NULL != eventRing)
        osal_free(nBuf);

    return ret;
}

static int32
_l2ntfy_isr_unInit(uint32 unit)
{
    uint32 index,value;

    /* Disable L2 notification interrupt */
    _l2ntfy_intrMask_set(unit, NIC_NTFY_DONE, DISABLED);
    _l2ntfy_intrMask_set(unit, NIC_NTFY_BUF_RUNOUT, DISABLED);
    _l2ntfy_intrMask_set(unit, NIC_NTFY_LOCALBUF_RUNOUT, DISABLED);

    /* Initial Description Ring */
    for(index = 0; index < ringSize; index ++)
    {
        if(index == ringSize -1)
            eventRing[index] = (uint32)(&nBuf[NTFY_RING_ENTRY_IDX(index)]) | (1 << 1) | L2NTFY_RING_OWNER_SWITCH;
        else
            eventRing[index] = (uint32)(&nBuf[NTFY_RING_ENTRY_IDX(index)]) | L2NTFY_RING_OWNER_SWITCH;
    }
    /* Clear status */
    _l2ntfy_intrSts_set(unit, NIC_NTFY_BUF_RUNOUT, ENABLED);

    drv_l2ntfy_fifoEmptyStatus_get(unit, &value);
    while(L2NTFY_FIFO_NOT_EMPTY == value)
    {
        osal_time_mdelay(1);
        drv_l2ntfy_fifoEmptyStatus_get(unit, &value);
    }
    /* Clear status */
    _l2ntfy_intrSts_set(unit, NIC_NTFY_LOCALBUF_RUNOUT, ENABLED);

    /* Release memory for nBuf */
    nBuf = (uint8 *)CACHED(nBuf);
    osal_free(nBuf);

    /* Release memory for Description Ring */
    eventRing = (uint32 *)CACHED(eventRing);
    osal_free(eventRing);

    return RT_ERR_OK;
}

/* Function Name:
 *      drv_l2ntfy_init
 * Description:
 *      Init L2-notification driver of the specified device.
 * Input:
 *      unit        - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - Invalid unit id
 *      RT_ERR_CHIP_NOT_FOUND   - The chip can not found
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      Must initialize L2-notification module before calling any nic APIs.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
drv_l2ntfy_init(uint32 unit)
{
    int32 ret = RT_ERR_FAILED;

    RT_INIT_REENTRY_CHK(l2ntfy_init[unit]);
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID), RT_ERR_UNIT_ID);

    if (!L2NTFY_CHK(unit))
    {
        return RT_ERR_CHIP_NOT_FOUND;
    }

    if (L2NTFY_CTRL(unit).l2ntfy_init == NULL)
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }
    L2NTFY_CTRL(unit).l2ntfy_init(unit);

    if(TRUE == HWP_CASCADE_MODE())
    {
        masterUnitId = HWP_MY_UNIT_ID();
        slaveUnitId = HWP_CASCADE_SLAVE_UNIT_ID();

        /* Compatible */
        if((HWP_9300_FAMILY_ID(unit)) && ( (0x1 == HWP_CHIP_REV(unit)) || (0x2 == HWP_CHIP_REV(unit))))
        {
            L2NTFY_CTRL(unit).l2ntfy_dst_set(unit, L2NTFY_DST_PKT_TO_MASTER);
        }
        else
        {
            L2NTFY_CTRL(unit).l2ntfy_dst_set(unit, L2NTFY_DST_PKT_TO_LOCAL);
        }

        if(masterUnitId == unit)
        {
            if ((ret = _l2ntfy_pkt_init(unit)) != RT_ERR_OK)
            {
                return ret;
            }
        }
    }
    else
    {
        L2NTFY_CTRL(unit).l2ntfy_dst_set(unit, l2ntfy_dst);

        if (HWP_8390_50_FAMILY(unit))
            l2ntfy_dst = L2NTFY_DST_NIC;

        /* Initialize the L2 notification module */
        if(L2NTFY_DST_NIC == l2ntfy_dst)
        {
            if ((ret = _l2ntfy_isr_init(unit)) != RT_ERR_OK)
            {
                return ret;
            }
        }
        else if(L2NTFY_DST_PKT_TO_LOCAL == l2ntfy_dst)
        {
            if ((ret = _l2ntfy_pkt_init(unit)) != RT_ERR_OK)
            {
                return ret;
            }
        }
    }

    drv_l2ntfy_backPressureThresh_set(unit, L2_NTFY_BP_THR);

    OSAL_INIT_WORK(&workq, _l2ntfy_bhWQ, &workq);
    OSAL_INIT_LIST_HEAD(&ntfyListHead.list);
    OSAL_INIT_WORK(&runoutWorkq, _l2ntfy_runoutHandle_bhWQ, &runoutWorkq);
    OSAL_INIT_LIST_HEAD(&runOutListHead.list);


    /* set init flag to complete init */
    l2ntfy_init[unit] = INIT_COMPLETED;

    return RT_ERR_OK;
}

void _l2ntfy_bhWQ(osal_work_struct_arg_t *data)
{
    uint32  unit, i, j, val, num = 0, curMem_usage;
    ntfy_collectArrayList_t    *pEntry, *n;
    rtk_l2ntfy_usrEventArray_t      *pUserCollection = NULL;
    rtk_l2ntfy_eventEntry_t         *pEventArray = NULL;

    osal_list_for_each_entry_safe(pEntry, n, &ntfyListHead.list, list)
    {
        pUserCollection = osal_alloc(sizeof(rtk_l2ntfy_usrEventArray_t));
        if (pUserCollection == NULL)
        {
            RT_ERR(RT_ERR_FAILED, (MOD_L2), "L2 notification memory allocate failed");
            return;
        }

        if(L2NTFY_MODE_NIC == pEntry->blockMode)
        {
            RT_LOG(LOG_DEBUG, MOD_L2, "FUN:%s---LINE:%d,event alloc=%d\n", __FUNCTION__ , __LINE__,(pEntry->blockSize) * nBufSize);
            pEventArray = osal_alloc((pEntry->blockSize) * nBufSize * sizeof(rtk_l2ntfy_eventEntry_t));
        }
        else
        {
            num = pEntry->blockSize / entrySize;
            RT_LOG(LOG_DEBUG, MOD_L2, "FUN:%s---LINE:%d,entryNum=%d\n", __FUNCTION__ , __LINE__,num);
            pEventArray = osal_alloc(num * sizeof(rtk_l2ntfy_eventEntry_t));
        }

        if (pEventArray == NULL)
        {
            RT_ERR(RT_ERR_FAILED, (MOD_L2), "L2 notification memory allocate failed");
            osal_free(pUserCollection);
            return;
        }
        unit = pEntry->unit;
        pUserCollection->eventArray = pEventArray;

        if(L2NTFY_MODE_NIC == pEntry->blockMode)
        {
            for (i = 0; i < pEntry->blockSize; i++)
            {
                for (j = 0; j < nBufSize; j++)
                {
                    if (RT_ERR_OK == _l2ntfy_rawToEvent_cnvt(unit, &(pEntry->pNBufTmp[NTFY_ENTRY_IDX(i, j)]), &pEventArray[num]))
                    {
                        totalEventCount++;
                        num++;
                    }
                    else
                    {
                        /* Event should be continous */
                        break;
                    }
                }
            }
        }
        else if(L2NTFY_MODE_PKT == pEntry->blockMode)
        {
            for (i = 0; i < num; i++)
            {
                if (RT_ERR_OK == _l2ntfy_rawToEvent_cnvt(unit, &(pEntry->pNBufTmp[i*entrySize]), &pEventArray[i]))
                {
                    totalEventCount++;
                }
                else
                {
                    /* Event should be continous, event num should reculate */
                    num = i;
                    break;
                }
            }
        }
        pUserCollection->entryNum = num;
        num = 0;

        if(TRUE == ntfy_debug_flag)
        {
            osal_printf("FUN:%s---LINE:%d,entryNum=%d,totalEventCount=%d\n", __FUNCTION__ , __LINE__,pUserCollection->entryNum,totalEventCount);
            for(i=0; i<pUserCollection->entryNum; i++)
            {
                osal_printf("COPY_TO_LIST: type=%d, vid=%4d, mac=%2x:%2x:%2x:%2x:%2x:%2x, slp=%x\n",
                    pUserCollection->eventArray[i].type,
                    pUserCollection->eventArray[i].fidVid ,
                    pUserCollection->eventArray[i].mac.octet[0],
                    pUserCollection->eventArray[i].mac.octet[1],
                    pUserCollection->eventArray[i].mac.octet[2],
                    pUserCollection->eventArray[i].mac.octet[3],
                    pUserCollection->eventArray[i].mac.octet[4],
                    pUserCollection->eventArray[i].mac.octet[5],
                    pUserCollection->eventArray[i].slp);
            }
        }

        /* Sometimes, the data is processed by previous interrupt */
        if (pUserCollection->entryNum != 0)
        {
            if (ntfy_callback)
                ntfy_callback(unit, pUserCollection);
        }

        val = pEntry->blockMemSize;
        curMem_usage = osal_atomic_sub_return(val, &mem_usage);
        if(curMem_usage == 0 && osal_atomic_read(&mem_lock))
        {
            osal_atomic_set(&mem_lock, 0);

            if(L2NTFY_MODE_NIC == pEntry->blockMode)
            {
                /* isr_handler may mem_lock, so should be run by Manual*/
                drv_l2ntfy_isr_handler(unit, 0);
                /* Enable L2 notification interrupt if no runout interrupt */
                if((0 == osal_atomic_read(&bufRunoutIsrLock)) &&
                   (0 == osal_atomic_read(&localBufRunoutIsrLock)))
                {
                    _l2ntfy_intrMask_set(unit, NIC_NTFY_DONE, ENABLED);
                }
            }
            else
            {
                /* Start receive L2 notification if no runout interrupt */
                if((0 == osal_atomic_read(&pktDropIsrLock)) &&
                   (0 == osal_atomic_read(&localBufRunoutIsrLock)))
                {
                    _l2ntfy_pktCpuQueBwCtrlEnable_set(HWP_MY_UNIT_ID(), L2NTFY_PKT_CPU_QUEUE_PRI, DISABLED);
                }
            }
            releaseCnt++;
        }

        L2NTFY_SEM_LOCK(unit);
        osal_list_del(&pEntry->list);
        L2NTFY_SEM_UNLOCK(unit);
        osal_free(pEntry->pNBufTmp);
        osal_free(pEntry);
        osal_free(pEventArray);
        osal_free(pUserCollection);
    }

    return;
}

void _l2ntfy_runoutHandle_bhWQ(osal_work_struct_arg_t *data)
{
    ntfy_runOutHdlList_t        *pEntry, *n;
    uint32                      newMgcNum = 0;

    osal_list_for_each_entry_safe(pEntry, n, &runOutListHead.list, list)
    {
        if(L2_NOTIFY_BUF_NBUF != pEntry->runOutType)
        {
            /* Disable l2ntfy, LONGAN-1466 */
            if(TRUE == HWP_CASCADE_MODE())
            {
                drv_l2ntfy_enable_set(masterUnitId, DISABLED);
                drv_l2ntfy_enable_set(slaveUnitId, DISABLED);
                l2DBG("FUN:%s---LINE:%d,master=%d, slave=%d,runOutType=%d,L2ntfy set disabled\n", __FUNCTION__ , __LINE__,masterUnitId, slaveUnitId, pEntry->runOutType);
            }
            else
            {
                drv_l2ntfy_enable_set(pEntry->unit, DISABLED);
                l2DBG("FUN:%s---LINE:%d,unit=%d,runOutType=%d,L2ntfy set disabled\n", __FUNCTION__ , __LINE__,pEntry->unit,pEntry->runOutType);
            }

            /* Change magic num, should before reset */
            if(TRUE == HWP_CASCADE_MODE())
            {
                newMgcNum = (mgcNum + 1)% 0xFF;
                l2DBG("FUN:%s---LINE:%d,master=%d, slave=%d,runOutType=%d,new mgcNum=%d\n", __FUNCTION__ , __LINE__,masterUnitId, slaveUnitId, pEntry->runOutType,newMgcNum);
                drv_l2ntfy_magicNum_set(masterUnitId, newMgcNum);
                drv_l2ntfy_magicNum_set(slaveUnitId, newMgcNum);
            }
            else
            {
                newMgcNum = (mgcNum + 1)% 0xFF;
                l2DBG("FUN:%s---LINE:%d,unit=%d,runOutType=%d,new mgcNum=%d\n", __FUNCTION__ , __LINE__,pEntry->unit,pEntry->runOutType,newMgcNum);
                drv_l2ntfy_magicNum_set(pEntry->unit, newMgcNum);
            }

            /* Reset l2ntfy */
            if(TRUE == HWP_CASCADE_MODE())
            {
                drv_l2ntfy_reset(masterUnitId);
                drv_l2ntfy_reset(slaveUnitId);
                l2DBG("FUN:%s---LINE:%d,master=%d, slave=%d,runOutType=%d,reset\n", __FUNCTION__ , __LINE__,masterUnitId, slaveUnitId, pEntry->runOutType);
            }
            else
            {
                drv_l2ntfy_reset(pEntry->unit);
                l2DBG("FUN:%s---LINE:%d,unit=%d,runOutType=%d,reset\n", __FUNCTION__ , __LINE__,pEntry->unit,pEntry->runOutType);
            }

        }

        /* Callback customer API */
        if (runout_callback)
            runout_callback(pEntry->unit, pEntry->runOutType);

        if(L2_NOTIFY_BUF_NBUF != pEntry->runOutType)
        {
            /* Enable l2ntfy */
            if(TRUE == HWP_CASCADE_MODE())
            {
                drv_l2ntfy_enable_set(masterUnitId, ENABLED);
                drv_l2ntfy_enable_set(slaveUnitId, ENABLED);
                l2DBG("FUN:%s---LINE:%d,master=%d, slave=%d,runOutType=%d,L2ntfy enabled\n", __FUNCTION__ , __LINE__,masterUnitId, slaveUnitId, pEntry->runOutType);
            }
            else
            {
                drv_l2ntfy_enable_set(pEntry->unit, ENABLED);
                l2DBG("FUN:%s---LINE:%d,unit=%d,runOutType=%d,L2ntfy set enabled\n", __FUNCTION__ , __LINE__,pEntry->unit,pEntry->runOutType);
            }
        }

        L2NTFY_SEM_LOCK(pEntry->unit);
        osal_list_del(&pEntry->list);
        L2NTFY_SEM_UNLOCK(pEntry->unit);

        if(L2_NOTIFY_BUF_NBUF == pEntry->runOutType)
        {
            osal_atomic_set(&bufRunoutIsrLock, 0);
            /* Enable buffer interrupt */
            _l2ntfy_intrMask_set(HWP_MY_UNIT_ID(), NIC_NTFY_BUF_RUNOUT, ENABLED);
        }
        else if(L2_NOTIFY_BUF_ASIC_FIFO == pEntry->runOutType)
        {
            osal_atomic_set(&localBufRunoutIsrLock, 0);
            /* Enable local buffer interrupt */
            _l2ntfy_intrMask_set(HWP_MY_UNIT_ID(), NIC_NTFY_LOCALBUF_RUNOUT, ENABLED);
            if(TRUE == HWP_CASCADE_MODE())
            {
                /* Todo, not process or special process slave runout, depend on hardware */

            }
        }
        else if(L2_NOTIFY_PKT_MISS == pEntry->runOutType)
        {
            osal_atomic_set(&pktDropIsrLock, 0);
        }

        osal_free(pEntry);
    }

    return;
}

/* Function Name:
 *      drv_l2ntfy_register
 * Description:
 *      Register to receive callbacks for L2-notification events of the specified device.
 * Input:
 *      unit        - unit id
 *      cb          - pointer to a handler of received packets
 *      runout_cb   - application data returned with callback (can be null)
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - Invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_CHIP_NOT_FOUND   - The chip can not found
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
drv_l2ntfy_register(uint32 unit, drv_l2ntfy_cb_f cb, drv_l2ntfy_runoutcb_f runout_cb)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID), RT_ERR_UNIT_ID);

    L2NTFY_SEM_LOCK(unit);
    ntfy_callback = cb;
    runout_callback = runout_cb;
    L2NTFY_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      drv_l2ntfy_unregister
 * Description:
 *      Unregister to receive callbacks for L2-notification events of the specified device.
 * Input:
 *      unit        - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - Invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_CHIP_NOT_FOUND   - The chip can not found
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
drv_l2ntfy_unregister(uint32 unit)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID), RT_ERR_UNIT_ID);

    L2NTFY_SEM_LOCK(unit);
    ntfy_callback = NULL;
    runout_callback = NULL;
    L2NTFY_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

void dump_bytes(uint8 *p, int len)
{
    int         i;

    for (i = 0; i < len; i++)
    {
        if ((i%16) == 0)
            osal_printf("[%04X] ", i);

        osal_printf("%02X ", p[i]&0x000000FF);
        if ((i%16) == 15)
            osal_printf("\n");
    }
    osal_printf("\n");
}


/* Function Name:
 *      drv_l2ntfy_pkt_handler
 * Description:
 *      Event handler for L2-notification events of the specified device.
 * Input:
 *      unit        - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - Invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_CHIP_NOT_FOUND   - The chip can not found
 * Applicable:
 *      9300, 9310
 * Note:
 *      This function processes the L2-notification events before hand over to user's program.
 */

drv_nic_rx_t
drv_l2ntfy_pkt_handler(uint32 unit, drv_nic_pkt_t *pPacket, void *pCookie)
{
    uint32          pktUnit = 0;
    uint32          pktSeqId = 0;
    uint32          pktLength = 0;
    uint32          curSeqId = 0;
    uint32          ret = NIC_RX_NOT_HANDLED;
    ntfy_collectArrayList_t *pCllctAryList = NULL;
    uint32          blockMemSize, total, val;

#if defined(CONFIG_SDK_RTL9300)
    l2DBG("rxCpuTag: device %d  source_port %d pysical_port %d,length %u, proLocStart=%d",
        pPacket->rx_tag.dev_id, pPacket->rx_tag.source_port, pPacket->rx_tag.physical_rx_port, (uint32)pPacket->length, proLoc);
#endif

    if((TRUE == ntfy_debug_flag) && (pPacket->length < 64))
    {
        dump_bytes(pPacket->data, (uint32)pPacket->length);
    }

    if ((*(pPacket->data + proLoc) == 0x88)
        &&(*(pPacket->data + proLoc + 1) == 0x99)
        &&(*(pPacket->data + proLoc + 2) == 0x05))
    {
        l2DBG("L2 notification packet,pktCnt=%d.......[O]",pktCnt);
        pktCnt++;

#if defined(CONFIG_SDK_RTL9300)
        pktUnit = HAL_DEV_TO_UNIT_ID(pPacket->rx_tag.dev_id);
#endif

        /* Should drop l2ntfy when pkt memory was used up */
        if (osal_atomic_read(&mem_lock))
        {
            drv_l2ntfy_pktDrop_handler(pktUnit, L2_NOTIFY_PKT_MISS);
            ret = NIC_RX_HANDLED_OWNED;
            goto Err;
        }

        /* L2 notification packet Effectiveness */
        if(mgcNum == *(pPacket->data + proLoc + 3))
        {
            curSeqId = sqcId[pktUnit];
            sqcId[pktUnit] = (sqcId[pktUnit] + 1) % (sqcIdMax+1);
            pktSeqId = (*(pPacket->data + proLoc + 4) << 8) | *(pPacket->data + proLoc + 5);

            l2DBG("now unit =%d, Seq expect = %d, actual = %d \n",pktUnit, curSeqId, pktSeqId);
            if(curSeqId == pktSeqId)
            {
                pktLength = pPacket->length;
                blockMemSize = sizeof(ntfy_collectArrayList_t) + pktLength * sizeof(uint8);

                if ((val = osal_atomic_read(&mem_usage)) + blockMemSize > MAX_MEM_USAGE)
                {
                    osal_atomic_set(&mem_lock, 1);
                    memMaxCnt++;
                    l2DBG("FUN:%s---LINE:%d,val=%d\n", __FUNCTION__ , __LINE__,val);
                    /* Drop this pkt */
                    drv_l2ntfy_pktDrop_handler(pktUnit, L2_NOTIFY_PKT_MISS);

                    /* Queue rate limit */
                    _l2ntfy_pktCpuQueBwCtrlEnable_set(HWP_MY_UNIT_ID(), L2NTFY_PKT_CPU_QUEUE_PRI, ENABLED);

                    lockCnt++;

                    ret = NIC_RX_HANDLED_OWNED;
                    goto Err;
                }
                else
                {
                    pCllctAryList = osal_alloc(sizeof(ntfy_collectArrayList_t));
                    if (pCllctAryList == NULL)
                    {
                        RT_ERR(RT_ERR_FAILED, (MOD_L2), "L2 notification PKT memory allocate failed");
                        ret = NIC_RX_HANDLED_OWNED;
                        goto Err;
                    }
                    pCllctAryList->unit = pktUnit;
                    pCllctAryList->blockMode = L2NTFY_MODE_PKT;
                    pCllctAryList->blockSize = pktLength - (proLoc + 6);
                    pCllctAryList->pNBufTmp = osal_alloc(pCllctAryList->blockSize);
                    if (pCllctAryList->pNBufTmp == NULL)
                    {
                        RT_ERR(RT_ERR_FAILED, (MOD_L2), "L2 notificatio PTK memory allocate failed");
                        ret = NIC_RX_HANDLED_OWNED;
                        goto Err;
                    }
                    pCllctAryList->blockMemSize = blockMemSize;
                    total = osal_atomic_add_return(blockMemSize, &mem_usage);
                    RT_LOG(LOG_DEBUG, MOD_L2, "%s():%d  val:%d   total:%d\n", __FUNCTION__, __LINE__, blockMemSize, total);

                    osal_memcpy(pCllctAryList->pNBufTmp, pPacket->data + proLoc + 6, pCllctAryList->blockSize);

                    osal_list_add_tail(&pCllctAryList->list, &ntfyListHead.list);
                    osal_schedule_work(&workq);
                }
            }
            else
            {
                l2DBG("Drop L2 notification, Seq wrong, expect = %d, actual = %d \n",curSeqId , pktSeqId);
                drv_l2ntfy_pktDrop_handler(pktUnit, L2_NOTIFY_PKT_MISS);
            }

        }
        else
        {
            l2DBG("Unit %d Drop L2 notification, Magic wrong, expect = %x, actual = %x \n",pktUnit, mgcNum, *(pPacket->data + proLoc + 3));
        }
        ret = NIC_RX_HANDLED_OWNED;
    }
    else
    {
        l2DBG("L2 notification packet.......[X]");
        ret = NIC_RX_NOT_HANDLED;
    }

Err:
    if (pCllctAryList != NULL && pCllctAryList->pNBufTmp == NULL)
        osal_free(pCllctAryList);


    if(NIC_RX_HANDLED_OWNED == ret)
    {
#if defined(CONFIG_SDK_RTL9300)
        l2DBG("free rxCpuTag: device %d  source_port %d pysical_port %d", pPacket->rx_tag.dev_id, pPacket->rx_tag.source_port, pPacket->rx_tag.physical_rx_port);
#endif
        /* need to free the packet by using drv_nic_pkt_free() */
        drv_nic_pkt_free(unit, pPacket);
    }

    return ret;
}

/* Function Name:
 *      drv_l2ntfy_isr_handler
 * Description:
 *      Event handler for L2-notification events of the specified device.
 * Input:
 *      unit        - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - Invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_CHIP_NOT_FOUND   - The chip can not found
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      This function processes the L2-notification events before hand over to user's program.
 */
int32
drv_l2ntfy_isr_handler(uint32 unit, void *isr_param)
{
    uint32  ringID;
    uint32  usedRingCnt = 0, ringCnt = 0;
    ntfy_collectArrayList_t *pCllctAryList = NULL;
    uint32  blockMemSize, total, val;

    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID), RT_ERR_UNIT_ID);
    if (!L2NTFY_CHK(unit))
    {
        return RT_ERR_CHIP_NOT_FOUND;
    }

    l2DBG("lastRingID=%d,mem_lock=%d,mem_usage=%d\n", lastRingID, osal_atomic_read(&mem_lock), osal_atomic_read(&mem_usage));

    if (osal_atomic_read(&mem_lock))
        return RT_ERR_OK;

    for(ringID = lastRingID; usedRingCnt < ringSize; )
    {
        if((eventRing[ringID] & L2NTFY_RING_OWNER_MASK) != L2NTFY_RING_OWNER_CPU)
            break;

        ringID = (ringID + 1) % ringSize;
        usedRingCnt++;
    }
    RT_LOG(LOG_DEBUG, MOD_L2, "FUN:%s---LINE:%d,usedRingCnt=%d\n", __FUNCTION__ , __LINE__,usedRingCnt);

    if (usedRingCnt == 0)  /* Sometimes, the data is processed by previous interrupt */
    {
        return RT_ERR_OK;
    }

    blockMemSize = sizeof(ntfy_collectArrayList_t) + usedRingCnt * nBufSize * entrySize * sizeof(uint8);

    if ((val = osal_atomic_read(&mem_usage)) + blockMemSize > MAX_MEM_USAGE)
    {
        osal_atomic_set(&mem_lock, 1);

        memMaxCnt++;
        l2DBG("FUN:%s---LINE:%d,val=%d\n", __FUNCTION__ , __LINE__,val);

        /* Disable L2 notification interrupt */
        _l2ntfy_intrMask_set(unit, NIC_NTFY_DONE, DISABLED);
        lockCnt++;

        return RT_ERR_OK;
    }

    pCllctAryList = osal_alloc(sizeof(ntfy_collectArrayList_t));
    if (pCllctAryList == NULL)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_L2), "L2 notificatio ISR memory allocate failed");
        goto Err;
    }
    pCllctAryList->unit = unit;
    pCllctAryList->blockMode = L2NTFY_MODE_NIC;
    pCllctAryList->blockSize = usedRingCnt;
    pCllctAryList->pNBufTmp = osal_alloc(usedRingCnt * nBufSize * entrySize * sizeof(uint8));
    if (pCllctAryList->pNBufTmp == NULL)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_L2), "L2 notificatio ISR memory allocate failed");
        goto Err;
    }
    pCllctAryList->blockMemSize = blockMemSize;
    total = osal_atomic_add_return(blockMemSize, &mem_usage);
    l2DBG("%s():%d  copy from buf id:%d  len:%d\n", __FUNCTION__, __LINE__, NTFY_RING_ENTRY_IDX(lastRingID), usedRingCnt * nBufSize * entrySize * sizeof(uint8));

    if (lastRingID + usedRingCnt > ringSize)   /* If wrapped, osal_memcpy by two part */
    {
        osal_memcpy(pCllctAryList->pNBufTmp, &nBuf[NTFY_RING_ENTRY_IDX(lastRingID)], (ringSize - lastRingID) * nBufSize * entrySize * sizeof(uint8));
        osal_memcpy(pCllctAryList->pNBufTmp + (ringSize - lastRingID)*nBufSize*entrySize, &nBuf[0], (usedRingCnt - (ringSize - lastRingID)) * nBufSize * entrySize * sizeof(uint8));
    }
    else
        osal_memcpy(pCllctAryList->pNBufTmp, &nBuf[NTFY_RING_ENTRY_IDX(lastRingID)], usedRingCnt * nBufSize * entrySize * sizeof(uint8));

    for(ringID = lastRingID; ringCnt < usedRingCnt; ringCnt++)
    {
        if((eventRing[ringID] & 1) != L2NTFY_RING_OWNER_CPU)
            break;

        osal_memset(&nBuf[NTFY_RING_ENTRY_IDX(ringID)], 0, nBufSize * entrySize * sizeof(uint8));
        eventRing[ringID] |= L2NTFY_RING_OWNER_SWITCH;

        lastRingID = (lastRingID + 1) % ringSize;
        ringID = (ringID + 1) % ringSize;
    }

    osal_list_add_tail(&pCllctAryList->list, &ntfyListHead.list);
    osal_schedule_work(&workq);

    return RT_ERR_OK;


Err:
    if (pCllctAryList != NULL && pCllctAryList->pNBufTmp == NULL)
        osal_free(pCllctAryList);

    return RT_ERR_OK;
}


/* Function Name:
 *      drv_l2ntfy_pktDrop_handler
 * Description:
 *      Event handler for L2-notification runout events of the specified device.
 * Input:
 *      unit        - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - Invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_CHIP_NOT_FOUND   - The chip can not found
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 */
int32
drv_l2ntfy_pktDrop_handler(uint32 unit, rtk_l2ntfy_runOutType_t dropType)
{
    ntfy_runOutHdlList_t *pRunoutHdlList = NULL;

    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID), RT_ERR_UNIT_ID);
    RT_INIT_CHK(l2ntfy_init[unit]);

    pktDropCnt[unit]++;

    if(osal_atomic_read(&pktDropIsrLock))
    {
        return RT_ERR_OK;
    }
    osal_atomic_set(&pktDropIsrLock, 1);

    pRunoutHdlList = osal_alloc(sizeof(ntfy_runOutHdlList_t));
    if (pRunoutHdlList == NULL)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_L2), "L2 notification Runout memory allocate failed");
        return RT_ERR_OK;
    }
    pRunoutHdlList->unit = unit;
    pRunoutHdlList->runOutType = dropType;

    osal_list_add_tail(&pRunoutHdlList->list, &runOutListHead.list);
    osal_schedule_work(&runoutWorkq);

    return RT_ERR_OK;
}


/* Function Name:
 *      drv_l2ntfy_bufRunout_handler
 * Description:
 *      Event handler for L2-notification runout events of the specified device.
 * Input:
 *      unit        - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - Invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_CHIP_NOT_FOUND   - The chip can not found
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      None
 */
int32
drv_l2ntfy_bufRunout_handler(uint32 unit, void *isr_param)
{
    ntfy_runOutHdlList_t *pRunoutHdlList = NULL;

    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID), RT_ERR_UNIT_ID);
    RT_INIT_CHK(l2ntfy_init[unit]);

    _l2ntfy_intrMask_set(unit, NIC_NTFY_BUF_RUNOUT, DISABLED);
    runoutCnt++;

    if(osal_atomic_read(&bufRunoutIsrLock))
    {
        return RT_ERR_OK;
    }
    osal_atomic_set(&bufRunoutIsrLock, 1);

    pRunoutHdlList = osal_alloc(sizeof(ntfy_runOutHdlList_t));
    if (pRunoutHdlList == NULL)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_L2), "L2 notification Runout memory allocate failed");
        return RT_ERR_OK;
    }
    pRunoutHdlList->unit = unit;
    pRunoutHdlList->runOutType = L2_NOTIFY_BUF_NBUF;

    osal_list_add_tail(&pRunoutHdlList->list, &runOutListHead.list);
    osal_schedule_work(&runoutWorkq);

    return RT_ERR_OK;
}

/* Function Name:
 *      drv_l2ntfy_localBufRunout_handler
 * Description:
 *      Event handler for L2-notification ASIC's buffer runout events of the specified device.
 * Input:
 *      unit        - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - Invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_CHIP_NOT_FOUND   - The chip can not found
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      None
 */
int32
drv_l2ntfy_localBufRunout_handler(uint32 unit, void *isr_param)
{
    ntfy_runOutHdlList_t *pRunoutHdlList = NULL;

    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID), RT_ERR_UNIT_ID);
    RT_INIT_CHK(l2ntfy_init[unit]);

    _l2ntfy_intrMask_set(HWP_MY_UNIT_ID(), NIC_NTFY_LOCALBUF_RUNOUT, DISABLED);
    /* Disable local buffer interrupt */
    if(TRUE == HWP_CASCADE_MODE())
    {
        /* Todo, slave not process or special process, depend on hardware */
        //
    }

    local_runoutCnt++;

    if(osal_atomic_read(&localBufRunoutIsrLock))
    {
        return RT_ERR_OK;
    }
    osal_atomic_set(&localBufRunoutIsrLock, 1);

    pRunoutHdlList = osal_alloc(sizeof(ntfy_runOutHdlList_t));
    if (pRunoutHdlList == NULL)
    {
        RT_ERR(RT_ERR_FAILED, (MOD_L2), "L2 notification Runout memory allocate failed");
        return RT_ERR_OK;
    }
    pRunoutHdlList->unit = unit;
    pRunoutHdlList->runOutType = L2_NOTIFY_BUF_ASIC_FIFO;

    osal_list_add_tail(&pRunoutHdlList->list, &runOutListHead.list);
    osal_schedule_work(&runoutWorkq);

    return RT_ERR_OK;
}

/* Function Name:
 *      drv_l2ntfy_enable_get
 * Description:
 *      Get the enable state of L2-notification mechanism.
 * Input:
 *      unit    - unit id
 * Output:
 *      pEnable - enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - Invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_CHIP_NOT_FOUND   - The chip can not found
 *      RT_ERR_NULL_POINTER     - Input parameter may be null pointer
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
drv_l2ntfy_enable_get(uint32 unit, rtk_enable_t *pEnable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID), RT_ERR_UNIT_ID);
    RT_PARAM_CHK(NULL == pEnable, RT_ERR_NULL_POINTER);

    if (!L2NTFY_CHK(unit))
    {
        return RT_ERR_CHIP_NOT_FOUND;
    }

    if (L2NTFY_CTRL(unit).l2ntfy_enable_get == NULL)
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    return L2NTFY_CTRL(unit).l2ntfy_enable_get(unit, pEnable);
}

/* Function Name:
 *      drv_l2ntfy_enable_set
 * Description:
 *      Set the enable state of L2-notification mechanism.
 * Input:
 *      unit   - unit id
 *      enable - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - Invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_CHIP_NOT_FOUND   - The chip can not found
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
drv_l2ntfy_enable_set(uint32 unit, rtk_enable_t enable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID), RT_ERR_UNIT_ID);

    if (!L2NTFY_CHK(unit))
    {
        return RT_ERR_CHIP_NOT_FOUND;
    }

    if (L2NTFY_CTRL(unit).l2ntfy_enable_set == NULL)
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    return L2NTFY_CTRL(unit).l2ntfy_enable_set(unit, enable);
}

/* Function Name:
 *      drv_l2ntfy_backPressureThresh_get
 * Description:
 *      Get L2-notification back-pressure threshold of the specified device.
 * Input:
 *      unit    - unit id
 * Output:
 *      pThresh - threshold
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - Invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_CHIP_NOT_FOUND   - The chip can not found
 *      RT_ERR_NULL_POINTER     - Input parameter may be null pointer
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      (1) Back-pressure is a mechanism to avoid ASIC's notification FIFO running out
 *      (2) Back-pressure only suppresses the aging out event when the notification FIFO is over the
 *          specified threshold.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
drv_l2ntfy_backPressureThresh_get(uint32 unit, uint32 *pThresh)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID), RT_ERR_UNIT_ID);
    RT_PARAM_CHK(NULL == pThresh, RT_ERR_NULL_POINTER);

    if (!L2NTFY_CHK(unit))
    {
        return RT_ERR_CHIP_NOT_FOUND;
    }

    if (L2NTFY_CTRL(unit).l2ntfy_backPressureThresh_get == NULL)
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    return L2NTFY_CTRL(unit).l2ntfy_backPressureThresh_get(unit, pThresh);
}

/* Function Name:
 *      drv_l2ntfy_backPressureThresh_set
 * Description:
 *      Set L2-notification back-pressure threshold of the specified device.
 * Input:
 *      unit    - unit id
 *      thresh  - threshold value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - Invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_CHIP_NOT_FOUND   - The chip can not found
 *      RT_ERR_OUT_OF_RANGE     - Input parameter out of range
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      (1) Back-pressure is a mechanism to avoid ASIC's notification FIFO running out
 *      (2) Back-pressure only suppresses the aging out event when the notification FIFO is over the
 *          specified threshold.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
drv_l2ntfy_backPressureThresh_set(uint32 unit, uint32 thresh)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID), RT_ERR_UNIT_ID);

    if (!L2NTFY_CHK(unit))
    {
        return RT_ERR_CHIP_NOT_FOUND;
    }

    if (L2NTFY_CTRL(unit).l2ntfy_backPressureThresh_set == NULL)
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    return L2NTFY_CTRL(unit).l2ntfy_backPressureThresh_set(unit, thresh);
}

/* Function Name:
 *      drv_l2ntfy_notificationEventEnable_get
 * Description:
 *      Get L2-notification event enable configuration of the specified device.
 * Input:
 *      unit    - unit id
 *      event   - L2-notification type
 * Output:
 *      pEnable - enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - Invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_CHIP_NOT_FOUND   - The chip can not found
 *      RT_ERR_NULL_POINTER     - Input parameter may be null pointer
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      For 8390, only suspend learn and linkdown flush types support enable/disable state configuration.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
drv_l2ntfy_notificationEventEnable_get(uint32 unit, rtk_l2ntfy_event_t event, rtk_enable_t *pEnable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID), RT_ERR_UNIT_ID);
    RT_PARAM_CHK(NULL == pEnable, RT_ERR_NULL_POINTER);

    if (!L2NTFY_CHK(unit))
    {
        return RT_ERR_CHIP_NOT_FOUND;
    }

    if (L2NTFY_CTRL(unit).l2ntfy_notificationEventEnable_get == NULL)
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    return L2NTFY_CTRL(unit).l2ntfy_notificationEventEnable_get(unit, event, pEnable);
}

/* Function Name:
 *      drv_l2ntfy_notificationEventEnable_set
 * Description:
 *      Set L2-notification event configuration of the specified device.
 * Input:
 *      unit    - unit id
 *      event   - L2-notification type
 *      enable  - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - Invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_CHIP_NOT_FOUND   - The chip can not found
 *      RT_ERR_INPUT            - Invalid input parameter
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      For 8390, only suspend learn and linkdown flush types support enable/disable state configuration.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
drv_l2ntfy_notificationEventEnable_set(uint32 unit, rtk_l2ntfy_event_t event, rtk_enable_t enable)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID), RT_ERR_UNIT_ID);

    if (!L2NTFY_CHK(unit))
    {
        return RT_ERR_CHIP_NOT_FOUND;
    }

    if (L2NTFY_CTRL(unit).l2ntfy_notificationEventEnable_set == NULL)
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    return L2NTFY_CTRL(unit).l2ntfy_notificationEventEnable_set(unit, event, enable);
}

/* Function Name:
 *      drv_l2ntfy_fifoEmptyStatus_get
 * Description:
 *      Get the empty status of ASIC's l2-notification FIFO in specified device.
 * Input:
 *      unit    - unit id
 *      event   - L2-notification type
 * Output:
 *      pEnable - enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - Invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_CHIP_NOT_FOUND   - The chip can not found
 *      RT_ERR_NULL_POINTER     - Input parameter may be null pointer
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      If L2-notification is switched to disable state, uses this API to make sure that ASIC's FIFO is empty
 *      before clean up memory.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
drv_l2ntfy_fifoEmptyStatus_get(uint32 unit, rtk_l2ntfy_fifoStatus_t *pStatus)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID), RT_ERR_UNIT_ID);
    RT_PARAM_CHK(NULL == pStatus, RT_ERR_NULL_POINTER);

    if (!L2NTFY_CHK(unit))
    {
        return RT_ERR_CHIP_NOT_FOUND;
    }

    if (L2NTFY_CTRL(unit).l2ntfy_fifoEmptyStatus_get == NULL)
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    return L2NTFY_CTRL(unit).l2ntfy_fifoEmptyStatus_get(unit, pStatus);
}

/* Function Name:
 *      drv_l2ntfy_counter_dump
 * Description:
 *      Dump the counter of L2-notification event happened in specified device.
 * Input:
 *      unit    - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - Invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_CHIP_NOT_FOUND   - The chip can not found
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
drv_l2ntfy_counter_dump(uint32 unit)
{
    int32               ret = RT_ERR_FAILED;
    uint32              i;
    rtk_l2ntfy_dst_t    dst;

    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID), RT_ERR_UNIT_ID);

    /* Check init state */
    RT_INIT_CHK(l2ntfy_init[unit]);
    RT_ERR_CHK(drv_l2ntfy_dst_get(unit, &dst), ret);

    L2NTFY_SEM_LOCK(unit);
    osal_printf("totalEventCount    : %u\n", totalEventCount);
    osal_printf("local_runoutCnt    : %u\n", local_runoutCnt);
    osal_printf("memMaxCnt          : %u\n", memMaxCnt);
    osal_printf("mem_usage:%d  lockCnt:%d releaseCnt:%d\n", osal_atomic_read(&mem_usage), lockCnt, releaseCnt);
    if(L2NTFY_DST_NIC == dst)
    {
        osal_printf("runoutCnt          : %u\n", runoutCnt);
    }
    else
    {
        osal_printf("totalL2NtfPktCount : %u\n", pktCnt);
        for(i = 0; i < RTK_MAX_NUM_OF_UNIT; i++)
        {
            if(pktDropCnt[i] != 0)
                osal_printf("unit%2d pktDrop: %u\n", i, pktDropCnt[i]);
        }
    }
    for(i = 0; i < RTK_MAX_NUM_OF_UNIT; i++)
    {
        if(l2ntfyResetCnt[i] != 0)
            osal_printf("unit%2d reset: %u\n", i, l2ntfyResetCnt[i]);
    }

    L2NTFY_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      drv_l2ntfy_counter_clear
 * Description:
 *      Clear the counter of L2-notification of the specified device.
 * Input:
 *      unit    - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - Invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_CHIP_NOT_FOUND   - The chip can not found
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
drv_l2ntfy_counter_clear(uint32 unit)
{
    uint32      i;

    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID), RT_ERR_UNIT_ID);

    L2NTFY_SEM_LOCK(unit);
    totalEventCount = 0;
    runoutCnt = 0;
    local_runoutCnt = 0;
    for(i = 0; i < RTK_MAX_NUM_OF_UNIT; i++)
    {
        pktDropCnt[i] = 0;
    }
    pktCnt = 0;
    L2NTFY_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      drv_l2ntfy_dst_get
 * Description:
 *      Get the destination of L2-notification mechanism.
 * Input:
 *      unit    - unit id
 * Output:
 *      pDst    - destination of L2-notification mechanism
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - Invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_CHIP_NOT_FOUND   - The chip can not found
 *      RT_ERR_NULL_POINTER     - Input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      L2NTFY_DST_PKT_TO_LOCAL, L2NTFY_DST_PKT_TO_MASTER and L2NTFY_DST_PKT_TO_ALL are all in the form of packet,
 *      and L2NTFY_DST_NIC in the form of interrupt directly.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
drv_l2ntfy_dst_get(uint32 unit, rtk_l2ntfy_dst_t *pDst)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID), RT_ERR_UNIT_ID);
    RT_PARAM_CHK(NULL == pDst, RT_ERR_NULL_POINTER);

    if (!L2NTFY_CHK(unit))
    {
        return RT_ERR_CHIP_NOT_FOUND;
    }

    if (L2NTFY_CTRL(unit).l2ntfy_dst_get == NULL)
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    return L2NTFY_CTRL(unit).l2ntfy_dst_get(unit, pDst);
}

/* Function Name:
 *      drv_l2ntfy_dst_set
 * Description:
 *      Set the destination of L2-notification mechanism.
 * Input:
 *      unit    - unit id
 *      dst     - destination of L2-notification mechanism
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - Invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_CHIP_NOT_FOUND   - The chip can not found
 * Applicable:
 *      9300, 9310
 * Note:
 *      L2NTFY_DST_PKT_TO_LOCAL, L2NTFY_DST_PKT_TO_MASTER and L2NTFY_DST_PKT_TO_ALL are all in the form of packet,
 *      and L2NTFY_DST_NIC in the form of interrupt directly.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
drv_l2ntfy_dst_set(uint32 unit, rtk_l2ntfy_dst_t dst)
{
    int32               ret;
    rtk_l2ntfy_dst_t    org_dst;

    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID), RT_ERR_UNIT_ID);

    if (!L2NTFY_CHK(unit))
    {
        return RT_ERR_CHIP_NOT_FOUND;
    }

    if (L2NTFY_CTRL(unit).l2ntfy_dst_set == NULL)
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if ((TRUE == HWP_CASCADE_MODE()) && (L2NTFY_DST_NIC == dst))
    {
        return RT_ERR_TYPE;
    }

    L2NTFY_SEM_LOCK(unit);
    drv_l2ntfy_dst_get(unit, &org_dst);
    L2NTFY_CTRL(unit).l2ntfy_dst_set(unit, dst);

    /* Should action when change mode, no action when slave unit in cascade */
    if(org_dst != dst)
    {
        if((FALSE == HWP_CASCADE_MODE()) || (unit == masterUnitId))
        {
            if((L2NTFY_DST_PKT_TO_LOCAL == org_dst)
               || (L2NTFY_DST_PKT_TO_MASTER == org_dst))
            {
                RT_ERR_CHK(_l2ntfy_pkt_unInit(unit), ret);
            }

            if(L2NTFY_DST_NIC == org_dst)
            {
                RT_ERR_CHK(_l2ntfy_isr_unInit(unit), ret);
            }

            if((L2NTFY_DST_PKT_TO_LOCAL == dst)
               || (L2NTFY_DST_PKT_TO_MASTER == dst))
            {
                RT_ERR_CHK(drv_nic_rx_register(unit, L2NTFY_RX_HANDLER_PRIORITY, drv_l2ntfy_pkt_handler, NULL, 0), ret);
                L2NTFY_CTRL(unit).l2ntfy_reset(unit);
                RT_ERR_CHK(_l2ntfy_pkt_init(unit), ret);
            }

            if(L2NTFY_DST_NIC == dst)
            {
                RT_ERR_CHK(_l2ntfy_isr_init(unit), ret);
            }
        }
    }

    L2NTFY_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      drv_l2ntfy_reset
 * Description:
 *      Reset the local buffer & state machine of L2-notification
 * Input:
 *      unit   - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - Invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_CHIP_NOT_FOUND   - The chip can not found
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 */
int32
drv_l2ntfy_reset(uint32 unit)
{
    int32               ret;
    rtk_l2ntfy_dst_t    now_dst;

    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID), RT_ERR_UNIT_ID);

    if (!L2NTFY_CHK(unit))
    {
        return RT_ERR_CHIP_NOT_FOUND;
    }

    if (L2NTFY_CTRL(unit).l2ntfy_reset == NULL)
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    drv_l2ntfy_dst_get(unit, &now_dst);

    /* Initialize the L2 notification module */
    if((FALSE == HWP_CASCADE_MODE()) || (unit == masterUnitId))
    {
        if((L2NTFY_DST_PKT_TO_LOCAL == now_dst)
            || (L2NTFY_DST_PKT_TO_MASTER == now_dst))
        {
            RT_ERR_CHK(_l2ntfy_pkt_unInit(unit), ret);
            RT_ERR_CHK(drv_nic_rx_register(unit, L2NTFY_RX_HANDLER_PRIORITY, drv_l2ntfy_pkt_handler, NULL, 0), ret);
            RT_ERR_CHK(_l2ntfy_pkt_init(unit), ret);
        }
        else if(L2NTFY_DST_NIC == now_dst)
        {
            RT_ERR_CHK(_l2ntfy_isr_unInit(unit), ret);
            RT_ERR_CHK(_l2ntfy_isr_init(unit), ret);
        }
    }

    l2ntfyResetCnt[unit]++;

    return L2NTFY_CTRL(unit).l2ntfy_reset(unit);
}

/* Function Name:
 *      drv_l2ntfy_iTag_get
 * Description:
 *      Get the configuration of inner tag.
 * Input:
 *      unit        - unit id
 * Output:
 *      pITagCfg    - pointer to inner tag configuration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - Invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_CHIP_NOT_FOUND   - The chip can not found
 *      RT_ERR_NULL_POINTER     - Input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 */
int32
drv_l2ntfy_iTag_get(uint32 unit, rtk_l2ntfy_iTagCfg_t *pITagCfg)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID), RT_ERR_UNIT_ID);
    RT_PARAM_CHK(NULL == pITagCfg, RT_ERR_NULL_POINTER);

    if (!L2NTFY_CHK(unit))
    {
        return RT_ERR_CHIP_NOT_FOUND;
    }

    if (L2NTFY_CTRL(unit).l2ntfy_iTag_get == NULL)
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    return L2NTFY_CTRL(unit).l2ntfy_iTag_get(unit, pITagCfg);
}

/* Function Name:
 *      drv_l2ntfy_iTag_set
 * Description:
 *      Set the configuration of inner tag.
 * Input:
 *      unit        - unit id
 *      pITagCfg    - pointer to inner tag configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - Invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_CHIP_NOT_FOUND   - The chip can not found
 * Applicable:
 *      9300, 9310
 * Note:
 *      Used only in packet mode
 */
int32
drv_l2ntfy_iTag_set(uint32 unit, rtk_l2ntfy_iTagCfg_t *pITagCfg)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID), RT_ERR_UNIT_ID);
    RT_PARAM_CHK(NULL == pITagCfg, RT_ERR_NULL_POINTER);

    if (!L2NTFY_CHK(unit))
    {
        return RT_ERR_CHIP_NOT_FOUND;
    }

    if (L2NTFY_CTRL(unit).l2ntfy_iTag_set == NULL)
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    return L2NTFY_CTRL(unit).l2ntfy_iTag_set(unit, pITagCfg);
}

/* Function Name:
 *      drv_l2ntfy_magicNum_get
 * Description:
 *      Get the magic number of L2-notification.
 * Input:
 *      unit    - unit id
 * Output:
 *      pMagicNumable - L2-notification magic number
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - Invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_CHIP_NOT_FOUND   - The chip can not found
 *      RT_ERR_NULL_POINTER     - Input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      Used only in packet mode
 */
int32
drv_l2ntfy_magicNum_get(uint32 unit, uint32 *pMagicNum)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID), RT_ERR_UNIT_ID);
    RT_PARAM_CHK(NULL == pMagicNum, RT_ERR_NULL_POINTER);

    if (!L2NTFY_CHK(unit))
    {
        return RT_ERR_CHIP_NOT_FOUND;
    }

    if (L2NTFY_CTRL(unit).l2ntfy_magicNum_get == NULL)
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    return L2NTFY_CTRL(unit).l2ntfy_magicNum_get(unit, pMagicNum);
}

/* Function Name:
 *      drv_l2ntfy_magicNum_set
 * Description:
 *      Set the mode of L2-notification mechanism.
 * Input:
 *      unit   - unit id
 *      magicNumable - L2-notification magic number
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - Invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_CHIP_NOT_FOUND   - The chip can not found
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 */
int32
drv_l2ntfy_magicNum_set(uint32 unit, uint32 magicNum)
{
    int32   ret = RT_ERR_FAILED;

    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID), RT_ERR_UNIT_ID);

    if (!L2NTFY_CHK(unit))
    {
        return RT_ERR_CHIP_NOT_FOUND;
    }

    if (L2NTFY_CTRL(unit).l2ntfy_magicNum_set == NULL)
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if ((ret = L2NTFY_CTRL(unit).l2ntfy_magicNum_set(unit, magicNum)) == RT_ERR_OK)
    {
        mgcNum = magicNum;
    }

    return ret;
}

/* Function Name:
 *      drv_l2ntfy_macAddr_get
 * Description:
 *      Get the MAC address of notification packet.
 * Input:
 *      unit    - unit id
 *      type    - MAC address type
 * Output:
 *      pMac    - pointer to MAC address
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 */
int32
drv_l2ntfy_macAddr_get(uint32 unit, rtk_l2ntfy_addrType_t type, rtk_mac_t *pMac)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID), RT_ERR_UNIT_ID);
    RT_PARAM_CHK(NULL == pMac, RT_ERR_NULL_POINTER);

    if (!L2NTFY_CHK(unit))
    {
        return RT_ERR_CHIP_NOT_FOUND;
    }

    if (L2NTFY_CTRL(unit).l2ntfy_macAddr_get == NULL)
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    return L2NTFY_CTRL(unit).l2ntfy_macAddr_get(unit, type, pMac);
}

/* Function Name:
 *      drv_l2ntfy_macAddr_set
 * Description:
 *      Set the MAC address of notification packet.
 * Input:
 *      unit    - unit id
 *      type    - MAC address type
 *      pMac    - pointer to MAC address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 */
int32
drv_l2ntfy_macAddr_set(uint32 unit, rtk_l2ntfy_addrType_t type, rtk_mac_t *pMac)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID), RT_ERR_UNIT_ID);
    RT_PARAM_CHK(NULL == pMac, RT_ERR_NULL_POINTER);

    if (!L2NTFY_CHK(unit))
    {
        return RT_ERR_CHIP_NOT_FOUND;
    }

    if (L2NTFY_CTRL(unit).l2ntfy_macAddr_set == NULL)
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    return L2NTFY_CTRL(unit).l2ntfy_macAddr_set(unit, type, pMac);
}

/* Function Name:
 *      drv_l2ntfy_maxEvent_get
 * Description:
 *      Get the maximum event number of a notification packet.
 * Input:
 *      unit    - unit id
 * Output:
 *      pNum    - pointer to maximum event number
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 */
int32
drv_l2ntfy_maxEvent_get(uint32 unit, uint32 *pNum)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID), RT_ERR_UNIT_ID);
    RT_PARAM_CHK(NULL == pNum, RT_ERR_NULL_POINTER);

    if (!L2NTFY_CHK(unit))
    {
        return RT_ERR_CHIP_NOT_FOUND;
    }

    if (L2NTFY_CTRL(unit).l2ntfy_maxEvent_get == NULL)
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    return L2NTFY_CTRL(unit).l2ntfy_maxEvent_get(unit, pNum);
}

/* Function Name:
 *      drv_l2ntfy_maxEvent_set
 * Description:
 *      Set the maximum event number of a notification packet.
 * Input:
 *      unit    - unit id
 *      num     - maximum event number
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 */
int32
drv_l2ntfy_maxEvent_set(uint32 unit, uint32 num)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID), RT_ERR_UNIT_ID);

    if (!L2NTFY_CHK(unit))
    {
        return RT_ERR_CHIP_NOT_FOUND;
    }

    if (L2NTFY_CTRL(unit).l2ntfy_maxEvent_set == NULL)
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    return L2NTFY_CTRL(unit).l2ntfy_maxEvent_set(unit, num);
}

/* Function Name:
 *      drv_l2ntfy_timeout_get
 * Description:
 *      Get the timeout value that a notification packet is formed.
 * Input:
 *      unit        - unit id
 *      mode        - the notification mode
 * Output:
 *      pTimeout    - pointer to timeout value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 */
int32
drv_l2ntfy_timeout_get(uint32 unit, rtk_l2ntfy_mode_t mode, uint32 *pTimeout)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID), RT_ERR_UNIT_ID);
    RT_PARAM_CHK(NULL == pTimeout, RT_ERR_NULL_POINTER);

    if (!L2NTFY_CHK(unit))
    {
        return RT_ERR_CHIP_NOT_FOUND;
    }

    if (L2NTFY_CTRL(unit).l2ntfy_timeout_get == NULL)
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    return L2NTFY_CTRL(unit).l2ntfy_timeout_get(unit, mode, pTimeout);
}

/* Function Name:
 *      drv_l2ntfy_timeout_set
 * Description:
 *      Set the timeout value that a notification packet is formed.
 * Input:
 *      unit        - unit id
 *      mode        - the notification mode
 *      timeout     - timeout value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 */
int32
drv_l2ntfy_timeout_set(uint32 unit, rtk_l2ntfy_mode_t mode, uint32 timeout)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID), RT_ERR_UNIT_ID);

    if (!L2NTFY_CHK(unit))
    {
        return RT_ERR_CHIP_NOT_FOUND;
    }

    if (L2NTFY_CTRL(unit).l2ntfy_timeout_set == NULL)
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    return L2NTFY_CTRL(unit).l2ntfy_timeout_set(unit, mode, timeout);
}

int32
drv_l2ntfy_sizeInfo_get(uint32 unit, uint32 *ringSize, uint32 *nBufSize)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID), RT_ERR_UNIT_ID);
    RT_PARAM_CHK(NULL == ringSize, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(NULL == nBufSize, RT_ERR_NULL_POINTER);

    if (!L2NTFY_CHK(unit))
    {
        return RT_ERR_CHIP_NOT_FOUND;
    }

    if (L2NTFY_CTRL(unit).l2ntfy_sizeInfo_get == NULL)
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    return L2NTFY_CTRL(unit).l2ntfy_sizeInfo_get(unit, ringSize, nBufSize);
}

/* Function Name:
 *      drv_l2ntfy_debug_get
 * Description:
 *      Get L2-notification debug flags of the specified device.
 * Input:
 *      unit    - unit id
 * Output:
 *      pFlags  - L2-notification debug flags
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - Invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_CHIP_NOT_FOUND   - The chip can not found
 *      RT_ERR_NULL_POINTER     - Input parameter may be null pointer
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      None
 */
int32
drv_l2ntfy_debug_get(uint32 unit, uint32 *pFlags)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID), RT_ERR_UNIT_ID);
    RT_PARAM_CHK(NULL == pFlags, RT_ERR_NULL_POINTER);

    RT_INIT_CHK(l2ntfy_init[unit]);

    L2NTFY_SEM_LOCK(unit);
    *pFlags = ntfy_debug_flag;
    L2NTFY_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      drv_l2ntfy_debug_set
 * Description:
 *      Set L2-notification debug flags of the specified device.
 * Input:
 *      unit    - unit id
 *      flags - NIC debug flags
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - Invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_CHIP_NOT_FOUND   - The chip can not found
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      None
 */
int32
drv_l2ntfy_debug_set(uint32 unit, uint32 flags)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID), RT_ERR_UNIT_ID);
    RT_INIT_CHK(l2ntfy_init[unit]);

    if ((flags & DEBUG_STOP_RELEASE_BIT) == 0 && (ntfy_debug_flag & DEBUG_STOP_RELEASE_BIT))
    {
        ntfy_debug_flag = flags;
        /* Add debug method here */
    }

    ntfy_debug_flag = flags;

    return RT_ERR_OK;
}

/* Function Name:
 *      drv_l2ntfy_counter_dump
 * Description:
 *      Dump the content of L2-notification event ring and buffer of the specified device.
 * Input:
 *      unit    - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - Invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_CHIP_NOT_FOUND   - The chip can not found
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      None
 */
int32
drv_l2ntfy_event_dump(uint32 unit)
{
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID), RT_ERR_UNIT_ID);
    RT_INIT_CHK(l2ntfy_init[unit]);

    osal_printf("Current ringID:%u \n", lastRingID);

    return RT_ERR_OK;
}

