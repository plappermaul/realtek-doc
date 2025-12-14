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
#include <dal/rtl9601b/dal_rtl9601b.h>
#include <rtk/gpio.h>
#include <dal/rtl9601b/dal_rtl9601b_gpio.h>
#include <ioal/mem32.h>



static uint32    gpio_init = INIT_COMPLETED;

//static int32 dal_rtl9601b_gpio_intrStatus_clean(uint32 gpioId);
//static int32 dal_rtl9601b_gpio_intrStatus_get(uint32 gpioId,rtk_enable_t *pState);

/* Function Name:
 *      dal_rtl9601b_gpio_init
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
dal_rtl9601b_gpio_init(void )
{
	RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPIO),"%s",__FUNCTION__);
	gpio_init = INIT_COMPLETED;

	return RT_ERR_OK;
}


/* Function Name:
 *      dal_rtl9601b_gpio_mode_set
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
dal_rtl9601b_gpio_mode_set(uint32 gpioId, rtk_gpio_mode_t mode)
{

    int32 ret;
	uint32 reg,value,tmp;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPIO), "gpioId=%d,mode=%d",gpioId, mode);

    /* check Init status */
    RT_INIT_CHK(gpio_init);

    /* parameter check */
    RT_PARAM_CHK((RTL9601B_GPIO_PIN_NUM <=gpioId), RT_ERR_INPUT);
    RT_PARAM_CHK((GPIO_MODE_END <=mode), RT_ERR_INPUT);

    /* function body */

	reg = RTL9601B_GPIO_DIR_ABCDr;

	if((ret = ioal_socMem32_read(reg,&value))!= RT_ERR_OK)
	{
	  RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");
	  return ret;
	}

	tmp = 1 << gpioId;
	value = ~tmp & value;
	value = mode << gpioId | value;

	if((ret = ioal_socMem32_write(reg,value))!= RT_ERR_OK)
	{
	  RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");
	  return ret;
	}
    return RT_ERR_OK;
}   /* end of dal_rtl9601b_gpio_mode_set */


/* Function Name:
 *      dal_rtl9601b_gpio_mode_get
 * Description:
 *     get gpio to input or output mode
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
int32
dal_rtl9601b_gpio_mode_get(uint32 gpioId, rtk_gpio_mode_t *mode)
{

    int32 ret;
	uint32 reg,value;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPIO), "gpioId=%d",gpioId);

    /* check Init status */
    RT_INIT_CHK(gpio_init);

    /* parameter check */
    RT_PARAM_CHK((RTL9601B_GPIO_PIN_NUM <=gpioId), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == mode), RT_ERR_NULL_POINTER);

    /* function body */

	reg = RTL9601B_GPIO_DIR_ABCDr;

	if((ret = ioal_socMem32_read(reg,&value))!= RT_ERR_OK)
	{
	  RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");
	  return ret;
	}
	*mode = (value >> gpioId ) & 0x1;

    return RT_ERR_OK;
}   /* end of dal_rtl9601b_gpio_mode_get */




/* Function Name:
 *      dal_rtl9601b_gpio_databit_get
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
dal_rtl9601b_gpio_databit_get(uint32 gpioId, uint32 *data)
{
    int32 ret;
	uint32 reg,value;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPIO), "gpioId=%d",gpioId);

    /* check Init status */
    RT_INIT_CHK(gpio_init);

    /* parameter check */
    RT_PARAM_CHK((RTL9601B_GPIO_PIN_NUM <=gpioId), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == data), RT_ERR_NULL_POINTER);
	reg = RTL9601B_GPIO_DATA_ABCDr;

	if((ret = ioal_socMem32_read(reg,&value))!= RT_ERR_OK)
	{
	  RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");
	  return ret;
	}

	*data = (value >> gpioId) & 0x1;
    return RT_ERR_OK;
}   /* end of dal_rtl9601b_gpio_databit_get */


/* Function Name:
 *      dal_rtl9601b_gpio_databit_set
 * Description:
 *     write data to gpio
 * Input:
 *      gpioId 		-gpio id from 0 to 71
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
dal_rtl9601b_gpio_databit_set(uint32 gpioId, uint32 data)
{

    int32 ret;
	uint32 reg,value,tmp;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPIO), "gpioId=%d",gpioId);

    /* check Init status */
    RT_INIT_CHK(gpio_init);

    /* parameter check */
    RT_PARAM_CHK((RTL9601B_GPIO_PIN_NUM <=gpioId), RT_ERR_INPUT);
    RT_PARAM_CHK((2 <=data), RT_ERR_INPUT);

	reg = RTL9601B_GPIO_DATA_ABCDr;

	if((ret = ioal_socMem32_read(reg,&value))!= RT_ERR_OK)
	{
	  RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");
	  return ret;
	}

	tmp = 1 << gpioId;
	value = ~tmp & value;
	value = data << gpioId | value;

	if((ret = ioal_socMem32_write(reg,value))!= RT_ERR_OK)
	{
	  RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");
	  return ret;
	}

    return RT_ERR_OK;
}   /* end of dal_rtl9601b_gpio_databit_set */



/* Function Name:
 *      dal_rtl9601b_gpio_state_set
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
dal_rtl9601b_gpio_state_set(uint32 gpioId, rtk_enable_t enable)
{

    int32 ret;
	uint32 reg,value,field;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPIO), "gpioId=%d,enable=%d",gpioId, enable);

    /* check Init status */
    RT_INIT_CHK(gpio_init);

    /* parameter check */
    RT_PARAM_CHK((RTL9601B_GPIO_PIN_NUM <= gpioId), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

	if(enable)
	{
		switch(gpioId){
		case RTK_GPIO_0:
		case RTK_GPIO_1:
			reg = RTL9601B_IO_MODE_ENr;
			field = RTL9601B_I2C_MASTER_ENf;
		break;
		case RTK_GPIO_2:
			value = 1;
			if ((ret = reg_field_write(RTL9601B_IO_MODE_ENr,RTL9601B_JTAG_DISf,&value)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
				return ret;
			}
			reg = RTL9601B_IO_LED_ENr;
			field = RTL9601B_LED0_ENf;
		break;
		case RTK_GPIO_3:
			value = 1;
			if ((ret = reg_field_write(RTL9601B_IO_MODE_ENr,RTL9601B_JTAG_DISf,&value)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
				return ret;
			}
			reg = RTL9601B_IO_LED_ENr;
			field = RTL9601B_LED1_ENf;
		break;
		case RTK_GPIO_4:
			value = 1;
			if ((ret = reg_field_write(RTL9601B_IO_MODE_ENr,RTL9601B_JTAG_DISf,&value)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
				return ret;
			}
			reg = RTL9601B_IO_MODE_ENr;
			field = RTL9601B_DIS_JTAGf;
		break;
		case RTK_GPIO_5:
			value = 1;
			if ((ret = reg_field_write(RTL9601B_IO_MODE_ENr,RTL9601B_JTAG_DISf,&value)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
				return ret;
			}
			reg = RTL9601B_IO_LED_ENr;
			field = RTL9601B_LED2_ENf;
		break;
		case RTK_GPIO_6:
			value = 1;
			if ((ret = reg_field_write(RTL9601B_IO_MODE_ENr,RTL9601B_JTAG_DISf,&value)) != RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
				return ret;
			}
			reg = RTL9601B_IO_LED_ENr;
			field = RTL9601B_LED3_ENf;
		break;
		case RTK_GPIO_7:
			reg = RTL9601B_IO_MODE_ENr;
			field = RTL9601B_DIS_TX_ENf;
		break;
		case RTK_GPIO_8:
			reg = RTL9601B_IO_MODE_ENr;
			field = RTL9601B_TX_SD_ENf;
		break;
		case RTK_GPIO_9:
		case RTK_GPIO_10:
			reg = RTL9601B_IO_MODE_ENr;
			field = RTL9601B_UART_ENf;
		break;
		case RTK_GPIO_11:
		case RTK_GPIO_12:
			reg = RTL9601B_IO_MODE_ENr;
			field = RTL9601B_I2C_SLAVE_DISf;

    		value = 1;
    		if ((ret = reg_field_write(reg,field,&value)) != RT_ERR_OK)
    		{
    			RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
    			return ret;
    		}
            return RT_ERR_OK;
		break;
		case RTK_GPIO_13:
			reg = RTL9601B_IO_MODE_ENr;
			field = RTL9601B_VOIP0_ENf;
		break;
		case RTK_GPIO_14:
			reg = RTL9601B_IO_MODE_ENr;
			field = RTL9601B_VOIP1_ENf;
		break;
		case RTK_GPIO_15:
			reg = RTL9601B_IO_MODE_ENr;
			field = RTL9601B_VOIP2_ENf;
		break;
		case RTK_GPIO_16:
			reg = RTL9601B_IO_MODE_ENr;
			field = RTL9601B_VOIP3_ENf;
		break;
		case RTK_GPIO_17:
			reg = RTL9601B_IO_MODE_ENr;
			field = RTL9601B_SPI_RST_ENf;
		break;
		case RTK_GPIO_18:
			reg = RTL9601B_IO_MODE_ENr;
			field = RTL9601B_VOIP4_ENf;
		break;
		case RTK_GPIO_19:
			reg = RTL9601B_IO_MODE_ENr;
			field = RTL9601B_VOIP5_ENf;
		break;
		case RTK_GPIO_20:
			reg = RTL9601B_IO_MODE_ENr;
			field = RTL9601B_VOIP6_ENf;
		break;
		case RTK_GPIO_21:
			reg = RTL9601B_IO_MODE_ENr;
			field = RTL9601B_VOIP7_ENf;
		break;
		case RTK_GPIO_22:
			reg = RTL9601B_IO_MODE_ENr;
			field = RTL9601B_VOIP8_ENf;
		break;
		case RTK_GPIO_23:
			reg = RTL9601B_IO_MODE_ENr;
			field = RTL9601B_TOD_ENf;
		break;
		case RTK_GPIO_24:
			reg = RTL9601B_IO_MODE_ENr;
			field = RTL9601B_GPONPPS_ENf;
		break;
		case RTK_GPIO_25:
		default:
			return RT_ERR_OK;
		break;
		}

		value = 0;
		if ((ret = reg_field_write(reg,field,&value)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
			return ret;
		}
	}
	else
	{
		/*if gpio disable, set to input mode*/
		dal_rtl9601b_gpio_mode_set(gpioId,GPIO_INPUT);
	}
    return RT_ERR_OK;
}   /* end of dal_rtl9601b_gpio_state_set */



/* Function Name:
 *      dal_rtl9601b_gpio_state_get
 * Description:
 *      enable or disable gpio function
 * Input:
 *      gpioId		- gpio id from 0~71
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
dal_rtl9601b_gpio_state_get(uint32 gpioId, rtk_enable_t *enable)
{
    int32 ret;
	uint32 reg,value,field;
    uint32 globalState=1;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPIO), "gpioId=%d",gpioId);

    /* check Init status */
    RT_INIT_CHK(gpio_init);

    /* parameter check */
    RT_PARAM_CHK((RTL9601B_GPIO_PIN_NUM <= gpioId), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == enable), RT_ERR_INPUT);


	switch(gpioId){
	case RTK_GPIO_0:
	case RTK_GPIO_1:
		reg = RTL9601B_IO_MODE_ENr;
		field = RTL9601B_I2C_MASTER_ENf;
	break;
	case RTK_GPIO_2:
		if ((ret = reg_field_read(RTL9601B_IO_MODE_ENr,RTL9601B_JTAG_DISf,&globalState)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
			return ret;
		}
		reg = RTL9601B_IO_LED_ENr;
		field = RTL9601B_LED0_ENf;
	break;
	case RTK_GPIO_3:
		if ((ret = reg_field_read(RTL9601B_IO_MODE_ENr,RTL9601B_JTAG_DISf,&globalState)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
			return ret;
		}
		reg = RTL9601B_IO_LED_ENr;
		field = RTL9601B_LED1_ENf;
	break;
	case RTK_GPIO_4:
		if ((ret = reg_field_read(RTL9601B_IO_MODE_ENr,RTL9601B_JTAG_DISf,&globalState)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
			return ret;
		}
		reg = RTL9601B_IO_MODE_ENr;
		field = RTL9601B_DIS_JTAGf;
	break;
	case RTK_GPIO_5:
		if ((ret = reg_field_read(RTL9601B_IO_MODE_ENr,RTL9601B_JTAG_DISf,&globalState)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
			return ret;
		}
		reg = RTL9601B_IO_LED_ENr;
		field = RTL9601B_LED2_ENf;
	break;
	case RTK_GPIO_6:
		if ((ret = reg_field_read(RTL9601B_IO_MODE_ENr,RTL9601B_JTAG_DISf,&globalState)) != RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
			return ret;
		}
		reg = RTL9601B_IO_LED_ENr;
		field = RTL9601B_LED3_ENf;
	break;
	case RTK_GPIO_7:
		reg = RTL9601B_IO_MODE_ENr;
		field = RTL9601B_DIS_TX_ENf;
	break;
	case RTK_GPIO_8:
		reg = RTL9601B_IO_MODE_ENr;
		field = RTL9601B_TX_SD_ENf;
	break;
	case RTK_GPIO_9:
	case RTK_GPIO_10:
		reg = RTL9601B_IO_MODE_ENr;
		field = RTL9601B_UART_ENf;
	break;
	case RTK_GPIO_11:
	case RTK_GPIO_12:
		reg = RTL9601B_IO_MODE_ENr;
		field = RTL9601B_I2C_SLAVE_DISf;
		
    	if ((ret = reg_field_read(reg,field,&value)) != RT_ERR_OK)
    	{
    		RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
    		return ret;
    	}
    	*enable = value;
		return RT_ERR_OK;
	break;
	case RTK_GPIO_13:
		reg = RTL9601B_IO_MODE_ENr;
		field = RTL9601B_VOIP0_ENf;
	break;
	case RTK_GPIO_14:
		reg = RTL9601B_IO_MODE_ENr;
		field = RTL9601B_VOIP1_ENf;
	break;
	case RTK_GPIO_15:
		reg = RTL9601B_IO_MODE_ENr;
		field = RTL9601B_VOIP2_ENf;
	break;
	case RTK_GPIO_16:
		reg = RTL9601B_IO_MODE_ENr;
		field = RTL9601B_VOIP3_ENf;
	break;
	case RTK_GPIO_17:
		reg = RTL9601B_IO_MODE_ENr;
		field = RTL9601B_SPI_RST_ENf;
	break;
	case RTK_GPIO_18:
		reg = RTL9601B_IO_MODE_ENr;
		field = RTL9601B_VOIP4_ENf;
	break;
	case RTK_GPIO_19:
		reg = RTL9601B_IO_MODE_ENr;
		field = RTL9601B_VOIP5_ENf;
	break;
	case RTK_GPIO_20:
		reg = RTL9601B_IO_MODE_ENr;
		field = RTL9601B_VOIP6_ENf;
	break;
	case RTK_GPIO_21:
		reg = RTL9601B_IO_MODE_ENr;
		field = RTL9601B_VOIP7_ENf;
	break;
	case RTK_GPIO_22:
		reg = RTL9601B_IO_MODE_ENr;
		field = RTL9601B_VOIP8_ENf;
	break;
	case RTK_GPIO_23:
		reg = RTL9601B_IO_MODE_ENr;
		field = RTL9601B_TOD_ENf;
	break;
	case RTK_GPIO_24:
		reg = RTL9601B_IO_MODE_ENr;
		field = RTL9601B_GPONPPS_ENf;
	break;
	case RTK_GPIO_25:
	default:
		*enable = ENABLED;
		return RT_ERR_OK;
	break;
	}

    if(globalState == 0)
    {
        *enable = DISABLED;
        return RT_ERR_OK;
    }

	if ((ret = reg_field_read(reg,field,&value)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_EPON|MOD_DAL), "");
		return ret;
	}
	*enable = !value;
    return RT_ERR_OK;
}   /* end of dal_rtl9601b_gpio_state_get */




/* Function Name:
 *      dal_rtl9601b_gpio_intr_set
 * Description:
 *     write data to gpio
 * Input:
 *      gpioId - gpio id from 0 to 63
 *      intrMode - gpio interrupt mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
int32
dal_rtl9601b_gpio_intr_set(uint32 gpioId,rtk_gpio_intrMode_t intrMode)
{
    int32 ret,i;
    uint32 value=0,reg,tmp,groupId,offset;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPIO), "intrMode=%d",intrMode);

    /* check Init status */
    RT_INIT_CHK(gpio_init);

    /* parameter check */
    RT_PARAM_CHK((GPIO_INTR_END <= intrMode), RT_ERR_INPUT);
    if((gpioId >= RTL9601B_GPIO_PIN_NUM) && 
       (gpioId != RTK_GPIO_INTR_ALL) &&
       (gpioId != RTK_GPIO_INTR_SET1) &&
       (gpioId != RTK_GPIO_INTR_SET2))
    {
        return RT_ERR_INPUT;
    }

	if(RTK_GPIO_INTR_ALL==gpioId)
	{
		for(i=0;i<16;i++)
		{
			value |= ((intrMode & 0x3) << (i*2));
		}
		reg	  = RTL9601B_GPIO_IMR_ABr;
		if((ret = ioal_socMem32_write(reg,value))!= RT_ERR_OK)
		{
		  RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");
		  return ret;
		}

		reg	  = RTL9601B_GPIO_IMR_CDr;
		if((ret = ioal_socMem32_write(reg,value))!= RT_ERR_OK)
		{
		  RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");
		  return ret;
		}

        /* 9601b has another GPIO IMR2 */
        value = (intrMode == GPIO_INTR_DISABLE) ? 0 : 0xffffffff;

        if((ret = ioal_socMem32_write(RTL9601B_GPIO_C0_IER_ABCDr,value))!= RT_ERR_OK)
		{
		  RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");
		  return ret;
		}

	}
	else
	if((gpioId == RTK_GPIO_INTR_SET1) || (gpioId == RTK_GPIO_INTR_SET2))
	{
		for(i=0;i<16;i++)
		{
			value |= ((intrMode & 0x3) << (i*2));
		}
        if(gpioId == RTK_GPIO_INTR_SET1)
            reg	 = RTL9601B_GPIO_IMR_ABr;
        else
            reg	 = RTL9601B_GPIO_IMR_CDr;
		if((ret = ioal_socMem32_write(reg,value))!= RT_ERR_OK)
		{
		  RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");
		  return ret;
		}

        /* 9601b has another GPIO IMR2 */
        if((ret = ioal_socMem32_read(RTL9601B_GPIO_C0_IER_ABCDr,&value))!= RT_ERR_OK)
		{
		  RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");
		  return ret;
		}

        if(gpioId == RTK_GPIO_INTR_SET1)
        {
            tmp = (intrMode == GPIO_INTR_DISABLE) ? 0 : 0xffff;
            value &= 0xffff0000;
        }
        else
        {
            tmp = (intrMode == GPIO_INTR_DISABLE) ? 0 : 0xffff0000;
            value &= 0xffff;
        }

        value |= tmp;

        if((ret = ioal_socMem32_write(RTL9601B_GPIO_C0_IER_ABCDr,value))!= RT_ERR_OK)
		{
		  RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");
		  return ret;
		}

	}else
	{

		groupId = gpioId / 16;
		offset  = gpioId % 16;

		switch(groupId){
		case 0:
		  	reg = RTL9601B_GPIO_IMR_ABr;
		break;
		case 1:
		  	reg = RTL9601B_GPIO_IMR_CDr;
		break;
        default:
            return RT_ERR_INPUT;
		}

		if((ret = ioal_socMem32_read(reg,&value))!= RT_ERR_OK)
		{
		  RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");
		  return ret;
		}

		tmp   = 0x3 << (offset*2);
		value = ~tmp & value;
		value =	((intrMode & 0x3 ) << (offset*2)) | value;

		if((ret = ioal_socMem32_write(reg,value))!= RT_ERR_OK)
		{
		  RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");
		  return ret;
		}

        /* 9601b has another GPIO IMR2 */
        if((ret = ioal_socMem32_read(RTL9601B_GPIO_C0_IER_ABCDr,&value))!= RT_ERR_OK)
		{
		  RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");
		  return ret;
		}

        value &= ~(1<<gpioId);
        tmp = (intrMode == GPIO_INTR_DISABLE) ? 0 : 1;
        value |= (tmp<<gpioId);

        if((ret = ioal_socMem32_write(RTL9601B_GPIO_C0_IER_ABCDr,value))!= RT_ERR_OK)
		{
		  RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");
		  return ret;
		}

	}

    return RT_ERR_OK;
}



/* Function Name:
 *      dal_rtl9601b_gpio_intr_get
 * Description:
 *     write data to gpio
 * Input:
 *      gpioId - gpio pin id from 0~63 can support interrupt
 *      pIntrMode -  point of interrupt for gpio
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
int32
dal_rtl9601b_gpio_intr_get(uint32 gpioId,rtk_gpio_intrMode_t *pIntrMode)
{

	int32 ret;
	uint32 value,reg,groupId,offset;

	/* check Init status */
	RT_INIT_CHK(gpio_init);

	/* parameter check */
	RT_PARAM_CHK((NULL == pIntrMode), RT_ERR_INPUT);
	RT_PARAM_CHK((RTL9601B_GPIO_INTR_NUM <= gpioId), RT_ERR_INPUT);

	groupId = gpioId / 16;
	offset  = gpioId % 16;

	switch(groupId){
	case 0:
	  	reg = RTL9601B_GPIO_IMR_ABr;
	break;
	case 1:
	  	reg = RTL9601B_GPIO_IMR_CDr;
	break;
    default:
        return RT_ERR_INPUT;
	}


	if((ret = ioal_socMem32_read(reg,&value))!= RT_ERR_OK)
	{
	  RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");
	  return ret;
	}

	*pIntrMode = (value >> (offset*2)) & 0x3;

	return RT_ERR_OK;
}


/* Function Name:
 *      dal_rtl9601b_gpio_intrStatus_clean
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
dal_rtl9601b_gpio_intrStatus_clean(uint32 gpioId)
{
	int32 ret;
    uint32 value,reg;

    /* check Init status */
    RT_INIT_CHK(gpio_init);

    /* parameter check */
    if((gpioId >= RTL9601B_GPIO_PIN_NUM) && 
       (gpioId != RTK_GPIO_INTR_ALL) &&
       (gpioId != RTK_GPIO_INTR_SET1) &&
       (gpioId != RTK_GPIO_INTR_SET2))
    {
        return RT_ERR_INPUT;
    }

	if(gpioId == RTK_GPIO_INTR_ALL)
	{
		value = 0;
		reg = RTL9601B_GPIO_IMS_ABCDr;
		if((ret = ioal_socMem32_write(reg,~value))!= RT_ERR_OK)
		{
		  RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");
		  return ret;
		}

	}else
	if(gpioId == RTK_GPIO_INTR_SET1)
	{
		value = 0;
		reg = RTL9601B_GPIO_IMS_ABCDr;
		if((ret = ioal_socMem32_write(reg,~value))!= RT_ERR_OK)
		{
		  RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");
		  return ret;
		}
	}else
	{
		value = 1 << (gpioId%32);
		reg = RTL9601B_GPIO_IMS_ABCDr;
		if((ret = ioal_socMem32_write(reg,value))!= RT_ERR_OK)
		{
		  RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");
		  return ret;
		}
	}

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_rtl9601b_gpio_intrStatus_get
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
dal_rtl9601b_gpio_intrStatus_get(uint32 gpioId,rtk_enable_t *pState)
{
	int32 ret;
    uint32 value,reg;

	/* check Init status */
	RT_INIT_CHK(gpio_init);

	/* parameter check */
	RT_PARAM_CHK((NULL == pState), RT_ERR_INPUT);
    if((gpioId >= RTL9601B_GPIO_PIN_NUM) && 
       (gpioId != RTK_GPIO_INTR_ALL) &&
       (gpioId != RTK_GPIO_INTR_SET1) &&
       (gpioId != RTK_GPIO_INTR_SET2))
    {
        return RT_ERR_INPUT;
    }

	if(gpioId == RTK_GPIO_INTR_SET1)
	{
		reg = RTL9601B_GPIO_IMS_ABCDr;
		if((ret = ioal_socMem32_read(reg,&value))!= RT_ERR_OK)
		{
		  RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");
		  return ret;
		}
		*pState = value;

	}else
	{
		reg = RTL9601B_GPIO_IMS_ABCDr;
		if((ret = ioal_socMem32_read(reg,&value))!= RT_ERR_OK)
		{
		  RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");
		  return ret;
		}
		*pState = value >> (gpioId%32) & 0x1;
	}

    return RT_ERR_OK;

}


