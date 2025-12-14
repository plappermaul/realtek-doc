include $(srctree)/arch/otto/function.makefile

CONFIG_OTTO_SNOF_GEN := 3
OTTO_SNOF_DRV_NAME := nor_spi_gen3

CONFIG_OTTO_SNAF_GEN ?= 1
OTTO_SNAF_DRV_NAME := spi_nand_gen$(CONFIG_OTTO_SNAF_GEN)

CONFIG_OTTO_ECC_GEN ?= 1
OTTO_ECC_DRV_NAME := ecc_gen$(CONFIG_OTTO_ECC_GEN)
obj-$(CONFIG_LUNA_MULTI_BOOT) += cmd_upimgtar.o
obj-$(CONFIG_LUNA_MULTI_BOOT) += cmd_upvmimg.o

obj-$(CONFIG_MULTICAST_UPGRADE) += multicast_upgrade.o

PLATFORM_CPPFLAGS += -I./board/$(VENDOR)/common -I$(OTTO_MIRROR)
PLATFORM_CPPFLAGS += -DCONFIG_UNDER_UBOOT

obj-y += soc_common.o
obj-y += osc_init.o
obj-y += cg_wrapper.o

# support u-boot cache command
obj-$(CONFIG_CMD_CACHE) += mips_cache_operations.o
obj-$(CONFIG_CMD_BOOT_B2) += boot_b2.o
# software patches
obj-$(CONFIG_BOARD_LATE_INIT) += swp.o

MIR_SNOF_DIR := mirror/nor_spi
MIR_SNAF_DIR := mirror/spi_nand
MIR_ECC_DIR  := mirror/ecc

# SPI-NOR
obj-$(CONFIG_CMD_SF) += snof_driver_wrapper.o
obj-$(CONFIG_CMD_SF) += $(MIR_SNOF_DIR)/nor_spif_core.o
obj-$(CONFIG_CMD_SF) += $(MIR_SNOF_DIR)/nor_spif_mxic.o
obj-$(CONFIG_CMD_SF) += $(MIR_SNOF_DIR)/nor_spif_winbond.o
obj-$(CONFIG_CMD_SF) += $(MIR_SNOF_DIR)/nor_spif_gd.o
obj-$(CONFIG_CMD_SF) += $(MIR_SNOF_DIR)/nor_spif_spansion.o
obj-$(CONFIG_CMD_SF) += $(MIR_SNOF_DIR)/nor_spif_micron.o

# SPI-NAND
obj-$(CONFIG_CMD_NAND) += snaf_driver_wrapper.o
ifeq ($(CONFIG_SPI_NAND_FLASH_INIT_FIRST),y)
obj-$(CONFIG_CMD_NAND) += $(MIR_SNAF_DIR)/spi_nand_common.o
obj-$(CONFIG_CMD_NAND) += $(MIR_SNAF_DIR)/spi_nand_ctrl.o
obj-$(CONFIG_CMD_NAND) += $(MIR_SNAF_DIR)/spi_nand_util.o
endif
ifeq ($(CONFIG_OTTO_SNAF_GEN), 2)
obj-$(CONFIG_CMD_NAND) += $(MIR_SNAF_DIR)/spi_nand_patch.o
obj-y += otto_sym.o
endif

# ECC
obj-${CONFIG_ECC_INIT} += ${MIR_ECC_DIR}/ecc_ctrl.o

OTTO_SOC_DIR := $(OTTO_TOP)/src/soc
OTTO_SPI_NOR_DIR := $(OTTO_MIRROR)/nor_spi
OTTO_SPI_NAND_DIR := $(OTTO_MIRROR)/spi_nand
OTTO_ECC_DIR := $(OTTO_MIRROR)/ecc

# this target is the entry to sync. common modules between Otto and U-Boot with
# $(get_otto_file) and $(get_otto_dir) defined in function.makefile.
PHONY += otto_common_sync
otto_common_sync:
ifeq ($(CONFIG_CMD_NAND), y)
	@$(call get_otto_dir,$(OTTO_SPI_NAND_DIR),$(OTTO_SOC_DIR)/$(OTTO_SNAF_DRV_NAME))
	@$(call get_otto_dir,$(OTTO_ECC_DIR),$(OTTO_SOC_DIR)/$(OTTO_ECC_DRV_NAME))
endif
ifeq ($(CONFIG_CMD_SF), y)
	@$(call get_otto_dir,$(OTTO_SPI_NOR_DIR),$(OTTO_SOC_DIR)/$(OTTO_SNOF_DRV_NAME))
endif

# Memory test
obj-$(CONFIG_CMD_DRAM_TEST) += memctl_dram_test.o
