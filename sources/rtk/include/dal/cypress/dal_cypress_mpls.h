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
 * Purpose : Definition those public MPLS routing APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *              1) MPLS
 */

#ifndef __DAL_CYPRESS_MPLS_H__
#define __DAL_CYPRESS_MPLS_H__


/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <rtk/mpls.h>
#include <dal/dal_mapper.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */


/*
 * Macro Declaration
 */


/*
 * Function Declaration
 */

/* Module Name : MPLS */

/* Function Name:
 *      dal_cypress_mplsMapper_init
 * Description:
 *      Hook mpls module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must Hook mpls module before calling any mpls APIs.
 */
extern int32
dal_cypress_mplsMapper_init(dal_mapper_t *pMapper);


/* Function Name:
 *      dal_cypress_mpls_init
 * Description:
 *      Initialize mpls module of the specified device.
 * Input:
 *      unit    - Device number
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      None
 */
extern int32
dal_cypress_mpls_init(uint32 unit);

/* Function Name:
 *      dal_cypress_mpls_ttlInherit_get
 * Description:
 *      Get MPLS TTL inherit properties
 * Input:
 *      unit    - Device number
 * Output:
 *      inherit - MPLS TTL inherit information
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_mpls_ttlInherit_get(uint32 unit, rtk_mpls_ttlInherit_t *inherit);

/* Function Name:
 *      dal_cypress_mpls_ttlInherit_set
 * Description:
 *      Set MPLS TTL inherit properties
 * Input:
 *      unit    - Device number
 *      inherit - MPLS TTL inherit information
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_cypress_mpls_ttlInherit_set(uint32 unit, rtk_mpls_ttlInherit_t inherit);

/* Function Name:
 *      dal_cypress_mpls_encap_get
 * Description:
 *      Get MPLS encapsulation properties
 * Input:
 *      unit    - Device number
 *      lib_idx - the index of MPLS table
 * Output:
 *      info    - MPLS encapsulation information
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None.
 */
extern int32
dal_cypress_mpls_encap_get(uint32 unit, uint32 lib_idx,
    rtk_mpls_encap_t *info);

/* Function Name:
 *      dal_cypress_mpls_encap_set
 * Description:
 *      Set MPLS encapsulation properties
 * Input:
 *      unit    - Device number
 *      info    - MPLS encapsulation information
 *      lib_idx - the index of MPLS table
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_cypress_mpls_encap_set(uint32 unit, uint32 lib_idx,
    rtk_mpls_encap_t *info);

/* Function Name:
 *      dal_cypress_mpls_enable_get
 * Description:
 *      Get MPLS state
 * Input:
 *      unit    - Device number
 * Output:
 *      pEnable - MPLS state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_mpls_enable_get(uint32 unit, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_cypress_mpls_enable_set
 * Description:
 *      Set MPLS state
 * Input:
 *      unit    - Device number
 *      enable  - state of MPLS
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_cypress_mpls_enable_set(uint32 unit, rtk_enable_t enable);

#endif /* __DAL_CYPRESS_MPLS_H__ */

