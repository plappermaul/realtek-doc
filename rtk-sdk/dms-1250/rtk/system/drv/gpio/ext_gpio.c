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

/*
 * Include Files
 */
#include <common/error.h>
#include <drv/gpio/ext_gpio.h>
#include <private/drv/rtl8231/rtl8231.h>
#include <private/drv/swcore/swcore.h>
#include <private/drv/swcore/chip_probe.h>
#include <common/rt_type.h>
#include <common/debug/rt_log.h>
#include <osal/sem.h>
#include <hwp/hw_profile.h>
#include <dev_config.h>
#include <drv/intr/intr.h>
#include <common/util/rt_util_intr.h>
#if defined(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE) || (defined(CONFIG_SDK_KERNEL_LINUX_USER_MODE) && !defined(__KERNEL__))
#include <osal/wait.h>
#include <osal/thread.h>
#include <soc/type.h>
#endif
/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
typedef struct drv_extGpio_devConfEntry_s
{
    uint32                  valid;
    drv_extGpio_devConf_t   data;
    osal_mutex_t            extGpio_sem;
    uint32                  regVal[31]; /* keep the shadow value of the RTL8231 register 0~30 */
} drv_extGpio_devConfEntry_t;

static drv_extGpio_devConfEntry_t extGpioConfEntry[RTK_MAX_NUM_OF_UNIT][EXT_GPIO_DEV_ID_END];

#define EXTGPIO_SEM_LOCK(unit, dev)    \
do {\
    if (osal_sem_mutex_take(extGpioConfEntry[unit][dev].extGpio_sem, OSAL_SEM_WAIT_FOREVER) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_LOCK_FAILED, (MOD_DAL|MOD_PORT), "semaphore lock failed");\
        return RT_ERR_SEM_LOCK_FAILED;\
    }\
} while(0)

#define EXTGPIO_SEM_UNLOCK(unit, dev)   \
do {\
    if (osal_sem_mutex_give(extGpioConfEntry[unit][dev].extGpio_sem) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_UNLOCK_FAILED, (MOD_DAL|MOD_PORT), "semaphore unlock failed");\
        return RT_ERR_SEM_UNLOCK_FAILED;\
    }\
} while(0)

typedef struct extGpio_database_s
{
    uint32  register_pin;
    uint32  init_pin;
    uint32  action_status;
    drv_gpioIsr_cb_f callback;

} extGpio_database_t;

static extGpio_database_t extGpiodata[EXT_GPIO_DEV_ID_END][EXT_GPIO_ID_END];

#if defined(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE) || (defined(CONFIG_SDK_KERNEL_LINUX_USER_MODE) && !defined(__KERNEL__))
rtk_enable_t extGpio_threadEnabled = DISABLED;
static drv_extGpioIsr_cb_f extGpioInterruptCb[EXT_GPIO_DEV_ID_END][EXT_GPIO_ID_END];
static osal_event_t drv_extGpioIsr_event;
#endif


/*
 * Macro Definition
 */
#define IS_EXTGPIO_UNIT_DEV_INVALID(unit, dev)  ((extGpioConfEntry[unit][dev].valid) ? 0 : 1)

/*
 * Function Declaration
 */


/*
 * Function Body
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
int32
drv_extGpio_reg_read(uint32 unit, uint32 dev, uint32 reg, uint32 *pData)
{
    int32   ret = RT_ERR_FAILED;

    RT_PARAM_CHK((unit >= RTK_MAX_NUM_OF_UNIT), RT_ERR_INPUT);
    RT_PARAM_CHK((dev >= EXT_GPIO_DEV_ID_END), RT_ERR_INPUT);
    RT_PARAM_CHK(IS_EXTGPIO_UNIT_DEV_INVALID(unit, dev), RT_ERR_INPUT);
    RT_PARAM_CHK((reg > 31), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    switch (extGpioConfEntry[unit][dev].data.access_mode)
    {
        case EXT_GPIO_ACCESS_MODE_I2C:
            RT_ERR_CHK(drv_rtl8231_i2c_read(unit, extGpioConfEntry[unit][dev].data.address, reg, pData), ret);
            break;
        case EXT_GPIO_ACCESS_MODE_MDC:
            RT_ERR_CHK(drv_rtl8231_mdc_read(unit, extGpioConfEntry[unit][dev].data.address, extGpioConfEntry[unit][dev].data.page, reg, pData), ret);
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    return RT_ERR_OK;
} /* end of drv_extGpio_reg_read */

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
int32
drv_extGpio_reg_write(uint32 unit, uint32 dev, uint32 reg, uint32 data)
{
    int32   ret = RT_ERR_FAILED;

    RT_PARAM_CHK((unit >= RTK_MAX_NUM_OF_UNIT), RT_ERR_INPUT);
    RT_PARAM_CHK((dev >= EXT_GPIO_DEV_ID_END), RT_ERR_INPUT);
    RT_PARAM_CHK(IS_EXTGPIO_UNIT_DEV_INVALID(unit, dev), RT_ERR_INPUT);
    RT_PARAM_CHK((reg > 31), RT_ERR_INPUT);

    switch (extGpioConfEntry[unit][dev].data.access_mode)
    {
        case EXT_GPIO_ACCESS_MODE_I2C:
            RT_ERR_CHK(drv_rtl8231_i2c_write(unit, extGpioConfEntry[unit][dev].data.address, reg, data), ret);
            break;
        case EXT_GPIO_ACCESS_MODE_MDC:
            RT_ERR_CHK(drv_rtl8231_mdc_write(unit, extGpioConfEntry[unit][dev].data.address, extGpioConfEntry[unit][dev].data.page, reg, data), ret);
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }
    /* keep the register value to shadow */
    extGpioConfEntry[unit][dev].regVal[reg] = data;
    return RT_ERR_OK;
} /* end of drv_extGpio_reg_write */

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
int32
drv_extGpio_devReady_get(uint32 unit, uint32 dev, uint32 *pIsReady)
{
    uint32  regVal = 0;
    int32   ret = RT_ERR_FAILED;

    RT_PARAM_CHK((unit >= RTK_MAX_NUM_OF_UNIT), RT_ERR_INPUT);
    RT_PARAM_CHK((dev >= EXT_GPIO_DEV_ID_END), RT_ERR_INPUT);
    RT_PARAM_CHK(IS_EXTGPIO_UNIT_DEV_INVALID(unit, dev), RT_ERR_INPUT);

    EXTGPIO_SEM_LOCK(unit, dev);

    switch (extGpioConfEntry[unit][dev].data.access_mode)
    {
        case EXT_GPIO_ACCESS_MODE_I2C:
        case EXT_GPIO_ACCESS_MODE_MDC:
            RT_ERR_HDL(drv_extGpio_reg_read(unit, dev, RTL8231_LED_FUNC1_ADDR, &regVal), errHandle, ret);
            if (RTL8231_LED_FUNC1_READY == (regVal & RTL8231_LED_FUNC1_READY))
                *pIsReady = 1;
            else
                *pIsReady = 0;
            break;
        default:
            EXTGPIO_SEM_UNLOCK(unit, dev);
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    EXTGPIO_SEM_UNLOCK(unit, dev);
    return RT_ERR_OK;

  errHandle:
    EXTGPIO_SEM_UNLOCK(unit, dev);
    return ret;

} /* end of drv_extGpio_devReady_get */

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
int32
drv_extGpio_dev_get(uint32 unit, uint32 dev, drv_extGpio_devConf_t *pData)
{
    RT_PARAM_CHK((unit >= RTK_MAX_NUM_OF_UNIT), RT_ERR_INPUT);
    RT_PARAM_CHK((dev >= EXT_GPIO_DEV_ID_END), RT_ERR_INPUT);
    RT_PARAM_CHK(IS_EXTGPIO_UNIT_DEV_INVALID(unit, dev), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    if (extGpioConfEntry[unit][dev].valid == 1)
    {
        pData->access_mode = extGpioConfEntry[unit][dev].data.access_mode;
        pData->address = extGpioConfEntry[unit][dev].data.address;
        pData->page = extGpioConfEntry[unit][dev].data.page;
        return RT_ERR_OK;
    }

    return RT_ERR_FAILED;
} /* end of drv_extGpio_dev_get */

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
int32
drv_extGpio_dev_init(uint32 unit, uint32 dev, drv_extGpio_devConf_t *pData)
{
    uint32  i;
    int32   ret;

    RT_PARAM_CHK((unit >= RTK_MAX_NUM_OF_UNIT), RT_ERR_INPUT);
    RT_PARAM_CHK((dev >= EXT_GPIO_DEV_ID_END), RT_ERR_INPUT);
    //RT_PARAM_CHK(IS_EXTGPIO_UNIT_DEV_INVALID(unit, dev), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    extGpioConfEntry[unit][dev].data.access_mode = pData->access_mode;
    extGpioConfEntry[unit][dev].data.address = pData->address;
    extGpioConfEntry[unit][dev].data.page = pData->page;
    extGpioConfEntry[unit][dev].valid = 1;

    extGpioConfEntry[unit][dev].extGpio_sem = osal_sem_mutex_create();

    if (extGpioConfEntry[unit][dev].data.access_mode == EXT_GPIO_ACCESS_MODE_I2C ||
        extGpioConfEntry[unit][dev].data.access_mode == EXT_GPIO_ACCESS_MODE_MDC)
    {
        /* keep the register value to shadow */
        for (i=0; i<31; i++)
        {
            RT_ERR_CHK(drv_extGpio_reg_read(unit, dev, i, &extGpioConfEntry[unit][dev].regVal[i]), ret);
        }
    }

    return RT_ERR_OK;
} /* end of drv_extGpio_dev_init */

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
int32
drv_extGpio_devEnable_get(uint32 unit, uint32 dev, rtk_enable_t *pEnable)
{
    uint32  regVal = 0;
    int32   ret = RT_ERR_FAILED;

    RT_PARAM_CHK((unit >= RTK_MAX_NUM_OF_UNIT), RT_ERR_INPUT);
    RT_PARAM_CHK((dev >= EXT_GPIO_DEV_ID_END), RT_ERR_INPUT);
    RT_PARAM_CHK(IS_EXTGPIO_UNIT_DEV_INVALID(unit, dev), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    EXTGPIO_SEM_LOCK(unit, dev);

    switch (extGpioConfEntry[unit][dev].data.access_mode)
    {
        case EXT_GPIO_ACCESS_MODE_I2C:
        case EXT_GPIO_ACCESS_MODE_MDC:
            RT_ERR_HDL(drv_extGpio_reg_read(unit, dev, RTL8231_LED_FUNC0_ADDR, &regVal), errHandle, ret);
            *pEnable = (regVal & RTL8231_LED_FUNC0_LED_START_MASK) >> RTL8231_LED_FUNC0_LED_START_OFFSET;
            break;
        default:
            EXTGPIO_SEM_UNLOCK(unit, dev);
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    EXTGPIO_SEM_UNLOCK(unit, dev);
    return RT_ERR_OK;

  errHandle:
    EXTGPIO_SEM_UNLOCK(unit, dev);
    return ret;

} /* end of drv_extGpio_devEnable_get */

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
int32
drv_extGpio_devEnable_set(uint32 unit, uint32 dev, rtk_enable_t enable)
{
    uint32  regVal = 0;
    int32   ret = RT_ERR_FAILED;

    RT_PARAM_CHK((unit >= RTK_MAX_NUM_OF_UNIT), RT_ERR_INPUT);
    RT_PARAM_CHK((dev >= EXT_GPIO_DEV_ID_END), RT_ERR_INPUT);
    RT_PARAM_CHK(IS_EXTGPIO_UNIT_DEV_INVALID(unit, dev), RT_ERR_INPUT);
    RT_PARAM_CHK(enable >= RTK_ENABLE_END, RT_ERR_INPUT);

    EXTGPIO_SEM_LOCK(unit, dev);

    switch (extGpioConfEntry[unit][dev].data.access_mode)
    {
        case EXT_GPIO_ACCESS_MODE_I2C:
        case EXT_GPIO_ACCESS_MODE_MDC:
            RT_ERR_HDL(drv_extGpio_reg_read(unit, dev, RTL8231_LED_FUNC0_ADDR, &regVal), errHandle, ret);
            regVal = (regVal & ~RTL8231_LED_FUNC0_LED_START_MASK) | (enable << RTL8231_LED_FUNC0_LED_START_OFFSET);
            RT_ERR_HDL(drv_extGpio_reg_write(unit, dev, RTL8231_LED_FUNC0_ADDR, regVal), errHandle, ret);
            break;
        default:
            EXTGPIO_SEM_UNLOCK(unit, dev);
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    EXTGPIO_SEM_UNLOCK(unit, dev);
    return RT_ERR_OK;

  errHandle:
    EXTGPIO_SEM_UNLOCK(unit, dev);
    return ret;
} /* end of drv_extGpio_devEnable_set */

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
int32
drv_extGpio_syncEnable_get(uint32 unit, uint32 dev, rtk_enable_t *pEnable)
{
    uint32  regVal = 0;
    int32   ret = RT_ERR_FAILED;

    RT_PARAM_CHK((unit >= RTK_MAX_NUM_OF_UNIT), RT_ERR_INPUT);
    RT_PARAM_CHK((dev >= EXT_GPIO_DEV_ID_END), RT_ERR_INPUT);
    RT_PARAM_CHK(IS_EXTGPIO_UNIT_DEV_INVALID(unit, dev), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    EXTGPIO_SEM_LOCK(unit, dev);
    switch (extGpioConfEntry[unit][dev].data.access_mode)
    {
        case EXT_GPIO_ACCESS_MODE_I2C:
        case EXT_GPIO_ACCESS_MODE_MDC:
            RT_ERR_HDL(drv_extGpio_reg_read(unit, dev, RTL8231_LED_FUNC0_ADDR, &regVal), errHandle, ret);
            *pEnable = (regVal & RTL8231_LED_FUNC0_EN_SYNC_GPIO_MASK) >> RTL8231_LED_FUNC0_EN_SYNC_GPIO_OFFSET;
            break;
        default:
            EXTGPIO_SEM_UNLOCK(unit, dev);
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    EXTGPIO_SEM_UNLOCK(unit, dev);
    return RT_ERR_OK;

  errHandle:
    EXTGPIO_SEM_UNLOCK(unit, dev);
    return ret;
} /* end of drv_extGpio_syncEnable_get */

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
int32
drv_extGpio_syncEnable_set(uint32 unit, uint32 dev, rtk_enable_t enable)
{
    uint32  regVal = 0;
    int32   ret = RT_ERR_FAILED;

    RT_PARAM_CHK((unit >= RTK_MAX_NUM_OF_UNIT), RT_ERR_INPUT);
    RT_PARAM_CHK((dev >= EXT_GPIO_DEV_ID_END), RT_ERR_INPUT);
    RT_PARAM_CHK(IS_EXTGPIO_UNIT_DEV_INVALID(unit, dev), RT_ERR_INPUT);
    RT_PARAM_CHK(enable >= RTK_ENABLE_END, RT_ERR_INPUT);

    EXTGPIO_SEM_LOCK(unit, dev);
    switch (extGpioConfEntry[unit][dev].data.access_mode)
    {
        case EXT_GPIO_ACCESS_MODE_I2C:
        case EXT_GPIO_ACCESS_MODE_MDC:
            RT_ERR_HDL(drv_extGpio_reg_read(unit, dev, RTL8231_LED_FUNC0_ADDR, &regVal), errHandle, ret);
            regVal = (regVal & ~RTL8231_LED_FUNC0_EN_SYNC_GPIO_MASK) | (enable << RTL8231_LED_FUNC0_EN_SYNC_GPIO_OFFSET);
            RT_ERR_HDL(drv_extGpio_reg_write(unit, dev, RTL8231_LED_FUNC0_ADDR, regVal), errHandle, ret);
            break;
        default:
            EXTGPIO_SEM_UNLOCK(unit, dev);
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    EXTGPIO_SEM_UNLOCK(unit, dev);
    return RT_ERR_OK;

  errHandle:
    EXTGPIO_SEM_UNLOCK(unit, dev);
    return ret;
} /* end of drv_extGpio_syncEnable_set */

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
int32
drv_extGpio_syncStatus_get(uint32 unit, uint32 dev, uint32 *pData)
{
    uint32  regVal = 0;
    int32   ret = RT_ERR_FAILED;

    RT_PARAM_CHK((unit >= RTK_MAX_NUM_OF_UNIT), RT_ERR_INPUT);
    RT_PARAM_CHK((dev >= EXT_GPIO_DEV_ID_END), RT_ERR_INPUT);
    RT_PARAM_CHK(IS_EXTGPIO_UNIT_DEV_INVALID(unit, dev), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    EXTGPIO_SEM_LOCK(unit, dev);
    switch (extGpioConfEntry[unit][dev].data.access_mode)
    {
        case EXT_GPIO_ACCESS_MODE_I2C:
        case EXT_GPIO_ACCESS_MODE_MDC:
            RT_ERR_HDL(drv_extGpio_reg_read(unit, dev, RTL8231_GPIO_CTRL2_ADDR, &regVal), errHandle, ret);
            *pData = (regVal & RTL8231_GPIO_CTRL2_SYNC_GPIO_MASK) >> RTL8231_GPIO_CTRL2_SYNC_GPIO_OFFSET;
            break;
        default:
            EXTGPIO_SEM_UNLOCK(unit, dev);
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    EXTGPIO_SEM_UNLOCK(unit, dev);
    return RT_ERR_OK;

  errHandle:
    EXTGPIO_SEM_UNLOCK(unit, dev);
    return ret;
} /* end of drv_extGpio_syncStatus_get */

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
int32
drv_extGpio_sync_start(uint32 unit, uint32 dev)
{
    uint32  regVal = 0;
    int32   ret = RT_ERR_FAILED;

    RT_PARAM_CHK((unit >= RTK_MAX_NUM_OF_UNIT), RT_ERR_INPUT);
    RT_PARAM_CHK((dev >= EXT_GPIO_DEV_ID_END), RT_ERR_INPUT);
    RT_PARAM_CHK(IS_EXTGPIO_UNIT_DEV_INVALID(unit, dev), RT_ERR_INPUT);

    EXTGPIO_SEM_LOCK(unit, dev);
    switch (extGpioConfEntry[unit][dev].data.access_mode)
    {
        case EXT_GPIO_ACCESS_MODE_I2C:
        case EXT_GPIO_ACCESS_MODE_MDC:
            RT_ERR_HDL(drv_extGpio_reg_read(unit, dev, RTL8231_GPIO_CTRL2_ADDR, &regVal), errHandle, ret);
            regVal = (regVal | RTL8231_LED_FUNC0_EN_SYNC_GPIO_MASK);
            RT_ERR_HDL(drv_extGpio_reg_write(unit, dev, RTL8231_GPIO_CTRL2_ADDR, regVal), errHandle, ret);
            break;
        default:
            EXTGPIO_SEM_UNLOCK(unit, dev);
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    EXTGPIO_SEM_UNLOCK(unit, dev);
    return RT_ERR_OK;

  errHandle:
    EXTGPIO_SEM_UNLOCK(unit, dev);
    return ret;
} /* end of drv_extGpio_sync_start */

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
int32
drv_extGpio_pin_get(uint32 unit, uint32 dev, uint32 gpioId, drv_extGpio_conf_t *pData)
{
    uint32  regVal = 0, sel_gpio = 0;
    int32   ret = RT_ERR_FAILED;

    RT_PARAM_CHK((unit >= RTK_MAX_NUM_OF_UNIT), RT_ERR_INPUT);
    RT_PARAM_CHK((dev >= EXT_GPIO_DEV_ID_END), RT_ERR_INPUT);
    RT_PARAM_CHK(IS_EXTGPIO_UNIT_DEV_INVALID(unit, dev), RT_ERR_INPUT);
    RT_PARAM_CHK((gpioId >= EXT_GPIO_ID_END), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    EXTGPIO_SEM_LOCK(unit, dev);
    switch (extGpioConfEntry[unit][dev].data.access_mode)
    {
        case EXT_GPIO_ACCESS_MODE_I2C:
        case EXT_GPIO_ACCESS_MODE_MDC:
            /* GPIO Selection */
            RT_ERR_HDL(drv_extGpio_reg_read(unit, dev, RTL8231_GPIO_PIN_SEL_ADDR(gpioId), &regVal), errHandle, ret);
            sel_gpio = (regVal >> (gpioId % 16)) & 0x1;
            if (sel_gpio)
            {
                /* Direction */
                if (gpioId < EXT_GPIO_ID32)
                {
                    RT_ERR_HDL(drv_extGpio_reg_read(unit, dev, RTL8231_GPIO_IO_SEL_ADDR(gpioId), &regVal), errHandle, ret);
                    if ((regVal >> (gpioId % 16)) & 0x1)
                        pData->direction = GPIO_DIR_IN;
                    else
                        pData->direction = GPIO_DIR_OUT;
                }
                else
                {
                    RT_ERR_HDL(drv_extGpio_reg_read(unit, dev, RTL8231_GPIO_PIN_SEL2_ADDR, &regVal), errHandle, ret);
                    if ((regVal >> ((gpioId % 32) + RTL8231_GPIO_PIN_SEL2_IOMASK_32_OFFSET)) & 0x1)
                        pData->direction = GPIO_DIR_IN;
                    else
                        pData->direction = GPIO_DIR_OUT;
                }
                /* Debounce */
                if (gpioId >= EXT_GPIO_ID31)
                {
                    RT_ERR_HDL(drv_extGpio_reg_read(unit, dev, RTL8231_LED_FUNC1_ADDR, &regVal), errHandle, ret);
                    if ((regVal >> ((gpioId % 31) + RTL8231_LED_FUNC1_EN_DEBOUNCING_31_OFFSET)) & 0x1)
                        pData->debounce = 1;
                    else
                        pData->debounce = 0;
                }
                else
                {
                    pData->debounce = 0;
                }
                /* Inverter */
                if (gpioId < EXT_GPIO_ID32)
                {
                    RT_ERR_HDL(drv_extGpio_reg_read(unit, dev, RTL8231_GPIO_INV_SEL_ADDR(gpioId), &regVal), errHandle, ret);
                    if ((regVal >> (gpioId % 16)) & 0x1)
                        pData->inverter = 1;
                    else
                        pData->inverter = 0;
                }
                else
                {
                    RT_ERR_HDL(drv_extGpio_reg_read(unit, dev, RTL8231_GPIO_PIN_SEL2_ADDR, &regVal), errHandle, ret);
                    if ((regVal >> ((gpioId % 32) + RTL8231_GPIO_PIN_SEL2_INVMASK_32_OFFSET)) & 0x1)
                        pData->inverter = 1;
                    else
                        pData->inverter = 0;
                }
            }
            else
            {
                EXTGPIO_SEM_UNLOCK(unit, dev);
                return RT_ERR_FAILED;
            }
            break;
        default:
            EXTGPIO_SEM_UNLOCK(unit, dev);
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    EXTGPIO_SEM_UNLOCK(unit, dev);
    return RT_ERR_OK;

  errHandle:
    EXTGPIO_SEM_UNLOCK(unit, dev);
    return ret;
} /* end of drv_extGpio_pin_get */

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
int32
drv_extGpio_pin_init(uint32 unit, uint32 dev, uint32 gpioId, drv_extGpio_conf_t *pData)
{
    uint32  regVal = 0, value;
    int32   ret = RT_ERR_FAILED;
    drv_extGpio_interruptType_t intr_type;
    rtk_enable_t intr_ebable;

    RT_PARAM_CHK((unit >= RTK_MAX_NUM_OF_UNIT), RT_ERR_INPUT);
    RT_PARAM_CHK((dev >= EXT_GPIO_DEV_ID_END), RT_ERR_INPUT);
    RT_PARAM_CHK(IS_EXTGPIO_UNIT_DEV_INVALID(unit, dev), RT_ERR_INPUT);
    RT_PARAM_CHK((gpioId >= EXT_GPIO_ID_END), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    EXTGPIO_SEM_LOCK(unit, dev);
    switch (extGpioConfEntry[unit][dev].data.access_mode)
    {
        case EXT_GPIO_ACCESS_MODE_I2C:
        case EXT_GPIO_ACCESS_MODE_MDC:
            /* GPIO Selection */
            RT_ERR_HDL(drv_extGpio_reg_read(unit, dev, RTL8231_GPIO_PIN_SEL_ADDR(gpioId), &regVal), errHandle, ret);
            regVal = (regVal | (1 << (gpioId % 16)));
            RT_ERR_HDL(drv_extGpio_reg_write(unit, dev, RTL8231_GPIO_PIN_SEL_ADDR(gpioId), regVal), errHandle, ret);

            /* Direction */
            if (pData->direction == GPIO_DIR_IN)
                value = 1;
            else
                value = 0;

            if (gpioId < EXT_GPIO_ID32)
            {
                RT_ERR_HDL(drv_extGpio_reg_read(unit, dev, RTL8231_GPIO_IO_SEL_ADDR(gpioId), &regVal), errHandle, ret);
                regVal = (regVal & ~(1 << (gpioId % 16))) | (value << (gpioId % 16));
                RT_ERR_HDL(drv_extGpio_reg_write(unit, dev, RTL8231_GPIO_IO_SEL_ADDR(gpioId), regVal), errHandle, ret);
            }
            else
            {
                RT_ERR_HDL(drv_extGpio_reg_read(unit, dev, RTL8231_GPIO_PIN_SEL2_ADDR, &regVal), errHandle, ret);
                regVal = (regVal & ~(1 << ((gpioId % 32) + RTL8231_GPIO_PIN_SEL2_IOMASK_32_OFFSET))) | (value << ((gpioId % 32) + RTL8231_GPIO_PIN_SEL2_IOMASK_32_OFFSET));
                RT_ERR_HDL(drv_extGpio_reg_write(unit, dev, RTL8231_GPIO_PIN_SEL2_ADDR, regVal), errHandle, ret);
            }

            /* Debounce */
            if (gpioId >= EXT_GPIO_ID31)
            {
                RT_ERR_HDL(drv_extGpio_reg_read(unit, dev, RTL8231_LED_FUNC1_ADDR, &regVal), errHandle, ret);
                regVal = (regVal & ~(1 << ((gpioId % 31) + RTL8231_LED_FUNC1_EN_DEBOUNCING_31_OFFSET))) | (pData->debounce << ((gpioId % 31) + RTL8231_LED_FUNC1_EN_DEBOUNCING_31_OFFSET));
                RT_ERR_HDL(drv_extGpio_reg_write(unit, dev, RTL8231_LED_FUNC1_ADDR, regVal), errHandle, ret);
            }

            /* Inverter */
            if (gpioId < EXT_GPIO_ID32)
            {
                RT_ERR_HDL(drv_extGpio_reg_read(unit, dev, RTL8231_GPIO_INV_SEL_ADDR(gpioId), &regVal), errHandle, ret);
                regVal = (regVal & ~(1 << (gpioId % 16))) | (pData->inverter << (gpioId % 16));
                RT_ERR_HDL(drv_extGpio_reg_write(unit, dev, RTL8231_GPIO_INV_SEL_ADDR(gpioId), regVal), errHandle, ret);
            }
            else
            {
                RT_ERR_HDL(drv_extGpio_reg_read(unit, dev, RTL8231_GPIO_PIN_SEL2_ADDR, &regVal), errHandle, ret);
                regVal = (regVal & ~(1 << ((gpioId % 32) + RTL8231_GPIO_PIN_SEL2_INVMASK_32_OFFSET))) | (pData->inverter << ((gpioId % 32) + RTL8231_GPIO_PIN_SEL2_INVMASK_32_OFFSET));
                RT_ERR_HDL(drv_extGpio_reg_write(unit, dev, RTL8231_GPIO_PIN_SEL2_ADDR, regVal), errHandle, ret);
            }

            /* Interrupt Mode */
            intr_type = pData->interruptEnable;
            if(intr_type == EXT_GPIO_INT_DISABLE)
            {
                intr_ebable = DISABLED;
            }else{
                intr_ebable = ENABLED;
            }

            ret = drv_rtl8231_intrEnable_set(unit, dev, gpioId, intr_ebable);
            if((ret != RT_ERR_OK)&&(ret != RT_ERR_CHIP_NOT_SUPPORTED))
            {
                EXTGPIO_SEM_UNLOCK(unit, dev);
                return RT_ERR_CHIP_NOT_SUPPORTED;
            }
            ret = drv_rtl8231_intrMode_set(unit, dev, gpioId, intr_type);
            if((ret != RT_ERR_OK)&&(ret != RT_ERR_CHIP_NOT_SUPPORTED))
            {
                EXTGPIO_SEM_UNLOCK(unit, dev);
                return RT_ERR_CHIP_NOT_SUPPORTED;
            }

             extGpiodata[dev][gpioId].init_pin = GPIO_FLAG_ON;
            break;
        default:
            EXTGPIO_SEM_UNLOCK(unit, dev);
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    EXTGPIO_SEM_UNLOCK(unit, dev);
    return RT_ERR_OK;

  errHandle:
    EXTGPIO_SEM_UNLOCK(unit, dev);
    return ret;
} /* end of drv_extGpio_pin_init */

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
int32
drv_extGpio_dataBit_get(uint32 unit, uint32 dev, uint32 gpioId, uint32 *pData)
{
    uint32  regVal = 0;
    int32   ret = RT_ERR_FAILED;

    RT_PARAM_CHK((unit >= RTK_MAX_NUM_OF_UNIT), RT_ERR_INPUT);
    RT_PARAM_CHK((dev >= EXT_GPIO_DEV_ID_END), RT_ERR_INPUT);
    RT_PARAM_CHK(IS_EXTGPIO_UNIT_DEV_INVALID(unit, dev), RT_ERR_INPUT);
    RT_PARAM_CHK((gpioId >= EXT_GPIO_ID_END), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    EXTGPIO_SEM_LOCK(unit, dev);

    switch (extGpioConfEntry[unit][dev].data.access_mode)
    {
        case EXT_GPIO_ACCESS_MODE_I2C:
        case EXT_GPIO_ACCESS_MODE_MDC:
            ret = drv_extGpio_reg_read(unit, dev, RTL8231_GPIO_CTRL_ADDR(gpioId), &regVal);
            if(ret != RT_ERR_OK)
            {
                EXTGPIO_SEM_UNLOCK(unit, dev);
                return ret;
            }
            *pData = (regVal & (1 << (gpioId % 16))) >> (gpioId % 16);
            break;
        default:
            EXTGPIO_SEM_UNLOCK(unit, dev);
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    EXTGPIO_SEM_UNLOCK(unit, dev);
    return RT_ERR_OK;
} /* end of drv_extGpio_dataBit_get */

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
int32
drv_extGpio_dataBit_set(uint32 unit, uint32 dev, uint32 gpioId, uint32 data)
{
    uint32  regVal = 0;
    int32   ret = RT_ERR_FAILED;

    RT_PARAM_CHK((unit >= RTK_MAX_NUM_OF_UNIT), RT_ERR_INPUT);
    RT_PARAM_CHK((dev >= EXT_GPIO_DEV_ID_END), RT_ERR_INPUT);
    RT_PARAM_CHK(IS_EXTGPIO_UNIT_DEV_INVALID(unit, dev), RT_ERR_INPUT);
    RT_PARAM_CHK((gpioId >= EXT_GPIO_ID_END), RT_ERR_INPUT);

    EXTGPIO_SEM_LOCK(unit, dev);

    switch (extGpioConfEntry[unit][dev].data.access_mode)
    {
        case EXT_GPIO_ACCESS_MODE_I2C:
        case EXT_GPIO_ACCESS_MODE_MDC:
            ret = drv_extGpio_reg_read(unit, dev, RTL8231_GPIO_CTRL_ADDR(gpioId), &regVal);
            if(ret != RT_ERR_OK)
            {
                EXTGPIO_SEM_UNLOCK(unit, dev);
                return ret;
            }
            regVal = (regVal & ~(1 << (gpioId % 16))) | (data << (gpioId % 16));
            ret = drv_extGpio_reg_write(unit, dev, RTL8231_GPIO_CTRL_ADDR(gpioId), regVal);
            if(ret != RT_ERR_OK)
            {
                EXTGPIO_SEM_UNLOCK(unit, dev);
                return ret;
            }
            break;
        default:
            EXTGPIO_SEM_UNLOCK(unit, dev);
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    EXTGPIO_SEM_UNLOCK(unit, dev);

    return RT_ERR_OK;
} /* end of drv_extGpio_dataBit_set */

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
int32
drv_extGpio_devRecovery_start(uint32 unit, uint32 dev)
{
    uint32  regVal = 0, i;
    int32   ret = RT_ERR_FAILED;

    RT_PARAM_CHK((unit >= RTK_MAX_NUM_OF_UNIT), RT_ERR_INPUT);
    RT_PARAM_CHK((dev >= EXT_GPIO_DEV_ID_END), RT_ERR_INPUT);

    EXTGPIO_SEM_LOCK(unit, dev);
    for (i=1; i<31; i++)
    {
        RT_ERR_HDL(drv_extGpio_reg_write(unit, dev, i, extGpioConfEntry[unit][dev].regVal[i]), errHandle, ret);
    }

    /* Recovery the device to active */
    regVal = (extGpioConfEntry[unit][dev].regVal[0] & ~RTL8231_LED_FUNC0_LED_START_MASK) | (1 << RTL8231_LED_FUNC0_LED_START_OFFSET);
    RT_ERR_HDL(drv_extGpio_reg_write(unit, dev, RTL8231_LED_FUNC0_ADDR, regVal), errHandle, ret);

    EXTGPIO_SEM_UNLOCK(unit, dev);
    return RT_ERR_OK;

  errHandle:
    EXTGPIO_SEM_UNLOCK(unit, dev);
    return ret;
} /* end of drv_extGpio_devRecovery_start */

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
int32
drv_extGpio_direction_get(uint32 unit, uint32 dev, uint32 gpioId, drv_gpio_direction_t *pData)
{
    uint32  sel_gpio = 0;
    uint32  regVal = 0;
    int32   ret = RT_ERR_FAILED;

    RT_PARAM_CHK((unit >= RTK_MAX_NUM_OF_UNIT), RT_ERR_INPUT);
    RT_PARAM_CHK((dev >= EXT_GPIO_DEV_ID_END), RT_ERR_INPUT);
    RT_PARAM_CHK(IS_EXTGPIO_UNIT_DEV_INVALID(unit, dev), RT_ERR_INPUT);
    RT_PARAM_CHK((gpioId >= EXT_GPIO_ID_END), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    EXTGPIO_SEM_LOCK(unit, dev);
    switch (extGpioConfEntry[unit][dev].data.access_mode)
    {
        case EXT_GPIO_ACCESS_MODE_MDC:
            /* GPIO Selection */
            RT_ERR_HDL(drv_extGpio_reg_read(unit, dev, RTL8231_GPIO_PIN_SEL_ADDR(gpioId), &regVal), errHandle, ret);
            sel_gpio = (regVal >> (gpioId % 16)) & 0x1;
            if (sel_gpio)
            {
                /* Direction */
                if (gpioId < EXT_GPIO_ID32)
                {
                    RT_ERR_HDL(drv_extGpio_reg_read(unit, dev, RTL8231_GPIO_IO_SEL_ADDR(gpioId), &regVal), errHandle, ret);
                    if ((regVal >> (gpioId % 16)) & 0x1)
                        *pData = GPIO_DIR_IN;
                    else
                        *pData = GPIO_DIR_OUT;
                }
                else
                {
                    RT_ERR_HDL(drv_extGpio_reg_read(unit, dev, RTL8231_GPIO_PIN_SEL2_ADDR, &regVal), errHandle, ret);
                    if ((regVal >> ((gpioId % 32) + RTL8231_GPIO_PIN_SEL2_IOMASK_32_OFFSET)) & 0x1)
                        *pData = GPIO_DIR_IN;
                    else
                        *pData = GPIO_DIR_OUT;
                }
            }
            else
            {
                EXTGPIO_SEM_UNLOCK(unit, dev);
                return RT_ERR_FAILED;
            }
            break;
        case EXT_GPIO_ACCESS_MODE_I2C:
        default:
            EXTGPIO_SEM_UNLOCK(unit, dev);
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    EXTGPIO_SEM_UNLOCK(unit, dev);
    return RT_ERR_OK;

  errHandle:
    EXTGPIO_SEM_UNLOCK(unit, dev);
    return ret;

} /* end of drv_extGpio_direction_get */

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
int32
drv_extGpio_direction_set(uint32 unit, uint32 dev, uint32 gpioId, drv_gpio_direction_t data)
{
    uint32  regVal = 0, value;
    int32   ret = RT_ERR_FAILED;

    RT_PARAM_CHK((unit >= RTK_MAX_NUM_OF_UNIT), RT_ERR_INPUT);
    RT_PARAM_CHK((dev >= EXT_GPIO_DEV_ID_END), RT_ERR_INPUT);
    RT_PARAM_CHK(IS_EXTGPIO_UNIT_DEV_INVALID(unit, dev), RT_ERR_INPUT);
    RT_PARAM_CHK((gpioId >= EXT_GPIO_ID_END), RT_ERR_INPUT);

    EXTGPIO_SEM_LOCK(unit, dev);
    switch (extGpioConfEntry[unit][dev].data.access_mode)
    {
        case EXT_GPIO_ACCESS_MODE_MDC:
            /* GPIO Selection */
            RT_ERR_HDL(drv_extGpio_reg_read(unit, dev, RTL8231_GPIO_PIN_SEL_ADDR(gpioId), &regVal), errHandle, ret);
            regVal = (regVal | (1 << (gpioId % 16)));
            RT_ERR_HDL(drv_extGpio_reg_write(unit, dev, RTL8231_GPIO_PIN_SEL_ADDR(gpioId), regVal), errHandle, ret);
            /* Direction */
            if (data == GPIO_DIR_IN)
                value = 1;
            else
                value = 0;

            if (gpioId < EXT_GPIO_ID32)
            {
                RT_ERR_HDL(drv_extGpio_reg_read(unit, dev, RTL8231_GPIO_IO_SEL_ADDR(gpioId), &regVal), errHandle, ret);
                regVal = (regVal & ~(1 << (gpioId % 16))) | (value << (gpioId % 16));
                RT_ERR_HDL(drv_extGpio_reg_write(unit, dev, RTL8231_GPIO_IO_SEL_ADDR(gpioId), regVal), errHandle, ret);
            }
            else
            {
                RT_ERR_HDL(drv_extGpio_reg_read(unit, dev, RTL8231_GPIO_PIN_SEL2_ADDR, &regVal), errHandle, ret);
                regVal = (regVal & ~(1 << ((gpioId % 32) + RTL8231_GPIO_PIN_SEL2_IOMASK_32_OFFSET))) | (value << ((gpioId % 32) + RTL8231_GPIO_PIN_SEL2_IOMASK_32_OFFSET));
                RT_ERR_HDL(drv_extGpio_reg_write(unit, dev, RTL8231_GPIO_PIN_SEL2_ADDR, regVal), errHandle, ret);
            }
            break;
        case EXT_GPIO_ACCESS_MODE_I2C:
        default:
            EXTGPIO_SEM_UNLOCK(unit, dev);
            return RT_ERR_CHIP_NOT_SUPPORTED;
    }

    EXTGPIO_SEM_UNLOCK(unit, dev);
    return RT_ERR_OK;

  errHandle:
    EXTGPIO_SEM_UNLOCK(unit, dev);
    return ret;

} /* end of drv_extGpio_direction_set */

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
int32
drv_extGpio_i2c_init(uint32 unit, uint32 dev, uint32 i2c_clock, uint32 i2c_data)
{
    return RT_ERR_CHIP_NOT_SUPPORTED;
} /* end of drv_extGpio_i2c_init */

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
int32
drv_extGpio_i2c_get(uint32 unit, uint32 dev, uint32 *pI2c_clock, uint32 *pI2c_data)
{
    return RT_ERR_CHIP_NOT_SUPPORTED;
} /* end of drv_extGpio_i2c_get */

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
int32
drv_extGpio_i2c_read(uint32 unit, uint32 dev, uint32 reg, uint32 *pData)
{
    return RT_ERR_CHIP_NOT_SUPPORTED;
} /* end of drv_extGpio_i2c_read */

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
int32
drv_extGpio_i2c_write(uint32 unit, uint32 dev, uint32 reg, uint32 data)
{
    return RT_ERR_CHIP_NOT_SUPPORTED;
} /* end of drv_extGpio_i2c_write */

#if defined(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE) || (defined(CONFIG_SDK_KERNEL_LINUX_USER_MODE) && !defined(__KERNEL__))

/* Function Name:
 *      drv_extGpio_isr_get
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
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
drv_extGpio_isr_get(uint32 unit, uint32 dev, uint32 pinId, uint32 * pData)
{
    int32 ret;

    RT_PARAM_CHK((unit >= RTK_MAX_NUM_OF_UNIT), RT_ERR_INPUT);
    RT_PARAM_CHK((dev >= EXT_GPIO_DEV_ID_END), RT_ERR_INPUT);
    RT_PARAM_CHK(IS_EXTGPIO_UNIT_DEV_INVALID(unit, dev), RT_ERR_INPUT);
    RT_PARAM_CHK((pinId >= EXT_GPIO_ID_END), RT_ERR_INPUT);

    ret = drv_rtl8231_intrStatus_get(unit,  dev, pinId, pData);

    return ret;
} /* end of drv_gpio_isr_get */


/* Function Name:
 *      _ext_gpio_interrupt_handler
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
static int _ext_gpio_interrupt_handler(void)
{
    int32 ret = RT_ERR_OK;
    uint32  gpio_data;
    uint32 unit = HWP_MY_UNIT_ID();

    uint32 dev, gpioId;

    for(dev = EXT_GPIO_DEV_ID0 ; dev < EXT_GPIO_DEV_ID_END ; dev++){
        for(gpioId = EXT_GPIO_ID0 ; gpioId < EXT_GPIO_ID_END ; gpioId++){

            if(extGpiodata[dev][gpioId].action_status == ENABLED){
                ret = drv_extGpio_isr_get(unit, dev, gpioId, &gpio_data);
                extGpioInterruptCb[dev][gpioId](&gpio_data);
                extGpiodata[dev][gpioId].action_status = DISABLED;
            }
        }
    }

    drv_intr_imrEnable_set(unit, INTR_ISR_EXT_GPIO, ENABLED);
    return ret;
}

/* Function Name:
 *      _ext_gpio_intr_thread
 * Description:
 *      EXT GPIO interrupt thread to receive interrupt event.
 * Input:
 *      pArg - Parameter that is provided when thread create.
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
static void *_ext_gpio_intr_thread(void *pArg)
{

    while(1)
    {
        osal_wait_event(drv_extGpioIsr_event);

        _ext_gpio_interrupt_handler();
    }
    return NULL;
}

/* Function Name:
 *      ext_gpio_intr_attach
 * Description:
 *      Connect EXT GPIO interrupt with handle thread
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Applicable:
 *     9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32 ext_gpio_intr_attach(void)
{
    int32 ret;

    /* Check arguments */
    ret = osal_thread_create(EXT_GPIO_INTR_THREAD_NAME, 4096, 0, (void *)_ext_gpio_intr_thread, NULL);
    if (0 == ret)
    {
      RT_ERR(ret, MOD_RTCORE, "EXT GPIO interrupt thread create failed");
      return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
}

/* Function Name:
 *      drv_extGpio_isr_clear
 * Description:
 *      Clear the interrupt status register value of a specified EXT GPIO ID.
 * Input:
 *      unit   - unit id
 *      dev    - external GPIO dev id
 *      gpioId - gpio id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_OUT_OF_RANGE
 *      RT_ERR_CHIP_NOT_SUPPORTED
 * Applicable:
 *      9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
drv_extGpio_isr_clear(uint32 unit, uint32 dev, uint32 pinId)
{
    int32 ret = RT_ERR_OK;

    ret = drv_rtl8231_intrStatus_clear(unit, dev, pinId);

    return ret;
} /* end of drv_extGpio_isr_clear */


/* Function Name:
*      drv_extGpio_isr
* Description:
*      EXT GPIO interrupt handler routine
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
osal_isrret_t drv_extGpio_isr(void *pParam)
{
    int32 ret = RT_ERR_OK;
    uint32 gIsr = 0;
    uint32      unit = HWP_MY_UNIT_ID();
    uint32 dev, gpioId;

    for(dev = EXT_GPIO_DEV_ID0 ; dev < EXT_GPIO_DEV_ID_END ; dev++){
        for(gpioId = EXT_GPIO_ID0 ; gpioId < EXT_GPIO_ID_END ; gpioId++){
            ret = drv_extGpio_isr_get(unit, dev, gpioId, &gIsr);

            if (RT_ERR_OK != ret)
            {
                break;
            }

            if (gIsr != 0)
            {
                if(extGpiodata[dev][gpioId].init_pin == GPIO_FLAG_ON && extGpiodata[dev][gpioId].register_pin == GPIO_FLAG_ON)
                {
                    drv_extGpio_isr_clear(unit, dev, gpioId);

                    extGpiodata[dev][gpioId].action_status = ENABLED;
                }
            }
        }/* end for */
    }/* end for */

    osal_wake_up(drv_extGpioIsr_event);

    return RT_ERR_OK;
}/* drv_extGpio_isr */


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
int32 drv_extGpio_intrHandler_register(uint32 unit, uint32 dev, uint32 gpioId, drv_extGpioIsr_cb_f extGpioIsrCallback)
{

    int32 ret = RT_ERR_FAILED;
    rtk_enable_t regIntrSrc = DISABLED;
    uint32 dev_id, gpio_pin_id;
    drv_extGpio_directAccessMode_t directAccess_Mode = EXT_GPIO_DIRECT_ACCESS_MODE_CONTINUOUS;
    rtk_enable_t directAccess = ENABLED;

    dev_id = dev;
    gpio_pin_id = gpioId;

    RT_PARAM_CHK((unit >= RTK_MAX_NUM_OF_UNIT), RT_ERR_INPUT);
    RT_PARAM_CHK((dev >= EXT_GPIO_DEV_ID_END), RT_ERR_INPUT);
    RT_PARAM_CHK(IS_EXTGPIO_UNIT_DEV_INVALID(unit, dev), RT_ERR_INPUT);
    RT_PARAM_CHK((gpioId >= EXT_GPIO_ID_END), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == extGpioIsrCallback), RT_ERR_NULL_POINTER);

    if(extGpio_threadEnabled == DISABLED){
        ret = osal_wait_module_create(&drv_extGpioIsr_event);
        if(ret != RT_ERR_OK)
            return RT_ERR_FAILED;

        ext_gpio_intr_attach();
        extGpio_threadEnabled = ENABLED;
    }

    for(dev_id = EXT_GPIO_DEV_ID0 ; dev_id < EXT_GPIO_DEV_ID_END ; dev_id++){
        for(gpio_pin_id = EXT_GPIO_ID0 ; gpio_pin_id < EXT_GPIO_ID_END ; gpio_pin_id++){
            if(extGpiodata[dev_id][gpio_pin_id].register_pin == GPIO_FLAG_ON)
                regIntrSrc = ENABLED;
        }
    }

    if(regIntrSrc == DISABLED){
        /* Register EXT_GPIO IRQ */
        RT_ERR_HDL(rt_util_intr_isr_register(INTR_ISR_EXT_GPIO, drv_extGpio_isr, NULL), error, ret);
        drv_rtl8231_directAccess_set(unit, extGpioConfEntry[unit][dev].data.address, directAccess_Mode, directAccess);
    }

    extGpioInterruptCb[dev][gpioId] = extGpioIsrCallback;
    extGpiodata[dev][gpioId].register_pin = GPIO_FLAG_ON;

    return RT_ERR_OK;

    error:
        RT_ERR(RT_ERR_FAILED, MOD_RTCORE, "Register EXT GPIO IRQ and Interrupt Handler Failed!");

    return RT_ERR_FAILED;
}


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
int32 drv_extGpio_intrHandler_unregister(uint32 unit, uint32 dev, uint32 gpioId)
{
    int32 ret = RT_ERR_FAILED;
    rtk_enable_t unRegIntrSrc = DISABLED;
    drv_extGpio_directAccessMode_t directAccess_Mode = EXT_GPIO_DIRECT_ACCESS_MODE_CONTINUOUS;
    rtk_enable_t directAccess = DISABLED;

    RT_PARAM_CHK((unit >= RTK_MAX_NUM_OF_UNIT), RT_ERR_INPUT);
    RT_PARAM_CHK((dev >= EXT_GPIO_DEV_ID_END), RT_ERR_INPUT);
    RT_PARAM_CHK(IS_EXTGPIO_UNIT_DEV_INVALID(unit, dev), RT_ERR_INPUT);
    RT_PARAM_CHK((gpioId >= EXT_GPIO_ID_END), RT_ERR_INPUT);

    extGpiodata[dev][gpioId].register_pin = GPIO_FLAG_OFF;

    for(dev = EXT_GPIO_DEV_ID0 ; dev < EXT_GPIO_DEV_ID_END ; dev++){
        for(gpioId = EXT_GPIO_ID0 ; gpioId < EXT_GPIO_ID_END ; gpioId++){
            if(extGpiodata[dev][gpioId].register_pin == GPIO_FLAG_ON)
                unRegIntrSrc = ENABLED;
        }
    }

    if(unRegIntrSrc == DISABLED)
    {
        RT_ERR_HDL(rt_util_intr_isr_unregister(INTR_ISR_EXT_GPIO), error, ret);
        drv_rtl8231_directAccess_set(unit, dev, directAccess_Mode, directAccess);
    }

    return RT_ERR_OK;


    error:
        RT_ERR(RT_ERR_FAILED, MOD_RTCORE, "Unregister EXT GPIO IRQ and Interrupt Handler Failed!");

    return RT_ERR_FAILED;

}


#endif /* #if defined(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE) || (defined(CONFIG_SDK_KERNEL_LINUX_USER_MODE) && !defined(__KERNEL__)) */



