/*
 * Copyright(c) Realtek Semiconductor Corporation, 2008
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Monitor Link fault status
 *
 * Feature :
 *
 */
#ifndef __DAL_LINKFAULTMON_H__
#define __DAL_LINKFAULTMON_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <dal/dal_mapper.h>

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
 *      dal_linkFaultMonEnable_set
 * Description:
 *      Set enable status of link fault monitor
 * Input:
 *      enable - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - initialize success
 *      RT_ERR_FAILED   - initialize fail
 *      RT_ERR_NOT_INIT - The module is not initial
 * Applicable:
 *      8390
 * Note:
 *      When enable link fault monitor, all CCM interrupt will be callback to upper layer.
 */
extern int32 dal_linkFaultMonEnable_set(rtk_enable_t enable);

/* Function Name:
 *      dal_linkFaultMon_register
 * Description:
 *      Register callback function for link fault detect notification
 * Input:
 *      linkMon_callback    - callback function for link change
 * Output:
 *      None
 * Return:
 *      RT_ERR_FAILED   - initialize fail
 *      RT_ERR_OK       - initialize success
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *
 */
extern int32 dal_linkFaultMon_register(rtk_oam_linkFaultMon_callback_t callback);

/* Function Name:
 *      dal_linkFaultMon_unregister
 * Description:
 *      Unregister callback function for link fault detect notification
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - initialize success
 *      RT_ERR_FAILED   - initialize fail
 *      RT_ERR_NOT_INIT - The module is not initial
 * Note:
 *
 */
extern int32 dal_linkFaultMon_unregister(void);

/* Function Name:
 *      dal_linkFaultMon_init
 * Description:
 *      Initial Link Fault Monitor component
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_FAILED   - initialize fail
 *      RT_ERR_OK       - initialize success
 * Note:
 *
 */
extern int32 dal_linkFaultMon_init(void);

#endif /* __DAL_LINKFAULTMON_H__ */
