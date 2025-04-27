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

#include <common/rt_type.h>
#include <common/debug/rt_log.h>
#include <private/drv/swcore/swcore.h>
#include <private/drv/swcore/chip_probe.h>
#include <drv/gpio/gpio.h>
#include <private/drv/gpio/gpio_mapper.h>
#include <hwp/hw_profile.h>
#include <common/util/rt_util_intr.h>

#ifndef __BOOTLOADER__
  #include <dev_config.h>
  #include <osal/wait.h>
  #include <osal/thread.h>
#endif //__BOOTLOADER__
#include <soc/type.h>

#if (defined(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE) || (defined(CONFIG_SDK_KERNEL_LINUX_USER_MODE) && defined(__KERNEL__)))
  #include <osal/atomic.h>
#endif
#include <osal/sem.h>



/*
 * Symbol Definition
 */
#define GPIO_SEM_LOCK(unit)    \
do {\
    if (osal_sem_mutex_take(gpio_sem[unit], OSAL_SEM_WAIT_FOREVER) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_LOCK_FAILED, (MOD_BSP), "semaphore lock failed");\
        return RT_ERR_SEM_LOCK_FAILED;\
    }\
} while(0)

#define GPIO_SEM_UNLOCK(unit)   \
do {\
    if (osal_sem_mutex_give(gpio_sem[unit]) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_UNLOCK_FAILED, (MOD_BSP), "semaphore unlock failed");\
        return RT_ERR_SEM_UNLOCK_FAILED;\
    }\
} while(0)


/*
 * Data Declaration
 */
static osal_mutex_t         gpio_sem[RTK_MAX_NUM_OF_UNIT];


typedef struct gpio_database_s
{
    uint32  register_pin;
    uint32  init_pin;
    uint32  action_status;
    drv_gpioIsr_cb_f callback;

} gpio_database_t;


static gpio_database_t data[GPIO_INTERNAL_PIN_END];
rtk_enable_t threadEnabled = DISABLED;

#if defined(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE) || (defined(CONFIG_SDK_KERNEL_LINUX_USER_MODE) && !defined(__KERNEL__)) /* kernel mode in kerne; user mode in user */
static drv_gpioIsr_cb_f gpioInterruptCb[GPIO_INTERNAL_PIN_END];
static osal_event_t drv_gpioIsr_event;
#endif

#if (defined(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE) || (defined(CONFIG_SDK_KERNEL_LINUX_USER_MODE) && defined(__KERNEL__)))
  /* GPIO ISR status shadow database in kernel space */
  uint8 gpio_pin_isr_sts_shadow[RTK_MAX_NUM_OF_UNIT][GPIO_INTERNAL_PIN_END];
#endif

/*
 * Macro Definition
 */

/*
 * Function Declaration
 */

/* Function Name:
 *      drv_gpio_init
 * Description:
 *      GPIO driver initilization
 * Input:
 *      unit  - UNIT ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_CHIP_NOT_SUPPORTED
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          The purpose of the function is changed. The original pin initialization function
 *          is implemented in the new API: drv_gpio_pin_init.
 */
int32
drv_gpio_init(uint32 unit)
{
    int32       ret;

    RT_INIT_MSG("  GPIO Init");

#if defined(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE) || (defined(CONFIG_SDK_KERNEL_LINUX_USER_MODE) && !defined(__KERNEL__))
        ret = osal_wait_module_create(&drv_gpioIsr_event);
        if(ret != RT_ERR_OK)
        {
             RT_INIT_MSG(": (WAIT INIT Fail)\n");
            return RT_ERR_FAILED;
        }
#endif


    if (!GPIO_CHK(unit))
    {
        RT_INIT_MSG(": (Not support)\n");
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    /* create semaphore */
    gpio_sem[unit] = osal_sem_mutex_create();
    if (0 == gpio_sem[unit])
    {
        RT_INIT_MSG(": (GPIO semaphore create failed)\n");
        return RT_ERR_FAILED;
    }


    if (GPIO_CTRL(unit).init != NULL)
    {
        if ((ret = GPIO_CTRL(unit).init(unit)) != RT_ERR_OK)
        {
            RT_INIT_MSG(": (Fail)\n");
            return ret;
        }
    }

    RT_INIT_MSG("\n");

    return RT_ERR_OK;
}

/* Function Name:
 *      drv_gpio_direction_set
 * Description:
 *      Set GPIO pin as input or output pin
 * Input:
 *      unit            - UNIT ID
 *      gpioId          - The GPIO port that will be configured
 *      direction       - Data direction, in or out
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_OUT_OF_RANGE
 *      RT_ERR_CHIP_NOT_SUPPORTED
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
drv_gpio_direction_set(
    uint32 unit,
    gpioID gpioId,
    drv_gpio_direction_t direction)
{
    uint32 port = GPIO_PORT(gpioId);
    uint32 pin = GPIO_PIN(gpioId);
    int32  ret;

    /* parameter check */
    RT_PARAM_CHK(!GPIO_PORT_CHK(port), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(!GPIO_PIN_CHK(pin), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(!GPIO_DIR_CHK(direction), RT_ERR_FAILED);


    if (!GPIO_CHK(unit) || (GPIO_CTRL(unit).direction_set == NULL))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    GPIO_SEM_LOCK(unit);
    if ((ret = GPIO_CTRL(unit).direction_set(unit, port, pin, direction)) != RT_ERR_OK)
    {
        GPIO_SEM_UNLOCK(unit);
        return ret;
    }
    GPIO_SEM_UNLOCK(unit);
    return RT_ERR_OK;
} /* end of drv_gpio_direction_set */

/* Function Name:
 *      drv_gpio_pin_init
 * Description:
 *      Init GPIO port
 * Input:
 *      unit            - UNIT ID
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
 *      RT_ERR_CHIP_NOT_SUPPORTED
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
int32
drv_gpio_pin_init(
    uint32 unit,
    gpioID gpioId,
    drv_gpio_control_t function,
    drv_gpio_direction_t direction,
    drv_gpio_interruptType_t interruptEnable)
{
    uint32 port = GPIO_PORT(gpioId);
    uint32 pin = GPIO_PIN(gpioId);
    int32  ret;

    /* parameter check */
    RT_PARAM_CHK(!GPIO_PORT_CHK(port), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(!GPIO_PIN_CHK(pin), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(!GPIO_CTRLFUNC_CHK(function), RT_ERR_FAILED);
    RT_PARAM_CHK(!GPIO_DIR_CHK(direction), RT_ERR_FAILED);
    RT_PARAM_CHK(!GPIO_INT_CHK(interruptEnable), RT_ERR_FAILED);

    if (!GPIO_CHK(unit))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    GPIO_SEM_LOCK(unit);
    if (GPIO_CTRL(unit).control_set != NULL)
    {
        if ((ret = GPIO_CTRL(unit).control_set(unit, port, pin, function)) != RT_ERR_OK)
        {
            GPIO_SEM_UNLOCK(unit);
            return ret;
        }
    }

    if (GPIO_CTRL(unit).direction_set != NULL)
    {
        if ((ret = GPIO_CTRL(unit).direction_set(unit, port, pin, direction)) != RT_ERR_OK)
        {
            GPIO_SEM_UNLOCK(unit);
            return ret;
        }
    }

    if (GPIO_CTRL(unit).intrMode_set != NULL)
    {
        if ((ret = GPIO_CTRL(unit).intrMode_set(unit, port, pin, interruptEnable)) != RT_ERR_OK)
        {
            GPIO_SEM_UNLOCK(unit);
            return ret;
        }
    }

    data[PORT_AND_PIN_TO_PINID(port,pin)].init_pin = GPIO_FLAG_ON;

    GPIO_SEM_UNLOCK(unit);
    return RT_ERR_OK;
} /* end of drv_gpio_pin_init */

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
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_OUT_OF_RANGE
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Only the GPO pin need to call the API to init default value.
 * Changes:
 *      None
 */
int32 drv_gpio_dataBit_init(uint32 unit, gpioID gpioId, uint32 data)
{
    uint32 port = GPIO_PORT(gpioId);
    uint32 pin = GPIO_PIN(gpioId);
    int32  ret;

    /* parameter check */
    RT_PARAM_CHK(!GPIO_PORT_CHK(port), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(!GPIO_PIN_CHK(pin), RT_ERR_OUT_OF_RANGE);

    RT_LOG(LOG_FUNC_ENTER, MOD_GENERAL, "[%s():%d] (port=%d,pin=%d)=%d\n", \
            __FUNCTION__, __LINE__, port, pin, data );

    if (!GPIO_CHK(unit) || (GPIO_CTRL(unit).dataBit_init == NULL))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    GPIO_SEM_LOCK(unit);
    if ((ret = GPIO_CTRL(unit).dataBit_init(unit, port, pin, data)) != RT_ERR_OK)
    {
        GPIO_SEM_UNLOCK(unit);
        return ret;
    }

    GPIO_SEM_UNLOCK(unit);
    return RT_ERR_OK;
} /* end of drv_gpio_dataBit_init */

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
 *      RT_ERR_CHIP_NOT_SUPPORTED
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
drv_gpio_dataBit_get(uint32 unit, gpioID gpioId, uint32 *pData)
{
    uint32 port = GPIO_PORT(gpioId);
    uint32 pin = GPIO_PIN(gpioId);
    int32  ret;

    /* parameter check */
    RT_PARAM_CHK(!GPIO_PORT_CHK(port), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(!GPIO_PIN_CHK(pin), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    if (!GPIO_CHK(unit) || (GPIO_CTRL(unit).dataBit_get == NULL))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    GPIO_SEM_LOCK(unit);
    if ((ret = GPIO_CTRL(unit).dataBit_get(unit, port, pin, pData)) != RT_ERR_OK)
    {
        GPIO_SEM_UNLOCK(unit);
        return ret;
    }

    GPIO_SEM_UNLOCK(unit);

    RT_LOG(LOG_FUNC_ENTER, MOD_GENERAL, "[%s():%d] (port=%d,pin=%d)=%d\n", \
            __FUNCTION__, __LINE__, port, pin, *pData );

    return RT_ERR_OK;
} /* end of drv_gpio_dataBit_get */

/* Function Name:
 *      drv_gpio_dataBit_set
 * Description:
 *      Set GPIO data
 * Input:
 *      unit   - UNIT ID
 *      gpioId - GPIO ID
 *      data   - Data to write
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_OUT_OF_RANGE
 *      RT_ERR_CHIP_NOT_SUPPORTED
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
drv_gpio_dataBit_set(uint32 unit, gpioID gpioId, uint32 data)
{
    uint32 port = GPIO_PORT(gpioId);
    uint32 pin = GPIO_PIN(gpioId);
    int32  ret;

    /* parameter check */
    RT_PARAM_CHK(!GPIO_PORT_CHK(port), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(!GPIO_PIN_CHK(pin), RT_ERR_OUT_OF_RANGE);

    RT_LOG(LOG_FUNC_ENTER, MOD_GENERAL, "[%s():%d] (port=%d,pin=%d)=%d\n", \
            __FUNCTION__, __LINE__, port, pin, data );


    if (!GPIO_CHK(unit) || (GPIO_CTRL(unit).dataBit_set == NULL))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    GPIO_SEM_LOCK(unit);
    if ((ret = GPIO_CTRL(unit).dataBit_set(unit, port, pin, data)) != RT_ERR_OK)
    {
        GPIO_SEM_UNLOCK(unit);
        return ret;
    }
    GPIO_SEM_UNLOCK(unit);
    return RT_ERR_OK;
} /* end of drv_gpio_dataBit_set */


/* Function Name:
 *      drv_gpio_isr_get
 * Description:
 *      Get the interrupt status register value of a specified GPIO ID
 * Input:
 *      gpioId  - GPIO ID
 * Output:
 *      pIsr    - Pointer to store return value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_OUT_OF_RANGE
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_CHIP_NOT_SUPPORTED
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
drv_gpio_isr_get(gpioID gpioId, uint32 *pIsr)
{
    uint32 port = GPIO_PORT(gpioId);
    uint32 pin = GPIO_PIN(gpioId);
    int32  ret;
    uint32 unit = HWP_MY_UNIT_ID();

    /* parameter check */
    RT_PARAM_CHK(!GPIO_PORT_CHK(port), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(!GPIO_PIN_CHK(pin), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pIsr), RT_ERR_NULL_POINTER);

    if (!GPIO_CHK(unit) || (GPIO_CTRL(unit).intrStatus_get == NULL))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if ((ret = GPIO_CTRL(unit).intrStatus_get(unit, port, pin, pIsr)) != RT_ERR_OK)
    {
        return ret;
    }
    return RT_ERR_OK;
} /* end of drv_gpio_isr_get */

/* Function Name:
 *      drv_gpio_isr_clear
 * Description:
 *      Clear the interrupt status register value of a specified GPIO ID.
 * Input:
 *      gpioId - GPIO ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_OUT_OF_RANGE
 *      RT_ERR_CHIP_NOT_SUPPORTED
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
drv_gpio_isr_clear(gpioID gpioId)
{
    uint32 port = GPIO_PORT(gpioId);
    uint32 pin = GPIO_PIN(gpioId);
    int32  ret;
    uint32 unit = HWP_MY_UNIT_ID();

    /* parameter check */
    RT_PARAM_CHK(!GPIO_PORT_CHK(port), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(!GPIO_PIN_CHK(pin), RT_ERR_OUT_OF_RANGE);

    if (!GPIO_CHK(unit) || (GPIO_CTRL(unit).intrStatus_clear == NULL))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    if ((ret = GPIO_CTRL(unit).intrStatus_clear(unit, port, pin)) != RT_ERR_OK)
    {
        return ret;
    }
    return RT_ERR_OK;
} /* end of drv_gpio_isr_clear */

#if defined(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE) || (defined(CONFIG_SDK_KERNEL_LINUX_USER_MODE) && !defined(__KERNEL__))

/* Function Name:
 *      _gpio_interrupt_handler
 * Description:
 *      Pending interrupt thread of gpio interrupt for waiting interrupt event.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
static int _gpio_interrupt_handler(void)
{
    int32 port,pin = 0;
    int32 portMin = 0, portMax = -1;
    int32 ret = RT_ERR_OK;
    gpio_pin_data_t  gpio_data;
    uint32 unit = HWP_MY_UNIT_ID();

    if (!GPIO_CHK(unit) || (GPIO_CTRL(unit).portRange_get == NULL))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    GPIO_CTRL(unit).portRange_get(unit, &portMin, &portMax);


    for(port = portMin; port <= portMax; port++)
    {
        for(pin = GPIO_PIN_MIN; pin <= GPIO_PIN_MAX ; pin++)
        {
            if(data[PORT_AND_PIN_TO_PINID(port,pin)].action_status == ENABLED){
                gpio_data.pin_id = PORT_AND_PIN_TO_PINID(port, pin);
                ret = drv_gpio_dataBit_get(unit, GPIO_ID(port,pin), (uint32 *)&(gpio_data.gpio_bit));
                if (RT_ERR_OK != ret)
                {
                    break;
                }
                gpio_data.dev_id = 0;
                gpioInterruptCb[PORT_AND_PIN_TO_PINID(port, pin)](&gpio_data);
                data[PORT_AND_PIN_TO_PINID(port, pin)].action_status = DISABLED;
            }
        }
    }
    return RT_ERR_OK;
}

/* Function Name:
 *      _gpio_intr_thread
 * Description:
 *      GPIO interrupt thread to receive interrupt event.
 * Input:
 *      pArg - Parameter that is provided when thread create.
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
static void *_gpio_intr_thread(void *pArg)
{

    while(1)
    {
        osal_wait_event(drv_gpioIsr_event);

        _gpio_interrupt_handler();
    }
    return NULL;
}

/* Function Name:
 *      gpio_intr_attach
 * Description:
 *      Connect interrupt with handle thread
 * Input:
 *      None
 * Output:
 *      None
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
int32 gpio_intr_attach(void)
{
    int32 ret;

    /* Check arguments */
    ret = osal_thread_create(GPIO_INTR_THREAD_NAME, 4096, 0, (void *)_gpio_intr_thread, NULL);
    if (0 == ret)
    {
      RT_ERR(ret, MOD_RTCORE, "GPIO interrupt thread create failed");
      return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}

/* Function Name:
*      drv_gpio_isr
* Description:
*      GPIO ABCD and EFG interrupt handler routine
* Input:
*      pParam - isr parameter
* Output:
*      None
* Return:
*      SYS_ERR_OK
*      SYS_ERR_FAILED
*      RT_ERR_CHIP_NOT_SUPPORTED
* Note:
*      None
*/
osal_isrret_t drv_gpio_isr(void *pParam)
{
    int32 port,pin = 0;
    int32 portMin = 0, portMax = -1;
    uint32 gIsr = 0;
    int32 ret = RT_ERR_OK;
    uint32      unit = HWP_MY_UNIT_ID();


    if (!GPIO_CHK(unit) || (GPIO_CTRL(unit).portRange_get == NULL))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    GPIO_CTRL(unit).portRange_get(unit, &portMin, &portMax);


    for(port = portMin; port <= portMax; port++)
    {
        for(pin = GPIO_PIN_MIN; pin <= GPIO_PIN_MAX ; pin++ )
        {
            ret = drv_gpio_isrStsShadow_get(unit, PORT_AND_PIN_TO_PINID(port,pin), &gIsr);
            if (RT_ERR_OK != ret)
            {
                return ret;
            }

            if (gIsr != 0)
            {
                if(data[PORT_AND_PIN_TO_PINID(port,pin)].init_pin == GPIO_FLAG_ON && data[PORT_AND_PIN_TO_PINID(port,pin)].register_pin == GPIO_FLAG_ON)
                {
                    data[PORT_AND_PIN_TO_PINID(port,pin)].action_status = ENABLED;
                }
            }
        }/* end for */
    }/* end for */

    osal_wake_up(drv_gpioIsr_event);
    return RT_ERR_OK;

}


/* Function Name:
 *      drv_gpio_intrHandler_register
 * Description:
 *      register GPIO interrupt callback function for specified GPIO pin
 * Input:
 *      None
 * Output:
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
int32 drv_gpio_intrHandler_register(GPIO_INTERNAL_PIN_t pin, drv_gpioIsr_cb_f gpioIsrCallback)
{

    int i = 0;
    int32 ret = RT_ERR_FAILED;
    rtk_enable_t regIntrSrc = DISABLED;


    RT_PARAM_CHK((NULL == gpioIsrCallback), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(IS_GPIO_PIN_INVALID(pin), RT_ERR_INPUT);

    if(threadEnabled == DISABLED){
        gpio_intr_attach();
        threadEnabled = ENABLED;
    }

    if(pin < GPIO_INTERNAL_PIN_END && pin >= A0){

        for(i = A0 ; i < GPIO_INTERNAL_PIN_END ; i++){
            if(data[i].register_pin == GPIO_FLAG_ON)
                regIntrSrc = ENABLED;
        }

        if(regIntrSrc == DISABLED){
            /* Register GPIO IRQ */
            RT_ERR_HDL(rt_util_intr_isr_register(INTR_ISR_GPIO, drv_gpio_isr, NULL), error, ret);

        }
        gpioInterruptCb[pin] = gpioIsrCallback;
        data[pin].register_pin = GPIO_FLAG_ON;
    }

    return RT_ERR_OK;

    error:
        RT_ERR(RT_ERR_FAILED, MOD_RTCORE, "Register GPIO IRQ and Interrupt Handler Failed!");

    return RT_ERR_FAILED;
}


/* Function Name:
 *      drv_gpio_intrHandler_unregister
 * Description:
 *      unregister GPIO interrupt callback function for specified GPIO pin
 * Input:
 *      None
 * Output:
 *      pin - specified GPIO pin
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
int32 drv_gpio_intrHandler_unregister(GPIO_INTERNAL_PIN_t pin)
{
    int i = 0;
    int32 ret = RT_ERR_FAILED;
    rtk_enable_t unRegIntrSrc = DISABLED;

    RT_PARAM_CHK(IS_GPIO_PIN_INVALID(pin), RT_ERR_INPUT);

    data[pin].register_pin = GPIO_FLAG_OFF;

    if(pin < GPIO_INTERNAL_PIN_END && pin >= A0){

        for(i = A0 ; i < GPIO_INTERNAL_PIN_END ; i++){
            if(data[i].register_pin == GPIO_FLAG_ON)
                unRegIntrSrc = ENABLED;
        }

        if(unRegIntrSrc == DISABLED)
            RT_ERR_HDL(rt_util_intr_isr_unregister(INTR_ISR_GPIO), error, ret);


    }

    return RT_ERR_OK;


    error:
        RT_ERR(RT_ERR_FAILED, MOD_RTCORE, "Unregister GPIO IRQ and Interrupt Handler Failed!");

    return RT_ERR_FAILED;

}
#endif //#if defined(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE) || (defined(CONFIG_SDK_KERNEL_LINUX_USER_MODE) && !defined(__KERNEL__))


#if (defined(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE) || (defined(CONFIG_SDK_KERNEL_LINUX_USER_MODE) && defined(__KERNEL__)))
/* Function Name:
 *      drv_gpio_isrStsShadow_backup
 * Description:
 *      back up GPIO status in shadow database
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_CHIP_NOT_SUPPORTED
 * Note:
 *      None
 */
int32
drv_gpio_isrStsShadow_backup(void)
{
    uint32  port, pin, gIsr;
    int32   ret, portMin = 0, portMax = -1;
    uint32  unit = HWP_MY_UNIT_ID();


    if (!GPIO_CHK(unit) || (GPIO_CTRL(unit).portRange_get == NULL))
    {
        return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    GPIO_CTRL(unit).portRange_get(unit, &portMin, &portMax);

    for(port = portMin; port <= portMax; port++)
    {
        for(pin = GPIO_PIN_MIN; pin <= GPIO_PIN_MAX ; pin++ )
        {
            ret = drv_gpio_isr_get(GPIO_ID(port,pin),(uint32 *)&gIsr);
            if (RT_ERR_OK != ret)
            {
                break;
            }

            if (gIsr != 0)
            {
                gpio_pin_isr_sts_shadow[unit][PORT_AND_PIN_TO_PINID(port,pin)] = 1;
                drv_gpio_isr_clear(GPIO_ID(port,pin));
            }
        }/* end for */
    }/* end for */
    return RT_ERR_OK;
}

/* Function Name:
 *      drv_gpio_isrStsShadow_get
 * Description:
 *      Get shadowed GPIO ISR status and clear the status to 0
 * Input:
 *      unit - unit ID
 *      pin - specified GPIO pin
 * Output:
 *      *pData - GPIO ISR status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
drv_gpio_isrStsShadow_get(uint32 unit, GPIO_INTERNAL_PIN_t pin, uint32 *pData)
{

    if (unit >= RTK_MAX_NUM_OF_UNIT)
    {
        return RT_ERR_INPUT;
    }

    if (pin >= GPIO_INTERNAL_PIN_END)
    {
        return RT_ERR_INPUT;
    }

    osal_isr_disable_interrupt();
    *pData = gpio_pin_isr_sts_shadow[unit][pin]; /* get status */
    gpio_pin_isr_sts_shadow[unit][pin] = 0; /* clear */
    osal_isr_enable_interrupt();
    return RT_ERR_OK;
}
#endif /* #if (defined(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE) || (defined(CONFIG_SDK_KERNEL_LINUX_USER_MODE) && defined(__KERNEL__))) */




