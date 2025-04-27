
#ifndef __TIMER_REG_H
#define __TIMER_REG_H


/*
 * Timer/Counter
 */
#define TC_BASE         0xB8003100
//#define TC0DATA         (TC_BASE + 0x00)
#define TC1DATA         (TC_BASE + 0x04)
   #define TCD_OFFSET      8
#define TC0CNT          (TC_BASE + 0x08)
#define TC1CNT          (TC_BASE + 0x0C)
#define TCCNR           (TC_BASE + 0x10)
   #define TC0EN           (1 << 31)
   #define TC0MODE_TIMER   (1 << 30)
   #define TC1EN           (1 << 29)
   #define TC1MODE_TIMER   (1 << 28)
#define TCIR            (TC_BASE + 0x14)
   #define TC0IE           (1 << 31)
   #define TC1IE           (1 << 30)
   #define TC0IP           (1 << 29)
   #define TC1IP           (1 << 28)
#define CDBR            (TC_BASE + 0x18)
   #define DIVF_OFFSET     16
#define WDTCNR          (TC_BASE + 0x1C)
#define WDTCLR          (1 << 23)
#define WDOG_IE         0x20000000
#define WDOG_NMI_EN     0x00000040
#define WDOG_NMI_DIS    0xFFFFFFBF

/*
 *  Timer/counter for 8380 TC & MP chip
 */
#define BSP_TIMER0_BASE		0xb8003100UL
#define BSP_TIMER0_IRQ		BSP_TC0_EXT_IRQ
#define TC0DATA			(BSP_TIMER0_BASE + 0x00)
#define RTL8380TC_TC1DATA			(BSP_TIMER0_BASE + 0x04)
   #define RTL8380TC_TCD_OFFSET		8
#define RTL8380TC_TC0CNT			(BSP_TIMER0_BASE + 0x08)
#define RTL8380TC_TC1CNT			(BSP_TIMER0_BASE + 0x0C)
#define RTL8380TC_TCCNR			(BSP_TIMER0_BASE + 0x10)
   #define RTL8380TC_TC0EN		(1 << 31)
   #define RTL8380TC_TC0MODE_TIMER	(1 << 30)
   #define RTL8380TC_TC1EN		(1 << 29)
   #define RTL8380TC_TC1MODE_TIMER	(1 << 28)
#define RTL8380TC_TCIR			(BSP_TIMER0_BASE + 0x14)
   #define RTL8380TC_TC0IE		(1 << 31)
   #define RTL8380TC_TC1IE		(1 << 30)
   #define RTL8380TC_TC0IP		(1 << 29)
   #define RTL8380TC_TC1IP		(1 << 28)
#define RTL8380TC_CDBR			(BSP_TIMER0_BASE + 0x18)
   #define RTL8380TC_DIVF_OFFSET		16
#define RTL8380TC_WDTCNR			(BSP_TIMER0_BASE + 0x1C)

#define RTL8380MP_TC1DATA          (BSP_TIMER0_BASE + 0x10)
#define RTL8380MP_TC0CNT           (BSP_TIMER0_BASE + 0x04)
#define RTL8380MP_TC1CNT           (BSP_TIMER0_BASE + 0x14)
#define RTL8380MP_TC0CTL           (BSP_TIMER0_BASE + 0x08)
#define RTL8380MP_TC1CTL           (BSP_TIMER0_BASE + 0x18)
   #define RTL8380MP_TCEN          (1 << 28)
   #define RTL8380MP_TCMODE_TIMER  (1 << 24)
   #define RTL8380MP_TCDIV_FACTOR  (0xFFFF << 0)
#define RTL8380MP_TC0INT           (BSP_TIMER0_BASE + 0xC)
#define RTL8380MP_TC1INT           (BSP_TIMER0_BASE + 0xC)
   #define RTL8380MP_TCIE          (1 << 20)
   #define RTL8380MP_TCIP          (1 << 16)
#define RTL8380MP_WDTCNR          (BSP_TIMER0_BASE + 0x50)



#endif /*__TIMER_REG_H*/

