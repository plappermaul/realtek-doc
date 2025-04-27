/*
 * Realtek Semiconductor Corp.
 *
 * rtk_bsp_rtl9300_intrCtrk.c
 *   DesignWare ICTL initialization and handlers
 *
 * This file is to part of BSP interrupt handlers
 *
 * Copyright (C) 2020 Realtek
 */

#include <common/rt_type.h>
#include <linux/irq.h>
#include <linux/irqchip.h>
#include <linux/irqdomain.h>
#include <linux/of_irq.h>
#include <linux/of_address.h>

#include "rtk_bsp_driver.h"
#include "rtk_bsp_rtl9300.h"
#include "rtk_bsp_rtl9300_intr.h"


rtl9300_intr_mapping_conf_t rtl9300_intr_mapping[] = {
    [0]                         = { .cpu_ip_id = BSP_UNUSED_FIELD,          .mapped_system_irq = BSP_UNUSED_FIELD },
    [1]                         = { .cpu_ip_id = BSP_UNUSED_FIELD,          .mapped_system_irq = BSP_UNUSED_FIELD },
    [RTL9300_TC2DEL_INTRSRC]    = { .cpu_ip_id = RTL9300_TC2DEL_CPU_IP,     .mapped_system_irq = RTL9300_TC2DEL_IRQ },
    [RTL9300_TC3DEL_INTRSRC]    = { .cpu_ip_id = RTL9300_TC3DEL_CPU_IP,     .mapped_system_irq = RTL9300_TC3DEL_IRQ },
    [RTL9300_TC4DEL_INTRSRC]    = { .cpu_ip_id = RTL9300_TC4DEL_CPU_IP,     .mapped_system_irq = RTL9300_TC4DEL_IRQ },
    [RTL9300_WDT0_INTRSRC]      = { .cpu_ip_id = RTL9300_WDT_IP1_CPU_IP,    .mapped_system_irq = RTL9300_WDT0_IRQ },
    [6]                         = { .cpu_ip_id = BSP_UNUSED_FIELD,          .mapped_system_irq = BSP_UNUSED_FIELD },
    [RTL9300_TC0_INTRSRC]       = { .cpu_ip_id = RTL9300_TC0_CPU_IP,        .mapped_system_irq = RTL9300_TC0_IRQ },
    [RTL9300_TC1_INTRSRC]       = { .cpu_ip_id = RTL9300_TC1_CPU_IP,        .mapped_system_irq = RTL9300_TC1_IRQ },
    [RTL9300_TC2_INTRSRC]       = { .cpu_ip_id = RTL9300_TC2_CPU_IP,        .mapped_system_irq = RTL9300_TC2_IRQ },
    [RTL9300_TC3_INTRSRC]       = { .cpu_ip_id = RTL9300_TC3_CPU_IP,        .mapped_system_irq = RTL9300_TC3_IRQ },
    [RTL9300_TC4_INTRSRC]       = { .cpu_ip_id = RTL9300_TC4_CPU_IP,        .mapped_system_irq = RTL9300_TC4_IRQ },
    [RTL9300_BTG_INTRSRC]       = { .cpu_ip_id = RTL9300_BTG_CPU_IP,        .mapped_system_irq = RTL9300_BTG_IRQ },
    [RTL9300_GPIO_INTRSRC]      = { .cpu_ip_id = RTL9300_GPIO_ABCD_CPU_IP,  .mapped_system_irq = RTL9300_GPIO_IRQ },
    [14]                        = { .cpu_ip_id = BSP_UNUSED_FIELD,          .mapped_system_irq = BSP_UNUSED_FIELD },
    [RTL9300_LXSTO_INTRSRC]     = { .cpu_ip_id = RTL9300_LXSTO_CPU_IP,      .mapped_system_irq = RTL9300_LXSTO_IRQ },
    [RTL9300_LXMTO_INTRSRC]     = { .cpu_ip_id = RTL9300_LXMTO_CPU_IP,      .mapped_system_irq = RTL9300_LXMTO_IRQ },
    [RTL9300_OCPTO_INTRSRC]     = { .cpu_ip_id = RTL9300_OCPTO_CPU_IP,      .mapped_system_irq = RTL9300_OCPTO_IRQ },
    [18]                        = { .cpu_ip_id = BSP_UNUSED_FIELD,          .mapped_system_irq = BSP_UNUSED_FIELD },
    [RTL9300_SPINAND_INTRSRC]   = { .cpu_ip_id = RTL9300_SPINAND_CPU_IP,    .mapped_system_irq = RTL9300_SPINAND_IRQ },
    [RTL9300_ECC_INTRSRC]       = { .cpu_ip_id = RTL9300_ECC_CPU_IP,        .mapped_system_irq = RTL9300_ECC_IRQ },
    [21]                        = { .cpu_ip_id = BSP_UNUSED_FIELD,          .mapped_system_irq = BSP_UNUSED_FIELD },
    [RTL9300_SEC_INTRSRC]       = { .cpu_ip_id = RTL9300_SEC_CPU_IP,        .mapped_system_irq = RTL9300_SEC_IRQ },
    [RTL9300_SWCORE_INTRSRC]    = { .cpu_ip_id = RTL9300_SWCORE_CPU_IP,     .mapped_system_irq = RTL9300_SWCORE_IRQ },
    [RTL9300_NIC_INTRSRC]       = { .cpu_ip_id = RTL9300_NIC_CPU_IP,        .mapped_system_irq = RTL9300_NIC_IRQ },
    [25]                        = { .cpu_ip_id = BSP_UNUSED_FIELD,          .mapped_system_irq = BSP_UNUSED_FIELD },
    [RTL9300_PCIE0_INTRSRC]     = { .cpu_ip_id = RTL9300_PCIE0_CPU_IP,      .mapped_system_irq = RTL9300_PCIE0_IRQ },
    [27]                        = { .cpu_ip_id = BSP_UNUSED_FIELD,          .mapped_system_irq = BSP_UNUSED_FIELD },
    [RTL9300_USBH2_INTRSRC]     = { .cpu_ip_id = RTL9300_USB_H2_CPU_IP,     .mapped_system_irq = RTL9300_USBH2_IRQ },
    [29]                        = { .cpu_ip_id = BSP_UNUSED_FIELD,          .mapped_system_irq = BSP_UNUSED_FIELD },
    [RTL9300_UART0_INTRSRC]     = { .cpu_ip_id = RTL9300_UART0_CPU_IP,      .mapped_system_irq = RTL9300_UART0_IRQ },
    [RTL9300_UART1_INTRSRC]     = { .cpu_ip_id = RTL9300_UART1_CPU_IP,      .mapped_system_irq = RTL9300_UART1_IRQ },
};


static void ictl_irq_mask(struct irq_data *d)
{
    RTK_BSP_REG32(RTL9300_GIMR) &= ~(1 << (d->hwirq));
}

static void ictl_irq_unmask(struct irq_data *d)
{
    RTK_BSP_REG32(RTL9300_GIMR) |= (1 << (d->hwirq));
}

static struct irq_chip rtl9300_ictl_irq_controller = {
	.name = "Realtek ICTL",
	.irq_mask_ack = ictl_irq_mask,
	.irq_mask = ictl_irq_mask,
	.irq_unmask = ictl_irq_unmask,
};

static int rtl9300_ictl_intc_map(struct irq_domain *d, unsigned int irq, irq_hw_number_t hw)
{
	irq_set_chip_and_handler(irq, &rtl9300_ictl_irq_controller, handle_level_irq);

	return 0;
}

static const struct irq_domain_ops irq_domain_ops = {
	.xlate = irq_domain_xlate_onecell,
	.map = rtl9300_ictl_intc_map,
};

static void rtl9300_irq_handler_2(struct irq_desc *desc)
{
    unsigned int intr_src = 0;
	unsigned int pending = RTK_BSP_REG32(RTL9300_GISR);

    for(intr_src = 0; intr_src < RTL9300_ICTL_NUM; intr_src++)
    {
        if((pending & (0x1 << intr_src)) && (rtl9300_intr_mapping[intr_src].cpu_ip_id == RTL9300_CPUIP_ID_2))
            generic_handle_irq(rtl9300_intr_mapping[intr_src].mapped_system_irq);
        else
            spurious_interrupt();
    }
}

static void rtl9300_irq_handler_3(struct irq_desc *desc)
{
    unsigned int intr_src = 0;
	unsigned int pending = RTK_BSP_REG32(RTL9300_GISR);

    for(intr_src = 0; intr_src < RTL9300_ICTL_NUM; intr_src++)
    {
        if((pending & (0x1 << intr_src)) && (rtl9300_intr_mapping[intr_src].cpu_ip_id == RTL9300_CPUIP_ID_3))
            generic_handle_irq(rtl9300_intr_mapping[intr_src].mapped_system_irq);
        else
            spurious_interrupt();
    }

}

static void rtl9300_irq_handler_4(struct irq_desc *desc)
{
    unsigned int intr_src = 0;
	unsigned int pending = RTK_BSP_REG32(RTL9300_GISR);

    for(intr_src = 0; intr_src < RTL9300_ICTL_NUM; intr_src++)
    {
        if((pending & (0x1 << intr_src)) && (rtl9300_intr_mapping[intr_src].cpu_ip_id == RTL9300_CPUIP_ID_4))
            generic_handle_irq(rtl9300_intr_mapping[intr_src].mapped_system_irq);
        else
            spurious_interrupt();
    }

}

static void rtl9300_irq_handler_5(struct irq_desc *desc)
{
    unsigned int intr_src = 0;
	unsigned int pending = RTK_BSP_REG32(RTL9300_GISR);

    for(intr_src = 0; intr_src < RTL9300_ICTL_NUM; intr_src++)
    {
        if((pending & (0x1 << intr_src)) && (rtl9300_intr_mapping[intr_src].cpu_ip_id == RTL9300_CPUIP_ID_5))
            generic_handle_irq(rtl9300_intr_mapping[intr_src].mapped_system_irq);
        else
            spurious_interrupt();
    }

}

static void rtl9300_irq_handler_6(struct irq_desc *desc)
{
    unsigned int intr_src = 0;
	unsigned int pending = RTK_BSP_REG32(RTL9300_GISR);

    for(intr_src = 0; intr_src < RTL9300_ICTL_NUM; intr_src++)
    {
        if((pending & (0x1 << intr_src)) && (rtl9300_intr_mapping[intr_src].cpu_ip_id == RTL9300_CPUIP_ID_6))
            generic_handle_irq(rtl9300_intr_mapping[intr_src].mapped_system_irq);
        else
            spurious_interrupt();
    }

}

static void rtl9300_irq_handler_7(struct irq_desc *desc)
{
    unsigned int intr_src = 0;
	unsigned int pending = RTK_BSP_REG32(RTL9300_GISR);

    for(intr_src = 0; intr_src < RTL9300_ICTL_NUM; intr_src++)
    {
        if((pending & (0x1 << intr_src)) && (rtl9300_intr_mapping[intr_src].cpu_ip_id == RTL9300_CPUIP_ID_7))
            generic_handle_irq(rtl9300_intr_mapping[intr_src].mapped_system_irq);
        else
            spurious_interrupt();
    }

}

static void __init _rtl9300_ictl_init(struct device_node *node,
				  unsigned int cpu_vec)
{
	struct irq_domain *domain;
	int irq;
    rtl9300_cpuIP_id_t cpu_intr_idx;

	irq = cpu_vec;

    RTK_BSP_REG32(RTL9300_IRR0) = 0x0;
    RTK_BSP_REG32(RTL9300_IRR1) = 0x0;
    RTK_BSP_REG32(RTL9300_IRR2) = 0x0;
    RTK_BSP_REG32(RTL9300_IRR3) = 0x0;

    RTK_BSP_REG32(RTL9300_IRR0) = RTL9300_IRR0_SETTING;  /* UART 0 */
    RTK_BSP_REG32(RTL9300_IRR1) = RTL9300_IRR1_SETTING;
    RTK_BSP_REG32(RTL9300_IRR2) = RTL9300_IRR2_SETTING;
    RTK_BSP_REG32(RTL9300_IRR3) = RTL9300_IRR3_SETTING;

	domain = irq_domain_add_legacy(node, RTL9300_ICTL_NUM, RTL9300_ICTL_BASE, 0,
				       &irq_domain_ops, NULL);

	if (!domain)
		printk("Failed to add irqdomain");


    for(cpu_intr_idx = RTL9300_CPUIP_ID_2; cpu_intr_idx < RTL9300_CPUIP_ID_MAX; cpu_intr_idx++)
    {
        switch(cpu_intr_idx)
        {
            case RTL9300_CPUIP_ID_2:
                irq_set_chained_handler_and_data(cpu_intr_idx, rtl9300_irq_handler_2, domain);
                break;
            case RTL9300_CPUIP_ID_3:
                irq_set_chained_handler_and_data(cpu_intr_idx, rtl9300_irq_handler_3, domain);
                break;
            case RTL9300_CPUIP_ID_4:
                irq_set_chained_handler_and_data(cpu_intr_idx, rtl9300_irq_handler_4, domain);
                break;
            case RTL9300_CPUIP_ID_5:
                irq_set_chained_handler_and_data(cpu_intr_idx, rtl9300_irq_handler_5, domain);
                break;
            case RTL9300_CPUIP_ID_6:
                irq_set_chained_handler_and_data(cpu_intr_idx, rtl9300_irq_handler_6, domain);
                break;
            case RTL9300_CPUIP_ID_7:
                irq_set_chained_handler_and_data(cpu_intr_idx, rtl9300_irq_handler_7, domain);
                break;
            default:
                RTK_BSP_DBG_MSG("\nCPU IP index error.\n");
                break;
        }
	}

}


static int __init rtl9300_intr_of_init(struct device_node *node,
				  struct device_node *parent)
{
	unsigned int cpu_vec;

	cpu_vec = irq_of_parse_and_map(node, 0);

	_rtl9300_ictl_init(node, cpu_vec);
	return 0;
}

IRQCHIP_DECLARE(rtk_switch, "rtk,9300-intr", rtl9300_intr_of_init);

