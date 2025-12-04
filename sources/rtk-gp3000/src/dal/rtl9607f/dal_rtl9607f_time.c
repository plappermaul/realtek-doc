/*
 * Copyright (C) 2023 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision:  $
 * $Date:  $
 *
 * Purpose : Definition of TIME API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) IEEE 1588
 *           (2) PON ToD
 *           (3) 1PPS/ToD
 *
 */


/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/rt/rt_time.h>
#include <dal/rtl9607f/dal_rtl9607f.h>
#include <dal/rtl9607f/dal_rtl9607f_time.h>
#include <osal/time.h>

#include "aal_ptp.h"
#include "aal_gpon.h"
#include "aal_pon.h"
#include "aal_l2_specpkt.h"
#include "aal_port.h"

#include "ni-drv-07f/ca_ni.h"
#include "ca_ext.h"

/*
 * Symbol Definition
 */

static rt_enable_t gPTPTrapEnable = DISABLED; //Default disable trap PTP packet
static rt_time_ptp_rx_callback gPTPRxCallback = NULL;

/*
 * Data Declaration
 */

#define PTP_MAX_PORT_NUM 8

static uint32    time_init = {INIT_NOT_COMPLETED};

rt_enable_t glb_ptpPort_enable[PTP_MAX_PORT_NUM];
rt_time_ptpIgr_action_t glb_ptpIgr_action[RT_PTP_IGR_ACTION_END];
rt_time_ptpEgr_action_t glb_ptpEgr_action = RT_PTP_EGR_ACTION_NONE;

/*
 * Macro Declaration
 */

extern uint32 ca_rtk_debug;

#ifdef CONFIG_REALTEK_BOARD_FPGA
#include <aal_init.h>
#define RTL9607F_LOG_ERR(fmt, arg...)  printk("[rtl9607f]:\t" fmt "\n",## arg)
#define RTL9607F_LOG_DBG(fmt, arg...)  printk("[rtl9607f]:\t" fmt "\n",## arg)
#else
#define RTL9607F_LOG_ERR(fmt, arg...)  printk(KERN_ERR "[rtl9607f]:\t" fmt "\n",## arg)
#define RTL9607F_LOG_DBG(fmt, arg...)  if(ca_rtk_debug&0x00000400) printk(KERN_INFO "[rtl9607f]:\t" fmt "\n",## arg)
#endif
/*
 * Function Declaration
 */

int32
dal_rtl9607f_time_igrAction_check(void)
{
    int32   ret = RT_ERR_OK;
    uint32 port,type;
    aal_ilpb_cfg_t ilpb_cfg;
    aal_l2_specpkt_ctrl_mask_t l2_spec_mask;
    aal_l2_specpkt_ctrl_t l2_spec_ctrl;
    aal_l2_specpkt_type_t  pkt_type;

    for(port=0;port<PTP_MAX_PORT_NUM;port++)
    {
        memset(&l2_spec_ctrl,0,sizeof(aal_l2_specpkt_ctrl_t));

        l2_spec_mask.u32 = 0;
        l2_spec_mask.s.learn_dis = 1;
        l2_spec_mask.s.ldpid = 1;
        l2_spec_mask.s.keep_ts = 1;
        l2_spec_mask.s.spcl_fwd = 1;

        for(type=RT_PTP_IGR_TYPE_L2;type<RT_PTP_IGR_TYPE_END;type++)
        {
            if(glb_ptpIgr_action[type] == RT_PTP_IGR_ACTION_TRAP2CPU && glb_ptpPort_enable[port] == ENABLED)
            {
                l2_spec_ctrl.learn_dis = 1;
                l2_spec_ctrl.ldpid = 0x10;
                l2_spec_ctrl.keep_ts = 1;
                l2_spec_ctrl.spcl_fwd = 1;
            }
            else
            {
                l2_spec_ctrl.learn_dis = 0;
                l2_spec_ctrl.ldpid = 0;
                l2_spec_ctrl.keep_ts = 0;
                l2_spec_ctrl.spcl_fwd = 0;
            }

            if(type == RT_PTP_IGR_TYPE_L2)
                pkt_type = AAL_L2_SPECPKT_TYPE_L2_PTP;
            else
                pkt_type = AAL_L2_SPECPKT_TYPE_PTP_UDP;

            ret = aal_port_ilpb_cfg_get(0,port,&ilpb_cfg);
            if(ret != CA_E_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return RT_ERR_FAILED;
            }

            ret = aal_l2_specpkt_ctrl_set(0,ilpb_cfg.spcl_pkt_idx,pkt_type,l2_spec_mask,&l2_spec_ctrl);
            if(ret != CA_E_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return RT_ERR_FAILED;
            }
        }

    }

    return ret;
}

int nic_register_rxhook(int portmask,int priority,p2rfunc_t rx);
int nic_unregister_rxhook(int portmask,int priority,p2rfunc_t rx);

int ptp_pkt_rx(struct napi_struct *napi,struct net_device *dev, struct sk_buff *skb, nic_hook_private_t *nh_priv)
{
    int32 ret = CA_E_OK;
    ca_uint32_t latnsec;
    ca_uint64_t sec;
    ca_uint32_t nsec;
    rt_time_timeStamp_t timeStamp;

    if(nh_priv->hdr_a->bits.hdr_type == 3)
    {
        if(nh_priv->hdr_a->bits.lspid >7)
            return RE8670_RX_STOP;

        ret = aal_ptp_rx_timer_get(0,nh_priv->hdr_a->bits.lspid,&latnsec,&sec,&nsec);
        if(ret != CA_E_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return RE8670_RX_STOP;
        }

        if(nh_priv->hdr_a->bits.bits_32_63.ptp_info.ptp_info != latnsec)
        {
            RTL9607F_LOG_DBG("rx PTP lspid=%d ptp_info = %u latnsec=%d not the same\n",nh_priv->hdr_a->bits.lspid,nh_priv->hdr_a->bits.bits_32_63.ptp_info.ptp_info,latnsec);
            return RE8670_RX_STOP;
        }

        RTL9607F_LOG_DBG("rx PTP lspid=%d ptp_info = %u\n",nh_priv->hdr_a->bits.lspid,nh_priv->hdr_a->bits.bits_32_63.ptp_info.ptp_info);

        if(gPTPRxCallback)
        {
            if(nsec < nh_priv->hdr_a->bits.bits_32_63.ptp_info.ptp_info)
                timeStamp.sec = sec + 1;
            else
                timeStamp.sec = sec;
            timeStamp.nsec = nh_priv->hdr_a->bits.bits_32_63.ptp_info.ptp_info;
            gPTPRxCallback(nh_priv->hdr_a->bits.lspid, skb->len, skb->data, timeStamp);
        }

        return RE8670_RX_STOP;
    }

    return RE8670_RX_CONTINUE;
}

/* Function Name:
 *      dal_rtl9607f_time_init
 * Description:
 *      Initialize Time module.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize Time module before calling any Time APIs.
 */
int32
dal_rtl9607f_time_init(void)
{
    int32   ret = RT_ERR_OK;
    uint32  port;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PTP),"%s",__FUNCTION__);

    for(port=0;port<PTP_MAX_PORT_NUM;port++)
    {
        glb_ptpPort_enable[port] = DISABLED;
    }

    glb_ptpIgr_action[RT_PTP_IGR_TYPE_L2] = RT_PTP_IGR_ACTION_NONE;
    glb_ptpIgr_action[RT_PTP_IGR_TYPE_UDP] = RT_PTP_IGR_ACTION_NONE;

    ret = aal_ptp_timer_enable_set(0,0);
    if(ret != CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return RT_ERR_FAILED;
    }

    ret = aal_ptp_init(0);
    if(ret != CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return RT_ERR_FAILED;
    }

    time_init = INIT_COMPLETED;

    return RT_ERR_OK;
} /* end of dal_rtl9607f_time_init */

/* Function Name:
 *      dal_rtl9607f_rt_time_curTime_get
 * Description:
 *      Get the current time.
 * Input:
 *      None
 * Output:
 *      pTimeStamp - pointer buffer of the current time
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
dal_rtl9607f_rt_time_curTime_get(rt_time_timeStamp_t *pTimeStamp)
{
    int32   ret = RT_ERR_OK;
    ca_uint64_t sec;
    ca_uint32_t nsec;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PTP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(time_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pTimeStamp), RT_ERR_NULL_POINTER);

    ret = aal_ptp_timer_get(0,&sec,&nsec);
    if(ret != CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return RT_ERR_FAILED;
    }

    pTimeStamp->sec = sec;
    pTimeStamp->nsec = nsec;

    return ret;
} /* end of dal_rtl9607f_rt_time_curTime_get */

/* Function Name:
 *      dal_rtl9607f_rt_time_offsetTime_set
 * Description:
 *      Set the reference time.
 * Input:
 *      sign      - significant
 *      timeStamp - reference timestamp value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      sign=0 for positive adjustment, sign=1 for negative adjustment.
 */
int32
dal_rtl9607f_rt_time_offsetTime_set(rt_time_ptpSign_t sign, rt_time_timeStamp_t timeStamp)
{
    int32   ret = RT_ERR_OK;
    ca_uint64_t ts;
    ca_uint32_t nts,i,cnt = 0;
    aal_ptp_match_load_t load;
    aal_ptp_match_load_mask_t load_mask;


    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PTP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(time_init);

    /* parameter check */
    RT_PARAM_CHK((RT_PTP_SIGN_END <= sign), RT_ERR_INPUT);
    RT_PARAM_CHK((RTL9607F_TIME_SEC_MAX < timeStamp.sec), RT_ERR_INPUT);
    RT_PARAM_CHK((RTL9607F_TIME_NSEC_MAX <= timeStamp.nsec), RT_ERR_INPUT);

#if 1
    while(1)
    {
        ret = aal_ptp_timer_get(0,&ts,&nts);
        if(ret != CA_E_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return RT_ERR_FAILED;
        }

        if(nts > 700000000)
        {
            cnt++;
            ca_mdelay(200);
            continue;
        }

        break;
    }

    if(sign == RT_PTP_SIGN_POSTIVE)
    {
        ret = aal_ptp_load_timer_set(0,ts + 1 + timeStamp.sec,timeStamp.nsec);
        if(ret != CA_E_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return RT_ERR_FAILED;
        }
    }
    else
    {
        if((ts + 1) < timeStamp.sec)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return RT_ERR_INPUT;
        }

        if(timeStamp.nsec == 0)
        {
            ret = aal_ptp_load_timer_set(0,ts + 1 - timeStamp.sec,0);
            if(ret != CA_E_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return RT_ERR_FAILED;
            }
        }
        else
        {
            ret = aal_ptp_load_timer_set(0,ts - timeStamp.sec,1000000000 - timeStamp.nsec);
            if(ret != CA_E_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return RT_ERR_FAILED;
            }
        }
    }

    load_mask.u32 = 0;
    load_mask.s.ptp_load = 1;
    load_mask.s.ptp_match_time = 1;

    memset(&load,0,sizeof(aal_ptp_match_load_t));
    load.ptp_load = 1;
    load.ptp_match_time = 0;
    ret = aal_ptp_match_load_set(0,load_mask,load);
    if(ret != CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return RT_ERR_FAILED;
    }

    for(i=0;i<10;i++)
    {
        ca_mdelay(100);
        ret = aal_ptp_match_load_get(0,&load);
        if(ret != CA_E_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return RT_ERR_FAILED;
        }

        if(load.ptp_load == 0)
        {
            if(ca_rtk_debug&0x00000400)
            {
                ret = aal_ptp_timer_get(0,&ts,&nts);
                if(ret != CA_E_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                }
                else
                {
                    printk("Match sec %llu  cnt %d\n",ts+1,cnt);
                    printk("Cur ts %llu nts %u\n",ts,nts);
                }
            }
            return RT_ERR_OK;
        }
    }

    RTL9607F_LOG_ERR("Checking ptp load is correct!!\n");
    return RT_ERR_FAILED;


#else
    uint32  val = 0;

    if(sign == RT_PTP_SIGN_POSTIVE)
    {
        val = (uint32)(timeStamp.sec & 0xFFFFFFFF);
    }
    else
    {
        if(timeStamp.nsec != 0)
        {
            val = (uint32)((~(timeStamp.sec + 1)+1) & 0xFFFFFFFFUL);
            /*bit 31~0*/

            val = (uint32)(((~(timeStamp.sec + 1)+1) >> 32) & 0xFFFF);
            /*bit 47~32*/
        }
        else
        {
            val = (uint32)((~timeStamp.sec +1) & 0xFFFFFFFFUL);
            /*bit 31~0*/

            val = (uint32)(((~timeStamp.sec +1) >> 32) & 0xFFFF);
            /*bit 47~32*/
        }
    }

    if(sign == RT_PTP_SIGN_POSTIVE)
    {
        val = (uint32)(timeStamp.nsec);
    }
    else
    {
        if(timeStamp.nsec != 0)
            val = (uint32)(1000000000 - timeStamp.nsec);
        else
            val = 0;
    }
#endif
    return ret;
} /* end of dal_rtl9607f_rt_time_offsetTime_set */

/* Function Name:
 *      dal_rtl9607f_rt_time_frequency_set
 * Description:
 *      Set frequency of PTP system time.
 * Input:
 *      freq - reference timestamp value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
dal_rtl9607f_rt_time_frequency_set(uint32 freq)
{
    int32   ret = RT_ERR_OK;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PTP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(time_init);

    /* parameter check */
    RT_PARAM_CHK((RTL9607F_TIME_FREQUENCY_MAX < freq), RT_ERR_INPUT);

    ret = aal_ptp_clock_period_set(0,freq);
    if(ret != CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return RT_ERR_FAILED;
    }

    return ret;
} /* end of dal_rtl9607f_rt_time_frequency_set */

/* Function Name:
 *      dal_rtl9607f_rt_time_frequency_get
 * Description:
 *      Set frequency of PTP system time.
 * Input:
 *      freq - reference timestamp value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
dal_rtl9607f_rt_time_frequency_get(uint32 *freq)
{
    int32   ret = RT_ERR_OK;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PTP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(time_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == freq), RT_ERR_NULL_POINTER);

    ret = aal_ptp_clock_period_get(0,freq);
    if(ret != CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return RT_ERR_FAILED;
    }

    return ret;
} /* end of dal_rtl9607f_rt_time_frequency_get */

/* Function Name:
 *      dal_rtl9607f_rt_time_ptpPortEnable_set
 * Description:
 *      Set PTP status of the specified port.
 * Input:
 *      port   - port id
 *      enable - status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - Invalid port id.
 *      RT_ERR_INPUT        - Invalid input parameters.
  * Note:
 *      Set enabling ptp function by port
 */
int32
dal_rtl9607f_rt_time_ptpPortEnable_set(rt_port_t port, rt_enable_t enable)
{
    int32   ret = RT_ERR_OK;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PTP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(time_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    glb_ptpPort_enable[port] = enable;

    ret = dal_rtl9607f_time_igrAction_check();
    if(ret != CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return RT_ERR_FAILED;
    }

    return ret;
} /* end of dal_rtl9607f_rt_time_ptpPortEnable_set */

/* Function Name:
 *      dal_rtl9607f_rt_time_ptpPortEnable_get
 * Description:
 *      Get PTP status of the specified port.
 * Input:
 *      port    - port id
 * Output:
 *      pEnable - status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      Get enabling status of prp function
 */
int32
dal_rtl9607f_rt_time_ptpPortEnable_get(rt_port_t port, rt_enable_t *pEnable)
{
    int32   ret = RT_ERR_OK;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PTP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(time_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    *pEnable = glb_ptpPort_enable[port];

    return ret;
} /* end of dal_rtl9607f_rt_time_ptpPortEnable_get */


/* Function Name:
 *      rt_time_ptpIgrAction_set
 * Description:
 *      Set Type of PTP ingress action .
 * Input:
 *      type   - ptp packtet type
 *      action - ptp ingress action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - Invalid input parameters.
  * Note:
 *      Type is Ethertype0x88f7 or udp port 319/320
 *      Action is no-operation or trap-to-cpu
 */
int32
dal_rtl9607f_rt_time_ptpIgrAction_set(rt_time_ptpIgr_type_t type,rt_time_ptpIgr_action_t action)
{
    int32   ret = RT_ERR_OK;


    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PTP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(time_init);

    /* parameter check */
    RT_PARAM_CHK((RT_PTP_IGR_TYPE_END <= type), RT_ERR_INPUT);
    RT_PARAM_CHK((RT_PTP_IGR_ACTION_END <= action), RT_ERR_INPUT);

    glb_ptpIgr_action[type] = action;

    ret = dal_rtl9607f_time_igrAction_check();
    if(ret != CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return RT_ERR_FAILED;
    }

    return ret;
} /* end of dal_rtl9607f_rt_time_ptpIgrAction_set */

/* Function Name:
 *      dal_rtl9607f_rt_time_ptpIgrAction_get
 * Description:
 *      Get Type of PTP ingress action .
 * Input:
 *      type   - ptp packtet type
 * Output:
 *      pAction - ptp ingress action
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      Type is Ethertype0x88f7 or udp port 319/320
 *      Action is no-operation or trap-to-cpu
 */
int32
dal_rtl9607f_rt_time_ptpIgrAction_get(rt_time_ptpIgr_type_t type,rt_time_ptpIgr_action_t *pAction)
{
    int32   ret = RT_ERR_OK;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PTP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(time_init);

    /* parameter check */
    RT_PARAM_CHK((RT_PTP_IGR_TYPE_END <= type), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    *pAction = glb_ptpIgr_action[type];

    return ret;
} /* end of dal_rtl9607f_rt_time_ptpIgrAction_get */

/* Function Name:
 *      dal_rtl9607f_rt_time_ptpEgrAction_set
 * Description:
 *      Set Type of PTP egress action .
 * Input:
 *      action - ptp engress action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
  * Note:
 *      Action is no-operation or one-step or two-step
 */
int32
dal_rtl9607f_rt_time_ptpEgrAction_set(rt_time_ptpEgr_action_t action)
{
    int32   ret = RT_ERR_OK;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PTP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(time_init);

    /* parameter check */
    RT_PARAM_CHK((RT_PTP_EGR_ACTION_END <= action), RT_ERR_INPUT);

    glb_ptpEgr_action = action;

    return ret;
} /* end of dal_rtl9607f_rt_time_ptpEgrAction_set */

/* Function Name:
 *      dal_rtl9607f_rt_time_ptpEgrAction_get
 * Description:
 *      Get Type of PTP egress action .
 * Input:
 *      None
 * Output:
 *      pAction - ptp egress action
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *     Action is no-operation or one-step or two-step
 */
int32
dal_rtl9607f_rt_time_ptpEgrAction_get(rt_time_ptpEgr_action_t *pAction)
{
    int32   ret = RT_ERR_OK;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PTP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(time_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    *pAction = glb_ptpEgr_action;

    return ret;
} /* end of dal_rtl9607f_rt_time_ptpEgrAction_get */

/* Function Name:
 *      dal_rtl9607f_rt_time_ptp_tx
 * Description:
 *      Send PTP Packet.
 * Input:
 *      port - destination Port
 *      msgLen - length of PTP Packet without CRC
 *      pMsg - content of PTP Packet without CRC
 *      type - none or L2 or UDP for informing which ptp packet is
 *      timeOffset - attach timestamp at offset of PTP packet when one step
 *      udpCSOft - informing chip UDP checksum offset for re-calculating when one step
 * Output:
 *      pTimeStamp - return recording timestamp when two step
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Type is none or L2 or UDP
 */
int32
dal_rtl9607f_rt_time_ptp_tx(uint8 port,uint32 msgLen,uint8 *pMsg,rt_time_ptpEgr_type_t type,uint16 timeOffset,uint16 udpCSOft,rt_time_timeStamp_t *pTimeStamp)
{
    int32   ret = RT_ERR_OK,i;
    HEADER_A_T ni_header_a;
    struct sk_buff *p_skb;
    NI_HV_GLB_PTP_INTERRUPT_t ni_ptp_interrupt;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PTP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(time_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pMsg), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pTimeStamp), RT_ERR_NULL_POINTER);

    if(glb_ptpPort_enable[port] != ENABLED)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PTP), "");
        return RT_ERR_FAILED;
    }

    memset(&ni_header_a, 0, sizeof(HEADER_A_T));

    if(type == RT_PTP_EGR_TYPE_NONE)
    {
        ni_header_a.bits.hdr_type = 0;
    }
    else if (type == RT_PTP_EGR_TYPE_L2)
    {
        ni_header_a.bits.hdr_type = 3;
        //one step
        if(glb_ptpEgr_action == RT_PTP_EGR_ACTION_ONESTEP)
        {
            ni_header_a.bits.bits_32_63.ptp_cmd.cmd_offset = timeOffset;
            ni_header_a.bits.bits_32_63.ptp_cmd.ptp_type = 0;
            ni_header_a.bits.bits_32_63.ptp_cmd.cmd_mode = 1;
            ni_header_a.bits.bits_32_63.ptp_cmd.cmd_op = 1;
        }
        //two step
        else if(glb_ptpEgr_action == RT_PTP_EGR_ACTION_TWOSTEP)
        {
            ni_header_a.bits.bits_32_63.ptp_cmd.ptp_type = 0;
            ni_header_a.bits.bits_32_63.ptp_cmd.cmd_mode = 0;
            ni_header_a.bits.bits_32_63.ptp_cmd.cmd_op = 0;
        }
    }
    else
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return RT_ERR_FAILED;
    }

    ni_header_a.bits.fe_bypass = 1;
    ni_header_a.bits.ldpid = port;

    p_skb = netdev_alloc_skb(NULL, 96);
    if (p_skb == NULL)
    {
        RTL9607F_LOG_ERR("netdev_alloc_skb fail\n");
        return RT_ERR_FAILED;
    }

    memcpy(p_skb->data,pMsg,msgLen);
    p_skb->len = msgLen;

    if(glb_ptpEgr_action == RT_PTP_EGR_ACTION_TWOSTEP)
    {
        ni_ptp_interrupt.wrd = (1<<port);
        CA_NI_REG_WRITE(ni_ptp_interrupt.wrd,NI_HV_GLB_PTP_INTERRUPT);
    }

    if(__rtk_ni_start_xmit(p_skb, &ni_header_a, NULL , NULL , NULL, 1) != NETDEV_TX_OK)
    {
        RTL9607F_LOG_ERR("%s %d : TX Error!!\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(glb_ptpEgr_action == RT_PTP_EGR_ACTION_TWOSTEP)
    {
        for(i=0;i<20;i++)
        {
            ca_udelay(100);
            ni_ptp_interrupt.wrd = CA_NI_REG_READ(NI_HV_GLB_PTP_INTERRUPT);
            if((ni_ptp_interrupt.wrd&(1<<port)))
            {
                ret = aal_ptp_two_step_timer_get(0,port,&pTimeStamp->sec,&pTimeStamp->nsec);
                if(ret != CA_E_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                    return RT_ERR_FAILED;
                }
                RTL9607F_LOG_DBG("%s %d : %dth ni_ptp_interrupt.wrd = 0x%x!!\n",__FUNCTION__,__LINE__,i,ni_ptp_interrupt.wrd);
                ni_ptp_interrupt.wrd = (1<<port);
                CA_NI_REG_WRITE(ni_ptp_interrupt.wrd,NI_HV_GLB_PTP_INTERRUPT);
                return RT_ERR_OK;
            }
        }
        RTL9607F_LOG_ERR("%s %d : Can't get timestamp!!\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    return ret;
} /* end of dal_rtl9607f_rt_time_ptp_tx */

/* Function Name:
 *      dal_rtl9607f_rt_time_ptp_rx_callback_register
 * Description:
 *      PTP packet rx call back
 * Input:
 *      ptpRx - call back function of PTP Packet
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      rt_time_ptp_rx_callback)(uins32* srcPort, uint32 *msgLen,uint8 *pMsg, rt_time_timeStamp_t *pTimeStamp), pTimeStamp is recording timestamp, pTimeStamp is recording timestamp
 */
int32
dal_rtl9607f_rt_time_ptp_rx_callback_register(rt_time_ptp_rx_callback ptpRx)
{
    int32   ret = RT_ERR_OK;

    if(DISABLED == gPTPTrapEnable)
    {
        if(nic_register_rxhook(0xffffffff,RENIC_RXPRI_PATCH,ptp_pkt_rx) != NIC_DRIVER_IS_READY)
        {
            RT_ERR(ret, (MOD_DAL|MOD_L2), "");
            return RT_ERR_FAILED;
        }
        gPTPTrapEnable = ENABLED;
    }

    gPTPRxCallback = ptpRx;

    return ret;
} /* end of dal_rtl9607f_rt_time_ptp_rx_callback_register */

/* Function Name:
 *      dal_rtl9607f_rt_time_ponTodTime_set
 * Description:
 *      Set the PON TOD time.
 * Input:
 *      ponTod   - pon tod configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 * Note:
 *      Set corresponding superfram/localtime to update timer
 */
int32
dal_rtl9607f_rt_time_ponTodTime_set(rt_pon_tod_t ponTod)
{
    int32   ret = RT_ERR_OK;
    ca_uint32_t pon_mode = aal_pon_mac_mode_get(0);
    ca_uint64_t sec;
    ca_uint32_t nsec;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PTP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(time_init);

    /* parameter check */
    RT_PARAM_CHK((RTL9607F_TIME_SEC_MAX < ponTod.timeStamp.sec), RT_ERR_INPUT);
    RT_PARAM_CHK((RTL9607F_TIME_NSEC_MAX <= ponTod.timeStamp.nsec), RT_ERR_INPUT);

    switch(pon_mode)
    {
        case ONU_PON_MAC_EPON_1G:
            RT_PARAM_CHK((0 != ponTod.ponMode), RT_ERR_INPUT);

            ret = aal_epon_onu_mpcp_timer_match_cfg_set(0,0,ponTod.startPoint.localTime);
            if(ret != CA_E_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return RT_ERR_FAILED;
            }

            break;

        case ONU_PON_MAC_GPON:
            RT_PARAM_CHK((1 != ponTod.ponMode), RT_ERR_INPUT);

            ret = aal_gpon_sup_frame_value_set(0,ponTod.startPoint.superFrame);
            if(ret != CA_E_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return RT_ERR_FAILED;
            }

            break;

        default:
            return RT_ERR_INPUT;
    }

    sec = ponTod.timeStamp.sec;
    nsec = ponTod.timeStamp.nsec;
    ret = aal_ptp_load_timer_set(0,sec,nsec);
    if(ret != CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return RT_ERR_FAILED;
    }

    return ret;
} /* end of dal_rtl9607f_rt_time_ponTodTime_set */

/* Function Name:
 *      dal_rtl9607f_rt_time_ponTodTime_get
 * Description:
 *      Get the PON TOD time.
 * Input:
 *      None
 * Output:
 *      pPonTod   - pon tod configuration
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 * Note:
 *      Get corresponding superfram/localtime to update timer
 */
int32
dal_rtl9607f_rt_time_ponTodTime_get(rt_pon_tod_t *pPonTod)
{
    int32   ret = RT_ERR_OK;
    ca_uint32_t pon_mode = aal_pon_mac_mode_get(0);
    ca_uint64_t sec;
    ca_uint32_t nsec;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PTP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(time_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPonTod), RT_ERR_NULL_POINTER);

    switch(pon_mode)
    {
        case ONU_PON_MAC_EPON_1G:
            pPonTod->ponMode = 0;

            ret = aal_epon_onu_mpcp_timer_match_cfg_get(0,0,&pPonTod->startPoint.localTime);
            if(ret != CA_E_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return RT_ERR_FAILED;
            }

            break;

        case ONU_PON_MAC_GPON:
            pPonTod->ponMode = 1;

            ret = aal_gpon_sup_frame_value_get(0,&pPonTod->startPoint.superFrame);
            if(ret != CA_E_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_L2), "");
                return RT_ERR_FAILED;
            }

            break;
        default:
            return RT_ERR_INPUT;
    }

    ret = aal_ptp_load_timer_get(0,&sec,&nsec);
    if(ret != CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return RT_ERR_FAILED;
    }

    pPonTod->timeStamp.sec = sec;
    pPonTod->timeStamp.nsec = nsec;

    return ret;
} /* end of dal_rtl9607f_rt_time_ponTodTime_get */

/* Function Name:
 *      dal_rtl9607f_rt_time_ponTodTimeEnable_set
 * Description:
 *      Set the PON TOD time.
 * Input:
 *      enable   - enable pon tod configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 * Note:
 *      Enable/Disable PON ToD to update time
 */
int32
dal_rtl9607f_rt_time_ponTodTimeEnable_set(rt_enable_t enable)
{
    int32   ret = RT_ERR_OK;
    aal_ptp_match_load_mask_t mask;
    aal_ptp_match_load_t load;

    memset(&load,0,sizeof(aal_ptp_match_load_t));

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PTP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(time_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    mask.u32 = 0;
    mask.s.pon_load = 1;

    load.pon_load = (enable == ENABLED)?1:0;

    ret = aal_ptp_match_load_set(0,mask,load);
    if(ret != CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return RT_ERR_FAILED;
    }

    return ret;
} /* end of dal_rtl9607f_rt_time_ponTodTimeEnable_set */

/* Function Name:
 *      dal_rtl9607f_rt_time_ponTodTimeEnable_get
 * Description:
 *      Get the PON TOD time.
 * Input:
 *      None
 * Output:
 *      pEnable   - enable pon tod configuration
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 * Note:
 *      Getting status enable/eisable PON ToD to update time
 */
int32
dal_rtl9607f_rt_time_ponTodTimeEnable_get(rt_enable_t *pEnable)
{
    int32   ret = RT_ERR_OK;
    aal_ptp_match_load_t load;

    memset(&load,0,sizeof(aal_ptp_match_load_t));

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PTP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(time_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    ret = aal_ptp_match_load_get(0,&load);
    if(ret != CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_L2), "");
        return RT_ERR_FAILED;
    }

    *pEnable = (load.pon_load ==1)?ENABLED:DISABLED;

    return ret;
} /* end of dal_rtl9607f_rt_time_ponTodTimeEnable_get */
