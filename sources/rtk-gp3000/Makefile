#
# Makefile for the Linux Wireless network device drivers.
#

include $(APOLLODIR)/Compiler_Flag

SDK_RTCOREDIR = $(SDKDIR)/system/linux/rtcore
SDK_RTKDIR    = $(SDKDIR)/src
SDK_UTDIR     = $(SDKDIR)/unittest
SDK_RTDRVDIR  = $(SDKDIR)/system/linux/rtdrv
SDK_INCLUDE   = $(SDKDIR)/include
SYS_INCLUDE   = $(SDKDIR)/system/include
DRV_INCLUDE   = $(SDKDIR)/system/linux
UT_INCLUDE    = $(SDKDIR)/unittest
SDK_MODULE    = $(SDKDIR)/src/module
SDK_ETCDIR    = $(SDKDIR)/rtk.etc

ifeq ($(CONFIG_DIAGSHELL_MODE),CONFIG_DIAGSHELL_RTK)
DIAGSHELL_DIR := $(SDKDIR)/src/app/diag_rtk
endif

ifeq ($(CONFIG_DIAGSHELL_MODE),CONFIG_DIAGSHELL_RAW)
DIAGSHELL_DIR := $(SDKDIR)/src/app/diag
endif


CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE=y


export SDKDIR SDK_INCLUDE SYS_INCLUDE DRV_INCLUDE UT_INCLUDE
export CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE

ifeq ($(filter y, $(CONFIG_LUNA_G3_SERIES)),y)
obj-y += src/
obj-y += system/linux/rtcore/
obj-y += system/linux/rtdrv/
ifeq ($(CONFIG_SDK_MODULES),y)
obj-y += src/module/
endif
else
ifneq ($(CONFIG_XDSL_RG_DIAGSHELL),y)
obj-y += src/
endif
ifeq ($(CONFIG_SDK_DRIVER_UNITTEST),y)
obj-y += unittest/
endif
obj-y += system/linux/rtcore/
obj-y += system/linux/rtdrv/
ifeq ($(CONFIG_SDK_MODULES),y)
obj-y += src/module/
endif
endif

all:
	$(MAKE) -C $(SDK_MODULE)    SRC_DIR=$(SDK_MODULE)       all;

install:
	$(MAKE) -C $(SDK_MODULE)    SRC_DIR=$(SDK_MODULE)       install;


ifeq ($(CONFIG_RTK_L34_ENABLE),y)
DEF += -D CONFIG_RTK_L34_ENABLE
ifeq ($(CONFIG_RTL9600_SERIES),y)
DEF += -D CONFIG_RTL9600_SERIES
endif
ifeq ($(CONFIG_RTL9607C_SERIES),y)
DEF += -D CONFIG_RTL9607C_SERIES
endif
ifeq ($(CONFIG_LUNA_G3_SERIES),y)
DEF += -D CONFIG_LUNA_G3_SERIES
endif
ifeq ($(CONFIG_RTL9603CVD_SERIES),y)
DEF += -D CONFIG_RTL9603CVD_SERIES
endif
endif

ifeq ($(CONFIG_RTK_L34_FLEETCONNTRACK_ENABLE),y)
DEF += -D CONFIG_RTK_L34_FLEETCONNTRACK_ENABLE
endif

ifeq ($(CONFIG_RTK_IGMP_MLD_SNOOPING_MODULE),m)
DEF += -D CONFIG_RTK_IGMP_MLD_SNOOPING_MODULE	
endif

ifeq ($(CONFIG_XDSL_RG_DIAGSHELL),y)
DEF += -D CONFIG_XDSL_RG_DIAGSHELL
endif

ifeq ($(CONFIG_OPENWRT_RG),y)
DEF += -D CONFIG_OPENWRT_RG
endif

ifeq ($(CONFIG_OPENWRT_DIAG),y)
DEF += -D CONFIG_OPENWRT_DIAG
endif

ifeq ($(CONFIG_SDK_RTL9601B),y)
DEF += -D CONFIG_SDK_RTL9601B
endif

ifeq ($(CONFIG_SDK_DRIVER_UNITTEST),y)
DEF += -D CONFIG_SDK_DRIVER_UNITTEST
endif

ifeq ($(CONFIG_GPON_FEATURE),y)
DEF += -D CONFIG_GPON_FEATURE
endif

ifeq ($(CONFIG_10G_GPON_FEATURE),y)
DEF += -D CONFIG_10G_GPON_FEATURE
endif

ifeq ($(CONFIG_EPON_FEATURE),y)
DEF += -D CONFIG_EPON_FEATURE
endif

ifeq ($(CONFIG_RTK_OAM_BY_BUFFER),y)
DEF += -D CONFIG_RTK_OAM_BY_BUFFER
endif

ifeq ($(CONFIG_10G_EPON_FEATURE),y)
DEF += -D CONFIG_10G_EPON_FEATURE
endif

ifeq ($(CONFIG_RLDP_FEATURE),y)
DEF += -D CONFIG_RLDP_FEATURE
endif

ifeq ($(CONFIG_REDUCED_DIAG),y)
DEF += -D CONFIG_REDUCED_DIAG
endif

ifeq ($(CONFIG_L34_TABLE_BASE_DIAG),y)
DEF += -D CONFIG_L34_TABLE_BASE_DIAG
endif

ifeq ($(CONFIG_EUROPA_FEATURE),y)
DEF += -D CONFIG_EUROPA_FEATURE
endif

ifeq ($(CONFIG_EXTERNAL_SWITCH),y)
DEF += -D CONFIG_EXTERNAL_SWITCH
endif

ifeq ($(CONFIG_COMMON_RT_API),y)
DEF += -D CONFIG_COMMON_RT_API
endif

ifeq ($(CONFIG_COMMON_RT_PONMISC),y)
DEF += -D CONFIG_COMMON_RT_PONMISC
endif

clean:
	$(MAKE) -C $(SDK_RTKDIR) clean
	$(MAKE) -C $(SDK_RTCOREDIR) clean
	$(MAKE) -C $(SDK_UTDIR) clean
	rm -rf *.o


shell_tree:
	rm -f $(DIAGSHELL_DIR)/../diag_rtk/src/parser/*.o
ifeq ($(CONFIG_RT_DIAGSHELL_ONLY),y)
	cd $(DIAGSHELL_DIR)/config; \
        ./script/mk_parser.py $(DEF) rt/diag_shell.cli -c cparser_rt_tree.c -i cparser_rt_tree.h -fd ../include/flag/ -f cmd_flag.h; \
        mv ../config/cparser_rt_tree.c ../src/parser/cparser_rt_tree.c; \
        mv ../config/cparser_rt_tree.h ../include/parser/cparser_rt_tree.h; \
        cd - ;
else
	cd $(DIAGSHELL_DIR)/config; \
        ./script/mk_parser.py $(DEF) diag_shell.cli -fd ../include/flag/ -f cmd_flag.h; \
        mv ../config/cparser_tree.c ../src/parser/cparser_tree.c; \
        mv ../config/cparser_tree.h ../include/parser/cparser_tree.h; \
        cd - ;
endif
