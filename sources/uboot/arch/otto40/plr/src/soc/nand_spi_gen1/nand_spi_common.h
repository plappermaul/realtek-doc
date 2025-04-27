#ifndef NAND_SPI_COMMON_H
#define NAND_SPI_COMMON_H

/***********************************************
  * Driver Section Definitions
  ***********************************************/
#ifndef SECTION_NAND_SPI
    #define SECTION_NAND_SPI 
#endif
#ifndef SECTION_NAND_SPI_DATA
    #define SECTION_NAND_SPI_DATA 
#endif


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


/***********************************************
  *  Common Commands OP Code
  ***********************************************/
extern nand_spi_cmd_info_t nsc_sio_cmd_info;
extern nand_spi_cmd_info_t nsc_dio_cmd_info;

void nsc_reset_nand_spi_chip(void);
s32_t nsc_block_erase(nand_spi_flash_info_t *info, u32_t blk_pge_addr);
void nsc_write_enable(void);
void nsc_program_execute(nand_spi_flash_info_t *info, u32_t blk_pge_addr);
void nsc_page_data_read_to_cache_buf(nand_spi_flash_info_t *info, u32_t blk_pge_addr);
void nsc_wait_nand_spi_oip_ready(void);
void nsc_set_feature_register(u32_t feature_addr, u32_t setting);
u32_t nsc_get_feature_register(u32_t feature_addr);
u32_t nsc_read_spi_nand_id(u32_t man_addr, u32_t w_io_len, u32_t r_io_len);
s32_t nsc_check_program_status(void);
#endif //#ifndef NAND_SPI_COMMON_H

