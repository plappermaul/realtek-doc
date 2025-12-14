/*
 * Copyright (C) 2011 Realtek Semiconductor Corp. 
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated, 
 * modified or distributed under the authorized license from Realtek. 
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER 
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED. 
 *
 * Purpose : Definition those APIs interface for separating OS depend system call.
 *           Let the RTK SDK call the layer and become OS independent SDK package.
 *
 * Feature : memory relative API
 *
 */

#ifndef __OSAL_MEMORY_H__
#define __OSAL_MEMORY_H__

/*
 * Include Files
 */

#include <common/type.h>

#if defined(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE) && !defined(__KERNEL__) 
  #include <stdlib.h>
  #define osal_alloc malloc
  #define osal_free free
#endif

/*
 * Symbol Definition
 */
#define MEM_DEV_NAME        "/dev/mem"

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
 *      osal_alloc
 * Description:
 *      Allocate memory based on user require size.
 * Input:
 *      size   - size of allocate memory
 * Output:
 *      None
 * Return:
 *      NULL   - failed
 *      others - pointer of the allocated memory area.
 * Note:
 *      Linux Kernel Mode - 
 *               Implemented by using kmalloc with GFP_ATOMIC flag.
 *               The maximum size of memory allocated by kmalloc is 128Kbytes.
 *               kmalloc won't sleep with GFP_ATOMIC flag.
 */
extern void *
osal_alloc(uint32 size);

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
extern void
osal_free(void *pAddr);

/* Function Name:
 *      osal_mmap
 * Description:
 *      Map files or devices into memory
 * Input:
 *      addr   - physical offset address 
 *      length - maps length bytes starting at offset address
 * Output:
 *      None
 * Return:
 *      Returns a pointer to the mapped area
 * Note:
 *      None
 */
extern void *
osal_mmap(char* dev, uint32 addr, uint32 length);

/* Function Name:
 *      osal_munmap
 * Description:
 *      Unmap files or devices in memory
 * Input:
 *      addr   - virtual offset address 
 *      length - mapped length bytes starting at offset address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      1. Deletes the mappings for the specified address range, 
 *         and causes further references to addresses within the range
 *         to generate invalid memory references.
 *      2. The region is also automatically unmapped when the process is terminated.
 */
extern int32
osal_munmap(uint32 addr, uint32 length);

#endif /* __OSAL_MEMORY_H__ */
