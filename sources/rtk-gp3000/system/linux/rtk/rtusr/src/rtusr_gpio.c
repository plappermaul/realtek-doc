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
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <osal/lib.h>
#include <rtk/rtusr/include/rtusr_util.h>
#include <rtdrv/rtdrv_netfilter.h>
#include <rtk/gpio.h>



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
    rtdrv_gpioCfg_t gpio_cfg;
    osal_memset(&gpio_cfg,0x0,sizeof(rtdrv_gpioCfg_t));
    SETSOCKOPT(RTDRV_GPIO_INIT, &gpio_cfg, rtdrv_gpioCfg_t, 1);
    return RT_ERR_OK;
}



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
int32 
rtk_gpio_state_set(uint32 gpioId, rtk_enable_t enable)
{
    /* function body */
    rtdrv_gpioCfg_t gpio_cfg;
    osal_memset(&gpio_cfg,0x0,sizeof(rtdrv_gpioCfg_t));
    osal_memcpy(&gpio_cfg.gpioId, &gpioId, sizeof(uint32));
    osal_memcpy(&gpio_cfg.enable, &enable, sizeof(rtk_enable_t));
    SETSOCKOPT(RTDRV_GPIO_STATE_SET, &gpio_cfg, rtdrv_gpioCfg_t, 1);
    return RT_ERR_OK;
}   /* end of rtk_gpio_enable */


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
    /* function body */
    rtdrv_gpioCfg_t gpio_cfg;

    osal_memcpy(&gpio_cfg.gpioId, &gpioId, sizeof(uint32));
    GETSOCKOPT(RTDRV_GPIO_STATE_GET, &gpio_cfg, rtdrv_gpioCfg_t, 1);
    *enable = gpio_cfg.enable;

    return RT_ERR_OK;
}   /* end of rtk_gpio_enable */


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
    /* function body */
    rtdrv_gpioCfg_t gpio_cfg;

    osal_memset(&gpio_cfg,0x0,sizeof(rtdrv_gpioCfg_t));
    osal_memcpy(&gpio_cfg.gpioId, &gpioId, sizeof(uint32));
    osal_memcpy(&gpio_cfg.mode, &mode, sizeof(rtk_gpio_mode_t));
    SETSOCKOPT(RTDRV_GPIO_MODE_SET, &gpio_cfg, rtdrv_gpioCfg_t, 1);
    return RT_ERR_OK;
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
    /* function body */
    rtdrv_gpioCfg_t gpio_cfg;

    osal_memcpy(&gpio_cfg.gpioId, &gpioId, sizeof(uint32));
    GETSOCKOPT(RTDRV_GPIO_MODE_GET, &gpio_cfg, rtdrv_gpioCfg_t, 1);
    *mode = gpio_cfg.mode;
    
    return RT_ERR_OK;
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
    /* function body */
    rtdrv_gpioCfg_t gpio_cfg;

    osal_memcpy(&gpio_cfg.gpioId, &gpioId, sizeof(uint32));
    GETSOCKOPT(RTDRV_GPIO_DATABIT_GET, &gpio_cfg, rtdrv_gpioCfg_t, 1);
    *data = gpio_cfg.data;
    return RT_ERR_OK;
}   /* end of rtk_gpio_read */


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
    /* function body */
    rtdrv_gpioCfg_t gpio_cfg;

    osal_memset(&gpio_cfg, 0, sizeof(rtdrv_gpioCfg_t));
    osal_memcpy(&gpio_cfg.gpioId, &gpioId, sizeof(uint32));
    osal_memcpy(&gpio_cfg.data, &data, sizeof(uint32));
	
    SETSOCKOPT(RTDRV_GPIO_DATABIT_SET, &gpio_cfg, rtdrv_gpioCfg_t, 1);
    return RT_ERR_OK;
}   /* end of rtk_gpio_write */


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
   /* function body */
    rtdrv_gpioCfg_t gpio_cfg;

    osal_memset(&gpio_cfg,0x0,sizeof(rtdrv_gpioCfg_t));
    osal_memcpy(&gpio_cfg.intrMode, &intrMode, sizeof(rtk_gpio_intrMode_t));
    osal_memcpy(&gpio_cfg.gpioId, &gpioId, sizeof(uint32));
	
    SETSOCKOPT(RTDRV_GPIO_INTR_SET, &gpio_cfg, rtdrv_gpioCfg_t, 1);
    return RT_ERR_OK;
}   /* end of rtk_gpio_intr_set */


/* Function Name:
 *      rtk_gpio_intr_get
 * Description:
 *     write data to gpio
 * Input:
 *      gpioId - gpio pin number 
 *      pIntrMode -  point of gpio interrupt mode
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
     /* function body */
    rtdrv_gpioCfg_t gpio_cfg;
    osal_memcpy(&gpio_cfg.gpioId, &gpioId, sizeof(uint32));
    GETSOCKOPT(RTDRV_GPIO_INTR_GET, &gpio_cfg, rtdrv_gpioCfg_t, 1);
    *pIntrMode = gpio_cfg.intrMode;
    
    return RT_ERR_OK;
}   /* end of rtk_gpio_intr_get */


/* Function Name:
 *      rtk_gpio_intrStatus_clean
 * Description:
 *     clean  gpio interrupt status
 * Input:
 *      gpioId - gpio pin id for interrupt status get
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
   /* function body */
    rtdrv_gpioCfg_t gpio_cfg;
    osal_memset(&gpio_cfg,0x0,sizeof(rtdrv_gpioCfg_t));
    osal_memcpy(&gpio_cfg.gpioId, &gpioId, sizeof(uint32));
	
    SETSOCKOPT(RTDRV_GPIO_IMS_SET, &gpio_cfg, rtdrv_gpioCfg_t, 1);
    return RT_ERR_OK;
}   /* end of rtk_gpio_intr_set */


/* Function Name:
 *      rtk_gpio_intr_get
 * Description:
 *     write data to gpio
 * Input:
 *      gpioId - gpio pin number 
 *      pIntrMode -  point of gpio interrupt mode
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
     /* function body */
    rtdrv_gpioCfg_t gpio_cfg;
    osal_memcpy(&gpio_cfg.gpioId, &gpioId, sizeof(uint32));
    GETSOCKOPT(RTDRV_GPIO_IMS_GET, &gpio_cfg, rtdrv_gpioCfg_t, 1);
    *pState = gpio_cfg.enable;
    
    return RT_ERR_OK;
}   /* end of rtk_gpio_intr_get */

