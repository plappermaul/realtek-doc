#include <common.h>
#include <dm.h>
#include <fdtdec.h>
#include <generic-phy.h>
#include <usb.h>
#include <dwc3-uboot.h>

#include <usb/xhci.h>
#include <asm/io.h>
#include <linux/usb/dwc3.h>
#include <linux/usb/otg.h>
#include <linux/io.h>

#define BLK_RESET			(0xF4320098)
#define RESET_USB 			BIT(13)

#define BLK_RESET_EXT		(0xF432009C)
#define RESET_USB2_PORT_1 	BIT(5)

#define USB3CFG_PHY			(0xF432B000)
#define USB2CFG_PHY			(0xF432B020)
#define USB3EXT_REG			(0xF432B04C)
#define SERDES_PHY			(0xF4337000)

#define PHY_CONTROL			(0xF43200C4)
#define SYS_S2_USB3_SEL		BIT(19)
#define S2_PCIE_USB3_SEL	BIT(20)
#define S3_PCIE_USB3_SEL	BIT(24)

#define PHY_ISO_POWER_CTRL	(0xF432036C)
#define S2_POWER_USB3		BIT(11)

/*** Register Offset ***/
#define CORTINA_XHCI_GUSB3PIPECTL0	    0xc2c0	/* data: 0x010c0003 */
#define CORTINA_XHCI_GUSB3PIPECTL1	    0xc2c4	/* data: 0x010c0003 */
#define CORTINA_XHCI_GCTL	            0xc110	/* data: 0x2ee01000 */
#define CORTINA_XHCI_GSBUSCFG0	        0xc100	/* data: 0x0000000f */
#define CORTINA_XHCI_GTXTHRCFG	        0xc108	/* data: 0x23100000 */
#define CORTINA_XHCI_GRXTHRCFG	        0xc10c	/* data: 0x03800000 */
#define CORTINA_XHCI_GUSB2PHYCFG_P0		0xc200	/* data: 0x40102400 */
#define CORTINA_XHCI_GUSB2PHYCFG_P1		0xc204	/* data: 0x40102400 */
#define CORTINA_XHCI_U3S2_PORTSC_P0		0x0440	/* # U3 S2 link status (port3, SS 0) */
#define CORTINA_XHCI_U3S3_PORTSC_P1		0x0450	/* # U3 S3 link status (port4, SS 1) */
#define CORTINA_XHCI_GUSB2PORTSC_P0		0x0420	/* data: 0x000206f3 */
#define CORTINA_XHCI_GUSB2PORTSC_P1		0x0430	/* data: 0x000206f3 */

/* Register configuration values */
#define CORTINA_XHCI_GUSB3GPIPECTL_VAL	0x010c0003
#define CORTINA_XHCI_GCTL_VAL			0x2ee01000
#define CORTINA_XHCI_GSBUSCFG0_VAL		0x0000000f
#define CORTINA_XHCI_GTXTHRCFG_VAL		0x23100000
#define CORTINA_XHCI_GRXTHRCFG_VAL		0x03800000
#define CORTINA_XHCI_GUSB2PHYCFG_VAL	0x40102400
#define CORTINA_XHCI_USB2PORTSC_VAL		0x000206f3

/* USB2(USB High Speed) register offsets */
#define USB2CFG_PHY_PORT0_CONFIG_OFFSET		0x00
#define USB2CFG_PHY_PORT1_CONFIG_OFFSET		0x08
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

#define USB2CFG_PHY_PORT0_STATUS_OFFSET		0x04
#define USB2CFG_PHY_PORT1_STATUS_OFFSET		0x0c
#define USB2CFG_PHY_PORT_VSTATUS_OUT_MASK(x)	((0xff & (x)) << 24)
#define USB2CFG_PHY_PORT_DEBUG_MASK(x)			(0xff & (x))

#define USB2CFG_PHY_PORT0_FORCE_MODE_OFFSET 0x10
#define USB2CFG_PHY_PORT0_RX_TERM_SEL			BIT(9)
#define USB2CFG_PHY_PORT1_FORCE_MODE_OFFSET 0x14
#define USB2CFG_PHY_PORT1_RX_TERM_SEL			BIT(25)

/* USB3(USB Super Speed) register offsets */
#define USB3CFG_CNTRL_OFFSET				0x00
#define USB3CFG_PHY_VAUX_RESET					BIT(31)
#define USB3CFG_BUS_CLKEN_GSLAVE				BIT(30)
#define USB3CFG_BUS_CLKEN_GMASTER				BIT(29)
#define USB3CFG_BIGENDIAN_GSLAVE				BIT(28)
#define USB3CFG_HOST_PORT_POWER_CTRL_PRESENT	BIT(27)
#define USB3CFG_HOST_MSI_ENABLE					BIT(26)
#define USB3CFG_HOST_LEGACY_SMI_PCI_CMD_REG_WR	BIT(25)
#define USB3CFG_HOST_LEGACY_SMI_BAR_WR			BIT(24)
#define USB3CFG_FLADJ_30MHZ_REG_MASK(x)			((0x3f & (x)) << 16)
#define USB3CFG_PHY_VAUX_RESET_U2_PORT1			BIT(9)
#define USB3CFG_PHY_VAUX_RESET_U2_PORT0			BIT(8)
#define USB3CFG_XHCI_BUS_MASTER_ENABLE			BIT(4)
#define USB3CFG_BUS_FILTER_BYPASS_MASK(x)		(0xf & (x))

#define USB3CFG_STATUS_OFFSET				0x04
#define USB3CFG_HOST_SYSTEM_ERR					BIT(31)
#define USB3CFG_LEGACY_SMI_INTERRUPT			BIT(16)
#define USB3CFG_HOST_CURRENT_BELT_MASK(x)		(0xfff & (x))

#define USB3CFG_PORT_CONFIG_OFFSET			0x08
#define USB3CFG_U3P1_HOST_DISABLE				BIT(26)
#define USB3CFG_U3P1_HUB_PORT_OVERCURRENT		BIT(25)
#define USB3CFG_U3P1_HUB_PORT_PERM_ATTACH		BIT(24)
#define USB3CFG_U3P0_HOST_DISABLE				BIT(18)
#define USB3CFG_U3P0_HUB_PORT_OVERCURRENT		BIT(17)
#define USB3CFG_U3P0_HUB_PORT_PERM_ATTACH		BIT(16)
#define USB3CFG_U2P1_HOST_DISABLE				BIT(10)
#define USB3CFG_U2P1_HUB_PORT_OVERCURRENT		BIT(9)
#define USB3CFG_U2P1_HUB_PORT_PERM_ATTACH		BIT(8)
#define USB3CFG_U2P0_HOST_DISABLE				BIT(2)
#define USB3CFG_U2P0_HUB_PORT_OVERCURRENT		BIT(1)
#define USB3CFG_U2P0_HUB_PORT_PERM_ATTACH		BIT(0)

#define USB3CFG_PORT_STATUS_OFFSET			0x0c
#define USB3CFG_U3P1_PIPE3_PHY_MODE_MASK(x)		((0x3 & (x)) << 28)
#define USB3CFG_U3P1_HUB_VBUS_CTRL				BIT(24)
#define USB3CFG_U3P0_PIPE3_PHY_MODE_MASK(x)		((0x3 & (x)) << 20)
#define USB3CFG_U3P0_HUB_VBUS_CTRL				BIT(16)
#define USB3CFG_U2P1_UTMI_FSLS_LOW_POWER		BIT(9)
#define USB3CFG_U2P1_HUB_VBUS_CTRL				BIT(8)
#define USB3CFG_U2P0_UTMI_FSLS_LOW_POWER		BIT(1)
#define USB3CFG_U2P0_HUB_VBUS_CTRL				BIT(0)

#define USB3CFG_PHY_PORT0_CONFIG0_OFFSET	0x10
#define USB3CFG_PHY_PORT1_CONFIG0_OFFSET	0x1c
#define USB3CFG_PHY_PORT_CKBUF_EN					BIT(31)
#define USB3CFG_PHY_PORT_MAC_PHY_PLL_EN_REG			BIT(30)
#define USB3CFG_PHY_PORT_MAC_PHY_RECV_DET_EN_REG	BIT(29)
#define USB3CFG_PHY_PORT_MAC_PHY_RECV_DET_REQ_REG	BIT(28)
#define USB3CFG_PHY_PORT_CLK_MODE_SEL_MASK(x)		(0x3 & (x))

#define USB3CFG_PHY_PORT0_STATUS0_OFFSET	0x14
#define USB3CFG_PHY_PORT1_STATUS0_OFFSET	0x20
#define USB3CFG_PHY_PORT_PHY_MAC_RECV_DET_END		BIT(21)
#define USB3CFG_PHY_PORT_PHY_MAC_RECV_DET_ACK		BIT(20)
#define USB3CFG_PHY_PORT_CLK_RDY					BIT(19)
#define USB3CFG_PHY_PORT_COUNT_NUM_VAL(x)			(0x7ffff & (x))

#define USB3CFG_PHY_PORT0_STATUS1_OFFSET	0x18
#define USB3CFG_PHY_PORT1_STATUS1_OFFSET	0x24
#define USB3CFG_PHY_PORT_DISPARITY_ERR_MASK(x)		((0xff & (x)) << 24)
#define USB3CFG_PHY_PORT_ELASTIC_BUF_UDF_MASK(x)	((0xff & (x)) << 16)
#define USB3CFG_PHY_PORT_ELASTIC_BUF_OVF_MASK(x)	((0xff & (x)) << 8)
#define USB3CFG_PHY_PORT_DECODE_ERR_MASK(x)			(0xff & (x))

#define USB3CFG_EXT_CTRL_OFFSET				0x00
#define USB3CFG_RX50_TERM_SEL				BIT(28)

const u8 u3_phy_addrA[16] = {0x01, 0x04, 0x05, 0x08, 0x09, 0x0a, 0x0b, 0x0d, 0x0e, 0x20, 0x21, 0x22, 0x23, 0x27, 0x2b, 0x33};
const u16 u3_phy_dataA[16] = {0xaccc, 0x7000, 0x6313, 0x30d1, 0x424c, 0xa610, 0xb905, 0xef2e, 0x1000, 0x705e, 0xf8e8, 0x0003, 0x0b66, 0x0241, 0x0038, 0x0001};
const u8 u2_phy_addrA[10] = {0xE0, 0xE2, 0xE4, 0xE7, 0xF4, 0xE7, 0xF4, 0xE0, 0xE0, 0xE0};
const u8 u2_phy_dataA[10] = {0x1F, 0xF9, 0x68, 0x71, 0xDB, 0x44, 0xBB, 0x25, 0x21, 0x25};

struct xhci_dwc3_platdata {
	struct phy *usb_phys;
	int num_phys;
};

void dwc3_set_mode(struct dwc3 *dwc3_reg, u32 mode)
{
	clrsetbits_le32(&dwc3_reg->g_ctl,
			DWC3_GCTL_PRTCAPDIR(DWC3_GCTL_PRTCAP_OTG),
			DWC3_GCTL_PRTCAPDIR(mode));
}

void dwc3_phy_reset(struct dwc3 *dwc3_reg)
{
	/* Assert USB3 PHY reset */
	setbits_le32(&dwc3_reg->g_usb3pipectl[0], DWC3_GUSB3PIPECTL_PHYSOFTRST);

	/* Assert USB2 PHY reset */
	setbits_le32(&dwc3_reg->g_usb2phycfg, DWC3_GUSB2PHYCFG_PHYSOFTRST);

	mdelay(100);

	/* Clear USB3 PHY reset */
	clrbits_le32(&dwc3_reg->g_usb3pipectl[0], DWC3_GUSB3PIPECTL_PHYSOFTRST);

	/* Clear USB2 PHY reset */
	clrbits_le32(&dwc3_reg->g_usb2phycfg, DWC3_GUSB2PHYCFG_PHYSOFTRST);
}

void dwc3_core_soft_reset(struct dwc3 *dwc3_reg)
{
	/* Before Resetting PHY, put Core in Reset */
	setbits_le32(&dwc3_reg->g_ctl, DWC3_GCTL_CORESOFTRESET);

	/* reset USB3 phy - if required */
	dwc3_phy_reset(dwc3_reg);

	mdelay(100);

	/* After PHYs are stable we can take Core out of reset state */
	clrbits_le32(&dwc3_reg->g_ctl, DWC3_GCTL_CORESOFTRESET);
}

int dwc3_core_init(struct dwc3 *dwc3_reg)
{
	u32 reg;
	u32 revision;
	unsigned int dwc3_hwparams1;

	revision = readl(&dwc3_reg->g_snpsid);
	/* This should read as U3 followed by revision number */
	if ((revision & DWC3_GSNPSID_MASK) != 0x55330000) {
		puts("this is not a DesignWare USB3 DRD Core\n");
		return -1;
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
		debug("No power optimization available\n");
	}

	/*
	 * WORKAROUND: DWC3 revisions <1.90a have a bug
	 * where the device can fail to connect at SuperSpeed
	 * and falls back to high-speed mode which causes
	 * the device to enter a Connect/Disconnect loop
	 */
	if ((revision & DWC3_REVISION_MASK) < 0x190a)
		reg |= DWC3_GCTL_U2RSTECN;

	writel(reg, &dwc3_reg->g_ctl);

	return 0;
}

static int xhci_rtk_remove(struct udevice *dev)
{
	struct xhci_dwc3_platdata *plat = dev_get_platdata(dev);

	dwc3_shutdown_phy(dev, plat->usb_phys, plat->num_phys);

	return xhci_deregister(dev);
}

static void taurus_usb3_reset(void)
{
	u32 reg_val = 0;

	reg_val = readl(PHY_CONTROL);
	reg_val |= (SYS_S2_USB3_SEL | S2_PCIE_USB3_SEL | S3_PCIE_USB3_SEL);
	writel(reg_val, PHY_CONTROL);

	reg_val = readl(PHY_ISO_POWER_CTRL);
	reg_val |= S2_POWER_USB3;
	writel(reg_val, PHY_ISO_POWER_CTRL);
	mdelay(100);

	reg_val = readl(USB3CFG_PHY + USB3CFG_CNTRL_OFFSET);
	reg_val |= (USB3CFG_PHY_VAUX_RESET | USB3CFG_PHY_VAUX_RESET_U2_PORT0 | USB3CFG_PHY_VAUX_RESET_U2_PORT1);
	writel(reg_val, USB3CFG_PHY + USB3CFG_CNTRL_OFFSET);
}

static void taurus_usb2_reset(void)
{
	u32 reg_val = 0;

	reg_val = readl(BLK_RESET);

	writel(reg_val | RESET_USB, BLK_RESET);
	mdelay(20);
	writel(reg_val & ~RESET_USB, BLK_RESET);
	mdelay(20);
	reg_val = readl(BLK_RESET);

	//deassert port1 block reset if it's asserted
	reg_val = readl(BLK_RESET_EXT);
	if(reg_val & RESET_USB2_PORT_1)
		writel(reg_val & ~RESET_USB2_PORT_1, BLK_RESET_EXT);

	//Pull-down resistor connected to DP
	//Pull-down resistor connected to DM
	reg_val = readl(USB2CFG_PHY + USB2CFG_PHY_PORT0_CONFIG_OFFSET);
	reg_val |= 0x00080064;
	writel(reg_val, USB2CFG_PHY + USB2CFG_PHY_PORT0_CONFIG_OFFSET);

	//Pull-down resistor connected to DP
	//Pull-down resistor connected to DM
	reg_val = readl(USB2CFG_PHY + USB2CFG_PHY_PORT1_CONFIG_OFFSET);
	reg_val |= 0x00080064;
	writel(reg_val, USB2CFG_PHY + USB2CFG_PHY_PORT1_CONFIG_OFFSET);
}

static void taurus_usb3_rx_term_setup(void)
{
	u32 reg_val = 0;

	reg_val = readl(USB3EXT_REG + USB3CFG_EXT_CTRL_OFFSET);
	reg_val &= ~USB3CFG_RX50_TERM_SEL;
	writel(reg_val, USB3EXT_REG + USB3CFG_EXT_CTRL_OFFSET);
}

static void taurus_usb2_rx_term_setup(void)
{
	u32 reg_val = 0;

	reg_val = readl(USB2CFG_PHY + USB2CFG_PHY_PORT0_FORCE_MODE_OFFSET);
	reg_val &= ~USB2CFG_PHY_PORT0_RX_TERM_SEL;
	writel(reg_val, USB2CFG_PHY + USB2CFG_PHY_PORT0_FORCE_MODE_OFFSET);

	reg_val = readl(USB2CFG_PHY + USB2CFG_PHY_PORT1_FORCE_MODE_OFFSET);
	reg_val &= ~USB2CFG_PHY_PORT1_RX_TERM_SEL;
	writel(reg_val, USB2CFG_PHY + USB2CFG_PHY_PORT1_FORCE_MODE_OFFSET);
}

static void taurus_usb2_phy_port_cfg(u8 vstat_in_mask, u8 vctrl_mask, int port_id)
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
	else {
		dev_info(taurus_phy->dev, "The usb2phy port id is invalid.\n");
		return;
	}

	writel(reg_val, USB2CFG_PHY + offset);

	reg_val &= ~USB2CFG_PHY_PORT_VLOADM;
	writel(reg_val, USB2CFG_PHY + offset);

	reg_val |= USB2CFG_PHY_PORT_VLOADM;
	writel(reg_val, USB2CFG_PHY + offset);

	/* Clear VCONTROL field before refilling */
	reg_val &= ~USB2CFG_PHY_PORT_VCNTRL_MASK(0xF);
	reg_val |= USB2CFG_PHY_PORT_VCNTRL_MASK(vctrl_mask2);
	writel(reg_val, USB2CFG_PHY + offset);

	reg_val &= ~USB2CFG_PHY_PORT_VLOADM;
	writel(reg_val, USB2CFG_PHY + offset);

	reg_val |= USB2CFG_PHY_PORT_VLOADM;
	writel(reg_val, USB2CFG_PHY + offset);
}

static inline u32 taurus_usb3_phy_read(u8 addr)
{
	u32 data = 0;
	u32 offset = addr << 2;

	data = readl(SERDES_PHY + offset);

	return data;
}

static inline void taurus_usb3_phy_write(u8 addr, u16 data)
{
	u32 offset = addr << 2;

	writel(data, SERDES_PHY + offset);
}

static int xhci_rtk_probe(struct udevice *dev)
{
	struct xhci_hcor *hcor;
	struct xhci_hccr *hccr;
	struct dwc3 *dwc3_reg;
	enum usb_dr_mode dr_mode;
	struct xhci_dwc3_platdata *plat = dev_get_platdata(dev);
	const char *phy;
	u32 reg;
	void __iomem *phy_reg;
	u32 reg_val = 0;
	int ret;
	int index, port_id;

	taurus_usb2_reset();
	mdelay(100);
	taurus_usb3_reset();

	for (index = 0; index < sizeof(u3_phy_addrA); index++) {
		taurus_usb3_phy_write(*(u3_phy_addrA + index), *(u3_phy_dataA + index));
		taurus_usb3_phy_read(*(u3_phy_addrA+index));
	}
	taurus_usb3_rx_term_setup();

	for (port_id = 0; port_id < 2; port_id++) {
		if (1 << port_id & 3) {
			for (index = 0; index < sizeof(u2_phy_addrA); index++) {
				taurus_usb2_phy_port_cfg(*(u2_phy_dataA + index),
				                         *(u2_phy_addrA + index), port_id);
			}
		}
	}
	taurus_usb2_rx_term_setup();

	hccr = (struct xhci_hccr *)((uintptr_t)dev_read_addr(dev));
	hcor = (struct xhci_hcor *)((uintptr_t)hccr +
			HC_LENGTH(xhci_readl(&(hccr)->cr_capbase)));

	dwc3_reg = (struct dwc3 *)((char *)(hccr) + DWC3_REG_OFFSET);
	dwc3_core_init(dwc3_reg);
	/* Set dwc3 usb2 phy config */
	reg = readl(&dwc3_reg->g_usb2phycfg[0]);

	phy = dev_read_string(dev, "phy_type");
	if (phy && strcmp(phy, "utmi_wide") == 0) {
		reg |= DWC3_GUSB2PHYCFG_PHYIF;
		reg &= ~DWC3_GUSB2PHYCFG_USBTRDTIM_MASK;
		reg |= DWC3_GUSB2PHYCFG_USBTRDTIM_16BIT;
	}

	if (dev_read_bool(dev, "snps,dis_enblslpm-quirk"))
		reg &= ~DWC3_GUSB2PHYCFG_ENBLSLPM;

	if (dev_read_bool(dev, "snps,dis-u2-freeclk-exists-quirk"))
		reg &= ~DWC3_GUSB2PHYCFG_U2_FREECLK_EXISTS;

	if (dev_read_bool(dev, "snps,dis_u2_susphy_quirk"))
		reg &= ~DWC3_GUSB2PHYCFG_SUSPHY;

	writel(reg, &dwc3_reg->g_usb2phycfg[0]);

	dr_mode = usb_get_dr_mode(dev_of_offset(dev));
	if (dr_mode == USB_DR_MODE_UNKNOWN)
		/* by default set dual role mode to HOST */
		dr_mode = USB_DR_MODE_HOST;

	dwc3_set_mode(dwc3_reg, dr_mode);

	return xhci_register(dev, hccr, hcor);
}

static const struct udevice_id xhci_usb_ids[] = {
	{ .compatible = "rtk,taurus-xhci3", },
	{ }
};

U_BOOT_DRIVER(xhci_rtk) = {
	.name	= "xhci_rtk",
	.id	= UCLASS_USB,
	.of_match = xhci_usb_ids,
	.probe = xhci_rtk_probe,
	.remove = xhci_rtk_remove,
	.ops = &xhci_usb_ops,
	.priv_auto_alloc_size = sizeof(struct xhci_ctrl),
	.platdata_auto_alloc_size = sizeof(struct xhci_dwc3_platdata),
	.flags = DM_FLAG_ALLOC_PRIV_DMA,
};