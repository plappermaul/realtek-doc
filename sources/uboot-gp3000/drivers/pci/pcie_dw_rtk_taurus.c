// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2022 Realtek Semiconductor
 *
 *
 */

#include <common.h>
#include <dm.h>
#include <pci.h>
#include <asm/io.h>
#include <asm-generic/gpio.h>

DECLARE_GLOBAL_DATA_PTR;

/* PCI Config space registers */
#define PCIE_CONFIG_BAR0		0x10
#define PCIE_LINK_STATUS_REG		0x80
#define PCIE_LINK_STATUS_SPEED_OFF	16
#define PCIE_LINK_STATUS_SPEED_MASK	(0xf << PCIE_LINK_STATUS_SPEED_OFF)
#define PCIE_LINK_STATUS_WIDTH_OFF	20
#define PCIE_LINK_STATUS_WIDTH_MASK	(0xf << PCIE_LINK_STATUS_WIDTH_OFF)

/* Resizable bar capability registers */
#define RESIZABLE_BAR_CAP		0x250
#define RESIZABLE_BAR_CTL0		0x254
#define RESIZABLE_BAR_CTL1		0x258

#define PCIE_PORT_LINK_CONTROL          0x710
#define PORT_LINK_DLL_LINK_EN           BIT(5)
#define PORT_LINK_FAST_LINK_MODE        BIT(7)

#define PCIE_LINK_WIDTH_SPEED_CONTROL   0x80C
#define PORT_LOGIC_N_FTS_MASK           GENMASK(7, 0)
#define PORT_LOGIC_SPEED_CHANGE         BIT(17)
#define PORT_LOGIC_LINK_WIDTH_MASK      GENMASK(12, 8)
#define PORT_LOGIC_LINK_WIDTH_1_LANES   (0x1<<8)
#define PORT_LOGIC_LINK_WIDTH_2_LANES   (0x2<<8)
#define PORT_LOGIC_LINK_WIDTH_4_LANES   (0x4<<8)
#define PORT_LOGIC_LINK_WIDTH_8_LANES   (0x8<<8)

/* iATU registers */
#define PCIE_ATU_REGION_INBOUND		(0x1 << 31)
#define PCIE_ATU_REGION_OUTBOUND	(0x0 << 31)
#define PCIE_ATU_REGION_INDEX1		(0x1 << 0)
#define PCIE_ATU_REGION_INDEX0		(0x0 << 0)
#define PCIE_ATU_CR1			0x904
#define PCIE_ATU_TYPE_MEM		(0x0 << 0)
#define PCIE_ATU_TYPE_IO		(0x2 << 0)
#define PCIE_ATU_TYPE_CFG0		(0x4 << 0)
#define PCIE_ATU_TYPE_CFG1		(0x5 << 0)
#define PCIE_ATU_CR2			0x908
#define PCIE_ATU_ENABLE			(0x1 << 31)
#define PCIE_ATU_BAR_MODE_ENABLE	(0x1 << 30)
#define PCIE_ATU_BUS(x)			(((x) & 0xff) << 24)
#define PCIE_ATU_DEV(x)			(((x) & 0x1f) << 19)
#define PCIE_ATU_FUNC(x)		(((x) & 0x7) << 16)

#define PCIE_ATU_UNR_REGION_CTRL1	0x00
#define PCIE_ATU_UNR_REGION_CTRL2	0x04
#define PCIE_ATU_UNR_LOWER_BASE		0x08
#define PCIE_ATU_UNR_UPPER_BASE		0x0C
#define PCIE_ATU_UNR_LOWER_LIMIT	0x10
#define PCIE_ATU_UNR_LOWER_TARGET	0x14
#define PCIE_ATU_UNR_UPPER_TARGET	0x18

#define PCIE_LINK_CAPABILITY		0x7C
#define PCIE_LINK_CTL_2			0xA0
#define TARGET_LINK_SPEED_MASK		0xF
#define LINK_SPEED_GEN_1		0x1
#define LINK_SPEED_GEN_2		0x2

#define PCIE_GEN3_RELATED		0x890
#define GEN3_EQU_DISABLE		(1 << 16)
#define GEN3_ZRXDC_NON_COMP		(1 << 0)

#define PCIE_GEN3_EQU_CTRL		0x8A8
#define GEN3_EQU_EVAL_2MS_DISABLE	(1 << 5)

#define PCIE_ROOT_COMPLEX_MODE_MASK	(0xF << 4)

#define PCIE_LINK_UP_TIMEOUT_MS		100

#define PCIE_GLOBAL_CONTROL		0x8000
#define PCIE_APP_LTSSM_EN		(1 << 2)
#define PCIE_DEVICE_TYPE_OFFSET		(4)
#define PCIE_DEVICE_TYPE_MASK		(0xF)
#define PCIE_DEVICE_TYPE_EP		(0x0) /* Endpoint */
#define PCIE_DEVICE_TYPE_LEP		(0x1) /* Legacy endpoint */
#define PCIE_DEVICE_TYPE_RC		(0x4) /* Root complex */

#define PCIE_GLOBAL_STATUS		0x8008
#define PCIE_GLB_STS_RDLH_LINK_UP	(1 << 1)
#define PCIE_GLB_STS_PHY_LINK_UP	(1 << 9)

#define PCIE_ARCACHE_TRC		0x8050
#define PCIE_AWCACHE_TRC		0x8054
#define ARCACHE_SHAREABLE_CACHEABLE	0x3511
#define AWCACHE_SHAREABLE_CACHEABLE	0x5311

#define PCIE_GLBL_CORE_CONFIG_REG					0x18
#define PCIE_LTSSM_ENABLE							BIT(0)
#define PCIE_LINK_DOWN_RST							BIT(6)

#define PCIE_GLBL_PM_INFO_RESET_VOLT_LOW_PWR_STATUS	0x1C
#define PWR_STATUS_SMLH_LTSSM_STATE_OFFSET			0x4
#define PWR_STATUS_SMLH_LTSSM_STATE_MASK			0x000003F0
#define PWR_STATUS_RDLH_LINK_UP						BIT(18)

#define PCIE_GET_ATU_OUTB_UNR_REG_OFFSET(region) \
		((region) << 9)

struct pcie_dw_taurus {
	void *glb_base;
	void *dbi_base;
	void *cfg_base;
	void *sds_base;
	void *atu_base;
	fdt_size_t dbi_size;
	fdt_size_t cfg_size;
	fdt_size_t atu_size;
	int first_busno;
	int lane;
	unsigned int id;

	/* IO and MEM PCI regions */
	struct pci_region io;
	struct pci_region mem;
};

static int pcie_dw_get_link_speed(const void *regs_base)
{
	return (readl(regs_base + PCIE_LINK_STATUS_REG) &
		PCIE_LINK_STATUS_SPEED_MASK) >> PCIE_LINK_STATUS_SPEED_OFF;
}

static int pcie_dw_get_link_width(const void *regs_base)
{
	return (readl(regs_base + PCIE_LINK_STATUS_REG) &
		PCIE_LINK_STATUS_WIDTH_MASK) >> PCIE_LINK_STATUS_WIDTH_OFF;
}

/**
 * pcie_dw_prog_outbound_atu() - Configure ATU for outbound accesses
 *
 * @pcie: Pointer to the PCI controller state
 * @index: ATU region index
 * @type: ATU accsess type
 * @cpu_addr: the physical address for the translation entry
 * @pci_addr: the pcie bus address for the translation entry
 * @size: the size of the translation entry
 */
static void pcie_dw_prog_outbound_atu(struct pcie_dw_taurus *pcie, int index,
				      int type, u64 cpu_addr, u64 pci_addr,
				      u32 size)
{
	static void *addr;

	addr = pcie->atu_base + PCIE_GET_ATU_OUTB_UNR_REG_OFFSET(index);
	writel(lower_32_bits(cpu_addr), addr + PCIE_ATU_UNR_LOWER_BASE);
	writel(upper_32_bits(cpu_addr), addr + PCIE_ATU_UNR_UPPER_BASE);
	writel(lower_32_bits(cpu_addr + size - 1), addr + PCIE_ATU_UNR_LOWER_LIMIT);
	writel(lower_32_bits(pci_addr), addr + PCIE_ATU_UNR_LOWER_TARGET);
	writel(upper_32_bits(pci_addr), addr + PCIE_ATU_UNR_UPPER_TARGET);
	writel(type, addr + PCIE_ATU_UNR_REGION_CTRL1);
	writel(PCIE_ATU_ENABLE, addr + PCIE_ATU_UNR_REGION_CTRL2);
}

static uintptr_t set_cfg_address(struct pcie_dw_taurus *pcie,
				 pci_dev_t d, uint where)
{
	uintptr_t va_address;
	u32 atu_type;

	/*
	 * Region #0 is used for Outbound CFG space access.
	 * Direction = Outbound
	 * Region Index = 0
	 */

	if (PCI_BUS(d) == (pcie->first_busno + 1))
		/* For local bus, change TLP Type field to 4. */
		atu_type = PCIE_ATU_TYPE_CFG0;
	else
		/* Otherwise, change TLP Type field to 5. */
		atu_type = PCIE_ATU_TYPE_CFG1;

	if ((PCI_BUS(d) == pcie->first_busno) && (PCI_DEV(d) == 0)) {
		/* Accessing root port configuration space. */
		va_address = (uintptr_t)pcie->dbi_base;
	} else {
		u64 target;
		target = PCI_MASK_BUS(d) | (PCI_BUS(d) - pcie->first_busno) << 24 ;
		target |= (PCI_DEV(d) << 19);
		target |= (PCI_FUNC(d) << 16);
		pcie_dw_prog_outbound_atu(pcie, PCIE_ATU_REGION_INDEX1,
					  atu_type, (u64)pcie->cfg_base,
					  target, pcie->cfg_size);
		va_address = (uintptr_t)pcie->cfg_base;
	}

	va_address += where & ~0x3;

	return va_address;
}

static int pcie_dw_addr_valid(pci_dev_t d, int first_busno)
{
	if ((PCI_BUS(d) == first_busno) && (PCI_DEV(d) > 0))
		return 0;
	if ((PCI_BUS(d) == first_busno + 1) && (PCI_DEV(d) > 0))
		return 0;
	return 1;
}

static int pcie_dw_taurus_read_config(struct udevice *bus, pci_dev_t bdf,
				     uint offset, ulong *valuep,
				     enum pci_size_t size)
{
	struct pcie_dw_taurus *pcie = dev_get_priv(bus);
	uintptr_t va_address;
	ulong value;

	if (!pcie_dw_addr_valid(bdf, pcie->first_busno)) {
		debug("- out of range\n");
		*valuep = pci_get_ff(size);
		return 0;
	}

	va_address = set_cfg_address(pcie, bdf, offset);

	value = readl(va_address);

	*valuep = pci_conv_32_to_size(value, offset, size);

	pcie_dw_prog_outbound_atu(pcie, PCIE_ATU_REGION_INDEX0,
				  PCIE_ATU_TYPE_IO, pcie->io.phys_start,
				  pcie->io.bus_start, pcie->io.size);
	return 0;
}

static int pcie_dw_taurus_write_config(struct udevice *bus, pci_dev_t bdf,
				      uint offset, ulong value,
				      enum pci_size_t size)
{
	struct pcie_dw_taurus *pcie = dev_get_priv(bus);
	uintptr_t va_address;
	ulong old;

	debug("PCIE CFG write: (b,d,f)=(%2d,%2d,%2d) ",
	      PCI_BUS(bdf), PCI_DEV(bdf), PCI_FUNC(bdf));
	debug("(addr,val)=(0x%04x, 0x%08lx)\n", offset, value);

	if (!pcie_dw_addr_valid(bdf, pcie->first_busno)) {
		debug("- out of range\n");
		return 0;
	}

	va_address = set_cfg_address(pcie, bdf, offset);

	old = readl(va_address);
	value = pci_conv_size_to_32(old, value, offset, size);
	writel(value, va_address);

	pcie_dw_prog_outbound_atu(pcie, PCIE_ATU_REGION_INDEX0,
				  PCIE_ATU_TYPE_IO, pcie->io.phys_start,
				  pcie->io.bus_start, pcie->io.size);

	return 0;
}

static int pcie_taurus_is_linkup(struct pcie_dw_taurus *pcie) {
	u32 val;

	val = readl(pcie->glb_base + PCIE_GLBL_PM_INFO_RESET_VOLT_LOW_PWR_STATUS);
	val &= PWR_STATUS_RDLH_LINK_UP;

	return (val != 0);
}

static int wait_for_link_up(struct pcie_dw_taurus *pcie)
{
	unsigned long timeout;

	timeout = get_timer(0) + PCIE_LINK_UP_TIMEOUT_MS;
	while (!pcie_taurus_is_linkup(pcie)) {
		if (get_timer(0) > timeout)
			return 0;
	};

	return 1;
}

static int pcie_taurus_establish_link(struct pcie_dw_taurus *pcie) {
	if (pcie_taurus_is_linkup(pcie))
		return 0;

	writel(PCIE_LTSSM_ENABLE, pcie->glb_base + PCIE_GLBL_CORE_CONFIG_REG);

	mdelay(2);
	return 0;
}

struct serdes_param {
	u16 offset;
	u16 value;
};

static const struct serdes_param sds_params[] = {
	{0x04, 0xa855},
	{0x08, 0x60c6},
	{0x18, 0x001f},
	{0x20, 0x3591},
	{0x28, 0xf670},
	{0x2c, 0xa90d},
	{0x34, 0xf71a},
	{0x38, 0x1000},
	{0x74, 0xda1f},
	{0x80, 0xd477},
	{0x88, 0x0023},
	{0x8c, 0x0b66},
	{0x90, 0x4f30},
	{0xa0, 0xf802},
	{0xc0, 0x0055},
	{0xc8, 0xf0f3},
	{0x24, 0x500c},
	{0x24, 0x520c},
	/* S0_0 & S0_1 gen2 for revB */
	{0x104, 0xa84a},
	{0x108, 0x60c6},
	{0x118, 0x001f},
	{0x120, 0x3591},
	{0x128, 0xf670},
	{0x12c, 0xa90d},
	{0x134, 0xf71a},
	{0x138, 0x1000},
	{0x174, 0xda1f},
	{0x180, 0xd479},
	{0x184, 0x55aa},
	{0x188, 0x0023},
	{0x18c, 0x0b66},
	{0x190, 0x4f0c},
	{0x1a0, 0xf802},
	{0x1bc, 0x3900},
	{0x1c0, 0x004a},
	{0x1c8, 0xf0f3},
	{0x124, 0x500c},
	{0x124, 0x520c},
};

static void dbi_write_u32(struct pcie_dw_taurus *pcie, u32 offset, u32 val) {
	writel(val, pcie->dbi_base + offset);
}

static u32 dbi_read_u32(struct pcie_dw_taurus *pcie, u32 offset) {
	return readl(pcie->dbi_base + offset);
}

static void pcie_global_reset(struct pcie_dw_taurus *pcie) {
	GLOBAL_GLOBAL_CONFIG_t global_config;
	GLOBAL_BLOCK_RESET_t gblkreset;
	GLOBAL_DPHY_RESET_t gdphyreset;
	GLOBAL_PHY_CONTROL_t gphyctrl;
	GLOBAL_PHY_ISO_POWER_CTRL_t gphypwr;

	switch (pcie->id) {
	case 0:
		global_config.wrd = readl(GLOBAL_GLOBAL_CONFIG);
		global_config.bf.cfg_pcie_0_clken = 1;
		global_config.bf.ps_pcie0_en = 1;
		writel(global_config.wrd, GLOBAL_GLOBAL_CONFIG);
		mdelay(5);

		gblkreset.wrd = readl(GLOBAL_BLOCK_RESET);
		gblkreset.bf.reset_pcie0 = 1;
		writel(gblkreset.wrd, GLOBAL_BLOCK_RESET);

		gdphyreset.wrd = readl(GLOBAL_DPHY_RESET);
		gdphyreset.bf.reset_dphy_s0_pcie = 1;
		writel(gdphyreset.wrd, GLOBAL_DPHY_RESET);

		gphyctrl.wrd = readl(GLOBAL_PHY_CONTROL);
		gphyctrl.bf.s0_rxaui_mode = 0;
		writel(gphyctrl.wrd, GLOBAL_PHY_CONTROL);

		gphypwr.wrd = readl(GLOBAL_PHY_ISO_POWER_CTRL);
		gphypwr.bf.s0_PCIE2_ISOLATE_0 = 0;
		gphypwr.bf.s0_POW_PCIE2_0 = 1;
		gphypwr.bf.s0_ISO_ANA_B = 1;
		gphypwr.bf.s0_LANE0_CA09PC_EN = 1;
		gphypwr.bf.s0_LANE0_CA33PC_EN = 1;
		if (pcie->lane > 1) {
			gphypwr.bf.s0_PCIE2_ISOLATE_1 = 0;
			gphypwr.bf.s0_POW_PCIE2_1 = 1;
			gphypwr.bf.s0_ISO_ANA_B = 1;
			gphypwr.bf.s0_LANE1_CA09PC_EN = 1;
			gphypwr.bf.s0_LANE1_CA33PC_EN = 1;
		}
		writel(gphypwr.wrd, GLOBAL_PHY_ISO_POWER_CTRL);
		mdelay(20);

		gdphyreset.wrd = readl(GLOBAL_DPHY_RESET);
		gdphyreset.bf.reset_dphy_s0_pcie = 0;
		writel(gdphyreset.wrd, GLOBAL_DPHY_RESET);

		gblkreset.wrd = readl(GLOBAL_BLOCK_RESET);
		gblkreset.bf.reset_pcie0 = 0;
		writel(gblkreset.wrd, GLOBAL_BLOCK_RESET);
		mdelay(20);
		//printf("PHY_STATUS:%x\n", readl(GLOBAL_PHY_STATUS));

		break;
	case 1:
		global_config.wrd = readl(GLOBAL_GLOBAL_CONFIG);
		global_config.bf.cfg_pcie_1_clken = 1;
		global_config.bf.ps_pcie1_en = 1;
		writel(global_config.wrd, GLOBAL_GLOBAL_CONFIG);
		mdelay(5);

		gblkreset.wrd = readl(GLOBAL_BLOCK_RESET);
		gblkreset.bf.reset_pcie1 = 1;
		writel(gblkreset.wrd, GLOBAL_BLOCK_RESET);

		gdphyreset.wrd = readl(GLOBAL_DPHY_RESET);
		gdphyreset.bf.reset_dphy_s1_pcie = 1;
		writel(gdphyreset.wrd, GLOBAL_DPHY_RESET);

		gphypwr.wrd = readl(GLOBAL_PHY_ISO_POWER_CTRL);
		gphypwr.bf.s1_PCIE2_ISOLATE_0 = 0;
		gphypwr.bf.s1_POW_PCIE2_0 = 1;
		gphypwr.bf.s1_ISO_ANA_B = 1;
		gphypwr.bf.s1_LANE0_CA09PC_EN = 1;
		gphypwr.bf.s1_LANE0_CA33PC_EN = 1;
		if (pcie->lane > 1) {
			gphypwr.bf.s1_PCIE2_ISOLATE_1 = 0;
			gphypwr.bf.s1_POW_PCIE2_1 = 1;
			gphypwr.bf.s1_ISO_ANA_B = 1;
			gphypwr.bf.s1_LANE1_CA09PC_EN = 1;
			gphypwr.bf.s1_LANE1_CA33PC_EN = 1;
		}
		writel(gphypwr.wrd, GLOBAL_PHY_ISO_POWER_CTRL);
		mdelay(20);

		gdphyreset.wrd = readl(GLOBAL_DPHY_RESET);
		gdphyreset.bf.reset_dphy_s1_pcie = 0;
		writel(gdphyreset.wrd, GLOBAL_DPHY_RESET);

		gblkreset.wrd = readl(GLOBAL_BLOCK_RESET);
		gblkreset.bf.reset_pcie1 = 0;
		writel(gblkreset.wrd, GLOBAL_BLOCK_RESET);
		mdelay(20);

	case 2:
		global_config.wrd = readl(GLOBAL_GLOBAL_CONFIG);
		global_config.bf.cfg_pcie_2_clken = 1;
		global_config.bf.ps_pcie2_en = 1;
		writel(global_config.wrd, GLOBAL_GLOBAL_CONFIG);
		mdelay(5);

		gblkreset.wrd = readl(GLOBAL_BLOCK_RESET);
		gblkreset.bf.reset_pcie2 = 1;
		writel(gblkreset.wrd, GLOBAL_BLOCK_RESET);

		gdphyreset.wrd = readl(GLOBAL_DPHY_RESET);
		gdphyreset.bf.reset_dphy_s2_pcie = 1;
		writel(gdphyreset.wrd, GLOBAL_DPHY_RESET);

		gphyctrl.wrd = readl(GLOBAL_PHY_CONTROL);
		gphyctrl.bf.s2_pcie_usb3_sgmii_sel = 0;
		gphyctrl.bf.s2_pcie_usb3_sel = 0;
		gphyctrl.bf.sys_pcie2_and_s2_sel = 1;
		writel(gphyctrl.wrd, GLOBAL_PHY_CONTROL);

		gphypwr.wrd = readl(GLOBAL_PHY_ISO_POWER_CTRL);
		gphypwr.bf.s2_CPHY_ISOLATE = 0;
		gphypwr.bf.s2_POW_PCIE2 = 1;
		gphypwr.bf.s2_POW_USB3 = 0;
		gphypwr.bf.s2_ISO_ANA_B = 1;
		gphypwr.bf.s2_CA09PC_EN = 1;
		gphypwr.bf.s2_CA33PC_EN = 1;
		writel(gphypwr.wrd, GLOBAL_PHY_ISO_POWER_CTRL);
		mdelay(20);

		gdphyreset.wrd = readl(GLOBAL_DPHY_RESET);
		gdphyreset.bf.reset_dphy_s2_pcie = 0;
		writel(gdphyreset.wrd, GLOBAL_DPHY_RESET);

		gblkreset.wrd = readl(GLOBAL_BLOCK_RESET);
		gblkreset.bf.reset_pcie2 = 0;
		writel(gblkreset.wrd, GLOBAL_BLOCK_RESET);
		mdelay(20);
	default:
		return;
	};
}

#define PCIE_GLBL_PCIE_CONTR_CFG_START_ADDR			0x40
#define PCIE_GLBL_PCIE_CONTR_CFG_END_ADDR			0x44
#define PCIE_GLBL_PCIE_CONTR_IATU_BASE_ADDR			0x48

static int pcie_dw_taurus_probe(struct udevice *dev)
{
	struct pcie_dw_taurus *pcie = dev_get_priv(dev);
	struct udevice *ctlr = pci_get_controller(dev);
	struct pci_controller *hose = dev_get_uclass_priv(ctlr);
	const struct serdes_param *parm;
	int i, ret;
	u32 tmp;

	debug("Setup PCIE%d lane=%d\n", pcie->id, pcie->lane);

	pcie_global_reset(pcie);

	/* patch serdes */
	for (i=0; i<ARRAY_SIZE(sds_params); i++) {
		parm = &sds_params[i];
		writel(parm->value, pcie->sds_base + parm->offset); // lane0
		writel(parm->value, pcie->sds_base + parm->offset + 0x1000); // lane1
		mdelay(1);
	}

	writel(lower_32_bits((unsigned long)pcie->dbi_base),
		pcie->glb_base + PCIE_GLBL_PCIE_CONTR_CFG_START_ADDR);
	writel(lower_32_bits((unsigned long)(pcie->dbi_base+pcie->dbi_size-1)),
		pcie->glb_base + PCIE_GLBL_PCIE_CONTR_CFG_END_ADDR);
	writel(lower_32_bits((unsigned long)pcie->atu_base),
		pcie->glb_base + PCIE_GLBL_PCIE_CONTR_IATU_BASE_ADDR);

	tmp = dbi_read_u32(pcie, PCIE_PORT_LINK_CONTROL);
	tmp &= ~PORT_LINK_FAST_LINK_MODE;
	tmp |= PORT_LINK_DLL_LINK_EN;
	debug("PCIE_PORT_LINK_CONTROL:%x=>%x\n", dbi_read_u32(pcie, PCIE_PORT_LINK_CONTROL),tmp);
	dbi_write_u32(pcie, PCIE_PORT_LINK_CONTROL, tmp);

	tmp = dbi_read_u32(pcie, PCIE_LINK_WIDTH_SPEED_CONTROL);
	tmp &= ~PORT_LOGIC_LINK_WIDTH_MASK;
	switch (pcie->lane) {
	case 1:
		tmp |= PORT_LOGIC_LINK_WIDTH_1_LANES;
		break;
	case 2:
		tmp |= PORT_LOGIC_LINK_WIDTH_2_LANES;
		break;
	}
	debug("PCIE_LINK_WIDTH_SPEED_CONTROL:%x=>%x\n", dbi_read_u32(pcie, PCIE_LINK_WIDTH_SPEED_CONTROL),tmp);
	dbi_write_u32(pcie, PCIE_LINK_WIDTH_SPEED_CONTROL, tmp);

#ifdef CONFIG_DM_GPIO
	struct gpio_desc reset_gpio;

	ret = gpio_request_by_name(dev, "device-reset", 0, &reset_gpio,
			     GPIOD_IS_OUT);

	if (!ret && dm_gpio_is_valid(&reset_gpio)) {
		dm_gpio_set_value(&reset_gpio, 0); /* assert */
		mdelay(200);
		dm_gpio_set_value(&reset_gpio, 1); /* de-assert */
		mdelay(200);
	}
#else
	debug("PCIE Reset on GPIO support is missing\n");
#endif /* CONFIG_DM_GPIO */

	pcie->first_busno = dev->seq;

	pcie_taurus_establish_link(pcie);

	wait_for_link_up(pcie);

	/* Don't register host if link is down */
	if (!pcie_taurus_is_linkup(pcie)) {
		printf("PCIE-%d: Link down\n", dev->seq);
	} else {
		printf("PCIE-%d: Link up (Gen%d-x%d, Bus%d)\n", dev->seq,
		       pcie_dw_get_link_speed(pcie->dbi_base),
		       pcie_dw_get_link_width(pcie->dbi_base),
		       hose->first_busno);
	}

	/* Store the IO and MEM windows settings for future use by the ATU */
	pcie->io.phys_start = hose->regions[0].phys_start; /* IO base */
	pcie->io.bus_start  = hose->regions[0].bus_start;  /* IO_bus_addr */
	pcie->io.size	    = hose->regions[0].size;	   /* IO size */

	pcie->mem.phys_start = hose->regions[1].phys_start; /* MEM base */
	pcie->mem.bus_start  = hose->regions[1].bus_start;  /* MEM_bus_addr */
	pcie->mem.size	     = hose->regions[1].size;	    /* MEM size */

	pcie_dw_prog_outbound_atu(pcie, PCIE_ATU_REGION_INDEX1,
				  PCIE_ATU_TYPE_MEM, pcie->mem.phys_start,
				  pcie->mem.bus_start, pcie->mem.size);

	writel(0, pcie->dbi_base + PCI_BASE_ADDRESS_0);
	writew(PCI_CLASS_BRIDGE_PCI, pcie->dbi_base + PCI_CLASS_DEVICE);

	return 0;
}

static int pcie_dw_taurus_ofdata_to_platdata(struct udevice *dev)
{
	struct pcie_dw_taurus *pcie = dev_get_priv(dev);

	pcie->glb_base = (void *)devfdt_get_addr_index(dev, 0);
	if ((fdt_addr_t)pcie->glb_base == FDT_ADDR_T_NONE)
		return -EINVAL;

	pcie->dbi_base = (void *)devfdt_get_addr_size_index(dev, 1, &pcie->dbi_size);
	if ((fdt_addr_t)pcie->dbi_base == FDT_ADDR_T_NONE)
		return -EINVAL;

	pcie->cfg_base = (void *)devfdt_get_addr_size_index(dev, 2, &pcie->cfg_size);
	if ((fdt_addr_t)pcie->cfg_base == FDT_ADDR_T_NONE)
		return -EINVAL;

	pcie->atu_base = (void *)devfdt_get_addr_size_index(dev, 3, &pcie->atu_size);
	if ((fdt_addr_t)pcie->atu_base == FDT_ADDR_T_NONE)
		return -EINVAL;

	pcie->sds_base = (void *)devfdt_get_addr_index(dev, 4);
	if ((fdt_addr_t)pcie->sds_base == FDT_ADDR_T_NONE)
		return -EINVAL;

	pcie->lane = dev_read_u32_default(dev, "num-lanes", 1);
	pcie->id   = dev_read_u32_default(dev, "id", 255);
	if (pcie->id > 2)
		return -EINVAL;
	if ((pcie->lane > 2) || ((pcie->id == 2) && (pcie->lane > 1)))
		return -EINVAL;

	return 0;
}

static const struct dm_pci_ops pcie_dw_taurus_ops = {
	.read_config	= pcie_dw_taurus_read_config,
	.write_config	= pcie_dw_taurus_write_config,
};

static const struct udevice_id pcie_dw_taurus_ids[] = {
	{ .compatible = "realtek,taurus-pcie" },
	{ }
};

U_BOOT_DRIVER(pcie_dw_rtk_taurus) = {
	.name			= "pcie_dw_rtk_taurus",
	.id			= UCLASS_PCI,
	.of_match		= pcie_dw_taurus_ids,
	.ops			= &pcie_dw_taurus_ops,
	.ofdata_to_platdata	= pcie_dw_taurus_ofdata_to_platdata,
	.probe			= pcie_dw_taurus_probe,
	.priv_auto_alloc_size	= sizeof(struct pcie_dw_taurus),
};
