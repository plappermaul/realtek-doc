#ifndef _MEMCNTLR_H_
#define _MEMCNTLR_H_

#include <soc.h>
#include <register_map.h>
#include <dram/memcntlr_util.h>

#define DFI_RATE    (2)
#define DDR3_MR0dv  (0x00001A22)
#define DDR3_MR1dv  (0x00000000)
#define DDR3_MR2dv  (0x00000000)
#define DDR3_MR3dv  (0x00000000)

typedef union {
	struct {
		u32_t mbz_0:19;
		u32_t fast_exit_en:1;
		u32_t wr:3;
		u32_t dll_reset:1;
		u32_t test_mode_en:1;
		u32_t cl_h:3;
		u32_t rd_interleave:1;
		u32_t cl_l:1;
		u32_t bl:2;
	} f;
	u32_t v;
} DDR3_MR0_T;

typedef union {
	struct {
		u32_t mbz_0:19;
		u32_t output_buf_dis:1;
		u32_t tdqs_en:1;
		u32_t mbz_1:1;
		u32_t rtt_nom_h:1;
		u32_t mbz_2:1;
		u32_t write_lvl_en:1;
		u32_t rtt_nom_m:1;
		u32_t odic_h:1;
		u32_t al:2;
		u32_t rtt_nom_l:1;
		u32_t odic_l:1;
		u32_t dll_dis:1;
	} f;
	u32_t v;
} DDR3_MR1_T;

typedef union {
	struct {
		u32_t mbz_0:21;
		u32_t rtt_wr:2;
		u32_t mbz_1:1;
		u32_t ext_temp_range_en:1;
		u32_t auto_self_ref_en:1;
		u32_t cwl:3;
		u32_t partial_ary_self_ref:3;
	} f;
	u32_t v;
} DDR3_MR2_T;

typedef union {
	struct {
		u32_t mbz_0:29;
		u32_t mpr_en:1;	      // 0: Normal, 1: Dataflow from/to MPR
		u32_t mpr_pattern:2;  // only 00b is available
	} f;
	u32_t v;
} DDR3_MR3_T;

/* DDR4 */
#define DDR4_MR0dv  (0x00000A22)
#define DDR4_MR1dv  (0x00000001)
#define DDR4_MR2dv  (0x00000000)
#define DDR4_MR3dv  (0x00000000)
#define DDR4_MR4dv  (0x00000000)
#define DDR4_MR5dv  (0x00000440)
//#define DDR4_MR6dv  (0x000000DE)
#define DDR4_MR6dv  (0x00000090)

typedef union {
	struct {
		u32_t mbz_0:20;
		u32_t wr_rtp:3;
		u32_t dll_reset:1;
		u32_t test_mode_en:1;
		u32_t cl_h:3;
		u32_t bt:1;
		u32_t cl_l:1;
		u32_t bl:2;
	} f;
	u32_t v;
} DDR4_MR0_T;

typedef union {
	struct {
		u32_t mbz_0:19;
		u32_t qoff:1;
		u32_t tdqs_en:1;
		u32_t rtt_nom:3;
		u32_t mbz_2:1;
		u32_t write_lvl_en:1;
		u32_t mbz_3:1;
		u32_t al:2;
		u32_t odi:2;
		u32_t dll:1;
	} f;
	u32_t v;
} DDR4_MR1_T;

typedef union {
	struct {
		u32_t mbz_0:19;
        u32_t write_crc:1;
		u32_t rtt_wr:3;
		u32_t mbz_1:1;
		u32_t lpasr:2;
		u32_t cwl:3;
		u32_t mbz_2:3;
	} f;
	u32_t v;
} DDR4_MR2_T;

typedef union {
	struct {
		u32_t mbz_0:19;
		u32_t mpr_read_format:2;	// 00: serial, 01: parallel, 10: staggered, 11: reserved temperature
		u32_t wr_cmd_lat:2;		// 00: 4 nCK, 01: 5 nCK, 10: 6 nCK
		u32_t fine_granu_ref_mode:3;	// 000: normal, 001: fixed 2x, 010: fixed 4x, 101: on_the_fly 2x, 110: on_the_fly 4x
		u32_t temp_sensor_rdout:1;	// 0: disabled, 1: enabled	(DDR4)
		u32_t per_dram_address:1;	// 0: disabled, 1: enabled      (DDR4)
		u32_t geardown:1;		// 0: 1/2 rate, 1: 1/4 rate	(DDR4)
		u32_t mpr_en:1;			// 0: Normal, 1: Dataflow from/to MPR
		u32_t mpr_pattern:2;		// 00: page0, 01: page1, 10: page2, 11: page3 (01~11: DDR4)
	} f;
	u32_t v;
} DDR4_MR3_T;

typedef union {
	struct {
		u32_t mbz_0:19;
		u32_t wr_preamble:1;		// 0: 1 nCK, 1: 2 nCK
		u32_t rd_preamble:1;		// 0: 1 nCK, 1: 2 nCK
		u32_t rd_prea_train_mode:1;	// 0: disabled, 1: enabled
		u32_t self_ref_abort:1;		// 0: disabled, 1: enabled
		u32_t cs_cmd_lat_mode:3;	// 000: disabled, 001: 3, 010: 4, 011: 5, 100: 6, 101: 8, 110: reserved, 111: reserved
		u32_t mbz_1:1;
		u32_t int_vref_mon:1;		// 0: disabled, 1: enabled
		u32_t temp_ctrl_ref_mode:1;	// 0: disabled, 1: enabled
		u32_t temp_ctrl_ref_range:1;	// 0: normal, 1: extended
		u32_t max_pw_down_mode:1;	// 0: disabled, 1: enabled
		u32_t mbz_2:1;
	} f;
	u32_t v;
} DDR4_MR4_T;	// DDR4

typedef union {
	struct {
		u32_t mbz_0:19;
		u32_t rd_dbi:1;
		u32_t wr_dbi:1;
		u32_t data_mask:1;
		u32_t ca_par_per_err:1;
		u32_t rtt_park:3;
		u32_t odt_in_buf_pdm:1;
		u32_t ca_par_err_st:1;
		u32_t crc_err_clear:1;
		u32_t ca_par_lat_mode:3;
	} f;
	u32_t v;
} DDR4_MR5_T;	// DDR4

typedef union {
	struct {
		u32_t mbz_0:19;
		u32_t tccd_l:3;
		u32_t mbz_1:2;
		u32_t vref_t_en:1;
		u32_t vref_t_range:1;
		u32_t vref_t_val:6;
	} f;
	u32_t v;
} DDR4_MR6_T;	// DDR4

typedef struct {
	/* MISC */
	unsigned int dram_type:3;
	unsigned int bankcnt:5;
	unsigned int pagesize:16;
	unsigned int rowcnt:5;
	unsigned int burst:6;
	/* DRR */
	unsigned int ref_num:4;
	unsigned int min_tref_ns:16;        //tREF,  unit is "ns"
	unsigned int min_trfc_ns:10;        //tRFC,  unit is "ns"
	/* TPR0 */
	unsigned int min_tzqcs_tck:10;      //tZQCS, unit is "tCK"
	unsigned int min_tcke_ns:10;        //tCKE,  unit is "ns"
	unsigned int min_trtp_ns:10;        //tRTP,  unit is "ns"
	unsigned int min_twr_ns:10;         //tWR,   unit is "ns"
	unsigned int min_tras_ns:10;        //tRAS,  unit is "ns"
	unsigned int min_trp_ns:10;         //tRP,   unit is "ns"
	/* TPR1 */
	unsigned int min_tfaw_ns:10;        //tFAW,  unit is "ns"
	unsigned int min_trtw_tck:10;       //tRTW,  unit is "tCK"
	unsigned int min_twtr_ns:10;        //tWTR,  unit is "ns"
	unsigned int min_tccd_tck:10;       //tCCD,  unit is "tCK"
	unsigned int min_trcd_ns:10;        //tRCD,  unit is "ns"
	unsigned int min_trc_ns:10;         //tRC,   unit is "ns"
	unsigned int min_trrd_ns:10;        //tRRD,  unit is "ns"
	/* TPR2 */
	unsigned int min_rst_us:10;         //keep reset low at least times,  unit is "us"
	unsigned int min_cke_us:10;         //wait CKE stable at least times,  unit is "us"
	unsigned int min_tmrd_tck:10;       //tMRD,  unit is "tCK"
	/* TPR3 (for DDR4) */
	unsigned int min_tccd_r_tck:10;     //tCCD_R, unit is "tCK"
	unsigned int min_twtr_s_tck:10;     //tWTR_S, unit is "tCK"
	unsigned int min_tccd_s_tck:10;     //tCCD_S, unit is "tCK"
	unsigned int min_dpin_cmd_lat:10;   //DPIN_CMD_LAT, tCAL?, unit is "tCK"
	/* MR_INFO */
	unsigned int add_lat:4;             //add_lat
	unsigned int rd_lat:4;              //rd_lat
	unsigned int wr_lat:4;              //wr_lat
	/* DCR */
	unsigned int dq32_en:2;             // for HALF_DQ and DQ_32
	unsigned int zqc_en:1;              // for ZQC
	unsigned int gear_down:1;           // for enable geardown function (DDR4)
	unsigned int dbi:1;                 // for enable dbi funciotn (DDR4)
	/* IOCR */
	unsigned int tphy_rdata:8;          // for TPHY_RDATA_EN
	unsigned int tphy_wlat:8;           // for TPHY_WLAT
	unsigned int tphy_wdata:8;          // for TPHY_WDATA
	unsigned int stc_odt_en:1;          // for STC_ODT
	unsigned int stc_cke_en:1;          // for STC_CKE
	/* DRR */
	unsigned int ref_dis:1;
	/* RZQ */
	unsigned int rttnom;
	unsigned int rttwr;
	unsigned int odic;                  // output driving
	/* 2T preamble */
	unsigned int twpre:1;               // 2T WRITE Preamble, 0: 1nCK, 1: 2nCK
	unsigned int trpre:1;               // 2T READ Preamble, 0: 1nCK, 1: 2nCK
} mc_cntlr_opt_t;

typedef struct {
	//unsigned int dpi_long_cali:1;
	unsigned int rzq_ext:1;              // R480 calibration enable
    unsigned int cmd_grp2_en:1;          // 2nd command group enable (ODT1,CKE1,CSN1,CLKB1,CLK1)
	// CWL
	unsigned int data_pre:4;
	unsigned int dqs_rd_str_num:4;       // AFIFO_STR_0
	unsigned int dq_rd_str_num:4;        // AFIFO_STR_0
	unsigned int cmd_ex_rd_str_num:4;    // AFIFO_STR_1
	unsigned int cmd_rd_str_num:4;       // AFIFO_STR_1
	unsigned int rx_rd_str_num:4;        // AFIFO_STR_2
	// PI
	unsigned int cs_post_pi:5;
	unsigned int dck_post_pi:5;
	unsigned int dqs_post_pi_0:5;
	unsigned int dqs_post_pi_1:5;
	unsigned int dqs_post_pi_2:5;
	unsigned int dqs_post_pi_3:5;
	unsigned int dq_post_pi_0:5;
	unsigned int dq_post_pi_1:5;
	unsigned int dq_post_pi_2:5;
	unsigned int dq_post_pi_3:5;
	// RX setting
	unsigned int rd_fifo:5;
	unsigned int dqs_en:5;
	unsigned int rd_odt;
	unsigned int rd_odt_odd:5;
	// DQ setting
	unsigned int rd_dly_pos_dq0:5;
	unsigned int rd_dly_pos_dq1:5;
	unsigned int rd_dly_pos_dq2:5;
	unsigned int rd_dly_pos_dq3:5;
	unsigned int rd_dly_pos_dq4:5;
	unsigned int rd_dly_pos_dq5:5;
	unsigned int rd_dly_pos_dq6:5;
	unsigned int rd_dly_pos_dq7:5;
	unsigned int rd_dly_neg_dq0:5;
	unsigned int rd_dly_neg_dq1:5;
	unsigned int rd_dly_neg_dq2:5;
	unsigned int rd_dly_neg_dq3:5;
	unsigned int rd_dly_neg_dq4:5;
	unsigned int rd_dly_neg_dq5:5;
	unsigned int rd_dly_neg_dq6:5;
	unsigned int rd_dly_neg_dq7:5;
	unsigned int dq0_dly:5;
	unsigned int dq1_dly:5;
	unsigned int dq2_dly:5;
	unsigned int dq3_dly:5;
	unsigned int dq4_dly:5;
	unsigned int dq5_dly:5;
	unsigned int dq6_dly:5;
	unsigned int dq7_dly:5;
	// DM setting
	unsigned int dm0_dly:5;
	unsigned int dm1_dly:5;
	unsigned int dm2_dly:5;
	unsigned int dm3_dly:5;
	// for dq31~dq0 and dqs3~dqs0
	unsigned int ocd_odt[4];
	/* Vref_DQ */
	unsigned int ddr_vref_s;            // DDR side vref DQ select
	unsigned int ddr_vref_r;            // DDR side vref DQ range
	unsigned int dpi_vref_s;            // CTRL side vref DQ select
	unsigned int dpi_vref_r;            // CTRL side vref DQ range
} mc_dpi_opt_t;

typedef struct {
	//CCR_T ccr;
	DCR_T dcr;
	IOCR_T iocr;
	DRR_T drr;
	TPR0_T tpr0;
	TPR1_T tpr1;
	TPR2_T tpr2;
	TPR3_T tpr3;
	MR0_T mr0;
	MR1_T mr1;
	MR2_T mr2;
	MR3_T mr3;
	MR4_T mr4;
	MR5_T mr5;
	MR6_T mr6;
	MR_INFO_T mrinfo;
	MISC_T misc;
	PAD_CTRL_PROG_T pcpr;
} mc_register_set_t;

typedef struct {
	mc_cntlr_opt_t    *cntlr_opt;
	mc_dpi_opt_t	  *dpi_opt;
	mc_register_set_t *register_set;
} mc_info_t;


typedef struct {
	// PI
	int            init_dck_pi;
	int            init_dqs_pi;
	int            init_dq_pi;
	int            init_cs_pi;
	unsigned int   raw_dqss[4][2];  //DQS0~3, 0-31, 32-64
	unsigned int   raw_dqs_pi[2];
	unsigned int   raw_dck_pi[2];
	unsigned int   raw_dq_pi[4][2];
	unsigned int   raw_cs_pi[2];
	int            res_dqss[4];
	int            res_dck_pi;
	int            res_dqs_pi[4];
	int            res_dq_pi[4];
	int            res_cs_pi;
	// rd_str_num
	unsigned int   dqs_rsn;
	unsigned int   dq_rsn;
	unsigned int   cs_rsn;
	unsigned int   cmd_rsn;
	unsigned int   rx_rsn;
	unsigned int   res_dqs_rd_str_num;
	unsigned int   res_dq_rd_str_num;
	unsigned int   res_cmd_ex_rd_str_num;
	unsigned int   res_cmd_rd_str_num;
	unsigned int   res_rx_rd_str_num;
	// RX DQ delay
	unsigned int   raw_dqs_in_pos[9];
	unsigned int   raw_dqs_in_neg[9];
	unsigned int   res_dqs_in_pos[9];
	unsigned int   res_dqs_in_neg[9];
	// DQS_EN
	unsigned int   res_dqs_en;
	// TX DQ delay
	unsigned int   raw_dq_dly[8];
	unsigned int   res_dq_dly[8];
	// DM delay
	unsigned int   raw_dm_dly[4];
	unsigned int   res_dm_dly[4];
	// RD FIFO
	unsigned int   raw_rd_fifo;
	unsigned int   res_rd_fifo;
	// RD ODT
	unsigned int   raw_rd_odt[2];
	unsigned int   res_rd_odt_lo;
	unsigned int   res_rd_odt_hi;
	unsigned int   res_rd_odt_set;
	// VREF DQ
	unsigned int   raw_vref_s_txk[32];
	unsigned short scr_vref_s_txk[32];
	unsigned short res_vref_s_txk;
	unsigned short res_vref_r_txk;
	unsigned int   raw_vref_s_rxk[32];
	unsigned short scr_vref_s_rxk[32];
	unsigned short res_vref_s_rxk;
	unsigned short res_vref_r_rxk;
    // info of DQ32
    short          dq32;
} PI_CALI_T;
#endif
