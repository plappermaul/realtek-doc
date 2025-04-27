#ifndef __CG_HEADER__
#define __CG_HEADER__

typedef struct {
	s16_t ocp_mhz;
	s16_t mem_mhz;
	s16_t lx_mhz;
    s16_t spif_mhz;
} cg_dev_freq_t;

typedef struct {
	cg_dev_freq_t     dev_freq;
} cg_info_t;

extern u32_t cg_query_freq(u32_t dev_type);
void cg_result_decode(void);

extern cg_info_t cg_info_query;


#endif
