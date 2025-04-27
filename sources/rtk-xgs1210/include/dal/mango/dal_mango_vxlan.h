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

#ifndef __DAL_MANGO_VXLAN_H__
#define __DAL_MANGO_VXLAN_H__

/*
 * Include Files
 */
#include <dal/dal_mapper.h>

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
 *      dal_mango_vxlanMapper_init
 * Description:
 *      Hook VXLAN module of the specified device.
 * Input:
 *      pMapper - pointer of mapper
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      Must Hook VXLAN module before calling any VXLAN APIs.
 */
extern int32
dal_mango_vxlanMapper_init(dal_mapper_t *pMapper);

/* Function Name:
 *      dal_mango_vxlan_init
 * Description:
 *      Initialize VXLAN module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize VXLAN module before calling any tunneling APIs.
 */
extern int32
dal_mango_vxlan_init(uint32 unit);

/* Function Name:
 *      dal_mango_vxlan_vni_add
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
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 *      RT_ERR_TBL_FULL     - table is full
 * Note:
 *      None
 */
extern int32
dal_mango_vxlan_vni_add(uint32 unit, rtk_vxlan_vniEntry_t *pEntry);

/* Function Name:
 *      dal_mango_vxlan_vni_del
 * Description:
 *      Delete an extisting VNI entry
 * Input:
 *      unit   - unit id
 *      pEntry - pointer to VNI entry (intf_id and vni as match key)
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT       - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 *      RT_ERR_ENTRY_NOTFOUND - entry is not found
 * Note:
 *      None
 */
extern int32
dal_mango_vxlan_vni_del(uint32 unit, rtk_vxlan_vniEntry_t *pEntry);

/* Function Name:
 *      dal_mango_vxlan_vni_delAll
 * Description:
 *      Delete all VNI entries
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - the module is not initial
 * Note:
 *      None
 */
extern int32
dal_mango_vxlan_vni_delAll(uint32 unit);

/* Function Name:
 *      dal_mango_vxlan_vni_get
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
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Note:
 *      None
 */
extern int32
dal_mango_vxlan_vni_get(uint32 unit, rtk_vxlan_vniEntry_t *pEntry);

/* Function Name:
 *      dal_mango_vxlan_vni_set
 * Description:
 *      Set a VNI entry
 * Input:
 *      unit  - unit id
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
dal_mango_vxlan_vni_set(uint32 unit, rtk_vxlan_vniEntry_t *pEntry);

/* Function Name:
 *      dal_mango_vxlan_vni_getNext
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
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Note:
 *      (1) Use base index -1 to indicate to search from the beginging of VNI entry
 *      (2) If the returned index is -1, means not found the next valid entry
 */
extern int32
dal_mango_vxlan_vni_getNext(uint32 unit, int32 *pBase, rtk_vxlan_vniEntry_t *pEntry);

/* Function Name:
 *      dal_mango_vxlan_globalCtrl_get
 * Description:
 *      Get the global configuration of the specified control type
 * Input:
 *      unit - unit id
 *      type - control type
 * Output:
 *      pArg - pointer to the argurment
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - the module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be a null pointer
 * Note:
 *      None
 */
extern int32
dal_mango_vxlan_globalCtrl_get(uint32 unit, rtk_vxlan_globalCtrlType_t type, int32 *pArg);

/* Function Name:
 *      dal_mango_vxlan_globalCtrl_set
 * Description:
 *      Set the global configuration of the specified control type
 * Input:
 *      unit - unit id
 *      type - control type
 *      arg  - argurment
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
dal_mango_vxlan_globalCtrl_set(uint32 unit, rtk_vxlan_globalCtrlType_t type, int32 arg);

#endif /* __DAL_MANGO_VXLAN_H__ */

