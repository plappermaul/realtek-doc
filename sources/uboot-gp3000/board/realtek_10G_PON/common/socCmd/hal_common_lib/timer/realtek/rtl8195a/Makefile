
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean


#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS = 

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#

# Define the Rules to build the core targets
all: CORE_TARGETS
ifeq ($(CONFIG_TIMER_VERIFY),y)
	make -C tests all
	make -C src all
else
	make -C src all
endif		

	

#*****************************************************************************#
#              GENERATE OBJECT FILE 
#*****************************************************************************#
CORE_TARGETS: $(OBJS)



