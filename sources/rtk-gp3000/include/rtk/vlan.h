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

#ifndef __RTK_VLAN_H__
#define __RTK_VLAN_H__


/*
 * Include Files
 */
#include <common/rt_type.h>

/*
 * Symbol Definition
 */
#define RTK_MBR_CFG_IDX_MAX  (31)

/* acceptable frame type of 802.1Q and 802.1ad */
typedef enum rtk_vlan_acceptFrameType_e
{
    ACCEPT_FRAME_TYPE_ALL = 0,          /* untagged, priority-tagged and tagged */
    ACCEPT_FRAME_TYPE_TAG_ONLY,         /* tagged                               */
    ACCEPT_FRAME_TYPE_UNTAG_ONLY,       /* untagged and priority-tagged         */
    ACCPET_FRAME_TYPE_1P_1Q_TAG_ONLY,   /* tagged packet, include VID 0 and 4095*/
    ACCEPT_FRAME_TYPE_END
} rtk_vlan_acceptFrameType_t;

/* tagged mode of VLAN - reference realtek private specification */
typedef enum rtk_vlan_tagMode_e
{
    VLAN_TAG_MODE_ORIGINAL = 0,
    VLAN_TAG_MODE_KEEP_FORMAT,
    VLAN_TAG_MODE_PRI,
    VLAN_TAG_MODE_END
} rtk_vlan_tagMode_t;

typedef enum rtk_vlan_tagModeIpmc_e
{
    VLAN_TAG_MODE_IPMC_ORIGINAL = 0,
    VLAN_TAG_MODE_IPMC_KEEP_FORMAT,
    VLAN_TAG_MODE_IPMC_PRI,
    VLAN_TAG_MODE_IPMC_DEFAULT,    
    VLAN_TAG_MODE_IPMC_END
} rtk_vlan_tagModeIpmc_t;




typedef enum rtk_fidMode_e
{
    VLAN_FID_IVL = 0,
    VLAN_FID_SVL,
    VLAN_FID_MODE_END
} rtk_fidMode_t;


typedef enum rtk_leaky_type_e
{
    LEAKY_BRG_GROUP = 0,
    LEAKY_FD_PAUSE,
    LEAKY_SP_MCAST,
    LEAKY_1X_PAE,
    LEAKY_UNDEF_BRG_04,
    LEAKY_UNDEF_BRG_05,
    LEAKY_UNDEF_BRG_06,
    LEAKY_UNDEF_BRG_07,
    LEAKY_PROVIDER_BRIDGE_GROUP_ADDRESS,
    LEAKY_UNDEF_BRG_09,
    LEAKY_UNDEF_BRG_0A,
    LEAKY_UNDEF_BRG_0B,
    LEAKY_UNDEF_BRG_0C,
    LEAKY_PROVIDER_BRIDGE_GVRP_ADDRESS,
    LEAKY_8021AB,
    LEAKY_UNDEF_BRG_0F,
    LEAKY_BRG_MNGEMENT,
    LEAKY_UNDEFINED_11,
    LEAKY_UNDEFINED_12,
    LEAKY_UNDEFINED_13,
    LEAKY_UNDEFINED_14,
    LEAKY_UNDEFINED_15,
    LEAKY_UNDEFINED_16,
    LEAKY_UNDEFINED_17,
    LEAKY_UNDEFINED_18,
    LEAKY_UNDEFINED_19,
    LEAKY_UNDEFINED_1A,
    LEAKY_UNDEFINED_1B,
    LEAKY_UNDEFINED_1C,
    LEAKY_UNDEFINED_1D,
    LEAKY_UNDEFINED_1E,
    LEAKY_UNDEFINED_1F,
    LEAKY_GMRP,
    LEAKY_GVRP,
    LEAKY_UNDEF_GARP_22,
    LEAKY_UNDEF_GARP_23,
    LEAKY_UNDEF_GARP_24,
    LEAKY_UNDEF_GARP_25,
    LEAKY_UNDEF_GARP_26,
    LEAKY_UNDEF_GARP_27,
    LEAKY_UNDEF_GARP_28,
    LEAKY_UNDEF_GARP_29,
    LEAKY_UNDEF_GARP_2A,
    LEAKY_UNDEF_GARP_2B,
    LEAKY_UNDEF_GARP_2C,
    LEAKY_UNDEF_GARP_2D,
    LEAKY_UNDEF_GARP_2E,
    LEAKY_UNDEF_GARP_2F,
    LEAKY_IGMP,
    LEAKY_IPMULTICAST,
    LEAKY_CDP,
    LEAKY_SSTP,
    LEAKY_END,
}rtk_leaky_type_t;

typedef enum rtk_vlan_keep_type_e
{
    KEEP_BRG_GROUP = 0,
    KEEP_FD_PAUSE,
    KEEP_SP_MCAST,
    KEEP_1X_PAE,
    KEEP_UNDEF_BRG_04,
    KEEP_UNDEF_BRG_05,
    KEEP_UNDEF_BRG_06,
    KEEP_UNDEF_BRG_07,
    KEEP_PROVIDER_BRIDGE_GROUP_ADDRESS,
    KEEP_UNDEF_BRG_09,
    KEEP_UNDEF_BRG_0A,
    KEEP_UNDEF_BRG_0B,
    KEEP_UNDEF_BRG_0C,
    KEEP_PROVIDER_BRIDGE_GVRP_ADDRESS,
    KEEP_8021AB,
    KEEP_UNDEF_BRG_0F,
    KEEP_BRG_MNGEMENT,
    KEEP_UNDEFINED_11,
    KEEP_UNDEFINED_12,
    KEEP_UNDEFINED_13,
    KEEP_UNDEFINED_14,
    KEEP_UNDEFINED_15,
    KEEP_UNDEFINED_16,
    KEEP_UNDEFINED_17,
    KEEP_UNDEFINED_18,
    KEEP_UNDEFINED_19,
    KEEP_UNDEFINED_1A,
    KEEP_UNDEFINED_1B,
    KEEP_UNDEFINED_1C,
    KEEP_UNDEFINED_1D,
    KEEP_UNDEFINED_1E,
    KEEP_UNDEFINED_1F,
    KEEP_GMRP,
    KEEP_GVRP,
    KEEP_UNDEF_GARP_22,
    KEEP_UNDEF_GARP_23,
    KEEP_UNDEF_GARP_24,
    KEEP_UNDEF_GARP_25,
    KEEP_UNDEF_GARP_26,
    KEEP_UNDEF_GARP_27,
    KEEP_UNDEF_GARP_28,
    KEEP_UNDEF_GARP_29,
    KEEP_UNDEF_GARP_2A,
    KEEP_UNDEF_GARP_2B,
    KEEP_UNDEF_GARP_2C,
    KEEP_UNDEF_GARP_2D,
    KEEP_UNDEF_GARP_2E,
    KEEP_UNDEF_GARP_2F,
    KEEP_IGMP,
    KEEP_IPMULTICAST,
    KEEP_CDP,
    KEEP_SSTP,
    KEEP_END,
}rtk_vlan_keep_type_t;

/* Content of protocol vlan group */
typedef struct rtk_vlan_protoGroup_s
{
    rtk_vlan_protoVlan_frameType_t  frametype;
    uint32                          framevalue;
} rtk_vlan_protoGroup_t;

/* configuration of each protocol vlan */
typedef struct rtk_vlan_protoVlanCfg_s
{
    uint32      valid;
    rtk_vlan_t  vid;
    rtk_pri_t   pri;
    uint32      dei;
} rtk_vlan_protoVlanCfg_t;


typedef struct  rtk_vlan_mbrcfg_s
{
    uint16  evid;
    uint16  mbr;
    uint16  fid_msti;
    uint16  envlanpol;
    uint16  meteridx;
    uint16  vbpen;
    uint16  vbpri;
}rtk_vlan_mbrcfg_t;

typedef enum rtk_vlan_tagKeepType_e
{
    TAG_KEEP_TYPE_CONTENT = 0,
    TAG_KEEP_TYPE_END
}rtk_vlan_tagKeepType_t;

typedef enum rtk_vlan_resVidAction_e
{
    RESVID_ACTION_UNTAG = 0,
    RESVID_ACTION_TAG,
    RESVID_ACTION_END
}
rtk_vlan_resVidAction_t;

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
extern int32
rtk_vlan_init(void);

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
extern int32
rtk_vlan_create(rtk_vlan_t vid);

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
extern int32
rtk_vlan_destroy(rtk_vlan_t vid);


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
extern int32
rtk_vlan_destroyAll(uint32 restoreDefaultVlan);

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
extern int32
rtk_vlan_fid_get(rtk_vlan_t vid, rtk_fid_t *pFid);


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
extern int32
rtk_vlan_fid_set(rtk_vlan_t vid, rtk_fid_t fid);

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
extern int32
rtk_vlan_fidMode_get(rtk_vlan_t vid, rtk_fidMode_t *pMode);

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
extern int32
rtk_vlan_fidMode_set(rtk_vlan_t vid, rtk_fidMode_t mode);

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
extern int32
rtk_vlan_port_get(
    rtk_vlan_t     vid,
    rtk_portmask_t *pMemberPortmask,
    rtk_portmask_t *pUntagPortmask);


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
extern int32
rtk_vlan_port_set(
    rtk_vlan_t     vid,
    rtk_portmask_t *pMember_portmask,
    rtk_portmask_t *pUntag_portmask);


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
extern int32
rtk_vlan_extPort_get(
    rtk_vlan_t     vid,
    rtk_portmask_t *pExt_portmask);


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
extern int32
rtk_vlan_extPort_set(
    rtk_vlan_t     vid,
    rtk_portmask_t *pExt_portmask);

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
extern int32
rtk_vlan_stg_get(rtk_vlan_t vid, rtk_stg_t *pStg);


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
extern int32
rtk_vlan_stg_set(rtk_vlan_t vid, rtk_stg_t stg);

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
extern int32
rtk_vlan_priority_get(rtk_vlan_t vid, rtk_pri_t *pPriority);

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
extern int32
rtk_vlan_priority_set(rtk_vlan_t vid, rtk_pri_t priority);

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
extern int32
rtk_vlan_priorityEnable_get(rtk_vlan_t vid, rtk_enable_t *pEnable);


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
extern int32
rtk_vlan_priorityEnable_set(rtk_vlan_t vid, rtk_enable_t enable);

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
extern int32
rtk_vlan_policingEnable_get(rtk_vlan_t vid, rtk_enable_t *pEnable);

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
extern int32
rtk_vlan_policingEnable_set(rtk_vlan_t vid, rtk_enable_t enable);

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
extern int32
rtk_vlan_policingMeterIdx_get(rtk_vlan_t vid, uint32 *pIndex);

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
extern int32
rtk_vlan_policingMeterIdx_set(rtk_vlan_t vid, uint32 index);

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
extern int32
rtk_vlan_portAcceptFrameType_get(
    rtk_port_t                 port,
    rtk_vlan_acceptFrameType_t *pAcceptFrameType);


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
extern int32
rtk_vlan_portAcceptFrameType_set(
    rtk_port_t                 port,
    rtk_vlan_acceptFrameType_t acceptFrameType);

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
extern int32
rtk_vlan_vlanFunctionEnable_get(rtk_enable_t *pEnable);


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
extern int32
rtk_vlan_vlanFunctionEnable_set(rtk_enable_t enable);

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
extern int32
rtk_vlan_portIgrFilterEnable_get(rtk_port_t port, rtk_enable_t *pEnable);


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
extern int32
rtk_vlan_portIgrFilterEnable_set(rtk_port_t port, rtk_enable_t enable);

/* Function Name:
 *      rtk_vlan_leaky_get
 * Description:
 *      Get VLAN leaky.
 * Input:
 *      leakyType - Packet type for VLAN leaky.
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
extern int32
rtk_vlan_leaky_get(rtk_leaky_type_t leakyType, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_vlan_leaky_set
 * Description:
 *      Set VLAN leaky.
 * Input:
 *      leakyType - Packet type for VLAN leaky.
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
extern int32
rtk_vlan_leaky_set(rtk_leaky_type_t leakyType, rtk_enable_t enable);

/* Function Name:
 *      rtk_vlan_portLeaky_get
 * Description:
 *      Get VLAN port-based leaky.
 * Input:
 *      port - port ID
 *      leakyType - Packet type for VLAN leaky.
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
extern int32
rtk_vlan_portLeaky_get(rtk_port_t port, rtk_leaky_type_t leakyType, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_vlan_portLeaky_set
 * Description:
 *      Set VLAN port-based leaky.
 * Input:
 *      port - port ID
 *      leakyType - Packet type for VLAN leaky.
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
extern int32
rtk_vlan_portLeaky_set(rtk_port_t port, rtk_leaky_type_t leakyType, rtk_enable_t enable);

/* Function Name:
 *      rtk_vlan_keepType_get
 * Description:
 *      Get VLAN keep format setting.
 * Input:
 *      keepType - Packet type for VLAN keep format.
 * Output:
 *      pEnable - Leaky status.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT 		- Invalid input parameters.
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 */
extern int32
rtk_vlan_keepType_get(rtk_vlan_keep_type_t keepType, rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_vlan_keepType_set
 * Description:
 *      Set VLAN keep format setting.
 * Input:
 *      keepType - Packet type for VLAN keep format.
 * Output:
 *      pEnable - Leaky status.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT 		- Invalid input parameters.
 * Note:
 */
extern int32
rtk_vlan_keepType_set(rtk_vlan_keep_type_t keepType, rtk_enable_t enable);

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
extern int32
rtk_vlan_portPvid_get(rtk_port_t port, uint32 *pPvid);


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
extern int32
rtk_vlan_portPvid_set(rtk_port_t port, uint32 pvid);


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
 *      Configuration on EXT port 0 will be get from physical port 6.
 */
extern int32
rtk_vlan_extPortPvid_get(uint32 extPort, uint32 *pPvid);


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
 *      Configuration on EXT port 0 will be applied to physical port 6.
 */
extern int32
rtk_vlan_extPortPvid_set(uint32 extPort, uint32 pvid);

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
extern int32
rtk_vlan_protoGroup_get(
    uint32                  protoGroupIdx,
    rtk_vlan_protoGroup_t   *pProtoGroup);

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
extern int32
rtk_vlan_protoGroup_set(
    uint32                  protoGroupIdx,
    rtk_vlan_protoGroup_t   *pProtoGroup);

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
extern int32
rtk_vlan_portProtoVlan_get(
    rtk_port_t              port,
    uint32                  protoGroupIdx,
    rtk_vlan_protoVlanCfg_t *pVlanCfg);

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
extern int32
rtk_vlan_portProtoVlan_set(
    rtk_port_t              port,
    uint32                  protoGroupIdx,
    rtk_vlan_protoVlanCfg_t *pVlanCfg);

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
extern int32
rtk_vlan_tagMode_get(rtk_port_t port, rtk_vlan_tagMode_t *pTagMode);


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
extern int32
rtk_vlan_tagMode_set(rtk_port_t port, rtk_vlan_tagMode_t tagMode);




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
extern int32
rtk_vlan_portFid_get(rtk_port_t port, rtk_enable_t *pEnable, rtk_fid_t *pFid);


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
extern int32
rtk_vlan_portFid_set(rtk_port_t port, rtk_enable_t enable, rtk_fid_t fid);

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
rtk_vlan_portPriority_get(rtk_port_t port, rtk_pri_t *pPriority);


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
extern int32
rtk_vlan_portPriority_set(rtk_port_t port, rtk_pri_t priority);

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
extern int32
rtk_vlan_portEgrTagKeepType_get(rtk_port_t egr_port, rtk_portmask_t *pIgr_portmask, rtk_vlan_tagKeepType_t *pType);

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
extern int32
rtk_vlan_portEgrTagKeepType_set(rtk_port_t egr_port, rtk_portmask_t *pIgr_portmask, rtk_vlan_tagKeepType_t type);


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
extern int32
rtk_vlan_transparentEnable_get(rtk_enable_t *pEnable);

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
extern int32
rtk_vlan_transparentEnable_set(rtk_enable_t enable);

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
extern int32
rtk_vlan_cfiKeepEnable_get(rtk_enable_t *pEnable);

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
extern int32
rtk_vlan_cfiKeepEnable_set(rtk_enable_t enable);

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
extern int32
rtk_vlan_reservedVidAction_get(rtk_vlan_resVidAction_t *pAction_vid0, rtk_vlan_resVidAction_t *pAction_vid4095);

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
extern int32
rtk_vlan_reservedVidAction_set(rtk_vlan_resVidAction_t action_vid0, rtk_vlan_resVidAction_t action_vid4095);

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
extern int32
rtk_vlan_tagModeIp4mc_get(rtk_port_t port, rtk_vlan_tagModeIpmc_t *pTagMode);

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
extern int32
rtk_vlan_tagModeIp4mc_set(rtk_port_t port, rtk_vlan_tagModeIpmc_t tagMode);

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
extern int32
rtk_vlan_tagModeIp6mc_get(rtk_port_t port, rtk_vlan_tagModeIpmc_t *pTagMode);

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
extern int32
rtk_vlan_tagModeIp6mc_set(rtk_port_t port, rtk_vlan_tagModeIpmc_t tagMode);

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
extern int32
rtk_vlan_lutSvlanHashState_set(
    rtk_vlan_t     vid,
    rtk_enable_t enable);


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
extern int32
rtk_vlan_lutSvlanHashState_get(
    rtk_vlan_t     vid,
    rtk_enable_t *pEnable);

/* Function Name:
 *      rtk_vlan_extPortProtoVlan_get
 * Description:
 *      Get vlan of specificed protocol group on specified extension port.
 * Input:
 *      extPort 		- Extension port id
 *      protoGroup_idx 	- protocol group index
 * Output:
 *      pVlanCfg       - vlan configuration of protocol group
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
extern int32
rtk_vlan_extPortProtoVlan_get(
    uint32  		extPort,
    uint32      	protoGroupIdx,
    rtk_vlan_protoVlanCfg_t *pVlanCfg);

/* Function Name:
 *      rtk_vlan_extPortProtoVlan_set
 * Description:
 *      Set vlan of specificed protocol group on specified extension port.
 * Input:
 *      extPort 		- Extension port id
 *      protoGroupIdx  	- protocol group index
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
 * Note:
 *      None
 */
extern int32
rtk_vlan_extPortProtoVlan_set(
    uint32  		extPort,
    uint32      	protoGroupIdx,
    rtk_vlan_protoVlanCfg_t *pVlanCfg);

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
extern int32
rtk_vlan_extPortmaskIndex_get(
    rtk_vlan_t     vid,
    uint32 			*pExtPmskIdx);

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
extern int32
rtk_vlan_extPortmaskIndex_set(
    rtk_vlan_t 	vid,
    uint32 		extPmskIdx);

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
extern int32
rtk_vlan_extPortmaskCfg_get(
    uint32                  extEntryIdx,
    rtk_portmask_t 			*pExt_portmask);


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
extern int32
rtk_vlan_extPortmaskCfg_set(
    uint32                  extEntryIdx,
    rtk_portmask_t 			*pExt_portmask);

#endif /* __RTK_VLAN_H__ */

