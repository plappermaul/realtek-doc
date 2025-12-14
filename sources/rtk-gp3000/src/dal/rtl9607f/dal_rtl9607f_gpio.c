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
#include <dal/rtl9607f/dal_rtl9607f.h>
#include <rtk/gpio.h>
#include <dal/rtl9607f/dal_rtl9607f_gpio.h>
#include <aal_common.h>
#include <soc/cortina/registers.h>

#include <linux/io.h>

static uint32 gpio_init = INIT_COMPLETED;

/* Function Name:
 *      dal_rtl9607f_gpio_init
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
dal_rtl9607f_gpio_init ( void )
{
    RT_DBG(LOG_DEBUG, (MOD_DAL | MOD_GPIO), "%s", __FUNCTION__);
    gpio_init = INIT_COMPLETED;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl9607f_gpio_state_set
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
dal_rtl9607f_gpio_state_set ( uint32 gpioId, rtk_enable_t enable )
{
    int32 ret;
    uint32 value,tmp;
    uint32 groupId = gpioId/32;
    void __iomem *venus_gpio_mux_iobase = NULL;

    RT_DBG(LOG_DEBUG, (MOD_DAL | MOD_GPIO), "gpioId=%d,enable=%d", gpioId, enable);

    /* check Init status */
    RT_INIT_CHK(gpio_init);

    /* parameter check */
    RT_PARAM_CHK((RTL9607F_GPIO_PIN_NUM <= gpioId), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    /* function body */
    venus_gpio_mux_iobase = ioremap(GLOBAL_GPIO_MUX_0+0x4*groupId, 4);
    if (!venus_gpio_mux_iobase) {
        printk("%s %d: ioremap(%x) failed!!!\n",__FUNCTION__,__LINE__,GLOBAL_GPIO_MUX_0+0x4*groupId);
        return RT_ERR_FAILED;
    }
    tmp = 1<<(gpioId - 32*groupId) ;
//    value = CA_NE_REG_READ(GLOBAL_GPIO_MUX_0+0x4*groupId);
    value = readl(venus_gpio_mux_iobase);
    if(enable == TRUE)
        value |= tmp;
    else
        value &= ~tmp;
//    CA_NE_REG_WRITE(value,GLOBAL_GPIO_MUX_0+0x4*groupId);
    writel(value,venus_gpio_mux_iobase);

    iounmap(venus_gpio_mux_iobase);

    return RT_ERR_OK;
}   /* end of dal_rtl9607f_gpio_state_set */

/* Function Name:
 *      dal_rtl9607f_gpio_state_get
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
dal_rtl9607f_gpio_state_get ( uint32 gpioId, rtk_enable_t* enable )
{
    int32 ret;
    uint32 value;
    uint32 groupId = gpioId/32;
    void __iomem *venus_gpio_mux_iobase = NULL;

    RT_DBG(LOG_DEBUG, (MOD_DAL | MOD_GPIO), "gpioId=%d", gpioId);

    /* check Init status */
    RT_INIT_CHK(gpio_init);

    /* parameter check */
    RT_PARAM_CHK((RTL9607F_GPIO_PIN_NUM <= gpioId), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == enable), RT_ERR_INPUT);

    /* function body */
    venus_gpio_mux_iobase = ioremap(GLOBAL_GPIO_MUX_0+0x4*groupId, 4);
    if (!venus_gpio_mux_iobase) {
        printk("%s %d: ioremap(%x) failed!!!\n",__FUNCTION__,__LINE__,GLOBAL_GPIO_MUX_0+0x4*groupId);
        return RT_ERR_FAILED;
    }

//    value = CA_NE_REG_READ(GLOBAL_GPIO_MUX_0+0x4*groupId);
    value = readl(venus_gpio_mux_iobase);

    if(value&(1<<(gpioId - 32*groupId)))
        *enable = TRUE;
    else
        *enable = FALSE;

    iounmap(venus_gpio_mux_iobase);

    return RT_ERR_OK;
}   /* end of dal_rtl9607f_gpio_state_get */

/* Function Name:
 *      dal_rtl9607f_gpio_mode_set
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
dal_rtl9607f_gpio_mode_set ( uint32 gpioId, rtk_gpio_mode_t mode )
{
    int32 ret;
    uint32 value,tmp;
    uint32 groupId = gpioId/32;
    void __iomem *venus_per_gpio_iobase = NULL;

    RT_DBG(LOG_DEBUG, (MOD_DAL | MOD_GPIO), "gpioId=%d,mode=%d", gpioId, mode);

    /* check Init status */
    RT_INIT_CHK(gpio_init);

    /* parameter check */
    RT_PARAM_CHK((RTL9607F_GPIO_PIN_NUM <= gpioId), RT_ERR_INPUT);
    RT_PARAM_CHK((GPIO_MODE_END <= mode), RT_ERR_INPUT);

    /* function body */
    venus_per_gpio_iobase = ioremap(PER_GPIO0_CFG+0x24*groupId, 4);
    if (!venus_per_gpio_iobase) {
        printk("%s %d: ioremap(%x) failed!!!\n",__FUNCTION__,__LINE__,PER_GPIO0_CFG+0x24*groupId);
        return RT_ERR_FAILED;
    }

    tmp = 1<<(gpioId - 32*groupId);
//    value = CA_NE_REG_READ(PER_GPIO0_CFG+0x24*groupId);
    value = readl(venus_per_gpio_iobase);

    if(mode == GPIO_INPUT)
        value |= tmp;
    else
        value &= ~tmp;
//    CA_NE_REG_WRITE(value,PER_GPIO0_CFG+0x24*groupId);
    writel(value,venus_per_gpio_iobase);

    iounmap(venus_per_gpio_iobase);

    return RT_ERR_OK;
}   /* end of dal_rtl9607f_gpio_mode_set */


/* Function Name:
 *      dal_rtl9607f_gpio_mode_get
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
dal_rtl9607f_gpio_mode_get ( uint32 gpioId, rtk_gpio_mode_t* mode )
{
    int32 ret;
    uint32 value;
    uint32 groupId = gpioId/32;
    void __iomem *venus_per_gpio_iobase = NULL;

    RT_DBG(LOG_DEBUG, (MOD_DAL | MOD_GPIO), "gpioId=%d", gpioId);

    /* check Init status */
    RT_INIT_CHK(gpio_init);

    /* parameter check */
    RT_PARAM_CHK((RTL9607F_GPIO_PIN_NUM <= gpioId), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == mode), RT_ERR_NULL_POINTER);

    /* function body */
    venus_per_gpio_iobase = ioremap(PER_GPIO0_CFG+0x24*groupId, 4);
    if (!venus_per_gpio_iobase) {
        printk("%s %d: ioremap(%x) failed!!!\n",__FUNCTION__,__LINE__,PER_GPIO0_CFG+0x24*groupId);
        return RT_ERR_FAILED;
    }

//    value = CA_NE_REG_READ(PER_GPIO0_CFG+0x24*groupId);
    value = readl(venus_per_gpio_iobase);

    if(value&1<<(gpioId - 32*groupId))
        *mode = GPIO_INPUT;
    else
        *mode = GPIO_OUTPUT;

    iounmap(venus_per_gpio_iobase);
    
    return RT_ERR_OK;
}   /* end of dal_rtl9607f_gpio_mode_get */

/* Function Name:
 *      dal_rtl9607f_gpio_databit_get
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
dal_rtl9607f_gpio_databit_get ( uint32 gpioId, uint32* data )
{
    int32 ret;
    uint32 value;
    uint32 groupId = gpioId/32;
    void __iomem *venus_per_gpio_iobase = NULL;

    RT_DBG(LOG_DEBUG, (MOD_DAL | MOD_GPIO), "gpioId=%d", gpioId);

    /* check Init status */
    RT_INIT_CHK(gpio_init);

    /* parameter check */
    RT_PARAM_CHK((RTL9607F_GPIO_PIN_NUM <= gpioId), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == data), RT_ERR_NULL_POINTER);

    /* function body */
    venus_per_gpio_iobase = ioremap(PER_GPIO0_IN+0x24*groupId, 4);
    if (!venus_per_gpio_iobase) {
        printk("%s %d: ioremap(%x) failed!!!\n",__FUNCTION__,__LINE__,PER_GPIO0_IN+0x24*groupId);
        return RT_ERR_FAILED;
    }

//        value = CA_NE_REG_READ(PER_GPIO0_IN+0x24*groupId);
    value = readl(venus_per_gpio_iobase);

    if(value&(1<<(gpioId - 32*groupId)))
        *data = 1;
    else
        *data = 0;

    iounmap(venus_per_gpio_iobase);

    return RT_ERR_OK;
}   /* end of dal_rtl9607f_gpio_databit_get */

/* Function Name:
 *      dal_rtl9607f_gpio_databit_set
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
dal_rtl9607f_gpio_databit_set ( uint32 gpioId, uint32 data )
{
    int32 ret;
    uint32 value,tmp;
    uint32 groupId = gpioId/32;
    void __iomem *venus_per_gpio_iobase = NULL;

    RT_DBG(LOG_DEBUG, (MOD_DAL | MOD_GPIO), "gpioId=%d data=%d", gpioId,data);

    /* check Init status */
    RT_INIT_CHK(gpio_init);

    /* parameter check */
    RT_PARAM_CHK((RTL9607F_GPIO_PIN_NUM <= gpioId), RT_ERR_INPUT);
    RT_PARAM_CHK((2 <= data), RT_ERR_INPUT);

    /* function body */
    tmp = 1<<(gpioId - 32*groupId);

    venus_per_gpio_iobase = ioremap(PER_GPIO0_OUT+0x24*groupId, 4);
    if (!venus_per_gpio_iobase) {
        printk("%s %d: ioremap(%x) failed!!!\n",__FUNCTION__,__LINE__,PER_GPIO0_OUT+0x24*groupId);
        return RT_ERR_FAILED;
    }

//        value = CA_NE_REG_READ(PER_GPIO0_OUT+0x24*groupId);
    value = readl(venus_per_gpio_iobase);

    if(data == 1)
        value |= tmp;
    else
        value &= ~tmp;
//        CA_NE_REG_WRITE(value,PER_GPIO0_OUT+0x24*groupId);
    writel(value,venus_per_gpio_iobase);

    iounmap(venus_per_gpio_iobase);

    return RT_ERR_OK;
}   /* end of dal_rtl9607f_gpio_databit_set */
