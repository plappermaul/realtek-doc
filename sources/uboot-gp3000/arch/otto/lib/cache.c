// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2003
 * Wolfgang Denk, DENX Software Engineering, <wd@denx.de>
 */

#include <common.h>
#include <cpu_func.h>
#include <asm/cacheops.h>
#ifdef CONFIG_MIPS_L2_CACHE
#include <asm/cm.h>
#endif
#include <asm/io.h>
#include <asm/mipsregs.h>
#include <asm/system.h>
#include <soc.h>

DECLARE_GLOBAL_DATA_PTR;

void mips_cache_probe(void)
{
#ifdef CONFIG_SYS_CACHE_SIZE_AUTO
	unsigned long conf1, il, dl;

	conf1 = read_c0_config1();

	il = (conf1 & MIPS_CONF1_IL) >> MIPS_CONF1_IL_SHF;
	dl = (conf1 & MIPS_CONF1_DL) >> MIPS_CONF1_DL_SHF;

	gd->arch.l1i_line_size = il ? (2 << il) : 0;
	gd->arch.l1d_line_size = dl ? (2 << dl) : 0;
#endif
}

static inline unsigned long icache_line_size(void)
{
#ifdef CONFIG_SYS_CACHE_SIZE_AUTO
	return gd->arch.l1i_line_size;
#else
	return CONFIG_SYS_ICACHE_LINE_SIZE;
#endif
}

static inline unsigned long dcache_line_size(void)
{
#ifdef CONFIG_SYS_CACHE_SIZE_AUTO
	return gd->arch.l1d_line_size;
#else
	return CONFIG_SYS_DCACHE_LINE_SIZE;
#endif
}

static inline unsigned long scache_line_size(void)
{
#ifdef CONFIG_MIPS_L2_CACHE
	return gd->arch.l2_line_size;
#else
	return CONFIG_SYS_SCACHE_LINE_SIZE;
#endif
}

#define cache_loop(start, end, lsize, ops...) do {			\
	const void *addr = (const void *)(start & ~(lsize - 1));	\
	const void *aend = (const void *)((end - 1) & ~(lsize - 1));	\
	const unsigned int cache_ops[] = { ops };			\
	unsigned int i;							\
									\
	if (!lsize)							\
		break;							\
									\
	for (; addr <= aend; addr += lsize) {				\
		for (i = 0; i < ARRAY_SIZE(cache_ops); i++)		\
			mips_cache(cache_ops[i], addr);			\
	}								\
} while (0)

void flush_cache(ulong start_addr, ulong size)
{
	/* aend will be miscalculated when size is zero, so we return here */
	if (size == 0)
		return;

	/* flush D-cache */
	writeback_invalidate_dcache_range(start_addr, start_addr+size);
	//writeback_invalidate_dcache_all();

	/* flush I-cache */
	invalidate_icache_range(start_addr, start_addr+size);
	//invalidate_icache_all();

	/* ensure cache ops complete before any further memory accesses */
	sync();

	/* ensure the pipeline doesn't contain now-invalid instructions */
	instruction_hazard_barrier();
}

void flush_dcache_range(ulong start_addr, ulong stop)
{

	/* aend will be miscalculated when size is zero, so we return here */
	if (start_addr == stop)
		return;

	writeback_invalidate_dcache_range(start_addr, stop);

	/* ensure cache ops complete before any further memory accesses */
	sync();
}

int dcache_status(void)
{
	unsigned int cca = read_c0_config() & CONF_CM_CMASK;
	return cca != CONF_CM_UNCACHED;
}

void dcache_enable(void)
{
	puts("Not supported!\n");
}

void dcache_disable(void)
{
	/* change CCA to uncached */
	change_c0_config(CONF_CM_CMASK, CONF_CM_UNCACHED);

	/* ensure the pipeline doesn't contain now-invalid instructions */
	instruction_hazard_barrier();
}
