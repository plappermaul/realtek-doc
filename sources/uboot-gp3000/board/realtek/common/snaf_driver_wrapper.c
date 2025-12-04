#include <common.h>
#include <soc.h>
#include <nand.h>
#include <osc.h>
#include <spi_nand/spi_nand_blr_util.h>
#include <symb_define.h>

#define IGNORED_FOR_NOW 0

#define SNAF_MIN(x, y) ((y) ^ (((x) ^ (y)) & -((x) < (y))))

#define SNAF_DEBUG_MSG 0
#if (SNAF_DEBUG_MSG == 1)
#    define snaf_dmsg(...) printf(__VA_ARGS__)
#else
#    define snaf_dmsg(...)
#endif

static struct nand_chip nand_chip;

#if CONFIG_USE_BBT_SKIP
#define MAX_BLOCKS (1024<<2)
#ifndef NUM_WORD
#define NUM_WORD(bits)  (((bits)+8*sizeof(u32_t)-1)/(8*sizeof(u32_t)))
#endif

uint8_t bb_skip_table[MAX_BLOCKS];
#define MAX_BAD_BLOCK_NUM 256
int32_t badblocks[MAX_BAD_BLOCK_NUM];

static inline int get_good_block(struct mtd_info *mtd, uint32_t *page_id)
{
    spi_nand_flash_info_t *snaf_info = otto_sc.snaf_info;
    int blk_shift = (nand_chip.phys_erase_shift - nand_chip.page_shift);
    int blk_num = (*page_id)>>blk_shift;
    uint32_t block_count = SNAF_NUM_OF_BLOCK(snaf_info);
    if(blk_num >= block_count) {
	return -1;
    }

    uint32_t skip_num = bb_skip_table[blk_num];
    if ( skip_num == 0xFF) {
	if(block_count - (blk_num+1)>= MAX_BAD_BLOCK_NUM){
	    return -1;
	}
	if (badblocks[block_count - (blk_num+1)] == 0) {
	    return -1;
	}
	if (badblocks[block_count - (blk_num+1)] < 0) {
	    *page_id = ((- badblocks[block_count - (blk_num+1)])<<blk_shift) + (((1<<blk_shift)-1) & (*page_id));
	    snaf_dmsg("get good block return 1, page_id is %x\n", *page_id );
	    return 1;
	}
	*page_id = (badblocks[block_count - (blk_num+1)] <<blk_shift) + (((1<<blk_shift)-1) & (*page_id));
	snaf_dmsg("get good block return 0 in original bad, page_id ix %x\n", *page_id);
	return 0;
    }
    *page_id = ((blk_num + skip_num)<<blk_shift) + (((1<<blk_shift)-1) & (*page_id));
    //printf("II: page_id is %x, block num is %d(%x), skip_num is %d", *page_id, blk_num, blk_num, skip_num);
    //printf("new (%x)\n", ((blk_num + skip_num)<<blk_shift) + (((1<<blk_shift)-1) & (*page_id)));

    return 0;
}
#else
static inline int
get_good_block(struct mtd_info *mtd, uint32_t *page_id)
{
    spi_nand_flash_info_t *snaf_info = otto_sc.snaf_info;
    int blk_shift = (nand_chip.phys_erase_shift - nand_chip.page_shift);
    int blk_num = (*page_id)>>blk_shift;
    uint32_t block_count = SNAF_NUM_OF_BLOCK(snaf_info);

    if(blk_num >= block_count) {
	return -1;
    }

    return 0;
}
#endif

#ifndef CONFIG_ECC_ERR_SHOW_PAGE
#define CONFIG_ECC_ERR_SHOW_PAGE 0
#endif
#if CONFIG_ECC_ERR_SHOW_PAGE
#define SHOW_DATA_LEN 16
static void rtk_show_page(uint32_t blk_pge_addr, uint32_t page_size, uint32_t oobsize, unsigned char *buf);
#endif

#if CONFIG_ECC_ERR_SHOW_PAGE
static void
rtk_show_page(uint32_t blk_pge_addr, uint32_t page_size, uint32_t oobsize, unsigned char *buf)
{
	int i;
	printf("\n ----- Block Page Addr %X ----- \n", blk_pge_addr);
	for ( i = 0; i < page_size+oobsize; i++)
	{
		if (0 == i%SHOW_DATA_LEN){
			printf("\n%04X: ", i);
		}
		printf("%02X ", buf[i]);
	}
	printf("\n--------\n");
	printf("\n");
#if 0
	for ( i = 0; i < oobsize; i++ )
	{
		if (0 == i%SHOW_DATA_LEN){
			printf("\n%04X: ", i);
		}
		printf("%02X", oobsize);
	}
#endif
	printf("\n");
}
#undef SHOW_DATA_LEN
#endif




static uint8_t otto_snaf_read_byte(struct mtd_info *mtd) {
    printf("DD: %s\n", __FUNCTION__);
    return 0;
}

static u16 otto_snaf_read_word(struct mtd_info *mtd) {
    printf("DD: %s\n", __FUNCTION__);
    return 0;
}

static void otto_snaf_write_byte(struct mtd_info *mtd, uint8_t byte) {
    printf("DD: %s\n", __FUNCTION__);
    return;
}

static void otto_snaf_write_buf(struct mtd_info *mtd, const uint8_t *buf, int len) {
    printf("DD: %s\n", __FUNCTION__);
    return;
}

static void otto_snaf_read_buf(struct mtd_info *mtd, uint8_t *buf, int len) {
    printf("DD: %s\n", __FUNCTION__);
    return;
}

static void otto_snaf_select_chip(struct mtd_info *mtd, int chip) {
    snaf_dmsg("DD: %s\n", __FUNCTION__);
    return;
}

static int otto_snaf_block_bad(struct mtd_info *mtd, loff_t ofs) {
    uint32_t blk_pge_addr = (ofs & (~(mtd->erasesize - 1))) >> nand_chip.page_shift;
    int ret = get_good_block(mtd, &blk_pge_addr);
#if CONFIG_USE_BBT_SKIP
    spi_nand_flash_info_t *snaf_info = otto_sc.snaf_info;
    const uint32_t col_addr = mtd->writesize+SNAF_OOB_SIZE(snaf_info)-4;
    uint32_t bbi4;
    uint32_t bbi;
#else
    const uint32_t col_addr = mtd->writesize;
    uint32_t bbi4;
    uint8_t bbi;
#endif

    if ( ret) {
	return 1;
    }
    /* pio access must be 4-byte aligned. */
    nasu_pio_read(&bbi4, sizeof(bbi4), blk_pge_addr, col_addr);
#if CONFIG_USE_BBT_SKIP
    bbi = *((uint32_t *)&bbi4);
    ret = (bbi == 0);
#else
    bbi = *((uint8_t *)&bbi4);
    ret = (bbi != 0xff);
#endif

    snaf_dmsg(
        "DD: %p <=%d= %06x,%04x; BBI4: %08x, BBI: %02x\n",
        &bbi4, sizeof(bbi4), blk_pge_addr, col_addr, bbi4, bbi);

    return ret;
}

static int otto_snaf_block_markbad(struct mtd_info *mtd, loff_t ofs) {
    printf("DD: %s\n", __FUNCTION__);
    return 0;
}

static void otto_snaf_cmd_ctrl(struct mtd_info *mtd, int dat, unsigned int ctrl) {
    printf("DD: %s\n", __FUNCTION__);
    return;
}

static int otto_snaf_dev_ready(struct mtd_info *mtd) {
    printf("DD: %s\n", __FUNCTION__);
    return 0;
}

static void otto_snaf_cmdfunc(
    struct mtd_info *mtd, unsigned command, int column, int page_addr
    ) {
    printf("DD: %s\n", __FUNCTION__);
    return;
}

static int otto_snaf_waitfunc(struct mtd_info *mtd, struct nand_chip *this) {
    printf("DD: %s\n", __FUNCTION__);
    return 0;
}

static int otto_snaf_erase(struct mtd_info *mtd, int page) {
    printf("DD: %s\n", __FUNCTION__);
    return 0;
}

static int otto_snaf_scan_bbt(struct mtd_info *mtd) {
    snaf_dmsg("EE: %s\n", __FUNCTION__);

    /* already set to bypass bbt in nand_chip; make it an error if scan_bbt is called. */
    while (1);

    return 0;
}

static int otto_snaf_write_page(
    struct mtd_info *mtd, struct nand_chip *chip, uint32_t offset, int data_len,
    const uint8_t *buf, int oob_required, int page, int raw
    ) {
    printf("DD: %s\n", __FUNCTION__);
    return 0;
}

static int otto_snaf_onfi_set_features(
    struct mtd_info *mtd, struct nand_chip *chip, int feature_addr, uint8_t *subfeature_para
    ) {
    printf("DD: %s\n", __FUNCTION__);
    return 0;
}

static int otto_snaf_onfi_get_features(
    struct mtd_info *mtd, struct nand_chip *chip, int feature_addr, uint8_t *subfeature_para
    ) {
    printf("DD: %s\n", __FUNCTION__);
    return 0;
}

static int otto_snaf_setup_read_retry(struct mtd_info *mtd, int retry_mode) {
    printf("DD: %s\n", __FUNCTION__);
    return 0;
}

static int otto_snaf_setup_data_interface(
    struct mtd_info *mtd, int chipnr, const struct nand_data_interface *conf
    ) {
    printf("DD: %s\n", __FUNCTION__);
    return 0;
}

static struct nand_ecclayout otto_nand_oob_64 = {
    .eccbytes = 40,
    .eccpos = {
            24, 25, 26, 27, 28, 29, 30, 31, 32, 33,
            34, 35, 36, 37, 38, 39, 40, 41, 42, 43,
            44, 45, 46, 47, 48, 49, 50, 51, 52, 53,
            54, 55, 56, 57, 58, 59, 60, 61, 62, 63},
    .oobfree = {
        {.offset = 2,
         .length = 22,}}
};

static struct nand_ecclayout otto_nand_oob_128 = {
    .eccbytes = 80,
    .eccpos = {
             48, 49, 50, 51, 52, 53, 54, 55, 56, 57,
             58, 59, 60, 61, 62, 63, 64, 65, 66, 67,
             68, 69, 70, 71, 72, 73, 74, 75, 76, 77,
             78, 79, 80, 81, 82, 83, 84, 85, 86, 87,
             88, 89, 90, 91, 92, 93, 94, 95, 96, 97,
             98, 99,100,101,102,103,104,105,106,107,
            108,109,110,111,112,113,114,115,116,117,
            118,119,120,121,122,123,124,125,126,127},
    .oobfree = {
        {.offset = 2,
         .length = 46,}}
};

static struct nand_chip nand_chip = {
    /* to be set on run-time */
    .mtd = {0},

    /* RTK SPI NAND cntlr. doesn't work like this */
    .IO_ADDR_R = NULL,
    .IO_ADDR_W = NULL,

    /* Ref. to access flash node in device tree. We don't leverage device tree so leave it */
    .flash_node = 0,

    /* dummy functions for observing U-Boot MTD/NAND behavior */
    .read_byte = otto_snaf_read_byte,
    .read_word = otto_snaf_read_word,
    .write_byte = otto_snaf_write_byte,
    .write_buf = otto_snaf_write_buf,
    .read_buf = otto_snaf_read_buf,
    .select_chip = otto_snaf_select_chip,
    .block_bad = otto_snaf_block_bad,
    .block_markbad = otto_snaf_block_markbad,
    .cmd_ctrl = otto_snaf_cmd_ctrl,
    .dev_ready = otto_snaf_dev_ready,
    .cmdfunc = otto_snaf_cmdfunc,
    .waitfunc = otto_snaf_waitfunc,
    .erase = otto_snaf_erase,
    .scan_bbt = otto_snaf_scan_bbt,
    .write_page = otto_snaf_write_page,
    .onfi_set_features = otto_snaf_onfi_set_features,
    .onfi_get_features = otto_snaf_onfi_get_features,
    .setup_read_retry = otto_snaf_setup_read_retry,
    .setup_data_interface = otto_snaf_setup_data_interface,

    /* chip dependent delay for transferring data from array to read regs (tR) */
    /* We don't need this, since the underlying driver already handle that. */
    .chip_delay = 0,

    /* to be set */
    .options = NAND_SKIP_BBTSCAN,

    /* to be set, check bbm.h for more info */
    .bbt_options = NAND_BBT_USE_FLASH,

    /* number of address bits a page, run-time */
    .page_shift = 0,

    /* number of addr. bits a block? run-time */
    .phys_erase_shift = 0,

    /* = phy_erase_shift, run-time */
    .bbt_erase_shift = 0,

    /* flash size addr. bits; run-time */
    .chip_shift = 0,

    .numchips = 1,

    /* flash size, run-time */
    .chipsize = 0,

    /* page numer mask, run-time */
    .pagemask = 0,

    /* holds the pagenumber which is currently in data_buf */
    .pagebuf = 0,

    /* bigflip count for the page which isb currently in data_buf */
    .pagebuf_bitflips = 0,

    /* minimal write size, size per ECC, run-time */
    .subpagesize = 0,

    /* SLC? MLC? etc. */
    .bits_per_cell = 1,

    /* ECC ability, run-time */
    .ecc_strength_ds = 0,

    /* Always 512-byte for gen1. */
    .ecc_step_ds = 512,

    /* SNAF doesn't support this. */
    .onfi_timing_mode_default = 0,

    /* position of BBI in spare. */
    .badblockpos = 0,

    /* minimum number of set bits in a good block's bad block marker position.
       E.g., 7 means at least 7 1s (ones) in BBI to present a good block. */
    .badblockbits = 8,

    .onfi_version = IGNORED_FOR_NOW,
    .jedec_version = IGNORED_FOR_NOW,

    /* ignore both until further requirement */
    .onfi_params = {{IGNORED_FOR_NOW}},
    .jedec_params = {{IGNORED_FOR_NOW}},

    /* ignore timing until further requirement. They should already been settle in Otto. */
    .data_interface = IGNORED_FOR_NOW,

    /* the number of read retry modes supported */
    .read_retries = IGNORED_FOR_NOW,

    /* the current state of the NAND device, ignored for now. */
    .state = IGNORED_FOR_NOW,

    /* laying out OOB data before writing, will be set later. */
    .oob_poi = 0,

    .controller = IGNORED_FOR_NOW,
    .ecclayout = IGNORED_FOR_NOW,

    .ecc = {
        .mode = NAND_ECC_NONE,
        .size = 0, //set by run-time => .ecc_step_ds
        .bytes = 16,
        .strength = 6,
    },

    .buffers = IGNORED_FOR_NOW,

    /* minimum buffer alignment required by a platform */
    .buf_align = 0x20, //32-byte cache line means 32-byte alignment.
    .hwcontrol = {
        .lock = {},
        .active = IGNORED_FOR_NOW,
    },

    /* bad block table pointer */
    .bbt = IGNORED_FOR_NOW,

    /* bad block table descriptor for flash loopup */
    .bbt_td = IGNORED_FOR_NOW,

    /* bad block table mirror descriptor */
    .bbt_md = IGNORED_FOR_NOW,

    /* bad block scan pattern used for initial bad block scan */
    .badblock_pattern = IGNORED_FOR_NOW,

    /* set at run-time for reloc. */
    .priv = 0,
};

 static int
 is_plr_first_load_part(plr_first_load_layout_t *fl_buf)
 {
     //if(nasu_ecc_engine_action(((u32_t)_spi_nand_info->_ecc_ability), fl_buf->data0, fl_buf->oob , 0) == ECC_CTRL_ERR) return ECC_CTRL_ERR;
     //if(nasu_ecc_engine_action(((u32_t)_spi_nand_info->_ecc_ability), fl_buf->data1, fl_buf->oob , 0) == ECC_CTRL_ERR) return ECC_CTRL_ERR;
     if(PLR_FL_GET_SIGNATURE(fl_buf) == SIGNATURE_PLR_FL) return 1;
     return 0;
 }

static int otto_snaf_mtd_read_oob(
    struct mtd_info *mtd, loff_t from, struct mtd_oob_ops *ops
    ) {
    uint32_t col_addr;
    uint32_t blk_pge_addr;
    uint32_t r_blk_pge_addr;
    uint32_t done_b, op_len;
    uint8_t *buf;
    const uint32_t dlen = ops->len;
    const uint32_t olen = ops->ooblen;
    const uint32_t pagesize = mtd->writesize;
    const uint32_t oobsize = mtd->oobsize;
    void *dma_buf, *ecc_buf;

    blk_pge_addr = from >> nand_chip.page_shift;
    r_blk_pge_addr = blk_pge_addr;

    /* olen only aval. when read.raw; each read_oob() processes just one page */
    if (olen) {
        col_addr = pagesize;

        blk_pge_addr = r_blk_pge_addr;
        get_good_block(mtd, &blk_pge_addr);
        nasu_pio_read(ops->oobbuf, olen, blk_pge_addr, col_addr);
        snaf_dmsg("DD: %p <=PR:%d= %06x,%04x\n", ops->oobbuf, olen, blk_pge_addr, col_addr);

        ops->oobretlen = olen;
    }

    col_addr = 0;
    done_b = 0;
    buf = ops->datbuf;

    /* make a cache line alligned space from stack */
    dma_buf = (void *)__builtin_alloca(pagesize + oobsize + CONFIG_SYS_DCACHE_LINE_SIZE * 2);
    dma_buf = (void *)(((uint32_t)dma_buf + 31) & (~(CONFIG_SYS_DCACHE_LINE_SIZE - 1)));

    /* 28 for BCH12 */
    ecc_buf = (void *)__builtin_alloca(28 + (CONFIG_SYS_DCACHE_LINE_SIZE*2));
    ecc_buf = (void *)(((uint32_t)ecc_buf + 31) & (~(CONFIG_SYS_DCACHE_LINE_SIZE - 1)));

    uint32_t off = (uint32_t)from & (pagesize - 1);
    if (off) {
        op_len = SNAF_MIN(dlen, (pagesize - off));

        invalidate_dcache_range((ulong)dma_buf, (ulong)dma_buf + pagesize + oobsize);

                blk_pge_addr = r_blk_pge_addr;
                get_good_block(mtd, &blk_pge_addr);
                if (olen) {
                        nasu_page_read(dma_buf, blk_pge_addr);
                        snaf_dmsg("DD: %p <=DR:%d= %06x,%04x\n", dma_buf, pagesize, blk_pge_addr, col_addr);
                } else {
                        int ret = nasu_page_read_ecc(dma_buf, blk_pge_addr, ecc_buf);
                        snaf_dmsg("DD: %p <=DE:%d= %06x,%04x\n", dma_buf, pagesize, blk_pge_addr, col_addr);
                        if ( IS_ECC_DECODE_FAIL(ret) == 1) {
                                plr_first_load_layout_t *fl_buf=(plr_first_load_layout_t *)dma_buf;
                                if(1 == is_plr_first_load_part(fl_buf)){
                                        //do nothing
                                } else {
                                        printf("ECC decode error in block page addr %X\n", blk_pge_addr);
#if CONFIG_ECC_ERR_SHOW_PAGE
                                        rtk_show_page(blk_pge_addr, pagesize, oobsize, dma_buf);
#endif
                                }
                        }
                }
        memcpy(buf, dma_buf+off, op_len);

        r_blk_pge_addr++;
        done_b += op_len;
        buf += op_len;
    }

    while (done_b < dlen) {
        op_len = SNAF_MIN((dlen - done_b), pagesize);

        invalidate_dcache_range((ulong)dma_buf, (ulong)dma_buf + pagesize + oobsize);

        blk_pge_addr = r_blk_pge_addr;
        get_good_block(mtd, &blk_pge_addr);
        if (olen) {
            nasu_page_read(dma_buf, blk_pge_addr);
            snaf_dmsg("DD: %p <=DR:%d= %06x,%04x\n", dma_buf, pagesize, blk_pge_addr, col_addr);
        } else {
            int ret = nasu_page_read_ecc(dma_buf, blk_pge_addr, ecc_buf);
            snaf_dmsg("DD: %p <=DE:%d= %06x,%04x\n", dma_buf, pagesize, blk_pge_addr, col_addr);
            if ( IS_ECC_DECODE_FAIL(ret) == 1) {
                    plr_first_load_layout_t *fl_buf=(plr_first_load_layout_t *)dma_buf;
                    if(1 == is_plr_first_load_part(fl_buf)){
                            //do nothing
                    } else {
                            printf("ECC decode error in block page addr %X\n", blk_pge_addr);
#if CONFIG_ECC_ERR_SHOW_PAGE
                            rtk_show_page(blk_pge_addr, pagesize, oobsize, dma_buf);
#endif
                    }
            } else if (ret != 0) {
                printf("ECC corrected %d bits\n", ret);
            }
        }

        memcpy(buf, dma_buf, op_len);

        r_blk_pge_addr++;
        done_b += op_len;
        buf += op_len;
    }
    ops->retlen = done_b;

    return 0;
}

static int otto_snaf_mtd_erase_one_blk(struct mtd_info *mtd, struct erase_info *instr) {
    /* starting addr. is forced to block-aligned by caller, i.e., need not col. addr. */
    uint32_t blk_pge_addr = instr->addr >> nand_chip.page_shift;
#if CONFIG_USE_BBT_SKIP
    spi_nand_flash_info_t *snaf_info = otto_sc.snaf_info;
    int blk_shift = (nand_chip.phys_erase_shift - nand_chip.page_shift);
    int blk_num = blk_pge_addr>>blk_shift;
    int ret = get_good_block(mtd, &blk_pge_addr);
    uint32_t block_count = SNAF_NUM_OF_BLOCK(snaf_info);
    if ( ret) {
	if (-1 == ret) {
	    return -1;
	}
	if(instr->scrub) {
	    if (blk_pge_addr){
		nasu_block_erase(blk_pge_addr);
		if (badblocks[block_count - (blk_num+1)] < 0){
		    badblocks[block_count - (blk_num+1)] = - badblocks[block_count - (blk_num+1)];
		}
	    } else {
		return -1;
	    }
	} else {
	    return -1;
	}
	return 0;
    }
#else
    get_good_block(mtd, &blk_pge_addr);
#endif

    snaf_dmsg("DD: Erase: SNAF:BP:%llx(%llu)... \n", instr->addr, instr->addr);

    nasu_block_erase(blk_pge_addr);

    return 0;
}

static int otto_snaf_mtd_write_oob(
    struct mtd_info *mtd, loff_t to, struct mtd_oob_ops *ops
    ) {
    uint32_t col_addr;
    uint32_t blk_pge_addr;
    uint32_t r_blk_pge_addr;
    uint32_t done_b, op_len;
    uint8_t *buf;
    const uint32_t dlen = ops->len;
    const uint32_t olen = ops->ooblen;
    const uint32_t pagesize = mtd->writesize;
    const uint32_t oobsize = mtd->oobsize;
    void *dma_buf, *ecc_buf;
    int ret;

    blk_pge_addr = to >> nand_chip.page_shift;
    r_blk_pge_addr = blk_pge_addr;
    ret = get_good_block(mtd, &blk_pge_addr);
    if(ret) {
	return ret;
    }

    /* make a cache line alligned space from stack */
    dma_buf = (void *)__builtin_alloca(pagesize + oobsize + CONFIG_SYS_DCACHE_LINE_SIZE * 2);
    dma_buf = (void *)(((uint32_t)dma_buf + 31) & (~(CONFIG_SYS_DCACHE_LINE_SIZE - 1)));

    /* max 28 for BCH12 */
    ecc_buf = (void *)__builtin_alloca(28 + (CONFIG_SYS_DCACHE_LINE_SIZE*2));
    ecc_buf = (void *)(((uint32_t)ecc_buf + 31) & (~(CONFIG_SYS_DCACHE_LINE_SIZE - 1)));

		col_addr = 0;
    done_b = 0;
    buf = ops->datbuf;

    if (MTD_OPS_RAW == ops->mode ) {
        if (dlen==0) { //write_oob() processes just one page
            blk_pge_addr = r_blk_pge_addr;
            ret = get_good_block(mtd, &blk_pge_addr);
	    if(ret) {
		return ret;
	    }

            /* read and replace */
            nasu_pio_read(ops->oobbuf, pagesize+oobsize, blk_pge_addr, 0);

            memcpy(dma_buf+pagesize, ops->oobbuf, olen);

            nasu_pio_write(ops->oobbuf, olen, blk_pge_addr, col_addr);
            snaf_dmsg("DD: %p =RPR:%d=> %06x,%04x\n", ops->oobbuf, olen, blk_pge_addr, col_addr);

            ops->oobretlen = olen;
        } else {
            while (done_b < dlen) {
                op_len = SNAF_MIN((dlen+olen - done_b), dlen+olen);

                memcpy(dma_buf, buf, op_len);

                writeback_dcache_range((uint32_t)dma_buf, (uint32_t)dma_buf + pagesize + oobsize);

                blk_pge_addr = r_blk_pge_addr;
                ret = get_good_block(mtd, &blk_pge_addr);
		if(ret) {
		    return ret;
		}
                nasu_pio_write(dma_buf, op_len, blk_pge_addr, 0);

                r_blk_pge_addr++;
                done_b += op_len;
                buf += op_len;
            }
            ops->retlen = done_b;
        }
    } else {
        while (done_b < dlen) {
            op_len = SNAF_MIN((dlen - done_b), pagesize);

            memcpy(dma_buf, buf, op_len);
            memset(dma_buf + pagesize, 0xff, oobsize);

            writeback_dcache_range((uint32_t)dma_buf, (uint32_t)dma_buf + pagesize + oobsize);
            blk_pge_addr = r_blk_pge_addr;
            ret = get_good_block(mtd, &blk_pge_addr);
	    if(ret) {
		return ret;
	    }

            if (olen) { // need?
                nasu_page_write(dma_buf, blk_pge_addr);
                snaf_dmsg("DD: %p =DR:%d=> %06x,%04x\n", dma_buf, pagesize, blk_pge_addr, col_addr);
            } else {
                nasu_page_write_ecc(dma_buf, blk_pge_addr, ecc_buf);
                snaf_dmsg("DD: %p =DE:%d=> %06x,%04x\n", dma_buf, pagesize, blk_pge_addr, col_addr);
            }

            r_blk_pge_addr++;
            done_b += op_len;
            buf += op_len;
        }
        ops->retlen = done_b;
    }

#if ((defined(CONFIG_SNAF_MX35LF2G14AC_WORKAROUND)) && (CONFIG_SNAF_MX35LF2G14AC_WORKAROUND == 1))
    uint32_t dummy;
    nasu_pio_read(&dummy, 4, blk_pge_addr, 0);
#endif

    return 0;
}

static int otto_snaf_mtd_block_markbad(struct mtd_info *mtd, loff_t ofs) {
    uint32_t blk_pge_addr = (ofs & (~(mtd->erasesize - 1))) >> nand_chip.page_shift;
    int ret = get_good_block(mtd, &blk_pge_addr);
    if(ret) {
	return ret;
    }

#if CONFIG_USE_BBT_SKIP
    spi_nand_flash_info_t *snaf_info = otto_sc.snaf_info;
    const uint32_t col_addr = mtd->writesize+SNAF_OOB_SIZE(snaf_info)-4;
#else
    const uint32_t col_addr = mtd->writesize;
#endif
    uint32_t bb_mark = 0x0;

    snaf_dmsg("DD: Erase: SNAF:BP:%llx(%llu)... \n", ofs, ofs);
    nasu_block_erase(blk_pge_addr);

    snaf_dmsg(
        "DD: %p =PR:%d=> %06x,%04x\n", &bb_mark, sizeof(bb_mark), blk_pge_addr, col_addr);
    nasu_pio_write(&bb_mark, sizeof(bb_mark), blk_pge_addr, col_addr);

    return 0;
}

void __weak
nand_storage_init(void) {
    return;
}

extern spi_nand_cmd_info_t nsc_sio_cmd_info;
extern spi_nand_model_info_t snaf_rom_general_model;
spi_nand_flash_info_t ub_spi_nand_flash_info = {
    .man_id                 = 0xDE,            // won't be used
    .dev_id                 = 0xFA27,          // won't be used
    ._num_block             = SNAF_MODEL_NUM_BLK_512,
    ._num_page_per_block    = SNAF_MODEL_NUM_PAGE_64,
    ._page_size             = SNAF_MODEL_PAGE_SIZE_2048B,
    ._spare_size            = SNAF_MODEL_SPARE_SIZE_64B,
    ._oob_size              = SNAF_MODEL_OOB_SIZE(24),
    ._ecc_ability           = ECC_MODEL_6T,
    ._ecc_encode            = VZERO,        // won't be used
    ._ecc_decode            = VZERO,        // won't be used
    ._reset                 = VZERO,        // won't be used
    ._cmd_info              = &nsc_sio_cmd_info,
    ._model_info            = &snaf_rom_general_model,
};

#if (CONFIG_SPI_NAND_FLASH_INIT_REST == 1)
fpv_u32_t                       *_nsu_reset_ptr;
fpv_u32_t                       *_nsu_en_on_die_ecc_ptr;
fpv_u32_t                       *_nsu_dis_on_die_ecc_ptr;
fpv_u32_t                       *_nsu_blk_unprotect_ptr;
spi_nand_get_feature_reg_t      *_nsu_get_feature_ptr;
spi_nand_set_feature_reg_t      *_nsu_set_feature_ptr;
spi_nand_read_id_t              *_nsu_read_id_ptr;

void spi_nand_retrieve_func(void) {
    symb_retrive_entry_t spi_nand_func_retrive_list[] = {
        {SNAF_RESET_SPI_NAND_FUNC, &_nsu_reset_ptr},
        {SNAF_SET_FEATURE_FUNC, &_nsu_set_feature_ptr},
        {SNAF_GET_FEATURE_FUNC, &_nsu_get_feature_ptr},
        {SNAF_READ_SPI_NAND_FUNC, &_nsu_read_id_ptr},
        {SNAF_DISABLE_ODE_FUNC, &_nsu_dis_on_die_ecc_ptr},
        {SNAF_ENABLE_ODE_FUNC, &_nsu_en_on_die_ecc_ptr},
        {SNAF_BLOCK_UNPROTECT_FUNC, &_nsu_blk_unprotect_ptr},
        {ENDING_SYMB_ID, VZERO},
    };
    symb_retrive_list(spi_nand_func_retrive_list, OTTO_TEMP_SYM_HEADER, OTTO_TEMP_SYM_END);
}
#endif

spi_nand_flash_info_t *nand_probe(uint32_t *cnt) {

#if (CONFIG_SPI_NAND_FLASH_INIT_REST == 1)
    spi_nand_retrieve_func();

    /* Initial rest chip(s) */
    typedef int (init_rest_func_t)(void);
    extern init_rest_func_t *LS_start_of_snaf_init_rest_func[], *LS_end_of_snaf_init_rest_func;

    init_rest_func_t **init_rest_func = LS_start_of_snaf_init_rest_func;
    while (init_rest_func!=&LS_end_of_snaf_init_rest_func) {
        if(0!=(*init_rest_func)()) { (*cnt)++; break;}
        ++init_rest_func;
    }
#endif // CONFIG_SPI_NAND_FLASH_INIT_REST

    return otto_sc.snaf_info;
}


#if CONFIG_USE_BBT_SKIP
/* inline function */
inline static void
_set_flags(u32_t *arr, u32_t i) {
    unsigned idx=i/(8*sizeof(u32_t));
    i &= (8*sizeof(u32_t))-1;
    arr[idx] |= 1UL << i;
}

inline static int
_get_flags(u32_t *arr, u32_t i) {
    unsigned idx=i/(8*sizeof(u32_t));
    i &= (8*sizeof(u32_t))-1;
    return (arr[idx] & (1UL << i)) != 0;
}

static void
create_bbt(struct mtd_info *mtd, u32_t *bbt_table)
{
    spi_nand_flash_info_t *snaf_info = otto_sc.snaf_info;
    const spi_nand_model_info_t *snaf_cmd = snaf_info->_model_info;
    uint8_t bb_tag[4];
    uint32_t blk_num;
    uint32_t block_count = SNAF_NUM_OF_BLOCK(snaf_info);
    const uint32_t pagesize = mtd->writesize;
    const uint32_t oobsize = mtd->oobsize;
    void *dma_buf, *ecc_buf;
#if CONFIG_USE_BBT_SKIP
    int bad_blk_idx = 0;

    memset(badblocks, 0x0, sizeof(badblocks));
#endif

    /* make a cache line alligned space from stack */
    dma_buf = (void *)__builtin_alloca(pagesize + oobsize + CONFIG_SYS_DCACHE_LINE_SIZE * 2);
    dma_buf = (void *)(((uint32_t)dma_buf + 31) & (~(CONFIG_SYS_DCACHE_LINE_SIZE - 1)));

    /* 28 for BCH12 */
    ecc_buf = (void *)__builtin_alloca(28 + (CONFIG_SYS_DCACHE_LINE_SIZE*2));
    ecc_buf = (void *)(((uint32_t)ecc_buf + 31) & (~(CONFIG_SYS_DCACHE_LINE_SIZE - 1)));

    puts("Hidden bad blcoks: ");

    for ( blk_num = 1; blk_num < block_count; blk_num++) {
        snaf_cmd->_pio_read(snaf_info, bb_tag, sizeof(bb_tag), blk_num<<6, SNAF_PAGE_SIZE(snaf_info));
        if (0xFF == bb_tag[0]) {
            continue;
        }
#if 1
        //printf("DD: bb_tag[0]=0x%x, snaf_info.page_size=%d\n", bb_tag[0], snaf_info.page_size);
        //ret = nasu_page_read_ecc(_tmp, blk_num<<6, _tmp_ecc);
        int ret = nasu_page_read_ecc(dma_buf, blk_num<<6, ecc_buf);
        if (IS_ECC_DECODE_FAIL(ret) == 1) {
            // ecc error, mark it as a bad block
        } else {
            if (0xFF == ((uint8_t*)dma_buf)[SNAF_PAGE_SIZE(snaf_info)]) {
                continue;
            }
        }
#endif
        printf("%4d ", blk_num);

#if CONFIG_USE_BBT_SKIP
	badblocks[bad_blk_idx] = -blk_num;
	bad_blk_idx += 1;
#endif
        _set_flags(bbt_table, blk_num);
    }
    puts("\n");
#if CONFIG_USE_BBT_SKIP
    if (bad_blk_idx > 1){
	for (int i = 0; i < bad_blk_idx/2; i++) {
	    int32_t blknm = badblocks[i];
	    badblocks[i] = badblocks[bad_blk_idx-(i+1)];
	    badblocks[bad_blk_idx-(i+1)] =  blknm;
	    //printf("swap badblocks idx %d %d\n", i, bad_blk_idx-(i+1));
	}
	//for (int i = 0; i < bad_blk_idx; i++)
	//{
	//    printf("badblocks[%d] = %d\n", i, badblocks[i]);
	//}
    }
#endif
}

static void
create_skip_table(struct mtd_info *mtd, uint32_t *bbt_table)
{
    spi_nand_flash_info_t *snaf_info = otto_sc.snaf_info;
    //const spi_nand_model_info_t *snaf_cmd = snaf_info->_model_info;
    unsigned int good_num;
    int j;
    unsigned int bbi_block;
    unsigned int skip_block;
    uint8_t *skip_table = bb_skip_table;
    skip_table[0] = 0;

    skip_block = good_num = bbi_block = 1;
    for (;bbi_block<SNAF_NUM_OF_BLOCK(snaf_info);bbi_block++) {
        j=_get_flags(bbt_table, bbi_block);

        if(j==0){ //good block

            skip_table[skip_block]=bbi_block-good_num;
            good_num++;
            skip_block++;
        }else{
            //printf("detect block %d is bad \n",bbi_block);
            if(bbi_block-good_num == 0xFF){
                break;
            }
        }
    }
    for (;skip_block<SNAF_NUM_OF_BLOCK(snaf_info);skip_block++){
        skip_table[skip_block]=0xff;
    }
}
#endif

static void otto_snaf_mx35lf2g14ac_warn(spi_nand_flash_info_t *info) {
	if ((info->man_id == 0xc2) && (info->dev_id == 0x20)) {
		printf("\nWW: SNAF: MX35LF2G14AC (mid: C2h, did: 20h) detected");
#if (!(defined(CONFIG_SNAF_MX35LF2G14AC_WORKAROUND)) || (CONFIG_SNAF_MX35LF2G14AC_WORKAROUND != 1))
		printf(", and OTTO_SNAF_MX35LF2G14AC_PATCH is not applied!!!");
#endif
		printf("\n");
	}
	return;
}


void board_nand_init(void)
{
    uint32_t chip_cnt=1;
    spi_nand_flash_info_t *snaf_info = nand_probe(&chip_cnt);
    #define OPCODE_IO(rwio) (1<<snaf_info->_cmd_info->rwio)
    struct mtd_info *mtd = nand_to_mtd(&nand_chip);
    uint32_t page_size, block_size, chip_size;


    if(snaf_info==NULL) return;

#if CONFIG_USE_BBT_SKIP
    u32_t _bbt_table[NUM_WORD(MAX_BLOCKS)];
    memset(_bbt_table, 0, sizeof(_bbt_table));
#endif

    otto_snaf_mx35lf2g14ac_warn(snaf_info);

    page_size = SNAF_PAGE_SIZE(snaf_info);
    block_size = page_size * SNAF_NUM_OF_PAGE_PER_BLK(snaf_info);
    chip_size = block_size * SNAF_NUM_OF_BLOCK(snaf_info);

    printf("\rSPI NAND: 0x%02X%02X, Mode(1%d%d/1%d%d)\n",  snaf_info->man_id, snaf_info->dev_id, OPCODE_IO(r_addr_io), OPCODE_IO(r_data_io), OPCODE_IO(w_addr_io), OPCODE_IO(w_data_io));

    /* set mtd sizes ere due to these size parameters will be destroyed soon. */
    mtd->writesize = page_size;
    mtd->erasesize = block_size;
    mtd->size = chip_size * chip_cnt;
    mtd->writebufsize = page_size;
    mtd->oobsize = SNAF_SPARE_SIZE(snaf_info);

    while (page_size >>= 1) {
        nand_chip.page_shift++;
    };

    while (block_size >>= 1) {
        nand_chip.phys_erase_shift++;
    }
    nand_chip.bbt_erase_shift = nand_chip.phys_erase_shift;

    nand_chip.chipsize = chip_size * chip_cnt;
    while (chip_size >>= 1) {
        nand_chip.chip_shift++;
    }

    nand_chip.pagemask = (nand_chip.chipsize >> nand_chip.page_shift) - 1;

    nand_chip.subpagesize = nand_chip.ecc_step_ds;
    nand_chip.ecc.size = nand_chip.ecc_step_ds;

    nand_chip.ecc_strength_ds = (snaf_info->_ecc_ability + 1) * 6;
    nand_chip.ecc.layout = (mtd->oobsize==128)?&otto_nand_oob_128:&otto_nand_oob_64;

    nand_chip.priv = (void *)snaf_info;

    /* set default mtd and ecc */
    nand_scan_tail(mtd);

    /* replace to customized func. */
    /* called by mtd_read() @ mtdcore.c */
    mtd->_read_oob = otto_snaf_mtd_read_oob;

    mtd->_erase = otto_snaf_mtd_erase_one_blk;
    mtd->_write_oob = otto_snaf_mtd_write_oob;
    mtd->_block_markbad = otto_snaf_mtd_block_markbad;
#if CONFIG_USE_BBT_SKIP
    create_bbt(mtd, _bbt_table);
    create_skip_table(mtd, _bbt_table);
    puts("NAND:  ");
#endif

    nand_register(0, mtd);

    return;
}

