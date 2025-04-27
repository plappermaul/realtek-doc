#include <common.h>
#include <asm/otto_pll.h>
#include "cg_dev_freq.h"
#include "osc.h"

//#define MIPS_COMPANY_ID (0x01<<16)
//#define PROCESSOR_ID_34K (0x95<<8)

extern u32 board_CPU_freq_hz(void);
extern u32 board_LX_freq_hz(void);
extern u32 dram_type(void);

int plat_get_dram_size(void) {
#if 0
    extern otto_soc_context_t otto_sc;
    return (otto_sc.memsize<<20);
#else
    u32_t v = REG32(0xB814304C);
    u32_t b = v >> 12;
    u32_t r = (v >> 6) & 0x3F;
    u32_t c = v & 0x3F;
    return 1 << (b+r+c);
#endif
}

phys_size_t initdram(int board_type __attribute__((unused))) {
	return plat_get_dram_size();
}

#ifdef CONFIG_SPI_NOR_FLASH
#include <nor_spi/nor_spif_register.h>
#define SNAFCFRrv REG32(0xB801A400)
inline static void sw_set_spif_div(void) {
	extern otto_soc_context_t otto_sc;
	RMOD_SFCR(spi_clk_div, otto_sc.spif_div);
	#ifdef CONFIG_SPI_NAND_FLASH
		u32_t tmp = SNAFCFRrv;
		tmp &= ~(0x3F<<4); // bit 4~9
		tmp |= (otto_sc.snaf_pl<<8) | (otto_sc.snaf_div<<4);
		SNAFCFRrv = tmp;
	#endif
}
PATCH_REG(sw_set_spif_div, 2);
#endif //CONFIG_SPI_NOR_FLASH

int checkboard (void) {
    printf("Board: RTL9310 CPU:%dMHz LX:%dMHz DDR:%dMHz\n",
    board_CPU_freq_mhz(),
	board_LX_freq_mhz(),
    board_DRAM_freq_mhz());
	return 0;
}
