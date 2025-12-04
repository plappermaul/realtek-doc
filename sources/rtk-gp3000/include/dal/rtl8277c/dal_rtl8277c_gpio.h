
/*
 * Copyright (C) 2012 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 45450 $
 * $Date: 2017-04-17 14:08:31 +0800 (Mon, 17 APR 2017) $
 *
 * Purpose : Definition of GPIO API
 *
 * Feature : Provide the APIs to enable and configure GPIO
 *
 */

#ifndef __DAL_RTL8277C_GPIO_H__
#define __DAL_RTL8277C_GPIO_H__

/*
 * Include Files
 */
#include <dal/rtl8277c/dal_rtl8277c.h>
#include <rtk/gpio.h>
#include <dal/rtl8277c/dal_rtl8277c_gpio.h>
#include <ioal/mem32.h>


#define RTL8277C_GPIO_PIN_NUM 160

/* Function Name:
 *      dal_rtl8277c_gpio_init
 * Description:
 *      gpio init function
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *
 */
extern int32
dal_rtl8277c_gpio_init ( void );

/* Function Name:
 *      dal_rtl8277c_gpio_ioremap_init
 * Description:
 *      gpio ioremap init function
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *
 */
extern int32
dal_rtl8277c_gpio_ioremap_init ( void );

/* Function Name:
 *      dal_rtl8277c_gpio_state_set
 * Description:
 *      enable or disable gpio function
 * Input:
 *      gpioId		- gpio id from 0~95
 *      enable		- enable or disable
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
extern int32
dal_rtl8277c_gpio_state_set ( uint32 gpioId, rtk_enable_t enable );

/* Function Name:
 *      dal_rtl8277c_gpio_state_get
 * Description:
 *      enable or disable gpio function
 * Input:
 *      gpioId		- gpio id from 0~95
 *      enable		- point for get gpio enable or disable
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
extern int32
dal_rtl8277c_gpio_state_get ( uint32 gpioId, rtk_enable_t* enable );

/* Function Name:
 *      dal_rtl8277c_gpio_mode_set
 * Description:
 *     set gpio to input or output mode
 * Input:
 *      gpioId 		-gpio id from 0 to 95 
 *	  mode		-gpio mode, input or output mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
extern int32
dal_rtl8277c_gpio_mode_set ( uint32 gpioId, rtk_gpio_mode_t mode );

/* Function Name:
 *      dal_rtl8277c_gpio_mode_get
 * Description:
 *     get gpio to input or output mode
 * Input:
 *      gpioId 		-gpio id from 0 to 95 
 *	  mode		-point for get gpio mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
extern int32
dal_rtl8277c_gpio_mode_get ( uint32 gpioId, rtk_gpio_mode_t* mode );

/* Function Name:
 *      dal_rtl8277c_gpio_databit_get
 * Description:
 *     read gpio data
 * Input:
 *      gpioId 		-gpio id from 0 to 95 
 *	  data		-point for read data from gpio
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
extern int32
dal_rtl8277c_gpio_databit_get ( uint32 gpioId, uint32* data );


/* Function Name:
 *      dal_rtl8277c_gpio_databit_set
 * Description:
 *     write data to gpio
 * Input:
 *      gpioId 		-gpio id from 0 to 95 
 *	  data		-write data to gpio
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
extern int32
dal_rtl8277c_gpio_databit_set ( uint32 gpioId, uint32 data );

#endif /* __DAL_RTL8277C_GPIO_H__ */