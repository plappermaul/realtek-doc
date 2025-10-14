LOC_DIR=src/hsl/phy/rtl8367_common_V1_4_2
LIB=HSL

include $(PRJ_PATH)/make/config.mk

SRC_LIST += acl.c cpu.c dot1x.c eee.c gpio.c i2c.c igmp.c interrupt.c l2.c leaky.c led.c mirror.c oam.c port.c ptp.c \
			qos.c rate.c rldp.c rtk_switch.c stat.c storm.c svlan.c trap.c trunk.c vlan.c chip.c rtl8367s.c
	
EXTRA_CFLAGS += -DMDC_MDIO_OPERATION

include $(PRJ_PATH)/make/components.mk
include $(PRJ_PATH)/make/defs.mk
include $(PRJ_PATH)/make/target.mk

all: dep obj
