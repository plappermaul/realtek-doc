NOWDIR  = $(shell pwd)
RTK_SPINOR_DIR := $(NOWDIR)/drivers/mtd/maps/rtk_norsf_g3/

EXTRA_CFLAGS += -D__LUNA_KERNEL__
EXTRA_CFLAGS += -I$(RTK_SPINOR_DIR)nor_spi
EXTRA_CFLAGS += -I$(RTK_SPINOR_DIR)

norsf_flist := nor_spif_core.o
norsf_flist += nor_spif_mxic.o
norsf_flist += nor_spif_spansion.o
norsf_flist += nor_spif_winbond.o
norsf_flist += nor_spif_gd.o
norsf_flist += nor_spif_micron.o

obj-y += $(norsf_flist)

#################################################

OTTO_SRC := /home/jiajheli/projects/150515_9300_sdk/sdk_main/trunk/u-boot-2011.12/arch/otto40
KERNEL_ROOT_DIR := /home/jiajheli/projects/150515_9300_sdk/sdk_main/trunk/kernel/uClinux/linux-2.6.x
SDK_NORSFG2_DIR := $(KERNEL_ROOT_DIR)/drivers/mtd/maps/rtk_norsf_g2

prep:
	cp $(OTTO_SRC)/plr/src/soc/nor_spi_gen2/*.c $(SDK_NORSFG2_DIR)
	cp $(OTTO_SRC)/plr/src/soc/nor_spi_gen2/*.h $(SDK_NORSFG2_DIR)
	cp $(OTTO_SRC)/plr/src/include/cross_env.h ../../../../include
	cp $(OTTO_SRC)/plr/src/include/reg_map_util.h ../../../../include
	cp $(OTTO_SRC)/include/asm/arch-rtl9300/soc.h ../../../../include
	cp $(OTTO_SRC)/lib/nor_spif_proj_dep.c .
