#include <soc.h>
#include <cpu/regdef.h>

#ifndef SECTION_PRELOADER_ENTRY
    #define SECTION_PRELOADER_ENTRY .uncache_unswapable_area_small
#endif

    .bss
_current_funcion:
    .word

    .section .reset_entry, "ax", @progbits
    .align 4
    .set    noreorder
    .global reset_isr
    .ent    reset_isr
reset_isr:
    la      a0, hook_func_start
    j       preloader_entry
    lui     s0, %hi(_current_funcion)
    .end    reset_isr
    
    
    .section SECTION_PRELOADER_ENTRY, "ax", @progbits
    .align 4
    .set    noreorder
preloader_loop:
    lw      a0, %lo(_current_funcion)(s0)
preloader_entry:
    la      a1, hook_func_end
dead_loop:
    beq     a0, a1, dead_loop   // if true, it will be a dead loop
    addiu   a2, a0, 4
    lw      a3, 0(a0)
    jalr    a3
    sw      a2, %lo(_current_funcion)(s0)
    b       preloader_loop      // pc related branch, because it would run on both cached and uncached area
    lui     s0, %hi(_current_funcion)

    