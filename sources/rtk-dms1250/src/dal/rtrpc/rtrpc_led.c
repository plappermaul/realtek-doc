/*
 * Copyright(c) Realtek Semiconductor Corporation, 2010
 * All rights reserved.
 *
 * $Revision: 79582 $
 * $Date: 2017-06-13 16:50:29 +0800 (Tue, 13 Jun 2017) $
 *
 * Purpose : Realtek Switch SDK rtrpc API Module
 *
 * Feature : The file have include the following module and sub-modules
 *           1) LED
 *
 */

#include <rtk/led.h>
#include <dal/rtrpc/rtrpc_led.h>
#include <dal/rtrpc/rtrpc_msg.h>
#include <rtdrv/rtdrv_netfilter.h>

int32 rtrpc_led_sysEnable_get(uint32 unit, rtk_led_type_t type, rtk_enable_t *pEnable)
{
    rtdrv_ledCfg_t led_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&led_cfg, 0, sizeof(rtdrv_ledCfg_t));
    led_cfg.unit = unit;
    led_cfg.type = type;
    GETSOCKOPT(RTDRV_LED_SYS_ENABLE_GET, &led_cfg, rtdrv_ledCfg_t, 1);
    *pEnable = led_cfg.enable;

    return RT_ERR_OK;
}

int32 rtrpc_led_sysEnable_set(uint32 unit, rtk_led_type_t type, rtk_enable_t enable)
{
    rtdrv_ledCfg_t led_cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&led_cfg, 0, sizeof(rtdrv_ledCfg_t));
    led_cfg.unit = unit;
    led_cfg.type = type;
    led_cfg.enable = enable;
    SETSOCKOPT(RTDRV_LED_SYS_ENABLE_SET, &led_cfg, rtdrv_ledCfg_t, 1);

    return RT_ERR_OK;
}

int32
rtrpc_led_portLedEntitySwCtrlEnable_get(uint32 unit, rtk_port_t port,
    uint32 entity, rtk_enable_t *pEnable)
{
    rtdrv_ledCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_ledCfg_t));
    cfg.unit = unit;
    cfg.port = port;
    cfg.entity = entity;
    GETSOCKOPT(RTDRV_LED_PORTLEDENTITYSWCTRLENABLE_GET, &cfg, rtdrv_ledCfg_t, 1);
    *pEnable = cfg.enable;

    return RT_ERR_OK;
}    /* end of rtk_led_portLedEntitySwCtrlEnable_get */

int32
rtrpc_led_portLedEntitySwCtrlEnable_set(uint32 unit, rtk_port_t port,
    uint32 entity, rtk_enable_t enable)
{
    rtdrv_ledCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_ledCfg_t));
    cfg.unit = unit;
    cfg.port = port;
    cfg.entity = entity;
    cfg.enable = enable;
    SETSOCKOPT(RTDRV_LED_PORTLEDENTITYSWCTRLENABLE_SET, &cfg, rtdrv_ledCfg_t, 1);

    return RT_ERR_OK;
}    /* end of rtk_led_portLedEntitySwCtrlEnable_set */

int32
rtrpc_led_swCtrl_start(uint32 unit)
{
    rtdrv_ledCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_ledCfg_t));
    cfg.unit = unit;
    SETSOCKOPT(RTDRV_LED_SWCTRL_START, &cfg, rtdrv_ledCfg_t, 1);

    return RT_ERR_OK;
}    /* end of rtk_led_swCtrl_start */

int32
rtrpc_led_portLedEntitySwCtrlMode_get(uint32 unit, rtk_port_t port,
    uint32 entity, rtk_port_media_t media, rtk_led_swCtrl_mode_t *pMode)
{
    rtdrv_ledCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_ledCfg_t));
    cfg.unit = unit;
    cfg.port = port;
    cfg.entity = entity;
    cfg.media = media;
    GETSOCKOPT(RTDRV_LED_PORTLEDENTITYSWCTRLMODE_GET, &cfg, rtdrv_ledCfg_t, 1);
    *pMode = cfg.mode;

    return RT_ERR_OK;
}    /* end of rtk_led_portLedEntitySwCtrlMode_get */

int32
rtrpc_led_portLedEntitySwCtrlMode_set(uint32 unit, rtk_port_t port,
    uint32 entity, rtk_port_media_t media, rtk_led_swCtrl_mode_t mode)
{
    rtdrv_ledCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_ledCfg_t));
    cfg.unit = unit;
    cfg.port = port;
    cfg.entity = entity;
    cfg.media = media;
    cfg.mode = mode;
    SETSOCKOPT(RTDRV_LED_PORTLEDENTITYSWCTRLMODE_SET, &cfg, rtdrv_ledCfg_t, 1);

    return RT_ERR_OK;
}    /* end of rtk_led_portLedEntitySwCtrlMode_set */

int32
rtrpc_led_sysMode_get(uint32 unit, rtk_led_swCtrl_mode_t *pMode)
{
    rtdrv_ledCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_ledCfg_t));
    cfg.unit = unit;
    GETSOCKOPT(RTDRV_LED_SYSMODE_GET, &cfg, rtdrv_ledCfg_t, 1);
    *pMode = cfg.mode;

    return RT_ERR_OK;
}    /* end of rtk_led_sysMode_get */

int32
rtrpc_led_sysMode_set(uint32 unit, rtk_led_swCtrl_mode_t mode)
{
    rtdrv_ledCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_ledCfg_t));
    cfg.unit = unit;
    cfg.mode = mode;
    SETSOCKOPT(RTDRV_LED_SYSMODE_SET, &cfg, rtdrv_ledCfg_t, 1);

    return RT_ERR_OK;
}    /* end of rtk_led_sysMode_set */

int32
rtrpc_led_blinkTime_get(uint32 unit, rtk_led_blinkTime_t *pTime)
{
    rtdrv_ledCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* parameter check */
    RT_PARAM_CHK((NULL == pTime), RT_ERR_NULL_POINTER);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_ledCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    GETSOCKOPT(RTDRV_LED_BLINKTIME_GET, &cfg, rtdrv_ledCfg_t, 1);
    osal_memcpy(pTime, &cfg.time, sizeof(rtk_led_blinkTime_t));

    return RT_ERR_OK;
}   /* end of rtk_led_blinkTime_get */

int32
rtrpc_led_blinkTime_set(uint32 unit, rtk_led_blinkTime_t time)
{
    rtdrv_ledCfg_t cfg;
    uint32 master_view_unit = unit;
    RTRPC_UNIT_ID_XLATE(master_view_unit, &unit);

    /* function body */
    osal_memset(&cfg, 0, sizeof(rtdrv_ledCfg_t));
    osal_memcpy(&cfg.unit, &unit, sizeof(uint32));
    osal_memcpy(&cfg.time, &time, sizeof(rtk_led_blinkTime_t));
    SETSOCKOPT(RTDRV_LED_BLINKTIME_SET, &cfg, rtdrv_ledCfg_t, 1);

    return RT_ERR_OK;
}   /* end of rtk_led_blinkTime_set */


int32
rtrpc_ledMapper_init(dal_mapper_t *pMapper)
{
    pMapper->led_sysEnable_get = rtrpc_led_sysEnable_get;
    pMapper->led_sysEnable_set = rtrpc_led_sysEnable_set;
    pMapper->led_portLedEntitySwCtrlEnable_get = rtrpc_led_portLedEntitySwCtrlEnable_get;
    pMapper->led_portLedEntitySwCtrlEnable_set = rtrpc_led_portLedEntitySwCtrlEnable_set;
    pMapper->led_portLedEntitySwCtrlMode_get = rtrpc_led_portLedEntitySwCtrlMode_get;
    pMapper->led_portLedEntitySwCtrlMode_set = rtrpc_led_portLedEntitySwCtrlMode_set;
    pMapper->led_swCtrl_start = rtrpc_led_swCtrl_start;
    pMapper->led_sysMode_get = rtrpc_led_sysMode_get;
    pMapper->led_sysMode_set = rtrpc_led_sysMode_set;
    pMapper->led_blinkTime_get = rtrpc_led_blinkTime_get;
    pMapper->led_blinkTime_set = rtrpc_led_blinkTime_set;
    return RT_ERR_OK;
}
