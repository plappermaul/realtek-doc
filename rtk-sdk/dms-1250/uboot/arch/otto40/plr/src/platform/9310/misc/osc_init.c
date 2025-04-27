#include <osc.h>

otto_soc_context_t otto_sc SECTION_SDATA;

void osc_init(otto_soc_context_t *otto_sc) {
	memcpy((void *)&otto_sc->cg_mhz, (void *)&cg_info_query.dev_freq, sizeof(otto_sc->cg_mhz));
        if(0==otto_sc->memsize) {
            otto_sc->memsize = 70; // assign a stange number
        }
	return;
}
