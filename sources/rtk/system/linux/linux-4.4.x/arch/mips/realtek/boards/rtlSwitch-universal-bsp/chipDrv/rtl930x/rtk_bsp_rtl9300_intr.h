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
 * Purpose : RTL9300 INTR source mapping
 *
 * Feature : RTL9300 INTR source mapping
 *
 */

/* CPU MIPS 34Kc Interrupt Source*/
#define RTL9300_CPU_IP_MIN          2
#define RTL9300_CPU_IP_MAX          7

#define BSP_UNUSED_FIELD            0xFFFFFFFF

#define RTL9300_ICTL_BASE	            16
#define RTL9300_ICTL_NUM	            32

/* Interrupt source index*/
#define RTL9300_TC0_INTRSRC             7
#define RTL9300_TC1_INTRSRC             8
#define RTL9300_TC2_INTRSRC             9
#define RTL9300_TC3_INTRSRC             10
#define RTL9300_TC4_INTRSRC             11
#define RTL9300_WDT0_INTRSRC            5
#define RTL9300_SWCORE_INTRSRC          23
#define RTL9300_NIC_INTRSRC             24
#define RTL9300_GPIO_INTRSRC            13
#define RTL9300_UART0_INTRSRC           30
#define RTL9300_UART1_INTRSRC           31
#define RTL9300_TC4DEL_INTRSRC          4
#define RTL9300_TC3DEL_INTRSRC          3
#define RTL9300_TC2DEL_INTRSRC          2
#define RTL9300_LXSTO_INTRSRC           15
#define RTL9300_LXMTO_INTRSRC           16
#define RTL9300_OCPTO_INTRSRC           17
#define RTL9300_BTG_INTRSRC             12
#define RTL9300_USBH2_INTRSRC           28
#define RTL9300_PCIE0_INTRSRC           26
#define RTL9300_SPINAND_INTRSRC         19
#define RTL9300_ECC_INTRSRC             20
#define RTL9300_SEC_INTRSRC             22

/* Interrupt IRQ */
#define RTL9300_TC0_IRQ                 (RTL9300_ICTL_BASE + RTL9300_TC0_INTRSRC)
#define RTL9300_TC1_IRQ                 (RTL9300_ICTL_BASE + RTL9300_TC1_INTRSRC)
#define RTL9300_TC2_IRQ                 (RTL9300_ICTL_BASE + RTL9300_TC2_INTRSRC)
#define RTL9300_TC3_IRQ                 (RTL9300_ICTL_BASE + RTL9300_TC3_INTRSRC)
#define RTL9300_TC4_IRQ                 (RTL9300_ICTL_BASE + RTL9300_TC4_INTRSRC)
#define RTL9300_WDT0_IRQ                (RTL9300_ICTL_BASE + RTL9300_WDT0_INTRSRC)
#define RTL9300_SWCORE_IRQ              (RTL9300_ICTL_BASE + RTL9300_SWCORE_INTRSRC)
#define RTL9300_NIC_IRQ                 (RTL9300_ICTL_BASE + RTL9300_NIC_INTRSRC)
#define RTL9300_GPIO_IRQ                (RTL9300_ICTL_BASE + RTL9300_GPIO_INTRSRC)
#define RTL9300_UART0_IRQ               (RTL9300_ICTL_BASE + RTL9300_UART0_INTRSRC)
#define RTL9300_UART1_IRQ               (RTL9300_ICTL_BASE + RTL9300_UART1_INTRSRC)
#define RTL9300_TC4DEL_IRQ              (RTL9300_ICTL_BASE + RTL9300_TC4DEL_INTRSRC)
#define RTL9300_TC3DEL_IRQ              (RTL9300_ICTL_BASE + RTL9300_TC3DEL_INTRSRC)
#define RTL9300_TC2DEL_IRQ              (RTL9300_ICTL_BASE + RTL9300_TC2DEL_INTRSRC)
#define RTL9300_LXSTO_IRQ               (RTL9300_ICTL_BASE + RTL9300_LXSTO_INTRSRC)
#define RTL9300_LXMTO_IRQ               (RTL9300_ICTL_BASE + RTL9300_LXMTO_INTRSRC)
#define RTL9300_OCPTO_IRQ               (RTL9300_ICTL_BASE + RTL9300_OCPTO_INTRSRC)
#define RTL9300_BTG_IRQ                 (RTL9300_ICTL_BASE + RTL9300_BTG_INTRSRC)
#define RTL9300_USBH2_IRQ               (RTL9300_ICTL_BASE + RTL9300_USBH2_INTRSRC)
#define RTL9300_PCIE0_IRQ               (RTL9300_ICTL_BASE + RTL9300_PCIE0_INTRSRC)
#define RTL9300_SPINAND_IRQ             (RTL9300_ICTL_BASE + RTL9300_SPINAND_INTRSRC)
#define RTL9300_ECC_IRQ                 (RTL9300_ICTL_BASE + RTL9300_ECC_INTRSRC)
#define RTL9300_SEC_IRQ                 (RTL9300_ICTL_BASE + RTL9300_SEC_INTRSRC)


/* Interrupt Connect to CPU IP index */
#define RTL9300_UART1_CPU_IP      			3
#define RTL9300_UART0_CPU_IP  			    2
#define RTL9300_USB_H2_CPU_IP  		 	    4
#define RTL9300_PCIE0_CPU_IP   		 	    3
#define RTL9300_NIC_CPU_IP         			6
#define	RTL9300_SWCORE_CPU_IP      			5
#define	RTL9300_SEC_CPU_IP      			3
#define	RTL9300_ECC_CPU_IP      			3
#define	RTL9300_SPINAND_CPU_IP    			3
#define	RTL9300_OCPTO_CPU_IP    			3
#define	RTL9300_LXMTO_CPU_IP    			3
#define	RTL9300_LXSTO_CPU_IP    			3
#define RTL9300_GPIO_ABCD_CPU_IP   			4
#define RTL9300_BTG_CPU_IP      			3
#define RTL9300_TC4_CPU_IP                  7
#define RTL9300_TC3_CPU_IP                  7
#define RTL9300_TC2_CPU_IP                  7
#define RTL9300_TC1_CPU_IP                  7
#define RTL9300_TC0_CPU_IP                  7
#define RTL9300_WDT_IP1_CPU_IP     			3
#define RTL9300_TC4DEL_CPU_IP               3
#define RTL9300_TC3DEL_CPU_IP               3
#define RTL9300_TC2DEL_CPU_IP               3

#define RTL9300_CPU_IP_REG_OFFSET           1

/* Interrupt Routing Register Configure */
#define RTL9300_UART1_RS      			    (RTL9300_UART1_CPU_IP - RTL9300_CPU_IP_REG_OFFSET)
#define RTL9300_UART0_RS  			        (RTL9300_UART0_CPU_IP - RTL9300_CPU_IP_REG_OFFSET)
#define RTL9300_USB_H2_RS  		 	        (RTL9300_USB_H2_CPU_IP - RTL9300_CPU_IP_REG_OFFSET)
#define RTL9300_PCIE0_RS   		 	        (RTL9300_PCIE0_CPU_IP - RTL9300_CPU_IP_REG_OFFSET)
#define RTL9300_NIC_RS         			    (RTL9300_NIC_CPU_IP - RTL9300_CPU_IP_REG_OFFSET)
#define	RTL9300_SWCORE_RS     			    (RTL9300_SWCORE_CPU_IP - RTL9300_CPU_IP_REG_OFFSET)
#define	RTL9300_SEC_RS      			    (RTL9300_SEC_CPU_IP - RTL9300_CPU_IP_REG_OFFSET)
#define	RTL9300_ECC_RS      			    (RTL9300_ECC_CPU_IP - RTL9300_CPU_IP_REG_OFFSET)
#define	RTL9300_SPINAND_RS    			    (RTL9300_SPINAND_CPU_IP - RTL9300_CPU_IP_REG_OFFSET)
#define	RTL9300_OCPTO_RS    			    (RTL9300_OCPTO_CPU_IP - RTL9300_CPU_IP_REG_OFFSET)
#define	RTL9300_LXMTO_RS    			    (RTL9300_LXMTO_CPU_IP - RTL9300_CPU_IP_REG_OFFSET)
#define	RTL9300_LXSTO_RS    			    (RTL9300_LXSTO_CPU_IP - RTL9300_CPU_IP_REG_OFFSET)
#define RTL9300_GPIO_ABCD_RS   			    (RTL9300_GPIO_ABCD_CPU_IP - RTL9300_CPU_IP_REG_OFFSET)
#define RTL9300_BTG_RS      			    (RTL9300_BTG_CPU_IP - RTL9300_CPU_IP_REG_OFFSET)
#define RTL9300_TC4_RS                      (RTL9300_TC4_CPU_IP - RTL9300_CPU_IP_REG_OFFSET)
#define RTL9300_TC3_RS                      (RTL9300_TC3_CPU_IP - RTL9300_CPU_IP_REG_OFFSET)
#define RTL9300_TC2_RS                      (RTL9300_TC2_CPU_IP - RTL9300_CPU_IP_REG_OFFSET)
#define RTL9300_TC1_RS                      (RTL9300_TC1_CPU_IP - RTL9300_CPU_IP_REG_OFFSET)
#define RTL9300_TC0_RS                      (RTL9300_TC0_CPU_IP - RTL9300_CPU_IP_REG_OFFSET)
#define RTL9300_WDT_IP1_RS     			    (RTL9300_WDT_IP1_CPU_IP - RTL9300_CPU_IP_REG_OFFSET)
#define RTL9300_TC4DEL_RS                   (RTL9300_TC4DEL_CPU_IP - RTL9300_CPU_IP_REG_OFFSET)
#define RTL9300_TC3DEL_RS                   (RTL9300_TC3DEL_CPU_IP - RTL9300_CPU_IP_REG_OFFSET)
#define RTL9300_TC2DEL_RS                   (RTL9300_TC2DEL_CPU_IP - RTL9300_CPU_IP_REG_OFFSET)


#define RTL9300_ICTL_REG_BASE	            0xb8003000UL

#if 1
#define RTL9300_IRR0  			            (RTL9300_ICTL_REG_BASE + 0x8)

#define RTL9300_IRR0_SETTING                ((RTL9300_UART1_RS  << 28) | \
                                             (RTL9300_UART0_RS	<< 24) | \
                                             (RTL9300_USB_H2_RS	<< 16) | \
                                             (RTL9300_PCIE0_RS	<< 8)  | \
                                             (RTL9300_NIC_RS	<< 0)	 \
                                            )

#define RTL9300_IRR1    	    		    (RTL9300_ICTL_REG_BASE + 0xc)
#define RTL9300_IRR1_SETTING                ((RTL9300_SWCORE_RS  << 28) | \
                                             (RTL9300_SEC_RS     << 24) | \
                                             (RTL9300_ECC_RS     << 16) | \
                                             (RTL9300_SPINAND_RS << 12) | \
                                             (RTL9300_OCPTO_RS   << 4)  | \
                                             (RTL9300_LXMTO_RS   << 0)    \
                                            )

#define RTL9300_IRR2	        		    (RTL9300_ICTL_REG_BASE + 0x10)
#define RTL9300_IRR2_SETTING                ((RTL9300_LXSTO_RS      << 28) | \
                                             (RTL9300_GPIO_ABCD_RS  << 20) | \
                                             (RTL9300_BTG_RS        << 16) | \
                                             (RTL9300_TC4_RS        << 12) | \
                                             (RTL9300_TC3_RS        << 8)  | \
                                             (RTL9300_TC2_RS        << 4)  | \
                                             (RTL9300_TC1_RS        << 0)    \
                                            )

#define RTL9300_IRR3			            (RTL9300_ICTL_REG_BASE + 0x14)
#define RTL9300_IRR3_SETTING                ((RTL9300_TC0_RS      << 28) | \
                                             (RTL9300_WDT_IP1_RS  << 20) | \
                                             (RTL9300_TC4DEL_RS   << 16) | \
                                             (RTL9300_TC3DEL_RS   << 12) | \
                                             (RTL9300_TC2DEL_RS   << 8)    \
                                            )
#else
#define RTL9300_IRR0  			            (RTL9300_ICTL_REG_BASE + 0x8)

#define RTL9300_IRR0_SETTING                ((RTL9300_UART1_RS  << 28) | \
                                             (RTL9300_UART0_RS	<< 24) | \
                                             (RTL9300_USB_H2_RS	<< 16) | \
                                             (RTL9300_NIC_RS	<< 0)	 \
                                            )

#define RTL9300_IRR1    	    		    (RTL9300_ICTL_REG_BASE + 0xc)
#define RTL9300_IRR1_SETTING                ((RTL9300_SWCORE_RS  << 28) \
                                            )

#define RTL9300_IRR2	        		    (RTL9300_ICTL_REG_BASE + 0x10)
#define RTL9300_IRR2_SETTING                ((RTL9300_TC4_RS        << 12) | \
                                             (RTL9300_TC3_RS        << 8)  | \
                                             (RTL9300_TC2_RS        << 4)  | \
                                             (RTL9300_TC1_RS        << 0)    \
                                            )

#define RTL9300_IRR3			            (RTL9300_ICTL_REG_BASE + 0x14)
#define RTL9300_IRR3_SETTING                (RTL9300_TC0_RS      << 28)

#endif


#define RTL9300_GIMR			            (RTL9300_ICTL_REG_BASE + 0x0)
   #define UART0_IE_RTL9300		            (1 << 30)
   #define TC0_IE_RTL9300		            (1 << 7)
   #define TC2_IE_RTL9300		            (1 << 9)

#define RTL9300_GISR			            (RTL9300_ICTL_REG_BASE + 0x4)
   #define UART0_IP_RTL9300		            (1 << 30)
   #define TC0_IP_RTL9300		            (1 << 7)


#define RTL9300_IRQ_2_INTRSRC(irq)		    (irq - RTL9300_ICTL_BASE)
#define RTL9300_INTRSRC_2_IRQ(intr_src)  	(intr_src + RTL9300_ICTL_BASE)


/* Interrupt device tree node name */
#define RTL9300_TC0                 "rtk_tc0"
#define RTL9300_TC1                 "rtk_tc1"
#define RTL9300_TC2                 "rtk_tc2"
#define RTL9300_TC3                 "rtk_tc3"
#define RTL9300_TC4                 "rtk_tc4"
#define RTL9300_WDT0                "rtk_wdt0"
#define RTL9300_SWCORE              "rtk_swcore"
#define RTL9300_NIC                 "rtk_nic"
#define RTL9300_GPIO                "rtk_gpio"
#define RTL9300_UART0               "ns16550a"
#define RTL9300_UART1               "rtk_uart1"
#define RTL9300_DEL_TC4             "rtk_del_tc4"
#define RTL9300_DEL_TC3             "rtk_del_tc3"
#define RTL9300_DEL_TC2             "rtk_del_tc2"
#define RTL9300_LXSTO               "rtk_lxsto"
#define RTL9300_LXMTO               "rtk_lxmto"
#define RTL9300_OCPTO               "rtk_OCPTO"
#define RTL9300_USBH2               "rtk_usbh2"
#define RTL9300_SPI_NAND            "rtk_spinand"
#define RTL9300_ECC                 "rtk_ecc"
#define RTL9300_SEC                 "rtk_sec"

#define RTL9300_GET_INTR_ID0        0


