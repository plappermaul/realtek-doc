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

#ifndef _MAC_AX_TXDESC_H_
#define _MAC_AX_TXDESC_H_

// WD body //
/* dword0 */
#define TXD_EN_HWSEQ_MODE_SH	0
#define TXD_EN_HWSEQ_MODE_MSK	0x3
#define TXD_HW_SSN_SEL_SH	2
#define TXD_HW_SSN_SEL_MSK	0x3
#define TXD_SMH_EN		BIT(4)
#define TXD_HWAMSDU		BIT(5)
#define TXD_HW_AES_IV		BIT(6)
#define TXD_WD_PAGE		BIT(7)
#define TXD_CHK_EN		BIT(8)
#define TXD_WP_INT		BIT(9)
#define TXD_STF_MODE		BIT(10)
#define TXD_HDR_LLC_LEN_SH	11
#define TXD_HDR_LLC_LEN_MSK	0x1f
#define TXD_CH_DMA_SH		16
#define TXD_CH_DMA_MSK		0xf
#define TXD_FWDL_EN		BIT(20)
#define TXD_PKT_OFFSET		BIT(21)
#define TXD_WDINFO_EN		BIT(22)
#define TXD_MOREDATA		BIT(23)
#define TXD_WP_OFFSET_SH	24
#define TXD_WP_OFFSET_MSK	0xff

/* dword1 */
#define TXD_SHCUT_CAMID_SH	0
#define TXD_SHCUT_CAMID_MSK	0xff
#define TXD_DMA_TXAGG_NUM_SH	8
#define TXD_DMA_TXAGG_NUM_MSK	0xff
#define TXD_PLD_SH		16
#define TXD_PLD_MSK		0xffff

/* dword2 */
#define TXD_TXPKTSIZE_SH	0
#define TXD_TXPKTSIZE_MSK	0xffff
#define TXD_RU_TC_SH		14
#define TXD_RU_TC_MSK		0x7
#define TXD_QSEL_SH		17
#define TXD_QSEL_MSK		0x3f
#define TXD_TID_IND		BIT(23)
#define TXD_MACID_SH		24
#define TXD_MACID_MSK		0x7f

/* dword3 */
#define TXD_WIFI_SEQ_SH		0
#define TXD_WIFI_SEQ_MSK	0xfff
#define TXD_AGG_EN		BIT(12)
#define TXD_BK			BIT(13)
#define TXD_RTS_TC_SH		14
#define TXD_RTS_TC_MSK		0x3f
#define TXD_DATA_TC_SH		20
#define TXD_DATA_TC_MSK		0x3f
#define TXD_MU_2ND_TC_SH	26
#define TXD_MU_2ND_TC_MSK	0x7
#define TXD_MU_TC_SH		29
#define TXD_MU_TC_MSK		0x7

/* dword4 */
#define TXD_TXDESC_CHECKSUM_SH	0
#define TXD_TXDESC_CHECKSUM_MSK	0xffff
#define TXD_AES_IV_L_SH		16
#define TXD_AES_IV_L_MSK	0xffff

/* dword5 */
#define TXD_AES_IV_H_SH		0
#define TXD_AES_IV_H_MSK	0xffffffff

// WD info //
/* dword6 */
#define TXD_MBSSID_SH		0
#define TXD_MBSSID_MSK		0xf
#define TXD_MULTIPORT_ID_SH	4
#define TXD_MULTIPORT_ID_MSK	0x7
#define TXD_DISRTSFB		BIT(9)
#define TXD_DISDATAFB		BIT(10)
#define TXD_DATA_LDPC		BIT(11)
#define TXD_DATA_STBC_SH	12
#define TXD_DATA_STBC_MSK	0x3
#define TXD_DATA_DCM		BIT(14)
#define TXD_DATA_ER		BIT(15)
#define TXD_DATARATE_SH		16
#define TXD_DATARATE_MSK	0x1ff
#define TXD_GI_LTF_SH		25
#define TXD_GI_LTF_MSK		0x7
#define TXD_DATA_BW_SH		28
#define TXD_DATA_BW_MSK		0x3
#define TXD_USERATE_SEL		BIT(30)
#define TXD_ACK_CH_INFO		BIT(31)

/* dword7 */
#define TXD_MAX_AGG_NUM_SH	0
#define TXD_MAX_AGG_NUM_MSK	0xff
#define TXD_BCN_SRCH_SEQ_SH	8
#define TXD_BCN_SRCH_SEQ_MSK	0x3
#define TXD_NAVUSEHDR		BIT(10)
#define TXD_BMC			BIT(11)
#define TXD_A_CTRL_BQR		BIT(12)
#define TXD_A_CTRL_UPH		BIT(13)
#define TXD_A_CTRL_BSR		BIT(14)
#define TXD_A_CTRL_CAS		BIT(15)
#define TXD_DATA_RTY_LOWEST_RATE_SH	16
#define TXD_DATA_RTY_LOWEST_RATE_MSK	0x1ff
#define TXD_DATA_TXCNT_LMT_SH	25
#define TXD_DATA_TXCNT_LMT_MSK	0x3f
#define TXD_DATA_TXCNT_LMT_SEL	BIT(31)

/* dword8 */
#define TXD_SEC_CAM_IDX_SH	0
#define TXD_SEC_CAM_IDX_MSK	0xff
#define TXD_SEC_HW_ENC		BIT(8)
#define TXD_SECTYPE_SH		9
#define TXD_SECTYPE_MSK		0xf
#define TXD_LIFETIME_SEL_SH	13
#define TXD_LIFETIME_SEL_MSK	0x7
#define TXD_FORCE_TXOP		BIT(17)
#define TXD_AMPDU_DENSITY_SH	18
#define TXD_AMPDU_DENSITY_MSK	0x7
#define TXD_LSIG_TXOP_EN	BIT(21)
#define TXD_TXPWR_OFSET_TYPE_SH		22
#define TXD_TXPWR_OFSET_TYPE_MSK	0x7

/* dword9 */
#define TXD_SIGNALING_TA_PKT_EN	BIT(0)
#define TXD_NDPA_SH		1
#define TXD_NDPA_MSK		0x3
#define TXD_SND_PKT_SEL_SH	3
#define TXD_SND_PKT_SEL_MSK	0x3
#define TXD_SIFS_TX		BIT(5)
#define TXD_HT_DATA_SND		BIT(6)
#define TXD_RTT_EN		BIT(9)
#define TXD_SPE_RPT		BIT(10)
#define TXD_BT_NULL		BIT(11)
#define TXD_TRI_FRAME		BIT(12)
#define TXD_NULL_1		BIT(13)
#define TXD_NULL_0		BIT(14)
#define TXD_RAW			BIT(15)
#define TXD_GROUP_BIT_IE_OFFSET_SH	16
#define TXD_GROUP_BIT_IE_OFFSET_MSK	0xff
#define TXD_BCNPKT_TSF_CTRL		BIT(26)
#define TXD_SIGNALING_TA_PKT_SC_SH	27
#define TXD_SIGNALING_TA_PKT_SC_MSK	0xf
#define TXD_FORCE_BSS_CLR		BIT(31)

/* dword10 */

/* dword11 */

// WP Info & ADDR info//
/* dword12 */
#define TXD_PCIE_SEQ_NUM_0_SH	0
#define TXD_PCIE_SEQ_NUM_0_MSK	0x7fff
#define TXD_VALID_0		BIT(15)
#define TXD_PCIE_SEQ_NUM_1_SH	16
#define TXD_PCIE_SEQ_NUM_1_MSK	0x7fff
#define TXD_VALID_1		BIT(31)

/* dword13 */
#define TXD_PCIE_SEQ_NUM_2_SH	0
#define TXD_PCIE_SEQ_NUM_2_MSK	0x7fff
#define TXD_VALID_2		BIT(15)
#define TXD_PCIE_SEQ_NUM_3_SH	16
#define TXD_PCIE_SEQ_NUM_3_MSK	0x7fff
#define TXD_VALID_3		BIT(31)
#endif
