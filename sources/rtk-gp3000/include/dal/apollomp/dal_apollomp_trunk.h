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
 * Purpose : Definition of TRUNK API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) User Configuration Trunk
 *
 */

#ifndef __DAL_APOLLOMP_TRUNK_H__
#define __DAL_APOLLOMP_TRUNK_H__


/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/port.h>


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

/* Module Name : TRUNK */

/* Function Name:
 *      dal_apollomp_trunk_init
 * Description:
 *      Initialize trunk module of the specified device.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize trunk module before calling any trunk APIs.
 */
extern int32
dal_apollomp_trunk_init(void);

/* Module Name    : Trunk                    */
/* Sub-module Name: User configuration trunk */

/* Function Name:
 *      dal_apollomp_trunk_distributionAlgorithm_get
 * Description:
 *      Get the distribution algorithm of the trunk group id from the specified device.
 * Input:
 *      trkGid       - trunk group id
 * Output:
 *      pAlgoBitmask - pointer buffer of bitmask of the distribution algorithm
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_LA_TRUNK_ID  - invalid trunk ID
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      You can use OR opertions in following bits to decide your algorithm.
 *      - TRUNK_DISTRIBUTION_ALGO_SPA_BIT        (source port)
 *      - TRUNK_DISTRIBUTION_ALGO_SMAC_BIT       (source mac)
 *      - TRUNK_DISTRIBUTION_ALGO_DMAC_BIT       (destination mac)
 *      - TRUNK_DISTRIBUTION_ALGO_SIP_BIT        (source ip)
 *      - TRUNK_DISTRIBUTION_ALGO_DIP_BIT        (destination ip)
 *      - TRUNK_DISTRIBUTION_ALGO_SRC_L4PORT_BIT (source layer4 port)
 *      - TRUNK_DISTRIBUTION_ALGO_DST_L4PORT_BIT (destination layer4 port)
 */
extern int32
dal_apollomp_trunk_distributionAlgorithm_get(uint32 trkGid, uint32 *pAlgoBitmask);


/* Function Name:
 *      dal_apollomp_trunk_distributionAlgorithm_set
 * Description:
 *      Set the distribution algorithm of the trunk group id from the specified device.
 * Input:
 *      trkGid       - trunk group id
 *      algoBitmask  - bitmask of the distribution algorithm
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_LA_TRUNK_ID - invalid trunk ID
 *      RT_ERR_LA_HASHMASK - invalid hash mask
 * Note:
 *      You can use OR opertions in following bits to decide your algorithm.
 *      - TRUNK_DISTRIBUTION_ALGO_SPA_BIT        (source port)
 *      - TRUNK_DISTRIBUTION_ALGO_SMAC_BIT       (source mac)
 *      - TRUNK_DISTRIBUTION_ALGO_DMAC_BIT       (destination mac)
 *      - TRUNK_DISTRIBUTION_ALGO_SIP_BIT        (source ip)
 *      - TRUNK_DISTRIBUTION_ALGO_DIP_BIT        (destination ip)
 *      - TRUNK_DISTRIBUTION_ALGO_SRC_L4PORT_BIT (source layer4 port)
 *      - TRUNK_DISTRIBUTION_ALGO_DST_L4PORT_BIT (destination layer4 port)
 */
extern int32
dal_apollomp_trunk_distributionAlgorithm_set(uint32 trkGid, uint32 algoBitmask);


/* Function Name:
 *      dal_apollomp_trunk_port_get
 * Description:
 *      Get the members of the trunk id from the specified device.
 * Input:
 *      trkGid                - trunk group id
 * Output:
 *      pTrunkMemberPortmask - pointer buffer of trunk member ports
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_LA_TRUNK_ID  - invalid trunk ID
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_apollomp_trunk_port_get(uint32 trkGid, rtk_portmask_t *pTrunkMemberPortmask);


/* Function Name:
 *      dal_apollomp_trunk_port_set
 * Description:
 *      Set the members of the trunk id to the specified device.
 * Input:
 *      trkGid               - trunk group id
 *      pTrunkMemberPortmask - trunk member ports
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_LA_TRUNK_ID       - invalid trunk ID
 *      RT_ERR_LA_MEMBER_OVERLAP - the specified port mask is overlapped with other group
 *      RT_ERR_LA_PORTNUM_DUMB   - it can only aggregate at most four ports when 802.1ad dumb mode
 *      RT_ERR_LA_PORTNUM_NORMAL - it can only aggregate at most eight ports when 802.1ad normal mode
 * Note:
 *      None
 */
extern int32
dal_apollomp_trunk_port_set(uint32 trkGid, rtk_portmask_t *pTrunkMemberPortmask);

/* Function Name:
 *      dal_apollomp_trunk_hashMappingTable_get
 * Description:
 *      Get hash value to port array in the trunk group id from the specified device.
 * Input:
 *      trk_gid          - trunk group id
 * Output:
 *      pHash2Port_array - pointer buffer of ports associate with the hash value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_LA_TRUNK_ID  - invalid trunk ID
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) The valid trk_gid is 0 in APOLLO
 */
extern int32
dal_apollomp_trunk_hashMappingTable_get(
    uint32                   trk_gid,
    rtk_trunk_hashVal2Port_t *pHash2Port_array);


/* Function Name:
 *      dal_apollomp_trunk_hashMappingTable_set
 * Description:
 *      Set hash value to port array in the trunk group id from the specified device.
 * Input:
 *      trk_gid          - trunk group id
 *      pHash2Port_array - ports associate with the hash value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID            - invalid unit id
 *      RT_ERR_LA_TRUNK_ID        - invalid trunk ID
 *      RT_ERR_NULL_POINTER       - input parameter may be null pointer
 *      RT_ERR_PORT_ID            - input error port id
 * Note:
 *      (1) The valid trk_gid is 0 in APOLLO
 */
extern int32
dal_apollomp_trunk_hashMappingTable_set(
    uint32                   trk_gid,
    rtk_trunk_hashVal2Port_t *pHash2Port_array);

/* Function Name:
 *      dal_apollomp_trunk_mode_get
 * Description:
 *      Get the trunk mode from the specified device.
 * Input:
 *      None
 * Output:
 *      pMode - pointer buffer of trunk mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The enum of the trunk mode as following
 *      - TRUNK_MODE_NORMAL
 *      - TRUNK_MODE_DUMB
 */
extern int32
dal_apollomp_trunk_mode_get(rtk_trunk_mode_t *pMode);


/* Function Name:
 *      dal_apollomp_trunk_mode_set
 * Description:
 *      Set the trunk mode to the specified device.
 * Input:
 *      mode - trunk mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 *      RT_ERR_INPUT   - invalid input parameter
 * Note:
 *      The enum of the trunk mode as following
 *      - TRUNK_MODE_NORMAL
 *      - TRUNK_MODE_DUMB
 */
extern int32
dal_apollomp_trunk_mode_set(rtk_trunk_mode_t mode);

/* Function Name:
 *      dal_apollomp_trunk_trafficSeparate_get
 * Description:
 *      Get the traffic separation setting of a trunk group from the specified device.
 * Input:
 *      trk_gid        - trunk group id
 * Output:
 *      pSeparateType   - pointer separated traffic type
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_LA_TRUNK_ID  - invalid trunk ID
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      SEPARATE_NONE: disable traffic separation
 *      SEPARATE_KNOWN_MULTI_AND_FLOOD: trunk MSB link up port is dedicated to TX known multicast and flooding (L2 lookup miss) traffic
 */
extern int32
dal_apollomp_trunk_trafficSeparate_get(uint32 trk_gid, rtk_trunk_separateType_t *pSeparateType);

/* Function Name:
 *      dal_apollomp_trunk_trafficSeparate_set
 * Description:
 *      Set the traffic separation setting of a trunk group from the specified device.
 * Input:
 *      trk_gid      - trunk group id
 *      separateType     - traffic separation setting
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID     - invalid unit id
 *      RT_ERR_LA_TRUNK_ID - invalid trunk ID
 *      RT_ERR_LA_HASHMASK - invalid hash mask
 * Note:
 *      SEPARATE_NONE: disable traffic separation
 *      SEPARATE_KNOWN_MULTI_AND_FLOOD: trunk MSB link up port is dedicated to TX known multicast and flooding (L2 lookup miss) traffic
 */
extern int32
dal_apollomp_trunk_trafficSeparate_set(uint32 trk_gid, rtk_trunk_separateType_t separateType);

/* Function Name:
 *      dal_apollomp_trunk_portQueueEmpty_get
 * Description:
 *      Get the port mask which all queues are empty.
 * Input:
 *      None.
 * Output:
 *      pEmpty_portmask   - pointer empty port mask
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None.
 */
extern int32
dal_apollomp_trunk_portQueueEmpty_get(rtk_portmask_t *pEmpty_portmask);

/* Function Name:
 *      dal_apollomp_trunk_trafficPause_get
 * Description:
 *      Get the traffic pause setting of a trunk group.
 * Input:
 *      trk_gid        - trunk group id
 * Output:
 *      pEnable        - pointer of traffic pause state.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_LA_TRUNK_ID  - invalid trunk ID
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None.
 */
extern int32
dal_apollomp_trunk_trafficPause_get(uint32 trk_gid, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_apollomp_trunk_trafficPause_set
 * Description:
 *      Set the traffic separation setting of a trunk group from the specified device.
 * Input:
 *      trk_gid      - trunk group id
 *      enable       - traffic pause state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_LA_TRUNK_ID - invalid trunk ID
 * Note:
 *      None.
 */
extern int32
dal_apollomp_trunk_trafficPause_set(uint32 trk_gid, rtk_enable_t enable);

#endif /* __DAL_APOLLOMP_TRUNK_H__ */
