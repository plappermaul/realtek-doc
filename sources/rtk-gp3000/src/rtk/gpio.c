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



/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/gpio.h>
#include <rtk/init.h>
#include <dal/dal_mgmt.h>


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
rtk_gpio_init(void )
{
	int32   ret;

	/* function body */
	if (NULL == RT_MAPPER->gpio_init)
	return RT_ERR_DRIVER_NOT_FOUND;
	RTK_API_LOCK();
	ret = RT_MAPPER->gpio_init();
	RTK_API_UNLOCK();
	return ret;
}



/* Function Name:
 *      rtk_gpio_state_set
 * Description:
 *      set enable or disable gpio function
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
int32 
rtk_gpio_state_set(uint32 gpioId, rtk_enable_t enable)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpio_state_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpio_state_set(gpioId, enable);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpio_state_set */

/* Function Name:
 *      rtk_gpio_state_get
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
int32 
rtk_gpio_state_get(uint32 gpioId, rtk_enable_t *enable)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpio_state_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpio_state_get(gpioId, enable);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpio_state_get */



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
int32 
rtk_gpio_mode_set(uint32 gpioId, rtk_gpio_mode_t mode)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpio_mode_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpio_mode_set(gpioId, mode);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpio_mode_set */


/* Function Name:
 *      rtk_gpio_mode_get
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
int32 
rtk_gpio_mode_get(uint32 gpioId, rtk_gpio_mode_t *mode)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpio_mode_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpio_mode_get(gpioId, mode);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpio_mode_set */


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
int32 
rtk_gpio_databit_get(uint32 gpioId, uint32 *data)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpio_databit_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpio_databit_get(gpioId, data);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpio_databit_get */


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
int32 
rtk_gpio_databit_set(uint32 gpioId, uint32 data)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpio_databit_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpio_databit_set(gpioId, data);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpio_databit_set */


/* Function Name:
 *      rtk_gpio_intr_set
 * Description:
 *     write data to gpio
 * Input:
 *      state: point of state, enable or disable
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
int32 
rtk_gpio_intr_set(uint32 gpioId,rtk_gpio_intrMode_t intrMode)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpio_intr_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpio_intr_set(gpioId,intrMode);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpio_intr_set */


/* Function Name:
 *      rtk_gpio_intr_get
 * Description:
 *     write data to gpio
 * Input:
 *      state: point of state, enable or disable
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
int32 
rtk_gpio_intr_get(uint32 gpioId,rtk_gpio_intrMode_t *pIntrMode)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpio_intr_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpio_intr_get(gpioId,pIntrMode);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpio_intr_get */


/* Function Name:
 *      rtk_gpio_intr_set
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
int32 
rtk_gpio_intrStatus_get(uint32 gpioId,rtk_enable_t *pState)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpio_intrStatus_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpio_intrStatus_get(gpioId,pState);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpio_intr_set */


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
int32 
rtk_gpio_intrStatus_clean(uint32 gpioId)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->gpio_intrStatus_clean)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->gpio_intrStatus_clean(gpioId);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_gpio_intr_get */

