LOC_DIR=src/hsl/phy/rtl8367_common_V1_4_2/dal
LIB=HSL

include $(PRJ_PATH)/make/config.mk

SRC_LIST += dal_mgmt.c 
	
EXTRA_CFLAGS += -DMDC_MDIO_OPERATION

include $(PRJ_PATH)/make/components.mk
include $(PRJ_PATH)/make/defs.mk
include $(PRJ_PATH)/make/target.mk

all: dep obj
