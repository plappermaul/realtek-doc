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
 * Purpose : PHY identify service APIs in the SDK.
 *
 * Feature : PHY identify service APIs
 *
 */

#ifndef __HAL_PHY_IDENTIFY_H__
#define __HAL_PHY_IDENTIFY_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <hal/phy/phydef.h>


/*
 * Function Declaration
 */

/* Function Name:
 *      phy_identify_OUI_check
 * Description:
 *      Identify the OUI is the realtek OUI or not?
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - Realtek OUI
 *      RT_ERR_FAILED - not Realtek OUI
 * Note:
 *      None
 */
extern int32
phy_identify_OUI_check(uint32 unit, rtk_port_t port);

/* Function Name:
 *      phy_identify_find
 * Description:
 *      Find this kind of PHY control structure from the phy supported list.
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      None
 * Return:
 *      NULL      - Not found
 *      Otherwise - Pointer of PHY control structure that found
 * Note:
 *      None
 */
extern rt_phyctrl_t *
phy_identify_find(uint32 unit, rtk_port_t port);

/* Function Name:
 *      phy_identify_phyid_get
 * Description:
 *      Get this phy model id and its revision id from chip.
 * Input:
 *      unit      - unit id
 *      port      - port id
 * Output:
 *      pModel_id - pointer buffer of phy model id
 *      pRev_id   - pointer buffer of phy revision id
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      None
 */
extern int32
phy_identify_phyid_get(
    uint32      unit,
    rtk_port_t  port,
    uint32      *pModel_id,
    uint32      *pRev_id);

/* Function Name:
 *      phy_identify_int_find
 * Description:
 *      Find this kind of PHY control structure from the internal phy supported list.
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      None
 * Return:
 *      NULL      - Not found
 *      Otherwise - Pointer of PHY control structure that found
 * Note:
 *      None
 */
extern rt_phyctrl_t *
phy_identify_int_find(uint32 unit, rtk_port_t port);

/* Function Name:
 *      phy_identify_10GeSerdesDrv_find
 * Description:
 *      Find 10GE serdes driver
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      None
 * Return:
 *      NULL      - Not found
 *      Otherwise - Pointer of PHY control structure that found
 * Note:
 *      None
 */
extern rt_phyctrl_t *
phy_identify_10GeSerdesDrv_find(uint32 unit, rtk_port_t port);


/* Function Name:
 *      phy_identify_serdesDrvCtrl_get
 * Description:
 *      Get serdes driver control block
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      None
 * Return:
 *      NULL      - Not found
 *      Otherwise - Pointer of PHY control structure that found
 * Note:
 *      None
 */
extern rt_phyctrl_t *
phy_identify_serdesDrvCtrl_get(uint32 unit, rtk_port_t port);

/* Function Name:
 *      phy_identify_init
 * Description:
 *      Initial identify databases
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
extern void
phy_identify_init(void);


/* Function Name:
 *      phy_identify_driver_find_by_phyType
 * Description:
 *      Find PHY driver from the phy type.
 * Input:
 *      phyType - PHY type
 * Output:
 *      None
 * Return:
 *      NULL      - Not found
 *      Otherwise - Pointer of PHY driver that found
 * Note:
 *      None
 */
extern rt_phyctrl_t *
phy_identify_driver_find_by_phyType(phy_type_t phyType);

/* Function Name:
 *      phy_identify_driver_find_blindly
 * Description:
 *      Find PHY driver from all drivers
 * Input:
 *      unit -  unit id
 *      port -  mac id
 * Output:
 *      None
 * Return:
 *      NULL      - Not found
 *      Otherwise - Pointer of PHY control that found
 * Note:
 *      None
 */
extern rt_phyctrl_t *
phy_identify_driver_find_blindly(uint32 unit, rtk_port_t port);

#endif /* __HAL_PHY_IDENTIFY_H__ */
