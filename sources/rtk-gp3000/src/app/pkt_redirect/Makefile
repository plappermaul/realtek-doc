#
# Copyright (C) 2011 Realtek Semiconductor Corp.
#
# Makefile for SDK packet redirect library
#

APOLLO_SDK_PATH = $(SDKDIR)

include $(APOLLODIR)/Compiler_Flag



#ASIC mode
CFLAGS := -DRTK_X86_CLE -DRTK_X86_ASICDRV -DCONFIG_APOLLO_CMD -DCONFIG_SMIDA1011 -DDISABLE_VLAN_SHADOW -DCONFIG_LINUX_USER_SHELL

include $(CHECKSEC_LIB_CONFIG)

#SDK_ROOT_DIR = $(APOLLO_SDK_PATH)
#SDK_INCLUDE   = $(SDK_ROOT_DIR)/include
#SYS_DIR = $(SDK_ROOT_DIR)/system/linux
#SYS_INCLUDE = $(SDK_ROOT_DIR)/system/include
#RTUSR_INC = $(SYS_DIR)/rtk/rtusr/include
#COMMON_DIR = $(SDK_ROOT_DIR)/src/common

PKT_REDIRECT_DIR := .
PKT_REDIRECT_INC_DIR  = $(PKT_REDIRECT_DIR)/inc
PKT_REDIRECT_SRC_DIR := $(PKT_REDIRECT_DIR)/src

INCLUDE += -I$(PKT_REDIRECT_INC_DIR) \
           -I.. \

LIBDY = libpr.so
LIBFILE = libpr.a
LIBDIR := ../lib

LIBSRC := \
	$(PKT_REDIRECT_SRC_DIR)/pkt_redirect_user.c

LIBOBJ := \
       $(PKT_REDIRECT_SRC_DIR)/pkt_redirect_user.o

LIB_INC := $(shell ls $(PKT_REDIRECT_INC_DIR)/*.h)

$(LIBFILE): $(LIBOBJ) $(LIB_INC)
	$(AR) rcs $(LIBDIR)/$@ $(LIBOBJ)

$(LIBDY):
	$(CC) $(INCLUDE) $(CFLAGS) $(EXTRA_CFLAGS) $(LDFLAGS) -fPIC -shared -o $(LIBDIR)/$@ $(LIBSRC)

%.o: %.c
	$(CC) $(INCLUDE) $(CFLAGS) $(EXTRA_CFLAGS) -c -o $@ $<


all: $(LIBFILE) $(LIBDY)
	cp -af $(LIBDIR)/$(LIBDY) $(APOLLODIR)/object/src/app/lib/


install:
	cp -af $(LIBDIR)/$(LIBDY) $(ROMFSDIR)/lib/
clean:
	rm -f $(LIBOBJ) $(LIBDIR)/$(LIBFILE) *.gdb *.elf *.elf2flt
