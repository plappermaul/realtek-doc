/*
 * Copyright (C) 2009 Realtek Semiconductor Corp.
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
//#include <platform.h>
#include <dev_config.h>
/*
 * Symbol Definition
 */
/*
 * Interrupt IRQ Assignments
 */
#define UART0_IRQ       31
#define UART1_IRQ       30
#define TC0_IRQ         29
#define TC1_IRQ         28
#define OCPTO_IRQ       27
#define HLXTO_IRQ       26
#define SLXTO_IRQ       25
#define NIC_IRQ         24
#define GPIO_ABCD_IRQ   23
#define GPIO_EFGH_IRQ   22
#define RTC_IRQ         21
#define SWCORE_IRQ      20

/*
 * Data Declaration
 */
/* RTK device configuration data */
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

    [RTK_DEV_GPIO_EFG] =
    {
        .dev_id = RTK_DEV_GPIO_EFG,
        .pName = "GPIO_EFG",
        .fIsr = NULL,
        .pIsr_param = NULL,
        .irq = GPIO_EFGH_IRQ
    },

    [RTK_DEV_RTC] =
    {
        .dev_id = RTK_DEV_RTC,
        .pName = "RTC",
        .fIsr = NULL,
        .pIsr_param = NULL,
        .irq = RTC_IRQ
    }
};

/*
 * Macro Definition
 */

/*
 * Function Declaration
 */

