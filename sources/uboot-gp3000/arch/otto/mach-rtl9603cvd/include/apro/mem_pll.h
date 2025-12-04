#ifndef __APRO_MEM_PLL_H__
#define __APRO_MEM_PLL_H__

typedef union {
	struct {
		unsigned int post_pi_sel3:5; //dqs1
		unsigned int post_pi_sel2:5; //dqs0
		unsigned int post_pi_sel1:5; //clkm
		unsigned int post_pi_sel0:5; //mtx (ck)
		unsigned int mbz:5;
		unsigned int en_post_pi:7;
	} f;
	unsigned int v;
} SYS_MEM_PLL_CTRL0_T;
#define SYS_MEM_PLL_CTRL0rv (*((regval)0xb8000234))
#define SYS_MEM_PLL_CTRL0dv (0x0000007F)
#define RMOD_SYS_MEM_PLL_CTRL0(...) rset(SYS_MEM_PLL_CTRL0, SYS_MEM_PLL_CTRL0rv, __VA_ARGS__)
#define RIZS_SYS_MEM_PLL_CTRL0(...) rset(SYS_MEM_PLL_CTRL0, 0, __VA_ARGS__)
#define RFLD_SYS_MEM_PLL_CTRL0(fld) (*((const volatile SYS_MEM_PLL_CTRL0_T *)0xb8000234)).f.fld

typedef union {
	struct {
		unsigned int cco_band:2;
		unsigned int mbz:20;
		unsigned int post_pi_sel5:5; //dq0~7
		unsigned int post_pi_sel4:5; //dq8~15
	} f;
	unsigned int v;
} SYS_MEM_PLL_CTRL1_T;
#define SYS_MEM_PLL_CTRL1rv (*((regval)0xb8000238))
#define SYS_MEM_PLL_CTRL1dv (0x400001EF)
#define RMOD_SYS_MEM_PLL_CTRL1(...) rset(SYS_MEM_PLL_CTRL1, SYS_MEM_PLL_CTRL1rv, __VA_ARGS__)
#define RIZS_SYS_MEM_PLL_CTRL1(...) rset(SYS_MEM_PLL_CTRL1, 0, __VA_ARGS__)
#define RFLD_SYS_MEM_PLL_CTRL1(fld) (*((const volatile SYS_MEM_PLL_CTRL1_T *)0xb8000238)).f.fld

typedef union {
	struct {
        unsigned int mbz0:2;
		unsigned int vco_ldo_vsel:2;
		unsigned int post_pi_rs:1;
		unsigned int pll_sel_cpmode:1;
		unsigned int post_pi_rl:2;
        unsigned int post_pi_bias:2;
        unsigned int pll_ldo_vsel:2;
        unsigned int pll_debug_enable:1;
        unsigned int lpf_sr:3;
        unsigned int pdiv:2;
        unsigned int lpf_cp:1;
        unsigned int cco_kvco:1;
        unsigned int icp:4;
        unsigned int loop_pi_isel:3;
        unsigned int mbz1:5;
	} f;
	unsigned int v;
} SYS_MEM_PLL_CTRL2_T;
#define SYS_MEM_PLL_CTRL2rv (*((regval)0xb800023C))
#define SYS_MEM_PLL_CTRL2dv (0x26622520)
#define RMOD_SYS_MEM_PLL_CTRL2(...) rset(SYS_MEM_PLL_CTRL2, SYS_MEM_PLL_CTRL2rv, __VA_ARGS__)
#define RIZS_SYS_MEM_PLL_CTRL2(...) rset(SYS_MEM_PLL_CTRL2, 0, __VA_ARGS__)
#define RFLD_SYS_MEM_PLL_CTRL2(fld) (*((const volatile SYS_MEM_PLL_CTRL2_T *)0xb800023C)).f.fld

typedef union {
	struct {
        unsigned int n_code:8;
		unsigned int pll_dum:8;
		unsigned int post_pi_sel:5;
        unsigned int mbz:5;
		unsigned int pll_sel_cpmode:6;
	} f;
	unsigned int v;
} SYS_MEM_PLL_CTRL3_T;
#define SYS_MEM_PLL_CTRL3rv (*((regval)0xb8000240))
#define SYS_MEM_PLL_CTRL3dv (0x160F0000)
#define RMOD_SYS_MEM_PLL_CTRL3(...) rset(SYS_MEM_PLL_CTRL3, SYS_MEM_PLL_CTRL3rv, __VA_ARGS__)
#define RIZS_SYS_MEM_PLL_CTRL3(...) rset(SYS_MEM_PLL_CTRL3, 0, __VA_ARGS__)
#define RFLD_SYS_MEM_PLL_CTRL3(fld) (*((const volatile SYS_MEM_PLL_CTRL3_T *)0xb8000240)).f.fld

typedef union {
	struct {
        unsigned int mbz0:10;
        unsigned int ph_sel:6;
        unsigned int mbz1:4;
		unsigned int clk_en:6;  // 20171110, Confirmed by Daniel Wu
		unsigned int clk_oe:6;
	} f;
	unsigned int v;
} SYS_MEM_PLL_CTRL4_T;
#define SYS_MEM_PLL_CTRL4rv (*((regval)0xb8000244))
#define SYS_MEM_PLL_CTRL4dv (0x00000FFF)
#define RMOD_SYS_MEM_PLL_CTRL4(...) rset(SYS_MEM_PLL_CTRL4, SYS_MEM_PLL_CTRL4rv, __VA_ARGS__)
#define RIZS_SYS_MEM_PLL_CTRL4(...) rset(SYS_MEM_PLL_CTRL4, 0, __VA_ARGS__)
#define RFLD_SYS_MEM_PLL_CTRL4(fld) (*((const volatile SYS_MEM_PLL_CTRL4_T *)0xb8000244)).f.fld

typedef union {
	struct {
        unsigned int weighting_sel:3;
        unsigned int step_in:13;
        unsigned int mbz:1;
        unsigned int sel_mode:1;
		unsigned int order:1;
		unsigned int f_code:13;
	} f;
	unsigned int v;
} SYS_MEM_PLL_CTRL5_T;
#define SYS_MEM_PLL_CTRL5rv (*((regval)0xb8000248))
#define SYS_MEM_PLL_CTRL5dv (0x00000000)
#define RMOD_SYS_MEM_PLL_CTRL5(...) rset(SYS_MEM_PLL_CTRL5, SYS_MEM_PLL_CTRL5rv, __VA_ARGS__)
#define RIZS_SYS_MEM_PLL_CTRL5(...) rset(SYS_MEM_PLL_CTRL5, 0, __VA_ARGS__)
#define RFLD_SYS_MEM_PLL_CTRL5(fld) (*((const volatile SYS_MEM_PLL_CTRL5_T *)0xb8000248)).f.fld

typedef union {
	struct {
        unsigned int mbz:18;
        unsigned int en_center_in:1;
        unsigned int bypass_pi:1;
        unsigned int tbase:12;
	} f;
	unsigned int v;
} SYS_MEM_PLL_CTRL6_T;
#define SYS_MEM_PLL_CTRL6rv (*((regval)0xb800024C))
#define SYS_MEM_PLL_CTRL6dv (0x00000000)
#define RMOD_SYS_MEM_PLL_CTRL6(...) rset(SYS_MEM_PLL_CTRL6, SYS_MEM_PLL_CTRL6rv, __VA_ARGS__)
#define RIZS_SYS_MEM_PLL_CTRL6(...) rset(SYS_MEM_PLL_CTRL6, 0, __VA_ARGS__)
#define RFLD_SYS_MEM_PLL_CTRL6(fld) (*((const volatile SYS_MEM_PLL_CTRL6_T *)0xb800024C)).f.fld

extern u32_t cg_guery_mem_freq(void);

#endif  //__MEM_PLL_H__
