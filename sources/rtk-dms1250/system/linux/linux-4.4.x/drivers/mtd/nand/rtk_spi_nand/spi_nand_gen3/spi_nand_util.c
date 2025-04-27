#include <common/rt_type.h>
#include <spi_nand/spi_nand_util.h>
#include <ecc/ecc_ctrl.h>
#include <util.h>
#ifndef SECTION_RECYCLE
    #define SECTION_RECYCLE
#endif

#ifndef CONFIG_UNDER_UBOOT
// linker script defines following symbol
extern const char lma_mapping_start SECTION_RO;

static u16_t plr_vmpage_table[PLR_MAP_TABLE_SIZE] SECTION_SDATA;
u32_t tlb_start_idx SECTION_SDATA;
u32_t chunk_per_tlb_page SECTION_SDATA;
u32_t otto_plr_page_buf_addr SECTION_SDATA;
#endif // CONFIG_UNDER_UBOOT

SECTION_RECYCLE int
nsu_probe(spi_nand_flash_info_t *info,
    const spi_nand_probe_t **probe_func_start,
    const spi_nand_probe_t **probe_func_end) {
    // result will be store in *info and its following structure
    spi_nand_flash_info_t *snaf_info=VZERO;

    const spi_nand_probe_t **probe_snf = probe_func_start;
    while (probe_snf != probe_func_end) {
        snaf_info = (*probe_snf)();
        if(snaf_info != VZERO) {
            spi_nand_cmd_info_t *local_cmd_info   = info->_cmd_info;
            spi_nand_model_info_t *local_model_info = info->_model_info;
            memcpy(info,snaf_info,sizeof(spi_nand_flash_info_t));
            memcpy(local_cmd_info, snaf_info->_cmd_info, sizeof(spi_nand_cmd_info_t));
            memcpy(local_model_info, snaf_info->_model_info, sizeof(spi_nand_model_info_t));
            info->_cmd_info = local_cmd_info;
            info->_model_info = local_model_info;
#ifndef CONFIG_UNDER_UBOOT
            u32_t page_size = SNAF_PAGE_SIZE(info);
            if(page_size == MAX_PAGE_BUF_SIZE) otto_plr_page_buf_addr = OTTO_PLR_PAGE_BUFFER;
            else otto_plr_page_buf_addr = (OTTO_PLR_OOB_BUFFER-page_size);
#endif // CONFIG_UNDER_UBOOT
            return 0;
        }
        ++probe_snf;
    }

    return -1;
}

#ifndef CONFIG_UNDER_UBOOT
SECTION_RECYCLE static int
is_plr_first_load_part(plr_first_load_layout_t *fl_buf)
{
    if(nsu_ecc_engine_action(((u32_t)_spi_nand_info->_ecc_ability), fl_buf->data0, fl_buf->oob0, 0) == ECC_CTRL_ERR) return ECC_CTRL_ERR;
    if(nsu_ecc_engine_action(((u32_t)_spi_nand_info->_ecc_ability), fl_buf->data1, fl_buf->oob1, 0) == ECC_CTRL_ERR) return ECC_CTRL_ERR;
    if(PLR_FL_GET_SIGNATURE(fl_buf) == SIGNATURE_PLR_FL) return 1;
    return 0;
}

SECTION_RECYCLE int
nsu_init(void)
{
    chunk_per_tlb_page = TLB_PAGE_SIZE / SNAF_PAGE_SIZE(_spi_nand_info);
    tlb_start_idx=((u32_t)&lma_mapping_start)/PLR_FL_PAGE_USAGE;

    int   ret;
    u32_t blk_pge_addr;
    u32_t max_blk_pge_num = SNAF_NUM_OF_BLOCK(_spi_nand_info) * SNAF_NUM_OF_PAGE_PER_BLK(_spi_nand_info);
    u32_t total_num_of_chunk = 0;   //OOB
    u8_t fill_table;
    u32_t fill_index = 0;
    u32_t cnt_pge = 0;
    u32_t cnt_valid_chunk = 0;
    oob_t *oob = oob_buffer;
    plr_first_load_layout_t *fl_buf=(plr_first_load_layout_t *)page_buffer;

    memset((void*)plr_vmpage_table, 0xFF, sizeof(plr_vmpage_table));
    for(blk_pge_addr=0; blk_pge_addr<max_blk_pge_num; blk_pge_addr++, cnt_pge++) {
        fill_table = 0;

        ret = nsu_page_read_ecc(_spi_nand_info, page_buffer, blk_pge_addr, ecc_buffer);

        if(1 == is_plr_first_load_part(fl_buf)){
            fill_index = PLR_FL_GET_PAGE_INDEX(fl_buf);
            if(0xFFFF == plr_vmpage_table[fill_index]) fill_table = 1;
        }else if(!IS_ECC_DECODE_FAIL(ret)){
            //Read OOB for each block
            if(SIGNATURE_PLR != oob->signature) continue;
            if(0 == total_num_of_chunk) total_num_of_chunk = oob->total_chunk;
            fill_index = oob->idx_chunk;
            if(0xFFFF == plr_vmpage_table[fill_index]) fill_table = 1;
        }

        if(1 == fill_table){
            plr_vmpage_table[fill_index] = (u16_t)blk_pge_addr;
            if(++cnt_valid_chunk == total_num_of_chunk) return 0;
        }

        if((cnt_pge >= total_num_of_chunk)&&(0 != total_num_of_chunk)){
            cnt_pge %= total_num_of_chunk;
            if((oob->idx_copy+1) == oob->num_copy) return -2;
        }
    }

    return -1;
}

SECTION_UNS_TEXT int
nsu_logical_page_read(void *data, u32_t page_num)
{
    int ret;
    u32_t i;
    u32_t spi_nand_chunk_size = SNAF_PAGE_SIZE(_spi_nand_info);
    u32_t index = page_num * chunk_per_tlb_page + tlb_start_idx;
    u16_t *mapping=plr_vmpage_table + index;

    for (i=0 ; i<chunk_per_tlb_page; ++i){
        if((*mapping) == 0xFFFF) break;
        ret = nsu_page_read_ecc(_spi_nand_info, page_buffer, (u32_t)(*mapping), ecc_buffer);
        if(IS_ECC_DECODE_FAIL(ret)) return -1;

        mass_copy(data, page_buffer, spi_nand_chunk_size);
        ++mapping;
        data += spi_nand_chunk_size;
    }
    dcache_wr_inv_all();
    icache_inv_all();
    return 0;
}
#endif // CONFIG_UNDER_UBOOT
