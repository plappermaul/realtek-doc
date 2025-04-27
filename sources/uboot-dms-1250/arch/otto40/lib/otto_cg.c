#if 0
#include <soc.h>
#include <asm/symb_define.h>

u32_t otto_cg_query_freq(u32_t dev_type) {
	const symbol_table_entry_t *ste;
	static fpu32_u32_t *_cg_query_freq = VZERO;

	if (_cg_query_freq == VZERO) {
		ste = symb_retrive(SF_SYS_QUERY_FREQ, _SYM_HEADER, _SYM_END);
		if (ste) {
			_cg_query_freq = (fpu32_u32_t *)ste->v.pvalue;
		} else {
			return 0;
		}
	}

	return _cg_query_freq(dev_type);
};
#else
#include <common.h>
#include <osc.h>

#ifndef SECTION_CLOCK_GEN
        #define SECTION_CLOCK_GEN
#endif

SECTION_CLOCK_GEN
u32_t otto_cg_query_freq(u32_t dev_type) {
        return CG_QUERY_FREQUENCY(dev_type, ((cg_dev_freq_t *)(&otto_sc.cg_mhz)));
};

#endif
