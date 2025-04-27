#ifndef __RTK_REG_H__
#define __RTK_REG_H__

#include <configs/autoconf.h>

#if CONFIG_LOADER_SHARE_SDK_CODE

    #include "ioal/ioal_init.h"
    #include "soc/type.h"
    #include "ioal/mem32.h"
    #define SWCORE_BASE_ADDR    SWCORE_VIRT_BASE
    #define SOC_BASE_ADDR       SOC_VIRT_BASE
    #define MEM32_WRITE_FIELD(reg,field_offset,field_mask,val)\
        {\
            uint32 regData;\
            regData = MEM32_READ(reg);\
            regData = regData & (~(field_mask));\
            regData = regData | (((val) << (field_offset)) & (field_mask));\
            MEM32_WRITE(reg, regData);\
        }
#else /* CONFIG_LOADER_SHARE_SDK_CODE */

#define SWCORE_BASE_ADDR    0xBB000000
#define SOC_BASE_ADDR       0xB8000000

#ifndef REG32
//#define REG32(reg)          (*((volatile uint32 *)(reg)))
#define REG32(reg)   (*(volatile unsigned int *)((unsigned int)reg))
#endif

#if defined(__little_endian__)
/*
 * big_endian() is a macro to make endian consistence between Chip and x86 PC.
 * All the ASIC registers stored in memory will be stored at big-endian order.
 * Therefore, we will see the same result when memory dump ASIC registers.
 */
 #ifndef big_endian32
 #define big_endian32(x) ((((x)&0xff000000)>>24)|(((x)&0x00ff0000)>>8)|(((x)&0x0000ff00)<<8)|(((x)&0x000000ff)<<24))
 #endif
#else
 #ifndef big_endian32
 #define big_endian32(x) (x)
 #endif
#endif

#ifndef MEM32_READ
#define MEM32_READ(reg)      big_endian32(REG32(reg))
#endif
#ifndef MEM32_WRITE
#define MEM32_WRITE(reg,val) REG32(reg)=big_endian32(val)
#endif
#ifndef MEM32_WRITE_FIELD
#if defined(CONFIG_RTL8380_TEST_CHIP)
#define SWCORE_REG_ACCESS_DELAY_TIME 1 /*1ms*/
#define MEM32_WRITE_FIELD(reg,field_offset,field_mask,val)\
    {\
        uint32 regData;\
        regData = MEM32_READ(reg);\
        OSAL_MDELAY(SWCORE_REG_ACCESS_DELAY_TIME);\
        regData = regData & (~field_mask);\
        regData = regData | ((val << field_offset) & field_mask);\
        MEM32_WRITE(reg, regData);\
        OSAL_MDELAY(SWCORE_REG_ACCESS_DELAY_TIME);\
    }
#else
#define MEM32_WRITE_FIELD(reg,field_offset,field_mask,val)\
    {\
        uint32 regData;\
        regData = MEM32_READ(reg);\
        regData = regData & (~(field_mask));\
        regData = regData | (((val) << (field_offset)) & (field_mask));\
        MEM32_WRITE(reg, regData);\
    }
#endif
#endif
#endif /* CONFIG_LOADER_SHARE_SDK_CODE */

#endif/*__RTK_REG_H__*/

