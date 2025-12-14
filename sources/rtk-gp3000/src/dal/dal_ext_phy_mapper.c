/*
 * Copyright (C) 2009 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <dal/dal_ext_phy_mapper.h>
#include <linux/module.h>





/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */



dal_ext_phy_mapper_t      *pExtPhy_mapper = NULL;


/*
 * Macro Declaration
 */

/*
 * Function Declaration
 */
    /* Function Name:
     *      dal_ext_switch_register_mapper
     * Description:
     *      Register mapper function for external phy dal. 
     * Input:
     *      pMapper pointer to mapper funcion
     * Output:
     *      None
     * Return:
     *      RT_ERR_FAILED   - register fail
     *      RT_ERR_OK       - register success
     * Note:
     *      
     */
int32
dal_ext_phy_register_mapper(dal_ext_phy_mapper_t *pMapper)
{
    pExtPhy_mapper = pMapper;

    return RT_ERR_OK;
}


/*
 * Function Declaration
 */
    /* Function Name:
     *      dal_ext_switch_register_mapper
     * Description:
     *      Register mapper function for external phy dal. 
     * Input:
     *      None
     * Output:
     *      None
     * Return:
     *      RT_ERR_FAILED   - register fail
     *      RT_ERR_OK       - register success
     * Note:
     *      
     */
int32
dal_ext_phy_unregister_mapper(void)
{
    pExtPhy_mapper = NULL;

    return RT_ERR_OK;
}


/*EXPORT function*/
EXPORT_SYMBOL(dal_ext_phy_register_mapper);
EXPORT_SYMBOL(dal_ext_phy_unregister_mapper);

