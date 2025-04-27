#include <soc.h>
#include <platform/dram/memcntlr.h>
#include <util.h>
#include <cg/cg.h>
#include <cg/mem_pll.h>

static mc_dram_param_t   dram_param;
static mc_cntlr_opt_t    cntlr_opt;
static mc_register_set_t register_set;
static mc_register_set_t default_rs;

mc_info_t meminfo = {
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
                rs->mr[0].f.mr_data |= ((3==dp->dram_type)?0x3:0x2);
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
    if (60 == co->cntlr_odt &&
        40 == co->cntlr_ocd) {
        rs->ddzqpcr.f.zprog = 0x85D3A;
		}
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

XNA(zq_cal_method) {
	return;
}

XNA(parallel_bank_en) {
    rs->dcr.f.prl_bank_act_en = co->parallel_bank_en;
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
	rs->ana_dll0.f.dll_mode = co->clk_dll_type;
	return;
}

XNA(dq_dll_type) {
	rs->daccr.f.ac_mode = co->dq_dll_type;
	return;
}

MEMCNTLR_SECTION
void mc_dram_phy_reset(void) {
        DACCRrv = DACCRrv & 0xFFFFFFEF;
        DACCRrv = DACCRrv | 0x10;
	DACCRrv = DACCRrv | 0x20;
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
__attribute__((weak))
void mc_cntlr_zq_static_patch(void) {}

MEMCNTLR_SECTION
void mc_cntlr_zq_calibration(void) {
	u32_t plimit = 0x10000;
	u32_t ddzqpsr, odtp, odtn, ocdp, ocdn;

	ISTAT_SET(cntlr_zq, MEM_CNTLR_ZQ_UNINIT);

	if (ISTAT_GET(to_reg) != MEM_TO_REG_OK) {
		return;
	}

        RMOD_DMCR(dis_dram_ref, 1);

	if (meminfo.cntlr_opt->zq_cal_method != 0) {
		/* configure_dram_type_on_pad(); */

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
			odtp = ((ddzqpsr >> 27) & 0x3);
			odtn = ((ddzqpsr >> 25) & 0x3);
			ocdp = ((ddzqpsr >> 23) & 0x3);
			ocdn = ((ddzqpsr >> 21) & 0x3);
			if ((odtp != 0) || /* ODTP must be completed with no error */
			    (odtn == 2) || /* ODTN must NOT be underflow (may tolerate code overflow error) */
			    (ocdp == 1) || /* OCDP must NOT be overflow (may tolerate code underflow error) */
			    (ocdn == 1)) { /* OCDN must NOT be overflow (may tolerate code underflow error) */
				ISTAT_SET(cntlr_zq, MEM_CNTLR_ZQ_ERROR);
			}
		}
        } else {
                /* static zq apply (this might be changed)*/
                mc_cntlr_zq_static_patch();
        }

        RMOD_DMCR(dis_dram_ref, 0);

	if (ISTAT_GET(cntlr_zq) == MEM_CNTLR_ZQ_UNINIT) {
		ISTAT_SET(cntlr_zq, MEM_CNTLR_ZQ_STRICT_OK);
	}

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

	ISTAT_SET(to_reg, MEM_TO_REG_UNINIT);

	if (ISTAT_GET(xlat) != MEM_XLAT_OK) {
		return;
	}

	MCRrv     = rs->mcr.v;
	DCRrv     = rs->dcr.v;
	DTR0rv    = rs->dtr0.v;
	DTR1rv    = rs->dtr1.v;
	DTR2rv    = rs->dtr2.v;
	DRCKOrv   = rs->drcko.v;
	DDZQPRrv  = rs->ddzqpr.v;
	DDZQPCRrv = rs->ddzqpcr.v;
	DACCRrv   = rs->daccr.v;
	DCDQMRrv  = rs->dcdqmr.v;

	CG_MEM_PLL_OE_DIS();
	DIDERrv   = rs->dider.v;
	DCSDCR0rv = rs->dcsdcr0.v;
	DCSDCR1rv = rs->dcsdcr1.v;
	DCSDCR2rv = rs->dcsdcr2.v;
	DCSDCR3rv = rs->dcsdcr3.v;
	CG_MEM_PLL_OE_EN();
	udelay(400);

	//ANA_DLL0rv = rs->ana_dll0.v;

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
                if (1==mrs_order[i]) {
                        DMCRrv = rs->mr[mrs_order[i]].v|0x1;
                        printf("DD: MR%d: %08x\n", mrs_order[i], rs->mr[mrs_order[i]].v|0x1);
                        while (RFLD_DMCR(dtr_up_busy_mrs_busy) == 1);
                }
		DMCRrv = rs->mr[mrs_order[i]].v;
		printf("DD: MR%d: %08x\n", mrs_order[i], rs->mr[mrs_order[i]].v);
		while (RFLD_DMCR(dtr_up_busy_mrs_busy) == 1) {
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
	
        RMOD_D3ZQCCR(zq_long_tri, 1);

	udelay(10);
	while (RFLD_D3ZQCCR(zq_long_tri) == 1) {
		;
	}

	return;
}

MEMCNTLR_SECTION
void static ddr3_en_read_buf_full_mask(void) {
        RMOD_MCR(ocp1_rbf_mask_en, 1,
                 ocp0_rbf_mask_en, 1);
        return;
}

MEMCNTLR_SECTION
void static tx_rx_calibration(void) {
	/* This is a dummy function by now. */
	return;
}

MEMCNTLR_SECTION
void mc_calibration(void) {
	ISTAT_SET(cal, MEM_CAL_UNINIT);

	if (ISTAT_GET(cntlr_zq) != MEM_CNTLR_ZQ_STRICT_OK) {
		return;
	}
        
        if (RFLD_MCR(dram_type) == 2) {
	        ddr3_zq_long_calibration();
		ddr3_en_read_buf_full_mask();
        }

	if (meminfo.cntlr_opt->cal_method != 0) {
		tx_rx_calibration();
	}

        mc_dram_phy_reset();
        
        udelay(10);

	if (ISTAT_GET(cal) == MEM_CAL_UNINIT) {
		ISTAT_SET(cal, MEM_CAL_OK);
	}

	return;
}

MEMCNTLR_SECTION
void mc_result_decode(void) {
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

	return;
}
REG_INIT_FUNC(mc_result_decode,        29);
REG_INIT_FUNC(mc_info_probe,           20);
REG_INIT_FUNC(mc_info_translation,     22);
REG_INIT_FUNC(mc_info_to_reg,          24);
REG_INIT_FUNC(mc_cntlr_zq_calibration, 26);
REG_INIT_FUNC(mc_calibration,          28);
