/*
 * Copyright (C) 2009-2016 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 74705 $
 * $Date: 2016-12-27 17:28:41 +0800 (Tue, 27 Dec 2016) $
 *
 * Purpose : Definition those APIs interface for separating OS depend system call.
 *           Let the RTK SDK call the layer and become OS independent SDK package.
 *
 * Feature : Wait relative API
 *
 */

#ifndef __OSAL_WAIT_H__
#define __OSAL_WAIT_H__

/*
 * Include Files
 */
#include <common/type.h>

/*
 * Symbol Definition
 */

/* Function Name:
 *      osal_wait_module_create
 * Description:
 *      Create a event structure.
 * Input:
 *      None
 * Output:
 *      event_id
 * Return:
 *
 * Note:
 *
 */
extern int32
osal_wait_module_create(uint32 *event_id);

/* Function Name:
 *      osal_wait_module_destroy
 * Description:
 *      Destroy the existed event structure.
 * Input:
 *      event_id
 * Output:
 *      None
 * Return:
 *
 * Note:
 *
 */
extern int32
osal_wait_module_destroy(uint32 event_id);

/* Function Name:
 *      osal_wait_event
 * Description:
 *      Wait a event that osal_wake_up() sends.
 * Input:
 *      event_id      -
 * Output:
 *      None
 * Return:
 *
 * Note:
 *
 */
extern int32
osal_wait_event(uint32 event_id);

/* Function Name:
 *      osal_wait_event
 * Description:
 *      Wake up a event queue.
 * Input:
 *      event_id      -
 * Output:
 *      None
 * Return:
 *
 * Note:
 *
 */
extern int32
osal_wake_up(uint32 event_id);

/* Function Name:
 *      osal_wait_module_list
 * Description:
 *      List all event structure.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *
 * Note:
 *
 */
extern void
osal_wait_module_list(void);


#endif /* __OSAL_WAIT_H__ */
