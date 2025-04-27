############################################################################
SDK_AUTOCONG_H=sdk_autoconf.h

all:

clean:
	$(MAKE) -C $(SDK_SCRIPTSDIR) clean
	$(MAKE) -C $(SDK_SCRIPTSDIR)/lxdialog clean
	@rm -f config.tk autoconf.h .config .config.old $(SDK_INC_COMMON)/$(SDK_AUTOCONG_H)

############################################################################
#
# The config stuff
#

ifeq ("x$(CONFIG_SHELL)","x")
CONFIG_SHELL := $(shell if [ -x "$$BASH" ]; then echo $$BASH; \
		else if [ -x /bin/bash ]; then echo /bin/bash; \
		else echo sh; fi ; fi)
endif

SDK_INC_COMMON := ../system/include/common
SDK_SCRIPTSDIR  := scripts

config.tk: Makefile config.in $(SDK_SCRIPTSDIR)/header.tk $(SDK_SCRIPTSDIR)/tail.tk \
		$(wildcard $(ROOTDIR)/prop/config.in)
	$(MAKE) -C $(SDK_SCRIPTSDIR) tkparse
	ARCH=m68knommu $(SDK_SCRIPTSDIR)/tkparse < config.in > config.tmp
	@if [ -f /usr/local/bin/wish ];	then \
		echo '#!'"/usr/local/bin/wish -f" > config.tk; \
	else \
		echo '#!'"/usr/bin/wish -f" > config.tk; \
	fi
	cat $(SDK_SCRIPTSDIR)/header.tk >> ./config.tk
	cat config.tmp >> config.tk
	rm -f config.tmp
	echo "set defaults \"defconfig\"" >> config.tk
	echo "set autoconf_file \"autoconf.h\"" >> config.tk
	echo "set help_file \"Configure.help\"" >> config.tk
	cat $(SDK_SCRIPTSDIR)/tail.tk >> config.tk
	chmod 755 config.tk

xconfig: config.tk
	@$(CONFIG_SHELL) -n config.in
	wish -f config.tk

oldconfig:
	@$(CONFIG_SHELL) -n config.in
	@HELP_FILE=Configure.help \
		AUTOCONF_FILE=autoconf.h \
		$(CONFIG_SHELL) $(SDK_SCRIPTSDIR)/Configure -d config.in

config:
	@$(CONFIG_SHELL) -n config.in
	@HELP_FILE=Configure.help \
		AUTOCONF_FILE=autoconf.h \
		$(CONFIG_SHELL) $(SDK_SCRIPTSDIR)/Configure config.in
	@cp autoconf.h $(SDK_INC_COMMON)/$(SDK_AUTOCONG_H)

menuconfig:
	@$(CONFIG_SHELL) -n config.in
	make -C $(SDK_SCRIPTSDIR)/lxdialog all
	@HELP_FILE=Configure.help \
		AUTOCONF_FILE=autoconf.h \
			$(CONFIG_SHELL) $(SDK_SCRIPTSDIR)/Menuconfig config.in
	@cp autoconf.h $(SDK_INC_COMMON)/$(SDK_AUTOCONG_H)

sdkconfig: menuconfig

############################################################################

