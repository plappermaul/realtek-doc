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
 * Purpose : PHY 8218E Driver APIs.
 *
 * Feature : PHY 8218E Driver APIs
 *
 */

#ifndef __HAL_PHY_PHY_RTL8218E_PATCH_H__
#define __HAL_PHY_PHY_RTL8218E_PATCH_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/port.h>


#ifdef CONFIG_SDK_RTL8218E

/* Function Name:
 *      phy_8218e_conf_init
 * Description:
 *      Set patch to PHY.
 * Input:
 *      unit        - unit id
 *      basePort    - base port id
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
extern void
phy_8218e_conf_init(uint32 unit, rtk_port_t basePort);

/* Function Name:
 *      sub_phy_8218e_patch_set
 * Description:
 *      Set patch to PHY.
 * Input:
 *      unit        - unit id
 *      port        - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
extern int32
sub_phy_8218e_patch_set(uint32 unit, rtk_port_t port);

/* Function Name:
 *      phy_8218e_conf_perPort_init
 * Description:
 *      Set patch to PHY.
 * Input:
 *      unit        - unit id
 *      port        - port id
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
extern void
phy_8218e_conf_perPort_init(uint32 unit, rtk_port_t port);

/* Function Name:
 *      sub_phy_8218e_patch_perPort_set
 * Description:
 *      Set patch to PHY.
 * Input:
 *      unit            - unit id
 *      port            - port id
 *      pRst_portmask   - reset port mask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
extern int32
sub_phy_8218e_patch_perPort_set(uint32 unit, rtk_port_t port, rtk_portmask_t *pRst_portmask);

#endif  /* CONFIG_SDK_RTL8218E */


#endif /* __HAL_PHY_PHY_RTL8218E_PATCH_H__ */
