#ifndef RTK_SPI_NAND_H
#define RTK_SPI_NAND_H




#define Strtoul simple_strtoul


#define CONF_SPI_NAND_UBOOT_COMMAND 1

/***********************************************
  * SPI_NAND's opcode
  ***********************************************/
#define SNAND_RESET_OP         (0xFF)
#define SNAND_SET_FEATURE_OP   (0x1F)
#define SNAND_BLOCK_ERASE_OP   (0xD8)
#define SNAND_RDID_OP          (0x9F)
#define SNAND_GET_FEATURE_OP   (0x0F)
#define SNAND_NORMAL_READ      (0x03)
#define SNAND_FAST_READ        (0x0B)
#define SNAND_FAST_READ_X2     (0x3B)
#define SNAND_FAST_READ_X4     (0x6B)
#define SNAND_FAST_READ_DIO    (0xBB)
#define SNAND_FAST_READ_QIO    (0xEB)
#define SNAND_PROGRAM_LOAD     (0x02)
#define SNAND_PROGRAM_LOAD_X4  (0x32)
#define SNAND_WRITE_OP    (snand_cmd_info.w_cmd)
#define SNAND_READ_OP     (snand_cmd_info.r_cmd)


/***********************************************
  * SPI_NAND's driver function
  ***********************************************/
#define SNAND_RESET_FUNC           (snffcn_pio_raw_cmd)
#define SNAND_SET_FEATURE_FUNC     (snffcn_pio_raw_cmd)  /*quad_enable(),  snand_wait_spi_nand_ready(), snand_wait_spi_nand_ready()*/
#define SNAND_BLOCK_ERASE_FUNC     (snffcn_pio_raw_cmd) /*block_erase()*/
#define SNAND_RDID_FUNC            (snffcn_pio_raw_cmd)
#define SNAND_GET_FEATURE_FUNC     (snffcn_pio_raw_cmd)
#define SNAND_WRITE_FUNC           (snffcn_pio_write_data)
#define SNAND_READ_FUNC            (snffcn_pio_read_data)


/***********************************************
  *  SPI_NAND's spec definition
  ***********************************************/
#define PAGE_SIZE_2K			(0x800)
#define PAGE_SIZE_4K			(0x1000)
#define PAGE_SIZE_2K_MASK		(0x7FF)
#define PAGE_SIZE_4K_MASK		(0xFFF)
#define PAGE_SPARE_SIZE_2K		(0x840)
#define PAGE_SPARE_SIZE_4K		(0x1100)


#define SNAND_RDID_W25N01GV		(0xEFAA21)
#define SNAND_RDID_W25N01GV_B		(0xEFAB21)

#define SNAND_MANUFACTURER_ID		(0xEF)
#define SNAND_DEVICE_ID_W25N01GV	(0xAA21)

#define MXIC_RDID_M35LF1GE4AB		(0xc212c2)

#define ETRON_RDID_EM73E044SNA		(0xd503d5)

/********************************************
  Export functions
  ********************************************/
//plr_nand_spi_info_t *probe_snand_chip(void);
int snand_block_erase(unsigned int blk_pge_addr);
void snand_wait_spi_nand_ready(void);
void snand_pio_read_data(void *ram_addr, unsigned int wr_bytes, unsigned int blk_pge_addr, unsigned int col_addr);
void snand_pio_write_data(void *ram_addr, unsigned int wr_bytes, unsigned int blk_pge_addr, unsigned int col_addr);
void snand_dma_read_data(void *dma_addr, unsigned int dma_len, unsigned int blk_pge_addr); //Less or More than 2112/4352 Bytes
void snand_dma_write_data(void *dma_addr, unsigned int dma_len, unsigned int blk_pge_addr); //Les  or More than 2112/4352 Bytes
void snand_page_read(void *dma_addr, unsigned int blk_pge_addr); //Only 2112/4352 Bytes
void snand_page_write(void *dma_addr, unsigned int blk_pge_addr); //Only 2112/4352 Bytes
void snand_page_write_with_ecc_encode(void * dma_addr,unsigned int blk_pge_addr, void *p_eccbuf);
int snand_page_read_with_ecc_decode(void * dma_addr,unsigned int blk_pge_addr, void *p_eccbuf);

#endif //#ifdef RTK_SPI_NAND_H

