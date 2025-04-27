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
 * $Revision$
 * $Date$
 *
 * Purpose : BSP APIs definition.
 *
 * Feature : MEM relative API
 *
 */
#ifndef __DEBUG_MEM_H__
#define __DEBUG_MEM_H__

/*
 * Include Files
 */
#include <common/error.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */

/*
 * Macro Definition
 */

/*
 * Function Declaration
 */

/* Function Name:
 *      debug_mem_read
 * Description:
 *      Get the value from memory.
 * Input:
 *      unit - unit id
 *      addr - register address
 * Output:
 *      pVal - pointer buffer of the register value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      1. Support single unit right now and ignore unit
 *      2. When we support the multiple chip in future, we will check the input unit
 */
int32
debug_mem_read(uint32 unit, uint32 addr, uint32 *pVal);

/* Function Name:
 *      debug_mem_write
 * Description:
 *      Set the value to memory.
 * Input:
 *      unit - unit id
 *      addr - register address
 *      val  - the value to write register
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      1. Support single unit right now and ignore unit
 *      2. When we support the multiple chip in future, we will check the input unit
 */
int32
debug_mem_write(uint32 unit, uint32 addr, uint32 val);

/* Function Name:
 *      debug_mem_show
 * Description:
 *      Show sdk memory utilization (by kmalloc or malloc)
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      1. Support single unit right now and ignore unit
 *      2. When we support the multiple chip in future, we will check the input unit
 */
int32
debug_mem_show(uint32 unit);

#endif/* __DEBUG_MEM_H__ */
