/*
 * Copyright (C) 2009-2016 Realtek Semiconductor Corp.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
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

/*
 * Include Files
 */
#include <asm/io.h>
#include <common/debug/rt_log.h>
#include <osal/cache.h>

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
int32
osal_cache_memory_flush(uint32 start_addr, uint32 bytes)
{
    /* Check the number of bytes */
    RT_INTERNAL_PARAM_CHK((0 == bytes), RT_ERR_INPUT);

#ifndef CONFIG_SDK_EXTERNAL_CPU
    /* write back io buffer and flush and invalidate dcache */
    dma_cache_wback_inv((unsigned long)start_addr, (unsigned long)bytes);
#endif /* !CONFIG_SDK_EXTERNAL_CPU */

    return RT_ERR_OK;
} /* end of osal_cache_memory_flush */

