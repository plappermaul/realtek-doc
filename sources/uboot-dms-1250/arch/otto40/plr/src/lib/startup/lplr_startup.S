#include <soc.h>

    .section .entry, "ax", @progbits
    .align 2
    .set    noreorder
    .global lplr_S_entry
    .ent    lplr_S_entry
lplr_S_entry:
    j       lplr_basic_init
    nop    
    .global always_return_zero  // a common used dummy function, which always return zero
always_return_zero:
    jr      ra
    move    v0, zero
    nop
    nop
    nop
    nop
    .end    lplr_S_entry

    
    .section .short_init, "ax", @progbits
    .align 2    
    .set    noreorder
    .ent    lplr_basic_init
lplr_basic_init:
    // 1. initial cpu
    CPU_BASIC_INIT
        
    // 2. initial cache and jump to cache area
    lui     ra, %hi(9f)
    j       rlx5281_cache_reset;
    ori     ra, ra, %lo(9f)
9:
    jal     rlx5281_dis_wb_buf_merge;
    nop;

    // 3. initial sram and clear up whole SRAM
    SRAM_CTRL_INIT
    li      a0, OTTO_SRAM_START
    add     a1, a0, OTTO_SRAM_SIZE - 4
8:    
    sw      zero, 0(a1)
    bne     a0, a1, 8b
    add     a1, a1, -4

    // 4. setup stack
    add     sp, a0, (OTTO_LPLR_SPI_NAND_STACK_DEF - OTTO_SRAM_START)

    // 5. jump to c_entry
    jal     lplr_c_entry
    nop
    
    // 6. jump to preloader S_entry 
    // CATION: (a) preloader may not return (b) preloader could change stack point
    li      a0, OTTO_SRAM_START
    jalr    a0
    nop
    
    // 7. traversal the init table and no return
    la      a0, traversal_init_table
    jr      a0
    nop   
    .end    lplr_basic_init

    
    .section .isr_entry_text, "ax", @progbits
    .align 2    
    .ent    lplr_isr_entry
lplr_isr_entry:
    lui     k0, %hi(OTTO_SRAM_START)
    lw      k1, (OTTO_HEADER_OFFSET + 24)(k0) 
    jr      k1
    nop
    .end    lplr_isr_entry
