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
 * $Date: 2012-10-16
 *
 * Purpose : EPON MAC register access APIs
 *
 * Feature : Provide the APIs to access EPON MAC
 *
 */
#ifndef _DAL_RTL9601B_EPON_H_
#define _DAL_RTL9601B_EPON_H_

#include <common/rt_error.h>
#include <common/rt_type.h>
#include <rtk/epon.h> 


/* Function Name:
 *      dal_rtl9601b_epon_init
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
extern int32 dal_rtl9601b_epon_init(void);  




/* Function Name:
 *      dal_rtl9601b_epon_intrMask_get
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
extern int32 dal_rtl9601b_epon_intrMask_get(rtk_epon_intrType_t intrType, rtk_enable_t *pState);


/* Function Name:
 *      dal_rtl9601b_epon_intrMask_set
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
extern int32 dal_rtl9601b_epon_intrMask_set(rtk_epon_intrType_t intrType, rtk_enable_t state);




/* Function Name:
 *      dal_rtl9601b_epon_topIntr_get
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
extern int32 dal_rtl9601b_epon_intr_get(rtk_epon_intrType_t intrType, rtk_enable_t *pState);


/* Function Name:
 *      dal_rtl9601b_epon_intr_disableAll
 * Description:
 *      Disable all of interrupt for EPON
 * Input:
 *
 * Output:
 *      
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 dal_rtl9601b_epon_intr_disableAll(void);  


/* Function Name:
 *      dal_rtl9601b_epon_llid_entry_set
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
extern int32 dal_rtl9601b_epon_llid_entry_set(rtk_epon_llid_entry_t *pLlidEntry);  

/* Function Name:
 *      dal_rtl9601b_epon_llid_entry_get
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
extern int32 dal_rtl9601b_epon_llid_entry_get(rtk_epon_llid_entry_t *pLlidEntry);  


/* Function Name:
 *      dal_rtl9601b_epon_forceLaserState_set
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
extern int32  dal_rtl9601b_epon_forceLaserState_set(rtk_epon_laser_status_t laserStatus);

/* Function Name:
 *      dal_rtl9601b_epon_forceLaserState_get
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
extern int32 dal_rtl9601b_epon_forceLaserState_get(rtk_epon_laser_status_t *pLaserStatus);


/* Function Name:
 *      dal_rtl9601b_epon_laserTime_set
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
extern int32  dal_rtl9601b_epon_laserTime_set(uint8 lasetOnTime, uint8 lasetOffTime);

/* Function Name:
 *      dal_rtl9601b_epon_laserTime_get
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
extern int32 dal_rtl9601b_epon_laserTime_get(uint8 *pLasetOnTime, uint8 *pLasetOffTime);

/* Function Name:
 *      dal_rtl9601b_epon_syncTime_get
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
extern int32 dal_rtl9601b_epon_syncTime_get(uint8 *pSyncTime);


/* Function Name:
 *      dal_rtl9601b_epon_registerReq_get
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
extern int32 dal_rtl9601b_epon_registerReq_get(rtk_epon_regReq_t *pRegEntry);

/* Function Name:
 *      dal_rtl9601b_epon_registerReq_set
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
extern int32 dal_rtl9601b_epon_registerReq_set(rtk_epon_regReq_t *pRegEntry);



/* Function Name:
 *      dal_rtl9601b_epon_churningKey_set
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
extern int32 dal_rtl9601b_epon_churningKey_set(rtk_epon_churningKeyEntry_t *pEntry);


/* Function Name:
 *      dal_rtl9601b_epon_churningKey_get
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
extern int32 dal_rtl9601b_epon_churningKey_get(rtk_epon_churningKeyEntry_t *pEntry);


/* Function Name:
 *      dal_rtl9601b_epon_usFecState_get
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
extern int32 dal_rtl9601b_epon_usFecState_get(rtk_enable_t *pState);


/* Function Name:
 *      dal_rtl9601b_epon_usFecState_set
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
extern int32 dal_rtl9601b_epon_usFecState_set(rtk_enable_t state);


/* Function Name:
 *      dal_rtl9601b_epon_dsFecState_get
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
extern int32 dal_rtl9601b_epon_dsFecState_get(rtk_enable_t *pState);


/* Function Name:
 *      dal_rtl9601b_epon_dsFecState_set
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
extern int32 dal_rtl9601b_epon_dsFecState_set(rtk_enable_t state);


/* Function Name:
 *      dal_rtl9601b_epon_dsFecState_set
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
extern int32 dal_rtl9601b_epon_mibCounter_get(rtk_epon_counter_t *pCounter);



/* Function Name:
 *      dal_rtl9601b_epon_mibGlobal_reset
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
extern int32
dal_rtl9601b_epon_mibGlobal_reset(void);


/* Function Name:
 *      dal_rtl9601b_epon_mibLlidIdx_reset
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
extern int32
dal_rtl9601b_epon_mibLlidIdx_reset(uint8 llidIdx);




/* Function Name:
 *      dal_rtl9601b_epon_mpcpTimeoutVal_get
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
extern int32
dal_rtl9601b_epon_mpcpTimeoutVal_get(uint32 *pTimeVal);



/* Function Name:
 *      dal_rtl9601b_epon_mpcpTimeoutVal_set
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
extern int32
dal_rtl9601b_epon_mpcpTimeoutVal_set(uint32 timeVal);




/* Function Name:
 *      dal_rtl9601b_epon_fecState_get
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
extern int32 dal_rtl9601b_epon_fecState_get(rtk_enable_t *pState);


/* Function Name:
 *      dal_rtl9601b_epon_fecState_set
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
extern int32 dal_rtl9601b_epon_fecState_set(rtk_enable_t state);

/* Function Name:
 *      dal_rtl9601b_epon_llidEntryNum_get
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
extern int32 dal_rtl9601b_epon_llidEntryNum_get(uint32 *num);



extern int32
dal_rtl9601b_epon_churningKeyMode_set(rtk_epon_churning_mode_t mode);

extern int32
dal_rtl9601b_epon_fecOverhead_set(uint32 val);

extern int32
dal_rtl9601b_epon_reportMode_set(rtk_epon_report_mode_t mode);

extern int32
dal_rtl9601b_epon_reportMode_get(rtk_epon_report_mode_t *pMode);



/* Function Name:
 *      dal_rtl9601b_epon_thresholdReport_set
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
extern int32
dal_rtl9601b_epon_thresholdReport_set(uint8 llidIdx, rtk_epon_report_threshold_t *pThresholdRpt);

/* Function Name:
 *      dal_rtl9601b_epon_thresholdReport_get
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
extern int32
dal_rtl9601b_epon_thresholdReport_get(uint8 llidIdx, rtk_epon_report_threshold_t *pThresholdRpt);




/* Function Name:
 *      dal_rtl9601b_epon_dbgInfo_get
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
int32 dal_rtl9601b_epon_dbgInfo_get(rtk_epon_dbgCnt_t *pDbgCnt);


/* Function Name:
 *      dal_rtl9601b_epon_losState_get
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
dal_rtl9601b_epon_losState_get(rtk_enable_t *pState);


/* Function Name:
 *      dal_rtl9601b_epon_forcePRBS_set
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
extern int32 dal_rtl9601b_epon_forcePRBS_set(rtk_epon_prbs_t prbsCfg);

/* Function Name:
 *      dal_rtl9601b_epon_forcePRBS_get
 * Description:
 *      Get force PRBS status
 * Input:
 *      None
 * Output:
 *      pPrbsCfg            - PRBS config
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 * Note:
 */
extern int32 dal_rtl9601b_epon_forcePRBS_get(rtk_epon_prbs_t *pPrbsCfg);


/* Function Name:
 *      dal_rtl9601b_epon_autoPowerSavingState_get
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
extern int32 dal_rtl9601b_epon_autoPowerSavingState_get(rtk_enable_t *pEnable);


/* Function Name:
 *      dal_rtl9601b_epon_autoPowerSavingState_set
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
extern int32 dal_rtl9601b_epon_autoPowerSavingState_set(rtk_enable_t enable);




/* Function Name:
 *      dal_rtl9601b_epon_autoPowerSavingOeRecoverTime_get
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
extern int32 dal_rtl9601b_epon_autoPowerSavingOeRecoverTime_get(int *pRecoverTime);


/* Function Name:
 *      dal_rtl9601b_epon_autoPowerSavingOeRecoverTime_set
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
extern int32 dal_rtl9601b_epon_autoPowerSavingOeRecoverTime_set(int recoverTime);



int32
dal_9601b_epon_opticalPolarity_get(rtk_epon_polarity_t *pPolarity);

int32
dal_9601b_epon_opticalPolarity_set(rtk_epon_polarity_t polarity);


extern int32 rtl9601b_raw_epon_forceLaserOn_set(uint32 force);
extern int32 rtl9601b_raw_epon_forceLaserOn_get(uint32 *pForce);


#endif /*#ifndef _DAL_RTL9601B_EPON_H_*/

