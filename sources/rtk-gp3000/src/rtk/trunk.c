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



/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/port.h>
#include <rtk/init.h>
#include <rtk/default.h>
#include <rtk/trunk.h>
#include <dal/dal_mgmt.h>

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
 *      rtk_trunk_init
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
int32
rtk_trunk_init(void)
{
    int32   ret;

    if (NULL == RT_MAPPER->trunk_init)
        return RT_ERR_OK;

    RTK_API_LOCK();
    ret = RT_MAPPER->trunk_init();
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_trunk_init */

/* Module Name    : Trunk                    */
/* Sub-module Name: User configuration trunk */

/* Function Name:
 *      rtk_trunk_distributionAlgorithm_get
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
int32
rtk_trunk_distributionAlgorithm_get(uint32 trkGid, uint32 *pAlgoBitmask)
{
    int32   ret;

    if (NULL == RT_MAPPER->trunk_distributionAlgorithm_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->trunk_distributionAlgorithm_get( trkGid, pAlgoBitmask);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_trunk_distributionAlgorithm_get */


/* Function Name:
 *      rtk_trunk_distributionAlgorithm_set
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
int32
rtk_trunk_distributionAlgorithm_set(uint32 trkGid, uint32 algoBitmask)
{
    int32   ret;

    if (NULL == RT_MAPPER->trunk_distributionAlgorithm_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->trunk_distributionAlgorithm_set( trkGid, algoBitmask);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_trunk_distributionAlgorithm_set */


/* Function Name:
 *      rtk_trunk_port_get
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
int32
rtk_trunk_port_get(uint32 trkGid, rtk_portmask_t *pTrunkMemberPortmask)
{
    int32   ret;

    if (NULL == RT_MAPPER->trunk_port_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->trunk_port_get( trkGid, pTrunkMemberPortmask);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_trunk_port_get */


/* Function Name:
 *      rtk_trunk_port_set
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
int32
rtk_trunk_port_set(uint32 trkGid, rtk_portmask_t *pTrunkMemberPortmask)
{
    int32   ret;

    if (NULL == RT_MAPPER->trunk_port_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->trunk_port_set( trkGid, pTrunkMemberPortmask);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_trunk_port_set */

/* Function Name:
 *      rtk_trunk_hashMappingTable_get
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
int32
rtk_trunk_hashMappingTable_get(
    uint32                   trk_gid,
    rtk_trunk_hashVal2Port_t *pHash2Port_array)
{
    int32   ret;

    if (NULL == RT_MAPPER->trunk_hashMappingTable_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->trunk_hashMappingTable_get( trk_gid, pHash2Port_array);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_trunk_hashMappingTable_get */


/* Function Name:
 *      rtk_trunk_hashMappingTable_set
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
 *      RT_ERR_LA_TRUNK_NOT_EXIST - the trunk doesn't exist
 *      RT_ERR_LA_NOT_MEMBER_PORT - the port is not a member port of the trunk
 *      RT_ERR_LA_CPUPORT         - CPU port can not be aggregated port
 * Note:
 *      (1) The valid trk_gid is 0 in APOLLO
 */
int32
rtk_trunk_hashMappingTable_set(
    uint32                   trk_gid,
    rtk_trunk_hashVal2Port_t *pHash2Port_array)
{
    int32   ret;

    if (NULL == RT_MAPPER->trunk_hashMappingTable_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->trunk_hashMappingTable_set( trk_gid, pHash2Port_array);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_trunk_hashMappingTable_set */

/* Function Name:
 *      rtk_trunk_mode_get
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
int32
rtk_trunk_mode_get(rtk_trunk_mode_t *pMode)
{
    int32   ret;

    if (NULL == RT_MAPPER->trunk_mode_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->trunk_mode_get( pMode);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_trunk_mode_get */


/* Function Name:
 *      rtk_trunk_mode_set
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
int32
rtk_trunk_mode_set(rtk_trunk_mode_t mode)
{
    int32   ret;

    if (NULL == RT_MAPPER->trunk_mode_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->trunk_mode_set( mode);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_trunk_mode_set */

/* Function Name:
 *      rtk_trunk_trafficSeparate_get
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
int32
rtk_trunk_trafficSeparate_get(uint32 trk_gid, rtk_trunk_separateType_t *pSeparateType)
{
    int32   ret;

    if (NULL == RT_MAPPER->trunk_trafficSeparate_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->trunk_trafficSeparate_get( trk_gid, pSeparateType);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_trunk_trafficSeparate_get */

/* Function Name:
 *      rtk_trunk_trafficSeparate_set
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
int32
rtk_trunk_trafficSeparate_set(uint32 trk_gid, rtk_trunk_separateType_t separateType)
{
    int32   ret;

    if (NULL == RT_MAPPER->trunk_trafficSeparate_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->trunk_trafficSeparate_set( trk_gid, separateType);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_trunk_trafficSeparate_set */

/* Function Name:
 *      rtk_trunk_portQueueEmpty_get
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
int32
rtk_trunk_portQueueEmpty_get(rtk_portmask_t *pEmpty_portmask)
{
    int32   ret;

    if (NULL == RT_MAPPER->trunk_portQueueEmpty_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->trunk_portQueueEmpty_get( pEmpty_portmask);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_trunk_portQueueEmpty_get */

/* Function Name:
 *      rtk_trunk_trafficPause_get
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
int32
rtk_trunk_trafficPause_get(uint32 trk_gid, rtk_enable_t *pEnable)
{
    int32   ret;

    if (NULL == RT_MAPPER->trunk_trafficPause_get)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->trunk_trafficPause_get( trk_gid, pEnable);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_trunk_trafficPause_get */

/* Function Name:
 *      rtk_trunk_trafficPause_set
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
int32
rtk_trunk_trafficPause_set(uint32 trk_gid, rtk_enable_t enable)
{
    int32   ret;

    if (NULL == RT_MAPPER->trunk_trafficPause_set)
        return RT_ERR_DRIVER_NOT_FOUND;

    RTK_API_LOCK();
    ret = RT_MAPPER->trunk_trafficPause_set( trk_gid, enable);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_trunk_trafficPause_set */

