
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
obj-$(CONFIG_CMD_RTK_I2C) += i2c_dw/dw_i2c_extend.o
obj-$(CONFIG_CMD_RTK_I2C) += i2c_dw/dw_i2c_extend_test.o
obj-$(CONFIG_CMD_RTK_I2C) += i2c_dw/dw_hal_i2c.o

obj-$(CONFIG_CMD_RTK_I2C) += i2c_rtk/rtk_i2c_extend.o
obj-$(CONFIG_CMD_RTK_I2C) += i2c_rtk/rtk_i2c_extend_test.o
obj-$(CONFIG_CMD_RTK_I2C) += i2c_rtk/rtk_hal_i2c.o 

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
obj-$(CONFIG_CMD_RTK_PCIE) += pcie/dw_pcie_extend.o
obj-$(CONFIG_CMD_RTK_PCIE) += pcie/dw_pcie_extend_test.o
obj-$(CONFIG_CMD_RTK_PCIE) += pcie/dw_hal_pcie.o

###============ PCM ====================== ###
obj-$(CONFIG_CMD_RTK_PCM) += pcm/rtk_pcm_extend.o
obj-$(CONFIG_CMD_RTK_PCM) += pcm/rtk_pcm_extend_test.o
obj-$(CONFIG_CMD_RTK_PCM) += pcm/rtk_hal_pcm.o

###============ WLAN ====================== ###
obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_hal_wlan.o
obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_wlan_extend.o
obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_wlan_extend_test.o
obj-$(CONFIG_CMD_RTK_WLAN) += wlan/Hal8196FPwrSeq.o
###============ sdio_device ====================== ###
#obj-y += sdio_device/

###============ sd30 ====================== ###
obj-$(CONFIG_CMD_RTK_SD30) += sd30/rtk_sd30_extend.o

###============ emmc ====================== ###
obj-$(CONFIG_CMD_RTK_EMMC) += emmc/rtk_emmc_extend.o


###============ usb_otg ====================== ###
#obj-y += usb_otg/

###============ pwm ====================== ###
obj-$(CONFIG_CMD_RTK_PWM) += pwm/rtk_pwm_extend.o 
obj-$(CONFIG_CMD_RTK_PWM) += pwm/rtk_pwm_extend_test.o 
obj-$(CONFIG_CMD_RTK_PWM) += pwm/rtk_hal_pwm.o
###============ timer ====================== ###
obj-$(CONFIG_CMD_RTK_TIMER) += timer/dw_hal_timer.o
obj-$(CONFIG_CMD_RTK_TIMER) += timer/dw_timer_extend.o 
obj-$(CONFIG_CMD_RTK_TIMER) += timer/dw_timer_extend_test.o 
