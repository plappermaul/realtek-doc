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


/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <dal/dal_mgmt.h>
#include <rtk/default.h>
#include <rtk/vxlan.h>

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
int32
rtk_vxlan_init(uint32 unit)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->vxlan_init(unit);
}   /* end of rtk_vxlan_init */

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
int32
rtk_vxlan_vni_add(uint32 unit, rtk_vxlan_vniEntry_t *pEntry)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->vxlan_vni_add(unit, pEntry);
}   /* end of rtk_vxlan_vni_add */

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
int32
rtk_vxlan_vni_del(uint32 unit, rtk_vxlan_vniEntry_t *pEntry)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->vxlan_vni_del(unit, pEntry);
}   /* end of rtk_vxlan_vni_del */

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
int32
rtk_vxlan_vni_delAll(uint32 unit)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->vxlan_vni_delAll(unit);
}   /* end of rtk_vxlan_vni_delAll */

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
int32
rtk_vxlan_vni_get(uint32 unit, rtk_vxlan_vniEntry_t *pEntry)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->vxlan_vni_get(unit, pEntry);
}   /* end of rtk_vxlan_vni_get */

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
int32
rtk_vxlan_vni_set(uint32 unit, rtk_vxlan_vniEntry_t *pEntry)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->vxlan_vni_set(unit, pEntry);
}   /* end of rtk_vxlan_vni_set */

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
int32
rtk_vxlan_vni_getNext(uint32 unit, int32 *pBase, rtk_vxlan_vniEntry_t *pEntry)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->vxlan_vni_getNext(unit, pBase, pEntry);
}   /* end of rtk_vxlan_vni_getNext */

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
int32
rtk_vxlan_globalCtrl_get(uint32 unit, rtk_vxlan_globalCtrlType_t type, int32 *pArg)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->vxlan_globalCtrl_get(unit, type, pArg);
}   /* end of rtk_vxlan_globalCtrl_get */

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
int32
rtk_vxlan_globalCtrl_set(uint32 unit, rtk_vxlan_globalCtrlType_t type, int32 arg)
{
    /* function body */
    RT_PARAM_CHK((unit > RTK_MAX_UNIT_ID) || (NULL == RT_MGMT(unit)) || (NULL == RT_MAPPER(unit)), RT_ERR_UNIT_ID);

    return RT_MAPPER(unit)->vxlan_globalCtrl_set(unit, type, arg);
}   /* end of rtk_vxlan_globalCtrl_set */

