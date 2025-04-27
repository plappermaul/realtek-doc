/*
 * Copyright (C) 2009-2016 Realtek Semiconductor Corp.
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Definition those public Bridge Port Extension APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *           (1) Bridge Port Extension
 */

#ifndef __RTK_BPE_H__
#define __RTK_BPE_H__

/*
 * Include Files
 */
#include <common/rt_type.h>

/*
 * Symbol Definition
 */
#define BPE_ECID_MAX_NUM                (0x3FFFFF)  /* the maximum of ECID */
#define BPE_ECID_NSG_MAX_NUM            (0x00FF)    /* NSG specified as 8 bits*/
#define BPE_ECID_EXT_MAX_NUM            (0x00FF)    /* extension of ECID specified as 8 bits*/
#define BPE_ECID_BASE_MAX_NUM           (0x0FFF)    /* base of ECID specified as 12 bits*/

#define BPE_ETAG_TPID_DEFAULT           (0x893F)

/* Definition for flags in rtk_bpe_pvidEntry_t */
#define RTK_BPE_PVID_FLAG_TRUNK_PORT    0x00000001  /* trunk id is valid */





typedef enum rtk_bpe_fwdMode_e
{
    RTK_BPE_FWDMODE_CTRL_BRIDGE = 0,            /* Controlling Bridge */
    RTK_BPE_FWDMODE_PORT_EXTENDER,              /* Port Extender */
    RTK_BPE_FWDMODE_END
} rtk_bpe_fwdMode_t;

typedef enum rtk_bpe_tagSts_e
{
    RTK_BPE_TAGSTS_UNTAGGED = 0,                /* all untagged */
    RTK_BPE_TAGSTS_TAGGED,                      /* all tagged */
    RTK_BPE_TAGSTS_PCID_CHK_MCAST_UNTAGGED,     /* for unicast E-CID:
                                                 *     untagged if it's equal to PCID,
                                                 *     or tagged.
                                                 * for multicast E-CID:
                                                 *     untagged.                        */
    RTK_BPE_TAGSTS_PCID_CHK_MCAST_TAGGED,       /* for unicast E-CID:
                                                 *     untagged if it's equal to PCID,
                                                 *     or tagged.
                                                 * for multicast E-CID:
                                                 *     tagged.                          */
    RTK_BPE_TAGSTS_END
} rtk_bpe_tagSts_t;

typedef enum rtk_bpe_vlanTagSts_e
{
    RTK_BPE_VTAGSTS_NORMAL,                     /* follow VLAN untagged set */
    RTK_BPE_VTAGSTS_MCAST_TAGGED,               /* mcast channel always VLAN tagged out */
    RTK_BPE_VTAGSTS_END                         /* end of enum */
} rtk_bpe_vlanTagSts_t;

/* BPE port P-CID configuration type (PCFG) */
typedef enum rtk_bpe_pcidCfgType_e
{
    RTK_BPE_PCFG_NONE = 0,                      /* Dummy */

    RTK_BPE_PCFG_BASE_ONLY,                     /* 1: get/set base of P-CID only*/
    RTK_BPE_PCFG_EXT_ONLY,                      /* 2: get/set extension of P-CID only*/
    RTK_BPE_PCFG_EXT_AND_BASE,                  /* 3: get/set both base and extension*/

    RTK_BPE_PCFG_END                            /* end of enum */
} rtk_bpe_pcidCfgType_t;


/* BPE global control type (GCT) */
typedef enum rtk_bpe_globalCtrlType_e
{
    RTK_BPE_GCT_NONE = 0,                       /* Dummy */

    RTK_BPE_GCT_ETAG_TPID,                      /* uint32 */
    RTK_BPE_GCT_ETAG_CPU_KEEP,                  /* rtk_enable_t */
    RTK_BPE_GCT_ECID_RPF_FAIL_ACT,              /* rtk_action_t */
    RTK_BPE_GCT_PE_LM_FTLR,                     /* rtk_enable_t */

    RTK_BPE_GCT_END                             /* end of enum */
} rtk_bpe_globalCtrlType_t;

/* BPE port control type (PCT) */
typedef enum rtk_bpe_portCtrlType_e
{
    RTK_BPE_PCT_NONE = 0,                       /* Dummy */

    RTK_BPE_PCT_PETAG_PARSE_EN,                 /* rtk_enable_t */

    /* PE_PORT_ETAG_IGR_CTRL */
    RTK_BPE_PCT_IGR_RSVD_ECID_FLTR_EN,          /* rtk_enable_t */
    RTK_BPE_PCT_IGR_MC_ECID_FLTR_EN,            /* rtk_enable_t */
    RTK_BPE_PCT_IGR_ECID_RPF_CHK_EN,            /* rtk_enable_t */

    /* PE_PORT_ETAG_EGR_CTRL */
    RTK_BPE_PCT_EGR_TAG_DEI_RMK_EN,             /* rtk_enable_t */
    RTK_BPE_PCT_EGR_TAG_PRI_KEEP_EN,            /* rtk_enable_t */
    RTK_BPE_PCT_EGR_SRC_PORT_FLTR_EN,           /* rtk_enable_t */

    RTK_BPE_PCT_USE_DEFAULT,                    /* rtk_enable_t */
    RTK_BPE_PCT_BASE_PE,                        /* rtk_enable_t */

    RTK_BPE_PCT_END                             /* end of enum */
} rtk_bpe_portCtrlType_t;

/* types of PE-tag priority remarking source */
typedef enum rtk_bpe_priRmkSrc_e
{
    PETAG_PRI_RMK_SRC_INT_PRI,                  /* internal priority */
    PETAG_PRI_RMK_SRC_END                       /* end of enum */
} rtk_bpe_priRmkSrc_t;


/*
 * Data Declaration
 */
typedef struct rtk_bpe_pvidEntry_s
{
    uint32          flags;          /* Configuration flags */
    uint32          nsg_id;         /* hash key (name space group id) */
    rtk_bpe_ecid_t  ecid;           /* hash key */

    rtk_vlan_t      pvid;           /* extended port's PVID */

    uint32          entry_idx;      /* returned physical entry index after a successful read/write */

    /* port binding */
    union
    {
        rtk_port_t      port_id;       /* available when flag RTK_BPE_PVID_FLAG_TRUNK_PORT is NOT set */
        rtk_trunk_t     trunk_id;    /* available when flag RTK_BPE_PVID_FLAG_TRUNK_PORT is set */
    };
} rtk_bpe_pvidEntry_t;

typedef struct rtk_bpe_fwdEntry_s
{
    uint32          nsg_id;         /* hash key (name space group id) */
    rtk_bpe_ecid_t  ecid;           /* hash key */

    rtk_portmask_t  portmask;       /* member ports */

    uint32          entry_idx;      /* returned physical entry index after a successful read/write */

} rtk_bpe_fwdEntry_t;

typedef struct rtk_bpe_priRmkVal_s
{
    union
    {
        struct
        {
            rtk_pri_t val;
        } pri;
    };
} rtk_bpe_priRmkVal_t;



/*
 * Macro Declaration
 */
#define BPE_ECID_GRP(_ecid)         (((_ecid) & (0x3U << 20)) >> 20)
#define BPE_ECID_EXT(_ecid)         (((_ecid) & (0xFFU << 12)) >> 12)
#define BPE_ECID_BASE(_ecid)        (((_ecid) & (0xFFFU << 0)) >> 0)
#define BPE_ECID_PCID(_ext, _base)  ((((_ext) & 0xFFU) << 12) | (((_base) & 0xFFFU) << 0))


#define BPE_ECID_GRP_SET(_ecid, _grp)       (((_ecid) & ~(0x3U << 20)) | (((_grp) & 0x3U) << 20))
#define BPE_ECID_EXT_SET(_ecid, _ext)       (((_ecid) & ~(0xFFU << 12)) | (((_ext) & 0xFFU) << 12))
#define BPE_ECID_BASE_SET(_ecid, _base)     (((_ecid) & ~(0xFFFU << 0)) | (((_base) & 0xFFFU) << 0))


/*
 * Function Declaration
 */

/* Function Name:
 *      rtk_bpe_init
 * Description:
 *      Initialize BPE module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      9310
 * Note:
 *      Must initialize BPE module before calling any BPE APIs.
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_bpe_init(uint32 unit);

/* Module Name    : BPE (Bridge Port Extension) */
/* Sub-module Name: BPE (Bridge Port Extension) */

/* Function Name:
 *      rtk_bpe_portFwdMode_get
 * Description:
 *      Get the forwarding mode of a specified port
 * Input:
 *      unit  - unit id
 *      port  - port id
 * Output:
 *      pMode - pointer to the forwarding mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - the module is not initial
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_bpe_portFwdMode_get(uint32 unit, rtk_port_t port, rtk_bpe_fwdMode_t *pMode);

/* Function Name:
 *      rtk_bpe_portFwdMode_set
 * Description:
 *      Set the forwarding mode of a specified port
 * Input:
 *      unit - unit id
 *      port - port id
 *      mode - forwarding mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - the module is not initial
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_bpe_portFwdMode_set(uint32 unit, rtk_port_t port, rtk_bpe_fwdMode_t mode);

/* Function Name:
 *      rtk_bpe_portEcidNameSpaceGroupId_get
 * Description:
 *      Get the E-CID naming space group ID of a specified port
 * Input:
 *      unit     - unit id
 *      port     - port id
 * Output:
 *      pGroupId - pointer to the group id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - the module is not initial
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_bpe_portEcidNameSpaceGroupId_get(uint32 unit, rtk_port_t port, uint32 *pGroupId);

/* Function Name:
 *      rtk_bpe_portEcidNameSpaceGroupId_set
 * Description:
 *      Set the E-CID naming space group ID of a specified port
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      groupId - group id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - the module is not initial
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_bpe_portEcidNameSpaceGroupId_set(uint32 unit, rtk_port_t port, uint32 groupId);

/* Function Name:
 *      rtk_bpe_portPcid_get
 * Description:
 *      Get the PCID of a specified port
 * Input:
 *      unit  - unit id
 *      port  - port id
 *      type  - ecid configutation
 * Output:
 *      pPcid - pointer to PCID (Port's E-CID)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - the module is not initial
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_bpe_portPcid_get(uint32 unit, rtk_port_t port,
    rtk_bpe_pcidCfgType_t type, rtk_bpe_ecid_t *pPcid);

/* Function Name:
 *      rtk_bpe_portPcid_set
 * Description:
 *      Set the PCID of a specified port
 * Input:
 *      unit - unit id
 *      port - port id
 *      type - ecid configutation
 *      pcid - PCID (Port's E-CID)
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - the module is not initial
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_bpe_portPcid_set(uint32 unit, rtk_port_t port,
    rtk_bpe_pcidCfgType_t type, rtk_bpe_ecid_t pcid);

/* Function Name:
 *      rtk_bpe_portPcidAct_get
 * Description:
 *      Get the action of PCID packet of a specified port
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pAction - pointer to action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - the module is not initial
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_bpe_portPcidAct_get(uint32 unit, rtk_port_t port, rtk_action_t *pAction);

/* Function Name:
 *      rtk_bpe_portPcidAct_set
 * Description:
 *      Set the action of PCID packet of a specified port
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      action - action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - the module is not initial
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_bpe_portPcidAct_set(uint32 unit, rtk_port_t port, rtk_action_t action);

/* Function Name:
 *      rtk_bpe_portEgrTagSts_get
 * Description:
 *      Get the egress PE-tag status configuration of a specified port
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pStatus - pointer to the tag status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - the module is not initial
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_bpe_portEgrTagSts_get(uint32 unit, rtk_port_t port, rtk_bpe_tagSts_t *pStatus);

/* Function Name:
 *      rtk_bpe_portEgrTagSts_set
 * Description:
 *      Set the egress PE-tag status configuration of a specified port
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      status - tag status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - the module is not initial
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_bpe_portEgrTagSts_set(uint32 unit, rtk_port_t port, rtk_bpe_tagSts_t status);

/* Function Name:
 *      rtk_bpe_portEgrVlanTagSts_get
 * Description:
 *      Get the egress VLAN tag status configuration of a specified port
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pVlan_status - pointer to the tag status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - the module is not initial
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_bpe_portEgrVlanTagSts_get(uint32 unit, rtk_port_t port, rtk_bpe_vlanTagSts_t *pVlan_status);

/* Function Name:
 *      rtk_bpe_portEgrVlanTagSts_set
 * Description:
 *      Set the egress VLAN tag status configuration of a specified port
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      vlan_status - tag status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - the module is not initial
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_bpe_portEgrVlanTagSts_set(uint32 unit, rtk_port_t port, rtk_bpe_vlanTagSts_t vlan_status);


/* Function Name:
 *      rtk_bpe_pvidEntry_add
 * Description:
 *      Add an extended port's PVID entry on the specified device.
 * Input:
 *      unit   - unit id
 *      pEntry - pointer to the PVID entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_bpe_pvidEntry_add(uint32 unit, rtk_bpe_pvidEntry_t *pEntry);

/* Function Name:
 *      rtk_bpe_pvidEntry_del
 * Description:
 *      Delete an extended port's PVID entry on the specified device.
 * Input:
 *      unit   - unit id
 *      pEntry - pointer to the PVID entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_bpe_pvidEntry_del(uint32 unit, rtk_bpe_pvidEntry_t *pEntry);

/* Function Name:
 *      rtk_bpe_pvidEntry_get
 * Description:
 *      Get an extended port's PVID entry on the specified device.
 * Input:
 *      unit   - unit id
 *      pEntry - pointer to the PVID entry
 * Output:
 *      pEntry - pointer to the returned PVID entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_bpe_pvidEntry_get(uint32 unit, rtk_bpe_pvidEntry_t *pEntry);

/* Function Name:
 *      rtk_bpe_pvidEntryNextValid_get
 * Description:
 *      Get a next valid PVID entry from PVID table.
 * Input:
 *      unit      - unit id
 *      pScan_idx - currently scan index of PVID table to get next
 * Output:
 *      pScan_idx - returned found scan index (-1 means not found)
 *      pEntry    - pointer to PVID entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - invalid unit id
 *      RT_ERR_NOT_INIT         - the module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_ENTRY_NOT_EXIST  - entry is not existed
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_bpe_pvidEntryNextValid_get(uint32 unit, int32 *pScan_idx, rtk_bpe_pvidEntry_t *pEntry);

/* Function Name:
 *      rtk_bpe_fwdEntry_add
 * Description:
 *      Add an E-CID forwarding entry on the specified device.
 * Input:
 *      unit   - unit id
 *      pEntry - pointer to the E-CID forwarding entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_bpe_fwdEntry_add(uint32 unit, rtk_bpe_fwdEntry_t *pEntry);

/* Function Name:
 *      rtk_bpe_fwdEntry_del
 * Description:
 *      Delete an E-CID forwarding entry on the specified device.
 * Input:
 *      unit   - unit id
 *      pEntry - pointer to the E-CID forwarding entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_bpe_fwdEntry_del(uint32 unit, rtk_bpe_fwdEntry_t *pEntry);

/* Function Name:
 *      rtk_bpe_fwdEntry_get
 * Description:
 *      Get an E-CID forwarding entry on the specified device.
 * Input:
 *      unit   - unit id
 *      pEntry - pointer to the E-CID forwarding entry
 * Output:
 *      pEntry - pointer to the returned E-CID forwarding entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_bpe_fwdEntry_get(uint32 unit, rtk_bpe_fwdEntry_t *pEntry);

/* Function Name:
 *      rtk_bpe_fwdEntryNextValid_get
 * Description:
 *      Get a next valid E-CID forwarding entry on the specified device.
 * Input:
 *      unit      - unit id
 *      pScan_idx - currently scan index of E-CID forwarding table to get next.
 * Output:
 *      pScan_idx - returned found scan index (-1 means not found)
 *      pEntry    - pointer to E-CID forwarding entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - invalid unit id
 *      RT_ERR_NOT_INIT         - the module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_ENTRY_NOT_EXIST  - entry is not existed
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_bpe_fwdEntryNextValid_get(uint32 unit, int32 *pScan_idx, rtk_bpe_fwdEntry_t *pEntry);

/* Function Name:
 *      rtk_bpe_globalCtrl_get
 * Description:
 *      Get the configuration of the specified control type
 * Input:
 *      unit - unit id
 *      type - control type
 * Output:
 *      pArg - pointer to the argument
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_bpe_globalCtrl_get(uint32 unit, rtk_bpe_globalCtrlType_t type, int32 *pArg);

/* Function Name:
 *      rtk_bpe_globalCtrl_set
 * Description:
 *      Set the configuration of the specified control type
 * Input:
 *      unit - unit id
 *      type - control type
 *      arg  - argument
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_bpe_globalCtrl_set(uint32 unit, rtk_bpe_globalCtrlType_t type, int32 arg);

/* Function Name:
 *      rtk_bpe_portCtrl_get
 * Description:
 *      Get the configuration of the specified control type and port
 * Input:
 *      unit - unit id
 *      port - port id
 *      type - control type
 * Output:
 *      pArg - pointer to the argument
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 *      RT_ERR_PORT_ID      - invalid port id
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_bpe_portCtrl_get(uint32 unit, rtk_port_t port, rtk_bpe_portCtrlType_t type, int32 *pArg);

/* Function Name:
 *      rtk_bpe_portCtrl_set
 * Description:
 *      Set the configuration of the specified control type and port
 * Input:
 *      unit - unit id
 *      port - port id
 *      type - control type
 *      arg  - argument
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - the module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_bpe_portCtrl_set(uint32 unit, rtk_port_t port, rtk_bpe_portCtrlType_t type, int32 arg);

/* Function Name:
 *      rtk_bpe_priRemarking_get
 * Description:
 *      Get the remarked priority (3 bits) of a specified source.
 * Input:
 *      unit - unit id
 *      src  - remark source
 *      val  - remark source value
 * Output:
 *      pPri - pointer to the remarked priority value (range from 0 ~ 7)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - invalid unit id
 *      RT_ERR_NOT_INIT         - the module is not initial
 *      RT_ERR_QOS_INT_PRIORITY - invalid priority
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Applicable:
 *      9310
 * Note:
 *      Supported remarking source is as following:
 *      - rtk_bpe_priRmkSrc_t \ Chip:           9310
 *      - PETAG_PRI_RMK_SRC_INT_PRI             O
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_bpe_priRemarking_get(uint32 unit, rtk_bpe_priRmkSrc_t src, rtk_bpe_priRmkVal_t val, rtk_pri_t *pPri);

/* Function Name:
 *      rtk_bpe_priRemarking_set
 * Description:
 *      Set the remarked priority (3 bits) of a specified source.
 * Input:
 *      unit - unit id
 *      src  - remark source
 *      val  - remark source value
 *      pri  - remarked priority value (range from 0 ~ 7)
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - invalid unit id
 *      RT_ERR_NOT_INIT         - the module is not initial
 *      RT_ERR_QOS_INT_PRIORITY - invalid priority
 * Applicable:
 *      9310
 * Note:
 *      Supported remarking source is as following:
 *      - rtk_bpe_priRmkSrc_t \ Chip:           9310
 *      - PETAG_PRI_RMK_SRC_INT_PRI             O
 * Changes:
 *      [SDK_3.0.0]
 *          New added function.
 */
extern int32
rtk_bpe_priRemarking_set(uint32 unit, rtk_bpe_priRmkSrc_t src, rtk_bpe_priRmkVal_t val, rtk_pri_t pri);

#endif /* __RTK_BPE_H__ */

