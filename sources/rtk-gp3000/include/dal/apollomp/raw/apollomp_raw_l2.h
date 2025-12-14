#ifndef _APOLLOMP_RAW_L2_H_
#define _APOLLOMP_RAW_L2_H_

/****************************************************************/
/* Header File inclusion                                        */
/****************************************************************/
#include <dal/apollomp/raw/apollomp_raw.h>
/****************************************************************/
/* Constant Definition                                          */
/****************************************************************/
#define APOLLOMP_RAW_LUT_AGESPEEDMAX    0x1FFFFF
#define APOLLOMP_RAW_LUT_FWDPRIMAX      0x7
#define APOLLOMP_RAW_LUT_AGEMAX         0x7
#define APOLLOMP_RAW_LUT_EXTDSL_SPAMAX  0XF
#define APOLLOMP_RAW_LUT_L3IDXAMAX      0XFFFFFFF
#define APOLLOMP_RAW_LUT_EXTMBR         0x3F
#define APOLLOMP_RAW_LUT_GIPMAX         0XFFFFFF

#define APOLLOMP_LUT_4WAY_NO                  2048
#define APOLLOMP_LUT_CAM_NO                   64
#define APOLLOMP_LUT_TBL_NO                   (APOLLOMP_LUT_4WAY_NO + APOLLOMP_LUT_CAM_NO) /*2K 4 ways hash entries + 64 CAM entries*/
#define APOLLOMP_LUT_TBL_MAX                  (APOLLOMP_LUT_TBL_NO - 1)

#define APOLLOMP_PORTMASK                     0x7F

#define APOLLOMP_IPMC_TABLE_ENTRY             64
#define APOLLOMP_IPMC_TABLE_IDX_MAX           (APOLLOMP_IPMC_TABLE_ENTRY - 1)

/****************************************************************/
/* Type Definition                                              */
/****************************************************************/
typedef enum apollomp_raw_l2_entryType_e
{

	APOLLOMP_RAW_LUT_ENTRY_TYPE_L2UC = 0,
    APOLLOMP_RAW_LUT_ENTRY_TYPE_L2MC_DSL,
    APOLLOMP_RAW_LUT_ENTRY_TYPE_L3MC_DSL,
    APOLLOMP_RAW_LUT_ENTRY_TYPE_L3MC_ROUTE,
    APOLLOMP_RAW_LUT_ENTRY_TYPE_END,

}apollomp_raw_l2_entryType_t;

typedef enum apollomp_raw_l2_saCtlAct_e
{
	APOLLOMP_RAW_LUT_SACTL_ACT_FORWARD = 0,
	APOLLOMP_RAW_LUT_SACTL_ACT_DROP,
	APOLLOMP_RAW_LUT_SACTL_ACT_TRAP,
	APOLLOMP_RAW_LUT_SACTL_ACT_COPY,
	APOLLOMP_RAW_LUT_SACTL_ACT_END,
}apollomp_raw_l2_saCtlAct_t;


typedef enum apollomp_raw_l2_commonAct_e
{
	APOLLOMP_RAW_L2_COMMON_ACT_FORWARD = 0,
	APOLLOMP_RAW_L2_COMMON_ACT_DROP,
	APOLLOMP_RAW_L2_COMMON_ACT_TRAP,
	APOLLOMP_RAW_L2_COMMON_ACT_COPY2CPU,
	APOLLOMP_RAW_L2_COMMON_ACT_END,
}apollomp_raw_l2_commonAct_t;


typedef enum apollomp_raw_l2_unknMcAct_e
{
	APOLLOMP_RAW_L2_UNKNMC_ACT_FORWARD = 0,
	APOLLOMP_RAW_L2_UNKNMC_ACT_DROP,
	APOLLOMP_RAW_L2_UNKNMC_ACT_TRAP,
	APOLLOMP_RAW_L2_UNKNMC_ACT_EXCLUDE_RMA,
	APOLLOMP_RAW_L2_UNKNMC_ACT_END,
}apollomp_raw_l2_unknMcAct_t;

typedef enum apollomp_raw_l2_readMethod_e
{
	APOLLOMP_RAW_LUT_READ_METHOD_MAC = 0,
	APOLLOMP_RAW_LUT_READ_METHOD_ADDRESS,
	APOLLOMP_RAW_LUT_READ_METHOD_NEXT_ADDRESS,
	APOLLOMP_RAW_LUT_READ_METHOD_NEXT_L2UC,
	APOLLOMP_RAW_LUT_READ_METHOD_NEXT_L2MC,
	APOLLOMP_RAW_LUT_READ_METHOD_NEXT_L3MC,
	APOLLOMP_RAW_LUT_READ_METHOD_NEXT_L2L3MC,
	APOLLOMP_RAW_LUT_READ_METHOD_NEXT_L2UCSPA,
	APOLLOMP_RAW_LUT_READ_METHOD_END,
}apollomp_raw_l2_readMethod_t;

typedef enum apollomp_raw_l2_flushStatus_e
{
    APOLLOMP_RAW_FLUSH_STATUS_NONBUSY =0,
    APOLLOMP_RAW_FLUSH_STATUS_BUSY,
    APOLLOMP_RAW_FLUSH_STATUS_END,
}apollomp_raw_l2_flushStatus_t;

typedef enum apollomp_raw_l2_flushMode_e
{
	APOLLOMP_RAW_FLUSH_MODE_PORT = 0,
	APOLLOMP_RAW_FLUSH_MODE_VID,
	APOLLOMP_RAW_FLUSH_MODE_FID,
	APOLLOMP_RAW_FLUSH_MODE_END,
}apollomp_raw_l2_flushMode_t;

typedef enum apollomp_raw_l2_flushType_e
{
	APOLLOMP_RAW_FLUSH_TYPE_DYNAMIC = 0,
	APOLLOMP_RAW_FLUSH_TYPE_STATIC ,
	APOLLOMP_RAW_FLUSH_TYPE_BOTH,
	APOLLOMP_RAW_FLUSH_TYPE_END,
}apollomp_raw_l2_flushType_t;

typedef enum apollomp_raw_l2_ipMcLookupOp_e
{
   APOLLOMP_RAW_LUT_IPMCLOOKUP_TYPE_DIPSIP = 0,
   APOLLOMP_RAW_LUT_IPMCLOOKUP_TYPE_DIP,
   APOLLOMP_RAW_LUT_IPMCLOOKUP_TYPE_END,
}apollomp_raw_l2_ipMcLookupOp_t;

typedef enum apollomp_raw_l2_ipMcHashType_e
{
   APOLLOMP_RAW_LUT_IPMCHASH_TYPE_DMACFID=0,
   APOLLOMP_RAW_LUT_IPMCHASH_TYPE_DIPSIP,
   APOLLOMP_RAW_LUT_IPMCHASH_TYPE_GIPVID,
   APOLLOMP_RAW_LUT_IPMCHASH_TYPE_END,
}apollomp_raw_l2_ipMcHashType_t;

typedef enum apollomp_raw_l2_cfg_enable_e
{
   APOLLOMP_RAW_LUT_CFG_ENABLE=0,
   APOLLOMP_RAW_LUT_CFG_DISABLE,
   APOLLOMP_RAW_LUT_CFG_END,
}apollomp_raw_l2_cfg_enable_t;

typedef enum apollomp_raw_l2_flood_enable_e
{
   APOLLOMP_RAW_LUT_FLOOD_DROP=0,
   APOLLOMP_RAW_LUT_FLOOD_FLOODING,
   APOLLOMP_RAW_LUT_FLOO_END,
}apollomp_raw_l2_flood_enable_t;

typedef enum apollomp_raw_l2_McAllow_e
{
	APOLLOMP_RAW_L2_MC_DROP = 0,
    APOLLOMP_RAW_L2_MC_ALLOW,
    APOLLOMP_RAW_L2_MC_END,
}apollomp_raw_l2_McAllow_t;

typedef enum apollomp_raw_l2_l2HashType_e
{
	APOLLOMP_RAW_L2_HASH_SVL = 0,
    APOLLOMP_RAW_L2_HASH_IVL,
    APOLLOMP_RAW_L2_HASH_END
}apollomp_raw_l2_l2HashType_t;



typedef struct apollomp_raw_flush_ctrl_s
{
    apollomp_raw_l2_flushStatus_t flushStatus;
    apollomp_raw_l2_flushMode_t flushMode;
    apollomp_raw_l2_flushType_t flushType;
    rtk_vlan_t vid;
    rtk_fid_t fid;
}apollomp_raw_flush_ctrl_t;

typedef struct apollomp_raw_l2_cfg_s
{
    apollomp_raw_l2_ipMcLookupOp_t ipMcLookupOpType;
    apollomp_raw_l2_ipMcHashType_t ipMcHashType;
    apollomp_raw_l2_cfg_enable_t linkDownAgeout;
    apollomp_raw_l2_cfg_enable_t bcamDis;
    uint32 ageSpeed;
}apollomp_raw_l2_cfg_t;

typedef struct apollomp_raw_l2_table_s
{
    apollomp_raw_l2_readMethod_t method;
    /*s/w information*/
    apollomp_raw_l2_entryType_t table_type;
    uint32 lookup_hit;
    uint32 lookup_busy;
    uint32 address;
    uint32 wait_time;


    /*common part*/
	uint32 l3lookup;
    uint32 lut_pri;
	uint32 fwdpri_en;
    uint32 nosalearn;
    uint32 valid;

    /*----L2----*/
    rtk_mac_t mac;
	uint32 cvid_fid;
    apollomp_raw_l2_l2HashType_t ivl_svl;

    /*l2 uc*/
    uint32 fid;
    uint32 efid;
	uint32 sapri_en;
    uint32 spa;
	uint32 age;
    uint32 auth;
    uint32 sa_block;
	uint32 da_block;
	uint32 ext_dsl_spa;
    uint32 arp_used;

    /*----L3----*/
    ipaddr_t gip;
    uint32 gip_only;
	uint32 wan_sa;


    /* ---L3 MC DSL---*/
    uint32 sip_vid;

    /* ---L3 MC ROUTE---*/
    uint32 l3_idx;
    uint32 ext_fr;


    /*(L2 MC DSL)(L3 MC DSL)(L3 MC ROUTE)*/
	uint32 mbr;
    uint32 ext_mbr;

}apollomp_lut_table_t;

extern int32 apollomp_raw_l2_lookUpTb_set(apollomp_lut_table_t *pL2Table);
extern int32 apollomp_raw_l2_lookUpTb_get(apollomp_lut_table_t *pL2Table);
extern int32 apollomp_raw_l2_flushEn_set(rtk_port_t port, rtk_enable_t enabled);
extern int32 apollomp_raw_l2_flushEn_get(rtk_port_t port, rtk_enable_t *pEnabled);
extern int32 apollomp_raw_l2_flushCtrl_set(apollomp_raw_flush_ctrl_t *pCtrl);
extern int32 apollomp_raw_l2_flushCtrl_get(apollomp_raw_flush_ctrl_t *pCtrl);
extern int32 apollomp_raw_l2_unMatched_saCtl_set(rtk_port_t port, rtk_action_t action);
extern int32 apollomp_raw_l2_unMatched_saCtl_get(rtk_port_t port, rtk_action_t* pAction);
extern int32 apollomp_raw_l2_unkn_saCtl_set(rtk_port_t port, rtk_action_t action);
extern int32 apollomp_raw_l2_unkn_saCtl_get(rtk_port_t port, rtk_action_t* pAction);
extern int32 apollomp_raw_l2_unkn_ucDaCtl_set(rtk_port_t port, rtk_action_t action);
extern int32 apollomp_raw_l2_unkn_ucDaCtl_get(rtk_port_t port, rtk_action_t* pAction);
extern int32 apollomp_raw_l2_learnOverAct_set(rtk_port_t port, rtk_action_t action);
extern int32 apollomp_raw_l2_learnOverAct_get(rtk_port_t port, rtk_action_t* pAction);
extern int32 apollomp_raw_l2_agingEnable_set(rtk_port_t port, rtk_enable_t enable);
extern int32 apollomp_raw_l2_agingEnable_get(rtk_port_t port, rtk_enable_t *pEnable);
extern int32 apollomp_raw_l2_lrnLimitNo_set(rtk_port_t port, uint32 num);
extern int32 apollomp_raw_l2_lrnLimitNo_get(rtk_port_t port, uint32 *pNum);
extern int32 apollomp_raw_l2_lrnCnt_get(rtk_port_t port, uint32 *pNum);
extern int32 apollomp_raw_l2_lrnOverSts_set(rtk_port_t port, uint32 value);
extern int32 apollomp_raw_l2_lrnOverSts_get(rtk_port_t port, uint32 *pValue);
extern int32 apollomp_raw_l2_sysLrnOverAct_set(rtk_action_t action);
extern int32 apollomp_raw_l2_sysLrnOverAct_get(rtk_action_t* pAction);
extern int32 apollomp_raw_l2_sysLrnLimitNo_set(uint32 num);
extern int32 apollomp_raw_l2_sysLrnLimitNo_get(uint32* pNum);
extern int32 apollomp_raw_l2_sysLrnOverSts_set( uint32 value);
extern int32 apollomp_raw_l2_sysLrnOverSts_get( uint32 *pValue);
extern int32 apollomp_raw_l2_SysLrnCnt_get( uint32 *pNum);
extern int32 apollomp_raw_unkn_l2Mc_set(rtk_port_t port, rtk_action_t action);
extern int32 apollomp_raw_unkn_l2Mc_get(rtk_port_t port, rtk_action_t* pAction);
extern int32 apollomp_raw_unkn_ip4Mc_set(rtk_port_t port, rtk_action_t action);
extern int32 apollomp_raw_unkn_ip4Mc_get(rtk_port_t port, rtk_action_t* pAction);
extern int32 apollomp_raw_unkn_ip6Mc_set(rtk_port_t port, rtk_action_t action);
extern int32 apollomp_raw_unkn_ip6Mc_get(rtk_port_t port, rtk_action_t* pAction);
extern int32 apollomp_raw_unkn_mcPri_set(uint32 prity);
extern int32 apollomp_raw_unkn_mcPri_get(uint32* pPrity);
extern int32 apollomp_raw_l2_bcFlood_set(rtk_port_t port, rtk_enable_t enabled);
extern int32 apollomp_raw_l2_bcFlood_get(rtk_port_t port, rtk_enable_t *pEnabled);
extern int32 apollomp_raw_l2_unknUcFlood_set(rtk_port_t port, rtk_enable_t enabled);
extern int32 apollomp_raw_l2_unknUcFlood_get(rtk_port_t port, rtk_enable_t *pEnabled);
extern int32 apollomp_raw_l2_unknMcFlood_set(rtk_port_t port, rtk_enable_t enabled);
extern int32 apollomp_raw_l2_unknMcFlood_get(rtk_port_t port, rtk_enable_t *pEnabled);
extern int32 apollomp_raw_l2_efid_set(rtk_port_t port, rtk_fid_t efid);
extern int32 apollomp_raw_l2_efid_get(rtk_port_t port, rtk_fid_t* pEfid);
extern int32 apollomp_raw_l2_ipmcAction_set(rtk_port_t port, rtk_action_t action);
extern int32 apollomp_raw_l2_ipmcAction_get(rtk_port_t port, rtk_action_t *pAction);
extern int32 apollomp_raw_l2_igmp_Mc_table_set(uint32 index, rtk_ip_addr_t dip, uint32 portmask);
extern int32 apollomp_raw_l2_igmp_Mc_table_get(uint32 index, rtk_ip_addr_t *pDip, uint32 *pPortmask);

extern int32 apollomp_raw_l2_camEnable_set(rtk_enable_t enable);
extern int32 apollomp_raw_l2_camEnable_get(rtk_enable_t *pEnable);
extern int32 apollomp_raw_l2_agingTime_set(uint32 agingTime);
extern int32 apollomp_raw_l2_agingTime_get(uint32 *pAgingTime);
extern int32 apollomp_raw_l2_ipmcLookupOp_set(apollomp_raw_l2_ipMcLookupOp_t type);
extern int32 apollomp_raw_l2_ipmcLookupOp_get(apollomp_raw_l2_ipMcLookupOp_t *pType);
extern int32 apollomp_raw_l2_ipmcHashType_set(apollomp_raw_l2_ipMcHashType_t type);
extern int32 apollomp_raw_l2_ipmcHashType_get(apollomp_raw_l2_ipMcHashType_t *pType);
extern int32 apollomp_raw_l2_flushLinkDownPortAddrEnable_set(rtk_enable_t enable);
extern int32 apollomp_raw_l2_flushLinkDownPortAddrEnable_get(rtk_enable_t *pEnable);
extern int32 apollomp_raw_l2_unknReservedMcFlood_set(rtk_enable_t state);
extern int32 apollomp_raw_l2_unknReservedMcFlood_get(rtk_enable_t *pState);


#endif /*#ifndef _APOLLOMP_RAW_L2_H_*/

