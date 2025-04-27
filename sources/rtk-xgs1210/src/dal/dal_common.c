/*
 * Copyright(c) Realtek Semiconductor Corporation, 2008
 * All rights reserved.
 *
 * $Revision: 71708 $
 * $Date: 2016-09-19 11:31:17 +0800 (Mon, 19 Sep 2016) $
 *
 * Purpose : Common API for DAL
 *
 * Feature : The file have include the following module and sub-modules
 *           1) Common unsupported function
 *
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>

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

/* Module Name : */

/* Function Name:
 *      dal_common_unavail
 * Description:
 *      Return chip not support
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_CHIP_NOT_SUPPORTED   - functions not supported by this chip model
 * Note:
 *      None
 */
int32 dal_common_unavail(void)
{
    return RT_ERR_CHIP_NOT_SUPPORTED;
} /* end of dal_common_unavail */

