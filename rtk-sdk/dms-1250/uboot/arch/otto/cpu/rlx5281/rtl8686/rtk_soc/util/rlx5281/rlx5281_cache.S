#include <asm/regdef.h>

#define MFC0_SEL(dst, src, sel)  .word (0x40000000 | ((dst) << 16) | ((src) << 11) | (sel))
#define MTC0_SEL(dst, src, sel)  .word (0x40800000 | ((dst) << 16) | ((src) << 11) | (sel))

	.text
        .globl invalidate_cache
	.ent invalidate_cache
invalidate_cache:
	addiu sp, sp, -4
	sw t1, 0(sp)

        mtc0 zero, $20
        la t1, 0x00000003 # bits 1,0 of COP0 reg 20
        mtc0 t1, $20
        nop

	lw t1, 0(sp)
	addiu sp, sp, 4
	jr ra
	nop
	.end invalidate_cache


	.text
        .globl invalidate_icache
	.ent invalidate_icache
invalidate_icache:
	addiu sp, sp, -4
	sw t1, 0(sp)

        mtc0 zero, $20
        la t1, 0x00000002 # bits 1,0 of COP0 reg 20
        mtc0 t1, $20
        nop

	lw t1, 0(sp)
	addiu sp, sp, 4
	jr ra
	nop
	.end invalidate_icache

        .globl write_back_invalidate_dcache
	.ent write_back_invalidate_dcache
write_back_invalidate_dcache:
#if 0
	addiu sp, sp, -4
	sw t1, 0(sp)

        mtc0 zero, $20 #write back dcache
        la t1, 0x00000200 # bits 9 of COP0 reg 20
        mtc0 t1, $20
	nop
	nop

	lw t1, 0(sp)
	addiu sp, sp, 4
	jr ra
	nop
#else
	mfc0 	t8, $20; \
	li      t9, (1<<9);\
	not	t9, t9; \
	and	t8, t8, t9; \
	mtc0 	t8, $20; \
	nop;		 \
	li	t9, (1<<9);\
	or	t8, t8, t9; \
	mtc0 	t8, $20;\
	nop;
	nop;
	li	t8, 0xa0000000
	lw	t9, 0(t8)
	nop
	jr ra
	nop
#endif
	.end write_back_invalidate_dcache

        .globl write_back_dcache
	.ent write_back_dcache
write_back_dcache:
	addiu sp, sp, -4
	sw t1, 0(sp)

        mtc0 zero, $20 #write back dcache
        la t1, 0x00000100 # bits 8 of COP0 reg 20
        mtc0 t1, $20
	nop
	nop

	lw t1, 0(sp)
	addiu sp, sp, 4
	jr ra
	nop
	.end write_back_dcache

        .ent enable_L2Cache
        .globl enable_L2Cache
enable_L2Cache:
        addiu sp, sp, -4
        sw t1, 0(sp)

        //mtc0 zero, $20
        MTC0_SEL(0, 20, 1)
        la t1, 0x01000000 # bit 24 of COP0 reg 20, sel = 1
        //mtc0 t1, $20
        MTC0_SEL(9, 20, 1)
        nop
        nop

        lw t1, 0(sp)
        addiu sp, sp, 4
        jr ra
        nop
        .end enable_L2Cache

        .ent disable_L2Cache
        .globl disable_L2Cache
disable_L2Cache:
        addiu sp, sp, -4
        sw t1, 0(sp)

        //mtc0 zero, $20
        MTC0_SEL(0, 20, 1)
        la t1, 0x02000000 # bit 25 of COP0 reg 20, sel = 1
        //mtc0 t1, $20
        MTC0_SEL(9, 20, 1)
        nop
        nop

        lw t1, 0(sp)
        addiu sp, sp, 4
        jr ra
        nop
        .end disable_L2Cache

