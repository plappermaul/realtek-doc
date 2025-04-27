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
#ifndef __DRV_GPIO_H__
#define __DRV_GPIO_H__

/*
 * Include Files
 */
#include <common/error.h>
#include <osal/isr.h>



/*
 * Symbol Definition
 */

#define GPIO_INTR_THREAD_NAME  "GPIO_INTR_THREAD"


/* define GPIO port */
typedef enum drv_gpio_port_e
{
    GPIO_PORT_A = 0,
    GPIO_PORT_B,
    GPIO_PORT_C,
    GPIO_PORT_D,
    GPIO_PORT_E,
    GPIO_PORT_F,
    GPIO_PORT_G,
    GPIO_PORT_END
} drv_gpio_port_t;

/* define GPIO pin function */
typedef enum drv_gpio_func_e
{
    GPIO_FUNC_CONTROL = 0,
    GPIO_FUNC_DIRECTION,
    GPIO_FUNC_DATA,
    GPIO_FUNC_INTERRUPT_STATUS,
    GPIO_FUNC_INTERRUPT_ENABLE,
    GPIO_FUNC_END
} drv_gpio_func_t;

/* define GPIO control function */
typedef enum drv_gpio_control_e
{
    GPIO_CTRLFUNC_NORMAL = 0,
    GPIO_CTRLFUNC_DEDICATE_PERIPHERAL,
    GPIO_CTRLFUNC_END
} drv_gpio_control_t;

/* define GPIO direction */
typedef enum drv_gpio_direction_e
{
    GPIO_DIR_IN = 0,
    GPIO_DIR_OUT,
    GPIO_DIR_END
} drv_gpio_direction_t;

/* define GPIO Interrupt Type */
typedef enum drv_gpio_interruptType_e
{
    GPIO_INT_DISABLE = 0,
    GPIO_INT_FALLING_EDGE,
    GPIO_INT_RISING_EDGE,
    GPIO_INT_BOTH_EDGE,
    GPIO_INT_TYPE_END
} drv_gpio_interruptType_t;

/* define GPIO pin id */
typedef enum GPIO_INTERNAL_PIN
{
    A0 = 0,
    A1,
    A2,
    A3,
    A4,
    A5,
    A6,
    A7,
    B0,
    B1,
    B2,
    B3,
    B4,
    B5,
    B6,
    B7,
    C0,
    C1,
    C2,
    C3,
    C4,
    C5,
    C6,
    C7,
    D0,
    D1,
    D2,
    D3,
    D4,
    D5,
    D6,
    D7,
    GPIO_INTERNAL_PIN_END
}GPIO_INTERNAL_PIN_t;

/* define GPIO pin data */
typedef struct gpio_pin_data_s
{
    int32 pin_id;
    int32 gpio_bit;
    int32 dev_id;

}gpio_pin_data_t;

typedef struct gpio_intr_hdl_entry_s {
    void (*fHandler)(void*);
    void *pHandler_param;
} gpio_intr_hdl_entry_t;

typedef uint32 gpioID;
typedef osal_isrret_t (*drv_gpioIsr_cb_f)(void *pParam);
typedef osal_isrret_t (*drv_gpioDispatch_cb_f)(void *pParam);

/*
 * Data Declaration
 */

/*
 * Macro Definition
 */

/*
 * Every pin of GPIO port can be mapped to a unique ID.
 * All the access to a GPIO pin must use the ID.
 * This macro is used to map the port and pin into the ID.
 */
#define GPIO_ID_PORT_OFFSET         16
#define GPIO_ID_PIN_MASK            ((1 << GPIO_ID_PORT_OFFSET) - 1)
#define GPIO_ID(port,pin)           (((uint32)port << GPIO_ID_PORT_OFFSET) | ((uint32)pin & GPIO_ID_PIN_MASK))
#define GPIO_PORT(id)               (id >> GPIO_ID_PORT_OFFSET)

#define GPIO_PIN(id)                (id & GPIO_ID_PIN_MASK)
#define GPIO_PIN_MIN                0
#define GPIO_PIN_MAX                7

#define GPIO_PORT_CHK(port)         ((port >= GPIO_PORT_A) && (port < GPIO_PORT_END))
#define GPIO_PIN_CHK(pin)           (pin <= GPIO_PIN_MAX)
#define GPIO_FUNC_CHK(func)         ((func >= GPIO_FUNC_CONTROL) && (func < GPIO_FUNC_END))
#define GPIO_CTRLFUNC_CHK(func)     ((func >= GPIO_CTRLFUNC_NORMAL) && (func < GPIO_CTRLFUNC_END))
#define GPIO_DIR_CHK(dir)           ((dir >= GPIO_DIR_IN) && (dir < GPIO_DIR_END))
#define GPIO_INT_CHK(type)          ((type >= GPIO_INT_DISABLE) && (type < GPIO_INT_TYPE_END))

#define PORT_AND_PIN_TO_PINID(port,pin)    (port*(GPIO_PIN_MAX+1)) + pin

#define PINID_TO_GPIO_PIN(pin_id)   (pin_id % (GPIO_PIN_MAX+1))
#define PINID_TO_GPIO_PORT(pin_id)  (pin_id / (GPIO_PIN_MAX+1))

//#define GPIO_PORT(id)               (id >> GPIO_ID_PORT_OFFSET)


#define GPIO_FLAG_ON                1
#define GPIO_FLAG_OFF               0


#define IS_GPIO_PIN_INVALID(pin)    \
    ((((pin) < GPIO_INTERNAL_PIN_END) || ((pin) >= A0)) ? 0 : 1)


#define EXT_GPIO_ID_DEV_OFFSET      16
#define EXT_GPIO_ID_MARK_OFFSET     31

#define EXT_GPIO_ID_PIN_MASK       ((1 << EXT_GPIO_ID_DEV_OFFSET) - 1)

#define EXT_GPIO_ID(dev,pin)       (((uint32)0x1 << EXT_GPIO_ID_MARK_OFFSET) | ((uint32)dev << GPIO_ID_PORT_OFFSET) | ((uint32)pin & GPIO_ID_PIN_MASK))
#define EXT_GPIO_DEV(id)           (((uint32)(id >> EXT_GPIO_ID_DEV_OFFSET)) & EXT_GPIO_ID_PIN_MASK)

#define EXT_GPIO_PIN(id)           (id & EXT_GPIO_ID_PIN_MASK)

#define IS_EXT_GPIO_PIN(id)    \
    ((((uint32)0x1) & ((uint32)id >> EXT_GPIO_ID_MARK_OFFSET)) ? 1 : 0)

/* input number of bits and transfer it to mask, for example, the mask of 3 bits is 0x7 */
#define GPIO_BITS_TO_MASK(_bits)                      ((0x1 << _bits) - 1)


/*
 * Function Declaration
 */


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
extern int32
drv_gpio_direction_set(
    uint32 unit,
    gpioID gpioId,
    drv_gpio_direction_t direction);

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
extern int32
drv_gpio_pin_init(
    uint32 unit,
    gpioID gpioId,
    drv_gpio_control_t function,
    drv_gpio_direction_t direction,
    drv_gpio_interruptType_t interruptEnable);

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
extern int32
drv_gpio_dataBit_init(uint32 unit, gpioID gpioId, uint32 data);

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
extern int32
drv_gpio_dataBit_get(uint32 unit, gpioID gpioId, uint32 *pData);

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
extern int32
drv_gpio_dataBit_set(uint32 unit, gpioID gpioId, uint32 data);

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
extern int32
drv_gpio_isr_get(gpioID gpioId, uint32 *pIsr);

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
extern int32
drv_gpio_isr_clear(gpioID gpioId);

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
extern int32
drv_gpio_intrHandler_register(GPIO_INTERNAL_PIN_t pin, drv_gpioIsr_cb_f gpioIsrCallback);

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
extern int32
drv_gpio_intrHandler_unregister(GPIO_INTERNAL_PIN_t pin);

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
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern osal_isrret_t
drv_gpio_isr(void *pParam);

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
extern int32
gpio_intr_attach(void);


/* Function Name:
 *      gpio_probe
 * Description:
 *      Probe GPIO module of the specified device.
 * Input:
 *      uint    - unit ID
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
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
gpio_probe(uint32 unit);


/* Function Name:
 *      drv_gpio_init
 * Description:
 *      GPIO driver initilization
 * Input:
 *      unit    - unit ID
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
extern int32
drv_gpio_init(uint32 unit);

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
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
drv_gpio_isrStsShadow_backup(void);

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
extern int32
drv_gpio_isrStsShadow_get(uint32 unit, GPIO_INTERNAL_PIN_t pin, uint32 *pData);

#endif/*__DRV_GPIO_H__*/

