//#include <string.h>
#include <soc.h>
#include <dram/memcntlr.h>
#include <cli/cli_util.h>
#include <cli/cli_access.h>
#include <osc.h>

#ifndef SECTION_CLI_ACCESS
#define SECTION_CLI_ACCESS
#endif

extern void mem_pll_init(void) UTIL_FAR;
cli_cmd_ret_t
cli_ddr_setup(const void *user, u32_t argc, const char *argv[]) {
	//mem_pll_init();
	mc_info_translation();
	mc_info_to_reg();
	mc_cntlr_zq_calibration();
	mc_calibration();
	mc_misc();
	mc_result_decode();
	return CCR_OK;
}

extern void dpi_calibration(u8_t dbg);
cli_cmd_ret_t
cli_ddr_calibration(const void *user, u32_t argc, const char *argv[]) {
    u32_t dbg=0;
    if(3==argc) {
        dbg = atoi(argv[2]);
    }
    dpi_calibration(dbg);
    return CCR_OK;
}

extern void mc_mode_register_read(void);
cli_cmd_ret_t
cli_mrs_readout(const void *user, u32_t argc, const char *argv[]) {
    mc_mode_register_read();
    return CCR_OK;
}

extern u32_t dpi_vrefdq_calibration(PI_CALI_T *info, u8_t dbg);
cli_cmd_ret_t
cli_vrefdq_calibration(const void *user, u32_t argc, const char *argv[]) {
#if 0
    u32_t dq=0, range=0;
    u32_t pat[2]={0x5A5AA5A5, 0xA5A55A5A};
    
    if(3<=argc) {
        dq = atoi(argv[2]);
    } 
    if(4<=argc) {
        range = atoi(argv[3]);
    }
    if(6<=argc) {
        pat[0] = atoi(argv[4]);
        pat[1] = atoi(argv[5]);
    }
    dpi_vrefdq_calibration(dq, range, pat);
#else
    PI_CALI_T info;
    memset((char *)&info, 0, sizeof(PI_CALI_T));
    dpi_vrefdq_calibration(&info, 0);
#endif
    return CCR_OK;
}

#if 1
cli_cmd_ret_t
cli_rxi310_init(const void *user, u32_t argc, const char *argv[]) {
    mc_rxi310_init(0);
    return CCR_OK;
}

extern void dpi_write_leveling(PI_CALI_T *info, u8_t dbg);
cli_cmd_ret_t
cli_ddr_wl(const void *user, u32_t argc, const char *argv[]) {
    PI_CALI_T info;
    memset((char *)&info, 0, sizeof(PI_CALI_T));
    dpi_write_leveling(&info, 2);
    return CCR_OK;
}

extern u32_t pat_ary[32] __attribute__ ((aligned(128)));
extern void dpi_pi_scan(PI_CALI_T *info, u8_t dbg);
extern int prb7_pattern_gen(u32_t *ary);
cli_cmd_ret_t
cli_ddr_ps(const void *user, u32_t argc, const char *argv[]) {
    PI_CALI_T info;
    memset((char *)&info, 0, sizeof(PI_CALI_T));
    prb7_pattern_gen(pat_ary);
    dpi_pi_scan(&info, 2);
    return CCR_OK;
}

extern u32_t dpi_dq_delay_tap_scan(PI_CALI_T *info, u8_t dbg);
cli_cmd_ret_t
cli_ddr_ddts(const void *user, u32_t argc, const char *argv[]) {
    PI_CALI_T info;
    memset((char *)&info, 0, sizeof(PI_CALI_T));
    prb7_pattern_gen(pat_ary);
    dpi_dq_delay_tap_scan(&info, 2);
    return CCR_OK;
}

extern u32_t dpi_rx_map_scan(PI_CALI_T *info, u8_t dbg);
cli_cmd_ret_t
cli_ddr_drms(const void *user, u32_t argc, const char *argv[]) {
    PI_CALI_T info;
    memset((char *)&info, 0, sizeof(PI_CALI_T));
    prb7_pattern_gen(pat_ary);
    dpi_rx_map_scan(&info, 2);
    return CCR_OK;
}

extern u32_t dpi_dqs_en_scan(PI_CALI_T *info, u8_t dbg);
cli_cmd_ret_t
cli_ddr_des(const void *user, u32_t argc, const char *argv[]) {
    PI_CALI_T info;
    memset((char *)&info, 0, sizeof(PI_CALI_T));
    dpi_dqs_en_scan(&info, 2);
    return CCR_OK;
}

void dram_write_pattern(unsigned int dram_start,int write_loop)
{
	int i,j;
	//unsigned int start_value;
	volatile unsigned int *start;

	const unsigned int mp_dram_patterns[] =
	{
		0x5f702d12,0x4a2b8a41,0x58cd3b26,0x95f9ca8b,
		0xd8a39473,0x9e50f51d,0x70a2f75d,0x799b8763,
		0x8e8c6cae,0xc7841f38,0x27da3f14,0xd14cae5f,
		0x1e213097,0xef5db56c,0xd18f7775,0xbd84b292,
		0xe436de4c,0x8988fea9,0xb5eeda8f,0xe17d1d7d,
		0xc088d667,0xcfb29b50,0x3013f231,0xfa660681,
		0x1568036c,0x779a76cc,0xf2d8967e,0xd3773d61,
		0x0d728dff,0xf086177d,0x4d66b22c,0x8977ae71,
		0x400a5bb1,0x922f7502,0x30730a9f,0x849347ee,
		0x191763a1,0xee886c17,0x6e933d90,0x3ebfb10a,
		0xe41add67,0x5810fd1e,0x0baabb9a,0x4544a5e0,
		0xbe0b0670,0x3284e2e8,0xc477d1dd,0xb6ef4343,
		0x8f27bcbe,0x2e7c3096,0xdba82956,0x88d47e5d,
		0xa91e9f80,0x43ed1c43,0x1cbdf31c,0x3ea1d07e,
		0x047d7a9c,0x34b8460b,0xe2f0c34e,0x2e66e4ae,
		0x0251b4a2,0x2c7d1dc0,0x15665d86,0x6602681d,
		0x6ecb2978,0x2b3be3b9,0xf8a25b31,0x7fe98177,
		0xd3d95dcc,0x147cbde9,0xc77e2d8c,0xd04847c6,
		0x145e4893,0x8c83f48a,0x31e6bd83,0xe8243da5,
		0xfab1efec,0x12d06bad,0x1676233b,0x812d59a4,
		0xec25a615,0x67d700a2,0xa42bf834,0x0f84f5d9,
		0xe4d75377,0x18023256,0x8c08e616,0x3bbd2fa3,
		0x0c0ed720,0xe7651af4,0xc65f8db7,0x2de326c5,
		0xc8673112,0x8403c70b,0x64eca900,0xd845a4c9,
		0xb332ff6e,0xa0ca14ac,0x9cae2fc1,0x0b7d6356,
		0x38337533,0xbc1f97c8,0x5c2da452,0xc40885a4,
		0x84eb2547,0xb0a3dd4f,0x00df7044,0x066a9033,
		0xc0f00747,0x09efbbcc,0x0bda3343,0xe956eb2d,
		0xdaa575e4,0x9d9755da,0x45277c7a,0xfd16abfe,
		0x009ea460,0x9e404bc9,0x2085c22f,0x3e7718f1,
		0x3c039132,0x96aff4a0,0x38c74b69,0x055f0e12,
		0x0950a632,0x408a14da,0xa57f4f61,0x241a27b7
	};

	dcache_wr_inv_all();

	for (i=0;i<write_loop;i++){

	/* write pattern*/
		start = (unsigned int *)(dram_start);
		for (j=0; j<(32768/4);j++) {	// cache size is 32Kbyte(32768 byte)
			*start = mp_dram_patterns[j % (sizeof(mp_dram_patterns) / sizeof(unsigned int))];	//repeat "mp_dram_patterns" and fill scan_size
			start++;
		}
		dcache_wr_inv_all();
	}
    puts("done\n");
	return;
}

cli_cmd_ret_t
cli_dram_write_pattern(const void *user, u32_t argc, const char *argv[]) {
    if(argc<3) return CCR_INCOMPLETE_CMD;
    u32_t l, a=atoi(argv[2]);
    if(4==argc) { l=atoi(argv[3]); }
    else l=1;

    dram_write_pattern(a, l);
    return CCR_OK;
}

cli_add_node(dk_wrl, call, cli_ddr_wl);
cli_add_help(dk_wrl, "call dk_wrl: write leveling");
cli_add_node(dk_ps, call, cli_ddr_ps);
cli_add_help(dk_ps, "call dk_ps: PI scan");
cli_add_node(dk_ddts, call, cli_ddr_ddts);
cli_add_help(dk_ddts, "call dk_ddts: dq delay tap scan");
cli_add_node(dk_drms, call, cli_ddr_drms);
cli_add_help(dk_drms, "call dk_drms: rx map scan");
cli_add_node(dk_des, call, cli_ddr_des);
cli_add_help(dk_des, "call dk_des: dqs en scan");
cli_add_node(rxi310_init, call, cli_rxi310_init);
cli_add_help(rxi310_init, "call rxi310_init: RXI310 init");
cli_add_node(dram_write_pattern, call, cli_dram_write_pattern);
cli_add_help(dram_write_pattern, "call dram_write_pattern: dram_write_pattern");
#endif
cli_add_node(mrs_readout, call, cli_mrs_readout);
cli_add_help(mrs_readout, "call mrs_readout");
cli_add_node(vrefdq_k, call, cli_vrefdq_calibration);
cli_add_help(vrefdq_k, "call vrefdq_k");
cli_add_node(dram_calibration, call, cli_ddr_calibration);
cli_add_help(dram_calibration, "call dram_calibration [debug on}: full calibration");

#define DRAM_INFO	    ((mc_info_t *)(_soc.dram_info))
#define DRAM_INFO_R     (DRAM_INFO->register_set)
#define DRAM_INFO_C     (DRAM_INFO->cntlr_opt)
#define DRAM_INFO_D     (DRAM_INFO->dpi_opt)

#define DEFINE_DDR_INT_VAR(name, family, is_dec, get_func_body, set_func_body) \
	SECTION_CLI_VAR int _CLI_VAR_DDR_ ## name ## _get_int_(u32_t *result) {get_func_body; return 0;} \
	SECTION_CLI_VAR int _CLI_VAR_DDR_ ## name ## _set_int_(u32_t value) {set_func_body; return 0;} \
	CLI_DEFINE_VAR(name, family, 1, 0, is_dec, \
	               _CLI_VAR_DDR_ ## name ## _get_int_, \
	               _CLI_VAR_DDR_ ## name ## _set_int_)

//#define DEFINE_DDR_STR_VAR(name, get_func_body, set_func_body)
#define DEFINE_DDR_STR_VAR(name, family, get_func_body, set_func_body)  \
	SECTION_CLI_VAR int _CLI_VAR_DDR_ ## name ## _get_str_(char *result) {get_func_body; return 0;} \
	SECTION_CLI_VAR int _CLI_VAR_DDR_ ## name ## _set_str_(const char *value) {set_func_body; return 0;} \
	CLI_DEFINE_VAR(name, family, 1, 0, 0, \
	               _CLI_VAR_DDR_ ## name ## _get_str_, \
	               _CLI_VAR_DDR_ ## name ## _set_str_)

#define DEFINE_DDR_INT_VAR_C_INT(var) 	  DEFINE_DDR_INT_VAR(var, ddr, 1, \
	                     {*result=DRAM_INFO_C->var; }, \
	                     {DRAM_INFO_C->var=value;})

#define DEFINE_DDR_INT_VAR_D_INT(var) 	  DEFINE_DDR_INT_VAR(var, ddr, 1, \
	                     {*result=DRAM_INFO_D->var; }, \
	                     {DRAM_INFO_D->var=value;})

#define DEFINE_DDR_INT_VAR_D_ARY(var,idx) DEFINE_DDR_INT_VAR(var##_##idx, ddr, 1, \
                                                             {*result=DRAM_INFO_D->var[idx]; }, \
                                                             {DRAM_INFO_D->var[idx]=value;})

#define DEFINE_DDR_INT_VAR_R_HEX(var)	  DEFINE_DDR_INT_VAR(var, ddr_reg, 0, \
	                     {*result=DRAM_INFO_R->var.v; }, \
	                     {DRAM_INFO_R->var.v=value;})

#define DEFINE_DDR_INT_VAR_R_ARY(var,idx) DEFINE_DDR_INT_VAR(var##_##idx, ddr_reg, 0, \
                                                             {*result=DRAM_INFO_R->var[idx].v; }, \
                                                             {DRAM_INFO_R->var[idx].v=value;})

cli_add_node(ddr, get, VZERO);
cli_add_parent(ddr, set);
cli_add_node(ddr_reg, get, VZERO);
cli_add_parent(ddr_reg, set);

// RXI-310
DEFINE_DDR_INT_VAR_C_INT(dram_type);
DEFINE_DDR_INT_VAR_C_INT(bankcnt);
DEFINE_DDR_INT_VAR_C_INT(pagesize);
DEFINE_DDR_INT_VAR_C_INT(ref_num);
DEFINE_DDR_INT_VAR_C_INT(min_tref_ns);
DEFINE_DDR_INT_VAR_C_INT(min_trfc_ns);
DEFINE_DDR_INT_VAR_C_INT(min_tzqcs_tck);
DEFINE_DDR_INT_VAR_C_INT(min_tcke_ns);
DEFINE_DDR_INT_VAR_C_INT(min_trtp_ns);
DEFINE_DDR_INT_VAR_C_INT(min_twr_ns);
DEFINE_DDR_INT_VAR_C_INT(min_tras_ns);
DEFINE_DDR_INT_VAR_C_INT(min_trp_ns);
DEFINE_DDR_INT_VAR_C_INT(min_tfaw_ns);
DEFINE_DDR_INT_VAR_C_INT(min_trtw_tck);
DEFINE_DDR_INT_VAR_C_INT(min_twtr_ns);
DEFINE_DDR_INT_VAR_C_INT(min_tccd_tck);
DEFINE_DDR_INT_VAR_C_INT(min_trcd_ns);
DEFINE_DDR_INT_VAR_C_INT(min_trc_ns);
DEFINE_DDR_INT_VAR_C_INT(min_trrd_ns);
DEFINE_DDR_INT_VAR_C_INT(min_tmrd_tck);
DEFINE_DDR_INT_VAR_C_INT(min_tccd_r_tck);
DEFINE_DDR_INT_VAR_C_INT(min_twtr_s_tck);
DEFINE_DDR_INT_VAR_C_INT(min_tccd_s_tck);
DEFINE_DDR_INT_VAR_C_INT(min_dpin_cmd_lat);
DEFINE_DDR_INT_VAR_C_INT(add_lat);
DEFINE_DDR_INT_VAR_C_INT(rd_lat);
DEFINE_DDR_INT_VAR_C_INT(wr_lat);

DEFINE_DDR_INT_VAR_C_INT(dq32_en);
DEFINE_DDR_INT_VAR_C_INT(zqc_en);
DEFINE_DDR_INT_VAR_C_INT(tphy_rdata);
DEFINE_DDR_INT_VAR_C_INT(tphy_wlat);
DEFINE_DDR_INT_VAR_C_INT(tphy_wdata);
DEFINE_DDR_INT_VAR_C_INT(stc_odt_en);
DEFINE_DDR_INT_VAR_C_INT(stc_cke_en);
DEFINE_DDR_INT_VAR_C_INT(ref_dis);
DEFINE_DDR_INT_VAR_C_INT(rttnom);
DEFINE_DDR_INT_VAR_C_INT(rttwr);
DEFINE_DDR_INT_VAR_C_INT(odic);
DEFINE_DDR_INT_VAR_C_INT(twpre);
DEFINE_DDR_INT_VAR_C_INT(trpre);

DEFINE_DDR_INT_VAR_D_INT(rzq_ext);
DEFINE_DDR_INT_VAR_D_INT(data_pre);
DEFINE_DDR_INT_VAR_D_INT(dqs_rd_str_num);
DEFINE_DDR_INT_VAR_D_INT(dq_rd_str_num);
DEFINE_DDR_INT_VAR_D_INT(cmd_ex_rd_str_num);
DEFINE_DDR_INT_VAR_D_INT(cmd_rd_str_num);
DEFINE_DDR_INT_VAR_D_INT(rx_rd_str_num);

DEFINE_DDR_INT_VAR_D_INT(cs_post_pi);
DEFINE_DDR_INT_VAR_D_INT(dck_post_pi);
DEFINE_DDR_INT_VAR_D_INT(dqs_post_pi);
DEFINE_DDR_INT_VAR_D_INT(dq_post_pi);
DEFINE_DDR_INT_VAR_D_INT(rd_fifo);
DEFINE_DDR_INT_VAR_D_INT(dqs_en);
DEFINE_DDR_INT_VAR_D_INT(rd_odt);
DEFINE_DDR_INT_VAR_D_INT(rd_odt_odd);
DEFINE_DDR_INT_VAR_D_INT(ddr_vref_s);
DEFINE_DDR_INT_VAR_D_INT(ddr_vref_r);
DEFINE_DDR_INT_VAR_D_INT(dpi_vref_s);
DEFINE_DDR_INT_VAR_D_INT(dpi_vref_r);

// Register Set
//DEFINE_DDR_INT_VAR_R_HEX(ccr);
DEFINE_DDR_INT_VAR_R_HEX(dcr);
DEFINE_DDR_INT_VAR_R_HEX(iocr);
DEFINE_DDR_INT_VAR_R_HEX(drr);
DEFINE_DDR_INT_VAR_R_HEX(tpr0);
DEFINE_DDR_INT_VAR_R_HEX(tpr1);
DEFINE_DDR_INT_VAR_R_HEX(tpr2);
DEFINE_DDR_INT_VAR_R_HEX(tpr3);
DEFINE_DDR_INT_VAR_R_HEX(mr0);
DEFINE_DDR_INT_VAR_R_HEX(mr1);
DEFINE_DDR_INT_VAR_R_HEX(mr2);
DEFINE_DDR_INT_VAR_R_HEX(mr3);
DEFINE_DDR_INT_VAR_R_HEX(mrinfo);
DEFINE_DDR_INT_VAR_R_HEX(misc);
