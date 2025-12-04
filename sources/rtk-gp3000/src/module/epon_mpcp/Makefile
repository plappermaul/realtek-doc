########################################################
#	Build Realtek EPON mpcp modules			           #
########################################################

include $(APOLLODIR)/Compiler_Flag

MODULE_NAME = epon_mpcp
KDIR =
PWD = $(shell pwd)

SDKDIR=$(APOLLODIR)/sdk
SDK_INCLUDE   = $(SDKDIR)/include
SYS_INCLUDE   = $(SDKDIR)/system/include
DRV_INCLUDE   = $(SDKDIR)/system/linux
PR_INCLUDE    = $(SDKDIR)/src/module/pkt_redirect

EXTRA_CFLAGS += $(PON_CFLAGS)
FLAGS+=-I$(SDK_INCLUDE) -I$(SYS_INCLUDE) -I$(DRV_INCLUDE) -I$(PR_INCLUDE) 
ifdef CONFIG_DEFAULTS_KERNEL_3_18
FLAGS+=-I$(KERNEL_DIR)/drivers/net/ethernet/realtek
else
ifdef CONFIG_DEFAULTS_KERNEL_4_4
FLAGS+=-I$(KERNEL_DIR)/drivers/net/ethernet/realtek/
else
FLAGS+=-I$(KERNEL_DIR)/drivers/net
endif
endif

EXTRA_CFLAGS+=$(FLAGS)

obj-m := $(MODULE_NAME).o

$(MODULE_NAME)-objs:= epon_mpcp_check.o

all:
	@echo "   BUILD module $(MODULE_NAME), $(FLAGS)"
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) ARCH=rlx CROSS_COMPILE=$(CROSS_COMPILE)


install:
	cp -af $(MODULE_NAME).ko $(ROMFSDIR)/lib/modules

clean:
	rm -rf *.o *~ core .depend .*.cmd *.mod.c .tmp_versions *.symvers *.ko
