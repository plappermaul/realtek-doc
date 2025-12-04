#ifndef _APOLLOMP_ASICDRV_VLAN_H_
#define _APOLLOMP_ASICDRV_VLAN_H_

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

typedef struct  apollomp_raw_vlanconfig_s
{
    uint32 	index;
    uint32 	evid;
    rtk_portmask_t 	mbr;
    rtk_portmask_t 	exMbr;
    uint32  fid_msti;
    uint32  envlanpol;
    uint32  meteridx;
    uint32  vbpen;
    uint32  vbpri;
}apollomp_raw_vlanconfig_t;



typedef enum apollomp_raw_vlan_l2HashType_e
{
	APOLLOMP_VLAN_HASH_SVL = 0,
    APOLLOMP_VLAN_HASH_IVL,
    APOLLOMP_VLAN_HASH_END
}apollomp_raw_vlan_l2HashType_t;



typedef struct  apollomp_raw_vlan4kentry_s{

    uint32 	vid;
    rtk_portmask_t mbr;
    rtk_portmask_t untag;
    rtk_portmask_t exMbr;
    uint32  fid_msti;
    uint32  envlanpol;
    uint32  meteridx;
    uint32  vbpen;
    uint32  vbpri;
    apollomp_raw_vlan_l2HashType_t ivl_svl;
}apollomp_raw_vlan4kentry_t;



typedef enum apollomp_raw_protoVlanFrameType_e
{
    APOLLOMP_PPVLAN_FRAME_TYPE_ETHERNET = 0,
    APOLLOMP_PPVLAN_FRAME_TYPE_LLC,
    APOLLOMP_PPVLAN_FRAME_TYPE_RFC1042,
    APOLLOMP_PPVLAN_FRAME_TYPE_END
} apollomp_raw_protoVlanFrameType_t;



typedef enum apollomp_raw_vlan_cfiKeepMode_e
{
    APOLLOMP_CFI_ALWAYS_0 = 0,
    APOLLOMP_CFI_KEEP_INGRESS,
    APOLLOMP_CFI_KEEP_TYPE_END
} apollomp_raw_vlan_cfiKeepMode_t;


typedef enum apollomp_raw_vlanAcpFrameType_e
{
    APOLLOMP_VLAN_ACCEPT_ALL = 0,
    APOLLOMP_VLAN_ACCEPT_TAGGED_ONLY,
    APOLLOMP_VLAN_ACCEPT_UNTAGGED_ONLY,
    APOLLOMP_VLAN_ACCEPT_1Q_1P_TAGGED_ONLY,
    APOLLOMP_VLAN_ACCEPT_TYPE_END
} apollomp_raw_vlanAcpFrameType_t;


typedef enum apollomp_raw_vlanTag_e
{
    APOLLOMP_VLAN_UNTAG = 0,
    APOLLOMP_VLAN_TAG,
    APOLLOMP_VLAN_TAG_TYPE_END
} apollomp_raw_vlanTag_t;




typedef enum apollomp_raw_vlan_egrTagMode_e
{
    APOLLOMP_VLAN_EGR_TAG_MODE_ORI = 0,
    APOLLOMP_VLAN_EGR_TAG_MODE_KEEP,
    APOLLOMP_VLAN_EGR_TAG_MODE_PRI_TAG,
    APOLLOMP_VLAN_EGR_TAG_MODE_END
} apollomp_raw_vlan_egrTagMode_t;



typedef struct apollomp_raw_protoValnEntryCfg_s
{
    uint32                           index;
    apollomp_raw_protoVlanFrameType_t  frameType;
    uint32                           etherType;
} apollomp_raw_protoValnEntryCfg_t;

typedef struct apollomp_raw_protoVlanCfg_s
{
    uint32 index;
    uint32 valid;
    uint32 vlan_idx;
    uint32 priority;
} apollomp_raw_protoVlanCfg_t;


extern int32 apollomp_raw_vlan_memberConfig_set(apollomp_raw_vlanconfig_t *pVlanCg);
extern int32 apollomp_raw_vlan_memberConfig_get(apollomp_raw_vlanconfig_t *pVlanCg);

extern int32 apollomp_raw_vlan_4kEntry_set(apollomp_raw_vlan4kentry_t *pVlan4kEntry );
extern int32 apollomp_raw_vlan_4kEntry_get(apollomp_raw_vlan4kentry_t *pVlan4kEntry );

extern int32 apollomp_raw_vlan_protoAndPortBasedEntry_set(rtk_port_t port, apollomp_raw_protoVlanCfg_t *pPpbCfg);
extern int32 apollomp_raw_vlan_portAndProtoBasedEntry_get(rtk_port_t port, apollomp_raw_protoVlanCfg_t *pPpbCfg);

extern int32 apollomp_raw_vlanProtoAndPortBasedEntry_get(apollomp_raw_protoValnEntryCfg_t *entry);
extern int32 apollomp_raw_vlanProtoAndPortBasedEntry_set(apollomp_raw_protoValnEntryCfg_t *entry);


extern int32 apollomp_raw_vlan_portBasedVID_set(rtk_port_t port, uint32 index);
extern int32 apollomp_raw_vlan_portBasedVID_get(rtk_port_t port, uint32 *pIndex);
extern int32 apollomp_raw_vlan_cfiKeepMode_get(apollomp_raw_vlan_cfiKeepMode_t *mode);
extern int32 apollomp_raw_vlan_cfiKeepMode_set(apollomp_raw_vlan_cfiKeepMode_t mode);
extern int32 apollomp_raw_vlan_acceptFrameType_get(rtk_port_t port, apollomp_raw_vlanAcpFrameType_t *type);
extern int32 apollomp_raw_vlan_acceptFrameType_set(rtk_port_t port, apollomp_raw_vlanAcpFrameType_t type);
extern int32 apollomp_raw_vlan_igrFlterEnable_set(rtk_port_t port, rtk_enable_t enable);
extern int32 apollomp_raw_vlan_igrFlterEnable_get(rtk_port_t port, rtk_enable_t *pEnable);
extern int32 apollomp_raw_vlan_egrTagMode_set(rtk_port_t port, apollomp_raw_vlan_egrTagMode_t mode);
extern int32 apollomp_raw_vlan_egrTagMode_get(rtk_port_t port, apollomp_raw_vlan_egrTagMode_t *pMode);


extern int32 apollomp_raw_vlan_vid0TagType_set(apollomp_raw_vlanTag_t type);
extern int32 apollomp_raw_vlan_vid0TagType_get(apollomp_raw_vlanTag_t *type);
extern int32 apollomp_raw_vlan_vid4095TagType_set(apollomp_raw_vlanTag_t type);
extern int32 apollomp_raw_vlan_vid4095TagType_get(apollomp_raw_vlanTag_t *type);

extern int32 apollomp_raw_vlan_transparentEnable_set(rtk_enable_t enable);
extern int32 apollomp_raw_vlan_transparentEnable_get(rtk_enable_t *pEnable);
extern int32 apollomp_raw_vlan_egrIgrFilterEnable_set(rtk_enable_t enable);
extern int32 apollomp_raw_vlan_egrIgrFilterEnable_get(rtk_enable_t *pEnable);


extern int32 apollomp_raw_vlan_portBasedFidEnable_set(rtk_port_t port, rtk_enable_t enable);
extern int32 apollomp_raw_vlan_portBasedFidEnable_get(rtk_port_t port, rtk_enable_t *pEnable);
extern int32 apollomp_raw_vlan_portBasedFid_get(rtk_port_t port, uint32 *fid);
extern int32 apollomp_raw_vlan_portBasedFid_set(rtk_port_t port, uint32 fid);

extern int32 apollomp_raw_vlan_portToMbrCfgIdx_set(rtk_port_t port, uint32 mbrCfgIdx);
extern int32 apollomp_raw_vlan_portToMbrCfgIdx_get(rtk_port_t port, uint32 *pMbrCfgIdx);
extern int32 apollomp_raw_vlan_vcPortToMbrCfgIdx_set(rtk_port_t port, uint32 mbrCfgIdx);
extern int32 apollomp_raw_vlan_vcPortToMbrCfgIdx_get(rtk_port_t port, uint32 *pMbrCfgIdx);
extern int32 apollomp_raw_vlan_extPortToMbrCfgIdx_set(rtk_port_t port, uint32 mbrCfgIdx);
extern int32 apollomp_raw_vlan_extPortToMbrCfgIdx_get(rtk_port_t port, uint32 *pMbrCfgIdx);

extern int32 apollomp_raw_vlan_egrKeepPmsk_get(rtk_port_t port, rtk_portmask_t *pPortMsk);
extern int32 apollomp_raw_vlan_egrKeepPmsk_set(rtk_port_t port, rtk_portmask_t portMsk);

extern int32 apollomp_raw_vlan_ipmcastLeaky_set(rtk_port_t port, rtk_enable_t enable);
extern int32 apollomp_raw_vlan_ipmcastLeaky_get(rtk_port_t port, rtk_enable_t *pEnable);



#endif /*#ifndef _APOLLOMP_ASICDRV_VLAN_H_*/

