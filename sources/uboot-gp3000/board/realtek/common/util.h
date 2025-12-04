#include <cross_env.h>
//#include <asm/otto_timer.h>

#define SECTION_SPI_NAND_PROBE_FUNC __attribute__ ((section (".spi_nand_probe_func")))

#define REG_SPI_NAND_PROBE_FUNC(fn) spi_nand_probe_t* __nspf_ ## ## fn ## _ \
        SECTION_SPI_NAND_PROBE_FUNC = (spi_nand_probe_t*) fn

#define _plr_soc_t              (*(soc_t *)(OTTO_SRAM_START+OTTO_HEADER_OFFSET))
#define _lplr_basic_io          _plr_soc_t.bios

#ifndef inline_memcpy
#define inline_memcpy           memcpy
#endif

#ifndef otto_lx_timer_udelay
#define otto_lx_timer_udelay    udelay
#endif

#ifndef OTTO_SRAM_START
#define OTTO_SRAM_START         (0x9F000000)
#endif

#ifndef OTTO_HEADER_OFFSET
#define OTTO_HEADER_OFFSET      (0x20)
#endif
