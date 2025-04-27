/*
 * Copyright (C) 2009-2015 Realtek Semiconductor Corp.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * $Revision: 97887 $
 * $Date: 2019-06-21 10:17:49 +0800 (Fri, 21 Jun 2019) $
 *
 */

#ifndef _PLATFORM_H
#define _PLATFORM_H

#include <common/rt_autoconf.h>

/*
 *  =============
 *  Utilty Macros
 *  =============
 */
#define REG8(reg)    (*(volatile unsigned char *)((unsigned int)reg))
#define REG32(reg)   (*(volatile unsigned int *)((unsigned int)reg))

#if defined(CONFIG_SDK_ENDIAN_LITTLE)
#define big_endian32(x) ((((x) & 0xff000000) >> 24) | (((x) & 0x00ff0000) >> 8) | (((x) & 0x0000ff00) << 8)|(((x) & 0x000000ff) << 24))
#define big_endian16(x) (((x) >> 8) | (((x) & 0x000000ff) << 8))
#else
#define big_endian32(x) (x)
#define big_endian16(x) (x)
#endif
#define big_endian(x) big_endian32(x) /* backward-compatible */

#define MEM32_READ(reg)         big_endian(REG32(reg))
#define MEM32_WRITE(reg,val)    REG32(reg) = big_endian(val)

/*
 *  ====================================
 *  Platform Configurable Common Options
 *  ====================================
 */

#define PROM_DEBUG      0

#if defined(CONFIG_SDK_FPGA_PLATFORM) && defined(CONFIG_SDK_RTL9300)
#define MHZ_9300  		25
#define MHZ             25
#define MHZ_9300_MP     25
#else
#define MHZ             200
#define MHZ_9300        200
#define MHZ_9300_MP     175
#endif
#define SYSCLK                  MHZ * 1000 * 1000
#define SYSCLK_9300_MP          MHZ_9300_MP * 1000 * 1000

#define BAUDRATE                115200  /* ex. 19200 or 38400 or 57600 or 115200 */
                                        /* For Early Debug */

#define DIVISOR_RTL8390     	55
#define DIVISOR_RTL9300     	55
#define DIVISOR             	1000

#define L2_BYPASS_MODE    (1<<12)


#if DIVISOR > (1 << 16)
#error "Exceed the Maximum Value of DivFactor"
#endif
#if DIVISOR_RTL9300 > (1 << 16)
#error "Exceed the Maximum Value of DivFactor"
#endif

/*
 *  ==========================
 *  Platform Register Settings
 *  ==========================
 */

/*
 * Memory Controller
 */
#define MC_MCR          0xB8001000
   #define MC_MCR_VAL      0x00000000

#define MC_DCR          0xB8001004
   #define MC_DCR0_VAL     0x54480000

#define MC_DTCR         0xB8001008
   #define MC_DTCR_VAL     0xFFFF05C0

/*
 * Reset
 */
#define	RGCR			0xBB001E70


/*
 * UART
 */
#define UART0_BASE      0xB8002000
#define UART0_RBR       (UART0_BASE + 0x000)
#define UART0_THR       (UART0_BASE + 0x000)
#define UART0_DLL       (UART0_BASE + 0x000)
#define UART0_IER       (UART0_BASE + 0x004)
#define UART0_DLM       (UART0_BASE + 0x004)
#define UART0_IIR       (UART0_BASE + 0x008)
#define UART0_FCR       (UART0_BASE + 0x008)
#define UART0_LCR       (UART0_BASE + 0x00C)
#define UART0_MCR       (UART0_BASE + 0x010)
#define UART0_LSR       (UART0_BASE + 0x014)

#define BSP_UART1_BASE      0xB8002100
#define UART1_RBR       (BSP_UART1_BASE + 0x000)
#define UART1_THR       (BSP_UART1_BASE + 0x000)
#define UART1_DLL       (BSP_UART1_BASE + 0x000)
#define UART1_IER       (BSP_UART1_BASE + 0x004)
#define UART1_DLM       (BSP_UART1_BASE + 0x004)
#define UART1_IIR       (BSP_UART1_BASE + 0x008)
#define UART1_FCR       (BSP_UART1_BASE + 0x008)
   #define FCR_EN          0x01
   #define FCR_RXRST       0x02
   #define     RXRST             0x02
   #define FCR_TXRST       0x04
   #define     TXRST             0x04
   #define FCR_DMA         0x08
   #define FCR_RTRG        0xC0
   #define     CHAR_TRIGGER_01   0x00
   #define     CHAR_TRIGGER_04   0x40
   #define     CHAR_TRIGGER_08   0x80
   #define     CHAR_TRIGGER_14   0xC0
#define UART1_LCR       (BSP_UART1_BASE + 0x00C)
   #define LCR_WLN         0x03
   #define     CHAR_LEN_5        0x00
   #define     CHAR_LEN_6        0x01
   #define     CHAR_LEN_7        0x02
   #define     CHAR_LEN_8        0x03
   #define LCR_STB         0x04
   #define     ONE_STOP          0x00
   #define     TWO_STOP          0x04
   #define LCR_PEN         0x08
   #define     PARITY_ENABLE     0x01
   #define     PARITY_DISABLE    0x00
   #define LCR_EPS         0x30
   #define     PARITY_ODD        0x00
   #define     PARITY_EVEN       0x10
   #define     PARITY_MARK       0x20
   #define     PARITY_SPACE      0x30
   #define LCR_BRK         0x40
   #define LCR_DLAB        0x80
   #define     DLAB              0x80
#define UART1_MCR       (BSP_UART1_BASE + 0x010)
#define UART1_LSR       (BSP_UART1_BASE + 0x014)
   #define LSR_DR          0x01
   #define     RxCHAR_AVAIL      0x01
   #define LSR_OE          0x02
   #define LSR_PE          0x04
   #define LSR_FE          0x08
   #define LSR_BI          0x10
   #define LSR_THRE        0x20
   #define     TxCHAR_AVAIL      0x00
   #define     TxCHAR_EMPTY      0x20
   #define LSR_TEMT        0x40
   #define LSR_RFE         0x80


/*****************************************************************
 * Legacy PIC
 * **************************************************************/
/*  * IRQ Controller  */
#define BSP_IRQ_CPU_BASE	0
#define BSP_IRQ_CPU_NUM		8

#define BSP_IRQ_ICTL_BASE	(BSP_IRQ_CPU_BASE + BSP_IRQ_CPU_NUM)


/************************************************************************
 * BSP External IRQ Num
 * ADD IRQ BASE Num
 ***********************************************************************/
/*
     *   RTL83xx & RTL93xx Interrupt Scheme
     *
     *   Source        IRQ      CPU INT
     *   --------      ------   -------
     *   UART0         31       3
     *   UART1         30       2
     *   TIMER0        29       6
     *   TIMER1        28       2
     *   OCPTO         27       2
     *   HLXTO         26       2
     *   SLXTO         25       2
     *   NIC           24       5
     *   GPIO_ABCD     23       5
     *   GPIO_EFGH     22       5
     *   RTC           21       5
     *   SWCORE        20       4
     *   WDT_IP1       19       5
     *   WDT_IP2       18       5
     *   USB_H2        17       2
*/

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
#define	SWCORE_IRQ	        	20
#define WDT_IP1_IRQ         	19
#define WDT_IP2_IRQ         	18
#define USB_H2_IRQ       		17

#define SYS_IRQ_MAX_NUMER       UART0_IRQ

/* 9300 Mapping index*/
#define RTL9300_UART1_IRQ       31
#define RTL9300_UART0_IRQ       30
#define RTL9300_USB_H2_IRQ      28
#define RTL9300_NIC_IRQ         24
#define RTL9300_SWCORE_IRQ	    23
#define RTL9300_GPIO_ABC_IRQ    13
#define RTL9300_TC4_IRQ         11
#define RTL9300_TC3_IRQ         10
#define RTL9300_TC2_IRQ         9
#define RTL9300_TC1_IRQ         8
#define RTL9300_TC0_IRQ         7
#define RTL9300_WDT_IP1_IRQ     5


/*
 * Interrupt Routing Selection
 */
#define UART0_RS       			2
#define UART1_RS       			1
#define TC0_RS         			5
#define TC1_RS        		 	1
#define OCPTO_RS       			1
#define HLXTO_RS       			1
#define SLXTO_RS       			1
#define NIC_RS         			4
#define GPIO_ABCD_RS   			4
#define GPIO_EFGH_RS   			4
#define RTC_RS         			4
#define	SWCORE_RS      			3
#define WDT_IP1_RS     			4
#define WDT_IP2_RS     			5
#define USB_H2_RS      		 	1


#define BSP_UART0_EXT_IRQ		(BSP_IRQ_ICTL_BASE + UART0_IRQ)
#define BSP_UART1_EXT_IRQ		(BSP_IRQ_ICTL_BASE + UART1_IRQ)
#define BSP_TC0_EXT_IRQ			(BSP_IRQ_ICTL_BASE + TC0_IRQ)
#define BSP_TC1_EXT_IRQ			(BSP_IRQ_ICTL_BASE + TC1_IRQ)
#define BSP_NIC_EXT_IRQ			(BSP_IRQ_ICTL_BASE + NIC_IRQ)
#define BSP_GPIO_ABCD_EXT_IRQ	(BSP_IRQ_ICTL_BASE + GPIO_ABCD_IRQ)
#define BSP_GPIO_EFGH_EXT_IRQ	(BSP_IRQ_ICTL_BASE + GPIO_EFGH_IRQ)
#define BSP_SWCORE_EXT_IRQ		(BSP_IRQ_ICTL_BASE + SWCORE_IRQ)
#define	BSP_WDT_IP1_IRQ	    	(BSP_IRQ_ICTL_BASE + WDT_IP1_IRQ)
#define	BSP_WDT_IP2_IRQ	    	(BSP_IRQ_ICTL_BASE + WDT_IP2_IRQ)
#define	BSP_USB_H2_IRQ	    	(BSP_IRQ_ICTL_BASE + USB_H2_IRQ)


#define BSP_UART0_EXT_IRQ_RTL9300		(BSP_IRQ_ICTL_BASE + RTL9300_UART0_IRQ)
#define BSP_UART1_EXT_IRQ_RTL9300		(BSP_IRQ_ICTL_BASE + RTL9300_UART1_IRQ)
#define BSP_TC0_EXT_IRQ_RTL9300			(BSP_IRQ_ICTL_BASE + RTL9300_TC0_IRQ)
#define BSP_TC1_EXT_IRQ_RTL9300			(BSP_IRQ_ICTL_BASE + RTL9300_TC1_IRQ)
#define BSP_NIC_EXT_IRQ_RTL9300			(BSP_IRQ_ICTL_BASE + RTL9300_NIC_IRQ)
#define BSP_GPIO_ABCD_EXT_IRQ_RTL9300	(BSP_IRQ_ICTL_BASE + RTL9300_GPIO_ABC_IRQ)
#define BSP_SWCORE_EXT_IRQ_RTL9300		(BSP_IRQ_ICTL_BASE + RTL9300_SWCORE_IRQ)
#define	BSP_WDT_IP1_IRQ_RTL9300	    	(BSP_IRQ_ICTL_BASE + RTL9300_WDT_IP1_IRQ)
#define	BSP_USB_H2_IRQ_RTL9300	    	(BSP_IRQ_ICTL_BASE + RTL9300_USB_H2_IRQ)


/*
  * MIPS32R2 counter
  */
#define BSP_COMPARE_IRQ		7
#define BSP_PERFCOUNT_IRQ	7

/*
  *  ICTL
  */
#define ICTL_MAX_IP        	32
#define ICTL_UNUSED_IP     	0xFFFFFFFF

#define ICTL_OFFSET(irq)	((irq) - BSP_IRQ_ICTL_BASE)

#define BSP_ICTL_BASE		0xb8003000UL

#define BSP_ICTL1_IRQ		(BSP_IRQ_CPU_BASE + 2)
#define BSP_ICTL2_IRQ		(BSP_IRQ_CPU_BASE + 3)
#define BSP_ICTL3_IRQ		(BSP_IRQ_CPU_BASE + 4)
#define BSP_ICTL4_IRQ		(BSP_IRQ_CPU_BASE + 5)
#define BSP_ICTL5_IRQ		(BSP_IRQ_CPU_BASE + 6)

#define GIMR			(BSP_ICTL_BASE + 0x0)
   #define UART0_IE		(1 << ICTL_OFFSET(BSP_UART0_EXT_IRQ))
   #define UART1_IE		(1 << ICTL_OFFSET(BSP_UART1_EXT_IRQ))
   #define TC0_IE		(1 << ICTL_OFFSET(BSP_TC0_EXT_IRQ))
   #define TC1_IE		(1 << ICTL_OFFSET(BSP_TC1_EXT_IRQ))
   #define NIC_IE		(1 << ICTL_OFFSET(BSP_NIC_EXT_IRQ))
   #define GPIO_ABCD_IE	(1 << ICTL_OFFSET(BSP_GPIO_ABCD_EXT_IRQ))
   #define SWCORE_IE	(1 << ICTL_OFFSET(BSP_SWCORE_EXT_IRQ))
   #define WDT_IP1_IE	(1 << ICTL_OFFSET(BSP_WDT_IP1_IRQ))
   #define WDT_IP2_IE	(1 << ICTL_OFFSET(BSP_WDT_IP2_IRQ))

   #define UART0_IE_RTL9300	     (1 << ICTL_OFFSET(BSP_UART0_EXT_IRQ_RTL9300))
   #define UART1_IE_RTL9300	     (1 << ICTL_OFFSET(BSP_UART1_EXT_IRQ_RTL9300))
   #define TC0_IE_RTL9300		 (1 << ICTL_OFFSET(BSP_TC0_EXT_IRQ_RTL9300))
   #define TC1_IE_RTL9300		 (1 << ICTL_OFFSET(BSP_TC1_EXT_IRQ_RTL9300))
   #define NIC_IE_RTL9300		 (1 << ICTL_OFFSET(BSP_NIC_EXT_IRQ_RTL9300))
   #define GPIO_ABCD_IE_RTL9300  (1 << ICTL_OFFSET(BSP_GPIO_ABCD_EXT_IRQ_RTL9300))
   #define SWCORE_IE_RTL9300     (1 << ICTL_OFFSET(BSP_SWCORE_EXT_IRQ_RTL9300))
   #define WDT_IP1_IE_RTL9300	 (1 << ICTL_OFFSET(BSP_WDT_IP1_IRQ_RTL9300))

#define GISR			(BSP_ICTL_BASE + 0x4)
   #define UART0_IP		(1 << ICTL_OFFSET(BSP_UART0_EXT_IRQ))
   #define UART1_IP		(1 << ICTL_OFFSET(BSP_UART1_EXT_IRQ))
   #define TC0_IP		(1 << ICTL_OFFSET(BSP_TC0_EXT_IRQ))
   #define TC1_IP		(1 << ICTL_OFFSET(BSP_TC1_EXT_IRQ))
   #define NIC_IP		(1 << ICTL_OFFSET(BSP_NIC_EXT_IRQ))
   #define GPIO_ABCD_IP	(1 << ICTL_OFFSET(BSP_GPIO_ABCD_EXT_IRQ))
   #define SWCORE_IP	(1 << ICTL_OFFSET(BSP_SWCORE_EXT_IRQ))
   #define WDT_IP1_IP	(1 << ICTL_OFFSET(BSP_WDT_IP1_IRQ))
   #define WDT_IP2_IP	(1 << ICTL_OFFSET(BSP_WDT_IP2_IRQ))

   #define UART0_IP_RTL9300	     (1 << ICTL_OFFSET(BSP_UART0_EXT_IRQ_RTL9300))
   #define UART1_IP_RTL9300	     (1 << ICTL_OFFSET(BSP_UART1_EXT_IRQ_RTL9300))
   #define TC0_IP_RTL9300		 (1 << ICTL_OFFSET(BSP_TC0_EXT_IRQ_RTL9300))
   #define TC1_IP_RTL9300		 (1 << ICTL_OFFSET(BSP_TC1_EXT_IRQ_RTL9300))
   #define NIC_IP_RTL9300		 (1 << ICTL_OFFSET(BSP_NIC_EXT_IRQ_RTL9300))
   #define GPIO_ABCD_IP_RTL9300  (1 << ICTL_OFFSET(BSP_GPIO_ABCD_EXT_IRQ_RTL9300))
   #define SWCORE_IP_RTL9300	 (1 << ICTL_OFFSET(BSP_SWCORE_EXT_IRQ_RTL9300))
   #define WDT_IP1_IP_RTL9300	 (1 << ICTL_OFFSET(BSP_WDT_IP1_IRQ_RTL9300))


   /* Interrupt Routing Selection */
#define UART0_RS       2
#define UART1_RS       1
#define TC0_RS         5
#define TC1_RS         1
#define OCPTO_RS       1
#define HLXTO_RS       1
#define SLXTO_RS       1
#define NIC_RS         4
#define GPIO_ABCD_RS   4
#define GPIO_EFGH_RS   4
#define RTC_RS         4
#define	SWCORE_RS      3
#define WDT_IP1_RS     4
#define WDT_IP2_RS     5



#define IRR0			    (BSP_ICTL_BASE + 0x8)

#define IRR0_SETTING        ((UART0_RS  << 28) | \
                             (UART1_RS  << 24) | \
                             (TC0_RS    << 20) | \
                             (TC1_RS    << 16) | \
                             (OCPTO_RS  << 12) | \
                             (HLXTO_RS  << 8)  | \
                             (SLXTO_RS  << 4)  | \
                             (NIC_RS    << 0)	\
                             )

#define IRR0_RTL9300_SETTING    ((UART1_RS  << 28) | \
                                 (UART0_RS  << 24) | \
                                 (USB_H2_RS << 16) | \
                                 (NIC_RS    << 0)	\
                                )


#define IRR1			     (BSP_ICTL_BASE + 0xc)
#define IRR1_SETTING         ((GPIO_ABCD_RS << 28) | \
                              (GPIO_EFGH_RS << 24) | \
                              (RTC_RS 	    << 20) | \
                              (SWCORE_RS	<< 16) | \
                              (WDT_IP1_RS   << 12) \
                             )

#define IRR1_RTL9300_SETTING    ((SWCORE_RS << 28))


#define IRR2			        (BSP_ICTL_BASE + 0x10)
#define IRR2_SETTING            0
#define IRR2_RTL9300_SETTING    ((TC1_RS << 0) | \
                                 (GPIO_ABCD_RS << 20) \
                                )



#define IRR3			        (BSP_ICTL_BASE + 0x14)
#define IRR3_SETTING            0
#define IRR3_RTL9300_SETTING    ((TC0_RS << 28) | \
                                 (WDT_IP1_RS << 20) \
                                )

/*
 * Timer/Counter for 9300
 */
#define TC_MAX          		  4
#define TC93XX_BASE               0xB8003200

#define RTL93XXMP_TC0DATA         (TC93XX_BASE)
#define RTL93XXMP_TC1DATA         (TC93XX_BASE + 0x10)
#define RTL93XXMP_TC2DATA         (TC93XX_BASE + 0x20)
#define RTL93XXMP_TC3DATA         (TC93XX_BASE + 0x30)

#define RTL93XXMP_TC0CNT          (TC93XX_BASE + 0x04)
#define RTL93XXMP_TC1CNT          (TC93XX_BASE + 0x14)

#define RTL93XXMP_TC0CTL          (TC93XX_BASE + 0x08)
#define RTL93XXMP_TC1CTL          (TC93XX_BASE + 0x18)
#define RTL93XXMP_TC2CTL          (TC93XX_BASE + 0x28)
#define RTL93XXMP_TC3CTL          (TC93XX_BASE + 0x38)


   #define RTL93XXMP_TCEN          (1 << 28)
   #define RTL93XXMP_TCMODE_TIMER  (1 << 24)
   #define RTL93XXMP_TCDIV_FACTOR  (0xFFFF << 0)
#define RTL93XXMP_TC0INT           (TC93XX_BASE + 0xC)
#define RTL93XXMP_TC1INT           (TC93XX_BASE + 0x1C)
   #define RTL93XXMP_TCIE          (1 << 20)
   #define RTL93XXMP_TCIP          (1 << 16)
#define RTL93XXMP_WDTCNR          (TC93XX_BASE + 0x50)

/*
 * Timer/Counter for 8390 ES & MP chip
 */
#define TC9X8X_BASE                   0xB8003100
#define TC09X8XDATA                   (TC9X8X_BASE + 0x00)

#define RTL8390MP_TC1DATA             (TC9X8X_BASE + 0x10)
#define RTL8390MP_TC0CNT              (TC9X8X_BASE + 0x04)
#define RTL8390MP_TC1CNT              (TC9X8X_BASE + 0x14)
#define RTL8390MP_TC0CTL              (TC9X8X_BASE + 0x08)
#define RTL8390MP_TC1CTL              (TC9X8X_BASE + 0x18)
   #define RTL8390MP_TCEN                 (1 << 28)
   #define RTL8390MP_TCMODE_TIMER         (1 << 24)
   #define RTL8390MP_TCDIV_FACTOR         (0xFFFF << 0)
#define RTL8390MP_TC0INT              (TC9X8X_BASE + 0xC)
#define RTL8390MP_TC1INT              (TC9X8X_BASE + 0x1C)
   #define RTL8390MP_TCIE                 (1 << 20)
   #define RTL8390MP_TCIP                 (1 << 16)
#define RTL8390MP_WDTCNR              (TC9X8X_BASE + 0x50)

/*
 * Timer/Counter for 8380 MP chip
 */

#define RTL8380MP_TC1DATA             (TC9X8X_BASE + 0x10)
#define RTL8380MP_TC0CNT              (TC9X8X_BASE + 0x04)
#define RTL8380MP_TC1CNT              (TC9X8X_BASE + 0x14)
#define RTL8380MP_TC0CTL              (TC9X8X_BASE + 0x08)
#define RTL8380MP_TC1CTL              (TC9X8X_BASE + 0x18)
   #define RTL8380MP_TCEN                 (1 << 28)
   #define RTL8380MP_TCMODE_TIMER         (1 << 24)
   #define RTL8380MP_TCDIV_FACTOR         (0xFFFF << 0)
#define RTL8380MP_TC0INT              (TC9X8X_BASE + 0xC)
#define RTL8380MP_TC1INT              (TC9X8X_BASE + 0x1C)
   #define RTL8380MP_TCIE                 (1 << 20)
   #define RTL8380MP_TCIP                 (1 << 16)
#define RTL8380MP_WDTCNR              (TC9X8X_BASE + 0x50)

#if defined(CONFIG_RTL8380_SERIES)
/* flash controller disable 4-byte address mode */
#define OTTO838x_FLASH_DISABLE_4BYTE_ADDR_MODE() \
    ({ \
        *((volatile uint32 *)(0xBB000058)) = 0x3; \
        switch(*((volatile uint32 *)(0xBB0000D0))){ \
            case 0x0:\
                break; \
            case 0x2:\
            default: \
                tmp = *((volatile uint32 *)(0xBB000FF8));\
                tmp &= ~(0xC0000000);\
                tmp |= (0x8FFFFFFF);\
                *((volatile uint32 *)(0xBB000FF8)) = tmp;\
                break; \
        }\
        *((volatile uint32 *)(0xBB000058)) = 0x0; \
    })
#endif

/************************************************************************
 * USB Host 2.0 controller
 ***********************************************************************/
#define RTK_EHCI_BASE           0xb8021000
#define RTK_EHCI_BASE_LEN       0xa4
#define RTK_USB_PHY_CTRL        0xb8000500
#define BSP_EHCI_UTMI_CTRL      (RTK_EHCI_BASE + 0xa4)

#endif /* _PLATFORM_H */
