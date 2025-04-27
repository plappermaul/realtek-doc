#ifndef NAND_SPI_MICRON_H
#define NAND_SPI_MICRON_H


/***********************************************
  *  MICRON's spec definition
  ***********************************************/
#define MID_MICRON      (0x2C)
#define DID_M78A        (0x14)


/********************************************
  Export Functions
  ********************************************/
nand_spi_flash_info_t *probe_micron_spi_nand_chip(void);
void micron_page_read(nand_spi_flash_info_t *info, void *dma_addr, u32_t blk_pge_addr);
s32_t micron_page_write(nand_spi_flash_info_t *info, void *dma_addr, u32_t blk_pge_addr);
s32_t micron_page_write_with_ecc_encode(nand_spi_flash_info_t *info, void *dma_addr, u32_t blk_pge_addr, void *p_eccbuf);
s32_t micron_page_read_with_ecc_decode(nand_spi_flash_info_t *info, void *dma_addr, u32_t blk_pge_addr, void *p_eccbuf);
s32_t micron_ondie_ecc_decode_status(nand_spi_flash_info_t *, void *dma_addr, void *p_eccbuf);
#endif //#ifndef NAND_SPI_MICRON_H

