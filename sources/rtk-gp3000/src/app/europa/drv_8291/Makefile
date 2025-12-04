#########################################################
#	Build Realtek EUROPA modules		                #
#########################################################
# add this line if this module used others compiler flag
include $(APOLLODIR)/Compiler_Flag

MODULE_NAME = europa_8291_drv

PWD = $(shell pwd)
#KDIR = /lib/modules/`uname -r`/build

# for sdk/src/module is built kernel module not build-in
#EXTRA_CFLAGS += $(FLAGS)

# add this line if this module used below compiler flag 
EXTRA_CFLAGS+= -DCONFIG_SDK_KERNEL_LINUX $(PON_CFLAGS)
EXTRA_CFLAGS+=-I$(SDKDIR)/include -I$(SDKDIR)/system/include -I$(SDKDIR)/system/linux \
		-I$(APOLLODIR)/ca-ne \
		-I$(APOLLODIR)/ca-plat/log \
		-I$(APOLLODIR)/ca-ne/ploam/cmn\
		-I$(ROOTDIR)/ca_packages/ca-scfg/1.0-r0/ca-scfg-1.0
# define driver version
EXTRA_CFLAGS+=-DEUROPA_8291_DRIVER_VERSION=$(MODULE_VERSION_8291)

obj-$(CONFIG_EUROPA) := $(MODULE_NAME).o

#$(MODULE_NAME)-objs := europa_main.o rtl8291_reg_acc.o rtl8291_api.o rtl8291_init.o ddmi.o
$(MODULE_NAME)-objs := europa_main.o rtl8291_reg_acc.o rtl8291_api.o rtl8291_init.o 

all: clean
	@echo "   BUILD module $(MODULE_NAME)"
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) CROSS_COMPILE=$(CROSS_COMPILE)	
	#$(MAKE) -C $(KERNEL_DIR) M=$(PWD) ARCH=rlx CROSS_COMPILE=$(CROSS_COMPILE)
 
install:
	@if [ ! -d $(ROMFSDIR)/lib/modules ]; \
        then \
                mkdir $(ROMFSDIR)/lib/modules ; \
    	fi
	cp -af $(MODULE_NAME).ko $(ROMFSDIR)/lib/modules
	cp -af $(MODULE_NAME).ko $(MODULE_NAME).ko$(RELEASE_VERSION_RTL8291)

clean:
	rm -rf *.o *~ core .depend .*.cmd *.mod.c .tmp_versions *.symvers *.ko modules.order
	rm -f $(MODULE_NAME).ko$(RELEASE_VERSION_RTL8291)
