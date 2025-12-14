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
 * $Revision: $
 * $Date: $
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
#include <hal/chipdef/chip.h>
#include <rtk/init.h>
#include <rtk/default.h>
#include <dal/dal_mgmt.h>
#include <common/util/rt_util.h>
#include <hal/common/halctrl.h>
#include <rtk/rt/rt_epon.h>
#include <rtk/rt/rt_rate.h>
#ifndef CONFIG_LUNA_G3_SERIES
#if defined(CONFIG_SDK_KERNEL_LINUX)
#include "re8686_nic.h"
#endif
#endif


static rt_enable_t churningStatus = DISABLED;



#ifndef CONFIG_LUNA_G3_SERIES
#if defined(CONFIG_SDK_KERNEL_LINUX)
int32 rt_rtk_epon_oam_tx(uint32 msgLen,uint8 *pMsg,uint8 llidIdx)
{
    int32   ret = RT_ERR_OK;
    uint32 chipId, rev, subtype;
    struct tx_info txInfo;
    rtk_portmask_t portMask;
    unsigned int rtl960x_pon_port_mask;
    extern int re8686_tx_with_Info(unsigned char *pPayload, unsigned short length, void *pInfo);

    rtk_switch_portMask_Clear(&portMask);
    rtk_switch_port2PortMask_set(&portMask,RTK_PORT_PON);
    rtl960x_pon_port_mask = portMask.bits[0];

    
    if(msgLen == 0||pMsg == NULL || llidIdx > 32)
        return RT_ERR_FAILED;

    memset(&txInfo,0x0,sizeof(struct tx_info));

    /*keep format, switch do not modify packet format ex:vlan tag*/
    GMAC_TXINFO_KEEP(&txInfo)=1;
    GMAC_TXINFO_CPUTAG(&txInfo)=1;
    GMAC_TXINFO_TX_PMASK(&txInfo)=rtl960x_pon_port_mask;

    GMAC_TXINFO_ASPRI(&txInfo)=1;
    GMAC_TXINFO_CPUTAG_PRI(&txInfo)=7;

    /* Specified the output queue according to the LLId index */
    GMAC_TXINFO_CPUTAG_PSEL(&txInfo)=1;

    ret = rtk_switch_version_get(&chipId, &rev, &subtype);
    if(RT_ERR_OK != ret)
    {
        printk("%s:%d rtk_switch_version_get failed %d!\n", __FILE__, __LINE__,ret);
        return ret;
    }     
    switch(chipId)
    {    
        case RTL9601B_CHIP_ID:
            GMAC_TXINFO_TX_DST_STREAM_ID(&txInfo)= 0x08 + (llidIdx * 16);
            break;

        case RTL9602C_CHIP_ID:
        case RTL9607C_CHIP_ID:
            switch(llidIdx)
            {
                case 0:
                    GMAC_TXINFO_TX_DST_STREAM_ID(&txInfo) = 8;
                    break;
                case 1:
                    GMAC_TXINFO_TX_DST_STREAM_ID(&txInfo) = 10;
                    break;
                case 2:
                    GMAC_TXINFO_TX_DST_STREAM_ID(&txInfo) = 24;
                    break;
                case 3:
                    GMAC_TXINFO_TX_DST_STREAM_ID(&txInfo) = 26;
                    break;
                case 4:
                    GMAC_TXINFO_TX_DST_STREAM_ID(&txInfo) = 40;
                    break;
                case 5:
                    GMAC_TXINFO_TX_DST_STREAM_ID(&txInfo) = 42;
                    break;
                case 6:
                    GMAC_TXINFO_TX_DST_STREAM_ID(&txInfo) = 56;
                    break;
                case 7:
                    GMAC_TXINFO_TX_DST_STREAM_ID(&txInfo) = 58;
                    break;
                        
            }
            break;
        case RTL9603CVD_CHIP_ID:
            switch(llidIdx)
            {
                case 0:
                    GMAC_TXINFO_TX_DST_STREAM_ID(&txInfo) = 8;
                    break;
                case 1:
                    GMAC_TXINFO_TX_DST_STREAM_ID(&txInfo) = 24;
                    break;
                case 2:
                    GMAC_TXINFO_TX_DST_STREAM_ID(&txInfo) = 40;
                    break;
                case 3:
                    GMAC_TXINFO_TX_DST_STREAM_ID(&txInfo) = 56;
                    break;
                case 4:
                    GMAC_TXINFO_TX_DST_STREAM_ID(&txInfo) = 72;
                    break;
                case 5:
                    GMAC_TXINFO_TX_DST_STREAM_ID(&txInfo) = 88;
                    break;
                case 6:
                    GMAC_TXINFO_TX_DST_STREAM_ID(&txInfo) = 104;
                    break;
                case 7:
                    GMAC_TXINFO_TX_DST_STREAM_ID(&txInfo) = 120;
                    break;
                        
            }
            break;
        case APOLLOMP_CHIP_ID:
        default:
            GMAC_TXINFO_TX_DST_STREAM_ID(&txInfo) = 7;
           break;
        
    } 
       
    re8686_tx_with_Info(pMsg, msgLen, &txInfo);

    return ret;
}

int32 rt_rtk_epon_dyinggasp_set(uint32 msgLen,uint8 *pMsg)
{
    int32   ret = RT_ERR_OK;
    uint32 chipId, rev, subtype;
    struct tx_info txInfo;
    rtk_portmask_t portMask;
    unsigned int rtl960x_pon_port_mask;
    extern int re8686_tx_with_Info(unsigned char *pPayload, unsigned short length, void *pInfo);

    rtk_switch_portMask_Clear(&portMask);
    rtk_switch_port2PortMask_set(&portMask,RTK_PORT_PON);
    rtl960x_pon_port_mask = portMask.bits[0];
    
    if(msgLen == 0||pMsg == NULL)
        return RT_ERR_FAILED;

    memset(&txInfo,0x0,sizeof(struct tx_info));
    
    GMAC_TXINFO_KEEP(&txInfo) = 1;
    GMAC_TXINFO_CPUTAG_PSEL(&txInfo) = 1;
    GMAC_TXINFO_CPUTAG(&txInfo) = 1;

    ret=rtk_switch_version_get(&chipId, &rev, &subtype);
 
    if(RT_ERR_OK != ret)
    {
        printk("%s:%d rtk_switch_version_get failed %d!\n", __FILE__, __LINE__,ret);
        return ret;
    } 
 
    switch(chipId)
    {    
        case RTL9601B_CHIP_ID:
        case RTL9602C_CHIP_ID:
        case RTL9607C_CHIP_ID:
        case RTL9603CVD_CHIP_ID:
            GMAC_TXINFO_TX_DST_STREAM_ID(&txInfo) = 0x1f;
            break;
        case APOLLOMP_CHIP_ID:
        default:
            GMAC_TXINFO_TX_DST_STREAM_ID(&txInfo) = 0x7f;
            break;
    }   
    
    GMAC_TXINFO_TX_PMASK(&txInfo) = rtl960x_pon_port_mask;
    re8686_tx_with_Info(pMsg, msgLen, &txInfo);

    return ret;
}

static rt_enable_t gOamTrapEnable = DISABLED; //Default disable trap OAM packet
static rt_epon_oam_rx_callback gOamRxCallback = NULL;
static rt_epon_mpcp_rx_callback gMpcpRxCallback = NULL;





static int _epon_register_pkt_accept_check(struct sk_buff *skb)
{
    int ret;
    
    rtk_epon_llid_entry_t regEntry;    
    
    printk("\n receive register packet mac [%2.2x %2.2x %2.2x %2.2x %2.2x %2.2x] flag:%d\n",
                    skb->data[0],
                    skb->data[1],
                    skb->data[2],
                    skb->data[3],
                    skb->data[4],
                    skb->data[5],
                    skb->data[22]);
    regEntry.llidIdx = 0;
    ret = rtk_epon_llid_entry_get(&regEntry);
    if(RT_ERR_OK != ret)
        return 0;    
    
    printk("\n -- local mac addr [%2.2x %2.2x %2.2x %2.2x %2.2x %2.2x]\n",
            regEntry.mac.octet[0],
            regEntry.mac.octet[1],
            regEntry.mac.octet[2],
            regEntry.mac.octet[3],
            regEntry.mac.octet[4],
            regEntry.mac.octet[5]);
    /*mac control packet*/
    if(skb->data[0]==0x01 && 
       skb->data[1]==0x80 &&
       skb->data[2]==0xC2 &&
       skb->data[3]==0x00 &&
       skb->data[4]==0x00 &&
       skb->data[5]==0x01)    
    {
        return 1;
    }
    
    /*local mac address*/
    if(skb->data[0]==regEntry.mac.octet[0] && 
       skb->data[1]==regEntry.mac.octet[1] &&
       skb->data[2]==regEntry.mac.octet[2] &&
       skb->data[3]==regEntry.mac.octet[3] &&
       skb->data[4]==regEntry.mac.octet[4] &&
       skb->data[5]==regEntry.mac.octet[5])    
    {
        return 1;
    }
    return 0;
}




static int _oam_receive_from_nic(struct re_private *cp,struct sk_buff *skb,struct rx_info *pRxInfo)
{
    uint8_t * oam_ptr;
    uint32_t oam_len;
    unsigned char llidIdx; 
    rt_epon_register_pkt_info_t reg_pkt_info;
    
    //printk("\n _oam_receive_from_nic \n");

    if((skb->data[12] == 0x88) && (skb->data[13] == 0x09) && (skb->data[14] == 0x03))
    {
        oam_ptr = skb->data;
        oam_len = skb->len;

        /* concate the LLID index to the end of the packet buffer */
        llidIdx = GMAC_RXINFO_PON_STREAM_ID(pRxInfo);
        llidIdx = llidIdx & 0xf;

        if(gOamRxCallback)
        {
            gOamRxCallback(oam_len, oam_ptr, llidIdx);
        }

        return RE8670_RX_STOP;
    }


    if((skb->data[12] == 0x88) && (skb->data[13] == 0x08))
    {
        //printk("\nmpcp rx opcode:%d\n",skb->data[15]);
        /*only handle register packet opcode=5*/
        if((skb->data[14] == 0x00) && (skb->data[15] == 0x5))
        {
            printk("\nregister packet flag: 0x%x\n",skb->data[22]);
                
            if(_epon_register_pkt_accept_check(skb)==0)
            {
                printk("\n this register is not for this onu!!\n");
                return RE8670_RX_STOP;
            }
            
            /* concate the LLID index to the end of the packet buffer */
            llidIdx = GMAC_RXINFO_PON_STREAM_ID(pRxInfo);
            llidIdx = llidIdx & 0xf;
            
            reg_pkt_info.assignPort       = skb->data[20]<<8|skb->data[21];
            reg_pkt_info.flag             = skb->data[22];
            reg_pkt_info.syncTime         = skb->data[23]<<8|skb->data[24];
            reg_pkt_info.echoPendingGrant = skb->data[25];
            
            if(gMpcpRxCallback)
            {
                gMpcpRxCallback(&reg_pkt_info, llidIdx);    
            }


            return RE8670_RX_STOP;

        }


        
    }

    return RE8670_RX_CONTINUE;
}

int32 rt_rtk_epon_oam_rx_callback_register(rt_epon_oam_rx_callback oamRx)
{
    int ret;
    rtk_portmask_t portMask;
    unsigned int rtl960x_pon_port_mask;
    extern int drv_nic_register_rxhook(int portmask,int priority,p2rfunc_t rx);

    rtk_switch_portMask_Clear(&portMask);
    rtk_switch_port2PortMask_set(&portMask,RTK_PORT_PON);
    rtl960x_pon_port_mask = portMask.bits[0];

    if(DISABLED == gOamTrapEnable)
    {
        if((ret = drv_nic_register_rxhook(rtl960x_pon_port_mask, RE8686_RXPRI_OAM, _oam_receive_from_nic))!=RT_ERR_OK)
        {
            printk("drv_nic_register_rxhook Error!!\n");
            return ret;
        }
        gOamTrapEnable = ENABLED;
    }

    gOamRxCallback = oamRx;

    return RT_ERR_OK;
}


int32 rt_rtk_mpcpRxregisterInfoCb_register(rt_epon_mpcp_rx_callback mpcpRx)
{
    gMpcpRxCallback = mpcpRx;

    return RT_ERR_OK;
}




#endif
#endif


/* Function Name:
 *      rt_epon_init
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
int32 rt_epon_init(void)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->epon_init)
        return RT_ERR_DRIVER_NOT_FOUND;
    
#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_ponmisc_init)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_ponmisc_init();
    RTK_API_UNLOCK();
    if(ret!= RT_ERR_OK)
        return ret;

    if (NULL == RT_MAPPER->epon_init)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->epon_init();
    RTK_API_UNLOCK();
    if(ret!= RT_ERR_OK)
        return ret;

    if (NULL == RT_MAPPER->rt_epon_init)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_epon_init();
    RTK_API_UNLOCK();
    if(ret!= RT_ERR_OK)
        return ret;

    if (NULL == RT_MAPPER->ponmac_init)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_init();
    RTK_API_UNLOCK();
    if(ret!= RT_ERR_OK)
        return ret;
#else
    if (NULL == RT_MAPPER->epon_init)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->epon_init();
    RTK_API_UNLOCK();
#endif

    return ret;
}   /* end of rt_epon_init */

/* Function Name:
 *      rt_epon_llid_entry_get
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
int32 rt_epon_llid_entry_get(rt_epon_llid_entry_t *pLlidEntry)  
{
    int32   ret;

    /* function body */
#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_epon_llid_entry_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_epon_llid_entry_get(pLlidEntry);
    RTK_API_UNLOCK();
#else
    if (NULL == RT_MAPPER->epon_llid_entry_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->epon_llid_entry_get((rtk_epon_llid_entry_t *)pLlidEntry);
    RTK_API_UNLOCK();
#endif
    return ret;
}   /* end of rt_epon_llid_entry_get */

/* Function Name:
 *      rt_epon_llid_entry_set
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
int32 rt_epon_llid_entry_set(rt_epon_llid_entry_t *pLlidEntry)
{
    int32   ret;

    /* function body */
#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_epon_llid_entry_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_epon_llid_entry_set(pLlidEntry);
    RTK_API_UNLOCK();
#else
    if (NULL == RT_MAPPER->epon_llid_entry_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->epon_llid_entry_set((rtk_epon_llid_entry_t *)pLlidEntry);
    RTK_API_UNLOCK();
#endif
    return ret;
}   /* end of rt_epon_llid_entry_set */

/* Function Name:
 *      rt_epon_registerReq_get
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
int32 rt_epon_registerReq_get(rt_epon_regReq_t *pRegEntry)
{
    int32   ret;

    /* function body */
#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_epon_registerReq_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_epon_registerReq_get(pRegEntry);
    RTK_API_UNLOCK();
#else
    if (NULL == RT_MAPPER->epon_registerReq_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->epon_registerReq_get((rtk_epon_regReq_t *)pRegEntry);
    RTK_API_UNLOCK();
#endif
    return ret;
}   /* end of rt_epon_registerReq_get */

/* Function Name:
 *      rt_epon_registerReq_set
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
int32 rt_epon_registerReq_set(rt_epon_regReq_t *pRegEntry)
{
    int32   ret;

    /* function body */
#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_epon_registerReq_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_epon_registerReq_set(pRegEntry);
    RTK_API_UNLOCK();
#else
    if (NULL == RT_MAPPER->epon_registerReq_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->epon_registerReq_set((rtk_epon_regReq_t *)pRegEntry);
    RTK_API_UNLOCK();
#endif

    if((RT_ERR_OK == ret) && (ENABLED == pRegEntry->doRequest))
    {
        churningStatus = DISABLED;
    }

    return ret;
}   /* end of rt_epon_registerReq_set */

/* Function Name:
 *      rt_epon_churningKey_get
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
int32 rt_epon_churningKey_get(rt_epon_churningKeyEntry_t *pEntry)
{
    int32   ret;

    /* function body */
#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_epon_churningKey_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_epon_churningKey_get(pEntry);
    RTK_API_UNLOCK();
#else
    if (NULL == RT_MAPPER->epon_churningKey_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->epon_churningKey_get((rtk_epon_churningKeyEntry_t *)pEntry);
    RTK_API_UNLOCK();
#endif
    return ret;
}   /* end of rt_epon_churningKey_get */

/* Function Name:
 *      rt_epon_churningKey_set
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
int32 rt_epon_churningKey_set(rt_epon_churningKeyEntry_t *pEntry)
{
    int32   ret;

    /* function body */
#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_epon_churningKey_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_epon_churningKey_set(pEntry);
    RTK_API_UNLOCK();
#else
    if (NULL == RT_MAPPER->epon_churningKey_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->epon_churningKey_set((rtk_epon_churningKeyEntry_t *)pEntry);
    RTK_API_UNLOCK();
#endif

    if(RT_ERR_OK == ret)
    {
        churningStatus = ENABLED;
    }

    return ret;
}   /* end of rt_epon_churningKey_set */





/* Function Name:
 *      rt_epon_churningStatus_get
 * Description:
 *      Get EPON tripple churning status
 * Input:
 *	    none
 * Output:
 *      pEnabled: pointer of status
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_NULL_POINTER - NULL input parameter 
 * Note:
 */
int32 rt_epon_churningStatus_get(rt_enable_t *pEnable)
{
    if(pEnable)
    {
        *pEnable = churningStatus;
        return RT_ERR_OK;
    }
    else
    {
        return RT_ERR_NULL_POINTER;
    }
} /* end of rt_epon_churningStatus_get */



/* Function Name:
 *      rt_epon_usFecState_get
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
int32 rt_epon_usFecState_get(rt_enable_t *pState)
{
    int32   ret;

    /* function body */
#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_epon_usFecState_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_epon_usFecState_get(pState);
    RTK_API_UNLOCK();
#else
    if (NULL == RT_MAPPER->epon_usFecState_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->epon_usFecState_get((rtk_enable_t *)pState);
    RTK_API_UNLOCK();
#endif
    return ret;
}   /* end of rt_epon_usFecState_get */


/* Function Name:
 *      rt_epon_usFecState_set
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
int32 rt_epon_usFecState_set(rt_enable_t state)
{
    int32   ret;

    /* function body */
#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_epon_usFecState_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_epon_usFecState_set(state);
    RTK_API_UNLOCK();
#else
    if (NULL == RT_MAPPER->epon_usFecState_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->epon_usFecState_set(state);
    RTK_API_UNLOCK();
#endif
    return ret;
}   /* end of rt_epon_usFecState_set */


/* Function Name:
 *      rt_epon_dsFecState_get
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
int32 rt_epon_dsFecState_get(rt_enable_t *pState)
{
    int32   ret;

    /* function body */
#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_epon_dsFecState_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_epon_dsFecState_get((rtk_enable_t *)pState);
    RTK_API_UNLOCK();
#else
    if (NULL == RT_MAPPER->epon_dsFecState_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->epon_dsFecState_get((rtk_enable_t *)pState);
    RTK_API_UNLOCK();
#endif
    return ret;
}   /* end of rt_epon_dsFecState_get */


/* Function Name:
 *      rt_epon_dsFecState_set
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
int32 rt_epon_dsFecState_set(rt_enable_t state)
{
    int32   ret;

    /* function body */
#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_epon_dsFecState_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_epon_dsFecState_set(state);
    RTK_API_UNLOCK();
#else
    if (NULL == RT_MAPPER->epon_dsFecState_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->epon_dsFecState_set(state);
    RTK_API_UNLOCK();
#endif
    return ret;
}   /* end of rt_epon_dsFecState_set */

/* Function Name:
 *      rt_epon_dsFecState_set
 * Description:
 *      Set down-stream fec state
 * Input:
 *       None
 * Output:
 *       pCounter : EPON mib counter 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rt_epon_mibCounter_get(rt_epon_counter_t *pCounter)
{
    int32   ret;

    /* function body */
#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_epon_mibCounter_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_epon_mibCounter_get(pCounter);
    RTK_API_UNLOCK();
#else
    if (NULL == RT_MAPPER->epon_mibCounter_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    {
        rtk_epon_counter_t rtk_mibCounter;
        
        
        memset(&rtk_mibCounter,0x0,sizeof(rtk_epon_counter_t));
        
        rtk_mibCounter.llidIdx=pCounter->llidIdx;
        
        ret = RT_MAPPER->epon_mibCounter_get(&rtk_mibCounter);

        pCounter->mpcpRxDiscGate          = rtk_mibCounter.mpcpRxDiscGate;      
        pCounter->fecCorrectedBlocks      = rtk_mibCounter.fecCorrectedBlocks;    
        pCounter->fecUncorrectedBlocks    = rtk_mibCounter.fecUncorrectedBlocks;
        pCounter->fecCodingVio            = rtk_mibCounter.fecCodingVio;
        pCounter->notBcstBitLlid7fff      = rtk_mibCounter.notBcstBitLlid7fff;
        pCounter->notBcstBitNotOnuLlid    = rtk_mibCounter.notBcstBitNotOnuLlid;
        pCounter->bcstBitPlusOnuLLid      = rtk_mibCounter.bcstBitPlusOnuLLid;
        pCounter->bcstNotOnuLLid          = rtk_mibCounter.bcstNotOnuLLid;
        pCounter->crc8Err                 = rtk_mibCounter.crc8Err;
        pCounter->mpcpTxRegRequest        = rtk_mibCounter.mpcpTxRegRequest;
        pCounter->mpcpTxRegAck            = rtk_mibCounter.mpcpTxRegAck;


        /*per LLID counter*/
        pCounter->llidIdxCnt.mpcpTxReport   = rtk_mibCounter.llidIdxCnt.mpcpTxReport;
        pCounter->llidIdxCnt.mpcpRxGate     = rtk_mibCounter.llidIdxCnt.mpcpRxGate;
        pCounter->llidIdxCnt.onuLlidNotBcst = rtk_mibCounter.llidIdxCnt.onuLlidNotBcst;
        
        pCounter->llidIdxCnt.queueTxFrames[0] = rtk_mibCounter.llidIdxCnt.queueTxFrames[0];
        pCounter->llidIdxCnt.queueTxFrames[1] = rtk_mibCounter.llidIdxCnt.queueTxFrames[1];
        pCounter->llidIdxCnt.queueTxFrames[2] = rtk_mibCounter.llidIdxCnt.queueTxFrames[2];
        pCounter->llidIdxCnt.queueTxFrames[3] = rtk_mibCounter.llidIdxCnt.queueTxFrames[3];
        pCounter->llidIdxCnt.queueTxFrames[4] = rtk_mibCounter.llidIdxCnt.queueTxFrames[4];
        pCounter->llidIdxCnt.queueTxFrames[5] = rtk_mibCounter.llidIdxCnt.queueTxFrames[5];
        pCounter->llidIdxCnt.queueTxFrames[6] = rtk_mibCounter.llidIdxCnt.queueTxFrames[6];
        pCounter->llidIdxCnt.queueTxFrames[7] = rtk_mibCounter.llidIdxCnt.queueTxFrames[7];
    }
    RTK_API_UNLOCK();
#endif
    return ret;
}   /* end of rt_epon_mibCounter_get */

/* Function Name:
 *      rt_epon_losState_get
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
int32
rt_epon_losState_get(rt_enable_t *pState)
{
    int32   ret;

    /* function body */
#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_epon_losState_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_epon_losState_get(pState);
    RTK_API_UNLOCK();
#else
    if (NULL == RT_MAPPER->epon_losState_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->epon_losState_get((rtk_enable_t *)pState);
    RTK_API_UNLOCK();
#endif
    return ret;
}   /* end of rt_epon_losState_get */


/* Function Name:
 *      rt_epon_lofState_get
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
int32 rt_epon_lofState_get(rt_enable_t *pState)
{
    int32   ret=RT_ERR_OK;
    /* function body */
#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_epon_losState_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_epon_losState_get(pState);
    RTK_API_UNLOCK();
#else
    if(pState);
#endif
    return ret;
}   /* end of rt_epon_losState_get */


/* Function Name:
 *      rt_epon_mpcp_gate_timer_set
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
int32 rt_epon_mpcp_gate_timer_set(uint32 gateTimer,rt_enable_t deregistration)
{
    int32   ret=RT_ERR_OK;
    /* function body */
#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_epon_mpcp_gate_timer_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_epon_mpcp_gate_timer_set(gateTimer,deregistration);
    RTK_API_UNLOCK();
#else
    if(gateTimer);
    if(deregistration);
#endif
    return ret;
}   /* end of rt_epon_mpcp_gate_timer_set */


/* Function Name:
 *      rt_epon_mpcp_gate_timer_get
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
int32 rt_epon_mpcp_gate_timer_get(uint32 *pGateTimer,rt_enable_t *pDeregistration)
{
    int32   ret=RT_ERR_OK;
    /* function body */
#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_epon_mpcp_gate_timer_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->rt_epon_mpcp_gate_timer_get(pGateTimer,pDeregistration);
    RTK_API_UNLOCK();
#else
    if(pGateTimer);
    if(pDeregistration);
#endif

    return ret;
}   /* end of rt_epon_mpcp_gate_timer_get */


/* Function Name:
 *      rt_epon_ponQueue_set
 * Description:
 *      Set the pon queue
 * Input:
 *      llid    - logical llid index
 *      queueId  - logical llid queue id
 *      pQueuecfg  - pointer to the queue configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The unit of granularity is 8Kbps.
 */
int32 
rt_epon_ponQueue_set(uint32 llid, uint32 queueId, rt_epon_queueCfg_t *pQueuecfg)
{
    int32   ret;
    rtk_ponmac_queue_t queue;
    rtk_ponmac_queueCfg_t queueCfg;

    /* function body */
    if (NULL == RT_MAPPER->ponmac_queue_add)
        return RT_ERR_DRIVER_NOT_FOUND;

    queue.schedulerId = llid;
    queue.queueId = queueId;
#ifdef CONFIG_LUNA_G3_SERIES
    if(pQueuecfg->cir == RT_MAX_RATE)
        queueCfg.cir = RT_RATE_PON_EGR_PIR_CIR_RATE_MAX;
    else
        queueCfg.cir = (pQueuecfg->cir << 3);

    if(pQueuecfg->pir == RT_MAX_RATE)
        queueCfg.pir = RT_RATE_PON_EGR_PIR_CIR_RATE_MAX;
    else
        queueCfg.pir = (pQueuecfg->pir << 3);
#else
    if(pQueuecfg->cir == RT_MAX_RATE)
        queueCfg.cir = RT_RATE_PON_EGR_PIR_CIR_RATE_MAX;
    else
        queueCfg.cir = pQueuecfg->cir;

    if(pQueuecfg->pir == RT_MAX_RATE)
        queueCfg.pir = RT_RATE_PON_EGR_PIR_CIR_RATE_MAX;
    else
        queueCfg.pir = pQueuecfg->pir;
#endif
    queueCfg.egrssDrop  = DISABLED;

    queueCfg.type = (RT_EPON_LLID_QUEUE_SCHEDULER_SP == pQueuecfg->scheduleType)?STRICT_PRIORITY:WFQ_WRR_PRIORITY;
    queueCfg.weight = (RT_EPON_LLID_QUEUE_SCHEDULER_SP == pQueuecfg->scheduleType)?0:pQueuecfg->weight;

    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_queue_add(&queue,&queueCfg);
    RTK_API_UNLOCK();

    return ret;
}   /* end of rt_epon_ponQueue_set */

/* Function Name:
 *      rt_epon_ponQueue_get
 * Description:
 *      Get the pon queue
 * Input:
 *      llid    - logical llid index
 *      queueId  - logical llid queue id
 * Output:
 *      pQueuecfg - pointer to the queue configuration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The unit of granularity is 8Kbps.
 */
int32 
rt_epon_ponQueue_get(uint32 llid, uint32 queueId, rt_epon_queueCfg_t *pQueuecfg)
{
    int32   ret;
    rtk_ponmac_queue_t queue;
    rtk_ponmac_queueCfg_t queueCfg;

    /* function body */
    if (NULL == RT_MAPPER->ponmac_queue_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    queue.schedulerId = llid;
    queue.queueId = queueId;

    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_queue_get(&queue,&queueCfg);
    RTK_API_UNLOCK();

    pQueuecfg->scheduleType = queueCfg.type;
#ifdef CONFIG_LUNA_G3_SERIES
    pQueuecfg->cir = (queueCfg.cir >> 3);
    pQueuecfg->pir = (queueCfg.pir >> 3);
#else
    pQueuecfg->cir = queueCfg.cir;
    pQueuecfg->pir = queueCfg.pir;
#endif
    pQueuecfg->weight = queueCfg.weight;

    return ret;
}   /* end of rt_epon_ponQueue_get */

/* Function Name:
 *      rt_epon_ponQueue_del
 * Description:
 *      Delete the pon queue
 * Input:
 *      llid    - logical llid index
 *      queueId  - logical llid queue id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 */
int32 
rt_epon_ponQueue_del(uint32 llid, uint32 queueId)
{
    int32   ret;
    rtk_ponmac_queue_t queue;

    /* function body */
    if (NULL == RT_MAPPER->ponmac_queue_del)
        return RT_ERR_DRIVER_NOT_FOUND;

    queue.schedulerId = llid;
    queue.queueId = queueId;

    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_queue_del(&queue);
    RTK_API_UNLOCK();

    return ret;
}   /* end of rt_epon_ponQueue_del */

/* Function Name:
 *      rt_epon_egrBandwidthCtrlRate_get
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
 *      The unit of granularity is 1Kbps.
 */
int32
rt_epon_egrBandwidthCtrlRate_get(uint32 *pRate)
{
    int32   ret;
    uint32 tmp = 0;

    /* function body */
    if (NULL == RT_MAPPER->ponmac_egrBandwidthCtrlRate_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_egrBandwidthCtrlRate_get(&tmp);
    RTK_API_UNLOCK();

    *pRate = tmp;

    return ret;
}   /* end of rt_epon_egrBandwidthCtrlRate_get */

/* Function Name:
 *      rt_epon_egrBandwidthCtrlRate_set
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
 *      The unit of granularity is 1Kbps.
 */
int32
rt_epon_egrBandwidthCtrlRate_set(uint32 rate)
{
    int32   ret;
    uint32 tmp = 0;

    /* function body */
    if (NULL == RT_MAPPER->ponmac_egrBandwidthCtrlRate_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    if(rate == RT_MAX_RATE)
        tmp = RT_RATE_PON_EGR_RATE_MAX;
    else
    {
        tmp = rate;
    }

    RTK_API_LOCK();
    ret = RT_MAPPER->ponmac_egrBandwidthCtrlRate_set(tmp);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rt_epon_egrBandwidthCtrlRate_set */


/* Function Name:
 *      rt_epon_oam_tx
 * Description:
 *      Send OAM Packet.
 * Input:
 *      msgLen - length of OAM Packet
 *      pMsg - content of OAM Packet
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
rt_epon_oam_tx(uint32 msgLen,uint8 *pMsg,uint8 llidIdx)
{
    int32   ret = RT_ERR_OK;
    if(msgLen == 0||pMsg == NULL || llidIdx > 32)
        return RT_ERR_FAILED;

#ifdef CONFIG_LUNA_G3_SERIES    
    if (NULL == RT_MAPPER->rt_epon_oam_tx)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->rt_epon_oam_tx(msgLen, pMsg, llidIdx);
    RTK_API_UNLOCK();
#else
#if defined(CONFIG_SDK_KERNEL_LINUX)
    ret = rt_rtk_epon_oam_tx(msgLen, pMsg, llidIdx);
#endif
#endif
    return ret;
}   /* end of rt_epon_oam_tx */

/* Function Name:
 *      rt_epon_oam_tx
 * Description:
 *      Send OAM Packet.
 * Input:
 *      oamRx - call back function of OAM Packet
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32 
rt_epon_oam_rx_callback_register(rt_epon_oam_rx_callback oamRx)
{
    int32   ret = RT_ERR_OK;

    if(oamRx == NULL)
        return RT_ERR_FAILED;

#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_epon_oam_rx_callback_register)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->rt_epon_oam_rx_callback_register(oamRx);
    RTK_API_UNLOCK();
#else
#if defined(CONFIG_SDK_KERNEL_LINUX)
    ret = rt_rtk_epon_oam_rx_callback_register(oamRx);
#endif
#endif
    return ret;
}   /* end of rt_epon_oam_rx_callback_register */

/* Function Name:
 *      rt_epon_dyinggasp_set
 * Description:
 *      Set dying gasp OAM Packet.
 * Input:
 *      msgLen - length of dying gasp OAM Packet
 *      pMsg - content o dying gaspf OAM Packet
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32 
rt_epon_dyinggasp_set(uint32 msgLen,uint8 *pMsg)
{
    int32   ret = RT_ERR_OK;

    if(msgLen == 0||pMsg == NULL)
        return RT_ERR_FAILED;

#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_epon_dyinggasp_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->rt_epon_dyinggasp_set(msgLen, pMsg);
    RTK_API_UNLOCK();
#else
#if defined(CONFIG_SDK_KERNEL_LINUX)
    ret = rt_rtk_epon_dyinggasp_set(msgLen, pMsg);
#endif
#endif
    return ret;
}   /* end of rt_epon_dyinggasp_set */

/* Function Name:
 *      rt_epon_mpcp_info_get
 * Description:
 *      Get epon mpcp information
 * Input:
 * Output:
 *      info - mpcp relative information 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *
 *   This API routine is used to get generic ONU MPCP information and can be invoked at any time. 
 *   However some of fields like laser on laser_off sync_time and olt_discinfo will be valid if ONU is not in registered
 *   state. Note that field onu_discinfo and olt_discinfo are only appropriate for 10G ONU. 
 */
int32 
rt_epon_mpcp_info_get(rt_epon_mpcp_info_t *info)
{
    int32   ret = RT_ERR_OK;

    if(info == NULL)
        return RT_ERR_FAILED;

#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_epon_mpcp_info_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->rt_epon_mpcp_info_get(info);
    RTK_API_UNLOCK();
#endif    

    return ret;

}



/* Function Name:
 *      rt_epon_mpcp_queue_threshold_set
 * Description:
 *      Set epon threshold report
 * Input:
 *      llidIdx       - llid index
 *      pThresholdRpt - threshole report setting
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_INPUT            - Error Input
 * Note:
 *      The the report level 1 for normal report type, max value is 4.
 */
int32 rt_epon_mpcp_queue_threshold_set(rt_epon_report_threshold_t *pThresholdRpt)
{
    int32   ret = RT_ERR_OK;

    if(pThresholdRpt == NULL)
        return RT_ERR_FAILED;

#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_epon_mpcp_queue_threshold_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->rt_epon_mpcp_queue_threshold_set(pThresholdRpt);
    RTK_API_UNLOCK();
#endif        

    return ret;

}

/* Function Name:
 *      rt_epon_mpcp_queue_threshold_get
 * Description:
 *      Get epon threshold report setting
 * Input:
 *      pThresholdRpt - threshole report setting
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_INPUT            - Error Input
 * Note:
 */
int32
rt_epon_mpcp_queue_threshold_get(rt_epon_report_threshold_t *pThresholdRpt)
{
    int32   ret = RT_ERR_OK;

    if(pThresholdRpt == NULL)
        return RT_ERR_FAILED;

#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_epon_mpcp_queue_threshold_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->rt_epon_mpcp_queue_threshold_get(pThresholdRpt);
    RTK_API_UNLOCK();
#endif     
    
    return ret;
}



/* Function Name:
 *      rt_epon_mpcpRxregisterInfoCb_register
 * Description:
 *      Send OAM Packet.
 * Input:
 *      oamRx - call back function of mpcp register packet info update
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32 
rt_epon_mpcpRxregisterInfoCb_register(rt_epon_mpcp_rx_callback mpcpRx)
{
    int32   ret = RT_ERR_OK;
    if(mpcpRx == NULL)
        return RT_ERR_FAILED;
#ifdef CONFIG_LUNA_G3_SERIES

#else
#if defined(CONFIG_SDK_KERNEL_LINUX)
    ret = rt_rtk_mpcpRxregisterInfoCb_register(mpcpRx);
#endif
#endif


    return ret;
}   /* end of rt_epon_mpcpRxregisterInfoCb_register */


/* Function Name:
 *      rt_epon_info_notify_callback_register
 * Description:
 *      Send OAM Packet.
 * Input:
 *      callback - call back function of  call back function of mpcp register packet info update
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32 rt_epon_info_notify_callback_register(rt_epon_info_notify_cb callback)
{
    int32   ret = RT_ERR_OK;

    if(callback == NULL)
        return RT_ERR_FAILED;

#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_epon_info_notify_callback_register)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->rt_epon_info_notify_callback_register(callback);
    RTK_API_UNLOCK();
#endif        

    return ret;

}




/* Function Name:
 *      rt_epon_ponLoopback_set
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
int32 rt_epon_ponLoopback_set(uint32 llid,rt_enable_t enable)
{
    int32   ret = RT_ERR_OK;


#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_epon_ponLoopback_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->rt_epon_ponLoopback_set(llid,enable);
    RTK_API_UNLOCK();
#else
    if(llid);
    if(enable);
#endif

    return ret;

}



/* Function Name:
 *      rt_epon_mpcp_dbg_log_dump
 * Description:
 *      dump mpcp debug log
 * Input:
 *      overWrite - set log to overwrite mode
 *      clearLog  - dump log and clear old log
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32 rt_epon_mpcp_dbg_log_dump(rt_enable_t overWrite,rt_enable_t clearLog)
{
    int32   ret = RT_ERR_OK;

#ifdef CONFIG_LUNA_G3_SERIES
    if (NULL == RT_MAPPER->rt_epon_mpcp_dbg_log_dump)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->rt_epon_mpcp_dbg_log_dump(overWrite,clearLog);
    RTK_API_UNLOCK();
#else
    if(overWrite);
    if(clearLog);
#endif

#if defined(CONFIG_SDK_KERNEL_LINUX)
	printk("\n ==================");
	printk("\n debug info");
	printk("\n ==================");
	printk("\n [type]");
	printk("\n RT_EPON_INFO_MPCP_REG_SUCCESS             :%d",RT_EPON_INFO_MPCP_REG_SUCCESS);
	printk("\n RT_EPON_INFO_MPCP_REG_FAIL                :%d",RT_EPON_INFO_MPCP_REG_FAIL);
	printk("\n RT_EPON_INFO_MPCP_GATE_TIMEOUT            :%d",RT_EPON_INFO_MPCP_GATE_TIMEOUT);
	printk("\n RT_EPON_INFO_LASER_OFF                    :%d",RT_EPON_INFO_LASER_OFF);
	printk("\n RT_EPON_INFO_LASER_RECOVER                :%d",RT_EPON_INFO_LASER_RECOVER);
	printk("\n RT_EPON_INFO_MPCP_TIMEDRIFT               :%d",RT_EPON_INFO_MPCP_TIMEDRIFT);
	printk("\n RT_EPON_INFO_MPCP_LLID_ENTRY_DISABLE      :%d",RT_EPON_INFO_MPCP_LLID_ENTRY_DISABLE);
	printk("\n RT_EPON_INFO_MPCP_RX_REG_PKT              :%d",RT_EPON_INFO_MPCP_RX_REG_PKT);
	printk("\n RT_EPON_INFO_MPCP_LLID_ENTRY_DISABLE_BY_SW:%d",RT_EPON_INFO_MPCP_LLID_ENTRY_DISABLE_BY_SW);

	printk("\n RT_EPON_INFO_MPCP_LLID_ENTRY_DISABLE is all register state change from on line to offline except DISABLE_BY_SW");


	printk("\n [sub type] for RT_EPON_INFO_MPCP_RX_REG_PKT only for register flag value\n");
#endif	
    return ret;	
}



