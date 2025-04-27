#include <common/rt_type.h>
#include <util.h>
#include <spi_nand/spi_nand_ctrl.h>
#include <spi_nand/spi_nand_common.h>
#include <naf_kernel_util.h>


SECTION_SPI_NAND_DATA
spi_nand_model_info_t snaf_rom_general_model = {
    ._pio_write = snaf_pio_write_data,
    ._pio_read = snaf_pio_read_data,
    ._page_read = snaf_page_read,
    ._page_write = snaf_page_write,
    ._page_read_ecc = snaf_page_read_with_ecc_decode,
    ._page_write_ecc = snaf_page_write_with_ecc_encode,
    ._block_erase = nsc_block_erase,
    ._wait_spi_nand_ready = nsc_wait_spi_nand_oip_ready,
};

SECTION_SPI_NAND_DATA
spi_nand_model_info_t snaf_ode_model = {
    ._pio_write = snaf_pio_write_data,
    ._pio_read = snaf_pio_read_data,
    ._page_read = snaf_page_read,
    ._page_write = snaf_page_write,
    ._page_read_ecc = snaf_page_read_with_ondie_ecc,
    ._page_write_ecc = snaf_page_write_with_ondie_ecc,
    ._block_erase = nsc_block_erase,
    ._wait_spi_nand_ready = nsc_wait_spi_nand_oip_ready,
};

SECTION_SPI_NAND u32_t
_pio_raw_cmd(u32_t cs_opcode, u32_t data, u32_t w_io_len, u32_t r_io_len)
{
    u32_t cs = _EXTRACT_CS_INFO(cs_opcode, _CS_INFO_LOC);
    u32_t opcode = _REMVOE_CS_INFO(cs_opcode, _CS_INFO_LOC);

    ACTIVATE_SPI_NAND_CS(cs);

    /* Step1: Prepare the write command (with data like address or not) */
    u32_t w_data = opcode<<24;
    if(data != SNAF_DONT_CARE){
        u32_t wlen = (w_io_len&0x3);
        if(0 != wlen){
            w_data |= (data<<((3-wlen)*8));
        }
    }

    /* Step2: PIO write command */
    TRIG_CTRLR_PIO_WRITE(w_data, w_io_len);

    /* Step3: Read the data from SPI NAND chip if it exists. */
    u32_t rd_val = 0;
    if (r_io_len != SNAF_DONT_CARE) {
        rd_val = TRIG_CTRLR_PIO_READ(r_io_len);
    }

    DEACTIVE_SPI_NAND_CS(cs);
    return rd_val;
}


SECTION_SPI_NAND static void
_pio_write_data(spi_nand_flash_info_t *info, u32_t cs_col_addr, u32_t wr_bytes, void *wr_buf)
{
    u32_t w_data;
    u32_t w_io_len;
    u32_t cs = _EXTRACT_CS_INFO(cs_col_addr, _CS_INFO_LOC);
    u32_t col_addr = _REMVOE_CS_INFO(cs_col_addr, _CS_INFO_LOC);

    ACTIVATE_SPI_NAND_CS(cs);

    /* Command: SIO, 1-Byte */
    w_data = (SNAF_RW_CMD(info)->w_cmd)<<24;
    w_io_len = IO_WIDTH_LEN(SIO_WIDTH,CMR_LEN(1));
    TRIG_CTRLR_PIO_WRITE(w_data, w_io_len);


    /* Address: addr_io_len, 2-Byte */
    w_data = col_addr<<16;
    w_io_len = IO_WIDTH_LEN(SNAF_RW_CMD(info)->w_addr_io, CMR_LEN(2));
    TRIG_CTRLR_PIO_WRITE(w_data, w_io_len);


    /* Data: data_io_len, wr_len-Byte */
    w_io_len = IO_WIDTH_LEN(SNAF_RW_CMD(info)->w_data_io, CMR_LEN(1));
    union {
        u8_t u8[4];
        u32_t u32;
    } tmp;
    while(wr_bytes > 0){
        /* The last few bytes may be garbage but won't be written to flash, since its length is explicitly given */
        tmp.u8[0] = *((u8_t *)wr_buf++);
        tmp.u8[1] = *((u8_t *)wr_buf++);
        tmp.u8[2] = *((u8_t *)wr_buf++);
        tmp.u8[3] = *((u8_t *)wr_buf++);

        if(wr_bytes < 4){
            TRIG_CTRLR_PIO_WRITE(tmp.u32,(w_io_len | CMR_LEN(wr_bytes)));
            wr_bytes = 0;
        }else{
            TRIG_CTRLR_PIO_WRITE(tmp.u32,(w_io_len | CMR_LEN(4)));
            wr_bytes -= 4;
        }
    }

    DEACTIVE_SPI_NAND_CS(cs);
}

SECTION_SPI_NAND static void
_pio_read_data(spi_nand_flash_info_t *info, u32_t cs_col_addr, u32_t wr_bytes, void *wr_buf)
{
    u32_t w_data;
    u32_t w_io_len;
    u32_t r_io_len;
    u32_t wr_bound;
    u32_t i;
    u32_t addr_io = SNAF_RW_CMD(info)->r_addr_io;
    u32_t dummy_cycles = SNAF_RW_CMD(info)->r_dummy_cycles;
    u32_t dummy_byte=0x00;
    u32_t cs = _EXTRACT_CS_INFO(cs_col_addr, _CS_INFO_LOC);
    u32_t col_addr = _REMVOE_CS_INFO(cs_col_addr, _CS_INFO_LOC);

    ACTIVATE_SPI_NAND_CS(cs);

    /* Command: SIO, 1-Byte */
    w_data = (SNAF_RW_CMD(info)->r_cmd)<<24;
    w_io_len = IO_WIDTH_LEN(SIO_WIDTH, CMR_LEN(1));
    TRIG_CTRLR_PIO_WRITE(w_data, w_io_len);

    /* Address: addr_io_len, 2-Byte Column Address */
    w_data = (col_addr<<16);
    w_io_len = IO_WIDTH_LEN(addr_io, CMR_LEN(2));
    TRIG_CTRLR_PIO_WRITE(w_data, w_io_len);

    /* Dummy Cycles: */
    w_data = dummy_byte<<(32-dummy_cycles);
    w_io_len = IO_WIDTH_LEN(addr_io, CMR_LEN((dummy_cycles/8)));
    TRIG_CTRLR_PIO_WRITE(w_data, w_io_len);

    /* Data: data_io_len, wr_len-Byte */
    r_io_len = IO_WIDTH_LEN(SNAF_RW_CMD(info)->r_data_io, CMR_LEN(1));
    wr_bound = (u32_t)wr_buf + wr_bytes;

    union {
        u8_t u8[4];
        u32_t u32;
    } tmp;
    if (wr_bytes >= 4) {
        while ((u32_t)wr_buf < (wr_bound & 0xFFFFFFFC)) {
            tmp.u32 = TRIG_CTRLR_PIO_READ((r_io_len|CMR_LEN(4)));
            *((u8_t *)wr_buf++) = tmp.u8[0];
            *((u8_t *)wr_buf++) = tmp.u8[1];
            *((u8_t *)wr_buf++) = tmp.u8[2];
            *((u8_t *)wr_buf++) = tmp.u8[3];
        }
    }
    for (i=0; i<(wr_bytes & 0x3); i++) {
        SNAFRCMRrv= r_io_len;
        WAIT_NAND_CTRLR_RDY();
        *((u8_t *)wr_buf++) = RFLD_SNAFRDR(rdata3);
    }

    DEACTIVE_SPI_NAND_CS(cs);
}

SECTION_SPI_NAND void
snaf_pio_read_data(spi_nand_flash_info_t *info, void *ram_addr, u32_t wr_bytes, u32_t blk_pge_addr, u32_t col_addr)
{
    u32_t cs=_EXTRACT_CS_INFO(blk_pge_addr, PGE_BLK_SHF(info->_num_block));
    blk_pge_addr=_REMVOE_CS_INFO(blk_pge_addr, PGE_BLK_SHF(info->_num_block));
    col_addr |= ((blk_pge_addr>>PAGE_SHF)&1)<<(PLANE_SHF_BITS(info->_page_size));

    nsc_page_data_read_to_cache_buf(info, cs, blk_pge_addr);
    _pio_read_data(info, col_addr|_CS_INFO(cs), wr_bytes, ram_addr);
}

SECTION_SPI_NAND s32_t
snaf_pio_write_data(spi_nand_flash_info_t *info, void *ram_addr, u32_t wr_bytes, u32_t blk_pge_addr, u32_t col_addr)
{
    s32_t ret;
    u32_t cs=_EXTRACT_CS_INFO(blk_pge_addr, PGE_BLK_SHF(info->_num_block));
    blk_pge_addr=_REMVOE_CS_INFO(blk_pge_addr, PGE_BLK_SHF(info->_num_block));
    col_addr |= ((blk_pge_addr>>PAGE_SHF)&1)<<(PLANE_SHF_BITS(info->_page_size));

    nsc_write_enable(cs);
    _pio_write_data(info, col_addr|_CS_INFO(cs), wr_bytes, ram_addr);
    nsc_program_execute(info, cs, blk_pge_addr);
    ret = nsc_check_program_status(cs);

    return ret;
}

SECTION_SPI_NAND void
snaf_page_read(spi_nand_flash_info_t *info, void *dma_addr, u32_t blk_pge_addr)
{
    u32_t w_data, w_io_len, dma_io_len;
    u32_t column_addr=0x0000|(((blk_pge_addr>>PAGE_SHF)&1)<<(PLANE_SHF_BITS(info->_page_size)));
    u32_t dummy=0x00;
    u32_t dma_len=SNAF_PAGE_SIZE(info)+SNAF_SPARE_SIZE(info);
    u32_t cs=_EXTRACT_CS_INFO(blk_pge_addr, PGE_BLK_SHF(info->_num_block));
    blk_pge_addr=_REMVOE_CS_INFO(blk_pge_addr, PGE_BLK_SHF(info->_num_block));


    /*(Step1) Read data from nand flash to cache register */
    nsc_page_data_read_to_cache_buf(info, cs, blk_pge_addr);

    /*(Step2) Enable CS */
    WAIT_NAND_CTRLR_RDY();
    SPI_NAND_CS(cs, ACT);

    /*(Step3) Send 1-Byte Command */
    w_data = SNAF_RW_CMD(info)->r_cmd<<24;
    w_io_len = IO_WIDTH_LEN(SIO_WIDTH,CMR_LEN(1));
    TRIG_CTRLR_PIO_WRITE(w_data, w_io_len);

    /*(Step4) Send 2-Byte Column Address */
    w_data = (column_addr<<16);
    w_io_len = IO_WIDTH_LEN(SNAF_RW_CMD(info)->r_addr_io, CMR_LEN(2));
    TRIG_CTRLR_PIO_WRITE(w_data, w_io_len);

    /*(Step5) Send Dummy Cycles */
    w_data = dummy<<(32-SNAF_RW_CMD(info)->r_dummy_cycles);
    w_io_len = IO_WIDTH_LEN(SNAF_RW_CMD(info)->r_addr_io, CMR_LEN((SNAF_RW_CMD(info)->r_dummy_cycles/8)));
    TRIG_CTRLR_PIO_WRITE(w_data, w_io_len);

    /*(Step6) DMA Read from cache register to RAM */
    //_lplr_basic_io.dcache_writeback_invalidate_range((u32_t)dma_addr, (u32_t)(dma_addr+dma_len));
    DMA_CACHE_WBACK_INV((u32_t)dma_addr, (u32_t)(dma_addr+dma_len));
    dma_io_len = IO_WIDTH_LEN(SNAF_RW_CMD(info)->r_data_io, dma_len);
    TRIG_CTRLR_DMA_WRITE_READ(PADDR(dma_addr), dma_io_len, DMA_READ_DIR);
    WAIT_NAND_CTRLR_RDY();
    SPI_NAND_CS(cs, DEACT);

}

SECTION_SPI_NAND s32_t
snaf_page_write(spi_nand_flash_info_t *info, void *dma_addr, u32_t blk_pge_addr)
/* The maximum DMA length is 2^16-Byte
  * Start from the initial cache register (CA=0x0000)
  */
{
    s32_t ret;
    u32_t w_data, w_io_len, dma_io_len;
    u32_t column_addr=0x0000|(((blk_pge_addr>>PAGE_SHF)&1)<<(PLANE_SHF_BITS(info->_page_size)));
    u32_t dma_len=SNAF_PAGE_SIZE(info)+SNAF_SPARE_SIZE(info);
    u32_t cs=_EXTRACT_CS_INFO(blk_pge_addr, PGE_BLK_SHF(info->_num_block));
    blk_pge_addr=_REMVOE_CS_INFO(blk_pge_addr, PGE_BLK_SHF(info->_num_block));


    /*(Step1) Enable WEL */
    nsc_write_enable(cs);

    /*(Step2) Enable CS */
    WAIT_NAND_CTRLR_RDY();
    SPI_NAND_CS(cs, ACT);


    /*(Step3) 1-Byte Command */
    w_data = SNAF_RW_CMD(info)->w_cmd<<24;
    w_io_len = IO_WIDTH_LEN(SIO_WIDTH,CMR_LEN(1));
    TRIG_CTRLR_PIO_WRITE(w_data, w_io_len);

    /*(Step4) 2-Byte Address */
    w_data = column_addr<<16;
    w_io_len = IO_WIDTH_LEN(SNAF_RW_CMD(info)->w_addr_io,CMR_LEN(2));
    TRIG_CTRLR_PIO_WRITE(w_data, w_io_len);

    /*(Step5) DMA Write to cache register from RAM */
    //_lplr_basic_io.dcache_writeback_invalidate_range((u32_t)dma_addr, (u32_t)(dma_addr+dma_len));
    DMA_CACHE_WBACK_INV((u32_t)dma_addr, (u32_t)(dma_addr+dma_len));
    dma_io_len = IO_WIDTH_LEN(SNAF_RW_CMD(info)->w_data_io, dma_len);
    TRIG_CTRLR_DMA_WRITE_READ(PADDR(dma_addr), dma_io_len, DMA_WRITE_DIR);
    WAIT_NAND_CTRLR_RDY();

    /*(Step6) Write data from cache register to spi nand flash */
    nsc_program_execute(info, cs, blk_pge_addr);
    ret = nsc_check_program_status(cs);

    return ret;
}

SECTION_SPI_NAND s32_t
snaf_page_write_with_ecc_encode(spi_nand_flash_info_t *info, void *dma_addr, u32_t blk_pge_addr, void *p_eccbuf)
{
    ecc_encode_bch_sector(ECC_CORRECT_BITS(info), dma_addr, p_eccbuf, SNAF_PAGE_SIZE(info)/BCH_SECTOR_SIZE);
    return snaf_page_write(info, dma_addr, blk_pge_addr);
}

SECTION_SPI_NAND s32_t
snaf_page_read_with_ecc_decode(spi_nand_flash_info_t *info, void *dma_addr, u32_t blk_pge_addr, void *p_eccbuf)
{
    snaf_page_read(info, dma_addr, blk_pge_addr);
    return ecc_decode_bch_sector(ECC_CORRECT_BITS(info), dma_addr, p_eccbuf, SNAF_PAGE_SIZE(info)/BCH_SECTOR_SIZE);
}

SECTION_SPI_NAND s32_t
snaf_page_write_with_ondie_ecc(spi_nand_flash_info_t *info, void *dma_addr, u32_t blk_pge_addr, void *p_eccbuf)
{
    return snaf_page_write(info, dma_addr, blk_pge_addr);
}

SECTION_SPI_NAND s32_t
snaf_page_read_with_ondie_ecc(spi_nand_flash_info_t *info, void *dma_addr, u32_t blk_pge_addr, void *p_eccbuf)
{
    const u32_t ode_cs_signature = 0xfffffc50;
    const u32_t cs = _EXTRACT_CS_INFO(blk_pge_addr, PGE_BLK_SHF(info->_num_block));

    snaf_page_read(info, dma_addr, blk_pge_addr);
    return info->_ecc_decode(ECC_USE_ODE, VZERO, (void *)(ode_cs_signature | cs));
}

SECTION_SPI_NAND void
snaf_send_instruction_genernal(u32_t cs, u32_t opcode, u32_t settings, u32_t io_len)
{
    u32_t w_io_len = IO_WIDTH_LEN(SIO_WIDTH, CMR_LEN(io_len));
    _pio_raw_cmd(opcode|_CS_INFO(cs), settings, w_io_len, SNAF_DONT_CARE);
}

#ifndef CONFIG_UNDER_UBOOT
symb_fdefine(SNAF_PIO_WRITE_FUNC, snaf_pio_write_data);
symb_fdefine(SNAF_PIO_READ_FUNC, snaf_pio_read_data);
symb_fdefine(SNAF_PAGE_WRITE_FUNC, snaf_page_write);
symb_fdefine(SNAF_PAGE_READ_FUNC, snaf_page_read);
symb_fdefine(SNAF_PAGE_WRITE_ECC_FUNC, snaf_page_write_with_ecc_encode);
symb_fdefine(SNAF_PAGE_READ_ECC_FUNC, snaf_page_read_with_ecc_decode);
symb_fdefine(SNAF_PAGE_WRITE_ODE_FUNC, snaf_page_write_with_ondie_ecc);
symb_fdefine(SNAF_PAGE_READ_ODE_FUNC, snaf_page_read_with_ondie_ecc);
symb_fdefine(SNAF_SEND_INSTR_GEN_FUNC, snaf_send_instruction_genernal);
#endif // CONFIG_UNDER_UBOOT
