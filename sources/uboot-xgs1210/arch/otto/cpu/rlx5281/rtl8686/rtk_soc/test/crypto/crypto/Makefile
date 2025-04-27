include $(TOPDIR)/config.mk

# CFLAGS += -DET_DEBUG -DDEBUG

O_TARGTE	= $(obj)crypto.o

COBJS   = rtl865xc_cryptoEngine.o desSim.o destest.o crypto_glue.o set_key.o ecb_enc.o
COBJS	+= des_enc.o rtl865xc_authEngine.o md5.o hmac.o authTest.o authSim.o sha1.o
COBJS	+= aesSim.o aes_core.o aes_block.o aesTest.o util.o cryptoCmd.o gdmaTest.o rtl_glue.o rtl865xc_ipsecEngine.o hmactest.o

SRCS	:= $(COBJS:.o=.c)
OBJS	:= $(addprefix $(obj),$(COBJS))

all:	$(O_TARGTE)

$(O_TARGTE):	$(obj).depend $(OBJS)
		$(LD) -r -o $@ $(OBJS)


# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

