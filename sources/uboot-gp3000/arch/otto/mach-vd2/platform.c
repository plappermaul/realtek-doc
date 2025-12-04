#include <soc.h>

static uint32_t
dram_type(void) {
	uint32_t mcr = REG32(0xb8001000);
	return ((mcr >> 28) + 1);
}

int checkboard (void) {
	printf(
		"Board: VD2, CPU:%dMHz, LX:%dMHz, DDR%d:%dMHz\n",
		GET_CPU_MHZ(),
		GET_LX_MHZ(),
		dram_type(),
		GET_MEM_MHZ());
	return 0;
}
