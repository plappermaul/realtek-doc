/*
 * Copyright (C) 2018 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: $
 * $Date: $
 *
 * Purpose : Definition of PON MAC API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) PON mac
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/ponmac.h>
#include <hal/chipdef/ca8279/ca8279_def.h>
#include <dal/ca8279/dal_ca8279.h>
#include <dal/ca8279/dal_ca8279_ponmac.h>
#include <dal/ca8279/dal_ca8279_qos.h>
#include <dal/ca8279/dal_ca8279_gpio.h>

#include "cortina-api/include/onu.h"
#include <aal_pon.h>
#include <aal_l2_te.h>
#include <soc/cortina/ca8279_pon_registers.h>
#include <cortina-api/include/rate.h>
/*
 * Macro Declaration
 */
#define MQNUM 8

/*
 * Symbol Definition
 */
static uint32 ponmac_init = INIT_NOT_COMPLETED;
static uint8 GBL_CAR_MODE = FALSE;

typedef struct ca_rtk_ponmac_queueCfg_s
{
    uint32 cir;
    uint32 pir;
    uint32 weight;
} ca_rtk_ponmac_queueCfg_t;

ca_rtk_ponmac_queueCfg_t g_ponqueue[MQNUM];

static int32
_dal_ca8279_car_mode_set(uint8 car_mode)
{
    int32   ret;
    uint32 queueId;
    ca_policer_t policer;
    ca_shaper_t shaper;
    aal_l2_te_glb_cfg_msk_t glb_cfg_msk;
    aal_l2_te_glb_cfg_t glb_cfg;
    aal_l2_te_wred_profile_sel_msk_t sel_msk;
    aal_l2_te_wred_profile_sel_t sel;

    memset(&glb_cfg_msk,0,sizeof(aal_l2_te_glb_cfg_msk_t));
    memset(&glb_cfg,0,sizeof(aal_l2_te_glb_cfg_t));
    memset(&sel_msk,0,sizeof(aal_l2_te_wred_profile_sel_msk_t));
    memset(&sel,0,sizeof(aal_l2_te_wred_profile_sel_t));

    if(car_mode == TRUE)
    {
        shaper.enable = 0;
        /*disable queue shaper*/
        for(queueId = 0;queueId < HAL_MAX_NUM_OF_QUEUE();queueId++)
        {
            if((ret = ca_queue_shaper_set(1,CA_PORT_ID(CA_PORT_TYPE_ETHERNET,7),queueId,&shaper)) != CA_E_OK)
            {
                RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
                return RT_ERR_FAILED;
            }
        }

        /*disable tail drop*/
        glb_cfg_msk.s.tail_drop_ena = 1;
        glb_cfg.tail_drop_ena = 0;
        if((ret = aal_l2_te_glb_cfg_set(1,glb_cfg_msk,&glb_cfg)) != CA_E_OK)
        {
            RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
            return RT_ERR_FAILED;
        }

        /*per queue select wred profile*/
        sel_msk.s.profile_id_0 = 1;
        sel_msk.s.profile_id_1 = 1;
        sel_msk.s.profile_id_2 = 1;
        sel_msk.s.profile_id_3 = 1;
        sel_msk.s.profile_id_4 = 1;
        sel_msk.s.profile_id_5 = 1;
        sel_msk.s.profile_id_6 = 1;
        sel_msk.s.profile_id_7 = 1;

        sel.profile_id[0] = 1;
        sel.profile_id[1] = 2;
        sel.profile_id[2] = 3;
        sel.profile_id[3] = 4;
        sel.profile_id[4] = 5;
        sel.profile_id[5] = 6;
        sel.profile_id[6] = 7;
        sel.profile_id[7] = 8;
        if((ret = aal_l2_te_wred_profile_sel_set(1,23,sel_msk,&sel)) != CA_E_OK)
        {
            RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
            return RT_ERR_FAILED;
        }
    }
    else
    {
        policer.mode = CA_POLICER_MODE_DISABLE;
        /*disable queue policer*/
        for(queueId = 0;queueId < HAL_MAX_NUM_OF_QUEUE();queueId++)
        {
            if((ret = ca_l2_flow_policer_set(1,queueId+1,&policer)) != CA_E_OK)
            {
                RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
                return RT_ERR_FAILED;
            }
        }

        /*enable tail drop*/
        glb_cfg_msk.s.tail_drop_ena = 1;
        glb_cfg.tail_drop_ena = 1;
        if((ret = aal_l2_te_glb_cfg_set(1,glb_cfg_msk,&glb_cfg)) != CA_E_OK)
        {
            RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
            return RT_ERR_FAILED;
        }

        /*per queue select default wred profile*/
        sel_msk.s.profile_id_0 = 1;
        sel_msk.s.profile_id_1 = 1;
        sel_msk.s.profile_id_2 = 1;
        sel_msk.s.profile_id_3 = 1;
        sel_msk.s.profile_id_4 = 1;
        sel_msk.s.profile_id_5 = 1;
        sel_msk.s.profile_id_6 = 1;
        sel_msk.s.profile_id_7 = 1;

        sel.profile_id[0] = 0;
        sel.profile_id[1] = 0;
        sel.profile_id[2] = 0;
        sel.profile_id[3] = 0;
        sel.profile_id[4] = 0;
        sel.profile_id[5] = 0;
        sel.profile_id[6] = 0;
        sel.profile_id[7] = 0;
        if((ret = aal_l2_te_wred_profile_sel_set(1,23,sel_msk,&sel)) != CA_E_OK)
        {
            RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
            return RT_ERR_FAILED;
        }
    }

    GBL_CAR_MODE = car_mode;

    return RT_ERR_OK;
}

static int32
_dal_ca8279_ponmac_queue_set(uint32 queueId, ca_rtk_ponmac_queueCfg_t queueCfg)
{
    int32   ret;
    ca_policer_t policer;
    ca_shaper_t shaper;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(ponmac_init);

    /* parameter check */
    RT_PARAM_CHK((queueId >= MQNUM), RT_ERR_INPUT);

//printk("%s %d,queueId=%d cir=%d pir=%d weight=%d GBL_CAR_MODE=%d\n"
//    ,__FUNCTION__,__LINE__,queueId,queueCfg.cir,queueCfg.pir,queueCfg.weight,GBL_CAR_MODE);

    /* function body */
    if(GBL_CAR_MODE == TRUE)
    {
        policer.mode = CA_POLICER_MODE_TRTCM;

        if(queueCfg.cir >= CA8279_METER_RATE_MAX)
        {
            policer.cir = CA8279_METER_RATE_MAX;
        }
        else
        {
            policer.cir = queueCfg.cir;
        }
        
        if(policer.cir == 0)
        {
            policer.cbs = 1; //set to minimum value
        }
        else if(policer.cir < 100)
        {
            policer.cbs = 10; 
        }
        else if((policer.cir >= 100) && (policer.cir < 1000))
        {
            policer.cbs = 64; //set to the value that approach to RTK setting (64  * 256 = 16384 ~= 0x3fff)
        }
        else if((policer.cir >= 1000) && (policer.cir < 10000))
        {
            policer.cbs = 100;
        }
        else if((policer.cir >= 10000) && (policer.cir < 100000))
        {
            policer.cbs = 1000;
        }
        else
        {
            policer.cbs = 0xfff; 
        }

        if(queueCfg.pir >= CA8279_METER_RATE_MAX)
        {
            policer.pir = CA8279_METER_RATE_MAX;
        }
        else
        {
            policer.pir = queueCfg.pir;
        }

        if(policer.pir == 0)
        {
            policer.pbs = 1; //set to minimum value
        }
        else if(policer.pir < 100)
        {
            policer.pbs = 10; 
        }
        else if((policer.pir >= 100) && (policer.pir < 1000))
        {
            policer.pbs = 64; //set to the value that approach to RTK setting (64  * 256 = 16384 ~= 0x3fff)
        }
        else if((policer.pir >= 1000) && (policer.pir < 10000))
        {
            policer.pbs = 100;
        }
        else if((policer.pir >= 10000) && (policer.pir < 100000))
        {
            policer.pbs = 1000;
        }
        else
        {
            policer.pbs = 0xfff; 
        }

        policer.pps = 0; /* True - PPS mode, false - BPS mode */

        if((ret = ca_l2_flow_policer_set(1,queueId+1,&policer)) != CA_E_OK)
        {
            RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
            return RT_ERR_FAILED;
        }
    }
    else
    {
        if(queueCfg.pir == 0 || queueCfg.pir >= CA8279_METER_RATE_MAX)
            shaper.enable =0;
        else
            shaper.enable =1;
        shaper.pps = 0; /* True - PPS mode, false - BPS mode */
        shaper.rate = queueCfg.pir;
        if(shaper.rate == 0)
        {
            shaper.burst_size = 1; //set to minimum value
        }
        else if(shaper.rate < 100)
        {
            shaper.burst_size = 10; 
        }
        else if((shaper.rate >= 100) && (shaper.rate < 1000))
        {
            shaper.burst_size = 64; //set to the value that approach to RTK setting (64  * 256 = 16384 ~= 0x3fff)
        }
        else if((shaper.rate >= 1000) && (shaper.rate < 10000))
        {
            shaper.burst_size = 100;
        }
        else if((shaper.rate >= 10000) && (shaper.rate < 100000))
        {
            shaper.burst_size = 1000;
        }
        else
        {
            shaper.burst_size = 0xfff; 
        }

        if((ret = ca_queue_shaper_set(1,CA_PORT_ID(CA_PORT_TYPE_ETHERNET,7),queueId,&shaper)) != CA_E_OK)
        {
            RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
            return RT_ERR_FAILED;
        }
    }

    return RT_ERR_OK;
}

static int32
_dal_ca8279_ponmac_queue_update(uint32 queueId, uint32 cir,uint32 pir,uint32 weight)
{
    int32 ret;
    uint32 i;
    uint8 adjCarFlag = FALSE;
    rtk_qos_queue_weights_t qWeights;

    /*update global queue setting*/
    g_ponqueue[queueId].cir = cir;
    g_ponqueue[queueId].pir = pir;
    g_ponqueue[queueId].weight = weight;

    /*adjust global queue setting for sp+wrr*/
    if(weight!=0 && queueId!=0)
    {
        for(i=0;i<queueId;i++)
        {
            if(g_ponqueue[i].weight==0)
            {
                g_ponqueue[i].weight=1;
            }
        }
    }

    if(weight==0 && queueId!=7)
    {
        for(i=(queueId+1);i<MQNUM;i++)
        {
            if(g_ponqueue[i].weight!=0)
            {
                g_ponqueue[i].weight=0;
            }
        }
    }

    /*Checking CIR*/
    for(i=0;i<MQNUM;i++)
    {
        if(g_ponqueue[i].cir!=0)
        {
            adjCarFlag = TRUE;
            break;
        }
    }

    /*CAR mode setting*/
    if(adjCarFlag == TRUE) 
    {
        if((ret = _dal_ca8279_car_mode_set(TRUE)) != CA_E_OK)
        {
            RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
            return ret;
        }
    }
    else
    {
        if((ret = _dal_ca8279_car_mode_set(FALSE)) != CA_E_OK)
        {
            RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
            return ret;
        }
    }

    /*rate setting*/
    for(i=0;i<MQNUM;i++)
    {
        if((ret = _dal_ca8279_ponmac_queue_set(i,g_ponqueue[i])) != CA_E_OK)
        {
            RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
            return ret;
        }
    }

    /*weight setting*/
    if((ret = dal_ca8279_qos_schedulingQueue_get(7,&qWeights)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    
    
    for(i=0;i<MQNUM;i++)
    {
        qWeights.weights[i] = g_ponqueue[i].weight;
    }
    
    if((ret = dal_ca8279_qos_schedulingQueue_set(7,&qWeights)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_ca8279_ponmac_init
 * Description:
 *      Configure PON MAC initial settings
 * Input:
 *      None.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_QUEUE_NUM - Invalid queue number
 * Note:
 */
int32
dal_ca8279_ponmac_init(void)
{
    int32 ret;
    aal_l2_te_wred_profile_msk_t msk;
    aal_l2_te_wred_profile_t profile;
    int i;

    ponmac_init = INIT_COMPLETED;

#ifdef CONFIG_BOARD_REALTEK_CA8277_PON_GPIO
    if((ret = dal_ca8279_gpio_state_set(7,ENABLED)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    if((ret = dal_ca8279_gpio_mode_set(7,GPIO_OUTPUT)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    if((ret = dal_ca8279_gpio_state_set(11,ENABLED)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    if((ret = dal_ca8279_gpio_mode_set(11,GPIO_OUTPUT)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    if((ret = dal_ca8279_ponmac_txDisableGpio_set(DISABLED)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    if((ret = dal_ca8279_ponmac_txPowerDisableGpio_set(DISABLED)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return RT_ERR_FAILED;
    }
#endif

    /*CAR mode initial*/
    memset(&msk,0,sizeof(aal_l2_te_wred_profile_msk_t));
    memset(&profile,0,sizeof(aal_l2_te_wred_profile_t));

    msk.s.mark_dp=1;
    msk.s.mark_idx=1;
    msk.s.unmark_dp=1;
    msk.s.unmark_idx=1;
    
    profile.mark_dp[0]=63;
    profile.mark_dp[1]=63;
    profile.mark_dp[2]=63;
    profile.mark_dp[3]=63;
    profile.mark_dp[4]=63;
    profile.mark_dp[5]=63;
    profile.mark_dp[6]=63;
    profile.mark_dp[7]=63;
    profile.mark_dp[8]=0;
    profile.mark_dp[9]=0;
    profile.mark_dp[10]=0;
    profile.mark_dp[11]=0;
    profile.mark_dp[12]=0;
    profile.mark_dp[13]=0;
    profile.mark_dp[14]=0;
    profile.mark_dp[15]=0;

    profile.mark_idx=9;

    profile.unmark_idx=9;
    
    for(i=1;i<=8;i++)
    {
        profile.mark_dp[9-i]=0;
        if((ret = aal_l2_te_wred_profile_set(1,i,msk,&profile)) != CA_E_OK)
        {
            RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
            return RT_ERR_FAILED;
        }
    }

    /*queue initialization*/
    for(i=0;i<MQNUM;i++)
    {
        g_ponqueue[i].cir = 0;
        g_ponqueue[i].pir = CA8279_METER_RATE_MAX;
        g_ponqueue[i].weight = 0;
    }

    return RT_ERR_OK;
} /* end of dal_ca8279_ponmac_init */

/* Function Name:
 *      dal_ca8279_ponmac_losState_get
 * Description:
 *      Get the current optical lost of signal (LOS) state
 * Input:
 *      None
 * Output:
 *      pEnable  - the current optical lost of signal state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 */
int32
dal_ca8279_ponmac_losState_get(rtk_enable_t *pState)
{
    ca_status_t  ret = CA_E_OK;
    GLOBAL_OPT_MODULE_STATUS_t opt_stas;

    /* check Init status */
    RT_INIT_CHK(ponmac_init);

    //opt_stas.wrd = AAL_GLB_REG_READ(GLOBAL_OPT_MODULE_STATUS);

    if((ret = ca_onu_losState_get(0, &opt_stas)) == CA_E_OK)
    {
        if((1 == opt_stas.bf.opt_rx_los) || (1 == opt_stas.bf.opt_mod_abs))
            *pState = ENABLED;
        else
            *pState = DISABLED;
    }

    return ret;
}   /* end of dal_ca8279_ponmac_losState_get */

/* Function Name:
 *      dal_ca8279_ponmac_queue_del
 * Description:
 *      delete queue from given scheduler id
 * Input:
 *      pQueue         - queue id and scheduler id for ths queue.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                               - OK
 *      RT_ERR_FAILED                           - Failed
 *      RT_ERR_NULL_POINTER                     - Pointer pQueue point to NULL.
 *      RT_ERR_INPUT                            - Invalid input parameters.
 * Note:
 *      None
 */
int32
dal_ca8279_ponmac_queue_del(rtk_ponmac_queue_t *pQueue)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(ponmac_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pQueue), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pQueue->schedulerId >= HAL_MAX_NUM_OF_GPON_TCONT()), RT_ERR_INPUT);
    RT_PARAM_CHK((pQueue->queueId >= MQNUM), RT_ERR_INPUT);

    /* function body */
    if((ret = _dal_ca8279_ponmac_queue_update(pQueue->queueId,0,CA8279_METER_RATE_MAX,0)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_ca8279_ponmac_queue_del */

/* Function Name:
 *      dal_ca8279_ponmac_queue_get
 * Description:
 *      get queue setting
 * Input:
 *      pQueue         - queue id and scheduler id for ths queue.
 * Output:
 *      pQueueCfg     - queue configuration
 * Return:
 *      RT_ERR_OK                               - OK
 *      RT_ERR_FAILED                           - Failed
 *      RT_ERR_NULL_POINTER                     - Pointer pQueueList/pQueue point to NULL.
 *      RT_ERR_INPUT                            - Invalid input parameters.
 * Note:
 *      None
 */
int32
dal_ca8279_ponmac_queue_get(rtk_ponmac_queue_t *pQueue, rtk_ponmac_queueCfg_t *pQueueCfg)
{
    int32   ret;
    ca_policer_t policer;
    ca_shaper_t shaper;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(ponmac_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pQueue), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pQueueCfg), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pQueue->schedulerId >= HAL_MAX_NUM_OF_GPON_TCONT()), RT_ERR_INPUT);
    RT_PARAM_CHK((pQueue->queueId >= MQNUM), RT_ERR_INPUT);

    /* function body */
    pQueueCfg->cir = g_ponqueue[pQueue->queueId].cir;
    pQueueCfg->pir = g_ponqueue[pQueue->queueId].pir;
    if(g_ponqueue[pQueue->queueId].weight == 0)
        pQueueCfg->type = STRICT_PRIORITY;
    else
        pQueueCfg->type = WFQ_WRR_PRIORITY;
    pQueueCfg->weight = g_ponqueue[pQueue->queueId].weight;

    return RT_ERR_OK;
} /* end of dal_ca8279_ponmac_queue_get */

/* Function Name:
 *      dal_ca8279_ponmac_queue_add
 * Description:
 *      Add queue to given scheduler id and apply queue setting
 * Input:
 *      pQueue         - queue id and scheduler id for ths queue.
 *      pQueueCfg     - queue configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                               - OK
 *      RT_ERR_FAILED                           - Failed
 *      RT_ERR_NULL_POINTER                     - Pointer pQueueList/pQueue point to NULL.
 *      RT_ERR_INPUT                            - Invalid input parameters.
 * Note:
 *      None
 */
int32
dal_ca8279_ponmac_queue_add(rtk_ponmac_queue_t *pQueue, rtk_ponmac_queueCfg_t *pQueueCfg)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(ponmac_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pQueue), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pQueueCfg), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pQueue->schedulerId >= HAL_MAX_NUM_OF_GPON_TCONT()), RT_ERR_INPUT);
    RT_PARAM_CHK((pQueue->queueId >= MQNUM), RT_ERR_INPUT);
    RT_PARAM_CHK((pQueueCfg->type >= QUEUE_TYPE_END), RT_ERR_INPUT);
    RT_PARAM_CHK((pQueueCfg->weight > 255), RT_ERR_INPUT);

//printk("%s %d pQueue->queueId=%d pQueueCfg->type=%d pQueueCfg->cir=%d pQueueCfg->pir=%d\n"
//    ,__FUNCTION__,__LINE__,pQueue->queueId,pQueueCfg->type,pQueueCfg->cir,pQueueCfg->pir);

    /* function body */
    if((ret = _dal_ca8279_ponmac_queue_update(pQueue->queueId,pQueueCfg->cir,pQueueCfg->pir,pQueueCfg->weight)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_ca8279_ponmac_queue_add */

/* Function Name:
 *      dal_ca8279_ponmac_egrBandwidthCtrlRate_get
 * Description:
 *      Get the egress bandwidth control rate.
 * Input:
 *      None
 * Output:
 *      pRate - egress bandwidth control rate
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      (1) The actual rate is "rate * chip granularity".
 *      (2) The unit of granularity in Apollo is Kbps.
 */
int32
dal_ca8279_ponmac_egrBandwidthCtrlRate_get(uint32 *pRate)
{
    uint32 data;
    int32 ret;
    ca_shaper_t shaper;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC), "");

    /* check Init status */
    RT_INIT_CHK(ponmac_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pRate), RT_ERR_NULL_POINTER);

    /* function body */
    if((ret = ca_port_shaper_get(1,CA_PORT_ID(CA_PORT_TYPE_ETHERNET,7),&shaper)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    if(shaper.enable == 0)
    {
        *pRate = CA8279_METER_RATE_MAX;
    }
    else
    {
        *pRate = shaper.rate;
    }

    return RT_ERR_OK;
}   /* end of dal_ca8279_ponmac_egrBandwidthCtrlRate_get */

/* Function Name:
 *      dal_ca8279_ponmac_egrBandwidthCtrlRate_set
 * Description:
 *      Set the pon port egress bandwidth control rate.
 * Input:
 *      rate - egress bandwidth control rate
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - Invalid port id
 *      RT_ERR_RATE    - Invalid input rate
 * Note:
 *      (1) The actual rate is "rate * chip granularity".
 *      (2) The unit of granularity in ca8279 is 1Kbps.
 */
int32
dal_ca8279_ponmac_egrBandwidthCtrlRate_set(uint32 rate)
{
    uint32 data;
    int32 ret;
    ca_shaper_t shaper;
    
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC), "rate=%d",rate);

    /* check Init status */
    RT_INIT_CHK(ponmac_init);

    /* parameter check */
    RT_PARAM_CHK((CA8279_PON_EGRESS_RATE_MAX < rate), RT_ERR_INPUT);

    /* function body */
    if(rate >= CA8279_METER_RATE_MAX)
        shaper.enable = 0;
    else
        shaper.enable = 1;
    shaper.pps = 0; /* True - PPS mode, false - BPS mode */
    shaper.rate = rate;
    if(shaper.rate == 0)
    {
        shaper.burst_size = 1; //set to minimum value
    }
    else if(shaper.rate < 100)
    {
        shaper.burst_size = 10; 
    }
    else if((shaper.rate >= 100) && (shaper.rate < 1000))
    {
        shaper.burst_size = 64; //set to the value that approach to RTK setting (64  * 256 = 16384 ~= 0x3fff)
    }
    else if((shaper.rate >= 1000) && (shaper.rate < 10000))
    {
        shaper.burst_size = 100;
    }
    else if((shaper.rate >= 10000) && (shaper.rate < 100000))
    {
        shaper.burst_size = 1000;
    }
    else
    {
        shaper.burst_size = 0xfff; 
    }

//printk("%s %d shaper.rate=%d\n",__FUNCTION__,__LINE__,shaper.rate);

    if((ret = ca_port_shaper_set(1,CA_PORT_ID(CA_PORT_TYPE_ETHERNET,7),&shaper)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}   /* end of dal_ca8279_ponmac_egrBandwidthCtrlRate_set */

/* Function Name:
 *      dal_ca8279_ponmac_egrBandwidthCtrlIncludeIfg_get
 * Description:
 *      get the pon port egress bandwidth control IFG state.
 * Input:
 *      None
 * Output:
 *      pIfgInclude - pointer of state of IFG 
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
int32
dal_ca8279_ponmac_egrBandwidthCtrlIncludeIfg_get(rtk_enable_t *pIfgInclude)
{
    int32 ret;
    ca_shaper_config_t config;

    /* check Init status */
    RT_INIT_CHK(ponmac_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIfgInclude), RT_ERR_NULL_POINTER);

    /* function body */
    if((ret = ca_shaper_config_get(1,&config)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    
    if(config.overhead == 0)
        *pIfgInclude = DISABLED;
    else
        *pIfgInclude = ENABLED;

    return RT_ERR_OK;
}   /* end of dal_ca8279_ponmac_egrBandwidthCtrlIncludeIfg_get */


/* Function Name:
 *      dal_ca8279_ponmac_egrBandwidthCtrlIncludeIfg_set
 * Description:
 *      set the pon port egress bandwidth control IFG state.
 * Input:
 *      ifgInclude - state of IFG 
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
int32
dal_ca8279_ponmac_egrBandwidthCtrlIncludeIfg_set(rtk_enable_t ifgInclude)
{
    int32 ret;
    ca_shaper_config_t config;
    ca_policer_config_t pol_config;

    /* check Init status */
    RT_INIT_CHK(ponmac_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= ifgInclude), RT_ERR_INPUT);

    /* function body */
    if(ifgInclude == ENABLED)
    {
        config.overhead = 20;
        pol_config.overhead = 20;
    }
    else
    {
        config.overhead = 0;
        pol_config.overhead = 0;
    }

    if((ret = ca_shaper_config_set(1,&config)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    if((ret = ca_l2_policer_config_set(1,&config)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_RATE | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}   /* end of dal_ca8279_ponmac_egrBandwidthCtrlIncludeIfg_set */

/* Function Name:
 *      dal_ca8279_ponmac_txDisableGpio_get
 * Description:
 *      get the pon module tx disable state.
 * Input:
 *      aggIndex  - aggregate tcont index
 * Output:
 *      pEnable - pointer of state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
int32
dal_ca8279_ponmac_txDisableGpio_get(rtk_enable_t *pEnable)
{
    int32 ret;
    uint32 data;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC), "");

    /* check Init status */
    RT_INIT_CHK(ponmac_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    if((ret = dal_ca8279_gpio_databit_get(11,&data)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    if(data == 1)
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;

    return RT_ERR_OK;
}   /* end of dal_ca8279_ponmac_txDisableGpio_get */

/* Function Name:
 *      dal_ca8279_ponmac_txDisableGpio_set
 * Description:
 *      set the pon module tx disable state.
 * Input:
 *      enable - state
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
int32
dal_ca8279_ponmac_txDisableGpio_set(rtk_enable_t enable)
{
    int32 ret;
    uint32 data;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC), "enable=%d",enable);

    /* check Init status */
    RT_INIT_CHK(ponmac_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    /* function body */
    if(enable == ENABLED)
        data = 1;
    else
        data = 0;

    if((ret = dal_ca8279_gpio_databit_set(11,data)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}   /* end of dal_ca8279_ponmac_txDisableGpio_set */

/* Function Name:
 *      dal_ca8279_ponmac_txPowerDisableGpio_get
 * Description:
 *      get the pon module tx power disable state.
 * Input:
 *      aggIndex  - aggregate tcont index
 * Output:
 *      pEnable - pointer of state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
int32
dal_ca8279_ponmac_txPowerDisableGpio_get(rtk_enable_t *pEnable)
{
    int32 ret;
    uint32 data;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC), "");

    /* check Init status */
    RT_INIT_CHK(ponmac_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
    if((ret = dal_ca8279_gpio_databit_get(7,&data)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    if(data == 1)
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;

    return RT_ERR_OK;
}   /* end of dal_ca8279_ponmac_txPowerDisableGpio_get */

/* Function Name:
 *      dal_ca8279_ponmac_txPowerDisableGpio_set
 * Description:
 *      set the pon module tx power disable state.
 * Input:
 *      enable - state
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
int32
dal_ca8279_ponmac_txPowerDisableGpio_set(rtk_enable_t enable)
{
    int32 ret;
    uint32 data;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC), "enable=%d",enable);

    /* check Init status */
    RT_INIT_CHK(ponmac_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    /* function body */
    if(enable == ENABLED)
        data = 1;
    else
        data = 0;

    if((ret = dal_ca8279_gpio_databit_set(7,data)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}   /* end of dal_ca8279_ponmac_txPowerDisableGpio_set */