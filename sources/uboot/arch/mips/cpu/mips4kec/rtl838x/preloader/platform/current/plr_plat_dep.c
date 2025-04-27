#include <preloader.h>
#include <soc.h>
#include "bspchip.h"
#include "dram/plr_dram_gen2_memctl.h"
#include <plr_pll_gen3.h>

/*
 * Function declaration
 */

 /* CPU related (cache flush) */
void plat_memctl_dcache_flush(void);

 /* PAD related (Pad driving, static ZQ setting) */
void plat_memctl_dram_pad_drv(unsigned int drv_strength);
void plat_memctl_dram_zq_setting(unsigned int is_static, unsigned int odt_ocd_ohm);


 /* Platform clock related (CPU clock, LX bus clock, Memory clock) */
unsigned int plat_memctl_CPU_clock_MHz(void);
unsigned int plat_memctl_MEM_clock_MHz(void);
unsigned int plat_memctl_LX_clock_MHz(void);

 /* Memory controller input signal delay (CLKM delay, CLKM_90 delay, TX delay) */
unsigned int plat_memctl_input_sig_delay(unsigned int first_param, unsigned int second_param, unsigned int third_param);
void plat_memctl_choose_delay_value(unsigned int min_value, unsigned int max_value);
void plat_memctl_ZQ_model_en(void);
void plat_memctl_ZQ_force_config(void);


/* PAD Controller Registers & Settings */
#define PAD_STATIC_VALUE1 0x0032D256
#define PAD_STATIC_VALUE2 0x0000025B
#define DP_CR       (*((volatile unsigned int *)(0xB8003A00)))
#define DP_MD0IOC   (*((volatile unsigned int *)(0xB8003A04)))
#define DP_MD1IOC   (*((volatile unsigned int *)(0xB8003A08)))
#define DP_MD2IOC   (*((volatile unsigned int *)(0xB8003A0C)))
#define DP_MD3IOC   (*((volatile unsigned int *)(0xB8003A10)))
#define DP_LDQSIOC  (*((volatile unsigned int *)(0xB8003A14)))
#define DP_HDQSIOC  (*((volatile unsigned int *)(0xB8003A18)))
#define DP_LNDQSIOC (*((volatile unsigned int *)(0xB8003A1C)))
#define DP_HNDQSIOC (*((volatile unsigned int *)(0xB8003A20)))
#define DP_LDMIOC   (*((volatile unsigned int *)(0xB8003A24)))
#define DP_HDMIOC   (*((volatile unsigned int *)(0xB8003A28)))
#define DP_MCIOC    (*((volatile unsigned int *)(0xB8003A2C)))
#define DP_NMCIOC   (*((volatile unsigned int *)(0xB8003A30)))
#define DP_MAIOC    (*((volatile unsigned int *)(0xB8003A34)))
#define DP_MISCIOC  (*((volatile unsigned int *)(0xB8003A38)))


/*
 * Function implementation 
 */

void plat_memctl_ZQ_force_config(void)
{
	if(memctlc_is_DDR2())
	{
		DP_CR		= 0x3;
	}
	else if(memctlc_is_DDR3())
	{
		DP_CR		= 0xB;
	}
	DP_MD0IOC	= PAD_STATIC_VALUE1;
	DP_MD1IOC	= PAD_STATIC_VALUE1;
	DP_MD2IOC	= PAD_STATIC_VALUE1;
	DP_MD3IOC	= PAD_STATIC_VALUE1;
	DP_LDQSIOC	= PAD_STATIC_VALUE1;
	DP_HDQSIOC	= PAD_STATIC_VALUE1;
	DP_LNDQSIOC = PAD_STATIC_VALUE1;
	DP_HNDQSIOC = PAD_STATIC_VALUE1;
	DP_LDMIOC	= PAD_STATIC_VALUE2;
	DP_HDMIOC	= PAD_STATIC_VALUE2;
	DP_MCIOC	= PAD_STATIC_VALUE2;
	DP_NMCIOC	= PAD_STATIC_VALUE2;
	DP_MAIOC	= PAD_STATIC_VALUE2;
	DP_MISCIOC	= PAD_STATIC_VALUE2;
}
/*
 * setting clock reverse indication.
 * Can't run in DRAM.
 */
void plat_mem_clk_rev_check(void)
{
        unsigned int *clk_rev_ctl_reg;
        unsigned int clk_rev;
        unsigned int cpu_clk;
        unsigned int mem_clk;
        unsigned int lx_clk;

        clk_rev = 0;
        clk_rev_ctl_reg = (unsigned int *)PERIREG_CMUCTLR_REG;

        cpu_clk = plat_memctl_CPU_clock_MHz();
        mem_clk = plat_memctl_MEM_clock_MHz();
        lx_clk = plat_memctl_LX_clock_MHz();

        if(cpu_clk < mem_clk){
                clk_rev = (clk_rev | PERIREG_OCP_SMALLER_MASK);
    	}
        if(lx_clk < mem_clk){
                clk_rev = (clk_rev | PERIREG_LX_SMALLER_MASK);
        }
        *clk_rev_ctl_reg = (*clk_rev_ctl_reg & ~(PERIREG_OCP_SMALLER_MASK | PERIREG_LX_SMALLER_MASK)) | clk_rev ;

        return;
}

 /* CPU related (cache flush) */
void plat_memctl_dcache_flush(void)
{
	sram_parameters._dcache_writeback_invalidate_all();
    return;
}

 /* PAD related (Pad driving, static ZQ setting) */
void plat_memctl_dram_pad_drv(unsigned int drv_strength)
{
	 return;
}
void plat_memctl_dram_zq_setting(unsigned int is_static, unsigned int odt_ocd_ohm)
{
	return;
}

 /* Platform clock related (CPU clock, LX bus clock, Memory clock) */
unsigned int plat_memctl_CPU_clock_MHz(void)
{
    unsigned int cpu_clk;
    
    cpu_clk = pll_query_freq(PLL_DEV_CPU);

    return cpu_clk;
}
unsigned int plat_memctl_MEM_clock_MHz(void)
{
    unsigned int mem_clk;

    mem_clk = pll_query_freq(PLL_DEV_MEM);
    
    return mem_clk;

}
unsigned int plat_memctl_LX_clock_MHz(void)
{
    unsigned int lx_freq_mhz;

    lx_freq_mhz = pll_query_freq(PLL_DEV_LX);
    
    return  lx_freq_mhz;

}

 /* Memory controller input signal delay (CLKM delay, CLKM_90 delay, TX delay) */
unsigned int plat_memctl_input_sig_delay(unsigned int first_param, unsigned int second_param, unsigned int third_param)
{
    unsigned int target_dcdr_value;
    unsigned int clkm90, clkm, tx_clk;
    volatile unsigned int delay_loop;
    
    clkm = first_param;
    tx_clk = third_param;
    clkm90 = second_param;

    target_dcdr_value = (clkm90 << 17 | clkm << 22 | tx_clk<<27 );

    REG32(REG_MEM_CTRL_DCDR) = target_dcdr_value;
    while( REG32(REG_MEM_CTRL_DCDR) != target_dcdr_value ){
        _memctl_debug_printf_E("error: ddcr not equal write data(0x%08x)\n", target_dcdr_value);
        REG32(REG_MEM_CTRL_DCDR) = target_dcdr_value;
    }

    /* Delay a little bit for waiting for more stable of the DRAM clock.*/
    delay_loop = 0x10000;
    while(delay_loop--);

    return 0;
}


void plat_memctl_choose_delay_value(unsigned int min_value, unsigned int max_value)
{
    unsigned int target_value;

    if(min_value != MEMCTL_INVALID_CLKM_DELAY_VALUE){

        target_value = min_value +(((max_value - min_value)) / 2);
    }else{
        target_value = 0;
    }
    plat_memctl_input_sig_delay(target_value, 0, 0);
}

void plat_memctl_dramclk_en(void)
{ 
    volatile unsigned int *sysreg_dram_clk_en_reg;
    volatile unsigned int delay_loop;

    sysreg_dram_clk_en_reg = (volatile unsigned int *)REG_PLL_GLB_CTRL;
    while((*sysreg_dram_clk_en_reg&PLL_GLB_CTRL_MEM_CHIP_CLK) != PLL_GLB_CTRL_MEM_CHIP_CLK ){
        *sysreg_dram_clk_en_reg |= PLL_GLB_CTRL_MEM_CHIP_CLK;
    }

    /* Delay a little bit for waiting for more stable of the DRAM clock.*/
    delay_loop = 0x10000;
    while(delay_loop--);
}

void plat_memctl_IO_PAD_patch(void)
{
	return;
}

void plat_memctl_show_dram_config(void)
{
    unsigned int i;
    volatile unsigned int *phy_reg;
    _memctl_debug_printf_I("\n");
    _memctl_debug_printf_I("MCR  (0x%08x):0x%08x, 0x%08x, 0x%08x, 0x%08x\n",MCR, REG32(MCR), REG32(DCR), REG32(DTR0), REG32(DTR1));
    _memctl_debug_printf_I("DTR2 (0x%08x):0x%08x\n", DTR2, REG32(DTR2));
    _memctl_debug_printf_I("DIDER(0x%08x):0x%08x\n", DIDER,REG32(DIDER)),
	_memctl_debug_printf_I("DCDR (0x%08x):0x%08x\n", DCDR, REG32(DCDR));
	_memctl_debug_printf_I("ZQPCR(0x%08x):0x%08x, 0x%08x, 0x%08x\n",DDZQPR, REG32(DDZQPR), REG32(DDZQPCR), REG32(DDZQPCR));
    _memctl_debug_printf_I("PHY Registers(0x%08x):\n", DACCR);
    phy_reg = (volatile unsigned int *)DACCR;
    for(i=0;i<11;i++){
        _memctl_debug_printf_I("0x%08x:0x%08x, 0x%08x, 0x%08x, 0x%08x\n", \
                phy_reg, *(phy_reg), *(phy_reg+1), *(phy_reg+2), *(phy_reg+3) );
        phy_reg+=4;
    }
}

u32_t plat_memctl_calculate_dqrf_delay(u32_t max_w_seq_start, u32_t max_w_len, u32_t max_r_seq_start, u32_t max_r_len) 
{
    u32_t ret_val;

    ret_val = (((max_w_seq_start + (max_w_len/3)) & 0x1f) << 24) | 
              (((max_r_seq_start + max_r_len - 1) & 0x1f) << 16) | 
              (((max_r_seq_start + (max_r_len/2)) & 0x1f) << 8) | 
              (((max_r_seq_start) & 0x1f) << 0);

    return ret_val;
}


