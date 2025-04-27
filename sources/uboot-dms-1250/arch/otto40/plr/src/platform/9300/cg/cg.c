#include <soc.h>
#include <uart/uart.h>
#include <cg/cg.h>
#include <osc.h>

#ifndef SECTION_CG
#define SECTION_CG
#endif
#ifndef SECTION_CG_INFO
#define SECTION_CG_INFO
#endif

cg_info_t cg_info_query SECTION_CG_INFO; 
extern otto_soc_context_t otto_sc;
extern u32_t get_spi_ctrl_divisor(void);
extern void set_spi_ctrl_divisor(u16_t clk_div, u16_t spif_mhz);

#ifndef PROJECT_ON_FPGA
#define VCO_FREQ(pdiv, div4, ncode)         ((25*((1==(div4))?4:1)*(2*((ncode)+2)))/(1<<(pdiv)))
#define CPU_FREQ(vco, div2, div3)           ((vco)/(((div2)+2)*((div3)+1)))   // div3: 0 for /1, 1 for /2, 2 reserved, 3 for /4
#define LNS_FREQ(vco, div2)                 ((vco)/(2*((div2)+2)))  // LX, SPI-NOR/NAND, SWITCH
#define DLL_FREQ(vco, div2)                 ((vco)/(((div2)+2)*4))
#define LN_DIV(vco, pll)                    (((vco)/(2*(pll)))-2)   // LX, SPI_NOR/NAND DIV

#define MIN_LX_PLL  (133)
#define MAX_LX_PLL  (204)
#define MIN_CPU_PLL (400)
#define MAX_CPU_PLL (800)
#define MIN_SPIF_PLL  (133)
#define MAX_SPIF_PLL  (204)
#define MIN_SRAM_PLL (250)
#define MAX_SRAM_PLL (500)
#define MAX_SPIF_FREQ (105)

static u32_t _cg_get_sw_vco(void) {
    u32_t vco;
    PLL_SW_CTRL0_T sw0 = { .v=PLL_SW_CTRL0rv };

    vco = VCO_FREQ(sw0.f.sw_cmu_sel_prediv, 
                   sw0.f.sw_cmu_sel_div4,
                   sw0.f.sw_cmu_ncode_in);
    return vco;
}
    
static u32_t _cg_get_cpu_freq(u32_t cpu0, u32_t cpum) {
    u32_t vco, freq;
    PLL_CPU_CTRL0_T c0 = { .v=cpu0 };
    PLL_CPU_MISC_CTRL_T cm = { .v=cpum };
    vco = VCO_FREQ(c0.f.cpu_cmu_sel_prediv, 
                   c0.f.cpu_cmu_sel_div4,
                   c0.f.cpu_cmu_ncode_in);
    
    freq = CPU_FREQ(vco, cm.f.reg_cmu_divn2_cpu, c0.f.cmu_divn3_cpu);
    return freq;
}

extern void fetch_and_lock_dcache_range(u32_t base_addr, u32_t end_addr);
extern void fetch_and_lock_icache_range(u32_t base_addr, u32_t end_addr);
extern void _change_cpu_sram_pll(u32_t cpu0_reg, u32_t cpum_reg, u32_t sram_pll_reg, u32_t sram_timing_reg);

UTIL_FAR SECTION_CG
void cg_cpu_sram_pll_init(cg_config_t *cg_config) {
    u32_t *addr = (u32_t *)_change_cpu_sram_pll;
    
    // locked 8KB cache
    fetch_and_lock_icache_range((u32_t)(CACHE_ALIGN(addr)), (u32_t)((CACHE_ALIGN(addr))+0x2000));

    // change cpu/sram pll and slow bit between ocp and SRAM/ROM
    _change_cpu_sram_pll(cg_config->cpu0, cg_config->cpum, cg_config->sram_reg, cg_config->sram_timing);

    // unlocked cache
    icache_inv_all();
}

UTIL_FAR SECTION_CG
void cg_lx_pll_init(cg_config_t *cg_config) {
    RMOD_PLL_GLB_CTRL0(lxb_clk_sel, 0x1);
    RMOD_PLL_SW_DIV_CTRL(cmu_divn2_lxb, cg_config->lx_pll_div);
    RMOD_PLL_GLB_CTRL0(lxb_clk_sel, 0x0);

    u32_t cmu_divn2_lxb __attribute__ ((unused)) = RFLD_PLL_SW_DIV_CTRL(cmu_divn2_lxb);
    RMOD_CPU_ASY_TIMING(sram_lx_pulse, cg_config->sram_lx_slow);
    // lx to dram will be set later
    cg_udelay(50, cg_info_query.dev_freq.cpu_mhz);
}

UTIL_FAR SECTION_CG
void cg_spif_pll_init(cg_config_t *cg_config) {
#ifdef OTTO_FLASH_NAND_SPI
// SPI-NAND
    set_spi_ctrl_divisor(cg_config->snaf_ctrl_div , 0);
    extern void set_spi_ctrl_latency(u16_t);
    set_spi_ctrl_latency(cg_config->snaf_pl);
#else
// SPI-NOR
    RMOD_PLL_GLB_CTRL0(nor_clk_sel, 1);
    RMOD_PLL_SW_DIV_CTRL(cmu_divn2_spi_nor, cg_config->spif_pll_div );
    otto_sc.spif_div = cg_config->spif_ctrl_div;
    set_spi_ctrl_divisor(cg_config->spif_ctrl_div+(cg_info_query.dev_freq.spif_mhz>50)?1:0, 0);

    // for NOR+NAND, and will set in u-Boot
    otto_sc.snaf_pl = cg_config->snaf_pl;
    otto_sc.snaf_div = cg_config->snaf_ctrl_div;

    RMOD_CMU_BC_OC0(se_spif, cg_config->oc_spif_slow);
    RMOD_PLL_GLB_CTRL0(nor_clk_sel, 0);
#endif  //ifdef OTTO_FLASH_NAND_SPI
    cg_udelay(50, cg_info_query.dev_freq.cpu_mhz);
}

int _cg_config_cpu(cg_config_t *cg_config) {
    if(cg_info_query.dev_freq.cpu_mhz > MAX_CPU_PLL ||
       cg_info_query.dev_freq.cpu_mhz < MIN_CPU_PLL) 
    {  return CPU_PLL_SET_FAIL; }

                      // parameters are from haitao
                      //     400,  425,  450,  475,  500,  525,  550,  575,  600,
                      //     625,  650,  675,  700,  725,  750,  775,  800 
    static u8_t ncode[] = { 0x2E, 0x31, 0x34, 0x37, 0x3A, 0x32, 0x35, 0x37, 0x2E,
                            0x30, 0x32, 0x34, 0x36, 0x38, 0x3A, 0x3C, 0x3E}; 
    static u8_t divn2[] = { 0x04, 0x04, 0x04, 0x04, 0x04, 0x03, 0x03, 0x03, 0x02,
                            0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02};    

    u32_t idx=(cg_info_query.dev_freq.cpu_mhz-MIN_CPU_PLL)/25;


    cg_config->ocp_pll = (MIN_CPU_PLL+idx*25)/2;
    PLL_CPU_CTRL0_T cpu0 = {.v = PLL_CPU_CTRL0rv };
    PLL_CPU_MISC_CTRL_T cpum = {.v = PLL_CPU_MISC_CTRLrv };
    PLL_SRAM_CTRL_T sram_reg = { .v = PLL_SRAM_CTRLrv };
    CPU_SRAM_ASY_TIMING_T sram_timing = { .v = CPU_SRAM_ASY_TIMINGrv };

    cpu0.f.cpu_cmu_ncode_in = ncode[idx];
    cpu0.f.cmu_divn3_cpu = 0;
    cpum.f.reg_cmu_divn2_cpu = divn2[idx];

    cg_info_query.dev_freq.cpu_mhz = _cg_get_cpu_freq(cpu0.v, cpum.v);    
    cg_config->ocp_pll = cg_info_query.dev_freq.cpu_mhz/2;

#if 1    
    // sram pll prepare, and slow bit check
    if(cg_info_query.dev_freq.sram_mhz==MAX_SRAM_PLL) {
        sram_reg.f.sram_pll_sel=1; 
    } else { 
        sram_reg.f.sram_pll_sel=0;
    }
    
    if(cg_info_query.dev_freq.sram_mhz<cg_config->ocp_pll) {
        sram_timing.f.sram_oc0_pulse=0;
    } else {
        sram_timing.f.sram_oc0_pulse=1;
    }
    
    if(cg_config->ocp_pll<cg_info_query.dev_freq.sram_mhz) {
        sram_timing.f.oc0_sram_pulse=0;
    } else if(cg_config->ocp_pll<(cg_info_query.dev_freq.sram_mhz<<1)) {
        sram_timing.f.oc0_sram_pulse=1;
    } else {
        sram_timing.f.oc0_sram_pulse=2;
    }
#endif
    
    cg_config->cpu0 = cpu0.v;
    cg_config->cpum = cpum.v;
    cg_config->sram_reg = sram_reg.v;
    cg_config->sram_timing = sram_timing.v;
    return 0;
    }

int _cg_config_lx(cg_config_t *cg_config) {
    cg_config->sw_vco = _cg_get_sw_vco();
  #if 1   // follow Juan, keep default
    if (cg_info_query.dev_freq.lx_mhz < MIN_LX_PLL ||
        cg_info_query.dev_freq.lx_mhz > MAX_LX_PLL) 
    { return LX_PLL_SET_FAIL; }
    
    //switch vco is fixed 2.4G, lx freq = vco/(2*(div+2))
    u32_t div=LN_DIV(cg_config->sw_vco, cg_info_query.dev_freq.lx_mhz); 
    if(div>LN_DIV(cg_config->sw_vco, MIN_LX_PLL)) div=LN_DIV(cg_config->sw_vco, OTTO_LX_DEF_FREQ);  // reset to default 175
    cg_config->lx_pll_div = div;
    
    cg_info_query.dev_freq.lx_mhz = LNS_FREQ(cg_config->sw_vco, div);
  #endif
    
    // slow bit, lx to sram
    u8_t slow = 0;
    if((3*cg_info_query.dev_freq.lx_mhz) <= cg_info_query.dev_freq.sram_mhz) { slow = 1; }
    else { slow = 0; }
    cg_config->sram_lx_slow = slow;
    return 0;
}

int _cg_config_spif(cg_config_t *cg_config) {
    if(0==cg_info_query.dev_freq.snaf_mhz)
        cg_info_query.dev_freq.snaf_mhz = cg_info_query.dev_freq.spif_mhz;
    if((cg_info_query.dev_freq.spif_mhz > MAX_SPIF_FREQ) ||
       (cg_info_query.dev_freq.snaf_mhz > MAX_SPIF_FREQ))
        return SPIF_PLL_SET_FAIL;
    extern clk_div_sel_info_t sclk_divisor[];

    u32_t divc=0, spif_mhz;
    u32_t tmp_divc=7, tmp_spif_mhz=0;
    while(END_OF_INFO!=sclk_divisor[divc].divisor) {
        spif_mhz = cg_info_query.dev_freq.lx_mhz/sclk_divisor[divc].divisor;
        if((spif_mhz<=cg_info_query.dev_freq.snaf_mhz) && (spif_mhz > tmp_spif_mhz)) {
            tmp_divc = divc;
            tmp_spif_mhz = spif_mhz;
        }
        ++divc;
    }
    cg_config->snaf_ctrl_div=tmp_divc;
    cg_info_query.dev_freq.snaf_mhz = tmp_spif_mhz;
    if(tmp_spif_mhz>50) cg_config->snaf_pl = 2;
    cg_config->snaf_pl = (tmp_spif_mhz>50)?2:1;
    
#ifndef OTTO_FLASH_NAND_SPI
    // SPI-NOR
    u32_t min_divp = LN_DIV(cg_config->sw_vco, MAX_SPIF_PLL);
    u32_t max_divp = LN_DIV(cg_config->sw_vco, MIN_SPIF_PLL);
    u32_t divp, tmp_divp=max_divp;
    divc=0; tmp_divc=7; tmp_spif_mhz=0;
    
    while(END_OF_INFO!=sclk_divisor[divc].divisor) {
        for (divp=min_divp; divp<=max_divp; divp++) {
            spif_mhz = LNS_FREQ(cg_config->sw_vco, divp)/sclk_divisor[divc].divisor;
            if ((spif_mhz<=cg_info_query.dev_freq.spif_mhz) && (spif_mhz > tmp_spif_mhz)) {
                tmp_divp = divp;
                tmp_divc = divc;
                tmp_spif_mhz = spif_mhz;
            }                
        }
        ++divc;
}
    cg_config->spif_pll_div = tmp_divp;
    cg_config->spif_ctrl_div = sclk_divisor[tmp_divc].div_to_ctrl;
    // slow bit check
    cg_config->oc_spif_slow = (cg_config->ocp_pll > tmp_spif_mhz)?0:1;
#endif
    // if nand only, spif_mhz = snaf_mhz, else nor: spif_mhz, nand: snaf_mhz
    cg_info_query.dev_freq.spif_mhz = tmp_spif_mhz;

    return PLL_SET_DONE;
}

extern int _cg_config_mem(cg_config_t *cg_config);
int cg_config_transform(cg_config_t *cg_config) {
    u32_t res;
    
    // config CPU
    res=_cg_config_cpu(cg_config);
    res|=_cg_config_lx(cg_config);
    res|=_cg_config_spif(cg_config);
    res|=_cg_config_mem(cg_config);
    return res;
}
    
extern void cg_mem_pll_init(cg_config_t *cg_config);
UTIL_FAR SECTION_CG
void cg_config_apply(cg_config_t *cg_config) {
    cg_cpu_sram_pll_init(cg_config); /* CPU PLL */
    cg_lx_pll_init(cg_config);       /* LX PLL */
    cg_spif_pll_init(cg_config);     /* SPI-NOR or SPI-NAND PLL */
    cg_mem_pll_init(cg_config);      /* MEM PLL */
}
#endif  //PROJECT_ON_FPGA

SECTION_CG 
u32_t cg_udelay(u32_t us, u32_t mhz) {
        u32_t loop_cnt = us*mhz/2;
        while (loop_cnt--) {
                ;
        }
        return loop_cnt;
}

UTIL_FAR SECTION_CG 
u32_t cg_query_freq(u32_t dev_type) {
    return CG_QUERY_FREQUENCY(dev_type,(&cg_info_query.dev_freq));
}

SECTION_CG
void cg_copy_info_to_sram(void) {
    // assume snaf = spif
    inline_memcpy(&cg_info_query, &cg_info_proj, sizeof(cg_info_t));
}

#ifdef PROJECT_ON_FPGA
//SECTION_CG
void cg_slow_bit_check() {
  
    // CAUSION: There are different CMU gen for FPGA and ASIC
    // handle freq. 
    u32_t val = REG32(0xB8000308);
    if((cg_info_query.dev_freq.cpu_mhz/2) > cg_info_query.dev_freq.mem_mhz) {
        val = val & 0xFFFFFFEF;
    } else {
        val = val | 0x10;
    }
    if(cg_info_query.dev_freq.lx_mhz > cg_info_query.dev_freq.mem_mhz) {
        val = val & 0xFFFFFFF8;
    } else {
        val = val | 0x7; 
    }
    REG32(0xB8000308) = val;
}
#endif

void cg_init(void) {
    int res=0;
  #ifdef PROJECT_ON_FPGA
    cg_slow_bit_check();
  #else
    cg_config_t cg_config;
    
    res = cg_config_transform(&cg_config);
    if (res!=PLL_SET_DONE) {
        printf("WW: CG Init Failed, res=0x%x\n", res);
    }

    cg_config_apply(&cg_config);
  #endif
    if (0==(res&LX_PLL_SET_FAIL) && uart_baud_rate!=0) {
        // re-init uart, lx_mhz might be changed
        uart_init(uart_baud_rate, cg_info_query.dev_freq.lx_mhz);
    }
    
    printf("II: CPU %dMHz, MEM %dMHz, LX %dMHz, SPIF %dMHz\n",
        cg_info_query.dev_freq.cpu_mhz,
        cg_info_query.dev_freq.mem_mhz,
        cg_info_query.dev_freq.lx_mhz,
        cg_info_query.dev_freq.spif_mhz);    
}

REG_INIT_FUNC(cg_copy_info_to_sram, 11);
REG_INIT_FUNC(cg_init, 13);
symb_pdefine(cg_info_dev_freq, SF_SYS_CG_DEV_FREQ, &(cg_info_query.dev_freq));

