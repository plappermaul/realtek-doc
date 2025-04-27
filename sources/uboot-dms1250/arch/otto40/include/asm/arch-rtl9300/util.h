//#include <configs/autoconf.h>
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

#define otto_lx_timer_udelay    udelay
