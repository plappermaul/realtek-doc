# Realtek Semiconductor Corp.
#
# bsp/Makefile
#     bsp Makefile
#
# Copyright (C) 2006-2012 Tony Wu (tonywu@realtek.com)
#

CPPFLAGS_vmlinux.lds := $(KBUILD_CFLAGS)

#
# Sheipa
#
core-y   += arch/mips/bsp/
cflags-y += -Iinclude/asm-mips -Iarch/mips/bsp/ -DRTL9300 -DRTK_UNIVERSAL_BSP -DRTK_LKM
load-y   += 0x80000000

#
# extra-y  += vmlinux.lds

obj-y := prom.o setup.o irq.o timer.o serial.o chip_probe.o chip_probe_export.o
obj-y +=init_l2cache.o
obj-y +=rtk_util.o
obj-$(CONFIG_SMP) += smp.o
obj-$(CONFIG_CEVT_EXT) += cevt-ext.o
