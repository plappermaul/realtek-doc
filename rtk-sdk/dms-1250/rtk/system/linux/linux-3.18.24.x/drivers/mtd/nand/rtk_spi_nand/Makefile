CURRDIR  = $(shell pwd)
RTK_SPINAND_DIR := $(CURRDIR)/drivers/mtd/nand/rtk_spi_nand/
# EXTRA_CFLAGS += -I$(RTK_SPINAND_DIR)/include/ -I$(RTK_SPINAND_DIR)/spi_nand_gen2/include/ -D__LUNA_KERNEL__ -DCONFIG_UNDER_UBOOT -DOTTO_SPI_NAND_GEN=2 -DSNAF_G2_WITHOUT_DUMMY_READY=0 -DECC_G1_WITHOUT_DUMMY_READY=0 -DCONFIG_SPI_NAND_FLASH_INIT_FIRST -DCONFIG_SPI_NAND_FLASH_INIT_REST -DCONFIG_MTD_NAND_MULTI_RTK=1

EXTRA_CFLAGS += -Wno-declaration-after-statement

ifeq ($(CONFIG_SDK_BSP_MTD_RTK_SPI_NAND), y)
EXTRA_CFLAGS += -DCONFIG_MTD_SPI_NAND_RTK
endif

ifeq ($(CONFIG_SDK_BSP_MTD_SPI_NAND_G2), y)
SPI_NAND_PATH  := spi_nand_gen2
else
SPI_NAND_PATH  := spi_nand_gen3
endif

ifeq ($(CONFIG_SDK_BSP_MTD_ECC_G1), y)
ECC_PATH       := ecc
else
ECC_PATH       := ecc_gen2
endif

EXTRA_CFLAGS += -I$(RTK_SPINAND_DIR)/include/ -I$(RTK_SPINAND_DIR)/$(SPI_NAND_PATH)/include/ -D__LUNA_KERNEL__ -DCONFIG_UNDER_UBOOT -DOTTO_SPI_NAND_GEN=2 -DSNAF_G2_WITHOUT_DUMMY_READY=0 -DECC_G1_WITHOUT_DUMMY_READY=0 -DCONFIG_SPI_NAND_FLASH_INIT_FIRST -DCONFIG_SPI_NAND_FLASH_INIT_REST -DCONFIG_MTD_NAND_MULTI_RTK=1
#EXTRA_CFLAGS = -I$(src) -I$(src)/spi_nand -I$(src)/ecc

ccflags-y := -I$(src) -I$(src)/$(ECC_PATH) -I../
ccflags-$(CONFIG_SDK_BSP_MTD_RTK_SPI_NAND) += -I$(src)/$(SPI_NAND_PATH)
ccflags-$(CONFIG_MTD_ONFI_NAND_RTK) += -I$(src)/$(ONFI_NAND_PATH)

spin_flist := spi_nand_common.o
spin_flist += spi_nand_ctrl.o
spin_flist += spi_nand_winbond.o
spin_flist += spi_nand_gd.o
spin_flist += spi_nand_zentel.o
spin_flist += spi_nand_toshiba.o
spin_flist += spi_nand_mxic.o
spin_flist += spi_nand_xtx.o
ifeq ($(SPI_NAND_PATH), spi_nand_gen3)
spin_flist += spi_nand_ato.o
spin_flist += spi_nand_dosilicon.o
spin_flist += spi_nand_esmt.o
spin_flist += spi_nand_etron.o
spin_flist += spi_nand_fmsh.o
spin_flist += spi_nand_heyangtek.o
spin_flist += spi_nand_longsys.o
spin_flist += spi_nand_micron.o
endif
spin_flist := $(addprefix $(SPI_NAND_PATH)/,$(spin_flist))

ecc_flist := ecc_ctrl.o
ifeq ($(ECC_PATH), ecc)
ecc_flist += bch6_software_encode.o
endif
ecc_flist := $(addprefix $(ECC_PATH)/,$(ecc_flist))

obj-y += $(ecc_flist)
obj-$(CONFIG_SDK_BSP_MTD_RTK_SPI_NAND)  += $(spin_flist)
obj-y += luna_mtd_protect.o
obj-y += luna_mtd_nand.o
obj-y += rtk_spi_nand_drv.o
