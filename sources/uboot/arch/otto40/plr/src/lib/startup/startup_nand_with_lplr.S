#include <soc.h>
    .section .reset_entry, "ax", @progbits
    .align 4
    .set    reorder
    .global plr_S_entry
    .ent    plr_S_entry
plr_S_entry:
#ifdef OTTO_PLR_STACK_DEF
    la      sp, OTTO_PLR_STACK_DEF - 4
#endif
#ifdef USE_CC_GPOPT
    la      gp, _gp
#endif

    .global always_return_zero
always_return_zero:
    .global bp_barrier
bp_barrier:
    move    v0, zero
    jr      ra
    .end    plr_S_entry
