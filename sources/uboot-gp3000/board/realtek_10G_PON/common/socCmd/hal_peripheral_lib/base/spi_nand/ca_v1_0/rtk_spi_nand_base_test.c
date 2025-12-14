/*
 * Overview: Realtek Serial NAND Flash Driver
 * Copyright (c) 2016 Realtek Semiconductor Corp. All Rights Reserved.
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
#include <common.h>
#include <malloc.h>
#include <asm/io.h>
#include <g3lite_registers.h>
#include <peripheral.h>

#include "spi_flash.h"
#include "spi_flash_internal.h"

#define CFI_MFR_MICRON			0x2C
#define CFI_MFR_GIGADEVICE		0xC8
#define CFI_MFR_MACRONIX		0xC2
#define CFI_MFR_MXIC			0xC2

#define SF_AC_OPCODE                	0x0
#define SF_AC_OPCODE_1_DATA           	0x1
#define SF_AC_OPCODE_2_DATA           	0x2
#define SF_AC_OPCODE_3_DATA           	0x3
#define SF_AC_OPCODE_4_DATA           	0x4
#define SF_AC_OPCODE_3_ADDR         	0x5
#define SF_AC_OPCODE_3_ADDR_1_DATA    	0x6
#define SF_AC_OPCODE_3_ADDR_2_DATA    	0x7
#define SF_AC_OPCODE_3_ADDR_3_DATA    	0x8
#define SF_AC_OPCODE_3_ADDR_4_DATA    	0x9
#define SF_AC_OPCODE_3_ADDR_X_1_DATA   	0xA
#define SF_AC_OPCODE_3_ADDR_X_2_DATA   	0xB
#define SF_AC_OPCODE_3_ADDR_X_3_DATA   	0xC
#define SF_AC_OPCODE_3_ADDR_X_4_DATA   	0xD
#define SF_AC_OPCODE_3_ADDR_4X_1_DATA  	0xE
#define SF_AC_OPCODE_EXTEND		0xF

#define SF_ACCESS_MIO_SINGLE		0
#define SF_ACCESS_MIO_DUAL		1
#define SF_ACCESS_MIO_QUARD		2

/* Chip select */
#ifdef CONFIG_RTL8198F_FPGA
#define SF_ACCESS_CE_ALT		1
#else /* ASIC board */
#define SF_ACCESS_CE_ALT		0
#endif /* CONFIG_CORTINA_BOARD_FPGA */

/* flashAddrCount*/
#define SF_ADDR_COUNT			1	/* 2 Byte address */
//#define SF_ADDR_COUNT			2	/* 3 Byte address */
//#define SF_ADDR_COUNT			3	/* 4 Byte address */

#define SF_START_INDIRECT_ENABLE	0x0002
#define SF_START_DMA_ENABLE		0x0008
//#define SF_START_BIT_WRITE		0x200
#define SFLASH_FORCEBURST		0x2000
#define SFLASH_FORCETERM		0x1000

/* Command for SPI NAND */
#define CMD_PAGE_READ			0x13
#define CMD_READ_SISO			0x03
#define CMD_READ_SIDO			0x3B
#define CMD_READ_SIQO			0x6B
#define CMD_READ_DUAL_IO		0xBB
#define CMD_READ_QUAD_IO		0xEB
#define CMD_PROG_PAGE_X1		0x02
#define CMD_PROG_PAGE_X4		0x32
#define CMD_PROG_RANDOM_X1		0x84
#define CMD_PROG_RANDOM_X4		0x34
#define CMD_PROG_PAGE_EXC		0x10
#define CMD_ERASE_BLK_32KB		0x52	/* NA */
#define CMD_ERASE_BLK_64KB		0xD8
#define CMD_WR_ENABLE			0x06
#define CMD_WR_DISABLE			0x04
#define CMD_READ_ID			0x9F
#define CMD_RESET			0xFF
#define CMD_RESET_MEMORY		0x99	/* NA */
#define CMD_GET_FEATURE_REG		0x0F
#define CMD_SET_FEATURE_REG		0x1F

/* feature / status reg */
#define REG_BLOCK_LOCK			0xa0
#define REG_OTP				0xb0
#define REG_STATUS			0xc0	/* timing */

/* status */
#define STATUS_OIP_MASK			0x01
#define STATUS_READY			(0 << 0)  /* Not in write operation */
#define STATUS_BUSY			(1 << 0)  /* In write operation */

#define STATUS_E_FAIL_MASK		0x04
#define STATUS_E_FAIL			(1 << 2)

#define STATUS_P_FAIL_MASK		0x08
#define STATUS_P_FAIL			(1 << 3)

#define STATUS_ECC_MASK			0x30
#define STATUS_ECC_1BIT_CORRECTED	(1 << 4)
#define STATUS_ECC_ERROR		(2 << 4)
#define STATUS_ECC_RESERVED		(3 << 4)

/*ECC enable defines*/
#define OTP_ECC_MASK		0x10
#define OTP_ECC_OFF			0
#define OTP_ECC_ON			1

#define ECC_DISABLED
#define ECC_IN_NAND
#define ECC_SOFT

/* QE */
#define OTP_QE_MASK			0x40 /* Bit6 QE (Quad Enable) */
#define OTP_QE_EN			1
#define OTP_QE_OFF			0

/* BURST MODE */
#define BURST_MODE_OFF			0

/* block lock */
#define BL_ALL_LOCKED			0x38
#define BL_1_2_LOCKED			0x30
#define BL_1_4_LOCKED			0x28
#define BL_1_8_LOCKED			0x20
#define BL_1_16_LOCKED			0x18
#define BL_1_32_LOCKED			0x10
#define BL_1_64_LOCKED			0x08
#define BL_ALL_UNLOCKED			0

#define PLANE_SELECT			0x40
#define PLANE_SELECT_BIT		0x1000

#define SFLASH_STATUS_WEL             	0x02
#define SFLASH_STATUS_WIP             	0x01
#define SFLASH_STATUS_BP		0x1C
#define SFLASH_STATUS_SRWD            	0x80

#define SFLASH_WEL_TIMEOUT		8000
#define SFLASH_PROGRAM_TIMEOUT		8000
#define SFLASH_ERASE_TIMEOUT		400000
#define SFLASH_CMD_TIMEOUT		40

/* DMA */
#define FLASH_RXDESC_ADDR_BASE		0x00002000
#define FLASH_TXDESC_ADDR_BASE		0x00003000

typedef volatile union {
  struct {
    ca_uint32   buffer_addr        :  8 ; /* bits 7:0 */
    ca_uint32   buffer_len         : 16 ; /* bits 23:8 */
    ca_uint32   rervd              :  7 ; /* bits 30:24 */
    ca_uint32   own                :  1 ; /* bits 31:31 */
  } bf ;
  ca_uint32       wrd ;
} FLASH_RXDESC_ADDR_BASE_t;

typedef volatile union {
  struct {
    ca_uint32   buffer_addr        :  8 ; /* bits 7:0 */
    ca_uint32   buffer_len         : 16 ; /* bits 23:8 */
    ca_uint32   sgm                :  1 ; /* bits 24:24 */
    ca_uint32   rervd              :  6 ; /* bits 30:25 */
    ca_uint32   own                :  1 ; /* bits 31:31 */
  } bf ;
  ca_uint32       wrd ;
} FLASH_TXDESC_ADDR_BASE_t;

/* FIFO */
#define FIFO_ADDR			0xE0000000
#define FIFO_ADDR_MAX			0xEFFFFFFF

static u32 spinand_type;
struct spi_flash *snandflash;
static u32 sflash_size = 0x40000000;		// 1G bit
static u32 spinand_page_size = 0x800;		// 2k page size
static u32 spinand_page_sz_mask = 0x7ff;
static u32 spinand_oob_size = 64;
static u32 spinand_blk_size = 64 * 2048;
static u32 spinand_dev_size = 128 * 0x200000;
static u8 oob_buff[64];

#define READ_FROM_CACHE(x,y,z)		spinand_read_cache_x1(x,y,z)
#define WRITE_TO_CACHE(x,y,z)		spinand_write_cache_x1(x,y,z)
#define DMA_READ_FROM_CACHE(x,y,z)	spinand_dma_read_x1(x,y,z)
#define DMA_WRITE_TO_CACHE(x,y,z)	spinand_dma_write_x1(x,y,z)

static void write_flash_ctrl_reg(unsigned int ofs, unsigned int data)
{
	//printf("ew %x %x\n",(unsigned long *)ofs, data);
	writel(data, (unsigned long *)ofs);
}

static unsigned int read_flash_ctrl_reg(unsigned int ofs)
{
	//printf("dw %x %x\n", (unsigned long *)ofs, readl((unsigned long *)ofs));
	return readl((unsigned long *)ofs);
}

static int wait_sflash_cmd_rdy(int timeout)
{
	int tmp;
	int cnt;
	cnt = 0;
	do {
		udelay(1);
		cnt ++;
		tmp = read_flash_ctrl_reg(FLASH_FLASH_ACCESS_START);
		if (((tmp >> 1) & 0x1) == 0) {
			udelay(15);
			return 0;
		}
	} while ( cnt < (timeout * 1000) );
	return -1;
}

int
spinand_mem_cmp(u32 dst1, u32 src1, u32 length1)
{
    u32    i, length;
    u32    dst, src;
    u32    dst_value, src_value;
    u8     error;

    // printf("\nSPIcmp().");
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

/*
 * spinand_read_status- send command 0xf to the SPI Nand status register
 * Description:
 *    After read, write, or erase, the Nand device is expected to set the
 *    busy status.
 *    This function is to allow reading the status of the command: read,
 *    write, and erase.
 *    Once the status turns to be ready, the other status bits also are
 *    valid status bits.
 */
static int spinand_read_bp_otp_status_reg(uint8_t * status, uint8_t addr)
{
	unsigned int reg_data;
	FLASH_SF_ACCESS_t sf_access;
	FLASH_SF_EXT_ACCESS_t sf_ext_access;

	write_flash_ctrl_reg(FLASH_TYPE, spinand_type);

	/* Get feature use 1 byte address as sub-command.
	 * We must use extend access code to meet this protocol.
	 */
	sf_access.wrd = 0;
	sf_access.bf.sflashOpCode = CMD_GET_FEATURE_REG;
	sf_access.bf.sflashAcCode = SF_AC_OPCODE_EXTEND;
	sf_access.bf.sflashCeAlt = SF_ACCESS_CE_ALT;
	write_flash_ctrl_reg(FLASH_SF_ACCESS, sf_access.wrd);

	sf_ext_access.wrd = 0;
	/* Use command in extend_acc register */
	sf_ext_access.bf.sflashDirRdCmdEn = 1;
	sf_ext_access.bf.sflashOpCode = CMD_GET_FEATURE_REG;
	/* No dummy cnt */
	sf_ext_access.bf.sflashDummyCount = -1;
	/* 1 byte address */
	sf_ext_access.bf.sflashAddrCount = 0;
	/* 1 byte data */
	sf_ext_access.bf.sflashDataCount = 0;
	write_flash_ctrl_reg(FLASH_SF_EXT_ACCESS, sf_ext_access.wrd);

	/* Command to get status */
	write_flash_ctrl_reg(FLASH_SF_ADDRESS, addr);

	/* Issue command */
	write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, SF_START_INDIRECT_ENABLE);

	/* Wait command ready */
	if (wait_sflash_cmd_rdy(SFLASH_CMD_TIMEOUT) != 0)
		return -1;

	/* Read out status */
	reg_data = read_flash_ctrl_reg(FLASH_SF_DATA);
	*status = reg_data & 0xFF;

	return 0;
}

/*
 * spinand_Get_feature_reg- send command 0x0f with address to Set Feature register
 * Description:
 * if address is a0 -> Block Protection
 * if address is b0 -> Secure OTP
 * if address is c0 -> Status
 */
void spinand_get_feature_reg(uint8_t addr)
{
	u8 tmp_status;
	tmp_status = 0;

	switch(addr)
	{
		case REG_BLOCK_LOCK:
			spinand_read_bp_otp_status_reg(&tmp_status, REG_BLOCK_LOCK);
			printf("Get Feature Block Protection: addr = 0x%x, value = 0x%x\n", addr, tmp_status);
			break;
		case REG_OTP:
			spinand_read_bp_otp_status_reg(&tmp_status, REG_OTP);
			printf("Get Feature Secure OTP: addr = 0x%x, value = 0x%x\n", addr, tmp_status);
			break;
		case REG_STATUS:
			spinand_read_bp_otp_status_reg(&tmp_status, REG_STATUS);
			printf("Get Feature Status: addr = 0x%x, value = 0x%x\n", addr, tmp_status);
			break;
		default:
			printf("Get Feature: addr error!!!\n");
			break;
	}
}

/*
 * spinand_set_feature_reg- send command 0x1f with address to Set Feature register
 * Description:
 * if address is a0 -> Block Protection
 * if address is b0 -> Secure OTP
 * if address is c0 -> Status
 */
static int spinand_set_feature_reg(uint8_t addr, uint8_t reg_v)
{
	FLASH_SF_ACCESS_t sf_access;
	FLASH_SF_EXT_ACCESS_t sf_ext_access;

	write_flash_ctrl_reg(FLASH_TYPE, spinand_type);

	/* Set feature use 1 byte address as sub-command.
	 * We must use extend access code to meet this protocol.
	 */
	sf_access.wrd = 0;
	sf_access.bf.sflashOpCode = CMD_SET_FEATURE_REG;
	/* sflashAcCode = 0xf */
	sf_access.bf.sflashAcCode = SF_AC_OPCODE_EXTEND;
	sf_access.bf.sflashCeAlt = SF_ACCESS_CE_ALT;
	write_flash_ctrl_reg(FLASH_SF_ACCESS, sf_access.wrd);

	/* Use command in extend_acc register */
	sf_ext_access.wrd = 0;
	sf_ext_access.bf.sflashOpCode = CMD_SET_FEATURE_REG;
	/* 1 byte data */
	sf_ext_access.bf.sflashDataCount = 0;
	/* 1 byte address */
	sf_ext_access.bf.sflashAddrCount = 0;
	/* No dummy cnt */
	sf_ext_access.bf.sflashDummyCount = -1;
	sf_ext_access.bf.sflashDirRdCmdEn = 1;
	write_flash_ctrl_reg(FLASH_SF_EXT_ACCESS, sf_ext_access.wrd);

	/* Command to set status */
	write_flash_ctrl_reg(FLASH_SF_ADDRESS, addr);

	/* Data register */
	write_flash_ctrl_reg(FLASH_SF_DATA, reg_v);

	/* Issue command */
	write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, SF_START_INDIRECT_ENABLE);

	/* Wait command ready */
	if (wait_sflash_cmd_rdy(SFLASH_CMD_TIMEOUT) != 0)
		return -1;

	if (addr == REG_BLOCK_LOCK)
		printf("Set Feature Block Protection: addr = 0x%x, value = 0x%x\n", addr, reg_v);
	if (addr == REG_OTP)
		printf("Set Feature Secure OTP: addr = 0x%x, value = 0x%x\n", addr, reg_v);
	if (addr == REG_STATUS)
		printf("Set Feature Status: addr = 0x%x, value = 0x%x\n", addr, reg_v);

	return 0;
}

/* wait_till_ready- Check device status
 * Description:
 *    Read device status register until timeout or OIP cleared.
 *
 */
#define MAX_WAIT_MS  (10)		/* erase might take 4ms */
static int wait_till_ready(void)
{
	int retval;
	u8 stat = 0;
	unsigned long timebase;

	timebase = get_timer(0);
	do {
		retval = spinand_read_bp_otp_status_reg(&stat, REG_STATUS);
		if (retval < 0)
			return -1;
		else if ((stat & (STATUS_E_FAIL_MASK | STATUS_P_FAIL_MASK)))
			break;
		else if (!(stat & STATUS_BUSY))
			break;
	} while (get_timer(timebase) < MAX_WAIT_MS);

	if (stat & (STATUS_E_FAIL_MASK | STATUS_P_FAIL_MASK)) {
		printf("Staus[0x%x] error!\n", stat);
		return -1;
	}

	if ((stat & STATUS_BUSY) == 0)
		return 0;

	return -1;
}

/*
 * spinand_write_enable- Enable write operation(erase,program)
 * Description:
 *    Enable write operation
 */
static int spinand_write_enable(void)
{
	FLASH_SF_ACCESS_t sf_access;

	write_flash_ctrl_reg(FLASH_TYPE, spinand_type);

	/* Write enable
	 * Opcode only protocol
	 */
	sf_access.wrd = 0;
	sf_access.bf.sflashOpCode = CMD_WR_ENABLE;
	sf_access.bf.sflashCeAlt = SF_ACCESS_CE_ALT;
	write_flash_ctrl_reg(FLASH_SF_ACCESS, sf_access.wrd);
	write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, SF_START_INDIRECT_ENABLE);

	/* Wait command ready */
	if (wait_sflash_cmd_rdy(SFLASH_CMD_TIMEOUT) != 0)
		printf("%s: command timeout!\n", __func__);

	return 0;
}

/*
 * spinand_write_disable- Enable write operation(erase,program)
 * Description:
 *    Disable write operation
 */
static int spinand_write_disable(void)
{
	FLASH_SF_ACCESS_t sf_access;

	write_flash_ctrl_reg(FLASH_TYPE, spinand_type);

	/* Write disable
	 * Opcode only protocol
	 */
	sf_access.wrd = 0;
	sf_access.bf.sflashOpCode = CMD_WR_DISABLE;
	sf_access.bf.sflashCeAlt = SF_ACCESS_CE_ALT;
	write_flash_ctrl_reg(FLASH_SF_ACCESS, sf_access.wrd);
	write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, SF_START_INDIRECT_ENABLE);

	/* Wait command ready */
	if (wait_sflash_cmd_rdy(SFLASH_CMD_TIMEOUT) != 0)
		printf("%s: command timeout!\n", __func__);

	return 0;
}

int spinand_program_execute(u32 offset)
{
	FLASH_SF_ACCESS_t sf_access;
	FLASH_SF_EXT_ACCESS_t sf_ext_access;

	write_flash_ctrl_reg(FLASH_TYPE, spinand_type);

	/* Flush cache to flash chip
	 * Opcode -> 24bits offset addr
	 */
	sf_access.wrd = 0;
	sf_access.bf.sflashAcCode = SF_AC_OPCODE_3_ADDR;
	sf_access.bf.sflashCeAlt = SF_ACCESS_CE_ALT;
	sf_access.bf.sflashOpCode = CMD_PROG_PAGE_EXC;
	write_flash_ctrl_reg(FLASH_SF_ACCESS, sf_access.wrd);

	sf_ext_access.wrd = 0;
	/* Use command in extend_acc register */
	sf_ext_access.bf.sflashDirRdCmdEn = 1;
	sf_ext_access.bf.sflashOpCode = CMD_PROG_PAGE_EXC;
	/* 0 dummy bits */
	sf_ext_access.bf.sflashDummyCount = -1;
	/* 3 byte address */
	sf_ext_access.bf.sflashAddrCount = 2;
	/* zero data byte */
	sf_ext_access.bf.sflashDataCount = -1;
	write_flash_ctrl_reg(FLASH_SF_EXT_ACCESS, sf_ext_access.wrd);

	/* Fill address */
	write_flash_ctrl_reg(FLASH_SF_ADDRESS, offset);

	write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, SF_START_INDIRECT_ENABLE);

	wait_sflash_cmd_rdy(SFLASH_CMD_TIMEOUT);

	return 0;
}

/*
 * spinand_reset- Reset controller
 * Description:
 *    Reset: reset SPI Nand memory
 */
void spinand_reset_controller(void)
{
	GLOBAL_BLOCK_RESET_SECURE_t global_block_reset_secure;
	
	global_block_reset_secure.wrd = 0;
	global_block_reset_secure.bf.reset_flash = 1;
	write_flash_ctrl_reg(GLOBAL_BLOCK_RESET_SECURE, global_block_reset_secure.wrd);

	udelay(10);

	global_block_reset_secure.wrd = 0;
	write_flash_ctrl_reg(GLOBAL_BLOCK_RESET_SECURE, global_block_reset_secure.wrd);
}

/*
 * spinand_reset_memory- Reset SPI Nand
 * Description:
 *    Reset: reset SPI Nand memory
 */
static int spinand_reset_memory(void)
{
	FLASH_SF_ACCESS_t sf_access;
	sf_access.wrd = 0;

	write_flash_ctrl_reg(FLASH_TYPE, spinand_type);

	/* Reset SPI-NAND
	 * Opcode only protocol
	 */
	sf_access.bf.sflashOpCode = CMD_RESET_MEMORY;
	sf_access.bf.sflashAcCode = SF_AC_OPCODE;
	sf_access.bf.sflashCeAlt = SF_ACCESS_CE_ALT;
	write_flash_ctrl_reg(FLASH_SF_ACCESS, sf_access.wrd);
	write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, SF_START_INDIRECT_ENABLE);
	if (wait_sflash_cmd_rdy(SFLASH_CMD_TIMEOUT) != 0)
		return -1;

	udelay(15);

	return 0;
}

/*
 * spinand_reset- Reset SPI Nand device
 * Description:
 *    Reset: reset SPI Nand device
 */
static int spinand_reset(void)
{
	FLASH_SF_ACCESS_t sf_access;

	write_flash_ctrl_reg(FLASH_TYPE, spinand_type);

	/* Reset SPI-NAND FLASH
	 * Opcode only protocol
	 */
	sf_access.wrd = 0;
	sf_access.bf.sflashOpCode = CMD_RESET;
	sf_access.bf.sflashAcCode = SF_AC_OPCODE;
	sf_access.bf.sflashCeAlt = SF_ACCESS_CE_ALT;
	write_flash_ctrl_reg(FLASH_SF_ACCESS, sf_access.wrd);
	write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, SF_START_INDIRECT_ENABLE);

	/* Wait command ready */
	if (wait_sflash_cmd_rdy(SFLASH_CMD_TIMEOUT) != 0) {
		printf("%s fail!\n", __func__);
		return -1;
	}

	/* elapse 1ms before issuing any other command */
	udelay(1000);

	if (wait_till_ready())
		printf("%s wait timeout!\n", __func__);
	return 0;
}

/*
 * spinand_read_id- Read SPI Nand ID
 * Description:
 *    Read ID: read two ID bytes from the SPI Nand device
 */
static int spinand_read_id(u8 * id)
{
	int tmout = 0;
	unsigned int reg_data;
	FLASH_SF_ACCESS_t sf_access;
	FLASH_SF_EXT_ACCESS_t sf_ext_access;

	write_flash_ctrl_reg(FLASH_TYPE, spinand_type);

	/* Read ID
	 * Micron/Macronix:
	 *  Opcode -> 8 dummy bute -> MAF_ID => DEV_ID
	 * GigaDevice:
	 *  Opcode -> MAF_ID => DEV_ID
	 */
	sf_access.wrd = 0;
	sf_access.bf.sflashAcCode = SF_AC_OPCODE_EXTEND;
	sf_access.bf.sflashCeAlt = SF_ACCESS_CE_ALT;
	sf_access.bf.sflashOpCode = CMD_READ_ID;
	write_flash_ctrl_reg(FLASH_SF_ACCESS, sf_access.wrd);

	sf_ext_access.wrd = 0;
	/* Use command in extend_acc register */
	sf_ext_access.bf.sflashDirRdCmdEn = 1;
	sf_ext_access.bf.sflashOpCode = CMD_READ_ID;
	/* 8 dummy bits */
	sf_ext_access.bf.sflashDummyCount = -1;
	/* 0 address byte */
	sf_ext_access.bf.sflashAddrCount = -1;
	/* page length */
	sf_ext_access.bf.sflashDataCount = 2;
	write_flash_ctrl_reg(FLASH_SF_EXT_ACCESS, sf_ext_access.wrd);

	/* Start request */
	write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, SF_START_INDIRECT_ENABLE);

	if (wait_sflash_cmd_rdy(SFLASH_CMD_TIMEOUT) != 0)
		tmout = 1;

	if (tmout) {
		id[0] = id[1] = id[2] = 0;
		printf("SPI_NAND ID: 0x%X%X%X\n", id[0], id[1], id[2]);
		return -1;
	}

	reg_data = read_flash_ctrl_reg(FLASH_SF_DATA);
	//printf("SPI_NAND ID: 0x%x\n", reg_data);
	id[0] = reg_data & 0x000000ff;
	id[1] = (reg_data & 0x0000ff00) >> 8;
	id[2] = (reg_data & 0x00ff0000) >> 16;
	printf("SPI_NAND ID: 0x%X%X\n", id[1], id[2]);
	return 0;
}

int spinand_flash_loop_back_irq_handle(void)
{
	/* Do clear interrupt */
	write_flash_ctrl_reg(DMA_SEC_DMA_SSP_Q_RXQ_COAL_INTERRUPT, 0x1);
	printf("DMA finished!!\n");
	return 0;
}

int spinand_flash_register_irq_handler(void)
{
	IRQ_HANDLE FlashIrqHandle;
	FlashIrqHandle.Data = 0;
	FlashIrqHandle.IrqNum = ( 26 + 32 );
	FlashIrqHandle.IrqFun = (IRQ_FUN) spinand_flash_loop_back_irq_handle;
	FlashIrqHandle.Priority = 0;

	/* init interrupt */
	irq_install_handler(FlashIrqHandle.IrqNum, FlashIrqHandle.IrqFun, (u32)(FlashIrqHandle.Data));
	return 0;
}

int spinand_dma_read_x1(u32 offset, int len, u8 * buf)
{
	unsigned int reg_data, tmp_timer;
	FLASH_SF_ACCESS_t sf_access;
	FLASH_SF_EXT_ACCESS_t sf_ext_access;
	FLASH_FIFO_CONTROL_t sf_fifo_control;
	DMA_SEC_DMA_SSP_Q_TXQ_BASE_DEPTH_t dma_ssp_txq_base_depth;
	DMA_SEC_DMA_SSP_Q_RXQ_BASE_DEPTH_t dma_ssp_rxq_base_depth;
	FLASH_RXDESC_ADDR_BASE_t flash_rxdesc_addr_base;
	FLASH_TXDESC_ADDR_BASE_t flash_txdesc_addr_base;
	reg_data = 0;
	tmp_timer = 0;

	printf(".");
	write_flash_ctrl_reg(FLASH_TYPE, spinand_type);

	/* Read from cache
	 * Opcode -> 16bits offset addr -> data
	 * 2 bytes address must use extend access mothod
	 */
	sf_access.wrd = 0;
	sf_access.bf.sflashOpCode = CMD_READ_SISO;
	sf_access.bf.sflashAcCode = SF_AC_OPCODE_EXTEND;
	sf_access.bf.sflashCeAlt = SF_ACCESS_CE_ALT;
	//sf_access.bf.sflashForceBurst = 1;
	write_flash_ctrl_reg(FLASH_SF_ACCESS, sf_access.wrd);

	sf_ext_access.wrd = 0;
	sf_ext_access.bf.sflashDirRdCmdEn = 1;
	sf_ext_access.bf.sflashOpCode = CMD_READ_SISO;
	sf_ext_access.bf.sflashDummyCount = 7;
	sf_ext_access.bf.sflashAddrCount = SF_ADDR_COUNT;
	sf_ext_access.bf.sflashDataCount = (len - 1);
	write_flash_ctrl_reg(FLASH_SF_EXT_ACCESS, sf_ext_access.wrd);

	write_flash_ctrl_reg(FLASH_SF_ADDRESS, offset);

	sf_fifo_control.wrd = 0;
	sf_fifo_control.bf.fifoWRTH = 3;
	sf_fifo_control.bf.fifoRDTH = 3;
	sf_fifo_control.bf.fifoClear = 1;
	sf_fifo_control.bf.fifoDbgSel = 0;
	sf_fifo_control.bf.fifoCmd = 2;
	write_flash_ctrl_reg(FLASH_FIFO_CONTROL, sf_fifo_control.wrd);

	dma_ssp_txq_base_depth.wrd = 0;
	dma_ssp_txq_base_depth.bf.depth = 1;
	dma_ssp_txq_base_depth.bf.base = (FLASH_TXDESC_ADDR_BASE >> 4);

	dma_ssp_rxq_base_depth.wrd = 0;
	dma_ssp_rxq_base_depth.bf.depth = 1;
	dma_ssp_rxq_base_depth.bf.base = (FLASH_RXDESC_ADDR_BASE >> 4);

	flash_txdesc_addr_base.wrd = 0;
	flash_txdesc_addr_base.bf.buffer_addr = 0;
	flash_txdesc_addr_base.bf.buffer_len = len;
	flash_txdesc_addr_base.bf.sgm = 1;
	flash_txdesc_addr_base.bf.own = 0;

	flash_rxdesc_addr_base.wrd = 0;
	flash_rxdesc_addr_base.bf.buffer_addr = 0;
	flash_rxdesc_addr_base.bf.buffer_len = len;
	flash_rxdesc_addr_base.bf.own = 0;

	/* Start access */
	write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, SF_START_DMA_ENABLE);

	/* Wait command ready */
	if (wait_sflash_cmd_rdy(SFLASH_CMD_TIMEOUT) != 0) {
		printf("%s fail!\n", __func__);
		return -1;
	}

	/* DMA begin */
	/* Set TX RX register */
	write_flash_ctrl_reg(DMA_SEC_DMA_GLB_DMA_SSP_TX_CTRL, 0); /* Disable TX */
	write_flash_ctrl_reg(DMA_SEC_DMA_GLB_DMA_SSP_RX_CTRL, 0); /* Disable RX */
	write_flash_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_CONTROL, 2); /* flush TXQ5 */
	write_flash_ctrl_reg(DMA_SEC_DMA_SSP_Q_RXQ_CONTROL, 2); /* flush TXQ5 */
	/* enable dma tx+rx */
	write_flash_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_CONTROL, 9); /* en TXQ5 */
	write_flash_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_BASE_DEPTH, dma_ssp_txq_base_depth.wrd);
	write_flash_ctrl_reg(DMA_SEC_DMA_SSP_Q_RXQ_CONTROL, 8); /* en RXQ5 */
	write_flash_ctrl_reg(DMA_SEC_DMA_SSP_Q_RXQ_BASE_DEPTH, dma_ssp_rxq_base_depth.wrd);
	/* prepare TXQ descriptor */
	write_flash_ctrl_reg(FLASH_TXDESC_ADDR_BASE, (FIFO_ADDR + offset));
	write_flash_ctrl_reg((FLASH_TXDESC_ADDR_BASE + 4), flash_txdesc_addr_base.wrd);
	write_flash_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_WPTR, 1);
	/* prepare RXQ descriptor */
	write_flash_ctrl_reg(FLASH_RXDESC_ADDR_BASE, buf);
	write_flash_ctrl_reg((FLASH_RXDESC_ADDR_BASE + 4), flash_rxdesc_addr_base.wrd);
	write_flash_ctrl_reg(DMA_SEC_DMA_SSP_Q_RXQ_RPTR, 0);
	/* dma tx + rx */
	write_flash_ctrl_reg(DMA_SEC_DMA_GLB_DMA_SSP_TX_CTRL, 1); /* enable TX */
	write_flash_ctrl_reg(DMA_SEC_DMA_GLB_DMA_SSP_RX_CTRL, 1); /* enable RX */

	/* get interrupt event and clear interrupt */
	/* register irq handler for DMA */
	do
	{
		// dw 0xf4324014
		//reg_data = read_flash_ctrl_reg(FLASH_FLASH_INTERRUPT);
		udelay(5);
		tmp_timer += 5;
		if (tmp_timer >= 2000)
		{
			//write_flash_ctrl_reg(DMA_SEC_DMA_SSP_Q_RXQ_COAL_INTERRUPT, 1);
			//printf("%s DMA wait timeout!!!\n", __func__);

			/* Finish burst write  */
			//sf_access.bf.sflashForceBurst = 0;
			//sf_access.bf.sflashForceTerm = 1;	/* Terminate burst mode */
			//write_flash_ctrl_reg(FLASH_SF_ACCESS, sf_access.wrd);
			break;
			//return -1;
		}
	} while (1);
	// ew 0xf7001a50 1
	write_flash_ctrl_reg(DMA_SEC_DMA_SSP_Q_RXQ_COAL_INTERRUPT, 1);

	//spinand_flash_register_irq_handler();
	/* DMA end */

	/* Finish burst read  */
	// 0xf4324030 0x1000
	//sf_access.bf.sflashForceBurst = 0;
	//sf_access.bf.sflashForceTerm = 1;
	//write_flash_ctrl_reg(FLASH_SF_ACCESS, sf_access.wrd);

	return 0;
}

static int spinand_dma_read(u32 offset, size_t len, void *buf)
{
	u32 page_id;
	u32 plane_select;
#if 0
	/* Handle head non-align part */
	if (offset & spinand_page_sz_mask) {
		page_id = offset / spinand_page_size;
		printf("%s non-alignment read :0x%x\n", __func__, page_id);
		plane_select = (page_id & PLANE_SELECT) ? PLANE_SELECT_BIT : 0;

		/* Read first page to flash's cache */
		if (spinand_read_page_to_cache(page_id) < 0) {
			printf("%s read to cache fail 1, page:%d\n", __func__, page_id);
			return -1;
		}

		/* Read first page from flash's cache to ddr*/
		DMA_READ_FROM_CACHE((offset & spinand_page_sz_mask) | plane_select,
				spinand_page_size - (offset & spinand_page_sz_mask),
				buf);

		len -= (spinand_page_size - (offset & spinand_page_sz_mask));
		buf += (spinand_page_size - (offset & spinand_page_sz_mask));
		offset += (spinand_page_size - (offset & spinand_page_sz_mask));
	}
#endif
	while (len >= spinand_page_size) {
		page_id = offset / spinand_page_size;
		plane_select = (page_id & PLANE_SELECT) ? PLANE_SELECT_BIT : 0;

		/* Read continuous pages to flash's cache */
		if (spinand_read_page_to_cache(page_id) < 0) {
			printf("%s read to cache fail 2, page:%d\n", __func__, page_id);
			return -1;
		}

		/* Read continuous pages from flash's cache (page head) to ddr*/

		/* Read pages from flash's cache (page head) to ddr*/
		DMA_READ_FROM_CACHE(0 | plane_select, spinand_page_size, buf);
		buf += spinand_page_size;
		offset += spinand_page_size;
		len -= spinand_page_size;
	}

	/* Handle tail non-align part */
	if (len > 0) {
		page_id = offset / spinand_page_size;
		plane_select = (page_id & PLANE_SELECT) ? PLANE_SELECT_BIT : 0;
		/* Read tail data to flash's cache */
		if (spinand_read_page_to_cache(page_id) < 0) {
			printf("%s read to cache fail 3, page:%d\n", __func__, page_id);
			return -1;
		}

		/* Read tail data from flash's cache(page head) to ddr */
		DMA_READ_FROM_CACHE(0 | plane_select, len, buf);
	}

	printf("\n");

	printf("%s DMA Mode Read from cache finish!!!\n", __func__);
	return 0;
}

int spinand_dma_write_x1(u32 offset, size_t len, unsigned char *buf)
{
	unsigned int reg_data, tmp_timer;
	FLASH_SF_ACCESS_t sf_access;
	FLASH_SF_EXT_ACCESS_t sf_ext_access;
	FLASH_FLASH_ACCESS_START_t access_start;
	FLASH_FIFO_CONTROL_t sf_fifo_control;
	DMA_SEC_DMA_SSP_Q_TXQ_BASE_DEPTH_t dma_ssp_txq_base_depth;
	DMA_SEC_DMA_SSP_Q_RXQ_BASE_DEPTH_t dma_ssp_rxq_base_depth;
	FLASH_RXDESC_ADDR_BASE_t flash_rxdesc_addr_base;
	FLASH_TXDESC_ADDR_BASE_t flash_txdesc_addr_base;
	reg_data = 0;
	tmp_timer = 0;

	printf(".");
	write_flash_ctrl_reg(FLASH_TYPE, spinand_type);

	sf_access.wrd = 0;
	sf_access.bf.sflashOpCode = CMD_PROG_PAGE_X1;
	sf_access.bf.sflashAcCode = SF_AC_OPCODE_EXTEND;
	sf_access.bf.sflashCeAlt = SF_ACCESS_CE_ALT;
	sf_access.bf.sflashForceBurst = 1;	/* Burst mode */
	write_flash_ctrl_reg(FLASH_SF_ACCESS, sf_access.wrd);

	sf_ext_access.wrd = 0;
	sf_ext_access.bf.sflashDirRdCmdEn = 1;
	sf_ext_access.bf.sflashOpCode = CMD_PROG_PAGE_X1;
	sf_ext_access.bf.sflashDummyCount = -1;
	sf_ext_access.bf.sflashAddrCount = SF_ADDR_COUNT;
	sf_ext_access.bf.sflashDataCount = (len - 1);
	write_flash_ctrl_reg(FLASH_SF_EXT_ACCESS, sf_ext_access.wrd);

	write_flash_ctrl_reg(FLASH_SF_ADDRESS, offset);

	sf_fifo_control.wrd = 0;
	sf_fifo_control.bf.fifoWRTH = 3;
	sf_fifo_control.bf.fifoRDTH = 3;
	sf_fifo_control.bf.fifoClear = 1;
	sf_fifo_control.bf.fifoDbgSel = 0;
	sf_fifo_control.bf.fifoCmd = 3;
	write_flash_ctrl_reg(FLASH_FIFO_CONTROL, sf_fifo_control.wrd);

	dma_ssp_txq_base_depth.wrd = 0;
	dma_ssp_txq_base_depth.bf.depth = 1;
	dma_ssp_txq_base_depth.bf.base = (FLASH_TXDESC_ADDR_BASE >> 4);

	dma_ssp_rxq_base_depth.wrd = 0;
	dma_ssp_rxq_base_depth.bf.depth = 1;
	dma_ssp_rxq_base_depth.bf.base = (FLASH_RXDESC_ADDR_BASE >> 4);

	flash_txdesc_addr_base.wrd = 0;
	flash_txdesc_addr_base.bf.buffer_addr = 0;
	flash_txdesc_addr_base.bf.buffer_len = len;
	flash_txdesc_addr_base.bf.sgm = 1;
	flash_txdesc_addr_base.bf.own = 0;

	flash_rxdesc_addr_base.wrd = 0;
	flash_rxdesc_addr_base.bf.buffer_addr = 0;
	flash_rxdesc_addr_base.bf.buffer_len = len;
	flash_rxdesc_addr_base.bf.own = 0;

	/* Start access */
	access_start.wrd = 0;
	access_start.bf.sflashFifoReq = 1;
	access_start.bf.sflashRegCmd = 1;
	write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, access_start.wrd);

	/* Wait command ready */
	if (wait_sflash_cmd_rdy(SFLASH_CMD_TIMEOUT) != 0) {
		printf("%s fail!\n", __func__);
		return -1;
	}

	/* DMA begin */
	/* Set TX RX register */
	write_flash_ctrl_reg(DMA_SEC_DMA_GLB_DMA_SSP_TX_CTRL, 0); /* Disable TX */
	write_flash_ctrl_reg(DMA_SEC_DMA_GLB_DMA_SSP_RX_CTRL, 0); /* Disable RX */
	// ew 0xf7001a18 2
	write_flash_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_CONTROL, 2); /* flush TXQ5 */
	// ew 0xf7001a00 2
	write_flash_ctrl_reg(DMA_SEC_DMA_SSP_Q_RXQ_CONTROL, 2); /* flush TXQ5 */
	/* enable dma tx+rx */
	// ew 0xf7001a18 9
	write_flash_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_CONTROL, 9); /* en TXQ5 */
	write_flash_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_BASE_DEPTH, dma_ssp_txq_base_depth.wrd);
	// ew 0xf7001a00 8
	write_flash_ctrl_reg(DMA_SEC_DMA_SSP_Q_RXQ_CONTROL, 8); /* en RXQ5 */
	write_flash_ctrl_reg(DMA_SEC_DMA_SSP_Q_RXQ_BASE_DEPTH, dma_ssp_rxq_base_depth.wrd);
	/* prepare TXQ descriptor */
	write_flash_ctrl_reg(FLASH_TXDESC_ADDR_BASE, buf);
	write_flash_ctrl_reg((FLASH_TXDESC_ADDR_BASE + 4), flash_txdesc_addr_base.wrd);
	write_flash_ctrl_reg(DMA_SEC_DMA_SSP_Q_TXQ_WPTR, 1);
	/* prepare RXQ descriptor */
	write_flash_ctrl_reg(FLASH_RXDESC_ADDR_BASE, (FIFO_ADDR + offset));
	write_flash_ctrl_reg((FLASH_RXDESC_ADDR_BASE + 4), flash_rxdesc_addr_base.wrd);
	write_flash_ctrl_reg(DMA_SEC_DMA_SSP_Q_RXQ_RPTR, 0);
	/* dma tx + rx */
	write_flash_ctrl_reg(DMA_SEC_DMA_GLB_DMA_SSP_TX_CTRL, 1); /* enable TX */
	write_flash_ctrl_reg(DMA_SEC_DMA_GLB_DMA_SSP_RX_CTRL, 1); /* enable RX */

	/* get interrupt event and clear interrupt */
	/* register irq handler for DMA */
	do
	{
		reg_data = read_flash_ctrl_reg(FLASH_FLASH_INTERRUPT);
		udelay(5);
		tmp_timer += 5;
		if (tmp_timer >= 2000)
		{
			write_flash_ctrl_reg(DMA_SEC_DMA_SSP_Q_RXQ_COAL_INTERRUPT, 1);
			printf("%s DMA wait timeout!!!\n", __func__);

			/* Finish burst write  */
			sf_access.bf.sflashForceBurst = 0;
			sf_access.bf.sflashForceTerm = 1;	/* Terminate burst mode */
			write_flash_ctrl_reg(FLASH_SF_ACCESS, sf_access.wrd);

			return -1;
		}
	} while (((reg_data >> 1) & 1) != 1);
	write_flash_ctrl_reg(DMA_SEC_DMA_SSP_Q_RXQ_COAL_INTERRUPT, 1);

	//spinand_flash_register_irq_handler();
	/* DMA end */

	/* Finish burst write  */
	sf_access.bf.sflashForceBurst = 0;
	sf_access.bf.sflashForceTerm = 1;	/* Terminate burst mode */
	write_flash_ctrl_reg(FLASH_SF_ACCESS, sf_access.wrd);

	return 0;
}

static int spinand_dma_write(u32 offset, size_t len, void *buf)
{
	u32 page_id;
	u32 plane_select;
	unsigned char *src = (unsigned char *)buf;

	if (len < 0) {
		return -1;
	}

	/* Sanity check */
	if (offset & spinand_page_sz_mask) {
		printf("Non alignment write(ADDR) not supported!\nThe unit is 8hex!!!\n");
		return -1;
	}

	if (len % spinand_page_size) {
		printf("Non alignment write(SIZE) not supported!\nThe unit is a PAGE!!!\n");
		return -1;
	}

	while (len > 0) {
		page_id = offset / spinand_page_size;
		plane_select = (page_id & PLANE_SELECT) ? PLANE_SELECT_BIT : 0;

		/* Enable write */
		spinand_write_enable();

		/* Write data to cache
		 * Could be x1(02h) or x4(32h)
		 */
		DMA_WRITE_TO_CACHE(0 | plane_select, spinand_page_size, buf);
		/* Program execute (Serial NAND only) */
		spinand_program_execute(page_id);
		udelay(15);
		/* Wait until program finish */
		if (wait_till_ready()) {
			printf("%s wait timeout 0x%x!\n", __func__, page_id);
			return -1;
		}

		offset += spinand_page_size;
		src += spinand_page_size;
		len -= spinand_page_size;
	}

	printf("\n");

	printf("%s DMA Mode Write to cache finish!!!\n", __func__);
	return 0;
}

int spinand_read_oob(struct spi_flash *snandflash, u32 offset, void *buf)
{
	unsigned int page_id = offset / spinand_page_size;
	unsigned int plane_select;
	u8 *buff8 = buf;

	plane_select = (page_id & PLANE_SELECT) ? PLANE_SELECT_BIT : 0;

	/* Read to cache */
	if (spinand_read_page_to_cache(page_id) < 0) {
		printf("%s read to cache fail, page:%d\n", __func__, page_id);
		return -1;
	}

	/* Read from page head */
	READ_FROM_CACHE(spinand_page_size | plane_select, spinand_oob_size, buf);
	if (buff8[0] != 0xff)
		printf("%x OOB:%x\n", offset, buff8[0]);

	return 0;
}

/* Page Read from Cache */
int spinand_read_page_to_cache(u32 page_id)
{
	volatile u8 status;
	FLASH_SF_ACCESS_t sf_access;

	write_flash_ctrl_reg(FLASH_TYPE, spinand_type);

	/* Read to cache
	 * Opcode -> 24bits offset addr => Get feature ....
	 * 3 bytes address must rely on extend access mothod
	 */
	sf_access.wrd = 0;
	sf_access.bf.sflashOpCode = CMD_PAGE_READ;
	sf_access.bf.sflashAcCode = SF_AC_OPCODE_3_ADDR;
	sf_access.bf.sflashCeAlt = SF_ACCESS_CE_ALT;
	write_flash_ctrl_reg(FLASH_SF_ACCESS, sf_access.wrd);

	/* Fill address */
	write_flash_ctrl_reg(FLASH_SF_ADDRESS, page_id);

	write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, SF_START_INDIRECT_ENABLE);

	/* Wait command ready */
	if (wait_sflash_cmd_rdy(SFLASH_CMD_TIMEOUT) != 0) {
		printf("%s fail!\n", __func__);
		return -1;
	}

	/* 80uS with ECC enabled per Micron datasheet */
	udelay(10);

	if (wait_till_ready()) {
		printf("%s wait timeout %d!\n", __func__, page_id);
		return -1;
	}

	spinand_read_bp_otp_status_reg(&status, REG_STATUS);
	if ((status & STATUS_OIP_MASK) == STATUS_READY) {
		if ((status & STATUS_ECC_MASK) == STATUS_ECC_ERROR) {
			printf("ECC error! page:%d\n", page_id);
			return -1;
		}
		if ((status & STATUS_ECC_MASK) == STATUS_ECC_1BIT_CORRECTED)
			printf("Bit corrected! page:%d\n", page_id);
	}

	return 0;
}

int spinand_read_cache_x1(u32 offset, int len, u8 * buf)
{
	unsigned int reg_data;
	FLASH_SF_ACCESS_t sf_access;
	FLASH_SF_EXT_ACCESS_t sf_ext_access;
	reg_data = 0;

	printf(".");
	write_flash_ctrl_reg(FLASH_TYPE, spinand_type);

	/* Read from cache
	 * Opcode -> 16bits offset addr -> data
	 * 2 bytes address must use extend access mothod
	 */
	sf_access.wrd = 0;
	sf_access.bf.sflashOpCode = CMD_READ_SISO;
	sf_access.bf.sflashAcCode = SF_AC_OPCODE_EXTEND;
	sf_access.bf.sflashCeAlt = SF_ACCESS_CE_ALT;
	sf_access.bf.sflashForceBurst = 1;
	write_flash_ctrl_reg(FLASH_SF_ACCESS, sf_access.wrd);

	sf_ext_access.wrd = 0;
	/* Use command in extend_acc register */
	sf_ext_access.bf.sflashDirRdCmdEn = 1;
	sf_ext_access.bf.sflashOpCode = CMD_READ_SISO;
	/* 8 dummy bits */
	sf_ext_access.bf.sflashDummyCount = 7;
	/* 2 byte address */
	sf_ext_access.bf.sflashAddrCount = SF_ADDR_COUNT;
	/* 4 bytes in one command request */
	sf_ext_access.bf.sflashDataCount = 3;
	write_flash_ctrl_reg(FLASH_SF_EXT_ACCESS, sf_ext_access.wrd);

	/* Fill address */
	write_flash_ctrl_reg(FLASH_SF_ADDRESS, offset);

	/* Read out data */
	while (len >= 4) {
		/* Start access */
		write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, SF_START_INDIRECT_ENABLE);

		/* Wait command ready */
		if (wait_sflash_cmd_rdy(SFLASH_CMD_TIMEOUT) != 0) {
			printf("%s fail!\n", __func__);
			return -1;
		}

		reg_data = read_flash_ctrl_reg(FLASH_SF_DATA);
		*buf++ = reg_data & 0xFF;
		*buf++ = (reg_data >> 8) & 0xFF;
		*buf++ = (reg_data >> 16) & 0xFF;
		*buf++ = (reg_data >> 24) & 0xFF;
		len -= 4;
	}

	if (len > 0) {
		/* Start access */
		write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, SF_START_INDIRECT_ENABLE);

		/* Wait command ready */
		if (wait_sflash_cmd_rdy(SFLASH_CMD_TIMEOUT) != 0) {
			printf("%s fail!\n", __func__);
			return -1;
		}

		reg_data = read_flash_ctrl_reg(FLASH_SF_DATA);
	}

	switch (len) {
		case 3:
			*buf++ = reg_data & 0xFF;
			*buf++ = (reg_data >> 8) & 0xFF;
			*buf++ = (reg_data >> 16) & 0xFF;
			break;
		case 2:
			*buf++ = reg_data & 0xFF;
			*buf++ = (reg_data >> 8) & 0xFF;
			break;
		case 1:
			*buf++ = reg_data & 0xFF;
			break;
		case 0:
			break;
		default:
			printf("%s error length %d!\n", __func__, len);
	}

	/* Finish burst read  */
	sf_access.bf.sflashForceBurst = 0;
	sf_access.bf.sflashForceTerm = 1;
	write_flash_ctrl_reg(FLASH_SF_ACCESS, sf_access.wrd);
	return 0;
}

int spinand_read_cache_x2(u32 offset, int len, u8 * buf)
{
	unsigned int reg_data;
	FLASH_SF_ACCESS_t sf_access;
	FLASH_SF_EXT_ACCESS_t sf_ext_access;
	reg_data = 0;

	printf(".");
	write_flash_ctrl_reg(FLASH_TYPE, spinand_type);

	/* Read from cache
	 * Opcode -> 16bits offset addr -> data
	 * 2 bytes address must rely on extend access mothod
	 */
	sf_access.wrd = 0;
	sf_access.bf.sflashAcCode = SF_AC_OPCODE_EXTEND;
	sf_access.bf.sflashForceBurst = 1;	/* Burst mode */
	sf_access.bf.sflashMIO = SF_ACCESS_MIO_DUAL;
	sf_access.bf.sflashMData = 1;
	sf_access.bf.sflashOpCode = CMD_READ_SIDO;
	sf_access.bf.sflashCeAlt = SF_ACCESS_CE_ALT;
	write_flash_ctrl_reg(FLASH_SF_ACCESS, sf_access.wrd);

	sf_ext_access.wrd = 0;
	/* Use command in extend_acc register */
	sf_ext_access.bf.sflashDirRdCmdEn = 1;
	sf_ext_access.bf.sflashOpCode = CMD_READ_SIDO;
	/* 8 dummy bits */
	sf_ext_access.bf.sflashDummyCount = 7;
	/* 2 byte address */
	sf_ext_access.bf.sflashAddrCount = SF_ADDR_COUNT;
	/* 4 bytes in one command request */
	sf_ext_access.bf.sflashDataCount = 3;
	write_flash_ctrl_reg(FLASH_SF_EXT_ACCESS, sf_ext_access.wrd);

	/* Fill address */
	write_flash_ctrl_reg(FLASH_SF_ADDRESS, offset);

	/* Read out data */
	while (len >= 4) {
		/* Start access */
		write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, SF_START_INDIRECT_ENABLE);

		/* Wait command ready */
		if (wait_sflash_cmd_rdy(SFLASH_CMD_TIMEOUT) != 0) {
			printf("%s fail!\n", __func__);
			return -1;
		}

		reg_data = read_flash_ctrl_reg(FLASH_SF_DATA);
		*buf++ = reg_data & 0xFF;
		*buf++ = (reg_data >> 8) & 0xFF;
		*buf++ = (reg_data >> 16) & 0xFF;
		*buf++ = (reg_data >> 24) & 0xFF;
		len -= 4;
	}

	if (len > 0) {
		/* Start access */
		write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, SF_START_INDIRECT_ENABLE);

		/* Wait command ready */
		if (wait_sflash_cmd_rdy(SFLASH_CMD_TIMEOUT) != 0) {
			printf("%s fail!\n", __func__);
			return -1;
		}

		reg_data = read_flash_ctrl_reg(FLASH_SF_DATA);
	}

	switch (len) {
		case 3:
			*buf++ = reg_data & 0xFF;
			*buf++ = (reg_data >> 8) & 0xFF;
			*buf++ = (reg_data >> 16) & 0xFF;
			break;
		case 2:
			*buf++ = reg_data & 0xFF;
			*buf++ = (reg_data >> 8) & 0xFF;
			break;
		case 1:
			*buf++ = reg_data & 0xFF;
			break;
		case 0:
			break;
		default:
			printf("%s error length %d!\n", __func__, len);
	}

	/* Finish burst read  */
	sf_access.bf.sflashForceBurst = 0;
	sf_access.bf.sflashForceTerm = 1;
	write_flash_ctrl_reg(FLASH_SF_ACCESS, sf_access.wrd);

	return 0;
}

int spinand_read_cache_x4(u32 offset, int len, u8 * buf)
{
	unsigned int reg_data;
	FLASH_SF_ACCESS_t sf_access;
	FLASH_SF_EXT_ACCESS_t sf_ext_access;
	reg_data = 0;

	printf(".");
	write_flash_ctrl_reg(FLASH_TYPE, spinand_type);

	/* Read from cache
	 * Opcode -> 16bits offset addr -> data
	 * 2 bytes address must rely on extend access mothod
	 */
	sf_access.wrd = 0;
	sf_access.bf.sflashMIO = SF_ACCESS_MIO_QUARD;
	sf_access.bf.sflashMData = 1;
	sf_access.bf.sflashForceBurst = 1;	/* Burst mode */
	sf_access.bf.sflashAcCode = SF_AC_OPCODE_EXTEND;
	sf_access.bf.sflashOpCode = CMD_READ_SIQO;
	sf_access.bf.sflashCeAlt = SF_ACCESS_CE_ALT;
	write_flash_ctrl_reg(FLASH_SF_ACCESS, sf_access.wrd);

	sf_ext_access.wrd = 0;
	/* Use command in extend_acc register */
	sf_ext_access.bf.sflashDirRdCmdEn = 1;
	sf_ext_access.bf.sflashOpCode = CMD_READ_SIQO;
	/* 8 dummy bits */
	sf_ext_access.bf.sflashDummyCount = 7;
	/* 2 byte address */
	sf_ext_access.bf.sflashAddrCount = SF_ADDR_COUNT;
	/* 4 bytes in one command request */
	sf_ext_access.bf.sflashDataCount = 3;
	write_flash_ctrl_reg(FLASH_SF_EXT_ACCESS, sf_ext_access.wrd);

	/* Fill address */
	write_flash_ctrl_reg(FLASH_SF_ADDRESS, offset);

	/* Read out data */
	while (len >= 4) {
		/* Start access */
		write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, SF_START_INDIRECT_ENABLE);

		/* Wait command ready */
		if (wait_sflash_cmd_rdy(SFLASH_CMD_TIMEOUT) != 0) {
			printf("%s fail!\n", __func__);
			return -1;
		}

		reg_data = read_flash_ctrl_reg(FLASH_SF_DATA);
		*buf++ = reg_data & 0xFF;
		*buf++ = (reg_data >> 8) & 0xFF;
		*buf++ = (reg_data >> 16) & 0xFF;
		*buf++ = (reg_data >> 24) & 0xFF;
		len -= 4;
	}

	if (len > 0) {
		/* Start access */
		write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, SF_START_INDIRECT_ENABLE);

		/* Wait command ready */
		if (wait_sflash_cmd_rdy(SFLASH_CMD_TIMEOUT) != 0) {
			printf("%s fail!\n", __func__);
			return -1;
		}

		reg_data = read_flash_ctrl_reg(FLASH_SF_DATA);
	}

	switch (len) {
		case 3:
			*buf++ = reg_data & 0xFF;
			*buf++ = (reg_data >> 8) & 0xFF;
			*buf++ = (reg_data >> 16) & 0xFF;
			break;
		case 2:
			*buf++ = reg_data & 0xFF;
			*buf++ = (reg_data >> 8) & 0xFF;
			break;
		case 1:
			*buf++ = reg_data & 0xFF;
			break;
		case 0:
			break;
		default:
			printf("%s error length!\n", __func__);
	}

	/* Finish burst read  */
	sf_access.bf.sflashForceBurst = 0;
	sf_access.bf.sflashForceTerm = 1;
	write_flash_ctrl_reg(FLASH_SF_ACCESS, sf_access.wrd);

	return 0;
}

int spinand_read_cache_Dual(u32 offset, int len, u8 * buf)
{
	unsigned int reg_data;
	FLASH_SF_ACCESS_t sf_access;
	FLASH_SF_EXT_ACCESS_t sf_ext_access;
	reg_data = 0;

	printf(".");
	write_flash_ctrl_reg(FLASH_TYPE, spinand_type);

	/* Read from cache
	 * Opcode -> 16bits offset addr -> data
	 * 2 bytes address must rely on extend access mothod
	 */
	sf_access.wrd = 0;
	sf_access.bf.sflashMIO = SF_ACCESS_MIO_DUAL;
	sf_access.bf.sflashMAddr = 1;
	sf_access.bf.sflashMData = 1;
	sf_access.bf.sflashForceBurst = 0;
	sf_access.bf.sflashAcCode = SF_AC_OPCODE_EXTEND;
	sf_access.bf.sflashOpCode = CMD_READ_DUAL_IO;
	sf_access.bf.sflashCeAlt = SF_ACCESS_CE_ALT;
	write_flash_ctrl_reg(FLASH_SF_ACCESS, sf_access.wrd);

	sf_ext_access.wrd = 0;
	/* Use command in extend_acc register */
	sf_ext_access.bf.sflashDirRdCmdEn = 1;
	sf_ext_access.bf.sflashOpCode = CMD_READ_DUAL_IO;
	/* 8 dummy bits */
	sf_ext_access.bf.sflashDummyCount = 3;
	sf_ext_access.bf.sflashAddrCount = SF_ADDR_COUNT;
	/* (len) bytes in one command request */
	sf_ext_access.bf.sflashDataCount = (len - 1);
	write_flash_ctrl_reg(FLASH_SF_EXT_ACCESS, sf_ext_access.wrd);

	/* Fill address */
	write_flash_ctrl_reg(FLASH_SF_ADDRESS, offset);

	/* Read out data */
	while (len >= 4) {
		/* Start access */
		write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, SF_START_INDIRECT_ENABLE);

		/* Wait command ready */
		if (wait_sflash_cmd_rdy(SFLASH_CMD_TIMEOUT) != 0) {
			printf("%s fail!\n", __func__);
			return -1;
		}

		reg_data = read_flash_ctrl_reg(FLASH_SF_DATA);
		*buf++ = reg_data & 0xFF;
		*buf++ = (reg_data >> 8) & 0xFF;
		*buf++ = (reg_data >> 16) & 0xFF;
		*buf++ = (reg_data >> 24) & 0xFF;
		len -= 4;
	}

	if (len > 0) {
		/* Start access */
		write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, SF_START_INDIRECT_ENABLE);

		/* Wait command ready */
		if (wait_sflash_cmd_rdy(SFLASH_CMD_TIMEOUT) != 0) {
			printf("%s fail!\n", __func__);
			return -1;
		}

		reg_data = read_flash_ctrl_reg(FLASH_SF_DATA);
	}

	switch (len) {
		case 3:
			*buf++ = reg_data & 0xFF;
			*buf++ = (reg_data >> 8) & 0xFF;
			*buf++ = (reg_data >> 16) & 0xFF;
			break;
		case 2:
			*buf++ = reg_data & 0xFF;
			*buf++ = (reg_data >> 8) & 0xFF;
			break;
		case 1:
			*buf++ = reg_data & 0xFF;
			break;
		case 0:
			break;
		default:
			printf("%s error length!\n", __func__);
	}

	return 0;
}

int spinand_read_cache_Quad(u32 offset, int len, u8 * buf)
{
	unsigned int reg_data;
	FLASH_SF_ACCESS_t sf_access;
	FLASH_SF_EXT_ACCESS_t sf_ext_access;
	reg_data = 0;

	printf(".");
	write_flash_ctrl_reg(FLASH_TYPE, spinand_type);

	/* Read from cache
	 * Opcode -> 16bits offset addr -> data
	 * 2 bytes address must rely on extend access mothod
	 */
	sf_access.wrd = 0;
	sf_access.bf.sflashMIO = SF_ACCESS_MIO_QUARD;
	sf_access.bf.sflashMAddr = 1;
	sf_access.bf.sflashMData = 1;
	sf_access.bf.sflashForceBurst = 0;
	sf_access.bf.sflashAcCode = SF_AC_OPCODE_EXTEND;
	sf_access.bf.sflashOpCode = CMD_READ_QUAD_IO;
	sf_access.bf.sflashCeAlt = SF_ACCESS_CE_ALT;
	write_flash_ctrl_reg(FLASH_SF_ACCESS, sf_access.wrd);

	sf_ext_access.wrd = 0;
	/* Use command in extend_acc register */
	sf_ext_access.bf.sflashDirRdCmdEn = 1;
	sf_ext_access.bf.sflashOpCode = CMD_READ_QUAD_IO;
	/* 8 dummy bits */
	sf_ext_access.bf.sflashDummyCount = 0x3f;
	//sf_ext_access.bf.sflashAddrCount = SF_ADDR_COUNT;
	sf_ext_access.bf.sflashAddrCount = 0x3; /* Winbond */
	/* (len) bytes in one command request */
	sf_ext_access.bf.sflashDataCount = (len - 1);
	write_flash_ctrl_reg(FLASH_SF_EXT_ACCESS, sf_ext_access.wrd);

	/* Fill address */
	write_flash_ctrl_reg(FLASH_SF_ADDRESS, offset);

	/* Read out data */
	while (len >= 4) {
		/* Start access */
		write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, SF_START_INDIRECT_ENABLE);

		/* Wait command ready */
		if (wait_sflash_cmd_rdy(SFLASH_CMD_TIMEOUT) != 0) {
			printf("%s fail!\n", __func__);
			return -1;
		}

		reg_data = read_flash_ctrl_reg(FLASH_SF_DATA);
		*buf++ = reg_data & 0xFF;
		*buf++ = (reg_data >> 8) & 0xFF;
		*buf++ = (reg_data >> 16) & 0xFF;
		*buf++ = (reg_data >> 24) & 0xFF;
		len -= 4;
	}

	if (len > 0) {
		/* Start access */
		write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, SF_START_INDIRECT_ENABLE);

		/* Wait command ready */
		if (wait_sflash_cmd_rdy(SFLASH_CMD_TIMEOUT) != 0) {
			printf("%s fail!\n", __func__);
			return -1;
		}

		reg_data = read_flash_ctrl_reg(FLASH_SF_DATA);
	}

	switch (len) {
		case 3:
			*buf++ = reg_data & 0xFF;
			*buf++ = (reg_data >> 8) & 0xFF;
			*buf++ = (reg_data >> 16) & 0xFF;
			break;
		case 2:
			*buf++ = reg_data & 0xFF;
			*buf++ = (reg_data >> 8) & 0xFF;
			break;
		case 1:
			*buf++ = reg_data & 0xFF;
			break;
		case 0:
			break;
		default:
			printf("%s error length!\n", __func__);
	}

	return 0;
}

static int spinand_read(struct spi_flash *snandflash,
						u32 offset, size_t len, void *buf)
{
	unsigned int page_id = offset / spinand_page_size;
	unsigned int plane_select;

	/* Handle head non-align part */
	if (offset & spinand_page_sz_mask) {
		printf("%s non-alignment read :0x%x\n", __func__, page_id);
		plane_select = (page_id & PLANE_SELECT) ? PLANE_SELECT_BIT : 0;

		/* Read first page to flash's cache */
		if (spinand_read_page_to_cache(page_id) < 0) {
			printf("%s read to cache fail 1, page:%d\n", __func__, page_id);
			return -1;
		}

		/* Read first page from flash's cache to ddr*/
		READ_FROM_CACHE((offset & spinand_page_sz_mask) | plane_select,
				spinand_page_size - (offset & spinand_page_sz_mask),
				buf);

		len -= spinand_page_size - (offset & spinand_page_sz_mask);
		buf += spinand_page_size - (offset & spinand_page_sz_mask);
		offset += spinand_page_size - (offset & spinand_page_sz_mask);
	}

	while (len >= spinand_page_size) {
		page_id = offset / spinand_page_size;
		plane_select = (page_id & PLANE_SELECT) ? PLANE_SELECT_BIT : 0;

		/* Read continuous pages to flash's cache */
		if (spinand_read_page_to_cache(page_id) < 0) {
			printf("%s read to cache fail 2, page:%d\n", __func__, page_id);
			return -1;
		}

		/* Read continuous pages from flash's cache (page head) to ddr*/
		READ_FROM_CACHE(0 | plane_select, spinand_page_size, buf);
		buf += spinand_page_size;
		offset += spinand_page_size;
		len -= spinand_page_size;
	}

	/* Handle tail non-align part */
	if (len > 0) {
		page_id = offset / spinand_page_size;
		plane_select = (page_id & PLANE_SELECT) ? PLANE_SELECT_BIT : 0;
		/* Read tail data to flash's cache */
		if (spinand_read_page_to_cache(page_id) < 0) {
			printf("%s read to cache fail 3, page:%d\n", __func__, page_id);
			return -1;
		}

		/* Read tail data from flash's cache(page head) to ddr */
		READ_FROM_CACHE(0|plane_select, len, buf);
	}

	printf("\n");

	printf("%s Indirect Mode Read from cache finish!!!\n", __func__);
	return 0;
}

int spinand_write_cache_x1(u32 offset, size_t len, unsigned char *buf)
{
	unsigned int reg_data;
	FLASH_SF_ACCESS_t sf_access;
	FLASH_SF_EXT_ACCESS_t sf_ext_access;
	FLASH_FLASH_ACCESS_START_t access_start;

	printf(".");
	write_flash_ctrl_reg(FLASH_TYPE, spinand_type);

	/* Write to cache
	 * Opcode -> 16bits offset addr -> data ....
	 * 2 bytes address must rely on extend access mothod
	 */
	sf_access.wrd = 0;
	sf_access.bf.sflashOpCode = CMD_PROG_PAGE_X1;
	sf_access.bf.sflashAcCode = SF_AC_OPCODE_EXTEND;
	sf_access.bf.sflashCeAlt = SF_ACCESS_CE_ALT;
	sf_access.bf.sflashForceBurst = 1;	/* Burst mode */
	write_flash_ctrl_reg(FLASH_SF_ACCESS, sf_access.wrd);

	sf_ext_access.wrd = 0;
	/* Use command in extend_acc register */
	sf_ext_access.bf.sflashDirRdCmdEn = 1;
	sf_ext_access.bf.sflashOpCode = CMD_PROG_PAGE_X1;
	/* 0 dummy bits */
	sf_ext_access.bf.sflashDummyCount = -1;
	/* 2 byte address */
	sf_ext_access.bf.sflashAddrCount = SF_ADDR_COUNT;
	/* ECC is handled by device, we just push data  */
	sf_ext_access.bf.sflashDataCount = 3;
	write_flash_ctrl_reg(FLASH_SF_EXT_ACCESS, sf_ext_access.wrd);

	/* Fill address */
	write_flash_ctrl_reg(FLASH_SF_ADDRESS, offset);

	access_start.wrd = 0;
	access_start.bf.sflashRegReq = 1;
	access_start.bf.sflashRegCmd = 1;
	/* Burst mode */
	while (len > 0) {
		reg_data = buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24);
		write_flash_ctrl_reg(FLASH_SF_DATA, reg_data);
		write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, access_start.wrd);

		/* Wait command ready */
		if (wait_sflash_cmd_rdy(SFLASH_CMD_TIMEOUT) != 0) {
			printf("%s timeout\n", __func__);
			return -1;
		}

		len -= 4;
		buf += 4;
	}

	/* Finish burst write  */
	sf_access.bf.sflashForceBurst = 0;
	sf_access.bf.sflashForceTerm = 1;	/* Terminate burst mode */
	write_flash_ctrl_reg(FLASH_SF_ACCESS, sf_access.wrd);

	return 0;
}

int spinand_write_cache_x4(u32 offset, size_t len, unsigned char *buf)
{
	unsigned int reg_data, reg_access_start;
	FLASH_SF_ACCESS_t sf_access;
	FLASH_SF_EXT_ACCESS_t sf_ext_access;
	FLASH_FLASH_ACCESS_START_t access_start;

	printf(".");
	write_flash_ctrl_reg(FLASH_TYPE, spinand_type);

	/* Write to cache
	 * Opcode -> 16bits offset addr -> data ....
	 * 2 bytes address must rely on extend access mothod
	 */
	sf_access.wrd = 0;
	sf_access.bf.sflashMAddr = 1;
	sf_access.bf.sflashMData = 1;
	sf_access.bf.sflashAcCode = SF_AC_OPCODE_EXTEND;
	sf_access.bf.sflashMIO = SF_ACCESS_MIO_QUARD;
	sf_access.bf.sflashForceBurst = 1;	/* Burst mode */
	sf_access.bf.sflashOpCode = CMD_PROG_PAGE_X4;
	sf_access.bf.sflashCeAlt = SF_ACCESS_CE_ALT;
	write_flash_ctrl_reg(FLASH_SF_ACCESS, sf_access.wrd);

	sf_ext_access.wrd = 0;
	/* Use command in extend_acc register */
	sf_ext_access.bf.sflashDirRdCmdEn = 1;
	sf_ext_access.bf.sflashOpCode = CMD_PROG_PAGE_X4;
	/* 0 dummy bits */
	sf_ext_access.bf.sflashDummyCount = -1;
	/* 2 byte address */
	sf_ext_access.bf.sflashAddrCount = SF_ADDR_COUNT;
	/* 4 bytes in one command request */
	sf_ext_access.bf.sflashDataCount = 3;
	write_flash_ctrl_reg(FLASH_SF_EXT_ACCESS, sf_ext_access.wrd);

	/* Fill address */
	write_flash_ctrl_reg(FLASH_SF_ADDRESS, offset);

	access_start.wrd = 0;
	access_start.bf.sflashRegReq = 1;
	access_start.bf.sflashRegCmd = 1;
	reg_access_start = access_start.wrd;
	/* Burst mode */
	while (len > 0) {
		reg_data = buf[0] | buf[1] << 8 | buf[2] << 16 | buf[3] << 24;
		write_flash_ctrl_reg(FLASH_SF_DATA, reg_data);
		write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, reg_access_start);

		/* Wait command ready */
		if (wait_sflash_cmd_rdy(SFLASH_CMD_TIMEOUT) != 0) {
			printf("%s timeout\n", __func__);
			return -1;
		}

		len -= 4;
		buf += 4;
	}

	/* elapse 15us before issuing any other command */
	/* Please do NOT remove!! */
	udelay(15);

	if (wait_till_ready()) {
		printf("%s wait timeout!\n", __func__);
		return -1;
	}

	/* Finish burst write  */
	sf_access.bf.sflashForceBurst = 0;
	sf_access.bf.sflashForceTerm = 1;	/* Terminate burst mode */
	write_flash_ctrl_reg(FLASH_SF_ACCESS, sf_access.wrd);

	return 0;
}

int spinand_write_page(u32 page_id, size_t len, unsigned char *buf)
{
	u32 plane_select;

	plane_select = (page_id & PLANE_SELECT) ? PLANE_SELECT_BIT : 0;


	/* 06h (WRITE ENABLE command)
	 * 02h (PROGRAM LOAD command) or 32h to load x4(multi-IO)
	 * 10h (PROGRAM EXECUTE command)
	 * 0Fh (GET FEATURES command to read the status)
	 */

	/* Enable write */
	spinand_write_enable();

	/* Write data to cache
	 * Could be x1(02h) or x4(32h)
	 */
	WRITE_TO_CACHE(0 | plane_select, len, buf);

	/* Program execute (Serial NAND only) */
	spinand_program_execute(page_id);

	/* Wait until program finish */
	if (wait_till_ready()) {
		printf("%s wait timeout 0x%x!\n", __func__, page_id);
		return -1;
	}

	return 0;
}

static int spinand_write(struct spi_flash *snandflash,
						 u32 offset, size_t len, const void *buf)
{
	unsigned char *src = (unsigned char *)buf;

	/* Sanity check */
	if (offset & spinand_page_sz_mask) {
		printf("Non alignment write(ADDR) not supported!\nThe unit is 8hex!!!\n");
		return -1;
	}

	if (len % spinand_page_size) {
		printf("Non alignment write(SIZE) not supported!\nThe unit is a PAGE!!!\n");
		return -1;
	}

	while (len > 0) {
		spinand_write_page(offset / spinand_page_size, spinand_page_size, src);
		offset += spinand_page_size;
		src += spinand_page_size;
		len -= spinand_page_size;
	}

	printf("\n");

	printf("%s Indirect Mode Write to cache finish!!!\n", __func__);
	return len > 0 ? -1 : 0;
}

/*
 * spinand_erase_block- erase one block
 * Description:
 *    ERASE BLOCK: erase one block at once, each block is 32K byte(0x8000)
 */
int spinand_erase_block(u32 block_addr)
{
	FLASH_SF_ACCESS_t sf_access;

	/* Enable write */
	spinand_write_enable();

	write_flash_ctrl_reg(FLASH_TYPE, spinand_type);

	/* Erase block
	 * Opcode -> 24bits Addr
	 */
	sf_access.wrd = 0;
	sf_access.bf.sflashOpCode = CMD_ERASE_BLK_64KB;
	sf_access.bf.sflashAcCode = SF_AC_OPCODE_3_ADDR;
	sf_access.bf.sflashCeAlt = SF_ACCESS_CE_ALT;
	write_flash_ctrl_reg(FLASH_SF_ACCESS, sf_access.wrd);
	write_flash_ctrl_reg(FLASH_SF_ADDRESS, block_addr);
	write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, SF_START_INDIRECT_ENABLE);

	/* Wait command ready */
	if (wait_sflash_cmd_rdy(SFLASH_CMD_TIMEOUT) != 0) {
		printf("%s fail!\n", __func__);
		return -1;
	}

	/* elapse 1ms before issuing any other command */
	mdelay(500);

	if (wait_till_ready()) {
		printf("%s wait timeout %d!\n", __func__, block_addr);
		return -1;
	}

	printf("spinand_erase_block done!!!\n");
	return 0;
}

int spinand_erase(struct spi_flash *snandflash, u32 offset, size_t len)
{
	unsigned int block_id;

	while (len > 0) {
		block_id = offset / spinand_blk_size;
		if (spinand_erase_block((block_id * spinand_blk_size)))
			break;
		len -= spinand_blk_size;
		offset += spinand_blk_size;
	}

	return len > 0 ? -1 : 0;
}

void set_spinand_clk(void)
{
	FLASH_SF_TIMING_t sf_timing;
	sf_timing.wrd = 0;
	sf_timing.bf.sflashIdleTime = 0x14;    /* 20 clk */
	sf_timing.bf.sflashHoldTime = 0x14;    /* 20 clk */
	sf_timing.bf.sflashSetupTime = 0x14;   /* 20 clk */
	sf_timing.bf.sflashClkWidth = 0x07;       /* 20M Hz */
	/* sf_timing.wrd = 0x07141414 */
	write_flash_ctrl_reg(FLASH_SF_TIMING, sf_timing.wrd);
}

struct spi_flash *spinand_flash_probe(unsigned int bus, unsigned int cs,
								  unsigned int max_hz, unsigned int spi_mode)
{
        FLASH_TYPE_t flash_type;
	GLOBAL_STRAP_CPU_WDATA_t global_strap_cpu_wdata;
	u8 chip_id[3] = { 0, 0, 0 }, tmp_var;
	u32 addr;

        /* set decode to general flash controller logic */
        global_strap_cpu_wdata.wrd = read_flash_ctrl_reg(GLOBAL_STRAP_CPU_WDATA);
        flash_type.wrd = 0;
        flash_type.bf.flashWidth = global_strap_cpu_wdata.bf.flash_width;
        flash_type.bf.flashSize = global_strap_cpu_wdata.bf.flash_size;
        flash_type.bf.flashPin = global_strap_cpu_wdata.bf.flash_pin;
        flash_type.bf.flashType = global_strap_cpu_wdata.bf.flash_type;
        spinand_type = flash_type.wrd;
        write_flash_ctrl_reg(FLASH_TYPE, spinand_type);

	/* Set flash timing */
	set_spinand_clk();

	if (spinand_reset() < 0)
		printf("Reset fail\n");

	snandflash = malloc(sizeof(struct spi_flash));
	if (!snandflash) {
		printf("SF: Failed to allocate memory\n");
		return NULL;
	}

	spinand_read_id(chip_id);

	spinand_read_bp_otp_status_reg(&tmp_var, REG_BLOCK_LOCK);
	printf("BP status:0x%x\n", tmp_var);

	/* Unprotect */
	spinand_set_feature_reg(REG_BLOCK_LOCK, tmp_var & ~0x78);
	printf("Unprotect: spinand_write_status, addr = 0x%x, var = 0x%x\n",REG_BLOCK_LOCK, (tmp_var & ~0x78));

#if OTP_QE_OFF
	/* Enable Quad IO */
	spinand_set_feature_reg(REG_OTP, tmp_var | OTP_QE_MASK);
	printf("Enable Quad IO: spinand_write_status, addr = 0x%x, var = 0x%x\n",REG_OTP, (tmp_var | OTP_QE_MASK));
#endif
	/* Read back to verify */
	spinand_read_bp_otp_status_reg(&tmp_var, REG_OTP);
	printf("OTP Status:0x%x\n", tmp_var);

	snandflash->spi = NULL;
	/* TBD: Try ONFI command to get detail device parameters */

	snandflash->size = sflash_size;
	snandflash->write = spinand_write;
	snandflash->erase = spinand_erase;
	snandflash->read = spinand_read;

	printf("Scaning Bad block...\n");
	for (addr = 0; addr < spinand_dev_size; addr += spinand_blk_size)
		spinand_read_oob(snandflash, addr, oob_buff);
	printf("\n");

	return snandflash;
}

void spinand_flash_free(struct spi_flash *snandflash)
{
	free(snandflash);
}

/* Flysky add for socCmd test item*/
void rtk_spi_nand_test(IN  void *Data)
{
//	struct spi_flash *spiFlash;
	u32*  InPutData;
	InPutData = (u32*)Data;
	u8 chip_id[3] = { 0, 0, 0 };

        //flash->temp_buf = temp_buf;

	switch(InPutData[0])
	{
		case 0:
			//CAUTION :  init memory and parameter , make sure this command have been excuted before any others
			//socCmd spinand 0
			//flash = spinand_flash_probe(InPutData[1], InPutData[2], InPutData[3], InPutData[4]);
			snandflash = spinand_flash_probe(0, 0, 0, 0);
			break;
		case 1:
			//TODO: READ ID (Read device ID) RDID
			//socCmd spinand 1
			spinand_read_id(chip_id);
			break;
		case 2:
			/**********************************************************************/
			/* RTL8198F SPI_NAND flash controller does NOT support DIRECT MODE yet */
			/**********************************************************************/
			/* DIRECT MODE(AUTO MODE) */
			printf("DIRECT MODE not available\n");
			break;
		case 3:
			/* INDIRECT MODE(USER MODE) */
			switch(InPutData[1])
			{
				case 0:
					/* INDIRECT MODE(USER MODE): READ */
					switch(InPutData[2])
					{
						case 0:
							/* INDIRECT MODE: PAGE READ (Array read), PRD */
							/* socCmd 3 0 0 offset len buf*/
							spinand_read(snandflash, InPutData[3], InPutData[4], InPutData[5]);
							break;
						case 1:
							/* INDIRECT MODE: READ SINGLE IN SINGLE OUT (Output cache data at column address) READ */
							/* socCmd 3 0 1 offset len buf */
							spinand_read_cache_x1(InPutData[3], InPutData[4], InPutData[5]);
							break;
						case 2:
							/* INDIRECT MODE: READ SINGLE IN DUAL OUT (Output cache data on IO[1:0]) 2READ */
							/* socCmd 3 0 2 offset len buf */
							spinand_read_cache_x2(InPutData[3], InPutData[4], InPutData[5]);
							break;
						case 3:
							/* INDIRECT MODE: READ SINGLE IN QUAD OUT (Output cache data on IO[3:0]) 4READ */
							/* socCmd 3 0 3 offset len buf */
							spinand_read_cache_x4(InPutData[3], InPutData[4], InPutData[5]);
							break;
                                                case 4:
                                                        /* INDIRECT MODE: READ FROM CACHE Dual IO (Input address/Output cache data on IO[1:0]) DREAD */
                                                        /* socCmd 3 0 4 offset len buf */
							spinand_read_cache_Dual(InPutData[3], InPutData[4], InPutData[5]);
                                                        break;
                                                case 5:
                                                        /* INDIRECT MODE: READ FROM CACHE Quad IO (Input address/Output cache data on IO[3:0]) QREAD */
                                                        /* socCmd 3 0 5 offset len buf */
                                                        spinand_read_cache_Quad(InPutData[3], InPutData[4], InPutData[5]);
                                                        break;
						case 6:
							//INDIRECT MODE: PAGE READ FROM CACHE
							spinand_read_page_to_cache(InPutData[3]);
							break;
					}
					break;
				case 1:
					/* INDIRECT MODE(USER MODE): WRITE */
					switch(InPutData[2])
					{
						case 0:
							/* INDIRECT MODE: PAGE WRITE (Array write) PW */
							/* socCmd 3 1 0 offset len buf */
							spinand_write(snandflash, InPutData[3], InPutData[4], InPutData[5]);
							break;
						case 1:
							/* INDIRECT MODE: WRITE PAGE x1 (Input cache data at column adress) */
							/* socCmd 3 1 1 offset len buf */
							spinand_write_cache_x1(InPutData[3], InPutData[4], InPutData[5]);
							break;
						case 2:
							/* NOT AVAILABLE: WRITE PAGE PROGRAM x2 (Input cache data on IO[1:0]) */
							printf("WRITE PAGE PROGRAM x2 not available\n");
							break;
						case 3:
							/* INDIRECT MODE: WRITE PAGE PROGRAM x4 (Input cache data on IO[3:0]) */
							/* socCmd 3 1 3 offset len buf */
							spinand_write_cache_x4(InPutData[3], InPutData[4], InPutData[5]);
							break;
						case 4:
							/* INDIRECT MODE: PROGRAM EXECUTE */
							/* socCmd 3 1 4 offset*/
							spinand_program_execute(InPutData[3]);
							break;
					}
					break;
			}
			break;
		case 4:
			/* DMA MODE */
			switch(InPutData[1])
			{
				case 0:
					/* DMA MODE: READ */
					switch(InPutData[2])
					{
						case 0:
							/* DMA: PAGE READ (Array read) PRD */
							/* socCmd 4 0 0 offset len buf */
							spinand_dma_read(InPutData[3], InPutData[4], InPutData[5]);
							break;
						case 1:
							/* DMA: READ SINGLE IN SINGLE OUT (Output cache data at column address) READ */
							spinand_dma_read_x1(InPutData[3], InPutData[4], InPutData[5]);
							break;
						case 2:
							/* TODO: DMA: READ SINGLE IN DUAL OUT (Output cache data on IO[1:0]) DREAD */
							//spinand_read_cache_x2(InPutData[3], InPutData[4], InPutData[5]);
							printf("DMA READ x2 not available\n");
							break;
						case 3:
							/* TODO: DMA: READ SINGLE IN QUAD OUT (Output cache data on IO[3:0]) QREAD */
							//spinand_read_cache_x4(InPutData[3], InPutData[4], InPutData[5]);
							printf("DMA READ x4 not available\n");
							break;
					}
					break;
				case 1:
					//TODO: DMA MODE: WRITE
					switch(InPutData[2])
					{
						case 0:
							/* DMA: PAGE WRITE (Array write) PWT */
							spinand_dma_write(InPutData[3], InPutData[4], InPutData[5]);
							break;
						case 1:
							/* DMA: WRITE PAGE (Input cache data at column adress) */
							spinand_dma_write_x1(InPutData[3], InPutData[4], InPutData[5]);
							break;
						case 2:
							/* NOT AVAILABLE: WRITE PAGE x2 (Input cache data on IO[1:0]) */
							printf("DMA WRITE PAGE PROGRAM x2 not available\n");
							break;
						case 3:
							/* TODO: DMA: WRITE PAGE x4 (Input cache data on IO[3:0]) */
							//spinand_dma_write_x4(InPutData[3], InPutData[4], InPutData[5]);
							printf("DMA WRITE PAGE PROGRAM x4 not available\n");
							break;
					}
					break;
			}
			break;
		case 5:
			/* RESET CONTROLLER */
			spinand_reset_controller();
			/* RESET FLASH MEMORY */
			spinand_reset_memory();
			/* RESET FLASH */
			spinand_reset();
			break;
		case 6:
			/* ERASE CHIP */
			/* spinand_erase_chip(); */
			printf("ERASE CHIP not available\n");
			break;
		case 7:
			/* ERASE */
			/* socCmd 7 offset len */
			spinand_erase(snandflash, InPutData[1], InPutData[2]);
			break;
		case 8:
			/* SECTOR ERASE */
			printf("SECTOR ERASE not available\n");
			break;
		case 9:
			/* BLOCK ERASE */
			/* socCmd 9 blk_addr */
			spinand_erase_block((InPutData[1] * spinand_blk_size));
			break;
		case 10:
			/* GET FEATURES */
			/* spinand_get_feature_reg(addr) */
			spinand_get_feature_reg(InPutData[1]);
			break;
		case 11:
			/* SET FEATURES */
			/* spinand_set_feature_reg(addr, value) */
			spinand_set_feature_reg(InPutData[1], InPutData[2]);
			break;
		case 12:
			/* WRITE ENABLE */
			spinand_write_enable();
			break;
		case 13:
			/* WRITE DISABLE */
			spinand_write_disable();
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
			/* spinand_enter_4byte_mode */
			printf("ENTER 4BYTE MODE not available\n");
			break;
		case 17:
			/* EXIT 4BYTE MODE */
			/* spinand_exit_4byte_mode(); */
			printf("EXIT 4BYTE MODE not available\n");
			break;
		case 18:
			/* COMPARE MEM */
			/* socCmd 18 addr1 addr2 len */
			spinand_mem_cmp(InPutData[1], InPutData[2], InPutData[3]);
			break;
	}
}
