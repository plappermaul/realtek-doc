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
 * Purpose : Definition those internal GPIO command and APIs in the SDK diagnostic shell.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) internal GPIO commands.
 */

/*
 * Include Files
 */
#include <common/debug/rt_log.h>
#include <common/rt_error.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <drv/gpio/gpio.h>
#include <diag_util.h>
#include <diag_om.h>
#include <diag_str.h>
#include <parser/cparser_priv.h>
#include <drv/gpio/generalCtrl_gpio.h>

#ifdef CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE
  #include <rtrpc/rtrpc_diag.h>
#endif

#ifdef CMD_GPIO_SET_DEV_DEV_ID_ACCESS_MDC_EXTRA_ADDRESS_DEV_ADDRESS
/*
* gpio set dev <UINT:dev_id> access ( mdc | extra ) address <UINT:dev_address>
*/
cparser_result_t cparser_cmd_gpio_set_dev_dev_id_access_mdc_extra_address_dev_address(cparser_context_t *context,
    uint32_t *dev_id_ptr,
    uint32_t *dev_address_ptr)

{
#if defined(CONFIG_SDK_DRIVER_GPIO) || defined(CONFIG_SDK_RTL8231)
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    uint32      devId;
    uint32      default_value = 0;
    uint32      direction;
    drv_extGpio_accessMode_t access_mode;
    drv_generalCtrlGpio_devConf_t genCtrl_gpio;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((dev_id_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((dev_address_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);

    direction = GPIO_DIR_OUT;
    if ('m' == TOKEN_CHAR(5, 0))
        access_mode = EXT_GPIO_ACCESS_MODE_MDC;
    else
        return CPARSER_NOT_OK;


    devId = *dev_id_ptr;
    genCtrl_gpio.default_value = default_value;
    genCtrl_gpio.direction = direction;

    genCtrl_gpio.ext_gpio.access_mode = access_mode;
    genCtrl_gpio.ext_gpio.address = *dev_address_ptr;

    DIAG_UTIL_ERR_CHK(drv_generalCtrlGPIO_dev_init(unit, devId, &genCtrl_gpio), ret);

    return CPARSER_OK;
#else
    DIAG_ERR_PRINT(RT_ERR_CHIP_NOT_SUPPORTED);
    return CPARSER_NOT_OK;
#endif /* defined(CONFIG_SDK_DRIVER_GPIO) || defined(CONFIG_SDK_RTL8231) */

}
#endif

#ifdef CMD_GPIO_SET_DEV_DEV_ID_PIN_GPIO_ID_INIT_DIRECTION_IN_OUT_DEFAULT_DEFAULT_VALUE
/*
* gpio set dev <UINT:dev_id> pin <UINT:gpio_id> init direction ( in | out ) default <UINT:default_value>
*/
cparser_result_t cparser_cmd_gpio_set_dev_dev_id_pin_gpio_id_init_direction_in_out_default_default_value(cparser_context_t *context,
    uint32_t *dev_id_ptr,
    uint32_t *gpio_id_ptr,
    uint32_t *default_value_ptr)
{
#if defined(CONFIG_SDK_DRIVER_GPIO) || defined(CONFIG_SDK_RTL8231)
    uint32        unit = 0;
    int32         ret = RT_ERR_FAILED;
    uint32        devId;
    gpioID        gpioId = 0;
    uint32        direction;
    drv_generalCtrlGpio_pinConf_t genCtrl_gpio;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((dev_id_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);

    direction = GPIO_DIR_OUT;
    if ('i' == TOKEN_CHAR(8, 0))
        direction = GPIO_DIR_IN;
    else
        direction = GPIO_DIR_OUT;


    devId = *dev_id_ptr;
    gpioId = *gpio_id_ptr;
    genCtrl_gpio.default_value = *default_value_ptr;
    genCtrl_gpio.direction = direction;
    genCtrl_gpio.int_gpio.function = GPIO_CTRLFUNC_NORMAL;
    genCtrl_gpio.int_gpio.interruptEnable = GPIO_INT_DISABLE;
    genCtrl_gpio.ext_gpio.direction = direction;
    genCtrl_gpio.ext_gpio.debounce = 0;
    genCtrl_gpio.ext_gpio.inverter = 0;

    DIAG_UTIL_ERR_CHK(drv_generalCtrlGPIO_pin_init(unit, devId, gpioId, &genCtrl_gpio), ret);

    return CPARSER_OK;
#else
    DIAG_ERR_PRINT(RT_ERR_CHIP_NOT_SUPPORTED);
    return CPARSER_NOT_OK;
#endif /* defined(CONFIG_SDK_DRIVER_GPIO) || defined(CONFIG_SDK_RTL8231) */
}
#endif

#ifdef CMD_GPIO_SET_DEV_DEV_ID_STATE_DISABLE_ENABLE
/*
* gpio set dev <UINT:dev_id> state ( disable | enable )
*/
cparser_result_t cparser_cmd_gpio_set_dev_dev_id_state_disable_enable(cparser_context_t *context,
    uint32_t *dev_id_ptr)
{
#if defined(CONFIG_SDK_DRIVER_GPIO) || defined(CONFIG_SDK_RTL8231)
    uint32        unit = 0;
    int32         ret = RT_ERR_FAILED;
    uint32        devId;
    rtk_enable_t enabled;


    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((dev_id_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);

    devId = *dev_id_ptr;

    if ('e' == TOKEN_CHAR(5, 0))
        enabled = ENABLED;
    else
        enabled = DISABLED;


    DIAG_UTIL_ERR_CHK(drv_generalCtrlGPIO_devEnable_set(unit, devId, enabled), ret);

    return CPARSER_OK;
#else
    DIAG_ERR_PRINT(RT_ERR_CHIP_NOT_SUPPORTED);
    return CPARSER_NOT_OK;
#endif /* defined(CONFIG_SDK_DRIVER_GPIO) || defined(CONFIG_SDK_RTL8231) */
}
#endif


#ifdef CMD_GPIO_SET_DEV_DEV_ID_PIN_GPIO_ID_DATA
/*
* gpio set dev <UINT:dev_id> pin <UINT:gpio_id> <UINT:data>
*/
cparser_result_t cparser_cmd_gpio_set_dev_dev_id_pin_gpio_id_data(cparser_context_t *context,
    uint32_t *dev_id_ptr,
    uint32_t *gpio_id_ptr,
    uint32_t *data_ptr)
{
#if defined(CONFIG_SDK_DRIVER_GPIO) || defined(CONFIG_SDK_RTL8231)
    uint32      unit = 0;
    int32       ret = RT_ERR_FAILED;
    uint32      devId;
    gpioID      gpioId;
    uint32      data_value;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((dev_id_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((gpio_id_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((data_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);

    devId = *dev_id_ptr;
    gpioId = *gpio_id_ptr;
    data_value = *data_ptr;

    DIAG_UTIL_ERR_CHK(drv_generalCtrlGPIO_dataBit_set(unit, devId, gpioId, data_value), ret);

    return CPARSER_OK;
#else
    DIAG_ERR_PRINT(RT_ERR_CHIP_NOT_SUPPORTED);
    return CPARSER_NOT_OK;
#endif /* defined(CONFIG_SDK_DRIVER_GPIO) || defined(CONFIG_SDK_RTL8231) */
}
#endif

#ifdef CMD_GPIO_GET_DEV_DEV_ID_PIN_GPIO_ID
/*
* gpio get dev <UINT:dev_id> pin <UINT:gpio_id>
*/
cparser_result_t cparser_cmd_gpio_get_dev_dev_id_pin_gpio_id(cparser_context_t *context,
    uint32_t *dev_id_ptr,
    uint32_t *gpio_id_ptr)
{
#if defined(CONFIG_SDK_DRIVER_GPIO) || defined(CONFIG_SDK_RTL8231)
    uint32        unit = 0;
    uint32        devId;
    gpioID        gpioId;
    uint32        data_value;

    DIAG_OM_GET_UNIT_ID(unit);

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((dev_id_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_PARAM_RANGE_CHK((gpio_id_ptr == NULL), CPARSER_ERR_INVALID_PARAMS);

    devId = *dev_id_ptr;
    gpioId = *gpio_id_ptr;

    drv_generalCtrlGPIO_dataBit_get(unit, devId, gpioId, &data_value);

    diag_util_printf("GPIO data = %u\n",data_value);
    diag_util_mprintf("\n");

    return CPARSER_OK;
#else
    DIAG_ERR_PRINT(RT_ERR_CHIP_NOT_SUPPORTED);
    return CPARSER_NOT_OK;
#endif /* defined(CONFIG_SDK_DRIVER_GPIO) || defined(CONFIG_SDK_RTL8231) */
}
#endif

