LOC_DIR=src/hsl/phy/rtl8367_common_V1_4_2/dal/rtl8367d
LIB=HSL

include $(PRJ_PATH)/make/config.mk

SRC_LIST += dal_rtl8367d_mapper.c dal_rtl8367d_acl.c dal_rtl8367d_cpu.c dal_rtl8367d_dot1x.c dal_rtl8367d_eee.c dal_rtl8367d_gpio.c \
	dal_rtl8367d_igmp.c dal_rtl8367d_interrupt.c dal_rtl8367d_l2.c dal_rtl8367d_leaky.c dal_rtl8367d_led.c \
	dal_rtl8367d_mirror.c dal_rtl8367d_port.c dal_rtl8367d_qos.c dal_rtl8367d_rate.c dal_rtl8367d_rldp.c \
	dal_rtl8367d_stat.c dal_rtl8367d_storm.c dal_rtl8367d_svlan.c dal_rtl8367d_switch.c dal_rtl8367d_trap.c dal_rtl8367d_trunk.c \
	dal_rtl8367d_vlan.c rtl8367d_asicdrv.c rtl8367d_smi.c
	
EXTRA_CFLAGS += -DMDC_MDIO_OPERATION

include $(PRJ_PATH)/make/components.mk
include $(PRJ_PATH)/make/defs.mk
include $(PRJ_PATH)/make/target.mk

all: dep obj
