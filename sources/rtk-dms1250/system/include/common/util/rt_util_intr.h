/* Copyright (C) 2009-2016 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 83734 $
 * $Date: 2017-11-24 18:52:53 +0800 (Fri, 24 Nov 2017) $
 *
 * Purpose : Definition of Interrupt control API
 *
 * Feature : The file includes the following modules
 *           (1) SWCORE
 *           (2) NIC
 *
 */
#ifndef __RT_UTIL_INTR_H__
#define __RT_UTIL_INTR_H__

/*
 * Include Files
 */
#include <common/rt_autoconf.h>
#include <common/type.h>
#include <common/rt_type.h>
#include <osal/isr.h>
#include <drv/intr/intr.h>


/*
 * Symbol Definition
 */
typedef int32 (*drv_intr_callback_t)(void *pIsr_param);
typedef uint32  rt_swcore_isrBitmap_t;

/*
 * Data Declaration
 */


/*
 * Macro Declaration
 */
/* for backward compatible */
#define rt_util_intr_swcoreIsr_register         rt_util_intr_isr_register
#define rt_util_intr_swcoreIsr_unregister       rt_util_intr_isr_unregister


/*
 * Function Declaration
 */

/* Function Name:
 *      rt_util_intr_isr_register
 * Description:
 *      Register swcore ISR callback
 * Input:
 *      isr_id - enum of ISR
 *      callback - user callback
 *      pIsr_param - user parameter. Shall give globle memory or malloc memory address.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
extern int32
rt_util_intr_isr_register(drv_intr_isr_t isr_id, drv_intr_callback_t callback, void *pIsr_param);

/* Function Name:
 *      rt_util_intr_isr_unregister
 * Description:
 *      Unregister swcore ISR callback
 * Input:
 *      isr_id - enum of ISR
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
extern int32
rt_util_intr_isr_unregister(drv_intr_isr_t isr_id);


#endif /* __RT_UTIL_INTR_H__ */



