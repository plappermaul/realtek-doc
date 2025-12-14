#
# Copyright (C) 2011 Realtek Semiconductor Corp.
#
# Makefile for SDK SEP User space App
#

APOLLO_SDK_PATH = $(SDKDIR)

LIB += -L$(APOLLODIR)/object/src/app/lib -lrtk -lpthread -lpr

SDK_ROOT_DIR = $(APOLLO_SDK_PATH)
SDK_INCLUDE   = $(SDK_ROOT_DIR)/include
SYS_INCLUDE = $(SDK_ROOT_DIR)/system/include


INCLUDE += -I$(SDK_INCLUDE) \
           -I$(SYS_INCLUDE) \
	   -I$(SDKDIR)/src/app/pkt_redirect/inc

SFP_APP_DIR := .
SFP_APP_INC_DIR  = $(SFP_APP_DIR)/inc
SFP_APP_SRC_DIR := $(SFP_APP_DIR)/src
EXEC = sfpapp
BIN = bin

OBJ_SFPAPP := $(SFP_APP_SRC_DIR)/sfp_app_main.o


$(EXEC): $(OBJ_SFPAPP)
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) $(INCLUDE) $(OBJ_SFPAPP) $(LIB) -o $(BIN)/$@
	$(STRIP) $(BIN)/$@

%.o: %.c
	$(CC) $(INCLUDE) $(CFLAGS) $(EXTRA_CFLAGS) -c -o $@ $<

# Force to rebuild all every time
all: clean  $(EXEC)

install: $(EXEC)
	chmod 775 $(BIN)/$(EXEC)
	cp -af $(BIN)/$(EXEC) $(BINDIR)

clean:
	rm -f $(OBJ_SFPAPP) $(BIN)/$(EXEC) *.gdb *.elf *.elf2flt

