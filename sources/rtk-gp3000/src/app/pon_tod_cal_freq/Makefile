#
# Copyright (C) 2011 Realtek Semiconductor Corp.
#
# Makefile for SDK PON ToD Calculate Time Frequency protocol Source
#

APOLLO_SDK_PATH = $(SDKDIR)

LIB += -L$(APOLLODIR)/object/src/app/lib -lrtk -lpthread -lpr -lrt

SDK_ROOT_DIR = $(APOLLO_SDK_PATH)
SDK_INCLUDE   = $(SDK_ROOT_DIR)/include
SYS_DIR = $(SDK_ROOT_DIR)/system/linux
SYS_INCLUDE = $(SDK_ROOT_DIR)/system/include
RTUSR_INC = $(SYS_DIR)/rtk/rtusr/include
COMMON_DIR = $(SDK_ROOT_DIR)/src/common


PON_TOD_DIR := .
PON_TOD_INC_DIR  = $(PON_TOD_DIR)/inc
PON_TOD_SRC_DIR := $(PON_TOD_DIR)/src

INCLUDE += -I$(SDK_INCLUDE) \
           -I$(SYS_INCLUDE) \
           -I$(RTUSR_INC) \
           -I$(SDK_INCLUDE)/hal/chipdef/apollo \
           -I$(SDKDIR)/src/app/pkt_redirect/inc \
           -I$(PON_TOD_INC_DIR) \
           -I$(SYS_DIR)

BIN = bin
EXEC = pontodFreq

OBJ_PONTOD := $(PON_TOD_SRC_DIR)/pontod_freq_main.o \
              $(PON_TOD_SRC_DIR)/pontod_freq_calc.o \
              $(PON_TOD_SRC_DIR)/pontod_freq_intr.o


$(EXEC): $(OBJ_PONTOD)
	$(CC) $(CFLAGS) $(PON_CFLAGS) $(EXTRA_CFLAGS) $(INCLUDE) $(OBJ_PONTOD) $(LIB) -o $(BIN)/$@
	$(STRIP) $(BIN)/$@
	

%.o: %.c
	$(CC) $(INCLUDE) $(CFLAGS) $(PON_CFLAGS) $(EXTRA_CFLAGS) -c -o $@ $<

# Force to rebuild all every time
all: clean  $(EXEC)

install: $(EXEC)
	chmod 775 $(BIN)/$(EXEC)
	cp -af $(BIN)/$(EXEC) $(BINDIR)

clean:
	rm -f $(OBJ_PONTOD) $(BIN)/$(EXEC) *.gdb *.elf *.elf2flt

