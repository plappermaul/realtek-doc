# Realtek Semiconductor Corp.
#
# bsp/Makefile
#     bsp Makefile
#
# Copyright (C) 2006-2015 Tony Wu (tonywu@realtek.com)
#

core-y   += arch/mips/bsp/
cflags-y += -I$(srctree)/arch/mips/bsp/
load-y   += 0x80000000

CHIPDRV_DIR        := chipDrv
RTL931XDRV_DIR	   = $(CHIPDRV_DIR)/rtl931x
RTL930XDRV_DIR	   = $(CHIPDRV_DIR)/rtl930x
RTL839XDRV_DIR	   = $(CHIPDRV_DIR)/rtl839x
RTL838XDRV_DIR	   = $(CHIPDRV_DIR)/rtl838x


obj-y := setup.o irq.o prom.o timer.o serial.o rtk_util.o chip_probe.o chip_probe_export.o
obj-$(CONFIG_OF)  += of.o
obj-$(CONFIG_SMP) += smp.o
obj-$(CONFIG_EARLY_PRINTK) += early-printk.o
obj-y += rtk_bsp_driver.o
obj-y += init_l2cache.o
obj-y += cevt_external_timer.o
obj-y += $(RTL931XDRV_DIR)/rtk_bsp_rtl9310.o
obj-y += $(RTL930XDRV_DIR)/rtk_bsp_rtl9300.o $(RTL930XDRV_DIR)/rtk_bsp_rtl9300_intrCtrl.o
obj-y += $(RTL839XDRV_DIR)/rtk_bsp_rtl8390.o $(RTL839XDRV_DIR)/rtk_bsp_rtl8390_intrCtrl.o
obj-y += $(RTL838XDRV_DIR)/rtk_bsp_rtl8380.o $(RTL838XDRV_DIR)/rtk_bsp_rtl8380_intrCtrl.o

##
## uncomment following lines for customized linker script
##
#CPPFLAGS_vmlinux.lds := -DTEXT_OFFSET=$(TEXT_OFFSET)
#export KBUILD_LDS := arch/arm64/bsp/vmlinux.lds
#extra-y += vmlinux.lds
