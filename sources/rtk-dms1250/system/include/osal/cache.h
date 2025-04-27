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
 * Purpose : Definition those APIs interface for separating OS depend system call.
 *           Let the RTK SDK call the layer and become OS independent SDK package.
 *
 * Feature : Cache relative API
 *
 */

#ifndef __OSAL_CACHE_H__
#define __OSAL_CACHE_H__


/*
 * Include Files
 */
#include <common/type.h>


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
 *      osal_cache_memory_flush
 * Description:
 *      Flush the specified area of cache memory.
 * Input:
 *      start_addr - start address
 *      bytes      - flush size
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK    - success.
 *      RT_ERR_INPUT - bytes is zero.
 * Note:
 *      osal_cache_memory_flush write back and invalidate dcache.
 */
extern int32
osal_cache_memory_flush(uint32 start_addr, uint32 bytes);

#endif /* __OSAL_CACHE_H__ */

