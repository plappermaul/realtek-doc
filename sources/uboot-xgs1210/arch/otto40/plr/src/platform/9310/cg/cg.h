#ifndef __CG_HEADER__
#define __CG_HEADER__

#include <register_map.h>
#include <cg/cg_dev_freq.h>
//#include <cg/glb_pll.h>

typedef struct {
	cg_dev_freq_t     dev_freq;
} cg_info_t;

typedef struct {
    u32_t   sp_pll; //sheipa pll
    
    // reg
    u32_t   set_cpu;
        u32_t   set_lx0;
        u32_t   set_lx1;
    u8_t    spif_pll_div2;
        u8_t    spif_ctrl_div;
    u8_t    snaf_ctrl_div;
    u8_t    snaf_pl;
        u8_t    oc_spif_slow;
} cg_config_t;

typedef struct clk_div_sel_info_s{
        u16_t divisor;
        u16_t div_to_ctrl;
} clk_div_sel_info_t;

__attribute__((far)) u32_t cg_query_freq(u32_t dev_type);
extern u32_t cg_udelay(u32_t us, u32_t mhz);

void cg_init(void);
void cg_result_decode(void);

extern cg_info_t cg_info_query;

#define PLL_SET_DONE        (0)
#define CPU_PLL_SET_FAIL    (1<<0)
#define LX_PLL_SET_FAIL     (1<<1)
#define SPIF_PLL_SET_FAIL   (1<<2)
#define MEM_PLL_SET_FAIL    (1<<3)
#define END_OF_INFO         (0xFFFF)
#endif
