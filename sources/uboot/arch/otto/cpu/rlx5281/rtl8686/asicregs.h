/*
* ----------------------------------------------------------------
* Copyright c                  Realtek Semiconductor Corporation, 2002  
* All rights reserved.
* 
* $Header: /usr/local/dslrepos/u-boot-2011.12/arch/mips/cpu/rlx5281/rtl8676/asicregs.h,v 1.1.1.1 2012/02/01 07:50:52 yachang Exp $
*
* Abstract: ASIC specific definitions.
*
* ---------------------------------------------------------------
*/

#ifndef _ASICREGS_H
#define _ASICREGS_H




/* Register access macro
*/
#define REG32(reg) (*(volatile uint32 *)(reg))
#define REG8(reg) (*(volatile uint8 *)(reg))


#define cache_flush rlx5281_cache_flush_dcache
/* 
 * --------------------------------
 *      RTL865XC Platform
 * --------------------------------
 */


/*
 *  refer to rtl865xc_asicregs.h of 865x driver of linux
 */

/* Basic features
*/
#if defined(CONFIG_RTL8196C)

#ifdef CONFIG_RTL8196D
#define MAX_PORT_NUMBER           5  /* 3 */ // FPGA
#define ALL_PORT_MASK                 0x1f		//0x7
#else
#define MAX_PORT_NUMBER           5
#define ALL_PORT_MASK                 0x3F
#endif

#else
#define MAX_PORT_NUMBER           6
#define ALL_PORT_MASK                 0x3F
#endif
#define PORT_NUM_MASK               7
#define RTL8651_MII_PORTNUMBER	5

#define RTL8651_MAC_NUMBER				6
#define RTL8651_PORT_NUMBER				RTL8651_MAC_NUMBER
#define RTL8651_L2TBL_ROW					256
#define RTL8651_L2TBL_COLUMN				4


/* Memory mapping of tables 
*/
#define SWTABLE_BASE_OF_ALL_TABLES 0xbb000000
#define TABLE_ENTRY_DISTANCE (8 * sizeof(uint32))
enum {
    TYPE_L2_SWITCH_TABLE = 0,
    TYPE_ARP_TABLE,
    TYPE_L3_ROUTING_TABLE,
    TYPE_MULTICAST_TABLE,
    TYPE_NETINTERFACE_TABLE,
    TYPE_EXT_INT_IP_TABLE,
    TYPE_VLAN_TABLE,
    TYPE_VLAN1_TABLE,
    TYPE_SERVER_PORT_TABLE,
    TYPE_L4_TCP_UDP_TABLE,
    TYPE_L4_ICMP_TABLE,
    TYPE_PPPOE_TABLE,
    TYPE_ACL_RULE_TABLE,
    TYPE_NEXT_HOP_TABLE,
    TYPE_RATE_LIMIT_TABLE,
    TYPE_ALG_TABLE,
};
#define table_access_addr_base(type) (SWTABLE_BASE_OF_ALL_TABLES + 0x10000 * (type))

/* rtl8651_clearAsicAllTable */
#define RTL8651_L2TBL_ROW					256
#define RTL8651_L2TBL_COLUMN				4

// ASIC specification part
#define RTL8651_PPPOE_NUMBER				8
#define RTL8651_ROUTINGTBL_SIZE				8
#define RTL8651_ARPTBL_SIZE					512
#define RTL8651_PPPOETBL_SIZE				8
#define RTL8651_TCPUDPTBL_SIZE				1024
#define RTL8651_TCPUDPTBL_BITS				10
#define RTL8651_ICMPTBL_SIZE				32
#define RTL8651_ICMPTBL_BITS				5
//#ifdef CONFIG_RTL8650BBASIC
#define RTL8651_IPTABLE_SIZE				16
#define RTL8651_SERVERPORTTBL_SIZE			16
#define RTL8651_NXTHOPTBL_SIZE				32
#define RTL8651_RATELIMITTBL_SIZE			32
//#else
//#define RTL8651_IPTABLE_SIZE				8
//#define RTL8651_SERVERPORTTBL_SIZE		8
//#endif /* CONFIG_RTL8650BBASIC */
#define RTL865XC_ALGTBL_SIZE				48

#define RTL8651_ALGTBL_SIZE					RTL865XC_ALGTBL_SIZE
#define RTL8651_MULTICASTTBL_SIZE			128
#define RTL8651_IPMULTICASTTBL_SIZE			128
#define RTL8651_NEXTHOPTBL_SIZE				32
#define RTL8651_RATELIMITTBL_SIZE			32
#define RTL8651_MACTBL_SIZE					1024
#define RTL8651_PROTOTRAPTBL_SIZE			8
#define RTL8651_VLANTBL_SIZE				8
#define RTL8651_ACLTBL_SIZE					128

#define RTL865XC_NETINTERFACE_NUMBER		8
#define RTL865XC_NETIFTBL_SIZE			8

#define RTL865XC_MAC_NETINTERFACE_NUMBER	4
#define RTL865XC_PORT_NUMBER				9

#define RTL8651_ACLTBL_ALL_TO_CPU			127  // This rule is always "To CPU"
#define RTL8651_ACLTBL_DROP_ALL				126 //This rule is always "Drop"
#define RTL8651_ACLTBL_PERMIT_ALL			125	// This rule is always "Permit"


#ifdef CONFIG_RTL8196D
#define RTL865XC_VLAN_NUMBER				16
#else
#define RTL865XC_VLAN_NUMBER				4096
#endif

/*
 * Memory Controll Registers
 */
#define MCR_BASE                            0xB8001000

#define MCR                                 (0x00 + MCR_BASE)    /* Memory Configuration Register */
#define MTCR0                               (0x04 + MCR_BASE)    /* Memory Timing Configuration Register 0 */
#define MTCR1                               (0x08 + MCR_BASE)    /* Memory Timing Configuration Register 1 */

/* UART registers 
*/
#define UART0_BASE                          0xB8002000
#define UART1_BASE                          0xB8002100

/* Global interrupt control registers 
*/
#if 0
#define GICR_BASE                           0xB8003000
#define GIMR                                (0x000 + GICR_BASE)       /* Global interrupt mask */
#define GISR                                (0x004 + GICR_BASE)       /* Global interrupt status */
#define IRR                                 (0x008 + GICR_BASE)       /* Interrupt routing */
#define IRR1                                (0x00C + GICR_BASE)       /* Interrupt routing */
#define IRR2                                (0x010 + GICR_BASE)       /* Interrupt routing */
#define IRR3                                (0x014 + GICR_BASE)       /* Interrupt routing */

/* Global interrupt mask register field definitions 
*/
#define TCIE                                (1 << 8)       /* Timer/Counter interrupt enable */
#define UART1IE                             (1 << 13)      /* UART 1 interrupt enable */
#define UART0IE                             (1 << 12)      /* UART 0 interrupt enable */
#define SWIE                                (1 << 15)      /* Switch core interrupt enable */
#define PABCIE                              (1 << 16)      /* GPIO port ABC interrupt enable */
#define IREQ0IE                             (1 << 22)      /* External interrupt 0 enable */
#define LBCTMOIE                            (1 << 2)       /* LBC time-out interrupt enable */

/* Global interrupt status register field definitions 
*/
#define TCIP                                (1 << 8)       /* Timer/Counter interrupt pending */
#define UART1IP                             (1 << 13)      /* UART 1 interrupt pending */
#define UART0IP                             (1 << 12)      /* UART 0 interrupt pending */
#define SWIP                                (1 << 15)      /* Switch core interrupt pending */
#define PABCIP                              (1 << 16)      /* GPIO port ABC interrupt pending */
#define IREQ0IP                             (1 << 22)      /* External interrupt 0 pending */
#define LBCTMOIP                            (1 << 2)       /* LBC time-out interrupt pending */

/* Interrupt routing register field definitions 
*/
#define TCIRS_OFFSET                        0              /* Timer/Counter interrupt routing select offset */
#define UART1IRS_OFFSET                     20              /* UART 1 interrupt routing select offset */
#define UART0IRS_OFFSET                     16              /* UART 0 interrupt routing select offset */
#define SWIRS_OFFSET                        28              /* Switch core interrupt routing select offset */
#define PABCIRS_OFFSET                      0              /* GPIO port B interrupt routing select offset */
#define IREQ0RS_OFFSET                      24              /* External interrupt 0 routing select offset */
#define LBCTMOIRS_OFFSET                    8              /* LBC time-out interrupt routing select offset */

/* Timer control registers 
*/
#define TC0DATA                             (0x100 + GICR_BASE)       /* Timer/Counter 0 data */
#define TC1DATA                             (0x104 + GICR_BASE)       /* Timer/Counter 1 data */
#define TC0CNT                              (0x108 + GICR_BASE)       /* Timer/Counter 0 count */
#define TC1CNT                              (0x10C + GICR_BASE)       /* Timer/Counter 1 count */
#define TCCNR                               (0x110 + GICR_BASE)       /* Timer/Counter control */
#define TCIR                                (0x114 + GICR_BASE)       /* Timer/Counter intertupt */
#define CDBR                                (0x118 + GICR_BASE)       /* Clock division base */
#define WDTCNR                              (0x11C + GICR_BASE)       /* Watchdog timer control */


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
#define TC0IE                               (1 << 31)       /* Timer/Counter 0 interrupt enable */
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

/* System clock generation
*/
#define SCCR                                (0x200 + GICR_BASE)     /* System Clock Control Register */
#define DPLCR0                              (0x204 + GICR_BASE)     /* DPLL Clock Control Register 0 */
#define DPLCR1                              (0x208 + GICR_BASE)     /* DPLL Clock Control Register 1 */
#define PCCR                                (0x20C + GICR_BASE)     /* Peripheral Clock Control Register */
#endif
/* GPIO control registers 
*/
#define GPIO_BASE                           0xB8003500
#define PABCDCNR                            (0x000 + GPIO_BASE)     /* Port ABCD control */
#define PABCDPTYPE                          (0x004 + GPIO_BASE)     /* Port ABCD type */
#define PABCDDIR                            (0x008 + GPIO_BASE)     /* Port ABCD direction */
#define PABCDDAT                            (0x00C + GPIO_BASE)     /* Port ABCD data */
#define PABCDISR                            (0x010 + GPIO_BASE)     /* Port ABCD interrupt status */
#define PABIMR                              (0x014 + GPIO_BASE)     /* Port AB interrupt mask */
#define PCDIMR                              (0x018 + GPIO_BASE)     /* Port CD interrupt mask */
#define PEFGHCNR                            (0x01C + GPIO_BASE)     /* Port ABCD control */
#define PEFGHPTYPE                          (0x020 + GPIO_BASE)     /* Port ABCD type */
#define PEFGHDIR                            (0x024 + GPIO_BASE)     /* Port ABCD direction */
#define PEFGHDAT                            (0x028 + GPIO_BASE)     /* Port ABCD data */
#define PEFGHISR                            (0x02C + GPIO_BASE)     /* Port ABCD interrupt status */
#define PEFIMR                              (0x030 + GPIO_BASE)     /* Port AB interrupt mask */
#define PGHIMR                              (0x034 + GPIO_BASE)     /* Port CD interrupt mask */

/*
 * ---------------------------------------------------
 */

/* Switch Core
*/
#define REAL_SWCORE_BASE                    0xBB800000
#define REAL_SYSTEM_BASE                    0xB8000000
#define REAL_HSB_BASE                       0xBB806280
#define REAL_HSA_BASE                       0xBB806200
#define REAL_SWTBL_BASE                     0xBB000000

#define SWCORE_BASE                         REAL_SWCORE_BASE
#define SYSTEM_BASE                         REAL_SYSTEM_BASE
#define HSB_BASE                            REAL_HSB_BASE
#define HSA_BASE                            REAL_HSA_BASE


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
#define LPCAP_1000BASE_OFFSET						10
#define ADVCAP_1000BASE_MASK						(0x3 << ADVCAP_1000BASE_OFFSET)		/* advertisement capability mask */
#define LPCAP_1000BASE_MASK							(0x3 << LPCAP_1000BASE_OFFSET)		/* link partner status mask */

#define CPU_IFACE_BASE                      (SYSTEM_BASE+0x10000)	/* 0xB8010000 */
#define CPUICR                              (0x000 + CPU_IFACE_BASE)    /* Interface control */

#define CPURPDCR0                            (0x004 + CPU_IFACE_BASE)    /* Rx pkthdr descriptor control 0 */
#define CPURPDCR1                            (0x008 + CPU_IFACE_BASE)    /* Rx pkthdr descriptor control 1 */
#define CPURPDCR2                            (0x00c + CPU_IFACE_BASE)    /* Rx pkthdr descriptor control 2 */
#define CPURPDCR3                            (0x010 + CPU_IFACE_BASE)    /* Rx pkthdr descriptor control 3 */
#define CPURPDCR4                            (0x014 + CPU_IFACE_BASE)    /* Rx pkthdr descriptor control 4 */
#define CPURPDCR5                            (0x018 + CPU_IFACE_BASE)    /* Rx pkthdr descriptor control 5 */
#define CPURPDCR(idx)			(CPURPDCR0 + (idx << 2))		/* Rx pkthdr descriptor control with index */

#define CPURMDCR0				(0x01c + CPU_IFACE_BASE)    /* Rx mbuf descriptor control */
#define CPUTPDCR0				(0x020 + CPU_IFACE_BASE)    /* Tx pkthdr descriptor control Low */
#define CPUTPDCR1				(0x024 + CPU_IFACE_BASE)    /* Tx pkthdr descriptor control High */
#define CPUTPDCR(idx)			(CPUTPDCR0 + (idx << 2))		/* Tx pkthdr descriptor control with index */

#ifdef CONFIG_RTL8196D
#define CPUTPDCR2				(0x060 + CPU_IFACE_BASE)    /* Tx Pkthdr Descriptor 2 Control Register */
#define CPUTPDCR3				(0x064 + CPU_IFACE_BASE)    /* Tx Pkthdr Descriptor 3 Control Register */
#endif

#define CPUIIMR					(0x028 + CPU_IFACE_BASE)    /* Interrupt mask control */
#define CPUIISR					(0x02c + CPU_IFACE_BASE)    /* Interrupt status control */
#define CPUQDM0					(0x030 + CPU_IFACE_BASE)    /* Queue ID 0 and Descriptor Ring Mapping Register */
#define CPUQDM1					(0x032 + CPU_IFACE_BASE)    /* Queue ID 1 and Descriptor Ring Mapping Register */
#define CPUQDM2					(0x034 + CPU_IFACE_BASE)    /* Queue ID 2 and Descriptor Ring Mapping Register */
#define CPUQDM3					(0x036 + CPU_IFACE_BASE)    /* Queue ID 3 and Descriptor Ring Mapping Register */
#define CPUQDM4					(0x038 + CPU_IFACE_BASE)    /* Queue ID 4 and Descriptor Ring Mapping Register */
#define CPUQDM5					(0x03a + CPU_IFACE_BASE)    /* Queue ID 5 and Descriptor Ring Mapping Register */



/* CPUICR - CPU interface control register field definitions 
*/
#define TXCMD                               (1 << 31)       /* Enable Tx */
#define RXCMD                               (1 << 30)       /* Enable Rx */
#define BUSBURST_32WORDS                    0
#define BUSBURST_64WORDS                    (1 << 28)
#define BUSBURST_128WORDS                   (2 << 28)
#define BUSBURST_256WORDS                   (3 << 28)
#define MBUF_128BYTES                       0
#define MBUF_256BYTES                       (1 << 24)
#define MBUF_512BYTES                       (2 << 24)
#define MBUF_1024BYTES                      (3 << 24)
#define MBUF_2048BYTES                      (4 << 24)
#define TXFD                                (1 << 23)       /* Notify Tx descriptor fetch */
#define SOFTRST                             (1 << 22)       /* Re-initialize all descriptors */
#define STOPTX                              (1 << 21)       /* Stop Tx */
#define SWINTSET                            (1 << 20)       /* Set software interrupt */
#define LBMODE                              (1 << 19)       /* Loopback mode */
#define LB10MHZ                             (1 << 18)       /* LB 10MHz */
#define LB100MHZ                            (1 << 18)       /* LB 100MHz */
#if defined(CONFIG_RTL865XB)||defined(CONFIG_RTL865XC)
#define MITIGATION                          (1 << 17)       /* Mitigation with timer1 */
#define EXCLUDE_CRC                         (1 << 16)       /* Exclude CRC from length */
#define RX_SHIFT_OFFSET                     0
#endif /*CONFIG_RTL865XB*/

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

#define MBUF_DESC_RUNOUT_IP0			(1 << 16)			/* Run out mbuf descriptor 0 interrupt pending */

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
#define DESCDIAG_BASE                       (SWCORECNR + 0x0100)
#define GDSR0                               (DESCDIAG_BASE + 0x000)         /* Global Descriptor Status Register 0 */
#define GDSR1                               (DESCDIAG_BASE + 0x004)         /* Global Descriptor Status Register 1 */
#define GCSR0                               (DESCDIAG_BASE + 0x008)         /* Global Congestion Status Register 0 */
#define GCSR1                               (DESCDIAG_BASE + 0x00c)         /* Global Congestion Status Register 1 */
#define P0_DCR0                             (DESCDIAG_BASE + 0x010)         /* Port 0 Descriptor Counter Register 0 */
#define P0_DCR1                             (DESCDIAG_BASE + 0x014)         /* Port 0 Descriptor Counter Register 1 */
#define P0_DCR2                             (DESCDIAG_BASE + 0x018)         /* Port 0 Descriptor Counter Register 2 */
#define P0_DCR3                             (DESCDIAG_BASE + 0x01c)         /* Port 0 Descriptor Counter Register 3 */
#define P1_DCR0                             (DESCDIAG_BASE + 0x020)         /* Port 1 Descriptor Counter Register 0 */
#define P1_DCR1                             (DESCDIAG_BASE + 0x024)         /* Port 1 Descriptor Counter Register 1 */
#define P1_DCR2                             (DESCDIAG_BASE + 0x028)         /* Port 1 Descriptor Counter Register 2 */
#define P1_DCR3                             (DESCDIAG_BASE + 0x02c)         /* Port 1 Descriptor Counter Register 3 */
#define P2_DCR0                             (DESCDIAG_BASE + 0x030)         /* Port 2 Descriptor Counter Register 0 */
#define P2_DCR1                             (DESCDIAG_BASE + 0x034)         /* Port 2 Descriptor Counter Register 1 */
#define P2_DCR2                             (DESCDIAG_BASE + 0x038)         /* Port 2 Descriptor Counter Register 2 */
#define P2_DCR3                             (DESCDIAG_BASE + 0x03c)         /* Port 2 Descriptor Counter Register 3 */
#define P3_DCR0                             (DESCDIAG_BASE + 0x040)         /* Port 3 Descriptor Counter Register 0 */
#define P3_DCR1                             (DESCDIAG_BASE + 0x044)         /* Port 3 Descriptor Counter Register 1 */
#define P3_DCR2                             (DESCDIAG_BASE + 0x048)         /* Port 3 Descriptor Counter Register 2 */
#define P3_DCR3                             (DESCDIAG_BASE + 0x04c)         /* Port 3 Descriptor Counter Register 3 */
#define P4_DCR0                             (DESCDIAG_BASE + 0x050)         /* Port 4 Descriptor Counter Register 0 */
#define P4_DCR1                             (DESCDIAG_BASE + 0x054)         /* Port 4 Descriptor Counter Register 1 */
#define P4_DCR2                             (DESCDIAG_BASE + 0x058)         /* Port 4 Descriptor Counter Register 2 */
#define P4_DCR3                             (DESCDIAG_BASE + 0x05c)         /* Port 4 Descriptor Counter Register 3 */
#define P5_DCR0                             (DESCDIAG_BASE + 0x060)         /* Port 5 Descriptor Counter Register 0 */
#define P5_DCR1                             (DESCDIAG_BASE + 0x064)         /* Port 5 Descriptor Counter Register 1 */
#define P5_DCR2                             (DESCDIAG_BASE + 0x068)         /* Port 5 Descriptor Counter Register 2 */
#define P5_DCR3                             (DESCDIAG_BASE + 0x06c)         /* Port 5 Descriptor Counter Register 3 */
#define P6_DCR0                             (DESCDIAG_BASE + 0x070)         /* Port CPU Descriptor Counter Register 0 */
#define P6_DCR1                             (DESCDIAG_BASE + 0x074)         /* Port CPU Descriptor Counter Register 1 */
#define P6_DCR2                             (DESCDIAG_BASE + 0x078)         /* Port CPU Descriptor Counter Register 2 */
#define P6_DCR3                             (DESCDIAG_BASE + 0x07c)         /* Port CPU Descriptor Counter Register 3 */

/* GDSR0 - Global Descriptor Status Register 0 */
#define DSCRUNOUT                           (1<<27)                         /* Descriptor Run Out */
#define TotalDscFctrl_Flag                  (1<<26)                         /* TotalDescriptor Flow Control event flag */
#define USEDDSC_OFFSET                      16                              /* Total Used Descriptor */
#define USEDDSC_MASK                        (0x3ff<<16)                     /* Total Used Descriptor */
#define SharedBufFCON_Flag                  (1<<14)                         /* SharedBufFCON threshold triggerred flag */
#define MaxUsedDsc_OFFSET                   0                               /* Max Used Descriptor Count History */
#define MaxUsedDsc_MASK                     (0x3ff<<0)                      /* Max Used Descriptor Count History */


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
#define CSCR                                (0x048+SWMACCR_BASE)     /* Checksum Control Register */
#define CCR                                 (0x048+SWMACCR_BASE)     /* Checksum Control Register */

// CONFIG_RTL8196D
#define _100_PHY_OC                      (0x04C+SWMACCR_BASE)     /* 10/100M PHY operating Control Register */
#define EMB_PHY_ID                        (0x050+SWMACCR_BASE)     /* Embedded 10/100M PHY ID Register */



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
#if defined(CONFIG_RTL8196C)
#define AcceptL2Err                          (1<<3)                   /* CPU port L2 CRC Error Allow; 0: Not Allowed, 1: Allowed (default) */
#endif
#define L4ChkSErrAllow                      (1<<2)                   /* L4 Checksum Error Allow */
#define L3ChkSErrAllow                      (1<<1)                   /* L3 Checksum Error Allow */
#define L2CRCErrAllow                       (1<<0)                   /* L2 CRC Error Allow */

/* Checksum control register field definitions 
*/
#define ALLOW_L2_CHKSUM_ERR                 (1 << 0)    /* Allow L2 checksum error */
#define ALLOW_L3_CHKSUM_ERR                 (1 << 1)    /* Allow L3 checksum error */
#define ALLOW_L4_CHKSUM_ERR                 (1 << 2)    /* Allow L4 checksum error */
#define EN_ETHER_L3_CHKSUM_REC              (1 << 3)    /* Enable L3 checksum recalculation*/
#define EN_ETHER_L4_CHKSUM_REC              (1 << 4)    /* Enable L4 checksum recalculation*/


/* EPOCR, EPIDR - Embedded PHY Operation Control Register */
#define Port_embPhyID_MASK(port)			( 0x1f << ( port * 5 ) )			/* Embedded PHY ID MASK of port 'idx' */
#define Port_embPhyID(id, port)				( id << ( port * 5 ) )			/* Embedded PHY ID setting of port 'idx' */

#define PCRAM_BASE                          (0x4100+SWCORE_BASE)
#define PITCR                               (0x000+PCRAM_BASE)       /* Port Interface Type Control Register */
#define PCRP0                               (0x004+PCRAM_BASE)       /* Port Configuration Register of Port 0 */
#define PCRP1                               (0x008+PCRAM_BASE)       /* Port Configuration Register of Port 1 */
#define PCRP2                               (0x00C+PCRAM_BASE)       /* Port Configuration Register of Port 2 */
#define PCRP3                               (0x010+PCRAM_BASE)       /* Port Configuration Register of Port 3 */
#define PCRP4                               (0x014+PCRAM_BASE)       /* Port Configuration Register of Port 4 */
#define PCRP5                               (0x018+PCRAM_BASE)       /* Port Configuration Register of Port 5 */
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

#if defined(CONFIG_RTL8196C)
/* PCRP0,PCRP1,PCRP2,PCRP3,PCRP4,PCRP5,PCRP6,PCRP7,PCRP8 - Port Configuration Register */
#define BYPASS_TCRC                         (1 << 31)   /* Not recalculate CRC error */
#define ExtPHYID_OFFSET                     (26)        /* External PHY ID */
#define ExtPHYID_MASK                       (0x1f<<26)  /* External PHY ID */
#define EnForceMode                         (1<<25)     /* Enable Force Mode to set link/speed/duplix/flow status */

	/* 	EnForceMode=0,  PollLinkStatus: do not care
		EnForceMode=1, PollLinkStatus=0 : ForceMode , disable Auto-Negotiation.  It's noted that The mode should be setting for MAC-to-MAC connection.
					    PollLinkStatus=1:  ForceMode with polling link status, disable Auto-Negotiation but polling phy's link status.
	 */
#define PollLinkStatus                           (1<<24)
#define ForceLink                           (1<<23)     /* 0-link down, 1-link up */

	/*	FrcAbi_AnAbi_sel[4:0]: bit 22 ~ 18
		If EnForceMode = 1, FrcAbi_AnAbi_sel is used to indicate the force mode operation. (For either MAC mode or PHY mode operation.)
		FrcAbi_AnAbi_sel[0] = ForceDuplex1: force FULL duplex ,  0: force HALF duplex 
		FrcAbi_AnAbi_sel[2:1] = ForceSpeed00: force 10Mbps   01: force 100Mbps  ( default setting for port#5 NFBI-PHY-mode spec. )
			10: force 1000Mbps 11: reserved.
		FrcAbi_AnAbi_sel[4:3] = Reserved, useless.
		If EnForceMode = 0, FrcAbi_AnAbi_sel is used to indicate Auto-Negotiation advertise ability.
		FrcAbi_AnAbi_sel[0]: 10Mbps Half-duplexFrcAbi_AnAbi_sel[1]: 10Mbps Full-duplexFrcAbi_AnAbi_sel[2]: 100Mbps Half-duplex
		FrcAbi_AnAbi_sel[3]: 100Mbps Full-duplexFrcAbi_AnAbi_sel[4]: 1000Mbps Full-duplex	
	 */
/* 8196c or 8198 has no bit 20, 21 definition */
#define IPMSTP_PortST_MASK				(3<<21)		/* Mask of IP Multicast Spanning Tree Protocol Port State Control */
#define IPMSTP_PortST_OFFSET				(21)			/* Offset of IP Multicast Spanning Tree Protocol Port State Control */
#define IPMSTP_PortST_DISABLE			(0<<21)		/* Disable State of IP Multicast Spanning Tree Protocol Port State Control */
#define IPMSTP_PortST_BLOCKING			(1<<21)		/* Blocking State of IP Multicast Spanning Tree Protocol Port State Control */
#define IPMSTP_PortST_LISTENING		(1<<21)		/* Listening State of IP Multicast Spanning Tree Protocol Port State Control */
#define IPMSTP_PortST_LEARNING			(2<<21)		/* Learning State of IP Multicast Spanning Tree Protocol Port State Control */
#define IPMSTP_PortST_FORWARDING		(3<<21)		/* Forwarding State of IP Multicast Spanning Tree Protocol Port State Control */

#define ForceSpeedMask                      (3<<19)     /* Force speed Mask */
#define ForceSpeed10M                       (0<<19)     /* Force speed 10M */
#define ForceSpeed100M                      (1<<19)     /* Force speed 100M */
#define ForceSpeed1000M                     (2<<19)     /* Force speed 1G */
#define ForceDuplex                         (1<<18)     /* Force Duplex */
#define AutoNegoSts_OFFSET                     (18)
#define AutoNegoSts_MASK                       (0x1f<<18) 
	/* bit 16, 17
		if EnForceMode = 1, PAUSE flow control ability Control .
			[0]= enable TX pause ability
			[1]= enable RX pause ability
		if EnForceMode =0, PHY advertise pause operation ability :
		  	[0]= PAUSE operation for full duplex links
		  	[1]= Asymmetric PAUSE operation for full duplex links
	 */
#define PauseFlowControl_MASK               (3<<16)      /* Mask for per-port 802.3 PAUSE flow control ability control */
#define PauseFlowControlNway                (0<<16)      /* Follow the PHY N-way result */
#define PauseFlowControlEtxDrx              (1<<16)      /* force: enable TX, disable RX */
#define PauseFlowControlDtxErx              (2<<16)      /* force: disable TX, enable RX */
#define PauseFlowControlEtxErx              (3<<16)      /* force: enable TX, enable RX */

#define MIIcfg_CRS                          (1<<15)     /* MII interface Parameter setup */
#define MIIcfg_COL                          (1<<14)     /* MII interface Parameter setup */
#define MIIcfg_RXER                         (1<<13)     /* MII interface Parameter setup */
#define GMIIcfg_CRS                         (1<<12)     /* GMII interface Parameter setup */
//#define BCSC_ENUNKNOWNUNICAST               (1<<11)     /* Enable control for unknown unicast packet */
#define BCSC_Types_MASK                     (7<<9)     /* Broadcast Storm Control packet type selection */
#define BCSC_ENMULTICAST                    (1<<10)     /* Enable control for multicast packet */
#define BCSC_ENBROADCAST                    (1<<9)     /* Enable control for broadcast packet */
#define ENBCSC								(1<<8)		/* Enable broadcast storm control */
#define EnLoopBack                          (1<<7)     /* Enable MAC-PHY interface Mii Loopback */
#define DisBKP                              (1<<6)      /* Per-port Disable Backpressure function for Half duplex */
#define STP_PortST_MASK					(3<<4)		/* Mask Spanning Tree Protocol Port State Control */
#define STP_PortST_OFFSET					(4)			/* Offset */
#define STP_PortST_DISABLE				(0<<4)		/* Disable State */
#define STP_PortST_BLOCKING				(1<<4)		/* Blocking State */
#define STP_PortST_LISTENING				(1<<4)		/* Listening State */
#define STP_PortST_LEARNING				(2<<4)		/* Learning State */
#define STP_PortST_FORWARDING			(3<<4)		/* Forwarding State */
#define MacSwReset					(1<<3)		/* 0: reset state, 1: normal state */
#define AcptMaxLen_OFFSET                   (1)         /* Enable the max acceptable packet length supported */
#define AcptMaxLen_MASK                     (3<<1)      /* Enable the max acceptable packet length supported */
#define AcptMaxLen_1536                     (0<<1)      /* 1536 Bytes (RTL865xB) */
#define AcptMaxLen_1552                     (1<<1)      /* 1552 Bytes (RTL865xB) */
#define AcptMaxLen_9K                       (2<<1)      /* 9K(9216) Bytes */
#define AcptMaxLen_16K                      (3<<1)      /* 16370 bytes = 16K-2(cutoff)-4(vlan)-8(pppoe) */
#define EnablePHYIf                         (1<<0)      /* Enable PHY interface. */
#define TXC_PHASE                         (1<<4)      /* 1=Delay TXC 2ns compared to TXD*/
#else
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
#define STP_PortST_MASK                     (3<<5)      /* Mask Spanning Tree Protocol Port State Control */
#define STP_PortST_OFFSET                   (5)         /* Offset */
#define STP_PortST_DISABLE                  (0<<5)      /* Disable State */
#define STP_PortST_BLOCKING                 (1<<5)      /* Blocking State */
#define STP_PortST_LISTENING                (1<<5)      /* Listening State */
#define STP_PortST_LEARNING                 (2<<5)      /* Learning State */
#define STP_PortST_FORWARDING               (3<<5)      /* Forwarding State */
#define AcptMaxLen_OFFSET                   (1)         /* Enable the max acceptable packet length supported */
#define AcptMaxLen_MASK                     (3<<1)      /* Enable the max acceptable packet length supported */
#define AcptMaxLen_1536                     (0<<1)      /* 1536 Bytes (RTL865xB) */
#define AcptMaxLen_1552                     (1<<1)      /* 1552 Bytes (RTL865xB) */
#define AcptMaxLen_9K                       (2<<1)      /* 9K(9216) Bytes */
#define AcptMaxLen_16K                      (3<<1)      /* 16370 bytes = 16K-2(cutoff)-4(vlan)-8(pppoe) */
#define EnablePHYIf                         (1<<0)      /* Enable PHY interface. */
#endif

/* PSRP0,PSRP1,PSRP2,PSRP3,PSRP4,PSRP5,PSRP6,PSRP7,PSRP8 - Port Status Register Port 0~8 */
#define LinkDownEventFlag                   (1<<8)      /* Port Link Down Event detecting monitor flag */
#define PortStatusNWayEnable                (1<<7)      /* N-Way Enable */
#define PortStatusRXPAUSE                   (1<<6)      /* Rx PAUSE */
#define PortStatusTXPAUSE                   (1<<5)      /* Tx PAUSE */
#define PortStatusLinkUp                    (1<<4)      /* Link Up */
#define PortStatusDuplex                    (1<<3)      /* Duplex */
#define PortStatusLinkSpeed_MASK            (3<<0)      /* Link Speed */
#define PortStatusLinkSpeed10M              (0<<0)      /* 10M */
#define PortStatusLinkSpeed100M             (1<<0)      /* 100M */
#define PortStatusLinkSpeed1000M            (2<<0)      /* 1000M */

/* P0GMIICR Port-0 / Port-5 GMII Configuration Register */
#define CFG_GMAC_MASK                       (3<<23)                  /* The register default reflect the HW power on strapping value of H/W pin. */
#define CFG_GMAC_RGMII                      (0<<23)                  /* RGMII mode */
#define CFG_GMAC_GMII_MII_MAC               (1<<23)                  /* GMII/MII MAC mode */
#define CFG_GMAC_GMII_MII_PHY               (2<<23)                  /* GMII/MII PHY mode */
#define CFG_GMAC_Reserved                   (3<<23)                  /* Reserved */
#if 0
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
#else
#define RGMII_RCOMP_MASK					(7<<0)					/* RGMII Input Timing compensation control */
#define RGMII_RCOMP_0NS						(0<<0)					/* Rcomp 0.0 ns */
#define RGMII_RCOMP_0DOT5NS					(1<<0)					/* Rcomp 0.5 ns */
#define RGMII_RCOMP_1NS						(2<<0)					/* Rcomp 1.0 ns */
#define RGMII_RCOMP_1DOT5NS					(3<<0)					/* Rcomp 1.5 ns */
#define RGMII_RCOMP_2NS						(4<<0)					/* Rcomp 2.0 ns */
#define RGMII_RCOMP_2DOT5NS					(5<<0)					/* Rcomp 2.5 ns */
#define RGMII_RCOMP_3NS						(6<<0)					/* Rcomp 3.0 ns */
#define RGMII_RCOMP_3DOT5NS					(7<<0)					/* Rcomp 3.5 ns */
#define RGMII_TCOMP_MASK					(1<<4)					/* RGMII Output Timing compensation control */
#define RGMII_TCOMP_0NS						(0<<4)					/* Tcomp 0.0 ns */
#define RGMII_TCOMP_2NS						(1<<4)					/* Tcomp 2.0 ns */

#define Conf_done							(1<<6)					//configuration is done to enable the frame reception and transmission
#endif

#define SWMISC_BASE                         (0x4200+SWCORE_BASE)
/* Chip Version ID Register */
#define CVIDR                               (0x00+SWMISC_BASE)     /* Chip Version ID Register */
#define SIRR                                (0x04+SWMISC_BASE)     /* System Initial and Reset Registe*/
#define BISTCR                              (0x08+SWMISC_BASE)     /* BIST control */

/* SIRR - System Initial and Reset Register */
#define FULL_RST                            (1 << 2)   /* Reset all tables & queues */
#define SEMI_RST                            (1 << 1)   /* Reset queues */
#define TRXRDY                              (1 << 0)    /* Start normal TX and RX */

/* BISTCR - BIST control register field definitions */
#define BISTing                             (1<<31)     /* 0 for finished, 1 for BIST is on going */
#define DisBIST                             (1<<30)     /* 0 Enable BIST, 1 for disable BIST */
#define BIST_READY_PATTERN                  0x018F0000
#define BIST_TRXRDY_PATTERN                 0x318f0000
#define BIST_QUEUE_MEMORY_FAIL_PATTERN      0x00700000
#define BIST_PACKET_BUFFER_FAIL_PATTERN     0x0E000000


#define ALE_BASE                            (0x4400+SWCORE_BASE)
#define TEACR  	      			            (0x00+ALE_BASE)       /* Table Entry Aging Control Register */
#define TEATCR                              (0x04+ALE_BASE)       /* Table entry aging time control */
#define RMACR                               (0x08+ALE_BASE)       /* Reserved Multicast Address Address Mapping */
#define ALECR                               (0x0C+ALE_BASE)       /* ALE Control Register */
#define MSCR                                (0x10+ALE_BASE)       /* Module Switch Control Register */
#define TTLCR                               (0x0C+ALE_BASE)       /* TTL control */
#define L4TOCR                              (0x14+ALE_BASE)       /* L4 Table Offset control */
#define SWTCR0                              (0x18+ALE_BASE)       /* swtich table control register 0 */
#define SWTCR1                              (0x1C+ALE_BASE)       /* swtich table control register 1 */
#define PLITIMR                             (0x20+ALE_BASE)       /* Port to LAN Interface Table Index Mapping Register */
#define DACLRCR                             (0x24+ALE_BASE)       /* swtich table control register*/
#define FFCR                                (0x28+ALE_BASE)       /*Frame Forwarding Configuratoin Register */
#define MGFCR_E0R0                          (0x2C+ALE_BASE)       /*L2 MAC Group Forwarding Control Register Entry_0 R0 */
#define MGFCR_E0R1                          (0x30+ALE_BASE)       /*L2 MAC Group Forwarding Control Register Entry_1 R1 */
#define MGFCR_E0R2                          (0x34+ALE_BASE)       /*L2 MAC Group Forwarding Control Register Entry_2 R2 */
#define MGFCR_E1R0                          (0x38+ALE_BASE)       /*L2 MAC Group Forwarding Control Register Entry_0 R0 */
#define MGFCR_E1R1                          (0x3C+ALE_BASE)       /*L2 MAC Group Forwarding Control Register Entry_1 R1 */
#define MGFCR_E1R2                          (0x40+ALE_BASE)       /*L2 MAC Group Forwarding Control Register Entry_2 R2 */
#define MGFCR_E2R0                          (0x44+ALE_BASE)       /*L2 MAC Group Forwarding Control Register Entry_0 R0 */
#define MGFCR_E2R1                          (0x48+ALE_BASE)       /*L2 MAC Group Forwarding Control Register Entry_1 R1 */
#define MGFCR_E2R2                          (0x4C+ALE_BASE)       /*L2 MAC Group Forwarding Control Register Entry_2 R2 */
#define MGFCR_E3R0                          (0x50+ALE_BASE)       /*L2 MAC Group Forwarding Control Register Entry_0 R0 */
#define MGFCR_E3R1                          (0x54+ALE_BASE)       /*L2 MAC Group Forwarding Control Register Entry_1 R1 */
#define MGFCR_E3R2                          (0x58+ALE_BASE)       /*L2 MAC Group Forwarding Control Register Entry_2 R2 */
#define OCR                                 L4TOCR /* Alias Name */


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
#define EN_PPPOE                            (1<<18)               /* Enable PPPoE auto-encapsulation and auto-decapsulation */
#define TTL_1Enable                         (1<<16)               /* Enable TTL-1 operation for L3 routing */
#define Fragment2CPU                        (1<<15)               /* When ACL is enabled, enable all fragmented IP packet to be trapped to CPU (because L4 is needed)
                                                                     When ACL is disabled, if this bit is set, L2 forwarding as before, L3 above operation will trapped to CPU.
                                                                     When ACL is disabled, if this bit is not set, IP fragment packet will be forwarded as normal via L3 routing or NAT. */
#define FRAG2CPU						    Fragment2CPU
#define MultiCastMTU_OFFSET                 (0)
#define MultiCastMTU_MASK                   (0x3fff)
#define MULTICAST_L2_MTU_MASK		        MultiCastMTU_MASK     /* Alias Name */
#define EN_TTL1                             TTL_1Enable           /* Alias Name */


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

/* SBFCR0 */
#define S_DSC_RUNOUT_OFFSET                 (0)                   /* Offset for Descriptor Run Out Threshold */
#define S_DSC_RUNOUT_MASK                   (0x3FF<<0)            /* Mask for Descriptor Run Out Threshold */

/* SBFCR1 */
#define SDC_FCOFF_OFFSET                    (16)                  /* Offset for system flow control turn off threshold */
#define SDC_FCOFF_MASK                      (0x1ff<<16)           /* Mask for system flow control turn off threshold */
#define SDC_FCON_OFFSET                     (0)                   /* Offset for system flow control turn on threshold */
#define SDC_FCON_MASK                       (0x1ff<<0)            /* Mask for system flow control turn on threshold */

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
#define LBPCR                               (0x104 + PSCR)          /* Leaky Bucket Parameter Control Register */
#define LBTTCR                              (0x108 + PSCR)          /* Leaky Bucket Token Threshold Control Register */

/* P0Q0RGCG - Per-Queue Rate Guarantee Control Register */
#define PPR_OFFSET                          (24)                    /* Peak Packet Rate, in times of APR        CNT2 */
#define PPR_MASK                            (7<<24)                 /* Peak Packet Rate, in times of APR        CNT2 */
#define L1_OFFSET                           (16)                    /* Bucket burst size of APR. unit: 1KB      MAX  */
#define L1_MASK                             (0xff<<16)              /* Bucket burst size of APR. unit: 1KB      MAX  */
#define APR_OFFSET                          (0)                     /* Average Packet Rate, in times of 64Kbps  CNT1 */
#define APR_MASK                            (0x3FFF<<0)             /* Average Packet Rate, in times of 64Kbps  CNT1 */


/* LBPCR - Leaky Bucket Parameter Control Register */
#define Token_OFFSET                        (8)                     /* Token used for adding budget in each time slot. */
#define Token_MASK                          (0xff<<8)               /* Token used for adding budget in each time slot. */
#define Tick_OFFSET                         (0)                     /* Tick used for time slot size unit */
#define Tick_MASK                           (0xff<<0)               /* Tick used for time slot size unit */

/* LBTTCR - Leaky Bucket Token Threshold Control Register */
#define L2_OFFSET                           (0)                     /* leaky Bucket Token Hi-threshold register */



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


/*
 * 802.1X Control Register */
#define DOT1X_BASE                          (SWCORE_BASE + 0x4B00)
#define DOT1XPORTCR					        (0x00 +DOT1X_BASE)      /* 802.1X port base control register*/
#define DOT1XMACCR					        (0x04 +DOT1X_BASE)      /* 802.1X mac base control register*/
#define GVGCR						        (0x08 +DOT1X_BASE)      /* Guest VLAN registerr*/


/* Table access and CPU interface control registers
*/
#define TACI_BASE                           (SWCORE_BASE + 0x00004D00)
#define SWTACR                              (0x000 + TACI_BASE)     /* Table Access Control */
#define SWTASR                              (0x004 + TACI_BASE)     /* Table Access Status */
#define SWTAA                               (0x008 + TACI_BASE)     /* Table Access Address */
#define TCR0                                (0x020 + TACI_BASE)     /* Table Access Control 0 */
#define TCR1                                (0x024 + TACI_BASE)     /* Table Access Control 1 */
#define TCR2                                (0x028 + TACI_BASE)     /* Table Access Control 2 */
#define TCR3                                (0x02C + TACI_BASE)     /* Table Access Control 3 */
#define TCR4                                (0x030 + TACI_BASE)     /* Table Access Control 4 */
#define TCR5                                (0x034 + TACI_BASE)     /* Table Access Control 5 */
#define TCR6                                (0x038 + TACI_BASE)     /* Table Access Control 6 */
#define TCR7                                (0x03C + TACI_BASE)     /* Table Access Control 7 */

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

/* Switch Core Control Registers 
*/
#define SWCORECNR                           (SWCORE_BASE + 0x00006000)

#define MACMR                               (0x004 + SWCORECNR)   /* MAC monitor */
#define VLANTCR                             (0x008 + SWCORECNR)   /* Vlan tag control */
#define DSCR0                               (0x00C + SWCORECNR)   /* Qos by DS control */
#define DSCR1                               (0x010 + SWCORECNR)   /* Qos by DS control */
#define QOSCR                               (0x014 + SWCORECNR)   /* Qos control */



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


/* Test mode enable register 
*/
#define TX_TEST_PORT_OFFSET                 26          /* Tx test mode enable port offset */
#define RX_TEST_PORT_OFFSET                 18          /* Rx test mode enable port offset */
#define MiiTxPktRDY_OFFSET                  10          /* Status flag of MII TX packet ready to send on port */
#define HSABUSY								(1 << 2)	/* HSA process ready flag */
#define ENHSBTESTMODE						(1 << 1)	/* Enable HSB Test Mode */
#define HSB_RDY 	   	   	   	   	   	   	(1 << 0)   	/* HSB ready */



/* Miscellaneous control registers 
*/

#define MISC_BASE                           (SWCORE_BASE + 0x00007000)
#define LEDCR                               (0x000 + SWCORE_BASE + 0x4300)     /* LED control */
#define BCR0                                (0x008 + MISC_BASE)     /* Input bandwidth control */
#define BCR1                                (0x00C + MISC_BASE)     /* Ouput bandwidth control */
#define FCREN                               (0x014 + MISC_BASE)     /* Flow control enable control */
#define FCRTH                               (0x018 + MISC_BASE)     /* Flow control threshold */
#define FCPTR                               (0x028 + MISC_BASE)     /* Flow control prime threshold register */
#define PTCR                                (0x01C + MISC_BASE)     /* Port trunk control */
#define SWTECR                              (0x020 + MISC_BASE)     /* Switch table extended control */
#define PTRAPCR                             (0x024 + MISC_BASE)     /* Protocol trapping control */
#define CRMR                                   (0x3C+SWMISC_BASE)    /*Chip Revision Management Register*/






#endif   /* _ASICREGS_H */

