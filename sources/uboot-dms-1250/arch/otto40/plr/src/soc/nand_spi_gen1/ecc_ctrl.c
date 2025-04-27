#include <util.h>
#include <nand_spi/ecc_ctrl.h>

SECTION_NAND_SPI s32_t 
check_ecc_ctrl_status(void)
{
    if(RFLD_ECSR(ecer)){
        if(RFLD_ECSR(all_one)){
            return ECC_DECODE_ALL_ONE;
        }else{
            return ECC_CTRL_ERR;
        }
    }
    return RFLD_ECSR(eccn);
}

SECTION_NAND_SPI s32_t 
ecc_engine_action(void *dma_addr, void *p_eccbuf, s32_t is_bch12, s32_t is_encode) 
{
    RMOD_ECCFR(ecc_cfg, is_bch12);       
    SET_ECC_DMA_TAG_ADDR(PADDR(p_eccbuf));
    SET_ECC_DMA_START_ADDR(PADDR(dma_addr));
    ECC_KICKOFF(is_encode);
    WAIT_ECC_CTRLR_RDY();
    return is_encode?0:check_ecc_ctrl_status();
}

SECTION_NAND_SPI void 
ecc_encode_bch(nand_spi_flash_info_t *info, void *dma_addr, void *p_eccbuf)
{
    u32_t encode_addr = (u32_t)dma_addr;
    u32_t page_size = BCH_SECTOR_SIZE*BCH_SECTOR_PER_PAGE;
    u8_t *tag_addr = (u8_t *)(encode_addr + page_size);
    u8_t *syn_addr = (u8_t *)(tag_addr + BCH_TAG_SIZE*BCH_SECTOR_PER_PAGE);

    u32_t syn_size;
    if(info->_is_bch12) syn_size = BCH12_SYNDROME_SIZE;
    else syn_size = BCH6_SYNDROME_SIZE;

    // 1. Cache Flush ......
    _lplr_basic_io.dcache_writeback_invalidate_range((u32_t)dma_addr, (u32_t)(dma_addr+page_size));
  
    u32_t j;
    for(j=0 ; j<BCH_SECTOR_PER_PAGE ; j++, encode_addr+=BCH_SECTOR_SIZE, tag_addr+=BCH_TAG_SIZE, syn_addr+=syn_size){
        //2. Coypy Tag & Cache Flush ......
        inline_memcpy(p_eccbuf, tag_addr, BCH_TAG_SIZE);
        _lplr_basic_io.dcache_writeback_invalidate_range((u32_t)p_eccbuf, (u32_t)(p_eccbuf+BCH_TAG_SIZE));

        //3. ECC encode
        ecc_engine_action((void*)encode_addr, p_eccbuf, info->_is_bch12, 1);

        //4. Store Tag & Syndrome
        inline_memcpy(tag_addr, p_eccbuf, BCH_TAG_SIZE);
        inline_memcpy(syn_addr, (p_eccbuf+BCH_TAG_SIZE), syn_size);
        _lplr_basic_io.dcache_writeback_invalidate_range((u32_t)tag_addr, (u32_t)(tag_addr+BCH_TAG_SIZE));
        _lplr_basic_io.dcache_writeback_invalidate_range((u32_t)syn_addr, (u32_t)(syn_addr+syn_size));
    }
}

s32_t ecc_decode_bch(nand_spi_flash_info_t *info, void *dma_addr, void *p_eccbuf)
{
    u32_t decode_addr = (u32_t)dma_addr;
    u32_t page_size = BCH_SECTOR_SIZE*BCH_SECTOR_PER_PAGE;
    u8_t *tag_addr = (u8_t *)(decode_addr+page_size);
    u8_t *syn_addr = (u8_t *)(tag_addr + (BCH_TAG_SIZE*BCH_SECTOR_PER_PAGE));

    u32_t syn_size;
    if(info->_is_bch12) syn_size = BCH12_SYNDROME_SIZE;
    else syn_size = BCH6_SYNDROME_SIZE;

    
    // 1. Cache Flush ......
    u32_t ecc_buf_size = BCH_TAG_SIZE+syn_size;
    u32_t page_size_with_spare = page_size + (ecc_buf_size*BCH_SECTOR_PER_PAGE);
    _lplr_basic_io.dcache_writeback_invalidate_range(decode_addr,(decode_addr+page_size_with_spare));  

    u32_t j, ecc_error_sector=0, sector_correct_bits=0;
    for(j=0 ; j<BCH_SECTOR_PER_PAGE ; j++, decode_addr+=BCH_SECTOR_SIZE, tag_addr+=BCH_TAG_SIZE, syn_addr+=syn_size){
        //2. Coypy Tag & Syndrome and then doing cache flush ......
    	inline_memcpy(p_eccbuf, tag_addr, BCH_TAG_SIZE);
    	inline_memcpy((p_eccbuf+BCH_TAG_SIZE), syn_addr, syn_size);
    	_lplr_basic_io.dcache_writeback_invalidate_range((u32_t)p_eccbuf, (u32_t)(p_eccbuf+ecc_buf_size));

        //3. ECC decode
        s32_t ret=ecc_engine_action((void*)decode_addr, p_eccbuf, info->_is_bch12, 0);
        if(ret == ECC_CTRL_ERR) ecc_error_sector |= (1<<j);
        else if(ret > sector_correct_bits) sector_correct_bits = ret;

        //4. Store Tag
        inline_memcpy(tag_addr, p_eccbuf, BCH_TAG_SIZE);
        _lplr_basic_io.dcache_writeback_invalidate_range((u32_t)tag_addr, (u32_t)(tag_addr+BCH_TAG_SIZE));
    }
    return ((ecc_error_sector==0)?(sector_correct_bits):(ECC_CTRL_ERR|ecc_error_sector));
}

symb_fdefine(ECC_BCH_ENCODE_FUNC, ecc_encode_bch);
symb_fdefine(ECC_BCH_DECODE_FUNC, ecc_decode_bch);
symb_fdefine(ECC_ENGINE_ACTION_FUNC, ecc_engine_action);

