include $(TOPDIR)/config.mk

# CFLAGS += -DET_DEBUG -DDEBUG

O_TARGTE = $(obj)rlx5281_util.o

COBJS   = rlx5281_idmem.o rlx5281_cache.o rlx5281_perCounter.o

SRCS	:= $(COBJS:.o=.S)
OBJS	:= $(addprefix $(obj),$(COBJS))

all:	$(O_TARGTE)

$(O_TARGTE):	$(obj).depend $(OBJS)
		$(LD) -r -o $@ $(OBJS)

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

