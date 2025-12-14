#
# Copyright (C) 2011 Realtek Semiconductor Corp.
#
# Makefile for SDK PTP Slave protocol Source
#

APOLLO_SDK_PATH = $(SDKDIR)

LIB += -L$(APOLLODIR)/object/src/app/lib -lrtk -lpthread -lpr -lrt

SDK_ROOT_DIR = $(APOLLO_SDK_PATH)
SDK_INCLUDE   = $(SDK_ROOT_DIR)/include
SYS_DIR = $(SDK_ROOT_DIR)/system/linux
SYS_INCLUDE = $(SDK_ROOT_DIR)/system/include
RTUSR_INC = $(SYS_DIR)/rtk/rtusr/include
COMMON_DIR = $(SDK_ROOT_DIR)/src/common


PTP_SLAVE_DIR := .
PTP_SLAVE_INC_DIR  = $(PTP_SLAVE_DIR)/inc
PTP_SLAVE_SRC_DIR := $(PTP_SLAVE_DIR)/src

INCLUDE += -I$(SDK_INCLUDE) \
           -I$(SYS_INCLUDE) \
           -I$(RTUSR_INC) \
           -I$(SDK_INCLUDE)/hal/chipdef/apollo \
           -I$(SDKDIR)/src/app/pkt_redirect/inc \
           -I$(PTP_SLAVE_INC_DIR) \
           -I$(SYS_DIR)

EXEC = ptpslave
BIN = bin


OBJ_PTPSLAVE := $(PTP_SLAVE_SRC_DIR)/ptp_slave_main.o \
                $(PTP_SLAVE_SRC_DIR)/ptp_slave_rx.o \
                $(PTP_SLAVE_SRC_DIR)/ptp_slave_calc.o

$(EXEC): $(OBJ_PTPSLAVE)
	mkdir -p $(BIN)
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) $(INCLUDE) $(OBJ_PTPSLAVE) $(LIB) -o $(BIN)/$@
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
	rm -f $(OBJ_PTPSLAVE) $(BIN)/$(EXEC) *.gdb *.elf *.elf2flt

