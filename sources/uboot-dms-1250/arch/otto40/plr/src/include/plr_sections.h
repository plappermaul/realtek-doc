#ifndef PLR_SECTION_H
#define PLR_SECTION_H

#include <soc.h>


#define SECTION_SPI_NAND_PROBE_FUNC __attribute__ ((section (".spi_nand_probe_func")))

#define REG_SPI_NAND_PROBE_FUNC(fn) spi_nand_probe_t* __nspf_ ## ## fn ## _ \
	SECTION_SPI_NAND_PROBE_FUNC = (spi_nand_probe_t*) fn 


#define SECTION_ONFI_PROBE_FUNC __attribute__ ((section (".onfi_probe_func")))

#define REG_ONFI_PROBE_FUNC(fn) onfi_probe_t* __ofnf_ ## ## fn ## _ \
	SECTION_ONFI_PROBE_FUNC = (onfi_probe_t*) fn 


#ifndef SECTION_RO
    #define SECTION_RO          __attribute__ ((section (".ro")))
#endif

#ifndef SECTION_TEXT
    #define SECTION_TEXT        __attribute__ ((section (".text")))
#endif

#ifndef SECTION_UNS_RO
    #define SECTION_UNS_RO      __attribute__ ((section (".unswapable_rodata")))
#endif

#ifndef SECTION_UNS_TEXT
    #define SECTION_UNS_TEXT    __attribute__ ((section (".unswapable_text")))
#endif

#ifndef SECTION_DATA
    #define SECTION_DATA        __attribute__ ((section (".data")))
#endif

#ifndef SECTION_SDATA
    #define SECTION_SDATA       __attribute__ ((section (".sdata")))
#endif

#ifndef SECTION_BSS
    #define SECTION_BSS         __attribute__ ((section (".bss")))
#endif

#ifndef SECTION_SBSS
    #define SECTION_SBSS        __attribute__ ((section (".sbss")))
#endif

#ifndef SECTION_RECYCLE
    #define SECTION_RECYCLE     __attribute__ ((section (".recycle_text")))
#endif

#ifndef SECTION_RECYCLE_DATA
    #define SECTION_RECYCLE_DATA    __attribute__ ((section (".recycle_data")))
#endif

#ifndef SECTION_SYM_TABLE
    #define SECTION_SYM_TABLE   __attribute__ ((section (".symbol_table")))
#endif

#ifndef SECTION_RESET_ENTRY
    #define SECTION_RESET_ENTRY __attribute__ ((section (".reset_entry")))
#endif

#ifndef SECTION_TLB_ISR
    #define SECTION_TLB_ISR     __attribute__ ((section (".isr_entry_text")))
#endif

#ifndef SECTION_SOC_STRU
    #define SECTION_SOC_STRU    __attribute__ ((section (".sdata.soc_stru")))
#endif

#ifndef SECTION_PARAMETERS
    // the parameters (variables) can be saved by preloader-CLI
    #define SECTION_PARAMETERS    __attribute__ ((section (".sdata.parameters")))
#endif
#ifndef SECTION_SRAM_TEXT
    #define SECTION_SRAM_TEXT   __attribute__ ((section (".sram_text")))
#endif


#endif //PLR_SECTION_H
