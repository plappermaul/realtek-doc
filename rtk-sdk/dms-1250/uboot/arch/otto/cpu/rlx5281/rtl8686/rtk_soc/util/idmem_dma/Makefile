include $(TOPDIR)/config.mk

# CFLAGS += -DET_DEBUG -DDEBUG

O_TARGTE = $(obj)idmem_dma_util.o

COBJS   = idmem_dma_op.o

SRCS	:= $(COBJS:.o=.c)
OBJS	:= $(addprefix $(obj),$(COBJS))

all:	$(O_TARGTE)

$(O_TARGTE):	$(obj).depend $(OBJS)
		$(LD) -r -o $@ $(OBJS)

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

