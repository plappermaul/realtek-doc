#
# Copyright (C) 2011 Realtek Semiconductor Corp. 
#
# Makefile for SDK Linux RTK Module
#

SDK_BUILD = $(SDKDIR)/build

ifeq ($(SDK_CONFIG), $(wildcard $(SDK_CONFIG)))
include $(SDK_CONFIG)
endif

ifeq ($(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE),y)
#EXTRA_CFLAGS += -DCONFIG_SDK_KERNEL_LINUX -DINITIAL_GPON_BY_CMD
#EXTRA_CFLAGS += -DCONFIG_SDK_KERNEL_LINUX
include $(SDK_BUILD)/Makefile.linux.kernel.rtk
else
include $(SDK_BUILD)/Makefile.linux.user.rtk
endif

