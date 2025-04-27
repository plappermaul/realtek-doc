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

#ifndef PROJECT_ON_FPGA
#define CPU_VCO_FREQ(n, f, pdiv, fac) (25*((2*((n)+2))*fac+((f)*fac/8192)))/((1<<(pdiv))*fac)
//#define CPU_VCO_FREQ(n, f, pdiv) (25*(2*((n)+2)+((f)/8192)))/((1<<(pdiv)))
#define LX_VCO_FREQ(n, pdiv)     (25*(2*((n)+2))/(1<<(pdiv)))
#define LNS_FREQ(vco, div2)      ((vco)/(2*((div2)+2)))  // LX, SPI-NOR/NAND
#define LN_DIV(vco, pll)         (((vco)/(2*(pll)))-2)   // LX, SPI_NOR/NAND DIV

#define GET_CPU_FREQ(n, d1, d2, d3) (25*(2*((n)+2)))/((d1+1)*(d2+1)*(1<<(d3)))
#define GET_CPU_FREQ2(n, f, d1, d2, d3)  (CPU_VCO_FREQ(n, f, 0, 10000))/((d1+1)*(d2+1)*(1<<(d3)))
//#define GET_CPU_FREQ2(n, f, d1, d2, d3)  (CPU_VCO_FREQ(n, f, 0))/((d1+1)*(d2+1)*(1<<(d3)))
#define GET_LX_FREQ(n, d1, d2)      (25*(2*((n)+2)))/((d1+1)*(d2+2))
#define GET_SPIF_FREQ(n, d1, d2)    GET_LX_FREQ(n, d1, d2)

#define MIN_LX_PLL      (125)
#define MAX_LX_PLL      (200)
#define MIN_CPU_PLL     (500)
#define MAX_CPU_PLL     (1600)
#define MIN_SPIF_PLL    (125)
#define MAX_SPIF_PLL    (200)
#define MAX_SPIF_FREQ   (105)
                   
int _cg_config_cpu(cg_config_t *c) {
    if(cg_info_query.dev_freq.cpu_mhz > MAX_CPU_PLL ||
       cg_info_query.dev_freq.cpu_mhz < MIN_CPU_PLL)
    {  return CPU_PLL_SET_FAIL; }
#if 0
    u32_t n, f, d1, d3;
    u32_t tn=0, tf=0, td1=0, td3=0;
    u32_t vco, vl=2200, vu=3200;
    u32_t cpu_mhz=0, tmp;
    for(n=0, f=0; n<0x40; n++) {
        for(f=0; f<8192; f++) {
            vco = CPU_VCO_FREQ(n, f, 0, 10000);
            //vco = CPU_VCO_FREQ(n, f, 0);
            if(vco<vl || vco>vu) continue;

            for(d1=0; d1<3; d1++) {
                for(d3=0; d3<4; d3++) {
                    tmp = GET_CPU_FREQ2(n, f, d1, 0, d3);
                    if((tmp<=cg_info_query.dev_freq.cpu_mhz)&&tmp>cpu_mhz) {
                        cpu_mhz=tmp;
                        tn=n; tf=f; td1=d1; td3=d3;
                    }
                }
            }
        }
    }
    printf("DD: vco=%d, n(0x%x), f(%d), d1(%d), d3(%d)\n", cpu_mhz, tn, tf, td1, td3);
    PLL_CPU_CTRL0_T c0 = { .v=PLL_CPU_CTRL0rv };
    c0.f.cpu_cmu_en       = 0;
    c0.f.cpu_cmu_bypass   = 0;
    c0.f.cpu_cmu_fcode_in = tf;
    c0.f.cpu_cmu_ncode_in = tn;
    c0.f.cmu_divn1_cpu    = td1;
    c0.f.cmu_divn2_cpu    = 0;
    c0.f.cmu_divn3_cpu    = td3;
#else
                                // 500,  600,  700,  800,  900, 1000, 1100, 1200, 1300
    static const u8_t ncode[] = { 0x26, 0x2E, 0x36, 0x3E, 0x22, 0x26, 0x2A, 0x2E, 0x34 };
    static const u8_t divn1[] = {  0x1,  0x1,  0x1,  0x1,  0x1,  0x1,  0x0,  0x0,  0x0 };
    static const u8_t divn2[] = {  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0,  0x0 };
    static const u8_t divn3[] = {  0x1,  0x1,  0x1,  0x1,  0x0,  0x0,  0x1,  0x1,  0x1 };
    u32_t idx = (cg_info_query.dev_freq.cpu_mhz-MIN_CPU_PLL)/100;
    
    PLL_CPU_CTRL0_T c0 = { .v=PLL_CPU_CTRL0rv };
    c0.f.cpu_cmu_en       = 0;
    c0.f.cpu_cmu_ncode_in = ncode[idx];
    c0.f.cmu_divn1_cpu    = divn1[idx];
    c0.f.cmu_divn2_cpu    = divn2[idx];
    c0.f.cmu_divn3_cpu    = divn3[idx];
#endif
    c->set_cpu = c0.v;
    //cg_info_query.dev_freq.cpu_mhz = GET_CPU_FREQ(c0.f.cpu_cmu_ncode_in, c0.f.cmu_divn1_cpu, c0.f.cmu_divn2_cpu, c0.f.cmu_divn3_cpu);
    cg_info_query.dev_freq.cpu_mhz = GET_CPU_FREQ2(c0.f.cpu_cmu_ncode_in, c0.f.cpu_cmu_fcode_in, c0.f.cmu_divn1_cpu, c0.f.cmu_divn2_cpu, c0.f.cmu_divn3_cpu);
    c->sp_pll = cg_info_query.dev_freq.cpu_mhz/2;
    //printf("DD: CPU idx=%d, ncode=0x%x, div1=0x%x, div2=0x%x, div3=0x%x\n",
    //        idx, c0.f.cpu_cmu_ncode_in, c0.f.cmu_divn1_cpu,
    //        c0.f.cmu_divn2_cpu, c0.f.cmu_divn3_cpu);
    //printf("DD: CPU freq=%d, sp_pll=%d\n", cg_info_query.dev_freq.cpu_mhz, c->sp_pll);
    return 0;
}

int _cg_config_lx(cg_config_t *c) {
    if (cg_info_query.dev_freq.lx_mhz < MIN_LX_PLL ||
        cg_info_query.dev_freq.lx_mhz > MAX_LX_PLL)
    { return LX_PLL_SET_FAIL; }
                           //125,  150,  175,  200
    static const u8_t ncode[] = { 0x3A, 0x3A, 0x36, 0x36 };
    static const u8_t div1[]  = {  0x1,  0x1,  0x1,  0x1 };
    static const u8_t div2[]  = {  0xA,  0x8,  0x6,  0x5 };
    
    u32_t idx = (cg_info_query.dev_freq.lx_mhz-MIN_LX_PLL)/25;
    
    PLL_LXB_CTRL0_T c0 = { .v=PLL_LXB_CTRL0rv };
    PLL_LXB_CTRL1_T c1 = { .v=PLL_LXB_CTRL1rv };
    
    c0.f.lxb_cmu_en       = 0;
    c0.f.lxb_cmu_ncode_in = ncode[idx];
    c1.f.cmu_div1_lxb     = div1[idx];
    c1.f.cmu_div2_lxb     = div2[idx];
    c->set_lx0 = c0.v;
    c->set_lx1 = c1.v;
    //printf("DD: LX idx=%d, ncode=%d, div1=%d, div2=%d\n",
    //        idx, c0.f.lxb_cmu_ncode_in,
    //        c1.f.cmu_div1_lxb, c1.f.cmu_div2_lxb);
    cg_info_query.dev_freq.lx_mhz = GET_LX_FREQ(c0.f.lxb_cmu_ncode_in, c1.f.cmu_div1_lxb, c1.f.cmu_div2_lxb);
    //printf("DD: LX freq=%d\n", cg_info_query.dev_freq.lx_mhz);
    return 0;
}

#define FL_FAC	(1000)
int _cg_config_spif(cg_config_t *c) {
    if(0==cg_info_query.dev_freq.snaf_mhz)
        cg_info_query.dev_freq.snaf_mhz = cg_info_query.dev_freq.spif_mhz;
    if((cg_info_query.dev_freq.spif_mhz > MAX_SPIF_FREQ) ||
       (cg_info_query.dev_freq.snaf_mhz > MAX_SPIF_FREQ))
        return SPIF_PLL_SET_FAIL;

    extern clk_div_sel_info_t sclk_divisor[];
    u32_t divc=0, spif_mhz;
    u32_t tmp_divc=7, tmp_spif_mhz=0;
    while(END_OF_INFO!=sclk_divisor[divc].divisor) {
        spif_mhz = (cg_info_query.dev_freq.lx_mhz*FL_FAC)/sclk_divisor[divc].divisor;
        if((spif_mhz<=(cg_info_query.dev_freq.snaf_mhz*FL_FAC)) && (spif_mhz > tmp_spif_mhz)) {
            tmp_divc = divc;
            tmp_spif_mhz = spif_mhz;
        }
        ++divc;
    }
    c->snaf_ctrl_div=tmp_divc;
    cg_info_query.dev_freq.snaf_mhz = tmp_spif_mhz/FL_FAC;
    c->snaf_pl = (tmp_spif_mhz>50)?2:1;

#ifndef OTTO_FLASH_NAND_SPI
    PLL_LXB_CTRL0_T lxb = { .v=PLL_LXB_CTRL0rv };
    // according to LX VCO
    u32_t vco = LX_VCO_FREQ(lxb.f.lxb_cmu_ncode_in, lxb.f.lxb_cmu_sel_prediv);
    u32_t min_div2 = LN_DIV(vco, MAX_SPIF_PLL);
    u32_t max_div2 = LN_DIV(vco, MIN_SPIF_PLL);
    u32_t div2, tmp_div2=max_div2;
    divc=0; tmp_divc=7; tmp_spif_mhz=0;
    
    while(END_OF_INFO!=sclk_divisor[divc].divisor) {
        for (div2=min_div2; div2<=max_div2; div2++) {
            spif_mhz = (LNS_FREQ(vco, div2)*FL_FAC)/sclk_divisor[divc].divisor;
            if((spif_mhz<=(cg_info_query.dev_freq.spif_mhz*FL_FAC)) && (spif_mhz > tmp_spif_mhz)) {
                tmp_div2 = div2;
                tmp_divc = divc;
                tmp_spif_mhz = spif_mhz;
            }
        }
        ++divc;
    }
    c->spif_pll_div2 = tmp_div2;
    c->spif_ctrl_div = sclk_divisor[tmp_divc].div_to_ctrl;
    // slow bit check
    c->oc_spif_slow = (c->sp_pll>LNS_FREQ(vco, tmp_div2))?0:1;
#endif
    // if nand-only, spif_mhz = snaf_mhz
    cg_info_query.dev_freq.spif_mhz = tmp_spif_mhz/FL_FAC;
    
    //printf("DD: LX VCO=%d, spif_pll div=%d, ctrl div=%d, freq=%dMHz\n",
    //        vco, c->spif_pll_div2, c->spif_ctrl_div, tmp_spif_mhz);
    return 0;
}

extern void bp_barrier(void) UTIL_FAR;
int cg_config_transform(cg_config_t *c) {
    u32_t res;
     // config CPU
    res=_cg_config_cpu(c);
    res|=_cg_config_lx(c);
    res|=_cg_config_spif(c);
    bp_barrier();
    //udelay(1000);
    return res;
}

UTIL_FAR SECTION_CG
void cg_cpu_pll_init(cg_config_t *c) {
    u32_t cnt=0;
    // change to lx clk
    RMOD_SSR(cksel_ocp0, 0x0);
    
    // apply CPU setting and cpu_cmu_en=0x0
    PLL_CPU_CTRL0rv = c->set_cpu;
    cg_udelay(10, cg_info_query.dev_freq.cpu_mhz);
    RMOD_PLL_CPU_CTRL0(cpu_cmu_en, 0x1);
    
    while(0==RFLD_PLL_GLB_SIG(cpu_clk_rdy)){
        cnt++;
        if(cnt>0x100000) break;
    }
    // change to cpu clk
    RMOD_SSR(cksel_ocp0, 0x1);
}

UTIL_FAR SECTION_CG
void cg_lx_pll_init(cg_config_t *c) {
    u32_t cnt=0;
    RMOD_PLL_GLB_CTRL(lxb_clksel, 0x1);
    
    // apply LX setting and cpu_cmu_en=0x0
    PLL_LXB_CTRL0rv = c->set_lx0;
    PLL_LXB_CTRL1rv = c->set_lx1;
    cg_udelay(10, cg_info_query.dev_freq.cpu_mhz);
    RMOD_PLL_LXB_CTRL0(lxb_cmu_en, 0x1);
    
    while(0==RFLD_PLL_GLB_SIG(lxb_clk_rdy)) {
        cnt++;
        if(cnt>0x100000) break;
    }
    
    RMOD_PLL_GLB_CTRL(lxb_clksel, 0x0);
}

extern void set_spi_ctrl_divisor(u16_t clk_div, u16_t spif_mhz);

UTIL_FAR SECTION_CG
void cg_spif_pll_init(cg_config_t *c) {
#ifdef OTTO_FLASH_NAND_SPI
    set_spi_ctrl_divisor(c->spif_ctrl_div , 0);
    extern void set_spi_ctrl_latency(u16_t);
    // setting pipe_lat
    set_spi_ctrl_latency(c->snaf_pl);
#else
    RMOD_PLL_GLB_CTRL(spi_nor_clksel, 0x1);
    RMOD_PLL_LXB_CTRL0(cmu_div2_spi, c->spif_pll_div2);
    otto_sc.spif_div = c->spif_ctrl_div;
    // +1 for slower, and restore original freq. in next step
    set_spi_ctrl_divisor(c->spif_ctrl_div+((cg_info_query.dev_freq.spif_mhz>50)?1:0), 0);

    // for NOR+NAND, and this will set in u-Boot
    otto_sc.snaf_pl = c->snaf_pl;
    otto_sc.snaf_div = c->snaf_ctrl_div;

    // setting slow bit OC v.s SPIF
    RMOD_CMUCR_OC0(se_spif, c->oc_spif_slow);    
    RMOD_PLL_GLB_CTRL(spi_nor_clksel, 0x0);
#endif
    cg_udelay(50, cg_info_query.dev_freq.cpu_mhz);
}

extern u32_t mem_pll_init(cg_config_t *c);
UTIL_FAR SECTION_CG
void cg_config_apply(cg_config_t *c) {
    cg_cpu_pll_init(c); 
    cg_lx_pll_init(c);  
    cg_spif_pll_init(c);
    mem_pll_init(c);
    udelay(1000);
}

#endif  //PROJECT_ON_FPGA


u32_t cg_udelay(u32_t us, u32_t mhz) {
        u32_t loop_cnt = us*mhz/2;
        while (loop_cnt--) {
                ;
        }
        return loop_cnt;
}

SECTION_CG
u32_t cg_query_freq(u32_t dev_type) {
    return CG_QUERY_FREQUENCY(dev_type,(&cg_info_query.dev_freq));
}


SECTION_CG
void cg_copy_info_to_sram(void) {
    inline_memcpy(&cg_info_query, &cg_info_proj, sizeof(cg_info_t));
}

void cg_init(void) {
    int res=0;
  #ifdef PROJECT_ON_FPGA
    cg_slow_bit_check();
  #else
    cg_config_t c;

    res = cg_config_transform(&c);
    if (res!=PLL_SET_DONE) {
        printf("WW: CG Config Failed, res=0x%x \n", res);
        return;
    }

    cg_config_apply(&c);
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
//REG_INIT_FUNC(cg_result_decode, 20);
