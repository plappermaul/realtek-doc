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
 * Purpose : RTL8380 BSP driver
 *
 * Feature : RTL8380 BSP driver
 *
 */

#include "rtk_type.h"

/* OS */
/* Keep USB uses low memory */
#define LINUX_LOWMEM_DMA_SET            1

/* CPU */


/* Memory */
#define  RTL8380_DEFAULT_SIZE           (128 << 20)
#define  RTL8380_LOWMEM_START           (0)
#define  RTL8380_LOWMEM_SIZE            (256 << 20)
#define  RTL8380_NO_HIGHMEM_SIZE        (0)


/* RESET */
#define RTL8380_RST_GLB_CTRL_1          0xBB000040
#define RTL8380_SW_CHIP_RST_OFFSET      (0x0)

/* WDT */
#define RTL8380_WDTINTRR                0xB8003154
#define RTL8380_WDTINTRR_PH1_IP_OFFSET  (31)


/* Clock */
#define RTL8380_BUS_MHZ         (200)
#define RTL8380_SYSCLK          (RTL8380_BUS_MHZ * 1000 * 1000)

/* UART */
#define RTL8380_UART0_PHY_BASE  0x18002000

#define RTL8380_BAUDRATE        115200  /* ex. 19200 or 38400 or 57600 or 115200 */

#define RTL8380_UART_DISABLE    0x00000000
#define RTL8380_UART_REG_OFFSET 24

#define RTL8380_UART0_BASE      0xB8002000
#define RTL8380_UART0_RBR       (RTL8380_UART0_BASE + 0x000)
#define RTL8380_UART0_THR       (RTL8380_UART0_BASE + 0x000)
#define RTL8380_UART0_DLL       (RTL8380_UART0_BASE + 0x000)
#define RTL8380_UART0_IER       (RTL8380_UART0_BASE + 0x004)
#define RTL8380_UART0_DLM       (RTL8380_UART0_BASE + 0x004)
#define RTL8380_UART0_IIR       (RTL8380_UART0_BASE + 0x008)
#define RTL8380_UART0_FCR       (RTL8380_UART0_BASE + 0x008)
#define RTL8380_UART0_LCR       (RTL8380_UART0_BASE + 0x00C)
#define RTL8380_UART0_MCR       (RTL8380_UART0_BASE + 0x010)
#define RTL8380_UART0_LSR       (RTL8380_UART0_BASE + 0x014)

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
#define RTL8380_LXR_MHZ         200
#define DIVISOR_RTL8380         100

#define RTL8380_TC_BASE         0xB8003100
#define RTL8380MP_TC0DATA       (RTL8380_TC_BASE)
#define RTL8380MP_TC0CTL        (RTL8380_TC_BASE + 0x08)
#define RTL8380MP_TC0INT        (RTL8380_TC_BASE + 0xC)


#define RTL8380MP_TCEN          (1 << 28)
#define RTL8380MP_TCMODE_TIMER  (1 << 24)
#define RTL8380MP_TCIP          (1 << 16)
#define RTL8380MP_TCIE          (1 << 20)

#define RTL8380_TC_REG_OFFSET   (0x10)

#define RTL8380_TC_NAME_SIZE    10


typedef enum rtl8380_cpu_id_e
{
    RTL8380_CPU_0 = 0,
    RTL8380_CPU_MAX
} rtl8380_cpu_id_t;


typedef enum rtl8380_extTC_id_e
{
    RTL8380_EXT_TIMER_0 = 0,
    RTL8380_EXT_TIMER_1,
    RTL8380_EXT_TIMER_2,
    RTL8380_EXT_TIMER_3,
    RTL8380_EXT_TIMER_4,
    RTL8380_EXT_TIMER_MAX
} rtl8380_extTC_id_t;

typedef enum rtl8380_cpuIP_id_e
{
    RTL8380_CPUIP_ID_0 = 0,
    RTL8380_CPUIP_ID_1,
    RTL8380_CPUIP_ID_2,
    RTL8380_CPUIP_ID_3,
    RTL8380_CPUIP_ID_4,
    RTL8380_CPUIP_ID_5,
    RTL8380_CPUIP_ID_6,
    RTL8380_CPUIP_ID_7,
    RTL8380_CPUIP_ID_MAX,
} rtl8380_cpuIP_id_t;

typedef struct rtl8380_intr_mapping_conf_s
{
    rtl8380_cpuIP_id_t          cpu_ip_id;
    unsigned int                mapped_system_irq;
} rtl8380_intr_mapping_conf_t;


extern char __dtb_ms_rtl8380_begin[];

extern unsigned int rtk_bspHighMem_start;
extern unsigned int rtk_busAddrConv_enable, rtk_hiMemPhyBase_addr, rtk_hiMemLogBase_addr;

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

/* Function Name:
 *      rtl8380_bspMapper_init
 * Description:
 *      Initial and Hook RTL8380 BSP drivers
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
extern int32 rtl8380_bspDriver_init(rtk_bsp_mapper_t *);


