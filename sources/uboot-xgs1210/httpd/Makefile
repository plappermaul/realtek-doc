include $(TOPDIR)/config.mk

# Suppress warnings when building with size optimization
CFLAGS += -fno-strict-aliasing

# no fsdata.o it's included in fs.o
LIB   = libhttpd.a
OBJS += uip.o uip_arch.o uip_arp.o httpd.o fs.o

all:    $(LIB)

$(LIB): $(OBJS)
	    $(AR) crv $@ $(OBJS)

#########################################################################

.depend: Makefile $(OBJS:.o=.c)
	     $(CC) -M $(CFLAGS) $(OBJS:.o=.c) > $@

sinclude .depend

#########################################################################

