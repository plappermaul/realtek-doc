
#include <config.h>
#include <asm/asm.h>
#include <asm/regdef.h>
#include <asm/mipsregs.h>
#include <asm/addrspace.h>
#include <asm/cacheops.h>


/*******************************************************************************
*
* mips_cache_reset - low level initialisation of the primary caches
*
* Invalidate D/I-caches and turn off I/DMEM(0,1)
*
* RETURNS: N/A
*
*/

NESTED(mips_cache_reset, 0, ra)	    
	.set	push
	.set	noreorder
	# Set transition 0->1 for DMEM0Off,IMEM0Off,IInval,DInval
    mfc0	t0, $20
	li		t1, 0xFFFFF7DC		
	and		t0, t0, t1
	mtc0	t0, $20
	nor		t1, t1, zero
	mtc0	t1, $20

	# Set transition 0->1 for DMEM1Off,IMEM1Off
	mfc0	t0, $20, 1
	li		t1, 0xFFFFF7DF
	and		t0, t0, t1
	mtc0	t0, $20, 1
	nor		t1, t1, zero
	mtc0	t1, $20, 1
	
	jr		ra
	nop
	.set	pop
    END(mips_cache_reset)

/*******************************************************************************
*
* dcache_status - get cache status
*
* RETURNS: 0 - cache disabled; 1 - cache enabled
*
*/
	.globl	dcache_status
#ifdef CONFIG_NOPIC
        .section ".start", "ax"
        .set nomips16
#else
        .text
#endif
	.ent	dcache_status
dcache_status:

	mfc0	v0, CP0_CONFIG
	andi	v0, v0, 1
	j	ra
	nop
	.end	dcache_status

/*******************************************************************************
*
* dcache_disable - disable cache
*
* RETURNS: N/A
*
*/
	.globl	dcache_disable
#ifdef CONFIG_NOPIC
        .section ".start", "ax"
        .set nomips16
#else
        .text
#endif
	.ent	dcache_disable
dcache_disable:

	mfc0	t0, CP0_CONFIG
	li	t1, -8
	and	t0, t0, t1
	ori	t0, t0, CONF_CM_UNCACHED
	mtc0	t0, CP0_CONFIG
	j	ra

	.end	dcache_disable


/********************************************
 * 
 * Flush and invalidate DCache.
 *
 ********************************************/

	.globl	rlx5281_cache_flush_dcache
	.ent 	rlx5281_cache_flush_dcache
rlx5281_cache_flush_dcache:
        /* write back invalidate dcache */
        mfc0    t0, $20
	nop
        li      t1, 0xFFFFFDFF
        and     t0, t0, t1
        mtc0    t0, $20
        nop
        li      t1, 0x00000200 # bit 9 of COP0 reg 20, select 0
        or      t0, t0, t1
        mtc0    t0, $20
        nop
        jr      ra
        nop
.end rlx5281_cache_flush_dcache


/********************************************
 * 
 * invalidate ICache.
 *
 ********************************************/

	.globl	rlx5281_cache_invali_icache
	.ent 	rlx5281_cache_invali_icache
rlx5281_cache_invali_icache:
        /* Invalidate icache */
        mfc0    t0, $20
	nop
        li      t1, 0xFFFFFFFD
        and     t0, t0, t1
        mtc0    t0, $20
        nop
        li      t1, 0x00000002 # bit 1 of COP0 reg 20, select 0
        or      t0, t0, t1
        mtc0    t0, $20
        nop
        jr      ra
        nop
.end rlx5281_cache_invali_icache

