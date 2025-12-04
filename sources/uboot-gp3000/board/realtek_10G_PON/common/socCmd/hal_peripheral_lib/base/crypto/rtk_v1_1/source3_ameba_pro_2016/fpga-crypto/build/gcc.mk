#
#  Copyright (c) 2011 Arduino.  All right reserved.
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License, or (at your option) any later version.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#  See the GNU Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
#

# Tool suffix when cross-compiling

ARM_GCC_TOOLCHAIN=/cygdrive/d/Workspace/toolchain/gcc-arm-5.2/bin


CROSS_COMPILE = $(ARM_GCC_TOOLCHAIN)/arm-none-eabi-



# Compilation tools
AR = $(CROSS_COMPILE)ar
CC = $(CROSS_COMPILE)gcc
CPP = $(CROSS_COMPILE)g++
AS = $(CROSS_COMPILE)as
NM = $(CROSS_COMPILE)nm
OBJDUMP = $(CROSS_COMPILE)objdump
RM=rm -Rf

SEP=/

# ---------------------------------------------------------------------------------------
# C Flags

CFLAGS = -g -gdwarf-3 
CFLAGS += -mcpu=cortex-m3 -mthumb -march=armv7-m
CFLAGS += -c -nostartfiles -fno-short-enums
CFLAGS += -Wall -Wpointer-arith -Wstrict-prototypes -Wundef
CFLAGS += -Wno-write-strings
CFLAGS += --save-temps
CFLAGS += -MMD 
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -DCONFIG_PLATFORM_8195a -DTARGET_LIKE_CORTEX_M3
CFLAGS += $(OPTIMIZATION) $(INCLUDES) 



# ---------------------------------------------------------------------------------------
# ASM Flags

ASFLAGS = -mcpu=cortex-m3 -mthumb -Wall -a -g $(INCLUDES)

