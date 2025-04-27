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

/*
 * Include Files
 */
#include <common/rt_autoconf.h>
#include <soc/type.h>
#include <common/debug/rt_log.h>
#include <osal/memory.h>
#include <ioal/ioal_init.h>
#include <common/util/rt_util_time.h>
#include <rtcore/rtcore.h>

#if !defined(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE)
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#endif

/*
 * Symbol Definition
 */
extern ioal_db_t ioal_db[];


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
debug_mem_read(uint32 unit, uint32 addr, uint32 *pVal)
{

#if defined(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE)

    *pVal = MEM32_READ(addr);

#else

    if ( (addr < (SWCORE_VIRT_BASE + SWCORE_MEM_SIZE)) && (addr >= SWCORE_VIRT_BASE) )
    {
        addr = addr - SWCORE_VIRT_BASE + ioal_db[unit].swcore_base;
    }
    else if ( (addr < (SOC_VIRT_BASE + SOC_MEM_SIZE)) && (addr >= SOC_VIRT_BASE) )
    {
        addr = addr - SOC_VIRT_BASE + ioal_db[unit].soc_base;
    }
    else if ( (addr < (SRAM_VIRT_BASE + SRAM_MEM_SIZE_128M)) && (addr >= SRAM_VIRT_BASE) )
    {
        addr = addr - SRAM_VIRT_BASE + ioal_db[unit].sram_base;
    }
    else if ( addr >= DRAM_CACHE_VIRT_BASE )
    {
        int32 fd;
        rtcore_ioctl_t dio;
        if ((fd = open(RTCORE_DEV_NAME, O_RDWR)) < 0)
            return RT_ERR_FAILED;

        dio.data[0] = unit;
        dio.data[1] = addr;
        ioctl(fd, RTCORE_REG_READ, &dio);
        *pVal = dio.data[2];

        close(fd);

        return RT_ERR_OK;
    }


    *pVal = MEM32_READ(addr);

#endif

    return RT_ERR_OK;
} /* end of debug_mem_read */

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
debug_mem_write(uint32 unit, uint32 addr, uint32 val)
{
#if defined(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE)

    MEM32_WRITE(addr, val);

#else

    if ( (addr < (SWCORE_VIRT_BASE + SWCORE_MEM_SIZE)) && (addr >= SWCORE_VIRT_BASE) )
    {
        addr = addr - SWCORE_VIRT_BASE + ioal_db[unit].swcore_base;
    }
    else if ( (addr < (SOC_VIRT_BASE + SOC_MEM_SIZE)) && (addr >= SOC_VIRT_BASE) )
    {
        addr = addr - SOC_VIRT_BASE + ioal_db[unit].soc_base;
    }
    else if ( (addr < (SRAM_VIRT_BASE + SRAM_MEM_SIZE_128M)) && (addr >= SRAM_VIRT_BASE) )
    {
        addr = addr - SRAM_VIRT_BASE + ioal_db[unit].sram_base;
    }
    else if ( ((addr < DRAM_CACHE_VIRT_BASE + DRAM_MEM_SIZE) && (addr >= DRAM_CACHE_VIRT_BASE)) ||
               ((addr < DRAM_UNCACHE_VIRT_BASE + DRAM_MEM_SIZE) && (addr >= DRAM_UNCACHE_VIRT_BASE)) )
    {
        int32 fd;
        rtcore_ioctl_t dio;
        if ((fd = open(RTCORE_DEV_NAME, O_RDWR)) < 0)
            return RT_ERR_FAILED;

        dio.data[0] = unit;
        dio.data[1] = addr;
        dio.data[2] = val;
        ioctl(fd, RTCORE_REG_WRITE, &dio);

        close(fd);

        return RT_ERR_OK;
    }


    MEM32_WRITE(addr, val);
#endif

    return RT_ERR_OK;
} /* end of debug_mem_write */

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
debug_mem_show(uint32 unit)
{
    osal_printf("\n");
    osal_printf("The total memory allocated: %d\n", 0);
    osal_printf("Max total memory ever allocated: %d\n", 0);

    return RT_ERR_OK;
} /* end of debug_mem_write */

