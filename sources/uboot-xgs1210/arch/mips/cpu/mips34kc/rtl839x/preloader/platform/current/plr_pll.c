#include <preloader.h>
#include <cpu_utils.h>
#include <plr_pll_gen2.h>
#include "bspchip.h"

unsigned int plat_memctl_LX_clock_MHz(void);
unsigned int plat_memctl_CPU_clock_MHz(void);
unsigned int plat_memctl_MEM_clock_MHz(void);

#define ABS(x) ((x + 0xffffffff) ^ 0xffffffff)

void pll_setup(void) {
	parameters._pll_query_freq = pll_query_freq;
	parameters._udelay = udelay;
	pll_gen2_setup();

	/* OCP, LX and Mem Freq slower check */
	unsigned int reg_val;
	unsigned int lx_clk  = plat_memctl_LX_clock_MHz();
	unsigned int cpu_clk = plat_memctl_CPU_clock_MHz();
	unsigned int mem_clk = plat_memctl_MEM_clock_MHz();

	reg_val = REG32(SYS_LX_CLK_FRQ_SLOWER_REG);
	if(cpu_clk <= mem_clk) {
        reg_val |= SYS_LX_CLK_FRQ_SLOWER_REG_OCP_CLK_SLOW_MASK;
    } else {
		reg_val &= ~SYS_LX_CLK_FRQ_SLOWER_REG_OCP_CLK_SLOW_MASK;
    }
	if(lx_clk <= mem_clk) {
       	/* Enable LX slower bit and LX jitter tolerance. */
		reg_val |= SYS_LX_CLK_FRQ_SLOWER_REG_LX_CLK_SLOW_MASK;
        REG32(0xb8001004) = REG32(0xb8001004) | 0x80000000;
	} else {
		reg_val &= ~SYS_LX_CLK_FRQ_SLOWER_REG_LX_CLK_SLOW_MASK;
	}
	REG32(SYS_LX_CLK_FRQ_SLOWER_REG) = reg_val;

	return;
}

void pll_setup_info(void) {

    const pll_info_t *pll_param_p;

    /* Retrive PLL register value */
    pll_param_p = &(parameters.soc.pll_info);

	/* 1-software or 0-pin */
	printf("\rII: PLL is set by %s... OK\n", (pll_param_p->set_by)?"SW":"HW pin");


 	/* User might want to enable LX jitter tolerance when freq. of LX and MEM are close.
	   Current threshold is 10%.
	   For the sake of simplicity, here just add 1 instead of rounding. */

	if (ABS(pll_query_freq(PLL_DEV_MEM) - pll_query_freq(PLL_DEV_LX)) <
	    ((pll_query_freq(PLL_DEV_MEM) / 10) + 1)) {
		printf("II: Frequencies of PLL_DEV_MEM and PLL_DEV_LX are close.\n");
	}
	return;
}
