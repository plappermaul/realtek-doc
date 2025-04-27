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
 * $Revision$
 * $Date$
 *
 * Purpose : Define board profile structure, API, and macros.
 *
 * Feature :
 *
 */

#ifndef __HWP_UTIL_H__
#define __HWP_UTIL_H__

#include <hal/chipdef/chipdef.h>

/*
 * Include Files
 */


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
 *      _hwp_portInUplink_check
 * Description:
 *      Check a port number if in the range of UPLINK port
 *      (uplink port has a special definition here: the last 4 ports of 9300/9310)
 * Input:
 *      unit        - Family ID
 *      port        - port ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - in range
 *      RT_ERR_FAILED   - out of range
 * Note:
 *      None
 */
extern int32
_hwp_portInUplink_check(uint32 family_id, uint32 port);


/* Function Name:
 *      hwp_unit_show
 * Description:
 *      Show unit information
 * Input:
 *      unit        - unit ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      None
 */
extern int32
hwp_unit_show(uint32 unit);


/* Function Name:
 *      hwp_parsedInfo_show
 * Description:
 *      Show hardware profile parsed information debug message
 * Input:
 *      unit        - unit ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      None
 */
extern int32
hwp_parsedInfo_show(uint32 unit);


/* Function Name:
 *      hwp_info_show
 * Description:
 *      Show the hardware profile information
 * Input:
 *      target_unit - show the profile infomration of this unit
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
hwp_info_show(uint32 target_unit);

extern int32
hwp_defaultProfilePort_update(hwp_hwProfile_t *defaultHwp, uint32 unit, uint8 mac_id, uint8 attr, uint8 medi, uint8 eth, uint8 phy_idx);

/* Function Name:
 *      hwp_defaultProfilePort_build
 * Description:
 *      Build up default hardware profile - port part
 * Input:
 *      defaultHwp  - the default hardware profile that try to build port information
 *      devInfo     - the rt_device_t information of unit 0 of defaultHwp
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - build up
 *      RT_ERR_FAILED   - failed
 * Note:
 *      None
 */
extern int32
hwp_defaultProfilePort_build(hwp_hwProfile_t *defaultHwp, rt_device_t *devInfo);


/* Function Name:
 *      hwp_defaultProfilePhy_build
 * Description:
 *      Build up default hardware profile - PHY part
 * Input:
 *      defaultHwp  - the default hardware profile that try to build port information
 *      port        - port number
 *      phyCtrl     - the control block of the phy connectted to the port
 *      phyBasePort - base port of this phy
 *      phyIndex    - phy index in hardware profile
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - build up
 *      RT_ERR_FAILED   - failed
 * Note:
 *      None
 */
extern int32
hwp_defaultProfilePhy_build(hwp_hwProfile_t *defaultHwp, uint8 port, rt_phyctrl_t *phyCtrl, uint8 phyBasePort, int phyIndex);

/* Function Name:
 *      hwp_profilePhy_del
 * Description:
 *      Delete PHY from hardware profile
 * Input:
 *      unit        - unit ID
 *      delMsk      - port mask for PHY delete
 * Output:
 *      None
 * Return:
 *      int32       - actual updated(deleted) number of ports to the hardware profile
 * Note:
 *      None
 */

extern int32
hwp_profilePhy_del(uint32 unit, rtk_portmask_t *delMsk);

/* Function Name:
 *      hwp_profilePhy_update
 * Description:
 *      Update PHY type from hardware profile.
 * Input:
 *      unit        - unit ID
 *      baseport    - base mac ID of the PHY
 *      phyType     - new PHY type
 * Output:
 *      None
 * Return:
 *      int32       - update count
 * Note:
 *      None
 */
extern int32
hwp_profilePhy_update(uint32 unit, uint32 baseport, uint32 phyType);

/* Function Name:
 *      hwp_multiSdsPortLaneNum_get
 * Description:
 *      Get lane number of the serdes for multi-serdes-Port.
 *      The lowest ID serdes is lane 0.
 * Input:
 *      unit        - unit ID
 *      sdsId       - serdes ID
 * Output:
 *      pLane       - lane number
 * Return:
 *      RT_ERR_OK       - success
 *      RT_ERR_FAILED   - failed. The sdsId is not multi-serdes-Port's serdes.
 * Note:
 *      None
 */
extern int32
hwp_multiSdsPortLaneNum_get(uint32 unit, uint8 sdsId, uint32 *pLane);

/* Function Name:
 *      hwp_get_port_by_baseport_offset
 * Description:
 *      Get port macid by base port macid and phy port offset
 * Input:
 *      unit       - unit id.
 *      baseId     - base port macid
 *      offset     - phy port offset
 * Output:
 *      pMacId - pointer to the result macid
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_ENTRY_NOTFOUND
 * Note:
 *      None
 */
extern int32
hwp_get_port_by_baseport_offset(int32 unit, uint8 baseId, uint8 offset, uint8 *pMacId);

/* Function Name:
 *      hwp_get_offset_by_baseport_port
 * Description:
 *      Get phy port offset by base port macid and port macid
 * Input:
 *      unit       - unit id.
 *      baseId     - base port macid
 *      macId      - port macid
 * Output:
 *      pOffset - pointer to the result phy port offset 
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_ENTRY_NOTFOUND
 * Note:
 *      None
 */
extern int32
hwp_get_offset_by_baseport_port(int32 unit, uint8 baseId, uint8 macId, uint8 *pOffset);

#endif /* __HWP_UTIL_H__ */
