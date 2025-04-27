/*
 * Realtek Semiconductor Corp.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 *
 * Copyright (C) 2015 Tony Wu <tonywu@realtek.com>
 */

#include <common/rt_type.h>
#include <linux/version.h>

#include <linux/of_fdt.h>
#include <linux/of_platform.h>

#include <asm/prom.h>

#include "rtk_bsp_driver.h"

void __init device_tree_init(void)
{
    if (!initial_boot_params)
        return;

    unflatten_and_copy_device_tree();
}

#if LINUX_VERSION_CODE < KERNEL_VERSION(3,19,0)
static int __init plat_of_setup(void)
{
    static struct of_device_id of_ids[3];
    int len = sizeof(of_ids[0].compatible);

    if (!of_have_populated_dt())
        panic("device tree not present");

    strlcpy(of_ids[0].compatible, "rtk,sheipa", len);
    strlcpy(of_ids[1].compatible, "sheipa-bus", len);

    if (of_platform_populate(NULL, of_ids, NULL, NULL))
        panic("failed to populate DT");

    return 0;
}
#else
static int __init plat_of_setup(void)
{
#ifdef CONFIG_SMP
    int ret = SDK_BSP_FAILED;
#endif
    rtk_bspDeviceTreeBus_setup();

#ifdef CONFIG_SMP
    ret = rtk_cpuExtTCSrcIRQ_restInit();
    if(ret != SDK_BSP_OK)
    {
        RTK_BSP_DBG_MSG("\n[%s][%d] rtk_cpuExtTCSrcIRQ_restInit failed\n",__FUNCTION__,__LINE__);
        return ret;
    }
#endif
    return SDK_BSP_OK;
}
#endif

arch_initcall(plat_of_setup);
