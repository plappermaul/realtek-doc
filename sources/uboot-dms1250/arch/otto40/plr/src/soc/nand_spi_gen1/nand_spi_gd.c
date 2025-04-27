#include <util.h>
#include <nand_spi/ecc_ctrl.h>
#include <nand_spi/nand_spi_ctrl.h>
#include <nand_spi/nand_spi_common.h>
#include <nand_spi/nand_spi_util.h>
#include <nand_spi/nand_spi_gd.h>

// policy decision
    //input: #define NSU_PROHIBIT_QIO, or NSU_PROHIBIT_DIO  (in project/info.in)
    //       #define NSU_GD_USING_QIO, NSU_GD_USING_DIO, NSU_GD_USING_SIO  (in project/info.in)
    //       #define NSU_DRIVER_IN_ROM, IS_RECYCLE_SECTION_EXIST (in template/info.in)
    //       #define NSU_USING_SYMBOL_TABLE_FUNCTION (in project/info.in)  

    //output: #define __DEVICE_REASSIGN, __DEVICE_USING_SIO, __DEVICE_USING_DIO, and __DEVICE_USING_QIO
    //        #define __SECTION_INIT_PHASE, __SECTION_INIT_PHASE_DATA
    //        #define __SECTION_RUNTIME, __SECTION_RUNTIME_DATA

#ifdef NSU_DRIVER_IN_ROM
    #define __SECTION_INIT_PHASE      SECTION_NAND_SPI
    #define __SECTION_INIT_PHASE_DATA SECTION_NAND_SPI_DATA
    #define __SECTION_RUNTIME         SECTION_NAND_SPI
    #define __SECTION_RUNTIME_DATA    SECTION_NAND_SPI_DATA
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

    #ifdef NSU_GD_USING_QIO
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
    #elif defined(NSU_GD_USING_DIO)
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

#if __DEVICE_USING_QIO
nand_spi_cmd_info_t gd_qio_cmd_info = {
    .w_cmd = PROGRAM_LOAD_X4_OP,
    .w_addr_io = SIO_WIDTH,
    .w_data_io = QIO_WIDTH,
    .r_cmd = FAST_READ_QIO_OP,
    .r_addr_io = QIO_WIDTH,
    .r_data_io = QIO_WIDTH,
    .r_dummy_cycles  = 8,
}
#endif

__SECTION_INIT_PHASE_DATA
nand_spi_flash_info_t gd_chip_info[] = {
    {
        .man_id              = MID_GD, 
        .dev_id              = DID_GD5F1GQ4UAYIG,
        ._num_block          = SNAF_MODEL_NUM_BLK_1024,
        ._num_page_per_block = SNAF_MODEL_NUM_PAGE_64,
        ._page_size          = SNAF_MODEL_PAGE_SIZE_2048B,
        ._spare_size         = SNAF_MODEL_SPARE_SIZE_64B,
        ._oob_size           = SNAF_MODEL_OOB_SIZE(24),
        ._is_bch12           = 0,
        
        #if __DEVICE_REASSIGN
            ._ecc_encode     = VZERO,
            ._ecc_decode     = VZERO,
            ._reset          = VZERO,
            ._cmd_info       = VZERO,
            ._model_info     = VZERO,
        #elif __DEVICE_USING_SIO
            ._ecc_encode     = ecc_encode_bch,
            ._ecc_decode     = ecc_decode_bch,
            ._reset          = nsu_reset_spi_nand_chip,
            ._cmd_info       = &nsc_sio_cmd_info,
            ._model_info     = &snaf_rom_general_model,
        #elif __DEVICE_USING_DIO
            ._ecc_encode     = ecc_encode_bch,
            ._ecc_decode     = ecc_decode_bch,
            ._reset          = nsu_reset_spi_nand_chip,
            ._cmd_info       = &nsc_dio_cmd_info,
            ._model_info     = &snaf_rom_general_model,
        #elif __DEVICE_USING_QIO
            ._ecc_encode     = ecc_encode_bch,
            ._ecc_decode     = ecc_decode_bch,
            ._reset          = nsu_reset_spi_nand_chip,
            ._cmd_info       = &gd_qio_cmd_info,
            ._model_info     = &snaf_rom_general_model,
        #endif
    },
    {
        .man_id              = MID_GD, 
        .dev_id              = DID_GD5F1GQ4UBYIG,
        ._num_block          = SNAF_MODEL_NUM_BLK_1024,
        ._num_page_per_block = SNAF_MODEL_NUM_PAGE_64,
        ._page_size          = SNAF_MODEL_PAGE_SIZE_2048B,
        ._spare_size         = SNAF_MODEL_SPARE_SIZE_64B,
        ._oob_size           = SNAF_MODEL_OOB_SIZE(24),
        ._is_bch12           = 0,
        
        #if __DEVICE_REASSIGN
            ._ecc_encode     = VZERO,
            ._ecc_decode     = VZERO,
            ._reset          = VZERO,
            ._cmd_info       = VZERO,
            ._model_info     = VZERO,
        #elif __DEVICE_USING_SIO
            ._ecc_encode     = ecc_encode_bch,
            ._ecc_decode     = ecc_decode_bch,
            ._reset          = nsu_reset_spi_nand_chip,
            ._cmd_info       = &nsc_sio_cmd_info,
            ._model_info     = &snaf_rom_general_model,
        #elif __DEVICE_USING_DIO
            ._ecc_encode     = ecc_encode_bch,
            ._ecc_decode     = ecc_decode_bch,
            ._reset          = nsu_reset_spi_nand_chip,
            ._cmd_info       = &nsc_dio_cmd_info,
            ._model_info     = &snaf_rom_general_model,
        #elif __DEVICE_USING_QIO
            ._ecc_encode     = ecc_encode_bch,
            ._ecc_decode     = ecc_decode_bch,
            ._reset          = nsu_reset_spi_nand_chip,
            ._cmd_info       = &gd_qio_cmd_info,
            ._model_info     = &snaf_rom_general_model,
        #endif
    },
};



__SECTION_INIT_PHASE void 
gd_block_unprotect(void)
{
    u32_t feature_addr=0xA0;
    u32_t value = 0x00;
    nsu_set_feature_reg(feature_addr, value);
}

__SECTION_INIT_PHASE void 
gd_disable_on_die_ecc(void)
{
    u32_t feature_addr=0xB0;
    u32_t value = nsu_get_feature_reg(feature_addr);
    value &= ~(1<<4);
    nsu_set_feature_reg(feature_addr, value);
}

__SECTION_INIT_PHASE u32_t 
gd_read_id(void)
{
    u32_t man_addr = 0x00;
    u32_t w_io_len = IO_WIDTH_LEN(SIO_WIDTH,CMR_LEN(2));
    u32_t r_io_len = IO_WIDTH_LEN(SIO_WIDTH,CMR_LEN(2));
    u32_t ret = nsu_read_spi_nand_id(man_addr, w_io_len, r_io_len);
    return ((ret>>16)&0xFFFF);
}

__SECTION_INIT_PHASE nand_spi_flash_info_t *
probe_gd_spi_nand_chip(void)
{
    nsu_reset_spi_nand_chip();

    u32_t rdid = gd_read_id();
    if(MID_GD != (rdid >>8)) return VZERO;

    u16_t did = rdid&0xFF;
    u32_t i;
    for(i=0 ; i<sizeof(gd_chip_info) ; i++){
        if(gd_chip_info[i].dev_id == did){
            #ifdef __DEVICE_REASSIGN
            gd_chip_info[i]._cmd_info = _nsu_cmd_info_ptr;
            gd_chip_info[i]._model_info = &nsu_model_info;
            gd_chip_info[i]._reset = _nsu_reset_ptr;
            gd_chip_info[i]._ecc_encode= _nsu_ecc_encode_ptr;
            gd_chip_info[i]._ecc_decode= _nsu_ecc_decode_ptr;
            #endif
            gd_block_unprotect();
            gd_disable_on_die_ecc();
            return &gd_chip_info[i];
        }
    }
    return VZERO;
}

REG_SPI_NAND_PROBE_FUNC(probe_gd_spi_nand_chip);

