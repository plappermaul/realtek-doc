MEMCNTLR_SECTION void __attribute__ ((weak)) mc_dump_dram_type(mc_dram_param_t *str) {
	printf("  dram_type: %d\n", str->dram_type);
	return;
}
MEMCNTLR_SECTION static inline void mc_xlat_n_assign_dram_type(mc_dram_param_t * __attribute__ ((unused)), mc_cntlr_opt_t * __attribute__ ((unused)), mc_register_set_t *);
MEMCNTLR_SECTION void __attribute__ ((weak)) mc_dump_bankcnt(mc_dram_param_t *str) {
	printf("  bankcnt: %d\n", str->bankcnt);
	return;
}
MEMCNTLR_SECTION static inline void mc_xlat_n_assign_bankcnt(mc_dram_param_t * __attribute__ ((unused)), mc_cntlr_opt_t * __attribute__ ((unused)), mc_register_set_t *);
MEMCNTLR_SECTION void __attribute__ ((weak)) mc_dump_buswidth(mc_dram_param_t *str) {
	printf("  buswidth: %d\n", str->buswidth);
	return;
}
MEMCNTLR_SECTION static inline void mc_xlat_n_assign_buswidth(mc_dram_param_t * __attribute__ ((unused)), mc_cntlr_opt_t * __attribute__ ((unused)), mc_register_set_t *);
MEMCNTLR_SECTION void __attribute__ ((weak)) mc_dump_max_row_addr(mc_dram_param_t *str) {
	printf("  max_row_addr: %d\n", str->max_row_addr);
	return;
}
MEMCNTLR_SECTION static inline void mc_xlat_n_assign_max_row_addr(mc_dram_param_t * __attribute__ ((unused)), mc_cntlr_opt_t * __attribute__ ((unused)), mc_register_set_t *);
MEMCNTLR_SECTION void __attribute__ ((weak)) mc_dump_max_col_addr(mc_dram_param_t *str) {
	printf("  max_col_addr: %d\n", str->max_col_addr);
	return;
}
MEMCNTLR_SECTION static inline void mc_xlat_n_assign_max_col_addr(mc_dram_param_t * __attribute__ ((unused)), mc_cntlr_opt_t * __attribute__ ((unused)), mc_register_set_t *);
MEMCNTLR_SECTION void __attribute__ ((weak)) mc_dump_min_trcd_ns(mc_dram_param_t *str) {
	printf("  min_trcd_ns: %d\n", str->min_trcd_ns);
	return;
}
MEMCNTLR_SECTION static inline void mc_xlat_n_assign_min_trcd_ns(mc_dram_param_t * __attribute__ ((unused)), mc_cntlr_opt_t * __attribute__ ((unused)), mc_register_set_t *);
MEMCNTLR_SECTION void __attribute__ ((weak)) mc_dump_min_trp_ns(mc_dram_param_t *str) {
	printf("  min_trp_ns: %d\n", str->min_trp_ns);
	return;
}
MEMCNTLR_SECTION static inline void mc_xlat_n_assign_min_trp_ns(mc_dram_param_t * __attribute__ ((unused)), mc_cntlr_opt_t * __attribute__ ((unused)), mc_register_set_t *);
MEMCNTLR_SECTION void __attribute__ ((weak)) mc_dump_min_tras_ns(mc_dram_param_t *str) {
	printf("  min_tras_ns: %d\n", str->min_tras_ns);
	return;
}
MEMCNTLR_SECTION static inline void mc_xlat_n_assign_min_tras_ns(mc_dram_param_t * __attribute__ ((unused)), mc_cntlr_opt_t * __attribute__ ((unused)), mc_register_set_t *);
MEMCNTLR_SECTION void __attribute__ ((weak)) mc_dump_cl_ck(mc_dram_param_t *str) {
	printf("  cl_ck: %d\n", str->cl_ck);
	return;
}
MEMCNTLR_SECTION static inline void mc_xlat_n_assign_cl_ck(mc_dram_param_t * __attribute__ ((unused)), mc_cntlr_opt_t * __attribute__ ((unused)), mc_register_set_t *);
MEMCNTLR_SECTION void __attribute__ ((weak)) mc_dump_cwl_ck(mc_dram_param_t *str) {
	printf("  cwl_ck: %d\n", str->cwl_ck);
	return;
}
MEMCNTLR_SECTION static inline void mc_xlat_n_assign_cwl_ck(mc_dram_param_t * __attribute__ ((unused)), mc_cntlr_opt_t * __attribute__ ((unused)), mc_register_set_t *);
MEMCNTLR_SECTION void __attribute__ ((weak)) mc_dump_min_twr_ns(mc_dram_param_t *str) {
	printf("  min_twr_ns: %d\n", str->min_twr_ns);
	return;
}
MEMCNTLR_SECTION static inline void mc_xlat_n_assign_min_twr_ns(mc_dram_param_t * __attribute__ ((unused)), mc_cntlr_opt_t * __attribute__ ((unused)), mc_register_set_t *);
MEMCNTLR_SECTION void __attribute__ ((weak)) mc_dump_min_trtp_ns(mc_dram_param_t *str) {
	printf("  min_trtp_ns: %d\n", str->min_trtp_ns);
	return;
}
MEMCNTLR_SECTION static inline void mc_xlat_n_assign_min_trtp_ns(mc_dram_param_t * __attribute__ ((unused)), mc_cntlr_opt_t * __attribute__ ((unused)), mc_register_set_t *);
MEMCNTLR_SECTION void __attribute__ ((weak)) mc_dump_min_twtr_ns(mc_dram_param_t *str) {
	printf("  min_twtr_ns: %d\n", str->min_twtr_ns);
	return;
}
MEMCNTLR_SECTION static inline void mc_xlat_n_assign_min_twtr_ns(mc_dram_param_t * __attribute__ ((unused)), mc_cntlr_opt_t * __attribute__ ((unused)), mc_register_set_t *);
MEMCNTLR_SECTION void __attribute__ ((weak)) mc_dump_min_trrd_ns(mc_dram_param_t *str) {
	printf("  min_trrd_ns: %d\n", str->min_trrd_ns);
	return;
}
MEMCNTLR_SECTION static inline void mc_xlat_n_assign_min_trrd_ns(mc_dram_param_t * __attribute__ ((unused)), mc_cntlr_opt_t * __attribute__ ((unused)), mc_register_set_t *);
MEMCNTLR_SECTION void __attribute__ ((weak)) mc_dump_min_tfaw_ns(mc_dram_param_t *str) {
	printf("  min_tfaw_ns: %d\n", str->min_tfaw_ns);
	return;
}
MEMCNTLR_SECTION static inline void mc_xlat_n_assign_min_tfaw_ns(mc_dram_param_t * __attribute__ ((unused)), mc_cntlr_opt_t * __attribute__ ((unused)), mc_register_set_t *);
MEMCNTLR_SECTION void __attribute__ ((weak)) mc_dump_min_trfc_ns(mc_dram_param_t *str) {
	printf("  min_trfc_ns: %d\n", str->min_trfc_ns);
	return;
}
MEMCNTLR_SECTION static inline void mc_xlat_n_assign_min_trfc_ns(mc_dram_param_t * __attribute__ ((unused)), mc_cntlr_opt_t * __attribute__ ((unused)), mc_register_set_t *);
MEMCNTLR_SECTION void __attribute__ ((weak)) mc_dump_max_trefi_ns(mc_dram_param_t *str) {
	printf("  max_trefi_ns: %d\n", str->max_trefi_ns);
	return;
}
MEMCNTLR_SECTION static inline void mc_xlat_n_assign_max_trefi_ns(mc_dram_param_t * __attribute__ ((unused)), mc_cntlr_opt_t * __attribute__ ((unused)), mc_register_set_t *);
MEMCNTLR_SECTION void __attribute__ ((weak)) mc_dump_min_tck_ps(mc_dram_param_t *str) {
	printf("  min_tck_ps: %d\n", str->min_tck_ps);
	return;
}
MEMCNTLR_SECTION static inline void mc_xlat_n_assign_min_tck_ps(mc_dram_param_t * __attribute__ ((unused)), mc_cntlr_opt_t * __attribute__ ((unused)), mc_register_set_t *);
MEMCNTLR_SECTION void __attribute__ ((weak)) mc_dump_max_tck_ps(mc_dram_param_t *str) {
	printf("  max_tck_ps: %d\n", str->max_tck_ps);
	return;
}
MEMCNTLR_SECTION static inline void mc_xlat_n_assign_max_tck_ps(mc_dram_param_t * __attribute__ ((unused)), mc_cntlr_opt_t * __attribute__ ((unused)), mc_register_set_t *);
MEMCNTLR_SECTION static inline void mc_dram_param_dump(void) {
	mc_dram_param_t *str = meminfo.dram_param;
	printf("II: meminfo.mc_dram_param @ %p (%dB)\n", str, sizeof(mc_dram_param_t));
	mc_dump_dram_type(str);
	mc_dump_bankcnt(str);
	mc_dump_buswidth(str);
	mc_dump_max_row_addr(str);
	mc_dump_max_col_addr(str);
	mc_dump_min_trcd_ns(str);
	mc_dump_min_trp_ns(str);
	mc_dump_min_tras_ns(str);
	mc_dump_cl_ck(str);
	mc_dump_cwl_ck(str);
	mc_dump_min_twr_ns(str);
	mc_dump_min_trtp_ns(str);
	mc_dump_min_twtr_ns(str);
	mc_dump_min_trrd_ns(str);
	mc_dump_min_tfaw_ns(str);
	mc_dump_min_trfc_ns(str);
	mc_dump_max_trefi_ns(str);
	mc_dump_min_tck_ps(str);
	mc_dump_max_tck_ps(str);
}
MEMCNTLR_SECTION void __attribute__ ((weak)) mc_dump_dqs0_en_hclk(mc_cntlr_opt_t *str) {
	printf("  dqs0_en_hclk: %d\n", str->dqs0_en_hclk);
	return;
}
MEMCNTLR_SECTION static inline void mc_xlat_n_assign_dqs0_en_hclk(mc_dram_param_t * __attribute__ ((unused)), mc_cntlr_opt_t * __attribute__ ((unused)), mc_register_set_t *);
MEMCNTLR_SECTION void __attribute__ ((weak)) mc_dump_dqs1_en_hclk(mc_cntlr_opt_t *str) {
	printf("  dqs1_en_hclk: %d\n", str->dqs1_en_hclk);
	return;
}
MEMCNTLR_SECTION static inline void mc_xlat_n_assign_dqs1_en_hclk(mc_dram_param_t * __attribute__ ((unused)), mc_cntlr_opt_t * __attribute__ ((unused)), mc_register_set_t *);
MEMCNTLR_SECTION void __attribute__ ((weak)) mc_dump_fast_exit(mc_cntlr_opt_t *str) {
	printf("  fast_exit: %d\n", str->fast_exit);
	return;
}
MEMCNTLR_SECTION static inline void mc_xlat_n_assign_fast_exit(mc_dram_param_t * __attribute__ ((unused)), mc_cntlr_opt_t * __attribute__ ((unused)), mc_register_set_t *);
MEMCNTLR_SECTION void __attribute__ ((weak)) mc_dump_normal_drv(mc_cntlr_opt_t *str) {
	printf("  normal_drv: %d\n", str->normal_drv);
	return;
}
MEMCNTLR_SECTION static inline void mc_xlat_n_assign_normal_drv(mc_dram_param_t * __attribute__ ((unused)), mc_cntlr_opt_t * __attribute__ ((unused)), mc_register_set_t *);
MEMCNTLR_SECTION void __attribute__ ((weak)) mc_dump_rtt_nom_ohm(mc_cntlr_opt_t *str) {
	printf("  rtt_nom_ohm: %d\n", str->rtt_nom_ohm);
	return;
}
MEMCNTLR_SECTION static inline void mc_xlat_n_assign_rtt_nom_ohm(mc_dram_param_t * __attribute__ ((unused)), mc_cntlr_opt_t * __attribute__ ((unused)), mc_register_set_t *);
MEMCNTLR_SECTION void __attribute__ ((weak)) mc_dump_rtt_wr_ohm(mc_cntlr_opt_t *str) {
	printf("  rtt_wr_ohm: %d\n", str->rtt_wr_ohm);
	return;
}
MEMCNTLR_SECTION static inline void mc_xlat_n_assign_rtt_wr_ohm(mc_dram_param_t * __attribute__ ((unused)), mc_cntlr_opt_t * __attribute__ ((unused)), mc_register_set_t *);
MEMCNTLR_SECTION void __attribute__ ((weak)) mc_dump_cntlr_odt(mc_cntlr_opt_t *str) {
	printf("  cntlr_odt: %d\n", str->cntlr_odt);
	return;
}
MEMCNTLR_SECTION static inline void mc_xlat_n_assign_cntlr_odt(mc_dram_param_t * __attribute__ ((unused)), mc_cntlr_opt_t * __attribute__ ((unused)), mc_register_set_t *);
MEMCNTLR_SECTION void __attribute__ ((weak)) mc_dump_cntlr_ocd(mc_cntlr_opt_t *str) {
	printf("  cntlr_ocd: %d\n", str->cntlr_ocd);
	return;
}
MEMCNTLR_SECTION static inline void mc_xlat_n_assign_cntlr_ocd(mc_dram_param_t * __attribute__ ((unused)), mc_cntlr_opt_t * __attribute__ ((unused)), mc_register_set_t *);
MEMCNTLR_SECTION void __attribute__ ((weak)) mc_dump_tx_delay(mc_cntlr_opt_t *str) {
	printf("  tx_delay: %d\n", str->tx_delay);
	return;
}
MEMCNTLR_SECTION static inline void mc_xlat_n_assign_tx_delay(mc_dram_param_t * __attribute__ ((unused)), mc_cntlr_opt_t * __attribute__ ((unused)), mc_register_set_t *);
MEMCNTLR_SECTION void __attribute__ ((weak)) mc_dump_clkm_delay(mc_cntlr_opt_t *str) {
	printf("  clkm_delay: %d\n", str->clkm_delay);
	return;
}
MEMCNTLR_SECTION static inline void mc_xlat_n_assign_clkm_delay(mc_dram_param_t * __attribute__ ((unused)), mc_cntlr_opt_t * __attribute__ ((unused)), mc_register_set_t *);
MEMCNTLR_SECTION void __attribute__ ((weak)) mc_dump_clkm90_delay(mc_cntlr_opt_t *str) {
	printf("  clkm90_delay: %d\n", str->clkm90_delay);
	return;
}
MEMCNTLR_SECTION static inline void mc_xlat_n_assign_clkm90_delay(mc_dram_param_t * __attribute__ ((unused)), mc_cntlr_opt_t * __attribute__ ((unused)), mc_register_set_t *);
MEMCNTLR_SECTION void __attribute__ ((weak)) mc_dump_dqs0_en_tap(mc_cntlr_opt_t *str) {
	printf("  dqs0_en_tap: %d\n", str->dqs0_en_tap);
	return;
}
MEMCNTLR_SECTION static inline void mc_xlat_n_assign_dqs0_en_tap(mc_dram_param_t * __attribute__ ((unused)), mc_cntlr_opt_t * __attribute__ ((unused)), mc_register_set_t *);
MEMCNTLR_SECTION void __attribute__ ((weak)) mc_dump_dqs1_en_tap(mc_cntlr_opt_t *str) {
	printf("  dqs1_en_tap: %d\n", str->dqs1_en_tap);
	return;
}
MEMCNTLR_SECTION static inline void mc_xlat_n_assign_dqs1_en_tap(mc_dram_param_t * __attribute__ ((unused)), mc_cntlr_opt_t * __attribute__ ((unused)), mc_register_set_t *);
MEMCNTLR_SECTION void __attribute__ ((weak)) mc_dump_dqs0_group_tap(mc_cntlr_opt_t *str) {
	printf("  dqs0_group_tap: %d\n", str->dqs0_group_tap);
	return;
}
MEMCNTLR_SECTION static inline void mc_xlat_n_assign_dqs0_group_tap(mc_dram_param_t * __attribute__ ((unused)), mc_cntlr_opt_t * __attribute__ ((unused)), mc_register_set_t *);
MEMCNTLR_SECTION void __attribute__ ((weak)) mc_dump_dqs1_group_tap(mc_cntlr_opt_t *str) {
	printf("  dqs1_group_tap: %d\n", str->dqs1_group_tap);
	return;
}
MEMCNTLR_SECTION static inline void mc_xlat_n_assign_dqs1_group_tap(mc_dram_param_t * __attribute__ ((unused)), mc_cntlr_opt_t * __attribute__ ((unused)), mc_register_set_t *);
MEMCNTLR_SECTION void __attribute__ ((weak)) mc_dump_dqm0_tap(mc_cntlr_opt_t *str) {
	printf("  dqm0_tap: %d\n", str->dqm0_tap);
	return;
}
MEMCNTLR_SECTION static inline void mc_xlat_n_assign_dqm0_tap(mc_dram_param_t * __attribute__ ((unused)), mc_cntlr_opt_t * __attribute__ ((unused)), mc_register_set_t *);
MEMCNTLR_SECTION void __attribute__ ((weak)) mc_dump_dqm1_tap(mc_cntlr_opt_t *str) {
	printf("  dqm1_tap: %d\n", str->dqm1_tap);
	return;
}
MEMCNTLR_SECTION static inline void mc_xlat_n_assign_dqm1_tap(mc_dram_param_t * __attribute__ ((unused)), mc_cntlr_opt_t * __attribute__ ((unused)), mc_register_set_t *);
MEMCNTLR_SECTION void __attribute__ ((weak)) mc_dump_wr_dq_delay(mc_cntlr_opt_t *str) {
	u32_t i;
	for (i=0; i<16; i+=4) {
		printf("  wr_dq_delay[%02d]: %d, %d, %d, %d\n", i, str->wr_dq_delay[i], str->wr_dq_delay[i+1], str->wr_dq_delay[i+2], str->wr_dq_delay[i+3]);
	}
	return;
}
MEMCNTLR_SECTION static inline void mc_xlat_n_assign_wr_dq_delay(mc_dram_param_t * __attribute__ ((unused)), mc_cntlr_opt_t * __attribute__ ((unused)), mc_register_set_t *);
MEMCNTLR_SECTION void __attribute__ ((weak)) mc_dump_rdr_dq_delay(mc_cntlr_opt_t *str) {
	u32_t i;
	for (i=0; i<16; i+=4) {
		printf("  rdr_dq_delay[%02d]: %d, %d, %d, %d\n", i, str->rdr_dq_delay[i], str->rdr_dq_delay[i+1], str->rdr_dq_delay[i+2], str->rdr_dq_delay[i+3]);
	}
	return;
}
MEMCNTLR_SECTION static inline void mc_xlat_n_assign_rdr_dq_delay(mc_dram_param_t * __attribute__ ((unused)), mc_cntlr_opt_t * __attribute__ ((unused)), mc_register_set_t *);
MEMCNTLR_SECTION void __attribute__ ((weak)) mc_dump_rdf_dq_delay(mc_cntlr_opt_t *str) {
	u32_t i;
	for (i=0; i<16; i+=4) {
		printf("  rdf_dq_delay[%02d]: %d, %d, %d, %d\n", i, str->rdf_dq_delay[i], str->rdf_dq_delay[i+1], str->rdf_dq_delay[i+2], str->rdf_dq_delay[i+3]);
	}
	return;
}
MEMCNTLR_SECTION static inline void mc_xlat_n_assign_rdf_dq_delay(mc_dram_param_t * __attribute__ ((unused)), mc_cntlr_opt_t * __attribute__ ((unused)), mc_register_set_t *);
MEMCNTLR_SECTION void __attribute__ ((weak)) mc_dump_write_lvl_en(mc_cntlr_opt_t *str) {
	printf("  write_lvl_en: %d\n", str->write_lvl_en);
	return;
}
MEMCNTLR_SECTION static inline void mc_xlat_n_assign_write_lvl_en(mc_dram_param_t * __attribute__ ((unused)), mc_cntlr_opt_t * __attribute__ ((unused)), mc_register_set_t *);
MEMCNTLR_SECTION void __attribute__ ((weak)) mc_dump_dll_dis(mc_cntlr_opt_t *str) {
	printf("  dll_dis: %d\n", str->dll_dis);
	return;
}
MEMCNTLR_SECTION static inline void mc_xlat_n_assign_dll_dis(mc_dram_param_t * __attribute__ ((unused)), mc_cntlr_opt_t * __attribute__ ((unused)), mc_register_set_t *);
MEMCNTLR_SECTION void __attribute__ ((weak)) mc_dump_cal_method(mc_cntlr_opt_t *str) {
	printf("  cal_method: %d\n", str->cal_method);
	return;
}
MEMCNTLR_SECTION static inline void mc_xlat_n_assign_cal_method(mc_dram_param_t * __attribute__ ((unused)), mc_cntlr_opt_t * __attribute__ ((unused)), mc_register_set_t *);
MEMCNTLR_SECTION static inline void mc_cntlr_opt_dump(void) {
	mc_cntlr_opt_t *str = meminfo.cntlr_opt;
	printf("II: meminfo.mc_cntlr_opt @ %p (%dB)\n", str, sizeof(mc_cntlr_opt_t));
	mc_dump_dqs0_en_hclk(str);
	mc_dump_dqs1_en_hclk(str);
	mc_dump_fast_exit(str);
	mc_dump_normal_drv(str);
	mc_dump_rtt_nom_ohm(str);
	mc_dump_rtt_wr_ohm(str);
	mc_dump_cntlr_odt(str);
	mc_dump_cntlr_ocd(str);
	mc_dump_tx_delay(str);
	mc_dump_clkm_delay(str);
	mc_dump_clkm90_delay(str);
	mc_dump_dqs0_en_tap(str);
	mc_dump_dqs1_en_tap(str);
	mc_dump_dqs0_group_tap(str);
	mc_dump_dqs1_group_tap(str);
	mc_dump_dqm0_tap(str);
	mc_dump_dqm1_tap(str);
	mc_dump_wr_dq_delay(str);
	mc_dump_rdr_dq_delay(str);
	mc_dump_rdf_dq_delay(str);
	mc_dump_write_lvl_en(str);
	mc_dump_dll_dis(str);
	mc_dump_cal_method(str);
}
MEMCNTLR_SECTION void __attribute__ ((weak)) mc_dump_mcr(mc_register_set_t *str) {
	printf("  mcr: %08x\n", str->mcr.v);
	return;
}
MEMCNTLR_SECTION void __attribute__ ((weak)) mc_dump_dcr(mc_register_set_t *str) {
	printf("  dcr: %08x\n", str->dcr.v);
	return;
}
MEMCNTLR_SECTION void __attribute__ ((weak)) mc_dump_dtr0(mc_register_set_t *str) {
	printf("  dtr0: %08x\n", str->dtr0.v);
	return;
}
MEMCNTLR_SECTION void __attribute__ ((weak)) mc_dump_dtr1(mc_register_set_t *str) {
	printf("  dtr1: %08x\n", str->dtr1.v);
	return;
}
MEMCNTLR_SECTION void __attribute__ ((weak)) mc_dump_dtr2(mc_register_set_t *str) {
	printf("  dtr2: %08x\n", str->dtr2.v);
	return;
}
MEMCNTLR_SECTION void __attribute__ ((weak)) mc_dump_dider(mc_register_set_t *str) {
	printf("  dider: %08x\n", str->dider.v);
	return;
}
MEMCNTLR_SECTION void __attribute__ ((weak)) mc_dump_drcko(mc_register_set_t *str) {
	printf("  drcko: %08x\n", str->drcko.v);
	return;
}
MEMCNTLR_SECTION void __attribute__ ((weak)) mc_dump_daccr(mc_register_set_t *str) {
	printf("  daccr: %08x\n", str->daccr.v);
	return;
}
MEMCNTLR_SECTION void __attribute__ ((weak)) mc_dump_ddzqpr(mc_register_set_t *str) {
	printf("  ddzqpr: %08x\n", str->ddzqpr.v);
	return;
}
MEMCNTLR_SECTION void __attribute__ ((weak)) mc_dump_dcdqmr(mc_register_set_t *str) {
	printf("  dcdqmr: %08x\n", str->dcdqmr.v);
	return;
}
MEMCNTLR_SECTION void __attribute__ ((weak)) mc_dump_ddzqpcr(mc_register_set_t *str) {
	printf("  ddzqpcr: %08x\n", str->ddzqpcr.v);
	return;
}
MEMCNTLR_SECTION void __attribute__ ((weak)) mc_dump_mr(mc_register_set_t *str) {
	u32_t i;
	for (i=0; i<4; i+=4) {
		printf("  mr[%02d]: %08x, %08x, %08x, %08x\n", i, str->mr[i].v, str->mr[i+1].v, str->mr[i+2].v, str->mr[i+3].v);
	}
	return;
}
MEMCNTLR_SECTION void __attribute__ ((weak)) mc_dump_dacdq_rr(mc_register_set_t *str) {
	u32_t i;
	for (i=0; i<16; i+=4) {
		printf("  dacdq_rr[%02d]: %08x, %08x, %08x, %08x\n", i, str->dacdq_rr[i].v, str->dacdq_rr[i+1].v, str->dacdq_rr[i+2].v, str->dacdq_rr[i+3].v);
	}
	return;
}
MEMCNTLR_SECTION void __attribute__ ((weak)) mc_dump_dacdq_fr(mc_register_set_t *str) {
	u32_t i;
	for (i=0; i<16; i+=4) {
		printf("  dacdq_fr[%02d]: %08x, %08x, %08x, %08x\n", i, str->dacdq_fr[i].v, str->dacdq_fr[i+1].v, str->dacdq_fr[i+2].v, str->dacdq_fr[i+3].v);
	}
	return;
}
MEMCNTLR_SECTION static inline void mc_register_set_dump(void) {
	mc_register_set_t *str = meminfo.register_set;
	printf("II: meminfo.mc_register_set @ %p (%dB)\n", str, sizeof(mc_register_set_t));
	mc_dump_mcr(str);
	mc_dump_dcr(str);
	mc_dump_dtr0(str);
	mc_dump_dtr1(str);
	mc_dump_dtr2(str);
	mc_dump_dider(str);
	mc_dump_drcko(str);
	mc_dump_daccr(str);
	mc_dump_ddzqpr(str);
	mc_dump_dcdqmr(str);
	mc_dump_ddzqpcr(str);
	mc_dump_mr(str);
	mc_dump_dacdq_rr(str);
	mc_dump_dacdq_fr(str);
}
MEMCNTLR_SECTION static inline void mc_xlat_n_assign(void) {
	mc_dram_param_t   *dp = meminfo.dram_param;
	mc_cntlr_opt_t    *co = meminfo.cntlr_opt;
	mc_register_set_t *rs = meminfo.register_set;
	if (meminfo.dram_param->dram_type != -1) {
		mc_xlat_n_assign_dram_type(dp, co, rs);
		if (ISTAT_GET(xlat) != MEM_XLAT_UNINIT) return;
	}
	if (meminfo.dram_param->bankcnt != -1) {
		mc_xlat_n_assign_bankcnt(dp, co, rs);
		if (ISTAT_GET(xlat) != MEM_XLAT_UNINIT) return;
	}
	if (meminfo.dram_param->buswidth != -1) {
		mc_xlat_n_assign_buswidth(dp, co, rs);
		if (ISTAT_GET(xlat) != MEM_XLAT_UNINIT) return;
	}
	if (meminfo.dram_param->max_row_addr != -1) {
		mc_xlat_n_assign_max_row_addr(dp, co, rs);
		if (ISTAT_GET(xlat) != MEM_XLAT_UNINIT) return;
	}
	if (meminfo.dram_param->max_col_addr != -1) {
		mc_xlat_n_assign_max_col_addr(dp, co, rs);
		if (ISTAT_GET(xlat) != MEM_XLAT_UNINIT) return;
	}
	if (meminfo.dram_param->min_trcd_ns != -1) {
		mc_xlat_n_assign_min_trcd_ns(dp, co, rs);
		if (ISTAT_GET(xlat) != MEM_XLAT_UNINIT) return;
	}
	if (meminfo.dram_param->min_trp_ns != -1) {
		mc_xlat_n_assign_min_trp_ns(dp, co, rs);
		if (ISTAT_GET(xlat) != MEM_XLAT_UNINIT) return;
	}
	if (meminfo.dram_param->min_tras_ns != -1) {
		mc_xlat_n_assign_min_tras_ns(dp, co, rs);
		if (ISTAT_GET(xlat) != MEM_XLAT_UNINIT) return;
	}
	if (meminfo.dram_param->cl_ck != -1) {
		mc_xlat_n_assign_cl_ck(dp, co, rs);
		if (ISTAT_GET(xlat) != MEM_XLAT_UNINIT) return;
	}
	if (meminfo.dram_param->cwl_ck != -1) {
		mc_xlat_n_assign_cwl_ck(dp, co, rs);
		if (ISTAT_GET(xlat) != MEM_XLAT_UNINIT) return;
	}
	if (meminfo.dram_param->min_twr_ns != -1) {
		mc_xlat_n_assign_min_twr_ns(dp, co, rs);
		if (ISTAT_GET(xlat) != MEM_XLAT_UNINIT) return;
	}
	if (meminfo.dram_param->min_trtp_ns != -1) {
		mc_xlat_n_assign_min_trtp_ns(dp, co, rs);
		if (ISTAT_GET(xlat) != MEM_XLAT_UNINIT) return;
	}
	if (meminfo.dram_param->min_twtr_ns != -1) {
		mc_xlat_n_assign_min_twtr_ns(dp, co, rs);
		if (ISTAT_GET(xlat) != MEM_XLAT_UNINIT) return;
	}
	if (meminfo.dram_param->min_trrd_ns != -1) {
		mc_xlat_n_assign_min_trrd_ns(dp, co, rs);
		if (ISTAT_GET(xlat) != MEM_XLAT_UNINIT) return;
	}
	if (meminfo.dram_param->min_tfaw_ns != -1) {
		mc_xlat_n_assign_min_tfaw_ns(dp, co, rs);
		if (ISTAT_GET(xlat) != MEM_XLAT_UNINIT) return;
	}
	if (meminfo.dram_param->min_trfc_ns != -1) {
		mc_xlat_n_assign_min_trfc_ns(dp, co, rs);
		if (ISTAT_GET(xlat) != MEM_XLAT_UNINIT) return;
	}
	if (meminfo.dram_param->max_trefi_ns != -1) {
		mc_xlat_n_assign_max_trefi_ns(dp, co, rs);
		if (ISTAT_GET(xlat) != MEM_XLAT_UNINIT) return;
	}
	if (meminfo.dram_param->min_tck_ps != -1) {
		mc_xlat_n_assign_min_tck_ps(dp, co, rs);
		if (ISTAT_GET(xlat) != MEM_XLAT_UNINIT) return;
	}
	if (meminfo.dram_param->max_tck_ps != -1) {
		mc_xlat_n_assign_max_tck_ps(dp, co, rs);
		if (ISTAT_GET(xlat) != MEM_XLAT_UNINIT) return;
	}
	if (meminfo.cntlr_opt->dqs0_en_hclk != -1) {
		mc_xlat_n_assign_dqs0_en_hclk(dp, co, rs);
		if (ISTAT_GET(xlat) != MEM_XLAT_UNINIT) return;
	}
	if (meminfo.cntlr_opt->dqs1_en_hclk != -1) {
		mc_xlat_n_assign_dqs1_en_hclk(dp, co, rs);
		if (ISTAT_GET(xlat) != MEM_XLAT_UNINIT) return;
	}
	if (meminfo.cntlr_opt->fast_exit != -1) {
		mc_xlat_n_assign_fast_exit(dp, co, rs);
		if (ISTAT_GET(xlat) != MEM_XLAT_UNINIT) return;
	}
	if (meminfo.cntlr_opt->normal_drv != -1) {
		mc_xlat_n_assign_normal_drv(dp, co, rs);
		if (ISTAT_GET(xlat) != MEM_XLAT_UNINIT) return;
	}
	if (meminfo.cntlr_opt->rtt_nom_ohm != -1) {
		mc_xlat_n_assign_rtt_nom_ohm(dp, co, rs);
		if (ISTAT_GET(xlat) != MEM_XLAT_UNINIT) return;
	}
	if (meminfo.cntlr_opt->rtt_wr_ohm != -1) {
		mc_xlat_n_assign_rtt_wr_ohm(dp, co, rs);
		if (ISTAT_GET(xlat) != MEM_XLAT_UNINIT) return;
	}
	if (meminfo.cntlr_opt->cntlr_odt != -1) {
		mc_xlat_n_assign_cntlr_odt(dp, co, rs);
		if (ISTAT_GET(xlat) != MEM_XLAT_UNINIT) return;
	}
	if (meminfo.cntlr_opt->cntlr_ocd != -1) {
		mc_xlat_n_assign_cntlr_ocd(dp, co, rs);
		if (ISTAT_GET(xlat) != MEM_XLAT_UNINIT) return;
	}
	if (meminfo.cntlr_opt->tx_delay != -1) {
		mc_xlat_n_assign_tx_delay(dp, co, rs);
		if (ISTAT_GET(xlat) != MEM_XLAT_UNINIT) return;
	}
	if (meminfo.cntlr_opt->clkm_delay != -1) {
		mc_xlat_n_assign_clkm_delay(dp, co, rs);
		if (ISTAT_GET(xlat) != MEM_XLAT_UNINIT) return;
	}
	if (meminfo.cntlr_opt->clkm90_delay != -1) {
		mc_xlat_n_assign_clkm90_delay(dp, co, rs);
		if (ISTAT_GET(xlat) != MEM_XLAT_UNINIT) return;
	}
	if (meminfo.cntlr_opt->dqs0_en_tap != -1) {
		mc_xlat_n_assign_dqs0_en_tap(dp, co, rs);
		if (ISTAT_GET(xlat) != MEM_XLAT_UNINIT) return;
	}
	if (meminfo.cntlr_opt->dqs1_en_tap != -1) {
		mc_xlat_n_assign_dqs1_en_tap(dp, co, rs);
		if (ISTAT_GET(xlat) != MEM_XLAT_UNINIT) return;
	}
	if (meminfo.cntlr_opt->dqs0_group_tap != -1) {
		mc_xlat_n_assign_dqs0_group_tap(dp, co, rs);
		if (ISTAT_GET(xlat) != MEM_XLAT_UNINIT) return;
	}
	if (meminfo.cntlr_opt->dqs1_group_tap != -1) {
		mc_xlat_n_assign_dqs1_group_tap(dp, co, rs);
		if (ISTAT_GET(xlat) != MEM_XLAT_UNINIT) return;
	}
	if (meminfo.cntlr_opt->dqm0_tap != -1) {
		mc_xlat_n_assign_dqm0_tap(dp, co, rs);
		if (ISTAT_GET(xlat) != MEM_XLAT_UNINIT) return;
	}
	if (meminfo.cntlr_opt->dqm1_tap != -1) {
		mc_xlat_n_assign_dqm1_tap(dp, co, rs);
		if (ISTAT_GET(xlat) != MEM_XLAT_UNINIT) return;
	}
	mc_xlat_n_assign_wr_dq_delay(dp, co, rs);
	mc_xlat_n_assign_rdr_dq_delay(dp, co, rs);
	mc_xlat_n_assign_rdf_dq_delay(dp, co, rs);
	if (meminfo.cntlr_opt->write_lvl_en != -1) {
		mc_xlat_n_assign_write_lvl_en(dp, co, rs);
		if (ISTAT_GET(xlat) != MEM_XLAT_UNINIT) return;
	}
	if (meminfo.cntlr_opt->dll_dis != -1) {
		mc_xlat_n_assign_dll_dis(dp, co, rs);
		if (ISTAT_GET(xlat) != MEM_XLAT_UNINIT) return;
	}
	if (meminfo.cntlr_opt->cal_method != -1) {
		mc_xlat_n_assign_cal_method(dp, co, rs);
		if (ISTAT_GET(xlat) != MEM_XLAT_UNINIT) return;
	}
	return;
}
