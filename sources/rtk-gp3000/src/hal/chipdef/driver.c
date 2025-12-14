/*
 * Copyright (C) 2012 Realtek Semiconductor Corp.
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
#include <hal/chipdef/driver.h>


/*
 * Data Declaration
 */


rt_driver_t rt_unknown_driver;

/* Definition major driver table */
rt_driver_t *rt_major_driver_table[] =
{
#if defined(CONFIG_SDK_APOLLO)
    /* RT_DRIVER_APOLLO */
    &apollo_driver,

    /* RT_DRIVER_APOLLO_REV_B */
    &apollo_driver_rev_b,
#endif

#if defined(CONFIG_SDK_APOLLOMP)
    &apollomp_a_driver,
#endif

#if defined(CONFIG_SDK_RTL9601B)
    &rtl9601b_a_driver,
#endif

#if defined(CONFIG_SDK_RTL9602C)
    &rtl9602c_a_driver,
#endif

#if defined(CONFIG_SDK_RTL9607C)
    &rtl9607c_a_driver,
#endif

#if defined(CONFIG_SDK_RTL9603CVD)
    &rtl9603cvd_a_driver,
#endif
    &rt_unknown_driver
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

