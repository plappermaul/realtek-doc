/*
 * Copyright (C) 2011 Realtek Semiconductor Corp. 
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated, 
 * modified or distributed under the authorized license from Realtek. 
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER 
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED. 
 * Purpose : SoC register definition
 *
 * Feature : Define the SoC register access type
 *
 */

#ifndef __BSP_SOC_TYPE_H__
#define __BSP_SOC_TYPE_H__

/* Register access macro */
#ifndef REG32
#define REG32(reg)      (*((volatile unsigned int *)(reg)))
#endif
#ifndef REG16
#define REG16(reg)      (*((volatile unsigned short *)(reg)))
#endif
#ifndef REG8
#define REG8(reg)       (*((volatile unsigned char *)(reg)))
#endif

/* Register access macro (READ_MEM32() and WRITE_MEM32()). */
#if defined(__MODEL_USER__)
/*
 * big_endian() is a macro to make endian consistence between BIG-Endian board and x86 PC.
 * All the ASIC registers stored in memory will be stored at big-endian order.
 * Therefore, we will see the same result when memory dump ASIC registers.
 */
#define big_endian32(x) ((((x) & 0xff000000) >> 24) | (((x) & 0x00ff0000) >> 8) | (((x) & 0x0000ff00) << 8)|(((x) & 0x000000ff) << 24))
#define big_endian16(x) (((x) >> 8) | (((x) & 0x000000ff) << 8))
#else
#define big_endian32(x) (x)
#define big_endian16(x) (x)
#endif
#define big_endian(x) big_endian32(x) /* backward-compatible */

#define MEM32_READ(reg)         big_endian(REG32(reg))
#define MEM32_WRITE(reg,val)    REG32(reg) = big_endian(val)
#define MEM16_READ(reg)         big_endian16(REG16(reg))
#define MEM16_WRITE(reg,val)    REG16(reg) = big_endian16(val)
#define MEM8_READ(reg)          (REG8(reg))
#define MEM8_WRITE(reg,val)     REG8(reg) = (val)

#ifndef WRITE_MEM32
#define WRITE_MEM32(reg,val)    MEM32_WRITE(reg,val)
#endif
#ifndef WRITE_MEM16
#define WRITE_MEM16(reg,val)    MEM16_WRITE(reg,val)
#endif
#ifndef WRITE_MEM8
#define WRITE_MEM8(reg,val)     MEM8_WRITE(reg,val)
#endif
#ifndef READ_MEM32
#define READ_MEM32(reg)         MEM32_READ(reg)
#endif
#ifndef READ_MEM16
#define READ_MEM16(reg)         MEM16_READ(reg)
#endif
#ifndef READ_MEM8
#define READ_MEM8(reg)          MEM8_READ(reg)
#endif


#if defined(__MODEL_USER__)
#define UNCACHE_MASK            (0)
#define UNCACHE(addr)           (addr)
#define CACHED(addr)            ((unsigned int)(addr))
#else
#define UNCACHE_MASK            (0x20000000)    /* MIPS */
#define UNCACHE(addr)           ((unsigned int)(addr) | (UNCACHE_MASK))
#define CACHED(addr)            ((unsigned int)(addr) & ~(UNCACHE_MASK))
#define KERNEL_MASK             (0x80000000)    /* MIPS */
#define KRNVIRT(addr)           ((unsigned int)(addr) | (KERNEL_MASK))
#endif

#endif /* end of __BSP_SOC_TYPE_H__  */

