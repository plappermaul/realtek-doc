#include <soc.h>
#include <util.h>
#include <cpu/tlb.h>
#include <cpu/mmu_drv.h>
#include <init_define.h>
#include <spi_nand/spi_nand_util.h>
#include <register_map.h>

extern u32_t mapped_physical_sram_start;
extern void plr_tlb_miss_tlb (void);

char _mmu_init_fail_msg[] SECTION_RECYCLE_DATA = {"no physical page was added, system stops\n"};
char _nsu_table_init_fail_msg[] SECTION_RECYCLE_DATA = {"plr mapping table init fail\n"};
char _nsu_probe_fail_msg[] SECTION_RECYCLE_DATA = {"plr probe nand flash device fail\n"};

extern const spi_nand_probe_t *start_of_spi_nand_probe_func;
extern const spi_nand_probe_t *end_of_spi_nand_probe_func;


SECTION_RECYCLE int
init_spi_nand_probe_both_list(void) {
    int ret = 0;
    if (nsu_probe(&plr_spi_nand_flash_info, &start_of_spi_nand_probe_func, &end_of_spi_nand_probe_func)==0) return 0;
    
    const symbol_table_entry_t *s;  
    if ((s=symb_retrive_lplr(SPI_NAND_DEVICE_PROB_LIST_START))==VZERO) return -1;
    const spi_nand_probe_t **lplr_probe_func_start=s->v.pvalue;
    
    if ((s=symb_retrive_lplr(SPI_NAND_DEVICE_PROB_LIST_END))==VZERO) return -1;
    const spi_nand_probe_t **lplr_probe_func_end=s->v.pvalue;
    
    ret =  nsu_probe(&plr_spi_nand_flash_info, lplr_probe_func_start, lplr_probe_func_end);

    return ret;
}

SECTION_RECYCLE void
init_spi_nand(void) {
    if (init_spi_nand_probe_both_list()<0) {
        puts(_nsu_probe_fail_msg);
        while(1);
    }

    // basic spi-nand init and establish the plr_mapping_table
    if(nsu_init()<0){
        puts(_nsu_table_init_fail_msg);
        while(1);
    }
}
REG_INIT_FUNC(init_spi_nand, 3);

#ifdef MMU_CHECK_INETRNAL_ERROR
SECTION_TEXT UTIL_FAR void
show_second_banner(void)  {
    inline_puts("    .text and .ro sections work!\n");
}
#endif

SECTION_RECYCLE void
_init_tlb(void) {
    // 1. clean up mmu driver
    u32_t end_of_mapping = OTTO_SRAM_START + OTTO_SRAM_SIZE;
    if (mmu_drv_init((u32_t)&mapped_physical_sram_start, end_of_mapping)==0) {
        puts(_mmu_init_fail_msg);
        while(1);
    }
    
    // 2. enable TLB and isr
    #if 0 // TODO: remove
    u32_t tr;
    __asm__ __volatile__ (
        "mfc0 %0, " TO_STR(CP0_STATUS) ASM_NL
        "ori %0, %0, 1" ASM_NL
        "mtc0 %0, " TO_STR(CP0_STATUS) ASM_NL
        : "=r" (tr)
    );
    #endif    
}

SECTION_UNS_TEXT void 
init_tlb(void) {
    _init_tlb();
    // 3. show message
    puts("II: TLB initial done:\n    .ro section works!\n");
    #ifdef MMU_CHECK_INETRNAL_ERROR
        show_second_banner();
    #endif
}

REG_INIT_FUNC(init_tlb, 10);


// --------------------------
//      export symbols
// --------------------------
