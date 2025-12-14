/*
 * Copyright (C) 2014 Realtek Semiconductor Corp.
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

#ifndef __MAC_PROBE_TOOL_H__
#define __MAC_PROBE_TOOL_H__

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
 * Function Declaration
 */
/* Function Name:
 *      mac_probe_tool_cid_get
 * Description:
 *      Get chip id and chip revision id.
 * Input:
 *      None
 * Output:
 *      pChip_id       - pointer buffer of chip id
 *      pChip_rev_id   - pointer buffer of chip revision id
 * Note:
 *      Always success
 */
void mac_probe_tool_cid_get(uint32 *pChip_id, uint32 *pChip_rev_id);

#endif

