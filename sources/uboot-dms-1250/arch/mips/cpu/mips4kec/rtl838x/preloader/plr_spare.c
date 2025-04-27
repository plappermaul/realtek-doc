/* $Revision: 1078 $ */
#ifdef __LUNA_KERNEL__
#include <kernel_soc.h>
#else
#include <plr_spare.h>
#endif

#ifdef USE_SOC_SPARE

void *
search_spare_by_type(u32_t type, u32_t *num_instances) {
    const spare_header_t *h=parameters.soc.spare_headers;
    while (h!=NULL) {
        if (h->type==type) {
            *num_instances=h->num_instances;
            return h->spare;
        } else if (h->type==SST_END) {
            return (void*)0;
        }
        ++h;
    }
    return (void*)0;
}

#ifdef SOC_NUM_DRAM_SPARE
/* return 0 for using the 1st dram model in soc.tcl,
   and vice versa. */
__attribute__((weak)) u32_t dram_model_select(void) {
	return 0;
}
#endif

#endif // USE_SOC_SPARE
