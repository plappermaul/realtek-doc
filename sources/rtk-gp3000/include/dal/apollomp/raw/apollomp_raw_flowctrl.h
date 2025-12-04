#ifndef _APOLLOMP_RAW_FLOWCTRL_H_
#define _APOLLOMP_RAW_FLOWCTRL_H_

/****************************************************************/
/* Header File inclusion                                        */
/****************************************************************/
#include <dal/apollomp/raw/apollomp_raw.h>

/****************************************************************/
/* Constant Definition                                          */
/****************************************************************/
#define APOLLOMP_PON_QUEUE_INDEX_MAX 	7
#define APOLLOMP_FLOWCTRL_PREFET_THRESHOLD_MAX 	0xFF
#define APOLLOMP_QUEUE_MASK                0xFF

/****************************************************************/
/* Type Definition                                              */
/****************************************************************/
typedef enum apollomp_raw_flowctrl_type_e
{
    APOLLOMP_FLOWCTRL_TYPE_EGRESS  = 0,
    APOLLOMP_FLOWCTRL_TYPE_INGRESS,
    APOLLOMP_FLOWCTRL_TYPE_END    

} apollomp_raw_flowctrl_type_t;

typedef enum apollomp_raw_flowctrl_jumbo_size_e
{
    APOLLOMP_FLOWCTRL_JUMBO_3K  = 0,
    APOLLOMP_FLOWCTRL_JUMBO_4K ,
    APOLLOMP_FLOWCTRL_JUMBO_6K ,    
    APOLLOMP_FLOWCTRL_JUMBO_MAX ,        
    APOLLOMP_FLOWCTRL_JUMBO_END    

} apollomp_raw_flowctrl_jumbo_size_t;





extern int32 apollomp_raw_flowctrl_type_set(apollomp_raw_flowctrl_type_t type);
extern int32 apollomp_raw_flowctrl_type_get(apollomp_raw_flowctrl_type_t* pType);
extern int32 apollomp_raw_flowctrl_jumboMode_set(rtk_enable_t enable);
extern int32 apollomp_raw_flowctrl_jumboMode_get(rtk_enable_t *pEnable);
extern int32 apollomp_raw_flowctrl_jumboSize_set(apollomp_raw_flowctrl_jumbo_size_t size);
extern int32 apollomp_raw_flowctrl_jumboSize_get(apollomp_raw_flowctrl_jumbo_size_t *pSize);
extern int32 apollomp_raw_flowctrl_dropAllThreshold_set(uint32 threshold);
extern int32 apollomp_raw_flowctrl_dropAllThreshold_get(uint32 *pThreshold);
extern int32 apollomp_raw_flowctrl_pauseAllThreshold_set(uint32 threshold);
extern int32 apollomp_raw_flowctrl_pauseAllThreshold_get(uint32 *pThreshold);
extern int32 apollomp_raw_flowctrl_globalFcoffHighThreshold_set(uint32 onThreshold, uint32 offThreshold);
extern int32 apollomp_raw_flowctrl_globalFcoffHighThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold);
extern int32 apollomp_raw_flowctrl_globalFcoffLowThreshold_set(uint32 onThreshold, uint32 offThreshold);
extern int32 apollomp_raw_flowctrl_globalFcoffLowThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold);
extern int32 apollomp_raw_flowctrl_globalFconHighThreshold_set(uint32 onThreshold, uint32 offThreshold);
extern int32 apollomp_raw_flowctrl_globalFconHighThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold);
extern int32 apollomp_raw_flowctrl_globalFconLowThreshold_set(uint32 onThreshold, uint32 offThreshold);
extern int32 apollomp_raw_flowctrl_globalFconLowThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold);
extern int32 apollomp_raw_flowctrl_portFconHighThreshold_set(uint32 onThreshold, uint32 offThreshold);
extern int32 apollomp_raw_flowctrl_portFconHighThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold);
extern int32 apollomp_raw_flowctrl_portFconLowThreshold_set(uint32 onThreshold, uint32 offThreshold);
extern int32 apollomp_raw_flowctrl_portFconLowThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold);
extern int32 apollomp_raw_flowctrl_portFcoffHighThreshold_set(uint32 onThreshold, uint32 offThreshold);
extern int32 apollomp_raw_flowctrl_portFcoffHighThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold);
extern int32 apollomp_raw_flowctrl_portFcoffLowThreshold_set(uint32 onThreshold, uint32 offThreshold);
extern int32 apollomp_raw_flowctrl_portFcoffLowThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold);
extern int32 apollomp_raw_flowctrl_jumboGlobalHighThreshold_set(uint32 onThreshold, uint32 offThreshold);
extern int32 apollomp_raw_flowctrl_jumboGlobalHighThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold);
extern int32 apollomp_raw_flowctrl_jumboGlobalLowThreshold_set(uint32 onThreshold, uint32 offThreshold);
extern int32 apollomp_raw_flowctrl_jumboGlobalLowThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold);
extern int32 apollomp_raw_flowctrl_jumboPortHighThreshold_set(uint32 onThreshold, uint32 offThreshold);
extern int32 apollomp_raw_flowctrl_jumboPortHighThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold);
extern int32 apollomp_raw_flowctrl_jumboPortLowThreshold_set(uint32 onThreshold, uint32 offThreshold);
extern int32 apollomp_raw_flowctrl_jumboPortLowThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold);
extern int32 apollomp_raw_flowctrl_queueEegressDropThreshold_set(uint32 queue, uint32 threshold);
extern int32 apollomp_raw_flowctrl_queueEegressDropThreshold_get(uint32 queue, uint32 *pThreshold);
extern int32 apollomp_raw_flowctrl_portEegressDropThreshold_set(uint32 port, uint32 threshold);
extern int32 apollomp_raw_flowctrl_portEegressDropThreshold_get(uint32 port, uint32 *pThreshold);
extern int32 apollomp_raw_flowctrl_queueEegressGapThreshold_set(uint32 threshold);
extern int32 apollomp_raw_flowctrl_queueEegressGapThreshold_get(uint32 *pThreshold);
extern int32 apollomp_raw_flowctrl_portEegressGapThreshold_set(uint32 threshold);
extern int32 apollomp_raw_flowctrl_portEegressGapThreshold_get(uint32 *pThreshold);
extern int32 apollomp_raw_flowctrl_egressDropEnable_set(uint32 port, uint32 queue, rtk_enable_t enable);
extern int32 apollomp_raw_flowctrl_egressDropEnable_get(uint32 port, uint32 queue, rtk_enable_t *pEnable);
extern int32 apollomp_raw_flowctrl_debugCtrl_set(uint32 port, uint32 portClear, rtk_bmp_t queueMaskClear);
extern int32 apollomp_raw_flowctrl_clrMaxUsedPageCnt_set(uint32 clear);
extern int32 apollomp_raw_flowctrl_clrTotalPktCnt_set(rtk_enable_t clear);
extern int32 apollomp_raw_flowctrl_totalPageCnt_get(uint32 *pCount);
extern int32 apollomp_raw_flowctrl_totalUsedPageCnt_get(uint32 *pCurrent, uint32 *pMaximum);
extern int32 apollomp_raw_flowctrl_publicUsedPageCnt_get(uint32 *pCurrent, uint32 *pMaximum);
extern int32 apollomp_raw_flowctrl_portUsedPageCnt_get(uint32 port, uint32 *pCurrent, uint32 *pMaximum);
extern int32 apollomp_raw_flowctrl_portEgressUsedPageCnt_get(uint32 port, uint32 *pCurrent, uint32 *pMaximum);
extern int32 apollomp_raw_flowctrl_publicFcoffUsedPageCnt_get(uint32 *pCurrent, uint32 *pMaximum);
extern int32 apollomp_raw_flowctrl_publicJumboUsedPageCnt_get(uint32 *pCurrent, uint32 *pMaximum);
extern int32 apollomp_raw_flowctrl_queueEgressUsedPageCnt_get(uint32 port, uint32 queue, uint32 *pCurrent, uint32 *pMaximum);
extern int32 apollomp_raw_flowctrl_ponGlobalHighThreshold_set(uint32 onThreshold, uint32 offThreshold);
extern int32 apollomp_raw_flowctrl_ponGlobalHighThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold);
extern int32 apollomp_raw_flowctrl_ponGlobalLowThreshold_set(uint32 onThreshold, uint32 offThreshold);
extern int32 apollomp_raw_flowctrl_ponGlobalLowThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold);
extern int32 apollomp_raw_flowctrl_ponPortHighThreshold_set(uint32 onThreshold, uint32 offThreshold);
extern int32 apollomp_raw_flowctrl_ponPortHighThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold);
extern int32 apollomp_raw_flowctrl_ponPortLowThreshold_set(uint32 onThreshold, uint32 offThreshold);
extern int32 apollomp_raw_flowctrl_ponPortLowThreshold_get(uint32 *pOnThreshold, uint32 *pOffThreshold);
extern int32 apollomp_raw_flowctrl_ponQueueEegressDropIndex_set(uint32 queue, uint32 index);
extern int32 apollomp_raw_flowctrl_ponQueueEegressDropIndex_get(uint32 queue, uint32 *pIndex);
extern int32 apollomp_raw_flowctrl_ponQueueEegressDropThreshold_set(uint32 index, uint32 threshold);
extern int32 apollomp_raw_flowctrl_ponQueueEegressDropThreshold_get(uint32 index, uint32 *pThreshold);
extern int32 apollomp_raw_flowctrl_ponQueueEegressGapThreshold_set(uint32 threshold);
extern int32 apollomp_raw_flowctrl_ponQueueEegressGapThreshold_get(uint32 *pThreshold);
extern int32 apollomp_raw_flowctrl_portPacketPageCnt_get(uint32 port, uint32 *pCount);
extern int32 apollomp_raw_flowctrl_ponUsedPageCtrl_set(uint32 clear);
extern int32 apollomp_raw_flowctrl_ponUsedPage_get(uint32 queue, uint32 *pCurrent, uint32 *pMaximum);
extern int32 apollomp_raw_flowctrl_txPrefet_set(uint32 threshold);
extern int32 apollomp_raw_flowctrl_txPrefet_get(uint32 *pThreshold);
extern int32 apollomp_raw_flowctrl_lowQueueThreshold_set(uint32 threshold);
extern int32 apollomp_raw_flowctrl_lowQueueThreshold_get(uint32 *pThreshold);
extern int32 apollomp_raw_flowctrl_highQueueMask_set(rtk_port_t port, rtk_bmp_t mask);
extern int32 apollomp_raw_flowctrl_highQueueMask_get(rtk_port_t port, rtk_bmp_t  *pMask);
extern int32 apollomp_raw_flowctrl_ponThreshold_set(void);
extern int32 apollomp_raw_flowctrl_fiberThreshold_set(void);
extern int32 apollomp_raw_flowctrl_defaultThreshold_set(void);
extern int32 apollomp_raw_flowctrl_patch(rtk_flowctrl_patch_t patch_type);
extern int32 apollomp_raw_flowctrl_ponLatchMaxUsedPage_set( rtk_enable_t enable);
extern int32 apollomp_raw_flowctrl_ponLatchMaxUsedPage_get( rtk_enable_t *pEnable);

#endif /*_APOLLOMP_RAW_FLOWCTRL_H_*/

