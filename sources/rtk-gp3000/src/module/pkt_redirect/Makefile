#########################################################
#	Build Realtek Packet Redirect modules									#
#########################################################

include $(APOLLODIR)/Compiler_Flag

MODULE_NAME=pr
PWD = $(shell pwd)

SDKDIR=$(APOLLODIR)/sdk
SDK_INCLUDE   = $(SDKDIR)/include
SYS_INCLUDE   = $(SDKDIR)/system/include
DRV_INCLUDE   = $(SDKDIR)/system/linux

EXTRA_CFLAGS+=$(PON_CFLAGS)

ifeq ($(CONFIG_RTK_IGMP),y)
EXTRA_CFLAGS+=-DCONFIG_RTK_IGMP
endif

ifeq ($(CONFIG_APOLLO_ROMEDRIVER),y)
EXTRA_CFLAGS+=-DCONFIG_APOLLO_ROMEDRIVER
endif

FLAGS+=-I$(SDK_INCLUDE) -I$(SYS_INCLUDE) -I$(DRV_INCLUDE) -I$(NETDIR)


EXTRA_CFLAGS+=$(FLAGS)



ifeq ($(CONFIG_MODULE_BUILD_IN),y)
BUILD_MODE=y
else
BUILD_MODE=m
endif

obj-$(BUILD_MODE) := $(MODULE_NAME).o

$(MODULE_NAME)-objs:= pkt_redirect.o

all:
	@echo "   BUILD module $(MODULE_NAME), $(FLAGS)"
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) ARCH=rlx CROSS_COMPILE=$(CROSS_COMPILE)
 

install:
	cp -af $(MODULE_NAME).ko $(ROMFSDIR)/lib/modules

clean:
	rm -rf *.o *~ core .depend .*.cmd *.mod.c .tmp_versions *.symvers *.ko 
