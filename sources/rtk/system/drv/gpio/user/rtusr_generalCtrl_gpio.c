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
 * Purpose : Definition those public GPIO routing APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *           (1) Internal GPIO
 *
 */

/*
 * Include Files
 */
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <common/debug/rt_log.h>
#include <dev_config.h>
#include <soc/type.h>
#include <drv/gpio/generalCtrl_gpio.h>
#include <common/util/rt_util_time.h>
#include <rtcore/rtcore.h>
#include <common/error.h>


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
 *      drv_generalCtrlGPIO_dev_init
 * Description:
 *      Init Internal/External GPIO dev
 * Input:
 *	  unit		 -
 *      dev              - dev 0 = Internal GPIO, others are External GPIO
 *      gpioId          - The internal/external GPIO pin that will be configured
 *      pData           - Internal/External GPIO pin configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_OUT_OF_RANGE
 *      RT_ERR_DRIVER_NOT_FOUND
 * Note:
 *      None
 */
int32
drv_generalCtrlGPIO_dev_init(
    uint32 unit,
    drv_generalCtrlGpio_devId_t dev,
    uint32 pinId,
    drv_generalCtrlGpio_devConf_t *pData)
{
    int32 fd;
    rtcore_ioctl_t dio;

    if ((fd = open(RTCORE_DEV_NAME, O_RDWR)) < 0)
        return RT_ERR_FAILED;

    osal_memset(&dio, 0, sizeof(rtcore_ioctl_t));

    dio.data[0] = unit;
    dio.data[1] = dev;
    dio.data[2] = pinId;
    if(dev == GEN_GPIO_DEV_ID0_INTERNAL)
    {

    }else{
        dio.data[3] = pData->direction;
        dio.data[4] = pData->default_value;
        dio.data[5] = pData->ext_gpio.access_mode;
        dio.data[6] = pData->ext_gpio.address;
        dio.data[7] = pData->ext_gpio.page;
    }

    ioctl(fd, RTCORE_GENCTRL_GPIO_DEV_INIT, &dio);

    close(fd);

    return dio.ret;
}

/* Function Name:
 *      drv_generalCtrlGPIO_pin_init
 * Description:
 *      Init Internal/External GPIO pin for simple input/output function
 * Input:
 *	  unit		 -
 *      dev              - dev 0 = Internal GPIO, others are External GPIO
 *      gpioId          - The internal/external GPIO pin that will be configured
 *      pData           - Internal/External GPIO pin configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_OUT_OF_RANGE
 *      RT_ERR_DRIVER_NOT_FOUND
 * Note:
 *      None
 */
int32
drv_generalCtrlGPIO_pin_init(
    uint32 unit,
    drv_generalCtrlGpio_devId_t dev,
    uint32 pinId,
    drv_generalCtrlGpio_pinConf_t *pData)
{
    int32 fd;
    rtcore_ioctl_t dio;

    if ((fd = open(RTCORE_DEV_NAME, O_RDWR)) < 0)
        return RT_ERR_FAILED;

    osal_memset(&dio, 0, sizeof(rtcore_ioctl_t));

    dio.data[0] = unit;
    dio.data[1] = dev;
    dio.data[2] = pinId;

    if(dio.data[1] == GEN_GPIO_DEV_ID0_INTERNAL)
    {
        dio.data[3] = pData->direction;
        dio.data[4] = pData->default_value;
        dio.data[5] = pData->int_gpio.function;
        dio.data[6] = pData->int_gpio.interruptEnable;
    }else{
        dio.data[3] = pData->direction;
        dio.data[4] = pData->default_value;

        dio.data[5] = pData->ext_gpio.direction;
        dio.data[6] = pData->ext_gpio.debounce;
        dio.data[7] = pData->ext_gpio.inverter;
    }

    ioctl(fd, RTCORE_GENCTRL_GPIO_PIN_INIT, &dio);

    close(fd);

    return dio.ret;

}


/* Function Name:
 *      drv_generalCtrlGPIO_devEnable_set
 * Description:
 *      Enable Internal/External GPIO dev
 * Input:
 *	  unit		 -
 *      dev              - dev 0 = Internal GPIO, others are External GPIO
 *      enable         - the status of the specified external GPIO device
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_OUT_OF_RANGE
 *      RT_ERR_DRIVER_NOT_FOUND
 * Note:
 *      None
 */
int32
drv_generalCtrlGPIO_devEnable_set(
    uint32 unit,
    drv_generalCtrlGpio_devId_t dev,
     rtk_enable_t enable)
{

    int32 fd;
    rtcore_ioctl_t dio;

    if ((fd = open(RTCORE_DEV_NAME, O_RDWR)) < 0)
        return RT_ERR_FAILED;

    osal_memset(&dio, 0, sizeof(rtcore_ioctl_t));

    dio.data[0] = unit;
    dio.data[1] = dev;
    dio.data[2] = enable;

    ioctl(fd, RTCORE_GENCTRL_GPIO_DEV_ENABLE_SET, &dio);

    close(fd);

    return dio.ret;

}


/* Function Name:
 *      drv_generalCtrlGPIO_dataBit_set
 * Description:
 *      Init Internal/External GPIO pin for simple input/output function
 * Input:
 *	  unit		 -
 *      dev              - dev 0 = Internal GPIO, others are External GPIO
 *      gpioId          - The internal/external GPIO pin that will be configured
 *      data            - Write out data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_OUT_OF_RANGE
 *      RT_ERR_DRIVER_NOT_FOUND
 * Note:
 *      None
 */
int32
drv_generalCtrlGPIO_dataBit_set(
    uint32 unit,
    drv_generalCtrlGpio_devId_t dev,
    uint32 pinId,
    uint32 data)
{
    int32 fd;
    rtcore_ioctl_t dio;

    if ((fd = open(RTCORE_DEV_NAME, O_RDWR)) < 0)
        return RT_ERR_FAILED;

    osal_memset(&dio, 0, sizeof(rtcore_ioctl_t));

    dio.data[0] = unit;
    dio.data[1] = dev;
    dio.data[2] = pinId;
    dio.data[3] = data;

    ioctl(fd, RTCORE_GENCTRL_GPIO_DATABIT_SET, &dio);

    close(fd);

    return dio.ret;

}

/* Function Name:
 *      drv_generalCtrlGPIO_dataBit_get
 * Description:
 *      Init Internal/External GPIO pin for simple input/output function
 * Input:
 *	  unit		 -
 *      dev              - dev 0 = Internal GPIO, others are External GPIO
 *      gpioId          - The internal/external GPIO pin that will be configured
 *      *pData         - Read back data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_OUT_OF_RANGE
 *      RT_ERR_DRIVER_NOT_FOUND
 * Note:
 *      None
 */
int32
drv_generalCtrlGPIO_dataBit_get(
    uint32 unit,
    drv_generalCtrlGpio_devId_t dev,
    uint32 pinId,
    uint32 *pData)
{
    int32 fd;
    rtcore_ioctl_t dio;

    if ((fd = open(RTCORE_DEV_NAME, O_RDWR)) < 0)
        return RT_ERR_FAILED;

    osal_memset(&dio, 0, sizeof(rtcore_ioctl_t));

    dio.data[0] = unit;
    dio.data[1] = dev;
    dio.data[2] = pinId;

    ioctl(fd, RTCORE_GENCTRL_GPIO_DATABIT_GET, &dio);

    *pData = dio.data[3];

    close(fd);

    return dio.ret;

}


