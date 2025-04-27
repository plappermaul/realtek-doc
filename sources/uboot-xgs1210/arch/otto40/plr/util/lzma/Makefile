TARGET := lzma
BIN := ../bin
OBJS = \
  LzmaUtil.o \
  Alloc.o \
  LzFind.o \
  LzmaDec.o \
  LzmaEnc.o \
  7zFile.o \
  7zStream.o \

CXX := g++ -I ../../release/ 
CFLAGS = -c -O2 -Wall -D_7ZIP_ST
CFLAGS += -m32

all: $(TARGET)

preconfig: $(TARGET)
	mv -f $(TARGET) $(BIN)
	$(MAKE) clean clean_dep

$(TARGET) : $(OBJS)
	$(CXX) -m32 -static -o $(TARGET) $(OBJS)

LzmaUtil.o: LzmaUtil.c
	$(CXX) $(CFLAGS) LzmaUtil.c

.c.o:
	$(CXX) $(CFLAGS) $^


.PHONY: clean clean_dep distclean all
clean:
	-rm -f $(TARGET) $(OBJS)

	
distclean: clean 

