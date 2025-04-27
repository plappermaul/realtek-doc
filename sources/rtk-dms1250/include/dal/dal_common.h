/*
 * Copyright(c) Realtek Semiconductor Corporation, 2008
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Common API for DAL
 *
 * Feature : The file have include the following module and sub-modules
 *           1) Common unsupported function
 *
 */
#ifndef __DAL_COMMON_H__
#define __DAL_COMMON_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>

/*
 * Symbol Definition
 */
typedef int32 (*dal_link_change_callback_f)(int32 unit, rtk_portmask_t *pChangedPortmask);


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

extern int32
dal_common_unavail(void);

#endif /* __DAL_COMMON_H__ */
