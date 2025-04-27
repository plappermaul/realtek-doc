/*
 * Copyright (C) 2009-2017 Realtek Semiconductor Corp.
 * All rights reserved.
 *
 * $Revision:  $
 * $Date:  $
 *
 * Purpose : Definition those public VXLAN tunneling APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *           (1) VXLAN tunneling APIs
 */

#ifndef __RTK_VXLAN_H__
#define __RTK_VXLAN_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <rtk/l3.h>

/*
 * Symbol Definition
 */
#define RTK_VXLAN_VNI_MAX           (0xFFFFFF)

typedef enum rtk_vxlan_ovidCmd_e
{
    RTK_VXLAN_OVID_CMD_UNTAG_PRITAG,    /* assign outer VID to outer untagged and outer priority tagged packet */
    RTK_VXLAN_OVID_CMD_UNTAG,           /* assign outer VID to outer untagged packet */
    RTK_VXLAN_OVID_CMD_ALL,             /* assign outer VID to all packet type */
    RTK_VXLAN_OVID_CMD_END,
} rtk_vxlan_ovidCmd_t;

typedef enum rtk_vxlan_ividCmd_e
{
    RTK_VXLAN_IVID_CMD_UNTAG_PRITAG,    /* assign inner VID to inner untagged and outer priority tagged packet */
    RTK_VXLAN_IVID_CMD_UNTAG,           /* assign inner VID to inner untagged packet */
    RTK_VXLAN_IVID_CMD_ALL,             /* assign inner VID to all packet type */
    RTK_VXLAN_IVID_CMD_END,
} rtk_vxlan_ividCmd_t;

/* VXLAN entry */
typedef struct rtk_vxlan_vniEntry_s
{
    /* match key */
    rtk_intf_id_t       intf_id;    /* interface ID (L2 type/VTEP) */
    uint32              vni;        /* VNI */

    /* options - fields when tunnel decapsulation */
    rtk_vxlan_ovidCmd_t ovid_cmd;
    rtk_vlan_t          ovid;
    rtk_vxlan_ividCmd_t ivid_cmd;
    rtk_vlan_t          ivid;
    rtk_vlanType_t      fwd_vlan;   /* fowarding VLAN */
    rtk_pri_t           int_pri;    /* internal priority for VXLAN tunnel terminated packet */
    uint32              priGrp_idx; /* index to priority selection group. Refer to rtk_qos_priSelGroup_set() */

    /* options - fields when tunnel encapsulation */
    uint32              qosPro_idx; /* index to tunnel QoS profile. Refer to rtk_tunnel_qosProfile_set() */

    uint32              entry_idx;  /* returned physical entry index after a successful read/write */
} rtk_vxlan_vniEntry_t;

/* VXLAN global control type */
typedef enum rtk_vxlan_globalCtrlType_e
{
    RTK_VXLAN_GCT_NONE = 0,                             /* Dummy */

    RTK_VXLAN_GCT_VXLAN_TRAP_TARGET,                    /* rtk_trapTarget_t */
    RTK_VXLAN_GCT_VXLAN_INVALID_HDR_ACT,                /* rtk_action_t */
    RTK_VXLAN_GCT_VXLAN_CTRL_FRAME_ACT,                 /* rtk_action_t */
    RTK_VXLAN_GCT_VXLAN_EXCEPT_FLAGS,                   /* uint32 */
    RTK_VXLAN_GCT_VXLAN_GPE_EXCEPT_FLAGS,               /* uint32 */
    RTK_VXLAN_GCT_VXLAN_EXCEPT_FLAGS_ACT,               /* rtk_action_t */
    RTK_VXLAN_GCT_VXLAN_LU_MIS_ACT,                     /* rtk_action_t */

    RTK_VXLAN_GCT_END                                   /* end of enum */
} rtk_vxlan_globalCtrlType_t;


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
 *      rtk_vxlan_init
 * Description:
 *      Initialize VXLAN module of the specified device.
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
 *      Must initialize VXLAN module before calling any tunneling APIs.
 * Changes:
 *      [SDK_3.1.1]
 *          New added function.
 */
extern int32
rtk_vxlan_init(uint32 unit);

/* Function Name:
 *      rtk_vxlan_vni_add
 * Description:
 *      Add a new VNI entry
 * Input:
 *      unit   - unit id
 *      pEntry - pointer to VNI entry (intf_id and vni as match key)
 * Output:
 *      pEntry - pointer to VNI entry (all info)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 *      RT_ERR_TBL_FULL     - table is full
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.1.1]
 *          New added function.
 */
extern int32
rtk_vxlan_vni_add(uint32 unit, rtk_vxlan_vniEntry_t *pEntry);

/* Function Name:
 *      rtk_vxlan_vni_del
 * Description:
 *      Delete an extisting VNI entry
 * Input:
 *      unit     - unit id
 *      vniEntry - VNI entry (intf_id and vni as match key)
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID        - invalid unit id
 *      RT_ERR_NOT_INIT       - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 *      RT_ERR_ENTRY_NOTFOUND - entry is not found
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.1.1]
 *          New added function.
 */
extern int32
rtk_vxlan_vni_del(uint32 unit, rtk_vxlan_vniEntry_t *pEntry);

/* Function Name:
 *      rtk_vxlan_vni_delAll
 * Description:
 *      Delete all VNI entries
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - the module is not initial
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.1.1]
 *          New added function.
 */
extern int32
rtk_vxlan_vni_delAll(uint32 unit);

/* Function Name:
 *      rtk_vxlan_vni_get
 * Description:
 *      Get a VNI entry by interface id and VNI as key.
 * Input:
 *      unit   - unit id
 *      pEntry - pointer to VNI entry (interface id and VNI as match key)
 * Output:
 *      pEntry - pointer to VNI entry (all info)
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
 *      [SDK_3.1.1]
 *          New added function.
 */
extern int32
rtk_vxlan_vni_get(uint32 unit, rtk_vxlan_vniEntry_t *pEntry);

/* Function Name:
 *      rtk_vxlan_vni_set
 * Description:
 *      Set a VNI entry
 * Input:
 *      unit   - unit id
 *      pEntry - pointer to VNI entry (interface id and VNI as match key)
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID         - invalid unit id
 *      RT_ERR_NOT_INIT        - the module is not initial
 *      RT_ERR_NULL_POINTER    - input parameter may be a null pointer
 *      RT_ERR_ENTRY_NOT_FOUND - table is full
 * Applicable:
 *      9310
 * Note:
 *      None
 * Changes:
 *      [SDK_3.1.1]
 *          New added function.
 */
extern int32
rtk_vxlan_vni_set(uint32 unit, rtk_vxlan_vniEntry_t *pEntry);

/* Function Name:
 *      rtk_vxlan_vni_getNext
 * Description:
 *      Get the next VNI entry
 * Input:
 *      unit   - unit id
 *      pBase  - start index
 * Output:
 *      pEntry - pointer to a VNI entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Applicable:
 *      9310
 * Note:
 *      (1) Use base index -1 to indicate to search from the beginging of VNI entry
 *      (2) If the returned index is -1, means not found the next valid entry
 * Changes:
 *      [SDK_3.1.1]
 *          New added function.
 */
extern int32
rtk_vxlan_vni_getNext(uint32 unit, int32 *pBase, rtk_vxlan_vniEntry_t *pEntry);

/* Function Name:
 *      rtk_vxlan_globalCtrl_get
 * Description:
 *      Get global configuration of a specified control type
 * Input:
 *      unit - unit id
 *      type - control type
 * Output:
 *      pArg - pointer to the argurment
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
 *      [SDK_3.1.1]
 *          New added function.
 */
extern int32
rtk_vxlan_globalCtrl_get(uint32 unit, rtk_vxlan_globalCtrlType_t type, int32 *pArg);

/* Function Name:
 *      rtk_vxlan_globalCtrl_set
 * Description:
 *      Set global configuration of a specified control type
 * Input:
 *      unit - unit id
 *      type - control type
 *      arg  - argurment
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
 *      [SDK_3.1.1]
 *          New added function.
 */
extern int32
rtk_vxlan_globalCtrl_set(uint32 unit, rtk_vxlan_globalCtrlType_t type, int32 arg);

#endif /* __RTK_VXLAN_H__ */

