export SWBOARD := rtl9300

OTTO_OSC_H     := $(TOPDIR)/include/asm/arch/osc.h
OTTO_CG_FREQ_H := $(TOPDIR)/arch/otto40/include/asm/otto_cg_dev_freq.h
OTTO_PLAT_DIR  := $(TOPDIR)/arch/otto40/plr/src/platform/9300/
OTTO_CPU_H     := $(TOPDIR)/arch/otto40/include/asm/arch/cpu.h

ifdef CONFIG_CMD_FLASH_TEST
CFLAGS  += '-DFLASHBASE=0xB4000000'
endif

ifdef CONFIG_CMD_CONCURENT_TEST
COBJS	+= gdma_driver.o concur_pattern.o concur_test.o
endif

include $(TOPDIR)/board/Realtek/switch/sdk/build/Makefile.uboot

header_sync:
	@$(call get_otto_file, $(OTTO_OSC_H), $(OTTO_PLAT_DIR)/osc.h)
	@$(call get_otto_file, $(OTTO_CG_FREQ_H), $(OTTO_PLAT_DIR)/cg/cg_dev_freq.h)
	@$(call get_otto_file, $(OTTO_CPU_H), arch/otto40/plr/src/soc/cpu/mips34kc/cpu.h)
