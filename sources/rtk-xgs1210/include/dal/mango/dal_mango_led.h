/*
 * Copyright (C) 2009-2016 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision:  $
 * $Date:  $
 *
 * Purpose : Definition those public LED APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) LED
 *
 */

#ifndef __DAL_MANGO_LED_H__
#define __DAL_MANGO_LED_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <rtk/port.h>
#include <rtk/led.h>
#include <dal/dal_mapper.h>


/*
 * Symbol Definition
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
 *      dal_mango_ledMapper_init
 * Description:
 *      Hook led module of the specified device.
 * Input:
 *      pMapper - pointer of mapper
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      Must Hook led module before calling any led APIs.
 */
extern int32
dal_mango_ledMapper_init(dal_mapper_t *pMapper);

/* Function Name:
 *      dal_mango_led_init
 * Description:
 *      Initialize port module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      1) Module must be initialized before using all of APIs in this module
 */
extern int32
dal_mango_led_init(uint32 unit);

/* Function Name:
 *      dal_mango_led_portLedEntitySwCtrlEnable_get
 * Description:
 *      Get LED status on specified port and LED entity.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      entity  - LED entity id
 * Output:
 *      pEnable - LED status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_mango_led_portLedEntitySwCtrlEnable_get(
    uint32          unit,
    rtk_port_t      port,
    uint32          entity,
    rtk_enable_t    *pEnable);

/* Function Name:
 *      dal_mango_led_portLedEntitySwCtrlEnable_set
 * Description:
 *      Set LED status on specified port and LED entity.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      entity - LED entity id
 *      enable - LED status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_mango_led_portLedEntitySwCtrlEnable_set(
    uint32          unit,
    rtk_port_t      port,
    uint32          entity,
    rtk_enable_t    enable);

/* Function Name:
 *      dal_mango_led_portLedEntitySwCtrlMode_get
 * Description:
 *      Get LED display mode on specified port, LED entity and media.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      entity - LED entity id
 *      media  - media type
 * Output:
 *      pMode  - LED display mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      1) Media type only supports PORT_MEDIA_COPPER and PORT_MEDIA_FIBER.
 *      2) System software control mode only support:
 *         - RTK_LED_SWCTRL_MODE_OFF,
 *         - RTK_LED_SWCTRL_MODE_BLINKING_32MS,
 *         - RTK_LED_SWCTRL_MODE_BLINKING_64MS,
 *         - RTK_LED_SWCTRL_MODE_BLINKING_128MS,
 *         - RTK_LED_SWCTRL_MODE_BLINKING_512MS,
 *         - RTK_LED_SWCTRL_MODE_BLINKING_CONTINUE,
 */
extern int32
dal_mango_led_portLedEntitySwCtrlMode_get(
    uint32                  unit,
    rtk_port_t              port,
    uint32                  entity,
    rtk_port_media_t        media,
    rtk_led_swCtrl_mode_t   *pMode);

/* Function Name:
 *      dal_mango_led_portLedEntitySwCtrlMode_set
 * Description:
 *      Set LED display mode on specified port, LED entity and media.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      entity - LED entity id
 *      media  - media type
 *      mode   - LED display mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      1) Media type only supports PORT_MEDIA_COPPER and PORT_MEDIA_FIBER.
 *      2) System software control mode only support:
 *         - RTK_LED_SWCTRL_MODE_OFF,
 *         - RTK_LED_SWCTRL_MODE_BLINKING_32MS,
 *         - RTK_LED_SWCTRL_MODE_BLINKING_64MS,
 *         - RTK_LED_SWCTRL_MODE_BLINKING_128MS,
 *         - RTK_LED_SWCTRL_MODE_BLINKING_512MS,
 *         - RTK_LED_SWCTRL_MODE_BLINKING_CONTINUE,
 */
extern int32
dal_mango_led_portLedEntitySwCtrlMode_set(
    uint32                  unit,
    rtk_port_t              port,
    uint32                  entity,
    rtk_port_media_t        media,
    rtk_led_swCtrl_mode_t   mode);

/* Function Name:
 *      dal_mango_led_swCtrl_start
 * Description:
 *      Start to apply LED configuration.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      entity - LED entity id
 *      enable - LED status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 * Note:
 *      None
 */
extern int32
dal_mango_led_swCtrl_start(uint32 unit);

/* Function Name:
 *      dal_mango_led_sysMode_get
 * Description:
 *      Get system LED display mode.
 * Input:
 *      unit  - unit id
 * Output:
 *      pMode - LED display mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      System software control mode only support:
 *      - RTK_LED_SWCTRL_MODE_OFF
 *      - RTK_LED_SWCTRL_MODE_BLINKING_256MS
 *      - RTK_LED_SWCTRL_MODE_BLINKING_512MS
 *      - RTK_LED_SWCTRL_MODE_BLINKING_CONTINUE
 */
extern int32
dal_mango_led_sysMode_get(uint32 unit, rtk_led_swCtrl_mode_t *pMode);


/* Function Name:
 *      dal_mango_led_sysMode_set
 * Description:
 *      Set system LED display mode.
 * Input:
 *      unit - unit id
 *      mode - LED display mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      System software control mode only support:
 *      - RTK_LED_SWCTRL_MODE_OFF
 *      - RTK_LED_SWCTRL_MODE_BLINKING_256MS
 *      - RTK_LED_SWCTRL_MODE_BLINKING_512MS
 *      - RTK_LED_SWCTRL_MODE_BLINKING_CONTINUE
 */
extern int32
dal_mango_led_sysMode_set(uint32 unit, rtk_led_swCtrl_mode_t mode);


/* Function Name:
 *      dal_mango_led_sysEnable_get
 * Description:
 *      Get led status on specified type.
 * Input:
 *      unit    - unit id
 *      type    - system led type
 * Output:
 *      pEnable - pointer to the led status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      LED type only supports LED_TYPE_SYS.
 */
extern int32
dal_mango_led_sysEnable_get(uint32 unit, rtk_led_type_t type, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_mango_led_sysEnable_set
 * Description:
 *      Set led status on specified type.
 * Input:
 *      unit   - unit id
 *      type   - system led type
 *      enable - led status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      LED type only supports LED_TYPE_SYS.
 */
extern int32
dal_mango_led_sysEnable_set(uint32 unit, rtk_led_type_t type, rtk_enable_t enable);

/* Function Name:
 *      dal_mango_led_blinkTime_get
 * Description:
 *      Get LED blinking cycle time
 * Input:
 *      unit  - unit id
 * Output:
 *      pTime - cycle time
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_mango_led_blinkTime_get(uint32 unit, rtk_led_blinkTime_t *pTime);

/* Function Name:
 *      dal_mango_led_blinkTime_set
 * Description:
 *      Set LED blinking cycle time
 * Input:
 *      unit - unit id
 *      time - cycle time
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_mango_led_blinkTime_set(uint32 unit, rtk_led_blinkTime_t time);

#endif /* __DAL_MANGO_LED_H__ */

