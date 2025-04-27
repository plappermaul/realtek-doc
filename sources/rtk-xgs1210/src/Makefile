
# Copyright (C) 2009 Realtek Semiconductor Corp. 
#
# Makefile for SDK Linux RTK Module
#

ifeq ($(SDK_CONFIG), $(wildcard $(SDK_CONFIG)))
include $(SDK_CONFIG)
endif

ifeq ($(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE),y)
include $(SDK_BUILD)/Makefile.linux.kernel.rtk
else
include $(SDK_BUILD)/Makefile.linux.user.rtk
endif

