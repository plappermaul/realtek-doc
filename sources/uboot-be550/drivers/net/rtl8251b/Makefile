LOC_DIR=/src/init/rtl8251b
LIB=HSL

include $(PRJ_PATH)/make/config.mk

SRC_LIST=

ifeq (TRUE, $(IN_RTL8251B_PHY))
	SRC_LIST += nic_rtl8251b.c nic_rtl8251b_init.c rtl8251b.c 
	EXTRA_CFLAGS += -DMDC_MDIO_OPERATION 
endif

include $(PRJ_PATH)/make/components.mk
include $(PRJ_PATH)/make/defs.mk
include $(PRJ_PATH)/make/target.mk

all: dep obj
