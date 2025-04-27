#include <preloader.h>
#include <mips-generic.h>

  .section preloader.uc, "ax", @progbits
  .align  4
  .set    noreorder
  .global soc_init
  .ent    soc_init
soc_init:
#include "preloader/platform/current/lplr_sram.H"

/* memcpy(SRAM_BASE, preloader_uclma, preloader_sz) */
  la      t0, preloader_sram_start
  li      t1, 0x20000000
  or      t0, t0, t1

  la      t1, UCSRAM_BASE

  la      t2, preloader_sram_size
  addu    t2, t0, t2
1:
  lw      t3, 0(t0)
  addi    t0, t0, 4
  sw      t3, 0(t1)
  bne     t0, t2, 1b
  addi    t1, t1, 4
  nop

  REBASED_CALL(cpu_init, UCSRAM_BASE)

  la      t0, soc_init_in_cache
  jr      t0
  nop
  .end    soc_init

GFUNC(soc_init_in_cache)
  la      sp, c_sp_pos
  lw      sp, 0(sp)

  la      t9, c_start
  jal     t9
  nop

  li      sp, CONFIG_SYS_TEXT_BASE - 8
  la      t9, board_init_f
  j       t9
  nop
END(soc_init_in_cache)
