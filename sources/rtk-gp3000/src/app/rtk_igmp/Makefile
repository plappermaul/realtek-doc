.PHONY: all clean

include $(APOLLODIR)/Compiler_Flag

SVN_VERSION=`svn info | grep -e Rev: | sed -e 's/.*Rev: //'`
IGMPDIR = ./igmp/src
RSDDIR  = ./rsd/src
UTILDIR = ./util/src
IPCDIR	= ./ipc
LIBSDIR	= ./pf_api/src
DRIVERDIR = ./driver
CONFIGDIR = ./config
CLIDIR = ./cli/src

IGMP_SRCS = $(shell find $(IGMPDIR) -name "*.c")
IGMP_SRCS += $(shell find $(RSDDIR) -name "*.c")
IGMP_SRCS += $(shell find $(UTILDIR) -name "*.c")
IGMP_OBJS = $(IGMP_SRCS:.c=.o)

CLI_SRCS = $(shell find $(CLIDIR) -name "*.c")
CLI_OBJS = $(CLI_SRCS:.c=.o)

IPC_SRCS = $(shell find $(IPCDIR) -name "igmp_api.c")
IPC_OBJS = $(IPC_SRCS:.c=.o)

LIBS_SRCS = $(shell find $(LIBSDIR) -name "*.c")
LIBS_OBJS = $(LIBS_SRCS:.c=.o)

EXEC = igmpd
CMD_EXEC = igmpcli
RUN_IGMP_SCRIPT="runigmp.sh"
LIB_IGMP_IPC    = igmpipc
SO_LIB_IGMP_IPC = lib$(LIB_IGMP_IPC).so
LIB_IGMP_COMM	= rtk_igmp_lib
A_LIB_IGMP_COMM = lib$(LIB_IGMP_COMM).a

LIB += -l$(LIB_IGMP_COMM) -L../lib/
LIB += -L$(APOLLODIR)/object/src/app/lib -lrtk
LIB += -lpr -lpthread -lrt -lm -l$(LIB_IGMP_IPC)
LIB += -lomci_fal 

SDK_INCLUDES   = -I$(SDKDIR)/include \
                 -I$(SDKDIR)/system/include \
                 -I$(SDKDIR)/system/linux \
                 -I$(SDKDIR)/src/app/pkt_redirect/inc \
				 -I$(SDKDIR)/src/app/omci_v1/API

INCLUDES  = -I./include -I./igmp/inc -I./util/inc -I./ -I./pf_api/inc \
            $(SDK_INCLUDES)

CFLAGS += -Wall -Os -Werror -Wno-strict-aliasing
CFLAGS += $(PON_CFLAGS)
CFLAGS += $(INCLUDES)
CFLAGS += -DCONFIG_SVN_VERSION="\"r${SVN_VERSION}\""

IGMPLIBS: $(LIBS_OBJS)
	$(AR) -r ../lib/$(A_LIB_IGMP_COMM) $(LIBS_OBJS)

LIBIPC:
	$(CC) $(CFLAGS) -fPIC -shared -o ../lib/$(SO_LIB_IGMP_IPC) $(IPC_SRCS)
	cp -af ../lib/$(SO_LIB_IGMP_IPC) $(SDKDIR)/../object/src/app/lib/

IGMP:$(IGMP_OBJS)	
	$(CC) $(CFLAGS) $(IGMP_OBJS) $(LIB) -o $(EXEC)

	
IGMPCLI:$(CLI_OBJS)
	$(CC) $(CFLAGS) $(CLI_OBJS) $(LIB) -o $(CMD_EXEC)

DRIVER:
	$(MAKE) -C $(DRIVERDIR) all

all: clean IGMPLIBS LIBIPC IGMPCLI IGMP DRIVER
	
install:
	chmod 775 $(EXEC)
	cp -af $(EXEC) $(BINDIR)
	chmod 775 $(CMD_EXEC)
	cp -af $(CMD_EXEC) $(BINDIR)
	cp -af ../lib/$(SO_LIB_IGMP_IPC) $(ROMFSDIR)/lib/
	cp -af ../lib/$(SO_LIB_IGMP_IPC) $(ROMFSDIR)/lib/features/$(SO_LIB_IGMP_IPC)
	$(ROMFSINST) -s ../$(SO_LIB_IGMP_IPC) /lib/features/$(SO_LIB_IGMP_IPC)
ifeq ($(CONFIG_SDK_RTL9601B),y)
	cp -af $(CONFIGDIR)/igmp_gpon_9601b.conf $(ROMFSDIR)/etc/igmp_gpon.conf
else ifeq ($(CONFIG_SDK_RTL9602C),y)
	cp -af $(CONFIGDIR)/igmp_gpon_9602c.conf $(ROMFSDIR)/etc/igmp_gpon.conf
else
	cp -af $(CONFIGDIR)/igmp_gpon_9607.conf $(ROMFSDIR)/etc/igmp_gpon.conf
endif
	cp -af $(RUN_IGMP_SCRIPT) $(ROMFSDIR)/etc/
	$(MAKE) -C $(DRIVERDIR) install

clean:	
	rm -rf $(EXEC) $(CMD_EXEC)
	find . -name "*.so" -o -name "*.o" -o -name ".a" -o -name $(EXEC) -o -name $(CMD_EXEC) | xargs rm -rf
	find $(SDKDIR)/../object/src/app/lib/ -name $(SO_LIB_IGMP_IPC) -o -name $(A_LIB_IGMP_COMM) | xargs rm -rf 
	rm -rf $(BINDIR)/$(CMD_EXEC) $(BINDIR)/$(EXEC) ../lib/$(CMD_EXEC) ../lib/$(EXEC) ../lib/$(SO_LIB_IGMP_IPC) ../lib/$(A_LIB_IGMP_COMM)
	rm -rf $(ROMFSDIR)/etc/igmp_gpon.conf $(ROMFSDIR)/lib/features/$(SO_LIB_IGMP_IPC) $(ROMFSDIR)/lib/$(SO_LIB_IGMP_IPC) $(SDKDIR)/src/app/bin/$(CMD_EXEC) $(SDKDIR)/src/app/bin/$(EXEC)
	$(MAKE) -C $(DRIVERDIR) clean
