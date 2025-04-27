#ifndef __OTTO_SOC_CONTEXT_H__
#define __OTTO_SOC_CONTEXT_H__ 1

#include <cross_env.h>

typedef struct {
	cg_dev_freq_t cg_mhz;
        u16_t         spif_div;
        u16_t         snaf_div;
        u16_t         snaf_pl;
} otto_soc_context_t;

extern otto_soc_context_t otto_sc;

void osc_init(otto_soc_context_t *);
#endif
