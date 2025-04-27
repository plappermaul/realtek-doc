#ifndef _RTL8380_REG_H
#define _RTL8380_REG_H

#include "interrupt_reg.h"
#include "timer_reg.h"

#include <rtk_reg.h>
#include "rtl8380_soc_reg.h"

#define UMSAR0 (SOC_BASE_ADDR | RTL8380_UNMAP_MEM_SEG_ADDR_ADDR)
#define EnUNMAP (0x01)

#define UMSSR0 (UMSAR0+0x4)

#define SRAMSAR0 (SOC_BASE_ADDR | RTL8380_SRAM_SEG_ADDR_ADDR)
#define SRAMSSR0 (SRAMSAR0+0x4)
#define SRAMSBR0 (SRAMSAR0+0x8)
#define SPSRAMCR (SRAMSAR0+0xC)
#define EnSRAM (0x01)

#define SRAM_SIZE_128B  (0x0)
#define SRAM_SIZE_256B  (0x1)
#define SRAM_SIZE_512B  (0x2)
#define SRAM_SIZE_1KB   (0x3)
#define SRAM_SIZE_2KB   (0x4)
#define SRAM_SIZE_4KB   (0x5)
#define SRAM_SIZE_8KB   (0x6)
#define SRAM_SIZE_16KB  (0x7)
#define SRAM_SIZE_32KB  (0x8)
#define SRAM_SIZE_64KB  (0x9)
#define SRAM_SIZE_128KB (0xA)

#define GLOBAL_PLL_CNTL         (0xBB030008)
#define CPU_PLL_CNTL            (0xBB030018)
#define LX_PLL_CNTL             (0xBB03001C)
#define MEM_PLL_CNTL            (0xBB030020)
#define PAKAGE_ID_ADDR		    (0xBB061028)

#define MEM_PLL_S_MASK          (0x00000700)
#define CPU_PLL_S_MASK          (0x0000000F)
#define LX_PLL_S_MASK           (0x000000F0)
#define PLL_N_MASK              (0x00007e00)

#define CALI_FAIL_DCDR_VALUE 	(0xFFFE0000)
#define CALI_FAIL_DDCR_VALUE 	(0xFFFFFC00)
#endif /*_RTL8380_REG_H*/
