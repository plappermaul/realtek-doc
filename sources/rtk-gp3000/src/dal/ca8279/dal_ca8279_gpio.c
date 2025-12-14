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



/*
 * Include Files
 */
#include <dal/ca8279/dal_ca8279.h>
#include <rtk/gpio.h>
#include <dal/ca8279/dal_ca8279_gpio.h>
#include <aal_pon.h>
#include <soc/cortina/registers.h>

static uint32 gpio_init = INIT_COMPLETED;

/* Function Name:
 *      dal_ca8279_gpio_init
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
int32
dal_ca8279_gpio_init ( void )
{
    RT_DBG(LOG_DEBUG, (MOD_DAL | MOD_GPIO), "%s", __FUNCTION__);
    gpio_init = INIT_COMPLETED;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_ca8279_gpio_state_set
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
int32
dal_ca8279_gpio_state_set ( uint32 gpioId, rtk_enable_t enable )
{
    int32 ret;
    uint32 value,tmp;

    RT_DBG(LOG_DEBUG, (MOD_DAL | MOD_GPIO), "gpioId=%d,enable=%d", gpioId, enable);

    /* check Init status */
    RT_INIT_CHK(gpio_init);

    /* parameter check */
    RT_PARAM_CHK((CA8279_GPIO_PIN_NUM <= gpioId), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    /* function body */
    tmp = 1<<gpioId;
    value = AAL_GLB_REG_READ(0x0058);
    if(enable == TRUE)
        value |= tmp;
    else
        value &= ~tmp;
    AAL_GLB_REG_WRITE(value,0x0058);

    return RT_ERR_OK;
}   /* end of dal_ca8279_gpio_state_set */

/* Function Name:
 *      dal_ca8279_gpio_state_get
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
int32
dal_ca8279_gpio_state_get ( uint32 gpioId, rtk_enable_t* enable )
{
    int32 ret;
    uint32 value;

    RT_DBG(LOG_DEBUG, (MOD_DAL | MOD_GPIO), "gpioId=%d,enable=%d", gpioId, enable);

    /* check Init status */
    RT_INIT_CHK(gpio_init);

    /* parameter check */
    RT_PARAM_CHK((CA8279_GPIO_PIN_NUM <= gpioId), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == enable), RT_ERR_INPUT);

    /* function body */
    value = AAL_GLB_REG_READ(0x0058);
    if(value&1<<gpioId)
        *enable = TRUE;
    else
        *enable = FALSE;

    return RT_ERR_OK;
}   /* end of dal_ca8279_gpio_state_get */

/* Function Name:
 *      dal_ca8279_gpio_mode_set
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
int32
dal_ca8279_gpio_mode_set ( uint32 gpioId, rtk_gpio_mode_t mode )
{
    int32 ret;
    uint32 value,tmp;

    RT_DBG(LOG_DEBUG, (MOD_DAL | MOD_GPIO), "gpioId=%d,mode=%d", gpioId, mode);

    /* check Init status */
    RT_INIT_CHK(gpio_init);

    /* parameter check */
    RT_PARAM_CHK((CA8279_GPIO_PIN_NUM <= gpioId), RT_ERR_INPUT);
    RT_PARAM_CHK((GPIO_MODE_END <= mode), RT_ERR_INPUT);

    /* function body */
    tmp = 1<<gpioId;
    value = AAL_PON_PERI_REG_READ(PER_GPIO0_CFG);
    if(mode == GPIO_INPUT)
        value |= tmp;
    else
        value &= ~tmp;
    AAL_PON_PERI_REG_WRITE(value,PER_GPIO0_CFG);

    return RT_ERR_OK;
}   /* end of dal_ca8279_gpio_mode_set */


/* Function Name:
 *      dal_ca8279_gpio_mode_get
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
int32
dal_ca8279_gpio_mode_get ( uint32 gpioId, rtk_gpio_mode_t* mode )
{
    int32 ret;
    uint32 reg, value, groupId, offset;

    RT_DBG(LOG_DEBUG, (MOD_DAL | MOD_GPIO), "gpioId=%d,mode=%d", gpioId, mode);

    /* check Init status */
    RT_INIT_CHK(gpio_init);

    /* parameter check */
    RT_PARAM_CHK((CA8279_GPIO_PIN_NUM <= gpioId), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == mode), RT_ERR_NULL_POINTER);

    /* function body */
    value = AAL_PON_PERI_REG_READ(PER_GPIO0_CFG);
    if(value&1<<gpioId)
        *mode = GPIO_INPUT;
    else
        *mode = GPIO_OUTPUT;
    
    return RT_ERR_OK;
}   /* end of dal_ca8279_gpio_mode_get */

/* Function Name:
 *      dal_ca8279_gpio_databit_get
 * Description:
 *      read gpio data
 * Input:
 *      gpioId 		-gpio id from 0 to 95 
 *	    data		-point for read data from gpio
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
int32
dal_ca8279_gpio_databit_get ( uint32 gpioId, uint32* data )
{
    int32 ret;
    uint32 value;
    rtk_gpio_mode_t mode;

    RT_DBG(LOG_DEBUG, (MOD_DAL | MOD_GPIO), "gpioId=%d", gpioId);

    /* check Init status */
    RT_INIT_CHK(gpio_init);

    /* parameter check */
    RT_PARAM_CHK((CA8279_GPIO_PIN_NUM <= gpioId), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == data), RT_ERR_NULL_POINTER);

    /* function body */
    if((ret = dal_ca8279_gpio_mode_get(gpioId,&mode))!=CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL | MOD_GPIO), "");
        return RT_ERR_FAILED;
    }

    if(mode == GPIO_INPUT)
    {
        value = AAL_PON_PERI_REG_READ(PER_GPIO0_IN);
        if(value&1<<gpioId)
            *data = 1;
        else
            *data = 0;
    }
    else
    {
        value = AAL_PON_PERI_REG_READ(PER_GPIO0_OUT);
        if(value&1<<gpioId)
            *data = 1;
        else
            *data = 0;
    }

    return RT_ERR_OK;
}   /* end of dal_ca8279_gpio_databit_get */

/* Function Name:
 *      dal_ca8279_gpio_databit_set
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
int32
dal_ca8279_gpio_databit_set ( uint32 gpioId, uint32 data )
{
    int32 ret;
    uint32 value,tmp;
    rtk_gpio_mode_t mode;

    RT_DBG(LOG_DEBUG, (MOD_DAL | MOD_GPIO), "gpioId=%d", gpioId);

    /* check Init status */
    RT_INIT_CHK(gpio_init);

    /* parameter check */
    RT_PARAM_CHK((CA8279_GPIO_PIN_NUM <= gpioId), RT_ERR_INPUT);
    RT_PARAM_CHK((2 <= data), RT_ERR_INPUT);

    /* function body */
    if((ret = dal_ca8279_gpio_mode_get(gpioId,&mode))!=CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL | MOD_GPIO), "");
        return RT_ERR_FAILED;
    }

    tmp = 1<<gpioId;

    if(mode == GPIO_INPUT)
    {
        value = AAL_PON_PERI_REG_READ(PER_GPIO0_IN);
        if(data == 1)
            value |= tmp;
        else
            value &= ~tmp;
        AAL_PON_PERI_REG_WRITE(value,PER_GPIO0_IN);
    }
    else
    {
        value = AAL_PON_PERI_REG_READ(PER_GPIO0_OUT);
        if(data == 1)
            value |= tmp;
        else
            value &= ~tmp;
        AAL_PON_PERI_REG_WRITE(value,PER_GPIO0_OUT);
    }

    return RT_ERR_OK;
}   /* end of dal_ca8279_gpio_databit_set */
