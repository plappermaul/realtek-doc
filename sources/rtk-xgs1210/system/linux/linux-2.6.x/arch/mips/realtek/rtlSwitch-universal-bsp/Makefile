#
# Makefile for the rtlSwitch-universal-bsp specific parts of the kernel
#

EXTRA_CFLAGS += -I$(SDK_INCLUDE) -I$(SYS_INCLUDE) -DRTK_UNIVERSAL_BSP -Werror -DRTK_LKM

obj-y	 	:= setup.o printf.o int.o mem.o chip_probe.o chip_probe_export.o
# obj-$(CONFIG_IRQ_GIC)           += gcmp-gic.o
obj-y        += gcmp-gic.o init_scache.o
EXTRA_AFLAGS := $(CFLAGS)

