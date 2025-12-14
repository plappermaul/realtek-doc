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
#include <linux/delay.h>
#include <common/rt_type.h>
#include <rtk/ponmac.h>
#include <rtk/rt/rt_rate.h>
#include <hal/chipdef/ca8277b/ca8277b_def.h>
#include <dal/ca8277b/dal_ca8277b.h>
#include <dal/ca8277b/dal_ca8277b_ponmac.h>
#include <dal/ca8277b/dal_ca8277b_qos.h>
#include <dal/ca8277b/dal_ca8277b_gpio.h>
#include <dal/ca8277b/dal_rt_ca8277b_ponmisc.h>

#include "cortina-api/include/onu.h"
#include <aal_pon.h>
#include <aal_l2_te.h>
#include <aal_psds.h>
#include <aal_xgpon.h>
#include <aal_gpon.h>
#include <aal_epon_mac.h>
#include <aal_puc.h>
#include <aal_l3_te.h>
#include <soc/cortina/ca8277b_registers.h>
#include <cortina-api/include/rate.h>
#include <scfg.h>

/*
 * Symbol Definition
 */
static uint32    ponmac_init = INIT_NOT_COMPLETED;
#define CA8277B_PUC_BP_NUM  (64)

/*
 * Macro Declaration
 */
#define RATE_KBPS_TO_VOQ_RATE_M(_kbps)  ((_kbps)/500)
#define RATE_KBPS_TO_VOQ_RATE_K(_kbps)  ((_kbps)%500)
#define VOQ_RATE_M_K_TO_RATE_KBPS(M,K)  (M*500+K)

#define EGR_RATE_KBPS_TO_VOQ_RATE_M(_kbps)  ((_kbps)/1000)
#define EGR_RATE_KBPS_TO_VOQ_RATE_K(_kbps)  ((_kbps)%1000)
#define VOQ_RATE_M_K_TO_EGR_RATE_KBPS(M,K)  (M*1000+K)

//#define PUC_VOQ_8Q_TCONT_START_IDX      (4)

/* Function Name:
 *      dal_ca8277b_ponmac_init
 * Description:
 *      Configure PON MAC initial settings
 * Input:
 *      None.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
int32
dal_ca8277b_ponmac_init(void)
{
    int32 ret;
    int i;
    uint32 voqId;
    aal_puc_ctrl_cfg_msk_t          puc_ctrl_config_mask; 
    aal_puc_ctrl_cfg_t              puc_ctrl_config;
    aal_puc_btc_cfg_msk_t           puc_btc_mask;
    aal_puc_btc_cfg_t               puc_btc_config;
    int q =0;
    rtk_ponmac_queue_t queue;
    rtk_ponmac_queueCfg_t queueCfg;

    ponmac_init = INIT_COMPLETED;

    if(ONU_PON_MAC_MODE_IGNORED == CA_PON_MODE) /* PONMODE is not ready/set*/
        return RT_ERR_FAILED;

#ifdef CONFIG_BOARD_REALTEK_CA8277_PON_GPIO
    if((ret = dal_ca8277b_gpio_state_set(CONFIG_TX_POWER_GPO_PIN,ENABLED)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    if((ret = dal_ca8277b_gpio_mode_set(CONFIG_TX_POWER_GPO_PIN,GPIO_OUTPUT)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    if((ret = dal_ca8277b_ponmac_txDisableGpio_set(ENABLED)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    if((ret = dal_ca8277b_gpio_state_set(CONFIG_TX_DISABLE_GPIO_PIN,ENABLED)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    if((ret = dal_ca8277b_gpio_mode_set(CONFIG_TX_DISABLE_GPIO_PIN,GPIO_OUTPUT)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    if((ret = dal_ca8277b_ponmac_txPowerDisableGpio_set(DISABLED)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return RT_ERR_FAILED;
    }
#endif

    if(CA_PON_MODE == ONU_PON_MAC_XGPON1 || CA_PON_MODE == ONU_PON_MAC_XGSPON)
    {
        mdelay(10);
        aal_psds_init(0,CA_PON_MODE);
        mdelay(10);
        aal_xgpon_reset(0,3);
    }

    puc_ctrl_config_mask.u32 = 0;
    memset(&puc_ctrl_config, 0, sizeof(aal_puc_ctrl_cfg_t));
    puc_ctrl_config_mask.s.shp_en = 1;
    puc_ctrl_config_mask.s.addtok_en = 1;
    puc_ctrl_config_mask.s.subtok_en = 1;
    puc_ctrl_config_mask.s.rl_addtok_en = 1;
    puc_ctrl_config_mask.s.rl_subtok_en = 1;
    puc_ctrl_config_mask.s.rl_en = 1;
    puc_ctrl_config_mask.s.rlovhd = 1;
    puc_ctrl_config_mask.s.shpovhd = 1;
    puc_ctrl_config_mask.s.agrshpovhd = 1;

    puc_ctrl_config.shp_en = 1;
    puc_ctrl_config.addtok_en = 1;
    puc_ctrl_config.subtok_en = 1;
    puc_ctrl_config.rl_addtok_en = 1; 
    puc_ctrl_config.rl_subtok_en = 1; 
    puc_ctrl_config.rl_en = 1; 
    puc_ctrl_config.rlovhd = 20;
    puc_ctrl_config.shpovhd = 20;
    puc_ctrl_config.agrshpovhd = 20;

    if((ret = aal_puc_ctrl_cfg_set(0, puc_ctrl_config_mask, &puc_ctrl_config)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    puc_btc_mask.u32 = 0;
    memset(&puc_btc_config, 0, sizeof(aal_puc_btc_cfg_t));
    puc_btc_mask.s.schen = 1;
    puc_btc_mask.s.sch2en = 1;
    puc_btc_config.schen = 1;
    puc_btc_config.sch2en = 1;
    if((ret = aal_puc_btc_cfg_set(0, puc_btc_mask, &puc_btc_config)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return RT_ERR_FAILED;
    }


    for(voqId =0;voqId<CA8277B_PUC_BP_NUM;voqId++)  
    {
        ret =  aal_puc_voq_bp_remap_data_set(0, voqId, voqId);
        if (ret != CA_E_OK)
        {
            RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
            return RT_ERR_FAILED;        
        }

    }

    /* VoQ based 32B buffer configuration, system provides up to 4 profile for selection */
    (void)aal_puc_voq_buf_lmt_sel1500_set(0, 0xfea9fea9);
    (void)aal_puc_voq_buf_lmt_sel3116_set(0, 0xfea9fea9);
    (void)aal_puc_voq_buf_lmt_sel4732_set(0, 0xfea9fea9);
    (void)aal_puc_voq_buf_lmt_sel6348_set(0, 0xfea9fea9);

    if(CA_PON_MODE >= ONU_PON_MAC_EPON_1G && CA_PON_MODE <= ONU_PON_MAC_EPON_BI10G)
    {
        /*Enabling logical llid 0 queue 0~7*/
        for(q=0;q<8;q++)
        {
            queue.schedulerId = 0;
            queue.queueId = q;
            queueCfg.cir = RT_RATE_PON_EGR_PIR_CIR_RATE_MAX;
            queueCfg.pir = RT_RATE_PON_EGR_PIR_CIR_RATE_MAX;
            queueCfg.weight = 0;
            queueCfg.egrssDrop  = DISABLED;
            queueCfg.type  = STRICT_PRIORITY;

            if((ret = dal_ca8277b_ponmac_queue_add(&queue,&queueCfg)) != RT_ERR_OK)
            {
                printk("%s %d : Can't adding llid 0 queue %d\n",__FUNCTION__,__LINE__,q);
            }
        }
    }

#ifdef CONFIG_BOARD_REALTEK_CA8277_PON_GPIO
    if(CA_PON_MODE_IS_EPON_FAMILY(CA_PON_MODE))
    {
    	mdelay(10);
        if((ret = dal_ca8277b_gpio_databit_set(CONFIG_TX_DISABLE_GPIO_PIN,0)) != CA_E_OK)
        {
            RT_ERR("%s %d dal_ca8277b_gpio_databit_set fail \n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }
#endif

    return RT_ERR_OK;
} /* end of dal_ca8277b_ponmac_init */

/* Function Name:
 *      dal_ca8277b_ponmac_losState_get
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
dal_ca8277b_ponmac_losState_get(rtk_enable_t *pState)
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
}   /* end of dal_ca8277b_ponmac_losState_get */

/* Function Name:
 *      dal_ca8277b_ponmac_serdesCdr_reset
 * Description:
 *      Serdes CDR reset
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
int32
dal_ca8277b_ponmac_serdesCdr_reset(rtk_enable_t *pState)
{
    ca_status_t  ret = CA_E_OK;

    /* check Init status */
    RT_INIT_CHK(ponmac_init);

    switch(CA_PON_MODE) 
	{
		case ONU_PON_MAC_EPON_1G:
		case ONU_PON_MAC_EPON_D10G:
		case ONU_PON_MAC_EPON_BI10G:
			if((ret = aal_epon_reset_all_set(0,0)) != CA_E_OK)
			{
				printk("%s %d epon_mac_reset fail ret=%d\n",__FUNCTION__,__LINE__,ret);
				RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
				return RT_ERR_FAILED;
			}
		break;
		case ONU_PON_MAC_GPON:
		case ONU_PON_MAC_XGPON1:
		case ONU_PON_MAC_XGSPON:
		case ONU_PON_MAC_NGPON2:
		break;
		default:
			return RT_ERR_FAILED;
	}
	
    return RT_ERR_OK;
}   /* end of dal_ca8277b_ponmac_serdesCdr_reset */

/* Function Name:
 *      dal_ca8277b_ponmac_linkState_get
 * Description:
 *      check SD and Sync state of GPON/EPON
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
dal_ca8277b_ponmac_linkState_get(rtk_ponmac_mode_t mode, uint32 *pSd, uint32 *pSync)
{
    ca_status_t  ret = CA_E_OK;
    GLOBAL_OPT_MODULE_STATUS_t opt_stas;
	ca_uint32_t                alarm_bits, state;

    /* check Init status */
    RT_INIT_CHK(ponmac_init);

    //opt_stas.wrd = AAL_GLB_REG_READ(GLOBAL_OPT_MODULE_STATUS);

    if((ret = ca_onu_losState_get(0, &opt_stas)) == CA_E_OK)
    {
        if((0 == opt_stas.bf.opt_rx_los) && (0 == opt_stas.bf.opt_mod_abs))
            *pSd = 1;
        else
            *pSd = 0;
    }
	else
	{
		printk("%s %d losState_get fail ret=%d\n",__FUNCTION__,__LINE__,ret);
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
		return RT_ERR_FAILED;
	}
	
	switch(CA_PON_MODE) 
	{
		case ONU_PON_MAC_EPON_1G:
			if((ret = aal_epon_mac_1g_link_status_get(0, 0, pSync)) != CA_E_OK)
			{
				printk("%s %d epon_mac_1g_link_get fail ret=%d\n",__FUNCTION__,__LINE__,ret);
				RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
				return RT_ERR_FAILED;
			}
		break;
		case ONU_PON_MAC_EPON_D10G:
		case ONU_PON_MAC_EPON_BI10G:
			if((ret = aal_epon_mac_xg_link_status_get(0, 0, pSync)) != CA_E_OK)
			{
				printk("%s %d epon_mac_xg_link_get fail ret=%d\n",__FUNCTION__,__LINE__,ret);
				RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
				return RT_ERR_FAILED;
			}
		break;
		case ONU_PON_MAC_GPON:
			if((ret = aal_gpon_current_alarms_get(0, &alarm_bits)) != CA_E_OK)
			{
				printk("%s %d gpon_alarm_get fail ret=%d\n",__FUNCTION__,__LINE__,ret);
				RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
				return RT_ERR_FAILED;
			}
			if((alarm_bits&AAL_GPON_ALARM_LOF) != 0)
				*pSync = 0;
			else
				*pSync = 1;
		break;
		case ONU_PON_MAC_XGPON1:
		case ONU_PON_MAC_XGSPON:
		case ONU_PON_MAC_NGPON2:
			if((ret = aal_xgpon_dsfp_sync_state_get(0, &state)) != CA_E_OK)
			{
				printk("%s %d xgpon_mac_sync_get fail ret=%d\n",__FUNCTION__,__LINE__,ret);
				RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
				return RT_ERR_FAILED;
			}
			if(state > 1)
				*pSync = 1;
			else 
				*pSync = 0;
		break;
		default:
			return RT_ERR_FAILED;
	}
	
    return RT_ERR_OK;
}   /* end of dal_ca8277b_ponmac_linkState_get */

/* Function Name:
 *      dal_ca8277b_ponmac_queue_del
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
dal_ca8277b_ponmac_queue_del(rtk_ponmac_queue_t *pQueue)
{
    int32   ret;
    ca_uint32_t voq_mode=0,voq_offest = 0;
    aal_puc_queue_cfg_msk_t puc_queue_cfg_msk;
    aal_puc_queue_cfg_t puc_queue_cfg;
    aal_puc_voq_tbc_mem_cfg_msk_t puc_voq_tbc_cfg_msk;
    aal_puc_voq_tbc_mem_cfg_t puc_voq_tbc_cfg;
    ca_uint32_t puc_voq_idx = 0;
    ca_uint32_t l3te_voq_idx = 0;
    aal_l3_te_shaper_tbc_cfg_msk_t l3_te_shp_tbc_cfg_mask;
    aal_l3_te_shaper_tbc_cfg_t l3_te_shp_tbc_cfg;
    uint16 phySchId = 0;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(ponmac_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pQueue), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pQueue->schedulerId >= HAL_MAX_NUM_OF_GPON_TCONT()), RT_ERR_INPUT);
    RT_PARAM_CHK((pQueue->queueId >= CA8277B_TCONT_QUEUE_MAX), RT_ERR_INPUT);

    if((ret = aal_puc_voq_map_cfg_get(0, &voq_mode)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    if(voq_mode == 0)
    {
        voq_offest = 8;
    }
    else if(voq_mode == 1)
    {
        voq_offest = 4;
    }
    else
    {
        printk("%s %d not support voq mode=%d\n",__FUNCTION__,__LINE__,voq_mode);
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    if(pQueue->queueId >= voq_offest)
    {
        printk("%s %d not support voq mode=%d queueId=%d\n",__FUNCTION__,__LINE__,voq_mode,pQueue->queueId);
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    if((ret = dal_rt_ca8277b_ponmisc_tcont_llid_logic2phy_get(pQueue->schedulerId, &phySchId) != RT_ERR_OK))
    {
        printk("%s %d get phy schedler ID:%u fail\n",__FUNCTION__,__LINE__,pQueue->schedulerId);
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    puc_voq_idx = phySchId*voq_offest+pQueue->queueId;
    l3te_voq_idx =phySchId*voq_offest+pQueue->queueId+64;


    puc_voq_tbc_cfg_msk.u32 = 0;
    memset(&puc_voq_tbc_cfg,0, sizeof(aal_puc_voq_tbc_mem_cfg_t));
    puc_voq_tbc_cfg_msk.s.enb = 1;
    puc_voq_tbc_cfg.enb = 1;
    aal_puc_rl_voq_tbc_mem_set(0, puc_voq_idx, puc_voq_tbc_cfg_msk, &puc_voq_tbc_cfg);

    puc_queue_cfg_msk.u32 = 0;
    memset(&puc_queue_cfg, 0, sizeof(aal_puc_queue_cfg_t));

    puc_queue_cfg_msk.s.queue_ena = 1;
    puc_queue_cfg.queue_ena = 0;
    if((ret = aal_puc_port_voq_map_set(0, phySchId, pQueue->queueId, puc_queue_cfg_msk, &puc_queue_cfg)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

#ifdef RTK_VOQ_VALID_SUPPORT
    if((ret = aal_puc_voq_valid_set(0, puc_voq_idx ,0)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return RT_ERR_FAILED;
    }
#endif

    if((ret = aal_puc_voq_flush_by_idx(0, phySchId, pQueue->queueId, pQueue->queueId)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    if(puc_voq_idx < 64 && voq_offest == 8)/*Use L3 TE shapper, only for PUC 8 queue mode*/
    {
        l3_te_shp_tbc_cfg_mask.s.state = 1;   
        l3_te_shp_tbc_cfg.state = CA_AAL_L3_SHAPER_ADMIN_STATE_FORWARD;

        if((ret = aal_l3_te_shaper_voq_tbc_set(0, l3te_voq_idx, l3_te_shp_tbc_cfg_mask, &l3_te_shp_tbc_cfg)) != CA_E_OK)
        {
            RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
            return RT_ERR_FAILED;
        }

    }
    
    return RT_ERR_OK;
} /* end of dal_ca8277b_ponmac_queue_del */

/* Function Name:
 *      dal_ca8277b_ponmac_queue_get
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
dal_ca8277b_ponmac_queue_get(rtk_ponmac_queue_t *pQueue, rtk_ponmac_queueCfg_t *pQueueCfg)
{
    int32   ret;
    ca_uint32_t voq_mode=0,voq_offest = 0;
    aal_puc_voq_tbc_mem_cfg_t puc_rl_voq_tbc_cfg;
    aal_puc_voq_tbc_mem_cfg_t puc_voq_tbc_cfg;
    aal_puc_queue_cfg_t puc_queue_cfg;
    ca_uint32_t puc_voq_idx = 0;
    ca_uint32_t l3te_voq_idx = 0;
    aal_l3_te_shaper_tbc_cfg_t l3_te_shp_tbc_cfg;
    uint16 phySchId = 0;
    
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(ponmac_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pQueue), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pQueueCfg), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pQueue->schedulerId >= HAL_MAX_NUM_OF_GPON_TCONT()), RT_ERR_INPUT);
    RT_PARAM_CHK((pQueue->queueId >= CA8277B_TCONT_QUEUE_MAX), RT_ERR_INPUT);

    if((ret = aal_puc_voq_map_cfg_get(0, &voq_mode)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    if(voq_mode == 0)
    {
        voq_offest = 8;
    }
    else if(voq_mode == 1)
    {
        voq_offest = 4;
    }
    else
    {
        printk("%s %d not support voq mode=%d\n",__FUNCTION__,__LINE__,voq_mode);
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    if(pQueue->queueId >= voq_offest)
    {
        printk("%s %d not support voq mode=%d queueId=%d\n",__FUNCTION__,__LINE__,voq_mode,pQueue->queueId);
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    /* function body */
    memset(&puc_rl_voq_tbc_cfg, 0, sizeof(aal_puc_voq_tbc_mem_cfg_t));
    memset(&puc_voq_tbc_cfg, 0, sizeof(aal_puc_voq_tbc_mem_cfg_t));
    memset(&puc_queue_cfg, 0, sizeof(aal_puc_queue_cfg_t));

    if((ret = dal_rt_ca8277b_ponmisc_tcont_llid_logic2phy_get(pQueue->schedulerId, &phySchId) != RT_ERR_OK))
    {
        printk("%s %d get phy schedler ID:%u fail\n",__FUNCTION__,__LINE__,pQueue->schedulerId);
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    puc_voq_idx = phySchId*voq_offest+pQueue->queueId;
    l3te_voq_idx =phySchId*voq_offest+pQueue->queueId+64;
    

    if(puc_voq_idx < 64 && voq_offest == 8) /*Use L3 TE shapper, only for PUC 8 queue mode*/
    {
        if((ret = aal_l3_te_shaper_voq_tbc_get(0, l3te_voq_idx, &l3_te_shp_tbc_cfg)) != CA_E_OK)
         {
             RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
             return RT_ERR_FAILED;
         }


        if(CA_AAL_L3_SHAPER_ADMIN_STATE_FORWARD == l3_te_shp_tbc_cfg.state)
        {
            pQueueCfg->pir = CA8277B_METER_RATE_MAX;
        }
        else
        {
            pQueueCfg->pir = VOQ_RATE_M_K_TO_RATE_KBPS(l3_te_shp_tbc_cfg.rate_m,l3_te_shp_tbc_cfg.rate_k);
        }

    }
    else
    {
    
        if((ret = aal_puc_rl_voq_tbc_mem_get(0, puc_voq_idx, &puc_rl_voq_tbc_cfg)) != CA_E_OK)
        {
            RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
            return RT_ERR_FAILED;
        }

        if(puc_rl_voq_tbc_cfg.enb == 0)
        {
            pQueueCfg->pir = CA8277B_METER_RATE_MAX;
        }
        else
        {
            pQueueCfg->pir = VOQ_RATE_M_K_TO_RATE_KBPS(puc_rl_voq_tbc_cfg.rate_m,puc_rl_voq_tbc_cfg.rate_k);
        }
    }
    if((ret = aal_puc_voq_tbc_mem_get(0, puc_voq_idx, &puc_voq_tbc_cfg)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    if(puc_voq_tbc_cfg.enb == 0)
    {
        pQueueCfg->cir = 0;
    }
    else
    {
        pQueueCfg->cir = VOQ_RATE_M_K_TO_RATE_KBPS(puc_voq_tbc_cfg.rate_m,puc_voq_tbc_cfg.rate_k);
    }

    if((ret = aal_puc_port_voq_map_get(0, phySchId, pQueue->queueId, &puc_queue_cfg)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    if(puc_queue_cfg.queue_ena == 0)
        return RT_ERR_ENTRY_NOTFOUND;

    pQueueCfg->weight = puc_queue_cfg.queue_weight;
    
    if(pQueueCfg->weight !=0)
        pQueueCfg->type = WFQ_WRR_PRIORITY;
    else
        pQueueCfg->type = STRICT_PRIORITY;
    
    return RT_ERR_OK;
} /* end of dal_ca8277b_ponmac_queue_get */

/* Function Name:
 *      dal_ca8277b_ponmac_queue_add
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
dal_ca8277b_ponmac_queue_add(rtk_ponmac_queue_t *pQueue, rtk_ponmac_queueCfg_t *pQueueCfg)
{
    int32   ret;
    ca_uint32_t voq_mode=0,voq_offest = 0;
    aal_puc_voq_tbc_mem_cfg_msk_t puc_rl_voq_tbc_cfg_msk;
    aal_puc_voq_tbc_mem_cfg_t puc_rl_voq_tbc_cfg;
    aal_puc_voq_tbc_mem_cfg_msk_t puc_voq_tbc_cfg_msk;
    aal_puc_voq_tbc_mem_cfg_t puc_voq_tbc_cfg;
    aal_puc_voq_drop_cfg_msk_t voq_drp_mask;
    aal_puc_voq_drop_cfg_t voq_drp;
    aal_puc_queue_cfg_msk_t puc_queue_cfg_msk;
    aal_puc_queue_cfg_t puc_queue_cfg;
    aal_puc_voq_sche_cfg_t voq_sche_config;
    aal_l3_te_shaper_tbc_cfg_msk_t l3_te_shp_tbc_cfg_mask;
    aal_l3_te_shaper_tbc_cfg_t l3_te_shp_tbc_cfg;
    ca_uint32_t tbc = 0;
    ca_uint32_t puc_voq_idx = 0;
    ca_uint32_t l3te_voq_idx = 0;
    uint16 phySchId = 0;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(ponmac_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pQueue), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pQueueCfg), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pQueue->schedulerId >= HAL_MAX_NUM_OF_GPON_TCONT()), RT_ERR_INPUT);
    RT_PARAM_CHK((pQueue->queueId >= CA8277B_TCONT_QUEUE_MAX), RT_ERR_INPUT);

    RT_PARAM_CHK((pQueueCfg->type >= QUEUE_TYPE_END), RT_ERR_INPUT);
    RT_PARAM_CHK((pQueueCfg->egrssDrop >= RTK_ENABLE_END), RT_ERR_INPUT);

//printk("%s %d pQueue->queueId=%d pQueueCfg->type=%d pQueueCfg->cir=%d pQueueCfg->pir=%d\n"
//,__FUNCTION__,__LINE__,pQueue->queueId,pQueueCfg->type,pQueueCfg->cir,pQueueCfg->pir);

    if((ret = aal_puc_voq_map_cfg_get(0, &voq_mode)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    if(voq_mode == 0)
    {
        voq_offest = 8;
    }
    else if(voq_mode == 1)
    {
        voq_offest = 4;
    }
    else
    {
        printk("%s %d not support voq mode=%d\n",__FUNCTION__,__LINE__,voq_mode);
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    if(pQueue->queueId >= voq_offest)
    {
        printk("%s %d not support voq mode=%d queueId=%d\n",__FUNCTION__,__LINE__,voq_mode,pQueue->queueId);
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return RT_ERR_FAILED;
    }


    if((ret = dal_rt_ca8277b_ponmisc_tcont_llid_logic2phy_get(pQueue->schedulerId, &phySchId) != RT_ERR_OK))
    {
        printk("%s %d get phy schedler ID:%u fail\n",__FUNCTION__,__LINE__,pQueue->schedulerId);
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return RT_ERR_FAILED;
    }



    puc_voq_idx =phySchId*voq_offest+pQueue->queueId;
    l3te_voq_idx =phySchId*voq_offest+pQueue->queueId+64;

    /*Disable queue drop*/
    voq_drp_mask.u32      = 0;
    memset(&voq_drp, 0, sizeof(voq_drp));

    voq_drp_mask.s.voqid  = 1;
    voq_drp_mask.s.enable = 1;
    voq_drp.enable = 0;
    voq_drp.voqid = puc_voq_idx;
    if((ret = aal_puc_voq_drop_set(0, voq_drp_mask, &voq_drp)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    /*Set Tcont Pri queue mapping*/
    puc_queue_cfg_msk.u32 = 0;
    memset(&puc_queue_cfg, 0, sizeof(aal_puc_queue_cfg_t));

    puc_queue_cfg_msk.u32 = ~0U;
    puc_queue_cfg.queue_id = puc_voq_idx;
    puc_queue_cfg.queue_weight = pQueueCfg->weight;
    puc_queue_cfg.queue_pri = pQueue->queueId;
    puc_queue_cfg.queue_ena = 1;
    if((ret = aal_puc_port_voq_map_set(0, phySchId, pQueue->queueId, puc_queue_cfg_msk, &puc_queue_cfg)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    /*Set voq sche table */
    memset(&voq_sche_config, 0, sizeof(aal_puc_voq_sche_cfg_t));

    if((ret = aal_puc_voq_sche_cfg_get(0,&voq_sche_config)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    if(voq_sche_config.basesel == 0)
        tbc = pQueueCfg->weight * 128;
    if(voq_sche_config.basesel == 1)
        tbc = pQueueCfg->weight * 256;
    if(voq_sche_config.basesel == 2)
        tbc = pQueueCfg->weight * 512;
    if(voq_sche_config.basesel == 3)
        tbc = pQueueCfg->weight * 1024;

    if((ret = aal_puc_port_voq_sche_info_tbl_set(0, phySchId,pQueue->queueId,tbc)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    /*Set PIR */
    if(puc_voq_idx < 64 && voq_offest==8)/*For 8 VoQ mode, Use L3 TE shapper*/
    {
        puc_rl_voq_tbc_cfg_msk.u32 = 0;
        memset(&puc_rl_voq_tbc_cfg,0, sizeof(aal_puc_voq_tbc_mem_cfg_t));
        puc_rl_voq_tbc_cfg_msk.s.enb = 1;
        puc_rl_voq_tbc_cfg.enb = 0;

        if((ret = aal_puc_rl_voq_tbc_mem_set(0, puc_voq_idx, puc_rl_voq_tbc_cfg_msk, &puc_rl_voq_tbc_cfg)) != CA_E_OK)
        {
            RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
            return RT_ERR_FAILED;
        }

        l3_te_shp_tbc_cfg_mask.u32 = 0;
        memset(&l3_te_shp_tbc_cfg, 0, sizeof(l3_te_shp_tbc_cfg));
        
        if(pQueueCfg->pir >= CA8277B_METER_RATE_MAX)
        {        
            l3_te_shp_tbc_cfg.rate_k = 0;
            l3_te_shp_tbc_cfg.rate_m = 0;
            l3_te_shp_tbc_cfg.bs = 5;
            l3_te_shp_tbc_cfg.mode = CA_AAL_L3_SHAPER_MODE_BPS;
            l3_te_shp_tbc_cfg.pkt_mode = CA_AAL_L3_SHAPER_TBC_PKT_MODE_CLASS0;
            l3_te_shp_tbc_cfg.tbc_count = 0;
            l3_te_shp_tbc_cfg.state = CA_AAL_L3_SHAPER_ADMIN_STATE_FORWARD;
        }
        else 
        {

            l3_te_shp_tbc_cfg.rate_k = RATE_KBPS_TO_VOQ_RATE_K(pQueueCfg->pir);
            l3_te_shp_tbc_cfg.rate_m = RATE_KBPS_TO_VOQ_RATE_M(pQueueCfg->pir);
            if(l3_te_shp_tbc_cfg.rate_m <50)
                l3_te_shp_tbc_cfg.bs = 5;
            else if(l3_te_shp_tbc_cfg.rate_m <100)
                l3_te_shp_tbc_cfg.bs = 10;
            else if(l3_te_shp_tbc_cfg.rate_m <1000)
                l3_te_shp_tbc_cfg.bs = 100;
            else
                l3_te_shp_tbc_cfg.bs = 1000;
            l3_te_shp_tbc_cfg.mode = CA_AAL_L3_SHAPER_MODE_BPS;
            l3_te_shp_tbc_cfg.pkt_mode = CA_AAL_L3_SHAPER_TBC_PKT_MODE_CLASS0;
            l3_te_shp_tbc_cfg.tbc_count = 0;
            l3_te_shp_tbc_cfg.state = CA_AAL_L3_SHAPER_ADMIN_STATE_SHAPER;

        }


        l3_te_shp_tbc_cfg_mask.s.rate_k = 1;
        l3_te_shp_tbc_cfg_mask.s.rate_m = 1;
        l3_te_shp_tbc_cfg_mask.s.bs = 1;
        l3_te_shp_tbc_cfg_mask.s.state = 1;
        l3_te_shp_tbc_cfg_mask.s.mode = 1;
        l3_te_shp_tbc_cfg_mask.s.tbc_count = 1;
        l3_te_shp_tbc_cfg_mask.s.pkt_mode = 1;

        if((ret = aal_l3_te_shaper_voq_tbc_set(0, l3te_voq_idx, l3_te_shp_tbc_cfg_mask, &l3_te_shp_tbc_cfg)) != CA_E_OK)
        {
            RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
            return RT_ERR_FAILED;
        }

    }
    else
    {
        puc_rl_voq_tbc_cfg_msk.u32 = 0;
        memset(&puc_rl_voq_tbc_cfg, 0, sizeof(aal_puc_voq_tbc_mem_cfg_t));
        
        if(pQueueCfg->pir >= CA8277B_METER_RATE_MAX)
        {
            puc_rl_voq_tbc_cfg.rate_k = 0;
            puc_rl_voq_tbc_cfg.rate_m = 0;
            puc_rl_voq_tbc_cfg.bs = 0;
            puc_rl_voq_tbc_cfg.mode = 0;
            puc_rl_voq_tbc_cfg.pkt_mode_class_sel = 0;
            puc_rl_voq_tbc_cfg.tbc = 0;
            puc_rl_voq_tbc_cfg.enb = 0x0;
        }
        else
        {
            puc_rl_voq_tbc_cfg.rate_k = RATE_KBPS_TO_VOQ_RATE_K(pQueueCfg->pir);
            puc_rl_voq_tbc_cfg.rate_m = RATE_KBPS_TO_VOQ_RATE_M(pQueueCfg->pir);
            if(puc_rl_voq_tbc_cfg.rate_m <50)
                puc_rl_voq_tbc_cfg.bs = 5;
            else if(puc_rl_voq_tbc_cfg.rate_m <100)
                puc_rl_voq_tbc_cfg.bs = 10;
            else if(puc_rl_voq_tbc_cfg.rate_m <1000)
                puc_rl_voq_tbc_cfg.bs = 100;
            else
                puc_rl_voq_tbc_cfg.bs = 1000;
            puc_rl_voq_tbc_cfg.mode = 0;
            puc_rl_voq_tbc_cfg.pkt_mode_class_sel = 0;
            puc_rl_voq_tbc_cfg.tbc = 0;
            puc_rl_voq_tbc_cfg.enb = 0x2;
        }

        puc_rl_voq_tbc_cfg_msk.s.rate_k = 1;
        puc_rl_voq_tbc_cfg_msk.s.rate_m = 1;
        puc_rl_voq_tbc_cfg_msk.s.bs = 1;
        puc_rl_voq_tbc_cfg_msk.s.enb = 1;
        puc_rl_voq_tbc_cfg_msk.s.mode = 1;
        puc_rl_voq_tbc_cfg_msk.s.tbc = 1;
        puc_rl_voq_tbc_cfg_msk.s.pkt_mode_class_sel = 1;

        if((ret = aal_puc_rl_voq_tbc_mem_set(0, puc_voq_idx, puc_rl_voq_tbc_cfg_msk, &puc_rl_voq_tbc_cfg)) != CA_E_OK)
        {
            RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
            return RT_ERR_FAILED;
        }
    }

    /*Set CIR */
    puc_voq_tbc_cfg_msk.u32 = 0;
    memset(&puc_voq_tbc_cfg, 0, sizeof(aal_puc_voq_tbc_mem_cfg_t));

    if(pQueueCfg->cir >= CA8277B_METER_RATE_MAX)
    {
        puc_voq_tbc_cfg.rate_k = 0;
        puc_voq_tbc_cfg.rate_m = 0;
        puc_voq_tbc_cfg.bs = 5;
        puc_voq_tbc_cfg.mode = 0;
        puc_voq_tbc_cfg.pkt_mode_class_sel = 0;
        puc_voq_tbc_cfg.tbc = 0;
        puc_voq_tbc_cfg.enb = 0x2;
    }
    else
    {
        puc_voq_tbc_cfg.rate_k = RATE_KBPS_TO_VOQ_RATE_K(pQueueCfg->cir);
        puc_voq_tbc_cfg.rate_m = RATE_KBPS_TO_VOQ_RATE_M(pQueueCfg->cir);
        if(puc_voq_tbc_cfg.rate_m <50)
            puc_voq_tbc_cfg.bs = 5;
        else if(puc_voq_tbc_cfg.rate_m <100)
            puc_voq_tbc_cfg.bs = 10;
        else if(puc_voq_tbc_cfg.rate_m <1000)
            puc_voq_tbc_cfg.bs = 100;
        else
            puc_voq_tbc_cfg.bs = 1000;
        puc_voq_tbc_cfg.mode = 0;
        puc_voq_tbc_cfg.pkt_mode_class_sel = 0;
        puc_voq_tbc_cfg.tbc = 0;
        puc_voq_tbc_cfg.enb = 0x2;
    }

    puc_voq_tbc_cfg_msk.s.rate_k = 1;
    puc_voq_tbc_cfg_msk.s.rate_m = 1;
    puc_voq_tbc_cfg_msk.s.bs = 1;
    puc_voq_tbc_cfg_msk.s.enb = 1;
    puc_voq_tbc_cfg_msk.s.mode = 1;
    puc_voq_tbc_cfg_msk.s.tbc = 1;
    puc_voq_tbc_cfg_msk.s.pkt_mode_class_sel = 1;

    if((ret = aal_puc_voq_tbc_mem_set(0, puc_voq_idx, puc_voq_tbc_cfg_msk, &puc_voq_tbc_cfg)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

#ifdef RTK_VOQ_VALID_SUPPORT
    if((ret = aal_puc_voq_valid_set(0, puc_voq_idx ,1)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return RT_ERR_FAILED;
    }
#endif

    return RT_ERR_OK;
} /* end of dal_ca8277b_ponmac_queue_add */

/* Function Name:
 *      dal_ca8277b_ponmac_egrBandwidthCtrlRate_get
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
dal_ca8277b_ponmac_egrBandwidthCtrlRate_get(uint32 *pRate)
{
    int32 ret;
    aal_puc_agr_shp_ctrl_cfg_t puc_agr_shp_ctrl_cfg;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC), "");

    /* check Init status */
    RT_INIT_CHK(ponmac_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pRate), RT_ERR_NULL_POINTER);

    /* function body */
    memset(&puc_agr_shp_ctrl_cfg,0,sizeof(aal_puc_agr_shp_ctrl_cfg_t));

    if((ret = aal_puc_agr_shp_ctrl_get(0, &puc_agr_shp_ctrl_cfg)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    if(puc_agr_shp_ctrl_cfg.enb == 0)
    {
        *pRate = CA8277B_METER_RATE_MAX;
    }
    else
    {
        *pRate = VOQ_RATE_M_K_TO_EGR_RATE_KBPS(puc_agr_shp_ctrl_cfg.rate_m,puc_agr_shp_ctrl_cfg.rate_k);
    }

    return RT_ERR_OK;
}   /* end of dal_ca8277b_ponmac_egrBandwidthCtrlRate_get */

/* Function Name:
 *      dal_ca8277b_ponmac_egrBandwidthCtrlRate_set
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
 *      (2) The unit of granularity in ca8277b is 1Kbps.
 */
int32
dal_ca8277b_ponmac_egrBandwidthCtrlRate_set(uint32 rate)
{
    int32 ret;
    aal_puc_agr_shp_ctrl_cfg_t puc_agr_shp_ctrl_cfg;
    
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC), "rate=%d",rate);

    /* check Init status */
    RT_INIT_CHK(ponmac_init);

    /* parameter check */
    RT_PARAM_CHK((CA8277B_PON_EGRESS_RATE_MAX < rate), RT_ERR_INPUT);

    /* function body */
    memset(&puc_agr_shp_ctrl_cfg,0,sizeof(aal_puc_agr_shp_ctrl_cfg_t));

    if(rate >= CA8277B_METER_RATE_MAX)
    {
        puc_agr_shp_ctrl_cfg.rate_k = 0;
        puc_agr_shp_ctrl_cfg.rate_m = 0;
        puc_agr_shp_ctrl_cfg.cbs = 0;
        puc_agr_shp_ctrl_cfg.enb = 0;
    }
    else
    {
        puc_agr_shp_ctrl_cfg.rate_k = EGR_RATE_KBPS_TO_VOQ_RATE_K(rate);
        puc_agr_shp_ctrl_cfg.rate_m = EGR_RATE_KBPS_TO_VOQ_RATE_M(rate);
        if(puc_agr_shp_ctrl_cfg.rate_m <50)
            puc_agr_shp_ctrl_cfg.cbs = 5;
        else if(puc_agr_shp_ctrl_cfg.rate_m <100)
            puc_agr_shp_ctrl_cfg.cbs = 10;
        else if(puc_agr_shp_ctrl_cfg.rate_m <1000)
            puc_agr_shp_ctrl_cfg.cbs = 100;
        else
            puc_agr_shp_ctrl_cfg.cbs = 1000;
        puc_agr_shp_ctrl_cfg.enb = 2;
    }

    if((ret = aal_puc_agr_shp_ctrl_set(0, &puc_agr_shp_ctrl_cfg)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}   /* end of dal_ca8277b_ponmac_egrBandwidthCtrlRate_set */

/* Function Name:
 *      dal_ca8277b_ponmac_egrBandwidthCtrlIncludeIfg_get
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
dal_ca8277b_ponmac_egrBandwidthCtrlIncludeIfg_get(rtk_enable_t *pIfgInclude)
{
    int32 ret;
    aal_puc_ctrl_cfg_t puc_ctrl_config;

    /* check Init status */
    RT_INIT_CHK(ponmac_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIfgInclude), RT_ERR_NULL_POINTER);

    /* function body */
    if((ret = aal_puc_ctrl_cfg_get(0, &puc_ctrl_config)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    if(puc_ctrl_config.agrshpovhd == 0)
        *pIfgInclude = DISABLED;
    else
        *pIfgInclude = ENABLED;

    return RT_ERR_OK;
}   /* end of dal_ca8277b_ponmac_egrBandwidthCtrlIncludeIfg_get */


/* Function Name:
 *      dal_ca8277b_ponmac_egrBandwidthCtrlIncludeIfg_set
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
dal_ca8277b_ponmac_egrBandwidthCtrlIncludeIfg_set(rtk_enable_t ifgInclude)
{
    int32 ret;
    aal_puc_ctrl_cfg_msk_t puc_ctrl_config_mask; 
    aal_puc_ctrl_cfg_t puc_ctrl_config;

    /* check Init status */
    RT_INIT_CHK(ponmac_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= ifgInclude), RT_ERR_INPUT);

    /* function body */
    puc_ctrl_config_mask.u32 = 0;
    puc_ctrl_config_mask.s.agrshpovhd =1;
    memset(&puc_ctrl_config,0,sizeof(aal_puc_ctrl_cfg_t));
    if(ifgInclude == ENABLED)
        puc_ctrl_config.agrshpovhd = 20;
    else
        puc_ctrl_config.agrshpovhd = 0;
    if((ret = aal_puc_ctrl_cfg_set(0, puc_ctrl_config_mask, &puc_ctrl_config)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}   /* end of dal_ca8277b_ponmac_egrBandwidthCtrlIncludeIfg_set */

/* Function Name:
 *      dal_ca8277b_ponmac_txDisableGpio_get
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
dal_ca8277b_ponmac_txDisableGpio_get(rtk_enable_t *pEnable)
{
    int32 ret;
    uint32 data;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC), "");

    /* check Init status */
    RT_INIT_CHK(ponmac_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
#ifdef CONFIG_BOARD_REALTEK_CA8277_PON_GPIO
    if((ret = dal_ca8277b_gpio_databit_get(CONFIG_TX_DISABLE_GPIO_PIN,&data)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    if(data == 1)
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;
#else
    *pEnable = DISABLED;
    return RT_ERR_CHIP_NOT_SUPPORTED;
#endif

    return RT_ERR_OK;
}   /* end of dal_ca8277b_ponmac_txDisableGpio_get */

/* Function Name:
 *      dal_ca8277b_ponmac_txDisableGpio_set
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
dal_ca8277b_ponmac_txDisableGpio_set(rtk_enable_t enable)
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

#ifdef CONFIG_BOARD_REALTEK_CA8277_PON_GPIO
    if((ret = dal_ca8277b_gpio_databit_set(CONFIG_TX_DISABLE_GPIO_PIN,data)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return RT_ERR_FAILED;
    }
#else
    return RT_ERR_CHIP_NOT_SUPPORTED;
#endif

    return RT_ERR_OK;
}   /* end of dal_ca8277b_ponmac_txDisableGpio_set */

/* Function Name:
 *      dal_ca8277b_ponmac_txPowerDisableGpio_get
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
dal_ca8277b_ponmac_txPowerDisableGpio_get(rtk_enable_t *pEnable)
{
    int32 ret;
    uint32 data;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC), "");

    /* check Init status */
    RT_INIT_CHK(ponmac_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

#ifdef CONFIG_BOARD_REALTEK_CA8277_PON_GPIO
    /* function body */
    if((ret = dal_ca8277b_gpio_databit_get(CONFIG_TX_POWER_GPO_PIN,&data)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    if(data == 1)
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;
#else
    *pEnable = DISABLED;
    return RT_ERR_CHIP_NOT_SUPPORTED;
#endif
    return RT_ERR_OK;
}   /* end of dal_ca8277b_ponmac_txPowerDisableGpio_get */

/* Function Name:
 *      dal_ca8277b_ponmac_txPowerDisableGpio_set
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
dal_ca8277b_ponmac_txPowerDisableGpio_set(rtk_enable_t enable)
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

#ifdef CONFIG_BOARD_REALTEK_CA8277_PON_GPIO
    if((ret = dal_ca8277b_gpio_databit_set(CONFIG_TX_POWER_GPO_PIN,data)) != CA_E_OK)
    {
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return RT_ERR_FAILED;
    }
#else
    return RT_ERR_CHIP_NOT_SUPPORTED;
#endif

    return RT_ERR_OK;
}   /* end of dal_ca8277b_ponmac_txPowerDisableGpio_set */