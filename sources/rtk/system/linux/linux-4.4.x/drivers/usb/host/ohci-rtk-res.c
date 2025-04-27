/*
 * Copyright (C) 2019-2021 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : OHCI HCD Resource (Host Controller Driver) for USB.
 *
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/delay.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/platform_device.h>
#include <common/error.h>

#define BSP_USB_PHY_CTRL		0xB8000500UL
#define BSP_OHCI_BASE			0xB8020000UL
#define BSP_OHCI_SIZE           0x00000200UL
#define BSP_OHCI_IRQ            43 /* Will be replaced by rtk_bspUSB_irq_get()*/

#define PADDR(addr)  ((addr) & 0x1FFFFFFF)

/* USB Host Controller */
#ifdef CONFIG_USB_OHCI_HCD
static struct resource bsp_usb_ohci_resource[] = {
        [0] = {
                .start = PADDR(BSP_OHCI_BASE),
                .end   = PADDR(BSP_OHCI_BASE) + BSP_OHCI_SIZE - 1,
                .flags = IORESOURCE_MEM,
        },
        [1] = {
                .start = BSP_OHCI_IRQ,
                .end   = BSP_OHCI_IRQ,
                .flags = IORESOURCE_IRQ,
        }
};

static u64 bsp_ohci_rtkGen1_dmamask = 0x3fffffffULL;

static struct platform_device bsp_usb_ohci_device = {
        .name          = "rtkGen1-ohci",
        .id            = 0,
        .num_resources = ARRAY_SIZE(bsp_usb_ohci_resource),
        .resource      = bsp_usb_ohci_resource,
        .dev           = {
            .dma_mask = &bsp_ohci_rtkGen1_dmamask,
            .coherent_dma_mask = 0x3fffffffULL,
        }
};
#endif

static struct platform_device *bsp_usb_devs0[] __initdata = {
        #ifdef CONFIG_USB_OHCI_HCD
        &bsp_usb_ohci_device,
        #endif
};

int rtkGen1_ohci_init(void)
{
    int ret = 0;

    ret = platform_add_devices(bsp_usb_devs0, ARRAY_SIZE(bsp_usb_devs0));
    if (ret < 0) {
            printk("ERROR: unable to add OHCI\n");
            return ret;
    }
    return 0;
}

MODULE_LICENSE("GPL");

