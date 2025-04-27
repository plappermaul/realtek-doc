#include <preloader.h>
#include <plr_pll_gen3.h>

/* From soc.h, PLL_DEV_CPU == 0, PLL_DEV_LX == 1,
   PLL_DEV_MEM == 2, and PLL_DEV_DSP == 3 */
static u32_t _pll_freq[3] = {PLL_MHZ_UNKNOWN, PLL_MHZ_UNKNOWN, PLL_MHZ_UNKNOWN};

SECTION_ON_FLASH pll_result_t
pll_gen3_get_to_mhz(const pll_info_t *pll_reg,
                    pll_gen3_mhz_t *pll_mhz) {
	pll_gen3_info_t tmp;

	if (pll_reg==NULL) {
		tmp.pll_cpu_ctl0 = REG32(REG_PLL_CPU_CTRL0);
		tmp.pll_cpu_ctl1 = REG32(REG_PLL_CPU_CTRL1);
		tmp.pll_mem_ctl0 = REG32(REG_PLL_MEM_CTRL0);
		tmp.pll_mem_ctl1 = REG32(REG_PLL_MEM_CTRL1);
		pll_reg=&tmp;
	}

	return _pll_gen3_get_to_mhz(pll_reg, pll_mhz);
}

/* otto_pll_gen3_set_to_reg() may change freq. of NOR SPI-F,
   therefore it must *NOT* run on flash! */
void
otto_pll_gen3_set_to_reg(pll_info_t *ptr,
                         unsigned int flag) {
	if( flag & OTTO_PLL_CPU_SET ) {
		/* Step1. Switch CPU PLL to LX Clock. [Bit12] =0*/
		REG32(REG_PLL_GLB_CTRL) &= ~PLL_GLB_CTRL_CPU_PLL_SRC_CPU;

		/* Step2. Disable CPU PLL [Bit0] =0 */
		REG32(REG_PLL_GLB_CTRL) &= ~PLL_GLB_CTRL_CPU_PLL_EN_MASK;

		/* Step3. Assigne value to OCP PLL */
        REG32(REG_PLL_CPU_MISC_CTRL) = ptr->pll_cpu_misc_ctrl;
        REG32(REG_PLL_CPU_CTRL0)     = ptr->pll_cpu_ctl0;
        REG32(REG_PLL_CPU_CTRL1)     = ptr->pll_cpu_ctl1;

		/* Step4. Enable (Reset) CPU PLL, in order to make sure the ready bit is effective [Bit0] =1*/
		REG32(REG_PLL_GLB_CTRL) |= PLL_GLB_CTRL_CPU_PLL_EN_MASK;

	 	/* Step5. Need to delay for a wihle, and then polling CPU_PLL_READY until it's 1 [Bit8]=1*/  
	    while(!(REG32(REG_PLL_GLB_CTRL) & PLL_GLB_CTRL_OCP_PLL_RDY_MASK));    

	    /* Step6. Switch CPU Clock to CPU PLL clock. [Bit12] =1 */
	    REG32(REG_PLL_GLB_CTRL) |= PLL_GLB_CTRL_CPU_PLL_SRC_CPU;
    } 

	if( flag & OTTO_PLL_MEM_SET ) {
		/* Step1. Disable MEM PLL [Bit2]=0*/
		REG32(REG_PLL_GLB_CTRL) &= ~PLL_GLB_CTRL_MEM_PLL_EN_MASK;

		/* Step2. Assigne value to MEM PLL */
		REG32(REG_PLL_MEM_MISC_CTRL) = ptr->pll_mem_misc_ctrl;
		REG32(REG_PLL_MEM_CTRL0)     =  ptr->pll_mem_ctl0;  
		REG32(REG_PLL_MEM_CTRL1)     =  ptr->pll_mem_ctl1;

		/* Step3. Enable MEM PLL [Bit2]=1 */
		REG32(REG_PLL_GLB_CTRL) |= PLL_GLB_CTRL_MEM_PLL_EN_MASK;

	 	/* Step4. Need to delay for a wihle, and then polling MEM_PLL_READY until it's 1 [Bit10]=1*/  
	    while(!(REG32(REG_PLL_GLB_CTRL) & PLL_GLB_CTRL_MEM_PLL_RDY_MASK));    

	    /* Step5. Enable MEM Clock to DRAM chip is moved to dram calibration code */
    }

	_pll_freq[PLL_DEV_CPU] = PLL_MHZ_UNKNOWN;
	_pll_freq[PLL_DEV_LX]  = PLL_MHZ_UNKNOWN;
	_pll_freq[PLL_DEV_MEM] = PLL_MHZ_UNKNOWN;

    return;
}

#if (defined(PLR_ENABLE_PLL_SET) ||	\
     defined(CONFIG_STANDALONE_UBOOT))
SECTION_ON_FLASH pll_result_t
pll_gen3_set_from_mhz(pll_info_t *pll_reg,
                      pll_gen3_mhz_t *pll_mhz) {
	pll_result_t res;

	if (pll_reg == NULL) {
		pll_info_t tmp;
		res = _pll_gen3_set_from_mhz(&tmp, pll_mhz);
		otto_pll_gen3_set_to_reg(&tmp, (OTTO_PLL_CPU_SET|OTTO_PLL_MEM_SET));
	} else {
		res = _pll_gen3_set_from_mhz(pll_reg, pll_mhz);
	}

	return res;
}
#endif //if (defined(PLR_ENABLE_PLL_SET)||defined(CONFIG_STANDALONE_UBOOT)||defined(__OTTO_COMPOSER__))

SECTION_ON_FLASH void 
pll_gen3_setup(void) {
    const pll_info_t *pll_param_p;

    /* Retrive PLL register value */
    pll_param_p = &(parameters.soc.pll_info);

//#define PLL_GEN3_DBG
#ifdef PLL_GEN3_DBG
    printf("set_by=%d\n", pll_param_p->set_by);
    printf("pll_cpu_ctl0=0x%08x\n",  pll_param_p->pll_cpu_ctl0);
    printf("pll_cpu_ctl1=0x%08x\n",  pll_param_p->pll_cpu_ctl1);
    printf("pll_cpu_misc_ctrl=0x%08x\n",  pll_param_p->pll_cpu_misc_ctrl);
    printf("pll_mem_ctl0 =0x%08x\n",  pll_param_p->pll_mem_ctl0);
    printf("pll_mem_ctl1 =0x%08x\n",  pll_param_p->pll_mem_ctl1);
    printf("pll_mem_misc_ctrl =0x%08x\n",  pll_param_p->pll_mem_misc_ctrl);
#endif    

	if (pll_param_p->set_by == 1) {
		/* 1-software or 0-pin */
		otto_pll_gen3_set_to_reg((pll_info_t *)pll_param_p,(OTTO_PLL_CPU_SET|OTTO_PLL_MEM_SET));
        printf("\rII: PLL is set by SW... ");
	} else {
		/* PLL is set by HW strapped-pin */
        printf("\rPLL is set by HW pin... ");
    }
    
	pll_query_freq(PLL_DEV_CPU);
}

/* Returns in MHz. */
u32_t
pll_query_freq(u32_t dev) {
	if (_pll_freq[dev] == PLL_MHZ_UNKNOWN) {
		pll_gen3_mhz_t pll_mhz;
		pll_result_t res;

		res = pll_gen3_get_to_mhz(NULL, &pll_mhz);
		if (res == PLL_RES_OK) {
			_pll_freq[PLL_DEV_CPU] = pll_mhz.cpu;
			_pll_freq[PLL_DEV_LX]  = pll_mhz.lx;
			_pll_freq[PLL_DEV_MEM] = pll_mhz.mem;
		} else {
			printf("EE: %s fails: %d\n", __func__, res);
			while (1);
		}
	}

	return _pll_freq[dev];
}
