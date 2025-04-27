/*
 * Copyright (C) 2018 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 75479 $
 * $Date: 2017-01-20 15:17:16 +0800 (Fri, 20 Jan 2017) $
 *
 * Purpose : PHY 8226 Driver APIs.
 *
 * Feature : PHY 8226 Driver APIs
 *
 */
#ifndef __HAL_PHY_PHY_RTL8226_H__
#define __HAL_PHY_PHY_RTL8226_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/port.h>


/*
 * Symbol Definition
 */
#define PORT_NUM_IN_8226        1





/*
 * Data Declaration
 */
extern rt_phyInfo_t phy_8226_info;
extern rt_phyInfo_t phy_8226b_info;


/*
 * Macro Declaration
 */


/*
 * Function Declaration
 */



/* Function Name:
 *      phy_8226drv_mapperInit
 * Description:
 *      Initialize PHY 8226 driver.
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
phy_8226drv_mapperInit(rt_phydrv_t *pPhydrv);

/* Function Name:
 *      phy_8226bdrv_mapperInit
 * Description:
 *      Initialize PHY 8226B driver.
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
phy_8226bdrv_mapperInit(rt_phydrv_t *pPhydrv);

#endif /* __HAL_PHY_PHY_RTL8226_H__ */

