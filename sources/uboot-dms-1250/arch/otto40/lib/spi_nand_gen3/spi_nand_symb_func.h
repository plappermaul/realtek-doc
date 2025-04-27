#ifndef SPI_NAND_SYMB_FUNC_H
#define SPI_NAND_SYMB_FUNC_H

#include <spi_nand/spi_nand_struct.h>
extern spi_nand_page_read_write_ecc_t  *_nsu_page_read_with_ode_ptr;
extern spi_nand_page_read_write_ecc_t  *_nsu_page_write_with_ode_ptr;
extern spi_nand_send_instr_gen_t       *_nsu_send_instr_gen_ptr;

extern fpv_u32_t                       *_nsu_oip_ready_ptr;
extern fpv_u32_t                       *_nsu_reset_ptr;
extern spi_nand_set_feature_reg_t      *_nsu_set_feature_ptr;
extern spi_nand_get_feature_reg_t      *_nsu_get_feature_ptr;
extern spi_nand_read_id_t              *_nsu_read_id_ptr;
extern fpv_u32_t                       *_nsu_dis_on_die_ecc_ptr;
extern fpv_u32_t                       *_nsu_en_on_die_ecc_ptr;
extern fpv_u32_t                       *_nsu_blk_unprotect_ptr;
extern spi_nand_stack2die_addr_trans_t *_nsu_s2d_addr_trans_ptr;

extern ecc_encode_t  *_nsu_ecc_encode_ptr;
extern ecc_decode_t  *_nsu_ecc_decode_ptr;
extern ecc_engine_t  *_nsu_ecc_engine_action_ptr;
extern ecc_encode_t  *_nsu_ecc_encode_4Kpage_ptr;
extern ecc_decode_t  *_nsu_ecc_decode_4Kpage_ptr;

extern spi_nand_cmd_info_t    *_nsu_cmd_info_ptr;
extern spi_nand_cmd_info_t    *_nsu_x2_cmd_info_ptr;
extern spi_nand_cmd_info_t    *_nsu_dio_cmd_info_ptr;
extern spi_nand_model_info_t  nsu_model_info;
extern spi_nand_flash_info_t  plr_spi_nand_flash_info;
extern spi_nand_cmd_info_t    plr_cmd_info;
extern spi_nand_model_info_t  plr_model_info;


#define nsu_pio_write(info, addr, len, blk_pge_addr, col_addr)     _spi_nand_info->_model_info->_pio_write(info, addr, len, blk_pge_addr, col_addr)
#define nsu_pio_read(info, addr, len, blk_pge_addr, col_addr)      _spi_nand_info->_model_info->_pio_read(info, addr, len, blk_pge_addr, col_addr)
#define nsu_page_write(info, dma_addr, blk_pge_addr)               _spi_nand_info->_model_info->_page_write(info, dma_addr, blk_pge_addr)
#define nsu_page_read(info, dma_addr, blk_pge_addr)                _spi_nand_info->_model_info->_page_read(info, dma_addr, blk_pge_addr)
#define nsu_page_write_ecc(info, dma_addr, blk_pge_addr, eccbuf)   _spi_nand_info->_model_info->_page_write_ecc(info, dma_addr, blk_pge_addr, eccbuf)
#define nsu_page_read_ecc(info, dma_addr, blk_pge_addr, eccbuf)    _spi_nand_info->_model_info->_page_read_ecc(info, dma_addr, blk_pge_addr, eccbuf)
#define nsu_send_instr_gen(cs, opcode, settings, io_len)           (*_nsu_send_instr_gen_ptr)(cs, opcode, settings, io_len)

#define nsu_block_erase(info, blk_pge_addr)                        _spi_nand_info->_model_info->_block_erase(info, blk_pge_addr)
#define nsu_wait_spi_nand_rdy()                                    _spi_nand_info->_model_info->_wait_spi_nand_ready()
#define nsu_reset_spi_nand_chip(cs)                                (*_nsu_reset_ptr)(cs)
#define nsu_set_feature_reg(cs, feature_addr, setting)             (*_nsu_set_feature_ptr)(cs, feature_addr, setting)
#define nsu_get_feature_reg(cs, feature_addr)                      (*_nsu_get_feature_ptr)(cs, feature_addr)
#define nsu_read_spi_nand_id(cs, man_addr, w_io_len, r_io_len)     (*_nsu_read_id_ptr)(cs, man_addr, w_io_len, r_io_len)
#define nsu_disable_on_die_ecc(cs)                                 (*_nsu_dis_on_die_ecc_ptr)(cs)
#define nsu_enable_on_die_ecc(cs)                                  (*_nsu_en_on_die_ecc_ptr)(cs)
#define nsu_block_unprotect(cs)                                    (*_nsu_blk_unprotect_ptr)(cs)
#define nsu_s2d_addr_trans(info, blk_pge_addr, sel_die_func)       (*_nsu_s2d_addr_trans_ptr)(info, blk_pge_addr, sel_die_func)

#define nsu_ecc_encode(ecc_ability, dma_addr, eccbuf)              _spi_nand_info->_ecc_encode(ecc_ability, dma_addr, eccbuf)
#define nsu_ecc_decode(ecc_ability, dma_addr, eccbuf)              _spi_nand_info->_ecc_decode(ecc_ability, dma_addr, eccbuf)
#define nsu_ecc_engine_action(ecc_ability, dma_addr, eccbuf, is_encode)    (*_nsu_ecc_engine_action_ptr)(ecc_ability, dma_addr, eccbuf, is_encode)
#define nsu_ecc_encode_4Kpage(ecc_ability, dma_addr, eccbuf)       (*_nsu_ecc_encode_4Kpage_ptr)(ecc_ability, dma_addr, eccbuf)
#define nsu_ecc_decode_4Kpage(ecc_ability, dma_addr, eccbuf)       (*_nsu_ecc_decode_4Kpage_ptr)(ecc_ability, dma_addr, eccbuf)
#endif  //SPI_NAND_SYMB_FUNC_H


