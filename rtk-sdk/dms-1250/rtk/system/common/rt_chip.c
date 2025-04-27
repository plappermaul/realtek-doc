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
 * Purpose : Define the chip utility in the SDK.
 *
 * Feature : SDK common utility (chip)
 *
 */

/*
 * Include Files
 */
#include <common/rt_autoconf.h>
#include <common/error.h>
#include <common/rt_type.h>
#include <hwp/hw_profile.h>
#include <common/rt_chip.h>



/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */


/*
 * Macro Definition
 */


/*
 * Function Declaration
 */


/* Function Name:
 *      rt_chip_familyIndex_get
 * Description:
 *      Get rt_chip_family_index_t of this unit
 * Input:
 *      unit    - the unit ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - Successed
 *      RT_ERR_FAILED   - Failed
 * Note:
 *      None
 */
int32
rt_chip_familyIndex_get(uint32 unit, rt_chip_family_index_t *index)
{

    if(HWP_UNIT_VALID_LOCAL(unit))
    {
        switch(HWP_CHIP_FAMILY_ID(unit))
        {
            case RTL8380_FAMILY_ID:
            case RTL8330_FAMILY_ID:
                *index = RT_CHIP_FAMILY_INDEX_8380;
                break;
            case RTL8390_FAMILY_ID:
            case RTL8350_FAMILY_ID:
                *index = RT_CHIP_FAMILY_INDEX_8390;
                break;
            case RTL9300_FAMILY_ID:
                *index = RT_CHIP_FAMILY_INDEX_9300;
                break;
            case RTL9310_FAMILY_ID:
                *index = RT_CHIP_FAMILY_INDEX_9310;
                break;
            default:
                return RT_ERR_FAILED;
        }
    }
    else
    {
        *index = RT_CHIP_FAMILY_INDEX_RPC;
    }

    return RT_ERR_OK;
}



