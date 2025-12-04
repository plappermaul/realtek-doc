
/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef _DW_NE_BASE_H_
#define _DW_NE_BASE_H_

#include "hal_api.h"
#include "dw_ne_base_reg.h"
#include "dw_ne_base_bit.h"


/* *****************************************************************
*							Definition 
*******************************************************************/
/* For Endpoint: RTL8168 */

#if 0
// Used Memory 
#define PCIE0_TX_DESC_ADDR	0xA0520000
#define PCIE0_RX_DESC_ADDR	0xA0530000

#define PCIE0_TX_BUFF_ADDR	0xA0580000
#define PCIE0_RX_BUFF_ADDR	0xA05A0000

#define PCIE1_TX_DESC_ADDR	0xA0620000
#define PCIE1_RX_DESC_ADDR	0xA0630000

#define PCIE1_TX_BUFF_ADDR	0xA0680000
#define PCIE1_RX_BUFF_ADDR	0xA06A0000

#define PCIE_NUM_TX_DESC	64
#define PCIE_NUM_RX_DESC	64

#define PCIE_TX_DESC_SIZE	16
#define PCIE_RX_DESC_SIZE	16

#define PCIE_TX_BUFF_SIZE	4096
#define PCIE_RX_BUFF_SIZE	4096

// RTL8168 Register
enum RTL8168_REGISTER {
	MAC0 = 0,		/* Ethernet hardware address. */
	MAC4 = 0x04,
	MAR0 = 8,		/* Multicast filter. */
	CounterAddrLow = 0x10,
	CounterAddrHigh = 0x14,
	TxDescStartAddrLow = 0x20,
	TxDescStartAddrHigh = 0x24,
	TxHDescStartAddrLow = 0x28,
	TxHDescStartAddrHigh = 0x2c,
	FLASH = 0x30,
	ERSR = 0x36,
	ChipCmd = 0x37,
	TxPoll = 0x38,
	IntrMask = 0x3C,
	IntrStatus = 0x3E,
	TxConfig = 0x40,
	RxConfig = 0x44,
	TCTR = 0x48,
	Cfg9346 = 0x50,
	Config0 = 0x51,
	Config1 = 0x52,
	Config2 = 0x53,
	Config3 = 0x54,
	Config4 = 0x55,
	Config5 = 0x56,
	TimeIntr = 0x58,
	PHYAR = 0x60,
	CSIDR = 0x64,
	CSIAR = 0x68,
	PHYstatus = 0x6C,
	EPHYAR = 0x80,
	DBG_reg = 0xD1,
	RxMaxSize = 0xDA,
	CPlusCmd = 0xE0,
	IntrMitigate = 0xE2,
	RxDescAddrLow = 0xE4,
	RxDescAddrHigh = 0xE8,
	EarlyTxThres = 0xEC,
	FuncEvent = 0xF0,
	FuncEventMask = 0xF4,
	FuncPresetState = 0xF8,
	FuncForceEvent = 0xFC,
};

enum RTL8168_REGISTER_CONTENT {
	/* InterruptStatusBits */
	SYSErr		= 0x8000,
	PCSTimeout	= 0x4000,
	SWInt		= 0x0100,
	TxDescUnavail	= 0x0080,
	RxFIFOOver	= 0x0040,
	LinkChg		= 0x0020,
	RxDescUnavail	= 0x0010,
	TxErr		= 0x0008,
	TxOK		= 0x0004,
	RxErr		= 0x0002,
	RxOK		= 0x0001,

	/* RxStatusDesc */
	RxRWT = (1 << 22),
	RxRES = (1 << 21),
	RxRUNT = (1 << 20),
	RxCRC = (1 << 19),

	/* ChipCmdBits */
	StopReq  = 0x80,
	CmdReset = 0x10,
	CmdRxEnb = 0x08,
	CmdTxEnb = 0x04,
	RxBufEmpty = 0x01,

	/* Cfg9346Bits */
	Cfg9346_Lock = 0x00,
	Cfg9346_Unlock = 0xC0,

	/* rx_mode_bits */
	AcceptErr = 0x20,
	AcceptRunt = 0x10,
	AcceptBroadcast = 0x08,
	AcceptMulticast = 0x04,
	AcceptMyPhys = 0x02,
	AcceptAllPhys = 0x01,

	/* Transmit Priority Polling*/
	HPQ = 0x80,
	NPQ = 0x40,
	FSWInt = 0x01,

	/* RxConfigBits */
	RxCfgFIFOShift = 13,
	RxCfgDMAShift = 8,

	/* ghhuang */
	RxDMA16B  = 0x000,
	RxDMA32B  = 0x100,
	RxDMA64B  = 0x200,
	RxDMA128B = 0x300,
	RxDMA256B = 0x400,
	RxDMA512B = 0x500,
	RxDMA1KB  = 0x600,
	RxDMAall  = 0x700,

	/* TxConfigBits */
	TxInterFrameGapShift = 24,
	TxDMAShift = 8,	/* DMA burst value (0-7) is shift this many bits */
	TxMACLoopBack = (1 << 17),	/* MAC loopback */

	TxDMA16B  = 0x000,
	TxDMA32B  = 0x100,
	TxDMA64B  = 0x200,
	TxDMA128B = 0x300,
	TxDMA256B = 0x400,
	TxDMA512B = 0x500,
	TxDMA1KB  = 0x600,
	TxDMAall  = 0x700,

	/* Config1 register p.24 */
	LEDS1		= (1 << 7),
	LEDS0		= (1 << 6),
	Speed_down	= (1 << 4),
	MEMMAP		= (1 << 3),
	IOMAP		= (1 << 2),
	VPD		= (1 << 1),
	PMEnable	= (1 << 0),	/* Power Management Enable */

	/* Config3 register */
	MagicPacket	= (1 << 5),	/* Wake up when receives a Magic Packet */
	LinkUp		= (1 << 4),	/* This bit is reserved in RTL8168B.*/
					/* Wake up when the cable connection is re-established */
	ECRCEN		= (1 << 3),	/* This bit is reserved in RTL8168B*/
	Jumbo_En	= (1 << 2),	/* This bit is reserved in RTL8168B*/
	RDY_TO_L23	= (1 << 1),	/* This bit is reserved in RTL8168B*/
	Beacon_en	= (1 << 0),	/* This bit is reserved in RTL8168B*/

	/* Config4 register */
	LANWake		= (1 << 1),	/* This bit is reserved in RTL8168B*/

	/* Config5 register */
	BWF		= (1 << 6),	/* Accept Broadcast wakeup frame */
	MWF		= (1 << 5),	/* Accept Multicast wakeup frame */
	UWF		= (1 << 4),	/* Accept Unicast wakeup frame */
	LanWake		= (1 << 1),	/* LanWake enable/disable */
	PMEStatus	= (1 << 0),	/* PME status can be reset by PCI RST# */

	/* CPlusCmd */
	EnableBist	= (1 << 15),
	Macdbgo_oe	= (1 << 14),
	Normal_mode	= (1 << 13),
	Force_halpdup	= (1 << 12),
	Force_rxflow_en	= (1 << 11),
	Force_txflow_en	= (1 << 10),
	Cxpl_dbg_sel	= (1 << 9),//This bit is reserved in RTL8168B
	ASF		= (1 << 8),//This bit is reserved in RTL8168C
	PktCntrDisable	= (1 << 7),
	RxVlan		= (1 << 6),
	RxChkSum	= (1 << 5),
	Macdbgo_sel	= 0x001C,
	INTT_0		= 0x0000,
	INTT_1		= 0x0001,
	INTT_2		= 0x0002,
	INTT_3		= 0x0003,

	/* rtl8168_PHYstatus */
	TxFlowCtrl = 0x40,
	RxFlowCtrl = 0x20,
	//_1000bpsF = 0x10,
	_100bps = 0x08,
	_10bps = 0x04,
	LinkStatus = 0x02,
	FullDup = 0x01,

	/* DBG_reg */
	Fix_Nak_1 = (1 << 4),
	Fix_Nak_2 = (1 << 3),
	DBGPIN_E2 = (1 << 0),

	/* DumpCounterCommand */
	CounterDump = 0x8,

	/* PHY access */
	PHYAR_Flag = 0x80000000,
	PHYAR_Write = 0x80000000,
	PHYAR_Read = 0x00000000,
	PHYAR_Reg_Mask = 0x1f,
	PHYAR_Reg_shift = 16,
	PHYAR_Data_Mask = 0xffff,

	/* EPHY access */
	EPHYAR_Flag = 0x80000000,
	EPHYAR_Write = 0x80000000,
	EPHYAR_Read = 0x00000000,
	EPHYAR_Reg_Mask = 0x1f,
	EPHYAR_Reg_shift = 16,
	EPHYAR_Data_Mask = 0xffff,

	/* CSI access */	
	CSIAR_Flag = 0x80000000,
	CSIAR_Write = 0x80000000,
	CSIAR_Read = 0x00000000,
	CSIAR_ByteEn = 0x0f,
	CSIAR_ByteEn_shift = 12,
	CSIAR_Addr_Mask = 0x0fff,
};

enum _RTL8168_DescStatusBit {
	DescOwn		= (1 << 31), /* Descriptor is owned by NIC */
	RingEnd		= (1 << 30), /* End of descriptor ring */
	FirstFrag	= (1 << 29), /* First segment of a packet */
	LastFrag	= (1 << 28), /* Final segment of a packet */

	/* Tx private */
	/*------ offset 0 of tx descriptor ------*/
	LargeSend	= (1 << 27), /* TCP Large Send Offload (TSO) */
	MSSShift	= 16,        /* MSS value position */
	MSSMask		= 0xfff,     /* MSS value + LargeSend bit: 12 bits */
	TxIPCS		= (1 << 18), /* Calculate IP checksum */
	TxUDPCS		= (1 << 17), /* Calculate UDP/IP checksum */
	TxTCPCS		= (1 << 16), /* Calculate TCP/IP checksum */
	TxVlanTag	= (1 << 17), /* Add VLAN tag */

	/*@@@@@@ offset 4 of tx descriptor => bits for RTL8168C/CP only		begin @@@@@@*/
	TxUDPCS_C	= (1 << 31), /* Calculate UDP/IP checksum */
	TxTCPCS_C	= (1 << 30), /* Calculate TCP/IP checksum */
	TxIPCS_C	= (1 << 29), /* Calculate IP checksum */
	/*@@@@@@ offset 4 of tx descriptor => bits for RTL8168C/CP only		end @@@@@@*/


	/* Rx private */
	/*------ offset 0 of rx descriptor ------*/
	PID1		= (1 << 18), /* Protocol ID bit 1/2 */
	PID0		= (1 << 17), /* Protocol ID bit 2/2 */
	

	RxIPF		= (1 << 16), /* IP checksum failed */
	RxUDPF		= (1 << 15), /* UDP/IP checksum failed */
	RxTCPF		= (1 << 14), /* TCP/IP checksum failed */
	RxVlanTag	= (1 << 16), /* VLAN tag available */

	/*@@@@@@ offset 0 of rx descriptor => bits for RTL8168C/CP only		begin @@@@@@*/
	RxUDPT		= (1 << 18),
	RxTCPT		= (1 << 17),
	/*@@@@@@ offset 0 of rx descriptor => bits for RTL8168C/CP only		end @@@@@@*/

	/*@@@@@@ offset 4 of rx descriptor => bits for RTL8168C/CP only		begin @@@@@@*/
	RxV6F		= (1 << 31),
	RxV4F		= (1 << 30),
	/*@@@@@@ offset 4 of rx descriptor => bits for RTL8168C/CP only		end @@@@@@*/
};


/* PCIE Module Selection */
typedef enum _PCIE_MODULE_SEL_ {
	PCIE_MODULE_SEL_0 = 0,
	PCIE_MODULE_SEL_1 = 1,
	PCIE_MODULE_SEL_2 = 2,
	PCIE_MODULE_SEL_3 = 3
}PCIE_MODULE_SEL,*PPCIE_MODULE_SEL;


/* PCIE EP Rx Status */
typedef enum _PCIE_EP_RX_STATUS_ {
	PCIE_EP_RX_STATUS_FAIL 		= -1,
	PCIE_EP_RX_STATUS_SUCCESS 	= 0,
}PCIE_EP_RX_STATUS,*PPCIE_EP_RX_STATUS;
#endif

/* *****************************************************************
*							Function prototype
*******************************************************************/
void	HalPCIeRCConfigWrite32(IN u32 PCIeIdx, IN u32 PCIeReg, IN u32 PCIeVal);
u32		HalPCIeRCConfigRead32(IN u32 PCIeIdx, IN u32 PCIeReg);
void	HalPCIeRCConfigWrite8(IN u32 PCIeIdx, IN u32 PCIeReg, IN u8 PCIeVal);
u8		HalPCIeRCConfigRead8(IN u32 PCIeIdx, IN u32 PCIeReg);

void	HalPCIeRCExtendConfigWrite32(IN u32 PCIeIdx, IN u32 PCIeReg, IN u32 PCIeVal);
u32		HalPCIeRCExtendConfigRead32(IN u32 PCIeIdx, IN u32 PCIeReg);

void	HalPCIeEPConfigWrite32(IN u32 PCIeIdx, IN u32 PCIeReg, IN u32 PCIeVal);
u32		HalPCIeEPConfigRead32(IN u32 PCIeIdx, IN u32 PCIeReg);
void	HalPCIeEPConfigWrite8(IN u32 PCIeIdx, IN u32 PCIeReg, IN u8 PCIeVal);
u8		HalPCIeEPConfigRead8(IN u32 PCIeIdx, IN u32 PCIeReg);

void	HalPCIeEPMemWrite32(IN u32 PCIeIdx, IN u32 PCIeReg, IN u32 PCIeVal);
u32		HalPCIeEPMemRead32(IN u32 PCIeIdx, IN u32 PCIeReg);
void	HalPCIeEPMemWrite8(IN u32 PCIeIdx, IN u32 PCIeReg, IN u8 PCIeVal);
u8		HalPCIeEPMemRead8(IN u32 PCIeIdx, IN u32 PCIeReg);


void	PCIeEPInitDWCommon(IN void *Data);
void	PCIeEPTxDWCommon(IN void *Data);
int		PCIeEPRxDWCommon(IN void *Data);
void	PCIeMDIOResetDWCommon(IN void *Data);
void	PCIePHYResetDWCommon(IN void *Data);
void	PCIeMDIOWriteDWCommon(IN u32 PCIeIdx, IN u32 mdioRegAddr, IN u32 mdioData);
void	PCIeCheckLinkDWCommon(IN void *Data);

void	PCIeMDIOPHYParameterSetRLE0269B(IN void *Data);
void	PCIeMDIOPHYParameterSetRLE0269C(IN void *Data);
void	PCIeMDIOPHYParameterSetRLE0600(IN void *Data);
void	PCIeMDIOPHYParameterSetRLE0591(IN void *Data);


#endif //#ifndef _PCIE_BASE_H_

