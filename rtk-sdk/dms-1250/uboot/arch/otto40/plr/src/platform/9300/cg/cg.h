#ifndef __CG_HEADER__
#define __CG_HEADER__

#include <register_map.h>
#include <cg/cg_dev_freq.h>
#include <cg/mem_pll.h>
#include <cg/glb_pll.h>

#define END_OF_INFO (0xFFFF)

typedef struct {
	cg_dev_freq_t       dev_freq;
} cg_info_t;

typedef struct {
    // for record
    u16_t   ocp_pll;
    u16_t   sw_vco;
    // reg
    u32_t   cpu0;
    u32_t   cpum;
    u32_t   sram_reg;
    u32_t   sram_timing;
    u32_t   mem0;
    u32_t   mem1;
    u32_t   mem2;
    u32_t   mem3;

    u8_t    lx_pll_div;
    u8_t    spif_pll_div;
    u8_t    spif_ctrl_div;
    u8_t    snaf_ctrl_div;
    u8_t    snaf_pl;
    u8_t    oc_spif_slow;
    u8_t    sram_lx_slow;    
    u8_t    oc_mem_slow;
    u8_t    lx_mem_slow;
} cg_config_t;

typedef struct clk_div_sel_info_s{
    u16_t divisor;
    u16_t div_to_ctrl;
} clk_div_sel_info_t;

__attribute__((far)) u32_t cg_query_freq(u32_t dev_type);
extern u32_t cg_udelay(u32_t us, u32_t mhz);

void cg_mem_pll_config_dqs_pi(u32_t dqs0_pi, u32_t dqs1_pi);
void cg_mem_pll_config_dq_pi(u32_t dq0_7_pi, u32_t dq8_15_pi);

extern void cg_init(void);
void cg_result_decode(void);

extern cg_info_t cg_info_query;

#ifndef CG_MEM_PLL_OE_DIS
#define CG_MEM_PLL_OE_DIS
#endif
#ifndef CG_MEM_PLL_OE_EN
#define CG_MEM_PLL_OE_EN
#endif

#define PLL_SET_DONE        (0)
#define CPU_PLL_SET_FAIL    (1<<0)
#define LX_PLL_SET_FAIL     (1<<1)
#define SPIF_PLL_SET_FAIL   (1<<2)
#define MEM_PLL_SET_FAIL    (1<<3)

#endif
