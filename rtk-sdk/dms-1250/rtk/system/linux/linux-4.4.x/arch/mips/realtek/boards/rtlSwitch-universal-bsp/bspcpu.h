/*
 * Realtek Semiconductor Corp.
 *
 * bsp/bspcpu.h
 *     Platform cpu and memory header file
 *
 * Copyright (C) 2006-2015 Tony Wu (tonywu@realtek.com)
 */
#ifndef _BSPCPU_H_
#define _BSPCPU_H_

# define cpu_mem_size		(CONFIG_SOC_MEM_SIZE << 20)

/*
 * scache => L2 cache
 * dcache => D cache
 * icache => I cache
 */
#ifndef CONFIG_SOC_OPT_OVERRIDE

#define cpu_icache_size		(64 << 10)
#define cpu_dcache_size		(32 << 10)
#define cpu_scache_size		(256 << 10)
#define cpu_icache_line		32
#define cpu_dcache_line		32
#define cpu_scache_line		32
#define cpu_tlb_entry		64
#define cpu_imem_size		0
#define cpu_dmem_size		0
#define cpu_smem_size		0

#else

# ifndef CONFIG_SOC_OPT_CONFREGS
#  define cpu_icache_size	(CONFIG_SOC_ICACHE_SIZE << 10)
#  define cpu_dcache_size	(CONFIG_SOC_DCACHE_SIZE << 10)
#  define cpu_scache_size	(CONFIG_SOC_SCACHE_SIZE << 10)
#  define cpu_icache_line	(CONFIG_SOC_ICACHE_LINE)
#  define cpu_dcache_line	(CONFIG_SOC_DCACHE_LINE)
#  define cpu_scache_line	(CONFIG_SOC_SCACHE_LINE)
#  define cpu_tlb_entry		(CONFIG_SOC_TLB_ENTRY)
# endif

# define cpu_imem_size		(CONFIG_SOC_IMEM_SIZE << 10)
# define cpu_dmem_size		(CONFIG_SOC_DMEM_SIZE << 10)
# define cpu_smem_size		(CONFIG_SOC_SMEM_SIZE << 10)

#endif

#endif
