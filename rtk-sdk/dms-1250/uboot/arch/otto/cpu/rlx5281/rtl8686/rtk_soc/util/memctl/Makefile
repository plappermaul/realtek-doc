include $(TOPDIR)/config.mk


subdir	=

.PHONY : $(subdir)

O_TARGTE = $(obj)memctl_util.o

LIB =

#COBJS   = memctl_dram.o  memctl_flash.o
COBJS   = memctl_dram.o
AOBJS   = memctl_dram_init.o

SRCS	:= $(COBJS:.o=.c) $(AOBJS:.o=.S)
OBJS	:= $(addprefix $(obj),$(COBJS)) $(addprefix $(obj),$(AOBJS))

all:	$(subdir) $(O_TARGTE)

$(O_TARGTE):	$(obj).depend $(OBJS)
		$(LD) -r -o $@ $(OBJS) $(LIB)

$(subdir):
		$(MAKE) -C $@ all

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

