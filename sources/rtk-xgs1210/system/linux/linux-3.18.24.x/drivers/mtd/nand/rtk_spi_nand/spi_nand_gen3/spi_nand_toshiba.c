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
  *  Toshiba's ID Definition
  ***********************************************/
#define MID_TOSHIBA         (0x98)
#define DID_TC58CVG0S3HRAIG (0xC2)
#define DID_TC58CVG1S3HRAIG (0xCB)
#define DID_TC58CVG2S3HRAIG (0xCD)

// policy decision
    //input: #define NSU_PROHIBIT_QIO, or NSU_PROHIBIT_DIO  (in project/info.in)
    //          #define NSU_TOSHIBA_USING_QIO, NSU_TOSHIBA_USING_DIO, NSU_TOSHIBA_USING_SIO  (in project/info.in)
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
    #if defined(NSU_TOSHIBA_USING_QIO) || defined(NSU_TOSHIBA_USING_DIO)
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

    #ifdef NSU_TOSHIBA_USING_QIO
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
    #elif defined(NSU_TOSHIBA_USING_DIO)
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

//The Toshiba specific function
void toshiba_ecc_encode(u32_t ecc_ability, void *dma_addr, void *fake_ptr_cs);
s32_t toshiba_ecc_decode(u32_t ecc_ability, void *dma_addr, void *fake_ptr_cs);

#ifdef CONFIG_SPI_NAND_FLASH_INIT_FIRST
#if __DEVICE_USING_QIO
__SECTION_INIT_PHASE_DATA
spi_nand_cmd_info_t toshiba_x4_cmd_info = {
    .w_cmd = PROGRAM_LOAD_OP,
    .w_addr_io = SIO_WIDTH,
    .w_data_io = SIO_WIDTH,
    .r_cmd = FAST_READ_X4_OP,
    .r_addr_io = SIO_WIDTH,
    .r_data_io = QIO_WIDTH,
    .r_dummy_cycles  = 8,
};
#endif

__SECTION_INIT_PHASE_DATA
spi_nand_flash_info_t toshiba_chip_info[] = {
    {
        .man_id              = MID_TOSHIBA,
        .dev_id              = DID_TC58CVG0S3HRAIG,
        ._num_block          = SNAF_MODEL_NUM_BLK_1024,
        ._num_page_per_block = SNAF_MODEL_NUM_PAGE_64,
        ._page_size          = SNAF_MODEL_PAGE_SIZE_2048B,
        ._spare_size         = SNAF_MODEL_SPARE_SIZE_64B,
        ._oob_size           = SNAF_MODEL_OOB_SIZE(24),
        ._ecc_ability        = ECC_USE_ODE,
        #if __DEVICE_REASSIGN
            ._ecc_encode     = VZERO,
            ._ecc_decode     = VZERO,
            ._reset          = VZERO,
            ._cmd_info       = VZERO,
            ._model_info     = VZERO,
        #else
            ._ecc_encode     = toshiba_ecc_encode,
            ._ecc_decode     = toshiba_ecc_decode,
            ._reset          = nsu_reset_spi_nand_chip,
            ._model_info     = &snaf_ode_model,
            #if __DEVICE_USING_SIO
                ._cmd_info   = &nsc_sio_cmd_info,
                #elif __DEVICE_USING_DIO
                ._cmd_info   = &nsc_x2_cmd_info,
            #elif __DEVICE_USING_QIO
                ._cmd_info   = &toshiba_x4_cmd_info,
            #endif
        #endif
    },
    {
        .man_id              = MID_TOSHIBA,
        .dev_id              = DID_TC58CVG1S3HRAIG,
        ._num_block          = SNAF_MODEL_NUM_BLK_2048,
        ._num_page_per_block = SNAF_MODEL_NUM_PAGE_64,
        ._page_size          = SNAF_MODEL_PAGE_SIZE_2048B,
        ._spare_size         = SNAF_MODEL_SPARE_SIZE_64B,
        ._oob_size           = SNAF_MODEL_OOB_SIZE(24),
        ._ecc_ability        = ECC_USE_ODE,
        #if __DEVICE_REASSIGN
            ._ecc_encode     = VZERO,
            ._ecc_decode     = VZERO,
            ._reset          = VZERO,
            ._cmd_info       = VZERO,
            ._model_info     = VZERO,
        #else
            ._ecc_encode     = toshiba_ecc_encode,
            ._ecc_decode     = toshiba_ecc_decode,
            ._reset          = nsu_reset_spi_nand_chip,
            ._model_info     = &snaf_ode_model,
            #if __DEVICE_USING_SIO
                ._cmd_info   = &nsc_sio_cmd_info,
            #elif __DEVICE_USING_DIO
                ._cmd_info   = &nsc_x2_cmd_info,
            #elif __DEVICE_USING_QIO
                ._cmd_info   = &toshiba_x4_cmd_info,
            #endif
        #endif
    },
    {
        .man_id              = MID_TOSHIBA,
        .dev_id              = DID_TC58CVG2S3HRAIG,
        ._num_block          = SNAF_MODEL_NUM_BLK_2048,
        ._num_page_per_block = SNAF_MODEL_NUM_PAGE_64,
        ._page_size          = SNAF_MODEL_PAGE_SIZE_4096B,
        ._spare_size         = SNAF_MODEL_SPARE_SIZE_128B,
        ._oob_size           = SNAF_MODEL_OOB_SIZE(48),
        ._ecc_ability        = ECC_USE_ODE,
        #if __DEVICE_REASSIGN
            ._ecc_encode     = VZERO,
            ._ecc_decode     = VZERO,
            ._reset          = VZERO,
            ._cmd_info       = VZERO,
            ._model_info     = VZERO,
        #else
            ._ecc_encode     = toshiba_ecc_encode,
            ._ecc_decode     = toshiba_ecc_decode,
            ._reset          = nsu_reset_spi_nand_chip,
            ._model_info     = &snaf_ode_model,
            #if __DEVICE_USING_SIO
                ._cmd_info   = &nsc_sio_cmd_info,
            #elif __DEVICE_USING_DIO
                ._cmd_info   = &nsc_x2_cmd_info,
            #elif __DEVICE_USING_QIO
                ._cmd_info   = &toshiba_x4_cmd_info,
            #endif
        #endif
    },
    {//This is for Default
        .man_id              = MID_TOSHIBA,
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
            ._cmd_info       = VZERO,
            ._model_info     = VZERO,
        #else
            ._ecc_encode     = toshiba_ecc_encode,
            ._ecc_decode     = toshiba_ecc_decode,
            ._reset          = nsu_reset_spi_nand_chip,
            ._cmd_info       = &nsc_sio_cmd_info,
            ._model_info     = &snaf_ode_model,
        #endif
    },
};
#endif // CONFIG_SPI_NAND_FLASH_INIT_FIRST

__SECTION_RUNTIME void
toshiba_ecc_encode(u32_t ecc_ability, void *dma_addr, void *fake_ptr_cs)
{
    return;
}


__SECTION_RUNTIME s32_t
toshiba_ecc_decode(u32_t ecc_ability, void *dma_addr, void *fake_ptr_cs)
{
	struct {
		union {
			u8_t bit_cnt:4;
			u8_t worst_sec:4;
		} f;
		u8_t v;
	} ecc_max_err_report;
	const u8_t fubar = 0x0f;
	const u8_t ecc_maximum_bit_flip_count_report_reg = 0x30;
	const u32_t cs = 0;

	ecc_max_err_report.v = nsu_get_feature_reg(cs, ecc_maximum_bit_flip_count_report_reg) >> 4;

	return (ecc_max_err_report.f.bit_cnt != fubar)?
		ecc_max_err_report.f.bit_cnt:
		(ECC_CTRL_ERR|ecc_max_err_report.f.worst_sec);
}

__SECTION_INIT_PHASE u32_t
toshiba_read_id(u32_t cs)
{
    u32_t w_io_len = IO_WIDTH_LEN(SIO_WIDTH,CMR_LEN(2));
    u32_t r_io_len = IO_WIDTH_LEN(SIO_WIDTH,CMR_LEN(2));
    u32_t ret = nsu_read_spi_nand_id(cs, 0, w_io_len, r_io_len);
    return ((ret>>16)&0xFFFF);
}

#ifdef CONFIG_SPI_NAND_FLASH_INIT_FIRST
__SECTION_INIT_PHASE spi_nand_flash_info_t *
probe_toshiba_spi_nand_chip(void)
{
    nsu_reset_spi_nand_chip(0);

    #ifndef NSU_DRIVER_IN_ROM
    udelay(1100);
    #endif

    u32_t rdid = toshiba_read_id(0);
    if(MID_TOSHIBA != ((rdid >>8)&0xFF)) return VZERO;

    u16_t did8 = rdid &0xFF;
    u32_t i;
    for(i=0 ; i<ELEMENT_OF_SNAF_INFO(toshiba_chip_info) ; i++){
        if((toshiba_chip_info[i].dev_id == did8) || (toshiba_chip_info[i].dev_id == DEFAULT_DATA_BASE)){
            #if __DEVICE_REASSIGN
                #if __DEVICE_USING_SIO
                    toshiba_chip_info[i]._cmd_info = _nsu_cmd_info_ptr;
                #elif __DEVICE_USING_DIO
                    toshiba_chip_info[i]._cmd_info = _nsu_dio_cmd_info_ptr;
                #elif __DEVICE_USING_QIO
                    toshiba_chip_info[i]._cmd_info = &toshiba_x4_cmd_info;
                #endif
                toshiba_chip_info[i]._model_info = &nsu_model_info;
                toshiba_chip_info[i]._reset = _nsu_reset_ptr;
                toshiba_chip_info[i]._ecc_encode= toshiba_ecc_encode;
                toshiba_chip_info[i]._ecc_decode= toshiba_ecc_decode;
                toshiba_chip_info[i]._model_info->_page_read_ecc = _nsu_page_read_with_ode_ptr;
                toshiba_chip_info[i]._model_info->_page_write_ecc = _nsu_page_write_with_ode_ptr;
            #endif //__DEVICE_REASSIGN

            nsu_enable_on_die_ecc(0);
            nsu_block_unprotect(0);
            return &toshiba_chip_info[i];
        }
    }
    return VZERO;
}
REG_SPI_NAND_PROBE_FUNC(probe_toshiba_spi_nand_chip);
#endif   // CONFIG_SPI_NAND_FLASH_INIT_FIRST
#ifdef CONFIG_SPI_NAND_FLASH_INIT_REST
int
toshiba_init_rest(spi_nand_flash_info_t * _spi_nand_info)
{
    u32_t cs=1;

    #ifndef NSU_DRIVER_IN_ROM
    extern void udelay(unsigned long usec);
    udelay(1100);
    #endif

    // check ID, cs0 and cs1 should be identical
    u32_t rdid = toshiba_read_id(cs);
    //u32_t cs0_id = (_spi_nand_info->man_id<<8) | _spi_nand_info->dev_id;
    if(((rdid>>8)!=MID_TOSHIBA) || ((rdid&0xFF)!=_spi_nand_info->dev_id))
      { return 0; }

    // reset
    nsu_reset_spi_nand_chip(cs);

    // misc
    nsu_enable_on_die_ecc(cs);
    nsu_block_unprotect(cs);
    return 1;
}
//REG_SPI_NAND_INIT_REST_FUNC(toshiba_init_rest);
#endif // CONFIG_SPI_NAND_FLASH_INIT_REST
