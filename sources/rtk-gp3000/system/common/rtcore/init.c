/*
 * Copyright (C) 2009 Realtek Semiconductor Corp.
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
 * Purpose : Realtek Switch SDK Core Module.
 *
 * Feature : Realtek Switch SDK Core Module
 *
 */

/*
 * Include Files
 */
//#include <common/rt_autoconf.h>
#include <common/debug/rt_log.h>
#include <ioal/ioal_init.h>
//#include <drv/nic/probe.h>
//#include <drv/watchdog/probe.h>
#if defined(CONFIG_SDK_RTL8231)
#include <drv/rtl8231/probe.h>
#endif

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */

/*
 * Macro Declaration
 */

/*
 * Function Declaration
 */

/* Function Name:
 *      rtcore_init
 * Description:
 *      Initialize RTCORE module with the specified device
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      1. INIT must be initialized before using all of APIs in each modules
 */
int32
rtcore_init(void)
{
    int32 ret = RT_ERR_FAILED;

    /* Will be used by all layers, Initialize first */
    RT_ERR_CHK(rt_log_init(), ret);

    /* Initialize the ioal layer */
    RT_ERR_CHK(ioal_init(), ret);

#if 0
#if !defined(CONFIG_SDK_DRIVER_NIC_USER_MODE)
    /* Probe the nic */
    RT_ERR_CHK(nic_probe(unit), ret);
#endif
#endif

#if 0
    /* Probe the watchdog */
    RT_ERR_CHK(watchdog_probe(), ret);
#endif

#if defined(CONFIG_SDK_RTL8231)
    RT_ERR_CHK(rtl8231_probe(unit), ret);
#endif

    return ret;

} /* end of rtcore_init */

