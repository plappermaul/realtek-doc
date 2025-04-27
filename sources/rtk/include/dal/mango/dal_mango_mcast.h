/*
 * Copyright (C) 2009-2016 Realtek Semiconductor Corp.
 * All rights reserved.
 *
 * $Revision$
 * $Date$
 *
 * Purpose : Definition those public multicast APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *            1) Multicast group
 *
 */
#ifndef __DAL_MANGO_MCAST_H__
#define __DAL_MANGO_MCAST_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/l3.h>
#include <rtk/ipmcast.h>
#include <dal/dal_mapper.h>
#include <dal/mango/dal_mango_l2.h>

/*
 * Symbol Definition
 */
#define DAL_MANGO_MCAST_OIL_MAX                     (16384)
#define DAL_MANGO_MCAST_GROUP_MAX                   (6144)
#define DAL_MANGO_MCAST_FWD_IDX_MAX                 (4096)
#define DAL_MANGO_MCAST_L2TUNNEL_NODE_MAX           (8192)
#define DAL_MANGO_MCAST_L3_NODE_MAX                 (6144)      /* for IPMC entry */

#define DAL_MANGO_MCAST_INVALID_FWD_IDX             (-1)
#define DAL_MANGO_MCAST_INVALID_LST_IDX             (-1)
#define DAL_MANGO_MCAST_INVALID_OIL_IDX             (-1)
#define DAL_MANGO_MCAST_OIL_END                     (DAL_MANGO_MCAST_INVALID_OIL_IDX)

/* Group ID */
#define DAL_MANGO_MCAST_GROUP_ID(_type, _index)         ((((_type) & 0xFF) << 24) | ((_index) & 0x00FFFFFF))
#define DAL_MANGO_MCAST_GROUP_ID_TYPE(_groupId)         (((_groupId) & 0xFF000000) >> 24)
#define DAL_MANGO_MCAST_GROUP_ID_IDX(_groupId)          ((_groupId) & 0x00FFFFFF)

/* VLAN node */
typedef struct dal_mango_mcast_vlanNode_s
{
    rtk_vlan_t  vid;    /* VLAN id */

    /* node of link list */
    RTK_LIST_NODE_REF_DEF(dal_mango_mcast_vlanNode_s, nodeRef);
} dal_mango_mcast_vlanNode_t;

/* L2MC node */
typedef struct dal_mango_mcast_l2Node_s
{
    dal_mango_l2_index_t    l2Idx;  /* L2 entry index */

    /* node of link list */
    RTK_LIST_NODE_REF_DEF(dal_mango_mcast_l2Node_s, nodeRef);
} dal_mango_mcast_l2Node_t;

/* L3MC node */
typedef struct dal_mango_mcast_l3Node_s
{
    uint32  ipmc_entryIdx;  /* IPMC entry index */
    uint32  ipmc_flags;     /* IPMC flags  */

    /* node of link list */
    RTK_LIST_NODE_REF_DEF(dal_mango_mcast_l3Node_s, nodeRef);
} dal_mango_mcast_l3Node_t;

/* VLAN entry info */
typedef struct dal_mango_mcast_vlanInfo_s
{
    uint32  tunnel_lst_valid;
    uint32  tunnel_lst_idx;
} dal_mango_mcast_vlanInfo_t;


/* L2MC entry info */
typedef struct dal_mango_mcast_l2Info_s
{
    int32   fwdIndex;
    uint32  l2_tnl_lst_valid;
    uint32  l2_tnl_lst_idx;
} dal_mango_mcast_l2Info_t;


/*
 * Data Declaration
 */

/*
 * Macro Declaration
 */

/*
 * Function Declaration
 */

/* Function Name:
 *      dal_mango_mcastMapper_init
 * Description:
 *      Hook mcast module of the specified device.
 * Input:
 *      pMapper - pointer of mapper
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      Must Hook mcast module before calling any mcast APIs.
 */
extern int32
dal_mango_mcastMapper_init(dal_mapper_t *pMapper);

/* Function Name:
 *      dal_mango_mcast_init
 * Description:
 *      Initialize multicast module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      Must initialize multicast module before calling any mcast APIs.
 */
extern int32
dal_mango_mcast_init(uint32 unit);

/* Function Name:
 *      _dal_mango_mcast_sem_lock
 * Description:
 *      lock multicast APIs' semaphore
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      Must initialize mcast module before calling any mcast APIs.
 *      This is an API called by sub-modules (IPMCAST).
 */
extern int32
_dal_mango_mcast_sem_lock(uint32 unit);

/* Function Name:
 *      _dal_mango_mcast_sem_unlock
 * Description:
 *      unlock multicast APIs' semaphore
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      Must initialize mcast module before calling any mcast APIs.
 *      This is an API called by sub-modules (IPMCAST).
 */
extern int32
_dal_mango_mcast_sem_unlock(uint32 unit);

/* Function Name:
 *      _dal_mango_mcast_fwdIdx_get
 * Description:
 *      Get the group l2 forward table index and oil start index
 * Input:
 *      unit      - unit id
 *      groupId   - mcast group ID
 * Output:
 *      pL2fwdIdx - pointer of the l2 forward index
 *      pL2LstIdx - pointer of the l2 MC index
 *      pOilIdx   - pointer of the l3 oil index
 *      pStkPmsk  - pointer of the l3 stack forward portmask
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      Must initialize mcast module before calling any mcast APIs.
 *      if not found, or invalid. *pL2fwdIdx = -1, *pL2LstIdx = -1, *pOilIdx = -1
 */
extern int32
_dal_mango_mcast_fwdIdx_get(uint32 unit, rtk_mcast_group_t groupId, int32 *pL2fwdIdx, int32 *pL2LstIdx, int32 *pOilIdx, uint32 *pStkPmsk);

#if 0
/* Function Name:
 *      _dal_mango_mcast_ipmc_bindState_get
 * Description:
 *      get the binding state of the group to ipmc entry
 * Input:
 *      unit      - unit id
 *      groupId   - mcast group
 *      pBind     - binding state
 *      pEntryIdx - ipmc entry index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 */
extern int32
_dal_mango_mcast_ipmc_bindState_get(uint32 unit, rtk_mcast_group_t groupId, rtk_enable_t *pBind, uint32 *pEntryIdx);
#endif

/* Function Name:
 *      _dal_mango_mcast_ipmc_bind
 * Description:
 *      Bind the group to an IPMC entry
 * Input:
 *      unit      - unit id
 *      groupId   - mcast group ID
 *      pIpmcAddr - pointer to the ipmc addr
 *      entryIdx  - ipmc entry index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 */
extern int32
_dal_mango_mcast_ipmc_bind(uint32 unit, rtk_mcast_group_t groupId, rtk_ipmc_addr_t *pIpmcAddr, uint32 entryIdx);

/* Function Name:
 *      _dal_mango_mcast_ipmc_unbind
 * Description:
 *      Unbind the group from an IPMC entry
 * Input:
 *      unit     - unit id
 *      groupId  - mcast group
 *      entryIdx - pointer to the ipmc addr table index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 */
extern int32
_dal_mango_mcast_ipmc_unbind(uint32 unit, rtk_mcast_group_t groupId, uint32 entryIdx);

/* Function Name:
 *      _dal_mango_mcast_vlan_bind
 * Description:
 *      Bind the group to an VLAN entry
 * Input:
 *      unit      - unit id
 *      groupId   - mcast group
 *      pNode     - pointer to the VLAN link-list node
 * Output:
 *      pVlanInfo - pointer to the VLAN info
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 */
extern int32
_dal_mango_mcast_vlan_bind(uint32 unit, rtk_mcast_group_t groupId, dal_mango_mcast_vlanNode_t *pNode, dal_mango_mcast_vlanInfo_t *pVlanInfo);

/* Function Name:
 *      _dal_mango_mcast_vlan_unbind
 * Description:
 *      Unbind the group from an VLAN entry
 * Input:
 *      unit    - unit id
 *      groupId - mcast group ID
 *      pNode   - pointer to the VLAN link-list node
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 */
extern int32
_dal_mango_mcast_vlan_unbind(uint32 unit, rtk_mcast_group_t groupId, dal_mango_mcast_vlanNode_t *pNode);

/* Function Name:
 *      _dal_mango_mcast_l2mc_bind
 * Description:
 *      Bind the group to an L2MC entry
 * Input:
 *      unit    - unit id
 *      groupId - mcast group
 *      pNode   - pointer to the L2 link-list node
 * Output:
 *      pL2Info - pointer to the L2 information
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 */
extern int32
_dal_mango_mcast_l2mc_bind(uint32 unit, rtk_mcast_group_t groupId, dal_mango_mcast_l2Node_t *pNode, dal_mango_mcast_l2Info_t *pL2Info);

/* Function Name:
 *      _dal_mango_mcast_l2mc_unbind
 * Description:
 *      Unbind the group from an L2MC entry
 * Input:
 *      unit    - unit id
 *      groupId - mcast group ID
 *      pNode   - pointer to the L2 link-list node
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 */
extern int32
_dal_mango_mcast_l2mc_unbind(uint32 unit, rtk_mcast_group_t groupId, dal_mango_mcast_l2Node_t *pNode);

/* Function Name:
 *      dal_mango_mcast_group_create
 * Description:
 *      Create a multicast group
 * Input:
 *      unit     - unit id
 *      flags    - RTK_MCAST_FLAG_WITH_ID
 *      type     - multicast group type
 * Output:
 *      pGroupId - pointer to multicast group ID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID            - invalid unit id
 *      RT_ERR_NOT_INIT           - The module is not initial
 *      RT_ERR_NULL_POINTER       - input parameter may be a null pointer
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 *
 */
extern int32
dal_mango_mcast_group_create(uint32 unit, uint32 flags, rtk_mcast_type_t type, rtk_mcast_group_t *pGroup);

/* Function Name:
 *      dal_mango_mcast_group_destroy
 * Description:
 *      Destrory the multicast group
 * Input:
 *      unit    - unit id
 *      groupId - multicast group ID
 * Output:
 *
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID            - invalid unit id
 *      RT_ERR_NOT_INIT           - The module is not initial
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 *      RT_ERR_ENTRY_NOTFOUND     - specified entry not found
 *      RT_ERR_NOT_ALLOWED        - action not allowed to destroy
 * Note:
 */
extern int32
dal_mango_mcast_group_destroy(uint32 unit, rtk_mcast_group_t groupId);

/* Function Name:
 *      dal_mango_mcast_group_getNext
 * Description:
 *      Get next multicast group
 * Input:
 *      unit     - unit id
 *      type     - mcast type
 *      pBase    - start index
 * Output:
 *      pGroupId - pointer to multicast group ID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Note:
 *      (1) use type = 0 to indicate include all type
 *      (2) use pBase = -1 to indicate get first valid group
 */
extern int32
dal_mango_mcast_group_getNext(uint32 unit, rtk_mcast_type_t type, int32 *pBase, rtk_mcast_group_t *pGroupId);

/* Function Name:
 *      dal_mango_mcast_egrIf_get
 * Description:
 *      Get next hop information of the multicast group
 * Input:
 *      unit         - unit id
 *      groupId      - multicast group ID
 *      egrIf_max    - number of rtk_mcast_egrif_t entry in pEgrIf buffer
 *      pEgrIf       - buffer pointer
 * Output:
 *      pEgrIf_count - returned rtk_mcast_egrif_t entry number
 *      pEgrIf       - pointer to the next hop information
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_NULL_POINTER   - input parameter may be a null pointer
 *      RT_ERR_INPUT          - invalid input parameter
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 *      RT_ERR_OUT_OF_RANGE   - partition value is out of range
 * Note:
 *      (1) should input enough buffer of pEgrIf
 */
extern int32
dal_mango_mcast_egrIf_get(uint32 unit, rtk_mcast_group_t groupId, int32 egrIf_max, rtk_mcast_egrif_t *pEgrIf, int32 *pEgrIf_count);

/* Function Name:
 *      dal_mango_mcast_egrIf_add
 * Description:
 *      Get next hop information of the multicast group
 * Input:
 *      unit    - unit id
 *      groupId - multicast group ID
 *      pEgrIf  - buffer pointer of mcast nh
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID            - invalid unit id
 *      RT_ERR_NOT_INIT           - The module is not initial
 *      RT_ERR_NULL_POINTER       - input parameter may be a null pointer
 *      RT_ERR_INPUT              - invalid input parameter
 *      RT_ERR_ENTRY_NOTFOUND     - specified entry not found
 *      RT_ERR_OUT_OF_RANGE       - partition value is out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 * Note:
 */
extern int32
dal_mango_mcast_egrIf_add(uint32 unit, rtk_mcast_group_t groupId, rtk_mcast_egrif_t *pEgrIf);

/* Function Name:
 *      dal_mango_mcast_egrIf_del
 * Description:
 *      Delete a NH from multicast group replication list
 * Input:
 *      unit    - unit id
 *      groupId - multicast group ID
 *      pEgrIf  - pointer to multicast next hop information
 * Output:
 *
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID            - invalid unit id
 *      RT_ERR_NOT_INIT           - The module is not initial
 *      RT_ERR_NULL_POINTER       - input parameter may be a null pointer
 *      RT_ERR_INPUT              - invalid input parameter
 *      RT_ERR_ENTRY_NOTFOUND     - specified entry not found
 *      RT_ERR_OUT_OF_RANGE       - partition value is out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 * Note:
 */
extern int32
dal_mango_mcast_egrIf_del(uint32 unit, rtk_mcast_group_t groupId, rtk_mcast_egrif_t *pEgrIf);

/* Function Name:
 *      rtk_mcast_egrIf_delAll
 * Description:
 *      Delete all NHs from multicast group replication list
 * Input:
 *      unit    - unit id
 *      groupId - multicast group
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_OUT_OF_RANGE - partition value is out of range
 * Note:
 */
extern int32
dal_mango_mcast_egrIf_delAll(uint32 unit, rtk_mcast_group_t groupId);

#endif /* __DAL_MANGO_MCAST_H__ */

