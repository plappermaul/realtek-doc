#######################################################3#
#	Build CA Realtek EPON modules			#
#########################################################

SRC_DIR:=`pwd`

all:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC_DIR) SUBDIRS=$(SRC_DIR)

module modules_install clean:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC_DIR) SUBDIRS=$(SRC_DIR) $@

.PHONY: all modules_install clean
