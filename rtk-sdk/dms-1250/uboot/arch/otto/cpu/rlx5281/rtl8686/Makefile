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

subdir = rtk_soc

LIB	= $(obj)lib$(SOC).o
LIBOBJS = rtk_soc/rtk_soc.o

#COBJS	= timer.o rtl8686_serial.o rtl8686_eth.o \
	swCore.o swNic_poll.o swTable.o vlanTable.o \
	rtl8367b/port.o rtl8367b/rtl8367b_asicdrv.o rtl8367b/smi.o
COBJS	= timer.o rtl8686_serial.o rtl8686_eth.o \
	re8670poll.o swCore.o
SOBJS	= lowlevel_init.o cp0_util.o rlx5281_ctr.o

.PHONY : $(subdir)

SRCS	:= $(SOBJS:.o=.S) $(COBJS:.o=.c)
OBJS	:= $(addprefix $(obj),$(LIBOBJS) $(SOBJS) $(COBJS))

all: $(subdir)	$(obj).depend $(LIB)

$(LIB):	$(OBJS)
	$(call cmd_link_o_target, $(OBJS))

$(subdir):
	$(MAKE) -C $@ all


#########################################################################

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################
