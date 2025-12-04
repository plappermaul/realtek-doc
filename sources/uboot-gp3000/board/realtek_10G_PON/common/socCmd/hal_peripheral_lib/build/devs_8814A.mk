ifeq ($(CONFIG_BOARD_rtl8196f),y)
ccflags-y  += -DRTL8196F_SUPPORT
endif
ifeq ($(CONFIG_BOARD_rtl8198f),y)
ccflags-y  += -DRTL8198F_SUPPORT
endif
ifeq ($(CONFIG_BOARD_rtl8197g),y)
ccflags-y  += -DRTL8197G_SUPPORT
endif



###============ WLAN ====================== ###
obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_hal_wlan.o
obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_wlan_extend.o
obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_wlan_extend_test.o
obj-$(CONFIG_CMD_RTK_WLAN_8197F) += wlan/Hal8196FPwrSeq.o
obj-$(CONFIG_CMD_RTK_WLAN_8814A) += wlan/Hal8814APwrSeq.o
obj-$(CONFIG_CMD_RTK_WLAN_8198F) += wlan/Hal8198FPwrSeq.o
obj-$(CONFIG_CMD_RTK_WLAN_MP_8198F_8814A) += wlan/Hal8814APwrSeq.o
#obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_wlan_SWPS_test.o

###============ PCIE ====================== ###
### already compiled in 98F 
ifeq ($(CONFIG_CMD_RTK_WLAN_8814A),y)
obj-$(CONFIG_CMD_RTK_PCIE) += pcie/dw_pcie_extend.o
obj-$(CONFIG_CMD_RTK_PCIE) += pcie/dw_pcie_extend_test.o
obj-$(CONFIG_CMD_RTK_PCIE) += pcie/dw_hal_pcie.o
endif
