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
COBJS-y += otto_uart.o otto_time.o

ifdef CONFIG_CMD_SF
COBJS-y += otto_nor_spi_flash.o
endif

ifeq ($(CONFIG_STANDALONE_UBOOT),y)
COBJS-y += plr_flash.o
endif

SRCS	:= $(SOBJS-y:.o=.S) $(COBJS-y:.o=.c)
OBJS	:= $(addprefix $(obj),$(SOBJS-y) $(COBJS-y))

$(LIB):	$(obj).depend $(OBJS)
	$(call cmd_link_o_target, $(OBJS))

#########################################################################

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

ifneq ($(filter plr_flash.c,$(MAKECMDGOALS)),plr_flash.c)
	sinclude $(obj).depend
endif

#########################################################################

ifeq ($(CONFIG_STANDALONE_UBOOT),y)
PLR_DIR       := $(OTTO_RELEASE_DIR)/../preloader
PLR_PLATFORM  := $(PLR_DIR)/platform/current

PLR_INCDIR := -I$(TOPDIR)/$(CPUDIR)/$(SOC)/preloader/include -I$(TOPDIR)/$(CPUDIR)/$(SOC)/preloader/platform/current/cpu

CFLAGS_arch/otto/lib/plr_flash.o := -mlong-calls
CPPFLAGS_arch/otto/lib/plr_flash.o := $(PLR_INCDIR)
CPPFLAGS += $(CPPFLAGS_arch/otto/lib/plr_flash.o)

plr_flash.c:
	@if [ ! -e $(PLR_PLATFORM)/$@ ]; then \
		if [ ! -e $@ ]; then \
			echo "EE: Missing $@" ; \
			exit 1; \
		fi \
	else \
		cmp $(PLR_PLATFORM)/$@ $@ > /dev/null 2>&1 ; \
		if [ "$$?" -ne 0 ]; then \
			echo -n "[MISC] $@... " ; \
			cp $(PLR_PLATFORM)/$@ $@ ; \
			echo "Updated" ; \
		fi \
	fi
endif
