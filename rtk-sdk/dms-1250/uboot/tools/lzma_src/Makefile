ifeq ($(QUIET), 0)
COMPILE_PROMPT :=
else
COMPILE_PROMPT = @echo "[CC] $@" &&
endif

PROG = lzma
CXX = g++
RM = rm -f
CFLAGS = -c -O2 -Wall -D_7ZIP_ST

OBJS = \
  LzmaUtil.o \
  Alloc.o \
  LzFind.o \
  LzmaDec.o \
  LzmaEnc.o \
  7zFile.o \
  7zStream.o \

all: $(PROG)

$(PROG): $(OBJS)
	$(COMPILE_PROMPT) $(CXX) -o $(PROG) $(OBJS)

LzmaUtil.o: LzmaUtil.c
	$(COMPILE_PROMPT) $(CXX) $(CFLAGS) LzmaUtil.c

.c.o:
	$(COMPILE_PROMPT) $(CXX) $(CFLAGS) $^

clean:
	-$(RM) $(PROG) $(OBJS)
