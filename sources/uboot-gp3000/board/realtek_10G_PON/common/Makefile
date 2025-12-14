obj-$(CONFIG_OF_SYSTEM_SETUP) += fdt_fixup.o
obj-$(CONFIG_CORTINA_UART) += serial_cortina.o
obj-$(CONFIG_CORTINA_SPINAND_BBT) += spi_nand/spi-nand-bbt.o
obj-$(CONFIG_CORTINA_SERIAL_NAND) += spi_nand/cortina_spi_nand.o
obj-$(CONFIG_CORTINA_SERIAL_NAND) += spi_nand/cortina_spi_nand_base.o
obj-$(CONFIG_CORTINA_SERIAL_NAND) += spi_nand/spi-nand-core.o
obj-${CONFIG_CMD_SPI_NAND_TEST_FUNC} += spi_nand/cmd_snaf_test.o
obj-$(CONFIG_CORTINA_SERIAL_NAND)  += spi_nand/cmd_spi_nand_bbt.o
obj-$(CONFIG_CORTINA_PNAND) += pnand/ca_nand.o
obj-$(CONFIG_MTD_CORTINA_SERIAL_NOR) += nor/serial_nor.o
obj-$(CONFIG_MTD_CORTINA_SERIAL_NOR) += nor/serial_nor_base.o
obj-${CONFIG_CMD_SPI_NOR_TEST_FUNC} += nor/cmd_spi_nor_test.o
obj-${CONFIG_MMC_DW_CORTINA} += ca_dw_mmc.o
obj-${CONFIG_MMC_RTK_EMMC} += mmc_driver_wrapper.o
obj-$(CONFIG_CMD_PART) += cmd_partinfo.o
CFLAGS_mmc_driver_wrapper.o += -I./drivers/mmc/ -I./drivers/mmc/mmc_cr_v1/sihnah_v1/

obj-$(CONFIG_NET) += cortina_ni.o
obj-y += cmd_upimgtar.o
obj-y += cmd_upvmimg.o
# obj-$(CONFIG_CORTINA_LED) += cortina_led.o
obj-$(CONFIG_RTK_WATCHDOG) += cmd_rtk_watchdog.o
obj-$(CONFIG_MULTICAST_UPGRADE) += multicast_upgrade.o
obj-${CONFIG_CMD_OTP} += smc_call/cmd_otp.o
obj-${CONFIG_CMD_CRYPTO} += smc_call/cmd_cryptography.o
obj-y += smc_call/cmd_bootloader_ver.o smc_call/cortina_smc.o
obj-y += cmd_uploadername.o
obj-y += cmd_ram_mt.o mdram_test.o
obj-${CONFIG_TARGET_RTK_VENUS_FT} += 8277B_FT/cmd_ft2.o
obj-${CONFIG_TARGET_RTK_VENUS_FT} += 8277B_FT/i2c-cortina.o
obj-${CONFIG_TARGET_RTK_VENUS_FT} += 8277B_FT/cmd_ne.o
obj-${CONFIG_TARGET_RTK_VENUS_FT} += 8277B_FT/xhci-ca7774.o

# socCmd
obj-$(CONFIG_CMD_SOC_TEST) += socCmd/

