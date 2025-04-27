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
 * Feature : memory relative API
 *
 */

/*
 * Include Files
 */
#include <linux/slab.h>
#include <common/debug/rt_log.h>
#include <osal/memory.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
//#define ENABLE_MEMSIZE_DEBUG /* for internal debug only */

#ifdef ENABLE_MEMSIZE_DEBUG
    static uint32 totalMemAllocated=0;
    static uint32 totalMemAllocated_max=0;
#endif

/*
 * Macro Definition
 */

/*
 * Function Declaration
 */

/* Function Name:
 *      osal_alloc
 * Description:
 *      Allocate memory based on user require size.
 * Input:
 *      size - size of allocate memory
 * Output:
 *      None
 * Return:
 *      pointer of the allocated memory area.
 * Note:
 *      linux - Implemented by using kmalloc with GFP_ATOMIC flag.
 *              The maximum size of memory allocated by kmalloc is 128Kbytes.
 *              kmalloc won't sleep with GFP_ATOMIC flag.
 */
void *
osal_alloc(uint32 size)
{
    void *p;

    p = kmalloc((size_t)size, GFP_ATOMIC);
#ifdef ENABLE_MEMSIZE_DEBUG
    if(p)
    {
        totalMemAllocated += ksize(p);
        if(totalMemAllocated > totalMemAllocated_max)
            totalMemAllocated_max = totalMemAllocated;
    }
#endif
    return p;
} /* end of osal_alloc */

/* Function Name:
 *      osal_free
 * Description:
 *      Free the memory buffer
 * Input:
 *      pAddr - address of buffer that want to free
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
void
osal_free(void *pAddr)
{
#ifdef ENABLE_MEMSIZE_DEBUG
    if(pAddr)
    {
        if(totalMemAllocated < ksize(pAddr))
            osal_printf("%s:ERROR!\n",__FUNCTION__);

        totalMemAllocated -= ksize(pAddr);
    }
#endif

    kfree(pAddr);
    pAddr = NULL;

} /* end of osal_free */

/* Function Name:
 *      osal_getAllocatedMem
 * Description:
 *      Get the total memory alllocated
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      uint32: the size
 * Note:
 *      None
 */
uint32
osal_getAllocatedMem(void)
{
#ifdef ENABLE_MEMSIZE_DEBUG
    return totalMemAllocated;
#else
    //osal_printf("memory size debug is disabled.\n");
    return 0;
#endif
} /* end of osal_getAllocatedMem */

/* Function Name:
 *      osal_getAllocatedMemMax
 * Description:
 *      Get maximum value of the total memory ever alllocated
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      uint32: the size
 * Note:
 *      None
 */
uint32
osal_getAllocatedMemMax(void)
{
#ifdef ENABLE_MEMSIZE_DEBUG
    return totalMemAllocated_max;
#else
    //osal_printf("memory size debug is disabled.\n");
    return 0;
#endif
} /* end of osal_getAllocatedMem */

