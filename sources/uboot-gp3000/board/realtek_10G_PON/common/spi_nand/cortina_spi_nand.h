/**
* spi-nand.h
*
* Copyright (c) 2009-2015 Micron Technology, Inc.
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*/
#ifndef _SPI_NAND_H
#define _SPI_NAND_H

#include <linux/types.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/flashchip.h>


#ifdef DEBUG
#define spinand_debug(fmt,args...)	printf("SPI-NAND DEBUG: "fmt,##args)
#else
#define spinand_debug(fmt,args...)
#endif
#define spinand_err(fmt,args...)  	printf("SPI-NAND ERROR: " fmt,##args)
#define spinand_info(fmt,args...)	printf("SPI-NAND INFO : " fmt,##args)
#define spinand_warn(fmt,args...)	printf("SPI-NAND WARN : " fmt,##args)
#define spinand_dbgloc()            printf("DD: %s %s %d\n",__FILE__, __FUNCTION__,__LINE__);

struct spi_nand_cmd ;
/*
 * Standard SPI-NAND flash commands
 */
#define SPI_NAND_CMD_RESET			0xff
#define SPI_NAND_CMD_GET_FEATURE			0x0f
#define SPI_NAND_CMD_SET_FEATURE			0x1f
#define SPI_NAND_CMD_PAGE_READ			0x13
#define SPI_NAND_CMD_READ_PAGE_CACHE_RDM		0x30
#define SPI_NAND_CMD_READ_PAGE_CACHE_LAST	0x3f
#define SPI_NAND_CMD_READ_FROM_CACHE		0x03
#define SPI_NAND_CMD_READ_FROM_CACHE_FAST	0x0b
#define SPI_NAND_CMD_READ_FROM_CACHE_X2		0x3b
#define SPI_NAND_CMD_READ_FROM_CACHE_DUAL_IO	0xbb
#define SPI_NAND_CMD_READ_FROM_CACHE_X4		0x6b
#define SPI_NAND_CMD_READ_FROM_CACHE_QUAD_IO	0xeb
#define SPI_NAND_CMD_BLK_ERASE			0xd8
#define SPI_NAND_CMD_PROG_EXC			0x10
#define SPI_NAND_CMD_PROG_LOAD			0x02
#define SPI_NAND_CMD_PROG_LOAD_RDM_DATA		0x84
#define SPI_NAND_CMD_PROG_LOAD_X4		0x32
#define SPI_NAND_CMD_PROG_LOAD_RDM_DATA_X4	0x34
#define SPI_NAND_CMD_READ_ID			0x9f
#define SPI_NAND_CMD_WR_DISABLE			0x04
#define SPI_NAND_CMD_WR_ENABLE			0x06
#define SPI_NAND_CMD_DIE_SEL_ESMT_WINBOND 0xC2
#define SPI_NAND_CMD_MXIC_ECC_STS_READ	 0x7C

#define SPI_NAND_CMD_END				0x0


/* feature registers */
#define REG_BLOCK_LOCK	0xa0
#define REG_CFG			0xb0
#define REG_STATUS		0xc0

/* B0 Address description */
#define PARAM_READ_EN		0x40
#define ECC_EN				0x10

/* status */
#define STATUS_OIP_MASK			0x01
#define STATUS_READY			(0 << 0)
#define STATUS_BUSY				(1 << 0)
#define STATUS_CRBSY_MASK		0x80

#define STATUS_E_FAIL_MASK		0x04
#define STATUS_E_FAIL			(1 << 2)

#define STATUS_P_FAIL_MASK		0x08
#define STATUS_P_FAIL			(1 << 3)

#define STATUS_ECC_1BIT_CORRECTED	(1 << 4)
#define STATUS_ECC_ERROR		(2 << 4)
#define STATUS_ECC_RESERVED		(3 << 4)

/*Configuration register defines*/
#define CFG_QE_MASK			0x01
#define CFG_QE_ENABLE		0x01
#define CFG_BUF_MASK		0x08
#define CFG_BUF_ENABLE		0x08
#define CFG_ECC_MASK		0X10
#define CFG_ECC_ENABLE		0x10
#define CFG_LOT_MASK		0x20
#define CFG_LOT_ENABLE		0x20
#define CFG_OTP_MASK		0xc0
#define CFG_OTP_ENTER		0x40
#define CFG_OTP_EXIT		0x00

/* block lock */
#define BL_ALL_LOCKED		0x7c
#define BL_U_1_1024_LOCKED		0x08
#define BL_U_1_512_LOCKED		0x10
#define BL_U_1_256_LOCKED		0x18
#define BL_U_1_128_LOCKED		0x20
#define BL_U_1_64_LOCKED		0x28
#define BL_U_1_32_LOCKED		0x30
#define BL_U_1_16_LOCKED		0x38
#define BL_U_1_8_LOCKED		0x40
#define BL_U_1_4_LOCKED		0x48
#define BL_U_1_2_LOCKED		0x50
#define BL_L_1_1024_LOCKED		0x0c
#define BL_L_1_512_LOCKED		0x14
#define BL_L_1_256_LOCKED		0x1c
#define BL_L_1_128_LOCKED		0x24
#define BL_L_1_64_LOCKED		0x2c
#define BL_L_1_32_LOCKED		0x34
#define BL_L_1_16_LOCKED		0x3c
#define BL_L_1_8_LOCKED		0x44
#define BL_L_1_4_LOCKED		0x4c
#define BL_L_1_2_LOCKED		0x54
#define BL_ALL_UNLOCKED		0X00


/* On-Die ECC bit mask definition */
#define SPI_NAND_MT29F_ECC_MASK		0x70
#define SPI_NAND_MT29F_ECC_0_BIT	0x00
#define SPI_NAND_MT29F_ECC_1_3_BIT	0x10
#define SPI_NAND_MT29F_ECC_4_6_BIT	0x30
#define SPI_NAND_MT29F_ECC_7_8_BIT	0x50
#define SPI_NAND_MT29F_ECC_UNCORR	0x20

#define SPI_NAND_STATUS_ECC_MASK    0x30
#define SPI_NAND_STATUS_ECC_SHIFT	 4
#define SPI_NAND_STATUS_ECC_NO_ERR  0x00


struct micron_onfi_specific {
	u8		two_plane_page_read;		/*166*/
	u8		reserved0[8];			/*167-174*/
	u8		otp_mode;				/*175*/
	u8		otp_page_start;		/*176*/
	u8		otp_data_protect_addr;	/*177*/
	u8		otp_page_number;			/*178*/
	u8		otp_feature_addr;			/*179*/
	u8		reserved1[68];			/*180-247*/
	u8		ecc_ability;			/*248*/
	u8		die_selection;			/*249*/
	u8		reserved2[3];			/*250-252*/
	u8		parameter_page_version;	/*253*/
}__attribute__((packed));

union spi_nand_vendor_specific {
	u8		vendor_specific[88];
	struct micron_onfi_specific micron_sepcific;
};

struct spi_nand_onfi_params {
	/* rev info and features block */
	/* 'O' 'N' 'F' 'I'  */
	u8		sig[4];				/*0-3*/
	__le16		revision;			/*4-5*/
	__le16		features;			/*6-7*/
	__le16		opt_cmd;			/*8-9*/
	u8		reserved0[22];			/*10-31*/

	/* manufacturer information block */
	char		manufacturer[12];		/*32-43*/
	char		model[20];			/*44-63*/
	u8		mfr_id;				/*64*/
	__le16		date_code;			/*65-66*/
	u8		reserved1[13];			/*67-79*/

	/* memory organization block */
	__le32		byte_per_page;			/*80-83*/
	__le16		spare_bytes_per_page;		/*84*85*/
	__le32		data_bytes_per_ppage;		/*86-89*/
	__le16		spare_bytes_per_ppage;		/*90-91*/
	__le32		pages_per_block;		/*92-95*/
	__le32		blocks_per_lun;			/*96-99*/
	u8		lun_count;			/*100*/
	u8		addr_cycles;			/*101*/
	u8		bits_per_cell;			/*102*/
	__le16		bb_per_lun;			/*103-104*/
	__le16		block_endurance;		/*105-106*/
	u8		guaranteed_good_blocks;		/*107*/
	__le16		guaranteed_block_endurance;	/*108-109*/
	u8		programs_per_page;		/*110*/
	u8		ppage_attr;			/*111*/
	u8		ecc_bits;			/*112*/
	u8		interleaved_bits;		/*113*/
	u8		interleaved_ops;		/*114*/
	u8		reserved2[13];			/*115-127*/

	/* electrical parameter block */
	u8		io_pin_capacitance_max;		/*128*/
	__le16		timing_mode;			/*129-130*/
	__le16		program_cache_timing_mode;	/*131-132*/
	__le16		t_prog;				/*133-134*/
	__le16		t_bers;				/*135-136*/
	__le16		t_r;				/*137-138*/
	__le16		t_ccs;				/*139-140*/
	u8		reserved3[23];			/*141-163*/

	/* vendor */
	__le16		vendor_specific_revision;	/*164-165*/
	union spi_nand_vendor_specific vendor;		/*166-253*/

	__le16		crc;				/*254-255*/
} __attribute__((packed));

#define ONFI_CRC_BASE	0x4F4E

/**
 * struct spi_nand_info - SPI-NAND Private Flash Chip Data
 * @name:		name of the chip
 * @spi:		[INTERN] point to spi device structure
 * @mfr_id:		[BOARDSPECIFIC] manufacture id
 * @dev_id:		[BOARDSPECIFIC] device id
 * @read_cache_op:	[REPLACEABLE] Opcode of read from cache
 * @write_cache_op:	[REPLACEABLE] Opcode of program load
 * @write_cache_rdm_op:	[REPLACEABLE] Opcode of program load random
 * @oobbuf:		[INTERN] buffer for read/write oob
 * @size:		[INTERN] the size of chip
 * @block_size:		[INTERN] the size of eraseblock
 * @page_size:		[INTERN] the size of page
 * @oob_size:	[INTERN] the size of page oob size
 * @block_shift:	[INTERN] number of address bits in a eraseblock
 * @page_shift:		[INTERN] number of address bits in a page (column
 *			address bits).
 * @pagemask:		[INTERN] page number mask = number of (pages / chip) - 1
 * @options:		[BOARDSPECIFIC] various chip options. They can partly
 *			be set to inform nand_scan about special functionality.
 * @ecc_strength:	[INTERN] ECC correctability
 * @refresh_threshold:	[INTERN] Bitflip threshold to return -EUCLEAN
 * @ecclayout:		[BOARDSPECIFIC] ECC layout control structure
 *			See the defines for further explanation.
 * @onfi_params:	[INTERN] holds the ONFI page parameter
 */
typedef struct die_select_cmd_s {
    u8      cmd;
    u8      addr;
    u8      n_addr;
    u8      n_data;
    u8      die[2];
}die_select_cmd_t;

typedef struct spi_nand_info spi_nand_info_t;


struct spi_nand_info {
	struct mtd_info	*mtd;
	char	*name;
	u8		mfr_id;
	u16		dev_id;

	u8		sclk;
	u8		read_cache_op;
    u8      dummy_cycle;
	u8		write_cache_op;
	u8		write_cache_rdm_op;
	die_select_cmd_t die_sel_op;

	int (*enable_ecc)(struct spi_nand_info *this);
	int (*disable_ecc)(struct spi_nand_info *this);
	void (*get_ecc_status)(struct spi_nand_info *this, unsigned int status, unsigned int *corrected, unsigned int *ecc_errors);

	u8		*oobbuf;
	u64		size;
	u32		block_size;
	u16		page_size;
	u16		oob_size;
	u8		lun_shift;
	u8		block_shift;
	u8		page_shift;
	u16		page_mask;
	u32		lun_mask;
	u32		options;
	u32		ecc_strength;
	u8		refresh_threshold;
	u8		lun;
	u8      numchips;
	struct nand_ecclayout *ecclayout;
	struct spi_nand_onfi_params	 onfi_params;
	int (*spinand_command_fn)(struct spi_nand_cmd *cmd);
};

struct spi_nand_flash {
	char	*name;
	u8		mfr_id;
	u16		dev_id;
	u16		page_size;
	u16		oob_size;
	u16		pages_per_blk;
	u16		blks_per_lun;
	u16		luns_per_chip;
	u16		ecc_strength;
	u16		options;
};

#define SPI_NAND_MAX_ADDR_LEN		4
#define MIN_TRY_COUNT		3

struct spi_nand_cmd {
	u8		cmd;
	u8		n_addr;		/* Number of address */
	u8		addr[SPI_NAND_MAX_ADDR_LEN];	/* Reg Offset */
	u8		n_dummy_cycle;		/* Number of dummy cycles */
	u32		n_tx;		/* Number of tx bytes */
	const u8	*tx_buf;	/* Tx buf */
	u32		n_rx;		/* Number of rx bytes */
	u8		*rx_buf;	/* Rx buf */
	u8		io_addr;
	u8		io_data;
};

enum {
	NORMAL_MODE,
	OTP_MODE,
};

#define SPI_NAND_INFO(nm, mid, did, pagesz, oobsz, pg_per_blk,\
	blk_per_lun, lun_per_chip, ecc_stren, opts)		\
	{ .name = (nm), .mfr_id = (mid), .dev_id = (did),\
	.page_size = (pagesz), .oob_size = (oobsz),\
	.pages_per_blk = (pg_per_blk), .blks_per_lun = (blk_per_lun),\
	.luns_per_chip = (lun_per_chip),			\
	.ecc_strength = (ecc_stren), .options = (opts) }

#define OPCODE_OW(mid, did, max_clock, rd_op, rd_dummy, wr_op, wr_rdm_op)		\
	{ .mfr_id = (mid), .dev_id = (did),\
	  .sclk = max_clock,\
	  .read_cache_op = (rd_op), .dummy_cycle = (rd_dummy),\
      .write_cache_op = (wr_op), .write_cache_rdm_op = (wr_rdm_op)}

/* Size information in SPI_NAND_DATA_BASE_T */
#define PAGE_2KB		 0x800		/*2048*/
#define PAGE_4KB		 0x1000		/*4096*/
#define OOB_64B          64
#define OOB_128B         128
#define OOB_256B         256
#define PAGES_PER_BLK	 64

/*SPI NAND chip options*/
#define SPI_NAND_NEED_PLANE_SELECT (1 << 0)
#define SPI_NAND_NEED_DIE_SELECT   (1 << 1)
#define SPI_NAND_NEED_QE_ENABLE    (1 << 2)
#define SPI_NAND_MFR_IS_ESMT       (7 << 3)
#define SPI_NAND_MFR_IS_GD         (7 << 4)
#define SPI_NAND_MFR_IS_ZENTEL     (7 << 5)


/*SPI NAND Manufacturer*/
#define SPI_NAND_MFR_ATO            0x9B
#define SPI_NAND_MFR_DOSILICON      0xE5
#define SPI_NAND_MFR_ESMT_GD_ZENTEL 0xC8
#define SPI_NAND_MFR_ETRON          0xD5
#define SPI_NAND_MFR_FMSH           0xA1
#define SPI_NAND_MFR_HEYANGTEK      0xC9
#define SPI_NAND_MFR_LONGSYS        0xCD
#define SPI_NAND_MFR_MACRONIX       0xC2
#define SPI_NAND_MFR_MICRON         0x2C
#define SPI_NAND_MFR_TOSHIBA        0x98
#define SPI_NAND_MFR_UNITED_MEMORY  0x52
#define SPI_NAND_MFR_WINBOND        0xEF
#define SPI_NAND_MFR_XIN_CUN        0x8C
#define SPI_NAND_MFR_XTX            0x0B
#define SPI_NAND_MFR_UNKNOWN        0x00

#define SPI_NAND_MAX_ID_LEN		  3

extern uint8_t acc_phy_offset;
static inline void nand_acc_phy_offs_addr(u16 enable)
{
#ifdef CONFIG_NAND_HIDE_BAD
	acc_phy_offset=enable;
#endif
}

#ifdef ilog2
#undef ilog2
#endif
static inline u8 ilog2(unsigned int v)
{
	u8 l = 0;
	while ((1UL << l) < v){
        l++;
    }
	return l;
}


#ifdef CONFIG_NAND_HIDE_BAD
u32 spi_nand_get_phy_offset(struct mtd_info *mtd, u32 viraddr);
void spi_nand_print_bad_block_hidden_list(void);
void update_factory_bad_block_list(struct spi_nand_info *info);
#endif


/* Function prototype in cortina_spi_nand_base.c*/
void spi_nand_show_info(struct spi_nand_info *info);
int spi_nand_block_isbad(struct spi_nand_info *info, loff_t offs);
int spi_nand_erase(struct spi_nand_info *info, uint64_t addr, uint64_t len);
int spi_nand_block_markbad(struct spi_nand_info *info, loff_t offs);
int spi_nand_block_markunbad(struct spi_nand_info *info, loff_t offs);
int spi_nand_read_oob(struct spi_nand_info *info, loff_t from, struct mtd_oob_ops *ops);
int spi_nand_write_oob(struct spi_nand_info *info, loff_t to, struct mtd_oob_ops *ops);
int spi_nand_scan_ident(struct mtd_info *mtd, int maxchips);


/* Function prototype in spi-nand-bbt.c*/
int spi_nand_default_bbt(struct mtd_info *mtd);


/* Function Extern functions for cross file use */
void spi_nand_set_clock(struct spi_nand_info *info);
uint32_t spi_nand_get_clock(void);
void ca_spinand_ctrlr_init(void);
int ca_spinand_cmd_fn(struct spi_nand_cmd * cmd);
#define IGNORED_FOR_NOW 0
#define USED_IN_RAW_NAND 0


#endif
