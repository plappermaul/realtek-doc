#ifndef NAND_SPI_BLR_UTIL_H
#define NAND_SPI_BLR_UTIL_H

#include "conf.h"
#define _plr_soc_t                  (*(soc_t *)(OTTO_SRAM_START+OTTO_HEADER_OFFSET))
#define _plr_nand_spi_info          _plr_soc_t.flash_info.nand_spi_info


// Preloader Symbol table retrieve definition
#define symb_start(p) p.bios.header.export_symb_list
#define symb_end(p)   p.bios.header.end_of_export_symb_list

#define RETRIEVE_SYMB_TABLE(ptr, symb_key, list, list_end) ({\
    const symbol_table_entry_t *p=list;\
    ptr=VZERO;\
    while(p!=list_end) {\
        if(p->key.id==(u32_t)symb_key){ptr=p->v.pvalue; break;}\
        if(p->key.id==ENDING_SYMB_ID) break;\
        ++p;\
    }\
})


// SPI NAND flash APIs (Using "user define")
#define blr_pio_read(info, addr, len, blk_pge_addr, col_addr)     info->_model_info->_pio_read(info, addr, len, blk_pge_addr, col_addr)
#define blr_pio_write(info, addr, len, blk_pge_addr, col_addr)    info->_model_info->_pio_write(info, addr, len, blk_pge_addr, col_addr)
#define blr_page_read(info, dma_addr, blk_pge_addr)   info->_model_info->_page_read(info, dma_addr, blk_pge_addr)

#define blr_page_read_ecc(info, dma_addr, blk_pge_addr, eccbuf)   info->_model_info->_page_read_ecc(info, dma_addr, blk_pge_addr, eccbuf)
#define blr_page_write_ecc(info, dma_addr, blk_pge_addr, eccbuf)  info->_model_info->_page_write_ecc(info, dma_addr, blk_pge_addr, eccbuf)
#define blr_block_erase(info, blk_pge_addr)                       info->_model_info->_block_erase(info, blk_pge_addr)


// SPI NAND flash APIs (Using "_plr_nand_spi_info")
#define nasu_pio_read(addr, len, blk_pge_addr, col_addr)     blr_pio_read(_plr_nand_spi_info, addr, len, blk_pge_addr, col_addr)
#define nasu_pio_write(addr, len, blk_pge_addr, col_addr)    blr_pio_write(_plr_nand_spi_info, addr, len, blk_pge_addr, col_addr)
#define nasu_page_read_ecc(dma_addr, blk_pge_addr, eccbuf)   blr_page_read_ecc(_plr_nand_spi_info, dma_addr, blk_pge_addr, eccbuf)
#define nasu_page_write_ecc(dma_addr, blk_pge_addr, eccbuf)  blr_page_write_ecc(_plr_nand_spi_info, dma_addr, blk_pge_addr, eccbuf)
#define nasu_block_erase(blk_pge_addr)                       blr_block_erase(_plr_nand_spi_info, blk_pge_addr)
  
// SPI NAND flash block & page address definition
#define BLOCK_ADDR(block_0_1023) (block_0_1023<<6)
#define BLOCK_PAGE_ADDR(block_0_1023, page_0_63) ((0x00<<16)|(block_0_1023<<6)|(page_0_63))

#endif //NAND_SPI_BLR_UTIL_H

