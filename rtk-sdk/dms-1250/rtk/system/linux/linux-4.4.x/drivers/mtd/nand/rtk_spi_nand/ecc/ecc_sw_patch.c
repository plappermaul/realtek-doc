#include <common/rt_type.h>
#include <string.h>
#include <init_define.h>
#include <soc.h>
#include <ecc/ecc_ctrl.h>


SECTION_UNS_TEXT s32_t
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

SECTION_UNS_TEXT s32_t
ecc_engine_action(u32_t ecc_ability, void *dma_addr, void *p_eccbuf, u32_t is_encode)
{
    u32_t is_bch12=0;
    if(12 == ecc_ability) is_bch12=1;

    u32_t addr_to_chk = ((u32_t)p_eccbuf)+BCH_TAG_SIZE+2;
    u32_t *chkpoint_uncache = (u32_t*)((u32_t)addr_to_chk|0x20000000);
    if(is_encode){
        chkpoint_uncache[0]=ALL_FF;
        chkpoint_uncache[1]=ALL_FF;
    }

    RMOD_ECCFR(ecc_cfg, is_bch12);
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

SECTION_UNS_TEXT void
ecc_controller_encode(u32_t ecc_ability, void *dma_addr, void *p_eccbuf)
{
    u32_t encode_addr = (u32_t)dma_addr;
    u32_t page_size = BCH_SECTOR_SIZE*BCH_SECTS_PER_2K_PAGE;
    u8_t *tag_addr = (u8_t *)(encode_addr + page_size);
    u8_t *syn_addr = (u8_t *)(tag_addr + BCH_TAG_SIZE*BCH_SECTS_PER_2K_PAGE);

    u32_t syn_size;
    if(12 == ecc_ability) syn_size = BCH12_SYNDROME_SIZE;
    else syn_size = BCH6_SYNDROME_SIZE;

    // 1. Cache Flush ......
    dcache_wr_inv((u32_t)dma_addr, (u32_t)(dma_addr+page_size));

    u32_t j;
    for(j=0 ; j<BCH_SECTS_PER_2K_PAGE ; j++, encode_addr+=BCH_SECTOR_SIZE, tag_addr+=BCH_TAG_SIZE, syn_addr+=syn_size){
        //2. Coypy Tag & Cache Flush ......
        inline_memcpy(p_eccbuf, tag_addr, BCH_TAG_SIZE);
        dcache_wr_inv((u32_t)p_eccbuf, (u32_t)(p_eccbuf+BCH_TAG_SIZE-4));

        //3. ECC encode
        ecc_engine_action(ecc_ability, (void*)encode_addr, p_eccbuf, 1);

        //4. Store Tag & Syndrome
        inline_memcpy(tag_addr, p_eccbuf, BCH_TAG_SIZE);
        inline_memcpy(syn_addr, (p_eccbuf+BCH_TAG_SIZE), syn_size);
        dcache_wr_inv((u32_t)tag_addr, (u32_t)(tag_addr+BCH_TAG_SIZE));
        dcache_wr_inv((u32_t)syn_addr, (u32_t)(syn_addr+syn_size));
    }
}

SECTION_RECYCLE void init_ecc_sw_patch(void)
{
    plr_spi_nand_flash_info._ecc_encode = ecc_controller_encode;
}
REG_INIT_FUNC(init_ecc_sw_patch, 5);

