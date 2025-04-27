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
 * Purpose : Definition of Init API
 *
 * Feature : Initialize All Layers of RTK Module
 *
 */

#ifndef __RTK_INIT_H__
#define __RTK_INIT_H__

/*
 * Include Files
 */
#include <common/rt_type.h>

/*
 * Symbol Definition
 */
#define DEFAULT_INIT_UNIT_ID    0

/*
 * Data Declaration
 */

/*
 * Function Declaration
 */

/* Function Name:
 *      rtk_init
 * Description:
 *      Initialize the specified device
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      INIT must be initialized before using all of APIs in each modules
 * Changes:
 *      None
 */
extern int32
rtk_init(void);

/* Function Name:
 *      rtk_unit_attach
 * Description:
 *      Attach an unit to HAL/DAL system of RTK
 * Input:
 *      unit_list   - unit ID
 *      devID_list  - device ID list
 *      unit_num    - number of units in the list
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_unit_attach(uint8 *unit_list, uint8 *devID_list, int32 unit_num);

/* Function Name:
 *      rtk_unit_detach
 * Description:
 *      detach an uniti from HAL/DAL system of RTK
 * Input:
 *      unit_list   - unit ID list
 *      unit_num    - number of units in the list
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Applicable:
 *      8380, 8390, 9300, 9310
 * Note:
 *      None
 * Changes:
 *      None
 */
extern int32
rtk_unit_detach(uint8 *unit_list, int32 unit_num);

#endif /* __RTK_INIT_H__ */
