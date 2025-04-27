#include <soc.h>
#include <dram/memcntlr.h>
#include <util.h>

extern mc_info_t meminfo;

MEMCNTLR_SECTION
void mc_prj_dep_patch(void) {
#ifdef PROJECT_ON_FPGA
        if (3==meminfo.dram_param->dram_type)  {
	        ddr3_mr0 mr0 = { .v = meminfo.register_set->mr[0].f.mr_data };
                u32_t cl = mr0.f.cl_h+1;

                mr0.f.cl_h = cl;
                mr0.f.dll_reset = 1;
                meminfo.register_set->mr[0].f.mr_data = mr0.v;
            
                /* enable dynamic fifo reset */
                //meminfo.register_set->daccr.f.ac_dyn_bptr_clr_en = 1;
        }
        
#endif
	return;
}

REG_INIT_FUNC(mc_prj_dep_patch,          23);
