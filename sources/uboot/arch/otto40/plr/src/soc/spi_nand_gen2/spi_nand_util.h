#ifndef SPI_NAND_UTIL_H
#define SPI_NAND_UTIL_H
#include <soc.h>

#ifdef NSU_USING_SYMBOL_TABLE_FUNCTION
    #ifdef NSU_DRIVER_IN_ROM
        #error 'lplr should not use symbol_table function'
    #endif
    #include "spi_nand_symb_func.h"
#else
    #include "spi_nand_common.h"
    #define nsu_reset_spi_nand_chip             nsc_reset_spi_nand_chip
    #define nsu_disable_on_die_ecc              nsc_disable_on_die_ecc
    #define nsu_enable_on_die_ecc               nsc_enable_on_die_ecc
    #define nsu_block_unprotect                 nsc_block_unprotect
    #define nsu_read_spi_nand_id(cs, m, w, r)   nsc_read_spi_nand_id(cs, m, w, r)
    #define nsu_get_feature_reg(cs, f)          nsc_get_feature_register(cs, f)
    #define nsu_set_feature_reg(cs, f, s)       nsc_set_feature_register(cs, f, s)
#endif

extern u32_t otto_plr_page_buf_addr;
#define page_buffer ((void*)(otto_plr_page_buf_addr))
#define oob_buffer   ((oob_t*)(OTTO_PLR_OOB_BUFFER))
#define ecc_buffer   ((void*)(OTTO_PLR_ECC_BUFFER))

typedef plr_oob_t oob_t;


int nsu_init(void);
int nsu_logical_page_read(void *data, u32_t page_num);
int nsu_probe(spi_nand_flash_info_t *info, const spi_nand_probe_t **, const spi_nand_probe_t **);

#endif //SPI_NAND_UTIL_H


