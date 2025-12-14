/*
 * Configuration for Cortina-Access Presidio board. Parts were derived from other ARM
 *   configurations.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

/*
 * This file is a conglomeration for DWC3-init sequence and further
 * Cortina G3 specific PHY-init sequence.
 */

#include <common.h>
#include <fdtdec.h>
//
#include <linux/libfdt.h>
#include <malloc.h>
#include <usb.h>
#include <watchdog.h>
#include <asm/gpio.h>
//#include <asm-generic/errno.h>
#include <linux/errno.h>
#include <linux/compat.h>
#include <linux/usb/dwc3.h>


#include "xhci.h"

#define USB_0_BASE 0xf0200000
#define USB_1_BASE 0xf0400000

#define XHCI_BASE_0 USB_0_BASE
#define XHCI_BASE_1 USB_1_BASE

/* Declare global data pointer */
DECLARE_GLOBAL_DATA_PTR;

/**
 * Contains pointers to register base addresses
 * for the usb controller.
 */
struct ca7774_xhci {
    struct xhci_hccr *hcd;
    struct dwc3 *dwc3_reg;
};

static struct ca7774_xhci g_ca_dw3_0,g_ca_dw3_1;

#if 0
static void ca7774_usb3_phy_init()
{
    /* TBD */
    printf("ca7774_usb3_phy_init\n");
    u32 reg,reg_value;
    reg = 0xf432b000;
    reg_value = readl(reg);

    writel(0xE8200010, 0xf432b000);
    writel(0x00000184, 0xf4320008);
}
#endif
#if 0
#define BIT(nr)			(1UL << (nr))
#define USB2CFG_CNTRL_OFFSET				0x00
#define USB2CFG_PHY_VAUX_RESET					BIT(31)
#define USB2CFG_BUS_CLKEN_GLAVE					BIT(30)
#define USB2CFG_BUS_CLKEN_GMASTER				BIT(29)
#define USB2CFG_BIGENDIAN_GSLAVE				BIT(28)
#define USB2CFG_HOST_PORT_POWER_CTRL_PRESENT	BIT(27)
#define USB2CFG_HOST_MSI_ENABLE					BIT(26)
#define USB2CFG_HOST_LEGACY_SMI_PCI_CMD_REG_WR	BIT(25)
#define USB2CFG_HOST_LEGACY_SMI_BAR_WR			BIT(24)
#define USB2CFG_FLADJ_30MHZ_REG_MASK(x)			((0x3f & (x)) << 16)
#define USB2CFG_PHY_VAUX_RESET_PORT2            BIT(10)
#define USB2CFG_PHY_VAUX_RESET_PORT1            BIT(9)
#define USB2CFG_PHY_VAUX_RESET_PORT0            BIT(8)
#define USB2CFG_XHCI_BUS_MASTER_ENABLE			BIT(4)

#define USB2CFG_STATUS_OFFSET				0x04
#define USB2CFG_HOST_SYSTEM_ERR					BIT(31)
#define USB2CFG_LEGACY_SMI_INTERRUPT			BIT(16)
#define USB2CFG_HOST_CURRENT_BELT_MASK(x)		(0xfff & (x))

#define USB2CFG_PORT_CONFIG_OFFSET			0x08
#define USB2CFG_HOST_DISABLE					BIT(2)
#define USB2CFG_HUB_PORT_OVERCURRENT			BIT(1)
#define USB2CFG_HUB_PORT_PERM_ATTACH			BIT(0)

#define USB2CFG_PORT_STATUS_OFFSET			0x0c
#define USB2CFG_UTMI_FSLS_LOW_POWER				BIT(1)
#define USB2CFG_HUB_VBUS_CTRL					BIT(0)

#define USB2CFG_PHY_PORT0_CONFIG_OFFSET		0x10
#define USB2CFG_PHY_PORT1_CONFIG_OFFSET		0x18
#define USB2CFG_PHY_PORT2_CONFIG_OFFSET		0x20
#define USB2CFG_PHY_PORT_VSTATUS_IN_MASK(x)		((0xff & (x)) << 24)
#define USB2CFG_PHY_PORT_VCNTRL_MASK(x)		((0xf & (x)) << 20)
#define USB2CFG_PHY_PORT_VLOADM					BIT(19)
#define USB2CFG_PHY_PORT_BY_PASS_ON				BIT(18)
#define USB2CFG_PHY_PORT_LF_PD_R_EN				BIT(9)
#define USB2CFG_PHY_PORT_CLKTSTEN				BIT(8)
#define USB2CFG_PHY_PORT_DPPULLDOWN				BIT(6)
#define USB2CFG_PHY_PORT_DMPULLDOWN				BIT(5)
#define USB2CFG_PHY_PORT_TXBITSTUFF_ENABLE		BIT(4)
#define USB2CFG_PHY_PORT_TXBITSTUFF_ENABLE_H	BIT(3)
#define USB2CFG_PHY_PORT_TX_ENABLE_N			BIT(2)
#define USB2CFG_PHY_PORT_TX_DAT					BIT(1)
#define USB2CFG_PHY_PORT_TX_SE0					BIT(0)

#define USB2CFG_PHY_PORT0_STATUS_OFFSET		0x14
#define USB2CFG_PHY_PORT1_STATUS_OFFSET		0x1c
#define USB2CFG_PHY_PORT2_STATUS_OFFSET		0x24
#define USB2CFG_PHY_PORT_VSTATUS_OUT_MASK(x)	((0xff & (x)) << 24)
#define USB2CFG_PHY_PORT_DEBUG_MASK(x)			(0xff & (x))


#define USB2PHY_REG    0xf432b028

#define U2_PHY_DATA_SIZE       9;
u8 u2_phy_data_addr[] = ///bits/ 8
			{0xF4 , 0xE0 , 0xE0 , 0xF4 , 0xE4 , 0xF5 , 0xF7 , 0xF4 , 0xE3};


u8 u2_phy_data_array[] = ///bits/ 8
			{0xBB , 0x21 , 0x25 , 0x9B , 0x8B , 0x01 , 0x32 , 0xBB , 0x44};


static void ca_usb2_phy_write(
	u8 vstat_in_mask, u8 vctrl_mask, int port_id)
{
	u32 offset = 0;
	u32 reg_val = 0;
	u8 vctrl_mask1, vctrl_mask2;

	vctrl_mask1 = vctrl_mask & 0x0F;
	vctrl_mask2 = (vctrl_mask >> 4) & 0x0F;
	reg_val |= (USB2CFG_PHY_PORT_VSTATUS_IN_MASK(vstat_in_mask)
		| USB2CFG_PHY_PORT_VCNTRL_MASK(vctrl_mask1)
		| USB2CFG_PHY_PORT_DPPULLDOWN
		| USB2CFG_PHY_PORT_DMPULLDOWN
		| USB2CFG_PHY_PORT_VLOADM
		| USB2CFG_PHY_PORT_TX_ENABLE_N);

	if (port_id == 0)
		offset = USB2CFG_PHY_PORT0_CONFIG_OFFSET;
	else if (port_id == 1)
		offset = USB2CFG_PHY_PORT1_CONFIG_OFFSET;
	else if (port_id == 2)
		offset = USB2CFG_PHY_PORT2_CONFIG_OFFSET;
	else
		printf("The usb2phy port num is unvalid.\n");

    //printf("ca_usb2_phy_write %x %x %x\n",USB2PHY_REG + offset,vctrl_mask,vstat_in_mask);

	writel(reg_val, USB2PHY_REG + offset);

	reg_val &= ~USB2CFG_PHY_PORT_VLOADM;
	writel(reg_val, USB2PHY_REG + offset);

	reg_val |= USB2CFG_PHY_PORT_VLOADM;
	writel(reg_val, USB2PHY_REG + offset);

	/* Clear VCONTROL field before refilling */
	reg_val &= ~USB2CFG_PHY_PORT_VCNTRL_MASK(0xF);
	reg_val |= USB2CFG_PHY_PORT_VCNTRL_MASK(vctrl_mask2);
	writel(reg_val, USB2PHY_REG + offset);

	reg_val &= ~USB2CFG_PHY_PORT_VLOADM;
	writel(reg_val, USB2PHY_REG + offset);

	reg_val |= USB2CFG_PHY_PORT_VLOADM;
	writel(reg_val, USB2PHY_REG + offset);

}

#define U3HOST_REG
#define S2USBPHY_U3PORT0    0xf4337000
#define S3USBPHY_U3PORT1    0xf4338000

#define PHY_DATA_SIZE       17;
u8 phy_data_addr[] = ///bits/ 8
			{0x01 , 0x26 , 0x2f , 0x0a , 0x06 , 0x23 , 0x20 , 0x21 , 0x22 , 0x27 , 0x08 , 0x0b , 0x0e , 0x0c , 0x09 , 0x09 ,
			 0x09};



u16 phy_data_array[] = ///bits/ 16
			{0xAC46 , 0x840E , 0x8648 , 0xC608 , 0x000c , 0xCB66 , 0x7069 , 0x68AA , 0x0013 , 0x0266 , 0x3011 , 0xB905 , 0x1000 , 0xC008 , 0x524C , 0x504C ,
			 0x524C};



static inline void ca_usb3_phy_write(unsigned addr, u16 data, int port_id)
{
	if (port_id == 0)
		writel(data, S2USBPHY_U3PORT0 + (addr << 2));
	if (port_id == 1)
		writel(data, S3USBPHY_U3PORT1 + (addr << 2));

}


static void ca7774_usb3_phy_init()
{

	u8 *addr = phy_data_addr;
	u16 *data = phy_data_array;
	int size = PHY_DATA_SIZE;
	int index,port_id;

	u8 *u2_addr = u2_phy_data_addr;
	u8 *u2_data = u2_phy_data_array;

	int time = 1;
	uint32_t value, usb;

	gpio_direction_output(4, 0);



	/* ============================== */
	/* force reset, S2/S3 */
	/* GLOBAL_BLOCK_RESET */
	writel(0xc03021c0, 0xf4320028);
	udelay(time);
	/* GLOBAL_DPHY_RESET */
	writel(0x000001bf, 0xf432002c);
	udelay(time);
	/* GLBAL_PHY_CONTROL */
	writel(0x00008803, 0xf4320050);
	udelay(time);

	/* USB2 DPPULLDOWN/DMPULLDOWN */
	writel(0x00080064, 0xf432b038);
	//udelay(time);
	writel(0x00080064, 0xf432b040);
	udelay(time);

#if 1
	for (index = 0; index < size; index++) {
		ca_usb3_phy_write( *(addr + index),
			*(data + index), 0);
        ca_usb3_phy_write( *(addr + index),
			*(data + index), 1);
	}

    size = U2_PHY_DATA_SIZE;
	/* USB2 PHY port[0:3] calibration and initialization */
	for (port_id = 0; port_id < 3; port_id++) {
		for (index = 0; index < size ; index++) {
			ca_usb2_phy_write(*(u2_data + index),
				*(u2_addr + index), port_id);
		}
	}
#endif

	/* ============================== */
	/* release reset */
	/* GLBAL_PHY_CONTROL */
	writel(0x0182a803, 0xf4320050);
	udelay(time);
	/* GLOBAL_DPHY_RESET */
	writel(0x000001a7, 0xf432002c);
	udelay(time);
	/* GLOBAL_BLOCK_RESET */
	writel(0xc03001c0, 0xf4320028);
	udelay(time);

	/* ============================== */
	/* USBCFG_U3_CONTROL (def 0x68200010) */
	writel(0xe8200010, 0xf432b000);
	udelay(time);
    /*# USBCFG_U2_CONTROL*/
	writel(0xe8200710, 0xf432b028);
	udelay(time);

	/* # xHCI (U3 0xf020_0000, U2 0xf040_0000) */
	/* GUSB3PIPECTL0 (def 0x010e0002) */
	writel(0x010c0003, 0xf020c2c0);
	//udelay(time);
    writel(0x010c0003, 0xf040c2c0);
	udelay(time);

    writel(0x010c0003, 0xf020c2c4);
    writel(0x010c0003, 0xf040c2c4);
    udelay(time);

	/* USBCFG_U2_CONTROL */
	//writel(0xe8200710, 0xf432b028);
	//udelay(time);

	/* GCTL */
	/* disable scrambling, GCTL[3]=1 */
	writel(0x2ee01000, 0xf020c110);
	//udelay(time);
    writel(0x2ee01000, 0xf040c110);
	udelay(time);


	/* GSBUSCFG0 */
	writel(0x0000000f, 0xf020c100);
	//udelay(time);
	writel(0x0000000f, 0xf040c100);
	udelay(time);
	/* GTXTHRCFG */
	writel(0x23100000, 0xf020c108);
	writel(0x23100000, 0xf040c108);
	udelay(time);
	/* GRXTHRCFG */
	writel(0x03800000, 0xf020c10c);
	writel(0x03800000, 0xf040c10c);
	udelay(time);
	/* GUSB2PHYCFG-p0, UTMI 8 bits */
	writel(0x40102400, 0xf020c200);
	writel(0x40102400, 0xf040c200);
	udelay(time);
	/* GUSB2PHYCFG-p1, UTMI 8 bits */
	writel(0x40102400, 0xf020c204);
    writel(0x40102400, 0xf040c204);
	udelay(time);
    mdelay(200);

	/* ============================== */
	/* check U3 LTSSM state */
	/* portsc[8:5] link state=0, [13:10] port speed */
	/* port S2 link status (port 3, SS 0) */
	/* 0xf0200440 expect 0x00xx1203 */
	/* port S3 link status (port 4, SS 1) */
	/* 0xf0200450 expect 0x00xx1203 */
	value = readl(0xf0200440);
	udelay(time);
	printf("0xf0200440: 0x%x\n", value);
	value = readl(0xf0200450);
	udelay(time);
	printf("0xf0200450: 0x%x\n\n", value);
	/* =============================== */
	/* USB2 port, program after device inserted */
	/* should be programmed by the xHCI driver */
	/* PORTSC, enable port */
	//writel(0x000206f3, 0xf0200420);
    //writel(0x000206f3, 0xf0400420);
	udelay(time);
	/* PORTSC, enable port */
	//writel(0x000206f3, 0xf0200430);
    //writel(0x000206f3, 0xf0400430);
	udelay(time);
	/* PORTSC, expect 0x00200e03, [8:5] link state=0 */
    //writel(0x000206f3, 0xf0200420);
    //writel(0x000206f3, 0xf0200430);

	gpio_set_value(4, 0);
	mdelay(500);
	gpio_set_value(4, 1);
	mdelay(2000);

    writel(0x000206f3, 0xf0200420);
    writel(0x000206f3, 0xf0200430);
    writel(0x000206f3, 0xf0400420);

    mdelay(200);

	usb = readl(0xf0200450);
	udelay(time);
	printf("0xf0200450: 0x%x\n", usb);

	if ((usb & 0x0000FFFF) == 0x1203) {
		printf("{USB3:Link PASS}\n\n");
	} else {
		printf("{USB3:Link FAIL}\n\n");
	}

    usb = readl(0xf0200420);

	if ((usb & 0x0000FFFF) == 0x0e03) {
		printf("{USB2-port0:Link PASS}\n\n");
	} else {
		printf("{USB2-port0:Link FAIL}\n\n");
		printf("0xf0200420: 0x%x\n", usb);
	}

    usb = readl(0xf0200430);

	if ((usb & 0x0000FFFF) == 0x0e03) {
		printf("{USB2-port1:Link PASS}\n\n");
	} else {
		printf("{USB2-port1:Link FAIL}\n\n");
		printf("0xf0200430: 0x%x\n", usb);
	}

    usb = readl(0xf0400420);

	if ((usb & 0x0000FFFF) == 0x0e03) {
		printf("{USB2-port2:Link PASS}\n\n");
	} else {
		printf("{USB2-port2:Link FAIL}\n\n");
		printf("0xf0400420: 0x%x\n", usb);
	}

	return 0;
}
#endif

static void ca7774_usb3_phy_exit()
{
    /* TBD */
}

static void dwc3_set_mode(struct dwc3 *dwc3_reg, u32 mode)
{
    clrsetbits_le32(&dwc3_reg->g_ctl,
		    DWC3_GCTL_PRTCAPDIR(DWC3_GCTL_PRTCAP_OTG),
		    DWC3_GCTL_PRTCAPDIR(mode));
}

static void dwc3_core_soft_reset(struct dwc3 *dwc3_reg)
{
    /* Before Resetting PHY, put Core in Reset */
    setbits_le32(&dwc3_reg->g_ctl, DWC3_GCTL_CORESOFTRESET);

    /* Assert USB3 PHY reset */
    setbits_le32(&dwc3_reg->g_usb3pipectl[0], DWC3_GUSB3PIPECTL_PHYSOFTRST);
    setbits_le32(&dwc3_reg->g_usb3pipectl[1], DWC3_GUSB3PIPECTL_PHYSOFTRST);

    /* Assert USB2 PHY reset */
    setbits_le32(&dwc3_reg->g_usb2phycfg[0], DWC3_GUSB2PHYCFG_PHYSOFTRST);
    setbits_le32(&dwc3_reg->g_usb2phycfg[1], DWC3_GUSB2PHYCFG_PHYSOFTRST);

    mdelay(100);

    /* Clear USB3 PHY reset */
    clrbits_le32(&dwc3_reg->g_usb3pipectl[0], DWC3_GUSB3PIPECTL_PHYSOFTRST);
    clrbits_le32(&dwc3_reg->g_usb3pipectl[1], DWC3_GUSB3PIPECTL_PHYSOFTRST);

    /* Clear USB2 PHY reset */
    clrbits_le32(&dwc3_reg->g_usb2phycfg[0], DWC3_GUSB2PHYCFG_PHYSOFTRST);
    clrbits_le32(&dwc3_reg->g_usb2phycfg[1], DWC3_GUSB2PHYCFG_PHYSOFTRST);

    //writel(0x40102400, &dwc3_reg->g_usb2phycfg[0]);
    //writel(0x40102400, &dwc3_reg->g_usb2phycfg[1]);


    /* After PHYs are stable we can take Core out of reset state */
    clrbits_le32(&dwc3_reg->g_ctl, DWC3_GCTL_CORESOFTRESET);
}

static int dwc3_core_init(struct dwc3 *dwc3_reg)
{
    u32 reg;
    u32 revision;
    unsigned int dwc3_hwparams1;
    
    revision = readl(&dwc3_reg->g_snpsid);
    /* This should read as U3 followed by revision number */
    if ((revision & DWC3_GSNPSID_MASK) != 0x55330000) {
	printf("this is not a DesignWare USB3 DRD Core\n");
	return -EINVAL;
    }
    dwc3_core_soft_reset(dwc3_reg);
    dwc3_hwparams1 = readl(&dwc3_reg->g_hwparams1);

    reg = readl(&dwc3_reg->g_ctl);
    reg &= ~DWC3_GCTL_SCALEDOWN_MASK;
    reg &= ~DWC3_GCTL_DISSCRAMBLE;
    switch (DWC3_GHWPARAMS1_EN_PWROPT(dwc3_hwparams1)) {
	case DWC3_GHWPARAMS1_EN_PWROPT_CLK:
	    reg &= ~DWC3_GCTL_DSBLCLKGTNG;
	    break;
	default:
	    printf("No power optimization available\n");
    }
    /*
     * WORKAROUND: DWC3 revisions <1.90a have a bug
     * where the device can fail to connect at SuperSpeed
     * and falls back to high-speed mode which causes
     * the device to enter a Connect/Disconnect loop
     */
    if ((revision & DWC3_REVISION_MASK) < 0x190a)
    {
	    reg |= DWC3_GCTL_U2RSTECN;
    }

    //writel(reg, &dwc3_reg->g_ctl);

    return 0;
}

static int ca7774_xhci_core_init(struct ca7774_xhci *ca_dw3)
{
    int ret;

   

    ret = dwc3_core_init(ca_dw3->dwc3_reg);
    if (ret) {
	debug("failed to initialize core\n");
	return -EINVAL;
    }
    /* We are hard-coding DWC3 core to Host Mode */
    //dwc3_set_mode(ca_dw3->dwc3_reg, DWC3_GCTL_PRTCAP_HOST);
    return 0;
}

static void ca7774_xhci_core_exit(struct ca7774_xhci *ca_dw3)
{
    ca7774_usb3_phy_exit();
}

int xhci_hcd_init(int index, struct xhci_hccr **hccr, struct xhci_hcor **hcor)
{
    struct ca7774_xhci *ctx=&g_ca_dw3_0;
    int ret;
uint32_t reg;
    if (index == 0)
    {
        printf("xhci_hcd_init 0\n");
        ctx = &g_ca_dw3_0;
        ctx->hcd = (struct xhci_hccr *)XHCI_BASE_0;
        //ca7774_usb3_phy_init();

        ctx->dwc3_reg = (struct dwc3 *)((char *)(ctx->hcd) + DWC3_REG_OFFSET);
        ret = ca7774_xhci_core_init(ctx);
        if (ret) {
	        printf("XHCI: failed to initialize controller\n");
	        return -EINVAL;
        }
    }else if (index == 1)
    {
        printf("xhci_hcd_init 1\n");
        ctx = &g_ca_dw3_1;
        ctx->hcd = (struct xhci_hccr *)XHCI_BASE_1;

         ctx->dwc3_reg = (struct dwc3 *)((char *)(ctx->hcd) + DWC3_REG_OFFSET);
        ret = ca7774_xhci_core_init(ctx);
        if (ret) {
	        printf("XHCI: failed to initialize controller\n");
	        return -EINVAL;
        }
    }


    

    *hccr = (ctx->hcd);
    *hcor = (struct xhci_hcor *)((uint32_t) * hccr
			 + HC_LENGTH(xhci_readl(&(*hccr)->cr_capbase)));

    debug("Cortina-xhci: init hccr %x and hcor %x hc_length %d\n",
	(uint32_t) * hccr, (uint32_t) * hcor,
	(uint32_t) HC_LENGTH(xhci_readl(&(*hccr)->cr_capbase)));
    

    return 0;
}

void xhci_hcd_stop(int index)
{
    struct ca7774_xhci *ctx = &g_ca_dw3_0;
    if (index==0)
    {
        ctx = &g_ca_dw3_0;
    }else if(index==1)
    {
        ctx = &g_ca_dw3_1;
    }

    ca7774_xhci_core_exit(ctx);
}


void usb_host_early_init(void)
{
        int time = 1;
        /* # xHCI (U3 0xf020_0000, U2 0xf040_0000) */
        /* GUSB3PIPECTL0 (def 0x010e0002) */
        writel(0x010c0003, 0xf020c2c0);
        //udelay(time);
    writel(0x010c0003, 0xf040c2c0);
        udelay(time);

    writel(0x010c0003, 0xf020c2c4);
    writel(0x010c0003, 0xf040c2c4);
    udelay(time);

        /* USBCFG_U2_CONTROL */
        //writel(0xe8200710, 0xf432b028);
        //udelay(time);

        /* GCTL */
        /* disable scrambling, GCTL[3]=1 */
        writel(0x2ee01000, 0xf020c110);
        //udelay(time);
    writel(0x2ee01000, 0xf040c110);
        udelay(time);


        /* GSBUSCFG0 */
        writel(0x0000000f, 0xf020c100);
        //udelay(time);
        writel(0x0000000f, 0xf040c100);
        udelay(time);
        /* GTXTHRCFG */
        writel(0x23100000, 0xf020c108);
        writel(0x23100000, 0xf040c108);
        udelay(time);
        /* GRXTHRCFG */
        writel(0x03800000, 0xf020c10c);
        writel(0x03800000, 0xf040c10c);
        udelay(time);
        /* GUSB2PHYCFG-p0, UTMI 8 bits */
        writel(0x40102400, 0xf020c200);
        writel(0x40102400, 0xf040c200);
        udelay(time);
        /* GUSB2PHYCFG-p1, UTMI 8 bits */
        writel(0x40102400, 0xf020c204);
    writel(0x40102400, 0xf040c204);
        udelay(time);

}

