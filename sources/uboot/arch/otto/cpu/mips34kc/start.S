/*
 *  Startup Code for MIPS32 CPU-core
 *
 *  Copyright (c) 2003	Wolfgang Denk <wd@denx.de>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <asm/regdef.h>
#include <asm/mipsregs.h>
#include <configs/autoconf.h>

#define C_SR   0
#define C_EPC  1
#define C_AT   2
#define C_V0   3
#define C_V1   4
#define C_A0   5
#define C_A1   6
#define C_A2   7
#define C_A3   8
#define C_T0   9
#define C_T1   10
#define C_T2   11
#define C_T3   12
#define C_T4   13
#define C_T5   14
#define C_T6   15
#define C_T7   16
#define C_T8   17
#define C_T9   18
#define C_RA   19
#define C_LO   20
#define C_HI   21
#define C_SIZE 22

	.macro SAVE_ALL
	.set push
	.set noat                          
	.set reorder
	subu  sp, C_SIZE*4
	sw    $1, C_AT*4(sp)               
	sw    v0, C_V0*4(sp)               
	sw    v1, C_V1*4(sp)               
	sw    a0, C_A0*4(sp)               
	sw    a1, C_A1*4(sp)               
	sw    a2, C_A2*4(sp)               
	sw    a3, C_A3*4(sp)               
	sw    t0, C_T0*4(sp)               
	sw    t1, C_T1*4(sp)               
	sw    t2, C_T2*4(sp)               
	sw    t3, C_T3*4(sp)               
	sw    t4, C_T4*4(sp)               
	sw    t5, C_T5*4(sp)               
	sw    t6, C_T6*4(sp)               
	sw    t7, C_T7*4(sp)               
	sw    t8, C_T8*4(sp)               
	sw    t9, C_T9*4(sp)               
	sw    ra, C_RA*4(sp)               
	mflo  k1
	sw    k1, C_LO*4(sp)               
	mfhi  k1                           
	sw    k1, C_HI*4(sp) 
	.set pop
	.endm

	.macro RESTORE_ALL                  
	.set push
	.set noat                          
	.set reorder
	lw    $1, C_AT*4(sp)               
	lw    v0, C_V0*4(sp)               
	lw    v1, C_V1*4(sp)               
	lw    a0, C_A0*4(sp)               
	lw    a1, C_A1*4(sp)               
	lw    a2, C_A2*4(sp)              
	lw    a3, C_A3*4(sp)               
	lw    t1, C_T1*4(sp)               
	lw    t2, C_T2*4(sp)               
	lw    t3, C_T3*4(sp)               
	lw    t4, C_T4*4(sp)               
	lw    t5, C_T5*4(sp)               
	lw    t6, C_T6*4(sp)               
	lw    t7, C_T7*4(sp)               
	lw    t8, C_T8*4(sp)               
	lw    t9, C_T9*4(sp)               
	lw    ra, C_RA*4(sp)               
	lw    k1, C_LO*4(sp)               
	mtlo  k1                        
	lw    k1, C_HI*4(sp)               
	mfhi  k1 
	li    t0, 1
	mfc0  k1, CP0_STATUS
	ins   k1, t0, 0, 1
	mtc0  k1, CP0_STATUS
	lw    t0, C_T0*4(sp)               
	addiu  sp, C_SIZE*4
	.set pop
	.endm


#define RVECENT(f,n) \
   b f; nop
#define XVECENT(f,bev) \
   b f     ;           \
   li k0,bev

	.set noreorder

	.globl _start
	.text
_start:
	RVECENT(reset,0)	/* U-boot entry point */
	RVECENT(reset,1)	/* software reboot */
	RVECENT(romReserved,2)
	RVECENT(romReserved,3)
	RVECENT(romReserved,4)
	RVECENT(romReserved,5)
	RVECENT(romReserved,6)
	RVECENT(romReserved,7)
	RVECENT(romReserved,8)
	RVECENT(romReserved,9)
	RVECENT(romReserved,10)
	RVECENT(romReserved,11)
	RVECENT(romReserved,12)
	RVECENT(romReserved,13)
	RVECENT(romReserved,14)
	RVECENT(romReserved,15)
	RVECENT(ramExcHandle,0x80)
	RVECENT(romReserved,17)
	RVECENT(romReserved,18)
	RVECENT(romReserved,19)
	RVECENT(romReserved,20)
	RVECENT(romReserved,21)
	RVECENT(romReserved,22)
	RVECENT(romReserved,23)
	RVECENT(romReserved,24)
	RVECENT(romReserved,25)
	RVECENT(romReserved,26)
	RVECENT(romReserved,27)
	RVECENT(romReserved,28)
	RVECENT(romReserved,29)
	RVECENT(romReserved,30)
	RVECENT(romReserved,31)
	RVECENT(romReserved,32)
	RVECENT(romReserved,33)
	RVECENT(romReserved,34)
	RVECENT(romReserved,35)
	RVECENT(romReserved,36)
	RVECENT(romReserved,37)
	RVECENT(romReserved,38)
	RVECENT(romReserved,39)
	RVECENT(romReserved,40)
	RVECENT(romReserved,41)
	RVECENT(romReserved,42)
	RVECENT(romReserved,43)
	RVECENT(romReserved,44)
	RVECENT(romReserved,45)
	RVECENT(romReserved,46)
	RVECENT(romReserved,47)
	la k1, intHandle
	j k1
	nop
	RVECENT(romReserved,50)
	RVECENT(romReserved,51)
	RVECENT(romReserved,52)
	RVECENT(romReserved,53)
	RVECENT(romReserved,54)
	RVECENT(romReserved,55)
	RVECENT(romReserved,56)
	RVECENT(romReserved,57)
	RVECENT(romReserved,58)
	RVECENT(romReserved,59)
	RVECENT(romReserved,60)
	RVECENT(romReserved,61)
	RVECENT(romReserved,62)
	RVECENT(romReserved,63)
	XVECENT(romExcHandle,0x200)	/* bfc00200: R4000 tlbmiss vector */
	RVECENT(romReserved,65)
	RVECENT(romReserved,66)
	RVECENT(romReserved,67)
	RVECENT(romReserved,68)
	RVECENT(romReserved,69)
	RVECENT(romReserved,70)
	RVECENT(romReserved,71)
	RVECENT(romReserved,72)
	RVECENT(romReserved,73)
	RVECENT(romReserved,74)
	RVECENT(romReserved,75)
	RVECENT(romReserved,76)
	RVECENT(romReserved,77)
	RVECENT(romReserved,78)
	RVECENT(romReserved,79)
	XVECENT(romExcHandle,0x280)	/* bfc00280: R4000 xtlbmiss vector */
	RVECENT(romReserved,81)
	RVECENT(romReserved,82)
	RVECENT(romReserved,83)
	RVECENT(romReserved,84)
	RVECENT(romReserved,85)
	RVECENT(romReserved,86)
	RVECENT(romReserved,87)
	RVECENT(romReserved,88)
	RVECENT(romReserved,89)
	RVECENT(romReserved,90)
	RVECENT(romReserved,91)
	RVECENT(romReserved,92)
	RVECENT(romReserved,93)
	RVECENT(romReserved,94)
	RVECENT(romReserved,95)
	XVECENT(romExcHandle,0x300)	/* bfc00300: R4000 cache vector */
	RVECENT(romReserved,97)
	RVECENT(romReserved,98)
	RVECENT(romReserved,99)
	RVECENT(romReserved,100)
	RVECENT(romReserved,101)
	RVECENT(romReserved,102)
	RVECENT(romReserved,103)
	RVECENT(romReserved,104)
	RVECENT(romReserved,105)
	RVECENT(romReserved,106)
	RVECENT(romReserved,107)
	RVECENT(romReserved,108)
	RVECENT(romReserved,109)
	RVECENT(romReserved,110)
	RVECENT(romReserved,111)
	XVECENT(romExcHandle,0x380)	/* bfc00380: R4000 general vector */
	RVECENT(romReserved,113)
	RVECENT(romReserved,114)
	RVECENT(romReserved,115)
	RVECENT(romReserved,116)
	RVECENT(romReserved,116)
	RVECENT(romReserved,118)
	RVECENT(romReserved,119)
	RVECENT(romReserved,120)
	RVECENT(romReserved,121)
	RVECENT(romReserved,122)
	RVECENT(romReserved,123)
	RVECENT(romReserved,124)
	RVECENT(romReserved,125)
	RVECENT(romReserved,126)
	RVECENT(romReserved,127)

#include "soc_init.H"

	/* We hope there are no more reserved vectors!
	 * 128 * 8 == 1024 == 0x400
	 * so this is address R_VEC+0x400 == 0xbfc00400
	 */
	.align 4
reset:
	li	sp, CONFIG_SYS_TEXT_BASE - 8
	la	t9, soc_init
	j		t9
	nop

/* Exception handlers. */
romReserved:
	b       romReserved
	nop

romExcHandle:
	b       romExcHandle

intHandle:
#ifdef CONFIG_INTERRUPT_SUPPORT
	mfc0 k1, CP0_STATUS         /* Clear Interrupt */
	ins k1, zero, 0, 1
	mtc0 k1, CP0_STATUS
	SAVE_ALL
	.set reorder
	la  t9, common_irq_dispatch /* Interrupt Entry */
	jal t9
	.set noreorder
	RESTORE_ALL
	eret
#endif	
	b  intHandle
	nop

ramExcHandle:
	b       ramExcHandle
	nop
