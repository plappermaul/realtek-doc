#ifdef __cplusplus
extern "C" {
#endif

#ifndef PLR_PLL_GEN3_H
#define PLR_PLL_GEN3_H

#define LX_CLK_MHZ    (200)

/* PLL Gen3 register address*/
#define REG_PLL_GLB_CTRL      (0xBB000FC0)
#define REG_PLL_CPU_CTRL0     (0xBB000FC4)
#define REG_PLL_CPU_CTRL1     (0xBB000FC8)
#define REG_PLL_CPU_MISC_CTRL (0xBB000FCC)
#define REG_PLL_MEM_CTRL0     (0xBB000FDC)
#define REG_PLL_MEM_CTRL1     (0xBB000FE0)
#define REG_PLL_MEM_MISC_CTRL (0xBB000FE4)

/* RTL8380's strapped-pin register address*/
//#define REG_STRAP_DBG         (0xBB00100C)
//#define REG_INT_RW_CTRL       (0xBB000058)  



/* PLL Gen3 global control register bit-field shift*/
#define BF_CPU_PLL_EN     (0)
#define BF_MEM_PLL_EN     (2)
#define BF_CPU_PLL_RDY    (8)
#define BF_MEM_PLL_RDY    (10)
#define BF_PLL_SC_MUX     (12)
#define BF_CLKM_INI       (13)

/* PLL Gen3 global control register bit-field mask definition */
#define PLL_GLB_CTRL_CPU_PLL_EN_MASK   (1 << BF_CPU_PLL_EN)

#define PLL_GLB_CTRL_MEM_PLL_EN_MASK   (1 << BF_MEM_PLL_EN)

#define PLL_GLB_CTRL_OCP_PLL_RDY_MASK  (1 << BF_CPU_PLL_RDY)
#define PLL_GLB_CTRL_MEM_PLL_RDY_MASK  (1 << BF_MEM_PLL_RDY)

#define PLL_GLB_CTRL_CPU_PLL_SRC_LX    (0 << BF_PLL_SC_MUX)
#define PLL_GLB_CTRL_CPU_PLL_SRC_CPU   (1 << BF_PLL_SC_MUX)

#define PLL_GLB_CTRL_MEM_CHIP_CLK      (1 << BF_CLKM_INI)

#define OTTO_PLL_CPU_SET    (1 << 0)
#define OTTO_PLL_DSP_SET    (1 << 1)
#define OTTO_PLL_MEM_SET    (1 << 2)
#define OTTO_PLL_LX_SET     (1 << 3)

typedef union{
	struct { 
		unsigned cmu_fcode_in:12;
		unsigned cmu_divn2:8;
		unsigned cmu_ncode_in:8; 
		unsigned cmu_bypass_pi:1; 
		unsigned cmu_sel_div4:1;
		unsigned cmu_sel_prediv:2;
	}bf;//bit field 
	u32_t regVal; //real value read from register
}reg_pll_cpu_mem_ctrl0;

typedef union{
	struct { 
		unsigned reserved:3; 
		unsigned cmu_divn3_sel:2; 
		unsigned cmu_divn2_selb:1; 
		unsigned cmu_en_ssc:1; 
		unsigned cmu_step_in:13; 
		unsigned cmu_tbase_in:12; 
	}bf; //bit field 
	u32_t regVal; //real value read from register
}reg_pll_cpu_mem_ctrl1;

#define PLL_MODE_BY_PIN 0
#define PLL_MODE_BY_SW  1
typedef struct {
	u32_t mode;
	u32_t cpu;
	u32_t lx;
	u32_t mem;
} pll_gen3_mhz_t;

void pll_gen3_setup(void);
u32_t pll_query_freq(u32_t dev);

pll_result_t _pll_gen3_get_to_mhz(const pll_info_t *pll_reg,
                                  pll_gen3_mhz_t *pll_mhz);
pll_result_t _pll_gen3_set_from_mhz(pll_info_t *pll_reg,
                                    const pll_gen3_mhz_t *pll_mhz);

pll_result_t pll_gen3_get_to_mhz(const pll_info_t *pll_reg,
                                 pll_gen3_mhz_t *pll_mhz);

#if (defined(PLR_ENABLE_PLL_SET) || \
     defined(CONFIG_STANDALONE_UBOOT))
pll_result_t pll_gen3_set_from_mhz(pll_info_t *pll_reg,	\
                                   pll_gen3_mhz_t *pll_mhz);
#endif 

#endif /* #ifndef PLR_PLL_GEN3_H */

#ifdef __cplusplus
}
#endif
