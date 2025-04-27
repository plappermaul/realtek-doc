#if defined(__LUNA_KERNEL__) || defined(CONFIG_UNDER_UBOOT)
//	#include "nor_spi_gen2/nor_spif_register.h"
	#include <nor_spi/nor_spif_register.h>
#else
	#include <nor_spi/nor_spif_register.h>
#endif

#ifndef SECTION_NOR_SPIF_GEN2_CORE
//#define SECTION_NOR_SPIF_GEN2_CORE __attribute__ ((section (".sram_text"), noinline))
#define SECTION_NOR_SPIF_GEN2_CORE
#endif

