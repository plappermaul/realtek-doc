#include <asm/regdef.h>

#define MFC0_SEL(dst, src, sel)  .word (0x40000000 | ((dst) << 16) | ((src) << 11) | (sel))
#define MTC0_SEL(dst, src, sel)  .word (0x40800000 | ((dst) << 16) | ((src) << 11) | (sel))

	.ent enable_CP3
	.globl enable_CP3
	.set noreorder
enable_CP3:
	addiu sp, sp, -8
	sw t1, 0(sp)
	sw t2, 4(sp)

        mfc0 t1, $12
        nop
        la t2, 0x80000000
        or t1, t2
        mtc0 t1, $12
        nop
        nop
	
	lw t1, 0(sp)
	lw t2, 4(sp)
	addiu sp, sp, 8
	jr ra
	nop
	.end enable_CP3

	.ent disable_IMEM0
	.globl disable_IMEM0
disable_IMEM0:
	addiu sp, sp, -4
	sw t1, 0(sp)

        mtc0 zero, $20
        la t1, 0x00000020 # bit 5 of COP0 reg 20, sel = 0
        mtc0 t1, $20
        nop
        nop

	lw t1, 0(sp)
	addiu sp, sp, 4
	jr ra
	nop
	.end disable_IMEM0


	.ent disable_IMEM1
	.globl disable_IMEM1
disable_IMEM1:
	addiu sp, sp, -4
	sw t1, 0(sp)

	MTC0_SEL(0, 20, 1) #mtc0 zero, $20
       	la t1, 0x00000020 # bit 5 of COP0 reg 20, sel = 1
	MTC0_SEL(9, 20, 1) #mtc0 t1, $20
       	nop
       	nop

	lw t1, 0(sp)
	addiu sp, sp, 4
	jr ra
	nop
	.end disable_IMEM1

	.ent disable_DMEM0
	.globl disable_DMEM0
disable_DMEM0:
	addiu sp, sp, -4
	sw t1, 0(sp)

        mtc0 zero, $20
        la t1, 0x00000800 # bit 11 of COP0 reg 20, sel = 0
        mtc0 t1, $20
        nop
        nop

	lw t1, 0(sp)
	addiu sp, sp, 4
	jr ra
	nop
	.end disable_DMEM0

	.ent disable_DMEM1
	.globl disable_DMEM1
disable_DMEM1:
	addiu sp, sp, -4
	sw t1, 0(sp)

       	//mtc0 zero, $20
	MTC0_SEL(0, 20, 1)
        la t1, 0x00000800 # bit 11 of COP0 reg 20, sel = 1
        //mtc0 t1, $20
	MTC0_SEL(9, 20, 1)
        nop
        nop

	lw t1, 0(sp)
	addiu sp, sp, 4
	jr ra
	nop
	.end disable_DMEM1


	.ent disable_L2MEM
	.globl disable_L2MEM
disable_L2MEM:
	addiu sp, sp, -4
	sw t1, 0(sp)

        //mtc0 zero, $20
	MTC0_SEL(0, 20, 1)
        la t1, 0x08000000 # bit 27 of COP0 reg 20, sel = 1
        //mtc0 t1, $20
	MTC0_SEL(9, 20, 1)
        nop
        nop

	lw t1, 0(sp)
	addiu sp, sp, 4
	jr ra
	nop
	.end disable_L2MEM


	.ent enable_DMEM0
	.globl enable_DMEM0
enable_DMEM0:
	addiu sp, sp, -4
	sw t1, 0(sp)

        mtc0 zero, $20
        la t1, 0x00000400 # bit 10 of COP0 reg 20
        mtc0 t1, $20
        nop
        nop

	lw t1, 0(sp)
	addiu sp, sp, 4
	jr ra
	nop
	.end enable_DMEM0

	.ent enable_DMEM1
	.globl enable_DMEM1
enable_DMEM1:
	addiu sp, sp, -4
	sw t1, 0(sp)

        //mtc0 zero, $20
	MTC0_SEL(0, 20, 1)
        la t1, 0x00000400 # bit 10 of COP0 reg 20
        //mtc0 t1, $20
	MTC0_SEL(9, 20, 1)
        nop
        nop

	lw t1, 0(sp)
	addiu sp, sp, 4
	jr ra
	nop
	.end enable_DMEM1

	.ent enable_IMEM0
	.globl enable_IMEM0
enable_IMEM0:
	addiu sp, sp, -4
	sw t1, 0(sp)

        mtc0 zero, $20
        la t1, 0x00000040 # bit 6 of COP0 reg 20
        mtc0 t1, $20
        nop
        nop

	lw t1, 0(sp)
	addiu sp, sp, 4
	jr ra
	nop
	.end enable_IMEM0

	.ent enable_IMEM1
	.globl enable_IMEM1
enable_IMEM1:
	addiu sp, sp, -4
	sw t1, 0(sp)

        //mtc0 zero, $20
	MTC0_SEL(0, 20, 1)
        la t1, 0x00000040 # bit 6 of COP0 reg 20
        //mtc0 t1, $20
	MTC0_SEL(9, 20, 1)
        nop
        nop

	lw t1, 0(sp)
	addiu sp, sp, 4
	jr ra
	nop
	.end enable_IMEM1


	.ent enable_L2MEM
	.globl enable_L2MEM
enable_L2MEM:
	addiu sp, sp, -4
	sw t1, 0(sp)

        //mtc0 zero, $20
	MTC0_SEL(0, 20, 1)
        la t1, 0x04000000 # bit 26 of COP0 reg 20, sel = 1
        //mtc0 t1, $20
	MTC0_SEL(9, 20, 1)
        nop
        nop

	lw t1, 0(sp)
	addiu sp, sp, 4
	jr ra
	nop
	.end enable_L2MEM


	/*a0:dram top*/
	/*a1:dram base*/
	.ent load_l2memRange
	.globl load_l2memRange
load_l2memRange:
	addiu sp, sp, -8
	sw t1, 0(sp)	
	sw t2, 4(sp)	

        move t1, a0
        move t2, a1
        mtc3 t1, $17 # IW top
        mtc3 t2, $16 # IW base
        nop
        nop

	lw t1, 0(sp)
	lw t2, 4(sp)
	addiu sp, sp, 8
	jr ra
	nop
	.end load_l2memRange


	/*a0:dram top*/
	/*a1:dram base*/
	.ent load_dmem0Range
	.globl load_dmem0Range
load_dmem0Range:
	addiu sp, sp, -8
	sw t1, 0(sp)	
	sw t2, 4(sp)	

        move t1, a0
        move t2, a1
        mtc3 t1, $5 # IW top
        mtc3 t2, $4 # IW base
        nop
        nop

        mfc3 t1, $5 # IW top
        mfc3 t2, $4 # IW base
	nop
	nop

	lw t1, 0(sp)
	lw t2, 4(sp)
	addiu sp, sp, 8
	jr ra
	nop
	.end load_dmem0Range


	/*a0:dram top*/
	/*a1:dram base*/
	.ent load_dmem1Range
	.globl load_dmem1Range
load_dmem1Range:
	addiu sp, sp, -8
	sw t1, 0(sp)	
	sw t2, 4(sp)	

        move t1, a0
        move t2, a1
        mtc3 t1, $7 # IW top
        mtc3 t2, $6 # IW base
        nop
        nop

	lw t1, 0(sp)
	lw t2, 4(sp)
	addiu sp, sp, 8
	jr ra
	nop
	.end load_dmem1Range


	/*a0:dram top*/
	/*a1:dram base*/
	.ent load_imem0Range
	.globl load_imem0Range
load_imem0Range:
	addiu sp, sp, -8
	sw t1, 0(sp)	
	sw t2, 4(sp)	

        move t1, a0
        move t2, a1
        mtc3 t1, $1 # IW top
        mtc3 t2, $0 # IW base
        nop
        nop

	lw t1, 0(sp)
	lw t2, 4(sp)
	addiu sp, sp, 8
	jr ra
	nop
	.end load_dmem0Range


	/*a0:iram top*/
	/*a1:iram base*/
	.ent load_imem1Range
	.globl load_imem1Range
load_imem1Range:
	addiu sp, sp, -8
	sw t1, 0(sp)	
	sw t2, 4(sp)	

        move t1, a0
        move t2, a1
        mtc3 t1, $3 # IW top
        mtc3 t2, $2 # IW base
        nop
        nop

	lw t1, 0(sp)
	lw t2, 4(sp)
	addiu sp, sp, 8
	jr ra
	nop
	.end load_imem1Range

	.ent IMEM0_refill
	.globl IMEM0_refill
IMEM0_refill:
	addiu sp, sp, -4
	sw t1, 0(sp)
	
        mtc0 zero, $20
        la t1, 0x00000010 # bit 4 of COP0 reg 20
        mtc0 t1, $20
        nop
        nop
	
	lw t1, 0(sp)
	addiu sp, sp, 4
	jr ra
	nop
	.end IMEM0_refill

	.ent IMEM1_refill
	.globl IMEM1_refill
IMEM1_refill:
	addiu sp, sp, -4
	sw t1, 0(sp)

        //mtc0 zero, $20
	MTC0_SEL(0, 20, 1)
        la t1, 0x00000010 # bit 4 of COP0 reg 20, sel = 1
        //mtc0 t1, $20
	MTC0_SEL(9, 20, 1)
        nop
        nop

	lw t1, 0(sp)
	addiu sp, sp, 4
	jr ra
	nop
	.end IMEM1_refill


/* big endian, a0: start address, a1: address of buffer */
	.ent read_imem_8bytes
	.globl read_imem_8bytes
read_imem_8bytes:
	addiu sp, sp, -8
	sw t1, 0(sp)	
	sw t2, 4(sp)	

	cache 6, 0(a0)
	/*read DATAHI*/
	mfc0  t1, $29 	
	/*read DATALOW*/
	mfc0  t2, $28
	sw    t1, 4(a1)	
	sw    t2, 0(a1)	

	lw t1, 0(sp)
	lw t2, 4(sp)
	addiu sp, sp, 8
	jr ra
	nop
	.end read_imem_8bytes


/* big endian, a0: High order 4 bytes, a1: Low order 4 bytes, a2: start addr */
	.ent write_imem_8bytes
	.globl write_imem_8bytes
write_imem_8bytes:

	/*write DATAHI*/
	mtc0  a0, $29
	nop
	/*write DATALOW*/
	mtc0  a1, $28
	nop
	cache 10, 0(a2)
	nop

	jr ra
	nop
	.end write_imem_8bytes


	.ent _read_cp0_pid
	.globl _read_cp0_pid
_read_cp0_pid:
	/*write DATAHI*/
	mfc0	v0, $15 /* PRID reg */
	nop
	jr ra
	nop
	.end _read_cp0_pid
