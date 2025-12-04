#ifndef _APOLLOMP_RAW_QOS_H_
#define _APOLLOMP_RAW_QOS_H_

/*
 * Include Files
 */
#include <dal/apollomp/raw/apollomp_raw.h>



/****************************************************************/
/* Constant Definition                                          */
/****************************************************************/



/*
 * Symbol Definition
 */


typedef enum apollomp_raw_qos_priType_e
{
    APOLLOMP_QOS_PRI_TYPE_SVLAN = 0,
    APOLLOMP_QOS_PRI_TYPE_SA,
    APOLLOMP_QOS_PRI_TYPE_LUTFWD,
    APOLLOMP_QOS_PRI_TYPE_CVLAN,
    APOLLOMP_QOS_PRI_TYPE_ACL,
    APOLLOMP_QOS_PRI_TYPE_DSCP,
    APOLLOMP_QOS_PRI_TYPE_DOT1Q,
    APOLLOMP_QOS_PRI_TYPE_PORT,
    APOLLOMP_QOS_PRI_TYPE_L4,
    APOLLOMP_QOS_PRI_TYPE_END
} apollomp_raw_qos_priType_t;



typedef enum apollomp_raw_dscp_source_e
{
    APOLLOMP_QOS_DSCP_SOURCE_INTR_PRI = 0,
    APOLLOMP_QOS_DSCP_SOURCE_ORI_DSCP,
    APOLLOMP_QOS_DSCP_SOURCE_USER_PRI,
    APOLLOMP_QOS_DSCP_SOURCE_TYPE_END
} apollomp_raw_dscp_source_t;


typedef enum apollomp_raw_ifg_include_e
{
    APOLLOMP_QOS_IFG_EXCLUDE = 0,
    APOLLOMP_QOS_IFG_INCLUDE,
    APOLLOMP_QOS_IFG_TYPE_END
} apollomp_raw_ifg_include_t;




typedef enum apollomp_raw_wfq_type_e
{
    APOLLOMP_QOS_QUEUE_STRICT = 0,
    APOLLOMP_QOS_QUEUE_WFQ,
    APOLLOMP_QOS_TYPE_END
} apollomp_raw_queue_type_t;


typedef enum apollomp_raw_igrbw_mode_e
{
    APOLLOMP_IGRBW_FLOWCTRL = 0,
    APOLLOMP_IGRBW_DROP,
    APOLLOMP_IGRBW_MODE_END
} apollomp_raw_igrbw_mode_t;

/*
 * Data Declaration
 */
 


/*
 * Function Declaration
 */
extern int32 apollomp_raw_qos_priToQidMappingTable_set(uint32 index, uint32 pri, uint32 qid);
extern int32 apollomp_raw_qos_priToQidMappingTable_get(uint32 index, uint32 pri, uint32 *pQid);
extern int32 apollomp_raw_qos_portQidMapIdx_set(rtk_port_t port, uint32 index);
extern int32 apollomp_raw_qos_portQidMapIdx_get(rtk_port_t port, uint32 *pIndex);
extern int32 apollomp_raw_qos_priRemapToCpu_set(uint32 pri, uint32 remapPri);
extern int32 apollomp_raw_qos_priRemapToCpu_get(uint32 pri, uint32 *pRemapPri);
extern int32 apollomp_raw_qos_1QPriRemap_set(uint32 pri, uint32 remapPri);
extern int32 apollomp_raw_qos_1QPriRemap_get(uint32 pri, uint32 *pRemapPri);
extern int32 apollomp_raw_qos_dscpPriRemap_set(uint32 dscp, uint32 remapPri);
extern int32 apollomp_raw_qos_dscpPriRemap_get(uint32 dscp, uint32 *pRemapPri);
extern int32 apollomp_raw_qos_portBasePri_set(rtk_port_t port, uint32 priority);
extern int32 apollomp_raw_qos_portBasePri_get(rtk_port_t port, uint32 *pPri);
extern int32 apollomp_raw_qos_priDecisionWeight_set(apollomp_raw_qos_priType_t type, uint32 weight);
extern int32 apollomp_raw_qos_priDecisionWeight_get(apollomp_raw_qos_priType_t type, uint32 *pWeight);
extern int32 apollomp_raw_qos_1qRemarkAbility_set(rtk_port_t port, rtk_enable_t enable);
extern int32 apollomp_raw_qos_1qRemarkAbility_get(rtk_port_t port, rtk_enable_t *pEnable);
extern int32 apollomp_raw_qos_dscpRemarkAbility_set(rtk_port_t port, rtk_enable_t enable);
extern int32 apollomp_raw_qos_dscpRemarkAbility_get(rtk_port_t port, rtk_enable_t *pEnable);
extern int32 apollomp_raw_qos_1qRemarkPri_set(uint32 priority, uint32 newPriority);
extern int32 apollomp_raw_qos_1qRemarkPri_get(uint32 priority, uint32 *pNewPriority);
extern int32 apollomp_raw_qos_dscpRemarkDscp_set(uint32 dscp, uint32 newDscp);
extern int32 apollomp_raw_qos_dscpRemarkDscp_get(uint32 dscp, uint32 *pNewDscp);
extern int32 apollomp_raw_qos_interPriRemarkDscp_set(uint32 intPri, uint32 newDscp);
extern int32 apollomp_raw_qos_interPriRemarkDscp_get(uint32 intPri, uint32 *pNewDscp);
extern int32 apollomp_raw_qos_dscpRemarkSource_set(rtk_port_t port, apollomp_raw_dscp_source_t source);
extern int32 apollomp_raw_qos_dscpRemarkSource_get(rtk_port_t port, apollomp_raw_dscp_source_t *pSource);

extern int32 apollomp_raw_qos_wfqBurstSize_set(uint32 burstsize);
extern int32 apollomp_raw_qos_wfqBurstSize_get(uint32 *pPurstsize);
extern int32 apollomp_raw_qos_wfqCalIncIfg_set(apollomp_raw_ifg_include_t mode);
extern int32 apollomp_raw_qos_wfqCalIncIfg_get(apollomp_raw_ifg_include_t *pMode);
extern int32 apollomp_raw_qos_bwCtrlEgrRate_set(rtk_port_t port, uint32 rate);
extern int32 apollomp_raw_qos_bwCtrlEgrRate_get(rtk_port_t port, uint32 *pRate);
extern int32 apollomp_raw_qos_wfqWeight_get(rtk_port_t port, uint32 qid, uint32 *pWeight);
extern int32 apollomp_raw_qos_wfqWeight_set(rtk_port_t port, uint32 qid, uint32 weight);
extern int32 apollomp_raw_qos_aprEnable_set(rtk_port_t port, uint32 qid, rtk_enable_t enable);
extern int32 apollomp_raw_qos_aprEnable_get(rtk_port_t port, uint32 qid, rtk_enable_t *pEnable);
extern int32 apollomp_raw_qos_queueType_set(rtk_port_t port, uint32 qid, apollomp_raw_queue_type_t type);
extern int32 apollomp_raw_qos_queueType_get(rtk_port_t port, uint32 qid, apollomp_raw_queue_type_t *pType);
extern int32 apollomp_raw_qos_aprMeter_set(rtk_port_t port, uint32 qid, uint32 meterIdx);
extern int32 apollomp_raw_qos_aprMeter_get(rtk_port_t port, uint32 qid, uint32 *pMeterIdx);
extern int32 apollomp_raw_qos_queueEmpty_get(uint32 *qEmpty);
extern int32 apollomp_raw_qos_ethernetAvEnable_set(rtk_port_t port, rtk_enable_t enable);
extern int32 apollomp_raw_qos_ethernetAvEnable_get(rtk_port_t port, rtk_enable_t *pEnable);
extern int32 apollomp_raw_qos_ethernetAvRemapPri_set(uint32 priority, uint32 newPriority);
extern int32 apollomp_raw_qos_ethernetAvRemapPri_get(uint32 priority, uint32 *pNewPriority);
extern int32 apollomp_raw_qos_bwCtrlIgrRateBypass_set(rtk_enable_t enable);
extern int32 apollomp_raw_qos_bwCtrlIgrRateBypass_get(rtk_enable_t *pEnable);
extern int32 apollomp_raw_qos_bwCtrlIgrRate_set(rtk_port_t port, uint32 rate);
extern int32 apollomp_raw_qos_bwCtrlIgrRate_get(rtk_port_t port, uint32 *pRate);
extern int32 apollomp_raw_qos_bwCtrlIgrRateCalIncIfg_set(rtk_port_t port, apollomp_raw_ifg_include_t mode);
extern int32 apollomp_raw_qos_bwCtrlIgrRateCalIncIfg_get(rtk_port_t port, apollomp_raw_ifg_include_t *pMode); 
extern int32 apollomp_raw_qos_bwCtrlIgrRateFc_set(rtk_port_t port, rtk_enable_t enable);
extern int32 apollomp_raw_qos_bwCtrlIgrRateFc_get(rtk_port_t port, rtk_enable_t *pEnable);
extern int32 apollomp_raw_qos_bwCtrlIgrBound_set(uint32 highThreshold, uint32 lowThreshold);
extern int32 apollomp_raw_qos_bwCtrlIgrBound_get(uint32 *pHighThreshold, uint32 *pLowThreshold);

#endif /*#ifndef _APOLLOMP_RAW_QOS_H_*/

