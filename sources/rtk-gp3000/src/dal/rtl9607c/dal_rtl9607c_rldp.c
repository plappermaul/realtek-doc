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
 * $Revision: 51853 $
 * $Date: 2014-10-01 16:09:21 +0800 (週三, 01 十月 2014) $
 *
 * Purpose : Declaration of RLDP and RLPP API
 *
 * Feature : The file have include the following module and sub-modules
 *           1) RLDP and RLPP configuration and status
 *
 */


/*
 * Include Files
 */
#include <common/rt_type.h>
#include <dal/rtl9607c/dal_rtl9607c.h>
#include <rtk/rldp.h>
#include <dal/rtl9607c/dal_rtl9607c_rldp.h>


/*
 * Symbol Definition
 */
#define HAL_IS_RLDP_PORT(port) (HAL_IS_ETHER_PORT(port) && !HAL_IS_PON_PORT(port) && !HAL_IS_RGMII_PORT(port))

/*
 * Data Declaration
 */
static uint32    rtl9607c_rldp_init = {INIT_NOT_COMPLETED}; 


/*
 * Macro Declaration
 */

/*
 * Function Declaration
 */

/* Module Name : RLDP */

/* Function Name:
 *      dal_rtl9607c_rldp_init
 * Description:
 *      Initialize rldp module.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize rldp module before calling any rldp APIs.
 */
int32
dal_rtl9607c_rldp_init(void)
{
    int32   ret;
    //uint32  value;
    rtk_port_t port;
    rtk_rldp_config_t config;
    rtk_rldp_portConfig_t portConfig;
    rtk_rldp_portStatus_t portStatus;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RLDP),"%s",__FUNCTION__);

    rtl9607c_rldp_init = INIT_COMPLETED;

    /* Default system settings */
    config.rldp_enable = DISABLED;
    config.fc_mode = RTK_RLDP_FLOWCTRL_DROP;
    config.magic.octet[0] = 0;
    config.magic.octet[1] = 0;
    config.magic.octet[2] = 0;
    config.magic.octet[3] = 0;
    config.magic.octet[4] = 0;
    config.magic.octet[5] = 0;
    config.compare_type = RTK_RLDP_CMPTYPE_MAGIC;
    config.interval_check = 2000;
    config.num_check = 4;
    config.interval_loop = 2000;
    config.num_loop = 4;
    if ((ret = dal_rtl9607c_rldp_config_set(&config)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_RLDP|MOD_DAL), "");
        rtl9607c_rldp_init = INIT_NOT_COMPLETED;
        return ret;
    }

    /* Default port settings */
    portConfig.tx_enable = DISABLED;
    HAL_SCAN_ALL_PORT(port)
    {
        if(HAL_IS_RLDP_PORT(port))
        {
            if ((ret = dal_rtl9607c_rldp_portConfig_set(port, &portConfig)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_RLDP|MOD_DAL), "");
                rtl9607c_rldp_init = INIT_NOT_COMPLETED;
                return ret;
            }
        }
    }

    /* Reset port status - write 1 to clear */
    portStatus.loop_enter = RTK_RLDP_LOOPSTS_LOOPING;
    portStatus.loop_leave = RTK_RLDP_LOOPSTS_LOOPING;
    HAL_SCAN_ALL_PORT(port)
    {
        if(HAL_IS_RLDP_PORT(port))
        {
            if ((ret = dal_rtl9607c_rldp_portStatus_clear(port, &portStatus)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_RLDP|MOD_DAL), "");
                rtl9607c_rldp_init = INIT_NOT_COMPLETED;
                return ret;
            }
        }
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607c_rldp_init */


/* Function Name:
 *      dal_rtl9607c_rldp_config_set
 * Description:
 *      Set RLDP module configuration
 * Input:
 *      pConfig - configuration structure of RLDP
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32
dal_rtl9607c_rldp_config_set(rtk_rldp_config_t *pConfig)
{
    int32   ret;
    uint32 tmp[2];
    uint32 val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RLDP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9607c_rldp_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pConfig), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((RTK_ENABLE_END <= pConfig->rldp_enable), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_RLDP_FLOWCTRL_END <= pConfig->fc_mode), RT_ERR_INPUT);
    RT_PARAM_CHK((RTL9607C_RLDP_INTERVAL_MAX < pConfig->interval_check), RT_ERR_INPUT);
    RT_PARAM_CHK((RTL9607C_RLDP_NUM_MAX < pConfig->num_check), RT_ERR_INPUT);	
    RT_PARAM_CHK((RTL9607C_RLDP_INTERVAL_MAX < pConfig->interval_loop), RT_ERR_INPUT);
    RT_PARAM_CHK((RTL9607C_RLDP_NUM_MAX < pConfig->num_loop), RT_ERR_INPUT);

    val = pConfig->rldp_enable;
    if ((ret = reg_field_write(RTL9607C_RLDP_CTRL_1r, RTL9607C_ENf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_RLDP), "");
        return ret;
    }

    val = pConfig->fc_mode;
    if ((ret = reg_field_write(RTL9607C_RLDP_CTRL_0r, RTL9607C_ACT_RUNOUTDSCf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_RLDP), "");
        return ret;
    }

    tmp[0] =
        (pConfig->magic.octet[2] << 24) |
            (pConfig->magic.octet[3] << 16) |
            (pConfig->magic.octet[4] << 8) |
            (pConfig->magic.octet[5]);
    tmp[1] = (pConfig->magic.octet[0] << 8) | (pConfig->magic.octet[1]);

    if ((ret = reg_field_write(
                    RTL9607C_RLDP_MAGIC_NUMr,
                    RTL9607C_NUM_31_0f,
                    &tmp[0])) != RT_ERR_OK) {

        RT_ERR(ret, (MOD_RLDP | MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_field_write(
                    RTL9607C_RLDP_MAGIC_NUMr,
                    RTL9607C_NUM_47_32f,
                    &tmp[1])) != RT_ERR_OK) {

        RT_ERR(ret, (MOD_RLDP | MOD_DAL), "");
        return ret;
    }	

    val = pConfig->interval_check;
    if ((ret = reg_field_write(
                    RTL9607C_RLDP_CHK_STS_CTRLr,
                    RTL9607C_PERIODf,
                    &val)) != RT_ERR_OK) {

        RT_ERR(ret, (MOD_DAL|MOD_RLDP), "");
        return ret;
    }

    val = pConfig->num_check;
    if ((ret = reg_field_write(
                    RTL9607C_RLDP_CHK_STS_CTRLr,
                    RTL9607C_CNTf,
                    &val)) != RT_ERR_OK) {

        RT_ERR(ret, (MOD_DAL|MOD_RLDP), "");
        return ret;
    }

    val = pConfig->interval_loop;
    if ((ret = reg_field_write(
                    RTL9607C_RLDP_LP_STS_CTRLr,
                    RTL9607C_PERIODf,
                    &val)) != RT_ERR_OK) {

        RT_ERR(ret, (MOD_DAL|MOD_RLDP), "");
        return ret;
    }

    val = pConfig->num_loop;
    if ((ret = reg_field_write(
                    RTL9607C_RLDP_LP_STS_CTRLr,
                    RTL9607C_CNTf,
                    &val)) != RT_ERR_OK) {

        RT_ERR(ret, (MOD_DAL|MOD_RLDP), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607c_rldp_config_set */


/* Function Name:
 *      dal_rtl9607c_rldp_config_get
 * Description:
 *      Get RLDP module configuration
 * Input:
 *      None
 * Output:
 *      pConfig - configuration structure of RLDP
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32
dal_rtl9607c_rldp_config_get(
    rtk_rldp_config_t *pConfig)
{
    int32 ret;
    uint32 val;
    uint32 tmp[2];

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RLDP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9607c_rldp_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pConfig), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(
                    RTL9607C_RLDP_CTRL_1r,
                    RTL9607C_ENf, &val)) != RT_ERR_OK) {

        RT_ERR(ret, (MOD_DAL|MOD_RLDP), "");
        return ret;
    }
    pConfig->rldp_enable = val;
    
    if ((ret = reg_field_read(
                    RTL9607C_RLDP_CTRL_0r,
                    RTL9607C_ACT_RUNOUTDSCf, &val)) != RT_ERR_OK) {

        RT_ERR(ret, (MOD_DAL|MOD_RLDP), "");
        return ret;
    }
    pConfig->fc_mode = val;

    if ((ret = reg_field_read(
                    RTL9607C_RLDP_MAGIC_NUMr,
                    RTL9607C_NUM_31_0f,
                    &tmp[0])) != RT_ERR_OK) {

        RT_ERR(ret, (MOD_RLDP | MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_field_read(
                    RTL9607C_RLDP_MAGIC_NUMr,
                    RTL9607C_NUM_47_32f,
                    &tmp[1])) != RT_ERR_OK) {

        RT_ERR(ret, (MOD_RLDP | MOD_DAL), "");
        return ret;
    }	

    pConfig->magic.octet[0] = (tmp[1]&0x0000FF00) >> 8;
    pConfig->magic.octet[1] = tmp[1]&0x000000FF;
    pConfig->magic.octet[2] = (tmp[0]&0xFF000000) >> 24;
    pConfig->magic.octet[3] = (tmp[0]&0x00FF0000) >> 16;
    pConfig->magic.octet[4] = (tmp[0]&0x0000FF00) >> 8;
    pConfig->magic.octet[5] = tmp[0]&0x000000FF;


    if ((ret = reg_field_read(
                    RTL9607C_RLDP_CHK_STS_CTRLr,
                    RTL9607C_PERIODf,
                    &val)) != RT_ERR_OK) {

        RT_ERR(ret, (MOD_DAL|MOD_RLDP), "");
        return ret;
    }
    pConfig->interval_check = val;

    if ((ret = reg_field_read(
                    RTL9607C_RLDP_CHK_STS_CTRLr,
                    RTL9607C_CNTf,
                    &val)) != RT_ERR_OK) {

        RT_ERR(ret, (MOD_DAL|MOD_RLDP), "");
        return ret;
    }
    pConfig->num_check = val;

    if ((ret = reg_field_read(
                    RTL9607C_RLDP_LP_STS_CTRLr,
                    RTL9607C_PERIODf,
                    &val)) != RT_ERR_OK) {

        RT_ERR(ret, (MOD_DAL|MOD_RLDP), "");
        return ret;
    }
    pConfig->interval_loop = val;

    if ((ret = reg_field_read(
                    RTL9607C_RLDP_LP_STS_CTRLr,
                    RTL9607C_CNTf,
                    &val)) != RT_ERR_OK) {

        RT_ERR(ret, (MOD_DAL|MOD_RLDP), "");
        return ret;
    }
    pConfig->num_loop = val;
    pConfig->compare_type = RTK_RLDP_CMPTYPE_MAGIC;

    return RT_ERR_OK;
} /* end of dal_rtl9607c_rldp_config_get */


/* Function Name:
 *      dal_rtl9607c_rldp_portConfig_set
 * Description:
 *      Set per port RLDP module configuration
 * Input:
 *      port   - port number to be configured
 *      pPortConfig - per port configuration structure of RLDP
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32
dal_rtl9607c_rldp_portConfig_set(
    rtk_port_t port,
    rtk_rldp_portConfig_t *pPortConfig)
{
    int32 ret;
    uint32 val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RLDP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9607c_rldp_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_INPUT);
    RT_PARAM_CHK(!HAL_IS_RLDP_PORT(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pPortConfig), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((RTK_ENABLE_END <= pPortConfig->tx_enable), RT_ERR_INPUT);

    val = (uint32)pPortConfig->tx_enable;
    if ((ret = reg_array_field_write(
                    RTL9607C_RLDP_PORT_TX_ENr,
                    port,
                    REG_ARRAY_INDEX_NONE,
                    RTL9607C_ENf,
                    &val)) != RT_ERR_OK) {

        RT_ERR(ret, (MOD_DAL|MOD_RLDP), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607c_rldp_portConfig_set */


/* Function Name:
 *      dal_rtl9607c_rldp_portConfig_get
 * Description:
 *      Get per port RLDP module configuration
 * Input:
 *      port    - port number to be get
 * Output:
 *      pPortConfig - per port configuration structure of RLDP
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32
dal_rtl9607c_rldp_portConfig_get(rtk_port_t port, rtk_rldp_portConfig_t *pPortConfig)
{
    int32 ret;
    uint32 val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RLDP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9607c_rldp_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_INPUT);
    RT_PARAM_CHK(!HAL_IS_RLDP_PORT(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pPortConfig), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(
                    RTL9607C_RLDP_PORT_TX_ENr,
                    port,
                    REG_ARRAY_INDEX_NONE,
                    RTL9607C_ENf,
                    &val)) != RT_ERR_OK) {

        RT_ERR(ret, (MOD_DAL|MOD_RLDP), "");
        return ret;
    }
    pPortConfig->tx_enable = (rtk_enable_t)val;

    return RT_ERR_OK;
} /* end of dal_rtl9607c_rldp_portConfig_get */


/* Function Name:
 *      dal_rtl9607c_rldp_status_get
 * Description:
 *      Get RLDP module status
 * Input:
 *      None
 * Output:
 *      pStatus - status structure of RLDP
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32
dal_rtl9607c_rldp_status_get(rtk_rldp_status_t *pStatus)
{
    int32   ret;
    uint32 tmp[2];

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RLDP),"%s",__FUNCTION__);
	
    /* check Init status */
    RT_INIT_CHK(rtl9607c_rldp_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pStatus), RT_ERR_NULL_POINTER);

    memset(pStatus, 0, sizeof(rtk_rldp_status_t));

    if ((ret = reg_field_read(
                    RTL9607C_RLDP_RNDM_NUMr,
                    RTL9607C_NUM_31_0f,
                    &tmp[0])) != RT_ERR_OK) {

        RT_ERR(ret, (MOD_RLDP | MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_field_read(
                    RTL9607C_RLDP_RNDM_NUMr,
                    RTL9607C_NUM_47_32f,
                    &tmp[1])) != RT_ERR_OK) {

        RT_ERR(ret, (MOD_RLDP | MOD_DAL), "");
        return ret;
    }	

    pStatus->id.octet[0] = (tmp[1]&0x0000FF00) >>8;
    pStatus->id.octet[1] = tmp[1]&0x000000FF;	
    pStatus->id.octet[2] = (tmp[0]&0xFF000000) >>24;
    pStatus->id.octet[3] = (tmp[0]&0x00FF0000) >>16;
    pStatus->id.octet[4] = (tmp[0]&0x0000FF00) >>8;
    pStatus->id.octet[5] = tmp[0]&0x000000FF;

    return RT_ERR_OK;
} /* end of dal_rtl9607c_rldp_status_get */


/* Function Name:
 *      dal_rtl9607c_rldp_portStatus_get
 * Description:
 *      Get RLDP module status
 * Input:
 *      port    - port number to be get
 * Output:
 *      pPortStatus - per port status structure of RLDP
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32
dal_rtl9607c_rldp_portStatus_get(
    rtk_port_t port,
    rtk_rldp_portStatus_t *pPortStatus)
{
    int32 ret;
    uint32 val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RLDP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9607c_rldp_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_INPUT);
    RT_PARAM_CHK(!HAL_IS_RLDP_PORT(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pPortStatus), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(
                    RTL9607C_RLDP_PORT_LP_STSr,
                    port,
                    REG_ARRAY_INDEX_NONE,
                    RTL9607C_STSf,
                    &val)) != RT_ERR_OK) {

        RT_ERR(ret, (MOD_DAL|MOD_RLDP), "");
        return ret;
    }
    pPortStatus->loop_status = val;

    if ((ret = reg_array_field_read(
                    RTL9607C_RLDP_PORT_LP_ENTER_STSr,
                    port,
                    REG_ARRAY_INDEX_NONE,
                    RTL9607C_STSf,
                    &val)) != RT_ERR_OK) {

        RT_ERR(ret, (MOD_DAL|MOD_RLDP), "");
        return ret;
    }
    pPortStatus->loop_enter = val;

    if ((ret = reg_array_field_read(
                    RTL9607C_RLDP_PORT_LP_LEAVE_STSr,
                    port,
                    REG_ARRAY_INDEX_NONE,
                    RTL9607C_STSf,
                    &val)) != RT_ERR_OK) {

        RT_ERR(ret, (MOD_DAL|MOD_RLDP), "");
        return ret;
    }
    pPortStatus->loop_leave = val;

    if ((ret = reg_array_field_read(
                    RTL9607C_RLDP_PORT_LP_PNUMr,
                    port,
                    REG_ARRAY_INDEX_NONE,
                    RTL9607C_P_NUMf,
                    &val)) != RT_ERR_OK) {

        RT_ERR(ret, (MOD_DAL|MOD_RLDP), "");
        return ret;
    }
    pPortStatus->looped_port = val;

    return RT_ERR_OK;
} /* end of dal_rtl9607c_rldp_portStatus_get */



/* Function
 *          Name:
 *
 *                  dal_rtl9607c_rldp_portStatus_clear
 * Description
 *          :
 *      Clear
 *                  RLDP module status * Input:
 *
 *      port    - port number to be clear
 *      pPortStatus - per port status structure of RLDP
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      Clear operation effect loop_enter and loop_leave only, other field in
 *      the structure are don't care
 */
int32
dal_rtl9607c_rldp_portStatus_clear(
    rtk_port_t port,
    rtk_rldp_portStatus_t *pPortStatus)
{
    int32 ret;
    uint32 val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RLDP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rtl9607c_rldp_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_INPUT);
    RT_PARAM_CHK(!HAL_IS_RLDP_PORT(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pPortStatus), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(
        (RTK_RLDP_LOOPSTS_END <= pPortStatus->loop_enter),
        RT_ERR_INPUT);
    RT_PARAM_CHK(
        (RTK_RLDP_LOOPSTS_END <= pPortStatus->loop_leave),
        RT_ERR_INPUT);

    if(RTK_RLDP_LOOPSTS_LOOPING == pPortStatus->loop_enter) {
        val = pPortStatus->loop_enter;
        if ((ret = reg_array_field_write(
                        RTL9607C_RLDP_PORT_LP_ENTER_STSr,
                        port,
                        REG_ARRAY_INDEX_NONE,
                        RTL9607C_STSf,
                        &val)) != RT_ERR_OK) {

            RT_ERR(ret, (MOD_DAL|MOD_RLDP), "");
            return ret;
        }
    }   

    if(RTK_RLDP_LOOPSTS_LOOPING == pPortStatus->loop_leave) {
        val = pPortStatus->loop_leave;
        if ((ret = reg_array_field_write(
                        RTL9607C_RLDP_PORT_LP_LEAVE_STSr,
                        port,
                        REG_ARRAY_INDEX_NONE,
                        RTL9607C_STSf,
                        &val)) != RT_ERR_OK) {

            RT_ERR(ret, (MOD_DAL|MOD_RLDP), "");
            return ret;
        }
    }

    return RT_ERR_OK;
} /* end of dal_rtl9607c_rldp_portStatus_clear */


