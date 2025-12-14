#ifndef _APOLLOMP_RAW_PONMAC_H_
#define _APOLLOMP_RAW_PONMAC_H_

/****************************************************************/
/* Header File inclusion                                        */
/****************************************************************/
#include <dal/apollomp/raw/apollomp_raw.h>
#include <dal/apollomp/raw/apollomp_raw_qos.h>

/****************************************************************/
/* Constant Definition                                          */
/****************************************************************/

typedef enum apollomp_raw_ponmac_draintOutState_e
{
	APOLLOMP_PONMAC_DRAINOUT_FIN = 0,
	APOLLOMP_PONMAC_DRAINOUT_DOING,
	APOLLOMP_PONMAC_DRAINOUT_END
}apollomp_raw_ponmac_draintOutState_t;

/****************************************************************/
/* Type Definition                                              */
/****************************************************************/

extern int32 apollomp_raw_ponMacQueueDrainOutState_set(uint32 qid);
extern int32 apollomp_raw_ponMacQueueDrainOutState_get(apollomp_raw_ponmac_draintOutState_t *pState);
extern int32 apollomp_raw_ponMacTcontDrainOutState_set(uint32 tcont);
extern int32 apollomp_raw_ponMacTcontDrainOutState_get(apollomp_raw_ponmac_draintOutState_t *pState);
extern int32 apollomp_raw_ponMacSidToQueueMap_set(uint32 sid, uint32 qid);
extern int32 apollomp_raw_ponMacSidToQueueMap_get(uint32 sid, uint32 *pQid);
extern int32 apollomp_raw_ponMacCirRate_set(uint32 qid, uint32 rate);
extern int32 apollomp_raw_ponMacCirRate_get(uint32 qid, uint32 *pRate);
extern int32 apollomp_raw_ponMacPirRate_set(uint32 qid, uint32 rate);
extern int32 apollomp_raw_ponMacPirRate_get(uint32 qid, uint32 *pRate);
extern int32 apollomp_raw_ponMacScheQmap_set(uint32 tcont, uint32 map);
extern int32 apollomp_raw_ponMacScheQmap_get(uint32 tcont, uint32 *pMap);
extern int32 apollomp_raw_ponMac_queueType_set(uint32 qid, apollomp_raw_queue_type_t type);
extern int32 apollomp_raw_ponMac_queueType_get(uint32 qid, apollomp_raw_queue_type_t *pType);
extern int32 apollomp_raw_ponMac_wfqWeight_set(uint32 qid, uint32 weight);
extern int32 apollomp_raw_ponMac_wfqWeight_get(uint32 qid, uint32 *pWeight);
extern int32 apollomp_raw_ponMac_egrDropState_set(uint32 qid, rtk_enable_t state);
extern int32 apollomp_raw_ponMac_egrDropState_get(uint32 qid, rtk_enable_t *pState);
extern int32 apollomp_raw_ponMacTcontEnable_set(uint32 tcont, rtk_enable_t enable);
extern int32 apollomp_raw_ponMacTcontEnable_get(uint32 tcont, rtk_enable_t *pEnable);
#endif /*#ifndef _APOLLOMP_RAW_PONMAC_H_*/

