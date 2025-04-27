#include <linux/config.h>

#include <asm/asm.h>
#include <asm/mipsregs.h>
#include <asm/regdef.h>
#include <asm/stackframe.h>

/*
 *   RTL8316S IRQ        Source
 *   -----------        ------
 *             0        Software (ignored)
 *             1        Software (ignored)
 *             2        Hardware
 *             3        Hardware
 *             4        Hardware
 *             5        Hardware
 *             6        Hardware
 *             7        Hardware
 */

   .text
   .set    noreorder
   .set    noat
   .align  5
   NESTED(rtl8316sIRQ, PT_SIZE, sp)
   SAVE_ALL
   CLI
   .set    at
   move	a0, sp
   jal	irq_dispatch
   nop				# delay slot
   j	ret_from_irq
   nop				# delay slot
irq_bad:
   j	irq_bad
   nop
   END(rtl8316sIRQ)
