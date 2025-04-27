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

#ifdef CONFIG_CEVT_EXT
extern void ext_clockevent_init(void);
// extern void vpe_local_disable(void);
#endif
/**************************************************
 * For VPE1,2....n initialization
**************************************************/

void __cpuinit plat_smp_init_secondary(void)
{
        int offset = 0;
#ifdef CONFIG_CEVT_EXT
    offset = smp_processor_id()*0x10;
/* Do not disable count, because linux-3.x will use count vaule do somethime.TODO Why and where */
    /* Disable internal Count register */
// 	write_c0_cause(read_c0_cause() | (1 << 27));


    /* Clear internal timer interrupt */
    write_c0_compare(0);

        ext_clockevent_init();
    REG32(RTL9300MP_TC0INT + offset) = RTL9300MP_TCIE;
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
