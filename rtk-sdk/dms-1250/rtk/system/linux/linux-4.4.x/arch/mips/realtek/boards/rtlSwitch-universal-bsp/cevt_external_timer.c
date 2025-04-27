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
 * Purpose : CEVT External Timer driver
 *
 * Feature : CEVT External Timer driver
 *
 */
#include <common/rt_type.h>
#include <linux/clockchips.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/cpu.h>
#include <linux/slab.h>
#include <linux/of_fdt.h>
#include <linux/of_platform.h>
#include <linux/of_irq.h>

#include <asm/time.h>
#include <asm/string.h>

#include "bspchip.h"
#include "chip_probe.h"
#include "rtk_bsp_driver.h"


#define TIMER_NAME_SIZE  15

DEFINE_PER_CPU(struct clock_event_device, ext_clockevent_device);

static int ext_timer_set_next_event(unsigned long delta,
                    struct clock_event_device *evt)
{
    return -EINVAL;
}

static void ext_timer_event_handler(struct clock_event_device *dev)
{
}


irqreturn_t cevt_extTimer_ack(int irq, void *dev_id)
{
    struct clock_event_device *cd = this_cpu_ptr(&ext_clockevent_device);
    unsigned int cpu = smp_processor_id();

    rtk_cpuExtTCSrc_ack(cpu);
    cd->event_handler(cd);

    return IRQ_HANDLED;
}

int cevt_extTimerIntr_setup(int cpu)
{
    int                 ret;
    unsigned int        tc_irq;
    char                *timer_name;
    unsigned int        timer_id;
    struct irqaction    *ext_irqaction;

    ext_irqaction = kmalloc(sizeof(struct irqaction), GFP_NOWAIT);
    if(ext_irqaction == NULL)
    {
        RTK_BSP_DBG_MSG("\n[%s][%d][CPU %d] ext_irqaction allcoation failed\n",__FUNCTION__,__LINE__,cpu);
        return SDK_BSP_FAILED;
    }
    memset(ext_irqaction, 0, sizeof(struct irqaction));

    timer_name = kmalloc(TIMER_NAME_SIZE, GFP_NOWAIT);
    if(timer_name == NULL)
    {
        RTK_BSP_DBG_MSG("\n[%s][%d][CPU %d] Timer Name buffer allcoation failed\n",__FUNCTION__,__LINE__,cpu);
        return SDK_BSP_FAILED;
    }
    memset(timer_name, 0, TIMER_NAME_SIZE);

    ret = rtk_cpuExtTCID_get(cpu, &timer_id);
    if(ret != SDK_BSP_OK)
    {
        RTK_BSP_DBG_MSG("\n[%s][%d][CPU %d] Get Extern TC ID failed\n",__FUNCTION__,__LINE__,cpu);
        return SDK_BSP_FAILED;
    }

    ret = sprintf(timer_name,"EXT_TIMER_%d",timer_id);
    if(ret == 0)
    {
        RTK_BSP_DBG_MSG("\n[%s][%d][CPU %d] Timer Name buffer NOT fill\n",__FUNCTION__,__LINE__,cpu);
        return SDK_BSP_FAILED;
    }

    ext_irqaction->handler  = cevt_extTimer_ack;
    ext_irqaction->flags    = IRQF_TIMER;
    ext_irqaction->name     = timer_name;

    ret = rtk_cpuExtTCSrcIRQ_get(cpu, &tc_irq);
    if(ret != SDK_BSP_OK)
    {
        RTK_BSP_DBG_MSG("\n[%s][%d][CPU %d] Get Extern TC IRQ failed\n",__FUNCTION__,__LINE__,cpu);
        return SDK_BSP_FAILED;
    }

    ret = setup_irq(tc_irq, ext_irqaction);
    if(ret != 0)
    {
        RTK_BSP_DBG_MSG("\n[%s]Setup IRQ(%d)!!!\n",__FUNCTION__,tc_irq);
    }

    return 0;
}

int cevt_clockevent_init(struct clock_event_device *cd)
{
    unsigned int        cpu = smp_processor_id();
    int                 ret = 0;
    char                *clockevent_name;
    unsigned int        tc_irq;

    clockevent_name = kmalloc(TIMER_NAME_SIZE, GFP_NOWAIT);
    if(clockevent_name == NULL)
    {
        RTK_BSP_DBG_MSG("\n[%s][%d][CPU %d] Timer Name buffer allocation failed\n",__FUNCTION__,__LINE__,cpu);
        return SDK_BSP_FAILED;
    }
    memset(clockevent_name, 0, TIMER_NAME_SIZE);
    ret = sprintf(clockevent_name,"RTK_TIMER_%d",cpu);
    if(ret == 0)
    {
        RTK_BSP_DBG_MSG("\n[%s][%d][CPU %d] Timer Name buffer NOT fill\n",__FUNCTION__,__LINE__,cpu);
        return SDK_BSP_FAILED;
    }

    ret = rtk_cpuExtTCSrcIRQ_get(cpu, &tc_irq);
    if(ret != SDK_BSP_OK)
    {
        RTK_BSP_DBG_MSG("\n[%s][%d][CPU %d] Get Extern TC IRQ failed\n",__FUNCTION__,__LINE__,cpu);
        return SDK_BSP_FAILED;

    }

    cd->name            = clockevent_name;
    cd->features        = CLOCK_EVT_FEAT_PERIODIC;
    cd->event_handler    = ext_timer_event_handler;
    cd->set_next_event    = ext_timer_set_next_event;
    cd->rating          = 100;
    cd->irq             = tc_irq;

    cd->cpumask = cpumask_of(cpu);

    clockevents_register_device(cd);

    ret = cevt_extTimerIntr_setup(cpu);

    return ret;
}

static void ext_clockevent_exit(struct clock_event_device *cd)
{
    disable_irq(cd->irq);
}

static void ext_clockevent_init_notify(void)
{
    int ret;
    /* init per cpu clockevent device*/
    ret = cevt_clockevent_init(this_cpu_ptr(&ext_clockevent_device));
    if (ret)
        RTK_BSP_DBG_MSG("%s: init clock event device for cpu %d failed\n", __func__, smp_processor_id());
}

static void ext_clockevent_exit_notify(void)
{
    ext_clockevent_exit(this_cpu_ptr(&ext_clockevent_device));
}

static int ext_cpu_notifier(struct notifier_block *nb, unsigned long action, void *data)
{

    switch (action & ~CPU_TASKS_FROZEN) {
    case CPU_STARTING:
        ext_clockevent_init_notify();
        break;
    case CPU_DYING:
        ext_clockevent_exit_notify();
        break;
    }

    return NOTIFY_OK;
}

static struct notifier_block ext_cpu_nb = {
    .notifier_call = ext_cpu_notifier,
};

static void __init rtk_cevt_timer_init(struct device_node *node)
{
    unsigned int cpu = smp_processor_id();
    int ret;

    RTK_BSP_DBG_MSG("[%s] Init clock event timer\n", __FUNCTION__);

    rtk_cpuExtTCSrc_init();

    cevt_clockevent_init(this_cpu_ptr(&ext_clockevent_device));


    rtk_cpuExtTCSrcIntr_enable(cpu, BSP_SET_ENABLED);

    ret = register_cpu_notifier(&ext_cpu_nb);

    if (ret < 0)
        RTK_BSP_DBG_MSG("[%s] Unable to register CPU notifier\n", __FUNCTION__);
}

CLOCKSOURCE_OF_DECLARE(ext_timer, "rtk,cevt-ext", rtk_cevt_timer_init);

