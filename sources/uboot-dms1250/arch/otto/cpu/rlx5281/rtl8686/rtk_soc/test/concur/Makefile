include $(TOPDIR)/config.mk

# CFLAGS += -DET_DEBUG -DDEBUG

O_TARGTE	= $(obj)CONCUR_TEST.o

COBJS   = concur_gdma.o concur_idmem.o concur_cpu.o concur_test.o

SRCS	:= $(COBJS:.o=.c)
OBJS	:= $(addprefix $(obj),$(COBJS))

all:	$(O_TARGTE)

$(O_TARGTE):	$(obj).depend $(OBJS)
		$(LD) -r -o $@ $(OBJS)


# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

