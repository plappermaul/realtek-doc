/*
 *  Bus Glue for Realtek built-in EHCI controller.
 *
 *  Copyright (C) 2008-2010 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 *  Parts of this file are based on Realtek' 2.6.30 BSP
 *	Copyright (C) 2013 Realtek Semiconductor, Corp.
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <common/rt_type.h>
#include <linux/platform_device.h>
#include <linux/delay.h>

extern void rtk_bspUSB_irq_get(unsigned int *irq_num);
extern int usb_disabled(void);
/**
 * ehci_rtk_gen1_probe - initialize FSL-based HCDs
 * @drvier: Driver to be used for this HCD
 * @pdev: USB Host Controller being probed
 * Context: !in_interrupt()
 *
 * Allocates basic resources for this USB host controller.
 *
 */
static int ehci_rtk_gen1_probe(const struct hc_driver *driver,
                 struct usb_hcd **hcd_out,
                 struct platform_device *pdev)
{
    struct usb_hcd *hcd;
    struct resource *res;
    int irq;
    int ret;

    pr_debug("initializing RTL-SOC USB Controller\n");
    res = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
    if (!res) {
        dev_dbg(&pdev->dev, "no IRQ specified for %s\n",
            dev_name(&pdev->dev));
        return -ENODEV;
    }

    rtk_bspUSB_irq_get(&irq);

    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    if (!res) {
        dev_dbg(&pdev->dev, "no base address specified for %s\n",
            dev_name(&pdev->dev));
        return -ENODEV;
    }

    hcd = usb_create_hcd(driver, &pdev->dev, dev_name(&pdev->dev));
    if (!hcd)
        return -ENOMEM;

    hcd->rsrc_start	= res->start;
    hcd->rsrc_len	= res->end - res->start + 1;

    if (!request_mem_region(hcd->rsrc_start, hcd->rsrc_len, hcd_name)) {
        dev_dbg(&pdev->dev, "controller already in use\n");
        ret = -EBUSY;
        goto err_put_hcd;
    }

    hcd->regs = ioremap(hcd->rsrc_start, hcd->rsrc_len);
    if (!hcd->regs) {
        dev_dbg(&pdev->dev, "error mapping memory\n");
        ret = -EFAULT;
        goto err_release_region;
    }

    ret = usb_add_hcd(hcd, irq, IRQF_SHARED);

    if (ret)
        goto err_iounmap;

    return 0;

 err_iounmap:
    iounmap(hcd->regs);

 err_release_region:
    release_mem_region(hcd->rsrc_start, hcd->rsrc_len);
 err_put_hcd:
    usb_put_hcd(hcd);
    return ret;
}

/**
 * rtk_gen1_usb_hcd_remove - shutdown processing for FSL-based HCDs
 * @dev: USB Host Controller being removed
 * Context: !in_interrupt()
 *
 *
 */
static void rtk_gen1_usb_hcd_remove(struct usb_hcd *hcd,
                   struct platform_device *pdev)
{
    usb_remove_hcd(hcd);
    iounmap(hcd->regs);
    release_mem_region(hcd->rsrc_start, hcd->rsrc_len);
    usb_put_hcd(hcd);
}

static int rtk_gen1_ehci_setup(struct usb_hcd *hcd)
{
    struct ehci_hcd *ehci = hcd_to_ehci(hcd);
    int ret;

    ehci->big_endian_desc = 1;
    ehci->big_endian_mmio = 1;
    ehci->caps = hcd->regs;
    ehci->regs = hcd->regs +
            HC_LENGTH(ehci,ehci_readl(ehci, &ehci->caps->hc_capbase));
    ehci->hcs_params = ehci_readl(ehci, &ehci->caps->hcs_params);

    ehci->sbrn = 0x20;
    //ehci->has_synopsys_hc_bug = 1;

    ehci_reset(ehci);

    ret = ehci_init(hcd);
    if (ret)
        return ret;

    //ehci_port_power(ehci, 0);

    return 0;
}

static const struct hc_driver rtk_gen1_ehci_hc_driver = {
    .description		= hcd_name,
    .product_desc		= "Realtek On-Chip EHCI Host Controller",
    .hcd_priv_size		= sizeof(struct ehci_hcd),

    .irq				= ehci_irq,
    .flags				= HCD_MEMORY | HCD_USB2 | HCD_BH,

    .reset				= rtk_gen1_ehci_setup,
    .start				= ehci_run,
    .stop				= ehci_stop,
    .shutdown			= ehci_shutdown,

    .urb_enqueue		= ehci_urb_enqueue,
    .urb_dequeue		= ehci_urb_dequeue,
    .endpoint_disable	= ehci_endpoint_disable,
    .endpoint_reset		= ehci_endpoint_reset,

    .get_frame_number	= ehci_get_frame,

    .hub_status_data	= ehci_hub_status_data,
    .hub_control		= ehci_hub_control,
#ifdef CONFIG_PM
    .bus_suspend        = ehci_bus_suspend,
    .bus_resume         = ehci_bus_resume,
#endif
    .relinquish_port	= ehci_relinquish_port,
    .port_handed_over	= ehci_port_handed_over,

    .clear_tt_buffer_complete = ehci_clear_tt_buffer_complete,
};

static int rtk_gen1_ehci_driver_probe(struct platform_device *pdev)
{
    struct usb_hcd *hcd = NULL;
    int ret;

    if (usb_disabled())
        return -ENODEV;

    ret = ehci_rtk_gen1_probe(&rtk_gen1_ehci_hc_driver, &hcd, pdev);

    return ret;
}

static int rtk_gen1_ehci_driver_remove(struct platform_device *pdev)
{
    struct usb_hcd *hcd = platform_get_drvdata(pdev);

    rtk_gen1_usb_hcd_remove(hcd, pdev);
    return 0;
}

MODULE_ALIAS("platform:rtk_gen1-ehci");

static struct platform_driver ehci_rtk_gen1_driver = {
    .probe		= rtk_gen1_ehci_driver_probe,
    .remove		= rtk_gen1_ehci_driver_remove,
    .driver = {
        .name	= "rtk_gen1-ehci",
    }
};

