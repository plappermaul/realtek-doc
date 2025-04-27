#ifndef _REG_MAP_GLB_PLL_H
#define _REG_MAP_GLB_PLL_H

/*-----------------------------------------------------
 Extraced from file_PLL_Bias.xml
-----------------------------------------------------*/
typedef union {
	struct {
		unsigned int mbz_0:23; //0
		unsigned int reg_envbgup_l:1; //0
		unsigned int reg_rbg2_l:2; //2
		unsigned int reg_rbgloop2_l:3; //4
		unsigned int reg_rbg_l:3; //4
	} f;
	unsigned int v;
} BG_POR_TOP_T;
#define BG_POR_TOPar (0xBB000A00)
#define BG_POR_TOPdv (0x000000A4)
#define BG_POR_TOPrv RVAL(BG_POR_TOP)
#define RMOD_BG_POR_TOP(...) RMOD(BG_POR_TOP, __VA_ARGS__)
#define RIZS_BG_POR_TOP(...) RIZS(BG_POR_TOP, __VA_ARGS__)
#define RFLD_BG_POR_TOP(fld) RFLD(BG_POR_TOP, fld)

typedef union {
	struct {
		unsigned int mbz_0:23; //0
		unsigned int sel_osc_clk_test_en:1; //0
		unsigned int sel_125m_test_en:1; //0
		unsigned int ckrefbuf_en:1; //1
		unsigned int ckrefbuf_cml_i_sds1:3; //3
		unsigned int ckrefbuf_cml_i_sds0:3; //3
	} f;
	unsigned int v;
} XTAL_TOP_T;
#define XTAL_TOPar (0xBB000A04)
#define XTAL_TOPdv (0x0000005B)
#define XTAL_TOPrv RVAL(XTAL_TOP)
#define RMOD_XTAL_TOP(...) RMOD(XTAL_TOP, __VA_ARGS__)
#define RIZS_XTAL_TOP(...) RIZS(XTAL_TOP, __VA_ARGS__)
#define RFLD_XTAL_TOP(fld) RFLD(XTAL_TOP, fld)

typedef union {
	struct {
		unsigned int mbz_0:26; //0
		unsigned int cmu_test_en_fref:1; //0
		unsigned int cmu_clk_test_en:1; //0
		unsigned int sel_iblpf_xtalpll:1; //1
		unsigned int pll_bg_pow:1; //1
		unsigned int pll_reg_bg:2; //3
	} f;
	unsigned int v;
} PLL_GLB_COM_T;
#define PLL_GLB_COMar (0xBB000A08)
#define PLL_GLB_COMdv (0x0000000F)
#define PLL_GLB_COMrv RVAL(PLL_GLB_COM)
#define RMOD_PLL_GLB_COM(...) RMOD(PLL_GLB_COM, __VA_ARGS__)
#define RIZS_PLL_GLB_COM(...) RIZS(PLL_GLB_COM, __VA_ARGS__)
#define RFLD_PLL_GLB_COM(fld) RFLD(PLL_GLB_COM, fld)

typedef union {
	struct {
		unsigned int mbz_0:22; //0
		unsigned int reg_ck25m_fref_en:1; //1
		unsigned int reg_ckrefbuf_cml_en:1; //0
		unsigned int reg_ckrefbuf_cml_i:3; //4
		unsigned int reg_ckref_term_r_sel:2; //2
		unsigned int reg_debug_sel_pll:1; //0
		unsigned int reg_pll_top:2; //0
	} f;
	unsigned int v;
} PLL_TOP_T;
#define PLL_TOPar (0xBB000A0C)
#define PLL_TOPdv (0x00000290)
#define PLL_TOPrv RVAL(PLL_TOP)
#define RMOD_PLL_TOP(...) RMOD(PLL_TOP, __VA_ARGS__)
#define RIZS_PLL_TOP(...) RIZS(PLL_TOP, __VA_ARGS__)
#define RFLD_PLL_TOP(fld) RFLD(PLL_TOP, fld)

typedef union {
	struct {
		unsigned int mbz_0:28; //0
		unsigned int lxb_clk_rdy:1; //0
		unsigned int cpu_clk_rdy:1; //0
		unsigned int pcie_clk_rdy:1; //0
		unsigned int sw_clk_rdy:1; //0
	} f;
	unsigned int v;
} PLL_GLB_SIG_T;
#define PLL_GLB_SIGar (0xBB000A10)
#define PLL_GLB_SIGdv (0x00000000)
#define PLL_GLB_SIGrv RVAL(PLL_GLB_SIG)
#define RMOD_PLL_GLB_SIG(...) RMOD(PLL_GLB_SIG, __VA_ARGS__)
#define RIZS_PLL_GLB_SIG(...) RIZS(PLL_GLB_SIG, __VA_ARGS__)
#define RFLD_PLL_GLB_SIG(fld) RFLD(PLL_GLB_SIG, fld)

typedef union {
	struct {
		unsigned int mbz_0:17; //0
		unsigned int sw_clksel:1; //0
		unsigned int bypass_ddr_pll_ref:1; //0
		unsigned int bypass_ddr_pll:1; //0
		unsigned int spi_nor_clksel:1; //0
		unsigned int lxb_clksel:1; //0
		unsigned int en_pll_mon:1; //0
		unsigned int sel_pll_mon:3; //0
		unsigned int mbz_1:1; //0
		unsigned int bypass_sw_pll:1; //0
		unsigned int bypass_pcie_pll:1; //0
		unsigned int mbz_2:1; //0
		unsigned int bypass_spi_nor_pll:1; //0
		unsigned int bypass_lxb_pll:1; //0
	} f;
	unsigned int v;
} PLL_GLB_CTRL_T;
#define PLL_GLB_CTRLar (0xBB000A14)
#define PLL_GLB_CTRLdv (0x00000000)
#define PLL_GLB_CTRLrv RVAL(PLL_GLB_CTRL)
#define RMOD_PLL_GLB_CTRL(...) RMOD(PLL_GLB_CTRL, __VA_ARGS__)
#define RIZS_PLL_GLB_CTRL(...) RIZS(PLL_GLB_CTRL, __VA_ARGS__)
#define RFLD_PLL_GLB_CTRL(fld) RFLD(PLL_GLB_CTRL, fld)

typedef union {
	struct {
		unsigned int cpu_cmu_en:1; //1
		unsigned int cpu_cmu_fcode_in:13; //0
		unsigned int cpu_cmu_ncode_in:8; //58
		unsigned int cpu_cmu_bypass:1; //1
		unsigned int cmu_divn1_cpu:2; //2
		unsigned int cmu_divn2_cpu:2; //0
		unsigned int cmu_divn3_cpu:2; //1
		unsigned int cpu_cmu_sel_prediv:2; //0
		unsigned int cmu_cpuclk_en:1; //1
	} f;
	unsigned int v;
} PLL_CPU_CTRL0_T;
#define PLL_CPU_CTRL0ar (0xBB000A18)
#define PLL_CPU_CTRL0dv (0x8000EB09)
#define PLL_CPU_CTRL0rv RVAL(PLL_CPU_CTRL0)
#define RMOD_PLL_CPU_CTRL0(...) RMOD(PLL_CPU_CTRL0, __VA_ARGS__)
#define RIZS_PLL_CPU_CTRL0(...) RIZS(PLL_CPU_CTRL0, __VA_ARGS__)
#define RFLD_PLL_CPU_CTRL0(fld) RFLD(PLL_CPU_CTRL0, fld)

typedef union {
	struct {
		unsigned int mbz_0:26; //0
		unsigned int cpu_cmu_time2_rst_width:2; //0
		unsigned int cpu_cmu_time0_ck:3; //1
		unsigned int cpu_cmu_en_wd:1; //1
	} f;
	unsigned int v;
} PLL_CPU_CTRL1_T;
#define PLL_CPU_CTRL1ar (0xBB000A1C)
#define PLL_CPU_CTRL1dv (0x00000003)
#define PLL_CPU_CTRL1rv RVAL(PLL_CPU_CTRL1)
#define RMOD_PLL_CPU_CTRL1(...) RMOD(PLL_CPU_CTRL1, __VA_ARGS__)
#define RIZS_PLL_CPU_CTRL1(...) RIZS(PLL_CPU_CTRL1, __VA_ARGS__)
#define RFLD_PLL_CPU_CTRL1(fld) RFLD(PLL_CPU_CTRL1, fld)

typedef union {
	struct {
		unsigned int mbz_0:5; //0
		unsigned int cpu_cmu_en_ssc:1; //0
		unsigned int cpu_cmu_step_in:13; //0
		unsigned int cpu_cmu_tbase_in:12; //0
		unsigned int cpu_cmu_ssc_order:1; //0
	} f;
	unsigned int v;
} PLL_CPU_CTRL2_T;
#define PLL_CPU_CTRL2ar (0xBB000A20)
#define PLL_CPU_CTRL2dv (0x00000000)
#define PLL_CPU_CTRL2rv RVAL(PLL_CPU_CTRL2)
#define RMOD_PLL_CPU_CTRL2(...) RMOD(PLL_CPU_CTRL2, __VA_ARGS__)
#define RIZS_PLL_CPU_CTRL2(...) RIZS(PLL_CPU_CTRL2, __VA_ARGS__)
#define RFLD_PLL_CPU_CTRL2(fld) RFLD(PLL_CPU_CTRL2, fld)

typedef union {
	struct {
		unsigned int mbz_0:8; //0
		unsigned int cpu_cmu_test_en:1; //0
		unsigned int cpu_25mout_en:1; //0
		unsigned int cpu_cmu_clkrdy:2; //2
		unsigned int cpu_cmu_cp_new_en:1; //0
		unsigned int cpu_cmu_big_kvco:1; //0
		unsigned int cpu_cmu_en_center_in:1; //0
		unsigned int cpu_cmu_pi_i_sel:2; //1
		unsigned int cpu_cmu_ldo_modesel:1; //1
		unsigned int cpu_cmu_ldo_sel:2; //1
		unsigned int cpu_cmu_lpf_rs:3; //3
		unsigned int cpu_cmu_lpf_cp:1; //1
		unsigned int cpu_cmu_sel_cp_i:5; //3
		unsigned int cpu_cmu_vc_dly:1; //1
		unsigned int reg_cmu_dly_cpu:1; //1
		unsigned int reg_fld_dsel_cpu:1; //0
	} f;
	unsigned int v;
} PLL_CPU_MISC_CTRL_T;
#define PLL_CPU_MISC_CTRLar (0xBB000A24)
#define PLL_CPU_MISC_CTRLdv (0x0020D71E)
#define PLL_CPU_MISC_CTRLrv RVAL(PLL_CPU_MISC_CTRL)
#define RMOD_PLL_CPU_MISC_CTRL(...) RMOD(PLL_CPU_MISC_CTRL, __VA_ARGS__)
#define RIZS_PLL_CPU_MISC_CTRL(...) RIZS(PLL_CPU_MISC_CTRL, __VA_ARGS__)
#define RFLD_PLL_CPU_MISC_CTRL(fld) RFLD(PLL_CPU_MISC_CTRL, fld)

typedef union {
	struct {
		unsigned int sw_cmu_en:1; //1
		unsigned int sw_cmu_fcode_in:13; //0
		unsigned int sw_cmu_ncode_in:8; //50
		unsigned int sw_cmu_bypass:1; //1
		unsigned int mbz_0:1; //0
		unsigned int cmu_divnsel_sw:1; //0
		unsigned int cmu_div2_sw:4; //2
		unsigned int sw_cmu_sel_prediv:2; //0
		unsigned int cmu_swclk_en:1; //1
	} f;
	unsigned int v;
} PLL_SW_CTRL0_T;
#define PLL_SW_CTRL0ar (0xBB000A28)
#define PLL_SW_CTRL0dv (0x8000CA11)
#define PLL_SW_CTRL0rv RVAL(PLL_SW_CTRL0)
#define RMOD_PLL_SW_CTRL0(...) RMOD(PLL_SW_CTRL0, __VA_ARGS__)
#define RIZS_PLL_SW_CTRL0(...) RIZS(PLL_SW_CTRL0, __VA_ARGS__)
#define RFLD_PLL_SW_CTRL0(fld) RFLD(PLL_SW_CTRL0, fld)

typedef union {
	struct {
		unsigned int mbz_0:26; //0
		unsigned int sw_cmu_time2_rst_width:2; //0
		unsigned int sw_cmu_time0_ck:3; //1
		unsigned int sw_cmu_en_wd:1; //1
	} f;
	unsigned int v;
} PLL_SW_CTRL1_T;
#define PLL_SW_CTRL1ar (0xBB000A2C)
#define PLL_SW_CTRL1dv (0x00000003)
#define PLL_SW_CTRL1rv RVAL(PLL_SW_CTRL1)
#define RMOD_PLL_SW_CTRL1(...) RMOD(PLL_SW_CTRL1, __VA_ARGS__)
#define RIZS_PLL_SW_CTRL1(...) RIZS(PLL_SW_CTRL1, __VA_ARGS__)
#define RFLD_PLL_SW_CTRL1(fld) RFLD(PLL_SW_CTRL1, fld)

typedef union {
	struct {
		unsigned int mbz_0:5; //0
		unsigned int sw_cmu_en_ssc:1; //0
		unsigned int sw_cmu_step_in:13; //0
		unsigned int sw_cmu_tbase_in:12; //0
		unsigned int sw_cmu_ssc_order:1; //0
	} f;
	unsigned int v;
} PLL_SW_CTRL2_T;
#define PLL_SW_CTRL2ar (0xBB000A30)
#define PLL_SW_CTRL2dv (0x00000000)
#define PLL_SW_CTRL2rv RVAL(PLL_SW_CTRL2)
#define RMOD_PLL_SW_CTRL2(...) RMOD(PLL_SW_CTRL2, __VA_ARGS__)
#define RIZS_PLL_SW_CTRL2(...) RIZS(PLL_SW_CTRL2, __VA_ARGS__)
#define RFLD_PLL_SW_CTRL2(fld) RFLD(PLL_SW_CTRL2, fld)

typedef union {
	struct {
		unsigned int mbz_0:8; //0
		unsigned int sw_cmu_test_en:1; //0
		unsigned int sw_25mout_en:1; //0
		unsigned int sw_cmu_clkrdy:2; //2
		unsigned int sw_cmu_cp_new_en:1; //0
		unsigned int sw_cmu_big_kvco:1; //0
		unsigned int sw_cmu_en_center_in:1; //0
		unsigned int sw_cmu_pi_i_sel:2; //1
		unsigned int sw_cmu_ldo_modesel:1; //1
		unsigned int sw_cmu_ldo_sel:2; //1
		unsigned int sw_cmu_lpf_rs:3; //3
		unsigned int sw_cmu_lpf_cp:1; //1
		unsigned int sw_cmu_sel_cp_i:5; //3
		unsigned int sw_cmu_vc_dly:1; //1
		unsigned int reg_cmu_dly_sw:1; //1
		unsigned int reg_fld_dsel_sw:1; //0
	} f;
	unsigned int v;
} PLL_SW_MISC_CTRL_T;
#define PLL_SW_MISC_CTRLar (0xBB000A34)
#define PLL_SW_MISC_CTRLdv (0x0020D71E)
#define PLL_SW_MISC_CTRLrv RVAL(PLL_SW_MISC_CTRL)
#define RMOD_PLL_SW_MISC_CTRL(...) RMOD(PLL_SW_MISC_CTRL, __VA_ARGS__)
#define RIZS_PLL_SW_MISC_CTRL(...) RIZS(PLL_SW_MISC_CTRL, __VA_ARGS__)
#define RFLD_PLL_SW_MISC_CTRL(fld) RFLD(PLL_SW_MISC_CTRL, fld)

typedef union {
	struct {
		unsigned int lxb_cmu_en:1; //1
		unsigned int lxb_cmu_fcode_in:13; //0
		unsigned int lxb_cmu_ncode_in:8; //54
		unsigned int lxb_cmu_bypass:1; //1
		unsigned int cmu_div1_if:2; //1
		unsigned int cmu_div2_spi:4; //5
		unsigned int lxb_cmu_sel_prediv:2; //0
		unsigned int cmu_ifclk_en:1; //0
	} f;
	unsigned int v;
} PLL_LXB_CTRL0_T;
#define PLL_LXB_CTRL0ar (0xBB000A38)
#define PLL_LXB_CTRL0dv (0x8000DAA8)
#define PLL_LXB_CTRL0rv RVAL(PLL_LXB_CTRL0)
#define RMOD_PLL_LXB_CTRL0(...) RMOD(PLL_LXB_CTRL0, __VA_ARGS__)
#define RIZS_PLL_LXB_CTRL0(...) RIZS(PLL_LXB_CTRL0, __VA_ARGS__)
#define RFLD_PLL_LXB_CTRL0(fld) RFLD(PLL_LXB_CTRL0, fld)

typedef union {
	struct {
		unsigned int mbz_0:14; //0
		unsigned int cmu_div1_spi:2; //1
		unsigned int cmu_div2_if:4; //5
		unsigned int cmu_div1_lxb:2; //1
		unsigned int cmu_div2_lxb:4; //5
		unsigned int lxb_cmu_time2_rst_width:2; //0
		unsigned int lxb_cmu_time0_ck:3; //1
		unsigned int lxb_cmu_en_wd:1; //1
	} f;
	unsigned int v;
} PLL_LXB_CTRL1_T;
#define PLL_LXB_CTRL1ar (0xBB000A3C)
#define PLL_LXB_CTRL1dv (0x00015543)
#define PLL_LXB_CTRL1rv RVAL(PLL_LXB_CTRL1)
#define RMOD_PLL_LXB_CTRL1(...) RMOD(PLL_LXB_CTRL1, __VA_ARGS__)
#define RIZS_PLL_LXB_CTRL1(...) RIZS(PLL_LXB_CTRL1, __VA_ARGS__)
#define RFLD_PLL_LXB_CTRL1(fld) RFLD(PLL_LXB_CTRL1, fld)

typedef union {
	struct {
		unsigned int mbz_0:5; //0
		unsigned int lxb_cmu_en_ssc:1; //0
		unsigned int lxb_cmu_step_in:13; //0
		unsigned int lxb_cmu_tbase_in:12; //0
		unsigned int lxb_cmu_ssc_order:1; //0
	} f;
	unsigned int v;
} PLL_LXB_CTRL2_T;
#define PLL_LXB_CTRL2ar (0xBB000A40)
#define PLL_LXB_CTRL2dv (0x00000000)
#define PLL_LXB_CTRL2rv RVAL(PLL_LXB_CTRL2)
#define RMOD_PLL_LXB_CTRL2(...) RMOD(PLL_LXB_CTRL2, __VA_ARGS__)
#define RIZS_PLL_LXB_CTRL2(...) RIZS(PLL_LXB_CTRL2, __VA_ARGS__)
#define RFLD_PLL_LXB_CTRL2(fld) RFLD(PLL_LXB_CTRL2, fld)

typedef union {
	struct {
		unsigned int mbz_0:6; //0
		unsigned int cmu_lxbclk_en:1; //1
		unsigned int cmu_spiclk_en:1; //1
		unsigned int lxb_cmu_test_en:1; //0
		unsigned int lxb_25mout_en:1; //0
		unsigned int lxb_cmu_clkrdy:2; //2
		unsigned int lxb_cmu_cp_new_en:1; //0
		unsigned int lxb_cmu_big_kvco:1; //0
		unsigned int lxb_cmu_en_center_in:1; //0
		unsigned int lxb_cmu_pi_i_sel:2; //1
		unsigned int lxb_cmu_ldo_modesel:1; //1
		unsigned int lxb_cmu_ldo_sel:2; //1
		unsigned int lxb_cmu_lpf_rs:3; //3
		unsigned int lxb_cmu_lpf_cp:1; //1
		unsigned int lxb_cmu_sel_cp_i:5; //3
		unsigned int lxb_cmu_vc_dly:1; //1
		unsigned int reg_cmu_dly_lxb:1; //1
		unsigned int reg_fld_dsel_lxb:1; //0
	} f;
	unsigned int v;
} PLL_LXB_MISC_CTRL_T;
#define PLL_LXB_MISC_CTRLar (0xBB000A44)
#define PLL_LXB_MISC_CTRLdv (0x0320D71E)
#define PLL_LXB_MISC_CTRLrv RVAL(PLL_LXB_MISC_CTRL)
#define RMOD_PLL_LXB_MISC_CTRL(...) RMOD(PLL_LXB_MISC_CTRL, __VA_ARGS__)
#define RIZS_PLL_LXB_MISC_CTRL(...) RIZS(PLL_LXB_MISC_CTRL, __VA_ARGS__)
#define RFLD_PLL_LXB_MISC_CTRL(fld) RFLD(PLL_LXB_MISC_CTRL, fld)

typedef union {
	struct {
		unsigned int pcie_cmu_en:1; //1
		unsigned int pcie_cmu_fcode_in:13; //0
		unsigned int pcie_cmu_ncode_in:8; //58
		unsigned int pcie_cmu_bypass:1; //1
		unsigned int cmu_divn1_pcie:2; //2
		unsigned int cmu_divn2_pcie:2; //0
		unsigned int cmu_divn3_pcie:2; //1
		unsigned int pcie_cmu_sel_prediv:2; //0
		unsigned int mbz_0:1; //0
	} f;
	unsigned int v;
} PLL_PCIE_CTRL0_T;
#define PLL_PCIE_CTRL0ar (0xBB000A48)
#define PLL_PCIE_CTRL0dv (0x8000EB08)
#define PLL_PCIE_CTRL0rv RVAL(PLL_PCIE_CTRL0)
#define RMOD_PLL_PCIE_CTRL0(...) RMOD(PLL_PCIE_CTRL0, __VA_ARGS__)
#define RIZS_PLL_PCIE_CTRL0(...) RIZS(PLL_PCIE_CTRL0, __VA_ARGS__)
#define RFLD_PLL_PCIE_CTRL0(fld) RFLD(PLL_PCIE_CTRL0, fld)

typedef union {
	struct {
		unsigned int mbz_0:26; //0
		unsigned int pcie_cmu_time2_rst_width:2; //0
		unsigned int pcie_cmu_time0_ck:3; //1
		unsigned int pcie_cmu_en_wd:1; //1
	} f;
	unsigned int v;
} PLL_PCIE_CTRL1_T;
#define PLL_PCIE_CTRL1ar (0xBB000A4C)
#define PLL_PCIE_CTRL1dv (0x00000003)
#define PLL_PCIE_CTRL1rv RVAL(PLL_PCIE_CTRL1)
#define RMOD_PLL_PCIE_CTRL1(...) RMOD(PLL_PCIE_CTRL1, __VA_ARGS__)
#define RIZS_PLL_PCIE_CTRL1(...) RIZS(PLL_PCIE_CTRL1, __VA_ARGS__)
#define RFLD_PLL_PCIE_CTRL1(fld) RFLD(PLL_PCIE_CTRL1, fld)

typedef union {
	struct {
		unsigned int mbz_0:5; //0
		unsigned int pcie_cmu_en_ssc:1; //0
		unsigned int pcie_cmu_step_in:13; //0
		unsigned int pcie_cmu_tbase_in:12; //0
		unsigned int pcie_cmu_ssc_order:1; //0
	} f;
	unsigned int v;
} PLL_PCIE_CTRL2_T;
#define PLL_PCIE_CTRL2ar (0xBB000A50)
#define PLL_PCIE_CTRL2dv (0x00000000)
#define PLL_PCIE_CTRL2rv RVAL(PLL_PCIE_CTRL2)
#define RMOD_PLL_PCIE_CTRL2(...) RMOD(PLL_PCIE_CTRL2, __VA_ARGS__)
#define RIZS_PLL_PCIE_CTRL2(...) RIZS(PLL_PCIE_CTRL2, __VA_ARGS__)
#define RFLD_PLL_PCIE_CTRL2(fld) RFLD(PLL_PCIE_CTRL2, fld)

typedef union {
	struct {
		unsigned int mbz_0:8; //0
		unsigned int pcie_cmu_test_en:1; //0
		unsigned int pcie_25mout_en:1; //0
		unsigned int pcie_cmu_clkrdy:2; //2
		unsigned int pcie_cmu_cp_new_en:1; //0
		unsigned int pcie_cmu_big_kvco:1; //0
		unsigned int pcie_cmu_en_center_in:1; //0
		unsigned int pcie_cmu_pi_i_sel:2; //1
		unsigned int pcie_cmu_ldo_modesel:1; //1
		unsigned int pcie_cmu_ldo_sel:2; //1
		unsigned int pcie_cmu_lpf_rs:3; //3
		unsigned int pcie_cmu_lpf_cp:1; //1
		unsigned int pcie_cmu_sel_cp_i:5; //3
		unsigned int pcie_cmu_vc_dly:1; //1
		unsigned int reg_cmu_dly_pcie:1; //1
		unsigned int reg_fld_dsel_pcie:1; //0
	} f;
	unsigned int v;
} PLL_PCIE_MISC_CTRL_T;
#define PLL_PCIE_MISC_CTRLar (0xBB000A54)
#define PLL_PCIE_MISC_CTRLdv (0x0020D71E)
#define PLL_PCIE_MISC_CTRLrv RVAL(PLL_PCIE_MISC_CTRL)
#define RMOD_PLL_PCIE_MISC_CTRL(...) RMOD(PLL_PCIE_MISC_CTRL, __VA_ARGS__)
#define RIZS_PLL_PCIE_MISC_CTRL(...) RIZS(PLL_PCIE_MISC_CTRL, __VA_ARGS__)
#define RFLD_PLL_PCIE_MISC_CTRL(fld) RFLD(PLL_PCIE_MISC_CTRL, fld)

#endif // _REG_MAP_GLB_PLL_H
