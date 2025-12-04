#include <osc.h>

#ifndef SECTION_DATA
#	define SECTION_DATA __attribute__ ((section (".data")))
#endif

/* otto_sc will be accessed before .bss section reset by relocate_code().
	 Therefore it must be on .data section to prevent from resetting to 0. */
SECTION_DATA otto_soc_context_t otto_sc;

void osc_init(otto_soc_context_t *src) {
	memcpy(&otto_sc, src, sizeof(otto_soc_context_t));
	return;
}
