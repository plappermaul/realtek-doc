/*
 * Copyright (C) 2018 Realtek Semiconductor Corp.
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
 * Purpose : Definition of GPON API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) GPON
 */


#ifndef __DAL_CA8277B_GPON_H__
#define __DAL_CA8277B_GPON_H__


/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/gponv2.h>

/*
 * Symbol Definition
 */
/*
 * Data Declaration
 */


/*
 * Function Declaration
 */

/* Function Name:
 *      dal_ca8277b_gpon_init
 * Description:
 *      gpon register level initial function
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 */
extern int32 dal_ca8277b_gpon_init(void);

/* Function Name:
 *      dal_ca8277b_gpon_onuState_set
 * Description:
 *      Set ONU State .
 * Input:
 *      onuState: onu state
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 dal_ca8277b_gpon_onuState_set(rtk_gpon_onuState_t  onuState);

/* Function Name:
 *      dal_ca8277b_gpon_onuState_get
 * Description:
 *      Get ONU State
 * Input:
 *
 * Output:
 *      pOnuState: point for get onu state
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 dal_ca8277b_gpon_onuState_get(rtk_gpon_onuState_t  *pOnuState);

/* Function Name:
 *      dal_ca8277b_gpon_password_set
 * Description:
 *      Set ONU PLOAM password/registration ID .
 * Input:
 *      rtk_gpon_password_t: password
 * Output:
 *
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 dal_ca8277b_gpon_password_set(rtk_gpon_password_t  *pwd);
#endif /* __DAL_CA8277B_GPON_H__ */
