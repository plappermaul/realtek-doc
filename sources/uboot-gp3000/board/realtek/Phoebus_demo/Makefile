include $(srctree)/arch/otto/function.makefile

PLATFORM_CPPFLAGS += -I./board/$(VENDOR)/common -I./board/$(BOARDDIR) -I$(OTTO_MIRROR)

PROJ_FOLDER := $(CONFIG_PROJ_FOLDER:"%"=%)

obj-y += board.o cmd_otp.o
ifneq ($(CONFIG_MULITCASTUPGRADE_BIND_WPSBUTTON),)
obj-y += swp_gpio_state.o
endif
ifneq ($(PROJ_FOLDER),)
PROJSWP_OBJ:=$(shell ls ./board/$(BOARDDIR)/$(PROJ_FOLDER)/swp_*.c)
PROJSWP_OBJ:=$(PROJSWP_OBJ:.c=.o)
PROJSWP_OBJ:=$(notdir $(PROJSWP_OBJ))
PROJSWP_OBJ:=$(addprefix $(PROJ_FOLDER)/,$(PROJSWP_OBJ))
obj-y += $(PROJSWP_OBJ)
else
obj-y += swp_flash_layout.o
obj-y += swp_led_function.o
obj-y += swp_loader_crc.o
obj-y += swp_sds_cmd.o
endif


OTTO_PLAT_DIR := $(OTTO_TOP)/src/platform/Phoebus
OTTO_OSC_H := $(OTTO_MIRROR)/osc.h
OTTO_CG_DEV_H := $(OTTO_MIRROR)/cg_dev_freq.h

# this target is the entry to sync. board/project-specific modules between Otto
# and U-Boot with $(get_otto_file) and $(get_otto_dir) defined in function.makefile.
PHONY += otto_board_sync
otto_board_sync:
	@$(call get_otto_file,$(OTTO_OSC_H),$(OTTO_PLAT_DIR)/misc/osc.h)
	@$(call get_otto_file,$(OTTO_CG_DEV_H),$(OTTO_PLAT_DIR)/cg/cg_dev_freq.h)
