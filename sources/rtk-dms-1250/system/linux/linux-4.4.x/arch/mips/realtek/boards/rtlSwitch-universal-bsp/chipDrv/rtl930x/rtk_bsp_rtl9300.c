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
 * Purpose : RTL9300 BSP driver
 *
 * Feature : RTL9300 BSP driver
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
#include "rtk_bsp_rtl9300.h"
#include "rtk_bsp_rtl9300_intr.h"

#include <linux/serial.h>
#include <linux/serial_core.h>
#include <linux/serial_8250.h>
#include <linux/serial_reg.h>
#include <linux/tty.h>


rtk_dev_dtsIntr_mapping_t rtl9300_dev_dtsIntr[] =
{
    [RTK_DEV_UART0] =
    {
        .dts_nodeName = RTL9300_UART0,
        .irq_number = RTL9300_UART0_IRQ,
    },

    [RTK_DEV_UART1] =
    {
        .dts_nodeName = RTL9300_UART1,
        .irq_number = RTL9300_UART1_IRQ,
    },

    [RTK_DEV_TC0] =
    {
        .dts_nodeName = RTL9300_TC0,
        .irq_number = RTL9300_TC0_IRQ,
    },

    [RTK_DEV_TC1] =
    {
        .dts_nodeName = RTL9300_TC1,
        .irq_number = RTL9300_TC1_IRQ,
    },
    [RTK_DEV_NIC] =
    {
        .dts_nodeName = RTL9300_NIC,
        .irq_number = RTL9300_NIC_IRQ,
    },

    [RTK_DEV_SWCORE] =
    {
        .dts_nodeName = RTL9300_SWCORE,
        .irq_number = RTL9300_SWCORE_IRQ,
    },
    [RTK_DEV_GPIO_ABCD] =
    {
        .dts_nodeName = RTL9300_GPIO,
        .irq_number = RTL9300_GPIO_IRQ,
    },
    [RTK_DEV_TC2] =
    {
        .dts_nodeName = RTL9300_TC2,
        .irq_number = RTL9300_TC2_IRQ,
    },
    [RTK_DEV_TC3] =
    {
        .dts_nodeName = RTL9300_TC3,
        .irq_number = RTL9300_TC3_IRQ,
    },
    [RTK_DEV_TC4] =
    {
        .dts_nodeName = RTL9300_TC4,
        .irq_number = RTL9300_TC4_IRQ,
    },
    [RTK_DEV_WDT_IP1] =
    {
        .dts_nodeName = RTL9300_WDT0,
        .irq_number = RTL9300_WDT0_IRQ,
    },
};

rtk_cpu_extTimer_mapping_t rtl9300_cpu_extTimer[] =
{
    [RTL9300_CPU_0] =
    {
        .extTimer_name = RTL9300_TC2,
    },
};

uint32 rtl9300_cpuToExtTC_mapping[RTL9300_CPU_MAX];
uint32 rtl9300_lowMemSize, rtl9300_highMemSize, rtl9300_dmaReservedSize;

/* Function Name:
 *      _rtl9300_cpuExtTimerID_get
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
static int _rtl9300_cpuExtTimerID_get(uint32 cpu, uint32 *timer_id)
{
    if(cpu >= RTL9300_CPU_MAX)
        return SDK_BSP_FAILED;

    *timer_id = rtl9300_cpuToExtTC_mapping[cpu];

    return SDK_BSP_OK;
}

/* Function Name:
 *      _rtl9300_cpuExtTimer_mapping
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
static int _rtl9300_cpuExtTimer_mapping(void)
{
    uint32              cpu_idx, timer_idx;
    int                 ret = 0;
    char                *extTimer_name;

    extTimer_name = kmalloc(RTL9300_TC_NAME_SIZE, GFP_NOWAIT);
    if(extTimer_name == NULL)
    {
        RTK_BSP_DBG_MSG("\n[%s][%d] Timer Name buffer allcoation failed\n",__FUNCTION__,__LINE__);
        return SDK_BSP_FAILED;
    }

    for(cpu_idx = 0; cpu_idx < RTL9300_CPU_MAX; cpu_idx++)
    {
        for(timer_idx = 0; timer_idx < RTL9300_EXT_TIMER_MAX; timer_idx++)
        {
            memset(extTimer_name, 0, RTL9300_TC_NAME_SIZE);
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
            ret = strcmp(rtl9300_cpu_extTimer[cpu_idx].extTimer_name, extTimer_name);
            if(ret == 0)
            {
                rtl9300_cpuToExtTC_mapping[cpu_idx] = timer_idx;
                break;
            }
        }
    }

    if(extTimer_name != NULL)
        kfree(extTimer_name);

    return SDK_BSP_OK;
}

/* Function Name:
 *      rtl9300_cpuExtTimerID_get
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
int rtl9300_cpuExtTimerID_get(uint32 cpu, uint32 *timer_id)
{
    int ret = SDK_BSP_FAILED;

    if(cpu >= RTL9300_CPU_MAX)
        return SDK_BSP_FAILED;
    if(timer_id == NULL)
        return SDK_BSP_FAILED;

    ret = _rtl9300_cpuExtTimerID_get(cpu, timer_id);

    return ret;
}


/* Function Name:
 *      rtl9300_cpuExtTimer_init
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
void rtl9300_cpuExtTimer_init(void){
    int cpu_idx, timer_id, offset;
    int TCDATA = 0;
    int TCCTL = 0;
    int ret;

    ret = _rtl9300_cpuExtTimer_mapping();
    if(ret != SDK_BSP_OK)
        return;

    TCDATA = ( (RTL9300_LXR_MHZ * 1000000)/ ((int) DIVISOR_RTL9300 * HZ) );

    for(cpu_idx = 0; cpu_idx < RTL9300_CPU_MAX; cpu_idx ++)
    {
        ret = _rtl9300_cpuExtTimerID_get(cpu_idx, &timer_id);
        if(ret != SDK_BSP_OK)
        {
            RTK_BSP_DBG_MSG("[%s](CPU %d)External Timer Initial failed!!!\n",__FUNCTION__,cpu_idx);
            return;
        }
        offset = (timer_id*RTL9300_TC_REG_OFFSET);
        if (RTK_BSP_REG32(RTL9300MP_TC0INT + offset) & RTL9300MP_TCIP){
            RTK_BSP_REG32(RTL9300MP_TC0INT + offset) |= RTL9300MP_TCIP;
        }
        /* disable timer before setting CDBR */
        RTK_BSP_REG32(RTL9300MP_TC0CTL + offset) = 0; /* disable timer before setting CDBR */
        RTK_BSP_REG32(RTL9300MP_TC0DATA + offset ) =  TCDATA;
    }

    /* Enable timer for all CPU at one time. Let the count of all timer is near */
    TCCTL = RTL9300MP_TCEN | RTL9300MP_TCMODE_TIMER | DIVISOR_RTL9300 ;

    for(cpu_idx = 0; cpu_idx < RTL9300_CPU_MAX; cpu_idx++)
    {
        ret = _rtl9300_cpuExtTimerID_get(cpu_idx, &timer_id);
        if(ret != SDK_BSP_OK)
        {
            RTK_BSP_DBG_MSG("[%s](CPU %d)External Timer Initial failed!!!\n",__FUNCTION__,cpu_idx);
            return;
        }
        offset = (timer_id*RTL9300_TC_REG_OFFSET);
        RTK_BSP_REG32(RTL9300MP_TC0CTL+ offset) = TCCTL;
    }
}

/* Function Name:
 *      rtl9300_cpuExtTimerIntr_enable
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
void rtl9300_cpuExtTimerIntr_enable(uint32 cpu, rtk_bsp_set_t enable)
{
    int ret = SDK_BSP_FAILED;
    uint32 timer_id;

    ret = _rtl9300_cpuExtTimerID_get(cpu, &timer_id);
    if(ret != SDK_BSP_OK)
        RTK_BSP_DBG_MSG("[%s](CPU %d)_rtl9300_cpuTimer_mapping failed\n",__FUNCTION__,cpu);

    RTK_BSP_DBG_MSG("[%s](CPU %d)_rtl9300_cpuTimer_mapping timer_id = %d,enable=%d\n",__FUNCTION__,cpu,timer_id,enable);
#if 1
    if(enable == BSP_SET_ENABLED)
        RTK_BSP_REG32((RTL9300MP_TC0INT)+(timer_id * RTL9300_TC_REG_OFFSET)) |= RTL9300MP_TCIE;
    else
        RTK_BSP_REG32((RTL9300MP_TC0INT)+(timer_id * RTL9300_TC_REG_OFFSET)) &= ~(RTL9300MP_TCIE);
#endif
    RTK_BSP_DBG_MSG("[%s](CPU %d)RTK_BSP_REG32((RTL9300MP_TC0INT) = 0x%08x\n",__FUNCTION__,cpu,RTK_BSP_REG32((RTL9300MP_TC0INT)+(timer_id * RTL9300_TC_REG_OFFSET)));

}

/* Function Name:
 *      rtl9300_cpuExtTimer_ack
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
void rtl9300_cpuExtTimer_ack(uint32 cpu)
{
    int ret = SDK_BSP_FAILED;
    uint32 offset, timer_id;

    if(cpu >= RTL9300_CPU_MAX)
        return;

    ret = _rtl9300_cpuExtTimerID_get(cpu, &timer_id);
    if(ret != SDK_BSP_OK)
        RTK_BSP_DBG_MSG("[%s](CPU %d)_rtl9300_cpuTimer_mapping\n",__FUNCTION__,cpu);

    offset = (timer_id * RTL9300_TC_REG_OFFSET);

    RTK_BSP_REG32(RTL9300MP_TC0INT + offset) |= RTL9300MP_TCIP;
}

/* Function Name:
 *      rtl9300_cpuExtTimerIRQ_get
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
int32 rtl9300_cpuExtTimerIRQ_get(uint32 cpu, uint32 *irq_num)
{
    int ret = SDK_BSP_FAILED;
    int loop_idx = 0;

    if(irq_num == NULL)
        return ret;

    for(loop_idx = 0; loop_idx < (sizeof(rtl9300_dev_dtsIntr)/sizeof(rtk_dev_dtsIntr_mapping_t)); loop_idx++)
    {
        if(rtl9300_dev_dtsIntr[loop_idx].dts_nodeName != NULL)
        {
            ret = strcmp(rtl9300_cpu_extTimer[cpu].extTimer_name, rtl9300_dev_dtsIntr[loop_idx].dts_nodeName);
            if(ret == 0)
            {
               *irq_num = rtl9300_dev_dtsIntr[loop_idx].irq_number;
                return SDK_BSP_OK;
            }
        }
    }
    return SDK_BSP_FAILED;
}

/* Function Name:
 *      rtl9300_cpuExtTimerIRQ_restInit
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
int32 rtl9300_cpuExtTimerIRQ_restInit(void)
{
    return SDK_BSP_OK;
}


/* Function Name:
 *      rtl9300_uart0_phyBaseAddress
 * Description:
 *      Get RTL9300 UART0 Physical Base Address.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RTL9300 UART0 Phyical Base Address
 * Note:
 *
 */
uint32 rtl9300_uart0_phyBaseAddress(void)
{
    return RTL9300_UART0_PHY_BASE;
}

/* Function Name:
 *      rtl9300_console_init
 * Description:
 *      Initial RTL9300 UART0 for console
 * Input:
 *      baudrate    -   Console baudrate
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *
 */
void rtl9300_console_init(uint32 baudrate)
{
    unsigned int value = 0;
    /* 8 bits, 1 stop bit, no parity. */
    RTK_BSP_REG32(RTL9300_UART0_LCR) = ((CHAR_LEN_8 | ONE_STOP | PARITY_DISABLE) << RTL9300_UART_REG_OFFSET);
    /* Reset/Enable the FIFO */
    RTK_BSP_REG32(RTL9300_UART0_FCR) = ((FCR_EN | RXRST | TXRST | CHAR_TRIGGER_14) << RTL9300_UART_REG_OFFSET);
    /* Disable All Interrupts */
    RTK_BSP_REG32(RTL9300_UART0_IER) = RTL9300_UART_DISABLE;
    /* Enable Divisor Latch */
    RTK_BSP_REG32(RTL9300_UART0_LCR) |= (DLAB << RTL9300_UART_REG_OFFSET);
    /* Set Divisor */

    value = (RTL9300_SYSCLK / (baudrate * 16) - 1) & 0x00FF;
    RTK_BSP_REG32(RTL9300_UART0_DLL) = (value << RTL9300_UART_REG_OFFSET);

    value = (((RTL9300_SYSCLK / (baudrate * 16) - 1) & 0xFF00) >> 8);
    RTK_BSP_REG32(RTL9300_UART0_DLM) = (value << RTL9300_UART_REG_OFFSET);

    /* Disable Divisor Latch */
    RTK_BSP_REG32(RTL9300_UART0_LCR) &= ((~DLAB) << RTL9300_UART_REG_OFFSET);
}

/* Function Name:
 *      rtl9300_bspDev_irq_get
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
int32 rtl9300_bspDev_irq_get(uint32 device_id, uint32 *irq_num, int32 *is_updated)
{
    if(device_id >= RTK_DEV_MAX)
        return SDK_BSP_FAILED;
    if(irq_num == NULL)
        return SDK_BSP_FAILED;
    if(is_updated == NULL)
        return SDK_BSP_FAILED;

    /* The device_id is invalid for this chip */
    if (device_id >= (sizeof(rtl9300_dev_dtsIntr)/sizeof(rtl9300_dev_dtsIntr[0])))
        return SDK_BSP_ERR_INPUT;
    if (!rtl9300_dev_dtsIntr[device_id].dts_nodeName)
        return SDK_BSP_ERR_INPUT;

    *is_updated = FALSE;

    *irq_num = rtl9300_dev_dtsIntr[device_id].irq_number;

    *is_updated = TRUE;

    return SDK_BSP_OK;
}

/* Function Name:
 *      rtk_bspUSB_irq_get
 * Description:
 *      Get RTL9300 USB Host 2.0 IRQ number.
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
uint32 rtl9300_bspUSB_irq_get(uint32 *irq_num)
{

    if(irq_num == NULL)
        return SDK_BSP_FAILED;

    *irq_num = RTL9300_USBH2_IRQ;

    return SDK_BSP_OK;
}

/* Function Name:
 *      rtl9300_bspChip_reset
 * Description:
 *      Reset RTL9300.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *
 */
void rtl9300_bspChip_reset(void)
{
    RTK_BSP_REG32(RTL9300_RST_GLB_CTRL) |= (0x1 << RTL9300_SW_CHIP_RST_OFFSET);
}

/* Function Name:
 *      rtl9300_bspDeviceTree_get
 * Description:
 *      Get RTL9300 Device Tree
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *
 */
void rtl9300_bspDeviceTree_get(void)
{
    __dt_setup_arch(&__dtb_ms_rtl9300_begin);
}


/* Function Name:
 *      rtl9300_bspDeviceTreeBus_setup
 * Description:
 *      Register RTL9300 Device Tree Buses
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *
 */
void rtl9300_bspDeviceTreeBus_setup(void)
{
    __dt_register_buses("rtk,switch2", "rtl9300");
}

/* Function Name:
 *      rtl9300_bspLowMem_DMA_setup
 * Description:
 *      Let RTL9300 USB driver can force to use low memory.
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
void rtl9300_bspLowMem_DMA_setup(void)
{
    /* Keep USB uses low memory */
    PCI_DMA_BUS_IS_PHYS = LINUX_LOWMEM_DMA_SET;
}


static int _rtl9300_get_dmasize_from_cmdline(unsigned int * dma_size)
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
 *      _rtl9300_auto_probe_memsize
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
static int _rtl9300_auto_probe_memsize(unsigned int * memory_size)
{
    const unsigned char BNKCNTv[] = {1, 2, 3};
    const unsigned char BUSWIDv[] = {0, 1, 2};
    const unsigned char ROWCNTv[] = {11, 12, 13, 14, 15, 16};
    const unsigned char COLCNTv[] = {8, 9, 10, 11, 12};
    unsigned int dcr = *((volatile int *)(RTL9300_DRAM_DCR));
    unsigned int byte_size;

    byte_size = (1 << (BNKCNTv[(dcr >> 28) & 0x3] +
                 BUSWIDv[(dcr >> 24) & 0x3] +
                 ROWCNTv[(dcr >> 20) & 0xF] +
                 COLCNTv[(dcr >> 16) & 0xF]));

    RTK_BSP_DBG_MSG("RTL9300 AUTO Probe MEM size = 0x%08x Bytes",byte_size);

    if((byte_size != MEMORY_SIZE_64M) && (byte_size != MEMORY_SIZE_128M) && (byte_size != MEMORY_SIZE_256M) && (byte_size != MEMORY_SIZE_512M) && (byte_size != MEMORY_SIZE_1024M) && (byte_size != MEMORY_SIZE_2048M))
    {
        *memory_size = 0;
        return SDK_BSP_FAILED;
    }else{
        *memory_size = byte_size;
        return SDK_BSP_OK;

    }
    return SDK_BSP_OK;
}

/* Function Name:
 *      _rtl9300_memSize_get
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
static int _rtl9300_memSize_get(unsigned int * memory_size)
{
    int ret = SDK_BSP_FAILED;
    unsigned int dram_size = 0;

#ifdef CONFIG_AUTO_PROBE_DRAM_SIZE
    ret = _rtl9300_auto_probe_memsize(&dram_size);
    if(ret != SDK_BSP_OK)
    {
        *memory_size = RTL9300_DEFAULT_SIZE;
    }
    else
    {
        *memory_size = dram_size;
    }
    return SDK_BSP_OK;
#else
    *memory_size = RTL9300_DEFAULT_SIZE;
    return SDK_BSP_OK;
#endif
}

/* Function Name:
 *      rtl9300_bspMemRegion_set
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
uint32 rtl9300_bspMemRegion_set(void)
{
    int ret = SDK_BSP_FAILED;
    unsigned int dram_size = 0;
    unsigned int rtk_dma_size = 0;

    ret = _rtl9300_get_dmasize_from_cmdline(&rtk_dma_size);
    if((ret != SDK_BSP_OK) && (ret != SDK_BSP_NOT_FOUND))
    {
        RTK_BSP_DBG_MSG("\nGet RTK User Mode DMA size failed!!!");
    }
    RTK_BSP_DBG_MSG("RTL9300 DMA size = 0x%08x Bytes\n",rtk_dma_size);

#ifdef CONFIG_HIGHMEM
    /* Setup RTL9300 Memory Zone 2 */
    RTK_BSP_REG32(RTL9300_O0DOR2) = RTL9300_ZONE2_OFFSET;
    rtk_busAddrConv_enable = SDK_BSP_ENABLED;
    rtk_hiMemPhyBase_addr = RTL9300_ZONE2_PHY;
    rtk_hiMemLogBase_addr = RTL9300_ZONE2_BASE;
    rtk_busAddrConv_offset = RTL9300_ZONE2_OFFSET;
#endif
    rtk_bspHighMem_start = RTL9300_LOWMEM_SIZE;

    ret = _rtl9300_memSize_get(&dram_size);
    if(ret != SDK_BSP_OK)
        return ret;

    if(dram_size > RTL9300_LOWMEM_SIZE)
    {
        add_memory_region(RTL9300_LOWMEM_START, (RTL9300_LOWMEM_SIZE - rtk_dma_size), BOOT_MEM_RAM);
#ifdef CONFIG_HIGHMEM
        add_memory_region(RTL9300_HIGHMEM_START, (dram_size - RTL9300_LOWMEM_SIZE), BOOT_MEM_RAM);
#endif
        rtl9300_lowMemSize = (RTL9300_LOWMEM_SIZE - rtk_dma_size);
        rtl9300_highMemSize = (dram_size - RTL9300_LOWMEM_SIZE);
        rtl9300_dmaReservedSize = rtk_dma_size;
    }
    else
    {
        add_memory_region(RTL9300_LOWMEM_START, (dram_size - rtk_dma_size), BOOT_MEM_RAM);
        rtl9300_lowMemSize = (dram_size - rtk_dma_size);
        rtl9300_highMemSize = 0;
        rtl9300_dmaReservedSize = rtk_dma_size;
    }

    return SDK_BSP_OK;
}

/* Function Name:
 *      rtl9300_bspMemSizeInfo_get
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
void rtl9300_bspMemSizeInfo_get(uint32 *lowMem_size, uint32 *highMem_size, uint32 *dmaReserved_size)
{
    *lowMem_size = rtl9300_lowMemSize;
    *highMem_size = rtl9300_highMemSize;
    *dmaReserved_size = rtl9300_dmaReservedSize;
}

#ifdef CONFIG_HIGHMEM
static void rtl9300_bspSRAM_disable(void){

    RTK_BSP_REG32(RTL9300_UMSAR0) = 0;
    RTK_BSP_REG32(RTL9300_UMSAR1) = 0;
    RTK_BSP_REG32(RTL9300_UMSAR2) = 0;
    RTK_BSP_REG32(RTL9300_UMSAR3) = 0;
    RTK_BSP_REG32(RTL9300_SRAMSAR0) = 0;
    RTK_BSP_REG32(RTL9300_SRAMSAR1) = 0;
    RTK_BSP_REG32(RTL9300_SRAMSAR2) = 0;
    RTK_BSP_REG32(RTL9300_SRAMSAR3) = 0;
}
#endif

/* Function Name:
 *      rtl9300_bspDriver_init
 * Description:
 *      Initial and Hook RTL9300 BSP drivers
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
int32 rtl9300_bspDriver_init(rtk_bsp_mapper_t *pMapper)
{

    if(pMapper == NULL)
        return SDK_BSP_FAILED;

#ifdef CONFIG_HIGHMEM
    rtl9300_bspSRAM_disable();
#endif
    pMapper->console_init               = rtl9300_console_init;
    pMapper->uart0_phyBaseAddress       = rtl9300_uart0_phyBaseAddress;
    pMapper->devIRQ_get                 = rtl9300_bspDev_irq_get;
    pMapper->chip_reset                 = rtl9300_bspChip_reset;
    pMapper->deviceTree_get             = rtl9300_bspDeviceTree_get;
    pMapper->deviceTreeBus_setup        = rtl9300_bspDeviceTreeBus_setup;
    pMapper->USB_irqGet                 = rtl9300_bspUSB_irq_get;
    pMapper->lowMem_DMA_setup           = rtl9300_bspLowMem_DMA_setup;
    pMapper->memRegion_set              = rtl9300_bspMemRegion_set;
    pMapper->memSizeInfo_get            = rtl9300_bspMemSizeInfo_get;
    pMapper->cpuExtTimer_init           = rtl9300_cpuExtTimer_init;
    pMapper->cpuExtTimer_ack            = rtl9300_cpuExtTimer_ack;
    pMapper->cpuExtTimerIntr_enable     = rtl9300_cpuExtTimerIntr_enable;
    pMapper->cpuExtTimerIRQ_get         = rtl9300_cpuExtTimerIRQ_get;
    pMapper->cpuExtTimerIRQ_restInit    = rtl9300_cpuExtTimerIRQ_restInit;
    pMapper->cpuExtTimerID_get          = rtl9300_cpuExtTimerID_get;

    return SDK_BSP_OK;
}

