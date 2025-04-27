#include <preloader.h>
#include <mips-generic.h>

GFUNC(cpu_init)
/* Backup return address */
  move    s6, ra

/* Clear watch registers */
  mtc0    zero, CP0_WATCHLO
  mtc0    zero, CP0_WATCHHI

/* STATUS register */
  mfc0    k0, CP0_STATUS
  li      k1, ~CP0_STATUS_IE
  and     k0, k1
  mtc0    k0, CP0_STATUS

/* (Watch Pending), SW0/1 should be cleared */
  mtc0    zero, CP0_CAUSE

/* Timer */
  mtc0    zero, CP0_COUNT
  mtc0    zero, CP0_COMPARE

#if (CONFIG_STANDALONE_UBOOT == 1)
  REBASED_CALL(mips_cache_reset, UCSRAM_BASE)
#else
  REBASED_CALL(mips_cache_reset, UCFLASH_BASE)
#endif

/* Enable cache. However, one should NOT access cached SRAM and cached DRAM until they are initialized. */
  mfc0    t0, CP0_CONFIG
  li      t1, ~CP0_CONF_CACHE_MASK
  and     t0, t0, t1
  li      t1, CP0_CONF_CACHABLE_NC_WB_WA
  or      t0, t0, t1
  mtc0    t0, CP0_CONFIG

  jr      s6
END(cpu_init)
