obj-$(CONFIG_CMD_SOC_TEST) += hal_command/
obj-$(CONFIG_CMD_SOC_TEST) += hal_common_lib/
obj-$(CONFIG_CMD_SOC_TEST) += hal_peripheral_lib/
obj-$(CONFIG_CMD_SOC_TEST) += common/
PLATFORM_CPPFLAGS += -I./board/realtek_10G_PON/taurus
#PLATFORM_CPPFLAGS += -I ./board/$(BOARDDIR)
#PLATFORM_CPPFLAGS += $(srctree)/board/$(BOARDDIR)/include
#PLATFORM_CPPFLAGS += $(TOPDIR)/include $(SRCTREE)/arch/$(ARCH)/include
PLATFORM_CPPFLAGS += -I ./board/realtek_10G_PON/common/socCmd/include
PLATFORM_CPPFLAGS += -I ./board/realtek_10G_PON/common/socCmd/hal_common_lib/common/include
PLATFORM_CPPFLAGS += -I ./board/realtek_10G_PON/common/socCmd/hal_common_lib/timer/realtek/rtl8198e/include
PLATFORM_CPPFLAGS += -I ./board/realtek_10G_PON/common/socCmd/hal_peripheral_lib/extend/98X/include
PLATFORM_CPPFLAGS += -I ./board/realtek_10G_PON/common/socCmd/hal_common_lib/system/98X/include

obj-$(CONFIG_CMD_RTK_LDMA_TEST) += ldma/ arm_pmu_lib/armv8_pmu_func.o
obj-$(CONFIG_CMD_RTK_PER_TMR_TEST) += ca_per_timer/ arm_pmu_lib/armv8_pmu_func.o
obj-$(CONFIG_CMD_RTK_XRAM_TEST) += xram/xram_mem.o
