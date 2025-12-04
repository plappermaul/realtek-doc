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
 * Purpose : PHY Common Driver APIs.
 *
 * Feature : PHY Common Driver APIs
 *
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <osal/time.h>
#include <osal/print.h>
#include <hal/common/miim.h>
#include <hal/phy/phydef.h>
#include <hal/phy/phy_common.h>


/*
 * Symbol Definition
 */


/*
 * Function Declaration
 */

/* Function Name:
 *      phy_common_unavail
 * Description:
 *      Return chip not support
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_CHIP_NOT_SUPPORTED   - functions not supported by this chip model
 * Note:
 *      None
 */
int32 phy_common_unavail(void)
{
    return RT_ERR_CHIP_NOT_SUPPORTED;
} /* end of phy_common_unavail */
