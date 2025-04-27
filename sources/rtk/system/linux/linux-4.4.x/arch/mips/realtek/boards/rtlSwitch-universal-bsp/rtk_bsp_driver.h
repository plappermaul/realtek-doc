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
 * Purpose : RTK BSP driver
 *
 * Feature : RTK BSP driver
 *
 */

#include "rtk_type.h"

/*
 * Macro Declaration
 */

#define CONFIG_AUTO_PROBE_DRAM_SIZE
#define RTK_BSP_DEBUG


#ifdef RTK_BSP_DEBUG
#define RTK_BSP_DBG_MSG(fmt, args...)     do { printk(fmt, ##args); } while (0)
#else
#define RTK_BSP_DBG_MSG(fmt, args...)     do { } while (0)
#endif

#define BSP_MAPPER_INIT_COMPLELE    0
#define BSP_MAPPER_INIT_FAILED      -1

#define SDK_BSP_OK                  0
#define SDK_BSP_FAILED              -1
#define SDK_BSP_NOT_FOUND           -2
#define SDK_BSP_ERR_INPUT           0xF001

#define SDK_BSP_DISABLE             0
#define SDK_BSP_ENABLED             1

#define MEMORY_SIZE_64M             (64 << 20)
#define MEMORY_SIZE_128M            (128 << 20)
#define MEMORY_SIZE_256M            (256 << 20)
#define MEMORY_SIZE_512M            (512 << 20)
#define MEMORY_SIZE_1024M           (1024 << 20)
#define MEMORY_SIZE_2048M           (2048 << 20)

#define RTK_EARLY_PRINTK_BAUDRATE   115200

/*
 * Data Declaration
 */

typedef enum rtk_bsp_set_e
{
    BSP_SET_DISABLED = 0,
    BSP_SET_ENABLED,
    BSP_SET_END
} rtk_bsp_set_t;

typedef struct rtk_bsp_mapper_s {
    void   (*console_init)(uint32);
    uint32 (*uart0_phyBaseAddress)(void);
    int32  (*devIRQ_get)(uint32, uint32 *, int32 *);
    void   (*chip_reset)(void);
    void   (*deviceTree_get)(void);
    void   (*deviceTreeBus_setup)(void);
    void   (*l2Cache_init)(void);
    uint32 (*USB_irqGet)(uint32 *);
    void   (*lowMem_DMA_setup)(void);
    uint32 (*memRegion_set)(void);
    void   (*memSizeInfo_get)(uint32 *, uint32 *, uint32 *);
    void   (*cpuExtTimer_init)(void);
    void   (*cpuExtTimer_ack)(uint32);
    void   (*cpuExtTimerIntr_enable)(uint32, rtk_bsp_set_t);
    int32  (*cpuExtTimerIRQ_get)(uint32, uint32 *);
    int32  (*cpuExtTimerIRQ_restInit)(void);
    int32  (*cpuExtTimerID_get)(uint32, uint32 *);
}rtk_bsp_mapper_t;

typedef struct rtk_dev_dtsIntr_mapping_s
{
    char *dts_nodeName;        /* Which external timer will mapping to CPU clockevent */
    uint32 irq_number;
} rtk_dev_dtsIntr_mapping_t;


typedef struct rtk_cpu_extTimer_mapping_s
{
    char *extTimer_name;        /* Which external timer will mapping to CPU clockevent */
} rtk_cpu_extTimer_mapping_t;


#ifdef CONFIG_CMDLINE_BOOL
extern char rtk_preDefine_cmdline[];
#endif

/* Function Name:
 *      rtk_bspDriver_init
 * Description:
 *      Initial and Hook RTK BSP driver
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
extern int32 rtk_bspDriver_init(void);

/* Function Name:
 *      rtk_bspConsole_init
 * Description:
 *      Initial Console for UART0
 * Input:
 *      baudrate    -   Console baudrate
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *
 */
extern void rtk_bspConsole_init(uint32 baudrate);

/* Function Name:
 *      rtk_bspUart0_phyAddr_get
 * Description:
 *      Get UART0 Physical Base Address
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      UART0 Physical Base Address
 * Note:
 *
 */
extern uint32 rtk_bspUart0_phyAddr_get(void);

/* Function Name:
 *      rtk_bspChip_reset
 * Description:
 *      Reset RTK switch chip.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *
 */
extern void rtk_bspChip_reset(void);

/* Function Name:
 *      rtk_bspDeviceTree_get
 * Description:
 *      Get Device Tree
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *
 */
extern void rtk_bspDeviceTree_get(void);

/* Function Name:
 *      rtk_bspDeviceTreeBus_setup
 * Description:
 *      Register Device Tree Bus
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *
 */
extern void rtk_bspDeviceTreeBus_setup(void);

/* Function Name:
 *      rtk_bspL2Cache_init
 * Description:
 *      Inital L2 cache
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *
 */
extern void rtk_bspL2Cache_init(void);

/* Function Name:
 *      rtk_bspDev_irq_get
 * Description:
 *      Get IRQ number of SDK dev_config index .
 * Input:
 *      device_id   -   SDK dev_config index
 * Output:
 *      irq_num     -   IRQ number
 *      is_updated  -   updated IRQ number for NOT
 * Return:
 *      SDK_BSP_OK
 *      SDK_BSP_FAILED
 * Note:
 *
 */
extern int32 rtk_bspDev_irq_get(uint32 device_id, uint32 *irq_num, int32 *is_updated);


/* Function Name:
 *      rtk_bspUSB_irq_get
 * Description:
 *      Get USB Host 2.0 IRQ number.
 * Input:
 *      device_id   -   SDK dev_config index
 * Output:
 *      irq_num     -   IRQ number
 * Return:
 *      None
 * Note:
 *
 */
extern void rtk_bspUSB_irq_get(uint32 *irq_num);

/* Function Name:
 *      rtk_bspLowMem_DMA_setup
 * Description:
 *      Let USB driver can force to use low memory.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *
 */
extern void rtk_bspLowMem_DMA_setup(void);

/* Function Name:
 *      rtk_bspMemRegion_setup
 * Description:
 *      Setup Linux Memory Region
 *      for High/Normal momory.
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
extern int rtk_bspMemRegion_setup(void);

/* Function Name:
 *      rtk_bspMemSizeInfo_get
 * Description:
 *      Get Low/High/RTK_DMA size
 * Input:
 *      None
 * Output:
 *      low_memory_size     - Low Memory Size
 *      high_memory_size    - High Memory Size
 *      dma_reserved_size   - RTK DMA Size
 * Return:
 *      SDK_BSP_OK
 *      SDK_BSP_FAILED
 * Note:
 *
 */
extern int rtk_bspMemSizeInfo_get(uint32 *low_memory_size, uint32 *high_memory_size, uint32 *dma_reserved_size);

/* Function Name:
 *      rtk_cpuExtTCSrc_init
 * Description:
 *      Initial all CPUs' specific External Timers.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *
 */
extern void rtk_cpuExtTCSrc_init(void);

/* Function Name:
 *      rtk_cpuExtTCSrc_ack
 * Description:
 *      Ack for CPU specific External Timer interrupt
 * Input:
 *      cpu     - cpu index
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *
 */
extern void rtk_cpuExtTCSrc_ack(uint32 cpu);


/* Function Name:
 *      rtk_cpuExtTCSrcIntr_enable
 * Description:
 *      Enable/Disable CPU specific External Timer Interrupt
 * Input:
 *      cpu         - cpu index
 *      enable      - Enable/Disable
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *
 */
extern void rtk_cpuExtTCSrcIntr_enable(uint32 cpu, rtk_bsp_set_t enable);

/* Function Name:
 *      rtk_cpuExtTCSrcIRQ_get
 * Description:
 *      Get the CPU related External timer IRQ.
 * Input:
 *      cpu         - cpu index
 * Output:
 *      irq_num     - cpu external TC IRQ number
 * Return:
 *      SDK_BSP_OK
 *      SDK_BSP_FAILED
 * Note:
 *
 */
extern int32 rtk_cpuExtTCSrcIRQ_get(uint32 cpu, uint32 *irq_num);

/* Function Name:
 *      rtk_cpuExtTCSrcIRQ_restInit
 * Description:
 *      CPU external timer source rest initial flow.
 * Input:
 *      cpu         - cpu index
 * Output:
 *      None
 * Return:
 *      SDK_BSP_OK
 *      SDK_BSP_FAILED
 * Note:
 *      Without CPU 0.
 *
 */
extern int32 rtk_cpuExtTCSrcIRQ_restInit(void);

/* Function Name:
 *      rtk_cpuExtTCID_get
 * Description:
 *      Get CPU specific External Timer ID.
 * Input:
 *      cpu         - cpu index
 * Output:
 *      timer_id    - timer id
 * Return:
 *      SDK_BSP_OK
 *      SDK_BSP_FAILED
 * Note:
 *
 */
extern int rtk_cpuExtTCID_get(uint32 cpu, uint32 *timer_id);

