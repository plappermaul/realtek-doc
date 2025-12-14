#
# (C) Copyright 2018 -
# Cortina-Access Taiwan Limited.
#
# SPDX-License-Identifier:	GPL-2.0+
#

obj-y	+= taurus.o enable_caches.o
obj-${CONFIG_DISPLAY_BOARDINFO} += taurus_cortex_freq.o
obj-${CONFIG_RTK_TAURUS_HW_TEST}    += hw_test/dss.o hw_test/fss_calibration_model.o hw_test/vsfc.o
obj-y += swp_led_function.o

