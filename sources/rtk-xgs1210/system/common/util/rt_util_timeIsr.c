/*
 * Copyright (C) 2009-2015 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 93105 $
 * $Date: 2018-10-29 18:00:51 +0800 (Mon, 29 Oct 2018) $
 *
 * Purpose : Define the utility macro and function in the SDK.
 *
 * Feature : SDK common utility (time)
 *
 */

/*
 * Include Files
 */
#include <common/rt_autoconf.h>
#include <common/error.h>
#include <osal/print.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/debug/rt_log.h>
#include <drv/tc/tc.h>
#include <hwp/hw_profile.h>
#include <osal/isr.h>
#include <common/util/rt_util_time.h>
#include <dev_config.h>
#include <rtcore/rtcore.h>


/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */
static rt_util_timeTcIsr_db_t tcIsr_Db;

/*
 * Macro Definition
 */


/*
 * Function Declaration
 */



/* Function Name:
 *      rt_util_tcID2devID_get
 * Description:
 *      Get the dev ID for specific TC ID
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *
 */
void rt_util_tcID2devID_get(uint32 tc_id, uint32 *dev_id)
{
    switch(tc_id)
    {
        case TC_ID0:
            *dev_id = RTK_DEV_TC0;
            break;
        case TC_ID1:
            *dev_id = RTK_DEV_TC1;
            break;
        case TC_ID2:
            *dev_id = RTK_DEV_TC2;
            break;
        case TC_ID3:
            *dev_id = RTK_DEV_TC3;
            break;
        case TC_ID4:
            *dev_id = RTK_DEV_TC4;
            break;
        case TC_ID5:
            *dev_id = RTK_DEV_TC5;
            break;
        case TC_ID6:
            *dev_id = RTK_DEV_TC6;
            break;
        default:
            *dev_id = RTK_DEV_MAX;
            break;
    }

}



/* Function Name:
 *      rt_util_hpt_tcIsr
 * Description:
 *      ISR of TC
 * Input:
 *      *p  - user data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *
 */
osal_isrret_t
rt_util_hpt_tcIsr(void *p)
{
    rt_util_timeTcIsr_db_t *db = (rt_util_timeTcIsr_db_t *)p;

    drv_tc_intState_clear(HWP_MY_UNIT_ID(), db->tc_id);

    rtk_uk_sharedMem->rt_tv1.sec++;

    return RT_ERR_OK;
}



int32
rt_util_hpt_isrRegister(void)
{
    uint32 dev_id;

    osal_printf("    TC util init (isr)\n");

    rt_util_hpt_whichTC_get(HWP_MY_UNIT_ID(),&tcIsr_Db.tc_id);

    rt_util_tcID2devID_get(tcIsr_Db.tc_id, &dev_id);

    osal_isr_unregister(dev_id);

    osal_isr_register(dev_id, rt_util_hpt_tcIsr, (void *)&tcIsr_Db);

    return RT_ERR_OK;
}



