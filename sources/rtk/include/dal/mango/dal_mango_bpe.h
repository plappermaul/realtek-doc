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
 * Purpose : Definition those public vlan APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) BPE
 *
 */
#ifndef __DAL_MANGO_BPE_H__
#define __DAL_MANGO_BPE_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <rtk/bpe.h>
#include <dal/dal_mapper.h>

/*
 * Symbol Definition
 */
#define BPE_PMSKLIST_INVALID                    (0) /* reserved index number for invalid entry*/
#define BPE_PVID_HASH_BITS                      (3)
#define BPE_FWD_HASH_BITS                       (2)


typedef enum dal_mango_bpe_indexType_e
{
    BPE_IN_HASH = 0,
    BPE_IN_CAM,
    BPE_INDEXTYPE_END
} dal_mango_bpe_indexType_t;

typedef enum dal_mango_bpe_getMethod_e
{
    BPE_GET_EXIST_ONLY = 0,
    BPE_GET_EXIST_OR_FREE,
    BPE_GET_FREE_ONLY,
    BPE_GETMETHOD_END
} dal_mango_bpe_getMethod_t;

typedef enum dal_mango_bpe_fwdEntryType_e
{
    BPE_FWD_TYPE_L2 = 0,
    BPE_FWD_TYPE_PE,
    BPE_FWDENTRYTYPE_END
} dal_mango_bpe_fwdEntryType_t;

typedef struct dal_mango_bpe_pvid_entry_s
{
    uint32      is_entry_exist;
    uint32      is_entry_valid;

    uint32      valid;
    uint32      ecid;
    uint32      vid;
    uint32      nsg;

    uint32      is_trk;
    uint32      port_id;
} dal_mango_bpe_pvid_entry_t;

typedef struct dal_mango_bpe_pvid_index_s
{
    dal_mango_bpe_indexType_t  index_type;       /* In CAM or In HASH */
    uint32  index;
    uint32  hashdepth;      /* only useful when entry is in hash table */
} dal_mango_bpe_pvid_index_t;

typedef struct dal_mango_bpe_fwd_hashIdx_s
{
    uint32 blk0_hashIdx;
    uint32 blk1_hashIdx;
} dal_mango_bpe_fwd_hashIdx_t;

typedef struct dal_mango_bpe_fwd_entry_s
{
    uint32      is_entry_exist;
    uint32      is_entry_valid;

    uint32      valid;
    uint32      format;
    uint32      entry_type;
    uint32      ecid;
    uint32      nsg;

    rtk_portmask_t portmask;
} dal_mango_bpe_fwd_entry_t;

typedef struct dal_mango_bpe_fwd_index_s
{
    dal_mango_bpe_indexType_t  index_type;       /* In CAM or In HASH */
    uint32  index;
    uint32  hashdepth;      /* only useful when entry is in hash table */
} dal_mango_bpe_fwd_index_t;


typedef enum dal_mango_bpe_ecidPmskPortType_e
{
    BPE_PMSK_INDEX = 0,
    BPE_OFFSET_PMSK,
    BPE_PORTTYPE_END
} al_mango_bpe_ecidPmskPortType_t;


typedef struct dal_mango_bpe_ecidPmsk_entry_s {
    uint32  curr_index; /* [NEW MCAST] remove it */
    uint32  next_index; /* [NEW MCAST] remove it */
    uint32  ecid;       /* [NEW MCAST] remove it */
    uint8   port_type;              /* 0: portmask index, 1: port offset */
    union {
        struct portoffset_entry_s
        {
            uint8      offset;      /* represents offset of pmsk*/
            uint8      pmsk;        /* partial portmask, shift by offset, only 8 bits */
        } port_offset;
        struct portmask_entry_s
        {
            uint32      pmsk_index;
        } portmask;
    };
} dal_mango_bpe_ecidPmsk_entry_t;

typedef struct dal_mango_bpe_pmskList_s {
    dal_mango_bpe_ecidPmsk_entry_t *pPmskList_entry;
    uint16                      free_index_head;
    uint16                      free_entry_count;
} dal_mango_bpe_pmskList_t;

/*
 * Data Declaration
 */

/*
 * Macro Definition
 */


/*
 * Function Declaration
 */
/* Function Name:
 *      dal_mango_bpeMapper_init
 * Description:
 *      Hook bpe module of the specified device.
 * Input:
 *      pMapper - pointer of mapper
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      Must Hook bpe module before calling any bpe APIs.
 */
extern int32
dal_mango_bpeMapper_init(dal_mapper_t *pMapper);


/* Module Name : BPE */

/* Function Name:
 *      dal_mango_bpe_init
 * Description:
 *      Initialize BPE module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize BPE module before calling any BPE APIs.
 */
extern int32
dal_mango_bpe_init(uint32 unit);

/* Function Name:
 *      dal_mango_bpe_portFwdMode_get
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
 *      RT_ERR_NOT_INIT - the module is not initial
 * Note:
 *      None
 */
extern int32
dal_mango_bpe_portFwdMode_get(uint32 unit, rtk_port_t port,
    rtk_bpe_fwdMode_t *pMode);

/* Function Name:
 *      dal_mango_bpe_portFwdMode_set
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
 *      RT_ERR_NOT_INIT - the module is not initial
 * Note:
 *      None
 */
extern int32
dal_mango_bpe_portFwdMode_set(uint32 unit, rtk_port_t port, rtk_bpe_fwdMode_t mode);

/* Function Name:
 *      dal_mango_bpe_portEcidNameSpaceGroupId_get
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
 *      RT_ERR_NOT_INIT - the module is not initial
 * Note:
 *      None
 */
extern int32
dal_mango_bpe_portEcidNameSpaceGroupId_get(uint32 unit, rtk_port_t port,
    uint32 *pGroupId);

/* Function Name:
 *      dal_mango_bpe_portEcidNameSpaceGroupId_set
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
 *      RT_ERR_NOT_INIT - the module is not initial
 * Note:
 *      None
 */
extern int32
dal_mango_bpe_portEcidNameSpaceGroupId_set(uint32 unit, rtk_port_t port,
    uint32 groupId);

/* Function Name:
 *      dal_mango_bpe_portPcid_get
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
 *      RT_ERR_NOT_INIT - the module is not initial
 * Note:
 *      None
 */
extern int32
dal_mango_bpe_portPcid_get(uint32 unit, rtk_port_t port,
    rtk_bpe_pcidCfgType_t type, rtk_bpe_ecid_t *pPcid);

/* Function Name:
 *      dal_mango_bpe_portPcid_set
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
 *      RT_ERR_NOT_INIT - the module is not initial
 * Note:
 *      None
 */
extern int32
dal_mango_bpe_portPcid_set(uint32 unit, rtk_port_t port,
    rtk_bpe_pcidCfgType_t type, rtk_bpe_ecid_t pcid);


/* Function Name:
 *      dal_mango_bpe_portPcidAct_get
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
 *      RT_ERR_NOT_INIT - the module is not initial
 * Note:
 *      None
 */
extern int32
dal_mango_bpe_portPcidAct_get(uint32 unit, rtk_port_t port, rtk_action_t *pAction);

/* Function Name:
 *      dal_mango_bpe_portPcidAct_set
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
 *      RT_ERR_NOT_INIT - the module is not initial
 * Note:
 *      None
 */
extern int32
dal_mango_bpe_portPcidAct_set(uint32 unit, rtk_port_t port, rtk_action_t action);

/* Function Name:
 *      dal_mango_bpe_portEgrTagSts_get
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
 *      RT_ERR_NOT_INIT - the module is not initial
 * Note:
 *      None
 */
extern int32
dal_mango_bpe_portEgrTagSts_get(uint32 unit, rtk_port_t port,
    rtk_bpe_tagSts_t *pStatus);

/* Function Name:
 *      dal_mango_bpe_portEgrTagSts_set
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
 *      RT_ERR_NOT_INIT - the module is not initial
 * Note:
 *      None
 */
extern int32
dal_mango_bpe_portEgrTagSts_set(uint32 unit, rtk_port_t port,
    rtk_bpe_tagSts_t status);

/* Function Name:
 *      dal_mango_bpe_portEgrVlanTagSts_get
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
 *      RT_ERR_NOT_INIT - the module is not initial
 * Note:
 *      None
 */
extern int32
dal_mango_bpe_portEgrVlanTagSts_get(uint32 unit, rtk_port_t port,
    rtk_bpe_vlanTagSts_t *pVlan_status);

/* Function Name:
 *      dal_mango_bpe_portEgrVlanTagSts_set
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
 *      RT_ERR_NOT_INIT - the module is not initial
 * Note:
 *      None
 */
extern int32
dal_mango_bpe_portEgrVlanTagSts_set(uint32 unit, rtk_port_t port,
    rtk_bpe_vlanTagSts_t vlan_status);

/* Function Name:
 *      dal_mango_bpe_pvidEntry_add
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
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_mango_bpe_pvidEntry_add(uint32 unit, rtk_bpe_pvidEntry_t *pEntry);

/* Function Name:
 *      dal_mango_bpe_pvidEntry_del
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
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_mango_bpe_pvidEntry_del(uint32 unit, rtk_bpe_pvidEntry_t *pEntry);

/* Function Name:
 *      dal_mango_bpe_pvidEntry_get
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
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_mango_bpe_pvidEntry_get(uint32 unit, rtk_bpe_pvidEntry_t *pEntry);

/* Function Name:
 *      dal_mango_bpe_pvidEntryNextValid_get
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
 *      RT_ERR_NOT_INIT         - the module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_ENTRY_NOT_EXIST  - entry is not existed
 * Note:
 *      None
 */
extern int32
dal_mango_bpe_pvidEntryNextValid_get(uint32 unit, int32 *pScan_idx,
    rtk_bpe_pvidEntry_t *pEntry);

/* Function Name:
 *      dal_mango_bpe_fwdEntry_add
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
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_mango_bpe_fwdEntry_add(uint32 unit, rtk_bpe_fwdEntry_t *pEntry);

/* Function Name:
 *      dal_mango_bpe_fwdEntry_del
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
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_mango_bpe_fwdEntry_del(uint32 unit, rtk_bpe_fwdEntry_t *pEntry);

/* Function Name:
 *      dal_mango_bpe_fwdEntry_get
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
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_mango_bpe_fwdEntry_get(uint32 unit, rtk_bpe_fwdEntry_t *pEntry);

/* Function Name:
 *      dal_mango_bpe_fwdEntryNextValid_get
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
 *      RT_ERR_NOT_INIT         - the module is not initial
 *      RT_ERR_INPUT            - invalid input parameter
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 *      RT_ERR_ENTRY_NOT_EXIST  - entry is not existed
 * Note:
 *      None
 */
extern int32
dal_mango_bpe_fwdEntryNextValid_get(uint32 unit, int32 *pScan_idx,
    rtk_bpe_fwdEntry_t *pEntry);

/* Function Name:
 *      dal_mango_bpe_globalCtrl_get
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
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Note:
 *      None
 */
extern int32
dal_mango_bpe_globalCtrl_get(uint32 unit, rtk_bpe_globalCtrlType_t type,
    int32 *pArg);

/* Function Name:
 *      dal_mango_bpe_globalCtrl_set
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
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Note:
 *      None
 */
extern int32
dal_mango_bpe_globalCtrl_set(uint32 unit, rtk_bpe_globalCtrlType_t type, int32 arg);

/* Function Name:
 *      dal_mango_bpe_portCtrl_get
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
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      None
 */
extern int32
dal_mango_bpe_portCtrl_get(uint32 unit, rtk_port_t port,
    rtk_bpe_portCtrlType_t type, int32 *pArg);

/* Function Name:
 *      dal_mango_bpe_portCtrl_set
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
 *      RT_ERR_NOT_INIT - the module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 * Note:
 *      None
 */
extern int32
dal_mango_bpe_portCtrl_set(uint32 unit, rtk_port_t port,
    rtk_bpe_portCtrlType_t type, int32 arg);

/* Function Name:
 *      dal_mango_bpe_priRemarking_get
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
 *      RT_ERR_NOT_INIT         - the module is not initial
 *      RT_ERR_QOS_INT_PRIORITY - invalid priority
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      Supported remarking source is as following:
 *      - rtk_bpe_priRmkSrc_t \ Chip:           9310
 *      - PETAG_PRI_RMK_SRC_INT_PRI             O
 */
extern int32
dal_mango_bpe_priRemarking_get(uint32 unit, rtk_bpe_priRmkSrc_t src,
    rtk_bpe_priRmkVal_t val, rtk_pri_t *pPri);

/* Function Name:
 *      dal_mango_bpe_priRemarking_set
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
 *      RT_ERR_NOT_INIT         - the module is not initial
 *      RT_ERR_QOS_INT_PRIORITY - invalid priority
 * Note:
 *      Supported remarking source is as following:
 *      - rtk_bpe_priRmkSrc_t \ Chip:           9310
 *      - PETAG_PRI_RMK_SRC_INT_PRI             O
 */
extern int32
dal_mango_bpe_priRemarking_set(uint32 unit, rtk_bpe_priRmkSrc_t src,
    rtk_bpe_priRmkVal_t val, rtk_pri_t pri);

/* Function Name:
 *      dal_mango_bpe_ecidPmskEntry_add
 * Description:
 *      Add an ECID_PMSK entry on the specified device.
 * Input:
 *      unit   - unit id
 *      index  - index of ECID_PMSK_LIST table
 * Output:
 *      pEntry - pointer to the ECID_PMSK_LIST table
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      DAL internal usage
 */
extern int32
dal_mango_bpe_ecidPmskEntry_add(uint32 unit, uint32 index, dal_mango_bpe_ecidPmsk_entry_t *pEntry);

/* Function Name:
 *      dal_mango_bpe_ecidPmskEntry_get
 * Description:
 *      Delete an ECID_PMSK entry on the specified device.
 * Input:
 *      unit   - unit id
 *      index  - index of ECID_PMSK_LIST table
 * Output:
 *      pEntry - pointer to the ECID_PMSK_LIST table
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      DAL internal usage
 */
extern int32
dal_mango_bpe_ecidPmskEntry_del(uint32 unit, uint32 index, dal_mango_bpe_ecidPmsk_entry_t *pEntry);

/* Function Name:
 *      dal_mango_bpe_ecidPmskEntry_get
 * Description:
 *      Get an ECID_PMSK entry on the specified device.
 * Input:
 *      unit   - unit id
 *      index  - index of ECID_PMSK_LIST table
 * Output:
 *      pEntry - pointer to the ECID_PMSK_LIST table
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      DAL internal usage
 */
extern int32
dal_mango_bpe_ecidPmskEntry_get(uint32 unit, uint32 index, dal_mango_bpe_ecidPmsk_entry_t *pEntry);

/* Function Name:
 *      dal_mango_bpe_ecidPmskEntry_set
 * Description:
 *      Set an ECID_PMSK entry on the specified device.
 * Input:
 *      unit   - unit id
 *      index  - index of ECID_PMSK_LIST table
 *      pEntry - pointer to the ECID_PMSK_LIST table
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      DAL internal usage
 */
extern int32
dal_mango_bpe_ecidPmskEntry_set(uint32 unit, uint32 index, dal_mango_bpe_ecidPmsk_entry_t *pEntry);


/* Function Name:
 *      dal_mango_bpe_ecidPmskEntryNextValid_get
 * Description:
 *      Get a next valid ECID_PMSK entry on the specified device.
 * Input:
 *      unit     - unit id
 *      index    - current index of ECID_PMSK_LIST table to get next
 * Output:
 *      pEntry   - pointer to the ECID_PMSK_LIST table
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      DAL internal usage
 */
int32 dal_mango_bpe_ecidPmskEntryNextValid_get(uint32 unit, uint32 index,
    dal_mango_bpe_ecidPmsk_entry_t *pEntry);



#endif /* __DAL_MANGO_BPE_H__ */

