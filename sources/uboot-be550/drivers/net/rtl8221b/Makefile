LOC_DIR=/src/init/rtl8221b
LIB=INIT

include $(PRJ_PATH)/make/config.mk

SRC_LIST= nic_rtl8226b.c nic_rtl8226b_init.c rtl8226b_mdio.c

EXTRA_CFLAGS += -Isrc/init/rtl8221b  -D_LITTLE_ENDIAN -DMDC_MDIO_OPERATION 
EXTRA_CFLAGS += -Iinclude/linux/

include $(PRJ_PATH)/make/components.mk
include $(PRJ_PATH)/make/defs.mk
include $(PRJ_PATH)/make/target.mk

all: dep obj
