#include <util.h>
#include <nand_spi/nand_spi_ctrl.h>
#include <nand_spi/nand_spi_common.h>
#include <nand_spi/nand_spi_util.h>
#include <nand_spi/nand_spi_micron.h>
#include <nand_spi/ecc_ctrl.h>
#include <stddef.h>


#ifdef NSU_DRIVER_IN_ROM
    #include <arch.h>
    #define __SECTION_INIT_PHASE SECTION_NAND_SPI
    #define __SECTION_INIT_PHASE_DATA SECTION_RO
    #define __SECTION_RUNTIME SECTION_NAND_SPI
    #define __SECTION_RUNTIME_DATA SECTION_RO
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

#if __DEVICE_USING_DIO
__SECTION_INIT_PHASE_DATA nand_spi_cmd_info_t micron_dio_cmd_info;
#elif __DEVICE_USING_QIO
__SECTION_INIT_PHASE_DATA nand_spi_cmd_info_t micron_qio_cmd_info;
#endif


#define _dummy_ecc_encode ((nand_spi_ecc_encode_t*)&always_return_zero)

__SECTION_INIT_PHASE_DATA nand_spi_model_info_t micron_model = {
    ._page_read = micron_page_read,
    ._page_write = micron_page_write,
    ._page_read_ecc = micron_page_read_with_ecc_decode,
    ._page_write_ecc = micron_page_write_with_ecc_encode,
};

// code implement
__SECTION_INIT_PHASE_DATA 
nand_spi_flash_info_t micron_chip_info[] = {
    {
        .man_id              = MID_MICRON, 
        .dev_id              = DID_M78A,
        ._num_block          = SNAF_MODEL_NUM_BLK_1024,
        ._num_page_per_block = SNAF_MODEL_NUM_PAGE_64,
        ._page_size          = SNAF_MODEL_PAGE_SIZE_2048B,
        ._spare_size         = SNAF_MODEL_SPARE_SIZE_128B,
        ._oob_size           = SNAF_MODEL_OOB_SIZE(24),
        ._is_bch12           = 1,
        
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
            ._model_info     = &micron_model,
        #elif __DEVICE_USING_DIO
            ._ecc_encode     = ecc_encode_bch,
            ._ecc_decode     = ecc_decode_bch,
            ._reset          = nsu_reset_spi_nand_chip,
            ._cmd_info       = &micron_dio_cmd_info,
            ._model_info     = &micron_model,
        #elif __DEVICE_USING_QIO
            ._ecc_encode     = ecc_encode_bch,
            ._ecc_decode     = ecc_decode_bch,
            ._reset          = nsu_reset_spi_nand_chip,
            ._cmd_info       = &micron_qio_cmd_info,
            ._model_info     = &micron_model,
        #endif
    },
};

__SECTION_INIT_PHASE void 
micron_block_unprotect(void)
{
    u32_t feature_addr=0xA0;
    u32_t value = 0x00;
    nsu_set_feature_reg(feature_addr, value);
}

__SECTION_INIT_PHASE int 
micron_ondie_ecc_decode_status(nand_spi_flash_info_t *ecc_drv, void *dma_addr, void *p_eccbuf)
{
    int eccsts = ((nsu_get_feature_reg(0xC0)>>4)&0x7);
    if(eccsts == 2){
        return ECC_CTRL_ERR;
    }else{
        return eccsts;
    }
    return eccsts;

}
__SECTION_RUNTIME void 
micron_page_read(nand_spi_flash_info_t *info, void *dma_addr, u32_t blk_pge_addr)
{
    nsu_page_read(info, dma_addr, blk_pge_addr);
    inline_memcpy(dma_addr+0x846, dma_addr+0x804, 12);
    inline_memcpy(dma_addr+0x866, dma_addr+0x810, 8);
}

__SECTION_RUNTIME s32_t 
micron_page_write(nand_spi_flash_info_t *info, void *dma_addr, u32_t blk_pge_addr)
{
    inline_memcpy(dma_addr+0x846, dma_addr+0x804, 12);
    inline_memcpy(dma_addr+0x866, dma_addr+0x810, 8);
    return nsu_page_write(info, dma_addr, blk_pge_addr);
}

__SECTION_RUNTIME s32_t
micron_page_write_with_ecc_encode(nand_spi_flash_info_t *info, void *dma_addr, u32_t blk_pge_addr, void *p_eccbuf)
{
    return nsu_page_write(info, dma_addr, blk_pge_addr);
}
__SECTION_RUNTIME int 
micron_page_read_with_ecc_decode(nand_spi_flash_info_t *info, void *dma_addr, u32_t blk_pge_addr, void *p_eccbuf)
{
    nsu_page_read(info,dma_addr,blk_pge_addr);
    return micron_ondie_ecc_decode_status(info, NULL, NULL);
}

__SECTION_INIT_PHASE u32_t 
micron_nsu_read_id(void)
{
    u32_t dummy_byte = 0x00;
    u32_t w_io_len = IO_WIDTH_LEN(SIO_WIDTH,CMR_LEN(2));
    u32_t r_io_len = IO_WIDTH_LEN(SIO_WIDTH,CMR_LEN(2));
    u32_t ret = nsu_read_spi_nand_id(dummy_byte, w_io_len, r_io_len);
    return ((ret>>16)&0xFFFF);
}

__SECTION_INIT_PHASE nand_spi_flash_info_t *
probe_micron_spi_nand_chip(void)
{
    nsu_reset_spi_nand_chip();
    u32_t rdid = micron_nsu_read_id();
    if(MID_MICRON != (rdid >>8)) return VZERO;
    u16_t did = rdid&0xFF;
    u32_t i;
    for(i=0 ; i<sizeof(micron_chip_info) ; i++){
        if(micron_chip_info[i].dev_id == did){
            #ifdef __DEVICE_REASSIGN
            micron_model._pio_read= nsu_model_info._pio_read;
            micron_model._pio_write = nsu_model_info._pio_write;
            micron_model._block_erase = nsu_model_info._block_erase;
            micron_model._wait_nand_spi_ready = nsu_model_info._wait_nand_spi_ready;
            micron_chip_info[i]._ecc_encode = _dummy_ecc_encode;
            micron_chip_info[i]._ecc_decode = micron_ondie_ecc_decode_status;
            micron_chip_info[i]._cmd_info = _nsu_cmd_info_ptr;
            micron_chip_info[i]._reset = _nsu_reset_ptr;
            #endif
            micron_block_unprotect();
            return &micron_chip_info[i];
        }
    }
    return VZERO;
}

REG_SPI_NAND_PROBE_FUNC(probe_micron_spi_nand_chip);

