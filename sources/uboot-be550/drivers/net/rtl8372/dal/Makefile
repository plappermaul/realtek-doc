LOC_DIR=src/hsl/phy/rtl8372/dal
LIB=HSL

include $(PRJ_PATH)/make/config.mk

SRC_LIST=

ifeq (TRUE, $(IN_RTL8372_PHY))
SRC_LIST += dal_mgmt.c 
	
EXTRA_CFLAGS += -DMDC_MDIO_OPERATION
endif

include $(PRJ_PATH)/make/components.mk
include $(PRJ_PATH)/make/defs.mk
include $(PRJ_PATH)/make/target.mk

all: dep obj
