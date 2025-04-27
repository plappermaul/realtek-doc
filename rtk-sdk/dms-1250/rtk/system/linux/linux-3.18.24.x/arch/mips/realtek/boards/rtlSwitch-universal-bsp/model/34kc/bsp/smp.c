/*
 * Realtek Semiconductor Corp.
 *
 * bsp/vsmp.c
 *     bsp VSMP initialization code
 *
 * Copyright (C) 2006-2012 Tony Wu (tonywu@realtek.com)
 */

#include <linux/version.h>
#include <linux/smp.h>
#include <linux/interrupt.h>
#include <asm/smp-ops.h>

#include "bspchip.h"
#include "int_setup.h"
#include "bsp_ioal.h"

extern void __init vpe_local_disable(void);

#ifdef CONFIG_CEVT_EXT
extern void ext_clockevent_init(void);
// extern void vpe_local_disable(void);
#endif
/**************************************************
 * For VPE1,2....n initialization
**************************************************/

void _bsp_smp_cpu_timer_intr_enable(int cpu_id)
{
    int             offset = 0;
    unsigned int    timer_intr_baseAddr = TC93XX_BASE + RTL93XXMP_TC_INTREG_OFFSET;

    offset = ((bsp_cpu_timerId_irq[cpu_id].timer_id)*TC_REG_OFFSET);

    BSP_REG32(timer_intr_baseAddr + offset) = RTL93XXMP_TCIE;
    return;

}

void __cpuinit plat_smp_init_secondary(void)
{

#ifdef CONFIG_CEVT_EXT
    int cpu_id = 0;
    vpe_local_disable();
    cpu_id = smp_processor_id();

    /* Clear internal timer interrupt */
    write_c0_compare(0);

        ext_clockevent_init();
    _bsp_smp_cpu_timer_intr_enable(cpu_id);
#endif
    change_c0_status(ST0_IM, STATUSF_IP0 |STATUSF_IP1 |STATUSF_IP2 | STATUSF_IP3 |  STATUSF_IP4 |
                STATUSF_IP5 | STATUSF_IP6 | STATUSF_IP7 );

}


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
#ifdef  CONFIG_MIPS_CPS
    if (!register_cps_smp_ops())
        return;
#endif
#ifdef  CONFIG_MIPS_CMP
    if (!register_cmp_smp_ops())
        return;
#endif
#ifdef CONFIG_MIPS_MT_SMP
    if (!register_vsmp_smp_ops())
        return;
#endif
}
