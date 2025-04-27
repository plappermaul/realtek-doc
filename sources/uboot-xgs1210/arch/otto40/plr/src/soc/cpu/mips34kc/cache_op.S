#define __ASM__
#include <soc.h>

GSFUNC(invalidate_icache_all)
	mtc0	zero, CP0_ITAGLO
	li	t0, CKSEG0
	li	t2, ICACHE_SIZE
	addu	t1, t0, t2
1:	
	cacheop(Index_Invalidate_I, t0)
	addiu	t0, t0, CACHELINE_SIZE
	bne	t0, t1, 1b
	jr	ra
END(invalidate_icache_all)

GSFUNC(writeback_invalidate_dcache_all)
	mtc0	zero, CP0_DTAGLO
	li	t0, CKSEG0
	li	t2, DCACHE_SIZE
	addu	t1, t0, t2
1:
	cacheop(Index_Writeback_Inv_D, t0)
	addiu	t0, t0, CACHELINE_SIZE
	bne	t0, t1, 1b
	jr	ra
END(writeback_invalidate_dcache_all)

GSFUNC(writeback_invalidate_dcache_range)
	move	t0, a0
	li	t1, CACHELINE_SIZE
	move	t2, a1
wi_dc_r_loop:
	cacheop(Hit_Writeback_Inv_D, t0)
	addu	t0, t1
	bge	t2, t0, wi_dc_r_loop
	j	ra
END(writeback_invalidate_dcache_range)

GSFUNC(invalidate_icache_range)
	move	t0, a0
	li	t1, CACHELINE_SIZE
	move	t2, a1
i_ic_r_loop:
	cacheop(Index_Invalidate_I, t0)
	addu	t0, t1
	bge	t2, t0, i_ic_r_loop
	j	ra
END(invalidate_icache_range)
