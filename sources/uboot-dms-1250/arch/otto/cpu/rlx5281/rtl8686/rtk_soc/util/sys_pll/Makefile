include $(TOPDIR)/config.mk

subdir	=

.PHONY : $(subdir)

O_TARGTE = $(obj)sys_pll_util.o

LIB =

COBJS   = sys_freq_ctl.o
AOBJS   = sys_pll_init.o

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

