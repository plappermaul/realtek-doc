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

/***********************************************
  *  MXIC's ID Definition
  ***********************************************/
#define MID_MXIC         (0xC2)
#define DID_MX35LF1GE4AB (0x12)
#define DID_MX35LF2GE4AB (0x22)
#define DID_MX35LF2G14AC (0x20) //No ODE
#define DID_MX35LF2GE4AD (0x26)
#define DID_MX35LF4GE4AD (0x37)

// policy decision
    //input: #define NSU_PROHIBIT_QIO, or NSU_PROHIBIT_DIO  (in project/info.in)
    //       #define NSU_MXIC_USING_QIO, NSU_MXIC_USING_DIO, NSU_MXIC_USING_SIO  (in project/info.in)
    //       #define NSU_DRIVER_IN_ROM, IS_RECYCLE_SECTION_EXIST (in template/info.in)
    //       #define NSU_USING_SYMBOL_TABLE_FUNCTION (in project/info.in)

    //output: #define __DEVICE_REASSIGN, __DEVICE_USING_SIO, __DEVICE_USING_DIO, and __DEVICE_USING_QIO
    //        #define __SECTION_INIT_PHASE, __SECTION_INIT_PHASE_DATA
    //        #define __SECTION_RUNTIME, __SECTION_RUNTIME_DATA

#ifdef NSU_DRIVER_IN_ROM
    #define __SECTION_INIT_PHASE      SECTION_SPI_NAND
    #define __SECTION_INIT_PHASE_DATA SECTION_SPI_NAND_DATA
    #define __SECTION_RUNTIME         SECTION_SPI_NAND
    #define __SECTION_RUNTIME_DATA    SECTION_SPI_NAND_DATA
    #if defined(NSU_MXIC_USING_QIO) || defined(NSU_MXIC_USING_DIO)
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

    #ifdef NSU_MXIC_USING_QIO
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
    #elif defined(NSU_MXIC_USING_DIO)
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
spi_nand_cmd_info_t mxic_x4_cmd_info = {
    .w_cmd = PROGRAM_LOAD_X4_OP,
    .w_addr_io = SIO_WIDTH,
    .w_data_io = QIO_WIDTH,
    .r_cmd = FAST_READ_X4_OP,
    .r_addr_io = SIO_WIDTH,
    .r_data_io = QIO_WIDTH,
    .r_dummy_cycles  = 8,
};
#endif

__SECTION_INIT_PHASE_DATA
spi_nand_flash_info_t mxic_chip_info[] = {
    {
        .man_id              = MID_MXIC,
        .dev_id              = DID_MX35LF1GE4AB,
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
            ._cmd_info       = VZERO,
            ._model_info     = VZERO,
        #else
            ._ecc_encode     = ecc_encode_bch,
            ._ecc_decode     = ecc_decode_bch,
            ._reset          = nsu_reset_spi_nand_chip,
            ._model_info     = &snaf_rom_general_model,
            #if __DEVICE_USING_SIO
                ._cmd_info   = &nsc_sio_cmd_info,
            #elif __DEVICE_USING_DIO
                ._cmd_info   = &nsc_x2_cmd_info,
            #elif __DEVICE_USING_QIO
                ._cmd_info   = &mxic_x4_cmd_info,
            #endif
        #endif
    },
    {
        .man_id              = MID_MXIC,
        .dev_id              = DID_MX35LF2GE4AB,
        ._num_block          = SNAF_MODEL_NUM_BLK_2048,
        ._num_page_per_block = SNAF_MODEL_NUM_PAGE_64,
        ._page_size          = SNAF_MODEL_PAGE_SIZE_2048B,
        ._spare_size         = SNAF_MODEL_SPARE_SIZE_64B,
        ._oob_size           = SNAF_MODEL_OOB_SIZE(24),
        ._ecc_ability        = ECC_MODEL_6T,
        #if __DEVICE_REASSIGN
            ._ecc_encode     = VZERO,
            ._ecc_decode     = VZERO,
            ._reset          = VZERO,
            ._cmd_info       = VZERO,
            ._model_info     = VZERO,
        #else
            ._ecc_encode     = ecc_encode_bch,
            ._ecc_decode     = ecc_decode_bch,
            ._reset          = nsu_reset_spi_nand_chip,
            ._model_info     = &snaf_rom_general_model,
            #if __DEVICE_USING_SIO
                ._cmd_info   = &nsc_sio_cmd_info,
            #elif __DEVICE_USING_DIO
                ._cmd_info   = &nsc_x2_cmd_info,
            #elif __DEVICE_USING_QIO
                ._cmd_info   = &mxic_x4_cmd_info,
            #endif
        #endif
    },
    {
        .man_id              = MID_MXIC,
        .dev_id              = DID_MX35LF2G14AC,
        ._num_block          = SNAF_MODEL_NUM_BLK_2048,
        ._num_page_per_block = SNAF_MODEL_NUM_PAGE_64,
        ._page_size          = SNAF_MODEL_PAGE_SIZE_2048B,
        ._spare_size         = SNAF_MODEL_SPARE_SIZE_64B,
        ._oob_size           = SNAF_MODEL_OOB_SIZE(24),
        ._ecc_ability        = ECC_MODEL_6T,
        #if __DEVICE_REASSIGN
            ._ecc_encode     = VZERO,
            ._ecc_decode     = VZERO,
            ._reset          = VZERO,
            ._cmd_info       = VZERO,
            ._model_info     = VZERO,
        #else
            ._ecc_encode     = ecc_encode_bch,
            ._ecc_decode     = ecc_decode_bch,
            ._reset          = nsu_reset_spi_nand_chip,
            ._model_info     = &snaf_rom_general_model,
            #if __DEVICE_USING_SIO
                ._cmd_info   = &nsc_sio_cmd_info,
            #elif __DEVICE_USING_DIO
                ._cmd_info   = &nsc_x2_cmd_info,
            #elif __DEVICE_USING_QIO
                ._cmd_info   = &mxic_x4_cmd_info,
            #endif
        #endif
    },
    {
        .man_id              = MID_MXIC,
        .dev_id              = DID_MX35LF2GE4AD,
        ._num_block          = SNAF_MODEL_NUM_BLK_2048,
        ._num_page_per_block = SNAF_MODEL_NUM_PAGE_64,
        ._page_size          = SNAF_MODEL_PAGE_SIZE_2048B,
        ._spare_size         = SNAF_MODEL_SPARE_SIZE_64B,
        ._oob_size           = SNAF_MODEL_OOB_SIZE(24),
        ._ecc_ability        = ECC_MODEL_6T,
        #if __DEVICE_REASSIGN
            ._ecc_encode     = VZERO,
            ._ecc_decode     = VZERO,
            ._reset          = VZERO,
            ._cmd_info       = VZERO,
            ._model_info     = VZERO,
        #else
            ._ecc_encode     = ecc_encode_bch,
            ._ecc_decode     = ecc_decode_bch,
            ._reset          = nsu_reset_spi_nand_chip,
            ._model_info     = &snaf_rom_general_model,
            #if __DEVICE_USING_SIO
                ._cmd_info   = &nsc_sio_cmd_info,
            #elif __DEVICE_USING_DIO
                ._cmd_info   = &nsc_x2_cmd_info,
            #elif __DEVICE_USING_QIO
                ._cmd_info   = &mxic_x4_cmd_info,
            #endif
        #endif
    },
    {
        .man_id              = MID_MXIC,
        .dev_id              = DID_MX35LF4GE4AD,
        ._num_block          = SNAF_MODEL_NUM_BLK_2048,
        ._num_page_per_block = SNAF_MODEL_NUM_PAGE_64,
        ._page_size          = SNAF_MODEL_PAGE_SIZE_4096B,
        ._spare_size         = SNAF_MODEL_SPARE_SIZE_128B,
        ._oob_size           = SNAF_MODEL_OOB_SIZE(48),
        ._ecc_ability        = ECC_MODEL_6T,
        #if __DEVICE_REASSIGN
            ._ecc_encode     = VZERO,
            ._ecc_decode     = VZERO,
            ._reset          = VZERO,
            ._cmd_info       = VZERO,
            ._model_info     = VZERO,
        #else
            ._ecc_encode     = ecc_encode_bch,
            ._ecc_decode     = ecc_decode_bch,
            ._reset          = nsu_reset_spi_nand_chip,
            ._model_info     = &snaf_rom_general_model,
            #if __DEVICE_USING_SIO
                ._cmd_info   = &nsc_sio_cmd_info,
            #elif __DEVICE_USING_DIO
                ._cmd_info   = &nsc_x2_cmd_info,
            #elif __DEVICE_USING_QIO
                ._cmd_info   = &mxic_x4_cmd_info,
            #endif
        #endif
    },
    {//This is for Default
        .man_id              = MID_MXIC,
        .dev_id              = DEFAULT_DATA_BASE,
        ._num_block          = SNAF_MODEL_NUM_BLK_2048,
        ._num_page_per_block = SNAF_MODEL_NUM_PAGE_64,
        ._page_size          = SNAF_MODEL_PAGE_SIZE_2048B,
        ._spare_size         = SNAF_MODEL_SPARE_SIZE_64B,
        ._oob_size           = SNAF_MODEL_OOB_SIZE(24),
        ._ecc_ability        = ECC_MODEL_6T,
        #if __DEVICE_REASSIGN
            ._ecc_encode     = VZERO,
            ._ecc_decode     = VZERO,
            ._reset          = VZERO,
            ._cmd_info       = VZERO,
            ._model_info     = VZERO,
        #else
            ._ecc_encode     = ecc_encode_bch,
            ._ecc_decode     = ecc_decode_bch,
            ._reset          = nsu_reset_spi_nand_chip,
            ._cmd_info       = &nsc_sio_cmd_info,
            ._model_info     = &snaf_rom_general_model,
        #endif
    }
};
#endif // CONFIG_SPI_NAND_FLASH_INIT_FIRST

#if __DEVICE_USING_QIO
__SECTION_INIT_PHASE void
mxic_quad_enable(u32_t cs)
{
    u32_t feature_addr=0xB0;
    u32_t value = 1;
    value |= nsu_get_feature_reg(cs, feature_addr);
    nsu_set_feature_reg(cs, feature_addr, value);
}
#endif

__SECTION_INIT_PHASE u32_t
mxic_read_id(u32_t cs)
{
    u32_t dummy = 0x00;
    u32_t w_io_len = IO_WIDTH_LEN(SIO_WIDTH,CMR_LEN(2));
    u32_t r_io_len = IO_WIDTH_LEN(SIO_WIDTH,CMR_LEN(2));

    u32_t ret = nsu_read_spi_nand_id(cs, dummy, w_io_len, r_io_len);
    return ((ret>>16)&0xFFFF);
}

#ifdef CONFIG_SPI_NAND_FLASH_INIT_FIRST
__SECTION_INIT_PHASE spi_nand_flash_info_t *
probe_mxic_spi_nand_chip(void)
{
    nsu_reset_spi_nand_chip(0);
    u32_t rdid = mxic_read_id(0);

    if(MID_MXIC != (rdid>>8)) return VZERO;

    u16_t did8  = rdid&0xFF;
    u32_t i;
    for(i=0 ; i<ELEMENT_OF_SNAF_INFO(mxic_chip_info) ; i++){
        if((mxic_chip_info[i].dev_id == did8) || (mxic_chip_info[i].dev_id == DEFAULT_DATA_BASE))
        {
            #if __DEVICE_REASSIGN
                #if __DEVICE_USING_SIO
                    mxic_chip_info[i]._cmd_info = _nsu_cmd_info_ptr;
                #elif __DEVICE_USING_DIO
                    mxic_chip_info[i]._cmd_info = _nsu_x2_cmd_info_ptr;
                #elif __DEVICE_USING_QIO
                    mxic_chip_info[i]._cmd_info = &mxic_x4_cmd_info;
                #endif
                mxic_chip_info[i]._model_info= &nsu_model_info;
                mxic_chip_info[i]._reset     = _nsu_reset_ptr;
                mxic_chip_info[i]._ecc_encode= _nsu_ecc_encode_ptr;
                mxic_chip_info[i]._ecc_decode= _nsu_ecc_decode_ptr;
            #endif

            if((mxic_chip_info[i].dev_id != DID_MX35LF2G14AC) && (mxic_chip_info[i].dev_id != DID_MX35LF2GE4AD) && (mxic_chip_info[i].dev_id != DID_MX35LF4GE4AD)){
                nsu_disable_on_die_ecc(0);
            }
            #if __DEVICE_USING_QIO
                mxic_quad_enable(0);
            #endif


            nsu_block_unprotect(0);
            return &mxic_chip_info[i];
        }
    }
    return VZERO;
}
REG_SPI_NAND_PROBE_FUNC(probe_mxic_spi_nand_chip);
#endif   // CONFIG_SPI_NAND_FLASH_INIT_FIRST
#ifdef CONFIG_SPI_NAND_FLASH_INIT_REST
int
mxic_init_rest(spi_nand_flash_info_t * _spi_nand_info)
{
    u32_t cs=1;

    // check ID, cs0 and cs1 should be identical
    u32_t rdid = mxic_read_id(cs);
    //u32_t cs0_id = (_spi_nand_info->man_id<<8) | _spi_nand_info->dev_id;
    if(((rdid>>8)!=MID_MXIC) || ((rdid&0xFF)!=_spi_nand_info->dev_id))
      { return 0; }

    // reset
    nsu_reset_spi_nand_chip(cs);

    nsu_disable_on_die_ecc(cs);
    nsu_block_unprotect(cs);
    return 1;
}
//REG_SPI_NAND_INIT_REST_FUNC(mxic_init_rest);
#endif // CONFIG_SPI_NAND_FLASH_INIT_REST
