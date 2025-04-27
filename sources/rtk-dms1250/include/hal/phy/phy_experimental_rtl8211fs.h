/*
 * Copyright (C) 2021 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: $
 * $Date: $
 *
 * Purpose : PHY experimental Driver APIs.
 *
 * Feature : PHY experimental Driver APIs.
 *
 */

#ifndef __HAL_PHY_EXPERIMENTAL_H__
#define __HAL_PHY_EXPERIMENTAL_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/port.h>

/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */
extern rt_phyInfo_t phy_8211FS_info;

/*
 * Macro Declaration
 */

/*
 * Function Declaration
 */

/* Function Name:
 *      phy_experimental_8211FS_mapperInit
 * Description:
 *      Initialize PHY driver.
 * Input:
 *      pPhydrv - pointer of phy driver
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
extern void
phy_experimental_8211FS_mapperInit(rt_phydrv_t *pPhydrv);

#endif /* __HAL_PHY_EXPERIMENTAL_H__ */

