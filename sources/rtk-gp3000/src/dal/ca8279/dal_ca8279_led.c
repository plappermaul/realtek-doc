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
 * $Date: 2013-10-02 11:32:20 +0800 (星期三, 02 十月 2013) $
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
#include <linux/leds-ca77xx.h>
#include <linux/kmod.h>

#include <hal/chipdef/ca8279/ca8279_def.h>
#include <dal/ca8279/dal_ca8279.h>
#include <dal/ca8279/dal_ca8279_led.h>
#include <rtk/led.h>

#if defined(CONFIG_LEDS_RTK_GPIO_LED) && defined(CONFIG_SDK_KERNEL_LINUX)
#include <../drivers/soc/realtek/led-generic.h>
#include <rtk/pon_led_profile.h>
extern void rtk_led_sw_on(int gpio_led_num , int op);
extern void rtk_led_sw_blink(int gpio_led_num , int interval);
#endif

#ifndef CONFIG_LEDS_RTK_GPIO_LED
#include <linux/mutex.h>
#endif

/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */
static uint32 led_init = {INIT_COMPLETED};

static uint32 g_blink_rate = 512;

static rtk_led_force_mode_t g_led_modeForce[CA8279_LED_INDEX_MAX];

#ifndef CONFIG_LEDS_RTK_GPIO_LED
static struct mutex led_sem;
#endif

/*
 * Function Declaration
 */

#ifndef CONFIG_LEDS_RTK_GPIO_LED
static int32
_dal_ca8279_call_user_cmd(char *cmd)
{
    int ret = 0;
    char *envp[] = {NULL};
    char *argv[] = {"/bin/bash", "-c", cmd ,NULL};

    if((ret = call_usermodehelper(argv[0], argv, envp, UMH_WAIT_PROC))!=0)
    {
        RT_ERR(ret, (MOD_DAL|MOD_LED), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

static int32
_dal_ca8279_led_blink(uint32 ledIdx)
{
    int ret = 0;
    char cmd1[128];
    char cmd2[128];
    char cmd3[128];
    char cmd4[128];
    char cmd5[128];

    sprintf(cmd1,"/bin/echo 1 > /sys/class/leds/green\:led%d/hw_enable",ledIdx);
    sprintf(cmd2,"/bin/echo sw > /sys/class/leds/green\:led%d/hw_event",ledIdx);
    sprintf(cmd3,"/bin/echo timer > /sys/class/leds/green\:led%d/trigger",ledIdx);
    sprintf(cmd4,"/bin/echo %d > /sys/class/leds/green\:led%d/delay_on",g_blink_rate,ledIdx);
    sprintf(cmd5,"/bin/echo %d > /sys/class/leds/green\:led%d/delay_off",g_blink_rate,ledIdx);

    if((ret = _dal_ca8279_call_user_cmd(cmd1))!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_LED), "");
        return RT_ERR_FAILED;
    }

    if((ret = _dal_ca8279_call_user_cmd(cmd2))!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_LED), "");
        return RT_ERR_FAILED;
    }

    if((ret = _dal_ca8279_call_user_cmd(cmd3))!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_LED), "");
        return RT_ERR_FAILED;
    }

    if((ret = _dal_ca8279_call_user_cmd(cmd4))!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_LED), "");
        return RT_ERR_FAILED;
    }
    
    if((ret = _dal_ca8279_call_user_cmd(cmd5))!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_LED), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

static int32
_dal_ca8279_led_on(uint32 ledIdx)
{
    int ret = 0;
    char cmd1[128];
    char cmd2[128];
    char cmd3[128];
    char cmd4[128];

    sprintf(cmd1,"/bin/echo 1 > /sys/class/leds/green\:led%d/hw_enable",ledIdx);
    sprintf(cmd2,"/bin/echo sw > /sys/class/leds/green\:led%d/hw_event",ledIdx);
    sprintf(cmd3,"/bin/echo none > /sys/class/leds/green\:led%d/trigger",ledIdx);
    sprintf(cmd4,"/bin/echo 1 > /sys/class/leds/green\:led%d/brightness",ledIdx);

    if((ret = _dal_ca8279_call_user_cmd(cmd1))!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_LED), "");
        return RT_ERR_FAILED;
    }

    if((ret = _dal_ca8279_call_user_cmd(cmd2))!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_LED), "");
        return RT_ERR_FAILED;
    }
    
    if((ret = _dal_ca8279_call_user_cmd(cmd3))!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_LED), "");
        return RT_ERR_FAILED;
    }
    
    if((ret = _dal_ca8279_call_user_cmd(cmd4))!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_LED), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

static int32
_dal_ca8279_led_off(uint32 ledIdx)
{
    int32 ret;
    char cmd1[128];
    char cmd2[128];
    char cmd3[128];
    char cmd4[128];

    sprintf(cmd1,"/bin/echo 1 > /sys/class/leds/green\:led%d/hw_enable",ledIdx);
    sprintf(cmd2,"/bin/echo sw > /sys/class/leds/green\:led%d/hw_event",ledIdx);
    sprintf(cmd3,"/bin/echo none > /sys/class/leds/green\:led%d/trigger",ledIdx);
    sprintf(cmd4,"/bin/echo 0 > /sys/class/leds/green\:led%d/brightness",ledIdx);

    if((ret = _dal_ca8279_call_user_cmd(cmd1))!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_LED), "");
        return RT_ERR_FAILED;
    }

    if((ret = _dal_ca8279_call_user_cmd(cmd2))!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_LED), "");
        return RT_ERR_FAILED;
    }

    if((ret = _dal_ca8279_call_user_cmd(cmd3))!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_LED), "");
        return RT_ERR_FAILED;
    }

    if((ret = _dal_ca8279_call_user_cmd(cmd4))!=RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_LED), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}
#endif

/* Function Name:
 *      dal_ca8279_led_init
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
dal_ca8279_led_init(void)
{
    int32 ledIdx;
    
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_LED),"%s",__FUNCTION__);

#ifndef CONFIG_LEDS_RTK_GPIO_LED
    mutex_init(&led_sem);
#endif

    led_init = INIT_COMPLETED;

    for(ledIdx=0;ledIdx<CA8279_LED_INDEX_MAX;ledIdx++)
    {
        g_led_modeForce[ledIdx] = LED_FORCE_OFF;
    }

    return RT_ERR_OK;
} /* end of dal_ca8279_led_init */


/* Function Name:
 *      dal_ca8279_led_blinkRate_get
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
dal_ca8279_led_blinkRate_get(rtk_led_blinkGroup_t group, rtk_led_blink_rate_t *pBlinkRate)
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
} /* end of dal_ca8279_led_blinkRate_get */

/* Function Name:
 *      dal_ca8279_led_blinkRate_set
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
dal_ca8279_led_blinkRate_set(rtk_led_blinkGroup_t group, rtk_led_blink_rate_t blinkRate)
{
#ifndef CONFIG_LEDS_RTK_GPIO_LED
    int32 ret;
#endif
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

    for(ledIdx=0;ledIdx<CA8279_LED_INDEX_MAX;ledIdx++)
    {
        if(g_led_modeForce[ledIdx] == LED_FORCE_BLINK)
        {
#if defined(CONFIG_LEDS_RTK_GPIO_LED) && defined(CONFIG_SDK_KERNEL_LINUX)
            rtk_led_sw_blink(ledIdx, HZ/(1024/(g_blink_rate/2)));
#else
            mutex_lock(&led_sem);

            if((ret = _dal_ca8279_led_blink(ledIdx))!=RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_LED), "");
                mutex_unlock(&led_sem);
                return RT_ERR_FAILED;
            }
            mutex_unlock(&led_sem);
#endif
        }
    }

    return RT_ERR_OK;
} /* end of dal_ca8279_led_blinkRate_set */

/* Function Name:
 *      dal_ca8279_led_modeForce_get
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
dal_ca8279_led_modeForce_get(uint32 ledIdx, rtk_led_force_mode_t *pMode)
{    
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_LED),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(led_init);

    /* parameter check */
    RT_PARAM_CHK((CA8279_LED_INDEX_MAX < ledIdx), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pMode), RT_ERR_NULL_POINTER);
    
    /* function body */
    *pMode = g_led_modeForce[ledIdx];

    return RT_ERR_OK;
} /* end of dal_ca8279_led_modeForce_get */

/* Function Name:
 *      dal_ca8279_led_modeForce_set
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
dal_ca8279_led_modeForce_set(uint32 ledIdx, rtk_led_force_mode_t mode)
{
#if defined(CONFIG_LEDS_RTK_GPIO_LED) && defined(CONFIG_SDK_KERNEL_LINUX)
    uint32 ledIdxMapped;
#else
    int32 ret;
#endif

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_LED),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(led_init);

    /* parameter check */
    RT_PARAM_CHK((CA8279_LED_INDEX_MAX < ledIdx), RT_ERR_INPUT);

#if defined(CONFIG_LEDS_RTK_GPIO_LED) && defined(CONFIG_SDK_KERNEL_LINUX)
    switch(ledIdx)
    {
        case PON_LED_GROUP_PON_STATUS:
            ledIdxMapped = LED_PON;
            break;
        case PON_LED_GROUP_LOS:
            ledIdxMapped = LED_LOS;
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
            rtk_led_sw_blink(ledIdxMapped, HZ/(1024/(g_blink_rate/2)));
#else
            mutex_lock(&led_sem);

            if((ret = _dal_ca8279_led_blink(ledIdx))!=RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_LED), "");
                mutex_unlock(&led_sem);
                return RT_ERR_FAILED;
            }
            mutex_unlock(&led_sem);

#endif
            break;
        case LED_FORCE_OFF:
#if defined(CONFIG_LEDS_RTK_GPIO_LED) && defined(CONFIG_SDK_KERNEL_LINUX)
            rtk_led_sw_on(ledIdxMapped, LED_OFF);
#else
            mutex_lock(&led_sem);

            if((ret = _dal_ca8279_led_off(ledIdx))!=RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_LED), "");
                mutex_unlock(&led_sem);
                return RT_ERR_FAILED;
            }
            mutex_unlock(&led_sem);

#endif
            break;
        case LED_FORCE_ON:
#if defined(CONFIG_LEDS_RTK_GPIO_LED) && defined(CONFIG_SDK_KERNEL_LINUX)
	      rtk_led_sw_on(ledIdxMapped, LED_ON);
#else
            mutex_lock(&led_sem);

            if((ret = _dal_ca8279_led_on(ledIdx))!=RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_LED), "");
                mutex_unlock(&led_sem);
                return RT_ERR_FAILED;
            }
            mutex_unlock(&led_sem);

#endif
            break;
        default:
            return RT_ERR_INPUT;
            break;
    }
    
    g_led_modeForce[ledIdx] = mode;

    return RT_ERR_OK;
} /* end of dal_ca8279_led_modeForce_set */
