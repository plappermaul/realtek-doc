/* Copyright (C) 2013 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 63932 $
 * $Date: 2015-12-08 14:06:29 +0800 (周二, 08 十二月 2015) $
 *
 * Purpose : RTK switch high-level API for RTL8367/RTL8367C
 * Feature : Here is a list of all functions and variables in i2c module.
 *
 */

#include <rtk_switch.h>
#include <rtk_error.h>
#include <port.h>
#include <linux/string.h>
#include <chip.h>
#include <rtk_types.h>
#include <i2c.h>
#include <dal/dal_mgmt.h>


/* Function Name:
 *      rtk_i2c_init
 * Description:
 *      I2C smart function initialization.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      This API is used to initialize EEE status.
 *      need used GPIO pins
 *      OpenDrain and clock
 */
rtk_api_ret_t rtk_i2c_init(void)
{
    rtk_api_ret_t retVal;

    if (NULL == RT_MAPPER->i2c_init)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    retVal = RT_MAPPER->i2c_init();
    RTK_API_UNLOCK();

    return retVal;
}

/* Function Name:
 *      rtk_i2c_mode_set
 * Description:
 *      Set I2C data byte-order.
 * Input:
 *      i2cmode - byte-order mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - Invalid input parameter.
 * Note:
 *      This API can set I2c traffic's byte-order .
 */
rtk_api_ret_t rtk_i2c_mode_set( rtk_I2C_16bit_mode_t i2cmode )
{
    rtk_api_ret_t retVal;

    if (NULL == RT_MAPPER->i2c_mode_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    retVal = RT_MAPPER->i2c_mode_set(i2cmode);
    RTK_API_UNLOCK();

    return retVal;
}

/* Function Name:
 *      rtk_i2c_mode_get
 * Description:
 *      Get i2c traffic byte-order setting.
 * Input:
 *      None
 * Output:
 *      pI2cMode - i2c byte-order
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_NULL_POINTER     - input parameter is null pointer
 * Note:
 *      The API can get i2c traffic byte-order setting.
 */
rtk_api_ret_t rtk_i2c_mode_get( rtk_I2C_16bit_mode_t * pI2cMode)
{
    rtk_api_ret_t retVal;

    if (NULL == RT_MAPPER->i2c_mode_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    retVal = RT_MAPPER->i2c_mode_get(pI2cMode);
    RTK_API_UNLOCK();

    return retVal;
}

/* Function Name:
 *      rtk_i2c_gpioPinGroup_set
 * Description:
 *      Set i2c SDA & SCL used GPIO pins group.
 * Input:
 *      pins_group - GPIO pins group
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_INPUT        - Invalid input parameter.
 * Note:
 *      The API can set i2c used gpio pins group.
 *      There are three group pins could be used
 */
rtk_api_ret_t rtk_i2c_gpioPinGroup_set( rtk_I2C_gpio_pin_t pins_group )
{
    rtk_api_ret_t retVal;

    if (NULL == RT_MAPPER->i2c_gpioPinGroup_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    retVal = RT_MAPPER->i2c_gpioPinGroup_set(pins_group);
    RTK_API_UNLOCK();

    return retVal;
}

/* Function Name:
 *      rtk_i2c_gpioPinGroup_get
 * Description:
 *      Get i2c SDA & SCL used GPIO pins group.
 * Input:
 *      None
 * Output:
 *      pPins_group - GPIO pins group
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_NULL_POINTER     - input parameter is null pointer
 * Note:
 *      The API can get i2c used gpio pins group.
 *      There are three group pins could be used
 */
rtk_api_ret_t rtk_i2c_gpioPinGroup_get( rtk_I2C_gpio_pin_t * pPins_group )
{
    rtk_api_ret_t retVal;

    if (NULL == RT_MAPPER->i2c_gpioPinGroup_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    retVal = RT_MAPPER->i2c_gpioPinGroup_get(pPins_group);
    RTK_API_UNLOCK();

    return retVal;
}

/* Function Name:
 *      rtk_i2c_data_read
 * Description:
 *      read i2c slave device register.
 * Input:
 *      deviceAddr   -   access Slave device address
 *      slaveRegAddr -   access Slave register address
 * Output:
 *      pRegData     -   read data
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_NULL_POINTER     - input parameter is null pointer
 * Note:
 *      The API can access i2c slave and read i2c slave device register.
 */
rtk_api_ret_t rtk_i2c_data_read(rtk_uint8 deviceAddr, rtk_uint32 slaveRegAddr, rtk_uint32 *pRegData)
{
    rtk_api_ret_t retVal;

    if (NULL == RT_MAPPER->i2c_data_read)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    retVal = RT_MAPPER->i2c_data_read(deviceAddr, slaveRegAddr, pRegData);
    RTK_API_UNLOCK();

    return retVal;
}

/* Function Name:
 *      rtk_i2c_data_write
 * Description:
 *      write data to i2c slave device register
 * Input:
 *      deviceAddr   -   access Slave device address
 *      slaveRegAddr -   access Slave register address
 *      regData      -   data to set
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 * Note:
 *      The API can access i2c slave and setting i2c slave device register.
 */
rtk_api_ret_t rtk_i2c_data_write(rtk_uint8 deviceAddr, rtk_uint32 slaveRegAddr, rtk_uint32 regData)
{
    rtk_api_ret_t retVal;

    if (NULL == RT_MAPPER->i2c_data_write)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    retVal = RT_MAPPER->i2c_data_write(deviceAddr, slaveRegAddr, regData);
    RTK_API_UNLOCK();

    return retVal;
}



