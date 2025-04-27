#include "regdef.h"
#include "mipsregs.h"
#include "addrspace.h"
#include "cacheops.h"

#define CFG_ICACHE_SIZE     (0x4000)
#define CFG_DCACHE_SIZE     (0x4000)
#define MIPS_MAX_CACHE_SIZE (0x4000)
#define CFG_CACHELINE_SIZE  (0x10)


/*
 *
 */
#define CACHE_OP( code, type )					( ((code) << 2) | (type) )

#define ICACHE_INDEX_INVALIDATE					CACHE_OP(0x0, 0)
#define ICACHE_INDEX_LOAD_TAG					CACHE_OP(0x1, 0)
#define ICACHE_INDEX_STORE_TAG					CACHE_OP(0x2, 0)
#define DCACHE_INDEX_WRITEBACK_INVALIDATE		CACHE_OP(0x0, 1)
#define DCACHE_INDEX_LOAD_TAG					CACHE_OP(0x1, 1)
#define DCACHE_INDEX_STORE_TAG					CACHE_OP(0x2, 1)
#define SCACHE_INDEX_WRITEBACK_INVALIDATE		CACHE_OP(0x0, 3)
#define SCACHE_INDEX_STORE_TAG					CACHE_OP(0x2, 3)

#define ICACHE_ADDR_HIT_INVALIDATE				CACHE_OP(0x4, 0)
#define ICACHE_ADDR_FILL						CACHE_OP(0x5, 0)
#define ICACHE_ADDR_FETCH_LOCK					CACHE_OP(0x7, 0)
#define DCACHE_ADDR_HIT_INVALIDATE				CACHE_OP(0x4, 1)
#define DCACHE_ADDR_HIT_WRITEBACK_INVALIDATE	CACHE_OP(0x5, 1)
#define DCACHE_ADDR_HIT_WRITEBACK				CACHE_OP(0x6, 1)
#define DCACHE_ADDR_FETCH_LOCK					CACHE_OP(0x7, 1)

#define SCACHE_ADDR_HIT_WRITEBACK_INVALIDATE	CACHE_OP(0x5, 3)

	
/*******************************************************************************
*
* mips_dcache_flush_all - flush dcache data
*
* RETURNS: N/A
*
*/

	.globl	mips_dcache_flush_all
	.text
	.ent	mips_dcache_flush_all
mips_dcache_flush_all:

	li	t2, CFG_DCACHE_SIZE
	li	t4, CFG_CACHELINE_SIZE

	li	a0, KSEG0
	move	a2, t2		# dcacheSize
	move	a3, t4		# dcacheLineSize
	move	a1, a2
	icacheop(a0,a1,a2,a3,Index_Writeback_Inv_D)

	j	ra

	.end	mips_dcache_flush_all

/********************************************************************************/
	.globl	mips_dcache_writeback
	.text
	.ent	mips_dcache_writeback
mips_dcache_writeback:

	cache	Hit_Writeback_D,0(a0)
	j	ra
	.end	mips_dcache_writeback	
	
/********************************************************************************/
	.globl	mips_dcache_invalidate
	.text
	.ent	mips_dcache_invalidate
mips_dcache_invalidate:
2:
	cache	Hit_Invalidate_D,0(a0)
	addu	a0, 16
	bltu	a0, a1, 2b
	j	ra
	.end	mips_dcache_invalidate
	

/********************************************************************************/
	.globl	mips_dcache_lock_8KB
	.text
	.ent	mips_dcache_lock_8KB
mips_dcache_lock_8KB:
	li	a0,0x9f020000
	li	a1,0x9f022000
2:
	cache	DCACHE_ADDR_FETCH_LOCK,0(a0)
	addu	a0, 16
	bltu	a0, a1, 2b
	j	ra
	.end	mips_dcache_lock_8KB

/********************************************************************************/
	.globl	mips_dcache_lock_16KB
	.text
	.ent	mips_dcache_lock_16KB
mips_dcache_lock_16KB:
	li	a0,0x9f020000
	li	a1,0x9f024000
2:
	cache	DCACHE_ADDR_FETCH_LOCK,0(a0)
	addu	a0, 16
	bltu	a0, a1, 2b
	j	ra
	.end	mips_dcache_lock_16KB


/********************************************************************************/
	.globl	mips_invalidate_aline
	.text
	.ent	mips_invalidate_aline
mips_invalidate_aline:
	cache	Hit_Invalidate_D,0(a0)
	j	ra
	.end	mips_invalidate_aline
