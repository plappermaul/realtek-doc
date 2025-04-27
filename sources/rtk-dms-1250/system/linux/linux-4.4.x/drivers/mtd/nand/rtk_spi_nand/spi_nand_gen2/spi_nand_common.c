#include <common/rt_type.h>
#include <util.h>
#include <spi_nand/spi_nand_ctrl.h>
#include <spi_nand/spi_nand_common.h>

SECTION_SPI_NAND_DATA
u32_t snaf_drv_ver  = SYM_SPI_NAND_VER;    // magic_1 + magic_2 + ver + drv_ver

SECTION_SPI_NAND_DATA
spi_nand_cmd_info_t nsc_sio_cmd_info = {
    .w_cmd = PROGRAM_LOAD_OP,
    .w_addr_io = SIO_WIDTH,
    .w_data_io = SIO_WIDTH,
    .r_cmd = NORMAL_READ_OP,
    .r_addr_io = SIO_WIDTH,
    .r_data_io = SIO_WIDTH,
    .r_dummy_cycles  = 8,
};

SECTION_SPI_NAND_DATA
spi_nand_cmd_info_t nsc_dio_cmd_info = {
    .w_cmd = PROGRAM_LOAD_OP,
    .w_addr_io = SIO_WIDTH,
    .w_data_io = SIO_WIDTH,
    .r_cmd = FAST_READ_DIO_OP,
    .r_addr_io = DIO_WIDTH,
    .r_data_io = DIO_WIDTH,
    .r_dummy_cycles  = 8,
};

SECTION_SPI_NAND_DATA
spi_nand_cmd_info_t nsc_x2_cmd_info = {
    .w_cmd = PROGRAM_LOAD_OP,
    .w_addr_io = SIO_WIDTH,
    .w_data_io = SIO_WIDTH,
    .r_cmd = FAST_READ_X2_OP,
    .r_addr_io = SIO_WIDTH,
    .r_data_io = DIO_WIDTH,
    .r_dummy_cycles  = 8,
};

SECTION_SPI_NAND void
nsc_write_enable(u32_t cs)
{
    u32_t w_io_len = IO_WIDTH_LEN(SIO_WIDTH,CMR_LEN(1));
    _pio_raw_cmd(WRITE_ENABLE_OP|_CS_INFO(cs), SNAF_DONT_CARE, w_io_len,SNAF_DONT_CARE);
}

SECTION_SPI_NAND void
nsc_program_execute(spi_nand_flash_info_t *info, u32_t cs, u32_t blk_pge_addr)
{
    /* 1-BYTE CMD + 1-BYTE Dummy + 2-BYTE Address */
    u32_t w_io_len = IO_WIDTH_LEN(SIO_WIDTH,CMR_LEN(4));
    _pio_raw_cmd(PROGRAM_EXECUTE_OP|_CS_INFO(cs), blk_pge_addr, w_io_len, SNAF_DONT_CARE);
    info->_model_info->_wait_spi_nand_ready(cs);
}

SECTION_SPI_NAND void
nsc_page_data_read_to_cache_buf(spi_nand_flash_info_t *info, u32_t cs, u32_t blk_pge_addr)
{
    /* 1-BYTE CMD + 1-BYTE Dummy + 2-BYTE Address */
    u32_t w_io_len = IO_WIDTH_LEN(SIO_WIDTH,CMR_LEN(4));
    _pio_raw_cmd(PAGE_DATA_READ_OP|_CS_INFO(cs), blk_pge_addr, w_io_len, SNAF_DONT_CARE);
    info->_model_info->_wait_spi_nand_ready(cs);
}

SECTION_SPI_NAND void
nsc_reset_spi_nand_chip(u32_t cs)
{
    u32_t w_io_len = IO_WIDTH_LEN(SIO_WIDTH,CMR_LEN(1));
    _pio_raw_cmd(RESET_OP|_CS_INFO(cs), SNAF_DONT_CARE, w_io_len, SNAF_DONT_CARE);
    otto_lx_timer_udelay(1000);
    nsc_wait_spi_nand_oip_ready(cs);
}

SECTION_SPI_NAND s32_t
nsc_check_program_status(u32_t cs)
{
    u32_t pfail = ((nsc_get_feature_register(cs, 0xC0)>>3)&0x1);
    if(pfail) return -1;
    return 0;
}

SECTION_SPI_NAND static s32_t
nsc_check_erase_status(u32_t cs)
{
    u32_t efail = ((nsc_get_feature_register(cs, 0xC0)>>2)&0x1);
    if(efail) return -1;
    return 0;
}

SECTION_SPI_NAND s32_t
nsc_block_erase(spi_nand_flash_info_t *info, u32_t blk_pge_addr)
{
    u32_t cs_shift = PGE_BLK_SHF(info->_num_block);
    u32_t cs=_EXTRACT_CS_INFO(blk_pge_addr, cs_shift);
    blk_pge_addr=_REMVOE_CS_INFO(blk_pge_addr, cs_shift);

    nsc_write_enable(cs);

    /* 1-BYTE CMD + 1-BYTE Dummy + 2-BYTE Address */
    u32_t w_io_len = IO_WIDTH_LEN(SIO_WIDTH,CMR_LEN(4));
    _pio_raw_cmd(BLOCK_ERASE_OP|_CS_INFO(cs), blk_pge_addr, w_io_len, SNAF_DONT_CARE);
    info->_model_info->_wait_spi_nand_ready(cs);

    /* Check Erase Status*/
    return nsc_check_erase_status(cs);
}

SECTION_SPI_NAND u32_t
nsc_read_spi_nand_id(u32_t cs, u32_t man_addr, u32_t w_io_len, u32_t r_io_len)
{
    return _pio_raw_cmd(RDID_OP|_CS_INFO(cs), man_addr, w_io_len, r_io_len);     // READ CSn
}

SECTION_SPI_NAND u32_t
nsc_get_feature_register(u32_t cs, u32_t feature_addr)
{
    u32_t w_io_len = IO_WIDTH_LEN(SIO_WIDTH,CMR_LEN(2));
    u32_t r_io_len = IO_WIDTH_LEN(SIO_WIDTH,CMR_LEN(1));
    u32_t ret = _pio_raw_cmd(GET_FEATURE_OP|_CS_INFO(cs), feature_addr, w_io_len, r_io_len);
    return ((ret>>24)&0xFF);
}

SECTION_SPI_NAND void
nsc_set_feature_register(u32_t cs, u32_t feature_addr, u32_t setting)
{
    u32_t w_io_len = IO_WIDTH_LEN(SIO_WIDTH,CMR_LEN(3));
    _pio_raw_cmd(SET_FEATURE_OP|_CS_INFO(cs), ((feature_addr<<8)|setting), w_io_len, SNAF_DONT_CARE);
}

SECTION_SPI_NAND void
nsc_wait_spi_nand_oip_ready(u32_t cs)
{
    u32_t feature_addr = 0xC0;
    u32_t oip = nsc_get_feature_register(cs, feature_addr);

    while((oip&0x1)!=0){
        oip = nsc_get_feature_register(cs, feature_addr);
    }
}

SECTION_SPI_NAND void
nsc_disable_on_die_ecc(u32_t cs)
{
    u32_t feature_addr=0xB0;
    u32_t value = nsc_get_feature_register(cs, feature_addr);
    value &= ~(1<<4);
    nsc_set_feature_register(cs, feature_addr,value);
}

SECTION_SPI_NAND void
nsc_enable_on_die_ecc(u32_t cs)
{
    u32_t feature_addr=0xB0;
    u32_t value = nsc_get_feature_register(cs, feature_addr) | (1<<4);
    nsc_set_feature_register(cs, feature_addr, value);
}

SECTION_SPI_NAND void
nsc_block_unprotect(u32_t cs)
{
    u32_t feature_addr=0xA0;
    u32_t value = 0x00;
    nsc_set_feature_register(cs, feature_addr, value);
}

#ifndef CONFIG_UNDER_UBOOT
symb_pdefine(nsc_sio_cmd_info, SPI_NAND_SIO_CMD_INFO, &nsc_sio_cmd_info);
symb_pdefine(nsc_dio_cmd_info, SPI_NAND_DIO_CMD_INFO, &nsc_dio_cmd_info);
symb_pdefine(nsc_x2_cmd_info, SPI_NAND_X2_CMD_INFO, &nsc_x2_cmd_info);
symb_fdefine(SNAF_READ_SPI_NAND_FUNC, nsc_read_spi_nand_id);
symb_fdefine(SNAF_SET_FEATURE_FUNC, nsc_set_feature_register);
symb_fdefine(SNAF_GET_FEATURE_FUNC, nsc_get_feature_register);
symb_fdefine(SNAF_RESET_SPI_NAND_FUNC, nsc_reset_spi_nand_chip);
symb_fdefine(SNAF_BLOCK_ERASE_FUNC, nsc_block_erase);
symb_fdefine(SNAF_WAIT_NAND_OIP_FUNC, nsc_wait_spi_nand_oip_ready);
symb_fdefine(SNAF_DISABLE_ODE_FUNC, nsc_disable_on_die_ecc);
symb_fdefine(SNAF_ENABLE_ODE_FUNC, nsc_enable_on_die_ecc);
symb_fdefine(SNAF_BLOCK_UNPROTECT_FUNC, nsc_block_unprotect);
symb_pdefine(snaf_drv_ver, SPI_NAND_DRV_VER, &snaf_drv_ver);
#endif	// CONFIG_UNDER_UBOOT
