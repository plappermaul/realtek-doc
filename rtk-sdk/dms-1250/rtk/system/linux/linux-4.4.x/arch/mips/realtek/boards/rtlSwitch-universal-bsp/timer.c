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
 * Purpose : bsp timer initialization code
 *
 *
 */

#include <common/rt_type.h>
#include <linux/version.h>
#include <linux/init.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,14,0)
#include <asm/mips-cps.h>
#else
#include <linux/irqchip/mips-gic.h>
#endif

#include "bspchip.h"
#include "rtk_bsp_driver.h"

#ifdef CONFIG_MIPS_GIC
#ifdef CONFIG_OF
#include <linux/version.h>
#include <linux/of.h>
#include <linux/clk-provider.h>
#include <linux/clocksource.h>
#endif

static void __init mips_gic_time_init(void)
{
#ifdef CONFIG_OF
    of_clk_init(NULL);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,13,0)
    timer_probe();
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(4,4,0)
    clocksource_probe();
#else
    clocksource_of_init();
#endif
#else
    gic_clocksource_init(BSP_CPU_FREQ);
#endif /* CONFIG_OF */
}
#else
static void __init mips_gic_time_init(void)
{
}
#endif /* CONFIG_MIPS_GIC */

#if defined(CONFIG_MIPS_GIC)
static void __init cevt_r4k_time_init(void)
{
    set_c0_cause(CAUSEF_DC);
    write_c0_count(0);
}
#elif !defined(CONFIG_CEVT_R4K)
static void __init cevt_r4k_time_init(void)
{
}
#endif

void __init plat_time_init(void)
{
    cevt_r4k_time_init();
    mips_gic_time_init();
}
