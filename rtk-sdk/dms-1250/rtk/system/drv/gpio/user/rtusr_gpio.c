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
#include <common/rt_autoconf.h>
#include <common/error.h>


#include <common/util/rt_util_time.h>
#include <rtcore/rtcore.h>
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
 *      drv_gpio_isrStsShadow_get
 * Description:
 *      Get ISR status from shadow database
 * Input:
 *      unit - unit ID
 *      pin - internal GPIO pin ID
 * Output:
 *      pData - ISR status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
drv_gpio_isrStsShadow_get(uint32 unit, GPIO_INTERNAL_PIN_t pin, uint32 *pData)
{
    int32 fd;
    rtcore_ioctl_t dio;

    if ((fd = open(RTCORE_DEV_NAME, O_RDWR)) < 0)
        return RT_ERR_FAILED;

    dio.data[0] = unit;
    dio.data[1] = pin;
    ioctl(fd, RTCORE_GPIO_ISR_STS_SHADOW_GET, &dio);
    *pData = dio.data[2];

    close(fd);

    return dio.ret;
}



