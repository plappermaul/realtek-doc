/*
 * Realtek Semiconductor Corp.
 *
 * bsp/bspchip.h:
 *   bsp chip address and IRQ mapping file
 *
 * Copyright (C) 2006-2015 Tony Wu (tonywu@realtek.com)
 */

#ifndef _BSPCHIP_H_
#define _BSPCHIP_H_

#include <linux/version.h>

#include <asm/mach-generic/irq.h>

/*
 * CPC Specific defiitions
 */
#define CPC_BASE_ADDR		0x1bde0000
#define CPC_BASE_SIZE		(24 * 1024)


#define U_RBR                   (0x000)
#define U_THR                   (0x000)
#define U_DLL                   (0x000)
#define U_IER                   (0x004)
#define U_DLM                   (0x004)
#define U_IIR                   (0x008)
#define U_FCR                   (0x008)
#define U_LCR                   (0x00C)
#define U_MCR                   (0x010)
#define U_LSR                   (0x014)
#define LSR_THRE                0x20

#define UART0_IRQ           	31
#define UART1_IRQ           	30
#define TC0_IRQ             	29
#define TC1_IRQ             	28
#define OCPTO_IRQ           	27
#define HLXTO_IRQ           	26
#define SLXTO_IRQ           	25
#define NIC_IRQ             	24
#define GPIO_ABCD_IRQ       	23
#define GPIO_EFGH_IRQ       	22
#define RTC_IRQ             	21
#define	SWCORE_IRQ	       	20
#define WDT_IP1_IRQ         	19
#define WDT_IP2_IRQ         	18
#define USB_H2_IRQ       	17
#define TC2_IRQ             	16
#define TC3_IRQ             	15
#define TC4_IRQ             	14
#define TC5_IRQ             	13
#define TC6_IRQ             	12


/*  * IRQ Controller  */
#define BSP_IRQ_CPU_BASE	0
#define BSP_IRQ_CPU_NUM		8

#define BSP_IRQ_ICTL_BASE	(BSP_IRQ_CPU_BASE + BSP_IRQ_CPU_NUM)

#define BSP_UART0_EXT_IRQ		(BSP_IRQ_ICTL_BASE + UART0_IRQ)
#define BSP_UART1_EXT_IRQ		(BSP_IRQ_ICTL_BASE + UART1_IRQ)
#define BSP_TC0_EXT_IRQ			(BSP_IRQ_ICTL_BASE + TC0_IRQ)
#define BSP_TC1_EXT_IRQ			(BSP_IRQ_ICTL_BASE + TC1_IRQ)
#define BSP_OCPTO_EXT_IRQ	        (BSP_IRQ_ICTL_BASE + OCPTO_IRQ)
#define BSP_HLXTO_EXT_IRQ	        (BSP_IRQ_ICTL_BASE + HLXTO_IRQ)
#define BSP_SLXTO_EXT_IRQ	        (BSP_IRQ_ICTL_BASE + SLXTO_IRQ)
#define BSP_NIC_EXT_IRQ			(BSP_IRQ_ICTL_BASE + NIC_IRQ)
#define BSP_GPIO_ABCD_EXT_IRQ	        (BSP_IRQ_ICTL_BASE + GPIO_ABCD_IRQ)
#define BSP_GPIO_EFGH_EXT_IRQ	        (BSP_IRQ_ICTL_BASE + GPIO_EFGH_IRQ)
#define BSP_SWCORE_EXT_IRQ		(BSP_IRQ_ICTL_BASE + SWCORE_IRQ)
#define	BSP_WDT_IP1_IRQ	    	        (BSP_IRQ_ICTL_BASE + WDT_IP1_IRQ)
#define	BSP_WDT_IP2_IRQ	    	        (BSP_IRQ_ICTL_BASE + WDT_IP2_IRQ)
#define	BSP_USB_H2_IRQ	    	        (BSP_IRQ_ICTL_BASE + USB_H2_IRQ)
#define BSP_TC2_EXT_IRQ			(BSP_IRQ_ICTL_BASE + TC2_IRQ)
#define BSP_TC3_EXT_IRQ			(BSP_IRQ_ICTL_BASE + TC3_IRQ)
#define BSP_TC4_EXT_IRQ			(BSP_IRQ_ICTL_BASE + TC4_IRQ)
#define BSP_TC5_EXT_IRQ			(BSP_IRQ_ICTL_BASE + TC5_IRQ)
#define BSP_TC6_EXT_IRQ			(BSP_IRQ_ICTL_BASE + TC6_IRQ)

#define RTK_BSP_ENABLE  		1
#define RTK_BSP_DISABLE         0


/************************************************************************
 * USB Host 2.0 controller
 ***********************************************************************/
//#define RTL9300_EHCI_BASE       0xb8021000
#define RTK_EHCI_BASE       0x18021000
#define RTK_EHCI_BASE_LEN   0xa4

#endif   /* _BSPCHIP_H */
