/*
 * Copyright (C) 2013 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * Purpose : RTL8367/RTL8367C switch high-level API
 *
 * Feature : The file includes I2C module high-layer API defination
 *
 */


#ifndef __DAL_RTL8367C_I2C_H__
#define __DAL_RTL8367C_I2C_H__

#include <rtk_types.h>
#include <../../i2c.h>

#define I2C_GPIO_MAX_GROUP (3)

/* Function Name:
 *      dal_rtl8367c_i2c_data_read
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
extern rtk_api_ret_t dal_rtl8367c_i2c_data_read(rtk_uint8 deviceAddr, rtk_uint32 slaveRegAddr, rtk_uint32 *pRegData);

/* Function Name:
 *      dal_rtl8367c_i2c_data_write
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
extern rtk_api_ret_t dal_rtl8367c_i2c_data_write(rtk_uint8 deviceAddr, rtk_uint32 slaveRegAddr, rtk_uint32 regData);


/* Function Name:
 *      dal_rtl8367c_i2c_init
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
extern rtk_api_ret_t dal_rtl8367c_i2c_init(void);

/* Function Name:
 *      dal_rtl8367c_i2c_mode_set
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
extern rtk_api_ret_t dal_rtl8367c_i2c_mode_set( rtk_I2C_16bit_mode_t i2cmode);

/* Function Name:
 *      dal_rtl8367c_i2c_mode_get
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
extern rtk_api_ret_t dal_rtl8367c_i2c_mode_get( rtk_I2C_16bit_mode_t * pI2cMode);


/* Function Name:
 *      dal_rtl8367c_i2c_gpioPinGroup_set
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
extern rtk_api_ret_t dal_rtl8367c_i2c_gpioPinGroup_set( rtk_I2C_gpio_pin_t pins_group);

/* Function Name:
 *      dal_rtl8367c_i2c_gpioPinGroup_get
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
extern rtk_api_ret_t dal_rtl8367c_i2c_gpioPinGroup_get(rtk_I2C_gpio_pin_t * pPins_group);







#endif /* end of __DAL_RTL8367C_I2C_H__ */

