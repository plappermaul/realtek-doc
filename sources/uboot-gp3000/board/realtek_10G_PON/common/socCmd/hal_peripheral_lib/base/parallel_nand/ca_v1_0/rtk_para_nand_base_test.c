/*
 * Copyright (c) 2016-2017 Cortina Access. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 * Modification History:
 *    #000 2016-01-01 Cortina_Owner   create file
 *    #001 2016-11-14 Flysky_Hung     proting to RTL8198F
 *    #002
 *    #003
 *    #004
 ******************************************************************************
 */
 /* Cortina define:
  * Parallel nand flash -> nflash
  * Parallel nor flash  -> pflash
 */
#include <common.h>
#include <nand.h>

#include <malloc.h>
#include <watchdog.h>
#include <linux/err.h>
//#include <linux/mtd/compat.h>
//#include <linux/mtd/mtd.h>
//#include <linux/mtd/nand.h>
//#include <linux/mtd/nand_ecc.h>

#ifdef CONFIG_MTD_PARTITIONS
#include <linux/mtd/partitions.h>
#endif

#include <asm/io.h>
#include <asm/errno.h>
#include <g3lite_registers.h>
#include <peripheral.h>

#include "pnand.h"

#define DRIVER_NAME "pnand"

#define ENOTSUPP	524	/* Operation is not supported */

#ifndef CONFIG_SYS_NAND_BASE_LIST
#define CONFIG_SYS_NAND_BASE_LIST { CONFIG_SYS_NAND_BASE }
#endif

DECLARE_GLOBAL_DATA_PTR;

#define OWN_DMA	0
#define OWN_SW	1


//static int nand_curr_device = -1;
//nand_info_t nand_info[CONFIG_SYS_MAX_NAND_DEVICE];

static struct nand_chip nand_chip[CONFIG_SYS_MAX_NAND_DEVICE];
static ulong base_address[CONFIG_SYS_MAX_NAND_DEVICE] = CONFIG_SYS_NAND_BASE_LIST;

static const char default_nand_name[] = "pnand";
static __attribute__((unused)) char dev_name[CONFIG_SYS_MAX_NAND_DEVICE][8];

/* OOB placement block for use with hardware ecc generation */
/*#ifdef CONFIG_PNAND_HWECC*/
static struct nand_ecclayout pnand_ecclayout;

#ifndef CONFIG_SYS_NAND_RESET_CNT
#define CONFIG_SYS_NAND_RESET_CNT 200000
#endif

//#if defined(CONFIG_CS752X_NAND_ECC_HW_BCH_8) || defined(CONFIG_CS752X_NAND_ECC_HW_BCH_16)
//  #define CONFIG_PNAND_ECC_HW_BCH
//#endif

#if defined(CONFIG_CS752X_NAND_ECC_HW_HAMMING_256) || defined(CONFIG_CS752X_NAND_ECC_HW_HAMMING_512)
  #define CONFIG_CS75XX_NAND_ECC_HW_HAMMING
#endif

/* Define default oob placement schemes for large and small page devices */
#ifdef	CONFIG_PNAND_ECC_HW_BCH

static struct nand_ecclayout pnand_bch_oob_16 = {
	.eccbytes = 13,
	.eccpos = {0, 1, 2, 3, 6, 7, 8, 9, 10, 11, 12, 13, 14},
	.oobfree = {
		{.offset = 15,
		 /* . length = 1}}  resever 1 for erase tags: 1 - 1 = 0*/
		 . length = 0}} /* resever 1 for erase tags: 1 - 1 = 0 */
};

#else

static struct nand_ecclayout pnand_oob_8 = {
	.eccbytes = 3,
	.eccpos = {0, 1, 2},
	.oobfree = {
		{.offset = 3,
		 .length = 2},
		{.offset = 6,
		 .length = 2}}
};

static struct nand_ecclayout pnand_oob_16 = {
	.eccbytes = 6,
	.eccpos = {0, 1, 2, 3, 6, 7},
	.oobfree = {
		{.offset = 8,
		 . length = 8}}
};

#endif


struct pnand_host {
	struct mtd_info		mtd;
	struct nand_chip	*nand;
	void __iomem		*io_base;
	struct device		*dev;
	unsigned int		col_addr;
	unsigned int		page_addr;
};


static struct pnand_host pnand_host;
static struct pnand_host *host = &pnand_host;


static unsigned int chip_en;
static unsigned int *pread, *pwrite;
GLOBAL_STRAP_t					global_strap;
FLASH_TYPE_t					flash_type;
FLASH_STATUS_t					flash_status;
FLASH_FLASH_ACCESS_START_t 			access_start;
FLASH_FLASH_ACCESS_START_t 			tmp_access;
FLASH_FLASH_INTERRUPT_t				flash_int_sts;
FLASH_FLASH_MASK_t				flash_int_mask;

FLASH_NF_ACCESS_t				nf_access;
FLASH_NF_COUNT_t				nf_count;
FLASH_NF_COMMAND_t				nf_command;
FLASH_NF_ADDRESS_1_t				nf_addr1;
FLASH_NF_ADDRESS_2_t				nf_addr2;
FLASH_NF_DATA_t					nf_data;

//FLASH_NF_FIFO_CONTROL_t			nf_fifo_ctl;
//FLASH_NF_FIFO_STATUS_t			nf_fifo_sts;
//FLASH_NF_FIFO_ADDRESS_t			nf_fifo_addr;
//FLASH_NF_FIFO_DATA_t				nf_fifo_data;

FLASH_NF_ECC_STATUS_t				nf_ecc_sts;
FLASH_NF_ECC_CONTROL_t				nf_ecc_ctl;
FLASH_NF_ECC_OOB_t				nf_ecc_oob;
FLASH_NF_ECC_GEN0_t				nf_ecc_gen0;
FLASH_NF_ECC_GEN1_t				nf_ecc_gen1;
FLASH_NF_ECC_GEN2_t				nf_ecc_gen2;
FLASH_NF_ECC_GEN3_t				nf_ecc_gen3;
FLASH_NF_ECC_GEN4_t				nf_ecc_gen4;
FLASH_NF_ECC_GEN5_t				nf_ecc_gen5;
FLASH_NF_ECC_GEN6_t				nf_ecc_gen6;
FLASH_NF_ECC_GEN7_t				nf_ecc_gen7;
FLASH_NF_ECC_GEN8_t				nf_ecc_gen8;
FLASH_NF_ECC_GEN9_t				nf_ecc_gen9;
FLASH_NF_ECC_GEN10_t				nf_ecc_gen10;
FLASH_NF_ECC_GEN11_t				nf_ecc_gen11;
FLASH_NF_ECC_GEN12_t				nf_ecc_gen12;
FLASH_NF_ECC_GEN13_t				nf_ecc_gen13;
FLASH_NF_ECC_GEN14_t				nf_ecc_gen14;
FLASH_NF_ECC_GEN15_t				nf_ecc_gen15;
FLASH_NF_ECC_RESET_t				nf_ecc_reset;

FLASH_NF_BCH_STATUS_t				nf_bch_sts;
FLASH_NF_BCH_ERROR_LOC01_t			nf_bch_err_loc01;
FLASH_NF_BCH_ERROR_LOC23_t			nf_bch_err_loc23;
FLASH_NF_BCH_ERROR_LOC45_t			nf_bch_err_loc45;
FLASH_NF_BCH_ERROR_LOC67_t			nf_bch_err_loc67;
FLASH_NF_BCH_CONTROL_t				nf_bch_ctrl;
FLASH_NF_BCH_OOB0_t				nf_bch_oob0;
FLASH_NF_BCH_OOB1_t				nf_bch_oob1;
FLASH_NF_BCH_OOB2_t				nf_bch_oob2;
FLASH_NF_BCH_OOB3_t				nf_bch_oob3;
FLASH_NF_BCH_OOB4_t				nf_bch_oob4;

/*DMA regs*/
/*#ifndef NAND_DIRECT_ACCESS*/
DMA_SEC_DMA_GLB_DMA_SSP_RX_CTRL_t 		dma_rxdma_ctrl;
DMA_SEC_DMA_GLB_DMA_SSP_TX_CTRL_t		dma_txdma_ctrl;
DMA_SEC_DMA_SSP_Q_TXQ_CONTROL_t 		dma_txq5_ctrl;
//DMA_DMA_SSP_RXQ5_PKTCNT_READ_t		dma_rxq5_pktcnt_read;
//DMA_DMA_SSP_TXQ5_PKTCNT_READ_t		dma_txq5_pktcnt_read;
DMA_SEC_DMA_SSP_Q_RXQ_BASE_DEPTH_t		dma_rxq5_base_depth;
//DMA_DMA_SSP_RXQ5_WPTR_t			dma_rxq5_wptr;
DMA_SEC_DMA_SSP_Q_RXQ_RPTR_t			dma_rxq5_rptr;
DMA_SEC_DMA_SSP_Q_TXQ_BASE_DEPTH_t		dma_txq5_base_depth;
DMA_SEC_DMA_SSP_Q_TXQ_WPTR_t			dma_txq5_wptr;
//DMA_DMA_SSP_TXQ5_RPTR_t			dma_txq5_rptr;
//DMA_DMA_SSP_RXQ5_FULL_THRESHOLD_t	 	dma_rxq5_threshold;
//DMA_DMA_SSP_RXQ5_PKTCNT_t			dma_rxq5_pktcnt;
//DMA_DMA_SSP_RXQ5_FULL_DROP_PKTCNT_t  	 	dma_rxq5_drop_pktcnt;
//DMA_DMA_SSP_TXQ5_PKTCNT_t			dma_txq5_pktcnt;
//DMA_DMA_SSP_DMA_SSP_INTERRUPT_0_t	 	dma_ssp0_intsts;
//DMA_DMA_SSP_DMA_SSP_INTENABLE_0_t 		dma_ssp0_int_enable;
//DMA_DMA_SSP_DMA_SSP_INTERRUPT_1_t 		dma_ssp1_intsts;
//DMA_DMA_SSP_DMA_SSP_INTENABLE_1_t 		dma_ssp1_int_enable;
//DMA_DMA_SSP_DESC_INTERRUPT_t	 		dma_ssp_desc_intsts;
//DMA_DMA_SSP_DESC_INTENABLE_t	 		dma_ssp_desc_int_enable;
DMA_SEC_DMA_SSP_Q_RXQ_COAL_INTERRUPT_t		dma_ssp_rxq5_intsts;
//DMA_DMA_SSP_RXQ5_INTENABLE_t	 		dma_ssp_rxq5_int_enable;
DMA_SEC_DMA_SSP_Q_TXQ_COAL_INTERRUPT_t		dma_ssp_txq5_intsts;
//DMA_DMA_SSP_TXQ5_INTENABLE_t	 		dma_ssp_txq5_int_enable;

DMA_SSP_TX_DESC_T *tx_desc;
DMA_SSP_RX_DESC_T *rx_desc;
/*#endif*/

/* Chip select */
#define NF_ACCESS_CE_ALT		0

 int pnand_block_checkbad(struct mtd_info *mtd, loff_t ofs, int getchip, int allowbbt);
 int pnand_get_device(struct nand_chip *chip, struct mtd_info *mtd, int new_state);
 uint8_t *pnand_fill_oob(struct nand_chip *chip, uint8_t *oob, struct mtd_oob_ops *ops);
 int pnand_do_write_oob(struct mtd_info *mtd, loff_t to, struct mtd_oob_ops *ops);
 int pnand_do_write_ops(struct mtd_info *mtd, loff_t to, struct mtd_oob_ops *ops);
 int pnand_do_read_ops(struct mtd_info *mtd, loff_t from, struct mtd_oob_ops *ops);
int pnand_erase_nand(struct mtd_info *mtd, struct erase_info *instr, int allowbbt);
 int pnand_check_wp (struct mtd_info *mtd);
 unsigned int reg_wait(unsigned int regaddr, unsigned int mask, unsigned int val, int timeout);
 void pnand_wait_ready(struct mtd_info *mtd);
void pnand_command(struct mtd_info *mtd, unsigned int command,
			 int column, int page_addr);
void pnand_command_lp(struct mtd_info *mtd, unsigned int command,
			 int column, int page_addr);

extern void sanitize_string(char *s, size_t len);
extern  u16 onfi_crc16(u16 crc, u8 const *p, size_t len);
extern  int nand_flash_detect_ext_param_page(struct mtd_info *mtd,
		struct nand_chip *chip, struct nand_onfi_params *p);
extern  void nand_onfi_detect_micron(struct nand_chip *chip,
		struct nand_onfi_params *p);

#define NAND_MAX_CHIPS CONFIG_SYS_NAND_MAX_CHIPS
#define BBT_PAGE_MASK	0xffffff3f
#define	SZ_32M 0x8000
#define	SZ_128M 0x20000

static int pnand_ecc_check = 0;
static u32 pnand_blk_size = 0x20000;		// 64 page = 128k byte
static u8 markp=0;
static u8 local_oob_buff[640];
static u8 local_ecc_code[560];
static int pnand_dma_access = 0;

#define BL1_RW_BASE			0x220000
#define FLASH_BASE			0xE0000000
/*8KB temporary buffer before BL1_RW_BASE */
unsigned char *data_buff=(unsigned char*)BL1_RW_BASE - 0x2000;

 unsigned int read_flash_ctrl_reg(unsigned int ofs)
{
	volatile unsigned int *base;

	base = (unsigned int *)( ofs);
	return (*base);
}

 void write_flash_ctrl_reg(unsigned int ofs,unsigned int data)
{
	unsigned int *base;

	base = (unsigned int *)( ofs);
	*base = data;
}

 unsigned int read_dma_ctrl_reg(unsigned int ofs)
{
	unsigned int *base;

	base = (unsigned int *)( ofs);
	return (*base);
}

 void write_dma_ctrl_reg(unsigned int ofs,unsigned int data)
{

	unsigned int *base;

	base = (unsigned int *)( ofs);
	*base = data;
}

uint32_t flash_reset(void)
{
	nf_access.wrd = 0;
	nf_access.bf.autoReset = 1;
	write_flash_ctrl_reg(FLASH_NF_ACCESS, nf_access.wrd);

	nf_ecc_reset.wrd = 0;
	nf_ecc_reset.bf.nflash_reset = 1;
	write_flash_ctrl_reg(FLASH_NF_ECC_RESET, nf_ecc_reset.wrd);

	udelay(1);

	return 0;
}

/**
 * pnand_get_device - [GENERIC] Get chip for selected access
 * @chip:	the nand chip descriptor
 * @mtd:	MTD device structure
 * @new_state:	the state which is requested
 *
 * Get the device and lock it for exclusive access
 */
#if 0
static int
pnand_get_device(struct nand_chip *chip, struct mtd_info *mtd, int new_state)
{


retry:


	/* Hardware controller shared among independent devices */
	if (!chip->controller->active)
		chip->controller->active = chip;

	if (chip->controller->active == chip && chip->state == FL_READY) {
		chip->state = new_state;

		return 0;
	}
	if (new_state == FL_PM_SUSPENDED) {

		return (chip->state == FL_PM_SUSPENDED) ? 0 : -EAGAIN;
	}

	goto retry;
}

#else
 int pnand_get_device (struct nand_chip *this, struct mtd_info *mtd, int new_state)
{
	this->state = new_state;
	return 0;
}
#endif

int
pnand_mem_cmp(u32 dst1, u32 src1, u32 length1)
{
	u32    i, length;
	u32    dst, src;
	u32    dst_value, src_value;
	u8     error;

	printf("\ndst1=0x%x,src1=0x%x,length1=0x%x\n", dst1, src1, length1);

	// dst = dst1+0xbd000000;
	dst = dst1;
	src = src1;
	length = length1;

	// printf("\ndst=%x,src=%x,length=%x\n",dst,src,length);
	error = 0;
	for (i = 0; i < length; i += 4) {
		dst_value = *(volatile unsigned int *) (dst + i);
		src_value = *(volatile unsigned int *) (src + i);
		if (dst_value != src_value) {
			printf("\n->%dth data(dst=%x ,src=%x) error\n", i, dst_value,
				src_value);
			error = 1;
			// return 1;
		}
	}
	if (!error) {
		printf("\n->Compare OK\n");
	}
	return error;
}

/**
 * pnand_do_write_oob - [MTD Interface] NAND write out-of-band
 * @mtd:	MTD device structure
 * @to:		offset to write to
 * @ops:	oob operation description structure
 *
 * NAND write out-of-band
 */
 int pnand_do_write_oob(struct mtd_info *mtd, loff_t to,
			     struct mtd_oob_ops *ops)
{
	int chipnr, page, status, len;
#if defined(CONFIG_PNAND_TEST_DRIVER)
	struct nand_chip *chip = mtd->priv;
#else
	struct nand_chip *chip = mtd_to_nand(mtd);
#endif

#ifdef CS75XX_NAND_DEBUG
	printf("%s: to = 0x%08x, len = %i\n",
			 __func__, (unsigned int)to, (int)ops->ooblen);
#endif

	if (ops->mode == MTD_OPS_AUTO_OOB)
		len = chip->ecc.layout->oobavail;
	else
		len= mtd->oobsize;


	/* Do not allow write past end of page */
	if ((ops->ooboffs + ops->ooblen) > len) {
		printf("%s: Attempt to write "
				"past end of page\n", __func__);
		return -EINVAL;
	}

	if (unlikely(ops->ooboffs >= len)) {
		printf("%s: Attempt to start "
				"write outside oob\n", __func__);
		return -EINVAL;
	}

	/* Do not allow reads past end of device */
	if (unlikely(to >= mtd->size ||
		     ops->ooboffs + ops->ooblen >
			((mtd->size >> chip->page_shift) -
			 (to >> chip->page_shift)) * len)) {
		printf("%s: Attempt write beyond "
				"end of device\n", __func__);
		return -EINVAL;
	}

	chipnr = (int)(to >> chip->chip_shift);
	chip->select_chip(mtd, chipnr);

	/* Shift to get page */
	page = (int)(to >> chip->page_shift);

	/*
	 * Reset the chip. Some chips (like the Toshiba TC5832DC found in one
	 * of my DiskOnChip 2000 test units) will clear the whole data page too
	 * if we don't do this. I have no clue why, but I seem to have 'fixed'
	 * it in the doc2000 driver in August 1999.  dwmw2.
	 */
	chip->cmdfunc(mtd, NAND_CMD_RESET, -1, -1);

	/* Check, if it is write protected */
	if (pnand_check_wp(mtd))
		return -EROFS;

	/* Invalidate the page cache, if we write to the cached page */
	if (page == chip->pagebuf)
		chip->pagebuf = -1;

	memset(chip->oob_poi, 0xff, mtd->oobsize);
	pnand_fill_oob(chip, ops->oobbuf, ops);
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

	for(i=timeout;i>0;i--)
	{
		tmp = read_flash_ctrl_reg(regaddr);
		if((tmp&mask) == val)
			return 0;//TRUE;
		udelay(5);
	}

	printf("reg_wait error !!  \n");

	return 1;//FALSE;
}



/*#ifdef CONFIG_PNAND_HWECC*/

 int pnand_correct_data(struct mtd_info *mtd, u_char *dat,
				 u_char *read_ecc, u_char *calc_ecc)
{
#if 0
#if defined(CONFIG_PNAND_TEST_DRIVER)
	struct nand_chip *nand_chip = mtd->priv;
	struct g3_nand_host *host = nand_chip->priv;
#else
	struct nand_chip *nand_chip = mtd_to_nand(mtd);
	struct pnand_host *host = mtd_to_nand(nand_chip);
#endif

	/*
	 * 1-Bit errors are automatically corrected in HW.  No need for
	 * additional correction.  2-Bit errors cannot be corrected by
	 * HW ECC, so we need to return failure
	 */
	unsigned short_t ecc_status = readw(&host->regs->nfc_ecc_status_result);

	if (((ecc_status & 0x3) == 2) || ((ecc_status >> 2) == 2)) {
		printf(
		      "CS75XX_NAND: HWECC uncorrectable 2-bit ECC error\n");
		return -1;
	}
#endif
	return 0;
}

 int pnand_calculate_ecc(struct mtd_info *mtd, const u_char *dat,
				  u_char *ecc_code)
{
	return 0;
}

 void check_flash_ctrl_status(void)
{
	int rty=0;

	flash_status.wrd = read_flash_ctrl_reg(FLASH_STATUS);
	while(flash_status.bf.nState)
	{
	     flash_status.wrd = read_flash_ctrl_reg(FLASH_STATUS);
	     udelay(5);
	     rty++;
	     if(rty > 50000)
	     {
		printk("FLASH_STATUS ERROR: %x\n",flash_status.wrd);
		return;
	     }
	}

}


/**
 * pnand_release_device - [GENERIC] release chip
 * @mtd:	MTD device structure
 *
 * Deselect, release chip lock and wake up anyone waiting on the device
 */
/* XXX U-BOOT XXX */
 void pnand_release_device (struct mtd_info *mtd)
{
#if defined(CONFIG_PNAND_TEST_DRIVER)
	struct nand_chip *this = mtd->priv;
#else
	struct nand_chip *this = mtd_to_nand(mtd);
#endif
	this->select_chip(mtd, -1);	/* De-select the NAND device */
}

void pnand_read_id(unsigned char *id)
{
	unsigned int opcode, i;
	const unsigned int extid=8;
	check_flash_ctrl_status();

	write_flash_ctrl_reg(FLASH_NF_ECC_CONTROL, 0x0);

	flash_type.wrd = read_flash_ctrl_reg(FLASH_TYPE);

	/* read maker code */
	nf_access.wrd = 0;
	nf_access.bf.nflashRegWidth = NF_ACCESS_WiDTH8;
	nf_access.bf.nflashCeAlt = NF_ACCESS_CE_ALT;
	write_flash_ctrl_reg(FLASH_NF_ACCESS, nf_access.wrd);

	/*need to check extid byte counts*/
	nf_count.wrd = 0;
	nf_count.bf.nflashRegCmdCount = NF_CNT_CMD_1;
	nf_count.bf.nflashRegAddrCount = NF_CNT_ADDR_1;
	nf_count.bf.nflashRegDataCount = NF_CNT_DATA_4;
	nf_count.bf.nflashRegOobCount = NF_CNT_EMPTY_OOB;
	write_flash_ctrl_reg(FLASH_NF_COUNT, nf_count.wrd);

	nf_command.wrd = 0;
	nf_command.bf.nflashRegCmd0 = NAND_CMD_READID;
	write_flash_ctrl_reg(FLASH_NF_COMMAND, nf_command.wrd);
	nf_addr1.wrd = 0;
	write_flash_ctrl_reg(FLASH_NF_ADDRESS_1, nf_addr1.wrd);
	nf_addr2.wrd = 0;
	write_flash_ctrl_reg(FLASH_NF_ADDRESS_2, nf_addr2.wrd);

	for( i=0; i < extid; i++) {

		access_start.wrd = 0;
		access_start.bf.nflashRegReq = FLASH_GO;
		access_start.bf.nflashRegCmd = FLASH_RD;
		write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, access_start.wrd);


		access_start.wrd=read_flash_ctrl_reg(FLASH_FLASH_ACCESS_START);
		tmp_access.wrd = 0;
		tmp_access.bf.nflashRegReq = 1;
		reg_wait(FLASH_FLASH_ACCESS_START, tmp_access.wrd, 0, 2000);

		opcode=read_flash_ctrl_reg(FLASH_NF_DATA);
		id[i] = (unsigned char)(( opcode >> ((i<<3) % 32 ) )& 0xff);
	}
	printf("Parallel_NAND ID:0x%2X%2X%2X%2X\n", id[0], id[1], id[2], id[3]);

	nf_ecc_reset.wrd = 0;
	nf_ecc_reset.bf.eccClear = ECC_CLR;
	nf_ecc_reset.bf.fifoClear = FIFO_CLR;
	nf_ecc_reset.bf.nflash_reset = NF_RESET;
	write_flash_ctrl_reg(FLASH_NF_ECC_RESET, nf_ecc_reset.wrd);
}

void pnand_onfi_id(unsigned char *id)
{
	unsigned int opcode, i;

	check_flash_ctrl_status();

	write_flash_ctrl_reg(FLASH_NF_ECC_CONTROL, 0x0);

	flash_type.wrd = read_flash_ctrl_reg(FLASH_TYPE);

	/*need to check extid byte counts*/
	nf_count.wrd = 0;

	nf_count.bf.nflashRegOobCount = NF_CNT_EMPTY_OOB;
	nf_count.bf.nflashRegDataCount = NF_CNT_DATA_4;
	nf_count.bf.nflashRegAddrCount = NF_CNT_ADDR_1;
	nf_count.bf.nflashRegCmdCount = NF_CNT_CMD_1;

	write_flash_ctrl_reg(FLASH_NF_COUNT, nf_count.wrd);

	nf_command.wrd = 0;
	nf_command.bf.nflashRegCmd0 = NAND_CMD_READID;
	write_flash_ctrl_reg(FLASH_NF_COMMAND, nf_command.wrd);
	nf_addr1.wrd = 0x20;	/* subcommand 0x20 to get ONFI signature */
	write_flash_ctrl_reg(FLASH_NF_ADDRESS_1, nf_addr1.wrd);
	nf_addr2.wrd = 0;
	write_flash_ctrl_reg(FLASH_NF_ADDRESS_2, nf_addr2.wrd);

	/* read maker code */
	nf_access.wrd = 0;
	nf_access.bf.nflashCeAlt = NF_ACCESS_CE_ALT;

	nf_access.bf.nflashRegWidth = NF_ACCESS_WiDTH8;
	write_flash_ctrl_reg(FLASH_NF_ACCESS, nf_access.wrd);


	for( i=0; i < 4; i++) {

		access_start.wrd = 0;
		access_start.bf.nflashRegReq = FLASH_GO;
		access_start.bf.nflashRegCmd = FLASH_RD;
		write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, access_start.wrd);


		access_start.wrd=read_flash_ctrl_reg(FLASH_FLASH_ACCESS_START);
		tmp_access.wrd = 0;
		tmp_access.bf.nflashRegReq = 1;
		reg_wait(FLASH_FLASH_ACCESS_START, tmp_access.wrd, 0, 2000);


		opcode=read_flash_ctrl_reg(FLASH_NF_DATA);
		id[i] = (unsigned char)(( opcode >> ((i<<3) % 32 ) )& 0xff);
	}


	nf_ecc_reset.wrd = 0;
	nf_ecc_reset.bf.eccClear = ECC_CLR;
	nf_ecc_reset.bf.fifoClear = FIFO_CLR;
	nf_ecc_reset.bf.nflash_reset = NF_RESET;
	write_flash_ctrl_reg(FLASH_NF_ECC_RESET, nf_ecc_reset.wrd);
}

int pnand_flash_detect_onfi(struct mtd_info *mtd, struct nand_chip *chip,
					int *busw)
{
	unsigned int opcode;
	int i, j;
	int val;
	struct nand_onfi_params *p = &chip->onfi_params;
	unsigned char onfi_sig[4], *onfi_param=(unsigned char*)p;

	pnand_onfi_id(onfi_sig);
	if((onfi_sig[0]!='O') || (onfi_sig[1]!='N') ||
		(onfi_sig[2]!='F') ||(onfi_sig[3]!='I'))
		return 0;

	check_flash_ctrl_status();

	write_flash_ctrl_reg(FLASH_NF_ECC_CONTROL, 0x0);

	flash_type.wrd = read_flash_ctrl_reg(FLASH_TYPE);

	/*need to check extid byte counts*/
	nf_count.wrd = 0;

	nf_count.bf.nflashRegOobCount = NF_CNT_EMPTY_OOB;
	nf_count.bf.nflashRegDataCount = sizeof(*p) -1;
	nf_count.bf.nflashRegAddrCount = NF_CNT_ADDR_1;
	nf_count.bf.nflashRegCmdCount = NF_CNT_CMD_1;

	write_flash_ctrl_reg(FLASH_NF_COUNT, nf_count.wrd);

	nf_command.wrd = 0;
	nf_command.bf.nflashRegCmd0 = NAND_CMD_PARAM;
	write_flash_ctrl_reg(FLASH_NF_COMMAND, nf_command.wrd);
	nf_addr1.wrd = 0x0;
	write_flash_ctrl_reg(FLASH_NF_ADDRESS_1, nf_addr1.wrd);
	nf_addr2.wrd = 0;
	write_flash_ctrl_reg(FLASH_NF_ADDRESS_2, nf_addr2.wrd);

	/* read maker code */
	nf_access.wrd = 0;
	nf_access.bf.nflashCeAlt = NF_ACCESS_CE_ALT;

	nf_access.bf.nflashRegWidth = NF_ACCESS_WiDTH8;
	write_flash_ctrl_reg(FLASH_NF_ACCESS, nf_access.wrd);


	for( i=0; i < sizeof(*p); i++) {

		access_start.wrd = 0;
		access_start.bf.nflashRegReq = FLASH_GO;
		access_start.bf.nflashRegCmd = FLASH_RD;
		write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, access_start.wrd);


		access_start.wrd=read_flash_ctrl_reg(FLASH_FLASH_ACCESS_START);
		tmp_access.wrd = 0;
		tmp_access.bf.nflashRegReq = 1;
		reg_wait(FLASH_FLASH_ACCESS_START, tmp_access.wrd, 0, 2000);


		opcode=read_flash_ctrl_reg(FLASH_NF_DATA);
		onfi_param[i] = (unsigned char)(( opcode >> ((i<<3) % 32 ) )& 0xff);
	}

	if (onfi_crc16(ONFI_CRC_BASE, (uint8_t *)p, 254) !=
				le16_to_cpu(p->crc))
		return 0;

	nf_ecc_reset.wrd = 0;
	nf_ecc_reset.bf.eccClear = ECC_CLR;
	nf_ecc_reset.bf.fifoClear = FIFO_CLR;
	nf_ecc_reset.bf.nflash_reset = NF_RESET;
	write_flash_ctrl_reg(FLASH_NF_ECC_RESET, nf_ecc_reset.wrd);

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
#if defined(CONFIG_PNAND_TEST_DRIVER)
	chip->chipsize *= (uint32_t)mtd->erasesize * p->lun_count;
#else
	chip->chipsize *= (uint64_t)mtd->erasesize * p->lun_count;
#endif
	chip->bits_per_cell = p->bits_per_cell;

	if (onfi_feature(chip) & ONFI_FEATURE_16_BIT_BUS)
		*busw = NAND_BUSWIDTH_16;
	else
		*busw = 0;

	if (p->ecc_bits != 0xff) {
		chip->ecc_strength_ds = p->ecc_bits;
		chip->ecc_step_ds = 512;
	} else if (chip->onfi_version >= 21 &&
		(onfi_feature(chip) & ONFI_FEATURE_EXT_PARAM_PAGE)) {

		/*
		 * The nand_flash_detect_ext_param_page() uses the
		 * Change Read Column command which maybe not supported
		 * by the chip->cmdfunc. So try to update the chip->cmdfunc
		 * now. We do not replace user supplied command function.
		 */
		if (mtd->writesize > 512 && chip->cmdfunc == pnand_command)
			chip->cmdfunc = pnand_command_lp;

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
 * pnand_fill_oob - [Internal] Transfer client buffer to oob
 * @chip:	nand chip structure
 * @oob:	oob data buffer
 * @ops:	oob ops structure
 */
 uint8_t *pnand_fill_oob(struct nand_chip *chip, uint8_t *oob,
				  struct mtd_oob_ops *ops)
{
	size_t len = ops->ooblen;

	switch(ops->mode) {

	case MTD_OPS_PLACE_OOB:
	case MTD_OPS_RAW:
		memcpy(chip->oob_poi + ops->ooboffs, oob, len);
		return oob + len;

	case MTD_OPS_AUTO_OOB: {
		struct nand_oobfree *free = chip->ecc.layout->oobfree;
		uint32_t boffs = 0, woffs = ops->ooboffs;
		size_t bytes = 0;

		for(; free->length && len; free++, len -= bytes) {
			/* Write request not from offset 0 ? */
			if (unlikely(woffs)) {
				if (woffs >= free->length) {
					woffs -= free->length;
					continue;
				}
				boffs = free->offset + woffs;
				bytes = min_t(size_t, len,
					      (free->length - woffs));
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
 * nand_transfer_oob - [Internal] Transfer oob to client buffer
 * @chip:	nand chip structure
 * @oob:	oob destination address
 * @ops:	oob ops structure
 * @len:	size of oob to transfer
 */
 uint8_t *pnand_transfer_oob(struct nand_chip *chip, uint8_t *oob,
				  struct mtd_oob_ops *ops, size_t len)
{
	switch(ops->mode) {

	case MTD_OPS_PLACE_OOB:
	case MTD_OPS_RAW:
		memcpy(oob, chip->oob_poi + ops->ooboffs, len);
		return oob + len;

	case MTD_OPS_AUTO_OOB: {
		struct nand_oobfree *free = chip->ecc.layout->oobfree;
		uint32_t boffs = 0, roffs = ops->ooboffs;
		size_t bytes = 0;

		for(; free->length && len; free++, len -= bytes) {
			/* Read request not from offset 0 ? */
			if (unlikely(roffs)) {
				if (roffs >= free->length) {
					roffs -= free->length;
					continue;
				}
				boffs = free->offset + roffs;
				bytes = min_t(size_t, len,
					      (free->length - roffs));
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
	//default:
		//BUG();
	}
	return NULL;
}

/**
 * pnand_block_isbad - [MTD Interface] Check if block at offset is bad
 * @mtd:	MTD device structure
 * @offs:	offset relative to mtd start
 */
 int pnand_block_isbad(struct mtd_info *mtd, loff_t offs)
{
	int rc;
	/* Check for invalid offset */
	if (offs > mtd->size)
		return -EINVAL;


//	write_flash_ctrl_reg( FLASH_TYPE, 0xf000);

	rc = pnand_block_checkbad(mtd, offs, 1, 0);


	return rc;
}

/**
 * pnand_block_markbad - [MTD Interface] Mark block at the given offset as bad
 * @mtd:	MTD device structure
 * @ofs:	offset relative to mtd start
 */
 int pnand_block_markbad(struct mtd_info *mtd, loff_t ofs)
{
#if defined(CONFIG_PNAND_TEST_DRIVER)
	struct nand_chip *chip = mtd->priv;
#else
	struct nand_chip *chip = mtd_to_nand(mtd);
#endif
	int ret;


//	write_flash_ctrl_reg( FLASH_TYPE, 0xf000);
	if ((ret = pnand_block_isbad(mtd, ofs))) {
		/* If it was bad already, return success and do nothing. */


		if (ret > 0)
			return 0;
		return ret;
	}

	ret= chip->block_markbad(mtd, ofs);


	return ret;
}


#if 0 //CONFIG_PM
static int pnand_suspend(struct platform_device *dev, pm_message_t state)
{
	struct mfd_cell *cell = (struct mfd_cell *)dev->dev.platform_data;

	struct nand_chip *chip = pnand_host->nand_chip; //mtd->priv;

	return pnand_get_device(chip, pnand_host->mtd, FL_PM_SUSPENDED);
}

static int pnand_resume(struct platform_device *dev)
{
	struct nand_chip *chip = pnand_host->nand_chip; //mtd->priv;

	if (chip->state == FL_PM_SUSPENDED)
		pnand_release_device(pnand_host->mtd);
	else
		printf(KERN_ERR "%s called for a chip which is not "
		       "in suspended state\n", __func__);
}
#else
#define pnand_suspend NULL
#define pnand_resume NULL
#endif

/**
 * pnand_sync - [MTD Interface] sync
 * @mtd:	MTD device structure
 *
 * Sync is actually a wait for chip ready function
 */
 void pnand_sync(struct mtd_info *mtd)
{
#if defined(CONFIG_PNAND_TEST_DRIVER)
	struct nand_chip *chip = mtd->priv;
#else
	struct nand_chip *chip = mtd_to_nand(mtd);
#endif

#ifdef CS75XX_NAND_DEBUG
	printf( "%s: called\n", __func__);
#endif

	/* Grab the lock and see if the device is available */
	pnand_get_device(chip, mtd, FL_SYNCING);
	/* Release it and go back */
	pnand_release_device(mtd);
}

/**
 * pnand_check_wp - [GENERIC] check if the chip is write protected
 * @mtd:	MTD device structure
 * Check, if the device is write protected
 *
 * The function expects, that the device is already selected
 */
 int pnand_check_wp (struct mtd_info *mtd)
{
#if defined(CONFIG_PNAND_TEST_DRIVER)
	struct nand_chip *this = mtd->priv;
#else
	struct nand_chip *this = mtd_to_nand(mtd);
#endif
	/* Check the WP bit */
	int ready;

	check_flash_ctrl_status();

	write_flash_ctrl_reg(FLASH_NF_ECC_CONTROL, 0x0); //disable ecc gen

	nf_count.wrd = 0;
	nf_count.bf.nflashRegOobCount = NF_CNT_EMPTY_OOB;
	nf_count.bf.nflashRegDataCount = NF_CNT_DATA_1;
	nf_count.bf.nflashRegAddrCount = NF_CNT_EMPTY_ADDR;
	nf_count.bf.nflashRegCmdCount = NF_CNT_CMD_1;
	write_flash_ctrl_reg(FLASH_NF_COUNT, nf_count.wrd);

	nf_command.wrd = 0;
	nf_command.bf.nflashRegCmd0 = NAND_CMD_STATUS;
	write_flash_ctrl_reg(FLASH_NF_COMMAND, nf_command.wrd);

	nf_access.wrd = 0;
	nf_access.bf.nflashCeAlt = NF_ACCESS_CE_ALT;
	//nf_access.bf.nflashDirWr = ;
	nf_access.bf.nflashRegWidth = NF_ACCESS_WiDTH8;
	write_flash_ctrl_reg(FLASH_NF_ACCESS, nf_access.wrd);

	access_start.wrd = 0;
	access_start.bf.nflashRegReq = FLASH_GO;
	access_start.bf.nflashRegCmd = FLASH_RD;
	write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, access_start.wrd);

	access_start.wrd=read_flash_ctrl_reg(FLASH_FLASH_ACCESS_START);
	while(access_start.bf.nflashRegReq)
	{
		access_start.wrd=read_flash_ctrl_reg(FLASH_FLASH_ACCESS_START);
		udelay(1);

	}

	ready = read_flash_ctrl_reg(FLASH_NF_DATA)&0xff;
	if(ready==0xff)
		printf("pnand_check_wp flash status : %x\n",read_flash_ctrl_reg(FLASH_STATUS));

	return (ready & NAND_STATUS_WP) ? 0 : 1;
}

/**
 * pnand_write_oob - [MTD Interface] NAND write data and/or out-of-band
 * @mtd:	MTD device structure
 * @to:		offset to write to
 * @ops:	oob operation description structure
 */
 int pnand_write_oob(struct mtd_info *mtd, loff_t to,
			  struct mtd_oob_ops *ops)
{
#if defined(CONFIG_PNAND_TEST_DRIVER)
	struct nand_chip *chip = mtd->priv;
#else
	struct nand_chip *chip = mtd_to_nand(mtd);
#endif
	int ret = -ENOTSUPP;

	ops->retlen = 0;

	/* Do not allow writes past end of device */
	if (ops->datbuf && (to + ops->len) > mtd->size) {
		printf("%s: Attempt write beyond "
				"end of device\n", __func__);
		return -EINVAL;
	}



	pnand_get_device(chip, mtd, FL_WRITING);

	switch(ops->mode) {
	case MTD_OPS_PLACE_OOB:
	case MTD_OPS_AUTO_OOB:
	case MTD_OPS_RAW:
		break;

	default:
		goto out;
	}

	if (!ops->datbuf)
		ret = pnand_do_write_oob(mtd, to, ops);
	else
		ret = pnand_do_write_ops(mtd, to, ops);

 out:
	pnand_release_device(mtd);

	return ret;
}

/**
 * pnand_do_read_oob - [Intern] NAND read out-of-band
 * @mtd:	MTD device structure
 * @from:	offset to read from
 * @ops:	oob operations description structure
 *
 * NAND read out-of-band data from the spare area
 */
 int pnand_do_read_oob(struct mtd_info *mtd, loff_t from,
			    struct mtd_oob_ops *ops)
{
	int page, realpage, chipnr, sndcmd = 1;
#if defined(CONFIG_PNAND_TEST_DRIVER)
	struct nand_chip *chip = mtd->priv;
#else
	struct nand_chip *chip = mtd_to_nand(mtd);
#endif
	int blkcheck = (1 << (chip->phys_erase_shift - chip->page_shift)) - 1;
	int readlen = ops->ooblen;
	int len;
	uint8_t *buf = ops->oobbuf;


#ifdef CS75XX_NAND_DEBUG
	printf("%s: from = 0x%08Lx, len = %i\n",
			__func__, (unsigned long long)from, readlen);
#endif

	if (ops->mode == MTD_OPS_AUTO_OOB)
		len = chip->ecc.layout->oobavail;
	else
		len = mtd->oobsize;


	if (unlikely(ops->ooboffs >= len)) {
		printf("%s: Attempt to start read "
					"outside oob\n", __func__);
		return -EINVAL;
	}

#if 0
	/* Do not allow read past end of page */
	if ((ops->ooboffs + ops->ooblen) > len) {
		printf("%s: Attempt to read "
				"past end of page\n", __func__);
		return -EINVAL;
	}

#endif

	/* Do not allow reads past end of device */
	if (unlikely(from >= mtd->size ||
		     ops->ooboffs + readlen > ((mtd->size >> chip->page_shift) -
					(from >> chip->page_shift)) * len)) {
		printf("%s: Attempt read beyond end "
					"of device\n", __func__);
		return -EINVAL;
	}

	chipnr = (int)(from >> chip->chip_shift);
	chip->select_chip(mtd, chipnr);

	/* Shift to get page */
	realpage = (int)(from >> chip->page_shift);
	page = realpage & chip->pagemask;

	while(1) {
		sndcmd = chip->ecc.read_oob(mtd, chip, page);

		len = min(len, readlen);
		buf = pnand_transfer_oob(chip, buf, ops, len);

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
				pnand_wait_ready(mtd);
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

		/* Check, if the chip supports auto page increment
		 * or if we have hit a block boundary.

		if (!NAND_CANAUTOINCR(chip) || !(page & blkcheck))
			sndcmd = 1;
		*/
	}

	ops->oobretlen = ops->ooblen;
	return 0;
}

/**
 * pnand_read_oob - [MTD Interface] NAND read data and/or out-of-band
 * @mtd:	MTD device structure
 * @from:	offset to read from
 * @ops:	oob operation description structure
 *
 * NAND read data and/or out-of-band data
 */
 int pnand_read_oob(struct mtd_info *mtd, loff_t from,
			 struct mtd_oob_ops *ops)
{
#if defined(CONFIG_PNAND_TEST_DRIVER)
	struct nand_chip *chip = mtd->priv;
#else
	struct nand_chip *chip = mtd_to_nand(mtd);
#endif
	int ret = -ENOTSUPP;

	ops->retlen = 0;

	/* Do not allow reads past end of device */
	if (ops->datbuf && (from + ops->len) > mtd->size) {
		printf("%s: Attempt read "
				"beyond end of device\n", __func__);
		return -EINVAL;
	}



	pnand_get_device(chip, mtd, FL_READING);

	switch(ops->mode) {
	case MTD_OPS_PLACE_OOB:
	case MTD_OPS_AUTO_OOB:
	case MTD_OPS_RAW:
		break;

	default:
		goto out;
	}

	if (!ops->datbuf)
		ret = pnand_do_read_oob(mtd, from, ops);
	else
		ret = pnand_do_read_ops(mtd, from, ops);

 out:
	pnand_release_device(mtd);


	return ret;
}


/**
 * pnand_write - [MTD Interface] NAND write with ECC
 * @mtd:	MTD device structure
 * @to:		offset to write to
 * @len:	number of bytes to write
 * @retlen:	pointer to variable to store the number of written bytes
 * @buf:	the data to write
 *
 * NAND write with ECC
 */
 int pnand_write(struct mtd_info *mtd, loff_t to, size_t len,
			  size_t *retlen, const uint8_t *buf)
{
#if defined(CONFIG_PNAND_TEST_DRIVER)
	struct nand_chip *chip = mtd->priv;
#else
	struct nand_chip *chip = mtd_to_nand(mtd);
#endif
	struct mtd_oob_ops ops;
	int ret;

	/* Do not allow reads past end of device */
	if ((to + len) > mtd->size)
		return -EINVAL;
	if (!len)
		return 0;


	pnand_get_device(chip, mtd, FL_WRITING);

	ops.len = len;
	ops.datbuf = (uint8_t *)buf;
	ops.oobbuf = NULL;
	ops.mode = MTD_OPS_PLACE_OOB;

	ret = pnand_do_write_ops(mtd, to, &ops);

	*retlen = ops.retlen;

	pnand_release_device(mtd);


	return ret;
}

/**
 * pnand_do_read_ops - [Internal] Read data with ECC
 *
 * @mtd:	MTD device structure
 * @from:	offset to read from
 * @ops:	oob ops structure
 *
 * Internal function. Called with chip held.
 */
 int pnand_do_read_ops(struct mtd_info *mtd, loff_t from,
			    struct mtd_oob_ops *ops)
{
	int chipnr, page, realpage, col, bytes, aligned;
#if defined(CONFIG_PNAND_TEST_DRIVER)
	struct nand_chip *chip = mtd->priv;
#else
	struct nand_chip *chip = mtd_to_nand(mtd);
#endif
	struct mtd_ecc_stats stats;
	int blkcheck = (1 << (chip->phys_erase_shift - chip->page_shift)) - 1;
	int sndcmd = 1;
	int ret = 0;
	uint32_t readlen = ops->len;
	uint32_t oobreadlen = ops->ooblen;
	uint8_t *bufpoi, *oob, *buf;

//	stats.failed= mtd->ecc_stats.failed;
	memcpy(&stats, &mtd->ecc_stats, sizeof (struct mtd_ecc_stats));

	chipnr = (int)(from >> chip->chip_shift);
	chip->select_chip(mtd, chipnr);

	realpage = (int)(from >> chip->page_shift);
	page = realpage & chip->pagemask;

	col = (int)(from & (mtd->writesize - 1));

	buf = ops->datbuf;
	oob = ops->oobbuf;

	while(1) {
		bytes = min(mtd->writesize - col, readlen);
		aligned = (bytes == mtd->writesize);

		/* Is the current page in the buffer ? */
		if (realpage != chip->pagebuf || oob) {
			bufpoi = aligned ? buf : chip->buffers->databuf;

			if (likely(sndcmd)) {
				chip->cmdfunc(mtd, NAND_CMD_READ0, 0x00, page);
				sndcmd = 0;
			}

			/* Now read the page into the buffer */
			if (unlikely(ops->mode == MTD_OPS_RAW))
				ret = chip->ecc.read_page_raw(mtd, chip,
							      bufpoi, 0, page);
			else if (!aligned && NAND_HAS_SUBPAGE_READ(chip) && !oob)
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
						oob = pnand_transfer_oob(chip,
							oob, ops, toread);
						oobreadlen -= toread;
					}
				} else
					buf = pnand_transfer_oob(chip, oob, ops, mtd->oobsize);
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
					pnand_wait_ready(mtd);
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

		/* Check, if the chip supports auto page increment
		 * or if we have hit a block boundary.

		if (!NAND_CANAUTOINCR(chip) || !(page & blkcheck))
			sndcmd = 1;
		*/
	}

	ops->retlen = ops->len - (size_t) readlen;
	if (oob)
		ops->oobretlen = ops->ooblen - oobreadlen;

	if (ret)
		return ret;

	if (mtd->ecc_stats.failed - stats.failed)
		return -EBADMSG;

	return  mtd->ecc_stats.corrected - stats.corrected ? -EUCLEAN : 0;
}

/**
 * pnand_read - [MTD Interface] MTD compability function for nand_do_read_ecc
 * @mtd:	MTD device structure
 * @from:	offset to read from
 * @len:	number of bytes to read
 * @retlen:	pointer to variable to store the number of read bytes
 * @buf:	the databuffer to put data
 *
 * Get hold of the chip and call nand_do_read
 */

 int pnand_read(struct mtd_info *mtd, loff_t from, size_t len,
		     size_t *retlen, uint8_t *buf)
{
#if defined(CONFIG_PNAND_TEST_DRIVER)
	struct nand_chip *chip = mtd->priv;
#else
	struct nand_chip *chip = mtd_to_nand(mtd);
#endif
	struct mtd_oob_ops ops;
	int ret;

	/* Do not allow reads past end of device */
	if ((from + len) > mtd->size)
		return -EINVAL;
	if (!len)
		return 0;

	pnand_get_device(chip, mtd, FL_READING);

	ops.len = len;
	ops.datbuf = buf;
	ops.oobbuf = NULL;
	ops.mode = MTD_OPS_PLACE_OOB;

	ret = pnand_do_read_ops(mtd, from, &ops);

	*retlen = ops.retlen;

	pnand_release_device(mtd);

	return ret;
}

/**
 * pnand_erase_block - [GENERIC] erase a block
 * @mtd:	MTD device structure
 * @page:	page address
 *
 * Erase a block.
 */

 int pnand_erase_block(struct mtd_info *mtd, uint32_t page)
{
//	int opcode,tst=0,tst1=0,tst2=0;
#if defined(CONFIG_PNAND_TEST_DRIVER)
	struct nand_chip *this = mtd->priv;
#else
	struct nand_chip *this = mtd_to_nand(mtd);
#endif
	//u64 test;

	check_flash_ctrl_status();

	/* Send commands to erase a page */
	write_flash_ctrl_reg(FLASH_NF_ECC_CONTROL, 0); //

	nf_count.wrd = 0;
	nf_command.wrd = 0;
	nf_addr1.wrd = 0;
	nf_count.bf.nflashRegAddrCount = NF_CNT_ADDR_3;
	nf_count.bf.nflashRegOobCount = NF_CNT_EMPTY_OOB;
	nf_count.bf.nflashRegDataCount = NF_CNT_EMPTY_DATA;
	nf_count.bf.nflashRegCmdCount = NF_CNT_CMD_2;
#if 0
	/*
	test = this->chipsize;
	test = test / mtd->writesize;
	if((this->chipsize/mtd->writesize) > 0x10000)
	*/

	test = 0x10000 * mtd->writesize;
	if( this->chipsize  > test) {
	    nf_count.bf.nflashRegAddrCount = NF_CNT_ADDR_3;
	} else {
	    nf_count.bf.nflashRegAddrCount = NF_CNT_ADDR_2;
	}
#endif
	nf_command.wrd = 0;
	nf_command.bf.nflashRegCmd0 = NAND_CMD_ERASE1;
	nf_command.bf.nflashRegCmd1 = NAND_CMD_ERASE2;

	nf_addr1.wrd = 0;
	nf_addr1.wrd = page;
	nf_addr2.wrd = 0;
	write_flash_ctrl_reg(FLASH_NF_COUNT, nf_count.wrd);
	write_flash_ctrl_reg(FLASH_NF_COMMAND, nf_command.wrd);
	write_flash_ctrl_reg(FLASH_NF_ADDRESS_1, nf_addr1.wrd);
	write_flash_ctrl_reg(FLASH_NF_ADDRESS_2, nf_addr2.wrd);

	nf_access.wrd = 0;
	nf_access.bf.nflashCeAlt = NF_ACCESS_CE_ALT;
	nf_access.bf.nflashRegWidth = NF_ACCESS_WiDTH8;

	write_flash_ctrl_reg(FLASH_NF_ACCESS, nf_access.wrd);
	access_start.wrd = 0;
	access_start.bf.nflashRegReq = FLASH_GO;
	access_start.bf.nflashRegCmd = FLASH_RD;  //no data access use read..
	write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, access_start.wrd);
	access_start.wrd=read_flash_ctrl_reg(FLASH_FLASH_ACCESS_START);
	tmp_access.wrd = 0;
	tmp_access.bf.nflashRegReq = 1;
	reg_wait(FLASH_FLASH_ACCESS_START, tmp_access.wrd, 0, 1000);

	return 0;
}



/**
 * pnand_block_checkbad - [GENERIC] Check if a block is marked bad
 * @mtd:	MTD device structure
 * @ofs:	offset from device start
 * @getchip:	0, if the chip is already selected
 * @allowbbt:	1, if its allowed to access the bbt area
 *
 * Check, if the block is bad. Either by reading the bad block table or
 * calling of the scan function.
 */
 int pnand_block_checkbad(struct mtd_info *mtd, loff_t ofs, int getchip,
			       int allowbbt)
{
#if defined(CONFIG_PNAND_TEST_DRIVER)
	struct nand_chip *chip = mtd->priv;
	if (!chip->bbt)
		return chip->block_bad(mtd, ofs, getchip);
#else
	struct nand_chip *chip = mtd_to_nand(mtd);
	if (!chip->bbt)
		return chip->block_bad(mtd, ofs);
#endif

	/* Return info from the table */
	return nand_isbad_bbt(mtd, ofs, allowbbt);
}

/**
 * pnand_erase - [MTD Interface] erase block(s)
 * @mtd:	MTD device structure
 * @instr:	erase instruction
 *
 * Erase one ore more blocks
 */
 int pnand_erase(struct mtd_info *mtd, struct erase_info *instr)
{
	int rc;

	rc = pnand_erase_nand(mtd, instr, 0);

	return rc;
}

/**
 * pnand_erase_nand - [Internal] erase block(s)
 * @mtd:	MTD device structure
 * @instr:	erase instruction
 * @allowbbt:	allow erasing the bbt area
 *
 * Erase one ore more blocks
 */
int pnand_erase_nand(struct mtd_info *mtd, struct erase_info *instr,
		    int allowbbt)
{
	int page, status, pages_per_block, ret, chipnr;
#if defined(CONFIG_PNAND_TEST_DRIVER)
	struct nand_chip *chip = mtd->priv;
#else
	struct nand_chip *chip = mtd_to_nand(mtd);
#endif
	int rewrite_bbt[NAND_MAX_CHIPS]={0};
	unsigned int bbt_masked_page = 0xffffffff;
	loff_t len;

#ifdef CS75XX_NAND_DEBUG
	printf("%s: start = 0x%012llx, len = %llu\n",
				__func__, (unsigned long long)instr->addr,
				(unsigned long long)instr->len);
#endif

	/* Start address must align on block boundary */
	if (instr->addr & ((1 << chip->phys_erase_shift) - 1)) {
		printf("%s: Unaligned address\n", __func__);
		printf( KERN_ALERT "unaligned_chipptr!!!");
		return -EINVAL;
	}

	/* Length must align on block boundary */
	if (instr->len & ((1 << chip->phys_erase_shift) - 1)) {
		printf("%s: Length not block aligned\n",
					__func__);
		return -EINVAL;
	}

	/* Do not allow erase past end of device */
	if ((instr->len + instr->addr) > mtd->size) {
		printf("%s: Erase past end of device\n",
					__func__);
		return -EINVAL;
	}

	instr->fail_addr = MTD_FAIL_ADDR_UNKNOWN;

	/* Grab the lock and see if the device is available */
	pnand_get_device(chip, mtd, FL_ERASING);

	/* Shift to get first page */
	page = (int)(instr->addr >> chip->page_shift);
	chipnr = (int)(instr->addr >> chip->chip_shift);

	/* Calculate pages in each block */
	pages_per_block = 1 << (chip->phys_erase_shift - chip->page_shift);

	/* Select the NAND device */
	chip->select_chip(mtd, chipnr);

	/* Check, if it is write protected */
	if (pnand_check_wp(mtd)) {
		printf("%s: Device is write protected!!!\n",
					__func__);
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
	//	bbt_masked_page = chip->bbt_td->pages[chipnr] & BBT_PAGE_MASK;

	/* Loop through the pages */
	len = instr->len;

	instr->state = MTD_ERASING;

	while (len) {
		/*
		 * heck if we have a bad block, we do not erase bad blocks !
		 */
		if (pnand_block_checkbad(mtd, ((loff_t) page) <<
					chip->page_shift, 0, allowbbt)) {
			printf(KERN_WARNING "%s: attempt to erase a bad block "
					"at page 0x%08x\n", __func__, page);
			instr->state = MTD_ERASE_FAILED;
			goto erase_exit;
		}

		/*
		 * Invalidate the page cache, if we erase the block which
		 * contains the current cached page
		 */
		if (page <= chip->pagebuf && chip->pagebuf <
		    (page + pages_per_block))
			chip->pagebuf = -1;

		//chip->erase_cmd(mtd, page & chip->pagemask);
		pnand_erase_block(mtd, page);

		status = chip->waitfunc(mtd, chip);

		/*
		 * See if operation failed and additional status checks are
		 * available
		 */
		if ((status & NAND_STATUS_FAIL) && (chip->errstat))
			status = chip->errstat(mtd, chip, FL_ERASING,
					       status, page);

		/* See if block erase succeeded */
		if (status & NAND_STATUS_FAIL) {
			printf("%s: Failed erase, "
					"page 0x%08x\n", __func__, page);
			instr->state = MTD_ERASE_FAILED;
			instr->fail_addr =
				((loff_t)page << chip->page_shift);
			goto erase_exit;
		}

		/*
		 * If BBT requires refresh, set the BBT rewrite flag to the
		 * page being erased
		 */
		if (bbt_masked_page != 0xffffffff &&
		    (page & BBT_PAGE_MASK) == bbt_masked_page)
			    rewrite_bbt[chipnr] =
					((loff_t)page << chip->page_shift);

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
			if (bbt_masked_page != 0xffffffff &&
			    (chip->bbt_td->options & NAND_BBT_PERCHIP))
				bbt_masked_page = chip->bbt_td->pages[chipnr] &
					BBT_PAGE_MASK;
		}
	}
	instr->state = MTD_ERASE_DONE;

 erase_exit:

	ret = instr->state == MTD_ERASE_DONE ? 0 : -EIO;

	/* Deselect and wake up anyone waiting on the device */
	pnand_release_device(mtd);

	/* Do call back function */
	if (!ret)
		mtd_erase_callback(instr);

	/*
	 * If BBT requires refresh and erase was successful, rewrite any
	 * selected bad block tables
	 */
	if (bbt_masked_page == 0xffffffff || ret)
		return ret;

	for (chipnr = 0; chipnr < chip->numchips; chipnr++) {
		if (!rewrite_bbt[chipnr])
			continue;
		/* update the BBT for chip */
		printf("%s: nand_update_bbt "
			"(%d:0x%x 0x%0x)\n", __func__, chipnr,
			rewrite_bbt[chipnr], chip->bbt_td->pages[chipnr]);
		nand_update_bbt(mtd, rewrite_bbt[chipnr]);
	}

	/* Return more or less happy */
	return ret;
}

/**
 * pnand_write_oob_std - [REPLACABLE] the most common OOB data write function
 * @mtd:	mtd info structure
 * @chip:	nand chip info structure
 * @page:	page number to write
 */
 int pnand_write_oob_std(struct mtd_info *mtd, struct nand_chip *chip,
			      int page)
{
	int status = 0, i;
	//const uint8_t *buf = chip->oob_poi;
	//int length = mtd->oobsize;

	check_flash_ctrl_status();

	chip->cmdfunc(mtd, NAND_CMD_SEQIN, mtd->writesize, page);
	//chip->write_buf(mtd, buf, length);
	/* Send command to program the OOB data */
	chip->cmdfunc(mtd, NAND_CMD_PAGEPROG, -1, -1);

	write_flash_ctrl_reg(FLASH_NF_ECC_CONTROL, 0x0); //disable ecc gen

	nf_command.wrd = 0;
	nf_addr1.wrd = 0;
	nf_count.wrd = 0;
	nf_count.bf.nflashRegOobCount = mtd->oobsize- 1;
	nf_count.bf.nflashRegDataCount = NF_CNT_EMPTY_DATA;

	nf_addr2.wrd = 0;

	if(chip->chipsize < SZ_32M )
	{
		nf_count.bf.nflashRegAddrCount = NF_CNT_ADDR_3;

		if(mtd->writesize > NF_CNT_512P_DATA)
		{
			nf_count.bf.nflashRegCmdCount = NF_CNT_CMD_2;
			nf_command.bf.nflashRegCmd0 = NAND_CMD_SEQIN;
			nf_command.bf.nflashRegCmd1 = NAND_CMD_PAGEPROG;
		}
		else
		{
			nf_count.bf.nflashRegCmdCount = NF_CNT_CMD_3;
			nf_command.bf.nflashRegCmd0 = NAND_CMD_READOOB;  //??
			nf_command.bf.nflashRegCmd1 = NAND_CMD_SEQIN;
			nf_command.bf.nflashRegCmd2 = NAND_CMD_PAGEPROG;
		}
		//read oob need to add page data size to match correct oob ddress
		nf_addr1.wrd = (((page & 0x00ffffff)<<8));
		nf_addr2.wrd = ((page & 0xff000000)>>24);
	}
	else if(chip->chipsize <= SZ_128M )
	{
		nf_count.bf.nflashRegAddrCount = NF_CNT_ADDR_4;
		nf_count.bf.nflashRegCmdCount = NF_CNT_CMD_2;
		nf_command.bf.nflashRegCmd0 = NAND_CMD_SEQIN;
		nf_command.bf.nflashRegCmd1 = NAND_CMD_PAGEPROG;
		nf_addr1.wrd = (((page & 0xffff)<<16) + (mtd->writesize&0xffff));
		nf_addr2.wrd = ((page & 0xffff0000)>>16);

	}
	else //if((chip->chipsize > (128 << 20)) ))
	{

		nf_count.bf.nflashRegAddrCount = NF_CNT_ADDR_5;
		nf_count.bf.nflashRegCmdCount = NF_CNT_CMD_2;
		nf_command.bf.nflashRegCmd0 = NAND_CMD_SEQIN;
		nf_command.bf.nflashRegCmd1 = NAND_CMD_PAGEPROG;
		nf_addr1.wrd = (((page & 0xffff)<<16) + (mtd->writesize&0xffff));
		nf_addr2.wrd = ((page & 0xffff0000)>>16);

	}

	write_flash_ctrl_reg(FLASH_NF_COUNT, nf_count.wrd);
	write_flash_ctrl_reg(FLASH_NF_COMMAND, nf_command.wrd); //write read id command
	write_flash_ctrl_reg(FLASH_NF_ADDRESS_1, nf_addr1.wrd); //write address 0x0
	write_flash_ctrl_reg(FLASH_NF_ADDRESS_2, nf_addr2.wrd);

	pwrite = (unsigned int *) chip->oob_poi;

	for(i=0;i<((mtd->oobsize/4));i++)
	{
		nf_access.wrd = 0;
		nf_access.bf.nflashCeAlt = NF_ACCESS_CE_ALT;
		//nf_access.bf.nflashDirWr = ;
		nf_access.bf.nflashRegWidth = NF_ACCESS_WiDTH32;
		write_flash_ctrl_reg(FLASH_NF_ACCESS, nf_access.wrd);

		write_flash_ctrl_reg(FLASH_NF_DATA,pwrite[i]);

		access_start.wrd = 0;
		access_start.bf.nflashRegReq = FLASH_GO;
		access_start.bf.nflashRegCmd = FLASH_WT;
		//access_start.bf.nflash_random_access = RND_ENABLE;
		write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, access_start.wrd);

		tmp_access.wrd = 0;
		tmp_access.bf.nflashRegReq = 1;
		reg_wait(FLASH_FLASH_ACCESS_START, tmp_access.wrd, 0, 1000);

	}
#if 0
	unsigned int rtmp[80];
	nf_command.bf.nflashRegCmd0 = NAND_CMD_READ0;
	nf_command.bf.nflashRegCmd1 = NAND_CMD_READSTART;
	write_flash_ctrl_reg(FLASH_NF_COMMAND, nf_command.wrd); //write read id command

	for(i=0;i<((mtd->oobsize/4));i++)
	{
		nf_access.wrd = 0;
		nf_access.bf.nflashCeAlt = NF_ACCESS_CE_ALT;
		//nf_access.bf.nflashDirWr = ;
		nf_access.bf.nflashRegWidth = NFLASH_WiDTH32;
		write_flash_ctrl_reg(FLASH_NF_ACCESS, nf_access.wrd);

		access_start.wrd = 0;
		access_start.bf.nflashRegReq = FLASH_GO;
		access_start.bf.nflashRegCmd = FLASH_RD;
		//access_start.bf.nflash_random_access = RND_ENABLE;
		write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, access_start.wrd);

		access_start.wrd=read_flash_ctrl_reg(FLASH_FLASH_ACCESS_START);
		while(access_start.bf.nflashRegReq)
		{
			access_start.wrd=read_flash_ctrl_reg(FLASH_FLASH_ACCESS_START);
			udelay(1);
		}
		rtmp[i] = read_flash_ctrl_reg(FLASH_NF_DATA);
	}
	if (memcmp((unsigned char *)pwrite , (unsigned char *)rtmp, mtd->oobsize))
		printf("W->R oob error\n");
#endif
	check_flash_ctrl_status();

	status = chip->waitfunc(mtd, chip);

	return status & NAND_STATUS_FAIL ? -EIO : 0;
}

/**
 * pnand_read_oob_std - [REPLACABLE] the most common OOB data read function
 * @mtd:	mtd info structure
 * @chip:	nand chip info structure
 * @page:	page number to read
 * @sndcmd:	flag whether to issue read command or not
 */
 int pnand_read_oob_std(struct mtd_info *mtd, struct nand_chip *chip,
			     int page, int sndcmd)
{
	int i;

	check_flash_ctrl_status();

	write_flash_ctrl_reg(FLASH_NF_ECC_CONTROL, 0x0); //disable ecc gen

	nf_command.wrd = 0;
	nf_addr1.wrd = 0;
	nf_addr2.wrd = 0;
	nf_count.wrd = 0;
	nf_count.bf.nflashRegOobCount = mtd->oobsize -1 ;
	nf_count.bf.nflashRegDataCount = NF_CNT_EMPTY_DATA;

	if(chip->chipsize < (32 << 20))
	{
		nf_count.bf.nflashRegAddrCount = NF_CNT_ADDR_3;
		nf_count.bf.nflashRegCmdCount = NF_CNT_CMD_1;
		if(mtd->writesize > NF_CNT_512P_DATA)
			nf_command.bf.nflashRegCmd0 = NAND_CMD_READ0;
		else
			nf_command.bf.nflashRegCmd0 = NAND_CMD_READOOB;

		nf_addr1.wrd = ((page & 0x00ffffff)<<8);
		nf_addr2.wrd = ((page & 0xff000000)>>24);
	}
	else if((chip->chipsize >= (32 << 20)) && (chip->chipsize <= (128 << 20)))
	{
		nf_count.bf.nflashRegAddrCount = NF_CNT_ADDR_4;
		nf_count.bf.nflashRegCmdCount = NF_CNT_CMD_1;
		nf_command.bf.nflashRegCmd0 = NAND_CMD_READ0;

		// Jeneng
		if(mtd->writesize > NF_CNT_512P_DATA){
			nf_count.bf.nflashRegCmdCount = NF_CNT_CMD_2;
			nf_command.bf.nflashRegCmd1 = NAND_CMD_READSTART;
		}
		nf_addr1.wrd = (((page & 0xffff)<<16) + (mtd->writesize&0xffff));
		nf_addr2.wrd = ((page & 0xffff0000)>>16);

	}
	else //if((chip->chipsize > (128 << 20)) ))
	{

		nf_count.bf.nflashRegAddrCount = NF_CNT_ADDR_5;
		nf_count.bf.nflashRegCmdCount = NF_CNT_CMD_2;
		nf_command.bf.nflashRegCmd0 = NAND_CMD_READ0;
		nf_command.bf.nflashRegCmd1 = NAND_CMD_READSTART;
		nf_addr1.wrd = (((page & 0xffff)<<16) + (mtd->writesize&0xffff));
		nf_addr2.wrd = ((page & 0xffff0000)>>16);
	}


	write_flash_ctrl_reg(FLASH_NF_COUNT, nf_count.wrd);
	write_flash_ctrl_reg(FLASH_NF_COMMAND, nf_command.wrd); //write read id command
	write_flash_ctrl_reg(FLASH_NF_ADDRESS_1, nf_addr1.wrd); //write address 0x0
	write_flash_ctrl_reg(FLASH_NF_ADDRESS_2, nf_addr2.wrd);

	pread = (unsigned int *) chip->oob_poi;

	for(i=0;i< mtd->oobsize/4;i++)
	{
		nf_access.wrd = 0;
		nf_access.bf.nflashCeAlt = NF_ACCESS_CE_ALT;
		//nf_access.bf.nflashDirWr = ;
		nf_access.bf.nflashRegWidth = NF_ACCESS_WiDTH32;
		write_flash_ctrl_reg(FLASH_NF_ACCESS, nf_access.wrd);

		access_start.wrd = 0;
		access_start.bf.nflashRegReq = FLASH_GO;
		access_start.bf.nflashRegCmd = FLASH_RD;
		//access_start.bf.nflash_random_access = RND_ENABLE;
		write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, access_start.wrd);

		access_start.wrd=read_flash_ctrl_reg(FLASH_FLASH_ACCESS_START);

		tmp_access.wrd = 0;
		tmp_access.bf.nflashRegReq = 1;
		reg_wait(FLASH_FLASH_ACCESS_START, tmp_access.wrd, 0, 1000);

			pread[i] = read_flash_ctrl_reg(FLASH_NF_DATA);

	}
	return sndcmd;
}

/**
 * pnand_write_page_hwecc - [REPLACABLE] hardware ecc based page write function
 * @mtd:	mtd info structure
 * @chip:	nand chip info structure
 * @buf:	data buffer
 */
 void pnand_write_page_hwecc(struct mtd_info *mtd, struct nand_chip *chip,
				  const uint8_t *buf, int oob_required)
{

	int i, j, eccsize = chip->ecc.size, page, col, reg_tmp;
	int eccbytes = chip->ecc.bytes;
	int eccsteps = chip->ecc.steps;
	/*uint8_t *ecc_calc = chip->buffers->ecccalc;*/
	/*const uint8_t *p = buf;*/
	uint32_t *eccpos = chip->ecc.layout->eccpos, *addr, reg_bch_gen;
	DMA_SEC_DMA_SSP_Q_RXQ_COAL_INTERRUPT_t	tmp_dma_ssp_rxq5_intsts;
	DMA_SEC_DMA_SSP_Q_TXQ_COAL_INTERRUPT_t	tmp_dma_ssp_txq5_intsts;
#ifdef CONFIG_PNAND_ECC_HW_BCH
	FLASH_NF_BCH_STATUS_t	tmp_bsc_sts;
#else
	FLASH_NF_ECC_STATUS_t	tmp_ecc_sts;
#endif
	FLASH_FIFO_CONTROL_t	flash_fifo_ctrl;

	check_flash_ctrl_status();

	page = host->page_addr;
	col  = host->col_addr;

	nf_ecc_reset.wrd = 3;
	nf_ecc_reset.bf.eccClear = ECC_CLR;
	nf_ecc_reset.bf.fifoClear = FIFO_CLR;
	write_flash_ctrl_reg(FLASH_NF_ECC_RESET, nf_ecc_reset.wrd);

	flash_int_sts.bf.regIrq = 1;
	write_flash_ctrl_reg(FLASH_FLASH_INTERRUPT, flash_int_sts.wrd);

	nf_ecc_reset.wrd = 0;
	nf_ecc_reset.bf.eccClear = 1;
	write_flash_ctrl_reg(FLASH_NF_ECC_RESET, nf_ecc_reset.wrd);

#ifdef CONFIG_PNAND_ECC_HW_BCH
		nf_bch_ctrl.wrd = 0;
		nf_bch_ctrl.bf.bchEn = BCH_ENABLE;
		nf_bch_ctrl.bf.bchOpcode = BCH_ENCODE;
#if	(CONFIG_PNAND_ECC_HW_BCH_LEVEL == 8)
			nf_bch_ctrl.bf.bchErrCap = BCH_ERR_CAP_8;
#elif (CONFIG_PNAND_ECC_HW_BCH_LEVEL == 16)
			nf_bch_ctrl.bf.bchErrCap = BCH_ERR_CAP_16;
#elif (CONFIG_PNAND_ECC_HW_BCH_LEVEL == 24)
			nf_bch_ctrl.bf.bchErrCap = BCH_ERR_CAP_24;
#elif (CONFIG_PNAND_ECC_HW_BCH_LEVEL == 40)
			nf_bch_ctrl.bf.bchErrCap = BCH_ERR_CAP_40;
#else
			printf("BCH Level is not recognized! %s:%d\n",__func__, __LINE__);
#endif
		write_flash_ctrl_reg(FLASH_NF_BCH_CONTROL, nf_bch_ctrl.wrd);

#else

		nf_ecc_ctl.wrd = 0;
	if((eccsize-1) == NF_CNT_512P_DATA)
			nf_ecc_ctl.bf.eccGenMode = ECC_GEN_512;
		else
			nf_ecc_ctl.bf.eccGenMode = ECC_GEN_256;
		nf_ecc_ctl.bf.eccEn = ECC_ENABLE;
		write_flash_ctrl_reg(FLASH_NF_ECC_CONTROL, nf_ecc_ctl.wrd);
#endif



	/*disable txq5*/
	dma_txq5_ctrl.bf.txq_en = 0;
	write_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_CONTROL, dma_txq5_ctrl.wrd);
	/*clr tx/rx eof*/
	dma_ssp_txq5_intsts.wrd = read_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_COAL_INTERRUPT);
	dma_ssp_txq5_intsts.bf.txq_eof = 0;
	write_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_COAL_INTERRUPT, dma_ssp_txq5_intsts.wrd);
	dma_ssp_rxq5_intsts.wrd = read_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_RXQ_COAL_INTERRUPT);
	dma_ssp_rxq5_intsts.bf.rxq_eof = 0;
	write_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_RXQ_COAL_INTERRUPT, dma_ssp_rxq5_intsts.wrd);


	nf_count.wrd = 0;
	nf_command.wrd = 0;
	nf_addr1.wrd = 0;
	nf_addr2.wrd = 0;
	nf_count.wrd = 0;
	nf_count.bf.nflashRegOobCount = mtd->oobsize - 1;
	nf_count.bf.nflashRegDataCount = mtd->writesize-1;

	if(chip->chipsize < (32 << 20))
	{
		nf_count.bf.nflashRegAddrCount = NF_CNT_ADDR_3;
		nf_count.bf.nflashRegCmdCount = NF_CNT_CMD_2;
		nf_command.bf.nflashRegCmd0 = NAND_CMD_SEQIN;
		nf_command.bf.nflashRegCmd1 = NAND_CMD_PAGEPROG;
		nf_addr1.wrd = (((page & 0x00ffffff)<<8));
		nf_addr2.wrd = ((page & 0xff000000)>>24);

	}
	else if((chip->chipsize >= (32 << 20)) && (chip->chipsize <= (128 << 20)))
	{
		nf_count.bf.nflashRegAddrCount = NF_CNT_ADDR_4;
		nf_count.bf.nflashRegCmdCount = NF_CNT_CMD_2;
		nf_command.bf.nflashRegCmd0 = NAND_CMD_SEQIN;
		nf_command.bf.nflashRegCmd1 = NAND_CMD_PAGEPROG;
		nf_addr1.wrd = (((page & 0xffff)<<16) );
		nf_addr2.wrd = ((page & 0xffff0000)>>16);

	}
	else /*if((chip->chipsize > (128 << 20)) ))*/
	{

		nf_count.bf.nflashRegAddrCount = NF_CNT_ADDR_5;
		nf_count.bf.nflashRegCmdCount = NF_CNT_CMD_2;
		nf_command.bf.nflashRegCmd0 = NAND_CMD_SEQIN;
		nf_command.bf.nflashRegCmd1 = NAND_CMD_PAGEPROG;
		nf_addr1.wrd = (((page & 0xffff)<<16) );
		nf_addr2.wrd = ((page & 0xffff0000)>>16);
	}

	write_flash_ctrl_reg(FLASH_NF_COUNT, nf_count.wrd);
	write_flash_ctrl_reg(FLASH_NF_COMMAND, nf_command.wrd);
	write_flash_ctrl_reg(FLASH_NF_ADDRESS_1, nf_addr1.wrd);
	write_flash_ctrl_reg(FLASH_NF_ADDRESS_2, nf_addr2.wrd);

	/*dma_map_single( NULL, (void *)buf, mtd->writesize, DMA_TO_DEVICE);*/

	nf_access.wrd = 0;
	nf_access.bf.nflashCeAlt = NF_ACCESS_CE_ALT;
	nf_access.bf.nflashRegWidth = NF_ACCESS_WiDTH8;
	/*write_flash_ctrl_reg(FLASH_NF_ACCESS, nf_access.wrd);

	write
	prepare dma descriptor
	chip->buffers->databuf
	nf_access.wrd = read_flash_ctrl_reg(FLASH_FLASH_ACCESS_START);
	*/
	nf_access.bf.nflashExtAddr = ((page << chip->page_shift) / 0x8000000);
	write_flash_ctrl_reg(FLASH_NF_ACCESS, nf_access.wrd);

	addr =  (unsigned int *)((page << chip->page_shift) % 0x8000000);
	addr = (unsigned int *)((unsigned int)addr + (unsigned int)(CONFIG_SYS_FLASH_BASE) );

	/*page data tx desc*/
	dma_txq5_wptr.wrd = read_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_WPTR);
	tx_desc[dma_txq5_wptr.bf.index].word1.bf.own = OWN_DMA;
	tx_desc[dma_txq5_wptr.bf.index].word1.bf.buf_len = mtd->writesize;
	tx_desc[dma_txq5_wptr.bf.index].buf_adr = (unsigned int)(buf);


	/*page data rx desc
	printf("pnand_write_page_hwecc : addr : %p  buf: %p",addr, buf);
	*/

	dma_rxq5_rptr.wrd = read_dma_ctrl_reg(	DMA_SEC_DMA_SSP_Q_RXQ_RPTR);
	rx_desc[dma_rxq5_rptr.bf.index].word1.bf.own = OWN_DMA;
	rx_desc[dma_rxq5_rptr.bf.index].word1.bf.buf_len = mtd->writesize;
	rx_desc[dma_rxq5_rptr.bf.index].buf_adr = (unsigned int)addr;

	/*oob rx desc*/
	addr = (unsigned int *)((unsigned int)addr + mtd->writesize);
	/*printf("  oob : addr(%p)  chip->oob_poi(%p) \n",addr, chip->oob_poi);*/

	dma_rxq5_rptr.bf.index = (dma_rxq5_rptr.bf.index + 1) % FDMA_DESC_NUM;
	rx_desc[dma_rxq5_rptr.bf.index].word1.bf.own = OWN_DMA;
	rx_desc[dma_rxq5_rptr.bf.index].word1.bf.buf_len = mtd->oobsize;
	rx_desc[dma_rxq5_rptr.bf.index].buf_adr = (unsigned int)addr;


	/*update page tx write ptr*/
	dma_txq5_wptr.bf.index = (dma_txq5_wptr.bf.index + 1) % FDMA_DESC_NUM;
	write_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_WPTR, dma_txq5_wptr.wrd);
	/*set axi_bus_len = 8
	set fifo control*/
	//nf_fifo_ctl.wrd = 0;
	//nf_fifo_ctl.bf.fifoCmd = FLASH_WT;
	//write_flash_ctrl_reg(FLASH_NF_FIFO_CONTROL, nf_fifo_ctl.wrd);

	flash_fifo_ctrl.wrd = read_flash_ctrl_reg(FLASH_FIFO_CONTROL);
	flash_fifo_ctrl.bf.fifoCmd = FIFO_CMD_WRITE;
	write_flash_ctrl_reg(FLASH_FIFO_CONTROL, flash_fifo_ctrl.wrd);

	access_start.wrd = 0;
	access_start.bf.nflashFifoReq = FLASH_GO;
	/*access_start.bf.nflashRegCmd = FLASH_WT;*/
	write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, access_start.wrd);

	/*enable txq5*/
	dma_txq5_ctrl.bf.txq_en = 1;
	write_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_CONTROL, dma_txq5_ctrl.wrd);

	dma_ssp_rxq5_intsts.wrd = read_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_RXQ_COAL_INTERRUPT);
	tmp_dma_ssp_rxq5_intsts.wrd = 0;
	tmp_dma_ssp_rxq5_intsts.bf.rxq_eof = 1;


	reg_wait(DMA_SEC_DMA_SSP_Q_RXQ_COAL_INTERRUPT, tmp_dma_ssp_rxq5_intsts.wrd , tmp_dma_ssp_rxq5_intsts.wrd, 1000);


	dma_ssp_txq5_intsts.wrd = read_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_COAL_INTERRUPT);
	tmp_dma_ssp_txq5_intsts.wrd = 0;
	tmp_dma_ssp_txq5_intsts.bf.txq_eof = 1;

	reg_wait(DMA_SEC_DMA_SSP_Q_TXQ_COAL_INTERRUPT, tmp_dma_ssp_txq5_intsts.wrd , tmp_dma_ssp_txq5_intsts.wrd, 1000);

	/*clr tx/rx eof*/

	dma_ssp_txq5_intsts.bf.txq_eof = 0;
	write_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_COAL_INTERRUPT, dma_ssp_txq5_intsts.wrd);

	dma_ssp_rxq5_intsts.bf.rxq_eof = 0;
	write_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_RXQ_COAL_INTERRUPT, dma_ssp_rxq5_intsts.wrd);



#if defined( CONFIG_PNAND_ECC_HW_BCH)
		nf_bch_sts.wrd=read_flash_ctrl_reg(FLASH_NF_BCH_STATUS);
		tmp_bsc_sts.wrd = 0;
		tmp_bsc_sts.bf.bchGenDone = 1;

		reg_wait(FLASH_NF_BCH_STATUS, tmp_bsc_sts.wrd , tmp_bsc_sts.wrd, 1000);

		udelay(10);

#else



		nf_ecc_sts.wrd=read_flash_ctrl_reg(FLASH_NF_ECC_STATUS);
		tmp_ecc_sts.wrd = 0;
		tmp_ecc_sts.bf.eccDone = 1;

		reg_wait(FLASH_NF_ECC_STATUS, tmp_ecc_sts.wrd , tmp_ecc_sts.wrd, 1000);

		udelay(10);

	nf_ecc_ctl.wrd = read_flash_ctrl_reg( FLASH_NF_ECC_CONTROL);
	nf_ecc_ctl.bf.eccEn= 0;
	write_flash_ctrl_reg(FLASH_NF_ECC_CONTROL, nf_ecc_ctl.wrd);  /*disable ecc gen*/

#endif
	/*printf("write page ecc(page %x) : ", page);*/

	for (i = 0,j = 0; eccsteps; eccsteps--, i++, j += eccbytes)
	{
#if defined( CONFIG_PNAND_ECC_HW_BCH)

			reg_bch_gen = read_flash_ctrl_reg(FLASH_NF_BCH_GEN0_0 + 72*i);
			chip->oob_poi[eccpos[j]] = reg_bch_gen & 0xff;
			chip->oob_poi[eccpos[j+1]] = (reg_bch_gen >> 8) & 0xff;
			chip->oob_poi[eccpos[j+2]] = (reg_bch_gen >> 16) & 0xff;
			chip->oob_poi[eccpos[j+3]] = (reg_bch_gen >> 24) & 0xff;
			reg_bch_gen = read_flash_ctrl_reg(FLASH_NF_BCH_GEN0_1 + 72*i);
			chip->oob_poi[eccpos[j+4]] = reg_bch_gen & 0xff;
			chip->oob_poi[eccpos[j+5]] = (reg_bch_gen >> 8) & 0xff;
			chip->oob_poi[eccpos[j+6]] = (reg_bch_gen >> 16) & 0xff;
			chip->oob_poi[eccpos[j+7]] = (reg_bch_gen >> 24) & 0xff;
			reg_bch_gen = read_flash_ctrl_reg(FLASH_NF_BCH_GEN0_2 + 72*i);
			chip->oob_poi[eccpos[j+8]] = reg_bch_gen & 0xff;
			chip->oob_poi[eccpos[j+9]] = (reg_bch_gen >> 8) & 0xff;
			chip->oob_poi[eccpos[j+10]] = (reg_bch_gen >> 16) & 0xff;
			chip->oob_poi[eccpos[j+11]] = (reg_bch_gen >> 24) & 0xff;
			reg_bch_gen = read_flash_ctrl_reg(FLASH_NF_BCH_GEN0_3 + 72*i);
			chip->oob_poi[eccpos[j+12]] = reg_bch_gen & 0xff;
			chip->oob_poi[eccpos[j+13]] = (reg_bch_gen >> 8 ) & 0xff;
#if (CONFIG_PNAND_ECC_HW_BCH_LEVEL > 8)
			chip->oob_poi[eccpos[j+14]] = (reg_bch_gen >> 16) & 0xff;
			chip->oob_poi[eccpos[j+15]] = (reg_bch_gen >> 24)& 0xff;
			reg_bch_gen = read_flash_ctrl_reg( FLASH_NF_BCH_GEN0_4 + 72*i);
			chip->oob_poi[eccpos[j+16]] = reg_bch_gen & 0xff;
			chip->oob_poi[eccpos[j+17]] = (reg_bch_gen >> 8 ) & 0xff;
			chip->oob_poi[eccpos[j+18]] = (reg_bch_gen >> 16) & 0xff;
			chip->oob_poi[eccpos[j+19]] = (reg_bch_gen >> 24)& 0xff;
			reg_bch_gen = read_flash_ctrl_reg( FLASH_NF_BCH_GEN0_5 + 72*i);
			chip->oob_poi[eccpos[j+20]] = reg_bch_gen & 0xff;
			chip->oob_poi[eccpos[j+21]] = (reg_bch_gen >> 8 ) & 0xff;
			chip->oob_poi[eccpos[j+22]] = (reg_bch_gen >> 16) & 0xff;
			chip->oob_poi[eccpos[j+23]] = (reg_bch_gen >> 24)& 0xff;
			reg_bch_gen = read_flash_ctrl_reg( FLASH_NF_BCH_GEN0_6 + 72*i);
			chip->oob_poi[eccpos[j+24]] = reg_bch_gen & 0xff;
			chip->oob_poi[eccpos[j+25]] = (reg_bch_gen >> 8 ) & 0xff;
			chip->oob_poi[eccpos[j+26]] = (reg_bch_gen >> 16) & 0xff;
			chip->oob_poi[eccpos[j+27]] = (reg_bch_gen >> 24)& 0xff;

  #endif
 #if (CONFIG_PNAND_ECC_HW_BCH_LEVEL > 16)
			reg_bch_gen = read_flash_ctrl_reg( FLASH_NF_BCH_GEN0_7 + 72*i);
			chip->oob_poi[eccpos[j+28]] = reg_bch_gen & 0xff;
			chip->oob_poi[eccpos[j+29]] = (reg_bch_gen >> 8 ) & 0xff;
			chip->oob_poi[eccpos[j+30]] = (reg_bch_gen >> 16) & 0xff;
			chip->oob_poi[eccpos[j+31]] = (reg_bch_gen >> 24)& 0xff;
			reg_bch_gen = read_flash_ctrl_reg( FLASH_NF_BCH_GEN0_8 + 72*i);
			chip->oob_poi[eccpos[j+32]] = reg_bch_gen & 0xff;
			chip->oob_poi[eccpos[j+33]] = (reg_bch_gen >> 8 ) & 0xff;
			chip->oob_poi[eccpos[j+34]] = (reg_bch_gen >> 16) & 0xff;
			chip->oob_poi[eccpos[j+35]] = (reg_bch_gen >> 24)& 0xff;
			reg_bch_gen = read_flash_ctrl_reg( FLASH_NF_BCH_GEN0_9 + 72*i);
			chip->oob_poi[eccpos[j+36]] = reg_bch_gen & 0xff;
			chip->oob_poi[eccpos[j+37]] = (reg_bch_gen >> 8 ) & 0xff;
			chip->oob_poi[eccpos[j+38]] = (reg_bch_gen >> 16) & 0xff;
			chip->oob_poi[eccpos[j+39]] = (reg_bch_gen >> 24)& 0xff;
			reg_bch_gen = read_flash_ctrl_reg(FLASH_NF_BCH_GEN0_10 + 72*i);
			chip->oob_poi[eccpos[j+40]] = reg_bch_gen & 0xff;
			chip->oob_poi[eccpos[j+41]] = (reg_bch_gen >> 8 ) & 0xff;
  #endif
 #if (CONFIG_PNAND_ECC_HW_BCH_LEVEL > 24)
			chip->oob_poi[eccpos[j+42]] = (reg_bch_gen >> 16) & 0xff;
			chip->oob_poi[eccpos[j+43]] = (reg_bch_gen >> 24)& 0xff;
			reg_bch_gen = read_flash_ctrl_reg( FLASH_NF_BCH_GEN0_11 + 72*i);
			chip->oob_poi[eccpos[j+44]] = reg_bch_gen & 0xff;
			chip->oob_poi[eccpos[j+45]] = (reg_bch_gen >> 8 ) & 0xff;
			chip->oob_poi[eccpos[j+46]] = (reg_bch_gen >> 16) & 0xff;
			chip->oob_poi[eccpos[j+47]] = (reg_bch_gen >> 24)& 0xff;
			reg_bch_gen = read_flash_ctrl_reg( FLASH_NF_BCH_GEN0_12 + 72*i);
			chip->oob_poi[eccpos[j+48]] = reg_bch_gen & 0xff;
			chip->oob_poi[eccpos[j+49]] = (reg_bch_gen >> 8 ) & 0xff;
			chip->oob_poi[eccpos[j+50]] = (reg_bch_gen >> 16) & 0xff;
			chip->oob_poi[eccpos[j+51]] = (reg_bch_gen >> 24)& 0xff;
			reg_bch_gen = read_flash_ctrl_reg( FLASH_NF_BCH_GEN0_13 + 72*i);
			chip->oob_poi[eccpos[j+52]] = reg_bch_gen & 0xff;
			chip->oob_poi[eccpos[j+53]] = (reg_bch_gen >> 8 ) & 0xff;
			chip->oob_poi[eccpos[j+54]] = (reg_bch_gen >> 16) & 0xff;
			chip->oob_poi[eccpos[j+55]] = (reg_bch_gen >> 24)& 0xff;
			reg_bch_gen = read_flash_ctrl_reg( FLASH_NF_BCH_GEN0_14 + 72*i);
			chip->oob_poi[eccpos[j+56]] = reg_bch_gen & 0xff;
			chip->oob_poi[eccpos[j+57]] = (reg_bch_gen >> 8 ) & 0xff;
			chip->oob_poi[eccpos[j+58]] = (reg_bch_gen >> 16) & 0xff;
			chip->oob_poi[eccpos[j+59]] = (reg_bch_gen >> 24)& 0xff;
			reg_bch_gen = read_flash_ctrl_reg( FLASH_NF_BCH_GEN0_15 + 72*i);
			chip->oob_poi[eccpos[j+60]] = reg_bch_gen & 0xff;
			chip->oob_poi[eccpos[j+61]] = (reg_bch_gen >> 8 ) & 0xff;
			chip->oob_poi[eccpos[j+62]] = (reg_bch_gen >> 16) & 0xff;
			chip->oob_poi[eccpos[j+63]] = (reg_bch_gen >> 24)& 0xff;
			reg_bch_gen = read_flash_ctrl_reg( FLASH_NF_BCH_GEN0_16 + 72*i);
			chip->oob_poi[eccpos[j+64]] = reg_bch_gen & 0xff;
			chip->oob_poi[eccpos[j+65]] = (reg_bch_gen >> 8 ) & 0xff;
			chip->oob_poi[eccpos[j+66]] = (reg_bch_gen >> 16) & 0xff;
			chip->oob_poi[eccpos[j+67]] = (reg_bch_gen >> 24)& 0xff;
			reg_bch_gen = read_flash_ctrl_reg( FLASH_NF_BCH_GEN0_17 + 72*i);
			chip->oob_poi[eccpos[j+68]] = reg_bch_gen & 0xff;
			chip->oob_poi[eccpos[j+69]] = (reg_bch_gen >> 8 ) & 0xff;

  #endif
#else

		nf_ecc_gen0.wrd = read_flash_ctrl_reg(FLASH_NF_ECC_GEN0 + 4*i);
		chip->oob_poi[eccpos[j]] = nf_ecc_gen0.wrd & 0xff;
		chip->oob_poi[eccpos[j+1]] = (nf_ecc_gen0.wrd >> 8) & 0xff;
		chip->oob_poi[eccpos[j+2]] = (nf_ecc_gen0.wrd >> 16) & 0xff;
		/* printf("%x ", nf_ecc_gen0.wrd);		*/
#endif
	}
	/*printf("\n");*/

#if defined(CONFIG_PNAND_ECC_HW_BCH)
	nf_bch_ctrl.wrd = read_flash_ctrl_reg(FLASH_NF_BCH_CONTROL);  /*disable ecc gen*/
		nf_bch_ctrl.bf.bchEn = BCH_DISABLE;
		write_flash_ctrl_reg(FLASH_NF_BCH_CONTROL, nf_bch_ctrl.wrd);

	/* jenfeng clear erase tag*/
	*(chip->oob_poi+ chip->ecc.layout->oobfree[0].offset +  chip->ecc.layout->oobfree[0].length)= 0;
#endif

	/*dma_map_single( NULL, (void *)chip->oob_poi, mtd->oobsize, DMA_TO_DEVICE);

	dma_txq5_wptr.wrd = read_dma_ctrl_reg(DMA_DMA_SSP_TXQ5_WPTR);
	*/
	tx_desc[dma_txq5_wptr.bf.index].word1.bf.own = OWN_DMA;
	tx_desc[dma_txq5_wptr.bf.index].word1.bf.buf_len = mtd->oobsize;
	tx_desc[dma_txq5_wptr.bf.index].buf_adr = (unsigned int)(chip->oob_poi);

	/*dma_cache_sync(NULL, chip->oob_poi, mtd->oobsize, DMA_BIDIRECTIONAL);
	update tx write ptr
	*/
	dma_txq5_wptr.bf.index = (dma_txq5_wptr.bf.index + 1) % FDMA_DESC_NUM;
	write_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_WPTR, dma_txq5_wptr.wrd);

	dma_ssp_rxq5_intsts.wrd = read_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_RXQ_COAL_INTERRUPT);
	tmp_dma_ssp_rxq5_intsts.wrd = 0;
	tmp_dma_ssp_rxq5_intsts.bf.rxq_eof = 1;


	reg_wait(DMA_SEC_DMA_SSP_Q_RXQ_COAL_INTERRUPT, tmp_dma_ssp_rxq5_intsts.wrd , tmp_dma_ssp_rxq5_intsts.wrd, 1000);

	dma_ssp_txq5_intsts.wrd = read_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_COAL_INTERRUPT);
	tmp_dma_ssp_txq5_intsts.wrd = 0;
	tmp_dma_ssp_txq5_intsts.bf.txq_eof = 1;

	reg_wait(DMA_SEC_DMA_SSP_Q_TXQ_COAL_INTERRUPT, tmp_dma_ssp_txq5_intsts.wrd , tmp_dma_ssp_txq5_intsts.wrd, 1000);

	access_start.wrd=read_flash_ctrl_reg(FLASH_FLASH_ACCESS_START);

	tmp_access.wrd = 0;
	tmp_access.bf.nflashFifoReq = 1;
	reg_wait(FLASH_FLASH_ACCESS_START, tmp_access.wrd, 0, 1000);

	/*update rx read ptr
	dma_rxq5_rptr.wrd = read_dma_ctrl_reg(DMA_DMA_SSP_RXQ5_RPTR);
	*/
	dma_rxq5_rptr.bf.index = (dma_rxq5_rptr.bf.index + 1) % FDMA_DESC_NUM;
	write_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_RXQ_RPTR, dma_rxq5_rptr.wrd);



	/*chip->write_buf(mtd, chip->oob_poi, mtd->oobsize);*/
	printf("%s(%d) finished!!!\n", __func__, __LINE__);

}


/**
 * pnand_read_page_hwecc - [REPLACABLE] hardware ecc based page read function
 * @mtd:	mtd info structure
 * @chip:	nand chip info structure
 * @buf:	buffer to store read data
 * @page:	page number to read
 *
 * Not for syndrome calculating ecc controllers which need a special oob layout
 */
 int pnand_read_page_hwecc(struct mtd_info *mtd, struct nand_chip *chip,
				uint8_t *buf, int oob_required, int page)
{
	int i, eccsize = chip->ecc.size, col;
	int eccbytes = chip->ecc.bytes;
	int eccsteps = chip->ecc.steps;
	uint8_t *p = buf;
	/*uint8_t *ecc_calc = chip->buffers->ecccalc;*/
	uint8_t *ecc_code = chip->buffers->ecccode;
	uint32_t *eccpos = chip->ecc.layout->eccpos, *addr;
#if defined(CONFIG_PNAND_TEST_DRIVER)
	struct pnand_host *host = nand_chip->priv;
#else
	struct pnand_host *host = mtd_to_nand(nand_chip);
#endif
	DMA_SEC_DMA_SSP_Q_RXQ_COAL_INTERRUPT_t	tmp_dma_ssp_rxq5_intsts;
	DMA_SEC_DMA_SSP_Q_TXQ_COAL_INTERRUPT_t	tmp_dma_ssp_txq5_intsts;
#if defined(CONFIG_PNAND_ECC_HW_BCH)
	FLASH_NF_BCH_STATUS_t	tmp_bsc_sts;
#else
	FLASH_NF_ECC_STATUS_t	tmp_ecc_sts;
#endif
	FLASH_FIFO_CONTROL_t	flash_fifo_ctrl;
	//FLASH_NF_ECC_RESET_t ecc_rst;


	check_flash_ctrl_status();

	col  = host->col_addr;
	p = buf;
	nf_ecc_reset.wrd = 0;
	nf_ecc_reset.bf.eccClear = ECC_CLR;
	write_flash_ctrl_reg(FLASH_NF_ECC_RESET, nf_ecc_reset.wrd);

	flash_int_sts.bf.regIrq = 1;
	write_flash_ctrl_reg(FLASH_FLASH_INTERRUPT, flash_int_sts.wrd);

	nf_ecc_reset.wrd = 0;
	nf_ecc_reset.bf.eccClear = 1;
	write_flash_ctrl_reg(FLASH_NF_ECC_RESET, nf_ecc_reset.wrd);

#ifdef CONFIG_PNAND_ECC_HW_BCH
		nf_bch_ctrl.wrd = 0;
		nf_bch_ctrl.bf.bchEn = BCH_ENABLE;
		nf_bch_ctrl.bf.bchOpcode = BCH_DECODE;
#if	(CONFIG_PNAND_ECC_HW_BCH_LEVEL == 8)
			nf_bch_ctrl.bf.bchErrCap = BCH_ERR_CAP_8;
#elif (CONFIG_PNAND_ECC_HW_BCH_LEVEL == 16)
			nf_bch_ctrl.bf.bchErrCap = BCH_ERR_CAP_16;
#elif (CONFIG_PNAND_ECC_HW_BCH_LEVEL == 24)
			nf_bch_ctrl.bf.bchErrCap = BCH_ERR_CAP_24;
#elif (CONFIG_PNAND_ECC_HW_BCH_LEVEL == 40)
			nf_bch_ctrl.bf.bchErrCap = BCH_ERR_CAP_40;
#else
			printf("BCH Level is not recognized! %s:%d\n",__func__, __LINE__);
#endif
		write_flash_ctrl_reg(FLASH_NF_BCH_CONTROL, nf_bch_ctrl.wrd);

#else

		nf_ecc_ctl.wrd = 0;
	if((eccsize-1) == NF_CNT_512P_DATA)
			nf_ecc_ctl.bf.eccGenMode = ECC_GEN_512;
	else
			nf_ecc_ctl.bf.eccGenMode = ECC_GEN_256;
		nf_ecc_ctl.bf.eccEn = ECC_ENABLE;
		write_flash_ctrl_reg(FLASH_NF_ECC_CONTROL, nf_ecc_ctl.wrd);
#endif



	/*disable txq5*/
	dma_txq5_ctrl.bf.txq_en = 0;
	write_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_CONTROL, dma_txq5_ctrl.wrd);

	dma_ssp_txq5_intsts.wrd = read_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_COAL_INTERRUPT);
	dma_ssp_txq5_intsts.bf.txq_eof = 0;
	write_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_COAL_INTERRUPT, dma_ssp_txq5_intsts.wrd);
	dma_ssp_rxq5_intsts.wrd = read_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_RXQ_COAL_INTERRUPT);
	dma_ssp_rxq5_intsts.bf.rxq_eof = 0;
	write_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_RXQ_COAL_INTERRUPT, dma_ssp_rxq5_intsts.wrd);

	/*for indirect access with DMA, because DMA not ready	*/
	nf_count.wrd = 0;
	nf_command.wrd = 0;
	nf_addr1.wrd = 0;
	nf_addr2.wrd = 0;
	nf_count.wrd = 0;
	nf_count.bf.nflashRegOobCount = mtd->oobsize - 1;
	nf_count.bf.nflashRegDataCount = mtd->writesize-1;

	if(chip->chipsize < (32 << 20))
	{
	nf_count.bf.nflashRegAddrCount = NF_CNT_ADDR_3;
		nf_count.bf.nflashRegCmdCount = NF_CNT_CMD_1;
		nf_command.bf.nflashRegCmd0 = NAND_CMD_READ0;
		nf_addr1.wrd = (((page & 0x00ffffff)<<8));
		nf_addr2.wrd = ((page & 0xff000000)>>24);
	}
	else if((chip->chipsize >= (32 << 20)) && (chip->chipsize <= (128 << 20)))
	{
		nf_count.bf.nflashRegAddrCount = NF_CNT_ADDR_4;
		nf_count.bf.nflashRegCmdCount = NF_CNT_CMD_1;
		nf_command.bf.nflashRegCmd0 = NAND_CMD_READ0;

		if(mtd->writesize > NF_CNT_512P_DATA) {
			nf_count.bf.nflashRegCmdCount = NF_CNT_CMD_2;
			nf_command.bf.nflashRegCmd1 = NAND_CMD_READSTART;
		}
		nf_addr1.wrd = (((page & 0xffff)<<16));
		nf_addr2.wrd = ((page & 0xffff0000)>>16);
	}
	else /*if((chip->chipsize > (128 << 20)) ))*/
	{
		nf_count.bf.nflashRegAddrCount = NF_CNT_ADDR_5;
		nf_count.bf.nflashRegCmdCount = NF_CNT_CMD_2;
		nf_command.bf.nflashRegCmd0 = NAND_CMD_READ0;
		nf_command.bf.nflashRegCmd1 = NAND_CMD_READSTART;
		nf_addr1.wrd = (((page & 0xffff)<<16));
		nf_addr2.wrd = ((page & 0xffff0000)>>16);
	}


	write_flash_ctrl_reg(FLASH_NF_COUNT, nf_count.wrd);
	write_flash_ctrl_reg(FLASH_NF_COMMAND, nf_command.wrd); /*write read id command*/
	write_flash_ctrl_reg(FLASH_NF_ADDRESS_1, nf_addr1.wrd); /*write address 0x0*/
	write_flash_ctrl_reg(FLASH_NF_ADDRESS_2, nf_addr2.wrd);


	nf_access.wrd = 0;
	nf_access.bf.nflashCeAlt = NF_ACCESS_CE_ALT;
	nf_access.bf.nflashRegWidth = NF_ACCESS_WiDTH8;
	/*	write_flash_ctrl_reg(FLASH_NF_ACCESS, nf_access.wrd);
	prepare dma descriptor
	nf_access.wrd = read_flash_ctrl_reg(FLASH_FLASH_ACCESS_START);
	*/
	nf_access.bf.nflashExtAddr = ((page << chip->page_shift) / 0x8000000);
	write_flash_ctrl_reg(FLASH_NF_ACCESS, nf_access.wrd);

	addr =  (unsigned int *)((page << chip->page_shift) % 0x8000000);
	addr = (unsigned int *)((unsigned int)addr + (unsigned int)(CONFIG_SYS_FLASH_BASE) );

	/*printf("%s : addr : %p  buf: %p",__func__, addr, buf);
	chip->buffers->databuf
	page tx data desc
	*/
	dma_txq5_wptr.wrd = read_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_WPTR);

	tx_desc[dma_txq5_wptr.bf.index].word1.bf.own = OWN_DMA;
	tx_desc[dma_txq5_wptr.bf.index].word1.bf.buf_len = mtd->writesize;
	tx_desc[dma_txq5_wptr.bf.index].buf_adr = (unsigned int)addr;

	/*oob tx desc*/
	dma_txq5_wptr.bf.index = (dma_txq5_wptr.bf.index + 1) % FDMA_DESC_NUM;

	addr = (unsigned int *)((unsigned int)addr + mtd->writesize);
	/*printf("   oob : addr (%p)  chip->oob_poi (%p) \n", addr, chip->oob_poi);*/
	tx_desc[dma_txq5_wptr.bf.index].word1.bf.own = OWN_DMA;
	tx_desc[dma_txq5_wptr.bf.index].word1.bf.buf_len = mtd->oobsize;
	tx_desc[dma_txq5_wptr.bf.index].buf_adr = (unsigned int)addr;

	/*page data rx desc*/
	dma_rxq5_rptr.wrd = read_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_RXQ_RPTR);
	rx_desc[dma_rxq5_rptr.bf.index].word1.bf.own = OWN_DMA;
	rx_desc[dma_rxq5_rptr.bf.index].word1.bf.buf_len = mtd->writesize;

	rx_desc[dma_rxq5_rptr.bf.index].buf_adr = (unsigned int)(buf);

	/*oob rx desc*/
	dma_rxq5_rptr.bf.index = (dma_rxq5_rptr.bf.index + 1) % FDMA_DESC_NUM;
	rx_desc[dma_rxq5_rptr.bf.index].word1.bf.own = OWN_DMA;
	rx_desc[dma_rxq5_rptr.bf.index].word1.bf.buf_len = mtd->oobsize;
	rx_desc[dma_rxq5_rptr.bf.index].buf_adr =  (unsigned int)(chip->oob_poi);


	/*dma_map_single( NULL, (void *)chip->oob_poi, mtd->oobsize, DMA_FROM_DEVICE);
	dma_map_single( NULL, (void *)buf, mtd->writesize, DMA_FROM_DEVICE);


	set axi_bus_len = 8

	set fifo control
	*/
	//nf_fifo_ctl.wrd = 0;
	//nf_fifo_ctl.bf.fifoCmd = FLASH_RD;
	//write_flash_ctrl_reg(FLASH_NF_FIFO_CONTROL, nf_fifo_ctl.wrd);

	flash_fifo_ctrl.wrd = read_flash_ctrl_reg(FLASH_FIFO_CONTROL);
	flash_fifo_ctrl.bf.fifoCmd = FIFO_CMD_READ;
	write_flash_ctrl_reg(FLASH_FIFO_CONTROL, flash_fifo_ctrl.wrd);

	access_start.wrd = 0;
	access_start.bf.nflashFifoReq = FLASH_GO;
	/*access_start.bf.nflashRegCmd = FLASH_RD;*/
	write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, access_start.wrd);

	/*update tx write ptr*/
	dma_txq5_wptr.bf.index = (dma_txq5_wptr.bf.index + 1) % FDMA_DESC_NUM;
	write_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_WPTR, dma_txq5_wptr.wrd);
	/*dma_txq5_wptr.wrd = read_dma_ctrl_reg(DMA_DMA_SSP_TXQ5_WPTR);

	enable txq5
	*/
	dma_txq5_ctrl.bf.txq_en = 1;
	write_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_CONTROL, dma_txq5_ctrl.wrd);

	tmp_dma_ssp_rxq5_intsts.wrd = 0;
	tmp_dma_ssp_rxq5_intsts.bf.rxq_eof = 1;


	reg_wait(DMA_SEC_DMA_SSP_Q_RXQ_COAL_INTERRUPT, tmp_dma_ssp_rxq5_intsts.wrd , tmp_dma_ssp_rxq5_intsts.wrd, 1000);

	tmp_dma_ssp_txq5_intsts.wrd = 0;
	tmp_dma_ssp_txq5_intsts.bf.txq_eof = 1;

	reg_wait(DMA_SEC_DMA_SSP_Q_TXQ_COAL_INTERRUPT, tmp_dma_ssp_txq5_intsts.wrd , tmp_dma_ssp_txq5_intsts.wrd, 1000);

	tmp_access.wrd = 0;
	tmp_access.bf.nflashFifoReq = 1;
	reg_wait(FLASH_FLASH_ACCESS_START, tmp_access.wrd, 0, 1000);


/**
	nf_ecc_sts.wrd=read_flash_ctrl_reg(FLASH_NF_ECC_STATUS);
	while(!nf_ecc_sts.bf.eccDone)
	{
		nf_ecc_sts.wrd=read_flash_ctrl_reg(FLASH_NF_ECC_STATUS);
		udelay(1);

	}

	write_flash_ctrl_reg(FLASH_NF_ECC_CONTROL, 0);




	dma_cache_sync(NULL, buf, mtd->writesize, DMA_BIDIRECTIONAL);
	dma_cache_sync(NULL, chip->oob_poi, mtd->oobsize, DMA_BIDIRECTIONAL);
	update rx read ptr
**/
	dma_rxq5_rptr.bf.index = (dma_rxq5_rptr.bf.index + 1) % FDMA_DESC_NUM;
	write_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_RXQ_RPTR, dma_rxq5_rptr.wrd);


#if defined(CONFIG_PNAND_ECC_HW_BCH)
		nf_bch_sts.wrd=read_flash_ctrl_reg(FLASH_NF_BCH_STATUS);

		tmp_bsc_sts.wrd = 0;
		tmp_bsc_sts.bf.bchGenDone = 1;

		reg_wait(FLASH_NF_BCH_STATUS, tmp_bsc_sts.wrd , tmp_bsc_sts.wrd, 1000);

		/**write_flash_ctrl_reg(FLASH_NF_BCH_CONTROL, 0);  **/

#else
		nf_ecc_sts.wrd=read_flash_ctrl_reg(FLASH_NF_ECC_STATUS);


		tmp_ecc_sts.wrd = 0;
		tmp_ecc_sts.bf.eccDone = 1;

		reg_wait(FLASH_NF_ECC_STATUS, tmp_ecc_sts.wrd , tmp_ecc_sts.wrd, 1000);


		nf_ecc_ctl.wrd = read_flash_ctrl_reg(FLASH_NF_ECC_CONTROL );  /**disable ecc gen**/
		nf_ecc_ctl.bf.eccEn = 0;
		write_flash_ctrl_reg(FLASH_NF_ECC_CONTROL, nf_ecc_ctl.wrd);  /**disable ecc gen**/
#endif
	/**dma_cache_maint((void *)chip->oob_poi, mtd->oobsize, DMA_FROM_DEVICE);
	dma_cache_maint((void *)buf, mtd->writesize, DMA_FROM_DEVICE);
	**/


#if defined(CONFIG_PNAND_ECC_HW_BCH)
		/** jenfeng**/
	if( 0xff ==  *(chip->oob_poi+ chip->ecc.layout->oobfree[0].offset +  chip->ecc.layout->oobfree[0].length)){
			/** Erase tga is on , No needs to check. **/
			goto BCH_EXIT;
		}

#endif
	for (i = 0; i < chip->ecc.total; i++)
		ecc_code[i] = chip->oob_poi[eccpos[i]];

	for (i = 0 ; eccsteps; eccsteps--, i += eccbytes, p += eccsize) {

#if defined(CONFIG_PNAND_ECC_HW_BCH)
			int j; /** (i/eccbytes);**/
			unsigned int reg_bch_oob;

#if 1
			reg_bch_oob = ecc_code[i]   | ecc_code[i+1]<<8 | ecc_code[i+2]<<16  | ecc_code[i+3]<<24;
			write_flash_ctrl_reg(FLASH_NF_BCH_OOB0, reg_bch_oob);

			reg_bch_oob = ecc_code[i+4] | ecc_code[i+5]<<8 | ecc_code[i+6]<<16  | ecc_code[i+7]<<24;
			write_flash_ctrl_reg(FLASH_NF_BCH_OOB1, reg_bch_oob);

			reg_bch_oob = ecc_code[i+8] | ecc_code[i+9]<<8 | ecc_code[i+10]<<16 | ecc_code[i+11]<<24;
			write_flash_ctrl_reg(FLASH_NF_BCH_OOB2, reg_bch_oob);

			reg_bch_oob = ecc_code[i+12] |ecc_code[i+13]<<8 ;
			write_flash_ctrl_reg(FLASH_NF_BCH_OOB3, reg_bch_oob);

#if (CONFIG_PNAND_ECC_HW_BCH_LEVEL > 8)
				reg_bch_oob = ecc_code[i+12] | ecc_code[i+13]<<8 | ecc_code[i+14]<<16 | ecc_code[i+15]<<24;
				write_flash_ctrl_reg(FLASH_NF_BCH_OOB3, reg_bch_oob);

				reg_bch_oob = ecc_code[i+16] | ecc_code[i+17]<<8 | ecc_code[i+18]<<16 | ecc_code[i+19]<<24;
				write_flash_ctrl_reg(FLASH_NF_BCH_OOB4, reg_bch_oob);

				reg_bch_oob = ecc_code[i+20] | ecc_code[i+21]<<8 | ecc_code[i+22]<<16 | ecc_code[i+23]<<24;
				write_flash_ctrl_reg(FLASH_NF_BCH_OOB5, reg_bch_oob);

				reg_bch_oob = ecc_code[i+24] | ecc_code[i+25]<<8 | ecc_code[i+26]<<16 | ecc_code[i+27]<<24;
				write_flash_ctrl_reg(FLASH_NF_BCH_OOB6, reg_bch_oob);
#endif
#if (CONFIG_PNAND_ECC_HW_BCH_LEVEL > 16)
				reg_bch_oob = ecc_code[i+28] | ecc_code[i+29]<<8 | ecc_code[i+30]<<16 | ecc_code[i+31]<<24;
				write_flash_ctrl_reg(FLASH_NF_BCH_OOB7, reg_bch_oob);

				reg_bch_oob = ecc_code[i+32] | ecc_code[i+33]<<8 | ecc_code[i+34]<<16 | ecc_code[i+35]<<24;
				write_flash_ctrl_reg(FLASH_NF_BCH_OOB8, reg_bch_oob);

				reg_bch_oob = ecc_code[i+36] | ecc_code[i+37]<<8 | ecc_code[i+38]<<16 | ecc_code[i+39]<<24;
				write_flash_ctrl_reg(FLASH_NF_BCH_OOB9, reg_bch_oob);

				reg_bch_oob = ecc_code[i+40] | ecc_code[i+41]<<8 ;
				write_flash_ctrl_reg(FLASH_NF_BCH_OOB10, reg_bch_oob);
#endif
#if (CONFIG_PNAND_ECC_HW_BCH_LEVEL > 24)
				reg_bch_oob = ecc_code[i+40] | ecc_code[i+41]<<8 | ecc_code[i+42]<<16 | ecc_code[i+43]<<24;
				write_flash_ctrl_reg(FLASH_NF_BCH_OOB10, reg_bch_oob);

				reg_bch_oob = ecc_code[i+44] | ecc_code[i+45]<<8 | ecc_code[i+46]<<16 | ecc_code[i+47]<<24;
				write_flash_ctrl_reg(FLASH_NF_BCH_OOB11, reg_bch_oob);

				reg_bch_oob = ecc_code[i+48] | ecc_code[i+49]<<8 | ecc_code[i+50]<<16 | ecc_code[i+51]<<24;
				write_flash_ctrl_reg(FLASH_NF_BCH_OOB12, reg_bch_oob);

				reg_bch_oob = ecc_code[i+52] | ecc_code[i+53]<<8 | ecc_code[i+54]<<16 | ecc_code[i+55]<<24;
				write_flash_ctrl_reg(FLASH_NF_BCH_OOB13, reg_bch_oob);

				reg_bch_oob = ecc_code[i+56] | ecc_code[i+57]<<8 | ecc_code[i+58]<<16 | ecc_code[i+59]<<24;
				write_flash_ctrl_reg(FLASH_NF_BCH_OOB14, reg_bch_oob);

				reg_bch_oob = ecc_code[i+60] | ecc_code[i+61]<<8 | ecc_code[i+62]<<16 | ecc_code[i+63]<<24;
				write_flash_ctrl_reg(FLASH_NF_BCH_OOB15, reg_bch_oob);

				reg_bch_oob = ecc_code[i+64] | ecc_code[i+65]<<8 | ecc_code[i+66]<<16 | ecc_code[i+67]<<24;
				write_flash_ctrl_reg(FLASH_NF_BCH_OOB16, reg_bch_oob);

				reg_bch_oob = ecc_code[i+68] | ecc_code[i+69]<<8 ;
				write_flash_ctrl_reg(FLASH_NF_BCH_OOB17, reg_bch_oob);

#endif
#endif

			nf_ecc_reset.wrd = read_flash_ctrl_reg(FLASH_NF_ECC_RESET);
			nf_ecc_reset.bf.eccClear = ECC_CLR;
			write_flash_ctrl_reg(FLASH_NF_ECC_RESET, nf_ecc_reset.wrd);

			nf_ecc_reset.wrd = 0;
			nf_ecc_reset.bf.eccClear= 1;
			udelay(10);
			reg_wait(FLASH_NF_ECC_RESET, nf_ecc_reset.wrd , 0, 1000);


			nf_bch_ctrl.wrd = read_flash_ctrl_reg(FLASH_NF_BCH_CONTROL);
			nf_bch_ctrl.bf.bchEn = 0;
			write_flash_ctrl_reg(FLASH_NF_BCH_CONTROL, nf_bch_ctrl.wrd);

			/** enable ecc compare**/
			nf_bch_ctrl.wrd = read_flash_ctrl_reg(FLASH_NF_BCH_CONTROL);
			nf_bch_ctrl.bf.bchCodeSel = (i/eccbytes);
			nf_bch_ctrl.bf.bchEn = 1;
			write_flash_ctrl_reg(FLASH_NF_BCH_CONTROL, nf_bch_ctrl.wrd);
			udelay(10);
			nf_bch_ctrl.bf.bchCompare = 1;
			write_flash_ctrl_reg(FLASH_NF_BCH_CONTROL, nf_bch_ctrl.wrd);

			tmp_bsc_sts.wrd = 0;
			tmp_bsc_sts.bf.bchDecDone = 1;

			reg_wait(FLASH_NF_BCH_STATUS, tmp_bsc_sts.wrd , tmp_bsc_sts.wrd, 1000);

			/** disable ecc compare**/
			nf_bch_ctrl.wrd = read_flash_ctrl_reg(FLASH_NF_BCH_CONTROL);
			nf_bch_ctrl.bf.bchCompare = 0;
			write_flash_ctrl_reg(FLASH_NF_BCH_CONTROL, nf_bch_ctrl.wrd);

			nf_bch_sts.wrd=read_flash_ctrl_reg(FLASH_NF_BCH_STATUS);

			switch(nf_bch_sts.bf.bchDecStatus)
			{
				case BCH_CORRECTABLE_ERR:
					printf("Correctable error(%x)!! addr:%x\n",nf_bch_sts.bf.bchErrNum, (unsigned int)addr - mtd->writesize);
					printf("Dst buf: %x [ColSel:%x ]\n", buf + nf_bch_ctrl.bf.bchCodeSel * BCH_DATA_UNIT, nf_bch_ctrl.bf.bchCodeSel);


					for(j=0  ;j<((nf_bch_sts.bf.bchErrNum+1)/2);j++)
					{
						nf_bch_err_loc01.wrd = read_flash_ctrl_reg(FLASH_NF_BCH_ERROR_LOC01 + j*4);

						if ( (j+1)*2 <= nf_bch_sts.bf.bchErrNum ) {
							if( ((nf_bch_err_loc01.bf.bchErrLoc1 & 0x3fff)>>3) < BCH_DATA_UNIT) {
								printf("pdata[%x]:%x =>",((i/eccbytes)*chip->ecc.size + ((nf_bch_err_loc01.bf.bchErrLoc1&0x1fff)>>3)), p[(nf_bch_err_loc01.bf.bchErrLoc1&0x1fff)>>3]);
								p[(nf_bch_err_loc01.bf.bchErrLoc1&0x3fff)>>3] ^= (1 << (nf_bch_err_loc01.bf.bchErrLoc1 & 0x07));
								printf("%x\n",p[(nf_bch_err_loc01.bf.bchErrLoc1&0x1fff)>>3]);
							} else {
								/** printf( "BCH bit error [%x]:[%x]\n", (nf_bch_err_loc01.bf.bchErrLoc1&0x1fff)>>3 - 0x200, nf_bch_err_loc01.bf.bchErrLoc1 & 0x07); **/

							}
						}

						if(((nf_bch_err_loc01.bf.bchErrLoc0 & 0x3fff)>>3) < BCH_DATA_UNIT)
						{
							printf("pdata[%x]:%x =>",((i/eccbytes)*chip->ecc.size + ((nf_bch_err_loc01.bf.bchErrLoc0&0x1fff)>>3)), p[(nf_bch_err_loc01.bf.bchErrLoc0&0x1fff)>>3]);
							p[(nf_bch_err_loc01.bf.bchErrLoc0&0x3fff)>>3] ^= (1 << (nf_bch_err_loc01.bf.bchErrLoc0 & 0x07));
							printf("%x\n",p[(nf_bch_err_loc01.bf.bchErrLoc0&0x1fff)>>3 ]);
						} else
						{
							/** printf( "BCH bit error [%x]:[%x]\n", (nf_bch_err_loc01.bf.bchErrLoc0&0x1fff)>>3 - 0x200, nf_bch_err_loc01.bf.bchErrLoc0 & 0x07); **/
						}
					}
					break;
				case BCH_UNCORRECTABLE:
					printf("uncorrectable error!!step %d \n", (i/eccbytes));
					if((i/eccbytes > 2) || (i/eccbytes)==0)
					mtd->ecc_stats.failed++;

					break;
			}
			/** clear compare sts
			nf_bch_sts.bf.bchDecDone = 0;
			write_flash_ctrl_reg(FLASH_NF_BCH_STATUS, nf_bch_sts.wrd);
			**/
			if ( pnand_ecc_check && i == 0 ) {
				unsigned char *free= buf+ mtd->writesize+ mtd->oobsize;
				*free= nf_bch_sts.bf.bchDecStatus;
				*(free+1)= nf_bch_sts.bf.bchErrNum;
			}

/*
			nf_ecc_reset.wrd = read_flash_ctrl_reg(FLASH_NF_ECC_RESET);
			nf_ecc_reset.bf.eccClear = ECC_CLR;
	//		nf_ecc_reset.bf.fifoClear = FIFO_CLR;
			write_flash_ctrl_reg(FLASH_NF_ECC_RESET, nf_ecc_reset.wrd);

			nf_ecc_reset.wrd = 0;
			nf_ecc_reset.bf.eccClear= 1;
			udelay(100);
			reg_wait(FLASH_NF_ECC_RESET, nf_ecc_reset.wrd , 0, 1000);
*/
#else

			/** for (i = 0 ; eccsteps; eccsteps--, i += eccbytes, p += eccsize) { ***/
			nf_ecc_oob.wrd = ecc_code[i] | ecc_code[i+1]<<8 | ecc_code[i+2]<<16;
			write_flash_ctrl_reg(FLASH_NF_ECC_OOB, nf_ecc_oob.wrd);

			nf_ecc_ctl.wrd = read_flash_ctrl_reg(FLASH_NF_ECC_CONTROL);
			nf_ecc_ctl.bf.eccCodeSel = (i/eccbytes);
			write_flash_ctrl_reg(FLASH_NF_ECC_CONTROL, nf_ecc_ctl.wrd);

			nf_ecc_sts.wrd = read_flash_ctrl_reg(FLASH_NF_ECC_STATUS);

			switch(nf_ecc_sts.bf.eccStatus)
			{
				case ECC_NO_ERR:
					/** printf("ECC no error!!\n");**/
					break;
				case ECC_1BIT_DATA_ERR:
					/* flip the bit */
					p[nf_ecc_sts.bf.eccErrByte] ^= (1 << nf_ecc_sts.bf.eccErrBit);
					/** printf("ecc_code- %x (%x) :\n",i ,chip->ecc.total);**/
					nf_ecc_gen0.wrd = read_flash_ctrl_reg(FLASH_NF_ECC_GEN0 + (4*(i/eccbytes)));
					/** for (i = 0; i < chip->ecc.total; i++)
					  printf(" %x", ecc_code[i]);
					**/
					printf("\nECC one bit data error(%x)!!(org: %x) HW(%xs) page(%x)\n", (i/eccbytes),nf_ecc_oob.wrd, nf_ecc_gen0.wrd, page);
					break;
				case ECC_1BIT_ECC_ERR:
					/** printf("ecc_code- %x (%x) :\n",i ,chip->ecc.total);**/
					nf_ecc_gen0.wrd = read_flash_ctrl_reg(FLASH_NF_ECC_GEN0 + (4*(i/eccbytes)));
					/** for (i = 0; i < chip->ecc.total; i++)
					  printf(" %x", ecc_code[i]);
					**/
					printf("\nECC one bit ECC error(%x)!!(org: %x) HW(%xs) page(%x)\n", (i/eccbytes), nf_ecc_oob.wrd, nf_ecc_gen0.wrd, page);
					break;
				case ECC_UNCORRECTABLE:
					mtd->ecc_stats.failed++;
					nf_ecc_gen0.wrd = read_flash_ctrl_reg(FLASH_NF_ECC_GEN0 + (4*(i/eccbytes)));
					/** printf("ecc_code- %x (%x) :\n",i ,chip->ecc.total);
					for (i = 0; i < chip->ecc.total; i++)
					  printf(" %x", ecc_code[i]);
					**/
					printf("\nECC uncorrectable error(%x)!!(org: %x) HW(%xs) page(%x)\n", (i/eccbytes), nf_ecc_oob.wrd, nf_ecc_gen0.wrd, page);
					break;
			}

			if ( pnand_ecc_check && i == 0 ) {
				unsigned char *free= buf+ mtd->writesize+ mtd->oobsize;
				*free=  nf_ecc_sts.bf.eccStatus;
			}

#endif
	} /** for (i = 0 ; eccsteps; eccsteps--, i += eccbytes, p += eccsize) **/

	#if defined(CONFIG_PNAND_ECC_HW_BCH)
BCH_EXIT:
	/** diasble bch **/
	nf_bch_ctrl.wrd = 0;
	write_flash_ctrl_reg(FLASH_NF_BCH_CONTROL, nf_bch_ctrl.wrd);
	#endif

#if 0
	dma_txq5_wptr.wrd = read_dma_ctrl_reg( DMA_DMA_SSP_TXQ5_WPTR );
	dma_rxq5_wptr.wrd = read_dma_ctrl_reg( DMA_DMA_SSP_RXQ5_WPTR );
	dma_ssp_rxq5_intsts.wrd = read_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_RXQ_COAL_INTERRUPT);
	if( dma_ssp_rxq5_intsts.bf.rxq5_full || ( dma_txq5_wptr.wrd % 2)  || ( dma_rxq5_wptr.wrd %2 ) ) {
		printf( "========= %s full[%d] [%d][%d] =============\n", __func__, dma_ssp_rxq5_intsts.bf.rxq5_full, dma_txq5_wptr.wrd, dma_rxq5_wptr.wrd );
		dma_ssp_rxq5_intsts.bf.rxq5_full= 3;
	}
#endif


	/** printf("%s : page(%x:%x) col(%x)  <<----\n", __func__, page, (page*mtd->writesize), col);**/
	printf("%s(%d) finished!!!\n", __func__, __LINE__);
	return 0;
}


/**
 * pnand_write_page_raw - [Intern] raw page write function
 * @mtd:	mtd info structure
 * @chip:	nand chip info structure
 * @buf:	data buffer
 *
 * Not for syndrome calculating ecc controllers, which use a special oob layout
 */
 void pnand_write_page_raw(struct mtd_info *mtd, struct nand_chip *chip,
				const uint8_t *buf, int oob_required)
{

	int i, j, eccsize = chip->ecc.size, page, col, reg_tmp;
	int eccbytes = chip->ecc.bytes;
	int eccsteps = chip->ecc.steps;
	/*uint8_t *ecc_calc = chip->buffers->ecccalc;*/
	/*const uint8_t *p = buf;*/
	uint32_t *eccpos = chip->ecc.layout->eccpos, *addr, reg_bch_gen;
	DMA_SEC_DMA_SSP_Q_RXQ_COAL_INTERRUPT_t	tmp_dma_ssp_rxq5_intsts;
	DMA_SEC_DMA_SSP_Q_TXQ_COAL_INTERRUPT_t	tmp_dma_ssp_txq5_intsts;
#ifdef CONFIG_PNAND_ECC_HW_BCH
	FLASH_NF_BCH_STATUS_t	tmp_bsc_sts;
#else
	FLASH_NF_ECC_STATUS_t	tmp_ecc_sts;
#endif
	FLASH_FIFO_CONTROL_t flash_fifo_ctrl;

	check_flash_ctrl_status();

	page = host->page_addr;
	col  = host->col_addr;

	nf_ecc_reset.wrd = 3;
	nf_ecc_reset.bf.eccClear = ECC_CLR;
	nf_ecc_reset.bf.fifoClear = FIFO_CLR;
	write_flash_ctrl_reg(FLASH_NF_ECC_RESET, nf_ecc_reset.wrd);

	flash_int_sts.bf.regIrq = 1;
	write_flash_ctrl_reg(FLASH_FLASH_INTERRUPT, flash_int_sts.wrd);

	nf_ecc_reset.wrd = 0;
	nf_ecc_reset.bf.eccClear = 1;
	write_flash_ctrl_reg(FLASH_NF_ECC_RESET, nf_ecc_reset.wrd);

#ifdef CONFIG_PNAND_ECC_HW_BCH
		write_flash_ctrl_reg(FLASH_NF_BCH_CONTROL, 0);
#else
		write_flash_ctrl_reg(FLASH_NF_ECC_CONTROL, 0);
#endif


	/*disable txq5*/
	dma_txq5_ctrl.bf.txq_en = 0;
	write_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_CONTROL, dma_txq5_ctrl.wrd);
	/*clr tx/rx eof*/
	dma_ssp_txq5_intsts.wrd = read_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_COAL_INTERRUPT);
	dma_ssp_txq5_intsts.bf.txq_eof = 0;
	write_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_COAL_INTERRUPT, dma_ssp_txq5_intsts.wrd);
	dma_ssp_rxq5_intsts.wrd = read_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_RXQ_COAL_INTERRUPT);
	dma_ssp_rxq5_intsts.bf.rxq_eof = 0;
	write_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_RXQ_COAL_INTERRUPT, dma_ssp_rxq5_intsts.wrd);


	nf_count.wrd = 0;
	nf_command.wrd = 0;
	nf_addr1.wrd = 0;
	nf_addr2.wrd = 0;
	nf_count.wrd = 0;
	nf_count.bf.nflashRegOobCount = mtd->oobsize - 1;
	nf_count.bf.nflashRegDataCount = mtd->writesize-1;

	if(chip->chipsize < (32 << 20))
	{
		nf_count.bf.nflashRegAddrCount = NF_CNT_ADDR_3;
		nf_count.bf.nflashRegCmdCount = NF_CNT_CMD_2;
		nf_command.bf.nflashRegCmd0 = NAND_CMD_SEQIN;
		nf_command.bf.nflashRegCmd1 = NAND_CMD_PAGEPROG;
		nf_addr1.wrd = (((page & 0x00ffffff)<<8));
		nf_addr2.wrd = ((page & 0xff000000)>>24);

	}
	else if((chip->chipsize >= (32 << 20)) && (chip->chipsize <= (128 << 20)))
	{
		nf_count.bf.nflashRegAddrCount = NF_CNT_ADDR_4;
		nf_count.bf.nflashRegCmdCount = NF_CNT_CMD_2;
		nf_command.bf.nflashRegCmd0 = NAND_CMD_SEQIN;
		nf_command.bf.nflashRegCmd1 = NAND_CMD_PAGEPROG;
		nf_addr1.wrd = (((page & 0xffff)<<16) );
		nf_addr2.wrd = ((page & 0xffff0000)>>16);

	}
	else /*if((chip->chipsize > (128 << 20)) ))*/
	{

		nf_count.bf.nflashRegAddrCount = NF_CNT_ADDR_5;
		nf_count.bf.nflashRegCmdCount = NF_CNT_CMD_2;
		nf_command.bf.nflashRegCmd0 = NAND_CMD_SEQIN;
		nf_command.bf.nflashRegCmd1 = NAND_CMD_PAGEPROG;
		nf_addr1.wrd = (((page & 0xffff)<<16) );
		nf_addr2.wrd = ((page & 0xffff0000)>>16);
	}

	write_flash_ctrl_reg(FLASH_NF_COUNT, nf_count.wrd);
	write_flash_ctrl_reg(FLASH_NF_COMMAND, nf_command.wrd);
	write_flash_ctrl_reg(FLASH_NF_ADDRESS_1, nf_addr1.wrd);
	write_flash_ctrl_reg(FLASH_NF_ADDRESS_2, nf_addr2.wrd);

	/*dma_map_single( NULL, (void *)buf, mtd->writesize, DMA_TO_DEVICE);*/

	nf_access.wrd = 0;
	nf_access.bf.nflashCeAlt = NF_ACCESS_CE_ALT;
	nf_access.bf.nflashRegWidth = NF_ACCESS_WiDTH8;
	/*write_flash_ctrl_reg(FLASH_NF_ACCESS, nf_access.wrd);

	write
	prepare dma descriptor
	chip->buffers->databuf
	nf_access.wrd = read_flash_ctrl_reg(FLASH_FLASH_ACCESS_START);
	*/
	nf_access.bf.nflashExtAddr = ((page << chip->page_shift) / 0x8000000);
	write_flash_ctrl_reg(FLASH_NF_ACCESS, nf_access.wrd);

	addr =  (unsigned int *)((page << chip->page_shift) % 0x8000000);
	addr = (unsigned int *)((unsigned int)addr + (unsigned int)(CONFIG_SYS_FLASH_BASE) );

	/*page data tx desc*/
	dma_txq5_wptr.wrd = read_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_WPTR);
	tx_desc[dma_txq5_wptr.bf.index].word1.bf.own = OWN_DMA;
	tx_desc[dma_txq5_wptr.bf.index].word1.bf.buf_len = mtd->writesize;
	tx_desc[dma_txq5_wptr.bf.index].buf_adr = (unsigned int)(buf);


	/*page data rx desc
	printf("pnand_write_page_hwecc : addr : %p  buf: %p",addr, buf);
	*/

	dma_rxq5_rptr.wrd = read_dma_ctrl_reg(	DMA_SEC_DMA_SSP_Q_RXQ_RPTR);
	rx_desc[dma_rxq5_rptr.bf.index].word1.bf.own = OWN_DMA;
	rx_desc[dma_rxq5_rptr.bf.index].word1.bf.buf_len = mtd->writesize;
	rx_desc[dma_rxq5_rptr.bf.index].buf_adr = (unsigned int)addr;

	/*oob rx desc*/
	addr = (unsigned int *)((unsigned int)addr + mtd->writesize);
	/*printf("  oob : addr(%p)  chip->oob_poi(%p) \n",addr, chip->oob_poi);*/

	dma_rxq5_rptr.bf.index = (dma_rxq5_rptr.bf.index + 1) % FDMA_DESC_NUM;
	rx_desc[dma_rxq5_rptr.bf.index].word1.bf.own = OWN_DMA;
	rx_desc[dma_rxq5_rptr.bf.index].word1.bf.buf_len = mtd->oobsize;
	rx_desc[dma_rxq5_rptr.bf.index].buf_adr = (unsigned int)addr;


	/*update page tx write ptr*/
	dma_txq5_wptr.bf.index = (dma_txq5_wptr.bf.index + 1) % FDMA_DESC_NUM;
	write_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_WPTR, dma_txq5_wptr.wrd);
	/*set axi_bus_len = 8
	set fifo control*/
	//nf_fifo_ctl.wrd = 0;
	//nf_fifo_ctl.bf.fifoCmd = FLASH_WT;
	//write_flash_ctrl_reg(FLASH_NF_FIFO_CONTROL, nf_fifo_ctl.wrd);

	flash_fifo_ctrl.wrd = read_flash_ctrl_reg(FLASH_FIFO_CONTROL);
	flash_fifo_ctrl.bf.fifoCmd = FIFO_CMD_WRITE;
	write_flash_ctrl_reg(FLASH_FIFO_CONTROL, flash_fifo_ctrl.wrd);

	access_start.wrd = 0;
	access_start.bf.nflashFifoReq = FLASH_GO;
	/*access_start.bf.nflashRegCmd = FLASH_WT;*/
	write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, access_start.wrd);

	/*enable txq5*/
	dma_txq5_ctrl.bf.txq_en = 1;
	write_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_CONTROL, dma_txq5_ctrl.wrd);

	dma_ssp_rxq5_intsts.wrd = read_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_RXQ_COAL_INTERRUPT);
	tmp_dma_ssp_rxq5_intsts.wrd = 0;
	tmp_dma_ssp_rxq5_intsts.bf.rxq_eof = 1;


	reg_wait(DMA_SEC_DMA_SSP_Q_RXQ_COAL_INTERRUPT, tmp_dma_ssp_rxq5_intsts.wrd , tmp_dma_ssp_rxq5_intsts.wrd, 1000);


	dma_ssp_txq5_intsts.wrd = read_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_COAL_INTERRUPT);
	tmp_dma_ssp_txq5_intsts.wrd = 0;
	tmp_dma_ssp_txq5_intsts.bf.txq_eof = 1;

	reg_wait(DMA_SEC_DMA_SSP_Q_TXQ_COAL_INTERRUPT, tmp_dma_ssp_txq5_intsts.wrd , tmp_dma_ssp_txq5_intsts.wrd, 1000);

	/*clr tx/rx eof*/

	dma_ssp_txq5_intsts.bf.txq_eof = 0;
	write_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_COAL_INTERRUPT, dma_ssp_txq5_intsts.wrd);

	dma_ssp_rxq5_intsts.bf.rxq_eof = 0;
	write_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_RXQ_COAL_INTERRUPT, dma_ssp_rxq5_intsts.wrd);


#if defined(CONFIG_PNAND_ECC_HW_BCH)
		/* jenfeng clear erase tag*/
		*(chip->oob_poi+ chip->ecc.layout->oobfree[0].offset +  chip->ecc.layout->oobfree[0].length)= 0;
#endif

	/*dma_map_single( NULL, (void *)chip->oob_poi, mtd->oobsize, DMA_TO_DEVICE);

	dma_txq5_wptr.wrd = read_dma_ctrl_reg(DMA_DMA_SSP_TXQ5_WPTR);
	*/
	tx_desc[dma_txq5_wptr.bf.index].word1.bf.own = OWN_DMA;
	tx_desc[dma_txq5_wptr.bf.index].word1.bf.buf_len = mtd->oobsize;
	tx_desc[dma_txq5_wptr.bf.index].buf_adr = (unsigned int)(chip->oob_poi);

	/*dma_cache_sync(NULL, chip->oob_poi, mtd->oobsize, DMA_BIDIRECTIONAL);
	update tx write ptr
	*/
	dma_txq5_wptr.bf.index = (dma_txq5_wptr.bf.index + 1) % FDMA_DESC_NUM;
	write_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_WPTR, dma_txq5_wptr.wrd);

	dma_ssp_rxq5_intsts.wrd = read_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_RXQ_COAL_INTERRUPT);
	tmp_dma_ssp_rxq5_intsts.wrd = 0;
	tmp_dma_ssp_rxq5_intsts.bf.rxq_eof = 1;


	reg_wait(DMA_SEC_DMA_SSP_Q_RXQ_COAL_INTERRUPT, tmp_dma_ssp_rxq5_intsts.wrd , tmp_dma_ssp_rxq5_intsts.wrd, 1000);

	dma_ssp_txq5_intsts.wrd = read_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_COAL_INTERRUPT);
	tmp_dma_ssp_txq5_intsts.wrd = 0;
	tmp_dma_ssp_txq5_intsts.bf.txq_eof = 1;

	reg_wait(DMA_SEC_DMA_SSP_Q_TXQ_COAL_INTERRUPT, tmp_dma_ssp_txq5_intsts.wrd , tmp_dma_ssp_txq5_intsts.wrd, 1000);

	access_start.wrd=read_flash_ctrl_reg(FLASH_FLASH_ACCESS_START);

	tmp_access.wrd = 0;
	tmp_access.bf.nflashFifoReq = 1;
	reg_wait(FLASH_FLASH_ACCESS_START, tmp_access.wrd, 0, 1000);

	/*update rx read ptr
	dma_rxq5_rptr.wrd = read_dma_ctrl_reg(DMA_DMA_SSP_RXQ5_RPTR);
	*/
	dma_rxq5_rptr.bf.index = (dma_rxq5_rptr.bf.index + 1) % FDMA_DESC_NUM;
	write_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_RXQ_RPTR, dma_rxq5_rptr.wrd);



	/*chip->write_buf(mtd, chip->oob_poi, mtd->oobsize);*/
	printf("%s(%d) finished!!!\n", __func__, __LINE__);
}


/**
 * pnand_read_page_raw - [Intern] read raw page data without ecc
 * @mtd:	mtd info structure
 * @chip:	nand chip info structure
 * @buf:	buffer to store read data
 */
 int pnand_read_page_raw(struct mtd_info *mtd, struct nand_chip *chip,
			      uint8_t *buf, int oob_required, int page)
{
	int i, eccsize = chip->ecc.size, col;
	int eccbytes = chip->ecc.bytes;
	int eccsteps = chip->ecc.steps;
	uint8_t *p = buf;
	/*uint8_t *ecc_calc = chip->buffers->ecccalc;*/
	uint8_t *ecc_code = chip->buffers->ecccode;
	uint32_t *eccpos = chip->ecc.layout->eccpos, *addr;;
	struct pnand_host *host = nand_chip->priv;
	DMA_SEC_DMA_SSP_Q_RXQ_COAL_INTERRUPT_t	tmp_dma_ssp_rxq5_intsts;
	DMA_SEC_DMA_SSP_Q_TXQ_COAL_INTERRUPT_t	tmp_dma_ssp_txq5_intsts;
#if defined(CONFIG_PNAND_ECC_HW_BCH)
	FLASH_NF_BCH_STATUS_t	tmp_bsc_sts;
#else
	FLASH_NF_ECC_STATUS_t	tmp_ecc_sts;
#endif
	FLASH_FIFO_CONTROL_t flash_fifo_ctrl;
	//FLASH_NF_ECC_RESET_t ecc_rst;
//	volatile uint32_t tmp;

//	check_flash_ctrl_status();

	col  = host->col_addr;
	p = buf;
	/*printf("%s : page(%x:%x) col(%x)  ---->>\n", __func__, page, (page*mtd->writesize), col);*/
	nf_ecc_reset.wrd = 0;
	nf_ecc_reset.bf.eccClear = ECC_CLR;
	nf_ecc_reset.bf.fifoClear = FIFO_CLR;
	write_flash_ctrl_reg(FLASH_NF_ECC_RESET, nf_ecc_reset.wrd);

	flash_int_sts.bf.regIrq = 1;
	write_flash_ctrl_reg(FLASH_FLASH_INTERRUPT, flash_int_sts.wrd);

	nf_ecc_reset.wrd = 0;
	nf_ecc_reset.bf.eccClear = 1;
	write_flash_ctrl_reg(FLASH_NF_ECC_RESET, nf_ecc_reset.wrd);

#ifdef CONFIG_PNAND_ECC_HW_BCH
		nf_bch_ctrl.wrd = 0;
		nf_bch_ctrl.bf.bchEn = BCH_ENABLE;
		nf_bch_ctrl.bf.bchOpcode = BCH_DECODE;
#if	(CONFIG_PNAND_ECC_HW_BCH_LEVEL == 8)
			nf_bch_ctrl.bf.bchErrCap = BCH_ERR_CAP_8;
#elif (CONFIG_PNAND_ECC_HW_BCH_LEVEL == 16)
			nf_bch_ctrl.bf.bchErrCap = BCH_ERR_CAP_16;
#elif (CONFIG_PNAND_ECC_HW_BCH_LEVEL == 24)
			nf_bch_ctrl.bf.bchErrCap = BCH_ERR_CAP_24;
#elif (CONFIG_PNAND_ECC_HW_BCH_LEVEL == 40)
			nf_bch_ctrl.bf.bchErrCap = BCH_ERR_CAP_40;
#else
			printf("BCH Level is not recognized! %s:%d\n",__func__, __LINE__);
#endif
		write_flash_ctrl_reg(FLASH_NF_BCH_CONTROL, 0);

#else

		nf_ecc_ctl.wrd = 0;
	if((eccsize-1) == NF_CNT_512P_DATA)
			nf_ecc_ctl.bf.eccGenMode = ECC_GEN_512;
	else
			nf_ecc_ctl.bf.eccGenMode = ECC_GEN_256;
		nf_ecc_ctl.bf.eccEn = ECC_ENABLE;
		write_flash_ctrl_reg(FLASH_NF_ECC_CONTROL, nf_ecc_ctl.wrd);
#endif

	/*disable txq5*/
	dma_txq5_ctrl.bf.txq_en = 0;
	write_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_CONTROL, dma_txq5_ctrl.wrd);

	dma_ssp_txq5_intsts.wrd = read_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_COAL_INTERRUPT);
	dma_ssp_txq5_intsts.bf.txq_eof = 0;
	write_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_COAL_INTERRUPT, dma_ssp_txq5_intsts.wrd);
	dma_ssp_rxq5_intsts.wrd = read_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_RXQ_COAL_INTERRUPT);
	dma_ssp_rxq5_intsts.bf.rxq_eof = 0;
	write_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_RXQ_COAL_INTERRUPT, dma_ssp_rxq5_intsts.wrd);

	/*for indirect access with DMA, because DMA not ready	*/
	nf_count.wrd = 0;
	nf_command.wrd = 0;
	nf_addr1.wrd = 0;
	nf_addr2.wrd = 0;
	nf_count.wrd = 0;
	nf_count.bf.nflashRegOobCount = (mtd->oobsize - 1);
	nf_count.bf.nflashRegDataCount = (mtd->writesize - 1) ;

	if(chip->chipsize < (32 << 20)) /* 0 < size < 32MB */
	{
		nf_count.bf.nflashRegAddrCount = NF_CNT_ADDR_3;
		nf_count.bf.nflashRegCmdCount = NF_CNT_CMD_1;
		nf_command.bf.nflashRegCmd0 = NAND_CMD_READ0;
		nf_addr1.wrd = (((page & 0x00ffffff)<<8));
		nf_addr2.wrd = ((page & 0xff000000)>>24);
	}
	else if((chip->chipsize >= (32 << 20)) && (chip->chipsize <= (128 << 20))) /* 32MB < size < 128MB */
	{
		nf_count.bf.nflashRegAddrCount = NF_CNT_ADDR_4;
		nf_count.bf.nflashRegCmdCount = NF_CNT_CMD_2;
		nf_command.bf.nflashRegCmd0 = NAND_CMD_READ0;

		if(mtd->writesize > NF_CNT_512P_DATA) {
			nf_count.bf.nflashRegCmdCount = NF_CNT_CMD_2;
			nf_command.bf.nflashRegCmd1 = NAND_CMD_READSTART;
		}
		nf_addr1.wrd = (((page & 0xffff)<<16));
		nf_addr2.wrd = ((page & 0xffff0000)>>16);
	}
	else /*if((chip->chipsize > (128 << 20)) ))*/ /* size > 128MB */
	{
		nf_count.bf.nflashRegAddrCount = NF_CNT_ADDR_5;
		nf_count.bf.nflashRegCmdCount = NF_CNT_CMD_2;
		nf_command.bf.nflashRegCmd0 = NAND_CMD_READ0;
		nf_command.bf.nflashRegCmd1 = NAND_CMD_READSTART;
		nf_addr1.wrd = (((page & 0xffff)<<16));
		nf_addr2.wrd = ((page & 0xffff0000)>>16);
	}


	write_flash_ctrl_reg(FLASH_NF_COUNT, nf_count.wrd);
	write_flash_ctrl_reg(FLASH_NF_COMMAND, nf_command.wrd); /*write read id command*/
	write_flash_ctrl_reg(FLASH_NF_ADDRESS_1, nf_addr1.wrd); /*write address 0x0*/
	write_flash_ctrl_reg(FLASH_NF_ADDRESS_2, nf_addr2.wrd);


	nf_access.wrd = 0;
	nf_access.bf.nflashCeAlt = NF_ACCESS_CE_ALT;
	nf_access.bf.nflashRegWidth = NF_ACCESS_WiDTH8;
	/*	write_flash_ctrl_reg(FLASH_NF_ACCESS, nf_access.wrd);
	prepare dma descriptor
	nf_access.wrd = read_flash_ctrl_reg(FLASH_FLASH_ACCESS_START);
	*/
	nf_access.bf.nflashExtAddr = ((page << chip->page_shift) / 0x8000000);
	write_flash_ctrl_reg(FLASH_NF_ACCESS, nf_access.wrd);

	addr =  (unsigned int *)((page << chip->page_shift) % 0x8000000);
	addr = (unsigned int *)((unsigned int)addr + (unsigned int)(CONFIG_SYS_FLASH_BASE) );

	/*printf("%s : addr : %p  buf: %p",__func__, addr, buf);
	chip->buffers->databuf
	page tx data desc
	*/
	dma_txq5_wptr.wrd = read_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_WPTR);

	tx_desc[dma_txq5_wptr.bf.index].word1.bf.own = OWN_DMA;
	tx_desc[dma_txq5_wptr.bf.index].word1.bf.buf_len = mtd->writesize;
	tx_desc[dma_txq5_wptr.bf.index].buf_adr = (unsigned int)addr;

	/*oob tx desc*/
	dma_txq5_wptr.bf.index = (dma_txq5_wptr.bf.index + 1) % FDMA_DESC_NUM;

	addr = (unsigned int *)((unsigned int)addr + mtd->writesize);
	/*printf("   oob : addr (%p)  chip->oob_poi (%p) \n", addr, chip->oob_poi);*/
	tx_desc[dma_txq5_wptr.bf.index].word1.bf.own = OWN_DMA;
	tx_desc[dma_txq5_wptr.bf.index].word1.bf.buf_len = mtd->oobsize;
	tx_desc[dma_txq5_wptr.bf.index].buf_adr = (unsigned int)addr;

	/*page data rx desc*/
	dma_rxq5_rptr.wrd = read_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_RXQ_RPTR);
	rx_desc[dma_rxq5_rptr.bf.index].word1.bf.own = OWN_DMA;
	rx_desc[dma_rxq5_rptr.bf.index].word1.bf.buf_len = mtd->writesize;

	rx_desc[dma_rxq5_rptr.bf.index].buf_adr = (unsigned int)(buf);

	/*oob rx desc*/
	dma_rxq5_rptr.bf.index = (dma_rxq5_rptr.bf.index + 1) % FDMA_DESC_NUM;
	rx_desc[dma_rxq5_rptr.bf.index].word1.bf.own = OWN_DMA;
	rx_desc[dma_rxq5_rptr.bf.index].word1.bf.buf_len = mtd->oobsize;
	rx_desc[dma_rxq5_rptr.bf.index].buf_adr =  (unsigned int)(chip->oob_poi);


	/*dma_map_single( NULL, (void *)chip->oob_poi, mtd->oobsize, DMA_FROM_DEVICE);
	dma_map_single( NULL, (void *)buf, mtd->writesize, DMA_FROM_DEVICE);

	set axi_bus_len = 8

	set fifo control
	*/
	//nf_fifo_ctl.wrd = 0;
	//nf_fifo_ctl.bf.fifoCmd = FLASH_RD;
	//write_flash_ctrl_reg(FLASH_NF_FIFO_CONTROL, nf_fifo_ctl.wrd);

	flash_fifo_ctrl.wrd = read_flash_ctrl_reg(FLASH_FIFO_CONTROL);
	flash_fifo_ctrl.bf.fifoCmd = FIFO_CMD_READ;
	write_flash_ctrl_reg(FLASH_FIFO_CONTROL, flash_fifo_ctrl.wrd);

	access_start.wrd = 0;
	access_start.bf.nflashFifoReq = FLASH_GO;
	/*access_start.bf.nflashRegCmd = FLASH_RD;*/
	write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, access_start.wrd);

	/*update tx write ptr*/
	dma_txq5_wptr.bf.index = (dma_txq5_wptr.bf.index + 1) % FDMA_DESC_NUM;
	write_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_WPTR, dma_txq5_wptr.wrd);
	/*dma_txq5_wptr.wrd = read_dma_ctrl_reg(DMA_DMA_SSP_TXQ5_WPTR);

	enable txq5
	*/
	dma_txq5_ctrl.bf.txq_en = 1;
	write_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_CONTROL, dma_txq5_ctrl.wrd);

	tmp_dma_ssp_rxq5_intsts.wrd = 0;
	tmp_dma_ssp_rxq5_intsts.bf.rxq_eof = 1;


	reg_wait(DMA_SEC_DMA_SSP_Q_RXQ_COAL_INTERRUPT, tmp_dma_ssp_rxq5_intsts.wrd , tmp_dma_ssp_rxq5_intsts.wrd, 1000);

	tmp_dma_ssp_txq5_intsts.wrd = 0;
	tmp_dma_ssp_txq5_intsts.bf.txq_eof = 1;

	reg_wait(DMA_SEC_DMA_SSP_Q_TXQ_COAL_INTERRUPT, tmp_dma_ssp_txq5_intsts.wrd , tmp_dma_ssp_txq5_intsts.wrd, 1000);

	tmp_access.wrd = 0;
	tmp_access.bf.nflashFifoReq = 1;
	reg_wait(FLASH_FLASH_ACCESS_START, tmp_access.wrd, 0, 1000);


	/**
	  nf_ecc_sts.wrd=read_flash_ctrl_reg(FLASH_NF_ECC_STATUS);
	  while(!nf_ecc_sts.bf.eccDone)
	  {
	  nf_ecc_sts.wrd=read_flash_ctrl_reg(FLASH_NF_ECC_STATUS);
	  udelay(1);

	  }

	  write_flash_ctrl_reg(FLASH_NF_ECC_CONTROL, 0);

	dma_cache_sync(NULL, buf, mtd->writesize, DMA_BIDIRECTIONAL);
	dma_cache_sync(NULL, chip->oob_poi, mtd->oobsize, DMA_BIDIRECTIONAL);
	update rx read ptr
	**/
	dma_rxq5_rptr.bf.index = (dma_rxq5_rptr.bf.index + 1) % FDMA_DESC_NUM;
	write_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_RXQ_RPTR, dma_rxq5_rptr.wrd);
	printf("%s(%d) finished!!!\n", __func__, __LINE__);

	return 0;
}

/**
 * pnand_write_page - [REPLACEABLE] write one page
 * @mtd:	MTD device structure
 * @chip:	NAND chip descriptor
 * @buf:	the data to write
 * @page:	page number to write
 * @cached:	cached programming
 * @raw:	use _raw version of write_page
 */
 int pnand_write_page(struct mtd_info *mtd, struct nand_chip *chip,
				   uint32_t offset, int data_len,
				   const uint8_t *buf, int oob_required,
				   int page, int cached, int raw)
{
	int status;

	chip->cmdfunc(mtd, NAND_CMD_SEQIN, 0x00, page);

	if (unlikely(raw))
	{
#if defined(CONFIG_PNAND_TEST_DRIVER)
		chip->ecc.write_page_raw(mtd, chip, buf, 1); /* pnand_write_page_raw */
#else
		chip->ecc.write_page_raw(mtd, chip, buf, 1, page);
#endif
	}
	else
	{
#if defined(CONFIG_PNAND_TEST_DRIVER)
		chip->ecc.write_page(mtd, chip, buf, 1); /* pnand_write_page_hwecc */
#else
		chip->ecc.write_page(mtd, chip, buf, 1, page);
#endif
	}

	/*
	 * Cached progamming disabled for now, Not sure if its worth the
	 * trouble. The speed gain is not very impressive. (2.3->2.6Mib/s)
	 */
	cached = 0;

	if (!cached || !(chip->options & NAND_CACHEPRG)) {

		chip->cmdfunc(mtd, NAND_CMD_PAGEPROG, -1, -1);
		status = chip->waitfunc(mtd, chip);
		/*
		 * See if operation failed and additional status checks are
		 * available
		 */
		if ((status & NAND_STATUS_FAIL) && (chip->errstat))
			status = chip->errstat(mtd, chip, FL_WRITING, status,
					       page);

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

/*
 * Get the flash and manufacturer id and lookup if the type is supported
 */
struct nand_flash_dev *pnand_get_flash_type(struct mtd_info *mtd,
						  struct nand_chip *chip,
						  int busw, int *maf_id)
{
	volatile int loop=0;
	struct nand_flash_dev *type = NULL;
	int i, dev_id, maf_idx;
	unsigned char id[8];
	u16 oobsize_8kp[]= { 0 , 128, 218, 400, 436, 512, 640, 0};
	FLASH_TYPE_t flash_type;

	/* Select the device */
	chip->select_chip(mtd, 0);

	/*
	 * Reset the chip, required by some chips (e.g. Micron MT29FxGxxxxx)
	 * after power-up
	 */
	chip->cmdfunc(mtd, NAND_CMD_RESET, -1, -1);

	/* Try if this is ONFI supported chip */
	if( pnand_flash_detect_onfi(mtd, chip, busw)==1){
		printf("ONFI\n");
		goto Set_flash_type;
	}
	else{
		printf("Legacy\n");
	}

	/* Send the command for reading device ID */
	chip->cmdfunc(mtd, NAND_CMD_RESET, -1, -1);
	chip->cmdfunc(mtd, NAND_CMD_READID, 0x00, -1);

	/* Read manufacturer and device IDs */
	memset( id, 0, sizeof( id));
	pnand_read_id(&id[0]);
	chip->cmdfunc(mtd, NAND_CMD_RESET, -1, -1);
	*maf_id  = id[0];
	dev_id = id[1];
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
		if (dev_id == type->dev_id) {
				break;
		}
	}



	/* Lookup the flash id */
//	for (i = 0; nand_flash_ids[i].name != NULL; i++) {
//		if (dev_id == nand_flash_ids[i].id) {
//			type =  &nand_flash_ids[i];
//			break;
//		}
//	}

	if (!type)
		return ERR_PTR(-ENODEV);

	if (!mtd->name)
		mtd->name = type->name;
#if defined(CONFIG_PNAND_TEST_DRIVER)
	chip->chipsize = (uint32_t)type->chipsize << 20;
#else
	chip->chipsize = (uint64_t)type->chipsize << 20;
#endif
	/* Newer devices have all the information in additional id bytes */
	if (!type->pagesize) {
		int extid;
		/*  The 3rd id byte holds MLC / multichip data  */
		//chip->cellinfo = id[2];//chip->read_byte(mtd);
		chip->bits_per_cell = nand_get_bits_per_cell(id[2]);
		/*  The 4th id byte is the important one  */
		extid = id[3];
		if( id[0] == id[6] && id[1] == id[7] &&
			id[0] == NAND_MFR_SAMSUNG  &&
			       !nand_is_slc(chip) && id[5] != 0x00) {
			mtd->writesize = 2048 * ( 1 << (extid & 0x3) ) ;

			/* Calc oobsize */
			mtd->oobsize =  oobsize_8kp[ ((extid & 0x40) >> 4) + ((extid >> 2) &0x03)] ;

			/* Calc blocksize. Blocksize is multiples of 128KB */
			mtd->erasesize = ( 1 << (((extid & 0x80) >> 5) + ((extid >> 4) & 0x03) )) * ( 128 * 1024) ;
			busw =  0;
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
		mtd->erasesize = type->erasesize;
		mtd->writesize = type->pagesize;
		mtd->oobsize = mtd->writesize / 32;
		busw = type->options & NAND_BUSWIDTH_16;
	}

	/* Try to identify manufacturer */
	for (maf_idx = 0; nand_manuf_ids[maf_idx].id != 0x0; maf_idx++) {
		if (nand_manuf_ids[maf_idx].id == *maf_id)
			break;
	}

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
	 if(id[0] == NAND_MFR_TOSHIBA && id[1]== 0xD3)
	 {
		int extid;
		extid = id[3];
		mtd->writesize = 1024 << (extid & 0x3);
		flash_type.wrd = read_flash_ctrl_reg(FLASH_TYPE);
		if(flash_type.bf.flashType==0x7)
			mtd->oobsize =  224;
		else if(flash_type.bf.flashType==0x6)
			mtd->oobsize =  128;
		else
			printk(KERN_INFO "Check flash type of HW strap pin for 4k page. \n");

		extid >>= 4;
		mtd->erasesize = (64 * 1024) << (extid & 0x03);
		if(id[2]==0x90)
		{
			type->name = "NAND 2GiB 3,3V 8-bit";
			mtd->name = "NAND 2GiB 3,3V 8-bit";
			type->chipsize  = 2048;
		}

		chip->chipsize = type->chipsize << 20;
		#ifndef CONFIG_PNAND_ECC_HW_BCH
			printk(KERN_INFO "NAND ECC Level 4bit ECC for each 512Byte is required.\n");
		#endif
	 }
Set_flash_type:
/////// middle debug : oob size not 4 bytes alignment
	if(mtd->oobsize%8)
		mtd->oobsize = mtd->oobsize - (mtd->oobsize%8);
///////

	printf("Page Size: 0x%x byte\n",mtd->writesize);
	printf("OOB Size: 0x%x byte\n",mtd->oobsize);

	/*
	 * Check, if buswidth is correct. Hardware drivers should set
	 * chip correct !
	 */
	if (busw != (chip->options & NAND_BUSWIDTH_16)) {
		printf(KERN_INFO "NAND device: Manufacturer ID:"
		       " 0x%02x, Chip ID: 0x%02x (%s %s)\n", *maf_id,
		       dev_id, nand_manuf_ids[maf_idx].name, mtd->name);
		printf(KERN_WARNING "NAND bus width %d instead %d bit\n",
		       (chip->options & NAND_BUSWIDTH_16) ? 16 : 8,
		       busw ? 16 : 8);
		return ERR_PTR(-EINVAL);
	}

	/* Calculate the address shift from the page size */
	chip->page_shift = ffs(mtd->writesize) - 1;
	/* Convert chipsize to number of pages per chip -1. */
	chip->pagemask = (chip->chipsize >> chip->page_shift) - 1;

	chip->bbt_erase_shift = chip->phys_erase_shift =
		ffs(mtd->erasesize) - 1;
	if (chip->chipsize & 0xffffffff)
		chip->chip_shift = ffs((unsigned)chip->chipsize) - 1;
	else
		chip->chip_shift = ffs((unsigned)(chip->chipsize >> 32)) + 32 - 1;

	/* Set the bad block position */
	chip->badblockpos = mtd->writesize > 512 ?
		NAND_LARGE_BADBLOCK_POS : NAND_SMALL_BADBLOCK_POS;

	/* Get chip options, preserve non chip based options */
	//chip->options &= ~NAND_CHIPOPTIONS_MSK;
	if( type != NULL) {
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

#ifdef CS75XX_NAND_DEBUG
	printf(KERN_INFO "NAND device: Manufacturer ID:"
	       " 0x%02x, Chip ID: 0x%02x (%s %s)\n", *maf_id, dev_id,
	       nand_manuf_ids[maf_idx].name, type->name);
#endif

	flash_type.wrd = read_flash_ctrl_reg(FLASH_TYPE);
	switch (mtd->writesize){
	case 512:
		flash_type.bf.flashType=0|0x4;
		break;
	case 2048:
		flash_type.bf.flashType=1|0x4;
		break;
	case 4096:
		flash_type.bf.flashType=2|0x4;
		break;
	case 8192:
		flash_type.bf.flashType=3|0x4;
		break;
	}
	flash_type.bf.flashSize=0;
	write_flash_ctrl_reg(FLASH_TYPE, flash_type.wrd);


	return type;
}

/*
 *	hardware specific access to control-lines
 *	ctrl:
 */
 void pnand_hwcontrol(struct mtd_info *mtd, int cmd,
				   unsigned int ctrl)
{
	/**
	struct sharpsl_nand *sharpsl = mtd_to_sharpsl(mtd);
#if defined(CONFIG_PNAND_TEST_DRIVER)
	struct nand_chip *chip = mtd->priv;
#else
	struct nand_chip *chip = mtd_to_nand(mtd);
#endif
  middle not yet
  nothing to do now.
  **/
}


/**
 * pnand_default_block_markbad - [DEFAULT] mark a block bad
 * @mtd:	MTD device structure
 * @ofs:	offset from device start
 *
 * This is the default implementation, which can be overridden by
 * a hardware specific driver.
*/
 int pnand_default_block_markbad(struct mtd_info *mtd, loff_t ofs)
{
#if defined(CONFIG_PNAND_TEST_DRIVER)
	struct nand_chip *chip = mtd->priv;
#else
	struct nand_chip *chip = mtd_to_nand(mtd);
#endif
	uint8_t buf[2] = { 0, 0 };
	struct mtd_oob_ops ops;
	int block, ret;

	/* Get block number */
	block = (int)(ofs >> chip->bbt_erase_shift);
	if (chip->bbt)
		chip->bbt[block >> 2] |= 0x01 << ((block & 0x03) << 1);

	/* Do we have a flash based bad block table ? */
	//if (chip->options & NAND_USE_FLASH_BBT)
	if( 0 )
		ret = nand_update_bbt(mtd, ofs);
	else {
		/* We write two bytes, so we dont have to mess with 16 bit
		 * access
		 */
		pnand_get_device(chip, mtd, FL_WRITING);
		ofs += mtd->oobsize;
		ops.mode = MTD_OPS_PLACE_OOB;
		ops.len = ops.ooblen = 2;
		ops.datbuf = NULL;
		ops.oobbuf = buf;
		ops.ooboffs = chip->badblockpos & ~0x01;

		ret = pnand_do_write_oob(mtd, ofs, &ops);
		pnand_release_device(mtd);
	}
	if (!ret)
		mtd->ecc_stats.badblocks++;

	return ret;
}

/**
 * pnand_block_bad - [DEFAULT] Read bad block marker from the chip
 * @mtd:	MTD device structure
 * @ofs:	offset from device start
 * @getchip:	0, if the chip is already selected
 *
 * Check, if the block is bad.
 */
 int pnand_block_bad(struct mtd_info *mtd, loff_t ofs, int getchip)
{
	int page, chipnr, res = 0;
#if defined(CONFIG_PNAND_TEST_DRIVER)
	struct nand_chip *chip = mtd->priv;
#else
	struct nand_chip *chip = mtd_to_nand(mtd);
#endif
	u16 bad;

	page = (int)(ofs >> chip->page_shift) & chip->pagemask;

	if (getchip) {
		chipnr = (int)(ofs >> chip->chip_shift);

		pnand_get_device(chip, mtd, FL_READING);

		/* Select the NAND device */
		chip->select_chip(mtd, chipnr);
	}

	pnand_read_oob_std(mtd, chip, page, 0);


	if(chip->oob_poi[chip->badblockpos] != 0xff)
		return 1;

	if (getchip)
		pnand_release_device(mtd);

	return res;
}

#define NOTALIGNED(x)	(x & (chip->subpagesize - 1)) != 0
/**
 * pnand_do_write_ops - [Internal] NAND write with ECC
 * @mtd:	MTD device structure
 * @to:		offset to write to
 * @ops:	oob operations description structure
 *
 * NAND write with ECC
 */
 int pnand_do_write_ops(struct mtd_info *mtd, loff_t to,
			     struct mtd_oob_ops *ops)
{
	int chipnr, realpage, page, blockmask, column;
#if defined(CONFIG_PNAND_TEST_DRIVER)
	struct nand_chip *chip = mtd->priv;
#else
	struct nand_chip *chip = mtd_to_nand(mtd);
#endif
	uint32_t writelen = ops->len;
	uint8_t *oob = ops->oobbuf;
	uint8_t *buf = ops->datbuf;
	int ret, subpage;
	int oob_required = oob ? 1 : 0;

	ops->retlen = 0;
	if (!writelen)
		return 0;

	/* reject writes, which are not page aligned */
	if (NOTALIGNED(to) || NOTALIGNED(ops->len)) {
		printf(KERN_NOTICE "%s: Attempt to write not "
				"page aligned data\n", __func__);
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
	if (pnand_check_wp(mtd))
		return -EIO;

	realpage = (int)(to >> chip->page_shift);
	page = realpage & chip->pagemask;
	blockmask = (1 << (chip->phys_erase_shift - chip->page_shift)) - 1;

	/* Invalidate the page cache, when we write to the cached page */
	if (to <= (chip->pagebuf << chip->page_shift) &&
	    (chip->pagebuf << chip->page_shift) < (to + ops->len))
		chip->pagebuf = -1;

	/* If we're not given explicit OOB data, let it be 0xFF */
	if (likely(!oob))
		memset(chip->oob_poi, 0xff, mtd->oobsize);

	while(1) {
		int bytes = mtd->writesize;
		int cached = writelen > bytes && page != blockmask;
		uint8_t *wbuf = buf;

		/* Partial page write ? */
		if (unlikely(column || writelen < (mtd->writesize - 1))) {
			cached = 0;
			bytes = min_t(int, bytes - column, (int) writelen);
			chip->pagebuf = -1;
			//chip->ecc.read_page(mtd, chip, chip->buffers->databuf, (page<<chip->page_shift));
			chip->ecc.read_page(mtd, chip, chip->buffers->databuf, 1, page);
			memcpy(&chip->buffers->databuf[column], buf, bytes);
			wbuf = chip->buffers->databuf;
		}

		if (unlikely(oob))
			oob = pnand_fill_oob(chip, oob, ops);

		ret = chip->write_page(mtd, chip, column, bytes, wbuf,
					oob_required, page, cached,
					(ops->mode == MTD_OPS_RAW));
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
 * pnand_wait - [DEFAULT]  wait until the command is done
 * @mtd:	MTD device structure
 * @chip:	NAND chip structure
 *
 * Wait for command done. This applies to erase and program only
 * Erase can take up to 400ms and program up to 20ms according to
 * general NAND and SmartMedia specs
 */
/* XXX U-BOOT XXX */
 int pnand_wait(struct mtd_info *mtd, struct nand_chip *this)
{
	unsigned long	timeo;
	int state = this->state;

	if (state == FL_ERASING)
		timeo = (CONFIG_SYS_HZ * 400) / 1000;
	else
		timeo = (CONFIG_SYS_HZ * 20) / 1000;

	timeo = (state == FL_ERASING ? 400 : 20);

	//if ((state == FL_ERASING) && (this->options & NAND_IS_AND))
	//	this->cmdfunc(mtd, NAND_CMD_STATUS_MULTI, -1, -1);
	//else
	//	this->cmdfunc(mtd, NAND_CMD_STATUS, -1, -1);

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
#ifdef PPCHAMELON_NAND_TIMER_HACK
	reset_timer();
	while (get_timer(0) < 10);
#endif /*  PPCHAMELON_NAND_TIMER_HACK */

	state = read_flash_ctrl_reg(FLASH_NF_DATA)&0xff;
	return state;
}


void pnand_enable_hwecc(struct mtd_info *mtd, int mode)
{
	return;
}

/**
 * pnand_verify_buf - [DEFAULT] Verify chip data against buffer
 * @mtd:	MTD device structure
 * @buf:	buffer containing the data to compare
 * @len:	number of bytes to compare
 *
 * Default verify function for 8bit buswith
 */
 #ifdef CONFIG_MTD_NAND_VERIFY_WRITE
 int pnand_verify_buf(struct mtd_info *mtd, const uint8_t *buf, int len)
{
	int i, page=0;
#if defined(CONFIG_PNAND_TEST_DRIVER)
	struct nand_chip *chip = mtd->priv;
#else
	struct nand_chip *chip = mtd_to_nand(mtd);
#endif
	//u_char *tmpdata, *tmpoob;tmpdata ,tmpoob
	size_t  retlen;
	retlen=0;

	page = host->page_addr;

	memset( chip->buffers->databuf, 0, mtd->writesize);
	chip->ecc.read_page(mtd, chip, chip->buffers->databuf, page);

	if(len==mtd->writesize)
	{
		for (i=0; i<len; i++)
		{
			if (buf[i] != chip->buffers->databuf[i])
			{
				printf("Data verify error -> page: %x, byte: %x, buf[i]:%x  chip->buffers->databuf[i]:%x \n",host->page_addr,i, buf[i], chip->buffers->databuf[i]);
				return i;
			}
		}
	}
	else if(len == mtd->oobsize)
	{
		for (i=0; i<len; i++)
		{
			if (buf[i] != chip->oob_poi[i])
			{
				printf("OOB verify error -> page: %x, byte: %x, buf[i]:%x  chip->oob_poi[i]:%x  \n",host->page_addr,i,buf[i],chip->oob_poi[i]);
				return i;
			}
		}
	}
	else
	{
		printf (KERN_WARNING "verify length not match 0x%08x\n", len);

		return -1;
	}

	return 0;
}
#endif

/**
 * pnand_read_buf - [DEFAULT] read chip data into buffer
 * @mtd:	MTD device structure
 * @buf:	buffer to store date
 * @len:	number of bytes to read
 *
 * Default read function for 8bit buswith
 */
 void pnand_read_buf(struct mtd_info *mtd, uint8_t *buf, int len)
{
	int page, col;
#if defined(CONFIG_PNAND_TEST_DRIVER)
	struct nand_chip *this = mtd->priv;
	struct pnand_host *host = nand_chip->priv;
#else
	struct nand_chip *this = mtd_to_nand(mtd);
	struct pnand_host *host = mtd_to_nand(nand_chip);
#endif

	if(len <= (mtd->writesize+ mtd->oobsize))
	{
		page = host->page_addr;
		col  = host->col_addr;

		this->ecc.read_page(mtd, this, this->buffers->databuf, 1, page);
		memcpy(buf, &(this->buffers->databuf[col]), len);

	}

}

/**
 * pnand_write_buf - [DEFAULT] write buffer to chip
 * @mtd:	MTD device structure
 * @buf:	data buffer
 * @len:	number of bytes to write
 *
 * Default write function for 8bit buswith
 */
 void pnand_write_buf(struct mtd_info *mtd, const uint8_t *buf, int len)
{
	int i, page=0,col=0;
#if defined(CONFIG_PNAND_TEST_DRIVER)
	struct nand_chip *chip = mtd->priv;
	struct pnand_host *host = nand_chip->priv;
#else
	struct nand_chip *chip = mtd_to_nand(mtd);
	struct pnand_host *host = mtd_to_nand(nand_chip);
#endif
	size_t  retlen;
	retlen=0;

	/** if(len <= (mtd->writesize+mtd->oobsize))**/
	if(len <= (mtd->writesize+ mtd->oobsize))
	{

		page = host->page_addr;
		col  = host->col_addr;

		chip->ecc.read_page(mtd, chip, chip->buffers->databuf, 1, page);

		for(i=0;i<len;i++)
			chip->buffers->databuf[col+i] = buf[i];
#if defined(CONFIG_PNAND_TEST_DRIVER)
		chip->ecc.write_page(mtd, chip, chip->buffers->databuf, 1);  /* pnand_write_page_hwecc */
#else
		chip->ecc.write_page(mtd, chip, chip->buffers->databuf, 1, page);  /* pnand_write_page_hwecc */
#endif
	}
}

/**
 * pnand_read_byte - [DEFAULT] read one byte from the chip
 * @mtd:	MTD device structure
 *
 * Default read function for 8bit buswith
 */
 uint8_t pnand_read_byte(struct mtd_info *mtd)
{
#if defined(CONFIG_PNAND_TEST_DRIVER)
	struct nand_chip *chip = mtd->priv;
	struct pnand_host *host = nand_chip->priv;
#else
	struct nand_chip *chip = mtd_to_nand(mtd);
	struct pnand_host *host = mtd_to_nand(nand_chip);
#endif
	unsigned int    data=0, page=0, col=0;

	page = host->page_addr;
	col  = host->col_addr;

	pnand_read_page_raw(mtd, chip, chip->buffers->databuf, 1, page);
	data = *(chip->buffers->databuf+col);

	return data&0xff;
}

/**
 * pnand_select_chip - [DEFAULT] control CE line
 * @mtd:	MTD device structure
 * @chipnr:	chipnumber to select, -1 for deselect
 *
 * Default select function for 1 chip devices.
 */
 void pnand_select_chip(struct mtd_info *mtd)
{
#if defined(CONFIG_PNAND_TEST_DRIVER)
	struct nand_chip *chip = mtd->priv;
#else
	struct nand_chip *chip = mtd_to_nand(mtd);
#endif

	switch (NF_ACCESS_CE_ALT) {
	case -1:
		chip_en = NF_ACCESS_CHIP0_EN;
		//printf("chip_en = %d\n", chip_en);
		break;
	case 0:
		chip_en = NF_ACCESS_CHIP0_EN;
		//printf("chip_en = %d\n", chip_en);
		break;
	case 1:
		chip_en = NF_ACCESS_CHIP1_EN;
		//printf("chip_en = %d\n", chip_en);
		break;

	default:
		chip_en = NF_ACCESS_CHIP0_EN;
		//printf("chip_en = %d\n", chip_en);
		/** BUG();**/
	}

}

/*
 * Wait for the ready pin, after a command
 * The timeout is catched later.
 */
 void pnand_wait_ready(struct mtd_info *mtd)
{
#if defined(CONFIG_PNAND_TEST_DRIVER)
	struct nand_chip *chip = mtd->priv;
#else
	struct nand_chip *chip = mtd_to_nand(mtd);
#endif
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
 * pnand_command - [DEFAULT] Send command to NAND device
 * @mtd:	MTD device structure
 * @command:	the command to be sent
 * @column:	the column address for this command, -1 if none
 * @page_addr:	the page address for this command, -1 if none
 *
 * Send command to NAND device. This function is used for small page
 * devices (256/512 Bytes per page)
 */
 void pnand_command(struct mtd_info *mtd, unsigned int command,
			 int column, int page_addr)
{
#if defined(CONFIG_PNAND_TEST_DRIVER)
	register struct nand_chip *chip = mtd->priv;
#else
	register struct nand_chip *chip = mtd_to_nand(mtd);
#endif

	int ctrl = NAND_CTRL_CLE | NAND_CTRL_CHANGE;
	//uint32_t rst_sts_cnt = CONFIG_SYS_NAND_RESET_CNT;
#if defined(CONFIG_PNAND_TEST_DRIVER)
	struct pnand_host *host = nand_chip->priv;
#else
	struct pnand_host *host = mtd_to_nand(nand_chip);
#endif

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
				/** FLASH_WRITE_REG(NF_ACCESS_ADDRESS,column);**/
				host->col_addr = column;

			if (page_addr != -1)
				/** FLASH_WRITE_REG(NF_ACCESS_ADDRESS,opcode|(page_addr<<8));**/
				host->page_addr = page_addr;

		}
		return;

	case NAND_CMD_RESET:
		check_flash_ctrl_status();
		udelay(chip->chip_delay);
		write_flash_ctrl_reg(FLASH_NF_ECC_CONTROL, 0x0); /** disable ecc gen**/
		nf_count.wrd = 0;
		nf_count.bf.nflashRegOobCount = NF_CNT_EMPTY_OOB;
		nf_count.bf.nflashRegDataCount = NF_CNT_EMPTY_DATA;
		nf_count.bf.nflashRegAddrCount = NF_CNT_EMPTY_ADDR;
		nf_count.bf.nflashRegCmdCount = NF_CNT_CMD_1;
		write_flash_ctrl_reg(FLASH_NF_COUNT, nf_count.wrd);

		nf_command.wrd = 0;
		nf_command.bf.nflashRegCmd0 = NAND_CMD_RESET;
		write_flash_ctrl_reg(FLASH_NF_COMMAND, nf_command.wrd); /** write read id command**/
		nf_addr1.wrd = 0;
		write_flash_ctrl_reg(FLASH_NF_ADDRESS_1, nf_addr1.wrd); /** write address 0x00**/
		nf_addr2.wrd = 0;
		write_flash_ctrl_reg(FLASH_NF_ADDRESS_2, nf_addr2.wrd); /** write address 0x00**/

		nf_access.wrd = 0;
		nf_access.bf.nflashCeAlt = NF_ACCESS_CE_ALT;
		/** nf_access.bf.nflashDirWr = ;**/
		nf_access.bf.nflashRegWidth = NF_ACCESS_WiDTH8;

		write_flash_ctrl_reg(FLASH_NF_ACCESS, nf_access.wrd);
		access_start.wrd = 0;
		access_start.bf.nflashRegReq = FLASH_GO;
		access_start.bf.nflashRegCmd = FLASH_WT;
		write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, access_start.wrd);

		tmp_access.wrd = 0;
		tmp_access.bf.nflashRegReq = 1;
		reg_wait(FLASH_FLASH_ACCESS_START, tmp_access.wrd, 0, 1000);
		access_start.wrd=read_flash_ctrl_reg(FLASH_FLASH_ACCESS_START);

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

	pnand_wait_ready(mtd);
}

/**
 * pnand_command_lp - [DEFAULT] Send command to NAND large page device
 * @mtd:	MTD device structure
 * @command:	the command to be sent
 * @column:	the column address for this command, -1 if none
 * @page_addr:	the page address for this command, -1 if none
 *
 * Send command to NAND device. This is the version for the new large page
 * devices We dont have the separate regions as we have in the small page
 * devices.  We must emulate NAND_CMD_READOOB to keep the code compatible.
 */
 void pnand_command_lp(struct mtd_info *mtd, unsigned int command,
			    int column, int page_addr)
{
#if defined(CONFIG_PNAND_TEST_DRIVER)
	register struct nand_chip *chip = mtd->priv;
	struct pnand_host *host = nand_chip->priv;
#else
	register struct nand_chip *chip = mtd_to_nand(mtd);
	struct pnand_host *host = mtd_to_nand(nand_chip);
#endif
	uint32_t rst_sts_cnt = CONFIG_SYS_NAND_RESET_CNT;

	/* Emulate NAND_CMD_READOOB */
	if (command == NAND_CMD_READOOB) {
		column += mtd->writesize;
		command = NAND_CMD_READ0;
	}

	/* Command latch cycle */
	chip->cmd_ctrl(mtd, command & 0xff,
		       NAND_NCE | NAND_CLE | NAND_CTRL_CHANGE);

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
			chip->cmd_ctrl(mtd, page_addr >> 8,
				       NAND_NCE | NAND_ALE);
			/* One more address cycle for devices > 128MiB */
			if (chip->chipsize > (128 << 20))
				chip->cmd_ctrl(mtd, page_addr >> 16,
					       NAND_NCE | NAND_ALE);
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
				/** FLASH_WRITE_REG(NF_ACCESS_ADDRESS,column);**/
				host->col_addr = column;

			if (page_addr != -1)
				/** FLASH_WRITE_REG(NF_ACCESS_ADDRESS,opcode|(page_addr<<8));**/
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
		nf_count.wrd = 0;
		nf_count.bf.nflashRegOobCount = NF_CNT_EMPTY_OOB;
		nf_count.bf.nflashRegDataCount = NF_CNT_EMPTY_DATA;
		nf_count.bf.nflashRegAddrCount = NF_CNT_EMPTY_ADDR;
		nf_count.bf.nflashRegCmdCount = NF_CNT_CMD_1;
		write_flash_ctrl_reg(FLASH_NF_COUNT, nf_count.wrd);

		nf_command.wrd = 0;
		nf_command.bf.nflashRegCmd0 = NAND_CMD_RESET;
		write_flash_ctrl_reg(FLASH_NF_COMMAND, nf_command.wrd); /** write read id command**/
		nf_addr1.wrd = 0;
		write_flash_ctrl_reg(FLASH_NF_ADDRESS_1, nf_addr1.wrd); /** write address 0x00**/
		nf_addr2.wrd = 0;
		write_flash_ctrl_reg(FLASH_NF_ADDRESS_2, nf_addr2.wrd); /** write address 0x00**/

		nf_access.wrd = 0;
		nf_access.bf.nflashCeAlt = NF_ACCESS_CE_ALT;
		/** nf_access.bf.nflashDirWr = ;**/
		nf_access.bf.nflashRegWidth = NF_ACCESS_WiDTH8;

		write_flash_ctrl_reg(FLASH_NF_ACCESS, nf_access.wrd);
		access_start.wrd = 0;
		access_start.bf.nflashRegReq = FLASH_GO;
		access_start.bf.nflashRegCmd = FLASH_WT;
		write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, access_start.wrd);

		tmp_access.wrd = 0;
		tmp_access.bf.nflashRegReq = 1;
		reg_wait(FLASH_FLASH_ACCESS_START, tmp_access.wrd, 0, 1000);
		access_start.wrd=read_flash_ctrl_reg(FLASH_FLASH_ACCESS_START);

		udelay(100);
		return;

	case NAND_CMD_RNDOUT:
		/* No ready / busy check necessary */
		chip->cmd_ctrl(mtd, NAND_CMD_RNDOUTSTART,
			       NAND_NCE | NAND_CLE | NAND_CTRL_CHANGE);
		chip->cmd_ctrl(mtd, NAND_CMD_NONE,
			       NAND_NCE | NAND_CTRL_CHANGE);
		return;

	case NAND_CMD_READ0:
		chip->cmd_ctrl(mtd, NAND_CMD_READSTART,
			       NAND_NCE | NAND_CLE | NAND_CTRL_CHANGE);
		chip->cmd_ctrl(mtd, NAND_CMD_NONE,
			       NAND_NCE | NAND_CTRL_CHANGE);

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

	pnand_wait_ready(mtd);
}

/*------------------------------------------------------------------------------
* 	page_write_direct_hwecc : nand page write direct hwecc
* 	nand_chip:   nand chip config
*	page_addr : flash access address
*	buff : data buffer
*	len : data len
-----------------------------------------------------------------------------+*/
 int page_write_hwecc_direct(struct mtd_info *mtd, struct nand_chip *chip, ca_uint8* page_addr, ca_uint8* buff, int len)
{
	FLASH_NF_BCH_STATUS_t	tmp_bsc_sts;
	FLASH_NF_ECC_STATUS_t	tmp_ecc_sts;

	int i, j, page_size, ecc_page;
	int eccbytes = chip->ecc.bytes;
	int eccsteps = chip->ecc.steps;
	int eccsize = chip->ecc.size;
	/*uint8_t *ecc_calc = chip->buffers->ecccalc;*/
	/*const uint8_t *p = buf;*/
	uint32_t *eccpos = chip->ecc.layout->eccpos, *addr, reg_bch_gen;
	page_size = len;

	check_flash_ctrl_status();

	nf_ecc_reset.wrd = 3;
	nf_ecc_reset.bf.eccClear = ECC_CLR;
	nf_ecc_reset.bf.fifoClear = FIFO_CLR;
	write_flash_ctrl_reg(FLASH_NF_ECC_RESET, nf_ecc_reset.wrd);

	flash_int_sts.bf.regIrq = 1;
	write_flash_ctrl_reg(FLASH_FLASH_INTERRUPT, flash_int_sts.wrd);

	nf_ecc_reset.wrd = 0;
	nf_ecc_reset.bf.eccClear = 1;
	write_flash_ctrl_reg(FLASH_NF_ECC_RESET, nf_ecc_reset.wrd);

	if(NF_ECC_HM_BCH >= ECC_BCH_8)
	{
		nf_bch_ctrl.wrd = 0;
		nf_bch_ctrl.bf.bchErrCap = NF_ECC_HM_BCH - ECC_BCH_8;
		nf_bch_ctrl.bf.bchEn = 1;
		nf_bch_ctrl.bf.bchOpcode = 1;//BCH_DECODE;
		write_flash_ctrl_reg(FLASH_NF_BCH_CONTROL, nf_bch_ctrl.wrd);
	}
	else
	{
		nf_ecc_ctl.wrd = 0;
		if(NF_ECC_HM_BCH == ECC_HM_256B_3B) {
			nf_ecc_ctl.bf.eccGenMode = ECC_GEN_256;	//256 bytes boundary
		} else {
			nf_ecc_ctl.bf.eccGenMode = ECC_GEN_512;	//512 bytes boundary
		}
			nf_ecc_ctl.bf.eccEn = ECC_ENABLE;
			write_flash_ctrl_reg(FLASH_NF_ECC_CONTROL, nf_ecc_ctl.wrd);
	}

	if(NF_ECC_HM_BCH >= ECC_BCH_8)
	{
		eccsteps = (page_size/BCH_DATA_UNIT);

		if(NF_ECC_HM_BCH == ECC_BCH_8)
			eccsize = BCH_GF_PARAM_M;
		else if(NF_ECC_HM_BCH == ECC_BCH_16)
			eccsize = 2* BCH_GF_PARAM_M;
		else if(NF_ECC_HM_BCH == ECC_BCH_24)
			eccsize = 3* BCH_GF_PARAM_M;
		else if(NF_ECC_HM_BCH == ECC_BCH_40)
			eccsize = 5* BCH_GF_PARAM_M;

		eccbytes = eccsteps * eccsize;
	}
	else
	{
		if(NF_ECC_HM_BCH == ECC_HM_256B_3B) {
			ecc_page=256;
		} else {
			ecc_page=512;
		}
		eccsteps = (page_size/ecc_page);
		eccbytes = (page_size/ecc_page)*3;
		eccsize = 3;
	}

	//write page data to gen ecc
	memcpy((void *)page_addr, (void *)buff, page_size);

	if(NF_ECC_HM_BCH >= ECC_BCH_8) //ecc mode : BCH
	{
		//nf_bch_sts.wrd=read_flash_ctrl_reg(FLASH_NF_BCH_STATUS);
		tmp_bsc_sts.wrd = 0;
		tmp_bsc_sts.bf.bchGenDone = 1;
		reg_wait(FLASH_NF_BCH_STATUS, tmp_bsc_sts.wrd , tmp_bsc_sts.wrd, 1000);
	}
	else	 //ecc mode :HM
	{
		nf_ecc_sts.wrd=read_flash_ctrl_reg(FLASH_NF_ECC_STATUS);
		tmp_ecc_sts.wrd = 0;
		tmp_ecc_sts.bf.eccDone = 1;
		reg_wait(FLASH_NF_ECC_STATUS, tmp_ecc_sts.wrd , tmp_ecc_sts.wrd, 1000);

		udelay(10);

		nf_ecc_ctl.wrd = read_flash_ctrl_reg(FLASH_NF_ECC_CONTROL);
		nf_ecc_ctl.bf.eccEn= 0;
		write_flash_ctrl_reg(FLASH_NF_ECC_CONTROL, nf_ecc_ctl.wrd);  /*disable ecc gen*/
	}
	//printf("%s(%d) eccsteps=%d, eccbytes=%d\n", __func__, __LINE__,eccsteps,eccbytes);
	/*printf("write page ecc(page_addr: 0x%x) : ", page_addr);*/
	for (i = 0,j = 0; eccsteps; eccsteps--, i++, j += eccbytes)
	{
		if (NF_ECC_HM_BCH <= ECC_HM_512B_3B)
		{
			nf_ecc_gen0.wrd = read_flash_ctrl_reg(FLASH_NF_ECC_GEN0 + 4*i);
			chip->oob_poi[eccpos[j]] = nf_ecc_gen0.wrd & 0xff;
			chip->oob_poi[eccpos[j+1]] = (nf_ecc_gen0.wrd >> 8) & 0xff;
			chip->oob_poi[eccpos[j+2]] = (nf_ecc_gen0.wrd >> 16) & 0xff;
			/* printf("%x ", nf_ecc_gen0.wrd); */
		}
		if (NF_ECC_HM_BCH >= ECC_BCH_8)
		{
			reg_bch_gen = read_flash_ctrl_reg(FLASH_NF_BCH_GEN0_0 + 72*i);
			chip->oob_poi[eccpos[j]] = reg_bch_gen & 0xff;
			chip->oob_poi[eccpos[j+1]] = (reg_bch_gen >> 8) & 0xff;
			chip->oob_poi[eccpos[j+2]] = (reg_bch_gen >> 16) & 0xff;
			chip->oob_poi[eccpos[j+3]] = (reg_bch_gen >> 24) & 0xff;
			reg_bch_gen = read_flash_ctrl_reg(FLASH_NF_BCH_GEN0_1 + 72*i);
			chip->oob_poi[eccpos[j+4]] = reg_bch_gen & 0xff;
			chip->oob_poi[eccpos[j+5]] = (reg_bch_gen >> 8) & 0xff;
			chip->oob_poi[eccpos[j+6]] = (reg_bch_gen >> 16) & 0xff;
			chip->oob_poi[eccpos[j+7]] = (reg_bch_gen >> 24) & 0xff;
			reg_bch_gen = read_flash_ctrl_reg(FLASH_NF_BCH_GEN0_2 + 72*i);
			chip->oob_poi[eccpos[j+8]] = reg_bch_gen & 0xff;
			chip->oob_poi[eccpos[j+9]] = (reg_bch_gen >> 8) & 0xff;
			chip->oob_poi[eccpos[j+10]] = (reg_bch_gen >> 16) & 0xff;
			chip->oob_poi[eccpos[j+11]] = (reg_bch_gen >> 24) & 0xff;
			reg_bch_gen = read_flash_ctrl_reg(FLASH_NF_BCH_GEN0_3 + 72*i);
			chip->oob_poi[eccpos[j+12]] = reg_bch_gen & 0xff;
			chip->oob_poi[eccpos[j+13]] = (reg_bch_gen >> 8 ) & 0xff;
		}
		if (NF_ECC_HM_BCH >= ECC_BCH_16)
		{
			reg_bch_gen = read_flash_ctrl_reg(FLASH_NF_BCH_GEN0_3 + 72*i);
			chip->oob_poi[eccpos[j+14]] = (reg_bch_gen >> 16) & 0xff;
			chip->oob_poi[eccpos[j+15]] = (reg_bch_gen >> 24)& 0xff;
			reg_bch_gen = read_flash_ctrl_reg( FLASH_NF_BCH_GEN0_4 + 72*i);
			chip->oob_poi[eccpos[j+16]] = reg_bch_gen & 0xff;
			chip->oob_poi[eccpos[j+17]] = (reg_bch_gen >> 8 ) & 0xff;
			chip->oob_poi[eccpos[j+18]] = (reg_bch_gen >> 16) & 0xff;
			chip->oob_poi[eccpos[j+19]] = (reg_bch_gen >> 24)& 0xff;
			reg_bch_gen = read_flash_ctrl_reg( FLASH_NF_BCH_GEN0_5 + 72*i);
			chip->oob_poi[eccpos[j+20]] = reg_bch_gen & 0xff;
			chip->oob_poi[eccpos[j+21]] = (reg_bch_gen >> 8 ) & 0xff;
			chip->oob_poi[eccpos[j+22]] = (reg_bch_gen >> 16) & 0xff;
			chip->oob_poi[eccpos[j+23]] = (reg_bch_gen >> 24)& 0xff;
			reg_bch_gen = read_flash_ctrl_reg( FLASH_NF_BCH_GEN0_6 + 72*i);
			chip->oob_poi[eccpos[j+24]] = reg_bch_gen & 0xff;
			chip->oob_poi[eccpos[j+25]] = (reg_bch_gen >> 8 ) & 0xff;
			chip->oob_poi[eccpos[j+26]] = (reg_bch_gen >> 16) & 0xff;
			chip->oob_poi[eccpos[j+27]] = (reg_bch_gen >> 24)& 0xff;

		}
		if (NF_ECC_HM_BCH >= ECC_BCH_24)
		{
			reg_bch_gen = read_flash_ctrl_reg( FLASH_NF_BCH_GEN0_7 + 72*i);
			chip->oob_poi[eccpos[j+28]] = reg_bch_gen & 0xff;
			chip->oob_poi[eccpos[j+29]] = (reg_bch_gen >> 8 ) & 0xff;
			chip->oob_poi[eccpos[j+30]] = (reg_bch_gen >> 16) & 0xff;
			chip->oob_poi[eccpos[j+31]] = (reg_bch_gen >> 24)& 0xff;
			reg_bch_gen = read_flash_ctrl_reg( FLASH_NF_BCH_GEN0_8 + 72*i);
			chip->oob_poi[eccpos[j+32]] = reg_bch_gen & 0xff;
			chip->oob_poi[eccpos[j+33]] = (reg_bch_gen >> 8 ) & 0xff;
			chip->oob_poi[eccpos[j+34]] = (reg_bch_gen >> 16) & 0xff;
			chip->oob_poi[eccpos[j+35]] = (reg_bch_gen >> 24)& 0xff;
			reg_bch_gen = read_flash_ctrl_reg( FLASH_NF_BCH_GEN0_9 + 72*i);
			chip->oob_poi[eccpos[j+36]] = reg_bch_gen & 0xff;
			chip->oob_poi[eccpos[j+37]] = (reg_bch_gen >> 8 ) & 0xff;
			chip->oob_poi[eccpos[j+38]] = (reg_bch_gen >> 16) & 0xff;
			chip->oob_poi[eccpos[j+39]] = (reg_bch_gen >> 24)& 0xff;
			reg_bch_gen = read_flash_ctrl_reg(FLASH_NF_BCH_GEN0_10 + 72*i);
			chip->oob_poi[eccpos[j+40]] = reg_bch_gen & 0xff;
			chip->oob_poi[eccpos[j+41]] = (reg_bch_gen >> 8 ) & 0xff;
		}
		if (NF_ECC_HM_BCH >= ECC_BCH_40)
		{
			reg_bch_gen = read_flash_ctrl_reg(FLASH_NF_BCH_GEN0_10 + 72*i);
			chip->oob_poi[eccpos[j+42]] = (reg_bch_gen >> 16) & 0xff;
			chip->oob_poi[eccpos[j+43]] = (reg_bch_gen >> 24)& 0xff;
			reg_bch_gen = read_flash_ctrl_reg( FLASH_NF_BCH_GEN0_11 + 72*i);
			chip->oob_poi[eccpos[j+44]] = reg_bch_gen & 0xff;
			chip->oob_poi[eccpos[j+45]] = (reg_bch_gen >> 8 ) & 0xff;
			chip->oob_poi[eccpos[j+46]] = (reg_bch_gen >> 16) & 0xff;
			chip->oob_poi[eccpos[j+47]] = (reg_bch_gen >> 24)& 0xff;
			reg_bch_gen = read_flash_ctrl_reg( FLASH_NF_BCH_GEN0_12 + 72*i);
			chip->oob_poi[eccpos[j+48]] = reg_bch_gen & 0xff;
			chip->oob_poi[eccpos[j+49]] = (reg_bch_gen >> 8 ) & 0xff;
			chip->oob_poi[eccpos[j+50]] = (reg_bch_gen >> 16) & 0xff;
			chip->oob_poi[eccpos[j+51]] = (reg_bch_gen >> 24)& 0xff;
			reg_bch_gen = read_flash_ctrl_reg( FLASH_NF_BCH_GEN0_13 + 72*i);
			chip->oob_poi[eccpos[j+52]] = reg_bch_gen & 0xff;
			chip->oob_poi[eccpos[j+53]] = (reg_bch_gen >> 8 ) & 0xff;
			chip->oob_poi[eccpos[j+54]] = (reg_bch_gen >> 16) & 0xff;
			chip->oob_poi[eccpos[j+55]] = (reg_bch_gen >> 24)& 0xff;
			reg_bch_gen = read_flash_ctrl_reg( FLASH_NF_BCH_GEN0_14 + 72*i);
			chip->oob_poi[eccpos[j+56]] = reg_bch_gen & 0xff;
			chip->oob_poi[eccpos[j+57]] = (reg_bch_gen >> 8 ) & 0xff;
			chip->oob_poi[eccpos[j+58]] = (reg_bch_gen >> 16) & 0xff;
			chip->oob_poi[eccpos[j+59]] = (reg_bch_gen >> 24)& 0xff;
			reg_bch_gen = read_flash_ctrl_reg( FLASH_NF_BCH_GEN0_15 + 72*i);
			chip->oob_poi[eccpos[j+60]] = reg_bch_gen & 0xff;
			chip->oob_poi[eccpos[j+61]] = (reg_bch_gen >> 8 ) & 0xff;
			chip->oob_poi[eccpos[j+62]] = (reg_bch_gen >> 16) & 0xff;
			chip->oob_poi[eccpos[j+63]] = (reg_bch_gen >> 24)& 0xff;
			reg_bch_gen = read_flash_ctrl_reg( FLASH_NF_BCH_GEN0_16 + 72*i);
			chip->oob_poi[eccpos[j+64]] = reg_bch_gen & 0xff;
			chip->oob_poi[eccpos[j+65]] = (reg_bch_gen >> 8 ) & 0xff;
			chip->oob_poi[eccpos[j+66]] = (reg_bch_gen >> 16) & 0xff;
			chip->oob_poi[eccpos[j+67]] = (reg_bch_gen >> 24)& 0xff;
			reg_bch_gen = read_flash_ctrl_reg( FLASH_NF_BCH_GEN0_17 + 72*i);
			chip->oob_poi[eccpos[j+68]] = reg_bch_gen & 0xff;
			chip->oob_poi[eccpos[j+69]] = (reg_bch_gen >> 8 ) & 0xff;
		}
	}
	/*printf("\n");*/
	if (NF_ECC_HM_BCH >= ECC_BCH_8)
	{
		nf_bch_ctrl.wrd = read_flash_ctrl_reg(FLASH_NF_BCH_CONTROL);  /*disable ecc gen*/
		nf_bch_ctrl.bf.bchEn = BCH_DISABLE;
		write_flash_ctrl_reg(FLASH_NF_BCH_CONTROL, nf_bch_ctrl.wrd);

		/* jenfeng clear erase tag*/
		*(chip->oob_poi+ chip->ecc.layout->oobfree[0].offset +  chip->ecc.layout->oobfree[0].length)= 0;
	}

	//write page data
	memcpy((void *)page_addr, (void *)buff, page_size);

	//write oob
	memcpy((void *)(page_addr + page_size), (void *)chip->oob_poi, mtd->oobsize);

	/*chip->write_buf(mtd, chip->oob_poi, mtd->oobsize);*/
	printf("%s(%d) finished!!!\n", __func__, __LINE__);
	flash_reset();
	return 0;
}

/**
 * pnand_write_buf_hwecc - [DEFAULT] write buffer to chip
 * @addr:	flash address
 * @buf:	data buffer
 * @len:	number of bytes to write
 * @boot_cfg:   pnand config
 *
 * Default write function for 8bit buswith
 */
int pnand_write_buf_hwecc_direct(struct mtd_info *mtd, uint8_t* addr, uint8_t* buff, int len)
{
#if defined(CONFIG_PNAND_TEST_DRIVER)
	struct nand_chip *chip = mtd->priv;
#else
	struct nand_chip *chip = mtd_to_nand(mtd);
#endif
	size_t  rlen;
	rlen=0;
	int page_size = mtd->writesize;
	uint8_t *addr_align;

	addr_align =(uint8_t *)( (uint32_t) addr & ~((uint32_t)page_size-1));

	/** if(len <= (mtd->writesize+mtd->oobsize))**/
	if (len > 0)
	{
		if (len < page_size) {
			//printf(".");
			printf("%s(%d)\n", __func__, __LINE__);
			page_write_hwecc_direct(mtd, chip, (void *)addr, (void *)buff, page_size);
			rlen = page_size;
			len = 0;
		}
		while (len >= page_size) {
			//printf(".");
			printf("%s(%d) addr_align=0x%x\n", __func__, __LINE__, (uint32_t)addr_align);
				page_write_hwecc_direct(mtd, chip, (void *)addr_align, (void *)buff, page_size);
			buff += page_size;
			addr_align += page_size;
			rlen += page_size;
			if (len > page_size) {
				len -= page_size;
			} else {
				printf("\n");
				len = 0;
				break;
			}
		}
		if (len > 0) {
			//printf(".\n");
			printf("%s(%d)\n", __func__, __LINE__);
				page_write_hwecc_direct(mtd, chip, (void *)addr_align, (void *)buff, page_size);
			rlen += page_size;
			len = 0;
		}
	}

	return rlen;
}

void pnand_block_write(struct mtd_info *mtd, unsigned int offset, size_t len, void *buffer)
{
	unsigned int flash_addr = offset | FLASH_BASE;
	unsigned int length_write = 0;

	//memcpy((void *)flash_addr, (void *)buffer, len);
	//page_write(&boot_cfg, (void *)flash_addr, (void *)buffer, len);
	length_write  = pnand_write_buf_hwecc_direct(mtd, (void *)flash_addr, (void *)buffer, len);
	printf("parallel nand write %d bytes \n", length_write);
	flash_reset();
}


/*------------------------------------------------------------------------------
* 	page_read : nand page read
*	page_add : access address
*	page_data : read buffer
*	oob_data : oob data buffer
*	page_size : read size
-----------------------------------------------------------------------------+*/
int page_read_hwecc_direct(struct mtd_info *mtd, struct nand_chip *chip, uint8_t* page_add, uint8_t* page_data, uint8_t* oob_data, int page_size)
{
	FLASH_NF_ECC_RESET_t		nf_ecc_reset;
	FLASH_NF_ECC_CONTROL_t		nf_ecc_ctl;
	FLASH_NF_BCH_CONTROL_t		nf_bch_ctrl;
	FLASH_NF_ECC_STATUS_t		nf_ecc_sts, nf_ecc_sts_msk;
	FLASH_NF_BCH_STATUS_t		nf_bch_sts, nf_bch_sts_msk;
	FLASH_NF_ECC_OOB_t		nf_ecc_oob;
	FLASH_NF_BCH_ERROR_LOC01_t	nf_bch_err_loc01;

	ca_uint32 errloc0,errloc1, bch_oob;
	ca_uint8 *ecc_p;
	int i, j;
	int oobsize = mtd->oobsize;
	int eccsize = chip->ecc.size;
	int eccbytes = chip->ecc.bytes;
	int eccsteps = chip->ecc.steps;
	uint8_t *ecc_code = chip->buffers->ecccode;
	uint32_t *eccpos = chip->ecc.layout->eccpos;
	if(reg_wait(FLASH_STATUS, NF_STS_MSK, NF_STS_VAL, NF_STS_PCNT) == FALSE)
		flash_reset();

	//ecc reset
	nf_ecc_reset.wrd = 0;
	nf_ecc_reset.bf.eccClear = ECC_CLR;
	nf_ecc_reset.bf.fifoClear = 1;
	write_flash_ctrl_reg(FLASH_NF_ECC_RESET, nf_ecc_reset.wrd);

	if(NF_ECC_HM_BCH >= ECC_BCH_8)
	{
		nf_bch_ctrl.wrd = 0;
		nf_bch_ctrl.bf.bchErrCap = NF_ECC_HM_BCH - ECC_BCH_8;
		nf_bch_ctrl.bf.bchEn = 1;
		nf_bch_ctrl.bf.bchOpcode = 1;//BCH_DECODE;
		write_flash_ctrl_reg(FLASH_NF_BCH_CONTROL, nf_bch_ctrl.wrd);
	}
	else
	{
		nf_ecc_ctl.wrd = 0;
		if(NF_ECC_HM_BCH == ECC_HM_256B_3B)
			nf_ecc_ctl.bf.eccGenMode = 0;	//256 bytes boundary
		else
			nf_ecc_ctl.bf.eccGenMode = 1;	//512 bytes boundary
		nf_ecc_ctl.bf.eccEn = 1;
		write_flash_ctrl_reg(FLASH_NF_ECC_CONTROL, nf_ecc_ctl.wrd);
	}

	//set oob size
	nf_count.wrd = 0;
	nf_count.bf.nflashRegDataCount = page_size-1;
	nf_count.bf.nflashRegOobCount = oobsize-1;
	write_flash_ctrl_reg(FLASH_NF_COUNT, nf_count.wrd);

	//set direct access nflashExtAddr, set and let HW switch access area window
	write_flash_ctrl_reg(FLASH_NF_ACCESS, 0);

	//read page data
	memcpy(page_data, page_add , page_size);

	memcpy(oob_data, (page_add + page_size), oobsize);

	if(NF_ECC_HM_BCH >= ECC_BCH_8) //ecc mode : BCH
	{
		nf_bch_sts_msk.wrd = 0;
		nf_bch_sts_msk.bf.bchGenDone = 1;
		if(reg_wait(FLASH_NF_BCH_STATUS, nf_bch_sts_msk.wrd, nf_bch_sts_msk.wrd, NF_START_BCNT)== FALSE )
			goto err_pageread;
	}
	else	 //ecc mode :HM
	{

		nf_ecc_sts_msk.wrd = 0;
		nf_ecc_sts_msk.bf.eccDone = 1;
		if(reg_wait(FLASH_NF_ECC_STATUS, nf_ecc_sts_msk.wrd, nf_ecc_sts_msk.wrd, NF_START_BCNT)== FALSE )
			goto err_pageread;

		//diable ecc and make sure in correct ecc mode
		nf_ecc_ctl.wrd = read_flash_ctrl_reg( FLASH_NF_ECC_CONTROL);    /**disable ecc gen**/
		nf_ecc_ctl.bf.eccEn= 0;
		write_flash_ctrl_reg(FLASH_NF_ECC_CONTROL, nf_ecc_ctl.wrd);	/**disable ecc gen**/
	}



	//*ecc check... free byte need > (oobsize - rest and not 8 bytes bytes - bad block check byte - byte indicate page not program) **/

	if(eccbytes > (oobsize - (oobsize%8) - 2) || eccbytes ==0)
		return FALSE;

	//copy ecc data of oob to new oob array for ease to cpmpare
	for(i=eccpos, j=0;i<oobsize;i++,j++)
	{
		if(i==markp)
			i++;
		ecc_code[j] = oob_data[i];
	}

	ecc_p = page_data;
	for(i=0;i<eccbytes;i+=eccsize)
	{
		if(NF_ECC_HM_BCH <= ECC_HM_512B_3B)
		{
			nf_ecc_oob.wrd = ecc_code[i] | ecc_code[i+1]<<8 | ecc_code[i+2]<<16;
			write_flash_ctrl_reg(FLASH_NF_ECC_OOB, nf_ecc_oob.wrd);
			nf_ecc_ctl.wrd = read_flash_ctrl_reg(FLASH_NF_ECC_CONTROL);
			nf_ecc_ctl.bf.eccCodeSel = (i/eccsize);
			write_flash_ctrl_reg(FLASH_NF_ECC_CONTROL, nf_ecc_ctl.wrd);
			nf_ecc_ctl.wrd = read_flash_ctrl_reg(FLASH_NF_ECC_CONTROL);	/* Dummy read flush write */
			nf_ecc_sts.wrd = read_flash_ctrl_reg(FLASH_NF_ECC_STATUS);
			switch(nf_ecc_sts.bf.eccStatus)
			{
				case ECC_1BIT_DATA_ERR:
					ecc_p[nf_ecc_sts.bf.eccErrByte] ^= (1 << nf_ecc_sts.bf.eccErrBit);
					break;
				case ECC_UNCORRECTABLE:
					goto err_pageread;
			}
		}
		else if(NF_ECC_HM_BCH >= ECC_BCH_8)
		{
			bch_oob = ecc_code[i] | ecc_code[i+1]<<8 | ecc_code[i+2]<<16 | ecc_code[i+3]<<24;
			write_flash_ctrl_reg(FLASH_NF_BCH_OOB0, bch_oob);
			bch_oob = ecc_code[i+4] | ecc_code[i+5]<<8 | ecc_code[i+6]<<16 | ecc_code[i+7]<<24;
			write_flash_ctrl_reg(FLASH_NF_BCH_OOB1, bch_oob);
			bch_oob = ecc_code[i+8] | ecc_code[i+9]<<8 | ecc_code[i+10]<<16 | ecc_code[i+11]<<24;
			write_flash_ctrl_reg(FLASH_NF_BCH_OOB2, bch_oob);
			bch_oob = ecc_code[i+12] | ecc_code[i+13]<<8;
			write_flash_ctrl_reg(FLASH_NF_BCH_OOB3, bch_oob);

			if(NF_ECC_HM_BCH >= ECC_BCH_16){
				bch_oob = ecc_code[i+12] | ecc_code[i+13]<<8 | ecc_code[i+14]<<16 | ecc_code[i+15]<<24;
				write_flash_ctrl_reg(FLASH_NF_BCH_OOB3, bch_oob);
				bch_oob = ecc_code[i+16] | ecc_code[i+17]<<8 | ecc_code[i+18]<<16 | ecc_code[i+19]<<24;
				write_flash_ctrl_reg(FLASH_NF_BCH_OOB4, bch_oob);
				bch_oob = ecc_code[i+20] | ecc_code[i+21]<<8 | ecc_code[i+22]<<16 | ecc_code[i+23]<<24;
				write_flash_ctrl_reg(FLASH_NF_BCH_OOB5, bch_oob);
				bch_oob = ecc_code[i+24] | ecc_code[i+25]<<8 | ecc_code[i+26]<<16 | ecc_code[i+27]<<24;
				write_flash_ctrl_reg(FLASH_NF_BCH_OOB6, bch_oob);
			}
			if(NF_ECC_HM_BCH >= ECC_BCH_24){
				bch_oob = ecc_code[i+28] | ecc_code[i+29]<<8 | ecc_code[i+30]<<16 | ecc_code[i+31]<<24;
				write_flash_ctrl_reg(FLASH_NF_BCH_OOB7, bch_oob);
				bch_oob = ecc_code[i+32] | ecc_code[i+33]<<8 | ecc_code[i+34]<<16 | ecc_code[i+35]<<24;
				write_flash_ctrl_reg(FLASH_NF_BCH_OOB8, bch_oob);
				bch_oob = ecc_code[i+36] | ecc_code[i+37]<<8 | ecc_code[i+38]<<16 | ecc_code[i+39]<<24;
				write_flash_ctrl_reg(FLASH_NF_BCH_OOB9, bch_oob);
				bch_oob = ecc_code[i+40] | ecc_code[i+41]<<8;
				write_flash_ctrl_reg(FLASH_NF_BCH_OOB10, bch_oob);
			}
			if(NF_ECC_HM_BCH >= ECC_BCH_40){
				bch_oob = ecc_code[i+40] | ecc_code[i+41]<<8 | ecc_code[i+42]<<16 | ecc_code[i+43]<<24;
				write_flash_ctrl_reg(FLASH_NF_BCH_OOB10, bch_oob);
				bch_oob = ecc_code[i+44] | ecc_code[i+45]<<8 | ecc_code[i+46]<<16 | ecc_code[i+47]<<24;
				write_flash_ctrl_reg(FLASH_NF_BCH_OOB11, bch_oob);
				bch_oob = ecc_code[i+48] | ecc_code[i+49]<<8 | ecc_code[i+50]<<16 | ecc_code[i+51]<<24;
				write_flash_ctrl_reg(FLASH_NF_BCH_OOB12, bch_oob);
				bch_oob = ecc_code[i+52] | ecc_code[i+53]<<8 | ecc_code[i+54]<<16 | ecc_code[i+55]<<24;
				write_flash_ctrl_reg(FLASH_NF_BCH_OOB13, bch_oob);
				bch_oob = ecc_code[i+56] | ecc_code[i+57]<<8 | ecc_code[i+58]<<16 | ecc_code[i+59]<<24;
				write_flash_ctrl_reg(FLASH_NF_BCH_OOB14, bch_oob);
				bch_oob = ecc_code[i+60] | ecc_code[i+61]<<8 | ecc_code[i+62]<<16 | ecc_code[i+63]<<24;
				write_flash_ctrl_reg(FLASH_NF_BCH_OOB15, bch_oob);
				bch_oob = ecc_code[i+64] | ecc_code[i+65]<<8 | ecc_code[i+66]<<16 | ecc_code[i+67]<<24;
				write_flash_ctrl_reg(FLASH_NF_BCH_OOB16, bch_oob);
				bch_oob = ecc_code[i+68] | ecc_code[i+69]<<8;
				write_flash_ctrl_reg(FLASH_NF_BCH_OOB17, bch_oob);
			}

			// Set CodeSel field for BCH decode.
			nf_bch_ctrl.wrd = read_flash_ctrl_reg(FLASH_NF_BCH_CONTROL);
			nf_bch_ctrl.bf.bchCodeSel = (i/eccsize);
			nf_bch_ctrl.bf.bchCompare = 0;
			write_flash_ctrl_reg(FLASH_NF_BCH_CONTROL, nf_bch_ctrl.wrd);
			nf_bch_sts.wrd=read_flash_ctrl_reg(FLASH_NF_BCH_STATUS);

			/* Start compare only after CodeSel is set. There is HW racing if set compare together with CodeSel! */
			nf_bch_ctrl.bf.bchCompare = 1;
			write_flash_ctrl_reg(FLASH_NF_BCH_CONTROL, nf_bch_ctrl.wrd);


			nf_bch_sts_msk.wrd = 0;
			nf_bch_sts_msk.bf.bchDecDone = 1;
			if(reg_wait(FLASH_NF_BCH_STATUS, nf_bch_sts_msk.wrd, nf_bch_sts_msk.wrd, NF_START_BCNT)== FALSE )
				goto err_pageread;
			nf_bch_sts.wrd=read_flash_ctrl_reg(FLASH_NF_BCH_STATUS);
			switch(nf_bch_sts.bf.bchDecStatus)
			{
				//case BCH_ING:
				//	break;
				case BCH_CORRECTABLE_ERR:
					for(j=0;j<((nf_bch_sts.bf.bchErrNum+1)/2);j++)
					{
						nf_bch_err_loc01.wrd = read_flash_ctrl_reg(FLASH_NF_BCH_ERROR_LOC01 + j*4);
						errloc0 = (nf_bch_err_loc01.bf.bchErrLoc0 & 0x3fff) >> 3;
						errloc1 = (nf_bch_err_loc01.bf.bchErrLoc1 & 0x3fff) >> 3;

						if ( (j+1)*2 <= nf_bch_sts.bf.bchErrNum ) {
							if( errloc1 < 0x400) {
								ecc_p[errloc1] ^= (1 << (nf_bch_err_loc01.bf.bchErrLoc1 & 0x07));
							}
						}

						if(errloc0 < 0x400) {
							ecc_p[errloc0] ^= (1 << (nf_bch_err_loc01.bf.bchErrLoc0 & 0x07));
						}
					}
					break;
				case BCH_UNCORRECTABLE:
					goto err_pageread;

			}
			nf_bch_ctrl.wrd = read_flash_ctrl_reg(FLASH_NF_BCH_CONTROL);
			nf_bch_ctrl.bf.bchCompare = 0;
			write_flash_ctrl_reg(FLASH_NF_BCH_CONTROL, nf_bch_ctrl.wrd);
		}
		ecc_p += (page_size/eccsteps);
	}

	if(NF_ECC_HM_BCH >= ECC_BCH_8)
		write_flash_ctrl_reg(FLASH_NF_BCH_CONTROL, 0);

	return TRUE;
err_pageread:

	//diasble bch
	if(NF_ECC_HM_BCH >= ECC_BCH_8)
	{
		write_flash_ctrl_reg(FLASH_NF_BCH_CONTROL, 0);
	}
	else
	{
		//diable ecc and make sure in correct ecc mode
		write_flash_ctrl_reg(FLASH_NF_ECC_CONTROL, 0);
	}
	flash_reset();
	printf("%s Out\n",__func__);
	return FALSE;

}

static int pnand_verify_bad(struct mtd_info *mtd, uint8_t* page_address)
{

	u32	i, page_size, oobsize;
	static unsigned char *oobp,tmp_buf[2];

	page_size = mtd->writesize;
	oobsize = mtd->oobsize;
	nf_access.wrd = 0;
	nf_access.bf.nflashExtAddr = 0;
	write_flash_ctrl_reg(FLASH_NF_ACCESS, nf_access.wrd);

	if(reg_wait(FLASH_STATUS, NF_STS_MSK, NF_STS_VAL, NF_STS_PCNT) == FALSE)
		flash_reset();

	//set oob size
	nf_count.wrd = 0;
	nf_count.bf.nflashRegDataCount = 3;
	nf_count.bf.nflashRegOobCount = oobsize- 1;
	write_flash_ctrl_reg(FLASH_NF_COUNT, nf_count.wrd);

	tmp_buf[0] = 0x0;
	tmp_buf[1] = 0x0;
	//check is bad block
	oobp = page_address;
	for(i=(page_size-4);i<(page_size+oobsize);i++) //read oob data to check bad block
	{
		if(i==(page_size+markp))
			tmp_buf[0] = oobp[i];
		else
			tmp_buf[1] = oobp[i];
	}

	if(tmp_buf[0]==0xff)
		return TRUE;		//good block

	return FALSE;	//bad block

}

int
flash_read_buf_hwecc_direct(struct mtd_info *mtd, uint8_t* addr, uint8_t* buff, int len)
{
#if defined(CONFIG_PNAND_TEST_DRIVER)
	struct nand_chip *chip = mtd->priv;
#else
	struct nand_chip *chip = mtd_to_nand(mtd);
#endif
	//uint32_t nand_addr;
	int rlen;
	uint8_t *read_oob=local_oob_buff;
	uint32_t  blockend, ecc_page;
	uint8_t *addr_align, skip_cpy;
	uint16_t offset;
	int oobsize = mtd->oobsize;
	int page_size = mtd->writesize;
	int eccsteps = chip->ecc.steps;
	int eccsize = chip->ecc.size;
	int eccbytes = chip->ecc.bytes;
	uint8_t *ecc_code = chip->buffers->ecccode;
	uint32_t *eccpos = chip->ecc.layout->eccpos;

	markp=0;
	rlen=0;

	if(NF_ECC_HM_BCH >= ECC_BCH_8)
	{
		eccsteps = (page_size/BCH_DATA_UNIT);

		if(NF_ECC_HM_BCH == ECC_BCH_8)
			eccsize = BCH_GF_PARAM_M;
		else if(NF_ECC_HM_BCH == ECC_BCH_16)
			eccsize = 2* BCH_GF_PARAM_M;
		else if(NF_ECC_HM_BCH == ECC_BCH_24)
			eccsize = 3* BCH_GF_PARAM_M;
		else if(NF_ECC_HM_BCH == ECC_BCH_40)
			eccsize = 5* BCH_GF_PARAM_M;

		eccbytes = eccsteps * eccsize;
	}
	else
	{
		if(NF_ECC_HM_BCH == ECC_HM_256B_3B) {
			ecc_page=256;
		} else {
			ecc_page=512;
		}
		eccsteps = (page_size/ecc_page);
		eccbytes = (page_size/ecc_page)*3;
		eccsize = 3;
	}

	if(oobsize>PAGE512_OOB_SIZE)
		markp = 0;//first byte
	else
		markp = 5; //6th byte

	/** last ecc byte put at 8 bytes - 1 address, ie oob lenght should be 8 bytes alignment because DMA in kernel **/
	eccpos = oobsize - (oobsize%8) - eccbytes;
	if(eccpos <= markp)
		eccpos--;
	/* oob buffer */
	read_oob = &local_oob_buff[0];
	if (!read_oob)
		goto err_read_buf;

	ecc_code = &local_ecc_code[0];
	if (!ecc_code)
		goto err_read_buf;

	addr_align =(uint8_t *)( (uint32_t) addr & ~((uint32_t)page_size-1));

	blockend = ((uint32_t)addr)+0x1000000; //SIZE_16M

	if (len > 0)
	{
	//search until 16MB
#if 0
NEXT_BLOCK:
		//check bad block, and give up if
		if(((uint32_t)(addr_align+rlen)) >= blockend) {
			printf("%s:too many bad block !\n", __func__);
			goto err_bad_blk;
		}
		if(pnand_verify_bad(mtd, addr))
		{
			//mark bad block
			addr_align += pnand_blk_size;
			addr += pnand_blk_size;
			goto NEXT_BLOCK;
		}
#endif
		//if first page address then skip to next page
		//if(addr==((uint8_t*)GOLDENGATE_FLASH_BASE))
		//	addr += page_size;

		check_flash_ctrl_status();

		if (len < page_size) {
				page_read_hwecc_direct(mtd, chip, addr_align, buff, read_oob, page_size);
			rlen = page_size;
			len = 0;
		}
		while (len >= page_size) {
			//printf(".");
				page_read_hwecc_direct(mtd, chip, addr_align, buff, read_oob, page_size);
			buff += page_size;
			addr_align += page_size;
			rlen += page_size;
			if (len > page_size) {
				len -= page_size;
			} else {
				printf("\n");
				len = 0;
				break;
			}
		}
		if (len > 0) {
			//printf(".\n");
				page_read_hwecc_direct(mtd, chip, addr_align, buff, read_oob, page_size);
			rlen += page_size;
			len = 0;
		}
	}
	flash_reset();
	return rlen;

err_read_buf:
	printf("%s:page_read fail !\n", __func__);
	flash_reset();
	return rlen;
#if 0
err_bad_blk:
	printf("addr:%lx, len:%x, rlen:%x", (unsigned long)addr_align,  (unsigned int)len,  (unsigned int)rlen);
	flash_reset();
	return rlen;
#endif
}

/**
 * pnand_read - [MTD Interface] MTD compability function for nand_do_read_ecc
 * @mtd:	MTD device structure
 * @from:	offset to read from
 * @len:	number of bytes to read
 * @retlen:	pointer to variable to store the number of read bytes
 * @buf:	the databuffer to put data
 *
 * Get hold of the chip and call nand_do_read
 */
void pnand_block_read(struct mtd_info *mtd, unsigned int offset, size_t len, void *buffer)
{
	unsigned int flash_addr, length_read;

		flash_addr = offset | FLASH_BASE;
	//printf("%s(%d): offset=%d, len=0x%x, buffer=0x%x\n", __func__, __LINE__,(unsigned int)offset,len,(UINT32)buffer);
	length_read  = flash_read_buf_hwecc_direct(mtd, (void *)flash_addr, (void *)buffer, len);
	printf("parallel nand read %d bytes \n", length_read);
	flash_reset();
}

 int pnand_dev_ready(struct mtd_info *mtd)
{
	int ready;

	check_flash_ctrl_status();

	write_flash_ctrl_reg(FLASH_NF_DATA,0xffffffff);
RD_STATUS:
	write_flash_ctrl_reg(FLASH_NF_ECC_CONTROL, 0x0); /** disable ecc gen**/
	nf_count.wrd = 0;
	nf_count.bf.nflashRegOobCount = NF_CNT_EMPTY_OOB;
	nf_count.bf.nflashRegDataCount = NF_CNT_DATA_1;
	nf_count.bf.nflashRegAddrCount = NF_CNT_EMPTY_ADDR;
	nf_count.bf.nflashRegCmdCount = NF_CNT_CMD_1;

	write_flash_ctrl_reg(FLASH_NF_COUNT, nf_count.wrd);

	nf_command.wrd = 0;
	nf_command.bf.nflashRegCmd0 = NAND_CMD_STATUS;
	write_flash_ctrl_reg(FLASH_NF_COMMAND, nf_command.wrd); /** write read id command**/
	nf_addr1.wrd = 0;
	write_flash_ctrl_reg(FLASH_NF_ADDRESS_1, nf_addr1.wrd); /** write address 0x00**/
	nf_addr2.wrd = 0;
	write_flash_ctrl_reg(FLASH_NF_ADDRESS_2, nf_addr2.wrd); /** write address 0x00**/

	nf_access.wrd = 0;
	nf_access.bf.nflashCeAlt = NF_ACCESS_CE_ALT;
	/** nf_access.bf.nflashDirWr = ;**/
	nf_access.bf.nflashRegWidth = NF_ACCESS_WiDTH8;

	write_flash_ctrl_reg(FLASH_NF_ACCESS, nf_access.wrd);
	access_start.wrd = 0;
	access_start.bf.nflashRegReq = FLASH_GO;
	access_start.bf.nflashRegCmd = FLASH_RD;
	write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, access_start.wrd);

	access_start.wrd=read_flash_ctrl_reg(FLASH_FLASH_ACCESS_START);
	tmp_access.wrd = 0;
	tmp_access.bf.nflashRegReq = 1;
	reg_wait(FLASH_FLASH_ACCESS_START, tmp_access.wrd, 0, 1000);

	check_flash_ctrl_status();

	ready = read_flash_ctrl_reg(FLASH_NF_DATA)&0xff;
	if(ready==0xff)
	{
		printf("pnand_dev_ready : %x\n",read_flash_ctrl_reg(FLASH_STATUS));
		goto RD_STATUS;
	}

	return (ready & NAND_STATUS_READY);
}

int board_pnand_init(struct nand_chip *this)
{
	FLASH_TYPE_t flash_type;
	GLOBAL_STRAP_CPU_WDATA_t global_strap_cpu_wdata;
	struct mtd_info *mtd;
	unsigned short tmp;
	int err = 0;

#if defined(CONFIG_PNAND_TEST_DRIVER)
	/* structures must be linked */
	mtd = &host->mtd;
	mtd->priv = this;
#else
	/* structures must be linked */
	mtd = &host->mtd;
	mtd->priv = mtd_to_nand;
#endif
	host->nand = this;

	/* 5 us command delay time */
	this->chip_delay = 20;

	this->priv = host;
	this->dev_ready = pnand_dev_ready;
	this->cmdfunc = pnand_command;
	this->select_chip = pnand_select_chip;
	this->read_byte = pnand_read_byte;
	/** this->read_word = pnand_read_word;**/
	this->write_buf = pnand_write_buf;
	this->read_buf = pnand_read_buf;
#ifdef CONFIG_MTD_NAND_VERIFY_WRITE
	this->verify_buf = pnand_verify_buf;
#endif

/** #ifdef CONFIG_PNAND_HWECC**/
	this->ecc.calculate = pnand_calculate_ecc;
	this->ecc.hwctl = pnand_enable_hwecc;
	this->ecc.correct = pnand_correct_data;
	this->ecc.mode = NAND_ECC_HW;

#if defined(CONFIG_PNAND_ECC_HW_BCH)
		this->ecc.size = BCH_DATA_UNIT;
#if (CONFIG_PNAND_ECC_HW_BCH_LEVEL > 24 )
			this->ecc.bytes = (BCH_GF_PARAM_M * 40) /8;
#elif (CONFIG_PNAND_ECC_HW_BCH_LEVEL > 16 )
			this->ecc.bytes = (BCH_GF_PARAM_M * 24) /8;
#elif  (CONFIG_PNAND_ECC_HW_BCH_LEVEL > 8 )
			this->ecc.bytes = (BCH_GF_PARAM_M * 16) /8;
#else
			this->ecc.bytes = (BCH_GF_PARAM_M * 8) /8;
#endif
#elif defined(CONFIG_CS752X_NAND_ECC_HW_HAMMING_512)
			this->ecc.size = 512;
			this->ecc.bytes = 3;
#else
			this->ecc.size = 256;
			this->ecc.bytes = 3;
#endif
/** #endif**/

	/* check, if a user supplied wait function given */
	this->waitfunc = pnand_wait;
	this->block_bad = pnand_block_bad;
	this->block_markbad = pnand_default_block_markbad;

	if (!this->scan_bbt)
		this->scan_bbt = nand_default_bbt;


	//this->controller = pnand_hwcontrol;

	/* set decode to general flash controller logic */
	global_strap_cpu_wdata.wrd = read_flash_ctrl_reg(GLOBAL_STRAP_CPU_WDATA);
	flash_type.wrd = 0;
	flash_type.bf.flashWidth = global_strap_cpu_wdata.bf.flash_width;
	flash_type.bf.flashSize = global_strap_cpu_wdata.bf.flash_size;
	flash_type.bf.flashPin = global_strap_cpu_wdata.bf.flash_pin;
	flash_type.bf.flashType = global_strap_cpu_wdata.bf.flash_type;
	write_flash_ctrl_reg(FLASH_TYPE, flash_type.wrd);

	this->cmd_ctrl = pnand_hwcontrol;
	/* Reset NAND */
	this->cmdfunc(mtd, NAND_CMD_RESET, -1, -1);

	return err;
}

/**
 * pnand_scan_tail - [NAND Interface] Scan for the NAND device
 * @mtd:	    MTD device structure
 * @maxchips:	    Number of chips to scan for
 *
 * This is the second phase of the normal nand_scan() function. It
 * fills out all the uninitialized function pointers with the defaults
 * and scans for a bad block table if appropriate.
 */
int pnand_scan_tail(struct mtd_info *mtd)
{
	int i, eccStartOffset;
#if defined(CONFIG_PNAND_TEST_DRIVER)
	struct nand_chip *chip = mtd->priv;
#else
	struct nand_chip *chip = mtd_to_nand(mtd);
#endif

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
#if defined(CONFIG_PNAND_ECC_HW_BCH)
			if( mtd->oobsize == 16) {
				chip->ecc.layout= &pnand_bch_oob_16;
		}  else
#elif  defined(CONFIG_CS752X_NAND_ECC_HW_HAMMING_512)||defined(CONFIG_CS752X_NAND_ECC_HW_HAMMING_256)
		if( mtd->oobsize == 8 ) {
			chip->ecc.layout= &pnand_oob_8;
		} else if( mtd->oobsize == 16 ) {
			chip->ecc.layout= &pnand_oob_16;
		} else
#endif
		{
				memset( &pnand_ecclayout, 0, sizeof( pnand_ecclayout));
				pnand_ecclayout.eccbytes= mtd->writesize/chip->ecc.size*chip->ecc.bytes;
				if( sizeof( pnand_ecclayout.eccpos) < 4*pnand_ecclayout.eccbytes ) {
					printf(KERN_WARNING "eccpos memory is less than needed eccbytes");
					return 1;
				}

				if ( pnand_ecclayout.eccbytes > mtd->oobsize) {
					printf("BCH%d need %d ECC but OOB size is %d!!\n", \
						(chip->ecc.bytes /BCH_GF_PARAM_M)*8, \
						pnand_ecclayout.eccbytes, mtd->oobsize);
					printf("You have to reduce ECC level!\n");
					return 1;
				}

				memset( pnand_ecclayout.eccpos, 0, sizeof( pnand_ecclayout.eccpos));
				eccStartOffset= mtd->oobsize - pnand_ecclayout.eccbytes;
				for( i = 0 ; i < pnand_ecclayout.eccbytes; ++i) {
					if( ( i+ eccStartOffset ) == chip->badblockpos) {
						continue;
					}
					pnand_ecclayout.eccpos[ i ]=  i + eccStartOffset;
				}

				pnand_ecclayout.oobfree[0].offset = 2;
				pnand_ecclayout.oobfree[0].length = mtd->oobsize - pnand_ecclayout.eccbytes - pnand_ecclayout.oobfree[0].offset;

#ifdef CONFIG_PNAND_ECC_HW_BCH
				/**  BCH algorithm needs one extra byte to tag erase status**/
				if ( pnand_ecclayout.oobfree[0].length == 0) {
					printf(KERN_WARNING "eccbytes is less than required");
					return 1;
				};
				pnand_ecclayout.oobfree[0].length -= 1;
#endif
				chip->ecc.layout= &pnand_ecclayout;
			}
		}

	if (!chip->write_page)
		chip->write_page = pnand_write_page;

	/*
	 * check ECC mode, default to software if 3byte/512byte hardware ECC is
	 * selected and we have 256 byte pagesize fallback to software ECC
	 */
	if (!chip->ecc.read_page_raw)
		chip->ecc.read_page_raw = pnand_read_page_raw;
	if (!chip->ecc.write_page_raw)
		chip->ecc.write_page_raw = pnand_write_page_raw;


		/* Use standard hwecc read page function ? */
		if (!chip->ecc.read_page)
			chip->ecc.read_page = pnand_read_page_hwecc;
		if (!chip->ecc.write_page)
			chip->ecc.write_page = pnand_write_page_hwecc;
		if (!chip->ecc.read_oob)
			chip->ecc.read_oob = pnand_read_oob_std;
		if (!chip->ecc.write_oob)
			chip->ecc.write_oob = pnand_write_oob_std;

	/*
	 * The number of bytes available for a client to place data into
	 * the out of band area
	 */
	chip->ecc.layout->oobavail = 0;
	for (i = 0; chip->ecc.layout->oobfree[i].length; i++)
	{
		chip->ecc.layout->oobavail +=
			chip->ecc.layout->oobfree[i].length;
	}
	mtd->oobavail = chip->ecc.layout->oobavail;

	/*
	 * Set the number of read / write steps for one page depending on ECC
	 * mode
	 */
	chip->ecc.steps = mtd->writesize / chip->ecc.size;
	if(chip->ecc.steps * chip->ecc.size != mtd->writesize) {
		printf(KERN_WARNING "Invalid ecc parameters\n");
		BUG();
	}
	chip->ecc.total = chip->ecc.steps * chip->ecc.bytes;

	/*
	 * Allow subpage writes up to ecc.steps. Not possible for MLC
	 * FLASH.
	 */
	if (!(chip->options & NAND_NO_SUBPAGE_WRITE) &&
	    nand_is_slc(chip))  {
		switch(chip->ecc.steps) {
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
	mtd->_erase = pnand_erase;
//	mtd->_point = NULL;
//	mtd->_unpoint = NULL;
	mtd->_read = pnand_read;
	mtd->_write = pnand_write;
	mtd->_read_oob = pnand_read_oob;
	mtd->_write_oob = pnand_write_oob;
	mtd->_sync = pnand_sync;
	mtd->_lock = NULL;
	mtd->_unlock = NULL;
//	mtd->_suspend = pnand_suspend;
//	mtd->_resume = pnand_resume;
	mtd->_block_isbad = pnand_block_isbad;
	mtd->_block_markbad = pnand_block_markbad;
	mtd->writebufsize = mtd->writesize;

	/* propagate ecc.layout to mtd_info */
	mtd->ecclayout = chip->ecc.layout;

	/* Check, if we should skip the bad block table scan */
	if (chip->options & NAND_SKIP_BBTSCAN)
		chip->options |= NAND_BBT_SCANNED;


//	chip->ecc.priv = nand_bch_init(mtd, chip->ecc.size, chip->ecc.bytes,
//					       &chip->ecc.layout);
//	chip->ecc.strength = chip->ecc.bytes * 8 / fls(8 * chip->ecc.size);
	return 0;
}


/**
 * pnand_scan_ident - [NAND Interface] Scan for the NAND device
 * @mtd:	     MTD device structure
 * @maxchips:	     Number of chips to scan for
 *
 * This is the first phase of the normal nand_scan() function. It
 * reads the flash ID and sets up MTD fields accordingly.
 *
 * The mtd->owner field must be set to the module of the caller.
 */
int pnand_scan_ident(struct mtd_info *mtd, int maxchips)
{
	int i, busw, nand_maf_id, nand_dev_id;
#if defined(CONFIG_PNAND_TEST_DRIVER)
	struct nand_chip *chip = mtd->priv;
#else
	struct nand_chip *chip = mtd_to_nand(mtd);
#endif
	struct nand_flash_dev *type;
	unsigned char id[8];

	/* Get buswidth to select the correct functions */
	busw = chip->options & NAND_BUSWIDTH_16;
	/* Set the default functions */
	/** nand_set_defaults(chip, busw);**/

	/* Read the flash type */
	type = pnand_get_flash_type(mtd, chip, busw, &nand_maf_id);

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
		pnand_read_id(id);
		/* Read manufacturer and device IDs */
		if (nand_maf_id != id[0] ||
		    nand_dev_id != id[1])
			break;
	}
#ifdef CS75XX_NAND_DEBUG
	if (i > 1)
		printf("%d NAND chips detected\n", i);
#endif

	/* Store the number of chips and calc total size for mtd */
	chip->numchips = i;
	mtd->size = i * chip->chipsize;
	//printf("mtd->size = 0x%x, i = 0x%x, chip->chipsize = 0x%x\n",mtd->size, i, chip->chipsize);
	return 0;
}

 int init_DMA_SSP( void )
{
	int i;
	unsigned char *tmp;
	DMA_SEC_DMA_GLB_DMA_LSO_CTRL_t dma_lso_ctrl;
	DMA_SEC_DMA_GLB_DMA_SSP_RX_CTRL_t dma_rxdma_ctrl;
	DMA_SEC_DMA_GLB_DMA_SSP_TX_CTRL_t dma_txdma_ctrl;
	DMA_SEC_DMA_SSP_Q_RXQ_BASE_DEPTH_t dma_rxq5_base_depth;
	DMA_SEC_DMA_SSP_Q_TXQ_BASE_DEPTH_t dma_txq5_base_depth;

	dma_lso_ctrl.wrd = read_dma_ctrl_reg(DMA_SEC_DMA_GLB_DMA_LSO_CTRL);
	dma_lso_ctrl.bf.tx_dma_enable = 1;
	write_dma_ctrl_reg(DMA_SEC_DMA_GLB_DMA_LSO_CTRL, dma_lso_ctrl.wrd);

	dma_rxdma_ctrl.wrd = read_dma_ctrl_reg(DMA_SEC_DMA_GLB_DMA_SSP_RX_CTRL);
	dma_txdma_ctrl.wrd = read_dma_ctrl_reg(DMA_SEC_DMA_GLB_DMA_SSP_TX_CTRL);

	if ((dma_rxdma_ctrl.bf.rx_check_own != 1)
	    && (dma_rxdma_ctrl.bf.rx_dma_enable != 1)) {
		dma_rxdma_ctrl.bf.rx_check_own = 1;
		dma_rxdma_ctrl.bf.rx_dma_enable = 1;
		write_dma_ctrl_reg(DMA_SEC_DMA_GLB_DMA_SSP_RX_CTRL,
				   dma_rxdma_ctrl.wrd);
	}
	if ((dma_txdma_ctrl.bf.tx_check_own != 1)
	    && (dma_txdma_ctrl.bf.tx_dma_enable != 1)) {
		dma_txdma_ctrl.bf.tx_check_own = 1;
		dma_txdma_ctrl.bf.tx_dma_enable = 1;
		write_dma_ctrl_reg(DMA_SEC_DMA_GLB_DMA_SSP_TX_CTRL,
				   dma_txdma_ctrl.wrd);
	}

	i = 16;
	tx_desc = (DMA_SSP_TX_DESC_T *) ((u32)malloc((sizeof(DMA_SSP_TX_DESC_T) * FDMA_DESC_NUM)+ i -1) & ~(i - 1));
	rx_desc = (DMA_SSP_RX_DESC_T *) ((u32)malloc((sizeof(DMA_SSP_RX_DESC_T) * FDMA_DESC_NUM)+ i -1) & ~(i - 1));
//(void *)(((u32)malloc(size + align) + align - 1) & ~(align - 1));
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

	/* printk("tx_desc_v: %p , rx_desc_v: %p \n", tx_desc, rx_desc); */
	/* set base address and depth */

	dma_rxq5_base_depth.bf.base = ((unsigned int)rx_desc) >> 4;
	dma_rxq5_base_depth.bf.depth = FDMA_DEPTH;
	write_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_RXQ_BASE_DEPTH,
			   dma_rxq5_base_depth.wrd);

	dma_txq5_base_depth.bf.base = ((unsigned int)tx_desc) >> 4;
	dma_txq5_base_depth.bf.depth = FDMA_DEPTH;
	write_dma_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_BASE_DEPTH,
			   dma_txq5_base_depth.wrd);

	memset((unsigned char *)tx_desc, 0,
	       (sizeof(DMA_SSP_TX_DESC_T) * FDMA_DESC_NUM));
	memset((unsigned char *)rx_desc, 0,
	       (sizeof(DMA_SSP_RX_DESC_T) * FDMA_DESC_NUM));

	for (i = 0; i < FDMA_DESC_NUM; i++) {
		/* set own by sw */
		tx_desc[i].word1.bf.own = OWN_SW;
		/* enable q5 Scatter-Gather memory copy */
		tx_desc[i].word1.bf.sgm = 0x1;
	}

	return 1;
}

int pnand_scan(struct mtd_info *mtd, int maxchips)
{
	int ret;

	/* Many callers got this wrong, so check for it for a while... */
	/* XXX U-BOOT XXX */
#if 0
	if (!mtd->owner && caller_is_module()) {
		printf(KERN_CRIT "nand_scan() called with NULL mtd->owner!\n");
		BUG();
	}
#endif

	ret = pnand_scan_ident(mtd, maxchips);
	if (!ret)
		ret = pnand_scan_tail(mtd);
	return ret;
}

int pnand_init_chip(struct mtd_info *mtd, struct nand_chip *nand,
			   ulong base_addr)
{
	int maxchips = CONFIG_SYS_NAND_MAX_CHIPS;
	int __attribute__((unused)) i = 0;
	int ret = 0;

	if( init_DMA_SSP() == 0 ) {
		printf("desc alloc error!! \n");
	}

	flash_reset();
	if (maxchips < 1)
		maxchips = 1;
#if defined(CONFIG_PNAND_TEST_DRIVER)
	mtd->priv = nand;
#else
	mtd->priv = mtd_to_nand(mtd);
#endif

	/** set to NAND data register**/
	nand->IO_ADDR_R = nand->IO_ADDR_W = (void  __iomem *)FLASH_NF_DATA;/** base_addr;**/
	if (board_pnand_init(nand) == 0) {
		if (pnand_scan(mtd, maxchips) == 0) {
			if (!mtd->name)
				mtd->name = (char *)default_nand_name;
#ifndef CONFIG_RELOC_FIXUP_WORKS
			else
				mtd->name += gd->reloc_off;
#endif

#ifdef CONFIG_MTD_DEVICE
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

void pnand_flash_init(void)
{
	struct mtd_info *mtd;
	int i, ret;
	unsigned int size = 0;

	for (i = 0; i < CONFIG_SYS_MAX_NAND_DEVICE; i++) {
		ret = pnand_init_chip(mtd, &nand_chip[i], base_address[i]);
		if(ret != 0){
			memset(&nand_info[i], 0, sizeof(struct mtd_info));
			return;
		}

		size += (mtd->size) / 1024;
		if (nand_curr_device == -1)
			nand_curr_device = i;
	}
	printf("Chip Size: %u MiB\n", size / 1024);

	nand_register(0);
#ifdef CONFIG_SYS_NAND_SELECT_DEVICE
	/*
	 * Select the chip in the board/cpu specific driver
	 */
	board_nand_select_device(nand_info[nand_curr_device].priv, nand_curr_device);
#endif
}

/* Flysky add for socCmd test item */
void rtk_para_nand_test(IN  void *Data)
{
	struct mtd_info *mtd;
#if defined(CONFIG_PNAND_TEST_DRIVER)
	struct nand_chip *chip = mtd->priv;
#else
	struct nand_chip *chip = mtd_to_nand(mtd);
#endif
	struct mtd_oob_ops *ops;
	struct erase_info instr;
	u32*  InPutData;
	InPutData = (u32*)Data;
	unsigned char id[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

	switch(InPutData[0])
	{
		case 0:
			//CAUTION :  init memory and parameter , make sure this command have been excuted before any others
			//socCmd PNAND 0
			pnand_flash_init();
			break;
		case 1:
			//TODO: READ ID (Read device ID) RDID
			//socCmd PNAND 1
			pnand_read_id(id);
			break;
		case 2:
			/**********************************************************************/
			/* RTL8198F Parallel_NAND flash controller does NOT support DIRECT MODE yet */
			/**********************************************************************/
			/* DIRECT MODE(AUTO MODE) */

			switch(InPutData[1])
			{
				case 0:
					//TODO: DIRECT MODE(AUTO MODE): READ
					switch(InPutData[2])
					{
						case 0:
							/* DIRECT MODE: READ */
							/* socCmd 2 0 0 offset len buf*/
							pnand_block_read(mtd, InPutData[3], InPutData[4], InPutData[5]);
							break;
					}
					break;
				case 1:
					//TODO: DIRECT MODE(AUTO MODE): WRITE
					switch(InPutData[2])
					{
						case 0:
							/* DIRECT MODE: WRITE */
							/* socCmd 2 1 0 offset len buf */
							pnand_block_write(mtd, InPutData[3], InPutData[4], InPutData[5]);
							break;
					}
					break;
			}
			break;
		case 3:
			/* INDIRECT MODE(USER MODE) */
			//pnand_dma_access = 0;
			switch(InPutData[1])
			{
				case 0:
					/* INDIRECT MODE(USER MODE): READ */
					switch(InPutData[2])
					{
						case 0:
							/* INDIRECT MODE: PAGE READ */
							/* socCmd 3 0 0 offset len buf*/
							pnand_read(mtd, InPutData[3], InPutData[4], ops->retlen, InPutData[5]);
							break;
					}
					break;
				case 1:
					/* INDIRECT MODE(USER MODE): WRITE */
					switch(InPutData[2])
					{
						case 0:
							/* INDIRECT MODE: PAGE WRITE */
							/* socCmd 3 1 0 offset len buf */
							pnand_write(mtd, InPutData[3], InPutData[4], ops->retlen, InPutData[5]);
							break;
					}
					break;
			}
			break;
		case 4:
			/* DMA MODE */
			//pnand_dma_access = 1;
			switch(InPutData[1])
			{
				case 0:
					/* DMA MODE: READ */
					switch(InPutData[2])
					{
						case 0:
							/* DMA: PAGE READ (Array read) PRD */
							/* socCmd 4 0 0 offset len buf */
							pnand_read(mtd, InPutData[3], InPutData[4], ops->retlen, InPutData[5]);
							break;
					}
					break;
				case 1:
					/* DMA MODE: WRITE */
					switch(InPutData[2])
					{
						case 0:
							/* DMA: PAGE WRITE (Array write) PWT */
							/* socCmd 4 1 0 offset len buf */
							pnand_write(mtd, InPutData[3], InPutData[4], ops->retlen, InPutData[5]);
							break;
					}
					break;
			}
			break;
		case 5:
			/* RESET FLASH */
			// socCmd 5
			flash_reset();
			break;
		case 6:
			/* ERASE CHIP */
			/* pnand_erase_chip(); */
			printf("ERASE CHIP not available\n");
			break;
		case 7:
			/* ERASE */
			/* socCmd 7 offset len */
#if defined(CONFIG_PNAND_TEST_DRIVER)
			instr.mtd = mtd->priv;
#else
			instr.mtd = mtd_to_nand(mtd);
#endif
			instr.addr = InPutData[1];
			instr.len = InPutData[2];

			pnand_erase(mtd, &instr);
			break;
		case 8:
			/* SECTOR ERASE */
			printf("SECTOR ERASE not available\n");
			break;
		case 9:
			/* BLOCK ERASE */
			//socCmd 9 blk_addr
			pnand_erase_block(mtd, (uint32_t)(InPutData[1] * pnand_blk_size));
			flash_reset();
			break;
		case 10:
			/* GET FEATURES */
			/* pnand_get_feature_reg(addr) */
			//pnand_get_feature_reg(InPutData[1]);
			break;
		case 11:
			/* SET FEATURES */
			/* pnand_set_feature_reg(addr, value) */
			//pnand_set_feature_reg(InPutData[1], InPutData[2]);
			break;
		case 12:
			/* WRITE ENABLE */
			//pnand_write_enable();
			break;
		case 13:
			/* WRITE DISABLE */
			//pnand_write_disable();
			break;
		case 14:
			//TODO: Power Down: Enter Deep Power Down
			printf("Enter Deep Power Down not available\n");
			break;
		case 15:
			//TODO: Power Down: Release from Deep Power Down
			printf("Release from Deep Power Down not available\n");
			break;
		case 16:
			/* ENTER 4BYTE MODE */
			/* pnand_enter_4byte_mode(); */
			printf("ENTER 4BYTE MODE not available\n");
			break;
		case 17:
			/* EXIT 4BYTE MODE */
			/* pnand_exit_4byte_mode(); */
			printf("EXIT 4BYTE MODE not available\n");
			break;
		case 18:
			/* COMPARE MEM */
			/* socCmd 18 addr1 addr2 len */
			pnand_mem_cmp(InPutData[1], InPutData[2], InPutData[3]);
			break;
	}
}