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

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <private/drv/swcore/swcore.h>
#include <private/drv/swcore/chip_probe.h>
#include <hwp/hw_profile.h>
#include <private/drv/watchdog/watchdog_mapper.h>
#include <private/drv/watchdog/watchdog_common.h>
#include <common/debug/rt_log.h>
/*
 * Symbol Definition
 */
#define WDG_DB_SIZE (sizeof(wdg_db)/sizeof(cid_prefix_group_t))

/*
 * Data Declaration
 */
const static cid_prefix_group_t wdg_db[] =
{
#if defined(CONFIG_SDK_RTL9310)
    /* RTL9300 series chips */
    {RTL9310_FAMILY_ID, WDG_R9310},
#endif
#if defined(CONFIG_SDK_RTL9300)
    /* RTL9300 series chips */
    {RTL9300_FAMILY_ID, WDG_R9300},
#endif
#if defined(CONFIG_SDK_RTL8390)
    /* RTL8390 series chips */
    {RTL8390_FAMILY_ID, WDG_R8390},
    {RTL8350_FAMILY_ID, WDG_R8390},
#endif
#if defined(CONFIG_SDK_RTL8380)
    /* RTL8380 series chips */
    {RTL8380_FAMILY_ID, WDG_R8380},
    {RTL8330_FAMILY_ID, WDG_R8380},
#endif
};

wdg_mapper_operation_t wdg_ops[WDG_CTRL_END] =
{
#if defined(CONFIG_SDK_RTL9310)
    {   /* WDG_R9310 */
        .scale_set = watchdog_scale_set,
        .scale_get = watchdog_scale_get,
        .enable_set = watchdog_enable_set,
        .enable_get = watchdog_enable_get,
        .kick = watchdog_kick,
        .init = watchdog_init,
        .threshold_set = watchdog_threshold_set,
        .threshold_get = watchdog_threshold_get,
    },
#endif
#if defined(CONFIG_SDK_RTL9300)
    {   /* WDG_R9300 */
        .scale_set = watchdog_scale_set,
        .scale_get = watchdog_scale_get,
        .enable_set = watchdog_enable_set,
        .enable_get = watchdog_enable_get,
        .kick = watchdog_kick,
        .init = watchdog_init,
        .threshold_set = watchdog_threshold_set,
        .threshold_get = watchdog_threshold_get,

    },
#endif
#if defined(CONFIG_SDK_RTL8390)
    {   /* WDG_R8390 */
        .scale_set = watchdog_scale_set,
        .scale_get = watchdog_scale_get,
        .enable_set = watchdog_enable_set,
        .enable_get = watchdog_enable_get,
        .kick = watchdog_kick,
        .init = watchdog_init,
        .threshold_set = watchdog_threshold_set,
        .threshold_get = watchdog_threshold_get,

    },
#endif
#if defined(CONFIG_SDK_RTL8380)
    {   /* WDG_R8380 */
        .scale_set = watchdog_scale_set,
        .scale_get = watchdog_scale_get,
        .enable_set = watchdog_enable_set,
        .enable_get = watchdog_enable_get,
        .kick = watchdog_kick,
        .init = watchdog_init,
        .threshold_set = watchdog_threshold_set,
        .threshold_get = watchdog_threshold_get,
    },
#endif
};

uint32 wdg_if[RTK_MAX_NUM_OF_UNIT] = {CID_GROUP_NONE};
uint32 wdg_chipId[RTK_MAX_NUM_OF_UNIT];

/*
 * Function Declaration
 */

/* Function Name:
 *      watchdog_probe
 * Description:
 *      Probe watchdog module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
int32
watchdog_probe(uint32 unit)
{
    uint32      i;
    uint32      cid;

    RT_INIT_MSG("  Watchdog probe (unit %u): ", unit);

    cid = HWP_CHIP_ID(unit);

    wdg_if[unit] = CID_GROUP_NONE;
    for (i = 0; i < WDG_DB_SIZE; i++)
    {
        if (CID_PREFIX_MATCH(wdg_db[i], cid))
        {
            wdg_if[unit] = wdg_db[i].gid;
            wdg_chipId[unit] = cid;
            RT_INIT_MSG("(found)\n");
            return RT_ERR_OK;
        }
    }

    RT_INIT_MSG("(Not found)\n");
    return RT_ERR_FAILED;

} /* end of watchdog_probe */
