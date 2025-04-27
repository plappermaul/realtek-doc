/*
 * Realtek Semiconductor Corp.
 *
 * bsp/bspcpu.h
 *     bsp cpu and memory configuration file
 *
 * Copyright (C) 2006-2012 Tony Wu (tonywu@realtek.com)
 */
#ifndef _BSPCPU_H_
#define _BSPCPU_H_

/*
 * scache => L2 cache
 * dcache => D cache
 * icache => I cache
 */
#ifdef CONFIG_RTK_CACHE_DEFINE
#define cpu_icache_size     (32 << 10)
#define cpu_dcache_size     (32 << 10)
#define cpu_scache_size     0
#define cpu_icache_line     32
#define cpu_dcache_line     32
#define cpu_scache_line     0
#define cpu_tlb_entry       64
#define cpu_mem_size        (32 << 20)

#define cpu_imem_size       0
#define cpu_dmem_size       0
#define cpu_smem_size       0
#endif
#define cpu_tlb_entry              64

/*
 * GCMP Specific definitions
 */
#define GCMP_BASE_ADDR			0x1fbf8000
#define GCMP_ADDRSPACE_SZ		(256 * 1024)
#define GCR_CONFIG_ADDR     0xbfbf8000  // KSEG0 address of the GCR registers
/*
 * GIC Specific definitions
 */
#define GIC_ADDRSPACE_SZ		(128 * 1024)
#define GIC_BASE_ADDR	        0x1ddc0000
#define GIC_BASE_SIZE		    (128 * 1024)

/*
 * CPC Specific definitions
 */
#define CPC_BASE_ADDR      0x1dde0000

#endif /* _BSPCPU_H_ */
