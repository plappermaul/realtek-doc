#include <preloader.h>
#include <plr_pll_gen2.h>

#define PLL_CTRL0_BASE 0xBB000028
#define PLL_CTRL1_BASE 0xBB00002C

/* From soc.h, PLL_DEV_CPU == 0, PLL_DEV_LX == 1, PLL_DEV_MEM == 2 */
static u32_t pll_freq[3] = {PLL_MHZ_UNKNOWN, PLL_MHZ_UNKNOWN, PLL_MHZ_UNKNOWN};

void otto_pll_gen2_set(unsigned int sys_cpu_pll_ctl0,
                       unsigned int sys_cpu_pll_ctl1,
                       unsigned int sys_mem_pll_ctl0,
                       unsigned int sys_mem_pll_ctl1,
                       unsigned int sys_lx_pll_ctl0,
                       unsigned int sys_lx_pll_ctl1,
                       unsigned int flag) {
	volatile unsigned int wait_loop_cnt;
	//unsigned int target_sys_pll_ctl;

	/*
	 * Note: We assume it's running in the SRAM on OCP0 bus now.
	 */

	wait_loop_cnt = 0x100000;	// 1.49 ms

	/* Switch CPU/MEM/LX Clock to Fixed clock */
	REG32(PLL_GLB_CTRL_A) = REG32(PLL_GLB_CTRL_A) | (PLL_GLB_CTRL_LXB_CLKSEL_FIXED_MASK | \
	                                                 PLL_GLB_CTRL_MEM_CLKSEL_FIXED_MASK | \
	                                                 PLL_GLB_CTRL_CPU_CLKSEL_FIXED_MASK);
	while(wait_loop_cnt--);


	/* Assigned value to PLL */

	if( flag & OTTO_PLL_CPU_SET ){
		REG32(CPU_PLL_CTRL0_A) = sys_cpu_pll_ctl0;
		REG32(CPU_PLL_CTRL1_A) = sys_cpu_pll_ctl1;
	}

	if( flag & OTTO_PLL_LX_SET ){
		REG32(LX_PLL_CTRL0_A) = sys_lx_pll_ctl0;
		REG32(LX_PLL_CTRL1_A) = sys_lx_pll_ctl1;
	}

	if( flag & OTTO_PLL_MEM_SET ){
		REG32(MEM_PLL_CTRL0_A) = sys_mem_pll_ctl0;
		REG32(MEM_PLL_CTRL1_A) = sys_mem_pll_ctl1;
	}

	/* waiting for PLL */
	wait_loop_cnt = 0x100000;
	while(wait_loop_cnt--);


	/* Switch CPU/MEM/LX Clock to CPU/MEM/LX PLL clock */
	wait_loop_cnt = 0x100000;
	REG32(PLL_GLB_CTRL_A) = REG32(PLL_GLB_CTRL_A) & (~(PLL_GLB_CTRL_LXB_CLKSEL_FIXED_MASK | \
	                                                   PLL_GLB_CTRL_MEM_CLKSEL_FIXED_MASK | \
	                                                   PLL_GLB_CTRL_CPU_CLKSEL_FIXED_MASK));

	while(wait_loop_cnt--);

	pll_freq[PLL_DEV_CPU] = PLL_MHZ_UNKNOWN;
	pll_freq[PLL_DEV_LX]  = PLL_MHZ_UNKNOWN;
	pll_freq[PLL_DEV_MEM] = PLL_MHZ_UNKNOWN;

	return;
}

pll_result_t
pll_gen2_get_to_mhz(const pll_info_t *pll_reg,
                    pll_gen2_mhz_t *pll_mhz) {
	pll_info_t tmp;

	if (pll_reg == NULL) {
		tmp.sys_cpu_pll_ctl0 = *((volatile u32_t *)(PLL_CTRL0_BASE+PLL_DEV_CPU*0x10));
		tmp.sys_cpu_pll_ctl1 = *((volatile u32_t *)(PLL_CTRL1_BASE+PLL_DEV_CPU*0x10));
		tmp.sys_lx_pll_ctl0  = *((volatile u32_t *)(PLL_CTRL0_BASE+PLL_DEV_LX*0x10));
		tmp.sys_lx_pll_ctl1  = *((volatile u32_t *)(PLL_CTRL1_BASE+PLL_DEV_LX*0x10));
		tmp.sys_mem_pll_ctl0 = *((volatile u32_t *)(PLL_CTRL0_BASE+PLL_DEV_MEM*0x10));
		tmp.sys_mem_pll_ctl1 = *((volatile u32_t *)(PLL_CTRL1_BASE+PLL_DEV_MEM*0x10));
		pll_reg = &tmp;
	}

	return _pll_gen2_get_to_mhz(pll_reg, pll_mhz);
}

#if (defined(PLR_ENABLE_PLL_SET) ||	\
     defined(CONFIG_STANDALONE_UBOOT))
pll_result_t
pll_gen2_set_from_mhz(pll_info_t *pll_reg,
                      pll_gen2_mhz_t *pll_mhz) {
	pll_result_t res;

	if (pll_reg == NULL) {
		pll_info_t tmp;
		res = _pll_gen2_set_from_mhz(&tmp, pll_mhz);
		otto_pll_gen2_set(tmp.sys_cpu_pll_ctl0,
		                  tmp.sys_cpu_pll_ctl1,
		                  tmp.sys_mem_pll_ctl0,
		                  tmp.sys_mem_pll_ctl1,
		                  tmp.sys_lx_pll_ctl0,
		                  tmp.sys_lx_pll_ctl1,
		                  (OTTO_PLL_CPU_SET | OTTO_PLL_MEM_SET | OTTO_PLL_LX_SET));
	} else {
		res = _pll_gen2_set_from_mhz(pll_reg, pll_mhz);
	}

	return res;
}
#endif

void pll_gen2_setup(void) {
	const pll_info_t *pll_param_p;

	/* Retrive PLL register value */
	pll_param_p = &(parameters.soc.pll_info);

	if(pll_param_p->set_by == 1) { /* 1-software or 0-pin */
		otto_pll_gen2_set(pll_param_p->sys_cpu_pll_ctl0,
		                  pll_param_p->sys_cpu_pll_ctl1,
		                  pll_param_p->sys_mem_pll_ctl0,
		                  pll_param_p->sys_mem_pll_ctl1,
		                  pll_param_p->sys_lx_pll_ctl0,
		                  pll_param_p->sys_lx_pll_ctl1,
		                  (OTTO_PLL_CPU_SET | OTTO_PLL_MEM_SET | OTTO_PLL_LX_SET));
	} 

	pll_query_freq(PLL_DEV_CPU);

	return;
}

/* Returns in MHz. */
u32_t pll_query_freq(u32_t dev) {
	if (PLL_MHZ_UNKNOWN == pll_freq[dev]) {
		pll_gen2_mhz_t pll_mhz;
		pll_result_t res;

		res = pll_gen2_get_to_mhz(NULL, &pll_mhz);

		if (res == PLL_RES_OK) {
			pll_freq[PLL_DEV_CPU] = pll_mhz.cpu;
			pll_freq[PLL_DEV_LX]  = pll_mhz.lx;
			pll_freq[PLL_DEV_MEM] = pll_mhz.mem;
		}
	}

	return pll_freq[dev];
}
