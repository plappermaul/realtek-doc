#include <config.h>
#include <version.h>
#include <asm/regdef.h>
#include <asm/mipsregs.h>
#include <asm/addrspace.h>


#define MFC0_SEL(dst, src, sel)  .word (0x40000000 | ((dst) << 16) | ((src) << 11) | (sel))
#define MTC0_SEL(dst, src, sel)  .word (0x40800000 | ((dst) << 16) | ((src) << 11) | (sel))

/*******************************************************************************
*
* en_wb_buf_merge - Enable RLX5281 write back buffer merge.
*
* RETURNS: N/A
*
*/
	.globl	en_wb_buf_merge
#ifdef CONFIG_NOPIC
        .section ".start", "ax"
        .set nomips16
#else
        .text
#endif
	.ent	en_wb_buf_merge
en_wb_buf_merge:
	/* Turn on write buffer write buffer merge */
	mfc0	t0, $20
	li	t1, 0xFFCFFFFF
	and	t0, t0, t1
	mtc0    t0, $20
	nop
	li      t1, 0x00100000 # bit 20 of COP0 reg 20, select 0
	or	t0, t0, t1
	mtc0    t0, $20
	nop
	jr	ra
	nop
	.end	en_wb_buf_merge

/*******************************************************************************
*
* dis_wb_buf_merge - Disable RLX5281 write back buffer merge.
*
* RETURNS: N/A
*
*/
	.globl	dis_wb_buf_merge
#ifdef CONFIG_NOPIC
        .section ".start", "ax"
        .set nomips16
#else
        .text
#endif
	.ent	dis_wb_buf_merge
dis_wb_buf_merge:
	/* Turn off write buffer merge */
	mfc0	t0, $20
	li	t1, 0xFFCFFFFF
	and	t0, t0, t1
	mtc0    t0, $20
	nop
	li      t1, 0x00200000 # bit 21 of COP0 reg 20, select 0
	or	t0, t0, t1
	mtc0    t0, $20
	nop
	jr	ra
	nop
	.end	dis_wb_buf_merge

/*******************************************************************************
*
* is_wb_buf_merge_en - 
*
* RETURNS: 1 -> Write buffer merge function is enabled.
*          0 -> Write buffer merge function is disabled.
*
*/
	.globl	is_wb_buf_merge_en
#ifdef CONFIG_NOPIC
        .section ".start", "ax"
        .set nomips16
#else
        .text
#endif
	.ent	is_wb_buf_merge_en
is_wb_buf_merge_en:
	mfc0	t0, $20
	li      t1, 0x00100000 # bit 20 of COP0 reg 20, select 0
	and	t0, t0, t1

	li	v0, 0x1	
	bne	zero, t0, 1f
	nop
	move	v0, zero
1:
	jr 	ra
	nop
	.end	is_wb_buf_merge_en

/*******************************************************************************
*
* en_wb_buf - Enable RLX5281 write back buffer.
*
* RETURNS: N/A
*
*/
	.globl	en_wb_buf
#ifdef CONFIG_NOPIC
        .section ".start", "ax"
        .set nomips16
#else
        .text
#endif
	.ent	en_wb_buf
en_wb_buf:
	/* Turn on write buffer */
	mfc0	t0, $20
	li	t1, 0xFF3FFFFF
	and	t0, t0, t1
	mtc0    t0, $20
	nop
	li      t1, 0x00400000 # bit 22 of COP0 reg 20, select 0
	or	t0, t0, t1
	mtc0    t0, $20
	nop
	jr	ra
	nop

	.end	en_wb_buf


/*******************************************************************************
*
* dis_wb_buf - Disable RLX5281 write back buffer.
*
* RETURNS: N/A
*
*/
	.globl	dis_wb_buf
#ifdef CONFIG_NOPIC
        .section ".start", "ax"
        .set nomips16
#else
        .text
#endif
	.ent	dis_wb_buf
dis_wb_buf:
	/* Turn off write buffer */
	mfc0	t0, $20
	li	t1, 0xFF3FFFFF
	and	t0, t0, t1
	mtc0    t0, $20
	nop
	li      t1, 0x00800000 # bit 23 of COP0 reg 20, select 0
	or	t0, t0, t1
	mtc0    t0, $20
	nop
	jr	ra
	nop

	.end	dis_wb_buf

/*******************************************************************************
*
* is_wb_en - 
*
* RETURNS: 1 -> Cache write allocation function is enabled.
*          0 -> Cache write allocation function is disabled.
*
*/
	.globl	is_wb_en
#ifdef CONFIG_NOPIC
        .section ".start", "ax"
        .set nomips16
#else
        .text
#endif
	.ent	is_wb_en
is_wb_en:
	/* Turn on cache Write allocation */
	mfc0	t0, $20
	li      t1, 0x00400000 # bit 22 of COP0 reg 20, select 0
	and	t0, t0, t1

	li	v0, 0x1	
	bne	zero, t0, 1f
	nop
	move	v0, zero
1:
	jr 	ra
	nop
	.end	is_wb_en


/*******************************************************************************
*
* en_cache_wa - Enable cache write allocation.
*
* RETURNS: N/A
*
*/
	.globl	en_cache_wa
#ifdef CONFIG_NOPIC
        .section ".start", "ax"
        .set nomips16
#else
        .text
#endif
	.ent	en_cache_wa
en_cache_wa:
	/* Turn on cache Write allocation */
	mfc0	t0, $20
	li	t1, 0xFFFF7F7F
	and	t0, t0, t1
	mtc0    t0, $20
	nop
	li      t1, 0x00000080 # bit 7 of COP0 reg 20, select 0
	or	t0, t0, t1
	mtc0    t0, $20
	nop
	jr	ra
	nop
	.end	en_cache_wa



/*******************************************************************************
*
* dis_cache_wa - Disable cache write allocation.
*
* RETURNS: N/A
*
*/
	.globl	dis_cache_wa
#ifdef CONFIG_NOPIC
        .section ".start", "ax"
        .set nomips16
#else
        .text
#endif
	.ent	dis_cache_wa
dis_cache_wa:
	/* Turn off cache Write allocation */
	mfc0	t0, $20
	li	t1, 0xFFFF7F7F
	and	t0, t0, t1
	mtc0    t0, $20
	nop
	li      t1, 0x00008000 # bit 15 of COP0 reg 20, select 0
	or	t0, t0, t1
	mtc0    t0, $20
	nop
	jr	ra
	nop
	.end	dis_cache_wa


/*******************************************************************************
*
* is_wa_en - 
*
* RETURNS: 1 -> Cache write allocation function is enabled.
*          0 -> Cache write allocation function is disabled.
*
*/
	.globl	is_wa_en
#ifdef CONFIG_NOPIC
        .section ".start", "ax"
        .set nomips16
#else
        .text
#endif
	.ent	is_wa_en
is_wa_en:
	/* Turn on cache Write allocation */
	mfc0	t0, $20
	nop
	li      t1, 0x00000080 # bit 7 of COP0 reg 20, select 0
	and	t0, t0, t1

	li	v0, 0x1	
	bne	zero, t0, 1f
	nop
	move	v0, zero
1:
	jr 	ra
	nop
	.end	is_wa_en


/*******************************************************************************
* Not supported cause Taroko merge register data too.
* en_uncache_write_merg - Enable uncached write merge.
*
* RETURNS: N/A
*
*/
	.globl	en_uncache_write_merg
#ifdef CONFIG_NOPIC
        .section ".start", "ax"
        .set nomips16
#else
        .text
#endif
	.ent	en_uncache_write_merg
en_uncache_write_merg:
	/* Turn on uncached write merge */
	MTC0_SEL(0, 20, 1)
	nop
	li      t1, 0x00100000 # bit 20 of COP0 reg 20, select 1
	MTC0_SEL(9, 20, 1)
	nop
	jr 	ra
	nop
	.end	en_uncache_write_merg


/*******************************************************************************
*
* dis_uncache_write_merg - Disable uncached write merge.
* Not supported cause Taroko merge register data too.
*
* RETURNS: N/A
*
*/
	.globl	dis_uncache_write_merg
#ifdef CONFIG_NOPIC
        .section ".start", "ax"
        .set nomips16
#else
        .text
#endif
	.ent	dis_uncache_write_merg
dis_uncache_write_merg:
	/* Turn off uncached write merge */
	MTC0_SEL(0, 20, 1)
	nop
	li      t1, 0x00200000 # bit 21 of COP0 reg 20, select 1
	MTC0_SEL(9, 20, 1)
	nop
	jr	ra
	nop
	.end	dis_uncache_write_merg


/*******************************************************************************
*
* dis_bran_predic - Disable branch prediction
* Not supported cause Taroko merge register data too.
*
* RETURNS: N/A
*
*/
	.globl	dis_bran_predic
#ifdef CONFIG_NOPIC
        .section ".start", "ax"
        .set nomips16
#else
        .text
#endif
	.ent	dis_bran_predic
dis_bran_predic:
	mtlxc0 zero, $4
	nop
	jr 	ra
	nop
	.end	dis_bran_predic


/*******************************************************************************
*
* en_bran_predic - Enable branch prediction
*
* RETURNS: N/A
*
*/
	.globl	en_bran_predic
#ifdef CONFIG_NOPIC
        .section ".start", "ax"
        .set nomips16
#else
        .text
#endif
	.ent	en_bran_predic
en_bran_predic:
	li 	t0, 0x38
	mtlxc0	t0, $4
	nop
	jr 	ra
	nop
	.end	en_bran_predic


/*******************************************************************************
*
* is_b_predic_en - 
*
* RETURNS: 1 -> the branch prediction is enabled.
*          0 -> the branch prediction is disabled.
*
*/
	.globl	is_b_predic_en
#ifdef CONFIG_NOPIC
        .section ".start", "ax"
        .set nomips16
#else
        .text
#endif
	.ent	is_b_predic_en
is_b_predic_en:
	li 	t1, 0x38
	mflxc0	t0, $4
	nop
	li	v0, 0x1	
	beq	t1, t0, 1f
	nop
	move	v0, zero
1:
	jr 	ra
	nop
	.end	is_b_predic_en

/*-------------------------------------------------------------------
**
** void lx5281_WriteBpctl(uint32)
**
**-------------------------------------------------------------------
*/
	.globl	lx5281_WriteBpctl
	.ent	lx5281_WriteBpctl
	.set noreorder
lx5281_WriteBpctl:
	MTLXC0	$4,$4
	nop
	nop
	jr		$31	
	nop
	.set reorder
	.end lx5281_WriteBpctl


/*-------------------------------------------------------------------
**
** void lx5281_ReadBpctl()
**
**-------------------------------------------------------------------
*/
	.globl	lx5281_ReadBpctl
	.ent	lx5281_ReadBpctl
	.set noreorder
lx5281_ReadBpctl:
	MFLXC0	$2,$4
	nop
	jr		$31	
	nop
	.set reorder
	.end lx5281_ReadBpctl	

