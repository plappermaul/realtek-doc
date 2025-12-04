#ifeq ($(CONFIG_BOARD_rtl8196f),y)
#CFLAGS  += -DRTL8196F_SUPPORT
#endif
#ifeq ($(CONFIG_BOARD_rtl8198f),y)
#CFLAGS  += -DRTL8198F_SUPPORT
#endif


###============ UART ====================== ###
obj-$(CONFIG_CMD_RTK_UART) += uart/rtk_uart_extend.o
obj-$(CONFIG_CMD_RTK_UART) += uart/rtk_uart_extend_test.o
obj-$(CONFIG_CMD_RTK_UART) += uart/rtk_hal_uart.o

###============ SPI ====================== ###
obj-$(CONFIG_CMD_RTK_SPI) += spi/dw_spi_extend.o
obj-$(CONFIG_CMD_RTK_SPI) += spi/dw_spi_extend_test.o
obj-$(CONFIG_CMD_RTK_SPI) += spi/dw_hal_spi.o

###============ GDMA ====================== ###
obj-$(CONFIG_CMD_RTK_GDMA) += gdma/dw_gdma_extend.o
obj-$(CONFIG_CMD_RTK_GDMA) += gdma/dw_gdma_extend_test.o
obj-$(CONFIG_CMD_RTK_GDMA) += gdma/dw_hal_gdma.o

###============ I2C ====================== ###
#obj-$(CONFIG_CMD_RTK_I2C) += i2c_dw/dw_i2c_extend.o
#obj-$(CONFIG_CMD_RTK_I2C) += i2c_dw/dw_i2c_extend_test.o
#obj-$(CONFIG_CMD_RTK_I2C) += i2c_dw/dw_hal_i2c.o

#obj-$(CONFIG_CMD_RTK_I2C) += i2c_rtk/rtk_i2c_extend.o
#obj-$(CONFIG_CMD_RTK_I2C) += i2c_rtk/rtk_i2c_extend_test.o
#obj-$(CONFIG_CMD_RTK_I2C) += i2c_rtk/rtk_hal_i2c.o 

###============ GPIO ====================== ###
#obj-$(CONFIG_CMD_RTK_GPIO) += gpio/dw_gpio_extend.o
#obj-$(CONFIG_CMD_RTK_GPIO) += gpio/dw_gpio_extend_test.o
#obj-$(CONFIG_CMD_RTK_GPIO) += gpio/dw_hal_gpio.o

###============ I2S ====================== ###
obj-$(CONFIG_CMD_RTK_I2S) += i2s/rtk_i2s_extend.o
obj-$(CONFIG_CMD_RTK_I2S) += i2s/rtk_i2s_extend_test.o
obj-$(CONFIG_CMD_RTK_I2S) += i2s/rtk_i2s_demo.o
obj-$(CONFIG_CMD_RTK_I2S) += i2s/rtk_sal_i2s.o
obj-$(CONFIG_CMD_RTK_I2S) += i2s/rtk_hal_i2s.o

###============ PCIE ====================== ###
#obj-$(CONFIG_CMD_RTK_PCIE) += pcie/dw_pcie_extend.o
#obj-$(CONFIG_CMD_RTK_PCIE) += pcie/dw_pcie_extend_test.o
#obj-$(CONFIG_CMD_RTK_PCIE) += pcie/dw_hal_pcie.o

#ifeq ($(CONFIG_BOARD_rtl8198f),y)
#obj-$(CONFIG_CMD_RTK_PCIE) += pcie/8852A/rtk_pcie_8852A.o
#obj-$(CONFIG_CMD_RTK_PCIE) += pcie/8852A/pltfm_cb.o
#obj-$(CONFIG_CMD_RTK_PCIE) += pcie/8852A/wlan_ctrl.o
#obj-$(CONFIG_CMD_RTK_PCIE) += pcie/8852A/halmac/mac_ax.o
#obj-$(CONFIG_CMD_RTK_PCIE) += pcie/8852A/halmac/mac_ax/_pcie.o
#obj-$(CONFIG_CMD_RTK_PCIE) += pcie/8852A/halmac/mac_ax/dbgpkg.o
#obj-$(CONFIG_CMD_RTK_PCIE) += pcie/8852A/halmac/mac_ax/efuse.o
#obj-$(CONFIG_CMD_RTK_PCIE) += pcie/8852A/halmac/mac_ax/fwcmd.o
#obj-$(CONFIG_CMD_RTK_PCIE) += pcie/8852A/halmac/mac_ax/fwdl.o
#obj-$(CONFIG_CMD_RTK_PCIE) += pcie/8852A/halmac/mac_ax/hw.o
#obj-$(CONFIG_CMD_RTK_PCIE) += pcie/8852A/halmac/mac_ax/init.o
#obj-$(CONFIG_CMD_RTK_PCIE) += pcie/8852A/halmac/mac_ax/pwr.o
#obj-$(CONFIG_CMD_RTK_PCIE) += pcie/8852A/halmac/mac_ax/role.o
#obj-$(CONFIG_CMD_RTK_PCIE) += pcie/8852A/halmac/mac_ax/tblupd.o
#obj-$(CONFIG_CMD_RTK_PCIE) += pcie/8852A/halmac/mac_ax/trx_desc.o
#obj-$(CONFIG_CMD_RTK_PCIE) += pcie/8852A/halmac/mac_ax/trxcfg.o
#obj-$(CONFIG_CMD_RTK_PCIE) += pcie/8852A/halmac/mac_ax/mac_8852a/init_8852a.o
#obj-$(CONFIG_CMD_RTK_PCIE) += pcie/8852A/halmac/mac_ax/mac_8852a/pwr_seq_8852a.o
#endif

###============ PCM ====================== ###
#obj-$(CONFIG_CMD_RTK_PCM) += pcm/rtk_pcm_extend.o
#obj-$(CONFIG_CMD_RTK_PCM) += pcm/rtk_pcm_extend_test.o
#obj-$(CONFIG_CMD_RTK_PCM) += pcm/rtk_hal_pcm.o

###============ WLAN ====================== ###
#obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_hal_wlan.o
#obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_wlan_extend.o
#obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_wlan_extend_test.o
#obj-$(CONFIG_CMD_RTK_WLAN_8197F) += wlan/Hal8196FPwrSeq.o
#obj-$(CONFIG_CMD_RTK_WLAN_8814A) += wlan/Hal8814APwrSeq.o
#obj-$(CONFIG_CMD_RTK_WLAN_8198F) += wlan/Hal8198FPwrSeq.o
#obj-$(CONFIG_CMD_RTK_WLAN_MP_8198F_8814A) += wlan/Hal8198FPwrSeq.o
#obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_wlan_SWPS_test.o

###============ sdio_device ====================== ###
#obj-y += sdio_device/

###============ sd30 ====================== ###
obj-$(CONFIG_CMD_RTK_SD30) += sd30/rtk_sd30_extend.o

###============ emmc ====================== ###
obj-$(CONFIG_CMD_RTK_EMMC) += emmc/rtk_emmc_extend.o


###============ usb_otg ====================== ###
#obj-y += usb_otg/

###============ pwm ====================== ###
#obj-$(CONFIG_CMD_RTK_PWM) += pwm/rtk_pwm_extend.o 
#obj-$(CONFIG_CMD_RTK_PWM) += pwm/rtk_pwm_extend_test.o 
#obj-$(CONFIG_CMD_RTK_PWM) += pwm/rtk_hal_pwm.o

###============ timer ====================== ###
#obj-$(CONFIG_CMD_RTK_TIMER) += timer/dw_hal_timer.o
#obj-$(CONFIG_CMD_RTK_TIMER) += timer/dw_timer_extend.o 
#obj-$(CONFIG_CMD_RTK_TIMER) += timer/dw_timer_extend_test.o

###============ RTC ====================== ###
obj-$(CONFIG_CMD_CA_RTC) += rtc/ca_hal_rtc.o
obj-$(CONFIG_CMD_CA_RTC) += rtc/ca_rtc_extend.o 
obj-$(CONFIG_CMD_CA_RTC) += rtc/ca_rtc_extend_test.o

###============ MISC ====================== ###
#obj-$(CONFIG_CMD_MISC_IP) += misc/hal_misc.o
#obj-$(CONFIG_CMD_MISC_IP) += misc/misc_extend.o 
#obj-$(CONFIG_CMD_MISC_IP) += misc/misc_extend_test.o

###============ NE ====================== ###
#obj-$(CONFIG_CMD_RTK_NE_8198F) += ne/ne_hw_init.o
#obj-$(CONFIG_CMD_RTK_NE_8198F) += ne/aal_ni_l2.o
#obj-$(CONFIG_CMD_RTK_NE_8198F) += ne/aal_arb.o
#obj-$(CONFIG_CMD_RTK_NE_8198F) += ne/aal_common.o
#obj-$(CONFIG_CMD_RTK_NE_8198F) += ne/aal_init.o
#obj-$(CONFIG_CMD_RTK_NE_8198F) += ne/aal_l2_vlan.o
#obj-$(CONFIG_CMD_RTK_NE_8198F) += ne/aal_l2_tm.o
#obj-$(CONFIG_CMD_RTK_NE_8198F) += ne/aal_l2_tm_cb.o
#obj-$(CONFIG_CMD_RTK_NE_8198F) += ne/aal_l2_te.o
#obj-$(CONFIG_CMD_RTK_NE_8198F) += ne/aal_l2_qm.o
#obj-$(CONFIG_CMD_RTK_NE_8198F) += ne/aal_l3_tm.o
#obj-$(CONFIG_CMD_RTK_NE_8198F) += ne/aal_l3_te.o
#obj-$(CONFIG_CMD_RTK_NE_8198F) += ne/ca_ni.o
#obj-$(CONFIG_CMD_RTK_NE_8198F) += ne/aal_mcast.o
#obj-$(CONFIG_CMD_RTK_NE_8198F) += ne/aal_table.o
#obj-$(CONFIG_CMD_RTK_NE_8198F) += ne/ca_mut.o
#obj-$(CONFIG_CMD_RTK_NE_8198F) += ne/eth_port.o
#obj-$(CONFIG_CMD_RTK_NE_8198F) += ne/aal_port.o
#obj-$(CONFIG_CMD_RTK_NE_8198F) += ne/port.o
#obj-$(CONFIG_CMD_RTK_NE_8198F) += ne/aal_phy.o
#obj-$(CONFIG_CMD_RTK_NE_8198F) += ne/aal_ni.o
#obj-$(CONFIG_CMD_RTK_NE_8198F) += ne/aal_fdb.o
#obj-$(CONFIG_CMD_RTK_NE_8198F) += ne/l2.o
#obj-$(CONFIG_CMD_RTK_NE_8198F) += ne/aal_arb.o
#obj-$(CONFIG_CMD_RTK_NE_8198F) += ne/vlan.o
#obj-$(CONFIG_CMD_RTK_NE_8198F) += ne/sys.o
#obj-$(CONFIG_CMD_RTK_NE_8198F) += ne/aal_l2_cls.o
