/* Based on implementation in "See MIPS Run". */
LFUNC(mips_init_dcache)
							mtc0	zero, CP0_TAGLO
/* clear all tags */
							li		t0, CKSEG0
							li		t1, DCACHE_SIZE
							addu	t1, t0, t1
1:						cacheop(Index_Store_Tag_D, t0)
							addiu	t0, CACHELINE_SIZE
							bne		t0, t1, 1b
/* load from each line (in cached space) */
							li		t0, CKSEG0
2:						lw		zero, 0(t0)
							addiu	t0, CACHELINE_SIZE
							bne		t0, t1, 2b
/* clear all tags */
							li		t0, CKSEG0
1:						cacheop(Index_Store_Tag_D, t0)
							addiu	t0, CACHELINE_SIZE
							bne		t0, t1, 1b
							jr		ra
END(mips_init_dcache)

GFUNC(writeback_invalidate_dcache_all)
							mtc0	zero, CP0_TAGLO
							li		t0, CKSEG0
							li		t2, DCACHE_SIZE
							addu	t1, t0, t2
1:
							cacheop(Index_Writeback_Inv_D, t0)
							addiu	t0, t0, CACHELINE_SIZE
							bne		t0, t1, 1b
							jr		ra
END(writeback_invalidate_dcache_all)
