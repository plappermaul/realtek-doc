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
 * Feature : Just dispatch information to Multiplex layer
 *
 */
#ifndef __DAL_EXT_PHY_MAPPER_H__
#define __DAL_EXT_PHY_MAPPER_H__

/*
 * Include Files
 */
 #if 1
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <rtk/port.h>
#endif

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */



typedef struct dal_ext_phy_mapper_s {

    /* Port */
    int32   (*port_link_get)(rtk_port_t, rtk_port_linkStatus_t *);
    int32   (*port_speedDuplex_get)(rtk_port_t, rtk_port_speed_t *, rtk_port_duplex_t *);
    int32   (*port_adminEnable_get)(rtk_port_t, rtk_enable_t *);
    int32   (*port_adminEnable_set)(rtk_port_t, rtk_enable_t);
    int32   (*port_phyAutoNegoAbility_get)(rtk_port_t, rtk_port_phy_ability_t *);
    int32   (*port_phyAutoNegoAbility_set)(rtk_port_t, rtk_port_phy_ability_t *);
    int32   (*port_phyPowerDown_get)(rtk_port_t, rtk_enable_t *);
    int32   (*port_phyPowerDown_set)(rtk_port_t, rtk_enable_t);
} dal_ext_phy_mapper_t;


extern dal_ext_phy_mapper_t      *pExtPhy_mapper;


/*NAPTR Inbound table access*/


/*
 * Macro Definition
 */


#define RT_EXT_PHY_MAPPER     pExtPhy_mapper


/*
 * Function Declaration
 */


/*
 * Function Declaration
 */
    /* Function Name:
     *      dal_ext_phy_register_mapper
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
extern int32
dal_ext_phy_register_mapper(dal_ext_phy_mapper_t *pMapper);

/*
 * Function Declaration
 */
    /* Function Name:
     *      dal_ext_phy_register_mapper
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
extern int32
dal_ext_phy_unregister_mapper(void);



/* Module Name : */


#endif /* __DAL_EXT_PHY_MAPPER_H __ */
