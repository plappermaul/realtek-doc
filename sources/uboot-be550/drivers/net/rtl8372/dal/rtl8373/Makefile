LOC_DIR=src/hsl/phy/rtl8372/dal/rtl8373
LIB=HSL

include $(PRJ_PATH)/make/config.mk

SRC_LIST=

ifeq (TRUE, $(IN_RTL8372_PHY))
SRC_LIST += dal_rtl8373_acl.c     dal_rtl8373_fc.c         dal_rtl8373_isolation.c  dal_rtl8373_mirror.c  dal_rtl8373_rate.c        dal_rtl8373_svlan.c   rtl8373_regField_list.c \
	    dal_rtl8373_cpuTag.c  dal_rtl8373_gpio.c       dal_rtl8373_led.c        dal_rtl8373_nic.c     dal_rtl8373_rma.c         dal_rtl8373_switch.c  rtl8373_reg_list.c \
            dal_rtl8373_dos.c     dal_rtl8373_hsb.c        dal_rtl8373_lut.c        dal_rtl8373_parser.c  dal_rtl8373_rtkpp.c       dal_rtl8373_trunk.c   rtl8373_smi.c \
            dal_rtl8373_dot1x.c   dal_rtl8373_i2c.c        dal_rtl8373_macsec.c     dal_rtl8373_port.c    dal_rtl8373_sharemeter.c  dal_rtl8373_vlan.c    rtl8373_tableField_list.c \
            dal_rtl8373_drv.c     dal_rtl8373_igmp.c       dal_rtl8373_mapper.c     dal_rtl8373_ptp.c     dal_rtl8373_storm.c       dal_rtl8373_wol.c     rtl8373_table_list.c \
            dal_rtl8373_eee.c     dal_rtl8373_interrupt.c  dal_rtl8373_mib.c        dal_rtl8373_qos.c     dal_rtl8373_stp.c         rtl8373_asicdrv.c
	
EXTRA_CFLAGS += -DMDC_MDIO_OPERATION
endif

include $(PRJ_PATH)/make/components.mk
include $(PRJ_PATH)/make/defs.mk
include $(PRJ_PATH)/make/target.mk

all: dep obj
