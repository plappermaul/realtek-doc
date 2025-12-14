#
# Copyright (C) 2011 Realtek Semiconductor Corp.
#
# Makefile for SDK PTP Master protocol Source
#

APOLLO_SDK_PATH = $(SDKDIR)

LIB += -L$(APOLLODIR)/object/src/app/lib -lrtk -lpthread -lpr -lrt

SDK_ROOT_DIR = $(APOLLO_SDK_PATH)
SDK_INCLUDE   = $(SDK_ROOT_DIR)/include
SYS_DIR = $(SDK_ROOT_DIR)/system/linux
SYS_INCLUDE = $(SDK_ROOT_DIR)/system/include
RTUSR_INC = $(SYS_DIR)/rtk/rtusr/include
COMMON_DIR = $(SDK_ROOT_DIR)/src/common


PTP_MASTER_DIR := .
PTP_MASTER_INC_DIR  = $(PTP_MASTER_DIR)/inc
PTP_MASTER_SRC_DIR := $(PTP_MASTER_DIR)/src

INCLUDE += -I$(SDK_INCLUDE) \
           -I$(SYS_INCLUDE) \
           -I$(RTUSR_INC) \
           -I$(SDK_INCLUDE)/hal/chipdef/apollo \
           -I$(SDKDIR)/src/app/pkt_redirect/inc \
           -I$(PTP_MASTER_INC_DIR) \
           -I$(SYS_DIR)

EXEC = ptpmaster
BIN = bin


OBJ_PTPMASTER := $(PTP_MASTER_SRC_DIR)/ptp_master_main.o \
                 $(PTP_MASTER_SRC_DIR)/ptp_master_rx.o \
                 $(PTP_MASTER_SRC_DIR)/ptp_master_timer.o


$(EXEC): $(OBJ_PTPMASTER)
	mkdir -p $(BIN)
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) $(INCLUDE) $(OBJ_PTPMASTER) $(LIB) -o $(BIN)/$@
	$(STRIP) $(BIN)/$@

%.o: %.c
	$(CC) $(INCLUDE) $(CFLAGS) $(EXTRA_CFLAGS) -c -o $@ $<

# Force to rebuild all every time
all: clean  $(EXEC)

install: $(EXEC)
	chmod 775 $(BIN)/$(EXEC)
	cp -af $(BIN)/$(EXEC) $(BINDIR)
	@if [ -f $(SDK_ROOT_DIR)/src/module/ptp_drv/ptp_drv.ko ] ; then \
	    $(MAKE) -C $(SDK_ROOT_DIR)/src/module/ptp_drv install; \
	fi

clean:
	rm -f $(OBJ_PTPMASTER) $(BIN)/$(EXEC) *.gdb *.elf *.elf2flt

