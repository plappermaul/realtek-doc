/*
 * Copyright (C) 2015 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision:
 * $Date: $
 *
 * Purpose : Definition those public Interrupt APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *           (1) Interrupt parameter settings
 */


/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/intr.h>
#include <rtk//rt/rt_intr.h>
#include <rtk//rt/rt_gpon.h>
#include <dal/rtl8277c/dal_rtl8277c.h>
#include <dal/rtl8277c/dal_rtl8277c_intr.h>
#include <dal/rtl8277c/dal_rtl8277c_switch.h>
#include <osal/time.h>

#include <ca_event.h>
#include <event_core.h>

#include <cortina-api/include/eth_port.h>

#ifdef CONFIG_SDK_MODULES
#include <module/intr_bcaster/intr_bcaster.h>
#endif

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
static uint32 intr_init = INIT_NOT_COMPLETED;

static rtk_enable_t LUNA_G3_INTR_REG[RT_INTR_TYPE_END];

static rt_intr_isr_rx_callback LUNA_G3_INTR_ISR[RT_INTR_TYPE_END] = {NULL};
/*
 * Function Declaration
 */

ca_uint32_t ca_rtk_link_change_intr(ca_device_id_t device_id,ca_uint32_t event_id,ca_void_t *event,ca_uint32_t len,ca_void_t *cookie)
{
    ca_event_eth_port_link_t* eth_port_link_ptr = (ca_event_eth_port_link_t *)event;
    int32 portId;
    uint32 intrSubType;
    rt_intr_link_change_t linkChg;
    
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_INTR),"%s",__FUNCTION__);

    //reference to aal_fdb.c
    portId = PORT_ID(eth_port_link_ptr->port_id);
    linkChg.port = portId;
    if(eth_port_link_ptr->status == CA_PORT_LINK_UP)
    {
        intrSubType = INTR_STATUS_LINKUP;
        linkChg.status = RT_INTR_LINKCHG_LINKUP;
    }
    else if(eth_port_link_ptr->status == CA_PORT_LINK_DOWN)
    {
        intrSubType = INTR_STATUS_LINKDOWN;
        linkChg.status = RT_INTR_LINKCHG_LINKDOWN;
    }
    else
    {
        /*not support type*/
        return CA_EVT_OK;
    }

#ifdef CONFIG_SDK_MODULES
    queue_broadcast(MSG_TYPE_LINK_CHANGE, intrSubType, portId, ENABLED);
#endif

    if(LUNA_G3_INTR_ISR[INTR_TYPE_LINK_CHANGE] != NULL)
        LUNA_G3_INTR_ISR[INTR_TYPE_LINK_CHANGE](&linkChg);

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_INTR),"%s",__FUNCTION__);

    return CA_EVT_OK;
}

ca_uint32_t ca_rtk_onu_state_change_intr(ca_device_id_t device_id,ca_uint32_t event_id,ca_void_t *event,ca_uint32_t len,ca_void_t *cookie)
{
    int32 portId;
    uint32 intrSubType;
    ca_event_gpon_stat_t* gpon_state_ptr = (ca_event_gpon_stat_t *)event;
    rt_gpon_onuState_t onuState;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_INTR),"%s",__FUNCTION__);

    portId = HAL_GET_PON_PORT();
    if(CA_PON_MODE == ONU_PON_MAC_XGPON1 || CA_PON_MODE == ONU_PON_MAC_XGSPON)
    {
        if(gpon_state_ptr->status == 0)
        {
            intrSubType = 1; //O1
        }
        else if(gpon_state_ptr->status == 1)  //(O23 in 10G GPON)
        {
            intrSubType = 3; //O3  
        }
        else if(gpon_state_ptr->status == 2)
        {
            intrSubType = 4; //O4
        }
        else if(gpon_state_ptr->status == 3)
        {
            intrSubType = 5; //O5
        }
        else if(gpon_state_ptr->status == 4)
        {
            intrSubType = 6; //O6
        }
        else if(gpon_state_ptr->status == 5)
        {
            intrSubType = 7; //O7
        }
        else 
        {
            RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_INTR),"%s: ignored state %d",__FUNCTION__, gpon_state_ptr->status);
    
            return CA_EVT_OK;
        }
    }
    else if(CA_PON_MODE == ONU_PON_MAC_GPON)
    {
        if(gpon_state_ptr->status == 0)
        {
            intrSubType = 1; //O1
        }
        else if(gpon_state_ptr->status == 1)
        {
            intrSubType = 2; //O2  
        }
        else if(gpon_state_ptr->status == 2)
        {
            intrSubType = 3; //O3
        }
        else if(gpon_state_ptr->status == 3)
        {
            intrSubType = 4; //O4
        }
        else if(gpon_state_ptr->status == 4)
        {
            intrSubType = 5; //O5
        }
        else if(gpon_state_ptr->status == 5)
        {
            intrSubType = 6; //O6
        }
        else if(gpon_state_ptr->status == 6)
        {
            intrSubType = 7; //O7
        }
        else 
        {
            RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_INTR),"%s: ignored state %d",__FUNCTION__, gpon_state_ptr->status);
    
            return CA_EVT_OK;
        }
    }
    else 
    {
        RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_INTR),"%s: ignored state %d",__FUNCTION__, gpon_state_ptr->status);

        return CA_EVT_OK;
    }
    
#ifdef CONFIG_SDK_MODULES
    queue_broadcast(MSG_TYPE_ONU_STATE, intrSubType, portId, ENABLED);
#endif

    if(LUNA_G3_INTR_ISR[INTR_TYPE_GPON] != NULL)
    {
        onuState = intrSubType;
        LUNA_G3_INTR_ISR[INTR_TYPE_GPON](&onuState);
    }

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_INTR),"%s",__FUNCTION__);

    return CA_EVT_OK;
}

ca_uint32_t ca_rtk_onu_dying_gasp_intr(ca_device_id_t device_id,ca_uint32_t event_id,ca_void_t *event,ca_uint32_t len,ca_void_t *cookie)
{
    int32 portId;
    uint32 intrSubType=0;

    printk(KERN_NOTICE "dying gasp interrupt!\n");
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_INTR),"%s",__FUNCTION__);

    portId = HAL_GET_PON_PORT();
#ifdef CONFIG_SDK_MODULES
    queue_broadcast(MSG_TYPE_DYING_GASP, intrSubType, portId, ENABLED);
#endif

    if(LUNA_G3_INTR_ISR[INTR_TYPE_DYING_GASP] != NULL)
        LUNA_G3_INTR_ISR[INTR_TYPE_DYING_GASP](NULL);

    osal_time_mdelay(100);
    /* reboot ONT self to prevent the situation of low power, but ONT alive */
    if (RT_ERR_OK != dal_rtl8277c_switch_chip_reset())
    {
        RT_DBG(LOG_ERR, (MOD_DAL|MOD_INTR),"%s",__FUNCTION__);
        return CA_EVT_OK;
    }

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_INTR),"%s",__FUNCTION__);

    return CA_EVT_OK;
}

ca_uint32_t ca_rtk_gpon_alarm_intr(ca_device_id_t device_id,ca_uint32_t event_id,ca_void_t *event,ca_uint32_t len,ca_void_t *cookie)
{
    int32 portId;
    ca_event_gpon_event_t *gpon_evt = (ca_event_gpon_event_t *)event;

    printk(KERN_NOTICE "gpon alarm interrupt!\n");
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_INTR),"%s",__FUNCTION__);

    if(gpon_evt == NULL)
        return CA_EVT_OK;

    portId = HAL_GET_PON_PORT();

    if(gpon_evt->event == CA_GONT_LOS && LUNA_G3_INTR_REG[INTR_TYPE_LOS] == ENABLED)
    {
#ifdef CONFIG_SDK_MODULES
        queue_broadcast(MSG_TYPE_LOS, gpon_evt->status, portId, ENABLED);
#endif
        if(LUNA_G3_INTR_ISR[INTR_TYPE_LOS] != NULL)
            LUNA_G3_INTR_ISR[INTR_TYPE_LOS](NULL);
    }

    if(gpon_evt->event == CA_GONT_LOF && LUNA_G3_INTR_REG[INTR_TYPE_LOF] == ENABLED)
    {
#ifdef CONFIG_SDK_MODULES
        queue_broadcast(MSG_TYPE_LOF, gpon_evt->status, portId, ENABLED);
#endif
        if(LUNA_G3_INTR_ISR[INTR_TYPE_LOF] != NULL)
            LUNA_G3_INTR_ISR[INTR_TYPE_LOF](NULL);
    }

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_INTR),"%s",__FUNCTION__);

    return CA_EVT_OK;
}

/* Function Name:
 *      dal_rtl8277c_intr_init
 * Description:
 *      Initialize interrupt module.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize interrupt module before calling any interrupt APIs.
 */
int32
dal_rtl8277c_intr_init(void)
{
    int32 ret = RT_ERR_OK;
    int i;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_INTR),"%s",__FUNCTION__);

    intr_init = INIT_COMPLETED;

    for(i=0;i<INTR_TYPE_END;i++)
    {
        LUNA_G3_INTR_REG[i] = DISABLED;
    }

    if((ret = dal_rtl8277c_intr_imr_set(INTR_TYPE_LINK_CHANGE,ENABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_INTR | MOD_DAL), "");
        intr_init = INIT_NOT_COMPLETED;
        return ret;
    }

    if((ret = dal_rtl8277c_intr_imr_set(INTR_TYPE_GPON,ENABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_INTR | MOD_DAL), "");
        intr_init = INIT_NOT_COMPLETED;
        return ret;
    }
    
    if((ret = dal_rtl8277c_intr_imr_set(INTR_TYPE_DYING_GASP,ENABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_INTR | MOD_DAL), "");
        intr_init = INIT_NOT_COMPLETED;
        return ret;
    }

    if((ret = dal_rtl8277c_intr_imr_set(INTR_TYPE_LOS,ENABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_INTR | MOD_DAL), "");
        intr_init = INIT_NOT_COMPLETED;
        return ret;
    }

    if((ret = dal_rtl8277c_intr_imr_set(INTR_TYPE_LOF,ENABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_INTR | MOD_DAL), "");
        intr_init = INIT_NOT_COMPLETED;
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_rtl8277c_intr_init */

/* Function Name:
 *      dal_rtl8277c_intr_imr_get
 * Description:
 *      Get interrupt mask.
 * Input:
 *      intr            - interrupt type
 *      pEnable      - pointer of return status
 *
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_INPUT
 *      RT_ERR_OUT_OF_RANGE
 * Note:
 *      None.
 */
int32
dal_rtl8277c_intr_imr_get(rtk_intr_type_t intr, rtk_enable_t *pEnable)
{
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_INTR),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(intr_init);

    /* parameter check */
    RT_PARAM_CHK((INTR_TYPE_END <= intr), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(intr>INTR_TYPE_END, RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    switch(intr)
    {
        case INTR_TYPE_LINK_CHANGE:
            break;
        case INTR_TYPE_GPON:
            break;
        case INTR_TYPE_DYING_GASP:
            break;
        case INTR_TYPE_LOS:
            break;
        case INTR_TYPE_LOF:
            break;
        default:
            RT_ERR(ret, (MOD_DAL|MOD_INTR), "");
            return RT_ERR_INPUT;

    }
    *pEnable = LUNA_G3_INTR_REG[intr];

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8277c_intr_imr_set
 * Description:
 *      Set interrupt mask.
 * Input:
 *      intr          - interrupt type
 *      enable      - interrupt state
 *
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_OUT_OF_RANGE
 * Note:
 *     None.
 */
int32
dal_rtl8277c_intr_imr_set(rtk_intr_type_t intr, rtk_enable_t enable)
{
    ca_status_t ret=CA_E_OK;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_INTR),"%s",__FUNCTION__);
    RT_INIT_CHK(intr_init);

    /* parameter check */
    RT_PARAM_CHK((INTR_TYPE_END <= intr), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(intr>INTR_TYPE_END, RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_OUT_OF_RANGE);

    switch(intr)
    {
        case INTR_TYPE_LINK_CHANGE:
            if(LUNA_G3_INTR_REG[intr] == DISABLED && enable == ENABLED)
            {
                if((ret = ca_event_register(0,CA_EVENT_ETH_PORT_LINK,ca_rtk_link_change_intr,NULL))!=CA_E_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_INTR), "");
                    return RT_ERR_FAILED;
                }
            }
            if(LUNA_G3_INTR_REG[intr] == ENABLED && enable == DISABLED)
            {
                if((ret = ca_event_deregister(0,CA_EVENT_ETH_PORT_LINK,ca_rtk_link_change_intr))!=CA_E_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_INTR), "");
                    return RT_ERR_FAILED;
                }
            }
            break;
        case INTR_TYPE_GPON:
            if(LUNA_G3_INTR_REG[intr] == DISABLED && enable == ENABLED)
            {
                if((ret = ca_event_register(0,CA_EVENT_GPON_REG_STAT_CHNG,ca_rtk_onu_state_change_intr,NULL))!=CA_E_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_INTR), "");
                    return RT_ERR_FAILED;
                }
            }
            if(LUNA_G3_INTR_REG[intr] == ENABLED && enable == DISABLED)
            {
                if((ret = ca_event_deregister(0,CA_EVENT_GPON_REG_STAT_CHNG, ca_rtk_onu_state_change_intr))!=CA_E_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_INTR), "");
                    return RT_ERR_FAILED;
                }
            }
            break;
        case INTR_TYPE_DYING_GASP:
            if(LUNA_G3_INTR_REG[intr] == DISABLED && enable == ENABLED)
            {
                if((ret = ca_event_register(0,CA_EVENT_PON_DYING_GASP,ca_rtk_onu_dying_gasp_intr,NULL))!=CA_E_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_INTR), "");
                    return RT_ERR_FAILED;
                }
            }
            if(LUNA_G3_INTR_REG[intr] == ENABLED && enable == DISABLED)
            {
                if((ret = ca_event_deregister(0,CA_EVENT_PON_DYING_GASP,ca_rtk_onu_dying_gasp_intr))!=CA_E_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_INTR), "");
                    return RT_ERR_FAILED;
                }
            }
            break;
        case INTR_TYPE_LOS:
            if(LUNA_G3_INTR_REG[intr] == DISABLED && enable == ENABLED)
            {
                if(LUNA_G3_INTR_REG[INTR_TYPE_LOF] == ENABLED)
                    break;
                if((ret = ca_event_register(0,CA_EVENT_GPON_ALARM_EVENT,ca_rtk_gpon_alarm_intr,NULL))!=CA_E_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_INTR), "");
                    return RT_ERR_FAILED;
                }
            }
            if(LUNA_G3_INTR_REG[intr] == ENABLED && enable == DISABLED)
            {
                if(LUNA_G3_INTR_REG[INTR_TYPE_LOF] == ENABLED)
                    break;
                if((ret = ca_event_deregister(0,CA_EVENT_GPON_ALARM_EVENT,ca_rtk_gpon_alarm_intr))!=CA_E_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_INTR), "");
                    return RT_ERR_FAILED;
                }
            }
            break;
        case INTR_TYPE_LOF:
            if(LUNA_G3_INTR_REG[intr] == DISABLED && enable == ENABLED)
            {
                if(LUNA_G3_INTR_REG[INTR_TYPE_LOS] == ENABLED)
                    break;
                if((ret = ca_event_register(0,CA_EVENT_GPON_ALARM_EVENT,ca_rtk_gpon_alarm_intr,NULL))!=CA_E_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_INTR), "");
                    return RT_ERR_FAILED;
                }
            }
            if(LUNA_G3_INTR_REG[intr] == ENABLED && enable == DISABLED)
            {
                if(LUNA_G3_INTR_REG[INTR_TYPE_LOS] == ENABLED)
                    break;
                if((ret = ca_event_deregister(0,CA_EVENT_GPON_ALARM_EVENT,ca_rtk_gpon_alarm_intr))!=CA_E_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_INTR), "");
                    return RT_ERR_FAILED;
                }
            }
            break;

        default:
            RT_ERR(ret, (MOD_DAL|MOD_INTR), "");
            return RT_ERR_INPUT;

    }
    LUNA_G3_INTR_REG[intr] = enable;

    return RT_ERR_OK;
} /* end of dal_rtl8277c_intr_imr_set */

/* Function Name:
 *      dal_rtl8277c_intr_isr_rx_callback_register
 * Description:
 *      Register isr callback function.
 * Input:
 *      isrRx   - pointer to the callback function for interrupt
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32
dal_rt_rtl8277c_intr_isr_rx_callback_register(rt_intr_type_t intr,rt_intr_isr_rx_callback isrRx)
{
    int32   ret;

    /* check Init status */
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_INTR),"%s",__FUNCTION__);
    RT_INIT_CHK(intr_init);

    /* parameter check */
    RT_PARAM_CHK((INTR_TYPE_END <= intr), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(intr>INTR_TYPE_END, RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == isrRx), RT_ERR_NULL_POINTER);

    /* function body */

    LUNA_G3_INTR_ISR[intr] = isrRx;

    
    return RT_ERR_OK;
}   /* end of dal_rt_rtl8277c_intr_isr_rx_callback_register */
