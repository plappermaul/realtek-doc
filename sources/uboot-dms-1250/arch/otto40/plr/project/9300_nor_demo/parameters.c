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
extern void writeback_invalidate_dcache_range(u32_t base_addr, u32_t end_addr);
extern void invalidate_icache_all(void);
extern void invalidate_icache_range(u32_t base_addr, u32_t end_addr);

#ifndef PLR_VERSION
    #define PLR_VERSION 0x04000000 
#endif

extern init_table_entry_t start_of_init_func_table, end_of_init_func_table;
extern symbol_table_entry_t start_of_symble_table, end_of_symble_table;

soc_t _soc SECTION_SOC = {
    .bios={
        .header= {
            .signature=SIGNATURE_PLR,
            .version=PLR_VERSION,
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
        .dcache_writeback_invalidate_range=&writeback_invalidate_dcache_range,
        .icache_invalidate_all=&invalidate_icache_all,
        .icache_invalidate_range=&invalidate_icache_range,
    },
};

u32_t uart_baud_rate  = 0;
const cg_info_t cg_info_proj SECTION_PROBEINFO = {
	.dev_freq ={
        .cpu_mhz  = 800,
        .mem_mhz  = 600,
        .lx_mhz   = 175,
	.spif_mhz = 25,
        .sram_mhz = OTTO_SRAM_DEF_FREQ, // do not change this
	},
};

symb_fdefine(SF_SYS_UDELAY,    otto_lx_timer_udelay);
symb_fdefine(SF_SYS_GET_TIMER, otto_lx_timer_get_timer);
symb_pdefine(cg_info_dev_freq, SF_SYS_CG_DEV_FREQ, &(cg_info_query.dev_freq));
