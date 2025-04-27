#
# Copyright (C) 2009 Realtek Semiconductor Corp. 
#
# Makefile for SDK Unit Test Directory
#

ifeq ($(SDK_CONFIG), $(wildcard $(SDK_CONFIG)))
include $(SDK_CONFIG)
endif

ifeq ($(CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE),y)
include $(SDK_BUILD)/Makefile.linux.kernel.ut
else
include $(SDK_BUILD)/Makefile.linux.user.ut
endif

