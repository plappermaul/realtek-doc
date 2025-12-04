#include <soc.h>

DECLARE_GLOBAL_DATA_PTR;

int plat_get_dram_size(void) {
        uint32_t v = REG32(0xB814304C);
        uint32_t b = v >> 12;
        uint32_t r = (v >> 6) & 0x3F;
        uint32_t c = v & 0x3F;
        return 1 << (b+r+c);
}

//phys_size_t dram_init(int board_type __attribute__((unused))) {
int dram_init(void) {
        gd->ram_size = plat_get_dram_size();
        return 0;
}

inline static uint32_t dram_type(void) {
        uint32_t dt = ((REG32(0xB8000100)>>18)&0x1)+3;
        return dt;
}

#ifdef CONFIG_SPI_NOR_FLASH
#include <nor_spi/nor_spif_register.h>
#define SNAFCFRrv REG32(0xB801A400)
inline static void sw_set_spif_div(void) {
        extern otto_soc_context_t otto_sc;
        RMOD_SFCR(spi_clk_div, otto_sc.spif_div);
        #ifdef CONFIG_SPI_NAND_FLASH
                uint32_t tmp = SNAFCFRrv;
                tmp &= ~(0x3F<<4); // bit 4~9
                tmp |= (otto_sc.snaf_pl<<8) | (otto_sc.snaf_div<<4);
                SNAFCFRrv = tmp;
        #endif
}
PATCH_REG(sw_set_spif_div, 2);
#endif //CONFIG_SPI_NOR_FLASH


int checkboard (void) {
        printf("Board: RTL9310, CPU:%dMHz, LX:%dMHz, DDR%d:%dMHz\n",
               GET_CPU_MHZ(),
               GET_LX_MHZ(),
               dram_type(),
               GET_MEM_MHZ());
        return 0;
}
