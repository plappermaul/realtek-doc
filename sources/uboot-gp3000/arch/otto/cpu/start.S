/* SPDX-License-Identifier: GPL-2.0+ */
/*
 *  Startup Code for MIPS32 CPU-core
 *
 *  Copyright (c) 2003	Wolfgang Denk <wd@denx.de>
 */

#include <asm-offsets.h>
#include <config.h>
#include <asm/asm.h>
#include <asm/regdef.h>
#include <asm/mipsregs.h>

#ifndef CONFIG_SYS_INIT_SP_ADDR
#define CONFIG_SYS_INIT_SP_ADDR	(CONFIG_SYS_SDRAM_BASE + \
				CONFIG_SYS_INIT_SP_OFFSET)
#endif

	.set noreorder

	.macro setup_stack_gd
	li	t0, -16
	PTR_LI	t1, CONFIG_SYS_INIT_SP_ADDR
	and	sp, t1, t0		# force 16 byte alignment
	PTR_SUBU \
		sp, sp, GD_SIZE		# reserve space for gd
	and	sp, sp, t0		# force 16 byte alignment
	move	k0, sp			# save gd pointer
#if CONFIG_VAL(SYS_MALLOC_F_LEN)
	li	t2, CONFIG_VAL(SYS_MALLOC_F_LEN)
	PTR_SUBU \
		sp, sp, t2		# reserve space for early malloc
	and	sp, sp, t0		# force 16 byte alignment
#endif
	move	fp, sp

	/* Clear gd */
	move	t0, k0
1:
	PTR_S	zero, 0(t0)
	blt	t0, t1, 1b
	PTR_ADDIU t0, PTRSIZE

#if CONFIG_VAL(SYS_MALLOC_F_LEN)
	PTR_S	sp, GD_MALLOC_BASE(k0)	# gd->malloc_base offset
#endif
	.endm

ENTRY(_start)
	/* U-Boot entry point */
	b	reset
	mtc0	zero, CP0_COUNT	# clear cp0 count for most accurate boot timing

#if defined(CONFIG_MIPS_INSERT_BOOT_CONFIG)
	/*
	 * Store some board-specific boot configuration. This is used by some
	 * MIPS systems like Malta.
	 */
	.org 0x10
	.word CONFIG_MIPS_BOOT_CONFIG_WORD0
	.word CONFIG_MIPS_BOOT_CONFIG_WORD1
#endif

#if defined(CONFIG_ROM_EXCEPTION_VECTORS)
	/*
	 * Exception vector entry points. When running from ROM, an exception
	 * cannot be handled. Halt execution and transfer control to debugger,
	 * if one is attached.
	 */
	.org 0x200
	/* TLB refill, 32 bit task */
	uhi_mips_exception

	.org 0x280
	/* XTLB refill, 64 bit task */
	uhi_mips_exception

	.org 0x300
	/* Cache error exception */
	uhi_mips_exception

	.org 0x380
	/* General exception */
	uhi_mips_exception

	.org 0x400
	/* Catch interrupt exceptions */
	uhi_mips_exception

	.org 0x480
	/* EJTAG debug exception */
1:	b	1b
	 nop

	.org 0x500
#endif

reset:
	/* Set up initial stack and global data */
	setup_stack_gd

	/* init. OSC in U-Boot side */
	PTR_LA  t9, osc_init
	jalr    t9
	nop

	move	a0, zero		# a0 <-- boot_flags = 0
	PTR_LA	t9, board_init_f

	jr	t9
	 move	ra, zero

	END(_start)
