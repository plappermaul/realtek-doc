#ifndef __SRAMCTL_H__
#define __SRAMCTL_H__
#ifndef __ASM__
#include "rtk_soc_common.h"
#endif


#define SRAM_REG_SET_SIZE	(0x10)
#define SRAM_REG_SET_NUM	(4)
#define SRAM_REG_BASE_ADDR	(0xB8004000)

#define C0SRAMSAR_REG_ADDR	(SRAM_REG_BASE_ADDR + 0x0)
#define C0SRAMSSR_REG_ADDR	(SRAM_REG_BASE_ADDR + 0x4)
#define C0SRAMSBR_REG_ADDR	(SRAM_REG_BASE_ADDR + 0x8)

#define C1SRAMSAR_REG_ADDR	(SRAM_REG_BASE_ADDR + 0x40)
#define C1SRAMSSR_REG_ADDR	(SRAM_REG_BASE_ADDR + 0x44)
#define C1SRAMSBR_REG_ADDR	(SRAM_REG_BASE_ADDR + 0x48)

#define SRAM_SEG_ENABLE		(0x1)

#define SRAM_SIZE_256B		(0x1)
#define SRAM_SIZE_512B		(0x2)
#define SRAM_SIZE_1KB		(0x3)
#define SRAM_SIZE_2KB		(0x4)
#define SRAM_SIZE_4KB		(0x5)
#define SRAM_SIZE_8KB		(0x6)
#define SRAM_SIZE_16KB		(0x7)
#define SRAM_SIZE_32KB		(0x8)
#define SRAM_SIZE_64KB		(0x9)
#define SRAM_SIZE_128KB		(0xA)
#define SRAM_SIZE_256KB		(0xB)
#define SRAM_SIZE_512KB		(0xC)
#define SRAM_SIZE_1MB		(0xD)

int sram_mapping(unsigned int segNo, unsigned int cpu_addr, \
		 unsigned int sram_addr, unsigned int sram_size_no);
int sram_unmapping(unsigned int segNo);
void sram_show_mapinfo(void);
unsigned int _is_Master_CPU(void);

#endif //end of __SRAMCTL_H__
