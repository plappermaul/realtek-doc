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
#ifndef __L2NTFY_RTL8390_H__
#define __L2NTFY_RTL8390_H__

/*
 * Include Files
 */
#include <common/error.h>
#include <private/drv/nic/nic_common.h>

/*
 * Symbol Definition
 */
#define RTL8390_L2NTFY_RING_SIZE                1700
#define RTL8390_NBUF_SIZE                       10
#define RTL8390_ENTRY_BYTE                      12
#define RTL8390_L2NTFY_BP_THR_MIN               1
#define RTL8390_L2NTFY_BP_THR_MAX               0x3ff

/*
 * Data Type Definition
 */
typedef struct l2ntfy_nBufEntry_8390_s
{
    uint32 type:2;
    uint32 fidVid:12;
    uint64 mac:48;
    uint32 slp:6;
    uint32 valid:1;
    uint32 reserved:27;
} __attribute__ ((aligned(1), packed)) l2ntfy_nBufEntry_8390_t;

typedef struct l2ntfy_nBuf_8390_s
{
    l2ntfy_nBufEntry_8390_t event[10];
} l2ntfy_nBuf_8390_t;



/*
 * Macro Definition
 */



/*
 * Function Declaration
 */

/* Module Name    : NIC     */

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
extern int32
r8390_l2ntfy_init(uint32 unit);

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
extern int32
r8390_l2ntfy_enable_get(uint32 unit, rtk_enable_t *pEnable);

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
extern int32
r8390_l2ntfy_enable_set(uint32 unit, rtk_enable_t enable);

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
extern int32
r8390_l2ntfy_backPressureThresh_get(uint32 unit, uint32 *pThresh);

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
extern int32
r8390_l2ntfy_backPressureThresh_set(uint32 unit, uint32 thresh);

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
 *      suspend learn and linkdown flush types support enable/disable state configuration.
 */
extern int32
r8390_l2ntfy_notificationEventEnable_get(uint32 unit, rtk_l2ntfy_event_t event, rtk_enable_t *pEnable);

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
 *      suspend learn and linkdown flush types support enable/disable state configuration.
 */
extern int32
r8390_l2ntfy_notificationEventEnable_set(uint32 unit, rtk_l2ntfy_event_t event, rtk_enable_t enable);

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
extern int32
r8390_l2ntfy_fifoEmptyStatus_get(uint32 unit, rtk_l2ntfy_fifoStatus_t *pStatus);


/* Function Name:
 *      r8390_l2ntfy_dst_get
 * Description:
 *      Get the destination of L2-notification mechanism.
 * Input:
 *      unit    - unit id
 * Output:
 *      pDst    - destination of L2-notification mechanism
 * Return:
 *      RT_ERR_CHIP_NOT_FOUND   - The chip can not found
 * Note:
 *      None
 */
extern int32
r8390_l2ntfy_dst_get(uint32 unit, rtk_l2ntfy_dst_t *pDst);

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
 *      RT_ERR_CHIP_NOT_FOUND   - The chip can not found
 * Note:
 *      None
 */
extern int32
r8390_l2ntfy_dst_set(uint32 unit, rtk_l2ntfy_dst_t dst);

/* Function Name:
 *      r8390_l2ntfy_reset
 * Description:
 *      Reset the local buffer & state machine of L2-notification
 * Input:
 *      unit   - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_CHIP_NOT_FOUND   - The chip can not found
 * Note:
 *      None
 */
extern int32
r8390_l2ntfy_reset(uint32 unit);

/* Function Name:
 *      r8390_l2ntfy_iTag_get
 * Description:
 *      Get the configuration of inner tag.
 * Input:
 *      unit        - unit id
 * Output:
 *      pITagCfg    - pointer to inner tag configuration
 * Return:
 *      RT_ERR_CHIP_NOT_FOUND   - The chip can not found
 * Note:
 *      None
 */
extern int32
r8390_l2ntfy_iTag_get(uint32 unit, rtk_l2ntfy_iTagCfg_t *pITagCfg);

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
 *      RT_ERR_CHIP_NOT_FOUND   - The chip can not found
 * Note:
 *      None
 */
extern int32
r8390_l2ntfy_iTag_set(uint32 unit, rtk_l2ntfy_iTagCfg_t *pITagCfg);


/* Function Name:
 *      r8390_l2ntfy_magicNum_get
 * Description:
 *      Get the magic number of L2-notification.
 * Input:
 *      unit            - unit id
 * Output:
 *      pMagicNumable   - L2-notification magic number
 * Return:
 *      RT_ERR_CHIP_NOT_FOUND   - The chip can not found
 * Note:
 *      None
 */
extern int32
r8390_l2ntfy_magicNum_get(uint32 unit, uint32 *pMagicNum);

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
 *      RT_ERR_CHIP_NOT_FOUND   - The chip can not found
 * Note:
 *      None
 */
extern int32
r8390_l2ntfy_magicNum_set(uint32 unit, uint32 magicNum);

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
 *      RT_ERR_CHIP_NOT_FOUND   - The chip can not found
 * Note:
 *      None
 */
extern int32
r8390_l2ntfy_macAddr_get(uint32 unit, rtk_l2ntfy_addrType_t type, rtk_mac_t *pMac);

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
 *      RT_ERR_CHIP_NOT_FOUND   - The chip can not found
 * Note:
 *      None
 */
extern int32
r8390_l2ntfy_macAddr_set(uint32 unit, rtk_l2ntfy_addrType_t type, rtk_mac_t *pMac);

/* Function Name:
 *      r8390_l2ntfy_maxEvent_get
 * Description:
 *      Get the maximum event number of a notification packet.
 * Input:
 *      unit    - unit id
 * Output:
 *      pNum    - pointer to maximum event number
 * Return:
 *      RT_ERR_CHIP_NOT_FOUND   - The chip can not found
 * Note:
 *      None
 */
extern int32
r8390_l2ntfy_maxEvent_get(uint32 unit, uint32 *pNum);

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
 *      RT_ERR_CHIP_NOT_FOUND   - The chip can not found
 * Note:
 *      None
 */
extern int32
r8390_l2ntfy_maxEvent_set(uint32 unit, uint32 num);

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
extern int32
r8390_l2ntfy_timeout_get(uint32 unit, rtk_l2ntfy_mode_t mode, uint32 *pTimeout);

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
extern int32
r8390_l2ntfy_timeout_set(uint32 unit, rtk_l2ntfy_mode_t mode, uint32 timeout);

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
extern int32
r8390_l2ntfy_rawToEvent_cnvt(uint32 unit, uint8 *pRaw, rtk_l2ntfy_eventEntry_t *pEvent);

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
extern int32
r8390_l2ntfy_sizeInfo_get(uint32 unit, uint32 *pRingSize, uint32 *pNBufSize);

/* Function Name:
 *      r8390_l2ntfy_entryLen_get
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
r8390_l2ntfy_entryLen_get(uint32 unit, uint32 *pEntrySize);

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
extern int32
r8390_l2ntfy_pktCpuQue_set(uint32 unit, uint32 queId);

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
extern int32
r8390_l2ntfy_l2LearningAge_disable(uint32 unit, uint32 array_index);

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
extern int32
r8390_l2ntfy_l2LearningAge_restore(uint32 unit, uint32 array_index);
#endif /* __L2NTFY_RTL8390_H__ */

