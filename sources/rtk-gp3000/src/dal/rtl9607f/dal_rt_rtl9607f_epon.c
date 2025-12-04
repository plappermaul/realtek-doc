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
#include <dal/rtl9607f/dal_rtl9607f.h>
#include <dal/rtl9607f/dal_rt_rtl9607f_epon.h>
#include <dal/rtl9607f/dal_rt_rtl9607f_ponmisc.h>
#include <dal/rtl9607f/dal_rtl9607f_trap.h>

#include "osal_spinlock.h"
#include <cortina-api/include/pkt_tx.h>
#include <cortina-api/include/epon.h>

#include <aal_pon.h>
#include <aal_puc.h>
#include <aal_pdc.h>


#include "ni-drv-07f/ca_ni.h"
#include "ca-ne-rtk/ca_ext.h"
#include <ca_event.h>


extern ca_uint32_t aal_init_pon;
extern ca_status_t ca_pon_init(ca_device_id_t device_id);
extern ca_status_t ca_pon_exit(ca_device_id_t device_id);

uint32 epon_init = {INIT_NOT_COMPLETED};

static uint8 regLlidIdx = 0;

static ca_uint32_t gPonMode;
static ca_mac_addr_t puc_da = {0x00, 0x13, 0x25, 0x00, 0x00, 0x00};
static ca_mac_addr_t puc_sa = {0x00, 0x13, 0x25, 0x00, 0x00, 0x01};

static ca_int8_t etype_oam[]  = {0xff, 0xf1};
static ca_int8_t etype_vlan[] = {0x81, 0x00};

static ca_mac_addr_t oam_da = {0x01, 0x80, 0xc2, 0x00, 0x00, 0x02};

static rt_enable_t gOamTrapEnable = DISABLED; //Default disable trap OAM packet
static rt_epon_oam_rx_callback gOamRxCallback = NULL;
static rt_epon_info_notify_cb dal_rt_rtl9607f_epon_info_callback = NULL;



int nic_register_rxhook(int portmask,int priority,p2rfunc_t rx);
static void _dal_rt_rtl9607f_epon_infoChange_notify(rtk_epon_info_notify_t info);



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



void _dal_rtl9607f_epon_infoChange_notify(rt_epon_info_notify_t info)
{
    if(NULL!=dal_rt_rtl9607f_epon_info_callback)
        dal_rt_rtl9607f_epon_info_callback(info);
}




ca_uint32_t dal_rt_rtl9607f_epon_register_status_intr(ca_device_id_t device_id,ca_uint32_t event_id,ca_void_t *event,ca_uint32_t len,ca_void_t *cookie)
{
    ca_event_reg_stat_t* evt_ptr = (ca_event_reg_stat_t *)event;
    rt_epon_info_notify_t eventInfo;

    memset(&eventInfo,0x0,sizeof(rt_epon_info_notify_t));

    if(evt_ptr->status == 1)
    {
        printk("llid-idx %d llid %d register\n",evt_ptr->llid_idx,evt_ptr->llid_value);
        eventInfo.llidIdx=0;
        eventInfo.event=RT_EPON_INFO_MPCP_REG_SUCCESS;
        _dal_rtl9607f_epon_infoChange_notify(eventInfo);    
    }
    else
    {
        printk("llid-idx %d de-register cause:%d\n",evt_ptr->llid_idx,evt_ptr->dereg_cause);
        
        if(evt_ptr->dereg_cause==AAL_MPCP_DEREG_SW)
        {        
                eventInfo.llidIdx=0;
                eventInfo.event=RT_EPON_INFO_MPCP_LLID_ENTRY_DISABLE;
                _dal_rtl9607f_epon_infoChange_notify(eventInfo);    
        }
        else
        {        
            eventInfo.llidIdx=0;
            eventInfo.event=RT_EPON_INFO_MPCP_REG_FAIL;
            _dal_rtl9607f_epon_infoChange_notify(eventInfo);    
        }
    }

    return 0;
}




ca_uint32_t dal_rt_rtl9607f_epon_link_intr(ca_device_id_t device_id,ca_uint32_t event_id,ca_void_t *event,ca_uint32_t len,ca_void_t *cookie)
{
    ca_event_epon_port_link_t* evt_ptr = (ca_event_epon_port_link_t *)event;
    rt_epon_info_notify_t eventInfo;

    memset(&eventInfo,0x0,sizeof(rt_epon_info_notify_t));

    if(evt_ptr->port_id == CA_PORT_ID(CA_PORT_TYPE_EPON, CA_PORT_ID_NI7))
    {
        if(evt_ptr->status == 1)
        {
            printk("epon port link up\n");
            eventInfo.llidIdx=0;
            eventInfo.event=RT_EPON_INFO_LASER_RECOVER;
            _dal_rtl9607f_epon_infoChange_notify(eventInfo);    
        }
        else
        {
            printk("epon port link down\n");
            eventInfo.llidIdx=0;
            eventInfo.event=RT_EPON_INFO_LASER_OFF;
            _dal_rtl9607f_epon_infoChange_notify(eventInfo);    

        }
    }

    return 0;
}




ca_uint32_t dal_rt_rtl9607f_epon_mpcp_event_intr(ca_device_id_t device_id,ca_uint32_t event_id,ca_void_t *event,ca_uint32_t len,ca_void_t *cookie)
{
    ca_event_epon_mpcp_event_t* evt_ptr = (ca_event_epon_mpcp_event_t *)event;
    rt_epon_info_notify_t eventInfo;

    memset(&eventInfo,0x0,sizeof(rt_epon_info_notify_t)); 

    //if(evt_ptr->port_id == CA_PORT_ID(CA_PORT_TYPE_EPON, CA_PORT_ID_NI7))
    {
        printk("CA_EVENT_EPON_MPCP_EVENT: %d\n",evt_ptr->event_id);
        switch(evt_ptr->event_id)
        {
            case AAL_EPON_MPCP_GATE_TIMEOUT:
                eventInfo.llidIdx=0;
                eventInfo.event=RT_EPON_INFO_MPCP_GATE_TIMEOUT;
                _dal_rtl9607f_epon_infoChange_notify(eventInfo);               
                break;                
            case AAL_EPON_MPCP_TIMEDRIFT:
                eventInfo.llidIdx=0;
                eventInfo.event=RT_EPON_INFO_MPCP_TIMEDRIFT;
                _dal_rtl9607f_epon_infoChange_notify(eventInfo);               
                break;                
            default:
                printk("CA_EVENT_EPON_MPCP_EVENT unknown!\n");
                break;            
        }
    }

    return 0;
}


ca_uint32_t dal_rt_rtl9607f_epon_mpcp_register_rx_intr(ca_device_id_t device_id,ca_uint32_t event_id,ca_void_t *event,ca_uint32_t len,ca_void_t *cookie)
{
    ca_event_epon_mpcp_reg_rx_event_t* evt_ptr = (ca_event_epon_mpcp_reg_rx_event_t *)event;
    rt_epon_info_notify_t eventInfo;

    printk("CA_EVENT_EPON_MPCP_REG_RX  flag:%d\n",evt_ptr->flag);
    
    eventInfo.llidIdx=0;
    eventInfo.event=RT_EPON_INFO_MPCP_RX_REG_PKT;
    eventInfo.info = evt_ptr->flag;
    _dal_rtl9607f_epon_infoChange_notify(eventInfo);               
    
    return 0;
}

/* Function Name:
 *      dal_rt_rtl9607f_epon_deinit
 * Description:
 *      epon register level de-initial function
 * Input:
 *      pon_mode    - current pon mode to be de-inited
 * Output:
 *      
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 
dal_rt_rtl9607f_epon_deinit(uint8 pon_mode)
{
    int ret;
    uint16 llidIdx = 0;
    ca_uint32_t ppid = 0;

    gPonMode = aal_pon_mac_mode_get(0);
    if(!CA_PON_MODE_IS_EPON_FAMILY(gPonMode))
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        printk("PON MODE 0x%d is note correct \n", gPonMode);
        epon_init = INIT_NOT_COMPLETED;
        return RT_ERR_FAILED;
    }

    if(epon_init!=INIT_COMPLETED)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        printk("EPON is note initialized! \n");
        epon_init = INIT_NOT_COMPLETED;
        return RT_ERR_FAILED;
    }

    if(aal_init_pon != 0)
    {
        /* PONMAC de-init */
        ret = ca_pon_exit(0);
        if(CA_E_OK != ret) {
            printk("%s:%d %s returns error, ret = %d\n",__FUNCTION__, __LINE__, "ca_pon_init()", ret);
            epon_init = INIT_NOT_COMPLETED;
            return RT_ERR_FAILED;
        }
    }

    /*deregister event Notify*/
        /*deregister status change*/        
    if((ret = ca_event_deregister(0,CA_EVENT_EPON_REG_STAT_CHNG,dal_rt_rtl9607f_epon_register_status_intr))!=CA_E_OK)
    {
        printk("EPON dal_rt_rtl9607f_epon_init event epon stat changing event deregister Error!!\n");
    }
        /*laser link status change*/        
    if((ret = ca_event_deregister(0,CA_EVENT_EPON_PORT_LINK,dal_rt_rtl9607f_epon_link_intr))!=CA_E_OK)
    {
        printk("EPON dal_rt_rtl9607f_epon_init  event epon port link deregister Error!!\n");
    }    
        /*mpcp event handle*/
    if((ret = ca_event_deregister(0,CA_EVENT_EPON_MPCP_EVENT,dal_rt_rtl9607f_epon_mpcp_event_intr))!=CA_E_OK)
    {
        printk("EPON dal_rt_rtl9607f_epon_init  event CA_EVENT_EPON_MPCP_EVENT deregister Error!!\n");
    }          
        /*register packet rx notify*/
    if((ret = ca_event_deregister(0,CA_EVENT_EPON_MPCP_REG_RX,dal_rt_rtl9607f_epon_mpcp_register_rx_intr))!=CA_E_OK)
    {
        printk("EPON dal_rt_rtl9607f_epon_init  event CA_EVENT_EPON_MPCP_REG_RX deregister Error!!\n");
    }          

   /* Reset PONMAC */
   ret = aal_pon_deinit(0, pon_mode);

    epon_init = INIT_NOT_COMPLETED;

    return RT_ERR_OK;    
}


/* Function Name:
 *      dal_rt_rtl9607f_epon_init
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
dal_rt_rtl9607f_epon_init(void)
{
	int ret;

    uint16 llidIdx = 0;

#if defined(CONFIG_CORTINA_BOARD_FPGA) || defined(CONFIG_REALTEK_BOARD_FPGA)
    aal_init_debug_set(0,2,0);
    mdelay(1000);
//    aal_init_debug_set(0,3,2);
//    mdelay(1000);
    
    dal_rt_rtl9607f_ponmisc_mode_set(RT_EPON_MODE,RT_1G25G_SPEED);
#endif

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

    if((ret = dal_rt_rtl9607f_ponmisc_tcont_llid_logic2phy_get(0,&llidIdx) != RT_ERR_OK))
    {
        printk("%s %d get phy schedler ID:%u fail\n",__FUNCTION__,__LINE__,llidIdx);
        RT_ERR(ret, (MOD_PONMAC | MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    aal_mpcp_reg_sw_index_offset_set(0,llidIdx);

    if((ret = dal_rtl9607f_trap_oamPduAction_set(ACTION_TRAP2CPU)) != RT_ERR_OK)
    {
        printk("%s %d trap OAM error, ret = %d\n",__FUNCTION__,__LINE__,ret);
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
#if defined(CONFIG_CORTINA_BOARD_FPGA) || defined(CONFIG_REALTEK_BOARD_FPGA)
        /*nothing*/
#else
        epon_init = INIT_NOT_COMPLETED;
        return RT_ERR_FAILED;
#endif
    }

#if defined(CONFIG_CORTINA_BOARD_FPGA) || defined(CONFIG_REALTEK_BOARD_FPGA)
    aal_init_debug_set(0,5,0);
    printk("%s %d FPGA epon init at 1G1G EPON\n",__FUNCTION__,__LINE__);
#endif


    /*register event Notify*/
        /*register status change*/        
    if((ret = ca_event_register(0,CA_EVENT_EPON_REG_STAT_CHNG,dal_rt_rtl9607f_epon_register_status_intr,NULL))!=CA_E_OK)
    {
        printk("EPON dal_rt_rtl9607f_epon_init event epon stat changing event register Error!!\n");
    }
        /*laser link status change*/        
    if((ret = ca_event_register(0,CA_EVENT_EPON_PORT_LINK,dal_rt_rtl9607f_epon_link_intr,NULL))!=CA_E_OK)
    {
        printk("EPON dal_rt_rtl9607f_epon_init  event epon port link register Error!!\n");
    }    
        /*mpcp event handle*/
    if((ret = ca_event_register(0,CA_EVENT_EPON_MPCP_EVENT,dal_rt_rtl9607f_epon_mpcp_event_intr,NULL))!=CA_E_OK)
    {
        printk("EPON dal_rt_rtl9607f_epon_init  event CA_EVENT_EPON_MPCP_EVENT Error!!\n");
    }          
        /*register packet rx notify*/
    if((ret = ca_event_register(0,CA_EVENT_EPON_MPCP_REG_RX,dal_rt_rtl9607f_epon_mpcp_register_rx_intr,NULL))!=CA_E_OK)
    {
        printk("EPON dal_rt_rtl9607f_epon_init  event CA_EVENT_EPON_MPCP_REG_RX Error!!\n");
    }          

	epon_init = INIT_COMPLETED;
    return RT_ERR_OK;    
} /* end of dal_rt_rtl9607f_epon_init */

/* Function Name:
 *      dal_rt_rtl9607f_epon_oam_tx
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
dal_rt_rtl9607f_epon_oam_tx(uint32 msgLen, uint8 *pMsg, uint8 llidIdx)
{
#ifndef CONFIG_RTK_OAM_BY_BUFFER
    struct sk_buff *p_skb;
    HEADER_A_T ni_header_a;
    ca_mac_addr_t puc_da = {0x00, 0x13, 0x25, 0x00, 0x00, 0x00};
    ca_mac_addr_t puc_sa = {0x00, 0x13, 0x25, 0x00, 0x00, 0x01};
    ca_int8_t etype_oam[]  = {0xff, 0xf1};
    unsigned char tmp[60];
    unsigned char tmpFlag = 0;
    ca_uint8_t sw_idx = 0;

    if(msgLen < 60)
    {
        memset(tmp,0,60);
        memcpy(tmp,pMsg,msgLen);
        msgLen = 60;
        tmpFlag = 1;
    }

    p_skb = netdev_alloc_skb(NULL, msgLen+32);
    if (p_skb == NULL) 
    {
        ca_printf("netdev_alloc_skb fail\n");
        return RT_ERR_FAILED;
    }

    aal_mpcp_reg_sw_index_offset_get(0,&sw_idx);

    puc_da[5] = (puc_da[5] & 0xc0) | (llidIdx+sw_idx);

    memcpy(p_skb->data,&puc_da[0],6);
    memcpy(p_skb->data+6,&puc_sa[0],6);
    memcpy(p_skb->data+12,&etype_oam[0],2);
    p_skb->data[14]=0x00;
    p_skb->data[15]=0x00;

    if(tmpFlag == 1)
        memcpy(p_skb->data+16,tmp,msgLen);
    else
        memcpy(p_skb->data+16,pMsg,msgLen);

    p_skb->len = msgLen+16;

    memset(&ni_header_a, 0, sizeof(HEADER_A_T));

    ni_header_a.bits.cos = 7;
#ifdef CONFIG_CORTINA_BOARD_FPGA
    ni_header_a.bits.ldpid = 0x20 + llidIdx;
#else
    ni_header_a.bits.ldpid = 0xf;
#endif
    ni_header_a.bits.lspid = 0x11; //For mirror
    ni_header_a.bits.pkt_size = p_skb->len;
    ni_header_a.bits.fe_bypass = 1;

    ni_header_a.bits.bits_32_63.pkt_info.no_drop = 1;

    __rtk_ni_start_xmit(p_skb, &ni_header_a, NULL , NULL , NULL, 1);
    
    return RT_ERR_OK;
#else/*#ifndef CONFIG_RTK_OAM_BY_BUFFER*/
    ca_uint32_t ret = CA_E_OK;
    RT_INIT_CHK(epon_init);
    if((ret = aal_epon_tx_swframe_set(0,EPON_TX_SW_FRAME_OAM,llidIdx,pMsg,msgLen)) != CA_E_OK)
    {
        printk("%s %d : TX OAM Error!!\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    
    return RT_ERR_OK;
#endif/*#ifndef CONFIG_RTK_OAM_BY_BUFFER*/
} /* end of dal_rt_rtl9607f_epon_oam_tx */

/* Function Name:
 *      dal_rt_rtl9607f_epon_oam_rx_callback_register
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
dal_rt_rtl9607f_epon_oam_rx_callback_register(rt_epon_oam_rx_callback oamRx)
{
    int32   ret;
    rt_port_t pon;
    int uniPortMask = 0, portmask = 0xffffffff;

    /* check Init status */
    RT_INIT_CHK(epon_init);

    pon = HAL_GET_PON_PORT();
    uniPortMask = (1 << pon)-1;
    portmask &= (~uniPortMask);/*Remove UNI port mask*/

    /* function body */

    if(DISABLED == gOamTrapEnable)
    {
        if((ret = nic_register_rxhook(portmask,RENIC_RXPRI_PATCH,_oam_receive_from_nic))!=RT_ERR_OK)
        {
            printk("drv_nic_register_rxhook Error!!\n");
            return ret;
        }
        gOamTrapEnable = ENABLED;
    }

    gOamRxCallback = oamRx;
    
    return RT_ERR_OK;
}   /* end of dal_rt_rtl9607f_epon_oam_rx_callback_register */

/* Function Name:
 *      dal_rt_rtl9607f_epon_dyinggasp_set
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
dal_rt_rtl9607f_epon_dyinggasp_set(uint32 msgLen, uint8 *pMsg)
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

} /* end of dal_rt_rtl9607f_epon_dyinggasp_set */

/* Function Name:
 *      dal_rt_rtl9607f_epon_llid_entry_get
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
int32 dal_rt_rtl9607f_epon_llid_entry_get(rt_epon_llid_entry_t *pLlidEntry)  
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

    ret = dal_rt_rtl9607f_epon_losState_get(&losState);
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
}   /* end of dal_rt_rtl9607f_epon_llid_entry_get */





int32 dal_rt_rtl9607f_raw_epon_mpcp_registration_set
(
    CA_IN ca_device_id_t device_id,
    CA_IN ca_port_id_t   port_id,
    CA_IN ca_llid_t      llid,
    CA_IN ca_boolean_t   enable
)
{
    ca_status_t             ret = CA_E_OK;
    aal_mpcp_llid_msk_t     msk;
    aal_mpcp_llid_cfg_t     cfg;
    aal_mpcp_llid_entry_t 	entry;
    ca_uint16_t llid_value;
    ca_uint32_t llid_bmp = 0;


    memset(&msk, 0, sizeof(aal_mpcp_llid_msk_t));
    memset(&cfg, 0, sizeof(aal_mpcp_llid_cfg_t));
    __PON_LOCK();
    ret = aal_mpcp_llid_cfg_get(device_id, port_id, (ca_uint8_t)llid, &cfg);
    __PON_UNLOCK();
    if(ret != CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        return ret;
    }

    msk.s.offline = 1;
    cfg.offline = (enable)?0:1;
    __PON_LOCK();
    ret = aal_mpcp_llid_cfg_set(device_id, port_id, (ca_uint8_t)llid, msk, &cfg);
    __PON_UNLOCK();

    if (enable)
    {
        llid_bmp = 1<<llid;
        __PON_LOCK();
        ret = aal_mpcp_reg_llid_add(0, llid_bmp);
        __PON_UNLOCK();

        if(ret != CA_E_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
            return ret;
        }
    }
    else
    {
        llid_bmp = 1<<llid;
        __PON_LOCK();
        aal_mpcp_reg_llid_del(0, llid_bmp);
        __PON_UNLOCK();

        __PON_LOCK();
        ret = aal_mpcp_reg_llid_get(device_id, port_id, llid, &llid_value);
        __PON_UNLOCK();
        if(ret != CA_E_OK)
        {
            if(ret != CA_E_NOT_FOUND)
            {
                RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
                return ret;
            }    
        }

        memset(&entry, 0, sizeof(aal_mpcp_llid_entry_t));
        entry.llid_idx = llid;
        entry.mac_idx = llid;
        entry.llid_value = llid_value;
        entry.mcast = 0;

        __PON_LOCK();
        ret = aal_mpcp_llid_entry_set(device_id, port_id, AAL_MPCP_LLID_ENTRY_DEL, &entry);
        __PON_UNLOCK();

        if(ret != CA_E_OK)
        {
            if(ret != CA_E_NOT_FOUND)
            {            
                RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
                return ret;
            }
        }
    }
    return ret;
}



/* Function Name:
 *      dal_rt_rtl9607f_epon_llid_entry_set
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
int32 dal_rt_rtl9607f_epon_llid_entry_set(rt_epon_llid_entry_t *pLlidEntry)  
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
        if((ret = dal_rt_rtl9607f_raw_epon_mpcp_registration_set(0, port_id, (ca_uint8_t)llid, 0))!=CA_E_OK)
        {
            if(ret != CA_E_NOT_FOUND)
            {
                RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
                return RT_ERR_FAILED;
            }        
	}        
    }
    
    /*get current register status*/
    {
        ca_status_t           ret = CA_E_OK;
        aal_mpcp_llid_cfg_t cfg;
        memset(&cfg, 0, sizeof(aal_mpcp_llid_cfg_t));

        __PON_LOCK();
        ret = aal_mpcp_llid_cfg_get(0, port_id, (ca_uint8_t)llid, &cfg);
        __PON_UNLOCK();

        if(ret != CA_E_OK)
        {
            if(ret != CA_E_NOT_FOUND)
            {
                RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
                return RT_ERR_FAILED;
            }
        }                 
        enable = (cfg.offline) ? 0 : 1;
    }

    /*only status disable need to apply new register mac address*/
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
}   /* end of dal_rt_rtl9607f_epon_llid_entry_set */

/* Function Name:
 *      dal_rt_rtl9607f_epon_registerReq_get
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
int32 dal_rt_rtl9607f_epon_registerReq_get(rt_epon_regReq_t *pRegEntry)
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
    
    /*get pendding grant*/
    {
        aal_mpcp_cfg_t cfg;    
        memset(&cfg, 0, sizeof(aal_mpcp_cfg_t));

       __PON_LOCK();    
        ret = aal_mpcp_cfg_get(0, port_id, &cfg);
       __PON_UNLOCK();
        if(ret != CA_E_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
            return RT_ERR_FAILED;
        }        
        pRegEntry->pendGrantNum = cfg.pending_grant;
    }
    

    llid = pRegEntry->llidIdx;

    {
        ca_status_t           ret = CA_E_OK;
        aal_mpcp_llid_cfg_t cfg;
        memset(&cfg, 0, sizeof(aal_mpcp_llid_cfg_t));

       __PON_LOCK();    
        ret = aal_mpcp_llid_cfg_get(0, port_id, (ca_uint8_t)llid, &cfg);
       __PON_UNLOCK();
        if(ret != CA_E_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
            return RT_ERR_FAILED;
        }                  
        enable = (cfg.offline) ? 0 : 1;
    }

    if(enable == 1)
        pRegEntry->doRequest = ENABLED;
    else
        pRegEntry->doRequest = DISABLED;

    return RT_ERR_OK;
}   /* end of dal_rt_rtl9607f_epon_registerReq_get */




/* Function Name:
 *      dal_rt_rtl9607f_epon_registerReq_set
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
int32 dal_rt_rtl9607f_epon_registerReq_set(rt_epon_regReq_t *pRegEntry)
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


    {
        /*get pendding grant*/
        {
            aal_mpcp_cfg_t cfg;    
            memset(&cfg, 0, sizeof(aal_mpcp_cfg_t));
            __PON_LOCK();        
            ret = aal_mpcp_cfg_get(0, port_id, &cfg);
            __PON_UNLOCK();
            if(ret != CA_E_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
                return RT_ERR_FAILED;
            }        
            pre_pending_grants = cfg.pending_grant;
        }
        if(pre_pending_grants != pending_grants)
        {/*upddate new pennding grant setting*/
            aal_mpcp_msk_t      msk;
            aal_mpcp_cfg_t      cfg;

            if(pending_grants < CA_EPON_MPCP_PENDING_GRANTS_MIN ||
                pending_grants > CA_EPON_MPCP_PENDING_GRANTS_MAX)
            {
                printk(KERN_INFO "the value of pending_grants (%d) is invalid. Range is 1~8.\r\n", pending_grants);
                return RT_ERR_INPUT;
            }
        
            memset(&msk, 0, sizeof(aal_mpcp_msk_t));
            memset(&cfg, 0, sizeof(aal_mpcp_cfg_t));
        
            msk.s.pending_grant = 1;
            cfg.pending_grant = pending_grants;

            __PON_LOCK();
            ret = aal_mpcp_cfg_set(0, port_id, msk, &cfg);        
            __PON_UNLOCK();
            if(ret != CA_E_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
                return RT_ERR_FAILED;
            }                   
        }
    }
    

    if(pRegEntry->doRequest == ENABLED)
    {
        printk(KERN_INFO "Enable register llid-idx %d\n",llid);
        if((ret = dal_rt_rtl9607f_raw_epon_mpcp_registration_set(0, port_id, (ca_uint8_t)llid, 1))!=CA_E_OK)
        {
            if(ret != CA_E_NOT_FOUND)
            {
                RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
                return RT_ERR_FAILED;
            }
        }
    }

    return RT_ERR_OK;
}   /* end of dal_rt_rtl9607f_epon_registerReq_set */

/* Function Name:
 *      dal_rt_rtl9607f_epon_churningKey_get
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
int32 dal_rt_rtl9607f_epon_churningKey_get(rt_epon_churningKeyEntry_t *pEntry)
{
    ca_status_t ret=CA_E_OK;
#if 1
    rt_port_t pon_port;
    ca_port_id_t port_id;
    ca_uint32_t pon_mode;
    aal_epon_crypto_xg_tbl_msk_t     msk;
    aal_epon_crypto_xg_tbl_field_t   cfg;
    aal_epon_crypto_cfg_t config;
    ca_uint32_t key_data[4];
    ca_epon_port_encryption_key_t key;

    
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "");
    
    RT_INIT_CHK(epon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((2 < pEntry->keyIdx), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_EPON_MAX_LLID <= pEntry->llidIdx), RT_ERR_INPUT);
    
    pon_port = HAL_GET_PON_PORT();
    port_id = CA_PORT_ID(CA_PORT_TYPE_EPON,pon_port);
    pon_mode = aal_pon_mac_mode_get(0);

    memset(&config, 0, sizeof(aal_epon_crypto_cfg_t));

    if (pon_mode == ONU_PON_MAC_EPON_1G)
    {
        __PON_LOCK();
        ret = aal_epon_crypto_1g_cfg_get(0, port_id, &config);
        __PON_UNLOCK();
        //TODO: ret checer
        //CA_RET_VALUE_CHECK(ret);

        __PON_LOCK();
        ret = aal_epon_crypto_1g_key_get(0, port_id, pEntry->llidIdx, AAL_EPON_CRYPTO_DIRECTION_RX, pEntry->keyIdx, (aal_epon_crypto_key_t *)key_data);
        __PON_UNLOCK();

        if (config.rx_mode == AAL_EPON_CRYPTO_1G_MODE_AES_DPOE)
        {
            memcpy(&key, &key_data[0], sizeof(ca_epon_port_encryption_key_t));
        }
        else
        {
            memset(&key.data, 0, sizeof(key.data));
            key.data[0] = (key_data[3] >>8)&0xff;
            key.data[1] = (key_data[3] >>16)&0xff;
            key.data[2] = (key_data[3] >>24)&0xff;
        }

    }
    else if (pon_mode == ONU_PON_MAC_EPON_D10G || pon_mode == ONU_PON_MAC_EPON_BI10G)
    {
        __PON_LOCK();
        ret = aal_epon_crypto_xg_cfg_get(0, port_id, &config);
        __PON_UNLOCK();

		//CA_RET_VALUE_CHECK(ret);

        memset(&cfg, 0, sizeof(aal_epon_crypto_xg_tbl_field_t));

        __PON_LOCK();
        ret = aal_epon_crypto_xg_sa_tbl_get(0,port_id,(ca_uint8_t)pEntry->llidIdx, AAL_EPON_CRYPTO_DIRECTION_RX, &cfg);
        __PON_UNLOCK();
        //CA_RET_VALUE_CHECK(ret);

		if(config.rx_mode == AAL_EPON_CRYPTO_XG_MODE_AES_DPOE)
		{
		    if(pEntry->keyIdx == 0)
		    {
		        memcpy(&key, &cfg.key0[0], sizeof(ca_epon_port_encryption_key_t));
		    }
	        else
	        {
	            memcpy(&key, &cfg.key1[0], sizeof(ca_epon_port_encryption_key_t));
	        }
	    }
	    else
	    {
	        if(pEntry->keyIdx == 0)
	        {
	            key.data[0] = (cfg.key0[0] >>16)&0xff;
	            key.data[1] = (cfg.key0[0] >>8 )&0xff;
	            key.data[2] = (cfg.key0[0]     )&0xff;
	            key.data[3] = (cfg.key0[1] >>8 )&0xff;
	            key.data[4] = (cfg.key0[1]     )&0xff;
	            key.data[5] = (cfg.key0[0] >>24)&0xff;
	            key.data[6] = (cfg.key0[2]     )&0xff;
	            key.data[7] = (cfg.key0[1] >>24)&0xff;
	            key.data[8] = (cfg.key0[1] >>16)&0xff;
	        }
	        else
	        {
	            key.data[0] = (cfg.key1[0] >>16)&0xff;
	            key.data[1] = (cfg.key1[0] >>8 )&0xff;
	            key.data[2] = (cfg.key1[0]     )&0xff;
	            key.data[3] = (cfg.key1[1] >>8 )&0xff;
	            key.data[4] = (cfg.key1[1]     )&0xff;
	            key.data[5] = (cfg.key1[0] >>24)&0xff;
	            key.data[6] = (cfg.key1[2]     )&0xff;
	            key.data[7] = (cfg.key1[1] >>24)&0xff;
	            key.data[8] = (cfg.key1[1] >>16)&0xff;
	        }
	    }
	}
#else
    rtk_port_t pon_port;
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

#endif

    return RT_ERR_OK;
}   /* end of dal_rt_rtl9607f_epon_churningKey_get */

/* Function Name:
 *      dal_rt_rtl9607f_epon_churningKey_set
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
int32 dal_rt_rtl9607f_epon_churningKey_set(rt_epon_churningKeyEntry_t *pEntry)
{
#if 1
    ca_status_t ret=CA_E_OK;
    ca_port_id_t port_id;
    rtk_port_t pon_port;
    ca_uint32_t port_type;
    ca_uint32_t port;
    ca_uint32_t pon_mode = aal_pon_mac_mode_get(0);
    aal_epon_crypto_xg_tbl_msk_t     msk;
    aal_epon_crypto_xg_tbl_field_t   cfg;
    aal_epon_crypto_cfg_t config;
    ca_uint32_t key_id;
    ca_uint32_t key_data[4];
    ca_epon_port_encryption_key_t key;
    
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "");
    
    RT_INIT_CHK(epon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((2 < pEntry->keyIdx), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_EPON_MAX_LLID <= pEntry->llidIdx), RT_ERR_INPUT);
    
    pon_port = HAL_GET_PON_PORT();
    port_id = CA_PORT_ID(CA_PORT_TYPE_EPON,pon_port);
    key_id = pEntry->keyIdx;
    port_type = PORT_TYPE(port_id); 
    port = PORT_ID(port_id);
    memset(&config, 0, sizeof(aal_epon_crypto_cfg_t));
    
    /* for 1G EPON, only surport RX decryption */
    if(pon_mode == ONU_PON_MAC_EPON_1G){
        ret = aal_epon_crypto_1g_enable_set(0, port_id, AAL_EPON_CRYPTO_DIRECTION_RX, TRUE);
        //CA_RET_VALUE_CHECK(ret);
    }
    else if(pon_mode == ONU_PON_MAC_EPON_BI10G){
            ret = aal_epon_crypto_xg_enable_set(0, port_id, AAL_EPON_CRYPTO_DIRECTION_RX, TRUE);
            //CA_RET_VALUE_CHECK(ret);
            ret = aal_epon_crypto_xg_enable_set(0, port_id, AAL_EPON_CRYPTO_DIRECTION_TX, FALSE);
            //CA_RET_VALUE_CHECK(ret);
    }else if(pon_mode == ONU_PON_MAC_EPON_D10G){
            ret = aal_epon_crypto_xg_enable_set(0, port_id, AAL_EPON_CRYPTO_DIRECTION_RX, TRUE);
            //CA_RET_VALUE_CHECK(ret);
            ret = aal_epon_crypto_xg_enable_set(0, port_id, AAL_EPON_CRYPTO_DIRECTION_TX, FALSE);
            //CA_RET_VALUE_CHECK(ret);
    }
    
    /* Configure key */
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
    
    if (pon_mode == ONU_PON_MAC_EPON_1G)
    {
        __PON_LOCK();
        ret = aal_epon_crypto_1g_cfg_get(0, port_id, &config);
        __PON_UNLOCK();

        if(config.rx_mode == AAL_EPON_CRYPTO_1G_MODE_AES_DPOE)
        {
            ca_printf("start to set 1G AES DPOE llid_idx %d, key %d, \r\n", pEntry->llidIdx, key_id);
            memcpy(&key_data[0], &key, sizeof(ca_epon_port_encryption_key_t));
        }
        else
        {
            ca_printf("start to set 1G churning llid_idx %d, key %d, \r\n", pEntry->llidIdx, key_id);
            key_data[3] = (key.data[0]<<8) | (key.data[1]<<16) | (key.data[2]<<24) ;
            key_data[2] = 0;
            key_data[1] = 0;
            key_data[0] = 0;
            ca_printf("key_data[3] = 0x%x\r\n", key_data[3]);
    }

        __PON_LOCK();
        ret = aal_epon_crypto_1g_key_set(0, port_id, (ca_uint8_t)pEntry->llidIdx, AAL_EPON_CRYPTO_DIRECTION_RX, key_id, (aal_epon_crypto_key_t *)key_data);
        __PON_UNLOCK();

        //CA_RET_VALUE_CHECK(ret);
    }
    else if (pon_mode == ONU_PON_MAC_EPON_D10G || pon_mode == ONU_PON_MAC_EPON_BI10G)
    {
        __PON_LOCK();
        ret = aal_epon_crypto_xg_cfg_get(0, port_id, &config);
        __PON_UNLOCK();


	    //CA_RET_VALUE_CHECK(ret);

	    memset(&msk, 0, sizeof(aal_epon_crypto_xg_tbl_msk_t));
	    memset(&cfg, 0, sizeof(aal_epon_crypto_xg_tbl_field_t));

	    if(config.rx_mode == AAL_EPON_CRYPTO_XG_MODE_AES_DPOE)
	    {
            ca_printf("start to set 10G AES DPOE llid_idx %d, key %d, \r\n", pEntry->llidIdx, key_id);
	        if(key_id == 0)
	        {
	            msk.s.key0 = 1;
	            memcpy(&cfg.key0[0], &key, sizeof(ca_epon_port_encryption_key_t));
	        }
	        else
	        {
	            msk.s.key1 = 1;
	            memcpy(&cfg.key1[0], &key, sizeof(ca_epon_port_encryption_key_t));
	        }

	    }
	    else
	    {
            ca_printf("start to set 10G churning llid_idx %d, key %d, \r\n", pEntry->llidIdx, key_id);
            /* CTC churning 3 * 3 bytes key -> ASIC key
            level_1 churnig key�� 0 1 2
            level_2 churnig key�� 3 4 5
            level_3 churnig key�� 6 7 8

                        byte3  byte2   byte1   byte0
            key[0] =      5       0       1       2
            key[1] =      7       8       3       4
            key[2] =      x       x       x       6
            */
	        if(key_id == 0)
	        {
	            msk.s.key0 = 1;
	            cfg.key0[3] = 0;
	            cfg.key0[2] = (key.data[6]);
	            cfg.key0[1] = (key.data[7]<<24) | (key.data[8]<<16) | (key.data[3]<<8) | (key.data[4]);
	            cfg.key0[0] = (key.data[5]<<24) | (key.data[0]<<16) | (key.data[1]<<8) | (key.data[2]);
	        }
	        else
	        {
	            msk.s.key1 = 1;
	            cfg.key1[3] = 0;
	            cfg.key1[2] = (key.data[6]);
	            cfg.key1[1] = (key.data[7]<<24) | (key.data[8]<<16) | (key.data[3]<<8) | (key.data[4]);
	            cfg.key1[0] = (key.data[5]<<24) | (key.data[0]<<16) | (key.data[1]<<8) | (key.data[2]);
	        }
	    }

        __PON_LOCK();
	    ret = aal_epon_crypto_xg_sa_tbl_set(0, port_id, (ca_uint8_t)pEntry->llidIdx, AAL_EPON_CRYPTO_DIRECTION_TX, msk, &cfg);
	    ret = aal_epon_crypto_xg_sa_tbl_set(0, port_id, (ca_uint8_t)pEntry->llidIdx, AAL_EPON_CRYPTO_DIRECTION_RX, msk, &cfg);
        __PON_UNLOCK();

	    //CA_RET_VALUE_CHECK(ret);
	}
#else 
    ca_status_t ret=CA_E_OK;
    rtk_port_t pon_port;
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
#endif
    return RT_ERR_OK;
}   /* end of dal_rtl9607f_epon_churningKey_set */

/* Function Name:
 *      dal_rt_rtl9607f_epon_usFecState_get
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
int32 dal_rt_rtl9607f_epon_usFecState_get(rt_enable_t *pState)
{
    #if 1
    ca_status_t             ret=CA_E_OK;
    rt_port_t               pon_port;
    ca_port_id_t            port_id;
    aal_epon_mac_ctrl_t     ctrl;
    aal_epon_fec_xg_cfg_t   fec_xg_cfg;
    aal_epon_fec_cfg_t      fec_1g_cfg;
    ca_uint8_t              onu_type = AAL_ONU_TYPE_1G_10G;
    ca_boolean_t            tx_enable=DISABLED;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "");

    /* check Init status */
    RT_INIT_CHK(epon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */
    pon_port = HAL_GET_PON_PORT();
    port_id = CA_PORT_ID(CA_PORT_TYPE_EPON,pon_port);
    
    memset(&ctrl, 0, sizeof(aal_epon_mac_ctrl_t));
    
    __PON_LOCK();
    ret = aal_epon_mac_onu_type_get(0, port_id, &onu_type);
    __PON_UNLOCK();
    //CA_RET_VALUE_CHECK(ret);
    
    if (onu_type == AAL_ONU_TYPE_1G_1G)
    {
        memset(&fec_1g_cfg, 0, sizeof(fec_1g_cfg));
        __PON_LOCK();
        ret = aal_epon_fec_cfg_get(0, port_id, &fec_1g_cfg);
        __PON_UNLOCK();

        //CA_RET_VALUE_CHECK(ret);

        tx_enable = fec_1g_cfg.tx_encoding_ena;
    }
    else if (onu_type == AAL_ONU_TYPE_1G_10G)
    {
        memset(&fec_1g_cfg, 0, sizeof(fec_1g_cfg));
        __PON_LOCK();
        ret = aal_epon_fec_cfg_get(0, port_id, &fec_1g_cfg);
        __PON_UNLOCK();

        //CA_RET_VALUE_CHECK(ret);

        tx_enable = fec_1g_cfg.tx_encoding_ena;
    }
    else if (onu_type == AAL_ONU_TYPE_10G_10G)
    {
        memset(&fec_xg_cfg, 0, sizeof(fec_xg_cfg));
        __PON_LOCK();
        ret = aal_epon_fec_xg_cfg_get(0, port_id, &fec_xg_cfg);
        __PON_UNLOCK();
        
        //CA_RET_VALUE_CHECK(ret);

        tx_enable = fec_xg_cfg.tx_ena;
    }
    
    if(tx_enable == 1)
        *pState = ENABLED;
    else
        *pState = DISABLED;
    
    #else
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
    #endif
    
    return RT_ERR_OK;
}   /* end of dal_rt_rtl9607f_epon_usFecState_get */

/* Function Name:
 *      dal_rt_rtl9607f_epon_usFecState_set
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
int32 dal_rt_rtl9607f_epon_usFecState_set(rt_enable_t state)
{
#if 1
    ca_status_t ret=CA_E_OK;
    rt_port_t pon_port;
    ca_port_id_t port_id;
    ca_boolean_t tx_enable;
    ca_boolean_t rx_enable;
    ca_uint8_t onu_type = AAL_ONU_TYPE_1G_10G;
    aal_epon_mac_ctrl_t         ctrl;
    aal_epon_fec_xg_cfg_t        fec_xg_cfg;
    aal_epon_fec_xg_cfg_msk_t    msk;
    aal_epon_fec_cfg_t        fec_1g_cfg;
    aal_epon_fec_msk_t        fec_1g_msk;
    aal_puc_rpt_cfg_msk_t     rpt_msk;
    aal_puc_rpt_cfg_t         rpt_cfg;
    aal_puc_btc_cfg_msk_t     btc_msk;
    aal_puc_btc_cfg_t         btc_cfg;
    
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "state=%d",state);

    /* check Init status */
    RT_INIT_CHK(epon_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= state), RT_ERR_INPUT);

    /* function body */
    pon_port = HAL_GET_PON_PORT();
    port_id = CA_PORT_ID(CA_PORT_TYPE_EPON,pon_port);
    
    if(state == ENABLED)
        tx_enable = 1;
    else
        tx_enable = 0;

    memset(&ctrl, 0, sizeof(aal_epon_mac_ctrl_t));
    ret = aal_epon_mac_onu_type_get(0, port_id, &onu_type);
    //CA_RET_VALUE_CHECK(ret);
    
    if (onu_type == AAL_ONU_TYPE_1G_1G)
    {
        memset(&fec_1g_cfg, 0, sizeof(fec_1g_cfg));
        __PON_LOCK();
        ret = aal_epon_fec_cfg_get(0, port_id, &fec_1g_cfg);
        __PON_UNLOCK();

        //CA_RET_VALUE_CHECK(ret);

        rx_enable = fec_1g_cfg.rx_decoding_ena;
        
        memset(&fec_1g_msk, 0, sizeof(fec_1g_msk));
        memset((void *)&fec_1g_cfg, 0, sizeof(fec_1g_cfg));
        fec_1g_msk.s.tx_encoding_ena = 1;
        fec_1g_msk.s.rx_decoding_ena = 1;
        fec_1g_cfg.tx_encoding_ena = tx_enable;
        fec_1g_cfg.rx_decoding_ena = rx_enable;
 
        __PON_LOCK();
        ret = aal_epon_fec_cfg_set(0, port_id, fec_1g_msk, &fec_1g_cfg);
        __PON_UNLOCK();
    }
    else if (onu_type == AAL_ONU_TYPE_1G_10G)
    {
        /* TX 1G */
        memset(&fec_1g_msk, 0, sizeof(fec_1g_msk));
        memset((void *)&fec_1g_cfg, 0, sizeof(fec_1g_cfg));
        fec_1g_msk.s.tx_encoding_ena = 1;
        fec_1g_msk.s.rx_decoding_ena = 1;
        fec_1g_cfg.tx_encoding_ena = tx_enable;
        fec_1g_cfg.rx_decoding_ena = 0;
 
        __PON_LOCK();
        ret = aal_epon_fec_cfg_set(0, port_id, fec_1g_msk, &fec_1g_cfg);
        __PON_UNLOCK();
    }
    else if (onu_type == AAL_ONU_TYPE_10G_10G)
    {
        memset(&fec_xg_cfg, 0, sizeof(fec_xg_cfg));
        __PON_LOCK();
        ret = aal_epon_fec_xg_cfg_get(0, port_id, &fec_xg_cfg);
        __PON_UNLOCK();
        
        //CA_RET_VALUE_CHECK(ret);
        rx_enable = (fec_xg_cfg.rx_mode == AAL_FEC_XG_RX_MODE_OFF ? FALSE : TRUE);
        
        /* TX 10G, RX 10G */
        memset(&msk, 0, sizeof(aal_epon_fec_xg_cfg_msk_t));
        memset(&fec_xg_cfg, 0, sizeof(aal_epon_fec_xg_cfg_t));
        msk.s.rx_mode = 1;
        msk.s.tx_ena  = 1;
        fec_xg_cfg.rx_mode   = (rx_enable == TRUE ? AAL_FEC_XG_RX_MODE_AUTO : AAL_FEC_XG_RX_MODE_OFF);
        fec_xg_cfg.tx_ena    = tx_enable;

        __PON_LOCK();
        ret = aal_epon_fec_xg_cfg_set(0, port_id, msk, &fec_xg_cfg);
        __PON_UNLOCK();
    }
    
        /* PUC report configuration */
    if (onu_type == AAL_ONU_TYPE_1G_10G)
    {
        memset((void *)&rpt_cfg, 0, sizeof(rpt_cfg));
        rpt_msk.u32 = 0;
 
        rpt_msk.s.rpt1gfecon = 1;
        rpt_cfg.rpt1gfecon   = tx_enable ? 1 : 0;
 
        __PON_LOCK();
        ret = aal_puc_rpt_cfg_set(0, rpt_msk, &rpt_cfg);
        __PON_UNLOCK();

        //CA_RET_VALUE_CHECK(ret);
 
        memset((void *)&btc_cfg, 0, sizeof(btc_cfg));
        btc_msk.u32 = 0;
 
        btc_msk.s.pyld1gfecon = 1;
        btc_cfg.pyld1gfecon   = tx_enable ? 1 : 0;
 
        __PON_LOCK();
        ret = aal_puc_btc_cfg_set(0, btc_msk, &btc_cfg);
        __PON_UNLOCK();

        //CA_RET_VALUE_CHECK(ret);
    }
#else
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
#endif
    return ret;
}   /* end of dal_rt_rtl9607f_epon_usFecState_set */

/* Function Name:
 *      dal_rtl9607f_epon_dsFecState_get
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
int32 dal_rt_rtl9607f_epon_dsFecState_get(rt_enable_t *pState)
{
#if 1
    ca_status_t             ret=CA_E_OK;
    rt_port_t               pon_port;
    ca_port_id_t            port_id;
    aal_epon_mac_ctrl_t     ctrl;
    aal_epon_fec_xg_cfg_t   fec_xg_cfg;
    aal_epon_fec_cfg_t      fec_1g_cfg;
    ca_uint8_t              onu_type = AAL_ONU_TYPE_1G_10G;
    ca_boolean_t            rx_enable = DISABLED;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "");

    /* check Init status */
    RT_INIT_CHK(epon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */
    pon_port = HAL_GET_PON_PORT();
    port_id = CA_PORT_ID(CA_PORT_TYPE_EPON,pon_port);
    
    memset(&ctrl, 0, sizeof(aal_epon_mac_ctrl_t));
    
    __PON_LOCK();
    ret = aal_epon_mac_onu_type_get(0, port_id, &onu_type);
    __PON_UNLOCK();
    //CA_RET_VALUE_CHECK(ret);
    
    if (onu_type == AAL_ONU_TYPE_1G_1G)
    {
        memset(&fec_1g_cfg, 0, sizeof(fec_1g_cfg));
        __PON_LOCK();
        ret = aal_epon_fec_cfg_get(0, port_id, &fec_1g_cfg);
        __PON_UNLOCK();

        //CA_RET_VALUE_CHECK(ret);

        rx_enable = fec_1g_cfg.rx_decoding_ena;
    }
    else if (onu_type == AAL_ONU_TYPE_1G_10G)
    {
        memset(&fec_xg_cfg, 0, sizeof(fec_xg_cfg));
        __PON_LOCK();
        ret = aal_epon_fec_xg_cfg_get(0, port_id, &fec_xg_cfg);
        __PON_UNLOCK();
        //CA_RET_VALUE_CHECK(ret);

        rx_enable = (fec_xg_cfg.rx_mode == AAL_FEC_XG_RX_MODE_OFF ? FALSE : TRUE);
    }
    else if (onu_type == AAL_ONU_TYPE_10G_10G)
    {
        memset(&fec_xg_cfg, 0, sizeof(fec_xg_cfg));
        __PON_LOCK();
        ret = aal_epon_fec_xg_cfg_get(0, port_id, &fec_xg_cfg);
        __PON_UNLOCK();
        
        //CA_RET_VALUE_CHECK(ret);
        rx_enable = (fec_xg_cfg.rx_mode == AAL_FEC_XG_RX_MODE_OFF ? FALSE : TRUE);
    }
    
    if(rx_enable == 1)
        *pState = ENABLED;
    else
        *pState = DISABLED;
    
#else
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
#endif
    return RT_ERR_OK;
}   /* end of dal_rt_rtl9607f_epon_dsFecState_get */

/* Function Name:
 *      dal_rt_rtl9607f_epon_dsFecState_set
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
int32 dal_rt_rtl9607f_epon_dsFecState_set(rt_enable_t state)
{
#if 1
    ca_status_t ret=CA_E_OK;
    rt_port_t pon_port;
    ca_port_id_t port_id;
    ca_boolean_t tx_enable;
    ca_boolean_t rx_enable;
    ca_uint8_t onu_type = AAL_ONU_TYPE_1G_10G;
    aal_epon_mac_ctrl_t         ctrl;
    aal_epon_fec_xg_cfg_t        fec_xg_cfg;
    aal_epon_fec_xg_cfg_msk_t    msk;
    aal_epon_fec_cfg_t        fec_1g_cfg;
    aal_epon_fec_msk_t        fec_1g_msk;
    aal_puc_rpt_cfg_msk_t     rpt_msk;
    aal_puc_rpt_cfg_t         rpt_cfg;
    aal_puc_btc_cfg_msk_t     btc_msk;
    aal_puc_btc_cfg_t         btc_cfg;
    
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "state=%d",state);

    /* check Init status */
    RT_INIT_CHK(epon_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= state), RT_ERR_INPUT);

    /* function body */
    pon_port = HAL_GET_PON_PORT();
    port_id = CA_PORT_ID(CA_PORT_TYPE_EPON,pon_port);
    
    if(state == ENABLED)
        rx_enable = 1;
    else
        rx_enable = 0;

    memset(&ctrl, 0, sizeof(aal_epon_mac_ctrl_t));
    ret = aal_epon_mac_onu_type_get(0, port_id, &onu_type);
    //CA_RET_VALUE_CHECK(ret);
    
    if (onu_type == AAL_ONU_TYPE_1G_1G)
    {
        memset(&fec_1g_cfg, 0, sizeof(fec_1g_cfg));
        __PON_LOCK();
        ret = aal_epon_fec_cfg_get(0, port_id, &fec_1g_cfg);
        __PON_UNLOCK();

        //CA_RET_VALUE_CHECK(ret);

        tx_enable = fec_1g_cfg.tx_encoding_ena;
        
        memset(&fec_1g_msk, 0, sizeof(fec_1g_msk));
        memset((void *)&fec_1g_cfg, 0, sizeof(fec_1g_cfg));
        fec_1g_msk.s.tx_encoding_ena = 1;
        fec_1g_msk.s.rx_decoding_ena = 1;
        fec_1g_cfg.tx_encoding_ena = tx_enable;
        fec_1g_cfg.rx_decoding_ena = rx_enable;
 
        __PON_LOCK();
        ret = aal_epon_fec_cfg_set(0, port_id, fec_1g_msk, &fec_1g_cfg);
        __PON_UNLOCK();
    }
    else if (onu_type == AAL_ONU_TYPE_1G_10G)
    {
        /* RX 10G */
        memset(&msk, 0, sizeof(aal_epon_fec_xg_cfg_msk_t));
        memset(&fec_xg_cfg, 0, sizeof(aal_epon_fec_xg_cfg_t));
        msk.s.rx_mode = 1;
        //msk.s.tx_ena  = 1;
        fec_xg_cfg.rx_mode   = (rx_enable == TRUE ? AAL_FEC_XG_RX_MODE_AUTO : AAL_FEC_XG_RX_MODE_OFF);
        //fec_xg_cfg.tx_ena    = 0;

        __PON_LOCK();
        ret = aal_epon_fec_xg_cfg_set(0, port_id, msk, &fec_xg_cfg);
        __PON_UNLOCK();
    }
    else if (onu_type == AAL_ONU_TYPE_10G_10G)
    {
        memset(&fec_xg_cfg, 0, sizeof(fec_xg_cfg));
        __PON_LOCK();
        ret = aal_epon_fec_xg_cfg_get(0, port_id, &fec_xg_cfg);
        __PON_UNLOCK();
        
        //CA_RET_VALUE_CHECK(ret);
        tx_enable = fec_xg_cfg.tx_ena;
        
        /* TX 10G, RX 10G */
        memset(&msk, 0, sizeof(aal_epon_fec_xg_cfg_msk_t));
        memset(&fec_xg_cfg, 0, sizeof(aal_epon_fec_xg_cfg_t));
        msk.s.rx_mode = 1;
        msk.s.tx_ena  = 1;
        fec_xg_cfg.rx_mode   = (rx_enable == TRUE ? AAL_FEC_XG_RX_MODE_AUTO : AAL_FEC_XG_RX_MODE_OFF);
        fec_xg_cfg.tx_ena    = tx_enable;

        __PON_LOCK();
        ret = aal_epon_fec_xg_cfg_set(0, port_id, msk, &fec_xg_cfg);
        __PON_UNLOCK();
    }
    #if 0  /*only change DS setting do not need to config us FEC*/   
    // TODO: need to check if only change ds fec state, does it need to report
    /* PUC report configuration */
    if (onu_type == AAL_ONU_TYPE_1G_10G)
    {
        memset((void *)&rpt_cfg, 0, sizeof(rpt_cfg));
        rpt_msk.u32 = 0;
 
        rpt_msk.s.rpt1gfecon = 1;
        rpt_cfg.rpt1gfecon   = tx_enable ? 1 : 0;
 
        __PON_LOCK();
        ret = aal_puc_rpt_cfg_set(0, rpt_msk, &rpt_cfg);
        __PON_UNLOCK();

        //CA_RET_VALUE_CHECK(ret);
 
        memset((void *)&btc_cfg, 0, sizeof(btc_cfg));
        btc_msk.u32 = 0;
 
        btc_msk.s.pyld1gfecon = 1;
        btc_cfg.pyld1gfecon   = tx_enable ? 1 : 0;
 
        __PON_LOCK();
        ret = aal_puc_btc_cfg_set(0, btc_msk, &btc_cfg);
        __PON_UNLOCK();
        //CA_RET_VALUE_CHECK(ret);
    }
    #endif

#else
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
#endif
    return RT_ERR_OK;
}   /* end of dal_rt_rtl9607f_epon_dsFecState_set */

/* Function Name:
 *      dal_rt_rtl9607f_epon_mibCounter_get
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
int32 dal_rt_rtl9607f_epon_mibCounter_get(rt_epon_counter_t *pCounter)
{
    ca_status_t ret=CA_E_OK;
    rt_port_t pon_port;
    ca_port_id_t port_id;
    aal_epon_mac_stats_port_mpcp_t stats_port_mpcp;
    aal_epon_mac_stats_port_mpcp_ctrl_t mpcp_mac_ctrl;
    aal_epon_fec_stats_t        stats;
    aal_epon_fec_cfg_t          fec_1g_cfg;
    aal_epon_fec_msk_t          fec_1g_msk;
    ca_uint8_t onu_type =       AAL_ONU_TYPE_1G_10G;
    aal_epon_mac_stats_llid_mpcp_t stats_llid_mpcp;
    aal_epon_mac_stats_port_eth_t stats_port_eth;
   
    

    /* check Init status */
    RT_INIT_CHK(epon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pCounter), RT_ERR_NULL_POINTER);

    /* function body */
    pon_port = HAL_GET_PON_PORT();
    port_id = CA_PORT_ID(CA_PORT_TYPE_EPON,pon_port);
    
    memset(&stats_port_mpcp, 0, sizeof(stats_port_mpcp));
    memset(&mpcp_mac_ctrl, 0, sizeof(mpcp_mac_ctrl));
    memset(&stats, 0, sizeof(aal_epon_fec_stats_t));
    
    __PON_LOCK();
    ret = aal_epon_mac_stats_port_mpcp_get(0, port_id, &stats_port_mpcp);
    __PON_UNLOCK();

    __PON_LOCK();
    ret = aal_epon_mac_stats_port_mpcp_ctrl_get(0, port_id, &mpcp_mac_ctrl);
    __PON_UNLOCK();
    
    ret = aal_epon_mac_onu_type_get(0, port_id, &onu_type);
    
    if (onu_type == AAL_ONU_TYPE_1G_1G)
    {
        memset(&fec_1g_msk, 0, sizeof(fec_1g_msk));
        memset((void *)&fec_1g_cfg, 0, sizeof(fec_1g_cfg));
        fec_1g_msk.s.cnt_clr = 1;
        fec_1g_cfg.cnt_clr = 1;

        __PON_LOCK();
        ret = aal_epon_fec_cfg_set(0, port_id, fec_1g_msk, &fec_1g_cfg);
        ret = aal_epon_fec_stats_get(0, port_id, &stats);
        __PON_UNLOCK();

    }else{
        __PON_LOCK();
        ret = aal_epon_fec_xg_stats_get(0, port_id, &stats);
        __PON_UNLOCK();
    }
    
    memset(&pCounter->llidIdxCnt,0,sizeof(rt_epon_llidCounter_t));
  
    pCounter->mpcpRxDiscGate = stats_port_mpcp.rx_epon_bcgatecnt;
    pCounter->fecCorrectedBlocks = stats.rx_corr_codeword;;
    pCounter->fecUncorrectedBlocks = stats.rx_uncorr_codeword;
    pCounter->fecTtotalCodeword = stats.rx_total_codeword;

    pCounter->fecCodingVio = 0;
    pCounter->notBcstBitLlid7fff =0;
    pCounter->notBcstBitNotOnuLlid =0;
    pCounter->bcstBitPlusOnuLLid =0;
    pCounter->bcstNotOnuLLid =0;
    pCounter->crc8Err = 0;
    pCounter->mpcpTxRegRequest = stats_port_mpcp.tx_epon_bcregreqcnt;
    

    memset(&pCounter->llidIdxCnt,0,sizeof(rt_epon_llidCounter_t));
    /*get per LLID mpcp counter*/
    __PON_LOCK();
    ret = aal_epon_mac_stats_llid_mpcp_get(0, port_id, pCounter->llidIdx,&stats_llid_mpcp);
    __PON_UNLOCK();        

    pCounter->llidIdxCnt.mpcpTxReport   = stats_llid_mpcp.tx_epon_reportcnt;
    pCounter->llidIdxCnt.mpcpRxGate     = stats_llid_mpcp.rx_epon_ucgatecnt;    
    pCounter->mpcpTxRegAck              = stats_llid_mpcp.tx_epon_regackcnt;


    memset(&stats_port_eth, 0, sizeof(aal_epon_mac_stats_port_eth_t));
    __PON_LOCK();
    ret = aal_epon_mac_stats_port_eth_get(0, port_id, &stats_port_eth);
    __PON_UNLOCK();        

    /*PON MAC Rx crc error*/
    pCounter->eponRxEthCrcErr = stats_port_eth.rx_crcerrcnt;
  
    return RT_ERR_OK;
}   /* end of dal_rt_rtl9607f_epon_mibCounter_get */



/* Function Name:
 *      dal_rt_rtl9607f_epon_mibCounter_clear
 * Description:
 *      Clear EPON MIB Counter
 * Input:
 *       None
 * Output:
 *       None 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_rt_rtl9607f_epon_mibCounter_clear(void)
{
    ca_status_t ret=CA_E_OK;
    rt_port_t pon_port;
    ca_port_id_t port_id;
    aal_epon_mac_stats_port_mpcp_t stats_port_mpcp;
    aal_epon_fec_stats_t        stats;
    aal_epon_fec_cfg_t          fec_1g_cfg;
    aal_epon_fec_msk_t          fec_1g_msk;
    ca_uint8_t onu_type =       AAL_ONU_TYPE_1G_10G;
    aal_epon_mac_stats_llid_mpcp_t stats_llid_mpcp;
    aal_epon_mac_stats_port_eth_t stats_port_eth;
    aal_epon_mac_stats_llid_eth_t stats_llid_eth;
   
    ca_boolean_t   epon_mib_rc;

    /* check Init status */
    RT_INIT_CHK(epon_init);


    /* function body */
    pon_port = HAL_GET_PON_PORT();
    port_id = CA_PORT_ID(CA_PORT_TYPE_EPON,pon_port);



    __PON_LOCK();
    ret = aal_epon_mac_mib_rc_get(0, port_id, &epon_mib_rc);
    ret = aal_epon_mac_mib_rc_set(0, port_id, CA_TRUE);
    __PON_UNLOCK();

    /*read mib counter, for clean counter*/    
    
    /*pon port mpcp*/
    __PON_LOCK();
    ret = aal_epon_mac_stats_port_mpcp_get(0, port_id, &stats_port_mpcp);
    __PON_UNLOCK();

    /*TBD*/
    /*get per LLID mpcp counter clear LLID 0 only*/
    __PON_LOCK();
    ret = aal_epon_mac_stats_llid_mpcp_get(0, port_id, 0,&stats_llid_mpcp);
    __PON_UNLOCK();        


    /*PON port Ethernent counter read for clear*/
    memset(&stats_port_eth, 0, sizeof(aal_epon_mac_stats_port_eth_t));
    __PON_LOCK();
    ret = aal_epon_mac_stats_port_eth_get(0, port_id, &stats_port_eth);
    __PON_UNLOCK();        

    /*get per LLID mpcp counter clear LLID 0 only*/
    __PON_LOCK();
    ret = aal_epon_mac_stats_llid_eth_get(0, port_id, 0,&stats_llid_eth);
    __PON_UNLOCK();        



    /*write back read clar setting*/    
    __PON_LOCK();
    ret = aal_epon_mac_mib_rc_set(0, port_id, epon_mib_rc);
    __PON_UNLOCK();


    
    /*clear FEC counter*/
    ret = aal_epon_mac_onu_type_get(0, port_id, &onu_type);
    
    if (onu_type == AAL_ONU_TYPE_1G_1G)
    {
        memset(&fec_1g_msk, 0, sizeof(fec_1g_msk));
        memset((void *)&fec_1g_cfg, 0, sizeof(fec_1g_cfg));
        fec_1g_msk.s.cnt_clr = 1;
        fec_1g_cfg.cnt_clr = 1;

        __PON_LOCK();
        ret = aal_epon_fec_cfg_set(0, port_id, fec_1g_msk, &fec_1g_cfg);
        ret = aal_epon_fec_stats_get(0, port_id, &stats);
        __PON_UNLOCK();

    }else{
        __PON_LOCK();
        ret = aal_epon_fec_xg_stats_get(0, port_id, &stats);
        __PON_UNLOCK();
    }
    
    return RT_ERR_OK;
    
}

/* Function Name:
 *      dal_rt_rtl9607f_epon_losState_get
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
int32 dal_rt_rtl9607f_epon_losState_get(rt_enable_t *pState)
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
}   /* end of dal_rt_rtl9607f_epon_losState_get */





/* Function Name:
 *      dal_rt_rtl9607f_epon_lofState_get
 * Description:
 *      Get laser lose of frame state.
 * Input:
 *      pState LOF state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32 dal_rt_rtl9607f_epon_lofState_get(rt_enable_t *pState)
{
    ca_status_t ret=CA_E_OK;
    rt_port_t pon_port;
    ca_port_id_t port_id;
    aal_epon_status_t epon_status;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "");

    /* check Init status */
    RT_INIT_CHK(epon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */
    pon_port = HAL_GET_PON_PORT();
    port_id = CA_PORT_ID(CA_PORT_TYPE_EPON,pon_port);

    __PON_LOCK();
    ret = aal_epon_status_get(0,port_id,&epon_status);
    __PON_UNLOCK();
    if(ret != CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        return RT_ERR_FAILED;
    }

    if ((!epon_status.pcs_sync_xg && (epon_status.speed > AAL_EPON_SPEED_MODE_DS_1G_US_1G))
       || (!epon_status.pcs_sync_1g && (epon_status.speed <= AAL_EPON_SPEED_MODE_DS_1G_US_1G)))
    {
        *pState = ENABLED;
    }
    else
    {
        *pState = DISABLED;
    }

    
    return RT_ERR_OK;
}   /* end of dal_rt_rtl9607f_epon_lofState_get */




/* Function Name:
 *      dal_rt_rtl9607f_epon_mpcp_gate_timer_set
 * Description:
 *      Set mpcp gate expire timer and deregister state.
 * Input:
 *      gate_timer      gate expire timer
 *      deregistration  gate expired deregister or not
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32 
dal_rt_rtl9607f_epon_mpcp_gate_timer_set(uint32 gateTimer,rt_enable_t deregistration)
{
    ca_status_t         ret = CA_E_OK;
    rt_port_t pon_port;
    ca_port_id_t port_id;

    aal_mpcp_msk_t      msk;
    aal_mpcp_cfg_t      cfg;

    /* function body */
    pon_port = HAL_GET_PON_PORT();
    port_id = CA_PORT_ID(CA_PORT_TYPE_EPON,pon_port);


    memset(&msk, 0, sizeof(aal_mpcp_msk_t));
    memset(&cfg, 0, sizeof(aal_mpcp_cfg_t));

    msk.s.gatetimer_threshold = 1;
    msk.s.gatetimer_dereg_en = 1;
    cfg.gatetimer_threshold = gateTimer;
    cfg.gatetimer_dereg_en = deregistration;

    __PON_LOCK();
    ret = aal_mpcp_cfg_set(0, port_id, msk, &cfg);
    __PON_UNLOCK();

    if(ret != CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        return RT_ERR_FAILED;
    }
    
    return RT_ERR_OK;    
}/* end of dal_rt_rtl9607f_epon_mpcp_gate_timer_set */




/* Function Name:
 *      dal_rt_rtl9607f_epon_mpcp_gate_timer_get
 * Description:
 *      Get mpcp gate expire timer and deregister state.
 * Input:
 *      *pGate_timer      gate expire timer
 *      *pDeregistration  gate expired deregister or not
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32 
dal_rt_rtl9607f_epon_mpcp_gate_timer_get(uint32 *pGateTimer,rt_enable_t *pDeregistration)
{
    ca_status_t         ret = CA_E_OK;
    aal_mpcp_cfg_t      cfg;
    rt_port_t pon_port;
    ca_port_id_t port_id;


    /* parameter check */
    RT_PARAM_CHK((NULL == pGateTimer), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pDeregistration), RT_ERR_NULL_POINTER);



    /* function body */
    pon_port = HAL_GET_PON_PORT();
    port_id = CA_PORT_ID(CA_PORT_TYPE_EPON,pon_port);

    memset(&cfg, 0, sizeof(aal_mpcp_cfg_t));

    __PON_LOCK();
    ret = aal_mpcp_cfg_get(0, port_id, &cfg);
    __PON_UNLOCK();
    
    if(ret != CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        return RT_ERR_FAILED;
    }
    *pGateTimer = cfg.gatetimer_threshold;
    *pDeregistration = cfg.gatetimer_dereg_en;

    return RT_ERR_OK;    
}/* end of dal_rt_rtl9607f_epon_mpcp_gate_timer_get */




/* Function Name:
 *      dal_rt_rtl9607f_epon_info_notify_callback_register
 * Description:
 *      set epon info notify callback
 * Input:
 *      callBack - call back function pointer
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_NULL_POINTER - NULL input parameter 
 * Note:
 */
int32 dal_rt_rtl9607f_epon_info_notify_callback_register(rt_epon_info_notify_cb callBack)
{
    
    dal_rt_rtl9607f_epon_info_callback = callBack;
    
    return RT_ERR_OK;    
}



/* Function Name:
 *      dal_rt_rt9607f_epon_ponLoopback_set
 * Description:
 *      config pon port loopback
 * Input:
 *      enable - set loopback status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32 dal_rt_rt9607f_epon_ponLoopback_set(uint32 llid,rt_enable_t enable)
{
    aal_ilpb_cfg_msk_t  ilpb_mask;
    aal_ilpb_cfg_t      ilpb_cfg;

    aal_pdc_pdc_map_mem_data_mask_t pdc_mask;
    aal_pdc_pdc_map_mem_data_t      pdc_cfg;

    ca_status_t ret = CA_E_OK;


    /*set ilpb loopback*/    
    ilpb_mask.u32[0] = 0;
    ilpb_mask.u32[1] = 0;

    memset(&ilpb_cfg, 0, sizeof(aal_ilpb_cfg_t));

    ret = aal_port_ilpb_cfg_get(0,0x20+llid,&ilpb_cfg);
    if(ret != CA_E_OK){
        return RT_ERR_FAILED;
    }

    ilpb_cfg.loop_enable = (enable > 0);
    //ilpb_cfg.da_sa_swap_en = (enable > 0);


    ilpb_mask.s.loop_enable = 1;
    //ilpb_mask.s.da_sa_swap_en = 1;

    ret = aal_port_ilpb_cfg_set(0,0x20 + llid,ilpb_mask,&ilpb_cfg);
    if(ret != CA_E_OK){
        return RT_ERR_FAILED;
    }

#if 0
    ret = aal_port_ilpb_cfg_set(0,0x20 + 0x3F,ilpb_mask,&ilpb_cfg);
    if(ret != CA_E_OK){
        return RT_ERR_FAILED;
    }
#endif

    /*set for normal unicast LLID*/
    memset(&pdc_mask, 0, sizeof(aal_pdc_pdc_map_mem_data_mask_t));
    memset(&pdc_cfg, 0, sizeof(aal_pdc_pdc_map_mem_data_t));
    
    pdc_mask.bf.lspid=1;
    /*set pdc mapping*/
    if(1==enable)
    {/*mapping PDC lspid to 0x20+llid_index*/
        pdc_cfg.lspid     = 0x20+llid;    
    }
    else
    {
        pdc_cfg.lspid     = AAL_PON_PORT_ID;    
    }    
  
    ret = aal_pdc_map_mem_set(0, llid, pdc_mask, &pdc_cfg);
    if(ret != CA_E_OK){
        return RT_ERR_FAILED;
    }

#if 0
    /*set for boroadcast llid*/
    memset(&pdc_mask, 0, sizeof(aal_pdc_pdc_map_mem_data_mask_t));
    memset(&pdc_cfg, 0, sizeof(aal_pdc_pdc_map_mem_data_t));
   
    pdc_mask.bf.lspid=1;
    /*set pdc mapping*/
    if(1==enable)
    {/*mapping PDC lspid to 0x20+llid_index*/
        pdc_cfg.lspid     = 0x20+llid;    
    }
    else
    {
        pdc_cfg.lspid     = AAL_PON_PORT_ID;    
    }    
    ret = aal_pdc_map_mem_set(0, 63, pdc_mask, &pdc_cfg);
    if(ret != CA_E_OK){
        return RT_ERR_FAILED;
    }
#endif

    return RT_ERR_OK;     
}



