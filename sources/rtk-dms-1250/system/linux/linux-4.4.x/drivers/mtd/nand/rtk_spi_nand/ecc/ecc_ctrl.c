#include <common/rt_type.h>
#if defined(__LUNA_KERNEL__)
#include <naf_kernel_util.h>
#include <ecc_ctrl.h>
#include <ecc_struct.h>
#else
#include <util.h>
#define DMA_CACHE_WBACK_INV(start_addr, end_addr) _lplr_basic_io.dcache_writeback_invalidate_range(start_addr, end_addr)
#include <ecc/ecc_ctrl.h>
#include <ecc/ecc_struct.h>
#endif


SECTION_ECC_CTRL s32_t
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

SECTION_ECC_CTRL s32_t
ecc_engine_action(u32_t ecc_ability, void *dma_addr, void *p_eccbuf, u32_t is_encode)
{
    u32_t is_bch12=0;
    if(12 == ecc_ability) is_bch12=1;

    u32_t addr_to_chk = ((u32_t)p_eccbuf)+BCH_TAG_SIZE+2;
    u32_t *chkpoint_uncache = (u32_t*)((u32_t)addr_to_chk|0x20000000);
    if(is_encode){
        chkpoint_uncache[0]=ALL_FF;
        chkpoint_uncache[1]=ALL_FF;
        mb();
    }

    ECC_FEATURE_SET(is_bch12);
    SET_ECC_DMA_TAG_ADDR(PADDR(p_eccbuf));
    SET_ECC_DMA_START_ADDR(PADDR(dma_addr));
    ECC_KICKOFF(is_encode);

    if(is_encode){
        NOPX5();
    }
    WAIT_ECC_CTRLR_RDY();

    if(is_encode){
        int upper_bound=10;
        while(((upper_bound--)>0)&&(chkpoint_uncache[0]==ALL_FF)&&(chkpoint_uncache[1]==ALL_FF)){
            NOPX10();
        }
    }

    return is_encode?0:check_ecc_ctrl_status();
}

SECTION_ECC_CTRL void
ecc_encode_bch_sector(u32_t ecc_ability, void *dma_addr, void *p_eccbuf, u32_t sector_per_page)
{
    u32_t encode_addr = (u32_t)dma_addr;
    u32_t page_size = BCH_SECTOR_SIZE*sector_per_page;
    u8_t *tag_addr = (u8_t *)(encode_addr + page_size);
    u8_t *syn_addr = (u8_t *)(tag_addr + BCH_TAG_SIZE*sector_per_page);
    u32_t syn_size = ((12 == ecc_ability)?BCH12_SYNDROME_SIZE:BCH6_SYNDROME_SIZE);

    // 1. Cache Flush ......
    DMA_CACHE_WBACK_INV((u32_t)dma_addr, (u32_t)(dma_addr+page_size-1));

    u32_t j;
    for(j=0 ; j<sector_per_page ; j++, encode_addr+=BCH_SECTOR_SIZE, tag_addr+=BCH_TAG_SIZE, syn_addr+=syn_size){
        //2. Coypy Tag & Cache Flush ......
        inline_memcpy(p_eccbuf, tag_addr, BCH_TAG_SIZE);
        mb();
        DMA_CACHE_WBACK_INV((u32_t)p_eccbuf, (u32_t)(p_eccbuf+BCH_TAG_SIZE-1));

        //3. ECC encode
        ecc_engine_action(ecc_ability, (void*)encode_addr, p_eccbuf, 1);

        //4. Store Tag & Syndrome
        inline_memcpy(tag_addr, p_eccbuf, BCH_TAG_SIZE);
        mb();
        inline_memcpy(syn_addr, (p_eccbuf+BCH_TAG_SIZE), syn_size);
        mb();
        DMA_CACHE_WBACK_INV((u32_t)tag_addr, (u32_t)(tag_addr+BCH_TAG_SIZE-1));
        DMA_CACHE_WBACK_INV((u32_t)syn_addr, (u32_t)(syn_addr+syn_size-1));
    }
}

SECTION_ECC_CTRL s32_t
ecc_decode_bch_sector(u32_t ecc_ability, void *dma_addr, void *p_eccbuf, u32_t sector_per_page)
{
    u32_t decode_addr = (u32_t)dma_addr;
    u32_t page_size = BCH_SECTOR_SIZE*sector_per_page;
    u8_t *tag_addr = (u8_t *)(decode_addr+page_size);
    u8_t *syn_addr = (u8_t *)(tag_addr + (BCH_TAG_SIZE*sector_per_page));
    u32_t syn_size = ((12 == ecc_ability)?BCH12_SYNDROME_SIZE:BCH6_SYNDROME_SIZE);

    // 1. Cache Flush ......
    u32_t ecc_buf_size = BCH_TAG_SIZE+syn_size;
    u32_t page_size_with_spare = page_size + (ecc_buf_size*sector_per_page);
    DMA_CACHE_WBACK_INV(decode_addr,(decode_addr+page_size_with_spare-1));

    u32_t j, ecc_error_sector=0, sector_correct_bits=0;
    for(j=0 ; j<sector_per_page ; j++, decode_addr+=BCH_SECTOR_SIZE, tag_addr+=BCH_TAG_SIZE, syn_addr+=syn_size){
        //2. Coypy Tag & Syndrome and then doing cache flush ......
        inline_memcpy(p_eccbuf, tag_addr, BCH_TAG_SIZE);
        mb();
        inline_memcpy((p_eccbuf+BCH_TAG_SIZE), syn_addr, syn_size);
        mb();
        DMA_CACHE_WBACK_INV((u32_t)p_eccbuf, (u32_t)(p_eccbuf+ecc_buf_size-1));

        //3. ECC decode
        s32_t ret=ecc_engine_action(ecc_ability, (void*)decode_addr, p_eccbuf, 0);
        if (ret == (s32_t)ECC_CTRL_ERR) {
            ecc_error_sector |= (1<<j);
        } else if (ret > (s32_t)sector_correct_bits) {
            sector_correct_bits = ret;
        }

        //4. Store Tag
        inline_memcpy(tag_addr, p_eccbuf, BCH_TAG_SIZE);
        mb();
        DMA_CACHE_WBACK_INV((u32_t)tag_addr, (u32_t)(tag_addr+BCH_TAG_SIZE-1));
    }
    return ((ecc_error_sector==0)?(sector_correct_bits):(ECC_CTRL_ERR|ecc_error_sector));
}

SECTION_ECC_CTRL void
ecc_encode_bch(u32_t ecc_ability, void *dma_addr, void *p_eccbuf)
{
    if(ECC_USE_ODE == ecc_ability) return;
    ecc_encode_bch_sector(ecc_ability, dma_addr, p_eccbuf, BCH_SECTS_PER_2K_PAGE);
}

SECTION_ECC_CTRL s32_t
ecc_decode_bch(u32_t ecc_ability, void *dma_addr, void *p_eccbuf)
{
    return ecc_decode_bch_sector(ecc_ability, dma_addr, p_eccbuf, BCH_SECTS_PER_2K_PAGE);
}

#if defined(__LUNA_KERNEL__)
ecc_encode_t *_ecc_encode_ptr = ecc_encode_bch;
ecc_decode_t *_ecc_decode_ptr = ecc_decode_bch;
ecc_engine_t *_ecc_engine_act_ptr = ecc_engine_action;
#endif

#ifndef CONFIG_UNDER_UBOOT
symb_fdefine(ECC_BCH_ENCODE_FUNC, ecc_encode_bch);
symb_fdefine(ECC_BCH_DECODE_FUNC, ecc_decode_bch);
symb_fdefine(ECC_ENGINE_ACTION_FUNC, ecc_engine_action);
#endif // CONFIG_UNDER_UBOOT
