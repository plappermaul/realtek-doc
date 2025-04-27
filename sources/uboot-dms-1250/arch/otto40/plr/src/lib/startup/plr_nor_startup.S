#include <soc.h>
#include <arch.h>

#define REBASE_CALL_TO_UCFLASH(f) \
  la      t0, f;      \
  lui     t1, 0xbfc0; \
  or      t0, t0, t1; \
  jalr    t0;         \
  nop

#define BUSY_LOOP() \
1:                  \
  b       1b;       \
  nop

// this loop is used to create a time window so ICE can break to debug and recover.
#define DEBUG_LOOP(n) \
  li      t0, n;      \
3:                    \
  bnez    t0, 3b;     \
  addi    t0, t0, -1;

  .section .exc_reset, "ax", @progbits
  .align  4
  .global plr_S_entry
  .ent    plr_S_entry
plr_S_entry:
  j       ucbasic_init
  nop
  .end plr_S_entry

  .section .exc_utlb, "ax", @progbits
  .align  4
utlb_handler:
  BUSY_LOOP()

  .section .exc_rest, "ax", @progbits
  .align  4
exc_handler:
  BUSY_LOOP()

  .section .flash_text, "ax", @progbits
  .align  4
  .set    noreorder
  .ent    ucbasic_init
ucbasic_init:
  DEBUG_LOOP(0x10000)

  CPU_BASIC_INIT

  REBASE_CALL_TO_UCFLASH(rlx5281_cache_reset)

  REBASE_CALL_TO_UCFLASH(rlx5281_dis_wb_buf_merge)

  SRAM_CTRL_INIT

  la      t0, csram_init
  jr      t0
  nop

  //get uc-sram code/data from flash
csram_init:
  la      t0, OTTO_SRAM_START
  la      t1, OTTO_ENTRY
  li      t2, %lo(end_of_sram - OTTO_SRAM_START)
1:
  lw      t3, 0(t1)
  lw      t4, 4(t1)
  sw      t3, 0(t0)
  sw      t4, 4(t0)
  addi    t2, t2, -8
  addi    t0, t0, 8
  bgtz    t2, 1b
  addi    t1, t1, 8

  la      a0, OTTO_SRAM_START
  la      a1, end_of_sram
  REBASE_CALL_TO_UCFLASH(rlx5281_write_back_inv_dcache_range)

init_bss:
  la      t0, start_of_bss
  la      t1, end_of_bss - 4
1:
  sw      zero, 0(t0)
  bne     t0, t1, 1b
  addi    t0, t0, 4

init_stack_guard:
  la      t0, sp_bottom
  addi    t1, t0, 32 - 4
  li      t2, 0xcafecafe
1:
  sw      t2, 0(t0)
  bne     t0, t1, 1b
  addi    t0, t0, 4

start_traversal_init_table:
  la      a0, traversal_init_table
  lui     sp, %hi(OTTO_PLR_STACK_DEF)
  jr      a0
  ori     sp, %lo(OTTO_PLR_STACK_DEF)
  .end    ucbasic_init
