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
 * $Revision: 83077 $
 * $Date: 2019-11-15 18:51:47 +0800 (Thu, 15 Nov 2019) $
 *
 * Purpose : rtk util for SDK module
 *
 * Feature : rtk util for SDK module
 *
 */


#include <common/rt_type.h>
#include <linux/kernel.h>
#include "chip_probe.h"

#define RT_ERR_OK 0

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
 *      SDK implement for Linux 3.18.24,
 *      IRQ number is fixed in SDK rtk_dev[].
 *      No need to implement to get IRQ number.
 */
int32 rtk_bspDev_irq_get(uint32 device_id, uint32 *irq_num, int32 *is_updated)

{
    *is_updated = FALSE;

    return RT_ERR_OK;
}

/* ===== Export BSP Symbol ===== */
EXPORT_SYMBOL(rtk_bspDev_irq_get);

