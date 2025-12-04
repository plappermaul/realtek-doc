#ifndef _RTL9607C_RAW_FLOWCTRL_H_
#define _RTL9607C_RAW_FLOWCTRL_H_

/****************************************************************/
/* Header File inclusion                                        */
/****************************************************************/
#include <common/rt_type.h>
#include <rtk/port.h>
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <dal/rtl9607c/dal_rtl9607c.h>


/****************************************************************/
/* Constant Definition                                          */
/****************************************************************/
#define RTL9607C_PON_QUEUE_INDEX_MAX 	7
#define RTL9607C_FLOWCTRL_PREFET_THRESHOLD_MAX 	0xFF
#define RTL9607C_QUEUE_MASK                0xFF

/****************************************************************/
/* Type Definition                                              */
/****************************************************************/

extern int32 rtl9607c_raw_flowctrl_dropAllThreshold_set(uint32 threshold);
extern int32 rtl9607c_raw_flowctrl_dropAllThreshold_get(uint32 *pThreshold);
extern int32 rtl9607c_raw_flowctrl_pauseAllThreshold_set(uint32 threshold);
extern int32 rtl9607c_raw_flowctrl_pauseAllThreshold_get(uint32 *pThreshold);
extern int32 rtl9607c_raw_flowctrl_globalFcoffHighThreshold_set(uint32 onThreshold, uint32 offThreshold);
extern int32 rtl9607c_raw_flowctrl_globalFcoffHighThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold);
extern int32 rtl9607c_raw_flowctrl_globalFcoffLowThreshold_set(uint32 onThreshold, uint32 offThreshold);
extern int32 rtl9607c_raw_flowctrl_globalFcoffLowThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold);
extern int32 rtl9607c_raw_flowctrl_globalFconHighThreshold_set(uint32 onThreshold, uint32 offThreshold);
extern int32 rtl9607c_raw_flowctrl_globalFconHighThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold);
extern int32 rtl9607c_raw_flowctrl_globalFconLowThreshold_set(uint32 onThreshold, uint32 offThreshold);
extern int32 rtl9607c_raw_flowctrl_globalFconLowThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold);
extern int32 rtl9607c_raw_flowctrl_portFconHighThreshold_set(uint32 onThreshold, uint32 offThreshold);
extern int32 rtl9607c_raw_flowctrl_portFconHighThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold);
extern int32 rtl9607c_raw_flowctrl_portFconLowThreshold_set(uint32 onThreshold, uint32 offThreshold);
extern int32 rtl9607c_raw_flowctrl_portFconLowThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold);
extern int32 rtl9607c_raw_flowctrl_portFcoffHighThreshold_set(uint32 onThreshold, uint32 offThreshold);
extern int32 rtl9607c_raw_flowctrl_portFcoffHighThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold);
extern int32 rtl9607c_raw_flowctrl_portFcoffLowThreshold_set(uint32 onThreshold, uint32 offThreshold);
extern int32 rtl9607c_raw_flowctrl_portFcoffLowThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold);
extern int32 rtl9607c_raw_flowctrl_queueEegressDropThreshold_set(uint32 queue, uint32 threshold);
extern int32 rtl9607c_raw_flowctrl_queueEegressDropThreshold_get(uint32 queue, uint32 *pThreshold);
extern int32 rtl9607c_raw_flowctrl_portEegressDropThreshold_set(uint32 port, uint32 threshold);
extern int32 rtl9607c_raw_flowctrl_portEegressDropThreshold_get(uint32 port, uint32 *pThreshold);
extern int32 rtl9607c_raw_flowctrl_egressDropEnable_set(uint32 port, uint32 queue, rtk_enable_t enable);
extern int32 rtl9607c_raw_flowctrl_egressDropEnable_get(uint32 port, uint32 queue, rtk_enable_t *pEnable);
extern int32 rtl9607c_raw_flowctrl_ponEgressDropEnable_set(uint32 port, uint32 queue, rtk_enable_t enable);
extern int32 rtl9607c_raw_flowctrl_ponEgressDropEnable_get(uint32 port, uint32 queue, rtk_enable_t *pEnable);
extern int32 rtl9607c_raw_flowctrl_debugCtrl_set(uint32 port, uint32 portClear, rtk_bmp_t queueMaskClear);
extern int32 rtl9607c_raw_flowctrl_clrMaxUsedPageCnt_set(uint32 clear);
extern int32 rtl9607c_raw_flowctrl_clrTotalPktCnt_set(rtk_enable_t clear);
extern int32 rtl9607c_raw_flowctrl_totalPageCnt_get(uint32 *pCount);
extern int32 rtl9607c_raw_flowctrl_totalUsedPageCnt_get(uint32 *pCurrent, uint32 *pMaximum);
extern int32 rtl9607c_raw_flowctrl_publicUsedPageCnt_get(uint32 *pCurrent, uint32 *pMaximum);
extern int32 rtl9607c_raw_flowctrl_portUsedPageCnt_get(uint32 port, uint32 *pCurrent, uint32 *pMaximum);
extern int32 rtl9607c_raw_flowctrl_portEgressUsedPageCnt_get(uint32 port, uint32 *pCurrent, uint32 *pMaximum);
extern int32 rtl9607c_raw_flowctrl_publicFcoffUsedPageCnt_get(uint32 *pCurrent, uint32 *pMaximum);
extern int32 rtl9607c_raw_flowctrl_queueEgressUsedPageCnt_get(uint32 port, uint32 queue, uint32 *pCurrent, uint32 *pMaximum);
extern int32 rtl9607c_raw_flowctrl_ponGlobalHighThreshold_set(uint32 onThreshold, uint32 offThreshold);
extern int32 rtl9607c_raw_flowctrl_ponGlobalHighThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold);
extern int32 rtl9607c_raw_flowctrl_ponGlobalLowThreshold_set(uint32 onThreshold, uint32 offThreshold);
extern int32 rtl9607c_raw_flowctrl_ponGlobalLowThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold);
extern int32 rtl9607c_raw_flowctrl_ponPortHighThreshold_set(uint32 onThreshold, uint32 offThreshold);
extern int32 rtl9607c_raw_flowctrl_ponPortHighThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold);
extern int32 rtl9607c_raw_flowctrl_ponPortLowThreshold_set(uint32 onThreshold, uint32 offThreshold);
extern int32 rtl9607c_raw_flowctrl_ponPortLowThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold);
extern int32 rtl9607c_raw_flowctrl_ponGlobalDropHighThreshold_set(uint32 onThreshold, uint32 offThreshold);
extern int32 rtl9607c_raw_flowctrl_ponGlobalDropHighThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold);
extern int32 rtl9607c_raw_flowctrl_ponGlobalDropLowThreshold_set(uint32 onThreshold, uint32 offThreshold);
extern int32 rtl9607c_raw_flowctrl_ponGlobalDropLowThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold);
extern int32 rtl9607c_raw_flowctrl_ponPortDropHighThreshold_set(uint32 onThreshold, uint32 offThreshold);
extern int32 rtl9607c_raw_flowctrl_ponPortDropHighThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold);
extern int32 rtl9607c_raw_flowctrl_ponPortDropLowThreshold_set(uint32 onThreshold, uint32 offThreshold);
extern int32 rtl9607c_raw_flowctrl_ponPortDropLowThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold);
extern int32 rtl9607c_raw_flowctrl_swpboGlobalHighThreshold_set(uint32 onThreshold, uint32 offThreshold);
extern int32 rtl9607c_raw_flowctrl_swpboGlobalHighThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold);
extern int32 rtl9607c_raw_flowctrl_swpboGlobalLowThreshold_set(uint32 onThreshold, uint32 offThreshold);
extern int32 rtl9607c_raw_flowctrl_swpboGlobalLowThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold);
extern int32 rtl9607c_raw_flowctrl_swpboPortHighThreshold_set(uint32 onThreshold, uint32 offThreshold);
extern int32 rtl9607c_raw_flowctrl_swpboPortHighThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold);
extern int32 rtl9607c_raw_flowctrl_swpboPortLowThreshold_set(uint32 onThreshold, uint32 offThreshold);
extern int32 rtl9607c_raw_flowctrl_swpboPortLowThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold);
extern int32 rtl9607c_raw_flowctrl_swpboEgressDropThreshold_set(uint32 threshold);
extern int32 rtl9607c_raw_flowctrl_swpboEgressDropThreshold_get(uint32 *pThreshold);
extern int32 rtl9607c_raw_flowctrl_swpboPortUsedUsedPage_get(uint32 *pCurrent, uint32 *pMaximum);
extern int32 rtl9607c_raw_flowctrl_swpboEgressUsedPage_get(uint32 *pCurrent, uint32 *pMaximum);
extern int32 rtl9607c_raw_flowctrl_swpboEgressQueueUsedPage_get(uint32 *pCurrent, uint32 *pMaximum);
extern int32 rtl9607c_raw_flowctrl_ponUsedPageCtrl_set(uint32 sid, uint32 clear);
extern int32 rtl9607c_raw_flowctrl_ponUsedPage_get(uint32 sid, uint32 *pCurrent, uint32 *pMaximum);
extern int32 rtl9607c_raw_flowctrl_txPrefet_set(uint32 threshold);
extern int32 rtl9607c_raw_flowctrl_txPrefet_get(uint32 *pThreshold);
extern int32 rtl9607c_raw_flowctrl_lowQueueThreshold_set(uint32 threshold);
extern int32 rtl9607c_raw_flowctrl_lowQueueThreshold_get(uint32 *pThreshold);
extern int32 rtl9607c_raw_flowctrl_highQueueMask_set(rtk_port_t port, rtk_bmp_t mask);
extern int32 rtl9607c_raw_flowctrl_highQueueMask_get(rtk_port_t port, rtk_bmp_t  *pMask);
extern int32 rtl9607c_raw_flowctrl_patch(rtk_flowctrl_patch_t patch_type);
extern int32 rtl9607c_raw_flowctrl_ponLatchMaxUsedPage_set( rtk_enable_t enable);
extern int32 rtl9607c_raw_flowctrl_ponLatchMaxUsedPage_get( rtk_enable_t *pEnable);
extern int32 rtl9607c_raw_flowctrl_ponSidState_set(uint32 sid, rtk_enable_t state);
extern int32 rtl9607c_raw_flowctrl_ponSidState_get(uint32 sid, rtk_enable_t *pState);
extern int32 rtl9607c_raw_flowctrl_patch(rtk_flowctrl_patch_t patch_type);
extern int32 rtl9607c_raw_flowctrl_portBackPressureEnable_set(uint32 port, rtk_enable_t enable);
extern int32 rtl9607c_raw_flowctrl_portBackPressureEnable_get(uint32 port, rtk_enable_t *pEnable);
extern int32 rtl9607c_raw_flowctrl_queueEegressGapThreshold_set(uint32 threshold);
extern int32 rtl9607c_raw_flowctrl_queueEegressGapThreshold_get(uint32 *pThreshold);
extern int32 rtl9607c_raw_flowctrl_rsvPageBeforeFcDrop_set(uint32 threshold);
extern int32 rtl9607c_raw_flowctrl_rsvPageBeforeFcDrop_get(uint32 *pThreshold);
extern int32 rtl9607c_raw_flowctrl_changeDuplexEnable_set(rtk_enable_t enable);
extern int32 rtl9607c_raw_flowctrl_changeDuplexEnable_get(rtk_enable_t *pEnable);
extern int32 rtl9607c_raw_flowctrl_portEegressGapThreshold_set(uint32 threshold);
extern int32 rtl9607c_raw_flowctrl_portEegressGapThreshold_get(uint32 *pThreshold);
extern int32 rtl9607c_raw_flowctrl_portFcDropCtrlEnable_set(uint32 port, rtk_enable_t enable);
extern int32 rtl9607c_raw_flowctrl_portFcDropCtrlEnable_get(uint32 port, rtk_enable_t *pEnable);
#endif /*_RTL9607C_RAW_FLOWCTRL_H_*/

