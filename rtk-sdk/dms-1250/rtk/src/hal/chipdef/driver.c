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
 * Purpose : driver symbol and data type definition in the SDK.
 *
 * Feature : driver symbol and data type definition
 *
 */

/*
 * Include Files
 */
#include <common/rt_autoconf.h>
#include <hal/chipdef/driver.h>


/*
 * Data Declaration
 */

/* Definition major driver table */
rt_driver_t *rt_major_driver_table[] =
{
#if defined(CONFIG_SDK_RTL8390)
    /* RT_DRIVER_RTL8390_A */
    &rtl8390_a_driver,
#endif /* end of defined(CONFIG_SDK_RTL8390) */
#if defined(CONFIG_SDK_RTL8380)
    /* RT_DRIVER_RTL8380_A */
    &rtl8380_a_driver,
#endif /* end of defined(CONFIG_SDK_RTL8380) */
#if defined(CONFIG_SDK_RTL9310)
    /* RT_DRIVER_RTL9310_A */
    &rtl9310_a_driver,
#endif
#if defined(CONFIG_SDK_RTL9300)
    /* RT_DRIVER_RTL9300_A */
    &rtl9300_a_driver,
#endif
}; /* end of rt_major_driver_table */


/*
 * Function Declaration
 */

/* Function Name:
 *      hal_find_driver
 * Description:
 *      Find the mac major driver from SDK supported driver lists.
 * Input:
 *      driver_id     - driver chip id
 *      driver_rev_id - driver chip revision id
 * Output:
 *      None
 * Return:
 *      NULL      - Not found
 *      Otherwise - Pointer of mac driver structure that found
 * Note:
 *      The function have found the exactly driver from SDK supported driver lists.
 */
rt_driver_t *
hal_find_driver(uint32 driver_id, uint32 driver_rev_id)
{
    uint32      driver_idx;
    rt_driver_t *pMdriver;

    for (driver_idx = 0; driver_idx < RT_DRIVER_END; driver_idx++)
    {
        pMdriver = rt_major_driver_table[driver_idx];
        if (pMdriver->driver_id == driver_id &&
            pMdriver->driver_rev_id == driver_rev_id)
        {
            return rt_major_driver_table[driver_idx];
        }
    }

    return NULL;
} /* end of hal_find_driver */
