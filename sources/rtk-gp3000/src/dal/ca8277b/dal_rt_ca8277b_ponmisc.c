/*
 * Copyright (C) 2019 Realtek Semiconductor Corp.
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
 * Purpose : Definition of PON misc API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) PON misc
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/rt/rt_ponmisc.h>
#include <hal/chipdef/ca8277b/ca8277b_def.h>
#include <dal/ca8277b/dal_ca8277b.h>
#include <dal/ca8277b/dal_ca8277b_ponmac.h>
#include <dal/ca8277b/dal_ca8277b_epon.h>
#include <dal/ca8277b/dal_ca8277b_gpon.h>
#include <dal/ca8277b/dal_ca8277b_stat.h>
#include <dal/ca8277b/dal_ca8277b_gpio.h>
#include <dal/ca8277b/dal_rt_ca8277b_ponmisc.h>
#include <scfg.h>
#include <aal_puc.h>
#include <aal_port.h>


extern ca_uint32_t aal_pon_mac_mode_set(ca_device_id_t device_id, ca_uint32_t pon_mode);
extern ca_status_t ca_pon_init(ca_device_id_t device_id);
//extern ca_status_t ca_pon_tx_register(ca_uint32_t (*handler)(ca_device_id_t device_id, void *pkt));
//extern ca_status_t __ca_tx_dying_gasp(CA_IN ca_device_id_t device_id, CA_IN ca_pkt_t    *pkt);
/*
 * Symbol Definition
 */
#define CA8277B_MAX_TCONT_LLID  (32)
#define CA8277B_TCONT_LLID_START_IDX (1) /*T-CONT 0 for OMCC use, data start from index 1 */
#define CA8277B_PUC_BP_NUM  (64)

/*
 * Data Declaration
 */
static uint32 ponmisc_init = INIT_NOT_COMPLETED;
static uint16 gTcont_llid_logic2PhyMap[CA8277B_MAX_TCONT_LLID]= {0};

/*
 * Macro Declaration
 */

 /*
 * Function Declaration
 */

static int _ponmisc_mode_set(ca_uint8_t pon_mode)
{
    int32 ret = RT_ERR_OK;
    uint32 voqId;
    uint32 cur_pon_mode;
    aal_puc_ctrl_cfg_msk_t          puc_ctrl_config_mask;
    aal_puc_ctrl_cfg_t              puc_ctrl_config;
    aal_puc_btc_cfg_msk_t           puc_btc_mask;
    aal_puc_btc_cfg_t               puc_btc_config;

    if(pon_mode >= ONU_PON_MAC_MAX)
    {
        return RT_ERR_INPUT;
    }

    cur_pon_mode =aal_pon_mac_mode_get(0);
    if(ONU_PON_MAC_MAX > cur_pon_mode) {
        ca_printf("%s:%d PON mode has been set as %d, can't set again\n",__func__, __LINE__, cur_pon_mode);
        return RT_ERR_FAILED;
    }

    ret =aal_pon_mac_mode_set(0, pon_mode);
    if(CA_E_OK != ret) {
        ca_printf("%s:%d %s returns error, ret = %d\n",__func__, __LINE__, "aal_pon_mac_mode_set()", ret);
        return RT_ERR_FAILED;
    }
    CA_PON_MODE = pon_mode;

    /* GPON/EPON init will be triggered by OMCI/OAM with rt_gpon_init/rt_epon_init
        only init for Ethernet mode */
    if(CA_PON_MODE_IS_ETHERNET(pon_mode))
    {
        /* load other pon related scfg settings */
        ret =aal_pon_scfg_load(0);

        /* PONMAC init */
        ret = ca_pon_init(0);
        if(CA_E_OK != ret) {
            ca_printf("%s:%d %s returns error, ret = %d\n",__func__, __LINE__, "ca_pon_init()", ret);
        }

        /* reset PON port couner */
        dal_ca8277b_stat_port_reset(HAL_GET_PON_PORT());
    }

    return RT_ERR_OK;
}


/* Function Name:
 *      dal_rt_ca8277b_ponmisc_init
 * Description:
 *      Configure PON misc initial settings
 * Input:
 *      None.
 * Output:
 *      pPonMode    - current running PON mode
 *      pPonSpeed   - current running PON speed
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
int32
dal_rt_ca8277b_ponmisc_init(void)
{
    int32 ret = RT_ERR_OK;

    ca_boolean_t phy_port_to_deep_q[CFG_ID_RTK_PHY_PORT_TO_DEEP_Q_LEN]= {1,1,1,1,0,0,0,0};
    uint8 portId;
    uint8 logicId = 0;
    uint8 phyId = 0;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_GPON),"%s",__FUNCTION__);

    /*Adjust physical and logic mapping table for T-CONT/LLID , it dependents  on the deep queue setting of LAN port*/    
    for(portId=CA8277B_TCONT_LLID_START_IDX;portId< CFG_ID_RTK_PHY_PORT_TO_DEEP_Q_LEN; portId++)
    {
        aal_port_physical_port_to_dq_get(0, portId, &phy_port_to_deep_q[portId]);
        if(phy_port_to_deep_q[portId] == 0)
            gTcont_llid_logic2PhyMap[logicId++] = portId;

    }
    
    for(phyId=CFG_ID_RTK_PHY_PORT_TO_DEEP_Q_LEN;phyId< CA8277B_MAX_TCONT_LLID; phyId++)
    {
        gTcont_llid_logic2PhyMap[logicId++] = phyId;
    }

    for(portId=CA8277B_TCONT_LLID_START_IDX;portId< CFG_ID_RTK_PHY_PORT_TO_DEEP_Q_LEN; portId++)
    {
        if(phy_port_to_deep_q[portId] == 1)
            gTcont_llid_logic2PhyMap[logicId++] = portId;
    }

#ifdef CONFIG_BOARD_REALTEK_CA8277_PON_GPIO
    dal_ca8277b_gpio_init();
#endif

    ponmisc_init = INIT_COMPLETED;

    return ret;
}/* end of dal_rt_ca8277b_ponmisc_init */

/* Function Name:
 *      dal_rt_ca8277b_ponmisc_mode_get
 * Description:
 *      Configure PON misc get settings
 * Input:
 *      None.
 * Output:
 *      pPonMode    - current running PON mode
 *      pPonSpeed   - current running PON speed
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_FEATURE_NOT_SUPPORTED
 *      RT_ERR_NULL_POINTER
 * Note:
 */
int32
dal_rt_ca8277b_ponmisc_mode_get(rt_ponmisc_ponMode_t *pPonMode,rt_ponmisc_ponSpeed_t *pPonSpeed)
{
    int32 ret = RT_ERR_OK;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC),"%s",__FUNCTION__);

    RT_INIT_CHK(ponmisc_init);

    RT_PARAM_CHK((NULL == pPonMode), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pPonSpeed), RT_ERR_NULL_POINTER);

    switch(CA_PON_MODE)
    {
        case ONU_PON_MAC_EPON_1G:
        case ONU_PON_MAC_EPON_D10G:
        case ONU_PON_MAC_EPON_BI10G:
            *pPonMode = RT_EPON_MODE;
            break;
        case ONU_PON_MAC_GPON:
        case ONU_PON_MAC_XGPON1:
        case ONU_PON_MAC_XGSPON:
            *pPonMode = RT_GPON_MODE;
            break;
        case ONU_PON_MAC_ETHERNET_10G:
            *pPonMode = RT_XFI_MODE;
            break;

        default:
            return RT_ERR_FEATURE_NOT_SUPPORTED;
    }

    switch(CA_PON_MODE)
    {
        case ONU_PON_MAC_EPON_1G:
        case ONU_PON_MAC_GPON:
            *pPonSpeed = RT_1G25G_SPEED;
            break;
        case ONU_PON_MAC_EPON_D10G:
        case ONU_PON_MAC_XGPON1:
            *pPonSpeed = RT_DN10G_SPEED;
            break;
        case ONU_PON_MAC_EPON_BI10G:
        case ONU_PON_MAC_XGSPON:
            *pPonSpeed = RT_BOTH10G_SPEED;
            break;
        case ONU_PON_MAC_ETHERNET_10G:
            *pPonSpeed = RT_10G_SPEED;
            break;
        default:
            return RT_ERR_FEATURE_NOT_SUPPORTED;
    }

    return ret;
}/* end of dal_rt_ca8277b_ponmisc_mode_get */

/* Function Name:
 *      dal_rt_ca8277b_ponmisc_mode_set
 * Description:
 *      Configure PON mode and speed settings
 * Input:
 *      ponMode    - PON mode
 *      ponSpeed   - PON speed
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_FEATURE_NOT_SUPPORTED
 *      RT_ERR_INPUT
 * Note:
 */
int32
dal_rt_ca8277b_ponmisc_mode_set(rt_ponmisc_ponMode_t ponMode, rt_ponmisc_ponSpeed_t ponSpeed)
{
    int32 ret = RT_ERR_OK;
    ca_uint8_t ponmode; 

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC),"%s",__FUNCTION__);

    RT_INIT_CHK(ponmisc_init);

    if((ponMode >= RT_PONMODE_END) || ((ponSpeed >= RT_PONSPEED_END) && (ponSpeed >= RT_ETHSPEED_END)))
            return RT_ERR_INPUT;

    if(ponMode == RT_GPON_MODE)
    {
        switch(ponSpeed)
        {
            case RT_1G25G_SPEED:
                ponmode = ONU_PON_MAC_GPON;
                break;
            case RT_DN10G_SPEED:
                ponmode = ONU_PON_MAC_XGPON1;
                break;
            case RT_BOTH10G_SPEED:
                ponmode = ONU_PON_MAC_XGSPON;
                break;
            default:
                return RT_ERR_FEATURE_NOT_SUPPORTED;
        }
    }
    else if(ponMode == RT_EPON_MODE)
    {
        switch(ponSpeed)
        {
            case RT_1G25G_SPEED:
                ponmode = ONU_PON_MAC_EPON_1G;
                break;
            case RT_DN10G_SPEED:
                ponmode = ONU_PON_MAC_EPON_D10G;
                break;
            case RT_BOTH10G_SPEED:
                ponmode = ONU_PON_MAC_EPON_BI10G;
                break;
            default:
                return RT_ERR_FEATURE_NOT_SUPPORTED;
         }
    }
    else if(ponMode == RT_XFI_MODE)
    {
        switch(ponSpeed)
        {
            case RT_10G_SPEED:
                ponmode = ONU_PON_MAC_ETHERNET_10G;
                break;
            default:
                return RT_ERR_FEATURE_NOT_SUPPORTED;
         }
    }
    else
        return RT_ERR_FEATURE_NOT_SUPPORTED;

    ret = _ponmisc_mode_set(ponmode);
    if(ret != RT_ERR_OK)
    {
        printk("%s %d set PON mode failed, mode=%d, speed=%d\n",__FUNCTION__,__LINE__, ponMode, ponSpeed);
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
    }

    return ret;
}/* end of dal_rt_ca8277b_ponmisc_mode_set */


/* Function Name:
 *      dal_rt_ca8277b_ponmisc_ponQueue_mode_get
 * Description:
 *      get currect PON queue mode setting
 * Input:
 *      None.
 * Output:
 *      pPonQueueMode    - current running PON queue mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 */
int32
dal_rt_ca8277b_ponmisc_ponQueue_mode_get(rt_ponmisc_ponQueueMode_t *pPonQueueMode)
{
    int32 ret = RT_ERR_OK;
    uint32 puc_voq_map_mode;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC),"%s",__FUNCTION__);

    RT_INIT_CHK(ponmisc_init);

    RT_PARAM_CHK((NULL == pPonQueueMode), RT_ERR_NULL_POINTER);

    ret = aal_puc_voq_map_cfg_get(0, &puc_voq_map_mode);
    if (ret != CA_E_OK)
    {
        return RT_ERR_FAILED;

    }

    if(AAL_PUC_VOQ_MODE_FLAT_MAP == puc_voq_map_mode)
    {
        *pPonQueueMode = RT_PON_QUEUE_FLOW_MODE;
    }
    else
    {
        *pPonQueueMode = RT_PON_QUEUE_FIXED_MODE;
    }
    
    return ret;
}/* end of dal_rt_ca8277b_ponmisc_ponQueue_mode_get */



/* Function Name:
 *      dal_rt_ca8277b_ponmisc_tcont_llid_logic2phy_get
 * Description:
 *      Get physical to logic mapping of T-CONT/LLID
 * Input:
 *      logicId   - logic index of T-CONT/LLID
 * Output:
 *      pPhyId    - pointer to physical index of T-CONT/LLID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_PORT_ID
 *      RT_ERR_NULL_POINTER
 * Note:
 */
int32
dal_rt_ca8277b_ponmisc_tcont_llid_logic2phy_get(uint16 logicId, uint16 *pPhyId)
{

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC),"%s",__FUNCTION__);

    RT_INIT_CHK(ponmisc_init);

    /* parameter check */
    RT_PARAM_CHK((logicId >= CA8277B_MAX_TCONT_LLID), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pPhyId), RT_ERR_NULL_POINTER);

    *pPhyId = gTcont_llid_logic2PhyMap[logicId];

    return RT_ERR_OK;
}

/* Function Name:
*      dal_rt_ca8277b_ponmisc_burstPolarityReverse_get
* Description:
*      Get the status of PON burst polarity reverse
* Input:
*      none
* Output:
*      pPolarity       - pointer of burst polarity reverse status
* Return:
*      RT_ERR_OK
*      RT_ERR_FAILED
*      RT_ERR_NULL_POINTER - input parameter may be null pointer
* Note:
*
*/
int32
dal_rt_ca8277b_ponmisc_burstPolarityReverse_get(rt_ponmisc_polarity_t *pPolarity)
{
    int32 ret = RT_ERR_OK;
    aal_psds_rg24_t psds_rg24;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC),"%s",__FUNCTION__);

    RT_INIT_CHK(ponmisc_init);

    RT_PARAM_CHK((NULL == pPolarity), RT_ERR_NULL_POINTER);
    
    ret = aal_psds_rg24_get(0, &psds_rg24);
    if (ret != CA_E_OK)
    {
        return RT_ERR_FAILED;
    }

    if((psds_rg24.reg_ben_por_sel >> 1) & 0x1)
        *pPolarity = RT_PONMISC_POLARITY_REVERSE_ON;
    else
        *pPolarity = RT_PONMISC_POLARITY_REVERSE_OFF;

    return ret;
}

/* Function Name:
*      dal_rt_ca8277b_ponmisc_burstPolarityReverse_set
* Description:
*      Set the status of PON burst polarity reverse
* Input:
*      polarity        - the burst polarity reverse status
* Output:
*      none
* Return:
*      RT_ERR_OK
*      RT_ERR_FAILED
*      RT_ERR_INPUT        - Invalid input parameters.
* Note:
*
*/
int32
dal_rt_ca8277b_ponmisc_burstPolarityReverse_set(rt_ponmisc_polarity_t polarity)
{
    int32 ret = RT_ERR_OK;
    aal_psds_rg24_t psds_rg24;
    aal_psds_rg24_mask_t psds_rg24_mask;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC),"%s",__FUNCTION__);

    RT_INIT_CHK(ponmisc_init);

    if(polarity >= RT_PONMISC_POLARITY_END)
        return RT_ERR_INPUT;

    memset(&psds_rg24_mask, 0, sizeof(psds_rg24_mask));
    psds_rg24_mask.s.reg_ben_por_sel = 1;
    psds_rg24.reg_ben_por_sel = (polarity << 1);
    ret = aal_psds_rg24_set(0, psds_rg24_mask, &psds_rg24);
    if (ret != CA_E_OK)
    {
        return RT_ERR_FAILED;
    }

    return ret;
}

/* Function Name:
*      rt_ponmisc_forceLaserState_get
* Description:
*      Get Force Laser status
* Input:
*      none
* Output:
*      pStatus       - pointer of Force Laser status
* Return:
*      RT_ERR_OK
*      RT_ERR_FAILED
*      RT_ERR_NULL_POINTER - input parameter may be null pointer
* Note:
*
*/
int32
dal_rt_ca8277b_ponmisc_forceLaserState_get(rt_ponmisc_laser_status_t *pStatus)
{
    int32 ret = RT_ERR_OK;
    aal_psds_gbox_ctrl_t psds_gbox;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC),"%s",__FUNCTION__);

    RT_INIT_CHK(ponmisc_init);

    RT_PARAM_CHK((NULL == pStatus), RT_ERR_NULL_POINTER);
    
    ret = aal_psds_gbox_ctrl_get(0, &psds_gbox);
    if (ret != CA_E_OK)
    {
        return RT_ERR_FAILED;
    }

    if(psds_gbox.lao_en)
        *pStatus = RT_PONMISC_LASER_STATUS_FORCE_ON;
    else
        *pStatus = RT_PONMISC_LASER_STATUS_NORMAL;

    return ret;
}

/* Function Name:
*      dal_rt_ca8277b_ponmisc_forceLaserState_set
* Description:
*      Set Force Laser status
* Input:
*      status       - Force Laser status
* Output:
*      none
* Return:
*      RT_ERR_OK
*      RT_ERR_FAILED
*      RT_ERR_INPUT        - Invalid input parameters.
* Note:
*
*/
int32
dal_rt_ca8277b_ponmisc_forceLaserState_set(rt_ponmisc_laser_status_t status)
{
    int32 ret = RT_ERR_OK;
    aal_psds_gbox_ctrl_t psds_gbox;
    aal_psds_gbox_ctrl_mask_t psds_gbox_mask;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC),"%s",__FUNCTION__);

    RT_INIT_CHK(ponmisc_init);

    // not support FORCE_OFF
    if(status >= RT_PONMISC_LASER_STATUS_FORCE_OFF)
        return RT_ERR_INPUT;

    memset(&psds_gbox_mask, 0, sizeof(psds_gbox_mask));
    psds_gbox_mask.s.lao_en = 1;
    psds_gbox.lao_en = status;
    ret = aal_psds_gbox_ctrl_set(0, psds_gbox_mask, &psds_gbox);
    if (ret != CA_E_OK)
    {
        return RT_ERR_FAILED;
    }

    return ret;
}

/* Function Name:
*      dal_rt_ca8277b_ponmisc_forcePRBS_get
* Description:
*      Get the PRBS config
* Input:
*      none
* Output:
*      pPrbsCfg       - pointer of PRBS config
* Return:
*      RT_ERR_OK
*      RT_ERR_FAILED
*      RT_ERR_NULL_POINTER - input parameter may be null pointer
* Note:
*
*/
int32
dal_rt_ca8277b_ponmisc_forcePRBS_get(rt_ponmisc_prbs_t *pPrbsCfg)
{
    int32 ret = RT_ERR_OK;
    aal_psds_gbox_ctrl_t psds_gbox;
    aal_psds_prbs_ctrl_t prbs_ctrl;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC),"%s",__FUNCTION__);

    RT_INIT_CHK(ponmisc_init);

    RT_PARAM_CHK((NULL == pPrbsCfg), RT_ERR_NULL_POINTER);
    
    ret = aal_psds_gbox_ctrl_get(0, &psds_gbox);
    if (ret != CA_E_OK)
    {
        return RT_ERR_FAILED;
    }

    ret = aal_psds_prbs_ctrl_get(0, &prbs_ctrl);
    if (ret != CA_E_OK)
    {
        return RT_ERR_FAILED;
    }

    if(psds_gbox.prbs_tx_en == 0)
        *pPrbsCfg = RT_PONMISC_PRBS_OFF;
    else if(prbs_ctrl.tx_prbs_mode == 0)
        *pPrbsCfg = RT_PONMISC_PRBS_31;
    else if(prbs_ctrl.tx_prbs_mode == 1)
        *pPrbsCfg = RT_PONMISC_PRBS_23;
    else if(prbs_ctrl.tx_prbs_mode == 2)
        *pPrbsCfg = RT_PONMISC_PRBS_15;
    else if(prbs_ctrl.tx_prbs_mode == 3)
        *pPrbsCfg = RT_PONMISC_PRBS_7;
    else
         return RT_ERR_FAILED;

    return ret;

}

/* Function Name:
*      dal_rt_ca8277b_ponmisc_forcePRBS_set
* Description:
*      Set the PRBS config
* Input:
*      none
* Output:
*      prbsCfg       - PRBS config
* Return:
*      RT_ERR_OK
*      RT_ERR_FAILED
*      RT_ERR_INPUT        - Invalid input parameters.
* Note:
*
*/
int32
dal_rt_ca8277b_ponmisc_forcePRBS_set(rt_ponmisc_prbs_t prbsCfg)
{
    int32 ret = RT_ERR_OK;
    uint32 prbs_mode=0;
    aal_psds_gbox_ctrl_t psds_gbox;
    aal_psds_gbox_ctrl_mask_t psds_gbox_mask;
    aal_psds_prbs_ctrl_t prbs_ctrl;
    aal_psds_prbs_ctrl_mask_t prbs_ctrl_mask;
    aal_psds_rga1_t  rga1;
    aal_psds_rga1_mask_t  rga1_mask;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC),"%s",__FUNCTION__);

    RT_INIT_CHK(ponmisc_init);

    if(prbsCfg == RT_PONMISC_PRBS_OFF)
    {
        memset(&psds_gbox_mask, 0xFF, sizeof(psds_gbox_mask));
        psds_gbox.lao_en = 0;
        psds_gbox.lpbk_tx_rx_en = 0;
        psds_gbox.lpbk_rx_tx_near_en = 0;
        psds_gbox.txd_inv_en = 0;
        psds_gbox.rxd_inv_en = 0;
        psds_gbox.tx_elst_ff_rsync = 0;
        psds_gbox.rx_elst_ff_rsync = 0;
        psds_gbox.tx_elst_ff_rsync_en = 0;
        psds_gbox.prbs_tx_en = 0;
        psds_gbox.prbs_rx_en = 0;

        switch(CA_PON_MODE)
        {
            case ONU_PON_MAC_EPON_1G: //0x400
                psds_gbox.rx_1gepn_turbo_bit_ordering= 1;
                psds_gbox.tx_bit_ordering = 0;
                psds_gbox.rx_bit_ordering = 0;
                break;
            case ONU_PON_MAC_GPON:  //0x450
                psds_gbox.rx_1gepn_turbo_bit_ordering= 1;
                psds_gbox.tx_bit_ordering = 1;
                psds_gbox.rx_bit_ordering = 1;
                break;
            case ONU_PON_MAC_EPON_D10G: //0x410
                psds_gbox.rx_1gepn_turbo_bit_ordering= 1;
                psds_gbox.tx_bit_ordering = 0;
                psds_gbox.rx_bit_ordering = 1;
                break;
            case ONU_PON_MAC_EPON_BI10G:  //fallthrough
            case ONU_PON_MAC_XGPON1:  //fallthrough
            case ONU_PON_MAC_XGSPON: //0x50
                psds_gbox.rx_1gepn_turbo_bit_ordering= 0;
                psds_gbox.tx_bit_ordering = 1;
                psds_gbox.rx_bit_ordering = 1;
                psds_gbox.tx_3p125g_bit_ordering = 0;
                break;
            default:
                return RT_ERR_FAILED;
        }

        if ((ret = aal_psds_gbox_ctrl_set(0, psds_gbox_mask, &psds_gbox)) != CA_E_OK)
        {
            return RT_ERR_FAILED;
        }

        return ret;
    }
    else
    {
        if(prbsCfg == RT_PONMISC_PRBS_31)
            prbs_mode = 0;
        else if(prbsCfg == RT_PONMISC_PRBS_23)
            prbs_mode = 1;
        else if(prbsCfg == RT_PONMISC_PRBS_15)
            prbs_mode = 2;
        else if(prbsCfg == RT_PONMISC_PRBS_7)
            prbs_mode = 3;
        else // only support PRBS 31/23/15/7
        {
            printk("Unsupported PRBS mode!\n");
            return RT_ERR_INPUT;
        }
    }
		
    memset(&psds_gbox_mask, 0xFF, sizeof(psds_gbox_mask));
    // 0x8053
    psds_gbox.lao_en = 1;
    psds_gbox.lpbk_tx_rx_en = 0;
    psds_gbox.lpbk_rx_tx_near_en = 0;
    psds_gbox.txd_inv_en = 0;
    psds_gbox.rxd_inv_en = 0;
    psds_gbox.rx_1gepn_turbo_bit_ordering= 0;
    psds_gbox.tx_elst_ff_rsync = 0;
    psds_gbox.rx_elst_ff_rsync = 0;
    psds_gbox.tx_bit_ordering = 1;
    psds_gbox.rx_bit_ordering = 1;
    psds_gbox.tx_elst_ff_rsync_en = 0;
    psds_gbox.tx_3p125g_bit_ordering = 0;
    psds_gbox.prbs_tx_en = 1;
    psds_gbox.prbs_rx_en = 1;
    if ((ret = aal_psds_gbox_ctrl_set(0, psds_gbox_mask, &psds_gbox)) != CA_E_OK)
    {
        return RT_ERR_FAILED;
    }

    memset(&prbs_ctrl_mask, 0, sizeof(prbs_ctrl_mask));
    prbs_ctrl_mask.s.rx_prbs_mode = 1;
    prbs_ctrl_mask.s.tx_prbs_mode = 1;
    prbs_ctrl_mask.s.rx_clr_cnt = 1;
    prbs_ctrl.rx_prbs_mode = prbs_mode;
    prbs_ctrl.tx_prbs_mode = prbs_mode;
    prbs_ctrl_mask.s.rx_clr_cnt = 1;
    if ((ret = aal_psds_prbs_ctrl_set(0, prbs_ctrl_mask, &prbs_ctrl)) != CA_E_OK)
    {
        return RT_ERR_FAILED;
    }

    memset(&prbs_ctrl_mask, 0, sizeof(prbs_ctrl_mask));
    prbs_ctrl_mask.s.rx_clr_cnt = 1;
    prbs_ctrl_mask.s.rx_clr_cnt = 0;
    if ((ret = aal_psds_prbs_ctrl_set(0, prbs_ctrl_mask, &prbs_ctrl)) != CA_E_OK)
    {
        return RT_ERR_FAILED;
    }

    // enable burst
    aal_psds_burst_enable(0, 1);

    memset(&rga1_mask, 0, sizeof(rga1_mask));
    rga1_mask.s.reg0_pi_m_mode_cdr2 = 1;
    rga1.reg0_pi_m_mode_cdr2 = 1;
    if ((ret = aal_psds_rga1_set(0, rga1_mask, &rga1)) != CA_E_OK)
    {
        return RT_ERR_FAILED;
    }

    return ret;
}

