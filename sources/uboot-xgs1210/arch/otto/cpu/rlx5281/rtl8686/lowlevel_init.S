#include <config.h>
#include <asm/asm.h>
#include <asm/regdef.h>
#include <asm/mipsregs.h>
#include <asm/arch/bspchip.h>
#include <asm/arch/memctl.h>

#include "macro.s"
	.set push
	.set noreorder
	.set nomips16
	
LEAF(lowlevel_init)
	/* Disable useless IPs */
	li	t0, SYSREG_IP_EN_CTL_REG
	li	t1, 0x8302   /* Enable GDMA0, GDMA1 and GMAC, NAND flash controller */
	sw	t1, 0(t0)

	/* Fine tune DDR LDO output */
	li	t0, SYSREG_ANA1_CTL_REG
	lw	t1, 0(t0)
	li	t2, ~(7 << 10) #Mask off 
	and	t1, t1, t2
	li	t2, (3 << 10)  #set 3'b011
	or	t1, t1, t2
	sw	t1, 0(t0)

        /* Enable flash mapping 0xbd000000 */
        li      t0, MCR
        lw      t1, 0(t0)
        li      t2, (~(1<<18))
        and     t1, t1, t2
        sw      t1, 0(t0)
        nop

        /* Speed Up SPI clock */
        #li      t0, 0xB8001200
	#li	 t1, 0x5fc00000
        #sw      t1, 0(t0)
        #nop


        /* Configure Zone 0 base address */
        li      t0, 0xB8001700
        #li     t1, 0x2000000
        #li     t2, 0x3ffffff
        #sw     t1, 0(t0)
        #sw     t2, 4(t0)
        sw      zero, 0(t0)
        nop

	/* Configure DMEM mapping to tempary stack */
        /* Enable CP3 */
        mfc0 t1, $12
        nop
        la t2, 0x80000000
        or t1, t2
        mtc0 t1, $12
        nop
        nop

        /* Disable DMEM 0 */
        mtc0 zero, $20
        la t1, 0x00000800 # bit 11 of COP0 reg 20, sel = 0
        mtc0 t1, $20
        nop
        nop

        li  a0, 0x10003fff
        li  a1, 0x10000000

        /* Load DMEM0 range */
        move t1, a0
        move t2, a1
        mtc3 t1, $5 # IW top
        mtc3 t2, $4 # IW base
        nop
        nop

        /* Enable DMEM0 */
        mtc0 zero, $20
        la t1, 0x00000400 # bit 10 of COP0 reg 20
        mtc0 t1, $20
        nop
        nop

	jr		ra
	nop	
END(lowlevel_init)
	.set pop



