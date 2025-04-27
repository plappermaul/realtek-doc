#ifndef NAND_SPI_SYMB_FUNC_H
#define NAND_SPI_SYMB_FUNC_H

extern nand_spi_void_t        *_nsu_reset_ptr;
extern nand_spi_get_feature_reg_t      *_nsu_get_feature_ptr;
extern nand_spi_set_feature_reg_t      *_nsu_set_feature_ptr;
extern nand_spi_read_id_t              *_nsu_read_id_ptr;

extern nand_spi_ecc_encode_t  *_nsu_ecc_encode_ptr;
extern nand_spi_ecc_decode_t  *_nsu_ecc_decode_ptr;
extern nand_spi_ecc_engine_t  *_nsu_ecc_engine_action_ptr;
extern nand_spi_cmd_info_t    *_nsu_cmd_info_ptr;
extern nand_spi_cmd_info_t    *_nsu_dio_cmd_info_ptr;
extern nand_spi_model_info_t  nsu_model_info;
extern nand_spi_flash_info_t  plr_nand_spi_flash_info;
extern nand_spi_cmd_info_t    plr_cmd_info;
extern nand_spi_model_info_t  plr_model_info;

#define nsu_reset_spi_nand_chip()                             (*_nsu_reset_ptr)()
#define nsu_read_spi_nand_id(man_addr, w_io_len, r_io_len)    (*_nsu_read_id_ptr)(man_addr, w_io_len, r_io_len)
#define nsu_ecc_engine_action(dma_addr, eccbuf, is_bch6, is_encode)    (*_nsu_ecc_engine_action_ptr)(dma_addr, eccbuf, is_bch6, is_encode)

#define nsu_get_feature_reg(feature_addr)                         (*_nsu_get_feature_ptr)(feature_addr)
#define nsu_set_feature_reg(feature_addr, setting)                (*_nsu_set_feature_ptr)(feature_addr, setting)
#define nsu_pio_read(info, addr, len, blk_pge_addr, col_addr)     _nand_spi_info->_model_info->_pio_read(info, addr, len, blk_pge_addr, col_addr)
#define nsu_pio_write(info, addr, len, blk_pge_addr, col_addr)    _nand_spi_info->_model_info->_pio_write(info, addr, len, blk_pge_addr, col_addr)
#define nsu_page_read(info, dma_addr, blk_pge_addr)               _nand_spi_info->_model_info->_page_read(info, dma_addr, blk_pge_addr)
#define nsu_page_read_ecc(info, dma_addr, blk_pge_addr, eccbuf)   _nand_spi_info->_model_info->_page_read_ecc(info, dma_addr, blk_pge_addr, eccbuf)
#define nsu_page_write(info, dma_addr, blk_pge_addr)              _nand_spi_info->_model_info->_page_write(info, dma_addr, blk_pge_addr)
#define nsu_page_write_ecc(info, dma_addr, blk_pge_addr, eccbuf)  _nand_spi_info->_model_info->_page_write_ecc(info, dma_addr, blk_pge_addr, eccbuf)
#define nsu_block_erase(info, blk_pge_addr)                       _nand_spi_info->_model_info->_block_erase(info, blk_pge_addr)
#define nsu_wait_nand_spi_rdy()                                   _nand_spi_info->_model_info->_wait_nand_spi_ready()
#define nsu_ecc_encode(info, dma_addr, eccbuf)                    _nand_spi_info->_ecc_encode(info, dma_addr, eccbuf)
#define nsu_ecc_decode(info, dma_addr, eccbuf)                    _nand_spi_info->_ecc_decode(info, dma_addr, eccbuf)
#endif


