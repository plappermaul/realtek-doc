/******************************************************************************
 * include/linux/mtd/rtk_nand.h
 * Overview: Realtek Nand Flash Specific Function and Data
 * Copyright (c) 2008 Realtek Semiconductor Corp. All Rights Reserved.
 * Modification History:
 *    #000 2008-06-11 Ken-Yu   create file
 *
 *******************************************************************************/
#ifndef __RTK_NAND_H
#define __RTK_NAND_H



/* 
define mars read/write NAND HW registers
use them because standard readb/writeb have warning msgs in our gcc 2.96
ex: passing arg 2 of `writeb' makes pointer from integer without a cast
*/
#define rtk_readb(offset)         (*(volatile unsigned char *)(offset))
#define rtk_readw(offset)         (*(volatile unsigned short *)(offset))
#define rtk_readl(offset)         (*(volatile unsigned long *)(offset))

#define rtk_writeb(val, offset)    (*(volatile unsigned char *)(offset) = val)
#define rtk_writew(val, offset)    (*(volatile unsigned short *)(offset) = val)
#define rtk_writel(val, offset)    (*(volatile unsigned long *)(offset) = val)


/* NAND Flash Command Sets */
#define CMD_READ_ID				0x90
#define CMD_READ_STATUS		0x70

#define CMD_PG_READ_C1		0x00
#define CMD_PG_READ_C2		0x30
#define CMD_PG_READ_C3		CMD_READ_STATUS

#define CMD_PG_READ_512_R2		0x01
#define CMD_PG_READ_512_R3		0x50


#define CMD_PG_WRITE_C1		0x80
#define CMD_PG_WRITE_C2		0x10
#define CMD_PG_WRITE_C3		CMD_READ_STATUS

#define CMD_BLK_ERASE_C1		0x60	//Auto Block Erase Setup command
#define CMD_BLK_ERASE_C2		0xd0	//CMD_ERASE_START
#define CMD_BLK_ERASE_C3		CMD_READ_STATUS	//CMD_STATUS_READ

#define CMD_RESET                 0xff
#define CMD_RANDOM_DATA_INPUT     0x85    /* RANDOM DATA write */ 

#define CMD_RANDOM_DATA_OUTPUT_C1 0x05    /* RANDOM DATA read */
#define CMD_RANDOM_DATA_OUTPUT_C2 0xe0 

/* NAND_ISR status, 0x1801_0324 */
#define NAND_ISR_XFER_MODE_DONE	(1 << 2)
#define NAND_ISR_AUTO_MODE_DONE	(1 << 3)
#define NAND_ISR_DMA_DONE	(1 << 4)
#define NAND_ISR_PFS_DONE	(1 << 7)	//polling flash status busy = 1

/* CMYu, 20090720 */
#define MCP_NONE	0
#define MCP_AES_ECB	1
#define MCP_AES_CBC	2
#define MCP_AES_CTR	3

/*
 * Standard NAND flash commands
 */
#define NAND_CMD_READ0		0
#define NAND_CMD_READ1		1
#define NAND_CMD_PAGEPROG	0x10
#define NAND_CMD_READOOB	0x50
#define NAND_CMD_ERASE1		0x60
#define NAND_CMD_STATUS		0x70
#define NAND_CMD_STATUS_MULTI	0x71
#define NAND_CMD_SEQIN		0x80
#define NAND_CMD_READID		0x90
#define NAND_CMD_ERASE2		0xd0
#define NAND_CMD_RESET		0xff

/* Extended commands for large page devices */
#define NAND_CMD_READSTART	0x30
#define NAND_CMD_CACHEDPROG	0x15




#if 0 //enable debug
#define dbg_printf(fmt,arg...) printk(fmt, ##arg)
#else
#define dbg_printf(fmt,arg...)

#endif


#endif  /* __RTK_NAND_H */


