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
 * $Revision$
 * $Date$
 *
 * Purpose : Definition of GPIO API
 *
 * Feature : Provide the APIs to enable and configure GPIO
 *
 */

#ifndef __RTK_GPIO_H__
#define __RTK_GPIO_H__

#include <common/rt_type.h>

/*
 * Include Files
 */

/* Take some large value for special use */
enum {
    RTK_GPIO_INTR_ALL = 1024,

    /*for internal usage only*/
    RTK_GPIO_INTR_SET1 = 1025,
    RTK_GPIO_INTR_SET2 = 1026,
    RTK_GPIO_INTR_SET3 = 1027,
};

typedef enum rtk_gpio_mode_e {
	GPIO_INPUT=0,
	GPIO_OUTPUT,
	GPIO_MODE_END
} rtk_gpio_mode_t;


typedef enum rtk_gpio_intrMode_e {
	
	GPIO_INTR_DISABLE=0,
	GPIO_INTR_ENABLE_FALLING_EDGE,
	GPIO_INTR_ENABLE_RISING_EDGE,
	GPIO_INTR_ENABLE_BOTH_EDGE,
	GPIO_INTR_END,

}rtk_gpio_intrMode_t;

/* Function Name:
 *      rtk_gpio_init
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
rtk_gpio_init(void );



/* Function Name:
 *      rtk_gpio_state_set
 * Description:
 *      enable or disable gpio function
 * Input:
 *      gpioId		- gpio id from 0~71
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
rtk_gpio_state_set(uint32 gpioId, rtk_enable_t enable);


/* Function Name:
 *      rtk_gpio_state_get
 * Description:
 *      enable or disable gpio function
 * Input:
 *      gpioId		- gpio id from 0~71
 *      enable		- point for get enable or disable 
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
extern int32 
rtk_gpio_state_get(uint32 gpioId, rtk_enable_t *enable);


/* Function Name:
 *      rtk_gpio_mode_set
 * Description:
 *     set gpio to input or output mode
 * Input:
 *      gpioId 		-gpio id from 0 to 71
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
rtk_gpio_mode_set(uint32 gpioId, rtk_gpio_mode_t mode);


/* Function Name:
 *      rtk_gpio_mode_get
 * Description:
 *     set gpio to input or output mode
 * Input:
 *      gpioId 		-gpio id from 0 to 71
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
rtk_gpio_mode_get(uint32 gpioId, rtk_gpio_mode_t *mode);


/* Function Name:
 *      rtk_gpio_databit_get
 * Description:
 *     read gpio data
 * Input:
 *      gpioId 		-gpio id from 0 to 71
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
rtk_gpio_databit_get(uint32 gpioId, uint32 *data);


/* Function Name:
 *      rtk_gpio_databit_set
 * Description:
 *     write data to gpio
 * Input:
 *      gpioId 		-gpio id from 0 to 71
 *	  data		-point for write data to gpio	
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
extern int32 
rtk_gpio_databit_set(uint32 gpioId, uint32 data);


/* Function Name:
 *      rtk_gpio_intr_set
 * Description:
 *     write data to gpio
 * Input:
 *	  gpioId - gpio id for interrupt 
 *      intrMode - gpio interrupt mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
extern int32 
rtk_gpio_intr_set(uint32 gpioId,rtk_gpio_intrMode_t intrMode);


/* Function Name:
 *      rtk_gpio_intr_get
 * Description:
 *     write data to gpio
 * Input:
 *	  gpioId - gpio id for interrupt 
 *      pIntrMode - point if gpio interrupt mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
extern int32 
rtk_gpio_intr_get(uint32 gpioId,rtk_gpio_intrMode_t *pIntrMode);


/* Function Name:
 *      rtk_gpio_intrStatus_clean
 * Description:
 *     clean gpio interrupt status
 * Input:
 *      gpioId - gpio pin id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
extern int32 
rtk_gpio_intrStatus_clean(uint32 gpioId);


/* Function Name:
 *      rtk_gpio_intrStatus_get
 * Description:
 *     Get gpio interrupt status value
 * Input:
 *      pState - point for gpio interrupt status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
extern int32 
rtk_gpio_intrStatus_get(uint32 gpioId,rtk_enable_t *pState);



#endif  /* __RTK_GPIO_H__ */
