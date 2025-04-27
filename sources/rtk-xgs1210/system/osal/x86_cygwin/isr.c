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
 * $Revision: 95661 $
 * $Date: 2019-03-11 09:59:41 +0800 (周一, 11 3月 2019) $
 *
 * Purpose : Definition those APIs interface for separating OS depend system call.
 *           Let the RTK SDK call the layer and become OS independent SDK package.
 *
 * Feature : ISR (interrupt service routine) relative API
 *
 */

/*
 * Include Files
 */
#include <osal/isr.h>
#include <common/error.h>

/* Function Name:
 *      osal_isr_register
 * Description:
 *      SDK register receive packet function.
 * Input:
 *      dev_id      - The device id defined in dev_config.h
 *      isr         - The intrrupt service routine function.
 *                    It is required to be called with 'isr_param' argument.
 *      isr_param   - The argument passed to 'isr' interrupt handler at interrupt time.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32 osal_isr_register(uint32 dev_id, osal_isr_t isr, void *isr_param)
{
    return RT_ERR_OK;
} /* osal_isr_register */

/* Function Name:
 *      osal_isr_unregister
 * Description:
 *      SDK unregister receive packet function.
 * Input:
 *      dev_id - The device id defined in device.h
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32 osal_isr_unregister(uint32 dev_id)
{
    return RT_ERR_OK;
} /* osal_isr_unregister */




