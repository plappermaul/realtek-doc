/*
 * Copyright (c) 2017 Cortina Access. All rights reserved.
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
 */
#include <common.h>
#include <asm/io.h>
#include <malloc.h>
#include "cortina_spi_nand.h"


/* SF controller relative definition */
#define SF_AC_OPCODE                	0x00
#define SF_AC_OPCODE_1_DATA           	0x01
#define SF_AC_OPCODE_2_DATA           	0x02
#define SF_AC_OPCODE_3_DATA           	0x03
#define SF_AC_OPCODE_4_DATA           	0x04
#define SF_AC_OPCODE_3_ADDR         	0x05
#define SF_AC_OPCODE_3_ADDR_1_DATA    	0x06
#define SF_AC_OPCODE_3_ADDR_2_DATA    	0x07
#define SF_AC_OPCODE_3_ADDR_3_DATA    	0x08
#define SF_AC_OPCODE_3_ADDR_4_DATA    	0x09
#define SF_AC_OPCODE_3_ADDR_X_1_DATA   	0x0A
#define SF_AC_OPCODE_3_ADDR_X_2_DATA   	0x0B
#define SF_AC_OPCODE_3_ADDR_X_3_DATA   	0x0C
#define SF_AC_OPCODE_3_ADDR_X_4_DATA   	0x0D
#define SF_AC_OPCODE_3_ADDR_4X_1_DATA  	0x0E
#define SF_AC_OPCODE_EXTEND				0x0F
#define SF_ACCESS_MIO_SINGLE	0
#define SF_ACCESS_MIO_DUAL		1
#define SF_ACCESS_MIO_QUARD		2

#define SF_START_BIT_ENABLE		0x0002
#define SFLASH_CMD_TIMEOUT		40

#if defined(CONFIG_TARGET_RTK_TAURUS) || defined(CONFIG_TARGET_RTK_ELNATH)
#define SF_WAIT_CTRL_RDY		((1<<16)|0x0002)
#elif defined(CONFIG_TARGET_VENUS)
#define SF_WAIT_CTRL_RDY		SF_START_BIT_ENABLE
#endif

#define GET_UP8BITS(x)	((x >> 0) & 0xff)

#if defined(CONFIG_ARM64)
#define FAST_IO_BASE	0x400000000
#else
/* armv7's mmu only mapping 4GB range with u-boot, so can't support addr over 4GB */
#define FAST_IO_BASE	0x00000000
#endif

static void write_flash_ctrl_reg(unsigned int ofs, unsigned int data)
{
    writel(data, FAST_IO_BASE | ofs);
    return ;
#if 0
    /* CONFIG_CORTINA_FLASH_CTRL_BASE is defined in the platform's
         * config header. This address may differ from FLASH_ID in that
         * the platform may need to use a logical, ATU, virtual tranlated
         * address. Therefore, we need to use base + offset addressing
         */
        unsigned int base = CONFIG_CORTINA_FLASH_CTRL_BASE;
        unsigned int offset = ofs - FLASH_ID;
        unsigned int * addr;
        addr = (unsigned int*) (base + offset);

        *addr = data;
#endif
}

static unsigned int read_flash_ctrl_reg(unsigned int ofs)
{
    return readl(ofs | FAST_IO_BASE);
#if 0
        /* CONFIG_CORTINA_FLASH_CTRL_BASE is defined in the platform's
         * config header. This address may differ from FLASH_ID in that
         * the platform may need to use a logical, ATU, virtual tranlated
         * address. Therefore, we need to use base + offset addressing
         */
        unsigned int base = CONFIG_CORTINA_FLASH_CTRL_BASE;
        unsigned int offset = ofs - FLASH_ID;
        unsigned int * addr;
        addr = (unsigned int*) (base + offset);

        return (*addr);
#endif
}

static int wait_flash_cmd_rdy(int timeout)
{
	int tmp;
	unsigned long timebase;

	timebase = get_timer(0);
	do {
		tmp = read_flash_ctrl_reg(FLASH_FLASH_ACCESS_START);
		if ((tmp & SF_WAIT_CTRL_RDY) == 0) {
			return 0;
		}
	} while (get_timer(timebase) < timeout);
	return -1;
}


#ifdef CONFIG_RTK_V7_FPGA_PLATFORM
void fpga_pull_hold_high(void)
{
    unsigned int reg_d=0;

	/* Workaround to fix level shift issue
	 * IO3(HOLD) is stick low by level shift IC.
	 * We have to pull high before identification.
	 */
	/* Mux to GPIO */
	reg_d = readl(GLOBAL_GPIO_MUX_1);
	reg_d |= 0x8;
	writel(reg_d, GLOBAL_GPIO_MUX_1);

    reg_d = readl(PER_GPIO1_CFG);
    reg_d &= ~0x8;
    writel(reg_d, PER_GPIO1_CFG);

    reg_d = readl(PER_GPIO1_OUT);
    reg_d |= 0x8;
    writel(reg_d, PER_GPIO1_OUT);

	/* Mux to Non-GPIO */
	reg_d = readl(GLOBAL_GPIO_MUX_1);
	reg_d &= ~0x8;
	writel(reg_d, GLOBAL_GPIO_MUX_1);
}
#endif

/*
 * spi_nand_read_status- send command 0xf to the SPI Nand status register
 * Description:
 *    After read, write, or erase, the Nand device is expected to set the
 *    busy status.
 *    This function is to allow reading the status of the command: read,
 *    write, and erase.
 *    Once the status turns to be ready, the other status bits also are
 *    valid status bits.
 */
static int spi_nand_read_status(uint8_t * status)
{

	unsigned int reg_data;
	FLASH_SF_ACCESS_t sf_access;
	FLASH_SF_EXT_ACCESS_t sf_ext_access;
	sf_access.wrd = 0;
	sf_ext_access.wrd = 0;

#ifdef CONFIG_RTK_V7_FPGA_PLATFORM
    fpga_pull_hold_high();
#endif

	/* Get feature use 1 byte address as sub-command.
	 * We must use extend access code to meet this protocol.
	 */
	sf_access.bf.sflashAcCode = SF_AC_OPCODE_EXTEND;
	write_flash_ctrl_reg(FLASH_SF_ACCESS, sf_access.wrd);

	/* Use command in extend_acc register */
	sf_ext_access.bf.sflashDirRdCmdEn = 1;
	sf_ext_access.bf.sflashOpCode = SPI_NAND_CMD_GET_FEATURE;
	/* No dummy cnt */
	sf_ext_access.bf.sflashDummyCount = -1;
	/* 1 byte address */
	sf_ext_access.bf.sflashAddrCount = 0;
	/* 1 byte data */
	sf_ext_access.bf.sflashDataCount = 0;
	write_flash_ctrl_reg(FLASH_SF_EXT_ACCESS, sf_ext_access.wrd);

	/* Command to get status */
	write_flash_ctrl_reg(FLASH_SF_ADDRESS, REG_STATUS);

	/* Issue command */
	write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, SF_START_BIT_ENABLE);

	/* Wait command ready */
	if (wait_flash_cmd_rdy(SFLASH_CMD_TIMEOUT) != 0)
		return -1;

	/* Read out status */
	reg_data = read_flash_ctrl_reg(FLASH_SF_DATA);
	*status = reg_data & 0xFF;

	return 0;
}

/* wait_till_ready- Check device status
 * Description:
 *    Read device status register until timeout or OIP cleared.
 *
 */
#define MAX_WAIT_MS  (800)		/* TOSHIBA take longer time on erase */
static int wait_till_ready(void)
{
	int retval;
	u8 stat = 0;
	unsigned long timebase;

	timebase = get_timer(0);
	do {
		retval = spi_nand_read_status(&stat);
		if (retval < 0)
			return -1;
		else if ((stat & (STATUS_E_FAIL_MASK | STATUS_P_FAIL_MASK)))
			break;
		else if (!(stat & STATUS_BUSY))
			break;
	} while (get_timer(timebase) < MAX_WAIT_MS);

	if (stat & (STATUS_E_FAIL_MASK | STATUS_P_FAIL_MASK)) {
		debug("Staus[0x%x] error!\n", stat);
		return -1;
	}

	if ((stat & STATUS_BUSY) == 0)
		return 0;

	return -1;
}

int ca_spinand_pio_raw_cmd(struct spi_nand_cmd *cmd)
{
	u8 *value = cmd->rx_buf;
	unsigned int reg_data;
	FLASH_SF_ACCESS_t sf_access;
	FLASH_SF_EXT_ACCESS_t sf_ext_access;
#if defined(CONFIG_TARGET_RTK_TAURUS) || defined(CONFIG_TARGET_RTK_ELNATH)
    FLASH_FLASH_ACCESS_START_t access_start;
    access_start.wrd = 0;
#endif

#ifdef CONFIG_RTK_V7_FPGA_PLATFORM
    fpga_pull_hold_high();
#endif

	sf_access.wrd = 0;
	sf_ext_access.wrd = 0;

	/* Set feature use 1 byte address as sub-command.
	 * We must use extend access code to meet this protocol.
	 */
	sf_access.bf.sflashAcCode = GET_UP8BITS(SF_AC_OPCODE_EXTEND);
	write_flash_ctrl_reg(FLASH_SF_ACCESS, sf_access.wrd);

	/* Use command in extend_acc register */
	sf_ext_access.bf.sflashDirRdCmdEn = 1;
	sf_ext_access.bf.sflashOpCode = cmd->cmd;
	/* n bits dummy cnt */
	sf_ext_access.bf.sflashDummyCount = cmd->n_dummy_cycle - 1;
	/* n byte address */
	sf_ext_access.bf.sflashAddrCount = cmd->n_addr - 1;
	/* n byte data */
    if(cmd->n_tx !=0){
	sf_ext_access.bf.sflashDataCount = cmd->n_tx - 1;
    }else if(cmd->n_rx !=0){
	sf_ext_access.bf.sflashDataCount = cmd->n_rx - 1;
    }else{
	sf_ext_access.bf.sflashDataCount = -1;
    }
	write_flash_ctrl_reg(FLASH_SF_EXT_ACCESS, sf_ext_access.wrd);

    u32 i;
    for(i=0;i<cmd->n_addr;i++){
        write_flash_ctrl_reg(FLASH_SF_ADDRESS, cmd->addr[i]);
    }

    if(cmd->n_tx){
        write_flash_ctrl_reg(FLASH_SF_DATA, *cmd->tx_buf);
    }

	/* Issue command */
#if defined(CONFIG_TARGET_RTK_TAURUS) || defined(CONFIG_TARGET_RTK_ELNATH)
    if(cmd->n_tx){
        access_start.bf.sflashRegCmd = 1;
    }
    access_start.bf.sflashRegReq = 1;
    write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, access_start.wrd);
#elif defined(CONFIG_TARGET_VENUS)
    write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, SF_START_BIT_ENABLE);
#endif

	/* Wait command ready */
	if (wait_flash_cmd_rdy(SFLASH_CMD_TIMEOUT) != 0){
		printf("EE: %s [wait_flash_cmd_rdy] timeout!\n", __func__);
		return -1;
    }
	if (wait_till_ready()) {
		printf("EE: %s [wait_till_ready] timeout!\n", __func__);
		return -1;
	}
    if(cmd->n_rx){
    	/* Read out status */
    	reg_data = read_flash_ctrl_reg(FLASH_SF_DATA);
    	*value = reg_data & 0xFF;
    }
	return 0;
}

/*
 * ca_spinand_get_feature - send command 0xf with with address <addr> to get register value
 * Description:
 */
static int ca_spinand_get_feature(struct spi_nand_cmd *cmd)
{
	uint8_t addr = cmd->addr[0];
	u8 *value = cmd->rx_buf;
	unsigned int reg_data;
	FLASH_SF_ACCESS_t sf_access;
	FLASH_SF_EXT_ACCESS_t sf_ext_access;

#ifdef CONFIG_RTK_V7_FPGA_PLATFORM
    fpga_pull_hold_high();
#endif

	sf_access.wrd = 0;
	sf_ext_access.wrd = 0;

	/* Get feature use 1 byte address as sub-command.
	 * We must use extend access code to meet this protocol.
	 */
	sf_access.bf.sflashAcCode = GET_UP8BITS(SF_AC_OPCODE_EXTEND);
	write_flash_ctrl_reg(FLASH_SF_ACCESS, sf_access.wrd);

	/* Use command in extend_acc register */
	sf_ext_access.bf.sflashDirRdCmdEn = 1;
	sf_ext_access.bf.sflashOpCode = SPI_NAND_CMD_GET_FEATURE;
	/* No dummy cnt */
	sf_ext_access.bf.sflashDummyCount = -1;/* (cmd_cfg->dummy_bytes * 8) - 1; */
	/* 1 byte address */
	sf_ext_access.bf.sflashAddrCount = 0;/* cmd_cfg->addr_bytes - 1; */
	/* 1 byte data */
	sf_ext_access.bf.sflashDataCount = 0;/* cmd_cfg->data_bits - 1; */
	write_flash_ctrl_reg(FLASH_SF_EXT_ACCESS, sf_ext_access.wrd);

	/* Command to get status */
	write_flash_ctrl_reg(FLASH_SF_ADDRESS, addr);

	/* Issue command */
	write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, SF_START_BIT_ENABLE);

	/* Wait command ready */
	if (wait_flash_cmd_rdy(SFLASH_CMD_TIMEOUT) != 0)
		return -1;

	/* Read out status */
	reg_data = read_flash_ctrl_reg(FLASH_SF_DATA);
	*value = reg_data & 0xFF;

	return 0;
}

/*
 * ca_spinand_set_feature - send command 0x1f with address <addr> to set register value
 * Description:
 */
static int ca_spinand_set_feature(struct spi_nand_cmd *cmd)
{
	uint8_t addr = cmd->addr[0];
	const u8 *value = cmd->tx_buf;
	FLASH_SF_ACCESS_t sf_access;
	FLASH_SF_EXT_ACCESS_t sf_ext_access;
#if defined(CONFIG_TARGET_RTK_TAURUS) || defined(CONFIG_TARGET_RTK_ELNATH)
    FLASH_FLASH_ACCESS_START_t access_start;
    access_start.wrd = 0;
#endif
    sf_access.wrd = 0;
    sf_ext_access.wrd = 0;

#ifdef CONFIG_RTK_V7_FPGA_PLATFORM
    fpga_pull_hold_high();
#endif

	/* Set feature use 1 byte address as sub-command.
	 * We must use extend access code to meet this protocol.
	 */
	sf_access.bf.sflashAcCode = GET_UP8BITS(SF_AC_OPCODE_EXTEND);
	write_flash_ctrl_reg(FLASH_SF_ACCESS, sf_access.wrd);

	/* Use command in extend_acc register */
	sf_ext_access.bf.sflashDirRdCmdEn = 1;
	sf_ext_access.bf.sflashOpCode = SPI_NAND_CMD_SET_FEATURE;
	/* No dummy cnt */
	sf_ext_access.bf.sflashDummyCount = -1;/* (cmd_cfg->dummy_bytes * 8) - 1; */
#if defined(CONFIG_TARGET_RTK_TAURUS) || defined(CONFIG_TARGET_RTK_ELNATH)
	/* cmd->n_addr byte address */
	sf_ext_access.bf.sflashAddrCount = cmd->n_addr - 1;
	/* cmd->n_tx byte data */
	sf_ext_access.bf.sflashDataCount = cmd->n_tx - 1;
#elif defined(CONFIG_TARGET_VENUS)
	/* 1 byte address */
	sf_ext_access.bf.sflashAddrCount = 0;/* cmd_cfg->addr_bytes - 1; */
	/* 1 byte data */
	sf_ext_access.bf.sflashDataCount = 0;/* cmd_cfg->data_bits - 1; */
#endif
	write_flash_ctrl_reg(FLASH_SF_EXT_ACCESS, sf_ext_access.wrd);

	/* Command to get status */
	write_flash_ctrl_reg(FLASH_SF_ADDRESS, addr);

	/* Data register */
	write_flash_ctrl_reg(FLASH_SF_DATA, *value);

	/* Issue command */
#if defined(CONFIG_TARGET_RTK_TAURUS) || defined(CONFIG_TARGET_RTK_ELNATH)
    if(cmd->n_tx != 0){
        access_start.bf.sflashRegCmd = 1;
    }
	access_start.bf.sflashRegReq = 1;
	write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, access_start.wrd);
#elif defined(CONFIG_TARGET_VENUS)
	write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, SF_START_BIT_ENABLE);
#endif
	/* Wait command ready */
	if (wait_flash_cmd_rdy(SFLASH_CMD_TIMEOUT) != 0)
		return -1;

	return 0;
}

/*
 * ca_spinand_write_enable- Enable write operation(erase,program)
 * Description:
 *    Enable write operation
 */
static int ca_spinand_write_enable(struct spi_nand_cmd *cmd)
{
	FLASH_SF_ACCESS_t sf_access;

#ifdef CONFIG_RTK_V7_FPGA_PLATFORM
    fpga_pull_hold_high();
#endif

	sf_access.wrd = 0;

	/* Write enable
	 * Opcode only protocol
	 */
	sf_access.bf.sflashOpCode = SPI_NAND_CMD_WR_ENABLE;
	sf_access.bf.sflashAcCode = GET_UP8BITS(SF_AC_OPCODE);
	write_flash_ctrl_reg(FLASH_SF_ACCESS, sf_access.wrd);
	write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, SF_START_BIT_ENABLE);

	/* Wait command ready */
	if (wait_flash_cmd_rdy(SFLASH_CMD_TIMEOUT) != 0)
		printf("%s: command timeout!\n", __func__);

	return 0;
}

/*
 * ca_spi_nand_reset - Reset SPI Nand
 * Description:
 *     Reset: reset SPI Nand device
 */
static int ca_spinand_reset(struct spi_nand_cmd *cmd)
{
    FLASH_SF_ACCESS_t sf_access;

#ifdef CONFIG_RTK_V7_FPGA_PLATFORM
    fpga_pull_hold_high();
#endif

    sf_access.wrd = 0;

	/* Reset SPI-NAND
	 * Opcode only protocol
	 */
	sf_access.bf.sflashOpCode = SPI_NAND_CMD_RESET;
	sf_access.bf.sflashAcCode = GET_UP8BITS(SF_AC_OPCODE);

	write_flash_ctrl_reg(FLASH_SF_ACCESS, sf_access.wrd);
	write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, SF_START_BIT_ENABLE);

    /* Wait command ready */
    if (wait_flash_cmd_rdy(SFLASH_CMD_TIMEOUT) != 0) {
		printf("%s fail!\n", __func__);
		return -1;
    }

    /* elapse 2ms for reset timing */
    udelay(2000);

    if (wait_till_ready()){
		printf("EE: %s [wait_till_ready] timeout!\n", __func__);
        return -1;
    }
    return 0;
}

/*
 * ca_spinand_read_id- Read SPI Nand ID
 * Description:
 *    Read ID: read two ID bytes from the SPI Nand device
 */
static int ca_spinand_read_id(struct spi_nand_cmd *cmd)
{
	u8 *id = cmd->rx_buf;
	unsigned int reg_data;
	FLASH_SF_ACCESS_t sf_access;
	FLASH_SF_EXT_ACCESS_t sf_ext_access;

#ifdef CONFIG_RTK_V7_FPGA_PLATFORM
    fpga_pull_hold_high();
#endif

	sf_access.wrd = 0;

	/* Read ID
	 * Micron/Macronix:
	 *  Opcode -> 8 dummy bits -> MAF_ID => DEV_ID
	 * GigaDevice:
	 *  Opcode -> MAF_ID => DEV_ID
	 */
	sf_access.bf.sflashAcCode = GET_UP8BITS(SF_AC_OPCODE_EXTEND);
	write_flash_ctrl_reg(FLASH_SF_ACCESS, sf_access.wrd);

	/* Use command in extend_acc register */
	sf_ext_access.bf.sflashDirRdCmdEn = 1;
	sf_ext_access.bf.sflashOpCode = SPI_NAND_CMD_READ_ID;
	/* 0 dummy bits */
	sf_ext_access.bf.sflashDummyCount = cmd->n_dummy_cycle-1;
	/* 0 address byte */
	sf_ext_access.bf.sflashAddrCount = cmd->n_addr-1;
	/* 3 of data length */
	sf_ext_access.bf.sflashDataCount = cmd->n_rx-1;
	write_flash_ctrl_reg(FLASH_SF_EXT_ACCESS, sf_ext_access.wrd);

    if(cmd->n_addr!=0){
        write_flash_ctrl_reg(FLASH_SF_ADDRESS, cmd->addr[0]);
    }

	/* Start request */
	write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, SF_START_BIT_ENABLE);

	if (wait_flash_cmd_rdy(SFLASH_CMD_TIMEOUT) != 0) {
		id[0] = id[1] = 0;
		return -1;
	}

	reg_data = read_flash_ctrl_reg(FLASH_SF_DATA);
    id[0] = reg_data & 0x000000ff;
    id[1] = (reg_data & 0x0000ff00) >> 8;
    id[2] = (reg_data & 0x00ff0000) >> 16;

	return 0;
}

static int ca_spinand_read_page_to_cache(struct spi_nand_cmd *cmd)
{
	u32 page_id = (u32)(cmd->addr[0] << 16) | (u32)(cmd->addr[1] << 8)
		| (u32)(cmd->addr[2]);
	FLASH_SF_ACCESS_t sf_access;
	FLASH_SF_EXT_ACCESS_t sf_ext_access;

#ifdef CONFIG_RTK_V7_FPGA_PLATFORM
    fpga_pull_hold_high();
#endif

	sf_access.wrd = 0;
	sf_ext_access.wrd = 0;

	/* Read to cache
	 * Opcode -> 24bits offset addr => Get feature ....
	 * 3 bytes address must rely on extend access mothod
	 */
	sf_access.bf.sflashAcCode = GET_UP8BITS(SF_AC_OPCODE_EXTEND);
	sf_access.bf.sflashMIO = SF_ACCESS_MIO_SINGLE;
	write_flash_ctrl_reg(FLASH_SF_ACCESS, sf_access.wrd);

	/* Use command in extend_acc register */
	sf_ext_access.bf.sflashDirRdCmdEn = 1;
	sf_ext_access.bf.sflashOpCode = SPI_NAND_CMD_PAGE_READ;
	/* 0 dummy bits */
	sf_ext_access.bf.sflashDummyCount = -1;/* (cmd_cfg->dummy_bytes * 8) - 1; */
	/* 3 byte address */
	sf_ext_access.bf.sflashAddrCount = 2;/* cmd_cfg->addr_bytes - 1; */
	/* ECC is handled by device, we just push data  */
	sf_ext_access.bf.sflashDataCount = -1;/* cmd_cfg->data_bits - 1; */
	write_flash_ctrl_reg(FLASH_SF_EXT_ACCESS, sf_ext_access.wrd);

	/* Fill address */
	write_flash_ctrl_reg(FLASH_SF_ADDRESS, page_id);

	write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, SF_START_BIT_ENABLE);

	/* Wait command ready */
	if (wait_flash_cmd_rdy(SFLASH_CMD_TIMEOUT) != 0) {
		printf("%s fail!\n", __func__);
		return -1;
	}

    /* Delay for Command deselect time for safe */
	udelay(1);

#if 0 //20200303: spi_nand_wait() will polling OIP in status register also
	/* Dummy delay for safe */
	udelay(10);

	if (wait_till_ready()) {
		printf("%s wait timeout %d!\n", __func__, page_id);
		return -1;
	}
#endif
	return 0;
}

static int ca_spinand_read_cache(struct spi_nand_cmd *cmd)
{
	u32 offset = (u32)(cmd->addr[0] << 8) | (u32)(cmd->addr[1]);
	u8 *buf = cmd->rx_buf;
	int len = cmd->n_rx;
	unsigned int reg_data = 0;
	FLASH_SF_ACCESS_t sf_access;
	FLASH_SF_EXT_ACCESS_t sf_ext_access;

#ifdef CONFIG_RTK_V7_FPGA_PLATFORM
    fpga_pull_hold_high();
#endif

	sf_access.wrd = 0;
	sf_ext_access.wrd = 0;

	/* Read from cache
	 * Opcode -> 16bits offset addr -> data
	 * 2 bytes address must rely on extend access mothod
	 */
	sf_access.bf.sflashAcCode = GET_UP8BITS(SF_AC_OPCODE_EXTEND);
	sf_access.bf.sflashForceBurst = 1;
	sf_access.bf.sflashMIO = (cmd->io_data>>1);
	sf_access.bf.sflashMAddr = (cmd->io_addr>1)?1:0;
	sf_access.bf.sflashMData = (cmd->io_data>1)?1:0;
	write_flash_ctrl_reg(FLASH_SF_ACCESS, sf_access.wrd);

	/* Use command in extend_acc register */
	sf_ext_access.bf.sflashDirRdCmdEn = 1;
	sf_ext_access.bf.sflashOpCode = cmd->cmd;
	/* 8 dummy bits */
	sf_ext_access.bf.sflashDummyCount = cmd->n_dummy_cycle-1;
	/* 2 byte address */
	sf_ext_access.bf.sflashAddrCount = 1;
	#if defined(CONFIG_TARGET_RTK_TAURUS) || defined(CONFIG_TARGET_RTK_ELNATH)
	/* len bytes in one command request */
	sf_ext_access.bf.sflashDataCount = len - 1;
	#elif defined(CONFIG_TARGET_VENUS)
	/* 4 bytes in one command request */
	sf_ext_access.bf.sflashDataCount = 3;
	#endif
	write_flash_ctrl_reg(FLASH_SF_EXT_ACCESS, sf_ext_access.wrd);

	/* Fill address */
	write_flash_ctrl_reg(FLASH_SF_ADDRESS, offset);

	/* Read out data */
	while (len >= 4) {
		/* Start access */
		write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, SF_START_BIT_ENABLE);

		/* Wait command ready */
		if (wait_flash_cmd_rdy(SFLASH_CMD_TIMEOUT) != 0) {
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
		write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, SF_START_BIT_ENABLE);
		/* Wait command ready */
		if (wait_flash_cmd_rdy(SFLASH_CMD_TIMEOUT) != 0) {
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

static int ca_spinand_write_cache_x1(struct spi_nand_cmd *cmd)
{
	u32 offset = (u32)(cmd->addr[0] << 8) | (u32)(cmd->addr[1]);
	u8 *buf = (u8 *)cmd->tx_buf;
	int len = cmd->n_tx;
	unsigned int reg_data;
	FLASH_SF_ACCESS_t sf_access;
	FLASH_SF_EXT_ACCESS_t sf_ext_access;
	FLASH_FLASH_ACCESS_START_t access_start;

#ifdef CONFIG_RTK_V7_FPGA_PLATFORM
    fpga_pull_hold_high();
#endif

	sf_access.wrd = 0;
	sf_ext_access.wrd = 0;
	access_start.wrd = 0;

	/* Write to cache
	 * Opcode -> 16bits offset addr -> data ....
	 * 2 bytes address must rely on extend access mothod
	 */
	sf_access.bf.sflashAcCode = GET_UP8BITS(SF_AC_OPCODE_EXTEND);
	sf_access.bf.sflashMIO = SF_ACCESS_MIO_SINGLE;
	sf_access.bf.sflashMData = 0;
	sf_access.bf.sflashForceBurst = 1;	/* Burst mode */
	write_flash_ctrl_reg(FLASH_SF_ACCESS, sf_access.wrd);

	/* Use command in extend_acc register */
	sf_ext_access.bf.sflashDirRdCmdEn = 1;
	sf_ext_access.bf.sflashOpCode = SPI_NAND_CMD_PROG_LOAD;
	/* 0 dummy bits */
	sf_ext_access.bf.sflashDummyCount = -1;/* (cmd_cfg->dummy_bytes * 8) - 1; */
	/* 2 byte address */
	sf_ext_access.bf.sflashAddrCount = 1;/* cmd_cfg->addr_bytes - 1; */
    /* ECC is handled by device, we just push data  */
#if defined(CONFIG_TARGET_RTK_TAURUS) || defined(CONFIG_TARGET_RTK_ELNATH)
	sf_ext_access.bf.sflashDataCount = len - 1;
#elif defined(CONFIG_TARGET_VENUS)
	sf_ext_access.bf.sflashDataCount = 3;/* cmd_cfg->data_bits - 1; */
#endif
	write_flash_ctrl_reg(FLASH_SF_EXT_ACCESS, sf_ext_access.wrd);

	/* Fill address */
	write_flash_ctrl_reg(FLASH_SF_ADDRESS, offset);

	access_start.bf.sflashRegReq = 1;
	access_start.bf.sflashRegCmd = 1;
	/* Burst mode */
	while (len > 0) {
		reg_data = buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24);
		write_flash_ctrl_reg(FLASH_SF_DATA, reg_data);
		write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, access_start.wrd);

		/* Wait command ready */
		if (wait_flash_cmd_rdy(SFLASH_CMD_TIMEOUT) != 0) {
			printf("%s timeout\n", __func__);
			return -1;
		}

		len -= 4;
		buf += 4;
	}

	sf_access.bf.sflashForceBurst = 0;
	sf_access.bf.sflashForceTerm = 1;	/* Terminate burst mode */
	write_flash_ctrl_reg(FLASH_SF_ACCESS, sf_access.wrd);

	return 0;
}

static int ca_spinand_write_cache_x4(struct spi_nand_cmd *cmd)
{
	u32 offset = (u32)(cmd->addr[0] << 8) | (u32)(cmd->addr[1]);
	u8 *buf = (u8 *)cmd->tx_buf;
	int len = cmd->n_tx;
	unsigned int reg_data;
	FLASH_SF_ACCESS_t sf_access;
	FLASH_SF_EXT_ACCESS_t sf_ext_access;
	FLASH_FLASH_ACCESS_START_t access_start;

	sf_access.wrd = 0;
	sf_ext_access.wrd = 0;
	access_start.wrd = 0;

#ifdef CONFIG_RTK_V7_FPGA_PLATFORM
    fpga_pull_hold_high();
#endif

	/* Write to cache
	 * Opcode -> 16bits offset addr -> data ....
	 * 2 bytes address must rely on extend access mothod
	 */
	sf_access.bf.sflashAcCode = GET_UP8BITS(SF_AC_OPCODE_EXTEND);
	sf_access.bf.sflashMIO = SF_ACCESS_MIO_QUARD;
	sf_access.bf.sflashMData = 1;
	sf_access.bf.sflashForceBurst = 1;	/* Burst mode */
	write_flash_ctrl_reg(FLASH_SF_ACCESS, sf_access.wrd);

	/* Use command in extend_acc register */
	sf_ext_access.bf.sflashDirRdCmdEn = 1;
	sf_ext_access.bf.sflashOpCode = SPI_NAND_CMD_PROG_LOAD_X4;
	/* 0 dummy bits */
	sf_ext_access.bf.sflashDummyCount = -1;/* (cmd_cfg->dummy_bytes * 8) - 1; */
	/* 2 byte address */
	sf_ext_access.bf.sflashAddrCount = 1;/* cmd_cfg->addr_bytes - 1; */
#if defined(CONFIG_TARGET_RTK_TAURUS) || defined(CONFIG_TARGET_RTK_ELNATH)
    /* len bytes in one command request */
	sf_ext_access.bf.sflashDataCount = len - 1;
#elif defined(CONFIG_TARGET_VENUS)
    /* 4 bytes in one command request */
	sf_ext_access.bf.sflashDataCount = 3;/* cmd_cfg->data_bits - 1; */
#endif
	write_flash_ctrl_reg(FLASH_SF_EXT_ACCESS, sf_ext_access.wrd);

	/* Fill address */
	write_flash_ctrl_reg(FLASH_SF_ADDRESS, offset);

	access_start.bf.sflashRegReq = 1;
	access_start.bf.sflashRegCmd = 1;
	/* Burst mode */
	while (len > 0) {
		reg_data = buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24);
		write_flash_ctrl_reg(FLASH_SF_DATA, reg_data);
		write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, access_start.wrd);

		/* Wait command ready */
		if (wait_flash_cmd_rdy(SFLASH_CMD_TIMEOUT) != 0) {
			printf("%s timeout\n", __func__);
			return -1;
		}

		len -= 4;
		buf += 4;
	}

	sf_access.bf.sflashForceBurst = 0;
	sf_access.bf.sflashForceTerm = 1;	/* Terminate burst mode */
	write_flash_ctrl_reg(FLASH_SF_ACCESS, sf_access.wrd);

	return 0;
}

static int ca_spinand_program_execute(struct spi_nand_cmd *cmd)
{
	u32	offset = (u32)(cmd->addr[0] << 16) | (u32)(cmd->addr[1] << 8)
		| (u32)(cmd->addr[2]);
	FLASH_SF_ACCESS_t sf_access;
	FLASH_SF_EXT_ACCESS_t sf_ext_access;

#ifdef CONFIG_RTK_V7_FPGA_PLATFORM
    fpga_pull_hold_high();
#endif

	sf_access.wrd = 0;
	sf_ext_access.wrd = 0;

	/* Flush cache to flash chip
	 * Opcode -> 24bits offset addr
	 */
	sf_access.bf.sflashAcCode = GET_UP8BITS(SF_AC_OPCODE_EXTEND);
	write_flash_ctrl_reg(FLASH_SF_ACCESS, sf_access.wrd);

	/* Use command in extend_acc register */
	sf_ext_access.bf.sflashDirRdCmdEn = 1;
	sf_ext_access.bf.sflashOpCode = SPI_NAND_CMD_PROG_EXC;
	/* 0 dummy bits */
	sf_ext_access.bf.sflashDummyCount = -1;/* (cmd_cfg->dummy_bytes * 8) - 1; */
	/* 3 byte address */
	sf_ext_access.bf.sflashAddrCount = 2;/* cmd_cfg->addr_bytes - 1; */
	/* zero data byte */
	sf_ext_access.bf.sflashDataCount = -1; /* cmd_cfg->data_bits - 1; */
	write_flash_ctrl_reg(FLASH_SF_EXT_ACCESS, sf_ext_access.wrd);

	/* Fill address */
	write_flash_ctrl_reg(FLASH_SF_ADDRESS, offset);

	write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, SF_START_BIT_ENABLE);

	/* Wait command ready */
	if (wait_flash_cmd_rdy(SFLASH_CMD_TIMEOUT) != 0) {
		printf("%s fail!\n", __func__);
		return -1;
	}

    /* Delay for Command deselect time for safe */
	udelay(1);
	return 0;
}

static int ca_spinand_erase_block(struct spi_nand_cmd *cmd)
{
	u32 block_id = (u32)(cmd->addr[0] << 16) | (u32)(cmd->addr[1] << 8)
		| (u32)(cmd->addr[2]);
	FLASH_SF_ACCESS_t sf_access;

	sf_access.wrd = 0;

#ifdef CONFIG_RTK_V7_FPGA_PLATFORM
    fpga_pull_hold_high();
#endif

	/* Write enable
	 * Opcode only protocol
	 */
	sf_access.bf.sflashOpCode = SPI_NAND_CMD_WR_ENABLE;
	sf_access.bf.sflashAcCode = GET_UP8BITS(SF_AC_OPCODE);
	write_flash_ctrl_reg(FLASH_SF_ACCESS, sf_access.wrd);
	write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, SF_START_BIT_ENABLE);

	/* Wait command ready */
	if (wait_flash_cmd_rdy(SFLASH_CMD_TIMEOUT) != 0)
		printf("%s: command timeout!\n", __func__);


	/* Erase block
	 * Opcode -> 24bits Addr
	 */
	sf_access.bf.sflashOpCode = SPI_NAND_CMD_BLK_ERASE;
	sf_access.bf.sflashAcCode = GET_UP8BITS(SF_AC_OPCODE_3_ADDR);
	write_flash_ctrl_reg(FLASH_SF_ACCESS, sf_access.wrd);
	write_flash_ctrl_reg(FLASH_SF_ADDRESS, block_id);
	write_flash_ctrl_reg(FLASH_FLASH_ACCESS_START, SF_START_BIT_ENABLE);

	/* Wait command ready */
	if (wait_flash_cmd_rdy(SFLASH_CMD_TIMEOUT) != 0) {
		printf("%s fail!\n", __func__);
		return -1;
	}

    /* Delay for Command deselect time for safe */
	udelay(1);

#if 0 //20200303: spi_nand_wait() will polling OIP in status register also
	/* elapse 1ms before issuing any other command */
	udelay(10);

	if (wait_till_ready()) {
		printf("%s wait timeout %d!\n", __func__, block_id);
		return -1;
	}
#endif
	return 0;
}

/*
 * ca_spinand_cmd_fn - to process a command to send to the SPI-NAND
 * by cortina SPI NAND controller
 * @chip: SPI-NAND device structure
 * @cmd: command structure
 * Description:
 *   Set up the command buffer to send to the SPI controller.
 *   The command buffer has to initialized to 0.
 */
int ca_spinand_cmd_fn(struct spi_nand_cmd *cmd)
{
	int ret = 0;
	switch (cmd->cmd) {
    	case SPI_NAND_CMD_RESET:
    		ret = ca_spinand_reset(cmd);
    		break;
    	case SPI_NAND_CMD_GET_FEATURE:
    		ret = ca_spinand_get_feature(cmd);
    		break;
    	case SPI_NAND_CMD_SET_FEATURE:
    		ret = ca_spinand_set_feature(cmd);
    		break;
    	case SPI_NAND_CMD_PAGE_READ:
    		ret = ca_spinand_read_page_to_cache(cmd);
    		break;
    	case SPI_NAND_CMD_READ_PAGE_CACHE_RDM:
    		/* Todo */
    		spinand_debug("%s()..READ_PAGE_CACHE_RDM not implement yet.\n",
    			__func__);
    		break;
    	case SPI_NAND_CMD_READ_PAGE_CACHE_LAST:
    		/* Todo */
    		spinand_debug("%s()...READ_PAGE_CACHE_LAST not implement yet.\n",
    			__func__);
    		break;
    	case SPI_NAND_CMD_READ_FROM_CACHE:
    	case SPI_NAND_CMD_READ_FROM_CACHE_FAST:
    	case SPI_NAND_CMD_READ_FROM_CACHE_X2:
    	case SPI_NAND_CMD_READ_FROM_CACHE_X4:
    	case SPI_NAND_CMD_READ_FROM_CACHE_DUAL_IO:
    	case SPI_NAND_CMD_READ_FROM_CACHE_QUAD_IO:
    		ret = ca_spinand_read_cache(cmd);
    		break;
    	case SPI_NAND_CMD_BLK_ERASE:
    		ret = ca_spinand_erase_block(cmd);
    		break;
    	case SPI_NAND_CMD_PROG_EXC:
    		ret = ca_spinand_program_execute(cmd);
    		break;
    	case SPI_NAND_CMD_PROG_LOAD:
    		ret = ca_spinand_write_cache_x1(cmd);
    		break;
    	case SPI_NAND_CMD_PROG_LOAD_RDM_DATA:
    		/* Todo */
    		spinand_debug("%s()...PROG_LOAD_RDM_DATA not implement yet.\n",
    			__func__);
    		break;
    	case SPI_NAND_CMD_PROG_LOAD_X4:
    		ret = ca_spinand_write_cache_x4(cmd);
    		break;
    	case SPI_NAND_CMD_PROG_LOAD_RDM_DATA_X4:
    		/* Todo */
    		spinand_debug("%s()...PROG_LOAD_RDM_DATA_X4 not implement yet.\n",
    			__func__);
    		break;
    	case SPI_NAND_CMD_READ_ID:
    		ret = ca_spinand_read_id(cmd);
    		break;
    	case SPI_NAND_CMD_WR_DISABLE:
    		/* Todo */
    		break;
    	case SPI_NAND_CMD_WR_ENABLE:
    		ret = ca_spinand_write_enable(cmd);
    		break;
        case SPI_NAND_CMD_DIE_SEL_ESMT_WINBOND:
        case SPI_NAND_CMD_MXIC_ECC_STS_READ:
            ret = ca_spinand_pio_raw_cmd(cmd);
            break;
    	case SPI_NAND_CMD_END:
    	default:
    		break;
	}

    return ret;
}

void spi_nand_set_clock(struct spi_nand_info *info)
{
    GLOBAL_EPLLDIV_t global_eplldiv;
    global_eplldiv.wrd = readl(GLOBAL_EPLLDIV);
    /* Set LS_AXI to 200MHz */
    if(global_eplldiv.bf.lsaxi_divsel != 20) {
        global_eplldiv.bf.lsaxi_divsel = 20;
        writel(global_eplldiv.wrd, GLOBAL_EPLLDIV);
    }

    /* Minimize flash timing */
#if defined(CONFIG_TARGET_VENUS)
    write_flash_ctrl_reg(FLASH_SF_TIMING, 0x07010101);
#elif defined(CONFIG_TARGET_RTK_TAURUS) || defined(CONFIG_TARGET_RTK_ELNATH)
    if(info->sclk >= 100) {
        write_flash_ctrl_reg(FLASH_SF_TIMING, 0x28010101);
        #if defined(CONFIG_RTK_V7_FPGA_PLATFORM) || defined(CONFIG_RTK_V8_FPGA_PLATFORM)
        write_flash_ctrl_reg(FLASH_SF_TIMING, 0x18010101);
        #endif
    } else if (info->sclk >= 50) {
        write_flash_ctrl_reg(FLASH_SF_TIMING, 0x19010101);
    } else if (info->sclk >= 40) {
        write_flash_ctrl_reg(FLASH_SF_TIMING, 0x17010101);
    }
#endif

#if defined(CONFIG_RTK_ZEBU_SIM)
    write_flash_ctrl_reg(FLASH_SF_TIMING, 0x19010101);
#endif
}

uint32_t spi_nand_get_clock(void)
{
    GLOBAL_EPLLDIV_t global_eplldiv;
    uint32_t lsaxi_MHz;
    uint32_t sflashClkWidth;

    global_eplldiv.wrd = readl(GLOBAL_EPLLDIV);
    lsaxi_MHz = 2000/(global_eplldiv.bf.lsaxi_divsel/2);
    sflashClkWidth = read_flash_ctrl_reg(FLASH_SF_TIMING)>>24&0xF;

    if (sflashClkWidth == 8) return (lsaxi_MHz/2);
    else if (sflashClkWidth == 9) return (lsaxi_MHz/4);
    else if (sflashClkWidth == 7) return (lsaxi_MHz/5);
    else if (sflashClkWidth == 6) return (lsaxi_MHz/6);
    else if (sflashClkWidth == 5) return (lsaxi_MHz/8);
    else if (sflashClkWidth == 4) return (lsaxi_MHz/10);
    else if (sflashClkWidth == 3) return (lsaxi_MHz/12);
    else if (sflashClkWidth == 2) return (lsaxi_MHz/14);
    else if (sflashClkWidth == 1) return (lsaxi_MHz/16);
    else return (lsaxi_MHz/18);
}

void ca_spinand_ctrlr_init(void)
{
	/* Set FLASH_TYPE as serial flash, flash_pin = 1 */
	write_flash_ctrl_reg(FLASH_TYPE, 0x8000);

#ifdef CONFIG_RTK_V7_FPGA_PLATFORM
    fpga_pull_hold_high();
#endif
}

