#include <util.h>
#include <nand_spi/nand_spi_ctrl.h>
#include <nand_spi/nand_spi_common.h>


SECTION_NAND_SPI_DATA
nand_spi_cmd_info_t nsc_sio_cmd_info = {
    .w_cmd = PROGRAM_LOAD_OP,
    .w_addr_io = SIO_WIDTH,
    .w_data_io = SIO_WIDTH,
    .r_cmd = NORMAL_READ_OP,
    .r_addr_io = SIO_WIDTH,
    .r_data_io = SIO_WIDTH,
    .r_dummy_cycles  = 8,
};

SECTION_NAND_SPI_DATA
nand_spi_cmd_info_t nsc_dio_cmd_info = {
    .w_cmd = PROGRAM_LOAD_OP,
    .w_addr_io = SIO_WIDTH,
    .w_data_io = SIO_WIDTH,
    .r_cmd = FAST_READ_DIO_OP,
    .r_addr_io = DIO_WIDTH,
    .r_data_io = DIO_WIDTH,
    .r_dummy_cycles  = 8,
};

SECTION_NAND_SPI void 
nsc_write_enable(void)
{
    u32_t w_io_len = IO_WIDTH_LEN(SIO_WIDTH,CMR_LEN(1));
    _pio_raw_cmd(WRITE_ENABLE_OP, SNAF_DONT_CARE, w_io_len,SNAF_DONT_CARE);
}

SECTION_NAND_SPI void 
nsc_program_execute(nand_spi_flash_info_t *info, u32_t blk_pge_addr)
{
    /* 1-BYTE CMD + 1-BYTE Dummy + 2-BYTE Address */
    u32_t w_io_len = IO_WIDTH_LEN(SIO_WIDTH,CMR_LEN(4));
    _pio_raw_cmd(PROGRAM_EXECUTE_OP, blk_pge_addr, w_io_len, SNAF_DONT_CARE);
    info->_model_info->_wait_nand_spi_ready();
}

SECTION_NAND_SPI void 
nsc_page_data_read_to_cache_buf(nand_spi_flash_info_t *info, u32_t blk_pge_addr)
{
    /* 1-BYTE CMD + 1-BYTE Dummy + 2-BYTE Address */
    u32_t w_io_len = IO_WIDTH_LEN(SIO_WIDTH,CMR_LEN(4));
    _pio_raw_cmd(PAGE_DATA_READ_OP, blk_pge_addr, w_io_len, SNAF_DONT_CARE);
    info->_model_info->_wait_nand_spi_ready();
}

SECTION_NAND_SPI void 
nsc_reset_nand_spi_chip(void)
{
    u32_t w_io_len = IO_WIDTH_LEN(SIO_WIDTH,CMR_LEN(1));
    _pio_raw_cmd(RESET_OP, SNAF_DONT_CARE, w_io_len, SNAF_DONT_CARE);
}

SECTION_NAND_SPI s32_t 
nsc_check_program_status(void)
{
    u32_t pfail = ((nsc_get_feature_register(0xC0)>>3)&0x1);
    if(pfail) return -1;
    return 0;
}

SECTION_NAND_SPI static s32_t 
nsc_check_erase_status(void)
{
    u32_t efail = ((nsc_get_feature_register(0xC0)>>2)&0x1);
    if(efail) return -1;
    return 0;
}

SECTION_NAND_SPI s32_t 
nsc_block_erase(nand_spi_flash_info_t *info, u32_t blk_pge_addr)
{
    nsc_write_enable();

    /* 1-BYTE CMD + 1-BYTE Dummy + 2-BYTE Address */
    u32_t w_io_len = IO_WIDTH_LEN(SIO_WIDTH,CMR_LEN(4));
    _pio_raw_cmd(BLOCK_ERASE_OP, blk_pge_addr, w_io_len, SNAF_DONT_CARE);
    info->_model_info->_wait_nand_spi_ready();

    /* Check Erase Status*/ 
    return nsc_check_erase_status();
}

SECTION_NAND_SPI u32_t 
nsc_read_spi_nand_id(u32_t man_addr, u32_t w_io_len, u32_t r_io_len)
{    
    return _pio_raw_cmd(RDID_OP, man_addr, w_io_len, r_io_len);
}

SECTION_NAND_SPI u32_t 
nsc_get_feature_register(u32_t feature_addr)
{
    u32_t w_io_len = IO_WIDTH_LEN(SIO_WIDTH,CMR_LEN(2));
    u32_t r_io_len = IO_WIDTH_LEN(SIO_WIDTH,CMR_LEN(1));
    u32_t ret = _pio_raw_cmd(GET_FEATURE_OP, feature_addr, w_io_len, r_io_len);
    return ((ret>>24)&0xFF);
}

SECTION_NAND_SPI void 
nsc_set_feature_register(u32_t feature_addr, u32_t setting)
{
    u32_t w_io_len = IO_WIDTH_LEN(SIO_WIDTH,CMR_LEN(3));
    _pio_raw_cmd(SET_FEATURE_OP, ((feature_addr<<8)|setting), w_io_len, SNAF_DONT_CARE);
}

SECTION_NAND_SPI void 
nsc_wait_nand_spi_oip_ready(void)
{
    u32_t feature_addr = 0xC0;
    u32_t oip = nsc_get_feature_register(feature_addr);

    while((oip&0x1)!=0){
        oip = nsc_get_feature_register(feature_addr);
    }
}

symb_pdefine(nsc_sio_cmd_info, NAND_SPI_SIO_CMD_INFO, &nsc_sio_cmd_info);
symb_pdefine(nsc_sio_cmd_info, NAND_SPI_DIO_CMD_INFO, &nsc_dio_cmd_info);
symb_fdefine(SNAF_SET_FEATURE_FUNC, nsc_set_feature_register);
symb_fdefine(SNAF_GET_FEATURE_FUNC, nsc_get_feature_register);
symb_fdefine(SNAF_RESET_SPI_NAND_FUNC, nsc_reset_nand_spi_chip);
symb_fdefine(SNAF_BLOCK_ERASE_FUNC, nsc_block_erase);
symb_fdefine(SNAF_WAIT_NAND_OIP_FUNC, nsc_wait_nand_spi_oip_ready);
symb_fdefine(SNAF_READ_SPI_NAND_FUNC, nsc_read_spi_nand_id);

