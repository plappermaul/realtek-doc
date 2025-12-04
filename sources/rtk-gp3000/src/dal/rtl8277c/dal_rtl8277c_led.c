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
 * $Revision: 43348 $
 * $Date: 2013-10-02 11:32:20 +0800 (?üÊ?‰∏? 02 ?ÅÊ? 2013) $
 *
 * Purpose : Definition of Security API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) attack prevention 
 */


/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#if !defined(CONFIG_LEDS_RTK_GPIO_LED) && defined(CONFIG_SDK_KERNEL_LINUX)
#include <linux/leds-ca77xx.h>
#endif
#include <linux/kmod.h>

#include <hal/chipdef/rtl8277c/rtl8277c_def.h>
#include <dal/rtl8277c/dal_rtl8277c.h>
#include <dal/rtl8277c/dal_rtl8277c_led.h>
#include <rtk/led.h>

#if defined(CONFIG_LEDS_RTK_GPIO_LED) && defined(CONFIG_SDK_KERNEL_LINUX)
#include <../drivers/soc/realtek/led-generic.h>
extern void rtk_led_sw_on(int gpio_led_num , int op);
extern void rtk_led_sw_blink(int gpio_led_num , int interval);
#endif

/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */
static uint32 led_init = {INIT_COMPLETED};

static uint32 g_blink_rate = 512;

static rtk_led_force_mode_t g_led_modeForce[RTL8277C_LED_INDEX_MAX] =	{LED_FORCE_OFF, LED_FORCE_OFF, LED_FORCE_OFF, LED_FORCE_OFF,
									LED_FORCE_OFF, LED_FORCE_OFF, LED_FORCE_OFF, LED_FORCE_OFF,
									LED_FORCE_OFF, LED_FORCE_OFF, LED_FORCE_OFF, LED_FORCE_OFF,
									LED_FORCE_OFF, LED_FORCE_OFF, LED_FORCE_OFF, LED_FORCE_OFF};

/*
 * Function Declaration
 */

/* Function Name:
 *      dal_rtl8277c_led_init
 * Description:
 *      Initialize led module.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize led module before calling any led APIs.
 */
int32
dal_rtl8277c_led_init(void)
{
    int32 ledIdx;
    
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_LED),"%s",__FUNCTION__);

    led_init = INIT_COMPLETED;

    for(ledIdx=0;ledIdx<RTL8277C_LED_INDEX_MAX;ledIdx++)
    {
        g_led_modeForce[ledIdx] = LED_FORCE_OFF;
    }

    return RT_ERR_OK;
} /* end of dal_rtl8277c_led_init */


/* Function Name:
 *      dal_rtl8277c_led_blinkRate_get
 * Description:
 *      Get LED blinking rate
 * Input:
 *      group      - led blinking group
 * Output:
 *      pBlinkRate - blinking rate.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT 		- Invalid input parameters.
 * Note:
 *      There are 8 types of LED blinking rates at 32ms, 48ms, 64ms, 96ms, 128ms, 256ms, 512ms, and 1024ms.
 */
int32 
dal_rtl8277c_led_blinkRate_get(rtk_led_blinkGroup_t group, rtk_led_blink_rate_t *pBlinkRate)
{
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_LED),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(led_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pBlinkRate), RT_ERR_NULL_POINTER);

    /* function body */
    switch(group)
    {
        case LED_BLINK_GROUP_FORCE_MODE:
            break;
        default:
            return RT_ERR_INPUT;
            break;
    }
    switch(g_blink_rate)
    {
        case 32:
            *pBlinkRate = LED_BLINKRATE_32MS;
            break;
        case 64:
            *pBlinkRate = LED_BLINKRATE_64MS;
            break;
        case 128:
            *pBlinkRate = LED_BLINKRATE_128MS;
            break;
        case 256:
            *pBlinkRate = LED_BLINKRATE_256MS;
            break;
        case 512:
            *pBlinkRate = LED_BLINKRATE_512MS;
            break;
        case 1024:
            *pBlinkRate = LED_BLINKRATE_1024MS;
            break;
        case 48:
            *pBlinkRate = LED_BLINKRATE_48MS;
            break;
        case 96:
            *pBlinkRate = LED_BLINKRATE_96MS;
            break;
        default:
            return RT_ERR_INPUT;
            break;     
    }

    return RT_ERR_OK;
} /* end of dal_rtl8277c_led_blinkRate_get */

/* Function Name:
 *      dal_rtl8277c_led_blinkRate_set
 * Description:
 *      Get LED blinking rate
 * Input:
 *      group      - led blinking group
 *      blinkRate  - blinking rate.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT 		- Invalid input parameters.
 * Note:
 *      There are 8 types of LED blinking rates at 32ms, 48ms, 64ms, 96ms, 128ms, 256ms, 512ms, and 1024ms.
 */
int32 
dal_rtl8277c_led_blinkRate_set(rtk_led_blinkGroup_t group, rtk_led_blink_rate_t blinkRate)
{
    int32 ledIdx;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_LED),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(led_init);

    /* parameter check */
    
    /* function body */
    switch(group)
    {
        case LED_BLINK_GROUP_FORCE_MODE:
            break;
        default:
            return RT_ERR_INPUT;
            break;
    }

    switch(blinkRate)
    {
        case LED_BLINKRATE_32MS:
            g_blink_rate = 32;
            break;
        case LED_BLINKRATE_64MS:
            g_blink_rate = 64;
            break;
        case LED_BLINKRATE_128MS:
            g_blink_rate = 128;
            break;
        case LED_BLINKRATE_256MS:
            g_blink_rate = 256;
            break;
        case LED_BLINKRATE_512MS:
            g_blink_rate = 512;
            break;
        case LED_BLINKRATE_1024MS:
            g_blink_rate = 1024;
            break;
        case LED_BLINKRATE_48MS:
            g_blink_rate = 48;
            break;
        case LED_BLINKRATE_96MS:
            g_blink_rate = 96;
            break;
        default:
            return RT_ERR_INPUT;
            break;     
    }

    for(ledIdx=0;ledIdx<RTL8277C_LED_INDEX_MAX;ledIdx++)
    {
        if(g_led_modeForce[ledIdx] == LED_FORCE_BLINK)
        {
#if defined(CONFIG_LEDS_RTK_GPIO_LED) && defined(CONFIG_SDK_KERNEL_LINUX)
            rtk_led_sw_blink(ledIdx, HZ/(1024/(g_blink_rate/2)));
#endif
        }
    }

    return RT_ERR_OK;
} /* end of dal_rtl8277c_led_blinkRate_set */

/* Function Name:
 *      dal_rtl8277c_led_modeForce_get
 * Description:
 *      Get Led group to congiuration force mode
 * Input:
 *      ledIdx - LED index id.
 * Output:
 *      pMode  - LED force mode.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT 		- Invalid input parameters.
 * Note:
 *      The API can get forced Led group mode.
 *      The force modes that can be set are as following:
 *      - LED_FORCE_BLINK,
 *      - LED_FORCE_OFF,
 *      - LED_FORCE_ON.
 */
int32
dal_rtl8277c_led_modeForce_get(uint32 ledIdx, rtk_led_force_mode_t *pMode)
{    
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_LED),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(led_init);

    /* parameter check */
    RT_PARAM_CHK((RTL8277C_LED_INDEX_MAX < ledIdx), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pMode), RT_ERR_NULL_POINTER);
    
    /* function body */
    *pMode = g_led_modeForce[ledIdx];

    return RT_ERR_OK;
} /* end of dal_rtl8277c_led_modeForce_get */

/* Function Name:
 *      dal_rtl8277c_led_modeForce_set
 * Description:
 *      Set Led group to congiuration force mode
 * Input:
 *      ledIdx - LED index id.
 *      mode  - LED force mode.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT 		- Invalid input parameters.
 * Note:
 *      The API can get forced Led group mode.
 *      The force modes that can be set are as following:
 *      - LED_FORCE_NORMAL,
 *      - LED_FORCE_BLINK,
 *      - LED_FORCE_OFF,
 *      - LED_FORCE_ON.
 */
int32
dal_rtl8277c_led_modeForce_set(uint32 ledIdx, rtk_led_force_mode_t mode)
{

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_LED),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(led_init);

    /* parameter check */
    RT_PARAM_CHK((RTL8277C_LED_INDEX_MAX < ledIdx), RT_ERR_INPUT);

#if defined(CONFIG_LEDS_RTK_GPIO_LED) && defined(CONFIG_SDK_KERNEL_LINUX)
    switch(ledIdx)
    {
        case LED_PON: //fallthrough
        case LED_LOS:
            break;
        default:
            return RT_ERR_INPUT;
            break;
    }
#endif

    /* function body */
    switch(mode)
    {
        case LED_FORCE_BLINK:
#if defined(CONFIG_LEDS_RTK_GPIO_LED) && defined(CONFIG_SDK_KERNEL_LINUX)
            rtk_led_sw_blink(ledIdx, HZ/(1024/(g_blink_rate/2)));
#endif
            break;
        case LED_FORCE_OFF:
#if defined(CONFIG_LEDS_RTK_GPIO_LED) && defined(CONFIG_SDK_KERNEL_LINUX)
            rtk_led_sw_on(ledIdx, LED_OFF);
#endif
            break;
        case LED_FORCE_ON:
#if defined(CONFIG_LEDS_RTK_GPIO_LED) && defined(CONFIG_SDK_KERNEL_LINUX)
	        rtk_led_sw_on(ledIdx, LED_ON);
#endif
            break;
        default:
            return RT_ERR_INPUT;
            break;
    }
    
    g_led_modeForce[ledIdx] = mode;

    return RT_ERR_OK;
} /* end of dal_rtl8277c_led_modeForce_set */
