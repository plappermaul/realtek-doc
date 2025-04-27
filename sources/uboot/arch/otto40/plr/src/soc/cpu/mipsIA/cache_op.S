#define __ASM__
#include <soc.h>

#include <cpu/cpu.s>

.set push
.set noreorder

GFUNC(bypass_L2)
	mfc0	a0, CP0_CONFIG2
	li	a1, 1
	ins	a0, a1, 12, 1
	mtc0	a0, CP0_CONFIG2
	jr	ra
	nop
END(bypass_L2)

GFUNC(init_l23)
	mtc0  zero, CP0_TAGLO2
	mtc0  zero, CP0_DATALO2
	mtc0  zero, CP0_DATAHI2
	lui   t0, 0x8000
	li    t1, (0x80000000 + L2CACHE_SIZE - CACHELINE_SIZE)
	_reset_next_l2tag:
	cacheop(Index_Store_Tag_SD, t0)
	bne   t0, t1, _reset_next_l2tag
	addi  t0, CACHELINE_SIZE
	j ra
	nop
END(init_l23)

GFUNC(enable_l23)
	li    t1, GCR_ADDR
	lw    t0, 0x0008(t1) // Read GCR_BASE
	ins   t0, zero, 0, 8 // Disable CM2 CCA override
	sw    t0, 0x0008(t1) // Write GCR_BASE
	j ra
	nop
END(enable_l23)

GFUNC(disable_l23)
	DISABLE_L23
	j ra
	nop
END(disable_l23)

.set pop

GSFUNC(invalidate_icache_all)
	mtc0	zero, CP0_TAGLO
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
	mtc0	zero, CP0_TAGLO, 2
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
