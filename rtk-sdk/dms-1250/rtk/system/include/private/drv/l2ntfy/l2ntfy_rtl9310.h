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
 * Feature : The file have include the following module and sub-modules
 *            1) CPU tag
 *            2) NIC tx
 *            3) NIC rx
 *
 */
#ifndef __L2NTFY_RTL9310_H__
#define __L2NTFY_RTL9310_H__

/*
 * Include Files
 */
#include <common/error.h>

/*
 * Symbol Definition
 */
#define RTL9310_L2NTFY_RING_SIZE                6000
#define NBUF_SIZE_9310                          8
#define RTL9310_ENTRY_BYTE                      13
#define RTL9310_L2NTFY_BP_THR_MIN               1
#define RTL9310_L2NTFY_BP_THR_MAX               0x7ff
#define RTL9310_L2NTFY_EVENT_MIN                1
#define RTL9310_L2NTFY_EVENT_MAX                0x7f
#define RTL9310_L2NTFY_PKT_TIMEOUT_MAX          0xfffff
#define RTL9310_L2NTFY_NIC_TIMEOUT_MAX          0xffff

/*
 * Data Type Definition
 */
typedef struct l2ntfy_nBufEntry_9310_s
{
    uint32 type:2;
    uint32 fidVid:12;
    uint64 mac:48;
    uint32 is_trk:1;
    uint32 slp:10;
    uint32 sus:1;
    uint32 sttc:1;
    uint32 sablk:1;
    uint32 dablk:1;
    uint32 nexthop:1;
    uint32 tagSts:1;
    uint32 agg_pri:3;
    uint32 agg_vid:12;
    uint32 ecid_rsvd:4;
    uint32 valid:1;
    uint32 reserved_1:5;
    uint16 reserved_2;
    uint8  reserved_3;
} __attribute__ ((aligned(1), packed)) l2ntfy_nBufEntry_9310_t;

typedef struct l2ntfy_nBufEntry_9310p_s
{
    uint32 type:2;
    uint32 fidVid:12;
    uint64 mac:48;
    uint32 is_trk:1;
    uint32 slp:10;
    uint32 sus:1;
    uint32 sttc:1;
    uint32 sablk:1;
    uint32 dablk:1;
    uint32 nexthop:1;
    uint32 l2Tnl:1;
    uint32 tagSts:1;
    uint32 agg_pri:3;
    uint32 agg_vid:12;
    uint32 ecid_rsvd:4;
    uint32 valid:1;
    uint32 reserved_1:4;
    uint16 reserved_2;
    uint8  reserved_3;
} __attribute__ ((aligned(1), packed)) l2ntfy_nBufEntry_9310p_t;

typedef struct l2ntfy_nBuf_9310_s
{
    l2ntfy_nBufEntry_9310_t event[NBUF_SIZE_9310];
} l2ntfy_nBuf_9310_t;



/*
 * Macro Definition
 */



/*
 * Function Declaration
 */

/* Module Name    : NIC     */

/* Function Name:
 *      r9310_l2ntfy_init
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
extern int32
r9310_l2ntfy_init(uint32 unit);

/* Function Name:
 *      r9310_l2ntfy_enable_get
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
r9310_l2ntfy_enable_get(uint32 unit, rtk_enable_t *pEnable);

/* Function Name:
 *      r9310_l2ntfy_enable_set
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
extern int32
r9310_l2ntfy_enable_set(uint32 unit, rtk_enable_t enable);

/* Function Name:
 *      r9310_l2ntfy_backPressureThresh_get
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
extern int32
r9310_l2ntfy_backPressureThresh_get(uint32 unit, uint32 *pThresh);

/* Function Name:
 *      r9310_l2ntfy_backPressureThresh_set
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
extern int32
r9310_l2ntfy_backPressureThresh_set(uint32 unit, uint32 thresh);

/* Function Name:
 *      r9310_l2ntfy_notificationEventEnable_get
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
extern int32
r9310_l2ntfy_notificationEventEnable_get(uint32 unit, rtk_l2ntfy_event_t event, rtk_enable_t *pEnable);

/* Function Name:
 *      r9310_l2ntfy_notificationEventEnable_set
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
extern int32
r9310_l2ntfy_notificationEventEnable_set(uint32 unit, rtk_l2ntfy_event_t event, rtk_enable_t enable);

/* Function Name:
 *      r9310_l2ntfy_fifoEmptyStatus_get
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
extern int32
r9310_l2ntfy_fifoEmptyStatus_get(uint32 unit, rtk_l2ntfy_fifoStatus_t *pStatus);

/* Function Name:
 *      r9310_l2ntfy_dst_get
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
extern int32
r9310_l2ntfy_dst_get(uint32 unit, rtk_l2ntfy_dst_t *pDst);

/* Function Name:
 *      r9310_l2ntfy_dst_set
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
extern int32
r9310_l2ntfy_dst_set(uint32 unit, rtk_l2ntfy_dst_t dst);

/* Function Name:
 *      r9310_l2ntfy_reset
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
extern int32
r9310_l2ntfy_reset(uint32 unit);

/* Function Name:
 *      r9310_l2ntfy_iTag_get
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
extern int32
r9310_l2ntfy_iTag_get(uint32 unit, rtk_l2ntfy_iTagCfg_t *pITagCfg);

/* Function Name:
 *      r9310_l2ntfy_iTag_set
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
extern int32
r9310_l2ntfy_iTag_set(uint32 unit, rtk_l2ntfy_iTagCfg_t *pITagCfg);

/* Function Name:
 *      r9310_l2ntfy_magicNum_get
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
extern int32
r9310_l2ntfy_magicNum_get(uint32 unit, uint32 *pMagicNum);

/* Function Name:
 *      r9310_l2ntfy_magicNum_set
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
extern int32
r9310_l2ntfy_magicNum_set(uint32 unit, uint32 magicNum);

/* Function Name:
 *      r9310_l2ntfy_macAddr_get
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
extern int32
r9310_l2ntfy_macAddr_get(uint32 unit, rtk_l2ntfy_addrType_t type, rtk_mac_t *pMac);

/* Function Name:
 *      r9310_l2ntfy_macAddr_set
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
extern int32
r9310_l2ntfy_macAddr_set(uint32 unit, rtk_l2ntfy_addrType_t type, rtk_mac_t *pMac);

/* Function Name:
 *      r9310_l2ntfy_maxEvent_get
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
extern int32
r9310_l2ntfy_maxEvent_get(uint32 unit, uint32 *pNum);

/* Function Name:
 *      r9310_l2ntfy_maxEvent_set
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
 *      RT_ERR_INPUT            - Invalid input parameter
 * Note:
 *      None
 */
extern int32
r9310_l2ntfy_maxEvent_set(uint32 unit, uint32 num);

/* Function Name:
 *      r9310_l2ntfy_timeout_get
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
extern int32
r9310_l2ntfy_timeout_get(uint32 unit, rtk_l2ntfy_mode_t mode, uint32 *pTimeout);

/* Function Name:
 *      r9310_l2ntfy_timeout_set
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
 *      RT_ERR_INPUT            - Invalid input parameter
 * Note:
 *      None
 */
extern int32
r9310_l2ntfy_timeout_set(uint32 unit, rtk_l2ntfy_mode_t mode, uint32 timeout);

/* Function Name:
 *      r9310_l2ntfy_rawToEvent_cnvt
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
extern int32
r9310_l2ntfy_rawToEvent_cnvt(uint32 unit, uint8 *pRaw, rtk_l2ntfy_eventEntry_t *pEvent);

/* Function Name:
 *      r9310_l2ntfy_sizeInfo_get
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
extern int32
r9310_l2ntfy_sizeInfo_get(uint32 unit, uint32 *ringSize, uint32 *nBufSize);

/* Function Name:
 *      r9310_l2ntfy_entryLen_get
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
extern int32
r9310_l2ntfy_entryLen_get(uint32 unit, uint32 *pEntrySize);

/* Function Name:
 *      r9310_l2ntfy_pktCpuQue_set
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
extern int32
r9310_l2ntfy_pktCpuQue_set(uint32 unit, uint32 queId);

/* Function Name:
 *      r9310_l2ntfy_pktCpuQueBwCtrlEnable_set
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
extern int32
r9310_l2ntfy_pktCpuQueBwCtrlEnable_set(uint32 unit, uint32 queId, rtk_enable_t enable);

/* Function Name:
 *      r9310_l2ntfy_pktCpuQueRate_set
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
extern int32
r9310_l2ntfy_pktCpuQueRate_set(uint32 unit, uint32 queId, uint32 rate);

/* Function Name:
 *      r9310_l2ntfy_l2LearningAge_disable
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
extern int32
r9310_l2ntfy_l2LearningAge_disable(uint32 unit, uint32 arb_id);

/* Function Name:
 *      r9310_l2ntfy_l2LearningAge_restore
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
extern int32
r9310_l2ntfy_l2LearningAge_restore(uint32 unit, uint32 arb_id);

/* Function Name:
 *      r9310_l2ntfy_l2_cb_register
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
extern int32
r9310_l2ntfy_l2_cb_register(uint32 unit, drv_l2ntfy_portNewMacOp_cb_f portNewMacOp_cb, drv_l2ntfy_agingTime_cb_f agingTime_cb);

/* Function Name:
 *      r9310_l2ntfy_queueCnt_get
 * Description:
 *      Get the egress used page count of the switch notification queue.
 * Input:
 *      unit    - unit id
 * Output:
 *      pCnt    - pointer to the page count
 * Return:
 *      RT_ERR_OK
 * Note:
 *      None
 */
extern int32
r9310_l2ntfy_queueCnt_get(uint32 unit, uint32 *pCnt);
#endif /* __L2NTFY_RTL9310_H__ */

