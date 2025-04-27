/*
 * Copyright 2006, Realtek Semiconductor Corp.
 *
 * rtl8196b/bsp/bspchip.h:
 *   RTL8196B chip-level header file
 *
 * Tony Wu (tonywu@realtek.com.tw)
 * Nov. 07, 2006
 */
#include <common.h>
#ifndef _BSPCHIP_H_
#define _BSPCHIP_H_

#define CONFIG_RLE0412 1

#ifdef CONFIG_UART0
#define CONFIG_USE_UART0
#else
#define CONFIG_USE_UART1
#endif

/* System registers */
#define BONDING_REG	(0xBB0001E8)
#define STRAPING_REG	(0xBB0001EC)
#define BONDING_REG_DRAMTYPE_FREQ43_FD_S	(5)	
#define BONDING_REG_DRAMTYPE_FREQ43_MASK	(3 << BONDING_REG_DRAMTYPE_FREQ43_FD_S)
#define STRAPING_REG_DRAMTYPE_FREQ210_FD_S	(22)
#define STRAPING_REG_DRAMTYPE_FREQ210_MASK	(7 << STRAPING_REG_DRAMTYPE_FREQ210_FD_S)

#define SYSREG_SYSTEM_STATUS_REG	(0xB8000044)
#define SYSREG_PIN_STATUS_REG		(0xB8000100)
#define SYSREG_LX_PLL_SEL_REG		(0xB8000228)
#define SYSREG_DRAM_CLK_EN_REG		(0xB800022c)
#define SYSREG_SYSCLK_CONTROL_REG	(0xB8000200)
#define SYSREG_MCKG_PHS_SEL_REG		(0xB8000220)
#define SYSREG_MCKG_FREQ_DIV_REG	(0xB8000224)
#define SYSREG_IP_EN_CTL_REG	        (0xB8000600)
#define SYSREG_ANA1_CTL_REG        	(0xB8000218)

#define SYSREG_SYSTEM_STATUS_CF_CKSE_OCP0_FD_S (2)
#define SYSREG_SYSTEM_STATUS_CF_CKSE_OCP0_MASK (1 << SYSREG_SYSTEM_STATUS_CF_CKSE_OCP0_FD_S)
#define SYSREG_SYSTEM_STATUS_CF_CKSE_OCP0_OCP0PLL SYSREG_SYSTEM_STATUS_CF_CKSE_OCP0_MASK
#define SYSREG_SYSTEM_STATUS_CF_CKSE_OCP0_LXPLL (0 << SYSREG_SYSTEM_STATUS_CF_CKSE_OCP0_FD_S)

#define SYSREG_PIN_STATUS_CKSEL_FD_S	(5)
#define SYSREG_PIN_STATUS_CKSEL_25MHZ	(0)
#define SYSREG_PIN_STATUS_CKSEL_40MHZ	(1)
#define SYSREG_PIN_STATUS_CKSEL_MASK	(1 << SYSREG_PIN_STATUS_CKSEL_FD_S)

#define SYSREG_DRAM_CLK_EN_FD_S		(0)
#define SYSREG_DRAM_CLK_EN_MASK		(1 << SYSREG_DRAM_CLK_EN_FD_S)
#define SYSREG_PIN_STATUS_CLSEL__FD_S	(5)
#define SYSREG_PIN_STATUS_CLSEL_MASK	(1 << SYSREG_PIN_STATUS_CLSEL__FD_S)
#define SYSREG_LX_DEFAULT_MHZ		(200)
#define SYSREG_MCKG_FREQ_DIV_FD_S    (0)
#define SYSREG_MCKG_FREQ_DIV_MASK    (0x3 << SYSREG_MCKG_FREQ_DIV_FD_S)

#define SYSREG_MCKG_PHS_SEL_PHS_FD_S	(0)
#define SYSREG_MCKG_PHS_SEL_PHS_MASK	(0x7 << SYSREG_MCKG_PHS_SEL_PHS_FD_S)
#define SYSREG_SYSCLK_CONTROL_SDPLL_FD_S (0)
#define SYSREG_SYSCLK_CONTROL_SDPLL_MASK (0x1f << SYSREG_SYSCLK_CONTROL_SDPLL_FD_S)
#define SYSREG_SYSCLK_CONTROL_OCP1PLL_FD_S (8)
#define SYSREG_SYSCLK_CONTROL_OCP1PLL_MASK (0x1f << SYSREG_SYSCLK_CONTROL_OCP1PLL_FD_S)
#define SYSREG_SYSCLK_CONTROL_OCP0PLL_FD_S (16)
#define SYSREG_SYSCLK_CONTROL_OCP0PLL_MASK (0x1f << SYSREG_SYSCLK_CONTROL_OCP0PLL_FD_S)
#define SYSREG_DDRCKODL_DDRCLM_TAP_FD_S (8)
#define SYSREG_DDRCKODL_DDRCLM_TAP_MASK (0x1f << SYSREG_DDRCKODL_DDRCLM_TAP_FD_S)

#define SYSREG_OCP0_DEFAULT_MHZ		(500)
#define SYSREG_DDRCKODL_REG		(0xB800021c)

#define SYSREG_CMUCTLR_REG		(0xB8000308)
#define SYSREG_OCP0_SMALLER_MASK	(16)
#define SYSREG_OCP1_SMALLER_MASK	(8)
#define SYSREG_LX0_SMALLER_MASK		(4)
#define SYSREG_LX1_SMALLER_MASK		(2)
#define SYSREG_LX2_SMALLER_MASK		(1)



/*
 * Register access macro
 */
#define REG32(reg)	(*(volatile unsigned int   *)((unsigned int)reg))
#define REG16(reg)	(*(volatile unsigned short *)((unsigned int)reg))
#define REG08(reg)	(*(volatile unsigned char  *)((unsigned int)reg))
#define REG8(reg)   (*(volatile unsigned char  *)((unsigned int)reg))

#define WRITE_MEM32(addr, val)   (*(volatile unsigned int *)   (addr)) = (val)
#define READ_MEM32(addr)         (*(volatile unsigned int *)   (addr))
#define WRITE_MEM16(addr, val)   (*(volatile unsigned short *) (addr)) = (val)
#define READ_MEM16(addr)         (*(volatile unsigned short *) (addr))
#define WRITE_MEM8(addr, val)    (*(volatile unsigned char *)  (addr)) = (val)
#define READ_MEM8(addr)          (*(volatile unsigned char *)  (addr))

#define PADDR(addr)  ((addr) & 0x1FFFFFFF)


/*
 * IRQ Controller
 */

/*
 *  ====================================
 *  Platform Configurable Common Options
 *  ====================================
 */

#define PROM_DEBUG      0

#define BSP_BAUDRATE      115200  /* ex. 19200 or 38400 or 57600 or 115200 */ 
                               /* For Early Debug */

            
/*                        
 * IRQ Mapping
 */
#define BSP_PTM_IRQ         31
#define BSP_LBCTMOs2_IRQ    30
#define BSP_LBCTMOs1_IRQ    29
#define BSP_PKT_IRQ         28
#define BSP_SPI_IRQ         27
#define BSP_NIC100_IRQ      26
#define BSP_SAR_IRQ         25
#define BSP_DMT_IRQ         24
#define BSP_PKT_NIC100_IRQ  23
#define BSP_PKT_NIC100MII_IRQ 22
#define BSP_GDMA_IRQ        21
#define BSP_SECURITY_IRQ    20
#define BSP_PCM_IRQ         19
#define BSP_PKT_SAR_IRQ     18
#define BSP_GPIO_EFGH_IRQ   17
#define BSP_GPIO_ABCD_IRQ   16
#define BSP_SW_IRQ          15
#define BSP_SWCORE_IRQ      BSP_SW_IRQ
#define BSP_PCIE_IRQ        14
#define BSP_UART1_IRQ       13
#define BSP_UART0_IRQ       12
#define BSP_USB_D_IRQ       11
#define BSP_USB_H_IRQ       10
#define BSP_TC1_IRQ         9
#define BSP_TC0_IRQ         8
#define BSP_LBCTMOm2_IRQ    7
#define BSP_LBCTMOm1_IRQ    6
#define BSP_SPEED_IRQ       5
#define BSP_LBCTMOs0_IRQ    4
#define BSP_LBCTMOm0_IRQ    3
#define BSP_OCPTMO_IRQ      2
#define BSP_PCIB0TO_IRQ     0


/*
 * Interrupt Routing Selection
 */
#define BSP_PTM_RS          2
#define BSP_LBCTMOs2_RS     2
#define BSP_LBCTMOs1_RS     2
#define BSP_PKT_RS          2
#define BSP_SPI_RS          2
#define BSP_NIC100_RS       2
#define BSP_SAR_RS          2
#define BSP_DMT_RS          2
#define BSP_PKT_NIC100_RS   2
#define BSP_PKT_NIC100MII_RS 2
#define BSP_PKT_SAR_RS      2
#define BSP_GDMA_RS         2
#define BSP_SECURITY_RS     2
#define BSP_PCM_RS          2
#define BSP_GPIO_EFGH_RS    2
#define BSP_GPIO_ABCD_RS    2
#define BSP_SW_RS           6
#define BSP_PCIE_RS         5
#define BSP_UART1_RS        2
#define BSP_UART0_RS        3
#define BSP_USB_D_RS        2
//cathy
//#define BSP_USB_H_RS        4
#define BSP_USB_H_RS        2
#define BSP_TC1_RS          2
#define BSP_TC0_RS          7
#define BSP_LBCTMOm2_RS     2
#define BSP_LBCTMOm1_RS     2
#define BSP_SPEED_RS        2
#define BSP_LBCTMOs0_RS     2
#define BSP_LBCTMOm0_RS     2
#define BSP_OCPTMO_RS       2
#define BSP_PCIB0TO_RS      2

#define BSP_DIVISOR         1000

#if BSP_DIVISOR > (1 << 16)
#error "Exceed the Maximum Value of DivFactor"
#endif

/*
 *  ==========================
 *  Platform Register Settings
 *  ==========================
 */

/*
 * CPU
 */
#define BSP_IMEM_BASE       0x00C00000
#define BSP_IMEM_TOP        0x00C03FFF

#define BSP_DMEM_BASE       0x00C04000
#define BSP_DMEM_TOP        0x00C05FFF

/*
 * SoC
 */
#define CHIP_ID_REG		0xB8000224
#define CHIP_ID_offset		16

/*
 * Memory Controller
 */
#define BSP_MC_MCR          0xB8001000
   #define BSP_MC_MCR_VAL      0x92A28000

#define BSP_MC_MTCR0        0xB8001004
   #define BSP_MC_MTCR0_VAL    0x12120000

#define BSP_MC_MTCR1        0xB8001008
   #define BSP_MC_MTCR1_VAL    0x00000FEB

#define BSP_MC_PFCR         0xB8001010
   #define BSP_MC_PFCR_VAL     0x00000101


#define BSP_MC_BASE         0xB8001000
#define BSP_NCR             (BSP_MC_BASE + 0x100)
#define BSP_NSR             (BSP_MC_BASE + 0x104)
#define BSP_NCAR            (BSP_MC_BASE + 0x108)
#define BSP_NADDR           (BSP_MC_BASE + 0x10C)
#define BSP_NDR             (BSP_MC_BASE + 0x110)

#define BSP_SFCR            (BSP_MC_BASE + 0x200)
#define BSP_SFDR            (BSP_MC_BASE + 0x204)

/*
 * NAND flash controller address
 */
//czyao , nand flash address
#define NAND_CTRL_BASE  0xB801A000//0xB801A000 //0xB8011000
#define NACFR  (NAND_CTRL_BASE + 0x0)
	    #define NAFC_RC 	 (1<<31)
#define NACR    (NAND_CTRL_BASE + 0x04)
        #define flash_READY  (1<<31)
        #define ECC_enable    (1<<30)
        #define RBO                  (1<<29)
        #define WBO                  (1<<28)
#define NACMR   (NAND_CTRL_BASE + 0x08)
        #define CECS1   (1<<31)
        #define CECS0   (1<<30)
        #define Chip_Seletc_Base        30
#define NAADR   (NAND_CTRL_BASE + 0x0C)
        #define enNextAD                (1<<27)
        #define AD2EN           (1<<26)
        #define AD1EN           (1<<25)
        #define AD0EN           (1<<24)
        #define CE_ADDR2                16
        #define CE_ADDR1                8
        #define CE_ADDR0                0
#define NADCRR  (NAND_CTRL_BASE + 0x10)
        #define TAG_DIS         (1<<6)
        #define DESC1           (1<<5)
        #define DESC0           (1<<4)
        #define DMARE           (1<<3)
        #define DMAWE           (1<<2)
        #define LBC_128         3
        #define LBC_64          2
        #define LBC_32          1
        #define LBC_16          0
#define NADR    (NAND_CTRL_BASE + 0x14)
#define NADFSAR (NAND_CTRL_BASE + 0x18)
#define NADRSAR (NAND_CTRL_BASE + 0x1C)
#define NASR    (NAND_CTRL_BASE + 0x20)
        #define NECN                    (1<<4)
        #define NRER                    (1<<3)
        #define NWER                    (1<<2)
        #define NDRS                    (1<<1)
        #define NDWS            (1<<0)
#define NADTSAR (NAND_CTRL_BASE + 0x54)
#define M_mask                  0xe0000000



/*
 * UART
 */
#define BSP_UART0_BASE      0xB8002000
#define BSP_UART0_MAP_BASE  0x18002000
#define BSP_UART0_RBR       (BSP_UART0_BASE + 0x000)
#define BSP_UART0_THR       (BSP_UART0_BASE + 0x000)
#define BSP_UART0_DLL       (BSP_UART0_BASE + 0x000)
#define BSP_UART0_IER       (BSP_UART0_BASE + 0x004)
#define BSP_UART0_DLM       (BSP_UART0_BASE + 0x004)
#define BSP_UART0_IIR       (BSP_UART0_BASE + 0x008)
#define BSP_UART0_FCR       (BSP_UART0_BASE + 0x008)
#define BSP_UART0_LCR       (BSP_UART0_BASE + 0x00C)
#define BSP_UART0_MCR       (BSP_UART0_BASE + 0x010)
#define BSP_UART0_LSR       (BSP_UART0_BASE + 0x014)

#define BSP_UART1_BASE      0xB8002100
#define BSP_UART1_RBR       (BSP_UART1_BASE + 0x000)
#define BSP_UART1_THR       (BSP_UART1_BASE + 0x000)
#define BSP_UART1_DLL       (BSP_UART1_BASE + 0x000)
#define BSP_UART1_IER       (BSP_UART1_BASE + 0x004)
#define BSP_UART1_DLM       (BSP_UART1_BASE + 0x004)
#define BSP_UART1_IIR       (BSP_UART1_BASE + 0x008)
#define BSP_UART1_FCR       (BSP_UART1_BASE + 0x008)
   #define BSP_FCR_EN          0x01
   #define BSP_FCR_RXRST       0x02
   #define     BSP_RXRST             0x02
   #define BSP_FCR_TXRST       0x04
   #define     BSP_TXRST             0x04
   #define BSP_FCR_DMA         0x08
   #define BSP_FCR_RTRG        0xC0
   #define     BSP_CHAR_TRIGGER_01   0x00
   #define     BSP_CHAR_TRIGGER_04   0x40
   #define     BSP_CHAR_TRIGGER_08   0x80
   #define     BSP_CHAR_TRIGGER_14   0xC0
#define BSP_UART1_LCR       (BSP_UART1_BASE + 0x00C)
   #define BSP_LCR_WLN         0x03
   #define     BSP_CHAR_LEN_5        0x00
   #define     BSP_CHAR_LEN_6        0x01
   #define     BSP_CHAR_LEN_7        0x02
   #define     BSP_CHAR_LEN_8        0x03
   #define BSP_LCR_STB         0x04
   #define     BSP_ONE_STOP          0x00
   #define     BSP_TWO_STOP          0x04
   #define BSP_LCR_PEN         0x08
   #define     BSP_PARITY_ENABLE     0x01
   #define     BSP_PARITY_DISABLE    0x00
   #define BSP_LCR_EPS         0x30
   #define     BSP_PARITY_ODD        0x00
   #define     BSP_PARITY_EVEN       0x10
   #define     BSP_PARITY_MARK       0x20
   #define     BSP_PARITY_SPACE      0x30
   #define BSP_LCR_BRK         0x40
   #define BSP_LCR_DLAB        0x80
   #define     BSP_DLAB              0x80
#define BSP_UART1_MCR       (BSP_UART1_BASE + 0x010)
#define BSP_UART1_LSR       (BSP_UART1_BASE + 0x014)
   #define BSP_LSR_DR          0x01
   #define     BSP_RxCHAR_AVAIL      0x01
   #define BSP_LSR_OE          0x02
   #define BSP_LSR_PE          0x04
   #define BSP_LSR_FE          0x08
   #define BSP_LSR_BI          0x10
   #define BSP_LSR_THRE        0x20
   #define     BSP_TxCHAR_AVAIL      0x00
   #define     BSP_TxCHAR_EMPTY      0x20
   #define BSP_LSR_TEMT        0x40
   #define BSP_LSR_RFE         0x80

#ifdef CONFIG_USE_UART1
	#define _UART_LSR      BSP_UART1_LSR
	#define _UART_FCR      BSP_UART1_FCR
	#define _UART_THR      BSP_UART1_THR
	#define _UART_LCR      BSP_UART1_LCR
	#define _UART_MCR      BSP_UART1_MCR
	#define _UART_IER      BSP_UART1_IER
	#define _UART_DLL      BSP_UART1_DLL
	#define _UART_DLM      BSP_UART1_DLM
	#define _UART_IRQ      BSP_UART1_IRQ
	#define _UART_BASE	BSP_UART1_BASE
	#define _UART_RBR	BSP_UART1_RBR
	#define _UART_IE	BSP_UART1_IE
#else //CONFIG_USE_UART0
	#define _UART_LSR      BSP_UART0_LSR
	#define _UART_FCR      BSP_UART0_FCR
	#define _UART_THR      BSP_UART0_THR
	#define _UART_LCR      BSP_UART0_LCR
	#define _UART_MCR      BSP_UART0_MCR
	#define _UART_IER      BSP_UART0_IER
	#define _UART_DLL      BSP_UART0_DLL
	#define _UART_DLM      BSP_UART0_DLM
	#define _UART_IRQ      BSP_UART0_IRQ
	#define _UART_BASE	BSP_UART0_BASE
	#define _UART_RBR      BSP_UART0_RBR
	#define _UART_IE       BSP_UART0_IE
#endif



/*
 * Interrupt Controller
 */
#define BSP_GIMR            0xB8003000
   #define BSP_PTM_IE          (1 << 31)
   #define BSP_LBCTMOs2_IE     (1 << 30)
   #define BSP_LBCTMOs1_IE     (1 << 29)
   #define BSP_PKT_IE          (1 << 28)
   #define BSP_SPI_IE          (1 << 27)
   #define BSP_NIC100_IE       (1 << 26)
   #define BSP_SAR_IE          (1 << 25)
   #define BSP_DMT_IE          (1 << 24)
   #define BSP_PKT_NIC100_IE   (1 << 23)
   #define BSP_PKT_NIC100MII_IE (1 << 22)
   #define BSP_GDMA_IE         (1 << 21)
   #define BSP_SECURITY_IE     (1 << 20)
   #define BSP_PCM_IE          (1 << 19)
   #define BSP_PKT_SAR_IE      (1 << 18)
   #define BSP_GPIO_EFGH_IE    (1 << 17)
   #define BSP_GPIO_ABCD_IE    (1 << 16)
   #define BSP_SW_IE           (1 << 15)
   #define BSP_PCIE_IE         (1 << 14)
   #define BSP_UART1_IE        (1 << 13)
   #define BSP_UART0_IE        (1 << 12)
   #define BSP_USB_D_IE        (1 << 11)
   #define BSP_USB_H_IE        (1 << 10)
   #define BSP_TC1_IE          (1 << 9)
   #define BSP_TC0_IE          (1 << 8)
   #define BSP_LBCTMOm2_IE     (1 << 7)
   #define BSP_LBCTMOm1_IE     (1 << 6)
   #define BSP_SPEED_IE        (1 << 5)
   #define BSP_LBCTMOs0_IE     (1 << 4)
   #define BSP_LBCTMOm0_IE     (1 << 3)
   #define BSP_OCPTMO_IE       (1 << 2)
//   #define BSP_PCIB1TO_IE      (1 << 1)
   #define BSP_PCIB0TO_IE      (1 << 0)

#define BSP_GISR            0xB8003004
   #define BSP_PTM_IP          (1 << 31)
   #define BSP_LBCTMOs2_IP     (1 << 30)
   #define BSP_LBCTMOs1_IP     (1 << 29)
   #define BSP_PKT_IP          (1 << 28)
   #define BSP_SPI_IP          (1 << 27)
   #define BSP_NIC100_IP       (1 << 26)
   #define BSP_SAR_IP          (1 << 25)
   #define BSP_DMT_IP          (1 << 24)
   #define BSP_PKT_NIC100_IP   (1 << 23)
   #define BSP_PKT_NIC100MII_IP (1 << 22)
   #define BSP_GDMA_IP         (1 << 21)
   #define BSP_SECURITY_IP     (1 << 20)
   #define BSP_PCM_IP          (1 << 19)
   #define BSP_PKT_SAR_IP      (1 << 18)
   #define BSP_GPIO_EFGH_IP    (1 << 17)
   #define BSP_GPIO_ABCD_IP    (1 << 16)
   #define BSP_SW_IP           (1 << 15)
   #define BSP_PCIE_IP         (1 << 14)
   #define BSP_UART1_IP        (1 << 13)
   #define BSP_UART0_IP        (1 << 12)
   #define BSP_USB_D_IP        (1 << 11)
   #define BSP_USB_H_IP        (1 << 10)
   #define BSP_TC1_IP          (1 << 9)
   #define BSP_TC0_IP          (1 << 8)
   #define BSP_LBCTMOm2_IP     (1 << 7)
   #define BSP_LBCTMOm1_IP     (1 << 6)
   #define BSP_SPEED_IP        (1 << 5)
   #define BSP_LBCTMOs0_IP     (1 << 4)
   #define BSP_LBCTMOm0_IP     (1 << 3)
   #define BSP_OCPTMO_IP       (1 << 2)
//   #define BSP_PCIB1TO_IP      (1 << 1)
   #define BSP_PCIB0TO_IP      (1 << 0)

#define BSP_IRR0            0xB8003008
#define BSP_IRR0_SETTING    ((BSP_LBCTMOm2_RS << 28) | \
                         (BSP_LBCTMOm1_RS << 24) | \
                         (BSP_SPEED_RS    << 20) | \
                         (BSP_LBCTMOs0_RS << 16) | \
                         (BSP_LBCTMOm0_RS << 12) | \
                         (BSP_OCPTMO_RS   << 8)  | \
                         (BSP_PCIB0TO_RS  << 0)    \
                        )

#define BSP_IRR1            0xB800300C
#define BSP_IRR1_SETTING    ((BSP_SW_RS    << 28) | \
                         (BSP_PCIE_RS  << 24) | \
                         (BSP_UART1_RS << 20) | \
                         (BSP_UART0_RS << 16) | \
                         (BSP_USB_D_RS << 12) | \
                         (BSP_USB_H_RS << 8)  | \
                         (BSP_TC1_RS   << 4)  | \
                         (BSP_TC0_RS   << 0)    \
                        )

#define BSP_IRR2            0xB8003010
#define BSP_IRR2_SETTING    ((BSP_PKT_NIC100_RS       << 28) | \
                         (BSP_PKT_NIC100MII_RS        << 24) | \
                         (BSP_GDMA_RS      << 20) | \
                         (BSP_SECURITY_RS  << 16) | \
                         (BSP_PCM_RS       << 12) | \
                         (BSP_PKT_SAR_RS	<< 8) | \
                         (BSP_GPIO_EFGH_RS << 4)  | \
                         (BSP_GPIO_ABCD_RS << 0)    \
                        )

#define BSP_IRR3            0xB8003014
#define BSP_IRR3_SETTING    ((BSP_PTM_RS      << 28) | \
                         (BSP_LBCTMOs2_RS << 24) | \
                         (BSP_LBCTMOs1_RS << 20) | \
                         (BSP_PKT_RS      << 16) | \
                         (BSP_SPI_RS      << 12) | \
                         (BSP_NIC100_RS   << 8)  | \
                         (BSP_SAR_RS      << 4)  | \
                         (BSP_DMT_RS      << 0)    \
                        )
/*
 * Timer/Counter
 */
#define BSP_TC_BASE         0xB8003100
#define BSP_TC0DATA         (BSP_TC_BASE + 0x00)
#define BSP_TC1DATA         (BSP_TC_BASE + 0x04)
   #define BSP_TCD_OFFSET      8
#define BSP_TC0CNT          (BSP_TC_BASE + 0x08)
#define BSP_TC1CNT          (BSP_TC_BASE + 0x0C)
#define BSP_TCCNR           (BSP_TC_BASE + 0x10)
   #define BSP_TC0EN           (1 << 31)
   #define BSP_TC0MODE_TIMER   (1 << 30)
   #define BSP_TC1EN           (1 << 29)
   #define BSP_TC1MODE_TIMER   (1 << 28)
#define BSP_TCIR            (BSP_TC_BASE + 0x14)
   #define BSP_TC0IE           (1 << 31)
   #define BSP_TC1IE           (1 << 30)
   #define BSP_TC0IP           (1 << 29)
   #define BSP_TC1IP           (1 << 28)
#define BSP_CDBR            (BSP_TC_BASE + 0x18)
   #define BSP_DIVF_OFFSET     16
#define BSP_WDTCNR          (BSP_TC_BASE + 0x1C)

#define WDTE_OFFSET                         24              /* Watchdog enable */
#define WDSTOP_PATTERN                      0xA5            /* Watchdog stop pattern */
#define WDTCLR                              (1 << 23)       /* Watchdog timer clear */
#define OVSEL_13                            0               /* Overflow select count 2^13 */


/*
 * System Clock
 */
#ifdef CONFIG_RLE0412
#define BSP_SCCR	0xB8000200
#else //CONFIG_RLE0412
#define BSP_SCCR	0xB8003200
#define BSP_PLL_CTRL	(BSP_SCCR + 0x04)
#define BSP_PLL2_CTRL	(BSP_SCCR + 0x08)
#define BSP_ANA1_CTRL	(BSP_SCCR + 0x14)
#define BSP_PLL3_CTRL	(BSP_SCCR + 0x18)
#define BSP_LDO_CTRL	(BSP_SCCR + 0x20)
#endif //CONFIG_RLE0412

/*
 * System MISC Control Register
 */
#ifdef CONFIG_RLE0412
#define BSP_MISC_CR_BASE		0xB8000300
#define BSP_MISC_PINMUX			(BSP_MISC_CR_BASE + 0x00)
	#define BSP_NANDF_GPIO			(1 << 30)					//0:gpio B4~7, C0~7, and D0~2, 1:NAND flash pins
	#define BSP_USBLED0_GPF5		(1 << 27)					//0:gpio F5, 1:usbled0
	#define BSP_USBLED1_GPF6		(1 << 26)					//0:gpio F6, 1:usbled1
	#define BSP_LEDS0_GPH4			(1 << 25)					//0:gpio H4, 1:LEDS0
	#define BSP_LEDS1_GPH5			(1 << 24)					//0:gpio H5, 1:LEDS1
	#define BSP_LEDS2_GPH6			(1 << 23)					//0:gpio H6, 1:LEDS2
	#define BSP_LEDS3_GPH7			(1 << 22)					//0:gpio H7, 1:LEDS3
	#define BSP_LEDP0_GPF0			(1 << 20)					//0:gpio F0, 1:LEDF0
	#define BSP_UR0_GPG0G1			(1 << 15)					//0:gpio G0 G1, 1:UART0 tx/rx
	#define BSP_CK25M_UR0			(1 << 12)					//0:UART0 tx/s2p/gpio, 1:CK25MOUT
	#define BSP_UART1_SEL			(3 << 8)
	#define BSP_VOIP_GPIO_PINMUX_G0 (1 << 5)
	#define BSP_VOIP_GPIO_PINMUX_G1 (1 << 4)
	#define BSP_JTAG_GPIO_PINMUX	(1 << 2)
#define BSP_MISC_PINOCR			(BSP_MISC_CR_BASE + 0x04)
	#define BSP_NAND_ADDR_MASK		(3 << 7)					// mask of the address cycle					
	#define BSP_BOOT_FLASH_TYPE	(1 << 6)					//0:spi/nor flash, 1:nand flash
#define BSP_MISC_PINSR			(BSP_MISC_CR_BASE + 0x08)
	#define BSP_CLKSEL				(1 << 23)					//0:35.328MHZ, 1:40MHZ
	#define BSP_BOOT_FLASH_STS		(1 << 5)					//0:nor flash, 1:spi flash
#define BSP_IP_SEL				(BSP_MISC_CR_BASE + 0x0C)
	#define BSP_EN_XSI_VOIP_SPI		(1 << 18)
	#define BSP_EN_NEW_VOIP_SPI		(1 << 17)
	#define BSP_EN_ISI_VOIP_SPI		(1 << 16)
	#define BSP_EN_VOIPACC			(1 << 13)
	#define BSP_EN_PCIE				(1 << 9)
	#define BSP_EN_PCM				(1 << 8)
	#define BSP_EN_USB_PHY0 		(1 << 7)
	#define BSP_EN_USB_PHY1 		(1 << 6)
	#define BSP_EN_USB_HOST 		(1 << 5)
	#define BSP_EN_USB_OTG			(1 << 4)
#define BSP_MISC_IO_DRIVING		(BSP_MISC_CR_BASE + 0x14)
#define BSP_USB_PHY_CTRL		(BSP_MISC_CR_BASE + 0x18)
	#define BSP_USB_UTMI_RESET0		(1 << 28)
	#define BSP_USB_UTMI_RESET1		(1 << 12)

#else //CONFIG_RLE0412
#define BSP_MISC_CR_BASE		0xB8003300
#define BSP_MISC_PINMUX			(BSP_MISC_CR_BASE + 0x00)
	#define BSP_JTAG_GPIO_PINMUX	(1 << 31)
	#define BSP_UART1_SEL			(3 << 20)
#define BSP_MISC_PINOCR			(BSP_MISC_CR_BASE + 0x04)
#define BSP_MISC_PINSR			(BSP_MISC_CR_BASE + 0x08)
	#define BSP_EN_USB_HOST			(1 << 23)
	#define BSP_EN_USB_OTG			(1 << 22)
	#define BSP_CLKSEL				(1 << 21)					//0:25MHZ, 1:35.328MHZ
#define BSP_IP_SEL				(BSP_MISC_CR_BASE + 0x0C)
	#define BSP_EN_PCIE				(1 << 9)
	#define BSP_EN_USB_PHY0 		(1 << 7)
	#define BSP_EN_USB_PHY1 		(1 << 6)
#define BSP_MISC_IO_DRIVING		(BSP_MISC_CR_BASE + 0x10)
#define BSP_USB_PHY_CTRL		(BSP_MISC_CR_BASE + 0x14)
	#define BSP_USB_UTMI_RESET0		(1 << 28)
	#define BSP_USB_UTMI_RESET1		(1 << 12)

#endif //CONFIG_RLE0412

#ifdef CONFIG_RLE0412
#define BSP_PCI_MISC				0xB8000400
#else
#define BSP_PCI_MISC				0xB8003400
#endif

/* 
 *GPIO control registers 
*/
#define GPIOCR_BASE 0xB8003500
/*Port A,B,C,D*/
#define GPIO_PABCD_CNR		(GPIOCR_BASE+0x00)	/*Port A,B,C,D control register*/
#define GPIO_PABCD_PTYPE	(GPIOCR_BASE+0x04)	/*Port A,B,C,D peripheral type control register*/
#define GPIO_PABCD_DIR		(GPIOCR_BASE+0x08)	/*Port A,B,C,D direction */
#define GPIO_PABCD_DAT		(GPIOCR_BASE+0x0C)	/*Port A,B,C,D data register*/
#define GPIO_PABCD_ISR		(GPIOCR_BASE+0x10)	/*Port A,B,C,D interrupt status register*/
#define GPIO_PAB_IMR		(GPIOCR_BASE+0x14)	/*Port A,B interrupt mask register*/
#define GPIO_PCD_IMR		(GPIOCR_BASE+0x18)	/*Port C,D interrupt mask register*/
#define GPIO_PEFGH_DIR		(GPIOCR_BASE+0x24)	/*Port E,F,G,H direction */
#define GPIO_PEFGH_DAT		(GPIOCR_BASE+0x28)	/*Port E,F,G,H data register*/
#define GPIO_PEFGH_ISR		(GPIOCR_BASE+0x2c)	/*Port E,F,G,H interrupt status register*/
#define GPIO_PEF_IMR		(GPIOCR_BASE+0x30)	/*Port E,F interrupt mask register*/
#define GPIO_PGH_IMR		(GPIOCR_BASE+0x34)	/*Port G,H interrupt mask register*/


/*Port A*/
#define GPIO_PADIR	(GPIOCR_BASE+0x00L)	/*Port A direction register*/
#define GPIO_PADAT	(GPIOCR_BASE+0x04L)	/*Port A data register*/
#define GPIO_PAISR	(GPIOCR_BASE+0x08L)	/*Port A interrupt status register*/
#define GPIO_PAIMR	(GPIOCR_BASE+0x0CL)	/*Port A interrupt mask register*/
/*Port B*/
#define GPIO_PBDIR	(GPIOCR_BASE+0x10L)	/*Port B direction register*/
#define GPIO_PBDAT	(GPIOCR_BASE+0x14L)	/*Port B data register*/
#define GPIO_PBISR	(GPIOCR_BASE+0x18L)	/*Port B interrupt status register*/
#define GPIO_PBIMR	(GPIOCR_BASE+0x1CL)	/*Port B interrupt mask register*/

/* ADSL State */
#define C_AMSW_IDLE                0
#define C_AMSW_L3                  1
#define C_AMSW_ACTIVATING          3
#define C_AMSW_SLOWBLINK_STATE     4
#define C_AMSW_INITIALIZING        6
#define C_AMSW_SHOWTIME_L0         9
#define C_AMSW_END_OF_LD          15

/*
* USB
*/
#define BSP_EHCI_BASE			0xB8021000
#define BSP_EHCI_UTMI_CTRL		(BSP_EHCI_BASE + 0xA4)

#define BSP_DWC_OTG_BASE		0xB8030000
#define BSP_DWC_OTG_PHY_CTRL	(BSP_DWC_OTG_BASE + 0x34)

#ifdef CONFIG_RLE0412
#define USBLED	0xB800031C
#else //CONFIG_RLE0412
#define USBLED	0xB8003328
#define RTL8672_USB_HOST_TX_SHOULD_CHECK //0412 has fixed this bug
#endif //CONFIG_RLE0412 

#ifdef CONFIG_USB_OTG_HOST_RTL8672
#ifndef CONFIG_RLE0412
#define RTL8672_OTG_ROOT_HUB_SPEED_SWITCH   //switch to full speed when high speed hub is connected
#endif
#endif

/*
 * PCIE Host Controller
 */
#define BSP_PCIE0_H_CFG     0xB8B00000
#define BSP_PCIE0_H_EXT     0xB8B01000
#define BSP_PCIE0_H_MDIO    (BSP_PCIE0_H_EXT + 0x00)
#define BSP_PCIE0_H_INTSTR  (BSP_PCIE0_H_EXT + 0x04)
#define BSP_PCIE0_H_PWRCR   (BSP_PCIE0_H_EXT + 0x08)
#define BSP_PCIE0_H_IPCFG   (BSP_PCIE0_H_EXT + 0x0C)
#define BSP_PCIE0_H_MISC    (BSP_PCIE0_H_EXT + 0x10)
#define BSP_PCIE0_D_CFG0    0xB8B10000
#define BSP_PCIE0_D_CFG1    0xB8B11000
#define BSP_PCIE0_D_MSG     0xB8B12000

#define BSP_PCIE1_H_CFG     0xB8B20000
#define BSP_PCIE1_H_EXT     0xB8B21000
#define BSP_PCIE1_H_MDIO    (BSP_PCIE1_H_EXT + 0x00)
#define BSP_PCIE1_H_INTSTR  (BSP_PCIE1_H_EXT + 0x04)
#define BSP_PCIE1_H_PWRCR   (BSP_PCIE1_H_EXT + 0x08)
#define BSP_PCIE1_H_IPCFG   (BSP_PCIE1_H_EXT + 0x0C)
#define BSP_PCIE1_H_MISC    (BSP_PCIE1_H_EXT + 0x10)
#define BSP_PCIE1_D_CFG0    0xB8B30000
#define BSP_PCIE1_D_CFG1    0xB8B31000
#define BSP_PCIE1_D_MSG     0xB8B32000

#define BSP_PCIE0_D_IO      0xB8C00000
#define BSP_PCIE1_D_IO      0xB8E00000
#define BSP_PCIE0_D_MEM     0xB9000000
#define BSP_PCIE1_D_MEM     0xBA000000

#ifdef CONFIG_RTL8192CD
//b[31:24]=GPIO_MOD[7:0], b[23:16]=GPIO_IO_SEL[7:0], b[15:8]=GPIO_OUT[]7:0], b[7:0]=GPIO_IN[7:0]
#define RTL8192CD_GPIO_PIN_CTRL		(BSP_PCIE0_D_MEM + 0x44)
	#define RTL8192CD_GPIO_IN_OFFSET		0
	#define RTL8192CD_GPIO_OUT_OFFSET		8
	#define RTL8192CD_GPIO_IO_SEL_OFFSET	16
#endif //CONFIG_RTL8192CD

/*
 * chip ID detection
 */
#if 1 //def CONFIG_RLE0412
#define IS_6028A		0
#define IS_6028B		0
#define IS_6085			0
#define IS_RLE0315		0
#define IS_6166			0
#define IS_8676V1		(REG32(CHIP_ID_REG)>>CHIP_ID_offset == 0x0412)
#define IS_8676V2		(REG32(CHIP_ID_REG)>>CHIP_ID_offset == 0x6239)
#define IS_RTL8676		(IS_8676V2 || IS_8676V1)
#else //CONFIG_RLE0412
#define IS_6028A 		(((REG32(BSP_SCCR) & 0x00f10000) == 0x00f10000) ? 1:0) //b21=1, b20=1, b16=1
#define IS_6028B 		(((REG32(BSP_SCCR) & 0x00f10000) == 0x00f00000) ? 1:0) //b21=1, b20=1, b16=0
#define IS_6085 		(((REG32(BSP_SCCR) & 0x00f10000) == 0x00e10000) ? 1:0) //b21=1, b20=0, b16=1
#define IS_RLE0315		(((REG32(BSP_SCCR) & 0x00f10000) == 0x00d10000) ? 1:0) //b21=0, b20=1, b16=1
#define IS_6166	 		IS_RLE0315
#define IS_0412			0
#define IS_RTL8676		0
#endif //CONFIG_RLE0412

#endif   /* _BSPCHIP_H */
