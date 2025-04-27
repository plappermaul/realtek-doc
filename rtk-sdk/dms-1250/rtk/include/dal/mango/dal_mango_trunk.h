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
 * Purpose : Definition those public TRUNK APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) Trunk
 *
 */
#ifndef __DAL_MANGO_TRUNK_H__
#define __DAL_MANGO_TRUNK_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <rtk/trunk.h>
#include <dal/dal_mapper.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
typedef int32 (*rtk_trunk_mon_callback_t)(int32 unit, rtk_trk_t trkId, rtk_trk_egrPort_t *pTrk_egr_ports);

/*
 * Macro Definition
 */


/*
 * Function Declaration
 */

/* Module Name : TRUNK */

extern int32 _dal_mango_trunk_semLock(uint32 unit);

extern int32 _dal_mango_trunk_semUnlock(uint32 unit);

/* Function Name:
 *      dal_mango_trunkMapper_init
 * Description:
 *      Hook trunk module of the specified device.
 * Input:
 *      pMapper - pointer of mapper
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      Must Hook trunk module before calling any trunk APIs.
 */
extern int32
dal_mango_trunkMapper_init(dal_mapper_t *pMapper);

/* Function Name:
 *      dal_mango_trunk_init
 * Description:
 *      Initialize trunk module of the specified device.
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize trunk module before calling any trunk APIs.
 */
extern int32
dal_mango_trunk_init(uint32 unit);

/* Function Name:
 *      dal_mango_trunk_mode_get
 * Description:
 *      Get the trunk mode from the specified device.
 * Input:
 *      unit  - unit id
 * Output:
 *      pMode - pointer buffer of trunk mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The enum of the trunk mode as following
 *      - TRUNK_MODE_STACKING
 *      - TRUNK_MODE_STANDALONE
 */
extern int32
dal_mango_trunk_mode_get (uint32 unit, rtk_trunk_mode_t *pMode);

/* Function Name:
 *      dal_mango_trunk_mon_register
 * Description:
 *      Register callback function for trunk change notification
 * Input:
 *      unit  - unit id
 *      trunkMon_callback    - callback function for trunk change
 * Output:
 *      None
 * Return:
 *      RT_ERR_FAILED   - initialize fail
 *      RT_ERR_OK       - initialize success
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *
 */
extern int32
dal_mango_trunk_mon_register(uint32 unit, rtk_trunk_mon_callback_t trunkMon_callback);

/* Function Name:
 *      dal_mango_trunk_mode_set
 * Description:
 *      Set the trunk mode to the specified device.
 * Input:
 *      unit - unit id
 *      mode - trunk mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID  - invalid unit id
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      The enum of the trunk mode as following
*       - TRUNK_MODE_STACKING
 *      - TRUNK_MODE_STANDALONE
 */
extern int32
dal_mango_trunk_mode_set(uint32 unit, rtk_trunk_mode_t mode);

/*
 * Function Declaration
 *      dal_mango_trunk_distributionAlgorithmTypeBind_get
 * Description:
 *      Get the distribution algorithm ID binded for packet types for a trunk group from the specified device.
 * Input:
 *      unit      - unit id
 *      trk_gid   - trunk group id
 *      type - bind packet type
 * Output:
 *      pAlgo_idx - pointer buffer of the distribution algorithm ID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_LA_TRUNK_ID  - invalid trunk ID
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_mango_trunk_distributionAlgorithmTypeBind_get(uint32 unit, rtk_trk_t trk_gid, rtk_trunk_bindType_t type, uint32 *pAlgo_idx);

/* Function Name:
 *      dal_mango_trunk_distributionAlgorithmBind_set
 * Description:
 *      Set the distribution algorithm ID binded for packet types for a trunk group from the specified device.
 * Input:
 *      unit     - unit id
 *      trk_gid  - trunk group id
 *      type - bind packet type
 *      algo_idx - index the distribution algorithm
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID     - invalid unit id
 *      RT_ERR_NOT_INIT    - The module is not initial
 *      RT_ERR_LA_TRUNK_ID - invalid trunk ID
 *      RT_ERR_LA_ALGO_ID  - invalid trunk algorithm ID
 * Note:
 *      None
 */
extern int32
dal_mango_trunk_distributionAlgorithmTypeBind_set(uint32 unit, rtk_trk_t trk_gid, rtk_trunk_bindType_t type, uint32 algo_idx);

/* Function Name:
 *      dal_mango_trunk_distributionAlgorithmTypeParam_get
 * Description:
 *      Get the distribution algorithm by packet type and algorithm ID from the specified device.
 * Input:
 *      unit          - unit id
 *      type         - packet type
 *      algo_idx    - algorithm index
 * Output:
 *      pAlgo_bitmask - pointer buffer of bitmask of the distribution algorithm
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_LA_ALGO_ID   - invalid trunk algorithm ID
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *
 */
extern int32
dal_mango_trunk_distributionAlgorithmTypeParam_get(uint32 unit, uint32 algo_idx, rtk_trunk_hashParamType_t type, uint32 *pAlgo_bitmask);

/* Function Name:
 *      dal_mango_trunk_distributionAlgorithmTypeParam_set
 * Description:
 *      Set the distribution algorithm by packet type and algorithm ID from the specified device.
 * Input:
 *      unit         - unit id
 *      type         - packet type
 *      algo_idx     - algorithm index
 *      algo_bitmask - bitmask of the distribution algorithm
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID     - invalid unit id
 *      RT_ERR_NOT_INIT    - The module is not initial
 *      RT_ERR_LA_ALGO_ID  - invalid trunk algorithm ID
 *      RT_ERR_LA_HASHMASK - invalid hash mask
 * Note:
 *      (1) The valid range of algo_idx is 0~1 in 9300 and 9310.
 *      (2) Following factors can be used in any combination to customize the L2 distribution algorithm.
 *          - TRUNK_DISTRIBUTION_ALGO_L2_SPA_BIT        (source port)
 *          - TRUNK_DISTRIBUTION_ALGO_L2_SMAC_BIT       (source mac)
 *          - TRUNK_DISTRIBUTION_ALGO_L2_DMAC_BIT       (destination mac)
 *          - TRUNK_DISTRIBUTION_ALGO_L2_VLAN_BIT        (vlan id)
 *      (3) Following factors can be used in any combination to customize the L3 distribution algorithm.
 *          - TRUNK_DISTRIBUTION_ALGO_L3_SPA_BIT        (source port)
 *          - TRUNK_DISTRIBUTION_ALGO_L3_SMAC_BIT       (source mac)
 *          - TRUNK_DISTRIBUTION_ALGO_L3_DMAC_BIT       (destination mac)
 *          - TRUNK_DISTRIBUTION_ALGO_L3_VLAN_BIT        (vlan id)
 *          - TRUNK_DISTRIBUTION_ALGO_L3_SIP_BIT        (source ip)
 *          - TRUNK_DISTRIBUTION_ALGO_L3_DIP_BIT        (destination ip)
 *          - TRUNK_DISTRIBUTION_ALGO_L3_SRC_L4PORT_BIT (source layer4 port)
 *          - TRUNK_DISTRIBUTION_ALGO_L3_DST_L4PORT_BIT (destination layer4 port)
 *          - TRUNK_DISTRIBUTION_ALGO_L3_PROTO_BIT (protocol)
 *          - TRUNK_DISTRIBUTION_ALGO_L3_FLOW_LABEL_BIT (ipv6 flow label)
 */
extern int32
dal_mango_trunk_distributionAlgorithmTypeParam_set(uint32 unit, uint32 algo_idx, rtk_trunk_hashParamType_t type, uint32 algo_bitmask);

/* Function Name:
 *      dal_mango_trunk_distributionAlgorithmShift_get
 * Description:
 *      Get the shift bits of distribution algorithm parameters from the specified device.
 * Input:
 *      unit     - unit id
 *      algo_idx - algorithm index
 * Output:
 *      pShift   - pointer buffer of shift bits of the distribution algorithm
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_LA_ALGO_ID   - invalid trunk algorithm ID
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
extern int32
dal_mango_trunk_distributionAlgorithmShiftGbl_get(uint32 unit,  rtk_trunk_distAlgoShift_t *pShift);

/* Function Name:
 *      dal_mango_trunk_distributionAlgorithmShift_set
 * Description:
 *      Set the shift bits of distribution algorithm parameters from the specified device.
 * Input:
 *      unit     - unit id
 *      algo_idx - algorithm index
 *      pShift   - shift bits of the distribution algorithm parameters
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_LA_ALGO_ID    - invalid trunk algorithm ID
 *      RT_ERR_LA_ALGO_SHIFT - invalid trunk algorithm shift
 *      RT_ERR_NULL_POINTER  - input parameter may be null pointer
 * Note:
 */
extern int32
dal_mango_trunk_distributionAlgorithmShiftGbl_set(uint32 unit, rtk_trunk_distAlgoShift_t *pShift);

/* Function Name:
 *      dal_mango_trunk_trafficSeparateEnable_get
 * Description:
 *      Get the traffic separation setting of a trunk group from the specified device.
 * Input:
 *      unit           - unit id
 *      trk_gid        - trunk group id
 * Output:
 *      pSeparateType      - pointer to traffic separation type
 * Return:
*      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_LA_TRUNK_ID  - invalid trunk ID
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) The configurable separation type includes SEPARATE_KNOWN_MULTI, SEPARATE_DLF, and SEPARATE_BCAST.
 *      (2) In 9310 stand-alone mode, the separated port is the link up egress member port with largest index.
 *      (3) In 9310 stacking mode, the separated port could be configured by rtk_trunk_trafficSeparatePort_set().
 */
extern int32
dal_mango_trunk_trafficSeparateEnable_get(uint32 unit, rtk_trk_t trk_gid, rtk_trunk_separateType_t separateType, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_mango_trunk_trafficSeparateEnable_set
 * Description:
 *      Set the traffic separation setting of a trunk group from the specified device.
 * Input:
 *      unit         - unit id
 *      trk_gid      - trunk group id
 *      separateType     - traffic separation setting
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_LA_TRUNK_ID  - invalid trunk ID
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) The configurable separation type includes SEPARATE_KNOWN_MULTI, SEPARATE_DLF, and SEPARATE_BCAST.
 *      (2) In 9310 stand-alone mode, the separated port is the link up egress member port with largest index.
 *      (2) In 9310 stacking mode, the separated port could be configured by dal_mango_trunk_trafficSeparatePort_set().
 */
extern int32
dal_mango_trunk_trafficSeparateEnable_set(uint32 unit, rtk_trk_t trk_gid, rtk_trunk_separateType_t separateType, rtk_enable_t enable);

/* Function Name:
 *      dal_mango_trunk_trafficSeparateDivision_get
 * Description:
 *      Get the division control of known multicast to other separated packets from the specified device.
 * Input:
 *      unit          - unit id
 * Output:
 *      pEnable    - pointer buffer of known multicast packets separation divide control enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) This function only works when SEPARATE_KNOWN_MULTI and SEPARATE_FLOOD are both enabled.
 *           In that case, if the division function is enabled, separated DLF or broadcast packet would choose trunk member ports with
 *           largest index to forward; separated known multicast packets would choose second largest index to forward.
 *      (2) This function only works when at least 3 trunk member ports link up.
 */
extern int32
dal_mango_trunk_trafficSeparateDivision_get(uint32 unit, rtk_enable_t *pEnable);


/* Function Name:
 *      dal_mango_trunk_trafficSeparateDivision_set
 * Description:
 *      Set the division control of known multicast to other separated packets to the specified device.
 * Input:
 *      unit        - unit id
 *      enable    - known multicast packets separation divide control enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT          - invalid input parameter
 * Note:
 *      (1) This function only works when SEPARATE_KNOWN_MULTI and SEPARATE_FLOOD are both enabled.
 *           In that case, if the division function is enabled, separated DLF or broadcast packet would choose trunk member ports with
 *           largest index to forward; separated known multicast packets would choose second largest index to forward.
 *      (2) This function only works when at least 3 trunk member ports link up.
 */
extern int32
dal_mango_trunk_trafficSeparateDivision_set(uint32 unit, rtk_enable_t enable);

/* Function Name:
 *      dal_mango_trunk_port_get
 * Description:
 *      Get the members of the trunk id from the specified device.
 * Input:
 *      unit                    - unit id
 *      trk_gid                 - trunk group id
 * Output:
 *      pTrunk_member_portmask - pointer buffer of trunk member ports
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_LA_TRUNK_ID  - invalid trunk ID
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 */
extern int32
dal_mango_trunk_port_get(uint32 unit, uint32 trk_gid, rtk_portmask_t *pTrunk_member_portmask);

/* Function Name:
 *      dal_mango_trunk_port_set
 * Description:
 *      Set the members of the trunk id to the specified device.
 * Input:
 *      unit                    - unit id
 *      trk_gid                 - trunk group id
 *      pTrunk_member_portmask - trunk member ports
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_LA_TRUNK_ID       - invalid trunk ID
 *      RT_ERR_NULL_POINTER      - null pointer
 *      RT_ERR_LA_MEMBER_OVERLAP - the specified port mask is overlapped with other group
 *      RT_ERR_LA_PORTMASK       - error port mask
 *      RT_ERR_LA_PORTNUM_NORMAL - it can only aggregate at most eight ports when 802.1ad normal mode
 * Note:
 *      None.
 */
extern int32
dal_mango_trunk_port_set(uint32 unit, uint32 trk_gid, rtk_portmask_t *pTrunk_member_portmask);

extern int32
_dal_mango_trunk_localPort_get (uint32 unit, rtk_trk_t trk_gid, rtk_portmask_t *pTrk_local_ports);

/* Function Name:
 *      dal_mango_trunk_localPort_get
 * Description:
 *      Get the local member ports of a trunk id from the specified device.
 * Input:
 *      unit                   - unit id
 *      trk_gid               - trunk group id that contains local trunk member ports
 * Output:
 *      pTrk_local_ports        - pointer buffer of local trunk member ports
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_LA_TRUNK_ID  - invalid trunk ID
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) The valid range of trk_gid is 0~25 in 9310 stand-alone mode, 0~127 in 9310 stacking mode.
 *      (2) The local trunk member portmask would be used for source port filtering, and non-unicast packet forwarding.
 *      (3) The local portmask are local members of the device. For example, if trunk 100 contains member ports
 *         (dev 0, port 1), (dev 0, port 2) and (dev 3, port1). Local portmask of trunk 100 of device 0 is port 1 and port 2.
 */
extern int32
dal_mango_trunk_localPort_get (uint32 unit, rtk_trk_t trk_gid, rtk_portmask_t *pTrk_local_ports);

extern int32
_dal_mango_trunk_localPort_set (uint32 unit, rtk_trk_t trk_gid, rtk_portmask_t *pTrk_local_ports);

/* Function Name:
 *      dal_mango_trunk_localPort _set
 * Description:
 *      Set the local member ports of a trunk id from the specified device.
 * Input:
 *      unit                   - unit id
 *      trk_gid               - trunk group id that contains local trunk member ports
 *      pTrk_local_ports         - local trunk member ports
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_LA_TRUNK_ID  - invalid trunk ID
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_LA_PORTMASK       - error port mask
 * Note:
 *      (1) The valid range of trk_gid is 0~25 in 9310 stand-alone mode, 0~127 in 9310 stacking mode.
 *      (2) The local trunk member portmask would be used for source port filtering, and non-unicast packet forwarding.
 *      (3) For 9310, the local portmask are trunk member of the device. For example, if trunk 100 contains member ports
 *         (dev 0, port 1), (dev 0, port 2) and (dev 3, port1). Local portmask of trunk 100 of device 0 is port 1 and port 2.
 */
extern int32
dal_mango_trunk_localPort_set (uint32 unit, rtk_trk_t trk_gid, rtk_portmask_t *pTrk_local_ports);

/* Function Name:
 *      dal_mango_trunk_egrPort_get
 * Description:
 *      Get the trunk egress candidate ports from the specified device.
 * Input:
 *      unit                   - unit id
 *      trk_gid                - trunk group id
 * Output:
 *      pTrk_egr_ports       - pointer buffer of trunk egress candidate ports
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_LA_TRUNK_ID  - invalid trunk ID
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) The valid range of trk_gid is 0~25 in 9310 stand-alone mode, 0~127 in 9310 stacking mode.
 *      (2) The traffic sent to trunk group would be hashed and an outgoing port would be picked from egress candidate ports.
 *      (3) Separated traffic or traffic ignoring trunk hash would not follow egress candidate ports to forward.
 */
extern int32
dal_mango_trunk_egrPort_get(uint32 unit, rtk_trk_t trk_gid, rtk_trk_egrPort_t *pTrk_egr_ports);

extern int32
_dal_mango_trunk_egrPort_set(uint32 unit, rtk_trk_t trk_gid, rtk_trk_egrPort_t *pTrk_egr_ports);

/* Function Name:
 *      dal_mango_trunk_egrPort_set
 * Description:
 *      Set the trunk egress candidate ports to the specified device.
 * Input:
 *      unit                   - unit id
 *      trk_gid                - trunk group id
 *      pTrk_egr_ports         - trunk egress candidate ports
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_LA_TRUNK_ID  - invalid trunk ID
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) The valid range of trk_gid is 0~25 in 9310 stand-alone mode, 0~127 in 9310 stacking mode.
 *      (2) The traffic sent to trunk group would be hashed and an outgoing port would be picked from egress candidate ports.
 *      (3) Separated traffic or traffic ignoring trunk hash would not follow egress candidate ports to forward.
 */
extern int32
dal_mango_trunk_egrPort_set(uint32 unit, rtk_trk_t trk_gid, rtk_trk_egrPort_t *pTrk_egr_ports);

/* Function Name:
 *      dal_mango_trunk_tunnelHashSrc_get
 * Description:
 *      Get the trunk hash option for tunnel packets to decide hashing by outer or inner header from the specified device.
 * Input:
 *      unit                   - unit id
 * Output:
 *      pTunnelHashSrc      - pointer buffer of option that trunk hash based on outer or inner layer for tunnel packets
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) The option would not be applied on tunnel start and tunnel terminate packets (both of them would hash by inner header).
 */
extern int32
dal_mango_trunk_tunnelHashSrc_get(uint32 unit, rtk_trunk_tunnelHashSrc_t *pTunnelHashSrc);

/* Function Name:
 *      dal_mango_trunk_tunnelHashSrc_set
 * Description:
 *      Get the trunk hash option for tunnel packets to decide hashing by outer or inner header from the specified device.
 * Input:
 *      unit                   - unit id
 *      trk_gid               - trunk group id
 *      tunnelHashSrc         - option that trunk hash based on outer or inner layer for tunnel packets
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_LA_TRUNK_ID  - invalid trunk ID
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) The option would not be applied on tunnel start and tunnel terminate packets (both of them would hash by inner header).
*/
extern int32
dal_mango_trunk_tunnelHashSrc_set(uint32 unit, rtk_trunk_tunnelHashSrc_t tunnelHashSrc);

extern int32
_dal_mango_trunk_stkTrkPort_get(uint32 unit, rtk_stk_trk_t stk_trk_gid, rtk_portmask_t *pStkPorts);

/* Function Name:
 *      dal_mango_trunk_stkTrkPort_get
 * Description:
 *      Get the stacking portmask of designated trunk group from the specified device.
 * Input:
 *      unit          - unit id
 *      stk_trk_gid     - stacking trunk group id
 * Output:
 *      pStkPorts - pointer buffer of stacking portmask of trunk group
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_LA_STACK_TRUNK_ID  - invalid stacking trunk ID
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) The valid range of stk_trk_gid is from 0 ~ 1.
 *      (2) The stacking portmask indicates if stacking ports from port ID 0 to 55
 *           belong to the trunk group.
 */
extern int32
dal_mango_trunk_stkTrkPort_get(uint32 unit, rtk_stk_trk_t stk_trk_gid, rtk_portmask_t *pStkPorts);

extern int32
_dal_mango_trunk_stkTrkPort_set(uint32 unit, rtk_stk_trk_t stk_trk_gid, rtk_portmask_t *pStkPorts);

/* Function Name:
 *      dal_mango_trunk_stkTrkPort_set
 * Description:
 *      Get the stacking portmask of designated trunk group from the specified device.
 * Input:
 *      unit          - unit id
 *      stk_trk_gid     - stacking trunk group id
 *      pStkPorts  - pointer to stacking portmask of trunk group
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_LA_STACK_TRUNK_ID  - invalid stacking trunk ID
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) The valid range of stk_trk_gid is from 0 ~ 1.
 *      (2) The stacking portmask indicates if stacking ports from port ID 0 to 55
 *           belong to the trunk group.
 */
extern int32
dal_mango_trunk_stkTrkPort_set(uint32 unit, rtk_stk_trk_t stk_trk_gid, rtk_portmask_t *pStkPorts);

/* Function Name:
 *      dal_mango_trunk_stkTrkHash_get
 * Description:
 *      Get the option that if stacking trunk would follow the hash result in stacking header or re-calculate hash by its own distribution algorithm ID
 * Input:
 *      unit      - unit id
 * Output:
 *      pStkTrkHash - pointer buffer of the option that if stacking trunk would follow the hash result in stacking header or re-calculate hash by its own distribution algorithm ID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) If STACK_TRK_HASH_KEEP is selected, the hash result in stacking header would be applied to select stacking trunk outgoing port.
 *      (2) If STACK_TRK_HASH_RECALCULATE is selected, use rtk_trunk_stkDistributionAlgorithmBind_set() to set the distribution algorithm ID.
 *           The recalculated hash result only decides the stacking trunk outgoing port and would not alter the hash result in stacking header.
 */
extern int32
dal_mango_trunk_stkTrkHash_get(uint32 unit, rtk_trunk_stkTrkHash_t *pStkTrkHash);

/* Function Name:
 *      dal_mango_trunk_stkTrkHash_set
 * Description:
 *      Set the option that if stacking trunk would follow the hash result in stacking header or re-calculate hash by its own hash mask index
 * Input:
 *      unit     - unit id
 *      stkTrkHash - the option that if stacking trunk would follow the hash result in stacking header or re-calculate hash by its own hash mask index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID     - invalid unit id
 *      RT_ERR_NOT_INIT    - The module is not initial
 *      RT_ERR_INPUT          - invalid input parameter
 * Note:
 *      (1) If STACK_TRK_HASH_KEEP is selected, the hash result in stacking header would be applied to select stacking trunk outgoing port.
 *      (2) If STACK_TRK_HASH_RECALCULATE is selected, use rtk_trunk_stkDistributionAlgorithmBind_set() to set the distribution algorithm ID.
 *           The recalculated hash result only decides the stacking trunk outgoing port and would not alter the hash result in stacking header.
 */
extern int32
dal_mango_trunk_stkTrkHash_set(uint32 unit, rtk_trunk_stkTrkHash_t stkTrkHash);

/* Function Name:
 *      dal_mango_trunk_stkDistributionAlgorithmTypeBind_get
 * Description:
 *      Get the distribution algorithm ID binding for a packe type of a stacking trunk group from the specified device.
 * Input:
 *      unit      - unit id
 *      stk_trk_gid     - trunk group id of stacking port
 *      type     - bind packet type
 * Output:
 *      pAlgo_idx - pointer buffer of the distribution algorithm ID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_LA_STACK_TRUNK_ID  - invalid stacking port trunk ID
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) The valid range fo stk_trk_gid is from 0 ~ 1.
 *      (2) The valid range of algo_idx is 0~1
 */
extern int32
dal_mango_trunk_stkDistributionAlgorithmTypeBind_get(uint32 unit, rtk_stk_trk_t stk_trk_gid, rtk_trunk_bindType_t type, uint32 *pAlgo_idx);

/* Function Name:
 *      dal_mango_trunk_stkDistributionAlgorithmTypeBind_set
 * Description:
 *      Set the distribution algorithm ID binding for a packet type of a stacking trunk group to the specified device.
 * Input:
 *      unit     - unit id
 *      stk_trk_gid   - trunk group id of stacking port
 *      type - bind packet type
 *      algo_idx - index the distribution algorithm
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID     - invalid unit id
 *      RT_ERR_NOT_INIT    - The module is not initial
 *      RT_ERR_LA_STACK_TRUNK_ID  - invalid stacking port trunk ID
 *      RT_ERR_LA_ALGO_ID  - invalid trunk algorithm ID
 * Note:
 *      (1) The valid range fo stk_trk_gid is from 0 ~ 1.
 *      (2) The valid range of algo_idx is 0~1
 */
extern int32
dal_mango_trunk_stkDistributionAlgorithmTypeBind_set(uint32 unit, rtk_stk_trk_t stk_trk_gid, rtk_trunk_bindType_t type, uint32 algo_idx);

/* Function Name:
 *      dal_mango_trunk_localFirst_get
 * Description:
 *      Get the local-first load-balacing enable status from the specified device.
 * Input:
 *      unit          - unit id
 * Output:
 *      pLocalFirst - pointer to local-first load-balancing enable status
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
dal_mango_trunk_localFirst_get(uint32 unit, rtk_enable_t *pLocalFirst);

/* Function Name:
 *      dal_mango_trunk_localFirst_set
 * Description:
 *      Set the local-first load-balacing enable status to the specified device.
 * Input:
 *      unit        - unit id
 *      localFirst - local-first load-balancing enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID     - invalid unit id
 *      RT_ERR_NOT_INIT    - The module is not initial
 *      RT_ERR_INPUT       - Invalid input parameter
 * Note:
 *      Local first load balancing only works in stacking mode
 */
extern int32
dal_mango_trunk_localFirst_set(uint32 unit, rtk_enable_t localFirst);

/* Function Name:
 *      dal_mango_trunk_localFirstFailOver_get
 * Description:
 *      Get the local-first load balacing congest and link-fail avoidance enable status from the specified device.
 * Input:
 *      unit          - unit id
 * Output:
 *      pCongstAvoid - pointer to congest avoidance enable status
 *      pLinkFailAvoid - pointer to link fail avoidance enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      the failover funtion only works when local-first load balancing is enabled
 */
extern int32
dal_mango_trunk_localFirstFailOver_get(uint32 unit, rtk_enable_t *pCongstAvoid, rtk_enable_t *pLinkFailAvoid);

/* Function Name:
 *      dal_mango_trunk_localFirstFailOver_set
 * Description:
 *      Set the local-first load balacing congest and link-fail avoidance enable status to the specified device.
 * Input:
 *      unit          - unit id
 *      congstAvoid - congest avoidance enable status
 *      linkFailAvoid - link fail avoidance enable status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      the failover funtion only works when local-first load balancing is enabled
 */
extern int32
dal_mango_trunk_localFirstFailOver_set(uint32 unit, rtk_enable_t congstAvoid, rtk_enable_t linkFailAvoid);

/* Function Name:
 *      dal_mango_trunk_srcPortMap_get
 * Description:
 *      Get the info about whether Device+Port belongs to some trunk and if yes, get its trunk ID.
 * Input:
 *      unit                   - unit id
 *      devPort             - device ID and port
 * Output:
 *      pIsTrkMbr - pointer to get trunk or not
 *      pTrk_gid   - pointer to get trunk id.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *
 */
extern int32
dal_mango_trunk_srcPortMap_get(uint32 unit,  rtk_dev_port_t devPort, uint32 *pIsTrkMbr, rtk_trk_t *pTrk_gid);

extern int32
_dal_mango_trunk_srcPortMap_set(uint32 unit, rtk_dev_port_t devPort, uint32 isTrkMbr, rtk_trk_t trk_gid);

/* Function Name:
 *      dal_mango_trunk_srcPortMap_set
 * Description:
 *      Set the info about whether Device+Port belongs to some trunk and if yes, set its trunk ID.
 * Input:
 *      unit                   - unit id
 *      devPort             - device ID and port
 *      isTrkMbr            - trunk or not
 *      trk_gid              -  trunk id.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *
 */
extern int32
dal_mango_trunk_srcPortMap_set(uint32 unit, rtk_dev_port_t devPort, uint32 isTrkMbr, rtk_trk_t trk_gid);

#endif /* __DAL_MANGO_TRUNK_H__ */
