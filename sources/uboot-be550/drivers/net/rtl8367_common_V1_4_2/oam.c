/*
 * Copyright (C) 2013 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : RTK switch high-level API for RTL8367/RTL8367C
 * Feature : Here is a list of all functions and variables in OAM(802.3ah)  module.
 *
 */

#include <rtk_switch.h>
#include <rtk_error.h>
#include <oam.h>
#include <linux/string.h>

#include <dal/dal_mgmt.h>


/* Function Name:
 *      rtk_oam_init
 * Description:
 *      Initialize oam module.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize oam module before calling any oam APIs.
 */
rtk_api_ret_t rtk_oam_init(void)
{
    rtk_api_ret_t retVal;

    if (NULL == RT_MAPPER->oam_init)
        return RT_ERR_DRIVER_NOT_FOUND;
    
    RTK_API_LOCK();
    retVal = RT_MAPPER->oam_init();    
    RTK_API_UNLOCK();

    return retVal;
}


/* Function Name:
 *      rtk_oam_state_set
 * Description:
 *      This API set OAM state.
 * Input:
 *      enabled     -OAMstate
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_INPUT           - Error parameter
 * Note:
 *      This API set OAM state.
 */
rtk_api_ret_t rtk_oam_state_set(rtk_enable_t enabled)
{
    rtk_api_ret_t retVal;

    if (NULL == RT_MAPPER->oam_state_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    
    RTK_API_LOCK();
    retVal = RT_MAPPER->oam_state_set(enabled);    
    RTK_API_UNLOCK();

    return retVal;
}

/* Function Name:
 *      rtk_oam_state_get
 * Description:
 *      This API get OAM state.
 * Input:
 *      None.
 * Output:
 *      pEnabled        - H/W IGMP state
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_INPUT           - Error parameter
 * Note:
 *      This API set current OAM state.
 */
rtk_api_ret_t rtk_oam_state_get(rtk_enable_t *pEnabled)
{
    rtk_api_ret_t retVal;

    if (NULL == RT_MAPPER->oam_state_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    
    RTK_API_LOCK();
    retVal = RT_MAPPER->oam_state_get(pEnabled);    
    RTK_API_UNLOCK();

    return retVal;
}



/* Function Name:
 *      rtk_oam_parserAction_set
 * Description:
 *      Set OAM parser action
 * Input:
 *      port    - port id
 *      action  - parser action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      None
 */
rtk_api_ret_t  rtk_oam_parserAction_set(rtk_port_t port, rtk_oam_parser_act_t action)
{
    rtk_api_ret_t retVal;

    if (NULL == RT_MAPPER->oam_parserAction_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    
    RTK_API_LOCK();
    retVal = RT_MAPPER->oam_parserAction_set(port, action);    
    RTK_API_UNLOCK();

    return retVal;
}

/* Function Name:
 *      rtk_oam_parserAction_set
 * Description:
 *      Get OAM parser action
 * Input:
 *      port    - port id
 * Output:
 *      pAction  - parser action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      None
 */
rtk_api_ret_t  rtk_oam_parserAction_get(rtk_port_t port, rtk_oam_parser_act_t *pAction)
{
    rtk_api_ret_t retVal;

    if (NULL == RT_MAPPER->oam_parserAction_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    
    RTK_API_LOCK();
    retVal = RT_MAPPER->oam_parserAction_get(port, pAction);    
    RTK_API_UNLOCK();

    return retVal;
}


/* Function Name:
 *      rtk_oam_multiplexerAction_set
 * Description:
 *      Set OAM multiplexer action
 * Input:
 *      port    - port id
 *      action  - parser action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      None
 */
rtk_api_ret_t  rtk_oam_multiplexerAction_set(rtk_port_t port, rtk_oam_multiplexer_act_t action)
{
    rtk_api_ret_t retVal;

    if (NULL == RT_MAPPER->oam_multiplexerAction_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    
    RTK_API_LOCK();
    retVal = RT_MAPPER->oam_multiplexerAction_set(port, action);    
    RTK_API_UNLOCK();

    return retVal;
}

/* Function Name:
 *      rtk_oam_parserAction_set
 * Description:
 *      Get OAM multiplexer action
 * Input:
 *      port    - port id
 * Output:
 *      pAction  - parser action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      None
 */
rtk_api_ret_t  rtk_oam_multiplexerAction_get(rtk_port_t port, rtk_oam_multiplexer_act_t *pAction)
{
    rtk_api_ret_t retVal;

    if (NULL == RT_MAPPER->oam_multiplexerAction_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    
    RTK_API_LOCK();
    retVal = RT_MAPPER->oam_multiplexerAction_get(port, pAction);    
    RTK_API_UNLOCK();

    return retVal;
}


