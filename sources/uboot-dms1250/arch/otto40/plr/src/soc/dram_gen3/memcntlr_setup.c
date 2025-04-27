#include <soc.h>
#include <dram/memcntlr.h>
#include <util.h>

static mc_dram_param_t   dram_param MEMCNTLR_DATA_SECTION;
static mc_cntlr_opt_t    cntlr_opt MEMCNTLR_DATA_SECTION;
static mc_register_set_t register_set MEMCNTLR_DATA_SECTION;
static mc_register_set_t default_rs MEMCNTLR_DATA_SECTION;

mc_info_t meminfo MEMCNTLR_DATA_SECTION = {
	&dram_param, &cntlr_opt, &register_set
};

#include <dram/memcntlr_ag.h>

MEMCNTLR_SECTION
void dram_dump_info(void) {
	mc_dram_param_dump();
	mc_cntlr_opt_dump();
	mc_register_set_dump();
	return;
}

MEMCNTLR_SECTION
__attribute__((weak))
const mc_info_t *
proj_mc_info_select(void) {
	return (mc_info_t *)1;
}

MEMCNTLR_SECTION
void mc_info_probe(void) {
	const mc_info_t *mi;

	mi = proj_mc_info_select();
	ISTAT_SET(probe, MEM_PROBE_UNINIT);

	if (mi == VZERO) {
		ISTAT_SET(probe, MEM_PROBE_FAIL);
	} else if (((u32_t)mi) == 1) {
		ISTAT_SET(probe, MEM_PROBE_FUNC_MISSING);
	} else {
		inline_memcpy(&dram_param,   mi->dram_param,   sizeof(mc_dram_param_t));
		inline_memcpy(&cntlr_opt,    mi->cntlr_opt,    sizeof(mc_cntlr_opt_t));
		inline_memcpy(&register_set, mi->register_set, sizeof(mc_register_set_t));
		inline_memcpy(&default_rs,   mi->register_set, sizeof(mc_register_set_t));

		_soc.dram_info = (void *)&meminfo;

		ISTAT_SET(probe, MEM_PROBE_OK);
	}

	return;
}

#define NS2CK(ns, tck_ps) ((ns * 1000 + (tck_ps - 1)) / tck_ps)
#define XNA(fld) MEMCNTLR_SECTION void mc_xlat_n_assign_##fld(mc_dram_param_t *dp, mc_cntlr_opt_t *co, mc_register_set_t *rs)

XNA(dram_type) {
	if ((dp->dram_type != 1) &&
	    (dp->dram_type != 2) &&
	    (dp->dram_type != 3)) {
		ISTAT_SET(xlat, MEM_DRAM_TYPE_ERROR);
	} else {
		rs->mcr.f.dram_type = dp->dram_type - 1;
	}
	return;
}

XNA(bankcnt) {
	if ((dp->bankcnt != 2) &&
	    (dp->bankcnt != 4) &&
	    (dp->bankcnt != 8)) {
		ISTAT_SET(xlat, MEM_BANK_NUMBER_ERROR);
	} else {
		rs->dcr.f.bankcnt  = dp->bankcnt >> 2;
	}
	return;
}

XNA(buswidth) {
	if ((dp->buswidth != 8)  &&
	    (dp->buswidth != 16) &&
	    (dp->buswidth != 32)) {
		ISTAT_SET(xlat, MEM_BUS_WIDTH_ERROR);
	} else {
		rs->dcr.f.dbuswid  = dp->buswidth >> 4;
	}
	return;
}

XNA(max_row_addr) {
	if ((dp->max_row_addr < 10) ||
	    (dp->max_row_addr > 15)) {
		ISTAT_SET(xlat, MEM_ROW_NUM_ERROR);
	} else {
		rs->dcr.f.rowcnt   = dp->max_row_addr - 10;
	}
	return;
}

XNA(max_col_addr) {
	if ((dp->max_col_addr > 6) &&
	    (dp->max_col_addr < 10)) {
		rs->dcr.f.colcnt = dp->max_col_addr - 7;
	} else if (dp->max_col_addr == 11) {
		rs->dcr.f.colcnt = dp->max_col_addr - 8;
	} else if (dp->max_col_addr == 13) {
		rs->dcr.f.colcnt = dp->max_col_addr - 9;
	} else {
		ISTAT_SET(xlat, MEM_COL_NUM_ERROR);
	}
	return;
}

XNA(cl_ck) {
	/* For controller. */
	if ((dp->cl_ck < 2) ||
	    (dp->cl_ck > 11)) {
		ISTAT_SET(xlat, MEM_CL_ERROR);
		return;
	} else {
		rs->dtr0.f.t_cas     = dp->cl_ck - 1;
		rs->dtr0.f.t_cas_phy = dp->cl_ck - 1;
	}

	/* For DRAM chip. */
	if (dp->dram_type == 2) {
		if ((dp->cl_ck < 2) || (dp->cl_ck > 7)) {
			ISTAT_SET(xlat, MEM_MR0_ERROR);
		} else {
			ddr2_mr0 mr0 = { .v = rs->mr[0].f.mr_data };

			mr0.f.cl = dp->cl_ck;

			rs->mr[0].f.mr_data = mr0.v;
		}
	} else if (dp->dram_type == 3) {
		if ((dp->cl_ck < 5) || (dp->cl_ck > 16)) {
			ISTAT_SET(xlat, MEM_MR0_ERROR);
		} else {
			ddr3_mr0 mr0 = { .v = rs->mr[0].f.mr_data };

			mr0.f.cl_h = (dp->cl_ck - 4) & 0x7;
			mr0.f.cl_l = (dp->cl_ck - 4) >> 3;

			rs->mr[0].f.mr_data = mr0.v;
		}
	} else {
		ISTAT_SET(xlat, MEM_MR0_ERROR);
	}
	return;
}

XNA(cwl_ck) {
	/* Controller side. */
	if ((dp->cwl_ck < 1) ||
	    (dp->cwl_ck > 8)) {
		ISTAT_SET(xlat, MEM_CWL_ERROR);
		return;
	} else {
		rs->dtr0.f.t_cwl = dp->cwl_ck - 1;
	}

	/* For DRAM chip. */
	if (dp->dram_type == 2) {
		/* DDR2 needs not this parameter. */
	} else if (dp->dram_type == 3) {
		if ((dp->cwl_ck < 5) || (dp->cwl_ck > 12)) {
			ISTAT_SET(xlat, MEM_MR2_ERROR);
		} else {
			ddr3_mr2 mr2 = { .v = rs->mr[2].f.mr_data };

			mr2.f.cwl = dp->cwl_ck - 5;

			rs->mr[2].f.mr_data = mr2.v;
		}
	} else {
		ISTAT_SET(xlat, MEM_MR2_ERROR);
	}
	return;
}

XNA(min_trtp_ns) {
	u32_t tck_ps = 1000 * 1000 / GET_MEM_MHZ();

	rs->dtr0.f.t_rtp = NS2CK(dp->min_trtp_ns, tck_ps) - 1;
	return;
}

XNA(min_twtr_ns) {
	u32_t tck_ps = 1000 * 1000 / GET_MEM_MHZ();

	rs->dtr0.f.t_wtr = NS2CK(dp->min_twtr_ns, tck_ps) - 1;
	return;
}

XNA(min_twr_ns) {
	u32_t tck_ps = 1000 * 1000 / GET_MEM_MHZ();
	u32_t wr_ck = NS2CK(dp->min_twr_ns, tck_ps);

	if ((wr_ck - 1) > 7) {
		ISTAT_SET(xlat, MEM_WR_ERROR);
		return;
	} else {
		rs->dtr0.f.t_wr = wr_ck - 1;
	}

	/* for mr0 */
	if (dp->dram_type == 2) {
		ddr2_mr0 mr0 = { .v = rs->mr[0].f.mr_data };

		if (rs->dtr0.f.t_wr < 1) {
			rs->dtr0.f.t_wr = 1;
		}

		mr0.f.wr = rs->dtr0.f.t_wr;

		rs->mr[0].f.mr_data = mr0.v;
	} else if (dp->dram_type == 3) {
		ddr3_mr0 mr0 = { .v = rs->mr[0].f.mr_data };

		if (rs->dtr0.f.t_wr < 4) {
			rs->dtr0.f.t_wr = 4;
		}

		mr0.f.wr = rs->dtr0.f.t_wr + 1;
		mr0.f.wr = (mr0.f.wr > 9) ? (mr0.f.wr / 2) : (mr0.f.wr - 4);

		rs->mr[0].f.mr_data = mr0.v;
	} else {
		ISTAT_SET(xlat, MEM_WR_ERROR);
	}
	return;
}

XNA(max_trefi_ns) {
	u32_t max_trefi_ck, trefi_fac, trefi_unit;

	max_trefi_ck = (dp->max_trefi_ns * GET_MEM_MHZ()) / 1000;

	trefi_fac = 17;
	for (trefi_unit = 0; trefi_unit < 7; trefi_unit++) {
		trefi_fac = max_trefi_ck / (1 << (trefi_unit + 5));
		if (trefi_fac <= 16) {
			break;
		}
	}
	if ((trefi_fac > 16) || (trefi_fac == 0)) {
		ISTAT_SET(xlat, MEM_TREFI_ERROR);
	}
	rs->dtr0.f.t_refi      = trefi_fac - 1;
	rs->dtr0.f.t_refi_unit = trefi_unit;
	return;
}

XNA(min_trp_ns) {
	u32_t tck_ps = 1000 * 1000 / GET_MEM_MHZ();

	rs->dtr1.f.t_rp = NS2CK(dp->min_trp_ns,  tck_ps) - 1;
	return;
}

XNA(min_trcd_ns) {
	u32_t tck_ps = 1000 * 1000 / GET_MEM_MHZ();

	rs->dtr1.f.t_rcd = NS2CK(dp->min_trcd_ns, tck_ps) - 1;
	return;
}

XNA(min_trrd_ns) {
	u32_t tck_ps = 1000 * 1000 / GET_MEM_MHZ();

	rs->dtr1.f.t_rrd = NS2CK(dp->min_trrd_ns, tck_ps) - 1;
	return;
}

XNA(min_tfaw_ns) {
	u32_t tck_ps = 1000 * 1000 / GET_MEM_MHZ();

	rs->dtr1.f.t_fawg = NS2CK(dp->min_tfaw_ns, tck_ps) - 1;
	return;
}

XNA(min_trfc_ns) {
	u32_t tck_ps = 1000 * 1000 / GET_MEM_MHZ();

	rs->dtr2.f.t_rfc = NS2CK(dp->min_trfc_ns, tck_ps) - 1;
	return;
}

XNA(min_tras_ns) {
	u32_t tck_ps = 1000 * 1000 / GET_MEM_MHZ();

	rs->dtr2.f.t_ras = NS2CK(dp->min_tras_ns, tck_ps) - 1;
	return;
}

XNA(min_tck_ps) {
	u32_t tck_ps = 1000 * 1000 / GET_MEM_MHZ();

	if (tck_ps < dp->min_tck_ps) {
		ISTAT_SET(xlat, MEM_TCK_ERROR);
	}
	return;
}

XNA(max_tck_ps) {
	u32_t tck_ps = 1000 * 1000 / GET_MEM_MHZ();

	if (tck_ps > dp->max_tck_ps) {
		ISTAT_SET(xlat, MEM_TCK_ERROR);
	}
	return;
}

XNA(dqs0_en_hclk) {
	rs->dider.f.dqs0_en_hclk = co->dqs0_en_hclk;
	return;
}

XNA(dqs1_en_hclk) {
	rs->dider.f.dqs1_en_hclk = co->dqs1_en_hclk;
	return;
}

XNA(fast_exit) {
	/* fast_exit use the same bit in both DDR2 and DDR3 MR0. */
	ddr3_mr0 mr0 = { .v = rs->mr[0].f.mr_data };

	if (dp->dram_type == 2) {
		mr0.f.fast_exit_en = 1 - co->fast_exit;
	} else if (dp->dram_type == 3) {
		mr0.f.fast_exit_en = co->fast_exit;
	}

	rs->mr[0].f.mr_data = mr0.v;
	return;
}

XNA(normal_drv) {
	if (dp->dram_type == 2) {
		ddr2_emr1 mr1 = { .v = rs->mr[1].f.mr_data };
		mr1.f.output_drv_imp_ctl = 1 - co->normal_drv;
		rs->mr[1].f.mr_data = mr1.v;
	} else if (dp->dram_type == 3) {
		ddr3_mr1 mr1 = { .v = rs->mr[1].f.mr_data };
		mr1.f.output_drv_imp_ctl_l = co->normal_drv;
		rs->mr[1].f.mr_data = mr1.v;
	}

	return;
}

XNA(rtt_nom_ohm) {
	const u32_t rttnom = co->rtt_nom_ohm;

	if (dp->dram_type == 2) {
		ddr2_emr1 mr1 = { .v = rs->mr[1].f.mr_data };

		switch (rttnom) {
		case 0:
			mr1.f.rtt_nom_m = 0;
			mr1.f.rtt_nom_l = 0;
			rs->mr[1].f.mr_data = mr1.v;
			break;
		case 50:
		case 75:
		case 150:
			mr1.f.rtt_nom_m = ~((rttnom >> 3) | 0xfffffffe);
			mr1.f.rtt_nom_l = ~((rttnom >> 2) | 0xfffffffe);
			rs->mr[1].f.mr_data = mr1.v;
			break;
		default:
			ISTAT_SET(xlat, MEM_MR1_ERROR);
		}
	} else if (dp->dram_type == 3) {
		ddr3_mr1 mr1 = { .v = rs->mr[1].f.mr_data };

		switch (rttnom) {
		case 0:
			mr1.f.rtt_nom_h = 0;
			mr1.f.rtt_nom_m = 0;
			mr1.f.rtt_nom_l = 0;
			rs->mr[1].f.mr_data = mr1.v;
			break;
		case 20:
		case 30:
		case 40:
		case 60:
		case 120:
			mr1.f.rtt_nom_h = (rttnom <= 30)     ? 1 : 0;
			mr1.f.rtt_nom_m = (rttnom % 40) == 0 ? 1 : 0;
			mr1.f.rtt_nom_l = ((rttnom > 20) && (rttnom < 120)) ? 1 : 0;
			rs->mr[1].f.mr_data = mr1.v;
			break;
		default:
			ISTAT_SET(xlat, MEM_MR1_ERROR);
		}
	}

	return;
}

XNA(rtt_wr_ohm) {
	const u32_t rttwr = co->rtt_wr_ohm;

	if (dp->dram_type == 2) {
		/* do nothing. */
	} else if (dp->dram_type == 3) {
		if (((rttwr % 60) != 0) ||
		    (rttwr > 120)) {
			ISTAT_SET(xlat, MEM_MR2_ERROR);
		} else {
			ddr3_mr2 mr2 = { .v = rs->mr[2].f.mr_data };

			mr2.f.rtt_wr = (rttwr / 60);

			rs->mr[2].f.mr_data = mr2.v;
		}
	}

	return;
}

XNA(cntlr_odt) {
	const u8_t magic_zq[29] = {
		/* 0     1     2     3     4     5     6     7 */
		0x00, 0x48, 0x7c, 0x04, 0x20, 0x21, 0x00, 0x00,
		/* 8     9    10    11    12    13    14    15 */
		0x00, 0x00, 0x11, 0x41, 0x00, 0x31, 0x00, 0x0a,
		/*16    17    18    19    20    21    22    23 */
		0x00, 0x00, 0x00, 0x7e, 0x00, 0x3e, 0x30, 0x49,
		/*24    25    26    27    28 */
		0x00, 0x00, 0x19, 0x00, 0x00
	};
	u32_t zq_val[2], idx, i;

	zq_val[0] = co->cntlr_odt * 2;
	zq_val[1] = co->cntlr_ocd;
	/* ocd: 48/50/60/80/100/120/150/160/200/300/320/400/960/1200
	   idx: 19/21/ 2/22/ 13/  4/  5/ 15/ 26/ 10/  1/ 23/  3/  11 */
	for (i=0; i<2; i++) {
		idx = zq_val[i] % 29;				/* Ask no why, 29 is just a magic number. */
		zq_val[i] = magic_zq[idx];
		if (zq_val[i] == 0) {
			ISTAT_SET(xlat, MEM_CNTLR_ODT_OCD_ERROR);
		}
	}
	rs->ddzqpcr.f.zprog = ((zq_val[0] << 7) | zq_val[1]);
	return;
}

XNA(cntlr_ocd) {
	/* this field is XnA'd along with cntlr_odt. */
	return;
}

XNA(tx_delay) {
	rs->drcko.f.ddrck_phs_sel = co->tx_delay;
	return;
}

XNA(clkm_delay) {
	rs->drcko.f.ddrckm_tap = co->clkm_delay;
	return;
}

XNA(clkm90_delay) {
	rs->drcko.f.ddrckm90_tap = co->clkm90_delay;
	return;
}

XNA(dqs0_en_tap) {
	rs->dider.f.dqs0_en_tap = co->dqs0_en_tap;
	return;
}

XNA(dqs1_en_tap) {
	rs->dider.f.dqs1_en_tap = co->dqs1_en_tap;
	return;
}

XNA(dqs0_group_tap) {
	rs->daccr.f.dqs0_group_tap = co->dqs0_group_tap;
	return;
}

XNA(dqs1_group_tap) {
	rs->daccr.f.dqs1_group_tap = co->dqs1_group_tap;
	return;
}

XNA(dqm0_tap) {
	rs->dcdqmr.f.dqm0_phase_shift_90 = co->dqm0_tap;
	return;
}

XNA(dqm1_tap) {
	rs->dcdqmr.f.dqm1_phase_shift_90 = co->dqm1_tap;
	return;
}

XNA(wr_dq_delay) {
	u32_t i;

	for (i=0; i<16; i++) {
		rs->dacdq_rr[i].f.dqn_phase_shift_90 = co->wr_dq_delay[i];
	}

	return;
}

XNA(rdr_dq_delay) {
	u32_t i;

	for (i=0; i<16; i++) {
		rs->dacdq_rr[i].f.dqn_r_ac_cur_tap   = co->rdr_dq_delay[i];
	}

	return;
}

XNA(rdf_dq_delay) {
	u32_t i;

	for (i=0; i<16; i++) {
		rs->dacdq_fr[i].f.dqn_f_ac_cur_tap   = co->rdf_dq_delay[i];
	}

	return;
}

XNA(write_lvl_en) {
	return;
}

XNA(dll_dis) {
	if (co->dll_dis == 1) {
		/* DDR2/DDR3 share the same bit in MR0 for DLL disable */
		ddr3_mr1 mr1 = { .v = rs->mr[1].f.mr_data };

		mr1.f.dll_dis = 1;

		rs->mr[1].f.mr_data = mr1.v;
	} else {
		/* DDR2/DDR3 share the same bit in MR0 for DLL reset */
		ddr3_mr0 mr0 = { .v = rs->mr[0].f.mr_data };

		mr0.f.dll_reset = 1;

		rs->mr[0].f.mr_data = mr0.v;
	}
	return;
}

XNA(cal_method) {
	return;
}

XNA(cke_delay) {
	rs->dcsdcr0.f.cke_dly_tap = co->cke_delay;
	return;
}

XNA(cs1_delay) {
	rs->dcsdcr0.f.cs1_dly_tap = co->cs1_delay;
	return;
}

XNA(cs0_delay) {
	rs->dcsdcr0.f.cs0_dly_tap = co->cs0_delay;
	return;
}

XNA(odt_delay) {
	rs->dcsdcr0.f.odt_dly_tap = co->odt_delay;
	return;
}

XNA(ras_delay) {
	rs->dcsdcr0.f.ras_dly_tap = co->ras_delay;
	return;
}

XNA(cas_delay) {
	rs->dcsdcr0.f.cas_dly_tap = co->cas_delay;
	return;
}

XNA(we_delay) {
	rs->dcsdcr0.f.we_dly_tap = co->we_delay;
	return;
}


XNA(ba0_delay) {
	rs->dcsdcr1.f.ba0_dly_tap = co->ba0_delay;
	return;
}

XNA(ba1_delay) {
	rs->dcsdcr1.f.ba1_dly_tap = co->ba1_delay;
	return;
}

XNA(ba2_delay) {
	rs->dcsdcr1.f.ba2_dly_tap = co->ba2_delay;
	return;
}

XNA(dqs0_delay) {
	rs->dcsdcr1.f.dqs0_dly_tap = co->dqs0_delay;
	return;
}

XNA(dqs1_delay) {
	rs->dcsdcr1.f.dqs1_dly_tap = co->dqs1_delay;
	return;
}

XNA(addr_delay) {
	rs->dcsdcr2.f.a0_dly_tap = co->addr_delay[0];
	rs->dcsdcr2.f.a1_dly_tap = co->addr_delay[1];
	rs->dcsdcr2.f.a2_dly_tap = co->addr_delay[2];
	rs->dcsdcr2.f.a3_dly_tap = co->addr_delay[3];
	rs->dcsdcr2.f.a4_dly_tap = co->addr_delay[4];
	rs->dcsdcr2.f.a5_dly_tap = co->addr_delay[5];
	rs->dcsdcr2.f.a6_dly_tap = co->addr_delay[6];
	rs->dcsdcr2.f.a7_dly_tap = co->addr_delay[7];

	rs->dcsdcr3.f.a8_dly_tap  = co->addr_delay[8];
	rs->dcsdcr3.f.a9_dly_tap  = co->addr_delay[9];
	rs->dcsdcr3.f.a10_dly_tap = co->addr_delay[10];
	rs->dcsdcr3.f.a11_dly_tap = co->addr_delay[11];
	rs->dcsdcr3.f.a12_dly_tap = co->addr_delay[12];
	rs->dcsdcr3.f.a13_dly_tap = co->addr_delay[13];
	rs->dcsdcr3.f.a14_dly_tap = co->addr_delay[14];
	rs->dcsdcr3.f.a15_dly_tap = co->addr_delay[15];

	return;
}

XNA(clk_dll_type) {
	return;
}

XNA(dq_dll_type) {
	rs->daccr.f.ac_mode = co->dq_dll_type;
	return;
}

XNA(oride_autok_param) {
	return;
}

XNA(rx_win_sel_mul) {
	return;
}

XNA(rx_win_sel_div) {
	return;
}

XNA(bin_phs_sel) {
	return;
}

MEMCNTLR_SECTION
void mc_info_translation(void) {
	ISTAT_SET(xlat, MEM_XLAT_UNINIT);

	if (ISTAT_GET(probe) != MEM_PROBE_OK) {
		return;
	}

	mc_xlat_n_assign();

	if (ISTAT_GET(xlat) == MEM_XLAT_UNINIT) {
		ISTAT_SET(xlat, MEM_XLAT_OK);
	}

	return;
}

MEMCNTLR_SECTION
void mc_cntlr_zq_calibration(void) {
	u32_t plimit = 0x10000;
	u32_t ddzqpsr, ocdp, ocdn, zqc_cnt;

	ISTAT_SET(cntlr_zq, MEM_CNTLR_ZQ_UNINIT);

	if (ISTAT_GET(to_reg) != MEM_TO_REG_OK) {
		return;
	}

	/* configure_dram_type_on_pad(); */

	zqc_cnt = 0;
 zq_cali_start:
	RMOD_DDZQPCR(zctrl_start, 1);

	while (RFLD_DDZQPCR(zctrl_start) == 1) {
		plimit--;
		if (plimit == 0) {
			ISTAT_SET(cntlr_zq, MEM_CNTLR_ZQ_TIMEOUT);
			return;
		}
	}

	ddzqpsr = DDZQPSRrv;

	if (ddzqpsr & 0x20000000) {
		if ((zqc_cnt++) < MC_ZQC_RETRY_LIM) {
			ISTAT_SET(cntlr_zq, MEM_CNTLR_ZQ_RETRIED_STRICT_OK);
			udelay(1000);
			goto zq_cali_start;
		}

		ocdp = ((ddzqpsr >> 23) & 0x3);
		ocdn = ((ddzqpsr >> 21) & 0x3);

		if ((ocdp == 1) || /* OCDP must NOT be overflow (may tolerate code underflow error) */
		    (ocdn == 1)) { /* OCDN must NOT be overflow (may tolerate code underflow error) */
			ISTAT_SET(cntlr_zq, MEM_CNTLR_ZQ_ERROR);
		} else {
			ISTAT_SET(cntlr_zq, MEM_CNTLR_ZQ_RELAXED_OK);
		}
	}

	if (ISTAT_GET(cntlr_zq) == MEM_CNTLR_ZQ_UNINIT) {
		ISTAT_SET(cntlr_zq, MEM_CNTLR_ZQ_STRICT_OK);
	}

	return;
}

MEMCNTLR_SECTION
static inline void _mc_ana_dll_pow_configure(void) {
	const mc_cntlr_opt_t  *co = meminfo.cntlr_opt;

	RMOD_ANA_DLL0(ana_dll_pow0, 0);
	RMOD_ANA_DLL1(ana_dll_pow1, 0);
	udelay(100);

	if (co->clk_dll_type == 0) {
		RMOD_ANA_DLL0(bin_phs_sel, co->bin_phs_sel);
		RMOD_ANA_DLL1(bin_phs_sel, co->bin_phs_sel);
		udelay(100);
		RMOD_ANA_DLL0(ana_dll_pow0, 1);
		RMOD_ANA_DLL1(ana_dll_pow1, 1);
		udelay(1000000);
	}

	RMOD_ANA_DLL0(dll_mode, co->clk_dll_type);

	return;
}

MEMCNTLR_SECTION
void mc_info_to_reg(void) {
	const mc_dram_param_t *dp = meminfo.dram_param;
	mc_register_set_t *rs     = meminfo.register_set;
	DACDQ_IDX_RR_T *dq_r = (DACDQ_IDX_RR_T *)0xb8001510;
	DACDQ_IDX_FR_T *dq_f = (DACDQ_IDX_FR_T *)0xb8001550;
	u32_t i;
	u8_t mrs_order[] = {2, 3, 1, 0};
	ddr3_mr0 mr0 = { .v = rs->mr[0].f.mr_data };
	volatile u32_t poll_cnt;

	ISTAT_SET(to_reg, MEM_TO_REG_UNINIT);

	if (ISTAT_GET(xlat) != MEM_XLAT_OK) {
		return;
	}

	RMOD_MCR(d_init_trig, 1);
	udelay(1);
	while (RFLD_MCR(d_init_trig) == 1) {
		;
	}

	_mc_ana_dll_pow_configure();

	MCRrv     = rs->mcr.v;
	DCRrv     = rs->dcr.v;
	DTR0rv    = rs->dtr0.v;
	DTR1rv    = rs->dtr1.v;
	DTR2rv    = rs->dtr2.v;
	DIDERrv   = rs->dider.v;
	DRCKOrv   = rs->drcko.v;
	DDZQPRrv  = rs->ddzqpr.v;
	DDZQPCRrv = rs->ddzqpcr.v;
	DACCRrv   = rs->daccr.v;
	DCDQMRrv  = rs->dcdqmr.v;

	DCSDCR0rv = rs->dcsdcr0.v;
	DCSDCR1rv = rs->dcsdcr1.v;
	DCSDCR2rv = rs->dcsdcr2.v;
	DCSDCR3rv = rs->dcsdcr3.v;

	RMOD_DACCR(ac_bptr_clear, 1);

	for (i=0; i<16; i++) {
		dq_r->v = rs->dacdq_rr[i].v;
		dq_f->v = rs->dacdq_fr[i].v;
		dq_r++;
		dq_f++;
	}

	if (dp->dram_type == 2) {
		/* DG3 only supports 4 bursts for DDR2. */
		mr0.f.burst_len = 0x2;
	} else if (dp->dram_type == 3) {
		/* DG3 only supports 8 bursts for DDR3. */
		mr0.f.burst_len = 0x0;
	}
	rs->mr[0].f.mr_data = mr0.v;

	/* Set DDR2/3 mode registers. */
	for (i=0; i<4; i++) {
		DMCRrv = rs->mr[mrs_order[i]].v;
		udelay(1);
		poll_cnt = 0;
		while (RFLD_DMCR(dtr_up_busy_mrs_busy) == 1) {
			if ((poll_cnt++) > 100) {
				SYSTEM_RESET();
			}
			;
		}
	}

	if (ISTAT_GET(to_reg) == MEM_TO_REG_UNINIT) {
		ISTAT_SET(to_reg, MEM_TO_REG_OK);
	}

	return;
}

MEMCNTLR_SECTION
void static ddr3_zq_long_calibration(void) {
	RMOD_DMCR(dis_dram_ref, 1);

	RMOD_D3ZQCCR(zq_long_tri, 1);

	udelay(1);

	while (RFLD_D3ZQCCR(zq_long_tri) == 1) {
		;
	}

	RMOD_DMCR(dis_dram_ref, 0);

	return;
}

MEMCNTLR_SECTION
void static ddr3_en_read_buf_full_mask(void) {
	RMOD_MCR(ocp1_rbf_mask_en, 1,
	         ocp0_rbf_mask_en, 1);
	return;
}

u32_t dram_autok(void);

MEMCNTLR_SECTION
void mc_calibration(void) {
	ISTAT_SET(cal, MEM_CAL_UNINIT);

	if ((ISTAT_GET(cntlr_zq) != MEM_CNTLR_ZQ_STRICT_OK)  &&
	    (ISTAT_GET(cntlr_zq) != MEM_CNTLR_ZQ_RELAXED_OK) &&
	    (ISTAT_GET(cntlr_zq) != MEM_CNTLR_ZQ_RETRIED_STRICT_OK)) {
		return;
	}

	if (RFLD_MCR(dram_type) == 2) {
		ddr3_zq_long_calibration();
		ddr3_en_read_buf_full_mask();
	}

	if (meminfo.cntlr_opt->cal_method == 1) {
		dram_autok();
	}

	if (ISTAT_GET(cal) == MEM_CAL_UNINIT) {
		ISTAT_SET(cal, MEM_CAL_OK);
	}

	return;
}

MEMCNTLR_SECTION
void mc_result_decode(void) {
	const mc_register_set_t *rs = meminfo.register_set;
	s8_t *res[5];
	u32_t i;

	res[0] = (s8_t *)ISTAT_STR(probe);
	res[1] = (s8_t *)ISTAT_STR(xlat);
	res[2] = (s8_t *)ISTAT_STR(to_reg);
	res[3] = (s8_t *)ISTAT_STR(cntlr_zq);
	res[4] = (s8_t *)ISTAT_STR(cal);

	for (i=0; i<5; i++) {
		printf("II: %s\n", res[i]);
	}

	printf("II: cntlr zq result: %08x\n", DDZQPSRrv);
	printf("II: dcr result: %08x\n", DCRrv);

	if (meminfo.cntlr_opt->cal_method != 0) {
		return;
	}

	for (i=0; i<4; i++) {
		printf("II: MR%d: %08x\n", i, rs->mr[i].v);
	}

	return;
}
REG_INIT_FUNC(mc_result_decode,        29);
REG_INIT_FUNC(mc_info_probe,           20);
REG_INIT_FUNC(mc_info_translation,     22);
REG_INIT_FUNC(mc_info_to_reg,          24);
REG_INIT_FUNC(mc_cntlr_zq_calibration, 26);
REG_INIT_FUNC(mc_calibration,          28);

MEMCNTLR_SECTION
void mc_crit_reg(void) {
	volatile u32_t *ptr;

	puts("EE: Critical registers:\n");

	printf("0200: %08x\n", REG32(0xb8000200));
	printf("0204: %08x\n", REG32(0xb8000204));
	printf("0208: %08x\n", REG32(0xb8000208));
	printf("020c: %08x\n", REG32(0xb800020c));
	printf("0210: %08x\n", REG32(0xb8000210));
	printf("021c: %08x\n", REG32(0xb800021c));
	printf("0220: %08x\n", REG32(0xb8000220));
	printf("0224: %08x\n", REG32(0xb8000224));
	printf("0228: %08x\n", REG32(0xb8000228));
	printf("0308: %08x\n", REG32(0xb8000308));
	printf("030c: %08x\n", REG32(0xb800030c));
	printf("0310: %08x\n", REG32(0xb8000310));
	printf("1000: %08x\n", REG32(0xb8001000));
	printf("1004: %08x\n", REG32(0xb8001004));
	printf("1008: %08x\n", REG32(0xb8001008));
	printf("100c: %08x\n", REG32(0xb800100c));
	printf("1010: %08x\n", REG32(0xb8001010));
	printf("1050: %08x\n", REG32(0xb8001050));
	printf("1090: %08x\n", REG32(0xb8001090));
	printf("1094: %08x\n", REG32(0xb8001094));
	printf("1098: %08x\n", REG32(0xb8001098));
	printf("1500: %08x\n", REG32(0xb8001500));
	printf("1590: %08x\n", REG32(0xb8001590));
	printf("15b0: %08x\n", REG32(0xb80015b0));
	printf("15b4: %08x\n", REG32(0xb80015b4));
	printf("15b8: %08x\n", REG32(0xb80015b8));
	printf("15bc: %08x\n", REG32(0xb80015bc));

	ptr = (volatile u32_t *)0xb8001510;
	while (((u32_t)ptr) < 0xb8001590) {
		printf("%04x: %08x\n",
		       ((u32_t)ptr) & 0xffff,
		       *ptr);
		ptr++;
	}
	return;
}

MEMCNTLR_SECTION
u32_t mc_zqc_retry(u32_t odtp, u32_t odtn, u32_t ocdp, u32_t ocdn, u32_t zqc_cnt) {
	if ((odtp == 3) || (odtn == 3) || (ocdp == 3) || (ocdn == 3)) {
		if (zqc_cnt > 11) {
			puts("EE: ZQ cali. fails");
			while (1) {
				;
			}
		}
		printf("DD: ZQ cali. #%d in progress (%08x)...\n", zqc_cnt, DDZQPSRrv);

		/* delay 1ms before next try. */
		udelay(1000);
		return 1;
	} else {
		return 0;
	}
}

MEMCNTLR_SECTION
u32_t mc_akh_get_zq_setting(u32_t orig) {
	if (mc_akh_is_override_autok()) {
		return (DDZQPCRrv & 0xffff);
	} else {
		return orig;
	}
}

MEMCNTLR_SECTION
u32_t mc_akh_is_ddr2_drv_reduced(u32_t orig) {
	if (mc_akh_is_override_autok()) {
		return (1 - meminfo.cntlr_opt->normal_drv);
	} else {
		return orig;
	}
}

MEMCNTLR_SECTION
u32_t mc_akh_get_ddr2_odt_setting(u32_t orig) {
	if (mc_akh_is_override_autok()) {
		return meminfo.register_set->mr[1].v & ((1 << 2) | (1 << 6));
	} else {
		return orig;
	}
}

MEMCNTLR_SECTION
u32_t mc_akh_is_override_autok(void) {
	return meminfo.cntlr_opt->oride_autok_param;
}

MEMCNTLR_SECTION
u32_t mc_akh_rx_win_sel_mul(u32_t orig) {
	if (mc_akh_is_override_autok()) {
		return meminfo.cntlr_opt->rx_win_sel_mul;
	} else {
		return orig;
	}
}

MEMCNTLR_SECTION
u32_t mc_akh_rx_win_sel_div(u32_t orig) {
	if (mc_akh_is_override_autok()) {
		return meminfo.cntlr_opt->rx_win_sel_div;
	} else {
		return orig;
	}
}
