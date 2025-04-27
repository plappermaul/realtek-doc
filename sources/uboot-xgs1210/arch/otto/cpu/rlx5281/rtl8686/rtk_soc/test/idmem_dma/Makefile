include $(TOPDIR)/config.mk


subdir	=

.PHONY : $(subdir)

O_TARGTE = $(obj)idmem_dma_test.o

LIB =

COBJS   = idmemTest.o idmem_test.o
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

