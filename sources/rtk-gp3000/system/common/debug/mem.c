/*
 * Copyright (C) 2009 Realtek Semiconductor Corp.
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

/*
 * Include Files
 */
//#include <soc/soc.h>
#include <soc/type.h>
#include <common/debug/rt_log.h>

/*
 * Symbol Definition
 */
#define SOC_MEM_BASE    0xB8000000

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
debug_mem_read(uint32 addr, uint32 *pVal)
{
    /* Upper layer have check the unit, and don't need to check again */
    if ((addr & SOC_MEM_BASE) != SOC_MEM_BASE)
        return RT_ERR_FAILED;

    *pVal = MEM32_READ(addr);

    return RT_ERR_OK;
} /* end of debug_mem_read */

/* Function Name:
 *      debug_mem_write
 * Description:
 *      Set the value to memory.
 * Input:
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
debug_mem_write(uint32 addr, uint32 val)
{
    /* Upper layer have check the unit, and don't need to check again */
    if ((addr & SOC_MEM_BASE) != SOC_MEM_BASE)
        return RT_ERR_FAILED;

    MEM32_WRITE(addr, val);

    return RT_ERR_OK;
} /* end of debug_mem_write */
