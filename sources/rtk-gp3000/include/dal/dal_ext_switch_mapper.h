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
#ifndef __DAL_EXT_SWITCH_MAPPER_H__
#define __DAL_EXT_SWITCH_MAPPER_H__

/*
 * Include Files
 */
 #if 1
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <rtk/port.h>
#include <rtk/stat.h>
#include <rtk/switch.h>
#endif

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */



typedef struct dal_ext_switch_mapper_s {
    /* Switch */
    int32    (*switch_deviceInfo_get)(rtk_switch_devInfo_t *pDevInfo);

    /* Port */
    int32   (*port_link_get)(rtk_port_t, rtk_port_linkStatus_t *);
    int32   (*port_speedDuplex_get)(rtk_port_t, rtk_port_speed_t *, rtk_port_duplex_t *);
    int32   (*port_adminEnable_get)(rtk_port_t, rtk_enable_t *);
    int32   (*port_adminEnable_set)(rtk_port_t, rtk_enable_t);
    int32   (*port_phyAutoNegoAbility_get)(rtk_port_t, rtk_port_phy_ability_t *);
    int32   (*port_phyAutoNegoAbility_set)(rtk_port_t, rtk_port_phy_ability_t *);
    
    /* statistics */
    int32   (*stat_port_reset)(rtk_port_t);
    int32   (*stat_port_get)(rtk_port_t, rtk_stat_port_type_t, uint64 *);
    int32   (*stat_port_getAll)(rtk_port_t, rtk_stat_port_cntr_t *);

    /* Qos */
    int32 (*rate_portEgrBandwidthCtrlRate_get)(rtk_port_t, uint32 *);
    int32 (*rate_portEgrBandwidthCtrlRate_set)(rtk_port_t, uint32);

} dal_ext_switch_mapper_t;


extern dal_ext_switch_mapper_t      *pExtSw_mapper;


/*NAPTR Inbound table access*/


/*
 * Macro Definition
 */

#define IS_EXT_SWITCH_PORT(_port)   (_port >= CONFIG_EXTERNAL_SWITCH_PORT_OFFSET)
#define RT_PORT_TO_EXT_SWITCH_PORT(_port) (_port - CONFIG_EXTERNAL_SWITCH_PORT_OFFSET)
#define EXT_SWITCH_ORT_TO_RT_PORT(_port) (_port + CONFIG_EXTERNAL_SWITCH_PORT_OFFSET)



#define RT_EXT_SWITCH_MAPPER     pExtSw_mapper


/*
 * Function Declaration
 */


/*
 * Function Declaration
 */
    /* Function Name:
     *      dal_ext_switch_register_mapper
     * Description:
     *      Register mapper function for external switch dal. 
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
dal_ext_switch_register_mapper(dal_ext_switch_mapper_t *pMapper);

/*
 * Function Declaration
 */
    /* Function Name:
     *      dal_ext_switch_register_mapper
     * Description:
     *      Register mapper function for external switch dal. 
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
dal_ext_switch_unregister_mapper(void);



/* Module Name : */


#endif /* __DAL_EXT_SWITCH_MAPPER_H __ */
