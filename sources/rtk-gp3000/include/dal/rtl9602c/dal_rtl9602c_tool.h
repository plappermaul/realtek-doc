/*
 * Copyright (C) 2013 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 40104 $
 * $Date: 2013-06-08 18:48:58 +0800 (Sat, 08 Jun 2013) $
 *
 * Purpose : Definition of Switch Global API
 *
 * Feature : The file have include the following module and sub-modules
 *           (1) tool lib
 *
 */

#ifndef __DAL_RTL9602C_TOOL_H__
#define __DAL_RTL9602C_TOOL_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <hal/chipdef/chip.h>

/*
 * Symbol Definition
 */
#define RTL9602C_PATCH_INFO_IDX_MAX	256

/*
 * Data Declaration
 */

/*
 * Function Declaration
 */

/* Function Name:
 *      dal_rtl9602c_tool_get_chipSubType
 * Description:
 *      Get chip subtype.
 * Input:
 *      None
 * Output:
 *      chipSubType
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32 dal_rtl9602c_tool_get_chipSubType(uint32 *chipSubType);

/* Function Name:
 *      dal_rtl9602c_tool_get_capability
 * Description:
 *      Get chip capability.
 * Input:
 *      None
 * Output:
 *      capability
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32 dal_rtl9602c_tool_get_capability(uint32 *capability);

/* Function Name:
 *      dal_rtl9602c_tool_get_patch_info
 * Description:
 *      Get patch info.
 * Input:
 *      idx
 * Output:
 *      pData
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32 dal_rtl9602c_tool_get_patch_info(uint32 idx, uint32 *pData);

/* Function Name:
 *      dal_rtl9602c_tool_get_csExtId
 * Description:
 *      Get customer identification.
 * Input:
 *      None
 * Output:
 *      extId
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32 dal_rtl9602c_tool_get_csExtId(uint32 *extId);


#endif /* __DAL_RTL9602C_TOOL_H__ */

