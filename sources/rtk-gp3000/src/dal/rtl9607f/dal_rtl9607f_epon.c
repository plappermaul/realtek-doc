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
#include <rtk/epon.h>
#include <rtk/trap.h>
#include <rtk/l2.h>
#include <rtk/rt/rt_epon.h>
#include <hal/chipdef/rtl9607f/rtl9607f_def.h>
#include <dal/rtl9607f/dal_rtl9607f.h>
#include <dal/rtl9607f/dal_rtl9607f_epon.h>
#include <dal/rtl9607f/dal_rtl9607f_trap.h>
#include <dal/rtl9607f/dal_rtl9607f_l2.h>
#include <osal/time.h>

#include "osal_spinlock.h"
#include "cortina-api/include/epon.h"
#include "cortina-api/include/port.h"
#include "aal_pon.h"
#include "aal_epon.h"
#include "aal_mpcp.h"
#include "aal_puc.h"
#include "scfg.h"

static uint32 epon_init = {INIT_NOT_COMPLETED};

/* Function Name:
 *      dal_rtl9607f_epon_init
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
int32 dal_rtl9607f_epon_init(void)
{
    int32 ret=RT_ERR_OK;
    ca_uint32_t pon_mode = CA_PON_MODE;

#ifdef CONFIG_CORTINA_BOARD_FPGA
    printk("%s %d FPGA epon init\n",__FUNCTION__,__LINE__);
#else
    pon_mode = aal_pon_mac_mode_get(0);
    if(pon_mode != ONU_PON_MAC_EPON_D10G && pon_mode != ONU_PON_MAC_EPON_BI10G && pon_mode != ONU_PON_MAC_EPON_1G)
    {
        RT_ERR(ret, (MOD_DAL|MOD_EPON), "");
        epon_init = INIT_NOT_COMPLETED;
        return RT_ERR_OK;
    }
#endif
    epon_init = INIT_COMPLETED;

    return RT_ERR_OK;    
}   /* end of dal_rtl9607f_epon_init */

/* Function Name:
 *      dal_rtl9607f_epon_llid_entry_get
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
int32 dal_rtl9607f_epon_llid_entry_get(rtk_epon_llid_entry_t *pLlidEntry)  
{
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "");

    /* check Init status */
    RT_INIT_CHK(epon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pLlidEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((RTK_EPON_MAX_LLID <= pLlidEntry->llidIdx), RT_ERR_INPUT);
    
    /* function body */
    pLlidEntry->llid = 0x7fff;
    pLlidEntry->valid = DISABLED;
    pLlidEntry->reportTimer = 0;
    pLlidEntry->isReportTimeout = 0;

    return RT_ERR_OK;
}   /* end of dal_rtl9607f_epon_llid_entry_get */

/* Function Name:
 *      dal_rtl9607f_epon_llid_entry_set
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
int32 dal_rtl9607f_epon_llid_entry_set(rtk_epon_llid_entry_t *pLlidEntry)  
{
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "");

    /* parameter check */
    RT_PARAM_CHK((NULL == pLlidEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((RTK_EPON_MAX_LLID <= pLlidEntry->llidIdx), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= pLlidEntry->valid), RT_ERR_INPUT);

    /* function body */
    

    return RT_ERR_OK;
}   /* end of dal_rtl9607f_epon_llid_entry_set */

/* Function Name:
 *      dal_rtl9607f_epon_registerReq_get
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
int32 dal_rtl9607f_epon_registerReq_get(rtk_epon_regReq_t *pRegEntry)
{
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "");

    /* check Init status */
    RT_INIT_CHK(epon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pRegEntry), RT_ERR_NULL_POINTER);

    /* function body */
    pRegEntry->llidIdx = 0;
    pRegEntry->pendGrantNum = 0;
    pRegEntry->doRequest = DISABLED;

    return RT_ERR_OK;
}   /* end of dal_rtl9607f_epon_registerReq_get */

/* Function Name:
 *      dal_rtl9607f_epon_registerReq_set
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
int32 dal_rtl9607f_epon_registerReq_set(rtk_epon_regReq_t *pRegEntry)
{
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "");

    /* check Init status */
    RT_INIT_CHK(epon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pRegEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((RTK_EPON_MAX_LLID <= pRegEntry->llidIdx), RT_ERR_INPUT);

    /* function body */

    return RT_ERR_OK;
}   /* end of dal_rtl9607f_epon_registerReq_set */

/* Function Name:
 *      dal_rtl9607f_epon_churningKey_get
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
int32 dal_rtl9607f_epon_churningKey_get(rtk_epon_churningKeyEntry_t *pEntry)
{
    ca_uint32_t pon_mode;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "");

    /* check Init status */
    RT_INIT_CHK(epon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((2 < pEntry->keyIdx), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_EPON_MAX_LLID <= pEntry->llidIdx), RT_ERR_INPUT);

    /* function body */
    pon_mode = aal_pon_mac_mode_get(0);

    if(pon_mode == ONU_PON_MAC_EPON_1G)
    {
        pEntry->churningKey[0]=0;
        pEntry->churningKey[1]=0;
        pEntry->churningKey[2]=0;
        pEntry->churningKey1[0]=0;        
    
    }
    else
    {    
        pEntry->churningKey[0]=0;
        pEntry->churningKey[1]=0;
        pEntry->churningKey[2]=0;
        pEntry->churningKey1[0]=0;
        pEntry->churningKey1[1]=0;
        pEntry->churningKey1[2]=0;
        pEntry->churningKey2[0]=0;
        pEntry->churningKey2[1]=0;
        pEntry->churningKey2[2]=0;
    }


    return RT_ERR_OK;
}   /* end of dal_rtl9607f_epon_churningKey_set */

/* Function Name:
 *      dal_rtl9607f_epon_churningKey_set
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
int32 dal_rtl9607f_epon_churningKey_set(rtk_epon_churningKeyEntry_t *pEntry)
{
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "");

    /* check Init status */
    RT_INIT_CHK(epon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((2 < pEntry->keyIdx), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_EPON_MAX_LLID <= pEntry->llidIdx), RT_ERR_INPUT);
    /* function body */
    
    return RT_ERR_OK;
}   /* end of dal_rtl9607f_epon_churningKey_set */

/* Function Name:
 *      dal_rtl9607f_epon_usFecState_get
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
int32 dal_rtl9607f_epon_usFecState_get(rtk_enable_t *pState)
{
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "");

    /* check Init status */
    RT_INIT_CHK(epon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */
    *pState = DISABLED;

    return RT_ERR_OK;
}   /* end of dal_rtl9607f_epon_usFecState_get */

/* Function Name:
 *      dal_rtl9607f_epon_usFecState_set
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
int32 dal_rtl9607f_epon_usFecState_set(rtk_enable_t state)
{
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "state=%d",state);

    /* check Init status */
    RT_INIT_CHK(epon_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= state), RT_ERR_INPUT);

    /* function body */

    return RT_ERR_OK;
}   /* end of dal_rtl9607f_epon_usFecState_set */

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
int32 dal_rtl9607f_epon_dsFecState_get(rtk_enable_t *pState)
{
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "");

    /* check Init status */
    RT_INIT_CHK(epon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */
    *pState = DISABLED;

    return RT_ERR_OK;
}   /* end of dal_rtl9607f_epon_dsFecState_get */

/* Function Name:
 *      dal_rtl9607f_epon_dsFecState_set
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
int32 dal_rtl9607f_epon_dsFecState_set(rtk_enable_t state)
{
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "state=%d",state);

    /* check Init status */
    RT_INIT_CHK(epon_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= state), RT_ERR_INPUT);

    /* function body */

    return RT_ERR_OK;
}   /* end of dal_rtl9607f_epon_dsFecState_set */

/* Function Name:
 *      dal_rtl9607f_epon_mibCounter_get
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
int32 dal_rtl9607f_epon_mibCounter_get(rtk_epon_counter_t *pCounter)
{
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "state=%d",state);

    /* check Init status */
    RT_INIT_CHK(epon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pCounter), RT_ERR_NULL_POINTER);

    /* function body */
    memset(&pCounter->llidIdxCnt,0,sizeof(rtk_epon_llidCounter_t));

    return RT_ERR_OK;
}   /* end of dal_rtl9607f_epon_mibCounter_get */

/* Function Name:
 *      dal_rtl9607f_epon_losState_get
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
int32 dal_rtl9607f_epon_losState_get(rtk_enable_t *pState)
{
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "");

    /* check Init status */
    RT_INIT_CHK(epon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    /* function body */
    *pState = DISABLED;
    
    return RT_ERR_OK;
}   /* end of dal_apollomp_epon_losState_get */


/* Function Name:
 *      dal_rtl9607f_epon_llidEntryNum_get
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
int32 dal_rtl9607f_epon_llidEntryNum_get(uint32 *num)
{
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "");

    /* parameter check */
    RT_PARAM_CHK((NULL == num), RT_ERR_NULL_POINTER);

    /* function body */
    *num = RTL9607F_MAX_LLID_ENTRY;
    return RT_ERR_OK;
}   /* end of dal_rtl9607f_epon_llidEntryNum_get */



/* Function Name:
 *      dal_rtl9607f_epon_mpcp_info_get
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
dal_rtl9607f_epon_mpcp_info_get(rt_epon_mpcp_info_t *info)
{    
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "");

    /* check Init status */
    RT_INIT_CHK(epon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == info), RT_ERR_NULL_POINTER);

    /* function body */
    memset(info,0,sizeof(rt_epon_mpcp_info_t));

    return RT_ERR_OK;
}  /* end of dal_rtl9607f_epon_mpcp_info_get */


/* Function Name:
 *      dal_rtl9607f_epon_mpcp_queue_threshold_set
 * Description:
 *      Set epon threshold report
 * Input:
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
int32 dal_rtl9607f_epon_mpcp_queue_threshold_set(rt_epon_report_threshold_t *pThresholdRpt)
{
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "");

    /* check Init status */
    RT_INIT_CHK(epon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pThresholdRpt), RT_ERR_NULL_POINTER);

    /* function body */
    memset(pThresholdRpt,0,sizeof(rt_epon_report_threshold_t));

    return RT_ERR_OK;  
}  /* end of dal_rtl9607f_epon_mpcp_queue_threshold_set */


/* Function Name:
 *      dal_rtl9607f_epon_mpcp_queue_threshold_get
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
dal_rtl9607f_epon_mpcp_queue_threshold_get(rt_epon_report_threshold_t *pThresholdRpt)
{
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_EPON), "");

    /* check Init status */
    RT_INIT_CHK(epon_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pThresholdRpt), RT_ERR_NULL_POINTER);

    /* function body */

    return RT_ERR_OK; 
} /* end of dal_rtl9607f_epon_mpcp_queue_threshold_get */
