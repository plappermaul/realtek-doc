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
 * $Revision$
 * $Date$
 *
 * Purpose : Definition those public LED APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) LED
 *
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_bitop.h>
#include <common/util/rt_util.h>
#include <common/debug/rt_log.h>
#include <osal/sem.h>
#include <hal/mac/reg.h>
#include <hal/chipdef/allreg.h>
#include <hal/chipdef/maple/rtk_maple_reg_struct.h>
#include <hal/common/halctrl.h>
#include <dal/maple/dal_maple_led.h>
#include <dal/dal_mgmt.h>
#include <rtk/port.h>
#include <rtk/led.h>
#include <ioal/mem32.h>
#include <osal/time.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
static uint32       led_init[RTK_MAX_NUM_OF_UNIT] = {INIT_NOT_COMPLETED};
static osal_mutex_t led_sem[RTK_MAX_NUM_OF_UNIT];

/*
 * Macro Definition
 */
/* led semaphore handling */
#define LED_SEM_LOCK(unit)    \
do {\
    if (osal_sem_mutex_take(led_sem[unit], OSAL_SEM_WAIT_FOREVER) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_LOCK_FAILED, (MOD_DAL|MOD_LED), "semaphore lock failed");\
        return RT_ERR_SEM_LOCK_FAILED;\
    }\
} while(0)
#define LED_SEM_UNLOCK(unit)   \
do {\
    if (osal_sem_mutex_give(led_sem[unit]) != RT_ERR_OK)\
    {\
        RT_ERR(RT_ERR_SEM_UNLOCK_FAILED, (MOD_DAL|MOD_LED), "semaphore unlock failed");\
        return RT_ERR_SEM_UNLOCK_FAILED;\
    }\
} while(0)

/*
 * Function Declaration
 */

/* Function Name:
 *      dal_maple_ledMapper_init
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
int32
dal_maple_ledMapper_init(dal_mapper_t *pMapper)
{
    pMapper->led_init = dal_maple_led_init;
    pMapper->led_sysEnable_get = dal_maple_led_sysEnable_get;
    pMapper->led_sysEnable_set = dal_maple_led_sysEnable_set;
    pMapper->led_portLedEntitySwCtrlEnable_get = dal_maple_led_portLedEntitySwCtrlEnable_get;
    pMapper->led_portLedEntitySwCtrlEnable_set = dal_maple_led_portLedEntitySwCtrlEnable_set;
    pMapper->led_portLedEntitySwCtrlMode_get = dal_maple_led_portLedEntitySwCtrlMode_get;
    pMapper->led_portLedEntitySwCtrlMode_set = dal_maple_led_portLedEntitySwCtrlMode_set;
    pMapper->led_sysMode_get = dal_maple_led_sysMode_get;
    pMapper->led_sysMode_set = dal_maple_led_sysMode_set;
    pMapper->led_blinkTime_get = dal_maple_led_blinkTime_get;
    pMapper->led_blinkTime_set = dal_maple_led_blinkTime_set;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_maple_led_init
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
int32
dal_maple_led_init(uint32 unit)
{

    RT_INIT_REENTRY_CHK(led_init[unit]);
    led_init[unit] = INIT_NOT_COMPLETED;

    /* create semaphore */
    led_sem[unit] = osal_sem_mutex_create();
    if (0 == led_sem[unit])
    {
        RT_ERR(RT_ERR_FAILED, (MOD_DAL|MOD_LED), "semaphore create failed");
        return RT_ERR_FAILED;
    }

    /* set init flag to complete init */
    led_init[unit] = INIT_COMPLETED;

    return RT_ERR_OK;
} /* end of dal_maple_led_init */


/* Function Name:
 *      dal_maple_led_portLedEntitySwCtrlEnable_get
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
int32
dal_maple_led_portLedEntitySwCtrlEnable_get(
    uint32          unit,
    rtk_port_t      port,
    uint32          entity,
    rtk_enable_t    *pEnable)
{
    uint32  value;
    int32   ret;
    int32   index1, index2;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_LED), "unit=%d, port=%d, entity=%d",
            unit, port, entity);

    /* check Init status */
    RT_INIT_CHK(led_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((entity >= HAL_MAX_NUM_OF_LED_ENTITY(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if (0 == entity)
    {
        index1 = MAPLE_LED0_SW_P_EN_CTRLr;
        index2 = MAPLE_SW_CTRL_LED0_EN_27_0f;
    }
    else if (1 == entity)
    {
        index1 = MAPLE_LED1_SW_P_EN_CTRLr;
        index2 = MAPLE_SW_CTRL_LED1_EN_27_0f;
    }
    else
    {
        index1 = MAPLE_LED2_SW_P_EN_CTRLr;
        index2 = MAPLE_SW_CTRL_LED2_EN_27_0f;
    }

    LED_SEM_LOCK(unit);

    ret = reg_field_read(unit, index1, index2, &value);
    if (RT_ERR_OK != ret)
    {
        LED_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_LED), "");
        return ret;
    }

    LED_SEM_UNLOCK(unit);

    value = (value >> port) & 0x1;

    if (0 == value)
        *pEnable = DISABLED;
    else
        *pEnable = ENABLED;

    return RT_ERR_OK;
} /* end of dal_maple_led_portLedEntitySwCtrlEnable_get */

/* Function Name:
 *      dal_maple_led_portLedEntitySwCtrlEnable_set
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
int32
dal_maple_led_portLedEntitySwCtrlEnable_set(
    uint32          unit,
    rtk_port_t      port,
    uint32          entity,
    rtk_enable_t    enable)
{
    uint32  value, entity_value;
    int32   ret;
    int32   index1, index2;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_LED), "unit=%d, port=%d, entity=%d, enable=%d",
            unit, port, entity, enable);

    /* check Init status */
    RT_INIT_CHK(led_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((entity >= HAL_MAX_NUM_OF_LED_ENTITY(unit)), RT_ERR_INPUT);
    RT_PARAM_CHK((enable >= RTK_ENABLE_END), RT_ERR_INPUT);

    entity_value = (1 << port);

    if (0 == entity)
    {
        index1 = MAPLE_LED0_SW_P_EN_CTRLr;
        index2 = MAPLE_SW_CTRL_LED0_EN_27_0f;
    }
    else if (1 == entity)
    {
        index1 = MAPLE_LED1_SW_P_EN_CTRLr;
        index2 = MAPLE_SW_CTRL_LED1_EN_27_0f;
    }
    else
    {
        index1 = MAPLE_LED2_SW_P_EN_CTRLr;
        index2 = MAPLE_SW_CTRL_LED2_EN_27_0f;
    }

    LED_SEM_LOCK(unit);

    ret = reg_field_read(unit, index1, index2, &value);
    if (RT_ERR_OK != ret)
    {
        LED_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_LED), "");
        return ret;
    }

    if (ENABLED == enable)
        value |= entity_value;
    else
        value &= ~entity_value;

    ret = reg_field_write(unit, index1, index2, &value);
    if (RT_ERR_OK != ret)
    {
        LED_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_LED), "");
        return ret;
    }

    LED_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_led_portLedEntitySwCtrlEnable_set */

/* Function Name:
 *      dal_maple_led_portLedEntitySwCtrlMode_get
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
int32
dal_maple_led_portLedEntitySwCtrlMode_get(
    uint32                  unit,
    rtk_port_t              port,
    uint32                  entity,
    rtk_port_media_t        media,
    rtk_led_swCtrl_mode_t   *pMode)
{
    uint32  value;
    uint32  field;
    int32   ret = RT_ERR_INPUT;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_LED), "unit=%d, port=%d, entity=%d, media=%d",
            unit, port, entity, media);

    /* check Init status */
    RT_INIT_CHK(led_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((entity >= HAL_MAX_NUM_OF_LED_ENTITY(unit)), RT_ERR_INPUT);

    switch (media)
    {   /*NO DIFF between COPPER & FIBER*/
        case PORT_MEDIA_COPPER:
        case PORT_MEDIA_FIBER:
            switch (entity)
            {
                case 0:
                    field = MAPLE_SW_P_LED0_MODEf;
                    break;
                case 1:
                    field = MAPLE_SW_P_LED1_MODEf;
                    break;
                case 2:
                    field = MAPLE_SW_P_LED2_MODEf;
                    break;
                default:
                    RT_ERR(ret, (MOD_DAL|MOD_LED), "");
                    return RT_ERR_INPUT;
            }
            break;
        default:
            RT_LOG(LOG_DEBUG, (MOD_LED|MOD_DAL), "ret = 0x%x", ret);
            return RT_ERR_INPUT;
    }

    LED_SEM_LOCK(unit);

    ret = reg_array_field_read(unit, MAPLE_LED_SW_P_CTRLr, port,REG_ARRAY_INDEX_NONE, field, &value);
    if (RT_ERR_OK != ret)
    {
        LED_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_LED), "");
        return ret;
    }

    LED_SEM_UNLOCK(unit);

    switch (value)
    {
        case 0:
            *pMode = RTK_LED_SWCTRL_MODE_OFF;
            break;
        case 1:
            *pMode = RTK_LED_SWCTRL_MODE_BLINKING_32MS;
            break;
        case 2:
            *pMode = RTK_LED_SWCTRL_MODE_BLINKING_64MS;
            break;
        case 3:
            *pMode = RTK_LED_SWCTRL_MODE_BLINKING_128MS;
            break;
        case 4:
            *pMode = RTK_LED_SWCTRL_MODE_BLINKING_512MS;
            break;
        case 5:
            *pMode = RTK_LED_SWCTRL_MODE_BLINKING_CONTINUE;
            break;
        case 6:
            *pMode = RTK_LED_SWCTRL_MODE_BLINKING_256MS;
            break;
        case 7:
            *pMode = RTK_LED_SWCTRL_MODE_BLINKING_1024MS;
            break;
        default:
            RT_LOG(LOG_DEBUG, (MOD_LED|MOD_DAL), "ret = 0x%x", ret);
            return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_maple_led_portLedEntitySwCtrlMode_get */

/* Function Name:
 *      dal_maple_led_portLedEntitySwCtrlMode_set
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
int32
dal_maple_led_portLedEntitySwCtrlMode_set(
    uint32                  unit,
    rtk_port_t              port,
    uint32                  entity,
    rtk_port_media_t        media,
    rtk_led_swCtrl_mode_t   mode)
{
    uint32  value;
    uint32  field;
    int32   ret = RT_ERR_INPUT;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_LED), "unit=%d, port=%d, entity=%d, media=%d, mode=%d",
            unit, port, entity, media, mode);

    /* check Init status */
    RT_INIT_CHK(led_init[unit]);

    /* parameter check */
    RT_PARAM_CHK(!HWP_ETHER_PORT(unit, port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((entity >= HAL_MAX_NUM_OF_LED_ENTITY(unit)), RT_ERR_INPUT);

    switch (media)
    {
        case PORT_MEDIA_COPPER:
        case PORT_MEDIA_FIBER:
            switch (entity)
            {
                case 0:
                    field = MAPLE_SW_P_LED0_MODEf;
                    break;
                case 1:
                    field = MAPLE_SW_P_LED1_MODEf;
                    break;
                case 2:
                    field = MAPLE_SW_P_LED2_MODEf;
                    break;
                default:
                    RT_ERR(ret, (MOD_DAL|MOD_LED), "");
                    return RT_ERR_INPUT;
            }
            break;
        default:
            RT_LOG(LOG_DEBUG, (MOD_LED|MOD_DAL), "ret = 0x%x", ret);
            return RT_ERR_INPUT;
    }

    switch (mode)
    {
        case RTK_LED_SWCTRL_MODE_OFF:
            value = 0;
            break;
        case RTK_LED_SWCTRL_MODE_BLINKING_32MS:
            value = 1;
            break;
        case RTK_LED_SWCTRL_MODE_BLINKING_64MS:
            value = 2;
            break;
        case RTK_LED_SWCTRL_MODE_BLINKING_128MS:
            value = 3;
            break;
        case RTK_LED_SWCTRL_MODE_BLINKING_512MS:
            value = 4;
            break;
        case RTK_LED_SWCTRL_MODE_BLINKING_CONTINUE:
            value = 5;
            break;
        case RTK_LED_SWCTRL_MODE_BLINKING_256MS:
            value = 6;
            break;
        case RTK_LED_SWCTRL_MODE_BLINKING_1024MS:
            value = 7;
            break;
        default:
            RT_LOG(LOG_DEBUG, (MOD_LED|MOD_DAL), "ret = 0x%x", ret);
            return RT_ERR_INPUT;
    }

    LED_SEM_LOCK(unit);

    ret = reg_array_field_write(unit, MAPLE_LED_SW_P_CTRLr, port,
            REG_ARRAY_INDEX_NONE, field, &value);
    if (RT_ERR_OK != ret)
    {
        LED_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_LED), "");
        return ret;
    }

    LED_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_led_portLedEntitySwCtrlMode_set */

/* Function Name:
 *      dal_maple_led_sysMode_get
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
 *      - RTK_LED_SWCTRL_MODE_BLINKING_64MS
 *      - RTK_LED_SWCTRL_MODE_BLINKING_1024MS
 *      - RTK_LED_SWCTRL_MODE_BLINKING_CONTINUE
 */
int32
dal_maple_led_sysMode_get(uint32 unit, rtk_led_swCtrl_mode_t *pMode)
{
    uint32  action;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_LED), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(led_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pMode), RT_ERR_NULL_POINTER);

    LED_SEM_LOCK(unit);

    ret = reg_field_read(unit, MAPLE_LED_GLB_CTRLr, MAPLE_SYS_LED_MODEf,
            &action);
    if (RT_ERR_OK != ret)
    {
        LED_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_LED), "");
        return ret;
    }

    LED_SEM_UNLOCK(unit);

    switch (action)
    {
        case 0:
            *pMode = RTK_LED_SWCTRL_MODE_OFF;
            break;
        case 1:
            *pMode = RTK_LED_SWCTRL_MODE_BLINKING_64MS;
            break;
        case 2:
            *pMode = RTK_LED_SWCTRL_MODE_BLINKING_1024MS;
            break;
        case 3:
            *pMode = RTK_LED_SWCTRL_MODE_BLINKING_CONTINUE;
            break;
        default:
            RT_LOG(LOG_DEBUG, (MOD_LED|MOD_DAL), "ret = 0x%x", ret);
            return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_maple_led_sysMode_get */

/* Function Name:
 *      dal_maple_led_sysMode_set
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
 *      - RTK_LED_SWCTRL_MODE_BLINKING_64MS
 *      - RTK_LED_SWCTRL_MODE_BLINKING_1024MS
 *      - RTK_LED_SWCTRL_MODE_BLINKING_CONTINUE
 */
int32
dal_maple_led_sysMode_set(uint32 unit, rtk_led_swCtrl_mode_t mode)
{
    uint32  action;
    int32   ret = RT_ERR_INPUT;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_LED), "unit=%d, mode=%d", unit, mode);

    /* check Init status */
    RT_INIT_CHK(led_init[unit]);

    /* parameter check */

    switch (mode)
    {
        case RTK_LED_SWCTRL_MODE_OFF:
            action = 0;
            break;
        case RTK_LED_SWCTRL_MODE_BLINKING_64MS:
            action = 1;
            break;
        case RTK_LED_SWCTRL_MODE_BLINKING_1024MS:
            action = 2;
            break;
        case RTK_LED_SWCTRL_MODE_BLINKING_CONTINUE:
            action = 3;
            break;
        default:
            RT_LOG(LOG_DEBUG, (MOD_LED|MOD_DAL), "ret = 0x%x", ret);
            return RT_ERR_INPUT;
    }

    LED_SEM_LOCK(unit);

    ret = reg_field_write(unit, MAPLE_LED_GLB_CTRLr, MAPLE_SYS_LED_MODEf,
            &action);
    if (RT_ERR_OK != ret)
    {
        LED_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_LED), "");
        return ret;
    }

    LED_SEM_UNLOCK(unit);

    return RT_ERR_OK;
} /* end of dal_maple_led_sysMode_set */

/* Function Name:
 *      dal_maple_led_sysEnable_get
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
int32
dal_maple_led_sysEnable_get(uint32 unit, rtk_led_type_t type, rtk_enable_t *pEnable)
{
    uint32  action = 0;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_LED), "unit=%d, type=%d",
           unit, type);

    /* check Init status */
    RT_INIT_CHK(led_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((type != LED_TYPE_SYS), RT_ERR_INPUT);
    RT_PARAM_CHK(NULL == pEnable, RT_ERR_NULL_POINTER);

    LED_SEM_LOCK(unit);

    ret = reg_field_read(unit, MAPLE_LED_GLB_CTRLr, MAPLE_SYS_LED_ENf,
            &action);
    if (RT_ERR_OK != ret)
    {
        LED_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_LED), "");
        return ret;
    }

    LED_SEM_UNLOCK(unit);

    if (0 == action)
        *pEnable = DISABLED;
    else
        *pEnable = ENABLED;

    return ret;
} /* end of dal_maple_led_sysEnable_get */

/* Function Name:
 *      dal_maple_led_sysEnable_set
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
int32
dal_maple_led_sysEnable_set(uint32 unit, rtk_led_type_t type, rtk_enable_t enable)
{
    uint32  action = 0;
    int32   ret;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_LED), "unit=%d, type=%d, enable=%d",
           unit, type, enable);

    /* check Init status */
    RT_INIT_CHK(led_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((type != LED_TYPE_SYS), RT_ERR_INPUT);
    RT_PARAM_CHK(enable >= RTK_ENABLE_END, RT_ERR_INPUT);

    LED_SEM_LOCK(unit);

    if (ENABLED == enable)
        action = 1;
    else
        action = 0;

    ret = reg_field_write(unit, MAPLE_LED_GLB_CTRLr, MAPLE_SYS_LED_ENf,
            &action);
    if (RT_ERR_OK != ret)
    {
        LED_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_LED), "");
        return ret;
    }

    LED_SEM_UNLOCK(unit);
    return ret;
} /* end of dal_maple_led_sysEnable_set */

/* Function Name:
 *      dal_maple_led_blinkTime_get
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
int32
dal_maple_led_blinkTime_get(uint32 unit, rtk_led_blinkTime_t *pTime)
{
    int32   ret = RT_ERR_OK;
    uint32  blkTime;
    uint32  blkTime_lowBit,blkTime_highBit;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_LED), "unit=%d", unit);

    /* check Init status */
    RT_INIT_CHK(led_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((NULL == pTime), RT_ERR_NULL_POINTER);

    /* function body */
    LED_SEM_LOCK(unit);

    ret = reg_field_read(unit, MAPLE_LED_GLB_CTRLr, MAPLE_BLINK_TIME_SELf,
            &blkTime_lowBit);

    if (RT_ERR_OK != ret)
    {
        LED_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_LED), "");
        return ret;
    }

    ret = reg_field_read(unit, MAPLE_LED_GLB_CTRLr, MAPLE_BLINK_TIME_SEL_2f,
            &blkTime_highBit);

    if (RT_ERR_OK != ret)
    {
        LED_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_LED), "");
        return ret;
    }

    LED_SEM_UNLOCK(unit);

    blkTime = (blkTime_highBit << 2) | blkTime_lowBit;

    switch (blkTime)
    {
        case 0:
            *pTime = RTK_LED_BLINKTIME_32MS;
            break;
        case 1:
            *pTime = RTK_LED_BLINKTIME_64MS;
            break;
        case 2:
            *pTime = RTK_LED_BLINKTIME_128MS;
            break;
        case 3:
            *pTime = RTK_LED_BLINKTIME_512MS;
            break;
        case 4:
            *pTime = RTK_LED_BLINKTIME_256MS;
            break;
        case 5:
            *pTime = RTK_LED_BLINKTIME_1024MS;
            break;
        default:
            RT_LOG(LOG_DEBUG, (MOD_LED|MOD_DAL), "ret = 0x%x", ret);
            return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}   /* end of dal_maple_led_blinkTime_get */

/* Function Name:
 *      dal_maple_led_blinkTime_set
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
int32
dal_maple_led_blinkTime_set(uint32 unit, rtk_led_blinkTime_t time)
{
    int32   ret = RT_ERR_OK;
    uint32  value;
    uint32  blkTime_lowBit = 0, blkTime_highBit = 0;

    RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_LED), "unit=%d,time=%d", unit, time);

    /* check Init status */
    RT_INIT_CHK(led_init[unit]);

    /* parameter check */
    RT_PARAM_CHK((RTK_LED_BLINKTIME_END <= time), RT_ERR_INPUT);

    /* function body */
    switch (time)
    {
        case RTK_LED_BLINKTIME_32MS:
            value = 0;
            break;
        case RTK_LED_BLINKTIME_64MS:
            value = 1;
            break;
        case RTK_LED_BLINKTIME_128MS:
            value = 2;
            break;
        case RTK_LED_BLINKTIME_256MS:
            value = 4;
            break;
        case RTK_LED_BLINKTIME_512MS:
            value = 3;
            break;
        case RTK_LED_BLINKTIME_1024MS:
            value = 5;
            break;
        default:
            RT_LOG(LOG_DEBUG, (MOD_LED|MOD_DAL), "ret = 0x%x", ret);
            return RT_ERR_INPUT;
    }

    LED_SEM_LOCK(unit);

    blkTime_lowBit = value & 0x3;
    blkTime_highBit = (value >> 2) & 0x1;

    ret = reg_field_write(unit, MAPLE_LED_GLB_CTRLr, MAPLE_BLINK_TIME_SELf,
            &blkTime_lowBit);

    if (RT_ERR_OK != ret)
    {
        LED_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_LED), "");
        return ret;
    }

    ret = reg_field_write(unit, MAPLE_LED_GLB_CTRLr, MAPLE_BLINK_TIME_SEL_2f,
            &blkTime_highBit);

    if (RT_ERR_OK != ret)
    {
        LED_SEM_UNLOCK(unit);
        RT_ERR(ret, (MOD_DAL|MOD_LED), "");
        return ret;
    }

    LED_SEM_UNLOCK(unit);

    return ret;
}   /* end of dal_maple_led_blinkTime_set */

