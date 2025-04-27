#include <common/rt_type.h>
#include <spi_nand/spi_nand_ctrl.h>
#include <spi_nand/spi_nand_common.h>
#include <util.h>
#ifdef CONFIG_SPI_NAND_FLASH_INIT_FIRST
    #include <spi_nand/spi_nand_util.h>
#endif
#if defined(CONFIG_UNDER_UBOOT) && !defined(CONFIG_SPI_NAND_FLASH_INIT_FIRST)
    #include <spi_nand/spi_nand_blr_util.h>
    #include <spi_nand/spi_nand_symb_func.h>
#endif
#include <spi_nand_struct.h>

/***********************************************
  *  Winbond's ID Definition
  ***********************************************/
#define MID_WINBOND         (0xEF)
#define DID_W25N01GV        (0xAA21)
#define DID_W25N02KV        (0xAA22)

// policy decision
    //input: #define NSU_PROHIBIT_QIO, or NSU_PROHIBIT_DIO  (in project/info.in)
    //          #define NSU_WINBOND_USING_QIO, NSU_WINBOND_USING_DIO, NSU_WINBOND_USING_SIO  (in project/info.in)
    //          #define NSU_DRIVER_IN_ROM, IS_RECYCLE_SECTION_EXIST (in template/info.in)
    //          #define NSU_USING_SYMBOL_TABLE_FUNCTION (in project/info.in)
    //output: #define __DEVICE_REASSIGN, __DEVICE_USING_SIO, __DEVICE_USING_DIO, and __DEVICE_USING_QIO
    //            #define __SECTION_INIT_PHASE, __SECTION_INIT_PHASE_DATA
    //            #define __SECTION_RUNTIME, __SECTION_RUNTIME_DATA

#ifdef NSU_DRIVER_IN_ROM
    #define __SECTION_INIT_PHASE      SECTION_SPI_NAND
    #define __SECTION_INIT_PHASE_DATA SECTION_SPI_NAND_DATA
    #define __SECTION_RUNTIME         SECTION_SPI_NAND
    #define __SECTION_RUNTIME_DATA    SECTION_SPI_NAND_DATA
    #if defined(NSU_PROHIBIT_QIO) || defined(NSU_PROHIBIT_DIO)
        #error 'lplr should not run at ...'
    #endif
    #ifdef IS_RECYCLE_SECTION_EXIST
        #error 'lplr should not have recycle section ...'
    #endif
    #define __DEVICE_USING_SIO 1
    #define __DEVICE_USING_DIO 0
    #define __DEVICE_USING_QIO 0
#else
    #ifdef NSU_USING_SYMBOL_TABLE_FUNCTION
        #define __DEVICE_REASSIGN 1
    #endif
    #ifdef IS_RECYCLE_SECTION_EXIST
        #define __SECTION_INIT_PHASE        SECTION_RECYCLE
        #define __SECTION_INIT_PHASE_DATA   SECTION_RECYCLE_DATA
        #define __SECTION_RUNTIME           SECTION_UNS_TEXT
        #define __SECTION_RUNTIME_DATA      SECTION_UNS_RO
    #else
        #define __SECTION_INIT_PHASE
        #define __SECTION_INIT_PHASE_DATA
        #define __SECTION_RUNTIME
        #define __SECTION_RUNTIME_DATA
    #endif

    #ifdef NSU_WINBOND_USING_QIO
        #if defined(NSU_PROHIBIT_QIO) && defined(NSU_PROHIBIT_DIO)
            #define __DEVICE_USING_SIO 1
            #define __DEVICE_USING_DIO 0
            #define __DEVICE_USING_QIO 0
        #elif defined(NSU_PROHIBIT_QIO)
            #define __DEVICE_USING_SIO 0
            #define __DEVICE_USING_DIO 1
            #define __DEVICE_USING_QIO 0
        #else
            #define __DEVICE_USING_SIO 0
            #define __DEVICE_USING_DIO 0
            #define __DEVICE_USING_QIO 1
        #endif
    #elif defined(NSU_WINBOND_USING_DIO)
        #if defined(NSU_PROHIBIT_DIO)
            #define __DEVICE_USING_SIO 1
            #define __DEVICE_USING_DIO 0
            #define __DEVICE_USING_QIO 0
        #else
            #define __DEVICE_USING_SIO 0
            #define __DEVICE_USING_DIO 1
            #define __DEVICE_USING_QIO 0
        #endif
    #else
        #define __DEVICE_USING_SIO 1
        #define __DEVICE_USING_DIO 0
        #define __DEVICE_USING_QIO 0
    #endif
#endif

#ifdef CONFIG_SPI_NAND_FLASH_INIT_FIRST
#if __DEVICE_USING_QIO
__SECTION_INIT_PHASE_DATA
spi_nand_cmd_info_t winbond_qio_cmd_info = {
    .w_cmd = PROGRAM_LOAD_X4_OP,
    .w_addr_io = SIO_WIDTH,
    .w_data_io = QIO_WIDTH,
    .r_cmd = FAST_READ_QIO_OP,
    .r_addr_io = QIO_WIDTH,
    .r_data_io = QIO_WIDTH,
    .r_dummy_cycles  = 16,
};
#endif

#if defined(NSU_DRIVER_IN_ROM) || (defined(CONFIG_SPI_NAND_FLASH_INIT_FIRST) && defined(CONFIG_UNDER_UBOOT))
__SECTION_INIT_PHASE_DATA
spi_nand_model_info_t winbond_ode_model = {
    ._pio_write = snaf_pio_write_data,
    ._pio_read = snaf_pio_read_data,
    ._page_read = snaf_page_read,
    ._page_write = snaf_page_write,
    ._page_read_ecc = snaf_page_read_with_ondie_ecc,
    ._page_write_ecc = snaf_page_write_with_ondie_ecc,
    ._block_erase = nsc_block_erase,
    ._wait_spi_nand_ready = nsc_wait_spi_nand_oip_ready,
};
#endif

__SECTION_RUNTIME void
winbond_ecc_encode(u32_t ecc_ability, void *dma_addr, void *fake_ptr_cs) { return; }


__SECTION_RUNTIME s32_t
winbond_ecc_decode(u32_t ecc_ability, void *dma_addr, void *fake_ptr_cs)
{
    const u32_t cs = ((u32_t)fake_ptr_cs) & 0xF;
    u32_t ecst = nsu_get_feature_reg(cs, 0xC0);
    u32_t res;

    ecst = (ecst>>4)&0x3;
    if (0==ecst) { //no error
        res = 0;
    } else if (2==ecst) { // error and no correted
        res = ECC_CTRL_ERR;
    } else { // error and corrected
        res = nsu_get_feature_reg(cs, 0x10)>>4; // ECC Bit Flip Count Detection
    }

    return res;
}

__SECTION_INIT_PHASE_DATA
spi_nand_flash_info_t winbond_chip_info[] = {
    {
        .man_id              = MID_WINBOND,
        .dev_id              = DID_W25N01GV,
        ._num_block          = SNAF_MODEL_NUM_BLK_1024,
        ._num_page_per_block = SNAF_MODEL_NUM_PAGE_64,
        ._page_size          = SNAF_MODEL_PAGE_SIZE_2048B,
        ._spare_size         = SNAF_MODEL_SPARE_SIZE_64B,
        ._oob_size           = SNAF_MODEL_OOB_SIZE(24),
        ._ecc_ability        = ECC_MODEL_6T,
    #if __DEVICE_REASSIGN
            ._ecc_encode     = VZERO,
            ._ecc_decode     = VZERO,
            ._reset          = VZERO,
        ._model_info         = VZERO,
            ._cmd_info       = VZERO,
    #else
            ._ecc_encode     = ecc_encode_bch,
            ._ecc_decode     = ecc_decode_bch,
            ._reset          = nsu_reset_spi_nand_chip,
            ._model_info     = &snaf_rom_general_model,
        #if __DEVICE_USING_QIO
        ._cmd_info           = &winbond_qio_cmd_info,
    #elif __DEVICE_USING_DIO
            ._cmd_info       = &nsc_dio_cmd_info,
        #else
        ._cmd_info           = &nsc_sio_cmd_info,
        #endif
    #endif
    },{
        .man_id              = MID_WINBOND,
        .dev_id              = DID_W25N02KV,
        ._num_block          = SNAF_MODEL_NUM_BLK_2048,
        ._num_page_per_block = SNAF_MODEL_NUM_PAGE_64,
        ._page_size          = SNAF_MODEL_PAGE_SIZE_2048B,
        ._spare_size         = SNAF_MODEL_SPARE_SIZE_64B, //64B for internal ECC
        ._oob_size           = SNAF_MODEL_OOB_SIZE(62),
        ._ecc_ability        = ECC_USE_ODE,
    #if __DEVICE_REASSIGN
        ._ecc_encode         = VZERO,
        ._ecc_decode         = VZERO,
        ._reset              = VZERO,
        ._model_info         = VZERO,
        ._cmd_info           = VZERO,
    #else
        ._ecc_encode         = winbond_ecc_encode,
        ._ecc_decode         = winbond_ecc_decode,
            ._reset          = nsu_reset_spi_nand_chip,
        ._model_info         = &winbond_ode_model,
        #if __DEVICE_USING_QIO
            ._cmd_info       = &winbond_qio_cmd_info,
        #elif __DEVICE_USING_DIO
        ._cmd_info           = &nsc_dio_cmd_info,
        #else
        ._cmd_info           = &nsc_sio_cmd_info,
        #endif
    #endif
    },{//This is for Default
        .man_id              = MID_WINBOND,
        .dev_id              = DEFAULT_DATA_BASE,
        ._num_block          = SNAF_MODEL_NUM_BLK_1024,
        ._num_page_per_block = SNAF_MODEL_NUM_PAGE_64,
        ._page_size          = SNAF_MODEL_PAGE_SIZE_2048B,
        ._spare_size         = SNAF_MODEL_SPARE_SIZE_64B,
        ._oob_size           = SNAF_MODEL_OOB_SIZE(24),
        ._ecc_ability        = ECC_MODEL_6T,
    #if __DEVICE_REASSIGN
            ._ecc_encode     = VZERO,
            ._ecc_decode     = VZERO,
            ._reset          = VZERO,
        ._model_info         = VZERO,
            ._cmd_info       = VZERO,
    #else
            ._ecc_encode     = ecc_encode_bch,
            ._ecc_decode     = ecc_decode_bch,
            ._reset          = nsu_reset_spi_nand_chip,
        ._model_info         = &snaf_rom_general_model,
            ._cmd_info       = &nsc_sio_cmd_info,
    #endif
    }
};
#endif // CONFIG_SPI_NAND_FLASH_INIT_FIRST

__SECTION_INIT_PHASE void
winbond_block_unprotect(u32_t cs)
{
    u32_t feature_addr=0xA0;
    u32_t value = 0x00;
    value &= nsu_get_feature_reg(cs, feature_addr);
    nsu_set_feature_reg(cs, feature_addr, value);
}

__SECTION_INIT_PHASE u32_t
winbond_read_id(u32_t cs)
{
    u32_t dummy = 0x00;
    u32_t w_io_len = IO_WIDTH_LEN(SIO_WIDTH,CMR_LEN(2));
    u32_t r_io_len = IO_WIDTH_LEN(SIO_WIDTH,CMR_LEN(3));

    u32_t ret = nsu_read_spi_nand_id(cs, dummy, w_io_len, r_io_len);
    return ((ret>>8)&0xFFFFFF);
}

#ifdef CONFIG_SPI_NAND_FLASH_INIT_FIRST
__SECTION_INIT_PHASE spi_nand_flash_info_t *
probe_winbond_spi_nand_chip(void)
{
    const u32_t cs = 0;
    nsu_reset_spi_nand_chip(cs);
    u32_t rdid = winbond_read_id(cs);

    if(MID_WINBOND != (rdid >>16)) return VZERO;

    u16_t did = rdid&0xFFFF;
    u32_t i;
    for(i=0 ; i<ELEMENT_OF_SNAF_INFO(winbond_chip_info) ; i++){
        if((winbond_chip_info[i].dev_id == did) || (winbond_chip_info[i].dev_id == DEFAULT_DATA_BASE) ){
            #if __DEVICE_REASSIGN
                #if __DEVICE_USING_QIO
                winbond_chip_info[i]._cmd_info = &winbond_qio_cmd_info;
                #elif __DEVICE_USING_DIO
                winbond_chip_info[i]._cmd_info = _nsu_dio_cmd_info_ptr;
                #else
                winbond_chip_info[i]._cmd_info = _nsu_cmd_info_ptr;
                #endif
                winbond_chip_info[i]._model_info = &nsu_model_info;
                winbond_chip_info[i]._reset = _nsu_reset_ptr;
                if(ECC_USE_ODE==winbond_chip_info[i]._ecc_ability) {
                    nsu_model_info._page_read_ecc  = _nsu_page_read_with_ode_ptr;
                    nsu_model_info._page_write_ecc = _nsu_page_write_with_ode_ptr;
                    winbond_chip_info[i]._ecc_encode = winbond_ecc_encode;
                    winbond_chip_info[i]._ecc_decode = winbond_ecc_decode;
                } else {
                winbond_chip_info[i]._ecc_encode= _nsu_ecc_encode_ptr;
                winbond_chip_info[i]._ecc_decode= _nsu_ecc_decode_ptr;
                }
            #endif

            winbond_block_unprotect(cs);
            if(ECC_USE_ODE==winbond_chip_info[i]._ecc_ability) {
                nsu_enable_on_die_ecc(cs);
            } else {
                nsu_disable_on_die_ecc(cs);
            }
            return &winbond_chip_info[i];
        }
    }
    return VZERO;
}
REG_SPI_NAND_PROBE_FUNC(probe_winbond_spi_nand_chip);
#endif   // CONFIG_SPI_NAND_FLASH_INIT_FIRST
#ifdef CONFIG_SPI_NAND_FLASH_INIT_REST
int
winbond_init_rest(spi_nand_flash_info_t * _spi_nand_info)
{
    const u32_t cs=1;

    // check ID, cs0 and cs1 should be identical
    u32_t rdid = winbond_read_id(cs);
    if(((rdid>>8)!=MID_WINBOND) && ((rdid&0xFFFF)!=_spi_nand_info->dev_id))
      { return 0; }

    // reset
    nsu_reset_spi_nand_chip(cs);

    // misc
    winbond_block_unprotect(cs);

    if(ECC_USE_ODE==_spi_nand_info->_ecc_ability) {
        nsu_enable_on_die_ecc(cs);
    } else {
    nsu_disable_on_die_ecc(cs);
    }
    return 1;
}
/* Unused in Linux Kernel */
//REG_SPI_NAND_INIT_REST_FUNC(winbond_init_rest);
#endif // CONFIG_SPI_NAND_FLASH_INIT_REST
