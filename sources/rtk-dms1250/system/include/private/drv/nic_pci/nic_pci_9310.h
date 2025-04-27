/*
################################################################################
#
# r8168 is the Linux device driver released for Realtek Gigabit Ethernet
# controllers with PCI-Express interface.
#
# Copyright(c) 2016 Realtek Semiconductor Corp. All rights reserved.
#
# This program is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the Free
# Software Foundation; either version 2 of the License, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
# more details.
#
# You should have received a copy of the GNU General Public License along with
# this program; if not, see <http://www.gnu.org/licenses/>.
#
# Author:
# Realtek NIC software team <nicfae@realtek.com>
# No. 2, Innovation Road II, Hsinchu Science Park, Hsinchu 300, Taiwan
#
################################################################################
*/

/************************************************************************************
 *  This product is covered by one or more of the following patents:
 *  US6,570,884, US6,115,776, and US6,327,625.
 ***********************************************************************************/

#include <linux/version.h>
#include <private/drv/nic/nic_common.h>
#include <soc/type.h>


#define NIC_9310_CPU_TAG_SIZE   20
#define NIC_9310_SFLOW_RX       1
#define NIC_9310_SFLOW_TX       2

typedef struct nic_9310_cpuTag_s
{
    union {
        struct {
            uint8   CPUTAGIF:1;
            uint8       :1;
            uint8   SPA:6;
            uint8       :2;
            uint8   OF_LUMIS_TBL_ID:2;
            uint8   ACL_OF_HIT:2;
            uint8   OF_TBL_ID:2;
            uint8   SPN_IS_TRK:1;
            uint8   TRK_ID:7;
            uint8       :1;
            uint8   L2_ERR_PKT:1;
            uint8   L3_ERR_PKT:1;
            uint8   ATK_TYPE:5;
            uint16  QID:5;
            uint16      :1;
            uint16  SPN:10;
            uint16      :1;
            uint16  IDX:15;
            uint16  MIR_HIT:4;
            uint16  SFLOW:2;
            uint16  TT_HIT:1;
            uint16  TT_IDX:9;
            uint16      :1;
            uint16  OTAGIF:1;
            uint16  ITAGIF:1;
            uint16  FVID_SEL:1;
            uint16  FVID:12;
            uint8       :1;
            uint8   MAC_CST:1;
            uint8   DM_RXIDX:6;
            uint16  NEW_SA:1;
            uint16  PMV_FBD:1;
            uint16  L2_STTC_PMV:1;
            uint16  L2_DYN_PMV:1;
            uint16  OVERSIZE:1;
            uint16  PORT_DATA_IS_TRK:1;
            uint16  PORT_DATA:10;
            uint8   HASH_FULL:1;
            uint8   INVLD_SA:1;
            uint8   REASON:6;
        } __attribute__ ((aligned(1), packed)) rx;
        struct {
            uint16  CPUTAGIF:1;
            uint16      :15;
            uint8       :4;
            uint8   FWD_TYPE:4;
            uint8   ACL_ACT:1;
            uint8   CNGST_DROP:1;
            uint8   DM_PKT:1;
            uint8   DG_PKT:1;
            uint8   BP_FLTR:1;
            uint8   BP_STP:1;
            uint8   BP_VLAN_EGR:1;
            uint8   AS_TAGSTS:1;
            uint8   L3_ACT:1;
            uint8   ORI_TAGIF_EN:1;
            uint8   AS_QID:1;
            uint8   QID:5;
            uint16  ORI_ITAGIF:1;
            uint16  ORI_OTAGIF:1;
            uint16  FVID_SEL:1;
            uint16  FVID_EN:1;
            uint16  FVID:12;
            uint8   SRC_FLTR_EN:1;
            uint8   SP_IS_TRK:1;
            uint8   SPN9_4:6;
            uint32  SPN3_0:4;
            uint32  SW_DEV_ID:4;
            uint32  DPM55_32:24;
            uint32  DPM31_0;
        } __attribute__ ((aligned(1), packed)) tx;
    } un;
} nic_9310_cpuTag_t;


#ifndef NETIF_F_RXALL
#define NETIF_F_RXALL  0
#endif

#ifndef NETIF_F_RXFCS
#define NETIF_F_RXFCS  0
#endif

#ifndef HAVE_FREE_NETDEV
#define free_netdev(x)  kfree(x)
#endif

#ifndef SET_MODULE_OWNER
#define SET_MODULE_OWNER(dev)
#endif

#ifndef SA_SHIRQ
#define SA_SHIRQ IRQF_SHARED
#endif

#ifndef NETIF_F_GSO
#define gso_size    tso_size
#define gso_segs    tso_segs
#endif


#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE  1
#endif

#ifndef false
#define false 0
#endif

#ifndef true
#define true  1
#endif


#ifndef u64
#define u64     unsigned long long
#endif

#ifndef u32
#define u32     unsigned int
#endif

#ifndef u16
#define u16     unsigned short
#endif

#ifndef u8
#define u8     unsigned char
#endif


//Due to the hardware design of RTL8111B, the low 32 bit address of receive
//buffer must be 8-byte alignment.
#ifndef NET_IP_ALIGN
#define NET_IP_ALIGN        2
#endif
#define RTK_RX_ALIGN        8

#define MODULENAME "r8168"
#define PFX MODULENAME ": "

#define GPL_CLAIM "\
r8168  Copyright (C) 2016  Realtek NIC software team <nicfae@realtek.com> \n \
This program comes with ABSOLUTELY NO WARRANTY; for details, please see <http://www.gnu.org/licenses/>. \n \
This is free software, and you are welcome to redistribute it under certain conditions; see <http://www.gnu.org/licenses/>. \n"


#define R8168_MSG_DEFAULT \
    (NETIF_MSG_DRV | NETIF_MSG_PROBE | NETIF_MSG_IFUP | NETIF_MSG_IFDOWN)

#define TX_BUFFS_AVAIL(tp) \
    (tp->dirty_tx + NUM_TX_DESC - tp->cur_tx - 1)


/* MAC address length */
#ifndef MAC_ADDR_LEN
#define MAC_ADDR_LEN    6
#endif

#ifndef MAC_PROTOCOL_LEN
#define MAC_PROTOCOL_LEN    2
#endif

#define Reserved2_data  7
#define RX_DMA_BURST    7   /* Maximum PCI burst, '6' is 1024 */
#define TX_DMA_BURST_unlimited  7
#define TX_DMA_BURST_1024   6
#define TX_DMA_BURST_512    5
#define TX_DMA_BURST_256    4
#define TX_DMA_BURST_128    3
#define TX_DMA_BURST_64     2
#define TX_DMA_BURST_32     1
#define TX_DMA_BURST_16     0
#define Reserved1_data  0x3F
#define RxPacketMaxSize 0x3FE8  /* 16K - 1 - ETH_HLEN - VLAN - CRC... */
#define Jumbo_Frame_2k  (2 * 1024)
#define Jumbo_Frame_3k  (3 * 1024)
#define Jumbo_Frame_4k  (4 * 1024)
#define Jumbo_Frame_5k  (5 * 1024)
#define Jumbo_Frame_6k  (6 * 1024)
#define Jumbo_Frame_7k  (7 * 1024)
#define Jumbo_Frame_8k  (8 * 1024)
#define Jumbo_Frame_9k  (9 * 1024)
#define InterFrameGap   0x03    /* 3 means InterFrameGap = the shortest one */
#define RxEarly_off_V1 (0x07 << 11)
#define RxEarly_off_V2 (1 << 11)
#define Rx_Single_fetch_V2 (1 << 14)

#define R8168_REGS_SIZE     (256)
#define R8168_MAC_REGS_SIZE     (256)
#define R8168_PHY_REGS_SIZE     (16*2)
#define R8168_EPHY_REGS_SIZE  	(31*2)
#define R8168_ERI_REGS_SIZE  	(0x100)
#define R8168_REGS_DUMP_SIZE     (0x400)
#define R8168_PCI_REGS_SIZE  	(0x100)
#define R8168_NAPI_WEIGHT   64

#define RTL8168_TX_TIMEOUT  (6 * HZ)
#define RTL8168_LINK_TIMEOUT    (1 * HZ)
#define RTL8168_ESD_TIMEOUT (2 * HZ)

#define TALLY_MEM_SIZE          256
#define NUM_TX_DESC 200    /* Number of Tx descriptor registers */
#define NUM_RX_DESC 800    /* Number of Rx descriptor registers */

//#define RX_BUF_SIZE 0x05F3  /* 0x05F3 = 1522bye + 1 */
#define RX_BUF_SIZE 0x0607  /* 0x0607 = 1542 + 1 */
#define R8168_TX_RING_BYTES (NUM_TX_DESC * sizeof(struct TxDesc))
#define R8168_RX_RING_BYTES (NUM_RX_DESC * sizeof(struct RxDesc))

#define NODE_ADDRESS_SIZE 6

#define SHORT_PACKET_PADDING_BUF_SIZE 256


/* write/read MMIO register */
#define RTL_W8(reg, val8)           REG8(ioaddr + reg) = (val8)
#define RTL_W16(reg, val16)         REG16(ioaddr + reg) = (CPU_to_LE16(val16))
#define RTL_W32(reg, val32)         REG32(ioaddr + reg) = (CPU_to_LE32(val32))
#define RTL_R8(reg)                 REG8(ioaddr + reg)
#define RTL_R16(reg)                (LE16_to_CPU(REG16(ioaddr + reg)))
#define RTL_R32(reg)                (LE32_to_CPU(REG32(ioaddr + reg)))

#ifndef DMA_64BIT_MASK
#define DMA_64BIT_MASK  0xffffffffffffffffULL
#endif

#ifndef DMA_32BIT_MASK
#define DMA_32BIT_MASK  0x00000000ffffffffULL
#endif

#ifndef NETDEV_TX_OK
#define NETDEV_TX_OK 0      /* driver took care of packet */
#endif

#ifndef NETDEV_TX_BUSY
#define NETDEV_TX_BUSY 1    /* driver tx path was busy*/
#endif

#ifndef NETDEV_TX_LOCKED
#define NETDEV_TX_LOCKED -1 /* driver tx lock was already taken */
#endif

#ifndef ADVERTISED_Pause
#define ADVERTISED_Pause    (1 << 13)
#endif

#ifndef ADVERTISED_Asym_Pause
#define ADVERTISED_Asym_Pause   (1 << 14)
#endif

#ifndef ADVERTISE_PAUSE_CAP
#define ADVERTISE_PAUSE_CAP 0x400
#endif

#ifndef ADVERTISE_PAUSE_ASYM
#define ADVERTISE_PAUSE_ASYM    0x800
#endif

#ifndef MII_CTRL1000
#define MII_CTRL1000        0x09
#endif

#ifndef ADVERTISE_1000FULL
#define ADVERTISE_1000FULL  0x200
#endif

#ifndef ADVERTISE_1000HALF
#define ADVERTISE_1000HALF  0x100
#endif

/*****************************************************************************/



/*****************************************************************************/
/* Installations with ethtool version without eeprom, adapter id, or statistics
 * support */

#ifndef ETH_GSTRING_LEN
#define ETH_GSTRING_LEN 32
#endif


#ifndef ETHTOOL_GSTRINGS
#define ETHTOOL_GSTRINGS 0x1b
enum ethtool_stringset {
        ETH_SS_TEST             = 0,
        ETH_SS_STATS,
};
struct ethtool_gstrings {
        u32 cmd;            /* ETHTOOL_GSTRINGS */
        u32 string_set;     /* string set id e.c. ETH_SS_TEST, etc*/
        u32 len;            /* number of strings in the string set */
        u8 data[0];
};
#endif /* ETHTOOL_GSTRINGS */

#ifndef ETHTOOL_TEST
#define ETHTOOL_TEST 0x1a
enum ethtool_test_flags {
        ETH_TEST_FL_OFFLINE = (1 << 0),
        ETH_TEST_FL_FAILED  = (1 << 1),
};
struct ethtool_test {
        u32 cmd;
        u32 flags;
        u32 reserved;
        u32 len;
        u64 data[0];
};
#endif /* ETHTOOL_TEST */

#ifndef ETHTOOL_GEEPROM
#define ETHTOOL_GEEPROM 0xb
#undef ETHTOOL_GREGS
struct ethtool_eeprom {
        u32 cmd;
        u32 magic;
        u32 offset;
        u32 len;
        u8 data[0];
};

struct ethtool_value {
        u32 cmd;
        u32 data;
};
#endif /* ETHTOOL_GEEPROM */

#ifndef ETHTOOL_GLINK
#define ETHTOOL_GLINK 0xa
#endif /* ETHTOOL_GLINK */

#ifndef ETHTOOL_GREGS
#define ETHTOOL_GREGS       0x00000004 /* Get NIC registers */
#define ethtool_regs _kc_ethtool_regs
/* for passing big chunks of data */
struct _kc_ethtool_regs {
        u32 cmd;
        u32 version; /* driver-specific, indicates different chips/revs */
        u32 len; /* bytes */
        u8 data[0];
};
#endif /* ETHTOOL_GREGS */

#ifndef ETHTOOL_GMSGLVL
#define ETHTOOL_GMSGLVL     0x00000007 /* Get driver message level */
#endif
#ifndef ETHTOOL_SMSGLVL
#define ETHTOOL_SMSGLVL     0x00000008 /* Set driver msg level, priv. */
#endif
#ifndef ETHTOOL_NWAY_RST
#define ETHTOOL_NWAY_RST    0x00000009 /* Restart autonegotiation, priv */
#endif
#ifndef ETHTOOL_GLINK
#define ETHTOOL_GLINK       0x0000000a /* Get link status */
#endif
#ifndef ETHTOOL_GEEPROM
#define ETHTOOL_GEEPROM     0x0000000b /* Get EEPROM data */
#endif
#ifndef ETHTOOL_SEEPROM
#define ETHTOOL_SEEPROM     0x0000000c /* Set EEPROM data */
#endif

#ifndef ETHTOOL_SPAUSEPARAM
#define ETHTOOL_SPAUSEPARAM 0x00000013 /* Set pause parameters. */
#endif
#ifndef ETHTOOL_GRXCSUM
#define ETHTOOL_GRXCSUM     0x00000014 /* Get RX hw csum enable (ethtool_value) */
#endif
#ifndef ETHTOOL_SRXCSUM
#define ETHTOOL_SRXCSUM     0x00000015 /* Set RX hw csum enable (ethtool_value) */
#endif
#ifndef ETHTOOL_GTXCSUM
#define ETHTOOL_GTXCSUM     0x00000016 /* Get TX hw csum enable (ethtool_value) */
#endif
#ifndef ETHTOOL_STXCSUM
#define ETHTOOL_STXCSUM     0x00000017 /* Set TX hw csum enable (ethtool_value) */
#endif
#ifndef ETHTOOL_GSG
#define ETHTOOL_GSG     0x00000018 /* Get scatter-gather enable
* (ethtool_value) */
#endif
#ifndef ETHTOOL_SSG
#define ETHTOOL_SSG     0x00000019 /* Set scatter-gather enable
* (ethtool_value). */
#endif
#ifndef ETHTOOL_TEST
#define ETHTOOL_TEST        0x0000001a /* execute NIC self-test, priv. */
#endif
#ifndef ETHTOOL_GSTRINGS
#define ETHTOOL_GSTRINGS    0x0000001b /* get specified string set */
#endif
#ifndef ETHTOOL_GSTATS
#define ETHTOOL_GSTATS      0x0000001d /* get NIC-specific statistics */
#endif
#ifndef ETHTOOL_GTSO
#define ETHTOOL_GTSO        0x0000001e /* Get TSO enable (ethtool_value) */
#endif
#ifndef ETHTOOL_STSO
#define ETHTOOL_STSO        0x0000001f /* Set TSO enable (ethtool_value) */
#endif

#ifndef ETHTOOL_BUSINFO_LEN
#define ETHTOOL_BUSINFO_LEN 32
#endif

/*****************************************************************************/

enum RTL8168_DSM_STATE {
        DSM_MAC_INIT = 1,
        DSM_NIC_GOTO_D3 = 2,
        DSM_IF_DOWN = 3,
        DSM_NIC_RESUME_D3 = 4,
        DSM_IF_UP = 5,
};

enum RTL8168_registers {
        MAC0            = 0x00,     /* Ethernet hardware address. */
        MAC4            = 0x04,
        MAR0            = 0x08,     /* Multicast filter. */
        CounterAddrLow      = 0x10,
        CounterAddrHigh     = 0x14,
        CustomLED       = 0x18,
        TxDescStartAddrLow  = 0x20,
        TxDescStartAddrHigh = 0x24,
        TxHDescStartAddrLow = 0x28,
        TxHDescStartAddrHigh    = 0x2c,
        FLASH           = 0x30,
        ERSR            = 0x36,
        ChipCmd         = 0x37,
        TxPoll          = 0x38,
        IntrMask        = 0x3C,
        IntrStatus      = 0x3E,
        TxConfig        = 0x40,
        RxConfig        = 0x44,
        TCTR            = 0x48,
        Cfg9346         = 0x50,
        Config0         = 0x51,
        Config1         = 0x52,
        Config2         = 0x53,
        Config3         = 0x54,
        Config4         = 0x55,
        Config5         = 0x56,
        TDFNR           = 0x57,
        TimeInt0        = 0x58,
        TimeInt1        = 0x5C,
        PHYAR           = 0x60,
        CSIDR           = 0x64,
        CSIAR           = 0x68,
        PHYstatus       = 0x6C,
        MACDBG          = 0x6D,
        GPIO            = 0x6E,
        PMCH            = 0x6F,
        ERIDR           = 0x70,
        ERIAR           = 0x74,
        EPHY_RXER_NUM   = 0x7C,
        EPHYAR          = 0x80,
        TimeInt2        = 0x8C,
        OCPDR           = 0xB0,
        MACOCP          = 0xB0,
        OCPAR           = 0xB4,
        SecMAC0         = 0xB4,
        SecMAC4         = 0xB8,
        PHYOCP          = 0xB8,
        DBG_reg         = 0xD1,
        TwiCmdReg       = 0xD2,
        MCUCmd_reg      = 0xD3,
        RxMaxSize       = 0xDA,
        EFUSEAR         = 0xDC,
        CPlusCmd        = 0xE0,
        IntrMitigate    = 0xE2,
        RxDescAddrLow   = 0xE4,
        RxDescAddrHigh  = 0xE8,
        MTPS            = 0xEC,
        FuncEvent       = 0xF0,
        PPSW            = 0xF2,
        FuncEventMask   = 0xF4,
        TimeInt3        = 0xF4,
        FuncPresetState = 0xF8,
        IBCR0           = 0xF8,
        IBCR2           = 0xF9,
        IBIMR0          = 0xFA,
        IBISR0          = 0xFB,
        FuncForceEvent  = 0xFC,
};

enum RTL8168_register_content {
        /* InterruptStatusBits */
        SYSErr      = 0x8000,
        PCSTimeout  = 0x4000,
        SWInt       = 0x0100,
        TxDescUnavail   = 0x0080,
        RxFIFOOver  = 0x0040,
        LinkChg     = 0x0020,
        RxDescUnavail   = 0x0010,
        TxErr       = 0x0008,
        TxOK        = 0x0004,
        RxErr       = 0x0002,
        RxOK        = 0x0001,

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
        Cfg9346_EEDO = (1 << 0),
        Cfg9346_EEDI = (1 << 1),
        Cfg9346_EESK = (1 << 2),
        Cfg9346_EECS = (1 << 3),
        Cfg9346_EEM0 = (1 << 6),
        Cfg9346_EEM1 = (1 << 7),

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
        Reserved2_shift = 13,
        RxCfgDMAShift = 8,
        RxCfg_128_int_en = (1 << 15),
        RxCfg_fet_multi_en = (1 << 14),
        RxCfg_half_refetch = (1 << 13),
        RxCfg_9356SEL = (1 << 6),

        /* TxConfigBits */
        TxInterFrameGapShift = 24,
        TxDMAShift = 8, /* DMA burst value (0-7) is shift this many bits */
        TxMACLoopBack = (1 << 17),  /* MAC loopback */

        /* Config1 register */
        LEDS1       = (1 << 7),
        LEDS0       = (1 << 6),
        Speed_down  = (1 << 4),
        MEMMAP      = (1 << 3),
        IOMAP       = (1 << 2),
        VPD         = (1 << 1),
        PMEnable    = (1 << 0), /* Power Management Enable */

        /* Config2 register */
        PMSTS_En    = (1 << 5),

        /* Config3 register */
        Isolate_en  = (1 << 12), /* Isolate enable */
        MagicPacket = (1 << 5), /* Wake up when receives a Magic Packet */
        LinkUp      = (1 << 4), /* This bit is reserved in RTL8168B.*/
        /* Wake up when the cable connection is re-established */
        ECRCEN      = (1 << 3), /* This bit is reserved in RTL8168B*/
        Jumbo_En0   = (1 << 2), /* This bit is reserved in RTL8168B*/
        RDY_TO_L23  = (1 << 1), /* This bit is reserved in RTL8168B*/
        Beacon_en   = (1 << 0), /* This bit is reserved in RTL8168B*/

        /* Config4 register */
        Jumbo_En1   = (1 << 1), /* This bit is reserved in RTL8168B*/

        /* Config5 register */
        BWF     = (1 << 6), /* Accept Broadcast wakeup frame */
        MWF     = (1 << 5), /* Accept Multicast wakeup frame */
        UWF     = (1 << 4), /* Accept Unicast wakeup frame */
        LanWake     = (1 << 1), /* LanWake enable/disable */
        PMEStatus   = (1 << 0), /* PME status can be reset by PCI RST# */

        /* CPlusCmd */
        EnableBist  = (1 << 15),
        Macdbgo_oe  = (1 << 14),
        Normal_mode = (1 << 13),
        Force_halfdup   = (1 << 12),
        Force_rxflow_en = (1 << 11),
        Force_txflow_en = (1 << 10),
        Cxpl_dbg_sel    = (1 << 9),//This bit is reserved in RTL8168B
        ASF     = (1 << 8),//This bit is reserved in RTL8168C
        PktCntrDisable  = (1 << 7),
        RxVlan      = (1 << 6),
        RxChkSum    = (1 << 5),
        Macdbgo_sel = 0x001C,
        INTT_0      = 0x0000,
        INTT_1      = 0x0001,
        INTT_2      = 0x0002,
        INTT_3      = 0x0003,

        /* rtl8168_PHYstatus */
        PowerSaveStatus = 0x80,
        TxFlowCtrl = 0x40,
        RxFlowCtrl = 0x20,
        _1000bpsF = 0x10,
        _100bps = 0x08,
        _10bps = 0x04,
        LinkStatus = 0x02,
        FullDup = 0x01,

        /* DBG_reg */
        Fix_Nak_1 = (1 << 4),
        Fix_Nak_2 = (1 << 3),
        DBGPIN_E2 = (1 << 0),

        /* ResetCounterCommand */
        CounterReset = 0x1,
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

        /* ERI access */
        ERIAR_Flag = 0x80000000,
        ERIAR_Write = 0x80000000,
        ERIAR_Read = 0x00000000,
        ERIAR_Addr_Align = 4, /* ERI access register address must be 4 byte alignment */
        ERIAR_ExGMAC = 0,
        ERIAR_MSIX = 1,
        ERIAR_ASF = 2,
        ERIAR_OOB = 2,
        ERIAR_Type_shift = 16,
        ERIAR_ByteEn = 0x0f,
        ERIAR_ByteEn_shift = 12,

        /* OCP GPHY access */
        OCPDR_Write = 0x80000000,
        OCPDR_Read = 0x00000000,
        OCPDR_Reg_Mask = 0xFF,
        OCPDR_Data_Mask = 0xFFFF,
        OCPDR_GPHY_Reg_shift = 16,
        OCPAR_Flag = 0x80000000,
        OCPAR_GPHY_Write = 0x8000F060,
        OCPAR_GPHY_Read = 0x0000F060,
        OCPR_Write = 0x80000000,
        OCPR_Read = 0x00000000,
        OCPR_Addr_Reg_shift = 16,
        OCPR_Flag = 0x80000000,
        OCP_STD_PHY_BASE_PAGE = 0x0A40,

        /* MCU Command */
        Now_is_oob = (1 << 7),
        Txfifo_empty = (1 << 5),
        Rxfifo_empty = (1 << 4),

        /* E-FUSE access */
        EFUSE_WRITE = 0x80000000,
        EFUSE_WRITE_OK  = 0x00000000,
        EFUSE_READ  = 0x00000000,
        EFUSE_READ_OK   = 0x80000000,
        EFUSE_Reg_Mask  = 0x03FF,
        EFUSE_Reg_Shift = 8,
        EFUSE_Check_Cnt = 300,
        EFUSE_READ_FAIL = 0xFF,
        EFUSE_Data_Mask = 0x000000FF,

        /* GPIO */
        GPIO_en = (1 << 0),

};

enum _DescStatusBit {
        DescOwn     = (1 << 31), /* Descriptor is owned by NIC */
        RingEnd     = (1 << 30), /* End of descriptor ring */
        FirstFrag   = (1 << 29), /* First segment of a packet */
        LastFrag    = (1 << 28), /* Final segment of a packet */

        /* Tx private */
        /*------ offset 0 of tx descriptor ------*/
        LargeSend   = (1 << 27), /* TCP Large Send Offload (TSO) */
        GiantSendv4 = (1 << 26), /* TCP Giant Send Offload V4 (GSOv4) */
        GiantSendv6 = (1 << 25), /* TCP Giant Send Offload V6 (GSOv6) */
        LargeSend_DP = (1 << 16), /* TCP Large Send Offload (TSO) */
        MSSShift    = 16,        /* MSS value position */
        MSSMask     = 0x7FFU,    /* MSS value 11 bits */
        TxIPCS      = (1 << 18), /* Calculate IP checksum */
        TxUDPCS     = (1 << 17), /* Calculate UDP/IP checksum */
        TxTCPCS     = (1 << 16), /* Calculate TCP/IP checksum */
        TxVlanTag   = (1 << 17), /* Add VLAN tag */

        /*@@@@@@ offset 4 of tx descriptor => bits for RTL8168C/CP only     begin @@@@@@*/
        TxUDPCS_C   = (1 << 31), /* Calculate UDP/IP checksum */
        TxTCPCS_C   = (1 << 30), /* Calculate TCP/IP checksum */
        TxIPCS_C    = (1 << 29), /* Calculate IP checksum */
        TxIPV6F_C   = (1 << 28), /* Indicate it is an IPv6 packet */
        /*@@@@@@ offset 4 of tx descriptor => bits for RTL8168C/CP only     end @@@@@@*/


        /* Rx private */
        /*------ offset 0 of rx descriptor ------*/
        PID1        = (1 << 18), /* Protocol ID bit 1/2 */
        PID0        = (1 << 17), /* Protocol ID bit 2/2 */

#define RxProtoUDP  (PID1)
#define RxProtoTCP  (PID0)
#define RxProtoIP   (PID1 | PID0)
#define RxProtoMask RxProtoIP

        RxIPF       = (1 << 16), /* IP checksum failed */
        RxUDPF      = (1 << 15), /* UDP/IP checksum failed */
        RxTCPF      = (1 << 14), /* TCP/IP checksum failed */
        RxVlanTag   = (1 << 16), /* VLAN tag available */

        /*@@@@@@ offset 0 of rx descriptor => bits for RTL8168C/CP only     begin @@@@@@*/
        RxUDPT      = (1 << 18),
        RxTCPT      = (1 << 17),
        /*@@@@@@ offset 0 of rx descriptor => bits for RTL8168C/CP only     end @@@@@@*/

        /*@@@@@@ offset 4 of rx descriptor => bits for RTL8168C/CP only     begin @@@@@@*/
        RxV6F       = (1 << 31),
        RxV4F       = (1 << 30),
        /*@@@@@@ offset 4 of rx descriptor => bits for RTL8168C/CP only     end @@@@@@*/
};

enum features {
//  RTL_FEATURE_WOL = (1 << 0),
        RTL_FEATURE_MSI = (1 << 1),
};

enum wol_capability {
        WOL_DISABLED = 0,
        WOL_ENABLED = 1
};

enum bits {
        BIT_0 = (1 << 0),
        BIT_1 = (1 << 1),
        BIT_2 = (1 << 2),
        BIT_3 = (1 << 3),
        BIT_4 = (1 << 4),
        BIT_5 = (1 << 5),
        BIT_6 = (1 << 6),
        BIT_7 = (1 << 7),
        BIT_8 = (1 << 8),
        BIT_9 = (1 << 9),
        BIT_10 = (1 << 10),
        BIT_11 = (1 << 11),
        BIT_12 = (1 << 12),
        BIT_13 = (1 << 13),
        BIT_14 = (1 << 14),
        BIT_15 = (1 << 15),
        BIT_16 = (1 << 16),
        BIT_17 = (1 << 17),
        BIT_18 = (1 << 18),
        BIT_19 = (1 << 19),
        BIT_20 = (1 << 20),
        BIT_21 = (1 << 21),
        BIT_22 = (1 << 22),
        BIT_23 = (1 << 23),
        BIT_24 = (1 << 24),
        BIT_25 = (1 << 25),
        BIT_26 = (1 << 26),
        BIT_27 = (1 << 27),
        BIT_28 = (1 << 28),
        BIT_29 = (1 << 29),
        BIT_30 = (1 << 30),
        BIT_31 = (1 << 31)
};

enum effuse {
        EFUSE_NOT_SUPPORT = 0,
        EFUSE_SUPPORT_V1,
        EFUSE_SUPPORT_V2,
        EFUSE_SUPPORT_V3,
};
#define RsvdMask    0x3fffc000

struct TxDesc {
        u32 opts1;
        u32 opts2;
        u64 addr;
};

struct RxDesc {
        u32 opts1;
        u32 opts2;
        u64 addr;
};

struct ring_info {
        drv_nic_pkt_t  *pPacket;
        drv_nic_tx_cb_f tx_callback;
        u32     len;
        void    *cookie;
        u8      __pad[sizeof(void *) - sizeof(u32)];
};

struct pci_resource {
        u8  cmd;
        u8  cls;
        u16 io_base_h;
        u16 io_base_l;
        u16 mem_base_h;
        u16 mem_base_l;
        u8  ilr;
        u16 resv_0x1c_h;
        u16 resv_0x1c_l;
        u16 resv_0x20_h;
        u16 resv_0x20_l;
        u16 resv_0x24_h;
        u16 resv_0x24_l;
        u16 resv_0x2c_h;
        u16 resv_0x2c_l;
        u32 pci_sn_l;
        u32 pci_sn_h;
};

struct rtl8168_private {
        void *mmio_addr;    /* memory map physical address */
        struct pci_dev *pci_dev;    /* Index of PCI device */
        u32 msg_enable;
        u32 tx_tcp_csum_cmd;
        u32 tx_udp_csum_cmd;
        u32 tx_ip_csum_cmd;
        u32 tx_ipv6_csum_cmd;
        int max_jumbo_frame_size;
        int chipset;
        u32 mcfg;
        u32 cur_rx; /* Index into the Rx descriptor buffer of next Rx pkt. */
        u32 cur_tx; /* Index into the Tx descriptor buffer of next Rx pkt. */
        u32 txRDPIdx;
        u32 dirty_rx;
        u32 dirty_tx;
        struct TxDesc *TxDescArray; /* 256-aligned Tx descriptor ring */
        struct RxDesc *RxDescArray; /* 256-aligned Rx descriptor ring */
        uintptr TxPhyAddr;
        uintptr RxPhyAddr;
        drv_nic_pkt_t *Rx_skbuff[NUM_RX_DESC]; /* Rx data buffers */
        struct ring_info tx_skb[NUM_TX_DESC];   /* Tx data buffers */
        unsigned rx_buf_sz;
        struct pci_resource pci_cfg_space;
        unsigned int esd_flag;
        unsigned int pci_cfg_is_read;
        unsigned int rtl8168_rx_config;
        u16 cp_cmd;
        u16 intr_mask;
        u16 timer_intr_mask;
        int phy_auto_nego_reg;
        int phy_1000_ctrl_reg;
        u8 org_mac_addr[NODE_ADDRESS_SIZE];
        struct rtl8168_counters *tally_vaddr;
        uintptr tally_paddr;

#ifdef CONFIG_R8168_VLAN
        struct vlan_group *vlgrp;
#endif
        u32 wol_opts;
        u8  eeprom_type;
        u8  autoneg;
        u8  duplex;
        u16 speed;
        u16 eeprom_len;
        u16 cur_page;
        u32 bios_setting;

        unsigned features;

        u8 org_pci_offset_99;
        u8 org_pci_offset_180;
        u8 issue_offset_99_event;

        u8 org_pci_offset_80;
        u8 org_pci_offset_81;
        u8 use_timer_interrrupt;

        u32 keep_intr_cnt;

        u8  HwIcVerUnknown;
        u8  NotWrRamCodeToMicroP;
        u8  NotWrMcuPatchCode;
        u8  HwHasWrRamCodeToMicroP;

        u16 sw_ram_code_ver;
        u16 hw_ram_code_ver;

        u8 rtk_enable_diag;

        u8 ShortPacketSwChecksum;

        u8 UseSwPaddingShortPkt;

        u8 RequireAdcBiasPatch;
        u16 AdcBiasPatchIoffset;

        u8 RequireAdjustUpsTxLinkPulseTiming;
        u16 SwrCnt1msIni;

        u8 HwSuppNowIsOobVer;

        u8 RequiredSecLanDonglePatch;

        u32 HwFiberModeVer;

        u8 HwSuppMagicPktVer;

        u8 EnableDhcpTimeoutWake;
        u8 EnableTeredoOffload;
        u8 EnableKCPOffload;
#ifdef ENABLE_REALWOW_SUPPORT
        u32 DhcpTimeout;
        MP_KCP_INFO MpKCPInfo;
        //Realwow--------------
#endif //ENABLE_REALWOW_SUPPORT

};

enum eetype {
        EEPROM_TYPE_NONE=0,
        EEPROM_TYPE_93C46,
        EEPROM_TYPE_93C56,
        EEPROM_TWSI
};

enum mcfg {
        CFG_METHOD_1=0,
        CFG_METHOD_2,
        CFG_METHOD_3,
        CFG_METHOD_4,
        CFG_METHOD_5,
        CFG_METHOD_6,
        CFG_METHOD_7,
        CFG_METHOD_8,
        CFG_METHOD_9 ,
        CFG_METHOD_10,
        CFG_METHOD_11,
        CFG_METHOD_12,
        CFG_METHOD_13,
        CFG_METHOD_14,
        CFG_METHOD_15,
        CFG_METHOD_16,
        CFG_METHOD_17,
        CFG_METHOD_18,
        CFG_METHOD_19,
        CFG_METHOD_20,
        CFG_METHOD_21,
        CFG_METHOD_22,
        CFG_METHOD_23,
        CFG_METHOD_24,
        CFG_METHOD_25,
        CFG_METHOD_26,
        CFG_METHOD_27,
        CFG_METHOD_28,
        CFG_METHOD_29,
        CFG_METHOD_30,
        CFG_METHOD_MAX,
        CFG_METHOD_DEFAULT = 0xFF
};

#define LSO_32K 32000
#define LSO_64K 64000

#define NIC_MIN_PHYS_BUF_COUNT          (2)
#define NIC_MAX_PHYS_BUF_COUNT_LSO_64K  (24)
#define NIC_MAX_PHYS_BUF_COUNT_LSO2     (16*4)

#define GTTCPHO_SHIFT                   18
#define GTTCPHO_MAX                     0x7fU
#define GTPKTSIZE_MAX                   0x3ffffU
#define TCPHO_SHIFT                     18
#define TCPHO_MAX                       0x3ffU
#define LSOPKTSIZE_MAX                  0xffffU
#define MSS_MAX                         0x07ffu /* MSS value */

#define OOB_CMD_RESET       0x00
#define OOB_CMD_DRIVER_START    0x05
#define OOB_CMD_DRIVER_STOP 0x06
#define OOB_CMD_SET_IPMAC   0x41

#define WAKEUP_MAGIC_PACKET_NOT_SUPPORT (0)
#define WAKEUP_MAGIC_PACKET_V1 (1)
#define WAKEUP_MAGIC_PACKET_V2 (2)



extern struct rtl8168_private *tp;


void mdio_write(u32 RegAddr, u32 value);
void mdio_prot_write(u32 RegAddr, u32 value);
void mac_ocp_write(u16 reg_addr, u16 value);
u16 mac_ocp_read(u16 reg_addr);
void ClearEthPhyBit(u8 addr, u16 mask);
void SetEthPhyBit( u8  addr, u16  mask);
void OCP_write(u16 addr, u8 len, u32 data);
void OOB_notify(u8 cmd);
void rtl8168_init_ring_indexes(struct rtl8168_private *tp);
int rtl8168_eri_write(int addr, int len, u32 value, int type);
void OOB_mutex_lock(struct rtl8168_private *tp);
u32 mdio_read(u32 RegAddr);
u32 OCP_read(u16 addr, u8 len);
u32 rtl8168_eri_read(int addr, int len, int type);
u16 rtl8168_ephy_read(int RegAddr);
void EnableNowIsOob(struct rtl8168_private *tp);
void DisableNowIsOob(struct rtl8168_private *tp);
void OOB_mutex_unlock(struct rtl8168_private *tp);
void rtl8168_shutdown(struct pci_dev *pdev);
void rtl8168_remove_one(struct pci_dev *pdev);
int proc_get_tally_counter(void);


