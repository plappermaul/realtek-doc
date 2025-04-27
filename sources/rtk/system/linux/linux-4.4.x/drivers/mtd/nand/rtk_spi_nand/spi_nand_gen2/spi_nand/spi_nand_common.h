#ifndef SPI_NAND_COMMON_H
#define SPI_NAND_COMMON_H

#include <spi_nand/spi_nand_struct.h>


/***********************************************
  *  Common SPI NAND Command OP Code
  ***********************************************/
#define NORMAL_READ_OP      (0x03)
#define FAST_READ_SIO_OP    (0x0B)
#define FAST_READ_DIO_OP    (0xBB)
#define FAST_READ_QIO_OP    (0xEB)
#define FAST_READ_X2_OP     (0x3B)
#define FAST_READ_X4_OP     (0x6B)

#define PROGRAM_LOAD_OP     (0x02)
#define PROGRAM_LOAD_X4_OP  (0x32)

#define RESET_OP            (0xFF)
#define RDID_OP             (0x9F)
#define WRITE_ENABLE_OP     (0x06)
#define BLOCK_ERASE_OP      (0xD8)
#define PROGRAM_EXECUTE_OP  (0x10)
#define PAGE_DATA_READ_OP   (0x13)
#define SET_FEATURE_OP      (0x1F)
#define GET_FEATURE_OP      (0x0F)


#define DEFAULT_DATA_BASE (0xDEFA)


/***********************************************
  *  Common Commands OP Code
  ***********************************************/
extern spi_nand_cmd_info_t nsc_sio_cmd_info;
extern spi_nand_cmd_info_t nsc_dio_cmd_info;
extern spi_nand_cmd_info_t nsc_x2_cmd_info;

void nsc_reset_spi_nand_chip(u32_t cs);
void nsc_disable_on_die_ecc(u32_t cs);
void nsc_enable_on_die_ecc(u32_t cs);
void nsc_block_unprotect(u32_t cs);
s32_t nsc_block_erase(spi_nand_flash_info_t *info, u32_t blk_pge_addr);
void nsc_write_enable(u32_t cs);
void nsc_program_execute(spi_nand_flash_info_t *info, u32_t cs, u32_t blk_pge_addr);
void nsc_page_data_read_to_cache_buf(spi_nand_flash_info_t *info, u32_t cs, u32_t blk_pge_addr);
void nsc_wait_spi_nand_oip_ready(u32_t cs);
void nsc_set_feature_register(u32_t cs, u32_t feature_addr, u32_t setting);
u32_t nsc_get_feature_register(u32_t cs, u32_t feature_addr);
u32_t nsc_read_spi_nand_id(u32_t cs, u32_t man_addr, u32_t w_io_len, u32_t r_io_len);
s32_t nsc_check_program_status(u32_t cs);
#endif //SPI_NAND_COMMON_H

