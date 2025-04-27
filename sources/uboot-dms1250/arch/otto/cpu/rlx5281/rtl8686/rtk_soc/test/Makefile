include $(TOPDIR)/config.mk


subdir += memctl

ifdef CONFIG_CMD_IDMEM_TEST
subdir += idmem_dma
endif
ifdef CONFIG_CMD_GDMA_TEST
subdir += gdma
endif
ifdef CONFIG_CMD_CONCURENT_TEST
subdir += concur
endif
ifdef CONFIG_CMD_SRAM_TEST
subdir += sramctl
endif
ifdef CONFIG_CMD_DRAM_SHARE_TEST
subdir += dram_share
endif

.PHONY : $(subdir)

O_TARGTE = $(obj)rtk_soc_test.o

LIB = memctl/memctl_test.o
ifdef CONFIG_CMD_IDMEM_TEST
LIB += idmem_dma/idmem_dma_test.o
endif
ifdef CONFIG_CMD_GDMA_TEST
LIB += gdma/GDMA_TEST.o
endif
ifdef CONFIG_CMD_CONCURENT_TEST
LIB += concur/CONCUR_TEST.o
endif
ifdef CONFIG_CMD_SRAM_TEST
LIB += sramctl/sramctl_test.o
endif
ifdef CONFIG_CMD_DRAM_SHARE_TEST
LIB += dram_share/dram_share_test.o
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

