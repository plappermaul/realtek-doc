#include <nand_spi/ecc_ctrl.h>
#include <util.h>
#include <uart/uart.h>

nand_spi_flash_info_t bch_snaf_info SECTION_PARAMETERS;
#define SRC_CHUNK_BUFFER    (0xA0000000)
#define CHK_CHUNK_BUFFER    (0xA1000000)
#define MT_ECC_BUFFER       (0xA0001000)

#define src_page_buf ((void*)SRC_CHUNK_BUFFER)
#define chk_page_buf ((void*)CHK_CHUNK_BUFFER)
#define mt_ecc_buf   ((void*)MT_ECC_BUFFER)


#define _u32ptr_src_buf (volatile u32_t *)SRC_CHUNK_BUFFER
#define _u32ptr_chk_buf (volatile u32_t *)CHK_CHUNK_BUFFER
#define _u32ptr_ecc_buf (volatile u32_t *)MT_ECC_BUFFER

#define SECTION_NAND_SPI_MT __attribute__ ((section (".pge_align")))

const u32_t bch_patterns[] = {
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

SECTION_NAND_SPI_MT int data_compare(void *src, void *dst, u32_t len)
{
	u32_t i, cnt=0;
    volatile u32_t *source      = (volatile u32_t *)src;
    volatile u32_t *destination = (volatile u32_t *)dst;
	for(i=0 ; i<(len/4) ; i++){
		if(*(destination+i) != *(source+i)){
			printf("  <<ERROR>> src(0x%08x:0x%08x) != dst(0x%08x:0x%08x)\n",(source+i),*(source+i),(destination+i),*(destination+i));
            cnt++;
            if(cnt >=20) return -1;
		}
	}
	return 0;
}

SECTION_NAND_SPI_MT int ecc_destroy_pattern1(u32_t bch_ability)
{
    int ecc_sts;
    u32_t i, pat_idx;
	u32_t shifts=0, cnt=0;
#define BYTE_PER_SECTOR (512)
	u32_t sec_addr[5] = {
        CHK_CHUNK_BUFFER,
        CHK_CHUNK_BUFFER+BYTE_PER_SECTOR*1,
        CHK_CHUNK_BUFFER+BYTE_PER_SECTOR*2,
        CHK_CHUNK_BUFFER+BYTE_PER_SECTOR*3,
        CHK_CHUNK_BUFFER+BYTE_PER_SECTOR*4
    };
	volatile u32_t *addr0 = (volatile u32_t *)(sec_addr[0]);
	volatile u32_t *addr1 = (volatile u32_t *)(sec_addr[1]);
	volatile u32_t *addr2 = (volatile u32_t *)(sec_addr[2]);
	volatile u32_t *addr3 = (volatile u32_t *)(sec_addr[3]);

    if(12 == bch_ability){
        bch_snaf_info._is_bch12 = 1;
        bch_snaf_info._spare_size = SNAF_MODEL_SPARE_SIZE_128B;
    }
    
    u32_t page_size_with_spare = SNAF_PAGE_SIZE((&bch_snaf_info))+SNAF_SPARE_SIZE((&bch_snaf_info));



    printf("II: Start %s\n",__FUNCTION__);
    printf("    addr0=0x%x / addr1=0x%x / addr2=0x%x / addr3=0x%x\n",addr0,addr1,addr2,addr3);


    for(pat_idx=0 ; pat_idx<(sizeof(bch_patterns)/sizeof(u32_t)); pat_idx++){        
        addr0 = (volatile u32_t *)(sec_addr[0]);
        addr1 = (volatile u32_t *)(sec_addr[1]);
        addr2 = (volatile u32_t *)(sec_addr[2]);
        addr3 = (volatile u32_t *)(sec_addr[3]);


        for(i=0 ; i<(page_size_with_spare/4) ; i++){
            *(_u32ptr_src_buf+i) = bch_patterns[pat_idx];
        }  

        bch_snaf_info._ecc_encode(&bch_snaf_info, src_page_buf, mt_ecc_buf);
        inline_memcpy(chk_page_buf, src_page_buf, page_size_with_spare);
   
    	while(((u32_t)addr0<sec_addr[1])||((u32_t)addr1<sec_addr[2])||((u32_t)addr2<sec_addr[3])||((u32_t)addr3<sec_addr[4])){
    		cnt=0;
    		while((cnt<bch_ability) &&((u32_t)addr0<sec_addr[1]))
    		{
    			shifts=0;
    			while((shifts<32)&&(cnt<bch_ability)){
    				if(((*addr0>>shifts)&0x1)==0x1){
    					*addr0 &= ~(1<<shifts);
    					cnt++;
    				}
    				shifts++;
    			}
    			addr0++;
    		}

    		cnt=0;
    		while((cnt<bch_ability) &&((u32_t)addr1<sec_addr[2]))
    		{
    			shifts=0;
    			while((shifts<32)&&(cnt<bch_ability)){
    				if(((*addr1>>shifts)&0x1)==0x1){
    					*addr1 &= ~(1<<shifts);
    					cnt++;
    				}
    				shifts++;
    			}
    			addr1++;
    		}

    		cnt=0;
    		while((cnt<bch_ability) &&((u32_t)addr2<sec_addr[3]))
    		{
    			shifts=0;
    			while((shifts<32)&&(cnt<bch_ability)){
    				if(((*addr2>>shifts)&0x1)==0x1){
    					*addr2 &= ~(1<<shifts);
    					cnt++;
    				}
    				shifts++;
    			}
    			addr2++;
    		}


    		cnt=0;
    		while((cnt<bch_ability) &&((u32_t)addr3<sec_addr[4]))
    		{
    			shifts=0;
    			while((shifts<32)&&(cnt<bch_ability)){
    				if(((*addr3>>shifts)&0x1)==0x1){
    					*addr3 &= ~(1<<shifts);
    					cnt++;
    				}
    				shifts++;
    			}
    			addr3++;
    		}

            
            ecc_sts = bch_snaf_info._ecc_decode(&bch_snaf_info, chk_page_buf, mt_ecc_buf);
            if(IS_ECC_DECODE_FAIL(ecc_sts)){
                data_compare(src_page_buf, chk_page_buf, page_size_with_spare);
                printf("\n    <EE> BCH%d decode fail 0x%08x (pattern=0x%x)\n",bch_ability,ecc_sts, bch_patterns[pat_idx]);
                SWBREAK();
            }else printf("\r    Decode OK, correct %d bits (pattern=0x%08x)",ecc_sts, bch_patterns[pat_idx]);

        }
    }
	printf("II: %s (%d) Passed\n",__FUNCTION__,__LINE__);
	return 0;
}

SECTION_NAND_SPI_MT void ecc_all_one_test(u32_t bch_ability)
{
    u32_t i;
    int ecc_sts;

    if(12 == bch_ability){
        bch_snaf_info._is_bch12 = 1;
        bch_snaf_info._spare_size = SNAF_MODEL_SPARE_SIZE_128B;
    }
    
    u32_t page_size_with_spare = SNAF_PAGE_SIZE((&bch_snaf_info))+SNAF_SPARE_SIZE((&bch_snaf_info));


    printf("II: Start %s: (BCH %d)\n",__FUNCTION__,bch_ability);
    printf("    page_size_with_spare = %d (0x%x) bytes\n",page_size_with_spare,page_size_with_spare);



    //Step 1: Initialize the src data buffer to 0xFFFFFFFF
    for(i=0 ; i<(page_size_with_spare/4) ; i++){
        *(_u32ptr_src_buf+i) = 0xFFFFFFFF; 
    }

    //Step 2: Initialize the ecc buffer to 0xFFFFFFFF
    for(i=0 ; i<(bch_ability==6?4:7) ; i++){
        *(_u32ptr_ecc_buf+i) = 0xFFFFFFFF; 
    }

    
    //Step 3: ECC decode, ALL_ONE bit should be set
    ecc_sts = bch_snaf_info._ecc_decode(&bch_snaf_info, src_page_buf, mt_ecc_buf);
    if(IS_ECC_DECODE_FAIL(ecc_sts)) {
        printf("\n    <EE> ECC ALL ONE decode fail!!!\n");
        printf("    <EE> ecc_sts = 0x%x\n",ecc_sts);
        #if 1
/*
        for(i=0 ; i<(page_size_with_spare/4) ; i+=4){
            printf("  addr(0x%08x): 0x%08x  0x%08x  0x%08x  0x%08x\n",(_u32ptr_src_buf+i),*(_u32ptr_src_buf+i),*(_u32ptr_src_buf+i+1),*(_u32ptr_src_buf+i+2),*(_u32ptr_src_buf+i+3));
        }
        printf("\n\n\n");
*/
        for(i=0 ; i<(page_size_with_spare/4) ; i++){
            if(*(_u32ptr_src_buf+i) != 0xFFFFFFFF){
                printf("  <EE> addr(0x%08x):0x%08x != 0xFFFFFFFF\n",(_u32ptr_src_buf+i),*(_u32ptr_src_buf+i));
            }
        }

        printf("        ECC buffer(0x%08x): ",(_u32ptr_ecc_buf));
        for(i=0 ; i<7 ; i++){
            printf("0x%08x  ",*(_u32ptr_ecc_buf+i));
        }

        printf("      src_page_buf = 0x%08x\n",(u32_t)src_page_buf);
        printf("      mt_ecc_buf = 0x%08x\n",(u32_t)mt_ecc_buf);
        printf("      REG(0xB801A600) = 0x%08x\n",*(volatile u32_t *)0xB801A600);
        printf("      REG(0xB801A60C) = 0x%08x\n",*(volatile u32_t *)0xB801A60C);
        printf("      REG(0xB801A610) = 0x%08x\n",*(volatile u32_t *)0xB801A610);
        printf("      REG(0xB801A614) = 0x%08x\n",*(volatile u32_t *)0xB801A614);
        #endif
        SWBREAK();
    }
	printf("II: %s (%d) Passed\n",__FUNCTION__,__LINE__);
}

#define ROTATE_1BIT(v) (((v) << 1) | ((v & 0x80000000)>>31));
SECTION_NAND_SPI_MT void bch_encode_decode_test(u32_t bch_ability)
{
    u32_t i, pat_idx;
    int ecc_sts;

    if(12 == bch_ability){
        bch_snaf_info._is_bch12 = 1;
        bch_snaf_info._spare_size = SNAF_MODEL_SPARE_SIZE_128B;
    }
    
    u32_t page_size_with_spare = SNAF_PAGE_SIZE((&bch_snaf_info))+SNAF_SPARE_SIZE((&bch_snaf_info));


    printf("II: Start %s: (BCH %d)\n",__FUNCTION__,bch_ability);
    printf("    page_size_with_spare = %d (0x%x) bytes\n",page_size_with_spare,page_size_with_spare);


    //Step1: BCH_Encode then BCH_Decode: Pattern is rotate_1bit
    for(i=0 ; i<(page_size_with_spare/4) ; i++){
        *(_u32ptr_src_buf+i) = ROTATE_1BIT(((u32_t)(src_page_buf+i))); 
    }  
    bch_snaf_info._ecc_encode(&bch_snaf_info, src_page_buf, mt_ecc_buf);
    ecc_sts = bch_snaf_info._ecc_decode(&bch_snaf_info, src_page_buf, mt_ecc_buf);
    if(IS_ECC_DECODE_FAIL(ecc_sts)){
        printf("\n    <EE> BCH12 decode fail (ecc_sts=0x%08x)\n",ecc_sts);
        SWBREAK();
    }else printf("    Decode OK, correct %d bits (pattern=addr_rotate)\n",ecc_sts);
     

    //Step2: BCH_Encode then BCH_Decode: Pattern is bch_patterns
    for(pat_idx=0 ; pat_idx<(sizeof(bch_patterns)/sizeof(u32_t)); pat_idx++){        
        for(i=0 ; i<(page_size_with_spare/4) ; i++){
            *(_u32ptr_src_buf+i) = bch_patterns[pat_idx]; 
        }  
        bch_snaf_info._ecc_encode(&bch_snaf_info, src_page_buf, mt_ecc_buf);
        ecc_sts = bch_snaf_info._ecc_decode(&bch_snaf_info, src_page_buf, mt_ecc_buf);
        if(IS_ECC_DECODE_FAIL(ecc_sts)){
            printf("\n    <EE> BCH12 decode fail 0x%08x (pattern=0x%x)\n",ecc_sts, bch_patterns[pat_idx]);
            SWBREAK();
        }else printf("\r    Decode OK, correct %d bits (pattern=0x%08x)",ecc_sts, bch_patterns[pat_idx]);
    }
	printf("II: %s (%d) Passed\n",__FUNCTION__,__LINE__);
}


SECTION_NAND_SPI_MT void ecc_controller_test(void)
{
    inline_memcpy(&bch_snaf_info, _nand_spi_info, sizeof(nand_spi_flash_info_t));

    printf("II: Starting %s ......\n",__FUNCTION__);

    ecc_all_one_test(6);
//    ecc_all_one_test(12);
    bch_encode_decode_test(6);
    bch_encode_decode_test(12);
    ecc_destroy_pattern1(6);
    ecc_destroy_pattern1(12);

}


REG_INIT_FUNC(ecc_controller_test, 33);

