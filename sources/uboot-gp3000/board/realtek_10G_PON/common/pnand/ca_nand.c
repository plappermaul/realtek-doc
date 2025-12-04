/* SPDX-License-Identifier: GPL-2.0+ */

/*
 * Copyright (C) 2015-2020, Cortina-Access Incorporation.
 * Author: Jason Li <jason.li@cortina-access.com>
 *
 * Parallel NAND Controller Driver for CAxxxx based SoCs
 */

#include <common.h>
#include <nand.h>
#include <malloc.h>
#include <watchdog.h>
#include <linux/err.h>
#ifdef CONFIG_MTD_PARTITIONS
#include <linux/mtd/partitions.h>
#endif

#include <u-boot/crc.h>

#include <asm/io.h>
#include <linux/errno.h>
#include "ca_nand.h"

#ifdef CONFIG_JFFS2_NAND
#include <jffs2/jffs2.h>
#endif

#ifndef CONFIG_SYS_NAND_BASE_LIST
#define CONFIG_SYS_NAND_BASE_LIST { CONFIG_SYS_NAND_BASE }
#endif

DECLARE_GLOBAL_DATA_PTR;

#define OWN_DMA	0
#define OWN_SW	1

struct mtd_info cortina_nand_info[CONFIG_SYS_MAX_NAND_DEVICE];

static struct nand_chip nand_chip[CONFIG_SYS_MAX_NAND_DEVICE];

static const char default_nand_name[] = "ca_nand";
static __attribute__ ((unused))
char dev_name[CONFIG_SYS_MAX_NAND_DEVICE][8];

/* OOB placement block for use with hardware ecc generation */
/*#ifdef CONFIG_G3_NAND_HWECC*/
static struct nand_ecclayout ca_nand_ecclayout;

/* Define default oob placement schemes for large and small page devices */
#ifdef	CONFIG_CORTINA_PNAND_ECC_HW_BCH

static struct nand_ecclayout ca_nand_bch_oob_16 = {
	.eccbytes = 13,
	.eccpos = {0, 1, 2, 3, 6, 7, 8, 9, 10, 11, 12, 13, 14},
	.oobfree = {
		    {.offset = 15,
		     /* . length = 1}}  resever 1 for erase tags: 1 - 1 = 0 */
		     .length = 0}}	/* resever 1 for erase tags: 1 - 1 = 0 */
};

#else

static struct nand_ecclayout ca_nand_oob_8 = {
	.eccbytes = 3,
	.eccpos = {0, 1, 2},
	.oobfree = {
		    {.offset = 3,
		     .length = 2},
		    {.offset = 6,
		     .length = 2}}
};

static struct nand_ecclayout ca_nand_oob_16 = {
	.eccbytes = 6,
	.eccpos = {0, 1, 2, 3, 6, 7},
	.oobfree = {
		    {.offset = 8,
		     .length = 8}}
};

#endif

struct ca_nand_host {
	struct mtd_info mtd;
	struct nand_chip *nand;
	void __iomem *io_base;
	struct device *dev;
	unsigned int col_addr;
	unsigned int page_addr;
};

static struct ca_nand_host ca_host;
static struct ca_nand_host *host = &ca_host;

static unsigned int CHIP_EN;
static unsigned int *pread, *pwrite;
FLASH_TYPE_t flash_type;
FLASH_STATUS_t flash_status;
FLASH_NF_ACCESS_t nf_access;
FLASH_NF_COUNT_t nf_cnt;
FLASH_NF_COMMAND_t nf_cmd;
FLASH_NF_ADDRESS_1_t nf_addr1;
FLASH_NF_ADDRESS_2_t nf_addr2;
FLASH_NF_DATA_t nf_data;
FLASH_NF_ECC_STATUS_t ecc_sts;
FLASH_NF_ECC_CONTROL_t ecc_ctl;
FLASH_NF_ECC_OOB_t ecc_oob;
FLASH_NF_ECC_GEN0_t ecc_gen0;
FLASH_NF_ECC_GEN1_t ecc_gen1;
FLASH_NF_ECC_GEN2_t ecc_gen2;
FLASH_NF_ECC_GEN3_t ecc_gen3;
FLASH_NF_ECC_GEN4_t ecc_gen4;
FLASH_NF_ECC_GEN5_t ecc_gen5;
FLASH_NF_ECC_GEN6_t ecc_gen6;
FLASH_NF_ECC_GEN7_t ecc_gen7;
FLASH_NF_ECC_GEN8_t ecc_gen8;
FLASH_NF_ECC_GEN9_t ecc_gen9;
FLASH_NF_ECC_GEN10_t ecc_gen10;
FLASH_NF_ECC_GEN11_t ecc_gen11;
FLASH_NF_ECC_GEN12_t ecc_gen12;
FLASH_NF_ECC_GEN13_t ecc_gen13;
FLASH_NF_ECC_GEN14_t ecc_gen14;
FLASH_NF_ECC_GEN15_t ecc_gen15;
FLASH_FLASH_ACCESS_START_t flash_start;
FLASH_NF_ECC_RESET_t ecc_reset;
FLASH_FLASH_INTERRUPT_t flash_int_sts;
FLASH_FLASH_MASK_t flash_int_mask;
FLASH_NF_BCH_STATUS_t bch_sts;
FLASH_NF_BCH_ERROR_LOC01_t bch_err_loc01;
FLASH_NF_BCH_ERROR_LOC23_t bch_err_loc23;
FLASH_NF_BCH_ERROR_LOC45_t bch_err_loc45;
FLASH_NF_BCH_ERROR_LOC67_t bch_err_loc67;
FLASH_NF_BCH_CONTROL_t bch_ctrl;
FLASH_NF_BCH_OOB0_t reg_bch_oob;
FLASH_NF_BCH_OOB1_t bch_oob1;
FLASH_NF_BCH_OOB2_t bch_oob2;
FLASH_NF_BCH_OOB3_t bch_oob3;
FLASH_NF_BCH_OOB4_t bch_oob4;

/* DMA regs */
DMA_SEC_DMA_GLB_DMA_SSP_RX_CTRL_t dma_rxdma_ctrl;
DMA_SEC_DMA_GLB_DMA_SSP_TX_CTRL_t dma_txdma_ctrl;
DMA_SEC_DMA_SSP_Q_TXQ_CONTROL_t dma_txq5_ctrl;
DMA_SEC_DMA_SSP_Q_RXQ_BASE_DEPTH_t dma_rxq5_base_depth;
DMA_SEC_DMA_SSP_Q_RXQ_RPTR_t dma_rxq5_rptr;
DMA_SEC_DMA_SSP_Q_TXQ_BASE_DEPTH_t dma_txq5_base_depth;
DMA_SEC_DMA_SSP_Q_TXQ_WPTR_t dma_txq5_wptr;
DMA_SEC_DMA_SSP_Q_RXQ_COAL_INTERRUPT_t dma_ssp_rxq5_intsts;
DMA_SEC_DMA_SSP_Q_TXQ_COAL_INTERRUPT_t dma_ssp_txq5_intsts;

FLASH_FLASH_ACCESS_START_t tmp_access;

DMA_SSP_TX_DESC_T *tx_desc;
DMA_SSP_RX_DESC_T *rx_desc;

static void write_flash_ctrl_reg(unsigned int ofs, unsigned int data);
static unsigned int read_flash_ctrl_reg(unsigned int ofs);
static void check_flash_ctrl_status(void);
int ca_nand_block_checkbad(struct mtd_info *mtd, loff_t ofs, int getchip, int allowbbt);
int ca_nand_get_device(struct nand_chip *chip, struct mtd_info *mtd, int new_state);
uint8_t *ca_nand_fill_oob(struct nand_chip *chip, uint8_t * oob, struct mtd_oob_ops *ops);
int ca_nand_do_write_oob(struct mtd_info *mtd, loff_t to, struct mtd_oob_ops *ops);
int ca_nand_do_write_ops(struct mtd_info *mtd, loff_t to, struct mtd_oob_ops *ops);
int ca_nand_do_read_ops(struct mtd_info *mtd, loff_t from, struct mtd_oob_ops *ops);
int ca_nand_erase_nand(struct mtd_info *mtd, struct erase_info *instr, int allowbbt);
int ca_nand_check_wp(struct mtd_info *mtd);
unsigned int reg_wait(unsigned int regaddr, unsigned int mask, unsigned int val, int timeout);
void ca_nand_wait_ready(struct mtd_info *mtd);
void ca_nand_command(struct mtd_info *mtd, unsigned int command, int column, int page_addr);
void ca_nand_command_lp(struct mtd_info *mtd, unsigned int command, int column, int page_addr);

#ifdef DEBUG
#define nand_debug(fmt,args...)	printf("P-NAND DEBUG: " fmt,##args)
#else
#define nand_debug(fmt,args...)
#endif
#define nand_error(fmt,args...)	printf("P-NAND ERROR: " fmt,##args)


#ifdef CONFIG_NAND_HIDE_BAD
#define BAD_BLOCK_LIST_ELEMENT 128
#define BBL_INDICATORS	       4
#define BBL0_NAME 0x42626c30 //'Bbl0'
#define BBL1_NAME 0x316C6242 //'1lbB'

enum {
	FACTORY_1ST_SCAN = 1,
	FACTORY_FROM_BBT0= 2,
    FACTORY_FROM_BBT1= 3,
    FACTORY_UPDATED  = 4, //For debug use.
};
static uint8_t bbl_source=0;
static uint8_t bad_block_cnt=0;
static uint32_t bad_block_list[BAD_BLOCK_LIST_ELEMENT];
uint8_t acc_phy_offset=0;
static inline void nand_acc_phy_offs_addr(u16 enable)
{
	acc_phy_offset=enable;
}

int ca_nand_write(struct mtd_info *mtd, loff_t to, size_t len, size_t * retlen, const uint8_t * buf);
int ca_nand_erase(struct mtd_info *mtd, struct erase_info *instr);
int ca_nand_read(struct mtd_info *mtd, loff_t from, size_t len, size_t * retlen, uint8_t * buf);
int ca_nand_block_isbad(struct mtd_info *mtd, loff_t offs);

/*******************************************************
     After the first the factory bad block scanning
     Program the list to spi nand flash for later use
     The goal is to keep the original flash status and speed up booting

     This function will program two copy to flash in case.
     Each copy will be placed as:
         Page 0: Indicators for BBL
         Page 1: BBL content
*******************************************************/
static void store_bad_block_list_to_flash(struct mtd_info *mtd)
{
    struct nand_chip *chip = mtd->priv;
    struct erase_info instr;
    size_t retlen;
    uint32_t crc16=crc16_ccitt(0, (uint8_t *)bad_block_list, sizeof(bad_block_list));
    uint32_t indicator[BBL_INDICATORS] = {BBL0_NAME, bad_block_cnt, crc16, crc16};
    memset(&instr, 0, sizeof(instr));
    nand_acc_phy_offs_addr(0);

    instr.addr = NAND_BBL_OFFSET;
    instr.len = mtd->erasesize;
    ca_nand_erase(mtd, &instr);

    memset(chip->buffers->databuf, 0xFF, mtd->writesize);
    memcpy(chip->buffers->databuf,indicator, sizeof(indicator));
    ca_nand_write(mtd, instr.addr, mtd->writesize, &retlen, (const u8 *)chip->buffers->databuf);

    memset(chip->buffers->databuf, 0xFF, mtd->writesize);
    memcpy(chip->buffers->databuf,bad_block_list, sizeof(bad_block_list));
    ca_nand_write(mtd, instr.addr+mtd->writesize, mtd->writesize, &retlen, (const u8 *)chip->buffers->databuf);

    instr.addr = NAND_BBL_OFFSET_REDUND;
    instr.len = mtd->erasesize;
    ca_nand_erase(mtd, &instr);

    indicator[0] = BBL1_NAME;
    memset(chip->buffers->databuf, 0xFF, mtd->writesize);
    memcpy(chip->buffers->databuf,indicator, sizeof(indicator));
    ca_nand_write(mtd, instr.addr, mtd->writesize, &retlen, (const u8 *)chip->buffers->databuf);

    memset(chip->buffers->databuf, 0xFF, mtd->writesize);
    memcpy(chip->buffers->databuf,bad_block_list, sizeof(bad_block_list));
    ca_nand_write(mtd, instr.addr+mtd->writesize, mtd->writesize, &retlen, (const u8 *)chip->buffers->databuf);
}

static int xlat_offset_to_skip_factory_bad(struct mtd_info *mtd, loff_t *offset)
{
	/* Just return original offset:
	     1. if no bad block found
	     2. Physical flash offset access is assigned */
	if((bad_block_cnt==0) || (acc_phy_offset)){
		return 0;
    }

    struct nand_chip *chip = mtd->priv;
	uint32_t i=0;

	for(i=0; i<bad_block_cnt; i++){
		if(bad_block_list[i] <= *offset){
			*offset += mtd->erasesize;
		}else{
			break;
        }
	}

    if(*offset >= chip->chipsize){
        return -EINVAL;
    }

	return 0;
}

static void update_factory_bad_block_list(struct mtd_info *mtd, uint32_t offset, uint32_t is_add)
{
    struct nand_chip *chip = mtd->priv;
    uint32_t i=0;
    uint32_t local_buf[BAD_BLOCK_LIST_ELEMENT];
    memcpy(local_buf, bad_block_list, sizeof(local_buf));

    if(is_add){
        if(!bad_block_cnt){
            bad_block_list[bad_block_cnt++] = offset;
            goto end;
        }
        while(1){
            if(offset < bad_block_list[i]){
                bad_block_list[i] = offset;
                memcpy(&bad_block_list[i+1], &local_buf[i], sizeof(uint32_t)*(bad_block_cnt-i+1));
                bad_block_cnt++;
                goto end;
            }else if(offset == bad_block_list[i]){
                return;
            }else if((i+1)==bad_block_cnt){
                bad_block_list[i+1] = offset;
                bad_block_cnt++;
                goto end;
            }else{
                i++;
            }
        }
    }else{
        while(i<bad_block_cnt){
            if(offset != bad_block_list[i]){
                i++;
            }else{
                memcpy(&bad_block_list[i], &local_buf[i+1], sizeof(uint32_t)*(bad_block_cnt-i-1));
                bad_block_cnt--;
                bad_block_list[bad_block_cnt] = 0xFFFFFFFF;
                goto end;
            }
        }
    }

end:
    bbl_source = FACTORY_UPDATED;
    mtd->size = chip->chipsize - bad_block_cnt*mtd->erasesize;

    /* Update the latest one into flash chip */
    store_bad_block_list_to_flash(mtd);
    return;
}

static void scan_parallel_nand_factory_bad_blocks(struct mtd_info *mtd)
{
    /* First, check if the factory bad list is in flash chip?
         * 1) Read Bbt0 if the factory bad had already been scanned
         * 2) If Bbt0 is bad. Read Bbt1 for the back up copy
         */
    struct nand_chip *chip = mtd->priv;
    loff_t offset, start;
    size_t retlen, size;
    uint32_t ret=0;
    uint32_t indicator[BBL_INDICATORS];
    uint32_t tempread[BAD_BLOCK_LIST_ELEMENT];
    memset(indicator, 0xFF, sizeof(indicator));
    memset(tempread, 0xFF, sizeof(tempread));
    memset(bad_block_list, 0xFF, sizeof(bad_block_list));

    nand_acc_phy_offs_addr(1);
    for(offset=NAND_BBL_OFFSET ; offset<mtd->size ; offset+= mtd->erasesize){
        ca_nand_read(mtd, offset, sizeof(indicator), &retlen, (u8 *)indicator);
        if(((indicator[0]==BBL0_NAME) || (indicator[0]==BBL1_NAME)) && (indicator[2]==indicator[3])){
            ca_nand_read(mtd, offset+mtd->writesize, sizeof(tempread), &retlen, (u8 *)tempread);
            if(indicator[2] == crc16_ccitt(0, (u8 *)tempread, sizeof(tempread))){
                bad_block_cnt = indicator[1];
                bbl_source = (indicator[0]==BBL0_NAME)?FACTORY_FROM_BBT0:FACTORY_FROM_BBT1;
                memcpy(bad_block_list, tempread, sizeof(bad_block_list));
                mtd->size = chip->chipsize - bad_block_cnt*mtd->erasesize;
                goto found_list;
            }
        }
    }

    offset = 0;
    start = 0;
    size = mtd->size;
    bbl_source = FACTORY_1ST_SCAN;
    bad_block_cnt = 0;
    while(((offset - start) < size) && (bad_block_cnt<BAD_BLOCK_LIST_ELEMENT)) {
        ret = ca_nand_block_isbad(mtd, offset);
        if(ret == 1){
            bad_block_list[bad_block_cnt++] = offset;
        }
        offset += mtd->erasesize;
    }

    /* Fitst scan, store it into flash chip */
    store_bad_block_list_to_flash(mtd);

found_list:
    nand_acc_phy_offs_addr(0);
    return;
}

uint32_t nand_get_phy_offset(struct mtd_info *mtd, uint32_t viraddr)
{
    loff_t offset = viraddr;
    xlat_offset_to_skip_factory_bad(mtd, &offset);
    return offset;
}

void nand_print_bad_block_hidden_list(void)
{
    if(bbl_source==FACTORY_1ST_SCAN)
        puts("            BBL: Factory bad first scan\n");
    else if(bbl_source==FACTORY_UPDATED)
        printf("          BBL: Updated during debugging\n");
    else
        printf("          BBL: %s from flash\n",(bbl_source==FACTORY_FROM_BBT0)?"Bbl0":"Bbl1");

    puts("       Hidden: ");
    uint32_t i;
    for(i=0 ; i<bad_block_cnt ; i++){
        printf("0x%x ", bad_block_list[i]);
    }
    puts("\n");
    return;
}
#else
static inline void nand_acc_phy_offs_addr(u16 enable){}
#endif /* #ifdef CONFIG_NAND_HIDE_BAD */


#define NAND_MAX_CHIPS CONFIG_SYS_NAND_MAX_CHIPS
#define BBT_PAGE_MASK	0xffffff3f
#define	SZ_32M 0x8000
#define	SZ_128M 0x20000

static int dbg_ecc_check = 0;

uint32_t flash_reset(void)
{
	FLASH_NF_ACCESS_t nf_access;
	FLASH_NF_ECC_RESET_t nf_ecc_reset;

	nf_access.wrd = 0;
	nf_access.bf.autoReset = 1;
	write_flash_ctrl_reg(FLASH_NF_ACCESS, nf_access.wrd);

	nf_ecc_reset.wrd = 0;
	nf_ecc_reset.bf.nflash_reset = 1;
	write_flash_ctrl_reg(FLASH_NF_ECC_RESET, nf_ecc_reset.wrd);

	udelay(1);

	return 0;
}

static unsigned int read_flash_ctrl_reg(unsigned int ofs)
{
	return readl((unsigned long)ofs);
}

static void write_flash_ctrl_reg(unsigned int ofs, unsigned int data)
{
	writel(data, (unsigned long)ofs);
}

static unsigned int read_dma_global_reg(unsigned int ofs)
{
	return readl((const unsigned long)ofs);
}

void write_dma_global_reg(unsigned int ofs, unsigned int data)
{
	writel(data, (volatile unsigned long)ofs);
}

static unsigned int read_dma_ctrl_reg(unsigned int ofs)
{
//	return readl((const unsigned long)dma_ssp_base_address + ofs);
	return readl((const unsigned long)ofs);
}

void write_dma_ctrl_reg(unsigned int ofs, unsigned int data)
{
	writel(data, (volatile unsigned long)ofs);
}

/**
 * ca_nand_get_device - [GENERIC] Get chip for selected access
 * @chip:	the nand chip descriptor
 * @mtd:	MTD device structure
 * @new_state:	the state which is requested
 *
 * Get the device and lock it for exclusive access
 */
int ca_nand_get_device(struct nand_chip *this, struct mtd_info *mtd, int new_state)
{
	this->state = new_state;
	return 0;
}

/**
 * ca_nand_do_write_oob - [MTD Interface] NAND write out-of-band
 * @mtd:	MTD device structure
 * @to:		offset to write to
 * @ops:	oob operation description structure
 *
 * NAND write out-of-band
 */
int ca_nand_do_write_oob(struct mtd_info *mtd, loff_t to, struct mtd_oob_ops *ops)
{
	int chipnr, page, status, len;
	struct nand_chip *chip = mtd->priv;

#ifdef DEBUG
	printf("%s: to = 0x%08x, len = %i\n", __func__, (unsigned int)to, (int)ops->ooblen);
#endif

	if (ops->mode == MTD_OPS_AUTO_OOB)
		len = chip->ecc.layout->oobavail;
	else
		len = mtd->oobsize;

	/* Do not allow write past end of page */
	if ((ops->ooboffs + ops->ooblen) > len) {
		printf("%s: Attempt to write " "past end of page\n", __func__);
		return -EINVAL;
	}

	if (unlikely(ops->ooboffs >= len)) {
		printf("%s: Attempt to start " "write outside oob\n", __func__);
		return -EINVAL;
	}

	/* Do not allow reads past end of device */
	if (unlikely(to >= mtd->size ||
		     ops->ooboffs + ops->ooblen > ((mtd->size >> chip->page_shift) - (to >> chip->page_shift)) * len)) {
		printf("%s: Attempt write beyond " "end of device\n", __func__);
		return -EINVAL;
	}

	chipnr = (int)(to >> chip->chip_shift);
	chip->select_chip(mtd, chipnr);

	/* Shift to get page */
	page = (int)(to >> chip->page_shift);
#ifdef CONFIG_NAND_HIDE_BAD
    loff_t off_in_out = page<<chip->page_shift;
    int ret = xlat_offset_to_skip_factory_bad(mtd, &off_in_out);
    if(ret == -EINVAL){
        nand_error(KERN_ALERT "%s: offset xlat exceeds chip size\n",__FUNCTION__);
        return ret;
    }
    if((page<<chip->page_shift) != off_in_out){
        nand_debug(KERN_ALERT "read xlat: 0x%x => 0x%x\n",(page<<chip->page_shift),off_in_out);
    }
    page = off_in_out>>chip->page_shift;
#endif

	/*
	 * Reset the chip. Some chips (like the Toshiba TC5832DC found in one
	 * of my DiskOnChip 2000 test units) will clear the whole data page too
	 * if we don't do this. I have no clue why, but I seem to have 'fixed'
	 * it in the doc2000 driver in August 1999.  dwmw2.
	 */
	chip->cmdfunc(mtd, NAND_CMD_RESET, -1, -1);

	/* Check, if it is write protected */
	if (ca_nand_check_wp(mtd))
		return -EROFS;

	/* Invalidate the page cache, if we write to the cached page */
	if (page == chip->pagebuf)
		chip->pagebuf = -1;

	memset(chip->oob_poi, 0xff, mtd->oobsize);
	ca_nand_fill_oob(chip, ops->oobbuf, ops);
	status = chip->ecc.write_oob(mtd, chip, page & chip->pagemask);
	memset(chip->oob_poi, 0xff, mtd->oobsize);

	if (status)
		return status;

	ops->oobretlen = ops->ooblen;

	return 0;
}

/*
 * return register value after "(*reg) & mask == val", with timeout
 */
unsigned int reg_wait(unsigned int regaddr, unsigned int mask, unsigned int val, int timeout)
{
	unsigned int i, tmp;

	for (i = timeout; i > 0; i--) {
		tmp = read_flash_ctrl_reg(regaddr);
		if ((tmp & mask) == val)
			return 0;	//TRUE;
		udelay(5);
	}

	printf("reg_wait error !!  \n");

	return 1;		//FALSE;
}

unsigned int dma_wait(unsigned int regaddr, unsigned int mask, unsigned int val, int timeout)
{
	unsigned int i, tmp;

	for (i = timeout; i > 0; i--) {
		tmp = read_dma_ctrl_reg(regaddr);
		if ((tmp & mask) == val)
			return 0;	//TRUE;
		udelay(5);
	}

	printf("reg_wait error !!  \n");

	return 1;		//FALSE;
}

/*#ifdef CONFIG_G3_NAND_HWECC*/

int ca_nand_correct_data(struct mtd_info *mtd, u_char * dat, u_char * read_ecc, u_char * calc_ecc)
{
	/* Do nothing, data are corrected in read_page_page_hwecc */
	return 0;
}

int ca_nand_calculate_ecc(struct mtd_info *mtd, const u_char * dat, u_char * ecc_code)
{
	return 0;
}

void check_flash_ctrl_status(void)
{
	int rty = 0;

	flash_status.wrd = read_flash_ctrl_reg(FLASH_STATUS);
	while (flash_status.bf.nState) {
		flash_status.wrd = read_flash_ctrl_reg(FLASH_STATUS);
		udelay(5);
		rty++;
		if (rty > 50000) {
			printk("FLASH_STATUS ERROR: %x\n", flash_status.wrd);
			return;
		}
	}

}

/**
 * ca_nand_release_device - [GENERIC] release chip
 * @mtd:	MTD device structure
 *
 * Deselect, release chip lock and wake up anyone waiting on the device
 */
/* XXX U-BOOT XXX */
void ca_nand_release_device(struct mtd_info *mtd)
{
	struct nand_chip *this = mtd->priv;
	this->select_chip(mtd, -1);	/* De-select the NAND device */
}

void ca_nand_read_id(int chip_no, unsigned char *id)
{
	unsigned int opcode, i;
	const unsigned int extid = 8;
	FLASH_FLASH_ACCESS_START_t tmp_access;

	check_flash_ctrl_status();

	write_flash_ctrl_reg(FLASH_NF_ECC_CONTROL, 0x0);

	flash_type.wrd = read_flash_ctrl_reg(FLASH_TYPE);

	/*need to check extid byte counts */
	nf_cnt.wrd = 0;

	nf_cnt.bf.nflashRegOobCount = NCNT_EMPTY_OOB;
	nf_cnt.bf.nflashRegDataCount = NCNT_DATA_8;
	nf_cnt.bf.nflashRegAddrCount = NCNT_ADDR_1;
	nf_cnt.bf.nflashRegCmdCount = NCNT_CMD_1;

	write_flash_ctrl_reg(FLASH_NF_COUNT, nf_cnt.wrd);

	nf_cmd.wrd = 0;
	nf_cmd.bf.nflashRegCmd0 = NAND_CMD_READID;
	write_flash_ctrl_reg(FLASH_NF_COMMAND, nf_cmd.wrd);
	nf_addr1.wrd = 0;
	write_flash_ctrl_reg(FLASH_NF_ADDRESS_1, nf_addr1.wrd);
	nf_addr2.wrd = 0;
	write_flash_ctrl_reg(FLASH_NF_ADDRESS_2, nf_addr2.wrd);

	/* read maker code */
	nf_access.wrd = 0;
	nf_access.bf.nflashCeAlt = chip_no;

	nf_access.bf.nflashRegWidth = NFLASH_WiDTH8;
	write_flash_ctrl_reg(FLASH_NF_ACCESS, nf_access.wrd);

	for (i = 0; i < extid; i++) {

		flash_start.wrd = 0;
		flash_start.bf.nflashRegReq = FLASH_GO;
		flash_start.bf.nflashRegCmd = FLASH_RD;
		write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, flash_start.wrd);

		flash_start.wrd = read_flash_ctrl_reg(FLASH_FLASH_ACCESS_START);
		tmp_access.wrd = 0;
		tmp_access.bf.nflashRegReq = 1;
		reg_wait(FLASH_FLASH_ACCESS_START, tmp_access.wrd, 0, 2000);

		opcode = read_flash_ctrl_reg(FLASH_NF_DATA);
		id[i] = (unsigned char)((opcode >> ((i << 3) % 32)) & 0xff);
	}

	ecc_reset.wrd = 0;
	ecc_reset.bf.eccClear = ECC_CLR;
	ecc_reset.bf.fifoClear = FIFO_CLR;
	ecc_reset.bf.nflash_reset = NF_RESET;
	write_flash_ctrl_reg(FLASH_NF_ECC_RESET, ecc_reset.wrd);
}

/* Sanitize ONFI strings so we can safely print them */
static void sanitize_string(char *s, size_t len)
{
	ssize_t i;

	/* Null terminate */
	s[len - 1] = 0;

	/* Remove non printable chars */
	for (i = 0; i < len - 1; i++) {
		if (s[i] < ' ' || s[i] > 127)
			s[i] = '?';
	}

	/* Remove trailing spaces */
	strim(s);
}

static u16 onfi_crc16(u16 crc, u8 const *p, size_t len)
{
	int i;
	while (len--) {
		crc ^= *p++ << 8;
		for (i = 0; i < 8; i++)
			crc = (crc << 1) ^ ((crc & 0x8000) ? 0x8005 : 0);
	}

	return crc;
}

static int nand_setup_read_retry_micron(struct mtd_info *mtd, int retry_mode)
{
	struct nand_chip *chip = mtd_to_nand(mtd);
	uint8_t feature[ONFI_SUBFEATURE_PARAM_LEN] = { retry_mode };

	return chip->onfi_set_features(mtd, chip, ONFI_FEATURE_ADDR_READ_RETRY, feature);
}

/*
 * Configure chip properties from Micron vendor-specific ONFI table
 */
static void nand_onfi_detect_micron(struct nand_chip *chip, struct nand_onfi_params *p)
{
	struct nand_onfi_vendor_micron *micron = (void *)p->vendor;

	if (le16_to_cpu(p->vendor_revision) < 1)
		return;

	chip->read_retries = micron->read_retry_options;
	chip->setup_read_retry = nand_setup_read_retry_micron;
}

/* Parse the Extended Parameter Page. */
static int nand_flash_detect_ext_param_page(struct mtd_info *mtd, struct nand_chip *chip, struct nand_onfi_params *p)
{
	struct onfi_ext_param_page *ep;
	struct onfi_ext_section *s;
	struct onfi_ext_ecc_info *ecc;
	uint8_t *cursor;
	int ret = -EINVAL;
	int len;
	int i;

	len = le16_to_cpu(p->ext_param_page_length) * 16;
	ep = kmalloc(len, GFP_KERNEL);
	if (!ep)
		return -ENOMEM;

	/* Send our own NAND_CMD_PARAM. */
	chip->cmdfunc(mtd, NAND_CMD_PARAM, 0, -1);

	/* Use the Change Read Column command to skip the ONFI param pages. */
	chip->cmdfunc(mtd, NAND_CMD_RNDOUT, sizeof(*p) * p->num_of_param_pages, -1);

	/* Read out the Extended Parameter Page. */
	chip->read_buf(mtd, (uint8_t *) ep, len);
	if ((onfi_crc16(ONFI_CRC_BASE, ((uint8_t *) ep) + 2, len - 2)
	     != le16_to_cpu(ep->crc))) {
		pr_debug("fail in the CRC.\n");
		goto ext_out;
	}

	/*
	 * Check the signature.
	 * Do not strictly follow the ONFI spec, maybe changed in future.
	 */
	if (strncmp((char *)ep->sig, "EPPS", 4)) {
		pr_debug("The signature is invalid.\n");
		goto ext_out;
	}

	/* find the ECC section. */
	cursor = (uint8_t *) (ep + 1);
	for (i = 0; i < ONFI_EXT_SECTION_MAX; i++) {
		s = ep->sections + i;
		if (s->type == ONFI_SECTION_TYPE_2)
			break;
		cursor += s->length * 16;
	}
	if (i == ONFI_EXT_SECTION_MAX) {
		pr_debug("We can not find the ECC section.\n");
		goto ext_out;
	}

	/* get the info we want. */
	ecc = (struct onfi_ext_ecc_info *)cursor;

	if (!ecc->codeword_size) {
		pr_debug("Invalid codeword size\n");
		goto ext_out;
	}

	chip->ecc_strength_ds = ecc->ecc_bits;
	chip->ecc_step_ds = 1 << ecc->codeword_size;
	ret = 0;

 ext_out:
	kfree(ep);
	return ret;
}

void ca_nand_onfi_id(int chip_no, unsigned char *id)
{
	unsigned int opcode, i;
	FLASH_FLASH_ACCESS_START_t tmp_access;

	check_flash_ctrl_status();

	write_flash_ctrl_reg(FLASH_NF_ECC_CONTROL, 0x0);

	flash_type.wrd = read_flash_ctrl_reg(FLASH_TYPE);

	/*need to check extid byte counts */
	nf_cnt.wrd = 0;

	nf_cnt.bf.nflashRegOobCount = NCNT_EMPTY_OOB;
	nf_cnt.bf.nflashRegDataCount = NCNT_DATA_4;
	nf_cnt.bf.nflashRegAddrCount = NCNT_ADDR_1;
	nf_cnt.bf.nflashRegCmdCount = NCNT_CMD_1;

	write_flash_ctrl_reg(FLASH_NF_COUNT, nf_cnt.wrd);

	nf_cmd.wrd = 0;
	nf_cmd.bf.nflashRegCmd0 = NAND_CMD_READID;
	write_flash_ctrl_reg(FLASH_NF_COMMAND, nf_cmd.wrd);
	nf_addr1.wrd = 0x20;	/* subcommand 0x20 to get ONFI signature */
	write_flash_ctrl_reg(FLASH_NF_ADDRESS_1, nf_addr1.wrd);
	nf_addr2.wrd = 0;
	write_flash_ctrl_reg(FLASH_NF_ADDRESS_2, nf_addr2.wrd);

	/* read maker code */
	nf_access.wrd = 0;
	nf_access.bf.nflashCeAlt = chip_no;

	nf_access.bf.nflashRegWidth = NFLASH_WiDTH8;
	write_flash_ctrl_reg(FLASH_NF_ACCESS, nf_access.wrd);

	for (i = 0; i < 4; i++) {

		flash_start.wrd = 0;
		flash_start.bf.nflashRegReq = FLASH_GO;
		flash_start.bf.nflashRegCmd = FLASH_RD;
		write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, flash_start.wrd);

		flash_start.wrd = read_flash_ctrl_reg(FLASH_FLASH_ACCESS_START);
		tmp_access.wrd = 0;
		tmp_access.bf.nflashRegReq = 1;
		reg_wait(FLASH_FLASH_ACCESS_START, tmp_access.wrd, 0, 2000);

		opcode = read_flash_ctrl_reg(FLASH_NF_DATA);
		id[i] = (unsigned char)((opcode >> ((i << 3) % 32)) & 0xff);
	}

	ecc_reset.wrd = 0;
	ecc_reset.bf.eccClear = ECC_CLR;
	ecc_reset.bf.fifoClear = FIFO_CLR;
	ecc_reset.bf.nflash_reset = NF_RESET;
	write_flash_ctrl_reg(FLASH_NF_ECC_RESET, ecc_reset.wrd);
}

int ca_nand_flash_detect_onfi(struct mtd_info *mtd, struct nand_chip *chip, int *busw)
{
	unsigned int opcode;
	int i;
	int val;
	FLASH_FLASH_ACCESS_START_t tmp_access;
	struct nand_onfi_params *p = &chip->onfi_params;
	unsigned char onfi_sig[4], *onfi_param = (unsigned char *)p;

	ca_nand_onfi_id(0, onfi_sig);
	if ((onfi_sig[0] != 'O') || (onfi_sig[1] != 'N') || (onfi_sig[2] != 'F')
	    || (onfi_sig[3] != 'I'))
		return 0;

	check_flash_ctrl_status();

	write_flash_ctrl_reg(FLASH_NF_ECC_CONTROL, 0x0);

	flash_type.wrd = read_flash_ctrl_reg(FLASH_TYPE);

	/*need to check extid byte counts */
	nf_cnt.wrd = 0;

	nf_cnt.bf.nflashRegOobCount = NCNT_EMPTY_OOB;
	nf_cnt.bf.nflashRegDataCount = sizeof(*p) - 1;
	nf_cnt.bf.nflashRegAddrCount = NCNT_ADDR_1;
	nf_cnt.bf.nflashRegCmdCount = NCNT_CMD_1;

	write_flash_ctrl_reg(FLASH_NF_COUNT, nf_cnt.wrd);

	nf_cmd.wrd = 0;
	nf_cmd.bf.nflashRegCmd0 = NAND_CMD_PARAM;
	write_flash_ctrl_reg(FLASH_NF_COMMAND, nf_cmd.wrd);
	nf_addr1.wrd = 0x0;
	write_flash_ctrl_reg(FLASH_NF_ADDRESS_1, nf_addr1.wrd);
	nf_addr2.wrd = 0;
	write_flash_ctrl_reg(FLASH_NF_ADDRESS_2, nf_addr2.wrd);

	/* read maker code */
	nf_access.wrd = 0;
	nf_access.bf.nflashCeAlt = 0;

	nf_access.bf.nflashRegWidth = NFLASH_WiDTH8;
	write_flash_ctrl_reg(FLASH_NF_ACCESS, nf_access.wrd);

	for (i = 0; i < sizeof(*p); i++) {

		flash_start.wrd = 0;
		flash_start.bf.nflashRegReq = FLASH_GO;
		flash_start.bf.nflashRegCmd = FLASH_RD;
		write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, flash_start.wrd);

		flash_start.wrd = read_flash_ctrl_reg(FLASH_FLASH_ACCESS_START);
		tmp_access.wrd = 0;
		tmp_access.bf.nflashRegReq = 1;
		reg_wait(FLASH_FLASH_ACCESS_START, tmp_access.wrd, 0, 2000);

		opcode = read_flash_ctrl_reg(FLASH_NF_DATA);
		onfi_param[i] = (unsigned char)((opcode >> ((i << 3) % 32)) & 0xff);
	}

	if (onfi_crc16(ONFI_CRC_BASE, (uint8_t *) p, 254) != le16_to_cpu(p->crc))
		return 0;

	ecc_reset.wrd = 0;
	ecc_reset.bf.eccClear = ECC_CLR;
	ecc_reset.bf.fifoClear = FIFO_CLR;
	ecc_reset.bf.nflash_reset = NF_RESET;
	write_flash_ctrl_reg(FLASH_NF_ECC_RESET, ecc_reset.wrd);

	/* Check version */
	val = le16_to_cpu(p->revision);
	if (val & (1 << 5))
		chip->onfi_version = 23;
	else if (val & (1 << 4))
		chip->onfi_version = 22;
	else if (val & (1 << 3))
		chip->onfi_version = 21;
	else if (val & (1 << 2))
		chip->onfi_version = 20;
	else if (val & (1 << 1))
		chip->onfi_version = 10;

	if (!chip->onfi_version) {
		pr_info("unsupported ONFI version: %d\n", val);
		return 0;
	}

	sanitize_string(p->manufacturer, sizeof(p->manufacturer));
	sanitize_string(p->model, sizeof(p->model));
	if (!mtd->name)
		mtd->name = p->model;

	mtd->writesize = le32_to_cpu(p->byte_per_page);

	/*
	 * pages_per_block and blocks_per_lun may not be a power-of-2 size
	 * (don't ask me who thought of this...). MTD assumes that these
	 * dimensions will be power-of-2, so just truncate the remaining area.
	 */
	mtd->erasesize = 1 << (fls(le32_to_cpu(p->pages_per_block)) - 1);
	mtd->erasesize *= mtd->writesize;

	mtd->oobsize = le16_to_cpu(p->spare_bytes_per_page);

	/* See erasesize comment */
	chip->chipsize = 1 << (fls(le32_to_cpu(p->blocks_per_lun)) - 1);
	chip->chipsize *= (uint64_t) mtd->erasesize * p->lun_count;
	chip->bits_per_cell = p->bits_per_cell;

	if (onfi_feature(chip) & ONFI_FEATURE_16_BIT_BUS)
		*busw = NAND_BUSWIDTH_16;
	else
		*busw = 0;

	if (p->ecc_bits != 0xff) {
		chip->ecc_strength_ds = p->ecc_bits;
		chip->ecc_step_ds = 512;
	} else if (chip->onfi_version >= 21 && (onfi_feature(chip) & ONFI_FEATURE_EXT_PARAM_PAGE)) {

		/*
		 * The nand_flash_detect_ext_param_page() uses the
		 * Change Read Column command which maybe not supported
		 * by the chip->cmdfunc. So try to update the chip->cmdfunc
		 * now. We do not replace user supplied command function.
		 */
		if (mtd->writesize > 512 && chip->cmdfunc == ca_nand_command)
			chip->cmdfunc = ca_nand_command_lp;

		/* The Extended Parameter Page is supported since ONFI 2.1. */
		if (nand_flash_detect_ext_param_page(mtd, chip, p))
			pr_warn("Failed to detect ONFI extended param page\n");
	} else {
		pr_warn("Could not retrieve ONFI ECC requirements\n");
	}

	if (p->jedec_id == NAND_MFR_MICRON)
		nand_onfi_detect_micron(chip, p);

	return 1;
}

/**
 * ca_nand_fill_oob - [Internal] Transfer client buffer to oob
 * @chip:	nand chip structure
 * @oob:	oob data buffer
 * @ops:	oob ops structure
 */
uint8_t *ca_nand_fill_oob(struct nand_chip * chip, uint8_t * oob, struct mtd_oob_ops * ops)
{
	size_t len = ops->ooblen;

	switch (ops->mode) {

	case MTD_OPS_PLACE_OOB:
	case MTD_OPS_RAW:
		memcpy(chip->oob_poi + ops->ooboffs, oob, len);
		return oob + len;

	case MTD_OPS_AUTO_OOB:{
			struct nand_oobfree *free = chip->ecc.layout->oobfree;
			uint32_t boffs = 0, woffs = ops->ooboffs;
			size_t bytes = 0;

			for (; free->length && len; free++, len -= bytes) {
				/* Write request not from offset 0 ? */
				if (unlikely(woffs)) {
					if (woffs >= free->length) {
						woffs -= free->length;
						continue;
					}
					boffs = free->offset + woffs;
					bytes = min_t(size_t, len, (free->length - woffs));
					woffs = 0;
				} else {
					bytes = min_t(size_t, len, free->length);
					boffs = free->offset;
				}
				memcpy(chip->oob_poi + boffs, oob, bytes);
				oob += bytes;
			}
			return oob;
		}
		//default:
		//BUG();
	}
	return NULL;
}

/**
 * ca_nand_transfer_oob - [Internal] Transfer oob to client buffer
 * @chip:	nand chip structure
 * @oob:	oob destination address
 * @ops:	oob ops structure
 * @len:	size of oob to transfer
 */
uint8_t *ca_nand_transfer_oob(struct nand_chip * chip, uint8_t * oob, struct mtd_oob_ops * ops, size_t len)
{
	switch (ops->mode) {

	case MTD_OPS_PLACE_OOB:
	case MTD_OPS_RAW:
		memcpy(oob, chip->oob_poi + ops->ooboffs, len);
		return oob + len;

	case MTD_OPS_AUTO_OOB:{
			struct nand_oobfree *free = chip->ecc.layout->oobfree;
			uint32_t boffs = 0, roffs = ops->ooboffs;
			size_t bytes = 0;

			for (; free->length && len; free++, len -= bytes) {
				/* Read request not from offset 0 ? */
				if (unlikely(roffs)) {
					if (roffs >= free->length) {
						roffs -= free->length;
						continue;
					}
					boffs = free->offset + roffs;
					bytes = min_t(size_t, len, (free->length - roffs));
					roffs = 0;
				} else {
					bytes = min_t(size_t, len, free->length);
					boffs = free->offset;
				}
				memcpy(oob, chip->oob_poi + boffs, bytes);
				oob += bytes;
			}
			return oob;
		}
	}
	return NULL;
}

/**
 * ca_nand_block_isbad - [MTD Interface] Check if block at offset is bad
 * @mtd:	MTD device structure
 * @offs:	offset relative to mtd start
 */
int ca_nand_block_isbad(struct mtd_info *mtd, loff_t offs)
{
	int rc;
	/* Check for invalid offset */
	if (offs > mtd->size)
		return -EINVAL;

#ifdef CONFIG_NAND_HIDE_BAD
    loff_t off_in_out = offs;
    int ret = xlat_offset_to_skip_factory_bad(mtd, &off_in_out);
    if(ret == -EINVAL){
        nand_error("%s: offset xlat exceeds chip size\n",__FUNCTION__);
        return ret;
    }
    if(offs != off_in_out){
        nand_debug("read xlat: 0x%x => 0x%x\n",offs, off_in_out);
    }
    offs = off_in_out;
#endif
	rc = ca_nand_block_checkbad(mtd, offs, 1, 0);

	return rc;
}

/**
 * ca_nand_block_markbad - [MTD Interface] Mark block at the given offset as bad
 * @mtd:	MTD device structure
 * @ofs:	offset relative to mtd start
 */
int ca_nand_block_markbad(struct mtd_info *mtd, loff_t ofs)
{
	struct nand_chip *chip = mtd->priv;
	int ret;

	if ((ret = ca_nand_block_isbad(mtd, ofs))) {
		/* If it was bad already, return success and do nothing. */
		if (ret > 0)
			return 0;
		return ret;
	}

#ifdef CONFIG_NAND_HIDE_BAD
    loff_t off_in_out = ofs;
    ret = xlat_offset_to_skip_factory_bad(mtd, &off_in_out);
    if(ret == -EINVAL){
        nand_error("%s: offset xlat exceeds chip size\n",__FUNCTION__);
        return ret;
    }
    if(ofs != off_in_out){
        nand_debug("read xlat: 0x%x => 0x%x\n",ofs,off_in_out);
    }
    ofs = off_in_out;
#endif
	ret = chip->block_markbad(mtd, ofs);

	return ret;
}

/**
 * ca_nand_sync - [MTD Interface] sync
 * @mtd:	MTD device structure
 *
 * Sync is actually a wait for chip ready function
 */
void ca_nand_sync(struct mtd_info *mtd)
{
	struct nand_chip *chip = mtd->priv;

#ifdef DEBUG
	printf("%s: called\n", __func__);
#endif

	/* Grab the lock and see if the device is available */
	ca_nand_get_device(chip, mtd, FL_SYNCING);
	/* Release it and go back */
	ca_nand_release_device(mtd);
}

/**
 * ca_nand_check_wp - [GENERIC] check if the chip is write protected
 * @mtd:	MTD device structure
 * Check, if the device is write protected
 *
 * The function expects, that the device is already selected
 */
int ca_nand_check_wp(struct mtd_info *mtd)
{
//      struct nand_chip *this = mtd->priv;
	/* Check the WP bit */
	int ready;

	check_flash_ctrl_status();

	write_flash_ctrl_reg(FLASH_NF_ECC_CONTROL, 0x0);	//disable ecc gen

	nf_cnt.wrd = 0;
	nf_cnt.bf.nflashRegOobCount = NCNT_EMPTY_OOB;
	nf_cnt.bf.nflashRegDataCount = NCNT_DATA_1;
	nf_cnt.bf.nflashRegAddrCount = NCNT_EMPTY_ADDR;
	nf_cnt.bf.nflashRegCmdCount = NCNT_CMD_1;
	write_flash_ctrl_reg(FLASH_NF_COUNT, nf_cnt.wrd);

	nf_cmd.wrd = 0;
	nf_cmd.bf.nflashRegCmd0 = NAND_CMD_STATUS;
	write_flash_ctrl_reg(FLASH_NF_COMMAND, nf_cmd.wrd);

	nf_access.wrd = 0;
	nf_access.bf.nflashCeAlt = CHIP_EN;
	//nf_access.bf.nflashDirWr = ;
	nf_access.bf.nflashRegWidth = NFLASH_WiDTH8;
	write_flash_ctrl_reg(FLASH_NF_ACCESS, nf_access.wrd);

	flash_start.wrd = 0;
	flash_start.bf.nflashRegReq = FLASH_GO;
	flash_start.bf.nflashRegCmd = FLASH_RD;
	write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, flash_start.wrd);

	flash_start.wrd = read_flash_ctrl_reg(FLASH_FLASH_ACCESS_START);
	while (flash_start.bf.nflashRegReq) {
		flash_start.wrd = read_flash_ctrl_reg(FLASH_FLASH_ACCESS_START);
		udelay(1);

	}

	ready = read_flash_ctrl_reg(FLASH_NF_DATA) & 0xff;
	if (ready == 0xff)
		printf("ca_nand_check_wp flash status : %x\n", read_flash_ctrl_reg(FLASH_STATUS));

	return (ready & NAND_STATUS_WP) ? 0 : 1;
}

/**
 * ca_nand_write_oob - [MTD Interface] NAND write data and/or out-of-band
 * @mtd:	MTD device structure
 * @to:		offset to write to
 * @ops:	oob operation description structure
 */
int ca_nand_write_oob(struct mtd_info *mtd, loff_t to, struct mtd_oob_ops *ops)
{
	struct nand_chip *chip = mtd->priv;
	int ret = -ENOTSUPP;

	ops->retlen = 0;

	/* Do not allow writes past end of device */
	if (ops->datbuf && (to + ops->len) > mtd->size) {
		printf("%s: Attempt write beyond " "end of device\n", __func__);
		return -EINVAL;
	}

	ca_nand_get_device(chip, mtd, FL_WRITING);

	switch (ops->mode) {
	case MTD_OPS_PLACE_OOB:
	case MTD_OPS_AUTO_OOB:
	case MTD_OPS_RAW:
		break;

	default:
		goto out;
	}

#ifdef CONFIG_NAND_HIDE_BAD
	acc_phy_offset = 1;
#endif

	if (!ops->datbuf)
		ret = ca_nand_do_write_oob(mtd, to, ops);
	else
		ret = ca_nand_do_write_ops(mtd, to, ops);

#ifdef CONFIG_NAND_HIDE_BAD
	acc_phy_offset = 0;
#endif

 out:
	ca_nand_release_device(mtd);

	return ret;
}

/**
 * ca_nand_do_read_oob - [Intern] NAND read out-of-band
 * @mtd:	MTD device structure
 * @from:	offset to read from
 * @ops:	oob operations description structure
 *
 * NAND read out-of-band data from the spare area
 */
int ca_nand_do_read_oob(struct mtd_info *mtd, loff_t from, struct mtd_oob_ops *ops)
{
	int page, realpage, chipnr;
	struct nand_chip *chip = mtd->priv;
	int readlen = ops->ooblen;
	int len;
	uint8_t *buf = ops->oobbuf;
	uint32_t pa_be4xlat __attribute((unused));


#ifdef DEBUG
	printf("%s: from = 0x%08Lx, len = %i\n", __func__, (unsigned long long)from, readlen);
#endif

	if (ops->mode == MTD_OPS_AUTO_OOB)
		len = chip->ecc.layout->oobavail;
	else
		len = mtd->oobsize;

	if (unlikely(ops->ooboffs >= len)) {
		printf("%s: Attempt to start read " "outside oob\n", __func__);
		return -EINVAL;
	}

	/* Do not allow reads past end of device */
	if (unlikely(from >= mtd->size ||
		     ops->ooboffs + readlen > ((mtd->size >> chip->page_shift) - (from >> chip->page_shift)) * len)) {
		printf("%s: Attempt read beyond end " "of device\n", __func__);
		return -EINVAL;
	}

	chipnr = (int)(from >> chip->chip_shift);
	chip->select_chip(mtd, chipnr);

	/* Shift to get page */
	realpage = (int)(from >> chip->page_shift);
	page = realpage & chip->pagemask;
	pa_be4xlat = page;

	while (1) {
#ifdef CONFIG_NAND_HIDE_BAD
        loff_t off_in_out = pa_be4xlat<<chip->page_shift;
        int ret = xlat_offset_to_skip_factory_bad(mtd, &off_in_out);
        if(ret == -EINVAL){
            nand_error("%s: offset xlat exceeds chip size\n",__FUNCTION__);
            return ret;
        }
        if((pa_be4xlat<<chip->page_shift) != off_in_out){
            nand_debug("read xlat: 0x%x => 0x%x\n",(pa_be4xlat<<chip->page_shift),off_in_out);
        }
        page = off_in_out>>chip->page_shift;
        pa_be4xlat++;
#endif
		chip->ecc.read_oob(mtd, chip, page);

		len = min(len, readlen);
		buf = ca_nand_transfer_oob(chip, buf, ops, len);

		if (!(chip->options & NAND_NEED_READRDY)) {
			/*
			 * Apply delay or wait for ready/busy pin. Do this
			 * before the AUTOINCR check, so no problems arise if a
			 * chip which does auto increment is marked as
			 * NOAUTOINCR by the board driver.
			 */
			if (!chip->dev_ready)
				udelay(chip->chip_delay);
			else
				ca_nand_wait_ready(mtd);
		}

		readlen -= len;
		if (!readlen)
			break;

		/* Increment page address */
		realpage++;

		page = realpage & chip->pagemask;
		/* Check, if we cross a chip boundary */
		if (!page) {
			chipnr++;
			chip->select_chip(mtd, -1);
			chip->select_chip(mtd, chipnr);
		}
	}

	ops->oobretlen = ops->ooblen;
	return 0;
}

/**
 * ca_nand_read_oob - [MTD Interface] NAND read data and/or out-of-band
 * @mtd:	MTD device structure
 * @from:	offset to read from
 * @ops:	oob operation description structure
 *
 * NAND read data and/or out-of-band data
 */
int ca_nand_read_oob(struct mtd_info *mtd, loff_t from, struct mtd_oob_ops *ops)
{
	struct nand_chip *chip = mtd->priv;
	int ret = -ENOTSUPP;

	ops->retlen = 0;

	/* Do not allow reads past end of device */
	if (ops->datbuf && (from + ops->len) > mtd->size) {
		printf("%s: Attempt read " "beyond end of device\n", __func__);
		return -EINVAL;
	}

	ca_nand_get_device(chip, mtd, FL_READING);

	switch (ops->mode) {
	case MTD_OPS_PLACE_OOB:
	case MTD_OPS_AUTO_OOB:
	case MTD_OPS_RAW:
		break;

	default:
		goto out;
	}

#ifdef CONFIG_NAND_HIDE_BAD
	acc_phy_offset = 1;
#endif

	if (!ops->datbuf)
		ret = ca_nand_do_read_oob(mtd, from, ops);
	else
		ret = ca_nand_do_read_ops(mtd, from, ops);

#ifdef CONFIG_NAND_HIDE_BAD
	acc_phy_offset = 0;
#endif

 out:
	ca_nand_release_device(mtd);

	return ret;
}

/**
 * ca_nand_write - [MTD Interface] NAND write with ECC
 * @mtd:	MTD device structure
 * @to:		offset to write to
 * @len:	number of bytes to write
 * @retlen:	pointer to variable to store the number of written bytes
 * @buf:	the data to write
 *
 * NAND write with ECC
 */
int ca_nand_write(struct mtd_info *mtd, loff_t to, size_t len, size_t * retlen, const uint8_t * buf)
{
	struct nand_chip *chip = mtd->priv;
	struct mtd_oob_ops ops;
	int ret;

	/* Do not allow reads past end of device */
	if ((to + len) > mtd->size)
		return -EINVAL;
	if (!len)
		return 0;

	ca_nand_get_device(chip, mtd, FL_WRITING);

	ops.len = len;
	ops.datbuf = (uint8_t *) buf;
	ops.oobbuf = NULL;
	ops.mode = MTD_OPS_PLACE_OOB;

	ret = ca_nand_do_write_ops(mtd, to, &ops);

	*retlen = ops.retlen;

	ca_nand_release_device(mtd);

	return ret;
}

/**
 * ca_nand_do_read_ops - [Internal] Read data with ECC
 *
 * @mtd:	MTD device structure
 * @from:	offset to read from
 * @ops:	oob ops structure
 *
 * Internal function. Called with chip held.
 */
int ca_nand_do_read_ops(struct mtd_info *mtd, loff_t from, struct mtd_oob_ops *ops)
{
	int chipnr, page, realpage, col, bytes, aligned;
	struct nand_chip *chip = mtd->priv;
	struct mtd_ecc_stats stats;
	int sndcmd = 1;
	int ret = 0;
	uint32_t readlen = ops->len;
	uint32_t oobreadlen = ops->ooblen;
	uint8_t *bufpoi, *oob, *buf;
	uint32_t pa_be4xlat __attribute((unused));

	memcpy(&stats, &mtd->ecc_stats, sizeof(struct mtd_ecc_stats));

	chipnr = (int)(from >> chip->chip_shift);
	chip->select_chip(mtd, chipnr);

	realpage = (int)(from >> chip->page_shift);
	page = realpage & chip->pagemask;
	pa_be4xlat = page;

	col = (int)(from & (mtd->writesize - 1));

	buf = ops->datbuf;
	oob = ops->oobbuf;

	while (1) {
		bytes = min(mtd->writesize - col, readlen);
		aligned = (bytes == mtd->writesize);

#ifdef CONFIG_NAND_HIDE_BAD
        loff_t off_in_out = pa_be4xlat<<chip->page_shift;
        int ret = xlat_offset_to_skip_factory_bad(mtd, &off_in_out);
        if(ret == -EINVAL){
            nand_error("%s: offset xlat exceeds chip size\n",__FUNCTION__);
            return ret;
        }
        if((pa_be4xlat<<chip->page_shift) != off_in_out){
            nand_debug("read xlat: 0x%x => 0x%x\n",(pa_be4xlat<<chip->page_shift),off_in_out);
        }
        page = off_in_out>>chip->page_shift;
        pa_be4xlat++;
#endif

		/* Is the current page in the buffer ? */
		if (realpage != chip->pagebuf || oob) {
			bufpoi = aligned ? buf : chip->buffers->databuf;

			if (likely(sndcmd)) {
				chip->cmdfunc(mtd, NAND_CMD_READ0, 0x00, page);
				sndcmd = 0;
			}

			/* Now read the page into the buffer */
			if (unlikely(ops->mode == MTD_OPS_RAW))
				ret = chip->ecc.read_page_raw(mtd, chip, bufpoi, 0, page);
			else if (!aligned && NAND_HAS_SUBPAGE_READ(chip)
				 && !oob)
				ret = chip->ecc.read_subpage(mtd, chip, col, bytes, bufpoi, page);
			else
				ret = chip->ecc.read_page(mtd, chip, bufpoi, 1, page);
			if (ret < 0)
				break;

			/* Transfer not aligned data */
			if (!aligned) {
				if (!NAND_HAS_SUBPAGE_READ(chip) && !oob)
					chip->pagebuf = realpage;
				memcpy(buf, chip->buffers->databuf + col, bytes);
			}

			buf += bytes;

			if (unlikely(oob)) {
				/* Raw mode does data:oob:data:oob */
				if (ops->mode != MTD_OPS_RAW) {
					int toread = min(oobreadlen,
							 chip->ecc.layout->oobavail);
					if (toread) {
						oob = ca_nand_transfer_oob(chip, oob, ops, toread);
						oobreadlen -= toread;
					}
				} else
					buf = ca_nand_transfer_oob(chip, oob, ops, mtd->oobsize);
			}

			if (!(chip->options & NAND_NEED_READRDY)) {
				/*
				 * Apply delay or wait for ready/busy pin. Do
				 * this before the AUTOINCR check, so no
				 * problems arise if a chip which does auto
				 * increment is marked as NOAUTOINCR by the
				 * board driver.
				 */
				if (!chip->dev_ready)
					udelay(chip->chip_delay);
				else
					ca_nand_wait_ready(mtd);
			}
		} else {
			memcpy(buf, chip->buffers->databuf + col, bytes);
			buf += bytes;
		}

		readlen -= bytes;

		if (!readlen)
			break;

		/* For subsequent reads align to page boundary. */
		col = 0;
		/* Increment page address */
		realpage++;

		page = realpage & chip->pagemask;
		/* Check, if we cross a chip boundary */
		if (!page) {
			chipnr++;
			chip->select_chip(mtd, -1);
			chip->select_chip(mtd, chipnr);
		}
	}

	ops->retlen = ops->len - (size_t) readlen;
	if (oob)
		ops->oobretlen = ops->ooblen - oobreadlen;

	if (ret)
		return ret;

	if (mtd->ecc_stats.failed - stats.failed)
		return -EBADMSG;

	return mtd->ecc_stats.corrected - stats.corrected ? -EUCLEAN : 0;
}

/**
 * ca_nand_read - [MTD Interface] MTD compability function for nand_do_read_ecc
 * @mtd:	MTD device structure
 * @from:	offset to read from
 * @len:	number of bytes to read
 * @retlen:	pointer to variable to store the number of read bytes
 * @buf:	the databuffer to put data
 *
 * Get hold of the chip and call nand_do_read
 */

int ca_nand_read(struct mtd_info *mtd, loff_t from, size_t len, size_t * retlen, uint8_t * buf)
{
	struct nand_chip *chip = mtd->priv;
	struct mtd_oob_ops ops;
	int ret;

	/* Do not allow reads past end of device */
	if ((from + len) > mtd->size)
		return -EINVAL;
	if (!len)
		return 0;

	ca_nand_get_device(chip, mtd, FL_READING);

	ops.len = len;
	ops.datbuf = buf;
	ops.oobbuf = NULL;
	ops.mode = MTD_OPS_PLACE_OOB;

	ret = ca_nand_do_read_ops(mtd, from, &ops);

	*retlen = ops.retlen;

	ca_nand_release_device(mtd);

	return ret;
}

/**
 * ca_nand_erase_block - [GENERIC] erase a block
 * @mtd:	MTD device structure
 * @page:	page address
 *
 * Erase a block.
 */

int ca_nand_erase_block(struct mtd_info *mtd, int page)
{
	struct nand_chip *this = mtd->priv;
	u64 test;
	FLASH_FLASH_ACCESS_START_t tmp_access;

	check_flash_ctrl_status();

	/* Send commands to erase a page */
	write_flash_ctrl_reg(FLASH_NF_ECC_CONTROL, 0);	//

	nf_cnt.wrd = 0;
	nf_cmd.wrd = 0;
	nf_addr1.wrd = 0;
	nf_cnt.bf.nflashRegOobCount = NCNT_EMPTY_OOB;
	nf_cnt.bf.nflashRegDataCount = NCNT_EMPTY_DATA;
	nf_cnt.bf.nflashRegCmdCount = NCNT_CMD_2;

	test = 0x10000 * mtd->writesize;
	if (this->chipsize > test) {
		nf_cnt.bf.nflashRegAddrCount = NCNT_ADDR_3;
	} else {
		nf_cnt.bf.nflashRegAddrCount = NCNT_ADDR_2;
	}

	nf_cmd.bf.nflashRegCmd0 = NAND_CMD_ERASE1;
	nf_cmd.bf.nflashRegCmd1 = NAND_CMD_ERASE2;
	nf_addr1.wrd = page;
	nf_addr2.wrd = 0;
	write_flash_ctrl_reg(FLASH_NF_COUNT, nf_cnt.wrd);
	write_flash_ctrl_reg(FLASH_NF_COMMAND, nf_cmd.wrd);
	write_flash_ctrl_reg(FLASH_NF_ADDRESS_1, nf_addr1.wrd);
	write_flash_ctrl_reg(FLASH_NF_ADDRESS_2, nf_addr2.wrd);

	nf_access.wrd = 0;
	nf_access.bf.nflashCeAlt = CHIP_EN;
	nf_access.bf.nflashRegWidth = NFLASH_WiDTH8;

	write_flash_ctrl_reg(FLASH_NF_ACCESS, nf_access.wrd);
	flash_start.wrd = 0;
	flash_start.bf.nflashRegReq = FLASH_GO;
	flash_start.bf.nflashRegCmd = FLASH_RD;	//no data access use read..
	write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, flash_start.wrd);
	flash_start.wrd = read_flash_ctrl_reg(FLASH_FLASH_ACCESS_START);
	tmp_access.wrd = 0;
	tmp_access.bf.nflashRegReq = 1;
	reg_wait(FLASH_FLASH_ACCESS_START, tmp_access.wrd, 0, 1000);

	return 0;
}

/**
 * ca_nand_block_checkbad - [GENERIC] Check if a block is marked bad
 * @mtd:	MTD device structure
 * @ofs:	offset from device start
 * @getchip:	0, if the chip is already selected
 * @allowbbt:	1, if its allowed to access the bbt area
 *
 * Check, if the block is bad. Either by reading the bad block table or
 * calling of the scan function.
 */
int ca_nand_block_checkbad(struct mtd_info *mtd, loff_t ofs, int getchip, int allowbbt)
{
	struct nand_chip *chip = mtd->priv;

	if (!chip->bbt)
		return chip->block_bad(mtd, ofs);

	/* Return info from the table */
	return nand_isbad_bbt(mtd, ofs, allowbbt);
}

/**
 * ca_nand_erase - [MTD Interface] erase block(s)
 * @mtd:	MTD device structure
 * @instr:	erase instruction
 *
 * Erase one ore more blocks
 */
int ca_nand_erase(struct mtd_info *mtd, struct erase_info *instr)
{
	int rc;

	rc = ca_nand_erase_nand(mtd, instr, 0);

	return rc;

}

/**
 * ca_nand_erase_nand - [Internal] erase block(s)
 * @mtd:	MTD device structure
 * @instr:	erase instruction
 * @allowbbt:	allow erasing the bbt area
 *
 * Erase one ore more blocks
 */
int ca_nand_erase_nand(struct mtd_info *mtd, struct erase_info *instr, int allowbbt)
{
	int page, status, pages_per_block, ret, chipnr;
	struct nand_chip *chip = mtd->priv;
	unsigned int bbt_masked_page = 0xffffffff;
	loff_t len;
	uint32_t pa_be4xlat __attribute((unused));

#ifdef DEBUG
	printf("%s: start = 0x%012llx, len = %llu\n",
	       __func__, (unsigned long long)instr->addr, (unsigned long long)instr->len);
#endif

	/* Start address must align on block boundary */
	if (instr->addr & ((1 << chip->phys_erase_shift) - 1)) {
		printf("%s: Unaligned address\n", __func__);
		printf(KERN_ALERT "unaligned_chipptr!!!");
		return -EINVAL;
	}

	/* Length must align on block boundary */
	if (instr->len & ((1 << chip->phys_erase_shift) - 1)) {
		printf("%s: Length not block aligned\n", __func__);
		return -EINVAL;
	}

	/* Do not allow erase past end of device */
	if ((instr->len + instr->addr) > mtd->size) {
		printf("%s: Erase past end of device\n", __func__);
		return -EINVAL;
	}

	instr->fail_addr = MTD_FAIL_ADDR_UNKNOWN;

	/* Grab the lock and see if the device is available */
	ca_nand_get_device(chip, mtd, FL_ERASING);

	/* Shift to get first page */
	page = (int)(instr->addr >> chip->page_shift);
	chipnr = (int)(instr->addr >> chip->chip_shift);
	pa_be4xlat = page;

	/* Calculate pages in each block */
	pages_per_block = 1 << (chip->phys_erase_shift - chip->page_shift);

	/* Select the NAND device */
	chip->select_chip(mtd, chipnr);

	/* Check, if it is write protected */
	if (ca_nand_check_wp(mtd)) {
		printf("%s: Device is write protected!!!\n", __func__);
		instr->state = MTD_ERASE_FAILED;
		goto erase_exit;
	}

	/*
	 * If BBT requires refresh, set the BBT page mask to see if the BBT
	 * should be rewritten. Otherwise the mask is set to 0xffffffff which
	 * can not be matched. This is also done when the bbt is actually
	 * erased to avoid recusrsive updates
	 */
	//Jason if (chip->options & BBT_AUTO_REFRESH && !allowbbt)
	//      bbt_masked_page = chip->bbt_td->pages[chipnr] & BBT_PAGE_MASK;

	/* Loop through the pages */
	len = instr->len;

	instr->state = MTD_ERASING;

	while (len) {
#ifdef CONFIG_NAND_HIDE_BAD
        if(!instr->scrub){
            loff_t off_in_out = pa_be4xlat<<chip->page_shift;

            ret = xlat_offset_to_skip_factory_bad(mtd, &off_in_out);
            if(ret == -EINVAL){
                nand_error("%s: offset xlat exceeds chip size\n",__FUNCTION__);
                return ret;
            }
            if((pa_be4xlat<<chip->page_shift) != off_in_out){
                nand_debug("read xlat: 0x%x => 0x%x\n",(pa_be4xlat<<chip->page_shift),off_in_out);
            }
            page = off_in_out>>chip->page_shift;
            pa_be4xlat += pages_per_block;
        }
#endif

		/*
		 * Invalidate the page cache, if we erase the block which
		 * contains the current cached page
		 */
		if (page <= chip->pagebuf && chip->pagebuf < (page + pages_per_block))
			chip->pagebuf = -1;

		//chip->erase_cmd(mtd, page & chip->pagemask);
		ca_nand_erase_block(mtd, page);

		status = chip->waitfunc(mtd, chip);

		/* See if block erase succeeded */
		if (status & NAND_STATUS_FAIL) {
			printf("%s: Failed erase, " "page 0x%08x\n", __func__, page);
			instr->state = MTD_ERASE_FAILED;
			instr->fail_addr = ((loff_t) page << chip->page_shift);
			goto erase_exit;
		}
#ifdef CONFIG_NAND_HIDE_BAD
		if (instr->scrub) {
			update_factory_bad_block_list(mtd, page << chip->page_shift, 0);
		}
#endif

		/* Increment page address and decrement length */
		len -= (1 << chip->phys_erase_shift);
		page += pages_per_block;

		/* Check, if we cross a chip boundary */
		if (len && !(page & chip->pagemask)) {
			chipnr++;
			chip->select_chip(mtd, -1);
			chip->select_chip(mtd, chipnr);

			/*
			 * If BBT requires refresh and BBT-PERCHIP, set the BBT
			 * page mask to see if this BBT should be rewritten
			 */
			if (bbt_masked_page != 0xffffffff && (chip->bbt_td->options & NAND_BBT_PERCHIP))
				bbt_masked_page = chip->bbt_td->pages[chipnr] & BBT_PAGE_MASK;
		}
	}
	instr->state = MTD_ERASE_DONE;

 erase_exit:

	ret = instr->state == MTD_ERASE_DONE ? 0 : -EIO;

	/* Deselect and wake up anyone waiting on the device */
	ca_nand_release_device(mtd);

	/* Do call back function */
	if (!ret)
		mtd_erase_callback(instr);

	/*
	 * If BBT requires refresh and erase was successful, rewrite any
	 * selected bad block tables
	 */
	if (bbt_masked_page == 0xffffffff || ret)
		return ret;

	/* Return more or less happy */
	return ret;
}

/**
 * ca_nand_write_oob_std - [REPLACABLE] the most common OOB data write function
 * @mtd:	mtd info structure
 * @chip:	nand chip info structure
 * @page:	page number to write
 */
int ca_nand_write_oob_std(struct mtd_info *mtd, struct nand_chip *chip, int page)
{
	int status = 0, i;
	FLASH_FLASH_ACCESS_START_t tmp_access;
	//const uint8_t *buf = chip->oob_poi;
	//int length = mtd->oobsize;

	check_flash_ctrl_status();

	chip->cmdfunc(mtd, NAND_CMD_SEQIN, mtd->writesize, page);
	//chip->write_buf(mtd, buf, length);
	/* Send command to program the OOB data */
	chip->cmdfunc(mtd, NAND_CMD_PAGEPROG, -1, -1);

	write_flash_ctrl_reg(FLASH_NF_ECC_CONTROL, 0x0);	//disable ecc gen

	nf_cmd.wrd = 0;
	nf_addr1.wrd = 0;
	nf_cnt.wrd = 0;
	nf_cnt.bf.nflashRegOobCount = mtd->oobsize - 1;
	nf_cnt.bf.nflashRegDataCount = NCNT_EMPTY_DATA;

	nf_addr2.wrd = 0;

	if (chip->chipsize < SZ_32M) {
		nf_cnt.bf.nflashRegAddrCount = NCNT_ADDR_3;

		if (mtd->writesize > NCNT_512P_DATA) {
			nf_cnt.bf.nflashRegCmdCount = NCNT_CMD_2;
			nf_cmd.bf.nflashRegCmd0 = NAND_CMD_SEQIN;
			nf_cmd.bf.nflashRegCmd1 = NAND_CMD_PAGEPROG;
		} else {
			nf_cnt.bf.nflashRegCmdCount = NCNT_CMD_3;
			nf_cmd.bf.nflashRegCmd0 = NAND_CMD_READOOB;	//??
			nf_cmd.bf.nflashRegCmd1 = NAND_CMD_SEQIN;
			nf_cmd.bf.nflashRegCmd2 = NAND_CMD_PAGEPROG;
		}
		//read oob need to add page data size to match correct oob ddress
		nf_addr1.wrd = (((page & 0x00ffffff) << 8));
		nf_addr2.wrd = ((page & 0xff000000) >> 24);
	} else if (chip->chipsize <= SZ_128M) {
		nf_cnt.bf.nflashRegAddrCount = NCNT_ADDR_4;
		nf_cnt.bf.nflashRegCmdCount = NCNT_CMD_2;
		nf_cmd.bf.nflashRegCmd0 = NAND_CMD_SEQIN;
		nf_cmd.bf.nflashRegCmd1 = NAND_CMD_PAGEPROG;
		nf_addr1.wrd = (((page & 0xffff) << 16) + (mtd->writesize & 0xffff));
		nf_addr2.wrd = ((page & 0xffff0000) >> 16);

	} else			//if((chip->chipsize > (128 << 20)) ))
	{

		nf_cnt.bf.nflashRegAddrCount = NCNT_ADDR_5;
		nf_cnt.bf.nflashRegCmdCount = NCNT_CMD_2;
		nf_cmd.bf.nflashRegCmd0 = NAND_CMD_SEQIN;
		nf_cmd.bf.nflashRegCmd1 = NAND_CMD_PAGEPROG;
		nf_addr1.wrd = (((page & 0xffff) << 16) + (mtd->writesize & 0xffff));
		nf_addr2.wrd = ((page & 0xffff0000) >> 16);

	}

	write_flash_ctrl_reg(FLASH_NF_COUNT, nf_cnt.wrd);
	write_flash_ctrl_reg(FLASH_NF_COMMAND, nf_cmd.wrd);	//write read id command
	write_flash_ctrl_reg(FLASH_NF_ADDRESS_1, nf_addr1.wrd);	//write address 0x0
	write_flash_ctrl_reg(FLASH_NF_ADDRESS_2, nf_addr2.wrd);

	pwrite = (unsigned int *)chip->oob_poi;

	for (i = 0; i < ((mtd->oobsize / 4)); i++) {
		nf_access.wrd = 0;
		nf_access.bf.nflashCeAlt = CHIP_EN;
		//nf_access.bf.nflashDirWr = ;
		nf_access.bf.nflashRegWidth = NFLASH_WiDTH32;
		write_flash_ctrl_reg(FLASH_NF_ACCESS, nf_access.wrd);

		write_flash_ctrl_reg(FLASH_NF_DATA, pwrite[i]);

		flash_start.wrd = 0;
		flash_start.bf.nflashRegReq = FLASH_GO;
		flash_start.bf.nflashRegCmd = FLASH_WT;
		//flash_start.bf.nflash_random_access = RND_ENABLE;
		write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, flash_start.wrd);

		tmp_access.wrd = 0;
		tmp_access.bf.nflashRegReq = 1;
		reg_wait(FLASH_FLASH_ACCESS_START, tmp_access.wrd, 0, 1000);

	}
#if 0
	unsigned int rtmp[80];
	nf_cmd.bf.nflashRegCmd0 = NAND_CMD_READ0;
	nf_cmd.bf.nflashRegCmd1 = NAND_CMD_READSTART;
	write_flash_ctrl_reg(FLASH_NF_COMMAND, nf_cmd.wrd);	//write read id command

	for (i = 0; i < ((mtd->oobsize / 4)); i++) {
		nf_access.wrd = 0;
		nf_access.bf.nflashCeAlt = CHIP_EN;
		//nf_access.bf.nflashDirWr = ;
		nf_access.bf.nflashRegWidth = NFLASH_WiDTH32;
		write_flash_ctrl_reg(FLASH_NF_ACCESS, nf_access.wrd);

		flash_start.wrd = 0;
		flash_start.bf.nflashRegReq = FLASH_GO;
		flash_start.bf.nflashRegCmd = FLASH_RD;
		//flash_start.bf.nflash_random_access = RND_ENABLE;
		write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, flash_start.wrd);

		flash_start.wrd = read_flash_ctrl_reg(FLASH_FLASH_ACCESS_START);
		while (flash_start.bf.nflashRegReq) {
			flash_start.wrd = read_flash_ctrl_reg(FLASH_FLASH_ACCESS_START);
			udelay(1);

		}
		rtmp[i] = read_flash_ctrl_reg(FLASH_NF_DATA);
	a}
	if (memcmp((unsigned char *)pwrite, (unsigned char *)rtmp, mtd->oobsize))
		printf("W->R oob error\n");
#endif
	check_flash_ctrl_status();

	status = chip->waitfunc(mtd, chip);

	return status & NAND_STATUS_FAIL ? -EIO : 0;
}

/**
 * ca_nand_read_oob_std - [REPLACABLE] the most common OOB data read function
 * @mtd:	mtd info structure
 * @chip:	nand chip info structure
 * @page:	page number to read
 * @sndcmd:	flag whether to issue read command or not
 */
int ca_nand_read_oob_std(struct mtd_info *mtd, struct nand_chip *chip, int page)
{
	int i;
	FLASH_FLASH_ACCESS_START_t tmp_access;

	check_flash_ctrl_status();

	write_flash_ctrl_reg(FLASH_NF_ECC_CONTROL, 0x0);	//disable ecc gen

	nf_cmd.wrd = 0;
	nf_addr1.wrd = 0;
	nf_addr2.wrd = 0;
	nf_cnt.wrd = 0;
	nf_cnt.bf.nflashRegOobCount = mtd->oobsize - 1;
	nf_cnt.bf.nflashRegDataCount = NCNT_EMPTY_DATA;

	if (chip->chipsize < (32 << 20)) {
		nf_cnt.bf.nflashRegAddrCount = NCNT_ADDR_3;
		nf_cnt.bf.nflashRegCmdCount = NCNT_CMD_1;
		if (mtd->writesize > NCNT_512P_DATA)
			nf_cmd.bf.nflashRegCmd0 = NAND_CMD_READ0;
		else
			nf_cmd.bf.nflashRegCmd0 = NAND_CMD_READOOB;

		nf_addr1.wrd = ((page & 0x00ffffff) << 8);
		nf_addr2.wrd = ((page & 0xff000000) >> 24);
	} else if ((chip->chipsize >= (32 << 20))
		   && (chip->chipsize <= (128 << 20))) {
		nf_cnt.bf.nflashRegAddrCount = NCNT_ADDR_4;
		nf_cnt.bf.nflashRegCmdCount = NCNT_CMD_1;
		nf_cmd.bf.nflashRegCmd0 = NAND_CMD_READ0;

		// Jeneng
		if (mtd->writesize > NCNT_512P_DATA) {
			nf_cnt.bf.nflashRegCmdCount = NCNT_CMD_2;
			nf_cmd.bf.nflashRegCmd1 = NAND_CMD_READSTART;
		}
		nf_addr1.wrd = (((page & 0xffff) << 16) + (mtd->writesize & 0xffff));
		nf_addr2.wrd = ((page & 0xffff0000) >> 16);

	} else			//if((chip->chipsize > (128 << 20)) ))
	{

		nf_cnt.bf.nflashRegAddrCount = NCNT_ADDR_5;
		nf_cnt.bf.nflashRegCmdCount = NCNT_CMD_2;
		nf_cmd.bf.nflashRegCmd0 = NAND_CMD_READ0;
		nf_cmd.bf.nflashRegCmd1 = NAND_CMD_READSTART;
		nf_addr1.wrd = (((page & 0xffff) << 16) + (mtd->writesize & 0xffff));
		nf_addr2.wrd = ((page & 0xffff0000) >> 16);
	}

	write_flash_ctrl_reg(FLASH_NF_COUNT, nf_cnt.wrd);
	write_flash_ctrl_reg(FLASH_NF_COMMAND, nf_cmd.wrd);	//write read id command
	write_flash_ctrl_reg(FLASH_NF_ADDRESS_1, nf_addr1.wrd);	//write address 0x0
	write_flash_ctrl_reg(FLASH_NF_ADDRESS_2, nf_addr2.wrd);

	pread = (unsigned int *)chip->oob_poi;

	for (i = 0; i < mtd->oobsize / 4; i++) {
		nf_access.wrd = 0;
		nf_access.bf.nflashCeAlt = CHIP_EN;
		//nf_access.bf.nflashDirWr = ;
		nf_access.bf.nflashRegWidth = NFLASH_WiDTH32;
		write_flash_ctrl_reg(FLASH_NF_ACCESS, nf_access.wrd);

		flash_start.wrd = 0;
		flash_start.bf.nflashRegReq = FLASH_GO;
		flash_start.bf.nflashRegCmd = FLASH_RD;
		//flash_start.bf.nflash_random_access = RND_ENABLE;
		write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, flash_start.wrd);

		flash_start.wrd = read_flash_ctrl_reg(FLASH_FLASH_ACCESS_START);

		tmp_access.wrd = 0;
		tmp_access.bf.nflashRegReq = 1;
		reg_wait(FLASH_FLASH_ACCESS_START, tmp_access.wrd, 0, 1000);

		pread[i] = read_flash_ctrl_reg(FLASH_NF_DATA);

	}
	return 0;
}

/**
 * ca_nand_write_page_hwecc - [REPLACABLE] hardware ecc based page write function
 * @mtd:	mtd info structure
 * @chip:	nand chip info structure
 * @buf:	data buffer
 */
int ca_nand_write_page_hwecc(struct mtd_info *mtd, struct nand_chip *chip,
			     const uint8_t * buf, int oob_required, int page)
{
	int i, j, __attribute__ ((unused)) eccsize = chip->ecc.size;
	int eccbytes = chip->ecc.bytes;
	int eccsteps = chip->ecc.steps;
	/*uint8_t *ecc_calc = chip->buffers->ecccalc; */
	/*const uint8_t *p = buf; */
	uint32_t *eccpos = chip->ecc.layout->eccpos, *addr, __attribute__ ((unused)) reg_bch_gen;
	DMA_SEC_DMA_SSP_Q_RXQ_COAL_INTERRUPT_t tmp_dma_ssp_rxq5_intsts;
	DMA_SEC_DMA_SSP_Q_TXQ_COAL_INTERRUPT_t tmp_dma_ssp_txq5_intsts;
	FLASH_FLASH_ACCESS_START_t tmp_access;
#ifdef CONFIG_CORTINA_PNAND_ECC_HW_BCH
	FLASH_NF_BCH_STATUS_t tmp_bsc_sts;
#else
	FLASH_NF_ECC_STATUS_t tmp_ecc_sts;
#endif
	FLASH_FIFO_CONTROL_t flash_fifo_ctrl;

	check_flash_ctrl_status();

	//page = host->page_addr;

	ecc_reset.wrd = 3;
	ecc_reset.bf.eccClear = ECC_CLR;
	ecc_reset.bf.fifoClear = FIFO_CLR;
	write_flash_ctrl_reg(FLASH_NF_ECC_RESET, ecc_reset.wrd);

	flash_int_sts.bf.regIrq = 1;
	write_flash_ctrl_reg(FLASH_FLASH_INTERRUPT, flash_int_sts.wrd);

	ecc_reset.wrd = 0;
	ecc_reset.bf.eccClear = 1;
	write_flash_ctrl_reg(FLASH_NF_ECC_RESET, ecc_reset.wrd);

#ifdef CONFIG_CORTINA_PNAND_ECC_HW_BCH
	bch_ctrl.wrd = 0;
	bch_ctrl.bf.bchEn = BCH_ENABLE;
	bch_ctrl.bf.bchOpcode = BCH_ENCODE;
#if	(CONFIG_CORTINA_PNAND_ECC_LEVEL == 2)
	bch_ctrl.bf.bchErrCap = BCH_ERR_CAP_8;
#elif (CONFIG_CORTINA_PNAND_ECC_LEVEL == 3)
	bch_ctrl.bf.bchErrCap = BCH_ERR_CAP_16;
#elif (CONFIG_CORTINA_PNAND_ECC_LEVEL == 4)
	bch_ctrl.bf.bchErrCap = BCH_ERR_CAP_24;
#elif (CONFIG_CORTINA_PNAND_ECC_LEVEL == 5)
	bch_ctrl.bf.bchErrCap = BCH_ERR_CAP_40;
#else
	printf("BCH Level is not recognized! %s:%d\n", __func__, __LINE__);
#endif
	write_flash_ctrl_reg(FLASH_NF_BCH_CONTROL, bch_ctrl.wrd);

#else

	ecc_ctl.wrd = 0;
	if ((eccsize - 1) == NCNT_512P_DATA)
		ecc_ctl.bf.eccGenMode = ECC_GEN_512;
	else
		ecc_ctl.bf.eccGenMode = ECC_GEN_256;
	ecc_ctl.bf.eccEn = ECC_ENABLE;
	write_flash_ctrl_reg(FLASH_NF_ECC_CONTROL, ecc_ctl.wrd);
#endif

	/*disable txq5 */
	dma_txq5_ctrl.bf.txq_en = 0;
	write_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_CONTROL, dma_txq5_ctrl.wrd);
	/*clr tx/rx eof */
	dma_ssp_txq5_intsts.wrd = read_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_COAL_INTERRUPT);
	dma_ssp_txq5_intsts.bf.txq_eof = 0;
	write_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_COAL_INTERRUPT, dma_ssp_txq5_intsts.wrd);
	dma_ssp_rxq5_intsts.wrd = read_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_RXQ_COAL_INTERRUPT);
	dma_ssp_rxq5_intsts.bf.rxq_eof = 0;
	write_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_RXQ_COAL_INTERRUPT, dma_ssp_rxq5_intsts.wrd);

	nf_cnt.wrd = 0;
	nf_cmd.wrd = 0;
	nf_addr1.wrd = 0;
	nf_addr2.wrd = 0;
	nf_cnt.wrd = 0;
	nf_cnt.bf.nflashRegOobCount = mtd->oobsize - 1;
	nf_cnt.bf.nflashRegDataCount = mtd->writesize - 1;

	if (chip->chipsize < (32 << 20)) {
		nf_cnt.bf.nflashRegAddrCount = NCNT_ADDR_3;
		nf_cnt.bf.nflashRegCmdCount = NCNT_CMD_2;
		nf_cmd.bf.nflashRegCmd0 = NAND_CMD_SEQIN;
		nf_cmd.bf.nflashRegCmd1 = NAND_CMD_PAGEPROG;
		nf_addr1.wrd = (((page & 0x00ffffff) << 8));
		nf_addr2.wrd = ((page & 0xff000000) >> 24);

	} else if ((chip->chipsize >= (32 << 20))
		   && (chip->chipsize <= (128 << 20))) {
		nf_cnt.bf.nflashRegAddrCount = NCNT_ADDR_4;
		nf_cnt.bf.nflashRegCmdCount = NCNT_CMD_2;
		nf_cmd.bf.nflashRegCmd0 = NAND_CMD_SEQIN;
		nf_cmd.bf.nflashRegCmd1 = NAND_CMD_PAGEPROG;
		nf_addr1.wrd = (((page & 0xffff) << 16));
		nf_addr2.wrd = ((page & 0xffff0000) >> 16);

	} else {		/*if((chip->chipsize > (128 << 20)) )) */

		nf_cnt.bf.nflashRegAddrCount = NCNT_ADDR_5;
		nf_cnt.bf.nflashRegCmdCount = NCNT_CMD_2;
		nf_cmd.bf.nflashRegCmd0 = NAND_CMD_SEQIN;
		nf_cmd.bf.nflashRegCmd1 = NAND_CMD_PAGEPROG;
		nf_addr1.wrd = (((page & 0xffff) << 16));
		nf_addr2.wrd = ((page & 0xffff0000) >> 16);
	}

	write_flash_ctrl_reg(FLASH_NF_COUNT, nf_cnt.wrd);
	write_flash_ctrl_reg(FLASH_NF_COMMAND, nf_cmd.wrd);
	write_flash_ctrl_reg(FLASH_NF_ADDRESS_1, nf_addr1.wrd);
	write_flash_ctrl_reg(FLASH_NF_ADDRESS_2, nf_addr2.wrd);

	/*dma_map_single( NULL, (void *)buf, mtd->writesize, DMA_TO_DEVICE); */

	nf_access.wrd = 0;
	nf_access.bf.nflashCeAlt = CHIP_EN;
	nf_access.bf.nflashRegWidth = NFLASH_WiDTH8;
	/*write_flash_ctrl_reg(FLASH_NF_ACCESS, nf_access.wrd);

	   write
	   prepare dma descriptor
	   chip->buffers->databuf
	   nf_access.wrd = read_flash_ctrl_reg(FLASH_FLASH_ACCESS_START);
	 */
	nf_access.bf.nflashExtAddr = ((page << chip->page_shift) / 0x8000000);
	write_flash_ctrl_reg(FLASH_NF_ACCESS, nf_access.wrd);

	addr = (unsigned int *)(((unsigned long)page << chip->page_shift) % 0x8000000);
	addr = (unsigned int *)((unsigned long)addr + (unsigned long)(CONFIG_SYS_FLASH_BASE));

	/*page data tx desc */
	dma_txq5_wptr.wrd = read_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_WPTR);
	tx_desc[dma_txq5_wptr.bf.index].word1.bf.own = OWN_DMA;
	tx_desc[dma_txq5_wptr.bf.index].word1.bf.buf_len = mtd->writesize;
	tx_desc[dma_txq5_wptr.bf.index].buf_adr = (unsigned long)buf;

	dma_rxq5_rptr.wrd = read_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_RXQ_RPTR);
	rx_desc[dma_rxq5_rptr.bf.index].word1.bf.own = OWN_DMA;
	rx_desc[dma_rxq5_rptr.bf.index].word1.bf.buf_len = mtd->writesize;
	rx_desc[dma_rxq5_rptr.bf.index].buf_adr = (unsigned long)addr;

	/*oob rx desc */
	addr = (unsigned int *)((unsigned long)addr + mtd->writesize);
	/*printf("  oob : addr(%p)  chip->oob_poi(%p) \n",addr, chip->oob_poi); */

	dma_rxq5_rptr.bf.index = (dma_rxq5_rptr.bf.index + 1) % FDMA_DESC_NUM;
	rx_desc[dma_rxq5_rptr.bf.index].word1.bf.own = OWN_DMA;
	rx_desc[dma_rxq5_rptr.bf.index].word1.bf.buf_len = mtd->oobsize;
	rx_desc[dma_rxq5_rptr.bf.index].buf_adr = (unsigned long)addr;

	/*update page tx write ptr */
	dma_txq5_wptr.bf.index = (dma_txq5_wptr.bf.index + 1) % FDMA_DESC_NUM;
	write_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_WPTR, dma_txq5_wptr.wrd);
	/*set axi_bus_len = 8
	   set fifo control */
	//fifo_ctl.wrd = 0;
	//fifo_ctl.bf.fifoCmd = FLASH_WT;
	//write_flash_ctrl_reg(FLASH_NF_FIFO_CONTROL, fifo_ctl.wrd);

	flash_fifo_ctrl.wrd = read_flash_ctrl_reg(FLASH_FIFO_CONTROL);
	flash_fifo_ctrl.bf.fifoCmd = FIFO_CMD_WRITE;
	write_flash_ctrl_reg(FLASH_FIFO_CONTROL, flash_fifo_ctrl.wrd);

	flash_start.wrd = 0;
	flash_start.bf.nflashFifoReq = FLASH_GO;
	/*flash_start.bf.nflashRegCmd = FLASH_WT; */
	write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, flash_start.wrd);

	/*enable txq5 */
	dma_txq5_ctrl.bf.txq_en = 1;
	write_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_CONTROL, dma_txq5_ctrl.wrd);

	dma_ssp_rxq5_intsts.wrd = read_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_RXQ_COAL_INTERRUPT);
	tmp_dma_ssp_rxq5_intsts.wrd = 0;
	tmp_dma_ssp_rxq5_intsts.bf.rxq_eof = 1;

	dma_wait(DMA_SEC_DMA_SSP_Q_RXQ_COAL_INTERRUPT, tmp_dma_ssp_rxq5_intsts.wrd, tmp_dma_ssp_rxq5_intsts.wrd, 1000);

	dma_ssp_txq5_intsts.wrd = read_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_COAL_INTERRUPT);
	tmp_dma_ssp_txq5_intsts.wrd = 0;
	tmp_dma_ssp_txq5_intsts.bf.txq_eof = 1;

	dma_wait(DMA_SEC_DMA_SSP_Q_TXQ_COAL_INTERRUPT, tmp_dma_ssp_txq5_intsts.wrd, tmp_dma_ssp_txq5_intsts.wrd, 1000);

	/*clr tx/rx eof */

	dma_ssp_txq5_intsts.bf.txq_eof = 0;
	write_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_COAL_INTERRUPT, dma_ssp_txq5_intsts.wrd);

	dma_ssp_rxq5_intsts.bf.rxq_eof = 0;
	write_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_RXQ_COAL_INTERRUPT, dma_ssp_rxq5_intsts.wrd);

#if defined( CONFIG_CORTINA_PNAND_ECC_HW_BCH)
	bch_sts.wrd = read_flash_ctrl_reg(FLASH_NF_BCH_STATUS);
	tmp_bsc_sts.wrd = 0;
	tmp_bsc_sts.bf.bchGenDone = 1;

	reg_wait(FLASH_NF_BCH_STATUS, tmp_bsc_sts.wrd, tmp_bsc_sts.wrd, 1000);

	udelay(10);

#else

	ecc_sts.wrd = read_flash_ctrl_reg(FLASH_NF_ECC_STATUS);
	tmp_ecc_sts.wrd = 0;
	tmp_ecc_sts.bf.eccDone = 1;

	reg_wait(FLASH_NF_ECC_STATUS, tmp_ecc_sts.wrd, tmp_ecc_sts.wrd, 1000);

#endif

	for (i = 0, j = 0; eccsteps; eccsteps--, i++, j += eccbytes) {
#if defined( CONFIG_CORTINA_PNAND_ECC_HW_BCH)

		reg_bch_gen = read_flash_ctrl_reg(FLASH_NF_BCH_GEN0_0 + 72 * i);
		chip->oob_poi[eccpos[j]] = reg_bch_gen & 0xff;
		chip->oob_poi[eccpos[j + 1]] = (reg_bch_gen >> 8) & 0xff;
		chip->oob_poi[eccpos[j + 2]] = (reg_bch_gen >> 16) & 0xff;
		chip->oob_poi[eccpos[j + 3]] = (reg_bch_gen >> 24) & 0xff;
		reg_bch_gen = read_flash_ctrl_reg(FLASH_NF_BCH_GEN0_1 + 72 * i);
		chip->oob_poi[eccpos[j + 4]] = reg_bch_gen & 0xff;
		chip->oob_poi[eccpos[j + 5]] = (reg_bch_gen >> 8) & 0xff;
		chip->oob_poi[eccpos[j + 6]] = (reg_bch_gen >> 16) & 0xff;
		chip->oob_poi[eccpos[j + 7]] = (reg_bch_gen >> 24) & 0xff;
		reg_bch_gen = read_flash_ctrl_reg(FLASH_NF_BCH_GEN0_2 + 72 * i);
		chip->oob_poi[eccpos[j + 8]] = reg_bch_gen & 0xff;
		chip->oob_poi[eccpos[j + 9]] = (reg_bch_gen >> 8) & 0xff;
		chip->oob_poi[eccpos[j + 10]] = (reg_bch_gen >> 16) & 0xff;
		chip->oob_poi[eccpos[j + 11]] = (reg_bch_gen >> 24) & 0xff;
		reg_bch_gen = read_flash_ctrl_reg(FLASH_NF_BCH_GEN0_3 + 72 * i);
		chip->oob_poi[eccpos[j + 12]] = reg_bch_gen & 0xff;
		chip->oob_poi[eccpos[j + 13]] = (reg_bch_gen >> 8) & 0xff;
#if (CONFIG_CORTINA_PNAND_ECC_LEVEL > 2)
		chip->oob_poi[eccpos[j + 14]] = (reg_bch_gen >> 16) & 0xff;
		chip->oob_poi[eccpos[j + 15]] = (reg_bch_gen >> 24) & 0xff;
		reg_bch_gen = read_flash_ctrl_reg(FLASH_NF_BCH_GEN0_4 + 72 * i);
		chip->oob_poi[eccpos[j + 16]] = reg_bch_gen & 0xff;
		chip->oob_poi[eccpos[j + 17]] = (reg_bch_gen >> 8) & 0xff;
		chip->oob_poi[eccpos[j + 18]] = (reg_bch_gen >> 16) & 0xff;
		chip->oob_poi[eccpos[j + 19]] = (reg_bch_gen >> 24) & 0xff;
		reg_bch_gen = read_flash_ctrl_reg(FLASH_NF_BCH_GEN0_5 + 72 * i);
		chip->oob_poi[eccpos[j + 20]] = reg_bch_gen & 0xff;
		chip->oob_poi[eccpos[j + 21]] = (reg_bch_gen >> 8) & 0xff;
		chip->oob_poi[eccpos[j + 22]] = (reg_bch_gen >> 16) & 0xff;
		chip->oob_poi[eccpos[j + 23]] = (reg_bch_gen >> 24) & 0xff;
		reg_bch_gen = read_flash_ctrl_reg(FLASH_NF_BCH_GEN0_6 + 72 * i);
		chip->oob_poi[eccpos[j + 24]] = reg_bch_gen & 0xff;
		chip->oob_poi[eccpos[j + 25]] = (reg_bch_gen >> 8) & 0xff;
		chip->oob_poi[eccpos[j + 26]] = (reg_bch_gen >> 16) & 0xff;
		chip->oob_poi[eccpos[j + 27]] = (reg_bch_gen >> 24) & 0xff;

#endif
#if (CONFIG_CORTINA_PNAND_ECC_LEVEL > 3)
		reg_bch_gen = read_flash_ctrl_reg(FLASH_NF_BCH_GEN0_7 + 72 * i);
		chip->oob_poi[eccpos[j + 28]] = reg_bch_gen & 0xff;
		chip->oob_poi[eccpos[j + 29]] = (reg_bch_gen >> 8) & 0xff;
		chip->oob_poi[eccpos[j + 30]] = (reg_bch_gen >> 16) & 0xff;
		chip->oob_poi[eccpos[j + 31]] = (reg_bch_gen >> 24) & 0xff;
		reg_bch_gen = read_flash_ctrl_reg(FLASH_NF_BCH_GEN0_8 + 72 * i);
		chip->oob_poi[eccpos[j + 32]] = reg_bch_gen & 0xff;
		chip->oob_poi[eccpos[j + 33]] = (reg_bch_gen >> 8) & 0xff;
		chip->oob_poi[eccpos[j + 34]] = (reg_bch_gen >> 16) & 0xff;
		chip->oob_poi[eccpos[j + 35]] = (reg_bch_gen >> 24) & 0xff;
		reg_bch_gen = read_flash_ctrl_reg(FLASH_NF_BCH_GEN0_9 + 72 * i);
		chip->oob_poi[eccpos[j + 36]] = reg_bch_gen & 0xff;
		chip->oob_poi[eccpos[j + 37]] = (reg_bch_gen >> 8) & 0xff;
		chip->oob_poi[eccpos[j + 38]] = (reg_bch_gen >> 16) & 0xff;
		chip->oob_poi[eccpos[j + 39]] = (reg_bch_gen >> 24) & 0xff;
		reg_bch_gen = read_flash_ctrl_reg(FLASH_NF_BCH_GEN0_10 + 72 * i);
		chip->oob_poi[eccpos[j + 40]] = reg_bch_gen & 0xff;
		chip->oob_poi[eccpos[j + 41]] = (reg_bch_gen >> 8) & 0xff;
#endif
#if (CONFIG_CORTINA_PNAND_ECC_LEVEL > 4)
		chip->oob_poi[eccpos[j + 42]] = (reg_bch_gen >> 16) & 0xff;
		chip->oob_poi[eccpos[j + 43]] = (reg_bch_gen >> 24) & 0xff;
		reg_bch_gen = read_flash_ctrl_reg(FLASH_NF_BCH_GEN0_11 + 72 * i);
		chip->oob_poi[eccpos[j + 44]] = reg_bch_gen & 0xff;
		chip->oob_poi[eccpos[j + 45]] = (reg_bch_gen >> 8) & 0xff;
		chip->oob_poi[eccpos[j + 46]] = (reg_bch_gen >> 16) & 0xff;
		chip->oob_poi[eccpos[j + 47]] = (reg_bch_gen >> 24) & 0xff;
		reg_bch_gen = read_flash_ctrl_reg(FLASH_NF_BCH_GEN0_12 + 72 * i);
		chip->oob_poi[eccpos[j + 48]] = reg_bch_gen & 0xff;
		chip->oob_poi[eccpos[j + 49]] = (reg_bch_gen >> 8) & 0xff;
		chip->oob_poi[eccpos[j + 50]] = (reg_bch_gen >> 16) & 0xff;
		chip->oob_poi[eccpos[j + 51]] = (reg_bch_gen >> 24) & 0xff;
		reg_bch_gen = read_flash_ctrl_reg(FLASH_NF_BCH_GEN0_13 + 72 * i);
		chip->oob_poi[eccpos[j + 52]] = reg_bch_gen & 0xff;
		chip->oob_poi[eccpos[j + 53]] = (reg_bch_gen >> 8) & 0xff;
		chip->oob_poi[eccpos[j + 54]] = (reg_bch_gen >> 16) & 0xff;
		chip->oob_poi[eccpos[j + 55]] = (reg_bch_gen >> 24) & 0xff;
		reg_bch_gen = read_flash_ctrl_reg(FLASH_NF_BCH_GEN0_14 + 72 * i);
		chip->oob_poi[eccpos[j + 56]] = reg_bch_gen & 0xff;
		chip->oob_poi[eccpos[j + 57]] = (reg_bch_gen >> 8) & 0xff;
		chip->oob_poi[eccpos[j + 58]] = (reg_bch_gen >> 16) & 0xff;
		chip->oob_poi[eccpos[j + 59]] = (reg_bch_gen >> 24) & 0xff;
		reg_bch_gen = read_flash_ctrl_reg(FLASH_NF_BCH_GEN0_15 + 72 * i);
		chip->oob_poi[eccpos[j + 60]] = reg_bch_gen & 0xff;
		chip->oob_poi[eccpos[j + 61]] = (reg_bch_gen >> 8) & 0xff;
		chip->oob_poi[eccpos[j + 62]] = (reg_bch_gen >> 16) & 0xff;
		chip->oob_poi[eccpos[j + 63]] = (reg_bch_gen >> 24) & 0xff;
		reg_bch_gen = read_flash_ctrl_reg(FLASH_NF_BCH_GEN0_16 + 72 * i);
		chip->oob_poi[eccpos[j + 64]] = reg_bch_gen & 0xff;
		chip->oob_poi[eccpos[j + 65]] = (reg_bch_gen >> 8) & 0xff;
		chip->oob_poi[eccpos[j + 66]] = (reg_bch_gen >> 16) & 0xff;
		chip->oob_poi[eccpos[j + 67]] = (reg_bch_gen >> 24) & 0xff;
		reg_bch_gen = read_flash_ctrl_reg(FLASH_NF_BCH_GEN0_17 + 72 * i);
		chip->oob_poi[eccpos[j + 68]] = reg_bch_gen & 0xff;
		chip->oob_poi[eccpos[j + 69]] = (reg_bch_gen >> 8) & 0xff;

#endif
#else

		ecc_gen0.wrd = read_flash_ctrl_reg(FLASH_NF_ECC_GEN0 + 4 * i);
		chip->oob_poi[eccpos[j]] = ecc_gen0.wrd & 0xff;
		chip->oob_poi[eccpos[j + 1]] = (ecc_gen0.wrd >> 8) & 0xff;
		chip->oob_poi[eccpos[j + 2]] = (ecc_gen0.wrd >> 16) & 0xff;
		/* printf("%x ", ecc_gen0.wrd);         */
#endif
	}
	/*printf("\n"); */

	/* Disable ECC/BCH after read out */
#if defined(CONFIG_CORTINA_PNAND_ECC_HW_BCH)
	bch_ctrl.wrd = read_flash_ctrl_reg(FLASH_NF_BCH_CONTROL);	/*disable ecc gen */
	bch_ctrl.bf.bchEn = BCH_DISABLE;
	write_flash_ctrl_reg(FLASH_NF_BCH_CONTROL, bch_ctrl.wrd);

	/* jenfeng clear erase tag */
	*(chip->oob_poi + chip->ecc.layout->oobfree[0].offset + chip->ecc.layout->oobfree[0].length) = 0;
#else
	ecc_ctl.wrd = read_flash_ctrl_reg(FLASH_NF_ECC_CONTROL);
	ecc_ctl.bf.eccEn = 0;
	write_flash_ctrl_reg(FLASH_NF_ECC_CONTROL, ecc_ctl.wrd);	/*disable ecc gen */
#endif

	/*dma_map_single( NULL, (void *)chip->oob_poi, mtd->oobsize, DMA_TO_DEVICE);

	   dma_txq5_wptr.wrd = read_dma_ctrl_reg(DMA_DMA_SSP_TXQ5_WPTR);
	 */
	tx_desc[dma_txq5_wptr.bf.index].word1.bf.own = OWN_DMA;
	tx_desc[dma_txq5_wptr.bf.index].word1.bf.buf_len = mtd->oobsize;
	tx_desc[dma_txq5_wptr.bf.index].buf_adr = (unsigned long)(chip->oob_poi);

	/*dma_cache_sync(NULL, chip->oob_poi, mtd->oobsize, DMA_BIDIRECTIONAL);
	   update tx write ptr
	 */
	dma_txq5_wptr.bf.index = (dma_txq5_wptr.bf.index + 1) % FDMA_DESC_NUM;
	write_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_WPTR, dma_txq5_wptr.wrd);

	dma_ssp_rxq5_intsts.wrd = read_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_RXQ_COAL_INTERRUPT);
	tmp_dma_ssp_rxq5_intsts.wrd = 0;
	tmp_dma_ssp_rxq5_intsts.bf.rxq_eof = 1;

	dma_wait(DMA_SEC_DMA_SSP_Q_RXQ_COAL_INTERRUPT, tmp_dma_ssp_rxq5_intsts.wrd, tmp_dma_ssp_rxq5_intsts.wrd, 1000);

	dma_ssp_txq5_intsts.wrd = read_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_COAL_INTERRUPT);
	tmp_dma_ssp_txq5_intsts.wrd = 0;
	tmp_dma_ssp_txq5_intsts.bf.txq_eof = 1;

	dma_wait(DMA_SEC_DMA_SSP_Q_TXQ_COAL_INTERRUPT, tmp_dma_ssp_txq5_intsts.wrd, tmp_dma_ssp_txq5_intsts.wrd, 1000);

	flash_start.wrd = read_flash_ctrl_reg(FLASH_FLASH_ACCESS_START);

	tmp_access.wrd = 0;
	tmp_access.bf.nflashFifoReq = 1;
	reg_wait(FLASH_FLASH_ACCESS_START, tmp_access.wrd, 0, 1000);

	/*update rx read ptr
	   dma_rxq5_rptr.wrd = read_dma_ctrl_reg(DMA_DMA_SSP_RXQ5_RPTR);
	 */
	dma_rxq5_rptr.bf.index = (dma_rxq5_rptr.bf.index + 1) % FDMA_DESC_NUM;
	write_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_RXQ_RPTR, dma_rxq5_rptr.wrd);

	/*chip->write_buf(mtd, chip->oob_poi, mtd->oobsize); */
	return 0;
}

/**
 * ca_nand_read_page_hwecc - [REPLACABLE] hardware ecc based page read function
 * @mtd:	mtd info structure
 * @chip:	nand chip info structure
 * @buf:	buffer to store read data
 * @page:	page number to read
 *
 * Not for syndrome calculating ecc controllers which need a special oob layout
 */
int ca_nand_read_page_hwecc(struct mtd_info *mtd, struct nand_chip *chip, uint8_t * buf, int oob_required, int page)
{
	int i, eccsize = chip->ecc.size;
	int eccbytes = chip->ecc.bytes;
	int eccsteps = chip->ecc.steps;
	uint8_t *p = buf;
	/*uint8_t *ecc_calc = chip->buffers->ecccalc; */
	uint8_t *ecc_code = chip->buffers->ecccode;
	uint32_t *eccpos = chip->ecc.layout->eccpos, *addr;;
	DMA_SEC_DMA_SSP_Q_RXQ_COAL_INTERRUPT_t tmp_dma_ssp_rxq5_intsts;
	DMA_SEC_DMA_SSP_Q_TXQ_COAL_INTERRUPT_t tmp_dma_ssp_txq5_intsts;
	FLASH_FLASH_ACCESS_START_t tmp_access;
#if defined(CONFIG_CORTINA_PNAND_ECC_HW_BCH)
	FLASH_NF_BCH_STATUS_t tmp_bsc_sts;
#else
	FLASH_NF_ECC_STATUS_t tmp_ecc_sts;
#endif
	FLASH_FIFO_CONTROL_t flash_fifo_ctrl;
	//FLASH_NF_ECC_RESET_t ecc_rst;

	check_flash_ctrl_status();

	p = buf;
	ecc_reset.wrd = 0;
	ecc_reset.bf.eccClear = ECC_CLR;
	write_flash_ctrl_reg(FLASH_NF_ECC_RESET, ecc_reset.wrd);

	flash_int_sts.bf.regIrq = 1;
	write_flash_ctrl_reg(FLASH_FLASH_INTERRUPT, flash_int_sts.wrd);

	ecc_reset.wrd = 0;
	ecc_reset.bf.eccClear = 1;
	write_flash_ctrl_reg(FLASH_NF_ECC_RESET, ecc_reset.wrd);

#ifdef CONFIG_CORTINA_PNAND_ECC_HW_BCH
	bch_ctrl.wrd = 0;
	bch_ctrl.bf.bchEn = BCH_ENABLE;
	bch_ctrl.bf.bchOpcode = BCH_DECODE;
#if	(CONFIG_CORTINA_PNAND_ECC_LEVEL == 2)
	bch_ctrl.bf.bchErrCap = BCH_ERR_CAP_8;
#elif (CONFIG_CORTINA_PNAND_ECC_LEVEL == 3)
	bch_ctrl.bf.bchErrCap = BCH_ERR_CAP_16;
#elif (CONFIG_CORTINA_PNAND_ECC_LEVEL == 4)
	bch_ctrl.bf.bchErrCap = BCH_ERR_CAP_24;
#elif (CONFIG_CORTINA_PNAND_ECC_LEVEL == 5)
	bch_ctrl.bf.bchErrCap = BCH_ERR_CAP_40;
#else
	printf("BCH Level is not recognized! %s:%d\n", __func__, __LINE__);
#endif
	write_flash_ctrl_reg(FLASH_NF_BCH_CONTROL, bch_ctrl.wrd);

#else

	ecc_ctl.wrd = 0;
	if ((eccsize - 1) == NCNT_512P_DATA)
		ecc_ctl.bf.eccGenMode = ECC_GEN_512;
	else
		ecc_ctl.bf.eccGenMode = ECC_GEN_256;
	ecc_ctl.bf.eccEn = ECC_ENABLE;
	write_flash_ctrl_reg(FLASH_NF_ECC_CONTROL, ecc_ctl.wrd);
#endif

	/*disable txq5 */
	dma_txq5_ctrl.bf.txq_en = 0;
	write_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_CONTROL, dma_txq5_ctrl.wrd);

	dma_ssp_txq5_intsts.wrd = read_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_COAL_INTERRUPT);
	dma_ssp_txq5_intsts.bf.txq_eof = 0;
	write_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_COAL_INTERRUPT, dma_ssp_txq5_intsts.wrd);
	dma_ssp_rxq5_intsts.wrd = read_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_RXQ_COAL_INTERRUPT);
	dma_ssp_rxq5_intsts.bf.rxq_eof = 0;
	write_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_RXQ_COAL_INTERRUPT, dma_ssp_rxq5_intsts.wrd);

	/*for indirect access with DMA, because DMA not ready   */
	nf_cnt.wrd = 0;
	nf_cmd.wrd = 0;
	nf_addr1.wrd = 0;
	nf_addr2.wrd = 0;
	nf_cnt.wrd = 0;
	nf_cnt.bf.nflashRegOobCount = mtd->oobsize - 1;
	nf_cnt.bf.nflashRegDataCount = mtd->writesize - 1;

	if (chip->chipsize < (32 << 20)) {
		nf_cnt.bf.nflashRegAddrCount = NCNT_ADDR_3;
		nf_cnt.bf.nflashRegCmdCount = NCNT_CMD_1;
		nf_cmd.bf.nflashRegCmd0 = NAND_CMD_READ0;
		nf_addr1.wrd = (((page & 0x00ffffff) << 8));
		nf_addr2.wrd = ((page & 0xff000000) >> 24);
	} else if ((chip->chipsize >= (32 << 20))
		   && (chip->chipsize <= (128 << 20))) {
		nf_cnt.bf.nflashRegAddrCount = NCNT_ADDR_4;
		nf_cnt.bf.nflashRegCmdCount = NCNT_CMD_1;
		nf_cmd.bf.nflashRegCmd0 = NAND_CMD_READ0;

		if (mtd->writesize > NCNT_512P_DATA) {
			nf_cnt.bf.nflashRegCmdCount = NCNT_CMD_2;
			nf_cmd.bf.nflashRegCmd1 = NAND_CMD_READSTART;
		}
		nf_addr1.wrd = (((page & 0xffff) << 16));
		nf_addr2.wrd = ((page & 0xffff0000) >> 16);
	} else {		/*if((chip->chipsize > (128 << 20)) )) */

		nf_cnt.bf.nflashRegAddrCount = NCNT_ADDR_5;
		nf_cnt.bf.nflashRegCmdCount = NCNT_CMD_2;
		nf_cmd.bf.nflashRegCmd0 = NAND_CMD_READ0;
		nf_cmd.bf.nflashRegCmd1 = NAND_CMD_READSTART;
		nf_addr1.wrd = (((page & 0xffff) << 16));
		nf_addr2.wrd = ((page & 0xffff0000) >> 16);
	}

	write_flash_ctrl_reg(FLASH_NF_COUNT, nf_cnt.wrd);
	write_flash_ctrl_reg(FLASH_NF_COMMAND, nf_cmd.wrd);	/*write read id command */
	write_flash_ctrl_reg(FLASH_NF_ADDRESS_1, nf_addr1.wrd);	/*write address 0x0 */
	write_flash_ctrl_reg(FLASH_NF_ADDRESS_2, nf_addr2.wrd);

	nf_access.wrd = 0;
	nf_access.bf.nflashCeAlt = CHIP_EN;
	nf_access.bf.nflashRegWidth = NFLASH_WiDTH8;
	/*      write_flash_ctrl_reg(FLASH_NF_ACCESS, nf_access.wrd);
	   prepare dma descriptor
	   nf_access.wrd = read_flash_ctrl_reg(FLASH_FLASH_ACCESS_START);
	 */
	nf_access.bf.nflashExtAddr = ((page << chip->page_shift) / 0x8000000);
	write_flash_ctrl_reg(FLASH_NF_ACCESS, nf_access.wrd);

	addr = (unsigned int *)(((unsigned long)page << chip->page_shift) % 0x8000000);
	addr = (unsigned int *)((unsigned long)addr + (unsigned int)(CONFIG_SYS_FLASH_BASE));

	/*printf("%s : addr : %p  buf: %p",__func__, addr, buf);
	   chip->buffers->databuf
	   page tx data desc
	 */
	dma_txq5_wptr.wrd = read_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_WPTR);

	tx_desc[dma_txq5_wptr.bf.index].word1.bf.own = OWN_DMA;
	tx_desc[dma_txq5_wptr.bf.index].word1.bf.buf_len = mtd->writesize;
	tx_desc[dma_txq5_wptr.bf.index].buf_adr = (unsigned long)addr;

	/*oob tx desc */
	dma_txq5_wptr.bf.index = (dma_txq5_wptr.bf.index + 1) % FDMA_DESC_NUM;

	addr = (unsigned int *)((unsigned long)addr + mtd->writesize);
	/*printf("   oob : addr (%p)  chip->oob_poi (%p) \n", addr, chip->oob_poi); */
	tx_desc[dma_txq5_wptr.bf.index].word1.bf.own = OWN_DMA;
	tx_desc[dma_txq5_wptr.bf.index].word1.bf.buf_len = mtd->oobsize;
	tx_desc[dma_txq5_wptr.bf.index].buf_adr = (unsigned long)addr;

	/*page data rx desc */
	dma_rxq5_rptr.wrd = read_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_RXQ_RPTR);
	rx_desc[dma_rxq5_rptr.bf.index].word1.bf.own = OWN_DMA;
	rx_desc[dma_rxq5_rptr.bf.index].word1.bf.buf_len = mtd->writesize;

	rx_desc[dma_rxq5_rptr.bf.index].buf_adr = (unsigned long)(buf);

	/*oob rx desc */
	dma_rxq5_rptr.bf.index = (dma_rxq5_rptr.bf.index + 1) % FDMA_DESC_NUM;
	rx_desc[dma_rxq5_rptr.bf.index].word1.bf.own = OWN_DMA;
	rx_desc[dma_rxq5_rptr.bf.index].word1.bf.buf_len = mtd->oobsize;
	rx_desc[dma_rxq5_rptr.bf.index].buf_adr = (unsigned long)(chip->oob_poi);

	/*dma_map_single( NULL, (void *)chip->oob_poi, mtd->oobsize, DMA_FROM_DEVICE);
	   dma_map_single( NULL, (void *)buf, mtd->writesize, DMA_FROM_DEVICE);

	   set axi_bus_len = 8

	   set fifo control
	 */
	//fifo_ctl.wrd = 0;
	//fifo_ctl.bf.fifoCmd = FLASH_RD;
	//write_flash_ctrl_reg(FLASH_NF_FIFO_CONTROL, fifo_ctl.wrd);

	flash_fifo_ctrl.wrd = read_flash_ctrl_reg(FLASH_FIFO_CONTROL);
	flash_fifo_ctrl.bf.fifoCmd = FIFO_CMD_READ;
	write_flash_ctrl_reg(FLASH_FIFO_CONTROL, flash_fifo_ctrl.wrd);

	flash_start.wrd = 0;
	flash_start.bf.nflashFifoReq = FLASH_GO;
	/*flash_start.bf.nflashRegCmd = FLASH_RD; */
	write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, flash_start.wrd);

	/*update tx write ptr */
	dma_txq5_wptr.bf.index = (dma_txq5_wptr.bf.index + 1) % FDMA_DESC_NUM;
	write_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_WPTR, dma_txq5_wptr.wrd);
	/*dma_txq5_wptr.wrd = read_dma_ctrl_reg(DMA_DMA_SSP_TXQ5_WPTR);

	   enable txq5
	 */
	dma_txq5_ctrl.bf.txq_en = 1;
	write_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_CONTROL, dma_txq5_ctrl.wrd);

	tmp_dma_ssp_rxq5_intsts.wrd = 0;
	tmp_dma_ssp_rxq5_intsts.bf.rxq_eof = 1;

	dma_wait(DMA_SEC_DMA_SSP_Q_RXQ_COAL_INTERRUPT, tmp_dma_ssp_rxq5_intsts.wrd, tmp_dma_ssp_rxq5_intsts.wrd, 1000);

	tmp_dma_ssp_txq5_intsts.wrd = 0;
	tmp_dma_ssp_txq5_intsts.bf.txq_eof = 1;

	dma_wait(DMA_SEC_DMA_SSP_Q_TXQ_COAL_INTERRUPT, tmp_dma_ssp_txq5_intsts.wrd, tmp_dma_ssp_txq5_intsts.wrd, 1000);

	tmp_access.wrd = 0;
	tmp_access.bf.nflashFifoReq = 1;
	reg_wait(FLASH_FLASH_ACCESS_START, tmp_access.wrd, 0, 1000);

	/**
	  ecc_sts.wrd=read_flash_ctrl_reg(FLASH_NF_ECC_STATUS);
	  while(!ecc_sts.bf.eccDone)
	  {
	  ecc_sts.wrd=read_flash_ctrl_reg(FLASH_NF_ECC_STATUS);
	  udelay(1);

	  }

	  write_flash_ctrl_reg(FLASH_NF_ECC_CONTROL, 0);

	dma_cache_sync(NULL, buf, mtd->writesize, DMA_BIDIRECTIONAL);
	dma_cache_sync(NULL, chip->oob_poi, mtd->oobsize, DMA_BIDIRECTIONAL);
	update rx read ptr
	**/
	dma_rxq5_rptr.bf.index = (dma_rxq5_rptr.bf.index + 1) % FDMA_DESC_NUM;
	write_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_RXQ_RPTR, dma_rxq5_rptr.wrd);

#if defined(CONFIG_CORTINA_PNAND_ECC_HW_BCH)
	bch_sts.wrd = read_flash_ctrl_reg(FLASH_NF_BCH_STATUS);

	tmp_bsc_sts.wrd = 0;
	tmp_bsc_sts.bf.bchGenDone = 1;

	reg_wait(FLASH_NF_BCH_STATUS, tmp_bsc_sts.wrd, tmp_bsc_sts.wrd, 1000);

	/**write_flash_ctrl_reg(FLASH_NF_BCH_CONTROL, 0);  **/

#else
	ecc_sts.wrd = read_flash_ctrl_reg(FLASH_NF_ECC_STATUS);

	tmp_ecc_sts.wrd = 0;
	tmp_ecc_sts.bf.eccDone = 1;

	reg_wait(FLASH_NF_ECC_STATUS, tmp_ecc_sts.wrd, tmp_ecc_sts.wrd, 1000);

	ecc_ctl.wrd = read_flash_ctrl_reg(FLASH_NF_ECC_CONTROL);   /**disable ecc gen**/
	ecc_ctl.bf.eccEn = 0;
	write_flash_ctrl_reg(FLASH_NF_ECC_CONTROL, ecc_ctl.wrd);  /**disable ecc gen**/
#endif
	/**dma_cache_maint((void *)chip->oob_poi, mtd->oobsize, DMA_FROM_DEVICE);
	dma_cache_maint((void *)buf, mtd->writesize, DMA_FROM_DEVICE);
	**/

#if defined(CONFIG_CORTINA_PNAND_ECC_HW_BCH)
	/** jenfeng**/
	if (0xff == *(chip->oob_poi + chip->ecc.layout->oobfree[0].offset + chip->ecc.layout->oobfree[0].length)) {
		/** Erase tga is on , No needs to check. **/
		goto BCH_EXIT;
	}
#endif
	for (i = 0; i < chip->ecc.total; i++)
		ecc_code[i] = chip->oob_poi[eccpos[i]];

	for (i = 0; eccsteps; eccsteps--, i += eccbytes, p += eccsize) {

#if defined(CONFIG_CORTINA_PNAND_ECC_HW_BCH)
		int j; /** (i/eccbytes);**/
		unsigned int reg_bch_oob;

#if 1
		reg_bch_oob = ecc_code[i] | ecc_code[i + 1] << 8 | ecc_code[i + 2] << 16 | ecc_code[i + 3] << 24;
		write_flash_ctrl_reg(FLASH_NF_BCH_OOB0, reg_bch_oob);

		reg_bch_oob = ecc_code[i + 4] | ecc_code[i + 5] << 8 | ecc_code[i + 6] << 16 | ecc_code[i + 7] << 24;
		write_flash_ctrl_reg(FLASH_NF_BCH_OOB1, reg_bch_oob);

		reg_bch_oob = ecc_code[i + 8] | ecc_code[i + 9] << 8 | ecc_code[i + 10] << 16 | ecc_code[i + 11] << 24;
		write_flash_ctrl_reg(FLASH_NF_BCH_OOB2, reg_bch_oob);

		reg_bch_oob = ecc_code[i + 12] | ecc_code[i + 13] << 8;
		write_flash_ctrl_reg(FLASH_NF_BCH_OOB3, reg_bch_oob);

#if (CONFIG_CORTINA_PNAND_ECC_LEVEL > 2)
		reg_bch_oob =
		    ecc_code[i + 12] | ecc_code[i + 13] << 8 | ecc_code[i + 14] << 16 | ecc_code[i + 15] << 24;
		write_flash_ctrl_reg(FLASH_NF_BCH_OOB3, reg_bch_oob);

		reg_bch_oob =
		    ecc_code[i + 16] | ecc_code[i + 17] << 8 | ecc_code[i + 18] << 16 | ecc_code[i + 19] << 24;
		write_flash_ctrl_reg(FLASH_NF_BCH_OOB4, reg_bch_oob);

		reg_bch_oob =
		    ecc_code[i + 20] | ecc_code[i + 21] << 8 | ecc_code[i + 22] << 16 | ecc_code[i + 23] << 24;
		write_flash_ctrl_reg(FLASH_NF_BCH_OOB5, reg_bch_oob);

		reg_bch_oob =
		    ecc_code[i + 24] | ecc_code[i + 25] << 8 | ecc_code[i + 26] << 16 | ecc_code[i + 27] << 24;
		write_flash_ctrl_reg(FLASH_NF_BCH_OOB6, reg_bch_oob);
#endif
#if (CONFIG_CORTINA_PNAND_ECC_LEVEL > 3)
		reg_bch_oob =
		    ecc_code[i + 28] | ecc_code[i + 29] << 8 | ecc_code[i + 30] << 16 | ecc_code[i + 31] << 24;
		write_flash_ctrl_reg(FLASH_NF_BCH_OOB7, reg_bch_oob);

		reg_bch_oob =
		    ecc_code[i + 32] | ecc_code[i + 33] << 8 | ecc_code[i + 34] << 16 | ecc_code[i + 35] << 24;
		write_flash_ctrl_reg(FLASH_NF_BCH_OOB8, reg_bch_oob);

		reg_bch_oob =
		    ecc_code[i + 36] | ecc_code[i + 37] << 8 | ecc_code[i + 38] << 16 | ecc_code[i + 39] << 24;
		write_flash_ctrl_reg(FLASH_NF_BCH_OOB9, reg_bch_oob);

		reg_bch_oob = ecc_code[i + 40] | ecc_code[i + 41] << 8;
		write_flash_ctrl_reg(FLASH_NF_BCH_OOB10, reg_bch_oob);
#endif
#if (CONFIG_CORTINA_PNAND_ECC_LEVEL > 4)
		reg_bch_oob =
		    ecc_code[i + 40] | ecc_code[i + 41] << 8 | ecc_code[i + 42] << 16 | ecc_code[i + 43] << 24;
		write_flash_ctrl_reg(FLASH_NF_BCH_OOB10, reg_bch_oob);

		reg_bch_oob =
		    ecc_code[i + 44] | ecc_code[i + 45] << 8 | ecc_code[i + 46] << 16 | ecc_code[i + 47] << 24;
		write_flash_ctrl_reg(FLASH_NF_BCH_OOB11, reg_bch_oob);

		reg_bch_oob =
		    ecc_code[i + 48] | ecc_code[i + 49] << 8 | ecc_code[i + 50] << 16 | ecc_code[i + 51] << 24;
		write_flash_ctrl_reg(FLASH_NF_BCH_OOB12, reg_bch_oob);

		reg_bch_oob =
		    ecc_code[i + 52] | ecc_code[i + 53] << 8 | ecc_code[i + 54] << 16 | ecc_code[i + 55] << 24;
		write_flash_ctrl_reg(FLASH_NF_BCH_OOB13, reg_bch_oob);

		reg_bch_oob =
		    ecc_code[i + 56] | ecc_code[i + 57] << 8 | ecc_code[i + 58] << 16 | ecc_code[i + 59] << 24;
		write_flash_ctrl_reg(FLASH_NF_BCH_OOB14, reg_bch_oob);

		reg_bch_oob =
		    ecc_code[i + 60] | ecc_code[i + 61] << 8 | ecc_code[i + 62] << 16 | ecc_code[i + 63] << 24;
		write_flash_ctrl_reg(FLASH_NF_BCH_OOB15, reg_bch_oob);

		reg_bch_oob =
		    ecc_code[i + 64] | ecc_code[i + 65] << 8 | ecc_code[i + 66] << 16 | ecc_code[i + 67] << 24;
		write_flash_ctrl_reg(FLASH_NF_BCH_OOB16, reg_bch_oob);

		reg_bch_oob = ecc_code[i + 68] | ecc_code[i + 69] << 8;
		write_flash_ctrl_reg(FLASH_NF_BCH_OOB17, reg_bch_oob);

#endif
#endif

		ecc_reset.wrd = read_flash_ctrl_reg(FLASH_NF_ECC_RESET);
		ecc_reset.bf.eccClear = ECC_CLR;
		write_flash_ctrl_reg(FLASH_NF_ECC_RESET, ecc_reset.wrd);

		ecc_reset.wrd = 0;
		ecc_reset.bf.eccClear = 1;
		udelay(10);
		reg_wait(FLASH_NF_ECC_RESET, ecc_reset.wrd, 0, 1000);

		bch_ctrl.wrd = read_flash_ctrl_reg(FLASH_NF_BCH_CONTROL);
		bch_ctrl.bf.bchEn = 0;
		write_flash_ctrl_reg(FLASH_NF_BCH_CONTROL, bch_ctrl.wrd);

		/** enable ecc compare**/
		bch_ctrl.wrd = read_flash_ctrl_reg(FLASH_NF_BCH_CONTROL);
		bch_ctrl.bf.bchCodeSel = (i / eccbytes);
		bch_ctrl.bf.bchEn = 1;
		write_flash_ctrl_reg(FLASH_NF_BCH_CONTROL, bch_ctrl.wrd);
		udelay(10);
		bch_ctrl.bf.bchCompare = 1;
		write_flash_ctrl_reg(FLASH_NF_BCH_CONTROL, bch_ctrl.wrd);

		tmp_bsc_sts.wrd = 0;
		tmp_bsc_sts.bf.bchDecDone = 1;

		reg_wait(FLASH_NF_BCH_STATUS, tmp_bsc_sts.wrd, tmp_bsc_sts.wrd, 1000);

		/** disable ecc compare**/
		bch_ctrl.wrd = read_flash_ctrl_reg(FLASH_NF_BCH_CONTROL);
		bch_ctrl.bf.bchCompare = 0;
		write_flash_ctrl_reg(FLASH_NF_BCH_CONTROL, bch_ctrl.wrd);

		bch_sts.wrd = read_flash_ctrl_reg(FLASH_NF_BCH_STATUS);

		switch (bch_sts.bf.bchDecStatus) {
		case BCH_CORRECTABLE_ERR:
			printf("Correctable error(%x)!! addr:%lx\n",
			       bch_sts.bf.bchErrNum, (unsigned long)addr - mtd->writesize);
			printf("Dst buf: %p [ColSel:%x ]\n",
			       buf + bch_ctrl.bf.bchCodeSel * BCH_DATA_UNIT, bch_ctrl.bf.bchCodeSel);

			for (j = 0; j < ((bch_sts.bf.bchErrNum + 1) / 2); j++) {
				bch_err_loc01.wrd = read_flash_ctrl_reg(FLASH_NF_BCH_ERROR_LOC01 + j * 4);

				if ((j + 1) * 2 <= bch_sts.bf.bchErrNum) {
					if (((bch_err_loc01.bf.bchErrLoc1 & 0x3fff) >> 3) < BCH_DATA_UNIT) {
						printf("pdata[%x]:%x =>",
						       ((i / eccbytes) *
							chip->ecc.size + ((bch_err_loc01.bf.bchErrLoc1 & 0x1fff)
									  >> 3)),
						       p[(bch_err_loc01.bf.bchErrLoc1 & 0x1fff)
							 >> 3]);
						p[(bch_err_loc01.bf.bchErrLoc1 & 0x3fff) >> 3] ^=
						    (1 << (bch_err_loc01.bf.bchErrLoc1 & 0x07));
						printf("%x\n", p[(bch_err_loc01.bf.bchErrLoc1 & 0x1fff)
								 >> 3]);
					} else {
							/** printf( "BCH bit error [%x]:[%x]\n", (bch_err_loc01.bf.bchErrLoc1&0x1fff)>>3 - 0x200, bch_err_loc01.bf.bchErrLoc1 & 0x07); **/

					}
				}

				if (((bch_err_loc01.bf.bchErrLoc0 & 0x3fff) >> 3) < BCH_DATA_UNIT) {
					printf("pdata[%x]:%x =>",
					       ((i / eccbytes) *
						chip->ecc.size +
						((bch_err_loc01.bf.bchErrLoc0 & 0x1fff) >> 3)),
					       p[(bch_err_loc01.bf.bchErrLoc0 & 0x1fff) >> 3]);
					p[(bch_err_loc01.bf.bchErrLoc0 & 0x3fff) >> 3] ^=
					    (1 << (bch_err_loc01.bf.bchErrLoc0 & 0x07));
					printf("%x\n", p[(bch_err_loc01.bf.bchErrLoc0 & 0x1fff) >> 3]);
				} else {
						/** printf( "BCH bit error [%x]:[%x]\n", (bch_err_loc01.bf.bchErrLoc0&0x1fff)>>3 - 0x200, bch_err_loc01.bf.bchErrLoc0 & 0x07); **/
				}
			}
			break;
		case BCH_UNCORRECTABLE:
			printf("uncorrectable error!!step %d \n", (i / eccbytes));
			if ((i / eccbytes > 2) || (i / eccbytes) == 0)
				mtd->ecc_stats.failed++;

			break;
		}
		/** clear compare sts
		bch_sts.bf.bchDecDone = 0;
		write_flash_ctrl_reg(FLASH_NF_BCH_STATUS, bch_sts.wrd);
		**/
		if (dbg_ecc_check && i == 0) {
			unsigned char *free = buf + mtd->writesize + mtd->oobsize;
			*free = bch_sts.bf.bchDecStatus;
			*(free + 1) = bch_sts.bf.bchErrNum;
		}

/*
		ecc_reset.wrd = read_flash_ctrl_reg(FLASH_NF_ECC_RESET);
		ecc_reset.bf.eccClear = ECC_CLR;
//		ecc_reset.bf.fifoClear = FIFO_CLR;
		write_flash_ctrl_reg(FLASH_NF_ECC_RESET, ecc_reset.wrd);

		ecc_reset.wrd = 0;
		ecc_reset.bf.eccClear= 1;
		udelay(100);
		reg_wait(FLASH_NF_ECC_RESET, ecc_reset.wrd , 0, 1000);
*/
#else

		/** for (i = 0 ; eccsteps; eccsteps--, i += eccbytes, p += eccsize) { ***/
		ecc_oob.wrd = ecc_code[i] | ecc_code[i + 1] << 8 | ecc_code[i + 2] << 16;
		write_flash_ctrl_reg(FLASH_NF_ECC_OOB, ecc_oob.wrd);

		ecc_ctl.wrd = read_flash_ctrl_reg(FLASH_NF_ECC_CONTROL);
		ecc_ctl.bf.eccCodeSel = (i / eccbytes);
		write_flash_ctrl_reg(FLASH_NF_ECC_CONTROL, ecc_ctl.wrd);

		ecc_sts.wrd = read_flash_ctrl_reg(FLASH_NF_ECC_STATUS);

		switch (ecc_sts.bf.eccStatus) {
		case ECC_NO_ERR:
				/** printf("ECC no error!!\n");**/
			break;
		case ECC_1BIT_DATA_ERR:
			/* flip the bit */
			p[ecc_sts.bf.eccErrByte] ^= (1 << ecc_sts.bf.eccErrBit);
				/** printf("ecc_code- %x (%x) :\n",i ,chip->ecc.total);**/
			ecc_gen0.wrd = read_flash_ctrl_reg(FLASH_NF_ECC_GEN0 + (4 * (i / eccbytes)));
				/** for (i = 0; i < chip->ecc.total; i++)
				  printf(" %x", ecc_code[i]);
				**/
			printf
			    ("\nECC one bit data error(%x)!!(org: %x) HW(%xs) page(%x)\n",
			     (i / eccbytes), ecc_oob.wrd, ecc_gen0.wrd, page);
			break;
		case ECC_1BIT_ECC_ERR:
				/** printf("ecc_code- %x (%x) :\n",i ,chip->ecc.total);**/
			ecc_gen0.wrd = read_flash_ctrl_reg(FLASH_NF_ECC_GEN0 + (4 * (i / eccbytes)));
				/** for (i = 0; i < chip->ecc.total; i++)
				  printf(" %x", ecc_code[i]);
				**/
			printf
			    ("\nECC one bit ECC error(%x)!!(org: %x) HW(%xs) page(%x)\n",
			     (i / eccbytes), ecc_oob.wrd, ecc_gen0.wrd, page);
			break;
		case ECC_UNCORRECTABLE:
			mtd->ecc_stats.failed++;
			ecc_gen0.wrd = read_flash_ctrl_reg(FLASH_NF_ECC_GEN0 + (4 * (i / eccbytes)));
				/** printf("ecc_code- %x (%x) :\n",i ,chip->ecc.total);
				for (i = 0; i < chip->ecc.total; i++)
				  printf(" %x", ecc_code[i]);
				**/
			printf
			    ("\nECC uncorrectable error(%x)!!(org: %x) HW(%xs) page(%x)\n",
			     (i / eccbytes), ecc_oob.wrd, ecc_gen0.wrd, page);
			break;
		}

		if (dbg_ecc_check && i == 0) {
			unsigned char *free = buf + mtd->writesize + mtd->oobsize;
			*free = ecc_sts.bf.eccStatus;
		}
#endif
	} /** for (i = 0 ; eccsteps; eccsteps--, i += eccbytes, p += eccsize) **/

#if defined(CONFIG_CORTINA_PNAND_ECC_HW_BCH)
 BCH_EXIT:
	/** diasble bch **/
	bch_ctrl.wrd = 0;
	write_flash_ctrl_reg(FLASH_NF_BCH_CONTROL, bch_ctrl.wrd);
#endif

#if 0
	dma_txq5_wptr.wrd = read_dma_ctrl_reg(DMA_DMA_SSP_TXQ5_WPTR);
	dma_rxq5_wptr.wrd = read_dma_ctrl_reg(DMA_DMA_SSP_RXQ5_WPTR);
	dma_ssp_rxq5_intsts.wrd = read_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_RXQ_COAL_INTERRUPT);
	if (dma_ssp_rxq5_intsts.bf.rxq5_full || (dma_txq5_wptr.wrd % 2)
	    || (dma_rxq5_wptr.wrd % 2)) {
		printf("========= %s full[%d] [%d][%d] =============\n",
		       __func__, dma_ssp_rxq5_intsts.bf.rxq5_full, dma_txq5_wptr.wrd, dma_rxq5_wptr.wrd);
		dma_ssp_rxq5_intsts.bf.rxq5_full = 3;
	}
#endif

	return 0;
}

/**
 * ca_nand_write_page_raw - [Intern] raw page write function
 * @mtd:	mtd info structure
 * @chip:	nand chip info structure
 * @buf:	data buffer
 *
 * Not for syndrome calculating ecc controllers, which use a special oob layout
 */
int ca_nand_write_page_raw(struct mtd_info *mtd, struct nand_chip *chip,
			   const uint8_t * buf, int oob_required, int page)
{
	/*uint8_t *ecc_calc = chip->buffers->ecccalc; */
	/*const uint8_t *p = buf; */
	uint32_t *addr;
	DMA_SEC_DMA_SSP_Q_RXQ_COAL_INTERRUPT_t tmp_dma_ssp_rxq5_intsts;
	DMA_SEC_DMA_SSP_Q_TXQ_COAL_INTERRUPT_t tmp_dma_ssp_txq5_intsts;
	FLASH_FLASH_ACCESS_START_t tmp_access;
#ifndef CONFIG_CORTINA_PNAND_ECC_HW_BCH
	FLASH_NF_ECC_STATUS_t __attribute__ ((unused)) tmp_ecc_sts;
#endif
	FLASH_FIFO_CONTROL_t flash_fifo_ctrl;

	check_flash_ctrl_status();

	//page = host->page_addr;

	ecc_reset.wrd = 3;
	ecc_reset.bf.eccClear = ECC_CLR;
	ecc_reset.bf.fifoClear = FIFO_CLR;
	write_flash_ctrl_reg(FLASH_NF_ECC_RESET, ecc_reset.wrd);

	flash_int_sts.bf.regIrq = 1;
	write_flash_ctrl_reg(FLASH_FLASH_INTERRUPT, flash_int_sts.wrd);

	ecc_reset.wrd = 0;
	ecc_reset.bf.eccClear = 1;
	write_flash_ctrl_reg(FLASH_NF_ECC_RESET, ecc_reset.wrd);

#ifdef CONFIG_CORTINA_PNAND_ECC_HW_BCH
	write_flash_ctrl_reg(FLASH_NF_BCH_CONTROL, 0);
#else
	write_flash_ctrl_reg(FLASH_NF_ECC_CONTROL, 0);
#endif

	/*disable txq5 */
	dma_txq5_ctrl.bf.txq_en = 0;
	write_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_CONTROL, dma_txq5_ctrl.wrd);
	/*clr tx/rx eof */
	dma_ssp_txq5_intsts.wrd = read_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_COAL_INTERRUPT);
	dma_ssp_txq5_intsts.bf.txq_eof = 0;
	write_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_COAL_INTERRUPT, dma_ssp_txq5_intsts.wrd);
	dma_ssp_rxq5_intsts.wrd = read_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_RXQ_COAL_INTERRUPT);
	dma_ssp_rxq5_intsts.bf.rxq_eof = 0;
	write_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_RXQ_COAL_INTERRUPT, dma_ssp_rxq5_intsts.wrd);

	nf_cnt.wrd = 0;
	nf_cmd.wrd = 0;
	nf_addr1.wrd = 0;
	nf_addr2.wrd = 0;
	nf_cnt.wrd = 0;
	nf_cnt.bf.nflashRegOobCount = mtd->oobsize - 1;
	nf_cnt.bf.nflashRegDataCount = mtd->writesize - 1;

	if (chip->chipsize < (32 << 20)) {
		nf_cnt.bf.nflashRegAddrCount = NCNT_ADDR_3;
		nf_cnt.bf.nflashRegCmdCount = NCNT_CMD_2;
		nf_cmd.bf.nflashRegCmd0 = NAND_CMD_SEQIN;
		nf_cmd.bf.nflashRegCmd1 = NAND_CMD_PAGEPROG;
		nf_addr1.wrd = (((page & 0x00ffffff) << 8));
		nf_addr2.wrd = ((page & 0xff000000) >> 24);

	} else if ((chip->chipsize >= (32 << 20))
		   && (chip->chipsize <= (128 << 20))) {
		nf_cnt.bf.nflashRegAddrCount = NCNT_ADDR_4;
		nf_cnt.bf.nflashRegCmdCount = NCNT_CMD_2;
		nf_cmd.bf.nflashRegCmd0 = NAND_CMD_SEQIN;
		nf_cmd.bf.nflashRegCmd1 = NAND_CMD_PAGEPROG;
		nf_addr1.wrd = (((page & 0xffff) << 16));
		nf_addr2.wrd = ((page & 0xffff0000) >> 16);

	} else {		/*if((chip->chipsize > (128 << 20)) )) */

		nf_cnt.bf.nflashRegAddrCount = NCNT_ADDR_5;
		nf_cnt.bf.nflashRegCmdCount = NCNT_CMD_2;
		nf_cmd.bf.nflashRegCmd0 = NAND_CMD_SEQIN;
		nf_cmd.bf.nflashRegCmd1 = NAND_CMD_PAGEPROG;
		nf_addr1.wrd = (((page & 0xffff) << 16));
		nf_addr2.wrd = ((page & 0xffff0000) >> 16);
	}

	write_flash_ctrl_reg(FLASH_NF_COUNT, nf_cnt.wrd);
	write_flash_ctrl_reg(FLASH_NF_COMMAND, nf_cmd.wrd);
	write_flash_ctrl_reg(FLASH_NF_ADDRESS_1, nf_addr1.wrd);
	write_flash_ctrl_reg(FLASH_NF_ADDRESS_2, nf_addr2.wrd);

	/*dma_map_single( NULL, (void *)buf, mtd->writesize, DMA_TO_DEVICE); */

	nf_access.wrd = 0;
	nf_access.bf.nflashCeAlt = CHIP_EN;
	nf_access.bf.nflashRegWidth = NFLASH_WiDTH8;
	/*write_flash_ctrl_reg(FLASH_NF_ACCESS, nf_access.wrd);

	   write
	   prepare dma descriptor
	   chip->buffers->databuf
	   nf_access.wrd = read_flash_ctrl_reg(FLASH_FLASH_ACCESS_START);
	 */
	nf_access.bf.nflashExtAddr = ((page << chip->page_shift) / 0x8000000);
	write_flash_ctrl_reg(FLASH_NF_ACCESS, nf_access.wrd);

	addr = (unsigned int *)(((unsigned long)page << chip->page_shift) % 0x8000000);
	addr = (unsigned int *)((unsigned long)addr + (unsigned int)(CONFIG_SYS_FLASH_BASE));

	/*page data tx desc */
	dma_txq5_wptr.wrd = read_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_WPTR);
	tx_desc[dma_txq5_wptr.bf.index].word1.bf.own = OWN_DMA;
	tx_desc[dma_txq5_wptr.bf.index].word1.bf.buf_len = mtd->writesize;
	tx_desc[dma_txq5_wptr.bf.index].buf_adr = (unsigned long)(buf);

	/*page data rx desc
	   printf("ca_nand_write_page_hwecc : addr : %p  buf: %p",addr, buf);
	 */

	dma_rxq5_rptr.wrd = read_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_RXQ_RPTR);
	rx_desc[dma_rxq5_rptr.bf.index].word1.bf.own = OWN_DMA;
	rx_desc[dma_rxq5_rptr.bf.index].word1.bf.buf_len = mtd->writesize;
	rx_desc[dma_rxq5_rptr.bf.index].buf_adr = (unsigned long)addr;

	/*oob rx desc */
	addr = (unsigned int *)((unsigned long)addr + mtd->writesize);
	/*printf("  oob : addr(%p)  chip->oob_poi(%p) \n",addr, chip->oob_poi); */

	dma_rxq5_rptr.bf.index = (dma_rxq5_rptr.bf.index + 1) % FDMA_DESC_NUM;
	rx_desc[dma_rxq5_rptr.bf.index].word1.bf.own = OWN_DMA;
	rx_desc[dma_rxq5_rptr.bf.index].word1.bf.buf_len = mtd->oobsize;
	rx_desc[dma_rxq5_rptr.bf.index].buf_adr = (unsigned long)addr;

	/*update page tx write ptr */
	dma_txq5_wptr.bf.index = (dma_txq5_wptr.bf.index + 1) % FDMA_DESC_NUM;
	write_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_WPTR, dma_txq5_wptr.wrd);
	/*set axi_bus_len = 8
	   set fifo control */
	//fifo_ctl.wrd = 0;
	//fifo_ctl.bf.fifoCmd = FLASH_WT;
	//write_flash_ctrl_reg(FLASH_NF_FIFO_CONTROL, fifo_ctl.wrd);

	flash_fifo_ctrl.wrd = read_flash_ctrl_reg(FLASH_FIFO_CONTROL);
	flash_fifo_ctrl.bf.fifoCmd = FIFO_CMD_WRITE;
	write_flash_ctrl_reg(FLASH_FIFO_CONTROL, flash_fifo_ctrl.wrd);

	flash_start.wrd = 0;
	flash_start.bf.nflashFifoReq = FLASH_GO;
	/*flash_start.bf.nflashRegCmd = FLASH_WT; */
	write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, flash_start.wrd);

	/*enable txq5 */
	dma_txq5_ctrl.bf.txq_en = 1;
	write_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_CONTROL, dma_txq5_ctrl.wrd);

	dma_ssp_rxq5_intsts.wrd = read_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_RXQ_COAL_INTERRUPT);
	tmp_dma_ssp_rxq5_intsts.wrd = 0;
	tmp_dma_ssp_rxq5_intsts.bf.rxq_eof = 1;

	dma_wait(DMA_SEC_DMA_SSP_Q_RXQ_COAL_INTERRUPT, tmp_dma_ssp_rxq5_intsts.wrd, tmp_dma_ssp_rxq5_intsts.wrd, 1000);

	dma_ssp_txq5_intsts.wrd = read_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_COAL_INTERRUPT);
	tmp_dma_ssp_txq5_intsts.wrd = 0;
	tmp_dma_ssp_txq5_intsts.bf.txq_eof = 1;

	dma_wait(DMA_SEC_DMA_SSP_Q_TXQ_COAL_INTERRUPT, tmp_dma_ssp_txq5_intsts.wrd, tmp_dma_ssp_txq5_intsts.wrd, 1000);

	/*clr tx/rx eof */

	dma_ssp_txq5_intsts.bf.txq_eof = 0;
	write_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_COAL_INTERRUPT, dma_ssp_txq5_intsts.wrd);

	dma_ssp_rxq5_intsts.bf.rxq_eof = 0;
	write_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_RXQ_COAL_INTERRUPT, dma_ssp_rxq5_intsts.wrd);

#if defined(CONFIG_CORTINA_PNAND_ECC_HW_BCH)
	/* jenfeng clear erase tag */
	*(chip->oob_poi + chip->ecc.layout->oobfree[0].offset + chip->ecc.layout->oobfree[0].length) = 0;
#endif

	/*dma_map_single( NULL, (void *)chip->oob_poi, mtd->oobsize, DMA_TO_DEVICE);

	   dma_txq5_wptr.wrd = read_dma_ctrl_reg(DMA_DMA_SSP_TXQ5_WPTR);
	 */
	tx_desc[dma_txq5_wptr.bf.index].word1.bf.own = OWN_DMA;
	tx_desc[dma_txq5_wptr.bf.index].word1.bf.buf_len = mtd->oobsize;
	tx_desc[dma_txq5_wptr.bf.index].buf_adr = (unsigned long)(chip->oob_poi);

	/*dma_cache_sync(NULL, chip->oob_poi, mtd->oobsize, DMA_BIDIRECTIONAL);
	   update tx write ptr
	 */
	dma_txq5_wptr.bf.index = (dma_txq5_wptr.bf.index + 1) % FDMA_DESC_NUM;
	write_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_WPTR, dma_txq5_wptr.wrd);

	dma_ssp_rxq5_intsts.wrd = read_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_RXQ_COAL_INTERRUPT);
	tmp_dma_ssp_rxq5_intsts.wrd = 0;
	tmp_dma_ssp_rxq5_intsts.bf.rxq_eof = 1;

	dma_wait(DMA_SEC_DMA_SSP_Q_RXQ_COAL_INTERRUPT, tmp_dma_ssp_rxq5_intsts.wrd, tmp_dma_ssp_rxq5_intsts.wrd, 1000);

	dma_ssp_txq5_intsts.wrd = read_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_COAL_INTERRUPT);
	tmp_dma_ssp_txq5_intsts.wrd = 0;
	tmp_dma_ssp_txq5_intsts.bf.txq_eof = 1;

	dma_wait(DMA_SEC_DMA_SSP_Q_TXQ_COAL_INTERRUPT, tmp_dma_ssp_txq5_intsts.wrd, tmp_dma_ssp_txq5_intsts.wrd, 1000);

	flash_start.wrd = read_flash_ctrl_reg(FLASH_FLASH_ACCESS_START);

	tmp_access.wrd = 0;
	tmp_access.bf.nflashFifoReq = 1;
	reg_wait(FLASH_FLASH_ACCESS_START, tmp_access.wrd, 0, 1000);

	/*update rx read ptr
	   dma_rxq5_rptr.wrd = read_dma_ctrl_reg(DMA_DMA_SSP_RXQ5_RPTR);
	 */
	dma_rxq5_rptr.bf.index = (dma_rxq5_rptr.bf.index + 1) % FDMA_DESC_NUM;
	write_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_RXQ_RPTR, dma_rxq5_rptr.wrd);

	/*chip->write_buf(mtd, chip->oob_poi, mtd->oobsize); */
	return 0;
}

/**
 * ca_nand_read_page_raw - [Intern] read raw page data without ecc
 * @mtd:	mtd info structure
 * @chip:	nand chip info structure
 * @buf:	buffer to store read data
 */
int ca_nand_read_page_raw(struct mtd_info *mtd, struct nand_chip *chip, uint8_t * buf, int oob_required, int page)
{
	int __attribute__ ((unused)) eccbytes = chip->ecc.bytes;
	int __attribute__ ((unused)) eccsteps = chip->ecc.steps;
	int __attribute__ ((unused)) eccsize = eccsize = chip->ecc.size;
	uint32_t *addr;;
	DMA_SEC_DMA_SSP_Q_RXQ_COAL_INTERRUPT_t tmp_dma_ssp_rxq5_intsts;
	DMA_SEC_DMA_SSP_Q_TXQ_COAL_INTERRUPT_t tmp_dma_ssp_txq5_intsts;
	FLASH_FLASH_ACCESS_START_t tmp_access;
#ifndef CONFIG_CORTINA_PNAND_ECC_HW_BCH
	FLASH_NF_ECC_STATUS_t __attribute__ ((unused)) tmp_ecc_sts;
#endif
	FLASH_FIFO_CONTROL_t flash_fifo_ctrl;
	//FLASH_NF_ECC_RESET_t ecc_rst;
//      volatile uint32_t tmp;

//      check_flash_ctrl_status();

	ecc_reset.wrd = 0;
	ecc_reset.bf.eccClear = ECC_CLR;
	ecc_reset.bf.fifoClear = FIFO_CLR;
	write_flash_ctrl_reg(FLASH_NF_ECC_RESET, ecc_reset.wrd);

	flash_int_sts.bf.regIrq = 1;
	write_flash_ctrl_reg(FLASH_FLASH_INTERRUPT, flash_int_sts.wrd);

	ecc_reset.wrd = 0;
	ecc_reset.bf.eccClear = 1;
	write_flash_ctrl_reg(FLASH_NF_ECC_RESET, ecc_reset.wrd);

#ifdef CONFIG_CORTINA_PNAND_ECC_HW_BCH
	bch_ctrl.wrd = 0;
	bch_ctrl.bf.bchEn = BCH_ENABLE;
	bch_ctrl.bf.bchOpcode = BCH_DECODE;
#if	(CONFIG_CORTINA_PNAND_ECC_LEVEL == 2)
	bch_ctrl.bf.bchErrCap = BCH_ERR_CAP_8;
#elif (CONFIG_CORTINA_PNAND_ECC_LEVEL == 3)
	bch_ctrl.bf.bchErrCap = BCH_ERR_CAP_16;
#elif (CONFIG_CORTINA_PNAND_ECC_LEVEL == 4)
	bch_ctrl.bf.bchErrCap = BCH_ERR_CAP_24;
#elif (CONFIG_CORTINA_PNAND_ECC_LEVEL == 5)
	bch_ctrl.bf.bchErrCap = BCH_ERR_CAP_40;
#else
	printf("BCH Level is not recognized! %s:%d\n", __func__, __LINE__);
#endif
	write_flash_ctrl_reg(FLASH_NF_BCH_CONTROL, 0);

#else

	ecc_ctl.wrd = 0;
	if ((eccsize - 1) == NCNT_512P_DATA)
		ecc_ctl.bf.eccGenMode = ECC_GEN_512;
	else
		ecc_ctl.bf.eccGenMode = ECC_GEN_256;
	ecc_ctl.bf.eccEn = ECC_ENABLE;
	write_flash_ctrl_reg(FLASH_NF_ECC_CONTROL, ecc_ctl.wrd);
#endif

	/*disable txq5 */
	dma_txq5_ctrl.bf.txq_en = 0;
	write_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_CONTROL, dma_txq5_ctrl.wrd);

	dma_ssp_txq5_intsts.wrd = read_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_COAL_INTERRUPT);
	dma_ssp_txq5_intsts.bf.txq_eof = 0;
	write_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_COAL_INTERRUPT, dma_ssp_txq5_intsts.wrd);
	dma_ssp_rxq5_intsts.wrd = read_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_RXQ_COAL_INTERRUPT);
	dma_ssp_rxq5_intsts.bf.rxq_eof = 0;
	write_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_RXQ_COAL_INTERRUPT, dma_ssp_rxq5_intsts.wrd);

	/*for indirect access with DMA, because DMA not ready   */
	nf_cnt.wrd = 0;
	nf_cmd.wrd = 0;
	nf_addr1.wrd = 0;
	nf_addr2.wrd = 0;
	nf_cnt.wrd = 0;
	nf_cnt.bf.nflashRegOobCount = mtd->oobsize - 1;
	nf_cnt.bf.nflashRegDataCount = mtd->writesize - 1;

	if (chip->chipsize < (32 << 20)) {
		nf_cnt.bf.nflashRegAddrCount = NCNT_ADDR_3;
		nf_cnt.bf.nflashRegCmdCount = NCNT_CMD_1;
		nf_cmd.bf.nflashRegCmd0 = NAND_CMD_READ0;
		nf_addr1.wrd = (((page & 0x00ffffff) << 8));
		nf_addr2.wrd = ((page & 0xff000000) >> 24);
	} else if ((chip->chipsize >= (32 << 20))
		   && (chip->chipsize <= (128 << 20))) {
		nf_cnt.bf.nflashRegAddrCount = NCNT_ADDR_4;
		nf_cnt.bf.nflashRegCmdCount = NCNT_CMD_1;
		nf_cmd.bf.nflashRegCmd0 = NAND_CMD_READ0;

		if (mtd->writesize > NCNT_512P_DATA) {
			nf_cnt.bf.nflashRegCmdCount = NCNT_CMD_2;
			nf_cmd.bf.nflashRegCmd1 = NAND_CMD_READSTART;
		}
		nf_addr1.wrd = (((page & 0xffff) << 16));
		nf_addr2.wrd = ((page & 0xffff0000) >> 16);
	} else {		/*if((chip->chipsize > (128 << 20)) )) */

		nf_cnt.bf.nflashRegAddrCount = NCNT_ADDR_5;
		nf_cnt.bf.nflashRegCmdCount = NCNT_CMD_2;
		nf_cmd.bf.nflashRegCmd0 = NAND_CMD_READ0;
		nf_cmd.bf.nflashRegCmd1 = NAND_CMD_READSTART;
		nf_addr1.wrd = (((page & 0xffff) << 16));
		nf_addr2.wrd = ((page & 0xffff0000) >> 16);
	}

	write_flash_ctrl_reg(FLASH_NF_COUNT, nf_cnt.wrd);
	write_flash_ctrl_reg(FLASH_NF_COMMAND, nf_cmd.wrd);	/*write read id command */
	write_flash_ctrl_reg(FLASH_NF_ADDRESS_1, nf_addr1.wrd);	/*write address 0x0 */
	write_flash_ctrl_reg(FLASH_NF_ADDRESS_2, nf_addr2.wrd);

	nf_access.wrd = 0;
	nf_access.bf.nflashCeAlt = CHIP_EN;
	nf_access.bf.nflashRegWidth = NFLASH_WiDTH8;
	/*      write_flash_ctrl_reg(FLASH_NF_ACCESS, nf_access.wrd);
	   prepare dma descriptor
	   nf_access.wrd = read_flash_ctrl_reg(FLASH_FLASH_ACCESS_START);
	 */
	nf_access.bf.nflashExtAddr = ((page << chip->page_shift) / 0x8000000);
	write_flash_ctrl_reg(FLASH_NF_ACCESS, nf_access.wrd);

	addr = (unsigned int *)(((unsigned long)page << chip->page_shift) % 0x8000000);
	addr = (unsigned int *)((unsigned long)addr + (unsigned int)(CONFIG_SYS_FLASH_BASE));

	/*printf("%s : addr : %p  buf: %p",__func__, addr, buf);
	   chip->buffers->databuf
	   page tx data desc
	 */
	dma_txq5_wptr.wrd = read_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_WPTR);

	tx_desc[dma_txq5_wptr.bf.index].word1.bf.own = OWN_DMA;
	tx_desc[dma_txq5_wptr.bf.index].word1.bf.buf_len = mtd->writesize;
	tx_desc[dma_txq5_wptr.bf.index].buf_adr = (unsigned long)addr;

	/*oob tx desc */
	dma_txq5_wptr.bf.index = (dma_txq5_wptr.bf.index + 1) % FDMA_DESC_NUM;

	addr = (unsigned int *)((unsigned long)addr + mtd->writesize);
	/*printf("   oob : addr (%p)  chip->oob_poi (%p) \n", addr, chip->oob_poi); */
	tx_desc[dma_txq5_wptr.bf.index].word1.bf.own = OWN_DMA;
	tx_desc[dma_txq5_wptr.bf.index].word1.bf.buf_len = mtd->oobsize;
	tx_desc[dma_txq5_wptr.bf.index].buf_adr = (unsigned long)addr;

	/*page data rx desc */
	dma_rxq5_rptr.wrd = read_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_RXQ_RPTR);
	rx_desc[dma_rxq5_rptr.bf.index].word1.bf.own = OWN_DMA;
	rx_desc[dma_rxq5_rptr.bf.index].word1.bf.buf_len = mtd->writesize;

	rx_desc[dma_rxq5_rptr.bf.index].buf_adr = (unsigned long)(buf);

	/*oob rx desc */
	dma_rxq5_rptr.bf.index = (dma_rxq5_rptr.bf.index + 1) % FDMA_DESC_NUM;
	rx_desc[dma_rxq5_rptr.bf.index].word1.bf.own = OWN_DMA;
	rx_desc[dma_rxq5_rptr.bf.index].word1.bf.buf_len = mtd->oobsize;
	rx_desc[dma_rxq5_rptr.bf.index].buf_adr = (unsigned long)chip->oob_poi;

	/*dma_map_single( NULL, (void *)chip->oob_poi, mtd->oobsize, DMA_FROM_DEVICE);
	   dma_map_single( NULL, (void *)buf, mtd->writesize, DMA_FROM_DEVICE);

	   set axi_bus_len = 8

	   set fifo control
	 */
	//fifo_ctl.wrd = 0;
	//fifo_ctl.bf.fifoCmd = FLASH_RD;
	//write_flash_ctrl_reg(FLASH_NF_FIFO_CONTROL, fifo_ctl.wrd);

	flash_fifo_ctrl.wrd = read_flash_ctrl_reg(FLASH_FIFO_CONTROL);
	flash_fifo_ctrl.bf.fifoCmd = FIFO_CMD_READ;
	write_flash_ctrl_reg(FLASH_FIFO_CONTROL, flash_fifo_ctrl.wrd);

	flash_start.wrd = 0;
	flash_start.bf.nflashFifoReq = FLASH_GO;
	/*flash_start.bf.nflashRegCmd = FLASH_RD; */
	write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, flash_start.wrd);

	/*update tx write ptr */
	dma_txq5_wptr.bf.index = (dma_txq5_wptr.bf.index + 1) % FDMA_DESC_NUM;
	write_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_WPTR, dma_txq5_wptr.wrd);
	/*dma_txq5_wptr.wrd = read_dma_ctrl_reg(DMA_DMA_SSP_TXQ5_WPTR);

	   enable txq5
	 */
	dma_txq5_ctrl.bf.txq_en = 1;
	write_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_CONTROL, dma_txq5_ctrl.wrd);

	tmp_dma_ssp_rxq5_intsts.wrd = 0;
	tmp_dma_ssp_rxq5_intsts.bf.rxq_eof = 1;

	dma_wait(DMA_SEC_DMA_SSP_Q_RXQ_COAL_INTERRUPT, tmp_dma_ssp_rxq5_intsts.wrd, tmp_dma_ssp_rxq5_intsts.wrd, 1000);

	tmp_dma_ssp_txq5_intsts.wrd = 0;
	tmp_dma_ssp_txq5_intsts.bf.txq_eof = 1;

	dma_wait(DMA_SEC_DMA_SSP_Q_TXQ_COAL_INTERRUPT, tmp_dma_ssp_txq5_intsts.wrd, tmp_dma_ssp_txq5_intsts.wrd, 1000);

	tmp_access.wrd = 0;
	tmp_access.bf.nflashFifoReq = 1;
	reg_wait(FLASH_FLASH_ACCESS_START, tmp_access.wrd, 0, 1000);

	/**
	  ecc_sts.wrd=read_flash_ctrl_reg(FLASH_NF_ECC_STATUS);
	  while(!ecc_sts.bf.eccDone)
	  {
	  ecc_sts.wrd=read_flash_ctrl_reg(FLASH_NF_ECC_STATUS);
	  udelay(1);

	  }

	  write_flash_ctrl_reg(FLASH_NF_ECC_CONTROL, 0);

	dma_cache_sync(NULL, buf, mtd->writesize, DMA_BIDIRECTIONAL);
	dma_cache_sync(NULL, chip->oob_poi, mtd->oobsize, DMA_BIDIRECTIONAL);
	update rx read ptr
	**/
	dma_rxq5_rptr.bf.index = (dma_rxq5_rptr.bf.index + 1) % FDMA_DESC_NUM;
	write_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_RXQ_RPTR, dma_rxq5_rptr.wrd);

	return 0;
}

/**
 * ca_nand_write_page - [REPLACEABLE] write one page
 * @mtd:	MTD device structure
 * @chip:	NAND chip descriptor
 * @buf:	the data to write
 * @page:	page number to write
 * @cached:	cached programming
 * @raw:	use _raw version of write_page
 */
int ca_nand_write_page(struct mtd_info *mtd, struct nand_chip *chip,
		       uint32_t offset, int data_len, const uint8_t * buf, int oob_required, int page, int raw)
{
	int status;

	chip->cmdfunc(mtd, NAND_CMD_SEQIN, 0x00, page);

	if (unlikely(raw))
		chip->ecc.write_page_raw(mtd, chip, buf, 1, page);
	else
		chip->ecc.write_page(mtd, chip, buf, 1, page);

	/*
	 * Cached progamming disabled for now, Not sure if its worth the
	 * trouble. The speed gain is not very impressive. (2.3->2.6Mib/s)
	 */
	int cached = 0;

	if (!cached || !(chip->options & NAND_CACHEPRG)) {

		chip->cmdfunc(mtd, NAND_CMD_PAGEPROG, -1, -1);
		status = chip->waitfunc(mtd, chip);

		if (status & NAND_STATUS_FAIL)
			return -EIO;
	} else {
		chip->cmdfunc(mtd, NAND_CMD_CACHEDPROG, -1, -1);
		status = chip->waitfunc(mtd, chip);
	}

#ifdef CONFIG_MTD_NAND_VERIFY_WRITE
	/* Send command to read back the data */
	chip->cmdfunc(mtd, NAND_CMD_READ0, 0, page);

	if (chip->verify_buf(mtd, buf, mtd->writesize))
		return -EIO;
#endif
	return 0;
}

/* Extract the bits of per cell from the 3rd byte of the extended ID */
int nand_get_bits_per_cell(u8 cellinfo)
{
	int bits;

	bits = cellinfo & NAND_CI_CELLTYPE_MSK;
	bits >>= NAND_CI_CELLTYPE_SHIFT;
	return bits + 1;
}

static inline bool is_full_id_nand(struct nand_flash_dev *type)
{
	return type->id_len;
}

static bool find_full_id_nand(struct mtd_info *mtd, struct nand_chip *chip,
			      struct nand_flash_dev *type, u8 * id_data, int *busw)
{
#ifndef __UBOOT__
	if (!strncmp(type->id, id_data, type->id_len)) {
#else
	if (!strncmp((char *)type->id, (char *)id_data, type->id_len)) {
#endif
		mtd->writesize = type->pagesize;
		mtd->erasesize = type->erasesize;
		mtd->oobsize = type->oobsize;

		chip->bits_per_cell = nand_get_bits_per_cell(id_data[2]);
		chip->chipsize = (uint64_t) type->chipsize << 20;
		chip->options |= type->options;
		chip->ecc_strength_ds = NAND_ECC_STRENGTH(type);
		chip->ecc_step_ds = NAND_ECC_STEP(type);

		*busw = type->options & NAND_BUSWIDTH_16;

		if (!mtd->name)
			mtd->name = type->name;

		return true;
	}
	return false;
}

/*
 * Get the flash and manufacturer id and lookup if the type is supported
 */
struct nand_flash_dev *ca_nand_get_flash_type(struct mtd_info *mtd, struct nand_chip *chip, int busw, int *maf_id)
{
	struct nand_flash_dev *type = NULL;
	int maf_idx = 0;
	unsigned char id[8];
	u16 oobsize_8kp[] = { 0, 128, 218, 400, 436, 512, 640, 0 };
	FLASH_TYPE_t flash_type;

	/* Select the device */
	chip->select_chip(mtd, 0);

	/*
	 * Reset the chip, required by some chips (e.g. Micron MT29FxGxxxxx)
	 * after power-up
	 */
	chip->cmdfunc(mtd, NAND_CMD_RESET, -1, -1);

	/* Try if this is ONFI supported chip */
	if (ca_nand_flash_detect_onfi(mtd, chip, &busw) == 1) {
		printf("ONFI\n");
		goto Set_flash_type;
	} else {
		printf("Legacy\n");
	}

	/* Send the command for reading device ID */
	chip->cmdfunc(mtd, NAND_CMD_RESET, -1, -1);
	chip->cmdfunc(mtd, NAND_CMD_READID, 0x00, -1);

	/* Read manufacturer and device IDs */
	memset(id, 0, sizeof(id));
	ca_nand_read_id(0, &id[0]);
	chip->cmdfunc(mtd, NAND_CMD_RESET, -1, -1);
	*maf_id = id[0];
	//dev_id = id[1];
	printf
	    ("id[0:7]=0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x, 0x%02x\n",
	     id[0], id[1], id[2], id[3], id[4], id[5], id[6], id[7]);
	/* Try again to make sure, as some systems the bus-hold or other
	 * interface concerns can cause random data which looks like a
	 * possibly credible NAND flash to appear. If the two results do
	 * not match, ignore the device completely.
	 */

	chip->cmdfunc(mtd, NAND_CMD_READID, 0x00, -1);

	/* Read manufacturer and device IDs */

	/**
	tmp_manf = chip->read_byte(mtd);
	tmp_id = chip->read_byte(mtd);

	if (tmp_manf != *maf_id || tmp_id != dev_id) {
		printf(KERN_INFO "%s: second ID read did not match "
		       "%02x,%02x against %02x,%02x\n", __func__,
		       *maf_id, dev_id, tmp_manf, tmp_id);
		return ERR_PTR(-ENODEV);
	}
	**/

	if (!type)
		type = nand_flash_ids;

	for (; type->name != NULL; type++) {
		if (is_full_id_nand(type)) {
			if (find_full_id_nand(mtd, chip, type, id, &busw))
				goto ident_done;
		} else if (id[1] == type->dev_id) {
			break;
		}
	}

	if (!type)
		return ERR_PTR(-ENODEV);

	if (!mtd->name)
		mtd->name = type->name;

	chip->chipsize = (uint64_t) type->chipsize << 20;

	/* Newer devices have all the information in additional id bytes */
	if (!type->pagesize) {
		int extid;
		/*  The 3rd id byte holds MLC / multichip data  */
		//chip->cellinfo = id[2];//chip->read_byte(mtd);
		chip->bits_per_cell = nand_get_bits_per_cell(id[2]);
		/*  The 4th id byte is the important one  */
		extid = id[3];
		if (id[0] == id[6] && id[1] == id[7] && id[0] == NAND_MFR_SAMSUNG && !nand_is_slc(chip)
		    && id[5] != 0x00) {
			mtd->writesize = 2048 * (1 << (extid & 0x3));

			/* Calc oobsize */
			mtd->oobsize = oobsize_8kp[((extid & 0x40) >> 4) + ((extid >> 2) & 0x03)];

			/* Calc blocksize. Blocksize is multiples of 128KB */
			mtd->erasesize = (1 << (((extid & 0x80) >> 5) + ((extid >> 4) & 0x03))) * (128 * 1024);
			busw = 0;
		} else {
			/* Calc pagesize */
			mtd->writesize = 1024 << (extid & 0x3);
			extid >>= 2;
			/* Calc oobsize */
			mtd->oobsize = (8 << (extid & 0x01)) * (mtd->writesize >> 9);
			extid >>= 2;
			/* Calc blocksize. Blocksize is multiples of 64KiB */
			mtd->erasesize = (64 * 1024) << (extid & 0x03);
			extid >>= 2;
			/* Get buswidth information */
			busw = (extid & 0x01) ? NAND_BUSWIDTH_16 : 0;
		}
	} else {
		/*
		 * Old devices have chip data hardcoded in the device id table
		 */
		mtd->name = type->name;
		mtd->erasesize = type->erasesize;
		mtd->writesize = type->pagesize;
		mtd->oobsize = type->oobsize;
//              mtd->oobsize = mtd->writesize / 32;
		busw = type->options & NAND_BUSWIDTH_16;
	}

 ident_done:
	/* Try to identify manufacturer */
	for (maf_idx = 0; nand_manuf_ids[maf_idx].id != 0x0; maf_idx++) {
		if (nand_manuf_ids[maf_idx].id == *maf_id)
			break;
	}
#if 0
	/*  if Toshiba TH58NVG4S0FTA20 : 2GB, 4k page size, 256kB block size, 232B oob size
	 *  M_ID=0x98, D_ID=0xD3, ID[3]:[1:0] page size 1,2,4,8k;
	 *  ID[3]:[5:4] block size 64kB,128kB,256kB,512kB
	 *  And Strap pin need to set to 4k page with 224B oob size : flash type : 0x7
	 * ECC level : 4bit ECC for each 512Byte is required. So need to define BCH ECC algorithm.
	 * TH58NVG4S0FTA20 - 2G * 8bit
	 * 0x 98 D3 90 26 76 15 02 08
	 *
	 * TC58NVG3S0FTA00 - 1G * 8bit
	 * 0x 98 D3 91 26 76 16 08 00
	 */
	if (id[0] == NAND_MFR_TOSHIBA && id[1] == 0xD3) {
		int extid;
		extid = id[3];
		mtd->writesize = 1024 << (extid & 0x3);
		mtd->oobsize = 32 * (mtd->writesize >> 9);

		extid >>= 4;
		mtd->erasesize = (64 * 1024) << (extid & 0x03);
		if (id[2] == 0x90) {
			type->name = "NAND 2GiB 3,3V 8-bit";
			mtd->name = "NAND 2GiB 3,3V 8-bit";
			type->chipsize = 2048;
		}

		chip->chipsize = type->chipsize << 20;
#ifndef CONFIG_CORTINA_PNAND_ECC_HW_BCH
		printk(KERN_INFO "NAND ECC Level 4bit ECC for each 512Byte is required. \n");
#endif
	}
#endif
 Set_flash_type:
/////// middle debug : oob size not 4 bytes alignment
	if (mtd->oobsize % 8)
		mtd->oobsize = mtd->oobsize - (mtd->oobsize % 8);
///////

	/*
	 * Check, if buswidth is correct. Hardware drivers should set
	 * chip correct !
	 */
	if (busw != (chip->options & NAND_BUSWIDTH_16)) {
		printf(KERN_INFO "NAND device: Manufacturer ID:"
		       " 0x%02x, Chip ID: 0x%02x (%s %s)\n", *maf_id, id[1], nand_manuf_ids[maf_idx].name, mtd->name);
		printf(KERN_WARNING "NAND bus width %d instead %d bit\n",
		       (chip->options & NAND_BUSWIDTH_16) ? 16 : 8, busw ? 16 : 8);
		return ERR_PTR(-EINVAL);
	}

	/* Calculate the address shift from the page size */
	chip->page_shift = ffs(mtd->writesize) - 1;
	/* Convert chipsize to number of pages per chip -1. */
	chip->pagemask = (chip->chipsize >> chip->page_shift) - 1;

	chip->bbt_erase_shift = chip->phys_erase_shift = ffs(mtd->erasesize) - 1;
	if (chip->chipsize & 0xffffffff)
		chip->chip_shift = ffs((unsigned)chip->chipsize) - 1;
	else
		chip->chip_shift = ffs((unsigned)(chip->chipsize >> 32)) + 32 - 1;

	/* Set the bad block position */
	chip->badblockpos = mtd->writesize > 512 ? NAND_LARGE_BADBLOCK_POS : NAND_SMALL_BADBLOCK_POS;

	/* Get chip options, preserve non chip based options */
	//chip->options &= ~NAND_CHIPOPTIONS_MSK;
	if (type != NULL) {
		chip->options |= type->options;
	} else {
		chip->options = NAND_NO_SUBPAGE_WRITE;
	}

	/*
	 * Set chip as a default. Board drivers can override it, if necessary
	 */
	//chip->options |= NAND_NO_AUTOINCR;

	/* Check if chip is a not a samsung device. Do not clear the
	 * options for chips which are not having an extended id.
	 */
	if (*maf_id != NAND_MFR_SAMSUNG && !mtd->writesize)
		chip->options &= ~NAND_SAMSUNG_LP_OPTIONS;

#if 0
	/* Check for AND chips with 4 page planes */
	if (chip->options & NAND_4PAGE_ARRAY)
		chip->erase_cmd = multi_erase_cmd;
	else
		chip->erase_cmd = single_erase_cmd;

	/* Do not replace user supplied command function ! */
	if (mtd->writesize > 512 && chip->cmdfunc == cs752x_nand_command)
		chip->cmdfunc = cs752x_nand_command_lp;
#endif

#ifdef DEBUG
	printf(KERN_INFO "NAND device: Manufacturer ID:"
	       " 0x%02x, Chip ID: 0x%02x (%s %s)\n", *maf_id, dev_id, nand_manuf_ids[maf_idx].name, type->name);
#endif

	flash_type.wrd = read_flash_ctrl_reg(FLASH_TYPE);
	switch (mtd->writesize) {
	case 512:
		flash_type.bf.flashType = 0 | 0x4;
		break;
	case 2048:
		flash_type.bf.flashType = 1 | 0x4;
		break;
	case 4096:
		flash_type.bf.flashType = 2 | 0x4;
		break;
	case 8192:
		flash_type.bf.flashType = 3 | 0x4;
		break;
	}
	flash_type.bf.flashSize = 0;
	write_flash_ctrl_reg(FLASH_TYPE, flash_type.wrd);

	return type;
}

/*
 *	hardware specific access to control-lines
 *	ctrl:
 */
void ca_nand_hwcontrol(struct mtd_info *mtd, int cmd, unsigned int ctrl)
{
	/* Nothing to do */
}

/**
 * ca_nand_default_block_markbad - [DEFAULT] mark a block bad
 * @mtd:	MTD device structure
 * @ofs:	offset from device start
 *
 * This is the default implementation, which can be overridden by
 * a hardware specific driver.
*/
int ca_nand_default_block_markbad(struct mtd_info *mtd, loff_t ofs)
{
	struct nand_chip *chip = mtd->priv;
	uint8_t buf[2] = { 0, 0 };
	struct mtd_oob_ops ops;
	int block, ret;

	/* Get block number */
	block = (int)(ofs >> chip->bbt_erase_shift);
	if (chip->bbt)
		chip->bbt[block >> 2] |= 0x01 << ((block & 0x03) << 1);

	/* We write two bytes, so we don't have to mess with 16 bit
	 * access
	 */
	ca_nand_get_device(chip, mtd, FL_WRITING);
	ofs += mtd->oobsize;
	ops.mode = MTD_OPS_PLACE_OOB;
	ops.len = ops.ooblen = 2;
	ops.datbuf = NULL;
	ops.oobbuf = buf;
	ops.ooboffs = chip->badblockpos & ~0x01;

	ret = ca_nand_do_write_oob(mtd, ofs, &ops);
	ca_nand_release_device(mtd);

	if (!ret)
		mtd->ecc_stats.badblocks++;

#ifdef CONFIG_NAND_HIDE_BAD
	if (!ret) {
		update_factory_bad_block_list(mtd, block << chip->bbt_erase_shift, 1);
	}
#endif
	return ret;
}

/**
 * ca_nand_block_bad - [DEFAULT] Read bad block marker from the chip
 * @mtd:	MTD device structure
 * @ofs:	offset from device start
 * @getchip:	0, if the chip is already selected
 *
 * Check, if the block is bad.
 */
int ca_nand_block_bad(struct mtd_info *mtd, loff_t ofs)
{
#ifdef CONFIG_NAND_HIDE_BAD
	if (!acc_phy_offset) {
		return 0;
	}
#endif

	int page, res = 0;
	struct nand_chip *chip = mtd->priv;

	page = (int)(ofs >> chip->page_shift) & chip->pagemask;

	ca_nand_read_oob_std(mtd, chip, page);

	if (chip->oob_poi[chip->badblockpos] != 0xff)
		return 1;

	return res;
}

#define NOTALIGNED(x)	(x & (chip->subpagesize - 1)) != 0
/**
 * ca_nand_do_write_ops - [Internal] NAND write with ECC
 * @mtd:	MTD device structure
 * @to:		offset to write to
 * @ops:	oob operations description structure
 *
 * NAND write with ECC
 */
int ca_nand_do_write_ops(struct mtd_info *mtd, loff_t to, struct mtd_oob_ops *ops)
{
	int chipnr, realpage, page, column;
	struct nand_chip *chip = mtd->priv;
	uint32_t writelen = ops->len;
	uint8_t *oob = ops->oobbuf;
	uint8_t *buf = ops->datbuf;
	int ret, subpage;
	int oob_required = oob ? 1 : 0;
	uint32_t pa_be4xlat __attribute((unused));

	ops->retlen = 0;
	if (!writelen)
		return 0;

	/* reject writes, which are not page aligned */
	if (NOTALIGNED(to) || NOTALIGNED(ops->len)) {
		printf(KERN_NOTICE "%s: Attempt to write not " "page aligned data\n", __func__);
		return -EINVAL;
	}

	column = to & (mtd->writesize - 1);
	subpage = column || (writelen & (mtd->writesize - 1));

	if (subpage && oob) {
		return -EINVAL;
	}

	chipnr = (int)(to >> chip->chip_shift);
	chip->select_chip(mtd, chipnr);

	/* Check, if it is write protected */
	if (ca_nand_check_wp(mtd))
		return -EIO;

	realpage = (int)(to >> chip->page_shift);
	page = realpage & chip->pagemask;
	pa_be4xlat = page;

	/* Invalidate the page cache, when we write to the cached page */
	if (to <= (chip->pagebuf << chip->page_shift)
	    && (chip->pagebuf << chip->page_shift) < (to + ops->len))
		chip->pagebuf = -1;

	/* If we're not given explicit OOB data, let it be 0xFF */
	if (likely(!oob))
		memset(chip->oob_poi, 0xff, mtd->oobsize);

	while (1) {
#ifdef CONFIG_NAND_HIDE_BAD
        loff_t off_in_out = pa_be4xlat<<chip->page_shift;
        ret = xlat_offset_to_skip_factory_bad(mtd, &off_in_out);
        if(ret == -EINVAL){
            nand_error("%s: offset xlat exceeds chip size\n",__FUNCTION__);
            return ret;
        }
        if((pa_be4xlat<<chip->page_shift) != off_in_out){
            nand_debug("erase xlat: 0x%x => 0x%x\n",(pa_be4xlat<<chip->page_shift),off_in_out);
        }
        page = off_in_out>>chip->page_shift;
        pa_be4xlat++;
#endif

		int bytes = mtd->writesize;
		uint8_t *wbuf = buf;

		/* Partial page write ? */
		if (unlikely(column || writelen < (mtd->writesize - 1))) {
			bytes = min_t(int, bytes - column, (int)writelen);
			chip->pagebuf = -1;
			//chip->ecc.read_page(mtd, chip, chip->buffers->databuf, (page<<chip->page_shift));
			chip->ecc.read_page(mtd, chip, chip->buffers->databuf, 1, page);
			memcpy(&chip->buffers->databuf[column], buf, bytes);
			wbuf = chip->buffers->databuf;
		}

		if (unlikely(oob))
			oob = ca_nand_fill_oob(chip, oob, ops);

		ret = chip->write_page(mtd, chip, column, bytes, wbuf, oob_required, page, (ops->mode == MTD_OPS_RAW));
		if (ret)
			break;

		writelen -= bytes;
		if (!writelen)
			break;

		column = 0;
		buf += bytes;
		realpage++;

		page = realpage & chip->pagemask;
		/* Check, if we cross a chip boundary */
		if (!page) {
			chipnr++;
			chip->select_chip(mtd, -1);
			chip->select_chip(mtd, chipnr);
		}
	}

	ops->retlen = ops->len - writelen;
	if (unlikely(oob))
		ops->oobretlen = ops->ooblen;
	return ret;
}

/**
 * ca_nand_wait - [DEFAULT]  wait until the command is done
 * @mtd:	MTD device structure
 * @chip:	NAND chip structure
 *
 * Wait for command done. This applies to erase and program only
 * Erase can take up to 400ms and program up to 20ms according to
 * general NAND and SmartMedia specs
 */
/* XXX U-BOOT XXX */
int ca_nand_wait(struct mtd_info *mtd, struct nand_chip *this)
{
	unsigned long timeo;
	int state = this->state;

	if (state == FL_ERASING)
		timeo = (CONFIG_SYS_HZ * 400) / 1000;
	else
		timeo = (CONFIG_SYS_HZ * 20) / 1000;

	timeo = (state == FL_ERASING ? 400 : 20);

	/** reset_timer(); **/
	timeo += get_timer(0);

	while (1) {
		if (get_timer(0) > timeo) {
			printf("Timeout!");
			return 0x01;
		}

		if (this->dev_ready) {
			if (this->dev_ready(mtd))
				break;
		} else {
			/** if (this->read_byte(mtd) & NAND_STATUS_READY)**/
			break;
		}
	}

	state = read_flash_ctrl_reg(FLASH_NF_DATA) & 0xff;
	return state;
}

void ca_nand_enable_hwecc(struct mtd_info *mtd, int mode)
{
	return;
}

/**
 * ca_nand_verify_buf - [DEFAULT] Verify chip data against buffer
 * @mtd:	MTD device structure
 * @buf:	buffer containing the data to compare
 * @len:	number of bytes to compare
 *
 * Default verify function for 8bit buswith
 */
#ifdef CONFIG_MTD_NAND_VERIFY_WRITE
int ca_nand_verify_buf(struct mtd_info *mtd, const uint8_t * buf, int len)
{
	int i, page = 0;
	struct nand_chip *chip = mtd->priv;
	//u_char *tmpdata, *tmpoob;tmpdata ,tmpoob
	size_t retlen;
	retlen = 0;

	page = host->page_addr;

	memset(chip->buffers->databuf, 0, mtd->writesize);
	chip->ecc.read_page(mtd, chip, chip->buffers->databuf, page);

	if (len == mtd->writesize) {
		for (i = 0; i < len; i++) {
			if (buf[i] != chip->buffers->databuf[i]) {
				printf
				    ("Data verify error -> page: %x, byte: %x, buf[i]:%x  chip->buffers->databuf[i]:%x \n",
				     host->page_addr, i, buf[i], chip->buffers->databuf[i]);
				return i;
			}
		}
	} else if (len == mtd->oobsize) {
		for (i = 0; i < len; i++) {
			if (buf[i] != chip->oob_poi[i]) {
				printf
				    ("OOB verify error -> page: %x, byte: %x, buf[i]:%x  chip->oob_poi[i]:%x  \n",
				     host->page_addr, i, buf[i], chip->oob_poi[i]);
				return i;
			}
		}
	} else {
		printf(KERN_WARNING "verify length not match 0x%08x\n", len);

		return -1;
	}

	return 0;
}
#endif

/**
 * ca_nand_read_buf - [DEFAULT] read chip data into buffer
 * @mtd:	MTD device structure
 * @buf:	buffer to store date
 * @len:	number of bytes to read
 *
 * Default read function for 8bit buswith
 */
void ca_nand_read_buf(struct mtd_info *mtd, uint8_t * buf, int len)
{
	int page, col;
	struct nand_chip *this = mtd->priv;
	struct ca_nand_host *host = nand_chip->priv;

	if (len <= (mtd->writesize + mtd->oobsize)) {
		page = host->page_addr;
		col = host->col_addr;

		this->ecc.read_page(mtd, this, this->buffers->databuf, 1, page);
		memcpy(buf, &(this->buffers->databuf[col]), len);

	}

}

/**
 * ca_nand_write_buf - [DEFAULT] write buffer to chip
 * @mtd:	MTD device structure
 * @buf:	data buffer
 * @len:	number of bytes to write
 *
 * Default write function for 8bit buswith
 */
void ca_nand_write_buf(struct mtd_info *mtd, const uint8_t * buf, int len)
{
	int i, page = 0, col = 0;
	struct nand_chip *chip = mtd->priv;
	struct ca_nand_host *host = nand_chip->priv;

	/** if(len <= (mtd->writesize+mtd->oobsize))**/
	if (len <= (mtd->writesize + mtd->oobsize)) {

		page = host->page_addr;
		col = host->col_addr;

		chip->ecc.read_page(mtd, chip, chip->buffers->databuf, 1, page);

		for (i = 0; i < len; i++)
			chip->buffers->databuf[col + i] = buf[i];

		chip->ecc.write_page(mtd, chip, chip->buffers->databuf, 1, page);
	}
}

/**
 * ca_nand_read_byte - [DEFAULT] read one byte from the chip
 * @mtd:	MTD device structure
 *
 * Default read function for 8bit buswith
 */
uint8_t ca_nand_read_byte(struct mtd_info *mtd)
{

	struct nand_chip *chip = mtd->priv;
	struct ca_nand_host *host = nand_chip->priv;
	unsigned int data = 0, page = 0, col = 0;

	page = host->page_addr;
	col = host->col_addr;

	ca_nand_read_page_raw(mtd, chip, chip->buffers->databuf, 1, page);
	data = *(chip->buffers->databuf + col);

	return data & 0xff;
}

/**
 * ca_nand_select_chip - [DEFAULT] control CE line
 * @mtd:	MTD device structure
 * @chipnr:	chipnumber to select, -1 for deselect
 *
 * Default select function for 1 chip devices.
 */
void ca_nand_select_chip(struct mtd_info *mtd, int chipnr)
{
	switch (chipnr) {
	case -1:
		CHIP_EN = NFLASH_CHIP0_EN;
		break;
	case 0:
		CHIP_EN = NFLASH_CHIP0_EN;
		break;
	case 1:
		CHIP_EN = NFLASH_CHIP1_EN;
		break;

	default:
		CHIP_EN = NFLASH_CHIP0_EN;
		/** BUG();**/
	}

}

/*
 * Wait for the ready pin, after a command
 * The timeout is catched later.
 */
void ca_nand_wait_ready(struct mtd_info *mtd)
{
	struct nand_chip *chip = mtd->priv;
	u32 timeo = (CONFIG_SYS_HZ * 20) / 1000;

	/** reset_timer();**/
	timeo += get_timer(0);
	/* wait until command is processed or timeout occures */
	while (get_timer(0) < timeo) {
		if (chip->dev_ready)
			if (chip->dev_ready(mtd))
				break;
	}
}

/**
 * ca_nand_command - [DEFAULT] Send command to NAND device
 * @mtd:	MTD device structure
 * @command:	the command to be sent
 * @column:	the column address for this command, -1 if none
 * @page_addr:	the page address for this command, -1 if none
 *
 * Send command to NAND device. This function is used for small page
 * devices (256/512 Bytes per page)
 */
void ca_nand_command(struct mtd_info *mtd, unsigned int command, int column, int page_addr)
{
	register struct nand_chip *chip = mtd->priv;

	int ctrl = NAND_CTRL_CLE | NAND_CTRL_CHANGE;
	struct ca_nand_host *host = nand_chip->priv;

	FLASH_FLASH_ACCESS_START_t tmp_access;

	/*
	 * Write out the command to the device.
	 */
	if (command == NAND_CMD_SEQIN) {
		int readcmd;

		if (column >= mtd->writesize) {
			/* OOB area */
			column -= mtd->writesize;
			readcmd = NAND_CMD_READOOB;
		} else if (column < 256) {
			/* First 256 bytes --> READ0 */
			readcmd = NAND_CMD_READ0;
		} else {
			column -= 256;
			readcmd = NAND_CMD_READ1;
		}
		chip->cmd_ctrl(mtd, readcmd, ctrl);
		ctrl &= ~NAND_CTRL_CHANGE;
	}
	chip->cmd_ctrl(mtd, command, ctrl);

	/*
	 * Address cycle, when necessary
	 */
	ctrl = NAND_CTRL_ALE | NAND_CTRL_CHANGE;
	/* Serially input address */
	if (column != -1) {
		/* Adjust columns for 16 bit buswidth */
		if (chip->options & NAND_BUSWIDTH_16)
			column >>= 1;
		chip->cmd_ctrl(mtd, column, ctrl);
		ctrl &= ~NAND_CTRL_CHANGE;
		host->col_addr = column;
	}
	if (page_addr != -1) {
		chip->cmd_ctrl(mtd, page_addr, ctrl);
		ctrl &= ~NAND_CTRL_CHANGE;
		chip->cmd_ctrl(mtd, page_addr >> 8, ctrl);
		/* One more address cycle for devices > 32MiB */
		if (chip->chipsize > (32 << 20))
			chip->cmd_ctrl(mtd, page_addr >> 16, ctrl);

		host->page_addr = page_addr;
	}
	chip->cmd_ctrl(mtd, NAND_CMD_NONE, NAND_NCE | NAND_CTRL_CHANGE);

	/*
	 * program and erase have their own busy handlers
	 * status and sequential in needs no delay
	 */
	switch (command) {

	case NAND_CMD_PAGEPROG:
	case NAND_CMD_ERASE1:
	case NAND_CMD_ERASE2:
	case NAND_CMD_SEQIN:
	case NAND_CMD_STATUS:
		/*
		 * Write out the command to the device.
		 */
		if (column != -1 || page_addr != -1) {

			/* Serially input address */
			if (column != -1)
				/** FLASH_WRITE_REG(NFLASH_ADDRESS,column);**/
				host->col_addr = column;

			if (page_addr != -1)
				/** FLASH_WRITE_REG(NFLASH_ADDRESS,opcode|(page_addr<<8));**/
				host->page_addr = page_addr;

		}
		return;

	case NAND_CMD_RESET:
		check_flash_ctrl_status();
		udelay(chip->chip_delay);
		write_flash_ctrl_reg(FLASH_NF_ECC_CONTROL, 0x0); /** disable ecc gen**/
		nf_cnt.wrd = 0;
		nf_cnt.bf.nflashRegOobCount = NCNT_EMPTY_OOB;
		nf_cnt.bf.nflashRegDataCount = NCNT_EMPTY_DATA;
		nf_cnt.bf.nflashRegAddrCount = NCNT_EMPTY_ADDR;
		nf_cnt.bf.nflashRegCmdCount = NCNT_CMD_1;
		write_flash_ctrl_reg(FLASH_NF_COUNT, nf_cnt.wrd);

		nf_cmd.wrd = 0;
		nf_cmd.bf.nflashRegCmd0 = NAND_CMD_RESET;
		write_flash_ctrl_reg(FLASH_NF_COMMAND, nf_cmd.wrd); /** write read id command**/
		nf_addr1.wrd = 0;
		write_flash_ctrl_reg(FLASH_NF_ADDRESS_1, nf_addr1.wrd);	/** write address 0x00**/
		nf_addr2.wrd = 0;
		write_flash_ctrl_reg(FLASH_NF_ADDRESS_2, nf_addr2.wrd);	/** write address 0x00**/

		nf_access.wrd = 0;
		nf_access.bf.nflashCeAlt = CHIP_EN;
		/** nf_access.bf.nflashDirWr = ;**/
		nf_access.bf.nflashRegWidth = NFLASH_WiDTH8;

		write_flash_ctrl_reg(FLASH_NF_ACCESS, nf_access.wrd);
		flash_start.wrd = 0;
		flash_start.bf.nflashRegReq = FLASH_GO;
		flash_start.bf.nflashRegCmd = FLASH_WT;
		write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, flash_start.wrd);

		tmp_access.wrd = 0;
		tmp_access.bf.nflashRegReq = 1;
		reg_wait(FLASH_FLASH_ACCESS_START, tmp_access.wrd, 0, 1000);
		flash_start.wrd = read_flash_ctrl_reg(FLASH_FLASH_ACCESS_START);

		udelay(100);

		/* This applies to read commands */
	default:
		/*
		 * If we don't have access to the busy pin, we apply the given
		 * command delay
		 */
		if (!chip->dev_ready) {
			udelay(chip->chip_delay);
			return;
		}
	}
	/* Apply this short delay always to ensure that we do wait tWB in
	 * any case on any machine. */
	ndelay(100);

	ca_nand_wait_ready(mtd);
}

/**
 * ca_nand_command_lp - [DEFAULT] Send command to NAND large page device
 * @mtd:	MTD device structure
 * @command:	the command to be sent
 * @column:	the column address for this command, -1 if none
 * @page_addr:	the page address for this command, -1 if none
 *
 * Send command to NAND device. This is the version for the new large page
 * devices We dont have the separate regions as we have in the small page
 * devices.  We must emulate NAND_CMD_READOOB to keep the code compatible.
 */
void ca_nand_command_lp(struct mtd_info *mtd, unsigned int command, int column, int page_addr)
{
	register struct nand_chip *chip = mtd->priv;
	struct ca_nand_host *host = nand_chip->priv;
	FLASH_FLASH_ACCESS_START_t tmp_access;

	/* Emulate NAND_CMD_READOOB */
	if (command == NAND_CMD_READOOB) {
		column += mtd->writesize;
		command = NAND_CMD_READ0;
	}

	/* Command latch cycle */
	chip->cmd_ctrl(mtd, command & 0xff, NAND_NCE | NAND_CLE | NAND_CTRL_CHANGE);

	if (column != -1 || page_addr != -1) {
		int ctrl = NAND_CTRL_CHANGE | NAND_NCE | NAND_ALE;

		/* Serially input address */
		if (column != -1) {
			/* Adjust columns for 16 bit buswidth */
			if (chip->options & NAND_BUSWIDTH_16)
				column >>= 1;
			chip->cmd_ctrl(mtd, column, ctrl);
			ctrl &= ~NAND_CTRL_CHANGE;
			chip->cmd_ctrl(mtd, column >> 8, ctrl);
		}
		if (page_addr != -1) {
			chip->cmd_ctrl(mtd, page_addr, ctrl);
			chip->cmd_ctrl(mtd, page_addr >> 8, NAND_NCE | NAND_ALE);
			/* One more address cycle for devices > 128MiB */
			if (chip->chipsize > (128 << 20))
				chip->cmd_ctrl(mtd, page_addr >> 16, NAND_NCE | NAND_ALE);
		}
	}
	chip->cmd_ctrl(mtd, NAND_CMD_NONE, NAND_NCE | NAND_CTRL_CHANGE);

	/*
	 * program and erase have their own busy handlers
	 * status, sequential in, and deplete1 need no delay
	 */
	switch (command) {

	case NAND_CMD_CACHEDPROG:
	case NAND_CMD_PAGEPROG:
	case NAND_CMD_ERASE1:
	case NAND_CMD_ERASE2:
	case NAND_CMD_SEQIN:
	case NAND_CMD_RNDIN:
	case NAND_CMD_STATUS:
	case NAND_CMD_DEPLETE1:
		/*
		 * Write out the command to the device.
		 */
		if (column != -1 || page_addr != -1) {

			/* Serially input address */
			if (column != -1)
				/** FLASH_WRITE_REG(NFLASH_ADDRESS,column);**/
				host->col_addr = column;

			if (page_addr != -1)
				/** FLASH_WRITE_REG(NFLASH_ADDRESS,opcode|(page_addr<<8));**/
				host->page_addr = page_addr;

		}
		return;

		/*
		 * read error status commands require only a short delay
		 */
	case NAND_CMD_STATUS_ERROR:
	case NAND_CMD_STATUS_ERROR0:
	case NAND_CMD_STATUS_ERROR1:
	case NAND_CMD_STATUS_ERROR2:
	case NAND_CMD_STATUS_ERROR3:
		udelay(chip->chip_delay);
		return;

	case NAND_CMD_RESET:

		check_flash_ctrl_status();
		udelay(chip->chip_delay);
		write_flash_ctrl_reg(FLASH_NF_ECC_CONTROL, 0x0); /** disable ecc gen**/
		nf_cnt.wrd = 0;
		nf_cnt.bf.nflashRegOobCount = NCNT_EMPTY_OOB;
		nf_cnt.bf.nflashRegDataCount = NCNT_EMPTY_DATA;
		nf_cnt.bf.nflashRegAddrCount = NCNT_EMPTY_ADDR;
		nf_cnt.bf.nflashRegCmdCount = NCNT_CMD_1;
		write_flash_ctrl_reg(FLASH_NF_COUNT, nf_cnt.wrd);

		nf_cmd.wrd = 0;
		nf_cmd.bf.nflashRegCmd0 = NAND_CMD_RESET;
		write_flash_ctrl_reg(FLASH_NF_COMMAND, nf_cmd.wrd); /** write read id command**/
		nf_addr1.wrd = 0;
		write_flash_ctrl_reg(FLASH_NF_ADDRESS_1, nf_addr1.wrd);	/** write address 0x00**/
		nf_addr2.wrd = 0;
		write_flash_ctrl_reg(FLASH_NF_ADDRESS_2, nf_addr2.wrd);	/** write address 0x00**/

		nf_access.wrd = 0;
		nf_access.bf.nflashCeAlt = CHIP_EN;
		nf_access.bf.nflashRegWidth = NFLASH_WiDTH8;

		write_flash_ctrl_reg(FLASH_NF_ACCESS, nf_access.wrd);
		flash_start.wrd = 0;
		flash_start.bf.nflashRegReq = FLASH_GO;
		flash_start.bf.nflashRegCmd = FLASH_WT;
		write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, flash_start.wrd);

		tmp_access.wrd = 0;
		tmp_access.bf.nflashRegReq = 1;
		reg_wait(FLASH_FLASH_ACCESS_START, tmp_access.wrd, 0, 1000);
		flash_start.wrd = read_flash_ctrl_reg(FLASH_FLASH_ACCESS_START);

		udelay(100);
		return;

	case NAND_CMD_RNDOUT:
		/* No ready / busy check necessary */
		chip->cmd_ctrl(mtd, NAND_CMD_RNDOUTSTART, NAND_NCE | NAND_CLE | NAND_CTRL_CHANGE);
		chip->cmd_ctrl(mtd, NAND_CMD_NONE, NAND_NCE | NAND_CTRL_CHANGE);
		return;

	case NAND_CMD_READ0:
		chip->cmd_ctrl(mtd, NAND_CMD_READSTART, NAND_NCE | NAND_CLE | NAND_CTRL_CHANGE);
		chip->cmd_ctrl(mtd, NAND_CMD_NONE, NAND_NCE | NAND_CTRL_CHANGE);

		/* This applies to read commands */
	default:
		/*
		 * If we don't have access to the busy pin, we apply the given
		 * command delay
		 */
		if (!chip->dev_ready) {
			udelay(chip->chip_delay);
			return;
		}
	}

	/* Apply this short delay always to ensure that we do wait tWB in
	 * any case on any machine. */
	ndelay(100);

	ca_nand_wait_ready(mtd);
}

int ca_nand_dev_ready(struct mtd_info *mtd)
{
	int ready;
	FLASH_FLASH_ACCESS_START_t tmp_access;

	check_flash_ctrl_status();

	write_flash_ctrl_reg(FLASH_NF_DATA, 0xffffffff);
 RD_STATUS:
	write_flash_ctrl_reg(FLASH_NF_ECC_CONTROL, 0x0); /** disable ecc gen**/
	nf_cnt.wrd = 0;
	nf_cnt.bf.nflashRegOobCount = NCNT_EMPTY_OOB;
	nf_cnt.bf.nflashRegDataCount = NCNT_DATA_1;
	nf_cnt.bf.nflashRegAddrCount = NCNT_EMPTY_ADDR;
	nf_cnt.bf.nflashRegCmdCount = NCNT_CMD_1;

	write_flash_ctrl_reg(FLASH_NF_COUNT, nf_cnt.wrd);

	nf_cmd.wrd = 0;
	nf_cmd.bf.nflashRegCmd0 = NAND_CMD_STATUS;
	write_flash_ctrl_reg(FLASH_NF_COMMAND, nf_cmd.wrd); /** write read id command**/
	nf_addr1.wrd = 0;
	write_flash_ctrl_reg(FLASH_NF_ADDRESS_1, nf_addr1.wrd);	/** write address 0x00**/
	nf_addr2.wrd = 0;
	write_flash_ctrl_reg(FLASH_NF_ADDRESS_2, nf_addr2.wrd);	/** write address 0x00**/

	nf_access.wrd = 0;
	nf_access.bf.nflashCeAlt = CHIP_EN;
	nf_access.bf.nflashRegWidth = NFLASH_WiDTH8;

	write_flash_ctrl_reg(FLASH_NF_ACCESS, nf_access.wrd);
	flash_start.wrd = 0;
	flash_start.bf.nflashRegReq = FLASH_GO;
	flash_start.bf.nflashRegCmd = FLASH_RD;
	write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, flash_start.wrd);

	flash_start.wrd = read_flash_ctrl_reg(FLASH_FLASH_ACCESS_START);
	tmp_access.wrd = 0;
	tmp_access.bf.nflashRegReq = 1;
	reg_wait(FLASH_FLASH_ACCESS_START, tmp_access.wrd, 0, 1000);

	check_flash_ctrl_status();

	ready = read_flash_ctrl_reg(FLASH_NF_DATA) & 0xff;
	if (ready == 0xff) {
		printf("ca_nand_dev_ready : %x\n", read_flash_ctrl_reg(FLASH_STATUS));
		goto RD_STATUS;
	}

	return (ready & NAND_STATUS_READY);
}

int ca_board_nand_init(struct nand_chip *this)
{
	struct mtd_info *mtd;
	int err = 0;
	FLASH_TYPE_t flash_type;

	/* structures must be linked */
	mtd = &host->mtd;
	mtd->priv = this;
	host->nand = this;

	/* 5 us command delay time */
	this->chip_delay = 20;

	this->priv = host;
	this->dev_ready = ca_nand_dev_ready;
	this->cmdfunc = ca_nand_command;
	this->select_chip = ca_nand_select_chip;
	this->read_byte = ca_nand_read_byte;
	/** this->read_word = ca_nand_read_word;**/
	this->write_buf = ca_nand_write_buf;
	this->read_buf = ca_nand_read_buf;
#ifdef CONFIG_MTD_NAND_VERIFY_WRITE
	this->verify_buf = ca_nand_verify_buf;
#endif

/** #ifdef CONFIG_G3_NAND_HWECC**/
	this->ecc.calculate = ca_nand_calculate_ecc;
	this->ecc.hwctl = ca_nand_enable_hwecc;
	this->ecc.correct = ca_nand_correct_data;
	this->ecc.mode = NAND_ECC_HW;

#if defined(CONFIG_CORTINA_PNAND_ECC_HW_BCH)
	this->ecc.size = BCH_DATA_UNIT;
#if (CONFIG_CORTINA_PNAND_ECC_LEVEL > 4 )
	this->ecc.bytes = (BCH_GF_PARAM_M * 40) / 8;
#elif (CONFIG_CORTINA_PNAND_ECC_LEVEL > 3 )
	this->ecc.bytes = (BCH_GF_PARAM_M * 24) / 8;
#elif  (CONFIG_CORTINA_PNAND_ECC_LEVEL > 2 )
	this->ecc.bytes = (BCH_GF_PARAM_M * 16) / 8;
#else
	this->ecc.bytes = (BCH_GF_PARAM_M * 8) / 8;
#endif
#elif (CONFIG_CORTINA_PNAND_ECC_LEVEL == 1)
	this->ecc.size = 512;
	this->ecc.bytes = 3;
#else
	this->ecc.size = 256;
	this->ecc.bytes = 3;
#endif
/** #endif**/

	/* check, if a user supplied wait function given */
	this->waitfunc = ca_nand_wait;
	this->block_bad = ca_nand_block_bad;
	this->block_markbad = ca_nand_default_block_markbad;

	if (!this->scan_bbt)
		this->scan_bbt = nand_default_bbt;

	flash_type.wrd = read_flash_ctrl_reg(FLASH_TYPE);
	flash_type.bf.flashType = FLASH_NAND_8KP;
	flash_type.bf.flashPin = 1;	/* Assume > 256MB */
	flash_type.bf.flashSize = 2;
	flash_type.bf.flashWidth = 0;
	write_flash_ctrl_reg(FLASH_TYPE, flash_type.wrd);

	this->cmd_ctrl = ca_nand_hwcontrol;
	/* Reset NAND */
	this->cmdfunc(mtd, NAND_CMD_RESET, -1, -1);

	return err;
}

/**
 * ca_nand_scan_tail - [NAND Interface] Scan for the NAND device
 * @mtd:	    MTD device structure
 * @maxchips:	    Number of chips to scan for
 *
 * This is the second phase of the normal nand_scan() function. It
 * fills out all the uninitialized function pointers with the defaults
 * and scans for a bad block table if appropriate.
 */
int ca_nand_scan_tail(struct mtd_info *mtd)
{
	int i, eccStartOffset;
	struct nand_chip *chip = mtd->priv;

	if (!(chip->options & NAND_OWN_BUFFERS))
		chip->buffers = kmalloc(sizeof(*chip->buffers), GFP_KERNEL);
	if (!chip->buffers)
		return -ENOMEM;

	/* Set the internal oob buffer location, just after the page data */
	chip->oob_poi = chip->buffers->databuf + mtd->writesize;

	/*
	 * If no default placement scheme is given, select an appropriate one
	 */
	if (!chip->ecc.layout) {
#if defined(CONFIG_CORTINA_PNAND_ECC_HW_BCH)
		if (mtd->oobsize == 16) {
			chip->ecc.layout = &ca_nand_bch_oob_16;
		} else
#elif (CONFIG_CORTINA_PNAND_ECC_LEVEL == 0) || (CONFIG_CORTINA_PNAND_ECC_LEVEL == 1)
		if (mtd->oobsize == 8) {
			chip->ecc.layout = &ca_nand_oob_8;
		} else if (mtd->oobsize == 16) {
			chip->ecc.layout = &ca_nand_oob_16;
		} else
#endif
		{
			memset(&ca_nand_ecclayout, 0, sizeof(ca_nand_ecclayout));
			ca_nand_ecclayout.eccbytes = mtd->writesize / chip->ecc.size * chip->ecc.bytes;
			if (sizeof(ca_nand_ecclayout.eccpos) < 4 * ca_nand_ecclayout.eccbytes) {
				printf(KERN_WARNING "eccpos memory is less than needed eccbytes");
				return 1;
			}

			if (ca_nand_ecclayout.eccbytes > mtd->oobsize) {
				printf
				    ("BCH%d need %d ECC but OOB size is %d!!\n",
				     (chip->ecc.bytes / BCH_GF_PARAM_M) * 8, ca_nand_ecclayout.eccbytes, mtd->oobsize);
				printf("You have to reduce ECC level!\n");
				return 1;
			}

			memset(ca_nand_ecclayout.eccpos, 0, sizeof(ca_nand_ecclayout.eccpos));
			eccStartOffset = mtd->oobsize - ca_nand_ecclayout.eccbytes;
			for (i = 0; i < ca_nand_ecclayout.eccbytes; ++i) {
				if ((i + eccStartOffset) == chip->badblockpos) {
					continue;
				}
				ca_nand_ecclayout.eccpos[i] = i + eccStartOffset;
			}

			ca_nand_ecclayout.oobfree[0].offset = 2;
			ca_nand_ecclayout.oobfree[0].length =
			    mtd->oobsize - ca_nand_ecclayout.eccbytes - ca_nand_ecclayout.oobfree[0].offset;

#ifdef CONFIG_CORTINA_PNAND_ECC_HW_BCH
			/**  BCH algorithm needs one extra byte to tag erase status**/
			if (ca_nand_ecclayout.oobfree[0].length == 0) {
				printf(KERN_WARNING "eccbytes is less than required");
				return 1;
			};
			ca_nand_ecclayout.oobfree[0].length -= 1;
#endif
			chip->ecc.layout = &ca_nand_ecclayout;
		}
	}

	if (!chip->write_page)
		chip->write_page = ca_nand_write_page;

	/*
	 * check ECC mode, default to software if 3byte/512byte hardware ECC is
	 * selected and we have 256 byte pagesize fallback to software ECC
	 */
	if (!chip->ecc.read_page_raw)
		chip->ecc.read_page_raw = ca_nand_read_page_raw;
	if (!chip->ecc.write_page_raw)
		chip->ecc.write_page_raw = ca_nand_write_page_raw;

	/* Use standard hwecc read page function ? */
	if (!chip->ecc.read_page)
		chip->ecc.read_page = ca_nand_read_page_hwecc;
	if (!chip->ecc.write_page)
		chip->ecc.write_page = ca_nand_write_page_hwecc;
	if (!chip->ecc.read_oob)
		chip->ecc.read_oob = ca_nand_read_oob_std;
	if (!chip->ecc.write_oob)
		chip->ecc.write_oob = ca_nand_write_oob_std;

	/*
	 * The number of bytes available for a client to place data into
	 * the out of band area
	 */
	chip->ecc.layout->oobavail = 0;
	for (i = 0; chip->ecc.layout->oobfree[i].length; i++)
		chip->ecc.layout->oobavail += chip->ecc.layout->oobfree[i].length;
	mtd->oobavail = chip->ecc.layout->oobavail;

	/*
	 * Set the number of read / write steps for one page depending on ECC
	 * mode
	 */
	chip->ecc.steps = mtd->writesize / chip->ecc.size;
	if (chip->ecc.steps * chip->ecc.size != mtd->writesize) {
		printf(KERN_WARNING "Invalid ecc parameters\n");
		BUG();
	}
	chip->ecc.total = chip->ecc.steps * chip->ecc.bytes;

	/*
	 * Allow subpage writes up to ecc.steps. Not possible for MLC
	 * FLASH.
	 */
	if (!(chip->options & NAND_NO_SUBPAGE_WRITE) && nand_is_slc(chip)) {
		switch (chip->ecc.steps) {
		case 2:
			mtd->subpage_sft = 1;
			break;
		case 4:
		case 8:
			mtd->subpage_sft = 2;
			break;
		}
	}
	chip->subpagesize = mtd->writesize >> mtd->subpage_sft;

	/* Initialize state */
	chip->state = FL_READY;

	/* De-select the device */
	chip->select_chip(mtd, -1);

	/* Invalidate the pagebuffer reference */
	chip->pagebuf = -1;

	/* Fill in remaining MTD driver data */
	mtd->type = MTD_NANDFLASH;
	mtd->flags = MTD_CAP_NANDFLASH;
	mtd->_erase = ca_nand_erase;
	mtd->_read = ca_nand_read;
	mtd->_write = ca_nand_write;
	mtd->_read_oob = ca_nand_read_oob;
	mtd->_write_oob = ca_nand_write_oob;
	mtd->_sync = ca_nand_sync;
	mtd->_lock = NULL;
	mtd->_unlock = NULL;
	mtd->_block_isbad = ca_nand_block_isbad;
	mtd->_block_markbad = ca_nand_block_markbad;
	mtd->writebufsize = mtd->writesize;

	/* propagate ecc.layout to mtd_info */
	mtd->ecclayout = chip->ecc.layout;

	/* Check, if we should skip the bad block table scan */
	if (chip->options & NAND_SKIP_BBTSCAN)
		chip->options |= NAND_BBT_SCANNED;

	return 0;
}

/**
 * ca_nand_scan_ident - [NAND Interface] Scan for the NAND device
 * @mtd:	     MTD device structure
 * @maxchips:	     Number of chips to scan for
 *
 * This is the first phase of the normal nand_scan() function. It
 * reads the flash ID and sets up MTD fields accordingly.
 *
 * The mtd->owner field must be set to the module of the caller.
 */
int ca_nand_scan_ident(struct mtd_info *mtd, int maxchips)
{
	int i, busw, nand_maf_id = 0, nand_dev_id = 0;
	struct nand_chip *chip = mtd->priv;
	struct nand_flash_dev *type;
	unsigned char id[8];

	/* Get buswidth to select the correct functions */
	busw = chip->options & NAND_BUSWIDTH_16;

	/* Read the flash type */
	type = ca_nand_get_flash_type(mtd, chip, busw, &nand_maf_id);

	if (IS_ERR(type)) {
#ifndef CONFIG_SYS_NAND_QUIET_TEST
		printf(KERN_WARNING "No NAND device found!!!\n");
#endif
		chip->select_chip(mtd, -1);
		return PTR_ERR(type);
	}

	/* Check for a chip array */
	for (i = 1; i < maxchips; i++) {
		chip->select_chip(mtd, i);
		/* See comment in nand_get_flash_type for reset */
		chip->cmdfunc(mtd, NAND_CMD_RESET, -1, -1);
		/* Send the command for reading device ID */
		ca_nand_read_id(0, id);
		/* Read manufacturer and device IDs */
		if (nand_maf_id != id[0] || nand_dev_id != id[1])
			break;
	}
#ifdef DEBUG
	if (i > 1)
		printf("%d NAND chips detected\n", i);
#endif

	/* Store the number of chips and calc total size for mtd */
	chip->numchips = i;
	mtd->size = i * chip->chipsize;

	return 0;
}

int init_DMA_SSP(void)
{
	int i;
	DMA_SEC_DMA_GLB_DMA_LSO_CTRL_t dma_lso_ctrl;
	DMA_SEC_DMA_GLB_DMA_SSP_RX_CTRL_t dma_rxdma_ctrl;
	DMA_SEC_DMA_GLB_DMA_SSP_TX_CTRL_t dma_txdma_ctrl;
	DMA_SEC_DMA_SSP_Q_RXQ_BASE_DEPTH_t dma_rxq5_base_depth;
	DMA_SEC_DMA_SSP_Q_TXQ_BASE_DEPTH_t dma_txq5_base_depth;
#ifdef CONFIG_TARGET_VENUS
	printf("Configure DMA ordering...  ");
	/* Workaround for DMA-reordering issue */
	writel(8, DMA_AXI_REO_AXI_REO_RD_ORIG_ID);
	writel(9, DMA_AXI_REO_AXI_REO_RD_ORIG_ID + 0x80);
	writel(10, DMA_AXI_REO_AXI_REO_RD_ORIG_ID + 0x100);
	writel(11, DMA_AXI_REO_AXI_REO_RD_ORIG_ID + 0x180);
	writel(12, DMA_AXI_REO_AXI_REO_RD_ORIG_ID + 0x200);
	writel(8, DMA_AXI_REO_AXI_REO_WR_ORIG_ID);
	writel(9, DMA_AXI_REO_AXI_REO_WR_ORIG_ID + 0x80);
	writel(10, DMA_AXI_REO_AXI_REO_WR_ORIG_ID + 0x100);
#endif

	dma_lso_ctrl.wrd = read_dma_global_reg(DMA_SEC_DMA_GLB_DMA_LSO_CTRL);
	dma_lso_ctrl.bf.tx_dma_enable = 1;
	write_dma_global_reg(DMA_SEC_DMA_GLB_DMA_LSO_CTRL, dma_lso_ctrl.wrd);

	dma_rxdma_ctrl.wrd = read_dma_global_reg(DMA_SEC_DMA_GLB_DMA_SSP_RX_CTRL);
	dma_txdma_ctrl.wrd = read_dma_global_reg(DMA_SEC_DMA_GLB_DMA_SSP_TX_CTRL);

	if ((dma_rxdma_ctrl.bf.rx_check_own != 1)
	    && (dma_rxdma_ctrl.bf.rx_dma_enable != 1)) {
		dma_rxdma_ctrl.bf.rx_check_own = 1;
		dma_rxdma_ctrl.bf.rx_dma_enable = 1;
		write_dma_global_reg(DMA_SEC_DMA_GLB_DMA_SSP_RX_CTRL, dma_rxdma_ctrl.wrd);
	}
	if ((dma_txdma_ctrl.bf.tx_check_own != 1)
	    && (dma_txdma_ctrl.bf.tx_dma_enable != 1)) {
		dma_txdma_ctrl.bf.tx_check_own = 1;
		dma_txdma_ctrl.bf.tx_dma_enable = 1;
		write_dma_global_reg(DMA_SEC_DMA_GLB_DMA_SSP_TX_CTRL, dma_txdma_ctrl.wrd);
	}

	i = 16;
	tx_desc = (DMA_SSP_TX_DESC_T *) ((uint64_t)
					 malloc((sizeof(DMA_SSP_TX_DESC_T) * FDMA_DESC_NUM) + i - 1) & ~(i - 1));
	rx_desc = (DMA_SSP_RX_DESC_T *) ((uint64_t)
					 malloc((sizeof(DMA_SSP_RX_DESC_T) * FDMA_DESC_NUM) + i - 1) & ~(i - 1));

	if (!rx_desc || !tx_desc) {
		printk("Buffer allocation for failed!\n");
		if (rx_desc) {
			kfree(rx_desc);
		}

		if (tx_desc) {
			kfree(tx_desc);
		}

		return 0;
	}

	/* set base address and depth */
	dma_rxq5_base_depth.bf.base = (unsigned long)rx_desc >> 4;
	dma_rxq5_base_depth.bf.depth = FDMA_DEPTH;
	write_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_RXQ_BASE_DEPTH, dma_rxq5_base_depth.wrd);

	dma_txq5_base_depth.bf.base = (unsigned long)tx_desc >> 4;
	dma_txq5_base_depth.bf.depth = FDMA_DEPTH;
	write_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_BASE_DEPTH, dma_txq5_base_depth.wrd);

	memset((unsigned char *)tx_desc, 0, (sizeof(DMA_SSP_TX_DESC_T) * FDMA_DESC_NUM));
	memset((unsigned char *)rx_desc, 0, (sizeof(DMA_SSP_RX_DESC_T) * FDMA_DESC_NUM));

	for (i = 0; i < FDMA_DESC_NUM; i++) {
		/* set own by sw */
		tx_desc[i].word1.bf.own = OWN_SW;
		/* enable q5 Scatter-Gather memory copy */
		tx_desc[i].word1.bf.sgm = 0x1;
	}

	return 1;
}

void nand_show_info(struct mtd_info *mtd)
{
//    struct nand_chip *chip = mtd->priv;
    printf("Parallel NAND: %s\n",mtd->name);
#ifdef CONFIG_NAND_HIDE_BAD
        nand_print_bad_block_hidden_list();
#endif
	printf("          OOB: %dB\n",mtd->oobsize);
	printf("         Page: %dB\n",mtd->writesize);
	printf("        Block: %dKB\n",mtd->erasesize/1024);
	printf("    available: 0x%x (%dMB)\n",(uint32_t)(mtd->size), (uint32_t)(mtd->size)/(1024*1024));
//	printf("     Chip: %dMB\n",(uint32_t)chip->chipsize/(1024*1024));
	printf("         Chip: ");
}

int ca_nand_scan(struct mtd_info *mtd, int maxchips)
{
	int ret;

	ret = ca_nand_scan_ident(mtd, maxchips);
	if (!ret)
		ret = ca_nand_scan_tail(mtd);
#ifdef CONFIG_NAND_HIDE_BAD
    scan_parallel_nand_factory_bad_blocks(mtd);
#endif
    nand_show_info(mtd);
	return ret;
}

int ca_nand_init_chip(struct mtd_info *mtd, struct nand_chip *nand)
{
	int maxchips = CONFIG_SYS_NAND_MAX_CHIPS;
	int __attribute__ ((unused)) i = 0;
	int ret = 0;

	if (init_DMA_SSP() == 0) {
		printf("desc alloc error!! \n");
	}

	flash_reset();
	if (maxchips < 1)
		maxchips = 1;
	mtd->priv = nand;

	/** set to NAND data register**/
	nand->IO_ADDR_R = nand->IO_ADDR_W = (void __iomem *)FLASH_NF_DATA;
	if (ca_board_nand_init(nand) == 0) {
		if (ca_nand_scan(mtd, maxchips) == 0) {
			if (!mtd->name)
				mtd->name = (char *)default_nand_name;
#ifndef CONFIG_RELOC_FIXUP_WORKS
			else
				mtd->name += gd->reloc_off;
#endif

#ifdef CONFIG_MTD
			/*
			 * Add MTD device so that we can reference it later
			 * via the mtdcore infrastructure (e.g. ubi).
			 */
			sprintf(dev_name[i], "nand%d", i);
			mtd->name = dev_name[i++];
			add_mtd_device(mtd);
#endif
		} else {
			mtd->name = NULL;
			ret = 1;
		}
	} else {
		mtd->name = NULL;
		mtd->size = 0;
		ret = 1;
	}

	return ret;
}

void cortina_nand_init(void)
{
	int i, ret;
	unsigned int size = 0;

	for (i = 0; i < CONFIG_SYS_MAX_NAND_DEVICE; i++) {
		ret = ca_nand_init_chip(&cortina_nand_info[i], &nand_chip[i]);
		if (ret != 0) {
			memset(&cortina_nand_info[i], 0, sizeof(struct mtd_info));
			return;
		}

		size += cortina_nand_info[i].size / 1024;
		if (nand_curr_device == -1)
			nand_curr_device = i;
	}

	nand_register(0, &cortina_nand_info[0]);
#ifdef CONFIG_SYS_NAND_SELECT_DEVICE
	/*
	 * Select the chip in the board/cpu specific driver
	 */
	board_nand_select_device(cortina_nand_info[nand_curr_device].priv, nand_curr_device);
#endif
}

void board_nand_init(void)
{
	cortina_nand_init();
}
