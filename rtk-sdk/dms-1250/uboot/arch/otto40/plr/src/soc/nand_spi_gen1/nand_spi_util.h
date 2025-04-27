#ifndef NAND_SPI_UTIL_H
#define NAND_SPI_UTIL_H
#include <soc.h>

#ifdef NSU_USING_SYMBOL_TABLE_FUNCTION
    #ifdef NSU_DRIVER_IN_ROM
        #error 'lplr should not use symbol_table function'
    #endif
    #include "nand_spi_symb_func.h"
#else
    #include "nand_spi_common.h"
    #define nsu_set_feature_reg nsc_set_feature_register
    #define nsu_reset_spi_nand_chip nsc_reset_nand_spi_chip
    #define nsu_read_spi_nand_id nsc_read_spi_nand_id
    #define nsu_get_feature_reg nsc_get_feature_register
    #define nsu_set_feature_reg nsc_set_feature_register
#endif

#define page_buffer ((void*)(OTTO_PLR_PAGE_BUFFER))
#define oob_buffer   ((oob_t*)(OTTO_PLR_OOB_BUFFER))
#define ecc_buffer   ((void*)(OTTO_PLR_ECC_BUFFER))

typedef plr_oob_t oob_t;


int nsu_init(void);
int nsu_logical_page_read(void *data, u32_t page_num);
int nsu_probe(nand_spi_flash_info_t *info, const nand_spi_probe_t **, const nand_spi_probe_t **);

#endif


