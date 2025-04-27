include $(TOPDIR)/config.mk


subdir	=

.PHONY : $(subdir)

O_TARGTE = $(obj)GDMA_TEST.o

LIB =

COBJS   = gdmaTest.o kmp.o gdma_glue.o  qp.o vsm.o base64.o virtualMac.o modelTrace.o icExport.o
COBJS   += gdmaModel.o gdma_utils.o rtl865xC_layer2asic.o icModel.o gdma_test.o
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

