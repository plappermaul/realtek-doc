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
 * Purpose : RTL8380 INTR source mapping
 *
 * Feature : RTL8380 INTR source mapping
 *
 */

/* CPU MIPS 34Kc Interrupt Source*/
#define RTL8380_CPU_IP_MIN              2
#define RTL8380_CPU_IP_MAX              7

#define BSP_UNUSED_FIELD                0xFFFFFFFF

#define RTL8380_ICTL_BASE	            16
#define RTL8380_ICTL_NUM	            32

/* Interrupt source index*/
#define RTL8380_TC0_INTRSRC             29
#define RTL8380_TC1_INTRSRC             28
#define RTL8380_TC2_INTRSRC             17
#define RTL8380_TC3_INTRSRC             16
#define RTL8380_TC4_INTRSRC             15
#define RTL8380_WDT0_INTRSRC            19
#define RTL8380_SWCORE_INTRSRC          20
#define RTL8380_NIC_INTRSRC             24
#define RTL8380_GPIO_INTRSRC            23
#define RTL8380_UART0_INTRSRC           31
#define RTL8380_UART1_INTRSRC           30
#define RTL8380_TC4DEL_INTRSRC          12
#define RTL8380_TC3DEL_INTRSRC          13
#define RTL8380_TC2DEL_INTRSRC          14
#define RTL8380_OCPTO_INTRSRC           27
#define RTL8380_HLXTO_INTRSRC           26
#define RTL8380_SLXTO_INTRSRC           25

/* Interrupt IRQ */
#define RTL8380_TC0_IRQ                 (RTL8380_ICTL_BASE + RTL8380_TC0_INTRSRC)
#define RTL8380_TC1_IRQ                 (RTL8380_ICTL_BASE + RTL8380_TC1_INTRSRC)
#define RTL8380_TC2_IRQ                 (RTL8380_ICTL_BASE + RTL8380_TC2_INTRSRC)
#define RTL8380_TC3_IRQ                 (RTL8380_ICTL_BASE + RTL8380_TC3_INTRSRC)
#define RTL8380_TC4_IRQ                 (RTL8380_ICTL_BASE + RTL8380_TC4_INTRSRC)
#define RTL8380_WDT0_IRQ                (RTL8380_ICTL_BASE + RTL8380_WDT0_INTRSRC)
#define RTL8380_SWCORE_IRQ              (RTL8380_ICTL_BASE + RTL8380_SWCORE_INTRSRC)
#define RTL8380_NIC_IRQ                 (RTL8380_ICTL_BASE + RTL8380_NIC_INTRSRC)
#define RTL8380_GPIO_IRQ                (RTL8380_ICTL_BASE + RTL8380_GPIO_INTRSRC)
#define RTL8380_UART0_IRQ               (RTL8380_ICTL_BASE + RTL8380_UART0_INTRSRC)
#define RTL8380_UART1_IRQ               (RTL8380_ICTL_BASE + RTL8380_UART1_INTRSRC)
#define RTL8380_TC4DEL_IRQ              (RTL8380_ICTL_BASE + RTL8380_TC4DEL_INTRSRC)
#define RTL8380_TC3DEL_IRQ              (RTL8380_ICTL_BASE + RTL8380_TC3DEL_INTRSRC)
#define RTL8380_TC2DEL_IRQ              (RTL8380_ICTL_BASE + RTL8380_TC2DEL_INTRSRC)
#define RTL8380_HLXTO_IRQ               (RTL8380_ICTL_BASE + RTL8380_HLXTO_INTRSRC)
#define RTL8380_SLXTO_IRQ               (RTL8380_ICTL_BASE + RTL8380_SLXTO_INTRSRC)
#define RTL8380_OCPTO_IRQ               (RTL8380_ICTL_BASE + RTL8380_OCPTO_INTRSRC)


/* Interrupt Connect to CPU IP index */
#define RTL8380_UART1_CPU_IP      			3
#define RTL8380_UART0_CPU_IP  			    2
#define RTL8380_NIC_CPU_IP         			6
#define	RTL8380_SWCORE_CPU_IP      			5
#define	RTL8380_OCPTO_CPU_IP    			3
#define	RTL8380_HLXTO_CPU_IP    			3
#define	RTL8380_SLXTO_CPU_IP    			3
#define RTL8380_GPIO_ABCD_CPU_IP   			4
#define RTL8380_TC4_CPU_IP                  6
#define RTL8380_TC3_CPU_IP                  6
#define RTL8380_TC2_CPU_IP                  6
#define RTL8380_TC1_CPU_IP                  6
#define RTL8380_TC0_CPU_IP                  6
#define RTL8380_WDT_IP1_CPU_IP     			3
#define RTL8380_TC4DEL_CPU_IP               3
#define RTL8380_TC3DEL_CPU_IP               3
#define RTL8380_TC2DEL_CPU_IP               3

#define RTL8380_CPU_IP_REG_OFFSET           1

/* Interrupt Routing Register Configure */
#define RTL8380_UART1_RS      			    (RTL8380_UART1_CPU_IP - RTL8380_CPU_IP_REG_OFFSET)
#define RTL8380_UART0_RS  			        (RTL8380_UART0_CPU_IP - RTL8380_CPU_IP_REG_OFFSET)
#define RTL8380_NIC_RS         			    (RTL8380_NIC_CPU_IP - RTL8380_CPU_IP_REG_OFFSET)
#define	RTL8380_SWCORE_RS     			    (RTL8380_SWCORE_CPU_IP - RTL8380_CPU_IP_REG_OFFSET)
#define	RTL8380_OCPTO_RS    			    (RTL8380_OCPTO_CPU_IP - RTL8380_CPU_IP_REG_OFFSET)
#define	RTL8380_HLXTO_RS    			    (RTL8380_HLXTO_CPU_IP - RTL8380_CPU_IP_REG_OFFSET)
#define	RTL8380_SLXTO_RS    			    (RTL8380_SLXTO_CPU_IP - RTL8380_CPU_IP_REG_OFFSET)
#define RTL8380_GPIO_ABCD_RS   			    (RTL8380_GPIO_ABCD_CPU_IP - RTL8380_CPU_IP_REG_OFFSET)
#define RTL8380_TC4_RS                      (RTL8380_TC4_CPU_IP - RTL8380_CPU_IP_REG_OFFSET)
#define RTL8380_TC3_RS                      (RTL8380_TC3_CPU_IP - RTL8380_CPU_IP_REG_OFFSET)
#define RTL8380_TC2_RS                      (RTL8380_TC2_CPU_IP - RTL8380_CPU_IP_REG_OFFSET)
#define RTL8380_TC1_RS                      (RTL8380_TC1_CPU_IP - RTL8380_CPU_IP_REG_OFFSET)
#define RTL8380_TC0_RS                      (RTL8380_TC0_CPU_IP - RTL8380_CPU_IP_REG_OFFSET)
#define RTL8380_WDT_IP1_RS     			    (RTL8380_WDT_IP1_CPU_IP - RTL8380_CPU_IP_REG_OFFSET)
#define RTL8380_TC4DEL_RS                   (RTL8380_TC4DEL_CPU_IP - RTL8380_CPU_IP_REG_OFFSET)
#define RTL8380_TC3DEL_RS                   (RTL8380_TC3DEL_CPU_IP - RTL8380_CPU_IP_REG_OFFSET)
#define RTL8380_TC2DEL_RS                   (RTL8380_TC2DEL_CPU_IP - RTL8380_CPU_IP_REG_OFFSET)


#define RTL8380_ICTL_REG_BASE	            0xb8003000UL

#define RTL8380_IRR0  			            (RTL8380_ICTL_REG_BASE + 0x8)

#define RTL8380_IRR0_SETTING                ((RTL8380_UART0_RS  << 28) | \
                                             (RTL8380_UART1_RS	<< 24) | \
                                             (RTL8380_TC0_RS	<< 20) | \
                                             (RTL8380_TC1_RS	<< 16) | \
                                             (RTL8380_OCPTO_RS	<< 12) | \
                                             (RTL8380_HLXTO_RS	<< 8) | \
                                             (RTL8380_SLXTO_RS	<< 4) | \
                                             (RTL8380_NIC_RS	<< 0)	 \
                                            )

#define RTL8380_IRR1    	    		    (RTL8380_ICTL_REG_BASE + 0xc)
#define RTL8380_IRR1_SETTING                ((RTL8380_GPIO_ABCD_RS  << 28) | \
                                             (RTL8380_SWCORE_RS     << 16) | \
                                             (RTL8380_WDT_IP1_RS    << 12) \
                                            )

#define RTL8380_IRR2	        		    (RTL8380_ICTL_REG_BASE + 0x10)
#define RTL8380_IRR2_SETTING                ((RTL8380_TC2_RS        << 28) | \
                                             (RTL8380_TC3_RS        << 24) | \
                                             (RTL8380_TC4_RS        << 20) | \
                                             (RTL8380_TC2DEL_RS     << 16) | \
                                             (RTL8380_TC3DEL_RS     << 12) | \
                                             (RTL8380_TC3DEL_RS     << 8)  \
                                            )

#define RTL8380_IRR3			            (RTL8380_ICTL_REG_BASE + 0x14)

#define RTL8380_GIMR			            (RTL8380_ICTL_REG_BASE + 0x0)
#define RTL8380_GISR			            (RTL8380_ICTL_REG_BASE + 0x4)
#define RTL8380_GIMR1			            (RTL8380_ICTL_REG_BASE + 0x18)
#define RTL8380_GISR1			            (RTL8380_ICTL_REG_BASE + 0x1c)


#define RTL8380_IRQ_2_INTRSRC(irq)		    (irq - RTL8380_ICTL_BASE)
#define RTL8380_INTRSRC_2_IRQ(intr_src)  	(intr_src + RTL8380_ICTL_BASE)


/* Interrupt device tree node name */
#define RTL8380_TC0                 "rtk_tc0"
#define RTL8380_TC1                 "rtk_tc1"
#define RTL8380_TC2                 "rtk_tc2"
#define RTL8380_TC3                 "rtk_tc3"
#define RTL8380_TC4                 "rtk_tc4"
#define RTL8380_WDT0                "rtk_wdt0"
#define RTL8380_SWCORE              "rtk_swcore"
#define RTL8380_NIC                 "rtk_nic"
#define RTL8380_GPIO                "rtk_gpio"
#define RTL8380_UART0               "ns16550a"
#define RTL8380_UART1               "rtk_uart1"
#define RTL8380_DEL_TC4             "rtk_del_tc4"
#define RTL8380_DEL_TC3             "rtk_del_tc3"
#define RTL8380_DEL_TC2             "rtk_del_tc2"
#define RTL8380_HLXTO               "rtk_hlxto"
#define RTL8380_SLXTO               "rtk_slxto"
#define RTL8380_OCPTO               "rtk_OCPTO"


#define RTL8380_GET_INTR_ID0        0


