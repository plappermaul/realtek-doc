#include <osc.h>

void osc_init(otto_soc_context_t *otto_sc) {
	memcpy((void *)&otto_sc->cg_mhz, (void *)&cg_info_query.dev_freq, sizeof(otto_sc->cg_mhz));
	return;
}
