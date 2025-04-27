#include <soc.h>
#include <cg/cg.h>

#ifndef SECTION_CG
#define SECTION_CG 
#endif

static void reg_to_mhz(void);
cg_info_t cg_info_query; 

SECTION_CG
u32_t cg_query_freq(u32_t dev_type) {
	if (dev_type > 3) {
		return (u32_t)(-1);
	}
    switch(dev_type){
        case CG_DEV_OCP:
            return cg_info_query.dev_freq.ocp_mhz;
        case CG_DEV_MEM:
            return cg_info_query.dev_freq.mem_mhz;
        case CG_DEV_LX:
            return cg_info_query.dev_freq.lx_mhz;
        case CG_DEV_SPIF:
            return cg_info_query.dev_freq.spif_mhz;
        default:
            //Should not enter this case
            printf("Error: Should not enter this case!!!\n");
            return (u32_t)(-1);
    }
}

void cg_result_decode(void) {

	printf("II: OCP %dMHz, MEM %dMHz, LX %dMHz, SPIF %dMHz\n",
	       cg_info_query.dev_freq.ocp_mhz,
	       cg_info_query.dev_freq.mem_mhz,
	       cg_info_query.dev_freq.lx_mhz,
	       cg_info_query.dev_freq.spif_mhz);

	return;
}
REG_INIT_FUNC(cg_result_decode, 20);
