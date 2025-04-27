/*
 * Realtek Semiconductor Corp.
 *
 * pci.c:
 *     bsp PCI initialization code
 *
 * Copyright (C) 2006-2012 Tony Wu (tonywu@realtek.com)
 */
#include <linux/init.h>
#include <linux/pci.h>
#include <linux/types.h>
#include <linux/delay.h>

#include <asm/cpu.h>
#include <asm/io.h>
#include <asm/pci.h>

#include "bspchip.h"

extern struct pci_ops bsp_pcie_ops;

static struct resource pcie_mem_resource = {
    .name = "PCIe Memory resources",
    .start = 0x19400000,
    .end = 0x19ffffff,
    .flags = IORESOURCE_MEM,
};

static struct resource pcie_io_resource = {
    .name = "PCIe I/O resources",
    .start = 0x19200000,
    .end = 0x193fffff,
    .flags = IORESOURCE_IO,
};

static struct pci_controller bsp_pcie_controller = {
    .pci_ops = &bsp_pcie_ops,
    .mem_resource = &pcie_mem_resource,
    .mem_offset = 0x19400000,
    .io_resource = &pcie_io_resource,
    .io_offset = 0x19200000,
};

static int __init bsp_pcie_init(void)
{
    unsigned int val;

    val = REG32(0xbb004000);
    if (val != 0x3) {
        printk("no device found, skipping PCIe initialization\n");
        return 0;
    }

    /* initialize Sheipa PCIe */
    printk("Initializing Sheipa PCIe controller\n");
    printk("I/O base = %lx, size = %lx\n", BSP_PCIE_IO_BASE, BSP_PCIE_IO_SIZE);
    printk("MEM base = %lx, size = %lx\n", BSP_PCIE_MEM_BASE, BSP_PCIE_MEM_SIZE);

    REG32(BSP_PCIE_RC_CFG + 0x04) = 0x00100007;
    REG32(BSP_PCIE_RC_CFG + 0x78) = 0x105030;

    /* enable IRQ, turn off ICTL mask */
    REG32(BSP_ICTL_MASK) &= ~(1 << (BSP_PS_PCIE_IRQ_INTA - BSP_IRQ_ICTL_BASE));
    REG32(BSP_ICTL_MASK) &= ~(1 << (BSP_PS_PCIE_IRQ_INTB - BSP_IRQ_ICTL_BASE));
    REG32(BSP_ICTL_MASK) &= ~(1 << (BSP_PS_PCIE_IRQ_INTC - BSP_IRQ_ICTL_BASE));
    REG32(BSP_ICTL_MASK) &= ~(1 << (BSP_PS_PCIE_IRQ_INTD - BSP_IRQ_ICTL_BASE));

    bsp_pcie_controller.io_map_base = (unsigned long)ioremap(BSP_PCIE_IO_BASE,
                                 BSP_PCIE_IO_SIZE);
        set_io_port_base(bsp_pcie_controller.io_map_base);

    register_pci_controller(&bsp_pcie_controller);
    return 0;
}
arch_initcall(bsp_pcie_init);
