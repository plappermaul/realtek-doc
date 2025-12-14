#include <osc.h>
#include <cg_dev_freq.h>

#if defined(CG_QUERY_FREQUENCY)
#else
#	error "EE: missing CG_QUERY_FREQUENCY"
#endif

u32_t cg_query_freq(u32_t dev_type) {
	const cg_dev_freq_t *cg = &otto_sc.cg_mhz;
	return CG_QUERY_FREQUENCY(dev_type, cg);
}
