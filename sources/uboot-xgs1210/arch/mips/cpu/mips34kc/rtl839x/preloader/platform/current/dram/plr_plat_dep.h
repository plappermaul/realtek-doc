#ifndef PLR_PLAT_DEP_H
#define PLR_PLAT_DEP_H
#include <soc.h>

void plat_mem_clk_rev_check(void);
//void plat_memctl_IO_PAD_patch(void);
void plat_memctl_dramclk_en(void);

/* CPU related (cache flush) */
void plat_memctl_dcache_flush(void);

/* PAD related (Pad driving, static ZQ setting) */
//void plat_memctl_dram_pad_drv(unsigned int drv_strength);
//void plat_memctl_dram_zq_setting(unsigned int is_static, unsigned int odt_ocd_ohm);


/* Platform clock related (CPU clock, LX bus clock, Memory clock) */
unsigned int plat_memctl_CPU_clock_MHz(void);
unsigned int plat_memctl_MEM_clock_MHz(void);
unsigned int plat_memctl_DSP_clock_MHz(void);
unsigned int plat_memctl_LX_clock_MHz(void);

/* Memory controller input signal delay (CLKM delay, CLKM_90 delay, TX delay) */
unsigned int plat_memctl_input_sig_delay(unsigned int first_param, unsigned int second_param, unsigned int third_param);
//void plat_memctl_choose_delay_value(unsigned int min_value, unsigned int max_value);
void plat_memctl_ZQ_model_en(void);
void plat_memctl_ZQ_force_config(void);
void plat_memctl_show_dram_config(void);
u32_t plat_memctl_calculate_dqrf_delay(u32_t max_w_seq_start, u32_t max_w_len, u32_t max_r_seq_start, u32_t max_r_len);

#endif /* #ifndef PLR_PLAT_DEP_H */
