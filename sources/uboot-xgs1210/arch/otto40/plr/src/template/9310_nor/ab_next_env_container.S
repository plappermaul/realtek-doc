#include <soc.h>

#ifndef STANDALONE_UBOOT
  .section .next_env, "a"
  .global next_env
next_env:
  .incbin "../../uboot/u-boot.bin"
#endif
