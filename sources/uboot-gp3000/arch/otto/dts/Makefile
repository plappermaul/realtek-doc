# SPDX-License-Identifier: GPL-2.0+

dtb-y += realtek,generic.dtb
dtb-$(CONFIG_TARGET_RTL9603CVD) += apro_03cvd_fit_control.dtb
dtb-$(CONFIG_TARGET_VD2) += vd2_fit_control.dtb
dtb-$(CONFIG_TARGET_PHOEBUS) += phoebus_fit_control.dtb

targets += $(dtb-y)

# Add any required device tree compiler flags here
DTC_FLAGS +=

PHONY += dtbs
dtbs: $(addprefix $(obj)/, $(dtb-y))
	@:

clean-files := *.dtb
