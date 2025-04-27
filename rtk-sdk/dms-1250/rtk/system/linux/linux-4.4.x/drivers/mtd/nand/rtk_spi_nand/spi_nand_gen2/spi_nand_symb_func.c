#include <common/rt_type.h>
#include <soc.h>
#include <spi_nand/spi_nand_symb_func.h>
#include <symb_define.h>

#ifndef ALWAYS_RETURN_ZERO
    #define _spi_nand_dummy_func ALWAYS_RETURN_ZERO
#else
    s32_t _spi_nand_dummy_func(void) {return 0;}
#endif

// existing only during probe phase
u32_t *snaf_drv_ver_ptr SECTION_RECYCLE_DATA;
spi_nand_model_info_t  nsu_model_info SECTION_RECYCLE_DATA;
spi_nand_cmd_info_t    *_nsu_cmd_info_ptr SECTION_RECYCLE_DATA;
spi_nand_cmd_info_t    *_nsu_dio_cmd_info_ptr SECTION_SDATA;
spi_nand_read_id_t     *_nsu_read_id_ptr SECTION_RECYCLE_DATA = (spi_nand_read_id_t *)_spi_nand_dummy_func;
fpv_u32_t              *_nsu_reset_ptr SECTION_RECYCLE_DATA = (fpv_u32_t *)_spi_nand_dummy_func;
fpv_u32_t              *_nsu_en_on_die_ecc_ptr SECTION_RECYCLE_DATA = (fpv_u32_t *)_spi_nand_dummy_func;
fpv_u32_t              *_nsu_dis_on_die_ecc_ptr SECTION_RECYCLE_DATA= (fpv_u32_t *)_spi_nand_dummy_func;
fpv_u32_t              *_nsu_blk_unprotect_ptr SECTION_RECYCLE_DATA   = (fpv_u32_t *)_spi_nand_dummy_func;
ecc_encode_t  *_nsu_ecc_encode_ptr SECTION_RECYCLE_DATA = (ecc_encode_t *)_spi_nand_dummy_func;
ecc_decode_t  *_nsu_ecc_decode_ptr SECTION_RECYCLE_DATA = (ecc_decode_t *)_spi_nand_dummy_func;
ecc_engine_t  *_nsu_ecc_engine_action_ptr SECTION_RECYCLE_DATA = (ecc_engine_t *)_spi_nand_dummy_func;
spi_nand_page_read_write_ecc_t  *_nsu_page_read_with_ode_ptr = (spi_nand_page_read_write_ecc_t *)_spi_nand_dummy_func;
spi_nand_page_read_write_ecc_t  *_nsu_page_write_with_ode_ptr = (spi_nand_page_read_write_ecc_t *)_spi_nand_dummy_func;

// should globally exists
spi_nand_get_feature_reg_t  *_nsu_get_feature_ptr SECTION_SDATA = (spi_nand_get_feature_reg_t *)_spi_nand_dummy_func;
spi_nand_set_feature_reg_t  *_nsu_set_feature_ptr SECTION_SDATA = (spi_nand_set_feature_reg_t *)_spi_nand_dummy_func;


symb_retrive_entry_t spi_nand_func_retrive_list[] SECTION_RECYCLE_DATA = {
    {SNAF_PIO_WRITE_FUNC, &(nsu_model_info._pio_write)},
    {SNAF_PIO_READ_FUNC, &(nsu_model_info._pio_read)},
    {SNAF_PAGE_WRITE_FUNC, &nsu_model_info._page_write},
    {SNAF_PAGE_READ_FUNC, &nsu_model_info._page_read},
    {SNAF_PAGE_WRITE_ECC_FUNC, &nsu_model_info._page_write_ecc},
    {SNAF_PAGE_READ_ODE_FUNC, &_nsu_page_read_with_ode_ptr},
    {SNAF_PAGE_WRITE_ODE_FUNC, &_nsu_page_write_with_ode_ptr},
    {SNAF_PAGE_READ_ECC_FUNC, &nsu_model_info._page_read_ecc},
    {SNAF_BLOCK_ERASE_FUNC, &nsu_model_info._block_erase},
    {SNAF_WAIT_NAND_OIP_FUNC, &nsu_model_info._wait_spi_nand_ready},
    {ECC_BCH_ENCODE_FUNC, &_nsu_ecc_encode_ptr},
    {ECC_BCH_DECODE_FUNC, &_nsu_ecc_decode_ptr},
    {SNAF_RESET_SPI_NAND_FUNC, &_nsu_reset_ptr},
    {SNAF_ENABLE_ODE_FUNC, &_nsu_en_on_die_ecc_ptr},
    {SNAF_DISABLE_ODE_FUNC, &_nsu_dis_on_die_ecc_ptr},
    {SNAF_BLOCK_UNPROTECT_FUNC, &_nsu_blk_unprotect_ptr},
    {SNAF_SET_FEATURE_FUNC, &_nsu_set_feature_ptr},
    {SNAF_GET_FEATURE_FUNC, &_nsu_get_feature_ptr},
    {SNAF_READ_SPI_NAND_FUNC, &_nsu_read_id_ptr},
    {ECC_ENGINE_ACTION_FUNC, &_nsu_ecc_engine_action_ptr},
    {SPI_NAND_SIO_CMD_INFO, &_nsu_cmd_info_ptr},
    {SPI_NAND_X2_CMD_INFO, &_nsu_dio_cmd_info_ptr},
    {SPI_NAND_DRV_VER, &snaf_drv_ver_ptr},
    {ENDING_SYMB_ID, VZERO},
};

char _snaf_ver_fail_msg[] SECTION_RECYCLE_DATA = {"WW: SNAF version check failed!\n"};
char _snaf_ver_ok_msg[] SECTION_RECYCLE_DATA = {"II: SNAF version check OK!\n"};

SECTION_RECYCLE void
spi_nand_func_symbol_retrive(void)
{
    symb_retrive_list(spi_nand_func_retrive_list, lplr_symb_list_range);

    if (!snaf_drv_ver_ptr||(SYM_SPI_NAND_VER!=*snaf_drv_ver_ptr)) {
        puts(_snaf_ver_fail_msg);
        while(1);
    }
    puts(_snaf_ver_ok_msg);
}

REG_INIT_FUNC(spi_nand_func_symbol_retrive, 2);


