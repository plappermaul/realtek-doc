#include <common.h>
#include <usb.h>
#include <asm/io.h>
#include "ehci.h"
#include "ehci-core.h"

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

#define USB_PHY_ACCESS_NOT_READ_YET     1

#define EHCI2_BASE                      0xb8021200UL
#define RTK_EHCI_BASE_LEN               0xa4


struct ehci_hc {
	unsigned long base;
	unsigned long utmi_reg;
	unsigned long usb2_ext_reg;
};

typedef struct usb_phy_conf_s
{
    unsigned int                reg;
    unsigned int                value;
} usb_phy_conf_t;

static const struct ehci_hc ehci2 = {
	.base         = EHCI2_BASE,
	.utmi_reg     = EHCI2_BASE + RTK_EHCI_BASE_LEN,
	.usb2_ext_reg = 0xB8140300UL,
};

usb_phy_conf_t rtl9300_usbPhy_config_parameter[] = {
    { 0xf4, 0x9b },
    { 0xe5, 0xf2 }
};

void usb_host_discon_config(void)
{
    uint reg_val = 0;

    /*Do not By pass phy detect.*/
    reg_val = REG32(USB_PHY_FM);
    reg_val &= ~(FM_R_FDISCON_MASK);
    REG32(USB_PHY_FM) = reg_val;
}

static void usb_phy_reg_write(unsigned int reg, unsigned int value)
{
    unsigned int    ctrl_reg_value, data_reg_value;

    data_reg_value = REG32(USB_PHY_UPCR);
    data_reg_value &= ~(VSTATUS_IN_MASK);
    data_reg_value |= (value & VSTATUS_IN_MASK);                        /* Need to reserve [31:8]*/
    REG32(USB_PHY_UPCR) = data_reg_value;                               /*Setup Write Data*/

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
}
#if CONFIG_RTK_USB_PHYREG_READ
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
#endif

void usb_host_led_config(void)
{
    uint reg_val = 0;

    /*Set GPIO pin to USB LED function */
    reg_val = REG32(GPIO_SEL_CTRL);
    reg_val |= (GPIO18_USBLED_SEL_MASK);
    REG32(GPIO_SEL_CTRL) = reg_val;

    /* Config USB LED Blinking Speed */
    reg_val = REG32(EHCI_EXT_CFG1);
    reg_val |= (EHCI_EXT_CFG1_BS_LED_MASK);
    REG32(EHCI_EXT_CFG1) = reg_val;
}

void usb_phy_configure_process(void)
{
    unsigned int loop_idx;

    /* USB 9300 PHY parameter setup*/
    for (loop_idx = 0; loop_idx < (sizeof(rtl9300_usbPhy_config_parameter)/sizeof(usb_phy_conf_t)); loop_idx++)
        usb_phy_reg_write(rtl9300_usbPhy_config_parameter[loop_idx].reg, rtl9300_usbPhy_config_parameter[loop_idx].value);
}


int ehci_hcd_init(void)
{
    usb_host_discon_config();
    usb_host_led_config();
    usb_phy_configure_process();

	hccr = (struct ehci_hccr *)ehci2.base;
	hcor = (struct ehci_hcor *)((uint32_t) hccr +
			HC_LENGTH(ehci_readl(&hccr->cr_capbase)));

	return 0;
}

int ehci_hcd_stop(void)
{
	return 0;
}

