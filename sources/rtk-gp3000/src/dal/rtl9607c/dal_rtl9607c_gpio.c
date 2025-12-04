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
#include <dal/rtl9607c/dal_rtl9607c.h>
#include <rtk/gpio.h>
#include <dal/rtl9607c/dal_rtl9607c_gpio.h>
#include <ioal/mem32.h>


static uint32 gpio_init = INIT_COMPLETED;
static uint32 gpioDataIsSet[RTL9607C_GPIO_PIN_NUM/32];
static uint32 gpioDataRegDb[RTL9607C_GPIO_PIN_NUM/32];

/* Function Name:
 *      dal_rtl9607c_gpio_init
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
dal_rtl9607c_gpio_init ( void )
{
    RT_DBG(LOG_DEBUG, (MOD_DAL | MOD_GPIO), "%s", __FUNCTION__);

    /* Clear software database */
    memset(gpioDataIsSet, 0, sizeof(uint32) * (RTL9607C_GPIO_PIN_NUM/32));
    memset(gpioDataRegDb, 0, sizeof(uint32) * (RTL9607C_GPIO_PIN_NUM/32));
    
    gpio_init = INIT_COMPLETED;

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_rtl9607c_gpio_state_set
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
dal_rtl9607c_gpio_state_set ( uint32 gpioId, rtk_enable_t enable )
{
    int32 ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL | MOD_GPIO), "gpioId=%d,enable=%d", gpioId, enable);

    /* check Init status */
    RT_INIT_CHK(gpio_init);

    /* parameter check */
    RT_PARAM_CHK((RTL9607C_GPIO_PIN_NUM <= gpioId), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    /* function body */
    if ((ret = reg_array_field_write(
                    RTL9607C_IO_GPIO_ENr, 
                    REG_ARRAY_INDEX_NONE, 
                    gpioId, 
                    RTL9607C_IO_GPIO_ENf, 
                    &enable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL | MOD_GPIO), "");
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9607c_gpio_state_set */



/* Function Name:
 *      dal_rtl9607c_gpio_state_get
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
dal_rtl9607c_gpio_state_get ( uint32 gpioId, rtk_enable_t* enable )
{
    int32 ret;

    uint32 value;

    RT_DBG(LOG_DEBUG, (MOD_DAL | MOD_GPIO), "gpioId=%d,enable=%d", gpioId, *enable);

    /* check Init status */
    RT_INIT_CHK(gpio_init);

    /* parameter check */
    RT_PARAM_CHK((RTL9607C_GPIO_PIN_NUM <= gpioId), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == enable), RT_ERR_INPUT);

    /* function body */
    if ((ret = reg_array_field_read(
                    RTL9607C_IO_GPIO_ENr, 
                    REG_ARRAY_INDEX_NONE, 
                    gpioId, 
                    RTL9607C_IO_GPIO_ENf, 
                    &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL | MOD_GPIO), "");
        return ret;
    }
    
    *enable = value;

    return RT_ERR_OK;
}   /* end of dal_rtl9607c_gpio_state_get */

/* Function Name:
 *      dal_rtl9607c_gpio_mode_set
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
dal_rtl9607c_gpio_mode_set ( uint32 gpioId, rtk_gpio_mode_t mode )
{

    int32 ret;
    uint32 reg, value, groupId, offset, tmp;

    RT_DBG(LOG_DEBUG, (MOD_DAL | MOD_GPIO), "gpioId=%d,mode=%d", gpioId, mode);

    /* check Init status */
    RT_INIT_CHK(gpio_init);

    /* parameter check */
    RT_PARAM_CHK((RTL9607C_GPIO_PIN_NUM <= gpioId), RT_ERR_INPUT);
    RT_PARAM_CHK((GPIO_MODE_END <= mode), RT_ERR_INPUT);

    /* function body */
    groupId = gpioId / 32;
    offset = gpioId % 32;

    switch (groupId)
    {
        case 0:
            reg = RTL9607C_GPIO_DIR_ABCDr;
            break;
        case 1:
            reg = RTL9607C_GPIO_DIR_EFGHr;
            break;
        case 2:
            reg = RTL9607C_GPIO_DIR_JKMNr;
            break;
        default:
            return RT_ERR_INPUT;

    }

    if ((ret = ioal_socMem32_read(reg, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL | MOD_GPIO), "");
        return ret;
    }

    tmp = 1 << offset;
    value = ~tmp & value;
    value = mode << offset | value;

    if ((ret = ioal_socMem32_write(reg, value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL | MOD_GPIO), "");
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9607c_gpio_mode_set */


/* Function Name:
 *      dal_rtl9607c_gpio_mode_get
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
dal_rtl9607c_gpio_mode_get ( uint32 gpioId, rtk_gpio_mode_t* mode )
{

    int32 ret;
    uint32 reg, value, groupId, offset;

    RT_DBG(LOG_DEBUG, (MOD_DAL | MOD_GPIO), "gpioId=%d,mode=%d", gpioId, *mode);

    /* check Init status */
    RT_INIT_CHK(gpio_init);

    /* parameter check */
    RT_PARAM_CHK((RTL9607C_GPIO_PIN_NUM <= gpioId), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == mode), RT_ERR_NULL_POINTER);

    /* function body */
    groupId = gpioId / 32;
    offset = gpioId % 32;

    switch (groupId)
    {
        case 0:
            reg = RTL9607C_GPIO_DIR_ABCDr;
            break;
        case 1:
            reg = RTL9607C_GPIO_DIR_EFGHr;
            break;
        case 2:
            reg = RTL9607C_GPIO_DIR_JKMNr;
            break;
        default:
            return RT_ERR_INPUT;

    }

    if ((ret = ioal_socMem32_read(reg, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL | MOD_GPIO), "");
        return ret;
    }

    *mode = (value >> offset) & 0x1;

    return RT_ERR_OK;
}   /* end of dal_rtl9607c_gpio_mode_get */




/* Function Name:
 *      dal_rtl9607c_gpio_databit_get
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
int32
dal_rtl9607c_gpio_databit_get ( uint32 gpioId, uint32* data )
{
    int32 ret;
    uint32 reg, value, groupId, offset;

    RT_DBG(LOG_DEBUG, (MOD_DAL | MOD_GPIO), "gpioId=%d", gpioId);

    /* check Init status */
    RT_INIT_CHK(gpio_init);

    /* parameter check */
    RT_PARAM_CHK((RTL9607C_GPIO_PIN_NUM <= gpioId), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == data), RT_ERR_NULL_POINTER);

    groupId = gpioId / 32;
    offset = gpioId % 32;

    switch (groupId)
    {
        case 0:
            reg = RTL9607C_GPIO_DATA_ABCDr;
            break;
        case 1:
            reg = RTL9607C_GPIO_DATA_EFGHr;
            break;
        case 2:
            reg = RTL9607C_GPIO_DATA_JKMNr;
            break;
        default:
            return RT_ERR_INPUT;
    }

    if ((ret = ioal_socMem32_read(reg, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL | MOD_GPIO), "");
        return ret;
    }
    *data = (value >> offset) & 0x1;

    return RT_ERR_OK;
}   /* end of dal_rtl9607c_gpio_databit_get */


/* Function Name:
 *      dal_rtl9607c_gpio_databit_set
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
dal_rtl9607c_gpio_databit_set ( uint32 gpioId, uint32 data )
{

    int32 ret;
    uint32 reg, dataReg, value, groupId, offset, tmp;

    RT_DBG(LOG_DEBUG, (MOD_DAL | MOD_GPIO), "gpioId=%d", gpioId);

    /* check Init status */
    RT_INIT_CHK(gpio_init);

    /* parameter check */
    RT_PARAM_CHK((RTL9607C_GPIO_PIN_NUM <= gpioId), RT_ERR_INPUT);
    RT_PARAM_CHK((2 <= data), RT_ERR_INPUT);

    groupId = gpioId / 32;
    offset = gpioId % 32;

    switch (groupId)
    {
        case 0:
            reg = RTL9607C_GPIO_DATA_ABCDr;
            break;
        case 1:
            reg = RTL9607C_GPIO_DATA_EFGHr;
            break;
        case 2:
            reg = RTL9607C_GPIO_DATA_JKMNr;
            break;
        default:
            return RT_ERR_INPUT;
    }

    if ((ret = ioal_socMem32_read(reg, &dataReg)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL | MOD_GPIO), "");
        return ret;
    }

    /* Compose the value to be written from stored database & actual read value */
    value = (gpioDataIsSet[groupId] & gpioDataRegDb[groupId]) | (~gpioDataIsSet[groupId] & dataReg);

    tmp = 1 << offset;
    value = ~tmp & value;
    value = data << offset | value;

    /* Update database and set flag */
    gpioDataIsSet[groupId] |= (1<<offset);
    gpioDataRegDb[groupId] = (gpioDataRegDb[groupId] & ~(1<<offset)) | (data << offset);

    if ((ret = ioal_socMem32_write(reg, value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL | MOD_GPIO), "");
        return ret;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl9607c_gpio_databit_set */

/* Function Name:
 *      dal_rtl9607c_gpio_intr_set
 * Description:
 *     write data to gpio
 * Input:
 *      intrId: group id 0 for gpio pin 97, group id 1 for gpio pin 98 
 *          group id 2 for gpio pin 99
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
dal_rtl9607c_gpio_intr_set ( uint32 gpioId, rtk_gpio_intrMode_t intrMode )
{
    int32 ret, i;
    uint32 value = 0, reg, tmp, groupId, offset, data;

    RT_DBG(LOG_DEBUG, (MOD_DAL | MOD_GPIO), "intrMode=%d", intrMode);

    /* check Init status */
    RT_INIT_CHK(gpio_init);

    /* parameter check */
    RT_PARAM_CHK((GPIO_INTR_END <= intrMode), RT_ERR_INPUT);
    if((gpioId >= RTL9607C_GPIO_PIN_NUM) && 
       (gpioId != RTK_GPIO_INTR_ALL) &&
       (gpioId != RTK_GPIO_INTR_SET1) &&
       (gpioId != RTK_GPIO_INTR_SET2) &&
       (gpioId != RTK_GPIO_INTR_SET3))
    {
        return RT_ERR_INPUT;
    }

    if (RTK_GPIO_INTR_ALL == gpioId)
    {

        for (i = 0; i < 16; i++)
        {
            value |= (intrMode << (i * 2)) & 0x3;
        }
        reg = RTL9607C_GPIO_IMR_ABr;
        if ((ret = ioal_socMem32_write(reg, value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL | MOD_GPIO), "");
            return ret;
        }

        reg = RTL9607C_GPIO_IMR_CDr;
        if ((ret = ioal_socMem32_write(reg, value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL | MOD_GPIO), "");
            return ret;
        }

        reg = RTL9607C_GPIO_IMR_EFr;
        if ((ret = ioal_socMem32_write(reg, value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL | MOD_GPIO), "");
            return ret;
        }

        reg = RTL9607C_GPIO_IMR_GHr;
        if ((ret = ioal_socMem32_write(reg, value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL | MOD_GPIO), "");
            return ret;
        }

        reg = RTL9607C_GPIO_IMR_JKr;
        if ((ret = ioal_socMem32_write(reg, value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL | MOD_GPIO), "");
            return ret;
        }

        reg = RTL9607C_GPIO_IMR_MNr;
        if ((ret = ioal_socMem32_write(reg, value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL | MOD_GPIO), "");
            return ret;
        }

        reg = RTL9607C_GPIO_CPU0_ABCDr;
        value = intrMode > 0 ? 0xffffffff : 0;
        if ((ret = ioal_socMem32_write(reg, value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL | MOD_GPIO), "");
            return ret;
        }

        reg = RTL9607C_GPIO_CPU1_ABCDr;
        value = intrMode > 0 ? 0xffffffff : 0;
        if ((ret = ioal_socMem32_write(reg, value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL | MOD_GPIO), "");
            return ret;
        }
    }
    else if (gpioId == RTK_GPIO_INTR_SET1)
    {
        for (i = 0; i < 16; i++)
        {
            value |= (intrMode << (i * 2)) & 0x3;
        }
        reg = RTL9607C_GPIO_IMR_ABr;
        if ((ret = ioal_socMem32_write(reg, value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL | MOD_GPIO), "");
            return ret;
        }

        reg = RTL9607C_GPIO_IMR_CDr;
        if ((ret = ioal_socMem32_write(reg, value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL | MOD_GPIO), "");
            return ret;
        }

        reg = RTL9607C_GPIO_CPU0_ABCDr;
        value = intrMode > 0 ? 0xffffffff : 0;
        if ((ret = ioal_socMem32_write(reg, value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL | MOD_GPIO), "");
            return ret;
        }

        reg = RTL9607C_GPIO_CPU1_ABCDr;
        value = intrMode > 0 ? 0xffffffff : 0;
        if ((ret = ioal_socMem32_write(reg, value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL | MOD_GPIO), "");
            return ret;
        }
    }
    else if (gpioId == RTK_GPIO_INTR_SET2)
    {
        for (i = 0; i < 16; i++)
        {
            value |= (intrMode << (i * 2)) & 0x3;
        }
        reg = RTL9607C_GPIO_IMR_EFr;
        if ((ret = ioal_socMem32_write(reg, value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL | MOD_GPIO), "");
            return ret;
        }

        reg = RTL9607C_GPIO_IMR_GHr;
        if ((ret = ioal_socMem32_write(reg, value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL | MOD_GPIO), "");
            return ret;
        }
    }
    else if (gpioId == RTK_GPIO_INTR_SET3)
    {
        for (i = 0; i < 16; i++)
        {
            value |= (intrMode << (i * 2)) & 0x3;
        }
        reg = RTL9607C_GPIO_IMR_JKr;
        if ((ret = ioal_socMem32_write(reg, value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL | MOD_GPIO), "");
            return ret;
        }

        reg = RTL9607C_GPIO_IMR_MNr;
        if ((ret = ioal_socMem32_write(reg, value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL | MOD_GPIO), "");
            return ret;
        }
    }
    else
    {

        groupId = gpioId / 16;
        offset = gpioId % 16;

        switch (groupId)
        {
            case 0:
                reg = RTL9607C_GPIO_IMR_ABr;
                break;
            case 1:
                reg = RTL9607C_GPIO_IMR_CDr;
                break;
            case 2:
                reg = RTL9607C_GPIO_IMR_EFr;
                break;
            case 3:
                reg = RTL9607C_GPIO_IMR_GHr;
                break;
            case 4:
                reg = RTL9607C_GPIO_IMR_JKr;
                break;
            case 5:
                reg = RTL9607C_GPIO_IMR_MNr;
                break;

            default:
                return RT_ERR_INPUT;
        }

        if ((ret = ioal_socMem32_read(reg, &value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL | MOD_GPIO), "");
            return ret;
        }

        tmp = 0x3 << (offset * 2);
        value = ~tmp & value;
        value = ((intrMode & 0x3) << (offset * 2)) | value;

        if ((ret = ioal_socMem32_write(reg, value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL | MOD_GPIO), "");
            return ret;
        }
 
        if (groupId == 0 || groupId == 1)
        {
            /*control gpio to cpu0&1*/
            int idx = 0;

            while (idx < 2)
            {
                if (idx == 0)
                {
                    reg = RTL9607C_GPIO_CPU0_ABCDr;
                }
                else if (idx == 1)
                {
                    reg = RTL9607C_GPIO_CPU1_ABCDr;
                }

                if ((ret = ioal_socMem32_read(reg, &value)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL | MOD_GPIO), "");
                    return ret;
                }

                data = intrMode > 0 ? 1 : 0;
                tmp = 1 << (gpioId % 32);
                value = ~tmp & value;
                value = data << (gpioId % 32) | value;

                if ((ret = ioal_socMem32_write(reg, value)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL | MOD_GPIO), "");
                    return ret;
                }
                idx++;
            }
        }
    }

    return RT_ERR_OK;
}



/* Function Name:
 *      dal_rtl9607c_gpio_intr_get
 * Description:
 *     write data to gpio
 * Input:
 *      gpioId - gpio pin id from 0~95 can support interrupt
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
dal_rtl9607c_gpio_intr_get ( uint32 gpioId, rtk_gpio_intrMode_t* pIntrMode )
{

    int32 ret;
    uint32 value, reg, groupId, offset;

    /* check Init status */
    RT_INIT_CHK(gpio_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIntrMode), RT_ERR_INPUT);
    RT_PARAM_CHK((RTL9607C_GPIO_INTR_NUM <= gpioId), RT_ERR_INPUT);

    groupId = gpioId / 16;
    offset = gpioId % 16;

    switch (groupId)
    {
        case 0:
            reg = RTL9607C_GPIO_IMR_ABr;
            break;
        case 1:
            reg = RTL9607C_GPIO_IMR_CDr;
            break;
        case 2:
            reg = RTL9607C_GPIO_IMR_EFr;
            break;
        case 3:
            reg = RTL9607C_GPIO_IMR_GHr;
            break;
        case 4:
            reg = RTL9607C_GPIO_IMR_JKr;
            break;
        case 5:
            reg = RTL9607C_GPIO_IMR_MNr;
            break;
        default:
            return RT_ERR_INPUT;
    }


    if ((ret = ioal_socMem32_read(reg, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL | MOD_GPIO), "");
        return ret;
    }

    *pIntrMode = (value >> (offset * 2)) & 0x3;

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_rtl9607c_gpio_intrStatus_clean
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
dal_rtl9607c_gpio_intrStatus_clean ( uint32 gpioId )
{
    int32 ret;
    uint32 value, reg, groupId, offset;

    /* check Init status */
    RT_INIT_CHK(gpio_init);

    /* parameter check */
    if((gpioId >= RTL9607C_GPIO_PIN_NUM) && 
       (gpioId != RTK_GPIO_INTR_ALL) &&
       (gpioId != RTK_GPIO_INTR_SET1) &&
       (gpioId != RTK_GPIO_INTR_SET2) &&
       (gpioId != RTK_GPIO_INTR_SET3))
    {
        return RT_ERR_INPUT;
    }

    if (gpioId == RTK_GPIO_INTR_ALL)
    {
        value = 0;
        reg = RTL9607C_GPIO_IMS_ABCDr;
        if ((ret = ioal_socMem32_write(reg, ~value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL | MOD_GPIO), "");
            return ret;
        }
        reg = RTL9607C_GPIO_IMS_EFGHr;
        if ((ret = ioal_socMem32_write(reg, ~value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL | MOD_GPIO), "");
            return ret;
        }
        reg = RTL9607C_GPIO_IMS_JKMNr;
        if ((ret = ioal_socMem32_write(reg, ~value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL | MOD_GPIO), "");
            return ret;
        }
    }
    else if (gpioId == RTK_GPIO_INTR_SET1)
    {
        value = 0;
        reg = RTL9607C_GPIO_IMS_ABCDr;
        if ((ret = ioal_socMem32_write(reg, ~value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL | MOD_GPIO), "");
            return ret;
        }
    }
    else if (gpioId == RTK_GPIO_INTR_SET2)
    {
        value = 0;
        reg = RTL9607C_GPIO_IMS_EFGHr;
        if ((ret = ioal_socMem32_write(reg, ~value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL | MOD_GPIO), "");
            return ret;
        }
    }
    else if (gpioId == RTK_GPIO_INTR_SET3)
    {
        value = 0;
        reg = RTL9607C_GPIO_IMS_JKMNr;
        if ((ret = ioal_socMem32_write(reg, ~value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL | MOD_GPIO), "");
            return ret;
        }
    }
    else
    {

        groupId = gpioId / 32;
        offset = gpioId % 32;

        switch (groupId)
        {
            case 0:
                reg = RTL9607C_GPIO_IMS_ABCDr;
                break;
            case 1:
                reg = RTL9607C_GPIO_IMS_EFGHr;
                break;
            case 2:
                reg = RTL9607C_GPIO_IMS_JKMNr;
                break;
            default:
                return RT_ERR_INPUT;
        }

        value = 1 << offset;

        if ((ret = ioal_socMem32_write(reg, value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL | MOD_GPIO), "");
            return ret;
        }
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_rtl9607c_gpio_intrStatus_get
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
dal_rtl9607c_gpio_intrStatus_get ( uint32 gpioId, rtk_enable_t* pState )
{
    int32 ret;
    uint32 value, reg, groupId, offset;

    /* check Init status */
    RT_INIT_CHK(gpio_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pState), RT_ERR_INPUT);
    if((gpioId >= RTL9607C_GPIO_PIN_NUM) && 
       (gpioId != RTK_GPIO_INTR_ALL) &&
       (gpioId != RTK_GPIO_INTR_SET1) &&
       (gpioId != RTK_GPIO_INTR_SET2) &&
       (gpioId != RTK_GPIO_INTR_SET3))
    {
        return RT_ERR_INPUT;
    }

    if (gpioId == RTK_GPIO_INTR_SET1)
    {
        reg = RTL9607C_GPIO_IMS_ABCDr;
        if ((ret = ioal_socMem32_read(reg, &value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL | MOD_GPIO), "");
            return ret;
        }
        *pState = value;

    }
    else if (gpioId == RTK_GPIO_INTR_SET2)
    {
        reg = RTL9607C_GPIO_IMS_EFGHr;
        if ((ret = ioal_socMem32_read(reg, &value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL | MOD_GPIO), "");
            return ret;
        }
        *pState = value;
    }
    else if (gpioId == RTK_GPIO_INTR_SET3)
    {
        reg = RTL9607C_GPIO_IMS_JKMNr;
        if ((ret = ioal_socMem32_read(reg, &value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL | MOD_GPIO), "");
            return ret;
        }
        *pState = value;
    }
    else
    {

        groupId = gpioId / 32;
        offset = gpioId % 32;

        switch (groupId)
        {
            case 0:
                reg = RTL9607C_GPIO_IMS_ABCDr;
                break;
            case 1:
                reg = RTL9607C_GPIO_IMS_EFGHr;
                break;
            case 2:
                reg = RTL9607C_GPIO_IMS_JKMNr;
                break;
            default:
                return RT_ERR_INPUT;
        }

        if ((ret = ioal_socMem32_read(reg, &value)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL | MOD_GPIO), "");
            return ret;
        }
        *pState = value >> offset & 0x1;
    }

    return RT_ERR_OK;

}


