/*
 * Copyright (C) 2012 Realtek Semiconductor Corp. 
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated, 
 * modified or distributed under the authorized license from Realtek. 
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER 
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED. 
 *
 * $Revision: 39101 $
 * $Date: 2013-05-03 17:35:27 +0800 (星期五, 03 五月 2013) $
 *
 * Purpose : Definition of SVLAN API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) OAM (802.3ah) configuration
 *
 */




/*
 * Include Files
 */
#include <common/rt_type.h>
#include <dal/rtl9602c/dal_rtl9602c.h>
#include <rtk/oam.h>
#include <dal/rtl9602c/dal_rtl9602c_oam.h>



/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */
static uint32   rtl9602c_oam_init = {INIT_NOT_COMPLETED}; 


/*
 * Macro Declaration
 */


/*
 * Function Declaration
 */

/* Module Name : OAM */

/* Function Name:
 *      dal_rtl9602c_oam_init
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
int32
dal_rtl9602c_oam_init(void)
{
    int32   ret;
    rtk_port_t port;
    
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_OAM),"%s",__FUNCTION__);

    rtl9602c_oam_init = INIT_COMPLETED;
    
    HAL_SCAN_ALL_PORT(port)
    {
        if ((ret = dal_rtl9602c_oam_parserAction_set(port, OAM_PARSER_ACTION_FORWARD)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
            rtl9602c_oam_init = INIT_NOT_COMPLETED;
            return ret;
        }
        
        if ((ret = dal_rtl9602c_oam_multiplexerAction_set(port, OAM_MULTIPLEXER_ACTION_FORWARD)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_ACL|MOD_DAL), "");
            rtl9602c_oam_init = INIT_NOT_COMPLETED;
            return ret;
        }
    }
    
    return RT_ERR_OK;
} /* end of dal_rtl9602c_oam_init */


/* Function Name:
 *      dal_rtl9602c_oam_parserAction_set
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
int32
dal_rtl9602c_oam_parserAction_set(rtk_port_t port, rtk_oam_parser_act_t action)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_OAM),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9602c_oam_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((OAM_PARSER_ACTION_END <= action), RT_ERR_INPUT);

    val = (uint32)action;
    if ((ret = reg_array_field_write(RTL9602C_OAM_P_CTRL_0r, port, REG_ARRAY_INDEX_NONE, RTL9602C_OAM_PARSERf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OAM), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_oam_parserAction_set */

/* Function Name:
 *      dal_rtl9602c_oam_parserAction_set
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
int32
dal_rtl9602c_oam_parserAction_get(rtk_port_t port, rtk_oam_parser_act_t *pAction)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_OAM),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9602c_oam_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(RTL9602C_OAM_P_CTRL_0r, port, REG_ARRAY_INDEX_NONE, RTL9602C_OAM_PARSERf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OAM), "");
        return ret;
    }

    *pAction = (rtk_oam_parser_act_t)val;    
    
    return RT_ERR_OK;
} /* end of dal_rtl9602c_oam_parserAction_get */


/* Function Name:
 *      dal_rtl9602c_oam_multiplexerAction_set
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
int32
dal_rtl9602c_oam_multiplexerAction_set(rtk_port_t port, rtk_oam_multiplexer_act_t action)
{
    int32   ret;
    uint32  val;
    
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_OAM),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9602c_oam_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((OAM_MULTIPLEXER_ACTION_END <= action), RT_ERR_INPUT);

    val = (uint32)action;
    if ((ret = reg_array_field_write(RTL9602C_OAM_P_CTRL_1r, port, REG_ARRAY_INDEX_NONE, RTL9602C_OAM_MULTIPLEXERf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OAM), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9602c_oam_multiplexerAction_set */

/* Function Name:
 *      dal_rtl9602c_oam_multiplexerAction_set
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
int32
dal_rtl9602c_oam_multiplexerAction_get(rtk_port_t port, rtk_oam_multiplexer_act_t *pAction)
{
    int32   ret;
    uint32  val;
    
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_OAM),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9602c_oam_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(RTL9602C_OAM_P_CTRL_1r, port, REG_ARRAY_INDEX_NONE, RTL9602C_OAM_MULTIPLEXERf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_OAM), "");
        return ret;
    }

    *pAction = (rtk_oam_multiplexer_act_t)val;
    
    return RT_ERR_OK;
} /* end of dal_apollo_oammultiplexerAction_get */


