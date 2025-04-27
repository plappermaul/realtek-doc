#include <spi_nand/spi_nand_util.h>
#include <ecc/ecc_ctrl.h>
#include <init_define.h>
#include <lib/lzma/tlzma.h>
#include <dram/memcntlr_util.h>
#include <osc.h>

s32_t ecc_uboot_to_dram(void) 
{
    int ecc_sts;
    u32_t start_blk = 1;
    u32_t blk_pge_addr;
    u32_t max_blk_pge = SNAF_NUM_OF_BLOCK(_spi_nand_info)*SNAF_NUM_OF_PAGE_PER_BLK(_spi_nand_info);  
    u32_t page_size = SNAF_PAGE_SIZE(_spi_nand_info);
    u32_t need_cpy;              //Denote which chunk need to be copied to DRAM
    u32_t total_uboot_pages = 0;//OOB
    u32_t total_uboot_copies = 0;//OOB
    u32_t cnt_valid_page = 0;   //Used to count how many chunks be loaded successfully
    u32_t page_idx = 0;         //Used to count how many chunks be checked
    u32_t uboot_idx= 0;       //Used to count if there is another UBOOT can be read
    u32_t dma_addr;
    u8_t page_valid_table[(MAX_UBOOT_SIZE/page_size)];   //Used to denote which chunk is readed
    oob_t *oob = (oob_t *)oob_buffer;

    inline_bzero(page_valid_table, sizeof(page_valid_table)); 
    for(blk_pge_addr=(start_blk<<6); blk_pge_addr<max_blk_pge; blk_pge_addr++){
        need_cpy = 0;
        ecc_sts = nsu_page_read_ecc(_spi_nand_info, page_buffer, blk_pge_addr, ecc_buffer);        
        if(ECC_CTRL_ERR == ecc_sts) continue;
        page_idx = oob->idx_chunk;

        if(0 == total_uboot_pages){
            if(SIGNATURE_UBOOT != oob->signature) continue;
            total_uboot_pages  = oob->total_chunk;
            total_uboot_copies = oob->num_copy;                
            uboot_idx = oob->idx_copy;
            if(((uboot_idx+1) == total_uboot_copies) && ((page_idx+1) == total_uboot_pages)) return -1;
            need_cpy = 1;
        }else if(0 == page_valid_table[page_idx]){
            if(SIGNATURE_UBOOT != oob->signature) continue; 
            need_cpy = 1;
        }

        if(1 == need_cpy){
            #ifdef DBG_PRINT_SNAF_BRINGUP
                printf("DD: blk[%d], page[%2d], (0x08%x)\n",SNAF_BLOCK_ADDR(blk_pge_addr),SNAF_PAGE_ADDR(blk_pge_addr),blk_pge_addr);
            #endif
            dma_addr = ECC_DECODED_UBOOT_ADDR + (page_size*page_idx);
            inline_memcpy(dma_addr, page_buffer, page_size);
            page_valid_table[page_idx] = 1;
            cnt_valid_page++;
            if(cnt_valid_page == total_uboot_pages){                
                dcache_wr_inv_all();
                icache_inv_all();
                return total_uboot_pages;
            }
        }
        
        if(((uboot_idx+1) == total_uboot_copies) && ((page_idx+1) == total_uboot_pages)) return -2;
    }
    return -3;
}


void uboot_bring_up(void)
{
    void *uboot_inflate_addr, *load_addr, *entry_addr, *mcp_src_addr;
    u32_t mcp_size, comp_type;
    uimage_header_t *uhdr_info;
    otto_soc_context_t otto_sc;

    load_addr = entry_addr = uboot_inflate_addr = (void *)UBOOT_DECOMP_ADDR;
    uhdr_info = (uimage_header_t *)ECC_DECODED_UBOOT_ADDR;
    s32_t total_uboot_pages= ecc_uboot_to_dram();

    if(total_uboot_pages < 0){
        printf("EE: Loading Uboot Fail (%d)!!\n",total_uboot_pages);
        while(1);
    }

//    if((UIMG_MAGIC_NUM != uhdr_info->ih_magic) && (MAGIC_UBOOT_2011 != uhdr_info->ih_magic)) {
    /* Masked by MSP. Doesn't check MagiC number */	
    if(0) {
        /* U-boot image header does NOT exists, Copy the RAW data from 7MB-DDR to 0MB-DDR */
        mcp_src_addr = (void *)ECC_DECODED_UBOOT_ADDR;
        mcp_size = total_uboot_pages*SNAF_PAGE_SIZE(_spi_nand_info);
        printf("II: Copying %dKB deflated U-Boot (%p -> %p) ... ",mcp_size>>10, mcp_src_addr, uboot_inflate_addr);
        inline_memcpy(uboot_inflate_addr, mcp_src_addr, mcp_size);
    }else{
        /* Copy the U-boot based on the information in header structure */
        mcp_src_addr = (void *)(uhdr_info+1);
        mcp_size = uhdr_info->ih_size;
        entry_addr = (void *)uhdr_info->ih_ep;           
        load_addr = (void *)uhdr_info->ih_load;
        comp_type = uhdr_info->ih_comp;
        
        printf("II: U-boot Magic Number is 0x%x\n",uhdr_info->ih_magic);       
        if(comp_type == UIH_COMP_LZMA){
            printf("II: Inflating U-Boot (%p -> %p)... ",mcp_src_addr, load_addr);
            s32_t res;
            if(ISTAT_GET(cal) == MEM_CAL_OK){
                res = lzma_decompress((u8_t *)mcp_src_addr, (u8_t *)uboot_inflate_addr, &mcp_size);
            }else{
                res = lzma_chsp_jump((u8_t *)mcp_src_addr, (u8_t *)uboot_inflate_addr, &mcp_size, NEW_STACK_AT_DRAM);
            }

            if(res != DECOMPRESS_OK) {
                printf("\nEE: decompress failed: %d\n", res);
                while (1);
            }     
        }else if(comp_type == UIH_COMP_NONE){
            printf("II: Copying %dKB inflated U-Boot (%p -> %p)... ",mcp_size>>10, mcp_src_addr, load_addr);
            inline_memcpy(uboot_inflate_addr, mcp_src_addr, mcp_size);
        }else{
            puts("EE: Unsupported U-Boot format, copy it as uncompressed.\n");
            while(1);
        }
    }
    
    if(uboot_inflate_addr != load_addr){
        inline_backward_memcpy(load_addr, uboot_inflate_addr, mcp_size);
    }
    puts("OK\n");
    puts("II: Starting U-boot... \n");

    osc_init(&otto_sc);

    dcache_wr_inv_all();
    icache_inv_all();
    ((fpv_u32_t *)(entry_addr))((u32_t)&otto_sc);
    puts("EE: Should not run to here... \n");  
    while(1); // should never return
}

REG_INIT_FUNC(uboot_bring_up, 38);

