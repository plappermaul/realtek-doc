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
#include <hal/chipdef/rtl9607f/rtl9607f_def.h>
#include <dal/rtl9607f/dal_rtl9607f.h>
#include <dal/rtl9607f/dal_rtl9607f_ponmac.h>
#include <dal/rtl9607f/dal_rtl9607f_epon.h>
#include <dal/rtl9607f/dal_rt_rtl9607f_epon.h>
#include <dal/rtl9607f/dal_rtl9607f_gpon.h>
#include <dal/rtl9607f/dal_rt_rtl9607f_gpon.h>
#include <dal/rtl9607f/dal_rtl9607f_stat.h>
#include <dal/rtl9607f/dal_rtl9607f_gpio.h>
#include <dal/rtl9607f/dal_rt_rtl9607f_ponmisc.h>
#include <scfg.h>
#include <aal_puc.h>
#include <aal_port.h>
#include <aal_common.h>
#include <soc/cortina/registers.h>

extern ca_uint32_t aal_pon_mac_mode_set(ca_device_id_t device_id, ca_uint32_t pon_mode);
extern ca_status_t ca_pon_init(ca_device_id_t device_id);

/*
 * Symbol Definition
 */
#define RTL9607F_MAX_TCONT_LLID  (32)
#define RTL9607F_TCONT_START_IDX (1) /*T-CONT 0 for OMCC use, data start from index 1 */
#define RTL9607F_LLID_START_IDX (0) /*LLID start from index 0 */    


/*
 * Data Declaration
 */
static uint32 ponmisc_init = INIT_NOT_COMPLETED; 
static uint16 gTcont_llid_logic2PhyMap[RTL9607F_MAX_TCONT_LLID]= {0};


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

#ifdef CONFIG_BOARD_REALTEK_CA8277_PON_GPIO
#ifdef CONFIG_TRX_SWITCH_GPIO_PIN
    if(CA_PON_MODE_IS_GPON_FAMILY(pon_mode) || CA_PON_MODE_IS_EPON_FAMILY(pon_mode))
    {
        if((pon_mode == ONU_PON_MAC_GPON) || (pon_mode == ONU_PON_MAC_GPON_BI2G5) || (pon_mode == ONU_PON_MAC_EPON_1G))
        {
            //assume disable state is for GPON/EPON
            if((ret = dal_rtl9607f_ponmac_trxSwitchGpio_set(DISABLED)) != CA_E_OK)
            {
                ca_printf("%s %d dal_rtl9607f_ponmac_trxSwitchGpio_set fail \n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
        }
        else
        {
            //enable state is for other 10G modes
            if((ret = dal_rtl9607f_ponmac_trxSwitchGpio_set(ENABLED)) != CA_E_OK)
            {
                ca_printf("%s %d dal_rtl9607f_ponmac_trxSwitchGpio_set fail \n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
        }
    }
#endif
#endif

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
        dal_rtl9607f_stat_port_reset(HAL_GET_PON_PORT());
    }

    return RT_ERR_OK;
}


static int32
_dal_rt_rtl9607f_ponmisc_tcont_llid_logic2phy_init(ca_uint32_t pon_mode)
{
    uint8 portId,portStartIdx;
    uint8 logicId = 0; 

    portStartIdx = CA_PON_MODE_IS_GPON_FAMILY(pon_mode)?RTL9607F_TCONT_START_IDX:RTL9607F_LLID_START_IDX;

    /*For GPON,Logic T-CONT 0~30=> physical T-CONT 1~31
      For EPON,Logic LLID 0~31=> physical LLID  0~31*/
    for(portId=portStartIdx;portId< RTL9607F_MAX_TCONT_LLID; portId++)
    {
        gTcont_llid_logic2PhyMap[logicId++] = portId;
    }
     
    return RT_ERR_OK;
}


/* Function Name:
 *      dal_rt_rtl9607f_ponmisc_init
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
dal_rt_rtl9607f_ponmisc_init(void)
{
    int32 ret = RT_ERR_OK;
    ca_uint32_t pon_mode;

#ifdef CONFIG_CORTINA_BOARD_FPGA
    printk("%s %d FPGA ponmisc init\n",__FUNCTION__,__LINE__);
#endif
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC),"%s",__FUNCTION__);

    pon_mode = aal_pon_mac_mode_get(0);

    if(pon_mode < ONU_PON_MAC_MAX)
    {
        _dal_rt_rtl9607f_ponmisc_tcont_llid_logic2phy_init(pon_mode);
    }

#ifdef CONFIG_BOARD_REALTEK_CA8277_PON_GPIO
    dal_rtl9607f_gpio_init();
#endif

    ponmisc_init = INIT_COMPLETED;

    return ret;
}/* end of dal_rt_rtl9607f_ponmisc_init */

/* Function Name:
 *      dal_rt_rtl9607f_ponmisc_mode_get
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
dal_rt_rtl9607f_ponmisc_mode_get(rt_ponmisc_ponMode_t *pPonMode,rt_ponmisc_ponSpeed_t *pPonSpeed)
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
        case ONU_PON_MAC_GPON_BI2G5:
        case ONU_PON_MAC_XGPON1:
        case ONU_PON_MAC_XGSPON:
            *pPonMode = RT_GPON_MODE;
            break;
        case ONU_PON_MAC_NGPON2:
        case ONU_PON_MAC_NGPON2_D10G:
            *pPonMode = RT_NGPON2_MODE;
            break;
        case ONU_PON_MAC_ETHERNET_10G:
            *pPonMode = RT_XFI_MODE;
            break;
        case ONU_PON_MAC_ETHERNET_FIBER_100M:
        case ONU_PON_MAC_ETHERNET_FIBER_1G:
        case ONU_PON_MAC_ETHERNET_FIBER_2G5:
            *pPonMode = RT_FIBER_MODE;
            break;
        case ONU_PON_MAC_ETHERNET_SGMII_10M:
        case ONU_PON_MAC_ETHERNET_SGMII_100M:
        case ONU_PON_MAC_ETHERNET_SGMII_500M:
        case ONU_PON_MAC_ETHERNET_SGMII_1G:
        case ONU_PON_MAC_ETHERNET_SGMII_2G5:
            *pPonMode = RT_SGMII_MODE;
            break;
        case ONU_PON_MAC_ETHERNET_USXGMII_10G:
            *pPonMode = RT_USXGMII_MODE;
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
        case ONU_PON_MAC_GPON_BI2G5:
            *pPonSpeed = RT_BOTH2DOT5G_SPEED;
            break;
        case ONU_PON_MAC_EPON_D10G:
        case ONU_PON_MAC_XGPON1:
        case ONU_PON_MAC_NGPON2_D10G:
            *pPonSpeed = RT_DN10G_SPEED;
            break;
        case ONU_PON_MAC_EPON_BI10G:
        case ONU_PON_MAC_XGSPON:
        case ONU_PON_MAC_NGPON2:
            *pPonSpeed = RT_BOTH10G_SPEED;
            break;
        case ONU_PON_MAC_ETHERNET_10G:
        case ONU_PON_MAC_ETHERNET_USXGMII_10G:
            *pPonSpeed = RT_10G_SPEED;
            break;
        case ONU_PON_MAC_ETHERNET_SGMII_2G5:
        case ONU_PON_MAC_ETHERNET_FIBER_2G5:
            *pPonSpeed = RT_2DOT5G_SPEED;
            break;
        case ONU_PON_MAC_ETHERNET_FIBER_1G:
        case ONU_PON_MAC_ETHERNET_SGMII_1G:
            *pPonSpeed = RT_1G_SPEED;
            break;
        case ONU_PON_MAC_ETHERNET_SGMII_500M:
            *pPonSpeed = RT_500M_SPEED;
            break;
        case ONU_PON_MAC_ETHERNET_FIBER_100M:
        case ONU_PON_MAC_ETHERNET_SGMII_100M:
            *pPonSpeed = RT_100M_SPEED;
            break;
        case ONU_PON_MAC_ETHERNET_SGMII_10M:
            *pPonSpeed = RT_10M_SPEED;
            break;
        default:
            return RT_ERR_FEATURE_NOT_SUPPORTED;
    }

    return ret;
}/* end of dal_rt_rtl9607f_ponmisc_mode_get */

/* Function Name:
 *      dal_rt_rtl9607f_ponmisc_mode_set
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
dal_rt_rtl9607f_ponmisc_mode_set(rt_ponmisc_ponMode_t ponMode, rt_ponmisc_ponSpeed_t ponSpeed)
{
    int32 ret = RT_ERR_OK;
    ca_uint8_t ponmode; 
    ca_uint8_t cur_pon_mode;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC),"%s",__FUNCTION__);

    RT_INIT_CHK(ponmisc_init);

    cur_pon_mode = aal_pon_mac_mode_get(0);

    if(((ponMode >= RT_PONMODE_END) && (ponMode != PON_MODE_DISABLE)) || ((ponSpeed >= RT_PONSPEED_END) && (ponSpeed >= RT_ETHSPEED_END)))
            return RT_ERR_INPUT;

    if(ponMode == RT_GPON_MODE)
    {
#ifdef CONFIG_GPON_FEATURE
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
            case RT_BOTH2DOT5G_SPEED:
                ponmode = ONU_PON_MAC_GPON_BI2G5;
                break;
            default:
                return RT_ERR_FEATURE_NOT_SUPPORTED;
        }
#else
        return RT_ERR_FEATURE_NOT_SUPPORTED;
#endif
    }
    else if(ponMode == RT_EPON_MODE)
    {
#ifdef CONFIG_EPON_FEATURE
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
#else
         return RT_ERR_FEATURE_NOT_SUPPORTED;
#endif
    }
    else if(ponMode == RT_NGPON2_MODE)
    {
#ifdef CONFIG_GPON_FEATURE
        switch(ponSpeed)
        {
            case RT_BOTH10G_SPEED:
                ponmode = ONU_PON_MAC_NGPON2;
                break;
            case RT_DN10G_SPEED:
                ponmode = ONU_PON_MAC_NGPON2_D10G;
                break;
            default:
                return RT_ERR_FEATURE_NOT_SUPPORTED;
        }
#else
        return RT_ERR_FEATURE_NOT_SUPPORTED;
#endif
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
    else if(ponMode == RT_FIBER_MODE)
    {
        switch(ponSpeed)
        {
            case RT_100M_SPEED:
                ponmode = ONU_PON_MAC_ETHERNET_FIBER_100M;
                break;
            case RT_1G_SPEED:
                ponmode = ONU_PON_MAC_ETHERNET_FIBER_1G;
                break;
            case RT_2DOT5G_SPEED:
                ponmode = ONU_PON_MAC_ETHERNET_FIBER_2G5;
                break;
            default:
                return RT_ERR_FEATURE_NOT_SUPPORTED;
         }
    }
    else if(ponMode == RT_SGMII_MODE)
    {
        switch(ponSpeed)
        {
            case RT_1G_SPEED:
                ponmode = ONU_PON_MAC_ETHERNET_SGMII_1G;
                break;
            case RT_2DOT5G_SPEED:
                ponmode = ONU_PON_MAC_ETHERNET_SGMII_2G5;
                break;
            case RT_500M_SPEED:
                ponmode = ONU_PON_MAC_ETHERNET_SGMII_500M;
                break;
            case RT_100M_SPEED:
                ponmode = ONU_PON_MAC_ETHERNET_SGMII_100M;
                break;
            case RT_10M_SPEED:
                ponmode = ONU_PON_MAC_ETHERNET_SGMII_10M;
                break;
            default:
                return RT_ERR_FEATURE_NOT_SUPPORTED;
         }
    }
    else if(ponMode == RT_USXGMII_MODE)
    {
        switch(ponSpeed)
        {
            case RT_10G_SPEED:
                ponmode = ONU_PON_MAC_ETHERNET_USXGMII_10G;
                break;
            default:
                return RT_ERR_FEATURE_NOT_SUPPORTED;
         }
    }
    else if(ponMode == PON_MODE_DISABLE)
    {
        //do nothing, just waiting de-init
    }
    else
        return RT_ERR_INPUT;

    if(ONU_PON_MAC_MAX > cur_pon_mode)
    {
        /* de-init current PON mode*/
        if(CA_PON_MODE_IS_EPON_FAMILY(cur_pon_mode))
        {
#ifdef CONFIG_EPON_FEATURE
            dal_rt_rtl9607f_epon_deinit(cur_pon_mode);
#else
            return RT_ERR_FEATURE_NOT_SUPPORTED;
#endif
        }
        else if(CA_PON_MODE_IS_GPON_FAMILY(cur_pon_mode))
        {
#ifdef CONFIG_GPON_FEATURE
            dal_rt_rtl9607f_gpon_deinit(cur_pon_mode);
#else
            return RT_ERR_FEATURE_NOT_SUPPORTED;
#endif
        }
        else if(CA_PON_MODE_IS_ETHERNET(cur_pon_mode))
        {
             /* Reset PONMAC */
             ret = aal_pon_deinit(0, cur_pon_mode);
        }
        else
        {
            return RT_ERR_FEATURE_NOT_SUPPORTED;
        }

       // set to DISABLE mode first
        aal_pon_mac_mode_set(0, ONU_PON_MAC_MODE_IGNORED);
        CA_PON_MODE = ONU_PON_MAC_MODE_IGNORED;

    }

    if(ponMode == PON_MODE_DISABLE)
    {
        // leaving for DISABLE mode after de-init
        return ret;
    }

    ret = _ponmisc_mode_set(ponmode);
    if(ret != RT_ERR_OK)
    {
        printk("%s %d set PON mode failed, mode=%d, speed=%d\n",__FUNCTION__,__LINE__, ponMode, ponSpeed);
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
    }

    _dal_rt_rtl9607f_ponmisc_tcont_llid_logic2phy_init(ponmode);

    return ret;
}/* end of dal_rt_rtl9607f_ponmisc_mode_set */

/* Function Name:
 *      dal_rt_rtl9607f_ponmisc_ponQueue_mode_get
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
dal_rt_rtl9607f_ponmisc_ponQueue_mode_get(rt_ponmisc_ponQueueMode_t *pPonQueueMode)
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
}/* end of dal_rt_rtl9607f_ponmisc_ponQueue_mode_get */

/* Function Name:
 *      dal_rt_rtl9607f_ponmisc_tcont_llid_logic2phy_get
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
dal_rt_rtl9607f_ponmisc_tcont_llid_logic2phy_get(uint16 logicId, uint16 *pPhyId)
{

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC),"%s",__FUNCTION__);

    RT_INIT_CHK(ponmisc_init);

    /* parameter check */
    RT_PARAM_CHK((logicId >= RTL9607F_MAX_TCONT_LLID), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pPhyId), RT_ERR_NULL_POINTER);

    *pPhyId = gTcont_llid_logic2PhyMap[logicId];

    return RT_ERR_OK;
}

/* Function Name:
*      dal_rt_rtl9607f_ponmisc_burstPolarityReverse_get
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
dal_rt_rtl9607f_ponmisc_burstPolarityReverse_get(rt_ponmisc_polarity_t *pPolarity)
{
    int32 ret = RT_ERR_OK;
    uint32 polarity;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC),"%s",__FUNCTION__);

    RT_INIT_CHK(ponmisc_init);

    RT_PARAM_CHK((NULL == pPolarity), RT_ERR_NULL_POINTER);
    
    ret = aal_psds_tx_polarity_get(0, &polarity);
    if (ret != CA_E_OK)
    {
        return RT_ERR_FAILED;
    }

    *pPolarity = polarity;

    return ret;
}

/* Function Name:
*      dal_rt_rtl9607f_ponmisc_burstPolarityReverse_set
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
dal_rt_rtl9607f_ponmisc_burstPolarityReverse_set(rt_ponmisc_polarity_t polarity)
{
    int32 ret = RT_ERR_OK;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC),"%s",__FUNCTION__);

    RT_INIT_CHK(ponmisc_init);

    if(polarity >= RT_PONMISC_POLARITY_END)
        return RT_ERR_INPUT;

    ret = aal_psds_tx_polarity_set(0, polarity);
    if (ret != CA_E_OK)
    {
        return RT_ERR_FAILED;
    }

    return ret;
}

/* Function Name:
*      dal_rt_rtl9607f_ponmisc_rxlosPolarityReverse_get
* Description:
*      Get the status of PON rx_los polarity reverse
* Input:
*      none
* Output:
*      pPolarity       - pointer of rx_los polarity reverse status
* Return:
*      RT_ERR_OK
*      RT_ERR_FAILED
*      RT_ERR_NULL_POINTER - input parameter may be null pointer
* Note:
*
*/
int32
dal_rt_rtl9607f_ponmisc_rxlosPolarityReverse_get(rt_ponmisc_polarity_t *pPolarity)
{
    int32 ret = RT_ERR_OK;
    uint32 polarity;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC),"%s",__FUNCTION__);

    RT_INIT_CHK(ponmisc_init);

    RT_PARAM_CHK((NULL == pPolarity), RT_ERR_NULL_POINTER);
    
    ret = aal_psds_rx_polarity_get(0, &polarity);
    if (ret != CA_E_OK)
    {
        return RT_ERR_FAILED;
    }

    *pPolarity = polarity;

    return ret;
}

/* Function Name:
*      dal_rt_rtl9607f_ponmisc_rxlosPolarityReverse_set
* Description:
*      Set the status of PON rx_los polarity reverse
* Input:
*      polarity        - the rx_los polarity reverse status
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
dal_rt_rtl9607f_ponmisc_rxlosPolarityReverse_set(rt_ponmisc_polarity_t polarity)
{
    int32 ret = RT_ERR_OK;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC),"%s",__FUNCTION__);

    RT_INIT_CHK(ponmisc_init);

    if(polarity >= RT_PONMISC_POLARITY_END)
        return RT_ERR_INPUT;

    ret = aal_psds_rx_polarity_set(0, polarity);
    if (ret != CA_E_OK)
    {
        return RT_ERR_FAILED;
    }

    return ret;
}

/* Function Name:
*      dal_rt_rtl9607f_ponmisc_forceLaserState_get
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
dal_rt_rtl9607f_ponmisc_forceLaserState_get(rt_ponmisc_laser_status_t *pStatus)
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
*      dal_rt_rtl9607f_ponmisc_forceLaserState_set
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
dal_rt_rtl9607f_ponmisc_forceLaserState_set(rt_ponmisc_laser_status_t status)
{
    int32 ret = RT_ERR_OK;
    aal_psds_gbox_ctrl_t psds_gbox;
    aal_psds_gbox_ctrl_mask_t psds_gbox_mask;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC),"%s",__FUNCTION__);

    RT_INIT_CHK(ponmisc_init);

    // not support FORCE_OFF
    if(status >= RT_PONMISC_LASER_STATUS_FORCE_OFF)
        return RT_ERR_INPUT;

    memset(&psds_gbox, 0, sizeof(aal_psds_gbox_ctrl_t));
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
*      dal_rt_rtl9607f_ponmisc_forcePRBS_get
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
dal_rt_rtl9607f_ponmisc_forcePRBS_get(rt_ponmisc_prbs_t *pPrbsCfg)
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
*      dal_rt_rtl9607f_ponmisc_forcePRBS_set
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
dal_rt_rtl9607f_ponmisc_forcePRBS_set(rt_ponmisc_prbs_t prbsCfg)
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
        memset(&psds_gbox, 0, sizeof(aal_psds_gbox_ctrl_t));
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
        //all the bit ordering are synced in new PSDS design
        psds_gbox.tx_bit_ordering = 1;
        psds_gbox.rx_bit_ordering = 1;
        psds_gbox.rx_1gepn_turbo_bit_ordering= 1;
        psds_gbox.tx_3p125g_bit_ordering = 1;

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
		
    memset(&psds_gbox, 0, sizeof(aal_psds_gbox_ctrl_t));
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

    memset(&prbs_ctrl, 0, sizeof(aal_psds_prbs_ctrl_t));
    memset(&prbs_ctrl_mask, 0, sizeof(prbs_ctrl_mask));
    prbs_ctrl_mask.s.rx_prbs_mode = 1;
    prbs_ctrl_mask.s.tx_prbs_mode = 1;
    prbs_ctrl_mask.s.rx_clr_cnt = 1;
    prbs_ctrl.rx_prbs_mode = prbs_mode;
    prbs_ctrl.tx_prbs_mode = prbs_mode;
    prbs_ctrl.rx_clr_cnt = 1;
    if ((ret = aal_psds_prbs_ctrl_set(0, prbs_ctrl_mask, &prbs_ctrl)) != CA_E_OK)
    {
        return RT_ERR_FAILED;
    }
    
    memset(&prbs_ctrl, 0, sizeof(aal_psds_prbs_ctrl_t));
    memset(&prbs_ctrl_mask, 0, sizeof(prbs_ctrl_mask));
    prbs_ctrl_mask.s.rx_clr_cnt = 1;
    prbs_ctrl.rx_clr_cnt = 0;
    if ((ret = aal_psds_prbs_ctrl_set(0, prbs_ctrl_mask, &prbs_ctrl)) != CA_E_OK)
    {
        return RT_ERR_FAILED;
    }

    // enable burst
    aal_psds_burst_enable(0, 1);

    return ret;
}

/* Function Name:
*      dal_rt_rtl9607f_ponmisc_forcePRBS_rx_get
* Description:
*      Get the PRBS RX config
* Input:
*      none
* Output:
*      pPrbsCfg       - pointer of PRBS RX config
* Return:
*      RT_ERR_OK
*      RT_ERR_FAILED
*      RT_ERR_NULL_POINTER - input parameter may be null pointer
* Note:
*
*/
int32
dal_rt_rtl9607f_ponmisc_forcePRBS_rx_get(rt_ponmisc_prbs_t *pPrbsCfg)
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

    if(psds_gbox.prbs_rx_en == 0)
        *pPrbsCfg = RT_PONMISC_PRBS_OFF;
    else if(prbs_ctrl.rx_prbs_mode == 0)
        *pPrbsCfg = RT_PONMISC_PRBS_31;
    else if(prbs_ctrl.rx_prbs_mode == 1)
        *pPrbsCfg = RT_PONMISC_PRBS_23;
    else if(prbs_ctrl.rx_prbs_mode == 2)
        *pPrbsCfg = RT_PONMISC_PRBS_15;
    else if(prbs_ctrl.rx_prbs_mode == 3)
        *pPrbsCfg = RT_PONMISC_PRBS_7;
    else
         return RT_ERR_FAILED;

    return ret;

}

/* Function Name:
*      dal_rt_rtl9607f_ponmisc_forcePRBS_rx_set
* Description:
*      Set the PRBS RX config
* Input:
*      prbsCfg       - PRBS RX config
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
dal_rt_rtl9607f_ponmisc_forcePRBS_rx_set(rt_ponmisc_prbs_t prbsCfg)
{
    int32 ret = RT_ERR_OK;
    uint32 prbs_mode=0;
    uint32 prbs_tx_enabled = 0;
    aal_psds_gbox_ctrl_t psds_gbox;
    aal_psds_gbox_ctrl_mask_t psds_gbox_mask;
    aal_psds_prbs_ctrl_t prbs_ctrl;
    aal_psds_prbs_ctrl_mask_t prbs_ctrl_mask;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC),"%s",__FUNCTION__);

    RT_INIT_CHK(ponmisc_init);
    
    if(prbsCfg == RT_PONMISC_PRBS_OFF)
    {
        if ((ret = aal_psds_gbox_ctrl_get(0, &psds_gbox)) != CA_E_OK)
        {
            printk("aal_psds_gbox_ctrl_get error!\n");
            return RT_ERR_FAILED;
        }
        prbs_tx_enabled = psds_gbox.prbs_tx_en;

        memset(&psds_gbox, 0, sizeof(aal_psds_gbox_ctrl_t));
        memset(&psds_gbox_mask, 0, sizeof(psds_gbox_mask));
        psds_gbox_mask.s.prbs_rx_en = 1;
        psds_gbox.prbs_rx_en = 0;
        if(prbs_tx_enabled==0)
        {
            psds_gbox_mask.s.rx_1gepn_turbo_bit_ordering = 1;
            psds_gbox.rx_1gepn_turbo_bit_ordering = 1;
        }

        if ((ret = aal_psds_gbox_ctrl_set(0, psds_gbox_mask, &psds_gbox)) != CA_E_OK)
        {
            printk("aal_psds_gbox_ctrl_set error!\n");
        }

        memset(&prbs_ctrl, 0, sizeof(aal_psds_prbs_ctrl_t));
        memset(&prbs_ctrl_mask, 0, sizeof(prbs_ctrl_mask));
        prbs_ctrl_mask.s.rx_prbs_enbl = 1;
        prbs_ctrl.rx_prbs_enbl = 0;

        if ((ret = aal_psds_prbs_ctrl_set(0, prbs_ctrl_mask, &prbs_ctrl)) != CA_E_OK)
        {
            printk("aal_psds_prbs_ctrl_set error!\n");
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
		
    memset(&psds_gbox, 0, sizeof(aal_psds_gbox_ctrl_t));
    memset(&psds_gbox_mask, 0, sizeof(psds_gbox_mask));

    psds_gbox_mask.s.prbs_rx_en = 1;
    psds_gbox_mask.s.rx_1gepn_turbo_bit_ordering = 1;

    psds_gbox.rx_1gepn_turbo_bit_ordering = 0;
    psds_gbox.prbs_rx_en = 1;

    if ((ret = aal_psds_gbox_ctrl_set(0, psds_gbox_mask, &psds_gbox)) != CA_E_OK)
    {
        return RT_ERR_FAILED;
    }

    memset(&prbs_ctrl, 0, sizeof(aal_psds_prbs_ctrl_t));
    memset(&prbs_ctrl_mask, 0, sizeof(prbs_ctrl_mask));
    prbs_ctrl_mask.s.rx_prbs_enbl = 1;
    prbs_ctrl_mask.s.rx_prbs_mode = 1;
    prbs_ctrl_mask.s.rx_clr_cnt = 1;
    prbs_ctrl.rx_prbs_enbl = 1;
    prbs_ctrl.rx_prbs_mode = prbs_mode;
    prbs_ctrl.rx_clr_cnt = 1;
    if ((ret = aal_psds_prbs_ctrl_set(0, prbs_ctrl_mask, &prbs_ctrl)) != CA_E_OK)
    {
        return RT_ERR_FAILED;
    }

    memset(&prbs_ctrl, 0, sizeof(aal_psds_prbs_ctrl_t));
    memset(&prbs_ctrl_mask, 0, sizeof(prbs_ctrl_mask));
    prbs_ctrl_mask.s.rx_clr_cnt = 1;
    prbs_ctrl.rx_clr_cnt = 0;
    if ((ret = aal_psds_prbs_ctrl_set(0, prbs_ctrl_mask, &prbs_ctrl)) != CA_E_OK)
    {
        return RT_ERR_FAILED;
    }

    return ret;
}

/* Function Name:
*      dal_rt_rtl9607f_ponmisc_prbsRxStatus_get
* Description:
*      Get the PRBS RX Status
* Input:
*      none
* Output:
*      pPrbsRxStatus       - pointer of PRBS RX Status
* Return:
*      RT_ERR_OK
*      RT_ERR_FAILED
*      RT_ERR_NULL_POINTER - input parameter may be null pointer
* Note:
*
*/
int32
dal_rt_rtl9607f_ponmisc_prbsRxStatus_get(rt_ponmisc_prbsRxStatus_t *pPrbsRxStatus)
{
    int32 ret = RT_ERR_OK;
    aal_psds_prbs_ctrl_t prbs_ctrl;
    aal_psds_prbs_ctrl_mask_t prbs_ctrl_mask;
    aal_pon_opt_status_t opt;
    aal_psds_status_t psds_sts;

    pPrbsRxStatus->prbsRxState= RT_PONMISC_PRBS_RX_READY;
    pPrbsRxStatus->prbsRxErrCnt = 0;
    pPrbsRxStatus->prbsRxErrRate = 0;

    //Check if RX_LOS
    if((ret = aal_pon_optical_status_get(0, &opt)) != CA_E_OK)
    {
        return RT_ERR_FAILED;
    }

    if((1 == opt.opt_rx_los) || (1 == opt.opt_mod_abs))
    {
        pPrbsRxStatus->prbsRxState = RT_PONMISC_PRBS_RX_LOS;
        return RT_ERR_OK;
    }

    //Check PRBS Sync State & Error Count
    if((ret = aal_psds_status_get(0, &psds_sts)) != CA_E_OK)
    {
        return RT_ERR_FAILED;
    }

    if(1 == psds_sts.prbs_rx_out_of_sync)
    {
        pPrbsRxStatus->prbsRxState = RT_PONMISC_PRBS_RX_NOTAVAILABLE;
        return RT_ERR_OK;
    }
    pPrbsRxStatus->prbsRxErrCnt = psds_sts.prbs_err_cnt;

    //Reset PRBS RX Error Count
    memset(&prbs_ctrl, 0, sizeof(aal_psds_prbs_ctrl_t));
    memset(&prbs_ctrl_mask, 0, sizeof(prbs_ctrl_mask));
    prbs_ctrl_mask.s.rx_clr_cnt = 1;
    prbs_ctrl.rx_clr_cnt = 1;
    if ((ret = aal_psds_prbs_ctrl_set(0, prbs_ctrl_mask, &prbs_ctrl)) != CA_E_OK)
    {
        return RT_ERR_FAILED;
    }

    prbs_ctrl_mask.s.rx_clr_cnt = 1;
    prbs_ctrl.rx_clr_cnt = 0;
    if ((ret = aal_psds_prbs_ctrl_set(0, prbs_ctrl_mask, &prbs_ctrl)) != CA_E_OK)
    {
        return RT_ERR_FAILED;
    }

    return ret;
}

/* Function Name:
*      dal_rt_rtl9607f_ponmisc_selfRogue_cfg_get
* Description:
*      Get the self rogue detect config
* Input:
*      none
* Output:
*      pCfg       - pointer of self rogue detect config
* Return:
*      RT_ERR_OK            - OK
*      RT_ERR_FAILED        - failure
*      RT_ERR_NULL_POINTER  - input parameter may be null pointer
*      RT_ERR_OUT_OF_RANGE  - value out of range
* Note:
*
*/
int32
dal_rt_rtl9607f_ponmisc_selfRogue_cfg_get(rtk_enable_t *pCfg)
{
    int32 ret = RT_ERR_OK;
    ca_boolean_t  state;
    
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC),"%s",__FUNCTION__);

    RT_INIT_CHK(ponmisc_init);

    RT_PARAM_CHK((NULL == pCfg), RT_ERR_NULL_POINTER);

    ret = aal_pon_opt_rogue_state_get(0, &state);
    if (ret != CA_E_OK)
    {
        return RT_ERR_FAILED;
    }

    if(state == 0)
        *pCfg = DISABLED;
    else if(state == 1)
        *pCfg = ENABLED;
    else
        ret = RT_ERR_OUT_OF_RANGE;

    return ret;

}

/* Function Name:
*      dal_rt_rtl9607f_ponmisc_selfRogue_cfg_set
* Description:
*      Set the self rogue detect config
* Input:
*      cfg       - self rogue detect config
* Output:
*      none
* Return:
*      RT_ERR_OK           - OK
*      RT_ERR_FAILED       - failure
*      RT_ERR_INPUT        - Invalid input parameters.
* Note:
*
*/
int32
dal_rt_rtl9607f_ponmisc_selfRogue_cfg_set(rtk_enable_t cfg)
{
    int32 ret = RT_ERR_OK;
    ca_boolean_t  state;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC),"%s",__FUNCTION__);

    RT_INIT_CHK(ponmisc_init);

    if(cfg == ENABLED)
        state = 1;
    else if(cfg == DISABLED)
        state = 0;
    else
        return RT_ERR_INPUT;
    
    if ((ret = aal_pon_opt_rogue_state_set(0, state)) != CA_E_OK)
    {
        printk("aal_pon_opt_rogue_state_set error %x!\n",ret);
        return RT_ERR_FAILED;
    }

    return ret;
}

/* Function Name:
*      dal_rt_rtl9607f_ponmisc_selfRogue_count_get
* Description:
*      Get the self rogue detect counter
* Input:
*      none
* Output:
*      pCnt       - pointer of self rogue detect counter
* Return:
*      RT_ERR_OK            - OK
*      RT_ERR_FAILED        - failure
*      RT_ERR_NULL_POINTER  - input parameter may be null pointer
* Note:
*
*/
int32
dal_rt_rtl9607f_ponmisc_selfRogue_count_get(rt_ponmisc_rogue_cnt_t *pCnt)
{
    int32 ret = RT_ERR_OK;
    aal_pon_opt_rogue_cnt_t  rog_cnt;
    
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC),"%s",__FUNCTION__);

    RT_INIT_CHK(ponmisc_init);

    RT_PARAM_CHK((NULL == pCnt), RT_ERR_NULL_POINTER);

    if((ret = aal_pon_opt_rogue_cnt_get(0, &rog_cnt))!= CA_E_OK)
    {
        return RT_ERR_FAILED;
    }

    pCnt->rogue_mismatch = rog_cnt.opt_at0 + rog_cnt.opt_at1;
    pCnt->rogue_toolong = rog_cnt.opt_toolong;

    return ret;
}

/* Function Name:
*      dal_rt_rtl9607f_ponmisc_selfRogue_count_clear
* Description:
*      Clear the self rogue detect counter
* Input:
*      none
* Output:
*      none
* Return:
*      RT_ERR_OK           - OK
*      RT_ERR_FAILED       - failure
*      RT_ERR_INPUT        - Invalid input parameters.
* Note:
*
*/
int32
dal_rt_rtl9607f_ponmisc_selfRogue_count_clear(void)
{
    int32 ret = RT_ERR_OK;
    ca_boolean_t  state;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC),"%s",__FUNCTION__);

    RT_INIT_CHK(ponmisc_init);

    if ((ret = aal_pon_opt_rogue_cnt_clear(0)) != CA_E_OK)
    {
        return RT_ERR_FAILED;
    }

    return ret;
}

/* Function Name:
*      dal_rt_rtl9607f_ponmisc_dying_gasp_force_trigger
* Description:
*      force trigger dying gasp signal
* Input:
*      none
* Output:
*      none
* Return:
*      RT_ERR_OK           - OK
*      RT_ERR_FAILED       - failure
* Note:
*
*/
int32
dal_rt_rtl9607f_ponmisc_dying_gasp_force_trigger(void)
{
    int32 ret = RT_ERR_OK;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC),"%s",__FUNCTION__);

    RT_INIT_CHK(ponmisc_init);

    AAL_PERI_REG_WRITE(0x305,PER_DYING_GASP);

    return ret;
}

