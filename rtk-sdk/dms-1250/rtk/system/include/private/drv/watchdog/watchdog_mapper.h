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
 * Purpose : Definition those public watchdog APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) watchdog probe
 *
 */
#ifndef __DRV_WATCHDOG_MAPPER_H__
#define __DRV_WATCHDOG_MAPPER_H__

/*
 * Include Files
 */
#include <common/rt_autoconf.h>
#include <drv/watchdog/watchdog.h>

/*
 * Symbol Definition
 */
typedef enum wdg_controller_e
{
#if defined(CONFIG_SDK_RTL9310)
    WDG_R9310,
#endif
#if defined(CONFIG_SDK_RTL9300)
    WDG_R9300,
#endif
#if defined(CONFIG_SDK_RTL8390)
    WDG_R8390,
#endif
#if defined(CONFIG_SDK_RTL8380)
    WDG_R8380,
#endif
    WDG_CTRL_END
} wdg_controller_t;

typedef struct wdg_mapper_operation_s
{
    /* WATCHDOG */
    int32   (*scale_set)(uint32, drv_watchdog_scale_t);
    int32   (*scale_get)(uint32, drv_watchdog_scale_t *);
    int32   (*enable_set)(uint32, uint32);
    int32   (*enable_get)(uint32, uint32 *);
    int32   (*kick)(uint32);
    int32   (*init)(uint32);
    int32   (*threshold_set)(uint32, drv_watchdog_threshold_t *);
    int32   (*threshold_get)(uint32, drv_watchdog_threshold_t *);
} wdg_mapper_operation_t;

#define WDG_CTRL(unit)      wdg_ops[wdg_if[unit]]

/*
 * Data Declaration
 */
extern wdg_mapper_operation_t wdg_ops[];
extern uint32 wdg_if[];

/*
 * Macro Definition
 */

/*
 * Function Declaration
 */


#endif /* __DRV_WATCHDOG_MAPPER_H__ */

