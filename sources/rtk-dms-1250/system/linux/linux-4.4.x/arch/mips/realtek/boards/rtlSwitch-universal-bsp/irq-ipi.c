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
 * Purpose : IPI initialization and handlers
 *
 */

#if defined(CONFIG_SMP) \
    && !(defined(CONFIG_MIPS_CMP) || defined(CONFIG_MIPS_CPS))
#include <common/rt_type.h>
#include <linux/sched.h>
#include <linux/interrupt.h>

/*
 * Handle SMP IPI interrupts
 *
 * Two IPI interrupts, resched and call, are handled here.
 */
static irqreturn_t plat_ipi_resched(int irq, void *devid)
{
    scheduler_ipi();
    return IRQ_HANDLED;
}

static irqreturn_t plat_ipi_call(int irq, void *devid)
{
    generic_smp_call_function_interrupt();
    return IRQ_HANDLED;
}

static struct irqaction irq_resched __maybe_unused = {
    .handler    = plat_ipi_resched,
    .flags      = IRQF_PERCPU,
    .name       = "IPI resched"
};

static struct irqaction irq_call __maybe_unused = {
    .handler    = plat_ipi_call,
    .flags      = IRQF_PERCPU,
    .name       = "IPI call"
};

/*
 * Initialize IPI interrupts.
 *
 * In MIPS_MT_SMP mode, IPI interrupts are routed via SW0 and SW1.
 * When GIC is present, IPI interrupts are routed via GIC.
 */
static void __init plat_ipi_init(void)
{
#if defined(CONFIG_IRQ_GIC) || defined(CONFIG_MIPS_GIC)
    int irq;
    int cpu;

    /* setup GIC IPI interrupts */
    gic_setup_ipi(GIC_CPU_INT1, GIC_CPU_INT2);

    for (cpu = 0; cpu < NR_CPUS; cpu++) {
        irq = MIPS_GIC_IRQ_BASE + GIC_IPI_RESCHED(cpu);
        setup_irq(irq, &irq_resched);
        irq_set_handler(irq, handle_percpu_irq);

        irq = MIPS_GIC_IRQ_BASE + GIC_IPI_CALL(cpu);
        setup_irq(irq, &irq_call);
        irq_set_handler(irq, handle_percpu_irq);
    }
#elif defined(CONFIG_MIPS_MT_SMP)
    setup_irq(0, &irq_resched);
    setup_irq(1, &irq_call);
#endif
}

#else

static void __init plat_ipi_init(void)
{
}

#endif
