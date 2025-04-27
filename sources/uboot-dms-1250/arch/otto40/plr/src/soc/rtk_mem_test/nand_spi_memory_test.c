#include <nand_spi/nand_spi_util.h>
#include <nand_spi/nand_spi_ctrl.h>
#include <nand_spi/nand_spi_common.h>
#include <nand_spi/ecc_ctrl.h>
#include <util.h>
#include <uart/uart.h>



nand_spi_flash_info_t mt_snaf_info SECTION_PARAMETERS;
#define SIZE_3KB            (3*1024)
#define SRC_CHUNK_BUFFER    (0xA0000000)
#define SRC_OOB_BUFFER      (SRC_CHUNK_BUFFER + 0x800)
#define CHK_CHUNK_BUFFER    (SRC_CHUNK_BUFFER + SIZE_3KB)
#define CHK_OOB_BUFFER      (CHK_CHUNK_BUFFER + 0x800)
#define MT_ECC_BUFFER       (CHK_CHUNK_BUFFER + SIZE_3KB)

#define src_page_buf ((void*)(SRC_CHUNK_BUFFER))
#define src_oob_buf   ((oob_t*)(SRC_OOB_BUFFER))
#define chk_page_buf ((void*)(CHK_CHUNK_BUFFER))
#define chk_oob_buf   ((oob_t*)(CHK_OOB_BUFFER))
#define mt_ecc_buf    ((void*)(MT_ECC_BUFFER))

#define SECTION_NAND_SPI_MT __attribute__ ((section (".pge_align")))
#define MAX_CHUNK_SIZE (2112)
#define BLOCK_PAGE_ADDR(block_0_1023, page_0_63) ((0x00<<16)|(block_0_1023<<6)|(page_0_63))

const u32_t flash_patterns[] = {
    0x01010101,
    0xfefefefe,
    0x00000000,
    0xff00ff00,
    0x00ff00ff,
    0x0000ffff,
    0xffff0000,
    0xffffffff,
    0x5a5aa5a5,
    0xa5a5a5a5,
    0x55555555,
    0xaaaaaaaa,
    0x01234567,
    0x76543210,
    0x89abcdef,
    0xfedcba98,
};

SECTION_NAND_SPI_MT int data_compare_per_page(int ecc_sts, void *src, void *dst, u32_t blk_num, u32_t page_num)
{
    volatile u32_t *src_adr = (volatile u32_t *)src;
    volatile u32_t *dst_adr = (volatile u32_t *)dst;
    u32_t i, src_val, dst_val, cnt=0;
    int ret = 0;

    if(IS_ECC_DECODE_FAIL(ecc_sts)){
        printf("\n    <error> ECC error 0x%08x (Block %d)(page %d)\n",ecc_sts, blk_num, page_num);
        return -1;
    }else if(0 != ecc_sts){
        printf("\nECC %d bits (Block %d)(page %d)",ecc_sts, blk_num, page_num);
    }

    for(i=0 ; i<(MAX_CHUNK_SIZE/4) ; i++){
        src_val = *(src_adr+i); 
        dst_val = *(dst_adr+i);
        if(dst_val != src_val){
            printf("\n    <error> [blk:%d][page:%d] src(0x%x):0x%8x != dst(0x%x):0x%8x",blk_num,page_num,(src_adr+i),src_val,(dst_adr+i),dst_val);
            ret = -1;
            if(++cnt == 5){
                printf("\n    <error> [blk:%d][page:%d]] More than 5 words error!!!\n",blk_num,page_num);    
                return -1;
            }
        }
    }
    if(-1 == ret) printf("\n");
    return ret;
}

SECTION_NAND_SPI_MT void prepare_src_pattern_per_page(void *src, void *dst, u32_t pattern, u32_t page_size_with_spare)
{
    volatile u32_t *src_adr = (volatile u32_t *)src;
    volatile u32_t *dst_adr = (volatile u32_t *)dst;
    u32_t i;
    for(i=0 ; i<(page_size_with_spare/4) ; i++){
        *(src_adr+i) = pattern; 
        *(dst_adr+i) = 0x00000000;
    }
    *(src_adr+(0x800/4)) = 0xFFFFFFFF; 
}

SECTION_NAND_SPI_MT void page_dma_write_read_with_ecc_test(u32_t blk_start, u32_t blk_end, u32_t pat_start, u32_t pat_end)
{
    int ret=0, ecc_sts=0;
    u32_t blk_idx, page_idx, blk_pge_addr, i, cnt_bbi=0;
    oob_t *src_oob = (oob_t *)src_oob_buf;

    printf("II: Start %s (block %d to %d)\n",__FUNCTION__, blk_start, blk_end);

    for(i=pat_start; i <=pat_end ; i++){
        for(blk_idx=blk_start ; blk_idx<=blk_end ; blk_idx++){
            // 1.1 Block erase
            // 1.2 Check Block erase status
            // 1.3 Check BBI
            blk_pge_addr = BLOCK_PAGE_ADDR(blk_idx,0);
            if(-1 == nsu_block_erase(&mt_snaf_info, blk_pge_addr)){
                printf("\n    <error> Block %d earse fail\n",blk_idx);
                continue;
            }
            ecc_sts = nsu_page_read_ecc(&mt_snaf_info, src_page_buf, blk_pge_addr, mt_ecc_buf);
            if(IS_ECC_DECODE_FAIL(ecc_sts)){
                printf("\n    <error> ECC fail, Block %d BBI unknown\n",blk_idx);
                continue;
            }
            if(0xFFFF != src_oob->bbi){
                printf("\n    <error> Block %d is BAD BLOCK (0x%x)\n",blk_idx,src_oob->bbi);
                if(++cnt_bbi == (blk_end-blk_start+1)){
                    printf("    <error> All blocks are bad block!\n");
                    return;
                }
                continue;
            }

            // 2. Doing Chunk Write/Read between sram and spi nand (ECC)
            u32_t page_size_with_spare = mt_snaf_info._page_size + mt_snaf_info._spare_size;
            for(page_idx=0 ; page_idx<=63 ; page_idx++){ //page
                prepare_src_pattern_per_page((void *)src_page_buf, (void *)chk_page_buf, flash_patterns[i], page_size_with_spare);
                blk_pge_addr = BLOCK_PAGE_ADDR(blk_idx,page_idx);
                ret = nsu_page_write_ecc(&mt_snaf_info, src_page_buf, blk_pge_addr, mt_ecc_buf);
                if(ret != 0) printf("\n    <error> page_write[Block:%04d][page:%02d]",blk_idx, page_idx);

                ecc_sts  = nsu_page_read_ecc(&mt_snaf_info, chk_page_buf, blk_pge_addr, mt_ecc_buf);
                ret = data_compare_per_page(ecc_sts, (void *)src_page_buf, (void *)chk_page_buf, blk_idx, page_idx);
                if(ret == 0) printf("\r    <dma>[Block:%04d][page:%02d][pattern:0x%08x] OK",blk_idx, page_idx, flash_patterns[i]);
            }
        }
    }   
    printf("\nII: %s Passed\n",__FUNCTION__);
}

SECTION_NAND_SPI_MT void pio_write_read_with_ecc_test(u32_t blk_start, u32_t blk_end, u32_t pat_start, u32_t pat_end)
{
    int ret=0, ecc_sts=0;
    u32_t blk_idx, page_idx, blk_pge_addr, i, cnt_bbi=0;
    oob_t *src_oob = (oob_t *)src_oob_buf;

    printf("II: Start %s (block %d to %d)\n",__FUNCTION__, blk_start, blk_end);

    for(i=pat_start; i<=pat_end; i++){
        for(blk_idx=blk_start ; blk_idx<=blk_end ; blk_idx++){
            // 1.1 Block erase
            // 1.2 Check Block erase status
            // 1.3 Check BBI
            blk_pge_addr = BLOCK_PAGE_ADDR(blk_idx,0);
            if(-1 == nsu_block_erase(&mt_snaf_info, blk_pge_addr)){
                printf("\n    <error> Block %d earse fail\n",blk_idx);
                continue;
            }
            ecc_sts = nsu_page_read_ecc(&mt_snaf_info, src_page_buf, blk_pge_addr, mt_ecc_buf);
            if(IS_ECC_DECODE_FAIL(ecc_sts)){
                printf("\n    <error> ECC fail, Block %d BBI fail, ecc_sts 0x%x\n",blk_idx,ecc_sts);
                printf("\n    <error> src_page_buf 0x%x\n",(u32_t)src_page_buf);
                printf("\n    <error> src_oob_buf 0x%x\n",(u32_t)src_oob_buf);
                printf("\n    <error> mt_ecc_buf 0x%x\n",(u32_t)mt_ecc_buf);
                SWBREAK();
                continue;
            }
            if(0xFFFF != src_oob->bbi){
                printf("\n    <error> Block %d is BAD BLOCK (0x%x)\n",blk_idx,src_oob->bbi);
                if(++cnt_bbi == (blk_end-blk_start+1)){
                    printf("    <error> All blocks are bad block!\n");
                    return;
                }
                continue;
            }

            // 2. Doing PIO Write/Read between sram and spi nand (ECC)
            u32_t page_size_with_spare = mt_snaf_info._page_size + mt_snaf_info._spare_size;
            for(page_idx=0 ; page_idx<=63 ; page_idx++){ //page
                prepare_src_pattern_per_page((void *)src_page_buf, (void *)chk_page_buf, flash_patterns[i], page_size_with_spare);
                blk_pge_addr = BLOCK_PAGE_ADDR(blk_idx,page_idx);
                nsu_ecc_encode(&mt_snaf_info, src_page_buf, mt_ecc_buf);
                ret = nsu_pio_write(&mt_snaf_info, src_page_buf, MAX_CHUNK_SIZE, blk_pge_addr, 0x0);
                if(ret != 0) printf("\n    <error> pio_write[Block:%04d][page:%02d]",blk_idx, page_idx);

                nsu_pio_read(&mt_snaf_info, chk_page_buf, MAX_CHUNK_SIZE, blk_pge_addr, 0x0);
                ecc_sts  = nsu_ecc_decode(&mt_snaf_info, chk_page_buf, mt_ecc_buf);
                ret = data_compare_per_page(ecc_sts, (void *)src_page_buf, (void *)chk_page_buf, blk_idx, page_idx);
                if(ret == 0) printf("\r    <PIO>[Block:%04d][page:%02d][pattern:0x%08x] OK",blk_idx, page_idx, flash_patterns[i]);
                else SWBREAK();
            }
        }
    }   
    printf("\nII: %s Passed\n",__FUNCTION__);
}

SECTION_NAND_SPI_MT void spi_nand_flash_memory_test(void)
{
    u32_t blk_start, blk_end, pat_start, pat_end;

    inline_memcpy(&mt_snaf_info, _nand_spi_info, sizeof(nand_spi_flash_info_t));
    mt_snaf_info._cmd_info = _nsu_dio_cmd_info_ptr;

    printf("II: Start %s: (BCH %d) (w_cmd = 0x%02x) (r_cmd = 0x%02x)\n",__FUNCTION__, (mt_snaf_info._is_bch12?12:6), mt_snaf_info._cmd_info->w_cmd, mt_snaf_info._cmd_info->r_cmd);

    blk_start = 100;
    blk_end   = 101;
    pat_start = 0;
    pat_end   = 7;
    pio_write_read_with_ecc_test(blk_start, blk_end, pat_start, pat_end);

    blk_start = 600;
    blk_end   = 601;
    pat_start = 8;
    pat_end   = 15;
    page_dma_write_read_with_ecc_test(blk_start, blk_end, pat_start, pat_end);
}
REG_INIT_FUNC(spi_nand_flash_memory_test, 34);

