#ifndef __GLB_PLL_H__
#define __GLB_PLL_H__

#define RVAL(__reg)   (*((regval)__reg##ar))
#define RMOD(__reg, ...) rset(__reg, __reg##rv, __VA_ARGS__)
#define RIZS(__reg, ...) rset(__reg, 0, __VA_ARGS__)
#define RFLD(__reg, fld) (*((const volatile __reg##_T *)__reg##ar)).f.fld


typedef union {
	struct {
		unsigned int mbz0:12; //reserved and don't care 
		unsigned int cpu_clk_sel:1; 
		unsigned int nor_clk_sel:1; 
		unsigned int lxb_clk_sel:1;
		unsigned int sw_pll_rdy:1;
		unsigned int cpu_pll_rdy:1;
        unsigned int reg125m_pll_rdy:1;
        unsigned int mbz2:6; // bypass
        unsigned int en_ddr_pll:1;
        unsigned int en_nor_pll:1;
        unsigned int en_nand_pll:1;
        unsigned int en_usb_pll:1;
        unsigned int en_lxb_pll:1;
        unsigned int en_cpu_pll:1;
        unsigned int en_125m_pll:1;
        unsigned int en_sram_pll:1;
	} f;
	unsigned int v;
} PLL_GLB_CTRL0_T;
#define PLL_GLB_CTRL0ar         (0xBB00E200)
#define PLL_GLB_CTRL0dv         (0x000000FF)
#define PLL_GLB_CTRL0rv         RVAL(PLL_GLB_CTRL0)
#define RMOD_PLL_GLB_CTRL0(...) RMOD(PLL_GLB_CTRL0, __VA_ARGS__)
#define RIZS_PLL_GLB_CTRL0(...) RIZS(PLL_GLB_CTRL0, __VA_ARGS__)
#define RFLD_PLL_GLB_CTRL0(fld) RFLD(PLL_GLB_CTRL0, fld)

typedef union {
	struct {
		unsigned int mbz0:21; //reserved and don't care 
		unsigned int dll_pre_divn:2; 
		unsigned int mbz1:3; 
		unsigned int reg_cmu_dly_en_cpu:1;
		unsigned int mbo0:2;
		unsigned int reg_fld_dsel_cpu:1;
        unsigned int mbo1:2;        
	} f;
	unsigned int v;
} PLL_GLB_CTRL1_T;
#define PLL_GLB_CTRL1ar                 (0xBB00E204)
#define PLL_GLB_CTRL1dv                 (0x0000023F)
#define PLL_GLB_CTRL1rv                 RVAL(PLL_GLB_CTRL1)
#define RMOD_PLL_GLB_CTRL1(...)         RMOD(PLL_GLB_CTRL1, __VA_ARGS__)
#define RIZS_PLL_GLB_CTRL1(...)         RIZS(PLL_GLB_CTRL1, __VA_ARGS__)
#define RFLD_PLL_GLB_CTRL1(fld)         RFLD(PLL_GLB_CTRL1, fld)

typedef union {
	struct {
		unsigned int mbz0:4; //reserved and don't care 
		unsigned int cmu_test_en_cpu:1; 
		unsigned int cmu_divn3_cpu:2; 
		unsigned int cpu_cmu_fcode_in:13;
		unsigned int cpu_cmu_ncode_in:8;
		unsigned int cpu_cmu_bypass_pi:1;
        unsigned int cpu_cmu_sel_div4:1;    
        unsigned int cpu_cmu_sel_prediv:2;        
	} f;
	unsigned int v;
} PLL_CPU_CTRL0_T;
#define PLL_CPU_CTRL0ar                 (0xBB00E208)
#define PLL_CPU_CTRL0dv                 (0x020002E8)
#define PLL_CPU_CTRL0rv                 RVAL(PLL_CPU_CTRL0)
#define RMOD_PLL_CPU_CTRL0(...)         RMOD(PLL_CPU_CTRL0, __VA_ARGS__)
#define RIZS_PLL_CPU_CTRL0(...)         RIZS(PLL_CPU_CTRL0, __VA_ARGS__)
#define RFLD_PLL_CPU_CTRL0(fld)         RFLD(PLL_CPU_CTRL0, fld)

typedef union {
	struct {
		unsigned int cpu_cmu_en_ssc:1;
		unsigned int cpu_cmu_step_in:13;
		unsigned int cpu_cmu_tbase_in:12;
		unsigned int cpu_cmu_ssc_order:1;
		unsigned int cpu_cmu_time2_rst_width:2;
		unsigned int cpu_cmu_time0_ck:3;
	} f;
	unsigned int v;
} PLL_CPU_CTRL1_T;
#define PLL_CPU_CTRL1ar                 (0xBB00E20C)
#define PLL_CPU_CTRL1dv                 (0x00000001)
#define PLL_CPU_CTRL1rv                 RVAL(PLL_CPU_CTRL1)
#define RMOD_PLL_CPU_CTRL1(...)         RMOD(PLL_CPU_CTRL1, __VA_ARGS__)
#define RIZS_PLL_CPU_CTRL1(...)         RIZS(PLL_CPU_CTRL1, __VA_ARGS__)
#define RFLD_PLL_CPU_CTRL1(fld)         RFLD(PLL_CPU_CTRL1, fld)

typedef union {
	struct {
		unsigned int mbz0:1;
		unsigned int cpu_cmu_en:1;
		unsigned int cpu_cmu_clkrdy:2;
		unsigned int cpu_cmu_config:21;
        unsigned int reg_cmu_divn2_cpu:3;
        unsigned int reg_cmu_divn2_dll:3;
        unsigned int cpu_cmu_en_dll:1;
	} f;
	unsigned int v;
} PLL_CPU_MISC_CTRL_T;
#define PLL_CPU_MISC_CTRLar             (0xBB00E210)
#define PLL_CPU_MISC_CTRLdv             (0x6B439E13)
#define PLL_CPU_MISC_CTRLrv             RVAL(PLL_CPU_MISC_CTRL)
#define RMOD_PLL_CPU_MISC_CTRL(...)     RMOD(PLL_CPU_MISC_CTRL, __VA_ARGS__)
#define RIZS_PLL_CPU_MISC_CTRL(...)     RIZS(PLL_CPU_MISC_CTRL, __VA_ARGS__)
#define RFLD_PLL_CPU_MISC_CTRL(fld)     RFLD(PLL_CPU_MISC_CTRL, fld)

typedef union {
	struct {
		unsigned int mbz0:6; //reserved and don't care 
		unsigned int cmu_test_en_sw:1; 
		unsigned int sw_cmu_fcode_in:13;
		unsigned int sw_cmu_ncode_in:8;
		unsigned int sw_cmu_bypass_pi:1;
        unsigned int sw_cmu_sel_div4:1;    
        unsigned int sw_cmu_sel_prediv:2;        
	} f;
	unsigned int v;
} PLL_SW_CTRL0_T;
#define PLL_SW_CTRL0ar                  (0xBB00E214)
#define PLL_SW_CTRL0dv                  (0x000002F8)
#define PLL_SW_CTRL0rv                  RVAL(PLL_SW_CTRL0)
#define RMOD_PLL_SW_CTRL0(...)          RMOD(PLL_SW_CTRL0, __VA_ARGS__)
#define RIZS_PLL_SW_CTRL0(...)          RIZS(PLL_SW_CTRL0, __VA_ARGS__)
#define RFLD_PLL_SW_CTRL0(fld)          RFLD(PLL_SW_CTRL0, fld)

typedef union {
	struct {
        unsigned int mbz0:6;
		unsigned int sw_cmu_en_ssc:1;
		unsigned int sw_cmu_step_in:13;
		unsigned int sw_cmu_tbase_in:12;
	} f;
	unsigned int v;
} PLL_SW_CTRL1_T;
#define PLL_SW_CTRL1ar                  (0xBB00E218)
#define PLL_SW_CTRL1dv                  (0x00000000)
#define PLL_SW_CTRL1rv                  RVAL(PLL_SW_CTRL1)
#define RMOD_PLL_SW_CTRL1(...)          RMOD(PLL_SW_CTRL1, __VA_ARGS__)
#define RIZS_PLL_SW_CTRL1(...)          RIZS(PLL_SW_CTRL1, __VA_ARGS__)
#define RFLD_PLL_SW_CTRL1(fld)          RFLD(PLL_SW_CTRL1, fld)

typedef union {
	struct {
		unsigned int mbz0:3;
		unsigned int sw_cmu_config:29;
	} f;
	unsigned int v;
} PLL_SW_MISC_CTRL_T;
#define PLL_SW_MISC_CTRLar              (0xBB00E218)
#define PLL_SW_MISC_CTRLdv              (0x00D6873C)
#define PLL_SW_MISC_CTRLrv              RVAL(PLL_SW_MISC_CTRL)
#define RMOD_PLL_SW_MISC_CTRL(...)      RMOD(PLL_SW_MISC_CTRL, __VA_ARGS__)
#define RIZS_PLL_SW_MISC_CTRL(...)      RIZS(PLL_SW_MISC_CTRL, __VA_ARGS__)
#define RFLD_PLL_SW_MISC_CTRL(fld)      RFLD(PLL_SW_MISC_CTRL, fld)

typedef union {
	struct {
		unsigned int mbz0:14;
        unsigned int sw_cmu_en:1;
		unsigned int cmu_divn2_spi_nor:4;
        unsigned int cmu_divn2_lxb:4;
        unsigned int cmu_divn2_sw:4;
        unsigned int cmu_divn2_gphy:4;
        unsigned int en_gphy_dbg_pll:1;
	} f;
	unsigned int v;
} PLL_SW_DIV_CTRL_T;
#define PLL_SW_DIV_CTRLar               (0xBB00E220)
#define PLL_SW_DIV_CTRLdv               (0x0002AA26)
#define PLL_SW_DIV_CTRLrv               RVAL(PLL_SW_DIV_CTRL)
#define RMOD_PLL_SW_DIV_CTRL(...)       RMOD(PLL_SW_DIV_CTRL, __VA_ARGS__)
#define RIZS_PLL_SW_DIV_CTRL(...)       RIZS(PLL_SW_DIV_CTRL, __VA_ARGS__)
#define RFLD_PLL_SW_DIV_CTRL(fld)       RFLD(PLL_SW_DIV_CTRL, fld)


// SRAM_PLL
typedef union {
	struct {
		unsigned int mbz0:30;
        unsigned int sram_pll_sel:1;
        unsigned int mbz1:1;
	} f;
	unsigned int v;
} PLL_SRAM_CTRL_T;
#define PLL_SRAM_CTRLar                 (0xB8000700)
#define PLL_SRAM_CTRLdv                 (0x00000000)
#define PLL_SRAM_CTRLrv                 RVAL(PLL_SRAM_CTRL)
#define RMOD_PLL_SRAM_CTRL(...)         RMOD(PLL_SRAM_CTRL, __VA_ARGS__)
#define RIZS_PLL_SRAM_CTRL(...)         RIZS(PLL_SRAM_CTRL, __VA_ARGS__)
#define RFLD_PLL_SRAM_CTRL(fld)         RFLD(PLL_SRAM_CTRL, fld)

#endif // __GLB_PLL_H__
