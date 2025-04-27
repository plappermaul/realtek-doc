#include <config.h>
#include <version.h>
#include <asm/regdef.h>
#include <asm/mipsregs.h>
#include <asm/addrspace.h>

/********************************************
 * 
 * Read status register in CP0.
 *
 ********************************************/

        .globl CP0_status_read
        .ent CP0_status_read
CP0_status_read:
	mfc0	v0, $12
        jr ra
        nop
        .end CP0_status_write


/********************************************
 * 
 * Write status register in CP0.
 *
 ********************************************/

        .globl CP0_status_write
        .ent CP0_status_write
CP0_status_write:
	mtc0	a0, $12
        jr ra
        nop
        .end CP0_status_write


