#ifndef _OTTO_PLL_H_
#define _OTTO_PLL_H_
#include <soc.h>
#include <pblr.h>

#define board_CPU_freq_mhz()  parameters._pll_query_freq(PLL_DEV_CPU)
#define board_LX_freq_mhz()   parameters._pll_query_freq(PLL_DEV_LX)
#define board_DRAM_freq_mhz() parameters._pll_query_freq(PLL_DEV_MEM)
#define board_DSP_freq_mhz()  parameters._pll_query_freq(PLL_DEV_DSP)

#endif
