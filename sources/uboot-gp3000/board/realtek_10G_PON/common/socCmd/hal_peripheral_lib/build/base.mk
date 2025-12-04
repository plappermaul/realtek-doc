ifeq ($(CONFIG_CA8277B),y)
###============ I2S ====================== ###
obj-$(CONFIG_CMD_RTK_I2S) += i2s/rtk_v1_1/rtk_i2s_base.o
obj-$(CONFIG_CMD_RTK_I2S) += i2s/rtk_v1_1/rtk_i2s_base_test.o

###============ GDMA ====================== ###
obj-$(CONFIG_CMD_RTK_GDMA) += gdma/dw_2_22a/dw_gdma_base.o
obj-$(CONFIG_CMD_RTK_GDMA) += gdma/dw_2_22a/dw_gdma_base_test.o

###============ SPI ====================== ###
obj-$(CONFIG_CMD_RTK_SPI) += spi/dw_4_02a/dw_spi_base.o
obj-$(CONFIG_CMD_RTK_SPI) += spi/dw_4_02a/dw_spi_base_test.o

###============ RTC ====================== ###
obj-$(CONFIG_CMD_CA_RTC) += rtc/ca_v1_0/ca_rtc_base.o
obj-$(CONFIG_CMD_CA_RTC) += rtc/ca_v1_0/ca_rtc_base_test.o

###============ CRYPTO/IPSEC ====================== ###
obj-$(CONFIG_CMD_RTK_CRYPTO) += crypto/rtk_v2_0/rtk_crypto_base_common.o
obj-$(CONFIG_CMD_RTK_CRYPTO) += crypto/rtk_v2_0/cryptoSim_auth.o
obj-$(CONFIG_CMD_RTK_CRYPTO) += crypto/rtk_v2_0/cryptoSim_cipher.o
obj-$(CONFIG_CMD_RTK_CRYPTO) += crypto/rtk_v2_0/cryptoSim_swvrf.o
obj-$(CONFIG_CMD_RTK_CRYPTO) += crypto/rtk_v2_0/rtk_crypto_base.o
obj-$(CONFIG_CMD_RTK_CRYPTO) += crypto/rtk_v2_0/rtk_crypto_base_hal.o
obj-$(CONFIG_CMD_RTK_CRYPTO) += crypto/rtk_v2_0/rtk_crypto_base_test.o

###============ UART ====================== ###
obj-$(CONFIG_CMD_RTK_UART) += uart/rtk_v1_0/rtk_uart_base.o
obj-$(CONFIG_CMD_RTK_UART) += uart/rtk_v1_0/rtk_uart_base_test.o
endif


ifeq ($(CONFIG_BOARD_rtl8196f),y)
###============ UART ====================== ###
obj-$(CONFIG_CMD_RTK_UART) += uart/rtk_v1_0/rtk_uart_base.o
obj-$(CONFIG_CMD_RTK_UART) += uart/rtk_v1_0/rtk_uart_base_test.o

###============ SPI ====================== ###
obj-$(CONFIG_CMD_RTK_SPI) += spi/dw_3_18a/dw_spi_base.o
obj-$(CONFIG_CMD_RTK_SPI) += spi/dw_3_18a/dw_spi_base_test.o

###============ GDMA ====================== ###
obj-$(CONFIG_CMD_RTK_GDMA) += gdma/dw_2_16a/dw_gdma_base.o
obj-$(CONFIG_CMD_RTK_GDMA) += gdma/dw_2_16a/dw_gdma_base_test.o

###============ I2C ====================== ###
obj-$(CONFIG_CMD_RTK_I2C) += i2c_dw/dw_1_14a/dw_i2c_base.o
obj-$(CONFIG_CMD_RTK_I2C) += i2c_dw/dw_1_14a/dw_i2c_base_test.o

#obj-$(CONFIG_CMD_RTK_I2C) += i2c_rtk/rtk_i2c_base.o
#obj-$(CONFIG_CMD_RTK_I2C) += i2c_rtk/rtk_i2c_base_test.o

###============ GPIO ====================== ###
#obj-$(CONFIG_CMD_RTK_GPIO) += gpio/dw_gpio_base.o
#obj-$(CONFIG_CMD_RTK_GPIO) += gpio/dw_gpio_base_test.o

###============ I2S ====================== ###
obj-$(CONFIG_CMD_RTK_I2S) += i2s/rtk_v1_0/rtk_i2s_base.o
obj-$(CONFIG_CMD_RTK_I2S) += i2s/rtk_v1_0/rtk_i2s_base_test.o

###============ PCIE ====================== ###
obj-$(CONFIG_CMD_RTK_PCIE) += pcie/dw_4_20a/dw_pcie_base.o
obj-$(CONFIG_CMD_RTK_PCIE) += pcie/dw_4_20a/dw_pcie_base_test.o

###============ PCM ====================== ###
obj-$(CONFIG_CMD_RTK_PCM) += pcm/rtk_v1_0/rtk_pcm_base.o
obj-$(CONFIG_CMD_RTK_PCM) += pcm/rtk_v1_0/rtk_pcm_base_test.o
obj-$(CONFIG_CMD_RTK_PCM) += pcm/rtk_v1_0/rtk_pcm_reg_test.o

###============ sdio_device ====================== ###
#obj-y += sdio_device/

###============ usb_otg ====================== ###
#obj-y += usb_otg/

###============ pwm ====================== ###
obj-$(CONFIG_CMD_RTK_PWM) += pwm/rtk_v1_0/rtk_pwm_base.o 
obj-$(CONFIG_CMD_RTK_PWM) += pwm/rtk_v1_0/rtk_pwm_base_test.o

###============ timer ====================== ###
obj-$(CONFIG_CMD_RTK_TIMER) += timer/dw_v2_05a/dw_timer_base.o 
obj-$(CONFIG_CMD_RTK_TIMER) += timer/dw_v2_05a/dw_timer_base_test.o

###============ ipsec/crypto ====================== ###
obj-$(CONFIG_CMD_RTK_IPSEC) += crypto/rtk_v1_0/rtk_crypto_test.o 
obj-$(CONFIG_CMD_RTK_IPSEC) += crypto/rtk_v1_0/rtk_crypto_interrupt.o 
obj-$(CONFIG_CMD_RTK_IPSEC) += crypto/rtk_v1_0/rtk_crypto_rom.o 
obj-$(CONFIG_CMD_RTK_IPSEC) += crypto/rtk_v1_0/hal_crypto.o 
obj-$(CONFIG_CMD_RTK_IPSEC) += crypto/rtk_v1_0/cryptoSim.o 
obj-$(CONFIG_CMD_RTK_IPSEC) += crypto/rtk_v1_0/cryptoSim_aes.o 
obj-$(CONFIG_CMD_RTK_IPSEC) += crypto/rtk_v1_0/cryptoSim_des.o 
obj-$(CONFIG_CMD_RTK_IPSEC) += crypto/rtk_v1_0/cryptoSim_md5.o 
obj-$(CONFIG_CMD_RTK_IPSEC) += crypto/rtk_v1_0/cryptoSim_sha.o 
#obj-$(CONFIG_CMD_RTK_IPSEC) += crypto/rtk_v1_0/fips.o 
#obj-$(CONFIG_CMD_RTK_IPSEC) += crypto/rtk_v1_0/rngtest.o 
#obj-$(CONFIG_CMD_RTK_IPSEC) += crypto/rtk_v1_0/stats.o 

###============ parallel nand ============== ###
obj-$(CONFIG_CMD_RTK_PARALLEL_NAND) += parallel_nand/rtk_v1_0/rtk_nand_base_test.o
obj-$(CONFIG_CMD_RTK_PARALLEL_NAND) += parallel_nand/rtk_v1_0/rtk_nand_base.o
endif



ifeq ($(CONFIG_BOARD_rtl8197g),y)
###============ UART ====================== ###
obj-$(CONFIG_CMD_RTK_UART) += uart/rtk_v2_0/rtk_uart_base.o
obj-$(CONFIG_CMD_RTK_UART) += uart/rtk_v2_0/rtk_uart_base_test.o

###============ SPI ====================== ###
obj-$(CONFIG_CMD_RTK_SPI) += spi/dw_3_23a/dw_spi_base.o
obj-$(CONFIG_CMD_RTK_SPI) += spi/dw_3_23a/dw_spi_base_test.o

###============ GDMA ====================== ###
obj-$(CONFIG_CMD_RTK_GDMA) += gdma/dw_2_16a/dw_gdma_base.o
obj-$(CONFIG_CMD_RTK_GDMA) += gdma/dw_2_16a/dw_gdma_base_test.o

###============ I2C ====================== ###
obj-$(CONFIG_CMD_RTK_I2C) += i2c_dw/dw_1_14a/dw_i2c_base.o
obj-$(CONFIG_CMD_RTK_I2C) += i2c_dw/dw_1_14a/dw_i2c_base_test.o

#obj-$(CONFIG_CMD_RTK_I2C) += i2c_rtk/rtk_i2c_base.o
#obj-$(CONFIG_CMD_RTK_I2C) += i2c_rtk/rtk_i2c_base_test.o

###============ GPIO ====================== ###
#obj-$(CONFIG_CMD_RTK_GPIO) += gpio/dw_gpio_base.o
#obj-$(CONFIG_CMD_RTK_GPIO) += gpio/dw_gpio_base_test.o

###============ I2S ====================== ###
obj-$(CONFIG_CMD_RTK_I2S) += i2s/rtk_v1_0/rtk_i2s_base.o
obj-$(CONFIG_CMD_RTK_I2S) += i2s/rtk_v1_0/rtk_i2s_base_test.o

###============ PCIE ====================== ###
obj-$(CONFIG_CMD_RTK_PCIE) += pcie/dw_4_20a/dw_pcie_base.o
obj-$(CONFIG_CMD_RTK_PCIE) += pcie/dw_4_20a/dw_pcie_base_test.o

###============ NE ====================== ###
obj-$(CONFIG_CMD_RTK_NE) += ne/rtk_v1_0/dw_ne_base.o
obj-$(CONFIG_CMD_RTK_NE) += ne/rtk_v1_0/dw_ne_base_test.o

###============ PCM ====================== ###
obj-$(CONFIG_CMD_RTK_PCM) += pcm/rtk_v1_0/rtk_pcm_base.o
obj-$(CONFIG_CMD_RTK_PCM) += pcm/rtk_v1_0/rtk_pcm_base_test.o
obj-$(CONFIG_CMD_RTK_PCM) += pcm/rtk_v1_0/rtk_pcm_reg_test.o

###============ sdio_device ====================== ###
#obj-y += sdio_device/

###============ usb_otg ====================== ###
#obj-y += usb_otg/

###============ pwm ====================== ###
obj-$(CONFIG_CMD_RTK_PWM) += pwm/rtk_v1_0/rtk_pwm_base.o
obj-$(CONFIG_CMD_RTK_PWM) += pwm/rtk_v1_0/rtk_pwm_base_test.o

###============ timer ====================== ###
obj-$(CONFIG_CMD_RTK_TIMER) += timer/dw_v2_05a/dw_timer_base.o
obj-$(CONFIG_CMD_RTK_TIMER) += timer/dw_v2_05a/dw_timer_base_test.o

###============ ipsec/crypto ====================== ###
obj-$(CONFIG_CMD_RTK_IPSEC) += crypto/rtk_v1_1/rtk_crypto_test.o
obj-$(CONFIG_CMD_RTK_IPSEC) += crypto/rtk_v1_1/rtk_crypto_interrupt.o
obj-$(CONFIG_CMD_RTK_IPSEC) += crypto/rtk_v1_1/rtk_crypto_rom.o
obj-$(CONFIG_CMD_RTK_IPSEC) += crypto/rtk_v1_1/hal_crypto.o
obj-$(CONFIG_CMD_RTK_IPSEC) += crypto/rtk_v1_1/cryptoSim.o
obj-$(CONFIG_CMD_RTK_IPSEC) += crypto/rtk_v1_1/cryptoSim_aes.o
obj-$(CONFIG_CMD_RTK_IPSEC) += crypto/rtk_v1_1/cryptoSim_des.o
obj-$(CONFIG_CMD_RTK_IPSEC) += crypto/rtk_v1_1/cryptoSim_md5.o
obj-$(CONFIG_CMD_RTK_IPSEC) += crypto/rtk_v1_1/cryptoSim_sha.o
#obj-$(CONFIG_CMD_RTK_IPSEC) += crypto/rtk_v1_1/fips.o
#obj-$(CONFIG_CMD_RTK_IPSEC) += crypto/rtk_v1_1/rngtest.o
#obj-$(CONFIG_CMD_RTK_IPSEC) += crypto/rtk_v1_1/stats.o

###============ mbedtls ====================== ###
obj-$(CONFIG_CMD_MBEDTLS) += mbedtls/mbedtls-2.2.1/mbedtls_test.o
obj-$(CONFIG_CMD_MBEDTLS) += mbedtls/mbedtls-2.2.1/mbedtls_src/library/memory_buffer_alloc.o
obj-$(CONFIG_CMD_MBEDTLS) += mbedtls/mbedtls-2.2.1/mbedtls_src/library/platform.o
obj-$(CONFIG_CMD_MBEDTLS) += mbedtls/mbedtls-2.2.1/mbedtls_src/library/bignum.o
obj-$(CONFIG_CMD_MBEDTLS) += mbedtls/mbedtls-2.2.1/mbedtls_src/library/sha256.o
obj-$(CONFIG_CMD_MBEDTLS) += mbedtls/mbedtls-2.2.1/mbedtls_src/library/rsa.o
#obj-$(CONFIG_CMD_MBEDTLS) += mbedtls/mbedtls-2.2.1/mbedtls_src/library/ecdsa.o
#obj-$(CONFIG_CMD_MBEDTLS) += mbedtls/mbedtls-2.2.1/mbedtls_src/library/ecp_curves.o
#obj-$(CONFIG_CMD_MBEDTLS) += mbedtls/mbedtls-2.2.1/mbedtls_src/library/ecp.o
obj-$(CONFIG_CMD_MBEDTLS) += mbedtls/mbedtls-2.2.1/mbedtls_src/library/asn1parse.o
obj-$(CONFIG_CMD_MBEDTLS) += mbedtls/mbedtls-2.2.1/mbedtls_src/library/asn1write.o
obj-$(CONFIG_CMD_MBEDTLS) += mbedtls/mbedtls-2.2.1/mbedtls_src/library/pk.o
obj-$(CONFIG_CMD_MBEDTLS) += mbedtls/mbedtls-2.2.1/mbedtls_src/library/pk_wrap.o
obj-$(CONFIG_CMD_MBEDTLS) += mbedtls/mbedtls-2.2.1/mbedtls_src/library/oid.o
obj-$(CONFIG_CMD_MBEDTLS) += mbedtls/mbedtls-2.2.1/mbedtls_src/library/pkparse.o
obj-$(CONFIG_CMD_MBEDTLS) += mbedtls/mbedtls-2.2.1/mbedtls_src/library/md.o
obj-$(CONFIG_CMD_MBEDTLS) += mbedtls/mbedtls-2.2.1/mbedtls_src/library/md_wrap.o
CFLAGS +=  -DMBEDTLS_CONFIG_FILE="<mbedtls_config.h>" \
         -DMBEDTLS_KEY_ALG_ID=MBEDTLS_RSA -DMBEDTLS_RSA_LENGTH=384

## MBEDTLS_ECDSA, MBEDTLS_RSA
### MBEDTLS_RSA_LENGTH 256 = 2048bit, 384 = 3072bit, 512 = 4096bit

###============ serial nand ============== ###
obj-$(CONFIG_CMD_RTK_SPI_NAND) += spi_nand/rtk_v1_0/rtk_spi_nand_base_test.o
obj-$(CONFIG_CMD_RTK_SPI_NAND) += spi_nand/rtk_v1_0/rtk_spi_nand_base.o
obj-$(CONFIG_CMD_RTK_SPI_NAND) += spi_nand/rtk_v1_0/rtk_spi_nand_ctrl.o
obj-$(CONFIG_CMD_RTK_SPI_NAND) += spi_nand/rtk_v1_0/ecc_ctrl.o

###============ serial nor ============== ###
obj-$(CONFIG_CMD_RTK_SPI_NOR) += spi_nor/rtk_v1_0/rtk_spi_nor_base_test.o
obj-$(CONFIG_CMD_RTK_SPI_NOR) += spi_nor/rtk_v1_0/rtk_spi_nor_base.o
obj-$(CONFIG_CMD_RTK_SPI_NOR) += spi_nor/rtk_v1_0/flash.o
obj-$(CONFIG_CMD_RTK_SPI_NOR) += spi_nor/rtk_v1_0/flash_device.o
obj-$(CONFIG_CMD_RTK_SPI_NOR) += spi_nor/rtk_v1_0/spi_flash.o

###============ parallel nand ============== ###
obj-$(CONFIG_CMD_RTK_PARALLEL_NAND) += parallel_nand/rtk_v1_0/rtk_nand_base_test.o
obj-$(CONFIG_CMD_RTK_PARALLEL_NAND) += parallel_nand/rtk_v1_0/rtk_nand_base.o

###============ efuse ====================== ###
obj-$(CONFIG_CMD_RTK_EFUSE) += efuse/rtk_v1_0/rtk_efuse_base.o
obj-$(CONFIG_CMD_RTK_EFUSE) += efuse/rtk_v1_0/rtk_efuse_base_test.o

###============ NFBI ====================== ###
obj-$(CONFIG_CMD_RTK_NFBI) += nfbi/rtk_v1_0/rtk_nfbi_base.o 
obj-$(CONFIG_CMD_RTK_NFBI) += nfbi/rtk_v1_0/rtk_nfbi_base_test.o
endif




ifeq ($(CONFIG_BOARD_rtl8198f),y)
###============ UART ====================== ###
obj-$(CONFIG_CMD_RTK_UART) += uart/rtk_v1_0/rtk_uart_base.o
obj-$(CONFIG_CMD_RTK_UART) += uart/rtk_v1_0/rtk_uart_base_test.o

###============ SPI ====================== ###
obj-$(CONFIG_CMD_RTK_SPI) += spi/dw_3_18a/dw_spi_base.o
obj-$(CONFIG_CMD_RTK_SPI) += spi/dw_3_18a/dw_spi_base_test.o

###============ GDMA ====================== ###
obj-$(CONFIG_CMD_RTK_GDMA) += gdma/dw_2_16a/dw_gdma_base.o
obj-$(CONFIG_CMD_RTK_GDMA) += gdma/dw_2_16a/dw_gdma_base_test.o

###============ I2C ====================== ###
obj-$(CONFIG_CMD_RTK_I2C) += i2c_dw/dw_1_14a/dw_i2c_base.o
obj-$(CONFIG_CMD_RTK_I2C) += i2c_dw/dw_1_14a/dw_i2c_base_test.o

#obj-$(CONFIG_CMD_RTK_I2C) += i2c_rtk/rtk_i2c_base.o
#obj-$(CONFIG_CMD_RTK_I2C) += i2c_rtk/rtk_i2c_base_test.o


###============ GPIO ====================== ###
#obj-$(CONFIG_CMD_RTK_GPIO) += gpio/dw_gpio_base.o
#obj-$(CONFIG_CMD_RTK_GPIO) += gpio/dw_gpio_base_test.o


###============ I2S ====================== ###
obj-$(CONFIG_CMD_RTK_I2S) += i2s/rtk_v1_1/rtk_i2s_base.o
obj-$(CONFIG_CMD_RTK_I2S) += i2s/rtk_v1_1/rtk_i2s_base_test.o


###============ PCIE ====================== ###
obj-$(CONFIG_CMD_RTK_PCIE) += pcie/dw_4_60a/dw_pcie_base.o
obj-$(CONFIG_CMD_RTK_PCIE) += pcie/dw_4_60a/dw_pcie_base_test.o


###============ PCM ====================== ###
obj-$(CONFIG_CMD_RTK_PCM) += pcm/rtk_v1_0/rtk_pcm_base.o
obj-$(CONFIG_CMD_RTK_PCM) += pcm/rtk_v1_0/rtk_pcm_base_test.o
obj-$(CONFIG_CMD_RTK_PCM) += pcm/rtk_v1_0/rtk_pcm_reg_test.o


###============ sdio_device ====================== ###
#obj-y += sdio_device/

###============ usb_otg ====================== ###
#obj-y += usb_otg/

###============ pwm ====================== ###
obj-$(CONFIG_CMD_RTK_PWM) += pwm/rtk_v1_0/rtk_pwm_base.o
obj-$(CONFIG_CMD_RTK_PWM) += pwm/rtk_v1_0/rtk_pwm_base_test.o

###============ timer ====================== ###
obj-$(CONFIG_CMD_RTK_TIMER) += timer/dw_v2_05a/dw_timer_base.o 
obj-$(CONFIG_CMD_RTK_TIMER) += timer/dw_v2_05a/dw_timer_base_test.o

###============ ipsec/crypto ====================== ###
obj-$(CONFIG_CMD_ELL_IPSEC) += crypto/elliptic_v1_0/rtk_crypto_test.o 
obj-$(CONFIG_CMD_ELL_IPSEC) += crypto/elliptic_v1_0/rtk_crypto_interrupt.o 
obj-$(CONFIG_CMD_ELL_IPSEC) += crypto/elliptic_v1_0/rtk_crypto_rom.o 
obj-$(CONFIG_CMD_ELL_IPSEC) += crypto/elliptic_v1_0/hal_crypto.o 
obj-$(CONFIG_CMD_ELL_IPSEC) += crypto/elliptic_v1_0/cryptoSim.o 
obj-$(CONFIG_CMD_ELL_IPSEC) += crypto/elliptic_v1_0/cryptoSim_aes.o 
obj-$(CONFIG_CMD_ELL_IPSEC) += crypto/elliptic_v1_0/cryptoSim_des.o 
obj-$(CONFIG_CMD_ELL_IPSEC) += crypto/elliptic_v1_0/cryptoSim_md5.o 
obj-$(CONFIG_CMD_ELL_IPSEC) += crypto/elliptic_v1_0/cryptoSim_sha.o 
#obj-$(CONFIG_CMD_ELL_IPSEC) += crypto/elliptic_v1_0/fips.o 
#obj-$(CONFIG_CMD_ELL_IPSEC) += crypto/elliptic_v1_0/rngtest.o 
#obj-$(CONFIG_CMD_ELL_IPSEC) += crypto/elliptic_v1_0/stats.o 

###============ serial nand ============== ###
obj-$(CONFIG_CMD_RTK_SPI_NAND) += spi_nand/ca_v1_0/rtk_spi_nand_base_test.o
obj-$(CONFIG_CMD_RTK_SPI_NAND) += spi_nand/ca_v1_0/rtk_spi_nand_base.o

###============ serial nor ============== ###
obj-$(CONFIG_CMD_RTK_SPI_NOR) += spi_nor/ca_v1_0/rtk_spi_nor_base_test.o
obj-$(CONFIG_CMD_RTK_SPI_NOR) += spi_nor/ca_v1_0/rtk_spi_nor_base.o

###============ parallel nand ============== ###
obj-$(CONFIG_CMD_RTK_PARALLEL_NAND) += parallel_nand/ca_v1_0/rtk_para_nand_base_test.o
obj-$(CONFIG_CMD_RTK_PARALLEL_NAND) += parallel_nand/ca_v1_0/rtk_para_nand_base.o
obj-$(CONFIG_CMD_RTK_PARALLEL_NAND) += parallel_nand/ca_v1_0/nand.o
obj-$(CONFIG_CMD_RTK_PARALLEL_NAND) += parallel_nand/ca_v1_0/nand_base.o
obj-$(CONFIG_CMD_RTK_PARALLEL_NAND) += parallel_nand/ca_v1_0/nand_bbt.o
obj-$(CONFIG_CMD_RTK_PARALLEL_NAND) += parallel_nand/ca_v1_0/nand_ids.o
obj-$(CONFIG_CMD_RTK_PARALLEL_NAND) += parallel_nand/ca_v1_0/nand_util.o
obj-$(CONFIG_CMD_RTK_PARALLEL_NAND) += parallel_nand/ca_v1_0/nand_ecc.o

###============ pll clk ============== ###
obj-$(CONFIG_CMD_RTK_PLL_CLK) += pll_clk/pll_clk_v1_0/rtk_pll_clk_base_test.o
obj-$(CONFIG_CMD_RTK_PLL_CLK) += pll_clk/pll_clk_v1_0/rtk_pll_clk_base.o

###============ MISC ====================== ###
obj-$(CONFIG_CMD_MISC_IP) += misc/misc_v1_0/misc_base.o
obj-$(CONFIG_CMD_MISC_IP) += misc/misc_v1_0/misc_base_test.o
obj-$(CONFIG_CMD_MISC_IP) += misc/misc_v1_0/misc_a17_cpu.o

endif

ifeq ($(CONFIG_CMD_RTK_WLAN_8197G),y)
###============ WLAN ====================== ###
obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_97g/rtk_wlan_base.o
obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_97g/rtk_wlan_base_test.o
obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_97g/wl_common_ctrl.o
obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_97g/wl_common_hw.o
obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_97g/wl_fw_verify.o
obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_97g/wl_hw_init.o
obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_97g/wl_log.o
obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_97g/wl_security.o
obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_97g/wl_wapi.o
obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_97g/OUTSRC/data/HalHWImg_MAC.o
obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_97g/OUTSRC/data/odm_RegConfig.o
obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_97g/OUTSRC/odm_interface.o
obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_97g/HalPwrSeqCmd.o
endif

ifeq ($(CONFIG_CMD_RTK_WLAN_8198F),y)
###============ WLAN ====================== ###
obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_98f/rtk_wlan_base.o
obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_98f/rtk_wlan_base_test.o
obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_98f/wl_common_ctrl.o
obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_98f/wl_common_hw.o
obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_98f/wl_fw_verify.o
obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_98f/wl_hw_init.o
obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_98f/wl_log.o
obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_98f/wl_security.o
obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_98f/wl_wapi.o
obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_98f/OUTSRC/data/HalHWImg_MAC.o
obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_98f/OUTSRC/data/odm_RegConfig.o
obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_98f/OUTSRC/odm_interface.o
obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_98f/HalPwrSeqCmd.o
endif
ifeq ($(CONFIG_CMD_RTK_WLAN_8197F),y)
###============ WLAN ====================== ###
obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_96f/rtk_wlan_base.o
obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_96f/rtk_wlan_base_test.o
obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_96f/wl_common_ctrl.o
obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_96f/wl_common_hw.o
obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_96f/wl_fw_verify.o
obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_96f/wl_hw_init.o
obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_96f/wl_log.o
obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_96f/wl_security.o
obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_96f/wl_wapi.o
obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_96f/OUTSRC/data/HalHWImg_MAC.o
obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_96f/OUTSRC/data/odm_RegConfig.o
obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_96f/OUTSRC/odm_interface.o
obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_96f/HalPwrSeqCmd.o
endif
ifeq ($(CONFIG_CMD_RTK_WLAN_8814A),y)
###============ WLAN ====================== ###
obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_8814A/rtk_wlan_base.o
obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_8814A/rtk_wlan_base_test.o
obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_8814A/wl_common_ctrl.o
obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_8814A/wl_common_hw.o
obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_8814A/wl_fw_verify.o
obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_8814A/wl_hw_init.o
obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_8814A/wl_log.o
obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_8814A/wl_security.o
obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_8814A/wl_wapi.o
obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_8814A/OUTSRC/data/HalHWImg_MAC.o
obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_8814A/OUTSRC/data/odm_RegConfig.o
obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_8814A/OUTSRC/odm_interface.o
obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_8814A/HalPwrSeqCmd.o
endif
ifeq ($(CONFIG_CMD_RTK_WLAN_MP_8198F_8814A),y)
###============ WLAN ====================== ###
####the compiling sequence is important, dont change dynamically
obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_98f/rtk_wlan_base.o
obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_98f/rtk_wlan_base_test.o
obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_98f/wl_common_ctrl.o
obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_98f/wl_common_hw.o
obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_98f/wl_fw_verify.o
obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_98f/wl_hw_init.o
obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_98f/wl_log.o
obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_98f/wl_security.o
obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_98f/wl_wapi.o
obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_98f/OUTSRC/data/HalHWImg_MAC.o
obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_98f/OUTSRC/data/odm_RegConfig.o
obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_98f/OUTSRC/odm_interface.o
obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_98f/HalPwrSeqCmd.o

obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_8814A/rtk_wlan_base.o
obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_8814A/wl_hw_init.o
obj-$(CONFIG_CMD_RTK_WLAN) += wlan/rtk_8814A/OUTSRC/data/HalHWImg_MAC.o


endif

###==========8852A====================== ###
ifeq ($(CONFIG_BOARD_rtl8852a),y)
# empty
endif

ifeq ($(CONFIG_CMD_RTK_WLAN_8852A),y)
#empty
endif
###============ RTC ====================== ###
#obj-$(CONFIG_CMD_CA_RTC) += rtc/ca_v1_0/ca_rtc_base.o
#obj-$(CONFIG_CMD_CA_RTC) += rtc/ca_v1_0/ca_rtc_base_test.o


###============ WATCHDOG ================== ###
#obj-$(CONFIG_CMD_RTK_WATCHDOG) += watchdog/rtk_v1_1/rtk_watchdog_base_test.o

###============ BUS ====================== ###
#obj-$(CONFIG_CMD_BUS) += bus/rtk_v1_0/bus_base.o
#obj-$(CONFIG_CMD_BUS) += bus/rtk_v1_0/bus_base_test.o
