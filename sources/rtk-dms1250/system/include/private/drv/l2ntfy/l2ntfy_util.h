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
 * Purpose : Definition of L2 notification API.
 *
 * Feature : L2 notification initialization, register and unregister.
 *
 */

#ifndef __DRV_L2NTFY_UTIL_H__
#define __DRV_L2NTFY_UTIL_H__


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
extern int32
drv_l2ntfy_isr_handler(uint32 unit, void *isr_param);

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
extern int32
drv_l2ntfy_bufRunout_handler(uint32 unit, void *isr_param);

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
extern int32
drv_l2ntfy_localBufRunout_handler(uint32 unit, void *isr_param);

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
 *      RT_ERR_NULL_POINTER     - Input parameter may be null pointer
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      None
 */
extern int32
drv_l2ntfy_debug_get(uint32 unit, uint32 *pFlags);

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
 * Applicable:
 *      8390, 9300, 9310
 * Note:
 *      None
 */
extern int32
drv_l2ntfy_debug_set(uint32 unit, uint32 flags);

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
extern int32
drv_l2ntfy_event_dump(uint32 unit);

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
extern int32
drv_l2ntfy_reset(uint32 unit);

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
 *      RT_ERR_UNIT_ID          - Invalid unit id
 *      RT_ERR_CHIP_NOT_FOUND   - The chip can not found
 *      RT_ERR_NULL_POINTER     - Input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 */
extern int32
drv_l2ntfy_maxEvent_get(uint32 unit, uint32 *pNum);

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
 *      RT_ERR_UNIT_ID          - Invalid unit id
 *      RT_ERR_INPUT            - Invalid input parameter
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 */
extern int32
drv_l2ntfy_maxEvent_set(uint32 unit, uint32 num);

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
 *      RT_ERR_UNIT_ID          - Invalid unit id
 *      RT_ERR_CHIP_NOT_FOUND   - The chip can not found
 *      RT_ERR_NULL_POINTER     - Input parameter may be null pointer
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 */
extern int32
drv_l2ntfy_timeout_get(uint32 unit, rtk_l2ntfy_mode_t mode, uint32 *pTimeout);

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
 *      RT_ERR_UNIT_ID          - Invalid unit id
 *      RT_ERR_INPUT            - Invalid input parameter
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 */
extern int32
drv_l2ntfy_timeout_set(uint32 unit, rtk_l2ntfy_mode_t mode, uint32 timeout);

#endif  /* __DRV_L2NTFY_UTIL_H__ */

