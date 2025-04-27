/*
 * Copyright (C) 2009-2019 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 97155 $
 * $Date: 2019-11-10 14:22:30 +0800 (Sun, 10 Nov 2019) $
 *
 * Purpose : RTL8390 INTR source mapping
 *
 * Feature : RTL8390 INTR source mapping
 *
 */

/* CPU MIPS 34Kc Interrupt Source*/
#define RTL8390_CPU_IP_MIN          2
#define RTL8390_CPU_IP_MAX          7

#define BSP_UNUSED_FIELD            0xFFFFFFFF

#define RTL8390_ICTL_BASE	            16
#define RTL8390_ICTL_NUM	            32

/* Interrupt source index*/
#define RTL8390_TC0_INTRSRC             29
#define RTL8390_TC1_INTRSRC             28
#define RTL8390_TC2_INTRSRC             17
#define RTL8390_TC3_INTRSRC             16
#define RTL8390_TC4_INTRSRC             15
#define RTL8390_WDT0_INTRSRC            19
#define RTL8390_SWCORE_INTRSRC          20
#define RTL8390_NIC_INTRSRC             24
#define RTL8390_GPIO_INTRSRC            23
#define RTL8390_UART0_INTRSRC           31
#define RTL8390_UART1_INTRSRC           30
#define RTL8390_TC4DEL_INTRSRC          12
#define RTL8390_TC3DEL_INTRSRC          13
#define RTL8390_TC2DEL_INTRSRC          14
#define RTL8390_OCPTO_INTRSRC           27
#define RTL8390_HLXTO_INTRSRC           26
#define RTL8390_SLXTO_INTRSRC           25

/* Interrupt IRQ */
#define RTL8390_TC0_IRQ                 (RTL8390_ICTL_BASE + RTL8390_TC0_INTRSRC)
#define RTL8390_TC1_IRQ                 (RTL8390_ICTL_BASE + RTL8390_TC1_INTRSRC)
#define RTL8390_TC2_IRQ                 (RTL8390_ICTL_BASE + RTL8390_TC2_INTRSRC)
#define RTL8390_TC3_IRQ                 (RTL8390_ICTL_BASE + RTL8390_TC3_INTRSRC)
#define RTL8390_TC4_IRQ                 (RTL8390_ICTL_BASE + RTL8390_TC4_INTRSRC)
#define RTL8390_WDT0_IRQ                (RTL8390_ICTL_BASE + RTL8390_WDT0_INTRSRC)
#define RTL8390_SWCORE_IRQ              (RTL8390_ICTL_BASE + RTL8390_SWCORE_INTRSRC)
#define RTL8390_NIC_IRQ                 (RTL8390_ICTL_BASE + RTL8390_NIC_INTRSRC)
#define RTL8390_GPIO_IRQ                (RTL8390_ICTL_BASE + RTL8390_GPIO_INTRSRC)
#define RTL8390_UART0_IRQ               (RTL8390_ICTL_BASE + RTL8390_UART0_INTRSRC)
#define RTL8390_UART1_IRQ               (RTL8390_ICTL_BASE + RTL8390_UART1_INTRSRC)
#define RTL8390_TC4DEL_IRQ              (RTL8390_ICTL_BASE + RTL8390_TC4DEL_INTRSRC)
#define RTL8390_TC3DEL_IRQ              (RTL8390_ICTL_BASE + RTL8390_TC3DEL_INTRSRC)
#define RTL8390_TC2DEL_IRQ              (RTL8390_ICTL_BASE + RTL8390_TC2DEL_INTRSRC)
#define RTL8390_HLXTO_IRQ               (RTL8390_ICTL_BASE + RTL8390_HLXTO_INTRSRC)
#define RTL8390_SLXTO_IRQ               (RTL8390_ICTL_BASE + RTL8390_SLXTO_INTRSRC)
#define RTL8390_OCPTO_IRQ               (RTL8390_ICTL_BASE + RTL8390_OCPTO_INTRSRC)


/* Interrupt Connect to CPU IP index */
#define RTL8390_UART1_CPU_IP      			3
#define RTL8390_UART0_CPU_IP  			    2
#define RTL8390_NIC_CPU_IP         			6
#define	RTL8390_SWCORE_CPU_IP      			5
#define	RTL8390_OCPTO_CPU_IP    			3
#define	RTL8390_HLXTO_CPU_IP    			3
#define	RTL8390_SLXTO_CPU_IP    			3
#define RTL8390_GPIO_ABCD_CPU_IP   			4
#define RTL8390_TC4_CPU_IP                  6
#define RTL8390_TC3_CPU_IP                  6
#define RTL8390_TC2_CPU_IP                  6
#define RTL8390_TC1_CPU_IP                  6
#define RTL8390_TC0_CPU_IP                  6
#define RTL8390_WDT_IP1_CPU_IP     			3
#define RTL8390_TC4DEL_CPU_IP               3
#define RTL8390_TC3DEL_CPU_IP               3
#define RTL8390_TC2DEL_CPU_IP               3

#define RTL8390_CPU_IP_REG_OFFSET           1

/* Interrupt Routing Register Configure */
#define RTL8390_UART1_RS      			    (RTL8390_UART1_CPU_IP - RTL8390_CPU_IP_REG_OFFSET)
#define RTL8390_UART0_RS  			        (RTL8390_UART0_CPU_IP - RTL8390_CPU_IP_REG_OFFSET)
#define RTL8390_NIC_RS         			    (RTL8390_NIC_CPU_IP - RTL8390_CPU_IP_REG_OFFSET)
#define	RTL8390_SWCORE_RS     			    (RTL8390_SWCORE_CPU_IP - RTL8390_CPU_IP_REG_OFFSET)
#define	RTL8390_OCPTO_RS    			    (RTL8390_OCPTO_CPU_IP - RTL8390_CPU_IP_REG_OFFSET)
#define	RTL8390_HLXTO_RS    			    (RTL8390_HLXTO_CPU_IP - RTL8390_CPU_IP_REG_OFFSET)
#define	RTL8390_SLXTO_RS    			    (RTL8390_SLXTO_CPU_IP - RTL8390_CPU_IP_REG_OFFSET)
#define RTL8390_GPIO_ABCD_RS   			    (RTL8390_GPIO_ABCD_CPU_IP - RTL8390_CPU_IP_REG_OFFSET)
#define RTL8390_TC4_RS                      (RTL8390_TC4_CPU_IP - RTL8390_CPU_IP_REG_OFFSET)
#define RTL8390_TC3_RS                      (RTL8390_TC3_CPU_IP - RTL8390_CPU_IP_REG_OFFSET)
#define RTL8390_TC2_RS                      (RTL8390_TC2_CPU_IP - RTL8390_CPU_IP_REG_OFFSET)
#define RTL8390_TC1_RS                      (RTL8390_TC1_CPU_IP - RTL8390_CPU_IP_REG_OFFSET)
#define RTL8390_TC0_RS                      (RTL8390_TC0_CPU_IP - RTL8390_CPU_IP_REG_OFFSET)
#define RTL8390_WDT_IP1_RS     			    (RTL8390_WDT_IP1_CPU_IP - RTL8390_CPU_IP_REG_OFFSET)
#define RTL8390_TC4DEL_RS                   (RTL8390_TC4DEL_CPU_IP - RTL8390_CPU_IP_REG_OFFSET)
#define RTL8390_TC3DEL_RS                   (RTL8390_TC3DEL_CPU_IP - RTL8390_CPU_IP_REG_OFFSET)
#define RTL8390_TC2DEL_RS                   (RTL8390_TC2DEL_CPU_IP - RTL8390_CPU_IP_REG_OFFSET)


#define RTL8390_ICTL_REG_BASE	            0xb8003000UL

#define RTL8390_IRR0  			            (RTL8390_ICTL_REG_BASE + 0x8)

#define RTL8390_IRR0_SETTING                ((RTL8390_UART0_RS  << 28) | \
                                             (RTL8390_UART1_RS	<< 24) | \
                                             (RTL8390_TC0_RS	<< 20) | \
                                             (RTL8390_TC1_RS	<< 16) | \
                                             (RTL8390_OCPTO_RS	<< 12) | \
                                             (RTL8390_HLXTO_RS	<< 8) | \
                                             (RTL8390_SLXTO_RS	<< 4) | \
                                             (RTL8390_NIC_RS	<< 0)	 \
                                            )

#define RTL8390_IRR1    	    		    (RTL8390_ICTL_REG_BASE + 0xc)
#define RTL8390_IRR1_SETTING                ((RTL8390_GPIO_ABCD_RS  << 28) | \
                                             (RTL8390_SWCORE_RS     << 16) | \
                                             (RTL8390_WDT_IP1_RS    << 12) \
                                            )

#define RTL8390_IRR2	        		    (RTL8390_ICTL_REG_BASE + 0x10)
#define RTL8390_IRR2_SETTING                ((RTL8390_TC2_RS        << 28) | \
                                             (RTL8390_TC3_RS        << 24) | \
                                             (RTL8390_TC4_RS        << 20) | \
                                             (RTL8390_TC2DEL_RS     << 16) | \
                                             (RTL8390_TC3DEL_RS     << 12) | \
                                             (RTL8390_TC3DEL_RS     << 8)  \
                                            )

#define RTL8390_IRR3			            (RTL8390_ICTL_REG_BASE + 0x14)


#define RTL8390_GIMR			            (RTL8390_ICTL_REG_BASE + 0x0)
#define RTL8390_GISR			            (RTL8390_ICTL_REG_BASE + 0x4)
#define RTL8390_GIMR1			            (RTL8390_ICTL_REG_BASE + 0x18)
#define RTL8390_GISR1			            (RTL8390_ICTL_REG_BASE + 0x1c)


#define RTL8390_IRQ_2_INTRSRC(irq)		    (irq - RTL8390_ICTL_BASE)
#define RTL8390_INTRSRC_2_IRQ(intr_src)  	(intr_src + RTL8390_ICTL_BASE)


/* Interrupt device tree node name */
#define RTL8390_TC0                 "rtk_tc0"
#define RTL8390_TC1                 "rtk_tc1"
#define RTL8390_TC2                 "rtk_tc2"
#define RTL8390_TC3                 "rtk_tc3"
#define RTL8390_TC4                 "rtk_tc4"
#define RTL8390_WDT0                "rtk_wdt0"
#define RTL8390_SWCORE              "rtk_swcore"
#define RTL8390_NIC                 "rtk_nic"
#define RTL8390_GPIO                "rtk_gpio"
#define RTL8390_UART0               "ns16550a"
#define RTL8390_UART1               "rtk_uart1"
#define RTL8390_DEL_TC4             "rtk_del_tc4"
#define RTL8390_DEL_TC3             "rtk_del_tc3"
#define RTL8390_DEL_TC2             "rtk_del_tc2"
#define RTL8390_HLXTO               "rtk_hlxto"
#define RTL8390_SLXTO               "rtk_slxto"
#define RTL8390_OCPTO               "rtk_OCPTO"


#define RTL8390_GET_INTR_ID0        0


