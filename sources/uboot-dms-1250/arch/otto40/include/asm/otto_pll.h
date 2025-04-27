#ifndef _OTTO_PLL_H_
#define _OTTO_PLL_H_
#include <soc.h>

extern u32_t otto_cg_query_freq(u32_t dev_type);

#define board_CPU_freq_mhz()  otto_cg_query_freq(CG_DEV_OCP)
#define board_LX_freq_mhz()   otto_cg_query_freq(CG_DEV_LX)
#define board_DRAM_freq_mhz() otto_cg_query_freq(CG_DEV_MEM)

#endif
