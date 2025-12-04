include $(srctree)/arch/otto/function.makefile

PLATFORM_CPPFLAGS += -I./board/$(VENDOR)/common -I./board/$(BOARDDIR)

obj-y = board.o

OTTO_PLAT_DIR := $(OTTO_TOP)/src/platform/VD2_g2
OTTO_OSC_H := $(OTTO_MIRROR)/osc.h
OTTO_CG_DEV_H := $(OTTO_MIRROR)/cg_dev_freq.h

# this target is the entry to sync. board/project-specific modules between Otto
# and U-Boot with $(get_otto_file) and $(get_otto_dir) defined in function.makefile.
PHONY += otto_board_sync
otto_board_sync:
	@$(call get_otto_file,$(OTTO_OSC_H),$(OTTO_PLAT_DIR)/misc/osc.h)
	@$(call get_otto_file,$(OTTO_CG_DEV_H),$(OTTO_PLAT_DIR)/cg/cg_dev_freq.h)
