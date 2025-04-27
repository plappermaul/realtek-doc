#include <util.h>
#include <nand_spi/ecc_ctrl.h>
#include <nand_spi/nand_spi_ctrl.h>
#include <nand_spi/nand_spi_common.h>
#include <nand_spi/nand_spi_util.h>
#include <nand_spi/nand_spi_winbond.h>

// policy decision
    //input: #define NSU_PROHIBIT_QIO, or NSU_PROHIBIT_DIO  (in project/info.in)
    //          #define NSU_WINBOND_USING_QIO, NSU_WINBOND_USING_DIO, NSU_WINBOND_USING_SIO  (in project/info.in)
    //          #define NSU_DRIVER_IN_ROM, IS_RECYCLE_SECTION_EXIST (in template/info.in)
    //          #define NSU_USING_SYMBOL_TABLE_FUNCTION (in project/info.in)  

    //output: #define __DEVICE_REASSIGN, __DEVICE_USING_SIO, __DEVICE_USING_DIO, and __DEVICE_USING_QIO
    //            #define __SECTION_INIT_PHASE, __SECTION_INIT_PHASE_DATA
    //            #define __SECTION_RUNTIME, __SECTION_RUNTIME_DATA

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

#if __DEVICE_USING_QIO
__SECTION_INIT_PHASE_DATA 
nand_spi_cmd_info_t winbond_qio_cmd_info = {
    .w_cmd = PROGRAM_LOAD_X4_OP,
    .w_addr_io = SIO_WIDTH,
    .w_data_io = QIO_WIDTH,
    .r_cmd = FAST_READ_QIO_OP,
    .r_addr_io = QIO_WIDTH,
    .r_data_io = QIO_WIDTH,
    .r_dummy_cycles  = 16,
}
#endif

// code implement
__SECTION_INIT_PHASE_DATA
nand_spi_flash_info_t winbond_chip_info[] = {
    {
        .man_id              = MID_WINBOND, 
        .dev_id              = DID_W25N01GV,
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
            ._cmd_info       = &winbond_qio_cmd_info,
            ._model_info     = &snaf_rom_general_model,
    #endif
    },
};


__SECTION_INIT_PHASE void 
winbond_block_unprotect(void)
{
    u32_t feature_addr=0xA0;
    u32_t value = 0x00;
    value &= nsu_get_feature_reg(feature_addr);
    nsu_set_feature_reg(feature_addr, value);
}

__SECTION_INIT_PHASE void 
winbond_disable_on_die_ecc(void)
{
    u32_t feature_addr=0xB0;
    u32_t value = nsu_get_feature_reg(feature_addr);
    value &= ~(1<<4);
    nsu_set_feature_reg(feature_addr,value);
}

__SECTION_INIT_PHASE u32_t 
winbond_read_id(void)
{
    u32_t dummy = 0x00;
    u32_t w_io_len = IO_WIDTH_LEN(SIO_WIDTH,CMR_LEN(2));
    u32_t r_io_len = IO_WIDTH_LEN(SIO_WIDTH,CMR_LEN(3));
    
    u32_t ret = nsu_read_spi_nand_id(dummy, w_io_len, r_io_len);
    return ((ret>>8)&0xFFFFFF);
}

__SECTION_INIT_PHASE nand_spi_flash_info_t *
probe_winbond_spi_nand_chip(void)
{
    nsu_reset_spi_nand_chip();
    u32_t rdid = winbond_read_id();

    if(MID_WINBOND != (rdid >>16)) return VZERO;

    u16_t did = rdid&0xFFFF;
    u32_t i;
    for(i=0 ; i<sizeof(winbond_chip_info) ; i++){
        if(winbond_chip_info[i].dev_id == did){
            #ifdef __DEVICE_REASSIGN
                winbond_chip_info[i]._cmd_info = _nsu_cmd_info_ptr;
                winbond_chip_info[i]._model_info = &nsu_model_info;
                winbond_chip_info[i]._reset = _nsu_reset_ptr;
                winbond_chip_info[i]._ecc_encode= _nsu_ecc_encode_ptr;
                winbond_chip_info[i]._ecc_decode= _nsu_ecc_decode_ptr;
            #endif
            winbond_block_unprotect();
            winbond_disable_on_die_ecc();
            return &winbond_chip_info[i];
        }
    }
    return VZERO;
}

REG_SPI_NAND_PROBE_FUNC(probe_winbond_spi_nand_chip);

