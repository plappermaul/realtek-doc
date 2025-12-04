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
#include <common/rt_type.h>


/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
typedef struct rtk_dev_s
{
    int32 dev_id;
    char *pName;
    void *fIsr;
    void *pIsr_param;
    int32 irq;
} rtk_dev_t;

enum 
{
    RTK_DEV_UART0 = 0,
    RTK_DEV_UART1,
    RTK_DEV_TC0,
    RTK_DEV_TC1,
    RTK_DEV_OCPTO,
    RTK_DEV_HLXTO,
    RTK_DEV_SLXTO,
    RTK_DEV_NIC,
    RTK_DEV_SWCORE,
    RTK_DEV_GPIO_ABCD,
    RTK_DEV_GPIO_EFG,
    RTK_DEV_RTC,
    RTK_DEV_MAX
};

extern rtk_dev_t rtk_dev[RTK_DEV_MAX];

/*
 * Function Declaration
 */

