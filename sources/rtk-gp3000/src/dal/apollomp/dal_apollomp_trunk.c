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
#include <dal/apollomp/dal_apollomp.h>
#include <rtk/trunk.h>
#include <dal/apollomp/dal_apollomp_trunk.h>
#include <dal/apollomp/raw/apollomp_raw_trunk.h>
/*
 * Symbol Definition
 */
static uint32    trunk_init = INIT_NOT_COMPLETED;

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
int32
dal_apollomp_trunk_init(void)
{
    int32 ret;
    rtk_portmask_t empty_portmask;
    rtk_trunk_hashVal2Port_t hash_table;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_TRUNK),"%s",__FUNCTION__);

    /* parameter check */
    trunk_init = INIT_COMPLETED;

    if((ret = dal_apollomp_trunk_distributionAlgorithm_set(0, 0)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRUNK), "");
        trunk_init = INIT_NOT_COMPLETED;
        return ret;
    }

    RTK_PORTMASK_RESET(empty_portmask);
    if((ret = dal_apollomp_trunk_port_set(0, &empty_portmask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRUNK), "");
        trunk_init = INIT_NOT_COMPLETED;
        return ret;
    }

    osal_memset(&hash_table, 0x00, sizeof(rtk_trunk_hashVal2Port_t));
    if((ret = dal_apollomp_trunk_hashMappingTable_set(0, &hash_table)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRUNK), "");
        trunk_init = INIT_NOT_COMPLETED;
        return ret;
    }

    if((ret = dal_apollomp_trunk_mode_set(TRUNK_MODE_DUMB)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRUNK), "");
        trunk_init = INIT_NOT_COMPLETED;
        return ret;
    }

    if((ret = dal_apollomp_trunk_trafficSeparate_set(0, SEPARATE_NONE)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRUNK), "");
        trunk_init = INIT_NOT_COMPLETED;
        return ret;
    }

    if((ret = dal_apollomp_trunk_trafficPause_set(0, DISABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRUNK), "");
        trunk_init = INIT_NOT_COMPLETED;
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_trunk_init */

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
int32
dal_apollomp_trunk_distributionAlgorithm_get(uint32 trkGid, uint32 *pAlgoBitmask)
{
    int32  ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_TRUNK),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(trunk_init);

    /* parameter check */
    RT_PARAM_CHK((trkGid > HAL_TRUNK_GROUP_ID_MAX()), RT_ERR_LA_TRUNK_ID);
    RT_PARAM_CHK((NULL == pAlgoBitmask), RT_ERR_NULL_POINTER);

    if((ret = apollomp_raw_trunk_hashAlgorithm_get(pAlgoBitmask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRUNK), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_trunk_distributionAlgorithm_get */


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
int32
dal_apollomp_trunk_distributionAlgorithm_set(uint32 trkGid, uint32 algoBitmask)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_TRUNK),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(trunk_init);

    /* parameter check */
    RT_PARAM_CHK((trkGid > HAL_TRUNK_GROUP_ID_MAX()), RT_ERR_LA_TRUNK_ID);
    RT_PARAM_CHK((algoBitmask > TRUNK_DISTRIBUTION_ALGO_MASKALL), RT_ERR_INPUT);

    if((ret = apollomp_raw_trunk_hashAlgorithm_set(algoBitmask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRUNK), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_trunk_distributionAlgorithm_set */


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
int32
dal_apollomp_trunk_port_get(uint32 trkGid, rtk_portmask_t *pTrunkMemberPortmask)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_TRUNK),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(trunk_init);

    /* parameter check */
    RT_PARAM_CHK((trkGid > HAL_TRUNK_GROUP_ID_MAX()), RT_ERR_LA_TRUNK_ID);
    RT_PARAM_CHK((NULL == pTrunkMemberPortmask), RT_ERR_NULL_POINTER);

    if((ret = apollomp_raw_trunk_memberPort_get(pTrunkMemberPortmask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRUNK), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_trunk_port_get */


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
int32
dal_apollomp_trunk_port_set(uint32 trkGid, rtk_portmask_t *pTrunkMemberPortmask)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_TRUNK),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(trunk_init);

    /* parameter check */
    RT_PARAM_CHK((trkGid > HAL_TRUNK_GROUP_ID_MAX()), RT_ERR_LA_TRUNK_ID);
    RT_PARAM_CHK((NULL == pTrunkMemberPortmask), RT_ERR_NULL_POINTER);

    if(RTK_PORTMASK_IS_PORT_SET((*pTrunkMemberPortmask), HAL_GET_PON_PORT()))
        return RT_ERR_LA_PORTMASK;

    if(RTK_PORTMASK_IS_PORT_SET((*pTrunkMemberPortmask), HAL_GET_RGMII_PORT()))
        return RT_ERR_LA_PORTMASK;

    if(RTK_PORTMASK_IS_PORT_SET((*pTrunkMemberPortmask), HAL_GET_CPU_PORT()))
        return RT_ERR_LA_PORTMASK;

    if((ret = apollomp_raw_trunk_memberPort_set(pTrunkMemberPortmask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRUNK), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_trunk_port_set */

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
int32
dal_apollomp_trunk_hashMappingTable_get(
    uint32                   trk_gid,
    rtk_trunk_hashVal2Port_t *pHash2Port_array)
{
    int32   ret;
    uint32 hash;
    apollomp_raw_trunk_port_t trunk_port;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_TRUNK),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(trunk_init);

    /* parameter check */
    RT_PARAM_CHK((trk_gid > HAL_TRUNK_GROUP_ID_MAX()), RT_ERR_LA_TRUNK_ID);
    RT_PARAM_CHK((NULL == pHash2Port_array), RT_ERR_NULL_POINTER);

    for(hash = 0; hash < RTK_MAX_NUM_OF_TRUNK_HASH_VAL; hash++)
    {
        if((ret = apollomp_raw_trunk_hashMapping_get(hash, &trunk_port)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_TRUNK), "");
            return ret;
        }

        pHash2Port_array->value[hash] = (uint8)trunk_port;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_trunk_hashMappingTable_get */


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
int32
dal_apollomp_trunk_hashMappingTable_set(
    uint32                   trk_gid,
    rtk_trunk_hashVal2Port_t *pHash2Port_array)
{
    int32   ret;
    uint32 hash;
    apollomp_raw_trunk_port_t trunk_port;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_TRUNK),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(trunk_init);

    /* parameter check */
    RT_PARAM_CHK((trk_gid > HAL_TRUNK_GROUP_ID_MAX()), RT_ERR_LA_TRUNK_ID);
    RT_PARAM_CHK((NULL == pHash2Port_array), RT_ERR_NULL_POINTER);

    for(hash = 0; hash < RTK_MAX_NUM_OF_TRUNK_HASH_VAL; hash++)
    {
        if(pHash2Port_array->value[hash] >= APOLLOMP_RAW_TRUNK_PORT_END)
            return RT_ERR_PORT_ID;
    }

    for(hash = 0; hash < RTK_MAX_NUM_OF_TRUNK_HASH_VAL; hash++)
    {
        trunk_port = (apollomp_raw_trunk_port_t)pHash2Port_array->value[hash];
        if((ret = apollomp_raw_trunk_hashMapping_set(hash, trunk_port)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_TRUNK), "");
            return ret;
        }
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_trunk_hashMappingTable_set */

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
int32
dal_apollomp_trunk_mode_get(rtk_trunk_mode_t *pMode)
{
    int32   ret;
    apollomp_raw_trunk_mode_t raw_mode;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_TRUNK),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(trunk_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pMode), RT_ERR_NULL_POINTER);

    if((ret = apollomp_raw_trunk_mode_get(&raw_mode)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRUNK), "");
        return ret;
    }

    *pMode = ((APOLLOMP_RAW_TRUNK_USER_MODE == raw_mode) ? TRUNK_MODE_NORMAL : TRUNK_MODE_DUMB);
    return RT_ERR_OK;
} /* end of dal_apollomp_trunk_mode_get */


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
int32
dal_apollomp_trunk_mode_set(rtk_trunk_mode_t mode)
{
    int32   ret;
    apollomp_raw_trunk_mode_t raw_mode;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_TRUNK),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(trunk_init);

    /* parameter check */
    RT_PARAM_CHK((TRUNK_MODE_END <= mode), RT_ERR_INPUT);

    raw_mode = ((TRUNK_MODE_NORMAL == mode) ? APOLLOMP_RAW_TRUNK_USER_MODE : APOLLOMP_RAW_TRUNK_DUMB_MODE);
    if((ret = apollomp_raw_trunk_mode_set(raw_mode)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRUNK), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_trunk_mode_set */

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
int32
dal_apollomp_trunk_trafficSeparate_get(uint32 trk_gid, rtk_trunk_separateType_t *pSeparateType)
{
    int32   ret;
    apollomp_raw_trunk_floodMode_t mode;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_TRUNK),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(trunk_init);

    /* parameter check */
    RT_PARAM_CHK((trk_gid > HAL_TRUNK_GROUP_ID_MAX()), RT_ERR_LA_TRUNK_ID);
    RT_PARAM_CHK((NULL == pSeparateType), RT_ERR_NULL_POINTER);

    if((ret = apollomp_raw_trunk_flood_get(&mode)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRUNK), "");
        return ret;
    }

    *pSeparateType = ((APOLLOMP_RAW_TRUNK_NORMAL_FLOOD == mode) ? SEPARATE_NONE : SEPARATE_KNOWN_MULTI_AND_FLOOD);
    return RT_ERR_OK;
} /* end of dal_apollomp_trunk_trafficSeparate_get */

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
int32
dal_apollomp_trunk_trafficSeparate_set(uint32 trk_gid, rtk_trunk_separateType_t separateType)
{
    int32   ret;
    apollomp_raw_trunk_floodMode_t mode;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_TRUNK),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(trunk_init);

    /* parameter check */
    RT_PARAM_CHK((trk_gid > HAL_TRUNK_GROUP_ID_MAX()), RT_ERR_LA_TRUNK_ID);
    RT_PARAM_CHK((SEPARATE_END <= separateType), RT_ERR_INPUT);

    mode = ((SEPARATE_NONE == separateType) ? APOLLOMP_RAW_TRUNK_NORMAL_FLOOD : APOLLOMP_RAW_TRUNK_FWD_TO_FIRST_PORT);
    if((ret = apollomp_raw_trunk_flood_set(mode)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRUNK), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_trunk_trafficSeparate_set */

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
int32
dal_apollomp_trunk_portQueueEmpty_get(rtk_portmask_t *pEmpty_portmask)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_TRUNK),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(trunk_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEmpty_portmask), RT_ERR_INPUT);

    if ((ret = reg_field_read(APOLLOMP_P_QUEUE_EMPTYr, APOLLOMP_EMPTYf, &(pEmpty_portmask->bits[0]))) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_trunk_portQueueEmpty_get */

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
int32
dal_apollomp_trunk_trafficPause_get(uint32 trk_gid, rtk_enable_t *pEnable)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_TRUNK),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(trunk_init);

    /* parameter check */
    RT_PARAM_CHK((trk_gid > HAL_TRUNK_GROUP_ID_MAX()), RT_ERR_LA_TRUNK_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if((ret = apollomp_raw_trunk_flowControl_get(pEnable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRUNK), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_trunk_trafficPause_get */

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
int32
dal_apollomp_trunk_trafficPause_set(uint32 trk_gid, rtk_enable_t enable)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_TRUNK),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(trunk_init);

    /* parameter check */
    RT_PARAM_CHK((trk_gid > HAL_TRUNK_GROUP_ID_MAX()), RT_ERR_LA_TRUNK_ID);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    if((ret = apollomp_raw_trunk_flowControl_set(enable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRUNK), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_trunk_trafficPause_set */

