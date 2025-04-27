#
# (C) Copyright 2003-2006
# Wolfgang Denk, DENX Software Engineering, wd@denx.de.
#
# See file CREDITS for list of people who contributed to this
# project.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of
# the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston,
# MA 02111-1307 USA
#

include $(TOPDIR)/config.mk

LIB	= $(obj)lib$(ARCH).o

SOBJS-y	+=
COBJS-y	+= board.o bootm.o ashldi3.o ashrdi3.o lshrdi3.o
COBJS-y += otto_uart.o otto_time.o otto_sym.o otto_cg.o otto_util.o

COBJS-$(CONFIG_SPI_NOR_FLASH) += otto_nor_spi_flash.o

COBJS-$(CONFIG_NOR_SPIF_G3) += nor_spi_gen3/nor_spif_core.o nor_spif_proj_dep.o

COBJS-$(CONFIG_SPI_NOR_FLASH) += nor_spi_gen3/nor_spif_mxic.o
COBJS-$(CONFIG_SPI_NOR_FLASH) += nor_spi_gen3/nor_spif_spansion.o
COBJS-$(CONFIG_SPI_NOR_FLASH) += nor_spi_gen3/nor_spif_winbond.o
COBJS-$(CONFIG_SPI_NOR_FLASH) += nor_spi_gen3/nor_spif_gd.o
COBJS-$(CONFIG_SPI_NOR_FLASH) += nor_spi_gen3/nor_spif_micron.o

#COBJS-$(CONFIG_OTTO_FLASH_LAYOUT) += otto_flash_layout.o

ifeq ($(CONFIG_SPI_NAND_FLASH),y)
	COBJS-y += otto_spi_nand_flash.o
	COBJS-y += otto_spi_nand_bb_util.o
	ifeq ($(CONFIG_SPI_NAND_FLASH_G2),y)
		OTTO_SPI_NAND_GEN := 2
	else
		OTTO_SPI_NAND_GEN := 3
	endif
		ifeq ($(CONFIG_SPI_NAND_FLASH_INIT_FIRST),y)
			COBJS-y += $(OTTO_UBOOT_SNAF_DRV_NAME)/spi_nand_common.o
			COBJS-y += $(OTTO_UBOOT_SNAF_DRV_NAME)/spi_nand_ctrl.o
			COBJS-y += $(OTTO_UBOOT_SNAF_DRV_NAME)/spi_nand_util.o
		endif
                ifeq ($(CONFIG_SPI_NAND_FLASH_G3), y)
                    COBJS-y += $(OTTO_UBOOT_SNAF_DRV_NAME)/spi_nand_ato.o
                    COBJS-y += $(OTTO_UBOOT_SNAF_DRV_NAME)/spi_nand_dosilicon.o
                    COBJS-y += $(OTTO_UBOOT_SNAF_DRV_NAME)/spi_nand_esmt.o
                    COBJS-y += $(OTTO_UBOOT_SNAF_DRV_NAME)/spi_nand_etron.o
                    COBJS-y += $(OTTO_UBOOT_SNAF_DRV_NAME)/spi_nand_fmsh.o
                    COBJS-y += $(OTTO_UBOOT_SNAF_DRV_NAME)/spi_nand_heyangtek.o
                    COBJS-y += $(OTTO_UBOOT_SNAF_DRV_NAME)/spi_nand_longsys.o
                    COBJS-y += $(OTTO_UBOOT_SNAF_DRV_NAME)/spi_nand_micron.o
                    COBJS-y += $(OTTO_UBOOT_SNAF_DRV_NAME)/spi_nand_xtx.o
                endif
		COBJS-y += $(OTTO_UBOOT_SNAF_DRV_NAME)/spi_nand_gd.o
		COBJS-y += $(OTTO_UBOOT_SNAF_DRV_NAME)/spi_nand_mxic.o
		COBJS-y += $(OTTO_UBOOT_SNAF_DRV_NAME)/spi_nand_toshiba.o
		COBJS-y += $(OTTO_UBOOT_SNAF_DRV_NAME)/spi_nand_winbond.o
		COBJS-y += $(OTTO_UBOOT_SNAF_DRV_NAME)/spi_nand_zentel.o
	CPPFLAGS_arch/otto40/lib/otto_spi_nand_flash.o := -DOTTO_SPI_NAND_GEN=$(OTTO_SPI_NAND_GEN)
	#CPPFLAGS_arch/otto40/lib/otto_spi_nand_flash.o := -DOTTO_SPI_NAND_GEN=$(subst spi_nand_gen,,$(shell echo $(CONFIG_SPI_NAND_FLASH_GEN)))
endif

ifeq ($(CONFIG_ECC_ENGINE),y)
	ifeq ($(CONFIG_ECC_ENGINE_INIT),y)
		ifeq ($(CONFIG_ECC_ENGINE_G1), y)
			COBJS-y += ecc_gen1/ecc_ctrl.o
		else
			COBJS-y += ecc_gen2/ecc_ctrl.o
		endif
	endif
endif


CPPFLAGS_arch/otto40/lib/otto_sym.o := -DSECTION_SYMACCESS=

SRCS	:= $(SOBJS-y:.o=.S) $(COBJS-y:.o=.c)
OBJS	:= $(addprefix $(obj),$(SOBJS-y) $(COBJS-y) $(INC))

$(LIB):	$(obj).depend $(OBJS)
	$(call cmd_link_o_target, $(OBJS))

#########################################################################

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

	sinclude $(obj).depend

#########################################################################

