#include "mips-generic.h"
#include "addrspace.h"
#include "cacheops.h"

//#include "regdef.h"
//#include "mipsregs.h"

//#define PROCESSOR_ID_RLX5281 (0xdc02)
//
//        .text
//        .globl is_CPU
//        .ent is_CPU
//is_CPU:
//	li	t0, PROCESSOR_ID_RLX5281
//	mfc0	v0, $15
//	nop
//	bne	v0, t0, 1f
//	nop
//	li	v0, 1
//	jr	ra
//	nop
//1:
//	move	v0, zero
//	jr	ra
//	nop
//	.end is_CPU

#if 0
        .text
        .globl tword_write
        .ent tword_write
tword_write:
	lw	t0, 0(a0)
	lw	t1, 4(a0)
	nop
	st	t0, 0(a1)
	jr	ra
	nop
	.end tword_write


        .text
        .globl tword_read
        .ent tword_read
tword_read:
	lt	t0, 0(a0)
	nop
	sw	t0, 0(a1)
	sw	t1, 4(a1)
	jr	ra
	nop
	.end tword_read

        .text
        .globl dis_rlx5281_branch_prediction
        .ent dis_rlx5281_branch_prediction
dis_rlx5281_branch_prediction:
        #Disable branch prediction
#define BPCTL 4
        li     t0, 0xffffffef
        mflxc0 t1, $BPCTL
        nop
        and    t1, t1, t0
        mtlxc0 zero,$BPCTL
        nop
	jr	ra
	nop
	.end dis_rlx5281_branch_prediction

#endif
/********************************************
 *
 * Flush and invalidate DCache.
 *
 ********************************************/

        .globl  rlx5281_cache_flush_dcache
        .ent    rlx5281_cache_flush_dcache
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


/*******************************************************************************
*
* mips_cache_flush_all - flush dcache data
*
* RETURNS: N/A
*
*/

        .globl  mips_cache_flush_all
        .text
#ifdef CONFIG_NOPIC
        .set nomips16
#endif
        .ent    mips_cache_flush_all
mips_cache_flush_all:

        li      t2, 0x8000
        li      t4, 0x20

        li      a0, KSEG0
        move    a2, t2          # dcacheSize
        move    a3, t4          # dcacheLineSize
        move    a1, a2
        icacheop(a0,a1,a2,a3,Index_Writeback_Inv_D)

        j       ra

        .end    mips_cache_flush_all


