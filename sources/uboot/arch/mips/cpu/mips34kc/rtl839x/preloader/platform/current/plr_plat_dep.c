#include <preloader.h>
#include "bspchip.h"
#include "dram/plr_dram_gen2.h"
#include "plr_pll_gen2.h"
//#include "memctl.h"
//#include "sramctl.h"
#include "dram/plr_dram_gen2_memctl.h"
#include "dram/plr_plat_dep.h"

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
unsigned int plat_memctl_DSP_clock_MHz(void);
unsigned int plat_memctl_LX_clock_MHz(void);
void plat_mem_clk_rev_check(void);

 /* Memory controller input signal delay (CLKM delay, CLKM_90 delay, TX delay) */
unsigned int plat_memctl_input_sig_delay(unsigned int first_param, unsigned int second_param, unsigned int third_param);

void plat_memctl_ZQ_model_en(void);
void plat_memctl_ZQ_force_config(void);

/*
 * Function implementation 
 */

extern void mips_cache_flush_all(void);
//#define PLL_CTRL0_BASE 0xBB000028
//#define PLL_CTRL1_BASE 0xBB00002C
//
///* 0:1, 1:2, 2:4, 3:8. */
//#define SEL_PREDIV(x) ( (x)        & 0x3)
///* 0:1, 1:4 */
//#define SEL_D4(x)     (((x) >> 2)  & 0x1)
//#define NODE_IN(x)    (((x) >> 4)  & 0xFF)
//#define DIVN2(x)      (((x) >> 12) & 0xFF)
//
///* 0:2, 1:3, 2:4, 3:6 */
//#define DIVN3_SEL(x)  ((x) & 0x3)
//#define DIVN2_SELB(x) (((x) >> 2)  & 0x1)
//
//unsigned int pll_query_freq(unsigned int dev)
//{
//        const unsigned char divn3[] = {2, 3, 4, 6};
//        const unsigned char extra_divisor[] = {2, 2, 4}; //CPU, LX, MEM
//        const unsigned int pll_ctrl0 = *((volatile unsigned int *)(PLL_CTRL0_BASE+dev*0x10));
//        const unsigned int pll_ctrl1 = *((volatile unsigned int *)(PLL_CTRL1_BASE+dev*0x10));
//
//        return (25 *
//                (1 << SEL_PREDIV(pll_ctrl0)) *
//                (SEL_D4(pll_ctrl0) == 0 ? 1 : 4) *
//                (NODE_IN(pll_ctrl0) + 4) /
//                (DIVN2_SELB(pll_ctrl1) == 1  ?
//                divn3[DIVN3_SEL(pll_ctrl1)] :
//                (DIVN2(pll_ctrl0) + 4)) /
//                extra_divisor[dev]);
//}




#define DACCR_REG (*((volatile unsigned int *)(0xB8001500)))
#define PAD_STATIC_VALUE1 0x003512a2
#define PAD_STATIC_VALUE2 0x000002a3

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


void plat_memctl_ZQ_model_en(void)
{
    volatile unsigned int *dpcr;

    dpcr = (volatile unsigned int *)DPCR;
    /* Enable ZQ calibration model */
    if(memctlc_is_DDR3()){	// DDR 3
       *dpcr = *dpcr | 0xB;
    }else{			// DDR 1/2
       *dpcr = *dpcr | 0x3;
    }   
}

void plat_memctl_ZQ_force_config(void)
{

        DACCR_REG = DACCR_REG & 0xBFFFFFFF;

        /* OCD 60, ODT 75, from RDC. */
        DP_CR       = 0x0000000A;
        DP_MD0IOC   = PAD_STATIC_VALUE1;
        DP_MD1IOC   = PAD_STATIC_VALUE1;
        DP_MD2IOC   = PAD_STATIC_VALUE1;
        DP_MD3IOC   = PAD_STATIC_VALUE1;
        DP_LDQSIOC  = PAD_STATIC_VALUE1;
        DP_HDQSIOC  = PAD_STATIC_VALUE1;
        DP_LNDQSIOC = PAD_STATIC_VALUE1;
        DP_HNDQSIOC = PAD_STATIC_VALUE1;
        DP_LDMIOC   = PAD_STATIC_VALUE2;
        DP_HDMIOC   = PAD_STATIC_VALUE2;
        DP_MCIOC    = PAD_STATIC_VALUE2;
        DP_NMCIOC   = PAD_STATIC_VALUE2;
        DP_MAIOC    = PAD_STATIC_VALUE2;
        DP_MISCIOC  = PAD_STATIC_VALUE2;
}

#define ZQ_STATUS (*((volatile unsigned int *)(0xB8001098)))
inline void _zq_transform (zctrl_status_t zs, zq_force_v1_t *v1, zq_force_v2_t *v2) 
{
	v1->f.cfg_force = v2->f.cfg_force = 1;
    v1->f.ttfp = zs.f.ttfp;
    v1->f.ttfn = zs.f.ttfn;
    v1->f.ttcp = zs.f.ttcp;
    v1->f.ttcn = zs.f.ttcn;
    v1->f.plsb = v2->f.plsb = (zs.f.plsb_1 <<1) | zs.f.plsb_0;
    v1->f.nt = v2->f.nt = zs.f.nt;
    v1->f.pt = v2->f.pt = zs.f.pt;
    v1->f.e2 = v2->f.e2 = zs.f.e2;
}

void plat_memctl_zq_fine_tune(void)
{
    zctrl_status_t zs = (zctrl_status_t)ZQ_STATUS;
    zq_force_v1_t v1;
    zq_force_v2_t v2, v2m;
	_zq_transform(zs, &v1, &v2);

    /*printf("\n\tttfp=%d, tttfn=%d, ttcp=%d, ttcn=%d\n\tplsb=%d, nt=%d, pt=%d, e2=%d\n",
        v1.f.ttfp, v1.f.ttfn, v1.f.ttcp, v1.f.ttcn,
        v1.f.plsb, v1.f.nt, v1.f.pt, v1.f.e2);
    printf("\nZCTRL_STATUS=0x%x, v1=0x%x, v2=0x%x\n",
        zs.v, v1.v, v2.v);*/

    v2m.v=v2.v;
	// CK_N
	v2m.f.pt = (v2.f.pt>5)?7:(v2.f.pt+2);
	DP_NMCIOC = v2m.v;
	//printf("NMCIOC=0x%x\n", DP_NMCIOC);
}

/*
 * setting clock reverse indication.
 * Can't run in DRAM.
 */
#if 0
void plat_mem_clk_rev_check(void)
{
	unsigned int mem_clk;
	unsigned int cpu_clk;
	unsigned int lx_clk;

	lx_clk  = plat_memctl_LX_clock_MHz();
	cpu_clk = plat_memctl_CPU_clock_MHz();
	mem_clk = plat_memctl_MEM_clock_MHz();

	if(lx_clk <= mem_clk)
		REG32(SYS_LX_CLK_FRQ_SLOWER_REG) = SYS_LX_CLK_FRQ_SLOWER_REG_LX_CLK_SLOW_MASK;
	else
		REG32(SYS_LX_CLK_FRQ_SLOWER_REG) = 0;
	
        return;
}
#endif

 /* CPU related (cache flush) */
void plat_memctl_dcache_flush(void)
{

	mips_cache_flush_all();

	return;

}

 /* PAD related (Pad driving, static ZQ setting) */
void plat_memctl_dram_pad_drv(unsigned int drv_strength)
{
//    unsigned int dummy;
    
//    dummy = drv_strength;

}
void plat_memctl_dram_zq_setting(unsigned int is_static, unsigned int odt_ocd_ohm)
{
//    unsigned int dummy;
    
//    dummy = is_static;
//    dummy = odt_ocd_ohm;
}

 /* Platform clock related (CPU clock, LX bus clock, Memory clock) */
unsigned int plat_memctl_CPU_clock_MHz(void)
{
#if 0
        volatile unsigned int *mac_glb_ctrl;
        unsigned int cpu_clk_sel, cpu_clk;

        mac_glb_ctrl = (unsigned int *)SWCORE_MAC_GLB_CTRL_REG_ADDR;

        cpu_clk_sel = (*mac_glb_ctrl & SWCORE_MAC_GLB_CTRL_REG_CPU_CLK_SEL_MASK)\
                         >> SWCORE_MAC_GLB_CTRL_REG_CPU_CLK_SEL_FD_S;

        switch (cpu_clk_sel){

                case 1:
                        cpu_clk = 650;
                        break;
                case 2:
                        cpu_clk = 700;
                        break;
                case 3:
                        cpu_clk = 750;
                        break;
                default:
                        cpu_clk = 400;
                        break;

        }

        return cpu_clk;
#else
	return pll_query_freq(0);
#endif 
}

unsigned int plat_memctl_DSP_clock_MHz(void)
{
	return 0;
}
unsigned int plat_memctl_MEM_clock_MHz(void)
{
#if 0
        volatile unsigned int *mac_glb_ctrl;
        unsigned int ddr_clk_sel, ddr_clk;

        mac_glb_ctrl = (unsigned int *)SWCORE_MAC_GLB_CTRL_REG_ADDR;

        ddr_clk_sel = (*mac_glb_ctrl & SWCORE_MAC_GLB_CTRL_REG_DDR_CLK_SEL_MASK)\
                         >> SWCORE_MAC_GLB_CTRL_REG_DDR_CLK_SEL_FD_S;

        switch (ddr_clk_sel){

                case 1:
                        ddr_clk = 175;
                        break;
                case 2:
                        ddr_clk = 275;
                        break;
                case 3:
                        ddr_clk = 375;
                        break;
                case 4:
                        ddr_clk = 50;
                        break;
                case 5:
                        ddr_clk = 100;
                        break;
                case 6:
                        ddr_clk = 133;
                        break;
                case 7:
                        ddr_clk = 193;
                        break;
                default:
                        ddr_clk = 0;
                        break;

        }

        return ddr_clk;
#else
	return pll_query_freq(2);
#endif

}
unsigned int plat_memctl_LX_clock_MHz(void)
{
#if 0
	return 200;
#else
	return pll_query_freq(1);
#endif
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

    REG32(DCDR_A) = target_dcdr_value;
    while( REG32(DCDR_A) != target_dcdr_value ){
        _memctl_debug_printf_E("error: ddcr not equal write data(0x%08x)\n", target_dcdr_value);
        REG32(DCDR_A) = target_dcdr_value;
    }

    /* Delay a little bit for waiting for more stable of the DRAM clock.*/
    delay_loop = 0x10000;
    while(delay_loop--);

    return 0;
}

void plat_memctl_dramclk_en(void)
{
        volatile unsigned int *mac_glb_ctrl, delay_loop;
        unsigned int ddr_clk_sel;

        mac_glb_ctrl = (unsigned int *)SWCORE_MAC_GLB_CTRL_REG_ADDR;
        ddr_clk_sel = (*mac_glb_ctrl & SWCORE_MAC_GLB_CTRL_REG_DDR_CLK_SEL_MASK)\
                         >> SWCORE_MAC_GLB_CTRL_REG_DDR_CLK_SEL_FD_S;

	if(ddr_clk_sel){
	}else{
		/* select non zero dram clock setting. */
		*mac_glb_ctrl = (*mac_glb_ctrl & (~SWCORE_MAC_GLB_CTRL_REG_DDR_CLK_SEL_MASK)) |\
				(1 << SWCORE_MAC_GLB_CTRL_REG_DDR_CLK_SEL_FD_S);
		        /* Delay a little bit for waiting for more stable of the DRAM clock.*/
		delay_loop = 0x10000;
		while(delay_loop--);
	}

	return;
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

void plat_memctl_IO_PAD_patch(void)
{
	/* No patch code */
	return;
}

void plat_memctl_show_dram_config(void)
{
    printf(" DCDR: %08x\n", REG32(DCDR));
    printf("DIDER: %08x\n", REG32(DIDER));
    return;
}

u32_t plat_memctl_calculate_dqrf_delay(u32_t max_w_seq_start, u32_t max_w_len, u32_t max_r_seq_start, u32_t max_r_len) 
{
    u32_t ret_val;

    ret_val = (((max_w_seq_start + (max_w_len/4)) & 0x1f) << 24) | 
              (((max_r_seq_start + max_r_len - 1) & 0x1f) << 16) | 
              (((max_r_seq_start + (max_r_len/2)) & 0x1f) << 8) | 
              (((max_r_seq_start) & 0x1f) << 0);

    return ret_val;
}


