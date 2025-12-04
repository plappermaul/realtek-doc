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
#include <dal/apollomp/dal_apollomp.h>
#include <rtk/gpio.h>
#include <dal/apollomp/dal_apollomp_gpio.h>
#include <ioal/mem32.h>


static uint32    gpio_init = INIT_COMPLETED;

/* Function Name:
 *      dal_apollomp_gpio_init
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
dal_apollomp_gpio_init(void )
{
	RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPIO),"%s",__FUNCTION__);
	gpio_init = INIT_COMPLETED;		

	return RT_ERR_OK;
}



/* Function Name:
 *      dal_apollomp_gpio_state_set
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
dal_apollomp_gpio_state_set(uint32 gpioId, rtk_enable_t enable)
{
    int32 ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPIO), "gpioId=%d,enable=%d",gpioId, enable);

    /* check Init status */
    RT_INIT_CHK(gpio_init);

    /* parameter check */
    RT_PARAM_CHK((APOLLOMP_GPIO_PIN_NUM  <= gpioId), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

#if 1

    /* function body */
   if ((ret=reg_array_field_write(APOLLOMP_GPIO_CTRL_2r, REG_ARRAY_INDEX_NONE, gpioId, APOLLOMP_EN_GPIOf,&enable)) != RT_ERR_OK)
   {
   	RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");	
	return ret;
   }
#else
    /* function body */
    if ((ret=reg_array_field_write(APOLLOMP_GPIO_CTRL_2r, REG_ARRAY_INDEX_NONE, gpioId, APOLLOMP_EN_GPIOf,&enable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");	
        return ret;
    }

    groupId = gpioId / 32;
    offset  = gpioId % 32;

    switch(groupId){
        case 0:
            reg = APOLLOMP_GPIO_CTRL_ABCDr;
        break;
        case 1:
            reg = APOLLOMP_GPIO_CTRL_EFGHr;
        break;
        default:
            return RT_ERR_INPUT;
    }

    if((ret = ioal_socMem32_read(reg,&value))!= RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");		
        return ret;
    }

    tmp = 1 << offset;
    value = ~tmp & value;
    value = enable << offset | value;

    if((ret = ioal_socMem32_write(reg,value))!= RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");		
        return ret;
    }
	
#endif
    return RT_ERR_OK;
}   /* end of dal_apollomp_gpio_state_set */



/* Function Name:
 *      dal_apollomp_gpio_state_get
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
dal_apollomp_gpio_state_get(uint32 gpioId, rtk_enable_t *enable)
{
    int32 ret;

    uint32 value;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPIO), "gpioId=%d",gpioId);

    /* check Init status */
    RT_INIT_CHK(gpio_init);

    /* parameter check */
    RT_PARAM_CHK((APOLLOMP_GPIO_PIN_NUM  <= gpioId), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == enable), RT_ERR_INPUT);

#if 1
    /* function body */
   if ((ret=reg_array_field_read(APOLLOMP_GPIO_CTRL_2r, REG_ARRAY_INDEX_NONE, gpioId, APOLLOMP_EN_GPIOf,&value)) != RT_ERR_OK)
   {
   	RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");	
	return ret;
   }
    *enable = value;
#else
	groupId = gpioId / 32;
	offset  = gpioId % 32;

	switch(groupId){
    	case 0:
    		reg = APOLLOMP_GPIO_CTRL_ABCDr;
    	break;
    	case 1:
    		reg = APOLLOMP_GPIO_CTRL_EFGHr;
    	break;
        default:
            return RT_ERR_INPUT;

	}

	if((ret = ioal_socMem32_read(reg,&value))!= RT_ERR_OK)
	{
	  RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");		
	  return ret;
	}
	*enable = (value >> offset) & 0x1;
#endif
    return RT_ERR_OK;
}   /* end of dal_apollomp_gpio_state_get */


/* Function Name:
 *      dal_apollomp_gpio_mode_set
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
dal_apollomp_gpio_mode_set(uint32 gpioId, rtk_gpio_mode_t mode)
{

    int32 ret;	
	uint32 reg,value,groupId,offset,tmp;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPIO), "gpioId=%d,mode=%d",gpioId, mode);

    /* check Init status */
    RT_INIT_CHK(gpio_init);

    /* parameter check */
    RT_PARAM_CHK((APOLLOMP_GPIO_PIN_NUM  <=gpioId), RT_ERR_INPUT);
    RT_PARAM_CHK((GPIO_MODE_END <=mode), RT_ERR_INPUT);

    /* function body */
#if 0
   if ((ret=reg_array_field_write(APOLLOMP_GPIO_CTRL_4r, REG_ARRAY_INDEX_NONE, gpioId, APOLLOMP_SEL_GPIOf,&mode)) != RT_ERR_OK)
   {
   	RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");	
	return ret;
   }
#else
	groupId = gpioId / 32;
	offset	= gpioId % 32;

	switch(groupId){
    	case 0:
    		reg = APOLLOMP_GPIO_DIR_ABCDr;
    	break;
    	case 1:
    		reg = APOLLOMP_GPIO_DIR_EFGHr;
    	break;
        default:
            return RT_ERR_INPUT;

	}

	if((ret = ioal_socMem32_read(reg,&value))!= RT_ERR_OK)
	{
	  RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");		
	  return ret;
	}

	tmp = 1 << offset;
	value = ~tmp & value;
	value = mode << offset | value;

	if((ret = ioal_socMem32_write(reg,value))!= RT_ERR_OK)
	{
	  RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");		
	  return ret;
	}
#endif
    return RT_ERR_OK;
}   /* end of dal_apollomp_gpio_mode_set */


/* Function Name:
 *      dal_apollomp_gpio_mode_get
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
dal_apollomp_gpio_mode_get(uint32 gpioId, rtk_gpio_mode_t *mode)
{

    int32 ret;
	uint32 reg,value,groupId,offset;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPIO), "gpioId=%d,mode=%d",gpioId, (int32)mode);

    /* check Init status */
    RT_INIT_CHK(gpio_init);

    /* parameter check */
    RT_PARAM_CHK((APOLLOMP_GPIO_PIN_NUM  <=gpioId), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == mode), RT_ERR_NULL_POINTER);

    /* function body */
#if 0
   if ((ret=reg_array_field_read(APOLLOMP_GPIO_CTRL_4r, REG_ARRAY_INDEX_NONE, gpioId, APOLLOMP_SEL_GPIOf,&value)) != RT_ERR_OK)
   {
   	RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");	
	return ret;
   }

   *mode = value;
#else
	groupId = gpioId / 32;
	offset	= gpioId % 32;

	switch(groupId){
    	case 0:
    		reg = APOLLOMP_GPIO_DIR_ABCDr;
    	break;
    	case 1:
    		reg = APOLLOMP_GPIO_DIR_EFGHr;
    	break;
        default:
            return RT_ERR_INPUT;

	}

	if((ret = ioal_socMem32_read(reg,&value))!= RT_ERR_OK)
	{
	  RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");		
	  return ret;
	}
	*mode = (value >> offset) & 0x1;


#endif
    return RT_ERR_OK;
}   /* end of dal_apollomp_gpio_mode_get */




/* Function Name:
 *      dal_apollomp_gpio_databit_get
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
dal_apollomp_gpio_databit_get(uint32 gpioId, uint32 *data)
{
    int32 ret;
	uint32 reg,value,groupId,offset;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPIO), "gpioId=%d",gpioId);

    /* check Init status */
    RT_INIT_CHK(gpio_init);

    /* parameter check */
    RT_PARAM_CHK((APOLLOMP_GPIO_PIN_NUM  <=gpioId), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == data), RT_ERR_NULL_POINTER);

#if 0
    /*check gpio is enable*/
    if((ret=dal_apollomp_gpio_state_get(gpioId,&value)) != RT_ERR_OK)
    {
 	RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");	
	return ret;
    }
    if(value != ENABLED)
    {
      ret  = RT_ERR_NOT_ALLOWED;
    	RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");	
	return ret;
    }
    /*check gpio is input mode*/
    if((ret=dal_apollomp_gpio_mode_get(gpioId,&value)) != RT_ERR_OK)
    {
 	RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");	
	return ret;
    }
	
    if(value!=GPIO_INPUT)
    {
      ret = RT_ERR_NOT_ALLOWED;
    	RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");	
	return ret;
    }	

    /* function body */
   if ((ret=reg_array_field_read(APOLLOMP_GPIO_CTRL_1r, REG_ARRAY_INDEX_NONE, gpioId, APOLLOMP_STS_GPIOf,&value)) != RT_ERR_OK)
   {
   	RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");	
	return ret;
   }

   *data = value;
#else
	groupId = gpioId / 32;
	offset	= gpioId % 32;

	switch(groupId){
    	case 0:
    		reg = APOLLOMP_GPIO_DATA_ABCDr;
    	break;
    	case 1:
    		reg = APOLLOMP_GPIO_DATA_EFGHr;
    	break;
        default:
            return RT_ERR_INPUT;
	}

	if((ret = ioal_socMem32_read(reg,&value))!= RT_ERR_OK)
	{
	  RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");		
	  return ret;
	}	
	*data = (value >> offset) & 0x1;
#endif
    return RT_ERR_OK;
}   /* end of dal_apollomp_gpio_databit_get */


/* Function Name:
 *      dal_apollomp_gpio_databit_set
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
dal_apollomp_gpio_databit_set(uint32 gpioId, uint32 data)
{

    int32 ret;
	uint32 reg,value,groupId,offset,tmp;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPIO), "gpioId=%d",gpioId);

    /* check Init status */
    RT_INIT_CHK(gpio_init);

    /* parameter check */
    RT_PARAM_CHK((APOLLOMP_GPIO_PIN_NUM  <=gpioId), RT_ERR_INPUT);
    RT_PARAM_CHK((2 <=data), RT_ERR_INPUT);
#if 0
    /*check gpio is enable*/
    if((ret=dal_apollomp_gpio_state_get(gpioId,&value)) != RT_ERR_OK)
    {
 	RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");	
	return ret;
    }
    if(value != ENABLED)
    {
      ret  = RT_ERR_NOT_ALLOWED;
    	RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");	
	return ret;
    }
     /*check gpio is input mode*/
    if((ret=dal_apollomp_gpio_mode_get(gpioId,&value)) != RT_ERR_OK)
    {
 	RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");	
	return ret;
    }
	
    if(value!=GPIO_OUTPUT)
    {
      ret = RT_ERR_NOT_ALLOWED;
    	RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");	
	return ret;
    }	
	
    /* function body */
   if ((ret=reg_array_field_write(APOLLOMP_GPIO_CTRL_0r, REG_ARRAY_INDEX_NONE, gpioId, APOLLOMP_CTRL_GPIOf,&data)) != RT_ERR_OK)
   {
   	RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");	
	return ret;
   }
#else
	groupId = gpioId / 32;
	offset	= gpioId % 32;

	switch(groupId){
    	case 0:
    		reg = APOLLOMP_GPIO_DATA_ABCDr;
    	break;
    	case 1:
    		reg = APOLLOMP_GPIO_DATA_EFGHr;
    	break;
        default:
            return RT_ERR_INPUT;
	}

	if((ret = ioal_socMem32_read(reg,&value))!= RT_ERR_OK)
	{
	  RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");		
	  return ret;
	}

	tmp = 1 << offset;
	value = ~tmp & value;
	value = data << offset | value;

	if((ret = ioal_socMem32_write(reg,value))!= RT_ERR_OK)
	{
	  RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");		
	  return ret;
	}

#endif

    return RT_ERR_OK;
}   /* end of dal_apollomp_gpio_databit_set */

/* Function Name:
 *      dal_apollomp_gpio_intr_set
 * Description:
 *     write data to gpio
 * Input:
 *      intrId: id 0 for gpio pin 63, id 1 for gpio pin 65
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
dal_apollomp_gpio_intr_set(uint32 gpioId,rtk_gpio_intrMode_t intrMode)
{
    int32 ret,i;
    uint32 value = 0,reg,tmp,groupId,offset,data;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPIO), "intrMode=%d",intrMode);

    /* check Init status */
    RT_INIT_CHK(gpio_init);

    /* parameter check */
    RT_PARAM_CHK((GPIO_INTR_END <= intrMode), RT_ERR_INPUT);
    if((gpioId >= APOLLOMP_GPIO_PIN_NUM) && 
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
			value |= (intrMode << (i*2)) & 0x3;
		}
		reg	  = APOLLOMP_GPIO_IMR_ABr;
		if((ret = ioal_socMem32_write(reg,value))!= RT_ERR_OK)
		{
		  RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");		
		  return ret;
		}

		reg	  = APOLLOMP_GPIO_IMR_CDr;
		if((ret = ioal_socMem32_write(reg,value))!= RT_ERR_OK)
		{
		  RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");		
		  return ret;
		}
		
		reg   = APOLLOMP_GPIO_IMR_EFr;
		if((ret = ioal_socMem32_write(reg,value))!= RT_ERR_OK)
		{
		  RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");		
		  return ret;
		}
		
		reg   = APOLLOMP_GPIO_IMR_GHr;
		if((ret = ioal_socMem32_write(reg,value))!= RT_ERR_OK)
		{
		  RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");		
		  return ret;
		}
		
		reg = APOLLOMP_GPIO_CPU0_ABCDr;
		value = intrMode > 0 ? 0xffffffff:0;
		if((ret = ioal_socMem32_write(reg,value))!=RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");		
			return ret;
		}
		
	}else
	if(gpioId == RTK_GPIO_INTR_SET1)
	{
		for(i=0;i<16;i++)
		{
			value |= (intrMode << (i*2)) & 0x3;
		}
		reg	  = APOLLOMP_GPIO_IMR_ABr;
		if((ret = ioal_socMem32_write(reg,value))!= RT_ERR_OK)
		{
		  RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");		
		  return ret;
		}

		reg	  = APOLLOMP_GPIO_IMR_CDr;
		if((ret = ioal_socMem32_write(reg,value))!= RT_ERR_OK)
		{
		  RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");		
		  return ret;
		}

		reg = APOLLOMP_GPIO_CPU0_ABCDr;
		value = intrMode > 0 ? 0xffffffff:0;
		if((ret = ioal_socMem32_write(reg,value))!=RT_ERR_OK)
		{
			RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");		
			return ret;
		}
		
	}else
	if(gpioId == RTK_GPIO_INTR_SET2)
	{
		for(i=0;i<16;i++)
		{
			value |= (intrMode << (i*2)) & 0x3;
		}
		reg	  = APOLLOMP_GPIO_IMR_EFr;
		if((ret = ioal_socMem32_write(reg,value))!= RT_ERR_OK)
		{
		  RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");		
		  return ret;
		}

		reg	  = APOLLOMP_GPIO_IMR_GHr;
		if((ret = ioal_socMem32_write(reg,value))!= RT_ERR_OK)
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
    		  	reg = APOLLOMP_GPIO_IMR_ABr;
    		break;
    		case 1:
    		  	reg = APOLLOMP_GPIO_IMR_CDr;
    		break;
    		case 2:
    			reg = APOLLOMP_GPIO_IMR_EFr;
    		break;
    		case 3:
    			reg = APOLLOMP_GPIO_IMR_GHr;
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

		if(groupId == 0 || groupId == 1)
		{
			/*control gpio to cpu0*/
			reg = APOLLOMP_GPIO_CPU0_ABCDr;
			if((ret = ioal_socMem32_read(reg,&value))!=RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");		
				return ret;
			}

			data = intrMode > 0 ? 1:0;
			tmp = 1 << (gpioId%32);
			value = ~tmp & value;
			value = data << (gpioId%32) | value;
			
			if((ret = ioal_socMem32_write(reg,value))!=RT_ERR_OK)
			{
				RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");		
				return ret;
			}
		}
	}

    return RT_ERR_OK;
}



/* Function Name:
 *      dal_apollomp_gpio_intr_get
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
dal_apollomp_gpio_intr_get(uint32 gpioId,rtk_gpio_intrMode_t *pIntrMode)
{
   
	int32 ret;
	uint32 value,reg,groupId,offset;

	/* check Init status */
	RT_INIT_CHK(gpio_init);

	/* parameter check */
	RT_PARAM_CHK((NULL == pIntrMode), RT_ERR_INPUT);
	RT_PARAM_CHK((APOLLOMP_GPIO_INTR_NUM <= gpioId), RT_ERR_INPUT);

	groupId = gpioId / 16;
	offset  = gpioId % 16;

	switch(groupId){
    	case 0:
    	  	reg = APOLLOMP_GPIO_IMR_ABr;
    	break;
    	case 1:
    	  	reg = APOLLOMP_GPIO_IMR_CDr;
    	break;
    	case 2:
    		reg = APOLLOMP_GPIO_IMR_EFr;
    	break;
    	case 3:
    		reg = APOLLOMP_GPIO_IMR_GHr;
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
 *      dal_apollomp_gpio_intrStatus_clean
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
dal_apollomp_gpio_intrStatus_clean(uint32 gpioId)
{
	int32 ret;
    uint32 value,reg,groupId,offset;

    /* check Init status */
    RT_INIT_CHK(gpio_init);

    /* parameter check */
    if((gpioId >= APOLLOMP_GPIO_PIN_NUM) && 
       (gpioId != RTK_GPIO_INTR_ALL) &&
       (gpioId != RTK_GPIO_INTR_SET1) &&
       (gpioId != RTK_GPIO_INTR_SET2))
    {
        return RT_ERR_INPUT;
    }

	if(gpioId == RTK_GPIO_INTR_ALL)
	{		
		value = 0;
		reg = APOLLOMP_GPIO_IMS_ABCDr;
		if((ret = ioal_socMem32_write(reg,~value))!= RT_ERR_OK)
		{
		  RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");		
		  return ret;
		}
		reg = APOLLOMP_GPIO_IMS_EFGHr;
		if((ret = ioal_socMem32_write(reg,~value))!= RT_ERR_OK)
		{
		  RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");		
		  return ret;
		}
	}else
	if(gpioId == RTK_GPIO_INTR_SET1)
	{
		value = 0;
		reg = APOLLOMP_GPIO_IMS_ABCDr;
		if((ret = ioal_socMem32_write(reg,~value))!= RT_ERR_OK)
		{
		  RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");		
		  return ret;
		}
	}else
	if(gpioId == RTK_GPIO_INTR_SET2)
	{
		value = 0;
		reg = APOLLOMP_GPIO_IMS_EFGHr;
		if((ret = ioal_socMem32_write(reg,~value))!= RT_ERR_OK)
		{
		  RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");		
		  return ret;
		}
	}else
	{

		groupId = gpioId / 32;
		offset	= gpioId % 32;

		switch(groupId){
    		case 0:
    			reg = APOLLOMP_GPIO_IMS_ABCDr;
    		break;
    		case 1:
    			reg = APOLLOMP_GPIO_IMS_EFGHr;
    		break;
            default:
                return RT_ERR_INPUT;
		}
		
		value = 1 << offset;
		
		if((ret = ioal_socMem32_write(reg,value))!= RT_ERR_OK)
		{
		  RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");		
		  return ret;
		}
	}
    
    return RT_ERR_OK;
}


/* Function Name:
 *      dal_apollomp_gpio_intrStatus_get
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
dal_apollomp_gpio_intrStatus_get(uint32 gpioId,rtk_enable_t *pState)
{
	int32 ret;
    uint32 value,reg,groupId,offset;

	/* check Init status */
	RT_INIT_CHK(gpio_init);

	/* parameter check */
	RT_PARAM_CHK((NULL == pState), RT_ERR_INPUT);
    if((gpioId >= APOLLOMP_GPIO_PIN_NUM) && 
       (gpioId != RTK_GPIO_INTR_ALL) &&
       (gpioId != RTK_GPIO_INTR_SET1) &&
       (gpioId != RTK_GPIO_INTR_SET2))
    {
        return RT_ERR_INPUT;
    }

	if(gpioId == RTK_GPIO_INTR_SET1)
	{
		reg = APOLLOMP_GPIO_IMS_ABCDr;
		if((ret = ioal_socMem32_read(reg,&value))!= RT_ERR_OK)
		{
		  RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");		
		  return ret;
		} 
		*pState = value;
		
	}else
	if(gpioId == RTK_GPIO_INTR_SET2)
	{
		reg = APOLLOMP_GPIO_IMS_EFGHr;
		if((ret = ioal_socMem32_read(reg,&value))!= RT_ERR_OK)
		{
		  RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");		
		  return ret;
		}		
		*pState = value;
	}else
	{

		groupId = gpioId / 32;
		offset	= gpioId % 32;

		switch(groupId){
    		case 0:
    			reg = APOLLOMP_GPIO_IMS_ABCDr;
    		break;
    		case 1:
    			reg = APOLLOMP_GPIO_IMS_EFGHr;
    		break;
            default:
                return RT_ERR_INPUT;
		}

		if((ret = ioal_socMem32_read(reg,&value))!= RT_ERR_OK)
		{
		  RT_ERR(ret, (MOD_DAL|MOD_GPIO), "");		
		  return ret;
		}  
		*pState = value >> offset & 0x1;
	}
	
    return RT_ERR_OK;

}


