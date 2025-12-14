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
#ifndef _DAL_RTL9607F_EPON_H_
#define _DAL_RTL9607F_EPON_H_

#include <common/rt_error.h>
#include <common/rt_type.h>
#include <rtk/epon.h> 


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
extern int32 dal_rtl9607f_epon_init(void);

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
extern int32 dal_rtl9607f_epon_llid_entry_get(rtk_epon_llid_entry_t *pLlidEntry);

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
extern int32 dal_rtl9607f_epon_llid_entry_set(rtk_epon_llid_entry_t *pLlidEntry);

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
extern int32 dal_rtl9607f_epon_registerReq_get(rtk_epon_regReq_t *pRegEntry);

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
extern int32 dal_rtl9607f_epon_registerReq_set(rtk_epon_regReq_t *pRegEntry);

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
extern int32 dal_rtl9607f_epon_churningKey_get(rtk_epon_churningKeyEntry_t *pEntry);

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
extern int32 dal_rtl9607f_epon_churningKey_set(rtk_epon_churningKeyEntry_t *pEntry);

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
extern int32 dal_rtl9607f_epon_usFecState_get(rtk_enable_t *pState);

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
extern int32 dal_rtl9607f_epon_usFecState_set(rtk_enable_t state);

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
extern int32 dal_rtl9607f_epon_dsFecState_get(rtk_enable_t *pState);

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
extern int32 dal_rtl9607f_epon_dsFecState_set(rtk_enable_t state);

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
extern int32 dal_rtl9607f_epon_mibCounter_get(rtk_epon_counter_t *pCounter);

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
extern int32 dal_rtl9607f_epon_losState_get(rtk_enable_t *pState);

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
extern int32 dal_rtl9607f_epon_llidEntryNum_get(uint32 *num);

#endif /*#ifndef _DAL_RTL9607F_EPON_H_*/

