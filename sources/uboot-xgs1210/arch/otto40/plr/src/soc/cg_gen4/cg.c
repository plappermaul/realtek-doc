#include <soc.h>
#include <uart/uart.h>
#include <cg/cg.h>
#include <util.h>

#ifndef SECTION_CG
#define SECTION_CG
#endif
#ifndef SECTION_CG_INFO
#define SECTION_CG_INFO
#endif

static void reg_to_mhz(void);
cg_info_t cg_info_query SECTION_CG_INFO;
u32_t cg_status SECTION_CG_INFO;

SECTION_CG
u32_t cg_query_freq(u32_t dev_type) {
	if (0 == cg_info_query.dev_freq.ocp_mhz) {
		reg_to_mhz();
	}

	switch(dev_type){
	case CG_DEV_OCP:
		return cg_info_query.dev_freq.ocp_mhz;
	case CG_DEV_MEM:
		return cg_info_query.dev_freq.mem_mhz;
	case CG_DEV_LX:
		return cg_info_query.dev_freq.lx_mhz;
	case CG_DEV_SPIF:
		return cg_info_query.dev_freq.spif_mhz;
	default:
		return (u32_t)(-1);
	}
}

SECTION_CG
void cg_copy_info_to_sram(void) {
	inline_memcpy(&cg_info_query, &cg_info_proj, sizeof(cg_info_t));
}

/* [13:10]: clk_div in PLL - 5,
   [ 9: 7]: clk_div in NOR SPIF / 2,
   [ 6: 0]: freq */
#define NOR_SPIF_PLL_CFG(x, y, z) (((x-2) << 10) | (((y/2)-1) << 7) | z)
#define NOR_SPIF_CFG_F(x) ((u16_t)(x & 0x7f))

SECTION_CG
static u32_t _cg_calc_norsf(s16_t targ_freq,
                            cg_register_set_t *rs) {
	u16_t conf_nor_spif_freq, i;
	u16_t preset_spif_freq[] = {
		NOR_SPIF_PLL_CFG(17, 12, 5),
		NOR_SPIF_PLL_CFG(10, 10, 10),
		NOR_SPIF_PLL_CFG(11,  6, 15),
		NOR_SPIF_PLL_CFG(5,  10, 20),
		NOR_SPIF_PLL_CFG(5,   8, 25),
		NOR_SPIF_PLL_CFG(17,  2, 29),
		NOR_SPIF_PLL_CFG(7,   4, 36),
		NOR_SPIF_PLL_CFG(6,   4, 42),
		NOR_SPIF_PLL_CFG(11,  2, 45),
		NOR_SPIF_PLL_CFG(5,   4, 50),
		NOR_SPIF_PLL_CFG(3,   6, 56),
		NOR_SPIF_PLL_CFG(4,   4, 63),
		NOR_SPIF_PLL_CFG(7,   2, 71),
		NOR_SPIF_PLL_CFG(3,   4, 83),
		NOR_SPIF_PLL_CFG(5,   2, 100),
		NOR_SPIF_PLL_CFG(4,   2, 125),
	};
	/* preset_spif_freq =
	   [13:11]: clk_div in NOR SPIF / 2,
	   [10:7]: clk_div in PLL - 5,
	   [6:0]: freq */
	for (i=0; i<sizeof(preset_spif_freq)/sizeof(u16_t); i++) {
		if (targ_freq < NOR_SPIF_CFG_F(preset_spif_freq[i])) {
			goto proper_nor_spif_clk_found;
		}
	}
	i--;
 proper_nor_spif_clk_found:
	if ((i == 0) ||
	    ((NOR_SPIF_CFG_F(preset_spif_freq[i]) - targ_freq) <
	     (targ_freq - NOR_SPIF_CFG_F(preset_spif_freq[i-1])))) {
		conf_nor_spif_freq = preset_spif_freq[i];
	} else {
		conf_nor_spif_freq = preset_spif_freq[i-1];
	}
	rs->lx_clk_pll.f.spi_flash_clk_div = ((conf_nor_spif_freq >> 10) & 0xf);
	rs->sfcr.f.spi_clk_div = (conf_nor_spif_freq >> 7) & 0x7;

	return 0;
}

SECTION_CG
static u32_t _cg_calc_lx(s16_t targ_freq,
                         cg_register_set_t *rs) {
	u32_t divisor = (1000 / targ_freq) - 2;
	u32_t cg_status = 0;
	if ((divisor < 1) || (divisor > 17)) {
		cg_status |= CG_LX_WRONG;
	}
	rs->lx_clk_pll.f.lxpll_div = divisor;
	/* printf("DD: LX: %dMHz, divisor: %d\n", df->lx_mhz, divisor); */
	return cg_status;
}

SECTION_CG
static u32_t _cg_calc_mem(s16_t targ_freq,
                          cg_register_set_t *rs) {
	u32_t factor, divisor;
	u32_t cg_status = 0;

	if (targ_freq < 100) {
		/* 1 << '2' ==> 4 */
		divisor = 2;
	} else if (targ_freq < 200) {
		/* 1 << '1' ==> 2 */
		divisor = 1;
	} else {
		divisor = 0;
	}

	factor = ((targ_freq * 2 * (1 << divisor)) / 25) - 2;
	if ((factor < 14) || (factor > 30)) {
		cg_status = CG_MEM_WRONG;
	}
	rs->syspllctr.f.sdpll_div = factor;
	rs->mckg_div.f.mckg_divl = divisor;
	/* printf("DD: MEM: %dMHz, divisor: %d\n", df->mem_mhz, divisor); */
	return cg_status;
}

SECTION_CG
static u32_t _cg_calc_ocp(s16_t targ_freq,
                          cg_register_set_t *rs) {
	u32_t div_lmt, i;
	u32_t cg_status = 0, conf_div = 0;
	s16_t pll_out, pll_out_max;
	s16_t diff_targ_conf = 0x7fff, conf_pll_out = 400, curr_freq = 400;

	if (_soc.cid == 0x6422) {
		div_lmt = 9;
		pll_out_max = 525;
	} else {
		div_lmt = 7;
		pll_out_max = 600;
	}

	for (pll_out = 400; pll_out <= pll_out_max; pll_out += 25) {
		for (i = 0; i <= div_lmt; i++) {
			curr_freq = pll_out / (1 << i);
			if (curr_freq <= targ_freq) {
				if ((targ_freq - curr_freq) < diff_targ_conf) {
					diff_targ_conf = (targ_freq - curr_freq);
					conf_pll_out = pll_out;
					conf_div = i;
				}
			}
		}
	}

	rs->syspllctr.f.ocp0_pll_div = (conf_pll_out / 25) - 2;
	rs->pll2.v = PLL2dv;
	if (_soc.cid == 0x6422) {
		if (conf_div > 2) {
			rs->pll2.f.en_ocp_div = 2;
		} else {
			rs->pll2.f.en_ocp_div = conf_div;
		}
		conf_div -= rs->pll2.f.en_ocp_div;
	} else if (_soc.cid == 0x0639) {
		rs->pll2.f.en_ocp_div = 2;
	}

	rs->cmugcr.f.oc0_freq_div = conf_div;
	if (conf_div != 0) {
		rs->cmugcr.f.cmu_mode = 1;	/* set to CMU fixed mode. */
	}else {
		rs->cmugcr.f.cmu_mode = 0;	/* set to CMU disabled mode. */
	}

	/* printf("DD: OCP: targ: %dMHz, pll_out: %d, plldiv: %d, cmudiv: %d\n", */
	/*        targ_freq, */
	/*        conf_pll_out, */
	/*        _soc.cid == 0x6422? (1 << rs->pll2.f.en_ocp_div): 1, */
	/*        1 << conf_div); */
	return cg_status;
}

SECTION_CG
void cg_xlat_n_assign(void) {
	cg_dev_freq_t *df     = &cg_info_query.dev_freq;
	cg_register_set_t *rs = &cg_info_query.register_set;

	cg_status = 0;

	cg_status |= _cg_calc_ocp(df->ocp_mhz, rs);
	cg_status |= _cg_calc_mem(df->mem_mhz, rs);
	cg_status |= _cg_calc_lx(df->lx_mhz, rs);
	cg_status |= _cg_calc_norsf(df->spif_mhz, rs);

	return;
}

SECTION_CG
static u32_t cg_udelay(u32_t us, u32_t mhz) {
	u32_t loop_cnt = us*mhz/2;
	while (loop_cnt--) {
		;
	}
	return loop_cnt;
}

SECTION_CG
static void reg_to_mhz(void) {
#ifndef FPGA_ENVIRONMENT
	cg_info_query.dev_freq.ocp_mhz = (RFLD_SYSPLLCTR(ocp0_pll_div) + 2) * 25;
	cg_info_query.dev_freq.ocp_mhz /= (1 << RFLD_CMUGCR(oc0_freq_div));
	if (_soc.cid == 0x6422) {
		cg_info_query.dev_freq.ocp_mhz /= (1 << RFLD_PLL2(en_ocp_div));
	}

	cg_info_query.dev_freq.mem_mhz = ((RFLD_SYSPLLCTR(sdpll_div) + 2) * 25) >> 1;
	cg_info_query.dev_freq.mem_mhz >>= RFLD_MCKG_DIV(mckg_divl);
	cg_info_query.dev_freq.lx_mhz  = 1000 / (RFLD_LX_CLK_PLL(lxpll_div) + 2);

	cg_info_query.dev_freq.spif_mhz = (RFLD_SFCR(spi_clk_div) + 1)*2;
	cg_info_query.dev_freq.spif_mhz = 1000 / (RFLD_LX_CLK_PLL(spi_flash_clk_div) + 2) / cg_info_query.dev_freq.spif_mhz;
#endif
}

SECTION_CG
void cg_init(void) {
	cg_dev_freq_t *df     = &cg_info_query.dev_freq;
	cg_register_set_t *rs = &cg_info_query.register_set;
	u32_t pll_spif;

	/* 1: Enable DRAM clock de-glitch. */
	RMOD_DRAM_CLK_CHG(dram_clk_dg_en, 1);
	cg_udelay(1, 400);

	/* 2: Switch OCP to LX clock. */
	RMOD_SYS_STATUS(cf_ckse_ocp0, 0);
	cg_udelay(5, 200);

	/* 3: Change OCP and MEM divisors. */
	PLL2rv = rs->pll2.v;
	SYSPLLCTRrv = rs->syspllctr.v;
	MCKG_DIVrv = rs->mckg_div.v;
	cg_udelay(5, 200);

	/* 4. Switch OCP to original clock. */
	RMOD_SYS_STATUS(cf_ckse_ocp0, 1);
	cg_udelay(5, 200);

	/* 5. Disable DRAM clock de-glitch. */
	RMOD_DRAM_CLK_CHG(dram_clk_dg_en, 0);

	/* 6. Switch LX to 100MHz (RTL86865S) or 1/8 OCP clock. (RTL9601B) */
	RMOD_SYS_STATUS(cf_cksel_lx, 0);
	cg_udelay(5, df->ocp_mhz);

	/* 7. Change LX and SPIF (PLL) divisors.*/
	LX_CLK_PLLrv = rs->lx_clk_pll.v;
	RMOD_SFCR(spi_clk_div, rs->sfcr.f.spi_clk_div);
	cg_udelay(5, df->ocp_mhz);

	/* 8. Switch LX to original clock and done. */
	RMOD_SYS_STATUS(cf_cksel_lx, 1);

	/* 9. Apply CMU divisor. */
	CMUGCRrv = rs->cmugcr.v;

	reg_to_mhz();

	/* 10. Check for LX/OCP to DRAM slow bits. */
	rs->cmugcr.f.lx0_se_dram = ((s32_t)df->lx_mhz - (s32_t)df->mem_mhz - 1) >> 31;
	rs->cmugcr.f.lx1_se_dram = rs->cmugcr.f.lx0_se_dram;
	rs->cmugcr.f.lx2_se_dram = rs->cmugcr.f.lx0_se_dram;

	rs->cmugcr.f.oc0_se_dram = ((s32_t)df->ocp_mhz - (s32_t)df->mem_mhz - 1) >> 31;
	rs->cmuo0cr.f.oc0_se_dram_wk  = rs->cmugcr.f.oc0_se_dram;
	rs->cmuo0cr.f.oc0_se_dram_slp = rs->cmugcr.f.oc0_se_dram;

	rs->cmuo0cr.f.oc0_auto_bz = 1;
	rs->cmuo0cr.f.oc0_spif_hs = 1;
	rs->cmuo0cr.f.oc0_dram_hs = 1;

	/* 11. Check for OCP to SPIF slow bits. */
	pll_spif = (df->spif_mhz * (rs->sfcr.f.spi_clk_div + 1) * 2);
	rs->cmuo0cr.f.oc0_se_spif = ((s32_t)df->ocp_mhz - (s32_t)pll_spif - 1) >> 31;
	rs->cmuo0cr.f.oc0_se_spif_wk = rs->cmuo0cr.f.oc0_se_spif;
	rs->cmuo0cr.f.oc0_se_spif_slp = rs->cmuo0cr.f.oc0_se_spif;

	CMUO0CRrv = rs->cmuo0cr.v;
	CMUGCRrv = rs->cmugcr.v;

	uart_init(115200, df->lx_mhz);

	return;
}

SECTION_CG
static void _cg_status_decode(void) {
	const s8_t *bus_name[] = {"OCP", "MEM", "LX", "SPIF"};
	const u32_t bus_mask[] = {CG_OCP_WRONG, CG_MEM_WRONG, CG_LX_WRONG, CG_SPIF_WRONG};
	u32_t i;

	for (i=0; i<(sizeof(bus_mask)/sizeof(bus_mask[0])); i++) {
		if (cg_status & bus_mask[i]) {
			printf("WW: %s PLL works beyond spec.\n", bus_name[i]);
		}
	}
	return;
}

SECTION_CG
void cg_result_decode(void) {
	reg_to_mhz();

	_cg_status_decode();

	printf("II: OCP %dMHz(%d/P%d/C%d), MEM %dMHz(%d/(2*%d)), LX %dMHz,"
	       " SPIF %dMHz(1000/(%d*%d))\n",
	       /* OCP */
	       cg_info_query.dev_freq.ocp_mhz,
	       (RFLD_SYSPLLCTR(ocp0_pll_div) + 2) * 25,
	       _soc.cid == 0x6422? (1 << RFLD_PLL2(en_ocp_div)): 1,
	       1 << RFLD_CMUGCR(oc0_freq_div),
	       /* MEM */
	       cg_info_query.dev_freq.mem_mhz,
	       (RFLD_SYSPLLCTR(sdpll_div) + 2) * 25,
	       1 << RFLD_MCKG_DIV(mckg_divl),
	       /* LX */
	       cg_info_query.dev_freq.lx_mhz,
	       /* NOR SPIF */
	       cg_info_query.dev_freq.spif_mhz,
	       (RFLD_LX_CLK_PLL(spi_flash_clk_div) + 2),
	       (RFLD_SFCR(spi_clk_div) + 1) * 2);
	return;
}

REG_INIT_FUNC(cg_copy_info_to_sram, 15);
REG_INIT_FUNC(cg_xlat_n_assign, 17);
REG_INIT_FUNC(cg_init, 19);
REG_INIT_FUNC(cg_result_decode, 20);
