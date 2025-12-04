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
#include <dal/rtl8198f/dal_rtl8198f.h>
#include <dal/rtl8198f/dal_rtl8198f_intr.h>
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

static rtk_enable_t RTL8198F_INTR_REG[INTR_TYPE_END];

/*
 * Function Declaration
 */

ca_uint32_t ca_rtk_link_change_intr(ca_device_id_t device_id,ca_uint32_t event_id,ca_void_t *event,ca_uint32_t len,ca_void_t *cookie)
{
#ifdef CONFIG_SDK_MODULES
    ca_event_eth_port_link_t* eth_port_link_ptr = (ca_event_eth_port_link_t *)event;
    int32 portId;
    uint32 intrSubType;
    
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_INTR),"%s",__FUNCTION__);

    //reference to aal_fdb.c
    portId = PORT_ID(eth_port_link_ptr->port_id);
    if(eth_port_link_ptr->status == 1)
    {
        intrSubType = INTR_STATUS_LINKDOWN;
    }
    if(eth_port_link_ptr->status == 0)
    {
        intrSubType = INTR_STATUS_LINKUP;
    }
    queue_broadcast(MSG_TYPE_LINK_CHANGE, intrSubType, portId, ENABLED);
#endif

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_INTR),"%s",__FUNCTION__);

    return CA_EVT_OK;
}

/* Function Name:
 *      dal_rtl8198f_intr_init
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
dal_rtl8198f_intr_init(void)
{
    int32 ret = RT_ERR_OK;
    int i;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_INTR),"%s",__FUNCTION__);

    intr_init = INIT_COMPLETED;

    for(i=0;i<INTR_TYPE_END;i++)
    {
        RTL8198F_INTR_REG[i] = DISABLED;
    }

    if((ret = dal_rtl8198f_intr_imr_set(INTR_TYPE_LINK_CHANGE,ENABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_INTR | MOD_DAL), "");
        intr_init = INIT_NOT_COMPLETED;
        return ret;
    }
    
    return RT_ERR_OK;
} /* end of dal_rtl8198f_intr_init */

/* Function Name:
 *      dal_rtl8198f_intr_imr_get
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
dal_rtl8198f_intr_imr_get(rtk_intr_type_t intr, rtk_enable_t *pEnable)
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
        default:
            RT_ERR(ret, (MOD_DAL|MOD_INTR), "");
            return RT_ERR_INPUT;

    }
    *pEnable = RTL8198F_INTR_REG[intr];

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_rtl8198f_intr_imr_set
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
dal_rtl8198f_intr_imr_set(rtk_intr_type_t intr, rtk_enable_t enable)
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
            if(RTL8198F_INTR_REG[intr] == DISABLED && enable == ENABLED)
            {
                if((ret = ca_event_register(0,CA_EVENT_ETH_PORT_LINK,ca_rtk_link_change_intr,NULL))!=CA_E_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_INTR), "");
                    return RT_ERR_FAILED;
                }
            }
            if(RTL8198F_INTR_REG[intr] == ENABLED && enable == DISABLED)
            {
                if((ret = ca_event_deregister(0,CA_EVENT_ETH_PORT_LINK,ca_rtk_link_change_intr))!=CA_E_OK)
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
    RTL8198F_INTR_REG[intr] = enable;

    return RT_ERR_OK;
} /* end of dal_rtl8198f_intr_imr_set */

