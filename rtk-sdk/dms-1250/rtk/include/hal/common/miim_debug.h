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
 * $Revision: 91355 $
 * $Date: 2018-08-13 21:54:45 +0800 (Mon, 13 Aug 2018) $
 *
 * Purpose : MIIM service APIs in the SDK.
 *
 * Feature : MIIM service APIs
 *
 */
#ifndef __MIIM_DEBUG_H__
#define __MIIM_DEBUG_H__

/*
 * Include Files
 */

/*
 * Symbol Definition
 */


/*
 * Macro Definition
 */


/*
 * Data Declaration
 */

/*
 * Function Declaration
 */

/* Function Name:
 *      miim_debug_sdsRxCali_start
 * Description:
 *      Manually start rx-cali
 * Input:
 *      unit   - unit id
 *      port   - mac ID of PHY port 0 (PHY's base MAC id)
 *      sdsId  - PHY's serdes ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      None
 */
int32
miim_debug_sdsRxCali_start(uint32 unit, uint32 port, uint32 sdsId);


/* Function Name:
 *      _phy_debug_cmd
 * Description:
 *      PHY debug API.
 * Input:
 *      unit   - unit id
 *      cmd_str - command string
 *      portmask - configure port
 *      param1 - parameters
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
_phy_debug_cmd(uint32 unit, char *cmd_str, rtk_portmask_t *portmask, uint32 param1, uint32 param2, uint32 param3, uint32 param4, uint32 param5);


#endif /* __MIIM_DEBUG_H__ */




