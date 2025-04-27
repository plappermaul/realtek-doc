#include <dram/memcntlr.h>
#include <util.h>
#include <cg/cg.h>
#include <plr_sections.h>

#define SECTION_PROBEINFO __attribute__ ((section (".parameters")))

#undef _soc

#ifndef SECTION_SOC
    #define SECTION_SOC      __attribute__ ((section (".sdata.soc_stru")))
#endif

extern init_table_entry_t start_of_init_func_table, end_of_init_func_table;
extern symbol_table_entry_t start_of_symble_table, end_of_symble_table;
extern void writeback_invalidate_dcache_all(void);
extern void writeback_invalidate_dcache_range(u32_t base_addr, u32_t end_addr);
extern void invalidate_icache_all(void);
extern void invalidate_icache_range(u32_t base_addr, u32_t end_addr);

#ifndef PLR_VERSION
    #define PLR_VERSION 0
#endif

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
        .dcache_writeback_invalidate_all=&writeback_invalidate_dcache_all,
        .dcache_writeback_invalidate_range=&writeback_invalidate_dcache_range,
        .icache_invalidate_all=&invalidate_icache_all,
        .icache_invalidate_range=&invalidate_icache_range,
    },
};

/* DDR3 */
const mc_register_set_t ddr3_regset SECTION_PROBEINFO = {
    .dcr.v      = DCRdv,
    .iocr.v     = IOCRdv,
    .tpr0.v     = TPR0dv,
    .tpr1.v     = TPR1dv,
    .tpr2.v     = TPR2dv,
    .tpr3.v     = TPR3dv,
    /* modify MR default value */
    .mr0.v      = DDR3_MR0dv,
    .mr1.v      = DDR3_MR1dv,
    .mr2.v      = DDR3_MR2dv,
    .mr3.v      = DDR3_MR3dv,
};

const mc_cntlr_opt_t ddr3_cntlr_opt SECTION_PROBEINFO = {
    //#include "dram_models/nanya_NT5CC256M16EP-EK.h"
    #include "dram_models/jedec_ddr3_1600.h"
};

const mc_dpi_opt_t ddr3_dpi_opt SECTION_PROBEINFO = {
    .rzq_ext        = 0,    // 1: external, 0: internal R480 enable
    .data_pre       = 1,
    .cmd_grp2_en    = 0,
    .ocd_odt        = { ZP_X_OCD_ODT(37, 60, ZP_D3),
                        ZP_X_OCD_ODT(37, 75, ZP_D3_PD3),
                        0, 0 },
    #include "dram_models/board_QAB_2xDDR3_12XSFP.h"
    //#include "dram_models/board_QAB_48G_6X10G.h"
    //#include "dram_models/board_DEMO_48G_6X10G.h"
};

/* DDR4 */
const mc_register_set_t ddr4_regset SECTION_PROBEINFO = {
    .dcr.v      = DCRdv,
    .iocr.v     = IOCRdv,
    .tpr0.v     = TPR0dv,
    .tpr1.v     = TPR1dv,
    .tpr2.v     = TPR2dv,
    .tpr3.v     = TPR3dv,
    /* modify MR default value */
    .mr0.v      = DDR4_MR0dv,     // default, DLL on and fast exit, turn off test mode
    .mr1.v      = DDR4_MR1dv,     // dll enable
    .mr2.v      = DDR4_MR2dv,
    .mr3.v      = DDR4_MR3dv,
    .mr4.v      = DDR4_MR4dv,
    .mr5.v      = DDR4_MR5dv,
    .mr6.v      = DDR4_MR6dv,
};

const mc_cntlr_opt_t ddr4_cntlr_opt SECTION_PROBEINFO = {
    #include "dram_models/jedec_ddr4_1600.h"
};

const mc_dpi_opt_t ddr4_dpi_opt SECTION_PROBEINFO = {
    .rzq_ext        = 0,    // 1:external, 0: internal R480 enable
    .data_pre       = 1,
    .cmd_grp2_en    = 0,
    .ocd_odt        = { ZP_X_OCD_ODT(40, 60, ZP_D4_P5_CMD),
                        ZP_X_OCD_ODT(40, 60, ZP_D4_P8_DQ),
                        ZP_X_OCD_ODT(40, 73, ZP_D4_P8_PD3),
                        0 },
    #include "dram_models/board_QAB_DDR4.h"
};

const mc_info_t mi[] SECTION_PROBEINFO = {
	{(mc_cntlr_opt_t *)&ddr3_cntlr_opt,
	 (mc_dpi_opt_t *)&ddr3_dpi_opt,
	 (mc_register_set_t *)&ddr3_regset},
	{(mc_cntlr_opt_t *)&ddr4_cntlr_opt,
	 (mc_dpi_opt_t *)&ddr4_dpi_opt,
	 (mc_register_set_t *)&ddr4_regset},
};

MEMCNTLR_SECTION
const mc_info_t *
proj_mc_info_select(void) {
	u8_t dt = RFLD_PSR0(pin_dramtype);
	if(1==dt) {
		return &mi[1];
	} else {
	return &mi[0];
}
}

const u32_t uart_baud_rate SECTION_PROBEINFO=0;
const cg_info_t cg_info_proj SECTION_PROBEINFO = {
	.dev_freq ={
		.cpu_mhz  = 1000,
		.mem_mhz  = 800,
		.lx_mhz   = 200,
		.spif_mhz = 25,
	},
};

symb_fdefine(SF_SYS_UDELAY,    otto_lx_timer_udelay);
symb_fdefine(SF_SYS_GET_TIMER, otto_lx_timer_get_timer);
symb_pdefine(cg_info_dev_freq, SF_SYS_CG_DEV_FREQ, &(cg_info_query.dev_freq));
