/*
 * Copyright (C) 2009-2016 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 100813 $
 * $Date: 2019-10-08 22:22:51 +0800 (Tue, 08 Oct 2019) $
 *
 * Purpose : SoC register definition
 *
 * Feature : Define the SoC register access type
 *
 */

#ifndef __BSP_SOC_TYPE_H__
#define __BSP_SOC_TYPE_H__

#include <common/type.h>


/* Register access macro */
#ifndef REG32
#define REG32(reg)      (*((volatile uint32 *)(uintptr)(reg)))
#endif
#ifndef REG16
#define REG16(reg)      (*((volatile uint16 *)(uintptr)(reg)))
#endif
#ifndef REG8
#define REG8(reg)       (*((volatile uint8 *)(uintptr)(reg)))
#endif


#define byte_swap64(x) \
     (((((x) & 0xff00000000000000ull) >> 56)                      \
     | (((x) & 0x00ff000000000000ull) >> 40)                      \
     | (((x) & 0x0000ff0000000000ull) >> 24)                      \
     | (((x) & 0x000000ff00000000ull) >> 8)                      \
     | (((x) & 0x00000000ff000000ull) << 8)                      \
     | (((x) & 0x0000000000ff0000ull) << 24)                      \
     | (((x) & 0x000000000000ff00ull) << 40)                      \
     | (((x) & 0x00000000000000ffull) << 56)))

#define byte_swap32(x) ((((x) & 0xff000000) >> 24) | (((x) & 0x00ff0000) >> 8) | (((x) & 0x0000ff00) << 8)|(((x) & 0x000000ff) << 24))
#define byte_swap16(x) (((x) >> 8) | (((x) & 0x000000ff) << 8))


#if defined(CONFIG_SDK_ENDIAN_LITTLE)
#define LE16_to_CPU(val) (val)
#define LE32_to_CPU(val) (val)
#define LE64_to_CPU(val) (val)
#define BE16_to_CPU(val) byte_swap16(val)
#define BE32_to_CPU(val) byte_swap32(val)
#define BE64_to_CPU(val) byte_swap64(val)

#define CPU_to_LE16(val) (val)
#define CPU_to_LE32(val) (val)
#define CPU_to_LE64(val) (val)
#define CPU_to_BE16(val) byte_swap16(val)
#define CPU_to_BE32(val) byte_swap32(val)
#define CPU_to_BE64(val) byte_swap64(val)


#else
#define BE16_to_CPU(val) (val)
#define BE32_to_CPU(val) (val)
#define BE64_to_CPU(val) (val)
#define LE16_to_CPU(val) byte_swap16(val)
#define LE32_to_CPU(val) byte_swap32(val)
#define LE64_to_CPU(val) byte_swap64(val)

#define CPU_to_BE16(val) (val)
#define CPU_to_BE32(val) (val)
#define CPU_to_BE64(val) (val)
#define CPU_to_LE16(val) byte_swap16(val)
#define CPU_to_LE32(val) byte_swap32(val)
#define CPU_to_LE64(val) byte_swap64(val)

#endif

/*
 * big_endian() is a macro to make endian consistence between BIG-Endian board and x86 PC.
 * All the ASIC registers stored in memory will be stored at big-endian order.
 * Therefore, we will see the same result when memory dump ASIC registers.
 */
/* Register access macro (READ_MEM32() and WRITE_MEM32()). */
/* Embededed/Internal CPU is MIPS (big endian) and the Lextra Bus is big endian too */
#define MEM32_READ(reg)         BE32_to_CPU(REG32(reg))
#define MEM32_WRITE(reg,val)    REG32(reg) = CPU_to_BE32(val)

/* PCI bus is little endian */
#define PCI_MEM32_READ(reg)         LE32_to_CPU(REG32(reg))
#define PCI_MEM32_WRITE(reg,val)    REG32(reg) = CPU_to_LE32(val)


#define REG32_FIELD_SET(_data, _val, _fOffset, _fMask)      ((_data & ~(_fMask)) | ((_val << (_fOffset)) & (_fMask)))
#define REG32_FIELD_GET(_data, _fOffset, _fMask)            ((_data & (_fMask)) >> (_fOffset))
/* convert the range of bits [_mBit, _lBit] to mask */
#define UINT32_BITS_MASK(_mBit, _lBit)                      ((0xFFFFFFFF >> (31 - _mBit)) ^ ((1 << _lBit) - 1))

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

