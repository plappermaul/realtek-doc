#ifdef __OTTO_COMPOSER__
	#undef SECTION_ON_FLASH
	#define SECTION_ON_FLASH
#else
	#include <preloader.h>
#endif

#include <plr_pll_gen3.h>
#ifdef __cplusplus
extern "C" {
#endif

SECTION_ON_FLASH static u32_t
pll_gen3_query_ocp_mem_freq(const pll_gen3_info_t *info,
                            int is_ocp) {
	const u8_t divn3[] = {2, 3, 4, 6};
	reg_pll_cpu_mem_ctrl0 reg0;
	reg_pll_cpu_mem_ctrl1 reg1;
	u32_t f;

	if (is_ocp) {
		// cpu
		f=2;
		reg0.regVal = info->pll_cpu_ctl0;
		reg1.regVal = info->pll_cpu_ctl1;
	} else {
		// memory
		f=4;
		reg0.regVal = info->pll_mem_ctl0;
		reg1.regVal = info->pll_mem_ctl1;
	}

	return (25 *
	        (1 << reg0.bf.cmu_sel_prediv)*
	        ((reg0.bf.cmu_bypass_pi == 1) ? (reg0.bf.cmu_ncode_in + 4) : 0) *
	        ((reg0.bf.cmu_sel_div4 == 0) ? 1 : 4) /
	        (((reg1.bf.cmu_divn2_selb == 0) ? (reg0.bf.cmu_divn2 + 4) : divn3[reg1.bf.cmu_divn3_sel])*f)
	        );
}

#define pll_gen3_query_ocp_freq(i) pll_gen3_query_ocp_mem_freq(i, 1)
#define pll_gen3_query_mem_freq(i) pll_gen3_query_ocp_mem_freq(i, 0)
SECTION_ON_FLASH pll_result_t
_pll_gen3_get_to_mhz(const pll_info_t *pll_reg,
                     pll_gen3_mhz_t *pll_mhz) {
	pll_mhz->mode = PLL_MODE_BY_SW;

	pll_mhz->cpu  = pll_gen3_query_ocp_freq(pll_reg);
	pll_mhz->lx   = LX_CLK_MHZ;
	pll_mhz->mem  = pll_gen3_query_mem_freq(pll_reg);

	return PLL_RES_OK;
}

SECTION_ON_FLASH pll_result_t
_pll_gen3_set_from_mhz(pll_info_t *pll_reg,
                       const pll_gen3_mhz_t *pll_mhz) {
	u32_t cmu_fcode_in, cmu_divn2,cmu_ncode_in,cmu_bypass_pi,cmu_sel_div4,cmu_sel_prediv;
	u32_t cmu_divn3_sel,cmu_divn2_selb,cmu_en_ssc,cmu_step_in,cmu_tbase_in;
	u32_t cmu_ssc_order,cmu_time2_rst_width, cmu_time0_ck, cmu_clkrdy,cmu_big_kvco, cmu_lpf_rs,
		cmu_en_center_in, cmu_en_wd, cmu_pi_i_sel, cmu_sel_cp_i, cmu_sel_cco, cmu_ldo_sel,
		cmu_lpf_cp, cmu_cp_new_en, cmu_ldo_en, cmu_vc_dly, cmu_en_ckoobs, test_en;
	pll_result_t res = PLL_RES_OK;

	if (pll_mhz->mode == PLL_MODE_BY_SW) {
		pll_reg->set_by = 1;
	} else if (pll_mhz->mode == PLL_MODE_BY_PIN) {
		pll_reg->set_by = 0;
	} else {
		res = PLL_RES_BAD_MODE;
		return res;
	}

	//Calculate OCP PLL Register -------------------------------
	pll_reg->pll_cpu_ctl0 = pll_reg->pll_cpu_ctl1 = pll_reg->pll_cpu_misc_ctrl = 0;
	cmu_bypass_pi  = 0x1;
	cmu_en_ssc	   = 0x0;
	cmu_fcode_in   = 0x000;
	cmu_step_in    = 0x145;
	cmu_tbase_in   = 0x30E;
	cmu_ssc_order  = 0x0;
	cmu_time2_rst_width = 0x1;
	cmu_time0_ck   = 0x3;
	cmu_clkrdy	   = 0x1;
	cmu_lpf_rs	   = 0x2;
	cmu_en_center_in = 0x0;
	cmu_en_wd	   = 0x0;
	cmu_pi_i_sel   = 0x3;
	cmu_sel_cp_i   = 0x3;
	cmu_sel_cco    = 0x1;
	cmu_ldo_sel    = 0x6;
	cmu_lpf_cp	   = 0x1;
	cmu_cp_new_en  = 0x1;
	cmu_ldo_en	   = 0x1;
	cmu_vc_dly	   = 0x0;
	cmu_en_ckoobs  = 0x0;
	test_en 	   = 0x0;

	switch (pll_mhz->cpu){
	case 500:
		cmu_sel_prediv = 0;
		cmu_sel_div4   = 0x0;
		cmu_ncode_in   = 0x74;
		cmu_divn2	   = 0x4;
		cmu_divn3_sel  = 0x1;
		cmu_divn2_selb = 0x1;
		cmu_big_kvco   = 0x1;
		break;
	default:
		res = PLL_RES_FREQ_OUT_OF_RANGE;
	case 300:
		cmu_sel_prediv = 0;
		cmu_sel_div4   = 0x0;
		cmu_ncode_in   = 0x5C;
		cmu_divn2	   = 0x4;
		cmu_divn3_sel  = 0x2;
		cmu_divn2_selb = 0x1;
		cmu_big_kvco   = 0x0;
		break;
	}

	pll_reg->pll_cpu_ctl0 = (cmu_fcode_in<<20)|(cmu_divn2<<12)|(cmu_ncode_in<<4)|(cmu_bypass_pi<<3)|(cmu_sel_div4<<2)|(cmu_sel_prediv<<0);
	pll_reg->pll_cpu_ctl1 = (cmu_divn3_sel<<27)|(cmu_divn2_selb<<26)|(cmu_en_ssc<<25)|(cmu_step_in<<12)|(cmu_tbase_in<<0);
	pll_reg->pll_cpu_misc_ctrl = (cmu_ssc_order<<31)|(cmu_time2_rst_width<<29)|(cmu_time0_ck<<26)|(cmu_clkrdy<<24)|(cmu_big_kvco<<23)| \
		(cmu_lpf_rs<<20)|(cmu_en_center_in<<19)|(cmu_en_wd<<18)|(cmu_pi_i_sel<<14)|(cmu_sel_cp_i<<10)|(cmu_sel_cco<<9)|(cmu_ldo_sel<<6)| \
		(cmu_lpf_cp<<5)|(cmu_cp_new_en<<4)|(cmu_ldo_en<<3)|(cmu_vc_dly<<2)|(cmu_en_ckoobs<<1)|(test_en<<0);

	//Calculate MEM PLL Register -------------------------------
	pll_reg->pll_mem_ctl0 = pll_reg->pll_mem_ctl1 = pll_reg->pll_mem_misc_ctrl = 0;

	cmu_bypass_pi  = 0x1;
	cmu_en_ssc	   = 0x0;
	cmu_fcode_in   = 0x000;
	cmu_step_in    = 0x18;
	cmu_tbase_in   = 0xC80;
	cmu_ssc_order  = 0x0;
	cmu_time2_rst_width = 0x1;
	cmu_time0_ck   = 0x3;
	cmu_clkrdy	   = 0x1;
	cmu_lpf_rs	   = 0x2;
	cmu_en_center_in = 0x0;
	cmu_en_wd	   = 0x0;
	cmu_pi_i_sel   = 0x3;
	cmu_sel_cp_i   = 0x3;
	cmu_sel_cco    = 0x1;
	cmu_ldo_sel    = 0x6;
	cmu_lpf_cp	   = 0x1;
	cmu_cp_new_en  = 0x1;
	cmu_ldo_en	   = 0x1;
	cmu_vc_dly	   = 0x0;
	cmu_en_ckoobs  = 0x0;
	test_en 	   = 0x0;

	switch (pll_mhz->mem) {
	case 300:
		cmu_sel_prediv = 0;
		cmu_sel_div4   = 0x1;
		cmu_ncode_in   = 0x14;
		cmu_divn2      = 0x4;
		cmu_divn3_sel  = 0x0;
		cmu_divn2_selb = 0x1;
		cmu_big_kvco   = 0x0;
		break;
	default:
		res = PLL_RES_FREQ_OUT_OF_RANGE;
	case 192:
	case 193:
	case 194:
		cmu_sel_prediv = 0;
		cmu_sel_div4   = 0x1;
		cmu_ncode_in   = 0x1B;
		cmu_divn2      = 0x4;
		cmu_divn3_sel  = 0x2;
		cmu_divn2_selb = 0x1;
		cmu_big_kvco   = 0x1;
		break;
	}
	pll_reg->pll_mem_ctl0 = (cmu_fcode_in<<20)|(cmu_divn2<<12)|(cmu_ncode_in<<4)|(cmu_bypass_pi<<3)|(cmu_sel_div4<<2)|(cmu_sel_prediv<<0);
	pll_reg->pll_mem_ctl1 = (cmu_divn3_sel<<27)|(cmu_divn2_selb<<26)|(cmu_en_ssc<<25)|(cmu_step_in<<12)|(cmu_tbase_in<<0);
	pll_reg->pll_mem_misc_ctrl = (cmu_ssc_order<<31)|(cmu_time2_rst_width<<29)|(cmu_time0_ck<<26)|(cmu_clkrdy<<24)|(cmu_big_kvco<<23)| \
		(cmu_lpf_rs<<20)|(cmu_en_center_in<<19)|(cmu_en_wd<<18)|(cmu_pi_i_sel<<14)|(cmu_sel_cp_i<<10)|(cmu_sel_cco<<9)|(cmu_ldo_sel<<6)| \
		(cmu_lpf_cp<<5)|(cmu_cp_new_en<<4)|(cmu_ldo_en<<3)|(cmu_vc_dly<<2)|(cmu_en_ckoobs<<1)|(test_en<<0);

	return res;
}

#ifdef __cplusplus
}
#endif
