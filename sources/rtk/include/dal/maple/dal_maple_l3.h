/*
 * Copyright(c) Realtek Semiconductor Corporation, 2008
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Definition those public L3 APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *            1) L3 routing
 *
 */

#ifndef __DAL_MAPLE_L3_H__
#define __DAL_MAPLE_L3_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/l3.h>
#include <dal/dal_mgmt.h>


/* Function Name:
 *      dal_maple_l3Mapper_init
 * Description:
 *      Hook l3 module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must Hook l3 module before calling any l3 APIs.
 */
extern int32
dal_maple_l3Mapper_init(dal_mapper_t *pMapper);

/* Function Name:
 *      dal_maple_l3_init
 * Description:
 *      Initialize L3 module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize L3 module before calling any L3 APIs.
 */
extern int32
dal_maple_l3_init(uint32 unit);

/* Module Name : Layer3 routing */

/* Function Name:
 *      dal_maple_l3_routeEntry_get
 * Description:
 *      Get L3 rounting entry.
 * Input:
 *      unit   - unit id
 *      index  - index of host MAC address
 * Output:
 *      pEntry - L3 route entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The route entry is indexed by L2 NextHop entry.
 *      Valid index range:
 *      - 0~2047 in 8390
 *      - 0~511 in 8380
 */
extern int32
dal_maple_l3_routeEntry_get(uint32 unit, uint32 index, rtk_l3_routeEntry_t *pEntry);

/* Function Name:
 *      dal_maple_l3_routeEntry_set
 * Description:
 *      Set L3 rounting entry.
 * Input:
 *      unit   - unit id
 *      index  - index of host MAC address
 *      pEntry - L3 route entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None.
 */
extern int32
dal_maple_l3_routeEntry_set(uint32 unit, uint32 index, rtk_l3_routeEntry_t *pEntry);



#endif /*__DAL_MAPLE_L3_H__*/

