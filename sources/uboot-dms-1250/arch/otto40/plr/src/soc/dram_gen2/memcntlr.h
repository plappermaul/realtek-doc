#ifndef _MEMCNTLR_H_
#define _MEMCNTLR_H_

#include <soc.h>
#include <register_map.h>
#include <memcntlr_util.h>

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
	FLD(min_trcd_ns,  8);
	FLD(min_trp_ns,   8);
	FLD(min_tras_ns,  8);
	FLD(cl_ck,        8);
	FLD(cwl_ck,       8);
	FLD(min_twr_ns,   8);
	FLD(min_trtp_ns,  8);
	FLD(min_twtr_ns,  8);
	FLD(min_trrd_ns,  8);
	FLD(min_tfaw_ns,  8);
	FLD(min_trfc_ns,  16);
	FLD(max_trefi_ns, 16);
	FLD(min_tck_ps,   16);
	FLD(max_tck_ps,   16);
} mc_dram_param_t;

typedef struct {
	/* DIDER */
	BOOL(dqs0_en_hclk);
	BOOL(dqs1_en_hclk);

	/* Mode reigster features */
	BOOL(fast_exit);     /* 0: slow exit; 1: fast exit */
	BOOL(normal_drv);    /* 0: RZQ/6 (reduced driver), 1: RZQ/7 (normal driver). */
	FLD(rtt_nom_ohm, 9); /* 20, 30, 40, 60, and 120 ohm. */
	FLD(rtt_wr_ohm, 8);  /* 0 (off), 60, and 120 ohm. */

	/* DDZQPCR */
	FLD(cntlr_odt, 9);   /* 24/25/30/40/ 50/ 60/ 75/ 80/100/150/160/200 */
	FLD(cntlr_ocd, 10);  /* 48/50/60/80/100/120/150/160/200/300/320/400 */

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

	/* misc */
	BOOL(write_lvl_en);
	BOOL(dll_dis);
	FLD(cal_method, 8);
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
	ARYR(mr, DMCR_T, 4);
	ARYR(dacdq_rr, DACDQ0_IDX_RR_T, 16);
	ARYR(dacdq_fr, DACDQ0_IDX_FR_T, 16);
} mc_register_set_t;

typedef struct {
	mc_dram_param_t   *dram_param;
	mc_cntlr_opt_t    *cntlr_opt;
	mc_register_set_t *register_set;
} mc_info_t;

#endif
