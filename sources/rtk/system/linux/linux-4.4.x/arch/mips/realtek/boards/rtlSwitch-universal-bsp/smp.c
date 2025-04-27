/*
 * Realtek Semiconductor Corp.
 *
 * bsp/smp.c
 *     bsp SMP initialization code
 *
 * Copyright (C) 2006-2012 Tony Wu (tonywu@realtek.com)
 */
#include <common/rt_type.h>
#include <linux/smp.h>
#include <linux/interrupt.h>

#include <asm/smp-ops.h>

#ifdef CONFIG_MIPS_CM
#include <asm/mips-cm.h>
#include <asm/mips-cpc.h>
#endif

#include "bspchip.h"

/*
 * Called in kernel/smp-*.c to do secondary CPU initialization.
 *
 * All platform-specific initialization should be implemented in
 * this function.
 *
 * Known SMP callers are:
 *     kernel/smp-mt.c
 *     kernel/smp-cmp.c
 *
 * This function is called by secondary CPUs.
 */
void plat_smp_init_secondary(void)
{
#ifdef CONFIG_MIPS_GIC
    set_c0_cause(CAUSEF_DC);
    write_c0_count(0);
#endif
    change_c0_status(ST0_IM, 0xff00);
}

#ifdef CONFIG_MIPS_CM
phys_addr_t mips_cpc_default_phys_base(void)
{
    return CPC_BASE_ADDR;
}
#endif

/*
 * Called in bsp/setup.c to initialize SMP operations
 *
 * Depends on SMP type, plat_smp_init calls corresponding
 * SMP operation initializer in arch/mips/kernel
 *
 * Known SMP types are:
 *     MIPS_CMP
 *     MIPS_MT_SMP
 *     MIPS_CMP + MIPS_MT_SMP: 1004K (use MIPS_CMP)
 */
void __init plat_smp_init(void)
{
#if defined(CONFIG_MIPS_CM)
    mips_cm_probe();
    mips_cpc_probe();
#elif defined(CONFIG_MIPS_CMP)
    gcmp_probe(GCMP_BASE_ADDR, GCMP_BASE_SIZE);
#endif

    if (!register_cps_smp_ops())
        return;
}
