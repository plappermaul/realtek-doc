#
# Copyright (C) 2011 Realtek Semiconductor Corp.
#
# Makefile for SDK EUROPA parameter Source
#

APOLLO_SDK_PATH = $(SDKDIR)

# -------------------------------------------------------------------
PWD=$(shell pwd)
DRV_8290B_DIR=drv_8290b

EUROPA_8290B_DRV=europa_8290b_drv

# EUROPA_8290B_DRIVER_VERSION 0x00031C /* 0.3.28 */
MODULE_VERSION_8290B = 0x00031C


#VERSION=$(shell printf "%d" $(MODULE_VERSION_8290B))
VERSION_8290B_MAJOR = $(shell echo "" | awk -F: '{ print and(rshift($(MODULE_VERSION_8290B),16),0xff); }')
VERSION_8290B_MINOR = $(shell echo "" | awk -F: '{ print and(rshift($(MODULE_VERSION_8290B),8),0xff); }')
VERSION_8290B_PATCH = $(shell echo "" | awk -F: '{ print and(rshift($(MODULE_VERSION_8290B),0),0xff); }')

RELEASE_VERSION_8290B=_$(VERSION_8290B_MAJOR).$(VERSION_8290B_MINOR).$(VERSION_8290B_PATCH)
# -------------------------------------------------------------------

DRV_8290C_DIR=drv_8290c

EUROPA_8290C_DRV=europa_8290c_drv

# EUROPA_8290C_DRIVER_VERSION 0x010008 /* 1.0.8 */
MODULE_VERSION_8290C = 0x010008

#VERSION=$(shell printf "%d" $(MODULE_VERSION_8290C))
VERSION_8290C_MAJOR = $(shell echo "" | awk -F: '{ print and(rshift($(MODULE_VERSION_8290C),16),0xff); }')
VERSION_8290C_MINOR = $(shell echo "" | awk -F: '{ print and(rshift($(MODULE_VERSION_8290C),8),0xff); }')
VERSION_8290C_PATCH = $(shell echo "" | awk -F: '{ print and(rshift($(MODULE_VERSION_8290C),0),0xff); }')

RELEASE_VERSION_8290C=_$(VERSION_8290C_MAJOR).$(VERSION_8290C_MINOR).$(VERSION_8290C_PATCH)
# -------------------------------------------------------------------
DRV_8291_DIR=drv_8291

EUROPA_8291_DRV=europa_8291_drv

# EUROPA_8291_DRIVER_VERSION 0x010000 /* 1.0.0 */
MODULE_VERSION_8291 = 0x010000

#VERSION=$(shell printf "%d" $(MODULE_VERSION_8291))
VERSION_8291_MAJOR = $(shell echo "" | awk -F: '{ print and(rshift($(MODULE_VERSION_8291),16),0xff); }')
VERSION_8291_MINOR = $(shell echo "" | awk -F: '{ print and(rshift($(MODULE_VERSION_8291),8),0xff); }')
VERSION_8291_PATCH = $(shell echo "" | awk -F: '{ print and(rshift($(MODULE_VERSION_8291),0),0xff); }')

RELEASE_VERSION_8291=_$(VERSION_8291_MAJOR).$(VERSION_8291_MINOR).$(VERSION_8291_PATCH)
# -------------------------------------------------------------------
include $(APOLLODIR)/Compiler_Flag

LIB += ../lib/libpr.a
LIB += -L$(APOLLODIR)/object/src/app/lib -lrtk -lpthread -lrt -lm

#ASIC mode
CFLAGS := -DRTK_X86_CLE -DRTK_X86_ASICDRV -DCONFIG_APOLLO_CMD -DCONFIG_LINUX_USER_SHELL -D'EUROPA_8290B_VERSION="$(RELEASE_VERSION_8290B)"'


SDK_ROOT_DIR = $(APOLLO_SDK_PATH)
SDK_INCLUDE   = $(SDK_ROOT_DIR)/include
SYS_DIR = $(SDK_ROOT_DIR)/system/linux
SYS_INCLUDE = $(SDK_ROOT_DIR)/system/include
RTUSR_INC = $(SYS_DIR)/rtk/rtusr/include
COMMON_DIR = $(SDK_ROOT_DIR)/src/common

EUROPA_DIR := .
EUROPA_INC_DIR  = $(EUROPA_DIR)/inc
EUROPA_SRC_DIR := $(EUROPA_DIR)/src
EUROPA_USER_INC_DIR := $(EUROPA_INC_DIR)/user
EUROPA_USER_SRC_DIR := $(EUROPA_SRC_DIR)/user

ifeq ($(CONFIG_KERNEL_2_6_30),y)
  ifeq ($(CONFIG_KALLSYMS),y)
    EUROPA_8290B_DRV_DIR= drv_8290b_2_6_30
    EUROPA_8290C_DRV_DIR= drv_8290c_2_6_30
    EUROPA_8291_DRV_DIR= drv_8291_2_6_30
  else
    EUROPA_8290B_DRV_DIR= drv_8290b_2_6_30_no_kallsyms
    EUROPA_8290C_DRV_DIR= drv_8290c_2_6_30_no_kallsyms
    EUROPA_8291_DRV_DIR= drv_8291_2_6_30_no_kallsyms
  endif
endif

ifeq ($(CONFIG_KERNEL_3_18_x),y)
	ifeq ($(CONFIG_SMP),y)
		EUROPA_8290B_DRV_DIR= $(shell echo $(CONFIG_RSDK_DIR) | awk -F/ '{print $$NF}' | awk -F- '{print "drv_8290b_3_18-"$$3"-"$$6}')
		EUROPA_8290C_DRV_DIR= $(shell echo $(CONFIG_RSDK_DIR) | awk -F/ '{print $$NF}' | awk -F- '{print "drv_8290c_3_18-"$$3"-"$$6}')
		EUROPA_8291_DRV_DIR= $(shell echo $(CONFIG_RSDK_DIR) | awk -F/ '{print $$NF}' | awk -F- '{print "drv_8291_3_18-"$$3"-"$$6}')
	else
		EUROPA_8290B_DRV_DIR= $(shell echo $(CONFIG_RSDK_DIR) | awk -F/ '{print $$NF}' | awk -F- '{print "drv_8290b_3_18-"$$3"-"$$6"-no-smp"}')
		EUROPA_8290C_DRV_DIR= $(shell echo $(CONFIG_RSDK_DIR) | awk -F/ '{print $$NF}' | awk -F- '{print "drv_8290c_3_18-"$$3"-"$$6"-no-smp"}')
		EUROPA_8291_DRV_DIR= $(shell echo $(CONFIG_RSDK_DIR) | awk -F/ '{print $$NF}' | awk -F- '{print "drv_8291_3_18-"$$3"-"$$6"-no-smp"}')
	endif
endif

ifeq ($(CONFIG_KERNEL_4_4_x),y)
	ifeq ($(CONFIG_COMMON_RT_API),y)
		EUROPA_8290B_DRV_DIR= $(shell echo $(CONFIG_RSDK_DIR) | awk -F/ '{print $$NF}' | awk -F- '{print "drv_8290b_4_4-"$$3"-"$$6"-rt"}')
		EUROPA_8290C_DRV_DIR= $(shell echo $(CONFIG_RSDK_DIR) | awk -F/ '{print $$NF}' | awk -F- '{print "drv_8290c_4_4-"$$3"-"$$6"-rt"}')
		EUROPA_8291_DRV_DIR= $(shell echo $(CONFIG_RSDK_DIR) | awk -F/ '{print $$NF}' | awk -F- '{print "drv_8291_4_4-"$$3"-"$$6"-rt"}')
	else
		EUROPA_8290B_DRV_DIR= $(shell echo $(CONFIG_RSDK_DIR) | awk -F/ '{print $$NF}' | awk -F- '{print "drv_8290b_4_4-"$$3"-"$$6}')
		EUROPA_8290C_DRV_DIR= $(shell echo $(CONFIG_RSDK_DIR) | awk -F/ '{print $$NF}' | awk -F- '{print "drv_8290c_4_4-"$$3"-"$$6}')
		EUROPA_8291_DRV_DIR= $(shell echo $(CONFIG_RSDK_DIR) | awk -F/ '{print $$NF}' | awk -F- '{print "drv_8291_4_4-"$$3"-"$$6}')
	endif
endif

ifeq ($(CONFIG_KERNEL_5_10_x),y)
    ifeq ($(CONFIG_COMMON_RT_API),y)
        EUROPA_8290B_DRV_DIR= $(shell echo $(CONFIG_RSDK_DIR) | awk -F/ '{print $$NF}' | awk -F- '{print "drv_8290b_5_10-"$$3"-"$$6"-rt"}')
        EUROPA_8290C_DRV_DIR= $(shell echo $(CONFIG_RSDK_DIR) | awk -F/ '{print $$NF}' | awk -F- '{print "drv_8290c_5_10-"$$3"-"$$6"-rt"}')
        EUROPA_8291_DRV_DIR= $(shell echo $(CONFIG_RSDK_DIR) | awk -F/ '{print $$NF}' | awk -F- '{print "drv_8291_5_10-"$$3"-"$$6"-rt"}')
    else
        EUROPA_8290B_DRV_DIR= $(shell echo $(CONFIG_RSDK_DIR) | awk -F/ '{print $$NF}' | awk -F- '{print "drv_8290b_5_10-"$$3"-"$$6}')
        EUROPA_8290C_DRV_DIR= $(shell echo $(CONFIG_RSDK_DIR) | awk -F/ '{print $$NF}' | awk -F- '{print "drv_8290c_5_10-"$$3"-"$$6}')
        EUROPA_8291_DRV_DIR= $(shell echo $(CONFIG_RSDK_DIR) | awk -F/ '{print $$NF}' | awk -F- '{print "drv_8291_5_10-"$$3"-"$$6}')
    endif
endif

INCLUDE += -I$(EUROPA_INC_DIR) \
           -I$(EUROPA_USER_INC_DIR) \
           -I$(SDK_INCLUDE) \
           -I$(SYS_INCLUDE) \
           -I$(SDK_INCLUDE)/hal/chipdef/apollo \
           -I$(SDKDIR)/src/app/europa/inc

ifeq ($(CONFIG_OE_MODULE_I2C_PORT_0),y)
        EXTRA_CFLAGS += -DEUROPA_I2C_PORT=0
else
        EXTRA_CFLAGS += -DEUROPA_I2C_PORT=1
endif

#EXEC = europad
EXECLI = europacli
BIN = bin

EUROPACORE_INC := $(shell ls $(EUROPA_INC_DIR)/*.h)

EUROPACLI := \
       $(EUROPA_SRC_DIR)/europa_8290c.o \
       $(EUROPA_SRC_DIR)/europa_8291.o \
       $(EUROPA_SRC_DIR)/europa_cli.o
       
EXTRA_CFLAGS+=$(PON_CFLAGS)
FLAGS+=-I$(SDK_INCLUDE) -I$(SYS_INCLUDE) -I$(DRV_INCLUDE) -I$(PR_INCLUDE) -I$(KERNEL_DIR)/drivers/net

EXTRA_CFLAGS+=$(FLAGS)
       

$(EXECLI): $(EUROPACLI)
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) $(INCLUDE) $(EUROPACLI) $(LIB) -o $(BIN)/$@
	$(STRIP) $(BIN)/$@

%.o: %.c
	$(CC) $(INCLUDE) $(CFLAGS) $(EXTRA_CFLAGS) -c -o $@ $<

# Force to rebuild all every time
all: clean $(EXECLI) Driver
	##$(MAKE) -C $(DRV_DIR) all MODULE_VERSION=$(MODULE_VERSION)
	##cp -af drv/$(EUROPA_DRV).ko drv/$(EUROPA_DRV).ko$(RELEASE_VERSION)

Driver:
ifeq ($(CONFIG_EUROPA_FEATURE),y)	
	if [ ! -d $(EUROPA_8290B_DRV_DIR) ];  then \
		mkdir $(EUROPA_8290B_DRV_DIR) ;\
	fi

	if [ -f $(PWD)/$(DRV_8290B_DIR)/Makefile ];  then \
		@echo "Building europa 8290b driver..."; \
		$(MAKE) -C $(DRV_8290B_DIR) all MODULE_VERSION_8290B=$(MODULE_VERSION_8290B); \
		cp -af $(DRV_8290B_DIR)/$(EUROPA_8290B_DRV).ko $(DRV_8290B_DIR)/$(EUROPA_8290B_DRV).ko$(RELEASE_VERSION_8290B); \
		cp -af $(DRV_8290B_DIR)/$(EUROPA_8290B_DRV).ko$(RELEASE_VERSION_8290B) $(EUROPA_8290B_DRV_DIR)/$(EUROPA_8290B_DRV).ko$(RELEASE_VERSION_8290B); \
		cp -af $(DRV_8290B_DIR)/insert_europa.sh $(EUROPA_8290B_DRV_DIR)/ ; \
	fi
endif

ifeq ($(CONFIG_RTL8290C_FEATURE),y)
	if [ ! -d $(EUROPA_8290C_DRV_DIR) ];  then \
		mkdir $(EUROPA_8290C_DRV_DIR) ;\
	fi

	if [ -f $(PWD)/$(DRV_8290C_DIR)/Makefile ];  then \
		@echo "Building europa 8290c driver..."; \
		$(MAKE) -C $(DRV_8290C_DIR) all MODULE_VERSION_8290C=$(MODULE_VERSION_8290C); \
		cp -af $(DRV_8290C_DIR)/$(EUROPA_8290C_DRV).ko $(DRV_8290C_DIR)/$(EUROPA_8290C_DRV).ko$(RELEASE_VERSION_8290C); \
		cp -af $(DRV_8290C_DIR)/$(EUROPA_8290C_DRV).ko$(RELEASE_VERSION_8290C) $(EUROPA_8290C_DRV_DIR)/$(EUROPA_8290C_DRV).ko$(RELEASE_VERSION_8290C); \
		cp -af $(DRV_8290C_DIR)/insert_europa_8290c.sh $(EUROPA_8290C_DRV_DIR)/ ; \
	fi
endif

ifeq ($(CONFIG_RTL8291_FEATURE),y)
	if [ ! -d $(EUROPA_8291_DRV_DIR) ];  then \
		mkdir $(EUROPA_8291_DRV_DIR) ;\
	fi

	if [ -f $(PWD)/$(DRV_8291_DIR)/Makefile ];  then \
		@echo "Building europa RTL8291 driver..."; \
		$(MAKE) -C $(DRV_8291_DIR) all MODULE_VERSION_8291=$(MODULE_VERSION_8291); \
		cp -af $(DRV_8291_DIR)/$(EUROPA_8291_DRV).ko $(DRV_8291_DIR)/$(EUROPA_8291_DRV).ko$(RELEASE_VERSION_8291); \
		cp -af $(DRV_8291_DIR)/$(EUROPA_8291_DRV).ko$(RELEASE_VERSION_8291) $(EUROPA_8291_DRV_DIR)/$(EUROPA_8291_DRV).ko$(RELEASE_VERSION_8291); \
		cp -af $(DRV_8291_DIR)/insert_europa_8291.sh $(EUROPA_8291_DRV_DIR)/ ; \
	fi
endif

DriverClean:
ifeq ($(CONFIG_EUROPA_FEATURE),y)
	if [ -f $(PWD)/$(DRV_8290B_DIR)/Makefile ];  then \
	   @echo "Cleaning europa 8290b driver..."; \
	   $(MAKE) -C $(DRV_8290B_DIR) clean RELEASE_VERSION_8290B=$(RELEASE_VERSION_8290B); \
	fi 
endif

ifeq ($(CONFIG_RTL8290C_FEATURE),y)
	if [ -f $(PWD)/$(DRV_8290C_DIR)/Makefile ];  then \
	   @echo "Cleaning europa 8290c driver..."; \
	   $(MAKE) -C $(DRV_8290C_DIR) clean RELEASE_VERSION_8290C=$(RELEASE_VERSION_8290C); \
	fi 
endif
	
ifeq ($(CONFIG_RTL8291_FEATURE),y)
	if [ -f $(PWD)/$(DRV_8291_DIR)/Makefile ];  then \
	   @echo "Cleaning europa RTL8291 driver..."; \
	   $(MAKE) -C $(DRV_8291_DIR) clean RELEASE_VERSION_RTL8291=$(RELEASE_VERSION_RTL8291); \
	fi 
endif
	
install: $(EXECLI)
	cp $(BIN)/$(EXECLI) $(BINDIR)
ifeq ($(CONFIG_EUROPA_FEATURE),y)
	cp -af $(DRV_8290B_DIR)/$(EUROPA_8290B_DRV).ko$(RELEASE_VERSION_8290B) $(ROMFSDIR)/lib/modules/$(EUROPA_8290B_DRV).ko
	cp -af $(DRV_8290B_DIR)/insert_europa.sh $(ROMFSDIR)/etc/scripts/
	chmod 755 $(ROMFSDIR)/etc/scripts/insert_europa.sh
endif

ifeq ($(CONFIG_RTL8290C_FEATURE),y)
	cp -af $(DRV_8290C_DIR)/$(EUROPA_8290C_DRV).ko$(RELEASE_VERSION_8290C) $(ROMFSDIR)/lib/modules/$(EUROPA_8290C_DRV).ko
	cp -af $(DRV_8290C_DIR)/insert_europa_8290c.sh $(ROMFSDIR)/etc/scripts/
	chmod 755 $(ROMFSDIR)/etc/scripts/insert_europa_8290c.sh
endif

ifeq ($(CONFIG_RTL8291_FEATURE),y)
	cp -af $(DRV_8291_DIR)/$(EUROPA_8291_DRV).ko$(RELEASE_VERSION_RTL8291) $(ROMFSDIR)/lib/modules/$(EUROPA_8291_DRV).ko
	cp -af $(DRV_8291_DIR)/insert_europa_8291.sh $(ROMFSDIR)/etc/scripts/
	chmod 755 $(ROMFSDIR)/etc/scripts/insert_europa_8291.sh
endif
	# board parameters
	@echo "Building europa script..."; 

ifeq ("$(CONFIG_TX_POWER_TURN_ON_FEATURE)","")
	CONFIG_TX_POWER_GPO_PIN=255
endif

ifeq ($(CONFIG_EUROPA_FEATURE),y)
ifeq ($(CONFIG_OE_MODULE_I2C_PORT_0),y)
	$(ROMFSINST) -a "I2C_PORT=0 INTR_PIN=$(CONFIG_EUROPA_INTERRUPT_GPIO_PIN) TXDIS_PIN=$(CONFIG_TX_DISABLE_GPIO_PIN) TXPWR_PIN=$(CONFIG_TX_POWER_GPO_PIN)" /etc/scripts/insert_europa.sh
else
	$(ROMFSINST) -a "I2C_PORT=1 INTR_PIN=$(CONFIG_EUROPA_INTERRUPT_GPIO_PIN) TXDIS_PIN=$(CONFIG_TX_DISABLE_GPIO_PIN) TXPWR_PIN=$(CONFIG_TX_POWER_GPO_PIN)" /etc/scripts/insert_europa.sh
endif
endif

ifeq ($(CONFIG_RTL8290C_FEATURE),y)
ifeq ($(CONFIG_OE_MODULE_I2C_PORT_0),y)
	$(ROMFSINST) -a "I2C_PORT=0 " /etc/scripts/insert_europa_8290c.sh
else
	$(ROMFSINST) -a "I2C_PORT=1 " /etc/scripts/insert_europa_8290c.sh
endif
endif

ifeq ($(CONFIG_RTL8291_FEATURE),y)
ifeq ($(CONFIG_OE_MODULE_I2C_PORT_0),y)
	$(ROMFSINST) -a "I2C_PORT=0 " /etc/scripts/insert_europa_8291.sh
else
	$(ROMFSINST) -a "I2C_PORT=1 " /etc/scripts/insert_europa_8291.sh
endif
endif

clean: DriverClean
	rm -f $(EXECLI) *.gdb *.elf *.elf2flt
