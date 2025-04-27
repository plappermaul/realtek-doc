/*
* ----------------------------------------------------------------
* Copyright c                  Realtek Semiconductor Corporation, 2002  
* All rights reserved.
* 
* $Header: /tmp/cvsroot/uClinux-dist/linux-2.4.x/drivers/net/re865x/AsicDriver/rtl865xc_asicregs.h,v 1.111 2008-01-21 09:04:59 darcy_lu Exp $
*
* Abstract: ASIC specific definitions.
*
* $Author: darcy_lu $
*
* ---------------------------------------------------------------
*/

#ifndef _ASICREGS_H
#define _ASICREGS_H

#define CONFIG_RTL865XC


#define UNCACHED_ADDRESS(x) ((void *)(0x20000000 | (uint32)x ))
#define CACHED_ADDRESS(x) ((void*)(~0x20000000 & (uint32)x ))
#define PHYSICAL_ADDRESS(x) (((uint32)x) & 0x1fffffff)
#define KSEG0_ADDRESS(x) ((void*)(PHYSICAL_ADDRESS(x) | 0x80000000))
#define KSEG1_ADDRESS(x) ((void*)(PHYSICAL_ADDRESS(x) | 0xA0000000))

/*
 *	Just for check menuconfig ....
 */
#if defined(CONFIG_RTL865XB)
#elif defined(CONFIG_RTL865XC)
#else
	#error You must choice either 865xB or 865xC in 'make menuconfig'.
#endif

#undef SWTABLE_NO_HW

/* Basic features
*/
#define MAX_PORT_NUMBER           6
#define PORT_NUM_MASK               7
#define ALL_PORT_MASK                 0x3F


/* Filtering Database features */
#define RTL865XC_FID_BITMASK				0x3

/* Register access macro (REG*()).
*/
#define REG32(reg) 			(*((volatile uint32 *)(reg)))
#define REG16(reg) 			(*((volatile uint16 *)(reg)))
#define REG8(reg) 				(*((volatile uint8 *)(reg)))

/* Register access macro (READ_MEM32() and WRITE_MEM32()). */
//#if defined(RTL865X_MODEL_USER)||defined(RTL865X_TEST)
#if 0
/*
 * big_endian() is a macro to make endian consistence between 865x board and x86 PC.
 * All the ASIC registers stored in memory will be stored at big-endian order.
 * Therefore, we will see the same result when memory dump ASIC registers.
 *
 * big_endian32() and big_endian16() are like ntohs() and ntohl().
 * In order to reduce the complexity of include hirerachy, we don't use ntohs-related functions.
 */
#define big_endian32(x) ((((x)&0xff000000)>>24)|(((x)&0x00ff0000)>>8)|(((x)&0x0000ff00)<<8)|(((x)&0x000000ff)<<24))
#define big_endian16(x) (((x)>>8)|(((x)&0x000000ff)<<8))
#define big_endian(x) big_endian32(x) /* backward-compatible */
#else
#define big_endian32(x) (x)
#define big_endian16(x) (x)
#define big_endian(x) big_endian32(x) /* backward-compatible */
#endif

//#if defined(RTL865X_MODEL_USER)||defined(RTL865X_MODEL_KERNEL)
#if 1
void WRITE_MEM32(uint32 reg, uint32 val);
void WRITE_MEM16(uint32 reg, uint32 val);
void WRITE_MEM8(uint32 reg, uint32 val);
uint32 READ_MEM32(uint32 reg);
uint32 READ_MEM16(uint32 reg);
uint32 READ_MEM8(uint32 reg);
void WRITE_VIR32(uint32 reg, uint32 val);
void WRITE_VIR16(uint32 reg, uint32 val);
void WRITE_VIR8(uint32 reg, uint32 val);
uint32 READ_VIR32(uint32 reg);
uint32 READ_VIR16(uint32 reg);
uint32 READ_VIR8(uint32 reg);
#else
/* Target Platform, RTL865X_TEST */
#define WRITE_MEM32(reg,val) REG32(reg)=big_endian(val)
#define WRITE_MEM16(reg,val) REG16(reg)=big_endian16(val)
#define WRITE_MEM8(reg,val) REG8(reg)=(val)
#define READ_MEM32(reg) big_endian(REG32(reg))
#define READ_MEM16(reg) big_endian16(REG16(reg))
#define READ_MEM8(reg) (REG8(reg))
#endif

/* MACRO to update HW configuration */
#define UPDATE_MEM32(reg, val, mask, offset)	\
	do{ \
		WRITE_MEM32(	(reg),	\
						((READ_MEM32(reg) & ~((mask) << (offset))) | (((val) & (mask)) << (offset))));\
	} while (0)

/* MACRO to get value of HW configuration */
#define GET_MEM32_VAL(reg, mask, offset)	((READ_MEM32(reg) >> (offset)) & (mask))

#define	DIVISOR	1000         /* cfliu: change DIVISOR from 0xe to 4096 for watchdog */
                             /* yjlou: change DIVISOR from 4096 to 100 for better resolution. */
                             /* yjlou: change DIVISOR from 100 to 1000 for longer watchdog trigger. */
                             /* cfliu: change DIVISOR back to 4000 to let printk happy. */

#define REAL_SWCORE_BASE		0xBB800000

#define REAL_SYSTEM_BASE		0xB8000000
//#define REAL_SYSTEM_BASE		0xB8620000
//#define REAL_SYSTEM_BASE		0xB8018000

#define REAL_HSB_BASE			0xBB806280
#define REAL_HSA_BASE			0xBB806200
#define REAL_SWTBL_BASE			0xBB000000

//#if defined(RTL865X_TEST) || defined(RTL865X_MODEL_USER) || defined(RTL865X_MODEL_KERNEL)
#if 1
#define VIRTUAL_SWCORE_REG_SIZE	(1*64*1024) /* said, BB80_xxxx */
#define VIRTUAL_SYSTEM_REG_SIZE	(2*64*1024) /* said, B800_xxxx */
#define VIRTUAL_SWCORE_TBL_SIZE	(1*1024*1024+256*1024) /* said, BB00_xxxx */
extern int8						*pVirtualSWReg;
extern int8						*pVirtualSysReg;
extern int8						*pVirtualHsb;
extern int8						*pVirtualHsa;
extern int8						*pVirtualSWTable;
#define SWCORE_BASE			((uint32)pVirtualSWReg)
#define SWCORE_SIZE			VIRTUAL_SWCORE_REG_SIZE
#define SYSTEM_BASE			((uint32)pVirtualSysReg)
#define SYSTEM_SIZE			(VIRTUAL_SYSTEM_REG_SIZE)
#define HSB_BASE			((uint32)pVirtualHsb)
#define HSB_SIZE			0x40
#define HSA_BASE			((uint32)pVirtualHsa)
#define HSA_SIZE			0x40
#else
#define SWCORE_BASE			REAL_SWCORE_BASE
#define SYSTEM_BASE			REAL_SYSTEM_BASE
#define HSB_BASE			REAL_HSB_BASE
#define HSA_BASE			REAL_HSA_BASE
#endif

/* Table access and CPU interface control registers
*/
#define TACI_BASE                 		(SWCORE_BASE + 0x00004D00)
#define SWTACR                    		(0x000 + TACI_BASE)     /* Table Access Control */
#define SWTASR                     		(0x004 + TACI_BASE)     /* Table Access Status */
#define SWTAA                       		(0x008 + TACI_BASE)     /* Table Access Address */
#define TCR0                        		(0x020 + TACI_BASE)     /* Table Access Control 0 */
#define TCR1                        		(0x024 + TACI_BASE)     /* Table Access Control 1 */
#define TCR2                        		(0x028 + TACI_BASE)     /* Table Access Control 2 */
#define TCR3                        		(0x02C + TACI_BASE)     /* Table Access Control 3 */
#define TCR4                        		(0x030 + TACI_BASE)     /* Table Access Control 4 */
#define TCR5                        		(0x034 + TACI_BASE)     /* Table Access Control 5 */
#define TCR6                        		(0x038 + TACI_BASE)     /* Table Access Control 6 */
#define TCR7					(0x03C + TACI_BASE)     /* Table Access Control 7 */
/* Table access control register field definitions
*/
#define ACTION_MASK                 	1
#define ACTION_DONE                 	0
#define ACTION_START                	1
#define CMD_MASK                    	(7 << 1)
#define CMD_ADD                     	(1 << 1)
#define CMD_MODIFY                  	(1 << 2)
#define CMD_FORCE                   	(1 << 3)
#define STOP_TLU_READY			(1<<19)
#define EN_STOP_TLU				(1<<18)
/* Table access status register field definitions 
*/
#define TABSTS_MASK                 	1
#define TABSTS_SUCCESS              	0
#define TABSTS_FAIL                 	1
/* Vlan table access definitions 
*/
#define STP_DISABLE                 	0
#define STP_BLOCK                   	1
#define STP_LEARN                   	2
#define STP_FORWARD                 	3
/* Protocol trapping table access definitions
*/
#define TYPE_TRAP_ETHERNET     	0x00
#define TYPE_TRAP_IP                	0x02
#define TYPE_TRAP_TCP               	0x05
#define TYPE_TRAP_UDP               	0x06
/* L3 Routing table access definitions
*/
#define PROCESS_PPPOE               	0x00
#define PROCESS_DIRECT              	0x01
#define PROCESS_INDIRECT          	0x02
#define PROCESS_S_CPU             	0x04
#define PROCESS_NXT_HOP         	0x05
#define PROCESS_S_DROP           	0x06
#define PROCESS_N_DROP           	0x07
/* ACL table access definitions
*/
#define RULE_ETHERNET               		0
#define RULE_IP                     			2
#define RULE_IFSEL                  		3
#define RULE_ICMP                   		4
#define RULE_IGMP                  		5
#define RULE_TCP                    		6
#define RULE_UDP                    		7
#define RULE_SRC_FILTER             		8
#define RULE_DST_FILTER             		9
#define ACTION_PERMIT               		0
#define ACTION_REDIRECT             		1
#define ACTION_S_DROP               		2
#define ACTION_S_CPU                		3
#define ACTION_N_DROP               		4
#define ACTION_N_CPU                		5
#define ACTION_CPU_FOR_LOGGING     	5
#define ACTION_MIRROR               		6
#define ACTION_PPPOE_REDIRECT       	7
#define ACTION_DEFAULT_REDIRECT     	8
#define ACTION_DEFAULT_MIRROR       	9
#define ACTION_DROP_RATE_PPS        	10
#define ACTION_LOG2CPU_RATE_PPS    11
#define ACTION_DROP_RATE_BPS        	12
#define ACTION_LOG2CPU_RATE_BPS    13



/* MIB counter registers
*/
#define MIB_COUNTER_BASE											(SWCORE_BASE + 0x00001000)
#define MIB_CONTROL                 										(0x00 + MIB_COUNTER_BASE)   /*MIB control Register */

/* MIB counter registers
*/
#define MIB_ADDROFFSETBYPORT										0x80	/* Address offset of the same counters of each port. Ex: P0's ifInOctets counter and P1's ifInOctets counter. */
#define OFFSET_ETHERSTATSCPUEVENTPKT								0x84
#define OFFSET_IFINOCTETS_P0										0x100
#define OFFSET_IFINUCASTPKTS_P0										0x108
#define OFFSET_ETHERSTATSOCTETS_P0									0x10C
#define OFFSET_ETHERSTATSUNDERSIZEPKTS_P0							0x114
#define OFFSET_ETHERSTATSFRAGMEMTS_P0								0x118
#define OFFSET_ETHERSTATSPKTS64OCTETS_P0							0x11C
#define OFFSET_ETHERSTATSPKTS65TO127OCTETS_P0						0x120
#define OFFSET_ETHERSTATSPKTS128TO255OCTETS_P0						0x124
#define OFFSET_ETHERSTATSPKTS256TO511OCTETS_P0						0x128
#define OFFSET_ETHERSTATSPKTS512TO1023OCTETS_P0						0x12C
#define OFFSET_ETHERSTATSPKTS1024TO1518OCTETS_P0					0x130
#define OFFSET_ETHERSTATSOVERSIZEPKTS_P0							0x134
#define OFFSET_ETHERSTATSJABBERS_P0									0x138
#define OFFSET_ETHERSTATSMULTICASTPKTS_P0							0x13C
#define OFFSET_ETHERSTATSBROADCASTPKTS_P0							0x140
#define OFFSET_DOT1DTPPORTINDISCARDS_P0								0x144
#define OFFSET_ETHERSTATSDROPEVENTS_P0								0x148
#define OFFSET_DOT3STATSFCSERRORS_P0								0x14C
#define OFFSET_DOT3STATSSYMBOLERRORS_P0								0x150
#define OFFSET_DOT3CONTROLINUNKNOWNOPCODES_P0						0x154
#define OFFSET_DOT3INPAUSEFRAMES_P0									0x158
#define OFFSET_IFOUTOCTETS_P0										0x800
#define OFFSET_IFOUTUCASTPKTS_P0									0x808
#define OFFSET_IFOUTMULTICASTPKTS_P0								0x80C
#define OFFSET_IFOUTBROADCASTPKTS_P0								0x810
#define OFFSET_IFOUTDISCARDS										0x814
#define OFFSET_DOT3STATSSINGLECOLLISIONFRAMES_P0					0x818
#define OFFSET_DOT3STATSMULTIPLECOLLISIONFRAMES_P0					0x81C
#define OFFSET_DOT3STATSDEFERREDTRANSMISSIONS_P0					0x820
#define OFFSET_DOT3STATSLATECOLLISIONS_P0							0x824
#define OFFSET_DOT3STATSEXCESSIVECOLLISIONS_P0						0x828
#define OFFSET_DOT3OUTPAUSEFRAMES_P0								0x82C
#define OFFSET_DOT1DBASEPORTDELAYEXCEEDEDDISCARDS_P0				0x830
#define OFFSET_ETHERSTATSCOLLISIONS_P0								0x834
/* MIB control register field definitions 
*/
#define ALL_COUNTER_RESTART_MASK			0x0007ffff	/* SYS_COUNTER_RESTART | IN_COUNTER_RESTART_P8 | ... | OUT_COUNTER_RESTART_P0 */
#define SYS_COUNTER_RESTART				(1 << 18)
#define IN_COUNTER_RESTART_P8				(1 << 17)
#define OUT_COUNTER_RESTART_P8			(1 << 16)
#define IN_COUNTER_RESTART_P7				(1 << 15)
#define OUT_COUNTER_RESTART_P7			(1 << 14)
#define IN_COUNTER_RESTART_P6				(1 << 13)
#define OUT_COUNTER_RESTART_P6			(1 << 12)
#define IN_COUNTER_RESTART_P5				(1 << 11)
#define OUT_COUNTER_RESTART_P5			(1 << 10)
#define IN_COUNTER_RESTART_P4				(1 << 9)
#define OUT_COUNTER_RESTART_P4			(1 << 8)
#define IN_COUNTER_RESTART_P3				(1 << 7)
#define OUT_COUNTER_RESTART_P3			(1 << 6)
#define IN_COUNTER_RESTART_P2				(1 << 5)
#define OUT_COUNTER_RESTART_P2			(1 << 4)
#define IN_COUNTER_RESTART_P1				(1 << 3)
#define OUT_COUNTER_RESTART_P1			(1 << 2)
#define IN_COUNTER_RESTART_P0				(1 << 1)
#define OUT_COUNTER_RESTART_P0			(1 << 0)

#define PORT_FOR_COUNTING_OFFSET                    24	/*8650 offset while 8650B offset is 21*/

#define CPU_EVENTS                                  (0x074 + MIB_COUNTER_BASE)
#define EXT_PORT_FOR_COUNTING_MASK                  0x00E00000
#define EXT_PORT_FOR_COUNTING_OFFSET                21
#define RX_BYTES_1                                  (0x080 + MIB_COUNTER_BASE)
#define RX_PKTS_1                                   (0x084 + MIB_COUNTER_BASE)
#define DROP_EVENTS_1                               (0x088 + MIB_COUNTER_BASE)
#define CPU_EVENTS_1                                (0x08C + MIB_COUNTER_BASE)
#define RX_BYTES_2                                  (0x090 + MIB_COUNTER_BASE)
#define RX_PKTS_2                                   (0x094 + MIB_COUNTER_BASE)
#define DROP_EVENTS_2                               (0x098 + MIB_COUNTER_BASE)
#define CPU_EVENTS_2                                (0x09C + MIB_COUNTER_BASE)
#define TX_BYTES_1                                  (0x0A0 + MIB_COUNTER_BASE)
#define TX_PKTS_1                                   (0x0A4 + MIB_COUNTER_BASE)
#define TX_BYTES_2                                  (0x0A8 + MIB_COUNTER_BASE)
#define TX_PKTS_2                                   (0x0AC + MIB_COUNTER_BASE)

/* Pre-fetch control register
*/
#define PFBRst							(1<<16)			/* Pre-fetch buffer reset */
#define DFC_OV							(1<<15)			/* DFC counter overflow flag */
#define FPFHC_OV						(1<<14)			/* DPFHC counter overflow flag */
#define CDFC								(1<<13)			/* Clear Data Fetch Counter */
#define CDPFHC							(1<<12)			/* Clear Data Pre-Fetch Hit Counter */
#define EnDFC							(1<<11)			/* Enable Data Fetch Counter */
#define EnDPFHC							(1<<10)			/* Enable Data Pre-Fetch Hit Counter */
#define EnSDPreFetch_D_MASK			(0x00000300)		/* Enable SDRAM pre-fetch function for Data : Mask */
#define EnSDPreFetch_D_Disable			(0x00000100)		/* Enable SDRAM pre-fetch function for Data : disable */
#define EnSDPreFetch_D_4W				(0x00000200)		/* Enable SDRAM pre-fetch function for Data : 4W */
#define EnSDPreFetch_D_4W4W			(0x00000300)		/* Enable SDRAM pre-fetch function for Data : 4W+4W */
#define EnSDPreFetch_D_OFFSET			(8)				/* Enable SDRAM pre-fetch function for Data : Offset */
#define IFC_OV							(1<<7)			/* IFC counter overflow flag */
#define IPFHC_OV						(1<<6)			/* IPFHC counter overflow flag */
#define CIFC								(1<<5)			/* Clear Instruction Fetch Counter */
#define CIPFHC							(1<<4)			/* Clear Instruction Pre-Fetch Hit Counter */
#define EnIFC							(1<<3)			/* Enable Instruction Fetch Counter */
#define EnIPFHC							(1<<2)			/* Enable Instruction Pre-Fetch Hit Counter */
#define EnSDPreFetch_I_MASK			(0x00000003)		/* Enable SDRAM pre-fetch function for Instruction : Mask */
#define EnSDPreFetch_I_OFFSET			(0)				/* Enable SDRAM pre-fetch function for Instruction : Offset */
#define EnSDPreFetch_I_Disable			(0x00000001)		/* Enable SDRAM pre-fetch function for Instruction : disable */
#define EnSDPreFetch_I_4W				(0x00000002)		/* Enable SDRAM pre-fetch function for Instruction : 4W */
#define EnSDPreFetch_I_4W4W			(0x00000003)		/* Enable SDRAM pre-fetch function for Instruction : 4W+4W */


/* PHY control registers 
*/
#define PHY_BASE                                    (SWCORE_BASE + 0x00002000)
#define PORT0_PHY_CONTROL                           (0x000 + PHY_BASE)
#define PORT0_PHY_STATUS                            (0x004 + PHY_BASE)
#define PORT0_PHY_IDENTIFIER_1                      (0x008 + PHY_BASE)
#define PORT0_PHY_IDENTIFIER_2                      (0x00C + PHY_BASE)
#define PORT0_PHY_AUTONEGO_ADVERTISEMENT            (0x010 + PHY_BASE)
#define PORT0_PHY_AUTONEGO_LINK_PARTNER_ABILITY     (0x014 + PHY_BASE)
#define PORT1_PHY_CONTROL                           (0x020 + PHY_BASE)
#define PORT1_PHY_STATUS                            (0x024 + PHY_BASE)
#define PORT1_PHY_IDENTIFIER_1                      (0x028 + PHY_BASE)
#define PORT1_PHY_IDENTIFIER_2                      (0x02C + PHY_BASE)
#define PORT1_PHY_AUTONEGO_ADVERTISEMENT            (0x030 + PHY_BASE)
#define PORT1_PHY_AUTONEGO_LINK_PARTNER_ABILITY     (0x034 + PHY_BASE)
#define PORT2_PHY_CONTROL                           (0x040 + PHY_BASE)
#define PORT2_PHY_STATUS                            (0x044 + PHY_BASE)
#define PORT2_PHY_IDENTIFIER_1                      (0x048 + PHY_BASE)
#define PORT2_PHY_IDENTIFIER_2                      (0x04C + PHY_BASE)
#define PORT2_PHY_AUTONEGO_ADVERTISEMENT            (0x050 + PHY_BASE)
#define PORT2_PHY_AUTONEGO_LINK_PARTNER_ABILITY     (0x054 + PHY_BASE)
#define PORT3_PHY_CONTROL                           (0x060 + PHY_BASE)
#define PORT3_PHY_STATUS                            (0x064 + PHY_BASE)
#define PORT3_PHY_IDENTIFIER_1                      (0x068 + PHY_BASE)
#define PORT3_PHY_IDENTIFIER_2                      (0x06C + PHY_BASE)
#define PORT3_PHY_AUTONEGO_ADVERTISEMENT            (0x070 + PHY_BASE)
#define PORT3_PHY_AUTONEGO_LINK_PARTNER_ABILITY     (0x074 + PHY_BASE)
#define PORT4_PHY_CONTROL                           (0x080 + PHY_BASE)
#define PORT4_PHY_STATUS                            (0x084 + PHY_BASE)
#define PORT4_PHY_IDENTIFIER_1                      (0x088 + PHY_BASE)
#define PORT4_PHY_IDENTIFIER_2                      (0x08C + PHY_BASE)
#define PORT4_PHY_AUTONEGO_ADVERTISEMENT            (0x090 + PHY_BASE)
#define PORT4_PHY_AUTONEGO_LINK_PARTNER_ABILITY     (0x094 + PHY_BASE)
#define PORT5_PHY_CONTROL                           (0x0A0 + PHY_BASE)
#define PORT5_PHY_STATUS                            (0x0A4 + PHY_BASE)
#define PORT5_PHY_IDENTIFIER_1                      (0x0A8 + PHY_BASE)
#define PORT5_PHY_IDENTIFIER_2                      (0x0AC + PHY_BASE)
#define PORT5_PHY_AUTONEGO_ADVERTISEMENT            (0x0B0 + PHY_BASE)
#define PORT5_PHY_AUTONEGO_LINK_PARTNER_ABILITY     (0x0B4 + PHY_BASE)
#define PORT6_PHY_CONTROL                           (0x0C0 + PHY_BASE)
#define PORT6_PHY_STATUS                            (0x0C4 + PHY_BASE)
#define PORT6_PHY_IDENTIFIER_1                      (0x0C8 + PHY_BASE)
#define PORT6_PHY_IDENTIFIER_2                      (0x0CC + PHY_BASE)
#define PORT6_PHY_AUTONEGO_ADVERTISEMENT            (0x0D0 + PHY_BASE)
#define PORT6_PHY_AUTONEGO_LINK_PARTNER_ABILITY     (0x0D4 + PHY_BASE)
/* PHY control register field definitions 
*/
#define PHY_RESET                                   (1 << 15)
#define ENABLE_LOOPBACK                             (1 << 14)
#define SPEED_SELECT_100M                           (1 << 13)
#define SPEED_SELECT_10M                            0
#define ENABLE_AUTONEGO                             (1 << 12)
#define POWER_DOWN                                  (1 << 11)
#define ISOLATE_PHY                                 (1 << 10)
#define RESTART_AUTONEGO                            (1 << 9)
#define SELECT_FULL_DUPLEX                          (1 << 8)
#define SELECT_HALF_DUPLEX                          0
#define SPEED_SELECT_1000M                          (1 << 6)
/* PHY status register field definitions 
*/
#define STS_CAPABLE_100BASE_T4                      (1 << 15)
#define STS_CAPABLE_100BASE_TX_FD                   (1 << 14)
#define STS_CAPABLE_100BASE_TX_HD                   (1 << 13)
#define STS_CAPABLE_10BASE_TX_FD                    (1 << 12)
#define STS_CAPABLE_10BASE_TX_HD                    (1 << 11)
#define STS_MF_PREAMBLE_SUPPRESSION                 (1 << 6)
#define STS_AUTONEGO_COMPLETE                       (1 << 5)
#define STS_REMOTE_FAULT                            (1 << 4)
#define STS_CAPABLE_NWAY_AUTONEGO                   (1 << 3)
#define STS_LINK_ESTABLISHED                        (1 << 2)
#define STS_JABBER_DETECTED                         (1 << 1)
#define STS_CAPABLE_EXTENDED                        (1 << 0)
/* PHY identifier 1 
*/
#define OUT_3_18_MASK                               (0xFFFF << 16)
#define OUT_3_18_OFFSET                             16
#define OUT_19_24_MASK                              (0x3F << 10)
#define OUT_19_24_OFFSET                            10
#define MODEL_NUMBER_MASK                           (0x3F << 4)
#define MODEL_NUMBER_OFFSET                         4
#define REVISION_NUMBER_MASK                        0x0F
#define REVISION_NUMBER_OFFSET                      0
/* PHY auto-negotiation advertisement and 
link partner ability registers field definitions
*/
#define NEXT_PAGE_ENABLED                           (1 << 15)
#define ACKNOWLEDGE                                 (1 << 14)
#define REMOTE_FAULT                                (1 << 13)
#define CAPABLE_PAUSE                               (1 << 10)
#define CAPABLE_100BASE_T4                          (1 << 9)
#define CAPABLE_100BASE_TX_FD                       (1 << 8)
#define CAPABLE_100BASE_TX_HD                       (1 << 7)
#define CAPABLE_10BASE_TX_FD                        (1 << 6)
#define CAPABLE_10BASE_TX_HD                        (1 << 5)
#define SELECTOR_MASK                               0x1F
#define SELECTOR_OFFSET                             0
#define CAP_100BASE_OFFSET							5
#define CAP_100BASE_MASK							(0x1F << CAP_100BASE_OFFSET)	/* 10/100 capability mask */
/* PHY 1000BASE-T control and 1000BASE-T Status
*/
#define CAPABLE_1000BASE_TX_FD						(1 << 9)
#define CAPABLE_1000BASE_TX_HD						(1 << 8)
#define ADVCAP_1000BASE_OFFSET						8
#define LPCAP_1000BASE_OFFSET							10
#define ADVCAP_1000BASE_MASK							(0x3 << ADVCAP_1000BASE_OFFSET)		/* advertisement capability mask */
#define LPCAP_1000BASE_MASK							(0x3 << LPCAP_1000BASE_OFFSET)		/* link partner status mask */

#define CPU_IFACE_BASE				(SYSTEM_BASE+0x10000)		/* 0xB8010000 */
#define CPUICR						(0x000 + CPU_IFACE_BASE)		/* Interface control */

#define CPURPDCR0					(0x004 + CPU_IFACE_BASE)		/* Rx pkthdr descriptor control 0 */
#define CPURPDCR1					(0x008 + CPU_IFACE_BASE)		/* Rx pkthdr descriptor control 1 */
#define CPURPDCR2					(0x00c + CPU_IFACE_BASE)		/* Rx pkthdr descriptor control 2 */
#define CPURPDCR3					(0x010 + CPU_IFACE_BASE)		/* Rx pkthdr descriptor control 3 */
#define CPURPDCR4					(0x014 + CPU_IFACE_BASE)		/* Rx pkthdr descriptor control 4 */
#define CPURPDCR5					(0x018 + CPU_IFACE_BASE)		/* Rx pkthdr descriptor control 5 */
#define CPURPDCR(idx)				(CPURPDCR0 + (idx << 2))		/* Rx pkthdr descriptor control with index */

#define CPURMDCR0					(0x01c + CPU_IFACE_BASE)		/* Rx mbuf descriptor control */
#define CPUTPDCR0					(0x020 + CPU_IFACE_BASE)		/* Tx pkthdr descriptor control Low */
#define CPUTPDCR1					(0x024 + CPU_IFACE_BASE)		/* Tx pkthdr descriptor control High */
#define CPUTPDCR(idx)				(CPUTPDCR0 + (idx << 2))		/* Tx pkthdr descriptor control with index */

#define CPUIIMR						(0x028 + CPU_IFACE_BASE)		/* Interrupt mask control */
#define CPUIISR						(0x02c + CPU_IFACE_BASE)		/* Interrupt status control */
#define CPUQDM0						(0x030 + CPU_IFACE_BASE)		/* Queue ID 0 and Descriptor Ring Mapping Register */
#define CPUQDM1						(0x032 + CPU_IFACE_BASE)		/* Queue ID 1 and Descriptor Ring Mapping Register */
#define CPUQDM2						(0x034 + CPU_IFACE_BASE)		/* Queue ID 2 and Descriptor Ring Mapping Register */
#define CPUQDM3						(0x036 + CPU_IFACE_BASE)		/* Queue ID 3 and Descriptor Ring Mapping Register */
#define CPUQDM4						(0x038 + CPU_IFACE_BASE)		/* Queue ID 4 and Descriptor Ring Mapping Register */
#define CPUQDM5						(0x03a + CPU_IFACE_BASE)		/* Queue ID 5 and Descriptor Ring Mapping Register */


/* CPUICR - CPU interface control register field definitions 
*/
#define TXCMD						(1 << 31)			/* Enable Tx */
#define RXCMD						(1 << 30)			/* Enable Rx */
#define BUSBURST_32WORDS			0
#define BUSBURST_64WORDS			(1 << 28)
#define BUSBURST_128WORDS			(2 << 28)
#define BUSBURST_256WORDS			(3 << 28)
#define MBUF_128BYTES				0
#define MBUF_256BYTES				(1 << 24)
#define MBUF_512BYTES				(2 << 24)
#define MBUF_1024BYTES                      (3 << 24)
#define MBUF_2048BYTES                      (4 << 24)
#define TXFD							(1 << 23)			/* Notify Tx descriptor fetch */
#define SOFTRST						(1 << 22)			/* Re-initialize all descriptors */
#define STOPTX						(1 << 21)			/* Stop Tx */
#define SWINTSET					(1 << 20)			/* Set software interrupt */
#define LBMODE						(1 << 19)			/* Loopback mode */
#define LB10MHZ						(1 << 18)			/* LB 10MHz */
#define LB100MHZ					(1 << 18)			/* LB 100MHz */
#if defined(CONFIG_RTL865XB)||defined(CONFIG_RTL865XC)
#define MITIGATION					(1 << 17)			/* Mitigation with timer1 */
#define EXCLUDE_CRC					(1 << 16)			/* Exclude CRC from length */
#define RX_SHIFT_OFFSET			0
#endif /* CONFIG_RTL865XB */

/*
	CPU interface descriptor field defintions 
*/
#define DESC_OWNED_BIT					(1 << 0)
#define DESC_RISC_OWNED				(0 << 0)
#define DESC_SWCORE_OWNED			(1 << 0)

#define DESC_WRAP						(1 << 1)

#define DESC_ENG_OWNED				1


/* CPUIIMR - CPU interface interrupt mask register field definitions */
#define LINK_CHANGE_IE					(1 << 31)			/* Link change interrupt enable */

#define RX_ERR_IE0						(1 << 25)			/* Rx error interrupt enable for descriptor 0 */
#define RX_ERR_IE1						(1 << 26)			/* Rx error interrupt enable for descriptor 1 */
#define RX_ERR_IE2						(1 << 27)			/* Rx error interrupt enable for descriptor 2 */
#define RX_ERR_IE3						(1 << 28)			/* Rx error interrupt enable for descriptor 3 */
#define RX_ERR_IE4						(1 << 29)			/* Rx error interrupt enable for descriptor 4 */
#define RX_ERR_IE5						(1 << 30)			/* Rx error interrupt enable for descriptor 5 */
#define RX_ERR_IE_ALL					(0x3f<<25)			/* Rx error interrupt enable for any descriptor */

#define TX_ERR_IE0						(1 << 23)			/* Tx error interrupt pending for descriptor 0 */
#define TX_ERR_IE1						(1 << 24)			/* Tx error interrupt pending for descriptor 1 */
#define TX_ERR_IE_ALL					(0x3<<23)			/* Tx error interrupt pending for any descriptor */
#define TX_ERR_IE						(1 << 29)			/* Tx error interrupt enable */

#define PKTHDR_DESC_RUNOUT_IE0		(1 << 17)			/* Run out pkthdr descriptor 0 interrupt enable */
#define PKTHDR_DESC_RUNOUT_IE1		(1 << 18)			/* Run out pkthdr descriptor 1 interrupt enable */
#define PKTHDR_DESC_RUNOUT_IE2		(1 << 19)			/* Run out pkthdr descriptor 2 interrupt enable */
#define PKTHDR_DESC_RUNOUT_IE3		(1 << 20)			/* Run out pkthdr descriptor 3 interrupt enable */
#define PKTHDR_DESC_RUNOUT_IE4		(1 << 21)			/* Run out pkthdr descriptor 4 interrupt enable */
#define PKTHDR_DESC_RUNOUT_IE5		(1 << 22)			/* Run out pkthdr descriptor 5 interrupt enable */
#define PKTHDR_DESC_RUNOUT_IE_ALL	(0x3f << 17)			/* Run out anyone pkthdr descriptor interrupt pending */

#define MBUF_DESC_RUNOUT_IE_ALL		(1 << 11)			/* Run out anyone mbuf interrupt pending */
#define MBUF_DESC_RUNOUT_IE0			(1 << 11)			/* Run out mbuf descriptor 0 interrupt enable */

#define TX_DONE_IE0						(1 << 9)				/* Tx Descript Ring 0 one packet done interrupt enable */
#define TX_DONE_IE1						(1 << 10)			/* Tx Descript Ring 1 one packet done interrupt enable */
#define TX_DONE_IE_ALL					(0x3 << 9)			/* Any Tx Descript Ring one packet done interrupt enable */

#define RX_DONE_IE0						(1 << 3)				/* Rx Descript Ring 0 one packet done interrupt enable */
#define RX_DONE_IE1						(1 << 4)				/* Rx Descript Ring 1 one packet done interrupt enable */
#define RX_DONE_IE2						(1 << 5)				/* Rx Descript Ring 2 one packet done interrupt enable */
#define RX_DONE_IE3						(1 << 6)				/* Rx Descript Ring 3 one packet done interrupt enable */
#define RX_DONE_IE4						(1 << 7)				/* Rx Descript Ring 4 one packet done interrupt enable */
#define RX_DONE_IE5						(1 << 8)				/* Rx Descript Ring 5 one packet done interrupt enable */
#define RX_DONE_IE_ALL					(0x3f << 3)			/* Rx Descript Ring any one packet done interrupt enable */

#define TX_ALL_DONE_IE0				(1 << 1)				/* Tx Descript Ring 0 all packets done interrupt enable */
#define TX_ALL_DONE_IE1				(1 << 2)				/* Tx Descript Ring 1 all packets done interrupt enable */
#define TX_ALL_DONE_IE_ALL				(0x3 << 1)			/* Any Tx Descript Ring all packets done interrupt enable */

/* CPU interface interrupt status register field definitions */
#define LINK_CHANGE_IP					(1 << 31)			/* Link change interrupt pending */

#define RX_ERR_IP0						(1 << 25)			/* Rx error descriptor 0 interrupt pending */
#define RX_ERR_IP1						(1 << 26)			/* Rx error descriptor 1 interrupt pending */
#define RX_ERR_IP2						(1 << 27)			/* Rx error descriptor 2 interrupt pending */
#define RX_ERR_IP3						(1 << 28)			/* Rx error descriptor 3 interrupt pending */
#define RX_ERR_IP4						(1 << 29)			/* Rx error descriptor 4 interrupt pending */
#define RX_ERR_IP5						(1 << 30)			/* Rx error descriptor 5 interrupt pending */
#define RX_ERR_IP_ALL					(0x3f<<25)			/* Rx error any descriptor interrupt pending */
#define RX_ERR_IP(idx)					(1 << (25+(idx)))		/* Rx error descriptor [IDX] interrupt pending */

#define TX_ERR_IP0						(1 << 23)			/* Tx error descriptor 0 interrupt pending */
#define TX_ERR_IP1						(1 << 24)			/* Tx error descriptor 1 interrupt pending */
#define TX_ERR_IP_ALL					(0x3<<23)			/* Tx error any descriptor interrupt pending */
#define TX_ERR_IP(idx)					(1 << (23+(idx)))		/* Tx error descriptor [IDX] interrupt pending */

#define PKTHDR_DESC_RUNOUT_IP0		(1 << 17)			/* Run out pkthdr descriptor 0 interrupt pending */
#define PKTHDR_DESC_RUNOUT_IP1		(1 << 18)			/* Run out pkthdr descriptor 1 interrupt pending */
#define PKTHDR_DESC_RUNOUT_IP2		(1 << 19)			/* Run out pkthdr descriptor 2 interrupt pending */
#define PKTHDR_DESC_RUNOUT_IP3		(1 << 20)			/* Run out pkthdr descriptor 3 interrupt pending */
#define PKTHDR_DESC_RUNOUT_IP4		(1 << 21)			/* Run out pkthdr descriptor 4 interrupt pending */
#define PKTHDR_DESC_RUNOUT_IP5		(1 << 22)			/* Run out pkthdr descriptor 5 interrupt pending */
#define PKTHDR_DESC_RUNOUT_IP_ALL	(0x3f << 17)			/* Run out anyone pkthdr descriptor interrupt pending */
#define PKTHDR_DESC_RUNOUT_IP(idx)		(1 << (17+(idx)))		/* Run out pkthdr descriptor [IDX] interrupt pending */

#define MBUF_DESC_RUNOUT_IP_ALL		(1 << 11)			/* Run out anyone mbuf interrupt pending */
#define MBUF_DESC_RUNOUT_IP0			(1 << 11)			/* Run out mbuf descriptor 0 interrupt pending */

#define TX_DONE_IP0						(1 << 9)				/* Tx one packet done interrupt for descriptor 0 pending */
#define TX_DONE_IP1						(1 << 10)			/* Tx one packet done interrupt for descriptor 1 pending */
#define TX_DONE_IP_ALL					(0x3 << 9)			/* Tx one packet done interrupt for any descriptor pending */
#define TX_DONE_IP(idx)					(1 << (9+(idx)))		/* Tx one packet done interrupt for descriptor [IDX] pending */

#define RX_DONE_IP0						(1 << 3)				/* Rx one packet done 0 interrupt pending */
#define RX_DONE_IP1						(1 << 4)				/* Rx one packet done 1 interrupt pending */
#define RX_DONE_IP2						(1 << 5)				/* Rx one packet done 2 interrupt pending */
#define RX_DONE_IP3						(1 << 6)				/* Rx one packet done 3 interrupt pending */
#define RX_DONE_IP4						(1 << 7)				/* Rx one packet done 4 interrupt pending */
#define RX_DONE_IP5						(1 << 8)				/* Rx one packet done 5 interrupt pending */
#define RX_DONE_IP_ALL					(0x3f << 3)			/* Rx one packet done anyone interrupt pending */
#define RX_DONE_IP(idx)					(1 << (3+(idx)))		/* Rx one packet done [IDX] interrupt pending */

#define TX_ALL_DONE_IP0				(1 << 1)				/* Tx all packets done interrupt 0 pending */
#define TX_ALL_DONE_IP1				(1 << 2)				/* Tx all packets done interrupt 1 pending */
#define TX_ALL_DONE_IP_ALL				(0x03 << 1)			/* Tx all packets done any interrupt pending */
#define TX_ALL_DONE_IP(idx)				(1 << (1+(idx)))		/* Tx all packets done interrupt [IDX] pending */

#define INTPENDING_NIC_MASK			(RX_ERR_IP_ALL|TX_ERR_IP_ALL|RX_DONE_IP_ALL|PKTHDR_DESC_RUNOUT_IP_ALL)
#define INTPENDING_RUN_OUT			(PKTHDR_DESC_RUNOUT_IP_ALL)

/* CPUQDM0,CPUQDM1,CPUQDM2,CPUQDM3,CPUQDM4,CPUQDM5 - Queue ID 0~5 and Descriptor Ring Mapping Register */
#define CPURxDesc_OFFSET                12
#define CPURxDesc_MASK                  (7<<12)
#define Ext1RxDesc_OFFSET               8
#define Ext1RxDesc_MASK                 (7<<8)
#define Ext2RxDesc_OFFSET               4
#define Ext2RxDesc_MASK                 (7<<4)
#define Ext3RxDesc_OFFSET               0
#define Ext3RxDesc_MASK                 (7<<0)
#define CPU_RX_DESC_SHIFT				12
#define EXT1_RX_DESC_SHIFT				8
#define EXT2_RX_DESC_SHIFT				4
#define EXT3_RX_DESC_SHIFT				0




/* Switch Core Control Registers 
*/
#define SWCORECNR                           (SWCORE_BASE + 0x00006000)

#define MACMR                               (0x004 + SWCORECNR)   /* MAC monitor */
#define VLANTCR                             (0x008 + SWCORECNR)   /* Vlan tag control */
#define DSCR0                               (0x00C + SWCORECNR)   /* Qos by DS control */
#define DSCR1                               (0x010 + SWCORECNR)   /* Qos by DS control */
#define QOSCR                               (0x014 + SWCORECNR)   /* Qos control */
#if 0 /* phase out in 865xC */
#define SWTMCR                              (0x01C + SWCORECNR)   /* Switch table misc control */
#endif

#if 0 /* phase out in 865xC */
/* SWTMCR - Switch table misc control */
#define EN_BCAST                            (1<<30)               /* Enable Broadcast Handling */
#define MCAST_TO_CPU                        (1<<29)               /* Enable Multicast Table */
#define BRIDGE_PKT_TO_CPU                   (1<<27)               /*802.1d packet to CPU*/
#define MCAST_PORT_EXT_MODE_OFFSET          5                     /* Multicast port mode offset */
#define MCAST_PORT_EXT_MODE_MASK            (0x3f << 5)           /* Multicast port mode mask */
#define WAN_ROUTE_MASK                      (3 << 3)
#define WAN_ROUTE_FORWARD                   (0 << 3)              /* Route WAN packets */
#define WAN_ROUTE_TO_CPU                    (1 << 3)              /* Forward WAN packets to CPU */
#define WAN_ROUTE_DROP                      (2 << 3)              /* Drop WAN packets */
#endif

/* Descriptor Diagnostic Register */
#define DESCDIAG_BASE			(SWCORECNR + 0x0100)
#define GDSR0					(DESCDIAG_BASE + 0x000)         /* Global Descriptor Status Register 0 */
#define GDSR1					(DESCDIAG_BASE + 0x004)         /* Global Descriptor Status Register 1 */
#define PCSR0					(DESCDIAG_BASE + 0x008)         /* Port Congestion Status Register 0 */
#define PCSR1					(DESCDIAG_BASE + 0x00c)         /* Port Congestion Status Register 1 */
#define P0_DCR0					(DESCDIAG_BASE + 0x010)         /* Port 0 Descriptor Counter Register 0 */
#define P0_DCR1					(DESCDIAG_BASE + 0x014)         /* Port 0 Descriptor Counter Register 1 */
#define P0_DCR2					(DESCDIAG_BASE + 0x018)         /* Port 0 Descriptor Counter Register 2 */
#define P0_DCR3					(DESCDIAG_BASE + 0x01c)         /* Port 0 Descriptor Counter Register 3 */
#define P1_DCR0					(DESCDIAG_BASE + 0x020)         /* Port 1 Descriptor Counter Register 0 */
#define P1_DCR1					(DESCDIAG_BASE + 0x024)         /* Port 1 Descriptor Counter Register 1 */
#define P1_DCR2					(DESCDIAG_BASE + 0x028)         /* Port 1 Descriptor Counter Register 2 */
#define P1_DCR3					(DESCDIAG_BASE + 0x02c)         /* Port 1 Descriptor Counter Register 3 */
#define P2_DCR0					(DESCDIAG_BASE + 0x030)         /* Port 2 Descriptor Counter Register 0 */
#define P2_DCR1					(DESCDIAG_BASE + 0x034)         /* Port 2 Descriptor Counter Register 1 */
#define P2_DCR2					(DESCDIAG_BASE + 0x038)         /* Port 2 Descriptor Counter Register 2 */
#define P2_DCR3					(DESCDIAG_BASE + 0x03c)         /* Port 2 Descriptor Counter Register 3 */
#define P3_DCR0					(DESCDIAG_BASE + 0x040)         /* Port 3 Descriptor Counter Register 0 */
#define P3_DCR1					(DESCDIAG_BASE + 0x044)         /* Port 3 Descriptor Counter Register 1 */
#define P3_DCR2					(DESCDIAG_BASE + 0x048)         /* Port 3 Descriptor Counter Register 2 */
#define P3_DCR3					(DESCDIAG_BASE + 0x04c)         /* Port 3 Descriptor Counter Register 3 */
#define P4_DCR0					(DESCDIAG_BASE + 0x050)         /* Port 4 Descriptor Counter Register 0 */
#define P4_DCR1					(DESCDIAG_BASE + 0x054)         /* Port 4 Descriptor Counter Register 1 */
#define P4_DCR2					(DESCDIAG_BASE + 0x058)         /* Port 4 Descriptor Counter Register 2 */
#define P4_DCR3					(DESCDIAG_BASE + 0x05c)         /* Port 4 Descriptor Counter Register 3 */
#define P5_DCR0					(DESCDIAG_BASE + 0x060)         /* Port 5 Descriptor Counter Register 0 */
#define P5_DCR1					(DESCDIAG_BASE + 0x064)         /* Port 5 Descriptor Counter Register 1 */
#define P5_DCR2					(DESCDIAG_BASE + 0x068)         /* Port 5 Descriptor Counter Register 2 */
#define P5_DCR3					(DESCDIAG_BASE + 0x06c)         /* Port 5 Descriptor Counter Register 3 */
#define P6_DCR0					(DESCDIAG_BASE + 0x070)         /* Port CPU Descriptor Counter Register 0 */
#define P6_DCR1					(DESCDIAG_BASE + 0x074)         /* Port CPU Descriptor Counter Register 1 */
#define P6_DCR2					(DESCDIAG_BASE + 0x078)         /* Port CPU Descriptor Counter Register 2 */
#define P6_DCR3					(DESCDIAG_BASE + 0x07c)         /* Port CPU Descriptor Counter Register 3 */

#define Pn_DSCR0(idx)			(DESCDIAG_BASE + ((idx) * 0x10) + 0x010)
#define Pn_DSCR1(idx)			(DESCDIAG_BASE + ((idx) * 0x10) + 0x014)
#define Pn_DSCR2(idx)			(DESCDIAG_BASE + ((idx) * 0x10) + 0x018)
#define Pn_DSCR3(idx)			(DESCDIAG_BASE + ((idx) * 0x10) + 0x01c)

#define Pn_IQDesCnt_OFFSET		(0)
#define Pn_IQDesCnt_MASK		(0x3ff << Pn_IQDesCnt_OFFSET)

#define Pn_OQDSCR_MASK				(0x3ff)
#define Pn_ODD_OQDSCR_OFFSET		(16)
#define Pn_EVEN_OQDSCR_OFFSET		(0)

#define Pn_OQDSCR(port, queue)	\
	( \
	((queue) == 0) || ((queue) == 1)? \
		((queue & 0x01)?((READ_MEM32(Pn_DSCR0(port)) >> Pn_ODD_OQDSCR_OFFSET) & Pn_OQDSCR_MASK) : \
						((READ_MEM32(Pn_DSCR0(port)) >> Pn_EVEN_OQDSCR_OFFSET) & Pn_OQDSCR_MASK)): \
	((queue) == 2) || ((queue) == 3)? \
		((queue & 0x01)?((READ_MEM32(Pn_DSCR1(port)) >> Pn_ODD_OQDSCR_OFFSET) & Pn_OQDSCR_MASK) : \
						((READ_MEM32(Pn_DSCR1(port)) >> Pn_EVEN_OQDSCR_OFFSET) & Pn_OQDSCR_MASK)): \
	((queue) == 4) || ((queue) == 5)? \
		((queue & 0x01)?((READ_MEM32(Pn_DSCR2(port)) >> Pn_ODD_OQDSCR_OFFSET) & Pn_OQDSCR_MASK) : \
						((READ_MEM32(Pn_DSCR2(port)) >> Pn_EVEN_OQDSCR_OFFSET) & Pn_OQDSCR_MASK)): \
	0)

#define Pn_IQDSCR(port)	((READ_MEM32(Pn_DSCR3(port)) & Pn_IQDesCnt_MASK) >> Pn_IQDesCnt_OFFSET)

/* GDSR0 - Global Descriptor Status Register 0 */
#define DSCRUNOUT					(1<<27)					/* Descriptor Run Out */
#define TotalDscFctrl_Flag			(1<<26)					/* TotalDescriptor Flow Control event flag */
#define USEDDSC_OFFSET				16						/* Total Used Descriptor */
#define USEDDSC_MASK				(0x3ff<<16)				/* Total Used Descriptor */
#define SharedBufFCON_Flag			(1<<14)					/* SharedBufFCON threshold triggerred flag */
#define MaxUsedDsc_OFFSET			0						/* Max Used Descriptor Count History */
#define MaxUsedDsc_MASK			(0x3ff<<0)				/* Max Used Descriptor Count History */

/* PCSR0 - Port Congestion Status Register 0 */
#define P3OQCgst_OFFSET			(24)
#define P3OQCgst_MASK			(0x7f << P3OQCgst_OFFSET)
#define P2OQCgst_OFFSET			(16)
#define P2OQCgst_MASK			(0x7f << P2OQCgst_OFFSET)
#define P1OQCgst_OFFSET			(8)
#define P1OQCgst_MASK			(0x7f << P1OQCgst_OFFSET)
#define P0OQCgst_OFFSET			(0)
#define P0OQCgst_MASK			(0x7f << P0OQCgst_OFFSET)

#define P3QQCgst(queue)			(((READ_MEM32(PCSR0) & P3OQCgst_MASK) >> P3OQCgst_OFFSET) & (1 << (queue)))
#define P2QQCgst(queue)			(((READ_MEM32(PCSR0) & P2OQCgst_MASK) >> P2OQCgst_OFFSET) & (1 << (queue)))
#define P1QQCgst(queue)			(((READ_MEM32(PCSR0) & P1OQCgst_MASK) >> P1OQCgst_OFFSET) & (1 << (queue)))
#define P0QQCgst(queue)			(((READ_MEM32(PCSR0) & P0OQCgst_MASK) >> P0OQCgst_OFFSET) & (1 << (queue)))

/* PCSR1 - Port Congestion Status Register 1 */
#define IQCgst_OFFSET			(24)
#define IQCgst_MASK				(0x7f << P3OQCgst_OFFSET)
#define P6OQCgst_OFFSET			(16)
#define P6OQCgst_MASK			(0x7f << P6OQCgst_OFFSET)
#define P5OQCgst_OFFSET			(8)
#define P5OQCgst_MASK			(0x7f << P5OQCgst_OFFSET)
#define P4OQCgst_OFFSET			(0)
#define P4OQCgst_MASK			(0x7f << P4OQCgst_OFFSET)

#define IQCgst(port)				(((READ_MEM32(PCSR1) & IQCgst_MASK) >> IQCgst_OFFSET) & (1 << (port)))
#define P6QQCgst(queue)			(((READ_MEM32(PCSR1) & P6OQCgst_MASK) >> P6OQCgst_OFFSET) & (1 << (queue)))
#define P5QQCgst(queue)			(((READ_MEM32(PCSR1) & P5OQCgst_MASK) >> P5OQCgst_OFFSET) & (1 << (queue)))
#define P4QQCgst(queue)			(((READ_MEM32(PCSR1) & P4OQCgst_MASK) >> P4OQCgst_OFFSET) & (1 << (queue)))


#define TMCR                                (0x300 + SWCORECNR)   /* Test mode control */
#if 1 /* According David Lu 2006/02/10 */
#define MIITM_TXR0                          (0x400 + SWCORECNR)   /* MAC Test Mode MII TX Interface Register */
#define MIITM_TXR1                          (0x404 + SWCORECNR)   /* MAC Test Mode MII TX Interface Register */
#define MIITM_RXR0                          (0x408 + SWCORECNR)   /* MAC Test Mode MII RX Interface Register */
#define MIITM_RXR1                          (0x40C + SWCORECNR)   /* MAC Test Mode MII RX Interface Register */
#define GMIITM_TXR0                         (0x400 + SWCORECNR)   /* MAC Test Mode MII TX Interface Register */
#define GMIITM_TXR1                         (0x404 + SWCORECNR)   /* MAC Test Mode MII TX Interface Register */
#define GMIITM_RXR0                         (0x408 + SWCORECNR)   /* MAC Test Mode MII RX Interface Register */
#define GMIITM_RXR1                         (0x40C + SWCORECNR)   /* MAC Test Mode MII RX Interface Register */
#else /* Before 2006/02/09 */
#define MIITM_TXR                           (0x400 + SWCORECNR)   /* MAC Test Mode MII TX Interface Register */
#define MIITM_RXR                           (0x404 + SWCORECNR)   /* MAC Test Mode MII RX Interface Register */
#endif


#define EN_PHY_P4                           (1 << 9)
#define EN_PHY_P3                           (1 << 8)
#define EN_PHY_P2                           (1 << 7)
#define EN_PHY_P1                           (1 << 6)
#define EN_PHY_P0                           (1 << 5)
#define EN_FX_P4                           (1 << 4)
#define EN_FX_P3                           (1 << 3)
#define EN_FX_P2                           (1 << 2)
#define EN_FX_P1                           (1 << 1)
#define EN_FX_P0                           (1 << 0)

#define GUEST_VLAN_BITMASK		0xFFF
#define MULTICAST_BROADCAST_BIT	0x1
#define PCRP_SIZE					4
#define STP_PortST					5
#define STP_PortST_BITMASK			3



/* MAC monitor register field definitions 
*/
#define SYS_CLK_MASK                        (0x7 << 16)
#define SYS_CLK_100M                        (0 << 16)
#define SYS_CLK_90M                         (1 << 16)
#define SYS_CLK_85M                         (2 << 16)
#define SYS_CLK_96M                         (3 << 16)
#define SYS_CLK_80M                         (4 << 16)
#define SYS_CLK_75M                         (5 << 16)
#define SYS_CLK_70M                         (6 << 16)
#define SYS_CLK_50M                         (7 << 16)
/* VLAN tag control register field definitions 
*/
#define VLAN_TAG_ONLY                       (1 << 19)   /* Only accept tagged packets */
#define MII_ENFORCE_MODE			(1 << 4)	  /* Enable MII port property set by force mode */
/* Qos by DS control register 
*/
/* Qos control register 
*/
#define QWEIGHT_MASK                        (3 << 30)
#define QWEIGHT_ALWAYS_H                    (3 << 30)   /* Weighted round robin of priority always high first */
#define QWEIGHT_16TO1                       (2 << 30)   /* Weighted round robin of priority queue 16:1 */
#define QWEIGHT_8O1                         (1 << 30)   /* Weighted round robin of priority queue 8:1 */
#define QWEIGHT_4TO1                        0           /* Weighted round robin of priority queue 4:1 */
#define EN_FCA_AUTOOFF                      (1 << 29)   /* Enable flow control auto off */
#define DIS_DS_PRI                          (1 << 28)   /* Disable DS priority */
#define DIS_VLAN_PRI                        (1 << 27)   /* Disable 802.1p priority */
#define PORT5_H_PRI                         (1 << 26)   /* Port 5 high priority */
#define PORT4_H_PRI                         (1 << 25)   /* Port 4 high priority */
#define PORT3_H_PRI                         (1 << 24)   /* Port 3 high priority */
#define PORT2_H_PRI                         (1 << 23)   /* Port 2 high priority */
#define PORT1_H_PRI                         (1 << 22)   /* Port 1 high priority */
#define PORT0_H_PRI                         (1 << 21)   /* Port 0 high priority */
#define EN_QOS                              (1 << 20)   /* Enable QoS */
/* Switch core misc control register field definitions 
*/
#define DIS_P5_LOOPBACK                     (1 << 30)   /* Disable port 5 loopback */

#define LINK_RGMII							0			/* RGMII mode */
#define LINK_MII_MAC						1			/* GMII/MII MAC auto mode */
#define LINK_MII_PHY						2			/* GMII/MII PHY auto mode */
#define LINKMODE_OFFSET						23			/* Link type offset */
#define P5_LINK_RGMII						LINK_RGMII				/* Port 5 RGMII mode */
#define P5_LINK_MII_MAC                     LINK_MII_MAC			/* Port 5 GMII/MII MAC auto mode */
#define P5_LINK_MII_PHY                     LINK_MII_PHY			/* Port 5 GMII/MII PHY auto mode */
#define P5_LINK_OFFSET                      LINKMODE_OFFSET			/* Port 5 link type offset */
/*#define P4_USB_SEL                          (1 << 25)    Select port USB interface */
#define EN_P5_LINK_PHY                      (1 << 26)   /* Enable port 5 PHY provides link status to MAC */
#define EN_P4_LINK_PHY                      (1 << 25)   /* Enable port 4 PHY provides link status to MAC */
#define EN_P3_LINK_PHY                      (1 << 24)   /* Enable port 3 PHY provides link status to MAC */
#define EN_P2_LINK_PHY                      (1 << 23)   /* Enable port 2 PHY provides link status to MAC */
#define EN_P1_LINK_PHY                      (1 << 22)   /* Enable port 1 PHY provides link status to MAC */
#define EN_P0_LINK_PHY                      (1 << 21)   /* Enable port 0 PHY provides link status to MAC */




/*When enabled, the authorized node are allowed in one direction ("IN" direction)
to forward packet to the unauthorized node. Otherwise, it is not allowed. 
0= BOTH direction. (control for the packets that "SA=" or "DA=" this MAC address.) 
1= IN direction. (control for the packets that "SA=" this MAC address.)	*/
#define EN_8021X_TOGVLAN							( DOT1X_PROCESS_GVLAN<<GVLAN_PROCESS_BITS)			
#define EN_8021X_TOCPU								( DOT1X_PROCESS_TOCPU<<GVLAN_PROCESS_BITS)			
#define EN_8021X_DROP								( DOT1X_PROCESS_DROP<<GVLAN_PROCESS_BITS)			




#if 1 /* According David Lu 2006/02/10 */
/* Test mode Tx MII-like register field definitions 
*/
#define P0TxEN                              (1<<24)     /* Enable port 0 Mii Tx signal (1 - data valid) */
#define P1TxEN                              (1<<25)     /* Enable port 1 Mii Tx signal (1 - data valid) */
#define P2TxEN                              (1<<26)     /* Enable port 2 Mii Tx signal (1 - data valid) */
#define P0TxEN_OFFSET                       (24)     /* Enable port 0 Mii Tx */
#define P1TxEN_OFFSET                       (25)     /* Enable port 1 Mii Tx */
#define P2TxEN_OFFSET                       (26)     /* Enable port 2 Mii Tx */
#define P0TXD_OFFSET                        (0)     /* Port 0 TxD [7:0] */
#define P1TXD_OFFSET                        (8)     /* Port 1 TxD [7:0] */
#define P2TXD_OFFSET                        (16)     /* Port 2 TxD [7:0] */

#define P3TxEN                              (1<<24)     /* Enable port 3 Mii Tx signal (1 - data valid) */
#define P4TxEN                              (1<<25)     /* Enable port 4 Mii Tx signal (1 - data valid) */
#define P5TxEN                              (1<<26)     /* Enable port 5 Mii Tx signal (1 - data valid) */
#define P3TxEN_OFFSET                       (24)     /* Enable port 3 Mii Tx */
#define P4TxEN_OFFSET                       (25)     /* Enable port 4 Mii Tx */
#define P5TxEN_OFFSET                       (26)     /* Enable port 5 Mii Tx */
#define P3TXD_OFFSET                        (0)     /* Port 3 TxD [7:0] */
#define P4TXD_OFFSET                        (8)     /* Port 4 TxD [7:0] */
#define P5TXD_OFFSET                        (16)     /* Port 5 TxD [7:0] */
/* Test mode Rx MII-like register field definitions 
*/
#define P0RxEN                              (1<<24)     /* Enable port 0 Mii Rx signal (1 - data valid) */
#define P1RxEN                              (1<<25)     /* Enable port 1 Mii Rx signal (1 - data valid) */
#define P2RxEN                              (1<<26)     /* Enable port 2 Mii Rx signal (1 - data valid) */
#define P0RxEN_OFFSET                       (24)     /* Enable port 0 Mii Rx */
#define P1RxEN_OFFSET                       (25)     /* Enable port 1 Mii Rx */
#define P2RxEN_OFFSET                       (26)     /* Enable port 2 Mii Rx */
#define P0RXD_OFFSET                        (0)     /* Port 0 RxD [7:0] */
#define P1RXD_OFFSET                        (8)     /* Port 1 RxD [7:0] */
#define P2RXD_OFFSET                        (16)     /* Port 2 RxD [7:0] */

#define P3RxEN                              (1<<24)     /* Enable port 3 Mii Rx signal (1 - data valid) */
#define P4RxEN                              (1<<25)     /* Enable port 4 Mii Rx signal (1 - data valid) */
#define P5RxEN                              (1<<26)     /* Enable port 5 Mii Rx signal (1 - data valid) */
#define P3RxEN_OFFSET                       (24)     /* Enable port 3 Mii Rx */
#define P4RxEN_OFFSET                       (25)     /* Enable port 4 Mii Rx */
#define P5RxEN_OFFSET                       (26)     /* Enable port 5 Mii Rx */
#define P3RXD_OFFSET                        (0)     /* Port 3 RxD [7:0] */
#define P4RXD_OFFSET                        (8)     /* Port 4 RxD [7:0] */
#define P5RXD_OFFSET                        (16)     /* Port 5 RxD [7:0] */
#else /* Before 2006/02/09 */
/* Test mode Rx MII-like register field definitions 
*/
#define P5_RXDV                             (1 << 30)   /* Enable port 0 MII RXDV signal */
#define P4_RXDV                             (1 << 29)   /* Enable port 1 MII RXDV signal */
#define P3_RXDV                             (1 << 28)   /* Enable port 2 MII RXDV signal */
#define P2_RXDV                             (1 << 27)   /* Enable port 3 MII RXDV signal */
#define P1_RXDV                             (1 << 26)   /* Enable port 4 MII RXDV signal */
#define P0_RXDV                             (1 << 25)   /* Enable port 5 MII RXDV signal */
#define NIBBLE_MASK                         0x0F        /* Mask for a nibble */
#define P5_RXD_OFFSET                       20          /* Port 0 RXD MII signal */
#define P4_RXD_OFFSET                       16          /* Port 1 RXD MII signal */
#define P3_RXD_OFFSET                       12          /* Port 2 RXD MII signal */
#define P2_RXD_OFFSET                       8           /* Port 3 RXD MII signal */
#define P1_RXD_OFFSET                       4           /* Port 4 RXD MII signal */
#define P0_RXD_OFFSET                       0           /* Port 5 RXD MII signal */
/* Test mode Tx MII-like register field definitions 
*/
#define P5_TXDV                             (1 << 30)   /* Enable port 0 MII TXDV signal */
#define P4_TXDV                             (1 << 29)   /* Enable port 1 MII TXDV signal */
#define P3_TXDV                             (1 << 28)   /* Enable port 2 MII TXDV signal */
#define P2_TXDV                             (1 << 27)   /* Enable port 3 MII TXDV signal */
#define P1_TXDV                             (1 << 26)   /* Enable port 4 MII TXDV signal */
#define P0_TXDV                             (1 << 25)   /* Enable port 5 MII TXDV signal */
#define P5_TXD_OFFSET                       20          /* Port 0 TXD MII signal */
#define P4_TXD_OFFSET                       16          /* Port 1 TXD MII signal */
#define P3_TXD_OFFSET                       12          /* Port 2 TXD MII signal */
#define P2_TXD_OFFSET                       8           /* Port 3 TXD MII signal */
#define P1_TXD_OFFSET                       4           /* Port 4 TXD MII signal */
#define P0_TXD_OFFSET                       0           /* Port 5 TXD MII signal */
#endif

/* Test mode enable register 
*/
#define TX_TEST_PORT_OFFSET                 26          /* Tx test mode enable port offset */
#define RX_TEST_PORT_OFFSET                 18          /* Rx test mode enable port offset */
#define MiiTxPktRDY_OFFSET                  10          /* Status flag of MII TX packet ready to send on port */
#define HSABUSY								(1 << 2)	/* HSA process ready flag */
#define ENHSBTESTMODE						(1 << 1)	/* Enable HSB Test Mode */
#define HSB_RDY 	   	   	   	   	   	   	(1 << 0)   	/* HSB ready */
/* Gateway MAC low register 
*/
#define GMACL_OFFSET                        20          /* Gateway MAC[15:4] offset */
#define GMACL_MASK                          (0xFFFFFFFF << 20)  /* Gateway MAC[15:4] mask */




#define SWMACCR_BASE                        (SWCORE_BASE+0x4000)
/* MAC control register field definitions */
#define MACCR                               (0x000+SWMACCR_BASE)     /* MAC Configuration Register */
#define MDCIOCR                             (0x004+SWMACCR_BASE)      /* MDC/MDIO Command */
#define MDCIOSR                             (0x008+SWMACCR_BASE)      /* MDC/MDIO Status */
#define PMCR                                (0x00C+SWMACCR_BASE)      /* Port Mirror Control Register */
#define PPMAR                               (0x010+SWMACCR_BASE)     /* Per port matching action */
#define PATP0                               (0x014+SWMACCR_BASE)     /* Pattern for port 0 */
#define PATP1                               (0x018+SWMACCR_BASE)     /* Pattern for port 1 */
#define PATP2                               (0x01C+SWMACCR_BASE)     /* Pattern for port 2 */
#define PATP3                               (0x020+SWMACCR_BASE)     /* Pattern for port 3 */
#define PATP4                               (0x024+SWMACCR_BASE)     /* Pattern for port 4 */
#define PATP5                               (0x028+SWMACCR_BASE)     /* Pattern for port 5 */
#define MASKP0                              (0x02C+SWMACCR_BASE)     /* Mask for port 0 */
#define MASKP1                              (0x030+SWMACCR_BASE)     /* Mask for port 1 */
#define MASKP2                              (0x034+SWMACCR_BASE)     /* Mask for port 2 */
#define MASKP3                              (0x038+SWMACCR_BASE)     /* Mask for port 3 */
#define MASKP4                              (0x03C+SWMACCR_BASE)     /* Mask for port 4 */
#define MASKP5                              (0x040+SWMACCR_BASE)     /* Mask for port 5 */
#define BSCR								(0x044+SWMACCR_BASE)	 /* Broadcast Storm Control Rate Configuration */
#define CSCR                                (0x048+SWMACCR_BASE)     /* Checksum Control Register */
#define CCR                                 (0x048+SWMACCR_BASE)     /* Checksum Control Register */
#define EPOCR					(0x04C+SWMACCR_BASE)	/* Embedded PHY Operation Control Register */
#define EPIDR					(0x050+SWMACCR_BASE)	/* Embedded PHY ID Register */

/* MACCR - MAC control register field definitions */
#define NORMAL_BACKOFF                      (1 << 28)                /* Normal back off slot timer */
#define BACKOFF_EXPONENTIAL_3               (1 << 27)                /* Set back off exponential parameter 9 */
#define DIS_BACKOFF_BIST                    (1 << 26)                /* Disable back off BIST */
#define IPG_SEL                     	    (1 << 25)                /* Fixed IPG */
#define INFINITE_PAUSE_FRAMES               (1 << 24)                /* Infinite pause frames */
#define LONG_TXE       	                    (1 << 22)                /* Back pressure, carrier based */
#define DIS_MASK_CGST                       (1 << 21)
#define EN_48_DROP                          (1 << 20)

/* MDCIOCR - MDC/MDIO Command */
#define COMMAND_OFFSET				31			/* MDCMDIO Command */
#define COMMAND_MASK                        (1<<31)                  /* 0:Read Access, 1:Write Access */
#define COMMAND_READ                        (0<<31)                  /* 0:Read Access, 1:Write Access */
#define COMMAND_WRITE                       (1<<31)                  /* 0:Read Access, 1:Write Access */
#define PHYADD_OFFSET                       (24)                     /* PHY Address, said, PHY ID */
#define PHYADD_MASK                         (0x1f<<24)               /* PHY Address, said, PHY ID */
#define REGADD_OFFSET                       (16)                     /* PHY Register */
#define REGADD_MASK                         (0x1f<<16)               /* PHY Register */
#define WRDATA_OFFSET                       (0)                      /* Data to PHY register */
#define WRDATA_MASK                         (0xffff<<0)              /* Data to PHY register */

/* MDCIOSR - MDC/MDIO Status */
#define STATUS                              (1<<31)                  /* 0: Process Done, 1: In progress */
#define RDATA_OFFSET                        (0)                      /* Read Data Result of PHY register */
#define RDATA_MASK                          (0xffff<<0)              /* Read Data Result of PHY register */

/* PMCR - Port mirror control register field definitions */
#define MirrorPortMsk_OFFSET                18          /* Port receiving the mirrored traffic (single bit set to 1 is allowed) */
#define MirrorPortMsk_MASK                  (0x1ff<<18) /* Port receiving the mirrored traffic (single bit set to 1 is allowed) */
#define MirrorRxPrtMsk_OFFSET               9           /* Rx port to be mirrored (Source Mirroring, multiple ports allowed) */
#define MirrorRxPrtMsk_MASK                 (0x1ff<<9)  /* Rx port to be mirrored (Source Mirroring, multiple ports allowed) */
#define MirrorTxPrtMsk_OFFSET               0           /* Tx port to be mirrored (Destination Mirroring, multiple ports allowed) */
#define MirrorTxPrtMsk_MASK                 (0x1ff<<0)  /* Tx port to be mirrored (Destination Mirroring, multiple ports allowed) */

/* PPMAR - Per port matching action register field definitions */
#define EnPatternMatch_OFFSET               26          /* Enable pattern match port list */
#define EnPatternMatch_MASK                 (0x3f<<26)  /* Enable pattern match port list */
#define EnPatternMatch_P0                   (1<<26)     /* Enable pattern match for port 0 */
#define EnPatternMatch_P1                   (1<<27)     /* Enable pattern match for port 1 */
#define EnPatternMatch_P2                   (1<<28)     /* Enable pattern match for port 2 */
#define EnPatternMatch_P3                   (1<<29)     /* Enable pattern match for port 3 */
#define EnPatternMatch_P4                   (1<<30)     /* Enable pattern match for port 4 */
#define EnPatternMatch_P5                   (1<<31)     /* Enable pattern match for port 5 */
#define MatchOpRx5_OFFSET                   24          /* operation if matched on port 5 */
#define MatchOpRx5_MASK                     (3<<24)     /* operation if matched on port 5 */
#define MatchOpRx5_DROP                     (0<<24)     /* Drop if matched on port 5 */
#define MatchOpRx5_MTCPU                    (1<<24)     /* Mirror to CPU if matched on port 5 */
#define MatchOpRx5_FTCPU                    (2<<24)     /* Forward to CPU if matched on port 5 */
#define MatchOpRx5_MTMP                     (3<<24)     /* Mirror to Mirror Port if matched on port 5 */
#define MatchOpRx4_OFFSET                   22          /* operation if matched on port 4 */
#define MatchOpRx4_MASK                     (3<<22)     /* operation if matched on port 4 */
#define MatchOpRx4_DROP                     (0<<22)     /* Drop if matched on port 4 */
#define MatchOpRx4_MTCPU                    (1<<22)     /* Mirror to CPU if matched on port 4 */
#define MatchOpRx4_FTCPU                    (2<<22)     /* Forward to CPU if matched on port 4 */
#define MatchOpRx4_MTMP                     (3<<22)     /* Mirror to Mirror Port if matched on port 4 */
#define MatchOpRx3_OFFSET                   20          /* operation if matched on port 3 */
#define MatchOpRx3_MASK                     (3<<20)     /* operation if matched on port 3 */
#define MatchOpRx3_DROP                     (0<<20)     /* Drop if matched on port 3 */
#define MatchOpRx3_MTCPU                    (1<<20)     /* Mirror to CPU if matched on port 3 */
#define MatchOpRx3_FTCPU                    (2<<20)     /* Forward to CPU if matched on port 3 */
#define MatchOpRx3_MTMP                     (3<<20)     /* Mirror to Mirror Port if matched on port 3 */
#define MatchOpRx2_OFFSET                   18          /* operation if matched on port 2*/
#define MatchOpRx2_MASK                     (3<<18)     /* operation if matched on port 2 */
#define MatchOpRx2_DROP                     (0<<18)     /* Drop if matched on port 2 */
#define MatchOpRx2_MTCPU                    (1<<18)     /* Mirror to CPU if matched on port 25 */
#define MatchOpRx2_FTCPU                    (2<<18)     /* Forward to CPU if matched on port 2 */
#define MatchOpRx2_MTMP                     (3<<18)     /* Mirror to Mirror Port if matched on port 2 */
#define MatchOpRx1_OFFSET                   16          /* operation if matched on port 1 */
#define MatchOpRx1_MASK                     (3<<16)     /* operation if matched on port 1 */
#define MatchOpRx1_DROP                     (0<<16)     /* Drop if matched on port 1 */
#define MatchOpRx1_MTCPU                    (1<<16)     /* Mirror to CPU if matched on port 1 */
#define MatchOpRx1_FTCPU                    (2<<16)     /* Forward to CPU if matched on port 1 */
#define MatchOpRx1_MTMP                     (3<<16)     /* Mirror to Mirror Port if matched on port 1 */
#define MatchOpRx0_OFFSET                   14          /* operation if matched on port 0 */
#define MatchOpRx0_MASK                     (3<<14)     /* operation if matched on port 0 */
#define MatchOpRx0_DROP                     (0<<14)     /* Drop if matched on port 0 */
#define MatchOpRx0_MTCPU                    (1<<14)     /* Mirror to CPU if matched on port 0 */
#define MatchOpRx0_FTCPU                    (2<<14)     /* Forward to CPU if matched on port 0 */
#define MatchOpRx0_MTMP                     (3<<14)     /* Mirror to Mirror Port if matched on port 0 */

/* CSCR, CCR - Checksum Control Register */
#define EnL4ChkCal                          (1<<5)                   /* Enable L4 Checksum Re-calculation */
#define EnL3ChkCal                          (1<<4)                   /* Enable L3 Checksum Re-calculation */
#define L4ChkSErrAllow                      (1<<2)                   /* L4 Checksum Error Allow */
#define L3ChkSErrAllow                      (1<<1)                   /* L3 Checksum Error Allow */
#define L2CRCErrAllow                       (1<<0)                   /* L2 CRC Error Allow */

/* EPOCR, EPIDR - Embedded PHY Operation Control Register */
#define Port_embPhyID_MASK(port)			( 0x1f << ( port * 5 ) )			/* Embedded PHY ID MASK of port 'idx' */
#define Port_embPhyID(id, port)				( id << ( port * 5 ) )			/* Embedded PHY ID setting of port 'idx' */

#define PCRAM_BASE                          (SWCORE_BASE+0x4100)
#define PITCR                               (0x000 + PCRAM_BASE)     /* Port Interface Type Control Register */
#define PCRP0                               (0x004 + PCRAM_BASE)     /* Port Configuration Register of Port 0 */
#define PCRP1                               (0x008 + PCRAM_BASE)     /* Port Configuration Register of Port 1 */
#define PCRP2                               (0x00C + PCRAM_BASE)     /* Port Configuration Register of Port 2 */
#define PCRP3                               (0x010 + PCRAM_BASE)     /* Port Configuration Register of Port 3 */
#define PCRP4                               (0x014 + PCRAM_BASE)     /* Port Configuration Register of Port 4 */
#define PCRP5                               (0x018 + PCRAM_BASE)     /* Port Configuration Register of Port 5 */
#define PCRP6                               (0x01C + PCRAM_BASE)     /* Port Configuration Register of Ext Port 0 */
#define PCRP7                               (0x020 + PCRAM_BASE)     /* Port Configuration Register of Ext Port 1 */
#define PCRP8                               (0x024 + PCRAM_BASE)     /* Port Configuration Register of Ext Port 2 */
#define PSRP0                               (0x028 + PCRAM_BASE)     /* Port Status Register Port 0 */
#define PSRP1                               (0x02C + PCRAM_BASE)     /* Port Status Register Port 1 */
#define PSRP2                               (0x030 + PCRAM_BASE)     /* Port Status Register Port 2 */
#define PSRP3                               (0x034 + PCRAM_BASE)     /* Port Status Register Port 3 */
#define PSRP4                               (0x038 + PCRAM_BASE)     /* Port Status Register Port 4 */
#define PSRP5                               (0x03C + PCRAM_BASE)     /* Port Status Register Port 5 */
#define PSRP6                               (0x040 + PCRAM_BASE)     /* Port Status Register Port 6 */
#define PSRP7                               (0x044 + PCRAM_BASE)     /* Port Status Register Port 7 */
#define PSRP8                               (0x048 + PCRAM_BASE)     /* Port Status Register Port 8 */
#define P0GMIICR                            (0x04C + PCRAM_BASE)     /* Port-0 GMII Configuration Register */
#define P5GMIICR                            (0x050 + PCRAM_BASE)     /* Port-5 GMII Configuration Register */

/* PITCR - Port Interface Type Control Register  */
#define Port5_TypeCfg_GMII_MII_RGMII        (0<<10)
#define Port4_TypeCfg_UTP                   (0<< 8)
#define Port4_TypeCfg_SerDes                (1<< 8)
#define Port3_TypeCfg_UTP                   (0<< 6)
#define Port3_TypeCfg_SerDes                (1<< 6)
#define Port2_TypeCfg_UTP                   (0<< 4)
#define Port2_TypeCfg_SerDes                (1<< 4)
#define Port1_TypeCfg_UTP                   (0<< 2)
#define Port1_TypeCfg_SerDes                (1<< 2)
#define Port0_TypeCfg_UTP                   (0<< 0)
#define Port0_TypeCfg_GMII_MII_RGMII        (1<< 0)

/* PCRP0,PCRP1,PCRP2,PCRP3,PCRP4,PCRP5,PCRP6,PCRP7,PCRP8 - Port Configuration Register */
#define BYPASS_TCRC                         (1 << 29)   /* Not recalculate CRC error */
#define ExtPHYID_OFFSET                     (24)        /* External PHY ID */
#define ExtPHYID_MASK                       (0x1f<<24)  /* External PHY ID */
#define EnForceMode                         (1<<23)     /* Enable Force Mode to set link/speed/duplix/flow status */
#define ForceLink                           (1<<22)     /* 0-link down, 1-link up */
#define ForceSpeedMask                      (3<<20)     /* Force speed Mask */
#define ForceSpeed10M                       (0<<20)     /* Force speed 10M */
#define ForceSpeed100M                      (1<<20)     /* Force speed 100M */
#define ForceSpeed1000M                     (2<<20)     /* Force speed 1G */
#define ForceDuplex                         (1<<19)     /* Force Duplex */
#define MIIcfg_CRS                          (1<<18)     /* MII interface Parameter setup */
#define MIIcfg_COL                          (1<<17)     /* MII interface Parameter setup */
#define MIIcfg_RXER                         (1<<16)     /* MII interface Parameter setup */
#define GMIIcfg_CRS                         (1<<15)     /* GMII interface Parameter setup */
#define BCSC_Types_MASK                     (7<<12)     /* Broadcast Storm Control packet type selection */
#define BCSC_ENUNKNOWNUNICAST               (1<<14)     /* Enable control for unknown unicast packet */
#define BCSC_ENMULTICAST                    (1<<13)     /* Enable control for multicast packet */
#define BCSC_ENBROADCAST                    (1<<12)     /* Enable control for broadcast packet */
#define ENBCSC								(1<<11)		/* Enable broadcast storm control */
#define EnLoopBack                          (1<<10)     /* Enable MAC-PHY interface Mii Loopback */
#define PauseFlowControl_MASK               (3<<8)      /* Mask for per-port 802.3 PAUSE flow control ability control */
#define PauseFlowControlNway                (0<<8)      /* Follow the PHY N-way result */
#define PauseFlowControlEtxDrx              (1<<8)      /* force: enable TX, disable RX */
#define PauseFlowControlDtxErx              (2<<8)      /* force: disable TX, enable RX */
#define PauseFlowControlEtxErx              (3<<8)      /* force: enable TX, enable RX */
#define DisBKP                              (1<<7)      /* Per-port Disable Backpressure function for Half duplex */
#define STP_PortST_MASK					(3<<5)		/* Mask Spanning Tree Protocol Port State Control */
#define STP_PortST_OFFSET					(5)			/* Offset */
#define STP_PortST_DISABLE				(0<<5)		/* Disable State */
#define STP_PortST_BLOCKING				(1<<5)		/* Blocking State */
#define STP_PortST_LISTENING				(1<<5)		/* Listening State */
#define STP_PortST_LEARNING				(2<<5)		/* Learning State */
#define STP_PortST_FORWARDING			(3<<5)		/* Forwarding State */
#define IPMSTP_PortST_MASK				(3<<3)		/* Mask of IP Multicast Spanning Tree Protocol Port State Control */
#define IPMSTP_PortST_OFFSET				(3)			/* Offset of IP Multicast Spanning Tree Protocol Port State Control */
#define IPMSTP_PortST_DISABLE			(0<<3)		/* Disable State of IP Multicast Spanning Tree Protocol Port State Control */
#define IPMSTP_PortST_BLOCKING			(1<<3)		/* Blocking State of IP Multicast Spanning Tree Protocol Port State Control */
#define IPMSTP_PortST_LISTENING		(1<<3)		/* Listening State of IP Multicast Spanning Tree Protocol Port State Control */
#define IPMSTP_PortST_LEARNING			(2<<3)		/* Learning State of IP Multicast Spanning Tree Protocol Port State Control */
#define IPMSTP_PortST_FORWARDING		(3<<3)		/* Forwarding State of IP Multicast Spanning Tree Protocol Port State Control */
#define AcptMaxLen_OFFSET                   (1)         /* Enable the max acceptable packet length supported */
#define AcptMaxLen_MASK                     (3<<1)      /* Enable the max acceptable packet length supported */
#define AcptMaxLen_1536                     (0<<1)      /* 1536 Bytes (RTL865xB) */
#define AcptMaxLen_1552                     (1<<1)      /* 1552 Bytes (RTL865xB) */
#define AcptMaxLen_9K                       (2<<1)      /* 9K(9216) Bytes */
#define AcptMaxLen_16K                      (3<<1)      /* 16370 bytes = 16K-2(cutoff)-4(vlan)-8(pppoe) */
#define EnablePHYIf                         (1<<0)      /* Enable PHY interface. */

/* PSRP0,PSRP1,PSRP2,PSRP3,PSRP4,PSRP5,PSRP6,PSRP7,PSRP8 - Port Status Register Port 0~8 */
#define LinkDownEventFlag                   (1<<8)      /* Port Link Down Event detecting monitor flag */
#define PortStatusNWayEnable                (1<<7)      /* N-Way Enable */
#define PortStatusRXPAUSE                   (1<<6)      /* Rx PAUSE */
#define PortStatusTXPAUSE                   (1<<5)      /* Tx PAUSE */
#define PortStatusLinkUp                    (1<<4)      /* Link Up */
#define PortStatusDuplex                    (1<<3)      /* Duplex */
#define PortStatusLinkSpeed_MASK            (3<<0)      /* Link Speed */
#define PortStatusLinkSpeed_OFFSET	0
#define PortStatusLinkSpeed10M			(0<<0)	/* 10M */
#define PortStatusLinkSpeed100M			(1<<0)	/* 100M */
#define PortStatusLinkSpeed1000M		(2<<0)	/* 1000M */
#define PortStatusLinkSpeedReserved		(3<<0)	/* Reserved Speed */

/* P0GMIICR Port-0 / Port-5 GMII Configuration Register */
#define CFG_GMAC_MASK                       (3<<23)                  /* The register default reflect the HW power on strapping value of H/W pin. */
#define CFG_GMAC_RGMII                      (0<<23)                  /* RGMII mode */
#define CFG_GMAC_GMII_MII_MAC               (1<<23)                  /* GMII/MII MAC mode */
#define CFG_GMAC_GMII_MII_PHY               (2<<23)                  /* GMII/MII PHY mode */
#define CFG_GMAC_Reserved                   (3<<23)                  /* Reserved */

#define RGMII_RCOMP_MASK					(3<<0)					/* RGMII Input Timing compensation control */
#define RGMII_RCOMP_0NS						(0<<0)					/* Rcomp 0.0 ns */
#define RGMII_RCOMP_1DOT5NS					(1<<0)					/* Rcomp 1.5 ns */
#define RGMII_RCOMP_2NS						(2<<0)					/* Rcomp 2.0 ns */
#define RGMII_RCOMP_2DOT5NS					(3<<0)					/* Rcomp 3.0 ns */
#define RGMII_TCOMP_MASK					(7<<2)					/* RGMII Output Timing compensation control */
#define RGMII_TCOMP_0NS						(0<<2)					/* Tcomp 0.0 ns */
#define RGMII_TCOMP_1DOT5NS					(1<<2)					/* Tcomp 1.5 ns */
#define RGMII_TCOMP_2NS						(2<<2)					/* Tcomp 2.0 ns */
#define RGMII_TCOMP_2DOT5NS					(3<<2)					/* Tcomp 2.5 ns */
#define RGMII_TCOMP_3NS						(4<<2)					/* Tcomp 3.0 ns */
#define RGMII_TCOMP_4NS						(5<<2)					/* Tcomp 4.0 ns */
#define RGMII_TCOMP_6NS						(6<<2)					/* Tcomp 6.0 ns */
#define RGMII_TCOMP_7NS						(7<<2)					/* Tcomp 7.0 ns */

#define SWMISC_BASE                         (0x4200+SWCORE_BASE)
/* Chip Version ID Register */
#define CVIDR                               (0x00+SWMISC_BASE)     /* Chip Version ID Register */
#define SSIR						        (0x04+SWMISC_BASE)     /* System Initial and Reset Registe*/
#define BISTCR                              (0x08+SWMISC_BASE)     /* BIST control */
#define BISTTSDR0                           (0x0C+SWMISC_BASE)     /* BIST Test Status Diagnostic Register 0 */
#define BISTTSDR1                           (0x10+SWMISC_BASE)     /* BIST Test Status Diagnostic Register 1 */
#define BISTTSDR2                           (0x14+SWMISC_BASE)     /* BIST Test Status Diagnostic Register 2 */
#define BISTTSDR3                           (0x18+SWMISC_BASE)     /* BIST Test Status Diagnostic Register 3 */
#define BISTTSDR4                           (0x1C+SWMISC_BASE)     /* BIST Test Status Diagnostic Register 4 */
#define BISTTSDR5                           (0x20+SWMISC_BASE)     /* BIST Test Status Diagnostic Register 5 */
#define BISTTSDR6                           (0x24+SWMISC_BASE)     /* BIST Test Status Diagnostic Register 6 */
#define BISTTSDR7                           (0x28+SWMISC_BASE)     /* BIST Test Status Diagnostic Register 7 */
#define BISTTSDR8                           (0x2C+SWMISC_BASE)     /* BIST Test Status Diagnostic Register 8 */
#define BISTTSDR9                           (0x30+SWMISC_BASE)     /* BIST Test Status Diagnostic Register 9 */
#define BISTTSDR10                          (0x34+SWMISC_BASE)     /* BIST Test Status Diagnostic Register 10 */
#define BISTTSDR11                          (0x38+SWMISC_BASE)     /* BIST Test Status Diagnostic Register 11 */
#define CRMR                                (0x3C+SWMISC_BASE)     /* Chip Revision Management Register */
#define SIRR						        (SSIR)                 /* Alias Name */

/* 	SIRR, SSIR - System Initial and Reset Register*/
#define SwitchFullRst                       (1 << 2)   /* Reset all tables & queues */
#define SwitchSemiRst                       (1 << 1)   /* Reset queues */
#define TRXRDY                              (1 << 0)    /* Start normal TX and RX */
#define FULL_RST                            SwitchFullRst /* Alias Name */
#define SEMI_RST                            SwitchSemiRst /* Alias Name */

/* BISTCR - BIST control register field definitions */
#define DisBIST                             (1<<31)     /* (RO)1 if Disable all SRAM BIST. By default, all SRAM BIST are invoked after power-on reset completes. */
#define dis_skip_fp                         (1<<30)     /* Disable skipping fail pages. Only column repairs of the packet buffer take effect. */
#define diag_mode                           (1<<29)     /* Enter the diagnosis mode for the SRAM of packet buffer */
#define diag_done                           (1<<28)     /* The diagnosis mode for the packet buffer allows the user to perform diagnosis of the memory defects. */
#define lat_fail_info                       (1<<27)     /* Latch failure information for a specific testing condition. */
#define bisting_MASK                        (0x000007ff)/* bisting[10]=NICTXRAM BIST ongoingbisting
                                                                   [9]=NICRXRAM BIST ongoingbisting
                                                                   [8]=V4KRAM BIST ongoingbisting
                                                                   [7]=PBRAM BIST ongoingbisting
                                                                   [6]=OQRAM BIST ongoingbisting
                                                                   [5]=MIBRAM BIST ongoingbisting
                                                                   [4]=L2/L4RAM BIST ongoingbisting
                                                                   [3]=IQRAM BIST ongoingbisting
                                                                   [2]=HTRAM BIST ongoingbisting
                                                                   [1]=HSARAM BIST ongoingbisting
                                                                   [0]=ACL/AMI/RLMRAM BIST ongoing */
#define bisting_OFFSET                      0
#define BIST_READY_MASK                     bisting_MASK /* Alias Name */
#define BIST_READY_PATTERN                  0x00000000

/* BISTTSDR0 - IST Test Status Diagnostic Register 0 */
#define BISTTSDR0_READY_MASK                0x03ffffff
#define BISTTSDR0_READY_PATTERN             0x00000000

/* Chip version ID register field definitions */
#define RTL8650_CVID                        0x86500000


/**********************************************************************/
#define ALE_BASE				(0x4400+SWCORE_BASE)
#define TEACR					(0x00+ALE_BASE)       /* Table Entry Aging Control Register */
#define TEATCR					(0x04+ALE_BASE)       /* Table entry aging time control */
#define RMACR					(0x08+ALE_BASE)       /* Reserved Multicast Address Address Mapping */
#define ALECR					(0x0C+ALE_BASE)       /* ALE Control Register */
#define MSCR						(0x10+ALE_BASE)       /* Module Switch Control Register */
#define TTLCR					(0x0C+ALE_BASE)       /* TTL control */
#define L4TOCR					(0x14+ALE_BASE)       /* L4 Table Offset control */
#define SWTCR0					(0x18+ALE_BASE)       /* swtich table control register 0 */
#define SWTCR1					(0x1C+ALE_BASE)       /* swtich table control register 1 */
#define PLITIMR					(0x20+ALE_BASE)       /* Port to LAN Interface Table Index Mapping Register */
#define DACLRCR					(0x24+ALE_BASE)       /* default ACL rule control register */
#define FFCR						(0x28+ALE_BASE)       /*Frame Forwarding Configuratoin Register */
#define MGFCR_E0R0				(0x2C+ALE_BASE)       /*L2 MAC Group Forwarding Control Register Entry_0 R0 */
#define MGFCR_E0R1				(0x30+ALE_BASE)       /*L2 MAC Group Forwarding Control Register Entry_1 R1 */
#define MGFCR_E0R2				(0x34+ALE_BASE)       /*L2 MAC Group Forwarding Control Register Entry_2 R2 */
#define MGFCR_E1R0				(0x38+ALE_BASE)       /*L2 MAC Group Forwarding Control Register Entry_0 R0 */
#define MGFCR_E1R1				(0x3C+ALE_BASE)       /*L2 MAC Group Forwarding Control Register Entry_1 R1 */
#define MGFCR_E1R2				(0x40+ALE_BASE)       /*L2 MAC Group Forwarding Control Register Entry_2 R2 */
#define MGFCR_E2R0				(0x44+ALE_BASE)       /*L2 MAC Group Forwarding Control Register Entry_0 R0 */
#define MGFCR_E2R1				(0x48+ALE_BASE)       /*L2 MAC Group Forwarding Control Register Entry_1 R1 */
#define MGFCR_E2R2				(0x4C+ALE_BASE)       /*L2 MAC Group Forwarding Control Register Entry_2 R2 */
#define MGFCR_E3R0				(0x50+ALE_BASE)       /*L2 MAC Group Forwarding Control Register Entry_0 R0 */
#define MGFCR_E3R1				(0x54+ALE_BASE)       /*L2 MAC Group Forwarding Control Register Entry_1 R1 */
#define MGFCR_E3R2				(0x58+ALE_BASE)       /*L2 MAC Group Forwarding Control Register Entry_2 R2 */
#define OCR						L4TOCR				/* Alias Name */


/* TEACR - Table Entry Aging Control Register */
#define EnRateLimitTbAging                  (1<<6)                /* Enable Rate Limit table hardware aging function. */
#define EnL2FastAging                       (1<<5)                /* Enable L2 Fast Aging Out */
#define EnL2HashColOW                       (1<<4)                /* Enable L2 Tablsh Hash Collision Over Write */
#define IPMcastAgingDisable                 (1<<3)                /* 0=Enable IP Multicast table aging. 1=disable */
#define PPPoEAgingDisable                   (1<<2)                /* 0=Enable PPPoE Table Aging. 1=disable */
#define L4AgingDisable                      (1<<1)                /* 0=Enable L4 Aging. 1=disable */
#define L2AgingDisable                      (1<<0)                /* 0=Enable L2 Aging. 1=disable */

/* TEATCR - Table entry aging time control */
#define ICMPT_OFFSET                        24                    /* ICMP Timeout */
#define ICMPT_MASK                          (0x3f<<24)            /* ICMP TImeout */
#define UDPT_OFFSET                         18                    /* UDP Timeout */
#define UDPT_MASK                           (0x3f<<18)            /* UDP Timeout */
#define TCPLT_OFFSET                        12                    /* TCP Long Timeout */
#define TCPLT_MASK                          (0x3f<<12)            /* TCP Long Timeout */
#define TCPMT_OFFSET                        6                     /* TCP Medium Timeout */
#define TCPMT_MASK                          (0x3f<<6)             /* TCP Medium Timeout */
#define TCPST_OFFSET                        0                     /* TCP Short Timeout */
#define TCPST_MASK                          (0x3f<<0)             /* TCP Short Timeout */

/* RMACR - Reserved Multicast Address Address Mapping (01-80-c2-00-00-xx) */
#define MADDR20_2F                          (1<< 8)               /* GARP Reserved Address */
#define MADDR00_10                          (1<< 7)               /* 802.1d Reservedd Address */
#define MADDR21                             (1<< 6)               /* GVRP Address */
#define MADDR20                             (1<< 5)               /* GMRP Address */
#define MADDR10                             (1<< 4)               /* All LANs Bridge Management Group Address */
#define MADDR0E                             (1<< 3)               /* IEEE Std. 802.1AB Link Layer Discovery protocol multicast address */
#define MADDR03                             (1<< 2)               /* IEEE Std 802.1X PAE address */
#define MADDR02                             (1<< 1)               /* IEEE Std 802.3ad Slow Protocols-Multicast address */
#define MADDR00                             (1<< 0)               /* BPDU (Bridge Group Address) */

/* ALECR */
#define EN_PPPOE					(1<<18)			/* Enable PPPoE auto-encapsulation and auto-decapsulation */
#define TTL_1Enable					(1<<16)			/* Enable TTL-1 operation for L3 routing */
#define Fragment2CPU				(1<<15)			/*	When ACL is enabled, enable all fragmented IP packet to be trapped to CPU (because L4 is needed)
														When ACL is disabled, if this bit is set, L2 forwarding as before, L3 above operation will trapped to CPU.
														When ACL is disabled, if this bit is not set, IP fragment packet will be forwarded as normal via L3 routing or NAT. */
#define FRAG2CPU					Fragment2CPU
#define MultiCastMTU_OFFSET			(0)
#define MultiCastMTU_MASK			(0x3fff)
#define MULTICAST_L2_MTU_MASK		MultiCastMTU_MASK	/* Alias Name */
#define EN_TTL1						TTL_1Enable			/* Alias Name */


/* MSCR - Module Switch Control Register */
#define DisChk_CFI                          (1<< 9)               /* Disable Check CFI bit for L2 random packet testing purpose. */
#define EnRRCP2CPU                          (1<< 7)               /* Enable trap RRCP packet to CPU port for L2 testing purpose. RRCP: Realtek Remote Control Protocol (Proprietary) */
#define NATTM                               (1<< 6)               /* 0: Normal mode processing, 1: NAT Test Mode */
#define Enable_ST                           (1<< 5)               /* Enable Spanning Tree Protocol. 0: disable, 1: enable */
#define Ingress_ACL                         (1<< 4)               /* Enable Ingress ACL. 0: disable, 1: enable */
#define Egress_ACL                          (1<< 3)               /* Enable Egress ACL. 0: disable, 1: enable */
#define Mode_OFFSET                         0                     /* Switch operation layer function mode */
#define Mode_MASK                           (7<< 0)               /* Switch operation layer function mode */
#define Mode_enL2                           (1<< 0)               /* Enable L2 */
#define Mode_enL3                           (1<< 1)               /* Enable L3 */
#define Mode_enL4                           (1<< 2)               /* Enable L4 */
#define EN_STP                              Enable_ST             /* Alias Name */
#define EN_IN_ACL                           Ingress_ACL           /* Alias Name */
#define EN_OUT_ACL                          Egress_ACL            /* Alias Name */
#define EN_L4                               Mode_enL4             /* Alias Name */
#define EN_L3                               Mode_enL3             /* Alias Name */
#define EN_L2                               Mode_enL2             /* Alias Name */

/* SWTCR0 - swtich table control register */
#define STOP_TLU_STA                        (1<<19)               /* (RO) Table Lookup Stop Status. 1-STOP_TLU command execute is ready */
#define STOP_TLU                            (1<<18)               /* Stop Table Lookup Process 1-to stop */
#define LIMDBC_OFFSET                       (16)                  /* LAN Interface Multilayer-Decision-Base Control */
#define LIMDBC_MASK                         (3<<16)
#define LIMDBC_VLAN                         (0<<16)               /* By VLAN base */
#define LIMDBC_PORT                         (1<<16)               /* By Port base */
#define LIMDBC_MAC                          (2<<16)               /* By MAC base */
#define EnUkVIDtoCPU                        (1 << 15)             /* Enable trap unknown tagged VID (VLAN table lookup miss) packet to CPU */ 
#define NAPTF2CPU                           (1 << 14)             /*	Trap packets not in TCP/UDP/ICMP format and 
													destined to the interface required to do NAPT */
#define MultiPortModeP_OFFSET				(5)						/* Multicast Port Mode : Internal (0) or External (1) */
#define MultiPortModeP_MASK					(0x1ff)					/* {Ext3~Ext1,Port0~Port5} 0:Internal, 1:External */
#define MCAST_PORT_EXT_MODE_OFFSET		MultiPortModeP_OFFSET		/* Alias Name */
#define MCAST_PORT_EXT_MODE_MASK			MultiPortModeP_MASK		/* Alias Name */
#define WANRouteMode_OFFSET                 (3)
#define WANRouteMode_MASK                   (3<<3)
#define WANRouteMode_Forward                (0<<3)
#define WANRouteMode_ToCpu                  (1<<3)
#define WANRouteMode_Drop                   (2<<3)
#define WAN_ROUTE_MASK                      WANRouteMode_MASK
#define WAN_ROUTE_FORWARD                   WANRouteMode_Forward  /* Route WAN packets */
#define WAN_ROUTE_TO_CPU                    WANRouteMode_ToCpu    /* Forward WAN packets to CPU */
#define WAN_ROUTE_DROP                      WANRouteMode_Drop     /* Drop WAN packets */
#define EnNAPTAutoDelete                    (1<<2)
#define EnNAPTAutoLearn                     (1<<1)
#define EnNAPTRNotFoundDrop                 (1<<0)                /* 0: Reverse NAPT entry not found to forward to CPU, 1: Reverse NAPT entry not found to drop */
#define EN_NAPT_AUTO_DELETE                 EnNAPTAutoDelete      /* Enable NAPT auto delete */
#define EN_NAPT_AUTO_LEARN                  EnNAPTAutoLearn       /* Enable NAPT auto learn */
#define NAPTR_NOT_FOUND_DROP                EnNAPTRNotFoundDrop   /* Reverse NAPT not found to S_DROP */

/* SWTCR1 - swtich table control register */
#define L4EnHash1                           (1 << 13)   /* Enhanced Hash1 */
#define EnNAP8651B                          (1 << 12)   /* Enable 51B mode */
#define EN_RTL8650B                         EnNAP8651B  /* Alias Name */
#define ENFRAGTOACLPT                       (1 << 11)   /* Enable fragment packet checked by ACL and protocol trapper */
#define EN_FRAG_TO_ACLPT                    ENFRAGTOACLPT/* Alias Name */
#define EnNATT2LOG                          (1 << 10)   /* Enable trapping attack packets for logging */
#define EN_ATTACK_TO_LOG                    EnNATT2LOG  /* Alias Name */
#define EnL4WayH                            (1 << 9)    /* Enable 4-way hash on L4 TCP/UDP table */
#define EN_TCPUDP_4WAY_HASH                 EnL4WayH    /* Alias Name */
#define SelCpuReason                        (1 << 8)    /* Enable 51B CPU reason coding */
#define EN_51B_CPU_REASON                   SelCpuReason/* Alias Name */
#define EN_SPI6_WAN_SRVPRT                  (1 << 7)    /* Enable SPI-6 between WAN and server port */
#define EN_SPI5_WAN_NI                      (1 << 6)    /* Enable SPI-5 between WAN and NI */
#define EN_SPI4_WAN_DMZ                     (1 << 5)    /* Enable SPI-4 between WAN and DMZ */
#define EN_SPI3_DMZ_RLAN                    (1 << 4)    /* Enable SPI-3 between DMZ and RLAN */
#define EN_SPI2_DMZ_LAN                     (1 << 3)    /* Enable SPI-2 between DMZ and LAN */
#define EN_SPI1_WAN_RLAN                    (1 << 2)    /* Enable SPI-1 between WAN and RLAN */
#define EnSPIRDrp                           (1 << 1)    /* Enable dropping packets not found by reverse SPI */
#define EN_DROP_SPIR_NOT_FOUND              EnSPIRDrp   /* Alias Name */
#define TrapSPIUnknown                      (1 << 0)    /* Enable SPI trapping non-TCP/UDP/ICMP packets */
#define EN_SPI_TRAP_UNKNOWN                 TrapSPIUnknown/* Alias Name */
#if 0 /* no longer supported */
#define EN_FRAG_PPPOE                       (1 << 20)   /* Enable auto-fragment for PPPoE */
#define EN_MCAST_TBL_AGING  	   	   	   	(1 << 19)   /* Enable aging of IP multicast table */
#define EN_PPPOE_TBL_AGING  	   	   	   	(1 << 18)   /* Enable aging of PPPoE table */
#endif

/* PLITIMR - Port to LAN Interface Table Index Mapping Register */
#define INTP0_OFFSET                        0           /* Index for P0, pointing to Interface table */
#define INTP0_MASK                          (7<<0)      /* Index for P0, pointing to Interface table */
#define INTP1_OFFSET                        3           /* Index for P1, pointing to Interface table */
#define INTP1_MASK                          (7<<3)      /* Index for P1, pointing to Interface table */
#define INTP2_OFFSET                        6           /* Index for P2, pointing to Interface table */
#define INTP2_MASK                          (7<<6)      /* Index for P2, pointing to Interface table */
#define INTP3_OFFSET                        9           /* Index for P3, pointing to Interface table */
#define INTP3_MASK                          (7<<9)      /* Index for P3, pointing to Interface table */
#define INTP4_OFFSET                        12          /* Index for P4, pointing to Interface table */
#define INTP4_MASK                          (7<<12)     /* Index for P4, pointing to Interface table */
#define INTP5_OFFSET                        15          /* Index for P5, pointing to Interface table */
#define INTP5_MASK                          (7<<15)     /* Index for P5, pointing to Interface table */
#define INTExtP0_OFFSET                     18          /* Index for Ext0, pointing to Interface table */
#define INTExtP0_MASK                       (7<<18)     /* Index for Ext0, pointing to Interface table */
#define INTExtP1_OFFSET                     21          /* Index for Ext1, pointing to Interface table */
#define INTExtP1_MASK                       (7<<21)     /* Index for Ext1, pointing to Interface table */
#define INTExtP2_OFFSET                     24          /* Index for Ext2, pointing to Interface table */
#define INTExtP2_MASK                       (7<<24)     /* Index for Ext2, pointing to Interface table */

/* DACLRCR - Default ACL Rule Control Register */
#define ACLI_STA_MASK				(0x7f)
#define ACLI_EDA_OFFSET			7
#define ACLI_EDA_MASK				(0x7f<<ACLI_EDA_OFFSET)
#define ACLO_STA_OFFSET			14
#define ACLO_STA_MASK				(0x7f<<ACLO_STA_OFFSET)
#define ACLO_EDA_OFFSET			21
#define ACLO_EDA_MASK				(0x7f<<ACLO_EDA_OFFSET)

/* FFCR - Frame Forwarding Configuration Register */
#define IPMltCstCtrl_OFFSET                 (3)         /* IP Multicast Forwarding Control */
#define IPMltCstCtrl_MASK                   (3<<3)      /* IP Multicast Forwarding Control */
#define IPMltCstCtrl_Disable                (0<<3)      /* Disable IP Multicast table lookup (just follow L2 Multicast packet procedure) */
#define IPMltCstCtrl_Enable                 (1<<3)      /* Enable IP Multicast table lookup */
#define IPMltCstCtrl_TrapToCpu              (2<<3)      /* Tral all IP Multicast packet to CPU port */
#define EN_MCAST                            IPMltCstCtrl_Enable    /* Alias Name for Enable Multicast Table */
#define EnFlood2NonCgtPrt                   (1<<2)      /* Enable Flooding to non-Congested Port Only */
#define EnUnkUC2CPU                         (1<<1)      /* Enable Unknown Unicast Packet Trap to CPU port */
#define EnUnkMC2CPU                         (1<<0)      /* Enable Unknown Multicast Packet Trap to CPU port */
#define EN_UNUNICAST_TOCPU                  EnUnkUC2CPU /* Alias Name */
#define EN_UNMCAST_TOCPU                    EnUnkMC2CPU /* Alias Name */



#define SBFCTR                              (0x4500+SWCORE_BASE)  /* System Based Flow Control Threshold Register */
#define SBFCR0                              (0x000+SBFCTR)        /* System Based Flow Control Register 0 */
#define SBFCR1                              (0x004+SBFCTR)        /* System Based Flow Control Register 1 */
#define SBFCR2                              (0x008+SBFCTR)        /* System Based Flow Control Register 2 */
#define PBFCR0                              (0x00C+SBFCTR)        /* Port Based Flow Control Threshold Register */
#define PBFCR1                              (0x010+SBFCTR)        /* Port Based Flow Control Threshold Register */
#define PBFCR2                              (0x014+SBFCTR)        /* Port Based Flow Control Threshold Register */
#define PBFCR3                              (0x018+SBFCTR)        /* Port Based Flow Control Threshold Register */
#define PBFCR4                              (0x01C+SBFCTR)        /* Port Based Flow Control Threshold Register */
#define PBFCR5                              (0x020+SBFCTR)        /* Port Based Flow Control Threshold Register */
#define PBFCR6                              (0x024+SBFCTR)        /* Port Based Flow Control Threshold Register */
#define QDBFCRP0G0                          (0x028+SBFCTR)        /* Queue-Descriptor=Based Flow Control Threshold for Port 0 Group 0 */
#define QDBFCRP0G1                          (0x02C+SBFCTR)        /* Queue-Descriptor=Based Flow Control Threshold for Port 0 Group 1 */
#define QDBFCRP0G2                          (0x030+SBFCTR)        /* Queue-Descriptor=Based Flow Control Threshold for Port 0 Group 2 */
#define QDBFCRP1G0                          (0x034+SBFCTR)        /* Queue-Descriptor=Based Flow Control Threshold for Port 1 Group 0 */
#define QDBFCRP1G1                          (0x038+SBFCTR)        /* Queue-Descriptor=Based Flow Control Threshold for Port 1 Group 1 */
#define QDBFCRP1G2                          (0x03C+SBFCTR)        /* Queue-Descriptor=Based Flow Control Threshold for Port 1 Group 2 */
#define QDBFCRP2G0                          (0x040+SBFCTR)        /* Queue-Descriptor=Based Flow Control Threshold for Port 2 Group 0 */
#define QDBFCRP2G1                          (0x044+SBFCTR)        /* Queue-Descriptor=Based Flow Control Threshold for Port 2 Group 1 */
#define QDBFCRP2G2                          (0x048+SBFCTR)        /* Queue-Descriptor=Based Flow Control Threshold for Port 2 Group 2 */
#define QDBFCRP3G0                          (0x04C+SBFCTR)        /* Queue-Descriptor=Based Flow Control Threshold for Port 3 Group 0 */
#define QDBFCRP3G1                          (0x050+SBFCTR)        /* Queue-Descriptor=Based Flow Control Threshold for Port 3 Group 1 */
#define QDBFCRP3G2                          (0x054+SBFCTR)        /* Queue-Descriptor=Based Flow Control Threshold for Port 3 Group 2 */
#define QDBFCRP4G0                          (0x058+SBFCTR)        /* Queue-Descriptor=Based Flow Control Threshold for Port 4 Group 0 */
#define QDBFCRP4G1                          (0x05C+SBFCTR)        /* Queue-Descriptor=Based Flow Control Threshold for Port 4 Group 1 */
#define QDBFCRP4G2                          (0x060+SBFCTR)        /* Queue-Descriptor=Based Flow Control Threshold for Port 4 Group 2 */
#define QDBFCRP5G0                          (0x064+SBFCTR)        /* Queue-Descriptor=Based Flow Control Threshold for Port 5 Group 0 */
#define QDBFCRP5G1                          (0x068+SBFCTR)        /* Queue-Descriptor=Based Flow Control Threshold for Port 5 Group 1 */
#define QDBFCRP5G2                          (0x06C+SBFCTR)        /* Queue-Descriptor=Based Flow Control Threshold for Port 5 Group 2 */
#define QDBFCRP6G0                          (0x070+SBFCTR)        /* Queue-Descriptor=Based Flow Control Threshold for Port 6 Group 0 */
#define QDBFCRP6G1                          (0x074+SBFCTR)        /* Queue-Descriptor=Based Flow Control Threshold for Port 6 Group 1 */
#define QDBFCRP6G2                          (0x078+SBFCTR)        /* Queue-Descriptor=Based Flow Control Threshold for Port 6 Group 2 */
#define QPKTFCRP0G0                         (0x07C+SBFCTR)        /* Queue-Packet-Based Flow Control Register for Port 0 Group 0 */
#define QPKTFCRP0G1                         (0x080+SBFCTR)        /* Queue-Packet-Based Flow Control Register for Port 0 Group 1 */
#define QPKTFCRP0G2                         (0x084+SBFCTR)        /* Queue-Packet-Based Flow Control Register for Port 0 Group 2 */
#define QPKTFCRP1G0                         (0x088+SBFCTR)        /* Queue-Packet-Based Flow Control Register for Port 1 Group 0 */
#define QPKTFCRP1G1                         (0x08C+SBFCTR)        /* Queue-Packet-Based Flow Control Register for Port 1 Group 1 */
#define QPKTFCRP1G2                         (0x090+SBFCTR)        /* Queue-Packet-Based Flow Control Register for Port 1 Group 2 */
#define QPKTFCRP2G0                         (0x094+SBFCTR)        /* Queue-Packet-Based Flow Control Register for Port 2 Group 0 */
#define QPKTFCRP2G1                         (0x098+SBFCTR)        /* Queue-Packet-Based Flow Control Register for Port 2 Group 1 */
#define QPKTFCRP2G2                         (0x09C+SBFCTR)        /* Queue-Packet-Based Flow Control Register for Port 2 Group 2 */
#define QPKTFCRP3G0                         (0x0A0+SBFCTR)        /* Queue-Packet-Based Flow Control Register for Port 3 Group 0 */
#define QPKTFCRP3G1                         (0x0A4+SBFCTR)        /* Queue-Packet-Based Flow Control Register for Port 3 Group 1 */
#define QPKTFCRP3G2                         (0x0A8+SBFCTR)        /* Queue-Packet-Based Flow Control Register for Port 3 Group 2 */
#define QPKTFCRP4G0                         (0x0AC+SBFCTR)        /* Queue-Packet-Based Flow Control Register for Port 4 Group 0 */
#define QPKTFCRP4G1                         (0x0B0+SBFCTR)        /* Queue-Packet-Based Flow Control Register for Port 4 Group 1 */
#define QPKTFCRP4G2                         (0x0B4+SBFCTR)        /* Queue-Packet-Based Flow Control Register for Port 4 Group 2 */
#define QPKTFCRP5G0                         (0x0B8+SBFCTR)        /* Queue-Packet-Based Flow Control Register for Port 5 Group 0 */
#define QPKTFCRP5G1                         (0x0BC+SBFCTR)        /* Queue-Packet-Based Flow Control Register for Port 5 Group 1 */
#define QPKTFCRP5G2                         (0x0C0+SBFCTR)        /* Queue-Packet-Based Flow Control Register for Port 5 Group 2 */
#define FCCR0                               (0x0d0+SBFCTR)        /* Flow Control Configuration Register 0 */
#define FCCR1                               (0x0d4+SBFCTR)        /* Flow Control Configuration Register 1 */
#define PQPLGR                              (0x0d8+SBFCTR)        /* Per Queue Physical Length Gap Register */
#define QRR                                 (0x0dc+SBFCTR)        /* Queue Reset Register */
#define IQFCTCR                             (0x0E0+SBFCTR)        /* Input Queue Flow Control Threshold Configuration Register */

/* SBFCR0 */
#define S_DSC_RUNOUT_OFFSET                 (0)                   /* Offset for Descriptor Run Out Threshold */
#define S_DSC_RUNOUT_MASK                   (0x3FF<<0)            /* Mask for Descriptor Run Out Threshold */

/* SBFCR1 */
#define SDC_FCOFF_OFFSET                    (16)                  /* Offset for system flow control turn off threshold */
#define SDC_FCOFF_MASK                      (0x1ff<<16)           /* Mask for system flow control turn off threshold */
#define SDC_FCON_OFFSET                     (0)                   /* Offset for system flow control turn on threshold */
#define SDC_FCON_MASK                       (0x1ff<<0)            /* Mask for system flow control turn on threshold */

#define S_DSC_FCOFF_OFFSET                    (16)                  /* Offset for system flow control turn off threshold */
#define S_DSC_FCOFF_MASK                      (0x1ff<<16)           /* Mask for system flow control turn off threshold */
#define S_DSC_FCON_OFFSET                     (0)                   /* Offset for system flow control turn on threshold */
#define S_DSC_FCON_MASK                       (0x1ff<<0)            /* Mask for system flow control turn on threshold */



/* SBFCR2 */
#define S_Max_SBuf_FCOFF_OFFSET             (16)                  /* System max shared buffer flow control turn off threshold */
#define S_Max_SBuf_FCOFF_MASK               (0x1FF<<16)           /* System max shared buffer flow control turn off threshold */
#define S_Max_SBuf_FCON_OFFSET              (0)                   /* System max shared buffer flow control turn on threshold */
#define S_Max_SBuf_FCON_MASK                (0x1FF<<0)            /* System max shared buffer flow control turn on threshold */

/* PBFCR0~PBFCR6 - Port Based Flow Control Threshold Register */
#define P_MaxDSC_FCOFF_OFFSET               (16)                  /* Per-Port Max Used Descriptor Flow Control Turn Off Threshold */
#define P_MaxDSC_FCOFF_MASK                 (0x1ff<<16)           /* Per-Port Max Used Descriptor Flow Control Turn Off Threshold */
#define P_MaxDSC_FCON_OFFSET                (0)                   /* Per-Port Max Used Descriptor Flow Control Turn On Threshold */
#define P_MaxDSC_FCON_MASK                  (0x1ff<<0)            /* Per-Port Max Used Descriptor Flow Control Turn On Threshold */

/* QDBFCRP0G0,QDBFCRP0G1,QDBFCRP0G2
 * QDBFCRP1G0,QDBFCRP1G1,QDBFCRP1G2
 * QDBFCRP2G0,QDBFCRP2G1,QDBFCRP2G2
 * QDBFCRP3G0,QDBFCRP3G1,QDBFCRP3G2
 * QDBFCRP4G0,QDBFCRP4G1,QDBFCRP4G2
 * QDBFCRP5G0,QDBFCRP5G1,QDBFCRP5G2
 * - Queue-Descriptor=Based Flow Control Threshold for Port 0 Group 0 */
#define QG_DSC_FCOFF_OFFSET                 (16)                  /* Queue Descriptor Based Flow Control Turn Off Threshold */
#define QG_DSC_FCOFF_MASK                   (0x1f<<16)            /* Queue Descriptor Based Flow Control Turn Off Threshold */
#define QG_DSC_FCON_OFFSET                  (0)                   /* Queue Descriptor Based Flow Control Turn Off Threshold */
#define QG_DSC_FCON_MASK                    (0x7f<<0)             /* Queue Descriptor Based Flow Control Turn Off Threshold */

/* QPKTFCRP0G0,QPKTFCRP0G1,QPKTFCRP0G2
 * QPKTFCRP1G0,QPKTFCRP1G1,QPKTFCRP1G2
 * QPKTFCRP2G0,QPKTFCRP2G1,QPKTFCRP2G2
 * QPKTFCRP3G0,QPKTFCRP3G1,QPKTFCRP3G2
 * QPKTFCRP4G0,QPKTFCRP4G1,QPKTFCRP4G2
 * QPKTFCRP5G0,QPKTFCRP5G1,QPKTFCRP5G2
   - Queue-Packet-Based Flow Control Register for Port 0 Group 0 */
#define QG_QLEN_FCOFF_OFFSET                (16)                  /* Queue Packet Based Flow Control Turn Off Threshold */
#define QG_QLEN_FCOFF_MASK                  (0x1f<<16)            /* Queue Packet Based Flow Control Turn Off Threshold */
#define QG_QLEN_FCON_OFFSET                 (0)                   /* Queue Packet Based Flow Control Turn Off Threshold */
#define QG_QLEN_FCON_MASK                   (0x7f<<0)             /* Queue Packet Based Flow Control Turn Off Threshold */



/* FCCR0 - Flow Control enable/disable for port3~port0 */
/* FCCR1 - Flow Control enable/disable for port6~port4 */
#define Q_P0_EN_FC_OFFSET					(0)
#define Q_P0_EN_FC_MASK						(0x3f<<0)
#define Q_P1_EN_FC_OFFSET					(8)
#define Q_P1_EN_FC_MASK						(0x3f<<8)
#define Q_P2_EN_FC_OFFSET					(16)
#define Q_P2_EN_FC_MASK						(0x3f<<16)
#define Q_P3_EN_FC_OFFSET					(24)
#define Q_P3_EN_FC_MASK						(0x3f<<24)
#define Q_P4_EN_FC_OFFSET					(0)
#define Q_P4_EN_FC_MASK						(0x3f<<0)
#define Q_P5_EN_FC_OFFSET					(8)
#define Q_P5_EN_FC_MASK						(0x3f<<8)
#define Q_P6_EN_FC_OFFSET					(16)
#define Q_P6_EN_FC_MASK						(0x3f<<16)




/* PQPLGR - Per-Queue Physical length Gap Register */
#define QLEN_GAP_OFFSET						(0)					  /* Per Queue physical length gap */	
#define QLEN_GAP_MASK						(0xff<<0)			  /* Per Queue physical length gap */


/* QRR - Queue Reset Register */
#define QRST                                (1<<0)                /* Queue Reset */

//#ifdef CONFIG_RTL865XC_BCUT
/* IQFCTCR - Input Queue Flow Control Threshold Configuration Register */
#define IQ_DSC_FCON_OFFSET                 (8)                   /* Offset for input Queue Flow control turn OFF descriptor threshold */
#define IQ_DSC_FCON_MASK                   (0xff<<8)             /* Mask for input Queue Flow control turn OFF descriptor threshold */
#define IQ_DSC_FCOFF_OFFSET                (0)                   /* Offset for input Queue Flow control turn ON descriptor threshold */
#define IQ_DSC_FCOFF_MASK                  (0xff<<0)             /* Mask for input Queue Flow control turn ON descriptor threshold */
//#endif

/* QoS Function Control Register */
#define OQNCR_BASE                          (SWCORE_BASE+0x4700)  /* Output Queue Number Control Registers */
#define QOSFCR                              (0x00 + OQNCR_BASE)   /* QoS Function Control Register */
#define IBCR0                               (0x04 + OQNCR_BASE)   /* Ingress Bandwidth Control Register 0 */
#define IBCR1                               (0x08 + OQNCR_BASE)   /* Ingress Bandwidth Control Register 1 */
#define IBCR2                               (0x0C + OQNCR_BASE)   /* Ingress Bandwidth Control Register 2 */
#define PBPCR                               (0x14 + OQNCR_BASE)   /* Port Based Priority Control Register Address Mapping */
#define UPTCMCR0					        (0x18 + OQNCR_BASE)	  /* User Priority to Traffic Class Mapping for 1 output queue */	
#define UPTCMCR1				        	(0x1c + OQNCR_BASE)	  /* User Priority to Traffic Class Mapping for 2 output queue */	
#define UPTCMCR2				        	(0x20 + OQNCR_BASE)	  /* User Priority to Traffic Class Mapping for 3 output queue */	
#define UPTCMCR3			        		(0x24 + OQNCR_BASE)	  /* User Priority to Traffic Class Mapping for 4 output queue */	
#define UPTCMCR4			        		(0x28 + OQNCR_BASE)	  /* User Priority to Traffic Class Mapping for 5 output queue */	
#define UPTCMCR5			        		(0x2c + OQNCR_BASE)	  /* User Priority to Traffic Class Mapping for 6 output queue */	
#define LPTM8021Q                           (0x30 + OQNCR_BASE)   /*802.1Q priority to linear priority Transfer mapping.*/
#define DSCPCR0                             (0x34 + OQNCR_BASE)   /*DSCP Priority Control Register Address Mapping. */
#define DSCPCR1                             (0x38 + OQNCR_BASE)   /*DSCP Priority Control Register Address Mapping. */
#define DSCPCR2                             (0x3C + OQNCR_BASE)   /*DSCP Priority Control Register Address Mapping. */
#define DSCPCR3                             (0x40 + OQNCR_BASE)   /*DSCP Priority Control Register Address Mapping. */
#define DSCPCR4                             (0x44 + OQNCR_BASE)   /*DSCP Priority Control Register Address Mapping. */
#define DSCPCR5                             (0x48 + OQNCR_BASE)   /*DSCP Priority Control Register Address Mapping. */
#define DSCPCR6                             (0x4C + OQNCR_BASE)   /*DSCP Priority Control Register Address Mapping. */
#define QIDDPCR                             (0x50 + OQNCR_BASE)   /*Queue ID Decision Priority Register Address Mapping*/
#define QNUMCR                              (0x54 + OQNCR_BASE)   /*Queue Number Control Register*/
#define CPUQIDMCR0                          (0x58 + OQNCR_BASE)   /*CPU port QID Mapping Control Register (DP=include CPU)*/
#define CPUQIDMCR1                          (0x5C + OQNCR_BASE)   /*CPU port QID Mapping Control Register (DP=EXT0)*/
#define CPUQIDMCR2                          (0x60 + OQNCR_BASE)   /*CPU port QID Mapping Control Register (DP=EXT1)*/
#define CPUQIDMCR3                          (0x64 + OQNCR_BASE)   /*CPU port QID Mapping Control Register (DP=EXT2)*/
#define CPUQIDMCR4                          (0x68 + OQNCR_BASE)   /*CPU port QID Mapping Control Register (DP=multi-port of Ext port)*/
#define RMCR1P                              (0x6C + OQNCR_BASE)   /*802.1P  Remarking Control Register 0*/
#define DSCPRM0                             (0x70 + OQNCR_BASE)   /*DSCP Remarking Control Register 0*/
#define DSCPRM1                             (0x74 + OQNCR_BASE)   /*DSCP Remarking Control Register 1*/
#define RLRC                                (0x78 + OQNCR_BASE)   /*Remarking Layer Rule Control*/


/* QOSFCR - QoS Function Control Register */
#define BC_withPIFG_OFFSET                  (0)		              /* Bandwidth Conrol Include/Exclude Preamble&IFG. 0:exclude; 1:include */
#define BC_withPIFG_MASK                    (1<<0)                /* Bandwidth Conrol Include/Exclude Preamble&IFG. 0:exclude; 1:include */

/* IBCR0/IBCR1/IBCR2 general */
#define IBWC_ODDPORT_OFFSET		(16)				/* ODD-port Ingress Bandwidth Control Offset */
#define IBWC_ODDPORT_MASK			(0xFFFF<<16)	/* ODD-port Ingress Bandwidth Control MASK */
#define IBWC_EVENPORT_OFFSET		(0)				/* EVEN-port Ingress Bandwidth Control Offset */
#define IBWC_EVENPORT_MASK		(0xFFFF<<0)		/* EVEN-port Ingress Bandwidth Control MASK */

/* IBCR0 - Ingress Bandwidth Control Register 0 */
#define IBWC_P1_OFFSET                       (16)                  /* Port1 Ingress Bandwidth Control */
#define IBWC_P1_MASK                         (0xffff<<16)          /* Port1 Ingress Bandwidth Control */
#define IBWC_P0_OFFSET                       ( 0)                  /* Port0 Ingress Bandwidth Control */
#define IBWC_P0_MASK                         (0xffff<< 0)          /* Port0 Ingress Bandwidth Control */

/* IBCR1 - Ingress Bandwidth Control Register 1 */
#define IBWC_P3_OFFSET                       (16)                  /* Port3 Ingress Bandwidth Control */
#define IBWC_P3_MASK                         (0xffff<<16)          /* Port3 Ingress Bandwidth Control */
#define IBWC_P2_OFFSET                       ( 0)                  /* Port2 Ingress Bandwidth Control */
#define IBWC_P2_MASK                         (0xffff<< 0)          /* Port2 Ingress Bandwidth Control */

/* IBCR2 - Ingress Bandwidth Control Register 2 */
#define IBWC_P5_OFFSET                       (16)                  /* Port5 Ingress Bandwidth Control */
#define IBWC_P5_MASK                         (0xffff<<16)          /* Port5 Ingress Bandwidth Control */
#define IBWC_P4_OFFSET                       ( 0)                  /* Port4 Ingress Bandwidth Control */
#define IBWC_P4_MASK                         (0xffff<< 0)          /* Port4 Ingress Bandwidth Control */

/* PBPCR - Port Based Priority Control Register */
#define PBPRI_P8_OFFSET                      (24)                  /* Port Based Priority assign for port 8 */
#define PBPRI_P8_MASK						 (0x7<<24)			   /* Port Based Priority assign for port 8 */	
#define PBPRI_P7_OFFSET                      (21)                  /* Port Based Priority assign for port 7 */
#define PBPRI_P7_MASK						 (0x7<<21)			   /* Port Based Priority assign for port 7 */	
#define PBPRI_P6_OFFSET                      (18)                  /* Port Based Priority assign for port 6 */
#define PBPRI_P6_MASK						 (0x7<<18)			   /* Port Based Priority assign for port 6 */	
#define PBPRI_P5_OFFSET                      (15)                  /* Port Based Priority assign for port 5 */
#define PBPRI_P5_MASK						 (0x7<<15)			   /* Port Based Priority assign for port 5 */	
#define PBPRI_P4_OFFSET                      (12)                  /* Port Based Priority assign for port 4 */
#define PBPRI_P4_MASK						 (0x7<<12)			   /* Port Based Priority assign for port 4 */	
#define PBPRI_P3_OFFSET                      (9)                   /* Port Based Priority assign for port 3 */
#define PBPRI_P3_MASK						 (0x7<<9)			   /* Port Based Priority assign for port 3 */	
#define PBPRI_P2_OFFSET                      (6)                   /* Port Based Priority assign for port 2 */
#define PBPRI_P2_MASK						 (0x7<<6)			   /* Port Based Priority assign for port 2 */	
#define PBPRI_P1_OFFSET                      (3)                   /* Port Based Priority assign for port 1 */
#define PBPRI_P1_MASK						 (0x7<<3)			   /* Port Based Priority assign for port 1 */	
#define PBPRI_P0_OFFSET                      (0)                   /* Port Based Priority assign for port 0 */
#define PBPRI_P0_MASK						 (0x7<<0)			   /* Port Based Priority assign for port 0 */	


/* QIDDPCR - Queue ID Decision Priority Register Address Mapping*/
#define PBP_PRI_OFFSET                       0             /*Output queue decision priority assign for Port Based Priority*/
#define PBP_PRI_MASK                         (0xf<< 0)     /*Output queue decision priority assign for Port Based Priority*/
#define BP8021Q_PRI_OFFSET                   4             /*Output queue decision priority assign for 1Q Based Priority*/
#define BP8021Q_PRI_MASK                     (0xf<< 4)     /*Output queue decision priority assign for 1Q Based Priority*/
#define DSCP_PRI_OFFSET                      8             /*Output queue decision priority assign for DSCP Based Priority*/
#define DSCP_PRI_MASK                        (0xf<< 8)     /*Output queue decision priority assign for DSCP Based Priority*/
#define ACL_PRI_OFFSET                       12            /*Output queue decision priority assign for ACL Based Priority*/
#define ACL_PRI_MASK                         (0xf<<12)     /*Output queue decision priority assign for ACL Based Priority*/
#define NAPT_PRI_OFFSET                      16            /*Output queue decision priority assign for NAPT Based Priority*/
#define NAPT_PRI_MASK                        (0xf<<16)     /*Output queue decision priority assign for NAPT Based Priority*/

/* LPTM8021Q - 802.1Q priority to linear priority Transfer mapping. */
#define EN_8021Q2LTMPRI7					 21					   /* 8 level linear priority mapping for 802.1Q priority level 7 */	
#define EN_8021Q2LTMPRI7_MASK				 (0x7<<21)             /* 8 level linear priority mapping for 802.1Q priority level 7 */	
#define EN_8021Q2LTMPRI6					 18					   /* 8 level linear priority mapping for 802.1Q priority level 6 */	
#define EN_8021Q2LTMPRI6_MASK			  	 (0x7<<18)             /* 8 level linear priority mapping for 802.1Q priority level 6 */	
#define EN_8021Q2LTMPRI5					 15					   /* 8 level linear priority mapping for 802.1Q priority level 5 */	
#define EN_8021Q2LTMPRI5_MASK				 (0x7<<15)             /* 8 level linear priority mapping for 802.1Q priority level 5 */	
#define EN_8021Q2LTMPRI4					 12					   /* 8 level linear priority mapping for 802.1Q priority level 4 */	
#define EN_8021Q2LTMPRI4_MASK				 (0x7<<12)             /* 8 level linear priority mapping for 802.1Q priority level 4 */	
#define EN_8021Q2LTMPRI3					 9					   /* 8 level linear priority mapping for 802.1Q priority level 3 */	
#define EN_8021Q2LTMPRI3_MASK				 (0x7<<9)              /* 8 level linear priority mapping for 802.1Q priority level 3 */	
#define EN_8021Q2LTMPRI2					 6					   /* 8 level linear priority mapping for 802.1Q priority level 2 */	
#define EN_8021Q2LTMPRI2_MASK				 (0x7<<6)              /* 8 level linear priority mapping for 802.1Q priority level 2 */	
#define EN_8021Q2LTMPRI1					 3					   /* 8 level linear priority mapping for 802.1Q priority level 1 */	
#define EN_8021Q2LTMPRI1_MASK				 (0x7<<3)              /* 8 level linear priority mapping for 802.1Q priority level 1 */	
#define EN_8021Q2LTMPRI0					 0				 	   /* 8 level linear priority mapping for 802.1Q priority level 0 */	
#define EN_8021Q2LTMPRI0_MASK				 (0x7<<0)              /* 8 level linear priority mapping for 802.1Q priority level 0 */	


/* DSCPCR0 - DSCP Priority Control Register Address Mapping. */
#define DSCP0_PRI						 0
#define DSCP1_PRI						 3
#define DSCP2_PRI						 6
#define DSCP3_PRI						 9
#define DSCP4_PRI						 12
#define DSCP5_PRI						 15
#define DSCP6_PRI						 18
#define DSCP7_PRI						 21
#define DSCP8_PRI						 24
#define DSCP9_PRI						 27
#define DSCP32_PRI						6


/* QNUMCR - Queue Number Control Register*/
#define P0QNum_OFFSET                       (0)                   
#define P0QNum_MASK                         (7<<0)                /* Valid for 1~6 output queues */
#define P0QNum_1                            (1<<0)                /* 1 Output Queue */
#define P0QNum_2                            (2<<0)                /* 2 Output Queues */
#define P0QNum_3                            (3<<0)                /* 3 Output Queues */
#define P0QNum_4                            (4<<0)                /* 4 Output Queues */
#define P0QNum_5                            (5<<0)                /* 5 Output Queues */
#define P0QNum_6                            (6<<0)                /* 6 Output Queues */
#define P1QNum_OFFSET                       (3)                   
#define P1QNum_MASK                         (7<<3)                /* Valid for 1~6 output queues */
#define P1QNum_1                            (1<<3)                /* 1 Output Queue */
#define P1QNum_2                            (2<<3)                /* 2 Output Queues */
#define P1QNum_3                            (3<<3)                /* 3 Output Queues */
#define P1QNum_4                            (4<<3)                /* 4 Output Queues */
#define P1QNum_5                            (5<<3)                /* 5 Output Queues */
#define P1QNum_6                            (6<<3)                /* 6 Output Queues */
#define P2QNum_OFFSET                       (6)                   
#define P2QNum_MASK                         (7<<6)                /* Valid for 1~6 output queues */
#define P2QNum_1                            (1<<6)                /* 1 Output Queue */
#define P2QNum_2                            (2<<6)                /* 2 Output Queues */
#define P2QNum_3                            (3<<6)                /* 3 Output Queues */
#define P2QNum_4                            (4<<6)                /* 4 Output Queues */
#define P2QNum_5                            (5<<6)                /* 5 Output Queues */
#define P2QNum_6                            (6<<6)                /* 6 Output Queues */
#define P3QNum_OFFSET                       (9)                   
#define P3QNum_MASK                         (7<<9)                /* Valid for 1~6 output queues */
#define P3QNum_1                            (1<<9)                /* 1 Output Queue */
#define P3QNum_2                            (2<<9)                /* 2 Output Queues */
#define P3QNum_3                            (3<<9)                /* 3 Output Queues */
#define P3QNum_4                            (4<<9)                /* 4 Output Queues */
#define P3QNum_5                            (5<<9)                /* 5 Output Queues */
#define P3QNum_6                            (6<<9)                /* 6 Output Queues */
#define P4QNum_OFFSET                       (12)                  
#define P4QNum_MASK                         (7<<12)               /* Valid for 1~6 output queues */
#define P4QNum_1                            (1<<12)               /* 1 Output Queue */
#define P4QNum_2                            (2<<12)               /* 2 Output Queues */
#define P4QNum_3                            (3<<12)               /* 3 Output Queues */
#define P4QNum_4                            (4<<12)               /* 4 Output Queues */
#define P4QNum_5                            (5<<12)               /* 5 Output Queues */
#define P4QNum_6                            (6<<12)               /* 6 Output Queues */
#define P5QNum_OFFSET                       (15)                  
#define P5QNum_MASK                         (7<<15)               /* Valid for 1~6 output queues */
#define P5QNum_1                            (1<<15)               /* 1 Output Queue */
#define P5QNum_2                            (2<<15)               /* 2 Output Queues */
#define P5QNum_3                            (3<<15)               /* 3 Output Queues */
#define P5QNum_4                            (4<<15)               /* 4 Output Queues */
#define P5QNum_5                            (5<<15)               /* 5 Output Queues */
#define P5QNum_6                            (6<<15)               /* 6 Output Queues */
#define P6QNum_OFFSET                       (18)                  /* CPU port */
#define P6QNum_MASK                         (7<<18)               /* Valid for 1~6 output queues */
#define P6QNum_1                            (1<<18)               /* 1 Output Queue */
#define P6QNum_2                            (2<<18)               /* 2 Output Queues */
#define P6QNum_3                            (3<<18)               /* 3 Output Queues */
#define P6QNum_4                            (4<<18)               /* 4 Output Queues */
#define P6QNum_5                            (5<<18)               /* 5 Output Queues */
#define P6QNum_6                            (6<<18)               /* 6 Output Queues */

/* CPUQIDMCR0 - CPU port QID Mapping Control Register (DP=include CPU) */
#define CPUPri7QIDM_OFFSET                  28                    /* DP=include CPU, priority=7 */
#define CPUPri7QIDM_MASK                    (7<<28)               /* DP=include CPU, priority=7 */
#define CPUPri6QIDM_OFFSET                  24                    /* DP=include CPU, priority=6 */
#define CPUPri6QIDM_MASK                    (7<<24)               /* DP=include CPU, priority=6 */
#define CPUPri5QIDM_OFFSET                  20                    /* DP=include CPU, priority=5 */
#define CPUPri5QIDM_MASK                    (7<<20)               /* DP=include CPU, priority=5 */
#define CPUPri4QIDM_OFFSET                  16                    /* DP=include CPU, priority=4 */
#define CPUPri4QIDM_MASK                    (7<<16)               /* DP=include CPU, priority=4 */
#define CPUPri3QIDM_OFFSET                  12                    /* DP=include CPU, priority=3 */
#define CPUPri3QIDM_MASK                    (7<<12)               /* DP=include CPU, priority=3 */
#define CPUPri2QIDM_OFFSET                  8                     /* DP=include CPU, priority=2 */
#define CPUPri2QIDM_MASK                    (7<<8)                /* DP=include CPU, priority=2 */
#define CPUPri1QIDM_OFFSET                  4                     /* DP=include CPU, priority=1 */
#define CPUPri1QIDM_MASK                    (7<<4)                /* DP=include CPU, priority=1 */
#define CPUPri0QIDM_OFFSET                  0                     /* DP=include CPU, priority=0 */
#define CPUPri0QIDM_MASK                    (7<<0)                /* DP=include CPU, priority=0 */

/* CPUQIDMCR1 - CPU port QID Mapping Control Register (DP=EXT0) */
#define EXT0Pri7QIDM_OFFSET                 28                    /* DP=EXT0, priority=7 */
#define EXT0Pri7QIDM_MASK                   (7<<28)               /* DP=EXT0, priority=7 */
#define EXT0Pri6QIDM_OFFSET                 24                    /* DP=EXT0, priority=6 */
#define EXT0Pri6QIDM_MASK                   (7<<24)               /* DP=EXT0, priority=6 */
#define EXT0Pri5QIDM_OFFSET                 20                    /* DP=EXT0, priority=5 */
#define EXT0Pri5QIDM_MASK                   (7<<20)               /* DP=EXT0, priority=5 */
#define EXT0Pri4QIDM_OFFSET                 16                    /* DP=EXT0, priority=4 */
#define EXT0Pri4QIDM_MASK                   (7<<16)               /* DP=EXT0, priority=4 */
#define EXT0Pri3QIDM_OFFSET                 12                    /* DP=EXT0, priority=3 */
#define EXT0Pri3QIDM_MASK                   (7<<12)               /* DP=EXT0, priority=3 */
#define EXT0Pri2QIDM_OFFSET                 8                     /* DP=EXT0, priority=2 */
#define EXT0Pri2QIDM_MASK                   (7<<8)                /* DP=EXT0, priority=2 */
#define EXT0Pri1QIDM_OFFSET                 4                     /* DP=EXT0, priority=1 */
#define EXT0Pri1QIDM_MASK                   (7<<4)                /* DP=EXT0, priority=1 */
#define EXT0Pri0QIDM_OFFSET                 0                     /* DP=EXT0, priority=0 */
#define EXT0Pri0QIDM_MASK                   (7<<0)                /* DP=EXT0, priority=0 */

/* CPUQIDMCR1 - CPU port QID Mapping Control Register (DP=EXT1) */
#define EXT1Pri7QIDM_OFFSET                 28                    /* DP=EXT1, priority=7 */
#define EXT1Pri7QIDM_MASK                   (7<<28)               /* DP=EXT1, priority=7 */
#define EXT1Pri6QIDM_OFFSET                 24                    /* DP=EXT1, priority=6 */
#define EXT1Pri6QIDM_MASK                   (7<<24)               /* DP=EXT1, priority=6 */
#define EXT1Pri5QIDM_OFFSET                 20                    /* DP=EXT1, priority=5 */
#define EXT1Pri5QIDM_MASK                   (7<<20)               /* DP=EXT1, priority=5 */
#define EXT1Pri4QIDM_OFFSET                 16                    /* DP=EXT1, priority=4 */
#define EXT1Pri4QIDM_MASK                   (7<<16)               /* DP=EXT1, priority=4 */
#define EXT1Pri3QIDM_OFFSET                 12                    /* DP=EXT1, priority=3 */
#define EXT1Pri3QIDM_MASK                   (7<<12)               /* DP=EXT1, priority=3 */
#define EXT1Pri2QIDM_OFFSET                 8                     /* DP=EXT1, priority=2 */
#define EXT1Pri2QIDM_MASK                   (7<<8)                /* DP=EXT1, priority=2 */
#define EXT1Pri1QIDM_OFFSET                 4                     /* DP=EXT1, priority=1 */
#define EXT1Pri1QIDM_MASK                   (7<<4)                /* DP=EXT1, priority=1 */
#define EXT1Pri0QIDM_OFFSET                 0                     /* DP=EXT1, priority=0 */
#define EXT1Pri0QIDM_MASK                   (7<<0)                /* DP=EXT1, priority=0 */

/* CPUQIDMCR1 - CPU port QID Mapping Control Register (DP=EXT2) */
#define EXT2Pri7QIDM_OFFSET                 28                    /* DP=EXT2, priority=7 */
#define EXT2Pri7QIDM_MASK                   (7<<28)               /* DP=EXT2, priority=7 */
#define EXT2Pri6QIDM_OFFSET                 24                    /* DP=EXT2, priority=6 */
#define EXT2Pri6QIDM_MASK                   (7<<24)               /* DP=EXT2, priority=6 */
#define EXT2Pri5QIDM_OFFSET                 20                    /* DP=EXT2, priority=5 */
#define EXT2Pri5QIDM_MASK                   (7<<20)               /* DP=EXT2, priority=5 */
#define EXT2Pri4QIDM_OFFSET                 16                    /* DP=EXT2, priority=4 */
#define EXT2Pri4QIDM_MASK                   (7<<16)               /* DP=EXT2, priority=4 */
#define EXT2Pri3QIDM_OFFSET                 12                    /* DP=EXT2, priority=3 */
#define EXT2Pri3QIDM_MASK                   (7<<12)               /* DP=EXT2, priority=3 */
#define EXT2Pri2QIDM_OFFSET                 8                     /* DP=EXT2, priority=2 */
#define EXT2Pri2QIDM_MASK                   (7<<8)                /* DP=EXT2, priority=2 */
#define EXT2Pri1QIDM_OFFSET                 4                     /* DP=EXT2, priority=1 */
#define EXT2Pri1QIDM_MASK                   (7<<4)                /* DP=EXT2, priority=1 */
#define EXT2Pri0QIDM_OFFSET                 0                     /* DP=EXT2, priority=0 */
#define EXT2Pri0QIDM_MASK                   (7<<0)                /* DP=EXT2, priority=0 */

/* CPUQIDMCR1 - CPU port QID Mapping Control Register (DP=MultiEXT) */
#define MultiEXTPri7QIDM_OFFSET             28                    /* DP=multi-port of Ext port, priority=7 */
#define MultiEXTPri7QIDM_MASK               (7<<28)               /* DP=multi-port of Ext port, priority=7 */
#define MultiEXTPri6QIDM_OFFSET             24                    /* DP=multi-port of Ext port, priority=6 */
#define MultiEXTPri6QIDM_MASK               (7<<24)               /* DP=multi-port of Ext port, priority=6 */
#define MultiEXTPri5QIDM_OFFSET             20                    /* DP=multi-port of Ext port, priority=5 */
#define MultiEXTPri5QIDM_MASK               (7<<20)               /* DP=multi-port of Ext port, priority=5 */
#define MultiEXTPri4QIDM_OFFSET             16                    /* DP=multi-port of Ext port, priority=4 */
#define MultiEXTPri4QIDM_MASK               (7<<16)               /* DP=multi-port of Ext port, priority=4 */
#define MultiEXTPri3QIDM_OFFSET             12                    /* DP=multi-port of Ext port, priority=3 */
#define MultiEXTPri3QIDM_MASK               (7<<12)               /* DP=multi-port of Ext port, priority=3 */
#define MultiEXTPri2QIDM_OFFSET             8                     /* DP=multi-port of Ext port, priority=2 */
#define MultiEXTPri2QIDM_MASK               (7<<8)                /* DP=multi-port of Ext port, priority=2 */
#define MultiEXTPri1QIDM_OFFSET             4                     /* DP=multi-port of Ext port, priority=1 */
#define MultiEXTPri1QIDM_MASK               (7<<4)                /* DP=multi-port of Ext port, priority=1 */
#define MultiEXTPri0QIDM_OFFSET             0                     /* DP=multi-port of Ext port, priority=0 */
#define MultiEXTPri0QIDM_MASK               (7<<0)                /* DP=multi-port of Ext port, priority=0 */

/* RMCR1P - 802.1P  Remarking Control Register 0 */
#define RM1P_EN_OFFSET                        (24)                  /* Enable 802.1p remarking for destination port[7:0] */
#define RM1P_EN_MASK                          (0xff<<24)            /* Enable 802.1p remarking for destination port */
#define RM7_1P_OFFSET                         (21)                  /* New 3-bit priority for system parsed priority 7 */
#define RM7_1P_MASK                           (0x7<<21)             /* New 3-bit priority for system parsed priority 7 */
#define RM6_1P_OFFSET                         (18)                  /* New 3-bit priority for system parsed priority 6 */
#define RM6_1P_MASK                           (0x7<<18)             /* New 3-bit priority for system parsed priority 6 */
#define RM5_1P_OFFSET                         (15)                  /* New 3-bit priority for system parsed priority 5 */
#define RM5_1P_MASK                           (0x7<<15)             /* New 3-bit priority for system parsed priority 5 */
#define RM4_1P_OFFSET                         (12)                  /* New 3-bit priority for system parsed priority 4 */
#define RM4_1P_MASK                           (0x7<<12)             /* New 3-bit priority for system parsed priority 4 */
#define RM3_1P_OFFSET                         ( 9)                  /* New 3-bit priority for system parsed priority 3 */
#define RM3_1P_MASK                           (0x7<< 9)             /* New 3-bit priority for system parsed priority 3 */
#define RM2_1P_OFFSET                         ( 6)                  /* New 3-bit priority for system parsed priority 2 */
#define RM2_1P_MASK                           (0x7<< 6)             /* New 3-bit priority for system parsed priority 2 */
#define RM1_1P_OFFSET                         ( 3)                  /* New 3-bit priority for system parsed priority 1 */
#define RM1_1P_MASK                           (0x7<< 3)             /* New 3-bit priority for system parsed priority 1 */
#define RM0_1P_OFFSET                         ( 0)                  /* New 3-bit priority for system parsed priority 0 */
#define RM0_1P_MASK                           (0x7<< 0)             /* New 3-bit priority for system parsed priority 0 */

/* DSCPRM0 - DSCP Remarking Control Register 0 */
#define RM1P_EN_8_OFFSET                      (31)                  /* Enable 802.1p remarking for destination port[8] */
#define RM1P_EN_8                             (1<<31)               /* Enable 802.1p remarking for destination port[8] */
#define DSCPRM4_OFFSET                        (24)                  /* New 6-bit DSCP value for system parsed 3-bit priority 4 */
#define DSCPRM4_MASK                          (0x3f<<24)            /* New 6-bit DSCP value for system parsed 3-bit priority 4 */
#define DSCPRM3_OFFSET                        (18)                  /* New 6-bit DSCP value for system parsed 3-bit priority 3 */
#define DSCPRM3_MASK                          (0x3f<<18)            /* New 6-bit DSCP value for system parsed 3-bit priority 3 */
#define DSCPRM2_OFFSET                        (12)                  /* New 6-bit DSCP value for system parsed 3-bit priority 2 */
#define DSCPRM2_MASK                          (0x3f<<12)            /* New 6-bit DSCP value for system parsed 3-bit priority 2 */
#define DSCPRM1_OFFSET                        ( 6)                  /* New 6-bit DSCP value for system parsed 3-bit priority 1 */
#define DSCPRM1_MASK                          (0x3f<< 6)            /* New 6-bit DSCP value for system parsed 3-bit priority 1 */
#define DSCPRM0_OFFSET                        ( 0)                  /* New 6-bit DSCP value for system parsed 3-bit priority 0 */
#define DSCPRM0_MASK                          (0x3f<< 0)            /* New 6-bit DSCP value for system parsed 3-bit priority 0 */

/* DSCPRM1 - DSCP Remarking Control Register 1 */
#define DSCPRM_EN_OFFSET                      (23)                  /* Enable DSCP Remark for destination output port[8:0] */
#define DSCPRM_EN_MASK                        (0x1ff<<23)           /* Enable DSCP Remark for destination output port */
#define DSCPRM7_OFFSET                        (12)                  /* New 6-bit DSCP value for system parsed 3-bit priority 7 */
#define DSCPRM7_MASK                          (0x3f<<12)            /* New 6-bit DSCP value for system parsed 3-bit priority 7 */
#define DSCPRM6_OFFSET                        ( 6)                  /* New 6-bit DSCP value for system parsed 3-bit priority 6 */
#define DSCPRM6_MASK                          (0x3f<< 6)            /* New 6-bit DSCP value for system parsed 3-bit priority 6 */
#define DSCPRM5_OFFSET                        ( 0)                  /* New 6-bit DSCP value for system parsed 3-bit priority 5 */
#define DSCPRM5_MASK                          (0x3f<< 0)            /* New 6-bit DSCP value for system parsed 3-bit priority 5 */

/* RLRC - Remarking Layer Rule Control */
#define RMLC_DSCP_L4                        (1<<5)                  /* Applied on L4 Operation */
#define RMLC_DSCP_L3                        (1<<4)                  /* Applied on L3 Operation */
#define RMLC_DSCP_L2                        (1<<3)                  /* Applied on L2 Operation */
#define RMLC_8021P_L4                       (1<<2)                  /* Applied on L4 Operation */
#define RMLC_8021P_L3                       (1<<1)                  /* Applied on L3 Operation */
#define RMLC_8021P_L2                       (1<<0)                  /* Applied on L2 Operation */


/*
 * Packet Scheduling Control Register */
#define PSCR                                (SWCORE_BASE + 0x4800)
#define P0Q0RGCR                            (0x000 + PSCR)          /* Rate Guarantee Control Register of Port 0 Queue 0 */
#define P0Q1RGCR                            (0x004 + PSCR)          /* Rate Guarantee Control Register of Port 0 Queue 1 */
#define P0Q2RGCR                            (0x008 + PSCR)          /* Rate Guarantee Control Register of Port 0 Queue 2 */
#define P0Q3RGCR                            (0x00C + PSCR)          /* Rate Guarantee Control Register of Port 0 Queue 3 */
#define P0Q4RGCR                            (0x010 + PSCR)          /* Rate Guarantee Control Register of Port 0 Queue 4 */
#define P0Q5RGCR                            (0x014 + PSCR)          /* Rate Guarantee Control Register of Port 0 Queue 5 */
#define P1Q0RGCR                            (0x018 + PSCR)          /* Rate Guarantee Control Register of Port 1 Queue 0 */
#define P1Q1RGCR                            (0x01C + PSCR)          /* Rate Guarantee Control Register of Port 1 Queue 1 */
#define P1Q2RGCR                            (0x020 + PSCR)          /* Rate Guarantee Control Register of Port 1 Queue 2 */
#define P1Q3RGCR                            (0x024 + PSCR)          /* Rate Guarantee Control Register of Port 1 Queue 3 */
#define P1Q4RGCR                            (0x028 + PSCR)          /* Rate Guarantee Control Register of Port 1 Queue 4 */
#define P1Q5RGCR                            (0x02C + PSCR)          /* Rate Guarantee Control Register of Port 1 Queue 5 */
#define P2Q0RGCR                            (0x030 + PSCR)          /* Rate Guarantee Control Register of Port 2 Queue 0 */
#define P2Q1RGCR                            (0x034 + PSCR)          /* Rate Guarantee Control Register of Port 2 Queue 1 */
#define P2Q2RGCR                            (0x038 + PSCR)          /* Rate Guarantee Control Register of Port 2 Queue 2 */
#define P2Q3RGCR                            (0x03C + PSCR)          /* Rate Guarantee Control Register of Port 2 Queue 3 */
#define P2Q4RGCR                            (0x040 + PSCR)          /* Rate Guarantee Control Register of Port 2 Queue 4 */
#define P2Q5RGCR                            (0x044 + PSCR)          /* Rate Guarantee Control Register of Port 2 Queue 5 */
#define P3Q0RGCR                            (0x048 + PSCR)          /* Rate Guarantee Control Register of Port 3 Queue 0 */
#define P3Q1RGCR                            (0x04C + PSCR)          /* Rate Guarantee Control Register of Port 3 Queue 1 */
#define P3Q2RGCR                            (0x050 + PSCR)          /* Rate Guarantee Control Register of Port 3 Queue 2 */
#define P3Q3RGCR                            (0x054 + PSCR)          /* Rate Guarantee Control Register of Port 3 Queue 3 */
#define P3Q4RGCR                            (0x058 + PSCR)          /* Rate Guarantee Control Register of Port 3 Queue 4 */
#define P3Q5RGCR                            (0x05C + PSCR)          /* Rate Guarantee Control Register of Port 3 Queue 5 */
#define P4Q0RGCR                            (0x060 + PSCR)          /* Rate Guarantee Control Register of Port 4 Queue 0 */
#define P4Q1RGCR                            (0x064 + PSCR)          /* Rate Guarantee Control Register of Port 4 Queue 1 */
#define P4Q2RGCR                            (0x068 + PSCR)          /* Rate Guarantee Control Register of Port 4 Queue 2 */
#define P4Q3RGCR                            (0x06C + PSCR)          /* Rate Guarantee Control Register of Port 4 Queue 3 */
#define P4Q4RGCR                            (0x070 + PSCR)          /* Rate Guarantee Control Register of Port 4 Queue 4 */
#define P4Q5RGCR                            (0x074 + PSCR)          /* Rate Guarantee Control Register of Port 4 Queue 5 */
#define P5Q0RGCR                            (0x078 + PSCR)          /* Rate Guarantee Control Register of Port 5 Queue 0 */
#define P5Q1RGCR                            (0x07C + PSCR)          /* Rate Guarantee Control Register of Port 5 Queue 1 */
#define P5Q2RGCR                            (0x080 + PSCR)          /* Rate Guarantee Control Register of Port 5 Queue 2 */
#define P5Q3RGCR                            (0x084 + PSCR)          /* Rate Guarantee Control Register of Port 5 Queue 3 */
#define P5Q4RGCR                            (0x088 + PSCR)          /* Rate Guarantee Control Register of Port 5 Queue 4 */
#define P5Q5RGCR                            (0x08C + PSCR)          /* Rate Guarantee Control Register of Port 5 Queue 5 */
#define P6Q0RGCR                            (0x090 + PSCR)          /* Rate Guarantee Control Register of Port 6 Queue 0 */
#define P6Q1RGCR                            (0x094 + PSCR)          /* Rate Guarantee Control Register of Port 6 Queue 1 */
#define P6Q2RGCR                            (0x098 + PSCR)          /* Rate Guarantee Control Register of Port 6 Queue 2 */
#define P6Q3RGCR                            (0x09C + PSCR)          /* Rate Guarantee Control Register of Port 6 Queue 3 */
#define P6Q4RGCR                            (0x0A0 + PSCR)          /* Rate Guarantee Control Register of Port 6 Queue 4 */
#define P6Q5RGCR                            (0x0A4 + PSCR)          /* Rate Guarantee Control Register of Port 6 Queue 5 */
#define WFQRCRP0                            (0x0B0 + PSCR)          /* Weighted Fair Queue Rate Control Register of Port 0 */
#define WFQWCR0P0                           (0x0B4 + PSCR)          /* Weighted Fair Queue Weighting Control Register 0 of Port 0 */
#define WFQWCR1P0                           (0x0B8 + PSCR)          /* Weighted Fair Queue Weighting Control Register 1 of Port 0 */
#define WFQRCRP1                            (0x0BC + PSCR)          /* Weighted Fair Queue Rate Control Register of Port 1 */
#define WFQWCR0P1                           (0x0C0 + PSCR)          /* Weighted Fair Queue Weighting Control Register 0 of Port 1 */
#define WFQWCR1P1                           (0x0C4 + PSCR)          /* Weighted Fair Queue Weighting Control Register 1 of Port 1 */
#define WFQRCRP2                            (0x0C8 + PSCR)          /* Weighted Fair Queue Rate Control Register of Port 2 */
#define WFQWCR0P2                           (0x0CC + PSCR)          /* Weighted Fair Queue Weighting Control Register 0 of Port 2 */
#define WFQWCR1P2                           (0x0D0 + PSCR)          /* Weighted Fair Queue Weighting Control Register 1 of Port 2 */
#define WFQRCRP3                            (0x0D4 + PSCR)          /* Weighted Fair Queue Rate Control Register of Port 3 */
#define WFQWCR0P3                           (0x0D8 + PSCR)          /* Weighted Fair Queue Weighting Control Register 0 of Port 3 */
#define WFQWCR1P3                           (0x0DC + PSCR)          /* Weighted Fair Queue Weighting Control Register 1 of Port 3 */
#define WFQRCRP4                            (0x0E0 + PSCR)          /* Weighted Fair Queue Rate Control Register of Port 4 */
#define WFQWCR0P4                           (0x0E4 + PSCR)          /* Weighted Fair Queue Weighting Control Register 0 of Port 4 */
#define WFQWCR1P4                           (0x0E8 + PSCR)          /* Weighted Fair Queue Weighting Control Register 1 of Port 4 */
#define WFQRCRP5                            (0x0EC + PSCR)          /* Weighted Fair Queue Rate Control Register of Port 5 */
#define WFQWCR0P5                           (0x0F0 + PSCR)          /* Weighted Fair Queue Weighting Control Register 0 of Port 5 */
#define WFQWCR1P5                           (0x0F4 + PSCR)          /* Weighted Fair Queue Weighting Control Register 1 of Port 5 */
#define WFQRCRP6                            (0x0F8 + PSCR)          /* Weighted Fair Queue Rate Control Register of Port 6 */
#define WFQWCR0P6                           (0x0FC + PSCR)          /* Weighted Fair Queue Weighting Control Register 0 of Port 6 */
#define WFQWCR1P6                           (0x100 + PSCR)          /* Weighted Fair Queue Weighting Control Register 1 of Port 6 */
#define ELBPCR                              (0x104 + PSCR)          /* Leaky Bucket Parameter Control Register */
#define ELBTTCR                             (0x108 + PSCR)          /* Leaky Bucket Token Threshold Control Register */
#define ILBPCR1                             (0x10C + PSCR)          /* Ingress Leaky Bucket Parameter Control Register1 */
#define ILBPCR2                             (0x110 + PSCR)          /* Ingress Leaky Bucket Parameter Control Register2 */
#define ILB_CURRENT_TOKEN                   (0x114 + PSCR)          /* The current token of the Leaky bucket 2Bytes per port(Port 0~Port5) */

/* P0Q0RGCG - Per-Queue Rate Guarantee Control Register */
#define PPR_OFFSET                          (24)                    /* Peak Packet Rate, in times of APR        CNT2 */
#define PPR_MASK                            (7<<24)                 /* Peak Packet Rate, in times of APR        CNT2 */
#define L1_OFFSET                           (16)                    /* Bucket burst size of APR. unit: 1KB      MAX  */
#define L1_MASK                             (0xff<<16)              /* Bucket burst size of APR. unit: 1KB      MAX  */
#define APR_OFFSET                          (0)                     /* Average Packet Rate, in times of 64Kbps  CNT1 */
#define APR_MASK                            (0x3FFF<<0)             /* Average Packet Rate, in times of 64Kbps  CNT1 */

/* WFQECR0P0 - Weighted Fair Queue Weighting Control Register 0 */
#define SCHE3_WFQ                           (1<<31)                 /* Schedule Type for Queue 3: 1-WFQ ,0-Strict Priority */
#define SCHE3_OFFSET                        (31)   		            /* Schedule Type for Queue 3: 1-WFQ ,0-Strict Priority */
#define SCHE3_MASK                          (1<<31)                 /* Schedule Type for Queue 3: 1-WFQ ,0-Strict Priority */
#define WEIGHT3_OFFSET                      (24)                    /* Weight value assign of queue 3 for WFQ */
#define WEIGHT3_MASK                        (0x7f<<24)              /* Weight value assign of queue 3 for WFQ */
#define SCHE2_WFQ                           (1<<23)                 /* Schedule Type for Queue 2: 1-WFQ ,0-Strict Priority */
#define SCHE2_OFFSET                        (23)                    /* Schedule Type for Queue 2: 1-WFQ ,0-Strict Priority */
#define SCHE2_MASK                          (1<<23)                 /* Schedule Type for Queue 2: 1-WFQ ,0-Strict Priority */
#define WEIGHT2_OFFSET                      (16)                    /* Weight value assign of queue 2 for WFQ */
#define WEIGHT2_MASK                        (0x7f<<16)              /* Weight value assign of queue 2 for WFQ */
#define SCHE1_WFQ                           (1<<15)                 /* Schedule Type for Queue 1: 1-WFQ ,0-Strict Priority */
#define SCHE1_OFFSET                        (15)                    /* Schedule Type for Queue 1: 1-WFQ ,0-Strict Priority */
#define SCHE1_MASK                          (1<<15)                 /* Schedule Type for Queue 1: 1-WFQ ,0-Strict Priority */
#define WEIGHT1_OFFSET                      ( 8)                    /* Weight value assign of queue 1 for WFQ */
#define WEIGHT1_MASK                        (0x7f<< 8)              /* Weight value assign of queue 1 for WFQ */
#define SCHE0_WFQ                           (1<<7)                  /* Schedule Type for Queue 0: 1-WFQ ,0-Strict Priority */
#define SCHE0_OFFSET                        ( 7)                    /* Schedule Type for Queue 0: 1-WFQ ,0-Strict Priority */
#define SCHE0_MASK                          (1<<7)                  /* Schedule Type for Queue 0: 1-WFQ ,0-Strict Priority */
#define WEIGHT0_OFFSET                      ( 0)                    /* Weight value assign of queue 0 for WFQ */
#define WEIGHT0_MASK                        (0x7f<< 0)              /* Weight value assign of queue 0 for WFQ */
/* WFQECR0P0 - Weighted Fair Queue Weighting Control Register 1 */
#define SCHE5_WFQ                           (1<<15)                 /* Schedule Type for Queue 5: 1-WFQ ,0-Strict Priority */
#define SCHE5_OFFSET                        (15)                    /* Schedule Type for Queue 5: 1-WFQ ,0-Strict Priority */
#define SCHE5_MASK                          (1<<15)                 /* Schedule Type for Queue 5: 1-WFQ ,0-Strict Priority */
#define WEIGHT5_OFFSET                      ( 8)                    /* Weight value assign of queue 5 for WFQ */
#define WEIGHT5_MASK                        (0x7f<< 8)              /* Weight value assign of queue 5 for WFQ */
#define SCHE4_WFQ                           (1<< 7)                 /* Schedule Type for Queue 4: 1-WFQ ,0-Strict Priority */
#define SCHE4_OFFSET                        ( 7)                    /* Schedule Type for Queue 4: 1-WFQ ,0-Strict Priority */
#define SCHE4_MASK                          (1<< 7)                 /* Schedule Type for Queue 4: 1-WFQ ,0-Strict Priority */
#define WEIGHT4_OFFSET                      ( 0)                    /* Weight value assign of queue 4 for WFQ */
#define WEIGHT4_MASK                        (0x7f<< 0)              /* Weight value assign of queue 4 for WFQ */

/* ELBPCR - Egress Leaky Bucket Parameter Control Register */
#define Token_OFFSET                        (8)                     /* Token used for adding budget in each time slot. */
#define Token_MASK                          (0xff<<8)               /* Token used for adding budget in each time slot. */
#define Tick_OFFSET                         (0)                     /* Tick used for time slot size unit */
#define Tick_MASK                           (0xff<<0)               /* Tick used for time slot size unit */

/* ELBTTCR - Egress Leaky Bucket Token Threshold Control Register */
#define L2_OFFSET                           (0)                     /* leaky Bucket Token Hi-threshold register */
#if 0/* conflict with flagDef.h */
#define L2_MASK                             (0xffff<<0)             /* leaky Bucket Token Hi-threshold register */
#endif

/* ILBPCR1 - Ingress Leaky Bucket Parameter Control Register 1 */
#define UpperBound_OFFSET                   (16)                    /* Ingress BWC Parameter Upper bound Threshold (unit: 400bytes) */
#define UpperBound_MASK                     (0xffff<<16)            /* Ingress BWC Parameter Upper bound Threshold (unit: 400bytes) */
#define LowerBound_OFFSET                   (0)                     /* Ingress BWC Parameter Lower Bound Threshold (unit: 400 bytes) */
#define LowerBound_MASK                     (0xffff<<0)             /* Ingress BWC Parameter Lower Bound Threshold (unit: 400 bytes) */

/* ILBPCR2 - Ingress Leaky Bucket Parameter Control Register 2 */
#define ILB_feedToken_OFFSET                (8)                     /* Token is used for adding budget in each time slot */
#define ILB_feedToken_MASK                  (0xff<<8)               /* Token is used for adding budget in each time slot */
#define ILB_Tick_OFFSET                     (0)                     /* Tick is used for time slot size unit. */
#define ILB_Tick_MASK                       (0xff<<0)               /* Tick is used for time slot size unit. */






#define VCR0						(0x00 +0x4A00+SWCORE_BASE)  /* Vlan Control register*/
#define VCR1						(0x04 +0x4A00+SWCORE_BASE)  /* Vlan Control register*/
#define PVCR0						(0x08 +0x4A00+SWCORE_BASE)  /* port base control register*/
#define PVCR1						(0x0C +0x4A00+SWCORE_BASE)  /* port base control register*/
#define PVCR2						(0x10 +0x4A00+SWCORE_BASE)  /* port base control register*/
#define PVCR3						(0x14 +0x4A00+SWCORE_BASE)  /* port base control register*/
#define PVCR4						(0x18 +0x4A00+SWCORE_BASE)  /* port base control register*/
#define PBVCR0						(0x1C +0x4A00+SWCORE_BASE) /* Protocol-Based VLAN Control Register 0      */ 
#define PBVCR1						(0x20 +0x4A00+SWCORE_BASE)	/* Protocol-Based VLAN Control Register 1      */
#define PBVR0_0						(0x24 +0x4A00+SWCORE_BASE) /* Protocol-Based VLAN Rule 0 -- IPX           */
#define PBVR0_1						(0x28 +0x4A00+SWCORE_BASE) /* Protocol-Based VLAN Rule 0 -- IPX           */
#define PBVR0_2						(0x2C +0x4A00+SWCORE_BASE) /* Protocol-Based VLAN Rule 0 -- IPX           */
#define PBVR0_3						(0x30 +0x4A00+SWCORE_BASE) /* Protocol-Based VLAN Rule 0 -- IPX           */
#define PBVR0_4						(0x34 +0x4A00+SWCORE_BASE) /* Protocol-Based VLAN Rule 0 -- IPX           */
#define PBVR1_0						(0x38 +0x4A00+SWCORE_BASE)		/* Protocol-Based VLAN Rule 1 NetBIOS       */
#define PBVR1_1						(0x3C +0x4A00+SWCORE_BASE)		/* Protocol-Based VLAN Rule 1 NetBIOS       */
#define PBVR1_2						(0x40 +0x4A00+SWCORE_BASE)		/* Protocol-Based VLAN Rule 1 NetBIOS       */
#define PBVR1_3						(0x44 +0x4A00+SWCORE_BASE)		/* Protocol-Based VLAN Rule 1 NetBIOS       */
#define PBVR1_4						(0x48 +0x4A00+SWCORE_BASE)		/* Protocol-Based VLAN Rule 1 NetBIOS       */
#define PBVR2_0						(0x4C +0x4A00+SWCORE_BASE)		/* Protocol-Based VLAN Rule 2 PPPoE Control       */
#define PBVR2_1						(0x50 +0x4A00+SWCORE_BASE)		/* Protocol-Based VLAN Rule 2 PPPoE Control      */
#define PBVR2_2						(0x54 +0x4A00+SWCORE_BASE)		/* Protocol-Based VLAN Rule 2 PPPoE Control       */
#define PBVR2_3						(0x58 +0x4A00+SWCORE_BASE)		/* Protocol-Based VLAN Rule 2 PPPoE Control       */
#define PBVR2_4						(0x5C +0x4A00+SWCORE_BASE)		/* Protocol-Based VLAN Rule 2 PPPoE Control       */
#define PBVR3_0						(0x60 +0x4A00+SWCORE_BASE)		/* Protocol-Based VLAN Rule 3 PPPoE session       */
#define PBVR3_1						(0x64 +0x4A00+SWCORE_BASE)		/* Protocol-Based VLAN Rule 3 PPPoE session      */
#define PBVR3_2						(0x68 +0x4A00+SWCORE_BASE)		/* Protocol-Based VLAN Rule 3 PPPoE session       */
#define PBVR3_3						(0x6C +0x4A00+SWCORE_BASE)		/* Protocol-Based VLAN Rule 3 PPPoE session       */
#define PBVR3_4						(0x70 +0x4A00+SWCORE_BASE)		/* Protocol-Based VLAN Rule 3 PPPoE session    */
#define PBVR4_0						(0x74 +0x4A00+SWCORE_BASE)		/* Protocol-Based VLAN Rule 4 User-defined 1       */
#define PBVR4_1						(0x78 +0x4A00+SWCORE_BASE)		/* Protocol-Based VLAN Rule 4 User-defined 1      */
#define PBVR4_2						(0x7C +0x4A00+SWCORE_BASE)		/* Protocol-Based VLAN Rule 4 User-defined 1       */
#define PBVR4_3						(0x80 +0x4A00+SWCORE_BASE)		/* Protocol-Based VLAN Rule 4 User-defined 1       */
#define PBVR4_4						(0x84 +0x4A00+SWCORE_BASE)		/* Protocol-Based VLAN Rule 4 User-defined 1       */
#define PBVR5_0						(0x88 +0x4A00+SWCORE_BASE)		/* Protocol-Based VLAN Rule 4 User-defined 2       */
#define PBVR5_1						(0x8C +0x4A00+SWCORE_BASE)		/* Protocol-Based VLAN Rule 4 User-defined 2     */
#define PBVR5_2						(0x90 +0x4A00+SWCORE_BASE)		/* Protocol-Based VLAN Rule 4 User-defined 2       */
#define PBVR5_3						(0x94 +0x4A00+SWCORE_BASE)		/* Protocol-Based VLAN Rule 4 User-defined 2       */
#define PBVR5_4						(0x98 +0x4A00+SWCORE_BASE)		/* Protocol-Based VLAN Rule 4 User-defined 2       */

/* VCR0 - Vlan Control register */
#define En1QtagVIDignore                    (1<<31)                 /* Enable 1Q vlan unware*/
#define EN_1QTAGVIDIGNORE                   En1QtagVIDignore        /* Alias Name */
#define P8_AcptFType_OFFSET                 25                      /* Accept Frame Type for VLAN ingress control (Ext2) */
#define P8_AcptFType_MASK                   (3<<25)                 /* Accept Frame Type for VLAN ingress control (Ext2) */
#define P8_AcptFType_AdmitAllFrame          (0<<25)                 /* Accept tagged and untagged frame (Ext2) */
#define P8_AcptFType_AdmitTaggedOnly        (1<<25)                 /* Accept tagged frame only (Ext2) */
#define P8_AcptFType_AdmitUntagged          (2<<25)                 /* Accept untagged frame and priority tag frame (Ext2) */
#define P7_AcptFType_OFFSET                 23                      /* Accept Frame Type for VLAN ingress control (Ext1) */
#define P7_AcptFType_MASK                   (3<<23)                 /* Accept Frame Type for VLAN ingress control (Ext1) */
#define P7_AcptFType_AdmitAllFrame          (0<<23)                 /* Accept tagged and untagged frame (Ext1) */
#define P7_AcptFType_AdmitTaggedOnly        (1<<23)                 /* Accept tagged frame only (Ext1) */
#define P7_AcptFType_AdmitUntagged          (2<<23)                 /* Accept untagged frame and priority tag frame (Ext1) */
#define P6_AcptFType_OFFSET                 21                      /* Accept Frame Type for VLAN ingress control (Ext0) */
#define P6_AcptFType_MASK                   (3<<21)                 /* Accept Frame Type for VLAN ingress control (Ext0) */
#define P6_AcptFType_AdmitAllFrame          (0<<21)                 /* Accept tagged and untagged frame (Ext0) */
#define P6_AcptFType_AdmitTaggedOnly        (1<<21)                 /* Accept tagged frame only (Ext0) */
#define P6_AcptFType_AdmitUntagged          (2<<21)                 /* Accept untagged frame and priority tag frame (Ext0) */
#define P5_AcptFType_OFFSET                 19                      /* Accept Frame Type for VLAN ingress control */
#define P5_AcptFType_MASK                   (3<<19)                 /* Accept Frame Type for VLAN ingress control */
#define P5_AcptFType_AdmitAllFrame          (0<<19)                 /* Accept tagged and untagged frame */
#define P5_AcptFType_AdmitTaggedOnly        (1<<19)                 /* Accept tagged frame only */
#define P5_AcptFType_AdmitUntagged          (2<<19)                 /* Accept untagged frame and priority tag frame */
#define P4_AcptFType_OFFSET                 17                      /* Accept Frame Type for VLAN ingress control */
#define P4_AcptFType_MASK                   (3<<17)                 /* Accept Frame Type for VLAN ingress control */
#define P4_AcptFType_AdmitAllFrame          (0<<17)                 /* Accept tagged and untagged frame */
#define P4_AcptFType_AdmitTaggedOnly        (1<<17)                 /* Accept tagged frame only */
#define P4_AcptFType_AdmitUntagged          (2<<17)                 /* Accept untagged frame and priority tag frame */
#define P3_AcptFType_OFFSET                 15                      /* Accept Frame Type for VLAN ingress control */
#define P3_AcptFType_MASK                   (3<<15)                 /* Accept Frame Type for VLAN ingress control */
#define P3_AcptFType_AdmitAllFrame          (0<<15)                 /* Accept tagged and untagged frame */
#define P3_AcptFType_AdmitTaggedOnly        (1<<15)                 /* Accept tagged frame only */
#define P3_AcptFType_AdmitUntagged          (2<<15)                 /* Accept untagged frame and priority tag frame */
#define P2_AcptFType_OFFSET                 13                      /* Accept Frame Type for VLAN ingress control */
#define P2_AcptFType_MASK                   (3<<13)                 /* Accept Frame Type for VLAN ingress control */
#define P2_AcptFType_AdmitAllFrame          (0<<13)                 /* Accept tagged and untagged frame */
#define P2_AcptFType_AdmitTaggedOnly        (1<<13)                 /* Accept tagged frame only */
#define P2_AcptFType_AdmitUntagged          (2<<13)                 /* Accept untagged frame and priority tag frame */
#define P1_AcptFType_OFFSET                 11                      /* Accept Frame Type for VLAN ingress control */
#define P1_AcptFType_MASK                   (3<<11)                 /* Accept Frame Type for VLAN ingress control */
#define P1_AcptFType_AdmitAllFrame          (0<<11)                 /* Accept tagged and untagged frame */
#define P1_AcptFType_AdmitTaggedOnly        (1<<11)                 /* Accept tagged frame only */
#define P1_AcptFType_AdmitUntagged          (2<<11)                 /* Accept untagged frame and priority tag frame */
#define P0_AcptFType_OFFSET                 9                       /* Accept Frame Type for VLAN ingress control */
#define P0_AcptFType_MASK                   (3<<9)                  /* Accept Frame Type for VLAN ingress control */
#define P0_AcptFType_AdmitAllFrame          (0<<9)                  /* Accept tagged and untagged frame */
#define P0_AcptFType_AdmitTaggedOnly        (1<<9)                  /* Accept tagged frame only */
#define P0_AcptFType_AdmitUntagged          (2<<9)                  /* Accept untagged frame and priority tag frame */
#define EnVlanInF_OFFSET                    0                       /* Enable Vlan Ingress Filtering */
#define EnVlanInF_MASK                      (0x1ff<<0)              /* Enable Vlan Ingress Filtering */
#define EN_VLAN_INGRESS_FILTER              (1)                     /* Backward compatible for EnVlanInF */
#define EN_ALL_PORT_VLAN_INGRESS_FILTER	    EnVlanInF_MASK          /* Alias Name */

/* PVCR0, PVCR1, PVCR2, PVCR3, PVCR4 - Port-Based VLAN Control Register */
#define PVIDP0_OFFSET                       0                       /* Default PVID for Port 0 */
#define PVIDP0_MASK                         (0xfff<< 0)             /* Default PVID for Port 0 */
#define DPRIOP0_OFFSET                      12                      /* Default Port Priority for Port 0 */
#define DPRIOP0_MASK                        (7<<12)                 /* Default Port Priority for Port 0 */
#define PVIDP1_OFFSET                       16                      /* Default PVID for Port 1 */
#define PVIDP1_MASK                         (0xfff<<16)             /* Default PVID for Port 1 */
#define DPRIOP1_OFFSET                      28                      /* Default Port Priority for Port 1 */
#define DPRIOP1_MASK                        (7<<28)                 /* Default Port Priority for Port 1 */
#define PVIDP2_OFFSET                       0                       /* Default PVID for Port 2 */
#define PVIDP2_MASK                         (0xfff<< 0)             /* Default PVID for Port 2 */
#define DPRIOP2_OFFSET                      12                      /* Default Port Priority for Port 2 */
#define DPRIOP2_MASK                        (7<<12)                 /* Default Port Priority for Port 2 */
#define PVIDP3_OFFSET                       16                      /* Default PVID for Port 3 */
#define PVIDP3_MASK                         (0xfff<<16)             /* Default PVID for Port 3 */
#define DPRIOP3_OFFSET                      28                      /* Default Port Priority for Port 3 */
#define DPRIOP3_MASK                        (7<<28)                 /* Default Port Priority for Port 3 */
#define PVIDP4_OFFSET                       0                       /* Default PVID for Port 4 */
#define PVIDP4_MASK                         (0xfff<< 0)             /* Default PVID for Port 4 */
#define DPRIOP4_OFFSET                      12                      /* Default Port Priority for Port 4 */
#define DPRIOP4_MASK                        (7<<12)                 /* Default Port Priority for Port 4 */
#define PVIDP5_OFFSET                       16                      /* Default PVID for Port 5 */
#define PVIDP5_MASK                         (0xfff<<16)             /* Default PVID for Port 5 */
#define DPRIOP5_OFFSET                      28                      /* Default Port Priority for Port 5 */
#define DPRIOP5_MASK                        (7<<28)                 /* Default Port Priority for Port 5 */
#define PVIDP6_OFFSET                       0                       /* Default PVID for Port 6 */
#define PVIDP6_MASK                         (0xfff<< 0)             /* Default PVID for Port 6 */
#define DPRIOP6_OFFSET                      12                      /* Default Port Priority for Port 6 */
#define DPRIOP6_MASK                        (7<<12)                 /* Default Port Priority for Port 6 */
#define PVIDP7_OFFSET                       16                      /* Default PVID for Port 7 */
#define PVIDP7_MASK                         (0xfff<<16)             /* Default PVID for Port 7 */
#define DPRIOP7_OFFSET                      28                      /* Default Port Priority for Port 7 */
#define DPRIOP7_MASK                        (7<<28)                 /* Default Port Priority for Port 7 */
#define PVIDP8_OFFSET                       0                       /* Default PVID for Port 8 */
#define PVIDP8_MASK                         (0xfff<< 0)             /* Default PVID for Port 8 */
#define DPRIOP8_OFFSET                      12                      /* Default Port Priority for Port 8 */
#define DPRIOP8_MASK                        (7<<12)                 /* Default Port Priority for Port 8 */


/*
 * 802.1X Control Register */
#define DOT1X_BASE                          (SWCORE_BASE + 0x4B00)
#define DOT1XPORTCR					        (0x00 +DOT1X_BASE)      /* 802.1X port base control register*/
#define DOT1XMACCR					        (0x04 +DOT1X_BASE)      /* 802.1X mac base control register*/
#define GVGCR						        (0x08 +DOT1X_BASE)      /* Guest VLAN registerr*/

#define DOT1X_DIR_BOTH				0							/* Bi-directional */
#define DOT1X_DIR_IN				1							/* only filter IN */
#define DOT1X_UNAUTH				0							/* Un-auth */
#define DOT1X_AUTH					1							/* Auth */
#define DOT1X_DISABLE				0							/* Disable */
#define DOT1X_ENABLE				1							/* Enable */

/* DOT1XPORTCR - 802.1X port base control register */
#define Dot1XPB_P8OPDIR                     (1<<26)                 /* Uni/Bi-directional Control Type for Port 8. 0:BOTH, 1:IN */
#define Dot1XPB_P8Auth                      (1<<25)                 /* Authentication Status for Port 8. 0:Unauthorized, 1:Authorized */
#define Dot1XPB_P8En                        (1<<24)                 /* Enabled/Disable Port Based Access Control for Port 8. 0:Disable, 1:Enable */
#define Dot1XPB_P7OPDIR                     (1<<23)                 /* Uni/Bi-directional Control Type for Port 7. 0:BOTH, 1:IN */
#define Dot1XPB_P7Auth                      (1<<22)                 /* Authentication Status for Port 7. 0:Unauthorized, 1:Authorized */
#define Dot1XPB_P7En                        (1<<21)                 /* Enabled/Disable Port Based Access Control for Port 7. 0:Disable, 1:Enable */
#define Dot1XPB_P6OPDIR                     (1<<20)                 /* Uni/Bi-directional Control Type for Port 6. 0:BOTH, 1:IN */
#define Dot1XPB_P6Auth                      (1<<19)                 /* Authentication Status for Port 6. 0:Unauthorized, 1:Authorized */
#define Dot1XPB_P6En                        (1<<18)                 /* Enabled/Disable Port Based Access Control for Port 6. 0:Disable, 1:Enable */
#define Dot1XPB_P5OPDIR                     (1<<17)                 /* Uni/Bi-directional Control Type for Port 5. 0:BOTH, 1:IN */
#define Dot1XPB_P5Auth                      (1<<16)                 /* Authentication Status for Port 5. 0:Unauthorized, 1:Authorized */
#define Dot1XPB_P5En                        (1<<15)                 /* Enabled/Disable Port Based Access Control for Port 5. 0:Disable, 1:Enable */
#define Dot1XPB_P4OPDIR                     (1<<14)                 /* Uni/Bi-directional Control Type for Port 4. 0:BOTH, 1:IN */
#define Dot1XPB_P4Auth                      (1<<13)                 /* Authentication Status for Port 4. 0:Unauthorized, 1:Authorized */
#define Dot1XPB_P4En                        (1<<13)                 /* Enabled/Disable Port Based Access Control for Port 4. 0:Disable, 1:Enable */
#define Dot1XPB_P3OPDIR                     (1<<11)                 /* Uni/Bi-directional Control Type for Port 3. 0:BOTH, 1:IN */
#define Dot1XPB_P3Auth                      (1<<10)                 /* Authentication Status for Port 3. 0:Unauthorized, 1:Authorized */
#define Dot1XPB_P3En                        (1<< 9)                 /* Enabled/Disable Port Based Access Control for Port 3. 0:Disable, 1:Enable */
#define Dot1XPB_P2OPDIR                     (1<< 8)                 /* Uni/Bi-directional Control Type for Port 2. 0:BOTH, 1:IN */
#define Dot1XPB_P2Auth                      (1<< 7)                 /* Authentication Status for Port 2. 0:Unauthorized, 1:Authorized */
#define Dot1XPB_P2En                        (1<< 6)                 /* Enabled/Disable Port Based Access Control for Port 2. 0:Disable, 1:Enable */
#define Dot1XPB_P1OPDIR                     (1<< 5)                 /* Uni/Bi-directional Control Type for Port 1. 0:BOTH, 1:IN */
#define Dot1XPB_P1Auth                      (1<< 4)                 /* Authentication Status for Port 1. 0:Unauthorized, 1:Authorized */
#define Dot1XPB_P1En                        (1<< 3)                 /* Enabled/Disable Port Based Access Control for Port 1. 0:Disable, 1:Enable */
#define Dot1XPB_P0OPDIR                     (1<< 2)                 /* Uni/Bi-directional Control Type for Port 0. 0:BOTH, 1:IN */
#define Dot1XPB_P0Auth                      (1<< 1)                 /* Authentication Status for Port 0. 0:Unauthorized, 1:Authorized */
#define Dot1XPB_P0En                        (1<< 0)                 /* Enabled/Disable Port Based Access Control for Port 0. 0:Disable, 1:Enable */

#define Dot1XPB_POPDIR_offset(idx)	(((idx)*3) + 2)
#define Dot1XPB_PAuth_offset(idx)	(((idx)*3) + 1)
#define Dot1XPB_PEn_offset(idx)		((idx)*3)
#define Dot1XPB_POPDIR(idx, dir)		((dir) << Dot1XPB_POPDIR_offset(idx))
#define Dot1XPB_PAuth(idx, auth)		((auth) << Dot1XPB_PAuth_offset(idx))
#define Dot1XPB_PEn(idx, en)			((en) << Dot1XPB_PEn_offset(idx))

#define EN_8021X_PORT_CONTROL               Dot1XPB_P0En            /* Enable 1X Port Base Control*/
#define EN_8021X_PORT_AUTH                  Dot1XPB_P0Auth          /* port authorized*/
#define EN_8021X_PORT_DIR_IN                Dot1XPB_P0OPDIR         /* Only filter In direction*/
#define EN_DOT1XPB_P0                       Dot1XPB_P0En            /* Alias Name */
#define EN_DOT1XPB_P0_AUTH                  Dot1XPB_P0Auth          /* Alias Name */
#define EN_DOT1XPB_P0_OPINDIR               Dot1XPB_P0OPDIR         /* Alias Name */
#define EN_DOT1XPB_P1                       Dot1XPB_P1En            /* Alias Name */
#define EN_DOT1XPB_P1_AUTH                  Dot1XPB_P1Auth          /* Alias Name */
#define EN_DOT1XPB_P1_OPINDIR               Dot1XPB_P1OPDIR         /* Alias Name */
#define EN_DOT1XPB_P2                       Dot1XPB_P2En            /* Alias Name */
#define EN_DOT1XPB_P2_AUTH                  Dot1XPB_P2Auth          /* Alias Name */
#define EN_DOT1XPB_P2_OPINDIR               Dot1XPB_P2OPDIR         /* Alias Name */
#define EN_DOT1XPB_P3                       Dot1XPB_P3En            /* Alias Name */
#define EN_DOT1XPB_P3_AUTH                  Dot1XPB_P3Auth          /* Alias Name */
#define EN_DOT1XPB_P3_OPINDIR               Dot1XPB_P3OPDIR         /* Alias Name */
#define EN_DOT1XPB_P4                       Dot1XPB_P4En            /* Alias Name */
#define EN_DOT1XPB_P4_AUTH                  Dot1XPB_P4Auth          /* Alias Name */
#define EN_DOT1XPB_P4_OPINDIR               Dot1XPB_P4OPDIR         /* Alias Name */
#define EN_DOT1XPB_P5                       Dot1XPB_P5En            /* Alias Name */
#define EN_DOT1XPB_P5_AUTH                  Dot1XPB_P5Auth          /* Alias Name */
#define EN_DOT1XPB_P5_OPINDIR               Dot1XPB_P5OPDIR         /* Alias Name */
#define EN_DOT1XPB_P6                       Dot1XPB_P6En            /* Alias Name */
#define EN_DOT1XPB_P6_AUTH                  Dot1XPB_P6Auth          /* Alias Name */
#define EN_DOT1XPB_P6_OPINDIR               Dot1XPB_P6OPDIR         /* Alias Name */
#define EN_DOT1XPB_P7                       Dot1XPB_P7En            /* Alias Name */
#define EN_DOT1XPB_P7_AUTH                  Dot1XPB_P7Auth          /* Alias Name */
#define EN_DOT1XPB_P7_OPINDIR               Dot1XPB_P7OPDIR         /* Alias Name */
#define EN_DOT1XPB_P8                       Dot1XPB_P8En            /* Alias Name */
#define EN_DOT1XPB_P8_AUTH                  Dot1XPB_P8Auth          /* Alias Name */
#define EN_DOT1XPB_P8_OPINDIR               Dot1XPB_P8OPDIR         /* Alias Name */

/* DOT1XMACCR - 802.1X mac base control register*/
#define Dot1xMAC_OPDIR                      (1<<10)                 /* OperCOnntrolledDirections for MAC-Based ACCESS Control. 0:BOTH, 1:IN */
#define Dot1xMAC_OPDIR_IN			Dot1xMAC_OPDIR 
#define Dot1xMAC_OPDIR_BOTH		0 
#define Dot1xMAC_P8En                       (1<< 8)                 /* Enable/Disable MAC Address Based Control on Port 8. 0:disable, 1:enable */
#define Dot1xMAC_P7En                       (1<< 7)                 /* Enable/Disable MAC Address Based Control on Port 7. 0:disable, 1:enable */
#define Dot1xMAC_P6En                       (1<< 6)                 /* Enable/Disable MAC Address Based Control on Port 6. 0:disable, 1:enable */
#define Dot1xMAC_P5En                       (1<< 5)                 /* Enable/Disable MAC Address Based Control on Port 5. 0:disable, 1:enable */
#define Dot1xMAC_P4En                       (1<< 4)                 /* Enable/Disable MAC Address Based Control on Port 4. 0:disable, 1:enable */
#define Dot1xMAC_P3En                       (1<< 3)                 /* Enable/Disable MAC Address Based Control on Port 3. 0:disable, 1:enable */
#define Dot1xMAC_P2En                       (1<< 2)                 /* Enable/Disable MAC Address Based Control on Port 2. 0:disable, 1:enable */
#define Dot1xMAC_P1En                       (1<< 1)                 /* Enable/Disable MAC Address Based Control on Port 1. 0:disable, 1:enable */
#define Dot1xMAC_P0En                       (1<< 0)                 /* Enable/Disable MAC Address Based Control on Port 0. 0:disable, 1:enable */
#define EN_DOT1XMAC_OPDIR_IN                Dot1xMAC_OPDIR          /* Alias Name */  
#define EN_8021X_ALLPORT_MAC_CONTROL		0x01FF
#define EN_8021X_MAC_CONTROL                (1)                     /* Flag to check if port n is enabled */
#define EN_8021X_PORT0_MAC_CONTROL          Dot1xMAC_P0En           /* Alias Name */
#define EN_8021X_PORT1_MAC_CONTROL          Dot1xMAC_P1En           /* Alias Name */
#define EN_8021X_PORT2_MAC_CONTROL          Dot1xMAC_P2En           /* Alias Name */
#define EN_8021X_PORT3_MAC_CONTROL          Dot1xMAC_P3En           /* Alias Name */
#define EN_8021X_PORT4_MAC_CONTROL          Dot1xMAC_P4En           /* Alias Name */
#define EN_8021X_PORT5_MAC_CONTROL          Dot1xMAC_P5En           /* Alias Name */
#define EN_8021X_PORT6_MAC_CONTROL          Dot1xMAC_P6En           /* Alias Name */
#define EN_8021X_PORT7_MAC_CONTROL          Dot1xMAC_P7En           /* Alias Name */
#define EN_8021X_PORT8_MAC_CONTROL          Dot1xMAC_P8En           /* Alias Name */

/* GVGCR - Guest VLAN registerr*/
#define GVR2CPU                             (1<<15)                 /* Enable Guest VLAN routing packet to be trap to CPU */
#define GVOPDIR                             (1<<14)                 /* Guest Vlan operation direction control*/
#define Dot1xUNAUTHBH_OFFSET                12                      /* Defaulkt Process of unauthenticated client(/packet) for both port-based and MAC-based Access Control */
#define Dot1XUNAUTHBH_MASK                  (3<<12)                 /* Defaulkt Process of unauthenticated client(/packet) for both port-based and MAC-based Access Control */
#define Dot1xUNAUTHBH_DROP                  (0<<12)                 /* Drop unauthorized frame */
#define Dot1xUNAUTHBH_TOCPU                 (1<<12)                 /* Trap unauthorized frame to CPU */
#define Dot1xUNAUTHBH_GVLAN                 (2<<12)                 /* Enable limited access in Guest VLAN */
#define Dot1xUNAUTHBH_RESERVED              (3<<12)                 /* Reserved */
#define GVID_OFFSET                         0                       /* Guest VLAN VID */
#define GVID_MASK                           (0xfff<<0)              /* Guest VLAN VID */
#define EN_DOT1X_GVR2CPU                    GVR2CPU                 /* Alias Name */
#define EN_GVOPDIR                          GVOPDIR                 /* Alias Name */
#define GVLAN_PROCESS_BITS                  Dot1xUNAUTHBH_OFFSET    /* Alias Name */
#define GVLAN_PROCESS_MASK                  (Dot1XUNAUTHBH_MASK>>Dot1xUNAUTHBH_OFFSET) /* Alias Name */
#define DOT1X_PROCESS_DROP			        (Dot1xUNAUTHBH_DROP>>Dot1xUNAUTHBH_OFFSET) /* Alias Name */
#define DOT1X_PROCESS_TOCPU			        (Dot1xUNAUTHBH_TOCPU>>Dot1xUNAUTHBH_OFFSET) /* Alias Name */
#define DOT1X_PROCESS_GVLAN			        (Dot1xUNAUTHBH_GVLAN>>Dot1xUNAUTHBH_OFFSET) /* Alias Name */
#define DOT1X_PROCESS_RESERVED		        (Dot1xUNAUTHBH_RESERVED>>Dot1xUNAUTHBH_OFFSET) /* Alias Name */

/* Link Aggregation Control Register (IEEE 802.3ad) */
#define LAGCR_BASE                          (SWCORE_BASE + 0x4C00)
#define LAGHPMR0							(0x00 + LAGCR_BASE)	/* Link Aggregation Hashed Index to Port Mapping Register 0 */
#define LAGCR0								(0x08 + LAGCR_BASE)	/* Link Aggregation Control Register 0 */
#define LAGCR1								(0x0C + LAGCR_BASE)	/* Link Aggregation Control Register 1 */

/* output queue stats mask */
#define OUTPUTQUEUE_STAT_MASK_CR0			0xfc000000
#define OUTPUTQUEUE_STAT_MASK_CR1			0x3fffffff

/* LAGHPMR0 - Link Aggregation Hashed Index to Port Mapping Register 0 */
#define LAG_HASHIDX_BITNUM					4

/* LAGCR0 - Link Aggregation Control Register 0 */
#define TRUNKMASK_MASK						(0x1ff)

/* Miscellaneous control registers 
*/

#define MISC_BASE                           (SWCORE_BASE + 0x00007000)
#define LEDCR                               (0x000 + MISC_BASE)     /* LED control */
#define BCR0                                (0x008 + MISC_BASE)     /* Input bandwidth control */
#define BCR1                                (0x00C + MISC_BASE)     /* Ouput bandwidth control */
#define FCREN                               (0x014 + MISC_BASE)     /* Flow control enable control */
#define FCRTH                               (0x018 + MISC_BASE)     /* Flow control threshold */
#define FCPTR                               (0x028 + MISC_BASE)     /* Flow control prime threshold register */
#define PTCR                                (0x01C + MISC_BASE)     /* Port trunk control */
#define SWTECR                              (0x020 + MISC_BASE)     /* Switch table extended control */
#define PTRAPCR                             (0x024 + MISC_BASE)     /* Protocol trapping control */



#define GIDXMCR                             (0x07C + MISC_BASE)     /* GIDX mapping control */
#define PBVR2								(0x0A0 + MISC_BASE)		/* Protocol-Based VLAN Rule 2 -- NetBIOS       */
#define PBVR2E								(0x0A4 + MISC_BASE)		/* Protocol-Based VLAN Rule 2 Extended         */
#define PBVR3								(0x0A8 + MISC_BASE)		/* Protocol-Based VLAN Rule 3 -- PPPoE Control */
#define PBVR3E								(0x0AC + MISC_BASE)		/* Protocol-Based VLAN Rule 3 Extended         */
#define PBVR4								(0x0B0 + MISC_BASE)		/* Protocol-Based VLAN Rule 4 -- PPPoE Session */
#define PBVR4E								(0x0B4 + MISC_BASE)		/* Protocol-Based VLAN Rule 4 Extended         */
#define PBVR5								(0x0B8 + MISC_BASE)		/* Protocol-Based VLAN Rule 5 -- User-defined 1*/
#define PBVR5E								(0x0BC + MISC_BASE)		/* Protocol-Based VLAN Rule 5 Extended         */
#define PBVR6								(0x0C0 + MISC_BASE)		/* Protocol-Based VLAN Rule 6 -- User-defined 2*/
#define PBVR6E								(0x0C4 + MISC_BASE)		/* Protocol-Based VLAN Rule 6 Extended         */

#define PSIR0					(0x0C8 + MISC_BASE)		/* Port State Information Register 0 */
#define PSIR1					(0x0CC + MISC_BASE)		/* Port State Information Register 1 */
#define CHIPID                               (0x100 + MISC_BASE)     /* Chip version ID */

/* LED control register field definitions 
*/
#define LED_P0_SPEED                        (1 << 0)    /* LED port 0 collision */
#define LED_P0_ACT                          (1 << 1)    /* LED port 0 active */
#define LED_P0_COL                          (1 << 2)    /* LED port 0 speed 100M */
#define LED_P1_SPEED                        (1 << 3)    /* LED port 1 collision */
#define LED_P1_ACT                          (1 << 4)    /* LED port 1 active */
#define LED_P1_COL                          (1 << 5)    /* LED port 1 speed 100M */
#define LED_P2_SPEED                        (1 << 6)    /* LED port 2 collision */
#define LED_P2_ACT                          (1 << 7)    /* LED port 2 active */
#define LED_P2_COL                          (1 << 8)    /* LED port 2 speed 100M */
#define LED_P3_SPEED                        (1 << 9)    /* LED port 3 collision */
#define LED_P3_ACT                          (1 << 10)   /* LED port 3 active */
#define LED_P3_COL                          (1 << 11)   /* LED port 3 speed 100M */
#define LED_P4_SPEED                        (1 << 12)   /* LED port 4 collision */
#define LED_P4_ACT                          (1 << 13)   /* LED port 4 active */
#define LED_P4_COL                          (1 << 14)   /* LED port 4 speed 100M */
#define LED_P5_SPEED                        (1 << 15)   /* LED port 5 collision */
#define LED_P5_ACT                          (1 << 16)   /* LED port 5 active */
#define LED_P5_COL                          (1 << 17)   /* LED port 5 speed 100M */
#define EN_LED_CPU_CTRL                     (1 << 18)   /* Enable CPU control LED */
/* Bandwidth control register field definitions 
*/
#define OUT_BC_P0_OFFSET                    0           /* Output bandwidth control port 0 offset */
#define IN_BC_P0_OFFSET                     4           /* Input bandwidth control port 0 offset */
#define OUT_BC_P1_OFFSET                    8           /* Output bandwidth control port 1 offset */
#define IN_BC_P1_OFFSET                     12          /* Input bandwidth control port 1 offset */
#define OUT_BC_P2_OFFSET                    16          /* Output bandwidth control port 2 offset */
#define IN_BC_P2_OFFSET                     20          /* Input bandwidth control port 2 offset */
#define OUT_BC_P3_OFFSET                    24          /* Output bandwidth control port 3 offset */
#define IN_BC_P3_OFFSET                     48          /* Input bandwidth control port 3 offset */
#define OUT_BC_P4_OFFSET                    0           /* Output bandwidth control port 4 offset */
#define IN_BC_P4_OFFSET                     4           /* Input bandwidth control port 4 offset */
#define OUT_BC_P5_OFFSET                    8           /* Output bandwidth control port 5 offset */
#define IN_BC_P5_OFFSET                     12          /* Input bandwidth control port 5 offset */
#define BW_FULL_RATE                        0
#define BW_128K                             1
#define BW_256K                             2
#define BW_512K                             3
#define BW_1M                               4
#define BW_2M                               5
#define BW_4M                               6
#define BW_8M                               7
/* Checksum control register field definitions 
*/
#define ALLOW_L2_CHKSUM_ERR                 (1 << 0)    /* Allow L2 checksum error */
#define ALLOW_L3_CHKSUM_ERR                 (1 << 1)    /* Allow L3 checksum error */
#define ALLOW_L4_CHKSUM_ERR                 (1 << 2)    /* Allow L4 checksum error */
#define EN_ETHER_L3_CHKSUM_REC              (1 << 3)    /* Enable L3 checksum recalculation*/
#define EN_ETHER_L4_CHKSUM_REC              (1 << 4)    /* Enable L4 checksum recalculation*/
/* Flow control enable register field defintions 
*/
#define EN_INQ_FC_CPU                       (1 << 31)   /* Enable Flow Control on CPU Port */
#define EN_INQ_FC_5                         (1 << 30)   /* Enable Flow Control on Port 5 */
#define EN_INQ_FC_4                         (1 << 29)   /* Enable Flow Control on Port 4 */
#define EN_INQ_FC_3                         (1 << 28)   /* Enable Flow Control on Port 3 */
#define EN_INQ_FC_2                         (1 << 27)   /* Enable Flow Control on Port 2 */
#define EN_INQ_FC_1                         (1 << 26)   /* Enable Flow Control on Port 1 */
#define EN_INQ_FC_0                         (1 << 25)   /* Enable Flow Control on Port 0 */
#define EN_OUTQ_FC_CPU                      (1 << 24)   /* Enable Flow Control on CPU Port */
#define EN_OUTQ_FC_5                        (1 << 23)   /* Enable Flow Control on Port 5 */
#define EN_OUTQ_FC_4                        (1 << 22)   /* Enable Flow Control on Port 4 */
#define EN_OUTQ_FC_3                        (1 << 21)   /* Enable Flow Control on Port 3 */
#define EN_OUTQ_FC_2                        (1 << 20)   /* Enable Flow Control on Port 2 */
#define EN_OUTQ_FC_1                        (1 << 19)   /* Enable Flow Control on Port 1 */
#define EN_OUTQ_FC_0                        (1 << 18)   /* Enable Flow Control on Port 0 */
#define CPU_LAUNCH_FC_P5                    (1 << 17)   /* CPU launch flow control of Port 5 */
#define CPU_LAUNCH_FC_P4                    (1 << 16)   /* CPU launch flow control of Port 4 */
#define CPU_LAUNCH_FC_P3                    (1 << 15)   /* CPU launch flow control of Port 3 */
#define CPU_LAUNCH_FC_P2                    (1 << 14)   /* CPU launch flow control of Port 2 */
#define CPU_LAUNCH_FC_P1                    (1 << 13)   /* CPU launch flow control of Port 1 */
#define CPU_LAUNCH_FC_P0                    (1 << 12)   /* CPU launch flow control of Port 0 */
#define EN_MDC_MDIO_FC                      (1 << 10)   /* Enable MDC/MDIO Flow Control */
#define DSC_TH_OFFSET                       0           /* Descriptor Initial threshold */
/* Flow control threshold register field defintions 
*/
#define IN_Q_PER_PORT_BUF_FC_THH_OFFSET     24          /* InQ per port buffer page flow control high threshold offset */
#define IN_Q_PER_PORT_BUF_FC_THL_OFFSET     16          /* InQ per port buffer page flow control low threshold offset */
#define OUT_Q_PER_PORT_BUF_FC_THH_OFFSET    8           /* OutQ per port buffer page flow control high threshold offset */
#define OUT_Q_PER_PORT_BUF_FC_THL_OFFSET    0           /* OutQ per port buffer page flow control low threshold offset */
/* Flow control prime threshold register field defintions 
*/
#define IN_Q_PTH_OFFSET                     16          /* InQ Prime flow control threshold */
#define OUT_Q_PTH_OFFSET                    0           /* OutQ Prime flow control threshold */
/* Port trunking control register field definitions 
*/
#define LMPR7_OFFSET                        27          /* Physical port index for logical port 7 */
#define LMPR6_OFFSET                        24          /* Physical port index for logical port 6 */
#define LMPR5_OFFSET                        21          /* Physical port index for logical port 5 */
#define LMPR4_OFFSET                        18          /* Physical port index for logical port 4 */
#define LMPR3_OFFSET                        15          /* Physical port index for logical port 3 */
#define LMPR2_OFFSET                        12          /* Physical port index for logical port 2 */
#define LMPR1_OFFSET                        9           /* Physical port index for logical port 1 */
#define LMPR0_OFFSET                        6           /* Physical port index for logical port 0 */
#define TRUNK1_PORT_MASK_OFFSET             0           /* Physical port mask of trunk 1 */
/* Port trunking control register field definitions 
*/
#define EN_PPP_OP                           (1 << 31)   /* Enable PPPoE auto insert and remove */
/* Protocol trapping control register field definitions 
*/
#define EN_ARP_TRAP                         (1 << 24)   /* Enable trapping ARP packets */
#define EN_RARP_TRAP                        (1 << 25)   /* Enable trapping RARP packets */
#define EN_PPPOE_TRAP                       (1 << 26)   /* Enable trapping PPPoE packets */
#define EN_IGMP_TRAP                        (1 << 27)   /* Enable trapping IGMP packets */
#define EN_DHCP_TRAP1                       (1 << 28)   /* Enable trapping DHCP 67 packets */
#define EN_DHCP_TRAP2                       (1 << 29)   /* Enable trapping DHCP 68 packets */
#define EN_OSPF_TRAP                        (1 << 30)   /* Enable trapping OSPF packets */
#define EN_RIP_TRAP                         (1 << 31)   /* Enable trapping RIP packets */
/* Spanning tree control register field definitions 
*/
#define EN_ESTP_S_DROP                      (1 << 5)   /* Enable egress spanning tree forward S_Drop */
/* Broadcast storm control register field definitions 
*/
#define EN_BCAST_STORM                      (1 << 0)    /* Enable broadcast storm control */
#define BCAST_TH_MASK                       (0xFF << 1) /* Threshold within broadcast interval mask */
#define BCAST_TH_OFFSET                     1           /* Threshold within broadcast interval offset */
#define TI_100M_MASK                        (0x3F << 9) /* Time interval for 100M mask */
#define TI_100M_OFFSET                      9           /* Time interval for 100M offset */
#define TI_10M_MASK                         (0x3F << 15)/* Time interval for 10M mask */
#define TI_10M_OFFSET                       15          /* Time interval for 10M offset */
#define	EN_L2LRUHASH                    	(1<<4) /*Enable L2 Table hash collision over write the last used entry mode*/
#define	EN_L2FASTAGING                    (1<<5) /*Enable L2 Fast Aging Out*/

/* Table entry aging time control register field definitions 
*/
#define ICMP_TH_OFFSET                      26          /* ICMP timeout threshold offset */
#define ICMP_TH_MASK                        (0x3f << ICMP_TH_OFFSET)
#define UDP_TH_OFFSET                       20          /* UDP timeout threshold offset */
#define UDP_TH_MASK                         (0x3f << UDP_TH_OFFSET)
#define TCP_LONG_TH_OFFSET                  14          /* TCP long timeout threshold offset */
#define TCP_LONG_TH_MASK                    (0x3f << TCP_LONG_TH_OFFSET)
#define TCP_MED_TH_OFFSET                   8           /* TCP medium timeout threshold offset */
#define TCP_MED_TH_MASK                     (0x3f << TCP_MED_TH_OFFSET)
#define TCP_FAST_TH_OFFSET                  2           /* TCP fast timeout threshold offset */
#define TCP_FAST_TH_MASK                    (0x3f << TCP_FAST_TH_OFFSET)
/* Port mirror control register field definitions 
*/
#define MIRROR_TO_PORT_OFFSET               26          /* Port receiving the mirrored traffic offset */
#define MIRROR_FROM_PORT_RX_OFFSET          20          /* Rx port to be mirrored offset */
#define MIRROR_FROM_PORT_TX_OFFSET          14          /* Tx port to be mirrored offset */
/* Per port matching action register field definitions 
*/
#define EN_PMATCH_PORT_LIST_OFFSET          26          /* Enable pattern match port list offset */
#define MATCH_OP_P5_OFFSET                  24          /* Offset of operation if matched on port 5 */
#define MATCH_OP_P4_OFFSET                  22          /* Offset of operation if matched on port 4 */
#define MATCH_OP_P3_OFFSET                  20          /* Offset of operation if matched on port 3 */
#define MATCH_OP_P2_OFFSET                  18          /* Offset of operation if matched on port 2 */
#define MATCH_OP_P1_OFFSET                  16          /* Offset of operation if matched on port 1 */
#define MATCH_OP_P0_OFFSET                  14          /* Offset of operation if matched on port 0 */
#define MATCH_DROP                          0           /* Drop if matched */
#define MATCH_MIRROR_TO_CPU                 1           /* Mirror to CPU if matched */
#define MATCH_FORWARD_TO_CPU                2           /* Forward to CPU if matched */
#define MATCH_TO_MIRROR_PORT                3           /* To mirror port if matched */
/* Port based vlan config register field definitions 
*/
#define PVID_MASK                           7           /* MASK for PVID */
#define VIDP0_OFFSET                        0           /* Vlan table index for port 0 */
#define VIDP1_OFFSET                        3           /* Vlan table index for port 1 */
#define VIDP2_OFFSET                        6           /* Vlan table index for port 2 */
#define VIDP3_OFFSET                        9           /* Vlan table index for port 3 */
#define VIDP4_OFFSET                        12          /* Vlan table index for port 4 */
#define VIDP5_OFFSET                        15          /* Vlan table index for port 5 */
/* GIDX mapping control register field definitions 
*/
#define GIDX_MASK                           7           /* MASK for GIDX */
#define GIDX0_OFFSET                        0           /* Index to IP table */
#define GIDX1_OFFSET                        3           /* Index to IP table */
#define GIDX2_OFFSET                        6           /* Index to IP table */
#define GIDX3_OFFSET                        9           /* Index to IP table */
#define GIDX4_OFFSET                        12          /* Index to IP table */
#define GIDX5_OFFSET                        15          /* Index to IP table */
#define GIDX6_OFFSET                        18          /* Index to IP table */
#define GIDX7_OFFSET                        21          /* Index to IP table */
/* Offset control register field definitions 
*/
#define OCR_START_MASK                      (0x1f << 26)    /* Starting value of offset mask */
#define OCR_START_OFFSET                    26              /* Starting value of offset offset */
#define OCR_END_MASK                        (0x1f << 20)    /* End value of offset mask */
#define OCR_END_OFFSET                      20              /* End value of offset offset */
/* Protocol-based VLAN Control Register field definitions
*/
#define PBVCR_PROTO_TYPE_OFFSET				0			/* Protocol-Based VLAN Control Register Protocol Type */
#define PBVCR_PROTO_TYPE_MASK				(0x3 << PBVCR_PROTO_TYPE_OFFSET)
#define PBVCR_PROTO_VALUE_OFFSET			2			/* Protocol-Based VLAN Control Register Protocol Value */
#define PBVCR_PROTO_VALUE_MASK				(0xffff << PBVCR_PROTO_VALUE_OFFSET)
#define PBVCR_VIDXP5_OFFSET					21			/* VLAN index for Port 5 */
#define PBVCR_VIDXP5_MASK					(0x7 << PBVCR_VIDXP5_OFFSET)
#define PBVCR_VP5_OFFSET					20			/* Valid bit for Port 5 */
#define PBVCR_VP5_MASK						(0x1 << PBVCR_VP5_OFFSET)
#define PBVCR_VIDXP4_OFFSET					17			/* VLAN index for Port 4 */
#define PBVCR_VIDXP4_MASK					(0x7 << PBVCR_VIDXP4_OFFSET)
#define PBVCR_VP4_OFFSET					16			/* Valid bit for Port 4 */
#define PBVCR_VP4_MASK						(0x1 << PBVCR_VP4_OFFSET)
#define PBVCR_VIDXP3_OFFSET					13			/* VLAN index for Port 3 */
#define PBVCR_VIDXP3_MASK					(0x7 << PBVCR_VIDXP3_OFFSET)
#define PBVCR_VP3_OFFSET					12			/* Valid bit for Port 3 */
#define PBVCR_VP3_MASK						(0x1 << PBVCR_VP3_OFFSET)
#define PBVCR_VIDXP2_OFFSET					9			/* VLAN index for Port 2 */
#define PBVCR_VIDXP2_MASK					(0x7 << PBVCR_VIDXP2_OFFSET)
#define PBVCR_VP2_OFFSET					8			/* Valid bit for Port 2 */
#define PBVCR_VP2_MASK						(0x1 << PBVCR_VP2_OFFSET)
#define PBVCR_VIDXP1_OFFSET					5			/* VLAN index for Port 1 */
#define PBVCR_VIDXP1_MASK					(0x7 << PBVCR_VIDXP1_OFFSET)
#define PBVCR_VP1_OFFSET					4			/* Valid bit for Port 1 */
#define PBVCR_VP1_MASK						(0x1 << PBVCR_VP1_OFFSET)
#define PBVCR_VIDXP0_OFFSET					3			/* VLAN index for Port 0 */
#define PBVCR_VIDXP0_MASK					(0x7 << PBVCR_VIDXP0_OFFSET)
#define PBVCR_VP0_OFFSET					0			/* Valid bit for Port 0 */
#define PBVCR_VP0_MASK						(0x1 << PBVCR_VP0_OFFSET)
#define PBVCR_VIDXEP2_OFFSET				9			/* VLAN index for Extented Port 2 */
#define PBVCR_VIDXEP2_MASK					(0x7 << PBVCR_VIDXEP2_OFFSET)
#define PBVCR_VEP2_OFFSET					8			/* Valid bit for Extented Port 2 */
#define PBVCR_VEP2_MASK						(0x1 << PBVCR_VEP2_OFFSET)
#define PBVCR_VIDXEP1_OFFSET				5			/* VLAN index for Extented Port 1 */
#define PBVCR_VIDXEP1_MASK					(0x7 << PBVCR_VIDXEP1_OFFSET)
#define PBVCR_VEP1_OFFSET					4			/* Valid bit for Extented Port 1 */
#define PBVCR_VEP1_MASK						(0x1 << PBVCR_VEP1_OFFSET)
#define PBVCR_VIDXEP0_OFFSET				1			/* VLAN index for Extented Port 0 */
#define PBVCR_VIDXEP0_MASK					(0x7 << PBVCR_VIDXEP0_OFFSET)
#define PBVCR_VEP0_OFFSET					0			/* Valid bit for Extented Port 0 */
#define PBVCR_VEP0_MASK						(0x1 << PBVCR_VEP0_OFFSET)

#if 0 /* Not defined in 865xC spec */
/* PCMCIA registers 
*/
#define PCMCIA_BASE					(SYSTEM_BASE+0x0000)	/* 0xBD010000 */
#define PCMCIA_CFR					(0x000 + PCMCIA_BASE)      /* PCMCIA Command FIFO Register */
#define PCMCIA_IER					(0x004 + PCMCIA_BASE)      /* PCMCIA Interrupt enable register */
#define PCMCIA_FCR					(0x008 + PCMCIA_BASE)      /* PCMCIA FIFO control register */
#define PCMCIA_ISR					(0x00C + PCMCIA_BASE)      /* PCMCIA Interrupt Status register */
#define PCTCR						(0x010 + PCMCIA_BASE)      /* PCMCIA Timing Control Register */

//PCMCIA CFG
#define RD_PCMCIA_ATTR		0x10000000
#define WR_PCMCIA_ATTR		0x00000000
#define RD_PCMCIA_IO			0x14000000
#define WR_PCMCIA_IO			0x04000000

//PCMCIA IER
#define ERDAI					0x80000000
#define ERFEI					(1<<30)
#define ERFFI					(1<<29)
#define ECFDI					(1<<28)
#define ECFEI					(1<<27)
#define ECFFI					(1<<26)

//PCMCIA FCR
#define EFIFO					0x80000000
#define RFRST				(1<<30)
#define TFRST				(1<<29)
#define RTRG_MASK			(0x3<<27)
#define RTRG_01				(0x0<<27)
#define RTRG_12				(0x1<<27)
#define RTRG_16				(0x2<<27)
#define RTRG_26				(0x3<<27)
#define RFRC_MASK			(0x3f<<21)
#define CFRC_MASK			(0x3f<<15)

//PCMCIA LSR
#define DR					0x80000000
#define RxEMPTY				(1<<30)
#define RxqFULL				(1<<29)
#define CMOK					(1<<28)
#define CMEMPTY				(1<<27)
#define CMFULL				(1<<26)
#endif



/* UART registers 
*/
#define UART0_BASE                (SYSTEM_BASE+0x2000) /* 0xB8002000 */
#define UART1_BASE                (SYSTEM_BASE+0x2100)




/* System Control Registers */

/* Global interrupt control registers */
#define GICR_BASE                           (SYSTEM_BASE+0x3000) /* 0xB8003000 */
#define GIMR                                (0x000 + GICR_BASE)       /* Global interrupt mask */
#define GISR                                (0x004 + GICR_BASE)       /* Global interrupt status */
#define IRR                                 (0x008 + GICR_BASE)       /* Interrupt routing */
#define IRR0                                (0x008 + GICR_BASE)       /* Interrupt routing */
#define IRR1                                (0x00C + GICR_BASE)       /* Interrupt routing */
#define IRR2                                (0x010 + GICR_BASE)       /* Interrupt routing */
#define IRR3                                (0x014 + GICR_BASE)       /* Interrupt routing */
/* Global interrupt mask register field definitions 
*/
#define TCIE                                (1 << 29)       /* Timer/Counter interrupt enable */
#define USBIE                               (1 << 30)       /* USB interrupt enable */
#define PCMCIAIE                            (1 << 29)       /* PCMCIA interrupt enable */
#define UART1IE                             (1 << 28)       /* UART 1 interrupt enable */
#define UART0IE                             (1 << 27)       /* UART 0 interrupt enable */
#define PCIIE                               (1 << 26)       /* Parallel port interrupt enable */
#define SWIE                                (1 << 25)       /* Switch core interrupt enable */
#define PABCIE                              (1 << 24)       /* GPIO port ABC interrupt enable */
#define IREQ0IE                             (1 << 23)       /* External interrupt 0 enable */
#define LBCTMOIE                            (1 << 21)       /* LBC time-out interrupt enable */
#ifdef CONFIG_RTL865XB
#define CRYPTOIE                            (1 << 20)       /* Crypto engine interrupt enable */
#define AUTHIE                              (1 << 19)       /* Authentication engine interupt enable */
#define PCMIE                               (1 << 18)       /* PCM interface interrupt enable */
#define PDEIE                               (1 << 17)       /* GPIO port D,E,F,G,H,I interrupt enable */
#define PCIBTMOIE                           (1 << 16)       /* PCI bridge time-out interrupt enable */
#endif /*CONFIG_RTL865XB*/
/* Global interrupt status register field definitions 
*/
#define TCIP                                (1 << 31)       /* Timer/Counter interrupt pending */
#define USBIP                               (1 << 30)       /* USB host interrupt pending */
#define PCMCIAIP                            (1 << 29)       /* PCMCIA interrupt pending */
#define UART1IP                             (1 << 28)       /* UART 1 interrupt pending */
#define UART0IP                             (1 << 27)       /* UART 0 interrupt pending */
#define PCIIP                               (1 << 26)       /* PCI host interrupt pending */
#define SWIP                                (1 << 25)       /* Switch core interrupt pending */
#define PABCIP                              (1 << 24)       /* GPIO port ABC interrupt pending */
#define IREQIP                              (1 << 23)       /* External interrupt 0 pending */
#define LBCTMOIP                            (1 << 21)       /* LBC time-out interrupt pending */
#ifdef CONFIG_RTL865XB
#define CRYPTOIP                            (1 << 20)       /* Crypto engine interrupt pending */
#define AUTHIP                              (1 << 19)       /* Authentication engine interupt pending */
#define PCMIP                               (1 << 18)       /* PCM interface interrupt pending */
#define PDEIP                               (1 << 17)       /* GPIO port D,E,F,G,H,I interrupt pending */
#define PCIBTMOIP                           (1 << 16)       /* PCI bridge time-out interrupt pending */
#endif /*CONFIG_RTL865XB*/
/* Interrupt routing register field definitions 
*/
#define TCIRS_OFFSET                        30            /* Timer/Counter interrupt routing select offset */
#define USBIRS_OFFSET                       28            /* USB interrupt routing select offset */
#define PCMCIAIRS_OFFSET                    26            /* PCI interrupt routing select offset */
#define UART1IRS_OFFSET                     24            /* UART 1 interrupt routing select offset */
#define UART0IRS_OFFSET                     22            /* UART 0 interrupt routing select offset */
#define PCIIRS_OFFSET                       20            /* USB 1 interrupt routing select offset */
#define SWIRS_OFFSET                        18            /* Switch core interrupt routing select offset */
#define PABCIRS_OFFSET                      16            /* GPIO port B interrupt routing select offset */
#define IREQ0RS_OFFSET                      14            /* External interrupt 0 routing select offset */
#define LBCTMOIRS_OFFSET                    10            /* LBC time-out interrupt routing select offset */
#ifdef CONFIG_RTL865XB
#define TCIRS_MSB_OFFSET                    9       /* Timer/Counter interrupt routing select MSB offset */
#define USBIRS_MSB_OFFSET                   8       /* USB interrupt routing select MSB offset */
#define PCMCIAIRS_MSB_OFFSET                7       /* PCI interrupt routing select MSB offset */
#define UART1IRS_MSB_OFFSET                 6       /* UART 1 interrupt routing select MSB offset */
#define UART0IRS_MSB_OFFSET                 5       /* UART 0 interrupt routing select MSB offset */
#define PCIIRS_MSB_OFFSET                   4       /* USB 1 interrupt routing select MSB offset */
#define SWIRS_MSB_OFFSET                    3       /* Switch core interrupt routing select MSB offset */
#define PABCIRS_MSB_OFFSET                  2       /* GPIO port B interrupt routing select MSB offset */
#define IREQ0RS_MSB_OFFSET                  1       /* External interrupt 0 routing select MSB offset */
#define LBCTMOIRS_MSB_OFFSET                0       /* LBC time-out interrupt routing select MSB offset */
#define CRYPTIRS_OFFSET                     29      /* Crypto engine interrupt routing select offset */
#define AUTHIRS_OFFSET                      26      /* Authentication engine interrupt routing select offset */
#define PCMIRS_OFFSET                       23      /* PCM interface interrupt routing select offset */
#define PDEIRS_OFFSET                       20      /* GPIO port D,E,F,G,H,I interrupt routing select MSB offset */
#define PCIBTMOIRS_OFFSET                   17      /* PCI bridge time-out interrupt routing select MSB offset */
#endif /*CONFIG_RTL865XB*/





/* Timer control registers 
*/
#define TIMER_BASE                          (SYSTEM_BASE+0x3100)		/* 0xB8003100 */
#define TC0DATA                             (0x000 + TIMER_BASE)       /* Timer/Counter 0 data */
#define TC1DATA                             (0x004 + TIMER_BASE)       /* Timer/Counter 1 data */
#define TC0CNT                              (0x008 + TIMER_BASE)       /* Timer/Counter 0 count */
#define TC1CNT                              (0x00C + TIMER_BASE)       /* Timer/Counter 1 count */
#define TCCNR                               (0x010 + TIMER_BASE)       /* Timer/Counter control */
#define TCIR                                (0x014 + TIMER_BASE)       /* Timer/Counter intertupt */
#define CDBR                                (0x018 + TIMER_BASE)       /* Clock division base */
#define WDTCNR                              (0x01C + TIMER_BASE)       /* Watchdog timer control */
/* Timer/Counter data register field definitions 
*/
#define TCD_OFFSET                          8
/* Timer/Counter control register field defintions 
*/
#define TC0EN                               (1 << 31)       /* Timer/Counter 0 enable */
#define TC0MODE_COUNTER                     0               /* Timer/Counter 0 counter mode */
#define TC0MODE_TIMER                       (1 << 30)       /* Timer/Counter 0 timer mode */
#define TC1EN                               (1 << 29)       /* Timer/Counter 1 enable */
#define TC1MODE_COUNTER                     0               /* Timer/Counter 1 counter mode */
#define TC1MODE_TIMER                       (1 << 28)       /* Timer/Counter 1 timer mode */
/* Timer/Counter interrupt register field definitions 
*/
#define TC0IE                               ((uint32)1 << 31)       /* Timer/Counter 0 interrupt enable */
#define TC1IE                               (1 << 30)       /* Timer/Counter 1 interrupt enable */
#define TC0IP                               (1 << 29)       /* Timer/Counter 0 interrupt pending */
#define TC1IP                               (1 << 28)       /* Timer/Counter 1 interrupt pending */
/* Clock division base register field definitions 
*/
#define DIVF_OFFSET                         16
/* Watchdog control register field definitions 
*/
#define WDTE_OFFSET                         24              /* Watchdog enable */
#define WDSTOP_PATTERN                      0xA5            /* Watchdog stop pattern */
#define WDTCLR                              (1 << 23)       /* Watchdog timer clear */
#define OVSEL_15                            0               /* Overflow select count 2^15 */
#define OVSEL_16                            (1 << 21)       /* Overflow select count 2^16 */
#define OVSEL_17                            (2 << 21)       /* Overflow select count 2^17 */
#define OVSEL_18                            (3 << 21)       /* Overflow select count 2^18 */
#define WDTIND                              (1 << 20)       /* Indicate whether watchdog ever occurs */
/* RTC control register field definitions */
#define WRITE_CDLW_OK                       (1 << 31)       /* Write CDLW OK */
#define RTCSEL                              (1 << 30)       /* Fase mode */
#define RTCEN                               (1 << 29)       /* Enable */

/* System clock generation */
#define SCCR                                (SYSTEM_BASE+0x3200)	/* 0xB8003200  System Clock Control Register */
#define DPLCR0                             (SCCR+0x04)     /* DPLL Clock Control Register 0 */
#define DPLCR1                             (SCCR+0x08)     /* DPLL Clock Control Register 1 */
#define PCCR                                	(SCCR+0x0C)     /* Peripheral Clock Control Register */

#define SCCR_CPU_OFFSET		4		/* SCCR CPU(Lexra Bus 0) status offset */
#define SCCR_STATUS_MASK		0x07	/* SCCR LexaBus0/LexaBus1/Memory mask */

#define EN_S0DPLL                     	(1 << 31)   /* Enable S0DPLL Tuning */
#define EN_S1DPLL                     	(1 << 30)   /* Enable S1DPLL Tuning */
#define EN_MDPLL                     	(1 << 31)   /* Enable MDPLL Tuning */
#define S0UPDPLL_MASK			0x3F	/* S0_UP_DPLL mask*/
#define MUPDPLL_MASK			0x3F	/* M_UP_DPLL mask*/
#define S0DOWNDPLL_MASK		0x1F	/* S0_DOWN_DPLL mask*/
#define MDOWNDPLL_MASK		0x1F	/* M_DOWN_DPLL mask*/
#define S0UPDPLL_OFFSET			16		/* S0_UP_DPLL status offset*/
#define MUPDPLL_OFFSET			5		/* M_UP_DPLL status offset*/
#define S0DOWNDPLL_OFFSET		11		/* S0_DOWN_DPLL status offset*/
#define MDOWNDPLL_OFFSET		0		/* M_DOWN_DPLL status offset*/

#define S0DOWNDPLL_VALUE		(9 << S0DOWNDPLL_OFFSET)	/* S0_DOWN_DPLL default test value*/
#define S0UPDPLL_VALUE_225MHZ	(17 << S0UPDPLL_OFFSET)		/* S0_UP_DPLL test value 0*/
#define S0UPDPLL_VALUE_250MHZ	(19 << S0UPDPLL_OFFSET)		/* S0_UP_DPLL test value 1*/
#define S0UPDPLL_VALUE_275MHZ	(21 << S0UPDPLL_OFFSET)		/* S0_UP_DPLL test value 2*/
#define S0UPDPLL_VALUE_300MHZ	(23 << S0UPDPLL_OFFSET)		/* S0_UP_DPLL test value 3*/
#define S0UPDPLL_VALUE_325MHZ	(25 << S0UPDPLL_OFFSET)		/* S0_UP_DPLL test value 4*/
#define S0UPDPLL_VALUE_350MHZ	(27 << S0UPDPLL_OFFSET)		/* S0_UP_DPLL test value 5*/
#define S0UPDPLL_VALUE_375MHZ	(29 << S0UPDPLL_OFFSET)		/* S0_UP_DPLL test value 6*/
#define S0UPDPLL_VALUE_400MHZ	(31 << S0UPDPLL_OFFSET)		/* S0_UP_DPLL test value 7*/
#define S0UPDPLL_VALUE_NUM	8							/* S0_UP_DPLL total test values */
#define S0UPDPLL_VALUE_DEFAULT S0UPDPLL_VALUE_275MHZ		/* S0_UP_DPLL default test value */

#define MDOWNDPLL_VALUE		(9 << MDOWNDPLL_OFFSET)		/* M_DOWN_DPLL default test value*/
#define MUPDPLL_VALUE_137MHZ	(10 << MUPDPLL_OFFSET)		/* M_UP_DPLL test value 0*/
#define MUPDPLL_VALUE_150MHZ	(11 << MUPDPLL_OFFSET)		/* M_UP_DPLL test value 1*/
#define MUPDPLL_VALUE_162MHZ	(12 << MUPDPLL_OFFSET)		/* M_UP_DPLL test value 2*/
#define MUPDPLL_VALUE_175MHZ	(13 << MUPDPLL_OFFSET)		/* M_UP_DPLL test value 3*/
#define MUPDPLL_VALUE_187MHZ	(14 << MUPDPLL_OFFSET)		/* M_UP_DPLL test value 4*/
#define MUPDPLL_VALUE_NUM		5							/* M_UP_DPLL total test values */
#define MUPDPLL_VALUE_DEFAULT MUPDPLL_VALUE_137MHZ		/* M_UP_DPLL default test value */

/* GPIO Register Set */
#define GPIO_BASE                           (SYSTEM_BASE+0x3500)	/* 0xB8003500 */

#define PABCD_CNR                           (GPIO_BASE + 0x00)
#define PABCD_PTYPE                         (GPIO_BASE + 0x04)
#define PABCD_DIR                           (GPIO_BASE + 0x08)
#define PABCD_DAT                           (GPIO_BASE + 0x0C)
#define PABCD_ISR                           (GPIO_BASE + 0x10)
#define PAB_IMR                             (GPIO_BASE + 0x14)
#define PCD_IMR                             (GPIO_BASE + 0x18)
#define PEFGH_CNR                           (GPIO_BASE + 0x1C)
#define PEFGH_PTYPE                         (GPIO_BASE + 0x20)
#define PEFGH_DIR                           (GPIO_BASE + 0x24)
#define PEFGH_DAT                           (GPIO_BASE + 0x28)
#define PEFGH_ISR                           (GPIO_BASE + 0x2C)
#define PEF_IMR                             (GPIO_BASE + 0x30)
#define PGH_IMR                             (GPIO_BASE + 0x34)

/*added by Mark*/
/*PCI control regs*/
#define PCI_CTRL_BASE							(SYSTEM_BASE+0x3400)	/* 0xB8003400 */

/* GPIO control registers 
*/
#define GPIO_BASE							(SYSTEM_BASE+0x3500)	/* 0xB8003500 */
#define PABCNR                              (0x00C + GPIO_BASE)     /* Port AB control */
#define PABDIR                              (0x010 + GPIO_BASE)     /* Port AB direction */
#define PABDAT                              (0x014 + GPIO_BASE)     /* Port AB data */
#define PABISR                              (0x018 + GPIO_BASE)     /* Port AB interrupt status */
/* #define PABIMR                              (0x01C + GPIO_BASE)    */ /* Port AB interrupt mask */
/* #define PCIMR                               (0x050 + GPIO_BASE)     */ /* Port C interrupt mask */
/* Port ABC data register field definitions 
*/
#define PDA_OFFSET                          24              /* Port A data offset */
#define PDB_OFFSET                          16              /* Port B data offset */
#define PDC_OFFSET                          8               /* Port C data offset */
/* Port ABC interrupt status register field definitions 
*/
#define PAIP_OFFSET                         24              /* Port A pending status offset */
#define PBIP_OFFSET                         16              /* Port B pending status offset */
#define PCIP_OFFSET                         8               /* Port C pending status offset */



/* Peripheral Lexra timing control registers 
*/
#define PLTC                                (0x064 + GICR_BASE)     /* Peripheral Lexra timeout control */
/* Peripheral Lexra timeout control register field definitions
*/
#define TOEN                              	  (1 << 31)
#define TOLIMIT_2_7                         (0 << 28)
#define TOLIMIT_2_8                         (1 << 28)
#define TOLIMIT_2_9                         (2 << 28)
#define TOLIMIT_2_10                        (3 << 28)
#define TOLIMIT_2_11                        (4 << 28)
#define TOLIMIT_2_12                        (5 << 28)
#define TOLIMIT_2_13                        (6 << 28)
#define TOLIMIT_2_14                        (7 << 28)

/* Memory Control Register registers 
*/
#define MCR_BASE				(SYSTEM_BASE+0x1000)     /* 0xB8001000 */
#define MCR						(0x000 + MCR_BASE)       /* Memory configuration register */
#define MTCR0					(0x004 + MCR_BASE)       /* Memory timing configuration register */
#define MTCR1					(0x008 + MCR_BASE)       /* Memory timing configuration register */
#define PFCR					(0x10 + MCR_BASE)	/* Pre-Fetch Control Register */


#define CRYPTO_BASE			 (SYSTEM_BASE+0xC000)	/* 0xB801C000 */
#define IPSSDAR			(CRYPTO_BASE+0x00)	/* IPSec Source Descriptor Starting Address Register */
#define IPSDDAR			(CRYPTO_BASE+0x04)	/* IPSec Destination Descriptor Starting Address Register */
#define IPSCSR			(CRYPTO_BASE+0x08)	/* IPSec Command/Status Register */
#define IPSCTR			(CRYPTO_BASE+0x0C)	/* IPSec Control Register */

/* IPSec Command/Status Register */
#define IPS_SDUEIP		(1<<15)				/* Source Descriptor Unavailable Error Interrupt Pending */
#define IPS_SDLEIP		(1<<14)				/* Source Descriptor Length Error Interrupt Pending */
#define IPS_DDUEIP		(1<<13)				/* Destination Descriptor Unavailable Error Interrupt Pending */
#define IPS_DDOKIP		(1<<12)				/* Destination Descriptor OK Interrupt Pending */
#define IPS_DABFIP		(1<<11)				/* Data Address Buffer Interrupt Pending */
#define IPS_POLL		(1<<1)				/* Descriptor Polling. Set 1 to kick crypto engine to fetch source descriptor. */
#define IPS_SRST		(1<<0)				/* Software reset, write 1 to reset */

/* IPSec Control Register */
#define IPS_SDUEIE		(1<<15)				/* Source Descriptor Unavailable Error Interrupt Enable */
#define IPS_SDLEIE		(1<<14)				/* Source Descriptor Length Error Interrupt Enable */
#define IPS_DDUEIE		(1<<13)				/* Destination Descriptor Unavailable Error Interrupt Enable */
#define IPS_DDOKIE		(1<<12)				/* Destination Descriptor OK Interrupt Enable */
#define IPS_DABFIE		(1<<11)				/* Data Address Buffer Interrupt Enable */
#define IPS_LBKM		(1<<8)				/* Loopback mode enable */
#define IPS_SAWB		(1<<7)				/* Source Address Write Back */
#define IPS_CKE			(1<<6)				/* Clock enable */
#define IPS_DMBS_MASK	(0x7<<3)			/* Mask for Destination DMA Maximum Burst Size */
#define IPS_DMBS_16		(0x0<<3)			/* 16 Bytes */
#define IPS_DMBS_32		(0x1<<3)			/* 32 Bytes */
#define IPS_DMBS_64		(0x2<<3)			/* 64 Bytes */
#define IPS_DMBS_128	(0x3<<3)			/* 128 Bytes */
#define IPS_SMBS_MASK	(0x7<<0)			/* Mask for SourceDMA Maximum Burst Size */
#define IPS_SMBS_16		(0x0<<0)			/* 16 Bytes */
#define IPS_SMBS_32		(0x1<<0)			/* 32 Bytes */
#define IPS_SMBS_64		(0x2<<0)			/* 64 Bytes */
#define IPS_SMBS_128	(0x3<<0)			/* 128 Bytes */


/* PCM */
#define PCM_BASE		(SYSTEM_BASE+0x8000)	/* 0xB8008000 */
#define PCM_GCR			(PCM_BASE+0x00)		/* PCM Generic Control Register */
#define PCM_SCR			(PCM_BASE+0x04)		/* PCM Specific Control Register */
#define PCM_TSAR		(PCM_BASE+0x08)		/* PCM Time Slot Assignment Register */
#define PCM_BSIZE		(PCM_BASE+0x0C)		/* PCM Buffer Size Register */
#define PCM_CH0TXBSA	(PCM_BASE+0x10)		/* PCM Channel 0 Tx Buffer Starting Address */
#define PCM_CH1TXBSA	(PCM_BASE+0x14)		/* PCM Channel 1 Tx Buffer Starting Address */
#define PCM_CH2TXBSA	(PCM_BASE+0x18)		/* PCM Channel 2 Tx Buffer Starting Address */
#define PCM_CH3TXBSA	(PCM_BASE+0x1C)		/* PCM Channel 3 Tx Buffer Starting Address */
#define PCM_CH0RXBSA	(PCM_BASE+0x20)		/* PCM Channel 0 Rx Buffer Starting Address */
#define PCM_CH1RXBSA	(PCM_BASE+0x24)		/* PCM Channel 1 Rx Buffer Starting Address */
#define PCM_CH2RXBSA	(PCM_BASE+0x28)		/* PCM Channel 2 Rx Buffer Starting Address */
#define PCM_CH3RXBSA	(PCM_BASE+0x2C)		/* PCM Channel 3 Rx Buffer Starting Address */
#define PCM_IMR			(PCM_BASE+0x30)		/* PCM Interrupt Mask Register */
#define PCM_ISR			(PCM_BASE+0x34)		/* PCM Interrupt Status Register */

#define PAGE1_PCM_OWN ( 1 << 1 )
#define PAGE0_PCM_OWN ( 1 << 0 )
#define PCM_PAGE0 0
#define PCM_PAGE1 1
#define PCM_CHANNEL0 0
#define PCM_CHANNEL1 1
#define PCM_CHANNEL2 2
#define PCM_CHANNEL3 3
#define PCM_LINEAR   0
#define PCM_ALAW     1
#define PCM_ULAW     2

/* PCM Generic Control Register 
*/
#define PCM_AE			(1<<12)				/* Interrupt Enable */
#define PCM_CLKDIR		(1<<11)				/* 0:external source from codec, 1:internal PLL */
#define PCM_EXDSE		(1<<10)				/* PCM interface A Extra Data Strobe Enable */
#define PCM_FSINV		(1<< 9)				/* PCM interface frame synchronization polarity invert */
#define PCM_ICC_OFFSET	(0)					/* PCM interface Channel Inter Change control */
#define PCM_ICC_MASK	(0xf<<PCM_ICC_OFFSET)/* PCM interface Channel Inter Change control */

/* PCM Specific Control Register
*/
#define PCM_allCHILBE		(1<<28)				/* all channel internal loop back enable */

#define PCM_CH0CMPE		(1<<27)				/* Channel 0 compander enable */
#define PCM_CH0uA		(1<<26)				/* Channel 0 u-law/A-law selection; 0:u-law, 1:A-law */
#define PCM_CH0TE		(1<<25)				/* Channel 0 transmitter enable */
#define PCM_CH0RE		(1<<24)				/* Channel 0 receiver enable */

#define PCM_CH1CMPE		(1<<19)				/* Channel 1 compander enable */
#define PCM_CH1uA		(1<<18)				/* Channel 1 u-law/A-law selection; 0:u-law, 1:A-law */
#define PCM_CH1TE		(1<<17)				/* Channel 1 transmitter enable */
#define PCM_CH1RE		(1<<16)				/* Channel 1 receiver enable */

#define PCM_CH2CMPE		(1<< 11)				/* Channel 2 compander enable */
#define PCM_CH2uA		(1<< 10)				/* Channel 2 u-law/A-law selection; 0:u-law, 1:A-law */
#define PCM_CH2TE		(1<< 9)				/* Channel 2 transmitter enable */
#define PCM_CH2RE		(1<< 8)				/* Channel 2 receiver enable */

#define PCM_CH3CMPE		(1<< 3)				/* Channel 3 compander enable */
#define PCM_CH3uA		(1<< 2)				/* Channel 3 u-law/A-law selection; 0:u-law, 1:A-law */
#define PCM_CH3TE		(1<< 1)				/* Channel 3 transmitter enable */
#define PCM_CH3RE		(1<< 0)				/* Channel 3 receiver enable */

/* PCM Time Slot Assignment Register
*/
#define PCM_CH0TSA_OFFSET	(24)					/* Channel 0 Time Slot Assignment */
#define PCM_CH0TSA_MASK	(0x1f<<PCM_CH0TSA_OFFSET)	/* Channel 0 Time Slot Assignment */
#define PCM_CH1TSA_OFFSET	(16)					/* Channel 1 Time Slot Assignment */
#define PCM_CH1TSA_MASK	(0x1f<<PCM_CH1TSA_OFFSET)	/* Channel 1 Time Slot Assignment */
#define PCM_CH2TSA_OFFSET	( 8)					/* Channel 2 Time Slot Assignment */
#define PCM_CH2TSA_MASK	(0x1f<<PCM_CH2TSA_OFFSET)	/* Channel 2 Time Slot Assignment */
#define PCM_CH3TSA_OFFSET	( 0)					/* Channel 3 Time Slot Assignment */
#define PCM_CH3TSA_MASK	(0x1f<<PCM_CH3TSA_OFFSET)	/* Channel 3 Time Slot Assignment */

/* PCM Buffer Size Register
*/
#define PCM_CH0BSIZE_OFFSET	(24)						/* Channel 0 Buffer Size 4(n+1) bytes */
#define PCM_CH0BSIZE_MASK	(0xff<<PCM_CH0BSIZE_OFFSET)	/* Channel 0 Buffer Size 4(n+1) bytes */
#define PCM_CH1BSIZE_OFFSET	(16)						/* Channel 1 Buffer Size 4(n+1) bytes */
#define PCM_CH1BSIZE_MASK	(0xff<<PCM_CH1BSIZE_OFFSET)	/* Channel 1 Buffer Size 4(n+1) bytes */
#define PCM_CH2BSIZE_OFFSET	( 8)						/* Channel 2 Buffer Size 4(n+1) bytes */
#define PCM_CH2BSIZE_MASK	(0xff<<PCM_CH2BSIZE_OFFSET)	/* Channel 2 Buffer Size 4(n+1) bytes */
#define PCM_CH3BSIZE_OFFSET	( 0)						/* Channel 3 Buffer Size 4(n+1) bytes */
#define PCM_CH3BSIZE_MASK	(0xff<<PCM_CH3BSIZE_OFFSET)	/* Channel 3 Buffer Size 4(n+1) bytes */

/* PCM Channel 0 Tx Buffer Starting Address */
/* PCM Channel 1 Tx Buffer Starting Address */
/* PCM Channel 2 Tx Buffer Starting Address */
/* PCM Channel 3 Tx Buffer Starting Address */
/* PCM Channel 0 Rx Buffer Starting Address */
/* PCM Channel 1 Rx Buffer Starting Address */
/* PCM Channel 2 Rx Buffer Starting Address */
/* PCM Channel 3 Rx Buffer Starting Address */
#define PCM_TXBUFPTR_MASK	(0xfffffffc)			/* Physical Tx buffer starting address */
#define PCM_P1OWN			(0x00000002)			/* Page 1 own bit; 1:PCM own, 0:CPU own */
#define PCM_P0OWN			(0x00000001)			/* Page 0 own bit; 1:PCM own, 0:CPU own */

/* PCM Interrupt Mask Register 
*/
#define PCM_CH0P0OKIE		(1<<15)					/* Channel 0 page 0 OK Interrupt Enable */
#define PCM_CH0P1OKIE		(1<<14)					/* Channel 0 page 1 OK Interrupt Enable */
#define PCM_CH0TBUAIE		(1<<13)					/* Channel 0 Transmit Buffer Unavailable Interrupt Enable */
#define PCM_CH0RBUAIE		(1<<12)					/* Channel 0 Receive Buffer Unavailable Interrupt Enable */
#define PCM_CH1P0OKIE		(1<<11)					/* Channel 1 page 0 OK Interrupt Enable */
#define PCM_CH1P1OKIE		(1<<10)					/* Channel 1 page 1 OK Interrupt Enable */
#define PCM_CH1TBUAIE		(1<< 9)					/* Channel 1 Transmit Buffer Unavailable Interrupt Enable */
#define PCM_CH1RBUAIE		(1<< 8)					/* Channel 1 Receive Buffer Unavailable Interrupt Enable */
#define PCM_CH2P0OKIE		(1<< 7)					/* Channel 2 page 0 OK Interrupt Enable */
#define PCM_CH2P1OKIE		(1<< 6)					/* Channel 2 page 1 OK Interrupt Enable */
#define PCM_CH2TBUAIE		(1<< 5)					/* Channel 2 Transmit Buffer Unavailable Interrupt Enable */
#define PCM_CH2RBUAIE		(1<< 4)					/* Channel 2 Receive Buffer Unavailable Interrupt Enable */
#define PCM_CH3P0OKIE		(1<< 3)					/* Channel 3 page 0 OK Interrupt Enable */
#define PCM_CH3P1OKIE		(1<< 2)					/* Channel 3 page 1 OK Interrupt Enable */
#define PCM_CH3TBUAIE		(1<< 1)					/* Channel 3 Transmit Buffer Unavailable Interrupt Enable */
#define PCM_CH3RBUAIE		(1<< 0)					/* Channel 3 Receive Buffer Unavailable Interrupt Enable */

/* PCM Interrupt Status Register 
*/
#define PCM_CH0TXP0IP		(1<<31)					/* Channel 0 Tx Page 0 Interrupt Pending */
#define PCM_CH0TXP1IP		(1<<30)					/* Channel 0 Tx Page 1 Interrupt Pending */
#define PCM_CH0RXP0IP		(1<<29)					/* Channel 0 Rx Page 0 Interrupt Pending */
#define PCM_CH0RXP1IP		(1<<28)					/* Channel 0 Rx Page 1 Interrupt Pending */
#define PCM_CH0TXP0UA		(1<<27)					/* Channel 0 Tx Page 0 Unavailable Interrupt Pending */
#define PCM_CH0TXP1UA		(1<<26)					/* Channel 0 Tx Page 1 Unavailable Interrupt Pending */
#define PCM_CH0RXP0UA		(1<<25)					/* Channel 0 Rx Page 0 Unavailable Interrupt Pending */
#define PCM_CH0RXP1UA		(1<<24)					/* Channel 0 Rx Page 1 Unavailable Interrupt Pending */
#define PCM_CH1TXP0IP		(1<<23)					/* Channel 1 Tx Page 0 Interrupt Pending */
#define PCM_CH1TXP1IP		(1<<22)					/* Channel 1 Tx Page 1 Interrupt Pending */
#define PCM_CH1RXP0IP		(1<<21)					/* Channel 1 Rx Page 0 Interrupt Pending */
#define PCM_CH1RXP1IP		(1<<20)					/* Channel 1 Rx Page 1 Interrupt Pending */
#define PCM_CH1TXP0UA		(1<<19)					/* Channel 1 Tx Page 0 Unavailable Interrupt Pending */
#define PCM_CH1TXP1UA		(1<<18)					/* Channel 1 Tx Page 1 Unavailable Interrupt Pending */
#define PCM_CH1RXP0UA		(1<<17)					/* Channel 1 Rx Page 0 Unavailable Interrupt Pending */
#define PCM_CH1RXP1UA		(1<<16)					/* Channel 1 Rx Page 1 Unavailable Interrupt Pending */
#define PCM_CH2TXP0IP		(1<<15)					/* Channel 2 Tx Page 0 Interrupt Pending */
#define PCM_CH2TXP1IP		(1<<14)					/* Channel 2 Tx Page 1 Interrupt Pending */
#define PCM_CH2RXP0IP		(1<<13)					/* Channel 2 Rx Page 0 Interrupt Pending */
#define PCM_CH2RXP1IP		(1<<12)					/* Channel 2 Rx Page 1 Interrupt Pending */
#define PCM_CH2TXP0UA		(1<<11)					/* Channel 2 Tx Page 0 Unavailable Interrupt Pending */
#define PCM_CH2TXP1UA		(1<<10)					/* Channel 2 Tx Page 1 Unavailable Interrupt Pending */
#define PCM_CH2RXP0UA		(1<< 9)					/* Channel 2 Rx Page 0 Unavailable Interrupt Pending */
#define PCM_CH2RXP1UA		(1<< 8)					/* Channel 2 Rx Page 1 Unavailable Interrupt Pending */
#define PCM_CH3TXP0IP		(1<< 7)					/* Channel 3 Tx Page 0 Interrupt Pending */
#define PCM_CH3TXP1IP		(1<< 6)					/* Channel 3 Tx Page 1 Interrupt Pending */
#define PCM_CH3RXP0IP		(1<< 5)					/* Channel 3 Rx Page 0 Interrupt Pending */
#define PCM_CH3RXP1IP		(1<< 4)					/* Channel 3 Rx Page 1 Interrupt Pending */
#define PCM_CH3TXP0UA		(1<< 3)					/* Channel 3 Tx Page 0 Unavailable Interrupt Pending */
#define PCM_CH3TXP1UA		(1<< 2)					/* Channel 3 Tx Page 1 Unavailable Interrupt Pending */
#define PCM_CH3RXP0UA		(1<< 1)					/* Channel 3 Rx Page 0 Unavailable Interrupt Pending */
#define PCM_CH3RXP1UA		(1<< 0)					/* Channel 3 Rx Page 1 Unavailable Interrupt Pending */


/* Generic DMA */
#define GDMA_BASE   (SYSTEM_BASE+0xA000)	/* 0xB800A000 */
//#define GDMA_BASE   (SYSTEM_BASE+0x0000)	/* 0xB8620000 */
//#define GDMA_BASE   (SYSTEM_BASE+0x0000)	/* 0xB8018000 */
#define GDMACNR		(GDMA_BASE+0x00)	/* Generic DMA Control Register */
#define GDMAIMR		(GDMA_BASE+0x04)	/* Generic DMA Interrupt Mask Register */
#define GDMAISR		(GDMA_BASE+0x08)	/* Generic DMA Interrupt Status Register */
#define GDMAICVL	(GDMA_BASE+0x0C)	/* Generic DMA Initial Checksum Value (Left Part) Register */
#define GDMAICVR	(GDMA_BASE+0x10)	/* Generic DMA Initial Checksum Value (Right Part) Register */
#define GDMASBP0	(GDMA_BASE+0x20)	/* Generic DMA Source Block Pointer 0 Register */
#define GDMASBL0	(GDMA_BASE+0x24)	/* Generic DMA Source Block Length 0 Register */
#define GDMASBP1	(GDMA_BASE+0x28)	/* Generic DMA Source Block Pointer 1 Register */
#define GDMASBL1	(GDMA_BASE+0x2C)	/* Generic DMA Source Block Length 1 Register */
#define GDMASBP2	(GDMA_BASE+0x30)	/* Generic DMA Source Block Pointer 2 Register */
#define GDMASBL2	(GDMA_BASE+0x34)	/* Generic DMA Source Block Length 2 Register */
#define GDMASBP3	(GDMA_BASE+0x38)	/* Generic DMA Source Block Pointer 3 Register */
#define GDMASBL3	(GDMA_BASE+0x3C)	/* Generic DMA Source Block Length 3 Register */
#define GDMASBP4	(GDMA_BASE+0x40)	/* Generic DMA Source Block Pointer 4 Register */
#define GDMASBL4	(GDMA_BASE+0x44)	/* Generic DMA Source Block Length 4 Register */
#define GDMASBP5	(GDMA_BASE+0x48)	/* Generic DMA Source Block Pointer 5 Register */
#define GDMASBL5	(GDMA_BASE+0x4C)	/* Generic DMA Source Block Length 5 Register */
#define GDMASBP6	(GDMA_BASE+0x50)	/* Generic DMA Source Block Pointer 6 Register */
#define GDMASBL6	(GDMA_BASE+0x54)	/* Generic DMA Source Block Length 6 Register */
#define GDMASBP7	(GDMA_BASE+0x58)	/* Generic DMA Source Block Pointer 7 Register */
#define GDMASBL7	(GDMA_BASE+0x5C)	/* Generic DMA Source Block Length 7 Register */
#define GDMADBP0	(GDMA_BASE+0x60)	/* Generic DMA Destination Block Pointer 0 Register */
#define GDMADBL0	(GDMA_BASE+0x64)	/* Generic DMA Destination Block Length 0 Register */
#define GDMADBP1	(GDMA_BASE+0x68)	/* Generic DMA Destination Block Pointer 1 Register */
#define GDMADBL1	(GDMA_BASE+0x6C)	/* Generic DMA Destination Block Length 1 Register */
#define GDMADBP2	(GDMA_BASE+0x70)	/* Generic DMA Destination Block Pointer 2 Register */
#define GDMADBL2	(GDMA_BASE+0x74)	/* Generic DMA Destination Block Length 2 Register */
#define GDMADBP3	(GDMA_BASE+0x78)	/* Generic DMA Destination Block Pointer 3 Register */
#define GDMADBL3	(GDMA_BASE+0x7C)	/* Generic DMA Destination Block Length 3 Register */
#define GDMADBP4	(GDMA_BASE+0x80)	/* Generic DMA Destination Block Pointer 4 Register */
#define GDMADBL4	(GDMA_BASE+0x84)	/* Generic DMA Destination Block Length 4 Register */
#define GDMADBP5	(GDMA_BASE+0x88)	/* Generic DMA Destination Block Pointer 5 Register */
#define GDMADBL5	(GDMA_BASE+0x8C)	/* Generic DMA Destination Block Length 5 Register */
#define GDMADBP6	(GDMA_BASE+0x90)	/* Generic DMA Destination Block Pointer 6 Register */
#define GDMADBL6	(GDMA_BASE+0x94)	/* Generic DMA Destination Block Length 6 Register */
#define GDMADBP7	(GDMA_BASE+0x98)	/* Generic DMA Destination Block Pointer 7 Register */
#define GDMADBL7	(GDMA_BASE+0x9C)	/* Generic DMA Destination Block Length 7 Register */

/* Generic DMA Control Register */
#define GDMA_ENABLE			(1<<31)		/* Enable GDMA */
#define GDMA_POLL			(1<<30)		/* Kick off GDMA */
#define GDMA_FUNCMASK		(0xf<<24)	/* GDMA Function Mask */
#define GDMA_MEMCPY			(0x0<<24)	/* Memory Copy */
#define GDMA_CHKOFF			(0x1<<24)	/* Checksum Offload */
#define GDMA_STCAM			(0x2<<24)	/* Sequential T-CAM */
#define GDMA_MEMSET			(0x3<<24)	/* Memory Set */
#define GDMA_B64ENC			(0x4<<24)	/* Base 64 Encode */
#define GDMA_B64DEC			(0x5<<24)	/* Base 64 Decode */
#define GDMA_QPENC			(0x6<<24)	/* Quoted Printable Encode */
#define GDMA_QPDEC			(0x7<<24)	/* Quoted Printable Decode */
#define GDMA_MIC			(0x8<<24)	/* Wireless MIC */
#define GDMA_MEMXOR			(0x9<<24)	/* Memory XOR */
#define GDMA_MEMCMP			(0xa<<24)	/* Memory Compare */
#define GDMA_BYTESWAP		(0xb<<24)	/* Byte Swap */
#define GDMA_PATTERN		(0xc<<24)	/* Pattern Match */
#define GDMA_SWAPTYPE0		(0<<22)		/* Original:{0,1,2,3} => {1,0,3,2} */
#define GDMA_SWAPTYPE1		(1<<22)		/* Original:{0,1,2,3} => {3,2,1,0} */
#define GDMA_ENTSIZMASK		(3<<20)		/* T-CAM Entry Size Mask */
#define GDMA_ENTSIZ32		(0<<20)		/* T-CAM Entry Size 32 bits */
#define GDMA_ENTSIZ64		(1<<20)		/* T-CAM Entry Size 64 bits */
#define GDMA_ENTSIZ128		(2<<20)		/* T-CAM Entry Size 128 bits */
#define GDMA_ENTSIZ256		(3<<20)		/* T-CAM Entry Size 256 bits */

/* Generic DMA Interrupt Mask Register */
#define GDMA_COMPIE			(1<<31)		/* Completed Interrupt Enable */
#if 0 /* phase out */
#define GDMA_SBUDRIE		(1<<30)		/* Source Block Under-run Interrupt Enable */
#define GDMA_DBUDRIE		(1<<29)		/* destination Block Under-run Interrupt Enable */
#endif
#define GDMA_NEEDCPUIE		(1<<28)		/* Need-CPU Interrupt Enable */

/* Generic DMA Interrupt Status Register */
#define GDMA_COMPIP			(1<<31)		/* Completed Interrupt Status (write 1 to clear) */
#if 0 /* phase out */
#define GDMA_SBUDRIP		(1<<30)		/* Source Block Under-run Interrupt Status (write 1 to clear) */
#define GDMA_DBUDRIP		(1<<29)		/* destination Block Under-run Interrupt Status (write 1 to clear) */
#endif
#define GDMA_NEEDCPUIP		(1<<28)		/* Need-CPU Interrupt Status (write 1 to clear) */

/* Generic DMA Source Block Length n. Register */
#define GDMA_LDB			(1<<31)		/* Last Data Block */
#define GDMA_BLKLENMASK		(0x1fff)	/* Block Length (valid value: from 1 to 8K-1 bytes) */

#endif   /* _ASICREGS_H */

