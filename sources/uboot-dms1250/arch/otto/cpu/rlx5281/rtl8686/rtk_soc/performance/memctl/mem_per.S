#include <config.h>
#include <asm/regdef.h>
#include <asm/mipsregs.h>
#include <asm/addrspace.h>
#include <asm/cacheops.h>
#include "memory_performance.h"

#define DELAY_TIME (0x1000)

	.global cp3_read_byte
        .text
        .ent cp3_read_byte
	.set noreorder
cp3_read_byte:
	la t1, 0x0a060108
	move t2, zero
	beq t2, zero, 2f
	nop
1:
	lb t3, 0(a0) /* for active row */
	ctc3 t1, $0
2:
	lb t3, 0(a0) /* 100 lb instructions */
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	beq t2, zero, 1b
	add t2, t2, 1
	ctc3 zero, $0
	nop
	mfc3 v0, $8 #counter0[31:0]
	jr  ra
	nop
	.end cp3_read_byte


	.global cp3_write_byte
        .text
        .ent cp3_write_byte
	.set noreorder
cp3_write_byte:
	la t1, 0x0a060108
	move t2, zero
	beq t2, zero, 2f
	nop
1:
	lb t3, 0(a0) /* for active row */
	ctc3 t1, $0
2:
	sb a0, 0(a0)
	sb a0, 4(a0)
	sb a0, 8(a0)
	sb a0, 12(a0)
	sb a0, 16(a0)
	sb a0, 20(a0)
	sb a0, 24(a0)
	sb a0, 28(a0)
	sb a0, 32(a0)
	sb a0, 36(a0)
	sb a0, 40(a0)
	sb a0, 44(a0)
	sb a0, 48(a0)
	sb a0, 52(a0)
	sb a0, 56(a0)
	sb a0, 60(a0)
	sb a0, 64(a0)
	sb a0, 68(a0)
	sb a0, 72(a0)
	sb a0, 76(a0)
	sb a0, 80(a0)
	sb a0, 84(a0)
	sb a0, 88(a0)
	sb a0, 92(a0)
	sb a0, 96(a0)
	sb a0, 0(a0)
	sb a0, 4(a0)
	sb a0, 8(a0)
	sb a0, 12(a0)
	sb a0, 16(a0)
	sb a0, 20(a0)
	sb a0, 24(a0)
	sb a0, 28(a0)
	sb a0, 32(a0)
	sb a0, 36(a0)
	sb a0, 40(a0)
	sb a0, 44(a0)
	sb a0, 48(a0)
	sb a0, 52(a0)
	sb a0, 56(a0)
	sb a0, 60(a0)
	sb a0, 64(a0)
	sb a0, 68(a0)
	sb a0, 72(a0)
	sb a0, 76(a0)
	sb a0, 80(a0)
	sb a0, 84(a0)
	sb a0, 88(a0)
	sb a0, 92(a0)
	sb a0, 96(a0)
	sb a0, 0(a0)
	sb a0, 4(a0)
	sb a0, 8(a0)
	sb a0, 12(a0)
	sb a0, 16(a0)
	sb a0, 20(a0)
	sb a0, 24(a0)
	sb a0, 28(a0)
	sb a0, 32(a0)
	sb a0, 36(a0)
	sb a0, 40(a0)
	sb a0, 44(a0)
	sb a0, 48(a0)
	sb a0, 52(a0)
	sb a0, 56(a0)
	sb a0, 60(a0)
	sb a0, 64(a0)
	sb a0, 68(a0)
	sb a0, 72(a0)
	sb a0, 76(a0)
	sb a0, 80(a0)
	sb a0, 84(a0)
	sb a0, 88(a0)
	sb a0, 92(a0)
	sb a0, 96(a0)
	sb a0, 0(a0)
	sb a0, 4(a0)
	sb a0, 8(a0)
	sb a0, 12(a0)
	sb a0, 16(a0)
	sb a0, 20(a0)
	sb a0, 24(a0)
	sb a0, 28(a0)
	sb a0, 32(a0)
	sb a0, 36(a0)
	sb a0, 40(a0)
	sb a0, 44(a0)
	sb a0, 48(a0)
	sb a0, 52(a0)
	sb a0, 56(a0)
	sb a0, 60(a0)
	sb a0, 64(a0)
	sb a0, 68(a0)
	sb a0, 72(a0)
	sb a0, 76(a0)
	sb a0, 80(a0)
	sb a0, 84(a0)
	sb a0, 88(a0)
	sb a0, 92(a0)
	sb a0, 96(a0)
	beq t2, zero, 1b
	add t2, t2, 1
	ctc3 zero, $0
	nop
	mfc3 v0, $8 #counter0[31:0]
	jr  ra
	nop
	.end cp3_write_byte

	.global cp3_read_half
        .text
        .ent cp3_read_half
	.set noreorder
cp3_read_half:

	la t1, 0x0a060108
	move t2, zero
	beq t2, zero, 2f
	nop
1:
	lb t3, 0(a0) /* for active row */
	ctc3 t1, $0
2:
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	beq t2, zero, 1b
	add t2, t2, 1
	ctc3 zero, $0
	nop
	mfc3 v0, $8 #counter0[31:0]
	jr  ra
	nop
	.end cp3_read_half

	.global cp3_write_half
        .text
        .ent cp3_write_half
	.set noreorder
cp3_write_half:
	la t1, 0x0a060108
	move t2, zero
	beq t2, zero, 2f
	nop
1:
	lb t3, 0(a0) /* for active row */
	ctc3 t1, $0
2:
	sh a0, 0(a0)
	sh a0, 4(a0)
	sh a0, 8(a0)
	sh a0, 12(a0)
	sh a0, 16(a0)
	sh a0, 20(a0)
	sh a0, 24(a0)
	sh a0, 28(a0)
	sh a0, 32(a0)
	sh a0, 36(a0)
	sh a0, 40(a0)
	sh a0, 44(a0)
	sh a0, 48(a0)
	sh a0, 52(a0)
	sh a0, 56(a0)
	sh a0, 60(a0)
	sh a0, 64(a0)
	sh a0, 68(a0)
	sh a0, 72(a0)
	sh a0, 76(a0)
	sh a0, 80(a0)
	sh a0, 84(a0)
	sh a0, 88(a0)
	sh a0, 92(a0)
	sh a0, 96(a0)
	sh a0, 0(a0)
	sh a0, 4(a0)
	sh a0, 8(a0)
	sh a0, 12(a0)
	sh a0, 16(a0)
	sh a0, 20(a0)
	sh a0, 24(a0)
	sh a0, 28(a0)
	sh a0, 32(a0)
	sh a0, 36(a0)
	sh a0, 40(a0)
	sh a0, 44(a0)
	sh a0, 48(a0)
	sh a0, 52(a0)
	sh a0, 56(a0)
	sh a0, 60(a0)
	sh a0, 64(a0)
	sh a0, 68(a0)
	sh a0, 72(a0)
	sh a0, 76(a0)
	sh a0, 80(a0)
	sh a0, 84(a0)
	sh a0, 88(a0)
	sh a0, 92(a0)
	sh a0, 96(a0)
	sh a0, 0(a0)
	sh a0, 4(a0)
	sh a0, 8(a0)
	sh a0, 12(a0)
	sh a0, 16(a0)
	sh a0, 20(a0)
	sh a0, 24(a0)
	sh a0, 28(a0)
	sh a0, 32(a0)
	sh a0, 36(a0)
	sh a0, 40(a0)
	sh a0, 44(a0)
	sh a0, 48(a0)
	sh a0, 52(a0)
	sh a0, 56(a0)
	sh a0, 60(a0)
	sh a0, 64(a0)
	sh a0, 68(a0)
	sh a0, 72(a0)
	sh a0, 76(a0)
	sh a0, 80(a0)
	sh a0, 84(a0)
	sh a0, 88(a0)
	sh a0, 92(a0)
	sh a0, 96(a0)
	sh a0, 0(a0)
	sh a0, 4(a0)
	sh a0, 8(a0)
	sh a0, 12(a0)
	sh a0, 16(a0)
	sh a0, 20(a0)
	sh a0, 24(a0)
	sh a0, 28(a0)
	sh a0, 32(a0)
	sh a0, 36(a0)
	sh a0, 40(a0)
	sh a0, 44(a0)
	sh a0, 48(a0)
	sh a0, 52(a0)
	sh a0, 56(a0)
	sh a0, 60(a0)
	sh a0, 64(a0)
	sh a0, 68(a0)
	sh a0, 72(a0)
	sh a0, 76(a0)
	sh a0, 80(a0)
	sh a0, 84(a0)
	sh a0, 88(a0)
	sh a0, 92(a0)
	sh a0, 96(a0)
	beq t2, zero, 1b
	add t2, t2, 1
	ctc3 zero, $0
	nop
	mfc3 v0, $8 #counter0[31:0]
	jr  ra
	nop
	.end cp3_write_half


	.global read_byte
        .text
        .ent read_byte
	.set noreorder
read_byte:
	li t4, 0xb8003108
	li t0, 0xb8003110
	li t1, 0xC0000000
	move t2, zero
	beq t2, zero, 2f
	nop
1:
	lb t3, 0(a0) /* for active row */
	sw t1, 0(t0)
2:
	lb t3, 0(a0) /* 100 lb instructions */
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)
	lb t3, 0(a0)

	lw  v0, 0(t4)
	beq t2, zero, 1b
	add t2, t2, 1
	srl v0, v0, 5
	jr  ra
	nop
	.end read_byte


	.global write_byte
        .text
        .ent write_byte
	.set noreorder
write_byte:
	li t4, 0xb8003108
	li t0, 0xb8003110
	li t1, 0xC0000000
	move t2, zero

	beq t2, zero, 2f
	nop
1:
	lb t3, 0(a0) /* for active row */
	sw t1, 0(t0)
2:
	sb a0, 0(a0)
	sb a0, 4(a0)
	sb a0, 8(a0)
	sb a0, 12(a0)
	sb a0, 16(a0)
	sb a0, 20(a0)
	sb a0, 24(a0)
	sb a0, 28(a0)
	sb a0, 32(a0)
	sb a0, 36(a0)
	sb a0, 40(a0)
	sb a0, 44(a0)
	sb a0, 48(a0)
	sb a0, 52(a0)
	sb a0, 56(a0)
	sb a0, 60(a0)
	sb a0, 64(a0)
	sb a0, 68(a0)
	sb a0, 72(a0)
	sb a0, 76(a0)
	sb a0, 80(a0)
	sb a0, 84(a0)
	sb a0, 88(a0)
	sb a0, 92(a0)
	sb a0, 96(a0)
	sb a0, 0(a0)
	sb a0, 4(a0)
	sb a0, 8(a0)
	sb a0, 12(a0)
	sb a0, 16(a0)
	sb a0, 20(a0)
	sb a0, 24(a0)
	sb a0, 28(a0)
	sb a0, 32(a0)
	sb a0, 36(a0)
	sb a0, 40(a0)
	sb a0, 44(a0)
	sb a0, 48(a0)
	sb a0, 52(a0)
	sb a0, 56(a0)
	sb a0, 60(a0)
	sb a0, 64(a0)
	sb a0, 68(a0)
	sb a0, 72(a0)
	sb a0, 76(a0)
	sb a0, 80(a0)
	sb a0, 84(a0)
	sb a0, 88(a0)
	sb a0, 92(a0)
	sb a0, 96(a0)
	sb a0, 0(a0)
	sb a0, 4(a0)
	sb a0, 8(a0)
	sb a0, 12(a0)
	sb a0, 16(a0)
	sb a0, 20(a0)
	sb a0, 24(a0)
	sb a0, 28(a0)
	sb a0, 32(a0)
	sb a0, 36(a0)
	sb a0, 40(a0)
	sb a0, 44(a0)
	sb a0, 48(a0)
	sb a0, 52(a0)
	sb a0, 56(a0)
	sb a0, 60(a0)
	sb a0, 64(a0)
	sb a0, 68(a0)
	sb a0, 72(a0)
	sb a0, 76(a0)
	sb a0, 80(a0)
	sb a0, 84(a0)
	sb a0, 88(a0)
	sb a0, 92(a0)
	sb a0, 96(a0)
	sb a0, 0(a0)
	sb a0, 4(a0)
	sb a0, 8(a0)
	sb a0, 12(a0)
	sb a0, 16(a0)
	sb a0, 20(a0)
	sb a0, 24(a0)
	sb a0, 28(a0)
	sb a0, 32(a0)
	sb a0, 36(a0)
	sb a0, 40(a0)
	sb a0, 44(a0)
	sb a0, 48(a0)
	sb a0, 52(a0)
	sb a0, 56(a0)
	sb a0, 60(a0)
	sb a0, 64(a0)
	sb a0, 68(a0)
	sb a0, 72(a0)
	sb a0, 76(a0)
	sb a0, 80(a0)
	sb a0, 84(a0)
	sb a0, 88(a0)
	sb a0, 92(a0)
	sb a0, 96(a0)
	lw  v0, 0(t4)
	beq t2, zero, 1b
	add t2, t2, 1
	srl v0, v0, 5
	jr  ra
	nop
	.end write_byte


	.global read_half
        .text
        .ent read_half
	.set noreorder
read_half:
	li t4, 0xb8003108
	li t0, 0xb8003110
	li t1, 0xC0000000
	move t2, zero

	beq t2, zero, 2f
	nop
1:
	lh t3, 0(a0) /* for active row */
	sw t1, 0(t0)
2:
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lh t3, 0(a0)
	lw  v0, 0(t4)
	beq t2, zero, 1b
	add t2, t2, 1
	srl v0, v0, 5
	jr  ra
	nop
	.end read_half


	.global write_half
        .text
        .ent write_half
	.set noreorder
write_half:
	li t4, 0xb8003108
	li t0, 0xb8003110
	li t1, 0xC0000000
	move t2, zero

	beq t2, zero, 2f
	nop
1:
	lh t3, 0(a0) /* for active row */
	sw t1, 0(t0)
2:
	sh a0, 0(a0)
	sh a0, 4(a0)
	sh a0, 8(a0)
	sh a0, 12(a0)
	sh a0, 16(a0)
	sh a0, 20(a0)
	sh a0, 24(a0)
	sh a0, 28(a0)
	sh a0, 32(a0)
	sh a0, 36(a0)
	sh a0, 40(a0)
	sh a0, 44(a0)
	sh a0, 48(a0)
	sh a0, 52(a0)
	sh a0, 56(a0)
	sh a0, 60(a0)
	sh a0, 64(a0)
	sh a0, 68(a0)
	sh a0, 72(a0)
	sh a0, 76(a0)
	sh a0, 80(a0)
	sh a0, 84(a0)
	sh a0, 88(a0)
	sh a0, 92(a0)
	sh a0, 96(a0)
	sh a0, 0(a0)
	sh a0, 4(a0)
	sh a0, 8(a0)
	sh a0, 12(a0)
	sh a0, 16(a0)
	sh a0, 20(a0)
	sh a0, 24(a0)
	sh a0, 28(a0)
	sh a0, 32(a0)
	sh a0, 36(a0)
	sh a0, 40(a0)
	sh a0, 44(a0)
	sh a0, 48(a0)
	sh a0, 52(a0)
	sh a0, 56(a0)
	sh a0, 60(a0)
	sh a0, 64(a0)
	sh a0, 68(a0)
	sh a0, 72(a0)
	sh a0, 76(a0)
	sh a0, 80(a0)
	sh a0, 84(a0)
	sh a0, 88(a0)
	sh a0, 92(a0)
	sh a0, 96(a0)
	sh a0, 0(a0)
	sh a0, 4(a0)
	sh a0, 8(a0)
	sh a0, 12(a0)
	sh a0, 16(a0)
	sh a0, 20(a0)
	sh a0, 24(a0)
	sh a0, 28(a0)
	sh a0, 32(a0)
	sh a0, 36(a0)
	sh a0, 40(a0)
	sh a0, 44(a0)
	sh a0, 48(a0)
	sh a0, 52(a0)
	sh a0, 56(a0)
	sh a0, 60(a0)
	sh a0, 64(a0)
	sh a0, 68(a0)
	sh a0, 72(a0)
	sh a0, 76(a0)
	sh a0, 80(a0)
	sh a0, 84(a0)
	sh a0, 88(a0)
	sh a0, 92(a0)
	sh a0, 96(a0)
	sh a0, 0(a0)
	sh a0, 4(a0)
	sh a0, 8(a0)
	sh a0, 12(a0)
	sh a0, 16(a0)
	sh a0, 20(a0)
	sh a0, 24(a0)
	sh a0, 28(a0)
	sh a0, 32(a0)
	sh a0, 36(a0)
	sh a0, 40(a0)
	sh a0, 44(a0)
	sh a0, 48(a0)
	sh a0, 52(a0)
	sh a0, 56(a0)
	sh a0, 60(a0)
	sh a0, 64(a0)
	sh a0, 68(a0)
	sh a0, 72(a0)
	sh a0, 76(a0)
	sh a0, 80(a0)
	sh a0, 84(a0)
	sh a0, 88(a0)
	sh a0, 92(a0)
	sh a0, 96(a0)
	lw  v0, 0(t4)
	beq t2, zero, 1b
	add t2, t2, 1
	srl v0, v0, 5
	jr  ra
	nop
	.end write_half

	.global cp3_read_word
        .text
        .ent cp3_read_word
	.set noreorder
cp3_read_word:
	la t1, 0x0a060108
	move t2, zero
	beq t2, zero, 2f
	nop
1:
	lb t3, 0(a0) /* for active row */
	ctc3 t1, $0
2:
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	beq t2, zero, 1b
	add t2, t2, 1
	ctc3 zero, $0
	nop
	mfc3 v0, $8 #counter0[31:0]
	jr  ra
	nop
	.end cp3_read_word

	.global read_word
        .text
        .ent read_word
	.set noreorder
read_word:
	li t4, 0xb8003108
	li t0, 0xb8003110
	li t1, 0xC0000000
	move t2, zero

	beq t2, zero, 2f
	nop
1:
	lw t3, 0(a0) /* for active row */
	sw t1, 0(t0)
2:
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw t3, 0(a0)
	lw  v0, 0(t4)
	beq t2, zero, 1b
	add t2, t2, 1
	srl v0, v0, 5
	jr  ra
	nop
	.end read_word

	.global cp3_write_word
        .text
        .ent cp3_write_word
	.set noreorder
cp3_write_word:
	la t1, 0x0a060108
	move t2, zero
	beq t2, zero, 2f
	nop
1:
	lb t3, 0(a0) /* for active row */
	ctc3 t1, $0
2:
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	beq t2, zero, 1b
	add t2, t2, 1
	ctc3 zero, $0
	nop
	mfc3 v0, $8 #counter0[31:0]
	jr  ra
	nop
	.end cp3_write_word



	.global write_word
        .text
        .ent write_word
	.set noreorder
write_word:
	li t4, 0xb8003108
	li t0, 0xb8003110
	li t1, 0xC0000000
	move t2, zero

	beq t2, zero, 2f
	nop
1:
	lw t3, 0(a0) /* for active row */
	sw t1, 0(t0)
2:
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	sw a0, 0(a0)
	lw  v0, 0(t4)
	beq t2, zero, 1b
	add t2, t2, 1
	srl v0, v0, 5
	jr  ra
	nop
	.end write_word

	.global cp3_read_tword
        .text
        .ent cp3_read_tword
	.set noreorder
cp3_read_tword:

	la t1, 0x0a060108
	move t2, zero
	beq t2, zero, 2f
	nop
1:
	lb t3, 0(a0) /* for active row */
	ctc3 t1, $0
2:
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	beq t2, zero, 1b
	add t2, t2, 1
	ctc3 zero, $0
	nop
	mfc3 v0, $8 #counter0[31:0]
	jr  ra
	nop
	.end cp3_read_tword



	.global read_tword
        .text
        .ent read_tword
	.set noreorder
read_tword:
	li t4, 0xb8003108
	li t0, 0xb8003110
	li t1, 0xC0000000
	move t2, zero
	beq t2, zero, 2f
	nop
1:
	lw t3, 0(a0) /* for active row */
	sw t1, 0(t0)
2:
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lt t6, 0(a0)
	lw  v0, 0(t4)
	beq t2, zero, 1b
	add t2, t2, 1
	srl v0, v0, 5
	jr  ra
	nop
	.end read_tword


	.global cp3_write_tword
        .text
        .ent cp3_write_tword
	.set noreorder
cp3_write_tword:
	la t1, 0x0a060108
	move t2, zero
	beq t2, zero, 2f
	nop
1:
	lb t3, 0(a0) /* for active row */
	ctc3 t1, $0
2:
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	beq t2, zero, 1b
	add t2, t2, 1
	ctc3 zero, $0
	nop
	mfc3 v0, $8 #counter0[31:0]
	jr  ra
	nop
	.end cp3_write_tword


	.global write_tword
        .text
        .ent write_tword
	.set noreorder
write_tword:
	li t4, 0xb8003108
	li t0, 0xb8003110
	li t1, 0xC0000000
	move t2, zero
	beq t2, zero, 2f
	nop
1:
	lw t3, 0(a0) /* for active row */
	sw t1, 0(t0)
2:
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	st t0, 0(a0)
	lw  v0, 0(t4)
	beq t2, zero, 1b
	add t2, t2, 1
	srl v0, v0, 5
	jr  ra
	nop
	.end write_tword

	/* a0=address, a1=bytes size, a2=cache line size */
	.global refill_line
        .text
        .ent refill_line
	.set noreorder
refill_line:
	#enable CP3
	#load imem range
	#imem refill
	.end refill_line


	/* a0=address, a1=cache bytes size, a2=cache line size */
	.global cp3_read_line
        .text
        .ent cp3_read_line
	.set noreorder
cp3_read_line:
    mtc0 zero, $20 #write back invalidate dcache
    li v0, 0x00000200 # bits 9 of COP0 reg 20
    mtc0 v0, $20
	nop

	li t6, 0xDFFFFFFF
	la t1, 0x0a060108
	move t2, zero
	beq t2, zero, 2f
	nop
1:
	lw   t3, 0(a0) /* for active row */
	and a0, a0, t6
	ctc3 t1, $0
2:
	lw t3, 0(a0)
	lw t3, 32(a0)
	lw t3, 64(a0)
	lw t3, 96(a0)
	lw t3, 128(a0)
	lw t3, 160(a0)
	lw t3, 192(a0)
	lw t3, 224(a0)
	lw t3, 256(a0)
	lw t3, 288(a0)
	lw t3, 320(a0)
	lw t3, 352(a0)
	lw t3, 384(a0)
	lw t3, 416(a0)
	lw t3, 448(a0)
	lw t3, 480(a0)
	lw t3, 512(a0)
	lw t3, 544(a0)
	lw t3, 576(a0)
	lw t3, 608(a0)
	lw t3, 640(a0)
	lw t3, 672(a0)
	lw t3, 704(a0)
	lw t3, 736(a0)
	lw t3, 768(a0)
	lw t3, 800(a0)
	lw t3, 832(a0)
	lw t3, 864(a0)
	lw t3, 896(a0)
	lw t3, 928(a0)
	lw t3, 960(a0)
	lw t3, 992(a0)
	beq t2, zero, 1b
	add t2, t2, 1
	ctc3 zero, $0
	nop
	mfc3 v0, $8 #counter0[31:0]
	jr  ra
	nop
	.end cp3_read_line




	/* a0=address, a1=cache bytes size, a2=cache line size */
	.global read_line
        .text
        .ent read_line
	.set noreorder
read_line:
        mtc0 zero, $20 #write back invalidate dcache
        li v0, 0x00000200 # bits 9 of COP0 reg 20
        mtc0 v0, $20
	nop
	li t4, 0xb8003108
	li t0, 0xb8003110
	li t1, 0xC0000000
	li t6, 0xDFFFFFFF
	move t2, zero
	beq t2, zero, 2f

1:
	lw t3, 0(a0) /*active row*/
	and a0, a0, t6
	sw t1, 0(t0)
	lw t3, 0(t0)
2:
	lw t3, 0(a0)
	lw t3, 32(a0)
	lw t3, 64(a0)
	lw t3, 96(a0)
	lw t3, 128(a0)
	lw t3, 160(a0)
	lw t3, 192(a0)
	lw t3, 224(a0)
	lw t3, 256(a0)
	lw t3, 288(a0)
	lw t3, 320(a0)
	lw t3, 352(a0)
	lw t3, 384(a0)
	lw t3, 416(a0)
	lw t3, 448(a0)
	lw t3, 480(a0)
	lw t3, 512(a0)
	lw t3, 544(a0)
	lw t3, 576(a0)
	lw t3, 608(a0)
	lw t3, 640(a0)
	lw t3, 672(a0)
	lw t3, 704(a0)
	lw t3, 736(a0)
	lw t3, 768(a0)
	lw t3, 800(a0)
	lw t3, 832(a0)
	lw t3, 864(a0)
	lw t3, 896(a0)
	lw t3, 928(a0)
	lw t3, 960(a0)
	lw t3, 992(a0)
	lw  v0, 0(t4)
	beq t2, zero, 1b
	add t2, t2, 1
	srl v0, v0, 5
	jr  ra
	nop
	.end read_line


	.global cp3_flush_cache_test
    .text
    .ent cp3_flush_cache_test
	.set noreorder
cp3_flush_cache_test:
    mtc0 zero, $20 #write back invalidate dcache
    li v0, 0x00000200 # bits 9 of COP0 reg 20
    mtc0 v0, $20
	nop
	la t1, 0x0a060108
	li t2, 0xDFFFFFFF
	and a0, a0, t2
	addu a1, a0, a1 /* a0=addr, a1=cache size */
1:
	lw t3, 0(a0)
	nop
	sw a0, 0(a0)
	nop
	add a0, a0, 4
	bne a0, a1, 1b
	nop

        mtc0 zero, $20 #write back invalidate dcache
        li t3, 0x00000200 # bits 9 of COP0 reg 20
	ctc3 t1, $0
        mtc0 t3, $20
        nop

    ctc3 zero, $0
    nop
    mfc3 v0, $8 #counter0[31:0]
    jr  ra
    nop
	.end cp3_flush_cache_test


	.global flush_cache_test
        .text
        .ent flush_cache_test
	.set noreorder
flush_cache_test:
        mtc0 zero, $20 #write back invalidate dcache
        li v0, 0x00000200 # bits 9 of COP0 reg 20
        mtc0 v0, $20
	nop
	li v0, 0xb8003108
	li t0, 0xb8003110
	li t1, 0xC0000000
	li t2, 0xDFFFFFFF
	and a0, a0, t2
	addu a1, a0, a1 /* a0=addr, a1=cache size */
1:
	lw t3, 0(a0)
	nop
	sw a0, 0(a0)
	nop
	add a0, a0, 4
	bne a0, a1, 1b
	nop

        mtc0 zero, $20 #write back invalidate dcache
        li t3, 0x00000200 # bits 9 of COP0 reg 20
	sw t1, 0(t0)
        mtc0 t3, $20
        nop
	
	lw  v0, 0(v0)
	nop
	srl v0, v0, 5
	jr  ra
	nop
	.end flush_cache_test


#define START_CP3_COUNTER \ 
			ctc3 	a2, $0 ;

#define STOP_CP3_COUNTER \
		        ctc3 zero, $0 ;\
       			nop ;
	/* a0: DMEM Address, a1: DRAM Address, a2: counter mode*/
	.global DMEM_read_TWord_160_UR_I
        .text
        .ent DMEM_read_TWord_160_UR_I
	.set noreorder
	.section ".myTest", "ax"
DMEM_read_TWord_160_UR_I:
        mtc0 zero, $20 #write back invalidate dcache
        la t1, 0x00000200 # bits 8 of COP0 reg 20
        mtc0 t1, $20
	nop

	/*waiting for empty of write buffer*/
	li	t1, 10000
2:
	bne	zero, t1, 2b
	addi	t1, t1, -1

	START_CP3_COUNTER	

	lt	t0, 0(a0)
	st	t0, 0(a1)
	lt	t0, 8(a0)
	st	t0, 8(a1)
	lt	t0, 16(a0)
	st	t0, 16(a1)
	lt	t0, 24(a0)
	st	t0, 24(a1)
	lt	t0, 32(a0)
	st	t0, 32(a1)
	lt	t0, 40(a0)
	st	t0, 40(a1)
	lt	t0, 48(a0)
	st	t0, 48(a1)
	lt	t0, 56(a0)
	st	t0, 56(a1)
	lt	t0, 64(a0)
	st	t0, 64(a1)
	lt	t0, 72(a0)
	st	t0, 72(a1)

	lt	t0, 80(a0)
	st	t0, 80(a1)
	lt	t0, 88(a0)
	st	t0, 88(a1)
	lt	t0, 96(a0)
	st	t0, 96(a1)
	lt	t0, 104(a0)
	st	t0, 104(a1)
	lt	t0, 112(a0)
	st	t0, 112(a1)
	lt	t0, 120(a0)
	st	t0, 120(a1)
	lt	t0, 128(a0)
	st	t0, 128(a1)
	lt	t0, 136(a0)
	st	t0, 136(a1)
	lt	t0, 144(a0)
	st	t0, 144(a1)
	lt	t0, 152(a0)
	st	t0, 152(a1)

	STOP_CP3_COUNTER

	jr	ra
	nop
	.end DMEM_read_TWord_160_UR_I


	/* a0: DMEM Address, a1: DRAM Address, a2: counter mode */
	.global DMEM_read_TWord_160_UR_B
        .text
        .ent DMEM_read_TWord_160_UR_B
	.set noreorder
	.section ".myTest", "ax"
DMEM_read_TWord_160_UR_B:
        mtc0 zero, $20 #write back invalidate dcache
        la t1, 0x00000200 # bits 8 of COP0 reg 20
        mtc0 t1, $20
	nop

	/*waiting for empty of write buffer*/
	li	t1, 10000
2:
	bne	zero, t1, 2b
	addi	t1, t1, -1

	START_CP3_COUNTER

	lt	t0, 0(a0)
	lt	t2, 8(a0)
	lt	t4, 16(a0)
	lt	t6, 24(a0)
	st	t0, 0(a1)
	st	t2, 8(a1)
	st	t4, 16(a1)
	st	t6, 24(a1)

	lt	t0, 32(a0)
	lt	t2, 40(a0)
	lt	t4, 48(a0)
	lt	t6, 56(a0)
	st	t0, 32(a1)
	st	t2, 40(a1)
	st	t4, 48(a1)
	st	t6, 56(a1)

	lt	t0, 64(a0)
	lt	t2, 72(a0)
	lt	t4, 80(a0)
	lt	t6, 88(a0)
	st	t0, 64(a1)
	st	t2, 72(a1)
	st	t4, 80(a1)
	st	t6, 88(a1)

	lt	t0, 96(a0)
	lt	t2, 104(a0)
	lt	t4, 112(a0)
	lt	t6, 120(a0)
	st	t0, 96(a1)
	st	t2, 104(a1)
	st	t4, 112(a1)
	st	t6, 120(a1)

	lt	t0, 128(a0)
	lt	t2, 136(a0)
	lt	t4, 144(a0)
	lt	t6, 152(a0)
	st	t0, 128(a1)
	st	t2, 136(a1)
	st	t4, 144(a1)
	st	t6, 152(a1)

	STOP_CP3_COUNTER

	jr	ra
	nop

	.end DMEM_read_TWord_160_UR_B


	/* a0: DMEM Address, a1: DRAM Address, a2: counter mode */
	.global DMEM_read_TWord_480_UR_I
        .text
        .ent DMEM_read_TWord_480_UR_I
	.set noreorder
	.section ".myTest", "ax"
DMEM_read_TWord_480_UR_I:
        mtc0 zero, $20 #write back invalidate dcache
        la t1, 0x00000200 # bits 8 of COP0 reg 20
        mtc0 t1, $20
	nop

	/*waiting for empty of write buffer*/
	li	t1, 10000
2:
	bne	zero, t1, 2b
	addi	t1, t1, -1


	START_CP3_COUNTER

	lt	t0, 0(a0)
	st	t0, 0(a1)
	lt	t0, 8(a0)
	st	t0, 8(a1)
	lt	t0, 16(a0)
	st	t0, 16(a1)
	lt	t0, 24(a0)
	st	t0, 24(a1)
	lt	t0, 32(a0)
	st	t0, 32(a1)
	lt	t0, 40(a0)
	st	t0, 40(a1)
	lt	t0, 48(a0)
	st	t0, 48(a1)
	lt	t0, 56(a0)
	st	t0, 56(a1)
	lt	t0, 64(a0)
	st	t0, 64(a1)
	lt	t0, 72(a0)
	st	t0, 72(a1)

	lt	t0, 80(a0)
	st	t0, 80(a1)
	lt	t0, 88(a0)
	st	t0, 88(a1)
	lt	t0, 96(a0)
	st	t0, 96(a1)
	lt	t0, 104(a0)
	st	t0, 104(a1)
	lt	t0, 112(a0)
	st	t0, 112(a1)
	lt	t0, 120(a0)
	st	t0, 120(a1)
	lt	t0, 128(a0)
	st	t0, 128(a1)
	lt	t0, 136(a0)
	st	t0, 136(a1)
	lt	t0, 144(a0)
	st	t0, 144(a1)
	lt	t0, 152(a0)
	st	t0, 152(a1)

	lt	t0, 160(a0)
	st	t0, 160(a1)
	lt	t0, 168(a0)
	st	t0, 168(a1)
	lt	t0, 176(a0)
	st	t0, 176(a1)
	lt	t0, 184(a0)
	st	t0, 184(a1)
	lt	t0, 192(a0)
	st	t0, 192(a1)
	lt	t0, 200(a0)
	st	t0, 200(a1)
	lt	t0, 208(a0)
	st	t0, 208(a1)
	lt	t0, 216(a0)
	st	t0, 216(a1)
	lt	t0, 224(a0)
	st	t0, 224(a1)
	lt	t0, 232(a0)
	st	t0, 232(a1)

	lt	t0, 240(a0)
	st	t0, 240(a1)
	lt	t0, 248(a0)
	st	t0, 248(a1)
	lt	t0, 256(a0)
	st	t0, 256(a1)
	lt	t0, 264(a0)
	st	t0, 264(a1)
	lt	t0, 272(a0)
	st	t0, 272(a1)
	lt	t0, 280(a0)
	st	t0, 280(a1)
	lt	t0, 288(a0)
	st	t0, 288(a1)
	lt	t0, 296(a0)
	st	t0, 296(a1)
	lt	t0, 304(a0)
	st	t0, 304(a1)
	lt	t0, 312(a0)
	st	t0, 312(a1)

	lt	t0, 320(a0)
	st	t0, 320(a1)
	lt	t0, 328(a0)
	st	t0, 328(a1)
	lt	t0, 336(a0)
	st	t0, 336(a1)
	lt	t0, 344(a0)
	st	t0, 344(a1)
	lt	t0, 352(a0)
	st	t0, 352(a1)
	lt	t0, 360(a0)
	st	t0, 360(a1)
	lt	t0, 368(a0)
	st	t0, 368(a1)
	lt	t0, 376(a0)
	st	t0, 376(a1)
	lt	t0, 384(a0)
	st	t0, 384(a1)
	lt	t0, 392(a0)
	st	t0, 392(a1)

	lt	t0, 400(a0)
	st	t0, 400(a1)
	lt	t0, 408(a0)
	st	t0, 408(a1)
	lt	t0, 416(a0)
	st	t0, 416(a1)
	lt	t0, 424(a0)
	st	t0, 424(a1)
	lt	t0, 432(a0)
	st	t0, 432(a1)
	lt	t0, 440(a0)
	st	t0, 440(a1)
	lt	t0, 448(a0)
	st	t0, 448(a1)
	lt	t0, 456(a0)
	st	t0, 456(a1)
	lt	t0, 464(a0)
	st	t0, 464(a1)
	lt	t0, 472(a0)
	st	t0, 472(a1)

	STOP_CP3_COUNTER

	jr	ra
	nop

	.end DMEM_read_TWord_480_UR_I


	/* a0: DMEM Address, a1: DRAM Address, a2: counter mode */
	.global DMEM_read_TWord_480_UR_B
        .text
        .ent DMEM_read_TWord_480_UR_B
	.set noreorder
	.section ".myTest", "ax"
DMEM_read_TWord_480_UR_B:
        mtc0 zero, $20 #write back invalidate dcache
        la t1, 0x00000200 # bits 8 of COP0 reg 20
        mtc0 t1, $20
	nop

	/*waiting for empty of write buffer*/
	li	t1, 10000
2:
	bne	zero, t1, 2b
	addi	t1, t1, -1


	START_CP3_COUNTER

	lt	t0, 0(a0)
	lt	t2, 8(a0)
	lt	t4, 16(a0)
	lt	t6, 24(a0)
	st	t0, 0(a1)
	st	t2, 8(a1)
	st	t4, 16(a1)
	st	t6, 24(a1)

	lt	t0, 32(a0)
	lt	t2, 40(a0)
	lt	t4, 48(a0)
	lt	t6, 56(a0)
	st	t0, 32(a1)
	st	t2, 40(a1)
	st	t4, 48(a1)
	st	t6, 56(a1)

	lt	t0, 64(a0)
	lt	t2, 72(a0)
	lt	t4, 80(a0)
	lt	t6, 88(a0)
	st	t0, 64(a1)
	st	t2, 72(a1)
	st	t4, 80(a1)
	st	t6, 88(a1)


	lt	t0, 96(a0)
	lt	t2, 104(a0)
	lt	t4, 112(a0)
	lt	t6, 120(a0)
	st	t0, 96(a1)
	st	t2, 104(a1)
	st	t4, 112(a1)
	st	t6, 120(a1)

	lt	t0, 128(a0)
	lt	t2, 136(a0)
	lt	t4, 144(a0)
	lt	t6, 152(a0)
	st	t0, 128(a1)
	st	t2, 136(a1)
	st	t4, 144(a1)
	st	t6, 152(a1)

	lt	t0, 160(a0)
	lt	t2, 168(a0)
	lt	t4, 176(a0)
	lt	t6, 184(a0)
	st	t0, 160(a1)
	st	t2, 168(a1)
	st	t4, 176(a1)
	st	t6, 184(a1)

	lt	t0, 192(a0)
	lt	t2, 200(a0)
	lt	t4, 208(a0)
	lt	t6, 216(a0)
	st	t0, 192(a1)
	st	t2, 200(a1)
	st	t4, 208(a1)
	st	t6, 216(a1)

	lt	t0, 224(a0)
	lt	t2, 232(a0)
	lt	t4, 240(a0)
	lt	t6, 248(a0)
	st	t0, 224(a1)
	st	t2, 232(a1)
	st	t4, 240(a1)
	st	t6, 248(a1)


	lt	t0, 256(a0)
	lt	t2, 264(a0)
	lt	t4, 272(a0)
	lt	t6, 280(a0)
	st	t0, 256(a1)
	st	t2, 264(a1)
	st	t4, 272(a1)
	st	t6, 280(a1)

	lt	t0, 288(a0)
	lt	t2, 296(a0)
	lt	t4, 304(a0)
	lt	t6, 312(a0)
	st	t0, 288(a1)
	st	t2, 296(a1)
	st	t4, 304(a1)
	st	t6, 312(a1)

	lt	t0, 320(a0)
	lt	t2, 328(a0)
	lt	t4, 336(a0)
	lt	t6, 344(a0)
	st	t0, 320(a1)
	st	t2, 328(a1)
	st	t4, 336(a1)
	st	t6, 344(a1)

	lt	t0, 352(a0)
	lt	t2, 360(a0)
	lt	t4, 368(a0)
	lt	t6, 376(a0)
	st	t0, 352(a1)
	st	t2, 360(a1)
	st	t4, 368(a1)
	st	t6, 376(a1)

	lt	t0, 384(a0)
	lt	t2, 392(a0)
	lt	t4, 400(a0)
	lt	t6, 408(a0)
	st	t0, 384(a1)
	st	t2, 392(a1)
	st	t4, 400(a1)
	st	t6, 408(a1)


	lt	t0, 416(a0)
	lt	t2, 424(a0)
	lt	t4, 432(a0)
	lt	t6, 440(a0)
	st	t0, 416(a1)
	st	t2, 424(a1)
	st	t4, 432(a1)
	st	t6, 440(a1)

	lt	t0, 448(a0)
	lt	t2, 456(a0)
	lt	t4, 464(a0)
	lt	t6, 472(a0)
	st	t0, 448(a1)
	st	t2, 456(a1)
	st	t4, 464(a1)
	st	t6, 472(a1)

	STOP_CP3_COUNTER

	jr 	ra
	nop
	.end DMEM_read_TWord_480_UR_B

	/* a0: DMEM Address, a1: DRAM Address, a2: counter mode, a3: number of times */
	.global DMEM_read_TWord_LP
        .text
        .ent DMEM_read_TWord_LP
	.set noreorder
	.section ".myTest", "ax"
DMEM_read_TWord_LP:
        mtc0 zero, $20 #write back invalidate dcache
        la t1, 0x00000200 # bits 8 of COP0 reg 20
        mtc0 t1, $20
	nop

	addi	a3, a3, -1
	/*waiting for empty of write buffer*/
	li	t1, 10000
2:
	bne	zero, t1, 2b
	addi	t1, t1, -1

	START_CP3_COUNTER
1:
	lt  t0, 0(a0)
	addi a0, a0, 8
	st  t0, 0(a1)
	addi a1, a1, 8
	bne zero, a3, 1b
	add a3, a3, -1

	STOP_CP3_COUNTER

	jr ra
	nop
	.end DMEM_read_TWord_LP


	/* a0: DMEM Address, a1: DRAM Address, a2: counter mode, a3:byte size */
	.global Full_Cache_Read_DMEM_TWord_write_LP_INV
        .text
        .ent Full_Cache_Read_DMEM_TWord_write_LP_INV
	.set noreorder
	.section ".myTest", "ax"
Full_Cache_Read_DMEM_TWord_write_LP_INV:
	addi	t1, a1, DCACHE_SIZE
	li	t0, (DCACHE_SIZE-4)
	add	t2, t1, t0
	/* Dirtify DCache Data */
2:
	lw	t3, 0(t1)
	sw	t1, 0(t1)
	bne	t2, t1, 2b
	addi	t1, t1, 4

	move	t4, a1

	START_CP3_COUNTER

	/* Invalidate data in DCache within a1 address */
	add	t0, t4, a3
	addi	t0, t0, -CACHE_LINE_SIZE
1:
	cache 	0x11, 0(t4)
	bne   	t0, t4, 1b
	add   	t4, t4, CACHE_LINE_SIZE 
	STOP_CP3_COUNTER

	
	/* Read DRAM data into DCache and write it into DMEM */
	add	t0, a1, a3
1:
	lt	t4, 0(a1)
	add   	a1, a1, 8
	st	t4, 0(a0)
	bne   	t0, a1, 1b
	add	a0, a0, 8


	jr ra
	nop
	.end Full_Cache_Read_DMEM_TWord_write_LP_INV

	/* a0: DMEM Address, a1: DRAM Address, a2: counter mode, a3:byte size */
	.global Full_Cache_Read_DMEM_TWord_write_LP_RW
        .text
        .ent Full_Cache_Read_DMEM_TWord_write_LP_RW
	.set noreorder
	.section ".myTest", "ax"
Full_Cache_Read_DMEM_TWord_write_LP_RW:
	addi	t1, a1, DCACHE_SIZE
	li	t0, (DCACHE_SIZE-4)
	add	t2, t1, t0
	/* Dirtify DCache Data */
2:
	lw	t3, 0(t1)
	sw	t1, 0(t1)
	bne	t2, t1, 2b
	addi	t1, t1, 4

	move	t4, a1


	/* Invalidate data in DCache within a1 address */
	add	t0, t4, a3
	addi	t0, t0, -CACHE_LINE_SIZE
1:
	cache 	0x11, 0(t4)
	bne   	t0, t4, 1b
	add   	t4, t4, CACHE_LINE_SIZE 

	START_CP3_COUNTER
	/* Read DRAM data into DCache and write it into DMEM */
	add	t0, a1, a3
1:
	lt	t4, 0(a1)
	add   	a1, a1, 8
	st	t4, 0(a0)
	bne   	t0, a1, 1b
	add	a0, a0, 8
	STOP_CP3_COUNTER

	jr ra
	nop
	.end Full_Cache_Read_DMEM_TWord_write_LP_RW


	/* a0: DMEM Address, a1: DRAM Address, a2: counter mode, a3:byte size */
	.global Full_Cache_Read_DMEM_TWord_write_LP
        .text
        .ent Full_Cache_Read_DMEM_TWord_write_LP
	.set noreorder
	.section ".myTest", "ax"
Full_Cache_Read_DMEM_TWord_write_LP:
	addi	t1, a1, DCACHE_SIZE
	li	t0, (DCACHE_SIZE-4)
	add	t2, t1, t0
	/* Dirtify DCache Data */
2:
	lw	t3, 0(t1)
	sw	t1, 0(t1)
	bne	t2, t1, 2b
	addi	t1, t1, 4

	move	t4, a1

	START_CP3_COUNTER

	/* Invalidate data in DCache within a1 address */
	add	t0, t4, a3
	addi	t0, t0, -CACHE_LINE_SIZE
1:
	cache 	0x11, 0(t4)
	bne   	t0, t4, 1b
	add   	t4, t4, CACHE_LINE_SIZE 
	
	/* Read DRAM data into DCache and write it into DMEM */
	add	t0, a1, a3
1:
	lt	t4, 0(a1)
	add   	a1, a1, 8
	st	t4, 0(a0)
	bne   	t0, a1, 1b
	add	a0, a0, 8
	STOP_CP3_COUNTER


	jr ra
	nop
	.end Full_Cache_Read_DMEM_TWord_write_LP


	/* a0: DMEM Address, a1: DRAM Address, a2: counter mode, a3:byte size */
	.global Empty_Cache_Read_DMEM_TWord_write_LP
        .text
        .ent Empty_Cache_Read_DMEM_TWord_write_LP
	.set noreorder
	.section ".myTest", "ax"
Empty_Cache_Read_DMEM_TWord_write_LP:
        mtc0 zero, $20 #write back invalidate dcache
        la t1, 0x00000200 # bits 8 of COP0 reg 20
        mtc0 t1, $20
	nop

	/*waiting for empty of write buffer*/
	li	t1, 10000
2:
	bne	zero, t1, 2b
	addi	t1, t1, -1

	move	t4, a1

	START_CP3_COUNTER

	/* Invalidate data in DCache within a1 address */
	add	t0, t4, a3
	addi	t0, t0, -CACHE_LINE_SIZE
1:
	cache 	0x11, 0(t4)
	bne   	t0, t4, 1b
	add   	t4, t4, CACHE_LINE_SIZE 

	
	/* Read DRAM data into DCache and write it into DMEM */
	add	t0, a1, a3
1:
	lt	t4, 0(a1)
	add   	a1, a1, 8
	st	t4, 0(a0)
	bne   	t0, a1, 1b
	add	a0, a0, 8
	STOP_CP3_COUNTER


	jr ra
	nop
	.end Empty_Cache_Read_DMEM_TWord_write_LP


	/* a0: DMEM Address, a1: DRAM Address, a2: counter mode, a3:byte size */
	.global Empty_Cache_Read_DMEM_TWord_write_LP_INV
        .text
        .ent Empty_Cache_Read_DMEM_TWord_write_LP_INV
	.set noreorder
	.section ".myTest", "ax"
Empty_Cache_Read_DMEM_TWord_write_LP_INV:
        mtc0 zero, $20 #write back invalidate dcache
        la t1, 0x00000200 # bits 8 of COP0 reg 20
        mtc0 t1, $20
	nop

	/*waiting for empty of write buffer*/
	li	t1, 10000
2:
	bne	zero, t1, 2b
	addi	t1, t1, -1

	move	t4, a1

	START_CP3_COUNTER

	/* Invalidate data in DCache within a1 address */
	add	t0, t4, a3
	addi	t0, t0, -CACHE_LINE_SIZE
1:
	cache 	0x11, 0(t4)
	bne   	t0, t4, 1b
	add   	t4, t4, CACHE_LINE_SIZE 

	STOP_CP3_COUNTER

	
	/* Read DRAM data into DCache and write it into DMEM */
	add	t0, a1, a3
1:
	lt	t4, 0(a1)
	add   	a1, a1, 8
	st	t4, 0(a0)
	bne   	t0, a1, 1b
	add	a0, a0, 8


	jr ra
	nop
	.end Empty_Cache_Read_DMEM_TWord_write_LP_INV

	/* a0: DMEM Address, a1: DRAM Address, a2: counter mode, a3:byte size */
	.global Empty_Cache_Read_DMEM_TWord_write_LP_RW
        .text
        .ent Empty_Cache_Read_DMEM_TWord_write_LP_RW
	.set noreorder
	.section ".myTest", "ax"
Empty_Cache_Read_DMEM_TWord_write_LP_RW:
        mtc0 zero, $20 #write back invalidate dcache
        la t1, 0x00000200 # bits 8 of COP0 reg 20
        mtc0 t1, $20
	nop

	/*waiting for empty of write buffer*/
	li	t1, 10000
2:
	bne	zero, t1, 2b
	addi	t1, t1, -1

	move	t4, a1


	/* Invalidate data in DCache within a1 address */
	add	t0, t4, a3
	addi	t0, t0, -CACHE_LINE_SIZE
1:
	cache 	0x11, 0(t4)
	bne   	t0, t4, 1b
	add   	t4, t4, CACHE_LINE_SIZE 

	START_CP3_COUNTER
	
	/* Read DRAM data into DCache and write it into DMEM */
	add	t0, a1, a3
1:
	lt	t4, 0(a1)
	add   	a1, a1, 8
	st	t4, 0(a0)
	bne   	t0, a1, 1b
	add	a0, a0, 8
	STOP_CP3_COUNTER


	jr ra
	nop
	.end Empty_Cache_Read_DMEM_TWord_write_LP_RW


	/* a0: DMEM Address, a1: DRAM Address, a2: counter mode, a3:byte size */
	.global TWord_Read_DMEM_TWord_write_LP
        .text
        .ent TWord_Read_DMEM_TWord_write_LP
	.set noreorder
	.section ".myTest", "ax"
TWord_Read_DMEM_TWord_write_LP:
        mtc0 zero, $20 #write back invalidate dcache
        la t1, 0x00000200 # bits 8 of COP0 reg 20
        mtc0 t1, $20
	nop

	/*waiting for empty of write buffer*/
	li	t1, 10000
2:
	bne	zero, t1, 2b
	addi	t1, t1, -1

	move	t4, a1

	START_CP3_COUNTER
	/* Read DRAM data into DCache and write it into DMEM */
	add	t0, a1, a3
1:
	lt	t4, 0(a1)
	add   	a1, a1, 8
	st	t4, 0(a0)
	bne   	t0, a1, 1b
	add	a0, a0, 8
	STOP_CP3_COUNTER

	jr ra
	nop
	.end TWord_Read_DMEM_TWord_write_LP



	/* a0: DMEM Address, a1: DRAM Address, a2: counter mode */
	.global DMEM_read_Cache_Ins_Flush_160_read_d_in
        .text
        .ent DMEM_read_Cache_Ins_Flush_160_read_d_in
	.set noreorder
	.section ".myTest", "ax"
DMEM_read_Cache_Ins_Flush_160_read_d_in:
        mtc0 zero, $20 #write back invalidate dcache
        la t1, 0x00000200 # bits 8 of COP0 reg 20
        mtc0 t1, $20
	nop

	/*waiting for empty of write buffer*/
	li	t1, 10000
2:
	bne	zero, t1, 2b
	addi	t1, t1, -1

	START_CP3_COUNTER
	/* Read data into Dcache */
	move	t2, a1	     /* copy start address */
	addi	t0, a1, (160-CACHE_LINE_SIZE)  /* copy end address */
1:
	lw	t1, 0(t2)
	bne   	t0, t2, 1b
	addi   	t2, t2, CACHE_LINE_SIZE
	STOP_CP3_COUNTER


	lt	t0, 0(a0)
	st	t0, 0(a1)
	lt	t0, 8(a0)
	st	t0, 8(a1)
	lt	t0, 16(a0)
	st	t0, 16(a1)
	lt	t0, 24(a0)
	st	t0, 24(a1)
	lt	t0, 32(a0)
	st	t0, 32(a1)
	lt	t0, 40(a0)
	st	t0, 40(a1)
	lt	t0, 48(a0)
	st	t0, 48(a1)
	lt	t0, 56(a0)
	st	t0, 56(a1)
	lt	t0, 64(a0)
	st	t0, 64(a1)
	lt	t0, 72(a0)
	st	t0, 72(a1)

	lt	t0, 80(a0)
	st	t0, 80(a1)
	lt	t0, 88(a0)
	st	t0, 88(a1)
	lt	t0, 96(a0)
	st	t0, 96(a1)
	lt	t0, 104(a0)
	st	t0, 104(a1)
	lt	t0, 112(a0)
	st	t0, 112(a1)
	lt	t0, 120(a0)
	st	t0, 120(a1)
	lt	t0, 128(a0)
	st	t0, 128(a1)
	lt	t0, 136(a0)
	st	t0, 136(a1)
	lt	t0, 144(a0)
	st	t0, 144(a1)
	lt	t0, 152(a0)
	st	t0, 152(a1)

	addi	t0, a1, (160-CACHE_LINE_SIZE)	
1:
	cache 	0x19, 0(a1)
	bne   	t0, a1, 1b
	add   	a1, a1, CACHE_LINE_SIZE 


	jr	ra
	nop
	.end DMEM_read_Cache_Ins_Flush_160_read_d_in

	/* a0: DMEM Address, a1: DRAM Address, a2: counter mode */
	.global DMEM_read_Cache_Ins_Flush_160_write_d_in
        .text
        .ent DMEM_read_Cache_Ins_Flush_160_write_d_in
	.set noreorder
	.section ".myTest", "ax"
DMEM_read_Cache_Ins_Flush_160_write_d_in:
        mtc0 zero, $20 #write back invalidate dcache
        la t1, 0x00000200 # bits 8 of COP0 reg 20
        mtc0 t1, $20
	nop

	/*waiting for empty of write buffer*/
	li	t1, 10000
2:
	bne	zero, t1, 2b
	addi	t1, t1, -1

	/* Read data into Dcache */
	move	t2, a1	     /* copy start address */
	addi	t0, a1, (160-CACHE_LINE_SIZE)  /* copy end address */
1:
	lw	t1, 0(t2)
	bne   	t0, t2, 1b
	addi   	t2, t2, CACHE_LINE_SIZE


	START_CP3_COUNTER
	lt	t0, 0(a0)
	st	t0, 0(a1)
	lt	t0, 8(a0)
	st	t0, 8(a1)
	lt	t0, 16(a0)
	st	t0, 16(a1)
	lt	t0, 24(a0)
	st	t0, 24(a1)
	lt	t0, 32(a0)
	st	t0, 32(a1)
	lt	t0, 40(a0)
	st	t0, 40(a1)
	lt	t0, 48(a0)
	st	t0, 48(a1)
	lt	t0, 56(a0)
	st	t0, 56(a1)
	lt	t0, 64(a0)
	st	t0, 64(a1)
	lt	t0, 72(a0)
	st	t0, 72(a1)

	lt	t0, 80(a0)
	st	t0, 80(a1)
	lt	t0, 88(a0)
	st	t0, 88(a1)
	lt	t0, 96(a0)
	st	t0, 96(a1)
	lt	t0, 104(a0)
	st	t0, 104(a1)
	lt	t0, 112(a0)
	st	t0, 112(a1)
	lt	t0, 120(a0)
	st	t0, 120(a1)
	lt	t0, 128(a0)
	st	t0, 128(a1)
	lt	t0, 136(a0)
	st	t0, 136(a1)
	lt	t0, 144(a0)
	st	t0, 144(a1)
	lt	t0, 152(a0)
	st	t0, 152(a1)
	STOP_CP3_COUNTER

	addi	t0, a1, (160-CACHE_LINE_SIZE)	
1:
	cache 	0x19, 0(a1)
	bne   	t0, a1, 1b
	add   	a1, a1, CACHE_LINE_SIZE


	jr	ra
	nop
	.end DMEM_read_Cache_Ins_Flush_160_write_d_in

	/* a0: DMEM Address, a1: DRAM Address, a2: counter mode */
	.global DMEM_read_Cache_Ins_Flush_160_cache_ins
        .text
        .ent DMEM_read_Cache_Ins_Flush_160_cache_ins
	.set noreorder
	.section ".myTest", "ax"
DMEM_read_Cache_Ins_Flush_160_cache_ins:
        mtc0 zero, $20 #write back invalidate dcache
        la t1, 0x00000200 # bits 8 of COP0 reg 20
        mtc0 t1, $20
	nop

	/*waiting for empty of write buffer*/
	li	t1, 10000
2:
	bne	zero, t1, 2b
	addi	t1, t1, -1

	/* Read data into Dcache */
	move	t2, a1	     /* copy start address */
	addi	t0, a1, (160-CACHE_LINE_SIZE)  /* copy end address */
1:
	lw	t1, 0(t2)
	bne   	t0, t2, 1b
	addi   	t2, t2, CACHE_LINE_SIZE


	lt	t0, 0(a0)
	st	t0, 0(a1)
	lt	t0, 8(a0)
	st	t0, 8(a1)
	lt	t0, 16(a0)
	st	t0, 16(a1)
	lt	t0, 24(a0)
	st	t0, 24(a1)
	lt	t0, 32(a0)
	st	t0, 32(a1)
	lt	t0, 40(a0)
	st	t0, 40(a1)
	lt	t0, 48(a0)
	st	t0, 48(a1)
	lt	t0, 56(a0)
	st	t0, 56(a1)
	lt	t0, 64(a0)
	st	t0, 64(a1)
	lt	t0, 72(a0)
	st	t0, 72(a1)

	lt	t0, 80(a0)
	st	t0, 80(a1)
	lt	t0, 88(a0)
	st	t0, 88(a1)
	lt	t0, 96(a0)
	st	t0, 96(a1)
	lt	t0, 104(a0)
	st	t0, 104(a1)
	lt	t0, 112(a0)
	st	t0, 112(a1)
	lt	t0, 120(a0)
	st	t0, 120(a1)
	lt	t0, 128(a0)
	st	t0, 128(a1)
	lt	t0, 136(a0)
	st	t0, 136(a1)
	lt	t0, 144(a0)
	st	t0, 144(a1)
	lt	t0, 152(a0)
	st	t0, 152(a1)

	START_CP3_COUNTER
	addi	t0, a1, (160-CACHE_LINE_SIZE)
1:
	cache 	0x19, 0(a1)
	bne   	t0, a1, 1b
	add   	a1, a1, CACHE_LINE_SIZE

	STOP_CP3_COUNTER

	jr	ra
	nop
	.end DMEM_read_Cache_Ins_Flush_160_cache_ins


	/* a0: DMEM Address, a1: DRAM Address, a2: counter mode */
	.global DMEM_read_Cache_Ins_Flush_160_total
        .text
        .ent DMEM_read_Cache_Ins_Flush_160_total
	.set noreorder
	.section ".myTest", "ax"
DMEM_read_Cache_Ins_Flush_160_total:
        mtc0 zero, $20 #write back invalidate dcache
        la t1, 0x00000200 # bits 8 of COP0 reg 20
        mtc0 t1, $20
	nop

	/*waiting for empty of write buffer*/
	li	t1, 10000
2:
	bne	zero, t1, 2b
	addi	t1, t1, -1

	START_CP3_COUNTER
	/* Read data into Dcache */
	move	t2, a1	     /* copy start address */
	addi	t0, a1, (160-CACHE_LINE_SIZE)  /* copy end address */
1:
	lw	t1, 0(t2)
	bne   	t0, t2, 1b
	addi   	t2, t2, CACHE_LINE_SIZE


	lt	t0, 0(a0)
	st	t0, 0(a1)
	lt	t0, 8(a0)
	st	t0, 8(a1)
	lt	t0, 16(a0)
	st	t0, 16(a1)
	lt	t0, 24(a0)
	st	t0, 24(a1)
	lt	t0, 32(a0)
	st	t0, 32(a1)
	lt	t0, 40(a0)
	st	t0, 40(a1)
	lt	t0, 48(a0)
	st	t0, 48(a1)
	lt	t0, 56(a0)
	st	t0, 56(a1)
	lt	t0, 64(a0)
	st	t0, 64(a1)
	lt	t0, 72(a0)
	st	t0, 72(a1)

	lt	t0, 80(a0)
	st	t0, 80(a1)
	lt	t0, 88(a0)
	st	t0, 88(a1)
	lt	t0, 96(a0)
	st	t0, 96(a1)
	lt	t0, 104(a0)
	st	t0, 104(a1)
	lt	t0, 112(a0)
	st	t0, 112(a1)
	lt	t0, 120(a0)
	st	t0, 120(a1)
	lt	t0, 128(a0)
	st	t0, 128(a1)
	lt	t0, 136(a0)
	st	t0, 136(a1)
	lt	t0, 144(a0)
	st	t0, 144(a1)
	lt	t0, 152(a0)
	st	t0, 152(a1)

	addi	t0, a1, (160-CACHE_LINE_SIZE)
1:
	cache 	0x19, 0(a1)
	bne   	t0, a1, 1b
	add   	a1, a1, CACHE_LINE_SIZE

	STOP_CP3_COUNTER

	jr	ra
	nop
	.end DMEM_read_Cache_Ins_Flush_160_total


	/* a0: DMEM Address, a1: DRAM Address, a2: counter mode */
	.global DMEM_read_Cache_Ins_Flush_480_total
        .text
        .ent DMEM_read_Cache_Ins_Flush_480_total
	.set noreorder
	.section ".myTest", "ax"
DMEM_read_Cache_Ins_Flush_480_total:

        mtc0 zero, $20 #write back invalidate dcache
        la t1, 0x00000200 # bits 8 of COP0 reg 20
        mtc0 t1, $20
	nop

	/*waiting for empty of write buffer*/
	li	t1, 10000
2:
	bne	zero, t1, 2b
	addi	t1, t1, -1

	START_CP3_COUNTER
	/* Read data into Dcache */
	move	t2, a1	     /* copy start address */
	addi	t0, a1, (480-CACHE_LINE_SIZE)  /* copy end address */
1:
	lw	t1, 0(t2)
	bne   	t0, t2, 1b
	addi   	t2, t2, CACHE_LINE_SIZE 

	lt	t0, 0(a0)
	st	t0, 0(a1)
	lt	t0, 8(a0)
	st	t0, 8(a1)
	lt	t0, 16(a0)
	st	t0, 16(a1)
	lt	t0, 24(a0)
	st	t0, 24(a1)
	lt	t0, 32(a0)
	st	t0, 32(a1)
	lt	t0, 40(a0)
	st	t0, 40(a1)
	lt	t0, 48(a0)
	st	t0, 48(a1)
	lt	t0, 56(a0)
	st	t0, 56(a1)
	lt	t0, 64(a0)
	st	t0, 64(a1)
	lt	t0, 72(a0)
	st	t0, 72(a1)

	lt	t0, 80(a0)
	st	t0, 80(a1)
	lt	t0, 88(a0)
	st	t0, 88(a1)
	lt	t0, 96(a0)
	st	t0, 96(a1)
	lt	t0, 104(a0)
	st	t0, 104(a1)
	lt	t0, 112(a0)
	st	t0, 112(a1)
	lt	t0, 120(a0)
	st	t0, 120(a1)
	lt	t0, 128(a0)
	st	t0, 128(a1)
	lt	t0, 136(a0)
	st	t0, 136(a1)
	lt	t0, 144(a0)
	st	t0, 144(a1)
	lt	t0, 152(a0)
	st	t0, 152(a1)

	lt	t0, 160(a0)
	st	t0, 160(a1)
	lt	t0, 168(a0)
	st	t0, 168(a1)
	lt	t0, 176(a0)
	st	t0, 176(a1)
	lt	t0, 184(a0)
	st	t0, 184(a1)
	lt	t0, 192(a0)
	st	t0, 192(a1)
	lt	t0, 200(a0)
	st	t0, 200(a1)
	lt	t0, 208(a0)
	st	t0, 208(a1)
	lt	t0, 216(a0)
	st	t0, 216(a1)
	lt	t0, 224(a0)
	st	t0, 224(a1)
	lt	t0, 232(a0)
	st	t0, 232(a1)

	lt	t0, 240(a0)
	st	t0, 240(a1)
	lt	t0, 248(a0)
	st	t0, 248(a1)
	lt	t0, 256(a0)
	st	t0, 256(a1)
	lt	t0, 264(a0)
	st	t0, 264(a1)
	lt	t0, 272(a0)
	st	t0, 272(a1)
	lt	t0, 280(a0)
	st	t0, 280(a1)
	lt	t0, 288(a0)
	st	t0, 288(a1)
	lt	t0, 296(a0)
	st	t0, 296(a1)
	lt	t0, 304(a0)
	st	t0, 304(a1)
	lt	t0, 312(a0)
	st	t0, 312(a1)

	lt	t0, 320(a0)
	st	t0, 320(a1)
	lt	t0, 328(a0)
	st	t0, 328(a1)
	lt	t0, 336(a0)
	st	t0, 336(a1)
	lt	t0, 344(a0)
	st	t0, 344(a1)
	lt	t0, 352(a0)
	st	t0, 352(a1)
	lt	t0, 360(a0)
	st	t0, 360(a1)
	lt	t0, 368(a0)
	st	t0, 368(a1)
	lt	t0, 376(a0)
	st	t0, 376(a1)
	lt	t0, 384(a0)
	st	t0, 384(a1)
	lt	t0, 392(a0)
	st	t0, 392(a1)

	lt	t0, 400(a0)
	st	t0, 400(a1)
	lt	t0, 408(a0)
	st	t0, 408(a1)
	lt	t0, 416(a0)
	st	t0, 416(a1)
	lt	t0, 424(a0)
	st	t0, 424(a1)
	lt	t0, 432(a0)
	st	t0, 432(a1)
	lt	t0, 440(a0)
	st	t0, 440(a1)
	lt	t0, 448(a0)
	st	t0, 448(a1)
	lt	t0, 456(a0)
	st	t0, 456(a1)
	lt	t0, 464(a0)
	st	t0, 464(a1)
	lt	t0, 472(a0)
	st	t0, 472(a1)


	addi	t0, a1, (480-CACHE_LINE_SIZE)
1:
	cache 	0x19, 0(a1)
	bne   	t0, a1, 1b
	add   	a1, a1, CACHE_LINE_SIZE

	STOP_CP3_COUNTER

	jr	ra
	nop
	.end DMEM_read_Cache_Ins_Flush_480_total


	/* a0: DMEM Address, a1: DRAM Address, a2: counter mode */
	.global DMEM_read_Cache_Ins_Flush_480_read_d_in
        .text
        .ent DMEM_read_Cache_Ins_Flush_480_read_d_in
	.set noreorder
	.section ".myTest", "ax"
DMEM_read_Cache_Ins_Flush_480_read_d_in:

        mtc0 zero, $20 #write back invalidate dcache
        la t1, 0x00000200 # bits 8 of COP0 reg 20
        mtc0 t1, $20
	nop

	/*waiting for empty of write buffer*/
	li	t1, 10000
2:
	bne	zero, t1, 2b
	addi	t1, t1, -1

	START_CP3_COUNTER
	/* Read data into Dcache */
	move	t2, a1	     /* copy start address */
	addi	t0, a1, (480-CACHE_LINE_SIZE)  /* copy end address */
1:
	lw	t1, 0(t2)
	bne   	t0, t2, 1b
	addi   	t2, t2, CACHE_LINE_SIZE

	STOP_CP3_COUNTER

	lt	t0, 0(a0)
	st	t0, 0(a1)
	lt	t0, 8(a0)
	st	t0, 8(a1)
	lt	t0, 16(a0)
	st	t0, 16(a1)
	lt	t0, 24(a0)
	st	t0, 24(a1)
	lt	t0, 32(a0)
	st	t0, 32(a1)
	lt	t0, 40(a0)
	st	t0, 40(a1)
	lt	t0, 48(a0)
	st	t0, 48(a1)
	lt	t0, 56(a0)
	st	t0, 56(a1)
	lt	t0, 64(a0)
	st	t0, 64(a1)
	lt	t0, 72(a0)
	st	t0, 72(a1)

	lt	t0, 80(a0)
	st	t0, 80(a1)
	lt	t0, 88(a0)
	st	t0, 88(a1)
	lt	t0, 96(a0)
	st	t0, 96(a1)
	lt	t0, 104(a0)
	st	t0, 104(a1)
	lt	t0, 112(a0)
	st	t0, 112(a1)
	lt	t0, 120(a0)
	st	t0, 120(a1)
	lt	t0, 128(a0)
	st	t0, 128(a1)
	lt	t0, 136(a0)
	st	t0, 136(a1)
	lt	t0, 144(a0)
	st	t0, 144(a1)
	lt	t0, 152(a0)
	st	t0, 152(a1)

	lt	t0, 160(a0)
	st	t0, 160(a1)
	lt	t0, 168(a0)
	st	t0, 168(a1)
	lt	t0, 176(a0)
	st	t0, 176(a1)
	lt	t0, 184(a0)
	st	t0, 184(a1)
	lt	t0, 192(a0)
	st	t0, 192(a1)
	lt	t0, 200(a0)
	st	t0, 200(a1)
	lt	t0, 208(a0)
	st	t0, 208(a1)
	lt	t0, 216(a0)
	st	t0, 216(a1)
	lt	t0, 224(a0)
	st	t0, 224(a1)
	lt	t0, 232(a0)
	st	t0, 232(a1)

	lt	t0, 240(a0)
	st	t0, 240(a1)
	lt	t0, 248(a0)
	st	t0, 248(a1)
	lt	t0, 256(a0)
	st	t0, 256(a1)
	lt	t0, 264(a0)
	st	t0, 264(a1)
	lt	t0, 272(a0)
	st	t0, 272(a1)
	lt	t0, 280(a0)
	st	t0, 280(a1)
	lt	t0, 288(a0)
	st	t0, 288(a1)
	lt	t0, 296(a0)
	st	t0, 296(a1)
	lt	t0, 304(a0)
	st	t0, 304(a1)
	lt	t0, 312(a0)
	st	t0, 312(a1)

	lt	t0, 320(a0)
	st	t0, 320(a1)
	lt	t0, 328(a0)
	st	t0, 328(a1)
	lt	t0, 336(a0)
	st	t0, 336(a1)
	lt	t0, 344(a0)
	st	t0, 344(a1)
	lt	t0, 352(a0)
	st	t0, 352(a1)
	lt	t0, 360(a0)
	st	t0, 360(a1)
	lt	t0, 368(a0)
	st	t0, 368(a1)
	lt	t0, 376(a0)
	st	t0, 376(a1)
	lt	t0, 384(a0)
	st	t0, 384(a1)
	lt	t0, 392(a0)
	st	t0, 392(a1)

	lt	t0, 400(a0)
	st	t0, 400(a1)
	lt	t0, 408(a0)
	st	t0, 408(a1)
	lt	t0, 416(a0)
	st	t0, 416(a1)
	lt	t0, 424(a0)
	st	t0, 424(a1)
	lt	t0, 432(a0)
	st	t0, 432(a1)
	lt	t0, 440(a0)
	st	t0, 440(a1)
	lt	t0, 448(a0)
	st	t0, 448(a1)
	lt	t0, 456(a0)
	st	t0, 456(a1)
	lt	t0, 464(a0)
	st	t0, 464(a1)
	lt	t0, 472(a0)
	st	t0, 472(a1)


	addi	t0, a1, (480-CACHE_LINE_SIZE)	
1:
	cache 	0x19, 0(a1)
	bne   	t0, a1, 1b
	add   	a1, a1, CACHE_LINE_SIZE 


	jr	ra
	nop
	.end DMEM_read_Cache_Ins_Flush_480_read_d_in

	/* a0: DMEM Address, a1: DRAM Address, a2: counter mode */
	.global DMEM_read_Cache_Ins_Flush_480_write_d_in
        .text
        .ent DMEM_read_Cache_Ins_Flush_480_write_d_in
	.set noreorder
	.section ".myTest", "ax"
DMEM_read_Cache_Ins_Flush_480_write_d_in:

        mtc0 zero, $20 #write back invalidate dcache
        la t1, 0x00000200 # bits 8 of COP0 reg 20
        mtc0 t1, $20
	nop

	/*waiting for empty of write buffer*/
	li	t1, 10000
2:
	bne	zero, t1, 2b
	addi	t1, t1, -1

	/* Read data into Dcache */
	move	t2, a1	     /* copy start address */
	addi	t0, a1, (480-CACHE_LINE_SIZE)  /* copy end address */
1:
	lw	t1, 0(t2)
	bne   	t0, t2, 1b
	addi   	t2, t2, CACHE_LINE_SIZE 


	START_CP3_COUNTER
	lt	t0, 0(a0)
	st	t0, 0(a1)
	lt	t0, 8(a0)
	st	t0, 8(a1)
	lt	t0, 16(a0)
	st	t0, 16(a1)
	lt	t0, 24(a0)
	st	t0, 24(a1)
	lt	t0, 32(a0)
	st	t0, 32(a1)
	lt	t0, 40(a0)
	st	t0, 40(a1)
	lt	t0, 48(a0)
	st	t0, 48(a1)
	lt	t0, 56(a0)
	st	t0, 56(a1)
	lt	t0, 64(a0)
	st	t0, 64(a1)
	lt	t0, 72(a0)
	st	t0, 72(a1)

	lt	t0, 80(a0)
	st	t0, 80(a1)
	lt	t0, 88(a0)
	st	t0, 88(a1)
	lt	t0, 96(a0)
	st	t0, 96(a1)
	lt	t0, 104(a0)
	st	t0, 104(a1)
	lt	t0, 112(a0)
	st	t0, 112(a1)
	lt	t0, 120(a0)
	st	t0, 120(a1)
	lt	t0, 128(a0)
	st	t0, 128(a1)
	lt	t0, 136(a0)
	st	t0, 136(a1)
	lt	t0, 144(a0)
	st	t0, 144(a1)
	lt	t0, 152(a0)
	st	t0, 152(a1)

	lt	t0, 160(a0)
	st	t0, 160(a1)
	lt	t0, 168(a0)
	st	t0, 168(a1)
	lt	t0, 176(a0)
	st	t0, 176(a1)
	lt	t0, 184(a0)
	st	t0, 184(a1)
	lt	t0, 192(a0)
	st	t0, 192(a1)
	lt	t0, 200(a0)
	st	t0, 200(a1)
	lt	t0, 208(a0)
	st	t0, 208(a1)
	lt	t0, 216(a0)
	st	t0, 216(a1)
	lt	t0, 224(a0)
	st	t0, 224(a1)
	lt	t0, 232(a0)
	st	t0, 232(a1)

	lt	t0, 240(a0)
	st	t0, 240(a1)
	lt	t0, 248(a0)
	st	t0, 248(a1)
	lt	t0, 256(a0)
	st	t0, 256(a1)
	lt	t0, 264(a0)
	st	t0, 264(a1)
	lt	t0, 272(a0)
	st	t0, 272(a1)
	lt	t0, 280(a0)
	st	t0, 280(a1)
	lt	t0, 288(a0)
	st	t0, 288(a1)
	lt	t0, 296(a0)
	st	t0, 296(a1)
	lt	t0, 304(a0)
	st	t0, 304(a1)
	lt	t0, 312(a0)
	st	t0, 312(a1)

	lt	t0, 320(a0)
	st	t0, 320(a1)
	lt	t0, 328(a0)
	st	t0, 328(a1)
	lt	t0, 336(a0)
	st	t0, 336(a1)
	lt	t0, 344(a0)
	st	t0, 344(a1)
	lt	t0, 352(a0)
	st	t0, 352(a1)
	lt	t0, 360(a0)
	st	t0, 360(a1)
	lt	t0, 368(a0)
	st	t0, 368(a1)
	lt	t0, 376(a0)
	st	t0, 376(a1)
	lt	t0, 384(a0)
	st	t0, 384(a1)
	lt	t0, 392(a0)
	st	t0, 392(a1)

	lt	t0, 400(a0)
	st	t0, 400(a1)
	lt	t0, 408(a0)
	st	t0, 408(a1)
	lt	t0, 416(a0)
	st	t0, 416(a1)
	lt	t0, 424(a0)
	st	t0, 424(a1)
	lt	t0, 432(a0)
	st	t0, 432(a1)
	lt	t0, 440(a0)
	st	t0, 440(a1)
	lt	t0, 448(a0)
	st	t0, 448(a1)
	lt	t0, 456(a0)
	st	t0, 456(a1)
	lt	t0, 464(a0)
	st	t0, 464(a1)
	lt	t0, 472(a0)
	st	t0, 472(a1)

	STOP_CP3_COUNTER

	addi	t0, a1, (480-CACHE_LINE_SIZE)
1:
	cache 	0x19, 0(a1)
	bne   	t0, a1, 1b
	add   	a1, a1, CACHE_LINE_SIZE


	jr	ra
	nop
	.end DMEM_read_Cache_Ins_Flush_480_write_d_in


	/* a0: DMEM Address, a1: DRAM Address, a2: counter mode */
	.global DMEM_read_Cache_Ins_Flush_480_cache_ins
        .text
        .ent DMEM_read_Cache_Ins_Flush_480_cache_ins
	.set noreorder
	.section ".myTest", "ax"
DMEM_read_Cache_Ins_Flush_480_cache_ins:

        mtc0 zero, $20 #write back invalidate dcache
        la t1, 0x00000200 # bits 8 of COP0 reg 20
        mtc0 t1, $20
	nop

	/*waiting for empty of write buffer*/
	li	t1, 10000
2:
	bne	zero, t1, 2b
	addi	t1, t1, -1

	/* Read data into Dcache */
	move	t2, a1	     /* copy start address */
	addi	t0, a1, (480-CACHE_LINE_SIZE)  /* copy end address */
1:
	lw	t1, 0(t2)
	bne   	t0, t2, 1b
	addi   	t2, t2, CACHE_LINE_SIZE 

	lt	t0, 0(a0)
	st	t0, 0(a1)
	lt	t0, 8(a0)
	st	t0, 8(a1)
	lt	t0, 16(a0)
	st	t0, 16(a1)
	lt	t0, 24(a0)
	st	t0, 24(a1)
	lt	t0, 32(a0)
	st	t0, 32(a1)
	lt	t0, 40(a0)
	st	t0, 40(a1)
	lt	t0, 48(a0)
	st	t0, 48(a1)
	lt	t0, 56(a0)
	st	t0, 56(a1)
	lt	t0, 64(a0)
	st	t0, 64(a1)
	lt	t0, 72(a0)
	st	t0, 72(a1)

	lt	t0, 80(a0)
	st	t0, 80(a1)
	lt	t0, 88(a0)
	st	t0, 88(a1)
	lt	t0, 96(a0)
	st	t0, 96(a1)
	lt	t0, 104(a0)
	st	t0, 104(a1)
	lt	t0, 112(a0)
	st	t0, 112(a1)
	lt	t0, 120(a0)
	st	t0, 120(a1)
	lt	t0, 128(a0)
	st	t0, 128(a1)
	lt	t0, 136(a0)
	st	t0, 136(a1)
	lt	t0, 144(a0)
	st	t0, 144(a1)
	lt	t0, 152(a0)
	st	t0, 152(a1)

	lt	t0, 160(a0)
	st	t0, 160(a1)
	lt	t0, 168(a0)
	st	t0, 168(a1)
	lt	t0, 176(a0)
	st	t0, 176(a1)
	lt	t0, 184(a0)
	st	t0, 184(a1)
	lt	t0, 192(a0)
	st	t0, 192(a1)
	lt	t0, 200(a0)
	st	t0, 200(a1)
	lt	t0, 208(a0)
	st	t0, 208(a1)
	lt	t0, 216(a0)
	st	t0, 216(a1)
	lt	t0, 224(a0)
	st	t0, 224(a1)
	lt	t0, 232(a0)
	st	t0, 232(a1)

	lt	t0, 240(a0)
	st	t0, 240(a1)
	lt	t0, 248(a0)
	st	t0, 248(a1)
	lt	t0, 256(a0)
	st	t0, 256(a1)
	lt	t0, 264(a0)
	st	t0, 264(a1)
	lt	t0, 272(a0)
	st	t0, 272(a1)
	lt	t0, 280(a0)
	st	t0, 280(a1)
	lt	t0, 288(a0)
	st	t0, 288(a1)
	lt	t0, 296(a0)
	st	t0, 296(a1)
	lt	t0, 304(a0)
	st	t0, 304(a1)
	lt	t0, 312(a0)
	st	t0, 312(a1)

	lt	t0, 320(a0)
	st	t0, 320(a1)
	lt	t0, 328(a0)
	st	t0, 328(a1)
	lt	t0, 336(a0)
	st	t0, 336(a1)
	lt	t0, 344(a0)
	st	t0, 344(a1)
	lt	t0, 352(a0)
	st	t0, 352(a1)
	lt	t0, 360(a0)
	st	t0, 360(a1)
	lt	t0, 368(a0)
	st	t0, 368(a1)
	lt	t0, 376(a0)
	st	t0, 376(a1)
	lt	t0, 384(a0)
	st	t0, 384(a1)
	lt	t0, 392(a0)
	st	t0, 392(a1)

	lt	t0, 400(a0)
	st	t0, 400(a1)
	lt	t0, 408(a0)
	st	t0, 408(a1)
	lt	t0, 416(a0)
	st	t0, 416(a1)
	lt	t0, 424(a0)
	st	t0, 424(a1)
	lt	t0, 432(a0)
	st	t0, 432(a1)
	lt	t0, 440(a0)
	st	t0, 440(a1)
	lt	t0, 448(a0)
	st	t0, 448(a1)
	lt	t0, 456(a0)
	st	t0, 456(a1)
	lt	t0, 464(a0)
	st	t0, 464(a1)
	lt	t0, 472(a0)
	st	t0, 472(a1)


	START_CP3_COUNTER
	addi	t0, a1, (480-CACHE_LINE_SIZE)
1:
	cache 	0x19, 0(a1)
	bne   	t0, a1, 1b
	add   	a1, a1, CACHE_LINE_SIZE

	STOP_CP3_COUNTER

	jr	ra
	nop
	.end DMEM_read_Cache_Ins_Flush_480_cache_ins


	/* a0: DMEM Address, a1: DRAM Address, a2: counter mode */
	.global DMEM_read_cp0_Flush_480_flush
        .text
        .ent DMEM_read_cp0_Flush_480_flush
	.set noreorder
	.section ".myTest", "ax"
DMEM_read_cp0_Flush_480_flush:

        mtc0 zero, $20 #write back invalidate dcache
        la t1, 0x00000200 # bits 8 of COP0 reg 20
        mtc0 t1, $20
	nop

	/*waiting for empty of write buffer*/
	li	t1, 10000
2:
	bne	zero, t1, 2b
	addi	t1, t1, -1

	/* Read data into Dcache */
	move	t2, a1	     /* copy start address */
	addi	t0, a1, (480-CACHE_LINE_SIZE)  /* copy end address */
1:
	lw	t1, 0(t2)
	bne   	t0, t2, 1b
	addi   	t2, t2, CACHE_LINE_SIZE

	lt	t0, 0(a0)
	st	t0, 0(a1)
	lt	t0, 8(a0)
	st	t0, 8(a1)
	lt	t0, 16(a0)
	st	t0, 16(a1)
	lt	t0, 24(a0)
	st	t0, 24(a1)
	lt	t0, 32(a0)
	st	t0, 32(a1)
	lt	t0, 40(a0)
	st	t0, 40(a1)
	lt	t0, 48(a0)
	st	t0, 48(a1)
	lt	t0, 56(a0)
	st	t0, 56(a1)
	lt	t0, 64(a0)
	st	t0, 64(a1)
	lt	t0, 72(a0)
	st	t0, 72(a1)

	lt	t0, 80(a0)
	st	t0, 80(a1)
	lt	t0, 88(a0)
	st	t0, 88(a1)
	lt	t0, 96(a0)
	st	t0, 96(a1)
	lt	t0, 104(a0)
	st	t0, 104(a1)
	lt	t0, 112(a0)
	st	t0, 112(a1)
	lt	t0, 120(a0)
	st	t0, 120(a1)
	lt	t0, 128(a0)
	st	t0, 128(a1)
	lt	t0, 136(a0)
	st	t0, 136(a1)
	lt	t0, 144(a0)
	st	t0, 144(a1)
	lt	t0, 152(a0)
	st	t0, 152(a1)

	lt	t0, 160(a0)
	st	t0, 160(a1)
	lt	t0, 168(a0)
	st	t0, 168(a1)
	lt	t0, 176(a0)
	st	t0, 176(a1)
	lt	t0, 184(a0)
	st	t0, 184(a1)
	lt	t0, 192(a0)
	st	t0, 192(a1)
	lt	t0, 200(a0)
	st	t0, 200(a1)
	lt	t0, 208(a0)
	st	t0, 208(a1)
	lt	t0, 216(a0)
	st	t0, 216(a1)
	lt	t0, 224(a0)
	st	t0, 224(a1)
	lt	t0, 232(a0)
	st	t0, 232(a1)

	lt	t0, 240(a0)
	st	t0, 240(a1)
	lt	t0, 248(a0)
	st	t0, 248(a1)
	lt	t0, 256(a0)
	st	t0, 256(a1)
	lt	t0, 264(a0)
	st	t0, 264(a1)
	lt	t0, 272(a0)
	st	t0, 272(a1)
	lt	t0, 280(a0)
	st	t0, 280(a1)
	lt	t0, 288(a0)
	st	t0, 288(a1)
	lt	t0, 296(a0)
	st	t0, 296(a1)
	lt	t0, 304(a0)
	st	t0, 304(a1)
	lt	t0, 312(a0)
	st	t0, 312(a1)

	lt	t0, 320(a0)
	st	t0, 320(a1)
	lt	t0, 328(a0)
	st	t0, 328(a1)
	lt	t0, 336(a0)
	st	t0, 336(a1)
	lt	t0, 344(a0)
	st	t0, 344(a1)
	lt	t0, 352(a0)
	st	t0, 352(a1)
	lt	t0, 360(a0)
	st	t0, 360(a1)
	lt	t0, 368(a0)
	st	t0, 368(a1)
	lt	t0, 376(a0)
	st	t0, 376(a1)
	lt	t0, 384(a0)
	st	t0, 384(a1)
	lt	t0, 392(a0)
	st	t0, 392(a1)

	lt	t0, 400(a0)
	st	t0, 400(a1)
	lt	t0, 408(a0)
	st	t0, 408(a1)
	lt	t0, 416(a0)
	st	t0, 416(a1)
	lt	t0, 424(a0)
	st	t0, 424(a1)
	lt	t0, 432(a0)
	st	t0, 432(a1)
	lt	t0, 440(a0)
	st	t0, 440(a1)
	lt	t0, 448(a0)
	st	t0, 448(a1)
	lt	t0, 456(a0)
	st	t0, 456(a1)
	lt	t0, 464(a0)
	st	t0, 464(a1)
	lt	t0, 472(a0)
	st	t0, 472(a1)


	START_CP3_COUNTER
        mtc0 zero, $20 #write back dcache
        la t1, 0x00000100 # bits 8 of COP0 reg 20
        mtc0 t1, $20
	STOP_CP3_COUNTER

	jr	ra
	nop
	.end DMEM_read_cp0_Flush_480_flush



	/* a0: DMEM Address, a1: DRAM Address, a2: counter mode */
	.global DMEM_read_cp0_Flush_480_total
        .text
        .ent DMEM_read_cp0_Flush_480_total
	.set noreorder
	.section ".myTest", "ax"
DMEM_read_cp0_Flush_480_total:

        mtc0 zero, $20 #write back invalidate dcache
        la t1, 0x00000200 # bits 8 of COP0 reg 20
        mtc0 t1, $20
	nop

	/*waiting for empty of write buffer*/
	li	t1, 10000
2:
	bne	zero, t1, 2b
	addi	t1, t1, -1

	START_CP3_COUNTER

	/* Read data into Dcache */
	move	t2, a1	     /* copy start address */
	addi	t0, a1, (480-CACHE_LINE_SIZE)  /* copy end address */
1:
	lw	t1, 0(t2)
	bne   	t0, t2, 1b
	addi   	t2, t2, CACHE_LINE_SIZE

	lt	t0, 0(a0)
	st	t0, 0(a1)
	lt	t0, 8(a0)
	st	t0, 8(a1)
	lt	t0, 16(a0)
	st	t0, 16(a1)
	lt	t0, 24(a0)
	st	t0, 24(a1)
	lt	t0, 32(a0)
	st	t0, 32(a1)
	lt	t0, 40(a0)
	st	t0, 40(a1)
	lt	t0, 48(a0)
	st	t0, 48(a1)
	lt	t0, 56(a0)
	st	t0, 56(a1)
	lt	t0, 64(a0)
	st	t0, 64(a1)
	lt	t0, 72(a0)
	st	t0, 72(a1)

	lt	t0, 80(a0)
	st	t0, 80(a1)
	lt	t0, 88(a0)
	st	t0, 88(a1)
	lt	t0, 96(a0)
	st	t0, 96(a1)
	lt	t0, 104(a0)
	st	t0, 104(a1)
	lt	t0, 112(a0)
	st	t0, 112(a1)
	lt	t0, 120(a0)
	st	t0, 120(a1)
	lt	t0, 128(a0)
	st	t0, 128(a1)
	lt	t0, 136(a0)
	st	t0, 136(a1)
	lt	t0, 144(a0)
	st	t0, 144(a1)
	lt	t0, 152(a0)
	st	t0, 152(a1)

	lt	t0, 160(a0)
	st	t0, 160(a1)
	lt	t0, 168(a0)
	st	t0, 168(a1)
	lt	t0, 176(a0)
	st	t0, 176(a1)
	lt	t0, 184(a0)
	st	t0, 184(a1)
	lt	t0, 192(a0)
	st	t0, 192(a1)
	lt	t0, 200(a0)
	st	t0, 200(a1)
	lt	t0, 208(a0)
	st	t0, 208(a1)
	lt	t0, 216(a0)
	st	t0, 216(a1)
	lt	t0, 224(a0)
	st	t0, 224(a1)
	lt	t0, 232(a0)
	st	t0, 232(a1)

	lt	t0, 240(a0)
	st	t0, 240(a1)
	lt	t0, 248(a0)
	st	t0, 248(a1)
	lt	t0, 256(a0)
	st	t0, 256(a1)
	lt	t0, 264(a0)
	st	t0, 264(a1)
	lt	t0, 272(a0)
	st	t0, 272(a1)
	lt	t0, 280(a0)
	st	t0, 280(a1)
	lt	t0, 288(a0)
	st	t0, 288(a1)
	lt	t0, 296(a0)
	st	t0, 296(a1)
	lt	t0, 304(a0)
	st	t0, 304(a1)
	lt	t0, 312(a0)
	st	t0, 312(a1)

	lt	t0, 320(a0)
	st	t0, 320(a1)
	lt	t0, 328(a0)
	st	t0, 328(a1)
	lt	t0, 336(a0)
	st	t0, 336(a1)
	lt	t0, 344(a0)
	st	t0, 344(a1)
	lt	t0, 352(a0)
	st	t0, 352(a1)
	lt	t0, 360(a0)
	st	t0, 360(a1)
	lt	t0, 368(a0)
	st	t0, 368(a1)
	lt	t0, 376(a0)
	st	t0, 376(a1)
	lt	t0, 384(a0)
	st	t0, 384(a1)
	lt	t0, 392(a0)
	st	t0, 392(a1)

	lt	t0, 400(a0)
	st	t0, 400(a1)
	lt	t0, 408(a0)
	st	t0, 408(a1)
	lt	t0, 416(a0)
	st	t0, 416(a1)
	lt	t0, 424(a0)
	st	t0, 424(a1)
	lt	t0, 432(a0)
	st	t0, 432(a1)
	lt	t0, 440(a0)
	st	t0, 440(a1)
	lt	t0, 448(a0)
	st	t0, 448(a1)
	lt	t0, 456(a0)
	st	t0, 456(a1)
	lt	t0, 464(a0)
	st	t0, 464(a1)
	lt	t0, 472(a0)
	st	t0, 472(a1)


        mtc0 zero, $20 #write back dcache
        la t1, 0x00000100 # bits 8 of COP0 reg 20
        mtc0 t1, $20
	STOP_CP3_COUNTER

	jr	ra
	nop
	.end DMEM_read_cp0_Flush_480_total


	/* a0: DMEM Address, a1: DRAM Address, a2: counter mode, a3:size */
	.global DMEM_read_cp0_Flush_total_with_size
        .text
        .ent DMEM_read_cp0_Flush_total_with_size
	.set noreorder
	.section ".myTest", "ax"
DMEM_read_cp0_Flush_total_with_size:

        mtc0 zero, $20 #write back invalidate dcache
        la t1, 0x00000200 # bits 8 of COP0 reg 20
        mtc0 t1, $20
	nop

	/*waiting for empty of write buffer*/
	li	t1, 10000
2:
	bne	zero, t1, 2b
	addi	t1, t1, -1


	START_CP3_COUNTER
	/* Read data into Dcache */
	addi	t6, a3, (-CACHE_LINE_SIZE)
	move	t2, a1	     /* copy start address */
	add	t0, a1, t6  /* copy end address */
1:
	lw	t1, 0(t2)
	sw	t2, 0(t2)
	bne   	t0, t2, 1b
	addi   	t2, t2, CACHE_LINE_SIZE


	/* copy DMEM data into Dcache */
	addi	a3, a3, (-8)
	move	t2, a1	     /* copy start address */
	add	t0, a1, a3  /* copy end address */
1:
	lt	t4, 0(a0)
	add	a0, a0, 8
	st	t4, 0(t2)
	bne   	t0, t2, 1b
	addi   	t2, t2, 8

        mtc0 zero, $20 #write back dcache
        la t1, 0x00000100 # bits 8 of COP0 reg 20
        mtc0 t1, $20
	STOP_CP3_COUNTER

	jr	ra
	nop
	.end DMEM_read_cp0_Flush_total_with_size


	/* a0: DMEM Address, a1: DRAM Address, a2: counter mode, a3:size */
	.global DMEM_read_Cache_Ins_Flush_total_with_size
        .text
        .ent DMEM_read_Cache_Ins_Flush_total_with_size
	.set noreorder
	.section ".myTest", "ax"
DMEM_read_Cache_Ins_Flush_total_with_size:

        mtc0 zero, $20 #write back invalidate dcache
        la t1, 0x00000200 # bits 8 of COP0 reg 20
        mtc0 t1, $20
	nop

	/*waiting for empty of write buffer*/
	li	t1, 10000
2:
	bne	zero, t1, 2b
	addi	t1, t1, -1


	START_CP3_COUNTER
	/* Read data into Dcache */
	addi	t6, a3, (-CACHE_LINE_SIZE)
	move	t2, a1	     /* copy start address */
	add	t0, a1, t6  /* copy end address */
1:
	lw	t1, 0(t2)
	sw	t2, 0(t2)
	bne   	t0, t2, 1b
	addi   	t2, t2, CACHE_LINE_SIZE


	/* copy DMEM data into Dcache */
	addi	t6, a3, (-8)
	move	t2, a1	     /* copy start address */
	add	t0, a1,t6  /* copy end address */
1:
	lt	t4, 0(a0)
	add	a0, a0, 8
	st	t4, 0(t2)
	bne   	t0, t2, 1b
	addi   	t2, t2, 8

	add	t0, a1, a3	
1:
	cache 	0x19, 0(a1)
	bne   	t0, a1, 1b
	add   	a1, a1, CACHE_LINE_SIZE
	STOP_CP3_COUNTER

	jr	ra
	nop
	.end DMEM_read_Cache_Ins_Flush_total_with_size



	/* a0: DMEM Address, a1: DRAM Address, a2: counter mode, a3:size */
	.global DMEM_read_Cache_Ins_Flush_total_with_size_WA
        .text
        .ent DMEM_read_Cache_Ins_Flush_total_with_size_WA
	.set noreorder
	.section ".myTest", "ax"
DMEM_read_Cache_Ins_Flush_total_with_size_WA:

        mtc0 zero, $20 #write back invalidate dcache
        la t1, 0x00000200 # bits 8 of COP0 reg 20
        mtc0 t1, $20
	nop

	/*waiting for empty of write buffer*/
	li	t1, 10000
2:
	bne	zero, t1, 2b
	addi	t1, t1, -1

	/* Enable Write Allocation */
        mtc0 zero, $20 #write back invalidate dcache
        la t1, 0x00000080 # bits 8 of COP0 reg 20
        mtc0 t1, $20
	nop

	START_CP3_COUNTER


	/* Copy DMEM data into DCache */
	addi	t6, a3, (-8)
	move	t2, a1	     /* copy start address */
	add	t0, a1,t6  /* copy end address */
1:
	lt	t4, 0(a0)
	add	a0, a0, 8
	st	t4, 0(t2)
	bne   	t0, t2, 1b
	addi   	t2, t2, 8

	add	t0, a1, a3	
1:
	cache 	0x19, 0(a1)
	bne   	t0, a1, 1b
	add   	a1, a1, CACHE_LINE_SIZE
	STOP_CP3_COUNTER


        mtc0 zero, $20 #write back invalidate dcache
        la t1, 0x00000080 # bits 8 of COP0 reg 20
        mtc0 t1, $20
	nop
	
	/* Disable Write Allocation */
        mtc0 zero, $20 #write back invalidate dcache
        la t1, 0x00008000 # bits 8 of COP0 reg 20
        mtc0 t1, $20
	nop

	jr	ra
	nop
	.end DMEM_read_Cache_Ins_Flush_total_with_size_WA


	/* a0: DMEM Address, a1: DRAM Address, a2: counter mode */
	.global DMEM_read_cp0_Flush_no_data
        .text
        .ent DMEM_read_cp0_Flush_no_data
	.set noreorder
	.section ".myTest", "ax"
DMEM_read_cp0_Flush_no_data:

        mtc0 zero, $20 #write back invalidate dcache
        la t1, 0x00000200 # bits 8 of COP0 reg 20
        mtc0 t1, $20
	nop

	/*waiting for empty of write buffer*/
	li	t1, 10000
2:
	bne	zero, t1, 2b
	addi	t1, t1, -1
	
	START_CP3_COUNTER
        mtc0 zero, $20 #write back dcache
        la t1, 0x00000100 # bits 8 of COP0 reg 20
        mtc0 t1, $20
	STOP_CP3_COUNTER

	jr	ra
	nop
	.end DMEM_read_cp0_Flush_no_data
