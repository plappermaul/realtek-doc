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
 * Purpose : L2 notification initialization.
 *
 * Feature : L2 notification initialization
 *
 */

/*
 * Include Files
 */
#include <linux/version.h>
#include <common/debug/rt_log.h>
#include <common/error.h>
#include <drv/l2ntfy/l2ntfy.h>
#include <private/drv/l2ntfy/l2ntfy_rtl8390.h>
#include <private/drv/swcore/swcore_rtl8390.h>
#include <ioal/mem32.h>
#include <osal/lib.h>
#include <osal/memory.h>
#include <osal/print.h>
#include <osal/spl.h>
#include <osal/sem.h>
#include <hwp/hw_profile.h>


/*
 * Symbol Definition
 */


/*
 * Data Type Definition
 */


/*
 * Data Declaration
 */
static uint32               l2ntfy_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};
#if defined(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE)

#else
static osal_mutex_t                     l2ntfy_sem[RTK_MAX_NUM_OF_UNIT];
#endif
static uint32                           l2_lrn[LRN_AGE_SETTING_SHADOW_MAX][RTK_MAX_NUM_OF_PORTS];
static uint32                           l2_age_unit;


/*
 * Macro Definition
 */
#if defined(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE)
#define L2NTFY_SEM_LOCK(unit)           osal_isr_disable_interrupt()
#define L2NTFY_SEM_UNLOCK(unit)         osal_isr_enable_interrupt()
#else
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
#endif


/* Function Name:
 *      r8390_l2ntfy_init
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
 * Note:
 *      Must initialize L2-notification module before calling any nic APIs.
 */
int32
r8390_l2ntfy_init(uint32 unit)
{
    RT_INIT_REENTRY_CHK(l2ntfy_init[unit]);

#if defined(CONFIG_SDK_KERNEL_LINUX_USER_MODE)
    /* Create semaphore */
    l2ntfy_sem[unit] = osal_sem_mutex_create();
    if (0 == l2ntfy_sem[unit])
    {
        RT_ERR(RT_ERR_FAILED, (MOD_NIC), "semaphore create failed");
        return RT_ERR_FAILED;
    }
#endif

    l2ntfy_init[unit] = INIT_COMPLETED;

    return RT_ERR_OK;
}

/* Function Name:
 *      r8390_l2ntfy_enable_get
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
 *      RT_ERR_NULL_POINTER     - Input parameter may be null pointer
 * Note:
 *      None
 */
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

/* Function Name:
 *      r8390_l2ntfy_enable_set
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
 * Note:
 *      None
 */
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
 *      RT_ERR_INPUT            - Invalid input parameter
 * Note:
 *      (1) Back-pressure is a mechanism to avoid ASIC's notification FIFO running out
 *      (2) Back-pressure only suppresses the aging out event when the notification FIFO is over the
 *          specified threshold.
 */
int32
r8390_l2ntfy_backPressureThresh_set(uint32 unit, uint32 thresh)
{
    RT_INIT_CHK(l2ntfy_init[unit]);
    RT_PARAM_CHK((thresh > RTL8390_L2NTFY_BP_THR_MAX) || (thresh < RTL8390_L2NTFY_BP_THR_MIN), RT_ERR_INPUT);

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
 *      RT_ERR_INPUT            - Invalid input parameter
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
    uint32 val;

    RT_INIT_CHK(l2ntfy_init[unit]);

    L2NTFY_SEM_LOCK(unit);
    ioal_mem32_field_read(unit, RTL8390_L2_NOTIFICATION_CTRL_ADDR, RTL8390_L2_NOTIFICATION_CTRL_FIFO_EMPTY_OFFSET,
                            RTL8390_L2_NOTIFICATION_CTRL_FIFO_EMPTY_MASK, &val);
    L2NTFY_SEM_UNLOCK(unit);

    *pStatus = (val == 0) ? L2NTFY_FIFO_EMPTY : L2NTFY_FIFO_NOT_EMPTY;

    return RT_ERR_OK;
}

/* Function Name:
 *      r8390_l2ntfy_dst_get
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
 * Note:
 *      None
 */
int32
r8390_l2ntfy_dst_get(uint32 unit, rtk_l2ntfy_dst_t *pDst)
{
    *pDst = L2NTFY_DST_NIC;
    return RT_ERR_OK;
}


/* Function Name:
 *      r8390_l2ntfy_dst_set
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
 * Note:
 *      None
 */
int32
r8390_l2ntfy_dst_set(uint32 unit, rtk_l2ntfy_dst_t dst)
{
    return RT_ERR_CHIP_NOT_SUPPORTED;
}

/* Function Name:
 *      r8390_l2ntfy_reset
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
 * Note:
 *      None
 */
int32
r8390_l2ntfy_reset(uint32 unit)
{
    return RT_ERR_CHIP_NOT_SUPPORTED;
}

/* Function Name:
 *      r8390_l2ntfy_iTag_get
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
 * Note:
 *      None
 */
int32
r8390_l2ntfy_iTag_get(uint32 unit, rtk_l2ntfy_iTagCfg_t *pITagCfg)
{
    return RT_ERR_CHIP_NOT_SUPPORTED;
}

/* Function Name:
 *      r8390_l2ntfy_iTag_set
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
 * Note:
 *      None
 */
int32
r8390_l2ntfy_iTag_set(uint32 unit, rtk_l2ntfy_iTagCfg_t *pITagCfg)
{
    return RT_ERR_CHIP_NOT_SUPPORTED;
}

/* Function Name:
 *      r8390_l2ntfy_magicNum_get
 * Description:
 *      Get the magic number of L2-notification.
 * Input:
 *      unit            - unit id
 * Output:
 *      pMagicNumable   - L2-notification magic number
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - Invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_CHIP_NOT_FOUND   - The chip can not found
 *      RT_ERR_NULL_POINTER     - Input parameter may be null pointer
 * Note:
 *      None
 */
int32
r8390_l2ntfy_magicNum_get(uint32 unit, uint32 *pMagicNum)
{
    return RT_ERR_CHIP_NOT_SUPPORTED;
}

/* Function Name:
 *      r8390_l2ntfy_magicNum_set
 * Description:
 *      Set the mode of L2-notification mechanism.
 * Input:
 *      unit            - unit id
 *      magicNumable    - L2-notification magic number
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - Invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_CHIP_NOT_FOUND   - The chip can not found
 * Note:
 *      None
 */
int32
r8390_l2ntfy_magicNum_set(uint32 unit, uint32 magicNum)
{
    return RT_ERR_CHIP_NOT_SUPPORTED;
}

/* Function Name:
 *      r8390_l2ntfy_macAddr_get
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
 * Note:
 *      None
 */
int32
r8390_l2ntfy_macAddr_get(uint32 unit, rtk_l2ntfy_addrType_t type, rtk_mac_t *pMac)
{
    return RT_ERR_CHIP_NOT_SUPPORTED;
}

/* Function Name:
 *      r8390_l2ntfy_macAddr_set
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
 * Note:
 *      None
 */
int32
r8390_l2ntfy_macAddr_set(uint32 unit, rtk_l2ntfy_addrType_t type, rtk_mac_t *pMac)
{
    return RT_ERR_CHIP_NOT_SUPPORTED;
}

/* Function Name:
 *      r8390_l2ntfy_maxEvent_get
 * Description:
 *      Get the maximum event number of a notification packet.
 * Input:
 *      unit    - unit id
 * Output:
 *      pNum    - pointer to maximum event number
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
r8390_l2ntfy_maxEvent_get(uint32 unit, uint32 *pNum)
{
    return RT_ERR_CHIP_NOT_SUPPORTED;
}

/* Function Name:
 *      r8390_l2ntfy_maxEvent_set
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
 * Note:
 *      None
 */
int32
r8390_l2ntfy_maxEvent_set(uint32 unit, uint32 num)
{
    return RT_ERR_CHIP_NOT_SUPPORTED;
}

/* Function Name:
 *      r8390_l2ntfy_timeout_get
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
 * Note:
 *      None
 */
int32
r8390_l2ntfy_timeout_get(uint32 unit, rtk_l2ntfy_mode_t mode, uint32 *pTimeout)
{
    return RT_ERR_CHIP_NOT_SUPPORTED;
}

/* Function Name:
 *      r8390_l2ntfy_timeout_set
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
 * Note:
 *      None
 */
int32
r8390_l2ntfy_timeout_set(uint32 unit, rtk_l2ntfy_mode_t mode, uint32 timeout)
{
    return RT_ERR_CHIP_NOT_SUPPORTED;
}

/* Function Name:
 *      r8390_l2ntfy_rawToEvent_cnvt
 * Description:
 *      Translate notification raw data to event.
 * Input:
 *      unit        - unit id
 *      pRaw        - the notification raw data
 * Output:
 *      pEvent      - the notification event
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
r8390_l2ntfy_rawToEvent_cnvt(uint32 unit, uint8 *pRaw, rtk_l2ntfy_eventEntry_t *pEvent)
{
    l2ntfy_nBufEntry_8390_t *pEventEntry = (l2ntfy_nBufEntry_8390_t *)pRaw;

    RT_PARAM_CHK(NULL == pRaw, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(NULL == pEvent, RT_ERR_NULL_POINTER);

    L2NTFY_SEM_LOCK(unit);

    if(TRUE == pEventEntry->valid)
    {
        pEvent->fidVid = pEventEntry->fidVid;
        pEvent->mac.octet[0] = (uint8)((pEventEntry->mac >> 40) & 0xff);
        pEvent->mac.octet[1] = (uint8)((pEventEntry->mac >> 32) & 0xff);
        pEvent->mac.octet[2] = (uint8)((pEventEntry->mac >> 24) & 0xff);
        pEvent->mac.octet[3] = (uint8)((pEventEntry->mac >> 16) & 0xff);
        pEvent->mac.octet[4] = (uint8)((pEventEntry->mac >> 8) & 0xff);
        pEvent->mac.octet[5] = (uint8)(pEventEntry->mac & 0xff);
        pEvent->type = pEventEntry->type;
        pEvent->slp = pEventEntry->slp;
        L2NTFY_SEM_UNLOCK(unit);

        return RT_ERR_OK;
    }
    else
    {
        L2NTFY_SEM_UNLOCK(unit);

        return RT_ERR_TYPE;
    }

}

/* Function Name:
 *      r8390_l2ntfy_sizeInfo_get
 * Description:
 *      Get the ringSize/nBufSize value that chip support.
 * Input:
 *      unit        - unit id
 * Output:
 *      pRingSize   - pointer to ring size value
 *      pNBufSize   - pointer to nBuf size value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
r8390_l2ntfy_sizeInfo_get(uint32 unit, uint32 *pRingSize, uint32 *pNBufSize)
{
    /* Check init state */
    RT_INIT_CHK(l2ntfy_init[unit]);

    *pRingSize = RTL8390_L2NTFY_RING_SIZE;
    *pNBufSize = RTL8390_NBUF_SIZE;

    return RT_ERR_OK;
}

/* Function Name:
 *      r8390_l2ntfy_sizeInfo_get
 * Description:
 *      Get the entry size value that chip support.
 * Input:
 *      unit        - unit id
 * Output:
 *      pEntrySize  - pointer to entry size value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
r8390_l2ntfy_entryLen_get(uint32 unit, uint32 *pEntrySize)
{
    /* Check init state */
    RT_INIT_CHK(l2ntfy_init[unit]);

    *pEntrySize = RTL8390_ENTRY_BYTE;

    return RT_ERR_OK;
}

/* Function Name:
 *      r8390_l2ntfy_pktCpuQue_set
 * Description:
 *      Set the CPU queue ID that a notification packet was trapped.
 * Input:
 *      unit    - unit id
 *      queId   - CPU queue ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
r8390_l2ntfy_pktCpuQue_set(uint32 unit, uint32 queId)
{
    return RT_ERR_CHIP_NOT_SUPPORTED;
}

/* Function Name:
 *      r8390_l2ntfy_l2LearningAge_disable
 * Description:
 *      Disable per-port L2 learning ability.
 * Input:
 *      unit        - unit id
 *      array_idx   - index to learning/aging setting shadow database
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      None
 */
int32 r8390_l2ntfy_l2LearningAge_disable(uint32 unit, uint32 array_index)
{
    rtk_port_t  port;

    /* Disable learning */
    HWP_PORT_TRAVS(unit, port)
    {
        ioal_mem32_field_read(unit, RTL8390_L2_PORT_NEW_SALRN_ADDR(port),
                                    RTL8390_L2_PORT_NEW_SALRN_NEW_SALRN_OFFSET(port),
                                    RTL8390_L2_PORT_NEW_SALRN_NEW_SALRN_MASK(port), &l2_lrn[array_index][port]);
        ioal_mem32_field_write(unit, RTL8390_L2_PORT_NEW_SALRN_ADDR(port),
                                    RTL8390_L2_PORT_NEW_SALRN_NEW_SALRN_OFFSET(port),
                                    RTL8390_L2_PORT_NEW_SALRN_NEW_SALRN_MASK(port), 0x2);   //not learn
    }


    /* Disable aging */
    ioal_mem32_field_read(unit, RTL8390_L2_CTRL_1_ADDR, RTL8390_L2_CTRL_1_AGE_UNIT_OFFSET, RTL8390_L2_CTRL_1_AGE_UNIT_MASK, &l2_age_unit);
    ioal_mem32_field_write(unit, RTL8390_L2_CTRL_1_ADDR, RTL8390_L2_CTRL_1_AGE_UNIT_OFFSET, RTL8390_L2_CTRL_1_AGE_UNIT_MASK, 0);

    return RT_ERR_OK;
}

/* Function Name:
 *      r8390_l2ntfy_l2LearningAge_restore
 * Description:
 *      Restore per-port L2 learning ability.
 * Input:
 *      unit        - unit id
 *      array_idx   - index to learning/aging setting shadow database
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      None
 */
int32 r8390_l2ntfy_l2LearningAge_restore(uint32 unit, uint32 array_index)
{
    rtk_port_t  port;

    /* Restore learning */
    HWP_PORT_TRAVS(unit, port)
    {
        ioal_mem32_field_write(unit, RTL8390_L2_PORT_NEW_SALRN_ADDR(port),
                                     RTL8390_L2_PORT_NEW_SALRN_NEW_SALRN_OFFSET(port),
                                     RTL8390_L2_PORT_NEW_SALRN_NEW_SALRN_MASK(port), l2_lrn[array_index][port]);
    }


    ioal_mem32_field_write(unit, RTL8390_L2_CTRL_1_ADDR, RTL8390_L2_CTRL_1_AGE_UNIT_OFFSET, RTL8390_L2_CTRL_1_AGE_UNIT_MASK, l2_age_unit);

    return RT_ERR_OK;
}

