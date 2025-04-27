#ifndef _MEMCNTLR_H_
#define _MEMCNTLR_H_

#include <soc.h>
#include <dram/memcntlr_reg.h>
#include <dram/memcntlr_util.h>

typedef union {
        struct {
                unsigned int dontcare:11;
                unsigned int ttfp:3;
                unsigned int ttcp:3;
                unsigned int plsb_1:1;
                unsigned int ttfn:3;
                unsigned int ttcn:3;
                unsigned int pt:3;
                unsigned int plsb_0:1;
                unsigned int nt:3;
                unsigned int e2:1;
        } f;
        unsigned int v;
} zctrl_impedance_t;

typedef union {
        struct {
                unsigned int mbz_0:6; //0
                unsigned int dtn:2; //0
                unsigned int dtp:2; //0
                unsigned int force:1; //0
                unsigned int drv:1; //0
                unsigned int nt:3; //0
                unsigned int pt:3; //0
                unsigned int plsb:2; //0
                unsigned int ttcn:3; //0
                unsigned int ttcp:3; //0
                unsigned int ttfn:3; //0
                unsigned int ttfp:3; //0
        } f;
        unsigned int v;
} DDR_IOPDCR_TYPE0_T;

typedef union {
        struct {
                unsigned int mbz:6; //0
                unsigned int drv:1; //0
                unsigned int nt:3; //0
                unsigned int pt:3; //0
                unsigned int plsb:2; //0
                unsigned int force:1; //0
        } f;
        unsigned short v;
} DDR_IOPDCR_TYPE1_T;

typedef union {
	struct {
		u16_t mbz_0:3;
		u16_t fast_exit_en:1;
		u16_t wr:3;
		u16_t dll_reset:1;
		u16_t test_mode_en:1;
		u16_t cl:3;
		u16_t rd_interleave:1;
		u16_t burst_len:3;
	} f;
	u16_t v;
} ddr2_mr0;

typedef union {
	struct {
		u16_t mbz_0:3;
		u16_t output_buf_dis:1;
		u16_t rdqs_en:1;
		u16_t dqs_en:1;
		u16_t ocd_cal_prog:3;
		u16_t rtt_nom_m:1;
		u16_t al:3;
		u16_t rtt_nom_l:1;
		u16_t output_drv_imp_ctl:1;
		u16_t dll_dis:1;
	} f;
	u16_t v;
} ddr2_emr1;

typedef union {
	struct {
		u16_t mbz_0:8;
		u16_t ext_temp_range_en:1;
		u16_t mbz_1:3;
		u16_t dcc_en:1;
		u16_t partial_ary_self_ref:3;
	} f;
	u16_t v;
} ddr2_emr2;

typedef union {
	struct {
		u16_t mbz_0:16;
	} f;
	u16_t v;
} ddr2_emr3;

typedef union {
	struct {
		u16_t mbz_0:3;
		u16_t fast_exit_en:1;
		u16_t wr:3;
		u16_t dll_reset:1;
		u16_t test_mode_en:1;
		u16_t cl_h:3;
		u16_t rd_interleave:1;
		u16_t cl_l:1;
		u16_t burst_len:2;
	} f;
	u16_t v;
} ddr3_mr0;

typedef union {
	struct {
		u16_t mbz_0:3;
		u16_t output_buf_dis:1;
		u16_t tdqs_en:1;
		u16_t mbz_1:1;
		u16_t rtt_nom_h:1;
		u16_t mbz_2:1;
		u16_t write_lvl_en:1;
		u16_t rtt_nom_m:1;
		u16_t output_drv_imp_ctl_h:1;
		u16_t al:2;
		u16_t rtt_nom_l:1;
		u16_t output_drv_imp_ctl_l:1;
		u16_t dll_dis:1;
	} f;
	u16_t v;
} ddr3_mr1;

typedef union {
	struct {
		u16_t mbz_0:5;
		u16_t rtt_wr:2;
		u16_t mbz_1:1;
		u16_t ext_temp_range_en:1;
		u16_t auto_self_ref_en:1;
		u16_t cwl:3;
		u16_t partial_ary_self_ref:3;
	} f;
	u16_t v;
} ddr3_mr2;

typedef union {
	struct {
		u16_t mbz_0:13;
		u16_t mpr_en:1;
		u16_t mpr_pattern:2;
	} f;
	u16_t v;
} ddr3_mr3;

typedef struct {
	FLD(dram_type,    8);
	FLD(bankcnt,      8);
	FLD(buswidth,     8);
	FLD(max_row_addr, 8);
	FLD(max_col_addr, 8);
	FLD(min_trcd_ns,  10);
	FLD(min_trp_ns,   10);
	FLD(min_tras_ns,  10);
	FLD(cl_ck,        8);
	FLD(cwl_ck,       8);
	FLD(min_twr_ns,   10);
	FLD(min_trtp_ns,  10);
	FLD(min_twtr_ns,  10);
	FLD(min_trrd_ns,  10);
	FLD(min_tfaw_ns,  10);
	FLD(min_trfc_ns,  16);
	FLD(max_trefi_ns, 16);
	FLD(min_tck_ps,   16);
	FLD(max_tck_ps,   16);
} mc_dram_param_t;

typedef struct {
	/* For setting the bus synchronous interface */
	BOOL(bus_sync); /* 0: Async Mode; 1: Sync Mode */

	/* DIDER */
	BOOL(dqs0_en_hclk);
	BOOL(dqs1_en_hclk);

	/* Mode reigster features */
	BOOL(fast_exit);     /* 0: slow exit; 1: fast exit */
	BOOL(normal_drv);    /* 0: RZQ/6 (reduced driver), 1: RZQ/7 (normal driver). */   
	FLD(ddr2_odt_val, 9); /* 20, 30, 40, 60, and 120 ohm. */
	FLD(rtt_nom_ohm, 9); /* 20, 30, 40, 60, and 120 ohm. */
	FLD(rtt_wr_ohm, 8);  /* 0 (off), 60, and 120 ohm. */

	/* DDZQPCR */
	FLD(cntlr_odt, 10);   /* 24/25/30/40/ 50/ 60/ 75/ 80/100/150/160/200 */
	FLD(cntlr_ocd, 10);  /* 48/50/60/80/100/120/150/160/200/300/320/400 */
	FLD(cntlr_md_odt, 10);	/* 0 is disabled */
	FLD(cntlr_md_ocd, 10);	/* 0 is disabled */
	FLD(cntlr_dm_ocd, 10);	/* 0 is disabled */
	FLD(cntlr_ck_ocd, 10);	/* 0 is disabled */
	FLD(cntlr_dqs_odt, 10);	/* 0 is disabled */
	FLD(cntlr_dqs_ocd, 10);	/* 0 is disabled */
	FLD(cntlr_cmd_ocd, 10);	/* 0 is disabled */
	FLD(cntlr_ma_ocd, 10);	/* 0 is disabled */


	/* Major delay taps */
	FLD(tx_delay,       6);
	FLD(clkm_delay,     6);
	FLD(clkm90_delay,   6);
	FLD(dqs0_en_tap,    6); /* in DIDER. 0 ~ 31 taps. */
	FLD(dqs1_en_tap,    6); /* in DIDER. 0 ~ 31 taps. */
	FLD(dqs0_group_tap, 6); /* in DACCR. 0 ~ 31 taps. */
	FLD(dqs1_group_tap, 6); /* in DACCR. 0 ~ 31 taps. */
	FLD(dqm0_tap,       6); /* in DCDQMR. 0 ~ 31 taps. */
	FLD(dqm1_tap,       6); /* in DCDQMR. 0 ~ 31 taps. */
	ARY(wr_dq_delay,  8, 16);
	ARY(rdr_dq_delay, 8, 16); /* Read delay for rising edges. */
	ARY(rdf_dq_delay, 8, 16); /* Read delay for falling edges. */

	/* Minor delay taps */
	FLD(cke_delay, 6);
	FLD(cs0_delay, 6);
	FLD(cs1_delay, 6);
	FLD(odt_delay, 6);
	FLD(ras_delay, 6);
	FLD(cas_delay, 6);
	FLD(we_delay,  6);

	FLD(ba0_delay,  6);
	FLD(ba1_delay,  6);
	FLD(ba2_delay,  6);
	FLD(dqs0_delay, 7);
	FLD(dqs1_delay, 7);

	ARY(addr_delay, 8, 16);

	/* misc */
	BOOL(write_lvl_en);
	BOOL(dll_dis);
	BOOL(dq_dll_type);  /* 0: Analog; 1: Digital, b800_1500[31] */
	FLD(cal_method, 8);
	BOOL(zq_cal_method);
	BOOL(parallel_bank_en);
	BOOL(dbg_en);
} mc_cntlr_opt_t;

typedef struct {
	FLDR(mcr,  MCR_T);
	FLDR(dcr,  DCR_T);
	FLDR(dtr0, DTR0_T);
	FLDR(dtr1, DTR1_T);
	FLDR(dtr2, DTR2_T);
	FLDR(dider,  DIDER_T);
	FLDR(drcko,  DRCKO_T);
	FLDR(daccr,  DACCR_T);
	FLDR(ddzqpr, DDZQPR_T);
	FLDR(dcdqmr, DCDQMR_T);
	FLDR(ddzqpcr, DDZQPCR_T);

	FLDR(dcsdcr0,  DCSDCR0_T);
	FLDR(dcsdcr1,  DCSDCR1_T);
	FLDR(dcsdcr2,  DCSDCR2_T);
	FLDR(dcsdcr3,  DCSDCR3_T);

	FLDR(ana_dll0, ANA_DLL0_T);

	ARYR(mr, DMCR_T, 4);
	ARYR(dacdq_rr, DACDQ_IDX_RR_T, 16);
	ARYR(dacdq_fr, DACDQ_IDX_FR_T, 16);
} mc_register_set_t;

typedef struct {
	mc_dram_param_t   *dram_param;
	mc_cntlr_opt_t    *cntlr_opt;
	mc_register_set_t *register_set;
} mc_info_t;

extern mc_info_t meminfo;
#endif
