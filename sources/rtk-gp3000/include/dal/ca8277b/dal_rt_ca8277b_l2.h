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
 * Purpose : Definition of L2 API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1)  IVL/SVL mode
 */

#ifndef __DAL_RT_CA8277B_L2_H__
#define __DAL_RT_CA8277B_L2_H__
/*
 * Include Files
 */
#include <common/rt_type.h>


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
 *      dal_rt_l2_ivlSvl_get
 * Description:
 *      Configure PON misc get settings
 
 * Input:
 *      None.
 * Output:
 *      pIvlEn    - IVL is enable
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 */
extern int32
dal_rt_ca8277b_l2_ivlSvl_get(rt_enable_t *pIvlEn);

/* Function Name:
 *      dal_rt_ca8277b_ponmisc_ponQueue_mode_get
 * Description:
 *      get currect PON queue mode setting
 * Input:
 *      ivlEn    - IVL is enable
 * Output:
 *      pPonQueueMode    - current running PON queue mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
extern int32
dal_rt_ca8277b_l2_ivlSvl_set(rt_enable_t ivlEn);
    
#endif