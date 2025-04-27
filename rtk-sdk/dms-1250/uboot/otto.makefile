OTTO_INC_DIR := arch/otto40/plr/src/include
OTTO_IP_DIR  := arch/otto40/plr/src/soc

OTTO_XENV_H := include/asm/arch/cross_env.h
OTTO_RMU_H  := include/asm/arch/reg_map_util.h

ifeq ($(CONFIG_SPI_NOR_FLASH), y)
OTTO_UBOOT_SNOF_DRV_NAME := nor_spi_gen3
OTTO_UBOOT_SNOF_DRV_PATH := arch/otto40/lib/$(OTTO_UBOOT_SNOF_DRV_NAME)
export OTTO_UBOOT_SNOF_DRV_NAME OTTO_UBOOT_SNOF_DRV_PATH
endif

ifeq ($(CONFIG_SPI_NAND_FLASH), y)
ifeq ($(CONFIG_SPI_NAND_FLASH_G3), y)
OTTO_UBOOT_SNAF_DRV_NAME := spi_nand_gen3
else
OTTO_UBOOT_SNAF_DRV_NAME := spi_nand_gen2
endif
OTTO_UBOOT_SNAF_DRV_PATH := arch/otto40/lib/$(OTTO_UBOOT_SNAF_DRV_NAME)
export OTTO_UBOOT_SNAF_DRV_NAME OTTO_UBOOT_SNAF_DRV_PATH
endif

ifeq ($(CONFIG_ECC_ENGINE), y)
ifeq ($(CONFIG_ECC_ENGINE_G2), y)
OTTO_UBOOT_ECC_DRV_NAME := ecc_gen2
else
OTTO_UBOOT_ECC_DRV_NAME := ecc_gen1
endif
OTTO_UBOOT_ECC_DRV_PATH := arch/otto40/lib/$(OTTO_UBOOT_ECC_DRV_NAME)
export OTTO_UBOOT_ECC_DRV_NAME OTTO_UBOOT_ECC_DRV_PATH
endif

.PHONY: $(OTTO_SOC_H) .otto.v

$(OTTO_SOC_H): otto_arch_sync
	@if [ ! -e $(OTTO_RELEASE_DIR)/conf.h ]; then \
		if [ ! -e $@ ]; then \
			echo "EE: Missing $@" ; \
			exit 1; \
		fi \
	else \
		cmp $@ $(OTTO_RELEASE_DIR)/conf.h > /dev/null 2>&1 ; \
		if [ "$$?" -ne 0 ]; then \
			echo -n "[MISC] Generating $@... " ; \
			sed -e '/^#define USE_ON_FLASH_SECTION$$/d' -e '/^#define .\+SECTION.\+/d' $(OTTO_RELEASE_DIR)/conf.h > $@ ; \
			echo "done" ; \
		fi \
	fi

otto_arch_sync:
	@$(call get_otto_file, $(OTTO_XENV_H),   $(OTTO_INC_DIR)/cross_env.h)
	@$(call get_otto_file, $(OTTO_RMU_H),    $(OTTO_INC_DIR)/reg_map_util.h)
ifeq ($(CONFIG_SPI_NOR_FLASH), y)
	@$(call get_otto_dir,  $(OTTO_NORSF_G3), $(OTTO_IP_DIR)/nor_spi_gen3)
	@cd ./arch/otto40/include/asm/arch && rm -f nor_spi && ln -s ../../../lib/nor_spi_gen3 nor_spi
endif
ifeq ($(CONFIG_SPI_NAND_FLASH), y)
	@$(call get_otto_dir,  $(OTTO_UBOOT_SNAF_DRV_PATH), $(OTTO_IP_DIR)/$(OTTO_UBOOT_SNAF_DRV_NAME))
	@cd ./arch/otto40/include/asm/arch && rm -f spi_nand && ln -s ../../../lib/$(OTTO_UBOOT_SNAF_DRV_NAME) spi_nand
endif
ifeq ($(CONFIG_ECC_ENGINE), y)
	@$(call get_otto_dir,  $(OTTO_UBOOT_ECC_DRV_PATH), $(OTTO_IP_DIR)/$(OTTO_UBOOT_ECC_DRV_NAME))
	@cd ./arch/otto40/include/asm/arch && rm -f ecc && ln -s ../../../lib/$(OTTO_UBOOT_ECC_DRV_NAME) ecc 
endif

#DISTCLEAN_FILE += $(shell find ./arch/otto40/include/asm -name "soc.h" -o -name "reg_map_util.h" -o -name "cross_env.h" -o -name "otto_cg_dev_freq.h" -o -name "timer.h" -o -name "cpu.h" -o -name "osc.h" -o -name "misc_setting.h")
#DISTCLEAN_FILE += $(shell find ./arch/otto40/lib -name "nor_spif_proj_dep.c")
#DISTCLEAN_FOLDER += $(shell find ./arch/otto40/include/asm -name "nor_spi" -o -name "spi_nand" -o -name "ecc" -o -name "onfi")
#DISTCLEAN_FOLDER += $(shell find ./arch/otto40/lib -name "$(OTTO_UBOOT_SNOF_DRV_NAME)" -o -name "spi_nand*" -o -name "ecc_gen*" -o -name "onfi*")

#MASK this Target to avoid error distclean.
# otto_distclean:
#	@-rm -f $(DISTCLEAN_FILE) $(OTTO_SOC_H) $(OTTO_XENV_H) $(OTTO_RMU_H) .otto.E .otto.mk
#	@-rm -rf $(CPUDIR)/$(SOC)/preloader $(DISTCLEAN_FOLDER)

