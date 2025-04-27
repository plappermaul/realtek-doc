/*
 * Copyright 2012, Realtek Semiconductor Corp.
 *
 * drivers/usb/host/rtk_gen1-res.c
 *
 * $Author: cathy $
 *
 * USB resource for EHCI hcd
 */
#include <common/rt_type.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
//#include <bspchip.h>
#ifdef CONFIG_PCI
#include <linux/dma-mapping.h>
#endif
#include <bspchip.h>
#include "chip_probe.h"
//#include "platform.h"


MODULE_DESCRIPTION("Realtek USB platform device driver");
MODULE_LICENSE("GPL");


#if defined(CONFIG_USB_EHCI_HCD) || defined(CONFIG_USB_EHCI_HCD_MODULE)
#define USB_PHY_INSNREG05               0xb80210a4                          /* This is Little Endian format */
#define USB_PHY_UPCR                    0xb8000500
#define VSTATUS_IN_MASK                 0x000000ff
#define USB_PHY_FM                      0xb8140210
#define EHCI_EXT_CFG1                   0xb8140200
#define EHCI_PORTSC                     0xb8021054
#define GPIO_SEL_CTRL                   0xbb000200

#define INSNREG05_VCLM_OF               20                                  /* This is Little Endian format */
#define INSNREG05_VCLM_MASK             (0x1 << INSNREG05_VCLM_OF)          /* This is Little Endian format */
#define INSNREG05_VCTL_OF               16                                  /* This is Little Endian format */
#define INSNREG05_VCTL_MASK             (0xf << INSNREG05_VCTL_OF)          /* This is Little Endian format */
#define INSNREG05_VSTATUS_OF            24                                  /* This is Little Endian format */
#define INSNREG05_VSTATUS_MASK          (0xff << INSNREG05_VSTATUS_OF)      /* This is Little Endian format */
#define INSNREG05_VBUSY_OF              9                                   /* This is Little Endian format */
#define INSNREG05_VBUSY_MASK            (0x1 << INSNREG05_VBUSY_OF)         /* This is Little Endian format */
#define EHCI_PORTSC_CCS_OF              24                                  /* This is Little Endian format */
#define EHCI_PORTSC_CCS_MASK            (0x1 << EHCI_PORTSC_CCS_OF)         /* This is Little Endian format */
#define FM_R_FDISCON_OF                  5                                  /* This is Big    Endian format */
#define FM_R_FDISCON_MASK               (0x1 << FM_R_FDISCON_OF)            /* This is Big    Endian format */
#define EHCI_EXT_CFG1_EN_LED_OF         9                                   /* This is Big    Endian format */
#define EHCI_EXT_CFG1_EN_LED_MASK       (0x1 << EHCI_EXT_CFG1_EN_LED_OF)    /* This is Big    Endian format */
#define EHCI_EXT_CFG1_BS_LED_OF         8                                   /* This is Big    Endian format */
#define EHCI_EXT_CFG1_BS_LED_MASK       (0x1 << EHCI_EXT_CFG1_BS_LED_OF)    /* This is Big    Endian format */
#define GPIO18_USBLED_SEL_OF            10                                  /* This is Big    Endian format */
#define GPIO18_USBLED_SEL_MASK          (0x1 << GPIO18_USBLED_SEL_OF)       /* This is Big    Endian format */

#define USB_PHY_ACCESS_NOT_READ_YET 1

typedef struct usb_phy_conf_s
{
    unsigned int                reg;
    unsigned int                value;
} usb_phy_conf_t;

usb_phy_conf_t usbPhy_cali_flow[] = {
    { 0xf4, 0x9b },
    { 0xe4, 0x6c },
    { 0xe7, 0x81 },
    { 0xf4, 0xbb },
    { 0xe0, 0x21 },
    { 0xe0, 0x25 }
};

usb_phy_conf_t rtl9310_usbPhy_config_parameter[] = {
    { 0xf4, 0x9b },
    { 0xe0, 0x92 },
    { 0xe1, 0x30 },
    { 0xe2, 0xda },
    { 0xe3, 0x4d },
    { 0xe4, 0x84 },
    { 0xe5, 0x65 },
    { 0xe6, 0x01 },
    { 0xe7, 0xa1 },
    { 0xf0, 0xfc },
    { 0xf1, 0x8c },
    { 0xf2, 0x00 },
    { 0xf3, 0x11 },
    { 0xf4, 0x9b },
    { 0xf5, 0x95 },
    { 0xf6, 0x00 },
    { 0xf7, 0x0A },
    { 0xf4, 0xbb },
    { 0xe0, 0x25 },
    { 0xe1, 0xef },
    { 0xe2, 0x60 },
    { 0xe3, 0x00 },
    { 0xe4, 0x00 },
    { 0xe5, 0x11 },
    { 0xe6, 0x06 },
    { 0xe7, 0x66 }
};
usb_phy_conf_t rtl9300_usbPhy_config_parameter[] = {
    { 0xf4, 0x9b },
    { 0xe5, 0xf2 },
    { 0xe0, 0x03 },
    { 0xe6, 0x97 },
    { 0xe2, 0x97 }
};

extern void rtk_bspUSB_irq_get(uint32 *irq_num);

/*USB ready bit not work*/
void usb_phyAccessReady_wait(void)
{
    uint32 value = USB_PHY_ACCESS_NOT_READ_YET;

    do{
        value = ((REG32(USB_PHY_INSNREG05) & INSNREG05_VBUSY_MASK) >> (INSNREG05_VBUSY_OF));

    }while(value == USB_PHY_ACCESS_NOT_READ_YET);
}


/* Please check USB PHY specification for deatiled */

static void usb_phy_reg_write(unsigned int reg, unsigned int value)
{
    unsigned int    ctrl_reg_value, data_reg_value;

    data_reg_value = REG32(USB_PHY_UPCR);
    //printk("\n[0]data_reg_value = 0x%08x\n",data_reg_value);
    data_reg_value &= ~(VSTATUS_IN_MASK);
    data_reg_value |= (value & VSTATUS_IN_MASK);                        /* Need to reserve [31:8]*/
    REG32(USB_PHY_UPCR) = data_reg_value;                               /*Setup Write Data*/

    //printk("\n[0]value = 0x%08x\n",value);
    ctrl_reg_value = 0x00200000;                                        /*USB Port number : 1; (We used PHY Port 0 only)*/

    /* Set low nibble of Reg addres */
    /* VCTL 1->0->1, Send address Nibble to USB PHY*/
    ctrl_reg_value |= ((reg << INSNREG05_VCTL_OF)&INSNREG05_VCTL_MASK);
    ctrl_reg_value |= INSNREG05_VCLM_MASK;                              /*Set VCTL to HIGH*/
    //printk("[1]ctrl_reg_value = 0x%08x\n",ctrl_reg_value);
    REG32(USB_PHY_INSNREG05) = ctrl_reg_value;
    udelay(1);
    ctrl_reg_value &= ~(INSNREG05_VCLM_MASK);                           /*Set VCTL to LOW*/
    //printk("[2]ctrl_reg_value = 0x%08x\n",ctrl_reg_value);
    REG32(USB_PHY_INSNREG05) = ctrl_reg_value;
    udelay(1);
    ctrl_reg_value |= INSNREG05_VCLM_MASK;                              /*Set VCTL to HIGH*/
    //printk("[3]ctrl_reg_value = 0x%08x\n",ctrl_reg_value);
    REG32(USB_PHY_INSNREG05) = ctrl_reg_value;
    udelay(1);

    /* Set high nibble of Reg addres */
    /* VCTL 1->0->1, Send address Nibble to USB PHY*/
    ctrl_reg_value = 0x00200000;
    ctrl_reg_value |= ((reg << (INSNREG05_VCTL_OF - 4))&INSNREG05_VCTL_MASK);
    ctrl_reg_value |= INSNREG05_VCLM_MASK;                              /*Set VCTL to HIGH*/
    //printk("[4]ctrl_reg_value = 0x%08x\n",ctrl_reg_value);
    REG32(USB_PHY_INSNREG05) = ctrl_reg_value;
    udelay(1);
    ctrl_reg_value &= ~(INSNREG05_VCLM_MASK);                           /*Set VCTL to LOW*/
    //printk("[5]ctrl_reg_value = 0x%08x\n",ctrl_reg_value);
    REG32(USB_PHY_INSNREG05) = ctrl_reg_value;
    udelay(1);
    ctrl_reg_value |= INSNREG05_VCLM_MASK;                              /*Set VCTL to HIGH*/
    //printk("[6]ctrl_reg_value = 0x%08x\n",ctrl_reg_value);
    REG32(USB_PHY_INSNREG05) = ctrl_reg_value;
    udelay(1);
}

static void usb_phy_reg_read(unsigned int reg, unsigned int *pValue)
{
    unsigned int    ctrl_reg_value;

    ctrl_reg_value = 0x00200000;                                        /*USB Port number : 1; (We used PHY Port 0 only)*/

    /* Set low nibble of Reg addres */
    /* VCTL 1->0->1, Send address Nibble to USB PHY*/
    ctrl_reg_value |= ((reg << INSNREG05_VCTL_OF)&INSNREG05_VCTL_MASK);
    ctrl_reg_value |= INSNREG05_VCLM_MASK;                              /*Set VCTL to HIGH*/
    REG32(USB_PHY_INSNREG05) = ctrl_reg_value;
    udelay(1);

    ctrl_reg_value &= ~(INSNREG05_VCLM_MASK);                           /*Set VCTL to LOW*/
    REG32(USB_PHY_INSNREG05) = ctrl_reg_value;
    udelay(1);

    ctrl_reg_value |= INSNREG05_VCLM_MASK;                              /*Set VCTL to HIGH*/
    REG32(USB_PHY_INSNREG05) = ctrl_reg_value;
    udelay(1);


    /* Set high nibble of Reg addres */
    /* VCTL 1->0->1, Send address Nibble to USB PHY*/
    ctrl_reg_value = 0x00200000;
    ctrl_reg_value |= ((reg << (INSNREG05_VCTL_OF - 4))&INSNREG05_VCTL_MASK);
    ctrl_reg_value |= INSNREG05_VCLM_MASK;                              /*Set VCTL to HIGH*/
    REG32(USB_PHY_INSNREG05) = ctrl_reg_value;
    udelay(1);

    ctrl_reg_value &= ~(INSNREG05_VCLM_MASK);                           /*Set VCTL to LOW*/
    REG32(USB_PHY_INSNREG05) = ctrl_reg_value;
    udelay(1);

    ctrl_reg_value |= INSNREG05_VCLM_MASK;                              /*Set VCTL to HIGH*/
    REG32(USB_PHY_INSNREG05) = ctrl_reg_value;
    udelay(1);

    ctrl_reg_value = REG32(USB_PHY_INSNREG05);

    *pValue = ((ctrl_reg_value & INSNREG05_VSTATUS_MASK) >> INSNREG05_VSTATUS_OF);
    printk(" *pValue = 0x%x \n", *pValue);

}
void usb_phy_configure_process(void)
{
    unsigned int loop_idx;
    unsigned int bsp_chip_id = 0, bsp_chip_rev_id = 0, ret = 0;
    unsigned int bsp_chip_family_id = 0;

    ret = drv_swcore_cid_get((unsigned int)0, (unsigned int *)&bsp_chip_id, (unsigned int *)&bsp_chip_rev_id);
    if(ret == -1)
    {
        printk("\nbsp_init(), RTK Switch chip is not found!!!\n");
    }
    else
    {
        bsp_chip_family_id = FAMILY_ID(bsp_chip_id);
    }
    printk("%s: usb_phy_configure_process()!\n", __FUNCTION__);


    if((bsp_chip_family_id == RTL9310_FAMILY_ID))
    {
        /* USB Calibariton*/
        for (loop_idx = 0; loop_idx < (sizeof(usbPhy_cali_flow)/sizeof(usb_phy_conf_t)); loop_idx++)
        {
            usb_phy_reg_write(usbPhy_cali_flow[loop_idx].reg, usbPhy_cali_flow[loop_idx].value);
        }
        /* USB 9310 PHY parameter setup*/
        for (loop_idx = 0; loop_idx < (sizeof(rtl9310_usbPhy_config_parameter)/sizeof(usb_phy_conf_t)); loop_idx++)
            usb_phy_reg_write(rtl9310_usbPhy_config_parameter[loop_idx].reg, rtl9310_usbPhy_config_parameter[loop_idx].value);
    }
    else
    {
        /* USB 9300 PHY parameter setup*/
        for (loop_idx = 0; loop_idx < (sizeof(rtl9300_usbPhy_config_parameter)/sizeof(usb_phy_conf_t)); loop_idx++)
            usb_phy_reg_write(rtl9300_usbPhy_config_parameter[loop_idx].reg, rtl9300_usbPhy_config_parameter[loop_idx].value);
    }

}

static void usb_release(struct device *dev)
{
    /* normally not freed */
    dev->parent = NULL;
}

#endif

#if defined(CONFIG_USB_EHCI_HCD) || defined(CONFIG_USB_EHCI_HCD_MODULE)
static struct resource rtk_gen1_ehci_resources[] = {
    {
        #ifndef CONFIG_PCI
        .start  = RTK_EHCI_BASE,
        .end    = RTK_EHCI_BASE + 0x0000EFFF,
        #else
        .start	= PADDR(RTK_EHCI_BASE),
        .end	= PADDR(RTK_EHCI_BASE) + 0x0000EFFF,
        #endif
        .flags	= IORESOURCE_MEM,
    },
    {
        .start	= BSP_USB_H2_IRQ, /* BSP_IRQ_ICTL_BASE + USB_H2_IRQ */
        .flags	= IORESOURCE_IRQ,
    },
};

static u64 rtk_gen1_ehci_dma_mask = 0x3fffffffULL;

static struct platform_device rtk_gen1_ehci = {
    .name = "rtk_gen1-ehci",
    .id	= -1,
    .dev = {
        .release = usb_release,
        #ifndef CONFIG_PCI
        .dma_mask = &rtk_gen1_ehci_dma_mask,
        .coherent_dma_mask = 0x3fffffffULL,
        #else
        .dma_mask = &rtk_gen1_ehci_dmamask,
       .coherent_dma_mask = DMA_BIT_MASK(32),
        #endif
    },
    .num_resources = ARRAY_SIZE(rtk_gen1_ehci_resources),
    .resource = rtk_gen1_ehci_resources,
};
#endif //CONFIG_USB_EHCI_HCD || CONFIG_USB_EHCI_HCD_MODULE

void usb_host_check_port_status(void)
{
    uint reg_val = 0;
    unsigned int bsp_chip_id = 0, bsp_chip_rev_id = 0, ret = 0;
    unsigned int bsp_chip_family_id = 0;

    ret = drv_swcore_cid_get((unsigned int)0, (unsigned int *)&bsp_chip_id, (unsigned int *)&bsp_chip_rev_id);
    if(ret == -1)
    {
        printk("\nbsp_init(), RTK Switch chip is not found!!!\n");
    }
    else
    {
        bsp_chip_family_id = FAMILY_ID(bsp_chip_id);
    }

    if(((bsp_chip_family_id == RTL9300_FAMILY_ID) && (bsp_chip_rev_id == CHIP_REV_ID_A))
        || (bsp_chip_family_id == RTL9310_FAMILY_ID))
    {

        /*Set GPIO pin to USB LED function */
        reg_val = REG32(EHCI_PORTSC);
        reg_val &= (EHCI_PORTSC_CCS_MASK);
        if(reg_val == 0)
        {
            reg_val = REG32(EHCI_EXT_CFG1);
            reg_val |= (EHCI_EXT_CFG1_EN_LED_MASK);
            REG32(EHCI_EXT_CFG1) = reg_val;

            reg_val = REG32(EHCI_EXT_CFG1);
            reg_val &= ~(EHCI_EXT_CFG1_EN_LED_MASK);
            REG32(EHCI_EXT_CFG1) = reg_val;
        }
    }
}


void usb_host_discon_config(void)
{
    uint reg_val = 0;

    /*Do not By pass phy detect.*/
    reg_val = REG32(USB_PHY_FM);
    reg_val &= ~(FM_R_FDISCON_MASK);
    REG32(USB_PHY_FM) = reg_val;
}

void usb_host_led_config(void)
{
    uint reg_val = 0;
    unsigned int bsp_chip_id = 0, bsp_chip_rev_id = 0, ret = 0;
    unsigned int bsp_chip_family_id = 0;

    ret = drv_swcore_cid_get((unsigned int)0, (unsigned int *)&bsp_chip_id, (unsigned int *)&bsp_chip_rev_id);
    if(ret == -1)
    {
        printk("\nbsp_init(), RTK Switch chip is not found!!!\n");
    }
    else
    {
        bsp_chip_family_id = FAMILY_ID(bsp_chip_id);
    }

    if((bsp_chip_family_id == RTL9300_FAMILY_ID))
    {
        /*Set GPIO pin to USB LED function */
        reg_val = REG32(GPIO_SEL_CTRL);
        reg_val |= (GPIO18_USBLED_SEL_MASK);
        REG32(GPIO_SEL_CTRL) = reg_val;
    }

    /* Config USB LED Blinking Speed */
    reg_val = REG32(EHCI_EXT_CFG1);
    reg_val |= (EHCI_EXT_CFG1_BS_LED_MASK);
    REG32(EHCI_EXT_CFG1) = reg_val;
}

int rtk_gen1_hcd_cs_init (void)
{
    int retval = 0;
    uint32  usb_irq;

    rtk_bspUSB_irq_get(&usb_irq);

    printk("%s: rtk_gen1_hcd_cs_init(); USB IRQ = %d\n", __FUNCTION__,usb_irq);

#if defined(CONFIG_USB_EHCI_HCD) || defined(CONFIG_USB_EHCI_HCD_MODULE)
    retval = platform_device_register(&rtk_gen1_ehci);
    if (retval) {
        printk("%s: fail to register rtk_gen1_ehci! (%d [%x])\n", __FUNCTION__,retval,retval);
        return retval;
    }
    printk("%s: register rtk_gen1_ehci ok!\n", __FUNCTION__);
    usb_host_discon_config();
    usb_host_led_config();
    usb_phy_configure_process();
#endif //CONFIG_USB_EHCI_HCD || CONFIG_USB_EHCI_HCD_MODULE

    return retval;
}
//module_init (rtk_gen1_hcd_cs_init);

static void __exit rtk_gen1_hcd_cs_exit(void)
{

#if defined(CONFIG_USB_EHCI_HCD) || defined(CONFIG_USB_EHCI_HCD_MODULE)
    platform_device_unregister(&rtk_gen1_ehci);
#endif //CONFIG_USB_EHCI_HCD || CONFIG_USB_EHCI_HCD_MODULE
    return;
}
module_exit(rtk_gen1_hcd_cs_exit);

