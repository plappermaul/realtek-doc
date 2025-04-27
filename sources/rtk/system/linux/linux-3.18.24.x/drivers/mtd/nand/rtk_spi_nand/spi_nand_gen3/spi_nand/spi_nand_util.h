#ifndef SPI_NAND_UTIL_H
#define SPI_NAND_UTIL_H
#include <soc.h>

#ifdef NSU_USING_SYMBOL_TABLE_FUNCTION
    #ifdef NSU_DRIVER_IN_ROM
        #error 'lplr should not use symbol_table function'
    #endif
    #include "spi_nand_symb_func.h"
#else
    #if !defined(NSU_DRIVER_IN_ROM) && !defined(CONFIG_UNDER_UBOOT) && !defined(__LUNA_KERNEL__)
        #error 'C says: checkout common driver enviroment'
    #endif
    #include "spi_nand_common.h"
    #define nsu_reset_spi_nand_chip          nsc_reset_spi_nand_chip
    #define nsu_disable_on_die_ecc           nsc_disable_on_die_ecc
    #define nsu_enable_on_die_ecc            nsc_enable_on_die_ecc
    #define nsu_block_unprotect              nsc_block_unprotect
    #define nsu_block_erase                  nsc_block_erase
    #define nsu_read_spi_nand_id             nsc_read_spi_nand_id
    #define nsu_get_feature_reg              nsc_get_feature_register
    #define nsu_set_feature_reg              nsc_set_feature_register
    #define nsu_s2d_addr_trans               nsc_stack2die_addr_transform
    #define nsu_send_instr_gen               snaf_send_instruction_genernal
    #define nsu_page_read                    snaf_page_read
    #define nsu_page_write                   snaf_page_write
    #define nsu_page_read_with_ecc_decode    snaf_page_read_with_ecc_decode
    #define nsu_page_write_with_ecc_encode   snaf_page_write_with_ecc_encode
    #define nsu_snaf_pio_read_data           snaf_pio_read_data
    #define nsu_snaf_pio_write_data          snaf_pio_write_data
#endif

extern u32_t otto_plr_page_buf_addr;
#define page_buffer ((void*)(otto_plr_page_buf_addr))
#define oob_buffer   ((oob_t*)(OTTO_PLR_OOB_BUFFER))
#define ecc_buffer   ((void*)(OTTO_PLR_ECC_BUFFER))

typedef plr_oob_t oob_t;
typedef plr_oob_4kpage_t oob_4kpage_t;


int nsu_init(void);
int nsu_logical_page_read(void *data, u32_t page_num);
int nsu_probe(spi_nand_flash_info_t *info, const spi_nand_probe_t **, const spi_nand_probe_t **);

#endif //SPI_NAND_UTIL_H


