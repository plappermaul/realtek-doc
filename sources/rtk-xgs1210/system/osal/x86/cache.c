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
 * $Revision: 71708 $
 * $Date: 2016-09-19 11:31:17 +0800 (Mon, 19 Sep 2016) $
 *
 * Purpose : Definition those APIs interface for separating OS depend system call.
 *           Let the RTK SDK call the layer and become OS independent SDK package.
 *
 * Feature : Cache relative API
 *
 */

/*
 * Include Files
 */
#include <osal/cache.h>
#include <asm/io.h>

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
 *      RT_ERR_OK
 * Note:
 *      None
 */
int32 osal_cache_memory_flush(uint32 start_addr, uint32 bytes)
{
    dma_cache_wback_inv(start_addr, bytes);
    return RT_ERR_OK;
} /* osal_cache_memory_flush */
