LOC_DIR=src/hsl/phy/rtl8367_common_V1_4_2/dal/rtl8367c
LIB=HSL

include $(PRJ_PATH)/make/config.mk

SRC_LIST += dal_rtl8367c_mapper.c dal_rtl8367c_acl.c dal_rtl8367c_cpu.c dal_rtl8367c_dot1x.c dal_rtl8367c_eee.c dal_rtl8367c_gpio.c \
	dal_rtl8367c_i2c.c dal_rtl8367c_igmp.c dal_rtl8367c_interrupt.c dal_rtl8367c_l2.c dal_rtl8367c_leaky.c dal_rtl8367c_led.c \
	dal_rtl8367c_mirror.c dal_rtl8367c_oam.c dal_rtl8367c_port.c dal_rtl8367c_ptp.c dal_rtl8367c_qos.c dal_rtl8367c_rate.c dal_rtl8367c_rldp.c \
	dal_rtl8367c_stat.c dal_rtl8367c_storm.c dal_rtl8367c_svlan.c dal_rtl8367c_switch.c dal_rtl8367c_trap.c dal_rtl8367c_trunk.c \
	dal_rtl8367c_vlan.c rtl8367c_asicdrv.c rtl8367c_asicdrv_acl.c rtl8367c_asicdrv_cputag.c rtl8367c_asicdrv_dot1x.c rtl8367c_asicdrv_eav.c \
	rtl8367c_asicdrv_eee.c rtl8367c_asicdrv_fc.c rtl8367c_asicdrv_green.c rtl8367c_asicdrv_hsb.c rtl8367c_asicdrv_i2c.c rtl8367c_asicdrv_igmp.c \
	rtl8367c_asicdrv_inbwctrl.c rtl8367c_asicdrv_interrupt.c rtl8367c_asicdrv_led.c  rtl8367c_asicdrv_lut.c rtl8367c_asicdrv_meter.c \
	rtl8367c_asicdrv_mib.c rtl8367c_asicdrv_mirror.c rtl8367c_asicdrv_misc.c rtl8367c_asicdrv_oam.c rtl8367c_asicdrv_phy.c rtl8367c_asicdrv_port.c \
	rtl8367c_asicdrv_portIsolation.c rtl8367c_asicdrv_qos.c rtl8367c_asicdrv_rldp.c rtl8367c_asicdrv_rma.c rtl8367c_asicdrv_scheduling.c \
	rtl8367c_asicdrv_storm.c rtl8367c_asicdrv_svlan.c rtl8367c_asicdrv_trunking.c rtl8367c_asicdrv_unknownMulticast.c rtl8367c_asicdrv_vlan.c \
	rtl8367c_smi.c
	
EXTRA_CFLAGS += -DMDC_MDIO_OPERATION

include $(PRJ_PATH)/make/components.mk
include $(PRJ_PATH)/make/defs.mk
include $(PRJ_PATH)/make/target.mk

all: dep obj
