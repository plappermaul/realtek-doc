TARGET := bch6enc
BIN := ../bin
OBJS := main.o bch6.o ecc.o

CXX := g++ -I ../../release/ 
#CFLAGS = -Wall -O0 -g -Wno-unused-result
CFLAGS ?= -Wall -O3 -Wno-unused-result
#LDFLAGS = -L/usr/lib -lm

ALL_CPP := $(patsubst %.o,%.cpp,$(OBJS))
ALL_DEP := $(patsubst %.o,%.d,$(OBJS))

all: $(TARGET)

preconfig: $(TARGET)
	mv -f $(TARGET) $(BIN)
	$(MAKE) clean clean_dep

$(TARGET) : $(OBJS)
	$(CXX) -o $(TARGET) $^ $(LDFLAGS)

$(OBJS) : %.o : %.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

ifeq ($(MAKECMDGOALS),all)
include $(ALL_DEP)
endif

$(ALL_DEP) : %.d : %.cpp
	$(CXX) $(CFLAGS) -MM $< > $@

.PHONY: clean clean_dep distclean all
clean:
	-rm -f $(TARGET) $(OBJS)

clean_dep:
	-rm -f $(ALL_DEP)
	
distclean: clean clean_dep

