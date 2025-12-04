-include toolkit_path.in

-include $(CONFIG_BOARDDIR:"%"=%)/debug.in

ifeq (1,$(tr))
ifdef PREFERRED_TK_PREFIX
CROSS_COMPILE := $(PREFERRED_TK_PREFIX)
endif
endif

ifndef CROSS_COMPILE
$(error EE: missing CROSS_COMPILE. Read toolkit_path.in.sample for detail.)
endif

ifndef OTTO_TOP
$(error EE: missing OTTO_TOP. Read toolkit_path.in.sample for detail.)
endif

# otto build targets
ALL-y += u-boot.code
ALL-y += u-boot.lzma
ALL-y += u-boot.pimg

CLEAN_FILES += u-boot.code u-boot.lzma u-boot.pimg

OTTO_MIRROR = ./board/$(VENDOR)/common/mirror
export OTTO_MIRROR OTTO_TOP

quiet_cmd_objdump = OBJDUMP $@
      cmd_objdump = $(OBJDUMP) -Dlxt $< > $@

u-boot.code: u-boot
	$(call if_changed,objdump)

quiet_cmd_lzma = LZMA    $@
      cmd_lzma = lzma -c $< > $@

u-boot.lzma: u-boot.bin
	$(call if_changed,lzma)

# must use '=' instead of ':=' due to that CONFIG_SYS_TEXT_BASE is not available when
# including otto.makefile.
MKIMAGEFLAGS_u-boot.pimg = -C LZMA -O u-boot -A mips -T firmware
MKIMAGEFLAGS_u-boot.pimg += -e $(CONFIG_SYS_TEXT_BASE) -a $(CONFIG_SYS_TEXT_BASE)

u-boot.pimg: u-boot.lzma
	$(call if_changed,mkimage)

$(CONFIG_BOARDDIR:"%"=%)/debug.in:
