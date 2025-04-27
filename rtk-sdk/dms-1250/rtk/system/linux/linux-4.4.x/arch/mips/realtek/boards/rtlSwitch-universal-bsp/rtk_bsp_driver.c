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

#include <common/rt_type.h>
#include <asm/bootinfo.h>
#include <linux/printk.h>
#include "rtk_bsp_driver.h"
#include "chipDrv/rtl931x/rtk_bsp_rtl9310.h"
#include "chipDrv/rtl930x/rtk_bsp_rtl9300.h"
#include "chipDrv/rtl839x/rtk_bsp_rtl8390.h"
#include "chipDrv/rtl838x/rtk_bsp_rtl8380.h"
#include "chip_probe.h"
#include "rtk_util.h"

/*
 * Data Declaration
 */
rtk_bsp_mapper_t    rtk_bspMapper;
uint32              rtk_bspInit_state = BSP_MAPPER_INIT_FAILED;

unsigned int        rtk_bspHighMem_start;
unsigned int        rtk_hiMemPhyBase_addr, rtk_hiMemLogBase_addr;
unsigned int        rtk_busAddrConv_enable,rtk_busAddrConv_offset;


#ifdef CONFIG_CMDLINE_BOOL
char __initdata rtk_preDefine_cmdline[COMMAND_LINE_SIZE] = CONFIG_CMDLINE;
#endif


/* Module Name : */

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
void rtk_bspConsole_init(uint32 baudrate)
{
    if(rtk_bspInit_state != BSP_MAPPER_INIT_COMPLELE)
        return;
    if(rtk_bspMapper.console_init == NULL)
        return;

    rtk_bspMapper.console_init(baudrate);
}

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
uint32 rtk_bspUart0_phyAddr_get(void)
{
    uint32 base_addr;

    if(rtk_bspInit_state != BSP_MAPPER_INIT_COMPLELE)
        return SDK_BSP_FAILED;
    if(rtk_bspMapper.console_init == NULL)
        return SDK_BSP_FAILED;

    base_addr = rtk_bspMapper.uart0_phyBaseAddress();
    return base_addr;
}

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
void rtk_bspChip_reset(void)
{
    if(rtk_bspInit_state != BSP_MAPPER_INIT_COMPLELE)
        return;
    if(rtk_bspMapper.chip_reset == NULL)
        return;

    rtk_bspMapper.chip_reset();
}

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
void rtk_bspDeviceTree_get(void)
{
    if(rtk_bspInit_state != BSP_MAPPER_INIT_COMPLELE)
        return;
    if(rtk_bspMapper.deviceTree_get == NULL)
        return;

    rtk_bspMapper.deviceTree_get();
}

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
void rtk_bspDeviceTreeBus_setup(void)
{
    if(rtk_bspInit_state != BSP_MAPPER_INIT_COMPLELE)
        return;
    if(rtk_bspMapper.deviceTreeBus_setup == NULL)
        return;

    rtk_bspMapper.deviceTreeBus_setup();
}

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
int32 rtk_bspDev_irq_get(uint32 device_id, uint32 *irq_num, int32 *is_updated)

{
    int32 ret = SDK_BSP_FAILED;

    if(rtk_bspInit_state != BSP_MAPPER_INIT_COMPLELE)
        return ret;
    if(rtk_bspMapper.devIRQ_get == NULL)
        return ret;

    ret = rtk_bspMapper.devIRQ_get(device_id, irq_num, is_updated);
    return ret;
}

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
void rtk_bspUSB_irq_get(uint32 *irq_num)
{
    if(rtk_bspInit_state != BSP_MAPPER_INIT_COMPLELE)
        return;
    if(rtk_bspMapper.USB_irqGet == NULL)
        return;

    rtk_bspMapper.USB_irqGet(irq_num);
}


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
void rtk_bspL2Cache_init(void)
{
    if(rtk_bspInit_state != BSP_MAPPER_INIT_COMPLELE)
        return;
    if(rtk_bspMapper.l2Cache_init == NULL)
        return;

    rtk_bspMapper.l2Cache_init();
}

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
 *      None.
 *
 */
void rtk_bspLowMem_DMA_setup(void)
{
    if(rtk_bspInit_state != BSP_MAPPER_INIT_COMPLELE)
        return;
    if(rtk_bspMapper.lowMem_DMA_setup == NULL)
        return;

    rtk_bspMapper.lowMem_DMA_setup();
}

/* Function Name:
 *      rtk_bspMemRegion_setup
 * Description:
 *      Setup Linux Memory Region
 *      for High/Low momory.
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
int rtk_bspMemRegion_setup(void)
{
    int ret = SDK_BSP_FAILED;
    if(rtk_bspInit_state != BSP_MAPPER_INIT_COMPLELE)
        return ret;
    if(rtk_bspMapper.memRegion_set == NULL)
        return ret;

    ret = rtk_bspMapper.memRegion_set();
    return ret;
}

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
int rtk_bspMemSizeInfo_get(uint32 *low_memory_size, uint32 *high_memory_size, uint32 *dma_reserved_size)
{
    int ret = SDK_BSP_FAILED;
    if(rtk_bspInit_state != BSP_MAPPER_INIT_COMPLELE)
        return ret;
    if(rtk_bspMapper.memSizeInfo_get == NULL)
        return ret;

    rtk_bspMapper.memSizeInfo_get(low_memory_size, high_memory_size, dma_reserved_size);
    return SDK_BSP_OK;
}


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
void rtk_cpuExtTCSrc_init(void)
{
    if(rtk_bspInit_state != BSP_MAPPER_INIT_COMPLELE)
        return;
    if(rtk_bspMapper.cpuExtTimer_init == NULL)
        return;

    rtk_bspMapper.cpuExtTimer_init();
}

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
void rtk_cpuExtTCSrc_ack(uint32 cpu)
{
    if(rtk_bspInit_state != BSP_MAPPER_INIT_COMPLELE)
        return;
    if(rtk_bspMapper.cpuExtTimer_ack == NULL)
        return;

    rtk_bspMapper.cpuExtTimer_ack(cpu);

    rtk_watchdog_kick();
}

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
void rtk_cpuExtTCSrcIntr_enable(uint32 cpu, rtk_bsp_set_t enable)
{
    if(rtk_bspInit_state != BSP_MAPPER_INIT_COMPLELE)
        return;
    if(rtk_bspMapper.cpuExtTimerIntr_enable == NULL)
        return;

    rtk_bspMapper.cpuExtTimerIntr_enable(cpu, enable);
}

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
int32 rtk_cpuExtTCSrcIRQ_get(uint32 cpu, uint32 *irq_num)
{
    int32 ret = SDK_BSP_FAILED;

    if(rtk_bspInit_state != BSP_MAPPER_INIT_COMPLELE)
        return SDK_BSP_FAILED;
    if(rtk_bspMapper.cpuExtTimerIRQ_get == NULL)
        return SDK_BSP_FAILED;

    ret = rtk_bspMapper.cpuExtTimerIRQ_get(cpu, irq_num);
    return ret;
}

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
int32 rtk_cpuExtTCSrcIRQ_restInit(void)
{
    int32 ret = SDK_BSP_FAILED;

    if(rtk_bspInit_state != BSP_MAPPER_INIT_COMPLELE)
        return SDK_BSP_FAILED;
    if(rtk_bspMapper.cpuExtTimerIRQ_restInit == NULL)
        return SDK_BSP_FAILED;

    ret = rtk_bspMapper.cpuExtTimerIRQ_restInit();

    return ret;
}

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
int rtk_cpuExtTCID_get(uint32 cpu, uint32 *timer_id)
{
    int32 ret = SDK_BSP_FAILED;

    if(rtk_bspInit_state != BSP_MAPPER_INIT_COMPLELE)
        return SDK_BSP_FAILED;
    if(rtk_bspMapper.cpuExtTimerID_get == NULL)
        return SDK_BSP_FAILED;

    ret = rtk_bspMapper.cpuExtTimerID_get(cpu, timer_id);
    return ret;
}

static void _rtk_bspMapper_clear(void)
{
    rtk_bspMapper.console_init              = NULL;
    rtk_bspMapper.uart0_phyBaseAddress      = NULL;
    rtk_bspMapper.devIRQ_get                = NULL;
    rtk_bspMapper.chip_reset                = NULL;
    rtk_bspMapper.deviceTree_get            = NULL;
    rtk_bspMapper.deviceTreeBus_setup       = NULL;
    rtk_bspMapper.l2Cache_init              = NULL;
    rtk_bspMapper.USB_irqGet                = NULL;
    rtk_bspMapper.lowMem_DMA_setup          = NULL;
    rtk_bspMapper.memRegion_set             = NULL;
    rtk_bspMapper.memSizeInfo_get           = NULL;
    rtk_bspMapper.cpuExtTimer_init          = NULL;
    rtk_bspMapper.cpuExtTimer_ack           = NULL;
    rtk_bspMapper.cpuExtTimerIntr_enable    = NULL;
    rtk_bspMapper.cpuExtTimerIRQ_get        = NULL;
    rtk_bspMapper.cpuExtTimerIRQ_restInit   = NULL;
    rtk_bspMapper.cpuExtTimerID_get         = NULL;
}

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
int32 rtk_bspDriver_init(void)
{
    uint32 chip_id = 0, chip_rev_id = 0;
    int32  ret = SDK_BSP_FAILED;

    _rtk_bspMapper_clear();

    if((ret = drv_swcore_cid_get((uint32)0, (uint32 *)&chip_id, (uint32 *)&chip_rev_id)) != 0)
    {
        RTK_BSP_DBG_MSG("\nrtk_bspMapper_init() Failed\n");
        return ret;
    }
    else
    {
        ret = SDK_BSP_FAILED;
        switch(FAMILY_ID(chip_id))
        {
            case RTL9310_FAMILY_ID:
                ret = rtl9310_bspDriver_init(&rtk_bspMapper);
                rtk_bspInit_state = BSP_MAPPER_INIT_COMPLELE;
                break;
            case RTL9300_FAMILY_ID:
                ret = rtl9300_bspDriver_init(&rtk_bspMapper);
                rtk_bspInit_state = BSP_MAPPER_INIT_COMPLELE;
                break;
            case RTL8390_FAMILY_ID:
            case RTL8350_FAMILY_ID:
                ret = rtl8390_bspDriver_init(&rtk_bspMapper);
                rtk_bspInit_state = BSP_MAPPER_INIT_COMPLELE;
                break;
            case RTL8380_FAMILY_ID:
            case RTL8330_FAMILY_ID:
                ret = rtl8380_bspDriver_init(&rtk_bspMapper);
                rtk_bspInit_state = BSP_MAPPER_INIT_COMPLELE;
                break;
            default:
                RTK_BSP_DBG_MSG("\nCHIP not support.\n");
                break;
        }
    }
    return ret;
}


