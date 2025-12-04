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
 * $Date: 2013-10-16 $
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
#include <rtk/port.h>
#include <rtk/trap.h>
#include <rtk/l2.h>
#include <hal/chipdef/ca8279/ca8279_def.h>
#include <dal/ca8279/dal_ca8279.h>
#include <dal/ca8279/dal_ca8279_epon.h>
#include <dal/ca8279/dal_ca8279_trap.h>
#include <dal/ca8279/dal_ca8279_l2.h>
#include <osal/time.h>

#include "osal_spinlock.h"
#include "cortina-api/include/epon.h"
#include "cortina-api/include/port.h"
#include "aal_pon.h"
#include "aal_epon.h"
#include "aal_mpcp.h"
#include "scfg.h"

static uint32 epon_init = {INIT_NOT_COMPLETED};

static uint8 regLlidIdx = 0;

/* Function Name:
 *      dal_ca8279_epon_init
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
int32 dal_ca8279_epon_init(void)
{
    int32 ret=RT_ERR_OK;
    ca_uint32_t pon_mode;

    pon_mode = aal_pon_mac_mode_get(0);

    if(pon_mode != ONU_PON_MAC_EPON_D10G && pon_mode != ONU_PON_MAC_EPON_BI10G)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        epon_init = INIT_NOT_COMPLETED;
        return RT_ERR_OK;
    }

    if((ret = dal_ca8279_trap_oamPduAction_set(ACTION_TRAP2CPU)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        epon_init = INIT_NOT_COMPLETED;
        return RT_ERR_OK;
    }

    epon_init = INIT_COMPLETED;
//ca_epon_llid_encryption_active_key_index_set
//ca_port_encryption_mode_set

    return RT_ERR_OK;    
}   /* end of dal_ca8279_epon_init */

/* Function Name:
 *      dal_ca8279_epon_llid_entry_get
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
int32 dal_ca8279_epon_llid_entry_get(rtk_epon_llid_entry_t *pLlidEntry)  
{
    ca_status_t ret=CA_E_OK;
    rtk_port_t pon_port;
    ca_port_id_t port_id;
    ca_llid_t  llid;
    ca_mac_addr_t mac;
    ca_boolean_t reg_flag;
    ca_uint16_t reg_llid;
    rtk_enable_t losState = ENABLED;

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

    ret = dal_ca8279_epon_losState_get(&losState);
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
}   /* end of dal_ca8279_epon_llid_entry_get */

/* Function Name:
 *      dal_ca8279_epon_llid_entry_set
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
int32 dal_ca8279_epon_llid_entry_set(rtk_epon_llid_entry_t *pLlidEntry)  
{
    ca_status_t ret=CA_E_OK;
    rtk_port_t pon_port;
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
}   /* end of dal_ca8279_epon_llid_entry_set */

/* Function Name:
 *      dal_ca8279_epon_registerReq_get
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
int32 dal_ca8279_epon_registerReq_get(rtk_epon_regReq_t *pRegEntry)
{
    ca_status_t ret=CA_E_OK;
    rtk_port_t pon_port;
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
}   /* end of dal_ca8279_epon_registerReq_get */

/* Function Name:
 *      dal_ca8279_epon_registerReq_set
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
int32 dal_ca8279_epon_registerReq_set(rtk_epon_regReq_t *pRegEntry)
{
    ca_status_t ret=CA_E_OK;
    rtk_port_t pon_port;
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
}   /* end of dal_ca8279_epon_registerReq_set */

/* Function Name:
 *      dal_ca8279_epon_churningKey_get
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
int32 dal_ca8279_epon_churningKey_get(rtk_epon_churningKeyEntry_t *pEntry)
{
    ca_status_t ret=CA_E_OK;
    rtk_port_t pon_port;
    ca_port_id_t port_id;
    ca_llid_t llid;
    ca_uint32_t key_id;
    ca_epon_port_encryption_key_t key;

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

    pEntry->churningKey[0]=key.data[0];
    pEntry->churningKey[1]=key.data[1];
    pEntry->churningKey[2]=key.data[2];
    pEntry->churningKey1[0]=key.data[3];
    pEntry->churningKey1[1]=key.data[4];
    pEntry->churningKey1[2]=key.data[5];
    pEntry->churningKey2[0]=key.data[6];
    pEntry->churningKey2[1]=key.data[7];
    pEntry->churningKey2[2]=key.data[8];

    return RT_ERR_OK;
}   /* end of dal_ca8279_epon_churningKey_set */

/* Function Name:
 *      dal_ca8279_epon_churningKey_set
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
int32 dal_ca8279_epon_churningKey_set(rtk_epon_churningKeyEntry_t *pEntry)
{
    ca_status_t ret=CA_E_OK;
    rtk_port_t pon_port;
    ca_port_id_t port_id;
    ca_llid_t llid;
    ca_uint32_t key_id;
    ca_epon_port_encryption_key_t key;

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

    if((ret = ca_port_encryption_enable_set(0,port_id,CA_DIRECTION_ENC_RX,1))!=CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        return RT_ERR_FAILED;
    }

    key.data[0]=pEntry->churningKey[0];
    key.data[1]=pEntry->churningKey[1];
    key.data[2]=pEntry->churningKey[2];
    key.data[3]=pEntry->churningKey1[0];
    key.data[4]=pEntry->churningKey1[1];
    key.data[5]=pEntry->churningKey1[2];
    key.data[6]=pEntry->churningKey2[0];
    key.data[7]=pEntry->churningKey2[1];
    key.data[8]=pEntry->churningKey2[2];

    if((ret = ca_epon_llid_encryption_key_set(0,port_id,llid,key_id,&key))!=CA_E_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}   /* end of dal_ca8279_epon_churningKey_set */

/* Function Name:
 *      dal_ca8279_epon_usFecState_get
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
int32 dal_ca8279_epon_usFecState_get(rtk_enable_t *pState)
{
    ca_status_t ret=CA_E_OK;
    rtk_port_t pon_port;
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
}   /* end of dal_ca8279_epon_usFecState_get */

/* Function Name:
 *      dal_ca8279_epon_usFecState_set
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
int32 dal_ca8279_epon_usFecState_set(rtk_enable_t state)
{
    ca_status_t ret=CA_E_OK;
    rtk_port_t pon_port;
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
}   /* end of dal_ca8279_epon_usFecState_set */

/* Function Name:
 *      dal_ca8279_epon_dsFecState_get
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
int32 dal_ca8279_epon_dsFecState_get(rtk_enable_t *pState)
{
    ca_status_t ret=CA_E_OK;
    rtk_port_t pon_port;
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
}   /* end of dal_ca8279_epon_dsFecState_get */

/* Function Name:
 *      dal_ca8279_epon_dsFecState_set
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
int32 dal_ca8279_epon_dsFecState_set(rtk_enable_t state)
{
    ca_status_t ret=CA_E_OK;
    rtk_port_t pon_port;
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
}   /* end of dal_ca8279_epon_dsFecState_set */

/* Function Name:
 *      dal_ca8279_epon_mibCounter_get
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
int32 dal_ca8279_epon_mibCounter_get(rtk_epon_counter_t *pCounter)
{
    ca_status_t ret=CA_E_OK;
    rtk_port_t pon_port;
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

    memset(&pCounter->llidIdxCnt,0,sizeof(rtk_epon_llidCounter_t));
    
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
}   /* end of dal_ca8279_epon_mibCounter_get */

/* Function Name:
 *      dal_ca8279_epon_losState_get
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
int32 dal_ca8279_epon_losState_get(rtk_enable_t *pState)
{
    ca_status_t ret=CA_E_OK;
    rtk_port_t pon_port;
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
}   /* end of dal_apollomp_epon_losState_get */


/* Function Name:
 *      dal_ca8279_epon_llidEntryNum_get
 * Description:
 *      Get EPON support LLID entry number
 * Input:
 *       None
 * Output:
 *       *pState : glogal FEC state 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 dal_ca8279_epon_llidEntryNum_get(uint32 *num)
{
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "");

    /* parameter check */
    RT_PARAM_CHK((NULL == num), RT_ERR_NULL_POINTER);

    /* function body */
    *num = CA8279_MAX_LLID_ENTRY;
    return RT_ERR_OK;
}   /* end of dal_ca8279_epon_llidEntryNum_get */
