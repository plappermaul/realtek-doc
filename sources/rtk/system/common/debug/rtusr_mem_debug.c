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
#include <soc/type.h>
#include <common/debug/rt_log.h>
#include <ioal/ioal_init.h>
#include <common/util/rt_util_time.h>
#include <rtcore/rtcore.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

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
    int32 fd;
    rtcore_ioctl_t dio;

    if ((fd = open(RTCORE_DEV_NAME, O_RDWR)) < 0)
        return RT_ERR_FAILED;

    osal_memset(&dio, 0, sizeof(rtcore_ioctl_t));

    dio.data[0] = unit;
    dio.data[1] = addr;
    ioctl(fd, RTCORE_REG_READ, &dio);
    *pVal = dio.data[2];

    close(fd);

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
    int32 fd;
    rtcore_ioctl_t dio;
    if ((fd = open(RTCORE_DEV_NAME, O_RDWR)) < 0)
        return RT_ERR_FAILED;

    osal_memset(&dio, 0, sizeof(rtcore_ioctl_t));

    dio.data[0] = unit;
    dio.data[1] = addr;
    dio.data[2] = val;
    ioctl(fd, RTCORE_REG_WRITE, &dio);

    close(fd);

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


    return RT_ERR_OK;
} /* end of debug_mem_write */
