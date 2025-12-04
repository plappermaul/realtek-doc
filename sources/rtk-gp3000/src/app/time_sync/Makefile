#
# Copyright (C) 2011 Realtek Semiconductor Corp.
#
# Makefile for SDK Time Sync protocol Source
#

APOLLO_SDK_PATH = $(SDKDIR)

LIB += -L$(APOLLODIR)/object/src/app/lib -lrtk -lpthread -lpr -lrt

SDK_ROOT_DIR = $(APOLLO_SDK_PATH)
SDK_INCLUDE   = $(SDK_ROOT_DIR)/include
SYS_DIR = $(SDK_ROOT_DIR)/system/linux
SYS_INCLUDE = $(SDK_ROOT_DIR)/system/include
RTUSR_INC = $(SYS_DIR)/rtk/rtusr/include
COMMON_DIR = $(SDK_ROOT_DIR)/src/common


TIME_SYNC_DIR := .
TIME_SYNC_INC_DIR  = $(TIME_SYNC_DIR)/inc
TIME_SYNC_SRC_DIR := $(TIME_SYNC_DIR)/src

INCLUDE += -I$(SDK_INCLUDE) \
           -I$(SYS_INCLUDE) \
           -I$(RTUSR_INC) \
           -I$(SDK_INCLUDE)/hal/chipdef/apollo \
           -I$(SDKDIR)/src/app/pkt_redirect/inc \
           -I$(TIME_SYNC_INC_DIR) \
           -I$(SYS_DIR)

BIN = bin
EXEC = timesync
EXEC_CLI = timesync_cli

SH = sh

OBJ_TIMESYNC := $(TIME_SYNC_SRC_DIR)/time_sync.o \
			$(TIME_SYNC_SRC_DIR)/time_sync_monitor.o \
			$(TIME_SYNC_SRC_DIR)/time_sync_intr.o 

OBJ_TIMESYNC_CLI := $(TIME_SYNC_SRC_DIR)/time_sync_cli.o 


$(EXEC): $(OBJ_TIMESYNC)
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) $(INCLUDE) $(OBJ_TIMESYNC) $(LIB) -o $(BIN)/$@
	$(STRIP) $(BIN)/$@
	
$(EXEC_CLI): $(OBJ_TIMESYNC_CLI)
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) $(INCLUDE) $(OBJ_TIMESYNC_CLI) $(LIB) -o $(BIN)/$@
	$(STRIP) $(BIN)/$@

%.o: %.c
	$(CC) $(INCLUDE) $(CFLAGS) $(EXTRA_CFLAGS) -c -o $@ $<

# Force to rebuild all every time
all: clean  $(EXEC) $(EXEC_CLI)

install: $(EXEC)
	chmod 775 $(BIN)/$(EXEC)
	cp -af $(BIN)/$(EXEC) $(BINDIR)
	cp -af $(SH)/ppstod.sh $(ROMFSDIR)/etc/
	cp -af $(SH)/ptpmaster.sh $(ROMFSDIR)/etc/

clean:
	rm -f $(OBJ_TIMESYNC) $(OBJ_TIMESYNC_CLI) $(BIN)/$(EXEC) $(BIN)/$(EXEC_CLI) *.gdb *.elf *.elf2flt

