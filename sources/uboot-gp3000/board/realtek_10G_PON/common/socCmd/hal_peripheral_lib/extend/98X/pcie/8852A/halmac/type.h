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

#ifndef _MAC_AX_TYPE_H_
#define _MAC_AX_TYPE_H_

#include "pltfm_cfg.h"
#include "chip_cfg.h"
#include "errors.h"
#include "mac_ax/state_mach.h"
#include "feature_cfg.h"

/* platform callback */
#define PLTFM_SDIO_CMD52_R8(addr)                                              \
	adapter->pltfm_cb->sdio_cmd52_r8(adapter->drv_adapter, addr)
#define PLTFM_SDIO_CMD53_R8(addr)                                              \
	adapter->pltfm_cb->sdio_cmd53_r8(adapter->drv_adapter, addr)
#define PLTFM_SDIO_CMD53_R16(addr)                                             \
	adapter->pltfm_cb->sdio_cmd53_r16(adapter->drv_adapter, addr)
#define PLTFM_SDIO_CMD53_R32(addr)                                             \
	adapter->pltfm_cb->sdio_cmd53_r32(adapter->drv_adapter, addr)
#define PLTFM_SDIO_CMD53_RN(addr, size, val)                                   \
	adapter->pltfm_cb->sdio_cmd53_rn(adapter->drv_adapter, addr, size, val)
#define PLTFM_SDIO_CMD52_W8(addr, val)                                         \
	adapter->pltfm_cb->sdio_cmd52_w8(adapter->drv_adapter, addr, val)
#define PLTFM_SDIO_CMD53_W8(addr, val)                                         \
	adapter->pltfm_cb->sdio_cmd53_w8(adapter->drv_adapter, addr, val)
#define PLTFM_SDIO_CMD53_W16(addr, val)                                        \
	adapter->pltfm_cb->sdio_cmd53_w16(adapter->drv_adapter, addr, val)
#define PLTFM_SDIO_CMD53_W32(addr, val)                                        \
	adapter->pltfm_cb->sdio_cmd53_w32(adapter->drv_adapter, addr, val)
#define PLTFM_SDIO_CMD53_WN(addr, size, val)                                   \
	adapter->pltfm_cb->sdio_cmd53_wn(adapter->drv_adapter, addr, size, val)
#define PLTFM_SDIO_CMD52_CIA_R8(addr)                                          \
	adapter->pltfm_cb->sdio_cmd52_cia_r8(adapter->drv_adapter, addr)

#define PLTFM_REG_R8(addr)                                                     \
	adapter->pltfm_cb->reg_r8(adapter->drv_adapter, addr)
#define PLTFM_REG_R16(addr)                                                    \
	adapter->pltfm_cb->reg_r16(adapter->drv_adapter, addr)
#define PLTFM_REG_R32(addr)                                                    \
	adapter->pltfm_cb->reg_r32(adapter->drv_adapter, addr)
#define PLTFM_REG_W8(addr, val)                                                \
	adapter->pltfm_cb->reg_w8(adapter->drv_adapter, addr, val)
#define PLTFM_REG_W16(addr, val)                                               \
	adapter->pltfm_cb->reg_w16(adapter->drv_adapter, addr, val)
#define PLTFM_REG_W32(addr, val)                                               \
	adapter->pltfm_cb->reg_w32(adapter->drv_adapter, addr, val)

#define PLTFM_TX(buf, len)                                                     \
	adapter->pltfm_cb->tx(adapter->drv_adapter, buf, len)

#define PLTFM_FREE(buf, size)                                                  \
	adapter->pltfm_cb->rtl_free(adapter->drv_adapter, buf, size)
#define PLTFM_MALLOC(size)                                                     \
	adapter->pltfm_cb->rtl_malloc(adapter->drv_adapter, size)
#define PLTFM_MEMCPY(dest, src, size)                                          \
	adapter->pltfm_cb->rtl_memcpy(adapter->drv_adapter, dest, src, size)
#define PLTFM_MEMSET(addr, value, size)                                        \
	adapter->pltfm_cb->rtl_memset(adapter->drv_adapter, addr, value, size)
#define PLTFM_MEMCMP(ptr1, ptr2, num)                                          \
	adapter->pltfm_cb->rtl_memcmp(adapter->drv_adapter, ptr1, ptr2, num)
#define PLTFM_DELAY_US(us)                                                     \
	adapter->pltfm_cb->rtl_delay_us(adapter->drv_adapter, us)
#define PLTFM_DELAY_MS(ms)                                                     \
	adapter->pltfm_cb->rtl_delay_us(adapter->drv_adapter, ms)

#define PLTFM_MUTEX_INIT(mutex)                                                \
	adapter->pltfm_cb->mutex_init(adapter->drv_adapter, mutex)
#define PLTFM_MUTEX_DEINIT(mutex)                                              \
	adapter->pltfm_cb->mutex_deinit(adapter->drv_adapter, mutex)
#define PLTFM_MUTEX_LOCK(mutex)                                                \
	adapter->pltfm_cb->mutex_lock(adapter->drv_adapter, mutex)
#define PLTFM_MUTEX_UNLOCK(mutex)                                              \
	adapter->pltfm_cb->mutex_unlock(adapter->drv_adapter, mutex)

#define adapter_to_mac_ops(adapter) ((struct mac_ax_ops *)((adapter)->ops))
#define adapter_to_intf_ops(adapter)                                           \
	((struct mac_ax_intf_ops *)((adapter)->ops->intf_ops))

#define MAC_REG_R8(addr) ops->reg_read8(adapter, addr)
#define MAC_REG_R16(addr) ops->reg_read16(adapter, addr)
#define MAC_REG_R32(addr) ops->reg_read32(adapter, addr)
#define MAC_REG_W8(addr, val) ops->reg_write8(adapter, addr, val)
#define MAC_REG_W16(addr, val) ops->reg_write16(adapter, addr, val)
#define MAC_REG_W32(addr, val) ops->reg_write32(adapter, addr, val)

#define MAC_AX_CH_DMA_CH0	0
#define MAC_AX_CH_DMA_CH1	1
#define MAC_AX_CH_DMA_CH2	2
#define MAC_AX_CH_DMA_CH3	3
#define MAC_AX_CH_DMA_CH4	4
#define MAC_AX_CH_DMA_CH5	5
#define MAC_AX_CH_DMA_CH6	6
#define MAC_AX_CH_DMA_CH7	7
#define MAC_AX_CH_DMA_CH8	8
#define MAC_AX_CH_DMA_CH9	9
#define MAC_AX_CH_DMA_CH10	10
#define MAC_AX_CH_DMA_CH11	11
#define MAC_AX_CH_DMA_FWCMD	12

#define MAC_AX_WMM0_SEL		0
#define MAC_AX_WMM1_SEL		1
#define MAC_AX_WMM2_SEL		2
#define MAC_AX_WMM3_SEL		3

#define MAC_AX_MAX_RU_NUM	4

enum mac_ax_trx_mode {
	MAC_AX_TRX_NORMAL,
	MAC_AX_TRX_LOOPBACK,

	/* keep last */
	MAC_AX_TRX_LAST,
	MAC_AX_TRX_MAX = MAC_AX_TRX_LAST,
	MAC_AX_TRX_INVALID = MAC_AX_TRX_LAST,
};

enum mac_ax_intf {
	MAC_AX_INTF_USB,
	MAC_AX_INTF_SDIO,
	MAC_AX_INTF_PCIE,

	/* keep last */
	MAC_AX_INTF_LAST,
	MAC_AX_INTF_MAX = MAC_AX_INTF_LAST,
	MAC_AX_INTF_INVALID = MAC_AX_INTF_LAST,
};

enum mac_ax_pkt_t {
	MAC_AX_PKT_DATA,
	MAC_AX_PKT_MGNT,
	MAC_AX_PKT_CTRL,
	MAC_AX_PKT_8023,
	MAC_AX_PKT_H2C,
	MAC_AX_PKT_FWDL,
	MAC_AX_PKT_C2H,
	//mgt frame
	MAC_AX_PKT_ASSOC_REQ,
	MAC_AX_PKT_ASSOC_RSP,
	MAC_AX_PKT_REASSOC_REQ,
	MAC_AX_PKT_REASSOC_RSP,
	MAC_AX_PKT_PROBE_REQ,
	MAC_AX_PKT_PROBE_RSP,
	MAC_AX_PKT_BEACON,
	MAC_AX_PKT_ATIM,
	MAC_AX_PKT_DISASSOC,
	MAC_AX_PKT_AUTH,
	//control frame
	MAC_AX_PKT_PS_POLL,
	MAC_AX_PKT_RTS_CONTROL,
	MAC_AX_PKT_CTS_CONTROL,
	MAC_AX_PKT_CTS2SELF_CONTROL,
	MAC_AX_PKT_BASIC_ACK,
	MAC_AX_PKT_EXT_COMPR_ACK,
	MAC_AX_PKT_COMPR_ACK,
	MAC_AX_PKT_MULTI_TID_ACK,
	MAC_AX_PKT_GCR,
	MAC_AX_PKT_GLK_GCR,
	MAC_AX_PKT_MULTI_STA_ACK,
	MAC_AX_PKT_CF_END,
	MAC_AX_PKT_TRS_CONTROL,
	MAC_AX_PKT_OM_CONTROL,
	MAC_AX_PKT_HLA_CONTROL,
	MAC_AX_PKT_BSR_CONTROL,
	MAC_AX_PKT_UPH_CONTROL,
	MAC_AX_PKT_BQR_CONTROL,
	MAC_AX_PKT_CAS_CONTROL,
	//trigger frame
	MAC_AX_PKT_BASIC_TRIGGER,
	MAC_AX_PKT_BFRP,
	MAC_AX_PKT_MU_BAR,
	MAC_AX_PKT_MU_RTS,
	MAC_AX_PKT_BSRP,
	MAC_AX_PKT_GCR_MUBAR,
	MAC_AX_PKT_BQRP,
	MAC_AX_PKT_NFRP,
	/* keep last */
	MAC_AX_PKT_LAST,
	MAC_AX_PKT_MAX = MAC_AX_PKT_LAST,
	MAC_AX_PKT_INVALID = MAC_AX_PKT_LAST,
};

enum mac_ax_mem_sel {
	MAC_AX_MEM_SHARED_BUF,
	MAC_AX_MEM_DMAC_TBL,
	MAC_AX_MEM_STA_SCHED,
	MAC_AX_MEM_UL_GRP_TBL,
	MAC_AX_MEM_MU_SCORE_TBL,
	MAC_AX_MEM_MACHDR_SHORTCUT,
	MAC_AX_MEM_RXPLD_FLTR_CAM,
	MAC_AX_MEM_SECURITY_CAM,
	MAC_AX_MEM_WOW_CAM,
	MAC_AX_MEM_CMAC_TBL,
	MAC_AX_MEM_ADDR_CAM,
	MAC_AX_MEM_BA_CAM,
	MAC_AX_MEM_BCN_IE_CAM,

	/* keep last */
	MAC_AX_MEM_LAST,
	MAC_AX_MEM_MAX = MAC_AX_MEM_LAST,
	MAC_AX_MEM_INVALID = MAC_AX_MEM_LAST,
};

enum mac_ax_efuse_read_cfg {
	MAC_AX_EFUSE_R_AUTO,
	MAC_AX_EFUSE_R_DRV,
	MAC_AX_EFUSE_R_FW,

	/* keep last */
	MAC_AX_EFUSE_R_LAST,
	MAC_AX_EFUSE_R_MAX = MAC_AX_EFUSE_R_LAST,
	MAC_AX_EFUSE_R_INVALID = MAC_AX_EFUSE_R_LAST,
};

enum mac_ax_efuse_bank {
	MAC_AX_EFUSE_BANK_WIFI,
	MAC_AX_EFUSE_BANK_BT,

	/* keep last */
	MAC_AX_EFUSE_BANK_LAST,
	MAC_AX_EFUSE_BANK_MAX = MAC_AX_EFUSE_BANK_LAST,
	MAC_AX_EFUSE_BANK_INVALID = MAC_AX_EFUSE_BANK_LAST,
};

enum mac_ax_efuse_parser_cfg {
	MAC_AX_EFUSE_PARSER_MAP,
	MAC_AX_EFUSE_PARSER_MASK,

	/* keep last */
	MAC_AX_EFUSE_PARSER_LAST,
	MAC_AX_EFUSE_PARSER_MAX = MAC_AX_EFUSE_PARSER_LAST,
	MAC_AX_EFUSE_PARSER_INVALID = MAC_AX_EFUSE_PARSER_LAST,
};

enum mac_ax_data_rate {
	MAC_AX_CCK1		= 0x0,
	MAC_AX_CCK2		= 0x1,
	MAC_AX_CCK5_5		= 0x2,
	MAC_AX_CCK11		= 0x3,
	MAC_AX_OFDM6		= 0x4,
	MAC_AX_OFDM9		= 0x5,
	MAC_AX_OFDM12		= 0x6,
	MAC_AX_OFDM18		= 0x7,
	MAC_AX_OFDM24		= 0x8,
	MAC_AX_OFDM36		= 0x9,
	MAC_AX_OFDM48		= 0xA,
	MAC_AX_OFDM54		= 0xB,
	MAC_AX_MCS0		= 0x80,
	MAC_AX_MCS1		= 0x81,
	MAC_AX_MCS2		= 0x82,
	MAC_AX_MCS3		= 0x83,
	MAC_AX_MCS4		= 0x84,
	MAC_AX_MCS5		= 0x85,
	MAC_AX_MCS6		= 0x86,
	MAC_AX_MCS7		= 0x87,
	MAC_AX_MCS8		= 0x88,
	MAC_AX_MCS9		= 0x89,
	MAC_AX_MCS10		= 0x8A,
	MAC_AX_MCS11		= 0x8B,
	MAC_AX_MCS12		= 0x8C,
	MAC_AX_MCS13		= 0x8D,
	MAC_AX_MCS14		= 0x8E,
	MAC_AX_MCS15		= 0x8F,
	MAC_AX_MCS16		= 0x90,
	MAC_AX_MCS17		= 0x91,
	MAC_AX_MCS18		= 0x92,
	MAC_AX_MCS19		= 0x93,
	MAC_AX_MCS20		= 0x94,
	MAC_AX_MCS21		= 0x95,
	MAC_AX_MCS22		= 0x96,
	MAC_AX_MCS23		= 0x97,
	MAC_AX_MCS24		= 0x98,
	MAC_AX_MCS25		= 0x99,
	MAC_AX_MCS26		= 0x9A,
	MAC_AX_MCS27		= 0x9B,
	MAC_AX_MCS28		= 0x9C,
	MAC_AX_MCS29		= 0x9D,
	MAC_AX_MCS30		= 0x9E,
	MAC_AX_MCS31		= 0x9F,
	MAC_AX_VHT_NSS1_MCS0	= 0x100,
	MAC_AX_VHT_NSS1_MCS1	= 0x101,
	MAC_AX_VHT_NSS1_MCS2	= 0x102,
	MAC_AX_VHT_NSS1_MCS3	= 0x103,
	MAC_AX_VHT_NSS1_MCS4	= 0x104,
	MAC_AX_VHT_NSS1_MCS5	= 0x105,
	MAC_AX_VHT_NSS1_MCS6	= 0x106,
	MAC_AX_VHT_NSS1_MCS7	= 0x107,
	MAC_AX_VHT_NSS1_MCS8	= 0x108,
	MAC_AX_VHT_NSS1_MCS9	= 0x109,
	MAC_AX_VHT_NSS2_MCS0	= 0x110,
	MAC_AX_VHT_NSS2_MCS1	= 0x111,
	MAC_AX_VHT_NSS2_MCS2	= 0x112,
	MAC_AX_VHT_NSS2_MCS3	= 0x113,
	MAC_AX_VHT_NSS2_MCS4	= 0x114,
	MAC_AX_VHT_NSS2_MCS5	= 0x115,
	MAC_AX_VHT_NSS2_MCS6	= 0x116,
	MAC_AX_VHT_NSS2_MCS7	= 0x117,
	MAC_AX_VHT_NSS2_MCS8	= 0x118,
	MAC_AX_VHT_NSS2_MCS9	= 0x119,
	MAC_AX_VHT_NSS3_MCS0	= 0x120,
	MAC_AX_VHT_NSS3_MCS1	= 0x121,
	MAC_AX_VHT_NSS3_MCS2	= 0x122,
	MAC_AX_VHT_NSS3_MCS3	= 0x123,
	MAC_AX_VHT_NSS3_MCS4	= 0x124,
	MAC_AX_VHT_NSS3_MCS5	= 0x125,
	MAC_AX_VHT_NSS3_MCS6	= 0x126,
	MAC_AX_VHT_NSS3_MCS7	= 0x127,
	MAC_AX_VHT_NSS3_MCS8	= 0x128,
	MAC_AX_VHT_NSS3_MCS9	= 0x129,
	MAC_AX_VHT_NSS4_MCS0	= 0x130,
	MAC_AX_VHT_NSS4_MCS1	= 0x131,
	MAC_AX_VHT_NSS4_MCS2	= 0x132,
	MAC_AX_VHT_NSS4_MCS3	= 0x133,
	MAC_AX_VHT_NSS4_MCS4	= 0x134,
	MAC_AX_VHT_NSS4_MCS5	= 0x135,
	MAC_AX_VHT_NSS4_MCS6	= 0x136,
	MAC_AX_VHT_NSS4_MCS7	= 0x137,
	MAC_AX_VHT_NSS4_MCS8	= 0x138,
	MAC_AX_VHT_NSS4_MCS9	= 0x139,
	MAC_AX_HE_NSS1_MCS0	= 0x180,
	MAC_AX_HE_NSS1_MCS1	= 0x181,
	MAC_AX_HE_NSS1_MCS2	= 0x182,
	MAC_AX_HE_NSS1_MCS3	= 0x183,
	MAC_AX_HE_NSS1_MCS4	= 0x184,
	MAC_AX_HE_NSS1_MCS5	= 0x185,
	MAC_AX_HE_NSS1_MCS6	= 0x186,
	MAC_AX_HE_NSS1_MCS7	= 0x187,
	MAC_AX_HE_NSS1_MCS8	= 0x188,
	MAC_AX_HE_NSS1_MCS9	= 0x189,
	MAC_AX_HE_NSS1_MCS10	= 0x18A,
	MAC_AX_HE_NSS1_MCS11	= 0x18B,
	MAC_AX_HE_NSS2_MCS0	= 0x190,
	MAC_AX_HE_NSS2_MCS1	= 0x191,
	MAC_AX_HE_NSS2_MCS2	= 0x192,
	MAC_AX_HE_NSS2_MCS3	= 0x193,
	MAC_AX_HE_NSS2_MCS4	= 0x194,
	MAC_AX_HE_NSS2_MCS5	= 0x195,
	MAC_AX_HE_NSS2_MCS6	= 0x196,
	MAC_AX_HE_NSS2_MCS7	= 0x197,
	MAC_AX_HE_NSS2_MCS8	= 0x198,
	MAC_AX_HE_NSS2_MCS9	= 0x199,
	MAC_AX_HE_NSS2_MCS10	= 0x19A,
	MAC_AX_HE_NSS2_MCS11	= 0x19B,
	MAC_AX_HE_NSS3_MCS0	= 0x1A0,
	MAC_AX_HE_NSS3_MCS1	= 0x1A1,
	MAC_AX_HE_NSS3_MCS2	= 0x1A2,
	MAC_AX_HE_NSS3_MCS3	= 0x1A3,
	MAC_AX_HE_NSS3_MCS4	= 0x1A4,
	MAC_AX_HE_NSS3_MCS5	= 0x1A5,
	MAC_AX_HE_NSS3_MCS6	= 0x1A6,
	MAC_AX_HE_NSS3_MCS7	= 0x1A7,
	MAC_AX_HE_NSS3_MCS8	= 0x1A8,
	MAC_AX_HE_NSS3_MCS9	= 0x1A9,
	MAC_AX_HE_NSS3_MCS10	= 0x1AA,
	MAC_AX_HE_NSS3_MCS11	= 0x1AB,
	MAC_AX_HE_NSS4_MCS0	= 0x1B0,
	MAC_AX_HE_NSS4_MCS1	= 0x1B1,
	MAC_AX_HE_NSS4_MCS2	= 0x1B2,
	MAC_AX_HE_NSS4_MCS3	= 0x1B3,
	MAC_AX_HE_NSS4_MCS4	= 0x1B4,
	MAC_AX_HE_NSS4_MCS5	= 0x1B5,
	MAC_AX_HE_NSS4_MCS6	= 0x1B6,
	MAC_AX_HE_NSS4_MCS7	= 0x1B7,
	MAC_AX_HE_NSS4_MCS8	= 0x1B8,
	MAC_AX_HE_NSS4_MCS9	= 0x1B9,
	MAC_AX_HE_NSS4_MCS10	= 0x1BA,
	MAC_AX_HE_NSS4_MCS11	= 0x1BB
};

enum mac_ax_bw {
	MAC_AX_BW_20M = 0,
	MAC_AX_BW_40M = 1,
	MAC_AX_BW_80M = 2,
	MAC_AX_BW_160M = 3
};

enum mac_ax_gi_ltf {
	MAC_AX_LGI_4XHE32 = 0,
	MAC_AX_SGI_4XHE08 = 1,
	MAC_AX_2XHE16 = 2,
	MAC_AX_2XHE08 = 3,
	MAC_AX_1XHE16 = 4,
	MAC_AX_1XHE08 = 5
};

enum mac_ax_stbc {
	MAC_AX_STBC_DIS = 0,
	MAC_AX_STBC_EN = 1,
	MAC_AX_STBC_HT2 = 2
};

enum mac_ax_sdio_4byte_mode {
	MAC_AX_SDIO_4BYTE_MODE_DISABLE,
	MAC_AX_SDIO_4BYTE_MODE_RW,
	MAC_AX_SDIO_4BYTE_MODE_R,
	MAC_AX_SDIO_4BYTE_MODE_W,

	/* keep last */
	MAC_AX_SDIO_4BYTE_MODE_LAST,
	MAC_AX_SDIO_4BYTE_MODE_MAX = MAC_AX_SDIO_4BYTE_MODE_LAST,
	MAC_AX_SDIO_4BYTE_MODE_INVALID = MAC_AX_SDIO_4BYTE_MODE_LAST,
};

enum mac_ax_sdio_tx_mode {
	MAC_AX_SDIO_TX_MODE_AGG,
	MAC_AX_SDIO_TX_MODE_DUMMY_BLOCK,
	MAC_AX_SDIO_TX_MODE_DUMMY_AUTO,

	/* keep last */
	MAC_AX_SDIO_TX_MODE_LAST,
	MAC_AX_SDIO_TX_MODE_MAX = MAC_AX_SDIO_TX_MODE_LAST,
	MAC_AX_SDIO_TX_MODE_INVALID = MAC_AX_SDIO_TX_MODE_LAST,
};

/**
 * struct mac_ax_pkt_data - packet information of data type
 * @hdr_len: Length of header+LLC.
 *	For example,
 *	1. 802.11 MPDU without encryption
 *	    HEADERwLLC_LEN = (MAC header (without IV ) + LLC ) / 2
 *	2. 802.11 MPDU encryption without HW_AES_IV
 *	    HEADERwLLC_LEN = (MAC header (without IV ) + LLC ) / 2
 *	3.802.11MPDU encryption with HW_AES_IV
 *	   HEADERwLLC_LEN = (MAC header(reserved IV length)+LLC)/2
 *	4.ETHERNET II MSDU without encryption
 *	   HEADERwLLC_LEN = (DA+SA+TYPE) /2
 *	5.ETHERNET II MSDU encryption without HW_AES_IV
 *	   HEADERwLLC_LEN = (DA+SA+TYPE) /2
 *	6.ETHERNET II MSDU encryption with HW_AES_IV
 *	   HEADERwLLC_LEN = (DA+SA+TYPE) /2
 *	7.SNAP MSDU without encryption
 *	   HEADERwLLC_LEN = (DA+SA+LEN+LLC) /2
 *	8.SNAP MSDU encryption without HW_AES_IV
 *	   HEADERwLLC_LEN = (DA+SA+LEN+LLC) /2
 *	9.SNAP MSDU encryption with HW_AES_IV
 *	   HEADERwLLC_LEN = (DA+SA+LEN+LLC) /2
 * @ch: Channel index, MAC_AX_CH_DMA_CH0~MAC_AX_CH_DMA_CH11.
 * @macid: MAC ID.
 */
struct mac_ax_cctl_info {
// DWORD 0
	u32 DATARATE:9;
	u32 FORCE_TXOP:1;
	u32 DATA_BW:2;
	u32 DATA_GI_LTF:3;
	u32 DARF_TC_INDEX:1;
	u32 ARFR_CTRL:4;
	u32 ACQ_RPT_EN:1;
	u32 MGQ_RPT_EN:1;
	u32 ULQ_RPT_EN:1;
	u32 TWTQ_RPT_EN:1;
	u32 RSVD0:1;
	u32 DISRTSFB:1;
	u32 DISDATAFB:1;
	u32 TRYRATE:1;
	u32 AMPDU_DENSITY:4;

//DWORD 1
	u32 DATA_RTY_LOWEST_RATE:9;
	u32 AMPDU_TIME_SEL:1;
	u32 AMPDU_LEN_SEL:1;
	u32 RTS_TXCNT_LMT_SEL:1;
	u32 RTS_TXCNT_LMT:4;
	u32 RTSRATE:9;
	u32 RSVD1:2;
	u32 VCS_STBC:1;
	u32 RTS_RTY_LOWEST_RATE:4;

//DWORD 2
	u32 DATA_TX_CNT_LMT:6;
	u32 DATA_TXCNT_LMT_SEL:1;
	u32 MAX_AGG_NUM_SEL:1;
	u32 RTS_EN:1;
	u32 CTS2SELF_EN:1;
	u32 CCA_RTS:2;
	u32 HW_RTS_EN:1;
	u32 RTS_DROP_DATA_MODE:2;
	u32 RSVD2:1;
	u32 AMPDU_MAX_LEN:11;
	u32 UL_MU_DIS:1;
	u32 AMPDU_MAX_TIME:4;

//DWORD 3
	u32 MAX_AGG_NUM:8;
	u32 RSVD3:8;
	u32 VO_LFTIME_SEL:3;
	u32 VI_LFTIME_SEL:3;
	u32 BE_LFTIME_SEL:3;
	u32 BK_LFTIME_SEL:3;
	u32 SECTYPE:4;

//DWORD 4
	u32 MULTI_PORT_ID:3;
	u32 BMC:1;
	u32 MBSSID:4;
	u32 NAVUSEHDR:1;
	u32 TXPWR_MODE:3;
	u32 DATA_DCM:1;
	u32 DATA_ER:1;
	u32 DATA_LDPC:1;
	u32 DATA_STBC:1;
	u32 A_CTRL_BQR:1;
	u32 A_CTRL_UPH:1;
	u32 A_CTRL_BSR:1;
	u32 A_CTRL_CAS:1;
	u32 DATA_BW_ER:1;
	u32 LSIG_TXOP_EN:1;
	u32 RSVD4:5;
	u32 CTRL_CNT:4;

//DWORD 5
	u32 RESP_REF_RATE:9;
	u32 RSVD5:3;
	u32 ALL_ACK_SUPPORT:1;
	u32 BSR_QUEUE_SIZE_FORMAT:1;
	u32 RSVD6:1;
	u32 RSVD7:1;
	u32 NTX_PATH_EN:4;
	u32 PATH_MAP_A:2;
	u32 PATH_MAP_B:2;
	u32 PATH_MAP_C:2;
	u32 PATH_MAP_D:2;
	u32 ANTSEL_A:1;
	u32 ANTSEL_B:1;
	u32 ANTSEL_C:1;
	u32 ANTSEL_D:1;

//DWORD 6
	u32 ADDR_CAM_INDEX:8;
	u32 PAID:9;
	u32 ULDL:1;
	u32 DOPPLER_CTRL:2;
	u32 NOMINAL_PKT_PADDING:2;
	u32 RSVD8:2;
	u32 TXPWR_TOLERENCE:4;
	u32 RSVD9:4;

//DWORD 7
	u32 NC:3;
	u32 NR:3;
	u32 NG:2;
	u32 CB:2;
	u32 CS:2;
	u32 CSI_TXBF_EN:1;
	u32 CSI_STBC_EN:1;
	u32 CSI_LDPC_EN:1;
	u32 CSI_PARA_EN:1;
	u32 CSI_FIX_RATE:9;
	u32 CSI_GI_LTF:3;
	u32 RSVD10:1;
	u32 CSI_GID_SEL:1;
	u32 CSI_BW:2;
};

struct dctl_info {
// DWORD 0
	u32 QOS_FIELD_H:8;
	u32 HW_EXSEQ_MACID:7;
	u32 QOS_FIELD_H_EN:1;
	u32 AES_IV_L:16;

//DWORD 1
	u32 AES_IV_H:32;

//DWORD 2
	u32 SEQ0:12;
	u32 SEQ1:12;
	u32 AMSDU_MAX_LENGTH:3;
	u32 STA_AMSDU_EN:1;
	u32 CHKSUM_OFFLOAD_EN:1;
	u32 WITH_LLC:1;
	u32 RSVD0:1;
	u32 SEC_HW_ENC:1;

//DWORD 3
	u32 SEQ2:12;
	u32 SEQ3:12;
	u32 SEC_CAM_IDX:8;
};

struct mac_ax_pkt_data {
	u8 hdr_len;
	u8 ch;
	u8 macid;
	u8 agg_en;
	u8 max_agg_num;
	u8 lifetime_sel;
	u8 ampdu_density;
	u8 userate;
	u16 data_rate;
	u8 data_bw;
	u8 data_gi_ltf;
	u8 data_er;
	u8 data_dcm;
	u8 data_stbc;
	u8 data_ldpc;
	u8 dis_data_fb;
	u8 dis_rts_fb;
	u8 ac_type;
};

struct mac_ax_frame_info {
	u32 addr_1;
	u32 addr_2;
	u32 addr_3;
	enum mac_ax_pkt_t type;
	u8 pld_len;
};

struct mac_ax_txpkt_info {
	enum mac_ax_pkt_t type;
	u32 pktsize;
	union {
		struct mac_ax_pkt_data data;
	} u;
};

struct mac_ax_rpkt_data {
	u8 crc_err;
	u8 icv_err;
};

struct mac_ax_rxpkt_info {
	enum mac_ax_pkt_t type;
	u16 rxdlen;
	u32 pktsize;
	union {
		struct mac_ax_rpkt_data data;
	} u;
};

/**
 * struct mac_ax_role_info - role information
 * @macid: MAC ID.
 * @band: Band selection, band0 or band1.
 * @wmm: WMM selection, wmm0 ow wmm1.
 *	There are four sets about band and wmm,
 *	band0+wmm0, band0+wmm1, band1+wmm0,band1+wmm1.
 */
struct mac_ax_role_info {
	u8 macid;
	u8 band;
	u8 wmm;
};

struct mac_ax_hw_info {
	u8 done;
#define MAC_AX_CHIP_ID_8852A	0
	u8 chip_id;
#define MAC_AX_CHIP_CUT_A	0
#define MAC_AX_CHIP_CUT_B	1
#define MAC_AX_CHIP_CUT_C	2
#define MAC_AX_CHIP_CUT_D	3
#define MAC_AX_CHIP_CUT_E	4
#define MAC_AX_CHIP_CUT_F	5
#define MAC_AX_CHIP_CUT_G	6
#define MAC_AX_CHIP_CUT_TEST	30
	u8 chip_cut;
	enum mac_ax_intf intf;
	u8 tx_ch_num;
	u8 tx_data_ch_num;
	u8 wd_body_len;
	u8 wd_info_len;
	struct mac_pwr_cfg **pwr_on_seq;
	struct mac_pwr_cfg **pwr_off_seq;
	u8 pwr_seq_ver;
	u32 fifo_size;
	u16 macid_num;
	u32 wl_efuse_size;
	u32 efuse_size;
	u32 log_efuse_size;
	u32 bt_efuse_size;
	u32 sec_ctrl_efuse_size;
	u32 sec_data_efuse_size;
};

struct mac_ax_fw_info {
	u16 ver;
	u8 sub_ver;
	u8 sub_idex;
	u16 build_year;
	u16 build_mon;
	u16 build_date;
	u16 build_hour;
	u16 build_min;
};

struct mac_ax_efuse_param {
	u8 *efuse_map;
	u8 *bt_efuse_map;
	u8 *log_efuse_map;
	u32 efuse_end;
	u8 efuse_map_valid;
	u8 bt_efuse_map_valid;
	u8 log_efuse_map_valid;
	u8 auto_ck_en;
};

struct mac_ax_mac_pwr_info {
	u8 pwr_seq_proc;
	u8 pwr_in_lps;
};

struct mac_ax_sdio_info {
	enum mac_ax_sdio_4byte_mode sdio_4byte;
	enum mac_ax_sdio_tx_mode tx_mode;
	u16 block_size;
	u8 tx_seq;
};

struct mac_ax_sdio_tx_info {
	u32 size;
	u8 ch_dma;
	u8 dma_txagg_num;
};

struct mac_ax_pg_efuse_info {
	u8 *efuse_map;
	u32 efuse_map_size;
	u8 *efuse_mask;
	u32 efuse_mask_size;
};

struct mac_ax_tbl_hdr {
	u8 rw:1;
	u8 idx:7;
	u16 offset:5;
	u16 len:10;
	u16 type:1;
};

struct mac_ax_ru_rate_ent {
	u8 dcm:1;
	u8 ss:3;
	u8 mcs:4;
};

struct mac_ax_dl_fix_sta_ent {
	u8 mac_id;
	u8 ru_pos[3];
	u8 fix_rate:1;
	u8 fix_coding:1;
	u8 fix_txbf:1;
	u8 fix_pwr_fac:1;
	struct mac_ax_ru_rate_ent rate;
	u8 txbf:1;
	u8 coding:1;
	u8 pwr_boost_fac:5;
};

struct mac_ax_dlru_fixtbl {
	struct mac_ax_tbl_hdr tbl_hdr;
	u8 max_sta_num:3;
	u8 min_sta_num:3;
	u8 doppler:1;
	u8 stbc:1;
	u8 gi_ltf:3;
	u8 ma_type:1;
	u8 fixru_flag:1;
	struct mac_ax_dl_fix_sta_ent sta[MAC_AX_MAX_RU_NUM];
};

struct mac_ax_ul_fix_sta_ent {
	u8 mac_id;
	u8 ru_pos[3];
	u8 tgt_rssi[3];
	u8 fix_tgt_rssi: 1;
	u8 fix_rate: 1;
	u8 fix_coding: 1;
	u8 coding: 1;
	u8 rsvd1: 4;
	struct mac_ax_ru_rate_ent rate;
};

struct mac_ax_ulru_fixtbl {
	struct mac_ax_tbl_hdr tbl_hdr;
	u8 max_sta_num: 3;
	u8 min_sta_num: 3;
	u8 doppler: 1;
	u8 ma_type: 1;
	u8 gi_ltf: 3;
	u8 stbc: 1;
	u8 fix_tb_t_pe_nom: 1;
	u8 tb_t_pe_nom: 2;
	u8 fixru_flag: 1;
	u16 rsvd;
	struct mac_ax_ul_fix_sta_ent sta[MAC_AX_MAX_RU_NUM];
};

struct mac_ax_rura_report {
	u8 rt_tblcol: 6;
	u8 prtl_alloc: 1;
	u8 rate_chg: 1;
};

//for ul rua output
struct mac_ax_ulru_out_sta_ent {
	u8 dropping: 1;
	u8 tgt_rssi: 7;
	u8 mac_id;
	u8 ru_pos;
	u8 coding: 1;
	u8 vip_flag: 1;
	u8 rsvd1: 6;
	struct mac_ax_ru_rate_ent rate;
	struct mac_ax_rura_report rpt;
	u16 rsvs2;
};

struct mac_ax_ulrua_output {
	u8 ru2su: 1;
	u8 ppdu_bw: 2;
	u8 gi_ltf: 3;
	u8 stbc: 1;
	u8 doppler: 1;
	u16 rf_gain_fix: 1;
	u16 rf_gain_idx: 10;
	u16 tb_t_pe_nom: 2;
	u16 rsvd2: 3;
	u8 n_ltf_and_ma: 3;
	u8 sta_num: 4;
	u8 rsvd1: 1;

	u8 grp_mode: 1;
	u8 grp_id: 6;
	u8 fix_mode: 1;
	struct mac_ax_ulru_out_sta_ent sta[MAC_AX_MAX_RU_NUM];
};

struct mac_ul_macid_info {
	u8 macid;
	u8 pref_AC:2;
	u8 rsvd:6;
};

struct mac_ax_ul_fixinfo {
	struct mac_ax_tbl_hdr tbl_hdr;

	u16 ndpa_dur;
	u8 tf_type:3;
	u8 sig_ta_pkten:1;
	u8 sig_ta_pktsc:4;
	u8 murts_flag:1;
	u8 ndpa:2;
	u8 snd_pkt_sel:2;
	u8 gi_ltf:3;

	u32 data_rate:9;
	u32 data_er:1;
	u32 data_bw:2;
	u32 data_stbc:2;
	u32 data_ldpc:1;
	u32 data_dcm:1;
	u32 apep_len:12;
	u32 more_tf:1;
	u32 data_bw_er:1;
	u32 rsvd0:2;

	u16 multiport_id:3;
	u16 mbssid:4;
	u16 txpwr_mode:3;
	u16 rsvd1:6;
	u16 rsvd2;
	struct mac_ul_macid_info sta[4];
	struct mac_ax_ulrua_output ulrua;
};

struct mac_ax_mu_fixinfo {
	struct mac_ax_tbl_hdr tbl_hdr;
	u32 mu_thold_en:1; //T1 unit:us
	u32 mu_thold:30;
	u32 bypass_thold:1; //macid bypass tx time thold check
	u8 fix_mu_en:1; //macid bypass tx tp check
	u8 fix_mu_macid:7; //macid
};

struct mac_ax_ba_infotbl {
	struct mac_ax_tbl_hdr tbl_hdr;
	u32 fix_ba:1;
	u32 ru_psd:9;
	u32 tf_rate:9;
	u32 rf_gain_fix:1;
	u32 rf_gain_idx:10;
	u32 tb_ppdu_bw:2;
	struct mac_ax_ru_rate_ent rate;
	u8 gi_ltf:3;
	u8 doppler:1;
	u8 stbc:1;
	u8 sta_coding:1;
	u8 tb_t_pe_nom:2;
	u8 pr20_bw_en:1;
};

struct mac_ax_tf_ba {
	u32 fix_ba:1;
	u32 ru_psd:9;
	u32 tf_rate:9;
	u32 rf_gain_fix:1;
	u32 rf_gain_idx:10;
	u32 tb_ppdu_bw:2;
	struct mac_ax_ru_rate_ent rate;
	u8 gi_ltf:3;
	u8 doppler:1;
	u8 stbc:1;
	u8 sta_coding:1;
	u8 tb_t_pe_nom:2;
	u8 pr20_bw_en:1;
};

struct mac_ax_dl_ru_grptbl {
	struct mac_ax_tbl_hdr tbl_hdr;
	u16 ppdu_bw:2;
	u16 tx_pwr:9;
	u16 pwr_boost_fac:5;
	u8 fix_mode_flag:1;
	u8 rsvd1:7;
	u8 rsvd;
	struct mac_ax_tf_ba tf;
};

struct mac_ax_ul_ru_grptbl {
	struct mac_ax_tbl_hdr tbl_hdr;
	u32 grp_psd_max: 9;
	u32 grp_psd_min: 9;
	u32 tf_rate: 9;
	u32 fix_tf_rate: 1;
	u32 rsvd2: 4;
	u16 ppdu_bw: 2;
	u16 rf_gain_fix: 1;
	u16 rf_gain_idx: 10;
	u16 fix_mode_flag: 1;
	u16 rsvd1: 2;
};

struct mac_ax_bb_stainfo {
	struct mac_ax_tbl_hdr tbl_hdr;
//sta capability
	u8 gi_ltf_48spt:1;
	u8 gi_ltf_18spt:1;
	u8 rsvd3:6;
//downlink su
	u8 dlsu_info_en:1;
	u8 dlsu_bw:2;
	u8 dlsu_gi_ltf:3;
	u8 dlsu_doppler_ctrl:2;
	u8 dlsu_coding:1;
	u8 dlsu_txbf:1;
	u8 dlsu_stbc:1;
	u8 dl_fwcqi_flag:1;
	u8 dlru_ratetbl_ridx:4;
	u8 csi_info_bitmap;
	u32 dl_swgrp_bitmap;
	u16 dlsu_dcm:1;
	u16 rsvd1:6;
	u16 dlsu_rate:9;
	u8 dlsu_pwr:6;
	u8 rsvd2:2;
	u8 rsvd4;
//uplink su
	u8 ulsu_info_en:1;
	u8 ulsu_bw:2;
	u8 ulsu_gi_ltf:3;
	u8 ulsu_doppler_ctrl:2;
	u8 ulsu_dcm:1;
	u8 ulsu_ss:3;
	u8 ulsu_mcs:4;
	u16 ul_fwcqi_flag:1;
	u16 ulru_ratetbl_ridx:4;
	u16 ulsu_stbc:1;
	u16 ulsu_coding:1;
	u16 ulsu_rssi_m:9;
	u32 ul_swgrp_bitmap;
//tb info
};

struct mac_ax_adapter {
	struct mac_ax_ops *ops;
	void *drv_adapter;
	struct mac_ax_pltfm_cb *pltfm_cb;
	struct mac_ax_state_mach sm;
	struct mac_ax_hw_info *hw_info;
	struct mac_ax_fw_info *fw_info;
	struct mac_ax_efuse_param efuse_param;
	struct mac_ax_mac_pwr_info mac_pwr_info;
	struct mac_ax_sdio_info sdio_info;
};

struct mac_ax_pltfm_cb {
#if MAC_AX_SDIO_SUPPORT
	u8 (*sdio_cmd52_r8)(void *drv_adapter, u32 addr);
	u8 (*sdio_cmd53_r8)(void *drv_adapter, u32 addr);
	u16 (*sdio_cmd53_r16)(void *drv_adapter, u32 addr);
	u32 (*sdio_cmd53_r32)(void *drv_adapter, u32 addr);
	u8 (*sdio_cmd53_rn)(void *drv_adapter, u32 addr, u32 size, u8 *val);
	void (*sdio_cmd52_w8)(void *drv_adapter, u32 addr, u8 val);
	void (*sdio_cmd53_w8)(void *drv_adapter, u32 addr, u8 val);
	void (*sdio_cmd53_w16)(void *drv_adapter, u32 addr, u16 val);
	void (*sdio_cmd53_w32)(void *drv_adapter, u32 addr, u32 val);
	u8 (*sdio_cmd53_wn)(void *drv_adapter, u32 addr, u32 size, u8 *val);
	u8 (*sdio_cmd52_cia_r8)(void *drv_adapter, u32 addr);
#endif
#if (MAC_AX_USB_SUPPORT || MAC_AX_PCIE_SUPPORT)
	u8 (*reg_r8)(void *drv_adapter, u32 addr);
	u16 (*reg_r16)(void *drv_adapter, u32 addr);
	u32 (*reg_r32)(void *drv_adapter, u32 addr);
	void (*reg_w8)(void *drv_adapter, u32 addr, u8 val);
	void (*reg_w16)(void *drv_adapter, u32 addr, u16 val);
	void (*reg_w32)(void *drv_adapter, u32 addr, u32 val);
#endif
	u32 (*tx)(void *drv_adapter, u8 *buf, u32 len);
	void (*rtl_free)(void *drv_adapter, void *buf, u32 size);
	void* (*rtl_malloc)(void *drv_adapter, u32 size);
	void (*rtl_memcpy)(void *drv_adapter, void *dest, void *src, u32 size);
	void (*rtl_memset)(void *drv_adapter, void *addr, u8 val, u32 size);
	u32 (*rtl_memcmp)(void *drv_adapter, void *ptr1, void *ptr2, u32 num);
	void (*rtl_delay_us)(void *drv_adapter, u32 us);
	void (*rtl_delay_ms)(void *drv_adapter, u32 ms);

	void (*mutex_init)(void *drv_adapter, mac_ax_mutex *mutex);
	void (*mutex_deinit)(void *drv_adapter, mac_ax_mutex *mutex);
	void (*mutex_lock)(void *drv_adapter, mac_ax_mutex *mutex);
	void (*mutex_unlock)(void *drv_adapter, mac_ax_mutex *mutex);

	void (*msg_print)(void *drv_adapter, s8 *fmt, ...);
};

/**
 * mac_ax_intf_ops - interface related callbacks
 * @reg_read8:
 * @reg_write8:
 * @reg_read16:
 * @reg_write16:
 * @reg_read32:
 * @reg_write32:
 * @tx_allow_sdio:
 * @tx_cmd_addr_sdio:
 * @init_intf:
 */
struct mac_ax_intf_ops {
	u8 (*reg_read8)(struct mac_ax_adapter *adapter, u32 addr);
	void (*reg_write8)(struct mac_ax_adapter *adapter, u32 addr, u8 val);
	u16 (*reg_read16)(struct mac_ax_adapter *adapter, u32 addr);
	void (*reg_write16)(struct mac_ax_adapter *adapter, u32 addr, u16 val);
	u32 (*reg_read32)(struct mac_ax_adapter *adapter, u32 addr);
	void (*reg_write32)(struct mac_ax_adapter *adapter, u32 addr, u32 val);
	/**
	 * @tx_allow_sdio
	 * Only support SDIO interface. Using this API in other interface
	 * may cause system crash or segmentation fault.
	 */
	u32 (*tx_allow_sdio)(struct mac_ax_adapter *adapter,
			     struct mac_ax_sdio_tx_info *info);
	/**
	 * @tx_cmd_addr_sdio
	 * Only support SDIO interface. Using this API in other interface
	 * may cause system crash or segmentation fault.
	 */
	u32 (*tx_cmd_addr_sdio)(struct mac_ax_adapter *adapter,
				struct mac_ax_sdio_tx_info *info,
				u32 *cmd_addr);
	u32 (*intf_init)(struct mac_ax_adapter *adapter);
};

/**
 * struct mac_ax_ops - callbacks for mac control
 * All callbacks can be used after initializing mac_ax_ops by mac_ax_ops_init.
 * @intf_ops: interface related callbacks, refer struct mac_ax_intf_ops to get
 *	more deatails.
 * @get_hw_info: get mac hardware information
 * @mac_txdesc_len:
 * @build_mac_txdesc:
 */
struct mac_ax_ops {
	struct mac_ax_intf_ops *intf_ops;
	u32 (*pwr_switch)(struct mac_ax_adapter *adapter, u8 on);
	u32 (*sys_init)(struct mac_ax_adapter *adapter);
	u32 (*trx_init)(struct mac_ax_adapter *adapter,
			enum mac_ax_trx_mode mode);
	u32 (*fwdl)(struct mac_ax_adapter *adapter, u8 *fw, u32 len);
	u32 (*romdl)(struct mac_ax_adapter *adapter, u8 *rom, u32 romaddr,
		     u32 len);
	struct mac_ax_hw_info* (*get_hw_info)(struct mac_ax_adapter *adapter);
	u32 (*add_role)(struct mac_ax_adapter *adapter,
			struct mac_ax_role_info *info);
	u32 (*remove_role)(struct mac_ax_adapter *adapter, u8 macid);
	u32 (*txdesc_len)(struct mac_ax_adapter *adapter,
			  struct mac_ax_txpkt_info *info);
	u32 (*build_txdesc)(struct mac_ax_adapter *adapter,
			    struct mac_ax_txpkt_info *info, u8 *buf, u32 len);
	u32 (*parse_rxdesc)(struct mac_ax_adapter *adapter,
			    struct mac_ax_rxpkt_info *info, u8 *buf, u32 len);
	u32 (*process_c2h)(struct mac_ax_adapter *adapter, u8 *buf, u32 len);
	u32 (*upd_dlru_fixtbl)(struct mac_ax_adapter *adapter,
			       struct mac_ax_dlru_fixtbl *info);
	u32 (*upd_dlru_grptbl)(struct mac_ax_adapter *adapter,
			       struct mac_ax_dl_ru_grptbl *info);
	u32 (*upd_ulru_fixtbl)(struct mac_ax_adapter *adapter,
			       struct mac_ax_ulru_fixtbl *info);
	u32 (*upd_ulru_grptbl)(struct mac_ax_adapter *adapter,
			       struct mac_ax_ul_ru_grptbl *info);
	u32 (*upd_rusta_info)(struct mac_ax_adapter *adapter,
				  struct mac_ax_bb_stainfo *info);
	u32 (*upd_ba_infotbl)(struct mac_ax_adapter *adapter,
			      struct mac_ax_ba_infotbl *info);
	u32 (*upd_mu_info)(struct mac_ax_adapter *adapter,
			   struct mac_ax_mu_fixinfo *info);
	u32 (*upd_ul_fixinfo)(struct mac_ax_adapter *adapter,
			      struct mac_ax_ul_fixinfo *info);
	u32 (*dump_efuse_map_wl)(struct mac_ax_adapter *adapter,
				 enum mac_ax_efuse_read_cfg cfg);
	u32 (*dump_efuse_map_bt)(struct mac_ax_adapter *adapter);
	u32 (*write_efuse)(struct mac_ax_adapter *adapter, u32 addr, u8 val,
			   enum mac_ax_efuse_bank bank);
	u32 (*read_efuse)(struct mac_ax_adapter *adapter, u32 addr, u32 size,
			  u8 *val, enum mac_ax_efuse_bank bank);
	u32 (*get_efuse_avl_size)(struct mac_ax_adapter *adapter, u32 *size);
	u32 (*dump_log_efuse)(struct mac_ax_adapter *adapter,
			      enum mac_ax_efuse_parser_cfg parser_cfg,
			      enum mac_ax_efuse_read_cfg cfg);
	u32 (*read_log_efuse)(struct mac_ax_adapter *adapter, u32 addr,
			      u32 size, u8 *val);
	u32 (*write_log_efuse)(struct mac_ax_adapter *adapter, u32 addr,
			       u8 val);
	u32 (*pg_efuse_by_map)(struct mac_ax_adapter *adapter,
			       struct mac_ax_pg_efuse_info *info,
			       enum mac_ax_efuse_read_cfg cfg);
	u32 (*mask_log_efuse)(struct mac_ax_adapter *adapter,
			      struct mac_ax_pg_efuse_info *info);
	u32 (*pg_sec_data_by_map)(struct mac_ax_adapter *adapter,
				  struct mac_ax_pg_efuse_info *info);
	u32 (*cmp_sec_data_by_map)(struct mac_ax_adapter *adapter,
				   struct mac_ax_pg_efuse_info *info);
	u32 (*gen_tx_frame)(struct mac_ax_adapter *adapter,
			    struct mac_ax_frame_info *info, u8 *mem);
#if MAC_AX_FEATURE_DBGPKG
	u32 (*fwcmd_lb)(struct mac_ax_adapter *adapter, u32 len, u8 burst);
	u32 (*mem_dump)(struct mac_ax_adapter *adapter, enum mac_ax_mem_sel sel,
			u32 strt_addr, u32 size, u8 *data);
#endif

#if MAC_AX_FEATURE_HV
#endif
};

#endif
