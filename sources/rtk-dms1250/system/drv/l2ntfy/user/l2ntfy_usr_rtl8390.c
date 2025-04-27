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
 * $Revision$
 * $Date$
 *
 * Purpose : Definition those public NIC(Network Interface Controller) APIs and
 *           its data type in the SDK.
 *
 * Feature : The file have include the following module
 *           1) L2-Notification
 *
 */

/*
 * Include Files
 */
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <osal/cache.h>
#include <common/error.h>
#include <common/debug/rt_log.h>
#include <drv/l2ntfy/l2ntfy.h>
#include <private/drv/l2ntfy/l2ntfy_rtl8390.h>
#include <private/drv/swcore/swcore_rtl8390.h>
#include <common/util/rt_util_time.h>
#include <rtcore/rtcore.h>
#include <rtcore/user/rtcore_drv_usr.h>
#include <osal/lib.h>
#include <ioal/mem32.h>
#include <osal/sem.h>
#include <osal/thread.h>
#include <rtk/l2.h>
#include <ioal/ioal_init.h>


#define L2NTFY_RING_ADDR_KRN2USR(krn_addr)    (((uint32)krn_addr - (uint32)KRNVIRT(RTL8390_L2NTFY_RING_PHYS_BASE)) + l2ntfy_ring_base)
#define L2NTFY_RING_ADDR_USR2KRN(usr_addr)    (((uint32)usr_addr - l2ntfy_ring_base) + (uint32)KRNVIRT(RTL8390_L2NTFY_RING_PHYS_BASE))
#define L2NTFY_BUF_ADDR_KRN2USR(krn_addr)    (((uint32)krn_addr - (uint32)KRNVIRT(RTL8390_L2NTFY_BUF_PHYS_BASE)) + l2ntfy_buf_base)
#define L2NTFY_BUF_ADDR_USR2KRN(usr_addr)    (((uint32)usr_addr - l2ntfy_buf_base) + (uint32)KRNVIRT(RTL8390_L2NTFY_BUF_PHYS_BASE))

#define DEBUG_STOP_RELEASE_BIT                  (1 << L2NTFY_DEBUG_STOP_RELEASE)    /* 0x00000001 */
#define DEBUG_ATS_TEST                          (1 << L2NTFY_DEBUG_ATS_TEST)        /* 0x00000002 */

extern uint32       l2ntfy_ring_base;
extern uint32       l2ntfy_buf_base;

uint32 l2ntfy_ring_vir_addr;
uint32 l2ntfy_buf_vir_addr;
uint32 l2ntfy_usr_vir_addr;

//static rtk_l2ntfy_usrEventArray_t       _atsArray;      /* For ATS test */
static l2ntfy_nBuf_8390_t               *nBuf = NULL;
static uint32                           totalEventCount = 0;
static uint32                           runoutCnt = 0;
static uint32                           ntfy_debug_flag;
static uint32                           *eventRing = NULL;
static uint32                           lastRingID = 0;
static uint32                           curPos = 0;

static uint32                           l2ntfy_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};
static drv_l2ntfy_cb_f                  ntfy_callback = NULL;
static drv_l2ntfy_runoutcb_f            runout_callback = NULL;


static osal_mutex_t     l2ntfy_sem[RTK_MAX_NUM_OF_UNIT];
/*
 * Macro Definition
 */
/* semaphore handling */
#define L2NTFY_SEM_LOCK(unit)    \
do {\
    if (osal_sem_mutex_take(l2ntfy_sem[unit], OSAL_SEM_WAIT_FOREVER) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_LOCK_FAILED, (MOD_NIC), "semaphore lock failed");\
        return RT_ERR_SEM_LOCK_FAILED;\
    }\
} while(0)
#define L2NTFY_SEM_UNLOCK(unit)   \
do {\
    if (osal_sem_mutex_give(l2ntfy_sem[unit]) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_UNLOCK_FAILED, (MOD_NIC), "semaphore unlock failed");\
        return RT_ERR_SEM_UNLOCK_FAILED;\
    }\
} while(0)


/*
 * Function Declaration
 */
int32
r8390_l2ntfy_init(uint32 unit)
{
    int32                   ret = RT_ERR_FAILED;
    uint32                  i, j, value;
    uint32                  *pRing;
    l2ntfy_nBuf_8390_t      *pNbuf;


    /* create semaphore */
    l2ntfy_sem[unit] = osal_sem_mutex_create();
    if (0 == l2ntfy_sem[unit])
    {
        RT_ERR(RT_ERR_FAILED, (MOD_NIC), "semaphore create failed");
        return RT_ERR_FAILED;
    }

    RT_ERR_CHK(ioal_init_memRegion_get(unit, IOAL_MEM_L2NTFY_RING, &l2ntfy_ring_vir_addr), ret);
    RT_ERR_CHK(ioal_init_memRegion_get(unit, IOAL_MEM_L2NTFY_BUF, &l2ntfy_buf_vir_addr), ret);
    RT_ERR_CHK(ioal_init_memRegion_get(unit, IOAL_MEM_L2NTFY_USR, &l2ntfy_usr_vir_addr), ret);

    osal_memset((void*)(l2ntfy_ring_vir_addr), 0, sizeof(uint32) * RTL8390_L2NTFY_RING_SIZE);
    if ((ret = osal_cache_memory_flush((uint32)(L2NTFY_RING_ADDR_USR2KRN(l2ntfy_ring_vir_addr)), sizeof(uint32) * RTL8390_L2NTFY_RING_SIZE)) != RT_ERR_OK)
    {
        return ret;
    }
    osal_memset((void*)(l2ntfy_buf_vir_addr), 0, sizeof(l2ntfy_nBuf_8390_t) * RTL8390_L2NTFY_RING_SIZE);
    if ((ret = osal_cache_memory_flush((uint32)(L2NTFY_BUF_ADDR_USR2KRN(l2ntfy_buf_vir_addr)), sizeof(l2ntfy_nBuf_8390_t) * RTL8390_L2NTFY_RING_SIZE)) != RT_ERR_OK)
    {
        return ret;
    }

    pRing = (uint32*)l2ntfy_ring_vir_addr;
    pNbuf = (l2ntfy_nBuf_8390_t*)l2ntfy_buf_vir_addr;
    for (i = 0, j = 0; i < RTL8390_L2NTFY_RING_SIZE; i++)
    {
        pRing[i] = L2NTFY_BUF_ADDR_USR2KRN((uint32)(&pNbuf[i])) | L2NTFY_RING_OWNER_SWITCH;
        j += 10;
    }
    pRing[RTL8390_L2NTFY_RING_SIZE - 1] = L2NTFY_BUF_ADDR_USR2KRN((uint32)(&pNbuf[RTL8390_L2NTFY_RING_SIZE - 1])) | (1 << 1) | L2NTFY_RING_OWNER_SWITCH;    /* Wrap */



    ioal_mem32_read(unit, RTL8390_DMA_IF_INTR_MSK_ADDR, &value);
    value |= INT_8390_NTFY_DONE_MASK | INT_8390_NTFY_BUF_RUN_OUT_MASK | INT_8390_LOCAL_NTFY_BUF_RUN_OUT_MASK;
    ioal_mem32_write(unit, RTL8390_DMA_IF_INTR_MSK_ADDR, value);

    ioal_mem32_write(unit, RTL8390_DMA_IF_NBUF_BASE_DESC_ADDR_CTRL_ADDR, L2NTFY_RING_ADDR_USR2KRN(l2ntfy_ring_vir_addr));
    ioal_mem32_field_write(unit, RTL8390_L2_NOTIFICATION_CTRL_ADDR, RTL8390_L2_NOTIFICATION_CTRL_BP_THR_OFFSET, RTL8390_L2_NOTIFICATION_CTRL_BP_THR_MASK, 100);


    l2ntfy_init[unit] = INIT_COMPLETED;

    return RT_ERR_OK;
}

int32
r8390_l2ntfy_register(uint32 unit, drv_l2ntfy_cb_f cb, drv_l2ntfy_runoutcb_f runout_cb)
{
    ntfy_callback = cb;
    runout_callback = runout_cb;
    return RT_ERR_OK;
}

int32
r8390_l2ntfy_unregister(uint32 unit)
{
    ntfy_callback = NULL;
    runout_callback = NULL;
    return RT_ERR_OK;
}

int32
r8390_l2ntfy_isr_handler(uint32 unit, void *isr_param)
{
    uint32  ringID, entryID;
    uint32  usedRingCnt = 0, ringCnt = 0;
    uint32  usedEventCnt = 0;
    int32   ret = RT_ERR_FAILED;
    rtk_l2ntfy_eventEntry_t         *pEventArray;
    rtk_l2ntfy_usrEventArray_t      userCollection;
    const   uint32 eventArraySize = RTL8390_L2NTFY_RING_SIZE * NBUF_SIZE;


    eventRing = (uint32 *)l2ntfy_ring_vir_addr;
    nBuf = (l2ntfy_nBuf_8390_t*)l2ntfy_buf_vir_addr;
    pEventArray = (rtk_l2ntfy_eventEntry_t *)l2ntfy_usr_vir_addr;

    /* Count event number for allocate memory */
    for(ringID = lastRingID; usedRingCnt < RTL8390_L2NTFY_RING_SIZE; )
    {
        if ((eventRing[ringID] & 1) != L2NTFY_RING_OWNER_CPU)
        {
            break;
        }

        for(entryID = 0; entryID < NBUF_SIZE; entryID++)
        {
            if(nBuf[ringID].event[entryID].valid == TRUE)
            {
                pEventArray[curPos].fidVid = nBuf[ringID].event[entryID].fidVid;
                pEventArray[curPos].mac.octet[0] = (uint8)((nBuf[ringID].event[entryID].mac >> 40) & 0xff);
                pEventArray[curPos].mac.octet[1] = (uint8)((nBuf[ringID].event[entryID].mac >> 32) & 0xff);
                pEventArray[curPos].mac.octet[2] = (uint8)((nBuf[ringID].event[entryID].mac >> 24) & 0xff);
                pEventArray[curPos].mac.octet[3] = (uint8)((nBuf[ringID].event[entryID].mac >> 16) & 0xff);
                pEventArray[curPos].mac.octet[4] = (uint8)((nBuf[ringID].event[entryID].mac >> 8) & 0xff);
                pEventArray[curPos].mac.octet[5] = (uint8)(nBuf[ringID].event[entryID].mac & 0xff);
                pEventArray[curPos].type = nBuf[ringID].event[entryID].type;
                pEventArray[curPos].slp = nBuf[ringID].event[entryID].slp;

                usedEventCnt++;
                totalEventCount++;
                curPos = (curPos + 1) % eventArraySize;
            }
        }
        ringID = (ringID + 1) % RTL8390_L2NTFY_RING_SIZE;
        usedRingCnt++;
    }

    if (usedEventCnt == 0)  /* Sometimes, the data is processed by previous interrupt */
    {
        return RT_ERR_OK;
    }

    //pEventArray = (rtk_l2ntfy_eventEntry_t *)osal_alloc(usedEventCnt * sizeof(rtk_l2ntfy_eventEntry_t));

    for(ringID = lastRingID; ringCnt < usedRingCnt;)
    {
        if((eventRing[ringID] & 1) != L2NTFY_RING_OWNER_CPU)
            break;

        osal_memset(&nBuf[ringID], 0, sizeof(l2ntfy_nBuf_8390_t));
        if (ntfy_debug_flag & DEBUG_STOP_RELEASE_BIT)
        {
            /* Do nothing */
        }
        else
        {
            eventRing[ringID] |= L2NTFY_RING_OWNER_SWITCH;
        }

        lastRingID = (lastRingID + 1) % RTL8390_L2NTFY_RING_SIZE;
        ringID = (ringID + 1) % RTL8390_L2NTFY_RING_SIZE;
        ringCnt++;
    }

    userCollection.baseAddr = pEventArray;
    userCollection.entryNum = usedEventCnt;
    if (curPos >= userCollection.entryNum)
    {
        userCollection.wrap = 0;
        userCollection.startPtr = (curPos - userCollection.entryNum);
    }
    else
    {
        userCollection.wrap = 1;
        userCollection.startPtr = (RTL8390_L2NTFY_RING_SIZE * NBUF_SIZE) - (userCollection.entryNum - curPos);
    }

    if (ntfy_callback)
        ntfy_callback(unit, &userCollection);

    if ((ret = osal_cache_memory_flush((uint32)(L2NTFY_RING_ADDR_USR2KRN(l2ntfy_ring_vir_addr)), sizeof(uint32) * RTL8390_L2NTFY_RING_SIZE)) != RT_ERR_OK)
    {
        return ret;
    }
    if ((ret = osal_cache_memory_flush((uint32)(L2NTFY_BUF_ADDR_USR2KRN(l2ntfy_buf_vir_addr)), sizeof(l2ntfy_nBuf_8390_t) * RTL8390_L2NTFY_RING_SIZE)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
}
#if 0
int32
r8390_l2ntfy_isr_handler(uint32 unit, void *isr_param)
{
    int32   ret = RT_ERR_FAILED;
    uint32  curPos, num;
    rtk_l2ntfy_isrParam_t       *isrParam = (rtk_l2ntfy_isrParam_t*)isr_param;
    rtk_l2ntfy_eventEntry_t     *pEventArray;
    rtk_l2ntfy_usrEventArray_t  userCollection;


    curPos = isrParam->curPos;
    num = isrParam->num;
    printf("%s():%d  curPos:%d num:%d\n", __FUNCTION__, __LINE__, curPos, num);

    pEventArray = (rtk_l2ntfy_eventEntry_t*)l2ntfy_buf_vir_addr;

    userCollection.baseAddr = pEventArray;
    userCollection.entryNum = num;
    if (curPos >= num)
    {
        userCollection.wrap = 0;
        userCollection.startPtr = (curPos - num);
    }
    else
    {
        userCollection.wrap = 1;
        userCollection.startPtr = (RTL8390_L2NTFY_RING_SIZE * NBUF_SIZE) - (num - curPos);
    }
    if (ntfy_callback)
            ntfy_callback(unit, &userCollection);

    if ((ret = osal_cache_memory_flush((uint32)(L2NTFY_BUF_ADDR_USR2KRN(l2ntfy_buf_vir_addr)), sizeof(rtk_l2ntfy_eventEntry_t) * RTL8390_L2NTFY_RING_SIZE * NBUF_SIZE)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
}
#endif

int32
r8390_l2ntfy_bufRunout_handler(uint32 unit, void *isr_param)
{
    if (runout_callback)
        runout_callback(unit, L2_NOTIFY_BUF_NBUF);

    return RT_ERR_OK;
}

int32
r8390_l2ntfy_localBufRunout_handler(uint32 unit, void *isr_param)
{
    if (runout_callback)
        runout_callback(unit, L2_NOTIFY_BUF_ASIC_FIFO);

    return RT_ERR_OK;
}

int32
r8390_l2ntfy_enable_get(uint32 unit, rtk_enable_t *pEnable)
{
    RT_INIT_CHK(l2ntfy_init[unit]);

    L2NTFY_SEM_LOCK(unit);
    ioal_mem32_field_read(unit, RTL8390_L2_NOTIFICATION_CTRL_ADDR, RTL8390_L2_NOTIFICATION_CTRL_NOTIFICATION_EN_OFFSET,
                            RTL8390_L2_NOTIFICATION_CTRL_NOTIFICATION_EN_MASK, pEnable);
    L2NTFY_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

int32
r8390_l2ntfy_enable_set(uint32 unit, rtk_enable_t enable)
{
    RT_INIT_CHK(l2ntfy_init[unit]);

    L2NTFY_SEM_LOCK(unit);
    ioal_mem32_field_write(unit, RTL8390_L2_NOTIFICATION_CTRL_ADDR, RTL8390_L2_NOTIFICATION_CTRL_NOTIFICATION_EN_OFFSET,
                            RTL8390_L2_NOTIFICATION_CTRL_NOTIFICATION_EN_MASK, enable);
    L2NTFY_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      r8390_l2ntfy_backPressureThresh_get
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
 *      RT_ERR_NULL_POINTER     - Input parameter may be null pointer
 * Note:
 *      (1) Back-pressure is a mechanism to avoid ASIC's notification FIFO running out
 *      (2) Back-pressure only suppresses the aging out event when the notification FIFO is over the
 *          specified threshold.
 */
int32
r8390_l2ntfy_backPressureThresh_get(uint32 unit, uint32 *pThresh)
{
    RT_INIT_CHK(l2ntfy_init[unit]);

    L2NTFY_SEM_LOCK(unit);
    ioal_mem32_field_read(unit, RTL8390_L2_NOTIFICATION_CTRL_ADDR, RTL8390_L2_NOTIFICATION_CTRL_BP_THR_OFFSET,
                            RTL8390_L2_NOTIFICATION_CTRL_BP_THR_MASK, pThresh);
    L2NTFY_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      r8390_l2ntfy_backPressureThresh_set
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
 *      RT_ERR_OUT_OF_RANGE     - Input parameter out of range
 * Note:
 *      (1) Back-pressure is a mechanism to avoid ASIC's notification FIFO running out
 *      (2) Back-pressure only suppresses the aging out event when the notification FIFO is over the
 *          specified threshold.
 */
int32
r8390_l2ntfy_backPressureThresh_set(uint32 unit, uint32 thresh)
{
    RT_INIT_CHK(l2ntfy_init[unit]);

    L2NTFY_SEM_LOCK(unit);
    ioal_mem32_field_write(unit, RTL8390_L2_NOTIFICATION_CTRL_ADDR, RTL8390_L2_NOTIFICATION_CTRL_BP_THR_OFFSET,
                            RTL8390_L2_NOTIFICATION_CTRL_BP_THR_MASK, thresh);
    L2NTFY_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      r8390_l2ntfy_notificationEventEnable_get
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
 *      RT_ERR_NULL_POINTER     - Input parameter may be null pointer
 * Note:
 *      For 8390, only suspend learn and linkdown flush types support enable/disable state configuration.
 */
int32
r8390_l2ntfy_notificationEventEnable_get(uint32 unit, rtk_l2ntfy_event_t event, rtk_enable_t *pEnable)
{
    RT_INIT_CHK(l2ntfy_init[unit]);

    L2NTFY_SEM_LOCK(unit);
    if (event == L2NTFY_EVENT_LINKDOWNFLUSH)
    {
        ioal_mem32_field_read(unit, RTL8390_L2_CTRL_0_ADDR, RTL8390_L2_CTRL_0_FLUSH_NOTIFY_EN_OFFSET,
                                RTL8390_L2_CTRL_0_FLUSH_NOTIFY_EN_MASK, pEnable);
    }
    else
    {
        ioal_mem32_field_read(unit, RTL8390_L2_NOTIFICATION_CTRL_ADDR, RTL8390_L2_NOTIFICATION_CTRL_SUSPEND_NOTIFICATION_EN_OFFSET,
                                RTL8390_L2_NOTIFICATION_CTRL_SUSPEND_NOTIFICATION_EN_MASK, pEnable);
    }
    L2NTFY_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      r8390_l2ntfy_notificationEventEnable_set
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
 *      RT_ERR_INPUT            - Invalid input parameter
 * Note:
 *      For 8390, only suspend learn and linkdown flush types support enable/disable state configuration.
 */
int32
r8390_l2ntfy_notificationEventEnable_set(uint32 unit, rtk_l2ntfy_event_t event, rtk_enable_t enable)
{
    RT_INIT_CHK(l2ntfy_init[unit]);

    L2NTFY_SEM_LOCK(unit);
    if (event == L2NTFY_EVENT_LINKDOWNFLUSH)
    {
        ioal_mem32_field_write(unit, RTL8390_L2_CTRL_0_ADDR, RTL8390_L2_CTRL_0_FLUSH_NOTIFY_EN_OFFSET,
                                RTL8390_L2_CTRL_0_FLUSH_NOTIFY_EN_MASK, enable);
    }
    else
    {
        ioal_mem32_field_write(unit, RTL8390_L2_NOTIFICATION_CTRL_ADDR, RTL8390_L2_NOTIFICATION_CTRL_SUSPEND_NOTIFICATION_EN_OFFSET,
                                RTL8390_L2_NOTIFICATION_CTRL_SUSPEND_NOTIFICATION_EN_MASK, enable);
    }
    L2NTFY_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      r8390_l2ntfy_fifoEmptyStatus_get
 * Description:
 *      Get the empty status of ASIC's l2-notification FIFO in specified device.
 * Input:
 *      unit    - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - Invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 * Note:
 *      None
 */
int32
r8390_l2ntfy_fifoEmptyStatus_get(uint32 unit, rtk_l2ntfy_fifoStatus_t *pStatus)
{
    RT_INIT_CHK(l2ntfy_init[unit]);

    L2NTFY_SEM_LOCK(unit);
    ioal_mem32_field_read(unit, RTL8390_L2_NOTIFICATION_CTRL_ADDR, RTL8390_L2_NOTIFICATION_CTRL_FIFO_EMPTY_OFFSET,
                            RTL8390_L2_NOTIFICATION_CTRL_FIFO_EMPTY_MASK, pStatus);
    L2NTFY_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      r8390_l2ntfy_counter_dump
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
 * Note:
 *      None
 */
int32
r8390_l2ntfy_counter_dump(uint32 unit)
{
    /* Check init state */
    RT_INIT_CHK(l2ntfy_init[unit]);

    L2NTFY_SEM_LOCK(unit);
    osal_printf("runoutCnt       : %u\n", runoutCnt);
    osal_printf("totalEventCount : %u\n", totalEventCount);
    L2NTFY_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      r8390_l2ntfy_counter_clear
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
 * Note:
 *      None
 */
int32
r8390_l2ntfy_counter_clear(uint32 unit)
{
    /* Check init state */
    RT_INIT_CHK(l2ntfy_init[unit]);

    L2NTFY_SEM_LOCK(unit);
    runoutCnt = 0;
    totalEventCount = 0;
    L2NTFY_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

int32
r8390_l2ntfy_sizeInfo_get(uint32 unit, uint32 *ringSize, uint32 *nBufSize)
{
    /* Check init state */
    RT_INIT_CHK(l2ntfy_init[unit]);

    L2NTFY_SEM_LOCK(unit);
    *ringSize = RTL8390_L2NTFY_RING_SIZE;
    *nBufSize = NBUF_SIZE;
    L2NTFY_SEM_UNLOCK(unit);
    return RT_ERR_OK;
}

/* Function Name:
 *      r8390_l2ntfy_debug_get
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
 *      RT_ERR_NULL_POINTER     - Input parameter may be null pointer
 * Note:
 *      None
 */
int32
r8390_l2ntfy_debug_get(uint32 unit, uint32 *pFlags)
{
    RT_INIT_CHK(l2ntfy_init[unit]);

    L2NTFY_SEM_LOCK(unit);
    *pFlags = ntfy_debug_flag;
    L2NTFY_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

int32
r8390_l2ntfy_debug_set(uint32 unit, uint32 flags)
{
#if 0
    uint32  ringID;

    RT_INIT_CHK(l2ntfy_init[unit]);

    if ((flags & DEBUG_STOP_RELEASE_BIT) == 0 && (ntfy_debug_flag & DEBUG_STOP_RELEASE_BIT))
    {
        ntfy_debug_flag = flags;
        for(ringID = 0; ringID < _blockNum; ringID++)
        {
            eventRing[ringID] |= L2NTFY_RING_OWNER_SWITCH;
        }
        ioal_mem32_write(unit, RTL8390_DMA_IF_INTR_STS_ADDR, INT_8390_NTFY_BUF_RUN_OUT_MASK);
    }

    if (flags & DEBUG_ATS_TEST)
    {
        if (_atsArray.eventArray == NULL)
        {
            _atsArray.eventArray = osal_alloc(1024 * sizeof(rtk_l2ntfy_eventEntry_t));
            _atsArray.entryNum = 0;
        }
    }
    else if ((flags & DEBUG_ATS_TEST) == 0 && (ntfy_debug_flag & DEBUG_ATS_TEST))
    {
        osal_free(_atsArray.eventArray);
        _atsArray.eventArray = NULL;
    }

    ntfy_debug_flag = flags;
#endif
    return RT_ERR_OK;
}

int32
r8390_l2ntfy_event_dump(uint32 unit)
{
#if 0
    uint32  ringID, entryID;
    uint64  mac;

    RT_INIT_CHK(l2ntfy_init[unit]);

    for(ringID = 0; ringID < _blockNum; ringID++)
    {
        osal_printf("ringID:%d   own:%s\n", ringID, (eventRing[ringID] & 1) == L2NTFY_RING_OWNER_CPU ? "CPU" : "SW");
        {
            for(entryID = 0; entryID < NBUF_SIZE; entryID++)
            {
                mac = nBuf[ringID].event[entryID].mac;
                osal_printf("%2d-%d type:%d fid:%d  mac:%x-%x-%x-%x-%x-%x  slp:%d  valid:%d\n", ringID, entryID,
                                nBuf[ringID].event[entryID].type, nBuf[ringID].event[entryID].fidVid,
                                (uint8)((mac >> 40) & 0xff), (uint8)((mac >> 32) & 0xff), (uint8)((mac >> 24) & 0xff),
                                (uint8)((mac >> 16) & 0xff), (uint8)((mac >> 8) & 0xff), (uint8)(mac & 0xff),
                                nBuf[ringID].event[entryID].slp, nBuf[ringID].event[entryID].valid);

            }
        }
    }

    osal_printf("Current ringID:%d \n", lastRingID);

#endif
    return RT_ERR_OK;
}

