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

#ifndef __HAL_PHY_PHY_COMMON_H__
#define __HAL_PHY_PHY_COMMON_H__

/*
 * Include Files
 */
#include <common/rt_type.h>

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
extern int32
phy_common_unavail(void);

#endif /* __HAL_PHY_PHY_COMMON_H__ */
