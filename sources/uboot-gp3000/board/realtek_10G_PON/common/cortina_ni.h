/***********************************************************************/
/* This file contains unpublished documentation and software           */
/* proprietary to Cortina Access Incorporated. Any use or disclosure,  */
/* in whole or in part, of the information in this file without a      */
/* written consent of an officer of Cortina Access Incorporated is     */
/* strictly prohibited.                                                */
/* Copyright (c) 2024 by Cortina Access Incorporated.                  */
/***********************************************************************/
/*
 *
 * cortina_ni.h
 *
 */
#ifndef __CORTINA_NI_H
#define __CORTINA_NI_H

#include <asm/types.h>
#include <asm/io.h>
#include <config.h>

//#define CORTINA_NI_DBG 1

#ifdef CS_BIG_ENDIAN
#	define CRCPOLY_BE 0x04c11db7
#else /* CS_LITTLE_ENDIAN */
#	define CRCPOLY_LE 0xedb88320
#endif

#define CS_SWAP32(x) \
	(((x & 0xFF000000) >> 24) | ((x & 0xFF0000) >> 8) | ((x & 0xFF00) << 8) | \
	 ((x & 0xFF) << 24))

#ifdef CONFIG_MK_REFERENCEQ
#	define CONFIG_CS75XX_PHY_ADDR_GMAC0 4
#endif

#define GE_PORT0_PHY_ADDR CONFIG_NI_PHY_ADDR_GMAC0
#define GE_PORT1_PHY_ADDR CONFIG_NI_PHY_ADDR_GMAC1
#define GE_PORT2_PHY_ADDR CONFIG_NI_PHY_ADDR_GMAC2

#define PHY_ID_RTL8201 0x001cc810
#define PHY_ID_RTL8211 0x001cc910
#define PHY_ID_RTL8214 0x001cc940
#define PHY_ID_RTL8211_G3_ASIC 0x001cc980
#define PHY_ID_RTL8211_SATURN_ASIC 0x001cc900
#define PHY_ID_QCA8337 0x004dd035
#define PHY_ID_MASK 0xFFFFFFF0

#define GE_MAC_INTF_GMII 0x0
#define GE_MAC_INTF_MII 0x1
#define GE_MAC_INTF_RGMII_1000 0x2
#define GE_MAC_INTF_RGMII_100 0x3
#define GE_MAC_INTF_QSGMII_1000 0x4
#define GE_MAC_INTF_RMII 0x5

#define NI_TOP_NI_RTH_MAC_10M 1
#define NI_TOP_NI_RTH_MAC_100M 0
#define NI_TOP_NI_RTH_MAC_HALF 1
#define NI_TOP_NI_RTH_MAC_FULL 0

/* Defines the base and top address in CPU XRAM for packets to cpu instance 0 */
#define RX_TOP_ADDR 0x02FF /* 0x300 * 8-byte = 6K-byte */
#define RX_BASE_ADDR 0x0000

/* Defines the base and top address in CPU XRAM for packets from cpu instance
 * 0.*/
#define TX_TOP_ADDR 0x03FF /* 0x100 * 8-byte = 2K-byte */
#define TX_BASE_ADDR 0x0300

#define RX_0_CPU_PKT_DIS (1 << 0)
#define TX_0_CPU_PKT_DIS (1 << 9)

#define PHY_POLL_TIMES 0x200

//#define ca_uint32 	unsigned int

struct cortina_ni_priv {
	unsigned int rx_xram_base_adr;
	unsigned int rx_xram_end_adr;
	unsigned short rx_xram_start;
	unsigned short rx_xram_end;
	unsigned int tx_xram_base_adr;
	unsigned int tx_xram_end_adr;
	unsigned short tx_xram_start;
	unsigned short tx_xram_end;
};

typedef volatile union {
	struct {
		ca_uint32 next_link : 10; /* bits  9: 0 */
		ca_uint32 bytes_valid : 4; /* bits 13:10 */
		ca_uint32 reserved : 16; /* bits 29:14 */
		ca_uint32 hdr_a : 1; /* bits 30:30 */
		ca_uint32 ownership : 1; /* bits 31:31 */
	} bf;
	ca_uint32 wrd;
} NI_HEADER_X_T;

typedef volatile union {
	struct {
		ca_uint32 packet_size : 14; /* bits 13:0 */
		ca_uint32 byte_valid : 4; /* bits 17:14 */
		ca_uint32 pfc : 1; /* bits 18:18 */
		ca_uint32 valid : 1; /* bits 19:19 */
		ca_uint32 drop : 1; /* bits 20:20 */
		ca_uint32 runt : 1; /* bits 21:21 */
		ca_uint32 oversize : 1; /* bits 22:22 */
		ca_uint32 jumbo : 1; /* bits 23:23 */
		ca_uint32 link_status : 1; /* bits 24:24 */
		ca_uint32 jabber : 1; /* bits 25:25 */
		ca_uint32 crc_error : 1; /* bits 26:26 */
		ca_uint32 pause : 1; /* bits 27:27 */
		ca_uint32 oam : 1; /* bits 28:28 */
		ca_uint32 unknown_opcode : 1; /* bits 29:29 */
		ca_uint32 multicast : 1; /* bits 30:30 */
		ca_uint32 broadcast : 1; /* bits 31:31 */
	} bf;
	ca_uint32 wrd;
} NI_PACKET_STATUS_T;

#if (defined(CONFIG_TARGET_PRESIDIO_ASIC) || defined(CONFIG_TARGET_SATURN_ASIC) || defined(CONFIG_TARGET_VENUS) || defined(CONFIG_TARGET_RTK_TAURUS) || defined(CONFIG_TARGET_RTK_ELNATH)) && !defined(CONFIG_RTK_V7_FPGA_PLATFORM)

#	if defined(CONFIG_TARGET_PRESIDIO_ASIC)
#		define CA_NI_MDIO_REG_BASE 0xF4338
#	else
#		if defined(CONFIG_TARGET_VENUS)
#			define CA_NI_MDIO_REG_BASE 0xF4339
#		elif defined(CONFIG_TARGET_RTK_TAURUS)
#			define CA_NI_MDIO_REG_BASE 0xF4600
#		else
#			define CA_NI_MDIO_REG_BASE 0xD000B
#		endif

#	endif

typedef volatile union {
	struct {
		ca_uint32 reserved : 2; /* bits  1:0 */
		ca_uint32 reg_off : 5; /* bits  6:2 */
		ca_uint32 phy_addr : 5; /* bits 11:7 */
		ca_uint32 reg_base : 20; /* bits 31:12 */
	} bf;
	ca_uint32 wrd;
} NI_MDIO_OPER_T;

#endif

#define NI_PORT_0 0
#define NI_PORT_1 1
#define NI_PORT_2 2
#define NI_PORT_3 3
#define NI_PORT_4 4
#define NI_PORT_5 5
#define NI_PORT_6 6
#define NI_PORT_7 7

#if defined(CONFIG_TARGET_SATURN_FPGA) || defined(CONFIG_TARGET_SATURN_ASIC)
#	define NI_READ_POLL_COUNT 1000000
#else
#	define NI_READ_POLL_COUNT 1000
#endif

#define __MDIO_WR_FLAG (0)
#define __MDIO_RD_FLAG (1)
#define __MDIO_ACCESS_TIMEOUT (1000000)
#define __MDIO_PER_CLK (62500)
#define CA_MDIO_ADDR_MIN (1)
#define CA_MDIO_ADDR_MAX (31)
#define CA_MDIO_CLOCK_MIN (1)
#define CA_MDIO_CLOCK_MAX (20000)

#define __MDIO_ADDR_CHK(addr) \
	do { \
		if ((addr) < CA_MDIO_ADDR_MIN || (addr) > CA_MDIO_ADDR_MAX) { \
			return CA_E_PARAM; \
		} \
	} while (0)

#define CA_ASSERT(x) \
	do { \
		if (!(x)) return CA_E_PARAM; \
	} while (0)

#endif /* __CORTINA_NI_H */
