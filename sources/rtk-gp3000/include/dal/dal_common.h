/*
 * Copyright (C) 2011 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 * Purpose : Mapper Layer is used to seperate different kind of software or hardware platform
 *
 * Feature : Common function to Multiplex layer
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
#define DAL_UTIL_CHIP_REV             (dal_common_rev_get())

/*
 * Data Declaration
 */


/* Function Name:
 *      dal_common_unavail
 * Description:
 *      Unsupported function callback
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_FEATURE_NOT_SUPPORTED - Feature does not support
 * Note:
 *      None.
 */
extern int32
dal_common_unavail(void);

/* Function Name:
 *      dal_common_rev_get
 * Description:
 *      Get the revision ID
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      Revision ID
 * Note:
 *      None.
 */
extern int32
dal_common_rev_get(void);



#endif /* __DAL_COMMON_H__ */
