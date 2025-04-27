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
 * $Revision: 81289 $
 * $Date: 2017-08-15 20:45:44 +0800 (Tue, 15 Aug 2017) $
 *
 * Purpose : The data for setting MAC Serdes parameters
 *
 * Feature : The data struct of  MAC Serdes parameters
 *
 */

#ifndef __DAL_CYPRESS_SDS_H__
#define __DAL_CYPRESS_SDS_H__

#include <dal/dal_mapper.h>

/* Function Name:
 *      dal_cypress_sdsMapper_init
 * Description:
 *      Hook SerDes module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must Hook SerDes module before calling any port APIs.
 */
extern int32
dal_cypress_sdsMapper_init(dal_mapper_t *pMapper);

/* Function Name:
 *      dal_cypress_sds_init
 * Description:
 *      Initialize switch module of the specified device.
 * Input:
 *      unit          - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      1. Module must be initialized before using all of APIs in this module
 */
extern int32
dal_cypress_sds_init(uint32 unit);

/* Function Name:
 *      dal_cypress_sds_symErr_get
 * Description:
 *      Get the SerDes symbol error count.
 * Input:
 *      unit      - unit id
 *      unit      - SerDes id
 * Output:
 *      info      - symbol error count information
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_sds_symErr_get(uint32 unit, uint32 sds, rtk_sds_symErr_t *info);

/* Function Name:
 *      dal_cypress_sds_linkSts_get
 * Description:
 *      Get the SerDes link status.
 * Input:
 *      unit      - unit id
 *      sds       - SerDes id
 * Output:
 *      info      - link status information
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_cypress_sds_linkSts_get(uint32 unit, uint32 sds, rtk_sds_linkSts_t *info);

#endif /* __DAL_CYPRESS_SDS_H__ */
