#include <soc.h>
#include <dram/memcntlr.h>
#include <dram/memcntlr_ag.h>
#include <util.h>
#include <cg/cg.h>

static mc_cntlr_opt_t    cntlr_opt;
static mc_dpi_opt_t      dpi_opt;
static mc_register_set_t register_set;
static mc_register_set_t default_rs;

extern mc_var_t mc_var_begin, mc_var_end;

mc_info_t meminfo = {
    &cntlr_opt, &dpi_opt, &register_set
};

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

    if(mi == VZERO) {
        ISTAT_SET(probe, MEM_PROBE_FAIL);
    } else if(((u32_t)mi) == 1) {
        ISTAT_SET(probe, MEM_PROBE_FUNC_MISSING);
    } else {
        inline_memcpy(&cntlr_opt,    mi->cntlr_opt,    sizeof(mc_cntlr_opt_t));
        inline_memcpy(&dpi_opt,      mi->dpi_opt,      sizeof(mc_dpi_opt_t));
        inline_memcpy(&register_set, mi->register_set, sizeof(mc_register_set_t));
        inline_memcpy(&default_rs,   mi->register_set, sizeof(mc_register_set_t));

        _soc.dram_info = (void *)&meminfo;

        ISTAT_SET(probe, MEM_PROBE_OK);
    }

    return;
}

#define ROUNDUP(v, d)           (((v)+((d)-1))/(d))
#define NS2CK(ns, tck_ps)       ROUNDUP((ns)*1000, tck_ps)
#define NS2CK1(ns)              ROUNDUP((ns)*(GET_MEM_MHZ()), 1000)/DFI_RATE
#define NS2CK2(ns)              ROUNDUP(ROUNDUP((ns)*(GET_MEM_MHZ()), 1000), DFI_RATE)
#define BOUND_CHECK(v, min, max) ({ u32_t __v=v;    \
                                    if(__v > max) __v=max; \
                                    else if(__v < min) __v=min;    \
                                    __v; })

MEMCNTLR_SECTION
void mc_xlat_dram_type(mc_info_t *r) {
    u32_t _v = r->cntlr_opt->dram_type;
    if(DDR_TYPE_SPIN==_v) {    // according to strapped pin
        u8_t dt = RFLD_PSR0(pin_dramtype);
        r->register_set->dcr.f.ddr = (2==dt)?DDR_TYPE_LPDDR3:((1==dt)?DDR_TYPE_DDR4:DDR_TYPE_DDR3);
        r->cntlr_opt->dram_type = r->register_set->dcr.f.ddr;
    } else if((DDR_TYPE_DDR3==_v) ||
       (DDR_TYPE_DDR4==_v) ||
       (DDR_TYPE_LPDDR3==_v)) {
        r->register_set->dcr.f.ddr = _v;
    } else {
        ISTAT_SET(xlat, MEM_DRAM_TYPE_ERROR);
    }
    return;
}

MEMCNTLR_SECTION
void mc_xlat_bankcnt(mc_info_t *r) {
    u32_t _v = r->cntlr_opt->bankcnt;
    u32_t bc=0;
    for(bc=0; bc<=4; bc++) {
        if(_v==(2<<bc)) break;
    }
    if(4==bc) {
        ISTAT_SET(xlat, MEM_BANK_NUM_ERROR);
    } else {
        r->register_set->misc.f.bank_size = bc;
    }
    return;
}

MEMCNTLR_SECTION
void mc_xlat_pagesize(mc_info_t *r) {
    u32_t _v = r->cntlr_opt->pagesize;
    u32_t ps=0;
    u8_t dt = get_dq32_status();
    _v*=(dt+1);

    for(ps=0; ps<=8; ps++) {
        if(_v==(256<<ps)) break;
    }
    if(8==ps) {
        ISTAT_SET(xlat, MEM_PAGE_SIZE_ERROR);
    } else {
        r->register_set->misc.f.page_size = ps;
    }
    return;
}

MEMCNTLR_SECTION
void mc_xlat_burst(mc_info_t *r) {
    u32_t _v = r->cntlr_opt->burst >> 3;
    if(_v<2) {
        r->register_set->misc.f.bst_size = _v;
        // 0b00: 8(fixed), 0b10: 4(fixed)
        if(DDR_TYPE_DDR4==r->cntlr_opt->dram_type) {
            DDR4_MR0_T mr0 = {.v=r->register_set->mr0.v };
            mr0.f.bl = (1==_v)?0:2;
            r->register_set->mr0.v = mr0.v;
        } else {
            DDR3_MR0_T mr0 = {.v=r->register_set->mr0.v };
            mr0.f.bl = (1==_v)?0:2;
            r->register_set->mr0.v = mr0.v;
        }

    } else {
        ISTAT_SET(xlat, MEM_BL_ERROR);
    }
    return;
}

MEMCNTLR_SECTION
void mc_xlat_ref_num(mc_info_t *r) {
    u32_t _v = r->cntlr_opt->ref_num;
    if(_v<16) {
        r->register_set->drr.f.ref_num = r->cntlr_opt->ref_num;
    } else {
        ISTAT_SET(xlat, MEM_REF_NUM_ERROR);
    }
    return;
}

MEMCNTLR_SECTION
void mc_xlat_min_tref_ns(mc_info_t *r) {
    r->register_set->drr.f.tref = NS2CK2(r->cntlr_opt->min_tref_ns);
    return;
}

MEMCNTLR_SECTION
void mc_xlat_min_trfc_ns(mc_info_t *r) {
    r->register_set->drr.f.trfc = NS2CK2(r->cntlr_opt->min_trfc_ns);
    return;
}

MEMCNTLR_SECTION
void mc_xlat_min_tzqcs_tck(mc_info_t *r) {
    r->register_set->tpr0.f.tzqcs = ROUNDUP(r->cntlr_opt->min_tzqcs_tck, DFI_RATE);
    return;
}

MEMCNTLR_SECTION
void mc_xlat_min_tcke_ns(mc_info_t *r) {
    r->register_set->tpr0.f.tcke = NS2CK2(r->cntlr_opt->min_tcke_ns);
    return;
}

MEMCNTLR_SECTION
void mc_xlat_min_trtp_ns(mc_info_t *r) {
    r->register_set->tpr0.f.trtp = NS2CK2(r->cntlr_opt->min_trtp_ns);
    return;
}

MEMCNTLR_SECTION
void mc_xlat_min_twr_ns(mc_info_t *r) {
    u32_t _v = ROUNDUP(r->cntlr_opt->min_twr_ns, DFI_RATE);
    r->register_set->tpr0.f.twr = NS2CK2(r->cntlr_opt->min_twr_ns);
    if(DDR_TYPE_DDR4==r->cntlr_opt->dram_type) {
        DDR4_MR0_T mr0 = {.v=r->register_set->mr0.v };
        mr0.f.wr_rtp = (24==_v)?6:(_v-10)/2;
        r->register_set->mr0.v=mr0.v;
    } else {
        DDR3_MR0_T mr0 = {.v=r->register_set->mr0.v };
        mr0.f.wr = (_v>10)?((_v/2)&0x7):(_v-4);
        r->register_set->mr0.v=mr0.v;
    }
    return;
}

MEMCNTLR_SECTION
void mc_xlat_min_tras_ns(mc_info_t *r) {
    r->register_set->tpr0.f.tras = NS2CK2(r->cntlr_opt->min_tras_ns);
    return;
}

MEMCNTLR_SECTION
void mc_xlat_min_trp_ns(mc_info_t *r) {
    r->register_set->tpr0.f.trp = NS2CK2(r->cntlr_opt->min_trp_ns);
    return;
}

MEMCNTLR_SECTION
void mc_xlat_min_tfaw_ns(mc_info_t *r) {
    r->register_set->tpr1.f.tfaw = NS2CK2(r->cntlr_opt->min_tfaw_ns);
    return;
}

MEMCNTLR_SECTION
void mc_xlat_min_trtw_tck(mc_info_t *r) {
    r->register_set->tpr1.f.trtw = ROUNDUP(r->cntlr_opt->min_trtw_tck, DFI_RATE);
    return;
}

MEMCNTLR_SECTION
void mc_xlat_min_twtr_ns(mc_info_t *r) {
    r->register_set->tpr1.f.twtr = NS2CK1(r->cntlr_opt->min_twtr_ns);
    return;
}

MEMCNTLR_SECTION
void mc_xlat_min_tccd_tck(mc_info_t *r) {
    r->register_set->tpr1.f.tccd = ROUNDUP(r->cntlr_opt->min_tccd_tck, DFI_RATE);
    return;
}

MEMCNTLR_SECTION
void mc_xlat_min_trcd_ns(mc_info_t *r) {
    r->register_set->tpr1.f.trcd = NS2CK2(r->cntlr_opt->min_trcd_ns);
    return;
}

MEMCNTLR_SECTION
void mc_xlat_min_trc_ns(mc_info_t *r) {
    r->register_set->tpr1.f.trc = NS2CK2(r->cntlr_opt->min_trc_ns);
    return;
}

MEMCNTLR_SECTION
void mc_xlat_min_trrd_ns(mc_info_t *r) {
    r->register_set->tpr1.f.trrd = NS2CK2(r->cntlr_opt->min_trrd_ns);
    return;
}

MEMCNTLR_SECTION
void mc_xlat_min_rst_us(mc_info_t *r) {
    u32_t _t = (0==r->cntlr_opt->min_rst_us)?300:r->cntlr_opt->min_rst_us; // should be 200us, setting more
    u32_t _v = ROUNDUP(_t*1000, 1024);  // time = rst_tus*2^10 cycles, RST keep low time
    r->register_set->tpr2.f.rst_tus = NS2CK2(_v);
    return;
}

MEMCNTLR_SECTION
void mc_xlat_min_cke_us(mc_info_t *r) {
    u32_t _t = (0==r->cntlr_opt->min_cke_us)?600:r->cntlr_opt->min_cke_us; // should be 500us, setting more
    u32_t _v = ROUNDUP(_t*1000, 1024);  // time = tus*2^10 cycles, CKE stable time
    r->register_set->tpr2.f.tus = NS2CK2(_v);
    return;
}

MEMCNTLR_SECTION
void mc_xlat_min_tmrd_tck(mc_info_t *r) {
    r->register_set->tpr2.f.tmrd = ROUNDUP(r->cntlr_opt->min_tmrd_tck, DFI_RATE);
    return;
}

MEMCNTLR_SECTION
void mc_xlat_min_tccd_r_tck(mc_info_t *r) {
    r->register_set->tpr3.f.tccd_r = ROUNDUP(r->cntlr_opt->min_tccd_r_tck, DFI_RATE);
    return;
}

MEMCNTLR_SECTION
void mc_xlat_min_twtr_s_tck(mc_info_t *r) {
    r->register_set->tpr3.f.twtr_s = ROUNDUP(r->cntlr_opt->min_twtr_s_tck, DFI_RATE);
    return;
}

MEMCNTLR_SECTION
void mc_xlat_min_tccd_s_tck(mc_info_t *r) {
    r->register_set->tpr3.f.tccd_s = ROUNDUP(r->cntlr_opt->min_tccd_s_tck, DFI_RATE);
    return;
}

MEMCNTLR_SECTION
void mc_xlat_min_dpin_cmd_lat(mc_info_t *r) {
    r->register_set->tpr3.f.dpin_cmd_lat = r->cntlr_opt->min_dpin_cmd_lat;
}

MEMCNTLR_SECTION
void mc_xlat_add_lat(mc_info_t *r) {
    r->register_set->mrinfo.f.add_lat = r->cntlr_opt->add_lat;
    //FIXME
    return;
}

MEMCNTLR_SECTION
void mc_xlat_rd_lat(mc_info_t *r) {
    u32_t _v = r->cntlr_opt->rd_lat;
    r->register_set->mrinfo.f.rd_lat = ROUNDUP(_v, DFI_RATE); // CHECK
    if(DDR_TYPE_DDR4==r->cntlr_opt->dram_type) {
        DDR4_MR0_T mr0 = {.v=r->register_set->mr0.v };
        if(_v>=9 && _v<=16) {
            mr0.f.cl_h = (_v-9)>>1;
            mr0.f.cl_l = (_v-9)&0x1;
        } else if(_v>16 && _v<=22) {
            u32_t t;
            if((_v&0x1)==1) { // 17, 19, 21
                t=13+(_v-17)/2;
            } else {    // 18, 20, 22, 24
                t=8+(_v-18)/2;
            }
            mr0.f.cl_h = t>>1;
            mr0.f.cl_l = t&0x1;
        } else if(24==_v) {
            mr0.f.cl_h = 5;
            mr0.f.cl_l = 1;
        } else {
            ISTAT_SET(xlat, MEM_CL_ERROR);
            return;
        }
        r->register_set->mr0.v = mr0.v;
    } else {
        DDR3_MR0_T mr0 = {.v=r->register_set->mr0.v };
        if(_v<12) {
            mr0.f.cl_h = _v-4;
        } else {
            mr0.f.cl_h = _v-12;
            mr0.f.cl_l = 1;
        }
        r->register_set->mr0.v = mr0.v;
    }

    return;
}

MEMCNTLR_SECTION
void mc_xlat_wr_lat(mc_info_t *r) {
    u32_t _v = r->cntlr_opt->wr_lat + r->cntlr_opt->twpre;
    r->register_set->mrinfo.f.wr_lat = ROUNDUP(_v, DFI_RATE);    // CHECK
    if(DDR_TYPE_DDR4==r->cntlr_opt->dram_type) {
        DDR4_MR2_T mr2 = {.v=r->register_set->mr2.v};
        // 9~12
        if(_v <= 12) {
            mr2.f.cwl = _v-9;
        } else if(_v >= 14 && _v <= 18) {
            mr2.f.cwl = 4+(_v-14)/2;
        } else {
            ISTAT_SET(xlat, MEM_CWL_ERROR);
            return;
        }
        r->register_set->mr2.v = mr2.v;
    } else {
        DDR3_MR2_T mr2 = {.v=r->register_set->mr2.v};
        mr2.f.cwl = _v-5;
        r->register_set->mr2.v = mr2.v;
    }
    return;
}

MEMCNTLR_SECTION
void mc_xlat_dq32_en(mc_info_t *r) {
    u8_t dt = get_dq32_status();
    r->register_set->dcr.f.dq_32 = dt;
    r->register_set->dcr.f.half_dq = ~dt;
    return;
}

MEMCNTLR_SECTION
void mc_xlat_zqc_en(mc_info_t *r) {
    r->register_set->dcr.f.zqc = r->cntlr_opt->zqc_en;
}

MEMCNTLR_SECTION
void mc_xlat_gear_down(mc_info_t *r) {
    if(DDR_TYPE_DDR4==r->cntlr_opt->dram_type) {
        r->register_set->dcr.f.gear_down = r->cntlr_opt->gear_down;
        DDR4_MR3_T mr3 = {.v=r->register_set->mr3.v};
        mr3.f.geardown = r->cntlr_opt->gear_down;
        r->register_set->mr3.v = mr3.v;
    }
}

MEMCNTLR_SECTION
void mc_xlat_tphy_rdata(mc_info_t *r) {
    r->register_set->iocr.f.thpy_rdata = r->cntlr_opt->tphy_rdata;
    return;
}

MEMCNTLR_SECTION
void mc_xlat_tphy_wlat(mc_info_t *r) {
    r->register_set->iocr.f.tphy_wlat = r->cntlr_opt->tphy_wlat;
    return;
}

MEMCNTLR_SECTION
void mc_xlat_tphy_wdata(mc_info_t *r) {
    r->register_set->iocr.f.tphy_wdata = r->cntlr_opt->tphy_wdata;
    return;
}

MEMCNTLR_SECTION
void mc_xlat_stc_odt_en(mc_info_t *r) {
    r->register_set->iocr.f.stc_odt = r->cntlr_opt->stc_odt_en;
    return;
}

MEMCNTLR_SECTION
void mc_xlat_stc_cke_en(mc_info_t *r) {
    r->register_set->iocr.f.stc_cke = r->cntlr_opt->stc_cke_en;
    r->register_set->tpr0.f.txp = !(r->cntlr_opt->stc_cke_en);

    return;
}

MEMCNTLR_SECTION
void mc_xlat_ref_dis(mc_info_t *r) {
    r->register_set->drr.f.ref_dis = r->cntlr_opt->ref_dis;
    return;
}

MEMCNTLR_SECTION
void mc_xlat_rttnom(mc_info_t *r) {
    // FIXME, DDR4 RTTNOM
    u32_t rttnom = r->cntlr_opt->rttnom;
    if(DDR_TYPE_DDR4==r->cntlr_opt->dram_type) {
        #define RZQ     (240)
        DDR4_MR1_T mr1 = {.v=r->register_set->mr1.v};
        u32_t v = RZQ/rttnom;
        mr1.f.rtt_nom = ((v&0x1)<<2 | (v&0x2) | ((v>>2)&0x1));
        r->register_set->mr1.v = mr1.v;
    } else {
        DDR3_MR1_T mr1 = {.v=r->register_set->mr1.v};
        switch (rttnom) {
        case 0:
            mr1.f.rtt_nom_h = 0;
            mr1.f.rtt_nom_m = 0;
            mr1.f.rtt_nom_l = 0;
            break;
        case 20:
        case 30:
        case 40:
        case 60:
        case 120:
            mr1.f.rtt_nom_h = (rttnom<=30)?1:0;
            mr1.f.rtt_nom_m = (rttnom%40)==0?1:0;
            mr1.f.rtt_nom_l = ((rttnom>20)&&(rttnom<120))?1:0;
            break;
        default:
            ISTAT_SET(xlat, MEM_RTTNOM_ERROR);
            return;
        }
        r->register_set->mr1.v=mr1.v;
    }
}

MEMCNTLR_SECTION
void mc_xlat_rttwr(mc_info_t *r) {
    u32_t rttwr = r->cntlr_opt->rttwr;
    if(DDR_TYPE_DDR4==r->cntlr_opt->dram_type) {
        DDR4_MR2_T mr2 = {.v=r->register_set->mr2.v};
        u32_t v;
        switch(rttwr) {
            case 0: // disabled
                v=0;
                break;
            case 1: // Hi-Z
                v=3;
                break;
            case 80:
                v=4;
                break;
            case 120:
                v=1;
                break;
            case 240:
                v=2;
                break;
            default:
                ISTAT_SET(xlat, MEM_RTTWR_ERROR);
                return;
        }
        mr2.f.rtt_wr=v;
        r->register_set->mr2.v = mr2.v;
    } else {
        DDR3_MR2_T mr2 = {.v=r->register_set->mr2.v};
        if(((rttwr % 60) != 0) ||
            (rttwr > 120)) {
            ISTAT_SET(xlat, MEM_RTTWR_ERROR);
            return;
        } else {
            mr2.f.rtt_wr = (rttwr / 60);
        }
        r->register_set->mr2.v = mr2.v;
    }
}

MEMCNTLR_SECTION
void mc_xlat_odic(mc_info_t *r) {
    u32_t odi = r->cntlr_opt->odic;
    if(DDR_TYPE_DDR4==r->cntlr_opt->dram_type) {
        DDR4_MR1_T mr1 = {.v=r->register_set->mr1.v};
        mr1.f.odi = (34==odi)?0:1;  // 00: 34ohm, 01: 48ohm
        r->register_set->mr1.v = mr1.v;
    } else {
        DDR3_MR1_T mr1 = {.v=r->register_set->mr1.v};
        mr1.f.odic_h=0;
        mr1.f.odic_l=(34==odi)?1:0; // 00: 40ohm, 01: 34ohm
        r->register_set->mr1.v = mr1.v;
    }
}

MEMCNTLR_SECTION
void mc_xlat_twpre(mc_info_t *r) {
    if(DDR_TYPE_DDR4==r->cntlr_opt->dram_type) {
        DDR4_MR4_T mr4 = {.v=r->register_set->mr4.v};
        mr4.f.wr_preamble = r->cntlr_opt->twpre;
        r->register_set->mr4.v = mr4.v;
    }
}

MEMCNTLR_SECTION
void mc_xlat_trpre(mc_info_t *r) {
    if(DDR_TYPE_DDR4==r->cntlr_opt->dram_type) {
        DDR4_MR4_T mr4 = {.v=r->register_set->mr4.v};
        mr4.f.rd_preamble = r->cntlr_opt->trpre;
        //mr4.f.rd_prea_train_mode = r->cntlr_opt->trpre;
        r->register_set->mr4.v = mr4.v;
    }
}

MEMCNTLR_SECTION
void mc_xlat_ddr_vref_s(mc_info_t *r) {
    if(DDR_TYPE_DDR4==r->cntlr_opt->dram_type) {
        DDR4_MR6_T mr6 = {.v=r->register_set->mr6.v};
        mr6.f.vref_t_en = 1;
        mr6.f.vref_t_val = r->dpi_opt->ddr_vref_s;
        r->register_set->mr6.v = mr6.v;
    }
}

MEMCNTLR_SECTION
void mc_xlat_ddr_vref_r(mc_info_t *r) {
    if(DDR_TYPE_DDR4==r->cntlr_opt->dram_type) {
        DDR4_MR6_T mr6 = {.v=r->register_set->mr6.v};
        mr6.f.vref_t_range = r->dpi_opt->ddr_vref_r;
        r->register_set->mr6.v = mr6.v;
    }
}

DEFINE_RXI310_VAR(dram_type, cntlr_opt, mc_xlat_dram_type);
DEFINE_RXI310_VAR(bankcnt, cntlr_opt, mc_xlat_bankcnt);
DEFINE_RXI310_VAR(pagesize, cntlr_opt, mc_xlat_pagesize);
DEFINE_RXI310_VAR(burst, cntlr_opt, mc_xlat_burst);
DEFINE_RXI310_VAR(ref_num, cntlr_opt, mc_xlat_ref_num);
DEFINE_RXI310_VAR(min_tref_ns, cntlr_opt, mc_xlat_min_tref_ns);
DEFINE_RXI310_VAR(min_trfc_ns, cntlr_opt, mc_xlat_min_trfc_ns);
DEFINE_RXI310_VAR(min_tzqcs_tck, cntlr_opt, mc_xlat_min_tzqcs_tck);
DEFINE_RXI310_VAR(min_tcke_ns, cntlr_opt, mc_xlat_min_tcke_ns);
DEFINE_RXI310_VAR(min_trtp_ns, cntlr_opt, mc_xlat_min_trtp_ns);
DEFINE_RXI310_VAR(min_twr_ns, cntlr_opt, mc_xlat_min_twr_ns);
DEFINE_RXI310_VAR(min_tras_ns, cntlr_opt, mc_xlat_min_tras_ns);
DEFINE_RXI310_VAR(min_trp_ns, cntlr_opt, mc_xlat_min_trp_ns);
DEFINE_RXI310_VAR(min_tfaw_ns, cntlr_opt, mc_xlat_min_tfaw_ns);
DEFINE_RXI310_VAR(min_trtw_tck, cntlr_opt, mc_xlat_min_trtw_tck);
DEFINE_RXI310_VAR(min_twtr_ns, cntlr_opt, mc_xlat_min_twtr_ns);
DEFINE_RXI310_VAR(min_tccd_tck, cntlr_opt, mc_xlat_min_tccd_tck);
DEFINE_RXI310_VAR(min_trcd_ns, cntlr_opt, mc_xlat_min_trcd_ns);
DEFINE_RXI310_VAR(min_trc_ns, cntlr_opt, mc_xlat_min_trc_ns);
DEFINE_RXI310_VAR(min_trrd_ns, cntlr_opt, mc_xlat_min_trrd_ns);
DEFINE_RXI310_VAR(min_rst_us, cntlr_opt, mc_xlat_min_rst_us);
DEFINE_RXI310_VAR(min_cke_us, cntlr_opt, mc_xlat_min_cke_us);
DEFINE_RXI310_VAR(min_tmrd_tck, cntlr_opt, mc_xlat_min_tmrd_tck);
DEFINE_RXI310_VAR(min_tccd_r_tck, cntlr_opt, mc_xlat_min_tccd_r_tck);
DEFINE_RXI310_VAR(min_twtr_s_tck, cntlr_opt, mc_xlat_min_twtr_s_tck);
DEFINE_RXI310_VAR(min_tccd_s_tck, cntlr_opt, mc_xlat_min_tccd_s_tck);
DEFINE_RXI310_VAR(min_dpin_cmd_lat, cntlr_opt, mc_xlat_min_dpin_cmd_lat);
DEFINE_RXI310_VAR(add_lat, cntlr_opt, mc_xlat_add_lat);
DEFINE_RXI310_VAR(rd_lat, cntlr_opt, mc_xlat_rd_lat);
DEFINE_RXI310_VAR(wr_lat, cntlr_opt, mc_xlat_wr_lat);
DEFINE_RXI310_VAR(dq32_en, cntlr_opt, mc_xlat_dq32_en);
DEFINE_RXI310_VAR(zqc_en, cntlr_opt, mc_xlat_zqc_en);
DEFINE_RXI310_VAR(gear_down, cntlr_opt, mc_xlat_gear_down);

DEFINE_RXI310_VAR(tphy_rdata, cntlr_opt, mc_xlat_tphy_rdata);
DEFINE_RXI310_VAR(tphy_wlat, cntlr_opt, mc_xlat_tphy_wlat);
DEFINE_RXI310_VAR(tphy_wdata, cntlr_opt, mc_xlat_tphy_wdata);
DEFINE_RXI310_VAR(stc_odt_en, cntlr_opt, mc_xlat_stc_odt_en);
DEFINE_RXI310_VAR(stc_cke_en, cntlr_opt, mc_xlat_stc_cke_en);
DEFINE_RXI310_VAR(ref_dis, cntlr_opt, mc_xlat_ref_dis);
DEFINE_RXI310_VAR(rttnom, cntlr_opt, mc_xlat_rttnom);
DEFINE_RXI310_VAR(rttwr, cntlr_opt, mc_xlat_rttwr);
DEFINE_RXI310_VAR(odic, cntlr_opt, mc_xlat_odic);
DEFINE_RXI310_VAR(trpre, cntlr_opt, mc_xlat_trpre);
DEFINE_RXI310_VAR(twpre, cntlr_opt, mc_xlat_twpre);
DEFINE_RXI310_VAR(ddr_vref_s, dpi_opt, mc_xlat_ddr_vref_s);
DEFINE_RXI310_VAR(ddr_vref_r, dpi_opt, mc_xlat_ddr_vref_r);

MEMCNTLR_SECTION
unsigned int mc_xlat_ocd_odt(unsigned int ocd, unsigned int odt) {
    u32_t zprog = 0;
    return zprog;
}

u16_t jddr3_trfc[5] = { 90, 110, 160, 300, 350}; // 64MB ~ 1GB
u16_t jddr4_trfc[3] = { 160, 260, 350};          // 256MB ~ 1GB

MEMCNTLR_SECTION
void mc_update_trfc(mc_info_t *r, u32_t size) {
    u32_t base, i=0;
    u16_t *trfc_ary;
    if(DDR_TYPE_DDR4==r->cntlr_opt->dram_type) {
        base = 256; // 256MB
        trfc_ary = jddr4_trfc;
    } else {
        base = 64; // 64MB
        trfc_ary = jddr3_trfc;
    }
    while(1) {
        if(size == (base<<i)) {
            r->register_set->drr.f.trfc = NS2CK2(trfc_ary[i]);
            DRRrv = r->register_set->drr.v;
            break;
        }
        i++;
        if((base<<i) > 1024) break; // max. size is 1GB now (2018.4)
    }
}

MEMCNTLR_SECTION
void mc_dram_param_dump(mc_info_t *info,
                    const mc_var_t *begin,
                    const mc_var_t *end,
                    int order) {
    int step=1;
    if(order<0) {
        const mc_var_t *tmp=begin;
        step=-1;
        begin=end-1;
        end=tmp-1;
    }
    const mc_var_t *idx=begin;
    while (end != idx) {
        idx->dump(info);
        idx+=step;
    }
}

MEMCNTLR_SECTION
void dram_dump_info(void) {
    return mc_dram_param_dump(&meminfo, &mc_var_begin, &mc_var_end, 1);
}

MEMCNTLR_SECTION
void mc_xlat_n_assign(mc_info_t *info,
                      const mc_var_t *begin,
                      const mc_var_t *end,
                      int order) {
    int step=1;
    if(order<0) {
        const mc_var_t *tmp=begin;
        step=-1;
        begin=end-1;
        end=tmp-1;
    }
    const mc_var_t *idx=begin;
    while (end != idx) {
        idx->xlat(info);
        idx+=step;
    }
}

MEMCNTLR_SECTION
void mc_info_translation(void) {
    ISTAT_SET(xlat, MEM_XLAT_UNINIT);

    if(ISTAT_GET(probe) != MEM_PROBE_OK) {
        return;
    }

    mc_xlat_n_assign(&meminfo, &mc_var_begin, &mc_var_end, 1);

    if(ISTAT_GET(xlat) == MEM_XLAT_UNINIT) {
        ISTAT_SET(xlat, MEM_XLAT_OK);
    }

    return;
}

MEMCNTLR_SECTION
__attribute__((weak))
void mc_cntlr_zq_static_patch(void) {}

/* __attribute__((weak))
u32_t zq_calibration(void) { return 0; } */

MEMCNTLR_SECTION
void mc_cntlr_zq_calibration(void) {
    ISTAT_SET(cntlr_zq, MEM_CNTLR_ZQ_R480_UNINIT);

    if(ISTAT_GET(to_reg) != MEM_TO_REG_OK) {
        return;
    }
#ifndef PROJECT_ON_FPGA
    u32_t res;
    if(0==meminfo.dpi_opt->rzq_ext) {
        res=dpi_r480_calibration();
        if(MEM_CNTLR_ZQ_R480_UNINIT != res) {
            ISTAT_SET(cntlr_zq, res);
            return;
        }
    } else {
        puts("II: using external R480\n");
        RMOD_PAD_CTRL_PROG(rzq_ext_r240, 1);
    }

    res=dpi_zq_calibration(meminfo.dpi_opt->ocd_odt);
    if(MEM_CNTLR_ZQ_R480_UNINIT != res) {
        u32_t cnt=1;
        while(cnt<5) {
            WARN_PRINTF("Retry %d times ...\n", cnt);
            res=dpi_zq_calibration(meminfo.dpi_opt->ocd_odt);
            if(MEM_CNTLR_ZQ_R480_UNINIT == res) {
                goto PASS;
            }
            cnt++;
        }
        ISTAT_SET(cntlr_zq, res);
        return;
    }
    
    dpi_vref_dq_setting();
PASS:
#endif
    if(ISTAT_GET(cntlr_zq) == MEM_CNTLR_ZQ_R480_UNINIT) {
        ISTAT_SET(cntlr_zq, MEM_CNTLR_ZQ_R480_OK);
    }

    return;
}

MEMCNTLR_SECTION
void mc_info_to_reg(void) {
    ISTAT_SET(to_reg, MEM_TO_REG_UNINIT);

    if(ISTAT_GET(xlat) != MEM_XLAT_OK) {
        return;
    }

    mc_register_set_t *rs     = meminfo.register_set;

    // applied parameters
    MISCrv    = rs->misc.v;
    rs->dcr.f.dfi_rate = 2; // only support dfi rate = 2:1 now
    DCRrv     = rs->dcr.v;
    IOCRrv    = rs->iocr.v;
    MR0rv     = rs->mr0.v;
    MR1rv     = rs->mr1.v;
    MR2rv     = rs->mr2.v;
    MR3rv     = rs->mr3.v;
    MR4rv     = rs->mr4.v;
    MR5rv     = rs->mr5.v;
    MR6rv     = rs->mr6.v;
    MR_INFOrv = rs->mrinfo.v;
    DRRrv     = rs->drr.v;
    TPR0rv    = rs->tpr0.v;
    TPR1rv    = rs->tpr1.v;
    TPR2rv    = rs->tpr2.v;
    TPR3rv    = rs->tpr3.v;

    if(ISTAT_GET(to_reg) == MEM_TO_REG_UNINIT) {
        ISTAT_SET(to_reg, MEM_TO_REG_OK);
    }
    return;
}

extern void mc_dram_size_detect(void);

MEMCNTLR_SECTION
void mc_rxi310_init(u8_t silence) {

    // rxi310 init
    CSRrv = 0x700;      // disable mem access, disable bstc func.
    CCRrv = 0x1;

    u32_t cntdown = 0x10000000;
    while(0==RFLD_CCR(init)) {
        cntdown--;
        if(0==cntdown) {
            break;
        }
    }
    CSRrv = 0x600;      // enable mem access, disable bstc func.

    if(0==cntdown) {
        puts("II: RXI310 Init timeout\n");
    } else if(0==silence) {
        puts("II: RXI310 Init\n");
    }

    if(DDR_TYPE_DDR4==meminfo.cntlr_opt->dram_type) {
        DDR4_MR6_T mr6 = { .v = meminfo.register_set->mr6.v };
        if(mr6.f.vref_t_en) {   // training mode on, do exit
            CSRrv = 0x700;      // disable mem access, disable bstc func.
                                // FIXME, function???
            // for mr6.f.vref_t_en=0;
            CMD_DPINrv = 0x820C0010;
            CCRrv = 0x8;
            while(0==RFLD_CCR(dpit)) {
                cntdown--;
                if(0==cntdown) break;
            }
            CSRrv = 0x600;
        }
    }
    // add some delay for DPIN/BSTC mode change back to DRAM mode
    udelay(50);

    return;
}

MEMCNTLR_SECTION
u32_t dpi_setting(void) {
    INFO_PRINTF("DCK_PI:%2d, CS_PI:%2d\n",
        meminfo.dpi_opt->dck_post_pi, meminfo.dpi_opt->cs_post_pi);
    INFO_PRINTF("DQS_PI_0:%2d, DQS_PI_1:%2d, DQS_PI_2:%2d, DQS_PI_3:%2d\n",
        meminfo.dpi_opt->dqs_post_pi_0, meminfo.dpi_opt->dqs_post_pi_1,
        meminfo.dpi_opt->dqs_post_pi_2, meminfo.dpi_opt->dqs_post_pi_3);
    INFO_PRINTF("DQ_PI_0 :%2d, DQ_PI_1 :%2d, DQ_PI_2 :%2d, DQ_PI_3 :%2d\n",
        meminfo.dpi_opt->dq_post_pi_0, meminfo.dpi_opt->dq_post_pi_1,
        meminfo.dpi_opt->dq_post_pi_2, meminfo.dpi_opt->dq_post_pi_3);

    u32_t dc0=DPI_CTRL_0rv;
    READ_CTRL_0_0rv = meminfo.dpi_opt->dqs_en;//0x3;
    READ_CTRL_0_1rv = meminfo.dpi_opt->dqs_en;//0x3;
    READ_CTRL_0_2rv = meminfo.dpi_opt->dqs_en;//0x3;
    READ_CTRL_0_3rv = meminfo.dpi_opt->dqs_en;//0x3;
    // Read FIFO
    READ_CTRL_1rv = meminfo.dpi_opt->rd_fifo;//0x4;
    // ODT EN
    APPLY_TM_ODT_EN_OSD(meminfo.dpi_opt->rd_odt_odd);
    APPLY_TM_ODT_EN(meminfo.dpi_opt->rd_odt);

    INFO_PRINTF("DQS_EN:%2d, RD_FIFO:%2d, ODT_EN_ODD:0x%x, ODT_EN:0x%x\n",
        meminfo.dpi_opt->dqs_en, meminfo.dpi_opt->rd_fifo,
        meminfo.dpi_opt->rd_odt_odd, meminfo.dpi_opt->rd_odt);
#if 0
    INFO_PRINTF("ODT force on\n");
    RX_ODT_ALWAYS_ON(1);
#endif

    APPLY_DQS_IN_DLY(DQS_IN_DLY_0, fw_rd_dly_pos_sel_0, meminfo.dpi_opt->rd_dly_pos_dq0);
    APPLY_DQS_IN_DLY(DQS_IN_DLY_0, fw_rd_dly_pos_sel_1, meminfo.dpi_opt->rd_dly_pos_dq1);
    APPLY_DQS_IN_DLY(DQS_IN_DLY_0, fw_rd_dly_pos_sel_2, meminfo.dpi_opt->rd_dly_pos_dq2);
    APPLY_DQS_IN_DLY(DQS_IN_DLY_0, fw_rd_dly_pos_sel_3, meminfo.dpi_opt->rd_dly_pos_dq3);
    APPLY_DQS_IN_DLY(DQS_IN_DLY_1, fw_rd_dly_pos_sel_4, meminfo.dpi_opt->rd_dly_pos_dq4);
    APPLY_DQS_IN_DLY(DQS_IN_DLY_1, fw_rd_dly_pos_sel_5, meminfo.dpi_opt->rd_dly_pos_dq5);
    APPLY_DQS_IN_DLY(DQS_IN_DLY_1, fw_rd_dly_pos_sel_6, meminfo.dpi_opt->rd_dly_pos_dq6);
    APPLY_DQS_IN_DLY(DQS_IN_DLY_1, fw_rd_dly_pos_sel_7, meminfo.dpi_opt->rd_dly_pos_dq7);
    APPLY_DQS_IN_DLY(DQS_IN_DLY_2, fw_rd_dly_neg_sel_0, meminfo.dpi_opt->rd_dly_neg_dq0);
    APPLY_DQS_IN_DLY(DQS_IN_DLY_2, fw_rd_dly_neg_sel_1, meminfo.dpi_opt->rd_dly_neg_dq1);
    APPLY_DQS_IN_DLY(DQS_IN_DLY_2, fw_rd_dly_neg_sel_2, meminfo.dpi_opt->rd_dly_neg_dq2);
    APPLY_DQS_IN_DLY(DQS_IN_DLY_2, fw_rd_dly_neg_sel_3, meminfo.dpi_opt->rd_dly_neg_dq3);
    APPLY_DQS_IN_DLY(DQS_IN_DLY_3, fw_rd_dly_neg_sel_4, meminfo.dpi_opt->rd_dly_neg_dq4);
    APPLY_DQS_IN_DLY(DQS_IN_DLY_3, fw_rd_dly_neg_sel_5, meminfo.dpi_opt->rd_dly_neg_dq5);
    APPLY_DQS_IN_DLY(DQS_IN_DLY_3, fw_rd_dly_neg_sel_6, meminfo.dpi_opt->rd_dly_neg_dq6);
    APPLY_DQS_IN_DLY(DQS_IN_DLY_3, fw_rd_dly_neg_sel_7, meminfo.dpi_opt->rd_dly_neg_dq7);
    APPLY_DQ_DLY(DQ_DLY_0, dq0_dly_sel, meminfo.dpi_opt->dq0_dly);
    APPLY_DQ_DLY(DQ_DLY_0, dq1_dly_sel, meminfo.dpi_opt->dq1_dly);
    APPLY_DQ_DLY(DQ_DLY_0, dq2_dly_sel, meminfo.dpi_opt->dq2_dly);
    APPLY_DQ_DLY(DQ_DLY_0, dq3_dly_sel, meminfo.dpi_opt->dq3_dly);
    APPLY_DQ_DLY(DQ_DLY_0, dq4_dly_sel, meminfo.dpi_opt->dq4_dly);
    APPLY_DQ_DLY(DQ_DLY_0, dq5_dly_sel, meminfo.dpi_opt->dq5_dly);
    APPLY_DQ_DLY(DQ_DLY_0, dq6_dly_sel, meminfo.dpi_opt->dq6_dly);
    APPLY_DQ_DLY(DQ_DLY_0, dq7_dly_sel, meminfo.dpi_opt->dq7_dly);
    APPLY_DM_DLY(dm_dly_sel_0, meminfo.dpi_opt->dm0_dly);
    APPLY_DM_DLY(dm_dly_sel_1, meminfo.dpi_opt->dm1_dly);
    APPLY_DM_DLY(dm_dly_sel_2, meminfo.dpi_opt->dm2_dly);
    APPLY_DM_DLY(dm_dly_sel_3, meminfo.dpi_opt->dm3_dly);

    DPI_CTRL_0rv = 0x082e2002;
    DPI_CTRL_1rv = 0x0000000C;
    mc_rxi310_init(1);

    // restore DPI_CTRL_0rv
    DPI_CTRL_0rv = dc0;
    return 0;
}
extern u32_t *pat_ary;
extern u32_t dpi_calibration(void);
extern u32_t dpi_pi_scan(PI_CALI_T *info, u8_t dbg);
extern int prb7_pattern_gen(u32_t *ary);
MEMCNTLR_SECTION
void mc_calibration(void) {

    ISTAT_SET(cal, MEM_CAL_UNINIT);

    if(ISTAT_GET(cntlr_zq) != MEM_CNTLR_ZQ_R480_OK) {
        return;
    }
    //PI_CALI_T info;
    //memset((char *)&info, 0, sizeof(PI_CALI_T));
    //prb7_pattern_gen(pat_ary);
    //dpi_pi_scan(&info, 0);

#ifndef PROJECT_ON_FPGA
    dpi_setting();
#endif
    if(ISTAT_GET(cal) == MEM_CAL_UNINIT) {
        ISTAT_SET(cal, MEM_CAL_OK);
    }

    return;
}

MEMCNTLR_SECTION
__attribute__((weak))
void mc_result_show_dram_config(void);

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

    if(mc_result_show_dram_config)
        mc_result_show_dram_config();

    return;
}

REG_INIT_FUNC(mc_info_probe,           10);
REG_INIT_FUNC(mc_info_translation,     12);
REG_INIT_FUNC(mc_info_to_reg,          14);
REG_INIT_FUNC(mc_cntlr_zq_calibration, 16);
REG_INIT_FUNC(mc_calibration,          18);
REG_INIT_FUNC(mc_result_decode,        22);