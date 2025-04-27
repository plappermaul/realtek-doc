#include <soc.h>
#include <dram/memcntlr.h>
#include <dram/memcntlr_ag.h>
#include <cg/cg.h>

static mc_dram_param_t   dram_param;
static mc_cntlr_opt_t    cntlr_opt;
static mc_register_set_t register_set;
static mc_register_set_t default_rs;

mc_info_t meminfo = {
    &dram_param, &cntlr_opt, &register_set
};


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

#define NS2CK(ns, tck_ps)       ((ns * 1000 + (tck_ps - 1)) / tck_ps)
#define NS2CK2(ns, freq)        ((ns*freq + 999)/1000)
#define BOUND_CHECK(v, min, max) ({ u32_t __v=v;    \
                                    if (__v > max) __v=max; \
                                    else if (__v < min) __v=min;    \
                                    __v; })

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
            u32_t cl_ck = dp->cl_ck;
            if (GET_MEM_MHZ()>533) { 
                cl_ck --; 
                if (1==meminfo.cntlr_opt->dbg_en) {
                    printf("DD: DDR CL = %d\n", cl_ck);}
            }
            mr0.f.cl_h = (cl_ck - 4) & 0x7;
            mr0.f.cl_l = (cl_ck - 4) >> 3;

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
    u32_t tck = BOUND_CHECK(NS2CK2(dp->min_trtp_ns, GET_MEM_MHZ()), 4, 16);//max(4*tCK, ns)
    rs->dtr0.f.t_rtp = tck - 1;
    return;
}

XNA(min_twtr_ns) {
    u32_t tck = BOUND_CHECK(NS2CK2(dp->min_twtr_ns, GET_MEM_MHZ()), 4, 16);//max(4*tCK, ns)
    rs->dtr0.f.t_wtr = tck - 1;
    return;
}

XNA(min_twr_ns) {
    u32_t wr_ck = NS2CK2(dp->min_twr_ns, GET_MEM_MHZ());

    /* for mr0 */
    if (dp->dram_type == 2) {
        ddr2_mr0 mr0 = { .v = rs->mr[0].f.mr_data };

        if ((wr_ck < 2) || (wr_ck > 6)) {
            ISTAT_SET(xlat, MEM_WR_ERROR);
        } else {
            mr0.f.wr = rs->dtr0.f.t_wr = wr_ck-1;
            rs->mr[0].f.mr_data = mr0.v;
        }
    } else if (dp->dram_type == 3) {
        ddr3_mr0 mr0 = { .v = rs->mr[0].f.mr_data };

        if ((wr_ck < 5) || (wr_ck > 16)) {
            ISTAT_SET(xlat, MEM_WR_ERROR);
        } else { // twr = 5~8, 10, 12, 14, 16
            if (wr_ck > 8) {
                wr_ck = (wr_ck + 1) >> 1; // roundup and right shift
                mr0.f.wr = wr_ck;
                rs->dtr0.f.t_wr = (wr_ck << 1) - 1;
            } else {
                mr0.f.wr = wr_ck - 4;
                rs->dtr0.f.t_wr = wr_ck - 1;
            }
            rs->mr[0].f.mr_data = mr0.v;
        }
    } else {
        ISTAT_SET(xlat, MEM_MR0_ERROR);
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
    u32_t tck = BOUND_CHECK(NS2CK2(dp->min_trp_ns, GET_MEM_MHZ()), 4, 32);  //max(4*tCK, ns)
    rs->dtr1.f.t_rp = tck - 1;
    return;
}

XNA(min_trcd_ns) {
    u32_t tck = BOUND_CHECK(NS2CK2(dp->min_trcd_ns, GET_MEM_MHZ()), 1, 32);
    rs->dtr1.f.t_rcd = tck - 1;
    return;
}

XNA(min_trrd_ns) {
    u32_t tck = BOUND_CHECK(NS2CK2(dp->min_trrd_ns, GET_MEM_MHZ()), 4, 32);//max(4*tCK, ns)
    rs->dtr1.f.t_rrd = tck - 1;
    return;
}

XNA(min_tfaw_ns) {
    u32_t tck = BOUND_CHECK(NS2CK2(dp->min_tfaw_ns, GET_MEM_MHZ()), 1, 32);
    rs->dtr1.f.t_fawg = tck - 1;
    return;
}

XNA(min_trfc_ns) {
    u32_t tck = BOUND_CHECK(NS2CK2(dp->min_trfc_ns, GET_MEM_MHZ()), 1, 256);
    rs->dtr2.f.t_rfc = tck - 1;
    return;
}

XNA(min_tras_ns) {
    u32_t tck = BOUND_CHECK(NS2CK2(dp->min_tras_ns, GET_MEM_MHZ()), 1, 64);
    rs->dtr2.f.t_ras = tck - 1;
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

    if (dp->dram_type == 3) {
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

u16_t ocd_odt_table[19][2] MEMCNTLR_DATA_SECTION = {
 /* {OCD/ODT,   Value}  */
    {0,   0},
    {34,  0x76},
    {40,  0x3a},
    {45,  0x77},
    {48,  0x78},
    {50,  0x7d},
    {60,  0x30},
    {72,  0x28},
    {80,  0x60},
    {90,  0x20},
    {120, 0x21},
    {144, 0x42},
    {160, 0x19},
    {180, 0x10},
    {240, 0x05},
    {320, 0x49},
    {360, 0x8},    
    {480, 0x9},
    {600, 0},
};
    
XNA(cntlr_odt) {
    u32_t cntlr_odt = co->cntlr_odt << 1;
    u32_t cntlr_ocd = co->cntlr_ocd;
    u32_t idx = 0;
    while(!((cntlr_odt >= ocd_odt_table[idx][0]) && (cntlr_odt < ocd_odt_table[idx+1][0]))){
        idx++;
    }
    u32_t odt_val = ocd_odt_table[idx][1];
    
    idx = 0;
    while(!((cntlr_ocd >= ocd_odt_table[idx][0]) && (cntlr_ocd < ocd_odt_table[idx+1][0]))){
        idx++;
    }
    u32_t ocd_val = ocd_odt_table[idx][1];
    //printf("ocd_val =0x%x , odt_val0x%x \n\r",ocd_val,odt_val);

    rs->ddzqpcr.f.zprog = (ocd_val | ocd_val <<7 | odt_val <<14);    
    return;
}

XNA(cntlr_ocd) {
    /* this field is XnA'd along with cntlr_odt. */
    return;
}

XNA(cntlr_md_odt) { return; }
XNA(cntlr_md_ocd) { return; }
XNA(cntlr_dm_ocd) { return; }
XNA(cntlr_ck_ocd) { return; }
XNA(cntlr_dqs_odt) { return; }
XNA(cntlr_dqs_ocd) { return; }
XNA(cntlr_cmd_ocd) { return; }
XNA(cntlr_ma_ocd) { return; }

/*XNA(tx_delay) {
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
}*/

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

XNA(dbg_en) {
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

XNA(dq_dll_type) {
    rs->daccr.f.ac_mode = co->dq_dll_type;
    return;
}

MEMCNTLR_SECTION
void mc_dram_phy_reset(void) {
        DACCRrv = DACCRrv & 0xFFFFFFEF;
        DACCRrv = DACCRrv | 0x10;
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
#if 0
MEMCNTLR_SECTION
u32_t _mc_trans_zprog(u32_t ocd, u32_t odt) {
    u32_t zprog, idx=1;
    u32_t _ocd = ocd, _odt = odt<<1;
    u32_t ocd_z=0, odt_z=0;
    while (0!=ocd_odt_table[idx][1]) {
        if (_ocd>=ocd_odt_table[idx][0]) ocd_z=ocd_odt_table[idx][1];
        if (_odt>=ocd_odt_table[idx][0]) odt_z=ocd_odt_table[idx][1];
        idx++;
    }
    zprog = (ocd_z|ocd_z<<7|odt_z<<14);
    printf("DD: zprog = 0x%x\n", zprog);
    return zprog;
}

MEMCNTLR_SECTION
u32_t _mc_trans_type0_io_pad(void) {
    zctrl_impedance_t imp = {.v = RFLD_DDZQPSR(zctrl_imp) };
    DDR_IOPDCR_TYPE0_T iopd = {.v = 0};
    iopd.f.force = 1;
    iopd.f.nt    = imp.f.nt;
    iopd.f.pt    = imp.f.pt;
    iopd.f.plsb  = imp.f.plsb_0 | (imp.f.plsb_1 << 1);
    iopd.f.ttcn  = imp.f.ttcn;
    iopd.f.ttcp  = imp.f.ttcp;
    iopd.f.ttfn  = imp.f.ttfn;
    iopd.f.ttfp  = imp.f.ttfp;
    iopd.f.drv   = imp.f.e2;
    printf("DD: nt %d, pt %d, plsb %d, ttcn %d, ttcp %d, ttfn %d, ttfp %d, drv %d\n",
            iopd.f.nt, iopd.f.pt, iopd.f.plsb, iopd.f.ttcn, iopd.f.ttcp, iopd.f.ttfn, iopd.f.ttfp, iopd.f.drv);
    return iopd.v;
}

MEMCNTLR_SECTION
u32_t _mc_trans_type1_io_pad(void) {
    zctrl_impedance_t imp = {.v = RFLD_DDZQPSR(zctrl_imp) };
    DDR_IOPDCR_TYPE1_T iopd = {.v = 0 };
    iopd.f.force = 1;
    iopd.f.nt    = imp.f.nt;
    iopd.f.pt    = imp.f.pt;
    iopd.f.plsb  = imp.f.plsb_0 | (imp.f.plsb_1 << 1);
    iopd.f.drv   = imp.f.e2;
    printf("DD: nt %d, pt %d, plsb %d, drv %d\n",
		 iopd.f.nt, iopd.f.pt, iopd.f.plsb, iopd.f.drv);
    return iopd.v;
}
MEMCNTLR_SECTION
u32_t _mc_force_mode_retrieve(u32_t ocd, u32_t odt, u32_t type) {
    u32_t iopd=0;
    if (0==ocd || 0==odt) return 0; //invalid
    u32_t zprog = _mc_trans_zprog(ocd, odt);
    if(MEMCTL_ZQ_CALI_FAIL!=memctlc_ZQ_calibration(zprog)) {
        iopd = (type)?_mc_trans_type1_io_pad():_mc_trans_type0_io_pad();        
    }
    return iopd;
}
#endif

#define DONT_CARE_ODT	(20)
MEMCNTLR_SECTION
void mc_cntlr_zq_calibration(void) {
    ISTAT_SET(cntlr_zq, MEM_CNTLR_ZQ_UNINIT);

    if (ISTAT_GET(to_reg) != MEM_TO_REG_OK) {
        return;
    }

#if 0
    RMOD_DMCR(dis_dram_ref, 1);

    if (meminfo.cntlr_opt->zq_cal_method != 0) {
    u32_t iopd_md, iopd_dqs;
    u16_t iopd_dm, iopd_ck, iopd_cmd, iopd_ma;
    mc_cntlr_opt_t *co = meminfo.cntlr_opt;
    iopd_md  = _mc_force_mode_retrieve (co->cntlr_md_ocd, co->cntlr_md_odt, 0);
    iopd_dm  = _mc_force_mode_retrieve (co->cntlr_dm_ocd, DONT_CARE_ODT, 1);	// odt don't care
    iopd_ck  = _mc_force_mode_retrieve (co->cntlr_ck_ocd, DONT_CARE_ODT, 1);	// odt dont' care
    iopd_dqs = _mc_force_mode_retrieve (co->cntlr_dqs_ocd, co->cntlr_dqs_odt, 0);
    iopd_cmd = _mc_force_mode_retrieve (co->cntlr_cmd_ocd, DONT_CARE_ODT, 1);	// odt don't care
    iopd_ma  = _mc_force_mode_retrieve (co->cntlr_ma_ocd, DONT_CARE_ODT, 1);	// odt don't care
    printf("mc_cntlr_zq_calibration\n\r");
    memctlc_ZQ_calibration(meminfo.register_set->ddzqpcr.f.zprog);
    
    // apply force mode to register
    if( 0!= iopd_md ) {
        printf("DD: apply md with 0x%x\n", iopd_md);
        DDR_MD_IOPDCR0rv = iopd_md;
        DDR_MD_IOPDCR1rv = iopd_md;
        DDR_MD_IOPDCR2rv = iopd_md;
        DDR_MD_IOPDCR3rv = iopd_md;
    }
    if( 0!= iopd_dm ) {
        printf("DD: apply dm with 0x%x\n", iopd_dm);
        DDR_DM_IOPDCRrv = iopd_dm | (iopd_dm << 16);
    }
    if( 0!= iopd_ck ) {
        printf("DD: apply ck with 0x%x\n", iopd_ck);
        DDR_CK_IOPDCRrv = iopd_ck | (iopd_ck << 16);
    }
    if( 0!= iopd_dqs ) {
        printf("DD: apply dqs with 0x%x\n", iopd_dqs);
        DDR_DQS_IOPDCR0rv = iopd_dqs;
        DDR_DQS_IOPDCR1rv = iopd_dqs;
        DDR_DQS_IOPDCR2rv = iopd_dqs;
        DDR_DQS_IOPDCR3rv = iopd_dqs;
    }
    if( 0!= iopd_cmd || 0!= iopd_ma) {
	u32_t _ipod_ma =  iopd_ma | (iopd_cmd << 16);
        printf("DD: apply cmd and ma with 0x%x\n", _ipod_ma);
        DDR_MA_IOPDCRrv = _ipod_ma;
    }
    }
    RMOD_DMCR(dis_dram_ref, 0);
#endif
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
    
	RMOD_MCR(d_init_trig, 1);
	udelay(1);
	while (RFLD_MCR(d_init_trig) == 1);


	//MCRrv     = (MCRrv&0x1F)|rs->mcr.v;
	MCRrv     = rs->mcr.v;
	DCRrv     = rs->dcr.v;
	DTR0rv    = rs->dtr0.v;
	DTR1rv    = rs->dtr1.v;
	DTR2rv    = rs->dtr2.v;
	//DRCKOrv   = rs->drcko.v;
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


	for (i=0; i<16; i++) {
		dq_r->v = rs->dacdq_rr[i].v;
		dq_f->v = rs->dacdq_fr[i].v;
		dq_r++;
		dq_f++;
	}

	if (dp->dram_type == 2) {
        /* only supports 4 bursts for DDR2. */
		mr0.f.burst_len = 0x2;
	} else if (dp->dram_type == 3) {
        /* only supports 8 bursts for DDR3. */
		mr0.f.burst_len = 0x0;
	}
	rs->mr[0].f.mr_data = mr0.v;
	
	/* Set DDR2/3 mode registers. */
	for (i=0; i<4; i++) {
		if (1==mrs_order[i]) {
			DMCRrv = rs->mr[mrs_order[i]].v|0x1;
		if (1==meminfo.cntlr_opt->dbg_en) {
			printf("DD: MR%d: %08x\n", mrs_order[i], rs->mr[mrs_order[i]].v|0x1);}
		while (RFLD_DMCR(dtr_up_busy_mrs_busy) == 1);
	}
	DMCRrv = rs->mr[mrs_order[i]].v;
	if (1==meminfo.cntlr_opt->dbg_en) {
		printf("DD: MR%d: %08x\n", mrs_order[i], rs->mr[mrs_order[i]].v); }
		while (RFLD_DMCR(dtr_up_busy_mrs_busy) == 1);
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
    while (RFLD_D3ZQCCR(zq_long_tri) == 1);
    return;
}

MEMCNTLR_SECTION
void static ddr3_en_read_buf_full_mask(void) {
    RMOD_MCR(ocp1_rbf_mask_en, 1,
             ocp0_rbf_mask_en, 1);
    return;
}

MEMCNTLR_SECTION  
__attribute__((weak))
void mc_dll_config(void){}

MEMCNTLR_SECTION
void mc_calibration(void) {
    ISTAT_SET(cal, MEM_CAL_UNINIT);

    if (ISTAT_GET(cntlr_zq) != MEM_CNTLR_ZQ_STRICT_OK) {
        return;
    }
    int ret = 0;

    if (RFLD_MCR(dram_type) == 2) {
        ddr3_zq_long_calibration();
        ddr3_en_read_buf_full_mask();
    }

    /* enable dynamic fifo reset */
    RMOD_DACCR(ac_dyn_bptr_clr_en,1);

    /*if (meminfo.cntlr_opt->cal_method != 0) {      
        //mc_cntlr_zq_calibration();
        ret = dram_autok();
    } else {*/
        // toggle DMCR to clear
        DMCRrv = meminfo.register_set->mr[0].v;
    //}

    mc_dram_phy_reset();
    //RMOD_DACCR(ac_dyn_bptr_clr_en,0);

    /* enhance read queue on */
    RMOD_DCR(rq_en, 1);

    udelay(10);

    if(0 == ret){
        ISTAT_SET(cal, MEM_CAL_OK);
    }else{
        ISTAT_SET(cal, MEM_CAL_UNINIT);
    }

    return;
}

MEMCNTLR_SECTION
void mc_result_show_dram_config(void) {
    if (0==meminfo.cntlr_opt->dbg_en) return;
    unsigned int i, j;
    volatile unsigned int *phy_reg;
    const char *dq_edge_str[2] = {"Rising", "Falling"};

    puts("II: Delay Line Type... ");
    if(meminfo.cntlr_opt->dq_dll_type==0) {puts("analog\n");
    } else {puts("digital\n");}

    printf("II: MCR(%08x)    ZQPSR(%08x)\n", MCRrv, DDZQPSRrv);
    printf("II: DCR(%08x)    DTR0(%08x)    DTR1(%08x)    DTR2(%08x)\n",
               DCRrv, DTR0rv, DTR1rv, DTR2rv);
    printf("II: DACCR(%08x)  DACSPCR(%08x) DACSPAR(%08x) DACSPSR(%08x)\n",
               DACCRrv, DACSPCRrv, DACSPARrv, DACSPSRrv);

    phy_reg = (unsigned int *)(0xb8001510);

    puts("II: Write Delay (taps):");
    for (i=0; i<16; i++) {
        if ((i%8) == 0) {
            printf("\n    DQ%02d:", i);
        }
        printf("    %02d", (*(phy_reg + i) >> 24) & 0x1f);
    }

    for (j=0; j<2; j++) {
        unsigned int rdly;
        phy_reg = (unsigned int *)(0xb8001510 + j*0x40);
        printf("\nII: Read %s Edge Delay (Max./Current/Min. taps):", dq_edge_str[j]);
        for (i=0; i<16; i++) {
            rdly = (*(phy_reg + i)) & 0x1fffff;
            if ((i%4) == 0) {
                printf("\n    DQ%2d:", i);
            }
            printf("    %02d/%02d/%02d", (rdly >> 16) & 0x1f,
                                          (rdly >>  8) & 0x1f,
                                          (rdly >>  0) & 0x1f);
        }
    }
    puts("\n");
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

    mc_result_show_dram_config();

    return;
}

REG_INIT_FUNC(mc_info_probe,           12);
REG_INIT_FUNC(mc_info_translation,     22);
REG_INIT_FUNC(mc_info_to_reg,          24);
REG_INIT_FUNC(mc_cntlr_zq_calibration, 26);
REG_INIT_FUNC(mc_calibration,          28);
REG_INIT_FUNC(mc_result_decode,        30);
