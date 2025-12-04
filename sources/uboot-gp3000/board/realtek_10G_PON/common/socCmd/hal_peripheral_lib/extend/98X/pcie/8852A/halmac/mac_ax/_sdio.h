/******************************************************************************
 *
 * Copyright(c) 2019 Realtek Corporation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 ******************************************************************************/

#ifndef _MAC_AX_SDIO_H_
#define _MAC_AX_SDIO_H_

#include "../type.h"

/* SDIO CMD address mapping */
#define SDIO_8BYTE_LEN_MASK	0x0FFF
#define SDIO_4BYTE_LEN_MASK	0x03FF
#define SDIO_LOCAL_MSK		0x00000FFF
#define SDIO_LOCAL_SHIFT	0x00001000
#define	WLAN_IOREG_MSK		0xFFFE0000

/* IO Bus domain address mapping */
#define SDIO_LOCAL_BASE		0x80000000
#define WLAN_IOREG_BASE		0x00000000

#define SDIO_CMD_ADDR_TXFF_SHIFT	12
#define SDIO_CMD_ADDR_TXFF_0	0
#define SDIO_CMD_ADDR_TXFF_1	1
#define SDIO_CMD_ADDR_TXFF_2	2
#define SDIO_CMD_ADDR_TXFF_3	3
#define SDIO_CMD_ADDR_TXFF_4	4
#define SDIO_CMD_ADDR_TXFF_5	5
#define SDIO_CMD_ADDR_TXFF_6	6
#define SDIO_CMD_ADDR_TXFF_7	7
#define SDIO_CMD_ADDR_TXFF_8	8
#define SDIO_CMD_ADDR_TXFF_9	9
#define SDIO_CMD_ADDR_TXFF_10	10
#define SDIO_CMD_ADDR_TXFF_11	11
#define SDIO_CMD_ADDR_TXFF_12	12
#define SDIO_CMD_ADDR_RXFF	0x1F00

#define SDIO_WAIT_CNT		50

struct mac_sdio_tbl {
	mac_ax_mutex lock;
};

enum sdio_io_size {
	SDIO_IO_BYTE,
	SDIO_IO_WORD,
	SDIO_IO_DWORD,

	/* keep last */
	SDIO_IO_LAST,
	SDIO_IO_MAX = SDIO_IO_LAST,
	SDIO_IO_INVALID = SDIO_IO_LAST,
};

u8 reg_read8_sdio(struct mac_ax_adapter *adapter, u32 addr);
void reg_write8_sdio(struct mac_ax_adapter *adapter, u32 addr, u8 val);
u16 reg_read16_sdio(struct mac_ax_adapter *adapter, u32 addr);
void reg_write16_sdio(struct mac_ax_adapter *adapter, u32 addr, u16 val);
u32 reg_read32_sdio(struct mac_ax_adapter *adapter, u32 addr);
void reg_write32_sdio(struct mac_ax_adapter *adapter, u32 addr, u32 val);
u32 tx_allow_sdio(struct mac_ax_adapter *adapter,
		  struct mac_ax_sdio_tx_info *info);
u32 tx_cmd_addr_sdio(struct mac_ax_adapter *adapter,
		     struct mac_ax_sdio_tx_info *info, u32 *cmd_addr);
u32 sdio_init(struct mac_ax_adapter *adapter);

u32 sdio_tbl_init(struct mac_ax_adapter *adapter);
u32 sdio_tbl_exit(struct mac_ax_adapter *adapter);

#endif
