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
#ifndef __DRV_CTRL_GPIO_H__
#define __DRV_CTRL_GPIO_H__

/*
 * Include Files
 */
#include <drv/gpio/gpio.h>
#include <drv/gpio/ext_gpio.h>



/*
 * Symbol Definition
 */
#define INT_GPIO_DEV_ID         0
#define GEN_CTRL_DEV_ID_TO_EXT_DEV_ID_OFFSET      1


/*
 * Data Declaration
 */

typedef struct drv_intGpio_Conf_s
{
    drv_gpio_control_t function;
    drv_gpio_interruptType_t interruptEnable;
} drv_intGpio_Conf_t;


typedef struct drv_generalCtrlGpio_devConf_s
{

    drv_gpio_direction_t    direction;
    uint32                  default_value;
    drv_extGpio_devConf_t   ext_gpio;
} drv_generalCtrlGpio_devConf_t;

typedef struct drv_generalCtrlGpio_pinConf_s
{
    drv_gpio_direction_t    direction;
    uint32                  default_value;
    drv_intGpio_Conf_t      int_gpio;
    drv_extGpio_conf_t      ext_gpio;
} drv_generalCtrlGpio_pinConf_t;

/* define GPIO ISR call back data structure*/
typedef struct drv_generalCtrlGpio_isr_data_s
{
    int32 pin_id;
    int32 gpio_bit;
    int32 dev_id;   /* generalCtrl_gpio device ID */

}drv_generalCtrlGpio_isr_data_t;

typedef enum drv_generalCtrlGpio_devId_e
{
    GEN_GPIO_DEV_ID0_INTERNAL = 0,
    GEN_GPIO_DEV_ID1,
    GEN_GPIO_DEV_ID2,
    GEN_GPIO_DEV_ID3,
    GEN_GPIO_DEV_ID4,
    GEN_GPIO_DEV_ID5,
    GEN_GPIO_DEV_ID6,
    GEN_GPIO_DEV_ID7,
    GEN_GPIO_DEV_ID8,
    GEN_GPIO_DEV_ID_END
} drv_generalCtrlGpio_devId_t;

typedef struct drv_generalCtrlGpio_gpioId_s
{

    drv_generalCtrlGpio_devId_t     devId;
    uint32                          pinId;
} drv_generalCtrlGpio_gpioId_t;


#define IS_INVALID_GEN_CTRL_GPIO_DEV(dev)    \
    (((dev >= GEN_GPIO_DEV_ID_END)) ? 1 : 0)

#define IS_INT_GPIO_DEV(dev)    \
    (((dev == GEN_GPIO_DEV_ID0_INTERNAL)) ? 1 : 0)

#define INTERNAL_GPIO_PIN_CHK(pin)           ((pin < GPIO_INTERNAL_PIN_END))

#define EXTERNAL_GPIO_PIN_CHK(pin)           ((pin < EXT_GPIO_ID_END))

typedef osal_isrret_t (*drv_generalCtrlGPIO_Isr_cb_f)(void *pParam);
typedef osal_isrret_t (*drv_generalCtrlGPIO_Dispatch_cb_f)(void *pParam);


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
    drv_generalCtrlGpio_devConf_t *pData);

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
    rtk_enable_t enable);

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
    drv_generalCtrlGpio_pinConf_t *pData);

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
    uint32 data);

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
    uint32 *pData);

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
    drv_gpio_direction_t direction);

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
    drv_generalCtrlGPIO_Isr_cb_f gpioIsrCallback);

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
    uint32 pinId);

#endif/*__DRV_CTRL_GPIO_H__*/
