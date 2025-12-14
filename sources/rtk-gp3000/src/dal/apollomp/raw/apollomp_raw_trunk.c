/*
 * Copyright (C) 2011 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * Purpose : switch asic-level trunk API
 * Feature : trunk related functions
 *
 */

#include <dal/apollomp/raw/apollomp_raw_trunk.h>


/* Function Name:
 *      apollomp_raw_trunk_memberPort_set
 * Description:
 *      Set Trunk member ports
 * Input:
 *      pPortmask 		- Trunk Port Mask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32 apollomp_raw_trunk_memberPort_set(rtk_portmask_t *pPortmask)
{
    int32 ret;
    uint32 regData;
    rtk_port_t port;

    RT_PARAM_CHK(pPortmask == NULL, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HAL_IS_PORTMASK_VALID((*pPortmask)), RT_ERR_PORT_MASK);
    RT_PARAM_CHK(RTK_PORTMASK_IS_PORT_SET((*pPortmask), HAL_GET_CPU_PORT()), RT_ERR_PORT_MASK);
    RT_PARAM_CHK(RTK_PORTMASK_IS_PORT_SET((*pPortmask), HAL_GET_RGMII_PORT()), RT_ERR_PORT_MASK);

    HAL_SCAN_ALL_PORT(port)
    {
        if(!HAL_IS_CPU_PORT(port) && !HAL_IS_RGMII_PORT(port) && !HAL_IS_PON_PORT(port))
        {
            if(RTK_PORTMASK_IS_PORT_SET((*pPortmask), port))
                regData = 1;
            else
                regData = 0;

            if ((ret = reg_array_field_write(APOLLOMP_PORT_TRUNK_GROUP_ENr, (uint32)port, REG_ARRAY_INDEX_NONE, APOLLOMP_ENf, &regData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_TRUNK|MOD_DAL), "");
                return ret;
            }
        }
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_trunk_memberPort_set */

/* Function Name:
 *      apollomp_raw_trunk_memberPort_get
 * Description:
 *      Set Trunk member ports
 * Input:
 *      None
 * Output:
 *      pPortmask 		- Trunk Port Mask
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32 apollomp_raw_trunk_memberPort_get(rtk_portmask_t *pPortmask)
{
    int32 ret;
    uint32 regData;
    rtk_port_t port;

    RT_PARAM_CHK(pPortmask == NULL, RT_ERR_NULL_POINTER);

    /* Clear Portmask */
    RTK_PORTMASK_RESET(*pPortmask);

    HAL_SCAN_ALL_PORT(port)
    {
        if(!HAL_IS_CPU_PORT(port) && !HAL_IS_RGMII_PORT(port) && !HAL_IS_PON_PORT(port))
        {
            if ((ret = reg_array_field_read(APOLLOMP_PORT_TRUNK_GROUP_ENr, (uint32)port, REG_ARRAY_INDEX_NONE, APOLLOMP_ENf, &regData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_TRUNK|MOD_DAL), "");
                return ret;
            }

            if(1 == regData)
                RTK_PORTMASK_PORT_SET((*pPortmask), port);
        }
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_trunk_memberPort_get */

/* Function Name:
 *      apollomp_raw_trunk_hashAlgorithm_set
 * Description:
 *      Set Trunk Hash Algorithm
 * Input:
 *      hashsel         - The Hash Algorithm
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_LA_HASHMASK
 * Note:
 *      7 bits mask for link aggregation hash algorithm selection
 *    	0b0000001: SPA (Source Port)
 *    	0b0000010: SMAC
 *    	0b0000100: DMAC
 *    	0b0001000: SIP
 *    	0b0010000: DIP
 *    	0b0100000: TCP/UDP Source Port
 *    	0b1000000: TCP/UDP Destination Port
 */
int32 apollomp_raw_trunk_hashAlgorithm_set(uint32 hashsel)
{
    int32 ret;
    uint32 hash_idx;
    uint32 regData;
    uint32 hash_mask[APOLLOMP_RAW_TRUNK_ALGORITHM_NUM] = {
        APOLLOMP_RAW_TRUNK_HASH_SOURCE_PORT,
        APOLLOMP_RAW_TRUNK_HASH_SOURCE_MAC,
        APOLLOMP_RAW_TRUNK_HASH_DEST_MAC,
        APOLLOMP_RAW_TRUNK_HASH_SOURCE_IP,
        APOLLOMP_RAW_TRUNK_HASH_DEST_IP,
        APOLLOMP_RAW_TRUNK_HASH_SOURCE_L4PORT,
        APOLLOMP_RAW_TRUNK_HASH_DEST_L4PORT
    };

    uint32 raw_mask[APOLLOMP_RAW_TRUNK_ALGORITHM_NUM] = {
        APOLLOMP_SPA_HASHf,
        APOLLOMP_SMAC_HASHf,
        APOLLOMP_DMAC_HASHf,
        APOLLOMP_SIP_HASHf,
        APOLLOMP_DIP_HASHf,
        APOLLOMP_SPORT_HASHf,
        APOLLOMP_DPORT_HASHf
    };

    RT_PARAM_CHK(hashsel > APOLLOMP_RAW_TRUNK_HASH_ALL, RT_ERR_LA_HASHMASK);

    for(hash_idx = 0; hash_idx < APOLLOMP_RAW_TRUNK_ALGORITHM_NUM; hash_idx++)
    {
        if(hashsel & hash_mask[hash_idx])
            regData = 1;
        else
            regData = 0;

        if ((ret = reg_field_write(APOLLOMP_PORT_TRUNK_CTRLr, raw_mask[hash_idx], &regData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_TRUNK|MOD_DAL), "");
            return ret;
        }
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_trunk_hashAlgorithm_set */

/* Function Name:
 *      apollomp_raw_trunk_hashAlgorithm_get
 * Description:
 *      Get Trunk Hash Algorithm
 * Input:
 *      None.
 * Output:
 *      pHashsel        - The Hash Algorithm
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      7 bits mask for link aggregation hash algorithm selection
 *    	0b0000001: SPA (Source Port)
 *    	0b0000010: SMAC
 *    	0b0000100: DMAC
 *    	0b0001000: SIP
 *    	0b0010000: DIP
 *    	0b0100000: TCP/UDP Source Port
 *    	0b1000000: TCP/UDP Destination Port
 */
int32 apollomp_raw_trunk_hashAlgorithm_get(uint32 *pHashsel)
{
    int32 ret;
    uint32 hash_idx;
    uint32 regData;
    uint32 hash_mask[APOLLOMP_RAW_TRUNK_ALGORITHM_NUM] = {
        APOLLOMP_RAW_TRUNK_HASH_SOURCE_PORT,
        APOLLOMP_RAW_TRUNK_HASH_SOURCE_MAC,
        APOLLOMP_RAW_TRUNK_HASH_DEST_MAC,
        APOLLOMP_RAW_TRUNK_HASH_SOURCE_IP,
        APOLLOMP_RAW_TRUNK_HASH_DEST_IP,
        APOLLOMP_RAW_TRUNK_HASH_SOURCE_L4PORT,
        APOLLOMP_RAW_TRUNK_HASH_DEST_L4PORT
    };

    uint32 raw_mask[APOLLOMP_RAW_TRUNK_ALGORITHM_NUM] = {
        APOLLOMP_SPA_HASHf,
        APOLLOMP_SMAC_HASHf,
        APOLLOMP_DMAC_HASHf,
        APOLLOMP_SIP_HASHf,
        APOLLOMP_DIP_HASHf,
        APOLLOMP_SPORT_HASHf,
        APOLLOMP_DPORT_HASHf
    };

    RT_PARAM_CHK(pHashsel == NULL, RT_ERR_NULL_POINTER);
    *pHashsel = 0;

    for(hash_idx = 0; hash_idx < APOLLOMP_RAW_TRUNK_ALGORITHM_NUM; hash_idx++)
    {
        if ((ret = reg_field_read(APOLLOMP_PORT_TRUNK_CTRLr, raw_mask[hash_idx], &regData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_TRUNK|MOD_DAL), "");
            return ret;
        }

        if(1 == regData)
            *pHashsel |= hash_mask[hash_idx];
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_trunk_hashAlgorithm_get */

/* Function Name:
 *      apollomp_raw_trunk_flood_set
 * Description:
 *      Set trunk unknown DA flood.
 *      When this function is enabled, all unknown (L2 lookup miss)
 *      packets will be forward to first port of trunk group.
 * Input:
 *      mode          - Trunk flood mode
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None.
 */
int32 apollomp_raw_trunk_flood_set(apollomp_raw_trunk_floodMode_t mode)
{
    int32 ret;

    RT_PARAM_CHK(mode >= APOLLOMP_RAW_TRUNK_FLOOD_MODE_END, RT_ERR_INPUT);

    if ((ret = reg_field_write(APOLLOMP_PORT_TRUNK_CTRLr, APOLLOMP_PORT_TRUNK_FLOODf, (uint32 *)&mode)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_TRUNK|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_trunk_flood_set */

/* Function Name:
 *      apollomp_raw_trunk_flood_get
 * Description:
 *      Get trunk unknown DA flood.
 * Input:
 *      None.
 * Output:
 *      pMode          - Trunk flood mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None.
 */
int32 apollomp_raw_trunk_flood_get(apollomp_raw_trunk_floodMode_t *pMode)
{
    int32 ret;
    uint32 regData;

    RT_PARAM_CHK(pMode == NULL, RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_PORT_TRUNK_CTRLr, APOLLOMP_PORT_TRUNK_FLOODf, &regData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_TRUNK|MOD_DAL), "");
        return ret;
    }

    *pMode = ( (1 == regData) ? APOLLOMP_RAW_TRUNK_FWD_TO_FIRST_PORT : APOLLOMP_RAW_TRUNK_NORMAL_FLOOD);
    return RT_ERR_OK;
} /* end of apollomp_raw_trunk_flood_get */

/* Function Name:
 *      apollomp_raw_trunk_mode_set
 * Description:
 *      Set trunk mode (1: dumb mode, 0:user mode)
 * Input:
 *      mode           - Trunk mode.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None.
 */
int32 apollomp_raw_trunk_mode_set(apollomp_raw_trunk_mode_t mode)
{
    int32 ret;

    RT_PARAM_CHK(mode >= APOLLOMP_RAW_TRUNK_MODE_END, RT_ERR_INPUT);

    if ((ret = reg_field_write(APOLLOMP_PORT_TRUNK_CTRLr, APOLLOMP_PORT_TRUNK_DUMBf, (uint32 *)&mode)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_TRUNK|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_trunk_mode_set */

/* Function Name:
 *      apollomp_raw_trunk_mode_get
 * Description:
 *      get trunk mode
 * Input:
 *      None.
 * Output:
 *      pMode           - Trunk mode.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None.
 */
int32 apollomp_raw_trunk_mode_get(apollomp_raw_trunk_mode_t *pMode)
{
    int32 ret;
    uint32 regData;

    RT_PARAM_CHK(pMode == NULL, RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_PORT_TRUNK_CTRLr, APOLLOMP_PORT_TRUNK_DUMBf, &regData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_TRUNK|MOD_DAL), "");
        return ret;
    }

    *pMode = ( (1 == regData) ? APOLLOMP_RAW_TRUNK_DUMB_MODE : APOLLOMP_RAW_TRUNK_USER_MODE);
    return RT_ERR_OK;
} /* end of apollomp_raw_trunk_mode_get */

/* Function Name:
 *      apollomp_raw_trunk_flowControl_set
 * Description:
 *      Set trunk flow control
 * Input:
 *      state           - Trunk flow control state
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None.
 */
int32 apollomp_raw_trunk_flowControl_set(rtk_enable_t state)
{
    int32 ret;

    RT_PARAM_CHK(state >= RTK_ENABLE_END, RT_ERR_INPUT);

    if ((ret = reg_field_write(APOLLOMP_PORT_TRUNK_CTRLr, APOLLOMP_EN_FLOWCTRL_TG0f, (uint32 *)&state)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_TRUNK|MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_field_write(APOLLOMP_TRUNK_DROP_CFGr, APOLLOMP_TRUNK_DROPf, (uint32 *)&state)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_TRUNK|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_trunk_flowControl_set */

/* Function Name:
 *      apollomp_raw_trunk_flowControl_get
 * Description:
 *      Get trunk flow control
 * Input:
 *      None.
 * Output:
 *      pState           - Trunk flow control state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None.
 */
int32 apollomp_raw_trunk_flowControl_get(rtk_enable_t *pState)
{
    int32 ret;
    uint32 regData;

    RT_PARAM_CHK(pState == NULL, RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_PORT_TRUNK_CTRLr, APOLLOMP_EN_FLOWCTRL_TG0f, &regData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_TRUNK|MOD_DAL), "");
        return ret;
    }

    *pState = ( (1 == regData) ? ENABLED : DISABLED);
    return RT_ERR_OK;
} /* end of apollomp_raw_trunk_flowControl_get */

/* Function Name:
 *      apollomp_raw_trunk_hashMapping_set
 * Description:
 *      Set trunk hash value mapping.
 * Input:
 *      hash_value          - Hash Value (0 ~ 15)
 *      port                - The port of the hash value.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_OUT_OF_RANGE
 *      RT_ERR_INPUT
 * Note:
 *      None.
 */
int32 apollomp_raw_trunk_hashMapping_set(uint32 hash_value, apollomp_raw_trunk_port_t port)
{
    int32 ret;

    RT_PARAM_CHK(hash_value > APOLLOMP_RAW_TRUNK_HASH_VALUE_MAX, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(port >= APOLLOMP_RAW_TRUNK_PORT_END, RT_ERR_OUT_OF_RANGE);

    if ((ret = reg_array_field_write(APOLLOMP_PORT_TRUNK_HASH_MAPPINGr, REG_ARRAY_INDEX_NONE, hash_value, APOLLOMP_HASHf, (uint32 *)&port)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_TRUNK|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_trunk_hashMapping_set */

/* Function Name:
 *      apollomp_raw_trunk_hashMapping_get
 * Description:
 *      Set trunk hash value mapping.
 * Input:
 *      hash_value          - Hash Value (0 ~ 15)
 * Output:
 *      pPort               - The port of the hash value.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_OUT_OF_RANGE
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None.
 */
int32 apollomp_raw_trunk_hashMapping_get(uint32 hash_value, apollomp_raw_trunk_port_t *pPort)
{
    int32 ret;

    RT_PARAM_CHK(hash_value > APOLLOMP_RAW_TRUNK_HASH_VALUE_MAX, RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK(pPort == NULL, RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(APOLLOMP_PORT_TRUNK_HASH_MAPPINGr, REG_ARRAY_INDEX_NONE, hash_value, APOLLOMP_HASHf, (uint32 *)pPort)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_TRUNK|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_raw_trunk_hashMapping_get */
