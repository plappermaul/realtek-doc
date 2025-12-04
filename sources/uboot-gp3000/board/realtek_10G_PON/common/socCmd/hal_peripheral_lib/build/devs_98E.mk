
###============ UART ====================== ###
obj-$(CONFIG_CMD_RTK_UART) += uart/rtk_uart_extend.o
obj-$(CONFIG_CMD_RTK_UART) += uart/rtk_uart_extend_test.o
obj-$(CONFIG_CMD_RTK_UART) += uart/rtk_hal_uart.o

###============ GDMA ====================== ###
obj-$(CONFIG_CMD_RTK_GDMA) += gdma/dw_gdma_extend.o
obj-$(CONFIG_CMD_RTK_GDMA) += gdma/dw_hal_gdma.o

###============ I2C ====================== ###
obj-$(CONFIG_CMD_RTK_I2C) += i2c_dw/dw_i2c_extend.o
obj-$(CONFIG_CMD_RTK_I2C) += i2c_dw/dw_i2c_extend_test.o
obj-$(CONFIG_CMD_RTK_I2C) += i2c_dw/dw_hal_i2c.o

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
obj-$(CONFIG_CMD_RTK_PCIE) += pcie/dw_pcie_extend.o
obj-$(CONFIG_CMD_RTK_PCIE) += pcie/dw_pcie_extend_test.o
obj-$(CONFIG_CMD_RTK_PCIE) += pcie/dw_hal_pcie.o

###============ PCM ====================== ###
obj-$(CONFIG_CMD_RTK_PCM) += pcm/rtk_pcm_extend.o
obj-$(CONFIG_CMD_RTK_PCM) += pcm/rtk_pcm_extend_test.o
obj-$(CONFIG_CMD_RTK_PCM) += pcm/rtk_hal_pcm.o

###============ SPI ====================== ###
#obj-y += spi/


###============ WLAN ====================== ###
#obj-y += wlan/

###============ sdio_device ====================== ###
#obj-y += sdio_device/

###============ usb_otg ====================== ###
#obj-y += usb_otg/




