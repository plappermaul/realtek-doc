# SPDX-License-Identifier: GPL-2.0+

head-y := arch/otto/cpu/start.o

ifeq ($(CONFIG_SPL_BUILD),y)
ifneq ($(CONFIG_SPL_START_S_PATH),)
head-y := $(CONFIG_SPL_START_S_PATH:"%"=%)/start.o
endif
endif

libs-y += arch/otto/cpu/
libs-y += arch/otto/lib/

machine-$(CONFIG_TARGET_VD2) += vd2
machine-$(CONFIG_TARGET_RTL9310) += rtl9310
machine-$(CONFIG_TARGET_RTL9603CVD) += rtl9603cvd
machine-$(CONFIG_TARGET_PHOEBUS) += Phoebus

machdirs := $(patsubst %,arch/otto/mach-%/,$(machine-y))
libs-y += $(machdirs)

PLATFORM_CPPFLAGS += $(patsubst %,-I$(srctree)/%include,$(machdirs))

# Optimize for MIPS architectures
arch-$(CONFIG_OTTO_MIPS32_R2) += -march=mips32r2 -Wa,-mips32r2

# Allow extra optimization for specific CPUs/SoCs
tune-$(CONFIG_MIPS_TUNE_4KC) += -mtune=4kc
tune-$(CONFIG_MIPS_TUNE_14KC) += -mtune=14kc
tune-$(CONFIG_MIPS_TUNE_24KC) += -mtune=24kc
tune-$(CONFIG_MIPS_TUNE_34KC) += -mtune=34kc
tune-$(CONFIG_MIPS_TUNE_74KC) += -mtune=74kc

# Include default header files
cflags-y += -I$(srctree)/arch/otto/include/asm/mach-generic

PLATFORM_CPPFLAGS += $(arch-y) $(tune-y) $(cflags-y)

PHONY += otto_src_sync

otto_src_sync:
	$(Q)$(MAKE) $(build)=$(CONFIG_BOARDDIR)/../common otto_common_sync
	$(Q)$(MAKE) $(build)=$(CONFIG_BOARDDIR) otto_board_sync
ifneq (,$(wildcard $(CPUDIR)/$(SOC)/Makefile))
	$(Q)grep "^otto_soc_sync:" $(CPUDIR)/$(SOC)/Makefile > /dev/null && \
	$(MAKE) $(build)=$(CPUDIR)/$(SOC) otto_soc_sync || \
	true
endif

prepare: otto_src_sync
