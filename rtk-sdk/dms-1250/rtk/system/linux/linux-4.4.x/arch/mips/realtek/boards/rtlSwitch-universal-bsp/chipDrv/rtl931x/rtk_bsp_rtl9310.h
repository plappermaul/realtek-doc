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
 * Purpose : RTL9310 BSP driver
 *
 * Feature : RTL9310 BSP driver
 *
 */

#include "rtk_type.h"

/* OS */
/* Keep USB uses low memory */
#define LINUX_LOWMEM_DMA_SET            1

/* CPU */
#define RTL9310_MIPSIA_L2SIZE_OFFSET    (4)
#define RTL9310_MIPSIA_L2SIZE_MASK      (0x0f)
#define RTL9310_MIPSIA_L2_LINESIZE_0    (0x0)
#define RTL9310_MIPSIA_L2_LINESIZE_256  (0x7)
#define RTL9310_SINGLE_CPU_NUMBER       (1)

/* Memory */
#define  RTL9310_DRAM_MR6              (0xb814304c)
#define  RTL9310_DEFAULT_SIZE          (512 << 20)
#define  RTL9310_LOWMEM_START          (0)
#define  RTL9310_LOWMEM_SIZE           (256 << 20)
#define  RTL9310_HIGHMEM_START         (0x90000000)


/* RESET */
#define RTL9310_RST_GLB_CTRL            0xBB000400
#define RTL9310_SW_CHIP_RST_OFFSET      (0x0)


/* Clock */
#define RTL9310_BUS_MHZ         (200)
#define RTL9310_SYSCLK          (RTL9310_BUS_MHZ * 1000 * 1000)

/* UART */
#define RTL9310_UART0_PHY_BASE  0x18002000

#define RTL9310_BAUDRATE        115200  /* ex. 19200 or 38400 or 57600 or 115200 */

#define RTL9310_UART_DISABLE    0x00000000
#define RTL9310_UART_REG_OFFSET 24

#define RTL9310_UART0_BASE      0xB8002000
#define RTL9310_UART0_RBR       (RTL9310_UART0_BASE + 0x000)
#define RTL9310_UART0_THR       (RTL9310_UART0_BASE + 0x000)
#define RTL9310_UART0_DLL       (RTL9310_UART0_BASE + 0x000)
#define RTL9310_UART0_IER       (RTL9310_UART0_BASE + 0x004)
#define RTL9310_UART0_DLM       (RTL9310_UART0_BASE + 0x004)
#define RTL9310_UART0_IIR       (RTL9310_UART0_BASE + 0x008)
#define RTL9310_UART0_FCR       (RTL9310_UART0_BASE + 0x008)
#define RTL9310_UART0_LCR       (RTL9310_UART0_BASE + 0x00C)
#define RTL9310_UART0_MCR       (RTL9310_UART0_BASE + 0x010)
#define RTL9310_UART0_LSR       (RTL9310_UART0_BASE + 0x014)

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


/* Timer */
#define RTL9310_LXR_MHZ         200
#define DIVISOR_RTL9310         100
#define RTL9310_TC_MAX    		6 /* TC6 is reserved*/

#define RTL9310_TC_BASE         0xB8003200
#define RTL9310MP_TC0DATA       (RTL9310_TC_BASE)
#define RTL9310MP_TC0CTL        (RTL9310_TC_BASE + 0x08)
#define RTL9310MP_TC0INT        (RTL9310_TC_BASE + 0xC)


#define RTL9310MP_TCEN          (1 << 28)
#define RTL9310MP_TCMODE_TIMER  (1 << 24)
#define RTL9310MP_TCIP          (1 << 16)
#define RTL9310MP_TCIE          (1 << 20)

#define RTL9310_TC_REG_OFFSET   (0x10)

#define RTL9310_TC_NAME_SIZE    10


typedef enum rtl9310_cpu_id_e
{
    RTL9310_CPU_0 = 0,
    RTL9310_CPU_1,
    RTL9310_CPU_2,
    RTL9310_CPU_3,
    RTL9310_CPU_MAX
} rtl9310_cpu_id_t;


typedef enum rtl9310_extTC_id_e
{
    RTL9310_EXT_TIMER_0 = 0,
    RTL9310_EXT_TIMER_1,
    RTL9310_EXT_TIMER_2,
    RTL9310_EXT_TIMER_3,
    RTL9310_EXT_TIMER_4,
    RTL9310_EXT_TIMER_5,
    RTL9310_EXT_TIMER_6,
    RTL9310_EXT_TIMER_MAX
} rtl9310_extTC_id_t;

extern void init_l2_cache(void);
extern unsigned int PCI_DMA_BUS_IS_PHYS;

extern char __dtb_ms_rtl9310_begin[];


extern unsigned int rtk_bspHighMem_start;
extern unsigned int rtk_busAddrConv_enable, rtk_hiMemPhyBase_addr, rtk_hiMemLogBase_addr;

/* Function Name:
 *      rtl9310_bspMapper_init
 * Description:
 *      Initial and Hook RTL9310 BSP drivers
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      SDK_BSP_OK
 *      SDK_BSP_FAILED
 * Note:
 *
*/
extern int32 rtl9310_bspDriver_init(rtk_bsp_mapper_t *);


