/*
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 *
 * *
 * $Revision:  $
 * $Date: 2012-08-07
 *
 * Purpose : EPON MAC register access APIs
 *
 * Feature : Provide the APIs to access EPON MAC
 *
 */

/*
 * Include Files
 */
#include <common/rt_type.h>

#include <rtk/rt/rt_epon.h>
#include <rtk/rt/rt_ponmisc.h>
#include <rtk/rt/rt_rate.h>
#include <rtk/trap.h>
#include <dal/ca8277b/dal_ca8277b.h>
#include <dal/ca8277b/dal_rt_ca8277b_epon.h>
#include <dal/ca8277b/dal_ca8277b_ponmac.h>
#include <dal/ca8277b/dal_rt_ca8277b_ponmisc.h>
#include <dal/ca8277b/dal_ca8277b_trap.h>
#include <dal/ca8277b/dal_ca8277b_stat.h>

#include "osal_spinlock.h"
#include <cortina-api/include/pkt_tx.h>
#include <cortina-api/include/epon.h>
#include <cortina-api/include/port.h>

#include <aal_pon.h>
#include <aal_epon.h>
#include <aal_puc.h>
#include <rtk/ponmac.h>

#include "ni-drv/ca_ni.h"
#include "ca-ne-rtk/ca_ext.h"

extern ca_uint32_t aal_init_pon;
extern ca_status_t ca_pon_init(ca_device_id_t device_id);

static uint32 epon_init = {INIT_NOT_COMPLETED};
static ca_uint32_t gPonMode;
static ca_mac_addr_t puc_da = {0x00, 0x13, 0x25, 0x00, 0x00, 0x00};
static ca_mac_addr_t puc_sa = {0x00, 0x13, 0x25, 0x00, 0x00, 0x01};

static ca_int8_t etype_oam[]  = {0xff, 0xf1};
static ca_int8_t etype_vlan[] = {0x81, 0x00};

static ca_mac_addr_t oam_da = {0x01, 0x80, 0xc2, 0x00, 0x00, 0x02};

static rt_enable_t gOamTrapEnable = DISABLED; //Default disable trap OAM packet
static rt_epon_oam_rx_callback gOamRxCallback = NULL;

static uint8 regLlidIdx = 0;

extern int drv_nic_register_rxhook(int portmask,int priority,p2rfunc_t rx);

#define ETH_HEAD_LEN    16

static int _oam_receive_from_nic(struct napi_struct *napi,struct net_device *dev, struct sk_buff *skb, nic_hook_private_t *nh_priv)
{
    int ret;
    uint8_t * oam_ptr;
    uint32_t oam_len;
    unsigned char llidIdx; 
    uint8 isOamFlag = FALSE;
    ca_uint8_t sw_idx;

    if(0 != memcmp(&skb->data[0],puc_sa,6))
    {
        return RE8670_RX_CONTINUE;
    }

    if (0 == memcmp(&skb->data[12], etype_oam, sizeof(etype_oam)))
    {
        llidIdx = skb->data[11] & 0x3f;  /* restore llid: puc_da[5:0] */
        oam_ptr = skb->data+ETH_HEAD_LEN;
        oam_len = skb->len-ETH_HEAD_LEN;
        isOamFlag = TRUE;
    }
    else if(0 == memcmp(&skb->data[12], etype_vlan, sizeof(etype_vlan))) //single VID
    {
        if (0 == memcmp(&skb->data[16], etype_oam, sizeof(etype_oam)))
        {
            llidIdx = skb->data[11] & 0x3f;  /* restore llid: puc_da[5:0] */
            oam_ptr = skb->data+ETH_HEAD_LEN+4;
            oam_len = skb->len-ETH_HEAD_LEN-4;
            isOamFlag = TRUE;
        }
    }

    //check oam ether type
    if (isOamFlag == TRUE)
    {
        if((oam_ptr[12] == 0x88) && (oam_ptr[13] == 0x09) && (oam_ptr[14] == 0x03))
        {
            /* llid index of boardcast oam packet is 0x08, 0x0A(10) or 0x0B(11), it must be changed to llid 0 */
            if(llidIdx == 0x08 || llidIdx == 0x0A || llidIdx == 0x0B || llidIdx == 0x3f)
            {
                llidIdx = 0;
            }
            else
            {
                aal_mpcp_reg_sw_index_offset_get(0,&sw_idx);
                if(llidIdx > sw_idx)
                    llidIdx = llidIdx-sw_idx;
                else
                    llidIdx = 0;
            }

            if(gOamRxCallback)
            {
                gOamRxCallback(oam_len, oam_ptr, llidIdx);
            }

            return RE8670_RX_STOP;
        }
    }
    return RE8670_RX_CONTINUE;
}

int
_dal_ca8277b_epon_is_inited(void)
{
   return epon_init;
}


/* Function Name:
 *      dal_rt_ca8277b_epon_init
 * Description:
 *      epon register level initial function
 * Input:
 *
 * Output:
 *      
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 
dal_rt_ca8277b_epon_init(void)
{
	int ret,q =0;
    rtk_ponmac_queue_t queue;
    rtk_ponmac_queueCfg_t queueCfg;

    uint16 llidIdx = 0;

    gPonMode = aal_pon_mac_mode_get(0);
    if(!CA_PON_MODE_IS_EPON_FAMILY(gPonMode))
    {
        RT_ERR(ret, (MOD_DAL|MOD_GPON), "");
        printk("PON MODE 0x%d is note correct \n", gPonMode);
        epon_init = INIT_NOT_COMPLETED;
        return RT_ERR_FAILED;
    }

    if(aal_init_pon == 0)
    {
        /* load other pon related scfg settings */
        ret =aal_pon_scfg_load(0);
        if(CA_E_OK != ret) {
            printk("%s:%d %s returns error, ret = %d\n",__FUNCTION__, __LINE__, "aal_pon_scfg_load()", ret);
            epon_init = INIT_NOT_COMPLETED;
            return RT_ERR_FAILED;
        }

        /* PONMAC init */
        ret = ca_pon_init(0);
        if(CA_E_OK != ret) {
            printk("%s:%d %s returns error, ret = %d\n",__FUNCTION__, __LINE__, "ca_pon_init()", ret);
            epon_init = INIT_NOT_COMPLETED;
            return RT_ERR_FAILED;
        }
    }

    if((ret = dal_rt_ca8277b_ponmisc_tcont_llid_logic2phy_get(0,&llidIdx) != RT_ERR_OK))
    {
        printk("%s %d get phy schedler ID:%u fail\n",__FUNCTION__,__LINE__);
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    aal_mpcp_reg_sw_index_offset_set(0,llidIdx);

    if((ret = dal_ca8277b_trap_oamPduAction_set(ACTION_TRAP2CPU)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        epon_init = INIT_NOT_COMPLETED;
        return RT_ERR_OK;
    }
    dal_ca8277b_stat_port_reset(HAL_GET_PON_PORT());

	epon_init = INIT_COMPLETED;
    return RT_ERR_OK;    
} /* end of dal_rt_ca8277b_epon_init */

/* Function Name:
 *      dal_rt_ca8277b_epon_oam_tx
 * Description:
 *      Transmit OAM message.
 * Input:
 *      msgLen   - length of the OAM message to be transmitted
 *      pMsg     - pointer to the OAM message data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32
dal_rt_ca8277b_epon_oam_tx(uint32 msgLen, uint8 *pMsg, uint8 llidIdx)
{
#ifndef CONFIG_RTK_OAM_BY_BUFFER
    ca_pkt_t halPkt;
    ca_pkt_block_t pkt_data;
    int ret;
    unsigned char tmp[60];
    ca_uint8_t sw_idx;

    /* check Init status */
    RT_INIT_CHK(epon_init);

    memset(&halPkt,0,sizeof(halPkt));
    halPkt.block_count    = 1;
    halPkt.cos            = 8;
    halPkt.device_id      = 0;
    halPkt.src_port_id    = CA_PORT_ID(CA_PORT_TYPE_CPU, 0x10);
    halPkt.dst_port_id    = CA_PORT_ID(CA_PORT_TYPE_EPON, 7);
    halPkt.pkt_type       = CA_PKT_TYPE_OAM;
    halPkt.pkt_len        = msgLen;
    halPkt.pkt_data       = (ca_pkt_block_t *)&pkt_data;
    halPkt.pkt_data->len  = msgLen;
    halPkt.pkt_data->data = pMsg;
    halPkt.flags          = CA_TX_BYPASS_FWD_ENGINE;
    aal_mpcp_reg_sw_index_offset_get(0,&sw_idx);
    halPkt.dst_sub_port_id = sw_idx;

    if(msgLen < 60)
    {
        memset(tmp,0,60);
        memcpy(tmp,pMsg,msgLen);
        halPkt.pkt_len        = 60;
        halPkt.pkt_data->len  = 60;
        halPkt.pkt_data->data = tmp;
    }

    if(memcmp(pMsg,oam_da,6))
    {
        printk("%s %d : Not OAM DA no send\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if ((ret = __ca_tx(0, &halPkt)) != CA_E_OK)
    {
        printk("%s %d : TX OAM Error!!\n",__FUNCTION__,__LINE__);
    }

    return RT_ERR_OK;
#else
    ca_uint32_t ret = CA_E_OK;
    RT_INIT_CHK(epon_init);
    if((ret = aal_epon_tx_swframe_set(0,EPON_TX_SW_FRAME_OAM,llidIdx,pMsg,msgLen)) != CA_E_OK)
    {
        printk("%s %d : TX OAM Error!!\n",__FUNCTION__,__LINE__);
        return ;
    }
#endif

} /* end of dal_rt_ca8277b_epon_oam_tx */

/* Function Name:
 *      dal_rt_ca8277b_epon_oam_rx_callback_register
 * Description:
 *      Register OAM RX callback function.
 * Input:
 *      oamRx   - pointer to the callback function for OAM RX
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32
dal_rt_ca8277b_epon_oam_rx_callback_register(rt_epon_oam_rx_callback oamRx)
{
    int32   ret;
    rt_port_t pon;
    int ponPortMask;

    /* check Init status */
    RT_INIT_CHK(epon_init);

    pon = HAL_GET_PON_PORT();
    ponPortMask = 1 << pon;

    /* check Init status */
    RT_INIT_CHK(epon_init);

    /* function body */

    if(DISABLED == gOamTrapEnable)
    {
        if((ret = drv_nic_register_rxhook(ponPortMask,7,&_oam_receive_from_nic))!=RT_ERR_OK)
        {
            printk("drv_nic_register_rxhook Error!!\n");
            return ret;
        }
        gOamTrapEnable = ENABLED;
    }

    gOamRxCallback = oamRx;
    
    return RT_ERR_OK;
}   /* end of dal_rt_ca8277b_epon_oam_rx_callback_register */

/* Function Name:
 *      dal_rt_ca8277b_epon_dyinggasp_set
 * Description:
 *      Transmit dying gasp OAM message.
 * Input:
 *      msgLen   - length of the dying gasp OAM message to be transmitted
 *      pMsg     - pointer to the dying gasp OAM message data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
int32
dal_rt_ca8277b_epon_dyinggasp_set(uint32 msgLen, uint8 *pMsg)
{
    ca_uint32_t ret = CA_E_OK;
    ca_uint8_t send_count;

    /* check Init status */
    RT_INIT_CHK(epon_init);

    send_count = 3;

    if((ret = aal_epon_dying_gasp_set(0,7,send_count,pMsg,msgLen)) != CA_E_OK)
    {
        printk(0,"%s %d : aal_epon_dying_gasp_set Error!!",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;

} /* end of dal_rt_ca8277b_epon_dyinggasp_set */

/* Function Name:
 *      dal_rt_ca8277b_epon_llid_entry_get
 * Description:
 *      Get llid entry
 * Input:
 *      llidIdx: LLID table index
 * Output:
 *      pLlidEntry: LLID entry 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_rt_ca8277b_epon_llid_entry_get(rt_epon_llid_entry_t *pLlidEntry)  
{
    ca_status_t ret=CA_E_OK;
    rt_port_t pon_port;
    ca_port_id_t port_id;
    ca_llid_t  llid;
    ca_mac_addr_t mac;
    ca_boolean_t reg_flag;
    ca_uint16_t reg_llid;
    rt_enable_t losState = ENABLED;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "");

    /* check Init status */
    RT_INIT_CHK(epon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pLlidEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((RTK_EPON_MAX_LLID <= pLlidEntry->llidIdx), RT_ERR_INPUT);
    
    /* function body */
    pon_port = HAL_GET_PON_PORT();
    port_id = CA_PORT_ID(CA_PORT_TYPE_EPON,pon_port);
    llid = pLlidEntry->llidIdx;

    __PON_LOCK();
    ret = aal_mpcp_mac_addr_get(0,port_id,llid,mac);
    __PON_UNLOCK();
    if(ret != CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        return RT_ERR_FAILED;
    }

    memcpy(pLlidEntry->mac.octet,mac,6);

    ret = dal_rt_ca8277b_epon_losState_get(&losState);
    if(ret != CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        return RT_ERR_FAILED;
    }

    if(losState == ENABLED)
    {
        pLlidEntry->llid = 0x7fff;
        pLlidEntry->valid = DISABLED;
        pLlidEntry->reportTimer = 0;
        pLlidEntry->isReportTimeout = 0;

        return RT_ERR_OK;   
    }
    
    __PON_LOCK();
    ret = aal_mpcp_reg_status_get(0, port_id, llid, &reg_flag);
    __PON_UNLOCK();
    if(ret != CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        return RT_ERR_FAILED;
    }
    
    if(reg_flag == CA_TRUE)
    {
        __PON_LOCK();
        ret = aal_mpcp_reg_llid_get(0,port_id,llid, (ca_uint16_t*)&(reg_llid));
        __PON_UNLOCK();
        if(ret == CA_E_NOT_FOUND)
        {
            pLlidEntry->llid = 0x7fff;
            pLlidEntry->valid = DISABLED;
        }
        else if(ret == CA_E_OK)
        {
            pLlidEntry->llid = reg_llid;
            pLlidEntry->valid = ENABLED;
        }
        else
        {
            RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
            return RT_ERR_FAILED;
        }
    }
    else
    {
        pLlidEntry->llid = 0x7fff;
        pLlidEntry->valid = DISABLED;
    }

    pLlidEntry->reportTimer = 0;
    pLlidEntry->isReportTimeout = 0;

    return RT_ERR_OK;
}   /* end of dal_rt_ca8277b_epon_llid_entry_get */

/* Function Name:
 *      dal_rt_ca8277b_epon_llid_entry_set
 * Description:
 *      Set llid entry
 * Input:
 *      llidIdx: LLID table index
 * Output:
 *      pLlidEntry: LLID entry 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_rt_ca8277b_epon_llid_entry_set(rt_epon_llid_entry_t *pLlidEntry)  
{
    ca_status_t ret=CA_E_OK;
    rt_port_t pon_port;
    ca_port_id_t port_id;
    ca_llid_t  llid;
    ca_boolean_t reg_flag;
    ca_mac_addr_t mac;
    ca_boolean_t enable;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "");

    /* parameter check */
    RT_PARAM_CHK((NULL == pLlidEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((RTK_EPON_MAX_LLID <= pLlidEntry->llidIdx), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= pLlidEntry->valid), RT_ERR_INPUT);

    /* function body */
    pon_port = HAL_GET_PON_PORT();
    port_id = CA_PORT_ID(CA_PORT_TYPE_EPON,pon_port);
    llid = pLlidEntry->llidIdx;

    if(pLlidEntry->valid == DISABLED)
    {
        printk(KERN_INFO "Disable register llid-idx %d\n",llid);
        if((ret = ca_epon_mpcp_registration_set(0, port_id, (ca_uint8_t)llid, 0))!=CA_E_OK)
        {
            if(ret != CA_E_NOT_FOUND)
            {
                RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
                return RT_ERR_FAILED;
            }
        }        
    }

    if((ret = ca_epon_mpcp_registration_get(0, port_id, (ca_uint8_t)llid, &enable))!=CA_E_OK)
    {
        if(ret != CA_E_NOT_FOUND)
        {
            RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
            return RT_ERR_FAILED;
        }
    }

    if(enable == 0)
    {
        if(llid == 0)
        {
            __PON_LOCK();
            ret = aal_mpcp_mac_addr_get(0,port_id,llid,mac);
            __PON_UNLOCK();
            if(ret != CA_E_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
                return RT_ERR_FAILED;
            }

            if(memcmp(pLlidEntry->mac.octet,mac,6))
            {
                memcpy(mac,pLlidEntry->mac.octet,6);
                __PON_LOCK();
                ret = aal_mpcp_mac_addr_set(0,port_id,llid,mac);
                __PON_UNLOCK();
                if(ret != CA_E_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
                    return RT_ERR_FAILED;
                }
            }
        }
    }

    return RT_ERR_OK;
}   /* end of dal_rt_ca8277b_epon_llid_entry_set */

/* Function Name:
 *      dal_rt_ca8277b_epon_registerReq_get
 * Description:
 *      Get register request relative parameter
 * Input:
 *      pRegEntry : register request relative parament
 * Output:
 *       None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_rt_ca8277b_epon_registerReq_get(rt_epon_regReq_t *pRegEntry)
{
    ca_status_t ret=CA_E_OK;
    rt_port_t pon_port;
    ca_port_id_t port_id;
    ca_llid_t  llid;
    ca_uint32_t pending_grants;
    ca_boolean_t enable;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "");

    /* check Init status */
    RT_INIT_CHK(epon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pRegEntry), RT_ERR_NULL_POINTER);

    /* function body */
    pRegEntry->llidIdx = regLlidIdx;

    pon_port = HAL_GET_PON_PORT();
    port_id = CA_PORT_ID(CA_PORT_TYPE_EPON,pon_port);

    if((ret = ca_epon_mpcp_pending_grants_get(0,port_id,&pending_grants))!=CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        return RT_ERR_FAILED;
    }

    pRegEntry->pendGrantNum = pending_grants;

    llid = pRegEntry->llidIdx;

    if((ret = ca_epon_mpcp_registration_get(0,port_id,llid,&enable))!=CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        return RT_ERR_FAILED;
    }

    if(enable == 1)
        pRegEntry->doRequest = ENABLED;
    else
        pRegEntry->doRequest = DISABLED;

    return RT_ERR_OK;
}   /* end of dal_rt_ca8277b_epon_registerReq_get */

/* Function Name:
 *      dal_rt_ca8277b_epon_registerReq_set
 * Description:
 *      Set register request relative parameter
 * Input:
 *       None
 * Output:
 *       pRegEntry : register request relative parament 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_rt_ca8277b_epon_registerReq_set(rt_epon_regReq_t *pRegEntry)
{
    ca_status_t ret=CA_E_OK;
    rt_port_t pon_port;
    ca_port_id_t port_id;
    ca_llid_t  llid;
    ca_uint32_t pre_pending_grants;
    ca_uint32_t pending_grants;
    ca_boolean_t enable;
    ca_boolean_t silence_status = FALSE;
    ca_boolean_t reg_flag;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "");

    /* check Init status */
    RT_INIT_CHK(epon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pRegEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((RTK_EPON_MAX_LLID <= pRegEntry->llidIdx), RT_ERR_INPUT);

    /* function body */
    pon_port = HAL_GET_PON_PORT();
    port_id = CA_PORT_ID(CA_PORT_TYPE_EPON,pon_port);
    llid = pRegEntry->llidIdx;
    regLlidIdx = pRegEntry->llidIdx;
    pending_grants = pRegEntry->pendGrantNum;


    if((ret = ca_epon_mpcp_pending_grants_get(0,port_id,&pre_pending_grants))!=CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        return RT_ERR_FAILED;
    }

    if(pre_pending_grants != pending_grants)
    {
        if((ret = ca_epon_mpcp_pending_grants_set(0,port_id,pending_grants))!=CA_E_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
            return RT_ERR_FAILED;
        }
    }

    if(pRegEntry->doRequest == ENABLED)
    {
        printk(KERN_INFO "Enable register llid-idx %d\n",llid);
        if((ret = ca_epon_mpcp_registration_set(0, port_id, (ca_uint8_t)llid, 1))!=CA_E_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
            return RT_ERR_FAILED;
        }
    }

    return RT_ERR_OK;
}   /* end of dal_rt_ca8277b_epon_registerReq_set */

/* Function Name:
 *      dal_rt_ca8277b_epon_churningKey_get
 * Description:
 *      Get churning key entry
 * Input:
 *       None
 * Output:
 *       pEntry : churning key relative parameter 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_rt_ca8277b_epon_churningKey_get(rt_epon_churningKeyEntry_t *pEntry)
{
    ca_status_t ret=CA_E_OK;
    rt_port_t pon_port;
    ca_port_id_t port_id;
    ca_llid_t llid;
    ca_uint32_t key_id;
    ca_epon_port_encryption_key_t key;
    ca_uint32_t pon_mode;


    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "");

    /* check Init status */
    RT_INIT_CHK(epon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((2 < pEntry->keyIdx), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_EPON_MAX_LLID <= pEntry->llidIdx), RT_ERR_INPUT);

    /* function body */
    pon_port = HAL_GET_PON_PORT();
    port_id = CA_PORT_ID(CA_PORT_TYPE_EPON,pon_port);
    llid = pEntry->llidIdx;
    key_id = pEntry->keyIdx;

    if((ret = ca_epon_llid_encryption_key_get(0,port_id,llid,key_id,&key))!=CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        return RT_ERR_FAILED;
    }
    if(pon_mode == ONU_PON_MAC_EPON_1G)
    {
        pEntry->churningKey[0]=key.data[0];
        pEntry->churningKey[1]=key.data[1];
        pEntry->churningKey[2]=key.data[2];
        pEntry->churningKey1[0]=key.data[3];        
    
    }
    else
    {    
        pEntry->churningKey[0]=key.data[0];
        pEntry->churningKey[1]=key.data[1];
        pEntry->churningKey[2]=key.data[2];
        pEntry->churningKey1[0]=key.data[3];
        pEntry->churningKey1[1]=key.data[4];
        pEntry->churningKey1[2]=key.data[5];
        pEntry->churningKey2[0]=key.data[6];
        pEntry->churningKey2[1]=key.data[7];
        pEntry->churningKey2[2]=key.data[8];
    }


    return RT_ERR_OK;
}   /* end of dal_rt_ca8277b_epon_churningKey_get */

/* Function Name:
 *      dal_rt_ca8277b_epon_churningKey_set
 * Description:
 *      Set churning key entry
 * Input:
 *       pEntry : churning key relative parameter 
 * Output:
 *       None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_rt_ca8277b_epon_churningKey_set(rt_epon_churningKeyEntry_t *pEntry)
{
    ca_status_t ret=CA_E_OK;
    rt_port_t pon_port;
    ca_port_id_t port_id;
    ca_llid_t llid;
    ca_uint32_t key_id;
    ca_epon_port_encryption_key_t key;
    ca_uint32_t pon_mode = CA_PON_MODE ;


    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "");

    /* check Init status */
    RT_INIT_CHK(epon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((2 < pEntry->keyIdx), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_EPON_MAX_LLID <= pEntry->llidIdx), RT_ERR_INPUT);
    /* function body */
    pon_port = HAL_GET_PON_PORT();
    port_id = CA_PORT_ID(CA_PORT_TYPE_EPON,pon_port);
    llid = pEntry->llidIdx;
    key_id = pEntry->keyIdx;

    if(pon_mode != ONU_PON_MAC_EPON_1G)
    {

        if((ret = ca_port_encryption_enable_set(0,port_id,CA_DIRECTION_ENC_RX,1))!=CA_E_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
            return RT_ERR_FAILED;
        }
    }

    if(pon_mode == ONU_PON_MAC_EPON_1G)
    {
        key.data[0]=pEntry->churningKey[2];
        key.data[1]=pEntry->churningKey[1];
        key.data[2]=pEntry->churningKey[0];
        key.data[3]=0;
    }
    else
    {
        key.data[0]=pEntry->churningKey[0];
        key.data[1]=pEntry->churningKey[1];
        key.data[2]=pEntry->churningKey[2];
        key.data[3]=pEntry->churningKey1[0];
        key.data[4]=pEntry->churningKey1[1];
        key.data[5]=pEntry->churningKey1[2];
        key.data[6]=pEntry->churningKey2[0];
        key.data[7]=pEntry->churningKey2[1];
        key.data[8]=pEntry->churningKey2[2];
    }
    
    if((ret = ca_epon_llid_encryption_key_set(0,port_id,llid,key_id,&key))!=CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}   /* end of dal_rt_ca8277b_epon_churningKey_set */

/* Function Name:
 *      dal_rt_ca8277b_epon_usFecState_get
 * Description:
 *      Get upstream fec state
 * Input:
 *       None
 * Output:
 *       *pState : upstream FEC state 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_rt_ca8277b_epon_usFecState_get(rt_enable_t *pState)
{
    ca_status_t ret=CA_E_OK;
    rt_port_t pon_port;
    ca_port_id_t port_id;
    ca_boolean_t tx_enable;
    ca_boolean_t rx_enable;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "");

    /* check Init status */
    RT_INIT_CHK(epon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */
    pon_port = HAL_GET_PON_PORT();
    port_id = CA_PORT_ID(CA_PORT_TYPE_EPON,pon_port);

    if((ret = ca_epon_port_fec_enable_get(0,port_id,&tx_enable,&rx_enable))!=CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        return RT_ERR_FAILED;
    }

    if(tx_enable == 1)
        *pState = ENABLED;
    else
        *pState = DISABLED;

    return RT_ERR_OK;
}   /* end of dal_rt_ca8277b_epon_usFecState_get */

/* Function Name:
 *      dal_rt_ca8277b_epon_usFecState_set
 * Description:
 *      Set upstream fec state
 * Input:
 *       state : upstream FEC state
 * Output:
 *       None 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_rt_ca8277b_epon_usFecState_set(rt_enable_t state)
{
    ca_status_t ret=CA_E_OK;
    rt_port_t pon_port;
    ca_port_id_t port_id;
    ca_boolean_t tx_enable;
    ca_boolean_t rx_enable;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "state=%d",state);

    /* check Init status */
    RT_INIT_CHK(epon_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= state), RT_ERR_INPUT);

    /* function body */
    pon_port = HAL_GET_PON_PORT();
    port_id = CA_PORT_ID(CA_PORT_TYPE_EPON,pon_port);

    if((ret = ca_epon_port_fec_enable_get(0,port_id,&tx_enable,&rx_enable))!=CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        return RT_ERR_FAILED;
    }

    if(state == ENABLED)
        tx_enable = 1;
    else
        tx_enable = 0;

    if((ret = ca_epon_port_fec_enable_set(0,port_id,tx_enable,rx_enable))!=CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        return RT_ERR_FAILED;
    }

    return ret;
}   /* end of dal_rt_ca8277b_epon_usFecState_set */

/* Function Name:
 *      dal_rt_ca8277b_epon_dsFecState_get
 * Description:
 *      Get down-stream fec state
 * Input:
 *       None
 * Output:
 *       *pState : down-stream FEC state 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_rt_ca8277b_epon_dsFecState_get(rt_enable_t *pState)
{
    ca_status_t ret=CA_E_OK;
    rt_port_t pon_port;
    ca_port_id_t port_id;
    ca_boolean_t tx_enable;
    ca_boolean_t rx_enable;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "");

    /* check Init status */
    RT_INIT_CHK(epon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */
    pon_port = HAL_GET_PON_PORT();
    port_id = CA_PORT_ID(CA_PORT_TYPE_EPON,pon_port);

    if((ret = ca_epon_port_fec_enable_get(0,port_id,&tx_enable,&rx_enable))!=CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        return RT_ERR_FAILED;
    }

    if(rx_enable == 1)
        *pState = ENABLED;
    else
        *pState = DISABLED;

    return RT_ERR_OK;
}   /* end of dal_rt_ca8277b_epon_dsFecState_get */

/* Function Name:
 *      dal_rt_ca8277b_epon_dsFecState_set
 * Description:
 *      Set down-stream fec state
 * Input:
 *       state : down-stream FEC state
 * Output:
 *       None 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_rt_ca8277b_epon_dsFecState_set(rt_enable_t state)
{
    ca_status_t ret=CA_E_OK;
    rt_port_t pon_port;
    ca_port_id_t port_id;
    ca_boolean_t tx_enable;
    ca_boolean_t rx_enable;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "state=%d",state);

    /* check Init status */
    RT_INIT_CHK(epon_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= state), RT_ERR_INPUT);

    /* function body */
    pon_port = HAL_GET_PON_PORT();
    port_id = CA_PORT_ID(CA_PORT_TYPE_EPON,pon_port);

    if((ret = ca_epon_port_fec_enable_get(0,port_id,&tx_enable,&rx_enable))!=CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        return RT_ERR_FAILED;
    }

    if(state == ENABLED)
        rx_enable = 1;
    else
        rx_enable = 0;

    if((ret = ca_epon_port_fec_enable_set(0,port_id,tx_enable,rx_enable))!=CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}   /* end of dal_rt_ca8277b_epon_dsFecState_set */

/* Function Name:
 *      dal_rt_ca8277b_epon_mibCounter_get
 * Description:
 *      Get EPON MIB Counter
 * Input:
 *       None
 * Output:
 *       pCounter : EPON mib counter 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_rt_ca8277b_epon_mibCounter_get(rt_epon_counter_t *pCounter)
{
    ca_status_t ret=CA_E_OK;
    rt_port_t pon_port;
    ca_port_id_t port_id;
    ca_epon_port_stats_t stats;
    ca_epon_port_fec_stats_t fec_stats;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "state=%d",state);

    /* check Init status */
    RT_INIT_CHK(epon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pCounter), RT_ERR_NULL_POINTER);

    /* function body */
    pon_port = HAL_GET_PON_PORT();
    port_id = CA_PORT_ID(CA_PORT_TYPE_EPON,pon_port);

    if((ret = ca_epon_port_stats_get(0,port_id,1,&stats))!=CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        return RT_ERR_FAILED;
    }

    if((ret = ca_epon_port_fec_stats_get(0,port_id,1,&fec_stats))!=CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        return RT_ERR_FAILED;
    }

    memset(&pCounter->llidIdxCnt,0,sizeof(rt_epon_llidCounter_t));
    
    pCounter->mpcpRxDiscGate = stats.mpcp_rx_bc_gate;
    pCounter->fecCorrectedBlocks = fec_stats.corrected_codewords;
    pCounter->fecUncorrectedBlocks = fec_stats.uncorrectable_codewords;
    pCounter->fecCodingVio = 0;
    pCounter->notBcstBitLlid7fff =0;
    pCounter->notBcstBitNotOnuLlid =0;
    pCounter->bcstBitPlusOnuLLid =0;
    pCounter->bcstNotOnuLLid =0;
    pCounter->crc8Err = stats.rx_crc8_sld_err;
    pCounter->mpcpTxRegRequest = stats.mpcp_tx_bc_reg_req;
    pCounter->mpcpTxRegAck = stats.mpcp_tx_reg_ack;

    return RT_ERR_OK;
}   /* end of dal_rt_ca8277b_epon_mibCounter_get */

/* Function Name:
 *      dal_rt_ca8277b_epon_losState_get
 * Description:
 *      Get laser lose of signal state.
 * Input:
 *      pState LOS state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32 dal_rt_ca8277b_epon_losState_get(rt_enable_t *pState)
{
    ca_status_t ret=CA_E_OK;
    rt_port_t pon_port;
    ca_port_id_t port_id;
    aal_epon_status_t status;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "");

    /* check Init status */
    RT_INIT_CHK(epon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */
    pon_port = HAL_GET_PON_PORT();
    port_id = CA_PORT_ID(CA_PORT_TYPE_EPON,pon_port);

    __PON_LOCK();
    ret = aal_epon_status_get(0,port_id,&status);
    __PON_UNLOCK();
    if(ret != CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        return RT_ERR_FAILED;
    }

    if(status.sig_loss == 1)
        *pState = ENABLED;
    else
        *pState = DISABLED;
    
    return RT_ERR_OK;
}   /* end of dal_rt_ca8277b_epon_losState_get */
