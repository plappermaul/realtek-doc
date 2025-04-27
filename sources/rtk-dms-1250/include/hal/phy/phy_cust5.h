/*
 * Copyright (C) 2019 Realtek Semiconductor Corp.
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
 * Purpose : Customer PHY Driver sample codes
 *
 * Feature : Customer PHY Driver sample codes
 *
 */

#ifndef __HAL_PHY_PHY_CUST5_H__
#define __HAL_PHY_PHY_CUST5_H__

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
extern rt_phyInfo_t phy_cust5_info;



/*
 * Macro Declaration
 */


/*
 * Function Declaration
 */

/* Function Name:
 *      phy_identify_cust5
 * Description:
 *      Identify the port is CUST5 PHY or not
 *      This function will be called in "phy probe" stage for checking if the port's PHY is CUST5 PHY
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      model_id - model id (not used)
 *      rev_id   - revision id (not used)
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - port is CUST5 PHY, the port will then hook with this PHY driver
 *      RT_ERR_FAILED           - access failure or others
 *      RT_ERR_PHY_NOT_MATCH    - port is not CUST5 PHY
 * Note:
 *      None
 */
extern int32
phy_identify_cust5(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id);

/* Function Name:
 *      phy_cust5drv_mapperInit
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
phy_cust5drv_mapperInit(rt_phydrv_t *pPhydrv);




#endif /* __HAL_PHY_PHY_CUST5_H__ */












