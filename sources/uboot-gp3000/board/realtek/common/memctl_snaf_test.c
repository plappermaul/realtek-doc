#include <common.h>
#include <nand.h>
#include <soc.h>
#include <malloc.h>
#include <spi_nand/spi_nand_blr_util.h>
#include <asm/mipsregs.h>

struct mtd_info *mtd;


#define SIZE_5KB              (5*1024)
#define SNAF_SRC_CHUNK_BUF    (0x80000000)
#define SNAF_CHK_CHUNK_BUF    (SNAF_SRC_CHUNK_BUF + SIZE_5KB)
#define SNAF_ECC_TAG_BUF      (SNAF_CHK_CHUNK_BUF + SIZE_5KB)
#define wbuf ((u32_t *)(SNAF_SRC_CHUNK_BUF))
#define rbuf ((u32_t *)(SNAF_CHK_CHUNK_BUF))
#define eccb ((u32_t *)(SNAF_ECC_TAG_BUF))

u32_t pattern[] = {
    0x5a5aa5a5,
    0xff00ff00,
    0xa55aa55a,
    0x00ff00ff,
    0xa5a5a5a5,
    0x13572468,
    0x5a5a5a5a,
    0x24683579,
    0xace1ace1,
    0xffff0000,
    0x0000ffff,
    0x5555aaaa,
    0xaaaa5555,
    0x0f1e2d3c,
    0x01010101,
    0xFEFEFEFE
};

typedef struct {
	u32_t loops;
    u32_t bbi[8192];
	u32_t start_blk;
	u32_t blk_count;
	u32_t pattern_idx;
	u32_t pattern_assign;
    u32_t test_case;
    u8_t f_reset;
    u8_t f_bad;
} snaf_test_info_t;

#define _cache_flush	(((soc_t *)(0x9fc00020))->bios).dcache_writeback_invalidate_all
#define SET_SEED 0
#define GET_SEED 1
/*
  get_or_set = GET_SEED: get seed
  get_or_set = SET_SEED: set seed
*/
static void __srandom32(u32_t *a1, u32_t *a2, u32_t *a3, u32_t get_or_set)
{
    static int s1, s2, s3;
    if(GET_SEED==get_or_set){
        *a1=s1;
        *a2=s2;
        *a3=s3;
    }else{
        s1 = *a1;
        s2 = *a2;
        s3 = *a3;
    }
}

static u32_t __random32(void)
{
#define TAUSWORTHE(s,a,b,c,d) ((s&c)<<d) ^ (((s <<a) ^ s)>>b)
    u32_t s1, s2, s3;
    __srandom32(&s1, &s2, &s3, GET_SEED);

    s1 = TAUSWORTHE(s1, 13, 19, 4294967294UL, 12);
    s2 = TAUSWORTHE(s2, 2, 25, 4294967288UL, 4);
    s3 = TAUSWORTHE(s3, 3, 11, 4294967280UL, 17);

    __srandom32(&s1, &s2, &s3, SET_SEED);

    return (s1 ^ s2 ^ s3);
}


#define BLOCK_OFFSET    (6)
#define ROW_ADDR(b, p)  ((b<<BLOCK_OFFSET)|p)

#define BUF_RST_VAL     (0xCAFEBEEF)
#define START_BLOCK     (0)

#define MAX_PAGE_SIZE_PER_U32 ((4096+128)/4)

#define PATTERN1 ((pattern[(i&0xF)]<<(b%13))|(pattern[(i&0xF)]>>(32-(b%13))))
#define PATTERN2 (pattern[(i&0xF)])
#define PATTERN3 (ROW_ADDR(b, p))
#define PATTERN4 (((ROW_ADDR(b, p)&0xFF)<<24)|ROW_ADDR(b, p))


u32_t seed_ref;
static u32_t generate_patterns(u32_t pattern_idx, u32_t b, u32_t p, u32_t psize, u32_t bbi_idx)
{
#define SEED1   ((seed_ref*(b+1))&0xffffff)
#define SEED2   (seed_ref+b)
#define SEED3   (seed_ref-b)

    u32_t i, ra, rb, rc, v;

    // Clear write/read buffer
    memset(wbuf, 0xFF, SIZE_5KB);
    memset(rbuf, 0x00, SIZE_5KB);

    switch(pattern_idx) {
        case 0:
        for(i=0; i<psize; i++) *(wbuf+i) = pattern[(i&0xF)];
        for(i=0; i<psize; i++) {
            if(pattern[(i&0xF)] != *(wbuf+i)) {
                printf("EE: PATTERN0 write to addr(0x%x) 0x%x!=0x%x\n", (u32_t)(wbuf+i), *(wbuf+i), pattern[(i&0xF)]); return 1;}
        }
        break;

        case 1:
            for(i=0; i<psize; i++) *(wbuf+i)=PATTERN1;
            for(i=0; i<psize; i++){
               if(PATTERN1 != *(wbuf+i)) {puts("\nEE: PATTERN1 check failed!\n"); return 1;}
            }
            break;
        case 2:
            for(i=0; i<psize; i++) *(wbuf+i)=PATTERN2;
            for(i=0; i<psize; i++){
               if(PATTERN2 != *(wbuf+i)) {puts("\nEE: PATTERN2 check failed!\n"); return 1;}
            }
            break;
        case 3:
            for(i=0; i<psize; i++) *(wbuf+i)=PATTERN3;
            for(i=0; i<psize; i++){
               if(PATTERN3 != *(wbuf+i)) {puts("\nEE: PATTERN3 check failed!\n"); return 1;}
            }
            break;
        case 4:
            for(i=0; i<psize; i++) *(wbuf+i)=PATTERN4;
            for(i=0; i<psize; i++){
               if(PATTERN4 != *(wbuf+i)) {puts("\nEE: PATTERN4 check failed!\n"); return 1;}
            }
            break;
        case 5:
            ra=SEED1;rb=SEED2;rc=SEED3;
            __srandom32(&ra, &rb, &rc, SET_SEED);
            for(i=0; i<psize; i++) *(wbuf+i) = __random32();

            // verify random patterns
            __srandom32(&ra, &rb, &rc, SET_SEED);
            for(i=0; i<psize; i++) {
                v = __random32();
                if(v!= *(wbuf+i)) {
                    printf("EE: ramdom pattern write to addr(0x%x) 0x%x!=0x%x\n", (u32_t)(wbuf+i), *(wbuf+i), v);
                    return 1;
                }
            }
            break;
    }

    // Reset bad block indicator to ALL_ONE
    wbuf[bbi_idx] = 0xFFFFFFFF;
    return 0;
}

static u32_t snaf_pio_raw_test(spi_nand_flash_info_t *fi, snaf_test_info_t *ti)
{
    if (VZERO == fi) return 1;
    seed_ref = read_c0_count();

    u32_t i, b, p=0;  // block, page(chunk) and column address
    u32_t sb = ti->start_blk;
    u32_t nb = ti->start_blk + ti->blk_count;
    u32_t np = SNAF_NUM_OF_PAGE_PER_BLK(fi);
    u32_t psize = (SNAF_PAGE_SIZE(fi) + SNAF_SPARE_SIZE(fi)) >> 2;        // per_u32_pattern = psize / 4
    puts("II: do spi-nand pio test... write... ");

    _cache_flush();
    for(b=sb; b<nb; b++) {
        // bad block check
        if (1==ti->bbi[b]) continue;

        printf("BLK%04d\b\b\b\b\b\b\b", b);

        // block erase, 128KB
        nasu_block_erase(ROW_ADDR(b, 0));

        for(p=0; p<np; p++){
            // Generate  & verify pattern
            if (generate_patterns(ti->pattern_idx, b, p, psize, (SNAF_PAGE_SIZE(fi)/4))) return 1;

            // pattern write
            nasu_pio_write(wbuf, psize*4, ROW_ADDR(b, p), 0);
        }
    }
    puts("verify... ");
    _cache_flush();
    for(b=sb; b<nb; b++) {
        // bad block check
        if (1==ti->bbi[b]) continue;

        printf("BLK%04d\b\b\b\b\b\b\b", b);


        for(p=0; p<np; p++) {
            // pattern generate
            if (generate_patterns(ti->pattern_idx, b, p, psize, (SNAF_PAGE_SIZE(fi)/4))) return 1;

            // read page/chunk/column
            nasu_pio_read(rbuf, psize*4, ROW_ADDR(b, p), 0);

            for(i=0; i<psize; i++){
                if(*(wbuf+i) != *(rbuf+i)){
                    printf("\nEE: PAGE%04d,COL%04d,data:0x%08x != pattern:0x%08x\n", p, i<<2, *(wbuf+i), *(rbuf+i));
                    return 1;
                }
            }
        }
    }
    puts("done       \n");
    return 0;
}

static u32_t snaf_dma_raw_test(spi_nand_flash_info_t *fi, snaf_test_info_t *ti)
{
    if (VZERO == fi) return 1;
    seed_ref = read_c0_count();

    u32_t b, p;
    u32_t i;
    u32_t psize = SNAF_PAGE_SIZE(fi) + SNAF_SPARE_SIZE(fi);   // 2048 + 64 Bytes
    u32_t sb = ti->start_blk;
    u32_t nb = ti->start_blk+ti->blk_count;
    u32_t np = SNAF_NUM_OF_PAGE_PER_BLK(fi); //fi->_num_page_per_block;
    psize = SNAF_PAGE_SIZE(fi) >> 2; // for loop use

    printf("II: do spi-nand dma test ... write... ");

    _cache_flush();
    for(b=sb; b<nb; b++) {
        printf("BLK%04d\b\b\b\b\b\b\b", b);

        // bad block check
        if (1==ti->bbi[b]) continue;

        // rbuf block erase
        nasu_block_erase(ROW_ADDR(b, 0));

        for(p=0; p<np; p++) {
            // generate random patterns
            if (generate_patterns(ti->pattern_idx, b, p, psize, (SNAF_PAGE_SIZE(fi)/4))) return 1;

            nasu_page_write(wbuf, ROW_ADDR(b, p));
        }
    }
    puts("verify... ");
    _cache_flush();
    for(b=sb; b<nb; b++) {
        // bad block check
        if (1==ti->bbi[b]) continue;
        printf("BLK%04d\b\b\b\b\b\b\b", b);

        for(p=0; p<np; p++) {
            // generate random patterns
            if (generate_patterns(ti->pattern_idx, b, p, psize, (SNAF_PAGE_SIZE(fi)/4))) return 1;

            // dma read
            nasu_page_read(rbuf, ROW_ADDR(b, p));

            // data verify
            for(i=0; i<psize; i++) {
                if(*(wbuf+i)!= *(rbuf+i)) {
                    printf("\nEE: double confirm, BLK%04d,PAGE%04d,COL%04d, dst(addr:0x%x)0x%x != src(addr:0x%x)0x%x\n",
                            b, p, i<<2, (u32_t)(wbuf+i), *(wbuf+i), (u32_t)(rbuf+i), *(rbuf+i));
                    return 1;
                }
            }
        }
    }
    puts("done       \n");
    return 0;
}

static u32_t snaf_pio_dma_raw_cross_test(spi_nand_flash_info_t *fi, snaf_test_info_t *ti)
{
    if (VZERO == fi) return 1;
    seed_ref = read_c0_count();

    u32_t b, p, i;
    u32_t psize, sb, nb, np;

    psize = (SNAF_PAGE_SIZE(fi) + SNAF_SPARE_SIZE(fi)) >> 2;
    sb = ti->start_blk;
    nb = ti->start_blk+ti->blk_count;
    np = SNAF_NUM_OF_PAGE_PER_BLK(fi);

    puts("II: do dma/pio raw cross test... ");

    puts("dma w...");
    // do dma write and pio read back
    for(b=sb; b<nb; b++) {
        printf("BLK%04d\b\b\b\b\b\b\b", b);
        if (1==ti->bbi[b]){printf("(BBI:%d) ", b); continue;}

        nasu_block_erase(ROW_ADDR(b, 0));
        for(p=0; p < np; p++) {
             // generate and verify patterns
             if (generate_patterns(ti->pattern_idx, b, p, psize, (SNAF_PAGE_SIZE(fi)/4))) return 1;

            // dma write
            nasu_page_write(wbuf, ROW_ADDR(b, p));
        }
    }
    puts("pio r...");
    for(b=sb; b<nb; b++) {
        printf("BLK%04d\b\b\b\b\b\b\b", b);
        if (1==ti->bbi[b]){printf("(BBI:%d) ", b); continue;}

        for(p=0; p < np; p++) {
            if (generate_patterns(ti->pattern_idx, b, p, psize, (SNAF_PAGE_SIZE(fi)/4))) return 1;

            nasu_pio_read(rbuf, psize*4, ROW_ADDR(b, p), 0);
            for(i=0; i<psize; i++) {
                if( *(wbuf+i) != *(rbuf+i) ) {
                    printf("\nEE: dw-pr, PAGE%04d,COL%04d, data:0x%08x != pattern:0x%08x\n", p, i<<2, *(rbuf+i), *(wbuf+i));
                    return 1;
                }
            }
        }
    }

    // do pio write and dma read back
    puts("pio w...");
    for(b=sb; b<nb; b++) {
        printf("BLK%04d\b\b\b\b\b\b\b", b);
        if (1==ti->bbi[b]){printf("(BBI:%d) ", b); continue;}

        nasu_block_erase(ROW_ADDR(b, 0));
        for(p=0; p < np; p++) {
             // generate and verify patterns
             if (generate_patterns(ti->pattern_idx, b, p, psize, (SNAF_PAGE_SIZE(fi)/4))) return 1;

            // pio write
            nasu_pio_write(wbuf, psize*4, ROW_ADDR(b, p), 0);
        }
    }
    puts("dma r...");
    for(b=sb; b<nb; b++) {
        printf("BLK%04d\b\b\b\b\b\b\b", b);
        if (1==ti->bbi[b]){printf("(BBI:%d) ", b); continue;}
        for(p=0; p < np; p++) {
            // generate and verify patterns
            if (generate_patterns(ti->pattern_idx, b, p, psize, (SNAF_PAGE_SIZE(fi)/4))) return 1;

            nasu_page_read(rbuf, ROW_ADDR(b, p));

            for(i=0; i<psize; i++) {
                if(*(wbuf+i)!= *(rbuf+i)) {
                    printf("\nEE: pw-dr, PAGE%04d,COL%04d, dst(addr:0x%x)0x%x != pat(addr:0x%x)0x%x\n",
                            p, i<<2, (u32_t)(rbuf+i), *(rbuf+i), (u32_t)(wbuf+i), *(wbuf+i));
                    return 1;
                }
            }
        }
    }
    puts("...done       \n");
    return 0;
}

static u32_t snaf_scan_block_wr_with_ecc_test(spi_nand_flash_info_t *fi, snaf_test_info_t *ti)
{
    if (VZERO == fi) return 1;
    seed_ref = read_c0_count();

    u32_t sb = ti->start_blk;
    u32_t nb = ti->start_blk+ti->blk_count;
    u32_t np = SNAF_NUM_OF_PAGE_PER_BLK(fi);

    u32_t b, p, i;
    u32_t psize = (SNAF_PAGE_SIZE(fi) + SNAF_SPARE_SIZE(fi)) >> 2;
    u32_t vsize = (SNAF_PAGE_SIZE(fi) + SNAF_OOB_SIZE(fi)) >> 2;   //The verify size without syndrome
    int ret;

    printf("II: do %s ... ", __FUNCTION__);

    //1. DMA Wirte whole chip
    puts("dma w...");
    for(b=sb; b<nb; b++) {
        printf("BLK%04d\b\b\b\b\b\b\b", b);
        if (1==ti->bbi[b]){printf("(BBI:%d) ", b); continue;}

        nasu_block_erase(ROW_ADDR(b, 0));
        for(p=0; p < np; p++) {
            //Generate pattern
            if (generate_patterns(ti->pattern_idx, b, p, psize, (SNAF_PAGE_SIZE(fi)/4))) return 1;

            nasu_page_write_ecc(wbuf, ROW_ADDR(b, p), eccb);
        }
    }

    //2. PIO Read whole chip
    puts("pio r...");
    for(b=sb; b<nb; b++) {
        printf("BLK%04d\b\b\b\b\b\b\b", b);
        if (1==ti->bbi[b]){printf("(BBI:%d) ", b); continue;}

        for(p=0; p < np; p++) {
            //Generate pattern & Reset read buf to a known value
            if (generate_patterns(ti->pattern_idx, b, p, psize, (SNAF_PAGE_SIZE(fi)/4))) return 1;

            // pio read
            nasu_pio_read(rbuf, psize*4, ROW_ADDR(b, p), 0);
            ret= nasu_ecc_decode(rbuf, eccb);
            if(IS_ECC_DECODE_FAIL(ret)){
                printf("\nEE: dw-pr, BLK%04d,PAGE%04d: ecc_decode fail (0x%x)\n", b,p,ret);
                continue;
            }

            for(i=0; i<vsize; i++) {
                if( *(wbuf+i) != *(rbuf+i) ) {
                    printf("\nEE: dw-pr, PAGE%04d,COL%04d,data(0x%08x):0x%08x != pattern(0x%08x):0x%08x\n",p,i<<2,(u32_t)(rbuf+i),*(rbuf+i),(u32_t)(wbuf+i),*(wbuf+i));
                    return 1;
                }
            }
        }
    }


    //3. ECC & PIO Write whole chip
    puts("pio w...");
    for(b=sb; b<nb; b++) {
        printf("BLK%04d\b\b\b\b\b\b\b", b);
        if (1==ti->bbi[b]){printf("(BBI:%d) ", b); continue;}

        nasu_block_erase(ROW_ADDR(b, 0));
        for(p=0; p < np; p++) {
            //Generate pattern
            if (generate_patterns(ti->pattern_idx, b, p, psize, (SNAF_PAGE_SIZE(fi)/4))) return 1;
            //printf("\nII: addr=0x%x: blk(%d), pge(%d), pat(0x%x)", ROW_ADDR(b, p),b,p, pattern);

            nasu_ecc_encode(wbuf, eccb);
            nasu_pio_write(wbuf, psize*4, ROW_ADDR(b, p), 0);
        }
    }
    puts("dma r...");
    //4. DMA Read whole chip
    for(b=sb; b<nb; b++) {
        printf("BLK%04d\b\b\b\b\b\b\b", b);
        if (1==ti->bbi[b]){printf("(BBI:%d) ", b); continue;}

        for(p=0; p < np; p++) {
            //Generate pattern & //Reset read buf to a known value
            if (generate_patterns(ti->pattern_idx, b, p, psize, (SNAF_PAGE_SIZE(fi)/4))) return 1;

            //printf("\nII: addr=0x%x: blk(%d), pge(%d), pat(0x%x)", ROW_ADDR(b, p),b,p, pattern);

            ret = nasu_page_read_ecc(rbuf, ROW_ADDR(b, p), eccb);
            if(IS_ECC_DECODE_FAIL(ret)){
                printf("\nEE: pw-dr, BLK%04d,PAGE%04d: ecc_decode fail (0x%x)\n", b,p,ret);
                continue;
            }

            // Check page read pattern
            for(i=0; i<vsize; i++) {
                if(*(wbuf+i) != *(rbuf+i)) {
                    printf("\nEE: pw-dr, PAGE%04d,COL%04d ,dst(0x%x):0x%x != pat(0x%x):0x%x\n", p, i<<2,(u32_t)(rbuf+i),*(rbuf+i),(u32_t)(wbuf+i),*(wbuf+i));
                    return 1;
                }
            }
        }
    }
    puts("...done       \n");
    return 0;
}

static u32_t snaf_ecc_normal_test(spi_nand_flash_info_t *fi, snaf_test_info_t *ti)
{
    if (VZERO == fi) return 1;
    seed_ref = read_c0_count();

    u32_t b, p, i;
    u32_t psize, sb, nb, np;
    psize = (SNAF_PAGE_SIZE(fi)) >> 2;
    sb = ti->start_blk;
    nb = ti->start_blk+ti->blk_count;
    np = SNAF_NUM_OF_PAGE_PER_BLK(fi);

    puts("II: do ecc normal test... write... ");
    for(b=sb; b<nb; b++) {
        // bad block check
        if (1==ti->bbi[b]) continue;
        printf("BLK%04d\b\b\b\b\b\b\b", b);

        // generate patterns
        if (generate_patterns(ti->pattern_idx, 0, 0, psize, (SNAF_PAGE_SIZE(fi)/4))) return 1;

        nasu_block_erase(ROW_ADDR(b, 0));

        for(p=0; p < np; p++) {
            nasu_page_write_ecc(wbuf, ROW_ADDR(b, p), eccb);
        }
    }

    puts(" verify... ");
    for(b=sb; b<nb; b++) {
        // bad block check
        if (1==ti->bbi[b]) continue;
        printf("BLK%04d\b\b\b\b\b\b\b", b);

        // generate patterns
        if (generate_patterns(ti->pattern_idx, 0, 0, psize, (SNAF_PAGE_SIZE(fi)/4))) return 1;

        for(p=0; p < np; p++) {
            // data check
            u32_t ret = nasu_page_read_ecc(rbuf, ROW_ADDR(b, p), eccb);
            if(IS_ECC_DECODE_FAIL(ret)){
                printf("\nEE: pw-dr, BLK%04d,PAGE%04d: ecc_decode fail (0x%x)\n", b,p,ret);
                continue;
            }

            // verify data
            for(i=0; i<psize; i++) {
                if(*(rbuf+i)!= *(wbuf+i)) {
                    printf("EE: BLK%04d, PAGE%04d, read data 0x%x!=0x%x, ecc(0x%x)\n",
                            b, p, *(rbuf+i), *(wbuf+i), ret);
                    return 1;
                }
            }

        }
    }
    puts("done       \n");

    return 0;
}

u32_t snaf_ecc_trick_test(spi_nand_flash_info_t *fi, snaf_test_info_t *ti)
{
    u32_t b, p, s, i;
    int ret;
    u32_t psize, sb, nb, np;
    u32_t buf;
    u32_t p_buf[] = {0x1FFFFF1F, 0xFBEC3FFF, 0xFFFFC47F, 0xFBEF7BEE, 0xBEB6F7EF, 0x7BB6F7EF, 0xE1FE1FFF, 0xDBF7EEFD };

    psize = (SNAF_PAGE_SIZE(fi)) >> 2;
    sb = ti->start_blk;
    nb = ti->start_blk+ti->blk_count;
    np = SNAF_NUM_OF_PAGE_PER_BLK(fi); //fi->_num_page_per_block;

    puts("II: do ecc trick test... ");
    for(b=sb; b<nb; b++) {
        // bad block check
        if (1==ti->bbi[b]) continue;
        printf("BLK%04d\b\b\b\b\b\b\b", b);

        // generate patterns
        for(i=0; i<psize; i++) {
            *(wbuf+i) = pattern[(i&0xF)];
        }

        // verify patterns
        for(i=0; i<psize; i++) {
            if(pattern[(i&0xF)] != *(wbuf+i)) {
                printf("EE: pattern write to addr(0x%x) 0x%x!=0x%x\n",
                        (u32_t)(wbuf+i), *(wbuf+i), pattern[(i&0xF)]);
                return 1;
            }
        }

        nasu_block_erase(ROW_ADDR(b, 0));

        for(p=0; p < np; p++) {
            nasu_page_write_ecc(wbuf, ROW_ADDR(b, p), eccb);

            // data check
            ret=nasu_page_read_ecc(rbuf, ROW_ADDR(b, p), eccb);
            if(IS_ECC_DECODE_FAIL(ret)){
                printf("\nEE: pw-dr, BLK%04d,PAGE%04d: ecc_decode fail (0x%x)\n", b,p,ret);
                continue;
            }

            // verify data
            for(i=0; i<psize; i++) {
                if(*(rbuf+i)!= *(wbuf+i)) {
                    printf("EE: BLK%04d, PAGE%04d, read data 0x%x!=0x%x, ecc(0x%x)\n",
                            b, p, *(rbuf+i), *(wbuf+i), ret);
                    return 1;
                }
            }

            // lets do something bad
            // pick up sector for pollute
            s = (pattern[(p+b)&0xF]&0x3)*512;
            nasu_pio_read(&buf, 4, ROW_ADDR(b, p), s);
            //printf("DD: before buf=0x%x\n", buf);
            buf = buf & p_buf[(p&0x7)];
            //printf("DD: after buf=0x%x\n", buf);

            nasu_pio_write(&buf, 4, ROW_ADDR(b, p), s);
            buf = 0; // cleanup
            //nasu_pio_read(&buf, 4, ROW_ADDR(b, p), s);

            // data check
            ret=nasu_page_read_ecc(rbuf, ROW_ADDR(b, p), eccb);
            if(IS_ECC_DECODE_FAIL(ret)){
                printf("\nEE: pw-dr, BLK%04d,PAGE%04d: ecc_decode fail (0x%x)\n", b,p,ret);
                continue;
            }
            //printf("DD: chunk read =0x%x\n", *(rbuf+128));

            if(ECC_DECODE_SUCCESS != ret)
                printf("EE: ecc correct error (0x%x)\n", ret);
        }
    }
    puts("done       \n");

    return 0;
}

static u32_t snaf_bad_block_search(spi_nand_flash_info_t *fi, snaf_test_info_t *ti)
{
    u32_t b, nb, sb;
    u8_t buf[4];
    sb = ti->start_blk; // start block
    nb = ti->start_blk+ti->blk_count;

    puts("II: do bad block search: ");

    for(b=sb;b<nb;b++){ //block_num
        // performing erase
        printf("BLK%04d\b\b\b\b\b\b\b", b);
        if(0!=nasu_block_erase(ROW_ADDR(b, 0))) {
            ti->bbi[b]=1;
            continue;
        }
        nasu_pio_read(buf, 4, ROW_ADDR(b, 0), SNAF_PAGE_SIZE(fi));
        if(0xFF != buf[0]) ti->bbi[b]=1;
	}

    for(b=sb; b<nb; b++) {
        if(ti->bbi[b]) printf("BLK%04d ", b);
    }

    puts("...done\n");
    return 0;
}

enum cmd_error {
    ERR_INVALD_CMD,
    ERR_LOOP_CMD,
    ERR_RANGE_CMD,
    ERR_START_ADDR,
    ERR_SIZE,
};

enum reset_type {
    RESET_NONE=0,
    RESET_UBOOT,
    RESET_CHIP,
};

static int cmd_parsing(int argc, char * const argv[], spi_nand_flash_info_t *fi, snaf_test_info_t *ti)
{
	u32_t i=1, err;

    /* init value */
	while(i<argc) {
		if ('-' == argv[i][0]) {
            if( 0==(strcmp(argv[i],"-l")) || 0==(strcmp(argv[i],"-loops")) ) {
                u32_t loop;
                if((i+1) >= argc) { err=ERR_LOOP_CMD; goto error; }
                loop = simple_strtoul(argv[i+1], NULL, 10);
                if (0==loop) { puts("WW: invalid loop count(reset to 1)\n"); loop=1; }
                ti->loops = loop;
                i+=2;
            } else if (0==(strcmp(argv[i],"-r")) || 0==(strcmp(argv[i],"-range")) ) {
                u32_t blk, cnt;
                if((i+2) >= argc) { err=ERR_RANGE_CMD; goto error; }
                blk = simple_strtoul(argv[i+1], NULL, 10);
                cnt = simple_strtoul(argv[i+2], NULL, 10);
                printf("DD: input range %d %d\n", blk, cnt);
                if(0==blk || blk >= SNAF_NUM_OF_BLOCK(fi)) { err=ERR_START_ADDR; goto error; }
                if(0==cnt || cnt >= SNAF_NUM_OF_BLOCK(fi)) { err=ERR_SIZE; goto error; }
                if((blk+cnt) > SNAF_NUM_OF_BLOCK(fi)) { err=ERR_START_ADDR; goto error; }
                ti->start_blk=blk;
                ti->blk_count=cnt;
                i+=3;
            } else if(0==strcmp(argv[i],"-reset")) {
                ti->f_reset = RESET_UBOOT;
                i++;
            } else if(0==strcmp(argv[i],"-reset_all")){
                ti->f_reset = RESET_CHIP;
                i++;
            } else if(0==strcmp(argv[i],"-bad")){
                ti->f_bad = 1;
                i++;
            } else if((0==(strcmp(argv[i],"-tc"))) || (0==(strcmp(argv[i],"-test_case"))) ){
                ti->test_case = simple_strtoul(argv[i+1], NULL, 10);
                i+=2;
            } else if((0==(strcmp(argv[i],"-pat"))) || (0==(strcmp(argv[i],"-pattern_idx"))) ){
                ti->pattern_idx = simple_strtoul(argv[i+1], NULL, 10);
                ti->pattern_assign = 1;
                i+=2;
            } else {
                printf("WW: unknown command \"%s\" ignored.\n", argv[i]);
                i++;
            }
        } else {
            err = ERR_INVALD_CMD; goto error;
        }
	}

    if (0==ti->blk_count) { puts("EE: input error\n"); return 1;}

    return 0;
error:
    printf("EE: incomplete commands (type: %d)\n", err);
    return -1;
}

static int snaf_test(spi_nand_flash_info_t *fi, snaf_test_info_t *ti)
{
    u32_t pat_idx;

    for (pat_idx=0; pat_idx<6; pat_idx++) {
        if ((ti->pattern_assign) && (ti->pattern_idx != pat_idx)) continue;
        printf("II: pattern index = %d\n", pat_idx);

        switch(ti->test_case) {
            case 0:
                if (0!= snaf_ecc_normal_test(fi, ti)
                    || 0!=snaf_pio_raw_test(fi, ti)
                    || 0!=snaf_dma_raw_test(fi, ti)
                    || 0!=snaf_pio_dma_raw_cross_test(fi, ti)
                    || 0!=snaf_ecc_trick_test(fi, ti))
                {puts("EE: spi-nand flash test failed\n"); return 1;}
                break;

            case 1:
                if (0!= snaf_ecc_normal_test(fi, ti))
                {puts("EE: spi-nand flash test failed\n"); return 1;}
                break;

            case 2:
                if (0!=snaf_pio_raw_test(fi, ti))
                {puts("EE: spi-nand flash test failed\n"); return 1;}
                break;

            case 3:
                if (0!=snaf_dma_raw_test(fi, ti))
                {puts("EE: spi-nand flash test failed\n"); return 1;}
                break;

            case 4:
                if (0!=snaf_pio_dma_raw_cross_test(fi, ti))
                {puts("EE: spi-nand flash test failed\n"); return 1;}
                break;

            case 5:
                if (0!=snaf_ecc_trick_test(fi, ti))
                {puts("EE: spi-nand flash test failed\n"); return 1;}
                break;

            case 6:
                if (0!=snaf_scan_block_wr_with_ecc_test(fi, ti)){
                    puts("EE: snaf_scan_block_wr_with_ecc_test failed\n");
                    return 1;
                }
                break;
        }
    }

    return 0;
}

int do_snaf_test (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	mtd = get_nand_dev_by_index(0);
	if (!mtd)
		return 1;

    u32_t i=1;
	struct nand_chip *chip = mtd_to_nand(mtd);
    spi_nand_flash_info_t *fi = (spi_nand_flash_info_t *)chip->priv;
    snaf_test_info_t ti={0};
    ti.loops = 1;

    if (0!=cmd_parsing(argc, argv, fi, &ti)) return 1;
        printf("II: spi_nand test info:\n\tstart: BLK%04d, end: BLK%04d\n",
                ti.start_blk, ti.start_blk+ti.blk_count-1);
        printf("\tloop: %d, reset type: %s\n\tbad block search only: %c\n",
                ti.loops, ti.f_reset?(RESET_UBOOT==ti.f_reset?"reset":"reset_all"):"none", ti.f_bad?'y':'n');
        printf("\tall_testcase: %c\n\t",
                ti.test_case?'n':'y');

    snaf_bad_block_search(fi, &ti);
    if (ti.f_bad) return 0;

    while(i<=ti.loops) {
        printf("II: #%d test\n", i++);
        if(snaf_test(fi, &ti)) {return -1;}
    }
    if(RESET_UBOOT == ti.f_reset){
        do_reset (NULL, 0, 0, NULL);
    } else if (RESET_CHIP ==  ti.f_reset){
        //WDOG whole chip reset
        REG32(0xb8003268) = 0;
        REG32(0xb8003268) = 0x80000000;
    }
    return 0;
}

U_BOOT_CMD(
        msnaf_test, 10, 1, do_snaf_test,
        "msnaf_test  - do spi-nand flash test. ",
        "-r/-range <start block> <block count> [-l/-loops <test loops>] [-ac/-all_case] [-reset/-reset_all] [-bad]\n"\
        "    => do spi-nand flash test.\n" \
        "    => [-ac/-all_case]: Set '1' to do all test cases, or only do 'Block Scan Pattern' case."
);
