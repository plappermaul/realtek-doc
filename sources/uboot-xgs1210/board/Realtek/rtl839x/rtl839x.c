#include <common.h>
#include <asm/addrspace.h>
#include <asm/io.h>
#include <asm/mipsregs.h>
#include <asm/otto_pll.h>

#define MIPS_COMPANY_ID (0x01<<16)
#define PROCESSOR_ID_34K (0x95<<8)

phys_size_t initdram(int board_type __attribute__((unused))) {
#define DCR (*((volatile int *)(0xb8001004)))
	const unsigned char BNKCNTv[] = {1, 2, 3};
	const unsigned char BUSWIDv[] = {0, 1, 2};
	const unsigned char ROWCNTv[] = {11, 12, 13, 14, 15, 16};
	const unsigned char COLCNTv[] = {8, 9, 10, 11, 12};

	return 1 << (BNKCNTv[(DCR >> 28) & 0x3] +
	             BUSWIDv[(DCR >> 24) & 0x3] +
	             ROWCNTv[(DCR >> 20) & 0xF] +
	             COLCNTv[(DCR >> 16) & 0xF]);
}

int checkboard (void) {
	printf("Board: RTL839x CPU:%dMHz LXB:%dMHz MEM:%dMHz\n",
	       board_CPU_freq_mhz(), board_LX_freq_mhz(), board_DRAM_freq_mhz());

	return 0;
}
