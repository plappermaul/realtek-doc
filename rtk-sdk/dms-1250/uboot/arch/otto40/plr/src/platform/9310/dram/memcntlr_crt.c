#include <soc.h>
#include <dram/memcntlr.h>
#include <cg/cg.h>

#ifndef SECTION_CG
#define SECTION_CG
#endif
#ifndef SECTION_CG_INFO
#define SECTION_CG_INFO
#endif

#define _MEM_FREQ(ncode, fcode, pdiv)  (25*(ncode+3+(fcode/2048))/(pdiv))
#define _NCODE(freq, pdiv)      ((((freq)*(pdiv))/25)-3)
#define MEM_FREQ(ncode, fcode)  _MEM_FREQ(ncode, fcode, 1)
#define NCODE(freq)             _NCODE(freq, 1)
#define FCODE(freq)             ((((freq*1000)/25-(freq/25)*1000)*2046)+999)/1000

#ifndef PROJECT_ON_FPGA

#define MEM_PLL_CTL3_400_500    (0x40505254)
#define MEM_PLL_CTL3_500_600    (0x42D05374)
#define MEM_PLL_CTL3_600_725    (0x42D05475)
#define MEM_PLL_CTL3_725_850    (0x45505475)
//
extern cg_info_t cg_info_query SECTION_CG_INFO;
extern mc_info_t meminfo;
extern void dpi_set_2nd_cmd_group(void) UTIL_FAR;

UTIL_FAR SECTION_CG
u32_t mem_pll_init(void)
{
    u32_t n_code = NCODE(cg_info_query.dev_freq.mem_mhz);
    u32_t f_code = FCODE(cg_info_query.dev_freq.mem_mhz);
    u32_t pll_ctl3;

    u32_t f = MEM_FREQ(n_code, f_code);
    if (f>=400 && f<500) {
        pll_ctl3 = MEM_PLL_CTL3_400_500;
    } else if (f>=500 && f<600) {
        pll_ctl3 = MEM_PLL_CTL3_500_600;
    } else if (f>=600 && f<725) {
        pll_ctl3 = MEM_PLL_CTL3_600_725;
    } else if (f>=725 && f<850) {
        pll_ctl3 = MEM_PLL_CTL3_725_850;
    } else {
        return 1;
    }

    /* Enable clk for dcphy and pll reg */
    RMOD_DPI_DLL(reg_clken_dcphy, 0x1, reg_clken_pllreg, 0x1);

    /* PLL LDO Enable */
    RMOD_CRT_RST_CTL(rst_n, 1);
    udelay(1000);

    /* apply PLL setting */
    PLL_CTL3rv = pll_ctl3;
    RMOD_SSC3(dpi_n_code_t, n_code);
    RMOD_SSC2(dpi_f_code_t, f_code);

    /*  DCC initial value setting */
    DCC_DQS_0_T dcc_initv = {.v = DCC_DQS_0rv};
    dcc_initv.f.dpi_dcc_dqs_duty_sel = 0x0;
    dcc_initv.f.dpi_dcc_dqs_dsp_stop = 0x1;
    dcc_initv.f.dpi_dcc_dqs_div = 0x3;
    dcc_initv.f.dpi_dcc_dqs_int_gain = 0x1;
    dcc_initv.f.dpi_dcc_dqs_duty = 0x28;
    dcc_initv.f.dpi_dcc_dqs_duty_presetb = 0x3;
    dcc_initv.f.dpi_dcc_dqs_duty_preseta = 0x3;

    DCC_DQS_0rv = dcc_initv.v; //DQS0 close loop mode
    DCC_DQS_1rv = dcc_initv.v; //DQS1 close loop mode
    DCC_DQS_2rv = dcc_initv.v; //DQS2 close loop mode
    DCC_DQS_3rv = dcc_initv.v; //DQS3 close loop mode
    DCC_DQ_0rv  = dcc_initv.v; //DQ0 close loop mode
    DCC_DQ_1rv  = dcc_initv.v; //DQ1 close loop mode
    DCC_DQ_2rv  = dcc_initv.v; //DQ2 close loop mode
    DCC_DQ_3rv  = dcc_initv.v; //DQ3 close loop mode
    DCC_CK_0rv  = dcc_initv.v; //DCK close loop mode
    DCC_CK_1rv  = dcc_initv.v; //DCK close loop mode
    DCC_CMDrv   = dcc_initv.v; //CMD close loop mode
    DCC_CSrv    = dcc_initv.v; //CS close loop mode
    DCC_CS1rv   = dcc_initv.v; //CS1 close loop mode

    /* DPI initial value setting */
    #define __DQS_PI_0  (meminfo.dpi_opt->dqs_post_pi_0) //0x1
    #define __DQS_PI_1  (meminfo.dpi_opt->dqs_post_pi_1) //0x1
    #define __DQS_PI_2  (meminfo.dpi_opt->dqs_post_pi_2) //0x1
    #define __DQS_PI_3  (meminfo.dpi_opt->dqs_post_pi_3) //0x1
    #define __DQ_PI_0   (meminfo.dpi_opt->dq_post_pi_0)  //0x8
    #define __DQ_PI_1   (meminfo.dpi_opt->dq_post_pi_1)  //0x8
    #define __DQ_PI_2   (meminfo.dpi_opt->dq_post_pi_2)  //0x8
    #define __DQ_PI_3   (meminfo.dpi_opt->dq_post_pi_3)  //0x8
    #define __DCK_PI (meminfo.dpi_opt->dck_post_pi) //0x6
    #define __CS_PI  (meminfo.dpi_opt->cs_post_pi)  

    RMOD_PLL_PI0(dpi_post_pi_sel0,  __DCK_PI,\
                 dpi_post_pi_sel1,  __DCK_PI,\
                 dpi_post_pi_sel2,  __DQS_PI_0,\
                 dpi_post_pi_sel3,  __DQS_PI_1);
    RMOD_PLL_PI1(dpi_post_pi_sel4,  __DQS_PI_2,\
                 dpi_post_pi_sel5,  __DQS_PI_3,\
                 dpi_post_pi_sel6,  __DQ_PI_0);
    RMOD_PLL_PI2(dpi_post_pi_sel7,  __DQ_PI_1,\
                 dpi_post_pi_sel8,  __DQ_PI_2,\
                 dpi_post_pi_sel9,  __DQ_PI_3,\
                 dpi_post_pi_sel10, __CS_PI);    
    RMOD_PLL_PI3(dpi_post_pi_sel11, __CS_PI);

    u32_t oes_dck, oes_cs;  //oes_dqs, oes_dq,

    //oes_dqs = (__DQS_PI>15)?1:0;
    //oes_dq  = (__DQ0_PI >15)?1:0;
    oes_dck = (__DCK_PI>15)?1:0;
    oes_cs  = (__CS_PI >15)?1:0;

    RMOD_PLL_CTL1(dpi_oesync_op_sel_ck,    oes_dck, \
                  dpi_oesync_op_sel_dqs_0, (__DQS_PI_0>15)?1:0, \
                  dpi_oesync_op_sel_dqs_1, (__DQS_PI_1>15)?1:0, \
                  dpi_oesync_op_sel_dqs_2, (__DQS_PI_2>15)?1:0, \
                  dpi_oesync_op_sel_dqs_3, (__DQS_PI_3>15)?1:0, \
                  dpi_oesync_op_sel_dq_0,  (__DQ_PI_0>15)?1:0, \
                  dpi_oesync_op_sel_dq_1,  (__DQ_PI_1>15)?1:0, \
                  dpi_oesync_op_sel_dq_2,  (__DQ_PI_2>15)?1:0, \
                  dpi_oesync_op_sel_dq_3,  (__DQ_PI_3>15)?1:0, \
                  dpi_oesync_op_sel_cs,    oes_cs, \
                  dpi_oesync_op_sel_cs_1,  oes_cs, \
                  dpi_oesync_op_sel_ck_1,  oes_dck );

    /* delay DQ/DQS nT */
    #define __DQS_RD_STR  (meminfo.dpi_opt->dqs_rd_str_num)
    #define __DQ_RD_STR   (meminfo.dpi_opt->dq_rd_str_num)
    #define __CS_RD_STR  (meminfo.dpi_opt->cmd_ex_rd_str_num)
    #define __CMD_RD_STR  (meminfo.dpi_opt->cmd_rd_str_num)
    #define __RX_RD_STR   (meminfo.dpi_opt->rx_rd_str_num)
    RMOD_AFIFO_STR_0(dqs_rd_str_num_3, __DQS_RD_STR,\
                     dq_rd_str_num_3,  __DQ_RD_STR,\
                     dqs_rd_str_num_2, __DQS_RD_STR,\
                     dq_rd_str_num_2,  __DQ_RD_STR,\
                     dqs_rd_str_num_1, __DQS_RD_STR,\
                     dq_rd_str_num_1,  __DQ_RD_STR,\
                     dqs_rd_str_num_0, __DQS_RD_STR,\
                     dq_rd_str_num_0,  __DQ_RD_STR);
    RMOD_AFIFO_STR_SEL(cmd_rd_str_num_sel_pad_cs_n_1, 1, \
                       cmd_rd_str_num_sel_pad_cs_n, 1);
    RMOD_AFIFO_STR_1(cmd_ex_rd_str_num, __CS_RD_STR,\
                     cmd_rd_str_num, __CMD_RD_STR);
    RMOD_AFIFO_STR_2(rx_rd_str_num_3, __RX_RD_STR,\
                     rx_rd_str_num_2, __RX_RD_STR,\
                     rx_rd_str_num_1, __RX_RD_STR,\
                     rx_rd_str_num_0, __RX_RD_STR);

    u32_t dt = RFLD_PSR0(pin_dramtype); // before dram setting, read from strapped pin
    u8_t pmld3, pmd3;
    pmld3 = dt>>1;
    pmd3 = !(dt&0x1);

    // pin_mux DDR4: 0b00, DDR3: 0b01, LPDDR3: 0b10
    RMOD_CRT_CTL(pin_mux_ddr3, pmd3, pin_mux_lpddr3, pmld3, dpi_lpddr_en, pmld3, dpi_ddr3_mode, pmd3);

    RMOD_CRT_RST_CTL(ptr_rst_n, 0, rst_n, 0, pll_ssc_dig_rst_n, 1, pll_ldo_rst_n, 1);
    RMOD_CRT_RST_CTL(ptr_rst_n, 1, rst_n, 1, pll_ssc_dig_rst_n, 1, pll_ldo_rst_n, 1);
    udelay(1000);

    RMOD_SSC3(dpi_oc_en, 0x1);
    udelay(1000);
    RMOD_SSC3(dpi_oc_en, 0x0);  // ensure N_code F_code load successfully
    RMOD_SSC0(dpi_ssc_flag_init, 0x1);

    /* Turn on clock flow */
    RMOD_PLL_CTL4(dpi_dly_sel, 0, dpi_dly_sel_ck1, 0, dpi_en_post_pi, 0x3FFF);
    RMOD_PLL_CTL0(dpi_pll_clk_en, 0x1FFF, dpi_mck_clk_en, 0x1FFF);
    RMOD_PLL_CTL1(dpi_clk_oe, 0x7);
    u32_t cnt=0;
    while(1==RFLD_DPI_DLL(pll_stable)) {
        if (++cnt>0x100000) break;
    }

    REG32(0xBB000410) = 0x1;    // level shift enable(3.3v)
    udelay(1000);

    RMOD_CRT_CTL(force_rstn, 0);
    udelay(1000);
    
    // set CLK1 and 2nd cmd group signals
    dpi_set_2nd_cmd_group();

    // setting data pre (better DQ timing) and 2T preamble
    RMOD_DPI_CTRL_0(data_pre, meminfo.dpi_opt->data_pre,    \
                    _2t_wr_pre, meminfo.cntlr_opt->twpre,   \
                    _2t_rd_pre, meminfo.cntlr_opt->trpre);
    RMOD_PAD_BUS_0(en_dcc, 1, pd_ck, 0);
    udelay(1000);

    return 0;
}

#endif
