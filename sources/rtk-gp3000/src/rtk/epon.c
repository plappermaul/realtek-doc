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
 * $Revision$
 * $Date$
 *
 * Purpose : EPON MAC register access APIs
 *
 * Feature : Provide the APIs to access EPON MAC
 *
 */

#include <common/rt_error.h>
#include <common/rt_type.h>
#include <rtk/epon.h> 
#include <dal/dal_mgmt.h> 
#include <osal/lib.h>
#include <rtk/init.h> 

static char churningStatus = DISABLED;


/* Function Name:
 *      rtk_epon_init
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
int32 rtk_epon_init(void)  
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->epon_init)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->epon_init();
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_epon_init */




/* Function Name:
 *      rtk_epon_intrMask_get
 * Description:
 *      Get EPON interrupt mask
 * Input:
 *	  intrType: type of interrupt
 * Output:
 *      pState: point of get interrupt state
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_epon_intrMask_get(rtk_epon_intrType_t intrType, rtk_enable_t *pState)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->epon_intrMask_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->epon_intrMask_get(intrType, pState);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_epon_intrMask_get */


/* Function Name:
 *      rtk_epon_intrMask_set
 * Description:
 *      Set EPON interrupt mask
 * Input:
 * 	    intrType: type of top interrupt
 *      state: set interrupt mask state
 * Output:
 *      
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_epon_intrMask_set(rtk_epon_intrType_t intrType, rtk_enable_t state)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->epon_intrMask_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->epon_intrMask_set(intrType, state);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_epon_intrMask_set */




/* Function Name:
 *      rtk_epon_intr_get
 * Description:
 *      Set EPON interrupt state
 * Input:
 * 	  intrType: type of interrupt
 * Output:
 *      pState: point for get  interrupt state
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_epon_intr_get(rtk_epon_intrType_t intrType, rtk_enable_t *pState)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->epon_intr_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->epon_intr_get(intrType, pState);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_epon_intr_get */


/* Function Name:
 *      rtk_epon_intr_disableAll
 * Description:
 *      Disable all of top interrupt for EPON
 * Input:
 *
 * Output:
 *      
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_epon_intr_disableAll(void)  
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->epon_intr_disableAll)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->epon_intr_disableAll();
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_epon_intr_disableAll */



/* Function Name:
 *      rtk_epon_llid_entry_set
 * Description:
 *      Set llid entry
 * Input:
 *      None
 * Output:
 *      pLlidEntry: LLID entry 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_epon_llid_entry_set(rtk_epon_llid_entry_t *pLlidEntry)  
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->epon_llid_entry_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->epon_llid_entry_set(pLlidEntry);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_epon_llid_entry_set */

/* Function Name:
 *      rtk_epon_llid_entry_get
 * Description:
 *      Get llid entry
 * Input:
 *      None
 * Output:
 *      pLlidEntry: LLID entry 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_epon_llid_entry_get(rtk_epon_llid_entry_t *pLlidEntry)  
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->epon_llid_entry_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->epon_llid_entry_get(pLlidEntry);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_epon_llid_entry_get */


/* Function Name:
 *      rtk_epon_forceLaserState_set
 * Description:
 *      Set Force Laser status
 * Input:
 *      laserStatus: Force laser status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32  rtk_epon_forceLaserState_set(rtk_epon_laser_status_t laserStatus)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->epon_forceLaserState_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->epon_forceLaserState_set(laserStatus);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_epon_forceLaserState_set */

/* Function Name:
 *      rtk_epon_forceLaserState_get
 * Description:
 *      Get Force Laser status
 * Input:
 *      None
 * Output:
 *      pLaserStatus: Force laser status
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_epon_forceLaserState_get(rtk_epon_laser_status_t *pLaserStatus)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->epon_forceLaserState_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->epon_forceLaserState_get(pLaserStatus);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_epon_forceLaserState_get */


/* Function Name:
 *      rtk_epon_laserTime_set
 * Description:
 *      Set laserTime value
 * Input:
 *      lasetOnTime:  OE module laser on time
 *      lasetOffTime: OE module laser off time
 * Output:
 *      plaserStatus: 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32  rtk_epon_laserTime_set(uint8 lasetOnTime, uint8 lasetOffTime)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->epon_laserTime_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->epon_laserTime_set(lasetOnTime, lasetOffTime);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_epon_laserTime_set */

/* Function Name:
 *      rtk_epon_laserTime_get
 * Description:
 *      Get laser Time value
 * Input:
 *      None
 * Output:
 *      pLasetOnTime:  OE module laser on time
 *      pLasetOffTime: OE module laser off time
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_epon_laserTime_get(uint8 *pLasetOnTime, uint8 *pLasetOffTime)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->epon_laserTime_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->epon_laserTime_get(pLasetOnTime, pLasetOffTime);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_epon_laserTime_get */

/* Function Name:
 *      rtk_epon_syncTime_get
 * Description:
 *      Get sync Time value
 * Input:
 *      None
 * Output:
 *      pSyncTime  : olt assigned sync time
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_epon_syncTime_get(uint8 *pSyncTime)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->epon_syncTime_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->epon_syncTime_get(pSyncTime);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_epon_syncTime_get */


/* Function Name:
 *      rtk_epon_registerReq_get
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
int32 rtk_epon_registerReq_get(rtk_epon_regReq_t *pRegEntry)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->epon_registerReq_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->epon_registerReq_get(pRegEntry);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_epon_registerReq_get */

/* Function Name:
 *      rtk_epon_registerReq_set
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
int32 rtk_epon_registerReq_set(rtk_epon_regReq_t *pRegEntry)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->epon_registerReq_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->epon_registerReq_set(pRegEntry);
    RTK_API_UNLOCK();

    if((RT_ERR_OK == ret) && (ENABLED == pRegEntry->doRequest))
    {
        churningStatus = DISABLED;
    }

    return ret;
}   /* end of rtk_epon_registerReq_set */



/* Function Name:
 *      rtk_epon_churningKey_set
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
int32 rtk_epon_churningKey_set(rtk_epon_churningKeyEntry_t *pEntry)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->epon_churningKey_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->epon_churningKey_set(pEntry);
    RTK_API_UNLOCK();

    if(RT_ERR_OK == ret)
    {
        churningStatus = ENABLED;
    }

    return ret;
}   /* end of rtk_epon_churningKey_set */


/* Function Name:
 *      rtk_epon_churningKey_get
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
int32 rtk_epon_churningKey_get(rtk_epon_churningKeyEntry_t *pEntry)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->epon_churningKey_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->epon_churningKey_get(pEntry);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_epon_churningKey_set */


/* Function Name:
 *      rtk_epon_usFecState_get
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
int32 rtk_epon_usFecState_get(rtk_enable_t *pState)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->epon_usFecState_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->epon_usFecState_get(pState);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_epon_usFecState_get */


/* Function Name:
 *      rtk_epon_usFecState_set
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
int32 rtk_epon_usFecState_set(rtk_enable_t state)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->epon_usFecState_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->epon_usFecState_set(state);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_epon_usFecState_set */


/* Function Name:
 *      rtk_epon_dsFecState_get
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
int32 rtk_epon_dsFecState_get(rtk_enable_t *pState)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->epon_dsFecState_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->epon_dsFecState_get(pState);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_epon_dsFecState_get */


/* Function Name:
 *      rtk_epon_dsFecState_set
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
int32 rtk_epon_dsFecState_set(rtk_enable_t state)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->epon_dsFecState_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->epon_dsFecState_set(state);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_epon_dsFecState_set */


/* Function Name:
 *      rtk_epon_dsFecState_set
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
int32 rtk_epon_mibCounter_get(rtk_epon_counter_t *pCounter)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->epon_mibCounter_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->epon_mibCounter_get(pCounter);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_epon_mibCounter_get */



/* Function Name:
 *      rtk_epon_mibGlobal_reset
 * Description:
 *      Reset EPON global counters.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_STAT_GLOBAL_CNTR_FAIL - Could not reset Global Counter
 * Note:
 *      None
 */
int32
rtk_epon_mibGlobal_reset(void)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->epon_mibGlobal_reset)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->epon_mibGlobal_reset();
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_epon_mibGlobal_reset */


/* Function Name:
 *      rtk_epon_mibLlidIdx_reset
 * Description:
 *      Reset the specified LLID index counters.
 * Input:
 *      llidIdx - LLID table index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
rtk_epon_mibLlidIdx_reset(uint8 llidIdx)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->epon_mibLlidIdx_reset)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->epon_mibLlidIdx_reset(llidIdx);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_epon_mibLlidIdx_reset */




/* Function Name:
 *      rtk_epon_losState_get
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
rtk_epon_losState_get(rtk_enable_t *pState)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->epon_losState_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->epon_losState_get(pState);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_epon_losState_get */


/* Function Name:
 *      rtk_epon_mpcpTimeoutVal_get
 * Description:
 *      Get mpcp time out value.
 * Input:
 *      pTimeVal - pointer of mpcp tomeout value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
rtk_epon_mpcpTimeoutVal_get(uint32 *pTimeVal)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->epon_mpcpTimeoutVal_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->epon_mpcpTimeoutVal_get(pTimeVal);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_epon_mpcpTimeoutVal_get */



/* Function Name:
 *      rtk_epon_mpcpTimeoutVal_set
 * Description:
 *      Set mpcp time out value.
 * Input:
 *      timeVal - mpcp tomeout value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
rtk_epon_mpcpTimeoutVal_set(uint32 timeVal)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->epon_mpcpTimeoutVal_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->epon_mpcpTimeoutVal_set(timeVal);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_epon_mpcpTimeoutVal_set */



/* Function Name:
 *      rtk_epon_opticalPolarity_set
 * Description:
 *      Set OE module polarity.
 * Input:
 *      polarity - OE module polarity
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
rtk_epon_opticalPolarity_set(rtk_epon_polarity_t polarity)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->epon_opticalPolarity_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->epon_opticalPolarity_set(polarity);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_epon_opticalPolarity_set */

/* Function Name:
 *      rtk_epon_opticalPolarity_get
 * Description:
 *      Set OE module polarity.
 * Input:
 *      pPolarity - pointer of OE module polarity
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
rtk_epon_opticalPolarity_get(rtk_epon_polarity_t *pPolarity)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->epon_opticalPolarity_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->epon_opticalPolarity_get(pPolarity);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_epon_opticalPolarity_get */




/* Function Name:
 *      rtk_epon_fecState_get
 * Description:
 *      Get EPON global fec state
 * Input:
 *       None
 * Output:
 *       *pState : glogal FEC state 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_epon_fecState_get(rtk_enable_t *pState)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->epon_fecState_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->epon_fecState_get(pState);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_epon_fecState_get */


/* Function Name:
 *      rtk_epon_fecState_set
 * Description:
 *      Set EPON global fec state
 * Input:
 *       state : global FEC state
 * Output:
 *       None 
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_epon_fecState_set(rtk_enable_t state)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->epon_fecState_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->epon_fecState_set(state);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_epon_fecState_set */

/* Function Name:
 *      rtk_epon_llidEntryNum_get
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
int32 rtk_epon_llidEntryNum_get(uint32 *num)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->epon_llidEntryNum_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->epon_llidEntryNum_get(num);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_epon_llidEntryNum_get */

/* Function Name:
 *      rtk_epon_thresholdReport_set
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
int32
rtk_epon_thresholdReport_set(uint8 llidIdx, rtk_epon_report_threshold_t *pThresholdRpt)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->epon_thresholdReport_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->epon_thresholdReport_set(llidIdx, pThresholdRpt);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_epon_thresholdReport_set */

/* Function Name:
 *      rtk_epon_thresholdReport_get
 * Description:
 *      Get epon threshold report setting
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
 */
int32
rtk_epon_thresholdReport_get(uint8 llidIdx, rtk_epon_report_threshold_t *pThresholdRpt)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->epon_thresholdReport_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->epon_thresholdReport_get(llidIdx, pThresholdRpt);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_epon_thresholdReport_get */

/* Function Name:
 *      rtk_epon_dbgInfo_get
 * Description:
 *      Get EPON debug information
 * Input:
 *	  intrType: type of interrupt
 * Output:
 *      pDbgCnt: point of relative debug counter
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_epon_dbgInfo_get(rtk_epon_dbgCnt_t *pDbgCnt)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->epon_dbgInfo_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->epon_dbgInfo_get(pDbgCnt);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_epon_dbgInfo_get */

/* Function Name:
 *      rtk_epon_churningStatus_get
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
int32 rtk_epon_churningStatus_get(rtk_enable_t *pEnable)
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
}

/* Function Name:
 *      rtk_epon_forcePRBS_set
 * Description:
 *      Set force PRBS status
 * Input:
 *      prbsCfg             - PRBS config
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_epon_forcePRBS_set(rtk_epon_prbs_t prbsCfg)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->epon_forcePRBS_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->epon_forcePRBS_set(prbsCfg);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_epon_forcePRBS_set */

/* Function Name:
 *      rtk_epon_forcePRBS_get
 * Description:
 *      Get force PRBS status
 * Input:
 *      None
 * Output:
 *      pPrbsCfg             - PRBS config
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_epon_forcePRBS_get(rtk_epon_prbs_t *pPrbsCfg)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->epon_forcePRBS_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->epon_forcePRBS_get(pPrbsCfg);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_epon_forcePRBS_get */

/* Function Name:
 *      rtk_epon_multiLlidMode_set
 * Description:
 *      Set multiple LLID mode
 * Input:
 *      mode             - multiple LLID mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 *      
 */
int32 rtk_epon_multiLlidMode_set(rtk_epon_multiLlidMode_t mode)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->epon_multiLlidMode_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->epon_multiLlidMode_set(mode);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_epon_multiLlidMode_set */


/* Function Name:
 *      rtk_epon_multiLlidMode_get
 * Description:
 *      Get multiple LLID mode
 * Input:
 *      pMode             - multiple LLID mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
int32 rtk_epon_multiLlidMode_get(rtk_epon_multiLlidMode_t *pMode)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->epon_multiLlidMode_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->epon_multiLlidMode_get(pMode);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_epon_multiLlidMode_get */



/* Function Name:
 *      rtk_epon_reportQueueSet_set
 * Description:
 *      Set epon mpcp report queue set 
 * Input:
 *      queueSetNum       - queue set number
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_INPUT            - Error Input
 * Note:
 *      The the report queue set 1 for normal report type, max value is 4.
 */
int32
rtk_epon_reportQueueSet_set(uint8 queueSetNum)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->epon_reportQueueSet_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->epon_reportQueueSet_set(queueSetNum);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_epon_reportQueueSet_set */

/* Function Name:
 *      rtk_epon_reportQueueSet_get
 * Description:
 *      Get epon mpcp report queue set 
 * Input:
 *      pQueueSetNum       -  queue set number
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_INPUT            - Error Input
 * Note:
 */
int32
rtk_epon_reportQueueSet_get(uint8 *pQueueSetNum)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->epon_reportQueueSet_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->epon_reportQueueSet_get(pQueueSetNum);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_epon_reportQueueSet_get */




/* Function Name:
 *      rtk_epon_queueThresholdReport_set
 * Description:
 *      Set epon per queue threshold report
 * Input:
 *      queue       - queue id
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
int32
rtk_epon_queueThresholdReport_set(uint8 queue, rtk_epon_report_threshold_t *pThresholdRpt)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->epon_queueThresholdReport_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->epon_queueThresholdReport_set(queue, pThresholdRpt);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_epon_queueThresholdReport_set */



/* Function Name:
 *      rtk_epon_queueThresholdReport_get
 * Description:
 *      Set epon per queue threshold report
 * Input:
 *      queue       - queue id
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
int32
rtk_epon_queueThresholdReport_get(uint8 queue, rtk_epon_report_threshold_t *pThresholdRpt)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->epon_queueThresholdReport_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->epon_queueThresholdReport_get(queue, pThresholdRpt);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_epon_queueThresholdReport_get */



/* Function Name:
 *      rtk_epon_autoPowerSavingState_get
 * Description:
 *      get epon auto power saving state
 * Input:
 *	    pEnable - function state
 * Output:
 *      pEnabled: pointer of status
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_NULL_POINTER - NULL input parameter 
 * Note:
 */
int32 rtk_epon_autoPowerSavingState_get(rtk_enable_t *pEnable)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->epon_autoPowerSavingState_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->epon_autoPowerSavingState_get(pEnable);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_epon_autoPowerSavingState_get */


/* Function Name:
 *      rtk_epon_autoPowerSavingState_set
 * Description:
 *      set epon auto power saving state
 * Input:
 *	    enable - function state
 * Output:
 *      pEnabled: pointer of status
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_NULL_POINTER - NULL input parameter 
 * Note:
 */
int32 rtk_epon_autoPowerSavingState_set(rtk_enable_t enable)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->epon_autoPowerSavingState_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->epon_autoPowerSavingState_set(enable);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_epon_autoPowerSavingState_set */




/* Function Name:
 *      rtk_epon_autoPowerSavingOeRecoverTime_get
 * Description:
 *      get epon auto power saving OE recover time
 * Input:
 *	    pEnable - function state
 * Output:
 *      pEnabled: pointer of status
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_NULL_POINTER - NULL input parameter 
 * Note:
 */
int32 rtk_epon_autoPowerSavingOeRecoverTime_get(int *pRecoverTime)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->epon_autoPowerSavingOeRecoverTime_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->epon_autoPowerSavingOeRecoverTime_get(pRecoverTime);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_epon_autoPowerSavingOeRecoverTime_get */


/* Function Name:
 *      rtk_epon_autoPowerSavingOeRecoverTime_set
 * Description:
 *      set epon auto power saving OE recover time
 * Input:
 *	    enable - function state
 * Output:
 *      pEnabled: pointer of status
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_NULL_POINTER - NULL input parameter 
 * Note:
 */
int32 rtk_epon_autoPowerSavingOeRecoverTime_set(int recoverTime)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->epon_autoPowerSavingOeRecoverTime_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->epon_autoPowerSavingOeRecoverTime_set(recoverTime);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_epon_autoPowerSavingOeRecoverTime_set */


/* Function Name:
 *      rtk_epon_info_notify_callback_register
 * Description:
 *      set epon info notify callback
 * Input:
 *	    enable - function state
 * Output:
 *      pEnabled: pointer of status
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_NULL_POINTER - NULL input parameter 
 * Note:
 */
int32 rtk_epon_info_notify_callback_register(epon_info_notify_callback callBack)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->epon_info_notify_callback_register)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->epon_info_notify_callback_register(callBack);
    RTK_API_UNLOCK();
    return ret;   
}



/* Function Name:
 *      rtk_epon_mpcpTimeout_set
 * Description:
 *      set mpcp/gate timeout value
 * Input:
 *	    gateTimeout - get timeout value unit 10ms
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_NULL_POINTER - NULL input parameter 
 * Note:
 */
int32 rtk_epon_mpcpTimeout_set(uint32 gateTimeout)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->epon_mpcpTimeout_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->epon_mpcpTimeout_set(gateTimeout);
    RTK_API_UNLOCK();
    return ret;   
}


/* Function Name:
 *      rtk_epon_mpcpTimeout_get
 * Description:
 *      set mpcp/gate timeout value
 * Input:
 *	    pGateTimeout - get timeout value unit 10ms
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_NULL_POINTER - NULL input parameter 
 * Note:
 */
int32 rtk_epon_mpcpTimeout_get(uint32 *pGateTimeout)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->epon_mpcpTimeout_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->epon_mpcpTimeout_get(pGateTimeout);
    RTK_API_UNLOCK();
    return ret;   
}
