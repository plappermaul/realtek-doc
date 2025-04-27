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
 * Purpose : Definition those public External GPIO routing APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *           (1) External GPIO
 *
 */
#ifndef __DRV_EXT_GPIO_H__
#define __DRV_EXT_GPIO_H__

/*
 * Include Files
 */
#include <common/type.h>
#include <drv/gpio/gpio.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
typedef enum drv_extGpio_devId_e
{
    EXT_GPIO_DEV_ID0 = 0,
    EXT_GPIO_DEV_ID1,
    EXT_GPIO_DEV_ID2,
    EXT_GPIO_DEV_ID3,
    EXT_GPIO_DEV_ID4,
    EXT_GPIO_DEV_ID5,
    EXT_GPIO_DEV_ID6,
    EXT_GPIO_DEV_ID7,
    EXT_GPIO_DEV_ID_END
} drv_extGpio_devId_t;

typedef enum drv_extGpio_accessMode_e
{
    EXT_GPIO_ACCESS_MODE_I2C = 0,   /* I2C, applicable to 8328 */
    EXT_GPIO_ACCESS_MODE_MDC,       /* MDC/MDIC, applicable to 8389 */
    EXT_GPIO_ACCESS_MODE_END
} drv_extGpio_accessMode_t;

typedef enum drv_extGpio_directAccessMode_e
{
    EXT_GPIO_DIRECT_ACCESS_MODE_TRIGGER = 0,
    EXT_GPIO_DIRECT_ACCESS_MODE_CONTINUOUS,
    EXT_GPIO_DIRECT_ACCESS_MODE_END
} drv_extGpio_directAccessMode_t;

typedef struct drv_extGpio_devConf_s
{
    drv_extGpio_accessMode_t access_mode; /* I2C or MDC or EXTRA */
    uint32 address; /* Slave address or PHY address, valid when access_mode is MDC or MDC */
    uint32 page;    /* page number, valid when access_mode is MDC */
} drv_extGpio_devConf_t;

typedef enum drv_extGpio_id_e
{
    EXT_GPIO_ID0 = 0,
    EXT_GPIO_ID1,
    EXT_GPIO_ID2,
    EXT_GPIO_ID3,
    EXT_GPIO_ID4,
    EXT_GPIO_ID5,
    EXT_GPIO_ID6,
    EXT_GPIO_ID7,
    EXT_GPIO_ID8,
    EXT_GPIO_ID9,
    EXT_GPIO_ID10,
    EXT_GPIO_ID11,
    EXT_GPIO_ID12,
    EXT_GPIO_ID13,
    EXT_GPIO_ID14,
    EXT_GPIO_ID15,
    EXT_GPIO_ID16,
    EXT_GPIO_ID17,
    EXT_GPIO_ID18,
    EXT_GPIO_ID19,
    EXT_GPIO_ID20,
    EXT_GPIO_ID21,
    EXT_GPIO_ID22,
    EXT_GPIO_ID23,
    EXT_GPIO_ID24,
    EXT_GPIO_ID25,
    EXT_GPIO_ID26,
    EXT_GPIO_ID27,
    EXT_GPIO_ID28,
    EXT_GPIO_ID29,
    EXT_GPIO_ID30,
    EXT_GPIO_ID31,
    EXT_GPIO_ID32,
    EXT_GPIO_ID33,
    EXT_GPIO_ID34,
    EXT_GPIO_ID35,
    EXT_GPIO_ID36,
    EXT_GPIO_ID_END
} drv_extGpio_id_t;

/* define GPIO Interrupt Type */
typedef enum drv_extGpio_interruptType_e
{
    EXT_GPIO_INT_DISABLE = 0,
    EXT_GPIO_INT_FALLING_EDGE,
    EXT_GPIO_INT_RISING_EDGE,
    EXT_GPIO_INT_BOTH_EDGE,
    EXT_GPIO_INT_TYPE_END
} drv_extGpio_interruptType_t;


typedef struct drv_extGpio_conf_s
{
    uint32 direction;   /* GPIO_DIR_OUT or GPIO_DIR_IN */
    uint32 debounce;    /* 0: normal, 1: debounce */
    uint32 inverter;    /* 0: normal, 1: invert */
    drv_extGpio_interruptType_t interruptEnable;
} drv_extGpio_conf_t;

typedef osal_isrret_t (*drv_extGpioIsr_cb_f)(void *pParam);


/*
 * Macro Definition
 */

#define EXT_GPIO_INTR_THREAD_NAME  "EXT_GPIO_INTR_THREAD"


/*
 * Function Declaration
 */



/* Function Name:
 *      drv_extGpio_reg_read
 * Description:
 *      read external gpio device register
 * Input:
 *      unit - unit id
 *      dev  - external GPIO dev id
 *      reg  - register to read
 * Output:
 *      pData - read data
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
drv_extGpio_reg_read(uint32 unit, uint32 dev, uint32 reg, uint32 *pData);

/* Function Name:
 *      drv_extGpio_reg_write
 * Description:
 *      write external gpio device register
 * Input:
 *      unit - unit id
 *      dev  - external GPIO dev id
 *      reg  - register to write
 *      data - write data
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
drv_extGpio_reg_write(uint32 unit, uint32 dev, uint32 reg, uint32 data);

/* Function Name:
 *      drv_extGpio_devReady_get
 * Description:
 *      Get external GPIO ready status
 * Input:
 *      unit - unit id
 *      dev  - external GPIO dev id
 * Output:
 *      pIsReady - the device ready status
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
drv_extGpio_devReady_get(uint32 unit, uint32 dev, uint32 *pIsReady);

/* Function Name:
 *      drv_extGpio_dev_get
 * Description:
 *      Get the external GPIO settings in the specified device of the unit
 * Input:
 *      unit  - unit id
 *      dev   - external GPIO dev id
 * Output:
 *      pData - initialize configuration data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Get the access mode and relatived information from driver.
 * Changes:
 *      None
 */
extern int32
drv_extGpio_dev_get(uint32 unit, uint32 dev, drv_extGpio_devConf_t *pData);

/* Function Name:
 *      drv_extGpio_dev_init
 * Description:
 *      Initialize the external GPIO in the specified device of the unit
 * Input:
 *      unit  - unit id
 *      dev   - external GPIO dev id
 *      pData - initialize configuration data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      Specified the access mode and relatived information to driver.
 * Changes:
 *      None
 */
extern int32
drv_extGpio_dev_init(uint32 unit, uint32 dev, drv_extGpio_devConf_t *pData);

/* Function Name:
 *      drv_extGpio_devEnable_get
 * Description:
 *      Get the external GPIO status in the specified device of the unit
 * Input:
 *      unit - unit id
 *      dev  - external GPIO dev id
 * Output:
 *      pEnable - the buffer pointer of the status
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
drv_extGpio_devEnable_get(uint32 unit, uint32 dev, rtk_enable_t *pEnable);

/* Function Name:
 *      drv_extGpio_devEnable_set
 * Description:
 *      Set the external GPIO status in the specified device of the unit
 * Input:
 *      unit   - unit id
 *      dev    - external GPIO dev id
 *      enable - the status of the specified external GPIO device
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
drv_extGpio_devEnable_set(uint32 unit, uint32 dev, rtk_enable_t enable);

/* Function Name:
 *      drv_extGpio_syncEnable_get
 * Description:
 *      Get the external GPIO sync configuration status in the specified device of the unit
 * Input:
 *      unit - unit id
 *      dev  - external GPIO dev id
 * Output:
 *      pEnable - the buffer pointer of the sync configuration status
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
drv_extGpio_syncEnable_get(uint32 unit, uint32 dev, rtk_enable_t *pEnable);

/* Function Name:
 *      drv_extGpio_syncEnable_set
 * Description:
 *      Set the external GPIO sync configuration status in the specified device of the unit
 * Input:
 *      unit   - unit id
 *      dev    - external GPIO dev id
 *      enable - the sync configuration status
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
drv_extGpio_syncEnable_set(uint32 unit, uint32 dev, rtk_enable_t enable);

/* Function Name:
 *      drv_extGpio_syncStatus_get
 * Description:
 *      Get the external GPIO sync progress status in the specified device of the unit
 * Input:
 *      unit - unit id
 *      dev  - external GPIO dev id
 * Output:
 *      pData - the buffer pointer of the sync progress status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      The output value 1 means in progress and 0 means completed.
 * Changes:
 *      None
 */
extern int32
drv_extGpio_syncStatus_get(uint32 unit, uint32 dev, uint32 *pData);

/* Function Name:
 *      drv_extGpio_sync_start
 * Description:
 *      Trigger the external GPIO sync progress in the specified device of the unit
 * Input:
 *      unit - unit id
 *      dev  - external GPIO dev id
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
drv_extGpio_sync_start(uint32 unit, uint32 dev);

/* Function Name:
 *      drv_extGpio_pin_get
 * Description:
 *      Get the external GPIO pin function settings in the specified device of the unit
 * Input:
 *      unit   - unit id
 *      dev    - external GPIO dev id
 *      gpioId - gpio id
 * Output:
 *      pData  - the gpio pin configuration data
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
drv_extGpio_pin_get(uint32 unit, uint32 dev, uint32 gpioId, drv_extGpio_conf_t *pData);

/* Function Name:
 *      drv_extGpio_pin_init
 * Description:
 *      Initialize the external GPIO pin function in the specified device of the unit
 * Input:
 *      unit   - unit id
 *      dev    - external GPIO dev id
 *      gpioId - gpio id
 *      pData  - the gpio pin configuration data
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
drv_extGpio_pin_init(uint32 unit, uint32 dev, uint32 gpioId, drv_extGpio_conf_t *pData);

/* Function Name:
 *      drv_extGpio_dataBit_get
 * Description:
 *      Get the external GPIO pin value in the specified device of the unit
 * Input:
 *      unit   - unit id
 *      dev    - external GPIO dev id
 *      gpioId - gpio id
 * Output:
 *      pData  - the buffer pointer of the gpio pin value
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
drv_extGpio_dataBit_get(uint32 unit, uint32 dev, uint32 gpioId, uint32 *pData);

/* Function Name:
 *      drv_extGpio_dataBit_set
 * Description:
 *      Set the external GPIO pin value in the specified device of the unit
 * Input:
 *      unit   - unit id
 *      dev    - external GPIO dev id
 *      gpioId - gpio id
 *      data  - the gpio pin value
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
drv_extGpio_dataBit_set(uint32 unit, uint32 dev, uint32 gpioId, uint32 data);

/* Function Name:
 *      drv_extGpio_devRecovery_start
 * Description:
 *      Recovery the external GPIO status in the specified device of the unit
 * Input:
 *      unit   - unit id
 *      dev    - external GPIO dev id
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
drv_extGpio_devRecovery_start(uint32 unit, uint32 dev);

/* Function Name:
 *      drv_extGpio_direction_get
 * Description:
 *      Get the external GPIO pin direction in the specified device of the unit
 * Input:
 *      unit   - unit id
 *      dev    - external GPIO dev id
 *      gpioId - gpio id
 * Output:
 *      pData  - the buffer pointer of the gpio pin direction
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
drv_extGpio_direction_get(uint32 unit, uint32 dev, uint32 gpioId, drv_gpio_direction_t *pData);

/* Function Name:
 *      drv_extGpio_direction_set
 * Description:
 *      Set the external GPIO pin direction in the specified device of the unit
 * Input:
 *      unit   - unit id
 *      dev    - external GPIO dev id
 *      gpioId - gpio id
 *      data  - the gpio pin direction
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
drv_extGpio_direction_set(uint32 unit, uint32 dev, uint32 gpioId, drv_gpio_direction_t data);

/* Function Name:
 *      drv_extGpio_i2c_init
 * Description:
 *      Initialize the SCK/SDA in external GPIO pin in the specified device of the unit
 * Input:
 *      unit      - unit id
 *      dev       - external GPIO dev id
 *      i2c_clock - i2c SCK pin in external GPIO device
 *      i2c_data  - i2c SDA pin in external GPIO device
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Applicable:
 *      8380
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
drv_extGpio_i2c_init(uint32 unit, uint32 dev, uint32 i2c_clock, uint32 i2c_data);

/* Function Name:
 *      drv_extGpio_i2c_get
 * Description:
 *      Get the SCK/SDA in external GPIO pin in the specified device of the unit
 * Input:
 *      unit       - unit id
 *      dev        - external GPIO dev id
 * Output:
 *      pI2c_clock - buffer pointer of i2c SCK pin in external GPIO device
 *      pI2c_data  - buffer pointer of i2c SDA pin in external GPIO device
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Applicable:
 *      8380
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
drv_extGpio_i2c_get(uint32 unit, uint32 dev, uint32 *pI2c_clock, uint32 *pI2c_data);

/* Function Name:
 *      drv_extGpio_i2c_read
 * Description:
 *      Read the value of register in external GPIO pin in the specified device of the unit
 * Input:
 *      unit  - unit id
 *      dev   - external GPIO dev id
 *      reg   - register to read
 * Output:
 *      pData - buffer pointer of data value in external GPIO device
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Applicable:
 *      8380
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
drv_extGpio_i2c_read(uint32 unit, uint32 dev, uint32 reg, uint32 *pData);

/* Function Name:
 *      drv_extGpio_i2c_write
 * Description:
 *      Write the value of register in external GPIO pin in the specified device of the unit
 * Input:
 *      unit - unit id
 *      dev  - external GPIO dev id
 *      reg  - register to read
 *      data - data value in external GPIO device
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Applicable:
 *      8380
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
drv_extGpio_i2c_write(uint32 unit, uint32 dev, uint32 reg, uint32 data);

/* Function Name:
 *      drv_extGpio_intrHandler_register
 * Description:
 *      Register External GPIO interrupt callback function for specified EXT GPIO pin
 * Input:
 *      unit               - unit id
 *      dev                - external GPIO dev id
 *      gpioId             - gpio id
 *      extGpioIsrCallback - callback function
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Applicable:
 *       9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
drv_extGpio_intrHandler_register(uint32 unit, uint32 dev, uint32 gpioId, drv_extGpioIsr_cb_f extGpioIsrCallback);

/* Function Name:
 *      drv_extGpio_intrHandler_unregister
 * Description:
 *      Unregister EXT GPIO interrupt callback function for specified GPIO pin
 * Input:
 *      unit               - unit id
 *      dev                - external GPIO dev id
 *      gpioId             - gpio id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
drv_extGpio_intrHandler_unregister(uint32 unit, uint32 dev, uint32 gpioId);

#endif/*__DRV_EXT_GPIO_H__*/
