#
# (C) Copyright 2018 -
# Cortina-Access Taiwan Limited.
#
# SPDX-License-Identifier:	GPL-2.0+
#

obj-y	+= venus.o enable_caches.o
obj-$(CONFIG_RAM_MT)   += venus_memory_test.o
obj-$(CONFIG_DISPLAY_BOARDINFO) += venus_cortex_freq.o
obj-$(CONFIG_RTK_VENUS_CPUFREQ_CMD) += cmd_cortex_freq.o 
obj-y += swp_led_function.o

