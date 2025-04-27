#include <common.h>
#include <soc.h>
#include <asm/symb_define.h>
#include <asm/otto_util.h>
#include <asm/otto_spi_nand_flash.h>
#include <spi_nand/spi_nand_blr_util.h>

#define MAX_BLOCKS (1024<<2)


#ifndef USE_BBT_SKIP
#define USE_BBT_SKIP 1
#endif

#if 0
#if USE_BBT_SKIP
uint8_t bb_skip_table[MAX_BLOCKS];
#else
u32_t _bbt_table[NUM_WORD(MAX_BLOCKS)];
#endif
#endif

#define EBUF_SIZE                   (MAX_ECC_BUF_SIZE)
#define PBUF_SIZE                   (MAX_PAGE_BUF_SIZE+MAX_OOB_BUF_SIZE)

#if 0
static u_char _eccbuf[EBUF_SIZE] DATA_ALIGNED32;
static u_char _pbuf[PBUF_SIZE] DATA_ALIGNED32;
#endif

#define DEV_ID_LEN_BITS(dev_id) ((dev_id > 0xff)?16:8)

#define OTTO_TEMP_SYM_HEADER ((const symbol_table_entry_t *)(((soc_t *)(0x9fc00020))->bios.header.export_symb_list))
#define OTTO_TEMP_SYM_END    ((const symbol_table_entry_t *)(((soc_t *)(0x9fc00020))->bios.header.end_of_export_symb_list))

#if defined(CONFIG_MTD_DEVICE) && defined(CONFIG_MTD_PARTITIONS)
#ifdef CONFIG_SPI_NAND_FLASH
extern int rtk_spi_nand_init (void);
#endif
#endif

// Assumption: 2CS spi-nand will use same model
otto_spi_nand_info_t _info;
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

#if OTTO_SPI_NAND_GEN > 1
fpv_u32_t                       *_nsu_reset_ptr;
fpv_u32_t                       *_nsu_en_on_die_ecc_ptr;
fpv_u32_t                       *_nsu_dis_on_die_ecc_ptr;
fpv_u32_t                       *_nsu_blk_unprotect_ptr;
spi_nand_get_feature_reg_t      *_nsu_get_feature_ptr;
spi_nand_set_feature_reg_t      *_nsu_set_feature_ptr;
spi_nand_read_id_t              *_nsu_read_id_ptr;
#if OTTO_SPI_NAND_GEN > 2
spi_nand_send_instr_gen_t       *_nsu_send_instr_gen_ptr;
#endif

void spi_nand_retrieve_func(void) {
    symb_retrive_entry_t spi_nand_func_retrive_list[] = {
        {SNAF_RESET_SPI_NAND_FUNC, &_nsu_reset_ptr},
        {SNAF_SET_FEATURE_FUNC, &_nsu_set_feature_ptr},
        {SNAF_GET_FEATURE_FUNC, &_nsu_get_feature_ptr},
        {SNAF_READ_SPI_NAND_FUNC, &_nsu_read_id_ptr},
        {SNAF_DISABLE_ODE_FUNC, &_nsu_dis_on_die_ecc_ptr},
        {SNAF_ENABLE_ODE_FUNC, &_nsu_en_on_die_ecc_ptr},
        {SNAF_BLOCK_UNPROTECT_FUNC, &_nsu_blk_unprotect_ptr},
	#if OTTO_SPI_NAND_GEN > 2
        {SNAF_SEND_INSTR_GEN_FUNC, &_nsu_send_instr_gen_ptr},
	#endif
        {ENDING_SYMB_ID, VZERO},
    };
    symb_retrive_list(spi_nand_func_retrive_list, OTTO_TEMP_SYM_HEADER, OTTO_TEMP_SYM_END);
}
#endif

uint32_t spi_nand_chip_size(void) {
    return _info.chip_size;
}

uint32_t spi_nand_block_size(void) {
    return _info.block_size;
}

uint32_t spi_nand_page_size(void) {
    return _info.page_size;
}

uint32_t spi_nand_spare_size(void) {
    return _info.spare_size;
}

uint32_t spi_nand_oob_size(void) {
    return _info.oob_size;
}

uint32_t spi_nand_cs_count(void) {
    return _info.cs_count;
}

spi_nand_flash_info_t *spi_nand_flash_info(void) {
    return _info.flash_info;
}

u32_t spi_nand_get_feature(u32_t cs, u32_t feature_addr) {
    u32_t v = 0xFF;
#ifdef CONFIG_SPI_NAND_FLASH_INIT_FIRST
    u32_t nsc_get_feature_register(u32_t, u32_t);
    v = nsc_get_feature_register(cs, feature_addr);
#else
    if(_nsu_get_feature_ptr!=NULL) {
        v = _nsu_get_feature_ptr(cs, feature_addr);
    }
#endif
    return v;
}

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

#if 0
static void create_bbt(u32_t *bbt_table)
{
    uint8_t bb_tag[4];
    uint32_t blk_num;
    uint32_t block_count = _info.block_count;
    int ret;

    puts("Create bbt: ");

    for ( blk_num = 1; blk_num < block_count; blk_num++) {
        nasu_pio_read(bb_tag, 4, blk_num<<6, _info.page_size);
        if (0xFF == bb_tag[0]) {
            continue;
        }
        //printf("DD: bb_tag[0]=0x%x, _info.page_size=%d\n", bb_tag[0], _info.page_size);
        ret = nasu_page_read_ecc(_pbuf, blk_num<<6, _eccbuf);
        if (ECC_CTRL_ERR==(ret&0xFFFFFF00)) {
            // ecc error, mark it as a bad block
        } else {
            if (0xFF == _pbuf[_info.page_size]) {
                continue;
            }
        }
        printf("%4d ", blk_num);


        _set_flags(bbt_table, blk_num);
    }
    puts("\n");
#if !USE_BBT_SKIP
    puts("Don't use bad block skip table!\n");
#endif
}

#if USE_BBT_SKIP
static void create_skip_table(uint32_t *bbt_table){
    unsigned int good_num;
    int j;
    unsigned int bbi_block;
    unsigned int skip_block;
    uint8_t *skip_table = bb_skip_table;
    skip_table[0] = 0;

    skip_block = good_num = bbi_block = 1;
    for (;bbi_block<_info.block_count;bbi_block++) {
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
    for (;skip_block<_info.block_count;skip_block++){
        skip_table[skip_block]=0xff;
    }
}
#endif

static int get_good_block(u32_t blk_num, u32_t *mapped_blk_num)
{
#if USE_BBT_SKIP
    u32_t skip_num = bb_skip_table[blk_num];
    if( skip_num == 0xFF){
        return -1;
    }
    *mapped_blk_num = blk_num + skip_num;
#else
    *mapped_blk_num = blk_num;
#endif
    ////printf("DD: get good block %d => %d\n", blk_num, *mapped_blk_num);
    return 0;
}

#if !USE_BBT_SKIP
static int go_to_good_block(u32_t *blk_num)
{
    int j;
    for (;*blk_num<_info.block_count;(*blk_num)++) {
        j = _get_flags(_bbt_table, *blk_num);
        if ( j == 0 ) { //good block
            return 0;
        }
        printf("Skip Bad Block %d\n", *blk_num);
}

    return -1;
}
#endif

static int32_t
is_blank_data(uint32_t *buf, int32_t data_len)
{
    while (data_len--) {
        if (*buf != 0xFFFFFFFF) {
            return 0;
        }
        buf++;
}
    return 1;
}
#endif

spi_nand_flash_info_t *spi_nand_probe(uint32_t *count) {
#if OTTO_SPI_NAND_GEN > 1
    spi_nand_retrieve_func();
#ifdef CONFIG_SPI_NAND_FLASH_INIT_FIRST
    extern int nsu_probe(spi_nand_flash_info_t *info, const spi_nand_probe_t **probe_func_start, const spi_nand_probe_t **probe_func_end);
    REG32(0xb8000100) |= (0x2<<13);
    udelay(1000);
    if(0xFF==spi_nand_get_feature(0, 0xC0)) {
        puts("Unmount SPI-NAND flash\n");
        return NULL;
    }
    extern const spi_nand_probe_t *LS_start_of_spi_nand_probe_func;
    extern const spi_nand_probe_t *LS_end_of_spi_nand_probe_func;
    if(nsu_probe(_plr_spi_nand_info, &LS_start_of_spi_nand_probe_func, &LS_end_of_spi_nand_probe_func)) {
        puts("WW: no spi-nand flash probed\n");
        return NULL;
    }
#endif // CONFIG_SPI_NAND_FLASH_INIT_FIRST
#ifdef CONFIG_SPI_NAND_FLASH_INIT_REST

    /* Initial rest chip(s) */
    typedef int (init_rest_func_t)(void);
    extern init_rest_func_t *LS_start_of_snaf_init_rest_func[], *LS_end_of_snaf_init_rest_func;

    init_rest_func_t **init_rest_func = LS_start_of_snaf_init_rest_func;
    while (init_rest_func!=&LS_end_of_snaf_init_rest_func) {
        if(0!=(*init_rest_func)()) { (*count)++; break;}
        ++init_rest_func;
    }
#endif // CONFIG_SPI_NAND_FLASH_INIT_REST
#endif // OTTO_SPI_NAND_GEN > 1
    return _plr_spi_nand_info;
}

void spi_nand_init(void) {
#if 0
#if USE_BBT_SKIP
    u32_t _bbt_table[NUM_WORD(MAX_BLOCKS)];
#endif
#endif
    spi_nand_flash_info_t *fi;
    uint32_t count=1;

    /* fill spi nand info */
    fi = spi_nand_probe(&count);
    if(NULL==fi) return;

    _info.id = (fi->man_id << DEV_ID_LEN_BITS(fi->dev_id)) | fi->dev_id;
    _info.cs_count=count;
    _info.chip_size=SNAF_NUM_OF_BLOCK(fi)
                   *SNAF_NUM_OF_PAGE_PER_BLK(fi)
                   *SNAF_PAGE_SIZE(fi);
    _info.block_size=SNAF_NUM_OF_PAGE_PER_BLK(fi)
                   *SNAF_PAGE_SIZE(fi);
    _info.page_size=SNAF_PAGE_SIZE(fi);
    _info.oob_size=SNAF_OOB_SIZE(fi);
    _info.spare_size=SNAF_SPARE_SIZE(fi);
    _info.block_count=SNAF_NUM_OF_BLOCK(fi);
    _info.page_count=SNAF_NUM_OF_PAGE_PER_BLK(fi);
    _info.flash_info = fi;

    printf("SPI-NAND Flash: %X/Mode%d %dx%uMB\n", _info.id, fi->_cmd_info->r_data_io, count, spi_nand_chip_size()>>20);
#if 0
    memset(_bbt_table, '\0', sizeof(_bbt_table));
    if(_info.block_count > MAX_BLOCKS){
        printf("SPI-NAND Flash: Create Bad Block Table Error!\n Block Number is More than %d!\n",  MAX_BLOCKS);
    }else{
        create_bbt(_bbt_table);
#if USE_BBT_SKIP
        create_skip_table(_bbt_table);
#endif
    }
#endif
#if defined(CONFIG_MTD_DEVICE) && defined(CONFIG_MTD_PARTITIONS)
#ifdef CONFIG_SPI_NAND_FLASH
    rtk_spi_nand_init ();
#endif
#endif
}

void spi_nand_info(void) {
    printf("Device:\n SPI-NAND, ID:%x, size:%u MB*%d \n",
            _info.id, spi_nand_chip_size()>>20, _info.cs_count);
    printf(" each chip has       %8d block(s)\n", _info.block_count);
    printf(" each block has      %8d page(s)\n", _info.page_count);
    printf(" page size           %8d byte(s)\n", _info.page_size);
    printf(" erase size          %8d byte(s)\n", _info.block_size);
    printf(" spare size          %8d byte(s)\n", _info.spare_size);
    printf("     oob size        %8d byte(s)\n", _info.oob_size);
    printf("     others          %8d byte(s)\n", _info.spare_size-_info.oob_size);
}

#define BLK_IDX(off, bsize)         (off/(bsize))
#define PAG_IDX(off, bsize, psize)  ((off%(bsize))/(psize))
#define COL_OFF(off, psize)         (off%(psize))

#define BSIZE                       (_info.block_size)
#define PSIZE                       (_info.page_size)
#define SSIZE                       (_info.spare_size)
#define BCNT                        (_info.block_count)
#define PCNT                        (_info.page_count)


int _spi_nand_check_ecc(int r) {
    if (ECC_CTRL_ERR==(r&0xFFFFFF00)) {
        printf("\nWW: ecc check failed (ret 0x%x)\n", r);
        return -1;
    }
    return 0;
}

#if 0
int spi_nand_read_write(uint32_t offset, uint32_t length, u_char *buffer, char opt) {
    uint32_t bi, pi, co, r_bi;
    uint32_t size = length, l;

    // start addr
    bi = BLK_IDX(offset, PCNT*(PSIZE+SSIZE));
    pi = PAG_IDX(offset, PCNT*(PSIZE+SSIZE), (PSIZE+SSIZE));
    co = COL_OFF(offset, (PSIZE+SSIZE));

    //printf("DD: start from blk_0x%x, pg_0x%x, col_0x%x, size %d to buffer(0x%x)\n",
    //    bi, pi, co, size, buffer);

    while (size>0) {
        if(-1 == get_good_block(bi, &r_bi)){
            puts("\nEE: block error!\n");
            return -1;
        }
#if !USE_BBT_SKIP
        if (-1 == go_to_good_block(&r_bi)) {
            puts("\nEE: go good block error!\n");
            return -1;
        }
        bi = r_bi;
#endif
        // check column offset
        if (0<co) {
            puts("\nEE: column address incorrect!\n");
            return -1;
        } else if (0==co && size>(PSIZE+SSIZE)) {
            l=PSIZE+SSIZE;
            (opt)?nasu_page_write(buffer, BLOCK_PAGE_ADDR(r_bi, pi))
                :nasu_page_read(buffer, BLOCK_PAGE_ADDR(r_bi, pi));
        } else {
            //l=(size>((PSIZE+SSIZE)-co))?((PSIZE+SSIZE)-co):size;
            l=size;
            (opt)?nasu_pio_write(buffer, l, BLOCK_PAGE_ADDR(r_bi, pi), co)
                :nasu_pio_read(buffer, l, BLOCK_PAGE_ADDR(r_bi, pi), co);
        }
        //printf("DD: nasu_pio_%s buf 0x%x, len %d, 0x%x (b %d(%d), p %d), col 0x%x)\n",
        //    (opt)?"write":"read", (u32_t)buffer, l, BLOCK_PAGE_ADDR(r_bi, pi), r_bi, bi, pi, co);
        size-=l;
        co=0;
        buffer+=l;
        if (++pi >= PCNT) {  // block idx increased
            bi++;
            pi=0;
        }
        printf("%03d%%\b\b\b\b", 100-(100*size)/length);
    }
    return 0;
}

int spi_nand_write_ecc(uint32_t offset, uint32_t length, u_char *buffer) {
    uint32_t bi, pi, co, r_bi;
    uint32_t size = length, l;

    // start addr
    bi = BLK_IDX(offset, BSIZE);
    pi = PAG_IDX(offset, BSIZE, PSIZE);
    co = COL_OFF(offset, PSIZE);

    while (size>0) {
        if(-1 == get_good_block(bi, &r_bi)){
            puts("\nEE: block error!\n");
            return -1;
        }
#if !USE_BBT_SKIP
        if (-1 == go_to_good_block(&r_bi)) {
            puts("\nEE: go good block error!\n");
            return -1;
        }
        bi = r_bi;
#endif
        memset(_pbuf+_info.page_size, 0xFF, PBUF_SIZE-_info.page_size);
        // check column offset
        if (0<co) {
            // read first
            //ret=nasu_page_read_ecc(_pbuf, BLOCK_PAGE_ADDR(r_bi, pi), _eccbuf);
            //if (_spi_nand_check_ecc(ret)) return -1;

            l=(size>(PSIZE-co))?(PSIZE-co):size;

        } else {  //0==co
            if (size>=PSIZE) {
                l=PSIZE;
            } else {
                //ret=nasu_page_read_ecc(_pbuf, BLOCK_PAGE_ADDR(r_bi, pi), _eccbuf);
                //if (_spi_nand_check_ecc(ret)) return -1;
                l=size;
            }
        }
        memcpy(_pbuf+co, buffer, l);
        #if 0
        if (is_blank_data((uint32_t*)_pbuf, _info.page_size>>2)) {
        } else {
            ret=nasu_page_write_ecc(_pbuf, BLOCK_PAGE_ADDR(r_bi, pi), _eccbuf);
        }
        #else
        if (is_blank_data((uint32_t*)_pbuf, _info.page_size>>2)) {
            printf("Ignore to write blank data in %x\n", BLOCK_PAGE_ADDR(r_bi, pi));
        } else {
        nasu_ecc_encode(_pbuf, _eccbuf);
            if(nasu_page_write(_pbuf, BLOCK_PAGE_ADDR(r_bi, pi))) return -1;
        }
        #endif

        size-=l;
        co=0;
        buffer+=l;
        if (++pi >= PCNT) {  // block idx increased
            bi++;
            pi=0;
        }
        //printf("DD: spi_nand_write_ecc(0x%x, %d, 0x%x)\n",
        //        buffer, l, BLOCK_PAGE_ADDR(r_bi, pi), co);
    }
    return 0;
}

int spi_nand_read_ecc(uint32_t offset, uint32_t length, u_char *buffer) {
    uint32_t bi, pi, co, r_bi;
    uint32_t size = length, l;
    int ret;

    // start addr
    bi = BLK_IDX(offset, BSIZE);
    pi = PAG_IDX(offset, BSIZE, PSIZE);
    co = COL_OFF(offset, PSIZE);

    memset(buffer, 0, length);
    while (size>0) {
        if(-1 == get_good_block(bi, &r_bi)){
            puts("\nEE: block error!\n");
            return -1;
        }
#if !USE_BBT_SKIP
        if (-1 == go_to_good_block(&r_bi)) {
            puts("\nEE: go good block error!\n");
            return -1;
        }
        bi = r_bi;
#endif
        ret=nasu_page_read_ecc(_pbuf, BLOCK_PAGE_ADDR(r_bi, pi), _eccbuf);
        //printf("DD: ret=%x, _pbuf[0]=0x%x, _eccbuf[0]=0x%x, BPA=0x%x(r_bi %d, pi %d)\n",
	//	ret, _pbuf[0], _eccbuf[0], BLOCK_PAGE_ADDR(r_bi, pi), r_bi, pi);
        if (_spi_nand_check_ecc(ret)) return -1;

        // check column offset
        if (0<co) {
            l=(size>(PSIZE-co))?(PSIZE-co):size;
        } else { //0==co
            l=(size>PSIZE)?PSIZE:size;
        }
        memcpy(buffer, _pbuf+co, l);
        size-=l;
        co=0;
        buffer+=l;
        if (++pi >= PCNT) {  // block idx increased
            bi++;
            pi=0;
        }
    }
    return 0;
}

int spi_nand_erase(uint32_t offset, uint32_t length) {
    uint32_t bi, cnt=0, r_bi;
    int size = length;

    bi = BLK_IDX(offset, BSIZE);

    //printf("DD: block erase from 0x%x (block %d) BSIZE 0x%x.\n", offset, bi, BSIZE);
    while (size>0) {
        if(-1 == get_good_block(bi, &r_bi)){
            puts("\nEE: block error!\n");
            return -1;
        }
#if !USE_BBT_SKIP
        if (-1 == go_to_good_block(&r_bi)) {
            puts("\nEE: go good block error!\n");
            return -1;
        }
        bi = r_bi;
#endif
        //r_bi = bi; // for raw erase test

        //printf("DD: block erase %d(%d) block (0x%x)\n", r_bi, bi, BLOCK_PAGE_ADDR(r_bi, 0));
        if (-1==nasu_block_erase(BLOCK_PAGE_ADDR(r_bi, 0))) {
            puts("\nEE: erase failed!\n");
            return -1;
        }
        size-=BSIZE;
        cnt++;
        bi++;
    }
    //printf("from blk #%d size 0x%x is erased\n", BLK_IDX(offset, BSIZE), length);
    printf("%d block(s) erased \n", cnt);
    return 0;
}
#endif
