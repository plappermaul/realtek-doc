#include <config.h>
#include <asm/asm.h>
#include <asm/regdef.h>
#include <asm/mipsregs.h>
#include <asm/arch/bspchip.h>

	.set push
	.set noreorder
	.set nomips16
	
        .text
        .set noreorder
        .globl  _sys_adj_mem_clk
	.ent _sys_adj_mem_clk
_sys_adj_mem_clk:
	# We skip the PLL adjustment
        # when it's areadly the target freq.

	li	t0, SYSREG_SYSCLK_CONTROL_REG
	lw	t5, 0(t0)
	nop
	bne	a0, t5, 1f
	nop

	#what if it is the defaul value 0?
	#jr	ra
	#nop

1:
	#OK , we decide to adjust PLL setting
	# (1) CPU CLOCK select LX PLL clock
	li 	t6, SYSREG_SYSTEM_STATUS_REG
	lw	t7, 0(t6)
	li	t8, ~SYSREG_SYSTEM_STATUS_CF_CKSE_OCP0_MASK
	and	t7, t7, t8
	sw	t7, 0(t6)

	# (2) Delay a little bit after switching OCP0 clock to LX clock.
	li	t3, 4000
1:
	add	t3, t3, -1
	bne	zero, t3, 1b
	nop

	
	# (3) Invoke the PLL change.
	sw	a0, 0(t0)
2:
	lw	t1, 0(t0)
	bne	a0, t1, 2b
	nop	


	# (4) Set MEMCLK Phase 90 to 90
	# 0:45, 1:62.5, 2:90, 3:112.5, 4~7:135
	li	t0, SYSREG_MCKG_PHS_SEL_REG
	sw	a1, 0(t0)
2:
	lw	t1, 0(t0)
	bne	a1, t1, 2b
	nop
	

	# (5) Set MEMCLK Clock divison
	# 0: 1, 1: div 2, 2: div 4, 3:div 8
	li	t0, SYSREG_MCKG_FREQ_DIV_REG
	sw	a2, 0(t0)
2:
	lw	t1, 0(t0)
	bne	a2, t1, 2b
	nop


	# (6) Delay 4ms for stable clock.
	li	t0, 0x40000
1:
	add	t0, t0, -1
	bne	zero, t0, 1b
	nop
	

	# (7) CPU CLOCK select OCP0 PLL clock
	li 	t0, SYSREG_SYSTEM_STATUS_REG
	lw	t1, 0(t0)
	li	t2, SYSREG_SYSTEM_STATUS_CF_CKSE_OCP0_MASK
	or	t1, t1, t2
	sw	t1, 0(t0)
2:
	lw	t3, 0(t0)
	bne	t1, t3, 2b
	nop


	# Delay 4ms 
	li	t0, 4000
1:
	add	t0, t0, -1
	bne	zero, t0, 1b
	nop

	#reinit CPU again
	#li	t9, 0xbfc00000
	#jr	t9
	#nop

	jr	ra
	nop
_sys_adj_mem_clk_end:
	.end _sys_adj_mem_clk


        .text
        .set noreorder
        .globl  sys_adj_mem_clk
	.ent sys_adj_mem_clk
sys_adj_mem_clk:
	#a0: sys_pll_value  (cpu/dsp/mem clock)
	#a1: phase 90 value (mem clk phase 90)
	#a2: MCKG div value (mem clk divider)
	#a3: Mapped SRAM address, where the _sys_adj_mem_clk is copyed to.
        /* Runtime deterministic PLL intialization.
         * Enter PLL initialization if it isn't in DRAM.
         * Determine whether we run in DRAM.
         */
         /* If we run in DRAM, skip DRAM initialization.*/
        la      t0, _sys_adj_mem_clk
        li      t1, 0xF0000000
        li      t2, 0x80000000
        li      t3, 0xA0000000
        and     t1, t1, t0
        beq     t1, t2, skip_pll_init
        nop
        beq     t1, t3, skip_pll_init
        nop

	/* copy adj codes */
	move	t0, a3
	la	t1, _sys_adj_mem_clk
	la	t2, _sys_adj_mem_clk_end
1:
	lw	t3, 0(t1)
	add	t1, t1, 4
	sw	t3, 0(t0)
	add	t0, t0, 4
	bne	t1, t2, 1b
	nop

	jr	a3
	nop
skip_pll_init:
	jr	ra
	nop

	.end sys_adj_mem_clk

