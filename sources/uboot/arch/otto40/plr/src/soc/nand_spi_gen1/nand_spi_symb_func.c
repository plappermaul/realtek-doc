#include <soc.h>
#include <nand_spi/nand_spi_symb_func.h>
#include <plr_sections.h>
#include <symb_define.h>



#ifndef ALWAYS_RETURN_ZERO
    #define _spi_nand_dummy_func ALWAYS_RETURN_ZERO
#else
    s32_t _spi_nand_dummy_func(void) {return 0;}
#endif

// existing only during probe phase
nand_spi_model_info_t  nsu_model_info SECTION_RECYCLE_DATA;
nand_spi_cmd_info_t    *_nsu_cmd_info_ptr SECTION_RECYCLE_DATA;
nand_spi_cmd_info_t    *_nsu_dio_cmd_info_ptr SECTION_SDATA;
nand_spi_read_id_t     *_nsu_read_id_ptr SECTION_RECYCLE_DATA = (nand_spi_read_id_t *)_spi_nand_dummy_func;
nand_spi_void_t        *_nsu_reset_ptr SECTION_RECYCLE_DATA = (nand_spi_void_t *)_spi_nand_dummy_func;
nand_spi_ecc_encode_t  *_nsu_ecc_encode_ptr SECTION_RECYCLE_DATA = (nand_spi_ecc_encode_t *)_spi_nand_dummy_func;
nand_spi_ecc_decode_t  *_nsu_ecc_decode_ptr SECTION_RECYCLE_DATA = (nand_spi_ecc_decode_t *)_spi_nand_dummy_func;
nand_spi_ecc_engine_t  *_nsu_ecc_engine_action_ptr SECTION_RECYCLE_DATA = (nand_spi_ecc_engine_t *)_spi_nand_dummy_func;

// should globally exists
nand_spi_get_feature_reg_t  *_nsu_get_feature_ptr SECTION_SDATA = (nand_spi_get_feature_reg_t *)_spi_nand_dummy_func;
nand_spi_set_feature_reg_t  *_nsu_set_feature_ptr SECTION_SDATA = (nand_spi_set_feature_reg_t *)_spi_nand_dummy_func;


symb_retrive_entry_t spi_nand_func_retrive_list[] SECTION_RECYCLE_DATA = {
    {SNAF_PIO_WRITE_FUNC, &(nsu_model_info._pio_write)},
    {SNAF_PIO_READ_FUNC, &(nsu_model_info._pio_read)},
    {SNAF_PAGE_WRITE_FUNC, &nsu_model_info._page_write},
    {SNAF_PAGE_READ_FUNC, &nsu_model_info._page_read},
    {SNAF_PAGE_WRITE_ECC_FUNC, &nsu_model_info._page_write_ecc},
    {SNAF_PAGE_READ_ECC_FUNC, &nsu_model_info._page_read_ecc},
    {SNAF_BLOCK_ERASE_FUNC, &nsu_model_info._block_erase},
    {SNAF_WAIT_NAND_OIP_FUNC, &nsu_model_info._wait_nand_spi_ready},
    {ECC_BCH_ENCODE_FUNC, &_nsu_ecc_encode_ptr},
    {ECC_BCH_DECODE_FUNC, &_nsu_ecc_decode_ptr},
    {SNAF_RESET_SPI_NAND_FUNC, &_nsu_reset_ptr},
    {SNAF_SET_FEATURE_FUNC, &_nsu_set_feature_ptr},
    {SNAF_GET_FEATURE_FUNC, &_nsu_get_feature_ptr},
    {SNAF_READ_SPI_NAND_FUNC, &_nsu_read_id_ptr},
    {ECC_ENGINE_ACTION_FUNC, &_nsu_ecc_engine_action_ptr},
    {NAND_SPI_SIO_CMD_INFO, &_nsu_cmd_info_ptr},
    {NAND_SPI_DIO_CMD_INFO, &_nsu_dio_cmd_info_ptr},
    {ENDING_SYMB_ID, VZERO},
};

SECTION_RECYCLE void 
spi_nand_func_symbol_retrive(void)
{
    symb_retrive_list(spi_nand_func_retrive_list, lplr_symb_list_range);
}

REG_INIT_FUNC(spi_nand_func_symbol_retrive, 2);
  


