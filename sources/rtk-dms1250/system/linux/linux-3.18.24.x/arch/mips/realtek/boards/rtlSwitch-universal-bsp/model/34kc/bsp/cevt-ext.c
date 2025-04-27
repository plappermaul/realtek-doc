/*
 * Copyright 2008, Realtek Semiconductor Corp.
 *
 * Tony Wu (tonywu@realtek.com)
 * Dec. 07, 2008
 */
#include <linux/clockchips.h>
#include <linux/init.h>
#include <linux/interrupt.h>

#include <asm/time.h>
#include "bspchip.h"
#include "chip_probe.h"
#include "int_setup.h"
#include "prom.h"

extern void bsp_timer_ack(void);

int ext_timer_state(void)
{
    return 0;
}

int ext_timer_set_base_clock(unsigned int hz)
{
    return 0;
}

static int ext_timer_set_next_event(unsigned long delta,
                 struct clock_event_device *evt)
{
    return -EINVAL;
}

static void ext_timer_set_mode(enum clock_event_mode mode,
                struct clock_event_device *evt)
{
    return;
}

static void ext_timer_event_handler(struct clock_event_device *dev)
{
}



DEFINE_PER_CPU(struct clock_event_device, ext_clockevent_device);

static irqreturn_t ext_timer_interrupt(int irq, void *dev_id)
{
    struct clock_event_device *cd;

    cd = &per_cpu(ext_clockevent_device, smp_processor_id());

    /* Ack the RTC interrupt. */
    bsp_timer_ack();

    cd->event_handler(cd);
    return IRQ_HANDLED;
}

static struct irqaction ext_irqaction = {
    .handler	= ext_timer_interrupt,
    .flags		= IRQF_TIMER | IRQF_PERCPU,
    .name		= "EXT_TIMER",
};

int __cpuinit ext_clockevent_init(void)
{
    struct clock_event_device *cd;
    unsigned int irq;
    unsigned int cpu = smp_processor_id();
    cd = &per_cpu(ext_clockevent_device, cpu);

    if(cpu < NR_CPUS)
    {
        irq = IRQ_CONV_ICTL(bsp_cpu_timerId_irq[cpu].mapped_system_irq);
    }else{
        printk("Error CPU ID.");
        return -1;
    }

    cd->name		= "EXT_TIMER";
    cd->features	= CLOCK_EVT_FEAT_PERIODIC;

    cd->event_handler	= ext_timer_event_handler;
    cd->set_next_event	= ext_timer_set_next_event;
    cd->set_mode	= ext_timer_set_mode;

    cd->rating = 100;
    cd->irq = irq;

    clockevent_set_clock(cd, 32768);
    cd->max_delta_ns = clockevent_delta2ns(0x7fffffff, cd);
    cd->min_delta_ns = clockevent_delta2ns(0x300, cd);
    cd->cpumask = cpumask_of(cpu);

    clockevents_register_device(cd);

    setup_irq(irq, &ext_irqaction);
    irq_set_handler(irq, handle_percpu_irq);

    return 0;
}
