#ifndef OTTO_SPI_NAND_FLASH_H
#define OTTO_SPI_NAND_FLASH_H
#include <spi_nand/spi_nand_blr_util.h>

typedef struct otto_spi_nand_info_s{
    uint32_t id;
    uint32_t cs_count;
    uint32_t chip_size;     // unit: byte
    uint32_t block_size;    // unit: byte
    uint32_t page_size;
    uint32_t oob_size;
    uint32_t spare_size;
    uint32_t block_count;    
    uint32_t page_count;
    spi_nand_flash_info_t *flash_info;
}otto_spi_nand_info_t;

#endif
