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
#include <hal/chipdef/rtl8277c/rtl8277c_def.h>
#include <dal/rtl8277c/dal_rtl8277c.h>
#include <dal/rtl8277c/dal_rtl8277c_ponmac.h>
#include <dal/rtl8277c/dal_rtl8277c_qos.h>
#include <dal/rtl8277c/dal_rtl8277c_gpio.h>
#include <dal/rtl8277c/dal_rt_rtl8277c_ponmisc.h>


#include "cortina-api/include/onu.h"
#include <aal_pon.h>
#include <aal_l2_te.h>
#include <aal_psds.h>
#include <aal_xgpon.h>
#include <aal_gpon.h>
#include <aal_epon_mac.h>
#include <aal_puc.h>
#include <soc/cortina/rtl8277c_registers.h>
//#include <cortina-api/include/rate.h>
#include <aal_port.h>
#include <scfg.h>
#include <aal_l3_tm.h>
#include <aal_l3_te_cb.h>
#include <osal_spinlock.h>
#include <aal_common.h>
#include <aal_arb.h>
#include <aal_port.h>

/*
 * Symbol Definition
 */
static uint32    ponmac_init = INIT_NOT_COMPLETED;
#define RTL8277C_PUC_BP_NUM  (AAL_PUC_QUEUE_BP_NUM)
#if defined(CONFIG_EPON_FEATURE)
extern uint32 epon_init;
#endif
#if defined(CONFIG_GPON_FEATURE)
extern uint32 gpon_init;
#endif

/*
 * Macro Declaration
 */
#define RATE_KBPS_TO_VOQ_RATE_M(_kbps)  ((_kbps)/500)
#define RATE_KBPS_TO_VOQ_RATE_K(_kbps)  ((_kbps)%500)
#define VOQ_RATE_M_K_TO_RATE_KBPS(M,K)  (M*500+K)

#define EGR_RATE_KBPS_TO_VOQ_RATE_M(_kbps)  ((_kbps)/1000)
#define EGR_RATE_KBPS_TO_VOQ_RATE_K(_kbps)  ((_kbps)%1000)
#define VOQ_RATE_M_K_TO_EGR_RATE_KBPS(M,K)  (M*1000+K)

#define QM_PORT_COS_TO_VOQ(_port,_cos)  ((_port)*8+_cos) 

extern uint32 ca_rtk_debug;

#ifdef CONFIG_CORTINA_BOARD_FPGA
#include <aal_init.h>
#define RTL8277C_LOG_ERR(fmt, arg...)  printk("[rtl8277c]:\t" fmt "\n",## arg)
#define RTL8277C_LOG_DBG(fmt, arg...)  printk("[rtl8277c]:\t" fmt "\n",## arg) 
#else
#define RTL8277C_LOG_ERR(fmt, arg...)  printk(KERN_ERR "[rtl8277c]:\t" fmt "\n",## arg)
#define RTL8277C_LOG_DBG(fmt, arg...)  if(ca_rtk_debug & 0x400) {printk(KERN_DEBUG "[rtl8277c]:\t" fmt "\n",## arg);}
#endif

#define PON_QM_DIS_ENQUEUE_PROFILE_NUM 7

/* use device_tree to define pon_gpio for customer request reduce image versions */
#ifdef CONFIG_BOARD_REALTEK_CA8277_PON_GPIO
extern uint32 tx_disable_gpio;
#ifdef CONFIG_TRX_SWITCH_GPIO_PIN
uint32 trx_switch_gpio=0xFF;
#endif
#ifdef CONFIG_TX_POWER_TURN_ON_FEATURE
extern uint32 tx_power_gpio;
#endif
#endif

extern ca_uint g_aal_gpon_flow_lock;

/* Function Name:
 *      dal_rtl8277c_ponmac_init
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
dal_rtl8277c_ponmac_init(void)
{
    int32 ret;
    int i;
    uint32 voqId;
    aal_puc_ctrl_cfg_msk_t          puc_ctrl_config_mask; 
    aal_puc_ctrl_cfg_t              puc_ctrl_config;
    aal_puc_btc_cfg_msk_t           puc_btc_mask;
    aal_puc_btc_cfg_t               puc_btc_config;
    ca_uint32_t tqmvoqid,qmVoqid;
    ca_uint32_t port_id,cos;
    ca_boolean_t status;
//RTK_VOQ_VALID_SUPPORT
    aal_puc_cfg_msk_t puccfg_mask;
    aal_puc_cfg_t puccfg;
//RTK_VOQ_VALID_SUPPORT
    aal_l3_te_cb_comb_threshold_mask_t l3_te_thres_mask;
    aal_l3_te_cb_comb_threshold_t l3_te_thres_cfg;
    L3FE_GLB_GLB_CFG_t l3fe_glb_cfg;
    ca_uint64_t bmp = 0;

    ponmac_init = INIT_COMPLETED;

#ifdef CONFIG_CORTINA_BOARD_FPGA
    printk("%s %d FPGA ponmac init\n",__FUNCTION__,__LINE__);
#endif

    if(ONU_PON_MAC_MODE_IGNORED == CA_PON_MODE) /* PONMODE is not ready/set*/
        return RT_ERR_FAILED;

#ifdef CONFIG_BOARD_REALTEK_CA8277_PON_GPIO
#ifdef CONFIG_TX_POWER_TURN_ON_FEATURE
    if((ret = dal_rtl8277c_gpio_state_set(tx_power_gpio,ENABLED)) != CA_E_OK)
    {
        RTL8277C_LOG_ERR("%s %d dal_rtl8277c_gpio_state_set fail \n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if((ret = dal_rtl8277c_gpio_mode_set(tx_power_gpio,GPIO_OUTPUT)) != CA_E_OK)
    {
        RTL8277C_LOG_ERR("%s %d dal_rtl8277c_gpio_mode_set fail \n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
#endif

    if((ret = dal_rtl8277c_ponmac_txDisableGpio_set(ENABLED)) != CA_E_OK)
    {
        RTL8277C_LOG_ERR("%s %d dal_rtl8277c_ponmac_txDisableGpio_set fail \n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if((ret = dal_rtl8277c_gpio_state_set(tx_disable_gpio,ENABLED)) != CA_E_OK)
    {
        RTL8277C_LOG_ERR("%s %d dal_rtl8277c_gpio_state_set fail \n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if((ret = dal_rtl8277c_gpio_mode_set(tx_disable_gpio,GPIO_OUTPUT)) != CA_E_OK)
    {
        RTL8277C_LOG_ERR("%s %d dal_rtl8277c_gpio_mode_set fail \n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

#ifdef CONFIG_TX_POWER_TURN_ON_FEATURE
    if((ret = dal_rtl8277c_ponmac_txPowerDisableGpio_set(DISABLED)) != CA_E_OK)
    {
        RTL8277C_LOG_ERR("%s %d dal_rtl8277c_ponmac_txPowerDisableGpio_set fail \n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
#endif
#endif

#if 0 //remove old patch 
    if(CA_PON_MODE == ONU_PON_MAC_XGPON1 || CA_PON_MODE == ONU_PON_MAC_XGSPON)
    {
        mdelay(10);
        aal_psds_init(0,CA_PON_MODE);
        mdelay(10);
        aal_xgpon_reset(0,3);
    }
#endif

    puc_ctrl_config_mask.u32 = 0;
    memset(&puc_ctrl_config, 0, sizeof(aal_puc_ctrl_cfg_t));
    puc_ctrl_config_mask.s.shp_en = 1;
    puc_ctrl_config_mask.s.addtok_en = 1;
    puc_ctrl_config_mask.s.subtok_en = 1;
    puc_ctrl_config_mask.s.pir_addtok_en = 1;
    puc_ctrl_config_mask.s.pir_subtok_en = 1;
    puc_ctrl_config_mask.s.pir_en = 1;
    puc_ctrl_config_mask.s.pirovhd = 1;
    puc_ctrl_config_mask.s.shpovhd = 1;
    puc_ctrl_config_mask.s.agrshpovhd = 1;

    puc_ctrl_config.shp_en = 1;
    puc_ctrl_config.addtok_en = 1;
    puc_ctrl_config.subtok_en = 1;
    puc_ctrl_config.pir_addtok_en = 1; 
    puc_ctrl_config.pir_subtok_en = 1; 
    puc_ctrl_config.pir_en = 1; 
    puc_ctrl_config.pirovhd = 20;
    puc_ctrl_config.shpovhd = 20;
    puc_ctrl_config.agrshpovhd = 20;

    if((ret = aal_puc_ctrl_cfg_set(0, puc_ctrl_config_mask, &puc_ctrl_config)) != CA_E_OK)
    {
        RTL8277C_LOG_ERR("%s %d aal_puc_ctrl_cfg_set fail \n",__FUNCTION__,__LINE__);
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
        RTL8277C_LOG_ERR("%s %d aal_puc_btc_cfg_set fail \n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*For PUC VoQ 0~127 , it map to  64~191 of QM NI (WAN)*/
    for(voqId=0;voqId<128;voqId++)  
    {
        ret =  aal_puc_voq_bp_remap_data_set(0, voqId, voqId+64);
        if (ret != CA_E_OK)
        {
            RTL8277C_LOG_ERR("%s %d aal_puc_voq_bp_remap_data_set fail voqId=%d \n",
                __FUNCTION__, __LINE__,voqId);
            
            return RT_ERR_FAILED;
        }

    }

    /*For PUC VoQ 128~255 , default disable bp to QM    */
    for(voqId=128;voqId<256;voqId++)  
    {
        ret =  aal_puc_voq_bp_remap_data_set(0, voqId, AAL_PUC_QUEUE_BP_INVALID);
        if (ret != CA_E_OK)
        {
            RTL8277C_LOG_ERR("%s %d aal_puc_voq_bp_remap_data_set fail voqId=%d \n",
                __FUNCTION__, __LINE__,voqId);
            
            return RT_ERR_FAILED;
        }

    }

    /*For PUC VoQ 128~192 , it map to 0~63 of QM NI (LAN/WAN share)     */
    for(port_id=0;port_id< CFG_ID_RTK_PHY_PORT_TO_DEEP_Q_LEN; port_id++)
    {
        if(CA_E_OK != aal_port_physical_port_to_dq_get(0, port_id, &status))
        {
            /*Default treat the dq is used for LAN */
            status = 1;
        }

        for(cos=0;cos<8;cos++)
        {            
            qmVoqid = QM_PORT_COS_TO_VOQ(port_id,cos);
            voqId = qmVoqid+128;
            tqmvoqid = (status == 0)? qmVoqid:AAL_PUC_QUEUE_BP_INVALID;
        
            ret =  aal_puc_voq_bp_remap_data_set(0, voqId, tqmvoqid);
            if (ret != CA_E_OK)
            {
                RTL8277C_LOG_ERR("%s %d aal_puc_voq_bp_remap_data_set fail voqId=%d \n",
                    __FUNCTION__, __LINE__,voqId);

                return RT_ERR_FAILED;
            }            
        }
        
    }

//RTK_VOQ_VALID_SUPPORT 
    puccfg_mask.u32 = 0;
    memset(&puccfg, 0, sizeof(aal_puc_cfg_t));
    puccfg_mask.s.invalid_voqdrop_enable = 1;
    puccfg.invalid_voqdrop_enable = 1;
    ret = aal_puc_cfg_set(0, puccfg_mask, &puccfg);
    if (ret != CA_E_OK)
    {
        RTL8277C_LOG_ERR("%s %d aal_puc_cfg_set fail\n",
            __FUNCTION__, __LINE__);
        
        return RT_ERR_FAILED;
    }
//RTK_VOQ_VALID_SUPPORT

    l3_te_thres_mask.u32 = 0;
    memset(&l3_te_thres_cfg, 0, sizeof(aal_l3_te_cb_comb_threshold_t));
    l3_te_thres_mask.s.threshold_lo = 1;
    l3_te_thres_mask.s.threshold_hi = 1;
    l3_te_thres_mask.s.prvt = 1;
    l3_te_thres_cfg.threshold_lo = 0;
    l3_te_thres_cfg.threshold_hi = 0;
    l3_te_thres_cfg.prvt = 0;

    if((ret = aal_l3_te_cb_voq_threshold_profile_set(0,PON_QM_DIS_ENQUEUE_PROFILE_NUM,l3_te_thres_mask,&l3_te_thres_cfg)) != CA_E_OK)
    {
        RTL8277C_LOG_ERR("%s %d aal_l3_te_cb_voq_threshold_profile_set fail\n",
            __FUNCTION__, __LINE__);
        return RT_ERR_FAILED;
    }

    //NI-7
    if(CA_E_OK != aal_port_physical_port_to_dq_get(0, 7, &status))
    {
        /*Default treat the dq is used for LAN */
        status = 1;
    }
    if(status == 1)
    {
        for(voqId=56;voqId<64;voqId++)
        {
            if((ret = aal_l3_te_cb_voq_profile_sel_set(0,voqId+64,PON_QM_DIS_ENQUEUE_PROFILE_NUM)) != CA_E_OK)
            {
                RTL8277C_LOG_ERR("%s %d aal_l3_te_cb_voq_profile_sel_set fail\n",
                    __FUNCTION__, __LINE__);
                return RT_ERR_FAILED;
            }
        }
    }

    if(CA_PON_MODE_IS_GPON_FAMILY(CA_PON_MODE) || CA_PON_MODE_IS_EPON_FAMILY(CA_PON_MODE))
    {
        l3fe_glb_cfg.wrd = CA_NE_REG_READ(L3FE_GLB_GLB_CFG);
        l3fe_glb_cfg.bf.l3fe_port_mmship_chk_en = 1;
        l3fe_glb_cfg.bf.use_o_lspid_for_mmshp_chk = 1;
        CA_NE_REG_WRITE(l3fe_glb_cfg.wrd, L3FE_GLB_GLB_CFG);

        for(port_id=0;port_id<7;port_id++)
        {
            bmp = ~((1ULL<<port_id) | 0x80);
            if((ret = aal_port_mmshp_check_set(0, port_id, bmp))!= CA_E_OK)
            {
                RTL8277C_LOG_ERR("%s %d aal_arb_pdpid_map_set fail\n",
                    __FUNCTION__, __LINE__);
                return RT_ERR_FAILED;
            }
        }

        if((ret = aal_arb_pdpid_map_set(0,0,0,7,0xf))!= CA_E_OK)
        {
            RTL8277C_LOG_ERR("%s %d aal_arb_pdpid_map_set fail\n",
                __FUNCTION__, __LINE__);
            return RT_ERR_FAILED;
        }

        if((ret = aal_arb_pdpid_map_set(0,0,1,7,0xf))!= CA_E_OK)
        {
            RTL8277C_LOG_ERR("%s %d aal_arb_pdpid_map_set fail\n",
                __FUNCTION__, __LINE__);
            return RT_ERR_FAILED;
        }
    }

#ifdef CONFIG_BOARD_REALTEK_CA8277_PON_GPIO
    if(CA_PON_MODE_IS_EPON_FAMILY(CA_PON_MODE))
    {
        mdelay(10);
        if((ret = dal_rtl8277c_ponmac_txDisableGpio_set(DISABLED)) != CA_E_OK)
        {
            RTL8277C_LOG_ERR("%s %d dal_rtl8277c_ponmac_txDisableGpio_set fail \n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }
#endif

    return RT_ERR_OK;
} /* end of dal_rtl8277c_ponmac_init */

/* Function Name:
 *      dal_rtl8277c_ponmac_losState_get
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
dal_rtl8277c_ponmac_losState_get(rtk_enable_t *pState)
{
    ca_status_t  ret = CA_E_OK;
    aal_pon_opt_status_t opt_stas;

    /* check Init status */
    RT_INIT_CHK(ponmac_init);

    if((ret = aal_pon_optical_status_get(0, &opt_stas)) == CA_E_OK)
    {
        if((1 == opt_stas.opt_rx_los) || (1 == opt_stas.opt_mod_abs))
            *pState = ENABLED;
        else
            *pState = DISABLED;
    }

    return ret;
}   /* end of dal_rtl8277c_ponmac_losState_get */

/* Function Name:
 *      dal_rtl8277c_ponmac_serdesCdr_reset
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
dal_rtl8277c_ponmac_serdesCdr_reset(void)
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
}   /* end of dal_rtl8277c_ponmac_serdesCdr_reset */

/* Function Name:
 *      dal_ca8277c_ponmac_linkState_get
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
dal_ca8277c_ponmac_linkState_get(rtk_ponmac_mode_t mode, uint32 *pSd, uint32 *pSync)
{
    ca_status_t  ret = CA_E_OK;
    aal_pon_opt_status_t    opt_stas;
	ca_uint32_t             alarm_bits, state;

    /* check Init status */
    RT_INIT_CHK(ponmac_init);

    //opt_stas.wrd = AAL_GLB_REG_READ(GLOBAL_OPT_MODULE_STATUS);

    if((ret = aal_pon_optical_status_get(0, &opt_stas)) == CA_E_OK)
    {
        if((0 == opt_stas.opt_rx_los) && (0 == opt_stas.opt_mod_abs))
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

#if defined(CONFIG_GPON_FEATURE)
	if( CA_PON_MODE_IS_GPON_FAMILY(CA_PON_MODE) && ( gpon_init==INIT_NOT_COMPLETED ))
	{
		printk("GPON family but not inited \r\n");
		RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
		return RT_ERR_NOT_INIT;
	}
#endif

#if defined(CONFIG_EPON_FEATURE)
	if( CA_PON_MODE_IS_EPON_FAMILY(CA_PON_MODE) && ( epon_init==INIT_NOT_COMPLETED ))
	{
		printk("EPON family but not inited \r\n");
		RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
		return RT_ERR_NOT_INIT;
	}
#endif

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
}   /* end of dal_ca8277c_ponmac_linkState_get */

/* Function Name:
 *      dal_rtl8277c_ponmac_queue_del
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
dal_rtl8277c_ponmac_queue_del(rtk_ponmac_queue_t *pQueue)
{
    int32   ret;
    ca_uint32_t voq_mode=0,voq_offest = 0;
    aal_puc_queue_cfg_msk_t puc_queue_cfg_msk;
    aal_puc_queue_cfg_t puc_queue_cfg;
    aal_puc_voq_tbc_mem_cfg_msk_t puc_voq_tbc_cfg_msk;
    aal_puc_voq_tbc_mem_cfg_t puc_voq_tbc_cfg;
    ca_uint32_t puc_voq_idx = 0;
    uint16 phySchId = 0;
    int32 i;
    uint32 en_que_num;
    ca_boolean_t voq_valid;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC),"%s",__FUNCTION__);
    RTL8277C_LOG_DBG("Q_del schId=%d qId=%d\n",pQueue->schedulerId,pQueue->queueId);

    /* check Init status */
    RT_INIT_CHK(ponmac_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pQueue), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pQueue->schedulerId >= HAL_MAX_NUM_OF_GPON_TCONT()), RT_ERR_INPUT);
    RT_PARAM_CHK((pQueue->queueId >= RTL8277C_TCONT_QUEUE_MAX), RT_ERR_INPUT);

    if((ret = aal_puc_voq_map_cfg_get(0, &voq_mode)) != CA_E_OK)
    {
        RTL8277C_LOG_ERR("%s %d aal_puc_voq_map_cfg_get fail \n",__FUNCTION__,__LINE__);
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
        RTL8277C_LOG_ERR("%s %d not support voq mode=%d\n",__FUNCTION__,__LINE__,voq_mode);
        return RT_ERR_FAILED;
    }

    if(pQueue->queueId >= voq_offest)
    {
        RTL8277C_LOG_ERR("%s %d not support voq mode=%d queueId=%d\n",__FUNCTION__,__LINE__,voq_mode,pQueue->queueId);
        return RT_ERR_FAILED;
    }

    //puc_voq_idx =((pQueue->schedulerId+PUC_VOQ_8Q_TCONT_START_IDX)*voq_offest)+pQueue->queueId;

    if((ret = dal_rt_rtl8277c_ponmisc_tcont_llid_logic2phy_get(pQueue->schedulerId, &phySchId) != RT_ERR_OK))
    {
        printk("%s %d get phy schedler ID:%u fail\n",__FUNCTION__,__LINE__,pQueue->schedulerId);
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    puc_voq_idx = phySchId*voq_offest+pQueue->queueId;

//RTK_VOQ_VALID_SUPPORT
    if (CA_PON_MODE == ONU_PON_MAC_GPON)
    {
        ca_spin_lock(g_aal_gpon_flow_lock);
        if((ret = aal_puc_voq_valid_set(0, puc_voq_idx ,0)) != CA_E_OK)
        {
            ca_spin_unlock(g_aal_gpon_flow_lock);
            RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
            return RT_ERR_FAILED;
        }
        ca_spin_unlock(g_aal_gpon_flow_lock);
    }
    else
    {
        if((ret = aal_puc_voq_valid_set(0, puc_voq_idx ,0)) != CA_E_OK)
        {
            RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
            return RT_ERR_FAILED;
        }
    }
//RTK_VOQ_VALID_SUPPORT
#if 1
    puc_queue_cfg_msk.u32 = 0;
    memset(&puc_queue_cfg, 0, sizeof(aal_puc_queue_cfg_t));

    puc_queue_cfg_msk.s.queue_ena = 1;
    puc_queue_cfg.queue_ena = 0;
    if((ret = aal_puc_port_voq_map_set(0, phySchId, pQueue->queueId, puc_queue_cfg_msk, &puc_queue_cfg)) != CA_E_OK)
    {
        RTL8277C_LOG_ERR("%s %d aal_puc_port_voq_map_set fail phySchId:%u \n",__FUNCTION__,__LINE__, phySchId);
        return RT_ERR_FAILED;
    }
#endif
    if (CA_PON_MODE == ONU_PON_MAC_GPON)
    {
        en_que_num = 0;
        for(i=0; i<voq_offest; i++)
        {
            puc_voq_idx = phySchId*voq_offest+i;
            aal_puc_voq_valid_get(0, puc_voq_idx, &voq_valid);
            if(voq_valid == 1)
                en_que_num++;
        }

        if(en_que_num == 0)
        {
            ca_spin_lock(g_aal_gpon_flow_lock);
            if((ret = aal_puc_voq_flush_by_idx(0, phySchId, 0, 7)) != CA_E_OK)
            {
                ca_spin_unlock(g_aal_gpon_flow_lock);
                RTL8277C_LOG_ERR("%s %d aal_puc_voq_flush_by_idx fail \n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            ca_spin_unlock(g_aal_gpon_flow_lock);
        }
    }
    else
    {
        if((ret = aal_puc_voq_flush_by_idx(0, phySchId, pQueue->queueId, pQueue->queueId)) != CA_E_OK)
        {

            RTL8277C_LOG_ERR("%s %d aal_puc_voq_flush_by_idx fail \n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }

    return RT_ERR_OK;
} /* end of dal_rtl8277c_ponmac_queue_del */

/* Function Name:
 *      dal_rtl8277c_ponmac_queue_get
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
dal_rtl8277c_ponmac_queue_get(rtk_ponmac_queue_t *pQueue, rtk_ponmac_queueCfg_t *pQueueCfg)
{
    int32   ret;
    ca_uint32_t voq_mode=0,voq_offest = 0;
    aal_puc_voq_tbc_mem_cfg_t puc_pir_voq_tbc_cfg;
    aal_puc_voq_tbc_mem_cfg_t puc_voq_tbc_cfg;
    aal_puc_queue_cfg_t puc_queue_cfg;
    ca_uint32_t puc_voq_idx = 0;
    uint16 phySchId = 0;


    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(ponmac_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pQueue), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pQueueCfg), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pQueue->schedulerId >= HAL_MAX_NUM_OF_GPON_TCONT()), RT_ERR_INPUT);
    RT_PARAM_CHK((pQueue->queueId >= RTL8277C_TCONT_QUEUE_MAX), RT_ERR_INPUT);

    if((ret = aal_puc_voq_map_cfg_get(0, &voq_mode)) != CA_E_OK)
    {
        RTL8277C_LOG_ERR("%s %d aal_puc_voq_map_cfg_get fail \n",__FUNCTION__,__LINE__);
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
        RTL8277C_LOG_ERR("%s %d not support voq mode=%d\n",__FUNCTION__,__LINE__,voq_mode);
        return RT_ERR_FAILED;
    }

    if(pQueue->queueId >= voq_offest)
    {
        RTL8277C_LOG_ERR("%s %d not support voq mode=%d queueId=%d\n",__FUNCTION__,__LINE__,voq_mode,pQueue->queueId);
        return RT_ERR_FAILED;
    }

    /* function body */
    memset(&puc_pir_voq_tbc_cfg, 0, sizeof(aal_puc_voq_tbc_mem_cfg_t));
    memset(&puc_voq_tbc_cfg, 0, sizeof(aal_puc_voq_tbc_mem_cfg_t));
    memset(&puc_queue_cfg, 0, sizeof(aal_puc_queue_cfg_t));

    if((ret = dal_rt_rtl8277c_ponmisc_tcont_llid_logic2phy_get(pQueue->schedulerId, &phySchId) != RT_ERR_OK))
    {
        printk("%s %d get phy schedler ID:%u fail\n",__FUNCTION__,__LINE__,pQueue->schedulerId);
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    puc_voq_idx = phySchId*voq_offest+pQueue->queueId;


    if((ret = aal_puc_pir_voq_tbc_mem_get(0, puc_voq_idx, &puc_pir_voq_tbc_cfg)) != CA_E_OK)
    {
        RTL8277C_LOG_ERR("%s %d aal_puc_pir_voq_tbc_mem_get fail \n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(puc_pir_voq_tbc_cfg.rate_m == 0xffff && puc_pir_voq_tbc_cfg.rate_k == 0x3ff)
    {
        pQueueCfg->pir = RTL8277C_METER_RATE_MAX;
    }
    else
    {
        pQueueCfg->pir = VOQ_RATE_M_K_TO_RATE_KBPS(puc_pir_voq_tbc_cfg.rate_m,puc_pir_voq_tbc_cfg.rate_k);
    }

    if((ret = aal_puc_voq_tbc_mem_get(0, puc_voq_idx, &puc_voq_tbc_cfg)) != CA_E_OK)
    {
        RTL8277C_LOG_ERR("%s %d aal_puc_voq_tbc_mem_get fail \n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }


    if(puc_voq_tbc_cfg.rate_m == 0 && puc_voq_tbc_cfg.rate_k == 0)
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
} /* end of dal_rtl8277c_ponmac_queue_get */

/* Function Name:
 *      dal_rtl8277c_ponmac_queue_add
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
dal_rtl8277c_ponmac_queue_add(rtk_ponmac_queue_t *pQueue, rtk_ponmac_queueCfg_t *pQueueCfg)
{
    int32   ret;
    ca_uint32_t voq_mode=0,voq_offest = 0;
    aal_puc_voq_tbc_mem_cfg_msk_t puc_pir_voq_tbc_cfg_msk;
    aal_puc_voq_tbc_mem_cfg_t puc_pir_voq_tbc_cfg;
    aal_puc_voq_tbc_mem_cfg_msk_t puc_voq_tbc_cfg_msk;
    aal_puc_voq_tbc_mem_cfg_t puc_voq_tbc_cfg;
    aal_puc_voq_drop_cfg_msk_t voq_drp_mask;
    aal_puc_voq_drop_cfg_t voq_drp;
    aal_puc_queue_cfg_msk_t puc_queue_cfg_msk;
    aal_puc_queue_cfg_t puc_queue_cfg;
    aal_puc_voq_sche_cfg_t voq_sche_config;
    ca_uint32_t tbc = 0;
    ca_uint32_t puc_voq_idx = 0;
    uint16 phySchId = 0;


    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC),"%s",__FUNCTION__);

    RTL8277C_LOG_DBG("Q_add schId=%d qId=%d type=%d cir=%d pir=%d wt=%d\n"
        ,pQueue->schedulerId,pQueue->queueId,pQueueCfg->type,pQueueCfg->cir,pQueueCfg->pir,pQueueCfg->weight);

    /* check Init status */
    RT_INIT_CHK(ponmac_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pQueue), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pQueueCfg), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pQueue->schedulerId >= HAL_MAX_NUM_OF_GPON_TCONT()), RT_ERR_INPUT);
    RT_PARAM_CHK((pQueue->queueId >= RTL8277C_TCONT_QUEUE_MAX), RT_ERR_INPUT);

    RT_PARAM_CHK((pQueueCfg->type >= QUEUE_TYPE_END), RT_ERR_INPUT);
    RT_PARAM_CHK((pQueueCfg->egrssDrop >= RTK_ENABLE_END), RT_ERR_INPUT);

    if((ret = aal_puc_voq_map_cfg_get(0, &voq_mode)) != CA_E_OK)
    {
        RTL8277C_LOG_ERR("%s %d aal_puc_voq_map_cfg_get fail \n",__FUNCTION__,__LINE__);
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
        RTL8277C_LOG_ERR("%s %d not support voq mode=%d \n",__FUNCTION__,__LINE__,voq_mode);
        return RT_ERR_FAILED;
    }

    if(pQueue->queueId >= voq_offest)
    {
        RTL8277C_LOG_ERR("%s %d not support voq mode=%d queueId=%d\n",__FUNCTION__,__LINE__,voq_mode,pQueue->queueId);
        return RT_ERR_FAILED;
    }

    if((ret = dal_rt_rtl8277c_ponmisc_tcont_llid_logic2phy_get(pQueue->schedulerId, &phySchId) != RT_ERR_OK))
    {
        printk("%s %d get phy schedler ID:%u fail\n",__FUNCTION__,__LINE__,pQueue->schedulerId);
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    puc_voq_idx = phySchId*voq_offest+pQueue->queueId;

    /*Disable queue drop*/
    voq_drp_mask.u32      = 0;
    memset(&voq_drp, 0, sizeof(voq_drp));

    voq_drp_mask.s.voqid  = 1;
    voq_drp_mask.s.enable = 1;
    voq_drp.enable = 0;
    voq_drp.voqid = puc_voq_idx;
    if((ret = aal_puc_voq_drop_set(0, voq_drp_mask, &voq_drp)) != CA_E_OK)
    {
        RTL8277C_LOG_ERR("%s %d aal_puc_voq_drop_set fail \n",__FUNCTION__,__LINE__);
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
        RTL8277C_LOG_ERR("%s %d aal_puc_port_voq_map_set fail \n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*Set voq sche table */
    memset(&voq_sche_config, 0, sizeof(aal_puc_voq_sche_cfg_t));

    if((ret = aal_puc_voq_sche_cfg_get(0,&voq_sche_config)) != CA_E_OK)
    {
        RTL8277C_LOG_ERR("%s %d aal_puc_voq_sche_cfg_get fail \n",__FUNCTION__,__LINE__);
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
        RTL8277C_LOG_ERR("%s %d aal_puc_port_voq_sche_info_tbl_set fail \n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    puc_pir_voq_tbc_cfg_msk.u32 = 0;
    memset(&puc_pir_voq_tbc_cfg, 0, sizeof(aal_puc_voq_tbc_mem_cfg_t));
    
    if(pQueueCfg->pir >= RTL8277C_METER_RATE_MAX)
    {
        puc_pir_voq_tbc_cfg.rate_k = 0x3ff;
        puc_pir_voq_tbc_cfg.rate_m = 0xffff;
        puc_pir_voq_tbc_cfg.bs = 0xfff;
        puc_pir_voq_tbc_cfg.mode = 0;
        puc_pir_voq_tbc_cfg.pkt_mode_class_sel = 0;
        puc_pir_voq_tbc_cfg.tbc = 0;
    }
    else
    {
        puc_pir_voq_tbc_cfg.rate_k = RATE_KBPS_TO_VOQ_RATE_K(pQueueCfg->pir);
        puc_pir_voq_tbc_cfg.rate_m = RATE_KBPS_TO_VOQ_RATE_M(pQueueCfg->pir);
        if(puc_pir_voq_tbc_cfg.rate_m <50)
            puc_pir_voq_tbc_cfg.bs = 5;
        else if(puc_pir_voq_tbc_cfg.rate_m <100)
            puc_pir_voq_tbc_cfg.bs = 10;
        else if(puc_pir_voq_tbc_cfg.rate_m <1000)
            puc_pir_voq_tbc_cfg.bs = 100;
        else
            puc_pir_voq_tbc_cfg.bs = 1000;
        puc_pir_voq_tbc_cfg.mode = 0;
        puc_pir_voq_tbc_cfg.pkt_mode_class_sel = 0;
        puc_pir_voq_tbc_cfg.tbc = 0;
    }

    puc_pir_voq_tbc_cfg_msk.s.rate_k = 1;
    puc_pir_voq_tbc_cfg_msk.s.rate_m = 1;
    puc_pir_voq_tbc_cfg_msk.s.bs = 1;
    puc_pir_voq_tbc_cfg_msk.s.mode = 1;
    puc_pir_voq_tbc_cfg_msk.s.tbc = 1;
    puc_pir_voq_tbc_cfg_msk.s.pkt_mode_class_sel = 1;

    if((ret = aal_puc_pir_voq_tbc_mem_set(0, puc_voq_idx, puc_pir_voq_tbc_cfg_msk, &puc_pir_voq_tbc_cfg)) != CA_E_OK)
    {
        RTL8277C_LOG_ERR("%s %d aal_puc_pir_voq_tbc_mem_set fail \n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /*Set CIR */
    puc_voq_tbc_cfg_msk.u32 = 0;
    memset(&puc_voq_tbc_cfg, 0, sizeof(aal_puc_voq_tbc_mem_cfg_t));

    if(pQueueCfg->cir >= RTL8277C_METER_RATE_MAX)
    {
        puc_voq_tbc_cfg.rate_k = 0;
        puc_voq_tbc_cfg.rate_m = 0;
        puc_voq_tbc_cfg.bs = 0;
        puc_voq_tbc_cfg.mode = 0;
        puc_voq_tbc_cfg.pkt_mode_class_sel = 0;
        puc_voq_tbc_cfg.tbc = 0;
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
    }

    puc_voq_tbc_cfg_msk.s.rate_k = 1;
    puc_voq_tbc_cfg_msk.s.rate_m = 1;
    puc_voq_tbc_cfg_msk.s.bs = 1;
    puc_voq_tbc_cfg_msk.s.mode = 1;
    puc_voq_tbc_cfg_msk.s.tbc = 1;
    puc_voq_tbc_cfg_msk.s.pkt_mode_class_sel = 1;

    if((ret = aal_puc_voq_tbc_mem_set(0, puc_voq_idx, puc_voq_tbc_cfg_msk, &puc_voq_tbc_cfg)) != CA_E_OK)
    {
        RTL8277C_LOG_ERR("%s %d aal_puc_voq_tbc_mem_set fail \n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

//RTK_VOQ_VALID_SUPPORT
    if (CA_PON_MODE == ONU_PON_MAC_GPON)
    {
        ca_spin_lock(g_aal_gpon_flow_lock);
        if((ret = aal_puc_voq_valid_set(0, puc_voq_idx ,1)) != CA_E_OK)
        {
            ca_spin_unlock(g_aal_gpon_flow_lock);
            RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
            return RT_ERR_FAILED;
        }
        ca_spin_unlock(g_aal_gpon_flow_lock);
    }
    else
    {
        if((ret = aal_puc_voq_valid_set(0, puc_voq_idx ,1)) != CA_E_OK)
        {
            RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
            return RT_ERR_FAILED;
        }
    }
//RTK_VOQ_VALID_SUPPORT

    return RT_ERR_OK;
} /* end of dal_rtl8277c_ponmac_queue_add */

/* Function Name:
 *      dal_rtl8277c_ponmac_egrBandwidthCtrlRate_get
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
dal_rtl8277c_ponmac_egrBandwidthCtrlRate_get(uint32 *pRate)
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
        RTL8277C_LOG_ERR("%s %d aal_puc_agr_shp_ctrl_get fail \n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(puc_agr_shp_ctrl_cfg.enb == 0)
    {
        *pRate = RTL8277C_METER_RATE_MAX;
    }
    else
    {
        *pRate = VOQ_RATE_M_K_TO_EGR_RATE_KBPS(puc_agr_shp_ctrl_cfg.rate_m,puc_agr_shp_ctrl_cfg.rate_k);
    }

    return RT_ERR_OK;
}   /* end of dal_rtl8277c_ponmac_egrBandwidthCtrlRate_get */

/* Function Name:
 *      dal_rtl8277c_ponmac_egrBandwidthCtrlRate_set
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
 *      (2) The unit of granularity in rtl8277c is 1Kbps.
 */
int32
dal_rtl8277c_ponmac_egrBandwidthCtrlRate_set(uint32 rate)
{
    int32 ret;
    aal_puc_agr_shp_ctrl_cfg_t puc_agr_shp_ctrl_cfg;
    
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC), "rate=%d",rate);

    /* check Init status */
    RT_INIT_CHK(ponmac_init);

    /* parameter check */
    RT_PARAM_CHK((RTL8277C_PON_EGRESS_RATE_MAX < rate), RT_ERR_INPUT);

    /* function body */
    memset(&puc_agr_shp_ctrl_cfg,0,sizeof(aal_puc_agr_shp_ctrl_cfg_t));

    if(rate >= RTL8277C_METER_RATE_MAX)
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
        RTL8277C_LOG_ERR("%s %d aal_puc_agr_shp_ctrl_set fail \n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl8277c_ponmac_egrBandwidthCtrlRate_set */

/* Function Name:
 *      dal_rtl8277c_ponmac_egrBandwidthCtrlIncludeIfg_get
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
dal_rtl8277c_ponmac_egrBandwidthCtrlIncludeIfg_get(rtk_enable_t *pIfgInclude)
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
        RTL8277C_LOG_ERR("%s %d aal_puc_ctrl_cfg_get fail \n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(puc_ctrl_config.agrshpovhd == 0)
        *pIfgInclude = DISABLED;
    else
        *pIfgInclude = ENABLED;

    return RT_ERR_OK;
}   /* end of dal_rtl8277c_ponmac_egrBandwidthCtrlIncludeIfg_get */


/* Function Name:
 *      dal_rtl8277c_ponmac_egrBandwidthCtrlIncludeIfg_set
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
dal_rtl8277c_ponmac_egrBandwidthCtrlIncludeIfg_set(rtk_enable_t ifgInclude)
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
        RTL8277C_LOG_ERR("%s %d aal_puc_ctrl_cfg_set fail \n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}   /* end of dal_rtl8277c_ponmac_egrBandwidthCtrlIncludeIfg_set */

/* Function Name:
 *      dal_rtl8277c_ponmac_txDisableGpio_get
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
dal_rtl8277c_ponmac_txDisableGpio_get(rtk_enable_t *pEnable)
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
    if((ret = dal_rtl8277c_gpio_databit_get(tx_disable_gpio,&data)) != CA_E_OK)
    {
        RTL8277C_LOG_ERR("%s %d dal_rtl8277c_gpio_databit_get fail \n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

#ifdef CONFIG_TX_DISABLE_POLARITY_REVERSE
    if(data == 0)
#else
    if(data == 1)
#endif
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;
#else
    *pEnable = DISABLED;
#endif

    return RT_ERR_OK;
}   /* end of dal_rtl8277c_ponmac_txDisableGpio_get */

/* Function Name:
 *      dal_rtl8277c_ponmac_txDisableGpio_set
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
dal_rtl8277c_ponmac_txDisableGpio_set(rtk_enable_t enable)
{
    int32 ret;
    uint32 data;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC), "enable=%d",enable);

    /* check Init status */
    RT_INIT_CHK(ponmac_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    /* function body */
#ifdef CONFIG_BOARD_REALTEK_CA8277_PON_GPIO
#ifdef CONFIG_TX_DISABLE_POLARITY_REVERSE
    if(enable == DISABLED)
#else
    if(enable == ENABLED)
#endif
        data = 1;
    else
        data = 0;

    if((ret = dal_rtl8277c_gpio_databit_set(tx_disable_gpio,data)) != CA_E_OK)
    {
        RTL8277C_LOG_ERR("%s %d dal_rtl8277c_gpio_databit_set fail \n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
#endif

    return RT_ERR_OK;
}   /* end of dal_rtl8277c_ponmac_txDisableGpio_set */

/* Function Name:
 *      dal_rtl8277c_ponmac_txPowerDisableGpio_get
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
dal_rtl8277c_ponmac_txPowerDisableGpio_get(rtk_enable_t *pEnable)
{
    int32 ret;
    uint32 data;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC), "");

    /* check Init status */
    RT_INIT_CHK(ponmac_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    /* function body */
#if defined(CONFIG_BOARD_REALTEK_CA8277_PON_GPIO) && defined(CONFIG_TX_POWER_TURN_ON_FEATURE)
    if((ret = dal_rtl8277c_gpio_databit_get(tx_power_gpio,&data)) != CA_E_OK)
    {
        RTL8277C_LOG_ERR("%s %d dal_rtl8277c_gpio_databit_get fail \n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(data == 1)
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;
#else
    *pEnable = DISABLED;
#endif

    return RT_ERR_OK;
}   /* end of dal_rtl8277c_ponmac_txPowerDisableGpio_get */

/* Function Name:
 *      dal_rtl8277c_ponmac_txPowerDisableGpio_set
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
dal_rtl8277c_ponmac_txPowerDisableGpio_set(rtk_enable_t enable)
{
    int32 ret;
    uint32 data;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC), "enable=%d",enable);

    /* check Init status */
    RT_INIT_CHK(ponmac_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    /* function body */
#if defined(CONFIG_BOARD_REALTEK_CA8277_PON_GPIO) && defined(CONFIG_TX_POWER_TURN_ON_FEATURE)
    if(enable == ENABLED)
        data = 1;
    else
        data = 0;

    if((ret = dal_rtl8277c_gpio_databit_set(tx_power_gpio,data)) != CA_E_OK)
    {
        RTL8277C_LOG_ERR("%s %d dal_rtl8277c_gpio_databit_set fail \n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
#endif

    return RT_ERR_OK;
}   /* end of dal_rtl8277c_ponmac_txPowerDisableGpio_set */

/* Function Name:
 *      dal_rtl8277c_ponmac_trxSwitchGpio_set
 * Description:
 *      set the gpio state for switching transceiver module.
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
dal_rtl8277c_ponmac_trxSwitchGpio_set(rtk_enable_t enable)
{
    int32 ret;
    uint32 data;
    static uint32 trx_switch_init=0;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC), "enable=%d",enable);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    /* function body */
#ifdef CONFIG_BOARD_REALTEK_CA8277_PON_GPIO
#ifdef CONFIG_TRX_SWITCH_GPIO_PIN
    if(CONFIG_TRX_SWITCH_GPIO_PIN == 0xFF)
    {
        RTL8277C_LOG_DBG("%s %d TRX Switch GPIO is not configured \n",__FUNCTION__,__LINE__);
        return RT_ERR_OK;
    }

    if(trx_switch_init==0)
    {
        trx_switch_gpio = CONFIG_TRX_SWITCH_GPIO_PIN;
        if(CA_PON_MODE_IS_GPON_FAMILY(CA_PON_MODE) || CA_PON_MODE_IS_EPON_FAMILY(CA_PON_MODE))
        {
            if((ret = dal_rtl8277c_gpio_state_set(trx_switch_gpio,ENABLED)) != CA_E_OK)
            {
                RTL8277C_LOG_ERR("%s %d dal_rtl8277c_gpio_state_set fail \n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            if((ret = dal_rtl8277c_gpio_mode_set(trx_switch_gpio,GPIO_OUTPUT)) != CA_E_OK)
            {
                RTL8277C_LOG_ERR("%s %d dal_rtl8277c_gpio_mode_set fail \n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            trx_switch_init = 1;
        }
        else
        {
            RTL8277C_LOG_DBG("%s %d unsupported PON mode \n",__FUNCTION__,__LINE__);
            return RT_ERR_OK;
        }
    }

#ifdef CONFIG_TRX_SWITCH_GPIO_POLARITY_REVERSE
    if(enable == DISABLED)
#else
    if(enable == ENABLED)
#endif
        data = 1;
    else
        data = 0;

    if((ret = dal_rtl8277c_gpio_databit_set(trx_switch_gpio,data)) != CA_E_OK)
    {
        RTL8277C_LOG_ERR("%s %d dal_rtl8277c_gpio_databit_set fail \n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
#endif
#endif

    return RT_ERR_OK;
}   /* end of dal_rtl8277c_ponmac_txDisableGpio_set */

