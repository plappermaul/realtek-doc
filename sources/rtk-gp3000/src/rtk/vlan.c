/*
 * Copyright (C) 2012 Realtek Semiconductor Corp.
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
 * Purpose : Definition of VLAN API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) Vlan table configure and modification
 *           (2) Accept frame type
 *           (3) Vlan ingress/egress filter
 *           (4) Port based and protocol based vlan
 *           (5) TPID configuration
 *           (6) Ingress tag handling
 *           (7) Tag format handling
 *
 */



/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/port.h>
#include <rtk/init.h>
#include <rtk/default.h>
#include <rtk/vlan.h>
#include <dal/dal_mgmt.h>


/*
 * Symbol Definition
 */

/*
 * Macro Declaration
 */


/*
 * Function Declaration
 */
/* Function Name:
 *      rtk_vlan_init
 * Description:
 *      Initialize vlan module.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize vlan module before calling any vlan APIs.
 */
int32
rtk_vlan_init(void)
{
    int32   ret;
    if (NULL == RT_MAPPER->vlan_init)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->vlan_init();
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_vlan_init */

/* Module Name    : Vlan                                  */
/* Sub-module Name: Vlan table configure and modification */

/* Function Name:
 *      rtk_vlan_create
 * Description:
 *      Create the vlan in the specified device.
 * Input:
 *      vid  - vlan id to be created
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_VLAN_VID   - invalid vid
 *      RT_ERR_VLAN_EXIST - vlan is exist
 * Note:
 */
int32
rtk_vlan_create(rtk_vlan_t vid)
{
    int32   ret;
    if (NULL == RT_MAPPER->vlan_create)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->vlan_create( vid);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_vlan_create */

/* Function Name:
 *      rtk_vlan_destroy
 * Description:
 *      Destroy the vlan.
 * Input:
 *      vid  - vlan id to be destroyed
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 * Note:
 *      None
 */
int32
rtk_vlan_destroy(rtk_vlan_t vid)
{
    int32   ret;
    if (NULL == RT_MAPPER->vlan_destroy)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->vlan_destroy( vid);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_vlan_destroy */


/* Function Name:
 *      rtk_vlan_destroyAll
 * Description:
 *      Destroy all vlans except default vlan.
 * Input:
 *      restore_default_vlan - keep and restore default vlan id or not?
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_UNIT_ID  - invalid unit id
 * Note:
 *      The restore argument is permit following value:
 *      - 0: remove default vlan
 *      - 1: restore default vlan
 */
int32
rtk_vlan_destroyAll(uint32 restoreDefaultVlan)
{
    int32   ret;
    if (NULL == RT_MAPPER->vlan_destroyAll)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->vlan_destroyAll( restoreDefaultVlan);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_vlan_destroyAll */

/* Function Name:
 *      rtk_vlan_fid_get
 * Description:
 *      Get the filter id of the vlan.
 * Input:
 *      vid  - vlan id
 * Output:
 *      pFid - pointer buffer of filter id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 *      (1) In IVL mode, vid is equal with fid after vlan create.
 *      (2) You don't need to care fid when you use the IVL mode.
 *      (3) The API should be used for SVL mode.
 */
int32
rtk_vlan_fid_get(rtk_vlan_t vid, rtk_fid_t *pFid)
{
    int32   ret;
    if (NULL == RT_MAPPER->vlan_fid_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->vlan_fid_get( vid, pFid);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_vlan_fid_get */


/* Function Name:
 *      rtk_vlan_fid_set
 * Description:
 *      Set the filter id of the vlan.
 * Input:
 *      vid  - vlan id
 *      fid  - filter id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_OUT_OF_RANGE         - input parameter out of range
 * Note:
 */
int32
rtk_vlan_fid_set(rtk_vlan_t vid, rtk_fid_t fid)
{
    int32   ret;
    if (NULL == RT_MAPPER->vlan_fid_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->vlan_fid_set( vid, fid);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_vlan_fid_set */

/* Function Name:
 *      rtk_vlan_fidMode_get
 * Description:
 *      Get the filter id mode of the vlan.
 * Input:
 *      vid  - vlan id
 * Output:
 *      pMode - pointer buffer of filter id mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 *      mode can be: -VLAN__FID_IVL
 *                   -VLAN__FID_SVL
 */
int32
rtk_vlan_fidMode_get(rtk_vlan_t vid, rtk_fidMode_t *pMode)
{
    int32   ret;
    if (NULL == RT_MAPPER->vlan_fidMode_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->vlan_fidMode_get( vid, pMode);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_vlan_fidMode_get */

/* Function Name:
 *      rtk_vlan_fidMode_set
 * Description:
 *      Set the filter id mode of the vlan.
 * Input:
 *      vid   - vlan id
 *      mode  - filter id mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_OUT_OF_RANGE         - input parameter out of range
 * Note:
 *      mode can be: -VLAN__FID_IVL
 *                   -VLAN__FID_SVL
 */
int32
rtk_vlan_fidMode_set(rtk_vlan_t vid, rtk_fidMode_t mode)
{
    int32   ret;
    if (NULL == RT_MAPPER->vlan_fidMode_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->vlan_fidMode_set( vid, mode);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_vlan_fidMode_set */

/* Function Name:
 *      rtk_vlan_port_get
 * Description:
 *      Get the vlan members.
 * Input:
 *      vid              - vlan id
 * Output:
 *      pMember_portmask - pointer buffer of member ports
 *      pUntag_portmask  - pointer buffer of untagged member ports
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtk_vlan_port_get(
    rtk_vlan_t     vid,
    rtk_portmask_t *pMemberPortmask,
    rtk_portmask_t *pUntagPortmask)
{
    int32   ret;
    if (NULL == RT_MAPPER->vlan_port_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->vlan_port_get( vid, pMemberPortmask, pUntagPortmask);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_vlan_port_get */


/* Function Name:
 *      rtk_vlan_port_set
 * Description:
 *      Replace the vlan members.
 * Input:
 *      vid              - vlan id
 *      pMember_portmask - member ports
 *      pUntag_portmask  - untagged member ports
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 *      Don't care the original vlan members and replace with new configure
 *      directly.
 *      If users specify an empty extension portmask and CPU port is set in pMember_portmask,
 *      the packets will be restricted to be forwarded to CPU.
 *      Likewise, If users specify an non-empty extension portmask and
 *      CPU port is not set in pMember_portmask, the packets will be restricted to be forwarded to CPU. too.
 */
int32
rtk_vlan_port_set(
    rtk_vlan_t     vid,
    rtk_portmask_t *pMember_portmask,
    rtk_portmask_t *pUntag_portmask)
{
    int32   ret;
    if (NULL == RT_MAPPER->vlan_port_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->vlan_port_set( vid, pMember_portmask, pUntag_portmask);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_vlan_port_set */


/* Function Name:
 *      rtk_vlan_extPort_get
 * Description:
 *      Get the vlan extension members.
 * Input:
 *      vid              - vlan id
 * Output:
 *      pExt_portmask    - pointer buffer of extension member ports
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtk_vlan_extPort_get(
    rtk_vlan_t     vid,
    rtk_portmask_t *pExt_portmask)
{
    int32   ret;
    if (NULL == RT_MAPPER->vlan_extPort_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->vlan_extPort_get( vid, pExt_portmask);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_vlan_extPort_get */


/* Function Name:
 *      rtk_vlan_extPort_set
 * Description:
 *      Replace the vlan extension members.
 * Input:
 *      vid              - vlan id
 *      pExt_portmask    - extension member ports
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 *      Don't care the original vlan members and replace with new configure
 *      directly.
 *      If users specify an empty extension portmask and CPU port is set in pMember_portmask,
 *      the packets will be restricted to be forwarded to CPU.
 *      Likewise, If users specify an non-empty extension portmask and
 *      CPU port is not set in pMember_portmask, the packets will be restricted to be forwarded to CPU. too.
 */
int32
rtk_vlan_extPort_set(
    rtk_vlan_t     vid,
    rtk_portmask_t *pExt_portmask)
{
    int32   ret;
    if (NULL == RT_MAPPER->vlan_extPort_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->vlan_extPort_set( vid, pExt_portmask);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_vlan_extPort_set */

/* Function Name:
 *      rtk_vlan_stg_get
 * Description:
 *      Get spanning tree group instance of the vlan from the specified device.
 * Input:
 *      vid  - vlan id
 * Output:
 *      pStg - pointer buffer of spanning tree group instance
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtk_vlan_stg_get(rtk_vlan_t vid, rtk_stg_t *pStg)
{
    int32   ret;
    if (NULL == RT_MAPPER->vlan_stg_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->vlan_stg_get( vid, pStg);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_vlan_stg_get */


/* Function Name:
 *      rtk_vlan_stg_set
 * Description:
 *      Set spanning tree group instance of the vlan.
 * Input:
 *      vid  - vlan id
 *      stg  - spanning tree group instance
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_MSTI                 - invalid msti
 * Note:
 *      None
 */
int32
rtk_vlan_stg_set(rtk_vlan_t vid, rtk_stg_t stg)
{
    int32   ret;
    if (NULL == RT_MAPPER->vlan_stg_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->vlan_stg_set( vid, stg);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_vlan_stg_set */

/* Function Name:
 *      rtk_vlan_priority_get
 * Description:
 *      Get VLAN priority for each CVLAN.
 * Input:
 *      vid  - vlan id
 * Output:
 *      pPriority - 802.1p priority for the PVID.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_VLAN_VID     - Invalid VID parameter.
 *      RT_ERR_PORT_ID      - Invalid port number.
 * Note:
 *     This API is used to set priority per VLAN.
 */
int32
rtk_vlan_priority_get(rtk_vlan_t vid, rtk_pri_t *pPriority)
{
    int32   ret;
    if (NULL == RT_MAPPER->vlan_priority_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->vlan_priority_get( vid, pPriority);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_vlan_priority_get */

/* Function Name:
 *      rtk_vlan_priority_set
 * Description:
 *      Set VLAN priority for each CVLAN.
 * Input:
 *      vid  - vlan id
 *      priority - 802.1p priority for the PVID.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_VLAN_VID         - Invalid VID parameter.
 *      RT_ERR_VLAN_PRIORITY    - Invalid priority.
 * Note:
 *      This API is used to set priority per VLAN.
 */
int32
rtk_vlan_priority_set(rtk_vlan_t vid, rtk_pri_t priority)
{
    int32   ret;
    if (NULL == RT_MAPPER->vlan_priority_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->vlan_priority_set( vid, priority);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_vlan_priority_set */

/* Function Name:
 *      rtk_vlan_priorityEnable_get
 * Description:
 *      Get vlan based priority assignment status.
 * Input:
 *      vid  - vlan id
 * Output:
 *      pEnable - pointer to vlan based priority assignment status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtk_vlan_priorityEnable_get(rtk_vlan_t vid, rtk_enable_t *pEnable)
{
    int32   ret;
    if (NULL == RT_MAPPER->vlan_priorityEnable_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->vlan_priorityEnable_get( vid, pEnable);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_vlan_priorityEnable_get */


/* Function Name:
 *      rtk_vlan_priorityEnable_set
 * Description:
 *      Set vlan based priority assignment status.
 * Input:
 *      vid    - vlan id
 *      enable - vlan based priority assignment status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 * Note:
 *      None
 */
int32
rtk_vlan_priorityEnable_set(rtk_vlan_t vid, rtk_enable_t enable)
{
    int32   ret;
    if (NULL == RT_MAPPER->vlan_priorityEnable_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->vlan_priorityEnable_set( vid, enable);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_vlan_priorityEnable_set */

/* Function Name:
 *      rtk_vlan_policingEnable_get
 * Description:
 *      Get the policing mode of the vlan.
 * Input:
 *      vid  - vlan id
 * Output:
 *      pMode - pointer buffer of filter id mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 */
int32
rtk_vlan_policingEnable_get(rtk_vlan_t vid, rtk_enable_t *pEnable)
{
    int32   ret;
    if (NULL == RT_MAPPER->vlan_policingEnable_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->vlan_policingEnable_get( vid, pEnable);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_vlan_policingEnable_get */

/* Function Name:
 *      rtk_vlan_policingEnable_set
 * Description:
 *      Set the policing mode of the vlan.
 * Input:
 *      vid    - vlan id
 *      enable - State of Policing.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_OUT_OF_RANGE         - input parameter out of range
 * Note:
 */
int32
rtk_vlan_policingEnable_set(rtk_vlan_t vid, rtk_enable_t enable)
{
    int32   ret;
    if (NULL == RT_MAPPER->vlan_policingEnable_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->vlan_policingEnable_set( vid, enable);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_vlan_policingEnable_set */

/* Function Name:
 *      rtk_vlan_policingMeterIdx_get
 * Description:
 *      Get the policing mode of the vlan.
 * Input:
 *      vid  - vlan id
 * Output:
 *      pIndex - pointer of meter index
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 */
int32
rtk_vlan_policingMeterIdx_get(rtk_vlan_t vid, uint32 *pIndex)
{
    int32   ret;
    if (NULL == RT_MAPPER->vlan_policingMeterIdx_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->vlan_policingMeterIdx_get( vid, pIndex);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_vlan_policingMeterIdx_get */

/* Function Name:
 *      rtk_vlan_policingMeterIdx_set
 * Description:
 *      Set the policing mode of the vlan.
 * Input:
 *      vid    - vlan id
 *      index  - Meter index.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_OUT_OF_RANGE         - input parameter out of range
 * Note:
 */
int32
rtk_vlan_policingMeterIdx_set(rtk_vlan_t vid, uint32 index)
{
    int32   ret;
    if (NULL == RT_MAPPER->vlan_policingMeterIdx_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->vlan_policingMeterIdx_set( vid, index);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_vlan_policingMeterIdx_set */

/* Module Name     : vlan                */
/* Sub-module Name : vlan port attribute */

/* Function Name:
 *      rtk_vlan_portAcceptFrameType_get
 * Description:
 *      Get vlan accept frame type of the port.
 * Input:
 *      port               - port id
 * Output:
 *      pAcceptFrameType   - pointer buffer of accept frame type
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The accept frame type as following:
 *          - ACCEPT_FRAME_TYPE_ALL
 *          - ACCEPT_FRAME_TYPE_TAG_ONLY
 *          - ACCEPT_FRAME_TYPE_UNTAG_ONLY
 *
 */
int32
rtk_vlan_portAcceptFrameType_get(
    rtk_port_t                 port,
    rtk_vlan_acceptFrameType_t *pAcceptFrameType)
{
    int32   ret;
    if (NULL == RT_MAPPER->vlan_portAcceptFrameType_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->vlan_portAcceptFrameType_get( port, pAcceptFrameType);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_vlan_portAcceptFrameType_get */


/* Function Name:
 *      rtk_vlan_portAcceptFrameType_set
 * Description:
 *      Set vlan accept frame type of the port.
 * Input:
 *      port              - port id
 *      acceptFrameType   - accept frame type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT               - The module is not initial
 *      RT_ERR_PORT_ID                - invalid port id
 *      RT_ERR_VLAN_ACCEPT_FRAME_TYPE - invalid accept frame type
 *      RT_ERR_CHIP_NOT_SUPPORTED     - functions not supported by this chip model
 * Note:
 *      The accept frame type as following:
 *          - ACCEPT_FRAME_TYPE_ALL
 *          - ACCEPT_FRAME_TYPE_TAG_ONLY
 *          - ACCEPT_FRAME_TYPE_UNTAG_ONLY
 *          - ACCPET_FRAME_TYPE_1P_1Q_TAG_ONLY
 */
int32
rtk_vlan_portAcceptFrameType_set(
    rtk_port_t                 port,
    rtk_vlan_acceptFrameType_t acceptFrameType)
{
    int32   ret;
    if (NULL == RT_MAPPER->vlan_portAcceptFrameType_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->vlan_portAcceptFrameType_set( port, acceptFrameType);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_vlan_portAcceptFrameType_set */

/* Function Name:
 *      rtk_vlan_vlanFunctionEnable_get
 * Description:
 *      Get the VLAN enable status.
 * Input:
 *      enable - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 * Note:
 *      The status of vlan function is as following:
 *      - DISABLED
 *      - ENABLED
 */
int32
rtk_vlan_vlanFunctionEnable_get(rtk_enable_t *pEnable)
{
    int32   ret;
    if (NULL == RT_MAPPER->vlan_vlanFunctionEnable_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->vlan_vlanFunctionEnable_get( pEnable);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_vlan_vlanFunctionEnable_get */


/* Function Name:
 *      rtk_vlan_vlanFunctionEnable_set
 * Description:
 *      Set the VLAN enable status.
 * Input:
 *      enable - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 * Note:
 *      The status of vlan function is as following:
 *      - DISABLED
 *      - ENABLED
 */
int32
rtk_vlan_vlanFunctionEnable_set(rtk_enable_t enable)
{
    int32   ret;
    if (NULL == RT_MAPPER->vlan_vlanFunctionEnable_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->vlan_vlanFunctionEnable_set( enable);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_vlan_vlanFunctionEnable_set */

/* Module Name    : Vlan                       */
/* Sub-module Name: Vlan ingress/egress filter */

/* Function Name:
 *      rtk_vlan_portIgrFilterEnable_get
 * Description:
 *      Get vlan ingress filter status of the port.
 * Input:
 *      port        - port id
 * Output:
 *      pEnable     - pointer buffer of ingress filter status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The status of vlan function is as following:
 *      - DISABLED
 *      - ENABLED
 */
int32
rtk_vlan_portIgrFilterEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    if (NULL == RT_MAPPER->vlan_portIgrFilterEnable_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->vlan_portIgrFilterEnable_get( port, pEnable);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_vlan_portIgrFilterEnable_get */


/* Function Name:
 *      rtk_vlan_portIgrFilterEnable_set
 * Description:
 *      Set vlan ingress filter status of the port to the specified device.
 * Input:
 *      port       - port id
 *      enable     - ingress filter configure
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      The status of vlan ingress filter is as following:
 *          - DISABLED
 *          - ENABLED
 */
int32
rtk_vlan_portIgrFilterEnable_set(rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
    if (NULL == RT_MAPPER->vlan_portIgrFilterEnable_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->vlan_portIgrFilterEnable_set( port, enable);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_vlan_portIgrFilterEnable_set */

/* Function Name:
 *      rtk_vlan_leaky_get
 * Description:
 *      Get VLAN leaky.
 * Input:
 *      type - Packet type for VLAN leaky.
 * Output:
 *      pEnable - Leaky status.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT 		- Invalid input parameters.
 * Note:
 *      This API can get VLAN leaky status for RMA and IGMP/MLD packets.
 *      The leaky frame types are as following:
 *      - LEAKY_BRG_GROUP,
 *      - LEAKY_FD_PAUSE,
 *      - LEAKY_SP_MCAST,
 *      - LEAKY_1X_PAE,
 *      - LEAKY_UNDEF_BRG_04,
 *      - LEAKY_UNDEF_BRG_05,
 *      - LEAKY_UNDEF_BRG_06,
 *      - LEAKY_UNDEF_BRG_07,
 *      - LEAKY_PROVIDER_BRIDGE_GROUP_ADDRESS,
 *      - LEAKY_UNDEF_BRG_09,
 *      - LEAKY_UNDEF_BRG_0A,
 *      - LEAKY_UNDEF_BRG_0B,
 *      - LEAKY_UNDEF_BRG_0C,
 *      - LEAKY_PROVIDER_BRIDGE_GVRP_ADDRESS,
 *      - LEAKY_8021AB,
 *      - LEAKY_UNDEF_BRG_0F,
 *      - LEAKY_BRG_MNGEMENT,
 *      - LEAKY_UNDEFINED_11,
 *      - LEAKY_UNDEFINED_12,
 *      - LEAKY_UNDEFINED_13,
 *      - LEAKY_UNDEFINED_14,
 *      - LEAKY_UNDEFINED_15,
 *      - LEAKY_UNDEFINED_16,
 *      - LEAKY_UNDEFINED_17,
 *      - LEAKY_UNDEFINED_18,
 *      - LEAKY_UNDEFINED_19,
 *      - LEAKY_UNDEFINED_1A,
 *      - LEAKY_UNDEFINED_1B,
 *      - LEAKY_UNDEFINED_1C,
 *      - LEAKY_UNDEFINED_1D,
 *      - LEAKY_UNDEFINED_1E,
 *      - LEAKY_UNDEFINED_1F,
 *      - LEAKY_GMRP,
 *      - LEAKY_GVRP,
 *      - LEAKY_UNDEF_GARP_22,
 *      - LEAKY_UNDEF_GARP_23,
 *      - LEAKY_UNDEF_GARP_24,
 *      - LEAKY_UNDEF_GARP_25,
 *      - LEAKY_UNDEF_GARP_26,
 *      - LEAKY_UNDEF_GARP_27,
 *      - LEAKY_UNDEF_GARP_28,
 *      - LEAKY_UNDEF_GARP_29,
 *      - LEAKY_UNDEF_GARP_2A,
 *      - LEAKY_UNDEF_GARP_2B,
 *      - LEAKY_UNDEF_GARP_2C,
 *      - LEAKY_UNDEF_GARP_2D,
 *      - LEAKY_UNDEF_GARP_2E,
 *      - LEAKY_UNDEF_GARP_2F,
 *      - LEAKY_IGMP,
 *      - LEAKY_CDP,
 *      - LEAKY_SSTP,
 */
int32
rtk_vlan_leaky_get(rtk_leaky_type_t type, rtk_enable_t *pEnable)
{
    int32   ret;
    if (NULL == RT_MAPPER->vlan_leaky_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->vlan_leaky_get( type, pEnable);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_vlan_leaky_get */

/* Function Name:
 *      rtk_vlan_leaky_set
 * Description:
 *      Set VLAN leaky.
 * Input:
 *      type - Packet type for VLAN leaky.
 *      enable - Leaky status.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT 		- Invalid input parameters.
 *      RT_ERR_ENABLE       - Invalid enable input
 * Note:
 *      This API can set VLAN leaky for RMA and IGMP/MLD packets.
 *      The leaky frame types are as following:
 *      - LEAKY_BRG_GROUP,
 *      - LEAKY_FD_PAUSE,
 *      - LEAKY_SP_MCAST,
 *      - LEAKY_1X_PAE,
 *      - LEAKY_UNDEF_BRG_04,
 *      - LEAKY_UNDEF_BRG_05,
 *      - LEAKY_UNDEF_BRG_06,
 *      - LEAKY_UNDEF_BRG_07,
 *      - LEAKY_PROVIDER_BRIDGE_GROUP_ADDRESS,
 *      - LEAKY_UNDEF_BRG_09,
 *      - LEAKY_UNDEF_BRG_0A,
 *      - LEAKY_UNDEF_BRG_0B,
 *      - LEAKY_UNDEF_BRG_0C,
 *      - LEAKY_PROVIDER_BRIDGE_GVRP_ADDRESS,
 *      - LEAKY_8021AB,
 *      - LEAKY_UNDEF_BRG_0F,
 *      - LEAKY_BRG_MNGEMENT,
 *      - LEAKY_UNDEFINED_11,
 *      - LEAKY_UNDEFINED_12,
 *      - LEAKY_UNDEFINED_13,
 *      - LEAKY_UNDEFINED_14,
 *      - LEAKY_UNDEFINED_15,
 *      - LEAKY_UNDEFINED_16,
 *      - LEAKY_UNDEFINED_17,
 *      - LEAKY_UNDEFINED_18,
 *      - LEAKY_UNDEFINED_19,
 *      - LEAKY_UNDEFINED_1A,
 *      - LEAKY_UNDEFINED_1B,
 *      - LEAKY_UNDEFINED_1C,
 *      - LEAKY_UNDEFINED_1D,
 *      - LEAKY_UNDEFINED_1E,
 *      - LEAKY_UNDEFINED_1F,
 *      - LEAKY_GMRP,
 *      - LEAKY_GVRP,
 *      - LEAKY_UNDEF_GARP_22,
 *      - LEAKY_UNDEF_GARP_23,
 *      - LEAKY_UNDEF_GARP_24,
 *      - LEAKY_UNDEF_GARP_25,
 *      - LEAKY_UNDEF_GARP_26,
 *      - LEAKY_UNDEF_GARP_27,
 *      - LEAKY_UNDEF_GARP_28,
 *      - LEAKY_UNDEF_GARP_29,
 *      - LEAKY_UNDEF_GARP_2A,
 *      - LEAKY_UNDEF_GARP_2B,
 *      - LEAKY_UNDEF_GARP_2C,
 *      - LEAKY_UNDEF_GARP_2D,
 *      - LEAKY_UNDEF_GARP_2E,
 *      - LEAKY_UNDEF_GARP_2F,
 *      - LEAKY_IGMP,
 *      - LEAKY_CDP,
 *      - LEAKY_SSTP,
 */
int32
rtk_vlan_leaky_set(rtk_leaky_type_t type, rtk_enable_t enable)
{
    int32   ret;
    if (NULL == RT_MAPPER->vlan_leaky_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->vlan_leaky_set( type, enable);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_vlan_leaky_set */

/* Function Name:
 *      rtk_vlan_portLeaky_get
 * Description:
 *      Get VLAN port-based leaky.
 * Input:
 *      port - port ID
 *      type - Packet type for VLAN leaky.
 * Output:
 *      pEnable - Leaky status.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT 		- Invalid input parameters.
 *      RT_ERR_ENABLE       - Invalid enable input
 *      RT_ERR_NULL_POINTER - NULL Pointer
 * Note:
 *      This API can set VLAN leaky for RMA and IGMP/MLD packets.
 *      The leaky frame types are as following:
 *      - LEAKY_IPMULTICAST
 */
int32
rtk_vlan_portLeaky_get(rtk_port_t port, rtk_leaky_type_t type, rtk_enable_t *pEnable)
{
    int32   ret;
    if (NULL == RT_MAPPER->vlan_leaky_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->vlan_portLeaky_get( port, type, pEnable);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_vlan_portLeaky_get */

/* Function Name:
 *      rtk_vlan_portLeaky_set
 * Description:
 *      Set VLAN port-based leaky.
 * Input:
 *      port - port ID
 *      type - Packet type for VLAN leaky.
 *      enable - Leaky status.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT 		- Invalid input parameters.
 *      RT_ERR_ENABLE       - Invalid enable input
 * Note:
 *      This API can set VLAN leaky for RMA and IGMP/MLD packets.
 *      The leaky frame types are as following:
 *      - LEAKY_IPMULTICAST
 */
int32
rtk_vlan_portLeaky_set(rtk_port_t port, rtk_leaky_type_t type, rtk_enable_t enable)
{
    int32   ret;
    if (NULL == RT_MAPPER->vlan_portLeaky_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->vlan_portLeaky_set( port, type, enable);
    RTK_API_UNLOCK();
    return ret;
}

/* Function Name:
 *      rtk_vlan_keepType_get
 * Description:
 *      Get VLAN keep format setting.
 * Input:
 *      type - Packet type for VLAN keep format.
 * Output:
 *      pEnable - Leaky status.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT 		- Invalid input parameters.
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 */
int32
rtk_vlan_keepType_get(rtk_vlan_keep_type_t type, rtk_enable_t *pEnable)
{
    int32   ret;
    if (NULL == RT_MAPPER->vlan_keepType_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->vlan_keepType_get( type, pEnable);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_vlan_keepType_get */

/* Function Name:
 *      rtk_vlan_keepType_set
 * Description:
 *      Set VLAN keep format setting.
 * Input:
 *      type - Packet type for VLAN keep format.
 * Output:
 *      pEnable - Leaky status.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT 		- Invalid input parameters.
 * Note:
 */
int32
rtk_vlan_keepType_set(rtk_vlan_keep_type_t type, rtk_enable_t enable)
{
    int32   ret;
    if (NULL == RT_MAPPER->vlan_keepType_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->vlan_keepType_set( type, enable);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_vlan_keepType_set */

/* Module Name    : Vlan                               */
/* Sub-module Name: Port based and protocol based vlan */

/* Function Name:
 *      rtk_vlan_portPvid_get
 * Description:
 *      Get port default vlan id.
 * Input:
 *      port  - port id
 * Output:
 *      pPvid - pointer buffer of port default vlan id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtk_vlan_portPvid_get(rtk_port_t port, uint32 *pPvid)
{
    int32   ret;
    if (NULL == RT_MAPPER->vlan_portPvid_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->vlan_portPvid_get( port, pPvid);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_vlan_portPvid_get */


/* Function Name:
 *      rtk_vlan_portPvid_set
 * Description:
 *      Set port default vlan id.
 * Input:
 *      port - port id
 *      pvid - port default vlan id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_VLAN_VID - invalid vid
 * Note:
 *      None
 */
int32
rtk_vlan_portPvid_set(rtk_port_t port, uint32 pvid)
{
    int32   ret;
    if (NULL == RT_MAPPER->vlan_portPvid_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->vlan_portPvid_set( port, pvid);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_vlan_portPvid_set */


/* Function Name:
 *      rtk_vlan_extPortPvid_get
 * Description:
 *      Get extension port default vlan id.
 * Input:
 *      extPort - Extension port id
 * Output:
 *      pPvid - pointer buffer of port default vlan id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtk_vlan_extPortPvid_get(uint32 extPort, uint32 *pPvid)
{
    int32   ret;
    if (NULL == RT_MAPPER->vlan_extPortPvid_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->vlan_extPortPvid_get( extPort, pPvid);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_vlan_extPortPvid_get */


/* Function Name:
 *      rtk_vlan_extPortPvid_set
 * Description:
 *      Set extension port default vlan id.
 * Input:
 *      extPort - extension port id
 *      pvid    - extension port default vlan id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_VLAN_VID - invalid vid
 * Note:
 *      None
 */
int32
rtk_vlan_extPortPvid_set(uint32 extPort, uint32 pvid)
{
    int32   ret;
    if (NULL == RT_MAPPER->vlan_extPortPvid_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->vlan_extPortPvid_set( extPort, pvid);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_vlan_extPortPvid_set */

/* Function Name:
 *      rtk_vlan_protoGroup_get
 * Description:
 *      Get protocol group for protocol based vlan.
 * Input:
 *      protoGroupIdx  - protocol group index
 * Output:
 *      pProtoGroup    - pointer to protocol group
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - protocol group index is out of range
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtk_vlan_protoGroup_get(
    uint32                  protoGroupIdx,
    rtk_vlan_protoGroup_t   *pProtoGroup)
{
    int32   ret;
    if (NULL == RT_MAPPER->vlan_protoGroup_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->vlan_protoGroup_get( protoGroupIdx, pProtoGroup);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_vlan_protoGroup_get */

/* Function Name:
 *      rtk_vlan_protoGroup_set
 * Description:
 *      Set protocol group for protocol based vlan.
 * Input:
 *      protoGroupIdx  - protocol group index
 *      protoGroup     - protocol group
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_VLAN_FRAME_TYPE - invalid frame type
 *      RT_ERR_OUT_OF_RANGE    - protocol group index is out of range
 *      RT_ERR_INPUT           - invalid input parameter
 * Note:
 *      Frame type is as following:
 *      - FRAME_TYPE_ETHERNET
 *      - FRAME_TYPE_RFC1042 (SNAP)
 *      - FRAME_TYPE_LLCOTHER
 */
int32
rtk_vlan_protoGroup_set(
    uint32                  protoGroupIdx,
    rtk_vlan_protoGroup_t   *pProtoGroup)
{
    int32   ret;
    if (NULL == RT_MAPPER->vlan_protoGroup_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->vlan_protoGroup_set( protoGroupIdx, pProtoGroup);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_vlan_protoGroup_set */

/* Function Name:
 *      rtk_vlan_portProtoVlan_get
 * Description:
 *      Get vlan of specificed protocol group on specified port.
 * Input:
 *      port           - port id
 *      protoGroup_idx - protocol group index
 * Output:
 *      pVlan_cfg      - pointer to vlan configuration of protocol group
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_OUT_OF_RANGE - protocol group index is out of range
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtk_vlan_portProtoVlan_get(
    rtk_port_t              port,
    uint32                  protoGroupIdx,
    rtk_vlan_protoVlanCfg_t *pVlanCfg)
{
    int32   ret;
    if (NULL == RT_MAPPER->vlan_portProtoVlan_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->vlan_portProtoVlan_get( port, protoGroupIdx, pVlanCfg);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_vlan_portProtoVlan_get */

/* Function Name:
 *      rtk_vlan_portProtoVlan_set
 * Description:
 *      Set vlan of specificed protocol group on specified port.
 * Input:
 *      port           - port id
 *      protoGroupIdx  - protocol group index
 *      pVlanCfg       - vlan configuration of protocol group
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_OUT_OF_RANGE - protocol group index is out of range
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtk_vlan_portProtoVlan_set(
    rtk_port_t              port,
    uint32                  protoGroupIdx,
    rtk_vlan_protoVlanCfg_t *pVlanCfg)
{
    int32   ret;
    if (NULL == RT_MAPPER->vlan_portProtoVlan_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->vlan_portProtoVlan_set( port, protoGroupIdx, pVlanCfg);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_vlan_portProtoVlan_set */

/* Module Name    : Vlan                */
/* Sub-module Name: Tag format handling */

/* Function Name:
 *      rtk_vlan_tagMode_get
 * Description:
 *      Get vlan tagged mode of the port.
 * Input:
 *      port      - port id
 * Output:
 *      pTagMode  - pointer buffer of vlan tagged mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The vlan tagged mode as following:
 *      - VLAN_TAG_MODE_ORIGINAL        (depend on chip normal decision)
 *      - VLAN_TAG_MODE_KEEP_FORMAT     (keep ingress format to egress)
 *      - VLAN_TAG_MODE_PRI             (always priority tag out)
 */
int32
rtk_vlan_tagMode_get(rtk_port_t port, rtk_vlan_tagMode_t *pTagMode)
{
    int32   ret;
    if (NULL == RT_MAPPER->vlan_tagMode_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->vlan_tagMode_get( port, pTagMode);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_vlan_tagMode_get */


/* Function Name:
 *      rtk_vlan_tagMode_set
 * Description:
 *      Set vlan tagged mode of the port.
 * Input:
 *      port     - port id
 *      tagMode  - vlan tagged mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      The vlan tagged mode as following:
 *      - VLAN_TAG_MODE_ORIGINAL        (depend on chip normal decision)
 *      - VLAN_TAG_MODE_KEEP_FORMAT     (keep ingress format to egress)
 *      - VLAN_TAG_MODE_PRI             (always priority tag out)
 */
int32
rtk_vlan_tagMode_set(rtk_port_t port, rtk_vlan_tagMode_t tagMode)
{
    int32   ret;
    if (NULL == RT_MAPPER->vlan_tagMode_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->vlan_tagMode_set( port, tagMode);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_vlan_tagMode_set */




/* Function Name:
 *      rtk_vlan_portFid_get
 * Description:
 *      Get port-based filtering database
 * Input:
 *      port - Port id.
 * Output:
 *      pEnable - ebable port-based FID
 *      pFid - Specified filtering database.
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_INPUT - Invalid input parameters.
 *      RT_ERR_PORT_ID - Invalid port ID.
 * Note:
 *      The API can get port-based filtering database status. If the function is enabled, all input
 *      packets will be assigned to the port-based fid regardless vlan tag.
 */
int32
rtk_vlan_portFid_get(rtk_port_t port, rtk_enable_t *pEnable, rtk_fid_t *pFid)
{
    int32   ret;
    if (NULL == RT_MAPPER->vlan_portFid_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->vlan_portFid_get( port, pEnable, pFid);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_vlan_portFid_get */


/* Function Name:
 *      rtk_vlan_portFid_set
 * Description:
 *      Set port-based filtering database
 * Input:
 *      port - Port id.
 *      enable - ebable port-based FID
 *      fid - Specified filtering database.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_L2_FID - Invalid fid.
 *      RT_ERR_INPUT - Invalid input parameter.
 *      RT_ERR_PORT_ID - Invalid port ID.
 * Note:
 *      The API can set port-based filtering database. If the function is enabled, all input
 *      packets will be assigned to the port-based fid regardless vlan tag.
 */
int32
rtk_vlan_portFid_set(rtk_port_t port, rtk_enable_t enable, rtk_fid_t fid)
{
    int32   ret;
    if (NULL == RT_MAPPER->vlan_portFid_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->vlan_portFid_set( port, enable, fid);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_vlan_portFid_set */

/* Function Name:
 *      rtk_vlan_portPriority_get
 * Description:
 *      Get port-based priority
 * Input:
 *      port - Port id.
 * Output:
 *      pPriority - port-based priority
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_INPUT - Invalid input parameters.
 *      RT_ERR_PORT_ID - Invalid port ID.
 * Note:
 *
 */
extern int32
rtk_vlan_portPriority_get(rtk_port_t port, rtk_pri_t *pPriority)
{
    int32   ret;
    if (NULL == RT_MAPPER->vlan_portPriority_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->vlan_portPriority_get( port, pPriority);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_vlan_portPriority_get */


/* Function Name:
 *      rtk_vlan_portPriority_set
 * Description:
 *      Set port-based priority
 * Input:
 *      port - Port id.
 *      priority - VLAN port-based priority
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_INPUT - Invalid input parameter.
 *      RT_ERR_PORT_ID - Invalid port ID.
 * Note:
 */
int32
rtk_vlan_portPriority_set(rtk_port_t port, rtk_pri_t priority)
{
    int32   ret;
    if (NULL == RT_MAPPER->vlan_portPriority_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->vlan_portPriority_set( port, priority);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_vlan_portPriority_set */

/* Function Name:
 *      rtk_vlan_portEgrTagKeepType_get
 * Description:
 *      Get egress tag keep type
 * Input:
 *      egr_port     - Egress port id.
 * Output:
 *      pIgr_portmask - Pointer of Ingress portmask
 *      pType         - Pointer of tag keep type
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - Invalid input parameter.
 *      RT_ERR_PORT_ID      - Invalid port ID.
 *      RT_ERR_NULL_POINTER - Null pointer
 * Note:
 *      None
 */
int32
rtk_vlan_portEgrTagKeepType_get(rtk_port_t egr_port, rtk_portmask_t *pIgr_portmask, rtk_vlan_tagKeepType_t *pType)
{
    int32   ret;
    if (NULL == RT_MAPPER->vlan_portEgrTagKeepType_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->vlan_portEgrTagKeepType_get( egr_port, pIgr_portmask, pType);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_vlan_portEgrTagKeepType_get */

/* Function Name:
 *      rtk_vlan_portEgrTagKeepType_set
 * Description:
 *      Set egress tag keep type
 * Input:
 *      egr_port      - Egress port id.
 *      pIgr_portmask - Pointer of Ingress portmask
 *      Type          - Tag keep type
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - Invalid input parameter.
 *      RT_ERR_PORT_ID      - Invalid port ID.
 *      RT_ERR_NULL_POINTER - Null pointer
 *      RT_ERR_PORT_MASK    - Invalid port mask.
 * Note:
 *      None
 */
int32
rtk_vlan_portEgrTagKeepType_set(rtk_port_t egr_port, rtk_portmask_t *pIgr_portmask, rtk_vlan_tagKeepType_t type)
{
    int32   ret;
    if (NULL == RT_MAPPER->vlan_portEgrTagKeepType_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->vlan_portEgrTagKeepType_set( egr_port, pIgr_portmask, type);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_vlan_portEgrTagKeepType_set */


/* Function Name:
 *      rtk_vlan_transparentEnable_get
 * Description:
 *      Get state of VLAN transparent
 * Input:
 *      None
 * Output:
 *      pEnable - Pointer of VLAN transparent function
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_NULL_POINTER - Null pointer
 * Note:
 *      None
 */
int32
rtk_vlan_transparentEnable_get(rtk_enable_t *pEnable)
{
    int32   ret;
    if (NULL == RT_MAPPER->vlan_transparentEnable_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->vlan_transparentEnable_get( pEnable);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_vlan_transparentEnable_get */

/* Function Name:
 *      rtk_vlan_transparentEnable_set
 * Description:
 *      Set state of VLAN transparent
 * Input:
 *      enable  - VLAN transparent function.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - Invalid input parameter.
 * Note:
 *      None
 */
int32
rtk_vlan_transparentEnable_set(rtk_enable_t enable)
{
    int32   ret;
    if (NULL == RT_MAPPER->vlan_transparentEnable_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->vlan_transparentEnable_set( enable);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_vlan_transparentEnable_set */

/* Function Name:
 *      rtk_vlan_cfiKeepEnable_get
 * Description:
 *      Get state of CFI keep
 * Input:
 *      None
 * Output:
 *      pEnable - Pointer of CFI Keep
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_NULL_POINTER - Null pointer
 * Note:
 *      ENABLED: Keep original CFI value
 *      DISABLED: Always output VLAN tag with CFI = 0
 */
int32
rtk_vlan_cfiKeepEnable_get(rtk_enable_t *pEnable)
{
    int32   ret;
    if (NULL == RT_MAPPER->vlan_cfiKeepEnable_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->vlan_cfiKeepEnable_get( pEnable);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_vlan_cfiKeepEnable_get */

/* Function Name:
 *      rtk_vlan_cfiKeepEnable_set
 * Description:
 *      Set state of CFI keep
 * Input:
 *      enable  - state of CFI KEEP
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - Invalid input parameter.
 * Note:
 *      ENABLED: Keep original CFI value
 *      DISABLED: Always output VLAN tag with CFI = 0
 */
int32
rtk_vlan_cfiKeepEnable_set(rtk_enable_t enable)
{
    int32   ret;
    if (NULL == RT_MAPPER->vlan_cfiKeepEnable_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->vlan_cfiKeepEnable_set( enable);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_vlan_cfiKeepEnable_set */

/* Function Name:
 *      rtk_vlan_reservedVidAction_get
 * Description:
 *      Get the action of VID 0 and VID 4095 packet
 * Input:
 *      None
 * Output:
 *      pAction_vid0    - Pointer of VID 0 action
 *      pAction_vid4095 - Pointer of VID 4095 action
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_NULL_POINTER - Null pointer
 * Note:
 *      RESVID_ACTION_UNTAG: VID 0 or VID 4095 tagged packets will be treated as untagged packets
 *      RESVID_ACTION_UNTAG: VID 0 or VID 4095 tagged packets will be treated as tagged packets
 */
int32
rtk_vlan_reservedVidAction_get(rtk_vlan_resVidAction_t *pAction_vid0, rtk_vlan_resVidAction_t *pAction_vid4095)
{
    int32   ret;
    if (NULL == RT_MAPPER->vlan_reservedVidAction_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->vlan_reservedVidAction_get( pAction_vid0, pAction_vid4095);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_vlan_reservedVidAction_get */

/* Function Name:
 *      rtk_vlan_reservedVidAction_set
 * Description:
 *      Set the action of VID 0 and VID 4095 packet
 * Input:
 *      None
 * Output:
 *      action_vid0     - Pointer of VID 0 action
 *      action_vid4095  - Pointer of VID 4095 action
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - Error Input
 * Note:
 *      RESVID_ACTION_UNTAG: VID 0 or VID 4095 tagged packets will be treated as untagged packets
 *      RESVID_ACTION_UNTAG: VID 0 or VID 4095 tagged packets will be treated as tagged packets
 */
int32
rtk_vlan_reservedVidAction_set(rtk_vlan_resVidAction_t action_vid0, rtk_vlan_resVidAction_t action_vid4095)
{
    int32   ret;
    if (NULL == RT_MAPPER->vlan_reservedVidAction_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->vlan_reservedVidAction_set( action_vid0, action_vid4095);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_vlan_reservedVidAction_set */

/* Function Name:
 *      rtk_vlan_tagModeIp4mc_get
 * Description:
 *      Get vlan tagged mode for ipv4 multicast packet of the port.
 * Input:
 *      port      - port id
 * Output:
 *      pTagMode  - pointer buffer of vlan tagged mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The ipv4 multicast vlan tagged mode as following:
 *      - VLAN_TAG_MODE_IPMC_ORIGINAL        (depend on chip normal decision)
 *      - VLAN_TAG_MODE_IPMC_KEEP_FORMAT     (keep ingress format to egress)
 *      - VLAN_TAG_MODE_IPMC_PRI             (always priority tag out)
 *      - VLAN_TAG_MODE_IPMC_DEFAUL          (as default tag mode setting)
 */
int32
rtk_vlan_tagModeIp4mc_get(rtk_port_t port, rtk_vlan_tagModeIpmc_t *pTagMode)
{
    int32   ret;
    if (NULL == RT_MAPPER->vlan_tagModeIp4mc_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->vlan_tagModeIp4mc_get( port, pTagMode);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_vlan_tagModeIp4mc_get */


/* Function Name:
 *      rtk_vlan_tagModeIp4mc_set
 * Description:
 *      Set vlan tagged mode for ipv4 multicast packet of the port.
 * Input:
 *      port     - port id
 *      tagMode  - vlan tagged mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      The ipv4 multicast vlan tagged mode as following:
 *      - VLAN_TAG_MODE_IPMC_ORIGINAL        (depend on chip normal decision)
 *      - VLAN_TAG_MODE_IPMC_KEEP_FORMAT     (keep ingress format to egress)
 *      - VLAN_TAG_MODE_IPMC_PRI             (always priority tag out)
 *      - VLAN_TAG_MODE_IPMC_DEFAUL          (as default tag mode setting)
 */
int32
rtk_vlan_tagModeIp4mc_set(rtk_port_t port, rtk_vlan_tagModeIpmc_t tagMode)
{
    int32   ret;
    if (NULL == RT_MAPPER->vlan_tagModeIp4mc_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->vlan_tagModeIp4mc_set( port, tagMode);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_vlan_tagModeIp4mc_set */

/* Function Name:
 *      rtk_vlan_tagModeIp6mc_get
 * Description:
 *      Get vlan tagged mode for ipv6 multicast packet of the port.
 * Input:
 *      port      - port id
 * Output:
 *      pTagMode  - pointer buffer of vlan tagged mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The ipv4 multicast vlan tagged mode as following:
 *      - VLAN_TAG_MODE_IPMC_ORIGINAL        (depend on chip normal decision)
 *      - VLAN_TAG_MODE_IPMC_KEEP_FORMAT     (keep ingress format to egress)
 *      - VLAN_TAG_MODE_IPMC_PRI             (always priority tag out)
 *      - VLAN_TAG_MODE_IPMC_DEFAUL          (as default tag mode setting)
 */
int32
rtk_vlan_tagModeIp6mc_get(rtk_port_t port, rtk_vlan_tagModeIpmc_t *pTagMode)
{
    int32   ret;
    if (NULL == RT_MAPPER->vlan_tagModeIp6mc_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->vlan_tagModeIp6mc_get( port, pTagMode);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_vlan_tagModeIp6mc_get */


/* Function Name:
 *      rtk_vlan_tagModeIp6mc_set
 * Description:
 *      Set vlan tagged mode for ipv6 multicast packet of the port.
 * Input:
 *      port     - port id
 *      tagMode  - vlan tagged mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      The ipv4 multicast vlan tagged mode as following:
 *      - VLAN_TAG_MODE_IPMC_ORIGINAL        (depend on chip normal decision)
 *      - VLAN_TAG_MODE_IPMC_KEEP_FORMAT     (keep ingress format to egress)
 *      - VLAN_TAG_MODE_IPMC_PRI             (always priority tag out)
 *      - VLAN_TAG_MODE_IPMC_DEFAUL          (as default tag mode setting)
 */
int32
rtk_vlan_tagModeIp6mc_set(rtk_port_t port, rtk_vlan_tagModeIpmc_t tagMode)
{
    int32   ret;
    if (NULL == RT_MAPPER->vlan_tagModeIp6mc_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->vlan_tagModeIp6mc_set( port, tagMode);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_vlan_tagModeIp6mc_set */


/* Function Name:
 *      rtk_vlan_lutSvlanHashState_set
 * Description:
 *      Packet from SVLAN aware port will hash by SVLAN id.
 * Input:
 *      vid              - vlan id
 *      enable           - state for svlan aware port lut hash force using svlan  
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 */
int32
rtk_vlan_lutSvlanHashState_set(
    rtk_vlan_t     vid,
    rtk_enable_t enable)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->vlan_lutSvlanHashState_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->vlan_lutSvlanHashState_set(vid, enable);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_vlan_lutSvlanHashState_set */


/* Function Name:
 *      rtk_vlan_lutSvlanHashState_get
 * Description:
 *      Packet from SVLAN aware port will hash by SVLAN id.
 * Input:
 *      vid              - vlan id
 * Output:
 *      pEnable          - pointer to svlan aware port SVLAN lut hash status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 */
int32
rtk_vlan_lutSvlanHashState_get(
    rtk_vlan_t     vid,
    rtk_enable_t *pEnable)
{
    int32   ret;

    /* function body */
    if (NULL == RT_MAPPER->vlan_lutSvlanHashState_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->vlan_lutSvlanHashState_get(vid, pEnable);
    RTK_API_UNLOCK();
    return ret;
}   /* end of rtk_vlan_lutSvlanHashState_get */

/* Function Name:
 *      rtk_vlan_extPortProtoVlan_get
 * Description:
 *      Get vlan of specificed protocol group on specified extension port.
 * Input:
 *      extPort 		- Extension port id
 *      protoGroup_idx 	- protocol group index
 * Output:
 *      pVlan_cfg      - pointer to vlan configuration of protocol group
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_OUT_OF_RANGE - protocol group index is out of range
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtk_vlan_extPortProtoVlan_get(
    uint32  	extPort,
    uint32      protoGroupIdx,
    rtk_vlan_protoVlanCfg_t *pVlanCfg)
{
    int32   ret;
    if (NULL == RT_MAPPER->vlan_extPortProtoVlan_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->vlan_extPortProtoVlan_get(extPort, protoGroupIdx, pVlanCfg);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_vlan_extPortProtoVlan_get */

/* Function Name:
 *      rtk_vlan_extPortProtoVlan_set
 * Description:
 *      Set vlan of specificed protocol group on specified extension port.
 * Input:
 *      extPort 		- Extension port id
 *      protoGroupIdx  	- protocol group index
 *      pVlan_cfg      - pointer to vlan configuration of protocol group
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_OUT_OF_RANGE - protocol group index is out of range
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      None
 */
int32
rtk_vlan_extPortProtoVlan_set(
    uint32  	extPort,
    uint32      protoGroupIdx,
    rtk_vlan_protoVlanCfg_t *pVlanCfg)
{
    int32   ret;
    if (NULL == RT_MAPPER->vlan_extPortProtoVlan_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->vlan_extPortProtoVlan_set(extPort, protoGroupIdx, pVlanCfg);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_vlan_extPortProtoVlan_set */

/* Function Name:
 *      rtk_vlan_extPortmaskIndex_get
 * Description:
 *      Get the vlan extension members index.
 * Input:
 *      vid              - vlan id
 * Output:
 *      pExtPmskIdx    - pointer buffer of extension member index
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtk_vlan_extPortmaskIndex_get(
    rtk_vlan_t     vid,
    uint32 			*pExtPmskIdx)
{
    int32   ret;
    if (NULL == RT_MAPPER->vlan_extPortmaskIndex_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->vlan_extPortmaskIndex_get( vid, pExtPmskIdx);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_vlan_extPortmaskIndex_get */


/* Function Name:
 *      rtk_vlan_extPortmaskIndex_set
 * Description:
 *      Replace the vlan extension members index.
 * Input:
 *      vid              	- vlan id
 *      extPmskIdx    	- extension member index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 */
int32
rtk_vlan_extPortmaskIndex_set(
    rtk_vlan_t 	vid,
    uint32 		extPmskIdx)
{
    int32   ret;
    if (NULL == RT_MAPPER->vlan_extPortmaskIndex_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->vlan_extPortmaskIndex_set( vid, extPmskIdx);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_vlan_extPortmaskIndex_set */


/* Function Name:
 *      rtk_vlan_extPortmaskCfg_get
 * Description:
 *      Get extension port mask of specificed entry
 * Input:
 *      extEntryIdx - extension port mask entry index
 * Output:
 *      rtk_portmask_t *pExt_portmask     - extension port mask
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     	- The module is not initial
 *      RT_ERR_INPUT 			-index is out of range
 *      RT_ERR_NULL_POINTER 	- input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtk_vlan_extPortmaskCfg_get(
    uint32                  extEntryIdx,
    rtk_portmask_t 			*pExt_portmask)
{
    int32   ret;
    if (NULL == RT_MAPPER->vlan_extPortmaskCfg_get)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->vlan_extPortmaskCfg_get(extEntryIdx, pExt_portmask);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_vlan_extPortmaskCfg_get */

/* Function Name:
 *      rtk_vlan_extPortmaskCfg_set
 * Description:
 *      Set extension port mask of specificed entry
 * Input:
 *      extEntryIdx 		- extension port mask entry index
 *      pExt_portmask     	- extension port mask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT 			- index is out of range
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtk_vlan_extPortmaskCfg_set(
    uint32                  extEntryIdx,
    rtk_portmask_t 			*pExt_portmask)
{
    int32   ret;
    if (NULL == RT_MAPPER->vlan_extPortmaskCfg_set)
        return RT_ERR_DRIVER_NOT_FOUND;
    RTK_API_LOCK();
    ret = RT_MAPPER->vlan_extPortmaskCfg_set(extEntryIdx, pExt_portmask);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_vlan_extPortmaskCfg_set */




