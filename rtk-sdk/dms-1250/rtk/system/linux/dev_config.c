/*
 * Copyright (C) 2009-2016 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : BSP APIs definition.
 *
 * Feature : device configure API
 *
 */

/*
 * Include Files
 */
#include <linux/version.h>
#if (LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,19))
#include <platform.h>
#else
#include <bspchip.h>
#endif
#include <dev_config.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
/* RTK device configuration data */
#if (LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,19))
rtk_dev_t rtk_dev[] =
{
    [RTK_DEV_UART0] =
    {
        .dev_id = RTK_DEV_UART0,
        .pName = "UART0",
        .fIsr = NULL,
        .pIsr_param = NULL,
        .irq = UART0_IRQ
    },

    [RTK_DEV_UART1] =
    {
        .dev_id = RTK_DEV_UART1,
        .pName = "UART1",
        .fIsr = NULL,
        .pIsr_param = NULL,
        .irq = UART1_IRQ
    },

    [RTK_DEV_TC0] =
    {
        .dev_id = RTK_DEV_TC0,
        .pName = "TC0",
        .fIsr = NULL,
        .pIsr_param = NULL,
        .irq = TC0_IRQ
    },

    [RTK_DEV_TC1] =
    {
        .dev_id = RTK_DEV_TC1,
        .pName = "TC1",
        .fIsr = NULL,
        .pIsr_param = NULL,
        .irq = TC1_IRQ
    },

    [RTK_DEV_OCPTO] =
    {
        .dev_id = RTK_DEV_OCPTO,
        .pName = "OCPTO",
        .fIsr = NULL,
        .pIsr_param = NULL,
        .irq = OCPTO_IRQ
    },

    [RTK_DEV_HLXTO] =
    {
        .dev_id = RTK_DEV_HLXTO,
        .pName = "HLXTO",
        .fIsr = NULL,
        .pIsr_param = NULL,
        .irq = HLXTO_IRQ
    },

    [RTK_DEV_SLXTO] =
    {
        .dev_id = RTK_DEV_SLXTO,
        .pName = "SLXTO",
        .fIsr = NULL,
        .pIsr_param = NULL,
        .irq = SLXTO_IRQ
    },

    [RTK_DEV_NIC] =
    {
        .dev_id = RTK_DEV_NIC,
        .pName = "NIC",
        .fIsr = NULL,
        .pIsr_param = NULL,
        .irq = NIC_IRQ
    },

    [RTK_DEV_SWCORE] =
    {
        .dev_id = RTK_DEV_SWCORE,
        .pName = "SWCORE",
        .fIsr = NULL,
        .pIsr_param = NULL,
        .irq = SWCORE_IRQ
    },

    [RTK_DEV_GPIO_ABCD] =
    {
        .dev_id = RTK_DEV_GPIO_ABCD,
        .pName = "GPIO_ABCD",
        .fIsr = NULL,
        .pIsr_param = NULL,
        .irq = GPIO_ABCD_IRQ
    },
};
#endif
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,19))
rtk_dev_t rtk_dev[] =
{
    [RTK_DEV_UART0] =
    {
        .dev_id = RTK_DEV_UART0,
        .pName = "UART0",
        .fIsr = NULL,
        .pIsr_param = NULL,
        .irq = BSP_UART0_EXT_IRQ
    },

    [RTK_DEV_UART1] =
    {
        .dev_id = RTK_DEV_UART1,
        .pName = "UART1",
        .fIsr = NULL,
        .pIsr_param = NULL,
        .irq = BSP_UART1_EXT_IRQ
    },

    [RTK_DEV_TC0] =
    {
        .dev_id = RTK_DEV_TC0,
        .pName = "TC0",
        .fIsr = NULL,
        .pIsr_param = NULL,
        .irq = BSP_TC0_EXT_IRQ
    },

    [RTK_DEV_TC1] =
    {
        .dev_id = RTK_DEV_TC1,
        .pName = "TC1",
        .fIsr = NULL,
        .pIsr_param = NULL,
        .irq = BSP_TC1_EXT_IRQ
    },
    [RTK_DEV_OCPTO] =
    {
        .dev_id = RTK_DEV_OCPTO,
        .pName = "OCPTO",
        .fIsr = NULL,
        .pIsr_param = NULL,
        .irq = BSP_OCPTO_EXT_IRQ
    },

    [RTK_DEV_HLXTO] =
    {
        .dev_id = RTK_DEV_HLXTO,
        .pName = "HLXTO",
        .fIsr = NULL,
        .pIsr_param = NULL,
        .irq = BSP_HLXTO_EXT_IRQ
    },

    [RTK_DEV_SLXTO] =
    {
        .dev_id = RTK_DEV_SLXTO,
        .pName = "SLXTO",
        .fIsr = NULL,
        .pIsr_param = NULL,
        .irq = BSP_SLXTO_EXT_IRQ
    },
    [RTK_DEV_NIC] =
    {
        .dev_id = RTK_DEV_NIC,
        .pName = "NIC",
        .fIsr = NULL,
        .pIsr_param = NULL,
        .irq = BSP_NIC_EXT_IRQ
    },

    [RTK_DEV_SWCORE] =
    {
        .dev_id = RTK_DEV_SWCORE,
        .pName = "SWCORE",
        .fIsr = NULL,
        .pIsr_param = NULL,
        .irq = BSP_SWCORE_EXT_IRQ
    },

    [RTK_DEV_GPIO_ABCD] =
    {
        .dev_id = RTK_DEV_GPIO_ABCD,
        .pName = "GPIO_ABCD",
        .fIsr = NULL,
        .pIsr_param = NULL,
        .irq = BSP_GPIO_ABCD_EXT_IRQ
    },

    [RTK_DEV_TC2] =
    {
        .dev_id = RTK_DEV_TC2,
        .pName = "TC2",
        .fIsr = NULL,
        .pIsr_param = NULL,
        .irq = BSP_TC2_EXT_IRQ
    },

    [RTK_DEV_TC3] =
    {
        .dev_id = RTK_DEV_TC3,
        .pName = "TC3",
        .fIsr = NULL,
        .pIsr_param = NULL,
        .irq = BSP_TC3_EXT_IRQ
    },

    [RTK_DEV_TC4] =
    {
        .dev_id = RTK_DEV_TC4,
        .pName = "TC4",
        .fIsr = NULL,
        .pIsr_param = NULL,
        .irq = BSP_TC4_EXT_IRQ
    },

    [RTK_DEV_TC5] =
    {
        .dev_id = RTK_DEV_TC5,
        .pName = "TC5",
        .fIsr = NULL,
        .pIsr_param = NULL,
        .irq = BSP_TC5_EXT_IRQ
    },

    [RTK_DEV_TC6] =
    {
        .dev_id = RTK_DEV_TC6,
        .pName = "TC6",
        .fIsr = NULL,
        .pIsr_param = NULL,
        .irq = BSP_TC6_EXT_IRQ
    },
    [RTK_DEV_WDT_IP1] =
    {
        .dev_id = RTK_DEV_WDT_IP1,
        .pName = "WDT_IP1",
        .fIsr = NULL,
        .pIsr_param = NULL,
        .irq = BSP_WDT_IP1_IRQ
    },
};
#endif

/*
 * Macro Definition
 */

/*
 * Function Declaration
 */

