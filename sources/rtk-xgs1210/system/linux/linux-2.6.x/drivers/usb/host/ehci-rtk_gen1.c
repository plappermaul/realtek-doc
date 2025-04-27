#include <linux/platform_device.h>


/* called during probe() after chip reset completes */
static int ehci_setup(struct usb_hcd *hcd)
{
    struct ehci_hcd		*ehci = hcd_to_ehci(hcd);
    int			retval = 0;

    ehci->caps = hcd->regs;
    ehci->regs = hcd->regs + HC_LENGTH(readl(&ehci->caps->hc_capbase));
    dbg_hcs_params(ehci, "reset");
    dbg_hcc_params(ehci, "reset");

    /* cache this readonly data; minimize chip reads */
    ehci->hcs_params = readl(&ehci->caps->hcs_params);
    retval = ehci_halt(ehci);
    if (retval)
        return retval;

    /* data structure init */
    retval = ehci_init(hcd);
    if (retval)
        return retval;
    if (ehci_is_TDI(ehci))
        ehci_reset(ehci);

#ifdef	CONFIG_USB_SUSPEND
    /* REVISIT: the controller works fine for wakeup iff the root hub
     * itself is "globally" suspended, but usbcore currently doesn't
     * understand such things.
     *
     * System suspend currently expects to be able to suspend the entire
     * device tree, device-at-a-time.  If we failed selective suspend
     * reports, system suspend would fail; so the root hub code must claim
     * success.  That's lying to usbcore, and it matters for for runtime
     * PM scenarios with selective suspend and remote wakeup...
     */
    if (ehci->no_selective_suspend && device_can_wakeup(&pdev->dev))
        ehci_warn(ehci, "selective suspend/wakeup unavailable\n");
#endif

    return retval;
}

static int ehci_usb_hcd_probe(struct hc_driver *driver, struct platform_device *pdev)
{
    struct usb_hcd *hcd;
    struct resource *res;
    struct ehci_hcd *ehci;
    int retval = 0;
    unsigned int usb_irq;	//cathy for 8672

//	set_usbphy();

    res = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
    if (!res) {
        printk("%s: get irq resource failed!\n", __FUNCTION__);
        retval = -ENODEV;
        goto out1;
    }
    usb_irq = res->start;

//	hcd = usb_create_hcd(driver, &pdev->dev, (char *)dev_name(&pdev->dev));
    hcd = usb_create_hcd(driver, &pdev->dev, "rtk_gen1-ehci-hcd");

    if (!hcd) {
        printk("%s: create hcd failed!\n", __FUNCTION__);
        retval = -ENOMEM;
        goto out1;
    }

    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    if (!res) {
        printk("%s: get memory resource failed!\n", __FUNCTION__);
        retval = -ENODEV;
        goto out2;
    }
    #ifdef CONFIG_PCI
    hcd->regs = (void *) (res->start|0xb0000000);
    hcd->rsrc_start = (res->start|0xb0000000);
    #else
    hcd->regs = (void *) res->start;
        hcd->rsrc_start = res->start;
    #endif
    hcd->rsrc_len = res->end - res->start;

    ehci = hcd_to_ehci (hcd);
    ehci->caps = hcd->regs;
    ehci->regs = hcd->regs + HC_LENGTH (readl (&ehci->caps->hc_capbase));
    ehci->hcs_params = readl (&ehci->caps->hcs_params);

    retval = usb_add_hcd (hcd, usb_irq, IRQF_SHARED);
    if (retval != 0)
        goto out2;

    return retval;

out2:
    usb_put_hcd(hcd);
out1:
    return retval;
}

void ehci_restart(struct usb_hcd *hcd)
{
    struct ehci_hcd *ehci = hcd_to_ehci(hcd);

    /* halt, reset EHCI, and cleanup EHCI memory */
    ehci_port_power(ehci, 0);
    ehci_halt(ehci);
    del_timer_sync(&ehci->watchdog);
    spin_lock_irq(&ehci->lock);
    if(HC_IS_RUNNING(hcd->state))
        ehci_quiesce(ehci);
    ehci_reset(ehci);
    writel(0, &ehci->regs->intr_enable);
    remove_debug_files(ehci);
    if (ehci->async)
        ehci_work(ehci);
    spin_unlock_irq(&ehci->lock);
    ehci_mem_cleanup(ehci);

    /* allocate EHCI memory and start EHCI */
    ehci_setup(hcd);
    ehci_run(hcd);
    ehci_port_power (ehci, 1);

    return;
}
EXPORT_SYMBOL(ehci_restart);

struct hc_driver rtk_gen1_ehci_driver = {
    .description =		hcd_name,
    .product_desc =		"EHCI Host Controller",
    .hcd_priv_size =	sizeof(struct ehci_hcd),
    /*
     * generic hardware linkage
     */
    .irq =			ehci_irq,
    .flags =		HCD_MEMORY | HCD_USB2,

    /*
     * basic lifecycle operations
     */
    .reset =		ehci_setup,
    .start =		ehci_run,
    .stop =			ehci_stop,
    .shutdown =		ehci_shutdown,

    /*
     * managing i/o requests and associated device resources
     */
    .urb_enqueue =		ehci_urb_enqueue,
    .urb_dequeue =		ehci_urb_dequeue,
    .endpoint_disable =	ehci_endpoint_disable,

    /*
     * scheduling support
     */
    .get_frame_number =	ehci_get_frame,

    /*
     * root hub support
     */
    .hub_status_data =	ehci_hub_status_data,
    .hub_control =		ehci_hub_control,
    .bus_suspend =		ehci_bus_suspend,
    .bus_resume =		ehci_bus_resume,
#ifdef CONFIG_USB_PATCH_RTL9300
    .rtl9300_dma_process = rtl9300_hcd_dma_process,
#endif	//CONFIG_USB_PATCH_RTL9300
};

static int rtk_gen1_ehci_drv_probe (struct platform_device *pdev)
{
    return ehci_usb_hcd_probe (&rtk_gen1_ehci_driver, pdev);
}

static int rtk_gen1_ehci_drv_remove (struct platform_device *pdev)
{
    struct usb_hcd *hcd;
    int	retval = 0;

    hcd = platform_get_drvdata (pdev);

    if (!hcd) {
        printk("%s: get hcd failed!\n", __FUNCTION__);
        retval = -ENODEV;
        goto out;
    }

    usb_remove_hcd (hcd);

    if (hcd->driver->flags & HCD_MEMORY) {
        iounmap (hcd->regs);
        release_mem_region (hcd->rsrc_start, hcd->rsrc_len);
    }
    else {
        release_region (hcd->rsrc_start, hcd->rsrc_len);
    }
    usb_put_hcd (hcd);

out:
    return retval;
}

struct platform_driver rtk_gen1_platform_driver = {
    .probe = rtk_gen1_ehci_drv_probe,
    .remove = rtk_gen1_ehci_drv_remove,
    .shutdown = usb_hcd_platform_shutdown,
    .driver = {
        .name = "rtk_gen1-ehci",
    },
};

