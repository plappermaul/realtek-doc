#########################################################
#	Build Realtek rldp detect modules	       #
########################################################

include $(APOLLODIR)/Compiler_Flag

MODULE_NAME=rldp_drv
KDIR =
PWD = $(shell pwd)

SDKDIR=$(APOLLODIR)/sdk
SDK_INCLUDE   = $(SDKDIR)/include
SYS_INCLUDE   = $(SDKDIR)/system/include
DRV_INCLUDE   = $(SDKDIR)/system/linux
INTBCST_INCLUDE = $(SDKDIR)/src/module/intr_bcaster

EXTRA_CFLAGS+=$(PON_CFLAGS)

#FLAGS+=-I$(SDK_INCLUDE) -I$(SYS_INCLUDE) -I$(DRV_INCLUDE) -I$(NETDIR) -I(KERNEL_DIR)/drivers/net
FLAGS+=-I$(SDK_INCLUDE) -I$(SYS_INCLUDE) -I$(DRV_INCLUDE) -I$(NETDIR) 


EXTRA_CFLAGS+=$(FLAGS)



#ifeq ($(CONFIG_MODULE_BUILD_IN),y)
#BUILD_MODE=y
#else
#BUILD_MODE=m
#endif

#obj-$(BUILD_MODE) := $(MODULE_NAME).o
obj-m:=$(MODULE_NAME).o

$(MODULE_NAME)-objs:=proto_rldp.o

all:
	@echo "   BUILD module $(MODULE_NAME), $(FLAGS)"
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) ARCH=rlx CROSS_COMPILE=$(CROSS_COMPILE) modules
 

install:
	cp -af $(MODULE_NAME).ko $(ROMFSDIR)/lib/modules

clean:
	rm -rf *.o *~ core .depend .*.cmd *.mod.c .tmp_versions *.symvers *.ko 
