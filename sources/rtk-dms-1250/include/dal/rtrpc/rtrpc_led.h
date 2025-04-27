/*
* Copyright(c) Realtek Semiconductor Corporation, 2008
* All rights reserved.
*/
#ifndef __RTRPC_LED_H__
#define __RTRPC_LED_H__

/*
* Include Files
*/
#include <dal/dal_mapper.h>
#define rtk_led_sysEnable_get                            rtrpc_led_sysEnable_get
#define rtk_led_sysEnable_set                            rtrpc_led_sysEnable_set
#define rtk_led_portLedEntitySwCtrlEnable_get            rtrpc_led_portLedEntitySwCtrlEnable_get
#define rtk_led_portLedEntitySwCtrlEnable_set            rtrpc_led_portLedEntitySwCtrlEnable_set
#define rtk_led_swCtrl_start                             rtrpc_led_swCtrl_start
#define rtk_led_portLedEntitySwCtrlMode_get              rtrpc_led_portLedEntitySwCtrlMode_get
#define rtk_led_portLedEntitySwCtrlMode_set              rtrpc_led_portLedEntitySwCtrlMode_set
#define rtk_led_sysMode_get                              rtrpc_led_sysMode_get
#define rtk_led_sysMode_set                              rtrpc_led_sysMode_set
#define rtk_led_blinkTime_get                            rtrpc_led_blinkTime_get
#define rtk_led_blinkTime_set                            rtrpc_led_blinkTime_set
/*
 * Function Declaration
 */

extern int32
rtrpc_led_init(uint32 unit);

extern int32
rtrpc_led_sysEnable_get(uint32 unit, rtk_led_type_t type, rtk_enable_t *pEnable);

extern int32
rtrpc_led_sysEnable_set(uint32 unit, rtk_led_type_t type, rtk_enable_t enable);

extern int32
rtrpc_led_portLedEntitySwCtrlEnable_get(
    uint32          unit,
    rtk_port_t      port,
    uint32          entity,
    rtk_enable_t    *pEnable);

extern int32
rtrpc_led_portLedEntitySwCtrlEnable_set(
    uint32          unit,
    rtk_port_t      port,
    uint32          entity,
    rtk_enable_t    enable);

extern int32
rtrpc_led_portLedEntitySwCtrlMode_get(
    uint32                  unit,
    rtk_port_t              port,
    uint32                  entity,
    rtk_port_media_t        media,
    rtk_led_swCtrl_mode_t   *pMode);

extern int32
rtrpc_led_portLedEntitySwCtrlMode_set(
    uint32                  unit,
    rtk_port_t              port,
    uint32                  entity,
    rtk_port_media_t        media,
    rtk_led_swCtrl_mode_t   mode);

extern int32
rtrpc_led_swCtrl_start(uint32 unit);

extern int32
rtrpc_led_sysMode_get(uint32 unit, rtk_led_swCtrl_mode_t *pMode);

extern int32
rtrpc_led_sysMode_set(uint32 unit, rtk_led_swCtrl_mode_t mode);

extern int32
rtrpc_led_blinkTime_get(uint32 unit, rtk_led_blinkTime_t *pTime);

extern int32
rtrpc_led_blinkTime_set(uint32 unit, rtk_led_blinkTime_t time);

extern int32
rtrpc_ledMapper_init(dal_mapper_t *pMapper);

#endif /* __RTRPC_LED_H__ */

