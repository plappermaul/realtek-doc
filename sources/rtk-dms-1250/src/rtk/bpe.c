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

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <dal/dal_mgmt.h>
#include <rtk/default.h>
#include <rtk/bpe.h>

/*
 * Symbol Definition
 */

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
int32
rtk_bpe_init(uint32 unit)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->bpe_init(unit);
}   /* end of rtk_bpe_init */

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
int32
rtk_bpe_portFwdMode_get(uint32 unit, rtk_port_t port,
    rtk_bpe_fwdMode_t *pMode)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->bpe_portFwdMode_get(unit, port, pMode);
}   /* end of rtk_bpe_portFwdMode_get */

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
int32
rtk_bpe_portFwdMode_set(uint32 unit, rtk_port_t port, rtk_bpe_fwdMode_t mode)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->bpe_portFwdMode_set(unit, port, mode);
}   /* end of rtk_bpe_portFwdMode_set */

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
int32
rtk_bpe_portEcidNameSpaceGroupId_get(uint32 unit, rtk_port_t port,
    uint32 *pGroupId)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->bpe_portEcidNameSpaceGroupId_get(unit, port, pGroupId);
}   /* end of rtk_bpe_portEcidNameSpaceGroupId_get */

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
int32
rtk_bpe_portEcidNameSpaceGroupId_set(uint32 unit, rtk_port_t port,
    uint32 groupId)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->bpe_portEcidNameSpaceGroupId_set(unit, port, groupId);
}   /* end of rtk_bpe_portEcidNameSpaceGroupId_set */

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
int32
rtk_bpe_portPcid_get(uint32 unit, rtk_port_t port,
    rtk_bpe_pcidCfgType_t type, rtk_bpe_ecid_t *pPcid)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->bpe_portPcid_get(unit, port, type, pPcid);
}   /* end of rtk_bpe_portPcid_get */

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
int32
rtk_bpe_portPcid_set(uint32 unit, rtk_port_t port,
    rtk_bpe_pcidCfgType_t type, rtk_bpe_ecid_t pcid)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->bpe_portPcid_set(unit, port, type, pcid);
}   /* end of rtk_bpe_portPcid_set */


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
int32
rtk_bpe_portPcidAct_get(uint32 unit, rtk_port_t port, rtk_action_t *pAction)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->bpe_portPcidAct_get(unit, port, pAction);
}   /* end of rtk_bpe_portPcidAct_get */

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
int32
rtk_bpe_portPcidAct_set(uint32 unit, rtk_port_t port, rtk_action_t action)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->bpe_portPcidAct_set(unit, port, action);
}   /* end of rtk_bpe_portPcidAct_set */

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
int32
rtk_bpe_portEgrTagSts_get(uint32 unit, rtk_port_t port,
    rtk_bpe_tagSts_t *pStatus)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->bpe_portEgrTagSts_get(unit, port, pStatus);
}   /* end of rtk_bpe_portEgrTagSts_get */

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
int32
rtk_bpe_portEgrTagSts_set(uint32 unit, rtk_port_t port,
    rtk_bpe_tagSts_t status)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->bpe_portEgrTagSts_set(unit, port, status);
}   /* end of rtk_bpe_portEgrTagSts_set */


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
int32
rtk_bpe_portEgrVlanTagSts_get(uint32 unit, rtk_port_t port,
    rtk_bpe_vlanTagSts_t *pVlan_status)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->bpe_portEgrVlanTagSts_get(unit, port, pVlan_status);
}   /* end of rtk_bpe_portEgrVlanTagSts_get */

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
int32
rtk_bpe_portEgrVlanTagSts_set(uint32 unit, rtk_port_t port,
    rtk_bpe_vlanTagSts_t vlan_status)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->bpe_portEgrVlanTagSts_set(unit, port, vlan_status);
}   /* end of rtk_bpe_portEgrVlanTagSts_set */

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
int32
rtk_bpe_pvidEntry_add(uint32 unit, rtk_bpe_pvidEntry_t *pEntry)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->bpe_pvidEntry_add(unit, pEntry);
}   /* end of rtk_bpe_pvidEntry_add */

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
int32
rtk_bpe_pvidEntry_del(uint32 unit, rtk_bpe_pvidEntry_t *pEntry)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->bpe_pvidEntry_del(unit, pEntry);
}   /* end of rtk_bpe_pvidEntry_del */

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
int32
rtk_bpe_pvidEntry_get(uint32 unit, rtk_bpe_pvidEntry_t *pEntry)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->bpe_pvidEntry_get(unit, pEntry);
}   /* end of rtk_bpe_pvidEntry_get */

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
int32
rtk_bpe_pvidEntryNextValid_get(uint32 unit, int32 *pScan_idx,
    rtk_bpe_pvidEntry_t *pEntry)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->bpe_pvidEntryNextValid_get(unit, pScan_idx, pEntry);
}   /* end of rtk_bpe_pvidEntryNextValid_get */

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
int32
rtk_bpe_fwdEntry_add(uint32 unit, rtk_bpe_fwdEntry_t *pEntry)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->bpe_fwdEntry_add(unit, pEntry);
}   /* end of rtk_bpe_fwdEntry_add */

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
int32
rtk_bpe_fwdEntry_del(uint32 unit, rtk_bpe_fwdEntry_t *pEntry)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->bpe_fwdEntry_del(unit, pEntry);
}   /* end of rtk_bpe_fwdEntry_del */

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
int32
rtk_bpe_fwdEntry_get(uint32 unit, rtk_bpe_fwdEntry_t *pEntry)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->bpe_fwdEntry_get(unit, pEntry);
}   /* end of rtk_bpe_fwdEntry_get */

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
int32
rtk_bpe_fwdEntryNextValid_get(uint32 unit, int32 *pScan_idx,
    rtk_bpe_fwdEntry_t *pEntry)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->bpe_fwdEntryNextValid_get(unit, pScan_idx, pEntry);
}   /* end of rtk_bpe_fwdEntryNextValid_get */

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
int32
rtk_bpe_globalCtrl_get(uint32 unit, rtk_bpe_globalCtrlType_t type,
    int32 *pArg)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->bpe_globalCtrl_get(unit, type, pArg);
}   /* end of rtk_bpe_globalCtrl_get */

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
int32
rtk_bpe_globalCtrl_set(uint32 unit, rtk_bpe_globalCtrlType_t type, int32 arg)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->bpe_globalCtrl_set(unit, type, arg);
}   /* end of rtk_bpe_globalCtrl_set */

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
int32
rtk_bpe_portCtrl_get(uint32 unit, rtk_port_t port,
    rtk_bpe_portCtrlType_t type, int32 *pArg)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->bpe_portCtrl_get(unit, port, type, pArg);
}   /* end of rtk_bpe_portCtrl_get */

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
int32
rtk_bpe_portCtrl_set(uint32 unit, rtk_port_t port,
    rtk_bpe_portCtrlType_t type, int32 arg)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->bpe_portCtrl_set(unit, port, type, arg);
}   /* end of rtk_bpe_portCtrl_set */

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
int32
rtk_bpe_priRemarking_get(uint32 unit, rtk_bpe_priRmkSrc_t src,
    rtk_bpe_priRmkVal_t val, rtk_pri_t *pPri)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->bpe_priRemarking_get(unit, src, val, pPri);
}   /* end of rtk_bpe_priRemarking_get */

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
int32
rtk_bpe_priRemarking_set(uint32 unit, rtk_bpe_priRmkSrc_t src,
    rtk_bpe_priRmkVal_t val, rtk_pri_t pri)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->bpe_priRemarking_set(unit, src, val, pri);
}   /* end of rtk_bpe_priRemarking_set */


