# NOWDIR  = $(shell pwd)
NOWDIR  = $(shell pwd)
RTK_SPINOR_DIR := $(NOWDIR)/drivers/mtd/maps/rtk_norsf_g3
EXTRA_CFLAGS = -I$(RTK_SPINOR_DIR)/ -I$(RTK_SPINOR_DIR)/nor_spi/

obj-y += rtk_norsf_g3.o
obj-y += spi_probe.o spi_cmd.o

# norsf_flist := nor_spif_core.o
# norsf_flist += nor_spif_micron_rev.o
# norsf_flist += nor_spif_mxic_3b_mode.o nor_spif_mxic_4b_cmd.o
# norsf_flist := $(addprefix nor_spi/,$(norsf_flist))

# obj-y += $(norsf_flist)
obj-y += nor_spi/

