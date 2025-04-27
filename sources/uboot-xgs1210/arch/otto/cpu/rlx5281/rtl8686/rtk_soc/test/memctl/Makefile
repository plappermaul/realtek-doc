include $(TOPDIR)/config.mk


subdir	=

.PHONY : $(subdir)

O_TARGTE = $(obj)memctl_test.o

LIB =

COBJS   = memctl_dram.o
#COBJS   = memctl_dram.o  memctl_flash.o
ifndef CONFIG_SYS_NO_FLASH
COBJS +=memctl_flash.o
endif

AOBJS   =

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

