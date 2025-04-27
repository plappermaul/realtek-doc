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
 *
 * Purpose : Realtek Switch PCI init
 *
 * Feature : Realtek Switch PCI init
 *
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/netdevice.h>
#include <linux/cdev.h>
#include <linux/interrupt.h>


#include <common/type.h>
#include <soc/type.h>
#include <common/util/rt_util_time.h>
#include <rtcore/rtcore.h>
#include <rtcore/rtcore_pci.h>
#include <private/drv/nic/nic_common.h>
#include <drv/nic/nic.h>
#include <osal/lib.h>
#include <osal/print.h>
#include <osal/memory.h>

#include <common/util/rt_util_intr.h>
#include <common/util/rt_util_intrk.h>


#define R8168_REGS_SIZE                 (256)
#define MODULENAME_FUN_0                "r8111H"
#define MODULENAME_FUN_1                "r9310"
#define R9310_REGS_SIZE                 0x10000



#define R9310_R32(reg)                  ((unsigned int) readl (r9310_ioaddr + (reg)))
#define R9310_W32(reg, val32) writel    ((val32), r9310_ioaddr + (reg))



rtcore_pci_dev_t    rtcore_pci_dev[RT_PCIDEV_END];

extern atomic_t nic_wait_for_intr;
extern wait_queue_head_t nic_intr_wait_queue;


static struct pci_device_id rtl8168_pci_tbl[] = {
    { PCI_DEVICE(PCI_VENDOR_ID_REALTEK, 0x8311), },
    { PCI_VENDOR_ID_DLINK, 0x4300, 0x1186, 0x4b10,},
    {0,},
};

static struct pci_device_id r9310_pci_tbl[] = {
    { PCI_VENDOR_ID_REALTEK,    0x9310, PCI_ANY_ID, PCI_ANY_ID },
    {0,},
};

struct rtl8168_private      *tp;
void __iomem                *ioaddr = NULL;
void __iomem                *r9310_ioaddr = NULL;
static int                  use_dac = 1;
static int32                rtnic_complete = 0;
static int32                switch_complete = 0;

static irqreturn_t r8111h_interrupt(int irq, void *dev_instance);



static int
rtnic_init_pci(struct pci_dev *pdev)
{
    int rc = -ENOMEM;

    /* enable device (incl. PCI PM wakeup and hotplug setup) */
    rc = pci_enable_device(pdev);
    if (rc < 0) {
        osal_printf("%s():%d  8111H pci enable failed.\n", __FUNCTION__, __LINE__);
        goto out;
    }

    rc = pci_set_mwi(pdev);
    if (rc < 0)
        goto err_out_disable;


    /* make sure PCI base addr 1 is MMIO */
    if (!(pci_resource_flags(pdev, 2) & IORESOURCE_MEM)) {
        osal_printf("%s():%d  region #1 not an MMIO resource, aborting\n", __FUNCTION__, __LINE__);
        rc = -ENODEV;
        goto err_out_mwi;
    }
    /* check for weird/broken PCI region reporting */
    if (pci_resource_len(pdev, 2) < R8168_REGS_SIZE) {
        osal_printf("%s():%d  Invalid PCI region size(s), aborting\n", __FUNCTION__, __LINE__);
        rc = -ENODEV;
        goto err_out_mwi;
    }

    rc = pci_request_regions(pdev, MODULENAME_FUN_0);
    if (rc < 0) {
        osal_printf("%s():%d  could not request regions.\n", __FUNCTION__, __LINE__);
        goto err_out_mwi;
    }

    if ((sizeof(dma_addr_t) > 4) &&
        !pci_set_dma_mask(pdev, DMA_BIT_MASK(64))&& use_dac) {
            //dev->features |= NETIF_F_HIGHDMA;
    } else {
            rc = pci_set_dma_mask(pdev, DMA_BIT_MASK(32));
            if (rc < 0) {
                osal_printf("%s():%d  DMA configuration failed.\n", __FUNCTION__, __LINE__);
                goto err_out_free_res;
            }
    }

    pci_set_master(pdev);

    /* ioremap MMIO region */
    rtcore_pci_dev[RT_PCIDEV_NIC].physical_base_addr = pci_resource_start(pdev, 2);
    ioaddr = ioremap(pci_resource_start(pdev, 2), R8168_REGS_SIZE);
    rtcore_pci_dev[RT_PCIDEV_NIC].ioaddr = (uintptr)ioaddr;
    rtcore_pci_dev[RT_PCIDEV_NIC].mem_size = R8168_REGS_SIZE;
    if (ioaddr == NULL) {
        osal_printf("%s():%d  cannot remap MMIO, aborting\n", __FUNCTION__, __LINE__);
        rc = -EIO;
        goto err_out_free_res;
    }

    rtcore_pci_dev[RT_PCIDEV_NIC].pdev = pdev;
    pci_set_drvdata(pdev, &rtcore_pci_dev[RT_PCIDEV_NIC]);

    //remove this remark to print message
    //osal_printf("%s():%d pdev->irq:%d\n", __FUNCTION__, __LINE__, pdev->irq);
    rc = pci_enable_msi(pdev);
    if (rc)
    {
        osal_printf("%s():%d  rc:%d\n", __FUNCTION__, __LINE__, rc);
    }
    rc = request_irq(pdev->irq, r8111h_interrupt, 0, "NIC", pdev);
    if (rc)
    {
        osal_printf("%s():%d  rc:%d  pdev->irq:%d\n", __FUNCTION__, __LINE__, rc, pdev->irq);
        goto err_out_free_res;
    }


  out:
    return rc;

  err_out_free_res:
    pci_release_regions(pdev);

  err_out_mwi:
    pci_clear_mwi(pdev);

  err_out_disable:
    pci_disable_device(pdev);

    goto out;
}

static int
rtnic_init(struct pci_dev *pdev, const struct pci_device_id *ent)
{
    int32           ret;
    uint8           *vir_base;
    dma_addr_t      phy_base;
    struct page     *page;

    if ((ret = rtnic_init_pci(pdev)) != RT_ERR_OK)
    {
        osal_printf("%s():%d  init board failed.\n", __FUNCTION__, __LINE__);
        return ret;
    }

    vir_base = pci_alloc_consistent(pdev, CONSIST_MEM_RSVD_SIZE, &phy_base);
    if (vir_base == NULL)
    {
        osal_printf("%s():%d  init board failed.\n", __FUNCTION__, __LINE__);
        return RT_ERR_FAILED;
    }
    osal_memset(vir_base, 0, CONSIST_MEM_RSVD_SIZE);

    rtcore_pci_dev[RT_PCIDEV_NIC_DMA].pdev                  = pdev;
    rtcore_pci_dev[RT_PCIDEV_NIC_DMA].physical_base_addr    = (uintptr)phy_base;
    rtcore_pci_dev[RT_PCIDEV_NIC_DMA].vir_base_addr         = (uintptr)vir_base;
    rtcore_pci_dev[RT_PCIDEV_NIC_DMA].mem_size              = CONSIST_MEM_RSVD_SIZE;

    for(page = virt_to_page(vir_base); page < virt_to_page(vir_base + CONSIST_MEM_RSVD_SIZE); page++)
        SetPageReserved(page);

    rtnic_complete = TRUE;
    return ret;
}

static void
rtnic_remove_one(struct pci_dev *pdev)
{
    free_irq(pdev->irq, pdev);
    pci_disable_msi(pdev);
    iounmap(ioaddr);
    pci_release_regions(pdev);
    pci_disable_device(pdev);
}

static void rtnic_shutdown(struct pci_dev *pdev)
{
    return;
}

static irqreturn_t r8111h_interrupt(int irq, void *dev_instance)
{
    writew(0x0, ioaddr + (0x3c));

    if (atomic_dec_return(&nic_wait_for_intr) >= 0)
    {
        wake_up_interruptible(&nic_intr_wait_queue);
    }

    return IRQ_RETVAL(1);
}







static irqreturn_t r9310_interrupt(int irq, void *dev_instance)
{
    _rt_util_intr_swcore_isr(NULL);

    return IRQ_RETVAL(1);
}

static int
r9310_init_one(struct pci_dev *pdev,
                 const struct pci_device_id *ent)
{
    int rc;
    uint32 addr_space;

    rc = pci_enable_device(pdev);
    if (rc < 0)
    {
        osal_printf("%s():%d  9310 pci enable failed.\n", __FUNCTION__, __LINE__);
    }

    /* make sure PCI base addr 0 is MMIO */
    if (!(pci_resource_flags(pdev, 0) & IORESOURCE_MEM)) {
        osal_printf("%s():%d\n", __FUNCTION__, __LINE__);
        rc = -ENODEV;
    }


    if ((rc = pci_request_regions(pdev, MODULENAME_FUN_1)) != 0)
    {
        osal_printf("%s():%d\n", __FUNCTION__, __LINE__);
    }

    pci_set_master(pdev);

    addr_space = pci_resource_len(pdev, 0);
    rtcore_pci_dev[RT_PCIDEV_SWCORE].physical_base_addr = pci_resource_start(pdev, 0);
    r9310_ioaddr = pci_iomap(pdev, 0, 0);
    rtcore_pci_dev[RT_PCIDEV_SWCORE].ioaddr = (uintptr)r9310_ioaddr;
    if (r9310_ioaddr == NULL)
    {
        goto err_out_free_res;
    }
    rtcore_pci_dev[RT_PCIDEV_SWCORE].mem_size = R9310_REGS_SIZE;

    //remove this remark to print message
    //osal_printf("%s():%d  pdev->irq:%#x  \n", __FUNCTION__, __LINE__, pdev->irq);
    rc = request_irq(pdev->irq, r9310_interrupt, IRQF_SHARED, "r9310", pdev);
    if (rc)
    {
        osal_printf("%s():%d  rc:%d  pdev->irq:%d\n", __FUNCTION__, __LINE__, rc, pdev->irq);
        goto err_out_free_res;
    }

    rtcore_pci_dev[RT_PCIDEV_SWCORE].pdev = pdev;
    pci_set_drvdata(pdev, &rtcore_pci_dev[RT_PCIDEV_SWCORE]);


  out:
    switch_complete = TRUE;
    return 0;

  err_out_free_res:
    pci_release_regions(pdev);

    goto out;
}

static void
r9310_remove_one(struct pci_dev *pdev)
{
    free_irq(pdev->irq, pdev);
    iounmap(r9310_ioaddr);
    pci_release_regions(pdev);
    pci_disable_device(pdev);
}

static void r9310_shutdown(struct pci_dev *pdev)
{
    return;
}

static struct pci_driver rtl8168_pci_driver = {
    .name       = MODULENAME_FUN_0,
    .id_table   = rtl8168_pci_tbl,
    .probe      = rtnic_init,
    .remove     = rtnic_remove_one,
    .shutdown   = rtnic_shutdown,
};

static struct pci_driver rtl9310_pci_driver = {
    .name       = MODULENAME_FUN_1,
    .id_table   = r9310_pci_tbl,
    .probe      = r9310_init_one,
    .remove     = r9310_remove_one,
    .shutdown   = r9310_shutdown,
};
















int32
rtcore_pci_info_get(rt_pcidevID_t pci_dev_id, rtcore_pci_dev_t *pci_info)
{

    if (pci_dev_id >= RT_PCIDEV_END)
    {
        return RT_ERR_FAILED;
    }

    *pci_info = rtcore_pci_dev[pci_dev_id];
    return RT_ERR_OK;
}


int32
rtcore_pci_init(void)
{
    uint32 ret;

   osal_memset(&rtcore_pci_dev, 0, (sizeof(rtcore_pci_dev_t) * RT_PCIDEV_END));

    if ((ret = pci_register_driver(&rtl8168_pci_driver)) < 0)
    {
        osal_printf("%s():%d  unable to register 8111H pci dev  ret:%d\n", __FUNCTION__, __LINE__, ret);
        return RT_ERR_FAILED;
    }

    if (!rtnic_complete)
        return RT_ERR_FAILED;

    if ((ret = pci_register_driver(&rtl9310_pci_driver)) < 0)
    {
        osal_printf("%s():%d  unable to register 9310 pci dev  ret:%d\n", __FUNCTION__, __LINE__, ret);
        return RT_ERR_FAILED;
    }

    if (!switch_complete)
        return RT_ERR_FAILED;

    return RT_ERR_OK;

}

int32
rtcore_pci_exit(void)
{
    struct page *page;
    uint8       *vir_base;

    if (rtcore_pci_dev[RT_PCIDEV_NIC_DMA].vir_base_addr)
    {
        vir_base = (uint8*)rtcore_pci_dev[RT_PCIDEV_NIC_DMA].vir_base_addr;
        for(page = virt_to_page(vir_base); page < virt_to_page(vir_base + CONSIST_MEM_RSVD_SIZE); page++)
            ClearPageReserved(page);

        if (vir_base != NULL)
            pci_free_consistent(rtcore_pci_dev[RT_PCIDEV_NIC_DMA].pdev, CONSIST_MEM_RSVD_SIZE, vir_base, rtcore_pci_dev[RT_PCIDEV_NIC_DMA].physical_base_addr);
    }

    pci_unregister_driver(&rtl8168_pci_driver);
    pci_unregister_driver(&rtl9310_pci_driver);
    osal_memset(&rtcore_pci_dev, 0, (sizeof(rtcore_pci_dev_t) * RT_PCIDEV_END));
    return RT_ERR_OK;
}



