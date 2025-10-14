LOC_DIR=src/hsl/phy/rtl8372
LIB=HSL

include $(PRJ_PATH)/make/config.mk

SRC_LIST=

ifeq (TRUE, $(IN_RTL8372_PHY))
SRC_LIST += acl.c cpuTag.c dot1x.c gpio.c identify.c interrupt.c l2.c macsec.c miim.c nic.c port.c qos.c rldp.c rtk_switch.c storm.c trunk.c \
		chip.c dos.c eee.c i2c.c igmp.c isolation.c led.c mib.c mirror.c phy_rtl8224.c ptp.c rate.c rma.c sharemeter.c svlan.c vlan.c
	
EXTRA_CFLAGS += -DMDC_MDIO_OPERATION
endif

include $(PRJ_PATH)/make/components.mk
include $(PRJ_PATH)/make/defs.mk
include $(PRJ_PATH)/make/target.mk

all: dep obj
