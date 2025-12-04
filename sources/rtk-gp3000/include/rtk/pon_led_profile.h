/*
 * Copyright (C) 2015 Realtek Semiconductor Corp.
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
 * Purpose : Definition PON LED group used by board profiles
 *
 * Feature : The file have include the following module and sub-modules
 *           (1) PON LED settings
 */

#ifndef __PON_LED_PROFILE_H__
#define __PON_LED_PROFILE_H__

#if defined(CONFIG_LEDS_RTK_GPIO_LED) 
#include <linux/leds-ca77xx.h>
#include <../drivers/soc/realtek/led-generic.h>
#endif

#ifdef CONFIG_PON_LED_PROFILE_DEMO_RTL9607
#define PON_LED_GROUP_PON_STATUS        16
#endif /* CONFIG_PON_LED_PROFILE_DEMO_RTL9607 */

#ifdef CONFIG_PON_LED_PROFILE_DEMO_RTL9607_IAD_V00
#define PON_LED_GROUP_PON_STATUS        13
#define PON_LED_GROUP_LOS               3
#endif /* CONFIG_PON_LED_PROFILE_DEMO_RTL9607 */

#ifdef CONFIG_PON_LED_PROFILE_DEMO_RTL9601B
#define PON_LED_GROUP_PON_STATUS        1
#define PON_LED_GROUP_LOS               2
#endif /* CONFIG_PON_LED_PROFILE_DEMO_RTL9601B */

#ifdef CONFIG_PON_LED_PROFILE_DEMO_RTL9602B
#define PON_LED_GROUP_PON_STATUS        16
#endif /* CONFIG_PON_LED_PROFILE_DEMO_RTL9602B */

#ifdef CONFIG_PON_LED_PROFILE_DEMO_RTL9601
#define PON_LED_GROUP_PON_STATUS        9
#endif /* CONFIG_PON_LED_PROFILE_DEMO_RTL9601 */

#ifdef CONFIG_PON_LED_PROFILE_001
#define PON_LED_GROUP_PON_STATUS        1
#define PON_LED_GROUP_LOS               2
#define PON_LED_GROUP_SYSTEM            3
#endif /* CONFIG_PON_LED_PROFILE_001 */

#ifdef CONFIG_PON_LED_PROFILE_DEMO_RTL9607P
#define PON_LED_GROUP_PON_STATUS        13
#endif /* CONFIG_PON_LED_PROFILE_DEMO_RTL9607P */

#ifdef CONFIG_PON_LED_PROFILE_DEMO_RTL9602C
#define PON_LED_GROUP_PON_STATUS        12
#define PON_LED_GROUP_LOS               13
#endif /* CONFIG_PON_LED_PROFILE_DEMO_RTL9602C */

#ifdef CONFIG_PON_LED_PROFILE_DEMO_RTL9601C
/* NO any LED on board */
#endif /* CONFIG_PON_LED_PROFILE_DEMO_RTL9601C */

#ifdef CONFIG_PON_LED_PROFILE_DEMO_RTL9607C
#define PON_LED_GROUP_PON_STATUS        4
#define PON_LED_GROUP_LOS               9
#endif /* CONFIG_PON_LED_PROFILE_DEMO_RTL9607C */

#ifdef CONFIG_PON_LED_PROFILE_DEMO_RTL9607C_TBL_B
#define PON_LED_GROUP_PON_STATUS        4
#define PON_LED_GROUP_LOS               14
#endif /* CONFIG_PON_LED_PROFILE_DEMO_RTL9607C_TBL_B */

#ifdef CONFIG_PON_LDE_PROFILE_DEMO_CA8277_REVB
#if defined(CONFIG_LEDS_RTK_GPIO_LED) 
#define PON_LED_GROUP_PON_STATUS        LED_PON
#define PON_LED_GROUP_LOS               LED_LOS
#else
#define PON_LED_GROUP_PON_STATUS        14
#define PON_LED_GROUP_LOS               13
#endif
#endif /* CONFIG_PON_LDE_PROFILE_DEMO_CA8277 */

#ifdef CONFIG_PON_LED_PROFILE_DEMO_RTL9601D_SFU_HGU
#define PON_LED_GROUP_PON_STATUS        6
#define PON_LED_GROUP_LOS               15
#endif /* CONFIG_PON_LED_PROFILE_DEMO_RTL9601D_SFU_HGU */

#ifdef PON_LED_GROUP_PON_STATUS
#define PON_LED_PON_STATUS_CONFIG_SET(type, pConfig)                                        \
{                                                                                           \
    int ret;                                                                                \
    if((ret = rtk_led_config_set(ponStatusLed, type, pConfig)) != RT_ERR_OK)               \
        return ret;                                                                         \
}

#define PON_LED_PON_STATUS_FORCEMODE_SET(forceMode)                                         \
{                                                                                           \
    int ret;                                                                                \
    if((ret = rtk_led_modeForce_set(ponStatusLed, forceMode)) != RT_ERR_OK)                \
        return ret;                                                                         \
}
#else
#define PON_LED_PON_STATUS_CONFIG_SET(type, pConfig)
#define PON_LED_PON_STATUS_FORCEMODE_SET(forceMode)
#endif /* PON_LED_GROUP_PON_STATUS */

#ifdef PON_LED_GROUP_SYSTEM
#define PON_LED_SYSTEM_SET(type, pConfig)                                                   \
{                                                                                           \
    int ret;                                                                                \
    if((ret = rtk_led_config_set(systemLed, type, pConfig)) != RT_ERR_OK)                  \
        return ret;                                                                         \
}

#define PON_LED_SYSTEM_FORCEMODE_SET(forceMode)                                             \
{                                                                                           \
    int ret;                                                                                \
    if((ret = rtk_led_modeForce_set(systemLed, forceMode)) != RT_ERR_OK)                   \
        return ret;                                                                         \
}
#else
#define PON_LED_SYSTEM_SET(type, pConfig)
#define PON_LED_SYSTEM_FORCEMODE_SET(forceMode)
#endif /* PON_LED_GROUP_SYSTEM */

#ifdef PON_LED_GROUP_LOS
#define PON_LED_LOS_SET(type, pConfig)                                                      \
{                                                                                           \
    int ret;                                                                                \
    if((ret = rtk_led_config_set(losLed, type, pConfig)) != RT_ERR_OK)                     \
        return ret;                                                                         \
}

#define PON_LED_LOS_FORCEMODE_SET(forceMode)                                                \
{                                                                                           \
    int ret;                                                                                \
    if((ret = rtk_led_modeForce_set(losLed, forceMode)) != RT_ERR_OK)                      \
        return ret;                                                                         \
}
#else
#define PON_LED_LOS_SET(type, pConfig)
#define PON_LED_LOS_FORCEMODE_SET(forceMode)
#endif /* PON_LED_GROUP_LOS */

#endif /* __PON_LED_PROFILE_H__ */

