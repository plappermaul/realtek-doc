/******************************************************************************
 * $Id: luan_spi_nand.c,v 1.0 2016/06/13   Exp $
 * drivers/mtd/nand/rtk_nand.c
 * Overview: Realtek NAND Flash Controller Driver
 * Copyright (c) 2016 Realtek Semiconductor Corp. All Rights Reserved.
 *******************************************************************************/


#define DEBUG

#define NSU_DRIVER_IN_ROM
//#include <linux/device.h>
#undef DEBUG
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/mtd/nand.h>
#include <spi_nand/spi_nand_common.h>
#include "spi_nand/spi_nand_struct.h"
#include "ecc/ecc_struct.h"
#include <osc.h>
#include <asm/otto_util.h>
#include <malloc.h>

#define BANNER  "Realtek Luna SPI NAND Flash Driver"
#define VERSION  "$Id: luna_spi_nand.c,2016/06/13 00:00:00 Management Switch Product Exp $"

#define MAX_ALLOWED_ERR_IN_BLANK_PAGE (4)
#define RTK_MAX_FLASH_NUMBER 2

#define EBUF_SIZE                   (MAX_ECC_BUF_SIZE)
#define PBUF_SIZE                   (MAX_PAGE_BUF_SIZE+MAX_OOB_BUF_SIZE)

#define DEV_ID_SFT_BITS(id)         ((id>0xff)?8:0)
#define DEV_ID_MSB(id)              (id>>DEV_ID_SFT_BITS(id))
#define DEV_ID_LSB(id)              ((id>0xFF)?(id&0xFF):0)

#define printk printf
//static u_char _pbuf[PBUF_SIZE] DATA_ALIGNED32;
//static u_char _ebuf[EBUF_SIZE] DATA_ALIGNED32;

struct mtd_info *rtk_mtd[2];
extern spi_nand_flash_info_t *spi_nand_probe(uint32_t *count);
extern int      spi_nand_check_ecc(int r);
extern uint32_t spi_nand_chip_size(void);
extern uint32_t spi_nand_block_size(void);
extern uint32_t spi_nand_page_size(void);
extern uint32_t spi_nand_spare_size(void);
extern uint32_t spi_nand_oob_size(void);
extern uint32_t spi_nand_cs_count(void);
extern spi_nand_flash_info_t *spi_nand_flash_info(void);
extern int spi_nand_read_ecc(uint32_t offset, uint32_t length, u_char *buffer);
extern u32_t spi_nand_get_feature(u32_t cs, u32_t feature_addr);
//static uint32_t verify_page_addr;
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


struct luna_nand_t {
	struct nand_chip chip;
    spi_nand_flash_info_t *_spi_nand_info;
	u8_t *_oob_poi;
	unsigned int _writesize;
	unsigned int _spare_size;
	unsigned int readIndex;
	unsigned int writeIndex;
	unsigned int pre_command;
	unsigned int address_shift; // block number shift for different nand chip
	unsigned int static_shift;  // static block number chip for use different nand chip
	int selected_chip;
	int column;
	int page_addr;
	union {
		unsigned int v;
		unsigned char b[4];
		unsigned short w[2];
	}status;
	u8_t *_bbt;
	u32_t *_bbt_table;
	unsigned char *_ecc_buf;
	unsigned char *_page_buf;
};

//static struct spi_nand_flash_info_s spi_nand_flash_info;
//static struct spi_nand_flash_info_s *_spi_nand_info = &spi_nand_flash_info;
//static struct spi_nand_flash_info_s *_spi_nand_info = NULL;
static u32_t chip_count = 0; /* Add by MSP */


static u32_t RTK_NAND_PAGE_BUF_SIZE = (MAX_PAGE_BUF_SIZE+MAX_OOB_BUF_SIZE);
/* NOTE: Need to be changed if use a flash with more block */
#define MAX_BLOCKS (1024<<3)

static unsigned char *_ecc_buf  = NULL;
static unsigned char *_page_buf = NULL;
//static u8_t *_bbt               = NULL;
//static u32_t *_bbt_table        = NULL;

static struct nand_ecclayout nand_bch_oob_64 = {
    .eccbytes = 40, //ecc 40byte, + 4 bbi
	.eccpos = {
			24, 25, 26, 27, 28, 29, 30, 31, 32, 33,
			34, 35, 36, 37, 38, 39, 40, 41, 42, 43,
			44, 45, 46, 47, 48, 49, 50, 51, 52, 53,
			54, 55, 56, 57, 58, 59, 60, 61, 62, 63},
    .oobfree = {{.offset = 2, .length = 22}}
};

static struct nand_ecclayout nand_bch_oob_128 = {
    .eccbytes = 80, //ecc 40byte, + 4 bbi + ecc 40byte
	.eccpos = {
             48, 49, 50, 51, 52, 53, 54, 55, 56, 57,
             58, 59, 60, 61, 62, 63, 64, 65, 66, 67,
             68, 69, 70, 71, 72, 73, 74, 75, 76, 77,
             78, 79, 80, 81, 82, 83, 84, 85, 86, 87,
			88, 89, 90, 91, 92, 93, 94, 95, 96, 97,
			98, 99, 100, 101, 102, 103, 104, 105, 106, 107,
            108,109,110,111,112,113,114,115,116,117,
            118,119,120,121,122,123,124,125,126,127},
    .oobfree = {{.offset = 2, .length = 46}}
};

#if 0

#ifdef CONFIG_MTD_CMDLINE_PARTS
/* for fixed partition */
const char *ptypes[] = {"cmdlinepart", NULL};
//const char *ptypes[] = {"mtdparts=rtk_nand:640k@0(boot),6M@0x180000(linux),-(rootfs)", NULL};
#else
//eric, use cmdlinepart now
static struct mtd_partition def_parts[] = {
    { name: "boot",     offset: 0,          size: 0x100000,     mask_flags:0},
    { name: "env",      offset: 0x100000,   size: 0x20000,      mask_flags:0},
    { name: "env",      offset: 0x120000,   size: 0x20000,      mask_flags:0},
};
#endif
#endif

extern s32_t ecc_engine_action(u32_t ecc_ability, void *dma_addr, void *p_eccbuf, u32_t is_encode);

static u8_t
rtk_nand_get_spi_nand_status(u32_t cs)
{
    u32_t b0 = spi_nand_get_feature(cs, 0xB0);
    u32_t c0 = spi_nand_get_feature(cs, 0xC0);
    // Write Protect | BUSY | Generic Error
    u8_t s = (((~b0>>7)&0x1)<<7) | ((~c0&0x1) <<6) | (((c0>>2)&0x3)?1:0);
    return s;
}

static u8_t
rtk_nand_read_byte(struct mtd_info *mtd)
{
	struct luna_nand_t *chip = (struct luna_nand_t *)mtd->priv;

	if (chip->readIndex > 3) {
		return 0;
	}

	return chip->status.b[chip->readIndex++];
}

static u16_t
rtk_nand_read_word(struct mtd_info *mtd)
{
	struct luna_nand_t *chip = (struct luna_nand_t *)mtd->priv;
	u16_t reval;
	if (chip->readIndex > 3) {
		return 0;
	}

	chip->readIndex &= -2;
	reval = *((u16_t*)(&chip->status.w[(chip->readIndex)>>1]));
	chip->readIndex += 2;
	return reval;
}

static void
rtk_nand_write_buf(struct mtd_info *mtd, const u8_t *buf, int len)
{
	register struct luna_nand_t *chip = (struct luna_nand_t *)mtd->priv;

	if (chip->column+len > RTK_NAND_PAGE_BUF_SIZE) {
		return;
	}
	memcpy( chip->_page_buf+chip->column, buf, len);
	chip->column += len;
}

static void
rtk_nand_read_buf(struct mtd_info *mtd, u8_t *buf, int len)
{
	register struct luna_nand_t *chip = (struct luna_nand_t *)mtd->priv;

	len = len < (RTK_NAND_PAGE_BUF_SIZE -  chip->column ) ? len : (RTK_NAND_PAGE_BUF_SIZE -  chip->column );
	memcpy( buf, chip->_page_buf+chip->column, len);
	chip->readIndex = chip->column + len;
	chip->column +=   len;
}

static void
rtk_nand_select_chip(struct mtd_info *mtd, int chip_num)
{
	register struct luna_nand_t *chip = (struct luna_nand_t *)mtd->priv;
	if (chip_num >= RTK_MAX_FLASH_NUMBER) {
		printk("Select not correct Flash Number %d\n", chip_num);
		BUG();
    } else {
        chip->selected_chip = chip_num;
	}
}

static int
rtk_nand_waitfunc(struct mtd_info* mtd, struct nand_chip *this)
{
	struct luna_nand_t *chip = (struct luna_nand_t *)mtd->priv;
	// it will returh the status
    chip->_spi_nand_info->_model_info->_wait_spi_nand_ready(chip->selected_chip);
    //return chip->status.b[0];
    int st = rtk_nand_get_spi_nand_status(chip->selected_chip);
    return st;
}

#define SIGNATURE_PLR_FL         0x27317223
#define CHECK_FL_SECTION(buf)   (SIGNATURE_PLR_FL==*((uint32_t *)((buf)+0x200))&&0xFFFF==*((uint16_t *)((buf)+0x21A)))

static void
rtk_nand_cmdfunc(struct mtd_info *mtd, unsigned command, int column, int page_addr)
{
	register struct luna_nand_t *chip = (struct luna_nand_t *)mtd->priv;
	switch (command) {
	case NAND_CMD_READ0:
	case NAND_CMD_READOOB:
        chip->_spi_nand_info->_model_info->_page_read_ecc(chip->_spi_nand_info, chip->_page_buf, page_addr, chip->_ecc_buf);
        chip->column = (command==NAND_CMD_READ0)?column:chip->_writesize;
		chip->page_addr = page_addr;
		break;
	case NAND_CMD_CACHEDPROG:
	case NAND_CMD_PAGEPROG:
        /*chip->status.v = 0;
        chip->_spi_nand_info->_model_info->_page_write_ecc(chip->_spi_nand_info, chip->_page_buf, chip->page_addr, chip->_ecc_buf);
        chip->status.b[0] = rtk_nand_get_spi_nand_status(chip->selected_chip);
        chip->readIndex = 0;*/
		break;
	case NAND_CMD_SEQIN:
		chip->pre_command = NAND_CMD_SEQIN;
		chip->column = column;
		chip->page_addr = page_addr;
		break;
	case NAND_CMD_STATUS:
		chip->status.v = 0;
        chip->status.b[0] = rtk_nand_get_spi_nand_status(chip->selected_chip);
		chip->readIndex = 0;
		break;
	case NAND_CMD_RESET:
		//chip->_spi_nand_info->_reset();
		break;
	case NAND_CMD_READID:
		chip->status.v = 0;
		chip->status.b[0] = chip->_spi_nand_info->man_id;
        chip->status.b[1] = DEV_ID_MSB(chip->_spi_nand_info->dev_id);
        chip->status.b[2] = DEV_ID_LSB(chip->_spi_nand_info->dev_id);
		chip->readIndex = 0;
		break;
	case NAND_CMD_ERASE1:
        chip->status.v = 0;
        chip->_spi_nand_info->_model_info->_block_erase(chip->_spi_nand_info, page_addr);
        chip->status.b[0] = rtk_nand_get_spi_nand_status(chip->selected_chip);
		chip->readIndex = 0;
	case NAND_CMD_ERASE2:
		break;

		/*
		 * read error status commands require only a short delay
		 */

	case NAND_CMD_RNDOUT:
	case NAND_CMD_RNDIN:


	default:
        printk("Use NOT SUPPORTED command in NAND flash! 0x%x\n", command);
		BUG();
		break;
		/*
		 * If we don't have access to the busy pin, we apply the given
		 * command delay
		 */

	}
	chip->pre_command = command;
	return;
}

static void
rtk_nand_bug(struct mtd_info *mtd)
{
	printk("Use an unsupported function in MTD NAND driver");
	BUG();
}


static int
rtk_nand_read_page_hwecc(struct mtd_info *mtd,
					     struct nand_chip *_chip,
					     u8_t *buf, int oob_required, int page)
{
    int ret;
	register struct luna_nand_t *chip = (struct luna_nand_t *)mtd->priv;
    ret=chip->_spi_nand_info->_model_info->_page_read_ecc(chip->_spi_nand_info, chip->_page_buf, page, chip->_ecc_buf);
    if(ret) {
        if(CHECK_FL_SECTION(chip->_page_buf)) {
            puts("(OTTO First Load Pages)\n");
        } else if(ECC_CTRL_ERR == (ret&ECC_CTRL_ERR)) {
            mtd->ecc_stats.failed++;
            //printf("DD: block changes to bad in page_addr(0x%x) ret(0x%x), (0x%x)\n", page, ret, ret&ECC_CTRL_ERR);
        } else {
            mtd->ecc_stats.corrected += ret; // max correct bits over four sectors
            //printf("DD: %d bit-flip is dectected in page_addr(0x%x)\n", ret, page);
        }
    }
    //printf("DD: corrected: %d, failed: %d\n", mtd->ecc_stats.corrected, mtd->ecc_stats.failed);
	memcpy( buf, chip->_page_buf, chip->_writesize) ;
	memcpy(_chip->oob_poi, chip->_oob_poi, chip->_spare_size);
		return 0;
	}

static int
rtk_nand_read_page(struct mtd_info *mtd,
					     struct nand_chip *_chip,
					     u8_t *buf, int page)
{
    return rtk_nand_read_page_hwecc(mtd, _chip, buf, 0, page);
}

static int
rtk_nand_read_page_raw(struct mtd_info *mtd,
                         struct nand_chip *_chip,
                         u8_t *buf, int page)
{
    register struct luna_nand_t *chip = (struct luna_nand_t *)mtd->priv;
    chip->_spi_nand_info->_model_info->_page_read(chip->_spi_nand_info, chip->_page_buf, page);
    memcpy( buf, chip->_page_buf, chip->_writesize) ;
    memcpy(_chip->oob_poi, chip->_oob_poi, chip->_spare_size);
    return 0;
}

static int
rtk_nand_write_page_hwecc(struct mtd_info *mtd,
					      struct nand_chip *_chip,
					      const uint8_t *buf, int oob_required)
{
	register struct luna_nand_t *chip = (struct luna_nand_t *)mtd->priv;
	memcpy(chip->_page_buf, buf, chip->_writesize) ;
    memcpy(chip->_ecc_buf, _chip->oob_poi, chip->_spare_size);
    chip->status.v = 0;
    chip->_spi_nand_info->_model_info->_page_write_ecc(chip->_spi_nand_info, chip->_page_buf, chip->page_addr, chip->_ecc_buf);
    chip->status.b[0] = rtk_nand_get_spi_nand_status(chip->selected_chip);
    chip->readIndex = 0;
	return 0;
}

static void
rtk_nand_write_page(struct mtd_info *mtd,
					      struct nand_chip *_chip,
					      const uint8_t *buf)
{
    rtk_nand_write_page_hwecc(mtd, _chip, buf, 0);
}

static void
rtk_nand_write_page_raw(struct mtd_info *mtd,
                          struct nand_chip *_chip,
                          const uint8_t *buf)
{
    register struct luna_nand_t *chip = (struct luna_nand_t *)mtd->priv;
    memcpy(chip->_page_buf, buf, chip->_writesize) ;
    memcpy(chip->_page_buf+chip->_writesize, _chip->oob_poi, chip->_spare_size);
    chip->status.v = 0;
    chip->_spi_nand_info->_model_info->_page_write(chip->_spi_nand_info, chip->_page_buf, chip->page_addr);
    chip->status.b[0] = rtk_nand_get_spi_nand_status(chip->selected_chip);
    chip->readIndex = 0;
}

#define LUNA_SPI_OPTION (NAND_NO_SUBPAGE_WRITE)
static void
rtk_nand_ids(struct mtd_info *mtd)
{
	struct luna_nand_t *chip = (struct luna_nand_t *)mtd->priv;
    nand_flash_ids[0].name      = "ids0";
    nand_flash_ids[0].id        = chip->_spi_nand_info->dev_id >> ((chip->_spi_nand_info->dev_id&0xFF00)?8:0);
    nand_flash_ids[0].chipsize  = (mtd->size) >> 20;
    nand_flash_ids[0].pagesize  = mtd->writesize;
    nand_flash_ids[0].erasesize = mtd->erasesize;
    nand_flash_ids[0].options   = LUNA_SPI_OPTION;

#if 0
    int i;
    for(i=1; i<chip_count; i++){
        nand_flash_ids[i].name      = "ids1";
        nand_flash_ids[i].id        = nand_flash_ids[0].id        ;
        nand_flash_ids[i].chipsize  = nand_flash_ids[0].chipsize  ;
        nand_flash_ids[i].pagesize  = nand_flash_ids[0].pagesize  ;
        nand_flash_ids[i].erasesize = nand_flash_ids[0].erasesize ;
        nand_flash_ids[i].options   = nand_flash_ids[0].options   ;
    }
#endif
}

static void
rtk_nand_set(struct mtd_info *mtd)
{
	register struct nand_chip *chip = (struct nand_chip *)mtd->priv;

	if (!chip->chip_delay)
		chip->chip_delay = 20;

	chip->cmdfunc = rtk_nand_cmdfunc;
	chip->waitfunc = rtk_nand_waitfunc;

	chip->read_byte = rtk_nand_read_byte;
	chip->read_word = rtk_nand_read_word;
	chip->select_chip = rtk_nand_select_chip;
	chip->write_buf = rtk_nand_write_buf;
	chip->read_buf = rtk_nand_read_buf;

    // use default
    //chip->erase_cmd = rtk_nand_erase_cmd;
    //chip->block_bad = rtk_nand_block_bad;
    //chip->block_markbad = rtk_nand_block_markbad;
    //chip->scan_bbt = rtk_nand_scan_bbt;
}

static void
rtk_luna_nand_set(struct mtd_info *mtd)
{
	struct luna_nand_t *chip = (struct luna_nand_t *)mtd->priv;
    chip->_writesize = mtd->writesize;//SNAF_PAGE_SIZE(chip->_spi_nand_info);
	chip->_oob_poi =  (chip->_page_buf) + (chip->_writesize);
    chip->_spare_size = spi_nand_spare_size();//SNAF_SPARE_SIZE(chip->_spi_nand_info);
}

// FIXME!! need!!??
static void
rtk_luna_nand_chk(struct mtd_info *mtd)
{
	register struct luna_nand_t *chip = (struct luna_nand_t *)mtd->priv;

    //if (mtd->writesize != SNAF_PAGE_SIZE(chip->_spi_nand_info)) {
    if (mtd->writesize != chip->_writesize) {
		printk("SPI NAND: ERROR! Page size info not match, please check id table!\n");
		BUG();
	}

}

#if 1
static int
rtk_nand_ecc_set(struct mtd_info *mtd)
{
	struct luna_nand_t *chip = (struct luna_nand_t *)mtd->priv;
	register struct nand_ecc_ctrl *ecc = &(((struct nand_chip *)mtd->priv)->ecc);

	ecc->mode = NAND_ECC_HW;
	ecc->size = 512;
	ecc->bytes = 10;
	if (2048 == chip->_writesize) {
		ecc->layout = &nand_bch_oob_64;
	} else if (4096 == chip->_writesize ) {
		ecc->layout = &nand_bch_oob_128;
	} else {
		printk("%s: Error! unsupported page size %d\n", __FUNCTION__, chip->_writesize);
		return -1;
	}

    ecc->mode = NAND_ECC_HW;
	ecc->calculate = (void*)rtk_nand_bug;
	ecc->correct = (void*)rtk_nand_bug;

    ecc->read_page = rtk_nand_read_page;
    ecc->write_page = rtk_nand_write_page;
    ecc->read_page_raw = rtk_nand_read_page_raw;
    ecc->write_page_raw = rtk_nand_write_page_raw;
	ecc->read_subpage = (void*)rtk_nand_bug;
	ecc->strength = 6;
	return 0;
}

#endif

static int rtk_mtd_create_buffer(struct mtd_info *mtd);

/*************************************************************************
**  rtk_spi_nand_profile()
**	descriptions: rtk luna spi nand driver init function
**	parameters:
**	return:
**  note:
*************************************************************************/
static int
rtk_spi_nand_profile (struct mtd_info *mtd)
{
	//char *ptype;
	//int pnum = 0;
	//struct mtd_partition *mtd_parts;
    register struct luna_nand_t *chip = (struct luna_nand_t *)mtd->priv;

    chip->_spi_nand_info = spi_nand_flash_info();
#if 1
    if (rtk_mtd_create_buffer(mtd)) {
		return -1;
	}
#endif
    rtk_luna_nand_set(mtd);

    rtk_nand_set(mtd);
#if 1
    if (rtk_nand_ecc_set(mtd))
		return -1;
    rtk_nand_ids(mtd);
#endif
#if 1
    if(nand_scan(mtd, 1)){
		printk ("Warning: rtk nand scan error!\n");
		return -1;
	}
#endif

    rtk_luna_nand_chk(mtd);

#if 1
	/* check page size(write size) is 512/2048/4096.. must 512byte align */
    if (!(mtd->writesize & (0x200 - 1)))
		;//rtk_writel( rtk_mtd->oobblock >> 9, REG_PAGE_LEN);
	else {
		printk ("Error: pagesize is not 512Byte Multiple");
		return -1;
	}
#endif

#ifdef CONFIG_MTD_CMDLINE_PARTS
	/* partitions from cmdline */
	ptype = (char *)ptypes[0];
	pnum = parse_mtd_partitions (rtk_mtd, ptypes, &mtd_parts, 0);

	if (pnum <= 0) {
		printk (KERN_NOTICE "RTK: using the whole nand as a partitoin\n");
		if (add_mtd_device (rtk_mtd)) {
			printk (KERN_WARNING "RTK: Failed to register new nand device\n");
			return -EAGAIN;
		}
	} else {
		printk (KERN_NOTICE "RTK: using dynamic nand partition\n");
		if (add_mtd_partitions (rtk_mtd, mtd_parts, pnum)) {
			printk("%s: Error, cannot add %s device\n",
					__FUNCTION__, rtk_mtd->name);
			rtk_mtd->size = 0;
			return -EAGAIN;
		}
	}
#else
	/* fixed partition ,modify rtl8686_parts table*/
	printk("RTK: Register new nand device\n");

    if (add_mtd_device (mtd)) {
		printk(KERN_WARNING "RTK: Failed to register new nand device\n");
		//return -EAGAIN;
		return -11;
	}
#endif
	return 0;
}

static int
rtk_mtd_create_buffer(struct mtd_info *mtd)
{
	struct luna_nand_t *chip = (struct luna_nand_t *)mtd->priv;
    //const int bbt_table_len = (sizeof(*_bbt_table))*(NUM_WORD(SNAF_NUM_OF_BLOCK(chip->_spi_nand_info)));

    if (RTK_NAND_PAGE_BUF_SIZE < chip->_writesize + chip->_spare_size) {
        RTK_NAND_PAGE_BUF_SIZE = (chip->_writesize + chip->_spare_size);
	}
	_ecc_buf   = kmalloc((sizeof(*_ecc_buf))*MAX_ECC_BUF_SIZE + 32, GFP_KERNEL);
	_page_buf  = kmalloc((sizeof(*_page_buf))*RTK_NAND_PAGE_BUF_SIZE + 32, GFP_KERNEL);

    // align
	chip->_ecc_buf   = (__typeof__(*_ecc_buf)*)(((uintptr_t)_ecc_buf+31) & ~ (uintptr_t)0x1F);
	chip->_page_buf  = (__typeof__(*_page_buf)*)(((uintptr_t)_page_buf+31) & ~ (uintptr_t)0x1F);

    if (!_ecc_buf || !_page_buf) {
		printk ("%s: Error, no enough memory for buffer\n", __FUNCTION__);
		return -12;//ENOMEM;
	} else {
		return 0;
	}
}

#define MTDSIZE	(sizeof (struct mtd_info) + sizeof (struct luna_nand_t))
int rtk_spi_nand_init (void)
{
	struct nand_chip *this = NULL;
	//spi_nand_flash_info_t *fi;
	int rc = 0;
    //uint32_t count=1;

    rtk_mtd[0] = kmalloc (MTDSIZE, GFP_KERNEL); //mtd_info struct + nand_chip struct
    if (!rtk_mtd[0]) {
		printk ("%s: Error, no enough memory for rtk_mtd\n", __FUNCTION__);
		rc = -1;//ENOMEM;
	}
    memset ((char *)rtk_mtd[0], 0, MTDSIZE);
#if 1
    rtk_mtd[0]->priv = this = (struct nand_chip *)(rtk_mtd[0] + 1);
#endif

    chip_count             = spi_nand_cs_count();
    /* fill mtd info */
    rtk_mtd[0]->name       = "nand0";
    rtk_mtd[0]->size       = spi_nand_chip_size()*chip_count;
    rtk_mtd[0]->erasesize  = spi_nand_block_size();
    rtk_mtd[0]->writesize  = spi_nand_page_size();
    rtk_mtd[0]->oobsize    = spi_nand_oob_size();

    /* init spi nand flash */
    if (rtk_spi_nand_profile (rtk_mtd[0]) < 0) {
        rc = -1;
    }

    // try ...
#if 0   // could create 2 mtd, but need to figure out how to operate second one
    if(chip_count>1) {
        rtk_mtd[1] = kmalloc (MTDSIZE, GFP_KERNEL);
        if (!rtk_mtd[1]) {  // FIX
            printk ("%s: Error, no enough memory for rtk_mtd\n", __FUNCTION__);
            rc = -1;//ENOMEM;
        }
        rtk_mtd[1]->priv        = (struct nand_chip *)(rtk_mtd[1] + 1);
        rtk_mtd[1]->name        = "nand1";
        rtk_mtd[1]->size        = rtk_mtd[0]->size      ;
        rtk_mtd[1]->erasesize   = rtk_mtd[0]->erasesize ;
        rtk_mtd[1]->writesize   = rtk_mtd[0]->writesize ;
        rtk_mtd[1]->oobsize     = rtk_mtd[0]->oobsize   ;

        rtk_spi_nand_profile (rtk_mtd[1]);
	}
#endif

	return rc;

}

struct mtd_info *
get_mtd_info(void) {
    return rtk_mtd[0];
}
