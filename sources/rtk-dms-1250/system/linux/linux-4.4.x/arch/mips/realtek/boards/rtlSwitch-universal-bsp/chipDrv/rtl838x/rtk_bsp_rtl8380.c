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

#include <common/rt_type.h>
#include <linux/types.h>
#include <linux/of_fdt.h>
#include <linux/of_platform.h>
#include <linux/of_irq.h>
#include <linux/string.h>
#include <linux/interrupt.h>
#include <linux/slab.h>

#include <asm/prom.h>
#include <asm/mipsregs.h>

#include "dev_config.h"
#include "rtk_bsp_driver.h"
#include "rtk_bsp_rtl8380.h"
#include "rtk_bsp_rtl8380_intr.h"

#include <linux/serial.h>
#include <linux/serial_core.h>
#include <linux/serial_8250.h>
#include <linux/serial_reg.h>
#include <linux/tty.h>


rtk_dev_dtsIntr_mapping_t rtl8380_dev_dtsIntr[] =
{
    [RTK_DEV_UART0] =
    {
        .dts_nodeName = RTL8380_UART0,
        .irq_number = RTL8380_UART0_IRQ,
    },

    [RTK_DEV_UART1] =
    {
        .dts_nodeName = RTL8380_UART1,
        .irq_number = RTL8380_UART1_IRQ,
    },

    [RTK_DEV_TC0] =
    {
        .dts_nodeName = RTL8380_TC0,
        .irq_number = RTL8380_TC0_IRQ,
    },

    [RTK_DEV_TC1] =
    {
        .dts_nodeName = RTL8380_TC1,
        .irq_number = RTL8380_TC1_IRQ,
    },
    [RTK_DEV_NIC] =
    {
        .dts_nodeName = RTL8380_NIC,
        .irq_number = RTL8380_NIC_IRQ,
    },

    [RTK_DEV_SWCORE] =
    {
        .dts_nodeName = RTL8380_SWCORE,
        .irq_number = RTL8380_SWCORE_IRQ,
    },
    [RTK_DEV_GPIO_ABCD] =
    {
        .dts_nodeName = RTL8380_GPIO,
        .irq_number = RTL8380_GPIO_IRQ,
    },
    [RTK_DEV_TC2] =
    {
        .dts_nodeName = RTL8380_TC2,
        .irq_number = RTL8380_TC2_IRQ,
    },
    [RTK_DEV_TC3] =
    {
        .dts_nodeName = RTL8380_TC3,
        .irq_number = RTL8380_TC3_IRQ,
    },
    [RTK_DEV_TC4] =
    {
        .dts_nodeName = RTL8380_TC4,
        .irq_number = RTL8380_TC4_IRQ,
    },
};

rtk_cpu_extTimer_mapping_t rtl8380_cpu_extTimer[] =
{
    [RTL8380_CPU_0] =
    {
        .extTimer_name = RTL8380_TC2,
    },
};

uint32 rtl8380_cpuToExtTC_mapping[RTL8380_CPU_MAX];
uint32 rtl8380_lowMemSize, rtl8380_dmaReservedSize;

/* Function Name:
 *      _rtl8380_cpuExtTimerID_get
 * Description:
 *      Get CPU specific External Timer ID.
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
static int _rtl8380_cpuExtTimerID_get(uint32 cpu, uint32 *timer_id)
{
    if(cpu >= RTL8380_CPU_MAX)
        return SDK_BSP_FAILED;

    *timer_id = rtl8380_cpuToExtTC_mapping[cpu];

    return SDK_BSP_OK;
}

/* Function Name:
 *      _rtl8380_cpuExtTimer_mapping
 * Description:
 *      Setup CPU to External Timer mapping table.
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
static int _rtl8380_cpuExtTimer_mapping(void)
{
    uint32              cpu_idx, timer_idx;
    int                 ret = 0;
    char                *extTimer_name;

    extTimer_name = kmalloc(RTL8380_TC_NAME_SIZE, GFP_NOWAIT);
    if(extTimer_name == NULL)
    {
        RTK_BSP_DBG_MSG("\n[%s][%d] Timer Name buffer allcoation failed\n",__FUNCTION__,__LINE__);
        return SDK_BSP_FAILED;
    }

    for(cpu_idx = 0; cpu_idx < RTL8380_CPU_MAX; cpu_idx++)
    {
        for(timer_idx = 0; timer_idx < RTL8380_EXT_TIMER_MAX; timer_idx++)
        {
            memset(extTimer_name, 0, RTL8380_TC_NAME_SIZE);
            ret = sprintf(extTimer_name,"rtk_tc%d",timer_idx);
            if(ret == 0)
            {
                RTK_BSP_DBG_MSG("\n[%s][%d] Timer Name buffer NOT fill\n",__FUNCTION__,__LINE__);
                {
                    if(extTimer_name != NULL)
                        kfree(extTimer_name);
                    return SDK_BSP_FAILED;
                }
            }
            ret = strcmp(rtl8380_cpu_extTimer[cpu_idx].extTimer_name, extTimer_name);
            if(ret == 0)
            {
                rtl8380_cpuToExtTC_mapping[cpu_idx] = timer_idx;
                break;
            }
        }
    }

    if(extTimer_name != NULL)
        kfree(extTimer_name);

    return SDK_BSP_OK;
}

/* Function Name:
 *      rtl8380_cpuExtTimerID_get
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
int rtl8380_cpuExtTimerID_get(uint32 cpu, uint32 *timer_id)
{
    int ret = SDK_BSP_FAILED;

    if(cpu >= RTL8380_CPU_MAX)
        return SDK_BSP_FAILED;
    if(timer_id == NULL)
        return SDK_BSP_FAILED;

    ret = _rtl8380_cpuExtTimerID_get(cpu, timer_id);

    return ret;
}


/* Function Name:
 *      rtl8380_cpuExtTimer_init
 * Description:
 *      Initial all CPU specific External Timer.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *
 */
void rtl8380_cpuExtTimer_init(void){
    int cpu_idx, timer_id, offset;
    int TCDATA = 0;
    int TCCTL = 0;
    int ret;

    ret = _rtl8380_cpuExtTimer_mapping();
    if(ret != SDK_BSP_OK)
        return;

    TCDATA = ( (RTL8380_LXR_MHZ * 1000000)/ ((int) DIVISOR_RTL8380 * HZ) );

    for(cpu_idx = 0; cpu_idx < RTL8380_CPU_MAX; cpu_idx ++)
    {
        ret = _rtl8380_cpuExtTimerID_get(cpu_idx, &timer_id);
        if(ret != SDK_BSP_OK)
        {
            RTK_BSP_DBG_MSG("[%s](CPU %d)External Timer Initial failed!!!\n",__FUNCTION__,cpu_idx);
            return;
        }
        offset = (timer_id*RTL8380_TC_REG_OFFSET);
        if (RTK_BSP_REG32(RTL8380MP_TC0INT + offset) & RTL8380MP_TCIP){
            RTK_BSP_REG32(RTL8380MP_TC0INT + offset) |= RTL8380MP_TCIP;
        }
        /* disable timer before setting CDBR */
        RTK_BSP_REG32(RTL8380MP_TC0CTL + offset) = 0; /* disable timer before setting CDBR */
        RTK_BSP_REG32(RTL8380MP_TC0DATA + offset ) =  TCDATA;
    }

    /* Enable timer for all CPU at one time. Let the count of all timer is near */
    TCCTL = RTL8380MP_TCEN | RTL8380MP_TCMODE_TIMER | DIVISOR_RTL8380 ;

    for(cpu_idx = 0; cpu_idx < RTL8380_CPU_MAX; cpu_idx++)
    {
        ret = _rtl8380_cpuExtTimerID_get(cpu_idx, &timer_id);
        if(ret != SDK_BSP_OK)
        {
            RTK_BSP_DBG_MSG("[%s](CPU %d)External Timer Initial failed!!!\n",__FUNCTION__,cpu_idx);
            return;
        }
        offset = (timer_id*RTL8380_TC_REG_OFFSET);
        RTK_BSP_REG32(RTL8380MP_TC0CTL+ offset) = TCCTL;
    }
}

/* Function Name:
 *      rtl8380_cpuExtTimerIntr_enable
 * Description:
 *      Enable CPU specific External Timer interrupt.
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
void rtl8380_cpuExtTimerIntr_enable(uint32 cpu, rtk_bsp_set_t enable)
{
    int ret = SDK_BSP_FAILED;
    uint32 timer_id;

    ret = _rtl8380_cpuExtTimerID_get(cpu, &timer_id);
    if(ret != SDK_BSP_OK)
        RTK_BSP_DBG_MSG("[%s](CPU %d)_rtl8380_cpuTimer_mapping failed\n",__FUNCTION__,cpu);

    if(enable == BSP_SET_ENABLED)
        RTK_BSP_REG32((RTL8380MP_TC0INT)+(timer_id * RTL8380_TC_REG_OFFSET)) |= RTL8380MP_TCIE;
    else
        RTK_BSP_REG32((RTL8380MP_TC0INT)+(timer_id * RTL8380_TC_REG_OFFSET)) &= ~(RTL8380MP_TCIE);
}

/* Function Name:
 *      rtl8380_cpuExtTimer_ack
 * Description:
 *      CPU specific External Timer ack function.
 * Input:
 *      cpu         - cpu index
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *
 */
void rtl8380_cpuExtTimer_ack(uint32 cpu)
{
    int ret = SDK_BSP_FAILED;
    uint32 offset, timer_id;

    if(cpu >= RTL8380_CPU_MAX)
        return;

    ret = _rtl8380_cpuExtTimerID_get(cpu, &timer_id);
    if(ret != SDK_BSP_OK)
        RTK_BSP_DBG_MSG("[%s](CPU %d)_rtl8380_cpuTimer_mapping\n",__FUNCTION__,cpu);

    offset = (timer_id * RTL8380_TC_REG_OFFSET);

    RTK_BSP_REG32(RTL8380MP_TC0INT + offset) |= RTL8380MP_TCIP;

}

/* Function Name:
 *      rtl8380_cpuExtTimerIRQ_get
 * Description:
 *      Get the CPU specific External Timer IRQ.
 * Input:
 *      cpu         - cpu index
 * Output:
 *      irq_num     - External Timer IRQ
 * Return:
 *      SDK_BSP_OK
 *      SDK_BSP_FAILED
 * Note:
 *
 */
int32 rtl8380_cpuExtTimerIRQ_get(uint32 cpu, uint32 *irq_num)
{
    int ret = SDK_BSP_FAILED;
    int loop_idx = 0;

    if(irq_num == NULL)
        return ret;

    for(loop_idx = 0; loop_idx < (sizeof(rtl8380_dev_dtsIntr)/sizeof(rtk_dev_dtsIntr_mapping_t)); loop_idx++)
    {
        if(rtl8380_dev_dtsIntr[loop_idx].dts_nodeName != NULL)
        {
            ret = strcmp(rtl8380_cpu_extTimer[cpu].extTimer_name, rtl8380_dev_dtsIntr[loop_idx].dts_nodeName);
            if(ret == 0)
            {
               *irq_num = rtl8380_dev_dtsIntr[loop_idx].irq_number;
                return SDK_BSP_OK;
            }
        }
    }
    return SDK_BSP_FAILED;
}

/* Function Name:
 *      rtl8380_cpuExtTimerIRQ_restInit
 * Description:
 *      Do nothing
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      SDK_BSP_OK
 *      SDK_BSP_FAILED
 * Note:
 *      None
 */
int32 rtl8380_cpuExtTimerIRQ_restInit(void)
{
    return SDK_BSP_OK;
}


/* Function Name:
 *      rtl8380_uart0_phyBaseAddress
 * Description:
 *      Get RTL8380 UART0 Physical Base Address.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RTL8380 UART0 Phyical Base Address
 * Note:
 *
 */
uint32 rtl8380_uart0_phyBaseAddress(void)
{
    return RTL8380_UART0_PHY_BASE;
}

/* Function Name:
 *      rtl8380_console_init
 * Description:
 *      Initial RTL8380 UART0 for console
 * Input:
 *      baudrate    -   Console baudrate
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *
 */
void rtl8380_console_init(uint32 baudrate)
{
    unsigned int value = 0;
    /* 8 bits, 1 stop bit, no parity. */
    RTK_BSP_REG32(RTL8380_UART0_LCR) = ((CHAR_LEN_8 | ONE_STOP | PARITY_DISABLE) << RTL8380_UART_REG_OFFSET);
    /* Reset/Enable the FIFO */
    RTK_BSP_REG32(RTL8380_UART0_FCR) = ((FCR_EN | RXRST | TXRST | CHAR_TRIGGER_14) << RTL8380_UART_REG_OFFSET);
    /* Disable All Interrupts */
    RTK_BSP_REG32(RTL8380_UART0_IER) = RTL8380_UART_DISABLE;
    /* Enable Divisor Latch */
    RTK_BSP_REG32(RTL8380_UART0_LCR) |= (DLAB << RTL8380_UART_REG_OFFSET);
    /* Set Divisor */

    value = (RTL8380_SYSCLK / (baudrate * 16) - 1) & 0x00FF;
    RTK_BSP_REG32(RTL8380_UART0_DLL) = (value << RTL8380_UART_REG_OFFSET);

    value = (((RTL8380_SYSCLK / (baudrate * 16) - 1) & 0xFF00) >> 8);
    RTK_BSP_REG32(RTL8380_UART0_DLM) = (value << RTL8380_UART_REG_OFFSET);

    /* Disable Divisor Latch */
    RTK_BSP_REG32(RTL8380_UART0_LCR) &= ((~DLAB) << RTL8380_UART_REG_OFFSET);
}

/* Function Name:
 *      rtl8380_bspDev_irq_get
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
int32 rtl8380_bspDev_irq_get(uint32 device_id, uint32 *irq_num, int32 *is_updated)
{
    if(device_id >= RTK_DEV_MAX)
        return SDK_BSP_FAILED;
    if(irq_num == NULL)
        return SDK_BSP_FAILED;
    if(is_updated == NULL)
        return SDK_BSP_FAILED;

    /* The device_id is invalid for this chip */
    if (device_id >= (sizeof(rtl8380_dev_dtsIntr)/sizeof(rtl8380_dev_dtsIntr[0])))
        return SDK_BSP_ERR_INPUT;
    if (!rtl8380_dev_dtsIntr[device_id].dts_nodeName)
        return SDK_BSP_ERR_INPUT;

    *is_updated = FALSE;

    *irq_num = rtl8380_dev_dtsIntr[device_id].irq_number;

    *is_updated = TRUE;

    return SDK_BSP_OK;
}

/* Function Name:
 *      rtk_bspUSB_irq_get
 * Description:
 *      Get RTL8380 USB Host 2.0 IRQ number.
 * Input:
 *      device_id   -   SDK dev_config index
 * Output:
 *      irq_num     -   IRQ number
 * Return:
 *      Differnet chip has differnt intr source mapping,
 *      then IRQ number mapping is different, too.
 * Note:
 *
 */
uint32 rtl8380_bspUSB_irq_get(uint32 *irq_num)
{
    return SDK_BSP_OK;
}

/* Function Name:
 *      rtl8380_bspChip_reset
 * Description:
 *      Reset RTL8380.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *
 */
void rtl8380_bspChip_reset(void)
{
    uint32 tmp = 0;

    OTTO838x_FLASH_DISABLE_4BYTE_ADDR_MODE();
    RTK_BSP_REG32(RTL8380_RST_GLB_CTRL_1) |= (0x1 << RTL8380_SW_CHIP_RST_OFFSET);
}

/* Function Name:
 *      rtl8380_bspDeviceTree_get
 * Description:
 *      Get RTL8380 Device Tree
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *
 */
void rtl8380_bspDeviceTree_get(void)
{
    __dt_setup_arch(&__dtb_ms_rtl8380_begin);

}


/* Function Name:
 *      rtl8380_bspDeviceTreeBus_setup
 * Description:
 *      Register RTL8380 Device Tree Buses
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *
 */
void rtl8380_bspDeviceTreeBus_setup(void)
{
    __dt_register_buses("rtk,switch4", "rtl8380");

}

static int _rtl8380_get_dmasize_from_cmdline(unsigned int * dma_size)
{
    char *ptr;

    ptr = strstr(arcs_cmdline, "rtk_dma_size=");

    if (ptr)
        *dma_size = memparse(ptr + 13, &ptr);
    else{
        ptr = strstr(rtk_preDefine_cmdline, "rtk_dma_size=");
        if (ptr){
            *dma_size = memparse(ptr + 13, &ptr);
        }else{
            *dma_size = 0x0;  /* Default to 0 */
            return SDK_BSP_NOT_FOUND; /*Cannot get "rtk_dma_size" from command line*/
        }
    }

    return SDK_BSP_OK;

}

/* Function Name:
 *      _rtl8380_auto_probe_memsize
 * Description:
 *      Get the auto probe on-board meomory size. (DDR)
 * Input:
 *      None
 * Output:
 *      memory_size - Mega Byte
 * Return:
 *      SDK_BSP_OK
 *      SDK_BSP_FAILED
 * Note:
 *      The information is provided from Loader DDR Auto-Calibration.
 *
 */
static int _rtl8380_auto_probe_memsize(unsigned int * memory_size)
{
    return SDK_BSP_FAILED;
}

/* Function Name:
 *      _rtl8380_memSize_get
 * Description:
 *      Get the on-board meomory size. (DDR)
 * Input:
 *      None
 * Output:
 *      memory_size - Mega Byte
 * Return:
 *      SDK_BSP_OK
 *      SDK_BSP_FAILED
 * Note:
 *      None.
 *
 */
static int _rtl8380_memSize_get(unsigned int * memory_size)
{
    int ret = SDK_BSP_FAILED;
    unsigned int dram_size = 0;

#ifdef CONFIG_AUTO_PROBE_DRAM_SIZE
    ret = _rtl8380_auto_probe_memsize(&dram_size);
    if(ret != SDK_BSP_OK)
    {
        *memory_size = RTL8380_DEFAULT_SIZE;
    }
    else
    {
        *memory_size = dram_size;
    }
    return SDK_BSP_OK;
#else
    *memory_size = RTL8380_DEFAULT_SIZE;
    return SDK_BSP_OK;
#endif
}

/* Function Name:
 *      rtl8380_bspMemRegion_set
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
uint32 rtl8380_bspMemRegion_set(void)
{
    int ret = SDK_BSP_FAILED;
    unsigned int dram_size = 0;
    unsigned int rtk_dma_size = 0;

    ret = _rtl8380_get_dmasize_from_cmdline(&rtk_dma_size);
    if((ret != SDK_BSP_OK) && (ret != SDK_BSP_NOT_FOUND))
    {
        RTK_BSP_DBG_MSG("\nGet RTK User Mode DMA size failed!!!");
    }
    RTK_BSP_DBG_MSG("RTL8380 DMA size = 0x%08x Bytes\n",rtk_dma_size);

#ifdef CONFIG_HIGHMEM
    rtk_busAddrConv_enable = SDK_BSP_DISABLE;
#endif

    rtk_bspHighMem_start = RTL8380_LOWMEM_SIZE;

    ret = _rtl8380_memSize_get(&dram_size);
    if(ret != SDK_BSP_OK)
        return ret;

    RTK_BSP_DBG_MSG("RTL8380 Meomry size = 0x%08x Bytes\n",dram_size);

    add_memory_region(RTL8380_LOWMEM_START, (dram_size - rtk_dma_size), BOOT_MEM_RAM);

    rtl8380_lowMemSize = (dram_size - rtk_dma_size);
    rtl8380_dmaReservedSize = rtk_dma_size;
    return SDK_BSP_OK;
}

/* Function Name:
 *      rtl8380_bspMemSizeInfo_get
 * Description:
 *      Get Low/High/RTK_DMA size
 * Input:
 *      None
 * Output:
 *      low_memory_size     - Low Memory Size
 *      high_memory_size    - High Memory Size
 *      dma_reserved_size   - RTK DMA Size
 * Return:
 *      None
 * Note:
 *
 */
void rtl8380_bspMemSizeInfo_get(uint32 *lowMem_size, uint32 *highMem_size, uint32 *dmaReserved_size)
{
    *lowMem_size = rtl8380_lowMemSize;
    *highMem_size = RTL8380_NO_HIGHMEM_SIZE;
    *dmaReserved_size = rtl8380_dmaReservedSize;
}


/* Function Name:
 *      rtl8380_bspDriver_init
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
int32 rtl8380_bspDriver_init(rtk_bsp_mapper_t *pMapper)
{

    if(pMapper == NULL)
        return SDK_BSP_FAILED;

    pMapper->console_init               = rtl8380_console_init;
    pMapper->uart0_phyBaseAddress       = rtl8380_uart0_phyBaseAddress;
    pMapper->devIRQ_get                 = rtl8380_bspDev_irq_get;
    pMapper->chip_reset                 = rtl8380_bspChip_reset;
    pMapper->deviceTree_get             = rtl8380_bspDeviceTree_get;
    pMapper->deviceTreeBus_setup        = rtl8380_bspDeviceTreeBus_setup;
    pMapper->USB_irqGet                 = rtl8380_bspUSB_irq_get;
    pMapper->memRegion_set              = rtl8380_bspMemRegion_set;
    pMapper->memSizeInfo_get            = rtl8380_bspMemSizeInfo_get;
    pMapper->cpuExtTimer_init           = rtl8380_cpuExtTimer_init;
    pMapper->cpuExtTimer_ack            = rtl8380_cpuExtTimer_ack;
    pMapper->cpuExtTimerIntr_enable     = rtl8380_cpuExtTimerIntr_enable;
    pMapper->cpuExtTimerIRQ_get         = rtl8380_cpuExtTimerIRQ_get;
    pMapper->cpuExtTimerIRQ_restInit    = rtl8380_cpuExtTimerIRQ_restInit;
    pMapper->cpuExtTimerID_get          = rtl8380_cpuExtTimerID_get;

    return SDK_BSP_OK;
}

