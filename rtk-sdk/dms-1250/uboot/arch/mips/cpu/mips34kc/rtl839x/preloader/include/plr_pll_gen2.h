#ifdef __cplusplus
extern "C" {
#endif

#ifndef PLR_PLL_GEN2_H
#define PLR_PLL_GEN2_H

#define PLL_GLB_CTRL_A  (0xBB000024)
#define CPU_PLL_CTRL0_A (0xBB000028)
#define CPU_PLL_CTRL1_A (0xBB00002C)
#define LX_PLL_CTRL0_A  (0xBB000038)
#define LX_PLL_CTRL1_A  (0xBB00003C)
#define MEM_PLL_CTRL0_A (0xBB000048)
#define MEM_PLL_CTRL1_A (0xBB00004C)

#define PLL_GLB_CTRL_LXB_CLKSEL_FD_S                (13)
#define PLL_GLB_CTRL_LXB_CLKSEL_FIXED_MASK  (1 << PLL_GLB_CTRL_LXB_CLKSEL_FD_S)
#define PLL_GLB_CTRL_MEM_CLKSEL_FD_S                (12)
#define PLL_GLB_CTRL_MEM_CLKSEL_FIXED_MASK  (1 << PLL_GLB_CTRL_MEM_CLKSEL_FD_S)
#define PLL_GLB_CTRL_CPU_CLKSEL_FD_S                (11)
#define PLL_GLB_CTRL_CPU_CLKSEL_FIXED_MASK  (1 << PLL_GLB_CTRL_CPU_CLKSEL_FD_S)

#define OTTO_PLL_CPU_SET    (1 << 0)
#define OTTO_PLL_DSP_SET    (1 << 1)
#define OTTO_PLL_MEM_SET    (1 << 2)
#define OTTO_PLL_LX_SET     (1 << 3)

#define PLL_MODE_BY_PIN 0
#define PLL_MODE_BY_SW  1
typedef struct {
	u32_t mode;
	u32_t cpu;
	u32_t lx;
	u32_t mem;
} pll_gen2_mhz_t;

void otto_pll_gen2_set(unsigned int sys_cpu_pll_ctl0,
                       unsigned int sys_cpu_pll_ctl1,
                       unsigned int sys_mem_pll_ctl0,
                       unsigned int sys_mem_pll_ctl1,
                       unsigned int sys_lx_pll_ctl0,
                       unsigned int sys_lx_pll_ctl1,
                       unsigned int flag);

void pll_gen2_setup(void);
u32_t pll_query_freq(u32_t dev);

pll_result_t _pll_gen2_get_to_mhz(const pll_info_t *pll_reg,
                                  pll_gen2_mhz_t *pll_mhz);
pll_result_t _pll_gen2_set_from_mhz(pll_info_t *pll_reg,
                                    pll_gen2_mhz_t *pll_mhz);

pll_result_t pll_gen2_get_to_mhz(const pll_info_t *pll_reg,
                                 pll_gen2_mhz_t *pll_mhz);

#if (defined(PLR_ENABLE_PLL_SET) || \
     defined(CONFIG_STANDALONE_UBOOT))
pll_result_t pll_gen2_set_from_mhz(pll_info_t *pll_reg,
                                   pll_gen2_mhz_t *pll_mhz);
#endif

#endif /* #ifndef PLR_PLL_GEN2_H */

#ifdef __cplusplus
}
#endif
