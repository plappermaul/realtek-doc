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

PLR_DIR       := $(OTTO_RELEASE_DIR)/../preloader
PLR_HEADERS   := $(PLR_DIR)/include
PLR_PLATFORM  := $(PLR_DIR)/platform/current
PLR_COMMON    := $(PLR_DIR)

PLR_LDIR      := preloader
PLR_LHEADERS  := $(PLR_LDIR)/include
PLR_LPLATFORM := $(PLR_LDIR)/platform/current
PLR_LCOMMON   := $(PLR_LDIR)

PLR_COMM_HDRS := include/preloader.h include/stddef.h include/plr_pll_gen2.h include/plr_spare.h
PLR_PLAT_HDRS := cpu/mips-generic.h cpu/addrspace.h cpu/cacheops.h cpu/cpu_utils.h cpu/lplr_cpu_dep_cache.H \
                 lplr_sram.H bspchip.h idmem_dma.h \
                 dram/plr_dram_gen2_memctl.h dram/concur_test.h dram/plr_dram_gen2.h dram/plr_plat_dep.h \
                 concur_gdma.h concur_idmem.h concur_cpu.h

PLR_DRAM_SOBJS  := plr_platform.o
PLR_PLAT_SOBJS  := cpu/lplr_cpu.o cpu/lplr_cache.o $(PLR_DRAM_SOBJS)

PLR_UART_COBJS  := plr_uart.o
PLR_PLL_COBJS   := plr_pll.o plr_pll_gen2.o plr_pll_gen2_comp.o
PLR_DRAM_COBJS  := dram/plr_dram_gen2.o dram/plr_memctl_ddr2.o dram/plr_memctl_ddr3.o \
                   plr_plat_dep.o plr_concur_test.o plr_concur_cpu.o dram/plr_memctl_cali_dram.o
PLR_PLAT_COBJS  := $(PLR_UART_COBJS) $(PLR_PLL_COBJS) $(PLR_DRAM_COBJS)

PLR_COMM_COBJS  := plr_tutils_printf.o plr_spare.o plr_tutils.o

MODEL_INFO := $(TOPDIR)/model_info

PLR_INCDIR := $(PLR_LHEADERS) $(PLR_LPLATFORM)/cpu
PLR_INCDIR := $(addprefix -I$(shell pwd)/,$(PLR_INCDIR)) -I$(MODEL_INFO)

LIB	= $(obj)lib$(SOC).o

COBJS = soc_init_c_start.o soc_init_parameters.o \
        $(addprefix $(PLR_LPLATFORM)/, $(PLR_PLAT_COBJS)) \
        $(addprefix $(PLR_LCOMMON)/,   $(PLR_COMM_COBJS))
SOBJS = soc_init.o \
        $(addprefix $(PLR_LPLATFORM)/,$(PLR_PLAT_SOBJS))

SRCS	:= $(SOBJS:.o=.S) $(COBJS:.o=.c)
OBJS	:= $(addprefix $(obj),$(LIBOBJS) $(SOBJS) $(COBJS))

CFLAGS_arch/mips/cpu/mips34kc/rtl839x   := -mlong-calls
CPPFLAGS_arch/mips/cpu/mips34kc/rtl839x := $(PLR_INCDIR) -DDECOMP_ALGO=\"none\" -DDATE_TAG="\"$(PL_VER)\""
AFLAGS_arch/mips/cpu/mips34kc/rtl839x   := $(PLR_INCDIR)
CPPFLAGS += $(CPPFLAGS_arch/mips/cpu/mips34kc/rtl839x)

all: $(obj).depend $(LIB)

.PHONY : preloader

preloader:
	@if [ ! -d preloader/include ]; then \
		mkdir -p preloader/include ; \
	fi
	@if [ ! -d preloader/platform/current/cpu ]; then \
		mkdir -p preloader/platform/current/cpu ; \
	fi
	@if [ ! -d preloader/platform/current/dram ]; then \
		mkdir -p preloader/platform/current/dram ; \
	fi
	@for SRC in $(PLR_PLAT_SOBJS:.o=.S) $(PLR_PLAT_COBJS:.o=.c) $(PLR_PLAT_HDRS) ; \
	do \
		if [ ! -e $(PLR_PLATFORM)/$${SRC} ]; then \
			if [ ! -e $(PLR_LPLATFORM)/$${SRC} ]; then \
				echo "EE: Missing $(PLR_LPLATFORM)/$${SRC}" ; \
				exit 1 ; \
			fi \
		else \
			cmp $(PLR_PLATFORM)/$${SRC} $(PLR_LPLATFORM)/$${SRC} > /dev/null 2>&1 ; \
			if [ "$$?" -ne 0 ]; then \
				echo -n "[MISC] $(PLR_LPLATFORM)/$${SRC}... " ;\
				cp $(PLR_PLATFORM)/$${SRC} $(PLR_LPLATFORM)/$${SRC} ; \
				echo "Updated" ; \
			fi \
		fi \
	done
	@for SRC in $(PLR_COMM_COBJS:.o=.c) $(PLR_COMM_HDRS) ; \
	do \
		if [ ! -e $(PLR_COMMON)/$${SRC} ]; then \
			if [ ! -e $(PLR_LCOMMON)/$${SRC} ]; then \
				echo "EE: Missing $(PLR_LCOMMON)/$${SRC}" ; \
				exit 1 ; \
			fi \
		else \
			cmp $(PLR_COMMON)/$${SRC} $(PLR_LCOMMON)/$${SRC} > /dev/null 2>&1 ; \
			if [ "$$?" -ne 0 ]; then \
				echo -n "[MISC] $(PLR_LCOMMON)/$${SRC}... " ;\
				cp $(PLR_COMMON)/$${SRC} $(PLR_LCOMMON)/$${SRC} ; \
				echo "Updated" ; \
			fi \
		fi \
	done

$(LIB): preloader $(OBJS)
	$(call cmd_link_o_target, $(OBJS))

#########################################################################

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

ifneq ($(filter preloader,$(MAKECMDGOALS)),preloader)
	sinclude $(obj).depend
endif

#########################################################################
