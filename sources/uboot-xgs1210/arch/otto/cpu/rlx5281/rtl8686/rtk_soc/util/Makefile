include $(TOPDIR)/config.mk

subdir += memctl
subdir += sramctl
subdir += sys_pll
ifdef CONFIG_CMD_IDMEM_TEST
subdir += idmem_dma
subdir += rlx5281
endif

.PHONY : $(subdir)

O_TARGTE = $(obj)rtk_soc_util.o

LIB = memctl/memctl_util.o sramctl/sramctl_util.o
LIB += sys_pll/sys_pll_util.o
ifdef CONFIG_CMD_IDMEM_TEST
LIB += idmem_dma/idmem_dma_util.o
LIB += rlx5281/rlx5281_util.o
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

