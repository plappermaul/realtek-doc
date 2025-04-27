#include <soc.h>
#include <dram/memcntlr.h>
#include <plr_sections.h>
#include <util.h>
#include <cg/cg.h>

#define SECTION_PROBEINFO __attribute__ ((section (".parameters")))

#undef _soc

#ifndef SECTION_SOC
    #define SECTION_SOC      __attribute__ ((section (".sdata.soc_stru")))
#endif

extern void writeback_invalidate_dcache_all(void);
//extern void rlx5281_write_back_inv_dcache_range(unsigned int base_addr, unsigned int end_addr);
extern void invalidate_icache_all(void);
//extern void rlx5281_invalidate_icache_range(unsigned int base_addr, unsigned int end_addr);

#ifndef PLR_VERSION
    #define PLR_VERSION 0
#endif

extern init_table_entry_t start_of_init_func_table, end_of_init_func_table;
extern symbol_table_entry_t start_of_symble_table, end_of_symble_table;

/* plr_nor_spi_info_t gen_nor_spi_info = { */
/* 	.num_chips = 0x01, */
/* 	.addr_mode = 0x03, */
/* 	.prefer_divisor = 0x10, */
/* 	.size_per_chip = 0x17, */
/* 	.prefer_rx_delay0 = 0x00, */
/* 	.prefer_rx_delay1 = 0x00, */
/* 	.prefer_rx_delay2 = 0x00, */
/* 	.prefer_rx_delay3 = 0x00, */
/* 	.prefer_rd_cmd = 0x03, */
/* 	.prefer_rd_cmd_io = 0x00, */
/* 	.prefer_rd_dummy_c = 0x00, */
/* 	.prefer_rd_addr_io = 0x00, */
/* 	.prefer_rd_data_io = 0x00, */
/* 	.wr_cmd = 0x02, */
/* 	.wr_cmd_io = 0x00, */
/* 	.wr_dummy_c = 0x00, */
/* 	.wr_addr_io = 0x00, */
/* 	.wr_data_io = 0x00, */
/* 	.wr_boundary = 0x08, */
/* 	.erase_cmd = 0x20, */
/* 	.erase_unit = 0xc, */
/* 	.pm_method = 0x00, */
/* 	.pm_rdsr_cmd = 0x00, */
/* 	.pm_rdsr2_cmd = 0x00, */
/* 	.pm_wrsr_cmd = 0x00, */
/* 	.pm_enable_cmd = 0x00, */
/* 	.pm_enable_bits = 0x0000, */
/* 	.pm_status_len = 0x00, */
/* 	.rdbusy_cmd = 0x05, */
/* 	.rdbusy_len = 0x02, */
/* 	.rdbusy_loc = 0x00, */
/* 	.rdbusy_polling_period = 0x00, */
/* 	.id = 0xFFFFFF, */

/* 	._nor_spi_erase = VZERO, */
/* 	._nor_spi_read = VZERO, */
/* 	._nor_spi_write = VZERO, */
/* }; */

soc_t _soc SECTION_SOC = {
    .bios={
        .header= {
            .signature=SIGNATURE_PLR,
            .export_symb_list=&start_of_symble_table,
            .end_of_export_symb_list=&end_of_symble_table,
            .init_func_list=&start_of_init_func_table,
            .end_of_init_func_list=&end_of_init_func_table,
        },
        .isr=VZERO,
        .size_of_plr_load_firstly=0,
        .uart_putc=VZERO,
        .uart_getc=VZERO,
        .uart_tstc=VZERO,
        .dcache_writeback_invalidate_all= &writeback_invalidate_dcache_all,
        //.dcache_writeback_invalidate_range=&rlx5281_write_back_inv_dcache_range,
        .icache_invalidate_all=&invalidate_icache_all,
        //.icache_invalidate_range=&rlx5281_invalidate_icache_range,
    },
    /* .flash_info.nor_spi_info = &gen_nor_spi_info, */
};

u32_t uart_baud_rate  = 115200;

const mc_dram_param_t ddr2_database_para SECTION_PROBEINFO = {
	#include "dram_models/samsung_K4T51163QI-HCE7.h"
};

const mc_dram_param_t ddr3_database_para SECTION_PROBEINFO = {
    #include "dram_models/samsung_K4B1G1646G-BCH9_40MHz.h"
};

const mc_cntlr_opt_t ddr2_cntlr_opt SECTION_PROBEINFO = {
	.dqs0_en_hclk   = 0,
	.dqs1_en_hclk   = 0,
	.fast_exit      = 1,
	.normal_drv     = 1,
	.rtt_nom_ohm    = 0,
	.write_lvl_en   = 0,
	.dll_dis        = 0,
	.cntlr_odt      = 75,
	.cntlr_ocd      = 80,
	.tx_delay       = 0,
	.clkm_delay     = 0,
	.clkm90_delay   = 0,
	.dqs0_en_tap    = 0,
	.dqs1_en_tap    = 0,
	.dqs0_group_tap = 0,
	.dqs1_group_tap = 0,
	.dqm0_tap       = 0,
	.dqm1_tap       = 0,
	.wr_dq_delay    = {0},
	.rdr_dq_delay   = {0xf, 0xf, 0xf, 0xf,
	                   0xf, 0xf, 0xf, 0xf,
                       0xf, 0xf, 0xf, 0xf,
                       0xf, 0xf, 0xf, 0xf},
    .rdf_dq_delay   = {0xf, 0xf, 0xf, 0xf,
                       0xf, 0xf, 0xf, 0xf,
                       0xf, 0xf, 0xf, 0xf,
                       0xf, 0xf, 0xf, 0xf},
	.cal_method     = 0,
	.clk_dll_type   = 1,
	.dq_dll_type    = 1,
	.zq_cal_method  = 0,
	.parallel_bank_en = 1,
};

const mc_cntlr_opt_t ddr3_cntlr_opt SECTION_PROBEINFO = {
	.dqs0_en_hclk   = 0,
	.dqs1_en_hclk   = 0,
	.fast_exit      = 0,	// fast exit (dll on), slow exit (dll off)
	.normal_drv     = 1,
	.rtt_nom_ohm    = 120,
	.write_lvl_en   = 0,
	.dll_dis        = 0,
	.cntlr_odt      = 75,
	.cntlr_ocd      = 80,
	.tx_delay       = 0,
	.clkm_delay     = 0,
	.clkm90_delay   = 0,
	.dqs0_en_tap    = 0,
	.dqs1_en_tap    = 0,
	.dqs0_group_tap = 0,
	.dqs1_group_tap = 0,
	.dqm0_tap       = 0,
	.dqm1_tap       = 0,
	.wr_dq_delay    = {0},
	.rdr_dq_delay   = {0},
	.rdf_dq_delay   = {0},
	.cal_method     = 0,
	.clk_dll_type   = 1,
	.dq_dll_type    = 1,
	.zq_cal_method  = 0,
	.parallel_bank_en = 1,
};

const mc_register_set_t regset SECTION_PROBEINFO = {
	.mcr.v      = MCRdv,
	.dcr.v      = DCRdv,
	.dtr0.v     = DTR0dv,
	.dtr1.v     = DTR1dv,
	.dtr2.v     = DTR2dv,
	.dider.v    = DIDERdv,
	.drcko.v    = DRCKOdv,
	.daccr.v    = DACCRdv,
	.ddzqpr.v   = DDZQPRdv,
	.dcdqmr.v   = DCDQMRdv,
	.ddzqpcr.v  = DDZQPCRdv,
	.dcsdcr0.v  = DCSDCR0dv,
	.dcsdcr1.v  = DCSDCR1dv,
	.dcsdcr2.v  = DCSDCR2dv,
	.dcsdcr3.v  = DCSDCR3dv,
	.ana_dll0.v = ANA_DLL0dv,

	.mr[0].f.mr_mode_en = 1,
	.mr[1].f.mr_mode_en = 1,
	.mr[2].f.mr_mode_en = 1,
	.mr[3].f.mr_mode_en = 1,
	.mr[0].f.mr_mode    = 0,
	.mr[1].f.mr_mode    = 1,
	.mr[2].f.mr_mode    = 2,
	.mr[3].f.mr_mode    = 3,

	.dacdq_rr  = {
		{.v = DACDQ_IDX_RRdv}, {.v = DACDQ_IDX_RRdv},
		{.v = DACDQ_IDX_RRdv}, {.v = DACDQ_IDX_RRdv},
		{.v = DACDQ_IDX_RRdv}, {.v = DACDQ_IDX_RRdv},
		{.v = DACDQ_IDX_RRdv}, {.v = DACDQ_IDX_RRdv},
		{.v = DACDQ_IDX_RRdv}, {.v = DACDQ_IDX_RRdv},
		{.v = DACDQ_IDX_RRdv}, {.v = DACDQ_IDX_RRdv},
		{.v = DACDQ_IDX_RRdv}, {.v = DACDQ_IDX_RRdv},
		{.v = DACDQ_IDX_RRdv}, {.v = DACDQ_IDX_RRdv}
	},
	.dacdq_fr  = {
		{.v = DACDQ_IDX_FRdv}, {.v = DACDQ_IDX_FRdv},
		{.v = DACDQ_IDX_FRdv}, {.v = DACDQ_IDX_FRdv},
		{.v = DACDQ_IDX_FRdv}, {.v = DACDQ_IDX_FRdv},
		{.v = DACDQ_IDX_FRdv}, {.v = DACDQ_IDX_FRdv},
		{.v = DACDQ_IDX_FRdv}, {.v = DACDQ_IDX_FRdv},
		{.v = DACDQ_IDX_FRdv}, {.v = DACDQ_IDX_FRdv},
		{.v = DACDQ_IDX_FRdv}, {.v = DACDQ_IDX_FRdv},
		{.v = DACDQ_IDX_FRdv}, {.v = DACDQ_IDX_FRdv}
	},
};

const mc_info_t mi[] SECTION_PROBEINFO = {
	{(mc_dram_param_t *)&ddr3_database_para,
	 (mc_cntlr_opt_t *)&ddr3_cntlr_opt,
	 (mc_register_set_t *)&regset},
	{(mc_dram_param_t *)&ddr2_database_para,
	 (mc_cntlr_opt_t *)&ddr2_cntlr_opt,
	 (mc_register_set_t *)&regset},
};

MEMCNTLR_SECTION
const mc_info_t *
proj_mc_info_select(void) {
	if (RFLD_MCR(dram_type) == 2) {
		return &mi[0];
	} else if (RFLD_MCR(dram_type) == 1) {
		return &mi[1];
	} else {
		return VZERO;
	}
}

symb_fdefine(SF_SYS_UDELAY, otto_sys_udelay);
/* symb_pdefine(gen_nor_spi_info, SP_NOR_SPIF_INFO, &gen_nor_spi_info); */

const cg_info_t cg_info_proj SECTION_PROBEINFO = {
	.dev_freq ={
		.ocp_mhz  = 66,
		.mem_mhz  = 40,
		.lx_mhz   = 25,
		.spif_mhz = 25,
	},
    /*.register_set = {
		.syspllctr.v  = SYSPLLCTRdv,
		.lx_clk_pll.v = LX_CLK_PLLdv,
    },*/
};
symb_fdefine(SF_SYS_QUERY_FREQ, cg_query_freq);

