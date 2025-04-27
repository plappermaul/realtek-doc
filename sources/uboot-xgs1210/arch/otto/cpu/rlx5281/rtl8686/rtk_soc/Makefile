include $(TOPDIR)/config.mk

subdir += util
subdir += test
ifdef CONFIG_RTK_SOC_PERFORAMCE
subdir += performance
endif
.PHONY : $(subdir)

O_TARGTE = $(obj)rtk_soc.o

LIB = util/rtk_soc_util.o test/rtk_soc_test.o
ifdef CONFIG_RTK_SOC_PERFORAMCE
LIB += performance/rtk_soc_performance.o
endif

COBJS   = 
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

