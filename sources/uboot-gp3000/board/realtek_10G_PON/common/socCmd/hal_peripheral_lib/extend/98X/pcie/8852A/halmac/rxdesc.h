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

#ifndef _MAC_AX_RXDESC_H_
#define _MAC_AX_RXDESC_H_

/* dword0 */
#define RXD_RPKT_LEN_SH			0
#define RXD_RPKT_LEN_MSK		0x3FFF
#define RXD_SHIFT_SH			14
#define RXD_SHIFT_MSK			0x3
#define RXD_WL_HD_IV_LEN_SH		16
#define RXD_WL_HD_IV_LEN_MSK		0x3f
#define RXD_BB_SEL			BIT(22)
#define RXD_MAC_INFO_VLD		BIT(23)
#define RXD_RPKT_TYPE_SH		24
#define RXD_RPKT_TYPE_MSK		0xf
#define RXD_DRV_INFO_SIZE_SH		28
#define RXD_DRV_INFO_SIZE_MSK		0x7
#define RXD_LONG_RXD			BIT(31)

#define RXD_S_RPKT_TYPE_WIFI		0
#define RXD_S_RPKT_TYPE_PPDU		1
#define RXD_S_RPKT_TYPE_CH_INFO		2
#define RXD_S_RPKT_TYPE_BB_SCORE	3
#define RXD_S_RPKT_TYPE_TXCMD_RPT	4
#define RXD_S_RPKT_TYPE_SS2FW_RPT	5
#define RXD_S_RPKT_TYPE_TXRPT		6
#define RXD_S_RPKT_TYPE_PLDREL_HOST	7
#define RXD_S_RPKT_TYPE_DFS_RPT		8
#define RXD_S_RPKT_TYPE_PLDREL_WLCPU	9
#define RXD_S_RPKT_TYPE_C2H		10

/* dword1 */
#define RXD_PPDU_TYPE_SH		0
#define RXD_PPDU_TYPE_MSK		0xf
#define RXD_PPDU_CNT_SH			4
#define RXD_PPDU_CNT_MSK		0x7
#define RXD_SR_EN			BIT(7)
#define RXD_USER_ID_SH			8
#define RXD_USER_ID_MSK			0xff
#define RXD_RX_DATARATE_SH		16
#define RXD_RX_DATARATE_MSK		0x1ff
#define RXD_RX_GI_LTF_SH		25
#define RXD_RX_GI_LTF_MSK		0x7
#define RXD_NON_SRG_PPDU		BIT(28)
#define RXD_INTER_PPDU			BIT(29)
#define RXD_BW_SH			30
#define RXD_BW_MSK			0x3

/* dword2 */
#define RXD_FREERUN_CNT_SH		0
#define RXD_FREERUN_CNT_MSK		0xffffffff

/* dword3 */
#define RXD_AMPDU_END_PKT		BIT(4)
#define RXD_AMSDU			BIT(5)
#define RXD_AMSDU_CUT			BIT(6)
#define RXD_LAST_MSDU			BIT(7)
#define RXD_BYPASS			BIT(8)
#define RXD_CRC32			BIT(9)
#define RXD_ICVERR			BIT(10)
#define RXD_MAGIC_WAKE			BIT(11)
#define RXD_UNICAST_WAKE		BIT(12)
#define RXD_PATTERN_WAKE		BIT(13)
#define RXD_GET_CH_INFO_SH		14
#define RXD_GET_CH_INFO_MSK		0x3
#define RXD_PATTERN_IDX_SH		16
#define RXD_PATTERN_IDX_MSK		0x1f
#define RXD_TARGET_IDC_SH		21
#define RXD_TARGET_IDC_MSK		0x7
#define RXD_CHKSUM_OFFLOAD_EN		BIT(24)
#define RXD_WITH_LLC			BIT(25)
#define RXD_RX_STATS			BIT(26)

/* dword4 */
#define RXD_TYPE_SH			0
#define RXD_TYPE_MSK			0x3
#define RXD_MC				BIT(2)
#define RXD_BC				BIT(3)
#define RXD_MD				BIT(4)
#define RXD_MF				BIT(5)
#define RXD_PWR				BIT(6)
#define RXD_QOS				BIT(7)
#define RXD_TID_SH			8
#define RXD_TID_MSK			0xf
#define RXD_EOSP			BIT(12)
#define RXD_HTC				BIT(13)
#define RXD_QNULL			BIT(14)
#define RXD_SEQ_SH			16
#define RXD_SEQ_MSK			0xfff
#define RXD_FRAG_SH			28
#define RXD_FRAG_MSK			0xf

/* dword5 */
#define RXD_SEC_CAM_IDX_SH		0
#define RXD_SEC_CAM_IDX_MSK		0xff
#define RXD_ADDR_CAM_SH			8
#define RXD_ADDR_CAM_MSK		0xff
#define RXD_MAC_ID_SH			16
#define RXD_MAC_ID_MSK			0xff
#define RXD_RX_PL_ID_SH			24
#define RXD_RX_PL_ID_MSK		0xf
#define RXD_ADDR_CAM_VLD		BIT(28)
#define RXD_ADDR_FWD_EN			BIT(29)
#define RXD_RX_PL_MATCH			BIT(30)

/* dword6 */
#define RXD_MAC_ADDR_SH			0
#define RXD_MAC_ADDR_MSK		0xffffffff

/* dword7 */
#define RXD_MAC_ADDR_H_SH		0
#define RXD_MAC_ADDR_H_MSK		0xffff
#define RXD_SMART_ANT			BIT(16)

#define RXD_CRC32_ERR			BIT(9)
#define RXD_ICV_ERR			BIT(10)

#endif
