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
 * $Revision: 39101 $
 * $Date: 2013-05-03 17:35:27 +0800 (Fri, 03 May 2013) $
 *
 * Purpose : Definition of CPU tag API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) CPU tag functions set/get
 */

#ifndef __DAL_RTL9607C_CPU_H__
#define __DAL_RTL9607C_CPU_H__


/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/cpu.h>

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
 *      dal_rtl9607c_cpu_init
 * Description:
 *      Initialize cpu tag module.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize classification module before calling any classification APIs.
 */
extern int32
dal_rtl9607c_cpu_init(void);

/* Function Name:
 *      dal_rtl9607c_cpu_trapInsertTagByPort_set
 * Description:
 *      Set trap CPU insert tag state.
 * Input:
 *      port     - CPU port ID
 *      state    - insert CPU tag state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_OUT_OF_RANGE
 * Note:
 *      Must initialize cpu tag module before calling any cpu tag APIs.
 */
extern int32
dal_rtl9607c_cpu_trapInsertTagByPort_set(rtk_port_t port, rtk_enable_t state);

/* Function Name:
 *      dal_rtl9607c_cpu_trapInsertTagByPort_get
 * Description:
 *      Get trap CPU insert tag state.
 * Input:
 *      port     - CPU port ID
 *      pState   - the pointer of insert CPU tag state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      Must initialize cpu tag module before calling any cpu tag APIs.
 */
extern int32
dal_rtl9607c_cpu_trapInsertTagByPort_get(rtk_port_t port, rtk_enable_t *pState);

/* Function Name:
 *      dal_rtl9607c_cpu_tagAwareByPort_set
 * Description:
 *      Set CPU tag aware state.
 * Input:
 *      port     - CPU port ID
 *      state    - CPU tag aware state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      Must initialize cpu tag module before calling any cpu tag APIs.
 */
extern int32
dal_rtl9607c_cpu_tagAwareByPort_set(rtk_port_t port, rtk_enable_t state);

/* Function Name:
 *      dal_rtl9607c_cpu_tagAwareByPort_get
 * Description:
 *      Get CPU tag aware state.
 * Input:
 *      port     - CPU port ID
 *      pState   - the pointer of CPU tag aware state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      Must initialize cpu tag module before calling any cpu tag APIs.
 */
extern int32
dal_rtl9607c_cpu_tagAwareByPort_get(rtk_port_t port, rtk_enable_t *pState);


#endif /*#ifndef __DAL_RTL9607C_CPU_H__*/
