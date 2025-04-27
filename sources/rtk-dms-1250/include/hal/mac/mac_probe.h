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
 * Purpose : MAC probe and init service APIs in the SDK.
 *
 * Feature : MAC probe and init service APIs
 *
 */

#ifndef __HAL_MAC_PROBE_H__
#define __HAL_MAC_PROBE_H__

/*
 * Data Declaration
 */

/*
 * Function Declaration
 */


/* Function Name:
 *      mac_probe
 * Description:
 *      Probe the MAC chip in the specified chip.
 * Input:
 *      unit    - unit ID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      None
 */
int32
mac_probe(uint32 unit);

/* Function Name:
 *      mac_init
 * Description:
 *      Init the MAC chip in the specified chip.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - Failed
 * Note:
 *      None
 */
int32
mac_init(uint32 unit);

#endif  /* __HAL_MAC_PROBE_H__ */
