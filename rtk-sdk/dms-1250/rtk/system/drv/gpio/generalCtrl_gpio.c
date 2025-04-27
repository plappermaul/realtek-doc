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

#include <common/debug/rt_log.h>
#include <osal/lib.h>
#include <drv/gpio/gpio.h>
#include <soc/type.h>
#include <drv/gpio/ext_gpio.h>
#include <drv/gpio/generalCtrl_gpio.h>


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
 *	    unit		- unit id
 *      dev         - dev 0 = Internal GPIO, others are External GPIO
 *      pData       - Internal/External GPIO pin configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_OUT_OF_RANGE
 *      RT_ERR_DRIVER_NOT_FOUND
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
drv_generalCtrlGPIO_dev_init(
    uint32 unit,
    drv_generalCtrlGpio_devId_t dev,
    drv_generalCtrlGpio_devConf_t *pData)
{
    if(IS_INVALID_GEN_CTRL_GPIO_DEV(dev))
        return RT_ERR_OUT_OF_RANGE;

    RT_LOG(LOG_FUNC_ENTER, MOD_GENERAL, "[%s():%d] gpiodev=%d\n", __FUNCTION__, __LINE__, dev);

    if(IS_INT_GPIO_DEV(dev))
    {
        return RT_ERR_OK;
    }else{
#ifdef CONFIG_SDK_RTL8231
        int32                 ret;
        drv_extGpio_devConf_t ext_gpio_conf;

        osal_memset(&ext_gpio_conf, 0, sizeof(drv_extGpio_devConf_t));
        RT_LOG(LOG_FUNC_ENTER, MOD_GENERAL, "[%s():%d] access_mode=%d, address=%d\n",
                __FUNCTION__, __LINE__, pData->ext_gpio.access_mode, pData->ext_gpio.address);

        ext_gpio_conf.access_mode = pData->ext_gpio.access_mode;
        ext_gpio_conf.address = pData->ext_gpio.address;
        ext_gpio_conf.page = pData->ext_gpio.page;

        if((ret = drv_extGpio_dev_init(unit, (dev - GEN_CTRL_DEV_ID_TO_EXT_DEV_ID_OFFSET), &ext_gpio_conf)) != RT_ERR_OK)
            return RT_ERR_OK;
#else
            return RT_ERR_DRIVER_NOT_FOUND;
#endif
    }
    return RT_ERR_OK;
}

/* Function Name:
 *      drv_generalCtrlGPIO_devEnable_set
 * Description:
 *      Enable Internal/External GPIO dev
 * Input:
 *	    unit		- unit id
 *      dev         - dev 0 = Internal GPIO, others are External GPIO
 *      enable      - the status of the specified external GPIO device
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_OUT_OF_RANGE
 *      RT_ERR_DRIVER_NOT_FOUND
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
drv_generalCtrlGPIO_devEnable_set(
    uint32 unit,
    drv_generalCtrlGpio_devId_t dev,
     rtk_enable_t enable)
{
    if(IS_INVALID_GEN_CTRL_GPIO_DEV(dev))
        return RT_ERR_OUT_OF_RANGE;

    RT_LOG(LOG_FUNC_ENTER, MOD_GENERAL, "[%s():%d] (gpiodev=%d) enable=%d\n", \
            __FUNCTION__, __LINE__, dev, enable );

    if(IS_INT_GPIO_DEV(dev))
    {
        return RT_ERR_OK;
    }else{
#ifdef CONFIG_SDK_RTL8231
        int32                 ret;
        if((ret = drv_extGpio_devEnable_set(unit, (dev - GEN_CTRL_DEV_ID_TO_EXT_DEV_ID_OFFSET), enable)) != RT_ERR_OK)
            return RT_ERR_OK;
#else
        return RT_ERR_DRIVER_NOT_FOUND;
#endif
    }
    return RT_ERR_OK;
}

/* Function Name:
 *      drv_generalCtrlGPIO_pin_init
 * Description:
 *      Init Internal/External GPIO pin for simple input/output function
 * Input:
 *	    unit		- unit id
 *      dev         - dev 0 = Internal GPIO, others are External GPIO
 *      gpioId      - The internal/external GPIO pin that will be configured
 *      pData       - Internal/External GPIO pin configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_OUT_OF_RANGE
 *      RT_ERR_DRIVER_NOT_FOUND
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
drv_generalCtrlGPIO_pin_init(
    uint32 unit,
    drv_generalCtrlGpio_devId_t dev,
    uint32 pinId,
    drv_generalCtrlGpio_pinConf_t *pData)
{
    drv_gpio_direction_t  pin_direction;
    uint32                pin_default_value;
#ifdef CONFIG_SDK_DRIVER_GPIO
    uint32                int_port, int_pin;
    gpioID                gpioId;
#endif
    int32                 ret;

    if(IS_INVALID_GEN_CTRL_GPIO_DEV(dev))
        return RT_ERR_OUT_OF_RANGE;

    RT_LOG(LOG_FUNC_ENTER, MOD_GENERAL, "[%s():%d] (gpiodev=%d,pin=%d)=%d\n", \
            __FUNCTION__, __LINE__, dev, pinId, pData->default_value );

    pin_direction = pData->direction;
    pin_default_value = pData->default_value;

    if(IS_INT_GPIO_DEV(dev))
    {
#ifdef CONFIG_SDK_DRIVER_GPIO
        RT_PARAM_CHK(!INTERNAL_GPIO_PIN_CHK(pinId), RT_ERR_OUT_OF_RANGE);
        int_port = (pinId / 8);
        int_pin = (pinId % 8);
        gpioId = GPIO_ID(int_port, int_pin);
        if (pin_direction == GPIO_DIR_OUT)
        {
            drv_gpio_dataBit_init(unit, gpioId, pin_default_value);
            drv_gpio_dataBit_set(unit, gpioId, pin_default_value);
        }
        if((ret = drv_gpio_pin_init(unit, gpioId, pData->int_gpio.function, pin_direction, pData->int_gpio.interruptEnable))    != RT_ERR_OK)
            return ret;
#else
        return RT_ERR_DRIVER_NOT_FOUND;
#endif
    }
    else
    {
#ifdef CONFIG_SDK_RTL8231
        drv_extGpio_conf_t pin_conf;
        pin_conf.debounce = pData->ext_gpio.debounce;
        pin_conf.inverter = pData->ext_gpio.inverter;
        pin_conf.direction = pin_direction;
        pin_conf.interruptEnable = pData->ext_gpio.interruptEnable;
        RT_PARAM_CHK(!EXTERNAL_GPIO_PIN_CHK(pinId), RT_ERR_OUT_OF_RANGE);
        drv_extGpio_dataBit_set(unit, (dev - GEN_CTRL_DEV_ID_TO_EXT_DEV_ID_OFFSET), pinId, pin_default_value);
        if((ret = drv_extGpio_pin_init(unit, (dev - GEN_CTRL_DEV_ID_TO_EXT_DEV_ID_OFFSET), pinId, &pin_conf)) != RT_ERR_OK)
        {
            RT_LOG(LOG_FUNC_ENTER, MOD_GENERAL, "\n[%s():%d]RTL8231 pin init failed, ret = %x\n",
                __FUNCTION__, __LINE__,ret);
            return ret;
        }
#else
        return RT_ERR_DRIVER_NOT_FOUND;
#endif
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      drv_generalCtrlGPIO_dataBit_set
 * Description:
 *      Init Internal/External GPIO pin for simple input/output function
 * Input:
 *	    unit		- unit id
 *      dev         - dev 0 = Internal GPIO, others are External GPIO
 *      gpioId      - The internal/external GPIO pin that will be configured
 *      data        - Write out data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_OUT_OF_RANGE
 *      RT_ERR_DRIVER_NOT_FOUND
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
drv_generalCtrlGPIO_dataBit_set(
    uint32 unit,
    drv_generalCtrlGpio_devId_t dev,
    uint32 pinId,
    uint32 data)
{
#ifdef CONFIG_SDK_DRIVER_GPIO
    uint32                int_port, int_pin;
    gpioID                gpioId;
#endif
    int32                 ret;

    if(IS_INVALID_GEN_CTRL_GPIO_DEV(dev))
        return RT_ERR_OUT_OF_RANGE;

    RT_LOG(LOG_FUNC_ENTER, MOD_GENERAL, "[%s():%d] (gpiodev=%d,pin=%d)=%d\n", \
            __FUNCTION__, __LINE__, dev, pinId, data );

    if(IS_INT_GPIO_DEV(dev))
    {
#ifdef CONFIG_SDK_DRIVER_GPIO

        RT_PARAM_CHK(!INTERNAL_GPIO_PIN_CHK(pinId), RT_ERR_OUT_OF_RANGE);
        int_port = (pinId / 8);
        int_pin = (pinId % 8);
        gpioId = GPIO_ID(int_port, int_pin);
        if((ret = drv_gpio_dataBit_set(unit, gpioId, data)) != RT_ERR_OK)
            return ret;
#else
        return RT_ERR_DRIVER_NOT_FOUND;
#endif
    }
    else
    {
#ifdef CONFIG_SDK_RTL8231
        RT_PARAM_CHK(!EXTERNAL_GPIO_PIN_CHK(pinId), RT_ERR_OUT_OF_RANGE);
        if((ret = drv_extGpio_dataBit_set(unit, (dev - GEN_CTRL_DEV_ID_TO_EXT_DEV_ID_OFFSET), pinId, data)) != RT_ERR_OK)
            return ret;
#else
            return RT_ERR_DRIVER_NOT_FOUND;
#endif
    }
    return RT_ERR_OK;
}

/* Function Name:
 *      drv_generalCtrlGPIO_dataBit_get
 * Description:
 *      Init Internal/External GPIO pin for simple input/output function
 * Input:
 *	    unit		- unit id
 *      dev         - dev 0 = Internal GPIO, others are External GPIO
 *      gpioId      - The internal/external GPIO pin that will be configured
 *      *pData      - Read back data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_OUT_OF_RANGE
 *      RT_ERR_DRIVER_NOT_FOUND
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
drv_generalCtrlGPIO_dataBit_get(
    uint32 unit,
    drv_generalCtrlGpio_devId_t dev,
    uint32 pinId,
    uint32 *pData)
{
#ifdef CONFIG_SDK_DRIVER_GPIO
    uint32                int_port, int_pin;
    gpioID                gpioId;
#endif
    int32                 ret;

    if(IS_INVALID_GEN_CTRL_GPIO_DEV(dev))
        return RT_ERR_OUT_OF_RANGE;

    RT_LOG(LOG_FUNC_ENTER, MOD_GENERAL, "[%s():%d] (gpiodev=%d,pin=%d)\n", \
            __FUNCTION__, __LINE__, dev, pinId);


    if(IS_INT_GPIO_DEV(dev))
    {
#ifdef CONFIG_SDK_DRIVER_GPIO
        RT_PARAM_CHK(!INTERNAL_GPIO_PIN_CHK(pinId), RT_ERR_OUT_OF_RANGE);
        int_port = (pinId / 8);
        int_pin = (pinId % 8);
        gpioId = GPIO_ID(int_port, int_pin);
        if((ret = drv_gpio_dataBit_get(unit, gpioId, pData)) != RT_ERR_OK)
            return ret;
#else
        return RT_ERR_DRIVER_NOT_FOUND;
#endif
    }
    else
    {
#ifdef CONFIG_SDK_RTL8231
        RT_PARAM_CHK(!EXTERNAL_GPIO_PIN_CHK(pinId), RT_ERR_OUT_OF_RANGE);
        if((ret = drv_extGpio_dataBit_get(unit, (dev - GEN_CTRL_DEV_ID_TO_EXT_DEV_ID_OFFSET), pinId, pData)) != RT_ERR_OK)
            return ret;
#else
        return RT_ERR_DRIVER_NOT_FOUND;
#endif
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      drv_generalCtrlGPIO_direction_set
 * Description:
 *      Set Internal/External GPIO pin direction
 * Input:
 *	    unit		- unit id
 *      dev         - dev 0 = Internal GPIO, others are External GPIO
 *      gpioId      - The internal/external GPIO pin that will be configured
 *      pData       - Internal/External GPIO pin configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_OUT_OF_RANGE
 *      RT_ERR_DRIVER_NOT_FOUND
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
drv_generalCtrlGPIO_direction_set(
    uint32 unit,
    drv_generalCtrlGpio_devId_t dev,
    uint32 pinId,
    drv_gpio_direction_t direction)
{
#ifdef CONFIG_SDK_DRIVER_GPIO
    uint32                int_port, int_pin;
    gpioID                gpioId;
#endif
    int32                 ret;

    if(IS_INVALID_GEN_CTRL_GPIO_DEV(dev))
        return RT_ERR_OUT_OF_RANGE;

    RT_LOG(LOG_FUNC_ENTER, MOD_GENERAL, "[%s():%d] (gpiodev=%d,pin=%d)\n",
            __FUNCTION__, __LINE__, dev, pinId);

    if(IS_INT_GPIO_DEV(dev))
    {
#ifdef CONFIG_SDK_DRIVER_GPIO
        RT_PARAM_CHK(!INTERNAL_GPIO_PIN_CHK(pinId), RT_ERR_OUT_OF_RANGE);
        int_port = (pinId / 8);
        int_pin = (pinId % 8);
        gpioId = GPIO_ID(int_port, int_pin);
        if((ret = drv_gpio_direction_set(unit, gpioId, direction)) != RT_ERR_OK)
            return ret;
#else
        return RT_ERR_DRIVER_NOT_FOUND;
#endif
    }
    else
    {
#ifdef CONFIG_SDK_RTL8231
        RT_PARAM_CHK(!EXTERNAL_GPIO_PIN_CHK(pinId), RT_ERR_OUT_OF_RANGE);
        if((ret = drv_extGpio_direction_set(unit, (dev - GEN_CTRL_DEV_ID_TO_EXT_DEV_ID_OFFSET),
                pinId, direction)) != RT_ERR_OK)
        {
            RT_LOG(LOG_FUNC_ENTER, MOD_GENERAL, "\n[%s():%d]RTL8231 pin init failed, ret = %x\n",
                __FUNCTION__, __LINE__,ret);
            return ret;
        }
#else
        return RT_ERR_DRIVER_NOT_FOUND;
#endif
    }

    return RT_ERR_OK;
}

#if defined(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE) || (defined(CONFIG_SDK_KERNEL_LINUX_USER_MODE) && !defined(__KERNEL__))
/* Function Name:
 *      drv_generalCtrlGPIO_intrHandler_register
 * Description:
 *      register GPIO interrupt callback function for specified GPIO pin
 * Input:
 *      None
 * Output:
 *	    unit		    - unit id
 *      dev             - dev 0 = Internal GPIO, others are External GPIO
 *      pin             - specified GPIO pin
 *      gpioIsrCallback - callback function
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32 drv_generalCtrlGPIO_intrHandler_register(
    uint32 unit,
    drv_generalCtrlGpio_devId_t dev,
    uint32 pinId,
    drv_generalCtrlGPIO_Isr_cb_f gpioIsrCallback)
{
#ifdef CONFIG_SDK_DRIVER_GPIO
    GPIO_INTERNAL_PIN_t   gpioId;
#endif
    int32                 ret;

    if(IS_INVALID_GEN_CTRL_GPIO_DEV(dev))
        return RT_ERR_OUT_OF_RANGE;

    RT_LOG(LOG_FUNC_ENTER, MOD_GENERAL, "[%s():%d] (gpiodev=%d,pin=%d)\n",
            __FUNCTION__, __LINE__, dev, pinId);

    if(IS_INT_GPIO_DEV(dev))
    {
#ifdef CONFIG_SDK_DRIVER_GPIO
        RT_PARAM_CHK(!INTERNAL_GPIO_PIN_CHK(pinId), RT_ERR_OUT_OF_RANGE);
        gpioId = (GPIO_INTERNAL_PIN_t)pinId;
        if((ret = drv_gpio_intrHandler_register(gpioId, gpioIsrCallback)) != RT_ERR_OK)
            return ret;
#else
        return RT_ERR_DRIVER_NOT_FOUND;
#endif
    }
    else
    {
#ifdef CONFIG_SDK_RTL8231
        if((ret = drv_extGpio_intrHandler_register(unit, (dev - GEN_CTRL_DEV_ID_TO_EXT_DEV_ID_OFFSET), pinId, gpioIsrCallback)) != RT_ERR_OK)
            return ret;
#else
    return RT_ERR_DRIVER_NOT_FOUND;
#endif
    }
    return RT_ERR_OK;

}

/* Function Name:
 *      drv_generalCtrlGPIO_intrHandler_unregister
 * Description:
 *      unregister GPIO interrupt callback function for specified GPIO pin
 * Input:
 *      None
 * Output:
 *	    unit		    - unit id
 *      dev             - dev 0 = Internal GPIO, others are External GPIO
 *      pin             - specified GPIO pin
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32 drv_generalCtrlGPIO_intrHandler_unregister(
    uint32 unit,
    drv_generalCtrlGpio_devId_t dev,
    uint32 pinId)
{
#ifdef CONFIG_SDK_DRIVER_GPIO
    GPIO_INTERNAL_PIN_t   gpioId;
#endif
    int32                 ret;

    if(IS_INVALID_GEN_CTRL_GPIO_DEV(dev))
        return RT_ERR_OUT_OF_RANGE;

    RT_LOG(LOG_FUNC_ENTER, MOD_GENERAL, "[%s():%d] (gpiodev=%d,pin=%d)\n",
            __FUNCTION__, __LINE__, dev, pinId);

    if(IS_INT_GPIO_DEV(dev))
    {
#ifdef CONFIG_SDK_DRIVER_GPIO
        RT_PARAM_CHK(!INTERNAL_GPIO_PIN_CHK(pinId), RT_ERR_OUT_OF_RANGE);
        gpioId = (GPIO_INTERNAL_PIN_t)pinId;
        if((ret = drv_gpio_intrHandler_unregister(gpioId)) != RT_ERR_OK)
            return ret;
#else
        return RT_ERR_DRIVER_NOT_FOUND;
#endif
    }
    else
    {
#ifdef CONFIG_SDK_RTL8231
        if((ret = drv_extGpio_intrHandler_unregister(unit, (dev - GEN_CTRL_DEV_ID_TO_EXT_DEV_ID_OFFSET), pinId)) != RT_ERR_OK)
            return ret;
#else
        return RT_ERR_DRIVER_NOT_FOUND;
#endif
    }
    return RT_ERR_OK;
}
#endif /* defined(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE) || (defined(CONFIG_SDK_KERNEL_LINUX_USER_MODE) && !defined(__KERNEL__)) */

