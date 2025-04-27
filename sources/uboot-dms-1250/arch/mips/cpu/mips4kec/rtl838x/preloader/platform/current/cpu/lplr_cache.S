#include <mips-generic.h>
#include <soc.h>

#define	RA		t8

#if (DCACHE_SIZE > ICACHE_SIZE)
#define MIPS_MAX_CACHE_SIZE	DCACHE_SIZE
#else
#define MIPS_MAX_CACHE_SIZE	ICACHE_SIZE
#endif

#define cacheop(cmd, addr_reg) cache cmd, 0(addr_reg)

#define set_zero_64(dst_reg) \
							sw		zero, (0*4 - 64)(dst_reg); \
							sw		zero, (1*4 - 64)(dst_reg); \
							sw		zero, (2*4 - 64)(dst_reg); \
							sw		zero, (3*4 - 64)(dst_reg); \
							sw		zero, (4*4 - 64)(dst_reg); \
							sw		zero, (5*4 - 64)(dst_reg); \
							sw		zero, (6*4 - 64)(dst_reg); \
							sw		zero, (7*4 - 64)(dst_reg); \
							sw		zero, (8*4 - 64)(dst_reg); \
							sw		zero, (9*4 - 64)(dst_reg); \
							sw		zero, (10*4 - 64)(dst_reg); \
							sw		zero, (11*4 - 64)(dst_reg); \
							sw		zero, (12*4 - 64)(dst_reg); \
							sw		zero, (13*4 - 64)(dst_reg); \
							sw		zero, (14*4 - 64)(dst_reg); \
							sw		zero, (15*4 - 64)(dst_reg);

/* Based on implementation in "See MIPS Run". */
LFUNC(mips_init_icache)
							mtc0	zero, CP0_TAGLO
/* clear tag to invalidate */
							li		t0, CKSEG0
							li		t1, ICACHE_SIZE
							addu	t1, t0, t1
1:						cacheop(Index_Store_Tag_I, t0)
							addiu	t0, CACHELINE_SIZE
							bne		t0, t1, 1b
/* fill once, so data field parity is correct */
							li		t0, CKSEG0
2:						cacheop(Fill_I, t0)
							addiu	t0, CACHELINE_SIZE
							bne		t0, t1, 2b
/* invalidate again - prudent but not strictly neccessary */
							li		t0, CKSEG0
1:						cacheop(Index_Store_Tag_I, t0)
							addu	t0, CACHELINE_SIZE
							bne		t0, t1, 1b
							jr		ra
END(mips_init_icache)

/* Based on implementation in "See MIPS Run". */
GFUNC(mips_cache_reset)
							move	RA, ra

							li		v0, MIPS_MAX_CACHE_SIZE

/* clear that much memory starting from zero. */
							li		a0, CKSEG1
							addu	a1, a0, v0
2:						addiu	a0, 64
							set_zero_64(a0)
							bne		a0, a1, 2b

							bal		mips_init_icache

							bal		mips_init_dcache

							jr		RA
END(mips_cache_reset)

GFUNC(invalidate_icache_all)
							mtc0	zero, CP0_TAGLO
							li		t0, CKSEG0
							li		t2, ICACHE_SIZE
							addu	t1, t0, t2
1:						cacheop(Index_Invalidate_I, t0)
							addiu	t0, t0, CACHELINE_SIZE
							bne		t0, t1, 1b
							jr		ra
END(invalidate_icache_all)

#include "lplr_cpu_dep_cache.H"
