#ifndef _REG_MAP_DPI_CRT_H_
#define _REG_MAP_DPI_CRT_H_

/*DPI CRT*/
typedef union {
	struct {
		unsigned int mbz_0:1; //0x0
		unsigned int mck_cke_sel:1; //0x0
		unsigned int clk_ten_value:1; //0x0
		unsigned int clk_ten_mode:1; //0x0
		unsigned int force_rstn:1; //0x1
		unsigned int rstn_value:1; //0x1
		unsigned int fifo_rst_ini_tx:3; //0x0
		unsigned int fifo_rst_ini_rx:3; //0x0
		unsigned int dmy:13; //0x0
		unsigned int pin_mux_lpddr3:1; //0x0
		unsigned int pin_mux_ddr3:1; //0x1
		unsigned int pin_mux_mode:1; //0x0
		unsigned int dpi_lpddr_en:1; //0x0
		unsigned int dpi_dat_rate_sel:1; //0x0
		unsigned int dpi_ddr2_en:1; //0x0
		unsigned int dpi_ddr3_mode:1; //0x1
	} f;
	unsigned int v;
} CRT_CTL_T;
#define CRT_CTLar (0xB8142000)
#define CRT_CTLdv (0x0C000021)
#define CRT_CTLrv RVAL(CRT_CTL)
#define RMOD_CRT_CTL(...) RMOD(CRT_CTL, __VA_ARGS__)
#define RIZS_CRT_CTL(...) RIZS(CRT_CTL, __VA_ARGS__)
#define RFLD_CRT_CTL(fld) RFLD(CRT_CTL, fld)

typedef union {
	struct {
		unsigned int mbz_0:3; //0x0
		unsigned int dpi_pll_clk_en:13; //0x0
		unsigned int mbz_1:3; //0x0
		unsigned int dpi_mck_clk_en:13; //0x0
	} f;
	unsigned int v;
} PLL_CTL0_T;
#define PLL_CTL0ar (0xB8142004)
#define PLL_CTL0dv (0x00000000)
#define PLL_CTL0rv RVAL(PLL_CTL0)
#define RMOD_PLL_CTL0(...) RMOD(PLL_CTL0, __VA_ARGS__)
#define RIZS_PLL_CTL0(...) RIZS(PLL_CTL0, __VA_ARGS__)
#define RFLD_PLL_CTL0(fld) RFLD(PLL_CTL0, fld)

typedef union {
	struct {
		unsigned int mbz_0:13; //0x0
		unsigned int dpi_clk_oe:3; //0x0
		unsigned int mbz_1:3; //0x0
		unsigned int dpi_oesync_op_sel_ck_1:1; //0x0
		unsigned int dpi_oesync_op_sel_cs_1:1; //0x0
		unsigned int dpi_oesync_op_sel_cs:1; //0x0
		unsigned int dpi_oesync_op_sel_dq_3:1; //0x0
		unsigned int dpi_oesync_op_sel_dq_2:1; //0x0
		unsigned int dpi_oesync_op_sel_dq_1:1; //0x0
		unsigned int dpi_oesync_op_sel_dq_0:1; //0x0
		unsigned int dpi_oesync_op_sel_dqs_3:1; //0x0
		unsigned int dpi_oesync_op_sel_dqs_2:1; //0x0
		unsigned int dpi_oesync_op_sel_dqs_1:1; //0x0
		unsigned int dpi_oesync_op_sel_dqs_0:1; //0x0
		unsigned int dpi_oesync_op_sel_cmd:1; //0x0
		unsigned int dpi_oesync_op_sel_ck:1; //0x0
	} f;
	unsigned int v;
} PLL_CTL1_T;
#define PLL_CTL1ar (0xB8142008)
#define PLL_CTL1dv (0x00000000)
#define PLL_CTL1rv RVAL(PLL_CTL1)
#define RMOD_PLL_CTL1(...) RMOD(PLL_CTL1, __VA_ARGS__)
#define RIZS_PLL_CTL1(...) RIZS(PLL_CTL1, __VA_ARGS__)
#define RFLD_PLL_CTL1(fld) RFLD(PLL_CTL1, fld)

typedef union {
	struct {
		unsigned int mbz_0:3; //0x0
		unsigned int dpi_prbs_en:1; //0x0
		unsigned int dpi_nc_sel:2; //0x0
		unsigned int dpi_nc_clk_div_sel:2; //0x0
		unsigned int dpi_nc_prbs13_seed:13; //0x0
		unsigned int dpi_nc_prbs13_en:1; //0x0
		unsigned int dpi_nc_prbs13_invs:1; //0x0
		unsigned int dpi_nc_prbs13_max:1; //0x0
		unsigned int dpi_pll_dum:8; //0xF
	} f;
	unsigned int v;
} PLL_CTL2_T;
#define PLL_CTL2ar (0xB814200C)
#define PLL_CTL2dv (0x0000000F)
#define PLL_CTL2rv RVAL(PLL_CTL2)
#define RMOD_PLL_CTL2(...) RMOD(PLL_CTL2, __VA_ARGS__)
#define RIZS_PLL_CTL2(...) RIZS(PLL_CTL2, __VA_ARGS__)
#define RFLD_PLL_CTL2(fld) RFLD(PLL_CTL2, fld)

typedef union {
	struct {
		unsigned int mbz_0:3; //0x0
		unsigned int dpi_post_pi_sel3:5; //0x0
		unsigned int mbz_1:3; //0x0
		unsigned int dpi_post_pi_sel2:5; //0x0
		unsigned int mbz_2:3; //0x0
		unsigned int dpi_post_pi_sel1:5; //0x0
		unsigned int mbz_3:3; //0x0
		unsigned int dpi_post_pi_sel0:5; //0x0
	} f;
	unsigned int v;
} PLL_PI0_T;
#define PLL_PI0ar (0xB8142010)
#define PLL_PI0dv (0x00000000)
#define PLL_PI0rv RVAL(PLL_PI0)
#define RMOD_PLL_PI0(...) RMOD(PLL_PI0, __VA_ARGS__)
#define RIZS_PLL_PI0(...) RIZS(PLL_PI0, __VA_ARGS__)
#define RFLD_PLL_PI0(fld) RFLD(PLL_PI0, fld)

typedef union {
	struct {
		unsigned int mbz_0:3; //0x0
		unsigned int dpi_post_pi_sel:5; //0x0
		unsigned int mbz_1:3; //0x0
		unsigned int dpi_post_pi_sel6:5; //0x0
		unsigned int mbz_2:3; //0x0
		unsigned int dpi_post_pi_sel5:5; //0x0
		unsigned int mbz_3:3; //0x0
		unsigned int dpi_post_pi_sel4:5; //0x0
	} f;
	unsigned int v;
} PLL_PI1_T;
#define PLL_PI1ar (0xB8142014)
#define PLL_PI1dv (0x00000000)
#define PLL_PI1rv RVAL(PLL_PI1)
#define RMOD_PLL_PI1(...) RMOD(PLL_PI1, __VA_ARGS__)
#define RIZS_PLL_PI1(...) RIZS(PLL_PI1, __VA_ARGS__)
#define RFLD_PLL_PI1(fld) RFLD(PLL_PI1, fld)

typedef union {
	struct {
		unsigned int dpi_v11_ldo_vsel:3; //0x2
		unsigned int dpi_ckref_sel:1; //0x0
		unsigned int dpi_post_pi_rs:1; //0x0
		unsigned int dpi_post_pi_rl:2; //0x2
		unsigned int dpi_post_pi_bias:2; //0x2
		unsigned int dpi_pll_sel_cpmode:1; //0x1
		unsigned int dpi_pll_ldo_vsel:3; //0x2
		unsigned int dpi_pll_dbug_en:1; //0x0
		unsigned int dpi_pdiv:2; //0x0
		unsigned int dpi_lpf_sr:3; //0x3
		unsigned int dpi_lpf_cp:1; //0x1
		unsigned int dpi_loop_pi_isel:4; //0x6
		unsigned int dpi_icp:4; //0xC
		unsigned int mbz_0:1; //0x0
		unsigned int dpi_cco_kvco:1; //0x1
		unsigned int dpi_cco_band:2; //0x1
	} f;
	unsigned int v;
} PLL_CTL3_T;
#define PLL_CTL3ar (0xB8142018)
#define PLL_CTL3dv (0x455076C5)
#define PLL_CTL3rv RVAL(PLL_CTL3)
#define RMOD_PLL_CTL3(...) RMOD(PLL_CTL3, __VA_ARGS__)
#define RIZS_PLL_CTL3(...) RIZS(PLL_CTL3, __VA_ARGS__)
#define RFLD_PLL_CTL3(fld) RFLD(PLL_CTL3, fld)

typedef union {
	struct {
		unsigned int mbz_0:18; //0x0
		unsigned int dpi_oc_done_delay:6; //0x3F
		unsigned int dpi_hs_oc_stop_diff:2; //0x1
		unsigned int dpi_sel_oc_mode:2; //0x0
		unsigned int dpi_ssc_flag_init:1; //0x0
		unsigned int dpi_order:1; //0x0
		unsigned int dpi_en_ssc:1; //0x0
		unsigned int dpi_bypass_pi:1; //0x0
	} f;
	unsigned int v;
} SSC0_T;
#define SSC0ar (0xB814201C)
#define SSC0dv (0x00003F40)
#define SSC0rv RVAL(SSC0)
#define RMOD_SSC0(...) RMOD(SSC0, __VA_ARGS__)
#define RIZS_SSC0(...) RIZS(SSC0, __VA_ARGS__)
#define RFLD_SSC0(fld) RFLD(SSC0, fld)

typedef union {
	struct {
		unsigned int mbz_0:8; //0x0
		unsigned int dpi_gran_auto_rst:1; //0x1
		unsigned int dpi_dot_gran:3; //0x4
		unsigned int mbz_1:1; //0x0
		unsigned int dpi_gran_set:19; //0x21FB
	} f;
	unsigned int v;
} SSC1_T;
#define SSC1ar (0xB8142020)
#define SSC1dv (0x00C021FB)
#define SSC1rv RVAL(SSC1)
#define RMOD_SSC1(...) RMOD(SSC1, __VA_ARGS__)
#define RIZS_SSC1(...) RIZS(SSC1, __VA_ARGS__)
#define RFLD_SSC1(fld) RFLD(SSC1, fld)

typedef union {
	struct {
		unsigned int mbz_0:5; //0x0
		unsigned int dpi_f_code_t:11; //0x50A
		unsigned int mbz_1:5; //0x0
		unsigned int dpi_f_code:11; //0x5ED
	} f;
	unsigned int v;
} SSC2_T;
#define SSC2ar (0xB8142024)
#define SSC2dv (0x050A05ED)
#define SSC2rv RVAL(SSC2)
#define RMOD_SSC2(...) RMOD(SSC2, __VA_ARGS__)
#define RIZS_SSC2(...) RIZS(SSC2, __VA_ARGS__)
#define RFLD_SSC2(fld) RFLD(SSC2, fld)

typedef union {
	struct {
		unsigned int mbz_0:5; //0x0
		unsigned int dpi_oc_en:1; //0x0
		unsigned int dpi_oc_step_set:10; //0x28
		unsigned int dpi_n_code_t:8; //0x1A
		unsigned int dpi_n_code:8; //0x19
	} f;
	unsigned int v;
} SSC3_T;
#define SSC3ar (0xB8142028)
#define SSC3dv (0x00281A19)
#define SSC3rv RVAL(SSC3)
#define RMOD_SSC3(...) RMOD(SSC3, __VA_ARGS__)
#define RIZS_SSC3(...) RIZS(SSC3, __VA_ARGS__)
#define RFLD_SSC3(fld) RFLD(SSC3, fld)

typedef union {
	struct {
		unsigned int mbz_0:18; //0x0
		unsigned int dpi_time2_rst_width:2; //0x1
		unsigned int mbz_1:2; //0x0
		unsigned int dpi_time_rdy_ckout:2; //0x2
		unsigned int mbz_2:1; //0x0
		unsigned int dpi_time0_ck:3; //0x3
		unsigned int mbz_3:1; //0x0
		unsigned int dpi_f390k:2; //0x1
		unsigned int dpi_wd_enable:1; //0x0
	} f;
	unsigned int v;
} WDOG_T;
#define WDOGar (0xB814202C)
#define WDOGdv (0x00001232)
#define WDOGrv RVAL(WDOG)
#define RMOD_WDOG(...) RMOD(WDOG, __VA_ARGS__)
#define RIZS_WDOG(...) RIZS(WDOG, __VA_ARGS__)
#define RFLD_WDOG(fld) RFLD(WDOG, fld)

typedef union {
	struct {
		unsigned int dpi_dcc_cmd_dsp_dbg_sel:1; //0x0
		unsigned int mbz_0:1; //0x0
		unsigned int dpi_dcc_cmd_duty_sel:1; //0x1
		unsigned int dpi_dcc_cmd_dsp_stop:1; //0x1
		unsigned int mbz_1:1; //0x0
		unsigned int dpi_dcc_cmd_div:3; //0x3
		unsigned int mbz_2:3; //0x0
		unsigned int dpi_dcc_cmd_int_gain:5; //0x5
		unsigned int mbz_3:1; //0x0
		unsigned int dpi_dcc_cmd_duty:7; //0x28
		unsigned int mbz_4:1; //0x0
		unsigned int dpi_dcc_cmd_duty_presetb:3; //0x0
		unsigned int mbz_5:1; //0x0
		unsigned int dpi_dcc_cmd_duty_preseta:3; //0x0
	} f;
	unsigned int v;
} DCC_CMD_T;
#define DCC_CMDar (0xB8142030)
#define DCC_CMDdv (0x33052800)
#define DCC_CMDrv RVAL(DCC_CMD)
#define RMOD_DCC_CMD(...) RMOD(DCC_CMD, __VA_ARGS__)
#define RIZS_DCC_CMD(...) RIZS(DCC_CMD, __VA_ARGS__)
#define RFLD_DCC_CMD(fld) RFLD(DCC_CMD, fld)

typedef union {
	struct {
		unsigned int mbz_0:2; //0x0
		unsigned int dpi_dcc_dqs_duty_sel:1; //0x1
		unsigned int dpi_dcc_dqs_dsp_stop:1; //0x1
		unsigned int mbz_1:1; //0x0
		unsigned int dpi_dcc_dqs_div:3; //0x3
		unsigned int mbz_2:3; //0x0
		unsigned int dpi_dcc_dqs_int_gain:5; //0x5
		unsigned int mbz_3:1; //0x0
		unsigned int dpi_dcc_dqs_duty:7; //0x28
		unsigned int mbz_4:1; //0x0
		unsigned int dpi_dcc_dqs_duty_presetb:3; //0x0
		unsigned int mbz_5:1; //0x0
		unsigned int dpi_dcc_dqs_duty_preseta:3; //0x0
	} f;
	unsigned int v;
} DCC_DQS_0_T;
#define DCC_DQS_0ar (0xB8142034)
#define DCC_DQS_0dv (0x33052800)
#define DCC_DQS_0rv RVAL(DCC_DQS_0)
#define RMOD_DCC_DQS_0(...) RMOD(DCC_DQS_0, __VA_ARGS__)
#define RIZS_DCC_DQS_0(...) RIZS(DCC_DQS_0, __VA_ARGS__)
#define RFLD_DCC_DQS_0(fld) RFLD(DCC_DQS_0, fld)

typedef union {
	struct {
		unsigned int mbz_0:2; //0x0
		unsigned int dpi_dcc_dqs_duty_sel:1; //0x1
		unsigned int dpi_dcc_dqs_dsp_stop:1; //0x1
		unsigned int mbz_1:1; //0x0
		unsigned int dpi_dcc_dqs_div:3; //0x3
		unsigned int mbz_2:3; //0x0
		unsigned int dpi_dcc_dqs_int_gain:5; //0x5
		unsigned int mbz_3:1; //0x0
		unsigned int dpi_dcc_dqs_duty:7; //0x28
		unsigned int mbz_4:1; //0x0
		unsigned int dpi_dcc_dqs_duty_presetb:3; //0x0
		unsigned int mbz_5:1; //0x0
		unsigned int dpi_dcc_dqs_duty_preseta:3; //0x0
	} f;
	unsigned int v;
} DCC_DQS_1_T;
#define DCC_DQS_1ar (0xB8142038)
#define DCC_DQS_1dv (0x33052800)
#define DCC_DQS_1rv RVAL(DCC_DQS_1)
#define RMOD_DCC_DQS_1(...) RMOD(DCC_DQS_1, __VA_ARGS__)
#define RIZS_DCC_DQS_1(...) RIZS(DCC_DQS_1, __VA_ARGS__)
#define RFLD_DCC_DQS_1(fld) RFLD(DCC_DQS_1, fld)

typedef union {
	struct {
		unsigned int mbz_0:2; //0x0
		unsigned int dpi_dcc_dqs_duty_sel:1; //0x1
		unsigned int dpi_dcc_dqs_dsp_stop:1; //0x1
		unsigned int mbz_1:1; //0x0
		unsigned int dpi_dcc_dqs_div:3; //0x3
		unsigned int mbz_2:3; //0x0
		unsigned int dpi_dcc_dqs_int_gain:5; //0x5
		unsigned int mbz_3:1; //0x0
		unsigned int dpi_dcc_dqs_duty:7; //0x28
		unsigned int mbz_4:1; //0x0
		unsigned int dpi_dcc_dqs_duty_presetb:3; //0x0
		unsigned int mbz_5:1; //0x0
		unsigned int dpi_dcc_dqs_duty_preseta:3; //0x0
	} f;
	unsigned int v;
} DCC_DQS_2_T;
#define DCC_DQS_2ar (0xB814203C)
#define DCC_DQS_2dv (0x33052800)
#define DCC_DQS_2rv RVAL(DCC_DQS_2)
#define RMOD_DCC_DQS_2(...) RMOD(DCC_DQS_2, __VA_ARGS__)
#define RIZS_DCC_DQS_2(...) RIZS(DCC_DQS_2, __VA_ARGS__)
#define RFLD_DCC_DQS_2(fld) RFLD(DCC_DQS_2, fld)

typedef union {
	struct {
		unsigned int mbz_0:2; //0x0
		unsigned int dpi_dcc_dqs_duty_sel:1; //0x1
		unsigned int dpi_dcc_dqs_dsp_stop:1; //0x1
		unsigned int mbz_1:1; //0x0
		unsigned int dpi_dcc_dqs_div:3; //0x3
		unsigned int mbz_2:3; //0x0
		unsigned int dpi_dcc_dqs_int_gain:5; //0x5
		unsigned int mbz_3:1; //0x0
		unsigned int dpi_dcc_dqs_duty:7; //0x28
		unsigned int mbz_4:1; //0x0
		unsigned int dpi_dcc_dqs_duty_presetb:3; //0x0
		unsigned int mbz_5:1; //0x0
		unsigned int dpi_dcc_dqs_duty_preseta:3; //0x0
	} f;
	unsigned int v;
} DCC_DQS_3_T;
#define DCC_DQS_3ar (0xB8142040)
#define DCC_DQS_3dv (0x33052800)
#define DCC_DQS_3rv RVAL(DCC_DQS_3)
#define RMOD_DCC_DQS_3(...) RMOD(DCC_DQS_3, __VA_ARGS__)
#define RIZS_DCC_DQS_3(...) RIZS(DCC_DQS_3, __VA_ARGS__)
#define RFLD_DCC_DQS_3(fld) RFLD(DCC_DQS_3, fld)

typedef union {
	struct {
		unsigned int mbz_0:2; //0x0
		unsigned int dpi_dcc_dq_duty_sel:1; //0x1
		unsigned int dpi_dcc_dq_dsp_stop:1; //0x1
		unsigned int mbz_1:1; //0x0
		unsigned int dpi_dcc_dq_div:3; //0x3
		unsigned int mbz_2:3; //0x0
		unsigned int dpi_dcc_dq_int_gain:5; //0x5
		unsigned int mbz_3:1; //0x0
		unsigned int dpi_dcc_dq_duty:7; //0x28
		unsigned int mbz_4:1; //0x0
		unsigned int dpi_dcc_dq_duty_presetb:3; //0x0
		unsigned int mbz_5:1; //0x0
		unsigned int dpi_dcc_dq_duty_preseta:3; //0x0
	} f;
	unsigned int v;
} DCC_DQ_0_T;
#define DCC_DQ_0ar (0xB8142044)
#define DCC_DQ_0dv (0x33052800)
#define DCC_DQ_0rv RVAL(DCC_DQ_0)
#define RMOD_DCC_DQ_0(...) RMOD(DCC_DQ_0, __VA_ARGS__)
#define RIZS_DCC_DQ_0(...) RIZS(DCC_DQ_0, __VA_ARGS__)
#define RFLD_DCC_DQ_0(fld) RFLD(DCC_DQ_0, fld)

typedef union {
	struct {
		unsigned int mbz_0:2; //0x0
		unsigned int dpi_dcc_dq_duty_sel:1; //0x1
		unsigned int dpi_dcc_dq_dsp_stop:1; //0x1
		unsigned int mbz_1:1; //0x0
		unsigned int dpi_dcc_dq_div:3; //0x3
		unsigned int mbz_2:3; //0x0
		unsigned int dpi_dcc_dq_int_gain:5; //0x5
		unsigned int mbz_3:1; //0x0
		unsigned int dpi_dcc_dq_duty:7; //0x28
		unsigned int mbz_4:1; //0x0
		unsigned int dpi_dcc_dq_duty_presetb:3; //0x0
		unsigned int mbz_5:1; //0x0
		unsigned int dpi_dcc_dq_duty_preseta:3; //0x0
	} f;
	unsigned int v;
} DCC_DQ_1_T;
#define DCC_DQ_1ar (0xB8142048)
#define DCC_DQ_1dv (0x33052800)
#define DCC_DQ_1rv RVAL(DCC_DQ_1)
#define RMOD_DCC_DQ_1(...) RMOD(DCC_DQ_1, __VA_ARGS__)
#define RIZS_DCC_DQ_1(...) RIZS(DCC_DQ_1, __VA_ARGS__)
#define RFLD_DCC_DQ_1(fld) RFLD(DCC_DQ_1, fld)

typedef union {
	struct {
		unsigned int mbz_0:2; //0x0
		unsigned int dpi_dcc_dq_duty_sel:1; //0x1
		unsigned int dpi_dcc_dq_dsp_stop:1; //0x1
		unsigned int mbz_1:1; //0x0
		unsigned int dpi_dcc_dq_div:3; //0x3
		unsigned int mbz_2:3; //0x0
		unsigned int dpi_dcc_dq_int_gain:5; //0x5
		unsigned int mbz_3:1; //0x0
		unsigned int dpi_dcc_dq_duty:7; //0x28
		unsigned int mbz_4:1; //0x0
		unsigned int dpi_dcc_dq_duty_presetb:3; //0x0
		unsigned int mbz_5:1; //0x0
		unsigned int dpi_dcc_dq_duty_preseta:3; //0x0
	} f;
	unsigned int v;
} DCC_DQ_2_T;
#define DCC_DQ_2ar (0xB814204C)
#define DCC_DQ_2dv (0x33052800)
#define DCC_DQ_2rv RVAL(DCC_DQ_2)
#define RMOD_DCC_DQ_2(...) RMOD(DCC_DQ_2, __VA_ARGS__)
#define RIZS_DCC_DQ_2(...) RIZS(DCC_DQ_2, __VA_ARGS__)
#define RFLD_DCC_DQ_2(fld) RFLD(DCC_DQ_2, fld)

typedef union {
	struct {
		unsigned int mbz_0:2; //0x0
		unsigned int dpi_dcc_dq_duty_sel:1; //0x1
		unsigned int dpi_dcc_dq_dsp_stop:1; //0x1
		unsigned int mbz_1:1; //0x0
		unsigned int dpi_dcc_dq_div:3; //0x3
		unsigned int mbz_2:3; //0x0
		unsigned int dpi_dcc_dq_int_gain:5; //0x5
		unsigned int mbz_3:1; //0x0
		unsigned int dpi_dcc_dq_duty:7; //0x28
		unsigned int mbz_4:1; //0x0
		unsigned int dpi_dcc_dq_duty_presetb:3; //0x0
		unsigned int mbz_5:1; //0x0
		unsigned int dpi_dcc_dq_duty_preseta:3; //0x0
	} f;
	unsigned int v;
} DCC_DQ_3_T;
#define DCC_DQ_3ar (0xB8142050)
#define DCC_DQ_3dv (0x33052800)
#define DCC_DQ_3rv RVAL(DCC_DQ_3)
#define RMOD_DCC_DQ_3(...) RMOD(DCC_DQ_3, __VA_ARGS__)
#define RIZS_DCC_DQ_3(...) RIZS(DCC_DQ_3, __VA_ARGS__)
#define RFLD_DCC_DQ_3(fld) RFLD(DCC_DQ_3, fld)

typedef union {
	struct {
		unsigned int mbz_0:2; //0x0
		unsigned int dpi_dcc_ck_duty_sel:1; //0x1
		unsigned int dpi_dcc_ck_dsp_stop:1; //0x1
		unsigned int mbz_1:1; //0x0
		unsigned int dpi_dcc_ck_div:3; //0x3
		unsigned int mbz_2:3; //0x0
		unsigned int dpi_dcc_ck_int_gain:5; //0x5
		unsigned int mbz_3:1; //0x0
		unsigned int dpi_dcc_ck_duty:7; //0x28
		unsigned int mbz_4:1; //0x0
		unsigned int dpi_dcc_ck_duty_presetb:3; //0x0
		unsigned int mbz_5:1; //0x0
		unsigned int dpi_dcc_ck_duty_preseta:3; //0x0
	} f;
	unsigned int v;
} DCC_CK_0_T;
#define DCC_CK_0ar (0xB8142054)
#define DCC_CK_0dv (0x33052800)
#define DCC_CK_0rv RVAL(DCC_CK_0)
#define RMOD_DCC_CK_0(...) RMOD(DCC_CK_0, __VA_ARGS__)
#define RIZS_DCC_CK_0(...) RIZS(DCC_CK_0, __VA_ARGS__)
#define RFLD_DCC_CK_0(fld) RFLD(DCC_CK_0, fld)

typedef union {
	struct {
		unsigned int mbz_0:2; //0x0
		unsigned int dpi_dcc_ck_duty_sel:1; //0x1
		unsigned int dpi_dcc_ck_dsp_stop:1; //0x1
		unsigned int mbz_1:1; //0x0
		unsigned int dpi_dcc_ck_div:3; //0x3
		unsigned int mbz_2:3; //0x0
		unsigned int dpi_dcc_ck_int_gain:5; //0x5
		unsigned int mbz_3:1; //0x0
		unsigned int dpi_dcc_ck_duty:7; //0x28
		unsigned int mbz_4:1; //0x0
		unsigned int dpi_dcc_ck_duty_presetb:3; //0x0
		unsigned int mbz_5:1; //0x0
		unsigned int dpi_dcc_ck_duty_preseta:3; //0x0
	} f;
	unsigned int v;
} DCC_CK_1_T;
#define DCC_CK_1ar (0xB8142058)
#define DCC_CK_1dv (0x33052800)
#define DCC_CK_1rv RVAL(DCC_CK_1)
#define RMOD_DCC_CK_1(...) RMOD(DCC_CK_1, __VA_ARGS__)
#define RIZS_DCC_CK_1(...) RIZS(DCC_CK_1, __VA_ARGS__)
#define RFLD_DCC_CK_1(fld) RFLD(DCC_CK_1, fld)

typedef union {
	struct {
		unsigned int dmy:32; //0x0
	} f;
	unsigned int v;
} CRT_DMY_T;
#define CRT_DMYar (0xB814207C)
#define CRT_DMYdv (0x00000000)
#define CRT_DMYrv RVAL(CRT_DMY)
#define RMOD_CRT_DMY(...) RMOD(CRT_DMY, __VA_ARGS__)
#define RIZS_CRT_DMY(...) RIZS(CRT_DMY, __VA_ARGS__)
#define RFLD_CRT_DMY(fld) RFLD(CRT_DMY, fld)

typedef union {
	struct {
		unsigned int mbz_0:1; //0x0
		unsigned int dqs_rd_str_num_3:3; //0x1
		unsigned int mbz_1:1; //0x0
		unsigned int dq_rd_str_num_3:3; //0x1
		unsigned int mbz_2:1; //0x0
		unsigned int dqs_rd_str_num_2:3; //0x1
		unsigned int mbz_3:1; //0x0
		unsigned int dq_rd_str_num_2:3; //0x1
		unsigned int mbz_4:1; //0x0
		unsigned int dqs_rd_str_num_1:3; //0x1
		unsigned int mbz_5:1; //0x0
		unsigned int dq_rd_str_num_1:3; //0x1
		unsigned int mbz_6:1; //0x0
		unsigned int dqs_rd_str_num_0:3; //0x1
		unsigned int mbz_7:1; //0x0
		unsigned int dq_rd_str_num_0:3; //0x1
	} f;
	unsigned int v;
} AFIFO_STR_0_T;
#define AFIFO_STR_0ar (0xB8142080)
#define AFIFO_STR_0dv (0x11111111)
#define AFIFO_STR_0rv RVAL(AFIFO_STR_0)
#define RMOD_AFIFO_STR_0(...) RMOD(AFIFO_STR_0, __VA_ARGS__)
#define RIZS_AFIFO_STR_0(...) RIZS(AFIFO_STR_0, __VA_ARGS__)
#define RFLD_AFIFO_STR_0(fld) RFLD(AFIFO_STR_0, fld)

typedef union {
	struct {
		unsigned int mbz_0:15; //0x0
		unsigned int cmd_rd_str_num_sel_h_odt_1:1; //0x0
		unsigned int cmd_rd_str_num_sel_h_cke_1:1; //0x0
		unsigned int cmd_rd_str_num_sel_h_alert_1:1; //0x0
		unsigned int cmd_rd_str_num_sel_h_par_1:1; //0x0
		unsigned int cmd_rd_str_num_sel_h_ddr4_cke:1; //0x0
		unsigned int cmd_rd_str_num_sel_h_act_n:1; //0x0
		unsigned int cmd_rd_str_num_sel_h_bg0:1; //0x0
		unsigned int cmd_rd_str_num_sel_h_par:1; //0x0
		unsigned int cmd_rd_str_num_sel_h_ten:1; //0x0
		unsigned int mbz_1:1; //0x0
		unsigned int cmd_ex_rd_str_num:3; //0x2
		unsigned int mbz_2:1; //0x0
		unsigned int cmd_rd_str_num:3; //0x1
	} f;
	unsigned int v;
} AFIFO_STR_1_T;
#define AFIFO_STR_1ar (0xB8142084)
#define AFIFO_STR_1dv (0x00000021)
#define AFIFO_STR_1rv RVAL(AFIFO_STR_1)
#define RMOD_AFIFO_STR_1(...) RMOD(AFIFO_STR_1, __VA_ARGS__)
#define RIZS_AFIFO_STR_1(...) RIZS(AFIFO_STR_1, __VA_ARGS__)
#define RFLD_AFIFO_STR_1(fld) RFLD(AFIFO_STR_1, fld)

typedef union {
	struct {
		unsigned int mbz_0:17; //0x0
		unsigned int rx_rd_str_num_3:3; //0x1
		unsigned int mbz_1:1; //0x0
		unsigned int rx_rd_str_num_2:3; //0x1
		unsigned int mbz_2:1; //0x0
		unsigned int rx_rd_str_num_1:3; //0x1
		unsigned int mbz_3:1; //0x0
		unsigned int rx_rd_str_num_0:3; //0x1
	} f;
	unsigned int v;
} AFIFO_STR_2_T;
#define AFIFO_STR_2ar (0xB8142088)
#define AFIFO_STR_2dv (0x00001111)
#define AFIFO_STR_2rv RVAL(AFIFO_STR_2)
#define RMOD_AFIFO_STR_2(...) RMOD(AFIFO_STR_2, __VA_ARGS__)
#define RIZS_AFIFO_STR_2(...) RIZS(AFIFO_STR_2, __VA_ARGS__)
#define RFLD_AFIFO_STR_2(fld) RFLD(AFIFO_STR_2, fld)

typedef union {
	struct {
		unsigned int mbz_0:28; //0x0
		unsigned int ptr_rst_n:1; //0x1
		unsigned int rst_n:1; //0x0
		unsigned int pll_ssc_dig_rst_n:1; //0x0
		unsigned int pll_ldo_rst_n:1; //0x1
	} f;
	unsigned int v;
} CRT_RST_CTL_T;
#define CRT_RST_CTLar (0xB814208C)
#define CRT_RST_CTLdv (0x00000009)
#define CRT_RST_CTLrv RVAL(CRT_RST_CTL)
#define RMOD_CRT_RST_CTL(...) RMOD(CRT_RST_CTL, __VA_ARGS__)
#define RIZS_CRT_RST_CTL(...) RIZS(CRT_RST_CTL, __VA_ARGS__)
#define RFLD_CRT_RST_CTL(fld) RFLD(CRT_RST_CTL, fld)

typedef union {
	struct {
		unsigned int cmd_rd_str_num_sel_pad_alert:1; //0x0
		unsigned int cmd_rd_str_num_sel_pad_ddr4_cs_n:1; //0x0
		unsigned int cmd_rd_str_num_sel_pad_cs_n_1:1; //0x0
		unsigned int cmd_rd_str_num_sel_pad_cs_n:1; //0x0
		unsigned int cmd_rd_str_num_sel_pad_odt:1; //0x0
		unsigned int cmd_rd_str_num_sel_pad_cke:1; //0x0
		unsigned int cmd_rd_str_num_sel_pad_cas_n:1; //0x0
		unsigned int cmd_rd_str_num_sel_pad_ras_n:1; //0x0
		unsigned int cmd_rd_str_num_sel_pad_we_n:1; //0x0
		unsigned int cmd_rd_str_num_sel_pad_reset_n:1; //0x0
		unsigned int cmd_rd_str_num_sel_pad_bank_n:3; //0x0
		unsigned int cmd_rd_str_num_sel_pad_addr_ext:2; //0x0
		unsigned int cmd_rd_str_num_sel_pad_addr:17; //0x0
	} f;
	unsigned int v;
} AFIFO_STR_SEL_T;
#define AFIFO_STR_SELar (0xB8142094)
#define AFIFO_STR_SELdv (0x00000000)
#define AFIFO_STR_SELrv RVAL(AFIFO_STR_SEL)
#define RMOD_AFIFO_STR_SEL(...) RMOD(AFIFO_STR_SEL, __VA_ARGS__)
#define RIZS_AFIFO_STR_SEL(...) RIZS(AFIFO_STR_SEL, __VA_ARGS__)
#define RFLD_AFIFO_STR_SEL(fld) RFLD(AFIFO_STR_SEL, fld)

typedef union {
	struct {
		unsigned int mbz_0:3; //0x0
		unsigned int dpi_post_pi_sel10:5; //0x0
		unsigned int mbz_1:3; //0x0
		unsigned int dpi_post_pi_sel9:5; //0x0
		unsigned int mbz_2:3; //0x0
		unsigned int dpi_post_pi_sel8:5; //0x0
		unsigned int mbz_3:3; //0x0
		unsigned int dpi_post_pi_sel7:5; //0x0
	} f;
	unsigned int v;
} PLL_PI2_T;
#define PLL_PI2ar (0xB8142098)
#define PLL_PI2dv (0x00000000)
#define PLL_PI2rv RVAL(PLL_PI2)
#define RMOD_PLL_PI2(...) RMOD(PLL_PI2, __VA_ARGS__)
#define RIZS_PLL_PI2(...) RIZS(PLL_PI2, __VA_ARGS__)
#define RFLD_PLL_PI2(fld) RFLD(PLL_PI2, fld)

typedef union {
	struct {
		unsigned int mbz_0:3; //0x0
		unsigned int dpi_dly_sel:5; //0x10
		unsigned int mbz_1:3; //0x0
		unsigned int dpi_dly_sel_ck1:5; //0x10
		unsigned int mbz_2:2; //0x0
		unsigned int dpi_en_post_pi:14; //0x3FFF
	} f;
	unsigned int v;
} PLL_CTL4_T;
#define PLL_CTL4ar (0xB814209C)
#define PLL_CTL4dv (0x10103FFF)
#define PLL_CTL4rv RVAL(PLL_CTL4)
#define RMOD_PLL_CTL4(...) RMOD(PLL_CTL4, __VA_ARGS__)
#define RIZS_PLL_CTL4(...) RIZS(PLL_CTL4, __VA_ARGS__)
#define RFLD_PLL_CTL4(fld) RFLD(PLL_CTL4, fld)

typedef union {
	struct {
		unsigned int mbz_0:19; //0x0
		unsigned int dpi_post_pi_sel12:5; //0x0
		unsigned int mbz_1:3; //0x0
		unsigned int dpi_post_pi_sel11:5; //0x0
	} f;
	unsigned int v;
} PLL_PI3_T;
#define PLL_PI3ar (0xB81420A0)
#define PLL_PI3dv (0x00000000)
#define PLL_PI3rv RVAL(PLL_PI3)
#define RMOD_PLL_PI3(...) RMOD(PLL_PI3, __VA_ARGS__)
#define RIZS_PLL_PI3(...) RIZS(PLL_PI3, __VA_ARGS__)
#define RFLD_PLL_PI3(fld) RFLD(PLL_PI3, fld)

typedef union {
	struct {
		unsigned int dpi_dcc_cs_dsp_dbg_sel:1; //0x0
		unsigned int mbz_0:1; //0x0
		unsigned int dpi_dcc_cs_duty_sel:1; //0x1
		unsigned int dpi_dcc_cs_dsp_stop:1; //0x1
		unsigned int mbz_1:1; //0x0
		unsigned int dpi_dcc_cs_div:3; //0x3
		unsigned int mbz_2:3; //0x0
		unsigned int dpi_dcc_cs_int_gain:5; //0x5
		unsigned int mbz_3:1; //0x0
		unsigned int dpi_dcc_cs_duty:7; //0x28
		unsigned int mbz_4:1; //0x0
		unsigned int dpi_dcc_cs_duty_presetb:3; //0x0
		unsigned int mbz_5:1; //0x0
		unsigned int dpi_dcc_cs_duty_preseta:3; //0x0
	} f;
	unsigned int v;
} DCC_CS_T;
#define DCC_CSar (0xB81420A4)
#define DCC_CSdv (0x33052800)
#define DCC_CSrv RVAL(DCC_CS)
#define RMOD_DCC_CS(...) RMOD(DCC_CS, __VA_ARGS__)
#define RIZS_DCC_CS(...) RIZS(DCC_CS, __VA_ARGS__)
#define RFLD_DCC_CS(fld) RFLD(DCC_CS, fld)

typedef union {
	struct {
		unsigned int dpi_dcc_cs_1_dsp_dbg_sel:1; //0x0
		unsigned int mbz_0:1; //0x0
		unsigned int dpi_dcc_cs_1_duty_sel:1; //0x1
		unsigned int dpi_dcc_cs_1_dsp_stop:1; //0x1
		unsigned int mbz_1:1; //0x0
		unsigned int dpi_dcc_cs_1_div:3; //0x3
		unsigned int mbz_2:3; //0x0
		unsigned int dpi_dcc_cs_1_int_gain:5; //0x5
		unsigned int mbz_3:1; //0x0
		unsigned int dpi_dcc_cs_1_duty:7; //0x28
		unsigned int mbz_4:1; //0x0
		unsigned int dpi_dcc_cs_1_duty_presetb:3; //0x0
		unsigned int mbz_5:1; //0x0
		unsigned int dpi_dcc_cs_1_duty_preseta:3; //0x0
	} f;
	unsigned int v;
} DCC_CS1_T;
#define DCC_CS1ar (0xB81420A8)
#define DCC_CS1dv (0x33052800)
#define DCC_CS1rv RVAL(DCC_CS1)
#define RMOD_DCC_CS1(...) RMOD(DCC_CS1, __VA_ARGS__)
#define RIZS_DCC_CS1(...) RIZS(DCC_CS1, __VA_ARGS__)
#define RFLD_DCC_CS1(fld) RFLD(DCC_CS1, fld)

typedef union {
	struct {
		unsigned int mbz_0:6; //0x0
		unsigned int dpi_dss_speed_en:1; //0x0
		unsigned int dpi_dss_wire_sel:1; //0x0
		unsigned int dpi_dss_data_in:20; //0x0
		unsigned int dpi_dss_ro_sel:3; //0x0
		unsigned int dpi_dss_rst_n:1; //0x0
	} f;
	unsigned int v;
} DSS_VPRB_T;
#define DSS_VPRBar (0xB81420B0)
#define DSS_VPRBdv (0x00000000)
#define DSS_VPRBrv RVAL(DSS_VPRB)
#define RMOD_DSS_VPRB(...) RMOD(DSS_VPRB, __VA_ARGS__)
#define RIZS_DSS_VPRB(...) RIZS(DSS_VPRB, __VA_ARGS__)
#define RFLD_DSS_VPRB(fld) RFLD(DSS_VPRB, fld)

typedef union {
	struct {
		unsigned int mbz_0:8; //0x0
		unsigned int dpi_dss_count_out:20; //0x0
		unsigned int mbz_1:2; //0x0
		unsigned int dpi_dss_wsort_go:1; //0x0
		unsigned int dpi_dss_ready:1; //0x0
	} f;
	unsigned int v;
} DSS_MON_T;
#define DSS_MONar (0xB81420B4)
#define DSS_MONdv (0x00000000)
#define DSS_MONrv RVAL(DSS_MON)
#define RMOD_DSS_MON(...) RMOD(DSS_MON, __VA_ARGS__)
#define RIZS_DSS_MON(...) RIZS(DSS_MON, __VA_ARGS__)
#define RFLD_DSS_MON(fld) RFLD(DSS_MON, fld)

typedef union {
	struct {
		unsigned int adr07_dly_sel:4; //0x0
		unsigned int adr06_dly_sel:4; //0x0
		unsigned int adr05_dly_sel:4; //0x0
		unsigned int adr04_dly_sel:4; //0x0
		unsigned int adr03_dly_sel:4; //0x0
		unsigned int adr02_dly_sel:4; //0x0
		unsigned int adr01_dly_sel:4; //0x0
		unsigned int adr00_dly_sel:4; //0x0
	} f;
	unsigned int v;
} ADR_DLY_0_T;
#define ADR_DLY_0ar (0xB8142200)
#define ADR_DLY_0dv (0x00000000)
#define ADR_DLY_0rv RVAL(ADR_DLY_0)
#define RMOD_ADR_DLY_0(...) RMOD(ADR_DLY_0, __VA_ARGS__)
#define RIZS_ADR_DLY_0(...) RIZS(ADR_DLY_0, __VA_ARGS__)
#define RFLD_ADR_DLY_0(fld) RFLD(ADR_DLY_0, fld)

typedef union {
	struct {
		unsigned int adr15_dly_sel:4; //0x0
		unsigned int adr14_dly_sel:4; //0x0
		unsigned int adr13_dly_sel:4; //0x0
		unsigned int adr12_dly_sel:4; //0x0
		unsigned int adr11_dly_sel:4; //0x0
		unsigned int adr10_dly_sel:4; //0x0
		unsigned int adr09_dly_sel:4; //0x0
		unsigned int adr08_dly_sel:4; //0x0
	} f;
	unsigned int v;
} ADR_DLY_1_T;
#define ADR_DLY_1ar (0xB8142204)
#define ADR_DLY_1dv (0x00000000)
#define ADR_DLY_1rv RVAL(ADR_DLY_1)
#define RMOD_ADR_DLY_1(...) RMOD(ADR_DLY_1, __VA_ARGS__)
#define RIZS_ADR_DLY_1(...) RIZS(ADR_DLY_1, __VA_ARGS__)
#define RFLD_ADR_DLY_1(fld) RFLD(ADR_DLY_1, fld)

typedef union {
	struct {
		unsigned int alert_dly_sel:4; //0x0
		unsigned int ten_dly_sel:4; //0x0
		unsigned int adr16_dly_sel:4; //0x0
		unsigned int ba2_dly_sel:4; //0x0
		unsigned int ba1_dly_sel:4; //0x0
		unsigned int ba0_dly_sel:4; //0x0
		unsigned int adr5_ext_dly_sel:4; //0x0
		unsigned int adr4_ext_dly_sel:4; //0x0
	} f;
	unsigned int v;
} ADR_DLY_2_T;
#define ADR_DLY_2ar (0xB8142208)
#define ADR_DLY_2dv (0x00000000)
#define ADR_DLY_2rv RVAL(ADR_DLY_2)
#define RMOD_ADR_DLY_2(...) RMOD(ADR_DLY_2, __VA_ARGS__)
#define RIZS_ADR_DLY_2(...) RIZS(ADR_DLY_2, __VA_ARGS__)
#define RFLD_ADR_DLY_2(fld) RFLD(ADR_DLY_2, fld)

typedef union {
	struct {
		unsigned int mbz_0:2; //0x0
		unsigned int cs_n_1_dly_sel:4; //0x0
		unsigned int mbz_1:2; //0x0
		unsigned int cs_n_dly_sel:4; //0x0
		unsigned int odt_dly_sel:4; //0x0
		unsigned int we_n_dly_sel:4; //0x0
		unsigned int cas_n_dly_sel:4; //0x0
		unsigned int ras_n_dly_sel:4; //0x0
		unsigned int cke_dly_sel:4; //0x0
	} f;
	unsigned int v;
} CMD_DLY_0_T;
#define CMD_DLY_0ar (0xB814220C)
#define CMD_DLY_0dv (0x00000000)
#define CMD_DLY_0rv RVAL(CMD_DLY_0)
#define RMOD_CMD_DLY_0(...) RMOD(CMD_DLY_0, __VA_ARGS__)
#define RIZS_CMD_DLY_0(...) RIZS(CMD_DLY_0, __VA_ARGS__)
#define RFLD_CMD_DLY_0(fld) RFLD(CMD_DLY_0, fld)

typedef union {
	struct {
		unsigned int adr54_1to1:1; //0x1
		unsigned int mbz_0:2; //0x0
		unsigned int dck_1_oe:1; //0x1
		unsigned int dck_oe:1; //0x1
		unsigned int cke_oe:1; //0x1
		unsigned int ras_n_oe:1; //0x1
		unsigned int cas_n_oe:1; //0x1
		unsigned int we_n_oe:1; //0x1
		unsigned int odt_oe:1; //0x1
		unsigned int reset_n_oe:1; //0x1
		unsigned int bk2_oe:1; //0x1
		unsigned int bk1_oe:1; //0x1
		unsigned int bk0_oe:1; //0x1
		unsigned int ext_adr5_oe:1; //0x1
		unsigned int ext_adr4_oe:1; //0x1
		unsigned int adr15_oe:1; //0x1
		unsigned int adr14_oe:1; //0x1
		unsigned int adr13_oe:1; //0x1
		unsigned int adr12_oe:1; //0x1
		unsigned int adr11_oe:1; //0x1
		unsigned int adr10_oe:1; //0x1
		unsigned int adr09_oe:1; //0x1
		unsigned int adr08_oe:1; //0x1
		unsigned int adr07_oe:1; //0x1
		unsigned int adr06_oe:1; //0x1
		unsigned int adr05_oe:1; //0x1
		unsigned int adr04_oe:1; //0x1
		unsigned int adr03_oe:1; //0x1
		unsigned int adr02_oe:1; //0x1
		unsigned int adr01_oe:1; //0x1
		unsigned int adr00_oe:1; //0x1
	} f;
	unsigned int v;
} PAD_OE_CTL_T;
#define PAD_OE_CTLar (0xB8142210)
#define PAD_OE_CTLdv (0x9FFFFFFF)
#define PAD_OE_CTLrv RVAL(PAD_OE_CTL)
#define RMOD_PAD_OE_CTL(...) RMOD(PAD_OE_CTL, __VA_ARGS__)
#define RIZS_PAD_OE_CTL(...) RIZS(PAD_OE_CTL, __VA_ARGS__)
#define RFLD_PAD_OE_CTL(fld) RFLD(PAD_OE_CTL, fld)

typedef union {
	struct {
		unsigned int mbz_0:20; //0x0
		unsigned int iddq:12; //0x0
	} f;
	unsigned int v;
} PAD_IDDQ_T;
#define PAD_IDDQar (0xB8142214)
#define PAD_IDDQdv (0x00000000)
#define PAD_IDDQrv RVAL(PAD_IDDQ)
#define RMOD_PAD_IDDQ(...) RMOD(PAD_IDDQ, __VA_ARGS__)
#define RIZS_PAD_IDDQ(...) RIZS(PAD_IDDQ, __VA_ARGS__)
#define RFLD_PAD_IDDQ(fld) RFLD(PAD_IDDQ, fld)

typedef union {
	struct {
		unsigned int mbz_0:9; //0x0
		unsigned int zctrl_ovrd_en:1; //0x0
		unsigned int zctrl_ovrd_data:22; //0x0
	} f;
	unsigned int v;
} PAD_ZCTRL_OVRD_T;
#define PAD_ZCTRL_OVRDar (0xB8142218)
#define PAD_ZCTRL_OVRDdv (0x00000000)
#define PAD_ZCTRL_OVRDrv RVAL(PAD_ZCTRL_OVRD)
#define RMOD_PAD_ZCTRL_OVRD(...) RMOD(PAD_ZCTRL_OVRD, __VA_ARGS__)
#define RIZS_PAD_ZCTRL_OVRD(...) RIZS(PAD_ZCTRL_OVRD, __VA_ARGS__)
#define RFLD_PAD_ZCTRL_OVRD(fld) RFLD(PAD_ZCTRL_OVRD, fld)

typedef union {
	struct {
		unsigned int zctrl_filter_en:1; //0x0
		unsigned int dzq_up_sel:3; //0x0
		unsigned int dzq_auto_up:1; //0x0
		unsigned int zctrl_clk_sel:2; //0x2
		unsigned int zctrl_start:1; //0x0
		unsigned int mbz_0:4; //0x0
		unsigned int rzq_ext_r240:1; //0x0
		unsigned int dzq_ref_up:1; //0x0
		unsigned int rzq_comp_inv:1; //0x0
		unsigned int rzq_cal_en:1; //0x0
		unsigned int mbz_1:11; //0x0
		unsigned int zq_cal_wait:5; //0x3
	} f;
	unsigned int v;
} PAD_CTRL_PROG_T;
#define PAD_CTRL_PROGar (0xB814221C)
#define PAD_CTRL_PROGdv (0x04000003)
#define PAD_CTRL_PROGrv RVAL(PAD_CTRL_PROG)
#define RMOD_PAD_CTRL_PROG(...) RMOD(PAD_CTRL_PROG, __VA_ARGS__)
#define RIZS_PAD_CTRL_PROG(...) RIZS(PAD_CTRL_PROG, __VA_ARGS__)
#define RFLD_PAD_CTRL_PROG(fld) RFLD(PAD_CTRL_PROG, fld)

typedef union {
	struct {
		unsigned int cali_done:1; //0x0
		unsigned int cali_status_1:6; //for debug purpose
        unsigned int cali_status:4;   //for error check
		unsigned int ttfp:2;
		unsigned int ttcp:4;
		unsigned int ttcpmsb:1;
		unsigned int ttfn:2;
		unsigned int ttcn:4;
		unsigned int pocd_pull_up_imp:4;
		unsigned int nocd_pull_down_imp:4;
	} f;
	unsigned int v;
} PAD_ZCTRL_STATUS_T;
#define PAD_ZCTRL_STATUSar (0xB8142220)
#define PAD_ZCTRL_STATUSdv (0x00000000)
#define PAD_ZCTRL_STATUSrv RVAL(PAD_ZCTRL_STATUS)
#define RMOD_PAD_ZCTRL_STATUS(...) RMOD(PAD_ZCTRL_STATUS, __VA_ARGS__)
#define RIZS_PAD_ZCTRL_STATUS(...) RIZS(PAD_ZCTRL_STATUS, __VA_ARGS__)
#define RFLD_PAD_ZCTRL_STATUS(fld) RFLD(PAD_ZCTRL_STATUS, fld)

typedef union {
	struct {
		unsigned int mbz_0:5; //0x0
		unsigned int reg_ten_dm:2; //0x0
		unsigned int ten_en:1; //0x0
		unsigned int wrlvl_rddata:4; //0x0
		unsigned int mbz_1:17; //0x0
		unsigned int calvl_en:1; //0x0
		unsigned int wrlvl_trig:1; //0x0
		unsigned int wrlvl_en:1; //0x0
	} f;
	unsigned int v;
} WRLVL_CTRL_T;
#define WRLVL_CTRLar (0xB8142224)
#define WRLVL_CTRLdv (0x00000000)
#define WRLVL_CTRLrv RVAL(WRLVL_CTRL)
#define RMOD_WRLVL_CTRL(...) RMOD(WRLVL_CTRL, __VA_ARGS__)
#define RIZS_WRLVL_CTRL(...) RIZS(WRLVL_CTRL, __VA_ARGS__)
#define RFLD_WRLVL_CTRL(fld) RFLD(WRLVL_CTRL, fld)

typedef union {
	struct {
		unsigned int wrlvl_rddata:32; //0x0
	} f;
	unsigned int v;
} WRLVL_RD_DATA_T;
#define WRLVL_RD_DATAar (0xB8142228)
#define WRLVL_RD_DATAdv (0x00000000)
#define WRLVL_RD_DATArv RVAL(WRLVL_RD_DATA)
#define RMOD_WRLVL_RD_DATA(...) RMOD(WRLVL_RD_DATA, __VA_ARGS__)
#define RIZS_WRLVL_RD_DATA(...) RIZS(WRLVL_RD_DATA, __VA_ARGS__)
#define RFLD_WRLVL_RD_DATA(fld) RFLD(WRLVL_RD_DATA, fld)

typedef union {
	struct {
		unsigned int lbk_mode:2; //0x0
		unsigned int mbz_0:2; //0x0
		unsigned int top_dbg_sel:3; //0x0
		unsigned int mbz_1:3; //0x0
		unsigned int rw_dbg_sel0:2; //0x0
		unsigned int rd_dbg_sel0_3:5; //0x0
		unsigned int rd_dbg_sel0_2:5; //0x0
		unsigned int rd_dbg_sel0_1:5; //0x0
		unsigned int rd_dbg_sel0_0:5; //0x0
	} f;
	unsigned int v;
} TEST_CTRL0_T;
#define TEST_CTRL0ar (0xB814222C)
#define TEST_CTRL0dv (0x00000000)
#define TEST_CTRL0rv RVAL(TEST_CTRL0)
#define RMOD_TEST_CTRL0(...) RMOD(TEST_CTRL0, __VA_ARGS__)
#define RIZS_TEST_CTRL0(...) RIZS(TEST_CTRL0, __VA_ARGS__)
#define RFLD_TEST_CTRL0(fld) RFLD(TEST_CTRL0, fld)

typedef union {
	struct {
		unsigned int dq7_dly_sel:4; //0x0
		unsigned int dq6_dly_sel:4; //0x0
		unsigned int dq5_dly_sel:4; //0x0
		unsigned int dq4_dly_sel:4; //0x0
		unsigned int dq3_dly_sel:4; //0x0
		unsigned int dq2_dly_sel:4; //0x0
		unsigned int dq1_dly_sel:4; //0x0
		unsigned int dq0_dly_sel:4; //0x0
	} f;
	unsigned int v;
} DQ_DLY_0_0_T;
#define DQ_DLY_0_0ar (0xB8142230)
#define DQ_DLY_0_0dv (0x00000000)
#define DQ_DLY_0_0rv RVAL(DQ_DLY_0_0)
#define RMOD_DQ_DLY_0_0(...) RMOD(DQ_DLY_0_0, __VA_ARGS__)
#define RIZS_DQ_DLY_0_0(...) RIZS(DQ_DLY_0_0, __VA_ARGS__)
#define RFLD_DQ_DLY_0_0(fld) RFLD(DQ_DLY_0_0, fld)

typedef union {
	struct {
		unsigned int dq7_dly_sel:4; //0x0
		unsigned int dq6_dly_sel:4; //0x0
		unsigned int dq5_dly_sel:4; //0x0
		unsigned int dq4_dly_sel:4; //0x0
		unsigned int dq3_dly_sel:4; //0x0
		unsigned int dq2_dly_sel:4; //0x0
		unsigned int dq1_dly_sel:4; //0x0
		unsigned int dq0_dly_sel:4; //0x0
	} f;
	unsigned int v;
} DQ_DLY_0_1_T;
#define DQ_DLY_0_1ar (0xB8142234)
#define DQ_DLY_0_1dv (0x00000000)
#define DQ_DLY_0_1rv RVAL(DQ_DLY_0_1)
#define RMOD_DQ_DLY_0_1(...) RMOD(DQ_DLY_0_1, __VA_ARGS__)
#define RIZS_DQ_DLY_0_1(...) RIZS(DQ_DLY_0_1, __VA_ARGS__)
#define RFLD_DQ_DLY_0_1(fld) RFLD(DQ_DLY_0_1, fld)

typedef union {
	struct {
		unsigned int dq7_dly_sel:4; //0x0
		unsigned int dq6_dly_sel:4; //0x0
		unsigned int dq5_dly_sel:4; //0x0
		unsigned int dq4_dly_sel:4; //0x0
		unsigned int dq3_dly_sel:4; //0x0
		unsigned int dq2_dly_sel:4; //0x0
		unsigned int dq1_dly_sel:4; //0x0
		unsigned int dq0_dly_sel:4; //0x0
	} f;
	unsigned int v;
} DQ_DLY_0_2_T;
#define DQ_DLY_0_2ar (0xB8142238)
#define DQ_DLY_0_2dv (0x00000000)
#define DQ_DLY_0_2rv RVAL(DQ_DLY_0_2)
#define RMOD_DQ_DLY_0_2(...) RMOD(DQ_DLY_0_2, __VA_ARGS__)
#define RIZS_DQ_DLY_0_2(...) RIZS(DQ_DLY_0_2, __VA_ARGS__)
#define RFLD_DQ_DLY_0_2(fld) RFLD(DQ_DLY_0_2, fld)

typedef union {
	struct {
		unsigned int dq7_dly_sel:4; //0x0
		unsigned int dq6_dly_sel:4; //0x0
		unsigned int dq5_dly_sel:4; //0x0
		unsigned int dq4_dly_sel:4; //0x0
		unsigned int dq3_dly_sel:4; //0x0
		unsigned int dq2_dly_sel:4; //0x0
		unsigned int dq1_dly_sel:4; //0x0
		unsigned int dq0_dly_sel:4; //0x0
	} f;
	unsigned int v;
} DQ_DLY_0_3_T;
#define DQ_DLY_0_3ar (0xB814223C)
#define DQ_DLY_0_3dv (0x00000000)
#define DQ_DLY_0_3rv RVAL(DQ_DLY_0_3)
#define RMOD_DQ_DLY_0_3(...) RMOD(DQ_DLY_0_3, __VA_ARGS__)
#define RIZS_DQ_DLY_0_3(...) RIZS(DQ_DLY_0_3, __VA_ARGS__)
#define RFLD_DQ_DLY_0_3(fld) RFLD(DQ_DLY_0_3, fld)

typedef union {
	struct {
		unsigned int dm_dly_sel_3:4; //0x0
		unsigned int dqs_dly_sel_3:4; //0x0
		unsigned int dm_dly_sel_2:4; //0x0
		unsigned int dqs_dly_sel_2:4; //0x0
		unsigned int dm_dly_sel_1:4; //0x0
		unsigned int dqs_dly_sel_1:4; //0x0
		unsigned int dm_dly_sel_0:4; //0x0
		unsigned int dqs_dly_sel_0:4; //0x0
	} f;
	unsigned int v;
} DQ_DLY_1_T;
#define DQ_DLY_1ar (0xB8142240)
#define DQ_DLY_1dv (0x00000000)
#define DQ_DLY_1rv RVAL(DQ_DLY_1)
#define RMOD_DQ_DLY_1(...) RMOD(DQ_DLY_1, __VA_ARGS__)
#define RIZS_DQ_DLY_1(...) RIZS(DQ_DLY_1, __VA_ARGS__)
#define RFLD_DQ_DLY_1(fld) RFLD(DQ_DLY_1, fld)

typedef union {
	struct {
		unsigned int mbz_0:1; //0x0
		unsigned int dqs_rc1in_dly_sel:5; //0x0
		unsigned int mbz_1:1; //0x0
		unsigned int dqs_det_dly_sel:5; //0x0
		unsigned int tm_dq_lat_dly:4; //0x0
		unsigned int dqs_fifo_dly_sel:2; //0x0
		unsigned int dqs_en_dly_sel:6; //0x0
		unsigned int mbz_2:1; //0x0
		unsigned int tm_dqs_en:5; //0x0
		unsigned int tm_dqs_en_ftun:2; //0x3
	} f;
	unsigned int v;
} READ_CTRL_0_0_T;
#define READ_CTRL_0_0ar (0xB8142244)
#define READ_CTRL_0_0dv (0x00000003)
#define READ_CTRL_0_0rv RVAL(READ_CTRL_0_0)
#define RMOD_READ_CTRL_0_0(...) RMOD(READ_CTRL_0_0, __VA_ARGS__)
#define RIZS_READ_CTRL_0_0(...) RIZS(READ_CTRL_0_0, __VA_ARGS__)
#define RFLD_READ_CTRL_0_0(fld) RFLD(READ_CTRL_0_0, fld)

typedef union {
	struct {
		unsigned int mbz_0:1; //0x0
		unsigned int dqs_rc1in_dly_sel:5; //0x0
		unsigned int mbz_1:1; //0x0
		unsigned int dqs_det_dly_sel:5; //0x0
		unsigned int tm_dq_lat_dly:4; //0x0
		unsigned int dqs_fifo_dly_sel:2; //0x0
		unsigned int dqs_en_dly_sel:6; //0x0
		unsigned int mbz_2:1; //0x0
		unsigned int tm_dqs_en:5; //0x0
		unsigned int tm_dqs_en_ftun:2; //0x3
	} f;
	unsigned int v;
} READ_CTRL_0_1_T;
#define READ_CTRL_0_1ar (0xB8142248)
#define READ_CTRL_0_1dv (0x00000003)
#define READ_CTRL_0_1rv RVAL(READ_CTRL_0_1)
#define RMOD_READ_CTRL_0_1(...) RMOD(READ_CTRL_0_1, __VA_ARGS__)
#define RIZS_READ_CTRL_0_1(...) RIZS(READ_CTRL_0_1, __VA_ARGS__)
#define RFLD_READ_CTRL_0_1(fld) RFLD(READ_CTRL_0_1, fld)

typedef union {
	struct {
		unsigned int mbz_0:1; //0x0
		unsigned int dqs_rc1in_dly_sel:5; //0x0
		unsigned int mbz_1:1; //0x0
		unsigned int dqs_det_dly_sel:5; //0x0
		unsigned int tm_dq_lat_dly:4; //0x0
		unsigned int dqs_fifo_dly_sel:2; //0x0
		unsigned int dqs_en_dly_sel:6; //0x0
		unsigned int mbz_2:1; //0x0
		unsigned int tm_dqs_en:5; //0x0
		unsigned int tm_dqs_en_ftun:2; //0x3
	} f;
	unsigned int v;
} READ_CTRL_0_2_T;
#define READ_CTRL_0_2ar (0xB814224C)
#define READ_CTRL_0_2dv (0x00000003)
#define READ_CTRL_0_2rv RVAL(READ_CTRL_0_2)
#define RMOD_READ_CTRL_0_2(...) RMOD(READ_CTRL_0_2, __VA_ARGS__)
#define RIZS_READ_CTRL_0_2(...) RIZS(READ_CTRL_0_2, __VA_ARGS__)
#define RFLD_READ_CTRL_0_2(fld) RFLD(READ_CTRL_0_2, fld)

typedef union {
	struct {
		unsigned int mbz_0:1; //0x0
		unsigned int dqs_rc1in_dly_sel:5; //0x0
		unsigned int mbz_1:1; //0x0
		unsigned int dqs_det_dly_sel:5; //0x0
		unsigned int tm_dq_lat_dly:4; //0x0
		unsigned int dqs_fifo_dly_sel:2; //0x0
		unsigned int dqs_en_dly_sel:6; //0x0
		unsigned int mbz_2:1; //0x0
		unsigned int tm_dqs_en:5; //0x0
		unsigned int tm_dqs_en_ftun:2; //0x3
	} f;
	unsigned int v;
} READ_CTRL_0_3_T;
#define READ_CTRL_0_3ar (0xB8142250)
#define READ_CTRL_0_3dv (0x00000003)
#define READ_CTRL_0_3rv RVAL(READ_CTRL_0_3)
#define RMOD_READ_CTRL_0_3(...) RMOD(READ_CTRL_0_3, __VA_ARGS__)
#define RIZS_READ_CTRL_0_3(...) RIZS(READ_CTRL_0_3, __VA_ARGS__)
#define RFLD_READ_CTRL_0_3(fld) RFLD(READ_CTRL_0_3, fld)

typedef union {
	struct {
		unsigned int mbz_0:4; //0x0
		unsigned int ncon_3t_mode:1; //0x0
		unsigned int ncon_2t_mode:1; //0x0
		unsigned int mbz_1:14; //0x0
		unsigned int vref_scnt_ena:1; //0x0
		unsigned int rd_dly_follow_dq0:1; //0x0
		unsigned int dq_lat_en_force_sel:1; //0x0
		unsigned int dq_lat_en_force_sig:1; //0x0
		unsigned int mbz_2:2; //0x0
		unsigned int tm_rd_fifo:6; //0x2
	} f;
	unsigned int v;
} READ_CTRL_1_T;
#define READ_CTRL_1ar (0xB8142254)
#define READ_CTRL_1dv (0x00000002)
#define READ_CTRL_1rv RVAL(READ_CTRL_1)
#define RMOD_READ_CTRL_1(...) RMOD(READ_CTRL_1, __VA_ARGS__)
#define RIZS_READ_CTRL_1(...) RIZS(READ_CTRL_1, __VA_ARGS__)
#define RFLD_READ_CTRL_1(fld) RFLD(READ_CTRL_1, fld)

typedef union {
	struct {
		unsigned int cke_1_dly_sel:4; //0x0
		unsigned int ddr4_cs_n_dly_sel:4; //0x0
		unsigned int alert_1_dly_sel:4; //0x0
		unsigned int parity_1_dly_sel:4; //0x0
		unsigned int parity_dly_sel:4; //0x0
		unsigned int ddr4_bg0_dly_sel:4; //0x0
		unsigned int ddr4_cke_dly_sel:4; //0x0
		unsigned int act_n_dly_sel:4; //0x0
	} f;
	unsigned int v;
} CMD_DLY_1_T;
#define CMD_DLY_1ar (0xB8142258)
#define CMD_DLY_1dv (0x00000000)
#define CMD_DLY_1rv RVAL(CMD_DLY_1)
#define RMOD_CMD_DLY_1(...) RMOD(CMD_DLY_1, __VA_ARGS__)
#define RIZS_CMD_DLY_1(...) RIZS(CMD_DLY_1, __VA_ARGS__)
#define RFLD_CMD_DLY_1(fld) RFLD(CMD_DLY_1, fld)

typedef union {
	struct {
		unsigned int ocdp_set_7:4; //0x7
		unsigned int ocdp_set_6:4; //0x7
		unsigned int ocdp_set_5:4; //0x7
		unsigned int ocdp_set_4:4; //0x7
		unsigned int ocdp_set_3:4; //0x7
		unsigned int ocdp_set_2:4; //0x7
		unsigned int ocdp_set_1:4; //0x7
		unsigned int ocdp_set_0:4; //0x7
	} f;
	unsigned int v;
} OCDP_SET1_T;
#define OCDP_SET1ar (0xB814225C)
#define OCDP_SET1dv (0x77777777)
#define OCDP_SET1rv RVAL(OCDP_SET1)
#define RMOD_OCDP_SET1(...) RMOD(OCDP_SET1, __VA_ARGS__)
#define RIZS_OCDP_SET1(...) RIZS(OCDP_SET1, __VA_ARGS__)
#define RFLD_OCDP_SET1(fld) RFLD(OCDP_SET1, fld)

typedef union {
	struct {
		unsigned int ocdn_set_7:4; //0x7
		unsigned int ocdn_set_6:4; //0x7
		unsigned int ocdn_set_5:4; //0x7
		unsigned int ocdn_set_4:4; //0x7
		unsigned int ocdn_set_3:4; //0x7
		unsigned int ocdn_set_2:4; //0x7
		unsigned int ocdn_set_1:4; //0x7
		unsigned int ocdn_set_0:4; //0x7
	} f;
	unsigned int v;
} OCDN_SET1_T;
#define OCDN_SET1ar (0xB8142260)
#define OCDN_SET1dv (0x77777777)
#define OCDN_SET1rv RVAL(OCDN_SET1)
#define RMOD_OCDN_SET1(...) RMOD(OCDN_SET1, __VA_ARGS__)
#define RIZS_OCDN_SET1(...) RIZS(OCDN_SET1, __VA_ARGS__)
#define RFLD_OCDN_SET1(fld) RFLD(OCDN_SET1, fld)

typedef union {
	struct {
		unsigned int mbz_0:28; //0x0
		unsigned int odt_1_dly_sel:4; //0x0
	} f;
	unsigned int v;
} CMD_DLY_2_T;
#define CMD_DLY_2ar (0xB8142264)
#define CMD_DLY_2dv (0x00000000)
#define CMD_DLY_2rv RVAL(CMD_DLY_2)
#define RMOD_CMD_DLY_2(...) RMOD(CMD_DLY_2, __VA_ARGS__)
#define RIZS_CMD_DLY_2(...) RIZS(CMD_DLY_2, __VA_ARGS__)
#define RFLD_CMD_DLY_2(fld) RFLD(CMD_DLY_2, fld)

typedef union {
	struct {
		unsigned int mbz_0:1; //0x0
		unsigned int cal_lead3:3; //0x2
		unsigned int mbz_1:1; //0x0
		unsigned int cal_lead2:3; //0x2
		unsigned int mbz_2:1; //0x0
		unsigned int cal_lead1:3; //0x1
		unsigned int mbz_3:1; //0x0
		unsigned int cal_lead0:3; //0x1
		unsigned int mbz_4:1; //0x0
		unsigned int cal_lag3:3; //0x6
		unsigned int mbz_5:1; //0x0
		unsigned int cal_lag2:3; //0x6
		unsigned int mbz_6:1; //0x0
		unsigned int cal_lag1:3; //0x7
		unsigned int mbz_7:1; //0x0
		unsigned int cal_lag0:3; //0x7
	} f;
	unsigned int v;
} CAL_SHIFT_CTRL_T;
#define CAL_SHIFT_CTRLar (0xB8142268)
#define CAL_SHIFT_CTRLdv (0x22116677)
#define CAL_SHIFT_CTRLrv RVAL(CAL_SHIFT_CTRL)
#define RMOD_CAL_SHIFT_CTRL(...) RMOD(CAL_SHIFT_CTRL, __VA_ARGS__)
#define RIZS_CAL_SHIFT_CTRL(...) RIZS(CAL_SHIFT_CTRL, __VA_ARGS__)
#define RFLD_CAL_SHIFT_CTRL(fld) RFLD(CAL_SHIFT_CTRL, fld)

typedef union {
	struct {
		unsigned int mbz_0:1; //0x0
		unsigned int pre_shift_sel_7:3; //0x2
		unsigned int mbz_1:1; //0x0
		unsigned int pre_shift_sel_6:3; //0x2
		unsigned int mbz_2:1; //0x0
		unsigned int pre_shift_sel_5:3; //0x2
		unsigned int mbz_3:1; //0x0
		unsigned int pre_shift_sel_4:3; //0x2
		unsigned int mbz_4:1; //0x0
		unsigned int pre_shift_sel_3:3; //0x2
		unsigned int mbz_5:1; //0x0
		unsigned int pre_shift_sel_2:3; //0x2
		unsigned int mbz_6:1; //0x0
		unsigned int pre_shift_sel_1:3; //0x2
		unsigned int mbz_7:1; //0x0
		unsigned int pre_shift_sel_0:3; //0x2
	} f;
	unsigned int v;
} CAL_LS_SEL_0_T;
#define CAL_LS_SEL_0ar (0xB814226C)
#define CAL_LS_SEL_0dv (0x22222222)
#define CAL_LS_SEL_0rv RVAL(CAL_LS_SEL_0)
#define RMOD_CAL_LS_SEL_0(...) RMOD(CAL_LS_SEL_0, __VA_ARGS__)
#define RIZS_CAL_LS_SEL_0(...) RIZS(CAL_LS_SEL_0, __VA_ARGS__)
#define RFLD_CAL_LS_SEL_0(fld) RFLD(CAL_LS_SEL_0, fld)

typedef union {
	struct {
		unsigned int mbz_0:1; //0x0
		unsigned int pre_shift_sel_7:3; //0x2
		unsigned int mbz_1:1; //0x0
		unsigned int pre_shift_sel_6:3; //0x2
		unsigned int mbz_2:1; //0x0
		unsigned int pre_shift_sel_5:3; //0x2
		unsigned int mbz_3:1; //0x0
		unsigned int pre_shift_sel_4:3; //0x2
		unsigned int mbz_4:1; //0x0
		unsigned int pre_shift_sel_3:3; //0x2
		unsigned int mbz_5:1; //0x0
		unsigned int pre_shift_sel_2:3; //0x2
		unsigned int mbz_6:1; //0x0
		unsigned int pre_shift_sel_1:3; //0x2
		unsigned int mbz_7:1; //0x0
		unsigned int pre_shift_sel_0:3; //0x2
	} f;
	unsigned int v;
} CAL_LS_SEL_1_T;
#define CAL_LS_SEL_1ar (0xB8142270)
#define CAL_LS_SEL_1dv (0x22222222)
#define CAL_LS_SEL_1rv RVAL(CAL_LS_SEL_1)
#define RMOD_CAL_LS_SEL_1(...) RMOD(CAL_LS_SEL_1, __VA_ARGS__)
#define RIZS_CAL_LS_SEL_1(...) RIZS(CAL_LS_SEL_1, __VA_ARGS__)
#define RFLD_CAL_LS_SEL_1(fld) RFLD(CAL_LS_SEL_1, fld)

typedef union {
	struct {
		unsigned int mbz_0:1; //0x0
		unsigned int pre_shift_sel_7:3; //0x2
		unsigned int mbz_1:1; //0x0
		unsigned int pre_shift_sel_6:3; //0x2
		unsigned int mbz_2:1; //0x0
		unsigned int pre_shift_sel_5:3; //0x2
		unsigned int mbz_3:1; //0x0
		unsigned int pre_shift_sel_4:3; //0x2
		unsigned int mbz_4:1; //0x0
		unsigned int pre_shift_sel_3:3; //0x2
		unsigned int mbz_5:1; //0x0
		unsigned int pre_shift_sel_2:3; //0x2
		unsigned int mbz_6:1; //0x0
		unsigned int pre_shift_sel_1:3; //0x2
		unsigned int mbz_7:1; //0x0
		unsigned int pre_shift_sel_0:3; //0x2
	} f;
	unsigned int v;
} CAL_LS_SEL_2_T;
#define CAL_LS_SEL_2ar (0xB8142274)
#define CAL_LS_SEL_2dv (0x22222222)
#define CAL_LS_SEL_2rv RVAL(CAL_LS_SEL_2)
#define RMOD_CAL_LS_SEL_2(...) RMOD(CAL_LS_SEL_2, __VA_ARGS__)
#define RIZS_CAL_LS_SEL_2(...) RIZS(CAL_LS_SEL_2, __VA_ARGS__)
#define RFLD_CAL_LS_SEL_2(fld) RFLD(CAL_LS_SEL_2, fld)

typedef union {
	struct {
		unsigned int mbz_0:1; //0x0
		unsigned int pre_shift_sel_7:3; //0x2
		unsigned int mbz_1:1; //0x0
		unsigned int pre_shift_sel_6:3; //0x2
		unsigned int mbz_2:1; //0x0
		unsigned int pre_shift_sel_5:3; //0x2
		unsigned int mbz_3:1; //0x0
		unsigned int pre_shift_sel_4:3; //0x2
		unsigned int mbz_4:1; //0x0
		unsigned int pre_shift_sel_3:3; //0x2
		unsigned int mbz_5:1; //0x0
		unsigned int pre_shift_sel_2:3; //0x2
		unsigned int mbz_6:1; //0x0
		unsigned int pre_shift_sel_1:3; //0x2
		unsigned int mbz_7:1; //0x0
		unsigned int pre_shift_sel_0:3; //0x2
	} f;
	unsigned int v;
} CAL_LS_SEL_3_T;
#define CAL_LS_SEL_3ar (0xB8142278)
#define CAL_LS_SEL_3dv (0x22222222)
#define CAL_LS_SEL_3rv RVAL(CAL_LS_SEL_3)
#define RMOD_CAL_LS_SEL_3(...) RMOD(CAL_LS_SEL_3, __VA_ARGS__)
#define RIZS_CAL_LS_SEL_3(...) RIZS(CAL_LS_SEL_3, __VA_ARGS__)
#define RFLD_CAL_LS_SEL_3(fld) RFLD(CAL_LS_SEL_3, fld)

typedef union {
	struct {
		unsigned int mbz_0:1; //0x0
		unsigned int post_shift_sel_7:3; //0x2
		unsigned int mbz_1:1; //0x0
		unsigned int post_shift_sel_6:3; //0x2
		unsigned int mbz_2:1; //0x0
		unsigned int post_shift_sel_5:3; //0x2
		unsigned int mbz_3:1; //0x0
		unsigned int post_shift_sel_4:3; //0x2
		unsigned int mbz_4:1; //0x0
		unsigned int post_shift_sel_3:3; //0x2
		unsigned int mbz_5:1; //0x0
		unsigned int post_shift_sel_2:3; //0x2
		unsigned int mbz_6:1; //0x0
		unsigned int post_shift_sel_1:3; //0x2
		unsigned int mbz_7:1; //0x0
		unsigned int post_shift_sel_0:3; //0x2
	} f;
	unsigned int v;
} CAL_RS_SEL_0_T;
#define CAL_RS_SEL_0ar (0xB814227C)
#define CAL_RS_SEL_0dv (0x22222222)
#define CAL_RS_SEL_0rv RVAL(CAL_RS_SEL_0)
#define RMOD_CAL_RS_SEL_0(...) RMOD(CAL_RS_SEL_0, __VA_ARGS__)
#define RIZS_CAL_RS_SEL_0(...) RIZS(CAL_RS_SEL_0, __VA_ARGS__)
#define RFLD_CAL_RS_SEL_0(fld) RFLD(CAL_RS_SEL_0, fld)

typedef union {
	struct {
		unsigned int mbz_0:1; //0x0
		unsigned int post_shift_sel_7:3; //0x2
		unsigned int mbz_1:1; //0x0
		unsigned int post_shift_sel_6:3; //0x2
		unsigned int mbz_2:1; //0x0
		unsigned int post_shift_sel_5:3; //0x2
		unsigned int mbz_3:1; //0x0
		unsigned int post_shift_sel_4:3; //0x2
		unsigned int mbz_4:1; //0x0
		unsigned int post_shift_sel_3:3; //0x2
		unsigned int mbz_5:1; //0x0
		unsigned int post_shift_sel_2:3; //0x2
		unsigned int mbz_6:1; //0x0
		unsigned int post_shift_sel_1:3; //0x2
		unsigned int mbz_7:1; //0x0
		unsigned int post_shift_sel_0:3; //0x2
	} f;
	unsigned int v;
} CAL_RS_SEL_1_T;
#define CAL_RS_SEL_1ar (0xB8142280)
#define CAL_RS_SEL_1dv (0x22222222)
#define CAL_RS_SEL_1rv RVAL(CAL_RS_SEL_1)
#define RMOD_CAL_RS_SEL_1(...) RMOD(CAL_RS_SEL_1, __VA_ARGS__)
#define RIZS_CAL_RS_SEL_1(...) RIZS(CAL_RS_SEL_1, __VA_ARGS__)
#define RFLD_CAL_RS_SEL_1(fld) RFLD(CAL_RS_SEL_1, fld)

typedef union {
	struct {
		unsigned int mbz_0:1; //0x0
		unsigned int post_shift_sel_7:3; //0x2
		unsigned int mbz_1:1; //0x0
		unsigned int post_shift_sel_6:3; //0x2
		unsigned int mbz_2:1; //0x0
		unsigned int post_shift_sel_5:3; //0x2
		unsigned int mbz_3:1; //0x0
		unsigned int post_shift_sel_4:3; //0x2
		unsigned int mbz_4:1; //0x0
		unsigned int post_shift_sel_3:3; //0x2
		unsigned int mbz_5:1; //0x0
		unsigned int post_shift_sel_2:3; //0x2
		unsigned int mbz_6:1; //0x0
		unsigned int post_shift_sel_1:3; //0x2
		unsigned int mbz_7:1; //0x0
		unsigned int post_shift_sel_0:3; //0x2
	} f;
	unsigned int v;
} CAL_RS_SEL_2_T;
#define CAL_RS_SEL_2ar (0xB8142284)
#define CAL_RS_SEL_2dv (0x22222222)
#define CAL_RS_SEL_2rv RVAL(CAL_RS_SEL_2)
#define RMOD_CAL_RS_SEL_2(...) RMOD(CAL_RS_SEL_2, __VA_ARGS__)
#define RIZS_CAL_RS_SEL_2(...) RIZS(CAL_RS_SEL_2, __VA_ARGS__)
#define RFLD_CAL_RS_SEL_2(fld) RFLD(CAL_RS_SEL_2, fld)

typedef union {
	struct {
		unsigned int mbz_0:1; //0x0
		unsigned int post_shift_sel_7:3; //0x2
		unsigned int mbz_1:1; //0x0
		unsigned int post_shift_sel_6:3; //0x2
		unsigned int mbz_2:1; //0x0
		unsigned int post_shift_sel_5:3; //0x2
		unsigned int mbz_3:1; //0x0
		unsigned int post_shift_sel_4:3; //0x2
		unsigned int mbz_4:1; //0x0
		unsigned int post_shift_sel_3:3; //0x2
		unsigned int mbz_5:1; //0x0
		unsigned int post_shift_sel_2:3; //0x2
		unsigned int mbz_6:1; //0x0
		unsigned int post_shift_sel_1:3; //0x2
		unsigned int mbz_7:1; //0x0
		unsigned int post_shift_sel_0:3; //0x2
	} f;
	unsigned int v;
} CAL_RS_SEL_3_T;
#define CAL_RS_SEL_3ar (0xB8142288)
#define CAL_RS_SEL_3dv (0x22222222)
#define CAL_RS_SEL_3rv RVAL(CAL_RS_SEL_3)
#define RMOD_CAL_RS_SEL_3(...) RMOD(CAL_RS_SEL_3, __VA_ARGS__)
#define RIZS_CAL_RS_SEL_3(...) RIZS(CAL_RS_SEL_3, __VA_ARGS__)
#define RFLD_CAL_RS_SEL_3(fld) RFLD(CAL_RS_SEL_3, fld)

typedef union {
	struct {
		unsigned int mbz_0:3; //0x0
		unsigned int fw_rd_dly_pos_sel_3:5; //0xA
		unsigned int mbz_1:3; //0x0
		unsigned int fw_rd_dly_pos_sel_2:5; //0xA
		unsigned int mbz_2:3; //0x0
		unsigned int fw_rd_dly_pos_sel_1:5; //0xA
		unsigned int mbz_3:3; //0x0
		unsigned int fw_rd_dly_pos_sel_0:5; //0xA
	} f;
	unsigned int v;
} DQS_IN_DLY_0_0_T;
#define DQS_IN_DLY_0_0ar (0xB814228C)
#define DQS_IN_DLY_0_0dv (0x0A0A0A0A)
#define DQS_IN_DLY_0_0rv RVAL(DQS_IN_DLY_0_0)
#define RMOD_DQS_IN_DLY_0_0(...) RMOD(DQS_IN_DLY_0_0, __VA_ARGS__)
#define RIZS_DQS_IN_DLY_0_0(...) RIZS(DQS_IN_DLY_0_0, __VA_ARGS__)
#define RFLD_DQS_IN_DLY_0_0(fld) RFLD(DQS_IN_DLY_0_0, fld)

typedef union {
	struct {
		unsigned int mbz_0:3; //0x0
		unsigned int fw_rd_dly_pos_sel_3:5; //0xA
		unsigned int mbz_1:3; //0x0
		unsigned int fw_rd_dly_pos_sel_2:5; //0xA
		unsigned int mbz_2:3; //0x0
		unsigned int fw_rd_dly_pos_sel_1:5; //0xA
		unsigned int mbz_3:3; //0x0
		unsigned int fw_rd_dly_pos_sel_0:5; //0xA
	} f;
	unsigned int v;
} DQS_IN_DLY_0_1_T;
#define DQS_IN_DLY_0_1ar (0xB8142290)
#define DQS_IN_DLY_0_1dv (0x0A0A0A0A)
#define DQS_IN_DLY_0_1rv RVAL(DQS_IN_DLY_0_1)
#define RMOD_DQS_IN_DLY_0_1(...) RMOD(DQS_IN_DLY_0_1, __VA_ARGS__)
#define RIZS_DQS_IN_DLY_0_1(...) RIZS(DQS_IN_DLY_0_1, __VA_ARGS__)
#define RFLD_DQS_IN_DLY_0_1(fld) RFLD(DQS_IN_DLY_0_1, fld)

typedef union {
	struct {
		unsigned int mbz_0:3; //0x0
		unsigned int fw_rd_dly_pos_sel_3:5; //0xA
		unsigned int mbz_1:3; //0x0
		unsigned int fw_rd_dly_pos_sel_2:5; //0xA
		unsigned int mbz_2:3; //0x0
		unsigned int fw_rd_dly_pos_sel_1:5; //0xA
		unsigned int mbz_3:3; //0x0
		unsigned int fw_rd_dly_pos_sel_0:5; //0xA
	} f;
	unsigned int v;
} DQS_IN_DLY_0_2_T;
#define DQS_IN_DLY_0_2ar (0xB8142294)
#define DQS_IN_DLY_0_2dv (0x0A0A0A0A)
#define DQS_IN_DLY_0_2rv RVAL(DQS_IN_DLY_0_2)
#define RMOD_DQS_IN_DLY_0_2(...) RMOD(DQS_IN_DLY_0_2, __VA_ARGS__)
#define RIZS_DQS_IN_DLY_0_2(...) RIZS(DQS_IN_DLY_0_2, __VA_ARGS__)
#define RFLD_DQS_IN_DLY_0_2(fld) RFLD(DQS_IN_DLY_0_2, fld)

typedef union {
	struct {
		unsigned int mbz_0:3; //0x0
		unsigned int fw_rd_dly_pos_sel_3:5; //0xA
		unsigned int mbz_1:3; //0x0
		unsigned int fw_rd_dly_pos_sel_2:5; //0xA
		unsigned int mbz_2:3; //0x0
		unsigned int fw_rd_dly_pos_sel_1:5; //0xA
		unsigned int mbz_3:3; //0x0
		unsigned int fw_rd_dly_pos_sel_0:5; //0xA
	} f;
	unsigned int v;
} DQS_IN_DLY_0_3_T;
#define DQS_IN_DLY_0_3ar (0xB8142298)
#define DQS_IN_DLY_0_3dv (0x0A0A0A0A)
#define DQS_IN_DLY_0_3rv RVAL(DQS_IN_DLY_0_3)
#define RMOD_DQS_IN_DLY_0_3(...) RMOD(DQS_IN_DLY_0_3, __VA_ARGS__)
#define RIZS_DQS_IN_DLY_0_3(...) RIZS(DQS_IN_DLY_0_3, __VA_ARGS__)
#define RFLD_DQS_IN_DLY_0_3(fld) RFLD(DQS_IN_DLY_0_3, fld)

typedef union {
	struct {
		unsigned int mbz_0:3; //0x0
		unsigned int fw_rd_dly_pos_sel_7:5; //0xA
		unsigned int mbz_1:3; //0x0
		unsigned int fw_rd_dly_pos_sel_6:5; //0xA
		unsigned int mbz_2:3; //0x0
		unsigned int fw_rd_dly_pos_sel_5:5; //0xA
		unsigned int mbz_3:3; //0x0
		unsigned int fw_rd_dly_pos_sel_4:5; //0xA
	} f;
	unsigned int v;
} DQS_IN_DLY_1_0_T;
#define DQS_IN_DLY_1_0ar (0xB814229C)
#define DQS_IN_DLY_1_0dv (0x0A0A0A0A)
#define DQS_IN_DLY_1_0rv RVAL(DQS_IN_DLY_1_0)
#define RMOD_DQS_IN_DLY_1_0(...) RMOD(DQS_IN_DLY_1_0, __VA_ARGS__)
#define RIZS_DQS_IN_DLY_1_0(...) RIZS(DQS_IN_DLY_1_0, __VA_ARGS__)
#define RFLD_DQS_IN_DLY_1_0(fld) RFLD(DQS_IN_DLY_1_0, fld)

typedef union {
	struct {
		unsigned int mbz_0:3; //0x0
		unsigned int fw_rd_dly_pos_sel_7:5; //0xA
		unsigned int mbz_1:3; //0x0
		unsigned int fw_rd_dly_pos_sel_6:5; //0xA
		unsigned int mbz_2:3; //0x0
		unsigned int fw_rd_dly_pos_sel_5:5; //0xA
		unsigned int mbz_3:3; //0x0
		unsigned int fw_rd_dly_pos_sel_4:5; //0xA
	} f;
	unsigned int v;
} DQS_IN_DLY_1_1_T;
#define DQS_IN_DLY_1_1ar (0xB81422A0)
#define DQS_IN_DLY_1_1dv (0x0A0A0A0A)
#define DQS_IN_DLY_1_1rv RVAL(DQS_IN_DLY_1_1)
#define RMOD_DQS_IN_DLY_1_1(...) RMOD(DQS_IN_DLY_1_1, __VA_ARGS__)
#define RIZS_DQS_IN_DLY_1_1(...) RIZS(DQS_IN_DLY_1_1, __VA_ARGS__)
#define RFLD_DQS_IN_DLY_1_1(fld) RFLD(DQS_IN_DLY_1_1, fld)

typedef union {
	struct {
		unsigned int mbz_0:3; //0x0
		unsigned int fw_rd_dly_pos_sel_7:5; //0xA
		unsigned int mbz_1:3; //0x0
		unsigned int fw_rd_dly_pos_sel_6:5; //0xA
		unsigned int mbz_2:3; //0x0
		unsigned int fw_rd_dly_pos_sel_5:5; //0xA
		unsigned int mbz_3:3; //0x0
		unsigned int fw_rd_dly_pos_sel_4:5; //0xA
	} f;
	unsigned int v;
} DQS_IN_DLY_1_2_T;
#define DQS_IN_DLY_1_2ar (0xB81422A4)
#define DQS_IN_DLY_1_2dv (0x0A0A0A0A)
#define DQS_IN_DLY_1_2rv RVAL(DQS_IN_DLY_1_2)
#define RMOD_DQS_IN_DLY_1_2(...) RMOD(DQS_IN_DLY_1_2, __VA_ARGS__)
#define RIZS_DQS_IN_DLY_1_2(...) RIZS(DQS_IN_DLY_1_2, __VA_ARGS__)
#define RFLD_DQS_IN_DLY_1_2(fld) RFLD(DQS_IN_DLY_1_2, fld)

typedef union {
	struct {
		unsigned int mbz_0:3; //0x0
		unsigned int fw_rd_dly_pos_sel_7:5; //0xA
		unsigned int mbz_1:3; //0x0
		unsigned int fw_rd_dly_pos_sel_6:5; //0xA
		unsigned int mbz_2:3; //0x0
		unsigned int fw_rd_dly_pos_sel_5:5; //0xA
		unsigned int mbz_3:3; //0x0
		unsigned int fw_rd_dly_pos_sel_4:5; //0xA
	} f;
	unsigned int v;
} DQS_IN_DLY_1_3_T;
#define DQS_IN_DLY_1_3ar (0xB81422A8)
#define DQS_IN_DLY_1_3dv (0x0A0A0A0A)
#define DQS_IN_DLY_1_3rv RVAL(DQS_IN_DLY_1_3)
#define RMOD_DQS_IN_DLY_1_3(...) RMOD(DQS_IN_DLY_1_3, __VA_ARGS__)
#define RIZS_DQS_IN_DLY_1_3(...) RIZS(DQS_IN_DLY_1_3, __VA_ARGS__)
#define RFLD_DQS_IN_DLY_1_3(fld) RFLD(DQS_IN_DLY_1_3, fld)

typedef union {
	struct {
		unsigned int mbz_0:3; //0x0
		unsigned int fw_rd_dly_neg_sel_3:5; //0xA
		unsigned int mbz_1:3; //0x0
		unsigned int fw_rd_dly_neg_sel_2:5; //0xA
		unsigned int mbz_2:3; //0x0
		unsigned int fw_rd_dly_neg_sel_1:5; //0xA
		unsigned int mbz_3:3; //0x0
		unsigned int fw_rd_dly_neg_sel_0:5; //0xA
	} f;
	unsigned int v;
} DQS_IN_DLY_2_0_T;
#define DQS_IN_DLY_2_0ar (0xB81422AC)
#define DQS_IN_DLY_2_0dv (0x0A0A0A0A)
#define DQS_IN_DLY_2_0rv RVAL(DQS_IN_DLY_2_0)
#define RMOD_DQS_IN_DLY_2_0(...) RMOD(DQS_IN_DLY_2_0, __VA_ARGS__)
#define RIZS_DQS_IN_DLY_2_0(...) RIZS(DQS_IN_DLY_2_0, __VA_ARGS__)
#define RFLD_DQS_IN_DLY_2_0(fld) RFLD(DQS_IN_DLY_2_0, fld)

typedef union {
	struct {
		unsigned int mbz_0:3; //0x0
		unsigned int fw_rd_dly_neg_sel_3:5; //0xA
		unsigned int mbz_1:3; //0x0
		unsigned int fw_rd_dly_neg_sel_2:5; //0xA
		unsigned int mbz_2:3; //0x0
		unsigned int fw_rd_dly_neg_sel_1:5; //0xA
		unsigned int mbz_3:3; //0x0
		unsigned int fw_rd_dly_neg_sel_0:5; //0xA
	} f;
	unsigned int v;
} DQS_IN_DLY_2_1_T;
#define DQS_IN_DLY_2_1ar (0xB81422B0)
#define DQS_IN_DLY_2_1dv (0x0A0A0A0A)
#define DQS_IN_DLY_2_1rv RVAL(DQS_IN_DLY_2_1)
#define RMOD_DQS_IN_DLY_2_1(...) RMOD(DQS_IN_DLY_2_1, __VA_ARGS__)
#define RIZS_DQS_IN_DLY_2_1(...) RIZS(DQS_IN_DLY_2_1, __VA_ARGS__)
#define RFLD_DQS_IN_DLY_2_1(fld) RFLD(DQS_IN_DLY_2_1, fld)

typedef union {
	struct {
		unsigned int mbz_0:3; //0x0
		unsigned int fw_rd_dly_neg_sel_3:5; //0xA
		unsigned int mbz_1:3; //0x0
		unsigned int fw_rd_dly_neg_sel_2:5; //0xA
		unsigned int mbz_2:3; //0x0
		unsigned int fw_rd_dly_neg_sel_1:5; //0xA
		unsigned int mbz_3:3; //0x0
		unsigned int fw_rd_dly_neg_sel_0:5; //0xA
	} f;
	unsigned int v;
} DQS_IN_DLY_2_2_T;
#define DQS_IN_DLY_2_2ar (0xB81422B4)
#define DQS_IN_DLY_2_2dv (0x0A0A0A0A)
#define DQS_IN_DLY_2_2rv RVAL(DQS_IN_DLY_2_2)
#define RMOD_DQS_IN_DLY_2_2(...) RMOD(DQS_IN_DLY_2_2, __VA_ARGS__)
#define RIZS_DQS_IN_DLY_2_2(...) RIZS(DQS_IN_DLY_2_2, __VA_ARGS__)
#define RFLD_DQS_IN_DLY_2_2(fld) RFLD(DQS_IN_DLY_2_2, fld)

typedef union {
	struct {
		unsigned int mbz_0:3; //0x0
		unsigned int fw_rd_dly_neg_sel_3:5; //0xA
		unsigned int mbz_1:3; //0x0
		unsigned int fw_rd_dly_neg_sel_2:5; //0xA
		unsigned int mbz_2:3; //0x0
		unsigned int fw_rd_dly_neg_sel_1:5; //0xA
		unsigned int mbz_3:3; //0x0
		unsigned int fw_rd_dly_neg_sel_0:5; //0xA
	} f;
	unsigned int v;
} DQS_IN_DLY_2_3_T;
#define DQS_IN_DLY_2_3ar (0xB81422B8)
#define DQS_IN_DLY_2_3dv (0x0A0A0A0A)
#define DQS_IN_DLY_2_3rv RVAL(DQS_IN_DLY_2_3)
#define RMOD_DQS_IN_DLY_2_3(...) RMOD(DQS_IN_DLY_2_3, __VA_ARGS__)
#define RIZS_DQS_IN_DLY_2_3(...) RIZS(DQS_IN_DLY_2_3, __VA_ARGS__)
#define RFLD_DQS_IN_DLY_2_3(fld) RFLD(DQS_IN_DLY_2_3, fld)

typedef union {
	struct {
		unsigned int mbz_0:3; //0x0
		unsigned int fw_rd_dly_neg_sel_7:5; //0xA
		unsigned int mbz_1:3; //0x0
		unsigned int fw_rd_dly_neg_sel_6:5; //0xA
		unsigned int mbz_2:3; //0x0
		unsigned int fw_rd_dly_neg_sel_5:5; //0xA
		unsigned int mbz_3:3; //0x0
		unsigned int fw_rd_dly_neg_sel_4:5; //0xA
	} f;
	unsigned int v;
} DQS_IN_DLY_3_0_T;
#define DQS_IN_DLY_3_0ar (0xB81422BC)
#define DQS_IN_DLY_3_0dv (0x0A0A0A0A)
#define DQS_IN_DLY_3_0rv RVAL(DQS_IN_DLY_3_0)
#define RMOD_DQS_IN_DLY_3_0(...) RMOD(DQS_IN_DLY_3_0, __VA_ARGS__)
#define RIZS_DQS_IN_DLY_3_0(...) RIZS(DQS_IN_DLY_3_0, __VA_ARGS__)
#define RFLD_DQS_IN_DLY_3_0(fld) RFLD(DQS_IN_DLY_3_0, fld)

typedef union {
	struct {
		unsigned int mbz_0:3; //0x0
		unsigned int fw_rd_dly_neg_sel_7:5; //0xA
		unsigned int mbz_1:3; //0x0
		unsigned int fw_rd_dly_neg_sel_6:5; //0xA
		unsigned int mbz_2:3; //0x0
		unsigned int fw_rd_dly_neg_sel_5:5; //0xA
		unsigned int mbz_3:3; //0x0
		unsigned int fw_rd_dly_neg_sel_4:5; //0xA
	} f;
	unsigned int v;
} DQS_IN_DLY_3_1_T;
#define DQS_IN_DLY_3_1ar (0xB81422C0)
#define DQS_IN_DLY_3_1dv (0x0A0A0A0A)
#define DQS_IN_DLY_3_1rv RVAL(DQS_IN_DLY_3_1)
#define RMOD_DQS_IN_DLY_3_1(...) RMOD(DQS_IN_DLY_3_1, __VA_ARGS__)
#define RIZS_DQS_IN_DLY_3_1(...) RIZS(DQS_IN_DLY_3_1, __VA_ARGS__)
#define RFLD_DQS_IN_DLY_3_1(fld) RFLD(DQS_IN_DLY_3_1, fld)

typedef union {
	struct {
		unsigned int mbz_0:3; //0x0
		unsigned int fw_rd_dly_neg_sel_7:5; //0xA
		unsigned int mbz_1:3; //0x0
		unsigned int fw_rd_dly_neg_sel_6:5; //0xA
		unsigned int mbz_2:3; //0x0
		unsigned int fw_rd_dly_neg_sel_5:5; //0xA
		unsigned int mbz_3:3; //0x0
		unsigned int fw_rd_dly_neg_sel_4:5; //0xA
	} f;
	unsigned int v;
} DQS_IN_DLY_3_2_T;
#define DQS_IN_DLY_3_2ar (0xB81422C4)
#define DQS_IN_DLY_3_2dv (0x0A0A0A0A)
#define DQS_IN_DLY_3_2rv RVAL(DQS_IN_DLY_3_2)
#define RMOD_DQS_IN_DLY_3_2(...) RMOD(DQS_IN_DLY_3_2, __VA_ARGS__)
#define RIZS_DQS_IN_DLY_3_2(...) RIZS(DQS_IN_DLY_3_2, __VA_ARGS__)
#define RFLD_DQS_IN_DLY_3_2(fld) RFLD(DQS_IN_DLY_3_2, fld)

typedef union {
	struct {
		unsigned int mbz_0:3; //0x0
		unsigned int fw_rd_dly_neg_sel_7:5; //0xA
		unsigned int mbz_1:3; //0x0
		unsigned int fw_rd_dly_neg_sel_6:5; //0xA
		unsigned int mbz_2:3; //0x0
		unsigned int fw_rd_dly_neg_sel_5:5; //0xA
		unsigned int mbz_3:3; //0x0
		unsigned int fw_rd_dly_neg_sel_4:5; //0xA
	} f;
	unsigned int v;
} DQS_IN_DLY_3_3_T;
#define DQS_IN_DLY_3_3ar (0xB81422C8)
#define DQS_IN_DLY_3_3dv (0x0A0A0A0A)
#define DQS_IN_DLY_3_3rv RVAL(DQS_IN_DLY_3_3)
#define RMOD_DQS_IN_DLY_3_3(...) RMOD(DQS_IN_DLY_3_3, __VA_ARGS__)
#define RIZS_DQS_IN_DLY_3_3(...) RIZS(DQS_IN_DLY_3_3, __VA_ARGS__)
#define RFLD_DQS_IN_DLY_3_3(fld) RFLD(DQS_IN_DLY_3_3, fld)

typedef union {
	struct {
		unsigned int mbz_0:8; //0x0
		unsigned int vref_range_2:1; //0x1
		unsigned int vref_s_2:6; //0xA
		unsigned int vref_pd_2:1; //0x0
		unsigned int vref_range_1:1; //0x1
		unsigned int vref_s_1:6; //0xA
		unsigned int vref_pd_1:1; //0x0
		unsigned int vref_range_0:1; //0x1
		unsigned int vref_s_0:6; //0xA
		unsigned int vref_pd_0:1; //0x0
	} f;
	unsigned int v;
} PAD_BUS_2_T;
#define PAD_BUS_2ar (0xB81422CC)
#define PAD_BUS_2dv (0x00949494)
#define PAD_BUS_2rv RVAL(PAD_BUS_2)
#define RMOD_PAD_BUS_2(...) RMOD(PAD_BUS_2, __VA_ARGS__)
#define RIZS_PAD_BUS_2(...) RIZS(PAD_BUS_2, __VA_ARGS__)
#define RFLD_PAD_BUS_2(fld) RFLD(PAD_BUS_2, fld)

typedef union {
	struct {
		unsigned int mbz_0:4; //0x0
		unsigned int dt_set_0:28; //0x0
	} f;
	unsigned int v;
} DT_SET0_T;
#define DT_SET0ar (0xB81422D0)
#define DT_SET0dv (0x00000000)
#define DT_SET0rv RVAL(DT_SET0)
#define RMOD_DT_SET0(...) RMOD(DT_SET0, __VA_ARGS__)
#define RIZS_DT_SET0(...) RIZS(DT_SET0, __VA_ARGS__)
#define RFLD_DT_SET0(fld) RFLD(DT_SET0, fld)

typedef union {
	struct {
		unsigned int mbz_0:1; //0x0
		unsigned int dq_1_ttfn_sel:3; //0x0
		unsigned int mbz_1:1; //0x0
		unsigned int dq_0_ttfn_sel:3; //0x0
		unsigned int mbz_2:1; //0x0
		unsigned int dq_1_ttfp_sel:3; //0x0
		unsigned int mbz_3:1; //0x0
		unsigned int dq_0_ttfp_sel:3; //0x0
		unsigned int mbz_4:1; //0x0
		unsigned int dq_1_ttcn_sel:3; //0x0
		unsigned int mbz_5:1; //0x0
		unsigned int dq_0_ttcn_sel:3; //0x0
		unsigned int mbz_6:1; //0x0
		unsigned int dq_1_ttcp_sel:3; //0x0
		unsigned int mbz_7:1; //0x0
		unsigned int dq_0_ttcp_sel:3; //0x0
	} f;
	unsigned int v;
} DQ_ODT_SEL_0_T;
#define DQ_ODT_SEL_0ar (0xB81422D4)
#define DQ_ODT_SEL_0dv (0x00000000)
#define DQ_ODT_SEL_0rv RVAL(DQ_ODT_SEL_0)
#define RMOD_DQ_ODT_SEL_0(...) RMOD(DQ_ODT_SEL_0, __VA_ARGS__)
#define RIZS_DQ_ODT_SEL_0(...) RIZS(DQ_ODT_SEL_0, __VA_ARGS__)
#define RFLD_DQ_ODT_SEL_0(fld) RFLD(DQ_ODT_SEL_0, fld)

typedef union {
	struct {
		unsigned int mbz_0:1; //0x0
		unsigned int dq_1_ttfn_sel:3; //0x0
		unsigned int mbz_1:1; //0x0
		unsigned int dq_0_ttfn_sel:3; //0x0
		unsigned int mbz_2:1; //0x0
		unsigned int dq_1_ttfp_sel:3; //0x0
		unsigned int mbz_3:1; //0x0
		unsigned int dq_0_ttfp_sel:3; //0x0
		unsigned int mbz_4:1; //0x0
		unsigned int dq_1_ttcn_sel:3; //0x0
		unsigned int mbz_5:1; //0x0
		unsigned int dq_0_ttcn_sel:3; //0x0
		unsigned int mbz_6:1; //0x0
		unsigned int dq_1_ttcp_sel:3; //0x0
		unsigned int mbz_7:1; //0x0
		unsigned int dq_0_ttcp_sel:3; //0x0
	} f;
	unsigned int v;
} DQ_ODT_SEL_1_T;
#define DQ_ODT_SEL_1ar (0xB81422D8)
#define DQ_ODT_SEL_1dv (0x00000000)
#define DQ_ODT_SEL_1rv RVAL(DQ_ODT_SEL_1)
#define RMOD_DQ_ODT_SEL_1(...) RMOD(DQ_ODT_SEL_1, __VA_ARGS__)
#define RIZS_DQ_ODT_SEL_1(...) RIZS(DQ_ODT_SEL_1, __VA_ARGS__)
#define RFLD_DQ_ODT_SEL_1(fld) RFLD(DQ_ODT_SEL_1, fld)

typedef union {
	struct {
		unsigned int mbz_0:3; //0x0
		unsigned int odt_ttcp_set_3:5; //0x16
		unsigned int mbz_1:3; //0x0
		unsigned int odt_ttcp_set_2:5; //0x16
		unsigned int mbz_2:3; //0x0
		unsigned int odt_ttcp_set_1:5; //0x16
		unsigned int mbz_3:3; //0x0
		unsigned int odt_ttcp_set_0:5; //0x16
	} f;
	unsigned int v;
} ODT_TTCP0_SET1_T;
#define ODT_TTCP0_SET1ar (0xB81422E0)
#define ODT_TTCP0_SET1dv (0x16161616)
#define ODT_TTCP0_SET1rv RVAL(ODT_TTCP0_SET1)
#define RMOD_ODT_TTCP0_SET1(...) RMOD(ODT_TTCP0_SET1, __VA_ARGS__)
#define RIZS_ODT_TTCP0_SET1(...) RIZS(ODT_TTCP0_SET1, __VA_ARGS__)
#define RFLD_ODT_TTCP0_SET1(fld) RFLD(ODT_TTCP0_SET1, fld)

typedef union {
	struct {
		unsigned int odt_ttcn_set_7:4; //0x0
		unsigned int odt_ttcn_set_6:4; //0x6
		unsigned int odt_ttcn_set_5:4; //0x6
		unsigned int odt_ttcn_set_4:4; //0x6
		unsigned int odt_ttcn_set_3:4; //0x6
		unsigned int odt_ttcn_set_2:4; //0x6
		unsigned int odt_ttcn_set_1:4; //0x6
		unsigned int odt_ttcn_set_0:4; //0x6
	} f;
	unsigned int v;
} ODT_TTCN_SET1_T;
#define ODT_TTCN_SET1ar (0xB81422E4)
#define ODT_TTCN_SET1dv (0x06666666)
#define ODT_TTCN_SET1rv RVAL(ODT_TTCN_SET1)
#define RMOD_ODT_TTCN_SET1(...) RMOD(ODT_TTCN_SET1, __VA_ARGS__)
#define RIZS_ODT_TTCN_SET1(...) RIZS(ODT_TTCN_SET1, __VA_ARGS__)
#define RFLD_ODT_TTCN_SET1(fld) RFLD(ODT_TTCN_SET1, fld)

typedef union {
	struct {
		unsigned int mbz_0:2; //0x0
		unsigned int odt_ttfp_set_7:2; //0x0
		unsigned int mbz_1:2; //0x0
		unsigned int odt_ttfp_set_6:2; //0x2
		unsigned int mbz_2:2; //0x0
		unsigned int odt_ttfp_set_5:2; //0x2
		unsigned int mbz_3:2; //0x0
		unsigned int odt_ttfp_set_4:2; //0x2
		unsigned int mbz_4:2; //0x0
		unsigned int odt_ttfp_set_3:2; //0x2
		unsigned int mbz_5:2; //0x0
		unsigned int odt_ttfp_set_2:2; //0x2
		unsigned int mbz_6:2; //0x0
		unsigned int odt_ttfp_set_1:2; //0x2
		unsigned int mbz_7:2; //0x0
		unsigned int odt_ttfp_set_0:2; //0x2
	} f;
	unsigned int v;
} ODT_TTFP_SET1_T;
#define ODT_TTFP_SET1ar (0xB81422E8)
#define ODT_TTFP_SET1dv (0x02222222)
#define ODT_TTFP_SET1rv RVAL(ODT_TTFP_SET1)
#define RMOD_ODT_TTFP_SET1(...) RMOD(ODT_TTFP_SET1, __VA_ARGS__)
#define RIZS_ODT_TTFP_SET1(...) RIZS(ODT_TTFP_SET1, __VA_ARGS__)
#define RFLD_ODT_TTFP_SET1(fld) RFLD(ODT_TTFP_SET1, fld)

typedef union {
	struct {
		unsigned int mbz_0:1; //0x0
		unsigned int odt_ttfn_set_7:3; //0x0
		unsigned int mbz_1:1; //0x0
		unsigned int odt_ttfn_set_6:3; //0x4
		unsigned int mbz_2:1; //0x0
		unsigned int odt_ttfn_set_5:3; //0x4
		unsigned int mbz_3:1; //0x0
		unsigned int odt_ttfn_set_4:3; //0x4
		unsigned int mbz_4:1; //0x0
		unsigned int odt_ttfn_set_3:3; //0x4
		unsigned int mbz_5:1; //0x0
		unsigned int odt_ttfn_set_2:3; //0x4
		unsigned int mbz_6:1; //0x0
		unsigned int odt_ttfn_set_1:3; //0x4
		unsigned int mbz_7:1; //0x0
		unsigned int odt_ttfn_set_0:3; //0x4
	} f;
	unsigned int v;
} ODT_TTFN_SET1_T;
#define ODT_TTFN_SET1ar (0xB81422EC)
#define ODT_TTFN_SET1dv (0x04444444)
#define ODT_TTFN_SET1rv RVAL(ODT_TTFN_SET1)
#define RMOD_ODT_TTFN_SET1(...) RMOD(ODT_TTFN_SET1, __VA_ARGS__)
#define RIZS_ODT_TTFN_SET1(...) RIZS(ODT_TTFN_SET1, __VA_ARGS__)
#define RFLD_ODT_TTFN_SET1(fld) RFLD(ODT_TTFN_SET1, fld)

typedef union {
	struct {
		unsigned int mbz_0:17; //0x0
		unsigned int cke_1_ocdn_sel:3; //0x1
		unsigned int mbz_1:1; //0x0
		unsigned int cke_1_ocdp_sel:3; //0x1
		unsigned int mbz_2:1; //0x0
		unsigned int cke_ocdn_sel:3; //0x1
		unsigned int mbz_3:1; //0x0
		unsigned int cke_ocdp_sel:3; //0x1
	} f;
	unsigned int v;
} CKE_OCD_SEL_T;
#define CKE_OCD_SELar (0xB81422F0)
#define CKE_OCD_SELdv (0x00001111)
#define CKE_OCD_SELrv RVAL(CKE_OCD_SEL)
#define RMOD_CKE_OCD_SEL(...) RMOD(CKE_OCD_SEL, __VA_ARGS__)
#define RIZS_CKE_OCD_SEL(...) RIZS(CKE_OCD_SEL, __VA_ARGS__)
#define RFLD_CKE_OCD_SEL(fld) RFLD(CKE_OCD_SEL, fld)

typedef union {
	struct {
		unsigned int mbz_0:1; //0x0
		unsigned int cs_1_ocdn_sel:3; //0x1
		unsigned int mbz_1:1; //0x0
		unsigned int cs_1_ocdp_sel:3; //0x1
		unsigned int mbz_2:1; //0x0
		unsigned int adr45_ocdn_sel:3; //0x1
		unsigned int mbz_3:1; //0x0
		unsigned int adr45_ocdp_sel:3; //0x1
		unsigned int mbz_4:1; //0x0
		unsigned int cs_ocdn_sel:3; //0x1
		unsigned int mbz_5:1; //0x0
		unsigned int cs_ocdp_sel:3; //0x1
		unsigned int mbz_6:1; //0x0
		unsigned int adr_ocdn_sel:3; //0x1
		unsigned int mbz_7:1; //0x0
		unsigned int adr_ocdp_sel:3; //0x1
	} f;
	unsigned int v;
} ADR_OCD_SEL_T;
#define ADR_OCD_SELar (0xB81422F4)
#define ADR_OCD_SELdv (0x11111111)
#define ADR_OCD_SELrv RVAL(ADR_OCD_SEL)
#define RMOD_ADR_OCD_SEL(...) RMOD(ADR_OCD_SEL, __VA_ARGS__)
#define RIZS_ADR_OCD_SEL(...) RIZS(ADR_OCD_SEL, __VA_ARGS__)
#define RFLD_ADR_OCD_SEL(fld) RFLD(ADR_OCD_SEL, fld)

typedef union {
	struct {
		unsigned int mbz_0:1; //0x0
		unsigned int ck_n_1_ocdn_sel:3; //0x2
		unsigned int mbz_1:1; //0x0
		unsigned int ck_n_0_ocdn_sel:3; //0x2
		unsigned int mbz_2:1; //0x0
		unsigned int ck_n_1_ocdp_sel:3; //0x2
		unsigned int mbz_3:1; //0x0
		unsigned int ck_n_0_ocdp_sel:3; //0x2
		unsigned int mbz_4:1; //0x0
		unsigned int ck_p_1_ocdn_sel:3; //0x2
		unsigned int mbz_5:1; //0x0
		unsigned int ck_p_0_ocdn_sel:3; //0x2
		unsigned int mbz_6:1; //0x0
		unsigned int ck_p_1_ocdp_sel:3; //0x2
		unsigned int mbz_7:1; //0x0
		unsigned int ck_p_0_ocdp_sel:3; //0x2
	} f;
	unsigned int v;
} CK_OCD_SEL_T;
#define CK_OCD_SELar (0xB81422F8)
#define CK_OCD_SELdv (0x22222222)
#define CK_OCD_SELrv RVAL(CK_OCD_SEL)
#define RMOD_CK_OCD_SEL(...) RMOD(CK_OCD_SEL, __VA_ARGS__)
#define RIZS_CK_OCD_SEL(...) RIZS(CK_OCD_SEL, __VA_ARGS__)
#define RFLD_CK_OCD_SEL(fld) RFLD(CK_OCD_SEL, fld)

typedef union {
	struct {
		unsigned int en_dcc:1; //0x0
		unsigned int pd_ck:1; //0x1
		unsigned int en_dmyi:1; //0x0
		unsigned int vref_sel:1; //0x0
		unsigned int vref_range:1; //0x1
		unsigned int vref_s:6; //0xA
		unsigned int vref_currentsel:2; //0x3
		unsigned int vref_pd:1; //0x0
		unsigned int pad_bus_0:18; //0x0
	} f;
	unsigned int v;
} PAD_BUS_0_T;
#define PAD_BUS_0ar (0xB81422FC)
#define PAD_BUS_0dv (0x49580000)
#define PAD_BUS_0rv RVAL(PAD_BUS_0)
#define RMOD_PAD_BUS_0(...) RMOD(PAD_BUS_0, __VA_ARGS__)
#define RIZS_PAD_BUS_0(...) RIZS(PAD_BUS_0, __VA_ARGS__)
#define RFLD_PAD_BUS_0(fld) RFLD(PAD_BUS_0, fld)

typedef union {
	struct {
		unsigned int mbz_0:23; //0x0
		unsigned int ck0_peaking_en:1; //0x1
		unsigned int ck1_peaking_en:1; //0x1
		unsigned int pad_bus_1:7; //0x0
	} f;
	unsigned int v;
} PAD_BUS_1_T;
#define PAD_BUS_1ar (0xB8142300)
#define PAD_BUS_1dv (0x00000180)
#define PAD_BUS_1rv RVAL(PAD_BUS_1)
#define RMOD_PAD_BUS_1(...) RMOD(PAD_BUS_1, __VA_ARGS__)
#define RIZS_PAD_BUS_1(...) RIZS(PAD_BUS_1, __VA_ARGS__)
#define RFLD_PAD_BUS_1(fld) RFLD(PAD_BUS_1, fld)

typedef union {
	struct {
		unsigned int mbz_0:28; //0x0
		unsigned int trim_mode:4; //0x0
	} f;
	unsigned int v;
} ZQCMP_T;
#define ZQCMPar (0xB8142304)
#define ZQCMPdv (0x00000000)
#define ZQCMPrv RVAL(ZQCMP)
#define RMOD_ZQCMP(...) RMOD(ZQCMP, __VA_ARGS__)
#define RIZS_ZQCMP(...) RIZS(ZQCMP, __VA_ARGS__)
#define RFLD_ZQCMP(fld) RFLD(ZQCMP, fld)

typedef union {
	struct {
		unsigned int mbz_0:3; //0x0
		unsigned int odt_ttcp_set_3_pre:5; //0x16
		unsigned int mbz_1:3; //0x0
		unsigned int odt_ttcp_set_2_pre:5; //0x16
		unsigned int mbz_2:3; //0x0
		unsigned int odt_ttcp_set_1_pre:5; //0x16
		unsigned int mbz_3:3; //0x0
		unsigned int odt_ttcp_set_0_pre:5; //0x16
	} f;
	unsigned int v;
} ODT_TTCP0_SET0_T;
#define ODT_TTCP0_SET0ar (0xB8142308)
#define ODT_TTCP0_SET0dv (0x16161616)
#define ODT_TTCP0_SET0rv RVAL(ODT_TTCP0_SET0)
#define RMOD_ODT_TTCP0_SET0(...) RMOD(ODT_TTCP0_SET0, __VA_ARGS__)
#define RIZS_ODT_TTCP0_SET0(...) RIZS(ODT_TTCP0_SET0, __VA_ARGS__)
#define RFLD_ODT_TTCP0_SET0(fld) RFLD(ODT_TTCP0_SET0, fld)

typedef union {
	struct {
		unsigned int odt_ttcn_set_7_pre:4; //0x0
		unsigned int odt_ttcn_set_6_pre:4; //0x6
		unsigned int odt_ttcn_set_5_pre:4; //0x6
		unsigned int odt_ttcn_set_4_pre:4; //0x6
		unsigned int odt_ttcn_set_3_pre:4; //0x6
		unsigned int odt_ttcn_set_2_pre:4; //0x6
		unsigned int odt_ttcn_set_1_pre:4; //0x6
		unsigned int odt_ttcn_set_0_pre:4; //0x6
	} f;
	unsigned int v;
} ODT_TTCN_SET0_T;
#define ODT_TTCN_SET0ar (0xB814230C)
#define ODT_TTCN_SET0dv (0x06666666)
#define ODT_TTCN_SET0rv RVAL(ODT_TTCN_SET0)
#define RMOD_ODT_TTCN_SET0(...) RMOD(ODT_TTCN_SET0, __VA_ARGS__)
#define RIZS_ODT_TTCN_SET0(...) RIZS(ODT_TTCN_SET0, __VA_ARGS__)
#define RFLD_ODT_TTCN_SET0(fld) RFLD(ODT_TTCN_SET0, fld)

typedef union {
	struct {
		unsigned int mbz_0:2; //0x0
		unsigned int odt_ttfp_set_7_pre:2; //0x0
		unsigned int mbz_1:2; //0x0
		unsigned int odt_ttfp_set_6_pre:2; //0x2
		unsigned int mbz_2:2; //0x0
		unsigned int odt_ttfp_set_5_pre:2; //0x2
		unsigned int mbz_3:2; //0x0
		unsigned int odt_ttfp_set_4_pre:2; //0x2
		unsigned int mbz_4:2; //0x0
		unsigned int odt_ttfp_set_3_pre:2; //0x2
		unsigned int mbz_5:2; //0x0
		unsigned int odt_ttfp_set_2_pre:2; //0x2
		unsigned int mbz_6:2; //0x0
		unsigned int odt_ttfp_set_1_pre:2; //0x2
		unsigned int mbz_7:2; //0x0
		unsigned int odt_ttfp_set_0_pre:2; //0x2
	} f;
	unsigned int v;
} ODT_TTFP_SET0_T;
#define ODT_TTFP_SET0ar (0xB8142310)
#define ODT_TTFP_SET0dv (0x02222222)
#define ODT_TTFP_SET0rv RVAL(ODT_TTFP_SET0)
#define RMOD_ODT_TTFP_SET0(...) RMOD(ODT_TTFP_SET0, __VA_ARGS__)
#define RIZS_ODT_TTFP_SET0(...) RIZS(ODT_TTFP_SET0, __VA_ARGS__)
#define RFLD_ODT_TTFP_SET0(fld) RFLD(ODT_TTFP_SET0, fld)

typedef union {
	struct {
		unsigned int mbz_0:1; //0x0
		unsigned int odt_ttfn_set_7_pre:3; //0x0
		unsigned int mbz_1:1; //0x0
		unsigned int odt_ttfn_set_6_pre:3; //0x4
		unsigned int mbz_2:1; //0x0
		unsigned int odt_ttfn_set_5_pre:3; //0x4
		unsigned int mbz_3:1; //0x0
		unsigned int odt_ttfn_set_4_pre:3; //0x4
		unsigned int mbz_4:1; //0x0
		unsigned int odt_ttfn_set_3_pre:3; //0x4
		unsigned int mbz_5:1; //0x0
		unsigned int odt_ttfn_set_2_pre:3; //0x4
		unsigned int mbz_6:1; //0x0
		unsigned int odt_ttfn_set_1_pre:3; //0x4
		unsigned int mbz_7:1; //0x0
		unsigned int odt_ttfn_set_0_pre:3; //0x4
	} f;
	unsigned int v;
} ODT_TTFN_SET0_T;
#define ODT_TTFN_SET0ar (0xB8142314)
#define ODT_TTFN_SET0dv (0x04444444)
#define ODT_TTFN_SET0rv RVAL(ODT_TTFN_SET0)
#define RMOD_ODT_TTFN_SET0(...) RMOD(ODT_TTFN_SET0, __VA_ARGS__)
#define RIZS_ODT_TTFN_SET0(...) RIZS(ODT_TTFN_SET0, __VA_ARGS__)
#define RFLD_ODT_TTFN_SET0(fld) RFLD(ODT_TTFN_SET0, fld)

typedef union {
	struct {
		unsigned int dqm_oe_pre0p5t:1; //0x0
		unsigned int en_rd_dbi:1; //0x0
		unsigned int en_crc:1; //0x0
		unsigned int _2t_wr_pre:1; //0x0
		unsigned int dqs_oe_mask:1; //0x1
		unsigned int mbz_0:5; //0x0
		unsigned int data_pre:1; //0x0
		unsigned int data_post:1; //0x0
		unsigned int cal_dqsen_update_ena:1; //0x1
		unsigned int cal_dqsen_mode:2; //0x3
		unsigned int dqs_ie_sel:1; //0x1
		unsigned int _2t_rd_pre:1; //0x0
		unsigned int ref_cmd_sel:1; //0x0
		unsigned int ref_update_dly:6; //0x20
		unsigned int dqsen_mode:2; //0x0
		unsigned int cal_set_mode:2; //0x0
		unsigned int rst_fifo_mode:2; //0x1
		unsigned int fw_set_mode:2; //0x0
	} f;
	unsigned int v;
} DPI_CTRL_0_T;
#define DPI_CTRL_0ar (0xB8142318)
#define DPI_CTRL_0dv (0x080F2004)
#define DPI_CTRL_0rv RVAL(DPI_CTRL_0)
#define RMOD_DPI_CTRL_0(...) RMOD(DPI_CTRL_0, __VA_ARGS__)
#define RIZS_DPI_CTRL_0(...) RIZS(DPI_CTRL_0, __VA_ARGS__)
#define RFLD_DPI_CTRL_0(fld) RFLD(DPI_CTRL_0, fld)

typedef union {
	struct {
		unsigned int mbz_0:24; //0x0
		unsigned int write_en_3:1; //0x0
		unsigned int fw_set_rd_det:1; //0x0
		unsigned int write_en_2:1; //0x0
		unsigned int fw_set_cal_range:1; //0x0
		unsigned int write_en_1:1; //0x0
		unsigned int fw_set_rd_dly:1; //0x0
		unsigned int write_en_0:1; //0x0
		unsigned int fw_set_wr_dly:1; //0x0
	} f;
	unsigned int v;
} DPI_CTRL_1_T;
#define DPI_CTRL_1ar (0xB814231C)
#define DPI_CTRL_1dv (0x00000000)
#define DPI_CTRL_1rv RVAL(DPI_CTRL_1)
#define RMOD_DPI_CTRL_1(...) RMOD(DPI_CTRL_1, __VA_ARGS__)
#define RIZS_DPI_CTRL_1(...) RIZS(DPI_CTRL_1, __VA_ARGS__)
#define RFLD_DPI_CTRL_1(fld) RFLD(DPI_CTRL_1, fld)

typedef union {
	struct {
		unsigned int mbz_0:20; //0x0
		unsigned int cal_rd_dly_out_sel_3:3; //0x0
		unsigned int cal_rd_dly_out_sel_2:3; //0x0
		unsigned int cal_rd_dly_out_sel_1:3; //0x0
		unsigned int cal_rd_dly_out_sel_0:3; //0x0
	} f;
	unsigned int v;
} CAL_OUT_SEL_T;
#define CAL_OUT_SELar (0xB8142320)
#define CAL_OUT_SELdv (0x00000000)
#define CAL_OUT_SELrv RVAL(CAL_OUT_SEL)
#define RMOD_CAL_OUT_SEL(...) RMOD(CAL_OUT_SEL, __VA_ARGS__)
#define RIZS_CAL_OUT_SEL(...) RIZS(CAL_OUT_SEL, __VA_ARGS__)
#define RFLD_CAL_OUT_SEL(fld) RFLD(CAL_OUT_SEL, fld)

typedef union {
	struct {
		unsigned int cal_rd_dly_out:32; //0x0
	} f;
	unsigned int v;
} CAL_OUT_0_0_T;
#define CAL_OUT_0_0ar (0xB8142324)
#define CAL_OUT_0_0dv (0x00000000)
#define CAL_OUT_0_0rv RVAL(CAL_OUT_0_0)
#define RMOD_CAL_OUT_0_0(...) RMOD(CAL_OUT_0_0, __VA_ARGS__)
#define RIZS_CAL_OUT_0_0(...) RIZS(CAL_OUT_0_0, __VA_ARGS__)
#define RFLD_CAL_OUT_0_0(fld) RFLD(CAL_OUT_0_0, fld)

typedef union {
	struct {
		unsigned int cal_rd_dly_out:32; //0x0
	} f;
	unsigned int v;
} CAL_OUT_0_1_T;
#define CAL_OUT_0_1ar (0xB8142328)
#define CAL_OUT_0_1dv (0x00000000)
#define CAL_OUT_0_1rv RVAL(CAL_OUT_0_1)
#define RMOD_CAL_OUT_0_1(...) RMOD(CAL_OUT_0_1, __VA_ARGS__)
#define RIZS_CAL_OUT_0_1(...) RIZS(CAL_OUT_0_1, __VA_ARGS__)
#define RFLD_CAL_OUT_0_1(fld) RFLD(CAL_OUT_0_1, fld)

typedef union {
	struct {
		unsigned int cal_rd_dly_out:32; //0x0
	} f;
	unsigned int v;
} CAL_OUT_0_2_T;
#define CAL_OUT_0_2ar (0xB814232C)
#define CAL_OUT_0_2dv (0x00000000)
#define CAL_OUT_0_2rv RVAL(CAL_OUT_0_2)
#define RMOD_CAL_OUT_0_2(...) RMOD(CAL_OUT_0_2, __VA_ARGS__)
#define RIZS_CAL_OUT_0_2(...) RIZS(CAL_OUT_0_2, __VA_ARGS__)
#define RFLD_CAL_OUT_0_2(fld) RFLD(CAL_OUT_0_2, fld)

typedef union {
	struct {
		unsigned int cal_rd_dly_out:32; //0x0
	} f;
	unsigned int v;
} CAL_OUT_0_3_T;
#define CAL_OUT_0_3ar (0xB8142330)
#define CAL_OUT_0_3dv (0x00000000)
#define CAL_OUT_0_3rv RVAL(CAL_OUT_0_3)
#define RMOD_CAL_OUT_0_3(...) RMOD(CAL_OUT_0_3, __VA_ARGS__)
#define RIZS_CAL_OUT_0_3(...) RIZS(CAL_OUT_0_3, __VA_ARGS__)
#define RFLD_CAL_OUT_0_3(fld) RFLD(CAL_OUT_0_3, fld)

typedef union {
	struct {
		unsigned int wr_adr_pos:32; //0x0
	} f;
	unsigned int v;
} WR_FIFO_0_0_T;
#define WR_FIFO_0_0ar (0xB8142334)
#define WR_FIFO_0_0dv (0x00000000)
#define WR_FIFO_0_0rv RVAL(WR_FIFO_0_0)
#define RMOD_WR_FIFO_0_0(...) RMOD(WR_FIFO_0_0, __VA_ARGS__)
#define RIZS_WR_FIFO_0_0(...) RIZS(WR_FIFO_0_0, __VA_ARGS__)
#define RFLD_WR_FIFO_0_0(fld) RFLD(WR_FIFO_0_0, fld)

typedef union {
	struct {
		unsigned int wr_adr_pos:32; //0x0
	} f;
	unsigned int v;
} WR_FIFO_0_1_T;
#define WR_FIFO_0_1ar (0xB8142338)
#define WR_FIFO_0_1dv (0x00000000)
#define WR_FIFO_0_1rv RVAL(WR_FIFO_0_1)
#define RMOD_WR_FIFO_0_1(...) RMOD(WR_FIFO_0_1, __VA_ARGS__)
#define RIZS_WR_FIFO_0_1(...) RIZS(WR_FIFO_0_1, __VA_ARGS__)
#define RFLD_WR_FIFO_0_1(fld) RFLD(WR_FIFO_0_1, fld)

typedef union {
	struct {
		unsigned int wr_adr_pos:32; //0x0
	} f;
	unsigned int v;
} WR_FIFO_0_2_T;
#define WR_FIFO_0_2ar (0xB814233C)
#define WR_FIFO_0_2dv (0x00000000)
#define WR_FIFO_0_2rv RVAL(WR_FIFO_0_2)
#define RMOD_WR_FIFO_0_2(...) RMOD(WR_FIFO_0_2, __VA_ARGS__)
#define RIZS_WR_FIFO_0_2(...) RIZS(WR_FIFO_0_2, __VA_ARGS__)
#define RFLD_WR_FIFO_0_2(fld) RFLD(WR_FIFO_0_2, fld)

typedef union {
	struct {
		unsigned int wr_adr_pos:32; //0x0
	} f;
	unsigned int v;
} WR_FIFO_0_3_T;
#define WR_FIFO_0_3ar (0xB8142340)
#define WR_FIFO_0_3dv (0x00000000)
#define WR_FIFO_0_3rv RVAL(WR_FIFO_0_3)
#define RMOD_WR_FIFO_0_3(...) RMOD(WR_FIFO_0_3, __VA_ARGS__)
#define RIZS_WR_FIFO_0_3(...) RIZS(WR_FIFO_0_3, __VA_ARGS__)
#define RFLD_WR_FIFO_0_3(fld) RFLD(WR_FIFO_0_3, fld)

typedef union {
	struct {
		unsigned int wr_adr_neg:32; //0x0
	} f;
	unsigned int v;
} WR_FIFO_1_0_T;
#define WR_FIFO_1_0ar (0xB8142344)
#define WR_FIFO_1_0dv (0x00000000)
#define WR_FIFO_1_0rv RVAL(WR_FIFO_1_0)
#define RMOD_WR_FIFO_1_0(...) RMOD(WR_FIFO_1_0, __VA_ARGS__)
#define RIZS_WR_FIFO_1_0(...) RIZS(WR_FIFO_1_0, __VA_ARGS__)
#define RFLD_WR_FIFO_1_0(fld) RFLD(WR_FIFO_1_0, fld)

typedef union {
	struct {
		unsigned int wr_adr_neg:32; //0x0
	} f;
	unsigned int v;
} WR_FIFO_1_1_T;
#define WR_FIFO_1_1ar (0xB8142348)
#define WR_FIFO_1_1dv (0x00000000)
#define WR_FIFO_1_1rv RVAL(WR_FIFO_1_1)
#define RMOD_WR_FIFO_1_1(...) RMOD(WR_FIFO_1_1, __VA_ARGS__)
#define RIZS_WR_FIFO_1_1(...) RIZS(WR_FIFO_1_1, __VA_ARGS__)
#define RFLD_WR_FIFO_1_1(fld) RFLD(WR_FIFO_1_1, fld)

typedef union {
	struct {
		unsigned int wr_adr_neg:32; //0x0
	} f;
	unsigned int v;
} WR_FIFO_1_2_T;
#define WR_FIFO_1_2ar (0xB814234C)
#define WR_FIFO_1_2dv (0x00000000)
#define WR_FIFO_1_2rv RVAL(WR_FIFO_1_2)
#define RMOD_WR_FIFO_1_2(...) RMOD(WR_FIFO_1_2, __VA_ARGS__)
#define RIZS_WR_FIFO_1_2(...) RIZS(WR_FIFO_1_2, __VA_ARGS__)
#define RFLD_WR_FIFO_1_2(fld) RFLD(WR_FIFO_1_2, fld)

typedef union {
	struct {
		unsigned int wr_adr_neg:32; //0x0
	} f;
	unsigned int v;
} WR_FIFO_1_3_T;
#define WR_FIFO_1_3ar (0xB8142350)
#define WR_FIFO_1_3dv (0x00000000)
#define WR_FIFO_1_3rv RVAL(WR_FIFO_1_3)
#define RMOD_WR_FIFO_1_3(...) RMOD(WR_FIFO_1_3, __VA_ARGS__)
#define RIZS_WR_FIFO_1_3(...) RIZS(WR_FIFO_1_3, __VA_ARGS__)
#define RFLD_WR_FIFO_1_3(fld) RFLD(WR_FIFO_1_3, fld)

typedef union {
	struct {
		unsigned int rd_adr_pos:32; //0x0
	} f;
	unsigned int v;
} RD_FIFO_0_0_T;
#define RD_FIFO_0_0ar (0xB8142354)
#define RD_FIFO_0_0dv (0x00000000)
#define RD_FIFO_0_0rv RVAL(RD_FIFO_0_0)
#define RMOD_RD_FIFO_0_0(...) RMOD(RD_FIFO_0_0, __VA_ARGS__)
#define RIZS_RD_FIFO_0_0(...) RIZS(RD_FIFO_0_0, __VA_ARGS__)
#define RFLD_RD_FIFO_0_0(fld) RFLD(RD_FIFO_0_0, fld)

typedef union {
	struct {
		unsigned int rd_adr_pos:32; //0x0
	} f;
	unsigned int v;
} RD_FIFO_0_1_T;
#define RD_FIFO_0_1ar (0xB8142358)
#define RD_FIFO_0_1dv (0x00000000)
#define RD_FIFO_0_1rv RVAL(RD_FIFO_0_1)
#define RMOD_RD_FIFO_0_1(...) RMOD(RD_FIFO_0_1, __VA_ARGS__)
#define RIZS_RD_FIFO_0_1(...) RIZS(RD_FIFO_0_1, __VA_ARGS__)
#define RFLD_RD_FIFO_0_1(fld) RFLD(RD_FIFO_0_1, fld)

typedef union {
	struct {
		unsigned int rd_adr_pos:32; //0x0
	} f;
	unsigned int v;
} RD_FIFO_0_2_T;
#define RD_FIFO_0_2ar (0xB814235C)
#define RD_FIFO_0_2dv (0x00000000)
#define RD_FIFO_0_2rv RVAL(RD_FIFO_0_2)
#define RMOD_RD_FIFO_0_2(...) RMOD(RD_FIFO_0_2, __VA_ARGS__)
#define RIZS_RD_FIFO_0_2(...) RIZS(RD_FIFO_0_2, __VA_ARGS__)
#define RFLD_RD_FIFO_0_2(fld) RFLD(RD_FIFO_0_2, fld)

typedef union {
	struct {
		unsigned int rd_adr_pos:32; //0x0
	} f;
	unsigned int v;
} RD_FIFO_0_3_T;
#define RD_FIFO_0_3ar (0xB8142360)
#define RD_FIFO_0_3dv (0x00000000)
#define RD_FIFO_0_3rv RVAL(RD_FIFO_0_3)
#define RMOD_RD_FIFO_0_3(...) RMOD(RD_FIFO_0_3, __VA_ARGS__)
#define RIZS_RD_FIFO_0_3(...) RIZS(RD_FIFO_0_3, __VA_ARGS__)
#define RFLD_RD_FIFO_0_3(fld) RFLD(RD_FIFO_0_3, fld)

typedef union {
	struct {
		unsigned int rd_adr_neg:32; //0x0
	} f;
	unsigned int v;
} RD_FIFO_1_0_T;
#define RD_FIFO_1_0ar (0xB8142364)
#define RD_FIFO_1_0dv (0x00000000)
#define RD_FIFO_1_0rv RVAL(RD_FIFO_1_0)
#define RMOD_RD_FIFO_1_0(...) RMOD(RD_FIFO_1_0, __VA_ARGS__)
#define RIZS_RD_FIFO_1_0(...) RIZS(RD_FIFO_1_0, __VA_ARGS__)
#define RFLD_RD_FIFO_1_0(fld) RFLD(RD_FIFO_1_0, fld)

typedef union {
	struct {
		unsigned int rd_adr_neg:32; //0x0
	} f;
	unsigned int v;
} RD_FIFO_1_1_T;
#define RD_FIFO_1_1ar (0xB8142368)
#define RD_FIFO_1_1dv (0x00000000)
#define RD_FIFO_1_1rv RVAL(RD_FIFO_1_1)
#define RMOD_RD_FIFO_1_1(...) RMOD(RD_FIFO_1_1, __VA_ARGS__)
#define RIZS_RD_FIFO_1_1(...) RIZS(RD_FIFO_1_1, __VA_ARGS__)
#define RFLD_RD_FIFO_1_1(fld) RFLD(RD_FIFO_1_1, fld)

typedef union {
	struct {
		unsigned int rd_adr_neg:32; //0x0
	} f;
	unsigned int v;
} RD_FIFO_1_2_T;
#define RD_FIFO_1_2ar (0xB814236C)
#define RD_FIFO_1_2dv (0x00000000)
#define RD_FIFO_1_2rv RVAL(RD_FIFO_1_2)
#define RMOD_RD_FIFO_1_2(...) RMOD(RD_FIFO_1_2, __VA_ARGS__)
#define RIZS_RD_FIFO_1_2(...) RIZS(RD_FIFO_1_2, __VA_ARGS__)
#define RFLD_RD_FIFO_1_2(fld) RFLD(RD_FIFO_1_2, fld)

typedef union {
	struct {
		unsigned int rd_adr_neg:32; //0x0
	} f;
	unsigned int v;
} RD_FIFO_1_3_T;
#define RD_FIFO_1_3ar (0xB8142370)
#define RD_FIFO_1_3dv (0x00000000)
#define RD_FIFO_1_3rv RVAL(RD_FIFO_1_3)
#define RMOD_RD_FIFO_1_3(...) RMOD(RD_FIFO_1_3, __VA_ARGS__)
#define RIZS_RD_FIFO_1_3(...) RIZS(RD_FIFO_1_3, __VA_ARGS__)
#define RFLD_RD_FIFO_1_3(fld) RFLD(RD_FIFO_1_3, fld)

typedef union {
	struct {
		unsigned int ocdp_set_7_pre:4; //0x7
		unsigned int ocdp_set_6_pre:4; //0x7
		unsigned int ocdp_set_5_pre:4; //0x7
		unsigned int ocdp_set_4_pre:4; //0x7
		unsigned int ocdp_set_3_pre:4; //0x7
		unsigned int ocdp_set_2_pre:4; //0x7
		unsigned int ocdp_set_1_pre:4; //0x7
		unsigned int ocdp_set_0_pre:4; //0x7
	} f;
	unsigned int v;
} OCDP_SET0_T;
#define OCDP_SET0ar (0xB8142374)
#define OCDP_SET0dv (0x77777777)
#define OCDP_SET0rv RVAL(OCDP_SET0)
#define RMOD_OCDP_SET0(...) RMOD(OCDP_SET0, __VA_ARGS__)
#define RIZS_OCDP_SET0(...) RIZS(OCDP_SET0, __VA_ARGS__)
#define RFLD_OCDP_SET0(fld) RFLD(OCDP_SET0, fld)

typedef union {
	struct {
		unsigned int ocdn_set_7_pre:4; //0x7
		unsigned int ocdn_set_6_pre:4; //0x7
		unsigned int ocdn_set_5_pre:4; //0x7
		unsigned int ocdn_set_4_pre:4; //0x7
		unsigned int ocdn_set_3_pre:4; //0x7
		unsigned int ocdn_set_2_pre:4; //0x7
		unsigned int ocdn_set_1_pre:4; //0x7
		unsigned int ocdn_set_0_pre:4; //0x7
	} f;
	unsigned int v;
} OCDN_SET0_T;
#define OCDN_SET0ar (0xB8142378)
#define OCDN_SET0dv (0x77777777)
#define OCDN_SET0rv RVAL(OCDN_SET0)
#define RMOD_OCDN_SET0(...) RMOD(OCDN_SET0, __VA_ARGS__)
#define RIZS_OCDN_SET0(...) RIZS(OCDN_SET0, __VA_ARGS__)
#define RFLD_OCDN_SET0(fld) RFLD(OCDN_SET0, fld)

typedef union {
	struct {
		unsigned int mbz_0:5; //0x0
		unsigned int dcc_dsp_dbg_sel:1; //0x0
		unsigned int mbz_1:1; //0x0
		unsigned int dpi_dbg_dll_sel:1; //0x0
		unsigned int dqsen_dbg_sel:2; //0x0
		unsigned int rw_dbg_sel1:2; //0x0
		unsigned int rd_dbg_sel1_3:5; //0x0
		unsigned int rd_dbg_sel1_2:5; //0x0
		unsigned int rd_dbg_sel1_1:5; //0x0
		unsigned int rd_dbg_sel1_0:5; //0x0
	} f;
	unsigned int v;
} TEST_CTRL1_T;
#define TEST_CTRL1ar (0xB814237C)
#define TEST_CTRL1dv (0x00000000)
#define TEST_CTRL1rv RVAL(TEST_CTRL1)
#define RMOD_TEST_CTRL1(...) RMOD(TEST_CTRL1, __VA_ARGS__)
#define RIZS_TEST_CTRL1(...) RIZS(TEST_CTRL1, __VA_ARGS__)
#define RFLD_TEST_CTRL1(fld) RFLD(TEST_CTRL1, fld)

typedef union {
	struct {
		unsigned int mbz_0:16; //0x0
		unsigned int dqs_oe_dly_sel_3:4; //0x0
		unsigned int dqs_oe_dly_sel_2:4; //0x0
		unsigned int dqs_oe_dly_sel_1:4; //0x0
		unsigned int dqs_oe_dly_sel_0:4; //0x0
	} f;
	unsigned int v;
} DQ_DLY_2_T;
#define DQ_DLY_2ar (0xB8142380)
#define DQ_DLY_2dv (0x00000000)
#define DQ_DLY_2rv RVAL(DQ_DLY_2)
#define RMOD_DQ_DLY_2(...) RMOD(DQ_DLY_2, __VA_ARGS__)
#define RIZS_DQ_DLY_2(...) RIZS(DQ_DLY_2, __VA_ARGS__)
#define RFLD_DQ_DLY_2(fld) RFLD(DQ_DLY_2, fld)

typedef union {
	struct {
		unsigned int mbz_0:3; //0x0
		unsigned int det_dqs_shift_sel_2:5; //0xA
		unsigned int det_post_shift_sel_2:4; //0x3
		unsigned int det_pre_shift_sel_2:4; //0x3
		unsigned int mbz_1:3; //0x0
		unsigned int det_dqs_shift_sel_0:5; //0xA
		unsigned int det_post_shift_sel_0:4; //0x3
		unsigned int det_pre_shift_sel_0:4; //0x3
	} f;
	unsigned int v;
} VALID_WIN_SEL_T;
#define VALID_WIN_SELar (0xB8142384)
#define VALID_WIN_SELdv (0x0A330A33)
#define VALID_WIN_SELrv RVAL(VALID_WIN_SEL)
#define RMOD_VALID_WIN_SEL(...) RMOD(VALID_WIN_SEL, __VA_ARGS__)
#define RIZS_VALID_WIN_SEL(...) RIZS(VALID_WIN_SEL, __VA_ARGS__)
#define RFLD_VALID_WIN_SEL(fld) RFLD(VALID_WIN_SEL, fld)

typedef union {
	struct {
		unsigned int mbz_0:26; //0x0
		unsigned int rzq_480code:5; //0xF
		unsigned int rzq_cal_done:1; //0x0
	} f;
	unsigned int v;
} PAD_RZCTRL_STATUS_T;
#define PAD_RZCTRL_STATUSar (0xB8142388)
#define PAD_RZCTRL_STATUSdv (0x0000001E)
#define PAD_RZCTRL_STATUSrv RVAL(PAD_RZCTRL_STATUS)
#define RMOD_PAD_RZCTRL_STATUS(...) RMOD(PAD_RZCTRL_STATUS, __VA_ARGS__)
#define RIZS_PAD_RZCTRL_STATUS(...) RIZS(PAD_RZCTRL_STATUS, __VA_ARGS__)
#define RFLD_PAD_RZCTRL_STATUS(fld) RFLD(PAD_RZCTRL_STATUS, fld)

typedef union {
	struct {
		unsigned int test4_pattern:32; //0x0
	} f;
	unsigned int v;
} PRBS7_TMUX9_T;
#define PRBS7_TMUX9ar (0xB814238C)
#define PRBS7_TMUX9dv (0x00000000)
#define PRBS7_TMUX9rv RVAL(PRBS7_TMUX9)
#define RMOD_PRBS7_TMUX9(...) RMOD(PRBS7_TMUX9, __VA_ARGS__)
#define RIZS_PRBS7_TMUX9(...) RIZS(PRBS7_TMUX9, __VA_ARGS__)
#define RFLD_PRBS7_TMUX9(fld) RFLD(PRBS7_TMUX9, fld)

typedef union {
	struct {
		unsigned int mbz_0:3; //0x0
		unsigned int det_dqs_shift_sel_ph_2:5; //0x0
		unsigned int det_post_shift_sel_ph_2:4; //0x0
		unsigned int det_pre_shift_sel_ph_2:4; //0x0
		unsigned int mbz_1:3; //0x0
		unsigned int det_dqs_shift_sel_ph_0:5; //0x0
		unsigned int det_post_shift_sel_ph_0:4; //0x0
		unsigned int det_pre_shift_sel_ph_0:4; //0x0
	} f;
	unsigned int v;
} VALID_WIN_PH_T;
#define VALID_WIN_PHar (0xB8142390)
#define VALID_WIN_PHdv (0x00000000)
#define VALID_WIN_PHrv RVAL(VALID_WIN_PH)
#define RMOD_VALID_WIN_PH(...) RMOD(VALID_WIN_PH, __VA_ARGS__)
#define RIZS_VALID_WIN_PH(...) RIZS(VALID_WIN_PH, __VA_ARGS__)
#define RFLD_VALID_WIN_PH(fld) RFLD(VALID_WIN_PH, fld)

typedef union {
	struct {
		unsigned int mbz_0:27; //0x0
		unsigned int null_dqs_mode:1; //0x0
		unsigned int null_dqs_work_3:1; //0x0
		unsigned int null_dqs_work_2:1; //0x0
		unsigned int null_dqs_work_1:1; //0x0
		unsigned int null_dqs_work_0:1; //0x0
	} f;
	unsigned int v;
} NULL_DQS_T;
#define NULL_DQSar (0xB8142394)
#define NULL_DQSdv (0x00000000)
#define NULL_DQSrv RVAL(NULL_DQS)
#define RMOD_NULL_DQS(...) RMOD(NULL_DQS, __VA_ARGS__)
#define RIZS_NULL_DQS(...) RIZS(NULL_DQS, __VA_ARGS__)
#define RFLD_NULL_DQS(fld) RFLD(NULL_DQS, fld)

typedef union {
	struct {
		unsigned int bist_debug:32; //0x0
	} f;
	unsigned int v;
} BIST_DEBUG_T;
#define BIST_DEBUGar (0xB8142398)
#define BIST_DEBUGdv (0x00000000)
#define BIST_DEBUGrv RVAL(BIST_DEBUG)
#define RMOD_BIST_DEBUG(...) RMOD(BIST_DEBUG, __VA_ARGS__)
#define RIZS_BIST_DEBUG(...) RIZS(BIST_DEBUG, __VA_ARGS__)
#define RFLD_BIST_DEBUG(fld) RFLD(BIST_DEBUG, fld)

typedef union {
	struct {
		unsigned int mbz_0:24; //0x0
		unsigned int rx_lag_det_3:1; //0x0
		unsigned int rx_lead_det_3:1; //0x0
		unsigned int rx_lag_det_1:1; //0x0
		unsigned int rx_lead_det_1:1; //0x0
		unsigned int rx_lag_det_2:1; //0x0
		unsigned int rx_lead_det_2:1; //0x0
		unsigned int rx_lag_det_0:1; //0x0
		unsigned int rx_lead_det_0:1; //0x0
	} f;
	unsigned int v;
} VALID_WIN_DET_T;
#define VALID_WIN_DETar (0xB814239C)
#define VALID_WIN_DETdv (0x00000000)
#define VALID_WIN_DETrv RVAL(VALID_WIN_DET)
#define RMOD_VALID_WIN_DET(...) RMOD(VALID_WIN_DET, __VA_ARGS__)
#define RIZS_VALID_WIN_DET(...) RIZS(VALID_WIN_DET, __VA_ARGS__)
#define RFLD_VALID_WIN_DET(fld) RFLD(VALID_WIN_DET, fld)

typedef union {
	struct {
		unsigned int mbz_0:8; //0x0
		unsigned int bist_en:1; //0x0
		unsigned int bist_mode:3; //0x0
		unsigned int mbz_1:15; //0x0
		unsigned int bist_debug_sel:5; //0x0
	} f;
	unsigned int v;
} BIST_2TO1_0_T;
#define BIST_2TO1_0ar (0xB81423A0)
#define BIST_2TO1_0dv (0x00000000)
#define BIST_2TO1_0rv RVAL(BIST_2TO1_0)
#define RMOD_BIST_2TO1_0(...) RMOD(BIST_2TO1_0, __VA_ARGS__)
#define RIZS_BIST_2TO1_0(...) RIZS(BIST_2TO1_0, __VA_ARGS__)
#define RFLD_BIST_2TO1_0(fld) RFLD(BIST_2TO1_0, fld)

typedef union {
	struct {
		unsigned int mbz_0:26; //0x0
		unsigned int bist_rdy:1; //0x0
		unsigned int bist_fail_cmd:1; //0x0
		unsigned int bist_fail_data:4; //0x0
	} f;
	unsigned int v;
} BIST_2TO1_1_T;
#define BIST_2TO1_1ar (0xB81423A4)
#define BIST_2TO1_1dv (0x00000000)
#define BIST_2TO1_1rv RVAL(BIST_2TO1_1)
#define RMOD_BIST_2TO1_1(...) RMOD(BIST_2TO1_1, __VA_ARGS__)
#define RIZS_BIST_2TO1_1(...) RIZS(BIST_2TO1_1, __VA_ARGS__)
#define RFLD_BIST_2TO1_1(fld) RFLD(BIST_2TO1_1, fld)

typedef union {
	struct {
		unsigned int bist_pattern:32; //0x0
	} f;
	unsigned int v;
} BIST_PT_T;
#define BIST_PTar (0xB81423A8)
#define BIST_PTdv (0x00000000)
#define BIST_PTrv RVAL(BIST_PT)
#define RMOD_BIST_PT(...) RMOD(BIST_PT, __VA_ARGS__)
#define RIZS_BIST_PT(...) RIZS(BIST_PT, __VA_ARGS__)
#define RFLD_BIST_PT(fld) RFLD(BIST_PT, fld)

typedef union {
	struct {
		unsigned int write_en_15:1; //0x0
		unsigned int int_vref_dbg_en:1; //0x0
		unsigned int write_en_14:1; //0x0
		unsigned int fw_clr_vref_scnt:1; //0x0
		unsigned int write_en_13:1; //0x0
		unsigned int int_tm_dqsen_en:1; //0x0
		unsigned int write_en_12:1; //0x0
		unsigned int int_fifo_ptr_en:1; //0x0
		unsigned int write_en_11:1; //0x0
		unsigned int int_zq_en:1; //0x0
		unsigned int mbz_0:6; //0x0
		unsigned int write_en_7:1; //0x0
		unsigned int int_pll_en:1; //0x0
		unsigned int write_en_6:1; //0x0
		unsigned int int_cpu_3_en:1; //0x0
		unsigned int write_en_5:1; //0x0
		unsigned int int_cpu_2_en:1; //0x0
		unsigned int write_en_4:1; //0x0
		unsigned int int_cpu_1_en:1; //0x0
		unsigned int write_en_3:1; //0x0
		unsigned int int_cpu_0_en:1; //0x0
		unsigned int write_en_2:1; //0x0
		unsigned int fw_clr_dqs_int:1; //0x0
		unsigned int write_en_1:1; //0x0
		unsigned int int_cal_bdy_en:1; //0x0
		unsigned int write_en_0:1; //0x0
		unsigned int int_dqs_en:1; //0x0
	} f;
	unsigned int v;
} INT_CTRL_T;
#define INT_CTRLar (0xB81423AC)
#define INT_CTRLdv (0x00000000)
#define INT_CTRLrv RVAL(INT_CTRL)
#define RMOD_INT_CTRL(...) RMOD(INT_CTRL, __VA_ARGS__)
#define RIZS_INT_CTRL(...) RIZS(INT_CTRL, __VA_ARGS__)
#define RFLD_INT_CTRL(fld) RFLD(INT_CTRL, fld)

typedef union {
	struct {
		unsigned int mbz_0:20; //0x0
		unsigned int tm_dqsen_early_int_3:1; //0x0
		unsigned int tm_dqsen_early_int_2:1; //0x0
		unsigned int tm_dqsen_early_int_1:1; //0x0
		unsigned int tm_dqsen_early_int_0:1; //0x0
		unsigned int tm_dqsen_late_int_3:1; //0x0
		unsigned int tm_dqsen_late_int_2:1; //0x0
		unsigned int tm_dqsen_late_int_1:1; //0x0
		unsigned int tm_dqsen_late_int_0:1; //0x0
		unsigned int dqs_int_3:1; //0x0
		unsigned int dqs_int_2:1; //0x0
		unsigned int dqs_int_1:1; //0x0
		unsigned int dqs_int_0:1; //0x0
	} f;
	unsigned int v;
} INT_STATUS_0_T;
#define INT_STATUS_0ar (0xB81423B0)
#define INT_STATUS_0dv (0x00000000)
#define INT_STATUS_0rv RVAL(INT_STATUS_0)
#define RMOD_INT_STATUS_0(...) RMOD(INT_STATUS_0, __VA_ARGS__)
#define RIZS_INT_STATUS_0(...) RIZS(INT_STATUS_0, __VA_ARGS__)
#define RFLD_INT_STATUS_0(fld) RFLD(INT_STATUS_0, fld)

typedef union {
	struct {
		unsigned int cal_rbdy_int_neg:8; //0x0
		unsigned int cal_lbdy_int_neg:8; //0x0
		unsigned int cal_rbdy_int_pos:8; //0x0
		unsigned int cal_lbdy_int_pos:8; //0x0
	} f;
	unsigned int v;
} INT_STATUS_1_0_T;
#define INT_STATUS_1_0ar (0xB81423B4)
#define INT_STATUS_1_0dv (0x00000000)
#define INT_STATUS_1_0rv RVAL(INT_STATUS_1_0)
#define RMOD_INT_STATUS_1_0(...) RMOD(INT_STATUS_1_0, __VA_ARGS__)
#define RIZS_INT_STATUS_1_0(...) RIZS(INT_STATUS_1_0, __VA_ARGS__)
#define RFLD_INT_STATUS_1_0(fld) RFLD(INT_STATUS_1_0, fld)

typedef union {
	struct {
		unsigned int cal_rbdy_int_neg:8; //0x0
		unsigned int cal_lbdy_int_neg:8; //0x0
		unsigned int cal_rbdy_int_pos:8; //0x0
		unsigned int cal_lbdy_int_pos:8; //0x0
	} f;
	unsigned int v;
} INT_STATUS_1_1_T;
#define INT_STATUS_1_1ar (0xB81423B8)
#define INT_STATUS_1_1dv (0x00000000)
#define INT_STATUS_1_1rv RVAL(INT_STATUS_1_1)
#define RMOD_INT_STATUS_1_1(...) RMOD(INT_STATUS_1_1, __VA_ARGS__)
#define RIZS_INT_STATUS_1_1(...) RIZS(INT_STATUS_1_1, __VA_ARGS__)
#define RFLD_INT_STATUS_1_1(fld) RFLD(INT_STATUS_1_1, fld)

typedef union {
	struct {
		unsigned int cal_rbdy_int_neg:8; //0x0
		unsigned int cal_lbdy_int_neg:8; //0x0
		unsigned int cal_rbdy_int_pos:8; //0x0
		unsigned int cal_lbdy_int_pos:8; //0x0
	} f;
	unsigned int v;
} INT_STATUS_1_2_T;
#define INT_STATUS_1_2ar (0xB81423BC)
#define INT_STATUS_1_2dv (0x00000000)
#define INT_STATUS_1_2rv RVAL(INT_STATUS_1_2)
#define RMOD_INT_STATUS_1_2(...) RMOD(INT_STATUS_1_2, __VA_ARGS__)
#define RIZS_INT_STATUS_1_2(...) RIZS(INT_STATUS_1_2, __VA_ARGS__)
#define RFLD_INT_STATUS_1_2(fld) RFLD(INT_STATUS_1_2, fld)

typedef union {
	struct {
		unsigned int cal_rbdy_int_neg:8; //0x0
		unsigned int cal_lbdy_int_neg:8; //0x0
		unsigned int cal_rbdy_int_pos:8; //0x0
		unsigned int cal_lbdy_int_pos:8; //0x0
	} f;
	unsigned int v;
} INT_STATUS_1_3_T;
#define INT_STATUS_1_3ar (0xB81423C0)
#define INT_STATUS_1_3dv (0x00000000)
#define INT_STATUS_1_3rv RVAL(INT_STATUS_1_3)
#define RMOD_INT_STATUS_1_3(...) RMOD(INT_STATUS_1_3, __VA_ARGS__)
#define RIZS_INT_STATUS_1_3(...) RIZS(INT_STATUS_1_3, __VA_ARGS__)
#define RFLD_INT_STATUS_1_3(fld) RFLD(INT_STATUS_1_3, fld)

typedef union {
	struct {
		unsigned int mbz_0:21; //0x0
		unsigned int rzq_done:1; //0x0
		unsigned int zq_done:1; //0x0
		unsigned int mbz_1:8; //0x0
		unsigned int pll_stable:1; //0x0
	} f;
	unsigned int v;
} INT_STATUS_2_T;
#define INT_STATUS_2ar (0xB81423C4)
#define INT_STATUS_2dv (0x00000000)
#define INT_STATUS_2rv RVAL(INT_STATUS_2)
#define RMOD_INT_STATUS_2(...) RMOD(INT_STATUS_2, __VA_ARGS__)
#define RIZS_INT_STATUS_2(...) RIZS(INT_STATUS_2, __VA_ARGS__)
#define RFLD_INT_STATUS_2(fld) RFLD(INT_STATUS_2, fld)

typedef union {
	struct {
		unsigned int mbz_0:8; //0x0
		unsigned int ssc_ncode_debug:8; //0x0
		unsigned int mbz_1:2; //0x0
		unsigned int ssc_fmod:1; //0x0
		unsigned int ssc_oc_done:1; //0x0
		unsigned int mbz_2:1; //0x0
		unsigned int ssc_fcode_debug:11; //0x0
	} f;
	unsigned int v;
} SSC_STATUS_T;
#define SSC_STATUSar (0xB81423C8)
#define SSC_STATUSdv (0x00000000)
#define SSC_STATUSrv RVAL(SSC_STATUS)
#define RMOD_SSC_STATUS(...) RMOD(SSC_STATUS, __VA_ARGS__)
#define RIZS_SSC_STATUS(...) RIZS(SSC_STATUS, __VA_ARGS__)
#define RFLD_SSC_STATUS(fld) RFLD(SSC_STATUS, fld)

typedef union {
	struct {
		unsigned int mbz_0:32; //0x0
	} f;
	unsigned int v;
} DUMMY_0_T;
#define DUMMY_0ar (0xB81423CC)
#define DUMMY_0dv (0x00000000)
#define DUMMY_0rv RVAL(DUMMY_0)
#define RMOD_DUMMY_0(...) RMOD(DUMMY_0, __VA_ARGS__)
#define RIZS_DUMMY_0(...) RIZS(DUMMY_0, __VA_ARGS__)
#define RFLD_DUMMY_0(fld) RFLD(DUMMY_0, fld)

typedef union {
	struct {
		unsigned int mbz_0:32; //0x0
	} f;
	unsigned int v;
} DUMMY_1_T;
#define DUMMY_1ar (0xB81423D0)
#define DUMMY_1dv (0x00000000)
#define DUMMY_1rv RVAL(DUMMY_1)
#define RMOD_DUMMY_1(...) RMOD(DUMMY_1, __VA_ARGS__)
#define RIZS_DUMMY_1(...) RIZS(DUMMY_1, __VA_ARGS__)
#define RFLD_DUMMY_1(fld) RFLD(DUMMY_1, fld)

typedef union {
	struct {
		unsigned int mbz_0:32; //0x0
	} f;
	unsigned int v;
} DUMMY_2_T;
#define DUMMY_2ar (0xB81423D4)
#define DUMMY_2dv (0x00000000)
#define DUMMY_2rv RVAL(DUMMY_2)
#define RMOD_DUMMY_2(...) RMOD(DUMMY_2, __VA_ARGS__)
#define RIZS_DUMMY_2(...) RIZS(DUMMY_2, __VA_ARGS__)
#define RFLD_DUMMY_2(fld) RFLD(DUMMY_2, fld)

typedef union {
	struct {
		unsigned int mbz_0:32; //0x0
	} f;
	unsigned int v;
} DUMMY_3_T;
#define DUMMY_3ar (0xB81423D8)
#define DUMMY_3dv (0x00000000)
#define DUMMY_3rv RVAL(DUMMY_3)
#define RMOD_DUMMY_3(...) RMOD(DUMMY_3, __VA_ARGS__)
#define RIZS_DUMMY_3(...) RIZS(DUMMY_3, __VA_ARGS__)
#define RFLD_DUMMY_3(fld) RFLD(DUMMY_3, fld)

typedef union {
	struct {
		unsigned int mbz_0:16; //0x0
		unsigned int mbz_1:1; //0x0
		unsigned int fw_tm_dqs_en_ftun_3:3; //0x3
		unsigned int mbz_2:1; //0x0
		unsigned int fw_tm_dqs_en_ftun_2:3; //0x3
		unsigned int mbz_3:1; //0x0
		unsigned int fw_tm_dqs_en_ftun_1:3; //0x3
		unsigned int mbz_4:1; //0x0
		unsigned int fw_tm_dqs_en_ftun_0:3; //0x3
	} f;
	unsigned int v;
} TM_DQSEN_CAL_STAT_T;
#define TM_DQSEN_CAL_STATar (0xB81423DC)
#define TM_DQSEN_CAL_STATdv (0x00003333)
#define TM_DQSEN_CAL_STATrv RVAL(TM_DQSEN_CAL_STAT)
#define RMOD_TM_DQSEN_CAL_STAT(...) RMOD(TM_DQSEN_CAL_STAT, __VA_ARGS__)
#define RIZS_TM_DQSEN_CAL_STAT(...) RIZS(TM_DQSEN_CAL_STAT, __VA_ARGS__)
#define RFLD_TM_DQSEN_CAL_STAT(fld) RFLD(TM_DQSEN_CAL_STAT, fld)

typedef union {
	struct {
		unsigned int mbz_0:1; //0x0
		unsigned int mbz_1:1; //0x0
		unsigned int dqs_en_dly_sel_ph_3:6; //0x10
		unsigned int mbz_2:1; //0x0
		unsigned int mbz_3:1; //0x0
		unsigned int dqs_en_dly_sel_ph_2:6; //0x10
		unsigned int mbz_4:1; //0x0
		unsigned int mbz_5:1; //0x0
		unsigned int dqs_en_dly_sel_ph_1:6; //0x10
		unsigned int mbz_6:1; //0x0
		unsigned int mbz_7:1; //0x0
		unsigned int dqs_en_dly_sel_ph_0:6; //0x10
	} f;
	unsigned int v;
} DQSEN_CAL_STATUS_T;
#define DQSEN_CAL_STATUSar (0xB81423E0)
#define DQSEN_CAL_STATUSdv (0x10101010)
#define DQSEN_CAL_STATUSrv RVAL(DQSEN_CAL_STATUS)
#define RMOD_DQSEN_CAL_STATUS(...) RMOD(DQSEN_CAL_STATUS, __VA_ARGS__)
#define RIZS_DQSEN_CAL_STATUS(...) RIZS(DQSEN_CAL_STATUS, __VA_ARGS__)
#define RFLD_DQSEN_CAL_STATUS(fld) RFLD(DQSEN_CAL_STATUS, fld)

typedef union {
	struct {
		unsigned int mbz_0:4; //0x0
		unsigned int vref_sr1_int_det:1; //0x0
		unsigned int vref_sl0_int_det:1; //0x0
		unsigned int vref_sr1_dbi_int_neg:1; //0x0
		unsigned int vref_sr1_dbi_int_pos:1; //0x0
		unsigned int vref_sl0_dbi_int_neg:1; //0x0
		unsigned int vref_sl0_dbi_int_pos:1; //0x0
		unsigned int cal_rbdy_int_neg:1; //0x0
		unsigned int cal_lbdy_int_neg:1; //0x0
		unsigned int cal_rbdy_int_pos:1; //0x0
		unsigned int cal_lbdy_int_pos:1; //0x0
		unsigned int fifo_ptr_int_neg:9; //0x0
		unsigned int fifo_ptr_int_pos:9; //0x0
	} f;
	unsigned int v;
} INT_STATUS_3_0_T;
#define INT_STATUS_3_0ar (0xB81423E4)
#define INT_STATUS_3_0dv (0x00000000)
#define INT_STATUS_3_0rv RVAL(INT_STATUS_3_0)
#define RMOD_INT_STATUS_3_0(...) RMOD(INT_STATUS_3_0, __VA_ARGS__)
#define RIZS_INT_STATUS_3_0(...) RIZS(INT_STATUS_3_0, __VA_ARGS__)
#define RFLD_INT_STATUS_3_0(fld) RFLD(INT_STATUS_3_0, fld)

typedef union {
	struct {
		unsigned int mbz_0:4; //0x0
		unsigned int vref_sr1_int_det:1; //0x0
		unsigned int vref_sl0_int_det:1; //0x0
		unsigned int vref_sr1_dbi_int_neg:1; //0x0
		unsigned int vref_sr1_dbi_int_pos:1; //0x0
		unsigned int vref_sl0_dbi_int_neg:1; //0x0
		unsigned int vref_sl0_dbi_int_pos:1; //0x0
		unsigned int cal_rbdy_int_neg:1; //0x0
		unsigned int cal_lbdy_int_neg:1; //0x0
		unsigned int cal_rbdy_int_pos:1; //0x0
		unsigned int cal_lbdy_int_pos:1; //0x0
		unsigned int fifo_ptr_int_neg:9; //0x0
		unsigned int fifo_ptr_int_pos:9; //0x0
	} f;
	unsigned int v;
} INT_STATUS_3_1_T;
#define INT_STATUS_3_1ar (0xB81423E8)
#define INT_STATUS_3_1dv (0x00000000)
#define INT_STATUS_3_1rv RVAL(INT_STATUS_3_1)
#define RMOD_INT_STATUS_3_1(...) RMOD(INT_STATUS_3_1, __VA_ARGS__)
#define RIZS_INT_STATUS_3_1(...) RIZS(INT_STATUS_3_1, __VA_ARGS__)
#define RFLD_INT_STATUS_3_1(fld) RFLD(INT_STATUS_3_1, fld)

typedef union {
	struct {
		unsigned int mbz_0:4; //0x0
		unsigned int vref_sr1_int_det:1; //0x0
		unsigned int vref_sl0_int_det:1; //0x0
		unsigned int vref_sr1_dbi_int_neg:1; //0x0
		unsigned int vref_sr1_dbi_int_pos:1; //0x0
		unsigned int vref_sl0_dbi_int_neg:1; //0x0
		unsigned int vref_sl0_dbi_int_pos:1; //0x0
		unsigned int cal_rbdy_int_neg:1; //0x0
		unsigned int cal_lbdy_int_neg:1; //0x0
		unsigned int cal_rbdy_int_pos:1; //0x0
		unsigned int cal_lbdy_int_pos:1; //0x0
		unsigned int fifo_ptr_int_neg:9; //0x0
		unsigned int fifo_ptr_int_pos:9; //0x0
	} f;
	unsigned int v;
} INT_STATUS_3_2_T;
#define INT_STATUS_3_2ar (0xB81423EC)
#define INT_STATUS_3_2dv (0x00000000)
#define INT_STATUS_3_2rv RVAL(INT_STATUS_3_2)
#define RMOD_INT_STATUS_3_2(...) RMOD(INT_STATUS_3_2, __VA_ARGS__)
#define RIZS_INT_STATUS_3_2(...) RIZS(INT_STATUS_3_2, __VA_ARGS__)
#define RFLD_INT_STATUS_3_2(fld) RFLD(INT_STATUS_3_2, fld)

typedef union {
	struct {
		unsigned int mbz_0:4; //0x0
		unsigned int vref_sr1_int_det:1; //0x0
		unsigned int vref_sl0_int_det:1; //0x0
		unsigned int vref_sr1_dbi_int_neg:1; //0x0
		unsigned int vref_sr1_dbi_int_pos:1; //0x0
		unsigned int vref_sl0_dbi_int_neg:1; //0x0
		unsigned int vref_sl0_dbi_int_pos:1; //0x0
		unsigned int cal_rbdy_int_neg:1; //0x0
		unsigned int cal_lbdy_int_neg:1; //0x0
		unsigned int cal_rbdy_int_pos:1; //0x0
		unsigned int cal_lbdy_int_pos:1; //0x0
		unsigned int fifo_ptr_int_neg:9; //0x0
		unsigned int fifo_ptr_int_pos:9; //0x0
	} f;
	unsigned int v;
} INT_STATUS_3_3_T;
#define INT_STATUS_3_3ar (0xB81423F0)
#define INT_STATUS_3_3dv (0x00000000)
#define INT_STATUS_3_3rv RVAL(INT_STATUS_3_3)
#define RMOD_INT_STATUS_3_3(...) RMOD(INT_STATUS_3_3, __VA_ARGS__)
#define RIZS_INT_STATUS_3_3(...) RIZS(INT_STATUS_3_3, __VA_ARGS__)
#define RFLD_INT_STATUS_3_3(fld) RFLD(INT_STATUS_3_3, fld)

typedef union {
	struct {
		unsigned int mbz_0:24; //0x0
		unsigned int det_enable:1; //0x0
		unsigned int det_done:1; //0x0
		unsigned int det_result:6; //0x0
	} f;
	unsigned int v;
} HALF_CYC_DET_T;
#define HALF_CYC_DETar (0xB81423F4)
#define HALF_CYC_DETdv (0x00000000)
#define HALF_CYC_DETrv RVAL(HALF_CYC_DET)
#define RMOD_HALF_CYC_DET(...) RMOD(HALF_CYC_DET, __VA_ARGS__)
#define RIZS_HALF_CYC_DET(...) RIZS(HALF_CYC_DET, __VA_ARGS__)
#define RFLD_HALF_CYC_DET(fld) RFLD(HALF_CYC_DET, fld)

typedef union {
	struct {
		unsigned int mbz_0:3; //0x0
		unsigned int odt_ttcp_set_7_pre:5; //0x0
		unsigned int mbz_1:3; //0x0
		unsigned int odt_ttcp_set_6_pre:5; //0x16
		unsigned int mbz_2:3; //0x0
		unsigned int odt_ttcp_set_5_pre:5; //0x16
		unsigned int mbz_3:3; //0x0
		unsigned int odt_ttcp_set_4_pre:5; //0x16
	} f;
	unsigned int v;
} ODT_TTCP1_SET0_T;
#define ODT_TTCP1_SET0ar (0xB81423F8)
#define ODT_TTCP1_SET0dv (0x00161616)
#define ODT_TTCP1_SET0rv RVAL(ODT_TTCP1_SET0)
#define RMOD_ODT_TTCP1_SET0(...) RMOD(ODT_TTCP1_SET0, __VA_ARGS__)
#define RIZS_ODT_TTCP1_SET0(...) RIZS(ODT_TTCP1_SET0, __VA_ARGS__)
#define RFLD_ODT_TTCP1_SET0(fld) RFLD(ODT_TTCP1_SET0, fld)

typedef union {
	struct {
		unsigned int prbs7_ten:1; //0x0
		unsigned int odt_sel:3; //0x0
		unsigned int prbs7_dual_en:1; //0x0
		unsigned int cas_n_sel:3; //0x0
		unsigned int cke_test:1; //0x0
		unsigned int ras_n_sel:3; //0x0
		unsigned int prbs7_2t_en:1; //0x0
		unsigned int we_n_sel:3; //0x0
		unsigned int mbz_0:16; //0x0
	} f;
	unsigned int v;
} PRBS7_TMUX0_T;
#define PRBS7_TMUX0ar (0xB81423FC)
#define PRBS7_TMUX0dv (0x00000000)
#define PRBS7_TMUX0rv RVAL(PRBS7_TMUX0)
#define RMOD_PRBS7_TMUX0(...) RMOD(PRBS7_TMUX0, __VA_ARGS__)
#define RIZS_PRBS7_TMUX0(...) RIZS(PRBS7_TMUX0, __VA_ARGS__)
#define RFLD_PRBS7_TMUX0(fld) RFLD(PRBS7_TMUX0, fld)

typedef union {
	struct {
		unsigned int mbz_0:1; //0x0
		unsigned int adr07_sel:3; //0x0
		unsigned int mbz_1:1; //0x0
		unsigned int adr06_sel:3; //0x0
		unsigned int mbz_2:1; //0x0
		unsigned int adr05_sel:3; //0x0
		unsigned int mbz_3:1; //0x0
		unsigned int adr04_sel:3; //0x0
		unsigned int mbz_4:1; //0x0
		unsigned int adr03_sel:3; //0x0
		unsigned int mbz_5:1; //0x0
		unsigned int adr02_sel:3; //0x0
		unsigned int mbz_6:1; //0x0
		unsigned int adr01_sel:3; //0x0
		unsigned int mbz_7:1; //0x0
		unsigned int adr00_sel:3; //0x0
	} f;
	unsigned int v;
} PRBS7_TMUX1_T;
#define PRBS7_TMUX1ar (0xB8142400)
#define PRBS7_TMUX1dv (0x00000000)
#define PRBS7_TMUX1rv RVAL(PRBS7_TMUX1)
#define RMOD_PRBS7_TMUX1(...) RMOD(PRBS7_TMUX1, __VA_ARGS__)
#define RIZS_PRBS7_TMUX1(...) RIZS(PRBS7_TMUX1, __VA_ARGS__)
#define RFLD_PRBS7_TMUX1(fld) RFLD(PRBS7_TMUX1, fld)

typedef union {
	struct {
		unsigned int mbz_0:1; //0x0
		unsigned int adr15_sel:3; //0x0
		unsigned int mbz_1:1; //0x0
		unsigned int adr14_sel:3; //0x0
		unsigned int mbz_2:1; //0x0
		unsigned int adr13_sel:3; //0x0
		unsigned int mbz_3:1; //0x0
		unsigned int adr12_sel:3; //0x0
		unsigned int mbz_4:1; //0x0
		unsigned int adr11_sel:3; //0x0
		unsigned int mbz_5:1; //0x0
		unsigned int adr10_sel:3; //0x0
		unsigned int mbz_6:1; //0x0
		unsigned int adr09_sel:3; //0x0
		unsigned int mbz_7:1; //0x0
		unsigned int adr08_sel:3; //0x0
	} f;
	unsigned int v;
} PRBS7_TMUX2_T;
#define PRBS7_TMUX2ar (0xB8142404)
#define PRBS7_TMUX2dv (0x00000000)
#define PRBS7_TMUX2rv RVAL(PRBS7_TMUX2)
#define RMOD_PRBS7_TMUX2(...) RMOD(PRBS7_TMUX2, __VA_ARGS__)
#define RIZS_PRBS7_TMUX2(...) RIZS(PRBS7_TMUX2, __VA_ARGS__)
#define RFLD_PRBS7_TMUX2(fld) RFLD(PRBS7_TMUX2, fld)

typedef union {
	struct {
		unsigned int mbz_0:1; //0x0
		unsigned int ba02_sel:3; //0x0
		unsigned int mbz_1:1; //0x0
		unsigned int ba01_sel:3; //0x0
		unsigned int mbz_2:1; //0x0
		unsigned int ba00_sel:3; //0x0
		unsigned int mbz_3:1; //0x0
		unsigned int adr05_ext_sel:3; //0x0
		unsigned int mbz_4:1; //0x0
		unsigned int adr04_ext_sel:3; //0x0
		unsigned int mbz_5:5; //0x0
		unsigned int cs_n_sel:3; //0x0
		unsigned int mbz_6:1; //0x0
		unsigned int cs_n_1_sel:3; //0x0
	} f;
	unsigned int v;
} PRBS7_TMUX3_T;
#define PRBS7_TMUX3ar (0xB8142408)
#define PRBS7_TMUX3dv (0x00000000)
#define PRBS7_TMUX3rv RVAL(PRBS7_TMUX3)
#define RMOD_PRBS7_TMUX3(...) RMOD(PRBS7_TMUX3, __VA_ARGS__)
#define RIZS_PRBS7_TMUX3(...) RIZS(PRBS7_TMUX3, __VA_ARGS__)
#define RFLD_PRBS7_TMUX3(fld) RFLD(PRBS7_TMUX3, fld)

typedef union {
	struct {
		unsigned int dqs_test:1; //0x0
		unsigned int dq5_sel:3; //0x0
		unsigned int mbz_0:1; //0x0
		unsigned int dq4_sel:3; //0x0
		unsigned int mbz_1:1; //0x0
		unsigned int dq3_sel:3; //0x0
		unsigned int mbz_2:1; //0x0
		unsigned int dq2_sel:3; //0x0
		unsigned int mbz_3:1; //0x0
		unsigned int dq1_sel:3; //0x0
		unsigned int mbz_4:1; //0x0
		unsigned int dq0_sel:3; //0x0
		unsigned int data_prbs7_invs:1; //0x0
		unsigned int data_prbs7_seed:7; //0x1
	} f;
	unsigned int v;
} PRBS7_TMUX4_0_T;
#define PRBS7_TMUX4_0ar (0xB814240C)
#define PRBS7_TMUX4_0dv (0x00000001)
#define PRBS7_TMUX4_0rv RVAL(PRBS7_TMUX4_0)
#define RMOD_PRBS7_TMUX4_0(...) RMOD(PRBS7_TMUX4_0, __VA_ARGS__)
#define RIZS_PRBS7_TMUX4_0(...) RIZS(PRBS7_TMUX4_0, __VA_ARGS__)
#define RFLD_PRBS7_TMUX4_0(fld) RFLD(PRBS7_TMUX4_0, fld)

typedef union {
	struct {
		unsigned int dqs_test:1; //0x0
		unsigned int dq5_sel:3; //0x0
		unsigned int mbz_0:1; //0x0
		unsigned int dq4_sel:3; //0x0
		unsigned int mbz_1:1; //0x0
		unsigned int dq3_sel:3; //0x0
		unsigned int mbz_2:1; //0x0
		unsigned int dq2_sel:3; //0x0
		unsigned int mbz_3:1; //0x0
		unsigned int dq1_sel:3; //0x0
		unsigned int mbz_4:1; //0x0
		unsigned int dq0_sel:3; //0x0
		unsigned int data_prbs7_invs:1; //0x0
		unsigned int data_prbs7_seed:7; //0x1
	} f;
	unsigned int v;
} PRBS7_TMUX4_1_T;
#define PRBS7_TMUX4_1ar (0xB8142410)
#define PRBS7_TMUX4_1dv (0x00000001)
#define PRBS7_TMUX4_1rv RVAL(PRBS7_TMUX4_1)
#define RMOD_PRBS7_TMUX4_1(...) RMOD(PRBS7_TMUX4_1, __VA_ARGS__)
#define RIZS_PRBS7_TMUX4_1(...) RIZS(PRBS7_TMUX4_1, __VA_ARGS__)
#define RFLD_PRBS7_TMUX4_1(fld) RFLD(PRBS7_TMUX4_1, fld)

typedef union {
	struct {
		unsigned int dqs_test:1; //0x0
		unsigned int dq5_sel:3; //0x0
		unsigned int mbz_0:1; //0x0
		unsigned int dq4_sel:3; //0x0
		unsigned int mbz_1:1; //0x0
		unsigned int dq3_sel:3; //0x0
		unsigned int mbz_2:1; //0x0
		unsigned int dq2_sel:3; //0x0
		unsigned int mbz_3:1; //0x0
		unsigned int dq1_sel:3; //0x0
		unsigned int mbz_4:1; //0x0
		unsigned int dq0_sel:3; //0x0
		unsigned int data_prbs7_invs:1; //0x0
		unsigned int data_prbs7_seed:7; //0x1
	} f;
	unsigned int v;
} PRBS7_TMUX4_2_T;
#define PRBS7_TMUX4_2ar (0xB8142414)
#define PRBS7_TMUX4_2dv (0x00000001)
#define PRBS7_TMUX4_2rv RVAL(PRBS7_TMUX4_2)
#define RMOD_PRBS7_TMUX4_2(...) RMOD(PRBS7_TMUX4_2, __VA_ARGS__)
#define RIZS_PRBS7_TMUX4_2(...) RIZS(PRBS7_TMUX4_2, __VA_ARGS__)
#define RFLD_PRBS7_TMUX4_2(fld) RFLD(PRBS7_TMUX4_2, fld)

typedef union {
	struct {
		unsigned int dqs_test:1; //0x0
		unsigned int dq5_sel:3; //0x0
		unsigned int mbz_0:1; //0x0
		unsigned int dq4_sel:3; //0x0
		unsigned int mbz_1:1; //0x0
		unsigned int dq3_sel:3; //0x0
		unsigned int mbz_2:1; //0x0
		unsigned int dq2_sel:3; //0x0
		unsigned int mbz_3:1; //0x0
		unsigned int dq1_sel:3; //0x0
		unsigned int mbz_4:1; //0x0
		unsigned int dq0_sel:3; //0x0
		unsigned int data_prbs7_invs:1; //0x0
		unsigned int data_prbs7_seed:7; //0x1
	} f;
	unsigned int v;
} PRBS7_TMUX4_3_T;
#define PRBS7_TMUX4_3ar (0xB8142418)
#define PRBS7_TMUX4_3dv (0x00000001)
#define PRBS7_TMUX4_3rv RVAL(PRBS7_TMUX4_3)
#define RMOD_PRBS7_TMUX4_3(...) RMOD(PRBS7_TMUX4_3, __VA_ARGS__)
#define RIZS_PRBS7_TMUX4_3(...) RIZS(PRBS7_TMUX4_3, __VA_ARGS__)
#define RFLD_PRBS7_TMUX4_3(fld) RFLD(PRBS7_TMUX4_3, fld)

typedef union {
	struct {
		unsigned int mbz_0:21; //0x0
		unsigned int dm_sel:3; //0x0
		unsigned int mbz_1:1; //0x0
		unsigned int dq7_sel:3; //0x0
		unsigned int mbz_2:1; //0x0
		unsigned int dq6_sel:3; //0x0
	} f;
	unsigned int v;
} PRBS7_TMUX5_0_T;
#define PRBS7_TMUX5_0ar (0xB814241C)
#define PRBS7_TMUX5_0dv (0x00000000)
#define PRBS7_TMUX5_0rv RVAL(PRBS7_TMUX5_0)
#define RMOD_PRBS7_TMUX5_0(...) RMOD(PRBS7_TMUX5_0, __VA_ARGS__)
#define RIZS_PRBS7_TMUX5_0(...) RIZS(PRBS7_TMUX5_0, __VA_ARGS__)
#define RFLD_PRBS7_TMUX5_0(fld) RFLD(PRBS7_TMUX5_0, fld)

typedef union {
	struct {
		unsigned int mbz_0:21; //0x0
		unsigned int dm_sel:3; //0x0
		unsigned int mbz_1:1; //0x0
		unsigned int dq7_sel:3; //0x0
		unsigned int mbz_2:1; //0x0
		unsigned int dq6_sel:3; //0x0
	} f;
	unsigned int v;
} PRBS7_TMUX5_1_T;
#define PRBS7_TMUX5_1ar (0xB8142420)
#define PRBS7_TMUX5_1dv (0x00000000)
#define PRBS7_TMUX5_1rv RVAL(PRBS7_TMUX5_1)
#define RMOD_PRBS7_TMUX5_1(...) RMOD(PRBS7_TMUX5_1, __VA_ARGS__)
#define RIZS_PRBS7_TMUX5_1(...) RIZS(PRBS7_TMUX5_1, __VA_ARGS__)
#define RFLD_PRBS7_TMUX5_1(fld) RFLD(PRBS7_TMUX5_1, fld)

typedef union {
	struct {
		unsigned int mbz_0:21; //0x0
		unsigned int dm_sel:3; //0x0
		unsigned int mbz_1:1; //0x0
		unsigned int dq7_sel:3; //0x0
		unsigned int mbz_2:1; //0x0
		unsigned int dq6_sel:3; //0x0
	} f;
	unsigned int v;
} PRBS7_TMUX5_2_T;
#define PRBS7_TMUX5_2ar (0xB8142424)
#define PRBS7_TMUX5_2dv (0x00000000)
#define PRBS7_TMUX5_2rv RVAL(PRBS7_TMUX5_2)
#define RMOD_PRBS7_TMUX5_2(...) RMOD(PRBS7_TMUX5_2, __VA_ARGS__)
#define RIZS_PRBS7_TMUX5_2(...) RIZS(PRBS7_TMUX5_2, __VA_ARGS__)
#define RFLD_PRBS7_TMUX5_2(fld) RFLD(PRBS7_TMUX5_2, fld)

typedef union {
	struct {
		unsigned int mbz_0:21; //0x0
		unsigned int dm_sel:3; //0x0
		unsigned int mbz_1:1; //0x0
		unsigned int dq7_sel:3; //0x0
		unsigned int mbz_2:1; //0x0
		unsigned int dq6_sel:3; //0x0
	} f;
	unsigned int v;
} PRBS7_TMUX5_3_T;
#define PRBS7_TMUX5_3ar (0xB8142428)
#define PRBS7_TMUX5_3dv (0x00000000)
#define PRBS7_TMUX5_3rv RVAL(PRBS7_TMUX5_3)
#define RMOD_PRBS7_TMUX5_3(...) RMOD(PRBS7_TMUX5_3, __VA_ARGS__)
#define RIZS_PRBS7_TMUX5_3(...) RIZS(PRBS7_TMUX5_3, __VA_ARGS__)
#define RFLD_PRBS7_TMUX5_3(fld) RFLD(PRBS7_TMUX5_3, fld)

typedef union {
	struct {
		unsigned int mbz_0:29; //0x0
		unsigned int pre_shift_sel_8:3; //0x2
	} f;
	unsigned int v;
} CAL_LS_SEL_DBI_0_T;
#define CAL_LS_SEL_DBI_0ar (0xB814242C)
#define CAL_LS_SEL_DBI_0dv (0x00000002)
#define CAL_LS_SEL_DBI_0rv RVAL(CAL_LS_SEL_DBI_0)
#define RMOD_CAL_LS_SEL_DBI_0(...) RMOD(CAL_LS_SEL_DBI_0, __VA_ARGS__)
#define RIZS_CAL_LS_SEL_DBI_0(...) RIZS(CAL_LS_SEL_DBI_0, __VA_ARGS__)
#define RFLD_CAL_LS_SEL_DBI_0(fld) RFLD(CAL_LS_SEL_DBI_0, fld)

typedef union {
	struct {
		unsigned int mbz_0:29; //0x0
		unsigned int pre_shift_sel_8:3; //0x2
	} f;
	unsigned int v;
} CAL_LS_SEL_DBI_1_T;
#define CAL_LS_SEL_DBI_1ar (0xB8142430)
#define CAL_LS_SEL_DBI_1dv (0x00000002)
#define CAL_LS_SEL_DBI_1rv RVAL(CAL_LS_SEL_DBI_1)
#define RMOD_CAL_LS_SEL_DBI_1(...) RMOD(CAL_LS_SEL_DBI_1, __VA_ARGS__)
#define RIZS_CAL_LS_SEL_DBI_1(...) RIZS(CAL_LS_SEL_DBI_1, __VA_ARGS__)
#define RFLD_CAL_LS_SEL_DBI_1(fld) RFLD(CAL_LS_SEL_DBI_1, fld)

typedef union {
	struct {
		unsigned int mbz_0:29; //0x0
		unsigned int pre_shift_sel_8:3; //0x2
	} f;
	unsigned int v;
} CAL_LS_SEL_DBI_2_T;
#define CAL_LS_SEL_DBI_2ar (0xB8142434)
#define CAL_LS_SEL_DBI_2dv (0x00000002)
#define CAL_LS_SEL_DBI_2rv RVAL(CAL_LS_SEL_DBI_2)
#define RMOD_CAL_LS_SEL_DBI_2(...) RMOD(CAL_LS_SEL_DBI_2, __VA_ARGS__)
#define RIZS_CAL_LS_SEL_DBI_2(...) RIZS(CAL_LS_SEL_DBI_2, __VA_ARGS__)
#define RFLD_CAL_LS_SEL_DBI_2(fld) RFLD(CAL_LS_SEL_DBI_2, fld)

typedef union {
	struct {
		unsigned int mbz_0:29; //0x0
		unsigned int pre_shift_sel_8:3; //0x2
	} f;
	unsigned int v;
} CAL_LS_SEL_DBI_3_T;
#define CAL_LS_SEL_DBI_3ar (0xB8142438)
#define CAL_LS_SEL_DBI_3dv (0x00000002)
#define CAL_LS_SEL_DBI_3rv RVAL(CAL_LS_SEL_DBI_3)
#define RMOD_CAL_LS_SEL_DBI_3(...) RMOD(CAL_LS_SEL_DBI_3, __VA_ARGS__)
#define RIZS_CAL_LS_SEL_DBI_3(...) RIZS(CAL_LS_SEL_DBI_3, __VA_ARGS__)
#define RFLD_CAL_LS_SEL_DBI_3(fld) RFLD(CAL_LS_SEL_DBI_3, fld)

typedef union {
	struct {
		unsigned int mbz_0:29; //0x0
		unsigned int post_shift_sel_8:3; //0x2
	} f;
	unsigned int v;
} CAL_RS_SEL_DBI_0_T;
#define CAL_RS_SEL_DBI_0ar (0xB814243C)
#define CAL_RS_SEL_DBI_0dv (0x00000002)
#define CAL_RS_SEL_DBI_0rv RVAL(CAL_RS_SEL_DBI_0)
#define RMOD_CAL_RS_SEL_DBI_0(...) RMOD(CAL_RS_SEL_DBI_0, __VA_ARGS__)
#define RIZS_CAL_RS_SEL_DBI_0(...) RIZS(CAL_RS_SEL_DBI_0, __VA_ARGS__)
#define RFLD_CAL_RS_SEL_DBI_0(fld) RFLD(CAL_RS_SEL_DBI_0, fld)

typedef union {
	struct {
		unsigned int mbz_0:29; //0x0
		unsigned int post_shift_sel_8:3; //0x2
	} f;
	unsigned int v;
} CAL_RS_SEL_DBI_1_T;
#define CAL_RS_SEL_DBI_1ar (0xB8142440)
#define CAL_RS_SEL_DBI_1dv (0x00000002)
#define CAL_RS_SEL_DBI_1rv RVAL(CAL_RS_SEL_DBI_1)
#define RMOD_CAL_RS_SEL_DBI_1(...) RMOD(CAL_RS_SEL_DBI_1, __VA_ARGS__)
#define RIZS_CAL_RS_SEL_DBI_1(...) RIZS(CAL_RS_SEL_DBI_1, __VA_ARGS__)
#define RFLD_CAL_RS_SEL_DBI_1(fld) RFLD(CAL_RS_SEL_DBI_1, fld)

typedef union {
	struct {
		unsigned int mbz_0:29; //0x0
		unsigned int post_shift_sel_8:3; //0x2
	} f;
	unsigned int v;
} CAL_RS_SEL_DBI_2_T;
#define CAL_RS_SEL_DBI_2ar (0xB8142444)
#define CAL_RS_SEL_DBI_2dv (0x00000002)
#define CAL_RS_SEL_DBI_2rv RVAL(CAL_RS_SEL_DBI_2)
#define RMOD_CAL_RS_SEL_DBI_2(...) RMOD(CAL_RS_SEL_DBI_2, __VA_ARGS__)
#define RIZS_CAL_RS_SEL_DBI_2(...) RIZS(CAL_RS_SEL_DBI_2, __VA_ARGS__)
#define RFLD_CAL_RS_SEL_DBI_2(fld) RFLD(CAL_RS_SEL_DBI_2, fld)

typedef union {
	struct {
		unsigned int mbz_0:29; //0x0
		unsigned int post_shift_sel_8:3; //0x2
	} f;
	unsigned int v;
} CAL_RS_SEL_DBI_3_T;
#define CAL_RS_SEL_DBI_3ar (0xB8142448)
#define CAL_RS_SEL_DBI_3dv (0x00000002)
#define CAL_RS_SEL_DBI_3rv RVAL(CAL_RS_SEL_DBI_3)
#define RMOD_CAL_RS_SEL_DBI_3(...) RMOD(CAL_RS_SEL_DBI_3, __VA_ARGS__)
#define RIZS_CAL_RS_SEL_DBI_3(...) RIZS(CAL_RS_SEL_DBI_3, __VA_ARGS__)
#define RFLD_CAL_RS_SEL_DBI_3(fld) RFLD(CAL_RS_SEL_DBI_3, fld)

typedef union {
	struct {
		unsigned int mbz_0:27; //0x0
		unsigned int fw_rd_dly_pos_sel_8:5; //0xA
	} f;
	unsigned int v;
} DQS_IN_DLY_1_DBI_0_T;
#define DQS_IN_DLY_1_DBI_0ar (0xB814244C)
#define DQS_IN_DLY_1_DBI_0dv (0x0000000A)
#define DQS_IN_DLY_1_DBI_0rv RVAL(DQS_IN_DLY_1_DBI_0)
#define RMOD_DQS_IN_DLY_1_DBI_0(...) RMOD(DQS_IN_DLY_1_DBI_0, __VA_ARGS__)
#define RIZS_DQS_IN_DLY_1_DBI_0(...) RIZS(DQS_IN_DLY_1_DBI_0, __VA_ARGS__)
#define RFLD_DQS_IN_DLY_1_DBI_0(fld) RFLD(DQS_IN_DLY_1_DBI_0, fld)

typedef union {
	struct {
		unsigned int mbz_0:27; //0x0
		unsigned int fw_rd_dly_pos_sel_8:5; //0xA
	} f;
	unsigned int v;
} DQS_IN_DLY_1_DBI_1_T;
#define DQS_IN_DLY_1_DBI_1ar (0xB8142450)
#define DQS_IN_DLY_1_DBI_1dv (0x0000000A)
#define DQS_IN_DLY_1_DBI_1rv RVAL(DQS_IN_DLY_1_DBI_1)
#define RMOD_DQS_IN_DLY_1_DBI_1(...) RMOD(DQS_IN_DLY_1_DBI_1, __VA_ARGS__)
#define RIZS_DQS_IN_DLY_1_DBI_1(...) RIZS(DQS_IN_DLY_1_DBI_1, __VA_ARGS__)
#define RFLD_DQS_IN_DLY_1_DBI_1(fld) RFLD(DQS_IN_DLY_1_DBI_1, fld)

typedef union {
	struct {
		unsigned int mbz_0:27; //0x0
		unsigned int fw_rd_dly_pos_sel_8:5; //0xA
	} f;
	unsigned int v;
} DQS_IN_DLY_1_DBI_2_T;
#define DQS_IN_DLY_1_DBI_2ar (0xB8142454)
#define DQS_IN_DLY_1_DBI_2dv (0x0000000A)
#define DQS_IN_DLY_1_DBI_2rv RVAL(DQS_IN_DLY_1_DBI_2)
#define RMOD_DQS_IN_DLY_1_DBI_2(...) RMOD(DQS_IN_DLY_1_DBI_2, __VA_ARGS__)
#define RIZS_DQS_IN_DLY_1_DBI_2(...) RIZS(DQS_IN_DLY_1_DBI_2, __VA_ARGS__)
#define RFLD_DQS_IN_DLY_1_DBI_2(fld) RFLD(DQS_IN_DLY_1_DBI_2, fld)

typedef union {
	struct {
		unsigned int mbz_0:27; //0x0
		unsigned int fw_rd_dly_pos_sel_8:5; //0xA
	} f;
	unsigned int v;
} DQS_IN_DLY_1_DBI_3_T;
#define DQS_IN_DLY_1_DBI_3ar (0xB8142458)
#define DQS_IN_DLY_1_DBI_3dv (0x0000000A)
#define DQS_IN_DLY_1_DBI_3rv RVAL(DQS_IN_DLY_1_DBI_3)
#define RMOD_DQS_IN_DLY_1_DBI_3(...) RMOD(DQS_IN_DLY_1_DBI_3, __VA_ARGS__)
#define RIZS_DQS_IN_DLY_1_DBI_3(...) RIZS(DQS_IN_DLY_1_DBI_3, __VA_ARGS__)
#define RFLD_DQS_IN_DLY_1_DBI_3(fld) RFLD(DQS_IN_DLY_1_DBI_3, fld)

typedef union {
	struct {
		unsigned int mbz_0:27; //0x0
		unsigned int fw_rd_dly_neg_sel_8:5; //0xA
	} f;
	unsigned int v;
} DQS_IN_DLY_3_DBI_0_T;
#define DQS_IN_DLY_3_DBI_0ar (0xB814245C)
#define DQS_IN_DLY_3_DBI_0dv (0x0000000A)
#define DQS_IN_DLY_3_DBI_0rv RVAL(DQS_IN_DLY_3_DBI_0)
#define RMOD_DQS_IN_DLY_3_DBI_0(...) RMOD(DQS_IN_DLY_3_DBI_0, __VA_ARGS__)
#define RIZS_DQS_IN_DLY_3_DBI_0(...) RIZS(DQS_IN_DLY_3_DBI_0, __VA_ARGS__)
#define RFLD_DQS_IN_DLY_3_DBI_0(fld) RFLD(DQS_IN_DLY_3_DBI_0, fld)

typedef union {
	struct {
		unsigned int mbz_0:27; //0x0
		unsigned int fw_rd_dly_neg_sel_8:5; //0xA
	} f;
	unsigned int v;
} DQS_IN_DLY_3_DBI_1_T;
#define DQS_IN_DLY_3_DBI_1ar (0xB8142460)
#define DQS_IN_DLY_3_DBI_1dv (0x0000000A)
#define DQS_IN_DLY_3_DBI_1rv RVAL(DQS_IN_DLY_3_DBI_1)
#define RMOD_DQS_IN_DLY_3_DBI_1(...) RMOD(DQS_IN_DLY_3_DBI_1, __VA_ARGS__)
#define RIZS_DQS_IN_DLY_3_DBI_1(...) RIZS(DQS_IN_DLY_3_DBI_1, __VA_ARGS__)
#define RFLD_DQS_IN_DLY_3_DBI_1(fld) RFLD(DQS_IN_DLY_3_DBI_1, fld)

typedef union {
	struct {
		unsigned int mbz_0:27; //0x0
		unsigned int fw_rd_dly_neg_sel_8:5; //0xA
	} f;
	unsigned int v;
} DQS_IN_DLY_3_DBI_2_T;
#define DQS_IN_DLY_3_DBI_2ar (0xB8142464)
#define DQS_IN_DLY_3_DBI_2dv (0x0000000A)
#define DQS_IN_DLY_3_DBI_2rv RVAL(DQS_IN_DLY_3_DBI_2)
#define RMOD_DQS_IN_DLY_3_DBI_2(...) RMOD(DQS_IN_DLY_3_DBI_2, __VA_ARGS__)
#define RIZS_DQS_IN_DLY_3_DBI_2(...) RIZS(DQS_IN_DLY_3_DBI_2, __VA_ARGS__)
#define RFLD_DQS_IN_DLY_3_DBI_2(fld) RFLD(DQS_IN_DLY_3_DBI_2, fld)

typedef union {
	struct {
		unsigned int mbz_0:27; //0x0
		unsigned int fw_rd_dly_neg_sel_8:5; //0xA
	} f;
	unsigned int v;
} DQS_IN_DLY_3_DBI_3_T;
#define DQS_IN_DLY_3_DBI_3ar (0xB8142468)
#define DQS_IN_DLY_3_DBI_3dv (0x0000000A)
#define DQS_IN_DLY_3_DBI_3rv RVAL(DQS_IN_DLY_3_DBI_3)
#define RMOD_DQS_IN_DLY_3_DBI_3(...) RMOD(DQS_IN_DLY_3_DBI_3, __VA_ARGS__)
#define RIZS_DQS_IN_DLY_3_DBI_3(...) RIZS(DQS_IN_DLY_3_DBI_3, __VA_ARGS__)
#define RFLD_DQS_IN_DLY_3_DBI_3(fld) RFLD(DQS_IN_DLY_3_DBI_3, fld)

typedef union {
	struct {
		unsigned int rst_dqsen_cal:1; //0x0
		unsigned int mbz_0:3; //0x0
		unsigned int rst_3point_mode:2; //0x0
		unsigned int rst_dqsen_mode:2; //0x0
		unsigned int alert_ena:1; //0x0
		unsigned int alert_1_ena:1; //0x0
		unsigned int rd_update_dly:6; //0x9
		unsigned int mbz_1:3; //0x0
		unsigned int rd_update_dly_rx:5; //0xF
		unsigned int mbz_2:4; //0x0
		unsigned int cs_delay_sel:3; //0x0
		unsigned int cs_delay_en:1; //0x0
	} f;
	unsigned int v;
} DPI_CTRL_2_T;
#define DPI_CTRL_2ar (0xB814246C)
#define DPI_CTRL_2dv (0x00090F00)
#define DPI_CTRL_2rv RVAL(DPI_CTRL_2)
#define RMOD_DPI_CTRL_2(...) RMOD(DPI_CTRL_2, __VA_ARGS__)
#define RIZS_DPI_CTRL_2(...) RIZS(DPI_CTRL_2, __VA_ARGS__)
#define RFLD_DPI_CTRL_2(fld) RFLD(DPI_CTRL_2, fld)

typedef union {
	struct {
		unsigned int mbz_0:3; //0x0
		unsigned int odt_ttcp_set_7:5; //0x0
		unsigned int mbz_1:3; //0x0
		unsigned int odt_ttcp_set_6:5; //0x16
		unsigned int mbz_2:3; //0x0
		unsigned int odt_ttcp_set_5:5; //0x16
		unsigned int mbz_3:3; //0x0
		unsigned int odt_ttcp_set_4:5; //0x16
	} f;
	unsigned int v;
} ODT_TTCP1_SET1_T;
#define ODT_TTCP1_SET1ar (0xB8142470)
#define ODT_TTCP1_SET1dv (0x00161616)
#define ODT_TTCP1_SET1rv RVAL(ODT_TTCP1_SET1)
#define RMOD_ODT_TTCP1_SET1(...) RMOD(ODT_TTCP1_SET1, __VA_ARGS__)
#define RIZS_ODT_TTCP1_SET1(...) RIZS(ODT_TTCP1_SET1, __VA_ARGS__)
#define RFLD_ODT_TTCP1_SET1(fld) RFLD(ODT_TTCP1_SET1, fld)

typedef union {
	struct {
		unsigned int mon_burst_cnt_3:4; //0x0
		unsigned int mon_dqs_cnt_3:4; //0x0
		unsigned int mon_burst_cnt_2:4; //0x0
		unsigned int mon_dqs_cnt_2:4; //0x0
		unsigned int mon_burst_cnt_1:4; //0x0
		unsigned int mon_dqs_cnt_1:4; //0x0
		unsigned int mon_burst_cnt_0:4; //0x0
		unsigned int mon_dqs_cnt_0:4; //0x0
	} f;
	unsigned int v;
} MON_T;
#define MONar (0xB8142474)
#define MONdv (0x00000000)
#define MONrv RVAL(MON)
#define RMOD_MON(...) RMOD(MON, __VA_ARGS__)
#define RIZS_MON(...) RIZS(MON, __VA_ARGS__)
#define RFLD_MON(fld) RFLD(MON, fld)

typedef union {
	struct {
		unsigned int mbz_0:16; //0x0
		unsigned int cs_prbs7_invs:1; //0x0
		unsigned int cs_prbs7_seed:7; //0x1
		unsigned int cmd_prbs7_invs:1; //0x0
		unsigned int cmd_prbs7_seed:7; //0x1
	} f;
	unsigned int v;
} PRBS7_TMUX10_T;
#define PRBS7_TMUX10ar (0xB8142478)
#define PRBS7_TMUX10dv (0x00000101)
#define PRBS7_TMUX10rv RVAL(PRBS7_TMUX10)
#define RMOD_PRBS7_TMUX10(...) RMOD(PRBS7_TMUX10, __VA_ARGS__)
#define RIZS_PRBS7_TMUX10(...) RIZS(PRBS7_TMUX10, __VA_ARGS__)
#define RFLD_PRBS7_TMUX10(fld) RFLD(PRBS7_TMUX10, fld)

typedef union {
	struct {
		unsigned int mbz_0:1; //0x0
		unsigned int mbz_1:3; //0x0
		unsigned int mbz_2:1; //0x0
		unsigned int ddr4_cs_n:3; //0x0
		unsigned int mbz_3:1; //0x0
		unsigned int alert_in_1:3; //0x0
		unsigned int mbz_4:1; //0x0
		unsigned int parity_in_1:3; //0x0
		unsigned int mbz_5:1; //0x0
		unsigned int parity_in:3; //0x0
		unsigned int mbz_6:5; //0x0
		unsigned int ten_in:3; //0x0
		unsigned int mbz_7:1; //0x0
		unsigned int alert_in:3; //0x0
	} f;
	unsigned int v;
} PRBS7_TMUX6_T;
#define PRBS7_TMUX6ar (0xB814247C)
#define PRBS7_TMUX6dv (0x00000000)
#define PRBS7_TMUX6rv RVAL(PRBS7_TMUX6)
#define RMOD_PRBS7_TMUX6(...) RMOD(PRBS7_TMUX6, __VA_ARGS__)
#define RIZS_PRBS7_TMUX6(...) RIZS(PRBS7_TMUX6, __VA_ARGS__)
#define RFLD_PRBS7_TMUX6(fld) RFLD(PRBS7_TMUX6, fld)

typedef union {
	struct {
		unsigned int mbz_0:17; //0x0
		unsigned int odt_1_sel:3; //0x0
		unsigned int mbz_1:1; //0x0
		unsigned int adr16_sel:3; //0x0
		unsigned int mbz_2:1; //0x0
		unsigned int act_n:3; //0x0
		unsigned int mbz_3:1; //0x0
		unsigned int bg0:3; //0x0
	} f;
	unsigned int v;
} PRBS7_TMUX7_T;
#define PRBS7_TMUX7ar (0xB8142480)
#define PRBS7_TMUX7dv (0x00000000)
#define PRBS7_TMUX7rv RVAL(PRBS7_TMUX7)
#define RMOD_PRBS7_TMUX7(...) RMOD(PRBS7_TMUX7, __VA_ARGS__)
#define RIZS_PRBS7_TMUX7(...) RIZS(PRBS7_TMUX7, __VA_ARGS__)
#define RFLD_PRBS7_TMUX7(fld) RFLD(PRBS7_TMUX7, fld)

typedef union {
	struct {
		unsigned int mbz_0:24; //0x0
		unsigned int wrlvl_dqsdata:8; //0x0
	} f;
	unsigned int v;
} WRLVL_DQS_DATA_T;
#define WRLVL_DQS_DATAar (0xB8142484)
#define WRLVL_DQS_DATAdv (0x00000000)
#define WRLVL_DQS_DATArv RVAL(WRLVL_DQS_DATA)
#define RMOD_WRLVL_DQS_DATA(...) RMOD(WRLVL_DQS_DATA, __VA_ARGS__)
#define RIZS_WRLVL_DQS_DATA(...) RIZS(WRLVL_DQS_DATA, __VA_ARGS__)
#define RFLD_WRLVL_DQS_DATA(fld) RFLD(WRLVL_DQS_DATA, fld)

typedef union {
	struct {
		unsigned int test_pattern:32; //0x0
	} f;
	unsigned int v;
} PRBS7_TMUX8_T;
#define PRBS7_TMUX8ar (0xB8142488)
#define PRBS7_TMUX8dv (0x00000000)
#define PRBS7_TMUX8rv RVAL(PRBS7_TMUX8)
#define RMOD_PRBS7_TMUX8(...) RMOD(PRBS7_TMUX8, __VA_ARGS__)
#define RIZS_PRBS7_TMUX8(...) RIZS(PRBS7_TMUX8, __VA_ARGS__)
#define RFLD_PRBS7_TMUX8(fld) RFLD(PRBS7_TMUX8, fld)

typedef union {
	struct {
		unsigned int mbz_0:24; //0x0
		unsigned int tx_wdata_mask_3:1; //0x0
		unsigned int tx_wdata_mask_2:1; //0x0
		unsigned int tx_wdata_mask_1:1; //0x0
		unsigned int tx_wdata_mask_0:1; //0x0
		unsigned int dqs_dq_pd_en_3:1; //0x0
		unsigned int dqs_dq_pd_en_2:1; //0x0
		unsigned int dqs_dq_pd_en_1:1; //0x0
		unsigned int dqs_dq_pd_en_0:1; //0x0
	} f;
	unsigned int v;
} READ_CTRL_5_T;
#define READ_CTRL_5ar (0xB814248C)
#define READ_CTRL_5dv (0x00000000)
#define READ_CTRL_5rv RVAL(READ_CTRL_5)
#define RMOD_READ_CTRL_5(...) RMOD(READ_CTRL_5, __VA_ARGS__)
#define RIZS_READ_CTRL_5(...) RIZS(READ_CTRL_5, __VA_ARGS__)
#define RFLD_READ_CTRL_5(fld) RFLD(READ_CTRL_5, fld)

typedef union {
	struct {
		unsigned int mbz_0:14; //0x0
		unsigned int rx_lag_det_pfifo:9; //0x0
		unsigned int rx_lead_det_pfifo:9; //0x0
	} f;
	unsigned int v;
} VALID_WIN_DET_PFIFO_0_T;
#define VALID_WIN_DET_PFIFO_0ar (0xB8142490)
#define VALID_WIN_DET_PFIFO_0dv (0x00000000)
#define VALID_WIN_DET_PFIFO_0rv RVAL(VALID_WIN_DET_PFIFO_0)
#define RMOD_VALID_WIN_DET_PFIFO_0(...) RMOD(VALID_WIN_DET_PFIFO_0, __VA_ARGS__)
#define RIZS_VALID_WIN_DET_PFIFO_0(...) RIZS(VALID_WIN_DET_PFIFO_0, __VA_ARGS__)
#define RFLD_VALID_WIN_DET_PFIFO_0(fld) RFLD(VALID_WIN_DET_PFIFO_0, fld)

typedef union {
	struct {
		unsigned int mbz_0:14; //0x0
		unsigned int rx_lag_det_pfifo:9; //0x0
		unsigned int rx_lead_det_pfifo:9; //0x0
	} f;
	unsigned int v;
} VALID_WIN_DET_PFIFO_1_T;
#define VALID_WIN_DET_PFIFO_1ar (0xB8142494)
#define VALID_WIN_DET_PFIFO_1dv (0x00000000)
#define VALID_WIN_DET_PFIFO_1rv RVAL(VALID_WIN_DET_PFIFO_1)
#define RMOD_VALID_WIN_DET_PFIFO_1(...) RMOD(VALID_WIN_DET_PFIFO_1, __VA_ARGS__)
#define RIZS_VALID_WIN_DET_PFIFO_1(...) RIZS(VALID_WIN_DET_PFIFO_1, __VA_ARGS__)
#define RFLD_VALID_WIN_DET_PFIFO_1(fld) RFLD(VALID_WIN_DET_PFIFO_1, fld)

typedef union {
	struct {
		unsigned int mbz_0:14; //0x0
		unsigned int rx_lag_det_pfifo:9; //0x0
		unsigned int rx_lead_det_pfifo:9; //0x0
	} f;
	unsigned int v;
} VALID_WIN_DET_PFIFO_2_T;
#define VALID_WIN_DET_PFIFO_2ar (0xB8142498)
#define VALID_WIN_DET_PFIFO_2dv (0x00000000)
#define VALID_WIN_DET_PFIFO_2rv RVAL(VALID_WIN_DET_PFIFO_2)
#define RMOD_VALID_WIN_DET_PFIFO_2(...) RMOD(VALID_WIN_DET_PFIFO_2, __VA_ARGS__)
#define RIZS_VALID_WIN_DET_PFIFO_2(...) RIZS(VALID_WIN_DET_PFIFO_2, __VA_ARGS__)
#define RFLD_VALID_WIN_DET_PFIFO_2(fld) RFLD(VALID_WIN_DET_PFIFO_2, fld)

typedef union {
	struct {
		unsigned int mbz_0:14; //0x0
		unsigned int rx_lag_det_pfifo:9; //0x0
		unsigned int rx_lead_det_pfifo:9; //0x0
	} f;
	unsigned int v;
} VALID_WIN_DET_PFIFO_3_T;
#define VALID_WIN_DET_PFIFO_3ar (0xB814249C)
#define VALID_WIN_DET_PFIFO_3dv (0x00000000)
#define VALID_WIN_DET_PFIFO_3rv RVAL(VALID_WIN_DET_PFIFO_3)
#define RMOD_VALID_WIN_DET_PFIFO_3(...) RMOD(VALID_WIN_DET_PFIFO_3, __VA_ARGS__)
#define RIZS_VALID_WIN_DET_PFIFO_3(...) RIZS(VALID_WIN_DET_PFIFO_3, __VA_ARGS__)
#define RFLD_VALID_WIN_DET_PFIFO_3(fld) RFLD(VALID_WIN_DET_PFIFO_3, fld)

typedef union {
	struct {
		unsigned int mbz_0:4; //0x0
		unsigned int cke_1_rpt_dec:1; //0x0
		unsigned int cke_1_rpt_add:1; //0x0
		unsigned int cke_rpt_dec:1; //0x0
		unsigned int cke_rpt_add:1; //0x0
		unsigned int cs_1_rpt_dec:1; //0x0
		unsigned int cs_1_rpt_add:1; //0x0
		unsigned int cs_rpt_dec:1; //0x0
		unsigned int cs_rpt_add:1; //0x0
		unsigned int ca_rpt_dec:1; //0x0
		unsigned int ca_rpt_add:1; //0x0
		unsigned int rx_rpt_dec:1; //0x0
		unsigned int rx_rpt_add:1; //0x0
		unsigned int dqs_rpt_dec_3:1; //0x0
		unsigned int dqs_rpt_add_3:1; //0x0
		unsigned int dqs_rpt_dec_2:1; //0x0
		unsigned int dqs_rpt_add_2:1; //0x0
		unsigned int dqs_rpt_dec_1:1; //0x0
		unsigned int dqs_rpt_add_1:1; //0x0
		unsigned int dqs_rpt_dec_0:1; //0x0
		unsigned int dqs_rpt_add_0:1; //0x0
		unsigned int dq_rpt_dec_3:1; //0x0
		unsigned int dq_rpt_add_3:1; //0x0
		unsigned int dq_rpt_dec_2:1; //0x0
		unsigned int dq_rpt_add_2:1; //0x0
		unsigned int dq_rpt_dec_1:1; //0x0
		unsigned int dq_rpt_add_1:1; //0x0
		unsigned int dq_rpt_dec_0:1; //0x0
		unsigned int dq_rpt_add_0:1; //0x0
	} f;
	unsigned int v;
} TX_RPT_CTRL_T;
#define TX_RPT_CTRLar (0xB81424A0)
#define TX_RPT_CTRLdv (0x00000000)
#define TX_RPT_CTRLrv RVAL(TX_RPT_CTRL)
#define RMOD_TX_RPT_CTRL(...) RMOD(TX_RPT_CTRL, __VA_ARGS__)
#define RIZS_TX_RPT_CTRL(...) RIZS(TX_RPT_CTRL, __VA_ARGS__)
#define RFLD_TX_RPT_CTRL(fld) RFLD(TX_RPT_CTRL, fld)

typedef union {
	struct {
		unsigned int mbz_0:16; //0x0
		unsigned int rw_adr_dbi:16; //0x0
	} f;
	unsigned int v;
} DBI_FIFO_0_T;
#define DBI_FIFO_0ar (0xB81424A4)
#define DBI_FIFO_0dv (0x00000000)
#define DBI_FIFO_0rv RVAL(DBI_FIFO_0)
#define RMOD_DBI_FIFO_0(...) RMOD(DBI_FIFO_0, __VA_ARGS__)
#define RIZS_DBI_FIFO_0(...) RIZS(DBI_FIFO_0, __VA_ARGS__)
#define RFLD_DBI_FIFO_0(fld) RFLD(DBI_FIFO_0, fld)

typedef union {
	struct {
		unsigned int mbz_0:16; //0x0
		unsigned int rw_adr_dbi:16; //0x0
	} f;
	unsigned int v;
} DBI_FIFO_1_T;
#define DBI_FIFO_1ar (0xB81424A8)
#define DBI_FIFO_1dv (0x00000000)
#define DBI_FIFO_1rv RVAL(DBI_FIFO_1)
#define RMOD_DBI_FIFO_1(...) RMOD(DBI_FIFO_1, __VA_ARGS__)
#define RIZS_DBI_FIFO_1(...) RIZS(DBI_FIFO_1, __VA_ARGS__)
#define RFLD_DBI_FIFO_1(fld) RFLD(DBI_FIFO_1, fld)

typedef union {
	struct {
		unsigned int mbz_0:16; //0x0
		unsigned int rw_adr_dbi:16; //0x0
	} f;
	unsigned int v;
} DBI_FIFO_2_T;
#define DBI_FIFO_2ar (0xB81424AC)
#define DBI_FIFO_2dv (0x00000000)
#define DBI_FIFO_2rv RVAL(DBI_FIFO_2)
#define RMOD_DBI_FIFO_2(...) RMOD(DBI_FIFO_2, __VA_ARGS__)
#define RIZS_DBI_FIFO_2(...) RIZS(DBI_FIFO_2, __VA_ARGS__)
#define RFLD_DBI_FIFO_2(fld) RFLD(DBI_FIFO_2, fld)

typedef union {
	struct {
		unsigned int mbz_0:16; //0x0
		unsigned int rw_adr_dbi:16; //0x0
	} f;
	unsigned int v;
} DBI_FIFO_3_T;
#define DBI_FIFO_3ar (0xB81424B0)
#define DBI_FIFO_3dv (0x00000000)
#define DBI_FIFO_3rv RVAL(DBI_FIFO_3)
#define RMOD_DBI_FIFO_3(...) RMOD(DBI_FIFO_3, __VA_ARGS__)
#define RIZS_DBI_FIFO_3(...) RIZS(DBI_FIFO_3, __VA_ARGS__)
#define RFLD_DBI_FIFO_3(fld) RFLD(DBI_FIFO_3, fld)

typedef union {
	struct {
		unsigned int zq_bkg_ena:1; //0x0
		unsigned int dis_szq_cmd:1; //0x0
		unsigned int mbz_0:2; //0x0
		unsigned int mbz_1:2; //0x0
		unsigned int rzq_ncyc_lrg:10; //0x190
		unsigned int zq_pd_sw_time:4; //0x7
		unsigned int mbz_2:2; //0x0
		unsigned int zcal_lrg_time:10; //0x5A
	} f;
	unsigned int v;
} ZQ_BKG_CTRL_0_T;
#define ZQ_BKG_CTRL_0ar (0xB81424B4)
#define ZQ_BKG_CTRL_0dv (0x0190705A)
#define ZQ_BKG_CTRL_0rv RVAL(ZQ_BKG_CTRL_0)
#define RMOD_ZQ_BKG_CTRL_0(...) RMOD(ZQ_BKG_CTRL_0, __VA_ARGS__)
#define RIZS_ZQ_BKG_CTRL_0(...) RIZS(ZQ_BKG_CTRL_0, __VA_ARGS__)
#define RFLD_ZQ_BKG_CTRL_0(fld) RFLD(ZQ_BKG_CTRL_0, fld)

typedef union {
	struct {
		unsigned int mbz_0:16; //0x0
		unsigned int vref_rzq_dn2_sel:8; //0x0
		unsigned int cal_zq_set_ena:8; //0x0
	} f;
	unsigned int v;
} ZQ_BKG_CTRL_1_T;
#define ZQ_BKG_CTRL_1ar (0xB81424B8)
#define ZQ_BKG_CTRL_1dv (0x00000000)
#define ZQ_BKG_CTRL_1rv RVAL(ZQ_BKG_CTRL_1)
#define RMOD_ZQ_BKG_CTRL_1(...) RMOD(ZQ_BKG_CTRL_1, __VA_ARGS__)
#define RIZS_ZQ_BKG_CTRL_1(...) RIZS(ZQ_BKG_CTRL_1, __VA_ARGS__)
#define RFLD_ZQ_BKG_CTRL_1(fld) RFLD(ZQ_BKG_CTRL_1, fld)

typedef union {
	struct {
		unsigned int mbz_0:1; //0x0
		unsigned int cke_1_ph_en:1; //0x0
		unsigned int cke_ph_en:1; //0x0
		unsigned int cs_1_ph_en:1; //0x0
		unsigned int cs_ph_en:1; //0x0
		unsigned int cmd_ph_en:1; //0x0
		unsigned int mbz_1:1; //0x0
		unsigned int cke_1_geardown:1; //0x0
		unsigned int cke_geardown:1; //0x0
		unsigned int cs_1_geardown:1; //0x0
		unsigned int cs_geardown:1; //0x0
		unsigned int cmd_geardown:1; //0x0
		unsigned int mbz_2:1; //0x0
		unsigned int cke_1_ph_sel:3; //0x0
		unsigned int mbz_3:1; //0x0
		unsigned int cke_ph_sel:3; //0x0
		unsigned int mbz_4:1; //0x0
		unsigned int cs_1_ph_sel:3; //0x0
		unsigned int mbz_5:1; //0x0
		unsigned int cs_ph_sel:3; //0x0
		unsigned int mbz_6:1; //0x0
		unsigned int cmd_ph_sel:3; //0x0
	} f;
	unsigned int v;
} CMD_ADR_PH_T;
#define CMD_ADR_PHar (0xB81424BC)
#define CMD_ADR_PHdv (0x00000000)
#define CMD_ADR_PHrv RVAL(CMD_ADR_PH)
#define RMOD_CMD_ADR_PH(...) RMOD(CMD_ADR_PH, __VA_ARGS__)
#define RIZS_CMD_ADR_PH(...) RIZS(CMD_ADR_PH, __VA_ARGS__)
#define RFLD_CMD_ADR_PH(fld) RFLD(CMD_ADR_PH, fld)

typedef union {
	struct {
		unsigned int tm_odt_en:32; //0xFE
	} f;
	unsigned int v;
} READ_CTRL_6_0_T;
#define READ_CTRL_6_0ar (0xB81424C0)
#define READ_CTRL_6_0dv (0x000000FE)
#define READ_CTRL_6_0rv RVAL(READ_CTRL_6_0)
#define RMOD_READ_CTRL_6_0(...) RMOD(READ_CTRL_6_0, __VA_ARGS__)
#define RIZS_READ_CTRL_6_0(...) RIZS(READ_CTRL_6_0, __VA_ARGS__)
#define RFLD_READ_CTRL_6_0(fld) RFLD(READ_CTRL_6_0, fld)

typedef union {
	struct {
		unsigned int tm_odt_en:32; //0xFE
	} f;
	unsigned int v;
} READ_CTRL_6_1_T;
#define READ_CTRL_6_1ar (0xB81424C4)
#define READ_CTRL_6_1dv (0x000000FE)
#define READ_CTRL_6_1rv RVAL(READ_CTRL_6_1)
#define RMOD_READ_CTRL_6_1(...) RMOD(READ_CTRL_6_1, __VA_ARGS__)
#define RIZS_READ_CTRL_6_1(...) RIZS(READ_CTRL_6_1, __VA_ARGS__)
#define RFLD_READ_CTRL_6_1(fld) RFLD(READ_CTRL_6_1, fld)

typedef union {
	struct {
		unsigned int tm_odt_en:32; //0xFE
	} f;
	unsigned int v;
} READ_CTRL_6_2_T;
#define READ_CTRL_6_2ar (0xB81424C8)
#define READ_CTRL_6_2dv (0x000000FE)
#define READ_CTRL_6_2rv RVAL(READ_CTRL_6_2)
#define RMOD_READ_CTRL_6_2(...) RMOD(READ_CTRL_6_2, __VA_ARGS__)
#define RIZS_READ_CTRL_6_2(...) RIZS(READ_CTRL_6_2, __VA_ARGS__)
#define RFLD_READ_CTRL_6_2(fld) RFLD(READ_CTRL_6_2, fld)

typedef union {
	struct {
		unsigned int tm_odt_en:32; //0xFE
	} f;
	unsigned int v;
} READ_CTRL_6_3_T;
#define READ_CTRL_6_3ar (0xB81424CC)
#define READ_CTRL_6_3dv (0x000000FE)
#define READ_CTRL_6_3rv RVAL(READ_CTRL_6_3)
#define RMOD_READ_CTRL_6_3(...) RMOD(READ_CTRL_6_3, __VA_ARGS__)
#define RIZS_READ_CTRL_6_3(...) RIZS(READ_CTRL_6_3, __VA_ARGS__)
#define RFLD_READ_CTRL_6_3(fld) RFLD(READ_CTRL_6_3, fld)

typedef union {
	struct {
		unsigned int tm_pupd_rst:32; //0xFE
	} f;
	unsigned int v;
} READ_CTRL_8_0_T;
#define READ_CTRL_8_0ar (0xB81424D0)
#define READ_CTRL_8_0dv (0x000000FE)
#define READ_CTRL_8_0rv RVAL(READ_CTRL_8_0)
#define RMOD_READ_CTRL_8_0(...) RMOD(READ_CTRL_8_0, __VA_ARGS__)
#define RIZS_READ_CTRL_8_0(...) RIZS(READ_CTRL_8_0, __VA_ARGS__)
#define RFLD_READ_CTRL_8_0(fld) RFLD(READ_CTRL_8_0, fld)

typedef union {
	struct {
		unsigned int tm_pupd_rst:32; //0xFE
	} f;
	unsigned int v;
} READ_CTRL_8_1_T;
#define READ_CTRL_8_1ar (0xB81424D4)
#define READ_CTRL_8_1dv (0x000000FE)
#define READ_CTRL_8_1rv RVAL(READ_CTRL_8_1)
#define RMOD_READ_CTRL_8_1(...) RMOD(READ_CTRL_8_1, __VA_ARGS__)
#define RIZS_READ_CTRL_8_1(...) RIZS(READ_CTRL_8_1, __VA_ARGS__)
#define RFLD_READ_CTRL_8_1(fld) RFLD(READ_CTRL_8_1, fld)

typedef union {
	struct {
		unsigned int tm_pupd_rst:32; //0xFE
	} f;
	unsigned int v;
} READ_CTRL_8_2_T;
#define READ_CTRL_8_2ar (0xB81424D8)
#define READ_CTRL_8_2dv (0x000000FE)
#define READ_CTRL_8_2rv RVAL(READ_CTRL_8_2)
#define RMOD_READ_CTRL_8_2(...) RMOD(READ_CTRL_8_2, __VA_ARGS__)
#define RIZS_READ_CTRL_8_2(...) RIZS(READ_CTRL_8_2, __VA_ARGS__)
#define RFLD_READ_CTRL_8_2(fld) RFLD(READ_CTRL_8_2, fld)

typedef union {
	struct {
		unsigned int tm_pupd_rst:32; //0xFE
	} f;
	unsigned int v;
} READ_CTRL_8_3_T;
#define READ_CTRL_8_3ar (0xB81424DC)
#define READ_CTRL_8_3dv (0x000000FE)
#define READ_CTRL_8_3rv RVAL(READ_CTRL_8_3)
#define RMOD_READ_CTRL_8_3(...) RMOD(READ_CTRL_8_3, __VA_ARGS__)
#define RIZS_READ_CTRL_8_3(...) RIZS(READ_CTRL_8_3, __VA_ARGS__)
#define RFLD_READ_CTRL_8_3(fld) RFLD(READ_CTRL_8_3, fld)

typedef union {
	struct {
		unsigned int mbz_0:11; //0x0
		unsigned int tm_dqs_cal_pre:5; //0x0
		unsigned int mbz_1:3; //0x0
		unsigned int tm_dqsen_early_ph:5; //0xB
		unsigned int mbz_2:3; //0x0
		unsigned int tm_dqsen_late_ph:5; //0xB
	} f;
	unsigned int v;
} READ_CTRL_3_0_T;
#define READ_CTRL_3_0ar (0xB81424E0)
#define READ_CTRL_3_0dv (0x00000B0B)
#define READ_CTRL_3_0rv RVAL(READ_CTRL_3_0)
#define RMOD_READ_CTRL_3_0(...) RMOD(READ_CTRL_3_0, __VA_ARGS__)
#define RIZS_READ_CTRL_3_0(...) RIZS(READ_CTRL_3_0, __VA_ARGS__)
#define RFLD_READ_CTRL_3_0(fld) RFLD(READ_CTRL_3_0, fld)

typedef union {
	struct {
		unsigned int mbz_0:11; //0x0
		unsigned int tm_dqs_cal_pre:5; //0x0
		unsigned int mbz_1:3; //0x0
		unsigned int tm_dqsen_early_ph:5; //0xB
		unsigned int mbz_2:3; //0x0
		unsigned int tm_dqsen_late_ph:5; //0xB
	} f;
	unsigned int v;
} READ_CTRL_3_1_T;
#define READ_CTRL_3_1ar (0xB81424E4)
#define READ_CTRL_3_1dv (0x00000B0B)
#define READ_CTRL_3_1rv RVAL(READ_CTRL_3_1)
#define RMOD_READ_CTRL_3_1(...) RMOD(READ_CTRL_3_1, __VA_ARGS__)
#define RIZS_READ_CTRL_3_1(...) RIZS(READ_CTRL_3_1, __VA_ARGS__)
#define RFLD_READ_CTRL_3_1(fld) RFLD(READ_CTRL_3_1, fld)

typedef union {
	struct {
		unsigned int mbz_0:11; //0x0
		unsigned int tm_dqs_cal_pre:5; //0x0
		unsigned int mbz_1:3; //0x0
		unsigned int tm_dqsen_early_ph:5; //0xB
		unsigned int mbz_2:3; //0x0
		unsigned int tm_dqsen_late_ph:5; //0xB
	} f;
	unsigned int v;
} READ_CTRL_3_2_T;
#define READ_CTRL_3_2ar (0xB81424E8)
#define READ_CTRL_3_2dv (0x00000B0B)
#define READ_CTRL_3_2rv RVAL(READ_CTRL_3_2)
#define RMOD_READ_CTRL_3_2(...) RMOD(READ_CTRL_3_2, __VA_ARGS__)
#define RIZS_READ_CTRL_3_2(...) RIZS(READ_CTRL_3_2, __VA_ARGS__)
#define RFLD_READ_CTRL_3_2(fld) RFLD(READ_CTRL_3_2, fld)

typedef union {
	struct {
		unsigned int mbz_0:11; //0x0
		unsigned int tm_dqs_cal_pre:5; //0x0
		unsigned int mbz_1:3; //0x0
		unsigned int tm_dqsen_early_ph:5; //0xB
		unsigned int mbz_2:3; //0x0
		unsigned int tm_dqsen_late_ph:5; //0xB
	} f;
	unsigned int v;
} READ_CTRL_3_3_T;
#define READ_CTRL_3_3ar (0xB81424EC)
#define READ_CTRL_3_3dv (0x00000B0B)
#define READ_CTRL_3_3rv RVAL(READ_CTRL_3_3)
#define RMOD_READ_CTRL_3_3(...) RMOD(READ_CTRL_3_3, __VA_ARGS__)
#define RIZS_READ_CTRL_3_3(...) RIZS(READ_CTRL_3_3, __VA_ARGS__)
#define RFLD_READ_CTRL_3_3(fld) RFLD(READ_CTRL_3_3, fld)

typedef union {
	struct {
		unsigned int vref_sr1_int_neg:8; //0x0
		unsigned int vref_sr1_int_pos:8; //0x0
		unsigned int vref_sl0_int_neg:8; //0x0
		unsigned int vref_sl0_int_pos:8; //0x0
	} f;
	unsigned int v;
} INT_STATUS_4_0_T;
#define INT_STATUS_4_0ar (0xB81424F0)
#define INT_STATUS_4_0dv (0x00000000)
#define INT_STATUS_4_0rv RVAL(INT_STATUS_4_0)
#define RMOD_INT_STATUS_4_0(...) RMOD(INT_STATUS_4_0, __VA_ARGS__)
#define RIZS_INT_STATUS_4_0(...) RIZS(INT_STATUS_4_0, __VA_ARGS__)
#define RFLD_INT_STATUS_4_0(fld) RFLD(INT_STATUS_4_0, fld)

typedef union {
	struct {
		unsigned int vref_sr1_int_neg:8; //0x0
		unsigned int vref_sr1_int_pos:8; //0x0
		unsigned int vref_sl0_int_neg:8; //0x0
		unsigned int vref_sl0_int_pos:8; //0x0
	} f;
	unsigned int v;
} INT_STATUS_4_1_T;
#define INT_STATUS_4_1ar (0xB81424F4)
#define INT_STATUS_4_1dv (0x00000000)
#define INT_STATUS_4_1rv RVAL(INT_STATUS_4_1)
#define RMOD_INT_STATUS_4_1(...) RMOD(INT_STATUS_4_1, __VA_ARGS__)
#define RIZS_INT_STATUS_4_1(...) RIZS(INT_STATUS_4_1, __VA_ARGS__)
#define RFLD_INT_STATUS_4_1(fld) RFLD(INT_STATUS_4_1, fld)

typedef union {
	struct {
		unsigned int vref_sr1_int_neg:8; //0x0
		unsigned int vref_sr1_int_pos:8; //0x0
		unsigned int vref_sl0_int_neg:8; //0x0
		unsigned int vref_sl0_int_pos:8; //0x0
	} f;
	unsigned int v;
} INT_STATUS_4_2_T;
#define INT_STATUS_4_2ar (0xB81424F8)
#define INT_STATUS_4_2dv (0x00000000)
#define INT_STATUS_4_2rv RVAL(INT_STATUS_4_2)
#define RMOD_INT_STATUS_4_2(...) RMOD(INT_STATUS_4_2, __VA_ARGS__)
#define RIZS_INT_STATUS_4_2(...) RIZS(INT_STATUS_4_2, __VA_ARGS__)
#define RFLD_INT_STATUS_4_2(fld) RFLD(INT_STATUS_4_2, fld)

typedef union {
	struct {
		unsigned int vref_sr1_int_neg:8; //0x0
		unsigned int vref_sr1_int_pos:8; //0x0
		unsigned int vref_sl0_int_neg:8; //0x0
		unsigned int vref_sl0_int_pos:8; //0x0
	} f;
	unsigned int v;
} INT_STATUS_4_3_T;
#define INT_STATUS_4_3ar (0xB81424FC)
#define INT_STATUS_4_3dv (0x00000000)
#define INT_STATUS_4_3rv RVAL(INT_STATUS_4_3)
#define RMOD_INT_STATUS_4_3(...) RMOD(INT_STATUS_4_3, __VA_ARGS__)
#define RIZS_INT_STATUS_4_3(...) RIZS(INT_STATUS_4_3, __VA_ARGS__)
#define RFLD_INT_STATUS_4_3(fld) RFLD(INT_STATUS_4_3, fld)

typedef union {
	struct {
		unsigned int mbz_0:14; //0x0
		unsigned int rx_lag_det_nfifo:9; //0x0
		unsigned int rx_lead_det_nfifo:9; //0x0
	} f;
	unsigned int v;
} VALID_WIN_DET_NFIFO_0_T;
#define VALID_WIN_DET_NFIFO_0ar (0xB8142500)
#define VALID_WIN_DET_NFIFO_0dv (0x00000000)
#define VALID_WIN_DET_NFIFO_0rv RVAL(VALID_WIN_DET_NFIFO_0)
#define RMOD_VALID_WIN_DET_NFIFO_0(...) RMOD(VALID_WIN_DET_NFIFO_0, __VA_ARGS__)
#define RIZS_VALID_WIN_DET_NFIFO_0(...) RIZS(VALID_WIN_DET_NFIFO_0, __VA_ARGS__)
#define RFLD_VALID_WIN_DET_NFIFO_0(fld) RFLD(VALID_WIN_DET_NFIFO_0, fld)

typedef union {
	struct {
		unsigned int mbz_0:14; //0x0
		unsigned int rx_lag_det_nfifo:9; //0x0
		unsigned int rx_lead_det_nfifo:9; //0x0
	} f;
	unsigned int v;
} VALID_WIN_DET_NFIFO_1_T;
#define VALID_WIN_DET_NFIFO_1ar (0xB8142504)
#define VALID_WIN_DET_NFIFO_1dv (0x00000000)
#define VALID_WIN_DET_NFIFO_1rv RVAL(VALID_WIN_DET_NFIFO_1)
#define RMOD_VALID_WIN_DET_NFIFO_1(...) RMOD(VALID_WIN_DET_NFIFO_1, __VA_ARGS__)
#define RIZS_VALID_WIN_DET_NFIFO_1(...) RIZS(VALID_WIN_DET_NFIFO_1, __VA_ARGS__)
#define RFLD_VALID_WIN_DET_NFIFO_1(fld) RFLD(VALID_WIN_DET_NFIFO_1, fld)

typedef union {
	struct {
		unsigned int mbz_0:14; //0x0
		unsigned int rx_lag_det_nfifo:9; //0x0
		unsigned int rx_lead_det_nfifo:9; //0x0
	} f;
	unsigned int v;
} VALID_WIN_DET_NFIFO_2_T;
#define VALID_WIN_DET_NFIFO_2ar (0xB8142508)
#define VALID_WIN_DET_NFIFO_2dv (0x00000000)
#define VALID_WIN_DET_NFIFO_2rv RVAL(VALID_WIN_DET_NFIFO_2)
#define RMOD_VALID_WIN_DET_NFIFO_2(...) RMOD(VALID_WIN_DET_NFIFO_2, __VA_ARGS__)
#define RIZS_VALID_WIN_DET_NFIFO_2(...) RIZS(VALID_WIN_DET_NFIFO_2, __VA_ARGS__)
#define RFLD_VALID_WIN_DET_NFIFO_2(fld) RFLD(VALID_WIN_DET_NFIFO_2, fld)

typedef union {
	struct {
		unsigned int mbz_0:14; //0x0
		unsigned int rx_lag_det_nfifo:9; //0x0
		unsigned int rx_lead_det_nfifo:9; //0x0
	} f;
	unsigned int v;
} VALID_WIN_DET_NFIFO_3_T;
#define VALID_WIN_DET_NFIFO_3ar (0xB814250C)
#define VALID_WIN_DET_NFIFO_3dv (0x00000000)
#define VALID_WIN_DET_NFIFO_3rv RVAL(VALID_WIN_DET_NFIFO_3)
#define RMOD_VALID_WIN_DET_NFIFO_3(...) RMOD(VALID_WIN_DET_NFIFO_3, __VA_ARGS__)
#define RIZS_VALID_WIN_DET_NFIFO_3(...) RIZS(VALID_WIN_DET_NFIFO_3, __VA_ARGS__)
#define RFLD_VALID_WIN_DET_NFIFO_3(fld) RFLD(VALID_WIN_DET_NFIFO_3, fld)

typedef union {
	struct {
		unsigned int mbz_0:28; //0x0
		unsigned int tm_pupd_rst_odd:1; //0x1
		unsigned int tm_pupd_rst_sel:2; //0x0
		unsigned int tm_pupd_rst_dly:1; //0x0
	} f;
	unsigned int v;
} READ_CTRL_4_0_T;
#define READ_CTRL_4_0ar (0xB8142510)
#define READ_CTRL_4_0dv (0x00000008)
#define READ_CTRL_4_0rv RVAL(READ_CTRL_4_0)
#define RMOD_READ_CTRL_4_0(...) RMOD(READ_CTRL_4_0, __VA_ARGS__)
#define RIZS_READ_CTRL_4_0(...) RIZS(READ_CTRL_4_0, __VA_ARGS__)
#define RFLD_READ_CTRL_4_0(fld) RFLD(READ_CTRL_4_0, fld)

typedef union {
	struct {
		unsigned int mbz_0:28; //0x0
		unsigned int tm_pupd_rst_odd:1; //0x1
		unsigned int tm_pupd_rst_sel:2; //0x0
		unsigned int tm_pupd_rst_dly:1; //0x0
	} f;
	unsigned int v;
} READ_CTRL_4_1_T;
#define READ_CTRL_4_1ar (0xB8142514)
#define READ_CTRL_4_1dv (0x00000008)
#define READ_CTRL_4_1rv RVAL(READ_CTRL_4_1)
#define RMOD_READ_CTRL_4_1(...) RMOD(READ_CTRL_4_1, __VA_ARGS__)
#define RIZS_READ_CTRL_4_1(...) RIZS(READ_CTRL_4_1, __VA_ARGS__)
#define RFLD_READ_CTRL_4_1(fld) RFLD(READ_CTRL_4_1, fld)

typedef union {
	struct {
		unsigned int mbz_0:28; //0x0
		unsigned int tm_pupd_rst_odd:1; //0x1
		unsigned int tm_pupd_rst_sel:2; //0x0
		unsigned int tm_pupd_rst_dly:1; //0x0
	} f;
	unsigned int v;
} READ_CTRL_4_2_T;
#define READ_CTRL_4_2ar (0xB8142518)
#define READ_CTRL_4_2dv (0x00000008)
#define READ_CTRL_4_2rv RVAL(READ_CTRL_4_2)
#define RMOD_READ_CTRL_4_2(...) RMOD(READ_CTRL_4_2, __VA_ARGS__)
#define RIZS_READ_CTRL_4_2(...) RIZS(READ_CTRL_4_2, __VA_ARGS__)
#define RFLD_READ_CTRL_4_2(fld) RFLD(READ_CTRL_4_2, fld)

typedef union {
	struct {
		unsigned int mbz_0:28; //0x0
		unsigned int tm_pupd_rst_odd:1; //0x1
		unsigned int tm_pupd_rst_sel:2; //0x0
		unsigned int tm_pupd_rst_dly:1; //0x0
	} f;
	unsigned int v;
} READ_CTRL_4_3_T;
#define READ_CTRL_4_3ar (0xB814251C)
#define READ_CTRL_4_3dv (0x00000008)
#define READ_CTRL_4_3rv RVAL(READ_CTRL_4_3)
#define RMOD_READ_CTRL_4_3(...) RMOD(READ_CTRL_4_3, __VA_ARGS__)
#define RIZS_READ_CTRL_4_3(...) RIZS(READ_CTRL_4_3, __VA_ARGS__)
#define RFLD_READ_CTRL_4_3(fld) RFLD(READ_CTRL_4_3, fld)

typedef union {
	struct {
		unsigned int mbz_0:3; //0x0
		unsigned int det_dqs_shift_sel_3:5; //0xA
		unsigned int det_post_shift_sel_3:4; //0x3
		unsigned int det_pre_shift_sel_3:4; //0x3
		unsigned int mbz_1:3; //0x0
		unsigned int det_dqs_shift_sel_1:5; //0xA
		unsigned int det_post_shift_sel_1:4; //0x3
		unsigned int det_pre_shift_sel_1:4; //0x3
	} f;
	unsigned int v;
} VALID_WIN_SEL_ODD_T;
#define VALID_WIN_SEL_ODDar (0xB8142520)
#define VALID_WIN_SEL_ODDdv (0x0A330A33)
#define VALID_WIN_SEL_ODDrv RVAL(VALID_WIN_SEL_ODD)
#define RMOD_VALID_WIN_SEL_ODD(...) RMOD(VALID_WIN_SEL_ODD, __VA_ARGS__)
#define RIZS_VALID_WIN_SEL_ODD(...) RIZS(VALID_WIN_SEL_ODD, __VA_ARGS__)
#define RFLD_VALID_WIN_SEL_ODD(fld) RFLD(VALID_WIN_SEL_ODD, fld)

typedef union {
	struct {
		unsigned int mbz_0:3; //0x0
		unsigned int det_dqs_shift_sel_ph_3:5; //0x0
		unsigned int det_post_shift_sel_ph_3:4; //0x0
		unsigned int det_pre_shift_sel_ph_3:4; //0x0
		unsigned int mbz_1:3; //0x0
		unsigned int det_dqs_shift_sel_ph_1:5; //0x0
		unsigned int det_post_shift_sel_ph_1:4; //0x0
		unsigned int det_pre_shift_sel_ph_1:4; //0x0
	} f;
	unsigned int v;
} VALID_WIN_PH_ODD_T;
#define VALID_WIN_PH_ODDar (0xB8142524)
#define VALID_WIN_PH_ODDdv (0x00000000)
#define VALID_WIN_PH_ODDrv RVAL(VALID_WIN_PH_ODD)
#define RMOD_VALID_WIN_PH_ODD(...) RMOD(VALID_WIN_PH_ODD, __VA_ARGS__)
#define RIZS_VALID_WIN_PH_ODD(...) RIZS(VALID_WIN_PH_ODD, __VA_ARGS__)
#define RFLD_VALID_WIN_PH_ODD(fld) RFLD(VALID_WIN_PH_ODD, fld)

typedef union {
	struct {
		unsigned int mbz_0:14; //0x0
		unsigned int act_n_oe:1; //0x1
		unsigned int ddr4_ten_oe:1; //0x1
		unsigned int ddr4_alert0_oe:1; //0x1
		unsigned int ddr4_alert1_oe:1; //0x1
		unsigned int ddr4_cke_oe:1; //0x1
		unsigned int ddr4_bg0_oe:1; //0x1
		unsigned int parity_oe:1; //0x1
		unsigned int parity_1_oe:1; //0x1
		unsigned int adr16_oe:1; //0x1
		unsigned int ddr4_cs_n_oe:1; //0x1
		unsigned int mbz_1:4; //0x0
		unsigned int odt_1_oe:1; //0x1
		unsigned int cke_1_oe:1; //0x1
		unsigned int cs_n_1_oe:1; //0x1
		unsigned int cs_n_oe:1; //0x1
	} f;
	unsigned int v;
} PAD_OE_CTL_EX_T;
#define PAD_OE_CTL_EXar (0xB8142528)
#define PAD_OE_CTL_EXdv (0x0003FF0F)
#define PAD_OE_CTL_EXrv RVAL(PAD_OE_CTL_EX)
#define RMOD_PAD_OE_CTL_EX(...) RMOD(PAD_OE_CTL_EX, __VA_ARGS__)
#define RIZS_PAD_OE_CTL_EX(...) RIZS(PAD_OE_CTL_EX, __VA_ARGS__)
#define RFLD_PAD_OE_CTL_EX(fld) RFLD(PAD_OE_CTL_EX, fld)

typedef union {
	struct {
		unsigned int mbz_0:8; //0x0
		unsigned int odt_force_sel:1; //0x0
		unsigned int odt_force_sig:1; //0x0
		unsigned int mbz_1:18; //0x0
		unsigned int tm_odt_en_odd:1; //0x1
		unsigned int tm_odt_en_sel:2; //0x0
		unsigned int tm_odt_en_dly:1; //0x0
	} f;
	unsigned int v;
} READ_CTRL_2_0_T;
#define READ_CTRL_2_0ar (0xB814252C)
#define READ_CTRL_2_0dv (0x00000008)
#define READ_CTRL_2_0rv RVAL(READ_CTRL_2_0)
#define RMOD_READ_CTRL_2_0(...) RMOD(READ_CTRL_2_0, __VA_ARGS__)
#define RIZS_READ_CTRL_2_0(...) RIZS(READ_CTRL_2_0, __VA_ARGS__)
#define RFLD_READ_CTRL_2_0(fld) RFLD(READ_CTRL_2_0, fld)

typedef union {
	struct {
		unsigned int mbz_0:8; //0x0
		unsigned int odt_force_sel:1; //0x0
		unsigned int odt_force_sig:1; //0x0
		unsigned int mbz_1:18; //0x0
		unsigned int tm_odt_en_odd:1; //0x1
		unsigned int tm_odt_en_sel:2; //0x0
		unsigned int tm_odt_en_dly:1; //0x0
	} f;
	unsigned int v;
} READ_CTRL_2_1_T;
#define READ_CTRL_2_1ar (0xB8142530)
#define READ_CTRL_2_1dv (0x00000008)
#define READ_CTRL_2_1rv RVAL(READ_CTRL_2_1)
#define RMOD_READ_CTRL_2_1(...) RMOD(READ_CTRL_2_1, __VA_ARGS__)
#define RIZS_READ_CTRL_2_1(...) RIZS(READ_CTRL_2_1, __VA_ARGS__)
#define RFLD_READ_CTRL_2_1(fld) RFLD(READ_CTRL_2_1, fld)

typedef union {
	struct {
		unsigned int mbz_0:8; //0x0
		unsigned int odt_force_sel:1; //0x0
		unsigned int odt_force_sig:1; //0x0
		unsigned int mbz_1:18; //0x0
		unsigned int tm_odt_en_odd:1; //0x1
		unsigned int tm_odt_en_sel:2; //0x0
		unsigned int tm_odt_en_dly:1; //0x0
	} f;
	unsigned int v;
} READ_CTRL_2_2_T;
#define READ_CTRL_2_2ar (0xB8142534)
#define READ_CTRL_2_2dv (0x00000008)
#define READ_CTRL_2_2rv RVAL(READ_CTRL_2_2)
#define RMOD_READ_CTRL_2_2(...) RMOD(READ_CTRL_2_2, __VA_ARGS__)
#define RIZS_READ_CTRL_2_2(...) RIZS(READ_CTRL_2_2, __VA_ARGS__)
#define RFLD_READ_CTRL_2_2(fld) RFLD(READ_CTRL_2_2, fld)

typedef union {
	struct {
		unsigned int mbz_0:8; //0x0
		unsigned int odt_force_sel:1; //0x0
		unsigned int odt_force_sig:1; //0x0
		unsigned int mbz_1:18; //0x0
		unsigned int tm_odt_en_odd:1; //0x1
		unsigned int tm_odt_en_sel:2; //0x0
		unsigned int tm_odt_en_dly:1; //0x0
	} f;
	unsigned int v;
} READ_CTRL_2_3_T;
#define READ_CTRL_2_3ar (0xB8142538)
#define READ_CTRL_2_3dv (0x00000008)
#define READ_CTRL_2_3rv RVAL(READ_CTRL_2_3)
#define RMOD_READ_CTRL_2_3(...) RMOD(READ_CTRL_2_3, __VA_ARGS__)
#define RIZS_READ_CTRL_2_3(...) RIZS(READ_CTRL_2_3, __VA_ARGS__)
#define RFLD_READ_CTRL_2_3(fld) RFLD(READ_CTRL_2_3, fld)

typedef union {
	struct {
		unsigned int vref_shift_pos0:32; //0x0
	} f;
	unsigned int v;
} VREF_DBG_0_0_T;
#define VREF_DBG_0_0ar (0xB8142540)
#define VREF_DBG_0_0dv (0x00000000)
#define VREF_DBG_0_0rv RVAL(VREF_DBG_0_0)
#define RMOD_VREF_DBG_0_0(...) RMOD(VREF_DBG_0_0, __VA_ARGS__)
#define RIZS_VREF_DBG_0_0(...) RIZS(VREF_DBG_0_0, __VA_ARGS__)
#define RFLD_VREF_DBG_0_0(fld) RFLD(VREF_DBG_0_0, fld)

typedef union {
	struct {
		unsigned int vref_shift_pos0:32; //0x0
	} f;
	unsigned int v;
} VREF_DBG_0_1_T;
#define VREF_DBG_0_1ar (0xB8142544)
#define VREF_DBG_0_1dv (0x00000000)
#define VREF_DBG_0_1rv RVAL(VREF_DBG_0_1)
#define RMOD_VREF_DBG_0_1(...) RMOD(VREF_DBG_0_1, __VA_ARGS__)
#define RIZS_VREF_DBG_0_1(...) RIZS(VREF_DBG_0_1, __VA_ARGS__)
#define RFLD_VREF_DBG_0_1(fld) RFLD(VREF_DBG_0_1, fld)

typedef union {
	struct {
		unsigned int vref_shift_pos0:32; //0x0
	} f;
	unsigned int v;
} VREF_DBG_0_2_T;
#define VREF_DBG_0_2ar (0xB8142548)
#define VREF_DBG_0_2dv (0x00000000)
#define VREF_DBG_0_2rv RVAL(VREF_DBG_0_2)
#define RMOD_VREF_DBG_0_2(...) RMOD(VREF_DBG_0_2, __VA_ARGS__)
#define RIZS_VREF_DBG_0_2(...) RIZS(VREF_DBG_0_2, __VA_ARGS__)
#define RFLD_VREF_DBG_0_2(fld) RFLD(VREF_DBG_0_2, fld)

typedef union {
	struct {
		unsigned int vref_shift_pos0:32; //0x0
	} f;
	unsigned int v;
} VREF_DBG_0_3_T;
#define VREF_DBG_0_3ar (0xB814254C)
#define VREF_DBG_0_3dv (0x00000000)
#define VREF_DBG_0_3rv RVAL(VREF_DBG_0_3)
#define RMOD_VREF_DBG_0_3(...) RMOD(VREF_DBG_0_3, __VA_ARGS__)
#define RIZS_VREF_DBG_0_3(...) RIZS(VREF_DBG_0_3, __VA_ARGS__)
#define RFLD_VREF_DBG_0_3(fld) RFLD(VREF_DBG_0_3, fld)

typedef union {
	struct {
		unsigned int vref_shift_pos1:32; //0x0
	} f;
	unsigned int v;
} VREF_DBG_1_0_T;
#define VREF_DBG_1_0ar (0xB8142550)
#define VREF_DBG_1_0dv (0x00000000)
#define VREF_DBG_1_0rv RVAL(VREF_DBG_1_0)
#define RMOD_VREF_DBG_1_0(...) RMOD(VREF_DBG_1_0, __VA_ARGS__)
#define RIZS_VREF_DBG_1_0(...) RIZS(VREF_DBG_1_0, __VA_ARGS__)
#define RFLD_VREF_DBG_1_0(fld) RFLD(VREF_DBG_1_0, fld)

typedef union {
	struct {
		unsigned int vref_shift_pos1:32; //0x0
	} f;
	unsigned int v;
} VREF_DBG_1_1_T;
#define VREF_DBG_1_1ar (0xB8142554)
#define VREF_DBG_1_1dv (0x00000000)
#define VREF_DBG_1_1rv RVAL(VREF_DBG_1_1)
#define RMOD_VREF_DBG_1_1(...) RMOD(VREF_DBG_1_1, __VA_ARGS__)
#define RIZS_VREF_DBG_1_1(...) RIZS(VREF_DBG_1_1, __VA_ARGS__)
#define RFLD_VREF_DBG_1_1(fld) RFLD(VREF_DBG_1_1, fld)

typedef union {
	struct {
		unsigned int vref_shift_pos1:32; //0x0
	} f;
	unsigned int v;
} VREF_DBG_1_2_T;
#define VREF_DBG_1_2ar (0xB8142558)
#define VREF_DBG_1_2dv (0x00000000)
#define VREF_DBG_1_2rv RVAL(VREF_DBG_1_2)
#define RMOD_VREF_DBG_1_2(...) RMOD(VREF_DBG_1_2, __VA_ARGS__)
#define RIZS_VREF_DBG_1_2(...) RIZS(VREF_DBG_1_2, __VA_ARGS__)
#define RFLD_VREF_DBG_1_2(fld) RFLD(VREF_DBG_1_2, fld)

typedef union {
	struct {
		unsigned int vref_shift_pos1:32; //0x0
	} f;
	unsigned int v;
} VREF_DBG_1_3_T;
#define VREF_DBG_1_3ar (0xB814255C)
#define VREF_DBG_1_3dv (0x00000000)
#define VREF_DBG_1_3rv RVAL(VREF_DBG_1_3)
#define RMOD_VREF_DBG_1_3(...) RMOD(VREF_DBG_1_3, __VA_ARGS__)
#define RIZS_VREF_DBG_1_3(...) RIZS(VREF_DBG_1_3, __VA_ARGS__)
#define RFLD_VREF_DBG_1_3(fld) RFLD(VREF_DBG_1_3, fld)

typedef union {
	struct {
		unsigned int vref_shift_neg0:32; //0x0
	} f;
	unsigned int v;
} VREF_DBG_2_0_T;
#define VREF_DBG_2_0ar (0xB8142560)
#define VREF_DBG_2_0dv (0x00000000)
#define VREF_DBG_2_0rv RVAL(VREF_DBG_2_0)
#define RMOD_VREF_DBG_2_0(...) RMOD(VREF_DBG_2_0, __VA_ARGS__)
#define RIZS_VREF_DBG_2_0(...) RIZS(VREF_DBG_2_0, __VA_ARGS__)
#define RFLD_VREF_DBG_2_0(fld) RFLD(VREF_DBG_2_0, fld)

typedef union {
	struct {
		unsigned int vref_shift_neg0:32; //0x0
	} f;
	unsigned int v;
} VREF_DBG_2_1_T;
#define VREF_DBG_2_1ar (0xB8142564)
#define VREF_DBG_2_1dv (0x00000000)
#define VREF_DBG_2_1rv RVAL(VREF_DBG_2_1)
#define RMOD_VREF_DBG_2_1(...) RMOD(VREF_DBG_2_1, __VA_ARGS__)
#define RIZS_VREF_DBG_2_1(...) RIZS(VREF_DBG_2_1, __VA_ARGS__)
#define RFLD_VREF_DBG_2_1(fld) RFLD(VREF_DBG_2_1, fld)

typedef union {
	struct {
		unsigned int vref_shift_neg0:32; //0x0
	} f;
	unsigned int v;
} VREF_DBG_2_2_T;
#define VREF_DBG_2_2ar (0xB8142568)
#define VREF_DBG_2_2dv (0x00000000)
#define VREF_DBG_2_2rv RVAL(VREF_DBG_2_2)
#define RMOD_VREF_DBG_2_2(...) RMOD(VREF_DBG_2_2, __VA_ARGS__)
#define RIZS_VREF_DBG_2_2(...) RIZS(VREF_DBG_2_2, __VA_ARGS__)
#define RFLD_VREF_DBG_2_2(fld) RFLD(VREF_DBG_2_2, fld)

typedef union {
	struct {
		unsigned int vref_shift_neg0:32; //0x0
	} f;
	unsigned int v;
} VREF_DBG_2_3_T;
#define VREF_DBG_2_3ar (0xB814256C)
#define VREF_DBG_2_3dv (0x00000000)
#define VREF_DBG_2_3rv RVAL(VREF_DBG_2_3)
#define RMOD_VREF_DBG_2_3(...) RMOD(VREF_DBG_2_3, __VA_ARGS__)
#define RIZS_VREF_DBG_2_3(...) RIZS(VREF_DBG_2_3, __VA_ARGS__)
#define RFLD_VREF_DBG_2_3(fld) RFLD(VREF_DBG_2_3, fld)

typedef union {
	struct {
		unsigned int vref_shift_neg1:32; //0x0
	} f;
	unsigned int v;
} VREF_DBG_3_0_T;
#define VREF_DBG_3_0ar (0xB8142570)
#define VREF_DBG_3_0dv (0x00000000)
#define VREF_DBG_3_0rv RVAL(VREF_DBG_3_0)
#define RMOD_VREF_DBG_3_0(...) RMOD(VREF_DBG_3_0, __VA_ARGS__)
#define RIZS_VREF_DBG_3_0(...) RIZS(VREF_DBG_3_0, __VA_ARGS__)
#define RFLD_VREF_DBG_3_0(fld) RFLD(VREF_DBG_3_0, fld)

typedef union {
	struct {
		unsigned int vref_shift_neg1:32; //0x0
	} f;
	unsigned int v;
} VREF_DBG_3_1_T;
#define VREF_DBG_3_1ar (0xB8142574)
#define VREF_DBG_3_1dv (0x00000000)
#define VREF_DBG_3_1rv RVAL(VREF_DBG_3_1)
#define RMOD_VREF_DBG_3_1(...) RMOD(VREF_DBG_3_1, __VA_ARGS__)
#define RIZS_VREF_DBG_3_1(...) RIZS(VREF_DBG_3_1, __VA_ARGS__)
#define RFLD_VREF_DBG_3_1(fld) RFLD(VREF_DBG_3_1, fld)

typedef union {
	struct {
		unsigned int vref_shift_neg1:32; //0x0
	} f;
	unsigned int v;
} VREF_DBG_3_2_T;
#define VREF_DBG_3_2ar (0xB8142578)
#define VREF_DBG_3_2dv (0x00000000)
#define VREF_DBG_3_2rv RVAL(VREF_DBG_3_2)
#define RMOD_VREF_DBG_3_2(...) RMOD(VREF_DBG_3_2, __VA_ARGS__)
#define RIZS_VREF_DBG_3_2(...) RIZS(VREF_DBG_3_2, __VA_ARGS__)
#define RFLD_VREF_DBG_3_2(fld) RFLD(VREF_DBG_3_2, fld)

typedef union {
	struct {
		unsigned int vref_shift_neg1:32; //0x0
	} f;
	unsigned int v;
} VREF_DBG_3_3_T;
#define VREF_DBG_3_3ar (0xB814257C)
#define VREF_DBG_3_3dv (0x00000000)
#define VREF_DBG_3_3rv RVAL(VREF_DBG_3_3)
#define RMOD_VREF_DBG_3_3(...) RMOD(VREF_DBG_3_3, __VA_ARGS__)
#define RIZS_VREF_DBG_3_3(...) RIZS(VREF_DBG_3_3, __VA_ARGS__)
#define RFLD_VREF_DBG_3_3(fld) RFLD(VREF_DBG_3_3, fld)

typedef union {
	struct {
		unsigned int mbz_0:9; //0x0
		unsigned int vref_shift_det:7; //0x0
		unsigned int vref_shift_dbi:16; //0x0
	} f;
	unsigned int v;
} VREF_DBG_4_0_T;
#define VREF_DBG_4_0ar (0xB8142580)
#define VREF_DBG_4_0dv (0x00000000)
#define VREF_DBG_4_0rv RVAL(VREF_DBG_4_0)
#define RMOD_VREF_DBG_4_0(...) RMOD(VREF_DBG_4_0, __VA_ARGS__)
#define RIZS_VREF_DBG_4_0(...) RIZS(VREF_DBG_4_0, __VA_ARGS__)
#define RFLD_VREF_DBG_4_0(fld) RFLD(VREF_DBG_4_0, fld)

typedef union {
	struct {
		unsigned int mbz_0:9; //0x0
		unsigned int vref_shift_det:7; //0x0
		unsigned int vref_shift_dbi:16; //0x0
	} f;
	unsigned int v;
} VREF_DBG_4_1_T;
#define VREF_DBG_4_1ar (0xB8142584)
#define VREF_DBG_4_1dv (0x00000000)
#define VREF_DBG_4_1rv RVAL(VREF_DBG_4_1)
#define RMOD_VREF_DBG_4_1(...) RMOD(VREF_DBG_4_1, __VA_ARGS__)
#define RIZS_VREF_DBG_4_1(...) RIZS(VREF_DBG_4_1, __VA_ARGS__)
#define RFLD_VREF_DBG_4_1(fld) RFLD(VREF_DBG_4_1, fld)

typedef union {
	struct {
		unsigned int mbz_0:9; //0x0
		unsigned int vref_shift_det:7; //0x0
		unsigned int vref_shift_dbi:16; //0x0
	} f;
	unsigned int v;
} VREF_DBG_4_2_T;
#define VREF_DBG_4_2ar (0xB8142588)
#define VREF_DBG_4_2dv (0x00000000)
#define VREF_DBG_4_2rv RVAL(VREF_DBG_4_2)
#define RMOD_VREF_DBG_4_2(...) RMOD(VREF_DBG_4_2, __VA_ARGS__)
#define RIZS_VREF_DBG_4_2(...) RIZS(VREF_DBG_4_2, __VA_ARGS__)
#define RFLD_VREF_DBG_4_2(fld) RFLD(VREF_DBG_4_2, fld)

typedef union {
	struct {
		unsigned int mbz_0:9; //0x0
		unsigned int vref_shift_det:7; //0x0
		unsigned int vref_shift_dbi:16; //0x0
	} f;
	unsigned int v;
} VREF_DBG_4_3_T;
#define VREF_DBG_4_3ar (0xB814258C)
#define VREF_DBG_4_3dv (0x00000000)
#define VREF_DBG_4_3rv RVAL(VREF_DBG_4_3)
#define RMOD_VREF_DBG_4_3(...) RMOD(VREF_DBG_4_3, __VA_ARGS__)
#define RIZS_VREF_DBG_4_3(...) RIZS(VREF_DBG_4_3, __VA_ARGS__)
#define RFLD_VREF_DBG_4_3(fld) RFLD(VREF_DBG_4_3, fld)

typedef union {
	struct {
		unsigned int mbz_0:9; //0x0
		unsigned int zq_vref_sel_rzq_dn:1; //0x1
		unsigned int zq_vref_s_rzq_dn:6; //0xA
		unsigned int mbz_1:1; //0x0
		unsigned int zq_vref_range_rzq_dn:1; //0x1
		unsigned int mbz_2:1; //0x0
		unsigned int zq_pd_rzq_str:1; //0x0
		unsigned int mbz_3:1; //0x0
		unsigned int zq_vref_sel_ini:1; //0x0
		unsigned int zq_vref_s_ini:6; //0xA
		unsigned int mbz_4:1; //0x0
		unsigned int zq_vref_range_ini:1; //0x1
		unsigned int mbz_5:1; //0x0
		unsigned int zq_pd_ini:1; //0x0
	} f;
	unsigned int v;
} ZQ_PAD_CTRL_T;
#define ZQ_PAD_CTRLar (0xB8142590)
#define ZQ_PAD_CTRLdv (0x004A40A4)
#define ZQ_PAD_CTRLrv RVAL(ZQ_PAD_CTRL)
#define RMOD_ZQ_PAD_CTRL(...) RMOD(ZQ_PAD_CTRL, __VA_ARGS__)
#define RIZS_ZQ_PAD_CTRL(...) RIZS(ZQ_PAD_CTRL, __VA_ARGS__)
#define RFLD_ZQ_PAD_CTRL(fld) RFLD(ZQ_PAD_CTRL, fld)

typedef union {
	struct {
		unsigned int mbz_0:24; //0x0
		unsigned int zq_vref_s_rzq_dn2:6; //0x28
		unsigned int mbz_1:1; //0x0
		unsigned int zq_vref_range_rzq_dn2:1; //0x1
	} f;
	unsigned int v;
} ZQ_PAD_CTRL_2_T;
#define ZQ_PAD_CTRL_2ar (0xB8142594)
#define ZQ_PAD_CTRL_2dv (0x000000A1)
#define ZQ_PAD_CTRL_2rv RVAL(ZQ_PAD_CTRL_2)
#define RMOD_ZQ_PAD_CTRL_2(...) RMOD(ZQ_PAD_CTRL_2, __VA_ARGS__)
#define RIZS_ZQ_PAD_CTRL_2(...) RIZS(ZQ_PAD_CTRL_2, __VA_ARGS__)
#define RFLD_ZQ_PAD_CTRL_2(fld) RFLD(ZQ_PAD_CTRL_2, fld)

typedef union {
	struct {
		unsigned int mbz_0:5; //0x0
		unsigned int zprog:27; //0x0
	} f;
	unsigned int v;
} PAD_CTRL_ZPROG_0_T;
#define PAD_CTRL_ZPROG_0ar (0xB8142598)
#define PAD_CTRL_ZPROG_0dv (0x00000000)
#define PAD_CTRL_ZPROG_0rv RVAL(PAD_CTRL_ZPROG_0)
#define RMOD_PAD_CTRL_ZPROG_0(...) RMOD(PAD_CTRL_ZPROG_0, __VA_ARGS__)
#define RIZS_PAD_CTRL_ZPROG_0(...) RIZS(PAD_CTRL_ZPROG_0, __VA_ARGS__)
#define RFLD_PAD_CTRL_ZPROG_0(fld) RFLD(PAD_CTRL_ZPROG_0, fld)

typedef union {
	struct {
		unsigned int mbz_0:5; //0x0
		unsigned int zprog:27; //0x0
	} f;
	unsigned int v;
} PAD_CTRL_ZPROG_1_T;
#define PAD_CTRL_ZPROG_1ar (0xB814259C)
#define PAD_CTRL_ZPROG_1dv (0x00000000)
#define PAD_CTRL_ZPROG_1rv RVAL(PAD_CTRL_ZPROG_1)
#define RMOD_PAD_CTRL_ZPROG_1(...) RMOD(PAD_CTRL_ZPROG_1, __VA_ARGS__)
#define RIZS_PAD_CTRL_ZPROG_1(...) RIZS(PAD_CTRL_ZPROG_1, __VA_ARGS__)
#define RFLD_PAD_CTRL_ZPROG_1(fld) RFLD(PAD_CTRL_ZPROG_1, fld)

typedef union {
	struct {
		unsigned int mbz_0:5; //0x0
		unsigned int zprog:27; //0x0
	} f;
	unsigned int v;
} PAD_CTRL_ZPROG_2_T;
#define PAD_CTRL_ZPROG_2ar (0xB81425A0)
#define PAD_CTRL_ZPROG_2dv (0x00000000)
#define PAD_CTRL_ZPROG_2rv RVAL(PAD_CTRL_ZPROG_2)
#define RMOD_PAD_CTRL_ZPROG_2(...) RMOD(PAD_CTRL_ZPROG_2, __VA_ARGS__)
#define RIZS_PAD_CTRL_ZPROG_2(...) RIZS(PAD_CTRL_ZPROG_2, __VA_ARGS__)
#define RFLD_PAD_CTRL_ZPROG_2(fld) RFLD(PAD_CTRL_ZPROG_2, fld)

typedef union {
	struct {
		unsigned int mbz_0:5; //0x0
		unsigned int zprog:27; //0x0
	} f;
	unsigned int v;
} PAD_CTRL_ZPROG_3_T;
#define PAD_CTRL_ZPROG_3ar (0xB81425A4)
#define PAD_CTRL_ZPROG_3dv (0x00000000)
#define PAD_CTRL_ZPROG_3rv RVAL(PAD_CTRL_ZPROG_3)
#define RMOD_PAD_CTRL_ZPROG_3(...) RMOD(PAD_CTRL_ZPROG_3, __VA_ARGS__)
#define RIZS_PAD_CTRL_ZPROG_3(...) RIZS(PAD_CTRL_ZPROG_3, __VA_ARGS__)
#define RFLD_PAD_CTRL_ZPROG_3(fld) RFLD(PAD_CTRL_ZPROG_3, fld)

typedef union {
	struct {
		unsigned int mbz_0:5; //0x0
		unsigned int zprog:27; //0x0
	} f;
	unsigned int v;
} PAD_CTRL_ZPROG_4_T;
#define PAD_CTRL_ZPROG_4ar (0xB81425A8)
#define PAD_CTRL_ZPROG_4dv (0x00000000)
#define PAD_CTRL_ZPROG_4rv RVAL(PAD_CTRL_ZPROG_4)
#define RMOD_PAD_CTRL_ZPROG_4(...) RMOD(PAD_CTRL_ZPROG_4, __VA_ARGS__)
#define RIZS_PAD_CTRL_ZPROG_4(...) RIZS(PAD_CTRL_ZPROG_4, __VA_ARGS__)
#define RFLD_PAD_CTRL_ZPROG_4(fld) RFLD(PAD_CTRL_ZPROG_4, fld)

typedef union {
	struct {
		unsigned int mbz_0:5; //0x0
		unsigned int zprog:27; //0x0
	} f;
	unsigned int v;
} PAD_CTRL_ZPROG_5_T;
#define PAD_CTRL_ZPROG_5ar (0xB81425AC)
#define PAD_CTRL_ZPROG_5dv (0x00000000)
#define PAD_CTRL_ZPROG_5rv RVAL(PAD_CTRL_ZPROG_5)
#define RMOD_PAD_CTRL_ZPROG_5(...) RMOD(PAD_CTRL_ZPROG_5, __VA_ARGS__)
#define RIZS_PAD_CTRL_ZPROG_5(...) RIZS(PAD_CTRL_ZPROG_5, __VA_ARGS__)
#define RFLD_PAD_CTRL_ZPROG_5(fld) RFLD(PAD_CTRL_ZPROG_5, fld)

typedef union {
	struct {
		unsigned int mbz_0:5; //0x0
		unsigned int zprog:27; //0x0
	} f;
	unsigned int v;
} PAD_CTRL_ZPROG_6_T;
#define PAD_CTRL_ZPROG_6ar (0xB81425B0)
#define PAD_CTRL_ZPROG_6dv (0x00000000)
#define PAD_CTRL_ZPROG_6rv RVAL(PAD_CTRL_ZPROG_6)
#define RMOD_PAD_CTRL_ZPROG_6(...) RMOD(PAD_CTRL_ZPROG_6, __VA_ARGS__)
#define RIZS_PAD_CTRL_ZPROG_6(...) RIZS(PAD_CTRL_ZPROG_6, __VA_ARGS__)
#define RFLD_PAD_CTRL_ZPROG_6(fld) RFLD(PAD_CTRL_ZPROG_6, fld)

typedef union {
	struct {
		unsigned int mbz_0:5; //0x0
		unsigned int zprog:27; //0x0
	} f;
	unsigned int v;
} PAD_CTRL_ZPROG_7_T;
#define PAD_CTRL_ZPROG_7ar (0xB81425B4)
#define PAD_CTRL_ZPROG_7dv (0x00000000)
#define PAD_CTRL_ZPROG_7rv RVAL(PAD_CTRL_ZPROG_7)
#define RMOD_PAD_CTRL_ZPROG_7(...) RMOD(PAD_CTRL_ZPROG_7, __VA_ARGS__)
#define RIZS_PAD_CTRL_ZPROG_7(...) RIZS(PAD_CTRL_ZPROG_7, __VA_ARGS__)
#define RFLD_PAD_CTRL_ZPROG_7(fld) RFLD(PAD_CTRL_ZPROG_7, fld)

typedef union {
	struct {
		unsigned int mbz_0:24; //0x0
		unsigned int zq_ena_nocd2_7:1; //0x0
		unsigned int zq_ena_nocd2_6:1; //0x0
		unsigned int zq_ena_nocd2_5:1; //0x0
		unsigned int zq_ena_nocd2_4:1; //0x0
		unsigned int zq_ena_nocd2_3:1; //0x0
		unsigned int zq_ena_nocd2_2:1; //0x0
		unsigned int zq_ena_nocd2_1:1; //0x0
		unsigned int zq_ena_nocd2_0:1; //0x0
	} f;
	unsigned int v;
} ZQ_NOCD2_T;
#define ZQ_NOCD2ar (0xB81425B8)
#define ZQ_NOCD2dv (0x00000000)
#define ZQ_NOCD2rv RVAL(ZQ_NOCD2)
#define RMOD_ZQ_NOCD2(...) RMOD(ZQ_NOCD2, __VA_ARGS__)
#define RIZS_ZQ_NOCD2(...) RIZS(ZQ_NOCD2, __VA_ARGS__)
#define RFLD_ZQ_NOCD2(fld) RFLD(ZQ_NOCD2, fld)

typedef union {
	struct {
		unsigned int mbz_0:17; //0x0
		unsigned int dq_1_ocdn_sel:3; //0x0
		unsigned int mbz_1:1; //0x0
		unsigned int dq_0_ocdn_sel:3; //0x0
		unsigned int mbz_2:1; //0x0
		unsigned int dq_1_ocdp_sel:3; //0x0
		unsigned int mbz_3:1; //0x0
		unsigned int dq_0_ocdp_sel:3; //0x0
	} f;
	unsigned int v;
} DQ_OCD_SEL_0_T;
#define DQ_OCD_SEL_0ar (0xB81425C0)
#define DQ_OCD_SEL_0dv (0x00000000)
#define DQ_OCD_SEL_0rv RVAL(DQ_OCD_SEL_0)
#define RMOD_DQ_OCD_SEL_0(...) RMOD(DQ_OCD_SEL_0, __VA_ARGS__)
#define RIZS_DQ_OCD_SEL_0(...) RIZS(DQ_OCD_SEL_0, __VA_ARGS__)
#define RFLD_DQ_OCD_SEL_0(fld) RFLD(DQ_OCD_SEL_0, fld)

typedef union {
	struct {
		unsigned int mbz_0:17; //0x0
		unsigned int dq_1_ocdn_sel:3; //0x0
		unsigned int mbz_1:1; //0x0
		unsigned int dq_0_ocdn_sel:3; //0x0
		unsigned int mbz_2:1; //0x0
		unsigned int dq_1_ocdp_sel:3; //0x0
		unsigned int mbz_3:1; //0x0
		unsigned int dq_0_ocdp_sel:3; //0x0
	} f;
	unsigned int v;
} DQ_OCD_SEL_1_T;
#define DQ_OCD_SEL_1ar (0xB81425C4)
#define DQ_OCD_SEL_1dv (0x00000000)
#define DQ_OCD_SEL_1rv RVAL(DQ_OCD_SEL_1)
#define RMOD_DQ_OCD_SEL_1(...) RMOD(DQ_OCD_SEL_1, __VA_ARGS__)
#define RIZS_DQ_OCD_SEL_1(...) RIZS(DQ_OCD_SEL_1, __VA_ARGS__)
#define RFLD_DQ_OCD_SEL_1(fld) RFLD(DQ_OCD_SEL_1, fld)

typedef union {
	struct {
		unsigned int mbz_0:1; //0x0
		unsigned int dqs_p_1_ttfn_sel:3; //0x0
		unsigned int mbz_1:1; //0x0
		unsigned int dqs_p_0_ttfn_sel:3; //0x0
		unsigned int mbz_2:1; //0x0
		unsigned int dqs_p_1_ttfp_sel:3; //0x0
		unsigned int mbz_3:1; //0x0
		unsigned int dqs_p_0_ttfp_sel:3; //0x0
		unsigned int mbz_4:1; //0x0
		unsigned int dqs_p_1_ttcn_sel:3; //0x0
		unsigned int mbz_5:1; //0x0
		unsigned int dqs_p_0_ttcn_sel:3; //0x0
		unsigned int mbz_6:1; //0x0
		unsigned int dqs_p_1_ttcp_sel:3; //0x0
		unsigned int mbz_7:1; //0x0
		unsigned int dqs_p_0_ttcp_sel:3; //0x0
	} f;
	unsigned int v;
} DQS_P_ODT_SEL_0_T;
#define DQS_P_ODT_SEL_0ar (0xB81425C8)
#define DQS_P_ODT_SEL_0dv (0x00000000)
#define DQS_P_ODT_SEL_0rv RVAL(DQS_P_ODT_SEL_0)
#define RMOD_DQS_P_ODT_SEL_0(...) RMOD(DQS_P_ODT_SEL_0, __VA_ARGS__)
#define RIZS_DQS_P_ODT_SEL_0(...) RIZS(DQS_P_ODT_SEL_0, __VA_ARGS__)
#define RFLD_DQS_P_ODT_SEL_0(fld) RFLD(DQS_P_ODT_SEL_0, fld)

typedef union {
	struct {
		unsigned int mbz_0:1; //0x0
		unsigned int dqs_p_1_ttfn_sel:3; //0x0
		unsigned int mbz_1:1; //0x0
		unsigned int dqs_p_0_ttfn_sel:3; //0x0
		unsigned int mbz_2:1; //0x0
		unsigned int dqs_p_1_ttfp_sel:3; //0x0
		unsigned int mbz_3:1; //0x0
		unsigned int dqs_p_0_ttfp_sel:3; //0x0
		unsigned int mbz_4:1; //0x0
		unsigned int dqs_p_1_ttcn_sel:3; //0x0
		unsigned int mbz_5:1; //0x0
		unsigned int dqs_p_0_ttcn_sel:3; //0x0
		unsigned int mbz_6:1; //0x0
		unsigned int dqs_p_1_ttcp_sel:3; //0x0
		unsigned int mbz_7:1; //0x0
		unsigned int dqs_p_0_ttcp_sel:3; //0x0
	} f;
	unsigned int v;
} DQS_P_ODT_SEL_1_T;
#define DQS_P_ODT_SEL_1ar (0xB81425CC)
#define DQS_P_ODT_SEL_1dv (0x00000000)
#define DQS_P_ODT_SEL_1rv RVAL(DQS_P_ODT_SEL_1)
#define RMOD_DQS_P_ODT_SEL_1(...) RMOD(DQS_P_ODT_SEL_1, __VA_ARGS__)
#define RIZS_DQS_P_ODT_SEL_1(...) RIZS(DQS_P_ODT_SEL_1, __VA_ARGS__)
#define RFLD_DQS_P_ODT_SEL_1(fld) RFLD(DQS_P_ODT_SEL_1, fld)

typedef union {
	struct {
		unsigned int mbz_0:1; //0x0
		unsigned int dqs_n_1_ttfn_sel:3; //0x0
		unsigned int mbz_1:1; //0x0
		unsigned int dqs_n_0_ttfn_sel:3; //0x0
		unsigned int mbz_2:1; //0x0
		unsigned int dqs_n_1_ttfp_sel:3; //0x0
		unsigned int mbz_3:1; //0x0
		unsigned int dqs_n_0_ttfp_sel:3; //0x0
		unsigned int mbz_4:1; //0x0
		unsigned int dqs_n_1_ttcn_sel:3; //0x0
		unsigned int mbz_5:1; //0x0
		unsigned int dqs_n_0_ttcn_sel:3; //0x0
		unsigned int mbz_6:1; //0x0
		unsigned int dqs_n_1_ttcp_sel:3; //0x0
		unsigned int mbz_7:1; //0x0
		unsigned int dqs_n_0_ttcp_sel:3; //0x0
	} f;
	unsigned int v;
} DQS_N_ODT_SEL_0_T;
#define DQS_N_ODT_SEL_0ar (0xB81425D0)
#define DQS_N_ODT_SEL_0dv (0x00000000)
#define DQS_N_ODT_SEL_0rv RVAL(DQS_N_ODT_SEL_0)
#define RMOD_DQS_N_ODT_SEL_0(...) RMOD(DQS_N_ODT_SEL_0, __VA_ARGS__)
#define RIZS_DQS_N_ODT_SEL_0(...) RIZS(DQS_N_ODT_SEL_0, __VA_ARGS__)
#define RFLD_DQS_N_ODT_SEL_0(fld) RFLD(DQS_N_ODT_SEL_0, fld)

typedef union {
	struct {
		unsigned int mbz_0:1; //0x0
		unsigned int dqs_n_1_ttfn_sel:3; //0x0
		unsigned int mbz_1:1; //0x0
		unsigned int dqs_n_0_ttfn_sel:3; //0x0
		unsigned int mbz_2:1; //0x0
		unsigned int dqs_n_1_ttfp_sel:3; //0x0
		unsigned int mbz_3:1; //0x0
		unsigned int dqs_n_0_ttfp_sel:3; //0x0
		unsigned int mbz_4:1; //0x0
		unsigned int dqs_n_1_ttcn_sel:3; //0x0
		unsigned int mbz_5:1; //0x0
		unsigned int dqs_n_0_ttcn_sel:3; //0x0
		unsigned int mbz_6:1; //0x0
		unsigned int dqs_n_1_ttcp_sel:3; //0x0
		unsigned int mbz_7:1; //0x0
		unsigned int dqs_n_0_ttcp_sel:3; //0x0
	} f;
	unsigned int v;
} DQS_N_ODT_SEL_1_T;
#define DQS_N_ODT_SEL_1ar (0xB81425D4)
#define DQS_N_ODT_SEL_1dv (0x00000000)
#define DQS_N_ODT_SEL_1rv RVAL(DQS_N_ODT_SEL_1)
#define RMOD_DQS_N_ODT_SEL_1(...) RMOD(DQS_N_ODT_SEL_1, __VA_ARGS__)
#define RIZS_DQS_N_ODT_SEL_1(...) RIZS(DQS_N_ODT_SEL_1, __VA_ARGS__)
#define RFLD_DQS_N_ODT_SEL_1(fld) RFLD(DQS_N_ODT_SEL_1, fld)

typedef union {
	struct {
		unsigned int mbz_0:1; //0x0
		unsigned int dqs_n_1_ocdn_sel:3; //0x0
		unsigned int mbz_1:1; //0x0
		unsigned int dqs_n_0_ocdn_sel:3; //0x0
		unsigned int mbz_2:1; //0x0
		unsigned int dqs_n_1_ocdp_sel:3; //0x0
		unsigned int mbz_3:1; //0x0
		unsigned int dqs_n_0_ocdp_sel:3; //0x0
		unsigned int mbz_4:1; //0x0
		unsigned int dqs_p_1_ocdn_sel:3; //0x0
		unsigned int mbz_5:1; //0x0
		unsigned int dqs_p_0_ocdn_sel:3; //0x0
		unsigned int mbz_6:1; //0x0
		unsigned int dqs_p_1_ocdp_sel:3; //0x0
		unsigned int mbz_7:1; //0x0
		unsigned int dqs_p_0_ocdp_sel:3; //0x0
	} f;
	unsigned int v;
} DQS_OCD_SEL_0_T;
#define DQS_OCD_SEL_0ar (0xB81425D8)
#define DQS_OCD_SEL_0dv (0x00000000)
#define DQS_OCD_SEL_0rv RVAL(DQS_OCD_SEL_0)
#define RMOD_DQS_OCD_SEL_0(...) RMOD(DQS_OCD_SEL_0, __VA_ARGS__)
#define RIZS_DQS_OCD_SEL_0(...) RIZS(DQS_OCD_SEL_0, __VA_ARGS__)
#define RFLD_DQS_OCD_SEL_0(fld) RFLD(DQS_OCD_SEL_0, fld)

typedef union {
	struct {
		unsigned int mbz_0:1; //0x0
		unsigned int dqs_n_1_ocdn_sel:3; //0x0
		unsigned int mbz_1:1; //0x0
		unsigned int dqs_n_0_ocdn_sel:3; //0x0
		unsigned int mbz_2:1; //0x0
		unsigned int dqs_n_1_ocdp_sel:3; //0x0
		unsigned int mbz_3:1; //0x0
		unsigned int dqs_n_0_ocdp_sel:3; //0x0
		unsigned int mbz_4:1; //0x0
		unsigned int dqs_p_1_ocdn_sel:3; //0x0
		unsigned int mbz_5:1; //0x0
		unsigned int dqs_p_0_ocdn_sel:3; //0x0
		unsigned int mbz_6:1; //0x0
		unsigned int dqs_p_1_ocdp_sel:3; //0x0
		unsigned int mbz_7:1; //0x0
		unsigned int dqs_p_0_ocdp_sel:3; //0x0
	} f;
	unsigned int v;
} DQS_OCD_SEL_1_T;
#define DQS_OCD_SEL_1ar (0xB81425DC)
#define DQS_OCD_SEL_1dv (0x00000000)
#define DQS_OCD_SEL_1rv RVAL(DQS_OCD_SEL_1)
#define RMOD_DQS_OCD_SEL_1(...) RMOD(DQS_OCD_SEL_1, __VA_ARGS__)
#define RIZS_DQS_OCD_SEL_1(...) RIZS(DQS_OCD_SEL_1, __VA_ARGS__)
#define RFLD_DQS_OCD_SEL_1(fld) RFLD(DQS_OCD_SEL_1, fld)

typedef union {
	struct {
		unsigned int mbz_0:2; //0x0
		unsigned int dqs_slewrate:2; //0x0
		unsigned int mbz_1:1; //0x0
		unsigned int dqs_dly_emph:3; //0x0
		unsigned int mbz_2:1; //0x0
		unsigned int dqs_en_emphn:3; //0x7
		unsigned int mbz_3:1; //0x0
		unsigned int dqs_en_emphp:3; //0x7
		unsigned int mbz_4:2; //0x0
		unsigned int dq_slewrate:2; //0x0
		unsigned int mbz_5:1; //0x0
		unsigned int dq_dly_emph:3; //0x0
		unsigned int mbz_6:1; //0x0
		unsigned int dq_en_emphn:3; //0x7
		unsigned int mbz_7:1; //0x0
		unsigned int dq_en_emphp:3; //0x7
	} f;
	unsigned int v;
} RW_EMPH_CTL_0_T;
#define RW_EMPH_CTL_0ar (0xB81425E0)
#define RW_EMPH_CTL_0dv (0x00770077)
#define RW_EMPH_CTL_0rv RVAL(RW_EMPH_CTL_0)
#define RMOD_RW_EMPH_CTL_0(...) RMOD(RW_EMPH_CTL_0, __VA_ARGS__)
#define RIZS_RW_EMPH_CTL_0(...) RIZS(RW_EMPH_CTL_0, __VA_ARGS__)
#define RFLD_RW_EMPH_CTL_0(fld) RFLD(RW_EMPH_CTL_0, fld)

typedef union {
	struct {
		unsigned int mbz_0:2; //0x0
		unsigned int dqs_slewrate:2; //0x0
		unsigned int mbz_1:1; //0x0
		unsigned int dqs_dly_emph:3; //0x0
		unsigned int mbz_2:1; //0x0
		unsigned int dqs_en_emphn:3; //0x7
		unsigned int mbz_3:1; //0x0
		unsigned int dqs_en_emphp:3; //0x7
		unsigned int mbz_4:2; //0x0
		unsigned int dq_slewrate:2; //0x0
		unsigned int mbz_5:1; //0x0
		unsigned int dq_dly_emph:3; //0x0
		unsigned int mbz_6:1; //0x0
		unsigned int dq_en_emphn:3; //0x7
		unsigned int mbz_7:1; //0x0
		unsigned int dq_en_emphp:3; //0x7
	} f;
	unsigned int v;
} RW_EMPH_CTL_1_T;
#define RW_EMPH_CTL_1ar (0xB81425E4)
#define RW_EMPH_CTL_1dv (0x00770077)
#define RW_EMPH_CTL_1rv RVAL(RW_EMPH_CTL_1)
#define RMOD_RW_EMPH_CTL_1(...) RMOD(RW_EMPH_CTL_1, __VA_ARGS__)
#define RIZS_RW_EMPH_CTL_1(...) RIZS(RW_EMPH_CTL_1, __VA_ARGS__)
#define RFLD_RW_EMPH_CTL_1(fld) RFLD(RW_EMPH_CTL_1, fld)

typedef union {
	struct {
		unsigned int mbz_0:2; //0x0
		unsigned int dqs_slewrate:2; //0x0
		unsigned int mbz_1:1; //0x0
		unsigned int dqs_dly_emph:3; //0x0
		unsigned int mbz_2:1; //0x0
		unsigned int dqs_en_emphn:3; //0x7
		unsigned int mbz_3:1; //0x0
		unsigned int dqs_en_emphp:3; //0x7
		unsigned int mbz_4:2; //0x0
		unsigned int dq_slewrate:2; //0x0
		unsigned int mbz_5:1; //0x0
		unsigned int dq_dly_emph:3; //0x0
		unsigned int mbz_6:1; //0x0
		unsigned int dq_en_emphn:3; //0x7
		unsigned int mbz_7:1; //0x0
		unsigned int dq_en_emphp:3; //0x7
	} f;
	unsigned int v;
} RW_EMPH_CTL_2_T;
#define RW_EMPH_CTL_2ar (0xB81425E8)
#define RW_EMPH_CTL_2dv (0x00770077)
#define RW_EMPH_CTL_2rv RVAL(RW_EMPH_CTL_2)
#define RMOD_RW_EMPH_CTL_2(...) RMOD(RW_EMPH_CTL_2, __VA_ARGS__)
#define RIZS_RW_EMPH_CTL_2(...) RIZS(RW_EMPH_CTL_2, __VA_ARGS__)
#define RFLD_RW_EMPH_CTL_2(fld) RFLD(RW_EMPH_CTL_2, fld)

typedef union {
	struct {
		unsigned int mbz_0:2; //0x0
		unsigned int dqs_slewrate:2; //0x0
		unsigned int mbz_1:1; //0x0
		unsigned int dqs_dly_emph:3; //0x0
		unsigned int mbz_2:1; //0x0
		unsigned int dqs_en_emphn:3; //0x7
		unsigned int mbz_3:1; //0x0
		unsigned int dqs_en_emphp:3; //0x7
		unsigned int mbz_4:2; //0x0
		unsigned int dq_slewrate:2; //0x0
		unsigned int mbz_5:1; //0x0
		unsigned int dq_dly_emph:3; //0x0
		unsigned int mbz_6:1; //0x0
		unsigned int dq_en_emphn:3; //0x7
		unsigned int mbz_7:1; //0x0
		unsigned int dq_en_emphp:3; //0x7
	} f;
	unsigned int v;
} RW_EMPH_CTL_3_T;
#define RW_EMPH_CTL_3ar (0xB81425EC)
#define RW_EMPH_CTL_3dv (0x00770077)
#define RW_EMPH_CTL_3rv RVAL(RW_EMPH_CTL_3)
#define RMOD_RW_EMPH_CTL_3(...) RMOD(RW_EMPH_CTL_3, __VA_ARGS__)
#define RIZS_RW_EMPH_CTL_3(...) RIZS(RW_EMPH_CTL_3, __VA_ARGS__)
#define RFLD_RW_EMPH_CTL_3(fld) RFLD(RW_EMPH_CTL_3, fld)

typedef union {
	struct {
		unsigned int mbz_0:2; //0x0
		unsigned int cs1_slewrate:2; //0x0
		unsigned int mbz_1:1; //0x0
		unsigned int cs1_dly_emph:3; //0x0
		unsigned int mbz_2:1; //0x0
		unsigned int cs1_en_emphn:3; //0x7
		unsigned int mbz_3:1; //0x0
		unsigned int cs1_en_emphp:3; //0x7
		unsigned int mbz_4:2; //0x0
		unsigned int cs_slewrate:2; //0x0
		unsigned int mbz_5:1; //0x0
		unsigned int cs_dly_emph:3; //0x0
		unsigned int mbz_6:1; //0x0
		unsigned int cs_en_emphn:3; //0x7
		unsigned int mbz_7:1; //0x0
		unsigned int cs_en_emphp:3; //0x7
	} f;
	unsigned int v;
} CS_EMPH_CTL_T;
#define CS_EMPH_CTLar (0xB81425F0)
#define CS_EMPH_CTLdv (0x00770077)
#define CS_EMPH_CTLrv RVAL(CS_EMPH_CTL)
#define RMOD_CS_EMPH_CTL(...) RMOD(CS_EMPH_CTL, __VA_ARGS__)
#define RIZS_CS_EMPH_CTL(...) RIZS(CS_EMPH_CTL, __VA_ARGS__)
#define RFLD_CS_EMPH_CTL(fld) RFLD(CS_EMPH_CTL, fld)

typedef union {
	struct {
		unsigned int mbz_0:18; //0x0
		unsigned int cs_slewrate:2; //0x0
		unsigned int mbz_1:1; //0x0
		unsigned int cs_dly_emph:3; //0x0
		unsigned int mbz_2:1; //0x0
		unsigned int cs_en_emphn:3; //0x7
		unsigned int mbz_3:1; //0x0
		unsigned int cs_en_emphp:3; //0x7
	} f;
	unsigned int v;
} DDR4_CS_EMPH_CTL_T;
#define DDR4_CS_EMPH_CTLar (0xB81425F4)
#define DDR4_CS_EMPH_CTLdv (0x00000077)
#define DDR4_CS_EMPH_CTLrv RVAL(DDR4_CS_EMPH_CTL)
#define RMOD_DDR4_CS_EMPH_CTL(...) RMOD(DDR4_CS_EMPH_CTL, __VA_ARGS__)
#define RIZS_DDR4_CS_EMPH_CTL(...) RIZS(DDR4_CS_EMPH_CTL, __VA_ARGS__)
#define RFLD_DDR4_CS_EMPH_CTL(fld) RFLD(DDR4_CS_EMPH_CTL, fld)

typedef union {
	struct {
		unsigned int mbz_0:2; //0x0
		unsigned int ck1_slewrate:2; //0x0
		unsigned int mbz_1:1; //0x0
		unsigned int ck1_dly_emph:3; //0x0
		unsigned int mbz_2:1; //0x0
		unsigned int ck1_en_emphn:3; //0x7
		unsigned int mbz_3:1; //0x0
		unsigned int ck1_en_emphp:3; //0x7
		unsigned int mbz_4:2; //0x0
		unsigned int ck_slewrate:2; //0x0
		unsigned int mbz_5:1; //0x0
		unsigned int ck_dly_emph:3; //0x0
		unsigned int mbz_6:1; //0x0
		unsigned int ck_en_emphn:3; //0x7
		unsigned int mbz_7:1; //0x0
		unsigned int ck_en_emphp:3; //0x7
	} f;
	unsigned int v;
} CK_EMPH_CTL_T;
#define CK_EMPH_CTLar (0xB81425F8)
#define CK_EMPH_CTLdv (0x00770077)
#define CK_EMPH_CTLrv RVAL(CK_EMPH_CTL)
#define RMOD_CK_EMPH_CTL(...) RMOD(CK_EMPH_CTL, __VA_ARGS__)
#define RIZS_CK_EMPH_CTL(...) RIZS(CK_EMPH_CTL, __VA_ARGS__)
#define RFLD_CK_EMPH_CTL(fld) RFLD(CK_EMPH_CTL, fld)

typedef union {
	struct {
		unsigned int mbz_0:2; //0x0
		unsigned int adr_slewrate_1:2; //0x0
		unsigned int mbz_1:1; //0x0
		unsigned int adr_dly_emph_1:3; //0x0
		unsigned int mbz_2:1; //0x0
		unsigned int adr_en_emphn_1:3; //0x7
		unsigned int mbz_3:1; //0x0
		unsigned int adr_en_emphp_1:3; //0x7
		unsigned int mbz_4:2; //0x0
		unsigned int adr_slewrate_0:2; //0x0
		unsigned int mbz_5:1; //0x0
		unsigned int adr_dly_emph_0:3; //0x0
		unsigned int mbz_6:1; //0x0
		unsigned int adr_en_emphn_0:3; //0x7
		unsigned int mbz_7:1; //0x0
		unsigned int adr_en_emphp_0:3; //0x7
	} f;
	unsigned int v;
} ADR_EMPH_CTL_0_T;
#define ADR_EMPH_CTL_0ar (0xB81425FC)
#define ADR_EMPH_CTL_0dv (0x00770077)
#define ADR_EMPH_CTL_0rv RVAL(ADR_EMPH_CTL_0)
#define RMOD_ADR_EMPH_CTL_0(...) RMOD(ADR_EMPH_CTL_0, __VA_ARGS__)
#define RIZS_ADR_EMPH_CTL_0(...) RIZS(ADR_EMPH_CTL_0, __VA_ARGS__)
#define RFLD_ADR_EMPH_CTL_0(fld) RFLD(ADR_EMPH_CTL_0, fld)

typedef union {
	struct {
		unsigned int mbz_0:2; //0x0
		unsigned int adr_slewrate_1:2; //0x0
		unsigned int mbz_1:1; //0x0
		unsigned int adr_dly_emph_1:3; //0x0
		unsigned int mbz_2:1; //0x0
		unsigned int adr_en_emphn_1:3; //0x7
		unsigned int mbz_3:1; //0x0
		unsigned int adr_en_emphp_1:3; //0x7
		unsigned int mbz_4:2; //0x0
		unsigned int adr_slewrate_0:2; //0x0
		unsigned int mbz_5:1; //0x0
		unsigned int adr_dly_emph_0:3; //0x0
		unsigned int mbz_6:1; //0x0
		unsigned int adr_en_emphn_0:3; //0x7
		unsigned int mbz_7:1; //0x0
		unsigned int adr_en_emphp_0:3; //0x7
	} f;
	unsigned int v;
} ADR_EMPH_CTL_1_T;
#define ADR_EMPH_CTL_1ar (0xB8142600)
#define ADR_EMPH_CTL_1dv (0x00770077)
#define ADR_EMPH_CTL_1rv RVAL(ADR_EMPH_CTL_1)
#define RMOD_ADR_EMPH_CTL_1(...) RMOD(ADR_EMPH_CTL_1, __VA_ARGS__)
#define RIZS_ADR_EMPH_CTL_1(...) RIZS(ADR_EMPH_CTL_1, __VA_ARGS__)
#define RFLD_ADR_EMPH_CTL_1(fld) RFLD(ADR_EMPH_CTL_1, fld)

typedef union {
	struct {
		unsigned int a15_emph_sel:2; //0x0
		unsigned int a14_emph_sel:2; //0x0
		unsigned int a13_emph_sel:2; //0x0
		unsigned int a12_emph_sel:2; //0x0
		unsigned int a11_emph_sel:2; //0x0
		unsigned int a10_emph_sel:2; //0x0
		unsigned int a9_emph_sel:2; //0x0
		unsigned int a8_emph_sel:2; //0x0
		unsigned int a7_emph_sel:2; //0x0
		unsigned int a6_emph_sel:2; //0x0
		unsigned int a5_emph_sel:2; //0x0
		unsigned int a4_emph_sel:2; //0x0
		unsigned int a3_emph_sel:2; //0x0
		unsigned int a2_emph_sel:2; //0x0
		unsigned int a1_emph_sel:2; //0x0
		unsigned int a0_emph_sel:2; //0x0
	} f;
	unsigned int v;
} ADR_EMPH_SEL_0_T;
#define ADR_EMPH_SEL_0ar (0xB8142604)
#define ADR_EMPH_SEL_0dv (0x00000000)
#define ADR_EMPH_SEL_0rv RVAL(ADR_EMPH_SEL_0)
#define RMOD_ADR_EMPH_SEL_0(...) RMOD(ADR_EMPH_SEL_0, __VA_ARGS__)
#define RIZS_ADR_EMPH_SEL_0(...) RIZS(ADR_EMPH_SEL_0, __VA_ARGS__)
#define RFLD_ADR_EMPH_SEL_0(fld) RFLD(ADR_EMPH_SEL_0, fld)

typedef union {
	struct {
		unsigned int act_emph_sel:2; //0x0
		unsigned int rst_emph_sel:2; //0x0
		unsigned int we_emph_sel:2; //0x0
		unsigned int ras_emph_sel:2; //0x0
		unsigned int par1_emph_sel:2; //0x0
		unsigned int par_emph_sel:2; //0x0
		unsigned int odt_emph_sel:2; //0x0
		unsigned int cke1_emph_sel:2; //0x0
		unsigned int cke_emph_sel:2; //0x0
		unsigned int cas_emph_sel:2; //0x0
		unsigned int ba2_emph_sel:2; //0x0
		unsigned int ba1_emph_sel:2; //0x0
		unsigned int ba0_emph_sel:2; //0x0
		unsigned int ad1_5_emph_sel:2; //0x0
		unsigned int ad1_4_emph_sel:2; //0x0
		unsigned int a16_emph_sel:2; //0x0
	} f;
	unsigned int v;
} ADR_EMPH_SEL_1_T;
#define ADR_EMPH_SEL_1ar (0xB8142608)
#define ADR_EMPH_SEL_1dv (0x00000000)
#define ADR_EMPH_SEL_1rv RVAL(ADR_EMPH_SEL_1)
#define RMOD_ADR_EMPH_SEL_1(...) RMOD(ADR_EMPH_SEL_1, __VA_ARGS__)
#define RIZS_ADR_EMPH_SEL_1(...) RIZS(ADR_EMPH_SEL_1, __VA_ARGS__)
#define RFLD_ADR_EMPH_SEL_1(fld) RFLD(ADR_EMPH_SEL_1, fld)

typedef union {
	struct {
		unsigned int mbz_0:20; //0x0
		unsigned int odt_1_emph_sel:2; //0x0
		unsigned int alert1_emph_sel:2; //0x0
		unsigned int alert_emph_sel:2; //0x0
		unsigned int ddr4_ten_emph_sel:2; //0x0
		unsigned int ddr4_cke_emph_sel:2; //0x0
		unsigned int ddr4_bg0_emph_sel:2; //0x0
	} f;
	unsigned int v;
} ADR_EMPH_SEL_2_T;
#define ADR_EMPH_SEL_2ar (0xB814260C)
#define ADR_EMPH_SEL_2dv (0x00000000)
#define ADR_EMPH_SEL_2rv RVAL(ADR_EMPH_SEL_2)
#define RMOD_ADR_EMPH_SEL_2(...) RMOD(ADR_EMPH_SEL_2, __VA_ARGS__)
#define RIZS_ADR_EMPH_SEL_2(...) RIZS(ADR_EMPH_SEL_2, __VA_ARGS__)
#define RFLD_ADR_EMPH_SEL_2(fld) RFLD(ADR_EMPH_SEL_2, fld)

typedef union {
	struct {
		unsigned int dqsen_dbg:32; //0x0
	} f;
	unsigned int v;
} DBG_READ_0_T;
#define DBG_READ_0ar (0xB8142610)
#define DBG_READ_0dv (0x00000000)
#define DBG_READ_0rv RVAL(DBG_READ_0)
#define RMOD_DBG_READ_0(...) RMOD(DBG_READ_0, __VA_ARGS__)
#define RIZS_DBG_READ_0(...) RIZS(DBG_READ_0, __VA_ARGS__)
#define RFLD_DBG_READ_0(fld) RFLD(DBG_READ_0, fld)

typedef union {
	struct {
		unsigned int dpi_dbg_dll:32; //0x0
	} f;
	unsigned int v;
} DBG_READ_1_T;
#define DBG_READ_1ar (0xB8142614)
#define DBG_READ_1dv (0x00000000)
#define DBG_READ_1rv RVAL(DBG_READ_1)
#define RMOD_DBG_READ_1(...) RMOD(DBG_READ_1, __VA_ARGS__)
#define RIZS_DBG_READ_1(...) RIZS(DBG_READ_1, __VA_ARGS__)
#define RFLD_DBG_READ_1(fld) RFLD(DBG_READ_1, fld)

#endif //_REG_MAP_DPI_CRT_H_
