#######################################################3#
#	Build Realtek Net Virtual Device modules	#
#########################################################

include $(APOLLODIR)/Compiler_Flag

MODULE_NAME=netvd
PWD = $(shell pwd)

SDKDIR=$(APOLLODIR)/sdk
SDK_INCLUDE   = $(SDKDIR)/include
SYS_INCLUDE   = $(SDKDIR)/system/include
DRV_INCLUDE   = $(SDKDIR)/system/linux

EXTRA_CFLAGS+=$(PON_CFLAGS)
FLAGS+=-I$(SDK_INCLUDE) -I$(SYS_INCLUDE) -I$(DRV_INCLUDE) -I$(KERNEL_DIR)/drivers/net


EXTRA_CFLAGS+=$(FLAGS) -I$(KDIR)/drivers/net -I$(KDIR)/net/bridge/

ifeq ($(CONFIG_RTK_L34_ENABLE),y)
EXTRA_CFLAGS+=-I$(APOLLODIR)/romeDriver
endif

ifeq ($(CONFIG_MODULE_BUILD_IN),y)
BUILD_MODE=y
else
BUILD_MODE=m
endif

obj-$(BUILD_MODE):= $(MODULE_NAME).o

$(MODULE_NAME)-objs:= netvd_proc.o netvd_dev.o netvd_event.o netvd_route.o netvd_thread.o netvd_main.o netvd_ppp.o netvd_default_map.o netvd_netfilter.o

all:
	@echo "   BUILD module $(MODULE_NAME), $(FLAGS)"
	$(MAKE) -C $(KDIR) M=$(PWD) ARCH=rlx CROSS_COMPILE=$(CROSS_COMPILE)
 

install:
	cp -af $(MODULE_NAME).ko $(ROMFSDIR)/lib/modules

clean:
	rm -rf *.o *~ core .depend .*.cmd *.mod.c .tmp_versions  *.symvers *.ko 
