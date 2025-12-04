include $(SDKDIR)/src/dal/rtl8198f/DAL_Compiler_Flag

SRC_DIR:=`pwd`

DAL_SRC:= $(patsubst %.o,%.c, $(DAL_OBJ))

all:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC_DIR) SUBDIRS=$(SRC_DIR)

module modules_install clean:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC_DIR) SUBDIRS=$(SRC_DIR) $@

.PHONY: all modules_install clean
