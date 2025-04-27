#include <common.h>
#include <asm/arch/bspchip.h>
#include <asm/arch/memctl.h>
#include <asm/arch/sramctl.h>


unsigned int dramtype_freq_25oc[4][8] = {
						{100, 375, 225, 187, 100, 375, 225, 100},
						{150, 350, 225, 200, 150, 350, 225, 125},
						{187, 300, 250, 175, 187, 300, 250, 150},
						{200, 400, 300, 162, 200, 400, 300, 400},
					};
unsigned int dramtype_freq_40oc[4][8] = {
						{100, 380, 220, 190, 100, 380, 220, 100},
						{150, 340, 220, 200, 150, 340, 220, 125},
						{190, 300, 250, 175, 190, 300, 250, 150},
						{200, 400, 300, 160, 200, 400, 300, 400},
					};

  					/* 50,  75, 100, 125, 150, 175, 200, 225, 250, 275, 300, 325, 350, 375, 400 */
unsigned int sys_clk_control_dram25[] = { 0x4, 0x0, 0x4, 0x8, 0x0, 0x3, 0x4, 0x6, 0x8, 0x8, 0x0, 0x1, 0x2, 0x3, 0x4 };
unsigned int sys_clk_control_dram40[] = { 0x5, 0x0, 0x5, 0xa, 0x0, 0x4, 0x5, 0x7, 0xa, 0xa, 0x0, 0x1, 0x2, 0x4, 0x5 };
unsigned int sys_clk_mckg_phase90[]   = { 0x3, 0x2, 0x2, 0x2, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x2, 0x2, 0x2, 0x2, 0x2 };
unsigned int sys_clk_mckg_clk_div[]   = { 0x3, 0x2, 0x2, 0x2, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0 };


unsigned int _is_CKSEL_25MHz(void)
{
	if(REG(SYSREG_PIN_STATUS_REG) & SYSREG_PIN_STATUS_CLSEL_MASK)
		return 0;
	else
		return 1;
}


unsigned int sys_get_dramtype_freq_strapping(void)
{
	volatile unsigned int *bonding_reg;
	volatile unsigned int *strapping_reg;
	unsigned int b_sel, s_sel;

	bonding_reg = (volatile unsigned int *)BONDING_REG;
	strapping_reg = (volatile unsigned int *)STRAPING_REG;
	b_sel = (*bonding_reg & BONDING_REG_DRAMTYPE_FREQ43_MASK) >> BONDING_REG_DRAMTYPE_FREQ43_FD_S;
	s_sel = (*strapping_reg & STRAPING_REG_DRAMTYPE_FREQ210_MASK) >> STRAPING_REG_DRAMTYPE_FREQ210_FD_S;
 
	if(_is_CKSEL_25MHz()){
		return dramtype_freq_25oc[b_sel][s_sel];
	}else{
		return dramtype_freq_40oc[b_sel][s_sel];
	}
}

int sys_adj_clk(unsigned int cpu_clk, unsigned int dsp_clk, unsigned int mem_clk)
{
	unsigned int cpu_field;
	unsigned int dsp_field;
	unsigned int mem_field;
	unsigned int sys_pll_ctl_value;


	if(cpu_clk<500)
		cpu_clk = 500;
	if(cpu_clk>700)
		cpu_clk = 700;

	if(dsp_clk<500)
		dsp_clk = 500;
	if(dsp_clk>550)
		dsp_clk = 550;

	if(mem_clk<50)
		mem_clk = 50;
	if(mem_clk >400)
		mem_clk = 400;

	if(_is_CKSEL_25MHz()){
		mem_field = sys_clk_control_dram25[ ((mem_clk-50)/25) ];
		dsp_field = (dsp_clk/25)-20;
		cpu_field = (cpu_clk/25)-20;
	}else{
		mem_field = sys_clk_control_dram40[ ((mem_clk-50)/25) ];
		dsp_field = (dsp_clk/20)-25;
		cpu_field = (cpu_clk/20)-25;
	}

	sys_pll_ctl_value = (cpu_field << 16) | (dsp_field << 8) | mem_field;
	
	sram_mapping(0, 0xa0000000, 0x30000, SRAM_SIZE_32KB);
	memctl_unmapping(0, 0xa0000000, SRAM_SIZE_32KB);
	sys_adj_mem_clk( sys_pll_ctl_value, sys_clk_mckg_phase90[((mem_clk-50)/25)],\
			 sys_clk_mckg_clk_div[((mem_clk-50)/25)], 0xa0000000);
	sram_unmapping(0);
	memctl_unmapping_disable(0);

	return 0;
}


/* Function Name: 
 * 	sys_DSP_freq_mhz
 * Descripton:
 *	Get the current DSP frequency in MHz.
 * Input:
 *	None
 * Output:
 * 	None
 * Return:
 *  	DSP frequncy in MHz.
 */
unsigned int sys_DSP_freq_mhz(void)
{
//#ifdef CONFIG_SOFTWARE_OVERWRITE_FREQ
#if 1
	volatile unsigned int *sysclk_contr_reg;
	unsigned int dsp_clk;

	sysclk_contr_reg = (volatile unsigned int *)SYSREG_SYSCLK_CONTROL_REG;
	dsp_clk = (*sysclk_contr_reg & SYSREG_SYSCLK_CONTROL_OCP1PLL_MASK) \
		>> SYSREG_SYSCLK_CONTROL_OCP1PLL_FD_S;
	if(_is_CKSEL_25MHz()){
                printf("dsp_clk=%d\n", dsp_clk);
		dsp_clk = (dsp_clk + 20) * 25;
	}else{
		dsp_clk = (dsp_clk + 25) * 20;
	}

	return dsp_clk;
#else
	return CONFIG_DSPCLK_MHZ;
#endif

}



/* Function Name: 
 * 	sys_CPU_freq_mhz
 * Descripton:
 *	Get the current CPU frequency in MHz.
 * Input:
 *	None
 * Output:
 * 	None
 * Return:
 *  	CPU frequncy in MHz.
 */
unsigned int sys_CPU_freq_mhz(void)
{
//#ifdef CONFIG_SOFTWARE_OVERWRITE_FREQ
#if 1
	volatile unsigned int *sysclk_contr_reg;
	unsigned int cpu_clk;

	sysclk_contr_reg = (volatile unsigned int *)SYSREG_SYSCLK_CONTROL_REG;
	cpu_clk = (*sysclk_contr_reg & SYSREG_SYSCLK_CONTROL_OCP0PLL_MASK) \
		>> SYSREG_SYSCLK_CONTROL_OCP0PLL_FD_S;
	if(_is_CKSEL_25MHz()){
		cpu_clk = (cpu_clk + 20) * 25;
	}else{
		cpu_clk = (cpu_clk + 25) * 20;
	}

	return cpu_clk;
#else
	return CONFIG_CPUCLK_MHZ;
#endif
}



/* Function Name: 
 * 	sys_LX_freq_mhz
 * Descripton:
 *	Get the current LX bus frequency in MHz.
 * Input:
 *	None
 * Output:
 * 	None
 * Return:
 *  	LX bus frequncy in MHz.
 */
unsigned int sys_LX_freq_mhz(void)
{
//#ifdef CONFIG_SOFTWARE_OVERWRITE_FREQ
#if 1
	unsigned int lx_clk_div;
	unsigned int lx_clk_div_offset;
	unsigned int lx_freq_mhz;
	unsigned int src_oc_mhz;

	/* HW reset :apply defaul value into it*/
	if(REG(SYSREG_LX_PLL_SEL_REG) == 0){
		if(_is_CKSEL_25MHz()){
			REG(SYSREG_LX_PLL_SEL_REG) = 8; /* 200MHz */
		}else{
			REG(SYSREG_LX_PLL_SEL_REG) = 10;/* 200MHz */
		}
		/* Delay a littel bit time for PLL */
		
		return SYSREG_LX_DEFAULT_MHZ;
	}else{

		if(_is_CKSEL_25MHz()){
			src_oc_mhz = 25;
			lx_clk_div_offset = 6;
		}else{
			src_oc_mhz = 40 / 2;
			lx_clk_div_offset = 8;
		}

		lx_clk_div = REG(SYSREG_LX_PLL_SEL_REG) + lx_clk_div_offset;
		lx_freq_mhz = (src_oc_mhz * (lx_clk_div + 2))/2;
	}
	return  lx_freq_mhz;
#else
	return CONFIG_LXBUS_MHZ;
#endif
}

/* Function Name: 
 * 	sys_DRAM_freq_mhz
 * Descripton:
 *	Get the current DRAM frequency in MHz.
 * Input:
 *	None
 * Output:
 * 	None
 * Return:
 *  	DRAM Frequncy in MHz.
 */
unsigned int sys_DRAM_freq_mhz(void)
{
//#ifdef CONFIG_SOFTWARE_OVERWRITE_FREQ
#if 1
	volatile unsigned int *sysclk_contr_reg;
	volatile unsigned int *sysclk_mckg_div_reg;
	unsigned int mem_clk, clk_divison;

	sysclk_contr_reg = (volatile unsigned int *)SYSREG_SYSCLK_CONTROL_REG;
	sysclk_mckg_div_reg = (volatile unsigned int *)SYSREG_MCKG_FREQ_DIV_REG;
	clk_divison = *sysclk_mckg_div_reg;
	mem_clk = (*sysclk_contr_reg & SYSREG_SYSCLK_CONTROL_SDPLL_MASK) \
		>> SYSREG_SYSCLK_CONTROL_SDPLL_FD_S;

	if( _is_CKSEL_25MHz() ){
		mem_clk = (mem_clk + 12) * 25;
	}else{
		mem_clk = (mem_clk + 15) * 20;
	}

	while(clk_divison){
		mem_clk = mem_clk / 2;
		clk_divison--;
	}

	return mem_clk;
#else
	return sys_get_dramtype_freq_strapping();
#endif
}

