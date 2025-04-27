#include <nand_spi/nand_spi_util.h>
#include <nand_spi/ecc_ctrl.h>
#include <init_define.h>
#include <lib/lzma/tlzma.h>
#include <dram/memcntlr.h>

u32_t ecc_uboot_to_dram(void) 
{
    int ecc_sts;
    u32_t start_blk = 1;
    u32_t blk_pge_addr;
    u32_t max_blk_pge = SNAF_NUM_OF_BLOCK(_nand_spi_info)*SNAF_NUM_OF_PAGE_PER_BLK(_nand_spi_info);  
    u32_t chunk_size = SNAF_PAGE_SIZE(_nand_spi_info);
    u32_t need_cpy;              //Denote which chunk need to be copied to DRAM
    u32_t total_uboot_chunks = 0;//OOB
    u32_t total_uboot_copies = 0;//OOB
    u32_t cnt_valid_chunk = 0;   //Used to count how many chunks be loaded successfully
    u32_t cnt_chunk = 0;         //Used to count how many chunks be checked
    u32_t cnt_uboot   = 0;       //Used to count if there is another UBOOT can be read
    u32_t dma_addr;
    u8_t chunk_valid_table[(MAX_UBOOT_SIZE/MAX_PAGE_BUF_SIZE)];   //Used to denote which chunk is readed
    oob_t *oob = (oob_t *)oob_buffer;

    inline_bzero(chunk_valid_table, sizeof(chunk_valid_table)); 
    for(blk_pge_addr=(start_blk<<6); blk_pge_addr<max_blk_pge; blk_pge_addr++){
        need_cpy = 0;
       
        if(0 == total_uboot_chunks){
            ecc_sts = nsu_page_read_ecc(_nand_spi_info, page_buffer, blk_pge_addr, ecc_buffer);

            if(ECC_CTRL_ERR != ecc_sts){
                if(SIGNATURE_UBOOT != oob->signature){
                    cnt_chunk = 0;
                    continue;
                }
                total_uboot_chunks = oob->total_chunk;
                total_uboot_copies = oob->num_copy;
                
                if(cnt_chunk >= total_uboot_chunks){
                    cnt_uboot = oob->idx_copy+1;
                    cnt_chunk = oob->idx_chunk;
                    if((cnt_uboot == total_uboot_copies) && (cnt_chunk+1 == total_uboot_chunks)) return 0;
                }
                need_cpy = 1;
            }else{ 
                cnt_chunk++;
                continue;
            }
        }else if(0 == chunk_valid_table[cnt_chunk]){
            ecc_sts = nsu_page_read_ecc(_nand_spi_info, page_buffer, blk_pge_addr, ecc_buffer);
            if(ECC_CTRL_ERR != ecc_sts){
                if(SIGNATURE_UBOOT != oob->signature) return 0; 
                need_cpy = 1;
            } 
        }
        if(1 == need_cpy){
            dma_addr = ECC_DECODED_UBOOT_ADDR + (chunk_size*cnt_chunk);
            inline_memcpy(dma_addr, page_buffer, chunk_size);
            chunk_valid_table[cnt_chunk] = 1;
            cnt_valid_chunk++;
            if(cnt_valid_chunk == total_uboot_chunks){                
                dcache_wr_inv_all();
                icache_inv_all();
                return total_uboot_chunks;
            }
        }

        cnt_chunk++;
        if(cnt_chunk == total_uboot_chunks){
            cnt_chunk = 0;
            cnt_uboot = oob->idx_copy+1;
            if(cnt_uboot == total_uboot_copies) return 0;
        }
    }
    return 0;
}

void uboot_bring_up(void)
{
    void *uboot_inflate_addr, *load_addr, *entry_addr, *mcp_src_addr;
    u32_t total_uboot_chunks, mcp_size, comp_type;
    uimage_header_t *uhdr_info;

    load_addr = entry_addr = uboot_inflate_addr = (void *)UBOOT_DECOMP_ADDR;
    uhdr_info = (uimage_header_t *)ECC_DECODED_UBOOT_ADDR;
    total_uboot_chunks= ecc_uboot_to_dram();

    if(0 == total_uboot_chunks){
        puts("EE: Loading Uboot Fail!!\n");
        while(1);
    }

    if(UIMG_MAGIC_NUM != uhdr_info->ih_magic){
        /* U-boot image header does NOT exists, Copy the RAW data from 7MB-DDR to 0MB-DDR */
        mcp_src_addr = (void *)ECC_DECODED_UBOOT_ADDR;
        mcp_size = total_uboot_chunks*SNAF_PAGE_SIZE(_nand_spi_info);
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
    
    dcache_wr_inv_all();
    icache_inv_all();
    ((fpv_t *)entry_addr)();
    puts("EE: Should not run to here... \n");  
    while(1); // should never return
}

REG_INIT_FUNC(uboot_bring_up, 38);

