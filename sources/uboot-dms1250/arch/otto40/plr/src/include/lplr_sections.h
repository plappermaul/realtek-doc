#ifndef LPLR_SECTIONS_H
#define LPLR_SECTIONS_H
#include <soc.h>

#define SECTION_SPI_NAND_PROBE_FUNC __attribute__ ((section (".spi_nand_probe_func")))
#define REG_SPI_NAND_PROBE_FUNC(fn) spi_nand_probe_t* __nspf_ ## ## fn ## _ \
        SECTION_SPI_NAND_PROBE_FUNC = (spi_nand_probe_t*) fn

#ifndef SECTION_RO
    #define SECTION_RO          __attribute__ ((section (".ro")))
#endif

#ifndef SECTION_SOC_STRU
    #define SECTION_SOC_STRU    __attribute__ ((section (".soc_stru")))
#endif

#ifndef SECTION_SHORT_INIT
    #define SECTION_SHORT_INIT  __attribute__ ((section (".short_init")))
#endif

#endif //LPLR_SECTIONS_H

