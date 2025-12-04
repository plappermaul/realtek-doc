#ifndef _APOLLOMP_RAW_TRAP_H_
#define _APOLLOMP_RAW_TRAP_H_

/****************************************************************/
/* Header File inclusion                                        */
/****************************************************************/
#include <dal/apollomp/raw/apollomp_raw.h>
/****************************************************************/
/* Constant Definition                                          */
/****************************************************************/


/****************************************************************/
/* Type Definition                                              */
/****************************************************************/
typedef enum apollomp_raw_rmaAction_e
{
    APOLLOMP_RMA_ACT_FORWARD  = 0,
    APOLLOMP_RMA_ACT_TRAP ,
    APOLLOMP_RMA_ACT_DROP ,
    APOLLOMP_RMA_ACT_FORWARD_EXCLUDE_CPU ,
    APOLLOMP_RMA_ACT_END ,
} apollomp_raw_rmaAction_t;

typedef enum apollomp_raw_rmaFeature_e
{
    APOLLOMP_RMA_FUN_VLAN_LEAKY = 0,
    APOLLOMP_RMA_FUN_PISO_LEAKY,
    APOLLOMP_RMA_FUN_BYPASS_STORM,
    APOLLOMP_RMA_FUN_KEEP_CTGA_FMT,
    APOLLOMP_RMA_FUN_END,
} apollomp_raw_rmaFeature_t;

#if 0
typedef enum apollomp_raw_igmpmld_type_e
{
    APOLLOMP_RAW_TYPE_IGMPV1 = 0,
    APOLLOMP_RAW_TYPE_IGMPV2,
    APOLLOMP_RAW_TYPE_IGMPV3,
    APOLLOMP_RAW_TYPE_MLDV1,
    APOLLOMP_RAW_TYPE_MLDV2,
    APOLLOMP_RAW_TYPE_END,

} apollomp_raw_igmpmld_type_t;

typedef enum apollomp_raw_igmpAction_e
{
    APOLLOMP_RAW_IGMP_ACT_FORWARD  = 0,
    APOLLOMP_RAW_IGMP_ACT_DROP,
    APOLLOMP_RAW_IGMP_ACT_TRAP,
    APOLLOMP_RAW_IGMP_ACT_END,
} apollomp_raw_igmpAction_t;

typedef enum apollomp_raw_igmpChecksumErrAction_e
{
    APOLLOMP_RAW_CHECKSUM_ERR_FORWARD = 0,
    APOLLOMP_RAW_CHECKSUM_ERR_DROP,
    APOLLOMP_RAW_CHECKSUM_ERR_TRAP,
    APOLLOMP_RAW_CHECKSUM_ERR_END,
}apollomp_raw_igmpChecksumErrAction_t;

extern int32 apollomp_raw_trap_rmaPriority_set(uint32 priority);
extern int32 apollomp_raw_trap_rmaPriority_get(uint32 *pPriority);
extern int32 apollomp_raw_trap_rmaAction_set(uint32  rmaTail, rtk_action_t action);
extern int32 apollomp_raw_trap_rmaAction_get(uint32 rmaTail, rtk_action_t* pAction);
#endif
extern int32 apollomp_raw_trap_rmaFeature_set(uint32  rmaTail, apollomp_raw_rmaFeature_t type, rtk_enable_t enable);
extern int32 apollomp_raw_trap_rmaFeature_get(uint32  rmaTail, apollomp_raw_rmaFeature_t type, rtk_enable_t *pEnable);
extern int32 apollomp_raw_trap_rmaCiscoAction_set(uint32  rmaTail, rtk_action_t action);
extern int32 apollomp_raw_trap_rmaCiscoAction_get(uint32  rmaTail, rtk_action_t *pAction);
#if 0
extern int32 apollomp_raw_trap_rmaCiscoFeature_set(uint32  rmaTail, apollomp_raw_rmaFeature_t type, rtk_enable_t enable);
extern int32 apollomp_raw_trap_rmaCiscoFeature_get(uint32  rmaTail, apollomp_raw_rmaFeature_t type, rtk_enable_t *pEnable);
extern int32 apollomp_raw_trap_igmpAction_set(rtk_port_t port, apollomp_raw_igmpmld_type_t type, rtk_action_t action);
extern int32 apollomp_raw_trap_igmpAction_get(rtk_port_t port, apollomp_raw_igmpmld_type_t type, rtk_action_t *pAction);
extern int32 apollomp_raw_trap_igmpIsoLeaky_set(rtk_enable_t state);
extern int32 apollomp_raw_trap_igmpIsoLeaky_get(rtk_enable_t *pState);
#endif
extern int32 apollomp_raw_trap_igmpVLANLeaky_set(rtk_enable_t state);
extern int32 apollomp_raw_trap_igmpVLANLeaky_get(rtk_enable_t *pState);
extern int32 apollomp_raw_trap_igmpBypassStrom_set(rtk_enable_t state);
extern int32 apollomp_raw_trap_igmpBypassStrom_get(rtk_enable_t *pState);
#if 0
extern int32 apollomp_raw_trap_igmpChechsumError_set(rtk_action_t action);
extern int32 apollomp_raw_trap_igmpChechsumError_get(rtk_action_t *pAction);
#endif
#endif /*#ifndef _APOLLOMP_RAW_TRAP_H_*/

