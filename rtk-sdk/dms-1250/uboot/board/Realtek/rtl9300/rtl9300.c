#include <common.h>
#include <asm/otto_pll.h>
#include "cg_dev_freq.h"
#include "osc.h"

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

#ifdef CONFIG_SPI_NOR_FLASH
#include <nor_spi/nor_spif_register.h>
inline static void sw_set_spif_div(void) {
        extern otto_soc_context_t otto_sc;
        uint32_t div = otto_sc.spif_div;
        if(0xFF != div) {
            RMOD_SFCR(spi_clk_div, div);
        }
	#ifdef CONFIG_SPI_NAND_FLASH
		u32_t tmp = REG32(0xB801A400);
		tmp &= ~(0x3F<<4); // bit 4~9
                tmp |= otto_sc.snaf_div<<4;
		tmp |= otto_sc.snaf_pl<<8;
		REG32(0xB801A400) = tmp;
		//printf("DD: NOR: %dMHz, NAND: %dMHz\n",
		//	((cg_dev_freq_t *)(&otto_sc.cg_mhz))->spif_mhz,
		//	((cg_dev_freq_t *)(&otto_sc.cg_mhz))->snaf_mhz);
	#endif
}
PATCH_REG(sw_set_spif_div, 2);
#endif //CONFIG_SPI_NOR_FLASH

int checkboard (void) {
	//printf("Board: RTL9300 CPU:%dMHz LX:%dMHz DDR%d:%dMHz\n",
	//       board_CPU_freq_mhz(),
	//       board_LX_freq_mhz(),
	//       dram_type(), board_DRAM_freq_mhz());
        CAMEO_SIMPLE_DISPLAY_PRINTF(("Board: RTL9300 CPU:%dMHz LX:%dMHz DDR:%dMHz\n",
	       board_CPU_freq_mhz(),
	       board_LX_freq_mhz(),
               board_DRAM_freq_mhz()));
	return 0;
}
