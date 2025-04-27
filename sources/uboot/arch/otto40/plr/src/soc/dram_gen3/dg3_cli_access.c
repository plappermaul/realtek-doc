#include <string.h>
#include <soc.h>
#include <dram/memcntlr.h>
#include <cli/cli_util.h>
#include <cli/cli_access.h>

#ifndef SECTION_CLI_ACCESS
#define SECTION_CLI_ACCESS
#endif

cli_cmd_ret_t cli_ddr_setup(const void *user, u32_t argc, const char *argv[]) {
	mc_info_translation();
	mc_info_to_reg();
	mc_cntlr_zq_calibration();
	mc_calibration();
	mc_result_decode();
	return CCR_OK;
}

#define DRAM_INFO	    ((mc_info_t *)(_soc.dram_info))
#define DRAM_INFO_R     (DRAM_INFO->register_set)
#define DRAM_INFO_O     (DRAM_INFO->cntlr_opt)
#define DRAM_INFO_P     (DRAM_INFO->dram_param)

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

#define DEFINE_DDR_INT_VAR_P_INT(var) 	  DEFINE_DDR_INT_VAR(var, ddr, 1, \
	                     {*result=DRAM_INFO_P->var; }, \
	                     {DRAM_INFO_P->var=value;})

#define DEFINE_DDR_INT_VAR_O_INT(var) 	  DEFINE_DDR_INT_VAR(var, ddr, 1, \
	                     {*result=DRAM_INFO_O->var; }, \
	                     {DRAM_INFO_O->var=value;})

#define DEFINE_DDR_INT_VAR_O_ARY(var,idx) DEFINE_DDR_INT_VAR(var##_##idx, ddr, 1, \
                                                             {*result=DRAM_INFO_O->var[idx]; }, \
                                                             {DRAM_INFO_O->var[idx]=value;})

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

// DRAM Parameters
DEFINE_DDR_INT_VAR_P_INT(dram_type);
DEFINE_DDR_INT_VAR_P_INT(bankcnt);
DEFINE_DDR_INT_VAR_P_INT(buswidth);
DEFINE_DDR_INT_VAR_P_INT(max_row_addr);
DEFINE_DDR_INT_VAR_P_INT(max_col_addr);
DEFINE_DDR_INT_VAR_P_INT(min_trcd_ns);
DEFINE_DDR_INT_VAR_P_INT(min_trp_ns);
DEFINE_DDR_INT_VAR_P_INT(min_tras_ns);
DEFINE_DDR_INT_VAR_P_INT(cl_ck);
DEFINE_DDR_INT_VAR_P_INT(cwl_ck);
DEFINE_DDR_INT_VAR_P_INT(min_twr_ns);
DEFINE_DDR_INT_VAR_P_INT(min_trtp_ns);
DEFINE_DDR_INT_VAR_P_INT(min_twtr_ns);
DEFINE_DDR_INT_VAR_P_INT(min_trrd_ns);
DEFINE_DDR_INT_VAR_P_INT(min_tfaw_ns);
DEFINE_DDR_INT_VAR_P_INT(min_trfc_ns);
DEFINE_DDR_INT_VAR_P_INT(max_trefi_ns);
DEFINE_DDR_INT_VAR_P_INT(min_tck_ps);
DEFINE_DDR_INT_VAR_P_INT(max_tck_ps);
// Control Option
DEFINE_DDR_INT_VAR_O_INT(dqs0_en_hclk);
DEFINE_DDR_INT_VAR_O_INT(dqs1_en_hclk);
DEFINE_DDR_INT_VAR_O_INT(fast_exit);
DEFINE_DDR_INT_VAR_O_INT(normal_drv);
DEFINE_DDR_INT_VAR_O_INT(rtt_nom_ohm);
DEFINE_DDR_INT_VAR_O_INT(rtt_wr_ohm);
DEFINE_DDR_INT_VAR_O_INT(cntlr_odt);
DEFINE_DDR_INT_VAR_O_INT(cntlr_ocd);
DEFINE_DDR_INT_VAR_O_INT(tx_delay);
DEFINE_DDR_INT_VAR_O_INT(clkm_delay);
DEFINE_DDR_INT_VAR_O_INT(clkm90_delay);
DEFINE_DDR_INT_VAR_O_INT(dqs0_en_tap);
DEFINE_DDR_INT_VAR_O_INT(dqs1_en_tap);
DEFINE_DDR_INT_VAR_O_INT(dqs0_group_tap);
DEFINE_DDR_INT_VAR_O_INT(dqs1_group_tap);
DEFINE_DDR_INT_VAR_O_INT(dqm0_tap);
DEFINE_DDR_INT_VAR_O_INT(dqm1_tap);
DEFINE_DDR_INT_VAR_O_ARY(wr_dq_delay,0);
DEFINE_DDR_INT_VAR_O_ARY(wr_dq_delay,1);
DEFINE_DDR_INT_VAR_O_ARY(wr_dq_delay,2);
DEFINE_DDR_INT_VAR_O_ARY(wr_dq_delay,3);
DEFINE_DDR_INT_VAR_O_ARY(wr_dq_delay,4);
DEFINE_DDR_INT_VAR_O_ARY(wr_dq_delay,5);
DEFINE_DDR_INT_VAR_O_ARY(wr_dq_delay,6);
DEFINE_DDR_INT_VAR_O_ARY(wr_dq_delay,7);
DEFINE_DDR_INT_VAR_O_ARY(wr_dq_delay,8);
DEFINE_DDR_INT_VAR_O_ARY(wr_dq_delay,9);
DEFINE_DDR_INT_VAR_O_ARY(wr_dq_delay,10);
DEFINE_DDR_INT_VAR_O_ARY(wr_dq_delay,11);
DEFINE_DDR_INT_VAR_O_ARY(wr_dq_delay,12);
DEFINE_DDR_INT_VAR_O_ARY(wr_dq_delay,13);
DEFINE_DDR_INT_VAR_O_ARY(wr_dq_delay,14);
DEFINE_DDR_INT_VAR_O_ARY(wr_dq_delay,15);
DEFINE_DDR_INT_VAR_O_ARY(rdr_dq_delay,0);
DEFINE_DDR_INT_VAR_O_ARY(rdr_dq_delay,1);
DEFINE_DDR_INT_VAR_O_ARY(rdr_dq_delay,2);
DEFINE_DDR_INT_VAR_O_ARY(rdr_dq_delay,3);
DEFINE_DDR_INT_VAR_O_ARY(rdr_dq_delay,4);
DEFINE_DDR_INT_VAR_O_ARY(rdr_dq_delay,5);
DEFINE_DDR_INT_VAR_O_ARY(rdr_dq_delay,6);
DEFINE_DDR_INT_VAR_O_ARY(rdr_dq_delay,7);
DEFINE_DDR_INT_VAR_O_ARY(rdr_dq_delay,8);
DEFINE_DDR_INT_VAR_O_ARY(rdr_dq_delay,9);
DEFINE_DDR_INT_VAR_O_ARY(rdr_dq_delay,10);
DEFINE_DDR_INT_VAR_O_ARY(rdr_dq_delay,11);
DEFINE_DDR_INT_VAR_O_ARY(rdr_dq_delay,12);
DEFINE_DDR_INT_VAR_O_ARY(rdr_dq_delay,13);
DEFINE_DDR_INT_VAR_O_ARY(rdr_dq_delay,14);
DEFINE_DDR_INT_VAR_O_ARY(rdr_dq_delay,15);
DEFINE_DDR_INT_VAR_O_ARY(rdf_dq_delay,0);
DEFINE_DDR_INT_VAR_O_ARY(rdf_dq_delay,1);
DEFINE_DDR_INT_VAR_O_ARY(rdf_dq_delay,2);
DEFINE_DDR_INT_VAR_O_ARY(rdf_dq_delay,3);
DEFINE_DDR_INT_VAR_O_ARY(rdf_dq_delay,4);
DEFINE_DDR_INT_VAR_O_ARY(rdf_dq_delay,5);
DEFINE_DDR_INT_VAR_O_ARY(rdf_dq_delay,6);
DEFINE_DDR_INT_VAR_O_ARY(rdf_dq_delay,7);
DEFINE_DDR_INT_VAR_O_ARY(rdf_dq_delay,8);
DEFINE_DDR_INT_VAR_O_ARY(rdf_dq_delay,9);
DEFINE_DDR_INT_VAR_O_ARY(rdf_dq_delay,10);
DEFINE_DDR_INT_VAR_O_ARY(rdf_dq_delay,11);
DEFINE_DDR_INT_VAR_O_ARY(rdf_dq_delay,12);
DEFINE_DDR_INT_VAR_O_ARY(rdf_dq_delay,13);
DEFINE_DDR_INT_VAR_O_ARY(rdf_dq_delay,14);
DEFINE_DDR_INT_VAR_O_ARY(rdf_dq_delay,15);
DEFINE_DDR_INT_VAR_O_INT(write_lvl_en);
DEFINE_DDR_INT_VAR_O_INT(dll_dis);
DEFINE_DDR_INT_VAR_O_INT(clk_dll_type);
DEFINE_DDR_INT_VAR_O_INT(dq_dll_type);
DEFINE_DDR_INT_VAR_O_INT(cal_method);
DEFINE_DDR_INT_VAR_O_INT(cke_delay);
DEFINE_DDR_INT_VAR_O_INT(cs0_delay);
DEFINE_DDR_INT_VAR_O_INT(cs1_delay);
DEFINE_DDR_INT_VAR_O_INT(odt_delay);
DEFINE_DDR_INT_VAR_O_INT(ras_delay);
DEFINE_DDR_INT_VAR_O_INT(cas_delay);
DEFINE_DDR_INT_VAR_O_INT(we_delay);
DEFINE_DDR_INT_VAR_O_INT(ba0_delay);
DEFINE_DDR_INT_VAR_O_INT(ba1_delay);
DEFINE_DDR_INT_VAR_O_INT(ba2_delay);
DEFINE_DDR_INT_VAR_O_INT(dqs0_delay);
DEFINE_DDR_INT_VAR_O_INT(dqs1_delay);
DEFINE_DDR_INT_VAR_O_ARY(addr_delay,0);
DEFINE_DDR_INT_VAR_O_ARY(addr_delay,1);
DEFINE_DDR_INT_VAR_O_ARY(addr_delay,2);
DEFINE_DDR_INT_VAR_O_ARY(addr_delay,3);
DEFINE_DDR_INT_VAR_O_ARY(addr_delay,4);
DEFINE_DDR_INT_VAR_O_ARY(addr_delay,5);
DEFINE_DDR_INT_VAR_O_ARY(addr_delay,6);
DEFINE_DDR_INT_VAR_O_ARY(addr_delay,7);
DEFINE_DDR_INT_VAR_O_ARY(addr_delay,8);
DEFINE_DDR_INT_VAR_O_ARY(addr_delay,9);
DEFINE_DDR_INT_VAR_O_ARY(addr_delay,10);
DEFINE_DDR_INT_VAR_O_ARY(addr_delay,11);
DEFINE_DDR_INT_VAR_O_ARY(addr_delay,12);
DEFINE_DDR_INT_VAR_O_ARY(addr_delay,13);
DEFINE_DDR_INT_VAR_O_ARY(addr_delay,14);
DEFINE_DDR_INT_VAR_O_ARY(addr_delay,15);
DEFINE_DDR_INT_VAR_O_INT(bin_phs_sel);
DEFINE_DDR_INT_VAR_O_INT(oride_autok_param);
DEFINE_DDR_INT_VAR_O_INT(rx_win_sel_mul);
DEFINE_DDR_INT_VAR_O_INT(rx_win_sel_div);

// Register Set
DEFINE_DDR_INT_VAR_R_HEX(mcr);
DEFINE_DDR_INT_VAR_R_HEX(dcr);
DEFINE_DDR_INT_VAR_R_HEX(dtr0);
DEFINE_DDR_INT_VAR_R_HEX(dtr1);
DEFINE_DDR_INT_VAR_R_HEX(dtr2);
DEFINE_DDR_INT_VAR_R_HEX(dider);
DEFINE_DDR_INT_VAR_R_HEX(drcko);
DEFINE_DDR_INT_VAR_R_HEX(daccr);
DEFINE_DDR_INT_VAR_R_HEX(ddzqpr);
DEFINE_DDR_INT_VAR_R_HEX(dcdqmr);
DEFINE_DDR_INT_VAR_R_HEX(ddzqpcr);
DEFINE_DDR_INT_VAR_R_HEX(dcsdcr0);
DEFINE_DDR_INT_VAR_R_HEX(dcsdcr1);
DEFINE_DDR_INT_VAR_R_HEX(dcsdcr2);
DEFINE_DDR_INT_VAR_R_HEX(dcsdcr3);
DEFINE_DDR_INT_VAR_R_HEX(ana_dll0);
DEFINE_DDR_INT_VAR_R_ARY(mr,0);
DEFINE_DDR_INT_VAR_R_ARY(mr,1);
DEFINE_DDR_INT_VAR_R_ARY(mr,2);
DEFINE_DDR_INT_VAR_R_ARY(mr,3);
DEFINE_DDR_INT_VAR_R_ARY(dacdq_rr,0);
DEFINE_DDR_INT_VAR_R_ARY(dacdq_rr,1);
DEFINE_DDR_INT_VAR_R_ARY(dacdq_rr,2);
DEFINE_DDR_INT_VAR_R_ARY(dacdq_rr,3);
DEFINE_DDR_INT_VAR_R_ARY(dacdq_rr,4);
DEFINE_DDR_INT_VAR_R_ARY(dacdq_rr,5);
DEFINE_DDR_INT_VAR_R_ARY(dacdq_rr,6);
DEFINE_DDR_INT_VAR_R_ARY(dacdq_rr,7);
DEFINE_DDR_INT_VAR_R_ARY(dacdq_rr,8);
DEFINE_DDR_INT_VAR_R_ARY(dacdq_rr,9);
DEFINE_DDR_INT_VAR_R_ARY(dacdq_rr,10);
DEFINE_DDR_INT_VAR_R_ARY(dacdq_rr,11);
DEFINE_DDR_INT_VAR_R_ARY(dacdq_rr,12);
DEFINE_DDR_INT_VAR_R_ARY(dacdq_rr,13);
DEFINE_DDR_INT_VAR_R_ARY(dacdq_rr,14);
DEFINE_DDR_INT_VAR_R_ARY(dacdq_rr,15);
DEFINE_DDR_INT_VAR_R_ARY(dacdq_fr,0);
DEFINE_DDR_INT_VAR_R_ARY(dacdq_fr,1);
DEFINE_DDR_INT_VAR_R_ARY(dacdq_fr,2);
DEFINE_DDR_INT_VAR_R_ARY(dacdq_fr,3);
DEFINE_DDR_INT_VAR_R_ARY(dacdq_fr,4);
DEFINE_DDR_INT_VAR_R_ARY(dacdq_fr,5);
DEFINE_DDR_INT_VAR_R_ARY(dacdq_fr,6);
DEFINE_DDR_INT_VAR_R_ARY(dacdq_fr,7);
DEFINE_DDR_INT_VAR_R_ARY(dacdq_fr,8);
DEFINE_DDR_INT_VAR_R_ARY(dacdq_fr,9);
DEFINE_DDR_INT_VAR_R_ARY(dacdq_fr,10);
DEFINE_DDR_INT_VAR_R_ARY(dacdq_fr,11);
DEFINE_DDR_INT_VAR_R_ARY(dacdq_fr,12);
DEFINE_DDR_INT_VAR_R_ARY(dacdq_fr,13);
DEFINE_DDR_INT_VAR_R_ARY(dacdq_fr,14);
DEFINE_DDR_INT_VAR_R_ARY(dacdq_fr,15);
