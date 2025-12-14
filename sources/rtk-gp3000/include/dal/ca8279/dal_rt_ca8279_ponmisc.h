/*
 * Copyright (C) 2019 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: $
 * $Date: $
 *
 * Purpose : Definition of PON misc API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) PON misc
 */

#ifndef __DAL_RT_CA8279_PONMISC_H__
#define __DAL_RT_CA8279_PONMISC_H__
/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/ponmac.h>

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

/* Function Name:
 *      dal_rt_ca8279_ponmisc_init
 * Description:
 *      Configure PON misc initial settings
 * Input:
 *      None.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
extern int32
dal_rt_ca8279_ponmisc_init(void);

/* Function Name:
 *      dal_rt_ca8279_ponmisc_mode_get
 * Description:
 *      Configure PON misc get settings
 * Input:
 *      None.
 * Output:
 *      pPonMode    - current running PON mode
 *      pPonSpeed   - current running PON speed
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_FEATURE_NOT_SUPPORTED
 *      RT_ERR_NULL_POINTER
 * Note:
 */
extern int32
dal_rt_ca8279_ponmisc_mode_get(rt_ponmisc_ponMode_t *pPonMode,rt_ponmisc_ponSpeed_t *pPonSpeed);

#endif