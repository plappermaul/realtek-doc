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
#include <dal/longan/dal_longan_l2.h>
#include <common/debug/rt_log.h>
#include <common/error.h>
#include <drv/l2ntfy/l2ntfy.h>
#include <private/drv/l2ntfy/l2ntfy_rtl9300.h>
#include <private/drv/swcore/swcore_rtl9300.h>
#include <ioal/mem32.h>
#include <osal/lib.h>
#include <osal/memory.h>
#include <osal/print.h>
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
static uint32                           l2ntfy_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};
static osal_mutex_t                     l2ntfy_sem[RTK_MAX_NUM_OF_UNIT];



static drv_l2ntfy_portNewMacOp_cb_f _dal_longan_l2_portNewMacOp_set_cb = NULL;
static drv_l2ntfy_agingTime_cb_f _dal_longan_l2_agingTime_set_cb = NULL;


/*
 * Macro Definition
 */
/* Semaphore handling */
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

/* SS-112 */
#define L2NTFY_SS_112(unit, pVal) ioal_mem32_read(unit, RTL9300_DMY_REG1_NIC_ADDR, pVal)

/*
 * Function Declaration
 */


/* Function Name:
 *      r9300_l2ntfy_init
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
r9300_l2ntfy_init(uint32 unit)
{
    RT_INIT_REENTRY_CHK(l2ntfy_init[unit]);

    /* Create semaphore */
    l2ntfy_sem[unit] = osal_sem_mutex_create();
    if (0 == l2ntfy_sem[unit])
    {
        RT_ERR(RT_ERR_FAILED, (MOD_NIC), "semaphore create failed");
        return RT_ERR_FAILED;
    }

    /* Patch */
    ioal_mem32_write(unit, RTL9300_L2_NTFY_PKT_LOCAL_THR_ADDR, 0x4000008);

    l2ntfy_init[unit] = INIT_COMPLETED;

    return RT_ERR_OK;
}

/* Function Name:
 *      r9300_l2ntfy_enable_get
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
extern int32
r9300_l2ntfy_enable_get(uint32 unit, rtk_enable_t *pEnable)
{
    uint32 val;

    RT_INIT_CHK(l2ntfy_init[unit]);

    L2NTFY_SEM_LOCK(unit);
    L2NTFY_SS_112(unit, &val);
    ioal_mem32_field_read(unit, RTL9300_L2_NTFY_CTRL_ADDR, RTL9300_L2_NTFY_CTRL_NTFY_EN_OFFSET,
                            RTL9300_L2_NTFY_CTRL_NTFY_EN_MASK, &val);
    L2NTFY_SEM_UNLOCK(unit);

    *pEnable = val==1 ? ENABLED : DISABLED;

    return RT_ERR_OK;
}

/* Function Name:
 *      r9300_l2ntfy_enable_set
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
r9300_l2ntfy_enable_set(uint32 unit, rtk_enable_t enable)
{
    uint32 val;

    RT_INIT_CHK(l2ntfy_init[unit]);

    if(enable==ENABLED)
        val = 1;
    else if(enable==DISABLED)
        val = 0;
    else
        return RT_ERR_INPUT;

    L2NTFY_SEM_LOCK(unit);
    ioal_mem32_field_write(unit, RTL9300_L2_NTFY_CTRL_ADDR, RTL9300_L2_NTFY_CTRL_NTFY_EN_OFFSET,
                            RTL9300_L2_NTFY_CTRL_NTFY_EN_MASK, val);
    L2NTFY_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      r9300_l2ntfy_backPressureThresh_get
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
r9300_l2ntfy_backPressureThresh_get(uint32 unit, uint32 *pThresh)
{
    RT_INIT_CHK(l2ntfy_init[unit]);

    L2NTFY_SEM_LOCK(unit);
    L2NTFY_SS_112(unit, pThresh);
    ioal_mem32_field_read(unit, RTL9300_L2_NTFY_CTRL_ADDR, RTL9300_L2_NTFY_CTRL_BP_THR_OFFSET,
                            RTL9300_L2_NTFY_CTRL_BP_THR_MASK, pThresh);
    L2NTFY_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      r9300_l2ntfy_backPressureThresh_set
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
r9300_l2ntfy_backPressureThresh_set(uint32 unit, uint32 thresh)
{
    RT_INIT_CHK(l2ntfy_init[unit]);
    RT_PARAM_CHK((thresh > RTL9300_L2NTFY_BP_THR_MAX) || (thresh < RTL9300_L2NTFY_BP_THR_MIN), RT_ERR_INPUT);

    L2NTFY_SEM_LOCK(unit);
    ioal_mem32_field_write(unit, RTL9300_L2_NTFY_CTRL_ADDR, RTL9300_L2_NTFY_CTRL_BP_THR_OFFSET,
                            RTL9300_L2_NTFY_CTRL_BP_THR_MASK, thresh);
    L2NTFY_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      r9300_l2ntfy_notificationEventEnable_get
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
 *      suspend learn and linkdown flush types support enable/disable state configuration.
 */
int32
r9300_l2ntfy_notificationEventEnable_get(uint32 unit, rtk_l2ntfy_event_t event, rtk_enable_t *pEnable)
{
    uint32 offset=0;
    uint32 mask=0;
    uint32 val;

    RT_INIT_CHK(l2ntfy_init[unit]);

    switch(event)
    {
        case L2NTFY_EVENT_DYN:
            offset = RTL9300_L2_NTFY_CTRL_DYN_NTFY_EN_OFFSET;
            mask = RTL9300_L2_NTFY_CTRL_DYN_NTFY_EN_MASK;
            break;
        case L2NTFY_EVENT_STTC:
            offset = RTL9300_L2_NTFY_CTRL_STTC_NTFY_EN_OFFSET;
            mask = RTL9300_L2_NTFY_CTRL_STTC_NTFY_EN_MASK;
            break;
        case L2NTFY_EVENT_TAG:
            offset = RTL9300_L2_NTFY_CTRL_TAGSTS_NTFY_EN_OFFSET;
            mask = RTL9300_L2_NTFY_CTRL_TAGSTS_NTFY_EN_MASK;
            break;
        case L2NTFY_EVENT_SUSPEND:
            offset = RTL9300_L2_NTFY_CTRL_SUS_NTFY_EN_OFFSET;
            mask = RTL9300_L2_NTFY_CTRL_SUS_NTFY_EN_MASK;
            break;;
        case L2NTFY_EVENT_LINKDOWNFLUSH:
            offset = RTL9300_L2_NTFY_CTRL_LD_FLUSH_NTFY_EN_OFFSET;
            mask = RTL9300_L2_NTFY_CTRL_LD_FLUSH_NTFY_EN_MASK;
            break;
        case L2NTFY_EVENT_DASABLK:
            offset = RTL9300_L2_NTFY_CTRL_DASABLK_NTFY_EN_OFFSET;
            mask = RTL9300_L2_NTFY_CTRL_DASABLK_NTFY_EN_MASK;
            break;
        case L2NTFY_EVENT_HASHFULL:
            offset = RTL9300_L2_NTFY_CTRL_HASHFULL_NTFY_EN_OFFSET;
            mask = RTL9300_L2_NTFY_CTRL_HASHFULL_NTFY_EN_MASK;
            break;

        default:
            return RT_ERR_INPUT;
    }


    L2NTFY_SEM_LOCK(unit);
    L2NTFY_SS_112(unit, &val);
    ioal_mem32_field_read(unit, RTL9300_L2_NTFY_CTRL_ADDR, offset, mask, &val);
    L2NTFY_SEM_UNLOCK(unit);

    *pEnable = val==1 ? ENABLED : DISABLED;

    return RT_ERR_OK;
}

/* Function Name:
 *      r9300_l2ntfy_notificationEventEnable_set
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
 *      suspend learn and linkdown flush types support enable/disable state configuration.
 */
int32
r9300_l2ntfy_notificationEventEnable_set(uint32 unit, rtk_l2ntfy_event_t event, rtk_enable_t enable)
{
    uint32 offset=0;
    uint32 mask=0;
    uint32 val;

    RT_INIT_CHK(l2ntfy_init[unit]);

    switch(event)
    {
        case L2NTFY_EVENT_DYN:
            offset = RTL9300_L2_NTFY_CTRL_DYN_NTFY_EN_OFFSET;
            mask = RTL9300_L2_NTFY_CTRL_DYN_NTFY_EN_MASK;
            break;
        case L2NTFY_EVENT_STTC:
            offset = RTL9300_L2_NTFY_CTRL_STTC_NTFY_EN_OFFSET;
            mask = RTL9300_L2_NTFY_CTRL_STTC_NTFY_EN_MASK;
            break;
        case L2NTFY_EVENT_TAG:
            offset = RTL9300_L2_NTFY_CTRL_TAGSTS_NTFY_EN_OFFSET;
            mask = RTL9300_L2_NTFY_CTRL_TAGSTS_NTFY_EN_MASK;
            break;
        case L2NTFY_EVENT_SUSPEND:
            offset = RTL9300_L2_NTFY_CTRL_SUS_NTFY_EN_OFFSET;
            mask = RTL9300_L2_NTFY_CTRL_SUS_NTFY_EN_MASK;
            break;;
        case L2NTFY_EVENT_LINKDOWNFLUSH:
            offset = RTL9300_L2_NTFY_CTRL_LD_FLUSH_NTFY_EN_OFFSET;
            mask = RTL9300_L2_NTFY_CTRL_LD_FLUSH_NTFY_EN_MASK;
            break;
        case L2NTFY_EVENT_DASABLK:
            offset = RTL9300_L2_NTFY_CTRL_DASABLK_NTFY_EN_OFFSET;
            mask = RTL9300_L2_NTFY_CTRL_DASABLK_NTFY_EN_MASK;
            break;
        case L2NTFY_EVENT_HASHFULL:
            offset = RTL9300_L2_NTFY_CTRL_HASHFULL_NTFY_EN_OFFSET;
            mask = RTL9300_L2_NTFY_CTRL_HASHFULL_NTFY_EN_MASK;
            break;
        default:
            return RT_ERR_INPUT;
    }

    if(enable==ENABLED)
        val = 1;
    else if(enable==DISABLED)
        val = 0;
    else
        return RT_ERR_INPUT;

    L2NTFY_SEM_LOCK(unit);
    ioal_mem32_field_write(unit, RTL9300_L2_NTFY_CTRL_ADDR, offset, mask, val);

    L2NTFY_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      r9300_l2ntfy_fifoEmptyStatus_get
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
r9300_l2ntfy_fifoEmptyStatus_get(uint32 unit, rtk_l2ntfy_fifoStatus_t *pStatus)
{
    uint32 val;

    RT_INIT_CHK(l2ntfy_init[unit]);

    L2NTFY_SEM_LOCK(unit);
    L2NTFY_SS_112(unit, &val);
    ioal_mem32_field_read(unit, RTL9300_L2_NTFY_CTRL_ADDR, RTL9300_L2_NTFY_CTRL_FIFO_EMPTY_OFFSET,
                            RTL9300_L2_NTFY_CTRL_FIFO_EMPTY_MASK, &val);
    L2NTFY_SEM_UNLOCK(unit);

    *pStatus = (val==1) ? L2NTFY_FIFO_EMPTY : L2NTFY_FIFO_NOT_EMPTY;

    return RT_ERR_OK;
}

/* Function Name:
 *      r9300_l2ntfy_dst_get
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
r9300_l2ntfy_dst_get(uint32 unit, rtk_l2ntfy_dst_t *pDst)
{
    uint32 val;

    RT_INIT_CHK(l2ntfy_init[unit]);

    L2NTFY_SEM_LOCK(unit);
    L2NTFY_SS_112(unit, &val);
    ioal_mem32_field_read(unit, RTL9300_L2_NTFY_CTRL_ADDR, RTL9300_L2_NTFY_CTRL_NTFY_TYPE_OFFSET,
                            RTL9300_L2_NTFY_CTRL_NTFY_TYPE_MASK, &val);
    L2NTFY_SEM_UNLOCK(unit);

    switch(val)
    {
        case 0:
            *pDst = L2NTFY_DST_PKT_TO_LOCAL; break;
        case 1:
            *pDst = L2NTFY_DST_PKT_TO_MASTER; break;
        case 3:
            *pDst = L2NTFY_DST_NIC; break;
        default:
            return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      r9300_l2ntfy_dst_set
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
r9300_l2ntfy_dst_set(uint32 unit, rtk_l2ntfy_dst_t dst)
{
    uint32 val;

    RT_INIT_CHK(l2ntfy_init[unit]);

    switch(dst)
    {
        case L2NTFY_DST_PKT_TO_LOCAL:
            val = 0; break;
        case L2NTFY_DST_PKT_TO_MASTER:
            val = 1; break;
        case L2NTFY_DST_NIC:
            val = 3; break;
        default:
            return RT_ERR_INPUT;
    }

    L2NTFY_SEM_LOCK(unit);
    ioal_mem32_field_write(unit, RTL9300_L2_NTFY_CTRL_ADDR, RTL9300_L2_NTFY_CTRL_NTFY_TYPE_OFFSET,
                            RTL9300_L2_NTFY_CTRL_NTFY_TYPE_MASK, val);
    L2NTFY_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      r9300_l2ntfy_reset
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
r9300_l2ntfy_reset(uint32 unit)
{
    uint32 val=1;
    uint32 val1, val2;

    L2NTFY_SEM_LOCK(unit);
    /* SS-407*/
    do
    {
        ioal_mem32_field_write(unit, RTL9300_L2_NTFY_RST_ADDR, RTL9300_L2_NTFY_RST_RST_OFFSET,
                            RTL9300_L2_NTFY_RST_RST_MASK, val);
        ioal_mem32_read(unit, RTL9300_L2_NOTIFY_RING_BASE_ADDR_ADDR, &val1);
        ioal_mem32_read(unit, RTL9300_L2_NOTIFY_RING_CUR_ADDR_ADDR, &val2);
    }while(val1 != val2);
    L2NTFY_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      r9300_l2ntfy_iTag_get
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
r9300_l2ntfy_iTag_get(uint32 unit, rtk_l2ntfy_iTagCfg_t *pITagCfg)
{
    uint32 val;

    RT_INIT_CHK(l2ntfy_init[unit]);

    L2NTFY_SEM_LOCK(unit);
    ioal_mem32_field_read(unit, RTL9300_L2_NTFY_PKT_ITAG_0_ADDR, RTL9300_L2_NTFY_PKT_ITAG_0_ITAGIF_OFFSET,
                            RTL9300_L2_NTFY_PKT_ITAG_0_ITAGIF_MASK, &pITagCfg->iTagif);

    ioal_mem32_field_read(unit, RTL9300_L2_NTFY_PKT_ITAG_1_ADDR, RTL9300_L2_NTFY_PKT_ITAG_1_ITAG_OFFSET,
                            RTL9300_L2_NTFY_PKT_ITAG_1_ITAG_MASK, &val);
    pITagCfg->tpid = (val>>16) & 0xffff;
    pITagCfg->pri = (val>>13) & 0x7;
    pITagCfg->cfi = (val>>12) & 0x1;
    pITagCfg->vid = (val>>0) & 0xfff;
    L2NTFY_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      r9300_l2ntfy_iTag_set
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
r9300_l2ntfy_iTag_set(uint32 unit, rtk_l2ntfy_iTagCfg_t *pITagCfg)
{
    uint32 val;
    L2NTFY_SEM_LOCK(unit);
    ioal_mem32_field_write(unit, RTL9300_L2_NTFY_PKT_ITAG_0_ADDR, RTL9300_L2_NTFY_PKT_ITAG_0_ITAGIF_OFFSET,
                            RTL9300_L2_NTFY_PKT_ITAG_0_ITAGIF_MASK, pITagCfg->iTagif);

    val = (pITagCfg->tpid<<16) | (pITagCfg->pri<<13) | (pITagCfg->cfi<<12) | (pITagCfg->vid);
    ioal_mem32_field_write(unit, RTL9300_L2_NTFY_PKT_ITAG_1_ADDR, RTL9300_L2_NTFY_PKT_ITAG_1_ITAG_OFFSET,
                            RTL9300_L2_NTFY_PKT_ITAG_1_ITAG_MASK, val);
    L2NTFY_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      r9300_l2ntfy_magicNum_get
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
 * Note:
 *      None
 */
int32
r9300_l2ntfy_magicNum_get(uint32 unit, uint32 *pMagicNum)
{
    L2NTFY_SEM_LOCK(unit);
    ioal_mem32_field_read(unit, RTL9300_L2_NTFY_PKT_MAGIC_NUM_ADDR, RTL9300_L2_NTFY_PKT_MAGIC_NUM_NUM_OFFSET,
                            RTL9300_L2_NTFY_PKT_MAGIC_NUM_NUM_MASK, pMagicNum);
    L2NTFY_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      r9300_l2ntfy_magicNum_set
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
 * Note:
 *      None
 */
int32
r9300_l2ntfy_magicNum_set(uint32 unit, uint32 magicNum)
{
    RT_PARAM_CHK((magicNum > RTL9300_L2NTFY_MAGIC_NUM_MAX), RT_ERR_INPUT);

    L2NTFY_SEM_LOCK(unit);
    ioal_mem32_field_write(unit, RTL9300_L2_NTFY_PKT_MAGIC_NUM_ADDR, RTL9300_L2_NTFY_PKT_MAGIC_NUM_NUM_OFFSET,
                            RTL9300_L2_NTFY_PKT_MAGIC_NUM_NUM_MASK, magicNum);
    L2NTFY_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      r9300_l2ntfy_macAddr_get
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
r9300_l2ntfy_macAddr_get(uint32 unit, rtk_l2ntfy_addrType_t type, rtk_mac_t *pMac)
{
    uint32  val1, val2;

    L2NTFY_SEM_LOCK(unit);

    if (L2NTFY_ADDR_DMAC == type)
    {
        ioal_mem32_field_read(unit, RTL9300_L2_NTFY_PKT_MAC_ADDR, 0, 0xFFFF, &val1);
        ioal_mem32_field_read(unit, RTL9300_L2_NTFY_PKT_MAC_ADDR + 4, 0, 0xFFFFFFFF, &val2);
    }
    else
    {
        ioal_mem32_field_read(unit, RTL9300_L2_NTFY_PKT_MAC_ADDR + 8, 0, 0xFFFF, &val1);
        ioal_mem32_field_read(unit, RTL9300_L2_NTFY_PKT_MAC_ADDR + 0xc, 0, 0xFFFFFFFF, &val2);
    }

    L2NTFY_SEM_UNLOCK(unit);

    pMac->octet[0] = (val1 >> 8) & 0xff;
    pMac->octet[1] = (val1 >> 0) & 0xff;
    pMac->octet[2] = (val2 >> 24) & 0xff;
    pMac->octet[3] = (val2 >> 16) & 0xff;
    pMac->octet[4] = (val2 >> 8) & 0xff;
    pMac->octet[5] = (val2 >> 0) & 0xff;

    return RT_ERR_OK;
}

/* Function Name:
 *      r9300_l2ntfy_macAddr_set
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
r9300_l2ntfy_macAddr_set(uint32 unit, rtk_l2ntfy_addrType_t type, rtk_mac_t *pMac)
{
    uint32  val1, val2;

    val1 = (pMac->octet[0] << 8) | (pMac->octet[1] << 0);
    val2 = (pMac->octet[2] << 24) | (pMac->octet[3] << 16) | (pMac->octet[4] << 8) | (pMac->octet[5] << 0);
    L2NTFY_SEM_LOCK(unit);

    if (L2NTFY_ADDR_DMAC == type)
    {
        ioal_mem32_field_write(unit, RTL9300_L2_NTFY_PKT_MAC_ADDR, 0, 0xFFFF, val1);
        ioal_mem32_field_write(unit, RTL9300_L2_NTFY_PKT_MAC_ADDR + 4, 0, 0xFFFFFFFF, val2);
    }
    else
    {
        ioal_mem32_field_write(unit, RTL9300_L2_NTFY_PKT_MAC_ADDR + 8, 0, 0xFFFF, val1);
        ioal_mem32_field_write(unit, RTL9300_L2_NTFY_PKT_MAC_ADDR + 0xc, 0, 0xFFFFFFFF, val2);
    }

    L2NTFY_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      r9300_l2ntfy_maxEvent_get
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
r9300_l2ntfy_maxEvent_get(uint32 unit, uint32 *pNum)
{
    L2NTFY_SEM_LOCK(unit);
    ioal_mem32_field_read(unit, RTL9300_L2_NTFY_PKT_TIMEOUT_ADDR, RTL9300_L2_NTFY_PKT_TIMEOUT_MAX_EVENT_OFFSET,
                            RTL9300_L2_NTFY_PKT_TIMEOUT_MAX_EVENT_MASK, pNum);

    L2NTFY_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      r9300_l2ntfy_maxEvent_set
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
r9300_l2ntfy_maxEvent_set(uint32 unit, uint32 num)
{
    RT_PARAM_CHK((num > RTL9300_L2NTFY_PKT_EVENT_MAX) || (num < RTL9300_L2NTFY_PKT_EVENT_MIN), RT_ERR_INPUT);

    L2NTFY_SEM_LOCK(unit);
    ioal_mem32_field_write(unit, RTL9300_L2_NTFY_PKT_TIMEOUT_ADDR, RTL9300_L2_NTFY_PKT_TIMEOUT_MAX_EVENT_OFFSET,
                            RTL9300_L2_NTFY_PKT_TIMEOUT_MAX_EVENT_MASK, num);
    L2NTFY_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      r9300_l2ntfy_timeout_get
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
r9300_l2ntfy_timeout_get(uint32 unit, rtk_l2ntfy_mode_t mode, uint32 *pTimeout)
{
    RT_PARAM_CHK((L2NTFY_MODE_PKT != mode), RT_ERR_INPUT);

    L2NTFY_SEM_LOCK(unit);
    L2NTFY_SS_112(unit, pTimeout);
    ioal_mem32_field_read(unit, RTL9300_L2_NTFY_PKT_TIMEOUT_ADDR, RTL9300_L2_NTFY_PKT_TIMEOUT_TIMEOUT_OFFSET,
                            RTL9300_L2_NTFY_PKT_TIMEOUT_TIMEOUT_MASK, pTimeout);
    L2NTFY_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      r9300_l2ntfy_timeout_set
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
r9300_l2ntfy_timeout_set(uint32 unit, rtk_l2ntfy_mode_t mode, uint32 timeout)
{
    RT_PARAM_CHK((L2NTFY_MODE_PKT != mode), RT_ERR_INPUT);
    RT_PARAM_CHK((timeout > RTL9300_L2NTFY_PKT_TIMEOUT_MAX), RT_ERR_INPUT);

    L2NTFY_SEM_LOCK(unit);
    ioal_mem32_field_write(unit, RTL9300_L2_NTFY_PKT_TIMEOUT_ADDR, RTL9300_L2_NTFY_PKT_TIMEOUT_TIMEOUT_OFFSET,
                            RTL9300_L2_NTFY_PKT_TIMEOUT_TIMEOUT_MASK, timeout);
    L2NTFY_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      r9300_l2ntfy_rawToEvent_cnvt
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
r9300_l2ntfy_rawToEvent_cnvt(uint32 unit, uint8 *pRaw, rtk_l2ntfy_eventEntry_t *pEvent)
{
    l2ntfy_nBufEntry_9300_t *pEventEntry = (l2ntfy_nBufEntry_9300_t *)pRaw;

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
        pEvent->is_trk= pEventEntry->is_trk;
        pEvent->slp = pEventEntry->slp;
        pEvent->sus = pEventEntry->sus;
        pEvent->sttc= pEventEntry->sttc;
        pEvent->sablk = pEventEntry->saBlk;
        pEvent->dablk = pEventEntry->daBlk;
        pEvent->nexthop= pEventEntry->nextHop;
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
 *      r9300_l2ntfy_sizeInfo_get
 * Description:
 *      Get the ringSize/nBufSize value that chip support.
 * Input:
 *      unit        - unit id
 * Output:
 *      pRingSize    - pointer to ring size value
 *      pNBufSize    - pointer to nBuf size value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
r9300_l2ntfy_sizeInfo_get(uint32 unit, uint32 *pRingSize, uint32 *pNBufSize)
{
    /* Check init state */
    RT_INIT_CHK(l2ntfy_init[unit]);

    *pRingSize = RTL9300_L2NTFY_RING_SIZE;
    *pNBufSize = RTL9300_L2NTFY_NBUF_SIZE;

    return RT_ERR_OK;
}

/* Function Name:
 *      r9300_l2ntfy_sizeInfo_get
 * Description:
 *      Get the entry size value that chip support.
 * Input:
 *      unit        - unit id
 * Output:
 *      pEntrySize    - pointer to entry size value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
r9300_l2ntfy_entryLen_get(uint32 unit, uint32 *pEntrySize)
{
    /* Check init state */
    RT_INIT_CHK(l2ntfy_init[unit]);

    *pEntrySize = RTL9300_L2NTFY_ENTRY_BYTE;

    return RT_ERR_OK;
}

/* Function Name:
 *      r9300_l2ntfy_pktCpuQue_set
 * Description:
 *      Set the CPU queue ID that a notification packet was trapped.
 * Input:
 *      unit        - unit id
 *      queId        - CPU queue ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
r9300_l2ntfy_pktCpuQue_set(uint32 unit, uint32 queId)
{
    L2NTFY_SEM_LOCK(unit);

    ioal_mem32_field_write(unit, RTL9300_QM_RSN2CPUQID_CTRL_7_ADDR, RTL9300_QM_RSN2CPUQID_CTRL_7_L2_NTFY_OFFSET,
                            RTL9300_QM_RSN2CPUQID_CTRL_7_L2_NTFY_MASK, queId);

    L2NTFY_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      r9300_l2ntfy_pktCpuQueBwCtrlEnable_set
 * Description:
 *      Set the rate of CPU queue that a notification packet was trapped.
 * Input:
 *      unit        - unit id
 *      queId        - CPU queue ID
 *      enable      - enable status of egress queue bandwidth control
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
r9300_l2ntfy_pktCpuQueBwCtrlEnable_set(uint32 unit, uint32 queId, rtk_enable_t enable)
{
    L2NTFY_SEM_LOCK(unit);

    ioal_mem32_field_write(unit, RTL9300_EGBW_CPU_Q_MAX_LB_CTRL_ADDR(queId),  20,
                            (0x1 << 20), enable);

    L2NTFY_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      r9300_l2ntfy_pktCpuQueRate_set
 * Description:
 *      Set the rate of CPU queue that a notification packet was trapped.
 * Input:
 *      unit        - unit id
 *      queId        - CPU queue ID
 *      rate          - CPU queue rate
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
r9300_l2ntfy_pktCpuQueRate_set(uint32 unit, uint32 queId, uint32 rate)
{
    L2NTFY_SEM_LOCK(unit);

    ioal_mem32_field_write(unit, RTL9300_EGBW_CPU_Q_MAX_LB_CTRL_ADDR(queId),  0,
                            (0xFFFFF << 0), rate);

    L2NTFY_SEM_UNLOCK(unit);

    return RT_ERR_OK;
}

/* Function Name:
 *      r9300_l2ntfy_l2LearningAge_disable
 * Description:
 *      Disable per-port L2 learning ability.
 * Input:
 *      unit        - unit id
 *      arb_id      - id for arbiter
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      None
 */
int32 r9300_l2ntfy_l2LearningAge_disable(uint32 unit, uint32 arb_id)
{
    rtk_port_t  port;

    RT_PARAM_CHK(arb_id >= ARB_MAX, RT_ERR_INPUT);

    if ((NULL == _dal_longan_l2_portNewMacOp_set_cb) || (NULL == _dal_longan_l2_agingTime_set_cb))
        return RT_ERR_CB_FUNCTION_NOT_FOUND;

    /* Disable learning */
    HWP_PORT_TRAVS(unit, port)
    {
        _dal_longan_l2_portNewMacOp_set_cb(unit, port, 0x2, 0, arb_id);   //0x2: disable learning,  arg3: doesn't matter
    }


    /* Disable aging */
    _dal_longan_l2_agingTime_set_cb(unit, L2_AGE_TIME_NORMAL, 0x0, arb_id);   //0x0: disable aging

    return RT_ERR_OK;
}

/* Function Name:
 *      r9300_l2ntfy_l2LearningAge_restore
 * Description:
 *      Restore per-port L2 learning ability.
 * Input:
 *      unit        - unit id
 *      arb_id      - id for arbiter
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      None
 */
int32 r9300_l2ntfy_l2LearningAge_restore(uint32 unit, uint32 arb_id)
{
    rtk_port_t  port;

    RT_PARAM_CHK(arb_id >= ARB_MAX, RT_ERR_INPUT);


    if ((NULL == _dal_longan_l2_portNewMacOp_set_cb) || (NULL == _dal_longan_l2_agingTime_set_cb))
        return RT_ERR_CB_FUNCTION_NOT_FOUND;
    /* Restore learning */
    HWP_PORT_TRAVS(unit, port)
    {
        _dal_longan_l2_portNewMacOp_set_cb(unit, port, 0x0, 0, arb_id);   //0x0: enable learning,  arg3: doesn't matter
    }

    _dal_longan_l2_agingTime_set_cb(unit, L2_AGE_TIME_NORMAL, 0x1, arb_id);   // 0x1: enable aging

    return RT_ERR_OK;
}

/* Function Name:
 *      r9300_l2ntfy_l2_cb_register
 * Description:
 *      Register l2_portNewMacOp_set and l2_agingTime_set call back function.
 * Input:
 *      unit                 - unit id
 *      portNewMacOp_cb      - l2_portNewMacOp_set function pointer
 *      agingTime_cb         - l2_agingTime_set function pointer
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32 r9300_l2ntfy_l2_cb_register(uint32 unit, drv_l2ntfy_portNewMacOp_cb_f portNewMacOp_cb, drv_l2ntfy_agingTime_cb_f agingTime_cb)
{
    if( HWP_MY_UNIT_ID() != unit)
        return RT_ERR_FAILED;

    _dal_longan_l2_portNewMacOp_set_cb = portNewMacOp_cb;
    _dal_longan_l2_agingTime_set_cb = agingTime_cb;

    return RT_ERR_OK;
}
