#######################################################3#
#	Build Realtek IGMP modules			#
#########################################################

include $(APOLLODIR)/Compiler_Flag

MODULE_NAME = igmp_drv
KDIR =
PWD = $(shell pwd)

SDKDIR=$(APOLLODIR)/sdk
SDK_INCLUDE   = $(SDKDIR)/include
SYS_INCLUDE   = $(SDKDIR)/system/include
DRV_INCLUDE   = $(SDKDIR)/system/linux
PR_INCLUDE    = $(SDKDIR)/src/module/pkt_redirect

FLAGS+=-I$(SDK_INCLUDE) -I$(SYS_INCLUDE) -I$(DRV_INCLUDE) -I$(PR_INCLUDE) -I$(KERNEL_DIR)/drivers/net

EXTRA_CFLAGS+= -I$(SDK_INCLUDE) \
		-I$(DRV_INCLUDE) \
		-I$(SYS_INCLUDE) \
		-I$(PR_INCLUDE)

ifdef CONFIG_DEFAULTS_KERNEL_2_6
EXTRA_CFLAGS += -I$(KERNEL_DIR)/drivers/net/
else
EXTRA_CFLAGS += -I$(KERNEL_DIR)/drivers/net/ethernet/realtek/
endif

obj-m := $(MODULE_NAME).o

$(MODULE_NAME)-objs:= igmp_drv_main.o

all:
	@echo "   BUILD module $(MODULE_NAME)"
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) ARCH=rlx CROSS_COMPILE=$(CROSS_COMPILE)


install:
	@if [ ! -d $(ROMFSDIR)/lib/modules ]; \
	then \
	mkdir $(ROMFSDIR)/lib/modules ; \
	fi

	cp -af $(MODULE_NAME).ko $(ROMFSDIR)/lib/modules/

clean:
	rm -rf *.o *~ core .depend .*.cmd *.mod.c .tmp_versions *.symvers *.ko
