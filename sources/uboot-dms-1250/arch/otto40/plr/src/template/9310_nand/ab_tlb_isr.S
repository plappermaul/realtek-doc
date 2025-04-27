#include <cpu/tlb.h>

// callee save register can be ommit
#define SKIP_BACKUP_CALLEE_SAVE

    .section .sdata
    .align 2
bur_r1:     .word 0
bur_r2:     .word 0
bur_r3:     .word 0
bur_r4:     .word 0
bur_r5:     .word 0
bur_r6:     .word 0
bur_r7:     .word 0
bur_r8:     .word 0
bur_r9:     .word 0
bur_r10:    .word 0
bur_r11:    .word 0
bur_r12:    .word 0
bur_r13:    .word 0
bur_r14:    .word 0
bur_r15:    .word 0
#ifndef     SKIP_BACKUP_CALLEE_SAVE
bur_r16:    .word 0
bur_r17:    .word 0
bur_r18:    .word 0
bur_r19:    .word 0
bur_r20:    .word 0
bur_r21:    .word 0
bur_r22:    .word 0
bur_r23:    .word 0
#endif
bur_r24:    .word 0
bur_r25:    .word 0
bur_r30:    .word 0
bur_r31:    .word 0
bur_hi :    .word 0
bur_lo :    .word 0

// k0 will be 9f00_0000 in lplr_isr
#define REF_REG  k0


#define BACKUP_REG(n)   sw  $ ## n, %lo(bur_r ## n)(REF_REG)
#define RESTORE_REG(n)  lw  $ ## n, %lo(bur_r ## n)(REF_REG)

    .section .unswapable_text, "ax", @progbits
    .align 2
    .set    reorder
    .set    noat
    .global plr_tlb_miss_isr
    .ent    plr_tlb_miss_isr
plr_tlb_miss_isr:
    // 0. check if it is I-TLB miss
    // 1. backup registers (mips16); 
    //      note1. stack will not be swapped, it uses the 'user' stack
    //      note2. gp keep the same forever (not backup $28)
    
    BACKUP_REG(2)
    BACKUP_REG(3)
    BACKUP_REG(4)
    BACKUP_REG(5)
    BACKUP_REG(6)
    BACKUP_REG(7)
#ifndef     SKIP_BACKUP_CALLEE_SAVE
    BACKUP_REG(16)
    BACKUP_REG(17)
#endif
    BACKUP_REG(24)
    BACKUP_REG(31)
    
    mfc0    a0, CP0_CAUSE
    andi    a1, a0, 0x7c
    xori    a2, a1, 0x08
    move    v0, k1
8:
    mfhi    k1
    bnez    a2, 8b
    
    sw      k1, %lo(bur_hi)(REF_REG)
    mflo    k1
    sw      k1, %lo(bur_lo)(REF_REG)
    
    // 2. try doing some simple process: mmu_tlb_miss_check_first(badvaddr, entry_lo0, entry_lo1, vpage_num)
    mfc0    a0, CP0_BADVADDR
    srl     a2, a0, TLB_PAGE_SHIFT
    and     a3, a2, TLB_NUM_TLB_MASK
    srl     k1, a3, 1               // k1 will keep the index register for a while
    mtc0    k1, CP0_INDEX
    nop
    nop
    tlbr
    nop
    nop
    mfc0    a1, CP0_ENTRYLO0
    mfc0    a2, CP0_ENTRYLO1
    bnez    v0, tlb_invalid
    jal     mmu_tlb_miss_check_first
    bnez    v0, finish_mapping

tlb_invalid:
    // 3. backup registers (other than mips16)
    BACKUP_REG(1)
    BACKUP_REG(8)
    BACKUP_REG(9)
    BACKUP_REG(10)
    BACKUP_REG(11)
    BACKUP_REG(12)
    BACKUP_REG(13)
    BACKUP_REG(14)
    BACKUP_REG(15)
#ifndef     SKIP_BACKUP_CALLEE_SAVE
    BACKUP_REG(18)
    BACKUP_REG(19)
    BACKUP_REG(20)
    BACKUP_REG(21)
    BACKUP_REG(22)
    BACKUP_REG(23)
#endif
    BACKUP_REG(24)
    BACKUP_REG(25)
    BACKUP_REG(30)
    
    // 4. call mmu_tlb_miss_va_to_pa(badvaddr, entry_lo0, entry_lo1, vpage_num)
    //mtc0    k1, CP0_INDEX
    mfc0    a0, CP0_BADVADDR    
    srl     a2, a0, TLB_PAGE_SHIFT
    and     a3, a2, TLB_NUM_TLB_MASK    
    mfc0    a1, CP0_ENTRYLO0
    mfc0    a2, CP0_ENTRYLO1
    jal     mmu_tlb_miss_va_to_pa

    // 5. restore regusters (other than mips16)
    RESTORE_REG(1)
    RESTORE_REG(8)
    RESTORE_REG(9)
    RESTORE_REG(10)
    RESTORE_REG(11)
    RESTORE_REG(12)
    RESTORE_REG(13)
    RESTORE_REG(14)
    RESTORE_REG(15)
#ifndef     SKIP_BACKUP_CALLEE_SAVE
    RESTORE_REG(18)
    RESTORE_REG(19)
    RESTORE_REG(20)
    RESTORE_REG(21)
    RESTORE_REG(22)
    RESTORE_REG(23)
#endif
    RESTORE_REG(24)
    RESTORE_REG(25)
    RESTORE_REG(30)

finish_mapping:
    // 6. restore registers (mips16)
    RESTORE_REG(2)
    RESTORE_REG(3)
    RESTORE_REG(4)
    RESTORE_REG(5)
    RESTORE_REG(6)
    RESTORE_REG(7)
#ifndef     SKIP_BACKUP_CALLEE_SAVE
    RESTORE_REG(16)
    RESTORE_REG(17)
#endif
    RESTORE_REG(24)
    RESTORE_REG(31)
    lw      k1, %lo(bur_hi)(REF_REG)
    mthi    k1
    lw      k1, %lo(bur_lo)(REF_REG)
    mtlo    k1

    // 7. return from interrupt
    eret
    

    .end    plr_tlb_miss_isr

// provide to mmu driver 
    .section .unswapable_text, "ax", @progbits
    .align 2
    .set    reorder
    .set    noat
    .global tlb_update_badvaddr_entry
    .ent    tlb_update_badvaddr_entry
tlb_update_badvaddr_entry:
    // assume k1 keeping the index
    mtc0    k1, CP0_INDEX
    mtc0    a0, CP0_ENTRYHI
    mtc0    a1, CP0_ENTRYLO0
    mtc0    a2, CP0_ENTRYLO1
    nop
    nop
    tlbwi    
    nop
    nop
    jr      ra
    .end    tlb_update_badvaddr_entry
