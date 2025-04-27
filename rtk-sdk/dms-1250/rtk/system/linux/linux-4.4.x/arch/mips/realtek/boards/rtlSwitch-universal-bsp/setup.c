/*
 * Copyright (C) 2009-2019 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 97155 $
 * $Date: 2019-11-10 14:22:30 +0800 (Sun, 10 Nov 2019) $
 *
 * Purpose : restart callback and memory region setup
 *
 *
 */


#include <common/rt_type.h>
#include <linux/of_fdt.h>

#include <asm/prom.h>
#include <asm/time.h>
#include <asm/reboot.h>
#include <asm/fw/fw.h>

#include "bspchip.h"
#include "rtk_bsp_driver.h"
#include "rtk_util.h"


extern char __dtb_start[];

static void plat_machine_restart(char *command)
{
    rtk_restartCallBack_execute();

    RTK_BSP_DBG_MSG("[RTK MS]System restart.\n");
    rtk_bspChip_reset();   /* Reset whole chip */
}

static void plat_machine_halt(void)
{
    RTK_BSP_DBG_MSG("[RTK MS]System halted.\n");
    while(1);
}

/* callback function */
void __init plat_mem_setup(void)
{
    int ret = SDK_BSP_FAILED;

    /* set reset vectors */
    _machine_restart = plat_machine_restart;
    _machine_halt = plat_machine_halt;
    pm_power_off = plat_machine_halt;

    rtk_bspLowMem_DMA_setup();

#ifdef CONFIG_BUILTIN_DTB
    /*
     * Load the builtin devicetree. This causes the chosen node to be
     * parsed resulting in our memory appearing
     */
    rtk_bspDeviceTree_get();
#endif

    ret = rtk_bspMemRegion_setup();
    if(ret != SDK_BSP_OK)
        RTK_BSP_DBG_MSG("Memory Region Setup failed.");

}
