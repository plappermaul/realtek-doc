#include <common.h>
#include <asm/otto_pll.h>

//#define MIPS_COMPANY_ID (0x01<<16)
//#define PROCESSOR_ID_34K (0x95<<8)

extern u32 board_CPU_freq_hz(void);
extern u32 board_LX_freq_hz(void);
extern u32 dram_type(void);

phys_size_t initdram(int board_type __attribute__((unused))) {
	const unsigned char BNKCNTv[] = {1, 2, 3};
	const unsigned char BUSWIDv[] = {0, 1, 2};
	const unsigned char ROWCNTv[] = {11, 12, 13, 14, 15, 16};
	const unsigned char COLCNTv[] = {8, 9, 10, 11, 12};
	uint32_t dcr = *((volatile int *)(0xb8001004));

	return 1 << (BNKCNTv[(dcr >> 28) & 0x3] +
	             BUSWIDv[(dcr >> 24) & 0x3] +
	             ROWCNTv[(dcr >> 20) & 0xF] +
	             COLCNTv[(dcr >> 16) & 0xF]);
}

int checkboard (void) {
	//printf("Board: RTL9300 CPU:%dMHz LX:%dMHz DDR%d:%dMHz\n",
	//       board_CPU_freq_mhz(),
	//       board_LX_freq_mhz(),
	//       dram_type(), board_DRAM_freq_mhz());
        printf("Board: RTL9300 CPU:%dMHz LX:%dMHz DDR:%dMHz\n",
	       board_CPU_freq_mhz(),
	       board_LX_freq_mhz(),
               board_DRAM_freq_mhz());
	return 0;
}
