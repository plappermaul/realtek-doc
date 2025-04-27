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
 * $Revision: 93380 $
 * $Date: 2018-11-08 17:49:37 +0800 (Thu, 08 Nov 2018) $
 *
 * Purpose : DRV APIs definition.
 *
 * Feature : GPIO relative API
 *
 */

/*
 * Include Files
 */
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <common/error.h>
#include <common/util/rt_util_time.h>
#include <rtcore/rtcore.h>
#include <private/drv/swcore/swcore.h>
#include <private/drv/swcore/chip_probe.h>
#include <drv/gpio/gpio.h>

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
 *      drv_gpio_pin_init
 * Description:
 *      Init GPIO port
 * Input:
 *      gpioId          - The GPIO port that will be configured
 *      function        - Pin control function
 *      direction       - Data direction, in or out
 *      interruptEnable - Interrupt mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_OUT_OF_RANGE
 * Note:
 *      None
 */
int32
drv_gpio_pin_init(
    gpioID gpioId,
    drv_gpio_control_t function,
    drv_gpio_direction_t direction,
    drv_gpio_interruptType_t interruptEnable)
{
    int32 fd;
    rtcore_ioctl_t dio;

    if ((fd = open(RTCORE_DEV_NAME, O_RDWR)) < 0)
        return RT_ERR_FAILED;

    osal_memset(&dio, 0, sizeof(rtcore_ioctl_t));

    dio.data[0] = gpioId;
    dio.data[1] = function;
    dio.data[2] = direction;
    dio.data[3] = interruptEnable;
    ioctl(fd, RTCORE_GPIO_INIT, &dio);

    close(fd);

    return dio.ret;
} /* end of drv_gpio_pin_init */

/* Function Name:
 *      drv_gpio_dataBit_get
 * Description:
 *      Get the bit value of a specified GPIO ID
 * Input:
 *      unit   - UNIT ID
 *      gpioId - GPIO ID
 * Output:
 *      pData   - Pointer to store return value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_OUT_OF_RANGE
 * Note:
 *      None
 */
int32
drv_gpio_dataBit_get(uint32 unit, gpioID gpioId, uint32 *pData)
{
    int32 fd;
    rtcore_ioctl_t dio;

    if ((fd = open(RTCORE_DEV_NAME, O_RDWR)) < 0)
        return RT_ERR_FAILED;

    osal_memset(&dio, 0, sizeof(rtcore_ioctl_t));

    dio.data[0] = gpioId;
    dio.data[2] = unit;
    ioctl(fd, RTCORE_GPIO_DATABIT_GET, &dio);
    *pData = dio.data[1];

    close(fd);

    return dio.ret;
} /* end of drv_gpio_dataBit_get */

/* Function Name:
 *      drv_gpio_dataBit_set
 * Description:
 *      Set GPIO data
 * Input:
 *      uint32 - UNIt ID
 *      gpioId - GPIO ID
 *      data   - Data to write
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_OUT_OF_RANGE
 * Note:
 *      None
 */
int32
drv_gpio_dataBit_set(uint32 unit, gpioID gpioId, uint32 data)
{
    int32 fd;
    rtcore_ioctl_t dio;

    if ((fd = open(RTCORE_DEV_NAME, O_RDWR)) < 0)
        return RT_ERR_FAILED;

    osal_memset(&dio, 0, sizeof(rtcore_ioctl_t));

    dio.data[0] = gpioId;
    dio.data[1] = data;
    dio.data[2] = unit;
    ioctl(fd, RTCORE_GPIO_DATABIT_SET, &dio);

    close(fd);

    return dio.ret;
} /* end of drv_gpio_dataBit_set */

/* Function Name:
 *      drv_gpio_dataBit_init
 * Description:
 *      Initialize the bit value of a specified GPIO ID
 * Input:
 *      unit   - UNIT ID
 *      gpioId - GPIO ID
 * Output:
 *      data   - Data to write
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_OUT_OF_RANGE
 * Note:
 *      Only the GPO pin need to call the API to init default value.
 */
int32 drv_gpio_dataBit_init(uint32 unit, gpioID gpioId, uint32 data)
{
    int32 fd;
    rtcore_ioctl_t dio;

    if ((fd = open(RTCORE_DEV_NAME, O_RDWR)) < 0)
        return RT_ERR_FAILED;

    osal_memset(&dio, 0, sizeof(rtcore_ioctl_t));

    dio.data[0] = gpioId;
    dio.data[1] = data;
    dio.data[2] = unit;
    ioctl(fd, RTCORE_GPIO_DATABIT_INIT, &dio);

    close(fd);

    return dio.ret;
} /* end of drv_gpio_dataBit_init */
