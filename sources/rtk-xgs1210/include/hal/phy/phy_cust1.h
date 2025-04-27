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
 * $Revision: 71708 $
 * $Date: 2016-09-19 11:31:17 +0800 (Mon, 19 Sep 2016) $
 *
 * Purpose : PHY 8218D Driver APIs.
 *
 * Feature : PHY 8218D Driver APIs
 *
 */

#ifndef __HAL_PHY_PHY_CUST1_H__
#define __HAL_PHY_PHY_CUST1_H__

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
extern rt_phyInfo_t phy_cust1_info;



/*
 * Macro Declaration
 */


/*
 * Function Declaration
 */

/* Function Name:
 *      phy_identify_cust1
 * Description:
 *      Identify the port is CUST1  PHY or not?
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      model_id - model id
 *      rev_id   - revision id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - is intra serdes PHY
 *      RT_ERR_FAILED           - access failure or others
 *      RT_ERR_PHY_NOT_MATCH    - is not intra serdes PHY
 * Note:
 *      None
 */
extern int32
phy_identify_cust1(uint32 unit, rtk_port_t port, uint32 model_id, uint32 rev_id);

/* Function Name:
 *      phy_cust1drv_mapperInit
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
phy_cust1drv_mapperInit(rt_phydrv_t *pPhydrv);


/* Function Name:
 *      phy_cust1_swMacPollPhyStatus_get
 * Description:
 *      Get PHY status
 * Input:
 *      unit    - unit ID
 *      port    - port ID
 * Output:
 *      pphyStatus  - PHY status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
phy_cust1_swMacPollPhyStatus_get(uint32 unit, uint32 port, rtk_port_swMacPollPhyStatus_t *pphyStatus);

/* Function Name:
 *      phy_cust1_macIntfSerdesMode_get
 * Description:
 *      Get serdes mode
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pserdesMode - serdes mode
 * Return:
 *      None
 * Note:
 *      None
 */
extern int32
phy_cust1_macIntfSerdesMode_get(uint32 unit, rtk_port_t port, rt_serdesMode_t *pserdesMode);


/* Function Name:
 *      phy_cust1_imageFlash_load
 * Description:
 *      load Aquantia PHY image to flash
 * Input:
 *      unit - unit id
 *      port - port id
 *      size - image size
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
extern int32
phy_cust1_imageFlash_load(uint32 unit, rtk_port_t port, uint32 size, uint8 *image);


#endif /* __HAL_PHY_PHY_CUST1_H__ */












