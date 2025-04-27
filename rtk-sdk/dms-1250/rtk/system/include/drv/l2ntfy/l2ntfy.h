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
 * $Revision: 104516 $
 * $Date: 2020-02-26 11:30:04 +0800 (Wed, 26 Feb 2020) $
 *
 * Purpose : Definition of L2 notification API.
 *
 * Feature : L2 notification initialization, register and unregister.
 *
 */

#ifndef __DRV_L2NTFY_H__
#define __DRV_L2NTFY_H__

#include <rtk/l2.h>
#if defined(CONFIG_SDK_DRIVER_NIC) || defined(CONFIG_SDK_DRIVER_EXTC_NIC)
#include <drv/nic/nic.h>
#endif
#include <osal/memory.h>

typedef enum rtk_l2ntfy_dst_e
{
    L2NTFY_DST_PKT_TO_LOCAL,
    L2NTFY_DST_PKT_TO_MASTER,
    L2NTFY_DST_PKT_TO_ALL,
    L2NTFY_DST_NIC,
    L2NTFY_DST_END
}rtk_l2ntfy_dst_t;

typedef enum rtk_l2ntfy_mode_e
{
    L2NTFY_MODE_PKT,
    L2NTFY_MODE_NIC,
    L2NTFY_MODE_END
}rtk_l2ntfy_mode_t;

typedef enum rtk_l2ntfy_event_e
{
    L2NTFY_EVENT_DYN,                   /* Applicable to 9310, 9300 */
    L2NTFY_EVENT_STTC,                  /* Applicable to 9310, 9300 */
    L2NTFY_EVENT_TAG,                   /* Applicable to 9310, 9300 */
    L2NTFY_EVENT_SUSPEND,               /* Applicable to 8390, 9310, 9300 */
    L2NTFY_EVENT_LINKDOWNFLUSH,         /* Applicable to 8390, 9310, 9300 */
    L2NTFY_EVENT_DASABLK,               /* Applicable to 9310, 9300 */
    L2NTFY_EVENT_HASHFULL,              /* Applicable to 9310, 9300 */
    L2NTFY_EVENT_END
} rtk_l2ntfy_event_t;

typedef enum rtk_l2ntfy_addrType_e
{
    L2NTFY_ADDR_DMAC,
    L2NTFY_ADDR_SMAC,
    L2NTFY_ADDR_END
}rtk_l2ntfy_addrType_t;

typedef enum rtk_l2ntfy_fifoStatus_e
{
    L2NTFY_FIFO_EMPTY = 0,
    L2NTFY_FIFO_NOT_EMPTY,
    L2NTFY_FIFO_END
} rtk_l2ntfy_fifoStatus_t;

typedef enum rtk_l2ntfy_runOutType_e
{
    L2_NOTIFY_BUF_NBUF = 0,
    L2_NOTIFY_BUF_ASIC_FIFO,
    L2_NOTIFY_PKT_MISS,
    L2_NOTIFY_USR_MEM,
    L2_NOTIFY_END
} rtk_l2ntfy_runOutType_t;

typedef enum rtk_l2ntfy_type_e
{
    L2NTFY_TYPE_AGEOUT_LINKDOWNFLUSH_PORTMOVEINVALID = 0,
    L2NTFY_TYPE_NEW_LEARN,
    L2NTFY_TYPE_PORTMOVE,
    L2NTFY_TYPE_HASH_FULL,
    L2NTFY_TYPE_EVENT_LOST,
    L2NTFY_TYPE_LOCAL_BUF_RUNOUT,
    L2NTFY_TYPE_END
} rtk_l2ntfy_type_t;

typedef enum rtk_l2ntfy_ringOwner_e
{
    L2NTFY_RING_OWNER_CPU = 0,
    L2NTFY_RING_OWNER_SWITCH,
    L2NTFY_RING_OWNER_END
} rtk_l2ntfy_ringOwner_t;

typedef struct rtk_l2ntfy_iTagCfg_s
{
    uint32              iTagif;
    uint32              tpid;
    rtk_vlan_t          vid;
    uint32              cfi;
    rtk_pri_t           pri;
} rtk_l2ntfy_iTagCfg_t;

typedef struct rtk_l2ntfy_eventEntry_s
{
    rtk_l2ntfy_type_t   type;
    uint32              fidVid;
    rtk_mac_t           mac;
    uint32              slp;
#if defined(CONFIG_SDK_RTL9310) ||defined(CONFIG_SDK_RTL9300)
    uint8               is_trk;
    uint8               sus;
    uint8               sttc;
    uint8               sablk;
    uint8               dablk;
    uint8               nexthop;
    uint8               l2Tnl;
    uint32              l2Tnl_idx;
    rtk_vlan_t          agg_vid;
#endif
#if defined(CONFIG_SDK_RTL9310)
    uint8               tagSts;
    rtk_pri_t           agg_pri;
    rtk_bpe_ecid_t      ecid;
#endif
} rtk_l2ntfy_eventEntry_t;

typedef struct rtk_l2ntfy_usrEventArray_s
{
    uint32                  entryNum;
    rtk_l2ntfy_eventEntry_t *eventArray;
} rtk_l2ntfy_usrEventArray_t ;

#if defined(CONFIG_SDK_DRIVER_NIC_USER_MODE)
typedef struct rtk_l2ntfy_isrParam_s
{
    uint32  curPos;
    uint32  num;
} rtk_l2ntfy_isrParam_t;
#endif

typedef enum rtk_l2ntfy_debug_e
{
    /* Rx debug flags */
    L2NTFY_DEBUG_STOP_RELEASE = 0,   /* 0 */
    L2NTFY_DEBUG_ATS_TEST,
} rtk_l2ntfy_debug_t;

typedef int32 (*drv_l2ntfy_cb_f)(uint32 unit, rtk_l2ntfy_usrEventArray_t *pEventCollectArray);
typedef int32 (*drv_l2ntfy_runoutcb_f)(uint32 unit, rtk_l2ntfy_runOutType_t type);
typedef int32 (*drv_l2ntfy_portNewMacOp_cb_f)(uint32 unit, rtk_port_t port, uint32 mode, uint32 act, uint32 arb_id);
typedef int32 (*drv_l2ntfy_agingTime_cb_f)(uint32 unit, rtk_l2_ageTimeType_t type, uint32 aging_time, uint32 arb_id);



#define L2NTFY_RX_HANDLER_PRIORITY   0
#define L2NTFY_PKT_CPU_QUEUE_PRI     14
#define L2NTFY_PKT_CPU_QUEUE_BLK     0
#define L2NTFY_RING_OWNER_MASK       0x1
#define LRN_AGE_SETTING_SHADOW_MAX   2

/* Function Name:
 *      _l2ntfy_queueCnt_get
 * Description:
 *      Get the egress used page count of the switch notification queue.
 * Input:
 *      unit    - unit id
 * Output:
 *      pCnt    - pointer to the page count-
 * Return:
 *      RT_ERR_OK
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
_l2ntfy_queueCnt_get(uint32 unit, uint32 *pCnt);

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
extern int32
drv_l2ntfy_init(uint32 unit);


/* Function Name:
 *      l2ntfy_probe
 * Description:
 *      Probe l2ntfy module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
l2ntfy_probe(uint32 unit);


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
extern int32
drv_l2ntfy_register(uint32 unit, drv_l2ntfy_cb_f cb, drv_l2ntfy_runoutcb_f runout_cb);

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
extern int32
drv_l2ntfy_unregister(uint32 unit);

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
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern drv_nic_rx_t
drv_l2ntfy_pkt_handler(uint32 unit, drv_nic_pkt_t *pPacket, void *pCookie);

#ifndef CONFIG_SDK_DRIVER_EXTC_NIC
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
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
drv_l2ntfy_isr_handler(uint32 unit, void *isr_param);
#endif

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
extern int32
drv_l2ntfy_enable_get(uint32 unit, rtk_enable_t *pEnable);

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
extern int32
drv_l2ntfy_enable_set(uint32 unit, rtk_enable_t enable);

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
extern int32
drv_l2ntfy_backPressureThresh_get(uint32 unit, uint32 *pThresh);

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
 *      RT_ERR_INPUT            - Invalid input parameter
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
extern int32
drv_l2ntfy_backPressureThresh_set(uint32 unit, uint32 thresh);

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
 *      RT_ERR_INPUT            - Invalid input parameter
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      For 8390, only suspend learn and linkdown flush types support enable/disable state configuration.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
drv_l2ntfy_notificationEventEnable_get(uint32 unit, rtk_l2ntfy_event_t event, rtk_enable_t *pEnable);

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

extern int32
drv_l2ntfy_notificationEventEnable_set(uint32 unit, rtk_l2ntfy_event_t event, rtk_enable_t enable);

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
extern int32
drv_l2ntfy_fifoEmptyStatus_get(uint32 unit, rtk_l2ntfy_fifoStatus_t *pStatus);

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
extern int32
drv_l2ntfy_counter_dump(uint32 unit);

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
extern int32
drv_l2ntfy_counter_clear(uint32 unit);

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
extern int32
drv_l2ntfy_dst_get(uint32 unit, rtk_l2ntfy_dst_t *pDst);

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
extern int32
drv_l2ntfy_dst_set(uint32 unit, rtk_l2ntfy_dst_t dst);

/* Function Name:
 *      drv_l2ntfy_sizeInfo_get
 * Description:
 *      Get ring and buffer size
 * Input:
 *      unit        - unit id
 * Output:
 *      ringSize    - ring size
 *      nBufSize    - buffer size
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - Invalid unit id
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
extern int32
drv_l2ntfy_sizeInfo_get(uint32 unit, uint32 *ringSize, uint32 *nBufSize);

/* Function Name:
 *      drv_l2ntfy_l2LearningAge_disable
 * Description:
 *      Disable L2 learning/aging ability.
 * Input:
 *      unit        - unit id
 *      id          - arbiter id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - Invalid unit id
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
drv_l2ntfy_l2LearningAge_disable(uint32 unit, rtk_lrn_arbitor_id_t id);

/* Function Name:
 *      drv_l2ntfy_l2LearningAge_restore
 * Description:
 *      Restore L2 learning/aging ability.
 * Input:
 *      unit        - unit id
 *      id          - arbiter id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - Invalid unit id
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
drv_l2ntfy_l2LearningAge_restore(uint32 unit, rtk_lrn_arbitor_id_t id);

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
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
drv_l2ntfy_iTag_get(uint32 unit, rtk_l2ntfy_iTagCfg_t *pITagCfg);

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
 *      RT_ERR_NULL_POINTER     - Input parameter may be null pointer
 *      RT_ERR_INPUT            - Invalid input parameter
 *      RT_ERR_PRIORITY         - invalid priority value
 *      RT_ERR_VLAN_VID         - invalid vlan id
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
drv_l2ntfy_iTag_set(uint32 unit, rtk_l2ntfy_iTagCfg_t *pITagCfg);

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
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
drv_l2ntfy_magicNum_get(uint32 unit, uint32 *pMagicNum);

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
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
drv_l2ntfy_magicNum_set(uint32 unit, uint32 magicNum);

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
 *      RT_ERR_UNIT_ID          - Invalid unit id
 *      RT_ERR_NULL_POINTER     - Input parameter may be null pointer
 *      RT_ERR_CHIP_NOT_FOUND   - The chip can not found
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
drv_l2ntfy_macAddr_get(uint32 unit, rtk_l2ntfy_addrType_t type, rtk_mac_t *pMac);

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
 *      RT_ERR_UNIT_ID          - Invalid unit id
 *      RT_ERR_NULL_POINTER     - Input parameter may be null pointer
 *      RT_ERR_CHIP_NOT_FOUND   - The chip can not found
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
drv_l2ntfy_macAddr_set(uint32 unit, rtk_l2ntfy_addrType_t type, rtk_mac_t *pMac);


#endif  /* __DRV_L2NTFY_H__ */

