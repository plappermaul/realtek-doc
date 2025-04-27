#include <soc.h>
#include <cg/cg.h>
#include <cg/mem_pll.h>

#ifndef SECTION_CG
#define SECTION_CG
#endif

#define MEM_FREQ(pdiv, ncode)               (25*(ncode+2)/(2*(1<<pdiv)))

#ifndef PROJECT_ON_FPGA
UTIL_FAR SECTION_CG
u32_t cg_guery_mem_freq(u32_t _mem2, u32_t _mem3) {
    /* MEM */
    SYS_MEM_PLL_CTRL2_T mem2 = { .v=_mem2};
    SYS_MEM_PLL_CTRL3_T mem3 = { .v=_mem3 };  
    return MEM_FREQ(mem2.f.pdiv, mem3.f.n_code);
}

int _cg_config_mem(cg_config_t *cg_config) {
    // ERROR Check?
    // return Fail;
    if (100==cg_info_query.dev_freq.mem_mhz) {
        cg_config->mem0 = SYS_MEM_PLL_CTRL0dv;
        cg_config->mem1 = 0x000001EF;
        cg_config->mem2 = 0x26226400;
        cg_config->mem3 = 0x0E0F0000;
    } else {
                                //     250,        275,        300,        325,        350,
                                //     375,        400,        425,        450,        475,
                                //     500,        525,        550,        575,        600,
        static u32_t mem0[] = { 0x0008407F, 0x0008407F, 0x0008407F, 0x0008407F, 0x0008407F,
                                0x0008407F, 0x0000007F, 0x0008407F, 0x0008407F, 0x000C607F,
                                0x000C607F, 0x000C607F, 0x000C607F, 0x000C807F, 0x0000007F};
        static u32_t mem1[] = { 0x4000018F, 0x4000018F, 0x4000018F, 0x4000018F, 0x4000018F,
                                0x4000018F, 0x400001EF, 0x4000018F, 0x4000018F, 0x4000018F,
                                0x4000018F, 0x4000018F, 0x4000018F, 0x4000018F, 0x400001EF};
        static u32_t mem2[] = { 0x26222400, 0x26222400, 0x26222520, 0x26222520, 0x26222520,
                                0x26622520, 0x26622520, 0x26A23720, 0x26A23720, 0x26A23740,
                                0x26A23740, 0x26A33840, 0x26A33840, 0x26A33860, 0x26A33860};
        static u32_t mem3[] = { 0x120F0000, 0x140F0000, 0x160F0000, 0x180F0000, 0x1A0F0000, 
                                0x1C0F0000, 0x1E0F0000, 0x200F0000, 0x220F0000, 0x240F0000, 
                                0x260F0000, 0x280F0000, 0x2A0F0000, 0x2C0F0000, 0x2E0F0000};
        u32_t idx = (cg_info_query.dev_freq.mem_mhz-250)/25;
        if(idx>=(sizeof(mem0)/sizeof(u32_t))) idx=0;
        cg_config->mem0 = mem0[idx]; 	//SYS_MEM_PLL_CTRL0dv;
        cg_config->mem1 = mem1[idx];	//0x400001EF;
        cg_config->mem2 = mem2[idx];
        cg_config->mem3 = mem3[idx];
    }    
  
    cg_info_query.dev_freq.mem_mhz = cg_guery_mem_freq(cg_config->mem2, cg_config->mem3);

    cg_config->oc_mem_slow = (cg_config->ocp_pll > cg_info_query.dev_freq.mem_mhz)?0:1;
    cg_config->lx_mem_slow = (cg_info_query.dev_freq.lx_mhz > cg_info_query.dev_freq.mem_mhz)?0:1;
    return PLL_SET_DONE;
}

SECTION_CG
void cg_mem_pll_init(cg_config_t *cg_config) {
    CG_MEM_PLL_OE_DIS();
    cg_udelay(200, cg_info_query.dev_freq.cpu_mhz);

    SYS_MEM_PLL_CTRL0rv = cg_config->mem0;
    SYS_MEM_PLL_CTRL1rv = cg_config->mem1;
    SYS_MEM_PLL_CTRL2rv = cg_config->mem2;
    SYS_MEM_PLL_CTRL3rv = cg_config->mem3;
    SYS_MEM_PLL_CTRL5rv = SYS_MEM_PLL_CTRL5dv;
    SYS_MEM_PLL_CTRL6rv = SYS_MEM_PLL_CTRL6dv;
    cg_udelay(5, cg_info_query.dev_freq.cpu_mhz);
    CG_MEM_PLL_OE_EN();
    cg_udelay(200, cg_info_query.dev_freq.cpu_mhz);
    
    // OC v.s MEM slow bit
    RMOD_CMU_BC_OC0(se_dram, cg_config->oc_mem_slow);
    
    // LX v.s MEM slow bit
    RMOD_CMU_BC_LX0(se_dram, cg_config->lx_mem_slow);
    RMOD_CMU_BC_LX1(se_dram, cg_config->lx_mem_slow);
    RMOD_CMU_BC_LX2(se_dram, cg_config->lx_mem_slow);
    RMOD_CMU_BC_LXP(se_dram, cg_config->lx_mem_slow);
    cg_udelay(50, cg_info_query.dev_freq.cpu_mhz);
}

SECTION_CG UTIL_FAR
void cg_mem_pll_config_dqs_pi(u32_t dqs0_pi, u32_t dqs1_pi) {
    SYS_MEM_PLL_CTRL0_T pll0 = {.v=SYS_MEM_PLL_CTRL0rv};

    CG_MEM_PLL_OE_DIS();
    pll0.f.post_pi_sel2=dqs0_pi;         // DQS0
    pll0.f.post_pi_sel3=dqs1_pi;         // DQS1
    SYS_MEM_PLL_CTRL0rv=pll0.v;
    cg_udelay(5, cg_info_query.dev_freq.cpu_mhz);
    CG_MEM_PLL_OE_EN();
    cg_udelay(200, cg_info_query.dev_freq.cpu_mhz);
}

SECTION_CG UTIL_FAR
void cg_mem_pll_config_dq_pi(u32_t dq0_7_pi, u32_t dq8_15_pi) {
    SYS_MEM_PLL_CTRL1_T pll1 = {.v=SYS_MEM_PLL_CTRL1rv};

    CG_MEM_PLL_OE_DIS();
    pll1.f.post_pi_sel4=dq0_7_pi;
    pll1.f.post_pi_sel5=dq8_15_pi;
    SYS_MEM_PLL_CTRL1rv=pll1.v;
    cg_udelay(5, cg_info_query.dev_freq.cpu_mhz);
    CG_MEM_PLL_OE_EN();
    cg_udelay(200, cg_info_query.dev_freq.cpu_mhz);
}
#endif

// FIXME, DLL
//    // FIXME, for DLL debug    
//    PLL_CPU_MISC_CTRL_T r={.v=PLL_CPU_MISC_CTRLrv};
//    r.f.reg_cmu_divn2_dll=0x0;
//    PLL_CPU_MISC_CTRLrv=r.v;
//    // FIXME end
