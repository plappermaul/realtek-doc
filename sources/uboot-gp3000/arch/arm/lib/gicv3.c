// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright (c) 2020 Realtek Semiconductor Corp.
 * PSP Software Group
 */

#include <common.h>
#include <asm/io.h>
#include <asm/gic.h>
#include <fdtdec.h>
#include <linux/sizes.h>

DECLARE_GLOBAL_DATA_PTR;

/*
 * '0xa0'is used to initialize each interrtupt default priority.
 * This is an arbitrary value in current context.
 * Any value '0x80' to '0xff' will work for both NS and S state.
 * The values of individual interrupt and default has to be chosen
 * carefully if PMR and BPR based nesting and preemption has to be done.
 */
#define GIC_INT_DEF_PRI_X4		0xa0a0a0a0

/* Priority levels 0:255 */
#define GIC_PRI_MASK			0xff

/* Register bit definations */

/* GICD_CTLR Interrupt group definitions */
#define GICD_CTLR_ARE_NS		(1U << 4)
#define GICD_CTLR_ENABLE_G1A		(1U << 1)
#define GICD_CTLR_ENABLE_G1		(1U << 0)

/* GICD_CTLR Register write progress bit */
#define GICD_CTLR_RWP			(1U << 31)

/* GICD_ICFGR */
#define GICD_ICFGR_MASK			((1U << 2) - 1)
#define GICD_ICFGR_TYPE			(1U << 1)

/* GICD_TYPER.ITLinesNumber 0:4 */
#define GICD_TYPER_ITLINESNUM_MASK	0x1f

/* GICR_CTLR */
#define GICR_CTLR_RWP			(1U << 3)

/* GICR_WAKER */
#define GICR_WAKER_PS			(1U << 1)
#define GICR_WAKER_CA			(1U << 2)

/* Register constants */
#define ICC_SRE_ELx_SRE			(1U << 0)
#define ICC_SRE_ELx_DFB			(1U << 1)
#define ICC_SRE_ELx_DIB			(1U << 2)
#define ICC_SRE_EL3_EN			(1U << 3)

/* Core sysreg macros */
#define read_sysreg(reg) ({					\
	uint64_t val;						\
	__asm__ volatile("mrs %0, " __stringify(reg) : "=r" (val));\
	val;							\
})

#define write_sysreg(val, reg) ({				\
	__asm__ volatile("msr " __stringify(reg) ", %0" : : "r" (val));\
})

/* GIC base */
/* Fixme: update from platform specific define or dt */
#define GIC_NUM_CPU_IF			1
/* Fixme: arch support need to provide api/macro in SMP implementation */
#define GET_CPUID			0

uintptr_t gic_dist_base;
uintptr_t gic_rdists[GIC_NUM_CPU_IF];

#define gic_rdist_rd_base()	(gic_rdists[GET_CPUID])
#define gic_rdist_sgi_base()	(gic_rdist_rd_base() + SZ_64K)

#define GIC_DIST_BASE(initid)	((intid < 32) ? gic_rdist_sgi_base() : gic_dist_base)

typedef uintptr_t mem_addr_t;

/*
 * Wait for register write pending
 * TODO: add timed wait
 */
static int gic_wait_rwp(unsigned int intid)
{
	mem_addr_t base;
	u32 rwp_mask;

	if (intid < 32) {
		base = (gic_rdist_rd_base() + GICR_CTLR);
		rwp_mask = GICR_CTLR_RWP;
	} else {
		base = gic_dist_base + GICD_CTLR;
		rwp_mask = GICD_CTLR_RWP;
	}

	while (readl(base) & rwp_mask)
		;

	return 0;
}

void gic_irq_set_priority(unsigned int intid,
			      unsigned int prio, unsigned int flags)
{
	mem_addr_t base = GIC_DIST_BASE(intid);
	u32 mask = BIT(intid % 32);
	u32 shift;
	u32 val;

	/* Disable the interrupt */
	writel(mask, base + GICD_ICENABLERn + (intid / 32) * 4);
	gic_wait_rwp(intid);

	/* PRIORITYR registers provide byte access */
	writeb(prio & GIC_PRI_MASK, base + GICD_IPRIORITYRn + intid);

	/* Interrupt type config */
	shift = (intid % 32) * 2;

	val = readl(base + GICD_ICFGR + (intid / 16) * 4);
	val &= ~(GICD_ICFGR_MASK << shift);
	if (flags & 0x1) {
		val |= (GICD_ICFGR_TYPE << shift);
	}
	writel(val, base + GICD_ICFGR + (intid / 16) * 4);
}

void gic_irq_enable(unsigned int intid)
{
	mem_addr_t base = GIC_DIST_BASE(initid);
	u32 mask = BIT(intid % 32);

	writel(mask, base + GICD_ISENABLERn + (intid / 32) * 4);
}

void gic_irq_disable(unsigned int intid)
{
	mem_addr_t base = GIC_DIST_BASE(initid);
	u32 mask = BIT(intid % 32);

	writel(mask, base + GICD_ICENABLERn + (intid / 32) * 4);
	/* poll to ensure write is complete */
	gic_wait_rwp(intid);
}

int gic_irq_is_enabled(unsigned int intid)
{
	mem_addr_t base = GIC_DIST_BASE(initid);
	u32 mask = BIT(intid % 32);
	u32 val;

	val = readl(base + GICD_ISENABLERn + (intid / 32) * 4);

	return (val & mask) != 0;
}

unsigned int gic_get_active(void)
{
	int intid;

	/* (Pending -> Active / AP) or (AP -> AP) */
	intid = read_sysreg(ICC_IAR1_EL1);

	return intid;
}

void gic_eoi(unsigned int intid)
{
	/* (AP -> Pending) Or (Active -> Inactive) or (AP to AP) nested case */
	write_sysreg(intid, ICC_EOIR1_EL1);
}

/*
 * Wake up GIC redistributor.
 * clear ProcessorSleep and wait till ChildAsleep is cleared.
 * ProcessSleep to be cleared only when ChildAsleep is set
 * Check if redistributor is not powered already.
 */
static void gicv3_rdist_enable(void)
{
	mem_addr_t rbase = gic_rdist_rd_base();

	if (!(readl(rbase + GICR_WAKER) & GICR_WAKER_CA))
		return;

	clrbits_le32(rbase + GICR_WAKER, GICR_WAKER_PS);
	while (readl(rbase + GICR_WAKER) & GICR_WAKER_CA)
		;
}

/*
 * Initialize the cpu interface. This should be called by each core.
 */
static void gicv3_cpuif_init(void)
{
	mem_addr_t rbase = gic_rdist_sgi_base();
	u32 icc_sre;
	u32 intid;

	/* Disable all sgi ppi */
	writel(0xffffffff, rbase + GICR_ICENABLERn);
	/* Any sgi/ppi intid ie. 0-31 will select GICR_CTRL */
	gic_wait_rwp(0);

	/* Clear pending */
	writel(0xffffffff, rbase + GICR_ICPENDRn);

	/* Configure all SGIs/PPIs as G1NS. */
	writel(0xffffffff, rbase + GICR_IGROUPRn);
	writel(0x0, rbase + GICR_IGROUPMODRn);

	/*
	 * Configure default priorities for SGI 0:15 and PPI 0:15.
	 */
	for (intid = 0; intid < 32; intid += 4) {
		writel(GIC_INT_DEF_PRI_X4, rbase + GICR_IPRIORITYRn + intid);
	}

	/* Configure PPIs as level triggered */
	writel(0, rbase + GICR_ICFGR1);

	/* Check if system interface can be enabled. */
	icc_sre = read_sysreg(ICC_SRE_EL1);
	if (!(icc_sre & ICC_SRE_ELx_SRE)) {
		icc_sre = (icc_sre | ICC_SRE_ELx_SRE |
			   ICC_SRE_ELx_DIB | ICC_SRE_ELx_DFB);
		write_sysreg(icc_sre, ICC_SRE_EL1);
		icc_sre = read_sysreg(ICC_SRE_EL1);
	}

	write_sysreg(0xff, ICC_PMR_EL1);

	/* Allow group1 interrupts */
	write_sysreg(1, ICC_IGRPEN1_EL1);

	isb();
}

static void gicv3_dist_init(void)
{
	mem_addr_t base = gic_dist_base;
	unsigned int num_ints;
	unsigned int intid;

	num_ints = readl(base + GICD_TYPER);
	num_ints &= GICD_TYPER_ITLINESNUM_MASK;
	num_ints = (num_ints + 1) << 5;

	/*
	 * Default configuration of all SPIs
	 */
	for (intid = 32; intid < num_ints; intid += 32) {
		/* Disable interrupt */
		writel(0xffffffff, base + GICD_ICENABLERn + intid / 8);
		/* Clear pending */
		writel(0xffffffff, base + GICD_ICPENDRn + intid / 8);
		/* All SPIs are G1NS and owned by uboot */
		writel(0xffffffff, base + GICD_IGROUPRn + intid / 8);
		writel(0, base + GICD_IGROUPMODRn + intid / 8);
	}

	/* wait for rwp on GICD */
	gic_wait_rwp(32);

	/* Configure default priorities for all SPIs. */
	for (intid = 32; intid < num_ints; intid += 4)
		writel(GIC_INT_DEF_PRI_X4, base + GICD_IPRIORITYRn + intid);

	/* Configure all SPIs as active low, level triggered by default */
	for (intid = 32; intid < num_ints; intid += 16)
		writel(0, base + GICD_ICFGR + intid / 4);

	/* enable Group 1 non-secure interrupts */
	writel(GICD_CTLR_ARE_NS | GICD_CTLR_ENABLE_G1A | GICD_CTLR_ENABLE_G1,
	       base + GICD_CTLR);
}

void gic_init(void)
{
#if CONFIG_OF_CONTROL
	int gic, ret;
	struct fdt_resource res[2];

	gic = fdt_path_offset(gd->fdt_blob, "/gic");
	if (gic < 0) {
		debug("%s: Missing /gic node\n", __func__);
		return;
	}

	ret = fdt_get_resource(gd->fdt_blob, gic, "reg", 0, &res[0]);
	if (ret != 0) {
		debug("%s: Unable to decode gic dist base\n", __func__);
		return;
	}

	ret = fdt_get_resource(gd->fdt_blob, gic, "reg", 1, &res[1]);
	if (ret != 0) {
		debug("%s: Unable to decode gic rdist base\n", __func__);
		return;
	}

	gic_dist_base = res[0].start;
	gic_rdists[GET_CPUID] = res[1].start;
#else
	gic_dist_base = GICD_BASE;
	/* Fixme: populate each redistributor */
	gic_rdists[GET_CPUID] = GICR_BASE;
#endif

	gicv3_dist_init();
	gicv3_rdist_enable();
	gicv3_cpuif_init();
}
