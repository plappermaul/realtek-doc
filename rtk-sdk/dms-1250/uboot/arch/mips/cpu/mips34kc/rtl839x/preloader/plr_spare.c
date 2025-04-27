/* $Revision: 1400 $ */
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
#define DRAMI (parameters.soc.dram_info)
/* return 0 for using the 1st dram model in soc.tcl,
   and vice versa. */
__attribute__((weak)) u32_t dram_model_select(void) {
	dram_info_t *di;
	u32_t sel=0, di_num=0;
    u32_t ddrType = (*((volatile u32_t *)(0xB8001000))&0xF0000000);

    // check DRAM info in SRAM first
    if (ddrType != ((DRAMI.mcr)&0xF0000000)) {

       	di = get_dram_spare(&di_num);
        // check spare DRAM info in flash sequentially
        while (sel<di_num) {
            if(ddrType == ((((dram_info_t *)(di))->mcr)&0xF0000000) ) {
                return (sel+1);	// 0 means default entry, 1~x means spare entries
            }
            sel++;
            di++;
        }
    }
    return 0;
}
#endif

#endif // USE_SOC_SPARE
