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
 *
 *
 *
 * $Revision$
 * $Date$
 *
 * Purpose : GPON API
 *
 * Feature : Provide the APIs to access GPON MAC
 *
 */

/*
 * Include Files
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netdevice.h>

#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <osal/print.h>
#include <dal/dal_mgmt.h>
#include <dal/apollo/dal_apollo.h>
#include <dal/apollomp/dal_apollomp.h>
#include <rtk/gpon.h>
#include <dal/apollo/gpon/gpon_defs.h>
#include <rtk/cpu_tag.h>
#include <dal/apollo/gpon/gpon_platform.h>
//#include <apollo_reg_definition.h>
#include <rtk/irq.h>
#if defined(CONFIG_RTL9600_SERIES)
#include "re8686.h"
#elif defined(CONFIG_RTL9601B_SERIES)
#include "re8686_rtl9601b.h"
#elif defined(CONFIG_RTL9602C_SERIES)
#include "re8686_rtl9602c.h"
#else
#include "re8686_nic.h"
#endif

#ifdef CONFIG_RTK_OMCI_V1
#include <linux/workqueue.h>
#include <linux/sched.h>
#include <pkt_redirect.h>
#endif

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
int32 rtk_gpon_omci_rx_wrapper(struct re_private *cp, struct sk_buff *skb, struct rx_info *pRxInfo);
 #ifdef CONFIG_GPON_FEATURE
extern int drv_nic_register_rxhook(int portmask,int priority,void (*rx)(struct re_private *cp, struct sk_buff *skb, struct rx_info *pRxInfo));
extern int drv_nic_unregister_rxhook(int portmask,int priority,void (*rx)(struct re_private *cp, struct sk_buff *skb, struct rx_info *pRxInfo));
#endif
/*
 * Macro Definition
 */


/*
 * Function Declaration
 */

#ifdef CONFIG_RTK_OMCI_V1

extern int pkt_redirect_kernelApp_reg(unsigned short uid,void (*appCallback)(unsigned short dataLen, unsigned char *data));
extern int pkt_redirect_kernelApp_dereg(unsigned short uid);
extern int pkt_redirect_kernelApp_sendPkt( unsigned short dstUid,int isUser,unsigned short dataLen,unsigned char *data);

typedef struct omci_work_s {
	rtk_gpon_omci_msg_t omci;
	struct work_struct work;
}omci_work_t;

omci_work_t *omci_work;

static void omci_send_to_user(rtk_gpon_omci_msg_t* omci)
{
	int ret;
	rtk_gpon_pkt_t  data;
	memset(&data,0,sizeof(rtk_gpon_pkt_t));
	data.type = RTK_GPON_MSG_OMCI;
	memcpy(&data.msg.omci,omci,sizeof(rtk_gpon_omci_msg_t));
	if((ret = pkt_redirect_kernelApp_sendPkt(PR_USER_UID_GPONOMCI,1, RTK_GPON_OMCI_MSG_LEN, (unsigned char *)&data))!=RT_ERR_OK)
	{
		RT_ERR(ret,(MOD_GPON | MOD_DAL), "");
	}
	return;
}

static void omci_to_nic_work(struct work_struct *ws)
{
	int ret ;
	omci_work_t *work = (omci_work_t*) container_of(ws,omci_work_t, work) ;

	if((ret = rtk_gpon_omci_tx(&work->omci))!=RT_ERR_OK)
	{
		RT_ERR(ret,(MOD_GPON | MOD_DAL), "");
		return;
	}
	/*osal_printf("%s(), done\n",__FUNCTION__);*/
	return ;
}

static void omci_send_to_nic(unsigned short len,unsigned char *omci)
{
	int ret ;
	omci_work = kzalloc(sizeof(typeof(*omci_work)),GFP_KERNEL);
	memcpy(&omci_work->omci.msg[0],&omci[0],RTK_GPON_OMCI_MSG_LEN);
	INIT_WORK(&omci_work->work,omci_to_nic_work);
	schedule_work(&omci_work->work);
	return ;
}


static int32 omcc_create(void)
{
	int ret;
	rtk_ponmac_queue_t queue;
	rtk_ponmac_queueCfg_t queueCfg;
	/*queue assign*/
	queue.schedulerId = GPON_OMCI_TCONT_ID;
	queue.queueId = GPON_OMCI_QUEUE_ID;
	/*queue configure assign*/
	osal_memset(&queueCfg,0,sizeof(rtk_ponmac_queueCfg_t));
	queueCfg.cir = 0;
	queueCfg.pir = 0x1ffff;
	queueCfg.type = STRICT_PRIORITY;
	/*add omcc tcont & queue*/
	if((ret = rtk_ponmac_queue_add(&queue,&queueCfg))!=RT_ERR_OK)
	{
		RT_ERR(ret,(MOD_GPON | MOD_DAL), "");
		return ret;
	}
	/*assign strem id to tcont & queue*/
	if((ret = rtk_ponmac_flow2Queue_set(GPON_OMCI_FLOW_ID, &queue))!=RT_ERR_OK)
	{
		RT_ERR(ret,(MOD_GPON | MOD_DAL), "");
		return ret;
	}
	return RT_ERR_OK;
}


static int32 omcc_del(void)
{
	int ret;
	rtk_ponmac_queue_t queue;

	/*queue assign*/
	queue.schedulerId = GPON_OMCI_TCONT_ID;
	queue.queueId = GPON_OMCI_QUEUE_ID;
	/*add omcc tcont & queue*/
	if((ret = rtk_ponmac_queue_del(&queue))!=RT_ERR_OK)
	{
		RT_ERR(ret,(MOD_GPON | MOD_DAL), "");
		return ret;
	}
	/*assign strem id to tcont & queue*/
	queue.queueId = 31;
	queue.schedulerId = 24;
	if((ret = rtk_ponmac_flow2Queue_set(GPON_OMCI_FLOW_ID, &queue))!=RT_ERR_OK)
	{
		RT_ERR(ret,(MOD_GPON | MOD_DAL), "");
		return ret;
	}
	return RT_ERR_OK;
}

static int32 multicast_flow_create(void)
{
	int ret;
    uint32 flowId = GPON_MULTICAST_FLOW_ID;
    rtk_gpon_dsFlow_attr_t attr;

	/*queue configure assign*/
	osal_memset(&attr,0,sizeof(rtk_gpon_dsFlow_attr_t));
	attr.gem_port_id = GPON_MULTICAST_GEM_PORT;
	attr.type = RTK_GPON_FLOW_TYPE_ETH;
	/*add omcc tcont & queue*/
	if((ret = rtk_gpon_dsFlow_set(flowId, &attr))!=RT_ERR_OK)
	{
		RT_ERR(ret,(MOD_GPON | MOD_DAL), "");
		return ret;
	}

	return RT_ERR_OK;
}

#endif

/* Module Name    : GPON     */

/* Function Name:
 *      rtk_gpon_evtHdlStateChange_reg
 * Description:
 *      This function is called to register the callback function of the State Change.
 * Input:
 *      func            - the callback function to be registered
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 */
int32 rtk_gpon_evtHdlStateChange_reg(rtk_gpon_eventHandleFunc_stateChange_t func)
{
    return RT_MAPPER->gpon_eventHandler_stateChange_reg(func);
}

/* Function Name:
 *      rtk_gpon_evtHdlDsFecChange_reg
 * Description:
 *      This function is called to register the callback function of the D/S FEC Change.
 * Input:
 *      func            - the callback function to be registered
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 */
int32 rtk_gpon_evtHdlDsFecChange_reg(rtk_gpon_eventHandleFunc_fecChange_t func)
{
    return RT_MAPPER->gpon_eventHandler_dsFecChange_reg(func);
}

/* Function Name:
 *      rtk_gpon_evtHdlUsFecChange_reg
 * Description:
 *      This function is called to register the callback function of the U/S FEC Change.
 * Input:
 *      func            - the callback function to be registered
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 */
int32 rtk_gpon_evtHdlUsFecChange_reg(rtk_gpon_eventHandleFunc_fecChange_t func)
{
    return RT_MAPPER->gpon_eventHandler_usFecChange_reg(func);
}

/* Function Name:
 *      rtk_gpon_evtHdlUsPloamUrgEmpty_reg
 * Description:
 *      This function is called to register the callback function of the U/S PLOAM urgent queue is empty.
 * Input:
 *      func            - the callback function to be registered
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 */
int32 rtk_gpon_evtHdlUsPloamUrgEmpty_reg(rtk_gpon_eventHandleFunc_usPloamEmpty_t func)
{
    return RT_MAPPER->gpon_eventHandler_usPloamUrgEmpty_reg(func);
}

/* Function Name:
 *      rtk_gpon_evtHdlUsPloamNrmEmpty_reg
 * Description:
 *      This function is called to register the callback function of the U/S PLOAM normal queue is empty.
 * Input:
 *      func            - the callback function to be registered
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 */
int32 rtk_gpon_evtHdlUsPloamNrmEmpty_reg(rtk_gpon_eventHandleFunc_usPloamEmpty_t func)
{
    return RT_MAPPER->gpon_eventHandler_usPloamNrmEmpty_reg(func);
}

/* Function Name:
 *      rtk_gpon_evtHdlPloam_reg
 * Description:
 *      This function is called to register the event handler of the Rx PLOAM.
 * Input:
 *      func            - the callback function to be registered
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 */
int32 rtk_gpon_evtHdlPloam_reg(rtk_gpon_eventHandleFunc_ploam_t func)
{
    return RT_MAPPER->gpon_eventHandler_ploam_reg(func);
}

/* Function Name:
 *      rtk_gpon_evtHdlOmci_reg
 * Description:
 *      This function is called to register the event handler of the Rx OMCI.
 * Input:
 *      func            - the callback function to be registered
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 */
int32 rtk_gpon_evtHdlOmci_reg(rtk_gpon_eventHandleFunc_omci_t func)
{
    return RT_MAPPER->gpon_eventHandler_omci_reg(func);
}

/* Function Name:
 *      rtk_gpon_callbackQueryAesKey_reg
 * Description:
 *      This function is called to register the callback function of the AES Key Query.
 * Input:
 *      func            - the callback function to be registered
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 */
int32 rtk_gpon_callbackQueryAesKey_reg(rtk_gpon_callbackFunc_queryAesKey_t func)
{
    return RT_MAPPER->gpon_callback_queryAesKey_reg(func);
}

/* Function Name:
 *      rtk_gpon_evtHdlAlarm_reg
 * Description:
 *      This function is called to register the alarm event handler of the alarm.
 * Input:
 *      alarmType       - the alarm type
 *      func            - the callback function to be registered
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 */
int32 rtk_gpon_evtHdlAlarm_reg(rtk_gpon_alarm_type_t alarmType, rtk_gpon_eventHandleFunc_fault_t func)
{
    return RT_MAPPER->gpon_eventHandler_alarm_reg(alarmType, func);
}

/* Function Name:
 *      rtk_gpon_serialNumber_set
 * Description:
 *      GPON MAC Set Serial Number.
 * Input:
 *      pSN             - the pointer of Serial Number
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *      It should be called before the GPON MAC is activated.
 */
int32 rtk_gpon_serialNumber_set(rtk_gpon_serialNumber_t *pSN)
{
    return RT_MAPPER->gpon_serialNumber_set(pSN);
}

/* Function Name:
 *      rtk_gpon_serialNumber_get
 * Description:
 *      GPON MAC get Serial Number.
 * Input:
 *      None
 * Output:
 *      pSN             - the pointer of Serial Number
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *      It should be called before the GPON MAC is activated.
 */
int32 rtk_gpon_serialNumber_get(rtk_gpon_serialNumber_t *pSN)
{
    return RT_MAPPER->gpon_serialNumber_get(pSN);
}

/* Function Name:
 *      rtk_gpon_password_set
 * Description:
 *      GPON MAC set Password.
 * Input:
 *      pPwd             - the pointer of Password
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *      It should be called before the GPON MAC is activated.
 */
int32 rtk_gpon_password_set(rtk_gpon_password_t *pPwd)
{
    return RT_MAPPER->gpon_password_set(pPwd);
}

/* Function Name:
 *      rtk_gpon_password_get
 * Description:
 *      GPON MAC get Password.
 * Input:
 *      None
 * Output:
 *      pPwd             - the pointer of Password
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *      It should be called before the GPON MAC is activated.
 */
int32 rtk_gpon_password_get(rtk_gpon_password_t *pPwd)
{
    return RT_MAPPER->gpon_password_get(pPwd);
}

/* Function Name:
 *      rtk_gpon_parameter_set
 * Description:
 *      GPON MAC set parameters.
 * Input:
 *      type            - the parameter type
 *      pPara           - the pointer of Parameter
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *      It should be called before the GPON MAC is activated.
 */
int32 rtk_gpon_parameter_set(rtk_gpon_patameter_type_t type, void *pPara)
{
    return RT_MAPPER->gpon_parameter_set(type, pPara);
}

/* Function Name:
 *      rtk_gpon_parameter_get
 * Description:
 *      GPON MAC get parameters, which is set by rtk_gpon_parameter_set.
 * Input:
 *      type            - the parameter type
 * Output:
 *      pPara           - the pointer of Parameter
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_parameter_get(rtk_gpon_patameter_type_t type, void *pPara)
{
    return RT_MAPPER->gpon_parameter_get(type, pPara);
}

/* Function Name:
 *      rtk_gpon_activate
 * Description:
 *      GPON MAC Activating.
 * Input:
 *      initState       - the initial state when ONU active
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *      The GPON MAC is working now.
 */
int32 rtk_gpon_activate(rtk_gpon_initialState_t initState)
{
	int32 ret;
	int32 ponPort =1<< HAL_GET_PON_PORT();
	/*disable imr for gpon interrupt before register isr*/

	if((ret= rtk_intr_imr_set(INTR_TYPE_GPON,DISABLED)) != RT_ERR_OK)
	{
		RT_ERR(ret,(MOD_GPON | MOD_DAL), "");
        return ret ;
	}

	/*register gpon isr*/
	if((ret = rtk_irq_isr_register(INTR_TYPE_GPON,rtk_gpon_isr_entry)) != RT_ERR_OK)
	{
		RT_ERR(ret,(MOD_GPON | MOD_DAL), "");
		return ret;
	}

	if((ret = RT_MAPPER->gpon_activate(initState)) != RT_ERR_OK)
	{
		RT_ERR(ret,(MOD_GPON | MOD_DAL), "");
		return ret;
	}

	/* clear switch interrupt state for GPON*/
    if((ret = rtk_intr_ims_clear(INTR_TYPE_GPON)) != RT_ERR_OK)
    {
    	RT_ERR(ret,(MOD_GPON | MOD_DAL), "");
		return ret;
    }

    /* switch interrupt mask for GPON */
    if((ret = rtk_intr_imr_set(INTR_TYPE_GPON,ENABLED)) != RT_ERR_OK)
    {
    	RT_ERR(ret,(MOD_GPON | MOD_DAL), "");
		return ret;
    }
#ifdef CONFIG_RTK_OMCI_V1
	/*register omci callback function*/
	if((ret = pkt_redirect_kernelApp_reg(PR_KERNEL_UID_GPONOMCI,omci_send_to_nic))!=RT_ERR_OK)
	{
		RT_ERR(ret,(MOD_GPON | MOD_DAL), "");
		return ret;
	}
	if((ret = rtk_gpon_evtHdlOmci_reg(omci_send_to_user))!=RT_ERR_OK)
	{
		RT_ERR(ret,(MOD_GPON | MOD_DAL), "");
		return ret;
	}
	if((ret = omcc_create())!=RT_ERR_OK)
	{
		RT_ERR(ret,(MOD_GPON | MOD_DAL), "");
		return ret;
	}
    if((ret = multicast_flow_create())!=RT_ERR_OK)
	{
		RT_ERR(ret,(MOD_GPON | MOD_DAL), "");
		return ret;
	}
#endif

#ifdef CONFIG_GPON_FEATURE
    /*register omci rx handler*/
    if((ret = drv_nic_register_rxhook(ponPort,RE8686_RXPRI_OMCI,rtk_gpon_omci_rx_wrapper))!=RT_ERR_OK){
		RT_ERR(ret,(MOD_GPON | MOD_DAL), "");
		return ret;
    }
#endif



	return RT_ERR_OK;
}

/* Function Name:
 *      rtk_gpon_deActivate
 * Description:
 *      GPON MAC de-Activate.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *      The GPON MAC is out of work now.
 */
int32 rtk_gpon_deActivate(void)
{
	int32 ret;
	int32 ponPort = 1<< HAL_GET_PON_PORT();
 #ifdef CONFIG_GPON_FEATURE
	/*register omci rx handler*/
    	if((ret = drv_nic_unregister_rxhook(ponPort,RE8686_RXPRI_OMCI,&rtk_gpon_omci_rx_wrapper))!=RT_ERR_OK){
		RT_ERR(ret,(MOD_GPON | MOD_DAL), "");
		return ret;
    	}
#endif

#ifdef CONFIG_RTK_OMCI_V1

	if((ret = pkt_redirect_kernelApp_dereg(PR_KERNEL_UID_GPONOMCI))!=RT_ERR_OK)
	{
		RT_ERR(ret,(MOD_GPON | MOD_DAL), "");
		return ret;
	}

	if((ret = omcc_del())!=RT_ERR_OK)
	{
		RT_ERR(ret,(MOD_GPON | MOD_DAL), "");
		return ret;
	}
#endif
	/*disable imr for gpon interrupt before register isr*/
	if((ret= rtk_intr_imr_set(INTR_TYPE_GPON,DISABLED)) != RT_ERR_OK)
	{
		RT_ERR(ret,(MOD_GPON | MOD_DAL), "");
        return ret ;
	}
	/*unregister gpon isr*/
	if((ret = rtk_irq_isr_unregister(INTR_TYPE_GPON)) != RT_ERR_OK)
	{
		RT_ERR(ret,(MOD_GPON | MOD_DAL), "");
		return ret;
	}
    /*unregister DyingGasp isr*/
	if((ret = rtk_irq_isr_unregister(INTR_TYPE_DYING_GASP)) != RT_ERR_OK)
	{
		RT_ERR(ret,(MOD_GPON | MOD_DAL), "");
		return ret;
	}

	/* switch interrupt mask for GPON */
    if((ret = rtk_intr_imr_set(INTR_TYPE_GPON,DISABLED)) != RT_ERR_OK)
    {
    	RT_ERR(ret,(MOD_GPON | MOD_DAL), "");
		return ret;
    }

    return RT_MAPPER->gpon_deActivate();
}

/* Function Name:
 *      rtk_gpon_ponStatus_get
 * Description:
 *      GPON MAC Get PON Status.
 * Input:
 *      None
 * Output:
 *      pStatus         - pointer of status
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *      If the device is not activated, an unknown status is returned.
 */
int32 rtk_gpon_ponStatus_get(rtk_gpon_fsm_status_t *pStatus)
{
    return RT_MAPPER->gpon_ponStatus_get(pStatus);
}

/* Function Name:
 *      rtk_gpon_isr_entry
 * Description:
 *      GPON MAC ISR entry
 * Input:
 *      data            - user data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *      It should be called in interrupt process or a polling thread
 */
void rtk_gpon_isr_entry(void)
{
    uint32 int_val;
    uint32 val;
	int32  ret;

    /* switch interrupt clear GPON mask */
    RT_MAPPER->gpon_isr_entry();

    /* switch interrupt clear GPON state */
    if((ret=rtk_intr_ims_clear(INTR_TYPE_GPON)) != RT_ERR_OK)
	{
		RT_ERR(ret, (MOD_GPON | MOD_DAL), "");
        return ;
	}

    GPON_OS_Log(GPON_LOG_LEVEL_NORMAL,"GPON ISR finished!", ret);

	return;
}

/* Function Name:
 *      rtk_gpon_tcont_create
 * Description:
 *      GPON MAC Create a TCont by assigning an alloc id.
 * Input:
 *      pInd            - the pointer of ALLOC_id
 * Output:
 *      aAttr           - the pointer of tcont attribute(TCont id)
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *      A TCont ID is returned in pAttr.
 */
int32 rtk_gpon_tcont_create(rtk_gpon_tcont_ind_t *pInd, rtk_gpon_tcont_attr_t *pAttr)
{
    return RT_MAPPER->gpon_tcont_create(pInd, pAttr);
}

/* Function Name:
 *      rtk_gpon_tcont_destroy
 * Description:
 *      GPON MAC Remove a TCont.
 * Input:
 *      pInd            - the pointer of ALLOC_id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_tcont_destroy(rtk_gpon_tcont_ind_t *pInd)
{
    return RT_MAPPER->gpon_tcont_destroy(pInd);
}

/* Function Name:
 *      rtk_gpon_tcont_get
 * Description:
 *      GPON MAC Get a TCont with an alloc id.
 * Input:
 *      pInd            - the pointer of ALLOC_id
 * Output:
 *      aAttr           - the pointer of tcont attribute(TCont id)
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *      The TCont ID is returned in pAttr.
 */
int32 rtk_gpon_tcont_get(rtk_gpon_tcont_ind_t *pInd, rtk_gpon_tcont_attr_t *pAttr)
{
    return RT_MAPPER->gpon_tcont_get(pInd, pAttr);
}

/* Function Name:
 *      rtk_gpon_dsFlow_set
 * Description:
 *      GPON MAC set a D/S flow.
 * Input:
 *      flowId          - the flow id
 *      aAttr           - the pointer of flow attribute(Gem port id,...)
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_dsFlow_set(uint32 flowId, rtk_gpon_dsFlow_attr_t *pAttr)
{
    return RT_MAPPER->gpon_dsFlow_set(flowId, pAttr);
}

/* Function Name:
 *      rtk_gpon_dsFlow_get
 * Description:
 *      GPON MAC get a D/S flow.
 * Input:
 *      flowId          - the flow id
 * Output:
 *      aAttr           - the pointer of flow attribute(Gem port id,...)
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_dsFlow_get(uint32 flowId, rtk_gpon_dsFlow_attr_t *pAttr)
{
    return RT_MAPPER->gpon_dsFlow_get(flowId, pAttr);
}

/* Function Name:
 *      rtk_gpon_usFlow_set
 * Description:
 *      GPON MAC set a U/S flow.
 * Input:
 *      flowId          - the flow id
 *      aAttr           - the pointer of flow attribute(Gem port id,...)
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_usFlow_set(uint32 flowId, rtk_gpon_usFlow_attr_t *pAttr)
{
    return RT_MAPPER->gpon_usFlow_set(flowId, pAttr);
}

/* Function Name:
 *      rtk_gpon_usFlow_get
 * Description:
 *      GPON MAC get a U/S flow.
 * Input:
 *      flowId          - the flow id
 *      aAttr           - the pointer of flow attribute(Gem port id,...)
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_usFlow_get(uint32 flowId, rtk_gpon_usFlow_attr_t *pAttr)
{
    return RT_MAPPER->gpon_usFlow_get(flowId, pAttr);
}

/* Function Name:
 *      rtk_gpon_ploam_send
 * Description:
 *      GPON MAC Send a PLOAM in upstream.
 * Input:
 *      urgent          - specify it is a urgent(1) or normal(0) PLOAM message
 *      pPloam          - the pointer of PLOAM message
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *      A error is returned if the PLOAM is not sent.
 */
int32 rtk_gpon_ploam_send(int32 urgent, rtk_gpon_ploam_t *pPloam)
{
    return RT_MAPPER->gpon_ploam_send(urgent, pPloam);
}

/* Function Name:
 *      rtk_gpon_broadcastPass_set
 * Description:
 *      GPON MAC set the broadcast pass mode.
 * Input:
 *      mode            - turn on(1) or off(0) the broadcast pass mode.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_broadcastPass_set(int32 mode)
{
    return RT_MAPPER->gpon_broadcastPass_set(mode);
}

/* Function Name:
 *      rtk_gpon_broadcastPass_get
 * Description:
 *      GPON MAC get the broadcast pass mode.
 * Input:
 *      pMode           - the pointer of broadcast pass mode: turn on(1) or off(0).
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_broadcastPass_get(int32 *pMode)
{
    return RT_MAPPER->gpon_broadcastPass_get(pMode);
}

/* Function Name:
 *      rtk_gpon_nonMcastPass_set
 * Description:
 *      GPON MAC set the non-multidcast pass mode.
 * Input:
 *      mode            - turn on(1) or off(0) the non-multidcast pass mode.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_nonMcastPass_set(int32 mode)
{
    return RT_MAPPER->gpon_nonMcastPass_set(mode);
}

/* Function Name:
 *      rtk_gpon_nonMcastPass_get
 * Description:
 *      GPON MAC get the non-multidcast pass mode.
 * Input:
 *      pMode           - the pointer of non-multidcast pass mode: turn on(1) or off(0).
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_nonMcastPass_get(int32 *pMode)
{
    return RT_MAPPER->gpon_nonMcastPass_get(pMode);
}

/* Function Name:
 *      rtk_gpon_multicastAddrCheck_set
 * Description:
 *      GPON MAC set the address pattern.
 * Input:
 *      ipv4_pattern    - Address pattern of DA[47:24] for IPv4 packets.
 *      ipv6_pattern    - Address pattern of DA[47:32] for IPv6 packets.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_multicastAddrCheck_set(uint32 ipv4_pattern, uint32 ipv6_pattern)
{
    return RT_MAPPER->gpon_multicastAddrCheck_set(ipv4_pattern, ipv6_pattern);
}

/* Function Name:
 *      rtk_gpon_multicastAddrCheck_get
 * Description:
 *      GPON MAC get the address pattern.
 * Input:
 *      pIpv4_pattern    - Address pattern of DA[47:24] for IPv4 packets..
 *      pIpv6_pattern    - Address pattern of DA[47:24] for IPv6 packets..
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_multicastAddrCheck_get(uint32 *pIpv4_Pattern, uint32 *pIpv6_Pattern)
{
    return RT_MAPPER->gpon_multicastAddrCheck_get(pIpv4_Pattern, pIpv6_Pattern);
}

/* Function Name:
 *      rtk_gpon_macFilterMode_set
 * Description:
 *      GPON MAC set the mac filter mode.
 * Input:
 *      mode            - MAC table filter mode.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_macFilterMode_set(rtk_gpon_macTable_exclude_mode_t mode)
{
    return RT_MAPPER->gpon_macFilterMode_set(mode);
}

/* Function Name:
 *      rtk_gpon_macFilterMode_get
 * Description:
 *      GPON MAC get the mac filter mode.
 * Input:
 *      pMode           - pointer of MAC filter table filter mode.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_macFilterMode_get(rtk_gpon_macTable_exclude_mode_t *pMode)
{
    return RT_MAPPER->gpon_macFilterMode_get(pMode);
}

/* Function Name:
 *      rtk_gpon_mcForceMode_set
 * Description:
 *      GPON MAC set the multicast force mode.
 * Input:
 *      ipv4            - IPv4 multicast force mode.
 *      ipv6            - IPv6 multicast force mode.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_mcForceMode_set(rtk_gpon_mc_force_mode_t ipv4, rtk_gpon_mc_force_mode_t ipv6)
{
    return RT_MAPPER->gpon_mcForceMode_set(ipv4,ipv6);
}

/* Function Name:
 *      rtk_gpon_mcForceMode_get
 * Description:
 *      GPON MAC get the multicast force mode.
 * Input:
 *      pIpv4           - The pointer of IPv4 multicast force mode.
 *      pIv6            - The pointer of IPv6 multicast force mode.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_mcForceMode_get(rtk_gpon_mc_force_mode_t *pIpv4, rtk_gpon_mc_force_mode_t *pIpv6)
{
    return RT_MAPPER->gpon_mcForceMode_get(pIpv4,pIpv6);
}

/* Function Name:
 *      rtk_gpon_macEntry_add
 * Description:
 *      GPON MAC Add a MAC entry by the MAC Address.
 * Input:
 *      pEntry          - pointer of MAC filter table entry.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_macEntry_add(rtk_gpon_macTable_entry_t *pEntry)
{
    return RT_MAPPER->gpon_macEntry_add(pEntry);
}

/* Function Name:
 *      rtk_gpon_macEntry_del
 * Description:
 *      GPON MAC Remove a MAC entry by the MAC Address.
 * Input:
 *      pEntry          - pointer of MAC filter table entry.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_macEntry_del(rtk_gpon_macTable_entry_t *pEntry)
{
    return RT_MAPPER->gpon_macEntry_del(pEntry);
}

/* Function Name:
 *      rtk_gpon_macEntry_get
 * Description:
 *      GPON MAC Get a MAC entry by the table index.
 * Input:
 *      index           - index of MAC filter table entry.
 *      pEntry          - pointer of MAC filter table entry.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_macEntry_get(uint32 index, rtk_gpon_macTable_entry_t *pEntry)
{
    return RT_MAPPER->gpon_macEntry_get(index, pEntry);
}

/* Function Name:
 *      rtk_gpon_rdi_set
 * Description:
 *      GPON MAC set the RDI indicator in upstream.
 * Input:
 *      enable          - specify to turn on/off RDI.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_rdi_set(int32 enable)
{
    return RT_MAPPER->gpon_rdi_set(enable);
}

/* Function Name:
 *      rtk_gpon_rdi_get
 * Description:
 *      GPON MAC get the RDI indicator in upstream.
 * Input:
 *      pEnable         - the pointer of RDI indicator.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_rdi_get(int32 *pEnable)
{
    return RT_MAPPER->gpon_rdi_get(pEnable);
}

/* Function Name:
 *      rtk_gpon_powerLevel_set
 * Description:
 *      GPON MAC set ONU power level, it will update the TT field of
 *      Serial_Number_ONU PLOAMu message.
 * Input:
 *      level           - the power lever.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_powerLevel_set(uint8 level)
{
    return RT_MAPPER->gpon_powerLevel_set(level);
}

/* Function Name:
 *      rtk_gpon_powerLevel_get
 * Description:
 *      GPON MAC get ONU power level.
 * Input:
 *      pLevel          - the pointer of power lever.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_powerLevel_get(uint8 *pLevel)
{
    return RT_MAPPER->gpon_powerLevel_get(pLevel);
}

/* Function Name:
 *      rtk_gpon_alarmStatus_get
 * Description:
 *      GPON MAC get the alarm status.
 * Input:
 *      alarm           - the alarm type.
 *      pStatus         - the pointer of alarm status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_alarmStatus_get(rtk_gpon_alarm_type_t alarm, int32 *pStatus)
{
    return RT_MAPPER->gpon_alarmStatus_get(alarm, pStatus);
}

/* Function Name:
 *      rtk_gpon_globalCounter_get
 * Description:
 *      GPON MAC get global performance counter.
 * Input:
 *      type            - the PM type.
 *      pPara           - the pointer of counter data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_globalCounter_get (rtk_gpon_global_performance_type_t type, rtk_gpon_global_counter_t *pPara)
{
    return RT_MAPPER->gpon_globalCounter_get(type, pPara);
}

/* Function Name:
 *      rtk_gpon_tcontCounter_get
 * Description:
 *      GPON MAC get Tcont performance counter.
 * Input:
 *      tcontId         - the TCont id
 *      type            - the PM type.
 *      pPara           - the pointer of counter data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_tcontCounter_get (uint32 tcontId, rtk_gpon_tcont_performance_type_t type, rtk_gpon_tcont_counter_t *pPara)
{
    return RT_MAPPER->gpon_tcontCounter_get(tcontId, type, pPara);
}

/* Function Name:
 *      rtk_gpon_flowCounter_get
 * Description:
 *      GPON MAC get Flow performance counter.
 * Input:
 *      flowId          - the flow id
 *      type            - the PM type.
 *      pPara           - the pointer of counter data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_flowCounter_get (uint32 flowId, rtk_gpon_flow_performance_type_t type, rtk_gpon_flow_counter_t *pPara)
{
    return RT_MAPPER->gpon_flowCounter_get(flowId, type, pPara);
}

/* Function Name:
 *      rtk_gpon_version_get
 * Description:
 *      GPON MAC get the version infomation for debug.
 * Input:
 *      pHver           - the pointer of Hardware versiotn
 *      pSver           - the pointer of Software versiotn
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_version_get(rtk_gpon_device_ver_t *pHver, rtk_gpon_driver_ver_t *pSver)
{
    return RT_MAPPER->gpon_version_get(pHver, pSver);
}

/* Function Name:
 *      rtk_gpon_txForceLaser_set
 * Description:
 *      GPON MAC set the Laser status.
 * Input:
 *      status          - specify to force turn on/off laser
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_txForceLaser_set(rtk_gpon_laser_status_t status)
{
    return RT_MAPPER->gpon_txForceLaser_set(status);
}

/* Function Name:
 *      rtk_gpon_txForceLaser_get
 * Description:
 *      GPON MAC get the Laser status.
 * Input:
 *      pStatus         - pointer of force laser status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_txForceLaser_get(rtk_gpon_laser_status_t *pStatus)
{
    return RT_MAPPER->gpon_txForceLaser_get(pStatus);
}

/* Function Name:
 *      rtk_gpon_txForceIdle_set
 * Description:
 *      GPON MAC set to force insert the idle in upstream.
 * Input:
 *      pStatus         - specify to force send Idle
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_txForceIdle_set(int32 on)
{
    return RT_MAPPER->gpon_txForceIdle_set(on);
}

/* Function Name:
 *      rtk_gpon_txForceIdle_get
 * Description:
 *      GPON MAC get the status to force insert the idle in upstream.
 * Input:
 *      pStatus         - pointer of force Idle
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_txForceIdle_get(int32 *pOn)
{
    return RT_MAPPER->gpon_txForceIdle_get(pOn);
}

#if 0
/* Function Name:
 *      rtk_gpon_txForcePRBS_get
 * Description:
 *      GPON MAC set to force insert the PRBS in upstream.
 * Input:
 *      pStatus         - specify to force send PRBS
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_txForcePRBS_set(int32 on)
{
    return RT_MAPPER->gpon_txForcePRBS_set(on);
}

/* Function Name:
 *      rtk_gpon_txForcePRBS_get
 * Description:
 *      GPON MAC get the status to force insert the PRBS in upstream.
 * Input:
 *      pStatus         - pointer of force PRBS
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_txForcePRBS_get(int32* pOn)
{
    return RT_MAPPER->gpon_txForcePRBS_get(pOn);
}
#endif

/* Function Name:
 *      rtk_gpon_dsFecSts_get
 * Description:
 *      GPON MAC get the status to FEC in downstream from Ident field.
 * Input:
 *      pStatus         - pointer of D/S FEC status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_dsFecSts_get(int32* pEn)
{
    return RT_MAPPER->gpon_dsFecSts_get(pEn);
}

/* Function Name:
 *      rtk_gpon_version_show
 * Description:
 *      GPON MAC show version infomation in COM port.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *
 */
int32 rtk_gpon_version_show(void)
{
    RT_MAPPER->gpon_version_show();
}

/* Function Name:
 *      rtk_gpon_devInfo_show
 * Description:
 *      GPON MAC show the whole driver infomation in COM port.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *
 */
int32 rtk_gpon_devInfo_show(void)
{
    RT_MAPPER->gpon_devInfo_show();
}

/* Function Name:
 *      rtk_gpon_gtc_show
 * Description:
 *      GPON MAC show the whole GTC infomation in COM port.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *
 */
int32 rtk_gpon_gtc_show(void)
{
    RT_MAPPER->gpon_gtc_show();
}

/* Function Name:
 *      rtk_gpon_tcont_show
 * Description:
 *      GPON MAC show the TCont infomation in COM port.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *
 */
int32 rtk_gpon_tcont_show(uint32 tcont)
{
    RT_MAPPER->gpon_tcont_show(tcont);
}

/* Function Name:
 *      rtk_gpon_dsFlow_show
 * Description:
 *      GPON MAC show the D/S flow infomation in COM port.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *
 */
int32 rtk_gpon_dsFlow_show(uint32 flow)
{
    RT_MAPPER->gpon_dsFlow_show(flow);
}

/* Function Name:
 *      rtk_gpon_usFlow_show
 * Description:
 *      GPON MAC show the U/S flow infomation in COM port.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *
 */
int32 rtk_gpon_usFlow_show(uint32 flow)
{
    RT_MAPPER->gpon_usFlow_show(flow);
}

/* Function Name:
 *      rtk_gpon_macTable_show
 * Description:
 *      GPON MAC show Ethernet Mac Table in COM port.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *
 */
int32 rtk_gpon_macTable_show(void)
{
    RT_MAPPER->gpon_macTable_show();
}

/* Function Name:
 *      rtk_gpon_globalCounter_show
 * Description:
 *      GPON MAC show Global Counter in COM port.
 * Input:
 *      type    - counter type
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *
 */
int32 rtk_gpon_globalCounter_show(rtk_gpon_global_performance_type_t type)
{
    RT_MAPPER->gpon_globalCounter_show(type);
}

/* Function Name:
 *      rtk_gpon_tcontCounter_show
 * Description:
 *      GPON MAC show TCont Counter in COM port.
 * Input:
 *      idx         - TCont index
 *      type        - counter type
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *
 */
int32 rtk_gpon_tcontCounter_show(uint32 idx, rtk_gpon_tcont_performance_type_t type)
{
    RT_MAPPER->gpon_tcontCounter_show(idx, type);
}

/* Function Name:
 *      rtk_gpon_flowCounter_show
 * Description:
 *      GPON MAC show Flow Counter in COM port.
 * Input:
 *      idx         - Flow index
 *      type        - counter type
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *
 */
int32 rtk_gpon_flowCounter_show(uint32 idx, rtk_gpon_flow_performance_type_t type)
{
    RT_MAPPER->gpon_flowCounter_show(idx, type);
}

#if defined(OLD_FPGA_DEFINED)
int32 rtk_gpon_pktGen_cfg_set(uint32 item, uint32 tcont, uint32 len, uint32 gem, int32 omci)
{
    return RT_MAPPER->gpon_pktGen_cfg_set(item, tcont, len, gem, omci);
}

int32 rtk_gpon_pktGen_buf_set(uint32 item, uint8 *buf, uint32 len)
{
    return RT_MAPPER->gpon_pktGen_buf_set(item, buf, len);
}
#endif

/* Function Name:
 *      rtk_gpon_omci_tx
 * Description:
 *      Transmit OMCI message.
 * Input:
 *      pOmci       - pointer of OMCI message data
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *
 */
int32 rtk_gpon_omci_tx(rtk_gpon_omci_msg_t *pOmci)
{
    return RT_MAPPER->gpon_omci_tx(pOmci);
}


/* Function Name:
 *      rtk_gpon_omci_rx
 * Description:
 *      Receive OMCI message.
 * Input:
 *      pBuf        - pointer of received data
 *      len         - received data length
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *
 */
int32 rtk_gpon_omci_rx_wrapper(struct re_private *cp, struct sk_buff *skb, struct rx_info *pRxInfo)
{
	uint32 ret;
#if 0
    if(pRxInfo->opts2.bit.pon_stream_id== GPON_OMCI_FLOW_ID)
#else
    if(pRxInfo->opts3.bit.reason == 209)
#endif
    {
     	if((ret = rtk_gpon_omci_rx(skb->data,skb->len))!=RT_ERR_OK){
    		RT_ERR(ret,(MOD_GPON | MOD_DAL), "");
    		return 0;
    	}
	return 0;
    }
	return 1;
}

/* Function Name:
 *      rtk_gpon_omci_rx
 * Description:
 *      Receive OMCI message.
 * Input:
 *      pBuf        - pointer of received data
 *      len         - received data length
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *
 */
int32 rtk_gpon_omci_rx(uint8 *pBuf, uint32 len)
{
    rtk_gpon_omci_msg_t omci;

    if(len > RTK_GPON_OMCI_MSG_LEN)
        osal_memcpy(omci.msg, pBuf, RTK_GPON_OMCI_MSG_LEN);
    else
        osal_memcpy(omci.msg, pBuf, len);

    return RT_MAPPER->gpon_omci_rx(&omci);
}

/* Function Name:
 *      rtk_gpon_autoTcont_set
 * Description:
 *      GPON set the TCONT auto configuration.
 * Input:
 *      state          - state value.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_autoTcont_set(int32 state)
{
    return RT_MAPPER->gpon_autoTcont_set(state);
}

/* Function Name:
 *      rtk_gpon_autoTcont_get
 * Description:
 *      GPON get the TCONT auto configuration.
 * Input:
 *      pState         - the pointer of state.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_autoTcont_get(int32 *pState)
{
    return RT_MAPPER->gpon_autoTcont_get(pState);
}

/* Function Name:
 *      rtk_gpon_autoBoh_set
 * Description:
 *      GPON set the BOH auto configuration.
 * Input:
 *      offset          - state value.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_autoBoh_set(int32 state)
{
    return RT_MAPPER->gpon_autoBoh_set(state);
}

/* Function Name:
 *      rtk_gpon_autoBoh_get
 * Description:
 *      GPON get the BOH auto configuration.
 * Input:
 *      pOffset         - the pointer of state.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_autoBoh_get(int32 *pState)
{
    return RT_MAPPER->gpon_autoBoh_get(pState);
}

/* Function Name:
 *      rtk_gpon_eqdOffset_set
 * Description:
 *      GPON set the EQD offset.
 * Input:
 *      offset          - offset value.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_eqdOffset_set(int32 offset)
{
    return RT_MAPPER->gpon_eqdOffset_set(offset);
}

/* Function Name:
 *      rtk_gpon_eqdOffset_get
 * Description:
 *      GPON get the EQD offset.
 * Input:
 *      pOffset         - the pointer of offset.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_eqdOffset_get(int32 *pOffset)
{
    return RT_MAPPER->gpon_eqdOffset_get(pOffset);
}

/* Function Name:
 *      rtk_gpon_autoDisTx_set
 * Description:
 *      Enable or disable GPON auto TX_DIS function.
 * Input:
 *      state           - enable or disable TX_DIS.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK       - successful
 *      others          - fail
 * Note:
 *
 */
int32 rtk_gpon_autoDisTx_set(rtk_enable_t state)
{
    return RT_MAPPER->gpon_autoDisTx_set(state);
}


