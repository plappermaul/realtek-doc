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
 */

/*
 * Include Files
 */
#include <dal/dal_common.h>
#include <hal/mac/mac_probe.h>

/* Function Name:
 *      dal_common_unavail
 * Description:
 *      Unsupported function callback
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_FEATURE_NOT_SUPPORTED - Feature does not support
 * Note:
 *      None.
 */
int32 dal_common_unavail(void)
{
    return RT_ERR_FEATURE_NOT_SUPPORTED;
} /* end of dal_common_unavail */

/* Function Name:
 *      dal_common_rev_get
 * Description:
 *      Get the revision ID
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      Revision ID
 * Note:
 *      None.
 */
int32 dal_common_rev_get(void)
{
#if defined(CONFIG_SDK_LUNA_G3) || defined(CONFIG_SDK_CA8279)
	return 0xFFFFFFFF;
#else
    uint32 chipId;
    uint32 ChipRevId;
    uint32 ret;

	if (( ret = drv_swcore_cid_get(&chipId,&ChipRevId)) != RT_ERR_OK )
    {
        return 0xFFFFFFFF;
    }

    return (int32)ChipRevId;
#endif
} /* end of dal_common_rev_get */

