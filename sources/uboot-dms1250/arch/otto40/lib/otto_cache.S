#include <asm/mipsregs.h>

#define CKUSEG 0x00000000
#define CKSEG0 0x80000000
#define CKSEG1 0xa0000000
#define CKSEG2 0xc0000000
#define CKSEG3 0xe0000000

#define zero $0
#define AT   $1
#define v0   $2  /* return value */
#define v1   $3
#define a0   $4  /* argument registers */
#define a1   $5
#define a2   $6
#define a3   $7
#define t0   $8  /* caller saved */
#define t1   $9
#define t2   $10
#define t3   $11
#define t4   $12
#define t5   $13
#define t6   $14
#define t7   $15
#define s0   $16 /* callee saved */
#define s1   $17
#define s2   $18
#define s3   $19
#define s4   $20
#define s5   $21
#define s6   $22
#define s7   $23
#define t8   $24 /* caller saved */
#define t9   $25
#define k0   $26
#define k1   $27
#define gp   $28
#define sp   $29
#define s8   $30
#define ra   $31

//#define	RA		t8

#define Index_Invalidate_I    0x00
#define Index_Writeback_Inv_D 0x01
#define Index_Load_Tag_I      0x04
#define Index_Load_Tag_D      0x05
#define Index_Store_Tag_I     0x08
#define Index_Store_Tag_D     0x09
#define Hit_Invalidate_I      0x10
#define Hit_Invalidate_D      0x11
#define Fill_I                0x14
#define Hit_Writeback_Inv_D   0x15

#define cacheop(cmd, addr_reg) cache cmd, 0(addr_reg)

#define CACHELINE_SIZE 0x20
#define ICACHE_SIZE 0x8000
#define DCACHE_SIZE 0x8000

/********************************************
 * 
 * invalidate ICache.
 *
 ********************************************/
	.globl	otto_invalidate_icache_all
	.ent	otto_invalidate_icache_all
otto_invalidate_icache_all:
		mtc0	zero, CP0_TAGLO
		li		t0, CKSEG0
		li		t2, ICACHE_SIZE
		addu	t1, t0, t2
1:		cacheop(Index_Invalidate_I, t0)
		addiu	t0, t0, CACHELINE_SIZE
		bne		t0, t1, 1b
		jr		ra
	.end	otto_invalidate_icache_all


/********************************************
 * 
 * writeback and invalidate DCache.
 *
 ********************************************/
	.globl	otto_writeback_invalidate_dcache_all
	.ent	otto_writeback_invalidate_dcache_all
otto_writeback_invalidate_dcache_all:

		mtc0	zero, CP0_TAGLO, 2
		li		t0, CKSEG0
		li		t2, DCACHE_SIZE
		addu	t1, t0, t2
1:		cacheop(Index_Writeback_Inv_D, t0)
		addiu	t0, t0, CACHELINE_SIZE
		bne		t0, t1, 1b
		jr		ra
	.end	otto_writeback_invalidate_dcache_all

