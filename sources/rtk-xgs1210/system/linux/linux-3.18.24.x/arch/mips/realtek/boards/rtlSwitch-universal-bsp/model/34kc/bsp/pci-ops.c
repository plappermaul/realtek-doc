/*
 * Realtek Semiconductor Corp.
 *
 * bsp/pci-ops.c:
 * 	bsp PCI operation code
 *
 * Copyright (C) 2006-2012 Tony Wu (tonywu@realtek.com)
 */
#include <linux/types.h>
#include <linux/pci.h>
#include <linux/kernel.h>
#include <linux/delay.h>

#include <asm/pci.h>
#include <asm/io.h>

#include "bspchip.h"

#undef DEBUG_PCI_READ
#define DEBUG_PCI_WRITE

/*
 * PCI Config Read Byte
 */
static int sheipa_read_config(struct pci_bus *bus, unsigned int devfn,
                  int offset, int size, u32 *val)
{
    uint32_t address;
    uint32_t value;
    int dev, busno, func;

    busno = bus->number;
    dev = PCI_SLOT(devfn);
    func = PCI_FUNC(devfn);

    if (busno != 0 || dev != 0)
        return PCIBIOS_DEVICE_NOT_FOUND;

    address = BSP_PCIE_EP_CFG + offset;
    switch (size) {
    case 1:
        value = REG08(address);
        break;
    case 2:
        value = REG16(address);
        break;

    case 4:
    default:
        value = REG32(address);
        break;
    }

#ifdef DEBUG_PCI_READ
    printk("PCI_EP_READ: addr= %x, size=%d, value= %x\n", address, size, value);
#endif

    *val = value;
    return PCIBIOS_SUCCESSFUL;
}

static int sheipa_write_config(struct pci_bus *bus, unsigned int devfn,
                   int offset, int size, u32 val)
{
    uint32_t address;
    int dev, busno;

    if (offset == 0x79) {
        printk("INFO: sheipa bypass writing offset=0x79, size=%d, value=%u\n",
               size, val);
        return PCIBIOS_SUCCESSFUL;
    }

    busno = bus->number;
    dev = PCI_SLOT (devfn);

    if (busno != 0 || dev != 0)
        return PCIBIOS_DEVICE_NOT_FOUND;

    address = BSP_PCIE_EP_CFG + offset;
    switch (size) {
    case 1:
        REG08 (address) = val;
        break;
    case 2:
        REG16 (address) = val;
        break;
    case 4:
    default:
        REG32 (address) = val;
        break;
    }

#ifdef DEBUG_PCI_WRITE
    printk("PCI_EP_WRITE: addr= %x, size=%d, value= %x\n", address, size, val);
#endif
    return PCIBIOS_SUCCESSFUL;
}

/*
 * Sheipa PCIe pci_ops structure
 */
struct pci_ops bsp_pcie_ops = {
    .read = sheipa_read_config,
    .write = sheipa_write_config,
};
