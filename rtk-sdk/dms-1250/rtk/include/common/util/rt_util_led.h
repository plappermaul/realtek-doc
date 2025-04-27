/*
 * Copyright (C) 2009-2015 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision:
 * $Date:
 *
 * Purpose : Define the led utility macro and function in the SDK.
 *
 * Feature : SDK common utility
 *
 */

#ifndef __RT_UTIL_LED_H__
#define __RT_UTIL_LED_H__

/*
 * Include Files
 */
#include <hal/chipdef/allreg.h>
#include <private/drv/rtl8231/rtl8231.h>

/*
 * Symbol Definition
 */
#define RTL9300_BOX_ID_LED_NUM                  (4)
#define RTL9300_BOX_ID_LED_GPIO_BASE            (15)
#define RTL9300_BOX_ID_LED_GPIO_DEVID           (1)


/*
 * Data Type Declaration
 */

/*
 * Data Declaration
 */

/*
 * Macro Definition
 */

/*
 * Function Declaration
 */

/* Function Name:
 *      rtk_swled_on
 * Description:
 *      light on the led in runtime
 * Input:
 *      unit   - unit id
 *      start  - start port
 *      range  - the range of light on led
 *      index  - the led index of the port
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 */
extern void rtk_swled_on(uint32 unit, uint32 start, uint32 range, uint32 index);

/* Function Name:
 *      rtk_boxID_led_set
 * Description:
 *      Set the box id led.
 * Input:
 *      unit   - unit id
 *      box    - box id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 */
int32 rtk_boxID_led_set(uint32 unit, uint32 boxId);

/* Function Name:
 *      rtk_boxID_led_init
 * Description:
 *      Initial box id led.
 * Input:
 *      unit   - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 */
int32 rtk_boxID_led_init(uint32 unit);

/* Function Name:
 *      rtk_masterLedEnable_set
 * Description:
 *      Set stacking master led state.
 * Input:
 *      unit   - unit id.
 *      enable - master led state.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 */
int32 rtk_masterLedEnable_set(uint32 unit, rtk_enable_t enable);


#endif /* __RT_UTIL_LED_H__ */

