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
 *           1) vlan
 *
 */
#ifndef __DAL_MAPLE_VLAN_H__
#define __DAL_MAPLE_VLAN_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <rtk/vlan.h>
#include <dal/dal_mgmt.h>

/*
 * Symbol Definition
 */

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
 *      dal_maple_vlanMapper_init
 * Description:
 *      Hook vlan module of the specified device.
 * Input:
 *      pMapper - pointer of mapper
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      Must Hook vlan module before calling any vlan APIs.
 */
int32
dal_maple_vlanMapper_init(dal_mapper_t *pMapper);

/* Function Name:
 *      dal_maple_vlan_init
 * Description:
 *      Initialize vlan module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize vlan module before calling any vlan APIs.
 */
extern int32
dal_maple_vlan_init(uint32 unit);

/* Function Name:
 *      dal_maple_vlan_create
 * Description:
 *      Create the vlan in the specified device.
 * Input:
 *      unit - unit id
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
 *      1) Default FID/STG is assigned to CIST after vlan creation and
 *         FID/STG can be re-assigned later by dal_maple_vlan_stg_set
 *      2) Default lookup mode for L2 unicast and L2/IP multicast is assigned
 *         to be based on VID
 */
extern int32
dal_maple_vlan_create(uint32 unit, rtk_vlan_t vid);

/* Function Name:
 *      dal_maple_vlan_destroy
 * Description:
 *      Destroy the vlan in the specified device.
 * Input:
 *      unit - unit id
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
extern int32
dal_maple_vlan_destroy(uint32 unit, rtk_vlan_t vid);

/* Function Name:
 *      dal_maple_vlan_destroyAll
 * Description:
 *      Destroy all vlans except default vlan in the specified device.
 *      If restore_default_vlan is enable, default vlan will be restored.
 * Input:
 *      unit                 - unit id
 *      restore_default_vlan - keep or restore default vlan setting
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 * Note:
 *      The restore argument is permit following value:
 *      - 0: remove default vlan
 *      - 1: restore default vlan
 */
extern int32
dal_maple_vlan_destroyAll(uint32 unit, uint32 keep_and_restore_default_vlan);

/* Function Name:
 *      dal_maple_vlan_port_add
 * Description:
 *      Add one vlan member to the specified device.
 * Input:
 *      unit     - unit id
 *      vid      - vlan id
 *      port     - port id for add
 *      is_untag - untagged or tagged member
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_PORT_ID              - invalid port id
 *      RT_ERR_VLAN_PORT_MBR_EXIST  - member port exist in the specified vlan
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_OUT_OF_RANGE         - input parameter out of range
 * Note:
 *      1) The valid value of is_untag are {0: tagged, 1: untagged}
 */
extern int32
dal_maple_vlan_port_add(uint32 unit, rtk_vlan_t vid, rtk_port_t port, uint32 is_untag);

/* Function Name:
 *      dal_maple_vlan_port_del
 * Description:
 *      Delete one vlan member from the specified device.
 * Input:
 *      unit - unit id
 *      vid  - vlan id
 *      port - port id for delete
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_PORT_ID              - invalid port id
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 * Note:
 *      None
 */
extern int32
dal_maple_vlan_port_del(uint32 unit, rtk_vlan_t vid, rtk_port_t port);

/* Function Name:
 *      dal_maple_vlan_port_get
 * Description:
 *      Get the vlan members from the specified device.
 * Input:
 *      unit             - unit id
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
extern int32
dal_maple_vlan_port_get(
    uint32         unit,
    rtk_vlan_t     vid,
    rtk_portmask_t *pMember_portmask,
    rtk_portmask_t *pUntag_portmask);

/* Function Name:
 *      dal_maple_vlan_port_set
 * Description:
 *      Replace the vlan members in the specified device.
 * Input:
 *      unit             - unit id
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
 *      Replace with new specified VLAN member regardless of original setting
 */
extern int32
dal_maple_vlan_port_set(
    uint32         unit,
    rtk_vlan_t     vid,
    rtk_portmask_t *pMember_portmask,
    rtk_portmask_t *pUntag_portmask);

/* Function Name:
 *      dal_maple_vlan_stg_get
 * Description:
 *      Get spanning tree group instance of the vlan from the specified device.
 * Input:
 *      unit  - unit id
 *      vid   - vlan id
 * Output:
 *      pStg  - pointer buffer of spanning tree group instance
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 *      STG instance also represents FID in RTL8380
 */
extern int32
dal_maple_vlan_stg_get(uint32 unit, rtk_vlan_t vid, rtk_stg_t *pStg);

/* Function Name:
 *      dal_maple_vlan_stg_set
 * Description:
 *      Set spanning tree group instance of the vlan to the specified device.
 * Input:
 *      unit - unit id
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
 *      STG instance also represents FID in RTL8380
 */
extern int32
dal_maple_vlan_stg_set(uint32 unit, rtk_vlan_t vid, rtk_stg_t stg);

/* Function Name:
 *      dal_maple_vlan_l2LookupMode_get
 * Description:
 *      Get L2 lookup mode for L2  traffic.
 * Input:
 *      unit  - unit id
 *      vid   - vlan id
 *      type - dmac type
 * Output:
 *      pMode - lookup mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 *      Each VLAN can have its own lookup mode for L2  traffic
 */
extern int32
dal_maple_vlan_l2LookupMode_get(uint32 unit, rtk_vlan_t vid, rtk_vlan_l2mactype_t type, rtk_vlan_l2LookupMode_t *pMode);

/* Function Name:
 *      dal_maple_vlan_l2LookupMode_set
 * Description:
 *      Set L2 lookup mode for L2  traffic.
 * Input:
 *      unit  - unit id
 *      vid   - vlan id
 *      type - dmac type
 *      mode  - lookup mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_INPUT                - Invalid input parameter
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 * Note:
 *      Each VLAN can have its own lookup mode for L2 unicast traffic
 */
extern int32
dal_maple_vlan_l2LookupMode_set(uint32 unit, rtk_vlan_t vid, rtk_vlan_l2mactype_t type, rtk_vlan_l2LookupMode_t mode);
/* Function Name:
 *      dal_maple_vlan_profileIdx_get
 * Description:
 *      Get VLAN profile index of specified VLAN.
 * Input:
 *      unit - unit id
 *      vid  - vlan id
 * Output:
 *      pIdx - VLAN profile index
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 *      RT_ERR_NULL_POINTER         - input parameter may be null pointer
 * Note:
 *      Each VLAN can map to a VLAN profile (0~7)
 */
extern int32
dal_maple_vlan_profileIdx_get(uint32 unit, rtk_vlan_t vid, uint32 *pIdx);

/* Function Name:
 *      dal_maple_vlan_profileIdx_set
 * Description:
 *      Set VLAN profile index of specified VLAN.
 * Input:
 *      unit - unit id
 *      vid  - vlan id
 *      idx  - VLAN profile index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT             - The module is not initial
 *      RT_ERR_VLAN_VID             - invalid vid
 *      RT_ERR_OUT_OF_RANGE         - input value is out of range
 *      RT_ERR_VLAN_ENTRY_NOT_FOUND - specified vlan entry not found
 * Note:
 *      Each VLAN can map to a VLAN profile (0~7)
 */
extern int32
dal_maple_vlan_profileIdx_set(uint32 unit, rtk_vlan_t vid, uint32 idx);

/* Function Name:
 *      dal_maple_vlan_profile_get
 * Description:
 *      Get specific VLAN profile.
 * Input:
 *      unit     - unit id
 *      idx      - VLAN profile index
 * Output:
 *      pProfile - VLAN profile configuration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - profile index is out of range
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Totally 8 profile (0~7) are supported
 */
extern int32
dal_maple_vlan_profile_get(uint32 unit, uint32 idx, rtk_vlan_profile_t *pProfile);

/* Function Name:
 *      dal_maple_vlan_profile_set
 * Description:
 *      Set specific VLAN profile.
 * Input:
 *      unit     - unit id
 *      idx      - VLAN profile index
 *      pProfile - VLAN profile configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - profile index is out of range
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Totally 8 profile (0~7) are supported
 */
extern int32
dal_maple_vlan_profile_set(uint32 unit, uint32 idx, rtk_vlan_profile_t *pProfile);


/* Module Name     : vlan                */
/* Sub-module Name : vlan port attribute */

/* Function Name:
 *      dal_maple_vlan_portAcceptFrameType_get
 * Description:
 *      Get vlan accept frame type of the port from the specified device.
 * Input:
 *      unit               - unit id
 *      port               - port id
 *      type               - vlan type
 * Output:
 *      pAccept_frame_type - pointer buffer of accept frame type
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) The accept frame type as following:
 *          - ACCEPT_FRAME_TYPE_ALL
 *          - ACCEPT_FRAME_TYPE_TAG_ONLY
 *          - ACCEPT_FRAME_TYPE_UNTAG_ONLY
 *      (2) The API is used get accept frame type for 802.1Q or 802.1ad VLAN
 */
extern int32
dal_maple_vlan_portAcceptFrameType_get(
    uint32                     unit,
    rtk_port_t                 port,
    rtk_vlanType_t             type,
    rtk_vlan_acceptFrameType_t *pAccept_frame_type);

/* Function Name:
 *      rtk_vlan_portAcceptFrameType_set
 * Description:
 *      Set vlan accept frame type of the port to the specified device.
 * Input:
 *      unit              - unit id
 *      port              - port id
 *      accept_frame_type - accept frame type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT               - The module is not initial
 *      RT_ERR_PORT_ID                - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 *      RT_ERR_VLAN_ACCEPT_FRAME_TYPE - invalid accept frame type
 * Note:
 *      (1) The accept frame type as following:
 *          - ACCEPT_FRAME_TYPE_ALL
 *          - ACCEPT_FRAME_TYPE_TAG_ONLY
 *          - ACCEPT_FRAME_TYPE_UNTAG_ONLY
 *      (2) The API is used to set accept frame type for 802.1Q or 802.1ad VLAN
 */
extern int32
dal_maple_vlan_portAcceptFrameType_set(
    uint32                     unit,
    rtk_port_t                 port,
    rtk_vlanType_t             type,
    rtk_vlan_acceptFrameType_t accept_frame_type);

/* Function Name:
 *      dal_maple_vlan_portIgrFilter_get
 * Description:
 *      Get vlan ingress filter configuration of the port from the specified device.
 * Input:
 *      unit        - unit id
 *      port        - port id
 * Output:
 *      pIgr_filter - pointer buffer of ingress filter configuration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_maple_vlan_portIgrFilter_get(uint32 unit, rtk_port_t port, rtk_vlan_ifilter_t *pIgr_filter);

/* Function Name:
 *      dal_maple_vlan_portIgrFilter_set
 * Description:
 *      Set vlan ingress filter configuration of the port from the specified device.
 * Input:
 *      unit       - unit id
 *      port       - port id
 *      igr_filter - ingress filter configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_maple_vlan_portIgrFilter_set(uint32 unit, rtk_port_t port, rtk_vlan_ifilter_t igr_filter);

/* Function Name:
 *      dal_maple_vlan_portEgrFilterEnable_get
 * Description:
 *      Get enable status of egress filtering on specified port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to enable status of egress filtering
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_maple_vlan_portEgrFilterEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_maple_vlan_portEgrFilterEnable_set
 * Description:
 *      Set enable status of egress filtering on specified port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - enable status of egress filtering
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_maple_vlan_portEgrFilterEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      dal_maple_vlan_mcastLeakyEnable_get
 * Description:
 *      Get vlan egress leaky status of the system from the specified device.
 * Input:
 *      unit   - unit id
 * Output:
 *      pLeaky - pointer buffer of vlan leaky of egress
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Enable leaky function to allow L2/IP multicast traffic to across VLAN.
 *      That is, egress VLAN filtering is bypassed by L2/IP multicast traffic.
 */
extern int32
dal_maple_vlan_mcastLeakyEnable_get(uint32 unit, rtk_enable_t *pLeaky);

/* Function Name:
 *      dal_maple_vlan_mcastLeakyEnable_set
 * Description:
 *      Set vlan egress leaky configure of the system to the specified device.
 * Input:
 *      unit  - unit id
 *      leaky - vlan leaky of egress
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      Enable leaky function to allow L2/IP multicast traffic to across VLAN.
 *      That is, egress VLAN filtering is bypassed by L2/IP multicast traffic.
 */
extern int32
dal_maple_vlan_mcastLeakyEnable_set(uint32 unit, rtk_enable_t leaky);

/* Function Name:
 *      dal_maple_vlan_portPvidMode_get
 * Description:
 *      Get the configuration of  port-based VLAN mode.
 * Input:
 *      unit  - unit id
 *      port  - port id
 *      type - vlan type
 * Output:
 *      pMode - pointer to port-based VLAN mode configuration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Port-based VLAN can apply on different tag format, refer to rtk_vlan_pbVlan_mode_t
 */
extern int32
dal_maple_vlan_portPvidMode_get(uint32 unit, rtk_port_t port,rtk_vlanType_t type, rtk_vlan_pbVlan_mode_t *pMode);

/* Function Name:
 *      dal_maple_vlan_portPvidMode_set
 * Description:
 *      Set the configuration of port-based VLAN mode.
 * Input:
 *      unit - unit id
 *      port - port id
 *      type - vlan type
 *      mode - inner port-based VLAN mode configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      Port-based VLAN can apply on different tag format, refer to rtk_vlan_pbVlan_mode_t
 */
extern int32
dal_maple_vlan_portPvidMode_set(uint32 unit, rtk_port_t port, rtk_vlanType_t type, rtk_vlan_pbVlan_mode_t mode);


/* Function Name:
 *      dal_maple_vlan_portPvid_get
 * Description:
 *      Get port default vlan id from the specified device.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      type - vlan type
 * Output:
 *      pPvid  - pointer buffer of port default inner vlan id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_maple_vlan_portPvid_get(uint32 unit, rtk_port_t port,rtk_vlanType_t type, uint32 *pPvid);

/* Function Name:
 *      dal_maple_vlan_portPvid_set
 * Description:
 *      Set port default vlan id to the specified device.
 * Input:
 *      unit - unit id
 *      port - port id
 *      type - vlan type
 *      pvid - port default inner vlan id
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
extern int32
dal_maple_vlan_portPvid_set(uint32 unit, rtk_port_t port, rtk_vlanType_t type, uint32 pvid);

/* Function Name:
 *      dal_maple_vlan_portIgrVlanTransparentEnable_get
 * Description:
 *      Get enable status of keep tag format at ingress.
 * Input:
 *      unit       - unit id
 *      port      - port id
 *      type      - vlan type
 * Output:
 *      pEnable - enable status of keep tag format
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointers
 * Note:
 *      Packet will be transmitted with original tag format when following condition are true.
 *      - Enable keep tag format at ingress
 *      - Enable keep tag format at egress
 */
extern int32
dal_maple_vlan_portIgrVlanTransparentEnable_get(
    uint32          unit,
    rtk_port_t      port,
    rtk_vlanType_t type,
    rtk_enable_t    *pEnable);

/* Function Name:
 *      dal_maple_vlan_portIgrVlanTransparentEnable_set
 * Description:
 *      Set enable status of keep tag format at ingress.
 * Input:
 *      unit      - unit id
 *      port      - port id
 *      type      - vlan type
 *      enable  -  enable
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
 *      Packet will be transmitted with original tag format when following condition are true.
 *      - Enable keep tag format at ingress
 *      - Enable keep tag format at egress
 */
extern int32
dal_maple_vlan_portIgrVlanTransparentEnable_set(
    uint32          unit,
    rtk_port_t      port,
    rtk_vlanType_t type,
    rtk_enable_t    enable);

/* Function Name:
 *      dal_maple_vlan_portEgrVlanTransparentEnable_get
 * Description:
 *      Get enable status of keep tag format at egress.
 * Input:
 *      unit       - unit id
 *      port      - port id
 *      type      - vlan type
 * Output:
 *      pEnable - enable status of keep tag format
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointers
 * Note:
 *      Packet will be transmitted with original tag format when following condition are true.
 *      - Enable keep tag format at ingress
 *      - Enable keep tag format at egress
 */
extern int32
dal_maple_vlan_portEgrVlanTransparentEnable_get(
    uint32          unit,
    rtk_port_t      port,
    rtk_vlanType_t type,
    rtk_enable_t    *pEnable);

/* Function Name:
 *      dal_maple_vlan_portEgrVlanTransparentEnable_set
 * Description:
 *      Set enable status of keep tag format at egress.
 * Input:
 *      unit      - unit id
 *      port      - port id
 *      type      - vlan type
 *      enable  -  enable
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
 *      Packet will be transmitted with original tag format when following condition are true.
 *      - Enable keep tag format at ingress
 *      - Enable keep tag format at egress
 */
int32
dal_maple_vlan_portEgrVlanTransparentEnable_set(
    uint32          unit,
    rtk_port_t      port,
    rtk_vlanType_t type,
    rtk_enable_t    enable);

/* Function Name:
 *      dal_maple_vlan_portIgrTagKeepType_get
 * Description:
 *      Get keep type of keep tag format at ingress.
 * Input:
 *      unit           - unit id
 *      port           - port id
 * Output:
 *      pKeeptypeOuter - pointer to enable status of keep outer tag format
 *      pKeeptypeInner - pointer to enable status of keep inner tag format
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Packet will be transmitted with original tag format when following condition are true.
 *      - Enable keep tag format at ingress
 *      - Enable keep tag format at egress
 */
extern int32
dal_maple_vlan_portIgrTagKeepType_get(uint32 unit, rtk_port_t port, rtk_vlan_tagKeepType_t *pKeeptypeOuter, rtk_vlan_tagKeepType_t *pKeeptypeInner);

/* Function Name:
 *      dal_maple_vlan_portIgrTagKeepType_set
 * Description:
 *      Set keep type of keep tag format at ingress.
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      keeptypeOuter - keep type of keep outer tag format
 *      keeptypeInner - keep type of keep inner tag format
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      Packet will be transmitted with original tag format when following condition are true.
 *      - Set keep type of keep tag format at ingress
 *      - Set keep type of keep tag format at egress
 */
extern int32
dal_maple_vlan_portIgrTagKeepType_set(uint32 unit, rtk_port_t port, rtk_vlan_tagKeepType_t keeptypeOuter, rtk_vlan_tagKeepType_t keeptypeInner);

/* Function Name:
 *      dal_maple_vlan_portEgrTagKeepType_get
 * Description:
 *      Get keep type of keep tag format at egress.
 * Input:
 *      unit           - unit id
 *      port           - port id
 * Output:
 *      pKeeptypeOuter - pointer to enable status of keep outer tag format
 *      pKeeptypeInner - pointer to enable status of keep inner tag format
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Packet will be transmitted with original tag format when following condition are true.
 *      - Enable keep tag format at ingress
 *      - Enable keep tag format at egress
 */
extern int32
dal_maple_vlan_portEgrTagKeepType_get(uint32 unit, rtk_port_t port, rtk_vlan_tagKeepType_t *pKeeptypeOuter, rtk_vlan_tagKeepType_t *pKeeptypeInner);

/* Function Name:
 *      dal_maple_vlan_portEgrTagKeepType_set
 * Description:
 *      Set keep type of keep tag format at egress.
 * Input:
 *      unit          - unit id
 *      port          - port id
 *      keeptypeOuter - keep type of keep outer tag format
 *      keeptypeInner - keep type of keep inner tag format
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      Packet will be transmitted with original tag format when following condition are true.
 *      - Set keep type of keep tag format at ingress
 *      - Set keep type of keep tag format at egress
 */
extern int32
dal_maple_vlan_portEgrTagKeepType_set(uint32 unit, rtk_port_t port, rtk_vlan_tagKeepType_t keeptypeOuter, rtk_vlan_tagKeepType_t keeptypeInner);

/* Function Name:
 *      dal_maple_vlan_egrVlanCnvtDblTagEnable_get
 * Description:
 *      Enable egress VLAN conversion for double(outer+inner) tagged packet.
 * Input:
 *      unit    - unit id
 * Output:
 *      pEnable - pointer to enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_maple_vlan_egrVlanCnvtDblTagEnable_get(uint32 unit, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_maple_vlan_egrVlanCnvtDblTagEnable_set
 * Description:
 *      Enable egress VLAN conversion for double(outer+inner) tagged packet.
 * Input:
 *      unit   - unit id
 *      enable - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_maple_vlan_egrVlanCnvtDblTagEnable_set(uint32 unit, rtk_enable_t enable);

/* Function Name:
 *      dal_maple_vlan_egrVlanCnvtTblPowerEnable_get
 * Description:
 *      Get egress vlan conversion table power enable status.
 * Input:
 *      unit    - unit id
 * Output:
 *      pEnable - pointer to enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_maple_vlan_egrVlanCnvtTblPowerEnable_get(uint32 unit, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_maple_vlan_egrVlanCnvtTblPowerEnable_set
 * Description:
 *      Set egress vlan conversion table power enable status.
 * Input:
 *      unit   - unit id
 *      enable - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_maple_vlan_egrVlanCnvtTblPowerEnable_set(uint32 unit, rtk_enable_t enable);

/* Function Name:
 *      dal_maple_vlan_egrVlanCnvtVidSource_get
 * Description:
 *      Specify the VID source for doing egress VLAN conversion.
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pSrc - pointer to VID source
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_maple_vlan_portEgrVlanCnvtVidSource_get(uint32 unit, rtk_port_t port, rtk_l2_vlanMode_t *pSrc);

/* Function Name:
 *      dal_maple_vlan_egrVlanCnvtVidSource_set
 * Description:
 *      Specify the VID source for doing egress VLAN conversion.
 * Input:
 *      unit - unit id
 *      port - port id
 *      src  - VID source
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_maple_vlan_portEgrVlanCnvtVidSource_set(uint32 unit, rtk_port_t port, rtk_l2_vlanMode_t src);

/* Function Name:
 *      dal_maple_vlan_egrVlanCnvtVidTarget_get
 * Description:
 *      Specify the VID target for doing egress VLAN conversion.
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pTgt - pointer to VID source
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_maple_vlan_portEgrVlanCnvtVidTarget_get(uint32 unit, rtk_port_t port, rtk_l2_vlanMode_t *pTgt);

/* Function Name:
 *      dal_maple_vlan_egrVlanCnvtVidTarget_set
 * Description:
 *      Specify the VID target for doing egress VLAN conversion.
 * Input:
 *      unit - unit id
 *      port - port id
 *      tgt  - VID source
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_maple_vlan_portEgrVlanCnvtVidTarget_set(uint32 unit, rtk_port_t port, rtk_l2_vlanMode_t tgt);

/* Function Name:
 *      dal_maple_vlan_portEgrVlanCnvtLookupMissAct_get
 * Description:
 *      Specify egress port VID conversion table lookup miss action.
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pAct - pointer to action selection
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_maple_vlan_portEgrVlanCnvtLookupMissAct_get(uint32 unit, rtk_port_t port, rtk_vlan_lookupMissAct_t *pAct);

/* Function Name:
 *      dal_maple_vlan_portEgrVlanCnvtLookupMissAct_set
 * Description:
 *      Specify egress port VID conversion table lookup miss action.
 * Input:
 *      unit - unit id
 *      port - port id
 *      act  - action selection
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_maple_vlan_portEgrVlanCnvtLookupMissAct_set(uint32 unit, rtk_port_t port, rtk_vlan_lookupMissAct_t act);

/* Function Name:
 *      dal_maple_vlan_egrVlanCnvtEntry_get
 * Description:
 *      Get egress VLAN conversion (SC2C) entry.
 * Input:
 *      unit  - unit id
 *      index - entry index
 * Output:
 *      pData - configuration of egress VLAN conversion (SC2C) entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - entry index is out of range
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_maple_vlan_egrVlanCnvtEntry_get(uint32 unit, uint32 index, rtk_vlan_egrVlanCnvtEntry_t *pData);

/* Function Name:
 *      dal_maple_vlan_egrVlanCnvtEntry_set
 * Description:
 *      Set egress VLAN conversion (SC2C) entry.
 * Input:
 *      unit  - unit id
 *      index - entry index
 * Output:
 *      pData - configuration of egress VLAN conversion (SC2C) entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT        - The module is not initial
 *      RT_ERR_OUT_OF_RANGE    - entry index is out of range
 *      RT_ERR_NULL_POINTER    - input parameter may be null pointer
 *      RT_ERR_VLAN_VID        - invalid vid
 *      RT_ERR_PORT_ID         - invalid port id
 *      RT_ERR_QOS_1P_PRIORITY - Invalid 802.1p priority
 *      RT_ERR_VLAN_TPID_INDEX - Invalid TPID index
 *      RT_ERR_INPUT           - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_maple_vlan_egrVlanCnvtEntry_set(uint32 unit, uint32 index, rtk_vlan_egrVlanCnvtEntry_t *pData);

/* Function Name:
 *      dal_maple_vlan_tpidEntry_get
 * Description:
 *      Get TPID value from global  TPID pool.
 * Input:
 *      unit     - unit id
 *      type      - vlan tag type
 *      tpid_idx - index of TPID entry
 * Output:
 *      pTpid    - pointer to TPID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Global four inner TPID can be specified.
 *      Global four outer TPID can be specified
 *      Global one extra TPID can be specified
 */
extern int32
dal_maple_vlan_tpidEntry_get(uint32 unit, rtk_vlan_tagType_t type, uint32 tpid_idx, uint32 *pTpid);

/* Function Name:
 *      dal_maple_vlan_tpidEntry_set
 * Description:
 *      Set TPID value to global TPID pool.
 * Input:
 *      unit     - unit id
 *      type      - vlan tag type
 *      tpid_idx - index of TPID entry
 *      tpid     - TPID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *      Global four inner TPID can be specified.
 *      Global four outer TPID can be specified
 *      Global one extra TPID can be specified
 */
extern int32
dal_maple_vlan_tpidEntry_set(uint32 unit, rtk_vlan_tagType_t type, uint32 tpid_idx, uint32 tpid);

/* Function Name:
 *      dal_maple_vlan_portIgrTpid_get
 * Description:
 *      Get inner/outer TPIDs comparsion configration on specified port.
 * Input:
 *      unit     - unit id
 *      port    - port id
 *      type    -   vlan type
 * Output:
 *      pTpid_idx_mask - pointer to mask for index of tpid entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Specify which TPID to compare from TPID pool for parsing a inner/outer-tagged packet.
 *      The valid mask bits of tpid_idx is bit[3:0].
 */
extern int32
dal_maple_vlan_portIgrTpid_get(uint32 unit, rtk_port_t port, rtk_vlanType_t type, uint32 *pTpid_idx_mask);

/* Function Name:
 *      dal_maple_vlan_portIgrTpid_set
 * Description:
 *      Set inner/outer TPIDs comparsion configration on specified port
 * Input:
 *      unit     - unit id
 *      port    - port id
 *      type    -   vlan type
 *      tpid_idx_mask - mask for index of tpid entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      Specify which TPID to compare from TPID pool for parsing a inner/outer-tagged packet.
 *      The valid mask bits of tpid_idx is bit[3:0].
 */
extern int32
dal_maple_vlan_portIgrTpid_set(uint32 unit, rtk_port_t port, rtk_vlanType_t type, uint32 tpid_idx_mask);


/* Function Name:
 *      dal_maple_vlan_portIgrExtraTagEnable_get
 * Description:
 *      Get enable state of extra tag comparsion.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to the extra tag comparsion state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Only one extra tag is supported, refer to rtk_vlan_extraTpidEntry_set
 */
extern int32
dal_maple_vlan_portIgrExtraTagEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_maple_vlan_portIgrExtraTagEnable_set
 * Description:
 *      Enable extra tag comparsion.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - extra tag comparsion state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      Only one extra tag is supported, refer to rtk_vlan_extraTpidEntry_set
 */
extern int32
dal_maple_vlan_portIgrExtraTagEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      dal_maple_vlan_portEgrTagSts_get
 * Description:
 *      Get tag  status of egress port.
 * Input:
 *      unit - unit id
 *      port - port id
 *      type - vlan type
 * Output:
 *      pSts - tag status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_maple_vlan_portEgrTagSts_get(uint32 unit, rtk_port_t port, rtk_vlanType_t type, rtk_vlan_tagSts_t *pSts);

/* Function Name:
 *      dal_maple_vlan_portEgrTagSts_set
 * Description:
 *      Set tag status of egress port.
 * Input:
 *      unit - unit id
 *      port - port id
 *      type - vlan type
 *      sts  - tag status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_maple_vlan_portEgrTagSts_set(uint32 unit, rtk_port_t port, rtk_vlanType_t type,rtk_vlan_tagSts_t sts);

/* Function Name:
 *      dal_maple_vlan_portEgrTpid_get
 * Description:
 *      Get inner/outer TPID for inner/outer tag encapsulation when transmiiting a inner/outer-tagged pacekt.
 * Input:
 *      unit      - unit id
 *      port      - port id
 * Output:
 *      pTpid_idx - pointer to index of inner TPID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_maple_vlan_portEgrTpid_get(uint32 unit, rtk_port_t port,rtk_vlanType_t type, uint32 *pTpid_idx);

/* Function Name:
 *      dal_maple_vlan_portEgrTpid_set
 * Description:
 *      Set inner/outer TPID for inner/outer tag encapsulation when transmiiting a inner/outer-tagged pacekt.
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      tpid_idx - index of inner TPID
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *      None
 */
extern int32
dal_maple_vlan_portEgrTpid_set(uint32 unit, rtk_port_t port,rtk_vlanType_t type, uint32 tpid_idx);

/* Function Name:
 *      dal_maple_vlan_leakyStpFilter_get
 * Description:
 *      Get leaky STP filter status for multicast leaky is enabled.
 * Input:
 *      unit    - unit id
 * Output:
 *      pEnable - pointer to status of leaky STP filter
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_maple_vlan_leakyStpFilter_get(const uint32 unit, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_maple_vlan_leakyStpFilter_set
 * Description:
 *      Set leaky STP filter status for multicast leaky is enabled.
 * Input:
 *      unit   - unit id
 *      enable - status of leaky STP filter
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_maple_vlan_leakyStpFilter_set(const uint32 unit, const rtk_enable_t enable);

/* Function Name:
 *      dal_maple_vlan_portVlanAggrEnable_get
 * Description:
 *      Enable N:1 VLAN aggregation support on egress port.
 * Input:
 *      unit    - unit id
 *      port    - port id
 * Output:
 *      pEnable - pointer to enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_maple_vlan_portVlanAggrEnable_get(uint32 unit, rtk_port_t port, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_maple_vlan_portVlanAggrEnable_set
 * Description:
 *      Enable N:1 VLAN aggregation support on egress port.
 * Input:
 *      unit   - unit id
 *      port   - port id
 *      enable - enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_maple_vlan_portVlanAggrEnable_set(uint32 unit, rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      dal_maple_vlan_portVlanAggrCtrl_get
 * Description:
 *      Get port vlan-aggragation configration.
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      pVlanAggrCtrl - pointer to vlan-aggr ctrl
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_maple_vlan_portVlanAggrCtrl_get(uint32 unit, rtk_port_t port, rtk_vlan_aggrCtrl_t *pVlanAggrCtrl);

/* Function Name:
 * Description:
 *      Set port vlan-aggragation configration.
 * Input:
 *      unit - unit id
 *      port - port id
 * Output:
 *      vlanAggrCtrl - pointer to vlan-aggr ctrl
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_maple_vlan_portVlanAggrCtrl_set(uint32 unit, rtk_port_t port, rtk_vlan_aggrCtrl_t vlanAggrCtrl);

/* Function Name:
 *      dal_maple_vlan_portEgrVlanCnvtLuMisAct_get
 * Description:
 *      Get action for packet that doesn't hit any Egress VLAN Conversion (EVC) entry.
 * Input:
 *      unit    - unit id
 *      port   - port id for configure
 *      type   - vlan type
 * Output:
 *      pAction - pointer to VLAN conversion default action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      Forwarding action is as following
 *      - ACTION_DROP
 *      - ACTION_FORWARD
 */
extern int32
dal_maple_vlan_portEgrVlanCnvtLuMisAct_get(
    uint32                                unit,
    rtk_port_t                            port,
    rtk_vlanType_t                       type,
    rtk_vlan_lookupMissAct_t          *pAction);

/* Function Name:
 *      dal_maple_vlan_portEgrVlanCnvtLuMisAct_set
 * Description:
 *      Set action for packet that doesn't hit Egress VLAN Conversion (EVC) entry.
 * Input:
 *      unit   - unit id
 *      port   - port id for configure
 *      type  - vlan type
 *      action - VLAN conversion default action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      Forwarding action is as following
 *      - ACTION_DROP
 *      - ACTION_FORWARD
 */
extern int32
dal_maple_vlan_portEgrVlanCnvtLuMisAct_set(
    uint32                               unit,
    rtk_port_t                           port,
    rtk_vlanType_t                      type,
    rtk_vlan_lookupMissAct_t           action);

/* Function Name:
 *      dal_maple_vlan_egrVlanCnvtEntry_delAll
 * Description:
 *      Delete all egress VLAN conversion entry.
 * Input:
 *      unit  - unit id
 *      index - entry index
 * Output:
 *      pData - configuration of egress VLAN conversion entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 * Note:
 *      None
 */
extern int32
dal_maple_vlan_egrVlanCnvtEntry_delAll(uint32 unit);

/* Function Name:
 *      dal_maple_vlan_egrVlanCnvtRangeCheckEntry_get
 * Description:
 *      Get the configuration of VID range check for ingress VLAN conversion.
 * Input:
 *      unit  - unit id
 *      setIdx - vlan range check set index
 *      index - entry index
 * Output:
 *      pData - configuration of VID Range
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_maple_vlan_egrVlanCnvtRangeCheckEntry_get(
    uint32                                      unit,
    uint32                                      setIdx,
    uint32                                      index,
    rtk_vlan_vlanCnvtRangeCheck_vid_t          *pData);

/* Function Name:
 *      dal_maple_vlan_egrVlanCnvtRangeCheckEntry_set
 * Description:
 *      Set the configuration of VID range check for egress VLAN conversion.
 * Input:
 *      unit  - unit id
 *      setIdx - vlan range check set index
 *      index - entry index
 *      pData - configuration of Vid range
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_ENTRY_INDEX  - invalid entry index
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_VLAN_VID             - invalid vid
 * Note:
 *      None
 */
extern int32
dal_maple_vlan_egrVlanCnvtRangeCheckEntry_set(
    uint32                                  unit,
    uint32                                  setIdx,
    uint32                                  index,
    rtk_vlan_vlanCnvtRangeCheck_vid_t    *pData);

/* Function Name:
 *      dal_maple_vlan_portFwdVlan_get
 * Description:
 *      Get forwarding vlan(inner/outer vlan)  on specified port.
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      tagType    - packet tag type
 * Output:
 *      pVlanType -  pointer to inner/outer vlan
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *
 */
extern int32
dal_maple_vlan_portFwdVlan_get(
    uint32                          unit,
    rtk_port_t                      port,
    rtk_vlan_pktTagMode_t        tagMode,
    rtk_vlanType_t            *pVlanType);


/* Function Name:
 *      dal_maple_vlan_portFwdVlan_set
 * Description:
 *      Set forwarding vlan(inner/outer vlan)  on specified port.
 * Input:
 *      unit     - unit id
 *      port     - port id
 *      tagType    - packet tag type
 *      vlanType - inner/outer vlan
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *
 */
extern int32
dal_maple_vlan_portFwdVlan_set(
    uint32                          unit,
    rtk_port_t                      port,
    rtk_vlan_pktTagMode_t        tagMode,
    rtk_vlanType_t             vlanType);

#endif /* __DAL_MAPLE_VLAN_H__ */

