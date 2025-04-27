/*
 * Copyright (C) 2009-2016 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Definition those public L2 APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) l2 address table
 *
 */
#ifndef __DAL_MANGO_L2_H__
#define __DAL_MANGO_L2_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_list.h>
#include <rtk/l2.h>
#include <dal/dal_mapper.h>

/*
 * Symbol Definition
 */
#define RTL9310_L2ENTRY_DEVI_ID_OFFSET  6
#define RTL9310_L2ENTRY_DEVI_ID_MASK    0xf
#define RTL9310_L2ENTRY_PORT_MASK       0x3f
#define END_OF_MCAST_IDX                (0xFFFF)
#define MCAST_IDX_ALLOCATED             (0xFFFE)
#define STANDALONE_TRUNK_ID_MAX         26
#define LUT_TBL_BLK_NUM                 2
#define PORT_BLACK_HOLE_9310            61


/*
 * Data Type Definition
 */
typedef enum dal_mango_l2_getMethod_e
{
    L2_GET_EXIST_ONLY = 0,
    L2_GET_EXIST_OR_FREE,
    L2_GET_FREE_ONLY,
    DAL_MANGO_GETMETHOD_END
} dal_mango_l2_getMethod_t;

typedef struct dal_mango_mcast_index_s {
    uint16  next_index;
    uint16  ref_count;
} dal_mango_mcast_index_t;

typedef struct dal_mango_mcast_index_pool_s {
    dal_mango_mcast_index_t *pMcast_index_pool;
    uint32                  size_of_mcast_fwd_index;
    uint16                  free_index_head;
    uint16                  free_entry_count;
} dal_mango_mcast_index_pool_t;

typedef enum dal_mango_l2_entry_type_e
{
    L2_UNICAST = 0,
    L2_MULTICAST,
    ENTRY_OPENFLOW,
    ENTRY_PE_FWD,
    L2_ENTRY_TYPE_END
} dal_mango_l2_entry_type_t;

typedef enum dal_mango_l2_bpe_entry_type_e
{
    L2_BPE_CB,
    L2_BPE_PE,
    L2_BPE_ENTRY_TYPE_END
} dal_mango_l2_bpe_entry_type_t;


typedef enum dal_mango_l2_indexType_e
{
    L2_IN_HASH = 0,
    L2_IN_CAM,
    L2_INDEXTYPE_END
} dal_mango_l2_indexType_t;

typedef enum dal_mango_l2_entryType_e
{
    L2_TYPE_L2 = 0,     /* for L2 entry */
    L2_TYPE_OPENFLOW,   /* for OpenFlow */
    L2_ENTRYTYPE_END
} dal_mango_l2_entryType_t;


typedef struct dal_mango_l2_entry_s
{
    dal_mango_l2_entry_type_t   entry_type; /* unicast, l2 multicast */
    uint32                      is_entry_exist;
    uint32                      is_entry_valid;
    union {
        struct unicast_entry_s
        {
            uint32      valid;
            rtk_fid_t   fid;
            rtk_mac_t   mac;
            uint32      is_trk;
            rtk_port_t  slp;
            uint32      aging;
            uint32      sablock;
            uint32      dablock;
            uint32      is_static;
            uint32      suspending;
            uint32      nh;
            uint32      l2_tnl_if;
            uint32      l2_tnl_idx;
            uint32      tagSts;
            uint32      agg_pri;
            rtk_vlan_t  agg_vid;
            rtk_bpe_ecid_t  ecid;
        } unicast;
        struct l2mcast_entry_s
        {
            uint32      valid;
            rtk_vlan_t  fid;
            rtk_mac_t   mac;
            uint32      index;
            uint32      nh;
            uint32      local_fwd;
            uint32      remote_fwd;
            uint32      tunnel_list_idx;
        } l2mcast;
    };
} dal_mango_l2_entry_t;

typedef struct dal_mango_l2_index_s
{
    dal_mango_l2_indexType_t  index_type;       /* In CAM or In HASH */
    uint32  index;
    uint32  hashdepth;      /* only useful when entry is in hash table */
} dal_mango_l2_index_t;

typedef struct dal_mango_l2_ucastNhAddr_s
{
    rtk_fid_t           fid;
    rtk_mac_t           mac;
    uint32              l2_idx;
    rtk_l2_macInfo_t    macInfo;
    uint32              add_op_flags;   /* Refer to RTK_L2_ADD_OP_FLAG_XXX */
    uint32              isNh;
} dal_mango_l2_ucastNhAddr_t;

typedef struct dal_mango_l2_mcastNhAddr_s
{
    rtk_fid_t           vid;
    rtk_mac_t           mac;
    uint32              l2_idx;
} dal_mango_l2_mcastNhAddr_t;

typedef struct dal_mango_l2_hashIdx_s
{
    uint32 blk0_hashIdx;
    uint32 blk1_hashIdx;
} dal_mango_l2_hashIdx_t;

/*
 * Macro Definition
 */


/*
 * Function Declaration
 */

/* Module Name    : L2     */
/* Sub-module Name: Global */


extern int32
_dal_mango_l2_semLock(uint32 unit);

extern int32
_dal_mango_l2_semUnlock(uint32 unit);

extern int32
_dal_mango_l2_mcastFwdIndex_alloc(uint32 unit, int32 *pFwdIndex);

extern int32
_dal_mango_l2_mcastFwdIndex_free(uint32 unit, int32 index);

extern int32
_dal_mango_l2_mcastFwdPortmaskEntry_get(uint32 unit, int32 index, rtk_portmask_t *pPortmask);

extern int32
_dal_mango_l2_mcastFwdPortmaskEntry_set(uint32 unit, int32 index, rtk_portmask_t *pPortmask);

extern int32
_dal_mango_l2_entryToHashIdx(uint32 unit, dal_mango_l2_entry_t *pL2_entry, dal_mango_l2_hashIdx_t *pKey);

extern int32
_dal_mango_l2_getL2Entry(uint32 unit, dal_mango_l2_index_t * pIndex, dal_mango_l2_entry_t *pL2_entry);

extern int32
_dal_mango_l2_setL2Entry(uint32 unit, dal_mango_l2_index_t * pIndex, dal_mango_l2_entry_t *pL2_entry);

extern int32
_dal_mango_l2_tablePhyIndex_get(uint32 index_l, uint32 *index_p);

extern int32
_dal_mango_l2_tableLogIndex_get(uint32 index_p, uint32 *index_l);

extern int32
_dal_mango_l2_hashPhyIndex_get(uint32 index_p, uint32 *index_l);

/* Function Name:
 *      dal_mango_l2Mapper_init
 * Description:
 *      Hook l2 module of the specified device.
 * Input:
 *      pMapper - pointer of mapper
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      Must Hook l2 module before calling any l2 APIs.
 */
extern int32
dal_mango_l2Mapper_init(dal_mapper_t *pMapper);

/* Function Name:
 *      dal_mango_l2_init
 * Description:
 *      Initialize l2 module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      Must initialize l2 module before calling any l2 APIs.
 */
extern int32
dal_mango_l2_init(uint32 unit);

/* Module Name    : L2                */
/* Sub-module Name: Mac address flush */

/* Function Name:
 *      dal_mango_l2_flushLinkDownPortAddrEnable_get
 * Description:
 *      Get HW flush dynamic mac entries corresponding to linkdown port configuration.
 * Input:
 *      unit    - unit id
 * Output:
 *      pEnable - pointer buffer of state of HW clear linkdown port mac
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) The status of flush linkdown port address is as following:
 *          - DISABLED
 *          - ENABLED
 */
extern int32
dal_mango_l2_flushLinkDownPortAddrEnable_get(uint32 unit, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_mango_l2_flushLinkDownPortAddrEnable_set
 * Description:
 *      Set HW flush dynamic mac entries corresponding to linkdown port configuration.
 * Input:
 *      unit   - unit id
 *      enable - configure value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      (1) The status of flush linkdown port address is as following:
 *          - DISABLED
 *          - ENABLED
 */
extern int32
dal_mango_l2_flushLinkDownPortAddrEnable_set(uint32 unit, rtk_enable_t enable);

/* Function Name:
 *      dal_mango_l2_ucastAddr_flush
 * Description:
 *      Flush unicast address
 * Input:
 *      unit    - unit id
 *      pConfig - flush config
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_VLAN_VID     - invalid vlan id
 * Note:
 *      The L2 unicast entries in L2 table which meet the specified criteria are flushed by HW.
 */
extern int32
dal_mango_l2_ucastAddr_flush(uint32 unit, rtk_l2_flushCfg_t *pConfig);

/* Module Name    : L2                     */
/* Sub-module Name: Address learning limit */

/* Function Name:
 *      dal_mango_l2_macLearningCnt_get
 * Description:
 *      Get number of learned MAC addresses of specified type.
 * Input:
 *      unit             - unit id
 *      fid_macLimit_idx - index of VLAN MAC limit entry
 * Output:
 *      pNum             - number of learned MAC addresses
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) The valid range of fid_macLimit_idx is 0~31 in 8390 and 9310, and 0~7 in 8380 and 9300
 */
extern int32
dal_mango_l2_macLearningCnt_get(uint32 unit, rtk_l2_macLimitType_t type, rtk_l2_macCnt_t *pLimitCnt);

/* Function Name:
 *      dal_mango_l2_limitLearningNum_get
 * Description:
 *      Get the mac limit learning counts of specified device.
 * Input:
 *      unit     - unit id
 *      type - mac limit type
 *      pLimitCnt - pointer ro mac limit parameters
 * Output:
 *      pLimitCnt - pointer of mac limit learning counts
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_TRUNK_ID     - invalid trunk id
 * Note:
 *      (1) The mac learning counts only calculate dynamic mac numbers.
 */
extern int32
dal_mango_l2_limitLearningNum_get(uint32 unit, rtk_l2_macLimitType_t type, rtk_l2_macCnt_t *pLimitCnt);


/* Function Name:
 *      dal_mango_l2_limitLearningNum_set
 * Description:
 *      Set the mac limit learning counts of specified device.
 * Input:
 * Input:
 *      unit     - unit id
 *      type - mac limit type
 *      pLimitCnt - pointer ro mac limit parameters and mac limit learning counts
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_TRUNK_ID     - invalid trunk id
 * Note:
 *      (1) The mac learning counts only calculate dynamic mac numbers.
 */
extern int32
dal_mango_l2_limitLearningNum_set(uint32 unit, rtk_l2_macLimitType_t type, rtk_l2_macCnt_t *pLimitCnt);

/* Function Name:
 *      dal_mango_l2_limitLearningAction_get
 * Description:
 *      Get the mac limit action of specified device.
 * Input:
 *      unit     - unit id
 *      type - mac limit type
 *      pAction - pointer ro mac limit parameters
 * Output:
 *      pAction - pointer of mac limit action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_TRUNK_ID     - invalid trunk id
 * Note:
 *      (1) The mac learning counts only calculate dynamic mac numbers.
 */
extern int32
dal_mango_l2_limitLearningAction_get(uint32 unit, rtk_l2_macLimitType_t type, rtk_l2_macLimitAction_t *pAction);

/* Function Name:
 *      dal_mango_l2_limitLearningAction_set
 * Description:
 *      Set the mac limit action of specified device.
 * Input:
 * Input:
 *      unit     - unit id
 *      type - mac limit type
 *      pAction - pointer ro mac limit parameters and mac limit action
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_TRUNK_ID     - invalid trunk id
 * Note:
 *      (1) The mac learning counts only calculate dynamic mac numbers.
 */
extern int32
dal_mango_l2_limitLearningAction_set(uint32 unit, rtk_l2_macLimitType_t type, rtk_l2_macLimitAction_t *pAction);

/* Function Name:
 *      dal_mango_l2_fidLimitLearningEntry_get
 * Description:
 *      Get the specified VLAN MAC limit configuration.
 * Input:
 *      unit              - unit id
 *      fid_macLimit_idx  - index of VLAN MAC limit entry
 * Output:
 *      pFidMacLimitEntry - pointer to MAC limit configuration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) The valid range of fid_macLimit_idx is 0~31 in 8390 and 9310, and 0~7 in 8380 and 9300
 *      (2) Forwarding action is as following
 *          - LIMIT_LEARN_CNT_ACTION_DROP
 *          - LIMIT_LEARN_CNT_ACTION_FORWARD
 *          - LIMIT_LEARN_CNT_ACTION_TO_CPU
 *          - LIMIT_LEARN_CNT_ACTION_COPY_TO_CPU
 */
extern int32
dal_mango_l2_fidLimitLearningEntry_get(
    uint32                    unit,
    uint32                    fid_macLimit_idx,
    rtk_l2_fidMacLimitEntry_t *pFidMacLimitEntry);

/* Function Name:
 *      dal_mango_l2_fidLimitLearningEntry_set
 * Description:
 *      Set the specified VLAN MAC limit configuration.
 * Input:
 *      unit              - unit id
 *      fid_macLimit_idx  - index of VLAN MAC limit entry
 *      pFidMacLimitEntry - MAC limit configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_VLAN_VID     - invalid vlan id
 * Note:
 *      (1) The valid range of fid_macLimit_idx is 0~31 in 8390 and 9310, and 0~7 in 8380 and 9300
 *      (2) Forwarding action is as following
 *          - LIMIT_LEARN_CNT_ACTION_DROP
 *          - LIMIT_LEARN_CNT_ACTION_FORWARD
 *          - LIMIT_LEARN_CNT_ACTION_TO_CPU
 *          - LIMIT_LEARN_CNT_ACTION_COPY_TO_CPU
 *      (3) This API will flush all MACs learnt on specified VLAN before this entry takes effect.
 *          If the fid of an entry changes from one to another, this API will flush MACs learnt on these two VLANs.
 */
extern int32
dal_mango_l2_fidLimitLearningEntry_set(
    uint32                    unit,
    uint32                    fid_macLimit_idx,
    rtk_l2_fidMacLimitEntry_t *pFidMacLimitEntry);

/* Function Name:
 *      dal_mango_l2_fidLearningCnt_reset
 * Description:
 *      Reset number of learned MAC addresses on specified VLAN MAC limit entry.
 * Input:
 *      unit             - unit id
 *      fid_macLimit_idx - index of VLAN MAC limit entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *      (1) The valid range of fid_macLimit_idx is 0~31 in 8390 and 9310, and 0~7 in 8380 and 9300
 */
extern int32
dal_mango_l2_fidLearningCnt_reset(uint32 unit, uint32 fid_macLimit_idx);

/* Function Name:
 *      _dal_mango_l2_agingTime_set
 * Description:
 *      Set the aging time of suspending entry from the specified device.
 * Input:
 *      unit       - unit id
 *      type       - age time type
 *      aging_time - aging time
 *      arb_id     - id for arbiter
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      The unit is second.
 */
extern int32
_dal_mango_l2_agingTime_set(uint32 unit, rtk_l2_ageTimeType_t type, uint32 aging_time, uint32 arb_id);
/* Module Name    : L2                                          */
/* Sub-module Name: Parameter for L2 lookup and learning engine */
/* Function Name:
 *      dal_mango_l2_agingTime_get
 * Description:
 *      Get the aging time of from the specified device.
 * Input:
 *      unit        - unit id
 *      type       - age time type
 * Output:
 *      pAging_time - pointer buffer of aging time
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The unit is second.
 */
extern int32
dal_mango_l2_agingTime_get(uint32 unit, rtk_l2_ageTimeType_t type, uint32 *pAging_time);

/* Function Name:
 *      dal_mango_l2_agingTime_set
 * Description:
 *      Set the aging time of suspending entry from the specified device.
 * Input:
 *      unit       - unit id
 *      type       - age time type
 *      aging_time - aging time
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      The unit is second.
 */
extern int32
dal_mango_l2_agingTime_set(uint32 unit, rtk_l2_ageTimeType_t type, uint32 aging_time);

/* Function Name:
 *      dal_mango_l2_portAgingEnable_get
 * Description:
 *      Get the dynamic address aging out state configuration of the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - enable status of aging out
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_mango_l2_portAgingEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_mango_l2_portAgingEnable_set
 * Description:
 *      Set the dynamic address aging out state configuration of the specified port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - enable status of aging out
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_mango_l2_portAgingEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      dal_mango_l2_trkAgingEnable_get
 * Description:
 *      Get the dynamic address aging out state configuration of the specified trunk.
 * Input:
 *      unit    - unit id
 *      trunk   - trunk id
 * Output:
 *      pEnable - enable status of aging out
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_TRUNK_ID     - invalid trunk id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_mango_l2_trkAgingEnable_get(uint32 unit, rtk_trk_t trunk, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_mango_l2_trkAgingEnable_set
 * Description:
 *      Set the dynamic address aging out state configuration of the specified trunk.
 * Input:
 *      unit    - unit id
 *      trunk   - trunk id
 *      enable  - enable status of aging out
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_TRUNK_ID - invalid trunk id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_mango_l2_trkAgingEnable_set(uint32 unit, rtk_trk_t trunk, rtk_enable_t enable);

/* Function Name:
 *      dal_mango_l2_hashAlgo_get
 * Description:
 *      Get hash algorithm of layer2 table.
 * Input:
 *      unit        - unit id
 * Output:
 *      pHash_algo  - pointer to hash algorithm of layer2 switching
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_BUCKET_ID    - invalid bucket id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      none.
 */
extern int32
dal_mango_l2_hashAlgo_get(uint32 unit, uint32 *pHash_algo);

/* Function Name:
 *      dal_mango_l2_hashAlgo_set
 * Description:
 *      Set hash algorithm of layer2 table.
 * Input:
 *      unit        - unit id
 * Output:
 *      pHash_algo  - pointer to hash algorithm of layer2 switching
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_BUCKET_ID    - invalid bucket id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      none.
 */
extern int32
dal_mango_l2_hashAlgo_set(uint32 unit, uint32 hash_algo);

/* Function Name:
 *      dal_mango_l2_bucketHashAlgo_get
 * Description:
 *      Get bucket hash algorithm of layer2 table.
 * Input:
 *      unit        - unit id
 *      bucket      - bucket id
 * Output:
 *      pHash_algo  - pointer to hash algorithm of layer2 switching
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_BUCKET_ID    - invalid bucket id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      bucket can be 0 to 1.
 */
extern int32
dal_mango_l2_bucketHashAlgo_get(uint32 unit, uint32 bucket, uint32 *pHash_algo);

/* Function Name:
 *      dal_mango_l2_bucketHashAlgo_set
 * Description:
 *      Set bucket hash algorithm of layer2 table.
 * Input:
 *      unit        - unit id
 *      bucket      - bucket id
 *      hash_algo   - hash algorithm of layer2 switching
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_BUCKET_ID    - invalid bucket id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      The valid bucket is 0 to 1, and hash_algo is 0 and 1
 */
extern int32
dal_mango_l2_bucketHashAlgo_set(uint32 unit, uint32 bucket, uint32 hash_algo);

/* Module Name    : L2      */
/* Sub-module Name: Unicast */

/* Function Name:
 *      dal_mango_l2_learningFullAction_get
 * Description:
 *      Get the forwarding action which is taken when SA learning full.
 * Input:
 *      unit    - unit id
 * Output:
 *      pAction - pointer to forwarding action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_mango_l2_learningFullAction_get(uint32 unit, rtk_action_t *pAction);

/* Function Name:
 *      dal_mango_l2_learningFullAction_set
 * Description:
 *      Set the forwarding action which is taken when SA learning full.
 * Input:
 *      unit   - unit id
 *      action - forwarding action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_mango_l2_learningFullAction_set(uint32 unit, rtk_action_t action);

/* Function Name:
 *      _dal_mango_l2_portNewMacOp_set
 * Description:
 *      Set learning mode and forwarding action of new learned address on specified port.
 * Input:
 *      unit      - unit id
 *      port      - port id
 *      mode      - learning mode value
 *      act       - forwarding action value
 *      arb_id    - id for arbiter
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      1. This is a arbiter API, only when all caller are agree enable learning, can SALRN be enabled
 *         and uses the setting storing in the l2_lrn[ARB_NORMAL_API] to restore
 *      2. Only dal_mango_l2_portNewMacOp_set (ARB_NORMAL_API) can set NEW_SA_FWD action
 */
extern int32
_dal_mango_l2_portNewMacOp_set(uint32 unit, rtk_port_t port, uint32 mode, uint32 act, uint32 arb_id);

/* Function Name:
 *      dal_mango_l2_portNewMacOp_get
 * Description:
 *      Get learning mode and forwarding action of new learned address on specified port.
 * Input:
 *      unit       - unit id
 *      port       - port id
 * Output:
 *      pLrnMode   - pointer to learning mode
 *      pFwdAction - pointer to forwarding action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) Forwarding action is as following
 *          - ACTION_FORWARD
 *          - ACTION_DROP
 *          - ACTION_TRAP2CPU
 *          - ACTION_COPY2CPU
 *          - ACTION_TRAP2MASTERCPU
 *          - ACTION_COPY2MASTERCPU
 *
 *      (2) Learning mode is as following
 *          - HARDWARE_LEARNING
 *          - SOFTWARE_LEARNING
 *          - NOT_LEARNING
 */
extern int32
dal_mango_l2_portNewMacOp_get(
    uint32                  unit,
    rtk_port_t              port,
    rtk_l2_newMacLrnMode_t  *pLrnMode,
    rtk_action_t            *pFwdAction);

/* Function Name:
 *      dal_mango_l2_portNewMacOp_set
 * Description:
 *      Set learning mode and forwarding action of new learned address on specified port.
 * Input:
 *      unit      - unit id
 *      port      - port id
 *      lrnMode   - learning mode
 *      fwdAction - forwarding action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID    - invalid unit id
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_PORT_ID    - invalid port id
 *      RT_ERR_INPUT      - invalid input parameter
 *      RT_ERR_FWD_ACTION - invalid forwarding action
 * Note:
 *      (1) Forwarding action is as following
 *          - ACTION_FORWARD
 *          - ACTION_DROP
 *          - ACTION_TRAP2CPU
 *          - ACTION_COPY2CPU
 *          - ACTION_TRAP2MASTERCPU
 *          - ACTION_COPY2MASTERCPU
 *
 *      (2) Learning mode is as following
 *          - HARDWARE_LEARNING
 *          - SOFTWARE_LEARNING
 *          - NOT_LEARNING
 */
extern int32
dal_mango_l2_portNewMacOp_set(
    uint32                  unit,
    rtk_port_t              port,
    rtk_l2_newMacLrnMode_t  lrnMode,
    rtk_action_t            fwdAction);

/* Module Name    : L2              */
/* Sub-module Name: Unicast address */

/* Function Name:
 *      dal_mango_l2_addr_init
 * Description:
 *      Initialize content of buffer of L2 entry.
 *      Will fill vid ,MAC address and reset other field of L2 entry.
 * Input:
 *      unit     - unit id
 *      vid      - vlan id
 *      pMac     - MAC address
 *      pL2_addr - L2 entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_VLAN_VID     - invalid vlan id
 *      RT_ERR_MAC          - invalid mac address
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      Need to initialize L2 entry before add it.
 */
extern int32
dal_mango_l2_addr_init(
    uint32              unit,
    rtk_vlan_t          vid,
    rtk_mac_t           *pMac,
    rtk_l2_ucastAddr_t  *pL2_addr);

/* Function Name:
 *      dal_mango_l2_addr_add
 * Description:
 *      Add L2 entry to ASIC.
 * Input:
 *      unit     - unit id
 *      pL2_addr - L2 entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_VLAN_VID     - invalid vlan id
 *      RT_ERR_MAC          - invalid mac address
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      (1) The API can supported add by port or trunk-id view both.
 *          - If pL2_addr->flags have turn on the RTK_L2_UCAST_FLAG_TRUNK_PORT flag, mean the
 *            pL2_addr->trk_gid is valid and pL2_addr->port is invalid.
 *          - If pL2_addr->flags have turn off the RTK_L2_UCAST_FLAG_TRUNK_PORT flag, mean the
 *            pL2_addr->port is valid and pL2_addr->trk_gid is invalid.
 *      (2) When adding a entry with RTK_L2_UCAST_FLAG_SA_BLOCK or RTK_L2_UCAST_FLAG_SA_SECURE attribute,
 *          pL2_addr->port should be set to 0x3f in 8390 and 9310, and 0x1f in 8380 and 9300 if the specified MAC address
 *          would come from nowhere.
 */
extern int32
dal_mango_l2_addr_add(uint32 unit, rtk_l2_ucastAddr_t *pL2_addr);

/* Function Name:
 *      dal_mango_l2_addr_del
 * Description:
 *      Delete a L2 unicast address entry from the specified device.
 * Input:
 *      unit - unit id
 *      vid  - vlan id
 *      pMac - mac address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID           - invalid unit id
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_VLAN_VID          - invalid vid
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 *      RT_ERR_MAC               - invalid mac address
 *      RT_ERR_L2_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      VID is same as FID in IVL mode.
 */
extern int32
dal_mango_l2_addr_del(uint32 unit, rtk_vlan_t vid, rtk_mac_t *pMac);

/* Function Name:
 *      dal_mango_l2_addr_get
 * Description:
 *      Get L2 entry based on specified vid and MAC address
 * Input:
 *      unit     - unit id
 * Output:
 *      pL2_addr - pointer to L2 entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID           - invalid unit id
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_VLAN_VID          - invalid vlan id
 *      RT_ERR_MAC               - invalid mac address
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 *      RT_ERR_L2_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      (1) VID is same as FID in IVL mode.
 *      (2) The pL2_data.vid and pL2_data.mac is input key
 *      (3) The pL2_data.port, pL2_data.auth, pL2_data.sa_block,
 *          pL2_data.da_block and pL2_data.is_static is output.
 *      (4) If pL2_addr->flags have turn on the RTK_L2_UCAST_FLAG_TRUNK_PORT flag,
 *          mean the pL2_addr->trk_gid is valid and pL2_addr->port is valid also.
 *          The pL2_addr->port value is the represent port of pL2_addr->trk_gid.
 *      (5) If pL2_addr->flags have turn off the RTK_L2_UCAST_FLAG_TRUNK_PORT flag,
 *          mean the pL2_addr->port is valid and pL2_addr->trk_gid is invalid.
 */
extern int32
dal_mango_l2_addr_get(uint32 unit, rtk_l2_ucastAddr_t *pL2_addr);

/* Function Name:
 *      dal_mango_l2_addr_set
 * Description:
 *      Update content of L2 entry.
 * Input:
 *      unit     - unit id
 *      pL2_addr - L2 entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_VLAN_VID     - invalid vlan id
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_MAC          - invalid mac address
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      (1) The API can support adding entry by port or trunk-id view both.
 *          - If pL2_addr->flags have turn on the RTK_L2_UCAST_FLAG_TRUNK_PORT flag, mean the
 *            pL2_addr->trk_gid is valid and pL2_addr->port is invalid.
 *          - If pL2_addr->flags have turn off the RTK_L2_UCAST_FLAG_TRUNK_PORT flag, mean the
 *            pL2_addr->port is valid and pL2_addr->trk_gid is invalid.
 */
extern int32
dal_mango_l2_addr_set(uint32 unit, rtk_l2_ucastAddr_t *pL2_addr);

/* Function Name:
 *      dal_mango_l2_addr_delAll
 * Description:
 *      Delete all L2 unicast address entry from the specified device.
 * Input:
 *      unit           - unit id
 *      include_static - include static mac or not?
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_mango_l2_addr_delAll(uint32 unit, uint32 include_static);

/* Function Name:
 *      dal_mango_l2_nextValidAddr_get
 * Description:
 *      Get next valid L2 unicast address entry from the specified device.
 * Input:
 *      unit           - unit id
 *      pScan_idx      - currently scan index of l2 table to get next.
 *      include_static - the get type, include static mac or not.
 * Output:
 *      pL2_data       - structure of l2 address data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID           - invalid unit id
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 *      RT_ERR_INPUT             - invalid input parameter
 *      RT_ERR_L2_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      (1) The function will skip valid l2 multicast and ip multicast entry and
 *          returned next valid L2 unicast address which based on index order of l2 table.
 *      (2) Input -1 for getting the first entry of l2 table.
 *      (3) The pScan_idx is both the input and output argument.
 */
extern int32
dal_mango_l2_nextValidAddr_get(
    uint32              unit,
    int32               *pScan_idx,
    uint32              include_static,
    rtk_l2_ucastAddr_t  *pL2_data);

/* Function Name:
 *      _dal_mango_l2_nexthop_add
 * Description:
 *      Add L2 nexthop entry to ASIC based on specified VID and MAC address.
 * Input:
 *      unit                        - unit id
 *      dal_mango_l2_ucastNhAddr_t  - nexthop entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID           - invalid unit id
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 *      RT_ERR_INPUT             - invalid input parameter
 * Note:
 *      The function will add a nexthop entry which is used by L3 modules, and if:
 *      (1) pure L2 entry exists:
 *          this function just set the nexthop bit to 1 and leave other fields unchanged.
 *      (2) pure L2 entry doesn't exist:
 *          this function will add an entry with nexthop bit set to 1 and the portId be 0x3f.
 */
extern int32
_dal_mango_l2_nexthop_add(uint32 unit, dal_mango_l2_ucastNhAddr_t *pNexthop_addr);

/* Function Name:
 *      _dal_mango_l2_nexthop_del
 * Description:
 *      Delete L2 nexthop entry from ASIC based on specified VID and MAC address.
 * Input:
 *      unit                        - unit id
 *      dal_mango_l2_ucastNhAddr_t  - nexthop entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID           - invalid unit id
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 *      RT_ERR_INPUT             - invalid input parameter
 * Note:
 *      The function will delete a nexthop entry which is used by L3 modules, and if:
 *      (1) pure L2 entry exists:
 *          this function just clear the nexthop bit to 1 and leave other fields unchanged.
 *      (2) pure L2 entry doesn't exist:
 *          this function will set this entry invalid directly.
 */
extern int32
_dal_mango_l2_nexthop_del(uint32 unit, dal_mango_l2_ucastNhAddr_t *pNexthop_addr);

/* Function Name:
 *      _dal_mango_l2_nexthop_del_byIdx
 * Description:
 *      Delete L2 nexthop entry from ASIC based on index.
 * Input:
 *      unit    - unit id
 *      index   - index to nexthop entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID           - invalid unit id
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 *      RT_ERR_INPUT             - invalid input parameter
 * Note:
 *      The function will delete a nexthop entry which is used by L3 modules, and if:
 *      (1) pure L2 entry exists:
 *          this function just clear the nexthop bit to 1 and leave other fields unchanged.
 *      (2) pure L2 entry doesn't exist:
 *          this function will set this entry invalid directly.
 */
extern int32
_dal_mango_l2_nexthop_del_byIdx(uint32 unit, uint32 index);

/* Module Name    : L2           */
/* Sub-module Name: l2 multicast */

/* Function Name:
 *      dal_mango_l2_mcastAddr_init
 * Description:
 *      Initialize content of buffer of L2 multicast entry.
 *      Will fill vid ,MAC address and reset other field of L2 multicast entry.
 * Input:
 *      unit        - unit id
 *      vid         - vlan id
 *      pMac        - MAC address
 *      pMcast_addr - L2 multicast entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_VLAN_VID     - invalid vlan id
 *      RT_ERR_MAC          - invalid mac address
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      Need to initialize L2 multicast entry before add it.
 */
extern int32
dal_mango_l2_mcastAddr_init(uint32 unit, rtk_vlan_t vid, rtk_mac_t *pMac, rtk_l2_mcastAddr_t *pMcast_addr);

/* Function Name:
 *      dal_mango_l2_mcastAddr_add
 * Description:
 *      Add L2 multicast entry to ASIC.
 * Input:
 *      unit        - unit id
 *      pMcast_addr - L2 multicast entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_VLAN_VID     - invalid vlan id
 *      RT_ERR_MAC          - invalid mac address
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      (1) Don't need to configure pMcast_addr->fwdIndex because driver automatically allocates a
 *          free portmask entry index and return it back to pMcast_addr->fwdIndex.
 *      (2) pMcast_addr->portmask is used to configure the allocated portmask entry.
 */
extern int32
dal_mango_l2_mcastAddr_add(uint32 unit, rtk_l2_mcastAddr_t *pMcast_addr);

/* Function Name:
 *      dal_mango_l2_mcastAddr_del
 * Description:
 *      Delete a L2 multicast address entry from the specified device.
 * Input:
 *      unit - unit id
 *      vid  - vlan id
 *      pMac - multicast mac address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_VLAN_VID       - invalid vlan id
 *      RT_ERR_NULL_POINTER   - input parameter may be null pointer
 *      RT_ERR_MAC            - invalid mac address
 *      RT_ERR_L2_HASH_KEY    - invalid L2 Hash key
 *      RT_ERR_L2_EMPTY_ENTRY - the entry is empty(invalid)
 * Note:
 *      The corresponding portmask entry is cleared only if its reference count reaches 0.
 */
extern int32
dal_mango_l2_mcastAddr_del(uint32 unit, rtk_vlan_t vid, rtk_mac_t *pMac);

/* Function Name:
 *      dal_mango_l2_mcastAddr_get
 * Description:
 *      Get L2 multicast entry based on specified VID and MAC address.
 * Input:
 *      unit        - unit id
 *      pMcast_addr - L2 multicast entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_VLAN_VID     - invalid vlan id
 *      RT_ERR_MAC          - invalid mac address
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_mango_l2_mcastAddr_get(uint32 unit, rtk_l2_mcastAddr_t *pMcast_addr);

/* Function Name:
 *      dal_mango_l2_mcastAddr_set
 * Description:
 *      Update content of L2 multicast entry.
 * Input:
 *      unit        - unit id
 * Output:
 *      pMcast_addr - pointer to L2 multicast entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_VLAN_VID     - invalid vlan id
 *      RT_ERR_MAC          - invalid mac address
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_mango_l2_mcastAddr_set(uint32 unit, rtk_l2_mcastAddr_t *pMcast_addr);

/* Function Name:
 *      dal_mango_l2_mcastAddr_addByIndex
 * Description:
 *      Add L2 multicast entry to ASIC with specific forward portmask index.
 * Input:
 *      unit        - unit id
 *      pMcast_addr - L2 multicast entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_VLAN_VID     - invalid vlan id
 *      RT_ERR_MAC          - invalid mac address
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      (1) Specific pMcast_addr->fwdIndex.
 *      (2) pMcast_addr->portmask is used to configure the allocated portmask entry.
 */
extern int32
dal_mango_l2_mcastAddr_addByIndex(uint32 unit, rtk_l2_mcastAddr_t *pMcast_addr);

/* Function Name:
 *      dal_mango_l2_nextValidMcastAddr_get
 * Description:
 *      Get next valid L2 multicast address entry from the specified device.
 * Input:
 *      unit      - unit id
 *      pScan_idx - currently scan index of l2 table to get next.
 * Output:
 *      pL2_data  - structure of l2 address data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID           - invalid unit id
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 *      RT_ERR_L2_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      (1) The function will skip valid l2 unicast and ip multicast entry and
 *          returned next valid L2 multicast address which based on index order of l2 table.
 *      (2) Input -1 for getting the first entry of l2 table.
 *      (3) The pScan_idx is both the input and output argument.
 */
extern int32
dal_mango_l2_nextValidMcastAddr_get(
    uint32              unit,
    int32               *pScan_idx,
    rtk_l2_mcastAddr_t  *pL2_data);

/* Function Name:
 *      _dal_mango_l2_mcastNexthop_add
 * Description:
 *      Add L2 nexthop entry to ASIC based on specified VID and MAC address.
 * Input:
 *      unit                        - unit id
 *      dal_mango_l2_mcastNhAddr_t  - nexthop entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID           - invalid unit id
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 *      RT_ERR_INPUT             - invalid input parameter
 * Note:
 *      The function will add a nexthop entry which is used by L3 modules, and if:
 *      (1) pure L2 entry exists:
 *          this function just set the nexthop bit to 1 and leave other fields unchanged.
 *      (2) pure L2 entry doesn't exist:
 *          this function will return RT_ERR_L2_ENTRY_NOTFOUND.
 */
extern int32
_dal_mango_l2_mcastNexthop_add(uint32 unit, dal_mango_l2_mcastNhAddr_t *pNexthop_addr);

/* Function Name:
 *      _dal_mango_l2_mcastNexthop_del
 * Description:
 *      Delete L2 multicast nexthop entry from ASIC based on specified VID and MAC address.
 * Input:
 *      unit                        - unit id
 *      dal_mango_l2_mcastNhAddr_t  - nexthop entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID           - invalid unit id
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 *      RT_ERR_INPUT             - invalid input parameter
 * Note:
 *      The function will delete a nexthop entry which is used by L3 modules, and if:
 *      (1) pure L2 entry exists:
 *          this function just clear the nexthop bit to 1 and leave other fields unchanged.
 *      (2) pure L2 entry doesn't exist:
 *          this function will return RT_ERR_L2_ENTRY_NOTFOUND.
 */
extern int32
_dal_mango_l2_mcastNexthop_del(uint32 unit, dal_mango_l2_mcastNhAddr_t *pNexthop_addr);

/* Module Name    : L2                         */
/* Sub-module Name: Multicast forwarding table */

/* Function Name:
 *      dal_mango_l2_mcastFwdIndex_alloc
 * Description:
 *      Allocate index for multicast forwarding entry
 * Input:
 *      unit      - unit id
 *      pFwdIndex - pointer to index of multicast forwarding entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID                  - invalid unit id
 *      RT_ERR_NOT_INIT                 - The module is not initial
 *      RT_ERR_NULL_POINTER             - input parameter may be null pointer
 *      RT_ERR_L2_MULTI_FWD_INDEX       - invalid index of multicast forwarding entry
 *      RT_ERR_L2_MCAST_FWD_ENTRY_EXIST - Mcast forwarding entry already exist
 *      RT_ERR_L2_INDEXTBL_FULL         - L2 index table is full
 * Note:
 *      (1) If pFwdIndex is larger than or equal to 0, will use pFwdIndex as multicast index.
 *      (2) If pFwdIndex is smaller than 0, will allocate a free index and return it.
 *      (3) The reference count corresponds to the pFwdIndex is increased after a successfully allocation.
 */
extern int32
dal_mango_l2_mcastFwdIndex_alloc(
    uint32          unit,
    int32           *pFwdIndex);

/* Function Name:
 *      dal_mango_l2_mcastFwdIndex_free
 * Description:
 *      Free index for multicast forwarding entry
 * Input:
 *      unit  - unit id
 *      index - index of multicast forwarding entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID                      - invalid unit id
 *      RT_ERR_NOT_INIT                     - The module is not initial
 *      RT_ERR_L2_MULTI_FWD_INDEX           - invalid index of multicast forwarding portmask
 *      RT_ERR_L2_MCAST_FWD_ENTRY_NOT_EXIST - index of forwarding entry is not exist
 * Note:
 *      (1) The valid range of indx is 0 ~ (multicast forwarding table size - 1)
 *      (2) The reference count corresponds to the pFwdIndex is decreased after a successfully free.
 */
extern int32
dal_mango_l2_mcastFwdIndex_free(
    uint32          unit,
    int32           index);

/* Function Name:
 *      dal_mango_l2_mcastFwdPortmaskEntry_get
 * Description:
 *      Get portmask of multicast forwarding entry
 * Input:
 *      unit       - unit id
 *      index      - index of multicast forwarding portmask
 * Output:
 *      pPortmask  - pointer buffer of multicast ports
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID            - invalid unit id
 *      RT_ERR_NOT_INIT           - The module is not initial
 *      RT_ERR_NULL_POINTER       - input parameter may be null pointer
 *      RT_ERR_L2_MULTI_FWD_INDEX - invalid index of multicast forwarding portmask
 * Note:
 *      The valid range of indx is 0 ~ (multicast forwarding table size - 1)
 */
extern int32
dal_mango_l2_mcastFwdPortmaskEntry_get(
    uint32          unit,
    int32           index,
    rtk_portmask_t  *pPortmask);

/* Function Name:
 *      dal_mango_l2_mcastFwdPortmaskEntry_set
 * Description:
 *      Set portmask of multicast forwarding entry
 * Input:
 *      unit      - unit id
 *      index     - index of multicast forwarding portmask
 *      pPortmask - pointer buffer of multicast ports
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID            - invalid unit id
 *      RT_ERR_NOT_INIT           - The module is not initial
 *      RT_ERR_NULL_POINTER       - input parameter may be null pointer
 *      RT_ERR_L2_MULTI_FWD_INDEX - invalid index of multicast forwarding portmask
 * Note:
 *      The valid range of indx is 0 ~ (multicast forwarding table size - 1)
 */
extern int32
dal_mango_l2_mcastFwdPortmaskEntry_set(
    uint32          unit,
    int32           index,
    rtk_portmask_t  *pPortmask);

/* Module Name    : L2              */
/* Sub-module Name: CPU MAC address */

/* Function Name:
 *      dal_mango_l2_cpuMacAddr_add
 * Description:
 *      Add a CPU mac address entry to the lookup table.
 * Input:
 *      unit - unit id
 *      vid  - vlan id
 *      pMac - cpu mac address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_VLAN_VID     - invalid vid
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The packet destined to the CPU MAC is then forwarded to CPU port.
 */
extern int32
dal_mango_l2_cpuMacAddr_add(uint32 unit, rtk_vlan_t vid, rtk_mac_t *pMac);


/* Function Name:
 *      dal_mango_l2_cpuMacAddr_del
 * Description:
 *      Delete a CPU mac address entry from the lookup table.
 * Input:
 *      unit - unit id
 *      vid  - vlan id
 *      pMac - cpu mac address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_VLAN_VID     - invalid vid
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_mango_l2_cpuMacAddr_del(uint32 unit, rtk_vlan_t vid, rtk_mac_t *pMac);

/* Module Name    : L2        */
/* Sub-module Name: Port move */
/* Function Name:
 *      dal_mango_l2_portMoveAction_get
 * Description:
 *      Get forwarding action when port moving is detected.
 * Input:
 *      unit       - unit id
 *      type      - port move type
 *      pAction  - pointer to portmove parameter
 * Output:
 *      pAction - pointer to forwarding action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_mango_l2_portMoveAction_get(
    uint32              unit,
    rtk_l2_portMoveType_t type,
    rtk_l2_portMoveAct_t        *pAction);

/* Function Name:
 *      dal_mango_l2_portMoveAction_set
 * Description:
 *      Set forwarding action when port moving is detected.
 * Input:
 *      unit       - unit id
 *      type      - port move type
 *      pAction  - pointer to portmove parameter and action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID    - invalid unit id
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_PORT_ID    - invalid port id
 *      RT_ERR_FWD_ACTION - invalid forwarding action
 * Note:
 *      None
 */
extern int32
dal_mango_l2_portMoveAction_set(
    uint32              unit,
    rtk_l2_portMoveType_t type,
    rtk_l2_portMoveAct_t        *pAction);

/* Function Name:
 *      dal_mango_l2_portMoveLearn_get
 * Description:
 *      Get learning action when port moving is detected.
 * Input:
 *      unit       - unit id
 *      type      - port move type
 *      pLearn  - pointer to portmove parameter
 * Output:
 *      pLearn - pointer to learning action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_mango_l2_portMoveLearn_get(
    uint32              unit,
    rtk_l2_portMoveType_t type,
    rtk_l2_portMoveLrn_t        *pLearn);

/* Function Name:
 *      dal_mango_l2_portMoveLearn_set
 * Description:
 *      Set learning action when port moving is detected.
 * Input:
 *      unit       - unit id
 *      type      - port move type
 *      pLearn  - pointer to portmove parameter and learning action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID    - invalid unit id
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_PORT_ID    - invalid port id
 *      RT_ERR_FWD_ACTION - invalid forwarding action
 * Note:
 *      None
 */
extern int32
dal_mango_l2_portMoveLearn_set(
    uint32              unit,
    rtk_l2_portMoveType_t type,
    rtk_l2_portMoveLrn_t        *pLearn);

/* Module Name    : L2                        */
/* Sub-module Name: Parameter for lookup miss */

/* Function Name:
 *      dal_mango_l2_lookupMissFloodPortMask_get
 * Description:
 *      Get flooding port mask which limits the lookup missed flooding domain.
 * Input:
 *      unit            - unit id
 *      type            - type of lookup miss
 * Output:
 *      pFlood_portmask - flooding port mask configuration when unicast/multicast lookup missed.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_L2_PMSK_NOT_INIT - flooding portmask is not initialized
 *      RT_ERR_INPUT            - invalid input parameter
 * Note:
 *      (1) In 8390 and 8380, must invoke rtk_l2_lookupMissFloodPortMask_setByIndex() first.
 *      (2) In 8390, 8380, 9300 and 9310 only DLF_TYPE_UCAST and DLF_TYPE_BCAST are supported. For DLF_TYPE_MCAST,
 *          DLF_TYPE_IPMC and DLF_TYPE_IP6MC, refer to rtk_vlan_profile_set()/rtk_vlan_profile_get().
 */
extern int32
dal_mango_l2_lookupMissFloodPortMask_get(uint32 unit, rtk_l2_lookupMissType_t type, rtk_portmask_t *pFlood_portmask);

/* Function Name:
 *      dal_mango_l2_lookupMissFloodPortMask_set
 * Description:
 *      Set flooding port mask when unicast or multicast address lookup missed in L2 table.
 * Input:
 *      unit            - unit id
 *      type            - type of lookup miss
 *      pFlood_portmask - flooding port mask configuration when unicast/multicast lookup missed.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) In 9300 and 9310, only DLF_TYPE_UCAST and DLF_TYPE_BCAST are supported. For DLF_TYPE_MCAST,
 *          DLF_TYPE_IPMC and DLF_TYPE_IP6MC, refer to rtk_vlan_profile_set()/rtk_vlan_profile_get().
 */
extern int32
dal_mango_l2_lookupMissFloodPortMask_set(uint32 unit, rtk_l2_lookupMissType_t type, rtk_portmask_t *pFlood_portmask);

/* Function Name:
 *      dal_mango_l2_lookupMissFloodPortMask_add
 * Description:
 *      Add one port member to the lookup missed flooding port mask.
 * Input:
 *      unit       - unit id
 *      type       - type of lookup miss
 *      flood_port - flooding port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_L2_PMSK_NOT_INIT - flooding portmask is not initialized
 * Note:
 *      (1) In 8390 and 8380, must invoke rtk_l2_lookupMissFloodPortMask_setByIndex() first.
 *      (2) In 8390, 8380, 9300 and 9310 only DLF_TYPE_UCAST and DLF_TYPE_BCAST are supported. For DLF_TYPE_MCAST,
 *          DLF_TYPE_IPMC and DLF_TYPE_IP6MC, refer to rtk_vlan_profile_set()/rtk_vlan_profile_get().
 */
extern int32
dal_mango_l2_lookupMissFloodPortMask_add(uint32 unit, rtk_l2_lookupMissType_t type, rtk_port_t flood_port);

/* Function Name:
 *      dal_mango_l2_lookupMissFloodPortMask_del
 * Description:
 *      Delete one port member from the lookup missed flooding port mask.
 * Input:
 *      unit       - unit id
 *      type       - type of lookup miss
 *      flood_port - flooding port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID          - invalid unit id
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_L2_PMSK_NOT_INIT - flooding portmask is not initialized
 * Note:
 *      (1) In 8390 and 8380, must invoke rtk_l2_lookupMissFloodPortMask_setByIndex() first.
 *      (2) In 8390, 8380, 9300 and 9310 only DLF_TYPE_UCAST and DLF_TYPE_BCAST are supported. For DLF_TYPE_MCAST,
 *          DLF_TYPE_IPMC and DLF_TYPE_IP6MC, refer to rtk_vlan_profile_set()/rtk_vlan_profile_get().
 */
extern int32
dal_mango_l2_lookupMissFloodPortMask_del(uint32 unit, rtk_l2_lookupMissType_t type, rtk_port_t flood_port);

/* Function Name:
 *      dal_mango_l2_portLookupMissAction_get
 * Description:
 *      Get forwarding action of specified port when destination address lookup miss.
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      type    - type of lookup miss
 * Output:
 *      pAction - pointer to forwarding action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_INPUT        - invalid type of lookup miss
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Type of lookup missis as following:
 *      - DLF_TYPE_IPMC
 *      - DLF_TYPE_IP6MC
 *      - DLF_TYPE_UCAST (9300 and 9310 only support DLF_TYPE_UCAST)
 *      - DLF_TYPE_MCAST
 *
 *      Forwarding action is as following:
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_COPY2CPU
 *      - ACTION_TRAP2MASTERCPU
 *      - ACTION_COPY2MASTERCPU
 */
extern int32
dal_mango_l2_portLookupMissAction_get(uint32 unit, rtk_port_t port, rtk_l2_lookupMissType_t type, rtk_action_t *pAction);

/* Function Name:
 *      dal_mango_l2_portLookupMissAction_set
 * Description:
 *      Set forwarding action of specified port when destination address lookup miss.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      type   - type of lookup miss
 *      action - forwarding action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID    - invalid unit id
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_PORT_ID    - invalid port id
 *      RT_ERR_INPUT      - invalid type of lookup miss
 *      RT_ERR_FWD_ACTION - invalid forwarding action
 * Note:
 *      Type of lookup missis as following:
 *      - DLF_TYPE_IPMC
 *      - DLF_TYPE_IP6MC
 *      - DLF_TYPE_UCAST (9300 and 9310 only support DLF_TYPE_UCAST)
 *      - DLF_TYPE_MCAST
 *
 *      Forwarding action is as following:
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_COPY2CPU
 *      - ACTION_TRAP2MASTERCPU
 *      - ACTION_COPY2MASTERCPU
 */
extern int32
dal_mango_l2_portLookupMissAction_set(uint32 unit, rtk_port_t port, rtk_l2_lookupMissType_t type, rtk_action_t action);

/* Function Name:
 *      dal_mango_l2_portUcastLookupMissAction_get
 * Description:
 *      Get forwarding action of specified port when unicast destination address lookup miss.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pAction - pointer to forwarding action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Forwarding action is as following:
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_COPY2CPU
 *      - ACTION_TRAP2MASTERCPU
 *      - ACTION_COPY2MASTERCPU
 */
extern int32
dal_mango_l2_portUcastLookupMissAction_get(uint32 unit, rtk_port_t port, rtk_action_t *pAction);

/* Function Name:
 *      dal_mango_l2_portUcastLookupMissAction_set
 * Description:
 *      Set forwarding action of specified port when unicast destination address lookup miss.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      action - forwarding action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID    - invalid unit id
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_PORT_ID    - invalid port id
 *      RT_ERR_FWD_ACTION - invalid forwarding action
 * Note:
 *      Forwarding action is as following:
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_COPY2CPU
 *      - ACTION_TRAP2MASTERCPU
 *      - ACTION_COPY2MASTERCPU
 */
extern int32
dal_mango_l2_portUcastLookupMissAction_set(uint32 unit, rtk_port_t port, rtk_action_t action);

/* Module Name    : L2                 */
/* Sub-module Name: Parameter for MISC */

/* Function Name:
 *      dal_mango_l2_srcPortEgrFilterMask_get
 * Description:
 *      Get loopback filtering function on specified ports.
 * Input:
 *      unit             - unit id
 * Output:
 *      pFilter_portmask - ports which turn on loopback filtering function
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The loopback(egress port == ingress port) packet is dropped if the function is enabled.
 */
extern int32
dal_mango_l2_srcPortEgrFilterMask_get(uint32 unit, rtk_portmask_t *pFilter_portmask);

/* Function Name:
 *      dal_mango_l2_srcPortEgrFilterMask_set
 * Description:
 *      Set loopback filtering function on specified ports.
 * Input:
 *      unit             - unit id
 *      pFilter_portmask - ports which turn on loopback filtering function
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The loopback(egress port == ingress port) packet is dropped if the function is enabled.
 */
extern int32
dal_mango_l2_srcPortEgrFilterMask_set(uint32 unit, rtk_portmask_t *pFilter_portmask);

/* Function Name:
 *      dal_mango_l2_srcPortEgrFilterMask_add
 * Description:
 *      Enable the loopback filtering function on specified port.
 * Input:
 *      unit        - unit id
 *      filter_port - ports which turn on loopback filtering function
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 * Note:
 *      The loopback(egress port == ingress port) packet is dropped if the function is enabled.
 */
extern int32
dal_mango_l2_srcPortEgrFilterMask_add(uint32 unit, rtk_port_t filter_port);

/* Function Name:
 *      dal_mango_l2_srcPortEgrFilterMask_del
 * Description:
 *      Disable the loopback filtering function on specified port.
 * Input:
 *      unit        - unit id
 *      filter_port - ports which turn off loopback filtering function
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 * Note:
 *      The loopback(egress port == ingress port) packet is dropped if the function is enabled.
 */
extern int32
dal_mango_l2_srcPortEgrFilterMask_del(uint32 unit, rtk_port_t filter_port);

/*
 * MISC
 */

/* Function Name:
 *      dal_mango_l2_exceptionAddrAction_get
 * Description:
 *      Get forwarding action of packet with exception source MAC address.
 * Input:
 *      unit       - unit id
 *      exceptType - type of exception address
 * Output:
 *      pAction    - pointer to forward action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID             - invalid unit id
 *      RT_ERR_NOT_INIT            - The module is not initial
 *      RT_ERR_L2_EXCEPT_ADDR_TYPE - invalid exception address type
 *      RT_ERR_NULL_POINTER        - input parameter may be null pointer
 * Note:
 *      For 8390, 8380, 9300 and 9310, Exception address type is as following
 *      - SA_IS_BCAST_OR_MCAST
 *      - SA_IS_ZERO
 */
extern int32
dal_mango_l2_exceptionAddrAction_get(
    uint32                          unit,
    rtk_l2_exceptionAddrType_t      exceptType,
    rtk_action_t                    *pAction);

/* Function Name:
 *      dal_mango_l2_exceptionAddrAction_set
 * Description:
 *      Set forwarding action of packet with exception source MAC address.
 * Input:
 *      unit       - unit id
 *      exceptType - type of exception address
 *      action     - forward action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID             - invalid unit id
 *      RT_ERR_NOT_INIT            - The module is not initial
 *      RT_ERR_L2_EXCEPT_ADDR_TYPE - invalid exception address type
 *      RT_ERR_INPUT               - invalid input parameter
 * Note:
 *      For 8390, 8380, 9300 and 9310, Exception address type is as following
 *      - SA_IS_BCAST_OR_MCAST
 *      - SA_IS_ZERO
 */
extern int32
dal_mango_l2_exceptionAddrAction_set(
    uint32                          unit,
    rtk_l2_exceptionAddrType_t      exceptType,
    rtk_action_t                    action);

/* Function Name:
 *      dal_mango_l2_addrEntry_get
 * Description:
 *      Get the L2 table entry by index of the specified unit.
 * Input:
 *      unit      - unit id
 *      index     - l2 table index
 * Output:
 *      pL2_entry - pointer buffer of l2 table entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) The index valid range is from 0 to (L2 hash table size - 1)
 *          - 0 ~ (L2 hash table size - 1) entry in L2 hash table
 *      (2) The output entry have 2 variables (valid and entry_type) and its detail data structure
 *          - valid: 1 mean the entry is valid; 0: invalid
 *          - entry_type: FLOW_TYPE_UNICAST, FLOW_TYPE_L2_MULTI and FLOW_TYPE_IP_MULTI
 *                        the field is ignored if valid field is 0.
 *          - detail data structure is ignored if valid is 0, and its filed meanings is depended
 *            on the entry_type value.
 *      (3) If pL2_entry->flags have enabled the RTK_L2_UCAST_FLAG_TRUNK_PORT flag, mean the
 *          pL2_entry->unicast.trk_gid is valid trunk id value.
 */
extern int32
dal_mango_l2_addrEntry_get(uint32 unit, uint32 index, rtk_l2_entry_t *pL2_entry);

/* Function Name:
 *      dal_mango_l2_conflictAddr_get
 * Description:
 *      Get the conflict L2 table entry from one given L2 address in the specified unit.
 * Input:
 *      unit            - unit id
 *      pL2Addr         - l2 address to find its conflict entries
 *      cfAddrList_size - buffer size of the pCfAddrList
 * Output:
 *      pCfAddrList     - pointer buffer of the conflict l2 table entry list
 *      pCf_retCnt      - return number of find conflict l2 table entry list
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) The function can be used if add l2 entry return RT_ERR_L2_NO_EMPTY_ENTRY.
 *          Input the pL2Addr->entry_type and its hash key to get conflict entry information.
 *      (2) User want to prepare the return buffer pCfAddrList and via. cfAddrList_size argument
 *          tell driver its size.
 *      (3) The function will return valid L2 hash entry from the same bucket and the return number
 *          is filled in pCf_retCnt, entry data is filled in pCfAddrList.
 */
extern int32
dal_mango_l2_conflictAddr_get(
    uint32          unit,
    rtk_l2_entry_t  *pL2Addr,
    rtk_l2_entry_t  *pCfAddrList,
    uint32          cfAddrList_size,
    uint32          *pCf_retCnt);

/* Function Name:
 *      dal_mango_l2_zeroSALearningEnable_get
 * Description:
 *      Get enable status of all-zero-SA learning.
 * Input:
 *      unit    - unit id
 * Output:
 *      pEnable - pointer to enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_mango_l2_zeroSALearningEnable_get(uint32 unit, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_mango_l2_zeroSALearningEnable_set
 * Description:
 *      Set enable status of all-zero-SA learning.
 * Input:
 *      unit   - unit id
 *      enable - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_mango_l2_zeroSALearningEnable_set(uint32 unit, rtk_enable_t enable);

/* Function Name:
 *      dal_mango_l2_portDynamicPortMoveForbidEnable_get
 * Description:
 *      Get the port moveforbiddance configuration of the specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) Dynamic address entry port move in/out forbiddance enabled port is not allowed and the entry
 *          is not updated.
 *      (2) Regarding the forwrding action, refer to rtk_l2_dynamicPortMoveForbidAction_set.
 */
extern int32
dal_mango_l2_portDynamicPortMoveForbidEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_mango_l2_portDynamicPortMoveForbidEnable_set
 * Description:
 *      Set the port move forbiddance configuration of the specified port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      (1) Dynamic address entry port move in/out forbiddance enabled port is not allowed and the entry
 *          is not updated.
 *      (2) Regarding the forwrding action, refer to rtk_l2_dynamicPortMoveForbidAction_set.
 */
extern int32
dal_mango_l2_portDynamicPortMoveForbidEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      dal_mango_l2_trkDynamicPortMoveForbidEnable_get
 * Description:
 *      Get the port moveforbiddance configuration of the specified trunk.
 * Input:
 *      unit    - unit id
 *      tid     - trunk id
 * Output:
 *      pEnable - enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) Dynamic address entry port move in/out forbiddance enabled trunk is not allowed and the entry
 *          is not updated.
 *      (2) Regarding the forwrding action, refer to rtk_l2_dynamicPortMoveForbidAction_set.
 */
extern int32
dal_mango_l2_trkDynamicPortMoveForbidEnable_get(uint32 unit, rtk_trk_t tid, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_mango_l2_trkDynamicPortMoveForbidEnable_set
 * Description:
 *      Set the port move forbiddance configuration of the specified trunk.
 * Input:
 *      unit    - unit id
 *      tid     - trunk id
 *      enable  - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      (1) Dynamic address entry port move in/out forbiddance enabled trunk is not allowed and the entry
 *          is not updated.
 *      (2) Regarding the forwrding action, refer to rtk_l2_dynamicPortMoveForbidAction_set.
 */
extern int32
dal_mango_l2_trkDynamicPortMoveForbidEnable_set(uint32 unit, rtk_trk_t tid, rtk_enable_t enable);

/* Function Name:
 *      dal_mango_l2_portMacFilterEnable_get
 * Description:
 *      Get the mac filter configuration of the specified port.
 * Input:
 *      unit        - unit id
 *      port        - port id
 *      filterMode  - filter DA or SA
 * Output:
 *      pEnable     - enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_mango_l2_portMacFilterEnable_get(uint32 unit, rtk_port_t port, rtk_l2_macFilterMode_t filterMode, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_mango_l2_portMacFilterEnable_set
 * Description:
 *      Set the mac filter configuration of the specified port.
 * Input:
 *      unit        - unit id
 *      port        - port id
 *      filterMode  - filter DA or SA
 *      enable      - drop procedence assignment status for trapped packet
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_mango_l2_portMacFilterEnable_set(uint32 unit, rtk_port_t port, rtk_l2_macFilterMode_t filterMode, rtk_enable_t enable);

/* Function Name:
 *      dal_mango_l2_hwNextValidAddr_get
 * Description:
 *      get next valid entry with specific method.
 * Input:
 *      unit        - unit id
 *      pScan_idx   - the index which starting search from
 *      rtk_l2_nextValidType_t  - search Method
 * Output:
 *      pScan_idx           - the next valid entry index
 *      rtk_l2_entry_t      - the next valid entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_mango_l2_hwNextValidAddr_get(
    uint32              unit,
    int32               *pScan_idx,
    rtk_l2_nextValidType_t type,
    rtk_l2_entry_t  *pEntry);

/* Function Name:
 *      dal_mango_l2_portCtrl_get
 * Description:
 *      Get the configuration of the specified control type and port
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      type    - control type
 * Output:
 *      pArg    - pointer to the argurment
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 */
extern int32
dal_mango_l2_portCtrl_get(uint32 unit, rtk_port_t port, rtk_l2_portCtrlType_t type, int32 *pArg);


/* Function Name:
 *      dal_mango_l2_portCtrl_set
 * Description:
 *      Set the configuration of the specified control type and port
 * Input:
 *      unit    - unit id
 *      port    - port id
 *      type    - control type
 *      arg     - argurment
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 */
extern int32
dal_mango_l2_portCtrl_set(uint32 unit, rtk_port_t port, rtk_l2_portCtrlType_t type, int32 arg);

/* Function Name:
 *      dal_mango_l2_status_get
 * Description:
 *      Get the status or counter of the specified feature
 * Input:
 *      unit    - unit id
 *      type    - status type
 *      index   - index
 * Output:
 *      pArg    - pointer to the argurment
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      "index" may be only valid in some rtk_l2_stsType_t types.
 */
extern int32
dal_mango_l2_status_get(uint32 unit, rtk_l2_stsType_t type, uint32 index, uint32 *pArg);

/* Function Name:
 *      dal_mango_l2_stkLearningEnable_get
 * Description:
 *      Get the enable status of stacking system auto-learning mode.
 * Input:
 *      unit    - unit id
 * Output:
 *      pEnable - pointer to the enable status of stacking system auto-learning mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      When it is enabled, switch will learn the MAC and source ingress port of packet which received from stacking port.
 */
extern int32
dal_mango_l2_stkLearningEnable_get(uint32 unit, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_mango_l2_stkLearningEnable_set
 * Description:
 *      Set the enable status of stacking system auto-learning mode.
 * Input:
 *      unit    - unit id
 *      enable  - enable status of stacking system auto-learning mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 * Note:
 *      When it is enabled, switch will learn the MAC and source ingress port of packet which received from stacking port.
 */
extern int32
dal_mango_l2_stkLearningEnable_set(uint32 unit, rtk_enable_t enable);

/* Function Name:
 *      dal_mango_l2_stkKeepUcastEntryValid_get
 * Description:
 *      Get the setting about entry validity after aging period.
 * Input:
 *      unit    - unit id
 * Output:
 *      pEnable - enable status of aging out
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_mango_l2_stkKeepUcastEntryValid_get(uint32 unit, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_mango_l2_stkKeepUcastEntryValid_set
 * Description:
 *      Set the entry validity after aging period.
 * Input:
 *      unit    - unit id
 *      enable  - enable status of stacking system auto-learning mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      When it is enabled, entry will keep valid after its aging period.
 */
extern int32
dal_mango_l2_stkKeepUcastEntryValid_set(uint32 unit, rtk_enable_t enable);

/* Function Name:
 *      dal_mango_l2_entryCnt_get
 * Description:
 *      Get l2 entry counter
 * Input:
 *      unit    - unit id
 *      type    - l2 entry type
 * Output:
 *      pCnt    - pointer to the entry counter
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 *      RT_ERR_INPUT        - invalid input
 * Note:
 */
extern int32
dal_mango_l2_entryCnt_get(uint32 unit, rtk_l2_entryType_t type, uint32 *pCnt);

/* Function Name:
 *      dal_mango_l2_hashIdx_get
 * Description:
 *      Get L2 hash index based on specified vid and MAC address
 * Input:
 *      unit        - unit id
 *      pMacHashIdx - pointer to vid and mac
 * Output:
 *      pMacHashIdx - pointer to hash indexes
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID           - invalid unit id
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_VLAN_VID          - invalid vlan id
 *      RT_ERR_MAC               - invalid mac address
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 * Note:
 *      (1) VID is same as FID in IVL mode.
 *      (2) The pMacHashIdx.vid and pMacHashIdx.mac is input key
 *      (3) validBlk and validAlgo fields in pMacHashIdx specify how many blocks and hash-algo number
 *          the target unit supports.
 */
extern int32
dal_mango_l2_hashIdx_get(uint32 unit, rtk_l2_macHashIdx_t *pMacHashIdx);

/* Function Name:
 *      dal_mango_l2_addr_delByMac
 * Description:
 *      Delete all L2 entries with specific MAC
 * Input:
 *      unit            - unit id
 *      include_static  - include static mac or not
 *      pMac            - mac address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID           - invalid unit id
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 *      RT_ERR_INPUT             - invalid input
 * Note:
 */
extern int32
dal_mango_l2_addr_delByMac(uint32 unit, uint32 include_static, rtk_mac_t *pMac);

#endif /* __DAL_MANGO_L2_H__ */

