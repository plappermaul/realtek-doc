// This file is used to store parameters for DDR, and/or flash
#define DONT_DECLAIRE__SOC_SYMBOLS
#include <dram/memcntlr.h>
#include <cg/cg.h>
#include <uart/uart.h>
#include <plr_sections.h>
#include <symb_define.h>
#include <util.h>
#include <spi_nand/spi_nand_struct.h>

extern init_table_entry_t start_of_init_func_table, end_of_init_func_table;
extern symbol_table_entry_t start_of_symble_table, end_of_symble_table;
extern unsigned int lma_offset_before_mapped_area;
extern void plr_tlb_miss_isr(void);

#ifndef SECTION_SOC
    #define SECTION_SOC         SECTION_SOC_STRU
#endif 

spi_nand_cmd_info_t   plr_cmd_info SECTION_SDATA;
spi_nand_model_info_t plr_model_info SECTION_SDATA;
spi_nand_flash_info_t   plr_spi_nand_flash_info SECTION_SDATA ={
    ._cmd_info   = &plr_cmd_info,
    ._model_info = &plr_model_info,
};

soc_t _soc SECTION_SOC = {
    .bios={
        .header= {
            .signature=SIGNATURE_PLR_FL,
            .version=PLR_VERSION,
            .export_symb_list=&start_of_symble_table,
            .end_of_export_symb_list=&end_of_symble_table,
            .init_func_list=&start_of_init_func_table,
            .end_of_init_func_list=&end_of_init_func_table,
        },
        .isr=plr_tlb_miss_isr,
        .size_of_plr_load_firstly=(u32_t)&lma_offset_before_mapped_area,
        .uart_putc=VZERO,
        .uart_getc=VZERO,
        .uart_tstc=VZERO,
        .dcache_writeback_invalidate_all=VZERO,
        .icache_invalidate_all=VZERO,
    },
    .flash_info.spi_nand_info=&plr_spi_nand_flash_info,
};

symb_idefine(boot_storage_type, SF_BOOT_STORAGE_TYPE, BOOT_FROM_SPI_NAND);

const u32_t uart_baud_rate SECTION_PARAMETERS=0;
const cg_info_t cg_info_proj SECTION_PARAMETERS = {
        .dev_freq ={
                .cpu_mhz  = 800,
                .mem_mhz  = 600,
                .lx_mhz   = 175,
                .spif_mhz = 25,
		.sram_mhz = OTTO_SRAM_DEF_FREQ, // do not change this
        },
};

symb_pdefine(cg_info_dev_freq, SF_SYS_CG_DEV_FREQ, &(cg_info_query.dev_freq));
