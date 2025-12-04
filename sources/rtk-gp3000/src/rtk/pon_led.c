/*
 * Copyright (C) 2014 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 51563 $
 * $Date: 2014-09-23 10:56:10 +0800 (Tue, 23 Sep 2014) $
 *
 * Purpose : Definition those public PON LED APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *           (1) PON LED settings
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/error.h>
#include <rtk/pon_led.h>
#include <rtk/pon_led_profile.h>
#include <rtk/led.h>
#include <rtk/ponmac.h>
#include <osal/lib.h>
#include <osal/print.h>
#ifdef CONFIG_SDK_KERNEL_LINUX
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/version.h>
#endif

/*
 * Symbol Definition
 */
/*
 * Data Declaration
 */
#ifdef CONFIG_CU
static rtk_pon_led_spec_type_t pon_led_spec_type = 4;
#else
static rtk_pon_led_spec_type_t pon_led_spec_type = 0;
#endif

#ifdef CONFIG_SDK_KERNEL_LINUX
static struct task_struct *pLedBlink;
#endif
#ifdef PON_LED_GROUP_PON_STATUS
uint32 ponStatusLed = PON_LED_GROUP_PON_STATUS;
#endif
#ifdef PON_LED_GROUP_LOS
uint32 losLed = PON_LED_GROUP_LOS;
#endif
#ifdef PON_LED_GROUP_SYSTEM
uint32 systemLed = PON_LED_GROUP_SYSTEM;
#endif

/*
 * Function Declaration
 */
static int pon_led_gpon_spec_type_00_init(void)
{
#ifdef PON_LED_GROUP_PON_STATUS
{
    int ret;
    rtk_led_config_t ledConfig;
    rtk_led_force_mode_t forceMode;

    if((ret = rtk_led_parallelEnable_set(ponStatusLed, ENABLED)) != RT_ERR_OK)
        return ret;

    osal_memset(&ledConfig,0x0,sizeof(rtk_led_config_t));
    ledConfig.ledEnable[LED_CONFIG_FORCE_MODE] = 1;
    if((ret = rtk_led_config_set(ponStatusLed, LED_TYPE_PON, &ledConfig)) != RT_ERR_OK)
        return ret;

    forceMode = LED_FORCE_OFF;
    PON_LED_PON_STATUS_FORCEMODE_SET(forceMode);
}
#endif
    return RT_ERR_OK;
}

static int32 pon_led_gpon_spec_type_00(rtk_pon_led_pon_state_t ponStatus)
{
#ifdef CONFIG_SDK_KERNEL_LINUX
    static char init = 0;
    static rtk_pon_led_force_mode_t forceModeState = PON_LED_FORCE_MODE_NONE;
    static rtk_led_force_mode_t forceMode = LED_FORCE_OFF;
    static rtk_led_blink_rate_t rate = LED_BLINKRATE_32MS;
    int32 ret = RT_ERR_OK;

    RT_PARAM_CHK((PON_LED_STATE_END <= ponStatus), RT_ERR_INPUT);

    /* Init all used LED groups */
    if(0 == init)
    {
        ret = pon_led_gpon_spec_type_00_init();
        if(ret)
            return ret;

        init = 1;
    }

    switch(ponStatus)
    {
    case PON_LED_FORCE_OFF:
        forceModeState = PON_LED_FORCE_MODE_OFF;
        PON_LED_PON_STATUS_FORCEMODE_SET(LED_FORCE_OFF);
        break;
    case PON_LED_FORCE_ON:
        forceModeState = PON_LED_FORCE_MODE_ON;
        PON_LED_PON_STATUS_FORCEMODE_SET(LED_FORCE_ON);
        break;
    case PON_LED_STATE_RESTORE:
        forceModeState = PON_LED_FORCE_MODE_NONE;
        PON_LED_PON_STATUS_FORCEMODE_SET(forceMode);
        if((forceMode == LED_FORCE_BLINK || forceMode == LED_FORCE_BLINK_GROUP2) && (RT_ERR_OK != rtk_led_blinkRate_set(LED_BLINK_GROUP_FORCE_MODE, rate)))
        {
            return ret;
        }
        break;
    case PON_LED_STATE_O1:
        forceMode = LED_FORCE_OFF;
        if(PON_LED_FORCE_MODE_NONE == forceModeState)
        {
            PON_LED_PON_STATUS_FORCEMODE_SET(forceMode);
        }
        break;
    case PON_LED_STATE_O5:
        forceMode = LED_FORCE_ON;
        if(PON_LED_FORCE_MODE_NONE == forceModeState)
        {
            PON_LED_PON_STATUS_FORCEMODE_SET(forceMode);
        }
        break;
    case PON_LED_STATE_O2:
    case PON_LED_STATE_O3:
    case PON_LED_STATE_O4:
        forceMode = LED_FORCE_BLINK;
        rate = LED_BLINKRATE_512MS;
        if(PON_LED_FORCE_MODE_NONE == forceModeState)
        {
            PON_LED_PON_STATUS_FORCEMODE_SET(forceMode);
            if(RT_ERR_OK != rtk_led_blinkRate_set(LED_BLINK_GROUP_FORCE_MODE, rate))
            {
                return ret;
            }
        }
        break;
    default:
        /* Don't know what to do for unknow case, don't do anything */
        break;
    }

    return ret;
#else
    ponStatus = ponStatus;

    return RT_ERR_OK;
#endif
}

static int pon_led_epon_spec_type_00_init(void)
{
#ifdef PON_LED_GROUP_PON_STATUS
{
    int ret;
    rtk_led_config_t ledConfig;
    rtk_led_force_mode_t forceMode;

    if((ret = rtk_led_parallelEnable_set(ponStatusLed, ENABLED)) != RT_ERR_OK)
        return ret;

    osal_memset(&ledConfig,0x0,sizeof(rtk_led_config_t));
    ledConfig.ledEnable[LED_CONFIG_FORCE_MODE] = 1;

    if((ret = rtk_led_config_set(ponStatusLed, LED_TYPE_PON, &ledConfig)) != RT_ERR_OK)
        return ret;

    forceMode = LED_FORCE_OFF;
    PON_LED_PON_STATUS_FORCEMODE_SET(forceMode);
}
#endif
    return RT_ERR_OK;
}

static int32 pon_led_epon_spec_type_00(rtk_pon_led_pon_state_t ponStatus)
{
#ifdef CONFIG_SDK_KERNEL_LINUX
    static char init = 0;
    static rtk_pon_led_force_mode_t forceModeState = PON_LED_FORCE_MODE_NONE;
    static rtk_led_force_mode_t forceMode = LED_FORCE_OFF;
    static rtk_led_blink_rate_t rate = LED_BLINKRATE_32MS;
    int32 ret = RT_ERR_OK;

    RT_PARAM_CHK((PON_LED_STATE_END <= ponStatus), RT_ERR_INPUT);

    /* Init all used LED groups */
    if(0 == init)
    {
        ret = pon_led_epon_spec_type_00_init();
        if(ret)
            return ret;

        init = 1;
    }

    switch(ponStatus)
    {
    case PON_LED_FORCE_OFF:
        forceModeState = PON_LED_FORCE_MODE_OFF;
        PON_LED_PON_STATUS_FORCEMODE_SET(LED_FORCE_OFF);
        break;
    case PON_LED_FORCE_ON:
        forceModeState = PON_LED_FORCE_MODE_ON;
        PON_LED_PON_STATUS_FORCEMODE_SET(LED_FORCE_ON);
        break;
    case PON_LED_STATE_RESTORE:
        forceModeState = PON_LED_FORCE_MODE_NONE;
        PON_LED_PON_STATUS_FORCEMODE_SET(forceMode);
        if((forceMode == LED_FORCE_BLINK || forceMode == LED_FORCE_BLINK_GROUP2) && (RT_ERR_OK != rtk_led_blinkRate_set(LED_BLINK_GROUP_FORCE_MODE, rate)))
        {
            return ret;
        }
        break;
    case PON_LED_STATE_EPONMAC_DOWN:
    case PON_LED_STATE_EPONOAM_DOWN:
        forceMode = LED_FORCE_OFF;
        if(PON_LED_FORCE_MODE_NONE == forceModeState)
        {
            PON_LED_PON_STATUS_FORCEMODE_SET(forceMode);
        }
        break;
    case PON_LED_STATE_EPONMAC_UP:
    case PON_LED_STATE_EPONOAM_UP:
        forceMode = LED_FORCE_ON;
        if(PON_LED_FORCE_MODE_NONE == forceModeState)
        {
            PON_LED_PON_STATUS_FORCEMODE_SET(forceMode);
        }
        break;
    case PON_LED_STATE_EPONOAM_TRYING:
        forceMode = LED_FORCE_BLINK;
        rate = LED_BLINKRATE_512MS;
        if(PON_LED_FORCE_MODE_NONE == forceModeState)
        {
            PON_LED_PON_STATUS_FORCEMODE_SET(forceMode);
            if(RT_ERR_OK != rtk_led_blinkRate_set(LED_BLINK_GROUP_FORCE_MODE, rate))
            {
                return ret;
            }
        }
        break;
    default:
        /* Don't know what to do for unknow case, don't do anything */
        break;
    }

    return ret;
#else
    ponStatus = ponStatus;

    return RT_ERR_OK;
#endif
}

static int pon_led_others_spec_type_00_init(void)
{
#ifdef PON_LED_GROUP_LOS
{
    int ret;
    rtk_led_config_t ledConfig;
    rtk_led_force_mode_t forceMode;

    if((ret = rtk_led_parallelEnable_set(losLed, ENABLED)) != RT_ERR_OK)
        return ret;

    osal_memset(&ledConfig,0x0,sizeof(rtk_led_config_t));
    ledConfig.ledEnable[LED_CONFIG_FORCE_MODE] = 1;
    if((ret = rtk_led_config_set(losLed, LED_TYPE_PON, &ledConfig)) != RT_ERR_OK)
        return ret;

    forceMode = LED_FORCE_OFF;
    PON_LED_LOS_FORCEMODE_SET(forceMode);
}
#endif
    return RT_ERR_OK;
}

static int32 pon_led_others_spec_type_00(rtk_pon_led_pon_state_t ponStatus)
{
#ifdef CONFIG_SDK_KERNEL_LINUX
    static char init = 0;
    static rtk_pon_led_force_mode_t forceModeState = PON_LED_FORCE_MODE_NONE;
    static rtk_led_force_mode_t forceMode = LED_FORCE_OFF;
    static rtk_led_blink_rate_t rate = LED_BLINKRATE_32MS;
    int32 ret = RT_ERR_OK;

    RT_PARAM_CHK((PON_LED_STATE_END <= ponStatus), RT_ERR_INPUT);

    /* Init all used LED groups */
    if(0 == init)
    {
        ret = pon_led_others_spec_type_00_init();
        if(ret)
            return ret;

        init = 1;
    }

    switch(ponStatus)
    {
    case PON_LED_FORCE_OFF:
        forceModeState = PON_LED_FORCE_MODE_OFF;
        PON_LED_LOS_FORCEMODE_SET(LED_FORCE_OFF);
        break;
    case PON_LED_FORCE_ON:
        forceModeState = PON_LED_FORCE_MODE_ON;
        PON_LED_LOS_FORCEMODE_SET(LED_FORCE_ON);
        break;
    case PON_LED_STATE_RESTORE:
        forceModeState = PON_LED_FORCE_MODE_NONE;
        PON_LED_LOS_FORCEMODE_SET(forceMode);
        if((forceMode == LED_FORCE_BLINK || forceMode == LED_FORCE_BLINK_GROUP2) && (RT_ERR_OK != rtk_led_blinkRate_set(LED_BLINK_GROUP_FORCE_MODE, rate)))
        {
            return ret;
        }
        break;
    case PON_LED_STATE_LOS_OCCURRED:
        forceMode = LED_FORCE_BLINK;
        rate = LED_BLINKRATE_256MS;
        if(PON_LED_FORCE_MODE_NONE == forceModeState)
        {
            PON_LED_LOS_FORCEMODE_SET(forceMode);
            if(RT_ERR_OK != rtk_led_blinkRate_set(LED_BLINK_GROUP_FORCE_MODE, rate))
            {
                return ret;
            }
        }
        break;
    case PON_LED_STATE_LOS_CLEARED:
        forceMode = LED_FORCE_OFF;
        if(PON_LED_FORCE_MODE_NONE == forceModeState)
        {
            PON_LED_LOS_FORCEMODE_SET(forceMode);
        }
        break;
    default:
        /* Don't know what to do for unknow case, don't do anything */
        break;
    }

    return ret;
#else
    ponStatus = ponStatus;

    return RT_ERR_OK;
#endif
}

static int pon_led_gpon_spec_type_01_init(void)
{
#ifdef PON_LED_GROUP_PON_STATUS
{
    int ret;
    rtk_led_config_t ledConfig;
    rtk_led_force_mode_t forceMode;

    if((ret = rtk_led_parallelEnable_set(ponStatusLed, ENABLED)) != RT_ERR_OK)
        return ret;

    osal_memset(&ledConfig,0x0,sizeof(rtk_led_config_t));
    ledConfig.ledEnable[LED_CONFIG_FORCE_MODE] = 1;
    if((ret = rtk_led_config_set(ponStatusLed, LED_TYPE_PON, &ledConfig)) != RT_ERR_OK)
        return ret;

    forceMode = LED_FORCE_OFF;
    PON_LED_PON_STATUS_FORCEMODE_SET(forceMode);
}
#endif
    return RT_ERR_OK;
}

static int32 pon_led_gpon_spec_type_01(rtk_pon_led_pon_state_t ponStatus)
{
#ifdef CONFIG_SDK_KERNEL_LINUX
    static char init = 0;
    static rtk_pon_led_force_mode_t forceModeState = PON_LED_FORCE_MODE_NONE;
    static rtk_led_force_mode_t forceMode = LED_FORCE_OFF;
    static rtk_led_blink_rate_t rate = LED_BLINKRATE_32MS;
    int32 ret = RT_ERR_OK;

    RT_PARAM_CHK((PON_LED_STATE_END <= ponStatus), RT_ERR_INPUT);

    /* Init all used LED groups */
    if(0 == init)
    {
        ret = pon_led_gpon_spec_type_01_init();
        if(ret)
            return ret;

        init = 1;
    }

    switch(ponStatus)
    {
    case PON_LED_FORCE_OFF:
        forceModeState = PON_LED_FORCE_MODE_OFF;
        PON_LED_PON_STATUS_FORCEMODE_SET(LED_FORCE_OFF);
        break;
    case PON_LED_FORCE_ON:
        forceModeState = PON_LED_FORCE_MODE_ON;
        PON_LED_PON_STATUS_FORCEMODE_SET(LED_FORCE_ON);
        break;
    case PON_LED_STATE_RESTORE:
        forceModeState = PON_LED_FORCE_MODE_NONE;
        PON_LED_PON_STATUS_FORCEMODE_SET(forceMode);
        if((forceMode == LED_FORCE_BLINK || forceMode == LED_FORCE_BLINK_GROUP2) && (RT_ERR_OK != rtk_led_blinkRate_set(LED_BLINK_GROUP_FORCE_MODE, rate)))
        {
            return ret;
        }
        break;
    case PON_LED_STATE_O1:
        forceMode = LED_FORCE_OFF;
        if(PON_LED_FORCE_MODE_NONE == forceModeState)
        {
            PON_LED_PON_STATUS_FORCEMODE_SET(forceMode);
        }
        break;
    case PON_LED_STATE_O5:
        forceMode = LED_FORCE_ON;
        if(PON_LED_FORCE_MODE_NONE == forceModeState)
        {
            PON_LED_PON_STATUS_FORCEMODE_SET(forceMode);
        }
        break;
    case PON_LED_STATE_O2:
    case PON_LED_STATE_O3:
    case PON_LED_STATE_O4:
        forceMode = LED_FORCE_BLINK;
        rate = LED_BLINKRATE_512MS;
        if(PON_LED_FORCE_MODE_NONE == forceModeState)
        {
            PON_LED_PON_STATUS_FORCEMODE_SET(forceMode);
            if(RT_ERR_OK != rtk_led_blinkRate_set(LED_BLINK_GROUP_FORCE_MODE, rate))
            {
                return ret;
            }
        }
        break;
    default:
        /* Don't know what to do for unknow case, don't do anything */
        break;
    }

    return ret;
#else
    ponStatus = ponStatus;

    return RT_ERR_OK;
#endif
}

static int pon_led_epon_spec_type_01_init(void)
{
#ifdef PON_LED_GROUP_PON_STATUS
{
    int ret;
    rtk_led_config_t ledConfig;
    rtk_led_force_mode_t forceMode;

    if((ret = rtk_led_parallelEnable_set(ponStatusLed, ENABLED)) != RT_ERR_OK)
        return ret;

    osal_memset(&ledConfig,0x0,sizeof(rtk_led_config_t));
    ledConfig.ledEnable[LED_CONFIG_FORCE_MODE] = 1;
    if((ret = rtk_led_config_set(ponStatusLed, LED_TYPE_PON, &ledConfig)) != RT_ERR_OK)
        return ret;

    forceMode = LED_FORCE_OFF;
    PON_LED_PON_STATUS_FORCEMODE_SET(forceMode);
}
#endif
    return RT_ERR_OK;
}

static int32 pon_led_epon_spec_type_01(rtk_pon_led_pon_state_t ponStatus)
{
#ifdef CONFIG_SDK_KERNEL_LINUX
    static char init = 0;
    static rtk_pon_led_force_mode_t forceModeState = PON_LED_FORCE_MODE_NONE;
    static rtk_led_force_mode_t forceMode = LED_FORCE_OFF;
    static rtk_led_blink_rate_t rate = LED_BLINKRATE_32MS;
    int32 ret = RT_ERR_OK;

    RT_PARAM_CHK((PON_LED_STATE_END <= ponStatus), RT_ERR_INPUT);

    /* Init all used LED groups */
    if(0 == init)
    {
        ret = pon_led_epon_spec_type_01_init();
        if(ret)
            return ret;

        init = 1;
    }

    switch(ponStatus)
    {
    case PON_LED_FORCE_OFF:
        forceModeState = PON_LED_FORCE_MODE_OFF;
        PON_LED_PON_STATUS_FORCEMODE_SET(LED_FORCE_OFF);
        break;
    case PON_LED_FORCE_ON:
        forceModeState = PON_LED_FORCE_MODE_ON;
        PON_LED_PON_STATUS_FORCEMODE_SET(LED_FORCE_ON);
        break;
    case PON_LED_STATE_RESTORE:
        forceModeState = PON_LED_FORCE_MODE_NONE;
        PON_LED_PON_STATUS_FORCEMODE_SET(forceMode);
        if((forceMode == LED_FORCE_BLINK || forceMode == LED_FORCE_BLINK_GROUP2) && (RT_ERR_OK != rtk_led_blinkRate_set(LED_BLINK_GROUP_FORCE_MODE, rate)))
        {
            return ret;
        }
        break;
    case PON_LED_STATE_EPONOAM_DOWN:
        forceMode = LED_FORCE_OFF;
        if(PON_LED_FORCE_MODE_NONE == forceModeState)
        {
            PON_LED_PON_STATUS_FORCEMODE_SET(forceMode);
        }
        break;
    case PON_LED_STATE_EPONOAM_UP:
        forceMode = LED_FORCE_ON;
        if(PON_LED_FORCE_MODE_NONE == forceModeState)
        {
            PON_LED_PON_STATUS_FORCEMODE_SET(forceMode);
        }
        break;
    case PON_LED_STATE_EPONOAM_TRYING:
        forceMode = LED_FORCE_BLINK;
        rate = LED_BLINKRATE_512MS;
        if(PON_LED_FORCE_MODE_NONE == forceModeState)
        {
            PON_LED_PON_STATUS_FORCEMODE_SET(forceMode);
            if(RT_ERR_OK != rtk_led_blinkRate_set(LED_BLINK_GROUP_FORCE_MODE, rate))
                return ret;
        }
        break;
    default:
        /* Don't know what to do for unknow case, don't do anything */
        break;
    }

    return ret;
#else
    ponStatus = ponStatus;

    return RT_ERR_OK;
#endif
}

static int pon_led_others_spec_type_01_init(void)
{
#ifdef PON_LED_GROUP_LOS
{
    int ret;
    rtk_led_config_t ledConfig;
    rtk_led_force_mode_t forceMode;

    if((ret = rtk_led_parallelEnable_set(losLed, ENABLED)) != RT_ERR_OK)
        return ret;

    osal_memset(&ledConfig,0x0,sizeof(rtk_led_config_t));
    ledConfig.ledEnable[LED_CONFIG_FORCE_MODE] = 1;
    if((ret = rtk_led_config_set(losLed, LED_TYPE_PON, &ledConfig)) != RT_ERR_OK)
        return ret;

    forceMode = LED_FORCE_OFF;
    PON_LED_LOS_FORCEMODE_SET(forceMode);
}
#endif
#ifdef PON_LED_GROUP_SYSTEM
{
    int ret;
    rtk_led_config_t ledConfig;
    rtk_led_force_mode_t forceMode;

    if((ret = rtk_led_parallelEnable_set(systemLed, ENABLED)) != RT_ERR_OK)
        return ret;

    osal_memset(&ledConfig,0x0,sizeof(rtk_led_config_t));
    ledConfig.ledEnable[LED_CONFIG_FORCE_MODE] = 1;
    if((ret = rtk_led_config_set(systemLed, LED_TYPE_PON, &ledConfig)) != RT_ERR_OK)
        return ret;

    forceMode = LED_FORCE_OFF;
    PON_LED_SYSTEM_FORCEMODE_SET(forceMode);
}
#endif
    return RT_ERR_OK;
}

static int32 pon_led_others_spec_type_01(rtk_pon_led_pon_state_t ponStatus)
{
#ifdef CONFIG_SDK_KERNEL_LINUX
    static char init = 0;
    static char cnt = 0, toggle = 0;
    static rtk_pon_led_force_mode_t forceModeState = PON_LED_FORCE_MODE_NONE;
    static rtk_led_force_mode_t forceModeLos = LED_FORCE_OFF;
    static rtk_led_force_mode_t forceModeSys = LED_FORCE_OFF;
    int32 ret = RT_ERR_OK;

    RT_PARAM_CHK((PON_LED_STATE_END <= ponStatus), RT_ERR_INPUT);

    /* Init all used LED groups */
    if(0 == init)
    {
        ret = pon_led_others_spec_type_01_init();
        if(ret)
            return ret;

        init = 1;
    }

    switch(ponStatus)
    {
    case PON_LED_FORCE_OFF:
        forceModeState = PON_LED_FORCE_MODE_OFF;
        PON_LED_LOS_FORCEMODE_SET(LED_FORCE_OFF);
        PON_LED_SYSTEM_FORCEMODE_SET(LED_FORCE_OFF);
        break;
    case PON_LED_FORCE_ON:
        forceModeState = PON_LED_FORCE_MODE_ON;
        PON_LED_LOS_FORCEMODE_SET(LED_FORCE_ON);
        PON_LED_SYSTEM_FORCEMODE_SET(LED_FORCE_ON);
        break;
    case PON_LED_STATE_RESTORE:
        forceModeState = PON_LED_FORCE_MODE_NONE;
        PON_LED_LOS_FORCEMODE_SET(forceModeLos);
        PON_LED_SYSTEM_FORCEMODE_SET(forceModeSys);
        break;
    case PON_LED_STATE_LOS_OCCURRED:
        forceModeLos = LED_FORCE_ON;
        if(PON_LED_FORCE_MODE_NONE == forceModeState)
        {
            PON_LED_LOS_FORCEMODE_SET(forceModeLos);
        }
        break;
    case PON_LED_STATE_LOS_CLEARED:
        forceModeLos = LED_FORCE_OFF;
        if(PON_LED_FORCE_MODE_NONE == forceModeState)
        {
            PON_LED_LOS_FORCEMODE_SET(forceModeLos);
        }
        break;
    case PON_LED_STATE_TIMER_50MS:
        cnt++;
        if(cnt >= 20) /* blink every second: 20 * 50ms */
        {
            toggle = (toggle + 1) % 2;
            forceModeSys = (toggle == 1) ? LED_FORCE_ON : LED_FORCE_OFF;
            if(PON_LED_FORCE_MODE_NONE == forceModeState)
            {
                PON_LED_SYSTEM_FORCEMODE_SET(forceModeSys);
            }
            cnt = 0;
        }
        break;
    default:
        /* Don't know what to do for unknow case, don't do anything */
        break;
    }

    return ret;
#else
    ponStatus = ponStatus;

    return RT_ERR_OK;
#endif
}

static int pon_led_gpon_spec_type_02_init(void)
{
#ifdef PON_LED_GROUP_PON_STATUS
{
    int ret;
    rtk_led_config_t ledConfig;
    rtk_led_force_mode_t forceMode;

    if((ret = rtk_led_parallelEnable_set(ponStatusLed, ENABLED)) != RT_ERR_OK)
        return ret;

    osal_memset(&ledConfig,0x0,sizeof(rtk_led_config_t));
    ledConfig.ledEnable[LED_CONFIG_FORCE_MODE] = 1;
    if((ret = rtk_led_config_set(ponStatusLed, LED_TYPE_PON, &ledConfig)) != RT_ERR_OK)
        return ret;

    forceMode = LED_FORCE_OFF;
    PON_LED_PON_STATUS_FORCEMODE_SET(forceMode);
}
#endif
    return RT_ERR_OK;
}

static int32 pon_led_gpon_spec_type_02(rtk_pon_led_pon_state_t ponStatus)
{
#ifdef CONFIG_SDK_KERNEL_LINUX
    static char init = 0;
    static rtk_pon_led_pon_state_t authState = PON_LED_STATE_AUTH_OK;
    static rtk_pon_led_force_mode_t forceModeState = PON_LED_FORCE_MODE_NONE;
    static rtk_led_force_mode_t forceMode = LED_FORCE_OFF;
    static rtk_led_blink_rate_t rate = LED_BLINKRATE_32MS;
    int32 ret = RT_ERR_OK;

    RT_PARAM_CHK((PON_LED_STATE_END <= ponStatus), RT_ERR_INPUT);

    /* Init all used LED groups */
    if(0 == init)
    {
        ret = pon_led_gpon_spec_type_02_init();
        if(ret)
            return ret;

        init = 1;
    }

    switch(ponStatus)
    {
    case PON_LED_FORCE_OFF:
        forceModeState = PON_LED_FORCE_MODE_OFF;
        PON_LED_PON_STATUS_FORCEMODE_SET(LED_FORCE_OFF);
        break;
    case PON_LED_FORCE_ON:
        forceModeState = PON_LED_FORCE_MODE_ON;
        PON_LED_PON_STATUS_FORCEMODE_SET(LED_FORCE_ON);
        break;
    case PON_LED_STATE_RESTORE:
        forceModeState = PON_LED_FORCE_MODE_NONE;
        PON_LED_PON_STATUS_FORCEMODE_SET(forceMode);
        if((forceMode == LED_FORCE_BLINK || forceMode == LED_FORCE_BLINK_GROUP2) && (RT_ERR_OK != rtk_led_blinkRate_set(LED_BLINK_GROUP_FORCE_MODE, rate)))
        {
            return ret;
        }
        break;
    case PON_LED_STATE_O1:
    case PON_LED_STATE_O7:
        forceMode = LED_FORCE_OFF;
        if(PON_LED_FORCE_MODE_NONE == forceModeState)
        {
            PON_LED_PON_STATUS_FORCEMODE_SET(forceMode);
        }
        break;
    case PON_LED_STATE_AUTH_OK:
        forceMode = LED_FORCE_ON;
        if(PON_LED_FORCE_MODE_NONE == forceModeState)
        {
            PON_LED_PON_STATUS_FORCEMODE_SET(forceMode);
        }
        authState = PON_LED_STATE_AUTH_OK;
        break;
    case PON_LED_STATE_AUTH_NG:
        forceMode = LED_FORCE_OFF;
        if(PON_LED_FORCE_MODE_NONE == forceModeState)
        {
            PON_LED_PON_STATUS_FORCEMODE_SET(forceMode);
        }
        authState = PON_LED_STATE_AUTH_NG;
        break;
    case PON_LED_STATE_O2:
        if(authState == PON_LED_STATE_AUTH_NG)
        {
            /* OLT command to enter O2 after auth failed
             * If AUTH fail, 
             */
            break;
        }
    case PON_LED_STATE_O4:
    case PON_LED_STATE_O5:
        forceMode = LED_FORCE_BLINK;
        rate = LED_BLINKRATE_512MS;
        if(PON_LED_FORCE_MODE_NONE == forceModeState)
        {
            PON_LED_PON_STATUS_FORCEMODE_SET(forceMode);
            if(RT_ERR_OK != rtk_led_blinkRate_set(LED_BLINK_GROUP_FORCE_MODE, rate))
            {
                return ret;
            }
        }
        break;
    case PON_LED_STATE_O3:
        /* Ignore O3 state for special application */
    default:
        /* Don't know what to do for unknow case, don't do anything */
        break;
    }

    return ret;
#else
    ponStatus = ponStatus;

    return RT_ERR_OK;
#endif
}

static int pon_led_epon_spec_type_02_init(void)
{
#ifdef PON_LED_GROUP_PON_STATUS
{
    int ret;
    rtk_led_config_t ledConfig;
    rtk_led_force_mode_t forceMode;

    if((ret = rtk_led_parallelEnable_set(ponStatusLed, ENABLED)) != RT_ERR_OK)
        return ret;

    osal_memset(&ledConfig,0x0,sizeof(rtk_led_config_t));
    ledConfig.ledEnable[LED_CONFIG_FORCE_MODE] = 1;

    if((ret = rtk_led_config_set(ponStatusLed, LED_TYPE_PON, &ledConfig)) != RT_ERR_OK)
        return ret;

    forceMode = LED_FORCE_OFF;
    PON_LED_PON_STATUS_FORCEMODE_SET(forceMode);
}
#endif
    return RT_ERR_OK;
}

static int32 pon_led_epon_spec_type_02(rtk_pon_led_pon_state_t ponStatus)
{
#ifdef CONFIG_SDK_KERNEL_LINUX
    static char init = 0;
    static rtk_pon_led_force_mode_t forceModeState = PON_LED_FORCE_MODE_NONE;
    static rtk_led_force_mode_t forceMode = LED_FORCE_OFF;
    static rtk_led_blink_rate_t rate = LED_BLINKRATE_32MS;
    int32 ret = RT_ERR_OK;

    RT_PARAM_CHK((PON_LED_STATE_END <= ponStatus), RT_ERR_INPUT);

    /* Init all used LED groups */
    if(0 == init)
    {
        ret = pon_led_epon_spec_type_02_init();
        if(ret)
            return ret;

        init = 1;
    }

    switch(ponStatus)
    {
    case PON_LED_FORCE_OFF:
        forceModeState = PON_LED_FORCE_MODE_OFF;
        PON_LED_PON_STATUS_FORCEMODE_SET(LED_FORCE_OFF);
        break;
    case PON_LED_FORCE_ON:
        forceModeState = PON_LED_FORCE_MODE_ON;
        PON_LED_PON_STATUS_FORCEMODE_SET(LED_FORCE_ON);
        break;
    case PON_LED_STATE_RESTORE:
        forceModeState = PON_LED_FORCE_MODE_NONE;
        PON_LED_PON_STATUS_FORCEMODE_SET(forceMode);
        if((forceMode == LED_FORCE_BLINK || forceMode == LED_FORCE_BLINK_GROUP2) && (RT_ERR_OK != rtk_led_blinkRate_set(LED_BLINK_GROUP_FORCE_MODE, rate)))
        {
            return ret;
        }
        break;
    case PON_LED_STATE_EPONMAC_DOWN:
    case PON_LED_STATE_EPONOAM_DOWN:
        forceMode = LED_FORCE_OFF;
        if(PON_LED_FORCE_MODE_NONE == forceModeState)
        {
            PON_LED_PON_STATUS_FORCEMODE_SET(forceMode);
        }
        break;
    case PON_LED_STATE_EPONMAC_UP:
    case PON_LED_STATE_EPONOAM_UP:
        forceMode = LED_FORCE_ON;
        if(PON_LED_FORCE_MODE_NONE == forceModeState)
        {
            PON_LED_PON_STATUS_FORCEMODE_SET(forceMode);
        }
        break;
    case PON_LED_STATE_EPONOAM_TRYING:
        forceMode = LED_FORCE_BLINK;
        rate = LED_BLINKRATE_512MS;
        if(PON_LED_FORCE_MODE_NONE == forceModeState)
        {
            PON_LED_PON_STATUS_FORCEMODE_SET(forceMode);
            if(RT_ERR_OK != rtk_led_blinkRate_set(LED_BLINK_GROUP_FORCE_MODE, rate))
            {
                return ret;
            }
        }
        break;
    case PON_LED_STATE_AUTH_NG:
        forceMode = LED_FORCE_BLINK;
        rate = LED_BLINKRATE_512MS;
        if(PON_LED_FORCE_MODE_NONE == forceModeState)
        {
            PON_LED_PON_STATUS_FORCEMODE_SET(forceMode);
            if(RT_ERR_OK != rtk_led_blinkRate_set(LED_BLINK_GROUP_FORCE_MODE, rate))
            {
                return ret;
            }
        }
        break;
    case PON_LED_STATE_AUTH_OK:
        forceMode = LED_FORCE_ON;
        if(PON_LED_FORCE_MODE_NONE == forceModeState)
        {
            PON_LED_PON_STATUS_FORCEMODE_SET(forceMode);
        }
        break;
    default:
        /* Don't know what to do for unknow case, don't do anything */
        break;
    }

    return ret;
#else
    ponStatus = ponStatus;

    return RT_ERR_OK;
#endif
}

static int pon_led_others_spec_type_02_init(void)
{
#ifdef PON_LED_GROUP_LOS
{
    int ret;
    rtk_led_config_t ledConfig;
    rtk_led_force_mode_t forceMode;

    if((ret = rtk_led_parallelEnable_set(losLed, ENABLED)) != RT_ERR_OK)
        return ret;

    osal_memset(&ledConfig,0x0,sizeof(rtk_led_config_t));
    ledConfig.ledEnable[LED_CONFIG_FORCE_MODE] = 1;
    if((ret = rtk_led_config_set(losLed, LED_TYPE_PON, &ledConfig)) != RT_ERR_OK)
        return ret;

    forceMode = LED_FORCE_OFF;
    PON_LED_LOS_FORCEMODE_SET(forceMode);
}
#endif
    return RT_ERR_OK;
}

static int32 pon_led_others_spec_type_02(rtk_pon_led_pon_state_t ponStatus)
{
#ifdef CONFIG_SDK_KERNEL_LINUX
    static char init = 0;
    static rtk_pon_led_force_mode_t forceModeState = PON_LED_FORCE_MODE_NONE;
    static rtk_led_force_mode_t forceMode = LED_FORCE_OFF;
    static rtk_led_blink_rate_t rate = LED_BLINKRATE_32MS;
    int32 ret = RT_ERR_OK;

    RT_PARAM_CHK((PON_LED_STATE_END <= ponStatus), RT_ERR_INPUT);

    /* Init all used LED groups */
    if(0 == init)
    {
        ret = pon_led_others_spec_type_00_init();
        if(ret)
            return ret;

        init = 1;
    }

    switch(ponStatus)
    {
    case PON_LED_FORCE_OFF:
        forceModeState = PON_LED_FORCE_MODE_OFF;
        PON_LED_LOS_FORCEMODE_SET(LED_FORCE_OFF);
        break;
    case PON_LED_FORCE_ON:
        forceModeState = PON_LED_FORCE_MODE_ON;
        PON_LED_LOS_FORCEMODE_SET(LED_FORCE_ON);
        break;
    case PON_LED_STATE_RESTORE:
        forceModeState = PON_LED_FORCE_MODE_NONE;
        PON_LED_LOS_FORCEMODE_SET(forceMode);
        if((forceMode == LED_FORCE_BLINK || forceMode == LED_FORCE_BLINK_GROUP2) && (RT_ERR_OK != rtk_led_blinkRate_set(LED_BLINK_GROUP_FORCE_MODE, rate)))
        {
            return ret;
        }
        break;
    case PON_LED_STATE_LOS_OCCURRED:
        forceMode = LED_FORCE_BLINK;
        rate = LED_BLINKRATE_256MS;
        if(PON_LED_FORCE_MODE_NONE == forceModeState)
        {
            PON_LED_LOS_FORCEMODE_SET(forceMode);
            if(RT_ERR_OK != rtk_led_blinkRate_set(LED_BLINK_GROUP_FORCE_MODE, rate))
            {
                return ret;
            }
        }
        break;
    case PON_LED_STATE_LOS_CLEARED:
        forceMode = LED_FORCE_OFF;
        if(PON_LED_FORCE_MODE_NONE == forceModeState)
        {
            PON_LED_LOS_FORCEMODE_SET(forceMode);
        }
        break;
    default:
        /* Don't know what to do for unknow case, don't do anything */
        break;
    }

    return ret;
#else
    ponStatus = ponStatus;

    return RT_ERR_OK;
#endif
}

// LED SPEC 3
static int pon_led_gpon_spec_type_03_init(void)
{
#ifdef PON_LED_GROUP_PON_STATUS
{
    int ret;
    rtk_led_config_t ledConfig;
    rtk_led_force_mode_t forceMode;

    if((ret = rtk_led_parallelEnable_set(ponStatusLed, ENABLED)) != RT_ERR_OK)
        return ret;

    osal_memset(&ledConfig,0x0,sizeof(rtk_led_config_t));
    ledConfig.ledEnable[LED_CONFIG_FORCE_MODE] = 1;
    if((ret = rtk_led_config_set(ponStatusLed, LED_TYPE_PON, &ledConfig)) != RT_ERR_OK)
        return ret;

    forceMode = LED_FORCE_OFF;
    PON_LED_PON_STATUS_FORCEMODE_SET(forceMode);
}
#endif
    return RT_ERR_OK;
}

static int32 pon_led_gpon_spec_type_03(rtk_pon_led_pon_state_t ponStatus)
{
#ifdef CONFIG_SDK_KERNEL_LINUX
    static char init = 0;
    static rtk_pon_led_pon_state_t authState = PON_LED_STATE_AUTH_OK;
    static rtk_pon_led_force_mode_t forceModeState = PON_LED_FORCE_MODE_NONE;
    static rtk_led_force_mode_t forceMode = LED_FORCE_OFF;
    static rtk_led_blink_rate_t rate = LED_BLINKRATE_32MS;
    int32 ret = RT_ERR_OK;

    RT_PARAM_CHK((PON_LED_STATE_END <= ponStatus), RT_ERR_INPUT);

    /* Init all used LED groups */
    if(0 == init)
    {
        ret = pon_led_gpon_spec_type_03_init();
        if(ret)
            return ret;

        init = 1;
    }

    switch(ponStatus)
    {
    case PON_LED_FORCE_OFF:
        forceModeState = PON_LED_FORCE_MODE_OFF;
        PON_LED_PON_STATUS_FORCEMODE_SET(LED_FORCE_OFF);
        break;
    case PON_LED_FORCE_ON:
        forceModeState = PON_LED_FORCE_MODE_ON;
        PON_LED_PON_STATUS_FORCEMODE_SET(LED_FORCE_ON);
        break;
    case PON_LED_STATE_RESTORE:
        forceModeState = PON_LED_FORCE_MODE_NONE;
        PON_LED_PON_STATUS_FORCEMODE_SET(forceMode);
        if((forceMode == LED_FORCE_BLINK || forceMode == LED_FORCE_BLINK_GROUP2) && (RT_ERR_OK != rtk_led_blinkRate_set(LED_BLINK_GROUP_FORCE_MODE, rate)))
        {
            return ret;
        }
        break;
    case PON_LED_STATE_O1:
    case PON_LED_STATE_O7:  
        forceMode = LED_FORCE_OFF;
        if(PON_LED_FORCE_MODE_NONE == forceModeState)
        {
            PON_LED_PON_STATUS_FORCEMODE_SET(forceMode);
        }
        break;
    case PON_LED_STATE_AUTH_OK:
        forceMode = LED_FORCE_ON;
        if(PON_LED_FORCE_MODE_NONE == forceModeState)
        {
            PON_LED_PON_STATUS_FORCEMODE_SET(forceMode);
        }
        authState = PON_LED_STATE_AUTH_OK;
        break;
    case PON_LED_STATE_AUTH_NG:
        forceMode = LED_FORCE_OFF;
        if(PON_LED_FORCE_MODE_NONE == forceModeState)
        {
            PON_LED_PON_STATUS_FORCEMODE_SET(forceMode);
        }
        authState = PON_LED_STATE_AUTH_NG;
        break;
    case PON_LED_STATE_O2:
    case PON_LED_STATE_O3:
    case PON_LED_STATE_O4:
//  case PON_LED_STATE_O5:
        forceMode = LED_FORCE_BLINK;
        rate = LED_BLINKRATE_512MS;
        if(PON_LED_FORCE_MODE_NONE == forceModeState)
        {
            PON_LED_PON_STATUS_FORCEMODE_SET(forceMode);
            if(RT_ERR_OK != rtk_led_blinkRate_set(LED_BLINK_GROUP_FORCE_MODE, rate))
            {
                return ret;
            }
        }
        break;
    case PON_LED_STATE_O5:
        forceMode = LED_FORCE_ON;
        if(PON_LED_FORCE_MODE_NONE == forceModeState)
        {
            PON_LED_PON_STATUS_FORCEMODE_SET(forceMode);
        }
        break;
    default:
        /* Don't know what to do for unknow case, don't do anything */
        break;
    }

    return ret;
#else
    ponStatus = ponStatus;

    return RT_ERR_OK;
#endif
}

static int pon_led_epon_spec_type_03_init(void)
{
#ifdef PON_LED_GROUP_PON_STATUS
{
    int ret;
    rtk_led_config_t ledConfig;
    rtk_led_force_mode_t forceMode;

    if((ret = rtk_led_parallelEnable_set(ponStatusLed, ENABLED)) != RT_ERR_OK)
        return ret;

    osal_memset(&ledConfig,0x0,sizeof(rtk_led_config_t));
    ledConfig.ledEnable[LED_CONFIG_FORCE_MODE] = 1;

    if((ret = rtk_led_config_set(ponStatusLed, LED_TYPE_PON, &ledConfig)) != RT_ERR_OK)
        return ret;

    forceMode = LED_FORCE_OFF;
    PON_LED_PON_STATUS_FORCEMODE_SET(forceMode);
}
#endif
    return RT_ERR_OK;
}

static int32 pon_led_epon_spec_type_03(rtk_pon_led_pon_state_t ponStatus)
{
#ifdef CONFIG_SDK_KERNEL_LINUX
    static char init = 0;
    static rtk_pon_led_force_mode_t forceModeState = PON_LED_FORCE_MODE_NONE;
    static rtk_led_force_mode_t forceMode = LED_FORCE_OFF;
    static rtk_led_blink_rate_t rate = LED_BLINKRATE_32MS;
    int32 ret = RT_ERR_OK;

    RT_PARAM_CHK((PON_LED_STATE_END <= ponStatus), RT_ERR_INPUT);

    /* Init all used LED groups */
    if(0 == init)
    {
        ret = pon_led_epon_spec_type_03_init();
        if(ret)
            return ret;

        init = 1;
    }

    switch(ponStatus)
    {
    case PON_LED_FORCE_OFF:
        forceModeState = PON_LED_FORCE_MODE_OFF;
        PON_LED_PON_STATUS_FORCEMODE_SET(LED_FORCE_OFF);
        break;
    case PON_LED_FORCE_ON:
        forceModeState = PON_LED_FORCE_MODE_ON;
        PON_LED_PON_STATUS_FORCEMODE_SET(LED_FORCE_ON);
        break;
    case PON_LED_STATE_RESTORE:
        forceModeState = PON_LED_FORCE_MODE_NONE;
        PON_LED_PON_STATUS_FORCEMODE_SET(forceMode);
        if((forceMode == LED_FORCE_BLINK || forceMode == LED_FORCE_BLINK_GROUP2) && (RT_ERR_OK != rtk_led_blinkRate_set(LED_BLINK_GROUP_FORCE_MODE, rate)))
        {
            return ret;
        }
        break;
    case PON_LED_STATE_EPONMAC_DOWN:
    case PON_LED_STATE_EPONOAM_DOWN:
        forceMode = LED_FORCE_OFF;
        if(PON_LED_FORCE_MODE_NONE == forceModeState)
        {
            PON_LED_PON_STATUS_FORCEMODE_SET(forceMode);
        }
        break;
    case PON_LED_STATE_EPONMAC_UP:
    case PON_LED_STATE_EPONOAM_UP:
        forceMode = LED_FORCE_ON;
        if(PON_LED_FORCE_MODE_NONE == forceModeState)
        {
            PON_LED_PON_STATUS_FORCEMODE_SET(forceMode);
        }
        break;
    case PON_LED_STATE_EPONOAM_TRYING:
        forceMode = LED_FORCE_BLINK;
        rate = LED_BLINKRATE_512MS;
        if(PON_LED_FORCE_MODE_NONE == forceModeState)
        {
            PON_LED_PON_STATUS_FORCEMODE_SET(forceMode);
            if(RT_ERR_OK != rtk_led_blinkRate_set(LED_BLINK_GROUP_FORCE_MODE, rate))
            {
                return ret;
            }
        }
        break;
    case PON_LED_STATE_AUTH_NG:
        forceMode = LED_FORCE_BLINK;
        rate = LED_BLINKRATE_512MS;
        if(PON_LED_FORCE_MODE_NONE == forceModeState)
        {
            PON_LED_PON_STATUS_FORCEMODE_SET(forceMode);
            if(RT_ERR_OK != rtk_led_blinkRate_set(LED_BLINK_GROUP_FORCE_MODE, rate))
            {
                return ret;
            }
        }
        break;
    case PON_LED_STATE_AUTH_OK:
        forceMode = LED_FORCE_ON;
        if(PON_LED_FORCE_MODE_NONE == forceModeState)
        {
            PON_LED_PON_STATUS_FORCEMODE_SET(forceMode);
        }
        break;
    default:
        /* Don't know what to do for unknow case, don't do anything */
        break;
    }

    return ret;
#else
    ponStatus = ponStatus;

    return RT_ERR_OK;
#endif
}

static int pon_led_others_spec_type_03_init(void)
{
#ifdef PON_LED_GROUP_LOS
{
    int ret;
    rtk_led_config_t ledConfig;
    rtk_led_force_mode_t forceMode;

    if((ret = rtk_led_parallelEnable_set(losLed, ENABLED)) != RT_ERR_OK)
        return ret;

    osal_memset(&ledConfig,0x0,sizeof(rtk_led_config_t));
    ledConfig.ledEnable[LED_CONFIG_FORCE_MODE] = 1;
    if((ret = rtk_led_config_set(losLed, LED_TYPE_PON, &ledConfig)) != RT_ERR_OK)
        return ret;

    forceMode = LED_FORCE_OFF;
    PON_LED_LOS_FORCEMODE_SET(forceMode);
}
#endif
    return RT_ERR_OK;
}

static int32 pon_led_others_spec_type_03(rtk_pon_led_pon_state_t ponStatus)
{
#ifdef CONFIG_SDK_KERNEL_LINUX
    static char init = 0;
    static rtk_pon_led_force_mode_t forceModeState = PON_LED_FORCE_MODE_NONE;
    static rtk_led_force_mode_t forceMode = LED_FORCE_OFF;
    static rtk_led_blink_rate_t rate = LED_BLINKRATE_32MS;
    int32 ret = RT_ERR_OK;

    RT_PARAM_CHK((PON_LED_STATE_END <= ponStatus), RT_ERR_INPUT);

    /* Init all used LED groups */
    if(0 == init)
    {
        ret = pon_led_others_spec_type_03_init();
        if(ret)
            return ret;

        init = 1;
    }

    switch(ponStatus)
    {
    case PON_LED_FORCE_OFF:
        forceModeState = PON_LED_FORCE_MODE_OFF;
        PON_LED_LOS_FORCEMODE_SET(LED_FORCE_OFF);
        break;
    case PON_LED_FORCE_ON:
        forceModeState = PON_LED_FORCE_MODE_ON;
        PON_LED_LOS_FORCEMODE_SET(LED_FORCE_ON);
        break;
    case PON_LED_STATE_RESTORE:
        forceModeState = PON_LED_FORCE_MODE_NONE;
        PON_LED_LOS_FORCEMODE_SET(forceMode);
        if((forceMode == LED_FORCE_BLINK || forceMode == LED_FORCE_BLINK_GROUP2) && (RT_ERR_OK != rtk_led_blinkRate_set(LED_BLINK_GROUP_FORCE_MODE, rate)))
        {
            return ret;
        }
        break;
    case PON_LED_STATE_LOS_OCCURRED:
        forceMode = LED_FORCE_BLINK;
        rate = LED_BLINKRATE_256MS;
        if(PON_LED_FORCE_MODE_NONE == forceModeState)
        {
            PON_LED_LOS_FORCEMODE_SET(forceMode);
            if(RT_ERR_OK != rtk_led_blinkRate_set(LED_BLINK_GROUP_FORCE_MODE, rate))
            {
                return ret;
            }
        }
        break;
    case PON_LED_STATE_LOS_CLEARED:
        forceMode = LED_FORCE_OFF;
        if(PON_LED_FORCE_MODE_NONE == forceModeState)
        {
            PON_LED_LOS_FORCEMODE_SET(forceMode);
        }
        break;
    default:
        /* Don't know what to do for unknow case, don't do anything */
        break;
    }

    return ret;
#else
    ponStatus = ponStatus;

    return RT_ERR_OK;
#endif
}

//LED SPEC 3

static int pon_led_gpon_spec_type_04_init(void)
{
#ifdef PON_LED_GROUP_PON_STATUS
{
    int ret;
    rtk_led_config_t ledConfig;
    rtk_led_force_mode_t forceMode;

    if((ret = rtk_led_parallelEnable_set(ponStatusLed, ENABLED)) != RT_ERR_OK)
        return ret;

    osal_memset(&ledConfig,0x0,sizeof(rtk_led_config_t));
    ledConfig.ledEnable[LED_CONFIG_FORCE_MODE] = 1;
    if((ret = rtk_led_config_set(ponStatusLed, LED_TYPE_PON, &ledConfig)) != RT_ERR_OK)
        return ret;

    forceMode = LED_FORCE_OFF;
    PON_LED_PON_STATUS_FORCEMODE_SET(forceMode);
}
#endif
    return RT_ERR_OK;
}

static int32 pon_led_gpon_spec_type_04(rtk_pon_led_pon_state_t ponStatus)
{
#ifdef CONFIG_SDK_KERNEL_LINUX
    static char init = 0;
    static rtk_pon_led_force_mode_t forceModeState = PON_LED_FORCE_MODE_NONE;
    static rtk_led_force_mode_t forceMode = LED_FORCE_OFF;
    static rtk_led_blink_rate_t rate = LED_BLINKRATE_32MS;
    int32 ret = RT_ERR_OK;

    RT_PARAM_CHK((PON_LED_STATE_END <= ponStatus), RT_ERR_INPUT);

    /* Init all used LED groups */
    if(0 == init)
    {
        ret = pon_led_gpon_spec_type_04_init();
        if(ret)
            return ret;

        init = 1;
    }

    switch(ponStatus)
    {
    case PON_LED_FORCE_OFF:
        forceModeState = PON_LED_FORCE_MODE_OFF;
        PON_LED_PON_STATUS_FORCEMODE_SET(LED_FORCE_OFF);
        break;
    case PON_LED_FORCE_ON:
        forceModeState = PON_LED_FORCE_MODE_ON;
        PON_LED_PON_STATUS_FORCEMODE_SET(LED_FORCE_ON);
        break;
    case PON_LED_STATE_RESTORE:
        forceModeState = PON_LED_FORCE_MODE_NONE;
        PON_LED_PON_STATUS_FORCEMODE_SET(forceMode);
        if((forceMode == LED_FORCE_BLINK || forceMode == LED_FORCE_BLINK_GROUP2) && (RT_ERR_OK != rtk_led_blinkRate_set(LED_BLINK_GROUP_FORCE_MODE, rate)))
        {
            return ret;
        }
        break;
    case PON_LED_STATE_O1:
        forceMode = LED_FORCE_OFF;
        if(PON_LED_FORCE_MODE_NONE == forceModeState)
        {
            PON_LED_PON_STATUS_FORCEMODE_SET(forceMode);
        }
        break;
    case PON_LED_STATE_O5:
        forceMode = LED_FORCE_ON;
        if(PON_LED_FORCE_MODE_NONE == forceModeState)
        {
            PON_LED_PON_STATUS_FORCEMODE_SET(forceMode);
        }
		rtk_pon_led_status_set(PON_LED_PON_MODE_OTHERS, PON_LED_STATE_LOS_CLEARED);
        break;
    case PON_LED_STATE_O7:	
        rtk_pon_led_status_set(PON_LED_PON_MODE_OTHERS, PON_LED_STATE_O7);
    case PON_LED_STATE_O2:
    case PON_LED_STATE_O3:
    case PON_LED_STATE_O4:
    case PON_LED_STATE_O6:
	forceMode = LED_FORCE_BLINK;
	rate = LED_BLINKRATE_512MS;
	if(PON_LED_FORCE_MODE_NONE == forceModeState)
	{
	    PON_LED_PON_STATUS_FORCEMODE_SET(forceMode);
	    if(RT_ERR_OK != rtk_led_blinkRate_set(LED_BLINK_GROUP_FORCE_MODE, rate))
	    {
	        return ret;
	    }
	}
	break;
    default:
        /* Don't know what to do for unknow case, don't do anything */
        break;
    }

    return ret;
#else
    ponStatus = ponStatus;

    return RT_ERR_OK;
#endif
}

static int pon_led_epon_spec_type_04_init(void)
{
#ifdef PON_LED_GROUP_PON_STATUS
{
    int ret;
    rtk_led_config_t ledConfig;
    rtk_led_force_mode_t forceMode;

    if((ret = rtk_led_parallelEnable_set(ponStatusLed, ENABLED)) != RT_ERR_OK)
        return ret;

    osal_memset(&ledConfig,0x0,sizeof(rtk_led_config_t));
    ledConfig.ledEnable[LED_CONFIG_FORCE_MODE] = 1;

    if((ret = rtk_led_config_set(ponStatusLed, LED_TYPE_PON, &ledConfig)) != RT_ERR_OK)
        return ret;

    forceMode = LED_FORCE_OFF;
    PON_LED_PON_STATUS_FORCEMODE_SET(forceMode);
}
#endif
    return RT_ERR_OK;
}

static int32 pon_led_epon_spec_type_04(rtk_pon_led_pon_state_t ponStatus)
{
#ifdef CONFIG_SDK_KERNEL_LINUX
    static char init = 0;
    static rtk_pon_led_force_mode_t forceModeState = PON_LED_FORCE_MODE_NONE;
    static rtk_led_force_mode_t forceMode = LED_FORCE_OFF;
    static rtk_led_blink_rate_t rate = LED_BLINKRATE_32MS;
    int32 ret = RT_ERR_OK;

    RT_PARAM_CHK((PON_LED_STATE_END <= ponStatus), RT_ERR_INPUT);

    /* Init all used LED groups */
    if(0 == init)
    {
        ret = pon_led_epon_spec_type_04_init();
        if(ret)
            return ret;

        init = 1;
    }

    switch(ponStatus)
    {
    case PON_LED_FORCE_OFF:
        forceModeState = PON_LED_FORCE_MODE_OFF;
        PON_LED_PON_STATUS_FORCEMODE_SET(LED_FORCE_OFF);
        break;
    case PON_LED_FORCE_ON:
        forceModeState = PON_LED_FORCE_MODE_ON;
        PON_LED_PON_STATUS_FORCEMODE_SET(LED_FORCE_ON);
        break;
    case PON_LED_STATE_RESTORE:
        forceModeState = PON_LED_FORCE_MODE_NONE;
        PON_LED_PON_STATUS_FORCEMODE_SET(forceMode);
        if((forceMode == LED_FORCE_BLINK || forceMode == LED_FORCE_BLINK_GROUP2) && (RT_ERR_OK != rtk_led_blinkRate_set(LED_BLINK_GROUP_FORCE_MODE, rate)))
        {
            return ret;
        }
        break;
    case PON_LED_STATE_EPONMAC_DOWN:
    case PON_LED_STATE_EPONOAM_DOWN:
        forceMode = LED_FORCE_OFF;
        if(PON_LED_FORCE_MODE_NONE == forceModeState)
        {
            PON_LED_PON_STATUS_FORCEMODE_SET(forceMode);
        }
        break;
    case PON_LED_STATE_EPONMAC_UP:
    case PON_LED_STATE_EPONOAM_UP:
        forceMode = LED_FORCE_ON;
        if(PON_LED_FORCE_MODE_NONE == forceModeState)
        {
            PON_LED_PON_STATUS_FORCEMODE_SET(forceMode);
        }
        break;
    case PON_LED_STATE_EPONOAM_TRYING:
        forceMode = LED_FORCE_BLINK;
        rate = LED_BLINKRATE_512MS;
        if(PON_LED_FORCE_MODE_NONE == forceModeState)
        {
            PON_LED_PON_STATUS_FORCEMODE_SET(forceMode);
            if(RT_ERR_OK != rtk_led_blinkRate_set(LED_BLINK_GROUP_FORCE_MODE, rate))
            {
                return ret;
            }
        }
        break;
    case PON_LED_STATE_AUTH_NG:
        forceMode = LED_FORCE_BLINK;
        rate = LED_BLINKRATE_512MS;
        if(PON_LED_FORCE_MODE_NONE == forceModeState)
        {
            PON_LED_PON_STATUS_FORCEMODE_SET(forceMode);
            if(RT_ERR_OK != rtk_led_blinkRate_set(LED_BLINK_GROUP_FORCE_MODE, rate))
            {
                return ret;
            }
        }
        break;
    case PON_LED_STATE_AUTH_OK:
        forceMode = LED_FORCE_ON;
        if(PON_LED_FORCE_MODE_NONE == forceModeState)
        {
            PON_LED_PON_STATUS_FORCEMODE_SET(forceMode);
        }
        break;
    default:
        /* Don't know what to do for unknow case, don't do anything */
        break;
    }

    return ret;
#else
    ponStatus = ponStatus;

    return RT_ERR_OK;
#endif
}

static int pon_led_others_spec_type_04_init(void)
{
#ifdef PON_LED_GROUP_LOS
{
    int ret;
    rtk_led_config_t ledConfig;
    rtk_led_force_mode_t forceMode;

    if((ret = rtk_led_parallelEnable_set(losLed, ENABLED)) != RT_ERR_OK)
        return ret;

    osal_memset(&ledConfig,0x0,sizeof(rtk_led_config_t));
    ledConfig.ledEnable[LED_CONFIG_FORCE_MODE] = 1;
    if((ret = rtk_led_config_set(losLed, LED_TYPE_PON, &ledConfig)) != RT_ERR_OK)
        return ret;

    forceMode = LED_FORCE_OFF;
    PON_LED_LOS_FORCEMODE_SET(forceMode);
}
#endif
    return RT_ERR_OK;
}

static int32 pon_led_others_spec_type_04(rtk_pon_led_pon_state_t ponStatus)
{
#ifdef CONFIG_SDK_KERNEL_LINUX
    static char init = 0;
    static rtk_pon_led_force_mode_t forceModeState = PON_LED_FORCE_MODE_NONE;
    static rtk_led_force_mode_t forceMode = LED_FORCE_OFF;
    static rtk_led_blink_rate_t rate = LED_BLINKRATE_32MS;
    int32 ret = RT_ERR_OK;

    RT_PARAM_CHK((PON_LED_STATE_END <= ponStatus), RT_ERR_INPUT);

    /* Init all used LED groups */
    if(0 == init)
    {
        ret = pon_led_others_spec_type_04_init();
        if(ret)
            return ret;

        init = 1;
    }

    switch(ponStatus)
    {
    case PON_LED_FORCE_OFF:
        forceModeState = PON_LED_FORCE_MODE_OFF;
        PON_LED_LOS_FORCEMODE_SET(LED_FORCE_OFF);
        break;
    case PON_LED_FORCE_ON:
        forceModeState = PON_LED_FORCE_MODE_ON;
        PON_LED_LOS_FORCEMODE_SET(LED_FORCE_ON);
        break;
    case PON_LED_STATE_RESTORE:
        forceModeState = PON_LED_FORCE_MODE_NONE;
        PON_LED_LOS_FORCEMODE_SET(forceMode);
        if((forceMode == LED_FORCE_BLINK || forceMode == LED_FORCE_BLINK_GROUP2) && (RT_ERR_OK != rtk_led_blinkRate_set(LED_BLINK_GROUP_FORCE_MODE, rate)))
        {
            return ret;
        }
        break;
    case PON_LED_STATE_LOS_OCCURRED:
        forceMode = LED_FORCE_BLINK;
        rate = LED_BLINKRATE_256MS;
        if(PON_LED_FORCE_MODE_NONE == forceModeState)
        {
            PON_LED_LOS_FORCEMODE_SET(forceMode);
            if(RT_ERR_OK != rtk_led_blinkRate_set(LED_BLINK_GROUP_FORCE_MODE, rate))
            {
                return ret;
            }
        }
        break;
    case PON_LED_STATE_LOS_CLEARED:
        forceMode = LED_FORCE_OFF;	
        if(PON_LED_FORCE_MODE_NONE == forceModeState)
        {
            PON_LED_LOS_FORCEMODE_SET(forceMode);
        }
        break;
    case PON_LED_STATE_O7:
        forceMode = LED_FORCE_ON;
        if(PON_LED_FORCE_MODE_NONE == forceModeState)
        {
            PON_LED_LOS_FORCEMODE_SET(forceMode);
        }
        break;
    default:
        /* Don't know what to do for unknow case, don't do anything */
        break;
    }

    return ret;
#else
    ponStatus = ponStatus;

    return RT_ERR_OK;
#endif
}

#ifdef CONFIG_SDK_KERNEL_LINUX
struct proc_dir_entry *led_swBlink_proc_dir = NULL;
struct proc_dir_entry *led_swBlink_cfg_proc = NULL;

uint32 stopPolling = 0;

static ssize_t led_swBlink_cfg_write(struct file *file, const char *buffer, size_t count, loff_t *data)
{
	unsigned char tmpBuf[16] = {0};
    unsigned int value;
	int len = (count > 15) ? 15 : count;
    int ret;

	if (buffer && !copy_from_user(tmpBuf, buffer, len))
	{
        tmpBuf[len] = '\0';
		value = simple_strtoul(tmpBuf, NULL, 10);
		stopPolling = value;
		printk("change stopPolling to %u\n", stopPolling);

		return count;
	}
	return -EFAULT;
}

static int led_swBlink_cfg_read(struct seq_file *seq, void *v)
{
	seq_printf(seq, "stopPolling = %u\n", stopPolling);

	return 0;
}

static int led_swBlink_cfg_open_proc(struct inode *inode, struct file *file)
{
	return single_open(file, led_swBlink_cfg_read, NULL);
}


#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
struct proc_ops led_swBlink_cfg_fop = {
    .proc_open           = led_swBlink_cfg_open_proc,
    .proc_write          = led_swBlink_cfg_write,
    .proc_read           = seq_read,
    .proc_lseek          = seq_lseek,
    .proc_release        = single_release,
};
#else
struct file_operations led_swBlink_cfg_fop = {
	.open = led_swBlink_cfg_open_proc,
	.write = led_swBlink_cfg_write,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};
#endif

static int led_swBlink_thread(void *data)
{
    static rtk_enable_t lastLosState = RTK_ENABLE_END;
    int ret;
    rtk_enable_t losState;

    while(!kthread_should_stop())
    {
        /* It is NOT possible to wake up earlier */
        set_current_state(TASK_UNINTERRUPTIBLE);
        schedule_timeout(5 * HZ / 100); /* Wake up every 50 ms*/

		if(stopPolling)
		{
			continue;
		}

        rtk_pon_led_status_set(PON_LED_PON_MODE_OTHERS, PON_LED_STATE_TIMER_50MS);

        if((ret = rtk_ponmac_losState_get(&losState)) == RT_ERR_OK)
        {
            if(lastLosState != losState)
            {
                /* LOS state changed or first time execute */
                if(ENABLED == losState)
                {
                    rtk_pon_led_status_set(PON_LED_PON_MODE_OTHERS, PON_LED_STATE_LOS_OCCURRED);
                }
                else
                {
                    rtk_pon_led_status_set(PON_LED_PON_MODE_OTHERS, PON_LED_STATE_LOS_CLEARED);
                }
                lastLosState = losState;
            }
        }
    }

    return 0;
}
#endif

/* Function Name:
 *      rtk_pon_led_SpecType_set
 * Description:
 *      Set pon port spec type
 * Input:
 *      type            - PON port LED spec type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 */
int32
rtk_pon_led_SpecType_set(rtk_pon_led_spec_type_t type)
{
    RT_PARAM_CHK((PON_LED_SPEC_TYPE_END <= type), RT_ERR_INPUT);
    /* function body */
    pon_led_spec_type = type;

#ifdef CONFIG_SDK_KERNEL_LINUX
    if(NULL == pLedBlink)
    {
        struct task_struct *pKthread;

        pKthread = kthread_create(led_swBlink_thread, NULL, "led_swBlink");
        if(IS_ERR(pKthread))
        {
            printk("%s:%d led sw blink init failed %ld!\n", __FILE__, __LINE__, PTR_ERR(pKthread));
        }
        else
        {
            pLedBlink = pKthread;
            wake_up_process(pLedBlink);
            printk("%s:%d led sw blink init complete!\n", __FILE__, __LINE__);

			/* Create proc file for debug usage */
		    if(NULL == led_swBlink_proc_dir)
		    {
		        led_swBlink_proc_dir = proc_mkdir("led_swblink", NULL);
		    }
			if(led_swBlink_proc_dir)
			{
		    	led_swBlink_cfg_proc = proc_create("stop_polling", 0, led_swBlink_proc_dir, &led_swBlink_cfg_fop);
	        }
        }
    }
#endif

    return RT_ERR_OK;
}   /* end of rtk_pon_led_SpecType_set */

/* Function Name:
 *      rtk_pon_led_status_set
 * Description:
 *      Set GPON/EPON pon port LED by pon status
 * Input:
 *      ponMode             - GPON or EPON
 *      ponStatus           - GPON/EPON PON status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 */
int32
rtk_pon_led_status_set(rtk_pon_led_pon_mode_t ponMode, rtk_pon_led_pon_state_t ponStatus)
{
    int32   ret=RT_ERR_OK;

    /* function body */
    switch(ponMode)
    {
    case PON_LED_PON_MODE_GPON:
        switch(pon_led_spec_type)
        {
        case PON_LED_SPEC_TYPE_01:
            ret = pon_led_gpon_spec_type_01(ponStatus);
            break;

        case PON_LED_SPEC_TYPE_02:
            ret = pon_led_gpon_spec_type_02(ponStatus);
            break;

        case PON_LED_SPEC_TYPE_03:
            ret = pon_led_gpon_spec_type_03(ponStatus);
            break;

        case PON_LED_SPEC_TYPE_04:
            ret = pon_led_gpon_spec_type_04(ponStatus);
            break;
		
        case PON_LED_SPEC_TYPE_00:
        default:
            ret = pon_led_gpon_spec_type_00(ponStatus);
            break;
        }
        break;

    case PON_LED_PON_MODE_EPON:
        switch(pon_led_spec_type)
        {
        case PON_LED_SPEC_TYPE_01:
            ret = pon_led_epon_spec_type_01(ponStatus);
            break;

        case PON_LED_SPEC_TYPE_02:
            ret = pon_led_epon_spec_type_02(ponStatus);
            break;

        case PON_LED_SPEC_TYPE_03:
            ret = pon_led_epon_spec_type_03(ponStatus);
            break;

        case PON_LED_SPEC_TYPE_04:
            ret = pon_led_epon_spec_type_04(ponStatus);
            break;

        case PON_LED_SPEC_TYPE_00:
        default:
            ret = pon_led_epon_spec_type_00(ponStatus);
            break;
        }
        break;

    case PON_LED_PON_MODE_OTHERS:
        switch(pon_led_spec_type)
        {
        case PON_LED_SPEC_TYPE_01:
            ret = pon_led_others_spec_type_01(ponStatus);
            break;

        case PON_LED_SPEC_TYPE_02:
            ret = pon_led_others_spec_type_02(ponStatus);
            break;

        case PON_LED_SPEC_TYPE_03:
            ret = pon_led_others_spec_type_03(ponStatus);
            break;

        case PON_LED_SPEC_TYPE_04:
            ret = pon_led_others_spec_type_04(ponStatus);
            break;

        case PON_LED_SPEC_TYPE_00:
        default:
            ret = pon_led_others_spec_type_00(ponStatus);
            break;
        }
        break;

    default:
        ret = RT_ERR_INPUT;
        break;
    }

    return ret;
}   /* end of rtk_pon_led_status_set */

/* Function Name:
 *      rtk_pon_led_group_set
 * Description:
 *      Set PON LED group using which LED group
 * Input:
 *      ponLedGroup         - PON LED group
 *      ledGroupIdx         - LED group index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 */
int32
rtk_pon_led_group_set(rtk_pon_led_group_t ponLedGroup, uint32 ledGroupIdx)
{
    RT_PARAM_CHK((PON_LED_GROUP_END <= ponLedGroup), RT_ERR_INPUT);

	switch(ponLedGroup)
	{
#ifdef PON_LED_GROUP_PON_STATUS
	case PON_LED_GROUP_PON_STATUS_IDX:
		ponStatusLed = ledGroupIdx;
		break;
#endif
#ifdef PON_LED_GROUP_SYSTEM
	case PON_LED_GROUP_SYSTEM_IDX:
		systemLed = ledGroupIdx;
		break;
#endif
#ifdef PON_LED_GROUP_LOS
	case PON_LED_GROUP_LOS_IDX:
		losLed = ledGroupIdx;
		break;
#endif
	default:
		/* Dummy code to prevent unused variable */
		ledGroupIdx = ledGroupIdx;
		return RT_ERR_INPUT;
	}

	return RT_ERR_OK;
}


/* Function Name:
 *      rtk_pon_led_group_get
 * Description:
 *      Get PON LED group using which LED group
 * Input:
 *      ponLedGroup         - PON LED group
 *      pLedGroupIdx        - LED group index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 */
int32
rtk_pon_led_group_get(rtk_pon_led_group_t ponLedGroup, uint32 *pLedGroupIdx)
{
    RT_PARAM_CHK((PON_LED_GROUP_END <= ponLedGroup), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pLedGroupIdx), RT_ERR_INPUT);

	switch(ponLedGroup)
	{
#ifdef PON_LED_GROUP_PON_STATUS
	case PON_LED_GROUP_PON_STATUS_IDX:
		*pLedGroupIdx = ponStatusLed;
		break;
#endif
#ifdef PON_LED_GROUP_SYSTEM
	case PON_LED_GROUP_SYSTEM_IDX:
		*pLedGroupIdx = systemLed;
		break;
#endif
#ifdef PON_LED_GROUP_LOS
	case PON_LED_GROUP_LOS_IDX:
		*pLedGroupIdx = losLed;
		break;
#endif
	default:
		return RT_ERR_INPUT;
	}

	return RT_ERR_OK;
}

