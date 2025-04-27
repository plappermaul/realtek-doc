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
#ifndef __RT_UTIL_INTRK_H__
#define __RT_UTIL_INTRK_H__

/*
 * Include Files
 */
#include <common/rt_autoconf.h>
#include <common/type.h>
#include <common/rt_type.h>
#include <drv/intr/intr.h>


/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */


/*
 * Macro Declaration
 */


/*
 * Function Declaration
 */


/* Function Name:
 *      rt_util_intrk_swcore_handler
 * Description:
 *      swcore interrupt handler
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
rt_util_intrk_swcore_handler(uint32 unit);

/* Function Name:
 *      _rt_util_intr_swcore_isr
 * Description:
 *      ISR callback handler of OSAL.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern osal_isrret_t
_rt_util_intr_swcore_isr(void *pParam);

/* Function Name:
 *      _rt_util_intr_gpio_isr
 * Description:
 *      ISR callback handler of OSAL for Internal GPIO.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern osal_isrret_t
_rt_util_intr_gpio_isr(void *pParam);

/* Function Name:
 *      _rt_util_intr_handler
 * Description:
 *      Interrupt handler
 * Input:
 *      isr_glb_sts  - ISR status bitmap
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
extern int32
_rt_util_intr_handler(uint32 *isr_glb_sts);

/* Function Name:
 *      _rt_util_intr_os_register
 * Description:
 *      Register callback to the system
 * Input:
 *      isr_id - ISR ID
 * Output:
 *      pDev_type  - device type
 *      pOsCb - callback function for osal isr registration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
_rt_util_intr_os_register(drv_intr_isr_t isr_id);

/* Function Name:
 *      _rt_util_intr_os_unregister
 * Description:
 *      Register callback to the system
 * Input:
 *      isr_id - ISR ID
 * Output:
 *      pDev_type  - device type
 *      pOsCb - callback function for osal isr registration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
_rt_util_intr_os_unregister(drv_intr_isr_t isr_id);

#endif /* __RT_UTIL_INTRK_H__ */







