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
 * $Revision: 71708 $
 * $Date: 2016-09-19 11:31:17 +0800 (Mon, 19 Sep 2016) $
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
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <common/error.h>
#include <osal/memory.h>

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
 *      None
 */
void *
osal_alloc(uint32 size)
{
    return malloc(size);
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
    free(pAddr);
} /* end of osal_free */

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
void *
osal_mmap(char* dev, uint32 addr, uint32 length)
{
    int32 fd;
    void *vbase;

    if ((fd = open(dev, O_RDWR)) < 0)
        return (void *)RT_ERR_FAILED;

    vbase = mmap((void *)0, length, (PROT_READ | PROT_WRITE), MAP_SHARED, fd, addr);
    close(fd);

    if(MAP_FAILED == vbase)
        return (void *)RT_ERR_FAILED;

    return vbase;
} /* end of osal_mmap */

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
int32
osal_munmap(uint32 addr, uint32 length)
{
    return munmap((void *)addr, length) ? RT_ERR_FAILED : RT_ERR_OK;
} /* end of osal_munmap */
